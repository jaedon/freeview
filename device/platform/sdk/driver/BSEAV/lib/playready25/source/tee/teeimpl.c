#include <string.h>
#include <teecommon.h>
#include <teeimpl.h>
#include <teeimplinternal.h>
#include <drmblackboxtypes.h>
#include <oemhal.h>
#include <oemaes.h>
#include <oemaeskey.h>
#include <oem_brcm.h>

#define P256_SIGNATURE_SIZE 64
#define SHA256_DIGEST_LENGTH SHA256_DIGEST_SIZE_IN_BYTES

static DRM_BOOL secureStopOn = FALSE;



/***********************************************************************************************************************************
 * Master Key is permanently generated for a given TEE. In production should be randomly generated and NEVER the same across TEEs. *
 * This key is used to protect device private keys persisted by applications outside the TEE.                                      *
 ***********************************************************************************************************************************/
static const unsigned char MASTER_KEY[16] = { 0xf0, 0xe1, 0xd2, 0xc3, 0xb4, 0xa5, 0x96, 0x87, 0x78, 0x69, 0x5a, 0x4b, 0x3c, 0x2d, 0x1e, 0x0f };

#ifdef HWDECODING
int __aes_enc_dec_hw(const DRM_BYTE * const inbuf, const int inlen, DRM_BYTE * const outbuf, int *outlen, DRM_BOOL encrypt) {
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_AES_KEY aesKey;
	uint8_t *bufferptr = NULL;

    if (inlen % DRM_AES_BLOCKLEN != 0) {
        _error("Invalid Data Length %d\n", inlen);
        ChkDR(DRM_E_FAIL);
    }

	bufferptr = BCM_DECRYPTOR_SOC_Malloc(inlen);
	memset(bufferptr, 0, inlen);
	memcpy((void *)bufferptr, (void *)inbuf, inlen);

	if(encrypt)
	{
		if(BCM_DECRYPTOR_SOC_OTPKEY_DoEncrypt(bufferptr, bufferptr, inlen)!=0)
		{
			_error("BCM_DECRYPTOR_SOC_OTPKEY_DoEncrypt Fail \n");
			ChkDR(DRM_E_FAIL);
		}
	}
	else
	{
		if(BCM_DECRYPTOR_SOC_OTPKEY_DoDecrypt(bufferptr, bufferptr, inlen)!=0)
		{
			_error("BCM_DECRYPTOR_SOC_OTPKEY_DoDecrypt Fail \n");
			ChkDR(DRM_E_FAIL);
		}
	}
	memcpy(outbuf, bufferptr, inlen);
	*outlen = inlen;

    ErrorExit:
	if(bufferptr!=NULL)
		BCM_DECRYPTOR_SOC_Free(bufferptr);
    return DRM_SUCCEEDED(dr) ? TEE_SUCCESS : TEE_ERR_FAIL;
}
#endif
int __aes_enc_dec(const DRM_BYTE * const inbuf, const int inlen, DRM_BYTE * const outbuf, int *outlen, DRM_BOOL encrypt) {
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_AES_KEY aesKey;

    if (inlen % DRM_AES_BLOCKLEN != 0) {
        _error("Invalid Data Length %d\n", inlen);
        ChkDR(DRM_E_FAIL);
    }

    ChkDR( Oem_Aes_SetKey( MASTER_KEY, &aesKey ) );

    DRM_BYTE tmp[DRM_AES_BLOCKLEN];
    int i;
    for (i = 0; i < inlen; i += DRM_AES_BLOCKLEN) {
        _tee_memcpy(tmp, inbuf + i, DRM_AES_BLOCKLEN);
        if (encrypt) {
            ChkDR ( Oem_Aes_EncryptOne(&aesKey, tmp) );
        } else {
            ChkDR ( Oem_Aes_DecryptOne(&aesKey, tmp) );
        }
        _tee_memcpy(outbuf + i, tmp, DRM_AES_BLOCKLEN);
    }
    _tee_memset(tmp, 0, sizeof(tmp)); /* clean memory*/
    *outlen = inlen;

    ErrorExit:
    return DRM_SUCCEEDED(dr) ? TEE_SUCCESS : TEE_ERR_FAIL;
}

/*
 * export sensitive data to application by encrypting it with one of the TEE protection keys
 */
int _export_data(const DRM_BYTE * const inbuf, const int inlen, DRM_BYTE * const outbuf, int *outlen) {
#ifdef HWDECODING
    return __aes_enc_dec_hw(inbuf, inlen, outbuf, outlen, TRUE);
#else
	return __aes_enc_dec(inbuf, inlen, outbuf, outlen, TRUE);
#endif


}

/*
 * import previously exported sensitive data from application by decrypting it with one of the TEE protection keys
 */
int _import_data(const DRM_BYTE * const inbuf, const int inlen, DRM_BYTE * const outbuf, int *outlen) {
#ifdef HWDECODING
    return __aes_enc_dec_hw(inbuf, inlen, outbuf, outlen, FALSE);
#else
    return __aes_enc_dec(inbuf, inlen, outbuf, outlen, FALSE);
#endif
}

/*
 * SHA256 computation
 */
int _compute_sha256(const DRM_BYTE *inbuf, DRM_DWORD inlen, DRM_BYTE digest[SHA256_DIGEST_LENGTH]) {
    DRM_RESULT dr = DRM_SUCCESS;
    _print_hex("data", inbuf, inlen);
    DRM_SHA256_Context    shaContext  = { { { 0 } }, { 0 }, { 0 } };
    DRM_SHA256_Digest     digestValue = { { 0 } };
    ChkDR( DRM_SHA256_Init( &shaContext ) );
    ChkDR( DRM_SHA256_Update( &shaContext, inbuf, inlen ) );
    ChkDR( DRM_SHA256_Finalize( &shaContext, &digestValue ) );
    _tee_memcpy( digest, digestValue.m_rgbDigest, SHA256_DIGEST_SIZE_IN_BYTES );

    ErrorExit:
    return DRM_SUCCEEDED(dr) ? TEE_SUCCESS : TEE_ERR_FAIL;
}

/*************************************************************************************
 * Internal state retrieval function taking care of crypto protection and versioning *
 *************************************************************************************/

/*
 * validate encrypted sha256 prepended to the store data
 */
int _tee_validate_state_integrity(const unsigned char * const inbuf,
                                  const           int         inlen)
{
    int status = TEE_SUCCESS;
    DBG_ENTER;

    /* decrypt sha256 prepended to state data*/
    unsigned char foundSha[SHA256_DIGEST_LENGTH];
    int foundShaLen = SHA256_DIGEST_LENGTH;
    CHECK(_import_data(inbuf, SHA256_DIGEST_LENGTH, foundSha, &foundShaLen));

    /* compute sha256 of the state data*/
    unsigned char expectedSha[SHA256_DIGEST_LENGTH];
    CHECK(_compute_sha256(inbuf + SHA256_DIGEST_LENGTH, inlen - SHA256_DIGEST_LENGTH, expectedSha));

    /* compare prepended sha to computed sha*/
    if (memcmp(foundSha, expectedSha, SHA256_DIGEST_LENGTH) != 0) {
        _error("Corrupted State\n");
        RETURN(TEE_ERR_FAIL);
    }

    Exit:
    DBG_EXIT;
    return status;
}

/*
 * prepend encrypted sha256 to the store data
 */
int _tee_protect_state_integrity(      unsigned char * const inbuf,
                                 const           int         inlen)
{
    int status = TEE_SUCCESS;
    DBG_ENTER;

    /* compute sha256 of the state data*/
    unsigned char outSha[SHA256_DIGEST_LENGTH];
    CHECK(_compute_sha256(inbuf + SHA256_DIGEST_LENGTH, inlen - SHA256_DIGEST_LENGTH, outSha));

    /* encrypt sha256 with the master key and prepend it to the data*/
    int outLen = SHA256_DIGEST_LENGTH;
    CHECK(_export_data(outSha, SHA256_DIGEST_LENGTH, inbuf, &outLen));

    Exit:
    DBG_EXIT;
    return status;
}

/*
 * prepend encrypted sha256 and version to the store data
 */
int _teeGetState(DRM_BYTE  * const outState,
                 DRM_DWORD * const outStateLen)
{
    int status = TEE_SUCCESS;
    DBG_ENTER;

    /* See if input buffer is big enough for the header,*/
    /* which is encrypted SHA256 concatenated with the integer version*/
    int VERSION = _teeGetStateVersion();
    DRM_DWORD headerLen = SHA256_DIGEST_LENGTH + sizeof(VERSION);
    if (*outStateLen < headerLen) {
        status = TEE_ERR_BUFFERTOOSMALL;
        goto Exit;
    }

    /* See if the rest of the input buffer is big enough for secure stop data.*/
    /* If it is, write this data past the header*/
    DRM_DWORD outLen = *outStateLen - headerLen;
    CHECK(_tee_get_secure_stops(outState + headerLen, &outLen));

    /* Input buffer is big enough - write version past SHA256 location and compute total length*/
    *(int*)(outState + SHA256_DIGEST_LENGTH) = VERSION;
    *outStateLen = outLen + headerLen;

    /* Insert encrypted SHA256 of state data at the beginning*/
    CHECK(_tee_protect_state_integrity(outState, *outStateLen));

    Exit:
    DBG_EXIT;
    return status;
}

/*
 * validate persisted state and initialize secure stops
 */
int _teeInitState(const unsigned char * const inbuf,
                  const           int         inlen)
{
    int status = TEE_SUCCESS;
    DBG_ENTER;

    /* See if input buffer is big enough for the header,*/
    /* which is encrypted SHA256 concatenated with the integer version*/
    if (secureStopOn) {
        CHECK(_teeInitStateVersion());
        int VERSION = _teeGetStateVersion();
        const int headerSize = SHA256_DIGEST_LENGTH + sizeof(VERSION);

        if (inbuf != NULL && inlen >= headerSize && (_tee_validate_state_integrity(inbuf, inlen) == TEE_SUCCESS)) {
            int version = *(int*)(inbuf + SHA256_DIGEST_LENGTH);
            if (VERSION == version) {
                _dbg("secure store version match: %x\n", version);
                CHECK(_tee_init(inbuf + headerSize, inlen - headerSize));
            } else if (VERSION == (version + 1)) {
                _error("secure store version off-by-1: %x < %x\n", version, VERSION);
                CHECK(_tee_init(inbuf + headerSize, inlen - headerSize));
            } else {
                _error("secure store version mismatch: %x vs %x\n", version, VERSION);
                CHECK(_tee_init(NULL, 0));
            }
        } else {
            _error("missing or malformed secure store\n");
            CHECK(_tee_init(NULL, 0));
        }
    } else {
        _dbg("secure stop disabled\n");
        CHECK(_tee_init(NULL, 0));
    }

    Exit:
    DBG_EXIT;
    return status;
}

/***************************************************************************
 *                                                                         *
 * FUNCTIONS BELOW ARE DEFINED IN teeimpl.h. THEY ARE INVOKED FROM RICH OS *
 *                                                                         *
 ***************************************************************************/

/***************************
 * TEE Lifecycle Functions *
 ***************************/

int teeInitialize(const DRM_BYTE * const inbuf, const DRM_DWORD inlen) {
    int status = TEE_SUCCESS;
#if TARGET_LITTLE_ENDIAN
    _info("Initializing TEE (little-endian), useSecureStop: %d\n", secureStopOn);
#else
    _info("Initializing TEE (big-endian), useSecureStop: %d\n", secureStopOn);
#endif
    CHECK(_teeInitState(inbuf, inlen));

    Exit:
    return status;
}

int teeReinitialize() {
    int status = TEE_SUCCESS;
    _info("Reinitializing TEE, useSecureStop: %d\n", secureStopOn);
    /*Exit:*/
    return status;
}

int teeUninitialize() {
    int status = TEE_SUCCESS;
    _info("Uninitializing TEE, useSecureStop: %d\n", secureStopOn);
    CHECK(_tee_destroy());
	CHECK(_teeUninitStateVersion());/* dealloc for teestateversion file (humax porting)*/
    Exit:
    return status;
}

/*******************
 * Misc. Functions *
 *******************/

int teeRand(DRM_BYTE * const outbuf,
                 int   const outlen)
{
    Oem_Random_GetBytes(NULL, outbuf, outlen);
    return 1;
}

/*************************************************
 * Update Secure Stop for a given key, if needed *
 *************************************************/
int teeSecureStopUpdated(const OEM_HAL_KEY_REGISTER_INDEX inKeyIndex, DRM_BOOL * const outUpdate)
{
    int status = TEE_SUCCESS;
    /* DBG_ENTER;*/
    CHECK_NULL(outUpdate);

    if (secureStopOn) {
        CHECK(_tee_update_secure_stop(inKeyIndex, outUpdate));
        if (*outUpdate) {
            CHECK(_teeAdvanceStateVersion());
        }
    }

    Exit:
    /*DBG_EXIT;*/
    return status;
}

/*************************************************
 * Check if Secure Stop Functionality is Enabled *
 *************************************************/
int teeIsSecureStopOn(DRM_BOOL * const isOn) {
    *isOn = secureStopOn;
    return TEE_SUCCESS;
}

/***********************************************************************************************
 * Enable/Disable Secure Stop Functionality (default hard-coded into PlayReady implementation) *
 * If Secure Stop is disabled, other secure stops APIs become no-op:                           *
 * they do not fail, just return no secure stop / state data                                   *
 ***********************************************************************************************/
int teeTurnSecureStop(const DRM_BOOL inOnOff) {
    int status = TEE_SUCCESS;
    DBG_ENTER;
    if (inOnOff) {
        _info("turning secure stop on\n");
    } else {
        if (secureStopOn) {
            _info("turning secure stop off: purging sessions\n");
            _tee_init_secure_stops(NULL, 0); /* turning secures stop off purges all sessions and secure stops*/
        } else {
            _info("turning secure stop off\n");
        }
    }
    secureStopOn = inOnOff;

    /*Exit:*/
    DBG_EXIT;
    return status;
}

/***************************************************************************************
 * Create a new Session inside the TEE; generate and return 16-byte random session ID. *
 * The Session ID is to be used as the NONCE in a License Challenge.                   *
 ***************************************************************************************/
int teeStartSession(DRM_BYTE * const outSessionId)
{
    int status = TEE_SUCCESS;
    DBG_ENTER;
    CHECK_NULL(outSessionId);

    teeRand(outSessionId, TEE_SESSION_ID_LEN);
    _print_hex("SessionId", outSessionId, TEE_SESSION_ID_LEN);

    CHECK(_tee_add_session(outSessionId));

    Exit:
    DBG_EXIT;
    return status;
}

/******************************************************************************
 * Cancel a new Session inside the TEE. Only possible before License binding. *
 ******************************************************************************/
int teeCancelSession(const DRM_BYTE * const inSessionId) {
    int status = TEE_SUCCESS;
    DBG_ENTER;
    CHECK_NULL(inSessionId);

    _print_hex("SessionId", inSessionId, TEE_SESSION_ID_LEN);

    CHECK(_tee_cancel_session(inSessionId));

    Exit:
    DBG_EXIT;
    return status;
}

/***************************
 * Bind license to session *
 ***************************/
int teeBindLicense(const DRM_BYTE * const inSessionId,
                   const DRM_BYTE * const inKID)
{
    int status = TEE_SUCCESS;
    DBG_ENTER;
    CHECK_NULL(inSessionId);
    CHECK_NULL(inKID);

    CHECK(_tee_bind_license(inSessionId, inKID));

    Exit:
    DBG_EXIT;
    return status;
}

/***********************************************************************************
 * Bind content decryption key to a given session, at the beginning of a playback. *
 ***********************************************************************************/
int teeBindSession(const OEM_HAL_KEY_REGISTER_INDEX inKeyIndex,
                   const DRM_BYTE * const inKID)
{
    int status = TEE_SUCCESS;
    DBG_ENTER;
    CHECK_NULL(inKID);

    CHECK(_tee_bind_session(inKeyIndex, inKID));

    Exit:
    DBG_EXIT;
    return status;
}

/***************************************
 * Stop Session at the end of playback *
 ***************************************/
int teeStopSession(const OEM_HAL_KEY_REGISTER_INDEX inKeyIndex)
{
    int status = TEE_SUCCESS;
    /* DBG_ENTER;*/

    if (secureStopOn) {
        CHECK(_tee_remove_session(inKeyIndex));
    }

    Exit:
    /*DBG_EXIT;*/
    return status;
}

/*********************************************************************************************
 * Return IDs of outstanding secure stops.                                                   *
 *********************************************************************************************/
int teeGetSecureStopIds(DRM_BYTE outSessionIds[TEE_MAX_NUM_SECURE_STOPS][TEE_SESSION_ID_LEN],
                        DRM_DWORD * const outNumSessionIds)
{
    int status = TEE_SUCCESS;
    DBG_ENTER;
    CHECK_NULL(outSessionIds);
    CHECK_NULL(outNumSessionIds);

    if (secureStopOn) {
        CHECK(_tee_get_secure_stop_ids(outSessionIds, outNumSessionIds));
    } else {
        *outNumSessionIds = 0;
    }

    Exit:
    DBG_EXIT;
    return status;
}

/*********************************************************************************************
 * Return unsigned secure stop for a given session ID.                                       *
 * Return error if sessionID does not exist or corresponds to the playback still in-progress *
 *********************************************************************************************/

int teeGetSecureStop(const DRM_BYTE * const inSessionId,
                     const OEM_HAL_KEY_REGISTER_INDEX inKeyIndex,
                           DRM_BYTE * const outSecureStop,
                           DRM_DWORD * const outSecureStopLen,
                           TEE_SIGNATURE * const outSig)
{
    int status = TEE_SUCCESS;
    DBG_ENTER;
    CHECK_NULL(inSessionId);
    CHECK_NULL(outSecureStop);
    CHECK_NULL(outSecureStopLen);
    CHECK_NULL(outSig);

    _print_hex("SessionId", inSessionId, TEE_SESSION_ID_LEN);
    if (secureStopOn) {
		DRM_CRYPTO_CONTEXT CryptoContext;
        CHECK(_tee_get_secure_stop(inSessionId, outSecureStop, outSecureStopLen));
        _print_hex("SecureStop", outSecureStop, *outSecureStopLen);
        DRM_DWORD siglen = P256_SIGNATURE_SIZE;
        DRM_RESULT dr = Oem_Hal_SignWithPrivateKey(
            NULL, /* OEM Context*/
            &CryptoContext, /* Crypto Context*/
            outSecureStop, /* data to be signed*/
            *outSecureStopLen, /* size of the data to be signed*/
            outSig->data, /* signature buffer*/
            &siglen, /* signature buffer length*/
            OEM_HAL_HASH_TYPE_SHA256, /* hash algorithm*/
            OEM_HAL_SIGNATURE_SCHEME_ECDSA_P256, /* signature algorithm*/
            OEM_HAL_KEY_TYPE_ECC_P256_SIGN, /* key type*/
            inKeyIndex /* keyIndex*/
            );
        if (dr != DRM_SUCCESS)
        {
            _error("Oem_Hal_SignWithPrivateKey [%p %d %ld] - err %lx\n", outSecureStop, *outSecureStopLen, inKeyIndex, dr);
            RETURN(TEE_ERR_FAIL);
        }
        outSig->length = siglen;
    } else {
        RETURN(TEE_ERR_FAIL);
    }

    Exit:
    DBG_EXIT;
    return status;
}

/*********************************************************************************
 * Permanently remove outstanding secure stop for a given session ID.            *
 * Pass updated persistent state back to the caller.                             *
 * Return error if sessionID does not correspond to any outstanding secure stop. *
 *********************************************************************************/

int teeCommitSecureStop(const DRM_BYTE  * const inSessionId,
                              DRM_BYTE  * const outState,
                              DRM_DWORD * const outStateLen)
{
    int status = TEE_SUCCESS;
    DBG_ENTER;
    if (secureStopOn) {
        CHECK_NULL(inSessionId);
        CHECK_NULL(outState);
        CHECK_NULL(outStateLen);

        CHECK(_tee_commit_secure_stop(inSessionId));
        CHECK(_teeAdvanceStateVersion());
        CHECK(_teeGetState(outState, outStateLen));
    } else {
        RETURN(TEE_ERR_FAIL);
    }

    Exit:
    DBG_EXIT;
    return status;
}

/*************************************************************
 * Permanently remove all outstanding secure stops.          *
 * Pass updated (empty) persistent state back to the caller. *
 *************************************************************/

int teeResetSecureStops(DRM_BYTE  * const outState,
                        DRM_DWORD * const outStateLen,
                        DRM_DWORD * const outNumRemoved)
{
    int status = TEE_SUCCESS;
    DBG_ENTER;
    if (secureStopOn) {
        CHECK_NULL(outState);
        CHECK_NULL(outStateLen);
        CHECK_NULL(outNumRemoved);

        CHECK(_tee_reset_secure_stops(outNumRemoved));
        CHECK(_teeAdvanceStateVersion());
        CHECK(_teeGetState(outState, outStateLen));
    } else {
        RETURN(TEE_ERR_FAIL);
    }

    Exit:
    DBG_EXIT;
    return status;
}

/*****************************************************************************
 * Get current state to persist. State contains version and all secure stops *
 *****************************************************************************/

int teeGetState(DRM_BYTE  * const outState,
                DRM_DWORD * const outStateLen)
{
    int status = TEE_SUCCESS;
    DBG_ENTER;
    if (secureStopOn) {
        CHECK_NULL(outState);
        CHECK_NULL(outStateLen);

        CHECK(_teeGetState(outState, outStateLen));
    } else {
        RETURN(TEE_ERR_FAIL);
    }

    Exit:
    DBG_EXIT;
    return status;
}
