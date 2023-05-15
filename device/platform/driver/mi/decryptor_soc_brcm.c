#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>

#include "common_crypto.h"
#include "nexus_platform.h"
#include "nexus_dma.h"
#include "nexus_security.h"
#include "nexus_base_mmap.h"

#include "bstd.h"                       /* brcm includes */
#include "bdbg.h"
#include "bkni.h"

#include "decryptor.h"

#if defined(CONFIG_DEBUG)
#define PrintError		BDBG_ERR
#define PrintDebug		BDBG_MSG
#define PrintInfo		BDBG_WRN
#define BDBG_ENTER_THIS()   ((void)0)	/* BDBG_WRN((">>> %s :: Enter",__FUNCTION__)) */
#define BDBG_LEAVE_THIS()   ((void)0)	/* BDBG_WRN(("<<< %s :: Leave",__FUNCTION__)) */
#else
#define PrintError(...)	((void)0)
#define PrintDebug(...) ((void)0)
#define PrintInfo(...)  ((void)0)
#define BDBG_ENTER_THIS()   ((void)0)
#define BDBG_LEAVE_THIS()   ((void)0)
#endif

BDBG_MODULE(decryptor_soc);

#define ENABLE_TIME_DEBUG 0 /* If ENABLE_TIME_DEBUG '1', this is print heavy LOG. '0' is no print */
#define ENABLE_HW_AES 1 /* If ENABLE_HW_AES '1', this is H/W Decrypt. '0' is OpenSSL Decrypt */
#define ENABLE_HEAVY_DEBUG 0 /* If ENABLE_HEAVY_DEBUG '1', this is print heavy LOG about KEY, IV. '0' is no print */

//#define NOVA_SEC_TEST_RE_DECRYPT
#define NOVA_SEC_DUMP_CONFIG 0
#define NOVA_SEC_DUMP_HANDLE 0
#define NOVA_SEC_DUMP_DATA 0

#define PRINT_COLOR_YELLOW	"\033[01;33m"
#define PRINT_COLOR_GREEN	"\033[01;32m"
#define PRINT_COLOR_OFF		"\033[00m"

#if ENABLE_TIME_DEBUG
#include <sys/time.h>
#include <unistd.h>
static struct timespec s_tspec0, s_tspec1;
#define TIME_START clock_gettime(CLOCK_REALTIME, &s_tspec0);
#define TIME_END clock_gettime(CLOCK_REALTIME, &s_tspec1); \
	PrintInfo("(%d) %us, %09unano\n", __LINE__, \
	(int)((((double)s_tspec1.tv_sec*1.0e9+s_tspec1.tv_nsec) - ((double)s_tspec0.tv_sec*1.0e9+s_tspec0.tv_nsec))/1.0e9),  \
	(unsigned int)((unsigned long long)(((double)s_tspec1.tv_sec*1.0e9+s_tspec1.tv_nsec) - ((double)s_tspec0.tv_sec*1.0e9+s_tspec0.tv_nsec))%(unsigned long long)1.0e9));
#else
#define TIME_START
#define TIME_END
#endif

#define UNUSED_PARAM(x)	(void)x

typedef struct
{
	CommonCryptoHandle cryptoHandle;
#if (NEXUS_VERSION >= 1304)
	NEXUS_DmaHandle hDma;
	NEXUS_DmaJobHandle hDmaJob;
#endif
	NEXUS_KeySlotHandle KeyHandle;

	DECRYPTOR_Type_e tDecType;

	/* for PlayReady DRM */
	unsigned char *pEkl;

	/* for CTR Mode Ex */
	unsigned char *tmp_iv;
	unsigned char *tmp_ecount_buf;

	BKNI_EventHandle	hDmaEvent;

	//DRM_DECRYPT_CONTEXT pDecryptContext;
} DECRYPTOR_Handle;

typedef struct
{
	NEXUS_DmaHandle		hDma;
	BKNI_EventHandle	hDmaEvent;
	//BKNI_MutexHandle	hDmaMutexLock;
} NOVA_SEC_DmaHandle;

static BKNI_MutexHandle	s_hMiDmaMutexLock = NULL;

static int _MI_DECRYPTOR_SEC_SessionInit(void);
static void _MI_DECRYPTOR_SEC_SessionUnInit(void);


static void P_MI_DMA_CompletedCallback(void *pParam, int iParam)
{
	UNUSED_PARAM(iParam);
	PrintDebug(("P_MI_DMA_CompletedCallback fired! %x", (unsigned long)pParam));
	BKNI_SetEvent(pParam);
}


int DI_MI_DECRYPTOR_SOC_Init(void)
{
	int iErr = 0;

	iErr = _MI_DECRYPTOR_SEC_SessionInit();
	if (0 != iErr)
	{
		PrintError(("[%s] _MI_DECRYPTOR_SEC_SessionInit failed", __FUNCTION__));
		return -1;
	}

	return 0;
}

int DI_MI_DECRYPTOR_SOC_Uninit(void)
{
	_MI_DECRYPTOR_SEC_SessionUnInit();
	return 0;
}


inline static int do_decrypt_dma(DECRYPTOR_Handle *hDec, unsigned char* data, unsigned int size, unsigned char *key, unsigned char *iv)
{
#if (NEXUS_VERSION >= 1304)
	NEXUS_DmaJobBlockSettings blockSettings;
	NEXUS_DmaJobSettings jobSettings;
	NEXUS_DmaJobStatus jobStatus;
	NEXUS_SecurityClearKey ClearKey;
	NEXUS_Error nResult;

#if ENABLE_HEAVY_DEBUG
	PrintDebug(("ENTER: %s",__FUNCTION__));
	PrintInfo(("hDec->tDecType : %d\n", hDec->tDecType));
#endif
	/* Key Setting */
	if(hDec->tDecType == DECRYPTOR_TYPE_AES_128_CTR)
	{
#if ENABLE_HEAVY_DEBUG
		PrintInfo(("key = %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
			key[0], key[1], key[2], key[3], key[4], key[5], key[6], key[7],
			key[8], key[9], key[10], key[11], key[12], key[13], key[14], key[15]));
		PrintInfo(("iv	= %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
			iv[0], iv[1], iv[2], iv[3], iv[4], iv[5], iv[6], iv[7],
			iv[8], iv[9], iv[10], iv[11], iv[12], iv[13], iv[14], iv[15]));
#endif
		/* load the key */
		NEXUS_Security_GetDefaultClearKey(&ClearKey);
		ClearKey.keySize = 16;
		ClearKey.keyEntryType = NEXUS_SecurityKeyType_eClear;
		ClearKey.keyIVType = NEXUS_SecurityKeyIVType_eNoIV;
		memcpy(ClearKey.keyData, key, ClearKey.keySize);

		nResult = NEXUS_Security_LoadClearKey(hDec->KeyHandle, &ClearKey);
		if (nResult != NEXUS_SUCCESS)
		{
			PrintError(("--NEXUS_Security_LoadClearKey failed\n"));
			return (-1);
		}

		/* load the iv */
		NEXUS_Security_GetDefaultClearKey(&ClearKey);
		ClearKey.keySize = 16;
		ClearKey.keyEntryType = NEXUS_SecurityKeyType_eClear;
		ClearKey.keyIVType = NEXUS_SecurityKeyIVType_eIV;
		memcpy(ClearKey.keyData, iv, ClearKey.keySize);

		nResult = NEXUS_Security_LoadClearKey(hDec->KeyHandle, &ClearKey);
		if (nResult != NEXUS_SUCCESS)
		{
			PrintError(("++NEXUS_Security_LoadClearKey failed\n"));
			return (-1);
		}
	}
	else if(hDec->tDecType == DECRYPTOR_TYPE_AES_128_CBC)
	{
#if ENABLE_HEAVY_DEBUG
		PrintInfo(("key = %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
			key[0], key[1], key[2], key[3], key[4], key[5], key[6], key[7],
			key[8], key[9], key[10], key[11], key[12], key[13], key[14], key[15]));
		PrintInfo(("iv	= %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
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
			PrintError(("--NEXUS_Security_LoadClearKey failed\n"));
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
			PrintError(("++NEXUS_Security_LoadClearKey failed\n"));
			return (-1);
		}
	}
	else
	{
		PrintError(("Do not support dec type [%x]", hDec->tDecType));
		return (-1);
	}

	NEXUS_DmaJob_GetSettings(hDec->hDmaJob, &jobSettings);

	jobSettings.numBlocks = 1;
	jobSettings.completionCallback.callback = P_MI_DMA_CompletedCallback;
	jobSettings.completionCallback.context = hDec->hDmaEvent;
	jobSettings.dataFormat = NEXUS_DmaDataFormat_eBlock;
	jobSettings.keySlot = hDec->KeyHandle;

	nResult = NEXUS_DmaJob_SetSettings(hDec->hDmaJob, &jobSettings);
	if (nResult != NEXUS_SUCCESS)
	{
		PrintError(("NEXUS_DmaJob_SetSettings failed\n"));
		return (-1);
	}
	NEXUS_DmaJob_GetDefaultBlockSettings (&blockSettings);
	blockSettings.pSrcAddr = data;
	blockSettings.pDestAddr = data;
	blockSettings.blockSize = size;
	blockSettings.resetCrypto = true;
	blockSettings.scatterGatherCryptoStart = true;
	blockSettings.scatterGatherCryptoEnd = true;
	blockSettings.cached = true;

#if ENABLE_HEAVY_DEBUG
	PrintInfo(("  ==> %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x [%d]",
		data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
		data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15], size));
#endif

	NEXUS_DmaJob_ProcessBlocks(hDec->hDmaJob, &blockSettings, 1);

	BKNI_WaitForEvent(hDec->hDmaEvent, 200);

#if ENABLE_HEAVY_DEBUG
	PrintInfo(("  <== %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x [%d]",
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

inline static int do_encrypt_dma(DECRYPTOR_Handle *hDec, unsigned char *in, unsigned char *out, unsigned int size, unsigned char *key, unsigned char *iv)
{
	NEXUS_DmaJobSettings  dmaJobSetting;
	NEXUS_DmaSettings dmaSettings;
	NEXUS_DmaJobBlockSettings blockSettings;
	NEXUS_DmaJobHandle hDmaJob;
	NEXUS_DmaJobStatus jobStatus;
	/*TODO: DMA Handle form Resource Map? */
	NEXUS_DmaHandle hDma = NULL;
	NEXUS_SecurityClearKey ClearKey;
	NEXUS_Error nResult;

	BKNI_EventHandle	hDmaEvent = NULL;

	UNUSED_PARAM(iv);

	/* Key Setting */
	NEXUS_Security_GetDefaultClearKey(&ClearKey);
	ClearKey.keySize = 16;
	ClearKey.keyEntryType = NEXUS_SecurityKeyType_eOdd;
	ClearKey.keyIVType = NEXUS_SecurityKeyIVType_eNoIV;
	memcpy(ClearKey.keyData, key, ClearKey.keySize);

	nResult = NEXUS_Security_LoadClearKey(hDec->KeyHandle, &ClearKey);
	if (nResult != NEXUS_SUCCESS)
	{
		PrintError(("NEXUS_Security_LoadClearKey failed\n"));
		return (-1);
	}

	memset(&dmaSettings, 0, sizeof(NEXUS_DmaSettings));
	hDma = NEXUS_Dma_Open(NEXUS_ANY_ID, &dmaSettings);

	dmaSettings.endianMode = NEXUS_DmaEndianMode_eLittle;
	dmaSettings.swapMode = NEXUS_DmaSwapMode_eNone;
	NEXUS_Dma_SetSettings (hDma, &dmaSettings);

	BKNI_CreateEvent(&hDmaEvent);

	NEXUS_DmaJob_GetDefaultSettings (&dmaJobSetting);
	dmaJobSetting.numBlocks = 1;
	dmaJobSetting.completionCallback.callback = P_MI_DMA_CompletedCallback;
	dmaJobSetting.completionCallback.context = hDmaEvent;
	dmaJobSetting.dataFormat = NEXUS_DmaDataFormat_eBlock;
	dmaJobSetting.keySlot = hDec->KeyHandle;
	if ( (hDmaJob = NEXUS_DmaJob_Create (hDma, &dmaJobSetting)) == NULL )
	{
		PrintError(("[%s,%d] NEXUS_DmaJob_Create failed \n",__FUNCTION__,__LINE__));
		return (-1);
	}

	NEXUS_DmaJob_GetDefaultBlockSettings (&blockSettings);
	blockSettings.pSrcAddr = in;
	blockSettings.pDestAddr = out;
	blockSettings.blockSize = size;
	blockSettings.resetCrypto = true;
	blockSettings.scatterGatherCryptoStart = true;
	blockSettings.scatterGatherCryptoEnd = true;
	blockSettings.cached = true;

#if (BCHP_CHIP == 7422 || BCHP_CHIP == 7425 || BCHP_CHIP == 7231 || \
		BCHP_CHIP == 7344 || BCHP_CHIP == 7346)
	NEXUS_DmaJob_ProcessBlocks(hDmaJob, &blockSettings, 1);

	BKNI_WaitForEvent(hDmaEvent, 200);

#else
	if ( NEXUS_DmaJob_SetBlockSettings (hDmaJob, 0, &blockSettings)!= BERR_SUCCESS )
		return false;

	if ( NEXUS_DmaJob_Start (hDmaJob)!= BERR_SUCCESS )
		return false;

	BKNI_WaitForEvent(hDmaEvent, 200);

#endif
	if(hDmaEvent != NULL)
	{
		BKNI_DestroyEvent(hDmaEvent);
	}
	NEXUS_DmaJob_Destroy (hDmaJob);
	NEXUS_Dma_Close(hDma);
	return 0;
}
static int do_crypt_loadUniqueKey(NEXUS_KeySlotHandle keyHandle)
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
		PrintError(("[%s] NEXUS_Security_GenerateSessionKey errCode: %x\n", __FUNCTION__, nexusError));
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
		PrintError(("[%s] NEXUS_Security_GenerateControlWord errCode: %x\n", __FUNCTION__, nexusError));
		return -1;
	}

	return 0;
}

static int do_crypt_dma_otpkey(
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
	BKNI_EventHandle	hDmaEvent = NULL;

	int errCode = 0;

	if ((dma == NULL) || (keyHandle == NULL))
	{
		PrintError(("[%s] Parameter error!, DMA or KEY handle are NULL\n", __FUNCTION__));
		return -1;
	}
	BKNI_CreateEvent(&hDmaEvent);

    NEXUS_DmaJob_GetDefaultSettings(&jobSettings);

    jobSettings.numBlocks                   = 1;
	jobSettings.keySlot                     = keyHandle;
	jobSettings.dataFormat                  = NEXUS_DmaDataFormat_eBlock;
    jobSettings.completionCallback.callback = P_MI_DMA_CompletedCallback;
	jobSettings.completionCallback.context 	= hDmaEvent;

    dmaJob = NEXUS_DmaJob_Create(dma, &jobSettings);
	if (dmaJob == NULL)
	{
		PrintError(("[%s] NEXUS_DmaJob_Create error\n", __FUNCTION__));
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

	BKNI_WaitForEvent(hDmaEvent, 200);

ErrorExit:

	if(hDmaEvent != NULL)
	{
		BKNI_DestroyEvent(hDmaEvent);
	}

	NEXUS_DmaJob_Destroy(dmaJob);
	return errCode;
}

inline static int do_crypt_otpkey(unsigned char *in, unsigned char *out, unsigned int size, bool isEncryption)
{
	NEXUS_DmaSettings dmaSettings;
	NEXUS_DmaHandle hDma = NULL;
	NEXUS_Error nResult;
	NEXUS_SecurityKeySlotSettings 	keySettings;
	NEXUS_KeySlotHandle				keyHandle = NULL;
	NEXUS_SecurityAlgorithmSettings AlgorithmSettings;
	NEXUS_SecurityClearKey 			key;
	int errCode = 0;

	if(in == NULL || out == NULL)
	{
		errCode -1;
		goto ErrorExit;
	}

	memset(&dmaSettings, 0, sizeof(NEXUS_DmaSettings));
	hDma = NEXUS_Dma_Open(NEXUS_ANY_ID, &dmaSettings);

	NEXUS_Security_GetDefaultKeySlotSettings(&keySettings);
	keySettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;
	keyHandle = NEXUS_Security_AllocateKeySlot(&keySettings);
	if (keyHandle == NULL)
	{
		PrintError(("[%s,%d] Error in NEXUS_Security_AllocateKeySlot()\n\r", __FUNCTION__, __LINE__));
		errCode -1;
		goto ErrorExit;
	}
	NEXUS_Security_GetDefaultAlgorithmSettings(&AlgorithmSettings);
	// AES CBC Mode
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
		PrintError(("[%s,%d] NEXUS_Security_ConfigAlgorithm() error\n", __FUNCTION__, __LINE__));
		errCode -1;
		goto ErrorExit;
	}

	if (do_crypt_loadUniqueKey(keyHandle) != 0)
	{
		NEXUS_Security_FreeKeySlot(keyHandle);
		PrintError(("[%s] CRYPT_LoadUniqueKey error\n", __FUNCTION__));
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
		PrintError(("[%s] IV loading failed, line %d\n", __FUNCTION__, __LINE__ ));
		errCode -1;
		goto ErrorExit;
	}
	if(do_crypt_dma_otpkey(in, out, size, keyHandle,hDma) != 0)
	{
		PrintError(("[%s] P_CRYPT_CopyDma failed!\n",__FUNCTION__));
		errCode -1;
		goto ErrorExit;
	}

ErrorExit:
	if(hDma != NULL) NEXUS_Dma_Close(hDma);
	if(keyHandle != NULL) NEXUS_Security_FreeKeySlot(keyHandle);

	return errCode;
}

inline static int do_decrypt(DECRYPTOR_Handle *hDec, unsigned char *data, unsigned int size, unsigned char *key, unsigned char *iv)
{
	uint32_t ekl_size = 0;
	//	uint8_t* data_in_nexus=NULL;
	NEXUS_Error nResult;
	NEXUS_DmaJobBlockSettings blks[2];
	CommonCryptoJobSettings jobSettings;

	/*************************
	 *	1. NULL Check
	 *************************/
	if(data == NULL)
	{
		PrintError(("Buffer is NULL!\n"));
		return -1;
	}
	if(hDec->tDecType == DECRYPTOR_TYPE_WMDrmPd)
	{
		if(size < 16)
		{
			return -1;
		}
		BKNI_Memcpy(hDec->pEkl, key, 80);
		ekl_size = 80;
	}
	else if(hDec->tDecType == DECRYPTOR_TYPE_AES_128_CTR)
	{
		BKNI_Memcpy(hDec->pEkl, key+8, 8); /* Copy 8bytes for Key reorder */
		BKNI_Memcpy(hDec->pEkl+8, key, 8); /* Copy 8bytes for Key reorder */
		//BKNI_Memset(&hDec->pEkl[16], 0, 8);
		BKNI_Memcpy(&hDec->pEkl[16], iv+8, 8);
		BKNI_Memcpy(&hDec->pEkl[24], iv, 8);
		ekl_size = 32;

		PrintDebug(("key = %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
				key[0], key[1], key[2], key[3], key[4], key[5], key[6], key[7],
				key[8], key[9], key[10], key[11], key[12], key[13], key[14], key[15]));
		PrintDebug(("iv  = %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
			iv[0], iv[1], iv[2], iv[3], iv[4], iv[5], iv[6], iv[7],
			iv[8], iv[9], iv[10], iv[11], iv[12], iv[13], iv[14], iv[15]));
	}
	else if(hDec->tDecType == DECRYPTOR_TYPE_AES_128_CBC)
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

	PrintDebug(("  ==> %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x [%d]",
		data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
		data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15], size));

	nResult = CommonCrypto_DmaXfer(hDec->cryptoHandle, &jobSettings, (NEXUS_DmaJobBlockSettings*)blks, 2);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError(("%s - CommonCrypto_DmaXfer failed rc %x\n", __FUNCTION__, nResult));
	}

	PrintDebug(("  <== %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x [%d]",
		data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
		data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15], size));
	return 0;
}

inline static int secR2RCryptoOperation(
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
	BKNI_EventHandle	hDmaEvent = NULL;

    PrintDebug(("Enter %s", __FUNCTION__));

#if(CFG_BIG_ENDIAN==1)
    size_t i;
#endif
	BKNI_CreateEvent(&hDmaEvent);

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
    dma = NEXUS_Dma_Open(NEXUS_ANY_ID, NULL);

    NEXUS_DmaJob_GetDefaultSettings(&jobSettings);
    jobSettings.numBlocks = 1;
    jobSettings.keySlot = keyHandle;
    jobSettings.dataFormat = NEXUS_DmaDataFormat_eBlock;
    jobSettings.completionCallback.callback = P_MI_DMA_CompletedCallback;
    jobSettings.completionCallback.context = hDmaEvent;

    dmaJob = NEXUS_DmaJob_Create(dma, &jobSettings);

    NEXUS_DmaJob_GetDefaultBlockSettings(&blockSettings);
    blockSettings.pSrcAddr = input_data;
    blockSettings.pDestAddr = output_data;
    blockSettings.blockSize = xDataSize;
    blockSettings.resetCrypto = true;
    blockSettings.scatterGatherCryptoStart = true;
    blockSettings.scatterGatherCryptoEnd = true;
    blockSettings.cached = true;

	PrintDebug(("PROCESS DECRYPTION : %s",__FUNCTION__));

	NEXUS_DmaJob_ProcessBlocks(dmaJob, &blockSettings, 1);

	BKNI_WaitForEvent(hDmaEvent, 200);

#if(CFG_BIG_ENDIAN==1)
    for(i=0; i<xDataSize; i+=4)
        byteSwap(&(output_data[i]));
#endif

    /* Copy the data to M2M output buffer. */
    BKNI_Memcpy((void *)data, (void *)output_data, xDataSize );

	PrintDebug(("DECRYPTION CLEAR : %s",__FUNCTION__));

    /* Done.  Close DMA. */
	if(hDmaEvent != NULL)
	{
	    BKNI_DestroyEvent(hDmaEvent);
	}

	NEXUS_DmaJob_Destroy(dmaJob);

    NEXUS_Dma_Close(dma);

    NEXUS_Memory_Free(output_data);
    NEXUS_Memory_Free(input_data);

    PrintDebug(("LEAVE: %s",__FUNCTION__));
    return retCode;
}

inline static int do_decrypt_sec(DECRYPTOR_Handle *hDec, unsigned char *data, unsigned int size, unsigned char *iv)
{
    NEXUS_SecurityClearKey key;

	PrintDebug(("ENTER: %s",__FUNCTION__));

    if(hDec == NULL ) {
        PrintError(("%s SEC_ERROR_BAD_PARAMETER(NO DEC HANDLE)", __FUNCTION__));
        return -1;
    }
	if(hDec->KeyHandle == NULL) {
        PrintError(("%s SEC_ERROR_BAD_PARAMETER(NO KEY HANDLE)", __FUNCTION__));
        return -1;
    }

	if(data == NULL ) {
        PrintError(("%s SEC_ERROR_BAD_PARAMETER Data", __FUNCTION__));
        return -1;
    }
	PrintDebug(("LOAD KEYS: %s",__FUNCTION__));

    /* load IV key if needed */
    if (iv != NULL)
    {
        const unsigned char *pxIv = iv;
        size_t xIvSize = 16;

        PrintDebug(("%s IV load needed ", __FUNCTION__));

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
            PrintError(("Load IV failed"));
            return -1;
        }
    }
	PrintDebug(("DECRYPTION : %s",__FUNCTION__));

    if(secR2RCryptoOperation(hDec->KeyHandle , data, size)) {
        PrintError(("%s secR2RCryptoOperation error", __FUNCTION__));
        return -1;
    }
    PrintDebug(("LEAVE: %s",__FUNCTION__));
    return 0;

}

int DI_MI_DECRYPTOR_SOC_SetupSecureHandle(DECRYPTOR_Context_t tDecCtx, void* keyHandle)
{
	DECRYPTOR_Handle *hDec = (DECRYPTOR_Handle *)tDecCtx;
	int ret = 0;

	if(hDec == NULL || keyHandle == NULL)
	{
		return -1;
	}
	if(hDec->tDecType != DECRYPTOR_TYPE_SECURE_KEYHANDLE)
	{
		return -1;
	}
	hDec->KeyHandle = keyHandle;
	return ret;
}

int DI_MI_DECRYPTOR_SOC_DoDecrypt(DECRYPTOR_Context_t tDecCtx, unsigned char *pData, unsigned int ulSize, unsigned char *key, unsigned char *iv)
{
	DECRYPTOR_Handle *hDec = (DECRYPTOR_Handle *)tDecCtx;
	int ret = 0;

	PrintDebug(("ENTER: %s",__FUNCTION__));
	if(hDec == NULL)
	{
		PrintError(("[%s,%d] hDec is NULL\n", __FUNCTION__, __LINE__));
		return -1;
	}

	PrintInfo(("hDec->tDecType : %d\n", hDec->tDecType));
	switch(hDec->tDecType)
	{
		case DECRYPTOR_TYPE_AES_128_CTR:
		case DECRYPTOR_TYPE_AES_128_CBC:
#if (NEXUS_VERSION >= 1304)
			TIME_START;
			ret = do_decrypt_dma(hDec, pData, ulSize, key, iv);
			TIME_END;
			break;
#endif
		case DECRYPTOR_TYPE_WMDrmPd:
#if 0
		case DECRYPTOR_TYPE_AES_128_ECB:
#endif
			TIME_START;
			ret = do_decrypt(hDec, pData, ulSize, key, iv);
			TIME_END;
			break;
		case DECRYPTOR_TYPE_SECURE_KEYHANDLE:
			ret = do_decrypt_sec(hDec, pData, ulSize, iv);
			break;
		default:
			break;
	}
	return ret;
}

int DI_MI_DECRYPTOR_SOC_OTPKEY_DoEncrypt(unsigned char *in, unsigned char *out, unsigned int size)
{
	return do_crypt_otpkey(in, out, size, true);

}
int DI_MI_DECRYPTOR_SOC_OTPKEY_DoDecrypt(unsigned char *in, unsigned char *out, unsigned int size)
{
	return do_crypt_otpkey(in, out, size, false);
}

/* Little increment counter (128-bit int) by 1 */
inline static void counter_128_inc(unsigned char *ecount_buf)
{
	unsigned int n = 16;
	unsigned char c;

	do {
		--n;
		c = ecount_buf[n];
		++c;
		ecount_buf[n] = c;
		if (c) return;
	} while (n);

	return;
}

int DI_MI_DECRYPTOR_SOC_DoDecrypt_ExCTR(DECRYPTOR_Context_t tDecCtx, unsigned char *pData, unsigned int ulSize, unsigned char *key, unsigned char *iv, unsigned char *ecount_buf, unsigned int *num)
{
#if 0
	DECRYPTOR_Handle *hDec = (DECRYPTOR_Handle *)tDecCtx;
	unsigned int n = 0;
	unsigned char *in = pData;
	unsigned char *out = pData;
	unsigned char dummy_iv[16] = {0,};

#if ENABLE_HW_AES
#else
#include <openssl/aes.h>
	AES_KEY aes_key;
	AES_set_encrypt_key(key, 128, &aes_key);
#endif

	if(hDec == NULL)
	{
		return -1;
	}
	if(hDec->tDecType != DECRYPTOR_TYPE_AES_128_CTR_EX)
	{
		PrintInfo("%s - Not Support TYPE %u\n", __FUNCTION__, hDec->tDecType);
		return -1;
	}

	//PrintInfo("Size: %u", ulSize);

	n = *num;

	memcpy(hDec->tmp_iv, iv, 16);
	memcpy(hDec->tmp_ecount_buf, ecount_buf, 16);

#if 1 /* high performance */
	while (n && ulSize)
	{
		*(out++) = *(in++) ^ hDec->tmp_ecount_buf[n];
		--ulSize;
		n = (n+1) % 16;
	}
	TIME_START;
	while (ulSize>=16)
	{
#if ENABLE_HW_AES /* if ENABLE_HW_AES is '1', H/W Decrypt */
		do_encrypt_dma(hDec, hDec->tmp_iv, hDec->tmp_ecount_buf, 16, key, dummy_iv);
#else /* if ENABLE_HW_AES is '0', use OpenSSL */
		AES_ecb_encrypt(hDec->tmp_iv, hDec->tmp_ecount_buf, &aes_key, 1);
#endif

		counter_128_inc(hDec->tmp_iv);
		for (; n<16; n+=sizeof(size_t))
		{
			*(size_t *)(out+n) = *(size_t *)(in+n) ^ *(size_t *)(hDec->tmp_ecount_buf+n);
		}
		ulSize -= 16;
		out += 16;
		in  += 16;
		n = 0;
	}

	if (ulSize)
	{
#if ENABLE_HW_AES /* if ENABLE_HW_AES is '1', H/W Decrypt */
		do_encrypt_dma(hDec, hDec->tmp_iv, hDec->tmp_ecount_buf, 16, key, dummy_iv);
#else /* if ENABLE_HW_AES is '0', use OpenSSL */
		AES_ecb_encrypt(hDec->tmp_iv, hDec->tmp_ecount_buf, &aes_key, 1);
#endif
		counter_128_inc(hDec->tmp_iv);
		while (ulSize--)
		{
			out[n] = in[n] ^ hDec->tmp_ecount_buf[n];
			++n;
		}
	}
	TIME_END;
#else	/* low performance */
	size_t l=0;
	while (l<ulSize) {
		if (n==0) {
			TIME_START;

#if ENABLE_HW_AES	/* H/W Decrypt */
			do_encrypt_dma(hDec, hDec->tmp_iv, hDec->tmp_ecount_buf, 16, key, dummy_iv);
#else	/* OpenSSL */
			//AES_ecb_encrypt(hDec->tmp_iv, hDec->tmp_ecount_buf, &aes_key, 1);
			memcpy(hDec->tmp_ecount_buf, hDec->tmp_iv, 16);
#endif
			//counter_128_inc(hDec->tmp_iv);
			TIME_END;
		}
		out[l] = in[l] ^ hDec->tmp_ecount_buf[n];
		++l;
		n = (n+1) % 16;
	}
#endif
	*num = n;

	memcpy(ecount_buf, hDec->tmp_ecount_buf, 16);
	memcpy(iv, hDec->tmp_iv, 16);

	return 0;
#else
	UNUSED_PARAM(tDecCtx);
	UNUSED_PARAM(pData);
	UNUSED_PARAM(ulSize);
	UNUSED_PARAM(key);
	UNUSED_PARAM(iv);
	UNUSED_PARAM(ecount_buf);
	UNUSED_PARAM(num);
	return 0;
#endif
}


#if 0
int DECRYPTOR_SOC_DecryptSSTR(unsigned int sample_no, MIO_t *data, unsigned int payload_len, PIFF_SE_BOX_t* se)
{
	int ret=0;
	MIO_t* sample;
	PIFF_SE_BOX_t* pSe;

	sample = (MIO_t*) data;
	pSe = (PIFF_SE_BOX_t*) se;

	ret = prdy_aesctr_do_sample_decrypt(sample_no, sample, payload_len, pSe);
	return ret;
}
#endif

DECRYPTOR_Context_t DI_MI_DECRYPTOR_SOC_Create(DECRYPTOR_Type_e tDecType)
{
	DECRYPTOR_Handle *hDec = NULL;
	NEXUS_Error nResult;
	CommonCryptoSettings cryptoSettings;
	NEXUS_SecurityKeySlotSettings keySlotSettings;
	NEXUS_SecurityAlgorithmSettings AlgConfig;
#if (NEXUS_VERSION >= 1304)
	NEXUS_DmaJobSettings  dmaJobSetting;
#endif

	PrintDebug(("ENTER: %s",__FUNCTION__));
	/* Malloc Handle */
	hDec = (DECRYPTOR_Handle*)malloc(sizeof(DECRYPTOR_Handle));
	if(hDec == NULL)
	{
		PrintError(("Malloc failed\n"));
		return NULL;
	}
	memset(hDec, 0x0, sizeof(DECRYPTOR_Handle));

	hDec->tDecType = tDecType;
	if(tDecType == DECRYPTOR_TYPE_SECURE_KEYHANDLE)
	{
		return (DECRYPTOR_Context_t *)hDec;
	}

	/* dma init */
	CommonCrypto_GetDefaultSettings(&cryptoSettings);
	hDec->cryptoHandle = CommonCrypto_Open(&cryptoSettings);

	/* Key Slot Allocate */
	(void)NEXUS_Security_GetDefaultKeySlotSettings(&keySlotSettings);
	keySlotSettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;

	hDec->KeyHandle = NEXUS_Security_AllocateKeySlot(&keySlotSettings);
	if(hDec->KeyHandle == NULL)
	{
		PrintError(("NEXUS_Security_AllocateKeySlot failed\n"));
		goto error;
	}

	(void)NEXUS_Security_GetDefaultAlgorithmSettings(&AlgConfig);
	NEXUS_Memory_Allocate(80, NULL, (void **)&(hDec->pEkl));
	/* Key Slot Init Data */
	switch(tDecType)
	{
		case DECRYPTOR_TYPE_WMDrmPd:
			/* wmdrm Data Init */
			/* wmdrm10 keyslot init */
			AlgConfig.algorithm			= NEXUS_SecurityAlgorithm_eWMDrmPd;
			AlgConfig.operation			= NEXUS_SecurityOperation_eDecrypt;
			AlgConfig.terminationMode 	= NEXUS_SecurityTerminationMode_eClear;
			AlgConfig.enableExtKey		= true;
			AlgConfig.enableExtIv 		= true;
			break;
		case DECRYPTOR_TYPE_AES_128_CTR:
			/*	playready keyslot(=AES_CTR) init */
#if (NEXUS_VERSION >= 1304)
			AlgConfig.keyDestEntryType 	= NEXUS_SecurityKeyType_eClear;
			AlgConfig.algorithm         = NEXUS_SecurityAlgorithm_eAes128;
			AlgConfig.algorithmVar		= NEXUS_SecurityAlgorithmVariant_eCounter;
			AlgConfig.terminationMode 	= NEXUS_SecurityTerminationMode_eClear;
			AlgConfig.solitarySelect	= NEXUS_SecuritySolitarySelect_eClear;
			AlgConfig.ivMode			= NEXUS_SecurityIVMode_eRegular;
			AlgConfig.operation         = NEXUS_SecurityOperation_eDecrypt;
			AlgConfig.aesCounterSize    = NEXUS_SecurityAesCounterSize_e128Bits;
			/* AlgConfig.aesCounterMode    = NEXUS_SecurityCounterMode_ePartialBlockInNextPacket; */
			AlgConfig.aesCounterMode    = NEXUS_SecurityCounterMode_eGenericAllBlocks; /* Youtube and SSTR Support */
#else
			AlgConfig.algorithm			= NEXUS_SecurityAlgorithm_eAes128;
			AlgConfig.algorithmVar		= NEXUS_SecurityAlgorithmVariant_eCounter;
			AlgConfig.operation			= NEXUS_SecurityOperation_eDecrypt;
			AlgConfig.terminationMode 	= NEXUS_SecurityTerminationMode_eClear;
			AlgConfig.aesCounterSize	= NEXUS_SecurityAesCounterSize_e64Bits;
			AlgConfig.enableExtKey		= true;
			AlgConfig.enableExtIv 		= true;
#endif
			break;
		case DECRYPTOR_TYPE_AES_128_CBC:
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
			//AlgConfig.terminationMode 	= NEXUS_SecurityTerminationMode_eClear;
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
		case DECRYPTOR_TYPE_RC4:
			/*
			   AlgConfig.algorithm			= NEXUS_SecurityAlgorithm_eRc4;
			   AlgConfig.operation			= NEXUS_SecurityOperation_eDecrypt;
			   AlgConfig.terminationMode 	= NEXUS_SecurityTerminationMode_eClear;
			   AlgConfig.enableExtKey		= true;
			   AlgConfig.enableExtIv 		= true;
			   */
		default :
			PrintError(("%s - Not Support TYPE %u\n", __FUNCTION__, hDec->tDecType));
	}

	nResult = NEXUS_Security_ConfigAlgorithm(hDec->KeyHandle, &AlgConfig);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError(("NEXUS_Security_ConfigAlgorithm failed!\n"));
		goto error;
	}
#if (NEXUS_VERSION >= 1304)
	/* Open DMA handle */
	hDec->hDma = NEXUS_Dma_Open(NEXUS_ANY_ID, NULL);
	if (hDec->hDma == NULL)
	{
		PrintError(("NEXUS_Dma_Open failed!\n"));
		goto error;
	}
	NEXUS_DmaJob_GetDefaultSettings (&dmaJobSetting);
	dmaJobSetting.numBlocks = 1;
	dmaJobSetting.dataFormat = NEXUS_DmaDataFormat_eBlock;
	dmaJobSetting.keySlot = hDec->KeyHandle;
	dmaJobSetting.completionCallback.callback = NULL;
	hDec->hDmaJob = NEXUS_DmaJob_Create(hDec->hDma, &dmaJobSetting);
	if (hDec->hDmaJob == NULL)
	{
		PrintError(("%s(%d) Err> NEXUS_DmaJob_Create failed \n",__FUNCTION__,__LINE__));
		goto error;
	}
	BKNI_CreateEvent(&hDec->hDmaEvent);
	if( NULL == hDec->hDmaEvent)
	{
		PrintError(("%s(%d) Err> BKNI_CreateEvent failed \n", __FUNCTION__, __LINE__));
		goto error;
	}
#endif
	return (DECRYPTOR_Context_t *)hDec;

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
	if(hDec->hDmaEvent != NULL)
	{
		BKNI_DestroyEvent(hDec->hDmaEvent);
		hDec->hDmaEvent = NULL;
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

int DI_MI_DECRYPTOR_SOC_Destroy(DECRYPTOR_Context_t tDecCtx)
{
	DECRYPTOR_Handle *hDec = (DECRYPTOR_Handle *)tDecCtx;

	PrintDebug(("ENTER: %s",__FUNCTION__));
	if(hDec == NULL)
	{
		return 0;
	}

	if(hDec->pEkl != NULL)
	{
		NEXUS_Memory_Free(hDec->pEkl);
	}
	if(hDec->tDecType != DECRYPTOR_TYPE_SECURE_KEYHANDLE)
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
	if(hDec->hDmaEvent != NULL)
	{
		BKNI_DestroyEvent(hDec->hDmaEvent);
		hDec->hDmaEvent = NULL;
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

void *DI_MI_DECRYPTOR_SOC_Malloc(unsigned int unBytes)
{
	char *pMemory=NULL;
	PrintDebug(("ENTER: %s",__FUNCTION__));
	if(NEXUS_Memory_Allocate((size_t)unBytes, NULL, (void**)&pMemory) != NEXUS_SUCCESS)
	{
		PrintError(("[%s,%d] NEXUS_Memory_Allocate failed!\n", __FUNCTION__, __LINE__));
		return NULL;
	}
	return (void*)pMemory;
}

void DI_MI_DECRYPTOR_SOC_Free(void *pMemory)
{
	PrintDebug(("ENTER: %s",__FUNCTION__));
	if(pMemory)
	{
		(void)NEXUS_Memory_Free(pMemory);
		pMemory = NULL;
	}
}

void DI_MI_DECRYPTOR_SOC_GetType(DECRYPTOR_Context_t tDecCtx, DECRYPTOR_Type_e *eDecType)
{

	DECRYPTOR_Handle *hDec = (DECRYPTOR_Handle *)tDecCtx;
	if(hDec == NULL)
	{
		return ;
	}

	*eDecType = hDec->tDecType;
}


#if NOVA_SEC_DUMP_DATA
static void _MI_SEC_printBuff(char *title, unsigned char *pBuf,unsigned long size)
{
	char notitle[] = "no_title:";

	if (!title)
	{
		title = notitle;
	}
	while (size)
	{
		if (size >= 16)
		{
			PrintInfo((PRINT_COLOR_YELLOW"%s: %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x"PRINT_COLOR_OFF, title,
						pBuf[0], pBuf[1], pBuf[2], pBuf[3], pBuf[4], pBuf[5], pBuf[6], pBuf[7],
						pBuf[8], pBuf[9], pBuf[10], pBuf[11], pBuf[12], pBuf[13], pBuf[14], pBuf[15]));
			size -= 16;
			pBuf += 16;
		}
		else
		{
			while (size)
			{
				PrintInfo(("%.2x", pBuf[0]));
				size -= 1;
				pBuf += 1;
			}
		}
	}
}
#endif

#if NOVA_SEC_DUMP_CONFIG
static void _MI_DumpCAAlgorithmConfig(NEXUS_SecurityAlgorithmSettings *algorithm)
{
	int i;

	PrintInfo((PRINT_COLOR_GREEN"%s NEXUS_Security_ConfigAlgorithm\n"PRINT_COLOR_OFF, __FUNCTION__));
	PrintInfo(("%s NEXUS_SecurityAlgorithm algorithm %d ", __FUNCTION__, algorithm->algorithm));
	PrintInfo(("%s NEXUS_SecurityAlgorithmVariant algorithmVar %d ", __FUNCTION__, algorithm->algorithmVar));
	PrintInfo(("%s NEXUS_SecurityTerminationMode terminationMode %d ", __FUNCTION__, algorithm->terminationMode));
	PrintInfo(("%s NEXUS_SecurityCounterMode     aesCounterMode %d ", __FUNCTION__, algorithm->aesCounterMode));
	PrintInfo(("%s NEXUS_SecurityAesCounterSize  aesCounterSize %d ", __FUNCTION__, algorithm->aesCounterSize));
	PrintInfo(("%s NEXUS_SecurityIVMode          ivMode %d ", __FUNCTION__, algorithm->ivMode));
	PrintInfo(("%s NEXUS_SecuritySolitarySelect  solitarySelect %d ", __FUNCTION__, algorithm->solitarySelect));
	PrintInfo(("%s uint32_t                      caVendorID 0x%x ", __FUNCTION__, algorithm->caVendorID));
	PrintInfo(("%s unsigned                      askmModuleID %d ", __FUNCTION__, algorithm->askmModuleID));
	PrintInfo(("%s NEXUS_SecurityOtpId           otpId %d ", __FUNCTION__, algorithm->otpId));
	PrintInfo(("%s NEXUS_SecurityKey2Select      key2Select %d ", __FUNCTION__, algorithm->key2Select));
	PrintInfo(("%s NEXUS_SecurityDvbScrambleLevel dvbScramLevel %d ", __FUNCTION__, algorithm->dvbScramLevel));
	PrintInfo(("%s NEXUS_SecurityKeyType          keyDestEntryType %d ", __FUNCTION__, algorithm->keyDestEntryType));

	/* This member is NOT required for CA */
	PrintInfo(("%s NEXUS_SecurityOperation operation %d ", __FUNCTION__, algorithm->operation));
	PrintInfo(("%s NEXUS_SecurityAlgorithmConfigDestination dest %d ", __FUNCTION__, algorithm->dest));
	PrintInfo(("%s NEXUS_SecurityKeySource keySource %d ", __FUNCTION__, algorithm->keySource));
	PrintInfo(("%s bool          bMulti2Config %d ", __FUNCTION__, algorithm->bMulti2Config));
	PrintInfo(("%s uint16_t      multi2KeySelect %d ", __FUNCTION__, algorithm->multi2KeySelect));
	PrintInfo(("%s uint8_t       multi2RoundCount %d ", __FUNCTION__, algorithm->multi2RoundCount));

	PrintInfo(("%s bool enableExtKey %d ", __FUNCTION__, algorithm->enableExtKey));
	PrintInfo(("%s bool enableExtIv %d ", __FUNCTION__, algorithm->enableExtIv));
	PrintInfo(("%s bool bEncryptBeforeRave %d ", __FUNCTION__, algorithm->bEncryptBeforeRave));
	PrintInfo(("%s bool bRestrictEnable %d ", __FUNCTION__, algorithm->bRestrictEnable));
	PrintInfo(("%s bool bGlobalEnable %d ", __FUNCTION__, algorithm->bGlobalEnable));

	/* The following control rewriting the SC bits on newer platforms. */
	for (i = 0; i<NEXUS_SecurityPacketType_eMax; i++)
	{
		PrintInfo(("%s bool modifyScValue[i] %d ", __FUNCTION__, algorithm->modifyScValue[i]));
		PrintInfo(("%s NEXUS_SecurityAlgorithmScPolarity scValue[i] %d ", __FUNCTION__, algorithm->scValue[i]));
	}

	/* The following are only required for CA/CP on 40nm, 28nm chips */

	PrintInfo(("%s uint8_t  keyOffset %d ", __FUNCTION__, algorithm->keyOffset));
	PrintInfo(("%s uint8_t  ivOffset %d ", __FUNCTION__, algorithm->ivOffset));
	PrintInfo(("%s uint8_t  mscLengthSelect %d ", __FUNCTION__, algorithm->mscLengthSelect));
	PrintInfo(("%s NEXUS_Security_CustomerMode customerType %d ", __FUNCTION__, algorithm->customerType));
	PrintInfo(("%s uint8_t  macRegSelect %d ", __FUNCTION__, algorithm->macRegSelect));
	PrintInfo(("%s bool     IrModEnable %d ", __FUNCTION__, algorithm->IrModEnable));
	PrintInfo(("%s uint8_t  dvbCsa2keyCtrl %d ", __FUNCTION__, algorithm->dvbCsa2keyCtrl));
	PrintInfo(("%s uint8_t  dvbCsa2ivCtrl %d ", __FUNCTION__, algorithm->dvbCsa2ivCtrl));
	PrintInfo(("%s bool     dvbCsa2modEnabled %d ", __FUNCTION__, algorithm->dvbCsa2modEnabled));
	PrintInfo(("%s uint8_t  dvbCsa3dvbcsaVar %d ", __FUNCTION__, algorithm->dvbCsa3dvbcsaVar));
	PrintInfo(("%s uint8_t  dvbCsa3permutation %d ", __FUNCTION__, algorithm->dvbCsa3permutation));
	PrintInfo(("%s bool     dvbCsa3modXRC %d ", __FUNCTION__, algorithm->dvbCsa3modXRC));
	PrintInfo(("%s bool     EsModEnable %d ", __FUNCTION__, algorithm->EsModEnable));
	/*
	 * FIXME TODO: Remove #if 0 and fixup compile, so values get displayed
	 */
#if 0
	PrintInfo(("%s uint32_t extIVPtr %d ", __FUNCTION__, algorithm->extIVPtr));
	PrintInfo(("%s uint32_t extKeyPtr %d ", __FUNCTION__, algorithm->extKeyPtr));
#endif

	/* The following group of settings is for Zeus 4.x platforms only */
	PrintInfo(("%s bool     RpipeFromRregion %d ", __FUNCTION__, algorithm->RpipeFromRregion));
	PrintInfo(("%s bool     RpipeFromGregion %d ", __FUNCTION__, algorithm->RpipeFromGregion));
	PrintInfo(("%s bool     RpipeToRregion %d ", __FUNCTION__, algorithm->RpipeToRregion));
	PrintInfo(("%s bool     RpipeToGregion %d ", __FUNCTION__, algorithm->RpipeToGregion));
	PrintInfo(("%s bool     GpipeFromRregion %d ", __FUNCTION__, algorithm->GpipeFromRregion));
	PrintInfo(("%s bool     GpipeFromGregion %d ", __FUNCTION__, algorithm->GpipeFromGregion));
	PrintInfo(("%s bool     GpipeToRregion %d ", __FUNCTION__, algorithm->GpipeToRregion));
	PrintInfo(("%s bool     GpipeToGregion %d ", __FUNCTION__, algorithm->GpipeToGregion));

	/* The following is for ASKM 40nm Zeus 2 only. This array corresponds to bGpipePackets2Rregion and bRpipePackets2Rregion. */
	for (i = 0; i<NEXUS_SecurityPacketType_eMax; i++)
	{
		PrintInfo(("%s bool bRoutePipeToRestrictedRegion[i] %d ", __FUNCTION__, algorithm->bRoutePipeToRestrictedRegion[i]));
	}

	/* The following are only required for CA and CP for pre-Zeus 4.x platforms */
	PrintInfo(("%s bool bScAtscMode %d ", __FUNCTION__, algorithm->bScAtscMode));
	PrintInfo(("%s bool bAtscModEnable %d ", __FUNCTION__, algorithm->bAtscModEnable));
	PrintInfo(("%s bool bGlobalDropPktEnable %d ", __FUNCTION__, algorithm->bGlobalDropPktEnable));
	PrintInfo(("%s bool bRestrictDropPktEnable %d ", __FUNCTION__, algorithm->bRestrictDropPktEnable));
	PrintInfo(("%s bool bGlobalRegionOverwrite %d ", __FUNCTION__, algorithm->bGlobalRegionOverwrite));
	PrintInfo(("%s bool bRestrictSourceDropPktEnable %d ", __FUNCTION__, algorithm->bRestrictSourceDropPktEnable));

	/* The following are for CA/CP on legacy (pre-ASKM) systems only */
	PrintInfo(("%s bool bScPolarityEnable %d ", __FUNCTION__, algorithm->bScPolarityEnable));
	PrintInfo(("%s bool bSynEnable %d ", __FUNCTION__, algorithm->bSynEnable));
	PrintInfo(("%s bool bCPDDisable %d ", __FUNCTION__, algorithm->bCPDDisable));
	PrintInfo(("%s bool bCPSDisable %d ", __FUNCTION__, algorithm->bCPSDisable));

	/* The following are only required for M2M - pre-Zeus 4.x platforms */
	PrintInfo(("%s bool mscBitSelect %d ", __FUNCTION__, algorithm->mscBitSelect));
	PrintInfo(("%s bool bDisallowGG %d ", __FUNCTION__, algorithm->bDisallowGG));
	PrintInfo(("%s bool bDisallowGR %d ", __FUNCTION__, algorithm->bDisallowGR));
	PrintInfo(("%s bool bDisallowRG %d ", __FUNCTION__, algorithm->bDisallowRG));
	PrintInfo(("%s bool bDisallowRR %d ", __FUNCTION__, algorithm->bDisallowRR));
	PrintInfo(("%s bool enableTimestamps %d ", __FUNCTION__, algorithm->enableTimestamps));

	/* The following are legacy settings and should generally not be used */
	PrintInfo(("%s bool testKey2Select %d ", __FUNCTION__, algorithm->testKey2Select));
}
#endif


static NEXUS_Error _MI_SEC_DoDMA(NOVA_SEC_DmaHandle *pNovaHandle, NEXUS_KeySlotHandle pKeyHandle, unsigned char *pucIn, unsigned char *pucOut, unsigned int ulSize)
{
	NEXUS_DmaJobSettings		jobSettings;
	NEXUS_DmaJobHandle			hDmaJob = NULL;
	NEXUS_DmaJobBlockSettings	blockSettings;
	NEXUS_Error					nErr = NEXUS_SUCCESS;
	const unsigned char			*pInPtr  = pucIn;
	unsigned char				*pOutPtr = pucOut;
	unsigned char				*pBounceBuffer = NULL;

	if ((NULL == pNovaHandle) || (NULL == pKeyHandle))
	{
		PrintError(("[%s] Invalid pNovaHandle or pKeyHandle", __FUNCTION__));
		return NEXUS_INVALID_PARAMETER;
	}

	if ((NULL == pucIn)||(NULL == pucOut))
	{
		PrintError(("[%s] Invalid pucIn or pucOut", __FUNCTION__));
		return NEXUS_INVALID_PARAMETER;
	}

	if (0 == ulSize)
	{
		PrintError(("[%s] Invalid buffer size", __FUNCTION__));
		return NEXUS_INVALID_PARAMETER;
	}

	PrintDebug((PRINT_COLOR_GREEN"[%s] pNovaHandle: %08X"PRINT_COLOR_OFF, __FUNCTION__, pNovaHandle));
	NEXUS_DmaJob_GetDefaultSettings(&jobSettings);
	jobSettings.numBlocks = 1;
	jobSettings.keySlot = pKeyHandle;
	jobSettings.dataFormat = NEXUS_DmaDataFormat_eMpeg;
	jobSettings.completionCallback.callback = P_MI_DMA_CompletedCallback;
	jobSettings.completionCallback.context	= pNovaHandle->hDmaEvent;
	//jobSettings.timestampType               = NEXUS_TransportTimestampType_eNone;
	hDmaJob = NEXUS_DmaJob_Create(pNovaHandle->hDma, &jobSettings);
	if (NULL == hDmaJob)
	{
		PrintError(("[%s] NEXUS_DmaJob_Create failed", __FUNCTION__));
		nErr = NEXUS_NOT_AVAILABLE;
		goto _RET_DONE;
	}

	if (!NEXUS_AddrToOffset((void *)pucIn))
	{
		nErr = NEXUS_Memory_Allocate(sizeof(unsigned char) * ulSize, NULL, (void **)&pBounceBuffer);
		if ((NEXUS_SUCCESS != nErr) || (NULL == pBounceBuffer))
		{
			PrintError(("[%s] NEXUS_Memory_Allocate failed", __FUNCTION__));
			nErr = NEXUS_OUT_OF_SYSTEM_MEMORY;
			goto _RET_DONE;
		}

		BKNI_Memcpy(pBounceBuffer, pucIn, sizeof(unsigned char) * ulSize);
		pInPtr = pBounceBuffer;
	}

	if (!NEXUS_AddrToOffset((void *)pucOut))
	{
		if (NULL == pBounceBuffer)
		{
			nErr = NEXUS_Memory_Allocate(sizeof(unsigned char) * ulSize, NULL, (void **)&pBounceBuffer);
			if ((NEXUS_SUCCESS != nErr) || (NULL == pBounceBuffer))
			{
				PrintError(("[%s] NEXUS_Memory_Allocate failed #2", __FUNCTION__));
				nErr = NEXUS_OUT_OF_SYSTEM_MEMORY;
				goto _RET_DONE;
			}
		}

		pOutPtr = pBounceBuffer;
	}

	/* DMA */
	NEXUS_DmaJob_GetDefaultBlockSettings(&blockSettings);
	blockSettings.pSrcAddr = pInPtr;
	blockSettings.pDestAddr = pOutPtr;
	blockSettings.blockSize = ulSize;
	blockSettings.resetCrypto = true;
	blockSettings.scatterGatherCryptoStart = true;
	blockSettings.scatterGatherCryptoEnd = true;
	/* set cached flag. Nexus DMA will cache flush before and after transfer */
	blockSettings.cached = true;

	nErr = NEXUS_DmaJob_ProcessBlocks(hDmaJob, &blockSettings, 1);
	if (nErr == NEXUS_DMA_QUEUED )
	{
		nErr = NEXUS_SUCCESS;
	}

	BKNI_WaitForEvent(pNovaHandle->hDmaEvent, 200);

	if (pOutPtr != pucOut)
	{
		PrintError(("[%s] If this log is printed out, please alloc the memory using 'DI_MI_DECRYPTOR_SOC_Malloc' function for in/out buffer.", __FUNCTION__));
		BKNI_Memcpy((void *)pucOut, (void *)pOutPtr, ulSize );
	}

_RET_DONE:
	if (NULL != hDmaJob)
		NEXUS_DmaJob_Destroy(hDmaJob);

	if (NULL != pBounceBuffer)
		NEXUS_Memory_Free(pBounceBuffer);


	return nErr;
}


#if defined(NOVA_SEC_TEST_RE_DECRYPT)
static unsigned char s_ucIV[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

static NEXUS_Error _MI_SEC_ReDecrypt(NOVA_SEC_DmaHandle *pNovaHandle, unsigned char *pucIn, unsigned char *pucOut, unsigned int ulBufferSize, unsigned char *pucKey, unsigned char ucKeySize)
{
	NEXUS_Error nErr = NEXUS_SUCCESS;
	NEXUS_SecurityKeySlotSettings keySettings;
	NEXUS_KeySlotHandle					hDecKeyHandle = NULL;
	NEXUS_SecurityAlgorithmSettings 	NexusConfig;
	NEXUS_SecurityClearKey key;
	unsigned char i = 0;

	keySettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;
	keySettings.keySlotSource = NEXUS_SecurityKeySource_eFirstRamAskm;
	keySettings.keySlotType = NEXUS_SecurityKeySlotType_eType2;
	keySettings.client = NEXUS_SecurityClientType_eHost;
#if NOVA_SEC_DUMP_CONFIG
	PrintInfo((PRINT_COLOR_GREEN"[%s] keySlotEngine: %d"PRINT_COLOR_OFF, __FUNCTION__, keySettings.keySlotEngine));
	PrintInfo((PRINT_COLOR_GREEN"[%s] keySlotSource: %d"PRINT_COLOR_OFF, __FUNCTION__, keySettings.keySlotSource));
	PrintInfo((PRINT_COLOR_GREEN"[%s] keySlotType: %d"PRINT_COLOR_OFF, __FUNCTION__, keySettings.keySlotType));
	PrintInfo((PRINT_COLOR_GREEN"[%s] client: %d"PRINT_COLOR_OFF, __FUNCTION__, keySettings.client));
#endif
	hDecKeyHandle = NEXUS_Security_AllocateKeySlot(&keySettings);
	if(NULL == hDecKeyHandle)
	{
		PrintError(("[%s] NEXUS_Security_AllocateKeySlot failed", __FUNCTION__));
		nErr = NEXUS_NOT_AVAILABLE;
		goto _RET_ERR;
	}

#if NOVA_SEC_DUMP_HANDLE
	PrintInfo((PRINT_COLOR_GREEN"[%s] hDecKeyHandle: %08X"PRINT_COLOR_OFF, __FUNCTION__, hDecKeyHandle));
#endif

	/* Set up key for clear key */
	NEXUS_Security_GetDefaultAlgorithmSettings(&NexusConfig);
	NexusConfig.algorithm 			= NEXUS_SecurityAlgorithm_eAes;
	NexusConfig.algorithmVar  		= NEXUS_SecurityAlgorithmVariant_eCbc;
	//NexusConfig.dest				= NEXUS_SecurityAlgorithmConfigDestination_eMem2Mem;
	NexusConfig.terminationMode     = NEXUS_SecurityTerminationMode_eBlock;
	NexusConfig.ivMode              = NEXUS_SecurityIVMode_eRegular;
	NexusConfig.solitarySelect      = NEXUS_SecuritySolitarySelect_eCbcXorIv;
	NexusConfig.caVendorID          = 0x1234;
	NexusConfig.askmModuleID        = NEXUS_SecurityAskmModuleID_eModuleID_4;
	NexusConfig.otpId               = NEXUS_SecurityOtpId_eOtpVal;
	NexusConfig.key2Select			= NEXUS_SecurityKey2Select_eRealKey;
	NexusConfig.operation 			= NEXUS_SecurityOperation_eDecrypt;
	NexusConfig.modifyScValue[NEXUS_SecurityPacketType_eRestricted] = true;
	NexusConfig.modifyScValue[NEXUS_SecurityPacketType_eGlobal]     = true;
	NexusConfig.scValue[NEXUS_SecurityPacketType_eRestricted]       = NEXUS_SecurityAlgorithmScPolarity_eClear;
	NexusConfig.scValue[NEXUS_SecurityPacketType_eGlobal]           = NEXUS_SecurityAlgorithmScPolarity_eClear;

	for (i = 0; i <= NEXUS_SecurityKeyType_eClear; i++)
	{
		NexusConfig.keyDestEntryType 	= (NEXUS_SecurityKeyType)i;
		nErr = NEXUS_Security_ConfigAlgorithm(hDecKeyHandle, &NexusConfig);
		if (NEXUS_SUCCESS != nErr)
		{
			PrintError(("[%s] NEXUS_Security_ConfigAlgorithm failed", __FUNCTION__));
			goto _RET_ERR;
		}
	}
#if NOVA_SEC_DUMP_CONFIG
	PrintInfo((PRINT_COLOR_GREEN"[%s] DUMP NexusConfig"PRINT_COLOR_OFF, __FUNCTION__));
	_MI_DumpCAAlgorithmConfig(&NexusConfig);
#endif
	NEXUS_Security_GetDefaultClearKey(&key);
	key.keyIVType = NEXUS_SecurityKeyIVType_eNoIV;
	key.keySize = ucKeySize;
	key.dest = NEXUS_SecurityAlgorithmConfigDestination_eMax;
	for (i = 0; i <= NEXUS_SecurityKeyType_eClear; i++)
	{
		key.keyEntryType = (NEXUS_SecurityKeyType)i;

		BKNI_Memcpy(key.keyData, pucKey, key.keySize);
#if NOVA_SEC_DUMP_CONFIG
		_MI_SEC_printBuff("DUMP Clear Key:", pucKey, key.keySize);
#endif
		if (NEXUS_Security_LoadClearKey(hDecKeyHandle, &key) != 0) {
			PrintError(("[%s] NEXUS_Security_LoadClearKey failed", __FUNCTION__));
			goto _RET_ERR;
		}
	}

	key.keyIVType	 = NEXUS_SecurityKeyIVType_eIV;
	key.keySize = sizeof(s_ucIV);
	key.dest = NEXUS_SecurityAlgorithmConfigDestination_eMax;
	for (i = 0; i <= NEXUS_SecurityKeyType_eClear; i++)
	{
		key.keyEntryType = (NEXUS_SecurityKeyType)i;
		BKNI_Memcpy(key.keyData, s_ucIV, key.keySize);
		if (NEXUS_Security_LoadClearKey(hDecKeyHandle, &key) != 0)
		{
			PrintError(("[%s] NEXUS_Security_LoadClearKey IV failed", __FUNCTION__));
			goto _RET_ERR;
		}
	}

	nErr = _MI_SEC_DoDMA(pNovaHandle, hDecKeyHandle, pucIn, pucOut, ulBufferSize);
	if (NEXUS_SUCCESS != nErr)
	{
		PrintError(("[%s] _MI_SEC_DoDMA() Error", __FUNCTION__));
		goto _RET_ERR;
	}

_RET_ERR:
	if (NULL != hDecKeyHandle)
		NEXUS_Security_FreeKeySlot(hDecKeyHandle);

	return nErr;
}
#endif


static int _MI_DECRYPTOR_SEC_SessionInit(void)
{
	BERR_Code	bErr = BERR_SUCCESS;

	BDBG_ENTER_THIS();

	/* create mi mutex lock */
	bErr = BKNI_CreateMutex(&s_hMiDmaMutexLock);
	if (BERR_SUCCESS != bErr)
	{
		PrintError(("[%s] BKNI_CreateMutex failed", __FUNCTION__));
		return -1;
	}

	PrintInfo((PRINT_COLOR_GREEN"[%s] s_hMiDmaMutexLock: %08X"PRINT_COLOR_OFF, __FUNCTION__, s_hMiDmaMutexLock));

	BDBG_LEAVE_THIS();
	return 0;
}


static void _MI_DECRYPTOR_SEC_SessionUnInit(void)
{
	BDBG_ENTER_THIS();

	if (NULL != s_hMiDmaMutexLock)
	{
		PrintInfo((PRINT_COLOR_GREEN"[%s] s_hMiDmaMutexLock: %08X"PRINT_COLOR_OFF, __FUNCTION__, s_hMiDmaMutexLock));
        BKNI_DestroyMutex(s_hMiDmaMutexLock);
	}

	BDBG_LEAVE_THIS();
}


static NEXUS_Error _MI_SEC_CloseNovaDmaHandle(NOVA_SEC_DmaHandle *pNovaHandle)
{
	if (NULL == pNovaHandle)
	{
		PrintError(("[%s] Invalid pNovaHandle.", __FUNCTION__));
		return NEXUS_NOT_AVAILABLE;
	}

#if NOVA_SEC_DUMP_HANDLE
	PrintInfo((PRINT_COLOR_GREEN"[%s] pNovaHandle: %08X"PRINT_COLOR_OFF, __FUNCTION__, pNovaHandle));
#endif
	if (NULL != pNovaHandle->hDma)
	{
		NEXUS_Dma_Close(pNovaHandle->hDma);
	}

	if (NULL != pNovaHandle->hDmaEvent)
	{
		BKNI_DestroyEvent(pNovaHandle->hDmaEvent);
	}

	BKNI_Free(pNovaHandle);
	return NEXUS_SUCCESS;
}


static NOVA_SEC_DmaHandle *_MI_SEC_OpenNovaDmaHandle(void)
{
	NEXUS_Error	nErr = NEXUS_SUCCESS;
	NOVA_SEC_DmaHandle	*pNovaHandle = NULL;

	BDBG_ENTER_THIS();

	pNovaHandle = (NOVA_SEC_DmaHandle *)BKNI_Malloc(sizeof(NOVA_SEC_DmaHandle));
	if (NULL == pNovaHandle)
	{
		PrintError(("[%s] BKNI_Malloc failed", __FUNCTION__));
		goto _RET_ERR;
	}

	BKNI_Memset((void *)pNovaHandle, (int)NULL, sizeof(NOVA_SEC_DmaHandle));

	pNovaHandle->hDma = NEXUS_Dma_Open(NEXUS_ANY_ID, NULL);
	if (NULL == pNovaHandle->hDma)
	{
		PrintError(("[%s] NEXUS_Dma_Open failed", __FUNCTION__));
		goto _RET_ERR;
	}

	BKNI_CreateEvent(&pNovaHandle->hDmaEvent);
	if (NULL == pNovaHandle->hDmaEvent)
	{
		PrintError(("[%s] BKNI_CreateEvent failed", __FUNCTION__));
		goto _RET_ERR;
	}

#if NOVA_SEC_DUMP_HANDLE
	PrintInfo((PRINT_COLOR_GREEN"[%s] pNovaHandle: %08X"PRINT_COLOR_OFF, __FUNCTION__, pNovaHandle));
	PrintInfo((PRINT_COLOR_YELLOW"[%s] pNovaHandle->hDma: %08X"PRINT_COLOR_OFF, __FUNCTION__, pNovaHandle->hDma));
	PrintInfo((PRINT_COLOR_YELLOW"[%s] pNovaHandle->hDmaEvent: %08X"PRINT_COLOR_OFF, __FUNCTION__, pNovaHandle->hDmaEvent));
#endif

	BDBG_LEAVE_THIS();
	return pNovaHandle;

_RET_ERR:
	nErr = _MI_SEC_CloseNovaDmaHandle(pNovaHandle);
	if (NEXUS_SUCCESS != nErr)
	{
		PrintError(("[%s] _MI_SEC_ClearNovaDmaHandle() Error", __FUNCTION__));
	}

	return (NOVA_SEC_DmaHandle *)NULL;
}


int DI_MI_DECRYPTOR_SEC_SessionEncrypt(unsigned int *pKeyHandle, unsigned char *pucInput, unsigned char *pucOutput, unsigned int ulSize, unsigned char *pKey)
{
	NEXUS_Error	nErr = NEXUS_SUCCESS;
	NOVA_SEC_DmaHandle	*pNovaHandle = NULL;

	BDBG_ENTER_THIS();

	BKNI_AcquireMutex(s_hMiDmaMutexLock);

	pNovaHandle = _MI_SEC_OpenNovaDmaHandle();
	if (NULL == pNovaHandle)
	{
		PrintError(("[%s] _MI_SEC_OpenNovaDmaHandle() Error", __FUNCTION__));
		goto _RET_ERR;
	}

#if NOVA_SEC_DUMP_DATA
	PrintInfo(("[pKeyHandle: %08X] ################################################", pKeyHandle));
	_MI_SEC_printBuff("Plain pucInput:", pucInput, 32);
#endif

	nErr = _MI_SEC_DoDMA(pNovaHandle, (NEXUS_KeySlotHandle)pKeyHandle, pucInput, pucOutput, ulSize);
	if (NEXUS_SUCCESS != nErr)
	{
		PrintError(("[%s] _MI_SEC_DoDMA() Error", __FUNCTION__));
		goto _RET_ERR;
	}

#if NOVA_SEC_DUMP_DATA
	_MI_SEC_printBuff("Encrypted pucOutput:", pucOutput, 32);
#endif

#if defined(NOVA_SEC_TEST_RE_DECRYPT)
	nErr = _MI_SEC_ReDecrypt(pNovaHandle, pucOutput, pucOutput, ulSize, pKey, 16);
	if (NEXUS_SUCCESS != nErr)
	{
		PrintError(("[%s] _MI_SEC_ReDecrypt() Error", __FUNCTION__));
		goto _RET_ERR;
	}
#if NOVA_SEC_DUMP_DATA
	_MI_SEC_printBuff("Decrypted pucOutput:", pBounceBuffer, 32);
#endif
#else
	UNUSED_PARAM(pKey);
#endif
	nErr = _MI_SEC_CloseNovaDmaHandle(pNovaHandle);
	if (NEXUS_SUCCESS != nErr)
	{
		PrintError(("[%s] _MI_SEC_CloseNovaDmaHandle() Error", __FUNCTION__));
	}

	BKNI_ReleaseMutex(s_hMiDmaMutexLock);
	BDBG_LEAVE_THIS();
	return 0;

_RET_ERR:
	nErr = _MI_SEC_CloseNovaDmaHandle(pNovaHandle);
	if (NEXUS_SUCCESS != nErr)
	{
		PrintError(("[%s] _MI_SEC_CloseNovaDmaHandle() Error", __FUNCTION__));
	}

	BKNI_ReleaseMutex(s_hMiDmaMutexLock);
	return -1;
}


