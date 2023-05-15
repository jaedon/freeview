/*
 * oem_crypto.c
 *
 *  Created on: Oct 22, 2012
 *      Author: cdisc
 */
#include <string.h>
#include <sys/stat.h>

#include "bstd.h"
#include "bdbg.h"
#include "bkni.h"

#include "oem_crypto.h"

#include "drm_metadata.h"
#include "drm_common.h"
#include "drm_common_priv.h"
#include "drm_common_swcrypto.h"
#include "drm_widevine.h"
#include "drm_secure_store.h"
#include "drm_key_binding.h"


/* local definitions */
#define MAX_COMMON_KEY_SIZE (16)
static OEMCrypto_UINT8 commonKey[MAX_COMMON_KEY_SIZE] = {0x00};
static uint8_t previousLast16EncAlignedBytes[16] = {0x00};
static bool isCmnDrmInitialized = false;

#define BRCM_KEYBOX_L3_PATH "/data/brcm_keybox" 

/* MACRO called by all level 3 APIs */
#define CHECK_COMMON_DRM_INITIALIZE() \
		if(isCmnDrmInitialized == false)\
		{\
			DrmCommonInit_t commonDrmInit;\
			BKNI_Memset((uint8_t*)&commonDrmInit, 0x00, sizeof(DrmCommonInit_t));\
		    if(DRM_Common_BasicInitialize(commonDrmInit) != Drm_Success)\
		    {\
                BDBG_ERR(("%s - Error calling 'DRM_Common_BasicInitialize'", __FUNCTION__));\
		    	rc = OEMCrypto_ERROR_CMNDRM; \
		    	goto ErrorExit;\
		    }\
		    isCmnDrmInitialized = true;\
		}

BDBG_MODULE(oem_crypto);

/******************************************************************************
** FUNCTION
**   OEMCrypto_Initialize
**
** DESCRIPTION:
**    Initializes the crypto hardware
**
** PARAMETERS:
**   void
**
** RETURNS:
**   OEMCrypto_SUCCESS
**   OEMCrypto_ERROR_INIT_FAILED
**
******************************************************************************/
OEMCryptoResult OEMCrypto_Initialize(void)
{
	OEMCryptoResult rc = OEMCrypto_SUCCESS;

	BDBG_MSG(("%s - Entered function", __FUNCTION__));

	BDBG_MSG(("%s - Exiting function", __FUNCTION__));
	return rc;
}


/******************************************************************************
** FUNCTION
**   OEMCrypto_Terminate
**
** DESCRIPTION:
**    Close crypto engine and release resources
**
** PARAMETERS:
**   void
**
** RETURNS:
**   OEMCrypto_SUCCESS
**   OEMCrypto_ERROR_TERMINATE_FAILED
**
******************************************************************************/
OEMCryptoResult OEMCrypto_Terminate(void)
{
	OEMCryptoResult rc = OEMCrypto_SUCCESS;

	BDBG_MSG(("%s - Entered function", __FUNCTION__));

	BDBG_MSG(("%s - Exiting function", __FUNCTION__));
	return rc;
}



/******************************************************************************
** FUNCTION
**   OEMCrypto_Open
**
** DESCRIPTION:
**    Open the security engine and provide a block of memory that can be used
**    for crypto operations
**
** PARAMETERS:
**   buffer - address of a valid pointer
**   bufferSize - on return, size of the allocated buffer
**
** RETURNS:
**   OEMCrypto_SUCCESS
**   OEMCrypto_ERROR_OPEN_FAILED
**
******************************************************************************/
OEMCryptoResult OEMCrypto_Open(
		OEMCrypto_UINT8** buffer,
		OEMCrypto_UINT32 *bufferSize)
{
	OEMCryptoResult rc = OEMCrypto_SUCCESS;

	BDBG_MSG(("%s - Entered function", __FUNCTION__));

	if(DRM_Common_MemoryAllocate((uint8_t **)buffer, 16*1024*1024) != Drm_Success)
	{
		BDBG_ERR(("%s - Error allocating 16 MByte", __FUNCTION__));
		rc = OEMCrypto_ERROR_OPEN_FAILURE;
		goto ErrorExit;
	}

	*bufferSize = 16*1024*1024;

ErrorExit:
	BDBG_MSG(("%s - Exiting function", __FUNCTION__));
	return rc;
}




/******************************************************************************
** FUNCTION
**   OEMCrypto_Close
**
** DESCRIPTION:
**    Free the memory allocated in OEMCrypto_Open
**
** PARAMETERS:
**   void
**
** RETURNS:
**   OEMCrypto_SUCCESS
**   OEMCrypto_ERROR_CLOSE_FAILED
**
******************************************************************************/
OEMCryptoResult OEMCrypto_Close(void)
{
	OEMCryptoResult rc = OEMCrypto_SUCCESS;

	BDBG_MSG(("%s - Entered function", __FUNCTION__));

	BDBG_MSG(("%s - Exiting function", __FUNCTION__));
	return rc;
}



/******************************************************************************
** FUNCTION
**   OEMCrypto_SetEntitlementKey
**
** DESCRIPTION:
**    Decrypt Emm (or asset key) using the encrypted Device Key found in the keybox.
**
** PARAMETERS:
**   emmKey - pointer to the encrypted assert key
**   emmKeyLength - size of the emm key
**
** RETURNS:
**   OEMCrypto_SUCCESS
**   OEMCrypto_ERROR_NO_DEVICE_KEY: failed to decrypt device key
**   OEMCrypto_ERROR_KEYBOX_INVALID
**   OEMCrypto_ERROR_NO_ASSET_KEY: failed to decrypt asset key
**
******************************************************************************/
OEMCryptoResult OEMCrypto_SetEntitlementKey(
		const OEMCrypto_UINT8* emmKey,
		const OEMCrypto_UINT32 emmKeyLength)
{
	OEMCryptoResult rc = OEMCrypto_SUCCESS;

	BDBG_MSG(("%s - Entered function", __FUNCTION__));

	if(emmKeyLength > MAX_COMMON_KEY_SIZE)
	{
		BDBG_ERR(("%s - Length of key exceeds buffer", __FUNCTION__));
		rc = OEMCrypto_ERROR_NO_DEVICE_KEY;
		goto ErrorExit;
	}

	BKNI_Memcpy(commonKey, emmKey, emmKeyLength);

ErrorExit:
	BDBG_MSG(("%s - Exiting function", __FUNCTION__));
	return rc;
}



/******************************************************************************
** FUNCTION
**   OEMCrypto_DeriveControlWord
**
** DESCRIPTION:
**    using active keyladder from SetEntitlementKey, decrypt ECM,
**    return 4-byte ID and load decrypted content key for future payload decryption
**
** PARAMETERS:
**   ecmKey - pointer to the encrypted content key
**   ecmKeyLength - size of the ecm key
**   flags - to contain 4-byte ID value
**
** RETURNS:
**   OEMCrypto_SUCCESS
**   OEMCrypto_ERROR_NO_CW: failed to load the Control Word
**
******************************************************************************/
OEMCryptoResult OEMCrypto_DeriveControlWord(
		const OEMCrypto_UINT8* ecmKey,
		const OEMCrypto_UINT32 ecmKeyLength,
		OEMCrypto_UINT32* flags)
{
	OEMCryptoResult rc = OEMCrypto_SUCCESS;
	OEMCrypto_UINT8 Id[4] = {0x00};

	BDBG_MSG(("%s - Entered function", __FUNCTION__));

	if(ecmKeyLength > MAX_COMMON_KEY_SIZE)
	{
		BDBG_ERR(("%s - Length of key exceeds length supported", __FUNCTION__));
		rc = OEMCrypto_ERROR_NO_CW;
		goto ErrorExit;
	}

	if(DRM_Widevine_LoadCw(commonKey, (uint8_t*)ecmKey, Id, DrmSecurityKeyType_eOdd) != Drm_Success)
	{
		BDBG_ERR(("%s - Error loading control word", __FUNCTION__));
		rc = OEMCrypto_ERROR_NO_CW;
		goto ErrorExit;
	}

	(*flags) = GET_UINT32_FROM_BUF(Id);
	BDBG_MSG(("%s - flags = 0x%08x", __FUNCTION__, (*flags)));

ErrorExit:
	BDBG_MSG(("%s - Exiting function", __FUNCTION__));
	return rc;
}



/******************************************************************************
** FUNCTION
**   OEMCrypto_Decrypt
**
** DESCRIPTION:
**    Decrypt a buffer using AES-CBC (128bit) algorithm with CTS mode
**
** PARAMETERS:
**   input - input source
**   output - buffer to return clear payload. The size of the output buffer
**            must be >= size of the inputLength
**   inputLength - size of the input (encrypted) source buffer
**   initIvFlag - indicates whether or not to initialize the IV to all zeros
**   				(i.e. 1 = required, 0 = continue)
**
** RETURNS:
**   OEMCrypto_SUCCESS
**   OEMCrypto_ERROR_DECRYPT_FAILED
**
******************************************************************************/
OEMCryptoResult OEMCrypto_Decrypt(
		OEMCrypto_UINT8* input,
		OEMCrypto_UINT8* output,
		const OEMCrypto_UINT32 inputLength,
		OEMCrypto_UINT8 initIvFlag)
{
	OEMCryptoResult rc = OEMCrypto_SUCCESS;
	OEMCrypto_UINT8 tmp_last_16_aligned[16] = {0x00};
	OEMCrypto_UINT8 Iv[16] = {0x00};
	OEMCrypto_UINT8 allZero[16] = {0x00};
	OEMCrypto_UINT32 last_16_byte_aligned_offset = 0;
	DrmDestinationType dest_type = DrmDestinationType_eExternal;

	BDBG_MSG(("%s - Entered function", __FUNCTION__));

	if(input == output){
		dest_type = DrmDestinationType_eInPlace;
	}
	else{
		dest_type = DrmDestinationType_eExternal;
	}

	/* determine 16-byte aligned offset (i.e. if inputLength=35, aligned offset = 16, not 32 since remainder of 3 */
	if(inputLength > 15)
	{
		if(inputLength %16 == 0){
			last_16_byte_aligned_offset = inputLength-16;
		}
		else{
			/* example: if length = 33 then (33-(33%16))-16 = 16 */
			last_16_byte_aligned_offset = (inputLength - (inputLength%16)) - 16;
		}

		/* save the last aligned 16-bytes (in case input address == output address) */
		/* in a temporary buffer so we don't overwrite the 'previousLast16EncAlignedBytes' */
		BDBG_ERR(("%s - last aligned offset = '%u'", __FUNCTION__, last_16_byte_aligned_offset));
		BKNI_Memcpy(tmp_last_16_aligned, &input[last_16_byte_aligned_offset], 16);
	}
	else
	{
		BDBG_WRN(("%s - PACKET LENGTH < 16 bytes!!!!!!!!", __FUNCTION__));
		BDBG_WRN(("%s - PACKET LENGTH < 16 bytes!!!!!!!!", __FUNCTION__));
	}

	/* check initIvFlag */
	if(initIvFlag == 1){
		BDBG_ERR(("%s - all-zero IV loading...", __FUNCTION__));
		BKNI_Memcpy(Iv, allZero, 16);
	}
	else{
		BKNI_Memcpy(Iv, previousLast16EncAlignedBytes, 16);
	}

	/* Decrypt */
	if(DRM_Widevine_Decrypt((uint8_t*) input, (uint32_t) inputLength, (uint8_t*) output,
            				(uint8_t*) Iv, dest_type, DrmSecurityKeyType_eOdd) != Drm_Success)
	{
		BDBG_ERR(("%s - Error decrypting", __FUNCTION__));
		rc = OEMCrypto_ERROR_DECRYPT_FAILED;
		goto ErrorExit;
	}

	/* Overwrite the buffer the last encrypted aligned 16-bytes for use NEXT iteration*/
	BKNI_Memcpy(previousLast16EncAlignedBytes, tmp_last_16_aligned, 16);

ErrorExit:
	BDBG_MSG(("%s - Exiting function", __FUNCTION__));
	return rc;
}



/******************************************************************************
** FUNCTION
**   OEMCrypto_InstallKeybox
**
** DESCRIPTION:
**	Decrypt and load keybox into security processor
**
** PARAMETERS:
**   keybox - pointer to encrypted keybox data
**   keyboxLength - length of keybox data in bytes
**
** RETURNS:
**   OEMCrypto_SUCCESS
**   OEMCrypto_ERROR_WRITE_KEYBOX
**
******************************************************************************/
OEMCryptoResult OEMCrypto_InstallKeybox(
		OEMCrypto_UINT8* keybox,
		OEMCrypto_UINT32 keyboxLength)
{
	OEMCryptoResult rc = OEMCrypto_SUCCESS;
	DRM_Widevine_Init_t inputWvInitStruct;

	/* fetch default path ( ./drm.bin ) */
	inputWvInitStruct.key_file = NULL;
	inputWvInitStruct.mode = DRM_WidevineDecryptMode_eVod;

	BDBG_MSG(("%s - Entered function", __FUNCTION__));
	BDBG_MSG(("keybox[0] = , keyboxLength = %d", keybox[0], keyboxLength));

	if(DRM_Widevine_Initialize(inputWvInitStruct) != Drm_Success)
	{
		BDBG_ERR(("%s - Error initializing Common DRM module", __FUNCTION__));
		rc = OEMCrypto_ERROR_WRITE_KEYBOX;
		goto ErrorExit;
	}

ErrorExit:
	BDBG_MSG(("%s - Exiting function", __FUNCTION__));
	return rc;
}



/******************************************************************************
** FUNCTION
**   OEMCrypto_EncryptAndStoreKeybox (level 3)
**
** DESCRIPTION:
**	Encrypt and store keybox to persistent memory
**
** PARAMETERS:
**   keybox - pointer to encrypted keybox data
**   keyboxLength - length of keybox data in bytes
**
** RETURNS:
**   OEMCrypto_SUCCESS
**   OEMCrypto_ERROR_ENCRYPT_STORE_KEYBOX
**
******************************************************************************/
OEMCryptoResult OEMCrypto_EncryptAndStoreKeybox(
		OEMCrypto_UINT8* keybox,
		OEMCrypto_UINT32 keyboxLength)
{
	OEMCryptoResult rc = OEMCrypto_SUCCESS;

    
    BDBG_MSG(("OEMCrypto_EncryptAndStoreKeybox - Enter"));
	

	CHECK_COMMON_DRM_INITIALIZE();
	
#ifdef DEBUG_MODE	
	OEMCrypto_UINT32 i=0;
	BDBG_MSG(("OEMCrypto_EncryptAndStoreKeybox: Start of data\n"));
	for(i=0;i<keyboxLength;i++)
		BDBG_ERR(("[%d] = 0x%x ", i, keybox[i]));
	BDBG_MSG(("OEMCrypto_EncryptAndStoreKeybox: End of data\n"));
#endif

	if(DRM_SecureStore_AlignedFileEncWrite(BRCM_KEYBOX_L3_PATH, (uint8_t*) keybox, keyboxLength) != Drm_Success)
	{
		BDBG_ERR(("%s - Error encrypting and storing bin file", __FUNCTION__));
		rc = OEMCrypto_ERROR_ENCRYPT_STORE_KEYBOX;
		goto ErrorExit;
	}

ErrorExit:
   
    BDBG_MSG(("OEMCrypto_EncryptAndStoreKeybox - Exit"));
	return rc;
}



/******************************************************************************
** FUNCTION
**   OEMCrypto_isKeyBoxValid
**
** DESCRIPTION:
**	Validates keybox loaded into security processor
**
** PARAMETERS:
**   void
**
** RETURNS:
**   OEMCrypto_SUCCESS
**   OEMCrypto_ERROR_BAD_MAGIC
**   OEMCrypto_ERROR_BAD_CRC
**
******************************************************************************/
OEMCryptoResult OEMCrypto_isKeyBoxValid(void)
{
	OEMCryptoResult rc = OEMCrypto_SUCCESS;

	BDBG_MSG(("%s - Entered function", __FUNCTION__));

	BDBG_MSG(("%s - Exiting function", __FUNCTION__));
	return rc;
}



/******************************************************************************
** FUNCTION
**   OEMCrypto_GetDeviceID
**
** DESCRIPTION:
**	return the Device ID field of the keybox
**
** PARAMETERS:
**   deviceId[out] - pointer to the buffer that receives the Device ID
**   idLength[in/out] - on input, size of the caller's Id buffer
**   			on output, the number of bytes written into the buffer
**
** RETURNS:
**   OEMCrypto_SUCCESS
**   OEMCrypto_ERROR_SHORT_BUFFER: if buffer is too small to return device Id
**   OEMCrypto_ERROR_NO_DEVICEID: failed to return device ID
**
******************************************************************************/
OEMCryptoResult OEMCrypto_GetDeviceID(
		OEMCrypto_UINT8* deviceId,
		OEMCrypto_UINT32* idLength)
{
	OEMCryptoResult rc = OEMCrypto_SUCCESS;

	BDBG_MSG(("%s - Entered function", __FUNCTION__));

	if((*idLength) < DRM_WIDEVINE_DEVICE_ID_SIZE)
	{
		BDBG_ERR(("%s - deviceId buffer too small ('%u' < '%u')", __FUNCTION__, (*idLength), DRM_WIDEVINE_DEVICE_ID_SIZE));
		rc = OEMCrypto_ERROR_SHORT_BUFFER;
		goto ErrorExit;
	}


	if(DRM_Widevine_GetDeviceId((uint8_t*) deviceId) != Drm_Success)
	{
		BDBG_ERR(("%s - Error initializing Common DRM module", __FUNCTION__));
		rc = OEMCrypto_ERROR_NO_DEVICEID;
		goto ErrorExit;
	}

	(*idLength) = strlen((char*)deviceId);
	BDBG_MSG(("%s - Device ID = '%s', length = '%u'", __FUNCTION__));

ErrorExit:
	BDBG_MSG(("%s - Exiting function", __FUNCTION__));
	return rc;
}



/******************************************************************************
** FUNCTION
**   OEMCrypto_IdentifyDevice
**
** DESCRIPTION:
**	Return device unique identifier (not from the keybox)
**
** PARAMETERS:
**   deviceId[out] - pointer to the buffer that receives the unique id
**   idLength[in] - length of unique identifier, max 32 bytes
**
** RETURNS:
**   OEMCrypto_SUCCESS
**   OEMCrypto_ERROR_IDENTIFY_DEVICE
**
******************************************************************************/
#ifdef USE_TEST_DEVICEID
const char *g_string="rhea_device_1283345";
#endif

OEMCryptoResult OEMCrypto_IdentifyDevice(
		OEMCrypto_UINT8* deviceId,
		OEMCrypto_UINT32 idLength)
{
	OEMCryptoResult rc = OEMCrypto_SUCCESS;
	drm_key_binding_t bindingStruct;
	uint8_t digest[32] = {0x00};
	uint8_t allZero[8] = {0x00};
	char outStr[64] = {0x00}; /* hex to string doubles the length */

    CHECK_COMMON_DRM_INITIALIZE();
	
#if USE_TEST_DEVICEID
	 BKNI_Memset(deviceId, 0, idLength);
    BKNI_Memcpy(deviceId, g_string, idLength);
    deviceId[idLength - 1] = '\0';
#else
	
	BKNI_Memset((uint8_t*)&bindingStruct, 0x00, sizeof(drm_key_binding_t));

    BDBG_MSG(("OEMCrypto_IdentifyDevice - Enter (idLen = %d)", idLength));
	
	if(DRM_KeyBinding_FetchDeviceIds(&bindingStruct) != Drm_Success)
	{
		BDBG_ERR(("%s - Error fetching chip device ID", __FUNCTION__));
		rc = OEMCrypto_ERROR_IDENTIFY_DEVICE;

    
		goto ErrorExit;
	}
	
	if(BKNI_Memcmp(allZero, bindingStruct.devIdA, sizeof(bindingStruct.devIdA)) == 0)
	{
		BDBG_ERR(("%s - Chip device ID A all zero", __FUNCTION__));
		rc = OEMCrypto_ERROR_IDENTIFY_DEVICE;

    
		goto ErrorExit;
	}
	
	if(DRM_Common_SwSha256(bindingStruct.devIdA, digest, sizeof(bindingStruct.devIdA)) != Drm_Success)
	{
		BDBG_ERR(("%s - Error fetching chip device ID", __FUNCTION__));
		rc = OEMCrypto_ERROR_IDENTIFY_DEVICE;

    

		goto ErrorExit;
	}
	
	if(DRM_Common_ConvBinToStr(digest,	32,	DrmCommon_CovertFormat_eHex, outStr) != Drm_Success)
	{
		BDBG_ERR(("%s - Error converting to string", __FUNCTION__));
		rc = OEMCrypto_ERROR_IDENTIFY_DEVICE;

    
		goto ErrorExit;
	}
	
	BKNI_Memcpy(deviceId, outStr, idLength);
	
	deviceId[idLength-1] = '\0';
#endif

ErrorExit:
    BDBG_MSG(("OEMCrypto_IdentifyDevice - Exit (deviceId = %s)", deviceId));
	return rc;
}




/******************************************************************************
** FUNCTION
**   OEMCrypto_GetKeyData
**
** DESCRIPTION:
**	Return key data field from KeyBox
**
** PARAMETERS:
**   keyData[out] - pointer to the buffer that receives the Key Data
**   keyDataLength[in/out] - on input, size of the caller's Key Data buffer
**   			on output, the number of bytes written into the buffer
**
** RETURNS:
**   OEMCrypto_SUCCESS
**   OEMCrypto_ERROR_GET_KEY_DATA
**
******************************************************************************/
OEMCryptoResult OEMCrypto_GetKeyData(
		OEMCrypto_UINT8* keyData,
		OEMCrypto_UINT32* keyDataLength)
{
	OEMCryptoResult rc = OEMCrypto_SUCCESS;

	BDBG_MSG(("%s - Entered function", __FUNCTION__));

	if((*keyDataLength) < DRM_WIDEVINE_KEYID_SIZE)
	{
		BDBG_ERR(("%s - keyData buffer too small ('%u' < '%u')", __FUNCTION__, (*keyDataLength), DRM_WIDEVINE_KEYID_SIZE));
		rc = OEMCrypto_ERROR_SHORT_BUFFER;
		goto ErrorExit;
	}

	if(DRM_Widevine_GetKeyId((uint8_t*) keyData) != Drm_Success)
	{
		BDBG_ERR(("%s - Error getting Key Data field value", __FUNCTION__));
		rc = OEMCrypto_ERROR_GET_KEY_DATA;
		goto ErrorExit;
	}

	(*keyDataLength) = DRM_WIDEVINE_KEYID_SIZE;
	BDBG_MSG(("%s - keyDataLength = '%u'", __FUNCTION__, DRM_WIDEVINE_KEYID_SIZE));

ErrorExit:
	BDBG_MSG(("%s - Exiting function", __FUNCTION__));
	return rc;
}



/******************************************************************************
** FUNCTION
**   OEMCrypto_GetKeyboxData (level 3)
**
** DESCRIPTION:
**	 decrypt Keybox and return a range of bytes.
**
** PARAMETERS:
**   buffer - pointer to buffer that will receive data
**   offset - byte offset from the beginning of the keybox of the first byte
**   		  to return
**   length - number of bytes of data to return
**
** RETURNS:
**   OEMCrypto_SUCCESS
**   OEMCrypto_ERROR_GET_KEYBOX_DATA
**
******************************************************************************/
OEMCryptoResult OEMCrypto_GetKeyboxData(
		OEMCrypto_UINT8* buffer,
		OEMCrypto_UINT32 offset,
		OEMCrypto_UINT32 length)
{
	
	OEMCryptoResult rc = OEMCrypto_SUCCESS;
	uint8_t *meta_buf = NULL;
    struct stat info;

    
    BDBG_MSG(("OEMCrypto_GetKeyboxData - Enter"));
	BDBG_MSG(("buffer = '%u, offset  = '%u, length = '%u'", buffer, offset, length));

	CHECK_COMMON_DRM_INITIALIZE();
	
	if(stat(BRCM_KEYBOX_L3_PATH, &info) != 0){
    	BDBG_ERR(("%s - Error reading file '%s'", __FUNCTION__, BRCM_KEYBOX_L3_PATH));
        rc = Drm_InvalidParameter;
		goto ErrorExit;
    }
		
	BDBG_MSG(("%s - Allocating '%u' bytes", __FUNCTION__, info.st_size));

	if(DRM_Common_MemoryAllocate(&meta_buf, info.st_size) != Drm_Success)
	{
		BDBG_ERR(("%s - Error allocating '%u' bytes", __FUNCTION__, info.st_size));
		rc = OEMCrypto_ERROR_MEMORY;
    	goto ErrorExit;
	}


	if(DRM_SecureStore_AlignedFileDecRead(BRCM_KEYBOX_L3_PATH, meta_buf, ((uint32_t*)&info.st_size)) != Drm_Success)
	{
		BDBG_ERR(("%s - Error decrypting file '%s'", __FUNCTION__, BRCM_KEYBOX_L3_PATH));
		rc = OEMCrypto_ERROR_POINTER;
    	goto ErrorExit;
	}

	if(offset+length > 128)
	{
		BDBG_ERR(("%s - Offset overrun (%u, %u)", __FUNCTION__, offset, length));
		rc = OEMCrypto_ERROR_SHORT_BUFFER;
		goto ErrorExit;
	}

	

	BDBG_MSG(("%s - Copying '%u' bytes starting from offset '%u'", __FUNCTION__, length, offset));
	BKNI_Memcpy(buffer, &meta_buf[offset], length);

#ifdef DEBUG_MODE
	BDBG_MSG(("OEMCrypto_GetKeyboxData: Start of data\n"));
    int i;
	for(i=0;i<length;i++)
		BDBG_MSG(("[%d] = 0x%x ", i, buffer[i]));
	BDBG_MSG(("OEMCrypto_GetKeyboxData: End of data\n"));
#endif

ErrorExit:

	if(meta_buf != NULL){
		DRM_Common_MemoryFree(meta_buf);
	}


    BDBG_MSG(("OEMCrypto_GetKeyboxData - Exit"));
	return rc;
}




/******************************************************************************
** FUNCTION
**   OEMCrypto_GetRandom
**
** DESCRIPTION:
**	 Fill a buffer with HW generated bytes
**
** PARAMETERS:
**   randomData - buffer to store the random data
**   dataLength - length of random data buffer in bytes
**
** RETURNS:
**   OEMCrypto_SUCCESS
**   OEMCrypto_ERROR_RNG_FAILED
**   OEMCrypto_ERROR_RNG_NOT_SUPPORTED
**
******************************************************************************/
OEMCryptoResult OEMCrypto_GetRandom(
		OEMCrypto_UINT8* randomData,
		OEMCrypto_UINT32 dataLength)
{
	OEMCryptoResult rc = OEMCrypto_SUCCESS;

    

    BDBG_MSG(("OEMCrypto_GetRandom - Enter"));

	CHECK_COMMON_DRM_INITIALIZE();
	
	if(DRM_Common_GenerateRandomNumber((uint32_t)dataLength, (uint8_t*)randomData) != Drm_Success)
	{
		BDBG_ERR(("%s - Error generating random bytes", __FUNCTION__));
		rc = OEMCrypto_ERROR_RNG_FAILED;
		goto ErrorExit;
	}

ErrorExit:
	BDBG_MSG(("OEMCrypto_GetRandom - Exit"));
	return rc;
}

