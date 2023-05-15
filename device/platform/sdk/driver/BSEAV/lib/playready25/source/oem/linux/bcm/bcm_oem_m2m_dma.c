/**@@@+++@@@@******************************************************************
**
** Microsoft PlayReady
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>

#include "bstd.h"
#include "bdbg.h"
#include "bkni.h"

#ifdef PRDY_ROBUSTNESS

#include "common_crypto.h"
#include "nexus_platform.h"
#include "nexus_random_number.h"
#include "nexus_otpmsp.h"
#include "bchp.h"
#include "bchp_bsp_glb_control.h"
#include "bcm_oem.h"

#define CHIP_ID_INDEX   4
#define UNIQUE_CHIPID_LENGTH	16

#define UNUSED_PARAM(x)      (void)x

BDBG_MODULE(oem_m2m_dma);

typedef enum
{
	OEM_DECRYPTOR_TYPE_UNKNOWN = 0,	/**< UNKNOWN */
	OEM_DECRYPTOR_TYPE_AES_128_CTR,
	OEM_DECRYPTOR_TYPE_AES_128_CBC,
	OEM_DECRYPTOR_TYPE_AES_128_ECB,
	OEM_DECRYPTOR_TYPE_AES_128_CTR_EX,
	OEM_DECRYPTOR_TYPE_WMDrmPd,
	OEM_DECRYPTOR_TYPE_RC4,
	OEM_DECRYPTOR_TYPE_SECURE_KEYHANDLE,
	OEM_DECRYPTOR_TYPE_MAX
} OEM_DECRYPTOR_Type_e;

typedef struct
{
	CommonCryptoHandle cryptoHandle;
#if (NEXUS_VERSION >= 1304)
	NEXUS_DmaHandle hDma;
	NEXUS_DmaJobHandle hDmaJob;
#endif
	NEXUS_KeySlotHandle KeyHandle;

	OEM_DECRYPTOR_Type_e tDecType;

	/* for PlayReady DRM */
	unsigned char *pEkl;

	/* for CTR Mode Ex */
	unsigned char *tmp_iv;
	unsigned char *tmp_ecount_buf;

	/*DRM_DECRYPT_CONTEXT pDecryptContext;*/
} OEM_DECRYPTOR_Handle;

typedef void* OEM_DECRYPTOR_Context_t;

DRM_RESULT _Crypto_AesEcb(
	CommonCryptoHandle commonCryptoHandle,
	CommonCryptoKeySrc keySrc,
	CommonCryptoKeyLadderSettings *pKeyLadderSettings,
	uint8_t *pBuf,
	uint32_t buf_size,
	DRM_BYTE *pEncKey,
	NEXUS_SecurityOperation opType
)
{
    DRM_RESULT  dr = DRM_SUCCESS;
    NEXUS_SecurityKeySlotSettings keySlotSettings;
    NEXUS_KeySlotHandle keyHandle = NULL;
    CommonCryptoKeySettings keySettings;
    NEXUS_DmaJobBlockSettings blk;
    CommonCryptoJobSettings jobSettings;

    ChkArg( pKeyLadderSettings != NULL
         && pBuf  != NULL
         && buf_size != 0);

    ChkArg( !(pEncKey  == NULL && keySrc == CommonCrypto_eCustKey));

    if(keySrc == CommonCrypto_eCustKey)
        BKNI_Memcpy(pKeyLadderSettings->procInForKey4, pEncKey, 16);

    NEXUS_Security_GetDefaultKeySlotSettings(&keySlotSettings);
    keySlotSettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;

    keyHandle = NEXUS_Security_AllocateKeySlot(&keySlotSettings);
    if(keyHandle == NULL)
    {
        ChkDR(DRM_E_CRYPTO_FAILED);
    }

    CommonCrypto_GetDefaultKeySettings(&keySettings, keySrc);
    keySettings.keySrc  = keySrc;
    keySettings.keySlot = keyHandle;
    BKNI_Memcpy(&keySettings.src.keyLadderInfo, pKeyLadderSettings, sizeof(keySettings.src.keyLadderInfo));

	if(keySrc != CommonCrypto_eClearKey){
    	CommonCrypto_GetDefaultKeyLadderSettings((CommonCryptoKeyLadderSettings*)&keySettings.src.keyLadderInfo);
    }
    keySettings.alg.opType            = opType;
    keySettings.alg.algType           = NEXUS_SecurityAlgorithm_eAes;
    keySettings.alg.algVariant        = NEXUS_SecurityAlgorithmVariant_eEcb;
    keySettings.alg.termMode          = NEXUS_SecurityTerminationMode_eClear;
    keySettings.alg.enableExtKey      = false;
    keySettings.alg.enableExtIv       = false;
    keySettings.alg.aesCounterSize    = 0;

    if(CommonCrypto_SetupKey(commonCryptoHandle, &keySettings) != NEXUS_SUCCESS)
    {
        ChkDR(DRM_E_CRYPTO_FAILED);
    }

    NEXUS_DmaJob_GetDefaultBlockSettings(&blk);
    blk.pSrcAddr =  pBuf;
    blk.pDestAddr = pBuf;
    blk.blockSize = buf_size;
    blk.resetCrypto = true;
    blk.scatterGatherCryptoStart = true;
    blk.scatterGatherCryptoEnd = true;

    CommonCrypto_GetDefaultJobSettings(&jobSettings);
    jobSettings.keySlot = keyHandle;

    if(CommonCrypto_DmaXfer(commonCryptoHandle, &jobSettings, &blk, 1) != NEXUS_SUCCESS)
    {
        ChkDR(DRM_E_CRYPTO_FAILED);
    }

ErrorExit:
    if(keyHandle != NULL)
    {
        NEXUS_Security_FreeKeySlot(keyHandle);
    }

    return dr;
}


DRM_RESULT BCM_DecryptFromFile (DRM_VOID  *f_pOEMContext,
	const   DRM_DEVICE_ID_TYPE   f_eIDType,
	uint8_t* pDmaBuf,
	DRM_DWORD   cbFile,
	uint8_t *pKey)
{
	DRM_RESULT  dr = DRM_SUCCESS;
	CommonCryptoKeyLadderSettings *pKeyLadderSettings;

	oem_ctx_t *pOemCtx = (oem_ctx_t *)f_pOEMContext;

	if(f_eIDType == DRM_DIT_WMDRMPD_ID){
        pKey = pOemCtx->wmdrm_ki.data.wmdrm.key;
        if(pOemCtx->keySrc == CommonCrypto_eCustKey)
            pKeyLadderSettings = &pOemCtx->wmdrm_ki.info;
        else {
            pKeyLadderSettings = &pOemCtx->optKeySettings;
        }
    }
    else{
        pKey = pOemCtx->pr_ki.data.pr.key;
        if(pOemCtx->keySrc == CommonCrypto_eCustKey)
            pKeyLadderSettings = &pOemCtx->pr_ki.info;
        else {
            pKeyLadderSettings = &pOemCtx->optKeySettings;
        }
    }

    dr = _Crypto_AesEcb(pOemCtx->commonCryptoHandle, pOemCtx->keySrc, pKeyLadderSettings, pDmaBuf, cbFile,
                        pKey, NEXUS_SecurityOperation_eDecrypt);

	return dr;
}

DRM_RESULT BCM_EncryptToFile (DRM_VOID  *f_pOEMContext,
	const   DRM_DEVICE_ID_TYPE   f_eIDType,
	uint8_t* pDmaBuf,
	DRM_DWORD   cbFile,
	uint32_t size,
	uint8_t *pKey)
{
	DRM_RESULT  dr = DRM_SUCCESS;
	CommonCryptoKeyLadderSettings *pKeyLadderSettings;

	oem_ctx_t *pOemCtx = (oem_ctx_t *)f_pOEMContext;

	(void)(cbFile);

	if(f_eIDType == DRM_DIT_WMDRMPD_ID){
		pKey = pOemCtx->wmdrm_ki.data.wmdrm.key;
		if(pOemCtx->keySrc == CommonCrypto_eCustKey)
			pKeyLadderSettings = &pOemCtx->wmdrm_ki.info;
		else {
			pKeyLadderSettings = &pOemCtx->optKeySettings;
		}
	}
	else{
		pKey = pOemCtx->pr_ki.data.pr.key;
		if(pOemCtx->keySrc == CommonCrypto_eCustKey)
			pKeyLadderSettings = &pOemCtx->pr_ki.info;
		else {
			pKeyLadderSettings = &pOemCtx->optKeySettings;
		}
	}

	dr = _Crypto_AesEcb(pOemCtx->commonCryptoHandle, pOemCtx->keySrc, pKeyLadderSettings, pDmaBuf, size,
						pKey, NEXUS_SecurityOperation_eEncrypt);

	return dr;
}


/*****************************************************************************
** Function:    _GetPrPrivateKey
**
** Synopsis:    Obtains either the playready private key, or NDR private key
**
** Arguments:   [f_pOEMContext] : A pointer to the OEM context; may be NULL
**              [f_eKeyType]    : Either DRM_DKT_PLAYREADY_MODEL or
 **                               DRM_DKT_PLAYREADY_NDR_MODEL.
**              [f_pbPrivkey]   : Returned privkey
**              [f_cbPrivKey]   : Size of the key buffer
**
** Returns:     DRM_SUCCESS            - on success
**              DRM_E_PRIVKEYREADERROR - on any failure
**
******************************************************************************/
DRM_RESULT BCM_GetPrPrivateKey(
    DRM_VOID *f_pOEMContext,
    const  DRM_DEVICE_KEY_TYPE    f_eKeyType,
    DRM_BYTE *f_pbPrivkey,
    DRM_DWORD f_cbPrivKey )
{
    DRM_RESULT  dr = DRM_SUCCESS;
    uint32_t offset = 0;
    oem_ctx_t *pOemCtx = (oem_ctx_t *)f_pOEMContext;

    ChkArg( f_pOEMContext != NULL );
    ChkArg( f_pbPrivkey != NULL );

    ChkArg( (f_eKeyType == DRM_DKT_PLAYREADY_MODEL
            || f_eKeyType == DRM_DKT_PLAYREADY_NDR_MODEL));

    ChkBOOL(( f_cbPrivKey >= 32), DRM_E_BUFFERTOOSMALL );

    /*/ Copy the keys to a temporary buffer used for decryption. This is needed to keep the copy */
    /*/ of the WMDDRM keys encrypted in memory all the time.*/
    memcpy(pOemCtx->pPrData, &pOemCtx->pr_ki.data, sizeof(bdvd_playready_data_t));

    /*/ Decrypt both playready keys */
    offset = 8;

    /*/ We are decrypting the region private key from the DRM bin file. As a result, the key source is CommonCrypto_eCustKey */
    dr = _Crypto_AesEcb(pOemCtx->commonCryptoHandle, CommonCrypto_eCustKey, &pOemCtx->pr_ki.info, &pOemCtx->pPrBuf[offset],
                        64, pOemCtx->pPrData->key, NEXUS_SecurityOperation_eDecrypt);
    ChkDR(dr);

    /*/ Return proper key to the user. */
    offset += (f_eKeyType == DRM_DKT_PLAYREADY_MODEL) ? 0 : BDVD_PLAYREADY_MOD_KEY_SIZE;
    memcpy(f_pbPrivkey, &pOemCtx->pPrBuf[offset], BDVD_PLAYREADY_MOD_KEY_SIZE);

ErrorExit:
    /*/ Do not keep decrypted keys in memory */
    memset(pOemCtx->pPrData, 0, sizeof(bdvd_playready_data_t));

    return dr;

}


/*****************************************************************************
** Function:    _GetPrivateKey
**
** Synopsis:    Obtains either the Group Certificate (GC) private key, or
**              the Fallback signing key from the privkey file.
**
** Arguments:   [f_pOEMContext] : A pointer to the OEM context; may be NULL
**              [f_fGCprivkey]  : TRUE for GC privkey, FALSE for Fallback signing key
**              [f_pbPrivkey]   : Returned privkey
**              [f_cbPrivKey]   : Size of the key buffer
**
** Returns:     DRM_SUCCESS            - on success
**              DRM_E_PRIVKEYREADERROR - on any failure
**
** Notes:       The GC privkey and Fallback privkey are in the same file; the
**              GC privkey is the first 20 bytes, the Fallback signing key is the second 20 bytes.
**
******************************************************************************/
DRM_RESULT BCM_GetPrivateKey(
    DRM_VOID *f_pOEMContext,
    const DRM_BOOL f_fGCprivkey,
    DRM_BYTE *f_pbPrivkey,
    DRM_DWORD f_cbPrivKey )
{
    DRM_RESULT dr = DRM_SUCCESS;
    uint32_t offset;
    oem_ctx_t *pOemCtx = (oem_ctx_t *)f_pOEMContext;

    ChkArg( f_pOEMContext != NULL );
    ChkArg( f_pbPrivkey != NULL );
    ChkBOOL( f_cbPrivKey >= /*sizeof( PRIVKEY )*/20, DRM_E_BUFFERTOOSMALL );

    /*/ Copy the keys to a temporary buffer used for decryption. This is needed to keep the copy */
    /*/ of the WMDDRM keys encrypted in memory all the time. */
    memcpy(pOemCtx->pWmdrmData, &pOemCtx->wmdrm_ki.data.wmdrm, sizeof(bdvd_wmdrm_pd_data_t));

    /*/ Decrypt both wmdrm keys */
    offset = 8;

    /*/ We are decrypting the region private key from the DRM bin file. As a result, the key source is NEXUS_CmnCrypto_eCustKey */
    dr = _Crypto_AesEcb(pOemCtx->commonCryptoHandle, CommonCrypto_eCustKey, &pOemCtx->wmdrm_ki.info, &pOemCtx->pWmdrmBuf[offset],
                        64, pOemCtx->pWmdrmData->key, NEXUS_SecurityOperation_eDecrypt);
    ChkDR(dr);

    /*/ Return proper key to the user. */
    offset += (f_fGCprivkey) ? 0 : BDVD_WMDRM_PD_ENC_KEY_SIZE;
    memcpy(f_pbPrivkey, &pOemCtx->pWmdrmBuf[offset], BDVD_WMDRM_PD_KEY_SIZE);

ErrorExit:
    /*/ Do not keep decrypted keys in memory */
    memset(pOemCtx->pWmdrmData, 0, sizeof(bdvd_wmdrm_pd_data_t));
    return dr;

}

void BCM_Setpr_keys_valid(	DRM_VOID *f_pOEMContext, bool pr_keys_valid)
{
	oem_ctx_t *pOemCtx = (oem_ctx_t *)f_pOEMContext;

	pOemCtx->pr_keys_valid = pr_keys_valid;

	return;
}

bool BCM_Getpr_keys_valid(DRM_VOID *f_pOEMContext)
{
	oem_ctx_t *pOemCtx = (oem_ctx_t *)f_pOEMContext;

	return pOemCtx->pr_keys_valid;
}

void BCM_SetZGroupCert(	DRM_VOID *f_pOEMContext, uint8_t *pZGroupCert)
{
	oem_ctx_t *pOemCtx = (oem_ctx_t *)f_pOEMContext;

	pOemCtx->pZGroupCert = pZGroupCert;

	return;
}

uint8_t * BCM_GetZGroupCert( DRM_VOID *f_pOEMContext)
{
	oem_ctx_t *pOemCtx = (oem_ctx_t *)f_pOEMContext;

	return pOemCtx->pZGroupCert;
}

void BCM_SetZGroupCertSize( DRM_VOID *f_pOEMContext, uint32_t zgroupCertSize)
{
	oem_ctx_t *pOemCtx = (oem_ctx_t *)f_pOEMContext;

	pOemCtx->zgroupCertSize = zgroupCertSize;

	return;
}

unsigned int BCM_GetZGroupCertSize( DRM_VOID *f_pOEMContext)
{
	oem_ctx_t *pOemCtx = (oem_ctx_t *)f_pOEMContext;

	return pOemCtx->zgroupCertSize;
}

void BCM_Setrevoked(	 DRM_VOID *f_pOEMContext, bool revoked)
{
	oem_ctx_t *pOemCtx = (oem_ctx_t *)f_pOEMContext;

	pOemCtx->revoked = revoked;

	return;
}

bool BCM_Getrevoked( DRM_VOID *f_pOEMContext)
{
	oem_ctx_t *pOemCtx = (oem_ctx_t *)f_pOEMContext;

	return pOemCtx->revoked;
}

void BCM_Setwmdrm_keys_valid(	 DRM_VOID *f_pOEMContext, bool wmdrm_keys_valid)
{
	oem_ctx_t *pOemCtx = (oem_ctx_t *)f_pOEMContext;

	pOemCtx->wmdrm_keys_valid = wmdrm_keys_valid;

	return;
}

bool BCM_Getwmdrm_keys_valid( DRM_VOID *f_pOEMContext)
{
	oem_ctx_t *pOemCtx = (oem_ctx_t *)f_pOEMContext;

	return pOemCtx->wmdrm_keys_valid;
}

void BCM_SetDevTemplate( DRM_VOID *f_pOEMContext, uint8_t *pDevTemplate)
{
	oem_ctx_t *pOemCtx = (oem_ctx_t *)f_pOEMContext;

	pOemCtx->pDevTemplate = pDevTemplate;

	return;
}

uint8_t * BCM_GetDevTemplate( DRM_VOID *f_pOEMContext)
{
	oem_ctx_t *pOemCtx = (oem_ctx_t *)f_pOEMContext;

	return pOemCtx->pDevTemplate;
}

unsigned int BCM_GetDevTemplateSize( DRM_VOID *f_pOEMContext)
{
	oem_ctx_t *pOemCtx = (oem_ctx_t *)f_pOEMContext;

	return pOemCtx->devTemplateSize;
}

DRM_RESULT BCM_GetChipUniqueID(uint8_t *pucChipID, uint32_t ulChipIDLength)
{
	NEXUS_ReadOtpIO ReadOtpIO;
	unsigned long reg = 0;
	NEXUS_Error errCode = NEXUS_SUCCESS;
	int i;

	if(ulChipIDLength < UNIQUE_CHIPID_LENGTH || pucChipID == NULL)
		return DRM_E_INVALIDARG;

	memset(pucChipID, 0, ulChipIDLength);

/*******************
*1. Get BCHP_CHIP (Chip Number)
********************/
	uint32_t quotient = 0;
	quotient = BCHP_CHIP;

	for(i = 8 ; i > 0 ; --i)
	{
		pucChipID[i-1] = quotient%10;
		quotient = quotient/10;
	}
#if 0
	printf("\nChip NUMBER >>>\n");
	for (i = 0; i < 16; i++)
	{
		printf("%x ", pucChipID[i]);
	}
	printf("\nChip NUMBER <<<\n");
#endif
/*******************
*1. Get  Chip Unique id
********************/
	memset(&ReadOtpIO, 0, sizeof(ReadOtpIO));

	errCode = NEXUS_Security_ReadOTP(NEXUS_OtpCmdReadRegister_eKeyID, NEXUS_OtpKeyType_eA, &ReadOtpIO);
	if (errCode != NEXUS_SUCCESS)
	{
		printf("NEXUS_Security_ReadOTP() errCode: %x\n", errCode );
		return DRM_E_INVALIDARG;
	}
#if 0
	printf("Chip ID >>>\n");
	for (i = 0; i < 8; i++)
	{
		printf("[%d] 0x%x\n", i, ReadOtpIO.otpKeyIdBuf[i]);
	}
	printf("Chip ID <<<\n");
#endif
/*******************
*2. Override Chip Unique id
********************/
	pucChipID[8] = ReadOtpIO.otpKeyIdBuf[CHIP_ID_INDEX];
	pucChipID[9] = ReadOtpIO.otpKeyIdBuf[CHIP_ID_INDEX+1];
	pucChipID[10] = ReadOtpIO.otpKeyIdBuf[CHIP_ID_INDEX+2];
	pucChipID[11] = ReadOtpIO.otpKeyIdBuf[CHIP_ID_INDEX+3];

/*******************
*3. Get CAS Unique value (BSEC OTP HASH CRC Value)
********************/
	NEXUS_Platform_Registers_Read_Access(BCHP_BSP_GLB_CONTROL_OTP_HASH_CRC, (unsigned long *)&reg);
/*/	printf(" ### BSEC OTP HASH CRC = 0x%X ###\n", reg); */

/*******************
*4. Override CAS Unique value (BSEC OTP HASH CRC Value)
********************/
	pucChipID[12] = (reg & 0xFF000000) >> 24;
	pucChipID[13] = (reg & 0x00FF0000) >> 16;
	pucChipID[14] = (reg & 0x0000FF00) >> 8;
	pucChipID[15] = (reg & 0x000000FF) ;
#if 0
	printf("CHIPSET UNIQUE ID = ");
	for (i = 0; i < 16; i++)
	{
		printf("(%d)%x, ",i, pucChipID[i]);
	}
	printf("\n\nChip ID <<<\n");
#endif
	return DRM_SUCCESS;
}

uint8_t* BCM_Getwmdrm_serial_nb( DRM_VOID *f_pOEMContext)
{
	oem_ctx_t *pOemCtx = (oem_ctx_t *)f_pOEMContext;

	return pOemCtx->wmdrm_ki.serial_nb;
}

uint8_t* BCM_Getprdy_serial_nb( DRM_VOID *f_pOEMContext)
{
	oem_ctx_t *pOemCtx = (oem_ctx_t *)f_pOEMContext;

	return pOemCtx->pr_ki.serial_nb;
}

DRM_RESULT BCM_ReadBinFilePrdy(
	DRM_VOID *f_pOEMContext,
	unsigned char cust_key,
	unsigned char var_lo,
	unsigned char var_hi,
	unsigned char *SerialNumber,
	void *playready_data,
	unsigned int playready_data_size,
	void *DeviceCertificateTemplateSize,
	unsigned char *pSignature,
	unsigned char *prdyBinFile,
	unsigned int group_cert_offset,
	unsigned char *key)
{
	DRM_RESULT dr = DRM_SUCCESS;
	oem_ctx_t *pOemCtx = (oem_ctx_t *)f_pOEMContext;
	uint32_t    DRM_DataType;

	pOemCtx->pr_ki.info.custKeySelect = cust_key;
	pOemCtx->pr_ki.info.keyVarLow     = var_lo;
	pOemCtx->pr_ki.info.keyVarHigh    = var_hi;

	memcpy(pOemCtx->pr_ki.info.procInForKey3, SerialNumber, BDVD_DRM_SERIAL_NUMBER_LENGTH);
	memcpy(pOemCtx->pr_ki.serial_nb, SerialNumber, BDVD_DRM_SERIAL_NUMBER_LENGTH);
	memcpy(&pOemCtx->pr_ki.data, playready_data, playready_data_size);

	pOemCtx->zgroupCertSize = GET_UINT32_FROM_BUF(DeviceCertificateTemplateSize);

	/*/ Data is in big endian in flash. Convert it to native processor. */
	DRM_DataType = GET_UINT32_FROM_BUF(&pOemCtx->pr_ki.data);
	if(DRM_DataType != BDVD_DRM_DATA_PLAYREADY){
	    printf("%s -DRM_DataType != BDVD_DRM_DATA_PLAYREADY", __FUNCTION__);
	    ChkDR(DRM_E_PRIVKEYREADERROR);
	}

	memcpy(pSignature, &prdyBinFile[group_cert_offset + pOemCtx->zgroupCertSize], BDVD_PLAYREADY_BIN_ENC_SIGN_SIZE_IN_BYTES);
	 /*/ Signature is encrypted, decrypt it. */
	dr = _Crypto_AesEcb(pOemCtx->commonCryptoHandle, CommonCrypto_eCustKey, &pOemCtx->pr_ki.info, pSignature,
	                    BDVD_PLAYREADY_BIN_ENC_SIGN_SIZE_IN_BYTES, key, NEXUS_SecurityOperation_eDecrypt);

ErrorExit:
	return dr;
}

DRM_RESULT BCM_ReadBinFileWmdrm(
	DRM_VOID *f_pOEMContext,
	unsigned char cust_key,
	unsigned char var_lo,
	unsigned char var_hi,
	unsigned char *SerialNumber,
	void *wmdrm_data,
	unsigned int wmdrm_data_size,
	void *reserved,
	unsigned char *pSignature,
	unsigned char *prdyBinFile,
	unsigned int cbRead,
	unsigned char *key,
	uint32_t *pencTemplateSize)
{
	DRM_RESULT dr = DRM_SUCCESS;
	oem_ctx_t *pOemCtx = (oem_ctx_t *)f_pOEMContext;
	uint32_t    DRM_DataType;

	pOemCtx->wmdrm_ki.info.custKeySelect = cust_key;
	pOemCtx->wmdrm_ki.info.keyVarLow	 = var_lo;
	pOemCtx->wmdrm_ki.info.keyVarHigh	 = var_hi;

	memcpy(pOemCtx->wmdrm_ki.info.procInForKey3, SerialNumber, BDVD_DRM_SERIAL_NUMBER_LENGTH);
	memcpy(pOemCtx->wmdrm_ki.serial_nb, SerialNumber, BDVD_DRM_SERIAL_NUMBER_LENGTH);
	memcpy(&pOemCtx->wmdrm_ki.data, wmdrm_data, wmdrm_data_size);

	pOemCtx->devTemplateSize = GET_UINT32_FROM_BUF(reserved);
	if(pOemCtx->devTemplateSize % 16 != 0){
		*pencTemplateSize = pOemCtx->devTemplateSize + (16 - (pOemCtx->devTemplateSize % 16));
	}
	else {
		*pencTemplateSize = pOemCtx->devTemplateSize;
	}

	/*/ Data is in big endian in flash. Convert it to native processor. */
	DRM_DataType = GET_UINT32_FROM_BUF(&pOemCtx->wmdrm_ki.data);

	if(DRM_DataType != BDVD_DRM_DATA_WMDRM_PD){
		printf("%s -DRM_DataType != BDVD_DRM_DATA_WMDRM_PD", __FUNCTION__);
		ChkDR(DRM_E_PRIVKEYREADERROR);
	}

	memcpy(pSignature, &prdyBinFile[cbRead - BDVD_PLAYREADY_BIN_ENC_SIGN_SIZE_IN_BYTES], BDVD_PLAYREADY_BIN_ENC_SIGN_SIZE_IN_BYTES);

	/*/ Signature is encrypted, decrypt it. */
	dr = _Crypto_AesEcb(pOemCtx->commonCryptoHandle, CommonCrypto_eCustKey, &pOemCtx->wmdrm_ki.info, pSignature,
						BDVD_PLAYREADY_BIN_ENC_SIGN_SIZE_IN_BYTES, key, NEXUS_SecurityOperation_eDecrypt);

ErrorExit:
	return dr;
}

DRM_RESULT BCM_Generate_Random_Number(DRM_BYTE *f_rgbSecStorePassword, DRM_DWORD  *f_pcbBuffer)
{
	DRM_RESULT dr=DRM_SUCCESS;

    /* Generate random number*/
    NEXUS_RandomNumberOutput random_nb;
    NEXUS_RandomNumberGenerateSettings settings;

    NEXUS_RandomNumber_GetDefaultGenerateSettings(&settings);

    settings.type = NEXUS_RandomNumberType_eRngRaw;
    settings.destination = NEXUS_RandomNumberDestination_eHost;
    settings.randomNumberSize = /*DRM_SHA1_DIGEST_LEN*/20;

    if(NEXUS_RandomNumber_Generate( &settings, &random_nb) != NEXUS_SUCCESS)
    {
        ChkDR(DRM_E_CRYPTO_FAILED);
    }

    if(random_nb.size != /*DRM_SHA1_DIGEST_LEN*/20){
        ChkDR(DRM_E_CRYPTO_FAILED);
    }
    /* Copy the password in the return buffer*/
    memcpy(f_rgbSecStorePassword, random_nb.buffer, random_nb.size);
	*f_pcbBuffer = random_nb.size;

ErrorExit:
	return dr;
}

#if 1
void *BCM_DECRYPTOR_SOC_Malloc(unsigned int unBytes)
{
	char *pMemory=NULL;

	if(NEXUS_Memory_Allocate((size_t)unBytes, NULL, (void**)&pMemory) != NEXUS_SUCCESS)
	{
		return NULL;
	}
	return (void*)pMemory;
}

void BCM_DECRYPTOR_SOC_Free(void *pMemory)
{
	if(pMemory) {
		(void)NEXUS_Memory_Free(pMemory);
		pMemory = NULL;
	}
}

static int bcm_do_crypt_loadUniqueKey(NEXUS_KeySlotHandle keyHandle)
{
	NEXUS_Error	nexusError = NEXUS_SUCCESS;
	NEXUS_SecurityEncryptedSessionKey SessionKey;
	NEXUS_SecurityEncryptedControlWord CW;
	uint8_t myAucKeyData3[16] =
	{
		0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
		0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
	};
	uint8_t myAucKeyData4[16] =
	{
		0xe4, 0x62, 0x75, 0x1b, 0x5d, 0xd4, 0xbc, 0x02,
		0x27, 0x9e, 0xc9, 0x6c, 0xc8, 0x66, 0xec, 0x10
	};
#if defined(CONFIG_SECURITY_EXTENSION)
	NEXUS_Security_GetDefaultSessionKeySettings(&SessionKey);
#endif
	SessionKey.keyladderID			= NEXUS_SecurityKeyladderID_eA;
	SessionKey.keyladderType		= NEXUS_SecurityKeyladderType_e3Des;
	SessionKey.rootKeySrc			= NEXUS_SecurityRootKeySrc_eOtpKeyA;
	SessionKey.swizzleType			= NEXUS_SecuritySwizzleType_eNone;
#if (NEXUS_VERSION >= 1304)
	SessionKey.keyEntryType 		= NEXUS_SecurityKeyType_eClear;
#else
	SessionKey.keyEntryType 		= NEXUS_SecurityKeyType_eOdd;
#endif
#if defined(CONFIG_SECURITY_EXTENSION)
	SessionKey.dest			 		= NEXUS_SecurityAlgorithmConfigDestination_eMem2Mem;
#endif
 	SessionKey.operation     		= NEXUS_SecurityOperation_eDecrypt;
	SessionKey.operationKey2 		= NEXUS_SecurityOperation_eEncrypt;
	SessionKey.custSubMode	 		= NEXUS_SecurityCustomerSubMode_eGeneric_CP_128_4;
	SessionKey.virtualKeyLadderID 	= NEXUS_SecurityVirtualKeyladderID_eVKL5;
	SessionKey.keyMode		 		= NEXUS_SecurityKeyMode_eRegular;
	SessionKey.cusKeyL				= 0;
	SessionKey.cusKeyH				= 0;
	SessionKey.cusKeyVarL			= 0;
	SessionKey.cusKeyVarH			= 0xFF;
#if defined(CONFIG_SECURITY_EXTENSION)
	SessionKey.keyGenCmdID			= NEXUS_SecurityKeyGenCmdID_eKeyGen;
	SessionKey.sessionKeyOp 		= NEXUS_SecuritySessionKeyOp_eNoProcess;

	SessionKey.bASKMMode			= false;

	SessionKey.bSwapAESKey			= false;
	SessionKey.keyDestIVType		= NEXUS_SecurityKeyIVType_eNoIV;
#endif
	memcpy((void *)SessionKey.keyData, (void *)myAucKeyData3, sizeof(myAucKeyData3));
	SessionKey.bRouteKey			= false;

	nexusError = NEXUS_Security_GenerateSessionKey(keyHandle, &SessionKey);
	if (nexusError != NEXUS_SUCCESS)
	{
		printf("[%s] NEXUS_Security_GenerateSessionKey errCode: %x\n", __FUNCTION__, nexusError);
		return -1;
	}
#if defined(CONFIG_SECURITY_EXTENSION)
	NEXUS_Security_GetDefaultControlWordSettings(&CW);
#endif
	CW.keyladderID			= NEXUS_SecurityKeyladderID_eA;
	CW.keyladderType		= NEXUS_SecurityKeyladderType_e3Des;
#if (NEXUS_VERSION >= 1304)
	CW.keyEntryType 		= NEXUS_SecurityKeyType_eClear;
#else
	CW.keyEntryType 		= NEXUS_SecurityKeyType_eOdd;
#endif
#if defined(CONFIG_SECURITY_EXTENSION)
	CW.dest			 		= NEXUS_SecurityAlgorithmConfigDestination_eMem2Mem;

	CW.bASKMMode			= false;

#endif
	CW.custSubMode			= NEXUS_SecurityCustomerSubMode_eGeneric_CP_128_4;
	CW.virtualKeyLadderID	= NEXUS_SecurityVirtualKeyladderID_eVKL5;
	CW.keyMode				= NEXUS_SecurityKeyMode_eRegular;
	CW.keySize				= sizeof(myAucKeyData4);
	memcpy((void *)CW.keyData, (void *)myAucKeyData4, CW.keySize);
	CW.operation			= NEXUS_SecurityOperation_eDecrypt;
	CW.bRouteKey			= true;
#if defined(CONFIG_SECURITY_EXTENSION)
	CW.keyDestIVType		= NEXUS_SecurityKeyIVType_eNoIV;
	CW.keyGenCmdID			= NEXUS_SecurityKeyGenCmdID_eKeyGen;
	CW.bSwapAESKey			= false;
#endif
	nexusError = NEXUS_Security_GenerateControlWord(keyHandle, &CW);
	if (nexusError != NEXUS_SUCCESS)
	{
		printf("[%s] NEXUS_Security_GenerateControlWord errCode: %x\n", __FUNCTION__, nexusError);
		return -1;
	}

	return 0;
}

static int bcm_do_crypt_dma_otpkey(
	unsigned char 		*pBufSrc,
	unsigned char 		*pBufDest,
	unsigned long 		uBufLeng,
	NEXUS_KeySlotHandle	keyHandle,
	NEXUS_DmaHandle		dma
	)
{
    NEXUS_DmaJobSettings jobSettings;
    NEXUS_DmaJobHandle dmaJob;
    NEXUS_DmaJobBlockSettings blockSettings;
    NEXUS_DmaJobStatus jobStatus;
    NEXUS_Error rc = NEXUS_SUCCESS;
	int timeout = 100;		/* 100 ms */
	int errCode = 0;

	if ((dma == NULL) || (keyHandle == NULL))
	{
		printf("[%s] Parameter error!, DMA or KEY handle are NULL\n", __FUNCTION__);
		return -1;
	}

    NEXUS_DmaJob_GetDefaultSettings(&jobSettings);

    jobSettings.numBlocks                   = 1;
	jobSettings.keySlot                     = keyHandle;
	jobSettings.dataFormat                  = NEXUS_DmaDataFormat_eBlock;
    jobSettings.completionCallback.callback = NULL;

    dmaJob = NEXUS_DmaJob_Create(dma, &jobSettings);
	if (dmaJob == NULL)
	{
		printf("[%s] NEXUS_DmaJob_Create error\n", __FUNCTION__);
		return -1;
	}

    NEXUS_DmaJob_GetDefaultBlockSettings(&blockSettings);
    blockSettings.pSrcAddr                 = pBufSrc;
    blockSettings.pDestAddr                = pBufDest;
    blockSettings.blockSize                = uBufLeng;
	blockSettings.resetCrypto              = true;
	blockSettings.scatterGatherCryptoStart = true;
	blockSettings.scatterGatherCryptoEnd   = true;
	blockSettings.cached                   = true;

	rc = NEXUS_DmaJob_ProcessBlocks(dmaJob, &blockSettings, 1);

	while (timeout > 0)
	{
		rc = NEXUS_DmaJob_GetStatus(dmaJob, &jobStatus);
		if (rc != NEXUS_SUCCESS)
		{
			printf("[%s] NEXUS_DmaJob_GetStatus error(0x%x)\n", __FUNCTION__, rc);
			errCode = -1;
			break;
		}

		if (jobStatus.currentState == NEXUS_DmaJobState_eComplete)
		{
			break;
		}

		/*/ VK_TASK_Sleep(1)*/
		{
			struct timespec delay;
			struct timespec rem;
			int rc;
			delay.tv_sec = 1/1000;
			delay.tv_nsec = (1%1000) * 1000000;

			for(;;)	{
				rc = nanosleep(&delay, &rem);
				if(0!=rc)
				{
					if(EINTR == errno){ /*/#include <errno.h>*/
						delay = rem;
						continue;
					}
					errCode = -1;
					goto ErrorExit;
 				}
				break;
			}
		}

		timeout--;
	}

	if (timeout == 0)
	{
		printf("[%s] Timeout\n", __FUNCTION__);
		errCode = -1;
	}

ErrorExit:
	NEXUS_DmaJob_Destroy(dmaJob);
	return errCode;
}

inline static int bcm_do_crypt_otpkey(unsigned char *in, unsigned char *out, unsigned int size, bool isEncryption)
{
	NEXUS_DmaSettings dmaSettings;
	NEXUS_DmaHandle hDma = NULL;
	NEXUS_Error nResult;
	NEXUS_SecurityKeySlotSettings 	keySettings;
	NEXUS_KeySlotHandle				keyHandle = NULL;
	NEXUS_SecurityAlgorithmSettings AlgorithmSettings;
	NEXUS_SecurityClearKey 			key;
	int errCode = 0;

	memset(&dmaSettings, 0, sizeof(NEXUS_DmaSettings));
	hDma = NEXUS_Dma_Open(0, &dmaSettings);

	NEXUS_Security_GetDefaultKeySlotSettings(&keySettings);
	keySettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;
	keyHandle = NEXUS_Security_AllocateKeySlot(&keySettings);
	if (keyHandle == NULL)
	{
		printf("[%s(%d)] Error in NEXUS_Security_AllocateKeySlot()\n\r", __FUNCTION__, __LINE__);
		errCode -1;
		goto ErrorExit;
	}
	NEXUS_Security_GetDefaultAlgorithmSettings(&AlgorithmSettings);
	/*/ AES CBC Mode*/
	AlgorithmSettings.algorithm 		= NEXUS_SecurityAlgorithm_eAes;
	AlgorithmSettings.algorithmVar		= NEXUS_SecurityAlgorithmVariant_eCbc;
	AlgorithmSettings.dest				= NEXUS_SecurityAlgorithmConfigDestination_eMem2Mem;
	AlgorithmSettings.terminationMode	= NEXUS_SecurityTerminationMode_eCbcResidual ;
	AlgorithmSettings.operation 		= isEncryption ? NEXUS_SecurityOperation_eEncrypt : NEXUS_SecurityOperation_eDecrypt;

	AlgorithmSettings.ivMode 			= NEXUS_SecurityIVMode_eRegular;
	AlgorithmSettings.solitarySelect 	= NEXUS_SecuritySolitarySelect_eClear;
	AlgorithmSettings.caVendorID 		= 0x1234;
	AlgorithmSettings.askmModuleID 		= NEXUS_SecurityAskmModuleID_eModuleID_4;
	AlgorithmSettings.otpId 			= NEXUS_SecurityOtpId_eOtpVal;
	AlgorithmSettings.key2Select 		= NEXUS_SecurityKey2Select_eReserved1;

#if (NEXUS_VERSION >= 1304)
	AlgorithmSettings.keyDestEntryType	= NEXUS_SecurityKeyType_eClear;
#else
	AlgorithmSettings.keyDestEntryType 	= NEXUS_SecurityKeyType_eOdd;
#endif
	nResult = NEXUS_Security_ConfigAlgorithm(keyHandle, &AlgorithmSettings);
	if (nResult != NEXUS_SUCCESS)
	{
		NEXUS_Security_FreeKeySlot(keyHandle);
		printf("[%s(%d)] NEXUS_Security_ConfigAlgorithm() error\n", __FUNCTION__, __LINE__);
		errCode -1;
		goto ErrorExit;
	}

	if (bcm_do_crypt_loadUniqueKey(keyHandle) != 0)
	{
		NEXUS_Security_FreeKeySlot(keyHandle);
		printf("[%s] CRYPT_LoadUniqueKey error\n", __FUNCTION__);
		errCode -1;
		goto ErrorExit;
	}
	NEXUS_Security_GetDefaultClearKey(&key);

	key.keySize = 16;
#if (NEXUS_VERSION >= 1304)
	key.keyEntryType	= NEXUS_SecurityKeyType_eClear;
#else
	key.keyEntryType = NEXUS_SecurityKeyType_eOdd;
#endif
	key.keyIVType		= NEXUS_SecurityKeyIVType_eIV;
	nResult = NEXUS_Security_LoadClearKey(keyHandle, &key);
	if (nResult != NEXUS_SUCCESS)
	{
		NEXUS_Security_FreeKeySlot(keyHandle);
		printf("[%s] IV loading failed, line %d\n", __FUNCTION__, __LINE__ );
		errCode -1;
		goto ErrorExit;
	}
	if(bcm_do_crypt_dma_otpkey(in, out, size, keyHandle,hDma) != 0)
	{
		printf("[%s] P_CRYPT_CopyDma failed!\n",__FUNCTION__);
		errCode -1;
		goto ErrorExit;
	}

ErrorExit:
	if(hDma != NULL) NEXUS_Dma_Close(hDma);
	if(keyHandle != NULL) NEXUS_Security_FreeKeySlot(keyHandle);

	return errCode;
}
int BCM_DECRYPTOR_SOC_OTPKEY_DoEncrypt(unsigned char *in, unsigned char *out, unsigned int size)
{
	/*printf("!!!!!!!!FUNC = %s \n", __FUNCTION__);*/
	return bcm_do_crypt_otpkey(in, out, size, true);

}
int BCM_DECRYPTOR_SOC_OTPKEY_DoDecrypt(unsigned char *in, unsigned char *out, unsigned int size)
{
	/*printf("!!!!!!!!FUNC = %s \n", __FUNCTION__);*/
	return bcm_do_crypt_otpkey(in, out, size, false);
}





#endif /*/ #ifdef OTPKEY*/

#if 1

OEM_DECRYPTOR_Context_t BCM_DECRYPTOR_SOC_Create(OEM_DECRYPTOR_Type_e tDecType)
{
	OEM_DECRYPTOR_Handle *hDec = NULL;
	NEXUS_Error nResult;
	CommonCryptoSettings cryptoSettings;
	NEXUS_SecurityKeySlotSettings keySlotSettings;
	NEXUS_SecurityAlgorithmSettings AlgConfig;
#if (NEXUS_VERSION >= 1304)
	NEXUS_DmaSettings dmaSettings;
	NEXUS_DmaJobSettings  dmaJobSetting;
#endif
	/*printf("!!!!!!!!FUNC = %s \n", __FUNCTION__);*/
	/* Malloc Handle */
	hDec = (OEM_DECRYPTOR_Handle*)malloc(sizeof(OEM_DECRYPTOR_Handle));
	if(hDec == NULL)
	{
		BDBG_ERR(("Malloc failed\n"));
		return NULL;
	}
	memset(hDec, 0x0, sizeof(OEM_DECRYPTOR_Handle));

	hDec->tDecType = tDecType;
	if(tDecType == OEM_DECRYPTOR_TYPE_SECURE_KEYHANDLE)
	{
		return (OEM_DECRYPTOR_Context_t *)hDec;
	}
	/* dma init */
	CommonCrypto_GetDefaultSettings(&cryptoSettings);
	hDec->cryptoHandle = CommonCrypto_Open(&cryptoSettings);

#if (NEXUS_VERSION >= 1304)
	(void)NEXUS_Security_GetDefaultAlgorithmSettings(&AlgConfig);
	memset(&dmaSettings, 0, sizeof(NEXUS_DmaSettings));
	hDec->hDma = NEXUS_Dma_Open(0, &dmaSettings);

	NEXUS_DmaJob_GetDefaultSettings (&dmaJobSetting);
	dmaJobSetting.numBlocks = 1;
	dmaJobSetting.completionCallback.callback = NULL;
	dmaJobSetting.dataFormat = NEXUS_DmaDataFormat_eBlock;
	dmaJobSetting.keySlot = hDec->KeyHandle;
	if ( (hDec->hDmaJob = NEXUS_DmaJob_Create (hDec->hDma, &dmaJobSetting)) == NULL )
	{
		BDBG_ERR(("%s(%d) Err> NEXUS_DmaJob_Create failed \n",__FUNCTION__,__LINE__));
		goto error;
	}
#endif

	/* Key Slot Allocate */
	(void)NEXUS_Security_GetDefaultKeySlotSettings(&keySlotSettings);
	keySlotSettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;

	hDec->KeyHandle = NEXUS_Security_AllocateKeySlot(&keySlotSettings);
	if(hDec->KeyHandle == NULL)
	{
		BDBG_ERR(("NEXUS_Security_AllocateKeySlot failed\n"));
		goto error;
	}

	(void)NEXUS_Security_GetDefaultAlgorithmSettings(&AlgConfig);
	NEXUS_Memory_Allocate(80, NULL, (void **)&(hDec->pEkl));
	/* Key Slot Init Data */
	switch(tDecType)
	{
		case OEM_DECRYPTOR_TYPE_WMDrmPd:
			/* wmdrm Data Init */
			/* wmdrm10 keyslot init */
			AlgConfig.algorithm			= NEXUS_SecurityAlgorithm_eWMDrmPd;
			AlgConfig.operation			= NEXUS_SecurityOperation_eDecrypt;
			AlgConfig.terminationMode 	= NEXUS_SecurityTerminationMode_eClear;
			AlgConfig.enableExtKey		= true;
			AlgConfig.enableExtIv 		= true;
			break;
		case OEM_DECRYPTOR_TYPE_AES_128_CTR:
			/*	playready keyslot(=AES_CTR) init */
			AlgConfig.algorithm			= NEXUS_SecurityAlgorithm_eAes128;
			AlgConfig.algorithmVar		= NEXUS_SecurityAlgorithmVariant_eCounter;
			AlgConfig.operation			= NEXUS_SecurityOperation_eDecrypt;
			AlgConfig.terminationMode 	= NEXUS_SecurityTerminationMode_eClear;
			AlgConfig.aesCounterSize	= NEXUS_SecurityAesCounterSize_e64Bits;
			AlgConfig.enableExtKey		= true;
			AlgConfig.enableExtIv 		= true;
#if (NEXUS_VERSION >= 1304)
			AlgConfig.keyDestEntryType	= NEXUS_SecurityKeyType_eClear;
			AlgConfig.solitarySelect	= NEXUS_SecuritySolitarySelect_eClear;
			AlgConfig.ivMode			= NEXUS_SecurityIVMode_eRegular;
			AlgConfig.enableExtKey		= false;
			AlgConfig.enableExtIv		= false;
#endif

			break;
		case OEM_DECRYPTOR_TYPE_AES_128_CBC:
			/* Common AES, 128bit, CBC mode */
			AlgConfig.algorithm			= NEXUS_SecurityAlgorithm_eAes128;
			AlgConfig.algorithmVar		= NEXUS_SecurityAlgorithmVariant_eCbc;
			AlgConfig.operation			= NEXUS_SecurityOperation_eDecrypt;
			AlgConfig.terminationMode 	= NEXUS_SecurityTerminationMode_eClear;
			AlgConfig.enableExtKey		= true;
			AlgConfig.enableExtIv 		= true;
#if (NEXUS_VERSION >= 1304)
			AlgConfig.keyDestEntryType	= NEXUS_SecurityKeyType_eClear;
			AlgConfig.solitarySelect	= NEXUS_SecuritySolitarySelect_eClear;
			AlgConfig.ivMode			= NEXUS_SecurityIVMode_eRegular;
			AlgConfig.enableExtKey		= false;
			AlgConfig.enableExtIv		= false;
#endif
			break;
#if 0
		case DECRYPTOR_TYPE_AES_128_ECB:
			/* Common AES, 128bit, ECB mode */
			AlgConfig.algorithm			= NEXUS_SecurityAlgorithm_eAes128;
			AlgConfig.algorithmVar		= NEXUS_SecurityAlgorithmVariant_eEcb;
			AlgConfig.operation			= NEXUS_SecurityOperation_eDecrypt;
			AlgConfig.terminationMode 	= NEXUS_SecurityTerminationMode_eClear;
			AlgConfig.enableExtKey		= true;
			AlgConfig.enableExtIv 		= true;
			break;
		case DECRYPTOR_TYPE_AES_128_CTR_EX:
			/* Common AES, 128bit, CTR mode */
			/* BUT !! This Is Support DoDecrypt_ExCTR !! */
#if 0
			AlgConfig.algorithm			= NEXUS_SecurityAlgorithm_eAes128;
			AlgConfig.algorithmVar		= NEXUS_SecurityAlgorithmVariant_eEcb;
			AlgConfig.operation			= NEXUS_SecurityOperation_eEncrypt;
			AlgConfig.keyDestEntryType	= NEXUS_SecurityKeyType_eOdd;
			AlgConfig.dest				= NEXUS_SecurityAlgorithmConfigDestination_eMem2Mem;
			/*/AlgConfig.terminationMode 	= NEXUS_SecurityTerminationMode_eClear;*/
			AlgConfig.enableExtKey		= false;
			AlgConfig.enableExtIv 		= false;
			AlgConfig.key2Select	= NEXUS_SecurityKey2Select_eReserved1;
#else
			AlgConfig.algorithm 		= NEXUS_SecurityAlgorithm_eAes;
			AlgConfig.algorithmVar 		= NEXUS_SecurityAlgorithmVariant_eEcb;
			AlgConfig.terminationMode 	= NEXUS_SecurityTerminationMode_eClear;

			AlgConfig.scValue[NEXUS_SecurityPacketType_eGlobal] 	= NEXUS_SecurityAlgorithmScPolarity_eClear;
			AlgConfig.scValue[NEXUS_SecurityPacketType_eRestricted] = NEXUS_SecurityAlgorithmScPolarity_eClear;

			AlgConfig.operation			= NEXUS_SecurityOperation_eEncrypt;
			AlgConfig.dest				= NEXUS_SecurityAlgorithmConfigDestination_eMem2Mem;
			AlgConfig.keyDestEntryType	= NEXUS_SecurityKeyType_eOdd;

			AlgConfig.modifyScValue[NEXUS_SecurityPacketType_eGlobal] = 0;

			AlgConfig.ivMode 			= NEXUS_SecurityIVMode_eRegular;
			AlgConfig.solitarySelect 	= NEXUS_SecuritySolitarySelect_eClear;
			AlgConfig.caVendorID 		= 0x1234;
			AlgConfig.askmModuleID 		= NEXUS_SecurityAskmModuleID_eModuleID_4;
			AlgConfig.otpId 			= NEXUS_SecurityOtpId_eOtpVal;
			AlgConfig.key2Select 		= NEXUS_SecurityKey2Select_eReserved1;
			AlgConfig.enableExtKey		= false;
			AlgConfig.enableExtIv 		= false;
#endif
			break;
#endif
		case OEM_DECRYPTOR_TYPE_RC4:
			/*
			   AlgConfig.algorithm			= NEXUS_SecurityAlgorithm_eRc4;
			   AlgConfig.operation			= NEXUS_SecurityOperation_eDecrypt;
			   AlgConfig.terminationMode 	= NEXUS_SecurityTerminationMode_eClear;
			   AlgConfig.enableExtKey		= true;
			   AlgConfig.enableExtIv 		= true;
			   */
		default :
			BDBG_ERR(("%s - Not Support TYPE %u\n", __FUNCTION__, hDec->tDecType));
	}

	nResult = NEXUS_Security_ConfigAlgorithm(hDec->KeyHandle, &AlgConfig);
	if(nResult != NEXUS_SUCCESS)
	{
		BDBG_ERR(("NEXUS_Security_ConfigAlgorithm failed!\n"));
		goto error;
	}

	return (OEM_DECRYPTOR_Context_t *)hDec;

error:
	if(hDec->pEkl != NULL)
	{
		NEXUS_Memory_Free(hDec->pEkl);
	}
	if(hDec->KeyHandle != NULL)
	{
		(void)NEXUS_Security_FreeKeySlot(hDec->KeyHandle);
		hDec->KeyHandle = NULL;
	}
#if (NEXUS_VERSION >= 1304)
	if(hDec->hDmaJob != NULL)
	{
		NEXUS_DmaJob_Destroy (hDec->hDmaJob);
		hDec->hDmaJob = NULL;
	}
	if(hDec->hDma != NULL)
	{
		NEXUS_Dma_Close(hDec->hDma);
		hDec->hDma = NULL;
	}
#endif
	if(hDec->tmp_iv)
	{
		(void)NEXUS_Memory_Free(hDec->tmp_iv);
		hDec->tmp_iv = NULL;
	}
	if(hDec->tmp_ecount_buf)
	{
		(void)NEXUS_Memory_Free(hDec->tmp_ecount_buf);
		hDec->tmp_ecount_buf = NULL;
	}

	if(hDec != NULL)
	{
		(void)free(hDec);
		hDec = NULL;
	}

	return NULL;
}

int BCM_DECRYPTOR_SOC_Destroy(OEM_DECRYPTOR_Context_t tDecCtx)
{
	OEM_DECRYPTOR_Handle *hDec = (OEM_DECRYPTOR_Handle *)tDecCtx;

	if(hDec == NULL)
	{
		return 0;
	}

	if(hDec->pEkl != NULL)
	{
		NEXUS_Memory_Free(hDec->pEkl);
	}
	if(hDec->tDecType != OEM_DECRYPTOR_TYPE_SECURE_KEYHANDLE)
	{
		if(hDec->KeyHandle != NULL)
		{
			(void)NEXUS_Security_FreeKeySlot(hDec->KeyHandle);
			hDec->KeyHandle = NULL;
		}
	}
	if(hDec->cryptoHandle != NULL)
	{
		(void)CommonCrypto_Close(hDec->cryptoHandle);
		hDec->cryptoHandle = NULL;
	}
#if (NEXUS_VERSION >= 1304)
	if(hDec->hDmaJob != NULL)
	{
		NEXUS_DmaJob_Destroy (hDec->hDmaJob);
		hDec->hDmaJob = NULL;
	}
	if(hDec->hDma != NULL)
	{
		NEXUS_Dma_Close(hDec->hDma);
		hDec->hDma = NULL;
	}
#endif
	if(hDec->tmp_iv)
	{
		(void)NEXUS_Memory_Free(hDec->tmp_iv);
		hDec->tmp_iv = NULL;
	}
	if(hDec->tmp_ecount_buf)
	{
		(void)NEXUS_Memory_Free(hDec->tmp_ecount_buf);
		hDec->tmp_ecount_buf = NULL;
	}

	if(hDec != NULL)
	{
		(void)free(hDec);
		hDec = NULL;
	}

	return 0;
}


inline static int bcm_secR2RCryptoOperation(
    NEXUS_KeySlotHandle keyHandle,
    void* data,
    unsigned int xDataSize
)
{
    NEXUS_DmaHandle dma;
    NEXUS_DmaJobSettings jobSettings;
    NEXUS_DmaJobHandle dmaJob;
    NEXUS_DmaJobBlockSettings blockSettings;
	NEXUS_DmaJobStatus jobStatus;

    unsigned char *input_data;
    unsigned char *output_data;
    int    retCode  =  0;
    BKNI_EventHandle event;

    BDBG_MSG(("Enter %s", __FUNCTION__));

#if(CFG_BIG_ENDIAN==1)
    size_t i;
#endif
    BKNI_CreateEvent(&event);

    /* Need to allocate memory for M2M input and output buffers. */
    NEXUS_Memory_Allocate(sizeof(unsigned char) * xDataSize, NULL, (void *)&input_data);
    NEXUS_Memory_Allocate(sizeof(unsigned char) * xDataSize, NULL, (void *)&output_data);

    /* Copy the input data to M2M input buffer. */
    BKNI_Memcpy((void *)input_data, (void *)data, xDataSize );

#if(CFG_BIG_ENDIAN==1)
    for(i=0; i<xDataSize; i+=4)
        byteSwap(&(input_data[i]));
#endif

    /* Open DMA handle */
    dma = NEXUS_Dma_Open(0, NULL);

    NEXUS_DmaJob_GetDefaultSettings(&jobSettings);
    jobSettings.numBlocks = 1;
    jobSettings.keySlot = keyHandle;
    jobSettings.dataFormat = NEXUS_DmaDataFormat_eBlock;
    jobSettings.completionCallback.callback = NULL;
    jobSettings.completionCallback.context = event;

    dmaJob = NEXUS_DmaJob_Create(dma, &jobSettings);

    NEXUS_DmaJob_GetDefaultBlockSettings(&blockSettings);
    blockSettings.pSrcAddr = input_data;
    blockSettings.pDestAddr = output_data;
    blockSettings.blockSize = xDataSize;
    blockSettings.resetCrypto = true;
    blockSettings.scatterGatherCryptoStart = true;
    blockSettings.scatterGatherCryptoEnd = true;
    blockSettings.cached = true;

	BDBG_MSG(("PROCESS DECRYPTION : %s",__FUNCTION__));

	NEXUS_DmaJob_ProcessBlocks(dmaJob, &blockSettings, 1);

	jobStatus.currentState = NEXUS_DmaJobState_eUnknown;
	while (jobStatus.currentState != NEXUS_DmaJobState_eComplete)
	{
		NEXUS_DmaJob_GetStatus(dmaJob, &jobStatus);
		if ( jobStatus.currentState == NEXUS_DmaJobState_eComplete )
			break;
		usleep(1);
	}

#if(CFG_BIG_ENDIAN==1)
    for(i=0; i<xDataSize; i+=4)
        byteSwap(&(output_data[i]));
#endif

    /* Copy the data to M2M output buffer. */
    BKNI_Memcpy((void *)data, (void *)output_data, xDataSize );

	BDBG_MSG(("DECRYPTION CLEAR : %s",__FUNCTION__));

    /* Done.  Close DMA. */
    NEXUS_DmaJob_Destroy(dmaJob);

    NEXUS_Dma_Close(dma);
    BKNI_DestroyEvent(event);

    NEXUS_Memory_Free(output_data);
    NEXUS_Memory_Free(input_data);

    BDBG_MSG(("LEAVE: %s",__FUNCTION__));
    return retCode;
}

inline static int bcm_do_decrypt_dma(OEM_DECRYPTOR_Handle *hDec, unsigned char* data, unsigned int size, unsigned char *key, unsigned char *iv)
{
#if (NEXUS_VERSION >= 1304)
	NEXUS_DmaJobBlockSettings blockSettings;
	NEXUS_DmaJobSettings jobSettings;
	NEXUS_DmaJobStatus jobStatus;
	NEXUS_SecurityClearKey ClearKey;
	NEXUS_Error nResult;

	/* Key Setting */
	if(hDec->tDecType == OEM_DECRYPTOR_TYPE_AES_128_CTR)
	{
#if 1
		BDBG_MSG(("key = %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
			key[0], key[1], key[2], key[3], key[4], key[5], key[6], key[7],
			key[8], key[9], key[10], key[11], key[12], key[13], key[14], key[15]));
		BDBG_MSG(("iv	= %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
			iv[0], iv[1], iv[2], iv[3], iv[4], iv[5], iv[6], iv[7],
			iv[8], iv[9], iv[10], iv[11], iv[12], iv[13], iv[14], iv[15]));
#endif
		NEXUS_Security_GetDefaultClearKey(&ClearKey);
		ClearKey.keySize = 16;
		ClearKey.keyEntryType = NEXUS_SecurityKeyType_eClear;
		ClearKey.keyIVType = NEXUS_SecurityKeyIVType_eNoIV;
		memcpy(ClearKey.keyData, key, ClearKey.keySize);

		nResult = NEXUS_Security_LoadClearKey(hDec->KeyHandle, &ClearKey);
		if (nResult != NEXUS_SUCCESS)
		{
			BDBG_ERR(("--NEXUS_Security_LoadClearKey failed\n"));
			return (-1);
		}

		NEXUS_Security_GetDefaultClearKey(&ClearKey);
		ClearKey.keySize = 16;
		ClearKey.keyEntryType = NEXUS_SecurityKeyType_eClear;
		ClearKey.keyIVType = NEXUS_SecurityKeyIVType_eIV;
		memcpy(ClearKey.keyData, iv, ClearKey.keySize);

		nResult = NEXUS_Security_LoadClearKey(hDec->KeyHandle, &ClearKey);
		if (nResult != NEXUS_SUCCESS)
		{
			BDBG_ERR(("++NEXUS_Security_LoadClearKey failed\n"));
			return (-1);
		}
	}
	else if(hDec->tDecType == OEM_DECRYPTOR_TYPE_AES_128_CBC)
	{
#if 1
		BDBG_MSG(("key = %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
			key[0], key[1], key[2], key[3], key[4], key[5], key[6], key[7],
			key[8], key[9], key[10], key[11], key[12], key[13], key[14], key[15]));
		BDBG_MSG(("iv	= %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
			iv[0], iv[1], iv[2], iv[3], iv[4], iv[5], iv[6], iv[7],
			iv[8], iv[9], iv[10], iv[11], iv[12], iv[13], iv[14], iv[15]));
#endif
		NEXUS_Security_GetDefaultClearKey(&ClearKey);
		ClearKey.keySize = 16;
		ClearKey.keyEntryType = NEXUS_SecurityKeyType_eClear;
		ClearKey.keyIVType = NEXUS_SecurityKeyIVType_eNoIV;
		memcpy(ClearKey.keyData, key, ClearKey.keySize);

		nResult = NEXUS_Security_LoadClearKey(hDec->KeyHandle, &ClearKey);
		if (nResult != NEXUS_SUCCESS)
		{
			BDBG_ERR(("--NEXUS_Security_LoadClearKey failed\n"));
			return (-1);
		}

		NEXUS_Security_GetDefaultClearKey(&ClearKey);
		ClearKey.keySize = 16;
		ClearKey.keyEntryType = NEXUS_SecurityKeyType_eClear;
		ClearKey.keyIVType = NEXUS_SecurityKeyIVType_eIV;
		memcpy(ClearKey.keyData, iv, ClearKey.keySize);

		nResult = NEXUS_Security_LoadClearKey(hDec->KeyHandle, &ClearKey);
		if (nResult != NEXUS_SUCCESS)
		{
			BDBG_ERR(("++NEXUS_Security_LoadClearKey failed\n"));
			return (-1);
		}
	}
	else
	{
		BDBG_ERR(("Do not support dec type [%x]", hDec->tDecType));
		return (-1);
	}

	NEXUS_DmaJob_GetSettings(hDec->hDmaJob, &jobSettings);

	jobSettings.numBlocks = 1;
	jobSettings.completionCallback.callback = NULL;
	jobSettings.dataFormat = NEXUS_DmaDataFormat_eBlock;
	jobSettings.keySlot = hDec->KeyHandle;

	nResult = NEXUS_DmaJob_SetSettings(hDec->hDmaJob, &jobSettings);
	if (nResult != NEXUS_SUCCESS)
	{
		BDBG_ERR(("--NEXUS_Security_LoadClearKey failed\n"));
		return (-1);
	}

	NEXUS_DmaJob_GetDefaultBlockSettings (&blockSettings);
	blockSettings.pSrcAddr = data;
	blockSettings.pDestAddr = data;
	blockSettings.blockSize = size;
	blockSettings.cached = true;
	blockSettings.resetCrypto = true;
	blockSettings.scatterGatherCryptoStart = true;
	blockSettings.scatterGatherCryptoEnd = true;

#if 1
	BDBG_MSG(("  ==> %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x [%d]",
		data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
		data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15], size));
#endif

	NEXUS_DmaJob_ProcessBlocks(hDec->hDmaJob, &blockSettings, 1);

	for(;;)
	{
		NEXUS_DmaJob_GetStatus(hDec->hDmaJob, &jobStatus);
		if ( jobStatus.currentState == NEXUS_DmaJobState_eComplete )
			break;
		BKNI_Delay(1);
	}
#if 1
	BDBG_MSG(("  <== %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x [%d]",
		data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
		data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15], size));
#endif

#else /*( NEXUS_VERSION <= 1034) */
	UNUSED_PARAM(hDec);
	UNUSED_PARAM(data);
	UNUSED_PARAM(size);
	UNUSED_PARAM(key);
	UNUSED_PARAM(iv);
#endif
	return 0;
}

inline static int bcm_do_decrypt(OEM_DECRYPTOR_Handle *hDec, unsigned char *data, unsigned int size, unsigned char *key, unsigned char *iv)
{
	uint32_t ekl_size = 0;
	/*	uint8_t* data_in_nexus=NULL;*/
	NEXUS_Error nResult;
	NEXUS_DmaJobBlockSettings blks[2];
	CommonCryptoJobSettings jobSettings;

	/*************************
	 *	1. NULL Check
	 *************************/
	if(data == NULL)
	{
		BDBG_ERR(("Buffer is NULL!\n"));
		return -1;
	}
	if(hDec->tDecType == OEM_DECRYPTOR_TYPE_WMDrmPd)
	{
		if(size < 16)
		{
			return -1;
		}
		BKNI_Memcpy(hDec->pEkl, key, 80);
		ekl_size = 80;
	}
	else if(hDec->tDecType == OEM_DECRYPTOR_TYPE_AES_128_CTR)
	{
		BKNI_Memcpy(hDec->pEkl, key+8, 8); /* Copy 8bytes for Key reorder */
		BKNI_Memcpy(hDec->pEkl+8, key, 8); /* Copy 8bytes for Key reorder */
		/*/BKNI_Memset(&hDec->pEkl[16], 0, 8);*/
		BKNI_Memcpy(&hDec->pEkl[16], iv+8, 8);
		BKNI_Memcpy(&hDec->pEkl[24], iv, 8);
		ekl_size = 32;

		BDBG_MSG(("key = %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
				key[0], key[1], key[2], key[3], key[4], key[5], key[6], key[7],
				key[8], key[9], key[10], key[11], key[12], key[13], key[14], key[15]));
		BDBG_MSG(("iv  = %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
			iv[0], iv[1], iv[2], iv[3], iv[4], iv[5], iv[6], iv[7],
			iv[8], iv[9], iv[10], iv[11], iv[12], iv[13], iv[14], iv[15]));
	}
	else if(hDec->tDecType == OEM_DECRYPTOR_TYPE_AES_128_CBC)
	{
		BKNI_Memcpy(hDec->pEkl, key+8, 8); /* Copy 8bytes for Key reorder */
		BKNI_Memcpy(hDec->pEkl+8, key, 8); /* Copy 8bytes for Key reorder */
		BKNI_Memcpy(&hDec->pEkl[16], iv+8, 8); /* IV reorder */
		BKNI_Memcpy(&hDec->pEkl[24], iv, 8); /* IV reorder */
		ekl_size = 32;
	}
#if 0
	else if(hDec->tDecType == DECRYPTOR_TYPE_AES_128_ECB)
	{
		BKNI_Memcpy(hDec->pEkl, key+8, 8); /* Copy 8bytes for Key reorder */
		BKNI_Memcpy(hDec->pEkl+8, key, 8); /* Copy 8bytes for Key reorder */
		BKNI_Memset(&hDec->pEkl[16], 0x00, 16); /* IV Not Use */
		ekl_size = 32;
	}
#endif
	else
	{
		return -1;
	}

	NEXUS_DmaJob_GetDefaultBlockSettings(&blks[0]);
	blks[0].pSrcAddr				 = hDec->pEkl;
	blks[0].pDestAddr				 = hDec->pEkl;
	blks[0].blockSize				 = ekl_size;
	blks[0].cached					 = true;
	blks[0].resetCrypto 			 = true;
	blks[0].scatterGatherCryptoStart = true;
	blks[0].scatterGatherCryptoEnd	 = false;

	NEXUS_DmaJob_GetDefaultBlockSettings(&blks[1]);
	blks[1].pSrcAddr				 = data;
	blks[1].pDestAddr				 = data;
	blks[1].blockSize				 = size;
	blks[1].cached					 = true;
	blks[1].resetCrypto 			 = true;
	blks[1].scatterGatherCryptoStart = false;
	blks[1].scatterGatherCryptoEnd	 = true;

	CommonCrypto_GetDefaultJobSettings(&jobSettings);

	jobSettings.keySlot = hDec->KeyHandle;

	BDBG_MSG(("  ==> %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x [%d]",
		data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
		data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15], size));

	nResult = CommonCrypto_DmaXfer(hDec->cryptoHandle, &jobSettings, (NEXUS_DmaJobBlockSettings*)blks, 2);
	if(nResult != NEXUS_SUCCESS)
	{
		BDBG_ERR(("%s - CommonCrypto_DmaXfer failed rc %x\n", __FUNCTION__, nResult));
	}

	BDBG_MSG(("  <== %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x [%d]",
		data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
		data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15], size));
	return 0;
}

inline static int bcm_do_decrypt_sec(OEM_DECRYPTOR_Handle *hDec, unsigned char *data, unsigned int size, unsigned char *iv)
{
    NEXUS_SecurityClearKey key;

	BDBG_MSG(("ENTER: %s",__FUNCTION__));

    if(hDec == NULL ) {
        BDBG_ERR(("%s SEC_ERROR_BAD_PARAMETER(NO DEC HANDLE)", __FUNCTION__));
        return -1;
    }
	if(hDec->KeyHandle == NULL) {
        BDBG_ERR(("%s SEC_ERROR_BAD_PARAMETER(NO KEY HANDLE)", __FUNCTION__));
        return -1;
    }

	if(data == NULL ) {
        BDBG_ERR(("%s SEC_ERROR_BAD_PARAMETER Data", __FUNCTION__));
        return -1;
    }
	BDBG_MSG(("LOAD KEYS: %s",__FUNCTION__));

    /* load IV key if needed */
    if (iv != NULL)
    {
        const unsigned char *pxIv = iv;
        size_t xIvSize = 16;

        BDBG_MSG(("%s IV load needed ", __FUNCTION__));

        BKNI_Memset((void *)&key, 0, sizeof(key));
        NEXUS_Security_GetDefaultClearKey(&key);
        key.keyIVType	= NEXUS_SecurityKeyIVType_eIV;
        key.keyEntryType = NEXUS_SecurityKeyType_eIv;
        key.keySize = 16;

        if(xIvSize == 8) {
            BKNI_Memcpy(&(key.keyData[8]), pxIv, xIvSize);
        }
        else
            BKNI_Memcpy(key.keyData, pxIv, xIvSize);

        /* Load IV. */
        if (NEXUS_Security_LoadClearKey(hDec->KeyHandle , &key) != 0)
        {
            BDBG_WRN(("Load IV failed"));
            return -1;
        }
    }
	BDBG_MSG(("DECRYPTION : %s",__FUNCTION__));

    if(bcm_secR2RCryptoOperation(hDec->KeyHandle , data, size)) {
        BDBG_ERR(("%s secR2RCryptoOperation error", __FUNCTION__));
        return -1;
    }
    BDBG_MSG(("LEAVE: %s",__FUNCTION__));
    return 0;

}

int BCM_DECRYPTOR_SOC_DoDecrypt(OEM_DECRYPTOR_Context_t tDecCtx, unsigned char *pData, unsigned int ulSize, unsigned char *key, unsigned char *iv)
{
	OEM_DECRYPTOR_Handle *hDec = (OEM_DECRYPTOR_Handle *)tDecCtx;
	int ret = 0;


	if(hDec == NULL)
	{
		return -1;
	}

	switch(hDec->tDecType)
	{
		case OEM_DECRYPTOR_TYPE_AES_128_CTR:
		case OEM_DECRYPTOR_TYPE_AES_128_CBC:
#if (NEXUS_VERSION >= 1304)
			/*TIME_START;*/
			ret = bcm_do_decrypt_dma(hDec, pData, ulSize, key, iv);
			/*TIME_END;*/
			break;
#endif
		case OEM_DECRYPTOR_TYPE_WMDrmPd:
#if 0
		case DECRYPTOR_TYPE_AES_128_ECB:
#endif
			ret = bcm_do_decrypt(hDec, pData, ulSize, key, iv);
			break;
		case OEM_DECRYPTOR_TYPE_SECURE_KEYHANDLE:
			ret = bcm_do_decrypt_sec(hDec, pData, ulSize, iv);
			break;
		default:
			break;
	}
	return ret;
}

#endif /*/ #ifdef HWDEC*/

#endif


