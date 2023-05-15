/*
 * tpiStorageNova.c
 *
 *  Created on: Mar 19, 2014
 *      Author: scott
 */
/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/*
 * This is an implementation of the storage TPI code for the PS4.
 * At a high level, this file needs to provide functions to populate
 * saManufSSTable[], and to implement client secure store functions.
 */
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <strings.h>
#include "DebugTypes.h"
#include "CommTypes.h"
#include "StorageTypes.h"
#include "TeeInternalDpi.h"
#include "TeeDebug.h"
#include "StorageAgent.h"
#include "StorageAgentTPI.h"
#include "b64.h"
#include "tpiStorage.h"


/*
 * This is the key ladder base for application services offered by storageAgent. We
 * use this, together with our secure store keys, to derive encryption and HMAC keys
 * for each application. Note that we hold this key in memory, but it could also
 * be loaded directly into a crypto processor.
 *
 * In a "real" implementation, this key would be directly loaded from OTP, or
 * derived from a device-unique key that is loaded from OTP. It is very important
 * that this key be device-unique. Otherwise, it would be possible to migrate an
 * application secure store from one device to another.
 *
 * For the Nova implementation, we are hard-coding this, but we may want to replace
 * this with a whitebox
 */
uint8_t saKeyLadderBase[] = {0xF1,0xE0,0xEE,0x00,0xF3,0x1A,0x04,0x1F,
		                     0xAF,0x21,0x32,0xB1,0xB6,0x98,0x00,0x11};

uint8_t _SecStoreAesKey[] = {0x11,0x89,0xE3,0xF0,0x13,0x99,0x20,0x76,
								0xF1,0x27,0x92,0xBD,0xBB,0x12,0xB9,0x9A};

/*
 * Our implementation of storageAgent has its own device unique keys. In a "real"
 * implementation, we would probably re-name these variables to something less obvious, and
 * consider obfuscating the fact that these are crypto keys. Reverse engineers load binaries
 * into tools like IDA Pro, and then look for informative variable names like these. So at
 * minimum, you should strip your binaries.
 */
uint8_t saInternalKey[saAES_128_KEY_SIZE_BYTES];
uint8_t saInternalAesKey[saAES_128_KEY_SIZE_BYTES];
uint8_t saInternalHMACKey[saHMAC_SHA256_KEY_SIZE_BYTES];

/*
 *  We use the client ID to limit access to data items. Since the TEE comm agent
 *  controls the client ID, we can trust it to accurately report the client ID to
 *  us. Even if the REE is compromised, it cannot spoof a client ID.
 *
 */
static nfTeeClientId_t cryptoAgentClientId = nfSS_CLIENT_ID_TEE_NTBA_RO;

/*
 *  NRDApp Client Id
 */
static uint8_t nrdAppClientId[] = nfSS_CLIENT_ID_NRD_APP;

/*
 * We can't assume that we have dynamic memory allocation capability (e.g. malloc),
 * so we pre-allocated a work buffer for crypto operations. If your implementation
 * supports malloc, you could use dynamic allocation instead.
 */
static uint8_t WorkBuf[nfTEE_MAX_INTERNAL_SIZE];

/*
 * The following definition relates to how we protect/unprotect data. "Protect" is
 * our name for the act of providing client "secure store" services. For example, the
 * NRD application needs to protect data at rest (e.g. MSL tokens, the UI trust store, etc.)
 *
 * We use a proprietary "wrapper" for this data. Version is prepended to protected data so
 * that we can apply proper algorithms, formatting, etc. when unprotecting. If we don't recognize
 * (or implement) the version, we won't attempt to decrypt the data.
 */
static uint32_t	saStorageVersion = 0x10000001;

extern int nfGetNetflixCredentials( uint8_t Esn[], uint32_t *EsnSizePtr,
        							uint8_t   Kde[], uint32_t *KdeSizePtr,
        							uint8_t   Kdh[], uint32_t *KdhSizePtr );

#define saMAX_ESN_SIZE	40
#define saKDE_SIZE		16
#define saKDH_SIZE		32
#define saKDW_SIZE		16

static uint8_t Esn[saMAX_ESN_SIZE + 1];
static uint8_t Kde[saKDE_SIZE];
static uint8_t Kdh[saKDH_SIZE];

static uint8_t saLocalKDWStorage[saKDW_SIZE];

/*********************************************************************
 * @function saDeriveKdw
 *
 * @brief RFC5869 Kdw derivation function using SHA256
 *
 * @param[in] Kde - preshared netflix encryption key
 * @param[in] Kdh - preshared netflix hmac key
 * @param[out] Kdw - buffer for Kdw
 * @param[in] KdwMaxLength - max length of Kdw
 *
 *
 * @returns length of stored Kdw on success, -1 on failure
 *
 *********************************************************************/
static int saDeriveKdw( uint8_t Kde[], uint8_t Kdh[], uint8_t Kdw[], uint32_t KdwMaxLength )
{
	int		rc;

#define saKDW_SIZE 16
	uint8_t Salt[saKDW_SIZE] = {0x02, 0x76, 0x17, 0x98, 0x4f, 0x62, 0x27, 0x53,
							   0x9a, 0x63, 0x0b, 0x89, 0x7c, 0x01, 0x7d, 0x69};
	uint8_t Info[saKDW_SIZE] = {0x80, 0x9f, 0x82, 0xa7, 0xad, 0xdf, 0x54, 0x8d,
							   0x3e, 0xa9, 0xdd, 0x06, 0x7f, 0xf9, 0xbb, 0x91 };
#define saSHA256_LEN 32
	uint8_t FirstResult[saSHA256_LEN]; // HMAC-SHA256(salt, Kde||Kdh)
	uint8_t SecondResult[saSHA256_LEN]; // HMAC-SHA256(FirstResult, info))

#define saCONCAT_KEY_LEN (saKDE_SIZE + saKDH_SIZE) // concatentate Kde|Kdh
	uint8_t	ConcatKeys[saCONCAT_KEY_LEN];

	/* Kdw = trunc_128(HMAC-SHA256(HMAC-SHA256(salt, Kde||Kdh), info)) */

	// no nonsense.
	assert( KdwMaxLength >= saKDW_SIZE );
	assert(Kde != NULL);
	assert(Kdh != NULL);
	assert(Kdw != NULL);

	// concatenate Kde|Kdh
	(void) teeMemcpy(ConcatKeys, Kde, saKDE_SIZE);
	(void) teeMemcpy(ConcatKeys + saKDE_SIZE, Kdh, saKDH_SIZE);

	// compute FirstResult: key is Salt[], data is concatenated keys
	rc = teeHmacSha256( ConcatKeys, saCONCAT_KEY_LEN, Salt, saKDW_SIZE, FirstResult );
	if ( rc != saSHA256_LEN )
	{
		STORAGEDBG("%s: teeHmacSha256(FirstResult) failed, returned %d\n", __FUNCTION__, rc );
		return -1;
	}

	// compute SecondResult: key is FirstResult, data is Info[]
	rc = teeHmacSha256( Info, saKDW_SIZE, FirstResult, saSHA256_LEN, SecondResult );
	if ( rc != saSHA256_LEN )
	{
		STORAGEDBG("%s: teeHmacSha256(SecondResult) failed, returned %d\n", __FUNCTION__, rc );
		return -1;
	}

	// copy first 128 bits into Kdw if there is room
	if ( KdwMaxLength <= saKDW_SIZE )
		(void) teeMemcpy(Kdw, SecondResult, saKDW_SIZE );

	// also, save copy
	(void) teeMemcpy(saLocalKDWStorage, SecondResult, saKDW_SIZE );

	STORAGEDBG("%s: dumping derived Kdw:\n", __FUNCTION__ );
	//teePrintHexDump( saLocalKPWStorage, saKDW_SIZE );

	return saKDW_SIZE;

}/* end saDeriveKdw */


/*********************************************************************
 * @function saGetAppKeys
 *
 * @brief gets AES/Hmac keys for a given client ID
 *
 * @param[in] ClientId - ID for client who made request
 * @param[out] AesKeyBuf - buffer for AES key
 * @param[out] HmacKeyBuf - buffer for HmacSha256 key
 *
 * NB: Assumes 128-bit AES key and 256-bit HMAC key; make sure your buffers
 *     are big enough...
 *
 * @return nfTGR_SUCCESS if everything okay, appropriate error otherwise
 *
 *********************************************************************/
static nfTeeStorageResult_t saGetAppKeys( nfTeeClientId_t 	ClientId,
										  uint8_t 			AesKey[],
										  uint8_t 			HmacKey[] )
{
	uint8_t	TempBuf[saAES_128_KEY_SIZE_BYTES + saHMAC_SHA256_KEY_SIZE_BYTES];
	int		rc;

	/*
	 * Compute application-specific keys
	 */

	// first, encrypt saKeyLadderBase; this becomes saInternalKey
	(void) teeMemcpy( AesKey, ClientId, saAES_128_KEY_SIZE_BYTES );
	rc = teeAesCbc128EncryptZeroIV( AesKey,
			saAES_128_KEY_SIZE_BYTES, saInternalAesKey );
	if ( rc != 0 )
	{
		CRYPTODBG("%s: failed to create app key ladder base!?\n", __FUNCTION__);
		return nfTSR_INTERNAL_ERROR;
	}

	if ( ( rc = teeSha384(AesKey, saAES_128_KEY_SIZE_BYTES, TempBuf) ) != 0 )
	{
		CRYPTODBG("%s: teeSha384() failed!?\n", __FUNCTION__);
		return nfTSR_INTERNAL_ERROR;
	}

	// first 128 bits is AES key, next 256 is HMAC key
	(void)teeMemcpy( AesKey, TempBuf, saAES_128_KEY_SIZE_BYTES );
	(void)teeMemcpy( HmacKey, TempBuf + saAES_128_KEY_SIZE_BYTES, saHMAC_SHA256_KEY_SIZE_BYTES );

	// don't leave key material laying around...
	(void) teeBzero(TempBuf, sizeof(TempBuf));

	return (nfTeeStorageResult_t) nfTGR_SUCCESS;

}/* end saGetAppKeys */

/*********************************************************************
 * @function saTpiGetTAKeyLadderBase
 *
 * @brief creates device-unique key ladder base for a given (trusted) client ID
 *
 * @param[in] ClientId - ID for client who made request
 * @param[out] Buf - buffer for key ladder
 * @param[in] BufSize - size of buffer
  * @param[out] ActualSizePtr - pointer to where you output size
 *
 * @return nfTGR_SUCCESS if everything okay, appropriate error otherwise
 *
 *********************************************************************/
nfTeeStorageResult_t saTpiGetTAKeyLadderBase( nfTeeClientId_t 	ClientId,
										  	  uint8_t 			Buf[],
										  	  uint32_t 			BufSize,
										  	  uint32_t			*ActualSizePtr )
{
	int rc;

	/*
	 * NB: we currently only support this for cryptoAgent, so make
	 * sure it's the right clientId
	 */
	if ( teeMemcmp(cryptoAgentClientId, ClientId, sizeof(nfTeeClientId_t)) )
	{
		// they don't match, this is a security violation.
		STORAGEDBG("%s: wrong client ID.\n", __FUNCTION__);
		return nfTSR_NOT_FOUND;
	}

	// okay, generate the key. Remember, encrypts in place...

	if ( BufSize < sizeof(nfTeeClientId_t) )
	{
		STORAGEDBG("%s: passed buffer is too small (%d/%d)", __FUNCTION__,
					BufSize, sizeof(nfTeeClientId_t) );
		return nfTSR_BUFFER_TOO_SHORT;
	}

	teeMemcpy( Buf, ClientId, sizeof(nfTeeClientId_t) );
	rc = teeAesCbc128EncryptZeroIV( Buf, sizeof(nfTeeClientId_t), saKeyLadderBase);
	if ( rc != 0 )
	{
		STORAGEDBG("%s: failed to create internal key for cryptoAgent\n", __FUNCTION__ );
		// fatal
		assert(0);
	}

	STORAGEDBG("%s: successfully created cryptoAgent ladder key.\n", __FUNCTION__);
	*ActualSizePtr = sizeof(saKeyLadderBase);

	return (nfTeeStorageResult_t) nfTGR_SUCCESS;


}/* end saTpiGetKeyLadderBase */

/**************************************************
 * @function saTpiLoadNflxIdFile
 *
 * @brief storage agent TPI function to load Netflix idfile
 *
 * @param[in] ClientId - ID of client who can access these
 * @param[in] DataPtr - pointer to idfile data
 * @param[in] DataLength - length of data
 *
 * @return 0 if successful, -1 otherwise
 *
 *********************************************************************/
nfTeeStorageResult_t saTpiLoadNflxIdFile( nfTeeClientId_t ClientId, uint8_t *DataPtr, uint32_t DataLength )
{
	(void) ClientId;
	(void) DataPtr;
	(void) DataLength;

	STORAGEDBG("%s: this function is unimplemented\n",__FUNCTION__ );
	assert(0);
	return nfTSR_INTERNAL_ERROR;
}/* end saTpiLoadNflxIdFile */

/**************************************************
 * @function saTpiLoadDrmCredentials
 *
 * @brief   storage agent TPI function to load DRM credentials
 *
 * @param[in] ClientId - ID of client who can access these
 * @param[in] DataPtr - pointer to credential data
 * @param[in] DataLength - length of data
 *
 * @return 0 if successful, -1 otherwise
 *
 *********************************************************************/
nfTeeStorageResult_t saTpiLoadNflxDrmCredentials( nfTeeClientId_t ClientId, uint8_t *DataPtr, uint32_t DataLength )
{
	(void) ClientId;
	(void) DataPtr;
	(void) DataLength;

	STORAGEDBG("%s: this function is unimplemented\n", __FUNCTION__ );
	assert(0);
	return nfTSR_INTERNAL_ERROR;
}

/**************************************************
 * @function saTpiProtectData
 *
 * @brief   storage agent TPI function to protect application data
 *
 * @param[in] ClientId - ID of client requesting operation
 * @param[in] DataPtr - pointer to data that you want protected
 * @param[in] DataLength - length of unprotected data
 * @param[out] ProtectedDataPtr - pointer to preallocated buffer for protected data
 * @param[in] MaxProtectedDataLength - size of buffer at ProtectedDataPtr
 * @param[out] ProtectedDataLengthPtr - actual output length will be stored here.
 *
 * @return 0 if successful, -1 otherwise
 *
 *********************************************************************/
nfTeeStorageResult_t saTpiProtectData( nfTeeClientId_t	ClientId,
					  	  	  	  	   uint8_t 			*DataPtr,
					  	  	  	  	   uint32_t 		DataLength,
									  uint8_t 			*ProtectedDataPtr,
									  uint32_t 			MaxProtectedDataLength,
									  uint32_t 			*ProtectedDataLengthPtr )
{
	uint32_t 					ResponseCode;
	uint32_t					TotalLength;
	uint8_t						AppAesKey[saAES_128_KEY_SIZE_BYTES];
	uint8_t						AppHmacKey[saHMAC_SHA256_KEY_SIZE_BYTES];
	uint8_t						*BytePtr;
	uint8_t						*IVPtr;
	int							CiphertextLength;
	uint32_t					HmacDataLength;
	int							HmacLength;
	int							B64Length;

	/*
	 * This is not as easy as it looks. We need to do the following:
	 *
	 *  - compute final length, puke if it won't fit
	 * 	- get the app keys for this client
	 * 	- generate (random) IV
	 * 	- encrypt (padding if need be) the data
	 * 	- prepend version and IV
	 * 	- compute HMAC over version, IV, and encrypted data
	 * 	- append HMAC
	 * 	- b64-encode result into ProtectedDataPtr
	 * 	- update *ProtectedLengthPtr
	 */

	TotalLength = sizeof(saStorageVersion) + saAES_128_KEY_SIZE_BYTES
					+ saHMAC_SHA256_KEY_SIZE_BYTES + DataLength;

	if ( DataLength % 16 != 0 )
		TotalLength += saAES_128_KEY_SIZE_BYTES - (DataLength % 16);

	if ( TotalLength > (uint32_t) nfTEE_MAX_PROTECT_SIZE )
	{
		STORAGEDBG("%s: data too large (%d)\n", __FUNCTION__, TotalLength );
		return nfTSR_CHUNK_SIZE_EXCEEDED;
	}

	if ( ( ResponseCode = saGetAppKeys( ClientId, AppAesKey, AppHmacKey ) ) != nfTGR_SUCCESS )
	{
		STORAGEDBG("%s: failed to get app keys, bailing.\n", __FUNCTION__);
		return ResponseCode;
	}

	// start assembling the data
	BytePtr = WorkBuf;
	*((uint32_t *)BytePtr) = saStorageVersion;
	BytePtr += sizeof(uint32_t);

	// get IV into WorkBuf
	if ( ! teeCryptoRandomBytes( BytePtr, saAES_128_KEY_SIZE_BYTES ) )
	{
		// this is fatal
		STORAGEDBG("%s: failed to allocate random bytes for key seal IV\n", __FUNCTION__ );
		return nfTSR_CRYPT_RANDOM_FAILURE;
	}

	// save pointer to IV, advance BytePtr beyond IV
	IVPtr = BytePtr;
	BytePtr += saAES_128_KEY_SIZE_BYTES;

	/*
	 *  Now, put encrypted data at BytePtr. Note that we can fudge the max size parm
	 *  for the crypto call because we already know we have more than enough space in
	 *  Workbuf (yeah, yeah, I'm being lazy...)
	 */
	CiphertextLength = teeAes128CbcEncrypt( DataPtr, DataLength, AppAesKey, IVPtr, BytePtr,
									DataLength + saAES_128_KEY_SIZE_BYTES );

	if ( CiphertextLength <= 0 )
	{
		STORAGEDBG("%s: encrypt failure, bailing.", __FUNCTION__);
		return nfTSR_CIPHER_ERROR;
	}

	BytePtr += CiphertextLength;

	// how much data to HMAC?
	HmacDataLength = (uint32_t) (BytePtr - WorkBuf);

	// compute HMAC, store at BytePtr
	HmacLength = teeHmacSha256( WorkBuf, HmacDataLength, AppHmacKey,
								saHMAC_SHA256_KEY_SIZE_BYTES, BytePtr );

	CRYPTODBG("%s: back from teeHmacSha256, HmacLength is %d\n", __FUNCTION__, HmacLength );

	if ( HmacLength != saHMAC_SHA256_KEY_SIZE_BYTES )
	{
		STORAGEDBG("%s: HMAC failed (length: %d)\n", __FUNCTION__, HmacLength );
		return nfTSR_CIPHER_ERROR;
	}

	//  make sure there is room for b64-encoded data
	if ( ( B64Length = Base64encode_len(HmacDataLength + HmacLength)) > (int) MaxProtectedDataLength )
	{
		CRYPTODBG("%s: not enough room at protected data (%d/%d)\n", __FUNCTION__, MaxProtectedDataLength,
				Base64encode_len(HmacDataLength + HmacLength) );
		return nfTSR_OPERAND_TOO_SHORT;
	}

	STORAGEDBG("%s: B64Length is %d, MaxProtectedDataLength is %d\n", __FUNCTION__,
			 	 B64Length, MaxProtectedDataLength );

	// okay, do the encoding
	B64Length = Base64encode( (char *) ProtectedDataPtr,
			(const char *) WorkBuf, HmacDataLength + HmacLength );

	assert(B64Length > 0);

	*ProtectedDataLengthPtr = (uint32_t) B64Length;

	// okay, we're good.
	return (nfTeeStorageResult_t) nfTGR_SUCCESS;

}/* end saTpiProtectData */


/**************************************************
 * @function saTpiUnrotectData
 *
 * @brief   storage agent TPI function to protect application data
 *
 * @param[in] ClientId - ID of client requesting operation
 * @param[in] ProtectedDataPtr - pointer to protected data
 * @param[in] ProtectedDataLength - length of protected data
 * @param[out] DataPtr - pointer to preallocated buffer for unprotected data
 * @param[in] MaxDataLength - size of buffer at DataPtr
 * @param[out] UnprotectedDataLengthPtr - actual output length will be stored here.
 *
 * @return 0 if successful, -1 otherwise
 *
 *********************************************************************/
nfTeeStorageResult_t saTpiUnprotectData( nfTeeClientId_t 	ClientId,
					    				 uint8_t 			*ProtectedDataPtr,
					    				 uint32_t 			ProtectedDataLength,
					    				 uint8_t 			*DataPtr,
										 uint32_t 			MaxDataLength,
										 uint32_t 			*UnprotectedDataLengthPtr )
{
	uint32_t 		ResponseCode;
	uint32_t		TotalLength;
	uint8_t			AppAesKey[saAES_128_KEY_SIZE_BYTES];
	uint8_t			AppHmacKey[saHMAC_SHA256_KEY_SIZE_BYTES];
	uint8_t			HmacValue[saHMAC_SHA256_KEY_SIZE_BYTES];
	uint8_t			*BytePtr;
	uint8_t			*IVPtr;
	int				CiphertextLength;
	int				CleartextLength;
	uint32_t		HmacDataLength;
	int				HmacLength;

	/*
	 * We need to do the following:
	 *
	 *  - b64-decode the protected data
	 *  - sanity-check length, version
	 *  - compute worst-case final length, puke if it won't fit
	 * 	- get the app keys for this client
	 *  - validate HMAC
	 * 	- decrypt the data
	 */

	// XXX - make sure b64 data is NULL-terminated?

	// make sure decoded data will fit in workbuf
	if ( ( TotalLength = Base64decode_len((const char *)ProtectedDataPtr) ) > (int) sizeof(WorkBuf) )
	{
		STORAGEDBG("%s: can't fit decoded key into work buffer (%d/%d)\n", __FUNCTION__,
				TotalLength, sizeof(WorkBuf) );
		return nfTSR_CHUNK_SIZE_EXCEEDED;
	}

	// sanity-check total length
	if ( ProtectedDataLength <= TotalLength )
	{
		STORAGEDBG("%s: ProtectedDataLength is too short?! (%d/%d)\n", __FUNCTION__,
					ProtectedDataLength, TotalLength );
		return nfTSR_INTERNAL_ERROR;
	}

	// b64-decode protected data into workbuf
	if ( ( TotalLength = Base64decode((char *)WorkBuf, (const char *)ProtectedDataPtr) ) <= 0 )
	{
		STORAGEDBG("%s: Base64decode returned %d\n", __FUNCTION__, TotalLength );
		return nfTSR_INTERNAL_ERROR;
	}

	/*
	 * Sanity-check length; we know it should be version + IV + encr data + HMAC value, and because
	 * we use AES-128-CBC, encrypted data is at least the size of the key (the block size).
	 */
	if ( TotalLength < sizeof(saStorageVersion) + saAES_128_KEY_SIZE_BYTES
						+ saAES_128_KEY_SIZE_BYTES + saHMAC_SHA256_KEY_SIZE_BYTES )
	{
		// this can't be valid data.
		STORAGEDBG("%s: Base64decode returned %d, this is too small.\n", __FUNCTION__, TotalLength );
		return nfTSR_OPERAND_TOO_SHORT;
	}

	// check the version
	BytePtr = WorkBuf;
	if ( *((uint32_t *)BytePtr) != saStorageVersion )
	{
		// format error
		STORAGEDBG("%s: unrecognized version (%08x); TotalLength: %d.\n", __FUNCTION__,
					*((uint32_t *)BytePtr), TotalLength );
		//teePrintHexDump( WorkBuf, TotalLength );
		return nfTSR_UNRECOGNIZED_VERSION;
	}

	STORAGEDBG("%s: version is okay, getting app keys...\n", __FUNCTION__);

	BytePtr += sizeof(uint32_t);

	// get the app keys for this client
	if ( ( ResponseCode = saGetAppKeys( ClientId, AppAesKey, AppHmacKey ) ) != nfTGR_SUCCESS )
	{
		STORAGEDBG("%s: failed to get app keys, bailing.\n", __FUNCTION__);
		return ResponseCode;
	}

	STORAGEDBG("%s: got app keys...\n", __FUNCTION__);

	// save pointer to IV, advance BytePtr beyond IV
	IVPtr = BytePtr;
	BytePtr += saAES_128_KEY_SIZE_BYTES;

	// Verify HMAC
	HmacDataLength = (uint32_t) (TotalLength - saHMAC_SHA256_KEY_SIZE_BYTES);

	STORAGEDBG("%s: HmacDataLength: %d.\n", __FUNCTION__, HmacDataLength);
	// compute HMAC, store at BytePtr
	HmacLength = teeHmacSha256( WorkBuf, HmacDataLength, AppHmacKey,
								saHMAC_SHA256_KEY_SIZE_BYTES, HmacValue );

	STORAGEDBG("%s: back from teeHmacSha256, HmacLength is %d\n", __FUNCTION__, HmacLength );

	if ( HmacLength != saHMAC_SHA256_KEY_SIZE_BYTES )
	{
		STORAGEDBG("%s: HMAC failed (length: %d)\n", __FUNCTION__, HmacLength );
		return nfTSR_CIPHER_ERROR;
	}

	// compare calculated value to actual value
	if ( teeMemcmp( HmacValue, WorkBuf + HmacDataLength, saHMAC_SHA256_KEY_SIZE_BYTES ) != 0 )
	{
		STORAGEDBG("%s: HMAC failure on protected data\n", __FUNCTION__);
		return nfTSR_CIPHER_ERROR;
	}

	/*
	 * Okay, data is valid, so decrypt into passed buffer. Remember, IVPtr
	 * is pointing to IV, BytePtr is pointing to encrypted data. We need
	 * ciphertext length. It is TotalLength - (version + IV + HMAC).
	 */

	CiphertextLength = TotalLength - (sizeof(saStorageVersion) + saAES_128_KEY_SIZE_BYTES
			                           + saHMAC_SHA256_KEY_SIZE_BYTES );

	CleartextLength = teeAes128CbcDecrypt( BytePtr, CiphertextLength, AppAesKey,
										   IVPtr, DataPtr, MaxDataLength );

	if ( CleartextLength <= 0 )
	{
		STORAGEDBG("%s: decrypt failure, bailing.", __FUNCTION__);
		return nfTSR_CIPHER_ERROR;
	}

	*UnprotectedDataLengthPtr = (uint32_t) CleartextLength;
#if 0
	STORAGEDBG("%s: here's a hexdump of ClearDataPtr:\n", __FUNCTION__);
	teePrintHexDump(ClearDataPtr, CleartextLength);
#endif
	// okay, we're good.
	return (nfTeeStorageResult_t) nfTGR_SUCCESS;

}/* end saTpiUnprotectData */

/**************************************************
 * @function saTpiInit
 *
 * @brief   storage agent TPI initialization
 *
 * @param[in] InitDataPtr - pointer to initialization data
 * @param[in] InitDataLength - length of initialization data
 * @param[out] StatePtr - pointer to where you want storage agent state
 *
 * @return nfTGR_SUCCESS if initialized, error otherwise
 *
 *********************************************************************/
nfTeeStorageResult_t saTpiInit( uint8_t 				*InitDataPtr,
								uint32_t 				InitDataLength,
								nfStorageAgentState_t	*StatePtr )
{
	int 		rc;
	uint32_t	EsnSize = saMAX_ESN_SIZE + 1;
	uint32_t	KdeSize = saKDE_SIZE;
	uint32_t	KdhSize = saKDH_SIZE;

	(void) InitDataPtr;
	(void) InitDataLength;

	/*
	 * We need to do the following:
	 *
	 * 	- initialize saManufSSTable
	 * 		- load ESN, Kde, Kdh
	 * 		- compute Kdw and load
	 * 	- initialize app service
	 */

	// load Esn/Kde/Kdh
	rc = nfGetNetflixCredentials( Esn, &EsnSize, Kde, &KdeSize, Kdh, &KdhSize );
	if ( rc != 0 )
	{
		STORAGEDBG("%s: nfGetNeflixCredentials failed (rc: %d).\n", __FUNCTION__, rc );
		return nfTSR_INTERNAL_ERROR;
	}

	if ( (KdeSize != (uint32_t)saKDE_SIZE) || (KdhSize != (uint32_t)saKDH_SIZE))
	{
		STORAGEDBG("%s: invalid Kde/Kdh size (%d/%d)\n", __FUNCTION__, KdeSize, KdhSize);
		return nfTSR_INTERNAL_ERROR;
	}

	// Store Esn in table
	saManufSSTable[nfSSRI_ESN].DataLength = EsnSize;
	saManufSSTable[nfSSRI_ESN].DataPtr = Esn;
	saManufSSTable[nfSSRI_ESN].ClientKey = ntbaAGENT_KEY_ESN;
	saManufSSTable[nfSSRI_ESN].ClientIdPtr = &cryptoAgentClientId;

	// Store Kde/Kdh in table
	saManufSSTable[nfSSRI_KPE].DataLength = saKDE_SIZE;
	saManufSSTable[nfSSRI_KPE].DataPtr = Kde;
	saManufSSTable[nfSSRI_KPE].ClientKey = ntbaAGENT_KEY_KPE;
	saManufSSTable[nfSSRI_KPE].ClientIdPtr = &cryptoAgentClientId;

	saManufSSTable[nfSSRI_KPH].DataLength = saKDH_SIZE;
	saManufSSTable[nfSSRI_KPH].DataPtr = Kdh;
	saManufSSTable[nfSSRI_KPH].ClientKey = ntbaAGENT_KEY_KPH;
	saManufSSTable[nfSSRI_KPH].ClientIdPtr = &cryptoAgentClientId;

	// Store NRD App ID
	saManufSSTable[nfSSRI_NRD_APP_ID].DataLength = sizeof(nrdAppClientId);
	saManufSSTable[nfSSRI_NRD_APP_ID].DataPtr = nrdAppClientId;
	saManufSSTable[nfSSRI_NRD_APP_ID].ClientKey = teeAPPID_KEY_NRD;
	saManufSSTable[nfSSRI_NRD_APP_ID].ClientIdPtr = &cryptoAgentClientId;

	/*
	 * Derive Kdw from Kde/Kdh
	 */
	saManufSSTable[nfSSRI_KPW].DataLength = saDeriveKdw( Kde, Kdh, saLocalKDWStorage, (uint32_t) saKDW_SIZE );
	if ( saManufSSTable[nfSSRI_KPW].DataLength > 0 )
	{
		saManufSSTable[nfSSRI_KPW].DataPtr = saLocalKDWStorage;
		saManufSSTable[nfSSRI_KPW].ClientKey = ntbaAGENT_KEY_KPW;
		saManufSSTable[nfSSRI_KPW].ClientIdPtr = &cryptoAgentClientId;
	}
	else
	{
		STORAGEDBG("%s: failed to derive Kpw, bailing.\n", __FUNCTION__);
		return nfTSR_KEY_DERIV_ERROR;
	}

	/*
     * Derive key ladder for app storage services
     */
	/*
	* caInternalKey is our key ladder key. We will use it to derive our internal
	* AES and HMAC-SHA256 keys.
	*/
	{/* begin local scope limiter */
	   uint8_t TempBuf[saAES_128_KEY_SIZE_BYTES + saHMAC_SHA256_KEY_SIZE_BYTES];
	   int rc;

		// first, encrypt saKeyLadderBase; this becomes saInternalKey
		(void) teeMemcpy( saInternalKey, saKeyLadderBase, sizeof(saInternalKey) );
		rc = teeAesCbc128EncryptZeroIV( saInternalKey,
				sizeof(saInternalKey), _SecStoreAesKey );
		if ( rc != 0 )
		{
			CRYPTODBG("%s: failed to create key ladder base!?\n", __FUNCTION__);
			// fatal
			assert(0);
		}

		if ( ( rc = teeSha384(saInternalKey, saAES_128_KEY_SIZE_BYTES, TempBuf) ) != 0 )
		{
			CRYPTODBG("%s: teeSha384() failed!?\n", __FUNCTION__);
			// fatal
			assert(0);
		}
		// first 128 bits is AES key, next 256 is HMAC key
		(void)teeMemcpy( saInternalAesKey, TempBuf, saAES_128_KEY_SIZE_BYTES );
		(void)teeMemcpy( saInternalHMACKey, TempBuf + saAES_128_KEY_SIZE_BYTES, saHMAC_SHA256_KEY_SIZE_BYTES );

		CRYPTODBG("%s: dumping ladder rungs...\n", __FUNCTION__);
		//teePrintHexDump( TempBuf, 48 );

		// clear other keys from memory
		(void) teeBzero(saInternalKey, saAES_128_KEY_SIZE_BYTES);
		(void) teeBzero(TempBuf, saAES_128_KEY_SIZE_BYTES + saHMAC_SHA256_KEY_SIZE_BYTES);
		CRYPTODBG("%s: created internal keys.\n", __FUNCTION__);

	}/* end local scope limiter */

	// load keys into saManufSSTable

   // update state
   *StatePtr = nfSAS_RUN;

   return (nfTeeStorageResult_t) nfTGR_SUCCESS;

}/* end saTpiInit */
