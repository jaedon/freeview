
/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif

#include "linden_trace.h"
#include "vkernel.h"
#include "drv_err.h"
#include "drv_csd.h"

#include "nexus_types.h"
#include "nexus_platform.h"
/*#include "nexus_base.h"*/
#include "nexus_memory.h"
#include "nexus_dma.h"

#include "nexus_security.h"
#include "nexus_otpmsp.h"
#include "nexus_keyladder.h"

#if ((NEXUS_VERSION >= 1502) && (BCHP_CHIP == 7241))
#else
#include "bchp_sun_top_ctrl.h"
#endif

#if defined(CONFIG_CAS_NA)
#if defined(CONFIG_CAS_NA_NOCS_3X)
#include "nocs_csd_impl.h"
#include "nocs_csd.h"
#include "drv_flash.h"
#include "flash_map.h"
#if (NEXUS_VERSION >= 1501)
#include "csd_brcm_extensions.h"
#endif
#if defined(CONFIG_CAS_NA_NOCS_SEC)
#define DEBUG_DUMP_FPK 1
#include "ca_sec.h"
#endif
#if defined(CONFIG_CAS_NA_NOCS_DPT)
#include "ca_dpt.h"
#endif
#else
#include "nocs1x_csd_impl.h"
#include "nocs1x_csd.h"
#include "drv_flash.h"
#include "flash_map.h"
#endif
#endif
#if defined(CONFIG_NOR_FLASH)
#include "di_err.h"
#endif


/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define DEBUG_MODULE        TRACE_MODULE_DRV_CSD

#define UNUSED_PARAM(x)  ((void)x)

/**********************************************************************************************
						 CDS implementation specific structures
**********************************************************************************************/
#if defined(CONFIG_CAS_NA_NOCS_3X)
#else
#define CSD_NUID_LENGTH		(32)    /* in bits,  possible to be 64 or whatever*/
#endif

#define CHIP_ID_INDEX	4
#define P_CSD_MARKET_ID_DEFAULT	0 //0xFFFFFFFF
#define P_CSD_HOST_BOOT_DECRYPT_DEFAULT	8

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
typedef enum
{
	CSD_CRYPTO_MODE_TDES_ECB,
	CSD_CRYPTO_MODE_TDES_CBC,
	CSD_CRYPTO_MODE_AES_ECB,
	CSD_CRYPTO_MODE_AES_CBC,
	LAST_CSD_CRYPTO_MODE
} CSDCryptoMode;

typedef struct
{
	NEXUS_KeySlotHandle	keyHandle;
	NEXUS_DmaHandle		hDmaCsd;
	CSDCryptoMode		cryptoMode;
    unsigned int		isEncrypt;
} CSDRam2RamChannelID;

typedef struct
{
	size_t           	length;
	unsigned char*  	pData;
} CSDDataBlock;

typedef CSDDataBlock CSDKey;

typedef unsigned long int CSDDerivationLevelNumber;


/*******************************************************************/
/*********************** extern function prototypes ****************/
/*******************************************************************/
extern NEXUS_DmaHandle g_ulDmaHandle;
extern NEXUS_Error NEXUS_Platform_ReadRegister( uint32_t address, uint32_t *pValue );
#if defined(CONFIG_CAS_NA_NOCS_SEC)
extern TSecStatus secEncryptFlashProtKey(const TUnsignedInt8* pxInput, TUnsignedInt8* pxOutput, size_t xSize);
extern TSecStatus secGetChipsetExtension(const TChar** ppxChipsetExtension);
extern TSecStatus secGetChipsetRevision(const TChar** ppxChipsetRevision);
#endif

#if defined(CONFIG_CAS_CX)
extern DRV_CSD_ChipsetOTP ChipsetOTP[];
extern HUINT32 DRV_CFG_GetChipsetOTPMaxNum(void);
#endif

/**********************************************************************************************
						 CDS implementation specific  variables
**********************************************************************************************/

//TCsdInitParameters   	hCsdInitParameters;/* backup App level,  configuration parameters,*/

#if defined(CONFIG_CAS_NA_NOCS_3X)
#else
static unsigned long 	gCrcTab [256];
#endif
static unsigned char 	gJTAG_FusingProcess = FALSE;	/* JTAG fusing 진행 하면 TRUE로 세팅. */

/**********************************************************************************************
						 CDS implementation specific internal functions/macros
**********************************************************************************************/

/* 1 block transfer per DMA-MEM transaction, it can be updated to use multiple blocks per transaction.
    the max size = 64MB per block, so far this is enough

   - the memory for pBufSrc, pBufDest are allocated by App using BMEM_AllocAligned(), not released by CSD
   - this function does NOT close DMA handle; yet does free the DMA-MEM handle and its memory.
*/

#if defined(CONFIG_CAS_NA_NOCS_SEC)
static HUINT32 byteswap32(HUINT32 byte)
{
	byte = ((byte << 8) & 0xFF00FF00) | ((byte >> 8) & 0x00FF00FF);
	return (byte >> 16) | (byte << 16);
}
#endif

#if defined(CONFIG_CAS_NA_NOCS_3X)
#else
static unsigned long cmpCrc32 (unsigned char * xbufInp, int xbufLen)
{
	unsigned long crcVal;

	crcVal = 0xFFFFFFFF;
	while (xbufLen--)
	{
		crcVal = ((crcVal >> 8) & 0x00FFFFFF) ^ gCrcTab[(crcVal ^ *xbufInp++) & 0xFF];
	}

	return (crcVal ^ 0xFFFFFFFF);
}

static void genCrc32Tab (void)
{
  	unsigned long crcVal, poly;
  	register int i, j;

  	poly = 0xEDB88320L;
  	for (i = 0; i < 256; i++)
	{
		crcVal = i;
		for (j = 8; j > 0; j--)
		{
			if (crcVal & 1)
			{
				crcVal = (crcVal >> 1) ^ poly;
			}
			else
			{
				crcVal >>= 1;
			}
		}

		gCrcTab [i] = crcVal;
  	}
}

static DRV_Error csdDmaBlkTransfer(
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
    NEXUS_Error rc;

    NEXUS_DmaJob_GetDefaultSettings(&jobSettings);

    jobSettings.numBlocks                   = 1;
    jobSettings.keySlot                     = keyHandle;
	jobSettings.dataFormat                  = NEXUS_DmaDataFormat_eBlock;
    jobSettings.completionCallback.callback = NULL;

    dmaJob = NEXUS_DmaJob_Create(dma, &jobSettings);

    NEXUS_DmaJob_GetDefaultBlockSettings(&blockSettings);

    blockSettings.pSrcAddr                 = pBufSrc;
    blockSettings.pDestAddr                = pBufDest;
    blockSettings.blockSize                = uBufLeng;
	blockSettings.resetCrypto              = true;
	blockSettings.scatterGatherCryptoStart = true;
	blockSettings.scatterGatherCryptoEnd   = true;
	blockSettings.cached                   = true;

	NEXUS_DmaJob_SetBlockSettings(dmaJob, 0, &blockSettings);
	rc = NEXUS_DmaJob_Start(dmaJob);

	/* Clear current state */
	jobStatus.currentState = NEXUS_DmaJobState_eUnknown;

	while(jobStatus.currentState != NEXUS_DmaJobState_eComplete)
    {
        rc = NEXUS_DmaJob_GetStatus(dmaJob, &jobStatus);
		if (rc != NEXUS_SUCCESS)
		{
			PrintError("[csdDmaBlkTransder] NEXUS_DmaJob_GetStatus error(0x%x)\n", rc);

			return DRV_ERR;
		}

		VK_TASK_Sleep(1);
    }

	NEXUS_DmaJob_Destroy(dmaJob);

	return DRV_OK;
}

static DRV_Error csdAllocRam2RamKey(
	CSDRam2RamChannelID *xRam2RamChannelID,
	NEXUS_DmaHandle	hDmaCsd,
	CSDCryptoMode cryptoMode,
	unsigned int isEncrypt
)
{
	NEXUS_SecurityKeySlotSettings keySlotSettings;

    if ( xRam2RamChannelID == NULL )
    {
        PrintError("param is NULL\n");

        return DRV_ERR_INVALID_PARAMETER;
    }

    NEXUS_Security_GetDefaultKeySlotSettings(&keySlotSettings);
	keySlotSettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;
	xRam2RamChannelID->keyHandle = NEXUS_Security_AllocateKeySlot(&keySlotSettings);

    switch(cryptoMode)
    {
        case CSD_CRYPTO_MODE_TDES_ECB:
        case CSD_CRYPTO_MODE_TDES_CBC:
        case CSD_CRYPTO_MODE_AES_ECB:
        case CSD_CRYPTO_MODE_AES_CBC:
        case LAST_CSD_CRYPTO_MODE:
            xRam2RamChannelID->cryptoMode = cryptoMode;
            break;
        default:
            xRam2RamChannelID->cryptoMode = LAST_CSD_CRYPTO_MODE;
            break;
    }

    xRam2RamChannelID->hDmaCsd = hDmaCsd;
	xRam2RamChannelID->isEncrypt  = isEncrypt;

    return DRV_OK;
}

static void csdFreeRam2RamKey(CSDRam2RamChannelID *xRam2RamChannelID)
{
    NEXUS_Security_FreeKeySlot(xRam2RamChannelID->keyHandle);

    xRam2RamChannelID->cryptoMode = LAST_CSD_CRYPTO_MODE;
    xRam2RamChannelID->hDmaCsd = NULL;
    xRam2RamChannelID->isEncrypt  = 0;
}

static DRV_Error csdSetRam2RamKey(
    CSDRam2RamChannelID *xRam2RamChannelID,
    CSDDerivationLevelNumber xDerivationLevelNumber,
	const CSDKey pxProtectingKeyTable[],
    CSDCryptoMode xCryptoMode
)
{
	NEXUS_Error							errCode;
	unsigned char 						caData[16] = {0x0} ;
	unsigned int 						i;
	NEXUS_KeySlotHandle					keyHandle;
	NEXUS_SecurityAlgorithmSettings		NexusConfig;
	NEXUS_SecurityEncryptedSessionKey	SessionKey;
	NEXUS_SecurityEncryptedControlWord	CW;
	NEXUS_SecurityClearKey 				clearKey;

	if ( xRam2RamChannelID->hDmaCsd == NULL)
	{
		PrintError("parm is NULL!!\n");

		return DRV_ERR_INVALID_PARAMETER;
	}

	if ((xDerivationLevelNumber != 0) && (xDerivationLevelNumber != 2))
	{
		PrintError("not support this level %d\n", xDerivationLevelNumber);

		return DRV_ERR_INVALID_PARAMETER;
	}

	if (xCryptoMode != CSD_CRYPTO_MODE_TDES_ECB)
	{
		PrintError("not support this mode %d\n", xCryptoMode);

		return DRV_ERR_INVALID_PARAMETER;
	}

	VK_MEM_Memset((void *)&NexusConfig, 0, sizeof(NexusConfig));

	NEXUS_Security_GetDefaultClearKey(&clearKey);
	switch (xRam2RamChannelID->cryptoMode)
	{
		case CSD_CRYPTO_MODE_TDES_ECB:
			NexusConfig.algorithm     = NEXUS_SecurityAlgorithm_e3DesAba;
			NexusConfig.algorithmVar  = NEXUS_SecurityAlgorithmVariant_eEcb;
			break;
		case CSD_CRYPTO_MODE_TDES_CBC:
			NexusConfig.algorithm     = NEXUS_SecurityAlgorithm_e3DesAba;
			NexusConfig.algorithmVar  = NEXUS_SecurityAlgorithmVariant_eCbc;
			break;
		case CSD_CRYPTO_MODE_AES_ECB:
			NexusConfig.algorithm     = NEXUS_SecurityAlgorithm_eAes;
			NexusConfig.algorithmVar  = NEXUS_SecurityAlgorithmVariant_eEcb;
			break;
		case CSD_CRYPTO_MODE_AES_CBC:
			NexusConfig.algorithm     = NEXUS_SecurityAlgorithm_eAes;
			NexusConfig.algorithmVar  = NEXUS_SecurityAlgorithmVariant_eCbc;
			break;
		default:
			PrintError("not support this mode %d\n", xRam2RamChannelID->cryptoMode);
			return DRV_ERR_INVALID_PARAMETER;
	};

	/* check for valid key length */
	if (pxProtectingKeyTable[0].length != 16  )
	{
		PrintError("invalid length %d\n", pxProtectingKeyTable[0].length);

		return DRV_ERR_INVALID_PARAMETER;
	}

	if (xDerivationLevelNumber == 2)
	{
		if(pxProtectingKeyTable[1].length != 16 )
		{
			PrintError("invalid length %d\n", pxProtectingKeyTable[1].length);

			return DRV_ERR_INVALID_PARAMETER;
		}
	}

	if (xDerivationLevelNumber == 2)
	{
		NexusConfig.dest             = NEXUS_SecurityAlgorithmConfigDestination_eCa;
		NexusConfig.keyDestEntryType = NEXUS_SecurityKeyType_eOdd;

		if (xRam2RamChannelID->isEncrypt)
			NexusConfig.operation = NEXUS_SecurityOperation_eEncrypt;
		else
			NexusConfig.operation = NEXUS_SecurityOperation_eDecrypt;

        keyHandle = xRam2RamChannelID->keyHandle;
		errCode = NEXUS_Security_ConfigAlgorithm(keyHandle, &NexusConfig);
		if (errCode != 0)
		{
			PrintError("NEXUS_Security_ConfigAlgorithm(odd) errCode: %x\n", errCode);

			return DRV_ERR;
		}

		for(i = 0; i < 8; i++)
		{
			caData[i]     = pxProtectingKeyTable[0].pData[i + 8];
			caData[i + 8] = pxProtectingKeyTable[0].pData[i];
		}

#if defined(CONFIG_SECURITY_EXTENSION)
		NEXUS_Security_GetDefaultSessionKeySettings(&SessionKey);
#else
		VK_MEM_Memset((void *)&SessionKey, 0x0, sizeof(NEXUS_SecurityEncryptedControlWord));
#endif
		SessionKey.keyladderID   = NEXUS_SecurityKeyladderID_eA;
		SessionKey.keyladderType = NEXUS_SecurityKeyladderType_e3Des;
		SessionKey.swizzleType   = NEXUS_SecuritySwizzleType_eNone;
		SessionKey.cusKeyL       = 0;
		SessionKey.cusKeyH       = 0;
		SessionKey.cusKeyVarL    = 0;
		SessionKey.cusKeyVarH    = 0;
    	SessionKey.rootKeySrc    = NEXUS_SecurityRootKeySrc_eOtpKeyA;
	 	SessionKey.bRouteKey     = false;
	 	SessionKey.operation     = NEXUS_SecurityOperation_eDecrypt;
	 	SessionKey.keyEntryType  = NEXUS_SecurityKeyType_eOdd;
		VK_memcpy((void *)SessionKey.keyData, (void *)caData, sizeof(caData));
		errCode = NEXUS_Security_GenerateSessionKey(keyHandle, &SessionKey);

		if (errCode != 0)
		{
			PrintError("NEXUS_Security_GenerateSessionKey errCode: %x\n", errCode );

			return DRV_ERR;
		}

		for(i = 0; i < 16; i++)
		{
			caData[i] = pxProtectingKeyTable[1].pData[i];
		}

#if defined(CONFIG_SECURITY_EXTENSION)
		NEXUS_Security_GetDefaultControlWordSettings(&CW);
#else
		VK_MEM_Memset((void *)&CW, 0x0, sizeof(NEXUS_SecurityEncryptedControlWord));
#endif
		CW.keyladderID   = NEXUS_SecurityKeyladderID_eA;
		CW.keyladderType = NEXUS_SecurityKeyladderType_e3Des;
		CW.bRouteKey     = true;
		CW.operation     = NEXUS_SecurityOperation_eDecrypt;
		CW.keyEntryType  = NEXUS_SecurityKeyType_eOdd;
		VK_memcpy((void *)CW.keyData, (void *)caData, sizeof(caData));
		CW.keySize       = 16;
		errCode = NEXUS_Security_GenerateControlWord(  keyHandle , &CW);

		if (errCode != 0)
		{
			PrintError("NEXUS_Security_GenerateKey4or5(k3) errCode: %x\n", errCode );

			return DRV_ERR;
		}
	}
	else if (!xDerivationLevelNumber )
	{
		NexusConfig.dest             = NEXUS_SecurityAlgorithmConfigDestination_eCa;
		NexusConfig.keyDestEntryType = NEXUS_SecurityKeyType_eOdd;
        keyHandle = xRam2RamChannelID->keyHandle;
		if (xRam2RamChannelID->isEncrypt)
			NexusConfig.operation = NEXUS_SecurityOperation_eEncrypt;
		else
			NexusConfig.operation = NEXUS_SecurityOperation_eDecrypt;

		errCode = NEXUS_Security_ConfigAlgorithm( keyHandle, &NexusConfig);
		if (errCode != 0)
		{
			PrintError("NEXUS_Security_ConfigAlgorithm(odd) errCode: %x\n", errCode );

			return DRV_ERR;
		}

		clearKey.keySize      = 16;
	 	clearKey.keyEntryType = NEXUS_SecurityKeyType_eOdd;

		VK_memcpy(clearKey.keyData,  pxProtectingKeyTable[0].pData, clearKey.keySize);

		errCode = NEXUS_Security_LoadClearKey(keyHandle, &clearKey);
  		if (errCode != 0)
		{
			PrintError("NEXUS_Security_LoadClearKey (odd) errCode: %x\n", errCode );

			return DRV_ERR;
  		}
	}
	else
	{
		PrintError("Not Support!!\n");

		return DRV_ERR_INVALID_PARAMETER;
	}

	return DRV_OK;
}

static DRV_Error  csdEncryptData(
	CSDRam2RamChannelID   	*xRam2RamChannelID,
	const CSDDataBlock		*pxSrc,
	CSDDataBlock			*pxDst,
	CSDCryptoMode			xCryptoMode
)
{
	DRV_Error	errCode;

	if (xRam2RamChannelID->hDmaCsd == NULL)
	{
		PrintError("parm is NULL!!\n");

		return DRV_ERR_INVALID_PARAMETER;
	}

#if 0
	if(!xRam2RamChannelID->isEncrypt)
	{
		PrintError("inavlid parm!!\n");

		return DRV_ERR_INVALID_PARAMETER;
	}
#endif

	/* changed on 9/12 according to Hans update if ( xCryptoMode != CSD_CRYPTO_MODE_TDES_ECB)  return CSD_ERROR_CRYPTO_MODE_NOT_SUPPORTED;  */
	if( (xRam2RamChannelID->cryptoMode != xCryptoMode) || ( xCryptoMode == LAST_CSD_CRYPTO_MODE ) )
	{
		PrintError("crypto mode is not matched!! (%d-%d)\n", xRam2RamChannelID->cryptoMode, xCryptoMode);

		return DRV_ERR_INVALID_PARAMETER;
	}

	if( (pxSrc->length%8) || (pxSrc->length>pxDst->length) )
	{
		PrintError("invalid length %d\n", pxSrc->length);

		return DRV_ERR_INVALID_PARAMETER;
	}

	if ((pxSrc->length % 16)
		&& ((xCryptoMode == CSD_CRYPTO_MODE_AES_ECB) || (xCryptoMode ==  CSD_CRYPTO_MODE_AES_CBC)))
	{
		PrintError("Input data length mismatched! \n");

		return DRV_ERR_INVALID_PARAMETER;
	}

	errCode= csdDmaBlkTransfer(pxSrc->pData,
							   pxDst->pData,
							   pxSrc->length,
							   xRam2RamChannelID->keyHandle,
							   xRam2RamChannelID->hDmaCsd);
	if (errCode != DRV_OK)
	{
		PrintError("csdDmaBlkTransfer() errCode: %x\n", errCode );

		return DRV_ERR;
	}

	return DRV_OK;
}
#endif /* CONFIG_CAS_NA_NOCS_3X */

#if 0
static DRV_Error  csdDecryptData(
	CSDRam2RamChannelID   	*xRam2RamChannelID,
	const CSDDataBlock		*pxSrc,
	CSDDataBlock			*pxDst,
	CSDCryptoMode			xCryptoMode
)
{

	BERR_Code						errCode;


	if (xRam2RamChannelID->hDmaCsd == NULL)
	{
		PrintError("parm is NULL!!\n");
		return DRV_ERR_INVALID_PARAMETER;
	}

	if(xRam2RamChannelID->isEncrypt)
	{
		PrintError("inavlid parm!!\n");
		return DRV_ERR_INVALID_PARAMETER;
	}

	if( (xRam2RamChannelID->cryptoMode != xCryptoMode) || ( xCryptoMode == LAST_CSD_CRYPTO_MODE ) )
	{
		PrintError("crypto mode is not matched!! (%d-%d)\n", xRam2RamChannelID->cryptoMode, xCryptoMode);
		return DRV_ERR_INVALID_PARAMETER;
	}

	if( (pxSrc->length%8) || (pxSrc->length>pxDst->length) )
	{
		PrintError("invalid length %d\n", pxSrc->length);
		return DRV_ERR_INVALID_PARAMETER;
	}

	if((pxSrc->length % 16 )&&
	   (xCryptoMode ==	CSD_CRYPTO_MODE_AES_ECB || xCryptoMode ==  CSD_CRYPTO_MODE_AES_CBC))
	{
		PrintError("Input data length mismatched! \n");
		return DRV_ERR_INVALID_PARAMETER;
	}

	errCode= csdDmaBlkTransfer(pxSrc->pData,
							   pxDst->pData,
							   pxSrc->length,
							   xRam2RamChannelID->keyHandle,
							   xRam2RamChannelID->hDmaCsd);
	if (errCode != 0)
	{
		PrintError("csdDmaBlkTransfer() errCode: %x\n", errCode );
		return DRV_ERR;
	}

	return DRV_OK;
}
#endif

DRV_Error P_CSDSetOtpMSP(NEXUS_OtpCmdMsp eOtpCmdMsp, HUINT8 ucDataBitLen, HUINT32 ulDataBitMask, HUINT32 ulMspData)
{
	NEXUS_ReadMspParms readMspParams;
	NEXUS_ProgramMspIO progMspIO;
	NEXUS_ReadMspIO readMspIO;
	NEXUS_Error rc;

	readMspParams.readMspEnum = eOtpCmdMsp;
	if((rc = NEXUS_Security_ReadMSP (&readMspParams, &readMspIO)) != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_Security_ReadMSP failed, error = %d\n", rc);
		return DRV_ERR;
	}

	if( (readMspIO.mspDataBuf[0]==0x0) &&
		(readMspIO.mspDataBuf[1]==0x0) &&
		(readMspIO.mspDataBuf[2]==0x0) &&
		(readMspIO.mspDataBuf[3]==0x0) )
	{
		progMspIO.progMode 	  = NEXUS_OTP_DATASECTIONPROG_MODE;
		progMspIO.progMspEnum	  = eOtpCmdMsp;
		progMspIO.dataBitLen	  = ucDataBitLen;
		progMspIO.dataBitMask[0] = (ulDataBitMask >> 24) & 0xFF;
		progMspIO.dataBitMask[1] = (ulDataBitMask >> 16) & 0xFF;
		progMspIO.dataBitMask[2] = (ulDataBitMask >> 8) & 0xFF;
		progMspIO.dataBitMask[3] = ulDataBitMask & 0xFF;
		progMspIO.mspData[0]	  = (ulMspData >> 24) & 0xFF;
		progMspIO.mspData[1]	  = (ulMspData >> 16) & 0xFF;
		progMspIO.mspData[2]	  = (ulMspData >> 8) & 0xFF;
		progMspIO.mspData[3]	  = ulMspData & 0xFF;

		/* Issue command to program the OTP. */
		if((rc = NEXUS_Security_ProgramMSP (&progMspIO)) != NEXUS_SUCCESS)
		{
			PrintError("NEXUS_Security_ProgramMSP failed, error = %d\n", rc);
			return DRV_ERR;
		}

		readMspParams.readMspEnum = eOtpCmdMsp;
		if((rc = NEXUS_Security_ReadMSP (&readMspParams, &readMspIO)) != NEXUS_SUCCESS)
		{
			PrintError("NEXUS_Security_ReadMSP failed, error = %d\n", rc);
			return DRV_ERR;
		}

		if( (readMspIO.mspDataBuf[0] != ((ulMspData >> 24) & 0xFF)) ||
			(readMspIO.mspDataBuf[1] != ((ulMspData >> 16) & 0xFF)) ||
			(readMspIO.mspDataBuf[2] != ((ulMspData >> 8) & 0xFF)) ||
			(readMspIO.mspDataBuf[3] != (ulMspData & 0xFF)) )
		{
			PrintError("Programmed value does not match target value. Read again after RESET!\n");
		}
	}

	return DRV_OK;

}

DRV_Error P_CSDGetOtpMSP(NEXUS_OtpCmdMsp eOtpCmdMsp, HUINT32 *ulMspData)
{
	NEXUS_ReadMspParms readMspParams;
	NEXUS_ReadMspIO readMspIO;
	NEXUS_Error rc;

	if(ulMspData == NULL)
	{
		return DRV_ERR;
	}

	readMspParams.readMspEnum = eOtpCmdMsp;
	if((rc = NEXUS_Security_ReadMSP (&readMspParams, &readMspIO)) != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_Security_ReadMSP failed, error = %d\n", rc);
		return DRV_ERR;
	}

	*ulMspData = ((readMspIO.mspDataBuf[0] << 24) | (readMspIO.mspDataBuf[1] << 16) | (readMspIO.mspDataBuf[2] << 8) | readMspIO.mspDataBuf[3]);

	return DRV_OK;
}

static DRV_Error P_CSDGetBlockNumber(int *pBlockNo)
{
#if defined(CONFIG_TEMP_DI10)
	NEXUS_Error errCode = NEXUS_SUCCESS;
	NEXUS_ReadMspParms	ReadMspParms;
	NEXUS_ReadMspIO		ReadMspIO;

	VK_memset(&ReadMspParms, 0x00, sizeof(ReadMspParms));
	VK_memset(&ReadMspIO, 0x00, sizeof(ReadMspIO));

	ReadMspParms.readMspEnum = NEXUS_OtpCmdMsp_eFlashRegion0_2;
	errCode = NEXUS_Security_ReadMSP(&ReadMspParms, &ReadMspIO);
	if (errCode != 0)
	{
		PrintError("NEXUS_Security_ReadMSP() errCode: %x\n", errCode );
		return DRV_ERR;
	}

	*pBlockNo = ReadMspIO.mspDataBuf[3];
#else
	BSTD_UNUSED(pBlockNo);
#endif

	return DRV_OK;
}

static DRV_Error P_CSDSetBlockNumber(int BlockNo)
{
#if defined(CONFIG_TEMP_DI10)
	NEXUS_Error errCode = NEXUS_SUCCESS;
	NEXUS_ProgramMspIO	ProgMspIO;

	VK_memset(&ProgMspIO, 0x00, sizeof(ProgMspIO));

	ProgMspIO.progMode			= NEXUS_OtpMspCmdProgMode_eEnum;
	ProgMspIO.progMspEnum		= NEXUS_OtpCmdMsp_eFlashRegion0_2;
	ProgMspIO.dataBitLen		= 3;
	ProgMspIO.dataBitMask[3]	= 0x7;
	ProgMspIO.mspData[3]		= BlockNo;

	errCode=  NEXUS_Security_ProgramMSP(&ProgMspIO);
	if (errCode != 0)
	{
		PrintError("NEXUS_Security_ProgramMSP() errCode: %x\n", errCode );
		return DRV_ERR;
	}
#else
	BSTD_UNUSED(BlockNo);
#endif

	return DRV_OK;
}

static DRV_Error P_CSDGetKeyIndex(int *pkeyindex)
{
	NEXUS_Error errCode = NEXUS_SUCCESS;
	NEXUS_ReadMspParms	ReadMspParms;
	NEXUS_ReadMspIO		ReadMspIO;

	VK_memset(&ReadMspParms, 0x00, sizeof(ReadMspParms));
	VK_memset(&ReadMspIO, 0x00, sizeof(ReadMspIO));

	ReadMspParms.readMspEnum = NEXUS_OtpCmdMsp_ePublicKey0Index;
	errCode = NEXUS_Security_ReadMSP(&ReadMspParms, &ReadMspIO);
	if (errCode != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_Security_ReadMSP() errCode: %x\n", errCode );

		return DRV_ERR;
	}

	*pkeyindex = ReadMspIO.mspDataBuf[3];

	return DRV_OK;
}

static DRV_Error P_CSDSetKeyIndex(int keyindex)
{
	NEXUS_ProgramMspIO 	progMspIO;
	NEXUS_Error errCode = NEXUS_SUCCESS;

	VK_memset(&progMspIO, 0x00, sizeof(progMspIO));

	progMspIO.progMode		 = NEXUS_OTP_DATASECTIONPROG_MODE;
	progMspIO.progMspEnum	 = NEXUS_OtpCmdMsp_ePublicKey0Index;
	progMspIO.dataBitLen		= 3;
	progMspIO.dataBitMask[3]	= 0x7;
	progMspIO.mspData[3]     = keyindex; 	/* Set the key index accordingly */

	/* Issue command to program the OTP. */
	errCode = NEXUS_Security_ProgramMSP(&progMspIO);
	if (errCode != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_Security_ProgramMSP() errCode: %x\n", errCode );

		return DRV_ERR;
	}

	return DRV_OK;
}

static DRV_Error P_CSDGetBootMem(int *pflag)
{
#if defined(CONFIG_CAS_NA)
	TCsdStatus CsdStatus;
#if defined(CONFIG_CAS_NA_NOCS_3X)
	TCsdActivationFlag ScsActivationFlagState;
#else
	TCsdScsActivationFlag ScsActivationFlagState;
#endif
	CsdStatus=csdGetScsActivationFlag(&ScsActivationFlagState);
	if(CsdStatus != CSD_NO_ERROR)
	{
		PrintError("csdGetScsActivationFlag fail !!\n");
		return DRV_ERR;
	}
#if defined(CONFIG_CAS_NA_NOCS_3X)
	if(ScsActivationFlagState == CSD_ACTIVATION_FLAG_SET)
#else
	if(ScsActivationFlagState == CSD_SCS_ACTIVATION_FLAG_SET)
#endif
	{
		PrintError("SCS Activated !!\n");
		*pflag = 1;
	}
	else
	{
		PrintError("SCS Not Activated !!\n");
		*pflag = 0;
	}
#else
	NEXUS_Error errCode = NEXUS_SUCCESS;
	NEXUS_ReadMspParms	ReadMspParms;
	NEXUS_ReadMspIO		ReadMspIO;

	VK_memset(&ReadMspParms, 0x00, sizeof(ReadMspParms));
	VK_memset(&ReadMspIO, 0x00, sizeof(ReadMspIO));

	ReadMspParms.readMspEnum = NEXUS_OtpCmdMsp_eSecureBootEnable;
	errCode = NEXUS_Security_ReadMSP(&ReadMspParms, &ReadMspIO);
	if (errCode != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_Security_ReadMSP() errCode: %x\n", errCode );

		return DRV_ERR;
	}

	*pflag = ReadMspIO.mspDataBuf[3];
#endif
	return DRV_OK;
}

static DRV_Error P_CSDSetBootMem(void)
{
#if defined(CONFIG_CAS_NA)
	TCsdStatus CsdStatus;
	CsdStatus=csdEnableScs();
	if(CsdStatus != CSD_NO_ERROR)
	{
		PrintError("csdEnableScs fail !!\n");
		return DRV_ERR;
	}
#else
	NEXUS_Error errCode = NEXUS_SUCCESS;
	NEXUS_ProgramMspIO	ProgMspIO;

	VK_memset(&ProgMspIO, 0x00, sizeof(ProgMspIO));

	ProgMspIO.progMode       = NEXUS_OtpMspCmdProgMode_eEnum;
	ProgMspIO.progMspEnum    = NEXUS_OtpCmdMsp_eSecureBootEnable;
	ProgMspIO.dataBitLen		= 1;
	ProgMspIO.dataBitMask[0] = 0x0;
	ProgMspIO.dataBitMask[1] = 0x0;
	ProgMspIO.dataBitMask[2] = 0x0;
	ProgMspIO.dataBitMask[3] = 0x01;
	ProgMspIO.mspData[0]     = 0x0;
	ProgMspIO.mspData[1]     = 0x0;
	ProgMspIO.mspData[2]     = 0x0;
	ProgMspIO.mspData[3]		= 0x1;

	/* Issue command to program the OTP. */
	errCode=  NEXUS_Security_ProgramMSP(&ProgMspIO);
	if (errCode != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_Security_ProgramMSP() errCode: %x\n", errCode );

		return DRV_ERR;
	}
#endif
	return DRV_OK;
}

static DRV_Error P_CSDGetJTAGStatus(int *pflag)
{
#if defined(CONFIG_CAS_NA) && defined(CONFIG_CAS_NA_NOCS_3X)
	TCsdStatus CsdStatus;
	TCsdDebugInterfaceAccessMode xDebugInterfaceProtectionLevel; /* Default : Password Lock */

	CsdStatus=csdGetDebugInterfaceProtectionLevel(&xDebugInterfaceProtectionLevel);
	if(CsdStatus != CSD_NO_ERROR)
	{
		PrintError("csdSelectDebugInterfaceProtectionLevel fail !!\n");
		return DRV_ERR;
	}

	if((xDebugInterfaceProtectionLevel == CSD_DEBUG_INTERFACE_ACCESS_MODE_PASSWORD_PROTECTED) || (xDebugInterfaceProtectionLevel == CSD_DEBUG_INTERFACE_ACCESS_MODE_PASSWORD_PROTECTED))
	{
		*pflag = TRUE;
	}
	else
	{
		*pflag = FALSE;
		if(xDebugInterfaceProtectionLevel != CSD_DEBUG_INTERFACE_ACCESS_MODE_OPEN)
		{
			PrintError("Unknown Status [%d], fail !!\n", xDebugInterfaceProtectionLevel);
			return DRV_ERR;
		}
	}
#else
	NEXUS_Error errCode = NEXUS_SUCCESS;
	NEXUS_ReadMspParms	ReadMspParms;
	NEXUS_ReadMspIO		ReadMspIO;

	VK_memset(&ReadMspParms, 0x00, sizeof(ReadMspParms));
	VK_memset(&ReadMspIO, 0x00, sizeof(ReadMspIO));

	ReadMspParms.readMspEnum = NEXUS_OtpCmdMsp_eCrLockEnable;
	errCode = NEXUS_Security_ReadMSP(&ReadMspParms, &ReadMspIO);
	if (errCode != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_Security_ReadMSP() errCode: %x\n", errCode );

		return DRV_ERR;
	}

	*pflag = ReadMspIO.mspDataBuf[3];
#endif
	return DRV_OK;
}

static DRV_Error P_CSDSetJTAGDisable(void)
{
#if defined(CONFIG_CAS_NA) && defined(CONFIG_CAS_NA_NOCS_3X)
	TCsdStatus CsdStatus;
	TCsdDebugInterfaceAccessMode xDebugInterfaceProtectionLevel = CSD_DEBUG_INTERFACE_ACCESS_MODE_PASSWORD_PROTECTED; /* Default : Password Lock */

	CsdStatus=csdSelectDebugInterfaceProtectionLevel(xDebugInterfaceProtectionLevel);
	if(CsdStatus != CSD_NO_ERROR)
	{
		PrintError("csdSelectDebugInterfaceProtectionLevel fail !!\n");
		return DRV_ERR;
	}
#else
	NEXUS_Error errCode = NEXUS_SUCCESS;
	NEXUS_ProgramMspIO	ProgMspIO;

	VK_memset(&ProgMspIO, 0x00, sizeof(ProgMspIO));

	/* Blow settings are for Password Lock. */
	ProgMspIO.progMode		 = NEXUS_OtpMspCmdProgMode_eEnum;
	ProgMspIO.progMspEnum    = NEXUS_OtpCmdMsp_eCrLockEnable;
	ProgMspIO.dataBitLen	 = 1;
	ProgMspIO.dataBitMask[0] = 0x0;
	ProgMspIO.dataBitMask[1] = 0x0;
	ProgMspIO.dataBitMask[2] = 0x0;
	ProgMspIO.dataBitMask[3] = 0x01;
	ProgMspIO.mspData[0]     = 0x0;
	ProgMspIO.mspData[1]     = 0x0;
	ProgMspIO.mspData[2]     = 0x0;
	ProgMspIO.mspData[3]	 = 0x1;

	/* Issue command to program the OTP. */
	errCode=  NEXUS_Security_ProgramMSP(&ProgMspIO);
	if (errCode != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_Security_ProgramMSP() errCode: %x\n", errCode );

		return DRV_ERR;
	}
#endif
	return DRV_OK;
}

static DRV_Error P_CSDGetCWEStatus(int *pflag)
{
	NEXUS_Error errCode = NEXUS_SUCCESS;
	NEXUS_ReadMspParms	ReadMspParms;
	NEXUS_ReadMspIO		ReadMspIO;

	VK_memset(&ReadMspParms, 0x00, sizeof(ReadMspParms));
	VK_memset(&ReadMspIO, 0x00, sizeof(ReadMspIO));

	ReadMspParms.readMspEnum = NEXUS_OtpCmdMsp_eCaSoftwareKeyDisable;
	errCode = NEXUS_Security_ReadMSP(&ReadMspParms, &ReadMspIO);
	if (errCode != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_Security_ReadMSP() errCode: %x\n", errCode );

		return DRV_ERR;
	}

	*pflag = ReadMspIO.mspDataBuf[3];

	return DRV_OK;
}

static DRV_Error P_CSDSetCWE(void)
{
	NEXUS_Error errCode = NEXUS_SUCCESS;
	NEXUS_ProgramMspIO	ProgMspIO;

	VK_memset(&ProgMspIO, 0x00, sizeof(ProgMspIO));

	ProgMspIO.progMode       = NEXUS_OtpMspCmdProgMode_eEnum;
	ProgMspIO.progMspEnum    = NEXUS_OtpCmdMsp_eCaSoftwareKeyDisable;
	ProgMspIO.dataBitLen     = 1;
	ProgMspIO.dataBitMask[0] = 0x0;
	ProgMspIO.dataBitMask[1] = 0x0;
	ProgMspIO.dataBitMask[2] = 0x0;
	ProgMspIO.dataBitMask[3] = 0x01;
	ProgMspIO.mspData[0]     = 0x0;
	ProgMspIO.mspData[1]     = 0x0;
	ProgMspIO.mspData[2]     = 0x0;
	ProgMspIO.mspData[3]     = 0x1;

	/* Issue command to program the OTP. */
	errCode=  NEXUS_Security_ProgramMSP(&ProgMspIO);
	if (errCode != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_Security_ProgramMSP() errCode: %x\n", errCode );

		return DRV_ERR;
	}

	return DRV_OK;
}


static DRV_Error P_CSDGetSTBOwnerID(HUINT16 *pusSTBOwnerID)
{
	NEXUS_Error errCode = NEXUS_SUCCESS;
	NEXUS_ReadMspParms	ReadMspParms;
	NEXUS_ReadMspIO		ReadMspIO;
	HUINT32				value = 0;

	VK_memset(&ReadMspParms, 0x00, sizeof(ReadMspParms));
	VK_memset(&ReadMspIO, 0x00, sizeof(ReadMspIO));

	ReadMspParms.readMspEnum = NEXUS_OtpCmdMsp_eAskmStbOwnerId;
	errCode = NEXUS_Security_ReadMSP(&ReadMspParms, &ReadMspIO);
	if (errCode != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_Security_ReadMSP() errCode: %x\n", errCode );

		return DRV_ERR;
	}

	value = (ReadMspIO.mspDataBuf[0] << 24) + (ReadMspIO.mspDataBuf[1] << 16)
			+ (ReadMspIO.mspDataBuf[2] << 8) + ReadMspIO.mspDataBuf[3];

	*pusSTBOwnerID = (HUINT16)value;

	return DRV_OK;
}

static DRV_Error P_CSDSetSTBOwnerID(HUINT16 usSTBOwnerID)
{
	NEXUS_Error errCode = NEXUS_SUCCESS;
	NEXUS_ProgramMspIO	ProgMspIO;

	VK_memset(&ProgMspIO, 0x00, sizeof(ProgMspIO));

	ProgMspIO.progMode       = NEXUS_OtpMspCmdProgMode_eEnum;
	ProgMspIO.progMspEnum    = NEXUS_OtpCmdMsp_eAskmStbOwnerId;
	ProgMspIO.dataBitLen     = 16;
	ProgMspIO.dataBitMask[0] = 0x0;
	ProgMspIO.dataBitMask[1] = 0x0;
	ProgMspIO.dataBitMask[2] = 0xFF;
	ProgMspIO.dataBitMask[3] = 0xFF;
	ProgMspIO.mspData[0]     = 0x0;
	ProgMspIO.mspData[1]     = 0x0;
	ProgMspIO.mspData[2]     = (HUINT8)(usSTBOwnerID >> 8);
	ProgMspIO.mspData[3]     = (HUINT8)usSTBOwnerID;

	/* Issue command to program the OTP. */
	errCode=  NEXUS_Security_ProgramMSP(&ProgMspIO);
	if (errCode != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_Security_ProgramMSP() errCode: %x\n", errCode );

		return DRV_ERR;
	}

	return DRV_OK;
}

static DRV_Error P_CSDSetEncryptFPK(void)
{
	DRV_Error 	eDrvErr=DRV_OK;

#if defined(CONFIG_CAS_NA_NOCS_SEC)
#if defined(CONFIG_NOR_FLASH)
	DI_ERR_CODE eDiErr = DI_ERR_OK;
#endif
	HUINT8		*pFPKBuf = NULL;
	HUINT8		*pEncryptedFPKBuf = NULL;
	int		bootFlashHandle;

	PrintEnter();

	NEXUS_Memory_Allocate(NAGRA_FPK_BLOCK_SIZE, NULL, (void *)&pFPKBuf);
	if(pFPKBuf == NULL)
	{
		PrintError("[%s] FPKBuf is NULL !! \n",__func__);
		return DRV_ERR;
	}
	BKNI_Memset(pFPKBuf, 0x00, NAGRA_FPK_BLOCK_SIZE);

	NEXUS_Memory_Allocate(NAGRA_FPK_BLOCK_SIZE, NULL, (void *)&pEncryptedFPKBuf);
	if(pEncryptedFPKBuf == NULL)
	{
		PrintError("[%s] EncryptedFPKBuf is NULL !! \n",__func__);
		NEXUS_Memory_Free(pFPKBuf);
		return DRV_ERR;
	}
	BKNI_Memset(pEncryptedFPKBuf, 0x00, NAGRA_FPK_BLOCK_SIZE);

#if defined(CONFIG_NOR_FLASH)
	eDiErr = DI_FLASH_Read(NAGRA_FPK_LOGICAL_OFFSET, pFPKBuf, NAGRA_FPK_BLOCK_SIZE);
	if(eDiErr != DI_ERR_OK)
	{
		PrintError("[%s] DI_FLASH_Read() error !\n",__func__);
	}
#elif defined (CONFIG_NAND_FLASH)
	eDrvErr = DRV_NANDFLASH_BOOT_GetHandle(&bootFlashHandle);
	if(eDrvErr != DRV_OK)
	{
		PrintError("[%s] DRV_NANDFLASH_BOOT_GetHandle error !!\n",__func__);
		goto err;
	}

	eDrvErr = DRV_NANDFLASH_BOOT_Read(bootFlashHandle,NAGRA_FPK_LOGICAL_OFFSET,pFPKBuf,NAGRA_FPK_BLOCK_SIZE);
	if(eDrvErr != DRV_OK)
	{
		PrintError("[%s] DRV_NANDFLASH_BOOT_Read() error !!\r",__func__);
		goto err;
	}
#else
	// TODO: EMMC only case
	PrintError("[%s] Add EMMC flash write on here !!\r",__func__);
#endif
#if DEBUG_DUMP_FPK
	{
		int i=0;
		PrintError("========== FPK 16 bytes ===========\n");
		for(i=0;i<NAGRA_FPK_BLOCK_SIZE;i++)
			VK_printf("0x%x ",pFPKBuf[i]);

		VK_printf("\n");
		PrintError("\n===================================\n");
	}
#endif
	{
		/* SWAP data */
		HINT32 i=0;
		HUINT32 *pBuf=(HUINT32 *)pFPKBuf;
		for(i=0;i<NAGRA_FPK_BLOCK_SIZE/4;i++)
			pBuf[i]=byteswap32(pBuf[i]);

	}
#if DEBUG_DUMP_FPK
	{
		int i=0;
		PrintError("======== FPK SWAP 16 bytes ==========\n");
		for(i=0;i<NAGRA_FPK_BLOCK_SIZE;i++)
			VK_printf("0x%x ",pFPKBuf[i]);

		VK_printf("\n");
		PrintError("\n===================================\n");
	}
#endif
	{
		TSecStatus SecStatus;
		SecStatus = secEncryptFlashProtKey(pFPKBuf,pEncryptedFPKBuf,NAGRA_FPK_BLOCK_SIZE);
		if(SecStatus != SEC_NO_ERROR)
		{
			PrintError("[%s] secEncryptFlashProtKey error ! \n",__func__);
			eDrvErr = DRV_ERR;
		}
		else
		{
#if DEBUG_DUMP_FPK
			int i=0;
			PrintError("========== Encrypted FPK 16 bytes ===========\n");
			for(i=0;i<NAGRA_FPK_BLOCK_SIZE;i++)
				VK_printf("0x%x ",pEncryptedFPKBuf[i]);

			VK_printf("\n");
			PrintError("\n===========================================\n");
#endif
			{
				/* SWAP data */
				HINT32 i=0;
				HUINT32 *pBuf=(HUINT32 *)pEncryptedFPKBuf;
				for(i=0;i<NAGRA_FPK_BLOCK_SIZE/4;i++)
					pBuf[i]=byteswap32(pBuf[i]);

			}
#if DEBUG_DUMP_FPK
			{
				int i=0;
				PrintError("======== Encrypted FPK SWAP 16 bytes =========\n");
				for(i=0;i<NAGRA_FPK_BLOCK_SIZE;i++)
					VK_printf("0x%x ",pEncryptedFPKBuf[i]);

				VK_printf("\n");
				PrintError("\n===========================================\n");
			}
#endif
#if defined(CONFIG_NOR_FLASH)
			eDiErr = (DI_ERR_CODE)DI_FLASH_WriteNotAligned(NAGRA_FPK_LOGICAL_OFFSET, pEncryptedFPKBuf, NAGRA_FPK_BLOCK_SIZE);
			if(eDiErr != DI_ERR_OK)
			{
				PrintError("[%s] DI_FLASH_Write() error !\n",__func__);
			}
#elif defined(CONFIG_NAND_FLASH)
			eDrvErr = DRV_NANDFLASH_BOOT_Write(bootFlashHandle,NAGRA_FPK_LOGICAL_OFFSET,pEncryptedFPKBuf,NAGRA_FPK_BLOCK_SIZE);
			if(eDrvErr != DRV_OK)
			{
				PrintError("[%s] DRV_NANDFLASH_BOOT_Write() error !\n",__func__);
			}

			eDrvErr = DRV_NANDFLASH_BOOT_Sync(bootFlashHandle);
			if(eDrvErr != DRV_OK)
			{
				PrintError("[%s] DRV_NANDFLASH_BOOT_Sync() error !\n",__func__);
			}
#else 
			// TODO : EMMC only case
			PrintError("[%s] Add EMMC flash write on here !!\r",__func__);
#endif
		}
err:
		NEXUS_Memory_Free(pFPKBuf);
		NEXUS_Memory_Free(pEncryptedFPKBuf);

	}

	PrintExit();
#endif
	return eDrvErr;
}

static DRV_Error P_CSDGetRamScrambling(HUINT8 *pucRamScr)
{
#if defined(CONFIG_CAS_NA) && defined(CONFIG_CAS_NA_NOCS_3X)
	TCsdActivationFlag xScramblingActivationFlagState = CSD_ACTIVATION_FLAG_NOT_SET;
	TCsdStatus CsdStatus;

	if(pucRamScr == NULL)
	{
		PrintError("[%s:%d] Invalid Parameter ERROR!!\r\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	*pucRamScr = 0;

	CsdStatus = csdGetScramblingActivationFlag(&xScramblingActivationFlagState);

	if(CsdStatus != CSD_NO_ERROR)
	{
		PrintError("Error in csdGetScramblingActivationFlag)\n");
		return DRV_ERR;
	}

	if(CSD_ACTIVATION_FLAG_SET == xScramblingActivationFlagState)
	{
		*pucRamScr = 0x01;
	}
	else
	{
		*pucRamScr = 0;
	}
#else
	NEXUS_ReadMspParms readMspParams;
	NEXUS_ReadMspIO readMspIO;
	NEXUS_Error errCode = NEXUS_SUCCESS;;

	PrintEnter();

	if(pucRamScr == NULL)
	{
		PrintError("[Invalid Parameter] pucRamScr is NULL\n");
		return DRV_ERR_INVALID_PARAMETER;
	}

	readMspParams.readMspEnum = NEXUS_OtpCmdMsp_eForceDramScrambler;
	if((errCode = NEXUS_Security_ReadMSP (&readMspParams, &readMspIO)) != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_Security_ReadMSP failed, error = %d\n", errCode);

		return DRV_ERR;
	}

	*pucRamScr = (HUINT8)readMspIO.mspDataBuf[3];

	PrintExit();
#endif
	return DRV_OK;
}


static DRV_Error P_CSDSetRamScrambling(void)
{
#if defined(CONFIG_CAS_NA) && defined(CONFIG_CAS_NA_NOCS_3X)
	TCsdStatus CsdStatus;

	PrintEnter();

	CsdStatus = csdEnableRamScrambling();
	if(CsdStatus != CSD_NO_ERROR)
	{
		PrintError("csdEnableRamScrambling error!\n");
		return DRV_ERR;
	}

	PrintExit();
#else
	NEXUS_ReadMspParms readMspParams;
	NEXUS_ProgramMspIO progMspIO;
	NEXUS_ReadMspIO readMspIO;
	NEXUS_Error errCode = NEXUS_SUCCESS;;

	PrintEnter();
	readMspParams.readMspEnum = NEXUS_OtpCmdMsp_eForceDramScrambler;
	if((errCode = NEXUS_Security_ReadMSP (&readMspParams, &readMspIO)) != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_Security_ReadMSP failed, error = %d\n", errCode);

		return DRV_ERR;
	}

	if(readMspIO.mspDataBuf[3]==0x0)
	{
		progMspIO.progMode       = NEXUS_OTP_DATASECTIONPROG_MODE;
		progMspIO.progMspEnum    = NEXUS_OtpCmdMsp_eForceDramScrambler;
		progMspIO.dataBitLen     = 1;
		progMspIO.dataBitMask[0] = 0x0;
		progMspIO.dataBitMask[1] = 0x0;
		progMspIO.dataBitMask[2] = 0x0;
		progMspIO.dataBitMask[3] = 0x1;
		progMspIO.mspData[0]     = 0x0;
		progMspIO.mspData[1]     = 0x0;
		progMspIO.mspData[2]     = 0x0;
		progMspIO.mspData[3]     = 0x1;

		/* Issue command to program the OTP. */
		if((errCode = NEXUS_Security_ProgramMSP (&progMspIO)) != NEXUS_SUCCESS)
		{
			PrintError("NEXUS_Security_ProgramMSP failed, error = %d\n", errCode);

			return errCode;
		}

		readMspParams.readMspEnum = NEXUS_OtpCmdMsp_eForceDramScrambler;
		if((errCode = NEXUS_Security_ReadMSP (&readMspParams, &readMspIO)) != NEXUS_SUCCESS)
		{
			PrintError("NEXUS_Security_ReadMSP failed, error = %d\n", errCode);

			return errCode;
		}

		if(readMspIO.mspDataBuf[3] != 0x1)
		{
			PrintError("[Warning]Programmed value does not match target value. Read again after RESET!\n");
		}
	}
	PrintExit();
#endif
	return DRV_OK;
}

static DRV_Error P_CSDGetStbCaSn(HUINT8 *pucStbCaSn)
{
#if defined(CONFIG_CAS_NA)
	TCsdStatus CsdStatus;
	TCsd4BytesVector StbCaSn;

	PrintEnter();

	if(pucStbCaSn == NULL)
	{
		PrintError("P_CSDGetStbCaSn error, Invalid parameter!\n");
		return DRV_ERR;
	}

	*pucStbCaSn = 0;

	CsdStatus=csdGetStbCaSn(StbCaSn);
	if(CsdStatus != CSD_NO_ERROR)
	{
		PrintError("csdGetStbCaSn error!\n");
		return DRV_ERR;
	}
	pucStbCaSn[0] = StbCaSn[0];
	pucStbCaSn[1] = StbCaSn[1];
	pucStbCaSn[2] = StbCaSn[2];
	pucStbCaSn[3] = StbCaSn[3];

	PrintDebug("STB CAS SN : 0x%02X\n", *pucStbCaSn);

	PrintExit();

#else
	BSTD_UNUSED(pucStbCaSn);
#endif
	return DRV_OK;
}

static DRV_Error P_CSDSetStbCaSn(HUINT8 *pucStbCaSn)
{
#if defined(CONFIG_CAS_NA)
	TCsdStatus CsdStatus;
	TCsd4BytesVector StbCaSn;

	PrintEnter();

	if(pucStbCaSn == NULL)
	{
		PrintError("[%s] pucStbCaSn is NULL!\n",__func__);
		return DRV_ERR;
	}

	StbCaSn[0] = pucStbCaSn[0];
	StbCaSn[1] = pucStbCaSn[1];
	StbCaSn[2] = pucStbCaSn[2];
	StbCaSn[3] = pucStbCaSn[3];

	CsdStatus = csdSetStbCaSn(StbCaSn);
	if(CsdStatus != CSD_NO_ERROR)
	{
		PrintError("csdSetStbCaSn error!!\n");
		return DRV_ERR;
	}

	PrintExit();
#else
	BSTD_UNUSED(pucStbCaSn);
#endif
	return DRV_OK;
}

static DRV_Error P_CSDSetMarketID(HUINT32 ulMarketID)
{
#if defined(CONFIG_CAS_NA)
	TCsdStatus CsdStatus;
	/* the MarketSegment Id is 4bytes from Nagra. */
#if (BCHP_CHIP == 7241) || (BCHP_CHIP == 7429)
	TCsd4BytesVector xMarketSegmentId = {0x01,0x00,0x00,0x04};
#elif (BCHP_CHIP == 7346)
	TCsd4BytesVector xMarketSegmentId = {0x00,0x00,0x00,0x04};
#elif (BCHP_CHIP == 7358)
	TCsd4BytesVector xMarketSegmentId = {0x00,0x00,0x00,0x04};
#elif (BCHP_CHIP == 7439)
	TCsd4BytesVector xMarketSegmentId = {0x00,0x00,0x00,0x04};
#elif (BCHP_CHIP == 7584)
	TCsd4BytesVector xMarketSegmentId = {0x00,0x00,0x00,0x41};
#else
	TCsd4BytesVector xMarketSegmentId = {0, };
	if (ulMarketID == P_CSD_MARKET_ID_DEFAULT)
	{
		PrintError("[%s,%d]Not support!! write marketid using FDI_SYSTEM_SetMarketID()\n\r", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	xMarketSegmentId[0] = (ulMarketID >> 24) & 0xFF;
	xMarketSegmentId[1] = (ulMarketID >> 16) & 0xFF;
	xMarketSegmentId[2] = (ulMarketID >> 8) & 0xFF;
	xMarketSegmentId[3] = ulMarketID & 0xFF;
#endif
	CsdStatus = csdSetMarketSegmentId(xMarketSegmentId);
	if(CsdStatus != CSD_NO_ERROR)
	{
		PrintError("[%s,%d]Error in csdSetMarketSegmentId()\n\r", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}
#else /* CONFIG_CAS_NA */
	DRV_Error errCode = DRV_OK;
	HUINT32 ulDataBitMask = 0xFFFFFFFF;
	HUINT8 ucDataBitLen = 32;
#if defined(CONFIG_CAS_CX)
#if (BCHP_CHIP == 7552) || (BCHP_CHIP == 7583) || (BCHP_CHIP == 7250)
	ulMarketID = 0x2F210000;
#else
	if (ulMarketID == P_CSD_MARKET_ID_DEFAULT)
	{
		PrintError("[%s,%d]Not support!! write marketid using FDI_SYSTEM_SetMarketID()\n\r", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}
#endif

#elif defined(CONFIG_CAS_VE)
#if (BCHP_CHIP == 7241) || (BCHP_CHIP == 7429)
	ulMarketID = 0x35363031;
#else
	if (ulMarketID == P_CSD_MARKET_ID_DEFAULT)
	{
		PrintError("[%s,%d]Not support!! write marketid using FDI_SYSTEM_SetMarketID()\n\r", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}
#endif
#endif /* End of CONFIG_CAS_CX, CONFIG_CAS_VE */
	errCode = P_CSDSetOtpMSP(NEXUS_OtpCmdMsp_eMarketId, ucDataBitLen, ulDataBitMask, ulMarketID);
	if(errCode != DRV_OK)
	{
		PrintError("[%s,%d]Error in P_CSDSetOtpMSP()\n\r", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}
#endif /* End of CONFIG_CAS_NA */
	return DRV_OK;
}

static DRV_Error P_CSDSetBootMode(DRV_CSD_BootMode eCsdBootMode)
{
#if defined(CONFIG_CAS_NA) && defined(CONFIG_CAS_NA_NOCS_1X)
	TCsdStatus errCode;
	TCsdBootMode bootMode;

	PrintEnter();

	if(eCsdBootMode >= DRV_CSD_BOOT_MODE_MAX)
	{
		PrintError("[%s] eCsdBootMode is invalid paramters!\n", __func__);
		return DRV_ERR;
	}

	bootMode = eCsdBootMode;
	errCode = csdSetBootMode(bootMode);
	if(errCode != CSD_NO_ERROR)
	{
		PrintError("[%s] eCsdBootMode is not set! error!\n", __func__);
		return DRV_ERR;
	}

	PrintExit();
#else
	BSTD_UNUSED(eCsdBootMode);
#endif
	return DRV_OK;
}


static DRV_Error P_CSDGetBootMode(DRV_CSD_BootMode *pCsdBootMode)
{
#if defined(CONFIG_CAS_NA) && defined(CONFIG_CAS_NA_NOCS_1X)
	TCsdStatus errCode;
	TCsdBootMode bootMode;
	PrintEnter();

	errCode = csdGetBootMode(&bootMode);
	if(errCode != CSD_NO_ERROR)
	{
		PrintError("[%s] eCsdBootMode is not get! error!\n", __func__);
		return DRV_ERR;
	}

	*pCsdBootMode = bootMode;
	PrintExit();
#else
	BSTD_UNUSED(pCsdBootMode);
#endif
	return DRV_OK;
}


DRV_Error P_CSDGetEncryptionData(
	unsigned char *pInputData,
	unsigned char *pOutputData,
	unsigned int uiLength,
	unsigned int isEnryption)
{
#if defined(CONFIG_CAS_NA)
	TCsdR2RAlgorithm		Algorithm = CSD_R2R_ALGORITHM_TDES_K1K2K1;
	TCsdR2RCryptoOperationMode	Mode = CSD_R2R_CRYPTO_OPERATION_MODE_ECB;
	TCsdSize			CipheredContentKeySize;
	TCsdR2RKeyPathHandle		*pR2RKeyPathHandle;
	TCsdInitParameters		csdInit;
	TCsdStatus			CsdStatus;

	TCsdUnsignedInt8		*pcrypt_input=NULL;
	TCsdUnsignedInt8		*pcrypt_output=NULL;

	NEXUS_SecurityKeySlotSettings keySlotSettings;

	DRV_Error	errCode = DRV_OK;
	unsigned int 			remain_length = 0;
	unsigned int 			written_length = 0;

#if (BCHP_CHIP == 7429)
	const TCsdR2RCipheredProtectingKeys 	R2RCipheredProtectingKeys =
	{{0xA9, 0x32, 0x30, 0x31, 0x31, 0x4E, 0x61, 0x67, 0x72, 0x61, 0x76, 0x69, 0x73, 0x69, 0x6F, 0x6E},
	{0xA9, 0x32, 0x30, 0x31, 0x31, 0x4E, 0x61, 0x67, 0x72, 0x61, 0x76, 0x69, 0x73, 0x69, 0x6F, 0x6E}};

	TCsdUnsignedInt8 CipheredContentKey[] =
	{0xA9, 0x32, 0x30, 0x31, 0x31, 0x4E, 0x61, 0x67, 0x72, 0x61, 0x76, 0x69, 0x73, 0x69, 0x6F, 0x6E};

#elif (BCHP_CHIP == 7346)
	TCsdR2RCipheredProtectingKeys R2RCipheredProtectingKeys =
		{{0xdf, 0x72, 0xe9, 0x15, 0x9a, 0x30, 0xc6, 0x13, 0xec, 0x5b, 0x00, 0x42, 0xb8, 0xed, 0x5e, 0x0a}};

	TCsdUnsignedInt8 CipheredContentKey[16] =
		{0xdf, 0x72, 0xe9, 0x15, 0x9a, 0x30, 0xc6, 0x13, 0xec, 0x5b, 0x00, 0x42, 0xb8, 0xed, 0x5e, 0x0a};
#elif (BCHP_CHIP == 7358)
	TCsdR2RCipheredProtectingKeys R2RCipheredProtectingKeys =
		{{0xdf, 0x72, 0xe9, 0x15, 0x9a, 0x30, 0xc6, 0x13, 0xec, 0x5b, 0x00, 0x42, 0xb8, 0xed, 0x5e, 0x0a}};

	TCsdUnsignedInt8 CipheredContentKey[16] =
		{0xdf, 0x72, 0xe9, 0x15, 0x9a, 0x30, 0xc6, 0x13, 0xec, 0x5b, 0x00, 0x42, 0xb8, 0xed, 0x5e, 0x0a};
#elif (BCHP_CHIP == 7439)
	TCsdR2RCipheredProtectingKeys R2RCipheredProtectingKeys =
	{{0xA9, 0x32, 0x30, 0x31, 0x31, 0x4E, 0x61, 0x67, 0x72, 0x61, 0x76, 0x69, 0x73, 0x69, 0x6F, 0x6E},
	{0xA9, 0x32, 0x30, 0x31, 0x31, 0x4E, 0x61, 0x67, 0x72, 0x61, 0x76, 0x69, 0x73, 0x69, 0x6F, 0x6E}};

	TCsdUnsignedInt8 CipheredContentKey[16] =
	{0xA9, 0x32, 0x30, 0x31, 0x31, 0x4E, 0x61, 0x67, 0x72, 0x61, 0x76, 0x69, 0x73, 0x69, 0x6F, 0x6E};
#elif (BCHP_CHIP == 7584)
	TCsdR2RCipheredProtectingKeys R2RCipheredProtectingKeys =
		{{0xdf, 0x72, 0xe9, 0x15, 0x9a, 0x30, 0xc6, 0x13, 0xec, 0x5b, 0x00, 0x42, 0xb8, 0xed, 0x5e, 0x0a}};

	TCsdUnsignedInt8 CipheredContentKey[16] =
		{0xdf, 0x72, 0xe9, 0x15, 0x9a, 0x30, 0xc6, 0x13, 0xec, 0x5b, 0x00, 0x42, 0xb8, 0xed, 0x5e, 0x0a};
#elif (BCHP_CHIP == 7552)
	TCsdR2RCipheredProtectingKeys R2RCipheredProtectingKeys =
		{{0xdf, 0x72, 0xe9, 0x15, 0x9a, 0x30, 0xc6, 0x13, 0xec, 0x5b, 0x00, 0x42, 0xb8, 0xed, 0x5e, 0x0a}};

	TCsdUnsignedInt8 CipheredContentKey[16] =
		{0xdf, 0x72, 0xe9, 0x15, 0x9a, 0x30, 0xc6, 0x13, 0xec, 0x5b, 0x00, 0x42, 0xb8, 0xed, 0x5e, 0x0a};
#elif (BCHP_CHIP == 7362)
	TCsdR2RCipheredProtectingKeys R2RCipheredProtectingKeys =
		{{0xdf, 0x72, 0xe9, 0x15, 0x9a, 0x30, 0xc6, 0x13, 0xec, 0x5b, 0x00, 0x42, 0xb8, 0xed, 0x5e, 0x0a}};

	TCsdUnsignedInt8 CipheredContentKey[16] =
		{0xdf, 0x72, 0xe9, 0x15, 0x9a, 0x30, 0xc6, 0x13, 0xec, 0x5b, 0x00, 0x42, 0xb8, 0xed, 0x5e, 0x0a};

#else
#error "CHIP not supported"
#endif
	PrintEnter();

	if(pInputData == NULL || pOutputData == NULL || uiLength == 0)
	{
		PrintError("Buffer is NULL or Length is Zero!\n");
		return DRV_ERR;
	}

	NEXUS_Memory_Allocate(sizeof(TCsdR2RKeyPathHandle), NULL, (void *)&pR2RKeyPathHandle);
	BKNI_Memset(pR2RKeyPathHandle, 0x00, sizeof(TCsdR2RKeyPathHandle));

#if (NEXUS_VERSION >= 1501) || ((NEXUS_VERSION == 1401) && defined(CONFIG_CAS_NA_NOCS_3X))
	/* Get an R2R key path handle */
	CsdStatus = csdBrcmOpenR2RKeyPath(&pR2RKeyPathHandle);
	if (CSD_NO_ERROR != CsdStatus)
	{
		PrintError("%s: csdBrcmOpenR2RKeyPath failed\n", __FUNCTION__);
	}
#else
	pR2RKeyPathHandle->initHandle = &csdInit;

    NEXUS_Security_GetDefaultKeySlotSettings(&keySlotSettings);
	keySlotSettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;
#if defined(CONFIG_CAS_NA_NOCS_3X)
	pR2RKeyPathHandle->keyHandle = NEXUS_Security_AllocateKeySlot(&keySlotSettings);
	if(pR2RKeyPathHandle->keyHandle == NULL)
	{
		PrintError("Error:Allocate Key Slot is NULL !!\n");
		NEXUS_Memory_Free(pR2RKeyPathHandle);
		return DRV_ERR;
	}
#endif
#endif

	NEXUS_Memory_Allocate(16, NULL, (void *)&pcrypt_input);
	NEXUS_Memory_Allocate(16, NULL, (void *)&pcrypt_output);

	BKNI_Memset(pcrypt_input,0x00,16);
	BKNI_Memset(pcrypt_output,0x00,16);

	CipheredContentKeySize = sizeof(CipheredContentKey);

	remain_length = uiLength;
	written_length = 0;
	while(remain_length > 0 )
	{
   		if ( remain_length < 16 )
		{
			BKNI_Memset(pcrypt_input, 0x00, 16);
			BKNI_Memcpy(pcrypt_input, &pInputData[written_length], remain_length);

			if(isEnryption == 1)
			{
				CsdStatus=csdEncryptDataWithSecretContentKey(Algorithm,
								Mode,
								R2RCipheredProtectingKeys,
								CipheredContentKey,
								CipheredContentKeySize,
								NULL,
								0,
								true,
								pR2RKeyPathHandle,
								pcrypt_input,
								pcrypt_output,
								16);
				if(CsdStatus != CSD_NO_ERROR)
				{
					PrintError("Error:EncryptDataWithSecretContentKey error!\n");
					errCode = DRV_ERR;
					goto done;
				}

			}
			else
			{
				CsdStatus=csdDecryptDataWithSecretContentKey(Algorithm,
								Mode,
								R2RCipheredProtectingKeys,
								CipheredContentKey,
								CipheredContentKeySize,
								NULL,
								0,
								true,
								pR2RKeyPathHandle,
								pcrypt_input,
								pcrypt_output,
								16);
				if(CsdStatus != CSD_NO_ERROR)
				{
					PrintError("Error:DecryptDataWithSecretContentKey error!\n");
					errCode = DRV_ERR;
					goto done;
				}

			}

			BKNI_Memcpy(&pOutputData[written_length], pcrypt_output, remain_length);
			remain_length = 0;
			written_length = written_length+remain_length;
		}
		else
		{
		    BKNI_Memcpy(pcrypt_input, &pInputData[written_length], 16);

			if(isEnryption == 1)
			{
				CsdStatus=csdEncryptDataWithSecretContentKey(Algorithm,
								Mode,
								R2RCipheredProtectingKeys,
								CipheredContentKey,
								CipheredContentKeySize,
								NULL,
								0,
								true,
								pR2RKeyPathHandle,
								pcrypt_input,
								pcrypt_output,
								16);
				if(CsdStatus != CSD_NO_ERROR)
				{
					PrintError("Error:EncryptDataWithSecretContentKey error!\n");
					errCode = DRV_ERR;
					goto done;
				}

			}
			else
			{
				CsdStatus=csdDecryptDataWithSecretContentKey(Algorithm,
								Mode,
								R2RCipheredProtectingKeys,
								CipheredContentKey,
								CipheredContentKeySize,
								NULL,
								0,
								true,
								pR2RKeyPathHandle,
								pcrypt_input,
								pcrypt_output,
								16);
				if(CsdStatus != CSD_NO_ERROR)
				{
					PrintError("Error:DecryptDataWithSecretContentKey error!\n");
					errCode = DRV_ERR;
					goto done;
				}

			}

			VK_MEM_Memcpy(&pOutputData[written_length], pcrypt_output, 16);
			remain_length = remain_length-16;
			written_length = written_length + 16;
		}
	}

done:
#if defined(CONFIG_CAS_NA_NOCS_3X)
	#if (NEXUS_VERSION >= 1501) || (NEXUS_VERSION == 1401)
	if (pR2RKeyPathHandle)
		csdBrcmCloseR2RKeyPath(pR2RKeyPathHandle);
	#else
	if(pR2RKeyPathHandle->keyHandle != NULL)
	{
	   	NEXUS_Security_FreeKeySlot(pR2RKeyPathHandle->keyHandle);
   	}
	NEXUS_Memory_Free(pR2RKeyPathHandle);
	#endif
#else
	NEXUS_Memory_Free(pR2RKeyPathHandle);
#endif

	NEXUS_Memory_Free(pcrypt_input);
	NEXUS_Memory_Free(pcrypt_output);

	PrintExit();

	return errCode;

#else /* CONFIG_CAS_NA */
	CSDKey 				pxProtectingKeyTable[2] ;
#if (BCHP_CHIP==7403)
	unsigned char 	CKeyData1[] = {0X2D, 0xAF, 0x41, 0xAC, 0x1D, 0x3C, 0x68, 0xA4,
										   0x91, 0xCF, 0x5F, 0x30, 0x4E, 0x94, 0x0C, 0xC0
										  };
	unsigned char 	CKeyData2[] = {0X2D, 0xAF, 0x41, 0xAC, 0x1D, 0x3C, 0x68, 0xA4,
										   0x91, 0xCF, 0x5F, 0x30, 0x4E, 0x94, 0x0C, 0xC0
										  };
#elif  (BCHP_CHIP==7405)
	unsigned char 	CKeyData1[] = {0x2d, 0xaf, 0x41, 0xac, 0x1d, 0x3c, 0x68, 0xa4,
										   0x0e, 0x8c, 0xc8, 0xdd, 0x78, 0xcd, 0x9c, 0x23
										  };
	unsigned char 	CKeyData2[] = {0x2d, 0xaf, 0x41, 0xac, 0x1d, 0x3c, 0x68, 0xa4,
										   0x0e, 0x8c, 0xc8, 0xdd, 0x78, 0xcd, 0x9c, 0x23
										  };
#elif (BCHP_CHIP==7401)
	unsigned char 	CKeyData1[] = {0X23, 0x4e, 0x61, 0x67, 0x72, 0x61, 0x4b, 0x75,
										   0x64, 0x65, 0x6c, 0x73, 0x6b, 0x69, 0x23, 0x23
										  };

	unsigned char 	CKeyData2[] = {0X23, 0x4e, 0x61, 0x67, 0x72, 0x61, 0x4b, 0x75,
										   0x64, 0x65, 0x6c, 0x73, 0x6b, 0x69, 0x23, 0x23
										  };
#elif  (BCHP_CHIP == 7325)
	unsigned char 	CKeyData1[] = {0x2d, 0xaf, 0x41, 0xac, 0x1d, 0x3c, 0x68, 0xa4,
										   0x2c, 0x4b, 0x28, 0x32, 0xee, 0x01, 0xbc, 0x6d
										  };
	unsigned char 	CKeyData2[] = {0x2d, 0xaf, 0x41, 0xac, 0x1d, 0x3c, 0x68, 0xa4,
										   0x2c, 0x4b, 0x28, 0x32, 0xee, 0x01, 0xbc, 0x6d
										  };
#elif  (BCHP_CHIP == 7335)
	unsigned char 	CKeyData1[] = {0x2d, 0xaf, 0x41, 0xac, 0x1d, 0x3c, 0x68, 0xa4,
										   0xd8, 0xe8, 0xe0, 0xb0, 0xa5, 0x36, 0xc6, 0x4a
										  };
	unsigned char 	CKeyData2[] = {0x2d, 0xaf, 0x41, 0xac, 0x1d, 0x3c, 0x68, 0xa4,
										   0xd8, 0xe8, 0xe0, 0xb0, 0xa5, 0x36, 0xc6, 0x4a
										  };
#elif (BCHP_CHIP == 7346)
	/* Temporal key data, should be updated after getting key data from Broadcom */
	unsigned char 	CKeyData1[] = {0x2d, 0xaf, 0x41, 0xac, 0x1d, 0x3c, 0x68, 0xa4,
										   0xd8, 0xe8, 0xe0, 0xb0, 0xa5, 0x36, 0xc6, 0x4a
										  };
	unsigned char 	CKeyData2[] = {0x2d, 0xaf, 0x41, 0xac, 0x1d, 0x3c, 0x68, 0xa4,
										   0xd8, 0xe8, 0xe0, 0xb0, 0xa5, 0x36, 0xc6, 0x4a
										  };
#elif (BCHP_CHIP == 7422)
	/* Temporal key data, should be updated after getting key data from Broadcom */
	unsigned char 	CKeyData1[] = {0x2d, 0xaf, 0x41, 0xac, 0x1d, 0x3c, 0x68, 0xa4,
										   0xd8, 0xe8, 0xe0, 0xb0, 0xa5, 0x36, 0xc6, 0x4a
										  };
	unsigned char 	CKeyData2[] = {0x2d, 0xaf, 0x41, 0xac, 0x1d, 0x3c, 0x68, 0xa4,
										   0xd8, 0xe8, 0xe0, 0xb0, 0xa5, 0x36, 0xc6, 0x4a
										  };
#elif (BCHP_CHIP == 7231)
	/* Temporal key data, should be updated after getting key data from Broadcom */
	unsigned char 	CKeyData1[] = {0x2d, 0xaf, 0x41, 0xac, 0x1d, 0x3c, 0x68, 0xa4,
										   0xd8, 0xe8, 0xe0, 0xb0, 0xa5, 0x36, 0xc6, 0x4a
										  };
	unsigned char 	CKeyData2[] = {0x2d, 0xaf, 0x41, 0xac, 0x1d, 0x3c, 0x68, 0xa4,
										   0xd8, 0xe8, 0xe0, 0xb0, 0xa5, 0x36, 0xc6, 0x4a
										  };
#elif (BCHP_CHIP == 7358)
	unsigned char	CKeyData1[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
										0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
										  };
	unsigned char	CKeyData2[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
										0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
										  };
#elif (BCHP_CHIP == 7425)
	/* Temporal key data, should be updated after getting key data from Broadcom */
	unsigned char	CKeyData1[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
										0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
										  };
	unsigned char	CKeyData2[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
										0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
										  };
#elif (BCHP_CHIP == 7435)
	/* Temporal key data, should be updated after getting key data from Broadcom */
	unsigned char	CKeyData1[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
										0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
										  };
	unsigned char	CKeyData2[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
										0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
										  };
#elif (BCHP_CHIP == 7344)
		unsigned char	CKeyData1[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
											0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
											  };
		unsigned char	CKeyData2[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
											0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
											  };
#elif (BCHP_CHIP == 7241) || (BCHP_CHIP == 7429)
	/* Temporal key data, should be updated after getting key data from Broadcom */
	unsigned char 	CKeyData1[] = {0x2d, 0xaf, 0x41, 0xac, 0x1d, 0x3c, 0x68, 0xa4,
										   0xd8, 0xe8, 0xe0, 0xb0, 0xa5, 0x36, 0xc6, 0x4a
										  };
	unsigned char 	CKeyData2[] = {0x2d, 0xaf, 0x41, 0xac, 0x1d, 0x3c, 0x68, 0xa4,
										   0xd8, 0xe8, 0xe0, 0xb0, 0xa5, 0x36, 0xc6, 0x4a
										  };
#elif (BCHP_CHIP == 7445)
	/* Temporal key data, should be updated after getting key data from Broadcom */
	unsigned char	CKeyData1[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
										0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
										  };
	unsigned char	CKeyData2[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
										0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
										  };
#elif (BCHP_CHIP == 7362)
	unsigned char	CKeyData1[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
										0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
										  };
	unsigned char	CKeyData2[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
										0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
										  };
#elif (BCHP_CHIP == 7563)
		unsigned char	CKeyData1[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
											0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
											  };
		unsigned char	CKeyData2[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
											0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
											  };
#elif (BCHP_CHIP == 7584)
	/* Temporal key data, should be updated after getting key data from Broadcom */
	unsigned char	CKeyData1[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
										0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
										  };
	unsigned char	CKeyData2[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
										0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
										  };
#elif (BCHP_CHIP == 7552 )
	unsigned char	CKeyData1[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
										0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
										  };
	unsigned char	CKeyData2[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
										0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
										  };
#elif (BCHP_CHIP == 7250 )
	unsigned char	CKeyData1[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
										0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
										  };
	unsigned char	CKeyData2[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
										0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
										  };
#elif (BCHP_CHIP == 7439 ) || (BCHP_CHIP == 7252)
	unsigned char	CKeyData1[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
										0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
										  };
	unsigned char	CKeyData2[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
										0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
										  };
#elif (BCHP_CHIP == 73625)
		unsigned char	CKeyData1[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
											0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
											  };
		unsigned char	CKeyData2[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
											0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
											  };
#else
#error "CHIP not supported "
#endif

	CSDDataBlock			pxSrc;
	CSDDataBlock			pxDst;
	unsigned char			*encrypt_input = NULL;
	unsigned char			*encrypt_output = NULL;

//	unsigned int			i;
	unsigned int 			remain_length = 0;
	unsigned int 			written_length = 0;
	CSDRam2RamChannelID 	ram2ramChId;
	CSDRam2RamChannelID		*pRam2RamChannelID = &ram2ramChId;

	/* DMA src/dest buffers must be from Heap and alligned. Pls see the usage of Heap, Size, Alignment, Boundry
		the size of alligned buffer has no interface to change it, now hard coded to 16 bytes !!
	*/

	PrintEnter();

	NEXUS_Memory_Allocate(16, NULL, (void *)&encrypt_input);
	NEXUS_Memory_Allocate(16, NULL, (void *)&encrypt_output);

	VK_memset((void *)encrypt_input, 0x00, 16);
	VK_memset((void *)encrypt_output, 0x00, 16);

	pxProtectingKeyTable[0].length = 16;
	pxProtectingKeyTable[0].pData  =(unsigned char *) CKeyData1;

	pxProtectingKeyTable[1].length = 16;
	pxProtectingKeyTable[1].pData  = (unsigned char *)CKeyData2;

	if ( isEnryption == 0 )
	{
	    if ( csdAllocRam2RamKey(pRam2RamChannelID, g_ulDmaHandle, CSD_CRYPTO_MODE_TDES_ECB, 0) != DRV_OK )
	    {
			PrintError("Error:alloc Ram2Ram key error!\n");
			return DRV_ERR;
	    }
	}
	else
	{
	    if ( csdAllocRam2RamKey(pRam2RamChannelID, g_ulDmaHandle, CSD_CRYPTO_MODE_TDES_ECB, 1) != DRV_OK )
	    {
			PrintError("Error:alloc Ram2Ram key error!\n");
			return DRV_ERR;
	    }
	}

	if(csdSetRam2RamKey(pRam2RamChannelID,
						2,
						pxProtectingKeyTable,
						CSD_CRYPTO_MODE_TDES_ECB) != DRV_OK)
	{
		PrintError("Error:set Ram2Ram key error!\n");
        csdFreeRam2RamKey(pRam2RamChannelID);
		return DRV_ERR;
	}

	remain_length = uiLength;
	written_length = 0;
	while(remain_length > 0 )
	{
   		if ( remain_length < 16 )
		{
			VK_MEM_Memset(encrypt_input, 0x00, 16);
			VK_MEM_Memcpy(encrypt_input, &pInputData[written_length], remain_length);

	        pxSrc.length = 16;
	        pxSrc.pData = encrypt_input;
	        pxDst.length = 16;
	        pxDst.pData = encrypt_output;

	        PrintDebug(" to invoke DMA %d, pBufSrc = %08X src[0]=%02X src[15]=%02X,  pBufDest=%08X--- \n",  __LINE__,
							        (unsigned int) pxSrc.pData, pxSrc.pData[0], pxSrc.pData[15],(unsigned int) pxDst.pData );

	        if(csdEncryptData(pRam2RamChannelID, &pxSrc, &pxDst, CSD_CRYPTO_MODE_TDES_ECB) != DRV_OK)
	        {
		        PrintError("Error on encrypt data!\n");
                csdFreeRam2RamKey(pRam2RamChannelID);
		        NEXUS_Memory_Free(encrypt_input);
		        NEXUS_Memory_Free(encrypt_output);
                return DRV_ERR;
	        }

			VK_MEM_Memcpy(&pOutputData[written_length], encrypt_output, remain_length);
			remain_length = 0;
			written_length = written_length+remain_length;
		}
		else
		{
		    VK_MEM_Memcpy(encrypt_input, &pInputData[written_length], 16);

			pxSrc.length = 16;
			pxSrc.pData = encrypt_input;
			pxDst.length = 16;
			pxDst.pData = encrypt_output;

			PrintDebug(" to invoke DMA %d, pBufSrc = %08X src[0]=%02X src[15]=%02X,  pBufDest=%08X--- \n",  __LINE__,
									(unsigned int) pxSrc.pData, pxSrc.pData[0], pxSrc.pData[15],(unsigned int) pxDst.pData );

			if(csdEncryptData(pRam2RamChannelID, &pxSrc, &pxDst, CSD_CRYPTO_MODE_TDES_ECB) != DRV_OK)
			{
				PrintError("Error on encrypt data!\n");
		        csdFreeRam2RamKey(pRam2RamChannelID);
				NEXUS_Memory_Free(encrypt_input);
				NEXUS_Memory_Free(encrypt_output);
		        return DRV_ERR;
			}

			VK_MEM_Memcpy(&pOutputData[written_length], encrypt_output, 16);
			remain_length = remain_length-16;
			written_length = written_length + 16;
		}
	}

        csdFreeRam2RamKey(pRam2RamChannelID);
	NEXUS_Memory_Free(encrypt_input);
	NEXUS_Memory_Free(encrypt_output);
	PrintExit();

	return DRV_OK;
#endif /* CONFIG_CAS_NA && CONFIG_CAS_NA_NOCS_3X */
}

#if defined(CONFIG_CAS_NA)
static DRV_Error P_CSDGetStbCaSnByPK(HUINT8 *pucStbCaSn)
{
	DRV_Error 	eDrvErr=DRV_ERR;
	HUINT32		bootFlashHandle;
	HUINT8		*pFlashPKBuf = NULL;
	HUINT8		*pDecryptedPKBuf = NULL;
#if defined(CONFIG_CAS_NA_NOCS_3X)
	HUINT8		*pFlashCscDataBuf = NULL;
#endif

#if defined(CONFIG_NAND_FLASH) && !defined(CONFIG_NOR_FLASH)
	eDrvErr = DRV_NANDFLASH_BOOT_GetHandle(&bootFlashHandle);
	if(eDrvErr != DRV_OK)
	{
		PrintError("[%s] DRV_NANDFLASH_BOOT_GetHandle error !!\n",__func__);
		return DRV_ERR;
	}
#endif
	/* Memory Allocation */
	pFlashPKBuf = (HUINT8*)VK_MEM_Alloc(NAGRA_PK_BLOCK_SIZE);
	if(pFlashPKBuf == NULL)
	{
		PrintError("[%s:%d] pSrcBuffer is still NULL!! \n\r", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	pDecryptedPKBuf = (HUINT8*)VK_MEM_Alloc(NAGRA_PK_BLOCK_SIZE);
	if(pDecryptedPKBuf == NULL)
	{
		PrintError("[%s:%d] pDstBuffer is still NULL!! \n\r", __FUNCTION__, __LINE__);
		eDrvErr = DRV_ERR;
		goto err;
	}
	
#if defined(CONFIG_NOR_FLASH)
	eDrvErr = (DRV_Error)DI_FLASH_Read(NAGRA_PK_LOGICAL_OFFSET, pFlashPKBuf, NAGRA_PK_BLOCK_SIZE);
	if(eDrvErr != DRV_OK)
	{
		PrintError("[%s:%d] (DRV_Error)DI_FLASH_Read() returned ERROR!!\r\n", __FUNCTION__, __LINE__);
		eDrvErr = DRV_ERR;
		goto err;
	}
#else
	/* Read PK from flash memory according to flash map */
	eDrvErr = DRV_NANDFLASH_BOOT_Read(bootFlashHandle,
					NAGRA_PK_LOGICAL_OFFSET,
					pFlashPKBuf,
					NAGRA_PK_BLOCK_SIZE);

	if(eDrvErr != DRV_OK)
	{
		PrintError("[%s:%d] DRV_NANDFLASH_BOOT_Read() returned ERROR!!\r\n", __FUNCTION__, __LINE__);
		eDrvErr = DRV_ERR;
		goto err;
	}
#endif

	/* Decryption PK by Flash memory */
	eDrvErr = P_CSDGetEncryptionData(pFlashPKBuf, pDecryptedPKBuf, NAGRA_PK_BLOCK_SIZE, 0);
	if(eDrvErr != DRV_OK)
	{
		PrintError("[%s:%d] P_CSDGetEncryptionData() returned ERROR!!\r\n", __FUNCTION__, __LINE__);
		eDrvErr = DRV_ERR;
		goto err;
	}

	/* the paring key file is made-up as PK_LENGTH_SIZE(4) + STB_CA_SN(4) + PK(N) + PK_SIGN(128) + CRC(2) */
	pucStbCaSn[0] = pDecryptedPKBuf[4];
	pucStbCaSn[1] = pDecryptedPKBuf[5];
	pucStbCaSn[2] = pDecryptedPKBuf[6];
	pucStbCaSn[3] = pDecryptedPKBuf[7];

	PrintDebug("serialNumber[0] : %02x\n", pucStbCaSn[0]);
	PrintDebug("serialNumber[1] : %02x\n", pucStbCaSn[1]);
	PrintDebug("serialNumber[2] : %02x\n", pucStbCaSn[2]);
	PrintDebug("serialNumber[3] : %02x\n", pucStbCaSn[3]);

err:
	VK_MEM_Free(pFlashPKBuf);
	VK_MEM_Free(pDecryptedPKBuf);

	return eDrvErr;
}
#endif

static DRV_Error P_CSDSetDataSection1LockBit(void)
{

    NEXUS_Error          errCode = NEXUS_SUCCESS;
    NEXUS_ReadMspParms  readMspParms;
    NEXUS_ReadMspIO     readMspIO;
    NEXUS_ProgramMspIO  progMspIO;
    unsigned int        ulVal;
	unsigned char ucMSPData;

	ucMSPData = 0x2;

	VK_memset(&readMspParms, 0x00, sizeof(readMspParms));

    readMspParms.readMspEnum    = NEXUS_OtpCmdMsp_eDataSectionReadProtectBits;
    errCode = NEXUS_Security_ReadMSP(&readMspParms, &readMspIO);
    if (errCode != 0)
    {
        PrintError("NEXUS_Security_ReadMSP() errCode: %x\n", errCode );
		return DRV_ERR;
    }

    ulVal = readMspIO.mspDataBuf[3];
    PrintDebug("**** %x  %x   %x   %x\n", readMspIO.mspDataBuf[0], readMspIO.mspDataBuf[1],
                                      readMspIO.mspDataBuf[2], readMspIO.mspDataBuf[3]);

    if(ulVal == ucMSPData)
    {
        PrintDebug("Already programmed - bit value: %x\n", ulVal );
    }
	else
	{
		progMspIO.progMode		 = NEXUS_OTP_DATASECTIONPROG_MODE;
		progMspIO.progMspEnum	 = NEXUS_OtpCmdMsp_eDataSectionReadProtectBits;
		progMspIO.dataBitLen	 = 8;
		progMspIO.dataBitMask[0] = 0x0;
		progMspIO.dataBitMask[1] = 0x0;
		progMspIO.dataBitMask[2] = 0x0;
		progMspIO.dataBitMask[3] = ucMSPData;
		progMspIO.mspData[0]	 = 0x0;
		progMspIO.mspData[1]	 = 0x0;
		progMspIO.mspData[2]	 = 0x0;
		progMspIO.mspData[3]	 = ucMSPData;

		errCode = NEXUS_Security_ProgramMSP(&progMspIO);
		if (errCode != 0)
		{
			PrintError("NEXUS_Security_ProgramMSP() errCode: %x\n", errCode );
			return DRV_ERR;
		}

		VK_memset(&readMspParms, 0x00, sizeof(readMspParms));

	    readMspParms.readMspEnum    = NEXUS_OtpCmdMsp_eDataSectionReadProtectBits;
	    errCode = NEXUS_Security_ReadMSP(&readMspParms, &readMspIO);
	    if (errCode != 0)
	    {
	        PrintError("NEXUS_Security_ReadMSP() errCode: %x\n", errCode);
			return DRV_ERR;
	    }

	    ulVal = readMspIO.mspDataBuf[3];
	    PrintDebug("**** %x  %x   %x   %x\n", readMspIO.mspDataBuf[0], readMspIO.mspDataBuf[1],
	                                      readMspIO.mspDataBuf[2], readMspIO.mspDataBuf[3]);

	    if(ulVal)
	    {
	        PrintError("Programmed...Passed - bit value: %d\n", ulVal );
	    }
	    else
	    {
	        PrintError("Programmed...Failed - bit value: %d\n", ulVal );
			return DRV_ERR;
	    }
	}
	return DRV_OK;
}

static DRV_Error P_CSDSetSCSVersioing(HUINT16 usPvLength, HUINT8 *pucPvValue)
{
#if defined(CONFIG_CAS_NA) && defined(CONFIG_CAS_NA_NOCS_3X)
	TCsdStatus CsdStatus;
	TCsdScsPvPathHandle xPvPathHandle; /* Currently not used. 2015.11.11 */
	TCsdUnsignedInt8 ucPvValue[2] = {0,};

	/* Currently, usPvLength is should be 1. 2015.11.11 */
	ucPvValue[0] = (TCsdUnsignedInt8)*pucPvValue;
	CsdStatus = csdSetScsPv(CSD_SCS_PV_VERSIONING_REF, &xPvPathHandle, (TCsdUnsignedInt16)usPvLength, ucPvValue);

	if(CsdStatus != CSD_NO_ERROR)
	{
		PrintError("[%s:%d] csdSetScsPv() returned ERROR!!\r\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}
#else
	BSTD_UNUSED(usPvLength);
	BSTD_UNUSED(pucPvValue);
#endif
	return DRV_OK;
}

static DRV_Error P_CSDGetSCSVersioing(HUINT16 usPvLength, HUINT8 *pucPvValue)
	{
#if defined(CONFIG_CAS_NA) && defined(CONFIG_CAS_NA_NOCS_3X)
	TCsdStatus CsdStatus;
	TCsdScsPvPathHandle xPvPathHandle; /* Currently not used. 2015.11.11 */
	TCsdUnsignedInt8 ucPvValue[2] = {0,};

	*pucPvValue = 0;

	/* Currently, usPvLength is should be 1. 2015.11.11 */
	CsdStatus = csdGetScsPv(CSD_SCS_PV_VERSIONING_REF, &xPvPathHandle, (TCsdUnsignedInt16)usPvLength, ucPvValue);

	if(CsdStatus != CSD_NO_ERROR)
	{
		PrintError("[%s:%d] csdGetScsPv() returned ERROR!!\r\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	*pucPvValue = (HUINT8)ucPvValue[0];
#else
	BSTD_UNUSED(usPvLength);
	BSTD_UNUSED(pucPvValue);
#endif
		return DRV_OK;
}

static DRV_Error P_CSDSetFlashProtection(void)
{
#if defined(CONFIG_CAS_NA) && defined(CONFIG_CAS_NA_NOCS_3X)
	TCsdStatus CsdStatus;

	CsdStatus = csdEnableFlashProtection();

	if(CsdStatus != CSD_NO_ERROR)
	{
		PrintError("[%s:%d] csdEnableFlashProtection() returned ERROR!!\r\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}
#endif
	return DRV_OK;
}

static DRV_Error P_CSDGetFlashProtection(HUINT8 *pucFlag)
{
#if defined(CONFIG_CAS_NA) && defined(CONFIG_CAS_NA_NOCS_3X)
	TCsdStatus CsdStatus;
	TCsdActivationFlag xFlashProtectionActivationFlagState = CSD_ACTIVATION_FLAG_NOT_SET;

	if(pucFlag == NULL)
	{
		PrintError("[%s:%d] Invalid Parameter ERROR!!\r\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	*pucFlag = 0;

	CsdStatus = csdGetFlashProtectionActivationFlag(&xFlashProtectionActivationFlagState);

	if(CsdStatus != CSD_NO_ERROR)
	{
		PrintError("[%s:%d] csdGetScsPv() returned ERROR!!\r\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	if(CSD_ACTIVATION_FLAG_SET == xFlashProtectionActivationFlagState)
	{
		*pucFlag = 0x01;
	}
	else
	{
		*pucFlag = 0;
	}
#else
	BSTD_UNUSED(pucFlag);
#endif
	return DRV_OK;
}

/**********************************************************************************************
						 CDS Public Functions
**********************************************************************************************/
DRV_Error DRV_CSDInitialize( void )
{
	NEXUS_DmaSettings 	dmaSettings;
#if defined(CONFIG_CAS_NA)
	TCsdInitParameters	initParameters;
	TCsdStatus			status;
#endif

	PrintEnter();

#if defined(CONFIG_CAS_NA)
	VK_MEM_Memset((void *)&initParameters, 0, sizeof(TCsdInitParameters));
	status = csdInitialize(&initParameters);
	if (status != CSD_NO_ERROR)
	{
		PrintError("[DRV_CSDInitialize] csdInitialize error(0x%x)\n", status);

		PrintExit();

		return DRV_ERR;
	}
#endif

	NEXUS_Dma_GetDefaultSettings(&dmaSettings);
    dmaSettings.endianMode = NEXUS_DmaEndianMode_eLittle;
    dmaSettings.swapMode   = NEXUS_DmaSwapMode_eNone;
    NEXUS_Dma_SetSettings(g_ulDmaHandle, &dmaSettings);

	PrintExit();

	return DRV_OK;
}

DRV_Error DRV_CSDGetChipID(HUINT32 *pulChipID)
{
	NEXUS_ReadOtpIO	ReadOtpIO;
	NEXUS_Error errCode = NEXUS_SUCCESS;

	PrintEnter();
	VK_memset(&ReadOtpIO, 0x00, sizeof(ReadOtpIO));

	errCode = NEXUS_Security_ReadOTP(NEXUS_OtpCmdReadRegister_eKeyID, NEXUS_OtpKeyType_eA, &ReadOtpIO);
	if (errCode != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_Security_ReadOTP() errCode: %x\n", errCode );
		return DRV_ERR;
	}

	*pulChipID = (ReadOtpIO.otpKeyIdBuf[CHIP_ID_INDEX] << 24) + (ReadOtpIO.otpKeyIdBuf[CHIP_ID_INDEX + 1] << 16)
				+ (ReadOtpIO.otpKeyIdBuf[CHIP_ID_INDEX + 2] << 8) + ReadOtpIO.otpKeyIdBuf[CHIP_ID_INDEX + 3];

	PrintDebug("Chip ID : 0x%08X(%d)\n", *pulChipID, ReadOtpIO.otpKeyIdSize);

	PrintExit();

	return DRV_OK;
}

DRV_Error DRV_CSDGetChipIDx64(HUINT8 *pszChipIDx64)
{
	NEXUS_ReadOtpIO	ReadOtpIO;
	NEXUS_Error errCode = NEXUS_SUCCESS;

	PrintEnter();
	VK_memset(&ReadOtpIO, 0x00, sizeof(ReadOtpIO));

	errCode = NEXUS_Security_ReadOTP(NEXUS_OtpCmdReadRegister_eKeyID, NEXUS_OtpKeyType_eA, &ReadOtpIO);
	if (errCode != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_Security_ReadOTP() errCode: %x\n", errCode );
		return DRV_ERR;
	}

	// copy 8 byte chip id
	VK_MEM_Memcpy(pszChipIDx64, ReadOtpIO.otpKeyIdBuf, 8);

	PrintExit();

	return DRV_OK;
}

DRV_Error DRV_CSDGetSCS(HUINT8 *pucSCS)
{
	DRV_Error errCode = 0;

	PrintEnter();

	errCode = P_CSDGetBootMem((int *)pucSCS);
	if (errCode != 0)
	{
		PrintError("P_CSDGetBootMem() errCode: %x\n", errCode );

		return DRV_ERR;
	}

	PrintDebug("SCS : 0x%02X\n", *pucSCS);

	PrintExit();

	return DRV_OK;
}

DRV_Error DRV_CSDSetSCS(int lBlockSizeKB, int lKeyIndex)
{
	DRV_Error 	errCode = 0;
	int			i, cur_index, cur_boot_flag;
	int			cur_bockno = 0, new_blockno = 0;

	PrintEnter();

	/* 1st Step :  Setting Flash Size of BootCode, if we uses BSECK, then we don't need block size  */
	if( lBlockSizeKB > 0 )
	{
		/* 1.0 : convert blocksize define */
		/* 		0 for 8k, 1 for 16K, 2 for 32k, 3 for 64k,
				4 for 128K, 5 for 256k, 6 for 512k and 7 for 1M */
		for(i=0; i<8; i++)
		{
			if( (lBlockSizeKB >> i) & 0x08 )
			{
				new_blockno = i;

				break;
			}
		}

		if( i >= 8 )
		{
			PrintError("invalid block size %d > 1024 \n", lBlockSizeKB );

			return DRV_ERR;
		}

		PrintDebug("SCS : find block size num 0x%x (%dKB) \n", new_blockno, lBlockSizeKB);

		/* 1.1 : read block size */
		errCode = P_CSDGetBlockNumber(&cur_bockno);
		if (errCode != 0)
		{
			PrintError("P_CSDGetBlockNumber() errCode: %x\n", errCode );

			return DRV_ERR;
		}

		PrintDebug("SCS : Before Block size (0x%x-0x%x)\n", cur_bockno, new_blockno);

		if( new_blockno != cur_bockno )
		{
			/* 1.2 : write block size */
			errCode = P_CSDSetBlockNumber(new_blockno);
			if (errCode != 0)
			{
				PrintError("P_CSDSetBlockNumber() errCode: %x\n", errCode );

				return DRV_ERR;
			}

			/* 1.3 : verify block size */
			errCode = P_CSDGetBlockNumber(&cur_bockno);
			if (errCode != 0)
			{
				PrintError("P_CSDGetBlockNumber() errCode: %x\n", errCode );

				return DRV_ERR;
			}

			if( new_blockno != cur_bockno )
			{
				PrintError("Block size is diffrent (0x%x-0x%x)\n", new_blockno, cur_bockno);

				return DRV_ERR;
			}
		}
	}

	/* 2nd Step :  Setting  Key index. - should be "0" */
	if( lKeyIndex > 0 )
	{
		/* 2.1 : read Key index */
		errCode = P_CSDGetKeyIndex(&cur_index);
		if (errCode != 0)
		{
			PrintError("P_CSDGetKeyIndex() errCode: %x\n", errCode );

			return DRV_ERR;
		}

		PrintDebug("SCS : Before Key Index (0x%x-0x%x)\n", cur_index, lKeyIndex);

		if( lKeyIndex != cur_index )
		{
			/* 2.2 : write Key index */
			errCode = P_CSDSetKeyIndex(lKeyIndex);
			if (errCode != 0)
			{
				PrintError("P_CSDSetKeyIndex() errCode: %x\n", errCode );

				return DRV_ERR;
			}

			/* 2.3 : verify Key index */
			errCode = P_CSDGetKeyIndex(&cur_index);
			if (errCode != 0)
			{
				PrintError("P_CSDGetKeyIndex() errCode: %x\n", errCode );

				return DRV_ERR;
			}

			if( lKeyIndex != cur_index )
			{
				PrintError("key index is diffrent (0x%x-0x%x)\n", lKeyIndex, cur_index);

				return DRV_ERR;
			}
		}
	}
#if defined(CONFIG_CAS_NA)
#if defined(CONFIG_CAS_NA_NOCS_3X)
	/* NOCS 3.X needs to encrypt Flash Protection Key */
	errCode = P_CSDSetEncryptFPK();
	if (errCode != 0)
	{
		PrintError("P_CSDSetEncryptFPK() errCode: %x\n", errCode );
		return DRV_ERR;
	}

	errCode = P_CSDSetRamScrambling();
	if (errCode != 0)
	{
		PrintError("P_CSDSetRamScrambling() errCode: %x\n", errCode );
		return DRV_ERR;
	}
#endif

#if defined(CONFIG_CAS_NA_NOCS_1X)
	errCode = P_CSDSetBootMode(DRV_CSD_BOOT_FROM_NAND);
	if (errCode != 0)
	{
		PrintError("P_CSDGetBootMode() errCode: %x\n", errCode );
		return DRV_ERR;
	}
#endif

	errCode = P_CSDSetMarketID(P_CSD_MARKET_ID_DEFAULT);
	if (errCode != 0)
	{
		PrintError("P_CSDSetMarketID() errCode: %x\n", errCode );
		return DRV_ERR;
	}

	{

		/* Read, Decrypt, Get 4bytes Stb Ca Sn from specific flash area.
		*  Set them into OTP area.
		*/
		HUINT8	pucStbCaSn[4] = {0,0,0,0};
		HUINT8	pucStbCaSnCmp[4] = {0,0,0,0};

		errCode = P_CSDGetStbCaSnByPK(pucStbCaSn);
		if(errCode != 0)
		{
			PrintError("P_CSDGetStbCaSnByPK() errCode: %x\n", errCode );
			return DRV_ERR;
		}


		errCode = P_CSDGetStbCaSn(pucStbCaSnCmp);
		if(errCode != 0)
		{
			PrintError("P_CSDGetStbCaSnByPK() errCode: %x\n", errCode );
			return DRV_ERR;
		}

		if((pucStbCaSn[3] == pucStbCaSnCmp[3]) && (pucStbCaSn[2] == pucStbCaSnCmp[2])
			&& (pucStbCaSn[1] == pucStbCaSnCmp[1]) && (pucStbCaSn[0] == pucStbCaSnCmp[0]))
		{
			PrintError("STB CA SN [0x%02x][0x%02x][0x%02x][0x%02x] is same in OTP AREA!!\n",
			pucStbCaSn[3],pucStbCaSn[2],pucStbCaSn[1],pucStbCaSn[0]);

		}
		else
		{
			errCode = P_CSDSetStbCaSn(pucStbCaSn);
			if(errCode != 0)
			{
				PrintError("P_CSDGetStbCaSnByPK() errCode: %x\n", errCode );
				return DRV_ERR;
			}
		}
	}
#endif

#if defined(CONFIG_CAS_CX)
	errCode = P_CSDSetMarketID(P_CSD_MARKET_ID_DEFAULT);
	if (errCode != 0)
	{
		PrintError("P_CSDSetMarketID() errCode: %x\n", errCode);
		return DRV_ERR;
	}

	errCode = DRV_CSDSetHostBootCodeDescryptionSelect(P_CSD_HOST_BOOT_DECRYPT_DEFAULT);
	if (errCode != 0)
	{
		PrintError("DRV_CSDSetHostBootCodeDescryptionSelect() errCode: %x\n", errCode);
		return DRV_ERR;
	}
#endif
	/* 3rd Step :  Enable the secure flash. */

	/* 3.1 : write Secure Boot enable */
	errCode = P_CSDSetBootMem();
	if (errCode != 0)
	{
		PrintError("P_CSDSetBootMem() errCode: %x\n", errCode );

		return DRV_ERR;
	}

	/* 3.3 : verify Secure Boot enable */
	errCode = P_CSDGetBootMem(&cur_boot_flag);
	if (errCode != 0)
	{
		PrintError("P_CSDGetBootMem() errCode: %x\n", errCode );

		return DRV_ERR;
	}

	if( cur_boot_flag != 1 )
	{
		PrintError("fail secure boot enable!!\n");

		return DRV_ERR;
	}

	PrintExit();

	return DRV_OK;
}

DRV_Error DRV_CSDGetJTAG(HUINT8 *pucJTAG)
{
	DRV_Error errCode = 0;

	PrintEnter();

	errCode = P_CSDGetJTAGStatus((int *)pucJTAG);
	if (errCode != 0)
	{
		PrintError("P_CSDGetJTAGStatus() errCode: %x\n", errCode );

		return DRV_ERR;
	}

	/* JTAG Fusing 진행 하게 되면(DRV_CSDSetJTAG 실행) info에 ENABLE(TRUE)로 표시 되어야 함. */
	/* BCM 특성 상 reboot 되어야 실재 fusing 정보가 업데이트 됨. FACTORY 모드 표기 요구 사항에 따라 fusing status 관리 필요. */
	if( gJTAG_FusingProcess == TRUE )
	{
		*pucJTAG = TRUE;
	}
	else
	{
		/* fusing on(set) : JTAG Lock Enable */
		*pucJTAG = ( *pucJTAG != 0 ) ? TRUE : FALSE;
	}

	PrintDebug("JTAG : 0x%02X\n", *pucJTAG);

	PrintExit();

	return DRV_OK;
}

DRV_Error DRV_CSDSetJTAG(void)
{
	DRV_Error 	errCode = 0;
	int			cur_flag;

	PrintEnter();

	/* 1 : write JTAG disable */
	errCode = P_CSDSetJTAGDisable();
	if (errCode != 0)
	{
		PrintError("P_CSDSetJTAGDisable() errCode: %x\n", errCode );

		return DRV_ERR;
	}

	/* 2 : verify JTAG status */
	errCode = P_CSDGetJTAGStatus(&cur_flag);
	if (errCode != 0)
	{
		PrintError("P_CSDGetJTAGStatus() errCode: %x\n", errCode );

		return DRV_ERR;
	}

	PrintDebug("JTAG : 0x%02X\n", cur_flag);

#if 0
	if( cur_flag == 0 )
	{
		PrintError("fail JTAG disable!!\n");
		return DRV_ERR;
	}
#endif

	gJTAG_FusingProcess = TRUE;

	PrintExit();

	return DRV_OK;
}

DRV_Error DRV_CSDGetCWE(HUINT8 *pucCWE)
{
	DRV_Error errCode = 0;

	PrintEnter();

	errCode = P_CSDGetCWEStatus((int *)pucCWE);
	if (errCode != 0)
	{
		PrintError("P_CSDGetCWEStatus() errCode: %x\n", errCode );
		return DRV_ERR;
	}

	PrintDebug("CWE : 0x%02X\n", *pucCWE);

	PrintExit();

	return DRV_OK;
}

DRV_Error DRV_CSDSetCWE(void)
{
	DRV_Error 	errCode = 0;
	int			cur_flag;

	PrintEnter();

	/* 1 : write Control Word Encryption */
	errCode = P_CSDSetCWE();
	if (errCode != 0)
	{
		PrintError("P_CSDSetCWE() errCode: %x\n", errCode );

		return DRV_ERR;
	}

	/* 2 : verify CWE status */
	errCode = P_CSDGetCWEStatus(&cur_flag);
	if (errCode != 0)
	{
		PrintError("P_CSDGetCWEStatus() errCode: %x\n", errCode );

		return DRV_ERR;
	}

	PrintDebug("CWE : 0x%02X\n", cur_flag);

	if( cur_flag == 0 )
	{
		PrintError("fail CWE enable!!\n");

		return DRV_ERR;
	}

	PrintExit();

	return DRV_OK;
}



DRV_Error DRV_CSDGetSTBOwnerID(HUINT16 *pusSTBOwnerID)
{
	DRV_Error errCode = 0;

	PrintEnter();

	errCode = P_CSDGetSTBOwnerID(pusSTBOwnerID);
	if (errCode != 0)
	{
		PrintError("P_CSDGetCWEStatus() errCode: %x\n", errCode );
		return DRV_ERR;
	}

	PrintDebug("STB Owner ID : 0x%04X\n", *pusSTBOwnerID);

	PrintExit();

	return DRV_OK;
}

DRV_Error DRV_CSDSetSTBOwnerID(HUINT16 usSTBOwnerID)
{
	DRV_Error 	errCode = 0;
	HUINT16		usCurSTBOwnerID = 0;

	PrintEnter();

	/* 1 : write STB Owner ID */
	errCode = P_CSDSetSTBOwnerID(usSTBOwnerID);
	if (errCode != 0)
	{
		PrintError("P_CSDSetSTBOwnerID() errCode: %x\n", errCode );
	}

	/* 2 : verify CWE status */
	errCode = P_CSDGetSTBOwnerID(&usCurSTBOwnerID);
	if (errCode != 0)
	{
		PrintError("P_CSDGetSTBOwnerID() errCode: %x\n", errCode );
		return DRV_ERR;
	}

	PrintDebug("[%s, %d] usCurSTBOwnerID : 0x%04X\n", __FUNCTION__, __LINE__, usCurSTBOwnerID);
	if(usCurSTBOwnerID != usSTBOwnerID)
	{
		PrintError("usCurSTBOwnerID(0x%04x) is different from usSTBOwnerID(0x%04x)\n",usCurSTBOwnerID, usSTBOwnerID );
		return DRV_ERR;
	}

	PrintDebug("[%s, %d] STB Owner ID : 0x%04X\n", __FUNCTION__, __LINE__, usSTBOwnerID);

	PrintExit();

	return DRV_OK;
}


DRV_Error DRV_CSDGetMarketID(HUINT8 *pucMarketID)
{
#if defined(CONFIG_CAS_NA)
	TCsdStatus CsdStatus;
	TCsd4BytesVector MarketSegmentId;

	CsdStatus=csdGetMarketSegmentId(MarketSegmentId);
	if(CsdStatus != CSD_NO_ERROR)
	{
		PrintError("csdGetMarketSegmentId error!\n");
		return DRV_ERR;
	}

	pucMarketID[0] = MarketSegmentId[0];
	pucMarketID[1] = MarketSegmentId[1];
	pucMarketID[2] = MarketSegmentId[2];
	pucMarketID[3] = MarketSegmentId[3];

	return DRV_OK;
#else
	NEXUS_Error errCode = NEXUS_SUCCESS;
	NEXUS_ReadMspParms	ReadMspParms;
	NEXUS_ReadMspIO		ReadMspIO;

	PrintEnter();

	VK_memset(&ReadMspParms, 0x00, sizeof(ReadMspParms));
	VK_memset(&ReadMspIO, 0x00, sizeof(ReadMspIO));

	ReadMspParms.readMspEnum = NEXUS_OtpCmdMsp_eMarketId;
	errCode = NEXUS_Security_ReadMSP(&ReadMspParms, &ReadMspIO);

	pucMarketID[0] = ReadMspIO.mspDataBuf[0];
	pucMarketID[1] = ReadMspIO.mspDataBuf[1];
	pucMarketID[2] = ReadMspIO.mspDataBuf[2];
	pucMarketID[3] = ReadMspIO.mspDataBuf[3];


#endif
	PrintDebug("Market ID : 0x%02X%02X%02X%02X\n", pucMarketID[0],pucMarketID[1],pucMarketID[2],pucMarketID[3]);
	PrintExit();

	return DRV_OK;
}

/*  : Update according to the documented procedure */
DRV_Error DRV_CSDGetCheckNumber(HUINT32 *pxCheckNumber)
{
#if defined(CONFIG_CAS_NA)
	TCsdStatus			CsdStatus;
	TCsd4BytesVector 		NuidCheckNumber;

	CsdStatus=csdGetNUIDCheckNumber(NuidCheckNumber);
	if(CsdStatus != CSD_NO_ERROR)
	{
		PrintError(" Get NUID Check Number Error !!\n");
		return DRV_ERR;
	}

	*pxCheckNumber=(NuidCheckNumber[0] << 24) | (NuidCheckNumber[1] << 16 ) | (NuidCheckNumber[2] << 8) | NuidCheckNumber[3];

	return DRV_OK;

#else
	/*TCsdDerivationLevelNumber   xDerivationLevelNumber= 2; hardcode to 2 in the function call*/
	unsigned char		pxNuid[4] = {0, 0, 0, 0};
	CSDKey 				pxProtectingKeyTable[2] ;

#if (BCHP_CHIP==7403)
	unsigned char 	CKeyData1[] = {0X2D, 0xAF, 0x41, 0xAC, 0x1D, 0x3C, 0x68, 0xA4,
										   0x91, 0xCF, 0x5F, 0x30, 0x4E, 0x94, 0x0C, 0xC0
										  };
	unsigned char 	CKeyData2[] = {0X2D, 0xAF, 0x41, 0xAC, 0x1D, 0x3C, 0x68, 0xA4,
										   0x91, 0xCF, 0x5F, 0x30, 0x4E, 0x94, 0x0C, 0xC0
										  };
#elif  (BCHP_CHIP==7405)
	unsigned char 	CKeyData1[] = {0x2d, 0xaf, 0x41, 0xac, 0x1d, 0x3c, 0x68, 0xa4,
										   0x0e, 0x8c, 0xc8, 0xdd, 0x78, 0xcd, 0x9c, 0x23
										  };
	unsigned char 	CKeyData2[] = {0x2d, 0xaf, 0x41, 0xac, 0x1d, 0x3c, 0x68, 0xa4,
										   0x0e, 0x8c, 0xc8, 0xdd, 0x78, 0xcd, 0x9c, 0x23
										  };
#elif (BCHP_CHIP==7401)
	unsigned char 	CKeyData1[] = {0X23, 0x4e, 0x61, 0x67, 0x72, 0x61, 0x4b, 0x75,
										   0x64, 0x65, 0x6c, 0x73, 0x6b, 0x69, 0x23, 0x23
										  };

	unsigned char 	CKeyData2[] = {0X23, 0x4e, 0x61, 0x67, 0x72, 0x61, 0x4b, 0x75,
										   0x64, 0x65, 0x6c, 0x73, 0x6b, 0x69, 0x23, 0x23
										  };
#elif  (BCHP_CHIP == 7325)
	unsigned char 	CKeyData1[] = {0x2d, 0xaf, 0x41, 0xac, 0x1d, 0x3c, 0x68, 0xa4,
										   0x2c, 0x4b, 0x28, 0x32, 0xee, 0x01, 0xbc, 0x6d
										  };
	unsigned char 	CKeyData2[] = {0x2d, 0xaf, 0x41, 0xac, 0x1d, 0x3c, 0x68, 0xa4,
										   0x2c, 0x4b, 0x28, 0x32, 0xee, 0x01, 0xbc, 0x6d
										  };
#elif  (BCHP_CHIP == 7335)
	unsigned char 	CKeyData1[] = {0x2d, 0xaf, 0x41, 0xac, 0x1d, 0x3c, 0x68, 0xa4,
										   0xd8, 0xe8, 0xe0, 0xb0, 0xa5, 0x36, 0xc6, 0x4a
										  };
	unsigned char 	CKeyData2[] = {0x2d, 0xaf, 0x41, 0xac, 0x1d, 0x3c, 0x68, 0xa4,
										   0xd8, 0xe8, 0xe0, 0xb0, 0xa5, 0x36, 0xc6, 0x4a
										  };
#elif (BCHP_CHIP == 7346)
	/* Temporal key data, should be updated after getting key data from Broadcom */
	unsigned char 	CKeyData1[] = {0x2d, 0xaf, 0x41, 0xac, 0x1d, 0x3c, 0x68, 0xa4,
										   0xd8, 0xe8, 0xe0, 0xb0, 0xa5, 0x36, 0xc6, 0x4a
										  };
	unsigned char 	CKeyData2[] = {0x2d, 0xaf, 0x41, 0xac, 0x1d, 0x3c, 0x68, 0xa4,
										   0xd8, 0xe8, 0xe0, 0xb0, 0xa5, 0x36, 0xc6, 0x4a
										  };
#elif (BCHP_CHIP == 7422)
	/* Temporal key data, should be updated after getting key data from Broadcom */
	unsigned char 	CKeyData1[] = {0x2d, 0xaf, 0x41, 0xac, 0x1d, 0x3c, 0x68, 0xa4,
										   0xd8, 0xe8, 0xe0, 0xb0, 0xa5, 0x36, 0xc6, 0x4a
										  };
	unsigned char 	CKeyData2[] = {0x2d, 0xaf, 0x41, 0xac, 0x1d, 0x3c, 0x68, 0xa4,
										   0xd8, 0xe8, 0xe0, 0xb0, 0xa5, 0x36, 0xc6, 0x4a
										  };
#elif (BCHP_CHIP == 7231)
	/* Temporal key data, should be updated after getting key data from Broadcom */
	unsigned char 	CKeyData1[] = {0x2d, 0xaf, 0x41, 0xac, 0x1d, 0x3c, 0x68, 0xa4,
										   0xd8, 0xe8, 0xe0, 0xb0, 0xa5, 0x36, 0xc6, 0x4a
										  };
	unsigned char 	CKeyData2[] = {0x2d, 0xaf, 0x41, 0xac, 0x1d, 0x3c, 0x68, 0xa4,
										   0xd8, 0xe8, 0xe0, 0xb0, 0xa5, 0x36, 0xc6, 0x4a
										  };
#elif (BCHP_CHIP == 7358)
	unsigned char	CKeyData1[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
										0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
										  };
	unsigned char	CKeyData2[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
										0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
										  };
#elif (BCHP_CHIP == 7425)
	/* Temporal key data, should be updated after getting key data from Broadcom */
	unsigned char	CKeyData1[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
										0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
										  };
	unsigned char	CKeyData2[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
										0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
										  };
#elif (BCHP_CHIP == 7435)
	/* Temporal key data, should be updated after getting key data from Broadcom */
	unsigned char	CKeyData1[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
										0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
										  };
	unsigned char	CKeyData2[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
										0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
										  };
#elif (BCHP_CHIP == 7344)
		unsigned char	CKeyData1[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
											0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
											  };
		unsigned char	CKeyData2[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
											0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
											  };
#elif (BCHP_CHIP == 7241)
	/* Temporal key data, should be updated after getting key data from Broadcom */
	unsigned char 	CKeyData1[] = {0x2d, 0xaf, 0x41, 0xac, 0x1d, 0x3c, 0x68, 0xa4,
										   0xd8, 0xe8, 0xe0, 0xb0, 0xa5, 0x36, 0xc6, 0x4a
										  };
	unsigned char 	CKeyData2[] = {0x2d, 0xaf, 0x41, 0xac, 0x1d, 0x3c, 0x68, 0xa4,
										   0xd8, 0xe8, 0xe0, 0xb0, 0xa5, 0x36, 0xc6, 0x4a
										  };

#elif (BCHP_CHIP == 7429)
	/* Temporal key data, should be updated after getting key data from Broadcom */
	unsigned char 	CKeyData1[] = {0x2d, 0xaf, 0x41, 0xac, 0x1d, 0x3c, 0x68, 0xa4,
										   0xd8, 0xe8, 0xe0, 0xb0, 0xa5, 0x36, 0xc6, 0x4a
										  };
	unsigned char 	CKeyData2[] = {0x2d, 0xaf, 0x41, 0xac, 0x1d, 0x3c, 0x68, 0xa4,
										   0xd8, 0xe8, 0xe0, 0xb0, 0xa5, 0x36, 0xc6, 0x4a
										  };
#elif (BCHP_CHIP == 7445)
	/* Temporal key data, should be updated after getting key data from Broadcom */
	unsigned char	CKeyData1[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
										0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
										  };
	unsigned char	CKeyData2[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
										0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
										  };
#elif (BCHP_CHIP == 7362)
		unsigned char	CKeyData1[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
											0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
											  };
		unsigned char	CKeyData2[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
											0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
											  };
#elif (BCHP_CHIP == 7563)
		unsigned char	CKeyData1[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
											0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
											  };
		unsigned char	CKeyData2[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
											0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
											  };
#elif (BCHP_CHIP == 7584)
	/* Temporal key data, should be updated after getting key data from Broadcom */
	unsigned char	CKeyData1[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
										0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
								 		  };
	unsigned char	CKeyData2[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
										0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
										  };
#elif (BCHP_CHIP == 7552 )
	unsigned char	CKeyData1[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
										0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
										  };
	unsigned char	CKeyData2[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
										0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
										  };
#elif (BCHP_CHIP == 7250)
	/* Temporal key data, should be updated after getting key data from Broadcom */
	unsigned char	CKeyData1[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
										0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
										  };
	unsigned char	CKeyData2[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
										0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
										  };
#elif (BCHP_CHIP == 7439 ) || (BCHP_CHIP == 7252)
	/* Temporal key data, should be updated after getting key data from Broadcom */
	unsigned char	CKeyData1[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
										0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
										  };
	unsigned char	CKeyData2[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
										0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
										  };
#elif (BCHP_CHIP == 73625 )
		/* Temporal key data, should be updated after getting key data from Broadcom */
		unsigned char	CKeyData1[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
											0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
											  };
		unsigned char	CKeyData2[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
											0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
											  };

#else
#error "CHIP not supported"
#endif

	CSDDataBlock			pxSrc;
	CSDDataBlock			pxDst;
	unsigned char			*nuidData;
	unsigned char			*pre_nuidData;
	unsigned char			*crcinput;
	unsigned int			i;

	CSDRam2RamChannelID 	ram2ramChId;
	CSDRam2RamChannelID		*pRam2RamChannelID = &ram2ramChId;

	/* DMA src/dest buffers must be from Heap and alligned. Pls see the usage of Heap, Size, Alignment, Boundry
		the size of alligned buffer has no interface to change it, now hard coded to 16 bytes !!
	*/

	PrintEnter();

	NEXUS_Memory_Allocate(16, NULL, (void *)&nuidData);
	NEXUS_Memory_Allocate(16, NULL, (void *)&pre_nuidData);
	NEXUS_Memory_Allocate(16, NULL, (void *)&crcinput);

	VK_memset((void *)nuidData, 0x00, 16);
	VK_memset((void *)pre_nuidData, 0x00, 16);
	VK_memset((void *)crcinput, 0x00, 16);

	pxProtectingKeyTable[0].length = 16;
	pxProtectingKeyTable[0].pData  =(unsigned char *) CKeyData1;

	pxProtectingKeyTable[1].length = 16;
	pxProtectingKeyTable[1].pData  = (unsigned char *)CKeyData2;

    if ( csdAllocRam2RamKey(pRam2RamChannelID, g_ulDmaHandle, CSD_CRYPTO_MODE_TDES_ECB, 1) != DRV_OK )
    {
		PrintError("Error:alloc Ram2Ram key error!\n");
		return DRV_ERR;
    }

	if(csdSetRam2RamKey(pRam2RamChannelID,
						2,
						pxProtectingKeyTable,
						CSD_CRYPTO_MODE_TDES_ECB) != DRV_OK)
	{
		PrintError("Error:set Ram2Ram key error!\n");
		return DRV_ERR;
	}

	if(DRV_CSDGetChipID((HUINT32*)&pxNuid[0]) != DRV_OK)
	{
		PrintError("Error: get NUID error!\n");
        csdFreeRam2RamKey(pRam2RamChannelID);
		return DRV_ERR;
	}

	for (i = (16 - CSD_NUID_LENGTH / 8); i< 16; i++)
	{
		pre_nuidData[i] = pxNuid[i-(16-CSD_NUID_LENGTH/8)];  /* [0], [1], ..., [CSD_NUID_LENGTH/8 -1] */
	}

	for(i=0; i<16; i=i+4)
	{
		nuidData[i] = pre_nuidData[i+3];
		nuidData[i+1] = pre_nuidData[i+2];
		nuidData[i+2] = pre_nuidData[i+1];
		nuidData[i+3] = pre_nuidData[i];
	}

	PrintDebug("[%s]_%d NUID Input = ",__func__,__LINE__);
	for(i=0; i<16; i++)
	{
		if(i==15)
		{
			PrintDebug("%02X\n", nuidData[i]);
		}
		else
		{
			PrintDebug("%02X ", nuidData[i]);
		}

	}

	pxSrc.length = 16;
	pxSrc.pData = nuidData;
	pxDst.length = 16;
	pxDst.pData = crcinput;

	PrintDebug(" to invoke DMA %d, pBufSrc = %08X src[0]=%02X src[15]=%02X,  pBufDest=%08X--- \n",  __LINE__,
							(unsigned int) pxSrc.pData, pxSrc.pData[0], pxSrc.pData[15],(unsigned int) pxDst.pData );

	if(csdEncryptData(pRam2RamChannelID, &pxSrc, &pxDst, CSD_CRYPTO_MODE_TDES_ECB) != DRV_OK)
	{
		PrintError("Error on encrypt data!\n");
        csdFreeRam2RamKey(pRam2RamChannelID);

        NEXUS_Memory_Free(nuidData);
        NEXUS_Memory_Free(pre_nuidData);
        NEXUS_Memory_Free(crcinput);
        *pxCheckNumber = 0x0;
        return DRV_ERR;
	}

	PrintDebug("[%s]_%d CRC Input = ",__func__,__LINE__);
	for(i=0; i<16; i++)
	{
		if(i==15)
		{
			PrintDebug("%02X\n", pxDst.pData[i]);
		}
		else
		{
			PrintDebug("%02X ", pxDst.pData[i]);
		}
	}

	genCrc32Tab() ;
	*pxCheckNumber = cmpCrc32(&crcinput[0],16) ;

    csdFreeRam2RamKey(pRam2RamChannelID);

	NEXUS_Memory_Free(nuidData);
	NEXUS_Memory_Free(pre_nuidData);
	NEXUS_Memory_Free(crcinput);

	PrintDebug("Check number is 0x%x \n", *pxCheckNumber);

	PrintExit();

	return DRV_OK;
#endif
}

DRV_Error DRV_CSDGetCSCCheckNumber(HUINT32 *pxCheckNumber, unsigned char *pCSCData)
{
#if defined(CONFIG_CAS_NA)
	TCsdStatus			CsdStatus = CSD_NO_ERROR;
	TCsd4BytesVector 		CscCheckNumber = {0, };

	CsdStatus=csdGetCSCDCheckNumber(pCSCData, CscCheckNumber);
	if(CsdStatus != CSD_NO_ERROR)
	{
		PrintError(" Get CSC Check Number Error !!\n");
		return DRV_ERR;
	}

	*pxCheckNumber=(CscCheckNumber[0] << 24) | (CscCheckNumber[1] << 16 ) | (CscCheckNumber[2] << 8) | CscCheckNumber[3];

	return DRV_OK;

#else
	/*TCsdDerivationLevelNumber   xDerivationLevelNumber= 2; hardcode to 2 in the function call*/
	CSDKey 				pxProtectingKeyTable[2] ;

#if (BCHP_CHIP==7403)
	unsigned char 	CKeyData1[] = {0X2D, 0xAF, 0x41, 0xAC, 0x1D, 0x3C, 0x68, 0xA4,
										   0x91, 0xCF, 0x5F, 0x30, 0x4E, 0x94, 0x0C, 0xC0
										  };
	unsigned char 	CKeyData2[] = {0X2D, 0xAF, 0x41, 0xAC, 0x1D, 0x3C, 0x68, 0xA4,
										   0x91, 0xCF, 0x5F, 0x30, 0x4E, 0x94, 0x0C, 0xC0
										  };
#elif  (BCHP_CHIP==7405)
	unsigned char 	CKeyData1[] = {0x2d, 0xaf, 0x41, 0xac, 0x1d, 0x3c, 0x68, 0xa4,
										   0x0e, 0x8c, 0xc8, 0xdd, 0x78, 0xcd, 0x9c, 0x23
										  };
	unsigned char 	CKeyData2[] = {0x2d, 0xaf, 0x41, 0xac, 0x1d, 0x3c, 0x68, 0xa4,
										   0x0e, 0x8c, 0xc8, 0xdd, 0x78, 0xcd, 0x9c, 0x23
										  };
#elif (BCHP_CHIP==7401)
	unsigned char 	CKeyData1[] = {0X23, 0x4e, 0x61, 0x67, 0x72, 0x61, 0x4b, 0x75,
										   0x64, 0x65, 0x6c, 0x73, 0x6b, 0x69, 0x23, 0x23
										  };

	unsigned char 	CKeyData2[] = {0X23, 0x4e, 0x61, 0x67, 0x72, 0x61, 0x4b, 0x75,
										   0x64, 0x65, 0x6c, 0x73, 0x6b, 0x69, 0x23, 0x23
										  };
#elif  (BCHP_CHIP == 7325)
	unsigned char 	CKeyData1[] = {0x2d, 0xaf, 0x41, 0xac, 0x1d, 0x3c, 0x68, 0xa4,
										   0x2c, 0x4b, 0x28, 0x32, 0xee, 0x01, 0xbc, 0x6d
										  };
	unsigned char 	CKeyData2[] = {0x2d, 0xaf, 0x41, 0xac, 0x1d, 0x3c, 0x68, 0xa4,
										   0x2c, 0x4b, 0x28, 0x32, 0xee, 0x01, 0xbc, 0x6d
										  };
#elif  (BCHP_CHIP == 7335)
	unsigned char 	CKeyData1[] = {0x2d, 0xaf, 0x41, 0xac, 0x1d, 0x3c, 0x68, 0xa4,
										   0xd8, 0xe8, 0xe0, 0xb0, 0xa5, 0x36, 0xc6, 0x4a
										  };
	unsigned char 	CKeyData2[] = {0x2d, 0xaf, 0x41, 0xac, 0x1d, 0x3c, 0x68, 0xa4,
										   0xd8, 0xe8, 0xe0, 0xb0, 0xa5, 0x36, 0xc6, 0x4a
										  };
#elif (BCHP_CHIP == 7346)
	/* Temporal key data, should be updated after getting key data from Broadcom */
	unsigned char 	CKeyData1[] = {0x2d, 0xaf, 0x41, 0xac, 0x1d, 0x3c, 0x68, 0xa4,
										   0xd8, 0xe8, 0xe0, 0xb0, 0xa5, 0x36, 0xc6, 0x4a
										  };
	unsigned char 	CKeyData2[] = {0x2d, 0xaf, 0x41, 0xac, 0x1d, 0x3c, 0x68, 0xa4,
										   0xd8, 0xe8, 0xe0, 0xb0, 0xa5, 0x36, 0xc6, 0x4a
										  };
#elif (BCHP_CHIP == 7422)
	/* Temporal key data, should be updated after getting key data from Broadcom */
	unsigned char 	CKeyData1[] = {0x2d, 0xaf, 0x41, 0xac, 0x1d, 0x3c, 0x68, 0xa4,
										   0xd8, 0xe8, 0xe0, 0xb0, 0xa5, 0x36, 0xc6, 0x4a
										  };
	unsigned char 	CKeyData2[] = {0x2d, 0xaf, 0x41, 0xac, 0x1d, 0x3c, 0x68, 0xa4,
										   0xd8, 0xe8, 0xe0, 0xb0, 0xa5, 0x36, 0xc6, 0x4a
										  };
#elif (BCHP_CHIP == 7231)
	/* Temporal key data, should be updated after getting key data from Broadcom */
	unsigned char 	CKeyData1[] = {0x2d, 0xaf, 0x41, 0xac, 0x1d, 0x3c, 0x68, 0xa4,
										   0xd8, 0xe8, 0xe0, 0xb0, 0xa5, 0x36, 0xc6, 0x4a
										  };
	unsigned char 	CKeyData2[] = {0x2d, 0xaf, 0x41, 0xac, 0x1d, 0x3c, 0x68, 0xa4,
										   0xd8, 0xe8, 0xe0, 0xb0, 0xa5, 0x36, 0xc6, 0x4a
										  };
#elif (BCHP_CHIP == 7358)
	unsigned char	CKeyData1[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
										0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
										  };
	unsigned char	CKeyData2[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
										0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
										  };
#elif (BCHP_CHIP == 7425)
	/* Temporal key data, should be updated after getting key data from Broadcom */
	unsigned char	CKeyData1[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
										0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
										  };
	unsigned char	CKeyData2[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
										0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
										  };
#elif (BCHP_CHIP == 7435)
	/* Temporal key data, should be updated after getting key data from Broadcom */
	unsigned char	CKeyData1[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
										0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
										  };
	unsigned char	CKeyData2[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
										0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
										  };
#elif (BCHP_CHIP == 7344)
		unsigned char	CKeyData1[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
											0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
											  };
		unsigned char	CKeyData2[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
											0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
											  };
#elif (BCHP_CHIP == 7241)
	/* Temporal key data, should be updated after getting key data from Broadcom */
	unsigned char 	CKeyData1[] = {0x2d, 0xaf, 0x41, 0xac, 0x1d, 0x3c, 0x68, 0xa4,
										   0xd8, 0xe8, 0xe0, 0xb0, 0xa5, 0x36, 0xc6, 0x4a
										  };
	unsigned char 	CKeyData2[] = {0x2d, 0xaf, 0x41, 0xac, 0x1d, 0x3c, 0x68, 0xa4,
										   0xd8, 0xe8, 0xe0, 0xb0, 0xa5, 0x36, 0xc6, 0x4a
										  };
#elif (BCHP_CHIP == 7429)
	/* Temporal key data, should be updated after getting key data from Broadcom */
	unsigned char 	CKeyData1[] = {0x2d, 0xaf, 0x41, 0xac, 0x1d, 0x3c, 0x68, 0xa4,
										   0xd8, 0xe8, 0xe0, 0xb0, 0xa5, 0x36, 0xc6, 0x4a
										  };
	unsigned char 	CKeyData2[] = {0x2d, 0xaf, 0x41, 0xac, 0x1d, 0x3c, 0x68, 0xa4,
										   0xd8, 0xe8, 0xe0, 0xb0, 0xa5, 0x36, 0xc6, 0x4a
										  };
#elif (BCHP_CHIP == 7445)
	/* Temporal key data, should be updated after getting key data from Broadcom */
	unsigned char	CKeyData1[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
										0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
										  };
	unsigned char	CKeyData2[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
										0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
										  };
#elif (BCHP_CHIP == 7362)
		unsigned char	CKeyData1[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
											0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
											  };
		unsigned char	CKeyData2[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
											0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
											  };
#elif (BCHP_CHIP == 7563)
				unsigned char	CKeyData1[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
													0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
													  };
				unsigned char	CKeyData2[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
													0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
													  };
#elif (BCHP_CHIP == 7584)
	/* Temporal key data, should be updated after getting key data from Broadcom */
	unsigned char	CKeyData1[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
										0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
										  };
	unsigned char	CKeyData2[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
										0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
										  };
#elif (BCHP_CHIP == 7552 )
	unsigned char	CKeyData1[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
										0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
										  };
	unsigned char	CKeyData2[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
										0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
										  };
#elif (BCHP_CHIP == 7250)
	/* Temporal key data, should be updated after getting key data from Broadcom */
	unsigned char	CKeyData1[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
										0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
										  };
	unsigned char	CKeyData2[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
										0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
										  };
#elif (BCHP_CHIP == 7439 ) || (BCHP_CHIP == 7252)
	/* Temporal key data, should be updated after getting key data from Broadcom */
	unsigned char	CKeyData1[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
										0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
										  };
	unsigned char	CKeyData2[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
										0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
										  };
#elif (BCHP_CHIP == 73625 )
		/* Temporal key data, should be updated after getting key data from Broadcom */
		unsigned char	CKeyData1[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
											0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
											  };
		unsigned char	CKeyData2[] = {0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
											0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
											  };

#else
#error "CHIP not supported"
#endif

	CSDDataBlock			pxSrc;
	CSDDataBlock			pxDst;
	unsigned char			*cscdata = NULL;
	unsigned char			*crcinput = NULL;
//	unsigned int			i;

	CSDRam2RamChannelID 	ram2ramChId;
	CSDRam2RamChannelID		*pRam2RamChannelID = &ram2ramChId;

	/* DMA src/dest buffers must be from Heap and alligned. Pls see the usage of Heap, Size, Alignment, Boundry
		the size of alligned buffer has no interface to change it, now hard coded to 16 bytes !!
	*/

	PrintEnter();

	NEXUS_Memory_Allocate(16, NULL, (void *)&cscdata);
	NEXUS_Memory_Allocate(16, NULL, (void *)&crcinput);

	VK_memset((void *)cscdata, 0x00, 16);
	VK_memset((void *)crcinput, 0x00, 16);

	pxProtectingKeyTable[0].length = 16;
	pxProtectingKeyTable[0].pData  =(unsigned char *) CKeyData1;

	pxProtectingKeyTable[1].length = 16;
	pxProtectingKeyTable[1].pData  = (unsigned char *)CKeyData2;

    if ( csdAllocRam2RamKey(pRam2RamChannelID, g_ulDmaHandle, CSD_CRYPTO_MODE_TDES_ECB, 1) != DRV_OK )
    {
		PrintError("Error:alloc Ram2Ram key error!\n");
		return DRV_ERR;
    }
	if(csdSetRam2RamKey(pRam2RamChannelID,
						2,
						pxProtectingKeyTable,
						CSD_CRYPTO_MODE_TDES_ECB) != DRV_OK)
	{
		PrintError("Error:set Ram2Ram key error!\n");
        csdFreeRam2RamKey(pRam2RamChannelID);
		return DRV_ERR;
	}

    VK_MEM_Memcpy(cscdata, pCSCData, 16);

	pxSrc.length = 16;
	pxSrc.pData = cscdata;
	pxDst.length = 16;
	pxDst.pData = crcinput;

	PrintDebug(" to invoke DMA %d, pBufSrc = %08X src[0]=%02X src[15]=%02X,  pBufDest=%08X--- \n",  __LINE__,
							(unsigned int) pxSrc.pData, pxSrc.pData[0], pxSrc.pData[15],(unsigned int) pxDst.pData );

	if(csdEncryptData(pRam2RamChannelID, &pxSrc, &pxDst, CSD_CRYPTO_MODE_TDES_ECB) != DRV_OK)
	{
		PrintError("Error on encrypt data!\n");
        csdFreeRam2RamKey(pRam2RamChannelID);
    	NEXUS_Memory_Free(cscdata);
	    NEXUS_Memory_Free(crcinput);
        *pxCheckNumber = 0;
        return DRV_ERR;
	}

    genCrc32Tab() ;
	*pxCheckNumber = cmpCrc32(&crcinput[0],16) ;

    csdFreeRam2RamKey(pRam2RamChannelID);
	NEXUS_Memory_Free(cscdata);
	NEXUS_Memory_Free(crcinput);

	PrintExit();

	return DRV_OK;
#endif
}

DRV_Error DRV_CSDGetEncryptionData(
	unsigned char *pInputData,
	unsigned char *pOutputData,
	unsigned int uiLength,
	unsigned int isEnryption)
{
	return P_CSDGetEncryptionData(pInputData, pOutputData, uiLength, isEnryption);
}

DRV_Error DRV_CSDGetChipRev(unsigned char *pulChipRev)
{
#if ((NEXUS_VERSION >= 1502) && (BCHP_CHIP == 7241))
	*pulChipRev = 0;
#else
	HUINT32 productId = 0;

	PrintEnter();

	if (pulChipRev == NULL)
	{
		PrintError("pulChipRev is NULL\n");

		return DRV_ERR;
	}

    NEXUS_Platform_ReadRegister(BCHP_SUN_TOP_CTRL_PRODUCT_ID, &productId);

	PrintDebug("[DRV_CSDGetChipRev] productId(0x%x)\n", productId);

	pulChipRev[0] = (productId & 0xF0) >> 4;
	pulChipRev[1] = (productId & 0x0F);

	PrintExit();
#endif
	return DRV_OK;
}

DRV_Error DRV_CSDGetOtpDataSection(unsigned char ucIndex, unsigned char *pucDataSection)
{
	NEXUS_ReadDataSectIO		readDataSectIO;
	NEXUS_Error					errCode;
	unsigned int 				i;

	PrintEnter();

	if (ucIndex > NEXUS_OtpDataSection_eMax)
	{
		PrintError("ucIndex(%d) is too big\n", ucIndex);

		return DRV_ERR_INVALID_PARAMETER;
	}

	errCode = NEXUS_Security_ReadDataSect((NEXUS_OtpDataSection)ucIndex, &readDataSectIO);
	if (errCode != BERR_SUCCESS)
	{
		PrintError("NEXUS_Security_ReadDataSect error(0x%x), ucIndex(%d)\n", errCode, ucIndex);

		return DRV_ERR;
	}

	for (i = 0; i < readDataSectIO.dataSectSize; i++)
	{
		pucDataSection[i] = readDataSectIO.dataSectBuf[i];
	}

	PrintDebug("[DRV_CSDGetOtpDataSection] ucIndex(%d) dataSectSize(%d)\n", ucIndex, readDataSectIO.dataSectSize);

	PrintExit();

	return DRV_OK;
}

DRV_Error DRV_CSDSetOtpDataSection(unsigned char ucIndex,  unsigned char *pucDataSection, unsigned char *pucDataSectionCRC)
{
	NEXUS_ProgramDataSectIO		progDataSectIO;
	NEXUS_Error					errCode;
	int							i;
	DRV_Error	eRet;

	PrintEnter();

	if (ucIndex > NEXUS_OtpDataSection_eMax)
	{
		PrintError("ucIndex(%d) is too big\n", ucIndex);

		return DRV_ERR_INVALID_PARAMETER;
	}

	VK_MEM_Memset((void *)&progDataSectIO, 0x0, sizeof(NEXUS_ProgramDataSectIO));

	progDataSectIO.progDsEnum 	= (NEXUS_OtpDataSection)ucIndex;
	progDataSectIO.mode			= NEXUS_OTP_DATASECTIONPROG_MODE;
	for (i = 0; i < NEXUS_OTP_DATASECTION_LEN; i++)
	{
		progDataSectIO.dataSectData[i] = pucDataSection[i];
	}

        /* In case of DataSection1, Write with CRC */
	if(ucIndex == 0x1)
	{
		for (i = 0; i < NEXUS_OTP_DATASECTION_CRC_LEN; i++)
		{
			progDataSectIO.crc[i] = pucDataSectionCRC[i];
			PrintDebug("DS1-CRC(%d) : 0x%x\n", i, pucDataSectionCRC[i]);

		}
	}

	errCode = NEXUS_Security_ProgramDataSect(&progDataSectIO);
	if (errCode != BERR_SUCCESS)
	{
		PrintError("NEXUS_Security_ProgramDataSect error(0x%x), ucIndex(%d)\n", errCode, ucIndex);

		return DRV_ERR;
	}

	/* In case of DataSection1, Set the lock bit */
	if(ucIndex == 0x1)
	{
		eRet = P_CSDSetDataSection1LockBit();
		if (eRet != DRV_OK)
		{
			PrintError("P_CSDSetDataSection1LockBit() errCode: %x\n", eRet );

			return DRV_ERR;
		}
	}

	PrintExit();

	return DRV_OK;
}

DRV_Error DRV_CSDSetMarketID(HUINT32 ulMarketID)
{
	return P_CSDSetMarketID(ulMarketID);
}

DRV_Error DRV_CSDSetStbCaSn(HUINT8 *pucStbCaSn)
{
	return P_CSDSetStbCaSn(pucStbCaSn);
}

DRV_Error DRV_CSDGetStbCaSn(HUINT8 *pucStbCaSn)
{
	return P_CSDGetStbCaSn(pucStbCaSn);
}

DRV_Error DRV_CSDGetRamScrambling(HUINT8 *pucRamScr)
{
	return P_CSDGetRamScrambling(pucRamScr);
}

DRV_Error DRV_CSDSetRamScrambling(void)
{
	return P_CSDSetRamScrambling();
}

DRV_Error DRV_CSDSetEncryptFPK(void)
{
	return P_CSDSetEncryptFPK();
}

DRV_Error DRV_CSDGetStbCaSnByPK(HUINT8 *pucStbCaSn)
{
#if defined(CONFIG_CAS_NA)
	return P_CSDGetStbCaSnByPK(pucStbCaSn);
#else
	(void)pucStbCaSn;

	return DRV_OK;
#endif
}

DRV_Error DRV_CSDSetBootMode(DRV_CSD_BootMode eCsdBootMode)
{
	return P_CSDSetBootMode(eCsdBootMode);
}

DRV_Error DRV_CSDGetBootMode(DRV_CSD_BootMode *pCsdBootMode)
{
	return P_CSDGetBootMode(pCsdBootMode);
}

DRV_Error DRV_CSDSetSCSVersioning(HUINT16 usPvLength, HUINT8 *pucPvValue)
{
	return P_CSDSetSCSVersioing(usPvLength, pucPvValue);
}

DRV_Error DRV_CSDGetSCSVersioning(HUINT16 usPvLength, HUINT8 *pucPvValue)
{
	return P_CSDGetSCSVersioing(usPvLength, pucPvValue);
}

DRV_Error DRV_CSDSetFlashProtection(void)
{
	return P_CSDSetFlashProtection();
}

DRV_Error DRV_CSDGetFlashProtection(HUINT8 *pucFlag)
{
	return P_CSDGetFlashProtection(pucFlag);
}

#if defined(CONFIG_CAS_CX)
DRV_Error DRV_CSDSetOTPChipset(void)
{
	DRV_Error errCode = DRV_OK;
	HUINT32 ulIndex=0,ulMaxNum=0;

	ulMaxNum=DRV_CFG_GetChipsetOTPMaxNum();

	for(ulIndex=0; ulIndex < ulMaxNum; ulIndex++)
	{
		errCode = P_CSDSetOtpMSP((NEXUS_OtpCmdMsp)ChipsetOTP[ulIndex].ucIndex,ChipsetOTP[ulIndex].ucDataBitLen,ChipsetOTP[ulIndex].ulDataBitMask,ChipsetOTP[ulIndex].ulMspData);
		if(errCode != DRV_OK)
		{
			PrintError("[%s].[%d] %d is failed to set Chipset OTP!!\n",__func__,__LINE__,ChipsetOTP[ulIndex].ucIndex);
		}
	}
	return errCode;
}

DRV_Error DRV_CSDGetOTPChipset(HUINT8 *ucOTPChipset)
{
	DRV_Error errCode = DRV_OK, errCheck = DRV_OK;
	HUINT32 ulIndex=0,ulMaxNum=0,ulMspData=0;

	ulMaxNum=DRV_CFG_GetChipsetOTPMaxNum();

	for(ulIndex=0; ulIndex < ulMaxNum; ulIndex++)
	{
		errCode = P_CSDGetOtpMSP((NEXUS_OtpCmdMsp)ChipsetOTP[ulIndex].ucIndex,&ulMspData);
		if(errCode != DRV_OK)
		{
			PrintError("OtpCmd %d is failed to get Chipset OTP data!!\n",ChipsetOTP[ulIndex].ucIndex);
			break;
		}
		else
		{
			PrintError("OtpCmd %d is set=[0x%x] in-chip=[0x%x] OTP %s!!\n",ChipsetOTP[ulIndex].ucIndex,ChipsetOTP[ulIndex].ulMspData, ulMspData,(ChipsetOTP[ulIndex].ulMspData == ulMspData ? "Lock" : "Unlock"));
			if(ChipsetOTP[ulIndex].ulMspData != ulMspData)
			{
				errCheck = DRV_ERR;
			}
		}
	}

	if(errCheck == DRV_OK)
		*ucOTPChipset = TRUE;
	else
		*ucOTPChipset = FALSE;

	return errCode;
}


DRV_Error DRV_CSDSetHostBootCodeDescryptionSelect(HUINT32 ulMspData)
{
	DRV_Error errCode = DRV_OK;
	errCode = P_CSDSetOtpMSP(NEXUS_OtpCmdMsp_eHostBootCodeDecryptionSelect, 4, 0xF, ulMspData);
	if(errCode != DRV_OK)
	{
		PrintError("[%s,%d] NEXUS_OtpCmdMsp_eHostBootCodeDecryptionSelect is failed to set Chipset OTP!!\n",__func__, __LINE__);
		return DRV_ERR;
	}

	return DRV_OK;
}


DRV_Error DRV_CSDGetHostBootCodeDescryptionSelect(HUINT32 *pulMspData)
{
	DRV_Error errCode = DRV_OK;

	if (pulMspData == NULL)
	{
		PrintError("[%s,%d]Error!, pulMspData is NULL pointer!!\n",__func__, __LINE__);
		return DRV_ERR;
	}

	errCode = P_CSDGetOtpMSP((NEXUS_OtpCmdMsp)NEXUS_OtpCmdMsp_eHostBootCodeDecryptionSelect, pulMspData);
	if(errCode != DRV_OK)
	{
		PrintError("NEXUS_OtpCmdMsp_eHostBootCodeDecryptionSelect(%d) is failed to get Chipset OTP data!!\n", NEXUS_OtpCmdMsp_eHostBootCodeDecryptionSelect);
		return DRV_ERR;
	}

	PrintDebug("NEXUS_OtpCmdMsp_eHostBootCodeDecryptionSelect is set to [0x%x]!\n", *pulMspData);

	return DRV_OK;
}

DRV_Error DRV_CSDGetOTPField(HUINT32* pulOTPField1, HUINT32* pulOTPField2)
{
	DRV_Error errCode = DRV_OK;
	DRV_Error rtValue = DRV_OK;
	HUINT32 ulOTPField1 = 0;
	HUINT32 ulOTPField2 = 0;
	HUINT32 ulIndex=0,ulMaxNum=0,ulMspData=0;
	HUINT32 ulBitCount = 0;

	ulMaxNum = DRV_CFG_GetChipsetOTPMaxNum();
	ulBitCount = 0;

	if (ulMaxNum > 32)
	{
		for (ulIndex = 0; ulIndex < 32; ulIndex++)
		{
			errCode = P_CSDGetOtpMSP((NEXUS_OtpCmdMsp)ChipsetOTP[ulIndex].ucIndex,&ulMspData);
			if(errCode == DRV_OK)
			{
				if(ChipsetOTP[ulIndex].ulMspData == ulMspData)
				{
					ulOTPField1 |= (1 << ulBitCount);
				}
			}
			else
			{
				rtValue = DRV_ERR;
			}
			ulBitCount++;
		}

		ulBitCount = 0;
		for (ulIndex = 32; ulIndex < ulMaxNum; ulIndex++)
		{
			errCode = P_CSDGetOtpMSP((NEXUS_OtpCmdMsp)ChipsetOTP[ulIndex].ucIndex,&ulMspData);
			if(errCode == DRV_OK)
			{
				if(ChipsetOTP[ulIndex].ulMspData == ulMspData)
				{
					ulOTPField2 |= (1 << ulBitCount);
				}
			}
			else
			{
				rtValue = DRV_ERR;
			}
			ulBitCount++;
		}
	}
	else
	{
		for (ulIndex = 0; ulIndex < ulMaxNum; ulIndex++)
		{
			errCode = P_CSDGetOtpMSP((NEXUS_OtpCmdMsp)ChipsetOTP[ulIndex].ucIndex,&ulMspData);
			if(errCode == DRV_OK)
			{
				if(ChipsetOTP[ulIndex].ulMspData == ulMspData)
				{
					ulOTPField1 |= (1 << ulBitCount);
				}
			}
			else
			{
				rtValue = DRV_ERR;
			}
			ulBitCount++;
		}
	}

	*pulOTPField1 = ulOTPField1;
	*pulOTPField2 = ulOTPField2;

	return rtValue;
}

#else

DRV_Error DRV_CSDSetOTPChipset(void)
{
	PrintError("[%s,%d]Error, Conax chips only support this feature.\n\r", __FUNCTION__, __LINE__);
	return DRV_OK;
}


DRV_Error DRV_CSDGetOTPChipset(HUINT8 *ucOTPChipset)
{
	UNUSED_PARAM(ucOTPChipset);
	PrintError("[%s,%d]Error, Conax chips only support this feature.\n\r", __FUNCTION__, __LINE__);
	return DRV_OK;
}


DRV_Error DRV_CSDSetHostBootCodeDescryptionSelect(HUINT32 ulMspData)
{
	UNUSED_PARAM(ulMspData);
	PrintError("[%s,%d]Error, Conax chips only support this feature.\n\r", __FUNCTION__, __LINE__);
	return DRV_OK;
}


DRV_Error DRV_CSDGetHostBootCodeDescryptionSelect(HUINT32 *pulMspData)
{
	UNUSED_PARAM(pulMspData);
	PrintError("[%s,%d]Error, Conax chips only support this feature.\n\r", __FUNCTION__, __LINE__);
	return DRV_OK;
}

DRV_Error DRV_CSDGetOTPField(HUINT32* pulOTPField1, HUINT32* pulOTPField2)
{
	UNUSED_PARAM(pulOTPField1);
	UNUSED_PARAM(pulOTPField2);
	PrintError("[%s,%d]Error, Conax chips only support this feature.\n\r", __FUNCTION__, __LINE__);
	return DRV_OK;
}

#endif

#if defined(CONFIG_CAS_NA_NOCS_3X)
#if defined(CONFIG_CAS_NA_NOCS_SEC)
DRV_Error DRV_CSDGetChipSetExtension(HUINT8 *pvInfo)
{
	DRV_Error rst = DRV_OK;
	TSecStatus retCode = SEC_NO_ERROR;
	TChar* pxChipsetExtension;

	retCode = secGetChipsetExtension(&pxChipsetExtension);
	if(retCode != SEC_NO_ERROR)
	{
		PrintError("[%s](%d) Fail to secGetChipsetExtension!!\n", __func__, __LINE__);
		rst = DRV_ERR;
	}

	VK_memcpy(pvInfo, pxChipsetExtension, 3);

	return rst;
}

DRV_Error DRV_CSDGetChipSetCut(HUINT8 *pvInfo)
{
	DRV_Error rst = DRV_OK;
	TSecStatus retCode = SEC_NO_ERROR;
	TChar* pxChipsetRevision;

	retCode = secGetChipsetRevision(&pxChipsetRevision);
	if(retCode != SEC_NO_ERROR)
	{
		PrintError("[%s](%d) Fail to secGetChipsetRevision!!\n", __func__, __LINE__);
		rst = DRV_ERR;
	}

	VK_memcpy(pvInfo, pxChipsetRevision, 3);

	return rst;
}
#else
DRV_Error DRV_CSDGetChipSetExtension(HUINT8 *pvInfo)
{
	UNUSED_PARAM(pvInfo);
	return DRV_OK;
}

DRV_Error DRV_CSDGetChipSetCut(HUINT8 *pvInfo)
{
	UNUSED_PARAM(pvInfo);
	return DRV_OK;
}
#endif
#if defined(CONFIG_CAS_NA_NOCS_DPT)
DRV_Error DRV_CSDGetCertCheckNumber(HUINT8 *pCertCheckNumber)
{
	int retVal = -1;

	if(pCertCheckNumber == NULL)
	{
		return DRV_ERR;
	}

	retVal = dptGetCertCheckNumber(DPT_CERT_REPORT_CHECK_NUMBER, pCertCheckNumber);

	if(retVal != 0)
	{
		return DRV_ERR;
	}

	return DRV_OK;
}

DRV_Error DRV_CSDVerifyCscData(HUINT8 *pCSCData)
{
	int retVal = -1;

	retVal = dptVerifyCscData(pCSCData);

	if(retVal != 0)
	{
		return DRV_ERR;
	}

	return DRV_OK;
}
#else
DRV_Error DRV_CSDGetCertCheckNumber(HUINT8 *pCertCheckNumber)
{
	UNUSED_PARAM(pCertCheckNumber);
	return DRV_OK;
}
DRV_Error DRV_CSDVerifyCscData(HUINT8 *pCSCData)
{
	UNUSED_PARAM(pCSCData);
	return DRV_OK;
}
#endif
#else
DRV_Error DRV_CSDGetChipSetExtension(HUINT8 *pvInfo)
{
	UNUSED_PARAM(pvInfo);
	return DRV_OK;
}

DRV_Error DRV_CSDGetChipSetCut(HUINT8 *pvInfo)
{
	UNUSED_PARAM(pvInfo);
	return DRV_OK;
}

DRV_Error DRV_CSDGetCertCheckNumber(HUINT8 *pCertCheckNumber)
{
	UNUSED_PARAM(pCertCheckNumber);
	return DRV_OK;
}

DRV_Error DRV_CSDVerifyCscData(HUINT8 *pCSCData)
{
	UNUSED_PARAM(pCSCData);
	return DRV_OK;

}
#endif
/* End of drv_csd.c */
