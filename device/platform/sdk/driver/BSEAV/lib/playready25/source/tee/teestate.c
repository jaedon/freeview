#include <teecommon.h>
#include <teeimpl.h>
#include <teeder.h>
#include <teeimplinternal.h>

#include <ctype.h>
#include <memory.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <drmblackboxtypes.h>

#define MAX_NUM_SESSIONS TEE_MAX_NUM_SECURE_STOPS
static TEE_SESSION TEE_SESSIONS[MAX_NUM_SESSIONS];

#define SECURE_STOP_VERSION 1

// mutex support
static pthread_mutex_t mutex;
static pthread_mutexattr_t mutex_attr;
static int mutex_init = 1;

#define LOCK_INIT if (mutex_init) { pthread_mutexattr_init(&mutex_attr); pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_RECURSIVE); pthread_mutex_init(&mutex, &mutex_attr); mutex_init = 0; }
#define LOCK { pthread_mutex_lock(&mutex); }
#define UNLOCK { pthread_mutex_unlock(&mutex); }
#define LOCK_DESTROY if (!mutex_init) { pthread_mutex_destroy(&mutex); pthread_mutexattr_destroy(&mutex_attr); mutex_init = 1; }

/*
 * Initialize TEE state with the persisted state data
 */
int _tee_init(const DRM_BYTE * const inbuf,
                    DRM_DWORD  const inlen)
{
    DBG_ENTER;
    LOCK_INIT;
    LOCK;
    int status = TEE_SUCCESS;

    CHECK(_tee_init_secure_stops(inbuf, inlen));

    Exit:
    UNLOCK;
    DBG_EXIT;
    return status;
}

int _tee_destroy()
{
    int status = TEE_SUCCESS;
    LOCK_DESTROY;
    return status;
}

/*********************************
 * Session & Secure Stop Helpers *
 *********************************/

// session is active if its internal session id is set
int _is_active_session(const TEE_SESSION * const s) {
    return s->id != UNINIT_TYPE_ID;
}

// uninitialize session by un-setting its internal session id
void _uninit_session(TEE_SESSION * const s) {
    s->id = UNINIT_TYPE_ID;
}

// secure stop is initialized if its version is set to non-0 value
int _is_init_secure_stop(const SECURE_STOP * const ss) {
    return ss->version != 0;
}

// initialize secure stop by setting its version and session ID.
int _init_secure_stop(SECURE_STOP * const ss, const unsigned char * const session_id) {
    ss->version = SECURE_STOP_VERSION;
    memcpy(ss->session_id, session_id, TEE_SESSION_ID_LEN);
    memset(ss->kid, 0, TEE_KEY_ID_LEN);
    ss->stopped = 0;
    return TEE_SUCCESS;
}

int _uninit_secure_stop(SECURE_STOP * const ss) {
    _tee_memset(ss, 0, sizeof(SECURE_STOP));
	return TEE_SUCCESS;
}

// return session by session id
TEE_SESSION * __tee_get_session(const unsigned char * const session_id) {
    TEE_SESSION * session = NULL;
    int i;
    TEE_SESSION * s = &TEE_SESSIONS[0];
    for (i = 0; i < MAX_NUM_SESSIONS; ++i, ++s) {
        if (_is_active_session(s) && (memcmp(s->ss.session_id, session_id, TEE_SESSION_ID_LEN) == 0)) {
            session = s;
            break;
        }
    }
    return session;
}


// return session by content KID
TEE_SESSION * __tee_get_session_by_kid(const unsigned char * const kid) {
    TEE_SESSION * session = NULL;
    int i;
    TEE_SESSION * s = &TEE_SESSIONS[0];
    for (i = 0; i < MAX_NUM_SESSIONS; ++i, ++s) {
        if (_is_active_session(s) && (memcmp(s->ss.kid, kid, TEE_KEY_ID_LEN) == 0)) {
            session = s;
            break;
        }
    }
    return session;
}

#ifdef NDEBUG
#define __print_secure_stop(x)
#else
#define __print_secure_stop(x) __print_secure_stop_func(__func__, x)
void __print_secure_stop_func(const char * const header, const SECURE_STOP * const ss) {
    char id_str[2*TEE_SESSION_ID_LEN + 1];
    _get_hex(ss->session_id, TEE_SESSION_ID_LEN, id_str);
    char kid_str[2*TEE_KEY_ID_LEN + 1];
    _get_hex(ss->kid, TEE_KEY_ID_LEN, kid_str);
    _print("****** %s: sid=%s, kid=%s, v=%d, t=(%lx %lx %lx), dur=%ld s=%d\n",
        header, id_str, kid_str, ss->version,
        ss->start_time, ss->update_time, ss->auth_time,
        ss->update_time - ss->start_time, ss->stopped);
}
#endif

/*********************
 * Session Lifecycle *
 *********************/

static int nextSessionIndex = 0;

/*
 * add session with assigned ID
 */
int _tee_add_session(const unsigned char * const session_id) {
    int status = TEE_SUCCESS;
    DBG_ENTER;
    LOCK;
    TEE_SESSION * s = &TEE_SESSIONS[nextSessionIndex % MAX_NUM_SESSIONS];
    if (_is_init_secure_stop(&s->ss)) {
        _dbg("WARNING: overwriting secure stop at index %d\n", nextSessionIndex % MAX_NUM_SESSIONS);
    }
    s->id = nextSessionIndex++;
    s->key_id = UNINIT_TYPE_ID;
    _init_secure_stop(&s->ss, session_id);

    //Exit:
    UNLOCK;
    DBG_EXIT;
    return status;
}

/*
 * cancel session with given ID
 */
int _tee_cancel_session(const unsigned char * const session_id) {
    int status = TEE_SUCCESS;
    DBG_ENTER;
    LOCK;
    TEE_SESSION * s = __tee_get_session(session_id);
    if (s == NULL) RETURN(TEE_ERR_FAIL);
    _uninit_session(s);
    _uninit_secure_stop(&s->ss);
    int index = s - &TEE_SESSIONS[0];
    if ((nextSessionIndex > 0) && (index == ((nextSessionIndex-1) % MAX_NUM_SESSIONS))) {
        nextSessionIndex--;
        _dbg("Cancelled Session Caused Session Index Decrement to %d\n", nextSessionIndex);
    }

    Exit:
    UNLOCK;
    DBG_EXIT;
    return status;
}

/*
 * bind session ID to content KID at license storage time
 */
int _tee_bind_license(const unsigned char  * const session_id,
                      const unsigned char  * const kid)
{
    int status = TEE_SUCCESS;
    DBG_ENTER;
    LOCK;
    TEE_SESSION * s = __tee_get_session(session_id);
    if (s == NULL) {
        _print_hex("ERROR: unknown session: id ", session_id, TEE_SESSION_ID_LEN);
        goto Exit;
    }

    memcpy(s->ss.kid, kid, TEE_KEY_ID_LEN);

    s->ss.start_time = s->ss.update_time = s->ss.auth_time = 0L;

    __print_secure_stop(&s->ss);

Exit:
    UNLOCK;
    DBG_EXIT;
    return status;
}

/*
 * bind session to content decryption key and content KID at playback start time
 */
int _tee_bind_session(const OEM_HAL_KEY_REGISTER_INDEX key_id,
                      const unsigned char  * const kid)
{
    int status = TEE_SUCCESS;
    DBG_ENTER;
    LOCK;
    TEE_SESSION * s = __tee_get_session_by_kid(kid);
    if (s == NULL) {
        _print_hex("ERROR: unknown session: kid ", kid, TEE_KEY_ID_LEN);
        goto Exit;
    }

    s->key_id = key_id; // binding happens here
    s->ss.start_time = s->ss.update_time = _get_drm_time();
    s->ss.auth_time = 0L;

    __print_secure_stop(&s->ss);

Exit:
    UNLOCK;
    DBG_EXIT;
    return status;
}

/*
 * remove active session by decryption key id
 */
int _tee_remove_session(const OEM_HAL_KEY_REGISTER_INDEX key_id) {
    int status = TEE_SUCCESS;
    //DBG_ENTER;

    LOCK;
    int i;
    int found = 0;
    TEE_SESSION * s = &TEE_SESSIONS[0];;
    for (i = 0; i < MAX_NUM_SESSIONS; ++i, ++s) {
        if (_is_active_session(s)) {
            if (s->key_id == key_id) {
                if (found) {
                    _print_hex("PANIC: duplicate session_id ", s->ss.session_id, TEE_SESSION_ID_LEN);
                    RETURN(TEE_ERR_FAIL);
                }
                _print_hex("session_id ", s->ss.session_id, TEE_SESSION_ID_LEN);
                _dbg("key_id: %ld\n", s->key_id);
                s->ss.update_time = _get_drm_time();
                s->ss.stopped = 1;
                _uninit_session(s);
                __print_secure_stop(&s->ss);
                found = 1;
            }
        }
    }
    if (!found) {
        status = TEE_ERR_FAIL;
        goto Exit;
    }

    Exit:
    UNLOCK;
    //DBG_EXIT;
    return status;
}

// remove all active sessions - normally, should never find any
int _tee_reset_sessions() {
    int status = TEE_SUCCESS;

    DBG_ENTER;
    LOCK;
    int i;
    TEE_SESSION * s = &TEE_SESSIONS[0];
    for (i = 0; i < MAX_NUM_SESSIONS; ++i, ++s) {
        if (s->id != UNINIT_TYPE_ID) {
            if (s->key_id != UNINIT_TYPE_ID) {
                _info("destroying session %d\n", s->id);
                _print_hex("destroying session", s->ss.session_id, TEE_SESSION_ID_LEN);
                s->key_id = UNINIT_TYPE_ID;
            }
        }
    }
    nextSessionIndex = 0;

    // Exit:
    UNLOCK;
    DBG_EXIT;
    return status;
}

/*
 * calculate the combined length of all encoded fields of secure stop TBS data
 */
int __tee_get_secure_stop_fields_enc_len(SECURE_STOP * ss)
{
    return    _der_get_field_enc_len(1) // version
            + _der_get_field_enc_len(TEE_SESSION_ID_LEN) // NONCE
            + _der_get_field_enc_len(TEE_KEY_ID_LEN) // KID
            + _der_get_int_field_enc_len(ss->start_time) // start_time
            + _der_get_int_field_enc_len(ss->update_time) // update_time
            + _der_get_int_field_enc_len(ss->auth_time) // sign_time
            + _der_get_field_enc_len(1) // stopped
            ;
}

/*
 * calculate the length of the entire to-be-signed portion of secure stop
 */
int __tee_get_secure_stop_enc_len(SECURE_STOP * ss)
{
    return _der_get_field_enc_len(__tee_get_secure_stop_fields_enc_len(ss)); // including DER SEQUENCE header
}

void __tee_write_secure_stop(SECURE_STOP   * const ss,
                             unsigned char * const outbuf,
                             int             const inlen)
{
    // get the length of all concatenated encodings of secure stop fields
    int fields_len = __tee_get_secure_stop_fields_enc_len(ss);

    // write DER SEQUENCE header
    unsigned char * p = DER_WriteHeader(outbuf, DER_SEQUENCE, fields_len);

    // write each field
    p = _der_write_int(p, 1L);
    p = _der_write_octets(p, ss->session_id, TEE_SESSION_ID_LEN);
    p = _der_write_octets(p, ss->kid, TEE_KEY_ID_LEN);
    p = _der_write_int(p, ss->start_time);
    p = _der_write_int(p, ss->update_time);
    p = _der_write_int(p, ss->auth_time);
    p = _der_write_int(p, ss->stopped);

    if ((p - outbuf) != inlen) {
        _error("Serialization Error: %d vs %d\n", inlen, (p - outbuf));
    }
}

/*************************
 * Secure Stop Lifecycle *
 *************************/

// initialize TEE_SESSIONS array
void __tee_clear_sessions() {
    _tee_memset(&TEE_SESSIONS, 0, sizeof(TEE_SESSIONS));
    int i;
    TEE_SESSION * s = NULL;
    s = &TEE_SESSIONS[0];
    for (i = 0; i < MAX_NUM_SESSIONS; ++i, ++s) {
        _uninit_session(s);
        _uninit_secure_stop(&s->ss);
    }
}

/*
 * Initialize outstanding secure stops from persisted data
 */
int _tee_init_secure_stops(const DRM_BYTE * const inbuf,
                                 DRM_DWORD  const inlen)
{
    int status = TEE_SUCCESS;
    DBG_ENTER;

    __tee_clear_sessions();

    if (inbuf != NULL) {
        if (inlen > 0) {
            int n = inlen / sizeof(SECURE_STOP);
            if (n > MAX_NUM_SESSIONS) {
                RETURN(TEE_ERR_FAIL);
            }
            if ((inlen % sizeof(SECURE_STOP)) != 0) {
                RETURN(TEE_ERR_FAIL);
            }
            _info("Secure Stops: %d\n", n);
            SECURE_STOP * ss = (SECURE_STOP*)inbuf;
            TEE_SESSION * s = &TEE_SESSIONS[0];
            for (nextSessionIndex = 0; nextSessionIndex < n; nextSessionIndex++, ss++, s++) {
                _tee_memcpy(&s->ss, ss, sizeof(SECURE_STOP));
                s->id = s->key_id = UNINIT_TYPE_ID;
                __print_secure_stop(ss);
            }
        } else {
            _dbg("Secure Stop Data Too Small\n");
        }
    } else {
        _dbg("NULL Secure Stop Buffer\n");
    }

    Exit:
    DBG_EXIT;
    return status;
}

/*
 * Marshal all outstanding secure stops
 */
int _tee_get_secure_stops(DRM_BYTE * const outbuf, DRM_DWORD * const outlen) {
    DBG_ENTER;
    LOCK;
    int status = TEE_SUCCESS;

    DRM_DWORD num = 0;
    DRM_DWORD i;
    TEE_SESSION * s = NULL;

    // get the number of secure stops, both outstanding and active
    s = &TEE_SESSIONS[0];
    for (i = 0; i < MAX_NUM_SESSIONS; ++i, ++s) {
        if (_is_init_secure_stop(&s->ss)) ++num;
    }

    // return error if buffer too small
    DRM_DWORD minlen = num * sizeof(SECURE_STOP);
    if (*outlen < minlen) {
        *outlen = minlen;
        status = TEE_ERR_BUFFERTOOSMALL;
        goto Exit;
    }

    // copy secure stops if buffer big enough
    SECURE_STOP * p = (SECURE_STOP *)outbuf;
    for (i = 0; i < MAX_NUM_SESSIONS; ++i) {
        s = &TEE_SESSIONS[(nextSessionIndex + i) % MAX_NUM_SESSIONS];
        if (_is_init_secure_stop(&s->ss)) {
            _tee_memcpy(p, &s->ss, sizeof(SECURE_STOP));
            __print_secure_stop(&s->ss);
            ++p;
        }
    }
    *outlen = (unsigned char *)p - outbuf;

    Exit:
    UNLOCK;
    DBG_EXIT;
    return status;
}

int _tee_get_secure_stop_ids(DRM_BYTE session_id[][TEE_SESSION_ID_LEN],
                             DRM_DWORD * const outnum)
{
    DBG_ENTER;
    LOCK;
    int status = TEE_SUCCESS;

    SECURE_STOP * ss = NULL;
    TEE_SESSION * s = &TEE_SESSIONS[0];
    int i;
    int out = 0;
    for (i = 0; i < MAX_NUM_SESSIONS; ++i, ++s) {
        if (!_is_active_session(s) && _is_init_secure_stop(&s->ss)) {
            ss = &s->ss;
            _tee_memcpy(session_id[out], ss->session_id, TEE_SESSION_ID_LEN);
            _print_hex("id", session_id[out], TEE_SESSION_ID_LEN);
            ++out;
        }
    }
    *outnum = out;

    //Exit:
    UNLOCK;
    DBG_EXIT;
    return status;
}

/*
 * return marshalled outstanding secure stop by session ID.
 * "Outstanding" means secure stop with no active session.
 * Such secure stop can be returned for signing and giving to the application.
 */

int _tee_get_secure_stop(const DRM_BYTE  * const session_id,
                               DRM_BYTE  * const outSecureStop,
                               DRM_DWORD * const outSecureStopLen)
{
    _print_hex("id", session_id, TEE_SESSION_ID_LEN);
 
    LOCK;
    int status = TEE_SUCCESS;

    SECURE_STOP * ss = NULL;
    TEE_SESSION * s = &TEE_SESSIONS[0];
    int i;
    int count = 0;
    for (i = 0; i < MAX_NUM_SESSIONS; ++i, ++s) {
        if (_is_init_secure_stop(&s->ss)) {
            count++;
            if (memcmp(s->ss.session_id, session_id, TEE_SESSION_ID_LEN) == 0) {
                if (ss != NULL) {
                    __print_secure_stop(&s->ss);
                    _print_hex("PANIC: duplicate session", session_id, TEE_SESSION_ID_LEN);
                }
                if (!_is_active_session(s)) {
                    ss = &s->ss;
                } else {
                    _print_hex("ERROR: still active session", session_id, TEE_SESSION_ID_LEN);
                }
            }
        }
    }

    if (ss != NULL) {
        ss->auth_time = _get_drm_time();
        DRM_DWORD len = __tee_get_secure_stop_enc_len(ss);
        if (*outSecureStopLen < len) {
            _dbg("Secure Stop Buffer Too Small: %d, should be %d\n", *outSecureStopLen, len);
            *outSecureStopLen = len;
            status = TEE_ERR_BUFFERTOOSMALL; // this flow is normal
            goto Exit;
        } else {
            *outSecureStopLen = len;
            __tee_write_secure_stop(ss, outSecureStop, *outSecureStopLen);
            *outSecureStopLen = len;
            __print_secure_stop(ss);
        }
    } else {
        _error("Invalid secure stop id. Total secure stops found: %d\n", count);
        RETURN(TEE_ERR_FAIL);
    }

    Exit:
    UNLOCK;
    DBG_EXIT;
    return status;
}

/*
 * Update secure stop with a given TEE key ID
 * Update only if more than SECURE_STOP_UPDATE_TIME_SEC elapsed since the last update.
 */

int _tee_update_secure_stop(const int key_id, DRM_BOOL * const updated) {
    int status = TEE_SUCCESS;
    //DBG_ENTER;

    TEE_SESSION * s = &TEE_SESSIONS[0];
    int i;
    SECURE_STOP * ss = NULL;
    for (i = 0; i < MAX_NUM_SESSIONS; ++i, ++s) {
        if (_is_active_session(s) && _is_init_secure_stop(&s->ss)) {
            if (s->key_id == key_id) {
                ss = &s->ss;
            }
        }
    }

    if (ss != NULL) {
        time_t t = _get_drm_time();
        if (t > (ss->update_time + SECURE_STOP_UPDATE_TIME_SEC)) {
            ss->update_time = t;
            __print_secure_stop(ss);
            *updated = 1;
        } else {
            *updated = 0;
        }
    }

    //DBG_EXIT;
    return status;
}

/*
 * remove outstanding secure stop by session ID.
 * "Outstanding" means secure stop with no active session.
 */

int _tee_commit_secure_stop(const unsigned char * const session_id) {
    int status = TEE_SUCCESS;
    DBG_ENTER;
    LOCK;

    TEE_SESSION * s = &TEE_SESSIONS[0];
    int i;
    int found = 0;
    for (i = 0; i < MAX_NUM_SESSIONS; ++i, ++s) {
        if (!_is_active_session(s) && _is_init_secure_stop(&s->ss)) {
            if (memcmp(s->ss.session_id, session_id, TEE_SESSION_ID_LEN) == 0) {
                __print_secure_stop(&s->ss);
                if (found) {
                    _print_hex("PANIC: duplicate session ID", session_id, TEE_SESSION_ID_LEN);
                }
                _uninit_secure_stop(&s->ss);
                found = 1;
            }
        }
    }
    status = found ? TEE_SUCCESS : TEE_ERR_FAIL;

    UNLOCK;
    DBG_EXIT;
    return status;
}

/*
 * Clean secure stops corresponding to no active session
 */
int _tee_reset_secure_stops(DRM_DWORD * const outNumRemoved) {
    int status = TEE_SUCCESS;
    DBG_ENTER;
    LOCK;
    int i;
    TEE_SESSION * s = &TEE_SESSIONS[0];
    int numRemoved = 0;
    for (i = 0; i < MAX_NUM_SESSIONS; ++i, ++s) {
        if (!_is_active_session(s) && _is_init_secure_stop(&s->ss)) {
            __print_secure_stop(&s->ss);
            _uninit_secure_stop(&s->ss);
			++numRemoved;
        }
    }

    // Exit:
    UNLOCK;
    DBG_EXIT;
	*outNumRemoved = numRemoved;
    return status;
}

