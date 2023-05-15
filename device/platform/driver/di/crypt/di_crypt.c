#if defined(CONFIG_VK_STDLIB)
#else
#include <stdio.h>
#include <string.h>
#endif

#include "htype.h"
#include "vkernel.h"

#include "di_err.h"
#include "di_uart.h"
#include "di_crypt.h"

#include "drv_err.h"
#include "drv_pvr.h"
#include "drv_crypt.h"

#include "nexus_platform.h"
#include "nexus_memory.h"

#include "nexus_security.h"
#include "nexus_security_datatypes.h"
#include "nexus_dma.h"
#include "nexus_keyladder.h"

#if defined(CONFIG_CAS_NA)
#include "drv_csd.h"
#if (NEXUS_VERSION >= 1501)
#include "csd_brcm_extensions.h"
#endif
#if defined(CONFIG_CAS_NA_NOCS_3X)
#include "nocs_csd_impl.h"
#include "nocs_csd.h"
#else
#include "nocs1x_csd_impl.h"
#include "nocs1x_csd.h"
#endif
#endif

#if defined(CONFIG_PVR_SECURITY_ENGINE_INLINE_CRYPTO)
	#if defined(CONFIG_VK_STDLIB)
	#else
		#include 	<stdlib.h>
	#endif
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include "nexus_recpump.h"
#include "nexus_pid_channel.h"
#include "di_video.h"
#include "drv_video.h"
#endif

#if defined(CONFIG_KEY_PROVISIONING)
#include "bsagelib_types.h"
#include "bsagelib_tools.h"

#include "sage_srai.h"
#if (NEXUS_VERSION >= 1501) || ((NEXUS_VERSION == 1401) && defined(CONFIG_CAS_NA_NOCS_3X))
#include "sage_manufacturing_api.h"
#else
#include "sage_provisioning_host.h"
#endif
#include "sage_manufacturing_module_ids.h"

#include "drm_metadata_tl.h"
#endif

#define DEBUG_MODULE TRACE_MODULE_DI_PVR

#if	defined(CONFIG_LOCAL_TIMESTAMP_ENABLED_RECORD)
#define STREAM_ALIGN_BYTE			192
#define STREAM_SYNC_BYTE			4
#define STREAM_TSC_BYTE				7
#else
#define STREAM_ALIGN_BYTE			188
#define STREAM_SYNC_BYTE            0
#define STREAM_TSC_BYTE             3
#endif

#if defined(CONFIG_DEBUG)
	//#define DI_CRYPT_DEBUG
	#ifdef DI_CRYPT_DEBUG
	#define PrintDebug(fmt, ...)	VK_DBG_Print (fmt, ## __VA_ARGS__)
	#define PrintError(fmt, ...)	VK_DBG_Print (fmt, ## __VA_ARGS__)
	#define PrintEnter()			DI_UART_Print("(+)%s\n", __FUNCTION__)
	#define PrintExit()				DI_UART_Print("(-)%s\n", __FUNCTION__)
	#define PrintInfo				VK_printf
	#else
	#define PrintDebug(fmt, ...)	((void)0)
	#define PrintError(fmt, ...)	VK_DBG_Print (fmt, ## __VA_ARGS__)
	#define PrintEnter()			((void)0)
	#define PrintExit()				((void)0)
	#define PrintInfo				VK_printf
	#endif
#else
	#define PrintDebug(...)		((void)0)
	#define PrintError(...)		VK_DBG_Print(__VA_ARGS__)
	#define PrintEnter(...)		((void)0)
	#define PrintExit(...)		((void)0)
	#define PrintInfo(...)		((void)0)
#endif

#if !defined(CONFIG_CAS_NA)
#if defined(CONFIG_DEBUG)
static int  P_CRYPT_Print(const char *format,...);
#else
#define P_CRYPT_Print(...)		((void)0)
#endif
#endif

#define UNUSED_PARAM(x)  (void )(x) //{ volatile void *x; x = (void *)&(x); }

#if defined(CONFIG_PVR_SECURITY_ENGINE_INLINE_CRYPTO)

#define ITB_BUFFER_CHUNK_SIZE   (6*4*16)
#if defined(CONFIG_DOUBLE_SIZE_RECORD_BUFFER)
#define ITB_BUFFER_CHUNK_COUNT  192  /*actually over than double*/
#else
#define ITB_BUFFER_CHUNK_COUNT  48
#endif

typedef struct
{
	NEXUS_PidChannelHandle  pidChannelHandle;
	NEXUS_KeySlotHandle     dscKeyHandle;
}P_CRYPT_CPS_PID_HANDLES;


typedef struct
{
	NEXUS_PlaybackHandle    pbHandle;
	NEXUS_PlaypumpHandle    pbPumpHandle;
	NEXUS_FilePlayHandle    pbFileHandle;
	NEXUS_RecordHandle      recHandle;
	NEXUS_RecpumpHandle     recPumpHandle;
	NEXUS_FileRecordHandle  recFileHandle;
	P_CRYPT_CPS_PID_HANDLES pidHandle[eDI_CRYPT_PIDTYPE_MAX];
}P_CRYPT_CPS_INSTANCE;

/* Deleted to avoid link-error in shard lib compile. */
//static HBOOL s_bCpsPbCompleted = 0;
static unsigned long s_ulCpsDecryptSem;
#endif

#if defined(CONFIG_KEY_PROVISIONING)
#define SIZEOF_DRM_BINFILE_HEADER (192)
#define SIZEOF_DYNAMIC_OFFSET_HEADER (16)

#if (NEXUS_VERSION >= 1501) || ((NEXUS_VERSION == 1401) && defined(CONFIG_CAS_NA_NOCS_3X))
#define SAGE_BINDING_OPERATION_SUCCESSFULLY_OCCURRED (OPERATION_SUCCESSFULLY_OCCURRED)
#else
#define SAGE_BINDING_OPERATION_SUCCESSFULLY_OCCURRED (BINDING_OPERATION_SUCCESSFULLY_OCCURRED)
#endif

#define GET_UINT32_FROM_BUF(pBuf) \
    (((HUINT32)(((HUINT8*)(pBuf))[0]) << 24) | \
     ((HUINT32)(((HUINT8*)(pBuf))[1]) << 16) | \
     ((HUINT32)(((HUINT8*)(pBuf))[2]) << 8)  | \
     ((HUINT8 *)(pBuf))[3])

typedef struct P_HDMI_HDCP22_PROVISION
{
	HUINT8 *pucInput;
	HUINT8 *pucOutput;
	HUINT32 nKeySize;
    SRAI_PlatformHandle platformHandle;
    SRAI_ModuleHandle moduleHandle;
	SRAI_ModuleHandle validationHandle;
}P_HDMI_HDCP22_PROVISION_t;
static P_HDMI_HDCP22_PROVISION_t s_tHDCP22Provision;
#endif

/*****************************************************************************
* External variables
******************************************************************************/
#if defined(CONFIG_SHARED_COMPILE)
static NEXUS_DmaHandle g_ulDmaHandle;
#else
extern NEXUS_DmaHandle g_ulDmaHandle;
#endif


/*****************************************************************************
* Static variables
******************************************************************************/

#if defined(CONFIG_KEY_PROVISIONING)
int Key_Provisioning_Init(void);
extern void Key_Provisioning_Uninit(void);
extern int Key_Provisioning_ValidationKey(void);
extern int Key_Provisioning_ProcessBinFile(void);
static int _KeyParseAndDisplay(HUINT8 *pBinFile, HUINT32 binFileLength);
static const char * _KeyMapDrmEnumToString(uint32_t drm_type);
#endif

DRV_Error DRV_CRYPT_Init(void)
{
	int err;
#if defined(CONFIG_SHARED_COMPILE) || defined(CONFIG_DI_MODULE_CRYPT_SHARED_LIB)
	if (g_ulDmaHandle) {
		PrintError("[%s] Crypt Module alread initialized! \n", __FUNCTION__);
		return DRV_OK;
	}

	g_ulDmaHandle = NEXUS_Dma_Open(NEXUS_ANY_ID, NULL);
	if (!g_ulDmaHandle) {
		PrintError("[%s] Error! can't open dma channel\n", __FUNCTION__);
		return DRV_ERR;
	}
#endif

#if defined(CONFIG_PVR_SECURITY_ENGINE_INLINE_CRYPTO)
	err = VK_SEM_Create(&s_ulCpsDecryptSem, (const char*)"DI_CRYPT_CPS_SEM", VK_SUSPENDTYPE_FIFO);
	if (err != VK_OK)
	{
		PrintError("[%s] Error! can't create sem for cps decrypt.\n", __FUNCTION__);
		return DRV_ERR;
	}
#else
	UNUSED_PARAM(err);
#endif

	PrintDebug("[%s] Completed! \n", __FUNCTION__);
	return DRV_OK;
}

static void P_DMA_CompletedCallback(void *context, int param)
{
	unsigned long		vkEvtId = (unsigned long)context;
	unsigned long		vkEvt = (unsigned long)param;
	int					vkRet;

	vkRet = VK_EVENT_Send(vkEvtId, vkEvt);
	if (VK_OK != vkRet)
	{
		PrintError("%s VK_EVENT_Send error\n", __FUNCTION__);
	}
}

static DRV_Error P_CRYPT_CopyDma(
	DI_CRYPT_CryptDataFormat etCryptData,
	unsigned char 		*pBufSrc,
	unsigned char 		*pBufDest,
	unsigned long 		uBufLeng,
	NEXUS_KeySlotHandle	keyHandle,
	NEXUS_DmaHandle		dma
	)
{
    NEXUS_DmaJobSettings 		jobSettings;
    NEXUS_DmaJobHandle 			dmaJob = NULL;
    NEXUS_DmaJobBlockSettings 	blockSettings;
    NEXUS_Error rc = NEXUS_SUCCESS;
	DRV_Error 					errCode = DRV_OK;
	HBOOL						sema = FALSE;
	unsigned long				vkEvtId = 0;
	unsigned long				vkEvt;
	int							vkRet;


	if ((dma == NULL) || (keyHandle == NULL))
	{
		PrintError("[%s] Parameter error!, DMA or KEY handle are NULL\n", __FUNCTION__);
		errCode = DRV_ERR;
		goto done;
	}

	vkRet = VK_EVENT_Create(&vkEvtId, "DMA_EVENT");
	if (VK_OK != vkRet)
	{
		errCode = DRV_ERR;
		goto done;
	}
#if defined(CONFIG_PVR_SECURITY_ENGINE_INLINE_CRYPTO)
	VK_SEM_Get(s_ulCpsDecryptSem);
	sema = TRUE;
#endif
	NEXUS_DmaJob_GetDefaultSettings(&jobSettings);
	jobSettings.numBlocks                   = 1;
	jobSettings.keySlot                     = keyHandle;
	jobSettings.dataFormat                  = (etCryptData == DI_CRYPT_STREAM) ? NEXUS_DmaDataFormat_eMpeg : NEXUS_DmaDataFormat_eBlock;
	jobSettings.completionCallback.callback = P_DMA_CompletedCallback;
	jobSettings.completionCallback.context = (void*)vkEvtId;
#if defined(CONFIG_PVR_SECURITY_ENGINE_INLINE_CRYPTO)
#if defined(CONFIG_LOCAL_TIMESTAMP_ENABLED_RECORD)
#if defined(CONFIG_LOCAL_TIMESTAMP_TYPE_32BIT_BINARY)
	jobSettings.timestampType				= (etCryptData == DI_CRYPT_STREAM) ? NEXUS_TransportTimestampType_e32_Binary : NEXUS_TransportTimestampType_eNone;
#else
	jobSettings.timestampType               = (etCryptData == DI_CRYPT_STREAM) ? NEXUS_TransportTimestampType_eMod300 : NEXUS_TransportTimestampType_eNone;
#endif
#else
	jobSettings.timestampType               = NEXUS_TransportTimestampType_eNone;
#endif
#endif
	dmaJob = NEXUS_DmaJob_Create(dma, &jobSettings);
	if (dmaJob == NULL)
	{
		PrintError("[%s] NEXUS_DmaJob_Create error\n", __FUNCTION__);
		errCode = DRV_ERR;
		goto done;
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
	vkRet = VK_EVENT_ReceiveTimeout(vkEvtId, &vkEvt, 100);
	if (VK_OK != vkRet)
	{
		PrintError("DMA transfer failed\n");
		errCode = DRV_ERR;
		goto done;
	}

done:

	if (NULL != dmaJob)
	{
		NEXUS_DmaJob_Destroy(dmaJob);
	}

#if defined(CONFIG_PVR_SECURITY_ENGINE_INLINE_CRYPTO)
	if (TRUE == sema)
	{
		VK_SEM_Release(s_ulCpsDecryptSem);
	}
#endif

	if (vkEvtId != 0)
	{
		(void)VK_EVENT_Destroy(vkEvtId);
	}

	return errCode;
}

static DRV_Error P_CRYPT_LoadUniqueKey(NEXUS_KeySlotHandle keyHandle, DI_CRYPT_CryptMode eCryptMode)
{
	NEXUS_Error	nexusError = NEXUS_SUCCESS;
	NEXUS_SecurityEncryptedSessionKey SessionKey;
	NEXUS_SecurityEncryptedControlWord CW;

	HUINT8 myAucKeyData3[ENCRYPTION_KEY_LEN] =
	{
#if defined(CONFIG_CAS_NA)
		0xdf, 0x72, 0xe9, 0x15, 0x9a, 0x30, 0xc6, 0x13,
		0xec, 0x5b, 0x00, 0x42, 0xb8, 0xed, 0x5e, 0x0a
#else
		0x0f, 0x09, 0xa2, 0x06, 0x19, 0x88, 0xb6, 0x89,
		0x28, 0xeb, 0x90, 0x2e, 0xb2, 0x36, 0x18, 0x88
#endif
	};

	HUINT8 myAucKeyData4[ENCRYPTION_KEY_LEN] =
	{
#if defined(CONFIG_CAS_NA)
		0xdf, 0x72, 0xe9, 0x15, 0x9a, 0x30, 0xc6, 0x13,
		0xec, 0x5b, 0x00, 0x42, 0xb8, 0xed, 0x5e, 0x0a
#else
		0xe4, 0x62, 0x75, 0x1b, 0x5d, 0xd4, 0xbc, 0x02,
		0x27, 0x9e, 0xc9, 0x6c, 0xc8, 0x66, 0xec, 0x10
#endif
	};

	if(eCryptMode == DI_CRYPT_TDES_DTCPIP)
	{
		HUINT8 tempAucKeyData3[ENCRYPTION_KEY_LEN] =
		{
			0xa1, 0x7d, 0xdc, 0xab, 0xb2, 0x3b, 0x6d, 0xf4,
			0x1c, 0xa9, 0x36, 0x98, 0x84, 0x5b, 0x97, 0x19
		};

		HUINT8 tempAucKeyData4[ENCRYPTION_KEY_LEN] =
		{
			0xEC, 0x9B, 0xF2, 0x86, 0xD9, 0x25, 0x52, 0xB7,
			0x11, 0x49, 0x87, 0xF0, 0xAE, 0xC3, 0xF2, 0x43
		};

		VK_MEM_Memcpy((void *)myAucKeyData3, (void *)tempAucKeyData3, ENCRYPTION_KEY_LEN);
		VK_MEM_Memcpy((void *)myAucKeyData4, (void *)tempAucKeyData4, ENCRYPTION_KEY_LEN);
	}

	/* Set up the key ladder to route out the key for (operation) */
#if defined(CONFIG_SECURITY_EXTENSION)
	NEXUS_Security_GetDefaultSessionKeySettings(&SessionKey);
#endif
	SessionKey.keyladderID   		= NEXUS_SecurityKeyladderID_eA;
	SessionKey.keyladderType 		= NEXUS_SecurityKeyladderType_e3Des;
	if(eCryptMode == DI_CRYPT_TDES_DTCPIP)
	{
		SessionKey.rootKeySrc    		= NEXUS_SecurityRootKeySrc_eCuskey;
		SessionKey.swizzleType   		= NEXUS_SecuritySwizzleType_eSwizzle0;
	}
	else
	{
		SessionKey.rootKeySrc    		= NEXUS_SecurityRootKeySrc_eOtpKeyA;
		SessionKey.swizzleType   		= NEXUS_SecuritySwizzleType_eNone;
	}
#if (NEXUS_VERSION >= 1304)
	SessionKey.keyEntryType 		= NEXUS_SecurityKeyType_eClear;
#else
 	SessionKey.keyEntryType  		= NEXUS_SecurityKeyType_eOdd;
#endif
#if defined(CONFIG_SECURITY_EXTENSION)
	SessionKey.dest			 		= NEXUS_SecurityAlgorithmConfigDestination_eMem2Mem;
#endif
 	SessionKey.operation     		= NEXUS_SecurityOperation_eDecrypt;
	SessionKey.operationKey2 		= NEXUS_SecurityOperation_eEncrypt;
	SessionKey.custSubMode	 		= NEXUS_SecurityCustomerSubMode_eGeneric_CP_128_4;
	SessionKey.virtualKeyLadderID 	= NEXUS_SecurityVirtualKeyladderID_eVKL5;
	SessionKey.keyMode		 		= NEXUS_SecurityKeyMode_eRegular;
	if(eCryptMode == DI_CRYPT_TDES_DTCPIP)
	{
		SessionKey.cusKeyL       		= 0x17;
		SessionKey.cusKeyH       		= 0x17;
		SessionKey.cusKeyVarL    		= 0xd7;
		SessionKey.cusKeyVarH    		= 0x5F;
	}
	else
	{
	    SessionKey.cusKeyL       		= 0;
	    SessionKey.cusKeyH       		= 0;
	    SessionKey.cusKeyVarL    		= 0;
	    SessionKey.cusKeyVarH    		= 0xFF;
	}
#if defined(CONFIG_SECURITY_EXTENSION)
	SessionKey.keyGenCmdID			= NEXUS_SecurityKeyGenCmdID_eKeyGen;
	SessionKey.sessionKeyOp 		= NEXUS_SecuritySessionKeyOp_eNoProcess;
#if defined(CONFIG_CAS_NA)
	SessionKey.bASKMMode	 		= true;
#elif ((BCHP_CHIP == 7346 || BCHP_CHIP == 7344) && defined(CONFIG_CAS_IR)) || defined(CONFIG_CAS_ASKM)
	SessionKey.bASKMMode			= true;
#elif defined(CONFIG_PROD_YSR2000) || defined(CONFIG_PROD_YS1000) || defined(CONFIG_TEMP_VTV200) || defined(CONFIG_TEMP_YSR2000) || defined(CONFIG_TEMP_YS1000)//modified to temporary.
	SessionKey.bASKMMode            = true;
#else
	SessionKey.bASKMMode	 		= false;
#endif
	SessionKey.bSwapAESKey	 		= false;
	SessionKey.keyDestIVType 		= NEXUS_SecurityKeyIVType_eNoIV;
#endif
	VK_MEM_Memcpy((void *)SessionKey.keyData, (void *)myAucKeyData3, sizeof(myAucKeyData3));
 	SessionKey.bRouteKey     		= false;

	nexusError = NEXUS_Security_GenerateSessionKey(keyHandle, &SessionKey);
	if (nexusError != NEXUS_SUCCESS)
	{
		PrintError("[%s] NEXUS_Security_GenerateSessionKey errCode: %x\n", __FUNCTION__, nexusError);

		return DRV_ERR;
	}

	/* Load CW - key4 -- routed out */
#if defined(CONFIG_SECURITY_EXTENSION)
	NEXUS_Security_GetDefaultControlWordSettings(&CW);
#endif
	CW.keyladderID	 		= NEXUS_SecurityKeyladderID_eA;
	CW.keyladderType 		= NEXUS_SecurityKeyladderType_e3Des;
#if (NEXUS_VERSION >= 1304)
	CW.keyEntryType 		= NEXUS_SecurityKeyType_eClear;
#else
	CW.keyEntryType	 		= NEXUS_SecurityKeyType_eOdd;
#endif
#if defined(CONFIG_SECURITY_EXTENSION)
	CW.dest			 		= NEXUS_SecurityAlgorithmConfigDestination_eMem2Mem;
#if ((BCHP_CHIP == 7346 || BCHP_CHIP == 7344) && defined(CONFIG_CAS_IR)) || defined(CONFIG_CAS_ASKM)
	CW.bASKMMode	 		= true;
#else
	CW.bASKMMode	 		= false;
#endif
#endif
	CW.custSubMode	 		= NEXUS_SecurityCustomerSubMode_eGeneric_CP_128_4;
	CW.virtualKeyLadderID 	= NEXUS_SecurityVirtualKeyladderID_eVKL5;
	CW.keyMode		 		= NEXUS_SecurityKeyMode_eRegular;
	CW.keySize		 		= sizeof(myAucKeyData4);
	VK_MEM_Memcpy((void *)CW.keyData, (void *)myAucKeyData4, CW.keySize);
	CW.operation	 		= NEXUS_SecurityOperation_eDecrypt;
	CW.bRouteKey	 		= true;
#if defined(CONFIG_SECURITY_EXTENSION)
	CW.keyDestIVType 		= NEXUS_SecurityKeyIVType_eNoIV;
	CW.keyGenCmdID	 		= NEXUS_SecurityKeyGenCmdID_eKeyGen;
	CW.bSwapAESKey	 		= false;
#endif

	nexusError = NEXUS_Security_GenerateControlWord(keyHandle, &CW);
	if (nexusError != NEXUS_SUCCESS)
	{
		PrintError("[%s] NEXUS_Security_GenerateControlWord errCode: %x\n", __FUNCTION__, nexusError);

		return DRV_ERR;
	}

	return DRV_OK;
}

static DRV_Error P_CRYPT_EncryptData(DI_CRYPT_CryptKeyType etCryptKey, DI_CRYPT_CryptDataFormat etCryptData, HUINT8 *pInData, HUINT8 *pOutData, HUINT32 ulLength, HUINT8 *pKey, HUINT32 ulKeyLength, DI_CRYPT_CryptMode eCryptMode, HUINT8 *pIvKey, HBOOL isEncryption)
{
	DRV_Error 						errCode = DRV_OK;
	NEXUS_Error						nexusError;
    NEXUS_KeySlotHandle				keyHandle;
	NEXUS_SecurityKeySlotSettings 	keySettings;
	NEXUS_SecurityAlgorithmSettings AlgorithmSettings;
	NEXUS_SecurityClearKey 			key;
	HUINT8 							encKey[ENCRYPTION_KEY_LEN];
	NEXUS_DmaHandle					hDmaHandle = NULL;
	/* IV keys for AES-CBC */
    HUINT8 IVKeys[IV_KEY_LEN] =
	{
		0xad, 0xd6, 0x9e, 0xa3,0x89, 0xc8, 0x17, 0x72,
		0x1e, 0xd4, 0x0e, 0xab,0x3d, 0xbc, 0x7a, 0xf2
	};

	/* prevent warning. initialize the variable */
	VK_MEM_Memset(encKey, 0, ENCRYPTION_KEY_LEN);

	if ((etCryptKey != DI_CRYPT_DEFAULT_SYSTEM_KEY) && ((pKey == NULL) || (ulKeyLength > ENCRYPTION_KEY_LEN)))
	{
		PrintError("[%s(%d)] key is null(pkey=0x%lx) or invalid key size (key_len =%d) \n", __FUNCTION__, __LINE__, pKey, ulKeyLength);
		return DRV_ERR;
	}

	if (etCryptKey != DI_CRYPT_DEFAULT_SYSTEM_KEY)
	{
		VK_MEM_Memcpy(encKey, pKey, ulKeyLength);
	}

/*************************
*	NOTICE!!
*	Data length should not be aligned with 192 or 188 bytes!
*************************/
#if defined(CONFIG_PVR_SECURITY_ENGINE_INLINE_CRYPTO)
	if ((etCryptData == DI_CRYPT_STREAM) && (ulLength % STREAM_ALIGN_BYTE))
	{
		PrintError("[%s] Align error!, Data size(%d) must be aligned with %d.\n", __FUNCTION__, ulLength, STREAM_ALIGN_BYTE);
		return DRV_ERR;
	}
#endif

	NEXUS_Security_GetDefaultKeySlotSettings(&keySettings);
	keySettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;
#if defined(CONFIG_PVR_SECURITY_ENGINE_INLINE_CRYPTO)
	if (etCryptData == DI_CRYPT_STREAM)
	{
		keySettings.keySlotType = NEXUS_SecurityKeySlotType_eType0;
	}
#endif
	keyHandle = NEXUS_Security_AllocateKeySlot(&keySettings);
	if (keyHandle == NULL)
	{
		PrintError("[%s(%d)] Error in NEXUS_Security_AllocateKeySlot()\n\r", __FUNCTION__, __LINE__);

		return DRV_ERR;
	}

	NEXUS_Security_GetDefaultAlgorithmSettings(&AlgorithmSettings);
	switch (eCryptMode)
	{
		case DI_CRYPT_AES :
		case DI_CRYPT_AES_CBC_MAC :
			AlgorithmSettings.algorithm 	= NEXUS_SecurityAlgorithm_eAes;
			AlgorithmSettings.algorithmVar 	= NEXUS_SecurityAlgorithmVariant_eCbc;
			break;
		case DI_CRYPT_AES_ECB :
			AlgorithmSettings.algorithm 	= NEXUS_SecurityAlgorithm_eAes;
			AlgorithmSettings.algorithmVar 	= NEXUS_SecurityAlgorithmVariant_eEcb;
			break;
		case DI_CRYPT_TDES :
		case DI_CRYPT_TDES_DTCPIP:
			AlgorithmSettings.algorithm		= NEXUS_SecurityAlgorithm_e3DesAba;
			AlgorithmSettings.algorithmVar 	= NEXUS_SecurityAlgorithmVariant_eEcb;
#if defined(CONFIG_CRYPT_LEGACY)
			/*nothing to do if keylength is already 16.*/
			if(ulKeyLength == 14)
			{
				ulKeyLength = 16;
				DRV_CRYPT_ExpandKeyLength(encKey, encKey);
			}
#else
			DRV_CRYPT_ExpandKeyLength(encKey, encKey);
#endif
			break;
		default :
			NEXUS_Security_FreeKeySlot(keyHandle);

			PrintError("[%s(%d)] Not supported encryption type! (eEncType : %d)\n\r", __FUNCTION__, __LINE__, eCryptMode);

			return DRV_ERR_INVALID_PARAMETER;
	}

	AlgorithmSettings.dest				= NEXUS_SecurityAlgorithmConfigDestination_eMem2Mem;
	AlgorithmSettings.terminationMode 	= (eCryptMode == DI_CRYPT_AES_CBC_MAC) ? NEXUS_SecurityTerminationMode_eCbcResidual : NEXUS_SecurityTerminationMode_eClear;
	AlgorithmSettings.operation 		= isEncryption ? NEXUS_SecurityOperation_eEncrypt : NEXUS_SecurityOperation_eDecrypt;
#if defined(CONFIG_CAS_NA)
	if(etCryptKey == DI_CRYPT_DEFAULT_SYSTEM_KEY)
	{
		AlgorithmSettings.askmModuleID 		= NEXUS_SecurityAskmModuleID_eModuleID_8;
		AlgorithmSettings.testKey2Select 	= 0x0;
		AlgorithmSettings.otpId 			= NEXUS_SecurityOtpId_eOtpVal;
		AlgorithmSettings.key2Select 		= NEXUS_SecurityKey2Select_eFixedKey;
	}
	AlgorithmSettings.caVendorID 			= 0x97ed;
#elif ((BCHP_CHIP == 7346 || BCHP_CHIP == 7344) && defined(CONFIG_CAS_IR)) || defined(CONFIG_CAS_ASKM)
	AlgorithmSettings.ivMode 			= NEXUS_SecurityIVMode_eRegular;
	AlgorithmSettings.solitarySelect 	= NEXUS_SecuritySolitarySelect_eClear;
	AlgorithmSettings.caVendorID 		= 0xEAF6;
	AlgorithmSettings.askmModuleID 		= NEXUS_SecurityAskmModuleID_eModuleID_8;
	AlgorithmSettings.otpId 			= NEXUS_SecurityOtpId_eOtpVal;
	AlgorithmSettings.key2Select 		= NEXUS_SecurityKey2Select_eRealKey;
#elif defined(CONFIG_PROD_YSR2000) || defined(CONFIG_PROD_YS1000) || defined(CONFIG_TEMP_VTV200) || defined(CONFIG_TEMP_YSR2000) || defined(CONFIG_TEMP_YS1000) //modified to temporary.
	AlgorithmSettings.ivMode 			= NEXUS_SecurityIVMode_eRegular;
	AlgorithmSettings.solitarySelect 	= NEXUS_SecuritySolitarySelect_eClear;
	AlgorithmSettings.caVendorID 		= 0x1234;
	AlgorithmSettings.askmModuleID 		= NEXUS_SecurityAskmModuleID_eModuleID_8;
	AlgorithmSettings.otpId 			= NEXUS_SecurityOtpId_eOtpVal;
	AlgorithmSettings.key2Select 		= NEXUS_SecurityKey2Select_eFixedKey;
#else
	AlgorithmSettings.ivMode 			= NEXUS_SecurityIVMode_eRegular;
	AlgorithmSettings.solitarySelect 	= NEXUS_SecuritySolitarySelect_eClear;
	AlgorithmSettings.caVendorID 		= 0x1234;
	AlgorithmSettings.askmModuleID 		= NEXUS_SecurityAskmModuleID_eModuleID_4;
	AlgorithmSettings.otpId 			= NEXUS_SecurityOtpId_eOtpVal;
	AlgorithmSettings.key2Select 		= NEXUS_SecurityKey2Select_eReserved1;
#endif

#if (NEXUS_VERSION >= 1304)
#if defined(CONFIG_PVR_SECURITY_ENGINE_INLINE_CRYPTO)
	if (etCryptData == DI_CRYPT_STREAM)
	{
		AlgorithmSettings.keyDestEntryType  = NEXUS_SecurityKeyType_eOddAndEven;
	}
	else
	{
		AlgorithmSettings.keyDestEntryType  = NEXUS_SecurityKeyType_eClear;
	}
#else
	AlgorithmSettings.keyDestEntryType	= NEXUS_SecurityKeyType_eClear;
#endif
#else
	AlgorithmSettings.keyDestEntryType 	= NEXUS_SecurityKeyType_eOdd;
#endif
#if	defined(CONFIG_LOCAL_TIMESTAMP_ENABLED_RECORD)
	if (etCryptData == DI_CRYPT_STREAM)
	{
		AlgorithmSettings.enableTimestamps = true; /* enable timestamps */
	}
#endif
#if defined(CONFIG_PVR_SECURITY_ENGINE_INLINE_CRYPTO)
	if (etCryptData == DI_CRYPT_STREAM)
	{
		AlgorithmSettings.modifyScValue[NEXUS_SecurityPacketType_eRestricted] = true;
		AlgorithmSettings.modifyScValue[NEXUS_SecurityPacketType_eGlobal] = true;
		AlgorithmSettings.scValue[NEXUS_SecurityPacketType_eGlobal] = NEXUS_SecurityAlgorithmScPolarity_eClear;
		AlgorithmSettings.scValue[NEXUS_SecurityPacketType_eRestricted] = NEXUS_SecurityAlgorithmScPolarity_eClear;
	}
#endif
	nexusError = NEXUS_Security_ConfigAlgorithm(keyHandle, &AlgorithmSettings);
	if (nexusError != NEXUS_SUCCESS)
	{
		NEXUS_Security_FreeKeySlot(keyHandle);

		PrintError("[%s(%d)] NEXUS_Security_ConfigAlgorithm() error(%x)\n", __FUNCTION__, __LINE__, nexusError);

		return DRV_ERR;
	}

	if (etCryptKey == DI_CRYPT_DEFAULT_SYSTEM_KEY)
	{
		errCode = P_CRYPT_LoadUniqueKey(keyHandle, eCryptMode);
		if (errCode != DRV_OK)
		{
			NEXUS_Security_FreeKeySlot(keyHandle);

			PrintError("[%s] P_CRYPT_LoadUniqueKey error(0x%x)\n", __FUNCTION__, errCode);

			return errCode;
		}
	}
	else
	{
		NEXUS_Security_GetDefaultClearKey(&key);

		key.keySize      = ulKeyLength;
#if (NEXUS_VERSION >= 1304)
#if defined(CONFIG_PVR_SECURITY_ENGINE_INLINE_CRYPTO)
		if (etCryptData == DI_CRYPT_STREAM)
		{
			key.keyEntryType = NEXUS_SecurityKeyType_eOdd;
		}
		else
		{
			key.keyEntryType = NEXUS_SecurityKeyType_eClear;
		}
#else
		key.keyEntryType = NEXUS_SecurityKeyType_eClear;
#endif
#else
	 	key.keyEntryType = NEXUS_SecurityKeyType_eOdd;
#endif
		VK_MEM_Memcpy((void *)key.keyData, (void *)encKey, key.keySize);

		nexusError = NEXUS_Security_LoadClearKey(keyHandle, &key);
		if (nexusError != NEXUS_SUCCESS)
		{
			NEXUS_Security_FreeKeySlot(keyHandle);

			PrintError("[%s] NEXUS_Security_LoadClearKey() nexusError: %x, line %d\n", __FUNCTION__, nexusError, __LINE__ );

			return DRV_ERR;
		}

		if (eCryptMode == DI_CRYPT_AES)
		{
#if (NEXUS_VERSION >= 1304)
#else
			AlgorithmSettings.keyDestEntryType 	= NEXUS_SecurityKeyType_eEven;
			nexusError = NEXUS_Security_ConfigAlgorithm(keyHandle, &AlgorithmSettings);
			if (nexusError != NEXUS_SUCCESS)
			{
				NEXUS_Security_FreeKeySlot(keyHandle);

				PrintError("[%s(%d)] NEXUS_Security_ConfigAlgorithm() error(%x)\n", __FUNCTION__, __LINE__, nexusError);

				return DRV_ERR;
			}
#endif

#if (NEXUS_VERSION >= 1304)
#if defined(CONFIG_PVR_SECURITY_ENGINE_INLINE_CRYPTO)
			if (etCryptData == DI_CRYPT_STREAM)
			{
				key.keyEntryType = NEXUS_SecurityKeyType_eEven;
			}
			else
			{
				key.keyEntryType = NEXUS_SecurityKeyType_eClear;
			}
#else

			key.keyEntryType = NEXUS_SecurityKeyType_eClear;
#endif
#else
			key.keyEntryType = NEXUS_SecurityKeyType_eEven;
#endif
			nexusError = NEXUS_Security_LoadClearKey(keyHandle, &key);
			if (nexusError != NEXUS_SUCCESS)
			{
				NEXUS_Security_FreeKeySlot(keyHandle);

				PrintError("[%s] NEXUS_Security_LoadClearKey() nexusError: %x, line %d\n", __FUNCTION__, nexusError, __LINE__ );

				return DRV_ERR;
			}
		}

		if ((AlgorithmSettings.algorithmVar == NEXUS_SecurityAlgorithmVariant_eCbc) && (pIvKey != NULL))
		{
			NEXUS_Security_GetDefaultClearKey(&key);
			key.keySize      = ulKeyLength;
#if (NEXUS_VERSION >= 1304)
#if defined(CONFIG_PVR_SECURITY_ENGINE_INLINE_CRYPTO)
			key.keyEntryType = NEXUS_SecurityKeyType_eOddAndEven;
#else
			key.keyEntryType = NEXUS_SecurityKeyType_eClear;
#endif
#else
			key.keyEntryType = NEXUS_SecurityKeyType_eOdd;
#endif
			key.keyIVType = NEXUS_SecurityKeyIVType_eIV;

			VK_MEM_Memcpy((void *)key.keyData, (void *)pIvKey, key.keySize);

			nexusError = NEXUS_Security_LoadClearKey(keyHandle, &key);
			if (nexusError != NEXUS_SUCCESS)
			{
				NEXUS_Security_FreeKeySlot(keyHandle);

				PrintError("[%s] NEXUS_Security_LoadClearKey() nexusError: %x, line %d\n", __FUNCTION__, nexusError, __LINE__ );

				return DRV_ERR;
			}
			/* FIXME: Setting IV for even key may be needed if you really using both.*/
		}
	}

	if (eCryptMode == DI_CRYPT_AES_CBC_MAC)
	{
		NEXUS_Security_GetDefaultClearKey(&key);

		/* Load NULL IV keys for CBC-MAC */
		key.keySize 		= sizeof(IVKeys);
#if (NEXUS_VERSION >= 1304)
		key.keyEntryType 	= NEXUS_SecurityKeyType_eClear;
#else
		key.keyEntryType 	= NEXUS_SecurityKeyType_eOdd;
#endif
		key.keyIVType		= NEXUS_SecurityKeyIVType_eIV;

		nexusError = NEXUS_Security_LoadClearKey(keyHandle, &key);
		if (nexusError != NEXUS_SUCCESS)
		{
			NEXUS_Security_FreeKeySlot(keyHandle);

			PrintError("[%s] IV loading failed, line %d\n", __FUNCTION__, __LINE__ );

			return DRV_ERR;
		}
	}
	else
	{
		if ((AlgorithmSettings.algorithmVar == NEXUS_SecurityAlgorithmVariant_eCbc) && (pIvKey == NULL))
		{
			NEXUS_Security_GetDefaultClearKey(&key);

			/* Load IV keys for CBC */
			key.keySize 		= sizeof(IVKeys);
#if (NEXUS_VERSION >= 1304)
			key.keyEntryType	= NEXUS_SecurityKeyType_eClear;
#else
			key.keyEntryType 	= NEXUS_SecurityKeyType_eOdd;
#endif
			key.keyIVType		= NEXUS_SecurityKeyIVType_eIV;
			VK_MEM_Memcpy((void *)key.keyData, (void *)IVKeys, key.keySize);

			nexusError = NEXUS_Security_LoadClearKey(keyHandle, &key);
			if (nexusError != NEXUS_SUCCESS)
			{
				NEXUS_Security_FreeKeySlot(keyHandle);

				PrintError("[%s] IV loading failed, line %d\n", __FUNCTION__, __LINE__ );

				return DRV_ERR;
			}
		}
	}

	hDmaHandle = NEXUS_Dma_Open(NEXUS_ANY_ID, NULL);
	if (!hDmaHandle) {
		PrintError("[%s] Error! can't open dma channel\n", __FUNCTION__);
		return DRV_ERR;
	}

	errCode = P_CRYPT_CopyDma(etCryptData,
							   pInData,
							   pOutData,
							   ulLength,
							   keyHandle,
							   hDmaHandle);
	if (errCode != DRV_OK)
	{
		PrintError("[%s(%d)] Error on P_CRYPT_CopyDma data!\n", __FUNCTION__, __LINE__);
	}

	NEXUS_Dma_Close(hDmaHandle);
	NEXUS_Security_FreeKeySlot(keyHandle);

	return errCode;
}

/* Deleted to avoid link-error in shard lib compile. */
/* These functions are not any used for CPS playback. but, leaving the source code in order to keep the algorithm of XPT inline crypto. */
#if 0 /* defined(CONFIG_PVR_SECURITY_ENGINE_INLINE_CRYPTO) */
static void P_CRYPT_EndOfStreamCallback(void *context, int param)
{
	UNUSED_PARAM(context);
	if (param)
	{
		s_bCpsPbCompleted = TRUE;
		PrintDebug("[%s]end of stream\n", __FUNCTION__);
	}
	else
	{
		PrintDebug("[%s]beginning of stream\n", __FUNCTION__);
	}
}


static void P_CRYPTO_ReleaseResource(P_CRYPT_CPS_INSTANCE *pCpsIns)
{
	int i;

	if (pCpsIns->pbHandle != NULL)
	{
		NEXUS_Playback_Stop(pCpsIns->pbHandle);
	}

	if (pCpsIns->recHandle != NULL)
	{
		NEXUS_Record_Stop(pCpsIns->recHandle);
		NEXUS_Record_RemoveAllPidChannels(pCpsIns->recHandle);
	}

	if (pCpsIns->pbHandle != NULL)
	{
		for (i=0; i<eDI_CRYPT_PIDTYPE_MAX; i++)
		{
			if (pCpsIns->pidHandle[i].pidChannelHandle != NULL)
			{
				NEXUS_Playback_ClosePidChannel(pCpsIns->pbHandle, pCpsIns->pidHandle[i].pidChannelHandle);
			}
		}
		NEXUS_Playback_Destroy(pCpsIns->pbHandle);
	}

	if (pCpsIns->pbFileHandle != NULL)
	{
		NEXUS_FilePlay_Close(pCpsIns->pbFileHandle);
	}

	if (pCpsIns->recFileHandle != NULL)
	{
		NEXUS_FileRecord_Close(pCpsIns->recFileHandle);
	}

	if (pCpsIns->recHandle != NULL)
	{
		NEXUS_Record_Destroy(pCpsIns->recHandle);
	}
	if (pCpsIns->recPumpHandle != NULL)
	{
		NEXUS_Recpump_Close(pCpsIns->recPumpHandle);
	}
	for (i=0; i<eDI_CRYPT_PIDTYPE_MAX; i++)
	{
#if 0
		if ((pCpsIns->pidHandle[i].dscKeyHandle != NULL) && (pCpsIns->pidHandle[i].pidChannelHandle != NULL))
		{
			NEXUS_KeySlot_RemovePidChannel(pCpsIns->pidHandle[i].dscKeyHandle, pCpsIns->pidHandle[i].pidChannelHandle);
		}
#endif
		if (pCpsIns->pidHandle[i].dscKeyHandle != NULL)
		{
			NEXUS_Security_FreeKeySlot(pCpsIns->pidHandle[i].dscKeyHandle);
		}
	}
}


static NEXUS_PidChannelHandle P_CRYPTO_PidChannelOpen(NEXUS_PlaybackHandle pPlayback, DI_CRYPT_VideoCodec eVideoCodec, DI_CRYPT_PIDTYPE ePidType, HUINT16 usPid)
{
	NEXUS_PlaybackPidChannelSettings playbackPidSettings;
	NEXUS_PidChannelHandle pPidChannelHandle = NULL;

	if (pPlayback == NULL)
	{
		PrintError("[%s,%d] Error invalid parameter!!\n\r", __FUNCTION__, __LINE__);
		return NULL;
	}

	NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
	switch(ePidType)
	{
		case eDI_CRYPT_PIDTYPE_VIDEO:
			playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eVideo;
			playbackPidSettings.pidTypeSettings.video.codec = DRV_VIDEO_GetCodecType(0, eVideoCodec);
			break;
		case eDI_CRYPT_PIDTYPE_AUDIO_MAIN:
		case eDI_CRYPT_PIDTYPE_AUDIO_SUB:
		case eDI_CRYPT_PIDTYPE_AUDIO_SPDIF:
			playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eAudio;
			break;
		case eDI_CRYPT_PIDTYPE_PES:
		default:
			playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eOther;
			break;
	}

	pPidChannelHandle = NEXUS_Playback_OpenPidChannel(pPlayback, usPid, &playbackPidSettings);
	PrintDebug("[%s,%d] ePidType:%d, usPid: 0x%x, pPidChannelHandle: 0x%x\n", __FUNCTION__, __LINE__, ePidType, usPid, pPidChannelHandle);

	return pPidChannelHandle;
}


static NEXUS_KeySlotHandle P_CRYPTO_DecKeySlotSet(NEXUS_PidChannelHandle ulPidChannelHandle, DI_CRYPT_CryptMode eCryptMode, HUINT8 *pucKey, HUINT32 ulKeyLength)
{
	NEXUS_Error						nexusError;
    NEXUS_KeySlotHandle				decKeyHandle;
	NEXUS_SecurityKeySlotSettings 	keySettings;
	NEXUS_SecurityAlgorithmSettings algConfig;
	NEXUS_SecurityClearKey 			key;
	HUINT8 							decKey[ENCRYPTION_KEY_LEN];
#if 0
	/* IV keys for AES-CBC */
    HUINT8 IVKeys[IV_KEY_LEN] =
	{
		0xad, 0xd6, 0x9e, 0xa3,0x89, 0xc8, 0x17, 0x72,
		0x1e, 0xd4, 0x0e, 0xab,0x3d, 0xbc, 0x7a, 0xf2
	};
#endif

	if (ulPidChannelHandle == NULL)
	{
		PrintError("[%s,%d] Error invalid parameter!!\n\r", __FUNCTION__, __LINE__);
		return NULL;
	}
	VK_MEM_Memset(decKey, 0, ENCRYPTION_KEY_LEN);
	VK_MEM_Memcpy(decKey, pucKey, ulKeyLength);
	//DI_UART_Dump(decKey, ulKeyLength, 16);

	NEXUS_Security_GetDefaultKeySlotSettings(&keySettings);
	keySettings.keySlotEngine = NEXUS_SecurityEngine_eCa;
	decKeyHandle = NEXUS_Security_AllocateKeySlot(&keySettings);
	if (decKeyHandle == NULL)
	{
		PrintError("[%s,%d] Error in NEXUS_Security_AllocateKeySlot()\n\r", __FUNCTION__, __LINE__);
		return NULL;
	}

	NEXUS_Security_GetDefaultAlgorithmSettings(&algConfig);
	switch (eCryptMode)
	{
#if 0
		case DI_CRYPT_AES :
		case DI_CRYPT_AES_CBC_MAC :
			algConfig.algorithm 	= NEXUS_SecurityAlgorithm_eAes;
			algConfig.algorithmVar 	= NEXUS_SecurityAlgorithmVariant_eCbc;
			break;
#endif
		case DI_CRYPT_TDES :
		case DI_CRYPT_TDES_DTCPIP:
			algConfig.algorithm		= NEXUS_SecurityAlgorithm_e3DesAba;
			algConfig.algorithmVar 	= NEXUS_SecurityAlgorithmVariant_eEcb;
#if defined(CONFIG_CRYPT_LEGACY)
			/*nothing to do if keylength is already 16.*/
			if(ulKeyLength == 14)
			{
				ulKeyLength = 16;
				DRV_CRYPT_ExpandKeyLength(decKey, decKey);
			}
#else
			DRV_CRYPT_ExpandKeyLength(decKey, decKey);
#endif
			break;
		default :
			NEXUS_Security_FreeKeySlot(decKeyHandle);
			PrintError("[%s,%d] Not supported encryption type! (eEncType : %d)\n\r", __FUNCTION__, __LINE__, eCryptMode);
			return NULL;
	}

	algConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;
	algConfig.operation = NEXUS_SecurityOperation_eDecrypt;
	algConfig.dest = NEXUS_SecurityAlgorithmConfigDestination_eCa;
#if defined(CONFIG_LOCAL_TIMESTAMP_ENABLED_RECORD)
	algConfig.enableTimestamps = true;
#endif
	algConfig.modifyScValue[NEXUS_SecurityPacketType_eRestricted] = true;
	algConfig.modifyScValue[NEXUS_SecurityPacketType_eGlobal]     = true;
#if 1 /* nothing done  here for 40-nm platforms; CPS will modify SC bits. */
	algConfig.scValue[NEXUS_SecurityPacketType_eRestricted]       = NEXUS_SecurityAlgorithmScPolarity_eClear;
	algConfig.scValue[NEXUS_SecurityPacketType_eGlobal]           = NEXUS_SecurityAlgorithmScPolarity_eClear;
#endif
	nexusError = NEXUS_Security_ConfigAlgorithm(decKeyHandle, &algConfig);
	if (nexusError != NEXUS_SUCCESS)
	{
		NEXUS_Security_FreeKeySlot(decKeyHandle);
		PrintError("[%s,%d] Error(0x%x) in NEXUS_Security_ConfigAlgorithm()\n\r", __FUNCTION__, __LINE__, nexusError);
		return NULL;
	}

	NEXUS_Security_GetDefaultClearKey(&key);
	key.keyEntryType = NEXUS_SecurityKeyType_eEven;
	key.keySize = sizeof(decKey);
	key.keyIVType =NEXUS_SecurityKeyIVType_eNoIV;
	VK_MEM_Memcpy(key.keyData, decKey, sizeof(decKey));
	nexusError = NEXUS_Security_LoadClearKey(decKeyHandle, &key);
	if (nexusError != NEXUS_SUCCESS)
	{
		NEXUS_Security_FreeKeySlot(decKeyHandle);
		PrintError("[%s,%d] Error(0x%x) in NEXUS_Security_LoadClearKey()\n\r", __FUNCTION__, __LINE__, nexusError);
		return NULL;
	}

	key.keyEntryType = NEXUS_SecurityKeyType_eOdd;
	key.keySize = sizeof(decKey);
	key.keyIVType =NEXUS_SecurityKeyIVType_eNoIV;
	VK_MEM_Memcpy(key.keyData, decKey, sizeof(decKey));
	nexusError = NEXUS_Security_LoadClearKey(decKeyHandle, &key);
	if (nexusError != NEXUS_SUCCESS)
	{
		NEXUS_Security_FreeKeySlot(decKeyHandle);
		PrintError("[%s,%d] Error(0x%x) in NEXUS_Security_LoadClearKey()\n\r", __FUNCTION__, __LINE__, nexusError);
		return NULL;
	}

	/* Add video PID channel to keyslot */
	nexusError = NEXUS_KeySlot_AddPidChannel(decKeyHandle, ulPidChannelHandle);
	if (nexusError != NEXUS_SUCCESS)
	{
		NEXUS_Security_FreeKeySlot(decKeyHandle);
		PrintError("[%s,%d] Error(0x%x) in NEXUS_KeySlot_AddPidChannel()\n\r", __FUNCTION__, __LINE__, nexusError);
		return NULL;
	}

	PrintDebug("[%s,%d] decKeyHandle: 0x%x\n", __FUNCTION__, __LINE__, decKeyHandle);
	return decKeyHandle;
}


static DRV_Error P_CRYPTO_RecordAddPidChannel(NEXUS_RecordHandle ulRecordHandle, NEXUS_PidChannelHandle ulPidChannelHandle, DI_CRYPT_PIDTYPE eChannelType, DI_CRYPT_VideoCodec eVideoCodec)
{
	DRV_Error 						errCode = DRV_OK;
	NEXUS_Error						nexusError;
	NEXUS_RecordPidChannelSettings	recordPidSettings;

	if ((ulRecordHandle == NULL) || (ulPidChannelHandle == NULL))
	{
		PrintError("[%s,%d]Error invalid parameter!!\n\r", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	NEXUS_Record_GetDefaultPidChannelSettings(&recordPidSettings);
	switch(eChannelType)
	{
		case eDI_CRYPT_PIDTYPE_VIDEO:
			recordPidSettings.recpumpSettings.pidType = NEXUS_PidType_eVideo;
			recordPidSettings.recpumpSettings.pidTypeSettings.video.index = true;
			recordPidSettings.recpumpSettings.pidTypeSettings.video.codec = DRV_VIDEO_GetCodecType(0, eVideoCodec);
			//recordPidSettings.recpumpSettings.pidTypeSettings.video.pid = usPid;
			break;
		case eDI_CRYPT_PIDTYPE_AUDIO_MAIN:
		case eDI_CRYPT_PIDTYPE_AUDIO_SUB:
		case eDI_CRYPT_PIDTYPE_AUDIO_SPDIF:
			recordPidSettings.recpumpSettings.pidType = NEXUS_PidType_eAudio;
			break;
		case eDI_CRYPT_PIDTYPE_PES:
		default:
			recordPidSettings.recpumpSettings.pidType = NEXUS_PidType_eOther;
			break;
	}

	nexusError = NEXUS_Record_AddPidChannel(ulRecordHandle, ulPidChannelHandle, &recordPidSettings);
	if (nexusError != NEXUS_SUCCESS)
	{
		PrintError("[%s,%d]Error(0x%x) in NEXUS_Record_AddPidChannel()\n\r", __FUNCTION__, __LINE__, nexusError);
	}

	return errCode;
}



DRV_Error P_CRYPT_DecryptCps(DI_CRYPT_CryptMode eCryptMode, HUINT8 *pInData, HUINT8 *pOutData, HUINT32 ulLength, HUINT8 *pucKey, HUINT32 ulKeyLength, stCryptPid_t *pPid)
{
	DRV_Error 							errCode = DRV_OK;
    NEXUS_RecpumpOpenSettings			recpumpOpenSettings;
	NEXUS_RecpumpStatus 				recpumpStatus;
	NEXUS_RecordSettings				recordCfg;
	P_CRYPT_CPS_INSTANCE				stCpsIns;
	const char *recordfname = "/tmp/recordfile.ts\0";
    const char *recordindex = "/tmp/recordfile.nts\0";
	NEXUS_PlaybackSettings playbackSettings;
	const char *playfname = "/tmp/playfile.ts\0";
	FILE *fd;
	HUINT32	len, i, size;

	if ((pInData == NULL) || (pOutData == NULL))
	{
		PrintError("[%s,%d]Error invalid parameter!!\n\r", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	VK_SEM_Get(s_ulCpsDecryptSem);
	VK_MEM_Memset(&stCpsIns, 0, sizeof(stCpsIns));

	fd = fopen(playfname, "wb");
	if (fd == NULL)
	{
		PrintError("can't open file:%s\n", playfname);
		VK_SEM_Release(s_ulCpsDecryptSem);
		return DRV_ERR;
	}

	len = fwrite(pInData, 1, ulLength, fd);
	if (len != ulLength)
	{
		PrintError("[%s,%d] Error in fwrite() to write playback file\n\r", __FUNCTION__, __LINE__);
		fclose(fd);
		VK_SEM_Release(s_ulCpsDecryptSem);
		return DRV_ERR;
	}

	fclose(fd);

#if 0
	DI_UART_Print("[%s,%d] ulLength: %d\n", __FUNCTION__, __LINE__, ulLength);
	DI_UART_Print("eCryptMode: %d\n", eCryptMode);
	//DI_UART_Dump(pucKey, ulKeyLength, 16);
	DI_UART_Print("pPid->eVideoCodec : 0x%x\n", pPid->eVideoCodec);
	for (i=0; i<eDI_CRYPT_PIDTYPE_MAX; i++)
	{
		if (pPid->usPid[i] != DI_CRYPT_PID_NULL)
		{
			DI_UART_Print("pPid->usPid[%d] : 0x%x\n", i, pPid->usPid[i]);
		}
	}
#endif
    errCode = DRV_PVR_GetPlaypumpHandle(&stCpsIns.pbPumpHandle); //NEXUS_Playpump_Open(NEXUS_ANY_ID, NULL);
	if (errCode != DRV_OK)
	{
		PrintError("[%s,%d] Error in NEXUS_Playpump_Open()\n\r", __FUNCTION__, __LINE__);
		errCode = DRV_ERR;
		goto release_resource;
	}

    stCpsIns.pbHandle = NEXUS_Playback_Create();
	if (stCpsIns.pbHandle == NULL)
	{
		PrintError("[%s,%d] Error in NEXUS_Playback_Create()\n\r", __FUNCTION__, __LINE__);
		errCode = DRV_ERR;
		goto release_resource;
	}

    stCpsIns.pbFileHandle = NEXUS_FilePlay_OpenPosix_Nodirect(playfname, NULL);	//NEXUS_FilePlay_OpenPosix(playfname, NULL);
    if (stCpsIns.pbFileHandle == NULL)
	{
        PrintError("can't open file:%s\n", playfname);
        errCode = DRV_ERR;
		goto release_resource;
    }

	NEXUS_Playback_GetSettings(stCpsIns.pbHandle, &playbackSettings);
    playbackSettings.playpump = stCpsIns.pbPumpHandle;
    /* set a stream format, it could be any audio video transport type or file format, i.e NEXUS_TransportType_eMp4, NEXUS_TransportType_eAvi ... */
    playbackSettings.playpumpSettings.transportType = NEXUS_TransportType_eTs;

#if defined(CONFIG_LOCAL_TIMESTAMP_ENABLED_RECORD) && defined(CONFIG_LOCAL_TIMESTAMP_TYPE_32BIT_BINARY)
	playbackSettings.playpumpSettings.timestamp.type = NEXUS_TransportTimestampType_e32_Binary;
#endif
#if 0
	playbackSettings.playpumpSettings.timestamp.pacing = true;
	playbackSettings.playpumpSettings.timestamp.pacingMaxError = 2636;
	playbackSettings.playpumpSettings.timestamp.forceRestamping = true;
#endif
	playbackSettings.beginningOfStreamAction = NEXUS_PlaybackLoopMode_ePlay;
	playbackSettings.beginningOfStreamCallback.callback = P_CRYPT_EndOfStreamCallback;
	playbackSettings.beginningOfStreamCallback.param = 0;
	playbackSettings.endOfStreamAction = NEXUS_PlaybackLoopMode_ePlay;
	playbackSettings.endOfStreamCallback.callback = P_CRYPT_EndOfStreamCallback;
	playbackSettings.endOfStreamCallback.param = 1;
    NEXUS_Playback_SetSettings(stCpsIns.pbHandle, &playbackSettings);

	for (i=0; i<eDI_CRYPT_PIDTYPE_MAX; i++)
	{
		if (pPid->usPid[i] != DI_CRYPT_PID_NULL)
		{
			stCpsIns.pidHandle[i].pidChannelHandle = P_CRYPTO_PidChannelOpen(stCpsIns.pbHandle, pPid->eVideoCodec, i, pPid->usPid[i]);
			if (stCpsIns.pidHandle[i].pidChannelHandle == NULL)
			{
				PrintError("[%s,%d] Error in P_CRYPTO_PidChannelOpen()\n\r", __FUNCTION__, __LINE__);
				errCode = DRV_ERR;
				goto release_resource;
			}
			if (i < eDI_CRYPT_PIDTYPE_PCR)	//eDI_CRYPT_PIDTYPE_PES)
			{
				stCpsIns.pidHandle[i].dscKeyHandle = P_CRYPTO_DecKeySlotSet(stCpsIns.pidHandle[i].pidChannelHandle, eCryptMode, pucKey, ulKeyLength);
				if (stCpsIns.pidHandle[i].pidChannelHandle == NULL)
				{
					PrintError("[%s,%d] Error in P_CRYPTO_DecKeySlotSet()\n\r", __FUNCTION__, __LINE__);
					errCode = DRV_ERR;
					goto release_resource;
				}
			}
		}
	}

	/* Start playback */
	s_bCpsPbCompleted = FALSE;
	NEXUS_Playback_Start(stCpsIns.pbHandle, stCpsIns.pbFileHandle, NULL);

	NEXUS_Recpump_GetDefaultOpenSettings(&recpumpOpenSettings);
	/* set threshold to 80%. with band hold enabled, it's not actually a dataready threshold. it's
    a bandhold threshold. we are relying on the timer that's already in record. */
#if defined(CONFIG_LOCAL_TIMESTAMP_ENABLED_RECORD)
	recpumpOpenSettings.data.bufferSize = DRV_PVR_REC_BUFFER + 64;
#else
	recpumpOpenSettings.data.bufferSize = DRV_PVR_REC_BUFFER + 68;
#endif
	recpumpOpenSettings.data.dataReadyThreshold = (188/4)*4096*4;
	recpumpOpenSettings.data.alignment = 12;
#if defined(CONFIG_JAPAN_SPEC)
	recpumpOpenSettings.data.atomSize = 128*1024;
#endif
	recpumpOpenSettings.index.bufferSize = (ITB_BUFFER_CHUNK_SIZE*ITB_BUFFER_CHUNK_COUNT);
	recpumpOpenSettings.index.alignment = 0;
	recpumpOpenSettings.index.dataReadyThreshold = (ITB_BUFFER_CHUNK_SIZE*(ITB_BUFFER_CHUNK_COUNT/5));
	stCpsIns.recPumpHandle = NEXUS_Recpump_Open(NEXUS_ANY_ID, &recpumpOpenSettings);
    if (stCpsIns.recPumpHandle == NULL)
	{
		PrintError("[%s,%d] Error in NEXUS_Recpump_Open()\n\r", __FUNCTION__, __LINE__);
		errCode = DRV_ERR;
		goto release_resource;

	}

	stCpsIns.recHandle = NEXUS_Record_Create();
    if (stCpsIns.recHandle == NULL)
	{
		PrintError("[%s,%d] Error in NEXUS_Record_Create()\n\r", __FUNCTION__, __LINE__);
		errCode = DRV_ERR;
		goto release_resource;
	}

	NEXUS_Record_GetSettings(stCpsIns.recHandle, &recordCfg);
    recordCfg.recpump = stCpsIns.recPumpHandle;
    /* enable bandhold. required for record from playback. */
    recordCfg.recpumpSettings.bandHold = NEXUS_RecpumpFlowControl_eEnable;
#if defined(CONFIG_LOCAL_TIMESTAMP_ENABLED_RECORD) && defined(CONFIG_LOCAL_TIMESTAMP_TYPE_32BIT_BINARY)
	recordCfg.recpumpSettings.timestampType = NEXUS_TransportTimestampType_e32_Binary;
#if 1
	if ((pPid->usPid[eDI_CRYPT_PIDTYPE_PCR] != DI_CRYPT_PID_NULL) && (stCpsIns.pidHandle[eDI_CRYPT_PIDTYPE_PCR].pidChannelHandle != NULL))
	{
		recordCfg.recpumpSettings.adjustTimestampUsingPcrs = true;
		recordCfg.recpumpSettings.pcrPidChannel = stCpsIns.pidHandle[eDI_CRYPT_PIDTYPE_PCR].pidChannelHandle;
		//PrintDebug("[%s,%d] pcrPidChannel: 0x%x\n", __FUNCTION__, __LINE__, recordCfg.recpumpSettings.pcrPidChannel);
	}
#endif
#endif
    NEXUS_Record_SetSettings(stCpsIns.recHandle, &recordCfg);

    stCpsIns.recFileHandle = NEXUS_FileRecord_OpenPosix_Nodirect(recordfname, recordindex); //NEXUS_FileRecord_OpenPosix(recordfname, recordindex);
    if (stCpsIns.recFileHandle == NULL)
	{
        PrintError("can't open file:%s %s\n", recordfname, recordindex);
    	errCode = DRV_ERR;
		goto release_resource;
	}

	for (i=0; i<eDI_CRYPT_PIDTYPE_MAX; i++)
	{
		if (pPid->usPid[i] != DI_CRYPT_PID_NULL)
		{
			errCode = P_CRYPTO_RecordAddPidChannel(stCpsIns.recHandle, stCpsIns.pidHandle[i].pidChannelHandle, i, pPid->eVideoCodec);
			if (errCode != DRV_OK)
			{
				PrintError("[%s,%d] Error in P_CRYPTO_RecordAddPidChannel()\n\r", __FUNCTION__, __LINE__);
				errCode = DRV_ERR;
				goto release_resource;
			}
		}
	}

	NEXUS_Record_Start(stCpsIns.recHandle, stCpsIns.recFileHandle);
	len = 0;
	while (len < ulLength)
	{

		NEXUS_Recpump_GetStatus(stCpsIns.recPumpHandle, &recpumpStatus);
#if 0
		PrintDebug("status: RAVE %d\n", recpumpStatus.rave.index);
		PrintDebug("  data: %u total bytes \tfifo %u/%u\n", (HUINT32)recpumpStatus.data.bytesRecorded, recpumpStatus.data.fifoDepth, recpumpStatus.data.fifoSize);
		PrintDebug(" index: %u total bytes \tfifo %u/%u\n", (HUINT32)recpumpStatus.index.bytesRecorded, recpumpStatus.index.fifoDepth, recpumpStatus.index.fifoSize);
#endif
		len = (HUINT32)recpumpStatus.data.bytesRecorded;
		VK_TASK_Sleep(10);
		if (s_bCpsPbCompleted != FALSE)
		{
			VK_TASK_Sleep(100);
			break;
		}
	}
	NEXUS_Recpump_GetStatus(stCpsIns.recPumpHandle, &recpumpStatus);
	len = (HUINT32)recpumpStatus.data.bytesRecorded;
	PrintDebug("[%s,%d] Recorded data length: %d bytes\n", __FUNCTION__, __LINE__, len);

	fd = fopen(recordfname, "rb");
	if (fd == NULL)
	{
		PrintError("can't open file:%s\n", recordfname);
		errCode = DRV_ERR;
		goto release_resource;
	}

	fseek (fd, 0, SEEK_END);
	len = ftell(fd);
	PrintDebug("[%s,%d] Recorded ts file length: %d bytes\n", __FUNCTION__, __LINE__, len);

	if (len != ulLength)
	{
		PrintError("[%s,%d] The length of decrypted data(%d) does not match with encrypted data(%d).\n\r", __FUNCTION__, __LINE__, len, ulLength);
		PrintError("[%s,%d] Please check pid channels of playback.\n\r", __FUNCTION__, __LINE__);
	}

	rewind(fd);
	size = fread(pOutData, 1, len, fd);
	if (size != len)
	{
		PrintError("[%s,%d] Error in fread() to read decrypted file\n\r", __FUNCTION__, __LINE__);
		errCode = DRV_ERR;
		fclose(fd);
		goto release_resource;
	}

	fclose(fd);

release_resource:
	P_CRYPTO_ReleaseResource(&stCpsIns);
	VK_TASK_Sleep(10);
	if (stCpsIns.pbPumpHandle != NULL)
	{
		errCode = DRV_PVR_ReleasePlaypumpHandle(stCpsIns.pbPumpHandle);
		if (errCode != DRV_OK)
		{
			PrintError("[%s,%d] Error in DRV_PVR_ReleasePlaypumpHandle()\n\r", __FUNCTION__, __LINE__);
		}
	}
	VK_SEM_Release(s_ulCpsDecryptSem);
	return errCode;
}


DI_ERR_CODE DI_CRYPT_DecryptCps(stCrypt_t *stCryptInfo, stCryptPid_t *pPid)
{
	DRV_Error		errDrv = DRV_OK;
	DI_ERR_CODE		errDi = DI_ERR_OK;

	PrintEnter();

	if ((stCryptInfo == NULL) || (pPid == NULL))
	{
		PrintError("[%s,%d]Error invalid parameter!!\n\r", __FUNCTION__, __LINE__);
		return DI_ERR_INVALID_PARAM;
	}

	if ((stCryptInfo->ulBuffSize % STREAM_ALIGN_BYTE) != 0)
	{
		PrintError("[%s,%d] The size of ulBuffSize(%d) must be aligned with %d.\n", __FUNCTION__, __LINE__, stCryptInfo->ulBuffSize, STREAM_ALIGN_BYTE);
	}

	errDrv = P_CRYPT_DecryptCps( stCryptInfo->etCryptMode,
			stCryptInfo->pSrcBuffer,
			stCryptInfo->pDstBuffer,
			stCryptInfo->ulBuffSize,
			stCryptInfo->pKey,
			stCryptInfo->ulKeySize,
			pPid
			);
	if (errDrv != DRV_OK)
	{
		PrintError("[%s,%d]Error in P_CRYPT_DecryptCps()\n\r", __FUNCTION__, __LINE__);
		errDi = DI_ERR_ERROR;
	}

	PrintExit();
	return errDi;
}
#else
DI_ERR_CODE DI_CRYPT_DecryptCps(stCrypt_t *stCryptInfo, stCryptPid_t *pPid)
{
	PrintEnter();
	UNUSED_PARAM(stCryptInfo);
	UNUSED_PARAM(pPid);

	PrintError("[%s,%d]Error, 28nm chips can only support this feature.\n\r", __FUNCTION__, __LINE__);
	PrintExit();
	return DI_ERR_OK;
}
#endif

#if defined(CONFIG_CAS_NA)
static DRV_Error P_CSDGetEncryptionData(
	unsigned char *pInputData,
	unsigned char *pOutputData,
	unsigned int uiLength,
	unsigned int isEnryption)
{
	TCsdR2RAlgorithm		Algorithm = CSD_R2R_ALGORITHM_TDES_K1K2K1;
	TCsdR2RCryptoOperationMode	Mode = CSD_R2R_CRYPTO_OPERATION_MODE_ECB;
	TCsdSize			CipheredContentKeySize;
	TCsdR2RKeyPathHandle		*pR2RKeyPathHandle;
	TCsdStatus			CsdStatus;

	TCsdUnsignedInt8		*pcrypt_input=NULL;
	TCsdUnsignedInt8		*pcrypt_output=NULL;

#if (NEXUS_VERSION < 1501)
	NEXUS_SecurityKeySlotSettings keySlotSettings;
	TCsdInitParameters		csdInit;
#endif
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
	const TCsdR2RCipheredProtectingKeys R2RCipheredProtectingKeys =
		{{0xdf, 0x72, 0xe9, 0x15, 0x9a, 0x30, 0xc6, 0x13, 0xec, 0x5b, 0x00, 0x42, 0xb8, 0xed, 0x5e, 0x0a}};

	TCsdUnsignedInt8 CipheredContentKey[16] =
		{0xdf, 0x72, 0xe9, 0x15, 0x9a, 0x30, 0xc6, 0x13, 0xec, 0x5b, 0x00, 0x42, 0xb8, 0xed, 0x5e, 0x0a};
#elif (BCHP_CHIP == 7358)
	const TCsdR2RCipheredProtectingKeys R2RCipheredProtectingKeys =
		{{0xdf, 0x72, 0xe9, 0x15, 0x9a, 0x30, 0xc6, 0x13, 0xec, 0x5b, 0x00, 0x42, 0xb8, 0xed, 0x5e, 0x0a}};

	TCsdUnsignedInt8 CipheredContentKey[16] =
		{0xdf, 0x72, 0xe9, 0x15, 0x9a, 0x30, 0xc6, 0x13, 0xec, 0x5b, 0x00, 0x42, 0xb8, 0xed, 0x5e, 0x0a};
#elif (BCHP_CHIP == 7439)
	const TCsdR2RCipheredProtectingKeys R2RCipheredProtectingKeys =
	{{0xA9, 0x32, 0x30, 0x31, 0x31, 0x4E, 0x61, 0x67, 0x72, 0x61, 0x76, 0x69, 0x73, 0x69, 0x6F, 0x6E},
	{0xA9, 0x32, 0x30, 0x31, 0x31, 0x4E, 0x61, 0x67, 0x72, 0x61, 0x76, 0x69, 0x73, 0x69, 0x6F, 0x6E}};

	TCsdUnsignedInt8 CipheredContentKey[16] =
	{0xA9, 0x32, 0x30, 0x31, 0x31, 0x4E, 0x61, 0x67, 0x72, 0x61, 0x76, 0x69, 0x73, 0x69, 0x6F, 0x6E};
#elif (BCHP_CHIP == 7584)
	const TCsdR2RCipheredProtectingKeys R2RCipheredProtectingKeys =
		{{0xdf, 0x72, 0xe9, 0x15, 0x9a, 0x30, 0xc6, 0x13, 0xec, 0x5b, 0x00, 0x42, 0xb8, 0xed, 0x5e, 0x0a}};

	TCsdUnsignedInt8 CipheredContentKey[16] =
		{0xdf, 0x72, 0xe9, 0x15, 0x9a, 0x30, 0xc6, 0x13, 0xec, 0x5b, 0x00, 0x42, 0xb8, 0xed, 0x5e, 0x0a};
#elif (BCHP_CHIP == 7552)
	TCsdR2RCipheredProtectingKeys R2RCipheredProtectingKeys =
	{{0xA9, 0x32, 0x30, 0x31, 0x31, 0x4E, 0x61, 0x67, 0x72, 0x61, 0x76, 0x69, 0x73, 0x69, 0x6F, 0x6E},
	{0xA9, 0x32, 0x30, 0x31, 0x31, 0x4E, 0x61, 0x67, 0x72, 0x61, 0x76, 0x69, 0x73, 0x69, 0x6F, 0x6E}};

	TCsdUnsignedInt8 CipheredContentKey[] =
	{0xA9, 0x32, 0x30, 0x31, 0x31, 0x4E, 0x61, 0x67, 0x72, 0x61, 0x76, 0x69, 0x73, 0x69, 0x6F, 0x6E};
#elif (BCHP_CHIP == 7362)
	TCsdR2RCipheredProtectingKeys R2RCipheredProtectingKeys =
	{{0xA9, 0x32, 0x30, 0x31, 0x31, 0x4E, 0x61, 0x67, 0x72, 0x61, 0x76, 0x69, 0x73, 0x69, 0x6F, 0x6E},
	{0xA9, 0x32, 0x30, 0x31, 0x31, 0x4E, 0x61, 0x67, 0x72, 0x61, 0x76, 0x69, 0x73, 0x69, 0x6F, 0x6E}};

	TCsdUnsignedInt8 CipheredContentKey[] =
	{0xA9, 0x32, 0x30, 0x31, 0x31, 0x4E, 0x61, 0x67, 0x72, 0x61, 0x76, 0x69, 0x73, 0x69, 0x6F, 0x6E};
#else
#error "CHIP not supported"
#endif
	PrintEnter();

	if(pInputData == NULL || pOutputData == NULL || uiLength == 0)
	{
		PrintError("[%s,%d] Buffer is NULL or Length is Zero!\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}
	NEXUS_Memory_Allocate(sizeof(TCsdR2RKeyPathHandle), NULL, (void *)&pR2RKeyPathHandle);
	BKNI_Memset(pR2RKeyPathHandle, 0x00, sizeof(TCsdR2RKeyPathHandle));
#if (NEXUS_VERSION >= 1501) || ((NEXUS_VERSION == 1401) && defined(CONFIG_CAS_NA_NOCS_3X))
	/* Get an R2R key path handle */
	CsdStatus = csdBrcmOpenR2RKeyPath(&pR2RKeyPathHandle);
	if (CSD_NO_ERROR != CsdStatus)
	{
		PrintError("[%s,%d] csdBrcmOpenR2RKeyPath failed\n", __FUNCTION__, __LINE__);
	}
#else
	pR2RKeyPathHandle->initHandle = &csdInit;

    NEXUS_Security_GetDefaultKeySlotSettings(&keySlotSettings);
	keySlotSettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;
#if defined(CONFIG_CAS_NA_NOCS_3X)
	pR2RKeyPathHandle->keyHandle = NEXUS_Security_AllocateKeySlot(&keySlotSettings);
	if(pR2RKeyPathHandle->keyHandle == NULL)
	{
		PrintError("[%s,%d] Error:Allocate Key Slot is NULL !!\n", __FUNCTION__, __LINE__);
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
					PrintError("[%s,%d] Error:EncryptDataWithSecretContentKey error!\n", __FUNCTION__, __LINE__);
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
					PrintError("[%s,%d] Error:DecryptDataWithSecretContentKey error!\n", __FUNCTION__, __LINE__);
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
					PrintError("[%s,%d] Error:EncryptDataWithSecretContentKey error!\n", __FUNCTION__, __LINE__);
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
					PrintError("[%s,%d] Error:DecryptDataWithSecretContentKey error!\n", __FUNCTION__, __LINE__);
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
}
#endif

#if !defined(CONFIG_CAS_NA)
#if defined(CONFIG_DEBUG)
static int  P_CRYPT_Print(const char *format,...)
{
	va_list args;
	va_start(args, format);
	VK_vprintf(format, args);
	va_end(args);

	return 0;
}
#endif

static int P_CRYPT_Dump(unsigned char* pucBuffer, unsigned int uiLen, unsigned int uiAlign)
{
	unsigned int uiIndex;

	HAPPY(pucBuffer);

	for (uiIndex=0 ; uiIndex<uiLen ; uiIndex++)
	{
		if (uiAlign!=0 && (uiIndex%uiAlign)==0)
			PrintInfo("\n");
		PrintInfo("%02X ", *(pucBuffer+uiIndex));
	}
	PrintInfo("\n");

	return 0;
}
#endif

#if defined(CONFIG_CAS_CX)
/*
	65nm DRV_CSD 의 P_Decrypt_LPK() 을 기반으로 작성.
	40nm BCM75831(CAS CONAX) LPK Profile 을 참고하여 설정 적용.
*/

//#define LPK_DEBUG

#define LP_CONSTANT_SIZE 	16
#define LPPK_LPK_SIZE 		16
#define IV_DATA_SIZE 		16
#define LPK_DATA_BUFFER_SIZE 	256

static DRV_Error P_CRYPT_M2mLpkEncryptionData(unsigned char *LPK_ProtectedData, unsigned char *LP_ClearData, unsigned int uiLength, unsigned char *pKeyData, unsigned int uiKeyLength, unsigned char *pIvKey, unsigned int isEnryption)
{
	NEXUS_Error  			errCode;
	NEXUS_SecurityClearKey  key;
	NEXUS_SecurityAlgorithmSettings NexusConfig;
	NEXUS_SecurityKeySlotSettings keySlotSettings;
	NEXUS_SecurityEncryptedSessionKey SessionKey;
	NEXUS_SecurityEncryptedControlWord CW;
	NEXUS_KeySlotHandle	lpKeyHandle = NULL;
	uint8_t                 *lpSrc, *lpDest;
	unsigned char           buffer[8];
	int i;
#ifdef LPK_DEBUG
	int z;
#endif
#if 0
	SHA_CTX ctx;
	unsigned char           sha1Output[20];
#endif

	NEXUS_DmaHandle    dma=NULL;
	NEXUS_DmaJobSettings jobSettings;
	NEXUS_DmaJobHandle dmaJob;
	NEXUS_DmaJobBlockSettings blockSettings;
	NEXUS_DmaJobStatus jobStatus;
	unsigned char *LPPK_LPK;
	int Cnm1, Cn, nBytesToPad;

	/*This the Constant to generate LPPK */
	unsigned char			LP_CONSTANT[LP_CONSTANT_SIZE]={0x70, 0x72, 0x69,0x65,
											 0x74, 0x61, 0x72,0x79,
											 0x43, 0x6f, 0x6e, 0x61,
											 0x78, 0x50, 0x72 ,0x6F};

	if(isEnryption)
	{
		PrintError("%s is not support Enryption\n", __FUNCTION__);
		return DRV_ERR;
	}

	if(uiKeyLength != LPPK_LPK_SIZE)
	{
		PrintError("[%s] Key Data length is Error, uiKeyLength(%d)\n", __FUNCTION__, uiKeyLength);
		return DRV_ERR;
	}

	if(uiLength > LPK_DATA_BUFFER_SIZE)
	{
		PrintError("[%s] Data length is Error, uiLength(%d)\n", __FUNCTION__, uiLength);
		return DRV_ERR;
	}

	LPPK_LPK = pKeyData;

	/*allocate key slot for LP/M2M operation*/
	NEXUS_Security_GetDefaultKeySlotSettings(&keySlotSettings);
	keySlotSettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;
	lpKeyHandle = NEXUS_Security_AllocateKeySlot(&keySlotSettings);
	if(NULL == lpKeyHandle)
	{
		PrintError("[%s] lpKeyHandle is NULL\n", __FUNCTION__);
		return DRV_ERR;
	}

	NEXUS_Security_GetDefaultAlgorithmSettings(&NexusConfig);
	NexusConfig.algorithm 			= NEXUS_SecurityAlgorithm_e3DesAba;
	NexusConfig.algorithmVar  		= NEXUS_SecurityAlgorithmVariant_eCbc;
	NexusConfig.operation 			= NEXUS_SecurityOperation_eDecrypt;
	NexusConfig.keyDestEntryType    = NEXUS_SecurityKeyType_eOdd;
	NexusConfig.terminationMode		= NEXUS_SecurityTerminationMode_eClear;
	NexusConfig.ivMode				= NEXUS_SecurityIVMode_eRegular;
	NexusConfig.solitarySelect		= NEXUS_SecuritySolitarySelect_eClear;
	NexusConfig.caVendorID			= 0x1234;
	NexusConfig.askmModuleID		= NEXUS_SecurityAskmModuleID_eModuleID_4;
	NexusConfig.otpId				= NEXUS_SecurityOtpId_eOtpVal;
	NexusConfig.key2Select			= NEXUS_SecurityKey2Select_eReserved1;

	if ( NEXUS_Security_ConfigAlgorithm(lpKeyHandle, &NexusConfig)!= NEXUS_SUCCESS)
	{
		PrintError("[%s] Error in NEXUS_Security_ConfigAlgorithm(M2M/LP key) \n", __FUNCTION__);
		NEXUS_Security_FreeKeySlot(lpKeyHandle);
		return DRV_ERR;
	}

	NEXUS_Security_GetDefaultClearKey(&key);
	key.keyEntryType = NEXUS_SecurityKeyType_eOdd;
	key.keyIVType = NEXUS_SecurityKeyIVType_eIV;
	key.keySize = IV_DATA_SIZE;
	if(NULL == pIvKey)
	{
		VK_MEM_Memset((void *)key.keyData,0,IV_DATA_SIZE);
	}
	else
	{
		VK_MEM_Memcpy((void *)key.keyData, (void *)pIvKey, IV_DATA_SIZE);
	}

#ifdef LPK_DEBUG
	PrintDebug("IV = ");
	for(z=0; z<IV_DATA_SIZE; z++)
		PrintDebug("%02X ", key.keyData[z]);
	PrintDebug("\n");
#endif

	if ( NEXUS_Security_LoadClearKey (lpKeyHandle, &key) != NEXUS_SUCCESS )
	{
		PrintError("[%s] Load video IV failed \n", __FUNCTION__);
		NEXUS_Security_FreeKeySlot(lpKeyHandle);
		return DRV_ERR;
	}

	/*GENERATE LPPK from M2M key and the Constant */
	NEXUS_Security_GetDefaultSessionKeySettings(&SessionKey);
	SessionKey.cusKeyL = 0x13;
	SessionKey.cusKeyH= 0x13;
	SessionKey.cusKeyVarL = 0x77;
	SessionKey.cusKeyVarH = 0x40;
	SessionKey.keyGenCmdID = NEXUS_SecurityKeyGenCmdID_eKeyGen;
	SessionKey.sessionKeyOp = NEXUS_SecuritySessionKeyOp_eNoProcess;
	SessionKey.bASKMMode = false;
	SessionKey.bSwapAESKey = false;
	SessionKey.keyDestIVType = NEXUS_SecurityKeyIVType_eNoIV;
	SessionKey.operationKey2 = NEXUS_SecurityOperation_eEncrypt;
	SessionKey.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CP_128_4;
	SessionKey.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL4;
	SessionKey.keyMode = NEXUS_SecurityKeyMode_eRegular;
	SessionKey.keyladderID = NEXUS_SecurityKeyladderID_eA;
	SessionKey.keyladderType = NEXUS_SecurityKeyladderType_e3Des;
	SessionKey.swizzleType = NEXUS_SecuritySwizzleType_eNone;
	SessionKey.rootKeySrc = NEXUS_SecurityRootKeySrc_eOtpKeyA;
	SessionKey.bRouteKey = false;
	SessionKey.operation = NEXUS_SecurityOperation_eDecrypt;
	SessionKey.keyEntryType = NEXUS_SecurityKeyType_eOdd;

	VK_MEM_Memcpy((void *)SessionKey.keyData, (void *)&LP_CONSTANT[0], LP_CONSTANT_SIZE);

#ifdef LPK_DEBUG
	PrintDebug("LP_CONSTANT = ");
	for(z=0; z<LP_CONSTANT_SIZE; z++)
		PrintDebug("%02X ", SessionKey.keyData[z]);
	PrintDebug("\n");
#endif

	errCode = NEXUS_Security_GenerateSessionKey( lpKeyHandle, &SessionKey);
	if (errCode != NEXUS_SUCCESS)
	{
		PrintError("[%s] NEXUS_Security_GenerateSessionKey(odd) errCode: %x\n", __FUNCTION__, errCode);
		NEXUS_Security_FreeKeySlot(lpKeyHandle);
		return DRV_ERR;
	}

	/*GENERATE LPK from LPPK(LPK) */
	NEXUS_Security_GetDefaultControlWordSettings(&CW);
	CW.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CP_128_4;
	CW.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL4;
	CW.keyMode = NEXUS_SecurityKeyMode_eRegular;
	CW.keyDestIVType = NEXUS_SecurityKeyIVType_eNoIV;
	CW.keyGenCmdID = NEXUS_SecurityKeyGenCmdID_eKeyGen;
	CW.bSwapAESKey = false;
	CW.keyladderID = NEXUS_SecurityKeyladderID_eA;
	CW.keyladderType = NEXUS_SecurityKeyladderType_e3Des;
	CW.bRouteKey = true;
	CW.operation = NEXUS_SecurityOperation_eDecrypt;
	CW.keySize = LPPK_LPK_SIZE;
	CW.keyEntryType = NEXUS_SecurityKeyType_eOdd;

	VK_MEM_Memcpy((void *)CW.keyData, (void *)LPPK_LPK, LPPK_LPK_SIZE);

#ifdef LPK_DEBUG
	PrintDebug("LPPK_LPK = ");
	for(z=0; z<LPPK_LPK_SIZE; z++)
		PrintDebug("%02X ", CW.keyData[z]);
	PrintDebug("\n");
#endif

	errCode = NEXUS_Security_GenerateControlWord(lpKeyHandle , &CW);
	if (errCode != NEXUS_SUCCESS)
	{
		PrintError("[%s] NEXUS_Security_GenerateControlWord errCode: %x\n", __FUNCTION__, errCode);
		NEXUS_Security_FreeKeySlot(lpKeyHandle);
		return DRV_ERR;
	}

	/*DECRYPT the LPK protected data*/
	dma = g_ulDmaHandle;

	errCode = NEXUS_Memory_Allocate(LPK_DATA_BUFFER_SIZE, NULL, (void *)&lpSrc);
	if (errCode != NEXUS_SUCCESS)
	{
		PrintError("[%s] NEXUS_Security_GenerateSessionKey(odd) errCode: %x\n", __FUNCTION__, errCode);
		NEXUS_Security_FreeKeySlot(lpKeyHandle);
		return DRV_ERR;
	}

	errCode = NEXUS_Memory_Allocate(LPK_DATA_BUFFER_SIZE, NULL, (void *)&lpDest);
	if (errCode != NEXUS_SUCCESS)
	{
		PrintError("[%s] NEXUS_Security_GenerateSessionKey(odd) errCode: %x\n", __FUNCTION__, errCode);
		NEXUS_Memory_Free( (void *)lpSrc);
		NEXUS_Security_FreeKeySlot(lpKeyHandle);
		return DRV_ERR;
	}

	VK_MEM_Memset((void *)lpSrc,  	0, 	LPK_DATA_BUFFER_SIZE*sizeof(unsigned char));
	VK_MEM_Memset((void *)lpDest, 	0, 	LPK_DATA_BUFFER_SIZE*sizeof(unsigned char));

	PrintDebug("[%s] --->>>encrypted LP data:\n", __FUNCTION__);
	for(i=0;i<(int)uiLength;i++)
	{
		lpSrc[i]=LPK_ProtectedData[i];
		PrintDebug("%02X ",lpSrc[i]);
	}
	PrintDebug("\n[%s] <<<---", __FUNCTION__);

	/* buffer holds original C(n-1) block */
	Cnm1 = ((uiLength / 8) - 1) * 8;	 /* index of C(n-1) block */
	Cn	 = ((uiLength / 8) * 8);		 /* index of C(n) block - cipher text stealing */
	nBytesToPad = 8 - (uiLength - Cn);
	if (nBytesToPad == 8) /* patched from brcm 6/24 */
	{
		nBytesToPad = 0;
		Cn = Cnm1;
	}

	for(i=0;i<8;i++)
		buffer[i]=(lpSrc + Cnm1)[i];

	NEXUS_DmaJob_GetDefaultSettings(&jobSettings);
	jobSettings.numBlocks 					= 1;
	jobSettings.keySlot 					= lpKeyHandle;
	jobSettings.dataFormat 					= NEXUS_DmaDataFormat_eBlock;
	jobSettings.completionCallback.callback = NULL;

	dmaJob = NEXUS_DmaJob_Create(dma, &jobSettings);
	if(NULL == dmaJob)
	{
		PrintError("[%s] NEXUS_DmaJob_Create errCode: %x\n", __FUNCTION__, errCode);
		NEXUS_Memory_Free( (void *)lpDest);
		NEXUS_Memory_Free( (void *)lpSrc);
		NEXUS_Security_FreeKeySlot(lpKeyHandle);
		return DRV_ERR;
	}

	NEXUS_DmaJob_GetDefaultBlockSettings(&blockSettings);
	blockSettings.pSrcAddr 				    = (lpSrc+Cnm1);
	blockSettings.pDestAddr 				= lpDest;
	blockSettings.blockSize 				= 8;
	blockSettings.resetCrypto 				= true;
	blockSettings.scatterGatherCryptoStart 	= true;
	blockSettings.scatterGatherCryptoEnd 	= true;
	blockSettings.cached 					= true;

	errCode = NEXUS_DmaJob_ProcessBlocks(dmaJob, &blockSettings, 1);

	for(;;)
	{
		NEXUS_DmaJob_GetStatus(dmaJob, &jobStatus);

		if(jobStatus.currentState == NEXUS_DmaJobState_eComplete )
		{
			break;
		}
		VK_TASK_Sleep(1);
	}

#ifdef LPK_DEBUG
	PrintDebug("xxxx=====> Ciphertext decryption input\n");
	for(i=0;i<8;i++)
	{
		PrintDebug("%02x ",(lpSrc+Cnm1)[i]);
	}
	PrintDebug("\n");
	PrintDebug("xxxx=====> Ciphertext decryption result\n");
	for(i=0;i<8;i++)
	{
		PrintDebug("%02x ",lpDest[i]);
	}
	PrintDebug("\n");
#endif

	/* pad C(n) to the nearest multiple of the block size using tail of the decrypted C(n-1) */
	/* and then swap the two blocks */
	for(i = 0; i < (8 - nBytesToPad); i++)
		(lpSrc + Cnm1)[i]=(lpSrc + Cn)[i];
	for(i = (8 - nBytesToPad); i < 8; i++)
		(lpSrc + Cnm1)[i] = lpDest[i];
	for(i=0;i<8;i++)
		(lpSrc + Cn)[i] = buffer[i];

	NEXUS_DmaJob_GetDefaultBlockSettings(&blockSettings);
	blockSettings.pSrcAddr 					= lpSrc;
	blockSettings.pDestAddr 				= lpDest;
	blockSettings.blockSize 				= (uiLength+nBytesToPad);
	blockSettings.resetCrypto 				= true;
	blockSettings.scatterGatherCryptoStart 	= true;
	blockSettings.scatterGatherCryptoEnd 	= true;
	blockSettings.cached 					= true;

	errCode = NEXUS_DmaJob_ProcessBlocks(dmaJob, &blockSettings, 1);

	for(;;)
	{
		NEXUS_DmaJob_GetStatus(dmaJob, &jobStatus);

		if(jobStatus.currentState == NEXUS_DmaJobState_eComplete )
		{
			break;
		}
		VK_TASK_Sleep(1);
	}

	PrintDebug("[%s] --->>>clear LP data:\n", __FUNCTION__);
	for(i=0;i<(int)uiLength;i++)
	{
		LP_ClearData[i]=lpDest[i];
		PrintDebug("%02X ",LP_ClearData[i]);
	}
	PrintDebug("\n[%s] <<<---", __FUNCTION__);

#if 0
	PrintError("Prepare SHA1 Verification...\n");
	SHA1_Init(&ctx);
	SHA1_Update(&ctx,&LP_ClearData[0],LP_DATA_SIZE-6);
	SHA1_Final(sha1Output,&ctx);

	if (VK_strncmp(&sha1Output[14], (LP_ClearData+LP_DATA_SIZE-6),6) == 0)
	{
	    PrintError("SHA1 result is good for LP data \n");
	    return  1;
	}
	else
	{
	    PrintError("SHA1 result is bad for LP data \n");
	    return -1	;
	}
#endif

	NEXUS_DmaJob_Destroy(dmaJob);
	NEXUS_Memory_Free( (void *)lpDest);
	NEXUS_Memory_Free( (void *)lpSrc);
	NEXUS_Security_FreeKeySlot(lpKeyHandle);

	return DRV_OK;
}
#endif

/* 3DES key size is 112bit, expand this key to 128bit by inserting 0 in lsb of each bytes.*/
void DRV_CRYPT_ExpandKeyLength(HUINT8 *psrcKey, HUINT8 *pdstKey)
{
	int i;
	HUINT8 tempKey[ENCRYPTION_KEY_LEN];
#if 0
	int indexA = 0, indexB = 0;
	HUINT8 AA = 0x7F, BB = 0x00;
#else
	/* Variables for fast mode */
	HUINT8 ArrayA0[ENCRYPTION_KEY_LEN] = {0,0,1,2,3,4,5,6,7,7,8,9,10,11,12,13};
	HUINT8 ArrayA1[ENCRYPTION_KEY_LEN] = {0x00,0x01,0x03,0x07,0x0F,0x1F,0x3F,0x7F,0x00,0x01,0x03,0x07,0x0F,0x1F,0x3F,0x7F};
	HUINT8 ArrayA2[ENCRYPTION_KEY_LEN] = {0,7,6,5,4,3,2,1,0,7,6,5,4,3,2,1};
	HUINT8 ArrayC0[ENCRYPTION_KEY_LEN] = {0,1,2,3,4,5,6,7,7,8,9,10,11,12,13,13};
	HUINT8 ArrayC1[ENCRYPTION_KEY_LEN] = {0xFE,0xFC,0xF8,0xF0,0xE0,0xC0,0x80,0x00,0xFE,0xFC,0xF8,0xF0,0xE0,0xC0,0x80,0x00};
	HUINT8 ArrayC2[ENCRYPTION_KEY_LEN] = {0,1,2,3,4,5,6,7,0,1,2,3,4,5,6,7};
#endif

	if (psrcKey == NULL || pdstKey == NULL)
	{
		PrintError("[%s] Invalid input param(psrcKey=0x%lx  pdstKey=0x%lx), just return.\n\r", __FUNCTION__, psrcKey, pdstKey);

		return;
	}

	for(i = 0; i < ENCRYPTION_KEY_LEN; i++)
	{
#if 0
		indexA = indexB;
		indexB += ((i % 8) != 0) ? 1 : 0;
		AA = (AA == 0x7F) ? 0x00 : ((AA << 1) | 0x01);
		BB = (BB == 0x00) ? 0xFE : (BB << 1);
		tempKey[i] = (((psrcKey[indexA] & AA) << (8 - (i % 8))) | ((psrcKey[indexB] & BB) >> (i % 8)));
#else
		/* Fast mode */
		tempKey[i] = (((psrcKey[ArrayA0[i]] & ArrayA1[i]) << ArrayA2[i]) | ((psrcKey[ArrayC0[i]] & ArrayC1[i]) >> ArrayC2[i]));
#endif
	}

	VK_MEM_Memcpy(pdstKey, tempKey, ENCRYPTION_KEY_LEN);

	return;
}

DRV_Error DRV_CRYPT_EndecryptMedia(DI_CRYPT_CryptMode eCryptMode,HUINT8* pucSrcData, HUINT32 uiSrcSize, HUINT8* pucDestData, HUINT8* pucKey, HUINT32 uiKeySize, HUINT8* pucIV, HUINT32 uiIVSize, HBOOL bEncrypt)
{
	DRV_Error dResult;
	NEXUS_Error nResult;
	NEXUS_KeySlotHandle keyHandle;
	NEXUS_SecurityClearKey ClearKey;
	NEXUS_SecurityKeySlotSettings keySettings;
	NEXUS_SecurityAlgorithmSettings algorithmSettings;
	NEXUS_DmaHandle	 hDmaHandle = NULL;

	if(pucKey == NULL || pucSrcData == NULL || pucDestData == NULL)
	{
		PrintError("[%s] Key/SrcData/DstData is NULL\n", __FUNCTION__);
		return DRV_ERR;
	}

	if(pucIV == NULL && (eCryptMode == DI_CRYPT_AES || eCryptMode == DI_CRYPT_AES_CTR))
	{
		PrintError("[%s] IV is NULL\n", __FUNCTION__);
		return DRV_ERR;
	}

/******************************
*	1. Key Slot Setting
******************************/
	NEXUS_Security_GetDefaultKeySlotSettings(&keySettings);
	keySettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;
	keyHandle = NEXUS_Security_AllocateKeySlot(&keySettings);
	if(keyHandle == NULL)
	{
		PrintError("[%s] NEXUS_Security_AllocateKeySlot failed!\n", __FUNCTION__);
		return DRV_ERR;
	}

/******************************
*	2. Algorithm Setting
******************************/
	NEXUS_Security_GetDefaultAlgorithmSettings(&algorithmSettings);
	switch (eCryptMode)
	{
		case DI_CRYPT_AES :
			algorithmSettings.algorithm 	= NEXUS_SecurityAlgorithm_eAes;
			algorithmSettings.algorithmVar 	= NEXUS_SecurityAlgorithmVariant_eCbc;
			break;
		case DI_CRYPT_AES_CTR :
			algorithmSettings.algorithm = NEXUS_SecurityAlgorithm_eAes;
			algorithmSettings.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCounter;
			break;
		default :
			PrintError("%s(%d) Not supported encryption type! (eEncType : %d)\n\r",__FUNCTION__,__LINE__,eCryptMode);
			return DRV_ERR_INVALID_PARAMETER;
	}
	if (bEncrypt == TRUE)
	{
		algorithmSettings.operation = NEXUS_SecurityOperation_eEncrypt;
	}
	else
	{
		algorithmSettings.operation = NEXUS_SecurityOperation_eDecrypt;
	}

	algorithmSettings.dest				= NEXUS_SecurityAlgorithmConfigDestination_eMem2Mem;
	algorithmSettings.terminationMode	= NEXUS_SecurityTerminationMode_eClear;
#if (NEXUS_VERSION >= 1304)
	algorithmSettings.keyDestEntryType	= NEXUS_SecurityKeyType_eClear;
#else
	algorithmSettings.keyDestEntryType 	= NEXUS_SecurityKeyType_eOdd;
#endif
	nResult = NEXUS_Security_ConfigAlgorithm(keyHandle, &algorithmSettings);
	if (nResult != NEXUS_SUCCESS)
	{
		PrintError("[%s] NEXUS_Security_ConfigAlgorithm failed!\n", __FUNCTION__);
		return DRV_ERR;
	}

/******************************
*	3. Clear Key Setting
******************************/
	NEXUS_Security_GetDefaultClearKey(&ClearKey);
	ClearKey.keySize = uiKeySize;
#if (NEXUS_VERSION >= 1304)
	ClearKey.keyEntryType = NEXUS_SecurityKeyType_eClear;
#else
	ClearKey.keyEntryType = NEXUS_SecurityKeyType_eOdd;
#endif
	VK_MEM_Memcpy(ClearKey.keyData, pucKey, ClearKey.keySize);

	nResult = NEXUS_Security_LoadClearKey(keyHandle, &ClearKey);
	if (nResult != NEXUS_SUCCESS)
	{
		PrintError("[%s] NEXUS_Security_LoadClearKey failed\n",__FUNCTION__);
		return DRV_ERR;
	}

/******************************
*	4. Initialization Vector Setting
******************************/
	NEXUS_Security_GetDefaultClearKey(&ClearKey);
	ClearKey.keySize = uiIVSize;
#if (NEXUS_VERSION >= 1304)
	ClearKey.keyEntryType = NEXUS_SecurityKeyType_eClear;
#else
	ClearKey.keyEntryType = NEXUS_SecurityKeyType_eOdd;
#endif
	ClearKey.keyIVType = NEXUS_SecurityKeyIVType_eIV;
	VK_MEM_Memcpy(ClearKey.keyData, pucIV, ClearKey.keySize);

	nResult = NEXUS_Security_LoadClearKey(keyHandle, &ClearKey);
	if (nResult != NEXUS_SUCCESS)
	{
		PrintError("[%s] NEXUS_Security_LoadClearKey failed\n",__FUNCTION__);
		return DRV_ERR;
	}

/******************************
*	5. Decrypt using DMA
******************************/
	hDmaHandle = NEXUS_Dma_Open(NEXUS_ANY_ID, NULL);
	if (!hDmaHandle) {
		PrintError("[%s] Error! can't open dma channel\n", __FUNCTION__);
		return DRV_ERR;
	}
	dResult = P_CRYPT_CopyDma(DI_CRYPT_BLOCK, pucSrcData, pucDestData, uiSrcSize, keyHandle, hDmaHandle);
	if(dResult != DRV_OK)
	{
		NEXUS_Dma_Close(hDmaHandle);
		PrintError("[%s] P_CRYPT_CopyDma failed!\n",__FUNCTION__);
		return DRV_ERR;
	}
	NEXUS_Dma_Close(hDmaHandle);
	NEXUS_Security_FreeKeySlot(keyHandle);
	return DRV_OK;
}

DRV_Error DRV_CRYPT_Encrypt(stCrypt_t *stCryptInfo)
{
	if (stCryptInfo == NULL)
	{
		PrintError("[%s] NULL pointer\n",__FUNCTION__);

		return DI_ERR_INVALID_PARAM;
	}
	else
	{
		return (DRV_Error)P_CRYPT_EncryptData(stCryptInfo->etCryptKey,
												stCryptInfo->etCryptData,
												stCryptInfo->pSrcBuffer,
												stCryptInfo->pDstBuffer,
												stCryptInfo->ulBuffSize,
												stCryptInfo->pKey,
												stCryptInfo->ulKeySize,
												stCryptInfo->etCryptMode,
												stCryptInfo->pIvKey,
												TRUE);
	}
}

DRV_Error DRV_CRYPT_Decrypt(stCrypt_t *stCryptInfo)
{
	if (stCryptInfo == NULL)
	{
		PrintError("[%s] NULL pointer\n",__FUNCTION__);

		return DI_ERR_INVALID_PARAM;
	}
	else
	{
		return (DRV_Error)P_CRYPT_EncryptData(stCryptInfo->etCryptKey,
												stCryptInfo->etCryptData,
												stCryptInfo->pSrcBuffer,
												stCryptInfo->pDstBuffer,
												stCryptInfo->ulBuffSize,
												stCryptInfo->pKey,
												stCryptInfo->ulKeySize,
												stCryptInfo->etCryptMode,
												stCryptInfo->pIvKey,
												FALSE);
	}
}

void *DRV_CRYPT_MemAlloc(HUINT32 size)
{
	NEXUS_Error	nexusError;
	void		*pSrcBuf;

	nexusError = NEXUS_Memory_Allocate(size, NULL, (void **)&pSrcBuf);
	if ((pSrcBuf == NULL) || (nexusError != NEXUS_SUCCESS))
	{
		PrintError("[%s] NEXUS_Memory_Allocate error(0x%x) or pSrcBuf is NULL\n", __FUNCTION__, nexusError);

		return NULL;
	}

	return pSrcBuf;
}

void DRV_CRYPT_MemFree(void *p)
{
	if (p == NULL)
	{
		PrintError("[%s] NULL pointer\n",__FUNCTION__);

		return;
	}

	NEXUS_Memory_Free(p);
}

DI_ERR_CODE DI_CRYPT_Encrypt(stCrypt_t *stCryptInfo)
{
#if defined(CONFIG_CAS_NA)
	DRV_Error errorValue = DRV_ERR;

	if ( stCryptInfo->etCryptKey == DI_CRYPT_INTERNAL_CAS_KEY )
	{
		errorValue = P_CSDGetEncryptionData(stCryptInfo->pSrcBuffer, stCryptInfo->pDstBuffer, stCryptInfo->ulBuffSize, 1);
		if ( errorValue == DRV_OK )
		{
			return DI_ERR_OK;
		}
		else
		{
			return DI_ERR_ERROR;
		}
	}
	else
#endif
	return (DI_ERR_CODE)DRV_CRYPT_Encrypt(stCryptInfo);
}

DI_ERR_CODE DI_CRYPT_Decrypt(stCrypt_t *stCryptInfo)
{
#if defined(CONFIG_CAS_NA)
	DRV_Error errorValue = DRV_ERR;
	if ( stCryptInfo->etCryptKey == DI_CRYPT_INTERNAL_CAS_KEY )
	{
		errorValue = P_CSDGetEncryptionData(stCryptInfo->pSrcBuffer, stCryptInfo->pDstBuffer, stCryptInfo->ulBuffSize, 0);
		if ( errorValue == DRV_OK )
		{
			return DI_ERR_OK;
		}
		else
		{
			return DI_ERR_ERROR;
		}
	}
	else
#elif defined(CONFIG_CAS_CX)
	if ( stCryptInfo->etCryptKey == DI_CRYPT_INTERNAL_M2M_LPK_KEY )
	{
		if(P_CRYPT_M2mLpkEncryptionData(stCryptInfo->pSrcBuffer,
										stCryptInfo->pDstBuffer,
										stCryptInfo->ulBuffSize,
										stCryptInfo->pKey,
										stCryptInfo->ulKeySize,
										stCryptInfo->pIvKey,
										0) == DRV_OK)
		{
			return DI_ERR_OK;
		}
		else
		{
			return DI_ERR_ERROR;
		}
	}
	else
#endif
	{
		return (DI_ERR_CODE)DRV_CRYPT_Decrypt(stCryptInfo);
	}
}

void *DI_CRYPT_MemAlloc(HUINT32 size)
{
	return (void *)DRV_CRYPT_MemAlloc(size);
}

void DI_CRYPT_MemFree(void *p)
{
	DRV_CRYPT_MemFree(p);
}

#if defined(CONFIG_SHARED_COMPILE)
DI_ERR_CODE DI_CRYPT_Init(void)
{
	DI_ERR_CODE di_error = DI_ERR_ERROR;

	(void)NEXUS_Platform_Join();
	DONE_ERR(DRV_CRYPT_Init() != DRV_OK);

	di_error = DI_ERR_OK;
done:
	return di_error ;
}
#endif

#define DES_BUFFER_SIZE	256

const unsigned char ucTestData[DES_BUFFER_SIZE] =
{
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
	0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00,
	0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00,
	0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11, 0x10, 0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11, 0x10,
	0x1F, 0x1E, 0x1D, 0x1C, 0x1B, 0x1A, 0x19, 0x18, 0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11, 0x10,

	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
};

#if defined(CONFIG_CAS_NA)
/* HostKey - TDES - ECB */
const TCsdUnsignedInt8 r2r1InEncTdesEcb[8] =
                               {0x4e, 0x6f, 0x77, 0x20, 0x69, 0x73, 0x20, 0x74};

const TCsdUnsignedInt8 r2r1HostKeyEncTdesEcb[16] =
                               {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
                                0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x01};

const TCsdUnsignedInt8 r2r1OutEncTdesEcb[8] =
                               {0x03, 0xE6, 0x9F, 0x5B, 0xFA, 0x58, 0xEB, 0x42};

const TCsdUnsignedInt8 r2r1InDecTdesEcb[8] =
                               {0x4e, 0x6f, 0x77, 0x20, 0x69, 0x73, 0x20, 0x74};

const TCsdUnsignedInt8 r2r1HostKeyDecTdesEcb[16] =
                               {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
                                0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x01};

const TCsdUnsignedInt8 r2r1OutDecTdesEcb[8] =
                               {0x69, 0x96, 0xC8, 0xFA, 0x47, 0xA2, 0xAB, 0xEB};
#endif

void DRV_CRYPT_Test(DI_CRYPT_CryptMode cryptMode, DI_CRYPT_CryptKeyType cryptKey)
{
#if defined(CONFIG_CAS_NA)
	int i;
	TCsdUnsignedInt8 *dataIn;
	TCsdUnsignedInt8 *dataOut;
	TCsdR2RKeyPathHandle *pxR2RKeyPathHandle;
#if (NEXUS_VERSION < 1501)
	TCsdInitParameters csdInit;
#endif
	TCsdStatus	CsdStatus;
	HAPPY(cryptMode);
	HAPPY(cryptKey);

	NEXUS_Memory_Allocate(sizeof(TCsdR2RKeyPathHandle), NULL, (void *)&pxR2RKeyPathHandle);
	BKNI_Memset(pxR2RKeyPathHandle, 0x00, sizeof(TCsdR2RKeyPathHandle));
	
#if (NEXUS_VERSION >= 1501) || ((NEXUS_VERSION == 1401) && defined(CONFIG_CAS_NA_NOCS_3X))
	/* Get an R2R key path handle */
	CsdStatus = csdBrcmOpenR2RKeyPath(&pxR2RKeyPathHandle);
	if (CSD_NO_ERROR != CsdStatus)
	{
		PrintInfo("%s: csdBrcmOpenR2RKeyPath failed\n", __FUNCTION__);
	}
#else
	pxR2RKeyPathHandle->initHandle = &csdInit;
#if defined(CONFIG_CAS_NA_NOCS_3X)
	pxR2RKeyPathHandle->keyHandle = NULL;
#endif
#endif
	NEXUS_Memory_Allocate(sizeof(unsigned char)*8, NULL, (void *)&dataIn);
	NEXUS_Memory_Allocate(sizeof(unsigned char)*8, NULL, (void *)&dataOut);

	BKNI_Memcpy(dataIn, r2r1InEncTdesEcb, sizeof(TCsdUnsignedInt8)*8);

	PrintInfo("dataIn = ");
	for(i = 0; i < 8; i++)
		PrintInfo("0x%02x ", *(dataIn + i));
	PrintInfo("\n");

	for(i = 0; i < 10000; i++)
	{
		csdEncryptDataWithClearTextHostKey(
			CSD_R2R_ALGORITHM_TDES_K1K2K1,
			CSD_R2R_CRYPTO_OPERATION_MODE_ECB,
			r2r1HostKeyEncTdesEcb, 16,
			NULL, 0, FALSE,
			pxR2RKeyPathHandle,
			dataIn,
			dataOut,
			8
			);

		BKNI_Memcpy(dataIn, dataOut, sizeof(TCsdUnsignedInt8) * 8);
	}

	PrintInfo("dataOut = ");
	for(i = 0; i < 8; i++)
		PrintInfo("0x%02x ", *(dataOut + i));
	PrintInfo("\n");

	if (VK_memcmp(dataOut, r2r1OutEncTdesEcb, 8))
			PrintInfo("HostKey TDES ECB: Encryption Process Failed\n");
	else
		PrintInfo("HostKey TDES ECB: Encryption Process Successful\n");

	BKNI_Memcpy(dataIn, r2r1InDecTdesEcb, sizeof(TCsdUnsignedInt8) * 8);

	PrintInfo("dataIn = ");
	for(i = 0; i < 8; i++)
		PrintInfo("0x%02x ", *(dataIn + i));
	PrintInfo("\n");

	for(i = 0; i < 10000; i++)
	{
		csdDecryptDataWithClearTextHostKey(
			CSD_R2R_ALGORITHM_TDES_K1K2K1,
			CSD_R2R_CRYPTO_OPERATION_MODE_ECB,
			r2r1HostKeyDecTdesEcb, 16,
			NULL, 0, FALSE,
			pxR2RKeyPathHandle,
			dataIn,
			dataOut,
			8
			);

		BKNI_Memcpy(dataIn, dataOut, sizeof(TCsdUnsignedInt8)*8);
	}

	PrintInfo("dataOut = ");
	for(i = 0; i < 8; i++)
		PrintInfo("0x%02x ", *(dataOut + i));
	PrintInfo("\n");

	if (VK_memcmp(dataOut, r2r1OutDecTdesEcb, 8))
		PrintInfo("HostKey TDES ECB: Decrytpion Process Failed\n");
	else
		PrintInfo("HostKey TDES ECB: Decrytpion Process Successful\n");

#if defined(CONFIG_CAS_NA_NOCS_3X)
	#if (NEXUS_VERSION >= 1501) || (NEXUS_VERSION == 1401)
		if (pxR2RKeyPathHandle)
			csdBrcmCloseR2RKeyPath(pxR2RKeyPathHandle);
	#endif
#else
	NEXUS_Memory_Free(pxR2RKeyPathHandle);
#endif
	NEXUS_Memory_Free(dataOut);
	NEXUS_Memory_Free(dataIn);

#else
	unsigned char	pucKey[ENCRYPTION_KEY_LEN] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
	stCrypt_t		CryptInfo;
	DI_ERR_CODE		error;

	if (cryptMode == DI_CRYPT_AES_CBC_MAC)
		P_CRYPT_Print("*** AES-CBC-MAC Encryption Test : Original ***\n");
	else
		P_CRYPT_Print("*** Encryption/Decryption Test : Original ***\n");

	P_CRYPT_Dump(ucTestData, DES_BUFFER_SIZE, 16);
	P_CRYPT_Print("\n");

	VK_MEM_Memset((void *)&CryptInfo, 0, sizeof(stCrypt_t));
	if ((cryptMode == DI_CRYPT_TDES) && (cryptKey == DI_CRYPT_USER_KEY))
	{
		P_CRYPT_Print("*** DI_CRYPT_TDES, DI_CRYPT_USER_KEY ***\n");
		CryptInfo.pKey = pucKey;
	}
	else if ((cryptMode == DI_CRYPT_AES_CBC_MAC) && (cryptKey == DI_CRYPT_DEFAULT_SYSTEM_KEY))
	{
		P_CRYPT_Print("*** DI_CRYPT_AES_CBC_MAC, DI_CRYPT_DEFAULT_SYSTEM_KEY ***\n");
		CryptInfo.pKey = NULL;
	}
	else if ((cryptMode == DI_CRYPT_AES) && (cryptKey == DI_CRYPT_USER_KEY))
	{
		P_CRYPT_Print("*** DI_CRYPT_AES, DI_CRYPT_USER_KEY ***\n");
		CryptInfo.pKey = pucKey;
	}
	else if ((cryptMode == DI_CRYPT_TDES) && (cryptKey == DI_CRYPT_DEFAULT_SYSTEM_KEY))
	{
		P_CRYPT_Print("*** DI_CRYPT_TDES, DI_CRYPT_DEFAULT_SYSTEM_KEY ***\n");
		CryptInfo.pKey = NULL;
	}
	else if ((cryptMode == DI_CRYPT_AES) && (cryptKey == DI_CRYPT_DEFAULT_SYSTEM_KEY))
	{
		P_CRYPT_Print("*** DI_CRYPT_AES, DI_CRYPT_DEFAULT_SYSTEM_KEY ***\n");
		CryptInfo.pKey = NULL;
	}
	else
	{
		P_CRYPT_Print("*** NOT defined crypt test ***\n");

		return;
	}

	CryptInfo.etCryptMode = cryptMode;
	CryptInfo.etCryptKey = cryptKey;
	CryptInfo.ulKeySize = ENCRYPTION_KEY_LEN;
	CryptInfo.pSrcBuffer = DI_CRYPT_MemAlloc(DES_BUFFER_SIZE);
	if (CryptInfo.pSrcBuffer == NULL)
	{
		P_CRYPT_Print("Error DI_CRYPT_MemAlloc pSrcBuffer NULL\n");

		return;
	}
	VK_MEM_Memcpy((void *)CryptInfo.pSrcBuffer, (void *)ucTestData, DES_BUFFER_SIZE);

	CryptInfo.pDstBuffer = DI_CRYPT_MemAlloc(DES_BUFFER_SIZE);
	if (CryptInfo.pDstBuffer == NULL)
	{
		P_CRYPT_Print("Error DI_CRYPT_MemAlloc pDstBuffer NULL\n");
		DI_CRYPT_MemFree(CryptInfo.pSrcBuffer);

		return;
	}

	CryptInfo.ulBuffSize = DES_BUFFER_SIZE;
	error = DI_CRYPT_Encrypt(&CryptInfo);
	if (error != DI_ERR_OK)
	{
		P_CRYPT_Print("Error DI_CRYPT_Encrypt\n");
		DI_CRYPT_MemFree(CryptInfo.pSrcBuffer);
		DI_CRYPT_MemFree(CryptInfo.pDstBuffer);

		return;
	}

	if (cryptMode == DI_CRYPT_AES_CBC_MAC)
		P_CRYPT_Print("*** AES-CBC-MAC Encryption Test : Encryption ***\n");
	else
		P_CRYPT_Print("*** Encryption/Decryption Test : Encryption ***\n");

	P_CRYPT_Dump(CryptInfo.pDstBuffer, DES_BUFFER_SIZE, 16);
	P_CRYPT_Print("\n");

	//if (cryptMode != DI_CRYPT_AES_CBC_MAC)
	{
		if ((cryptMode == DI_CRYPT_TDES) && (cryptKey == DI_CRYPT_USER_KEY))
		{
			P_CRYPT_Print("*** DI_CRYPT_TDES, DI_CRYPT_USER_KEY ***\n");
			CryptInfo.pKey = pucKey;
		}
		else if ((cryptMode == DI_CRYPT_AES) && (cryptKey == DI_CRYPT_USER_KEY))
		{
			P_CRYPT_Print("*** DI_CRYPT_AES, DI_CRYPT_USER_KEY ***\n");
			CryptInfo.pKey = pucKey;
		}
		else if ((cryptMode == DI_CRYPT_TDES) && (cryptKey == DI_CRYPT_DEFAULT_SYSTEM_KEY))
		{
			P_CRYPT_Print("*** DI_CRYPT_TDES, DI_CRYPT_DEFAULT_SYSTEM_KEY ***\n");
			CryptInfo.pKey = NULL;
		}
		else if ((cryptMode == DI_CRYPT_AES) && (cryptKey == DI_CRYPT_DEFAULT_SYSTEM_KEY))
		{
			P_CRYPT_Print("*** DI_CRYPT_AES, DI_CRYPT_DEFAULT_SYSTEM_KEY ***\n");
			CryptInfo.pKey = NULL;
		}
		else if ((cryptMode == DI_CRYPT_AES_CBC_MAC) && (cryptKey == DI_CRYPT_DEFAULT_SYSTEM_KEY))
		{
			P_CRYPT_Print("*** DI_CRYPT_AES_CBC_MAC, DI_CRYPT_DEFAULT_SYSTEM_KEY ***\n");
			CryptInfo.pKey = NULL;
		}
		else
		{
			P_CRYPT_Print("*** NOT defined crypt test ***\n");

			return;
		}

		CryptInfo.etCryptMode = cryptMode;
		CryptInfo.etCryptKey = cryptKey;

		CryptInfo.ulKeySize = ENCRYPTION_KEY_LEN;
		VK_MEM_Memcpy(CryptInfo.pSrcBuffer, CryptInfo.pDstBuffer, DES_BUFFER_SIZE);
		VK_MEM_Memset(CryptInfo.pDstBuffer, 0, DES_BUFFER_SIZE);
		CryptInfo.ulBuffSize = DES_BUFFER_SIZE;
		error = DI_CRYPT_Decrypt(&CryptInfo);
		if (error != DI_ERR_OK)
		{
			P_CRYPT_Print("Error DI_CRYPT_Decrypt\n");
			DI_CRYPT_MemFree(CryptInfo.pSrcBuffer);
			DI_CRYPT_MemFree(CryptInfo.pDstBuffer);

			return;
		}

		P_CRYPT_Print("*** Encryption/Decryption Test : Descryption ***\n");
		P_CRYPT_Dump(CryptInfo.pDstBuffer, DES_BUFFER_SIZE, 16);
		if (VK_memcmp((void *)ucTestData, (void *)CryptInfo.pDstBuffer, DES_BUFFER_SIZE))
		{
			P_CRYPT_Print("*** Encryption/Decryption Test : FAIL\n");
		}
		else
		{
			P_CRYPT_Print("*** Encryption/Decryption Test : PASS\n");
		}
		DI_CRYPT_MemFree(CryptInfo.pSrcBuffer);
		DI_CRYPT_MemFree(CryptInfo.pDstBuffer);
		P_CRYPT_Print("\n");
	}
#endif
}

#if defined(CONFIG_PVR_SECURITY_ENGINE_INLINE_CRYPTO)
#define P_CRYPT_CHUNK_SIZE	(3*1024*1024)	//(192*(4*1024))

DRV_Error DRV_CRYPT_TestCps(void)
{
	DI_ERR_CODE     errDi = DI_ERR_OK;
	stCrypt_t		stCryptInfo;
	stCryptPid_t	pPid;
	static HUINT8	pucKey[ENCRYPTION_KEY_LEN] = {0xC7, 0x07, 0xB5, 0xC9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; //{0x0, 0x10, 0x32, 0x54, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
	FILE			*readfp = NULL;
	FILE			*writefp = NULL;
	HUINT32			ulReadLen, ulWriteLen, ulWriteTotal, ulReadTotal, ulFileSize;
	HLONG			lFileSize;
	const char *readfname = "/mnt/hd2/My Video/4_16625_101_20140924_1952_31_00217893_0.ts";
	const char *writefname = "/mnt/hd2/4_16625_101_20140924_1952_31_00217893_0_ca_decrypt.ts";
	static HBOOL	bEOF = FALSE;

	ulWriteTotal = ulReadTotal = 0;
	VK_MEM_Memset(&stCryptInfo, 0, sizeof(stCryptInfo));
	VK_MEM_Memset(&pPid, 0xFF, sizeof(pPid));

	stCryptInfo.etCryptMode = DI_CRYPT_TDES;
	stCryptInfo.etCryptKey = DI_CRYPT_USER_KEY;
	stCryptInfo.etCryptData = DI_CRYPT_STREAM;
	stCryptInfo.pKey = pucKey;
	stCryptInfo.ulKeySize = ENCRYPTION_KEY_LEN;

	stCryptInfo.pSrcBuffer = DI_CRYPT_MemAlloc(P_CRYPT_CHUNK_SIZE);
	if (stCryptInfo.pSrcBuffer == NULL)
	{
		PrintError("[%s,%d] Error in DI_CRYPT_MemAlloc() to read\n\r", __FUNCTION__, __LINE__);
		goto error;
	}

	stCryptInfo.pDstBuffer = DI_CRYPT_MemAlloc(P_CRYPT_CHUNK_SIZE);
	if (stCryptInfo.pDstBuffer == NULL)
	{
		PrintError("[%s,%d] Error in DI_CRYPT_MemAlloc() to write\n\r", __FUNCTION__, __LINE__);
		goto error;
	}

	pPid.eVideoCodec = eDI_CRYPT_VideoCodec_Mpeg2;
	pPid.usPid[eDI_CRYPT_PIDTYPE_VIDEO] = 0x100;
	pPid.usPid[eDI_CRYPT_PIDTYPE_AUDIO_MAIN] = 0x110;
	pPid.usPid[eDI_CRYPT_PIDTYPE_AUDIO_SUB] = 0x111;
	pPid.usPid[eDI_CRYPT_PIDTYPE_PCR] = 0x1ff; //pcr
	pPid.usPid[eDI_CRYPT_PIDTYPE_PES] = 0x130;
	pPid.usPid[eDI_CRYPT_PIDTYPE_PES+1] = 0x138;
	pPid.usPid[eDI_CRYPT_PIDTYPE_PES+2] = 0x0; //pat
	pPid.usPid[eDI_CRYPT_PIDTYPE_PES+3] = 0x1f0; //pmt
	pPid.usPid[eDI_CRYPT_PIDTYPE_PES+4] = 0x140; //dsmcc
	pPid.usPid[eDI_CRYPT_PIDTYPE_PES+5] = 0x160; //dsmcc
	pPid.usPid[eDI_CRYPT_PIDTYPE_PES+6] = 0x161; //dsmcc
	pPid.usPid[eDI_CRYPT_PIDTYPE_PES+7] = 0x162; //dsmcc
	pPid.usPid[eDI_CRYPT_PIDTYPE_PES+8] = 0x170; //dsmcc
	pPid.usPid[eDI_CRYPT_PIDTYPE_PES+9] = 0x171; //dsmcc
	pPid.usPid[eDI_CRYPT_PIDTYPE_PES+10] = 0x1f;

	readfp = fopen(readfname, "rb");
	if (readfp == NULL)
	{
		PrintError("fopen error %s\n\r", readfname);
		goto error;
	}

	writefp = fopen(writefname, "wb");
	if (writefp == NULL)
	{
		PrintError("fopen error %s\n\r", writefname);
		goto error;
	}

	fseek(readfp, 0, SEEK_END);
	lFileSize = ftell(readfp);
	if (lFileSize < 0)
	{
		ulFileSize = 0;
	}
	else
	{
		ulFileSize = (HUINT32)lFileSize;
	}
	rewind(readfp);
	PrintDebug("[%s,%d] Total size of decrypt file: %d\n", __FUNCTION__, __LINE__, ulFileSize);

	while(ulReadTotal < ulFileSize)
	{
		ulReadLen = fread(stCryptInfo.pSrcBuffer, 1, P_CRYPT_CHUNK_SIZE, readfp);
		if (ulReadLen < P_CRYPT_CHUNK_SIZE)
		{
			if (feof(readfp) != 0)
			{
				PrintError("[%s,%d] END OF FILE in fread()\n\r", __FUNCTION__, __LINE__);
				bEOF = TRUE;
			}
			else
			{
				PrintError("[%s,%d] Error in fread()\n\r", __FUNCTION__, __LINE__);
				break;
			}
		}

		stCryptInfo.ulBuffSize = ulReadLen;
		errDi = DI_CRYPT_DecryptCps(&stCryptInfo, &pPid);
		if (errDi != DI_ERR_OK)
		{
			PrintError("[%s,%d] Error in DI_CRYPT_DecryptCps()\n\r", __FUNCTION__, __LINE__);
			break;
		}

		VK_TASK_Sleep(10);

		ulWriteLen = fwrite(stCryptInfo.pDstBuffer, 1, stCryptInfo.ulBuffSize, writefp);
		if (ulWriteLen < stCryptInfo.ulBuffSize)
		{
			PrintError("[%s,%d] Error in fwrite()\n\r", __FUNCTION__, __LINE__);
		}

		ulReadTotal += ulReadLen;
		ulWriteTotal += ulWriteLen;

		PrintError("while() Read total: %d, Write total: %d\n", ulReadTotal, ulWriteTotal);

		if (bEOF != FALSE)
			break;
	}
	PrintDebug("END Read total: %d, Write total: %d\n", ulReadTotal, ulWriteTotal);

error:
	if (stCryptInfo.pSrcBuffer != NULL)
		DI_CRYPT_MemFree(stCryptInfo.pSrcBuffer);

	if (stCryptInfo.pDstBuffer != NULL)
		DI_CRYPT_MemFree(stCryptInfo.pDstBuffer);

	if (readfp != NULL)
		fclose(readfp);

	if (writefp != NULL)
		fclose(writefp);

	return DRV_OK;
}

DRV_Error DRV_CRYPT_TestM2m(void)
{
	DI_ERR_CODE     errDi = DI_ERR_OK;
	stCrypt_t		stCryptInfo;
	static HUINT8	pucKey[ENCRYPTION_KEY_LEN] = {0xC7, 0x07, 0xB5, 0xC9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; //{0x0, 0x10, 0x32, 0x54, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
	FILE			*readfp = NULL;
	FILE			*writefp = NULL;
	HUINT32			ulReadLen, ulWriteLen, ulWriteTotal, ulReadTotal, ulFileSize;
	HLONG			lFileSize;
	const char *readfname = "/mnt/hd2/My Video/4_16625_101_20140924_1952_31_00217893_0.ts";
	const char *writefname = "/mnt/hd2/4_16625_101_20140924_1952_31_00217893_0_m2m_decrypt.ts";
	static HBOOL	bEOF = FALSE;

	ulWriteTotal = ulReadTotal = 0;
	VK_MEM_Memset(&stCryptInfo, 0, sizeof(stCryptInfo));

	stCryptInfo.etCryptMode = DI_CRYPT_TDES;
	stCryptInfo.etCryptKey = DI_CRYPT_USER_KEY;
	stCryptInfo.etCryptData = DI_CRYPT_STREAM;
	stCryptInfo.pKey = pucKey;
	stCryptInfo.ulKeySize = ENCRYPTION_KEY_LEN;
	stCryptInfo.pSrcBuffer = DI_CRYPT_MemAlloc(P_CRYPT_CHUNK_SIZE);
	if (stCryptInfo.pSrcBuffer == NULL)
	{
		PrintError("[%s,%d] Error in DI_CRYPT_MemAlloc() to read\n\r", __FUNCTION__, __LINE__);
		goto error;
	}

	stCryptInfo.pDstBuffer = DI_CRYPT_MemAlloc(P_CRYPT_CHUNK_SIZE);
	if (stCryptInfo.pDstBuffer == NULL)
	{
		PrintError("[%s,%d] Error in DI_CRYPT_MemAlloc() to write\n\r", __FUNCTION__, __LINE__);
		goto error;
	}

	readfp = fopen(readfname, "rb");
	if (readfp == NULL)
	{
		PrintError("fopen error %s\n\r", readfname);
		goto error;
	}

	writefp = fopen(writefname, "wb");
	if (writefp == NULL)
	{
		PrintError("fopen error %s\n\r", writefname);
		goto error;
	}

	fseek(readfp, 0, SEEK_END);
	lFileSize = ftell(readfp);
	if (lFileSize < 0)
	{
		ulFileSize = 0;
	}
	else
	{
		ulFileSize = (HUINT32)lFileSize;
	}

	rewind(readfp);
	PrintDebug("[%s,%d] Total size of decrypt file: %d\n", __FUNCTION__, __LINE__, ulFileSize);

	while(ulReadTotal < ulFileSize)
	{
		ulReadLen = fread(stCryptInfo.pSrcBuffer, 1, P_CRYPT_CHUNK_SIZE, readfp);
		if (ulReadLen < P_CRYPT_CHUNK_SIZE)
		{
			if (feof(readfp) != 0)
			{
				PrintError("[%s,%d] END OF FILE in fread()\n\r", __FUNCTION__, __LINE__);
				bEOF = TRUE;
			}
			else
			{
				PrintError("[%s,%d] Error in fread()\n\r", __FUNCTION__, __LINE__);
				break;
			}
		}

		if ((ulReadLen % STREAM_ALIGN_BYTE) != 0)
		{
			ulReadLen = ulReadLen - (ulReadLen % STREAM_ALIGN_BYTE);
		}

		stCryptInfo.ulBuffSize = ulReadLen;
		errDi = DI_CRYPT_Decrypt(&stCryptInfo);
		if (errDi != DI_ERR_OK)
		{
			PrintError("[%s,%d] Error in DI_CRYPT_Decrypt()\n\r", __FUNCTION__, __LINE__);
			break;
		}

		VK_TASK_Sleep(10);

		ulWriteLen = fwrite(stCryptInfo.pDstBuffer, 1, stCryptInfo.ulBuffSize, writefp);
		if (ulWriteLen < stCryptInfo.ulBuffSize)
		{
			PrintError("[%s,%d] Error in fwrite()\n\r", __FUNCTION__, __LINE__);
		}

		ulReadTotal += ulReadLen;
		ulWriteTotal += ulWriteLen;

		PrintError("while() Read total: %d, Write total: %d\n", ulReadTotal, ulWriteTotal);

		if (bEOF != FALSE)
			break;
	}
	PrintDebug("END Read total: %d, Write total: %d\n", ulReadTotal, ulWriteTotal);

error:
	if (stCryptInfo.pSrcBuffer != NULL)
		DI_CRYPT_MemFree(stCryptInfo.pSrcBuffer);

	if (stCryptInfo.pDstBuffer != NULL)
		DI_CRYPT_MemFree(stCryptInfo.pDstBuffer);

	if (readfp != NULL)
		fclose(readfp);

	if (writefp != NULL)
		fclose(writefp);

	return DRV_OK;
}


#define P_CRYPT_TS_ALIGN	192
#define P_CRYPT_TS_NUM		4096
#define P_CRYPT_TS_SIZE		(P_CRYPT_TS_ALIGN*P_CRYPT_TS_NUM)

DRV_Error DRV_CRYPT_TestClearTSC(void)
{
	FILE			*readfp = NULL;
	FILE			*writefp = NULL;
	HUINT32			ulReadLen, ulWriteLen, ulWriteTotal, ulReadTotal, ulFileSize, i;
	HLONG			lFileSize;
	const char *readfname = "/mnt/hd2/4_16625_101_20140924_1544_48_00169757_0.ts";
	const char *writefname = "/mnt/hd2/4_16625_101_20140924_1544_48_00169757_0_ClearTSC.ts";
	static HBOOL	bEOF = FALSE;
	HUINT8	*pReadBuf = NULL;

	ulWriteTotal = ulReadTotal = 0;

	pReadBuf = DI_CRYPT_MemAlloc(P_CRYPT_TS_SIZE);
	if (pReadBuf == NULL)
	{
		PrintError("[%s,%d] Error in DI_CRYPT_MemAlloc() to read\n\r", __FUNCTION__, __LINE__);
		goto error;
	}

	readfp = fopen(readfname, "rb");
	if (readfp == NULL)
	{
		PrintError("fopen error %s\n\r", readfname);
		goto error;
	}

	writefp = fopen(writefname, "wb");
	if (writefp == NULL)
	{
		PrintError("fopen error %s\n\r", writefname);
		goto error;
	}

	fseek(readfp, 0, SEEK_END);
	lFileSize = ftell(readfp);
	if (lFileSize < 0)
	{
		ulFileSize = 0;
	}
	else
	{
		ulFileSize = (HUINT32)lFileSize;
	}
	rewind(readfp);
	PrintError("[%s,%d] Total size of decrypt file: %d\n", __FUNCTION__, __LINE__, ulFileSize);

	while(ulReadTotal < ulFileSize)
	{
		ulReadLen = fread(pReadBuf, 1, P_CRYPT_TS_SIZE, readfp);
		if (ulReadLen < P_CRYPT_TS_SIZE)
		{
			if (feof(readfp) != 0)
			{
				PrintError("[%s,%d] END OF FILE in fread()\n\r", __FUNCTION__, __LINE__);
				bEOF = TRUE;
			}
			else
			{
				PrintError("[%s,%d] Error in fread()\n\r", __FUNCTION__, __LINE__);
				break;
			}
		}

		for (i=0; i<P_CRYPT_TS_NUM; i++ )
		{
			unsigned int offset = i*P_CRYPT_TS_ALIGN;
			*(pReadBuf+offset+7) &= 0x3F;
		}
		VK_TASK_Sleep(10);

		ulWriteLen = fwrite(pReadBuf, 1, ulReadLen, writefp);
		if (ulWriteLen < ulReadLen)
		{
			PrintError("[%s,%d] Error in fwrite()\n\r", __FUNCTION__, __LINE__);
		}

		ulReadTotal += ulReadLen;
		ulWriteTotal += ulWriteLen;

		//PrintError("while() Read total: %d, Write total: %d\n", ulReadTotal, ulWriteTotal);

		if (bEOF != FALSE)
			break;
	}
	PrintError("END Read total: %d, Write total: %d\n", ulReadTotal, ulWriteTotal);

error:
	if (pReadBuf != NULL)
		DI_CRYPT_MemFree(pReadBuf);

	if (readfp != NULL)
		fclose(readfp);

	if (writefp != NULL)
		fclose(writefp);

	return DRV_OK;
}
#else
DRV_Error DRV_CRYPT_TestCps(void)
{
	PrintEnter();
	PrintError("[%s,%d]Error, 28nm chips can only support this feature.\n\r", __FUNCTION__, __LINE__);
	PrintExit();
	return DRV_OK;
}

DRV_Error DRV_CRYPT_TestM2m(void)
{
	PrintEnter();
	PrintError("[%s,%d]Error, 28nm chips can only support this feature.\n\r", __FUNCTION__, __LINE__);
	PrintExit();
	return DRV_OK;
}

DRV_Error DRV_CRYPT_TestClearTSC(void)
{
	PrintEnter();
	PrintError("[%s,%d]Error, 28nm chips can only support this feature.\n\r", __FUNCTION__, __LINE__);
	PrintExit();
	return DRV_OK;
}
#endif

DI_ERR_CODE DI_CRYPTO_KeyProvisioning(HUINT8 *pucInput, HUINT32 nKeySize, HUINT8 *pucOutput)
{
#if defined(CONFIG_KEY_PROVISIONING)
    int rc = 0;

	if(pucInput == NULL || pucOutput == NULL || !nKeySize)
	{
		PrintError("[%s] Error : Invalid parameter - pucInput(0x%x), pucOutput(0x%x)\r\n", __func__, pucInput, pucOutput);
		return DI_ERR_INVALID_PARAM;
	}

    PrintError("***************************************************************************************\r\n");
    PrintError("***************************************************************************************\r\n");
    PrintError("***\tBroadcom Corporation DRM bin file provisioning utility (Copyright 2014)\r\n");
    PrintError("***************************************************************************************\r\n");
    PrintError("***************************************************************************************\r\n");

	s_tHDCP22Provision.pucInput = pucInput;
	s_tHDCP22Provision.pucOutput = pucOutput;
	s_tHDCP22Provision.nKeySize = nKeySize;

    /* Initialize manufacturing platform and provsioning module */
    rc = Key_Provisioning_Init();
    if (rc != 0) {
        goto handle_error;
    }

    rc = Key_Provisioning_ProcessBinFile();
    if (rc != 0) {
        goto handle_error;
    }

handle_error:
	PrintError("[%s] Shutting down validation module and platform\r\n", __func__);
    Key_Provisioning_Uninit();

    if (rc) {
		PrintError("[%s] Failure in SAGE Provisioning tool\r\n", __func__);
		return DI_ERR_ERROR;
    }
#else
	BSTD_UNUSED(pucInput);
	BSTD_UNUSED(nKeySize);
	BSTD_UNUSED(pucOutput);
#endif
    return DI_ERR_OK;
}

DI_ERR_CODE DI_CRYPTO_KeyValidation(HUINT8 *pucInput, HUINT32 nKeySize)
{
#if defined(CONFIG_KEY_PROVISIONING)
    int rc = 0;

	if(pucInput == NULL || !nKeySize)
	{
		PrintError("[%s] Error : Invalid parameter - pucInput(0x%x)\r\n", __func__, pucInput);
		return DI_ERR_INVALID_PARAM;
	}

    PrintError("***************************************************************************************\r\n");
    PrintError("***************************************************************************************\r\n");
    PrintError("***\tBroadcom Corporation DRM bin file Validation utility (Copyright 2014)\r\n");
    PrintError("***************************************************************************************\r\n");
    PrintError("***************************************************************************************\r\n");

	s_tHDCP22Provision.pucInput = pucInput;
	s_tHDCP22Provision.nKeySize = nKeySize;

    /* Initialize manufacturing platform and provsioning module */
    rc = Key_Provisioning_Init();
    if (rc != 0)
	{
        goto handle_error;
    }

    rc = Key_Provisioning_ValidationKey();
    if (rc != 0)
	{
        goto handle_error;
    }

handle_error:
	PrintError("[%s] Shutting down validation module and platform\r\n", __func__);
    Key_Provisioning_Uninit();

    if (rc)
	{
		PrintError("[%s] Failure in SAGE Provisioning tool\r\n", __func__);
		return DI_ERR_ERROR;
    }
#else
	BSTD_UNUSED(pucInput);
	BSTD_UNUSED(nKeySize);
#endif
    return DI_ERR_OK;
}

#if defined(CONFIG_KEY_PROVISIONING)
int Key_Provisioning_Init(void)
{
    BERR_Code sage_rc;
    BSAGElib_State state;
    int rc = 0;

    BSAGElib_InOutContainer *container = SRAI_Container_Allocate();
    if (container == NULL) {
        rc = 1;
        goto handle_error;
    }

    /* Open the Manufacturing platform */
    sage_rc = SRAI_Platform_Open(BSAGE_PLATFORM_ID_MANUFACTURING,
                                 &state,
                                 &s_tHDCP22Provision.platformHandle);
    if (sage_rc != BERR_SUCCESS) {
        rc = 1;
        goto handle_error;
    }

    /* Check init state */
    if (state != BSAGElib_State_eInit)
    {
        /* Not yet initialized: send init command */
        sage_rc = SRAI_Platform_Init(s_tHDCP22Provision.platformHandle, NULL);
        if (sage_rc != BERR_SUCCESS)
        {
            rc = 1;
            goto handle_error;
        }
    }

    /* Initialize Provisioning module */
    sage_rc = SRAI_Module_Init(s_tHDCP22Provision.platformHandle,
                               PROVISIONING_MODULE,
                               container, &s_tHDCP22Provision.moduleHandle);
    if ((sage_rc != BERR_SUCCESS) || (container->basicOut[0] != 0))
    {
        rc = 1;
        goto handle_error;
    }

	/* Initialize Validation module */
	sage_rc = SRAI_Module_Init(s_tHDCP22Provision.platformHandle,
							   VALIDATION_MODULE,
							   container, &s_tHDCP22Provision.validationHandle);
	if ((sage_rc != BERR_SUCCESS) || (container->basicOut[0] != 0))
	{
		rc = 1;
		goto handle_error;
	}

handle_error:
    if (rc)
    {
        /* error: cleanup */
        Key_Provisioning_Uninit();
    }

    if(container != NULL)
    {
		PrintError("\t*** Freeing SRAI container\r\n");
        SRAI_Container_Free(container);
        container = NULL;
    }

    return rc;
}

void Key_Provisioning_Uninit(void)
{

    if (s_tHDCP22Provision.validationHandle)
    {
		PrintError("\t*** Uninitializing module '%p'\r\n", s_tHDCP22Provision.validationHandle);
        SRAI_Module_Uninit(s_tHDCP22Provision.validationHandle);
        s_tHDCP22Provision.validationHandle = NULL;
    }

    if (s_tHDCP22Provision.moduleHandle)
    {
		PrintError("\t*** Uninitializing module '%p'\r\n", s_tHDCP22Provision.moduleHandle);
        SRAI_Module_Uninit(s_tHDCP22Provision.moduleHandle);
        s_tHDCP22Provision.moduleHandle = NULL;
    }

    if (s_tHDCP22Provision.platformHandle)
    {
		PrintError("\t*** Uninitializing platform '%p'\r\n", s_tHDCP22Provision.platformHandle);
        SRAI_Platform_Close(s_tHDCP22Provision.platformHandle);
        s_tHDCP22Provision.platformHandle = NULL;
    }

    return;
}

int Key_Provisioning_ValidationKey(void)
{
    BERR_Code sage_rc;
    int rc = 0;

    BSAGElib_InOutContainer *container = NULL;

	container = SRAI_Container_Allocate();
    if (container == NULL)
    {
        rc = 1;
        goto handle_error;
    }

    /* Allocate memory in global region */
    container->blocks[0].data.ptr = SRAI_Memory_Allocate(s_tHDCP22Provision.nKeySize, SRAI_MemoryType_Shared);
    if(container->blocks[0].data.ptr == NULL)
    {
		PrintError("\t### Cannot allocate '%d' bytes of memory ^^^^^^^^^^^^^^^^^^^\r\n", s_tHDCP22Provision.nKeySize);
        rc = 1;
        goto handle_error;
    }

    container->blocks[0].len = s_tHDCP22Provision.nKeySize;

	VK_MEM_Memcpy(container->blocks[0].data.ptr, s_tHDCP22Provision.pucInput, s_tHDCP22Provision.nKeySize);

    /* todo : pass buff size to this function */
    rc = _KeyParseAndDisplay(container->blocks[0].data.ptr, container->blocks[0].len);
    if(rc != 0)
    {
        goto handle_error;
    }

    sage_rc = SRAI_Module_ProcessCommand(s_tHDCP22Provision.validationHandle, VALIDATION_COMMAND_ValidateHdcp22, container);
    if ((sage_rc != BERR_SUCCESS) || (container->basicOut[0] != 0))
    {
		PrintError("\t### Error processing bin file ^^^^^^^^^^^^^^^^^^\r\n");
        rc = 1;
        goto handle_error;
    }

	PrintError("\t*** Provisioning command successfully returned from SAGE, analyzing result....\r\n");

handle_error:

    if(container != NULL)
    {
    	if((container->basicOut[1] == SAGE_BINDING_OPERATION_SUCCESSFULLY_OCCURRED))
		{
			rc = 0;
		}
		else
		{
			rc = 1;
		}

        if(container->blocks[0].data.ptr != NULL)
		{
            SRAI_Memory_Free(container->blocks[0].data.ptr);
            container->blocks[0].data.ptr = NULL;
        }

        SRAI_Container_Free(container);
        container = NULL;
    }

    return rc;
}


int Key_Provisioning_ProcessBinFile(void)
{
    BERR_Code sage_rc;
    int rc = 0;

    BSAGElib_InOutContainer *container = NULL;

	container = SRAI_Container_Allocate();
    if (container == NULL)
    {
        rc = 1;
        goto handle_error;
    }

    /* Allocate memory in global region */
    container->blocks[0].data.ptr = SRAI_Memory_Allocate(s_tHDCP22Provision.nKeySize, SRAI_MemoryType_Shared);
    if(container->blocks[0].data.ptr == NULL)
    {
		PrintError("\t### Cannot allocate '%d' bytes of memory ^^^^^^^^^^^^^^^^^^^\r\n", s_tHDCP22Provision.nKeySize);
        rc = 1;
        goto handle_error;
    }

    container->blocks[0].len = s_tHDCP22Provision.nKeySize;

	VK_MEM_Memcpy(container->blocks[0].data.ptr, s_tHDCP22Provision.pucInput, s_tHDCP22Provision.nKeySize);

    /* todo : pass buff size to this function */
    rc = _KeyParseAndDisplay(container->blocks[0].data.ptr, container->blocks[0].len);
    if(rc != 0)
    {
        goto handle_error;
    }

    sage_rc = SRAI_Module_ProcessCommand(s_tHDCP22Provision.moduleHandle, PROVISIONING_COMMAND_ProcessBinFile, container);
    if ((sage_rc != BERR_SUCCESS) || (container->basicOut[0] != 0))
    {
		PrintError("\t### Error processing bin file ^^^^^^^^^^^^^^^^^^\r\n");
        rc = 1;
        goto handle_error;
    }

	PrintError("\t*** Provisioning command successfully returned from SAGE, analyzing result....\r\n");

handle_error:

    if(container != NULL)
    {
        /* overwrite bin file */
        if (container->basicOut[1] == SAGE_BINDING_OPERATION_SUCCESSFULLY_OCCURRED)
        {
			PrintError("\t*** SAGE side binding operation successfully completed\r\n");
			VK_MEM_Memcpy(s_tHDCP22Provision.pucOutput, container->blocks[0].data.ptr, s_tHDCP22Provision.nKeySize);
        }
        else
        {
            /* Display error code received from SAGE side */
			PrintError("\tThe following SAGE error occurred during the DRM binfile binding process (0x%08x):\r\n", container->basicOut[1]);
			PrintError("\t%s\r\n", BSAGElib_Tools_ReturnCodeToString(container->basicOut[1]));
			PrintError("\t%s\r\n", BSAGElib_Tools_ReturnCodeToString(container->basicOut[1]));
			PrintError("\t%s\r\n", BSAGElib_Tools_ReturnCodeToString(container->basicOut[1]));
        	rc = 1;
        }

		PrintError("\t*** Freeing shared memory\n");
        if(container->blocks[0].data.ptr != NULL){
            SRAI_Memory_Free(container->blocks[0].data.ptr);
            container->blocks[0].data.ptr = NULL;
        }

		PrintError("\t*** Freeing SRAI container\n");
        SRAI_Container_Free(container);
        container = NULL;
    }

    return rc;
}

static int _KeyParseAndDisplay(HUINT8 *pBinFile, HUINT32 binFileLength)
{
    HUINT32 number_of_drms = 0;
    HUINT32 current_drm_type = 0;
    HUINT32 i;
    const char * pString = NULL;

    if(binFileLength < (SIZEOF_DRM_BINFILE_HEADER + SIZEOF_DYNAMIC_OFFSET_HEADER))
    {
		PrintError("\tbinFileLength = %u is less than the size of a valid bin file(%u)\r\n", binFileLength, (SIZEOF_DRM_BINFILE_HEADER + SIZEOF_DYNAMIC_OFFSET_HEADER));
        return -1;
    }

    /* jump to the end of the header and get the number of DRMs */
    number_of_drms = GET_UINT32_FROM_BUF(&pBinFile[SIZEOF_DRM_BINFILE_HEADER]);

    /* sanity check */
    if(number_of_drms > MAX_NUMBER_BINFILE_DRM_TYPES)
    {
		PrintError("\tNumber of DRMs detected in header (%u) exceeds number of supported types (%u)\r\n", number_of_drms, MAX_NUMBER_BINFILE_DRM_TYPES);
        return -1;
    }

	PrintError("\tProvisioning the current DRM bin file for the following '%u' DRMs detected:\r\n", number_of_drms);
    for(i = 0; i < number_of_drms; i++)
    {
        /* todo : check out of bound access */
        current_drm_type = GET_UINT32_FROM_BUF(&pBinFile[SIZEOF_DRM_BINFILE_HEADER + SIZEOF_DYNAMIC_OFFSET_HEADER + 16*i]);
        pString = _KeyMapDrmEnumToString(current_drm_type);
        if(pString == NULL)
        {
			PrintError("\tInvalid DRM type detected (0x%08x), exiting application\r\n", current_drm_type);
            return -1;
        }

		PrintError("\t\t>>>> %s (0x%08x)\r\n", pString, current_drm_type);
    }

    return 0;
}

static const char * _KeyMapDrmEnumToString(uint32_t drm_type)
{
    switch(drm_type)
    {
    case BinFileDrmType_eNetflix:
        return "NETFLIX";
    case BinFileDrmType_eWidevine:
           return "WIDEVINE";
    case BinFileDrmType_eDtcpIp:
           return "DTCP-IP";
    case BinFileDrmType_ePlayready:
           return "PLAYREADY";
    case BinFileDrmType_eSecureSwRsa:
           return "SECURE_SW_RSA";
    case BinFileDrmType_eCustomPrivate:
           return "CUSTOM_PRIVATE";
    case BinFileDrmType_eAdobeAxcess:
           return "ADOBE";
    case BinFileDrmType_eHdcp22Rx:
           return "HDCP 2.2 RX";
    case BinFileDrmType_eHdcp22Tx:
           return "HDCP 2.2 TX";
    case BinFileDrmType_eMax:
           return NULL;
    default:
        return NULL;
    }
    return NULL;
}

#endif



