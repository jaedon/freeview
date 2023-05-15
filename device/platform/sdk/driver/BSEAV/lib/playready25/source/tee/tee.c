#include <tee.h>
#include <teeimpl.h>
#include <teeimplinternal.h>
#include <stdio.h>

#ifdef TEE_TESTMAIN
#define DRM_SUCCESS 0
#define DRM_E_FAIL 1
#define DRM_E_BUFFERTOOSMALL 2
#else
#include <drmtypes.h>
#include <drmresults.h>
#endif

/***********************
 * Client-side helpers *
 ***********************/

// convert TEE error code into PlayReady error code
int cvt(int code) {
    switch (code) {
        case TEE_SUCCESS:
            return DRM_SUCCESS;
        case TEE_ERR_FAIL:
            return DRM_E_FAIL;
        case TEE_ERR_BUFFERTOOSMALL:
            return DRM_E_BUFFERTOOSMALL;
        default:
            return DRM_E_FAIL;
    }
}

/***************************
 * TEE Lifecycle Functions *
 ***************************/

int TEE_Initialize(const DRM_BYTE * const inbuf,
                   const DRM_DWORD        inlen)
{
    return cvt(teeInitialize(inbuf, inlen));
}

int TEE_Reinitialize() {
    return cvt(teeReinitialize());
}

int TEE_Uninitialize() {
    return cvt(teeUninitialize());
}

/***********************************************************************************************
 * Enable/Disable Secure Stop Functionality (default hard-coded into PlayReady implementation) *
 * If Secure Stop is disabled, other secure stops APIs become no-op:                           *
 * they do not fail, just return no secure stop / state data                                   *
 ***********************************************************************************************/
int TEE_TurnSecureStop(const DRM_BOOL inOnOff) {
    return cvt(teeTurnSecureStop(inOnOff));
}

/*************************************************
 * Check if Secure Stop Functionality is Enabled *
 *************************************************/
int TEE_IsSecureStopOn(DRM_BOOL * const isOn) {
    return cvt(teeIsSecureStopOn(isOn));
}


/***************************************************************************************
 * Create a new Session inside the TEE; generate and return 16-byte random session ID. *
 * The Session ID is to be used as the NONCE in a License Challenge.                   *
 ***************************************************************************************/
int TEE_StartSession(DRM_BYTE * const outSessionId)
{
    return cvt(teeStartSession(outSessionId));
}

/******************************************************************************
 * Cancel a new Session inside the TEE. Only possible before License binding. *
 ******************************************************************************/
int TEE_CancelSession(const DRM_BYTE * const inSessionId) {
    return cvt(teeCancelSession(inSessionId));
}

/***************************
 * Bind license to session *
 ***************************/
int TEE_BindLicense(const DRM_BYTE  * const inSessionId,
                    const DRM_BYTE  * const inKID)
{
    return cvt(teeBindLicense(inSessionId, inKID));
}

/***********************************************************************************
 * Bind content decryption key to a given session, at the beginning of a playback. *
 ***********************************************************************************/
int TEE_BindSession(const OEM_HAL_KEY_REGISTER_INDEX inKeyIndex,
                    const DRM_BYTE * const inKID)
{
    return cvt(teeBindSession(inKeyIndex, inKID));
}

/***************************************
 * Stop Session at the end of playback *
 ***************************************/
int TEE_StopSession(const OEM_HAL_KEY_REGISTER_INDEX inKeyIndex)
{
    return cvt(teeStopSession(inKeyIndex));
}

/**************************************************************************
 * Check if secure stop state needs to be updated for a given content key *
 **************************************************************************/
int TEE_SecureStopUpdated(const OEM_HAL_KEY_REGISTER_INDEX inKeyIndex,
                                DRM_BOOL * const outUpdate)
{
    return cvt(teeSecureStopUpdated(inKeyIndex, outUpdate));
}

/*********************************************************************************************
 * Return IDs of outstanding secure stops.                                                   *
 *********************************************************************************************/
int TEE_GetSecureStopIds(DRM_BYTE          outSessionIds[TEE_MAX_NUM_SECURE_STOPS][TEE_SESSION_ID_LEN],
                         DRM_DWORD * const outNumSessionIds)
{
    return cvt(teeGetSecureStopIds(outSessionIds, outNumSessionIds));
}

/*********************************************************************************************
 * Return secure stop for a given session ID.                                                *
 * Return error if sessionID does not exist or corresponds to the playback still in-progress *
 *********************************************************************************************/

int TEE_GetSecureStop(const DRM_BYTE * const inSessionId,
                      const OEM_HAL_KEY_REGISTER_INDEX inKeyIndex,
                            DRM_BYTE * const outSecureStop,
                            DRM_DWORD  * const outSecureStopLen,
                            TEE_SIGNATURE * const outSig)
{
    return cvt(teeGetSecureStop(inSessionId,
                                inKeyIndex,
                                outSecureStop,
                                outSecureStopLen,
                                outSig));
}

/*********************************************************************************
 * Permanently remove outstanding secure stop for a given session ID.            *
 * Pass updated persistent state back to the caller.                             *
 * Return error if sessionID does not correspond to any outstanding secure stop. *
 *********************************************************************************/
int TEE_CommitSecureStop(const DRM_BYTE  * const inSessionId,
                               DRM_BYTE  * const outState,
                               DRM_DWORD * const outStateLen)
{
    return cvt(teeCommitSecureStop(inSessionId, outState, outStateLen));
}

/*************************************************************
 * Permanently remove all outstanding secure stops.          *
 * Pass updated (empty) persistent state back to the caller. *
 *************************************************************/
int TEE_ResetSecureStops(DRM_BYTE  * const outState,
                         DRM_DWORD * const outStateLen,
                         DRM_DWORD * const outNumRemoved)
{
     return cvt(teeResetSecureStops(outState, outStateLen, outNumRemoved));
}

/*****************************************************************************
 * Get current state to persist. State contains version and all secure stops *
 *****************************************************************************/
int TEE_GetState(DRM_BYTE  * const outState,
                 DRM_DWORD * const outStateLen)
{
     return cvt(teeGetState(outState, outStateLen));
}

