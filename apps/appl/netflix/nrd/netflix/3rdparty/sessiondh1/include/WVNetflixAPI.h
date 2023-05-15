#ifndef WV_NETFLIX_API
#define WV_NETFLIX_API

#include <stdint.h>
#include <stdlib.h>


#if OS_WIN
#ifdef CLIENTAPI_EXPORTS
#define CLIENT_API __declspec(dllexport)
#elif !defined(CLIENT_API_STATIC)
#define CLIENT_API __declspec(dllimport)
#else
#define CLIENT_API
#endif
#else
#define CLIENT_API
#endif

#define WV_VERSION "1.1.183-Netflix"

/* Error/status codes */
typedef enum {
    WVGlobalStatus_NoError = 0,
    WVGlobalStatus_OutOfMemory = 1,
    WVGlobalStatus_InvalidParameter = 2,
    WVGlobalStatus_InvalidState = 3
} WVGlobalStatus;

typedef enum {
    WVStopStatus_NoError = 0,
    WVStopStatus_InvalidParameter = 1,
    WVStopStatus_NotFound = 2,
    WVStopStatus_PersistError = 3,
    WVStopStatus_Disabled = 4
} WVStopStatus;

typedef enum {
    WVChallengeStatus_NoError = 0,
    WVChallengeStatus_OutOfMemory = 1,
    WVChallengeStatus_InvalidParameter = 2,
    WVChallengeStatus_ProtocolError = 3,
    WVChallengeStatus_CryptoError = 4,
    WVChallengeStatus_ECMUnderflow = 5,
    WVChallengeStatus_UnsupportedECMVersion = 6,
    WVChallengeStatus_ECMInvalid = 7,            // ECM integrity check failed
} WVChallengeStatus;

typedef enum {
    WVSessionStatus_NoError = 0,
    WVSessionStatus_OutOfMemory = 1,
    WVSessionStatus_InvalidSession = 2,
    WVSessionStatus_InvalidLicense = 3,
    WVSessionStatus_InvalidParameter = 4,
    WVSessionStatus_SecureStopError = 5,
    WVSessionStatus_NotEntitled = 6,
    WVSessionStatus_TamperDetect = 7
} WVSessionStatus;

typedef enum {
    WVClientDataStatus_NoError = 0,
    WVClientDataStatus_OutOfMemory = 1,
    WVClientDataStatus_InvalidParameter = 2
} WVClientDataStatus;

/* Data types for messages to be exchanged */
typedef uint8_t WV_AssetID[8];     /* 64 bit asset ID, network byte order */
typedef char *WV_Challenge;      /* null-terminated string */
typedef char *WV_License;        /* base64 encoded string */

typedef struct {                 /* binary blob */
    uint8_t *data;
    size_t length;
} WV_ECM;

/* 
 *   Type of WV_SessionID.  Since sessionID is persisted across reboots and should 
 *   be unique across devices, we generate a universally unique ID based on node 
 *   address, time and random value 
 */
#define UUID_SIZE 16 /* 128-bit universally unique ID */
typedef uint8_t WV_SessionID[UUID_SIZE];

/*
 *  Type of WV_SecureStop
 */
#define WV_SECURESTOP_PRIVATE_SIZE 16

typedef struct {
    uint32_t version;
    WV_AssetID assetID;
    WV_SessionID sessionID;
    uint8_t privateData[WV_SECURESTOP_PRIVATE_SIZE];
} WV_SecureStop;

#define WV_ENCODED_SECURE_STOP_SIZE (((sizeof(WV_SecureStop) + 2) / 3) * 4)
typedef char WV_EncodedSecureStop[WV_ENCODED_SECURE_STOP_SIZE + 1];


/* Callback definition.  Currently only define the secure stop message callback */
typedef struct {
    /*
     * Write buffer to persistent storage, return number of bytes written or -1 on error 
     */
    int32_t (*WriteFlash)(uint8_t *buffer, uint32_t offset, uint32_t length);

    /*
     * Read buffer from persistent storage, return number of bytes read or -1 on error
     */
    int32_t (*ReadFlash)(uint8_t *buffer, uint32_t offset, uint32_t length);
    
    /* 
     *  Get unique device ID, as NULL terminated string 
     */
    char *(*GetDeviceID)();

    /* 
     *  Get up to 'length' bytes of entropy into buffer, return the number of bytes provided 
     */
    uint32_t (*GetEntropy)(uint8_t *buffer, uint32_t length); 

} WV_Callbacks;

 
/*
 * WV_GlobalInit: 
 *
 * Global initialization, call once at startup or after calling WV_GlobalShutdown to reinitialize 
 *     [in] callbacks - vector of callback functions
 *
 * Return: a WVGlobalStatus code indicating success or error
 */

CLIENT_API WVGlobalStatus 
WV_GlobalInit(WV_Callbacks *callbacks);

/*
 * WV_GetVersion:
 *
 * Return string indicating Widevine library software version
 */
const char *WV_GetVersion();


/* 
 * WV_QuerySecureStops
 *
 * Retrieve persisted secure stop records from non-volatile storage.  Should be called on 
 * startup after WV_GlobalInit  to handle any previously aborted playback sessions and also 
 * after WV_SessionDestroy to handle the normal termination case.   The persisted secure 
 * stop entries are not removed until WV_CommitSecureStops is called.
 *
 *
 * Parameters:
 *     [in/out] - pNumEntries  on entry, the caller should set *pNumEntries to the size of the 
 *                caller¢s array.  On exit, *pNumEntries is set to number of entries returned,
 *                or the number of entries available if secureStops is NULL. 
 *
 *     [out] - secureStops  filled in with (*nEntries) secure stop structures.  secureStops
 *             may be passed as NULL to query the number of secureStops available.
 *
 * Returns:
 *     WVStopStatus code indicating success or error
 */

CLIENT_API WVStopStatus 
WV_QuerySecureStops(size_t *pNumEntries, WV_SecureStop *secureStops);

/*
 *   WV_CommitSecureStop
 *
 *   Remove one persisted secure stop record from non-volatile storage.  This function 
 *   should be called after receiving confirmation that the server processed the secure 
 *   stop message.
 *
 *   Parameters:
 *       [in] secureStop - the secureStop to commit
 *
 *   Returns:
 *       WVStopStatus code indicating success or error
 */

CLIENT_API WVStopStatus 
WV_CommitSecureStop(WV_SecureStop *secureStop);

/* 
 *   WV_EncodeSecureStop
 *   Convert a binary client-byte-order secure stop structure into a cross-platform
 *   portable encoded string representation for transmission to the server.
 *
 *       [in] secureStop: A pointer to the client binary secure stop structure
 *       [out] encodedSecureStop:  The WV_EncodedSecureStop which will
 *            receive the encoded representation of the secureStop parameter
 *
 *  Returns: 
 *       WVStopStatus code indicating success or error
 */
CLIENT_API WVStopStatus
WV_EncodeSecureStop(WV_SecureStop *secureStop, WV_EncodedSecureStop encodedSecureStop);

/* 
 *   WV_DecodeSecureStop
 *   Convert a cross-platform portable encoded secure stop structure
 *   into its binary client-byte-order secure stop representation.
 *
 *     [in] encodedSecureStop:  The WV_EncodedSecureStop which contains
 *         the encoded string representation of the secure stop
 *     [out] secureStop: A pointer to the client binary secure stop structure
 *
 *  Returns: 
 *       WVStopStatus code indicating success or error
 */
CLIENT_API WVStopStatus
WV_DecodeSecureStop(WV_EncodedSecureStop encodedSecureStop, WV_SecureStop *secureStop);

/* 
 *   WV_GetChallenge:
 *   Request the challenge for the specified asset and Entitlement Control Message.
 *
 *     [in] ecm:  the entitlement control message from the movie metadata
 *     [out] challenge:  on successful return, *challenge will be assigned a pointer to a 
 *         null-terminated challenge string.  The memory in which the challenge is
 *         stored will be allocated using the provided allocator parameter.  The caller is
 *         responsible for releasing the allocated memory using a corresponding 
 *         deallocator.  If WV_GetChallenge fails, as indicated by a non-zero 
 *         WVChallengeStatus  code, *challenge will be set to NULL.
 *     [in] allocator:  pointer to a memory allocator function for the challenge string, 
 *         the length of the string including the NULL termination is provided to the
 *         allocator in the length parameter.  If allocator is NULL, memory is allocated
 *         using malloc.
 *
 *  Returns: 
 *      A WVChallengeStatus code indicating success or error
 */

CLIENT_API WVChallengeStatus 
WV_GetChallenge(WV_ECM *ecm, WV_Challenge *challenge, void *(*allocator)(size_t nbytes));

/* 
 *   WV_GetAssetID:
 *   Requests the asset ID from the Entitlement Control Message.
 *
 *     [in] ecm:  the entitlement control message from the movie metadata
 *     [in, out] asset ID:  on successful return, asset ID buffer will be 
 *         filled in. If WV_GetAssetID fails, as indicated by a non-zero 
 *         WVChallengeStatus  code, assetID will be unchanged.
 *
 *  Returns: 
 *      A WVChallengeStatus code indicating success or error. Possible values
 *        WVChallengeStatus_NoError
 *        WVChallengeStatus_InvalidParameter
 *        WVChallengeStatus_ECMUnderflow
 *        WVChallengeStatus_UnsupportedECMVersion
 *        WVChallengeStatus_ECMInvalid
 */

CLIENT_API WVChallengeStatus 
WV_GetAssetID(WV_ECM *ecm, WV_AssetID assetID);

/* 
 *   WV_SessionCreate:
 *   Create a crypto session given the EMM.  
 *
 *     [in] license message from DRM server
 *     [out] session ID  identifies the created session 
 *
 *  Return: a WVSessionStatus code indicating success or error
 */ 

CLIENT_API WVSessionStatus 
WV_SessionCreate(WV_License license, WV_SessionID session);

/* 
 *   WV_Decrypt:
 *   Decrypt a buffer using the context of a created session.  Data is decrypted in place.
 *
 *     [in] session  specifies the session context
 *     [in] buffer  pointer to first byte of data to decrypt
 *     [in] byteCount  number of bytes to decrypt
 * 
 *  Return: a WVSessionStatus code indicating success or error
 */ 

CLIENT_API WVSessionStatus 
WV_Decrypt(WV_SessionID session, void *buffer, uint32_t byteCount);

/* 
 *   WV_SessionDestroy:
 *   Destroy a session by sessionID.  The session must have been previously created 
 *   with WV_SessionCreate.  Releases all resources associated with the session.
 *
 *     [in] session  specifies the session context to destroy
 */ 

CLIENT_API WVSessionStatus 
WV_SessionDestroy(WV_SessionID session);

/*
 *   WV_AddClientData:
 *   Add a {name, value} pair of data to be passed from the client to the server in 
 *   the challenge request.  The name and value can be any arbitrary sequence of bytes.  
 *   You may call WV_AddClientData multiple times to add additional {name, value} pairs 
 *   to the challenge request.
 *
 *     [in] name  A byte array containing the name of the client data to set
 *     [in] nameLength  The length of the name in bytes
 *     [in] value  A byte array containing the value of the client data to set
 *     [in] valueLength  The length of the value in bytes
 *
 *   Return: a WVClientDataStatus code indicating success or error
 */

CLIENT_API WVClientDataStatus 
WV_AddClientData(uint8_t *name, uint32_t nameLength, 
                 uint8_t *value, uint32_t valueLength);

/* 
 *   WV_GlobalShutdown:
 *   Global termination.  Call to release all resources associated with the Widevine DRM module. 
 *
 *   Return: a WVGlobalStatus code indicating success or error
 */

CLIENT_API WVGlobalStatus 
WV_GlobalShutdown();

/*
 *   WV_SetMinTimePerAESBlock:
 *   Only included in linux client simulation builds, this function allows the
 *   clock tick limit time per AES block decrypt to be set. It is needed because
 *   decryption speed will vary on different machines. The tick limit is used as
 *   part of the secure time verification.
 */

CLIENT_API void
WV_SetMinTimePerAESBlock(unsigned long nanosecs);


#endif
