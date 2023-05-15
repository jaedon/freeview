
/*******************************************************************/
/**************************** Header Files ****************************/
/*******************************************************************/

#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <unistd.h>

#include "bstd.h"
#include "bkni.h"

#include "nexus_platform.h"
#include "nexus_security.h"
#include "nexus_keyladder.h"

#include "vkernel.h"

#include "drv_err.h"
#include "linden_trace.h"

#include "di_dsc.h"
#include "drv_dsc.h"
#include "drv_demux.h"

/*******************************************************************/
/****************************** define *******************************/
/*******************************************************************/
#define DEBUG_MODULE        TRACE_MODULE_DI_SCART

#define UNUSED_PARAM(x)  ((void)x)

/*#define GRAP_KEY*/
#ifdef GRAP_KEY
#include "di_video.h"
#include "drv_video.h"
#endif

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
#define INVALID_HANDLE		0xFFFFFFFF

#if defined(CONFIG_PVR_INLINE_CRYPTO_DSC_KEYSLOT_SHARE)
#define P_DSC_NOT_LINKED	0x1FFFFFFF

typedef enum {
	P_DSC_USED_BY_DSC = 0x1,
	P_DSC_USED_BY_REC = 0x10
} P_DSC_DSC_USED_INFO;

#endif

#if defined(CONFIG_TEMP_HMX4KJP) || defined(CONFIG_PVR_INLINE_CRYPTO_DSC_KEYSLOT_SHARE)
#define USE_INTERNAL_HANDLE
#endif
/* 20090701 :: POOH :: use BCMD_XptSecKeySlot_eType1, this value can change at nexus_security.c */
/* NEXUS_SecurityEngine_eM2m and NEXUS_SecurityEngine_eCa use BCMD_XptSecKeySlot_eType1 */
/* phase 6.0 */
/* In 40nm chipset, CA slot uses BCMD_XptSecKeySlot_eType0 for M2M and CA */
/* So DI_DSC_SLOTNUM_MAXIMUM changed to 10, because slot number for BCMD_XptSecKeySlot_eType0is 10 */
/* If there isn't enough slot number, we need to enlarge slot number for BCMD_XptSecKeySlot_eType0 after getting appropriate value from broadcom */
/* And also DI_DSC_SLOTNUM_MAXIMUM should be changed. */
#if defined(CONFIG_CRYPTO_NEW_SLOT_NUM)
/* DI_DSC_SLOTNUM_MAXIMUM changed to 60, because the slot number of loader changed to 60 */
#define DI_DSC_SLOTNUM_MAXIMUM	60
#else
#define DI_DSC_SLOTNUM_MAXIMUM	10
#endif
#define MAX_KEY_LENGTH			32

#if defined(CONFIG_CAS_VE)
#define VMX_ENC_ALGO_MAX		(10)
#define VMX_SVCIDX_START		(0x40)
#define VMX_SVCIDX_END			(VMX_SVCIDX_START + VMX_ENC_ALGO_MAX - 1)
#endif

typedef struct tagDI_DSC_INFO
{
	unsigned long			bAllocated;
	unsigned int			ulDemuxId;
	unsigned short			pid;
	unsigned int 			ulPidChannel;
	NEXUS_KeySlotHandle		keyHandle;
	unsigned char			keyData[2][MAX_KEY_LENGTH]; /* Even & Odd key data */
	long					lUsedCount;
	unsigned long			s_ulUseCWPK;
#if defined(CONFIG_PVR_INLINE_CRYPTO_DSC_KEYSLOT_SHARE)
	DI_DSC_DESC_TYPE		eDscType;
	HUINT32					ulLinked;
	P_DSC_DSC_USED_INFO		eUsedBy;
#endif
} DI_DSC_INFO;

/*******************************************************************/
/************************ global variables *****************************/
/*******************************************************************/
#ifdef GRAP_KEY
extern FILE * tempFileHandle;
extern FILE * saveFileHandle;
extern FILE * cwpkFileHandle;

typedef struct protectSessionInfo
{
	unsigned char			useful;
	unsigned int			key_length;
	unsigned char			keyData[MAX_KEY_LENGTH];

}PROTECT_SESS_INFO;

static PROTECT_SESS_INFO	s_stSessInfo;
unsigned char 				writeflag[DI_DSC_SLOTNUM_MAXIMUM] = {0,};
#endif

/*******************************************************************/
/************************ static variables ******************************/
/*******************************************************************/
static unsigned long	s_ulDscInit = FALSE;
static DI_DSC_INFO 		s_stDscSlotInfo[DI_DSC_SLOTNUM_MAXIMUM];
static HULONG			s_ulDscSemId;

#if defined(CONFIG_CAS_VE)
static HUINT8			s_aucVmxEncAlgo[VMX_ENC_ALGO_MAX];	/* eEncryptionAlgorithm_t */
static HUINT32			s_ulVmxDemuxId;
#endif

/*******************************************************************/
/*********************** extern function prototypes ***********************/
/*******************************************************************/
#if defined(CONFIG_PVR_INLINE_CRYPTO_DSC_KEYSLOT_SHARE)
static DRV_Error P_DSC_OpenKeySlot(HUINT32 ulDscId, HUINT16 usPid, NEXUS_KeySlotHandle *pKeySlotHandle);
static DRV_Error P_DSC_SetConfigAlgorithm(NEXUS_KeySlotHandle keyHandle, DI_DSC_DESC_TYPE eDscType);
static DRV_Error P_DSC_CloseKeySlot(HUINT32 ulDscId);
#endif
/*******************************************************************/
/*********************** Static function prototypes ***********************/
/*******************************************************************/

DRV_Error DRV_DSC_Init(void)
{
	unsigned long i;
	int vkErr = 0;

	PrintEnter("DRV_DSC_Init\n\r");

	if (s_ulDscInit != FALSE)
	{
		PrintDebug("[DRV_DSC_Init] The DSC already be initaliaed!!\r\n");

		return DRV_ERR;
	}

	vkErr = VK_SEM_Create(&s_ulDscSemId, "DSC_SEM", VK_SUSPENDTYPE_PRIORITY);
	if(vkErr != VK_OK)
	{
		PrintError("Error in VK_SEM_Create()\n");
		return DRV_ERR;
	}

	VK_memset (s_stDscSlotInfo, 0, DI_DSC_SLOTNUM_MAXIMUM * sizeof(DI_DSC_INFO));

#ifdef GRAP_KEY
	VK_memset (&s_stSessInfo, 0,sizeof(PROTECT_SESS_INFO));
#endif

	for (i = 0; i < DI_DSC_SLOTNUM_MAXIMUM; i++)
	{
		s_stDscSlotInfo[i].bAllocated 		= FALSE;
		s_stDscSlotInfo[i].keyHandle 		= NULL;
		s_stDscSlotInfo[i].s_ulUseCWPK 		= FALSE;
		s_stDscSlotInfo[i].lUsedCount 		= 0;
		s_stDscSlotInfo[i].ulPidChannel 	= INVALID_HANDLE;
#if defined(CONFIG_PVR_INLINE_CRYPTO_DSC_KEYSLOT_SHARE)
		s_stDscSlotInfo[i].ulLinked			= P_DSC_NOT_LINKED;
#endif
	}

#ifdef GRAP_KEY
	s_stSessInfo.useful				= 0xff;
#endif

#if defined(CONFIG_CAS_VE)
	VK_memset(s_aucVmxEncAlgo, 0, sizeof(s_aucVmxEncAlgo));
	s_ulVmxDemuxId = 0;
#endif

	s_ulDscInit = TRUE;

	PrintExit("DRV_DSC_Init\n\r");

	return DRV_OK;
}

DI_ERR_CODE DI_DSC_Open(HUINT32 ulDemuxId, DI_DSC_DESC_TYPE type, HUINT32 *DescId, DI_DSC_EncryptionSettings *encryptionSetting)
{
#if !defined(CONFIG_PVR_INLINE_CRYPTO_DSC_KEYSLOT_SHARE)
	NEXUS_SecurityKeySlotSettings 	keySlotSettings;
	NEXUS_SecurityAlgorithmSettings AlgConfig;
#endif
	NEXUS_KeySlotHandle 			keyHandle = NULL;
	NEXUS_SecurityMulti2Settings 	config_multi2;
	NEXUS_Error 					rc = NEXUS_SUCCESS;
	int 							i = 0;

	PrintEnter("DI_DSC_Open");

	PrintDebug("[DI_DSC_Open] DI_DSC_DESC_TYPE(%d)\n\r", type);

#ifdef GRAP_KEY
	PrintError("\n\n[DI_DSC_Open] : ulDemuxId : 0x%x, type : %d\n\n", ulDemuxId, type);
#endif

	if(DescId == NULL)
	{
		PrintError("[DI_DSC_Open]: Resourec Error \r\n");
		return DI_ERR_NO_RESOURCE;
	}

	if((type == DI_DSC_DESC_MULTI2)&&( encryptionSetting == NULL))
	{
		PrintError("[DI_DSC_Open]: Resourec Error \r\n");
		return DI_ERR_NO_RESOURCE;
	}

	VK_SEM_Get(s_ulDscSemId);

#if !defined(CONFIG_PVR_INLINE_CRYPTO_DSC_KEYSLOT_SHARE)
	NEXUS_Security_GetDefaultKeySlotSettings(&keySlotSettings);

	switch (type)
	{
		case DI_DSC_DESC_DVB:
		case DI_DSC_DESC_AES:
		case DI_DSC_DESC_DES:
		case DI_DSC_DESC_DES_CLEARMODE:
		case DI_DSC_DESC_3DESABA:
		case DI_DSC_DESC_MULTI2:
#if defined(CONFIG_PVR_SECURITY_ENGINE_INLINE_CRYPTO)
			keySlotSettings.keySlotEngine = NEXUS_SecurityEngine_eCaCp;
#else
			keySlotSettings.keySlotEngine = NEXUS_SecurityEngine_eCa;
#endif
			break;
		case DI_DSC_DESC_AES_CLEARMODE:
#if defined(CONFIG_PVR_SECURITY_ENGINE_INLINE_CRYPTO)
			keySlotSettings.keySlotEngine = NEXUS_SecurityEngine_eCaCp;
#else
			keySlotSettings.keySlotEngine = NEXUS_SecurityEngine_eCa;
#endif
			keySlotSettings.keySlotType = NEXUS_SecurityKeySlotType_eType2;

			break;
		default:
			PrintError("[DI_DSC_Open] Not support type  = %d\r\n", type);
			VK_SEM_Release(s_ulDscSemId);
			return DI_ERR_ERROR;
	}

	keyHandle = NEXUS_Security_AllocateKeySlot(&keySlotSettings);
	if (keyHandle == NULL)
	{
		PrintError("[DI_DSC_Open] NEXUS_Security_AllocateKeySlot error, keyHandle is NULL\n");

		VK_SEM_Release(s_ulDscSemId);
		return DI_ERR_NO_RESOURCE;
	}

	NEXUS_Security_GetDefaultAlgorithmSettings(&AlgConfig);

#if ((BCHP_CHIP == 7346 || BCHP_CHIP == 7344) && defined(CONFIG_CAS_IR)) || defined(CONFIG_CAS_ASKM)
	AlgConfig.key2Select	= NEXUS_SecurityKey2Select_eFixedKey;
	AlgConfig.caVendorID	= 0xEAF6;
	AlgConfig.ivMode		= NEXUS_SecurityIVMode_eRegular;
	AlgConfig.otpId         = NEXUS_SecurityOtpId_eOtpVal;
	AlgConfig.askmModuleID	= NEXUS_SecurityAskmModuleID_eModuleID_17;
#elif defined(CONFIG_CAS_NA) && defined(CONFIG_CAS_NA_NOCS_1X)
#if (BCHP_CHIP == 7346 || BCHP_CHIP == 7425)
	AlgConfig.key2Select = NEXUS_SecurityKey2Select_eFixedKey;
#endif
	AlgConfig.caVendorID = 0x97ed;
	AlgConfig.ivMode = NEXUS_SecurityIVMode_eRegular;
	AlgConfig.otpId = NEXUS_SecurityOtpId_eOtpVal;
	AlgConfig.keyDestEntryType = NEXUS_SecurityKeyType_eOddAndEven;
	AlgConfig.askmModuleID	= NEXUS_SecurityAskmModuleID_eModuleID_5;
#else
	AlgConfig.askmModuleID	= NEXUS_SecurityAskmModuleID_eModuleID_3;
#endif

#if defined(CONFIG_PVR_SECURITY_ENGINE_INLINE_CRYPTO)
	AlgConfig.dest = NEXUS_SecurityAlgorithmConfigDestination_eCa;
#endif

	switch (type)
	{
		case DI_DSC_DESC_DVB:
			AlgConfig.algorithm 		= NEXUS_SecurityAlgorithm_eDvb;
			AlgConfig.dvbScramLevel 	= NEXUS_SecurityDvbScrambleLevel_eTs;
			AlgConfig.scValue[NEXUS_SecurityPacketType_eGlobal] 	= NEXUS_SecurityAlgorithmScPolarity_eClear;
			AlgConfig.scValue[NEXUS_SecurityPacketType_eRestricted] = NEXUS_SecurityAlgorithmScPolarity_eClear;
			AlgConfig.modifyScValue[NEXUS_SecurityPacketType_eGlobal] = true;
			AlgConfig.modifyScValue[NEXUS_SecurityPacketType_eRestricted] = true;
			break;

		case DI_DSC_DESC_AES:
			AlgConfig.algorithm 		= NEXUS_SecurityAlgorithm_eAes;
			AlgConfig.algorithmVar 		= NEXUS_SecurityAlgorithmVariant_eCbc;
#if defined(CONFIG_CAS_ARIB)
#if (NEXUS_VERSION >= 1302)
			AlgConfig.solitarySelect   = NEXUS_SecuritySolitarySelect_eCbcXorIv;
#else
			AlgConfig.solitarySelect   = NEXUS_SecuritySolitarySelect_eCbcXorIV;
#endif
			AlgConfig.terminationMode	=  NEXUS_SecurityTerminationMode_eCbcResidual;
#else
			AlgConfig.terminationMode 	= NEXUS_SecurityTerminationMode_eBlock;
#endif
			AlgConfig.scValue[NEXUS_SecurityPacketType_eGlobal] 	= NEXUS_SecurityAlgorithmScPolarity_eClear;
			AlgConfig.scValue[NEXUS_SecurityPacketType_eRestricted] = NEXUS_SecurityAlgorithmScPolarity_eClear;
			AlgConfig.modifyScValue[NEXUS_SecurityPacketType_eGlobal] = true;
			AlgConfig.modifyScValue[NEXUS_SecurityPacketType_eRestricted] = true;
			break;

		case DI_DSC_DESC_DES:
			AlgConfig.algorithm 		= NEXUS_SecurityAlgorithm_eDes;
			AlgConfig.dvbScramLevel 	= NEXUS_SecurityDvbScrambleLevel_eTs;
			AlgConfig.scValue[NEXUS_SecurityPacketType_eGlobal] 	= NEXUS_SecurityAlgorithmScPolarity_eClear;
			AlgConfig.scValue[NEXUS_SecurityPacketType_eRestricted] = NEXUS_SecurityAlgorithmScPolarity_eClear;
			AlgConfig.modifyScValue[NEXUS_SecurityPacketType_eGlobal] = true;
			AlgConfig.modifyScValue[NEXUS_SecurityPacketType_eRestricted] = true;

			break;

		case DI_DSC_DESC_AES_CLEARMODE:
			AlgConfig.algorithm 		= NEXUS_SecurityAlgorithm_eAes;
			AlgConfig.algorithmVar 		= NEXUS_SecurityAlgorithmVariant_eCbc;
			AlgConfig.scValue[NEXUS_SecurityPacketType_eGlobal] 	= NEXUS_SecurityAlgorithmScPolarity_eClear;
			AlgConfig.scValue[NEXUS_SecurityPacketType_eRestricted] = NEXUS_SecurityAlgorithmScPolarity_eClear;
			AlgConfig.modifyScValue[NEXUS_SecurityPacketType_eGlobal] = true;
			AlgConfig.modifyScValue[NEXUS_SecurityPacketType_eRestricted] = true;

			break;

		case DI_DSC_DESC_DES_CLEARMODE:
			AlgConfig.algorithm 		= NEXUS_SecurityAlgorithm_eDes;
			AlgConfig.dvbScramLevel 	= NEXUS_SecurityDvbScrambleLevel_eTs;
			AlgConfig.scValue[NEXUS_SecurityPacketType_eGlobal] 	= NEXUS_SecurityAlgorithmScPolarity_eClear;
			AlgConfig.scValue[NEXUS_SecurityPacketType_eRestricted] = NEXUS_SecurityAlgorithmScPolarity_eClear;
			AlgConfig.modifyScValue[NEXUS_SecurityPacketType_eGlobal] = true;
			AlgConfig.modifyScValue[NEXUS_SecurityPacketType_eRestricted] = true;
			break;

		case DI_DSC_DESC_3DESABA:
			AlgConfig.algorithm 		= NEXUS_SecurityAlgorithm_e3DesAba;
			AlgConfig.dvbScramLevel 	= NEXUS_SecurityDvbScrambleLevel_eTs;
			AlgConfig.terminationMode 	= NEXUS_SecurityTerminationMode_eCipherStealing;
			AlgConfig.scValue[NEXUS_SecurityPacketType_eGlobal] 	= NEXUS_SecurityAlgorithmScPolarity_eClear;
			AlgConfig.scValue[NEXUS_SecurityPacketType_eRestricted] = NEXUS_SecurityAlgorithmScPolarity_eClear;
			AlgConfig.modifyScValue[NEXUS_SecurityPacketType_eGlobal] = true;
			AlgConfig.modifyScValue[NEXUS_SecurityPacketType_eRestricted] = true;
			break;

		case DI_DSC_DESC_MULTI2:
			AlgConfig.algorithm 		= NEXUS_SecurityAlgorithm_eMulti2;
			AlgConfig.dvbScramLevel 	= NEXUS_SecurityDvbScrambleLevel_eTs;
			AlgConfig.algorithmVar =
				encryptionSetting->algorithmVar == DI_DSC_DESC_AlgorithmVariant_eCbc ? NEXUS_SecurityAlgorithmVariant_eCbc : NEXUS_SecurityAlgorithmVariant_eEcb;
			AlgConfig.multi2KeySelect 	= encryptionSetting->multi2KeySelect;
			if(encryptionSetting->algorithmVar == DI_DSC_DESC_AlgorithmVariant_eCbc)
			{
#if (NEXUS_VERSION >= 1302)
				AlgConfig.solitarySelect   = NEXUS_SecuritySolitarySelect_eCbcXorIv;
#else
				AlgConfig.solitarySelect   = NEXUS_SecuritySolitarySelect_eCbcXorIV;
#endif
			}
			else
			{
			    AlgConfig.solitarySelect   = NEXUS_SecuritySolitarySelect_eClear;
			}
			/* 20090912 :: POOH :: 아래 setting을 해야만 av가 제대로 나옴. */
			AlgConfig.terminationMode 	= NEXUS_SecurityTerminationMode_eBlock;

			AlgConfig.scValue[NEXUS_SecurityPacketType_eGlobal] = NEXUS_SecurityAlgorithmScPolarity_eClear;
			AlgConfig.scValue[NEXUS_SecurityPacketType_eRestricted] = NEXUS_SecurityAlgorithmScPolarity_eClear;
			AlgConfig.modifyScValue[NEXUS_SecurityPacketType_eGlobal] = true;
			AlgConfig.modifyScValue[NEXUS_SecurityPacketType_eRestricted] = true;
#if (NEXUS_VERSION >= 1304)
			AlgConfig.bMulti2Config  = true;
			AlgConfig.multi2RoundCount =  encryptionSetting->multi2Rounds;
#endif
			break;

		default:
			PrintError("[DI_DSC_Open] Not support type  = %d\r\n", type);
			NEXUS_Security_FreeKeySlot(keyHandle);
			VK_SEM_Release(s_ulDscSemId);
			return DI_ERR_ERROR;
	}

	rc = NEXUS_Security_ConfigAlgorithm (keyHandle, &AlgConfig);
	if ( rc != NEXUS_SUCCESS )
	{
		PrintError("[DI_DSC_Open] NEXUS_Security_ConfigAlgorithm error(0x%x)\r\n", rc);
		NEXUS_Security_FreeKeySlot(keyHandle);
		VK_SEM_Release(s_ulDscSemId);
		return DI_ERR_ERROR;
	}
#endif /* !defined(CONFIG_PVR_INLINE_CRYPTO_DSC_KEYSLOT_SHARE) */
	if ((type == DI_DSC_DESC_MULTI2) && (encryptionSetting->bIsUsed == TRUE))
	{
		PrintDebug("[DI_DSC_Open] DI_DSC_DESC_MULTI2\r\n");

		NEXUS_Security_GetDefaultMulti2Settings(&config_multi2);

		config_multi2.multi2KeySelect = encryptionSetting->multi2KeySelect;
		config_multi2.multi2Rounds = encryptionSetting->multi2Rounds;

		VK_MEM_Memcpy((void *)config_multi2.multi2SysKey, (void *)encryptionSetting->multi2SysKey, 32);

		if ( NEXUS_Security_ConfigMulti2(keyHandle, &config_multi2) != 0 )
		{
			PrintError("[DI_DSC_Open] Error NEXUS_Security_ConfigMulti2 0x%x\r\n", rc);

			NEXUS_Security_FreeKeySlot(keyHandle);
			VK_SEM_Release(s_ulDscSemId);
			return DI_ERR_ERROR;
		}
	}

	/* Search the freeSlot */
	for (i = 0; i < DI_DSC_SLOTNUM_MAXIMUM; i++)
	{
		if (s_stDscSlotInfo[i].bAllocated == FALSE)
		{
			s_stDscSlotInfo[i].ulDemuxId 	= ulDemuxId;
#if defined(CONFIG_PVR_INLINE_CRYPTO_DSC_KEYSLOT_SHARE)
			s_stDscSlotInfo[i].keyHandle    = NULL;
			s_stDscSlotInfo[i].eDscType	= type;
#else
			s_stDscSlotInfo[i].keyHandle 	= keyHandle;
#endif
			s_stDscSlotInfo[i].bAllocated 	= TRUE;

			VK_MEM_Memset((void *)s_stDscSlotInfo[i].keyData, 0x0, 2 * MAX_KEY_LENGTH);
#if defined(USE_INTERNAL_HANDLE)
			*DescId = (HUINT32)i;
#else
			*DescId = (HUINT32)keyHandle;
#endif
			break;
		}
	}

	if (i == DI_DSC_SLOTNUM_MAXIMUM)
	{
		PrintError("[DI_DSC_Open] All slot be allocated!!\r\n");
#if !defined(CONFIG_PVR_INLINE_CRYPTO_DSC_KEYSLOT_SHARE)
		NEXUS_Security_FreeKeySlot(keyHandle);
#endif
		VK_SEM_Release(s_ulDscSemId);
		return DI_ERR_ERROR;
	}

	PrintDebug("[DI_DSC_Open] slot(%d) DescId(0x%x)\r\n", i, s_stDscSlotInfo[i].bAllocated, *DescId);

	PrintExit("DI_DSC_Open");
	VK_SEM_Release(s_ulDscSemId);

	return DI_ERR_OK;
}

DI_ERR_CODE DI_DSC_SetPid(HUINT32 DescId, HUINT16 pid)
{
	NEXUS_Error rc = NEXUS_SUCCESS;
	int 		i = 0;
	DRV_Error	nRet = DRV_OK;
#if defined(CONFIG_PVR_INLINE_CRYPTO_DSC_KEYSLOT_SHARE)
	NEXUS_KeySlotHandle keyHandle = NULL;
#endif

	PrintError("######DI_DSC_SetPid ###\r\n");

	PrintError("[DI_DSC_SetPid] DescId(0x%x) pid(0x%x)\n",
				DescId,
				pid);

	VK_SEM_Get(s_ulDscSemId);
#if defined(USE_INTERNAL_HANDLE)
	i = DescId;
#else
	for (i = 0; i < DI_DSC_SLOTNUM_MAXIMUM; i++)
	{
		if (s_stDscSlotInfo[i].keyHandle == (NEXUS_KeySlotHandle)DescId)
		{
			PrintDebug("[DI_DSC_SetPid] It be found slot(%d)\r\n", i);
			PrintDebug("[DI_DSC_SetPid] s_stDscSlotInfo[%d].keyHandle = %x\r\n",i,s_stDscSlotInfo[i].keyHandle);
			PrintDebug("[DI_DSC_SetPid] DescId = %x\r\n",DescId);
			PrintDebug("[DI_DSC_SetPid] s_stDscSlotInfo[%d].bAllocated = %d \r\n", i, s_stDscSlotInfo[i].bAllocated);
			PrintDebug("[DI_DSC_SetPid] s_stDscSlotInfo[%d].lUsedCount = %d \r\n", i, s_stDscSlotInfo[i].lUsedCount);

			break;
		}
	}
#endif
#ifdef GRAP_KEY
	PrintError("\n\n[DI_DSC_SetPid] : ulDemuxId : 0x%x, pid : 0x%x\n\n", s_stDscSlotInfo[i].ulDemuxId, pid);
#endif

	if ( i == DI_DSC_SLOTNUM_MAXIMUM )
	{
		PrintError("[DI_DSC_SetPid] There isn't matched slot!!\r\n");

		VK_SEM_Release(s_ulDscSemId);
		return DI_ERR_ERROR;
	}

	if (s_stDscSlotInfo[i].bAllocated == TRUE)
	{
		nRet = DRV_DEMUX_ConnectPIDChannelToDSC(s_stDscSlotInfo[i].ulDemuxId, pid, &(s_stDscSlotInfo[i].ulPidChannel));
		if ( nRet != DRV_OK )
		{
			PrintError("[DI_DSC_SetPid] nRet= %x\r\n",nRet);

			VK_SEM_Release(s_ulDscSemId);
			return DI_ERR_ERROR;
		}

		if ( s_stDscSlotInfo[i].ulPidChannel == INVALID_HANDLE )
		{
			PrintError("[DI_DSC_SetPid] ( ulPidChannel == INVALID_HANDLE )\r\n");

			VK_SEM_Release(s_ulDscSemId);
			return DI_ERR_ERROR;
		}

#if defined(CONFIG_PVR_INLINE_CRYPTO_DSC_KEYSLOT_SHARE)
		nRet = P_DSC_OpenKeySlot(i, pid, &keyHandle);
		if (nRet != DRV_OK)
		{
			PrintError("[DI_DSC_SetPid] Error, P_DSC_OpenKeySlot\r\n");
			VK_SEM_Release(s_ulDscSemId);
			return DI_ERR_ERROR;
		}

		rc = NEXUS_Security_AddPidChannelToKeySlot((NEXUS_KeySlotHandle)keyHandle, s_stDscSlotInfo[i].ulPidChannel);
#else
#if defined(USE_INTERNAL_HANDLE)
		rc = NEXUS_Security_AddPidChannelToKeySlot((NEXUS_KeySlotHandle)s_stDscSlotInfo[i].keyHandle, s_stDscSlotInfo[i].ulPidChannel);
#else
		rc = NEXUS_Security_AddPidChannelToKeySlot((NEXUS_KeySlotHandle)DescId, s_stDscSlotInfo[i].ulPidChannel);
#endif
#endif
		if ( rc != NEXUS_SUCCESS )
		{
			PrintError("[DI_DSC_SetPid] NEXUS_Security_AddPidChannelToKeySlot error(0x%x)\r\n", rc);
			DRV_DEMUX_DisconnectPIDChannelFromDSC(s_stDscSlotInfo[i].ulPidChannel);
			VK_SEM_Release(s_ulDscSemId);
			return DI_ERR_ERROR;
		}

		s_stDscSlotInfo[i].lUsedCount++;
#if defined(CONFIG_CAS_VE)
		s_stDscSlotInfo[i].pid = pid;
#endif
#if defined(CONFIG_PVR_SECURITY_ENGINE_INLINE_CRYPTO)
		s_stDscSlotInfo[i].pid = pid;
#endif
#ifdef GRAP_KEY
		s_stDscSlotInfo[i].pid = pid;

		PrintDebug("\n\n[DI_DSC_SetPid] : s_stDscSlotInfo[i].ulDemuxId : %x, pid : 0x%x\n\n", s_stDscSlotInfo[i].ulDemuxId, s_stDscSlotInfo[i].pid);

		if ((tempFileHandle != NULL) && (saveFileHandle == NULL))
		{
			/*if(s_stDscSlotInfo[i].ulDemuxId == 0)*/
			if(writeflag[i] == 0)
			{
				VK_fprintf(tempFileHandle, "PID %01X %04X \n", s_stDscSlotInfo[i].ulDemuxId, s_stDscSlotInfo[i].pid);

				writeflag[i] = 1;
			}

			if (tempFileHandle == NULL)
			{
				writeflag[i] = 0;
			}
		}
#endif
	}
	else
	{
		PrintError("[DI_DSC_SetPid] Didn't allocate s_stDscSlotInfo[i].bAllocated\r\n");

		VK_SEM_Release(s_ulDscSemId);
		return DI_ERR_ERROR;
	}

	VK_SEM_Release(s_ulDscSemId);

	PrintExit("DI_DSC_SetPid");

	return DI_ERR_OK;
}

DI_ERR_CODE DI_DSC_ClearPid(HUINT32 DescId, HUINT16 pid)
{
	NEXUS_Error 	rc = NEXUS_SUCCESS;
	int 			i = 0;
	DRV_Error		nRet = DRV_OK;
	unsigned int	ulGetPidChannel = INVALID_HANDLE;
	unsigned int	ulNumChannel = 0;

	PrintEnter("DI_DSC_ClearPid");

	PrintDebug("[DI_DSC_ClearPid] DescId(0x%x) pid(0x%x)\n",
				DescId,
				pid);

	VK_SEM_Get(s_ulDscSemId);

#if defined(USE_INTERNAL_HANDLE)
	i = DescId;
#else
	for (i = 0; i < DI_DSC_SLOTNUM_MAXIMUM; i++)
	{
		if (s_stDscSlotInfo[i].keyHandle == (NEXUS_KeySlotHandle)DescId)
		{
			PrintDebug("[DI_DSC_ClearPid] It be found slot(%d)\r\n", i);
			PrintDebug("[DI_DSC_ClearPid] s_stDscSlotInfo[%d].keyHandle = %x\r\n", i, s_stDscSlotInfo[i].keyHandle);
			PrintDebug("[DI_DSC_ClearPid] DescId = %x\r\n", DescId);
			PrintDebug("[DI_DSC_ClearPid] s_stDscSlotInfo[%d].bAllocated = %d \r\n", i, s_stDscSlotInfo[i].bAllocated);
			PrintDebug("[DI_DSC_ClearPid] s_stDscSlotInfo[%d].lUsedCount = %d \r\n", i, s_stDscSlotInfo[i].lUsedCount);

			break;
		}
	}
#endif
	if ( i == DI_DSC_SLOTNUM_MAXIMUM )
	{
		PrintError("[DI_DSC_ClearPid] There isn't matched slot!!\r\n");

		VK_SEM_Release(s_ulDscSemId);
		return DI_ERR_ERROR;
	}

	if (s_stDscSlotInfo[i].lUsedCount == 0)
	{
		PrintError("[DI_DSC_ClearPid] (s_stDscSlotInfo[i].lUsedCount==0)\r\n");

		VK_SEM_Release(s_ulDscSemId);
		return DI_ERR_ERROR;
	}

	if (s_stDscSlotInfo[i].bAllocated == TRUE)
	{
/* 20100811 :: POOH :: IRDETO :: Add one DVB stream PID to load for the descrambler channel. */
/* 					It can load multiple PIDs with the same control word to one descrambler. */
/*					s_stDscSlotInfo[i].ulPidChannel을 사용하지 않음. */
		ulNumChannel = DRV_DEMUX_GetPIDChannelForDSC(s_stDscSlotInfo[i].ulDemuxId, pid, &ulGetPidChannel);

		PrintDebug("[DI_DSC_ClearPid] pid = %x ulGetPidChannel = %x ulNumChannel = %d \r\n",pid, ulGetPidChannel,ulNumChannel);

		if ((ulGetPidChannel == INVALID_HANDLE) || (ulNumChannel < 1))
		{
			PrintError("[DI_DSC_ClearPid] Get invalid pidchannel = %x\r\n", ulGetPidChannel);

			VK_SEM_Release(s_ulDscSemId);
			return DI_ERR_ERROR;
		}

		if (ulNumChannel == 1)
		{
#if defined(USE_INTERNAL_HANDLE)
			rc = NEXUS_Security_RemovePidChannelFromKeySlot((NEXUS_KeySlotHandle)s_stDscSlotInfo[i].keyHandle, ulGetPidChannel);
#else
			rc = NEXUS_Security_RemovePidChannelFromKeySlot((NEXUS_KeySlotHandle)DescId, ulGetPidChannel);
#endif
			if ( rc != NEXUS_SUCCESS )
			{
				PrintError("[DI_DSC_ClearPid] Error NEXUS_Security_RemovePidChannelFromKeySlot 0x%x\r\n", rc);

				VK_SEM_Release(s_ulDscSemId);
				return DI_ERR_ERROR;
			}
		}

		nRet = DRV_DEMUX_DisconnectPIDChannelFromDSC(ulGetPidChannel);
		if ( nRet != DRV_OK )
		{
			PrintError("[DI_DSC_ClearPid] nRet= %x\r\n", nRet);

			VK_SEM_Release(s_ulDscSemId);
			return DI_ERR_ERROR;
		}

		if (s_stDscSlotInfo[i].lUsedCount == 1)
		{
			s_stDscSlotInfo[i].ulPidChannel = INVALID_HANDLE;
		}

		s_stDscSlotInfo[i].lUsedCount--;
	}

	VK_SEM_Release(s_ulDscSemId);
	PrintExit("DI_DSC_ClearPid");

	return DI_ERR_OK;
}

/*****************************************************************
*
* Function Name:	DI_DSC_Close
* Inputs
* Outputs
* Returns
* Description
*
*****************************************************************/
DI_ERR_CODE DI_DSC_Close(HUINT32 DescId)
{
	int i = 0;
#if defined(CONFIG_PVR_INLINE_CRYPTO_DSC_KEYSLOT_SHARE)
	DRV_Error drvErr = DRV_OK;
#else
	NEXUS_SecurityInvalidateKeySettings tSecurityInvalidateKeySettings;
	NEXUS_Error rc = NEXUS_SUCCESS;
#endif

	PrintEnter("DI_DSC_Close");

	PrintDebug("[DI_DSC_Close] DescId(%d)\n",
				DescId);

	VK_SEM_Get(s_ulDscSemId);

#if defined(USE_INTERNAL_HANDLE)
	i = DescId;
#else
	for (i = 0; i < DI_DSC_SLOTNUM_MAXIMUM; i++)
	{
		if (s_stDscSlotInfo[i].keyHandle == (NEXUS_KeySlotHandle)DescId)
		{
			PrintDebug("[DI_DSC_Close] It be found slot(%d)\r\n", i);
			PrintDebug("[DI_DSC_Close] s_stDscSlotInfo[%d].keyHandle = %x\r\n",i,s_stDscSlotInfo[i].keyHandle);
			PrintDebug("[DI_DSC_Close] DescId = %x\r\n",DescId);
			break;
		}
	}
#endif
	if (i == DI_DSC_SLOTNUM_MAXIMUM)
	{
		PrintError("[DI_DSC_Close] There isn't matched slot!!\r\n");

		VK_SEM_Release(s_ulDscSemId);
		return DI_ERR_ERROR;
	}

	if (s_stDscSlotInfo[i].lUsedCount != 0)
	{
		PrintError("[DI_DSC_Close] (s_stDscSlotInfo[i].lUsedCount != 0)\r\n");

		VK_SEM_Release(s_ulDscSemId);
		return DI_ERR_ERROR;
	}

	if (s_stDscSlotInfo[i].bAllocated == TRUE)
	{
#if defined(CONFIG_PVR_INLINE_CRYPTO_DSC_KEYSLOT_SHARE)
		drvErr = P_DSC_CloseKeySlot(DescId);
		if (drvErr != DRV_OK)
		{
			PrintError("[%s] Error, P_DSC_CloseKeySlot(%d)!\r\n", __FUNCTION__, DescId);
			VK_SEM_Release(s_ulDscSemId);
			return DI_ERR_ERROR;
		}
#else
		NEXUS_Security_GetDefaultInvalidateKeySettings(&tSecurityInvalidateKeySettings);
		tSecurityInvalidateKeySettings.keyDestEntryType  = NEXUS_SecurityKeyType_eOddAndEven;
		tSecurityInvalidateKeySettings.invalidateKeyType = NEXUS_SecurityInvalidateKeyFlag_eDestKeyOnly;
		tSecurityInvalidateKeySettings.keyDestBlckType            = NEXUS_SecurityAlgorithmConfigDestination_eCa;
		tSecurityInvalidateKeySettings.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKLDummy;
#if defined(USE_INTERNAL_HANDLE)
		rc = NEXUS_Security_InvalidateKey((NEXUS_KeySlotHandle)s_stDscSlotInfo[i].keyHandle, &tSecurityInvalidateKeySettings);
#else
		rc = NEXUS_Security_InvalidateKey((NEXUS_KeySlotHandle)DescId, &tSecurityInvalidateKeySettings);
#endif
		if( rc != NEXUS_SUCCESS )
		{
			PrintError("[DI_DSC_Close] Error NEXUS_Security_InvalidateKey 0x%x\r\n", rc);
		}
#if defined(USE_INTERNAL_HANDLE)
		NEXUS_Security_FreeKeySlot((NEXUS_KeySlotHandle)s_stDscSlotInfo[i].keyHandle);
#else
		NEXUS_Security_FreeKeySlot((NEXUS_KeySlotHandle)DescId);
#endif
		s_stDscSlotInfo[i].bAllocated = FALSE;
		s_stDscSlotInfo[i].keyHandle = NULL;
		s_stDscSlotInfo[i].s_ulUseCWPK = FALSE;
		s_stDscSlotInfo[i].lUsedCount = 0;

#endif
#ifdef GRAP_KEY
		writeflag[i] = 0;
#endif
	}
	else
	{
		PrintError("[DI_DSC_Close] Didn't allocate \r\n");

		VK_SEM_Release(s_ulDscSemId);
		return DI_ERR_ERROR;
	}

	PrintExit("DI_DSC_Close");

	VK_SEM_Release(s_ulDscSemId);
	return DI_ERR_OK;
}

/*****************************************************************
*
* Function Name:	DI_DSC_SetKey
* Inputs
* Outputs
* Returns
* Description
*
*****************************************************************/
DI_ERR_CODE DI_DSC_SetKey(HUINT32 DescId,
							DI_DSC_DESC_ODD_EVEN parity,
							HUINT8 *p_key_data,
							HUINT32 key_length,
							HUINT8 *p_init_vector,		/* 사용하지 않는 경우 NULL */
							HUINT32 vector_length)		/* 사용하지 않는 경우 '0' */
{
	NEXUS_SecurityClearKey 				key;
	NEXUS_SecurityEncryptedControlWord 	encryptedCW;
    unsigned int 						slot_index = DI_DSC_SLOTNUM_MAXIMUM;
	NEXUS_Error 						rc = NEXUS_SUCCESS;
	unsigned int 						i = 0;
	unsigned long 						useCWPK = FALSE;
#ifdef GRAP_KEY
	unsigned int 						pcr = 0;
#endif

	PrintEnter("DI_DSC_SetKey");

	PrintDebug("[DI_DSC_SetKey] DescId(0x%x) parity(%d) key_length(%d) vector_length(%d)\r\n",
				DescId,
				parity,
				key_length,
				vector_length);

	if (((key_length != 0) && (p_key_data == NULL))||((key_length == 0) && (p_key_data != NULL)))
	{
		PrintError("[DI_DSC_SetKey] No data\n");
		return DI_ERR_INVALID_PARAM;
	}

	if (((vector_length != 0) && (p_init_vector == NULL))||((vector_length == 0) && (p_init_vector != NULL)))
	{
		PrintError("[DI_DSC_SetKey] No data\n");
		return DI_ERR_INVALID_PARAM;
	}

	if ((key_length == 0) && (p_key_data == NULL)&&(vector_length == 0) && (p_init_vector == NULL))
	{
		PrintError("[DI_DSC_SetKey] No data\n");
		return DI_ERR_INVALID_PARAM;
	}

	VK_SEM_Get(s_ulDscSemId);


	NEXUS_Security_GetDefaultClearKey(&key);
	/* NEXUS_SecurityEncryptedControlWord type의 encryptedCW 을 default setting 및 initialize하는 nexus basic 함수가 없어서, 직접 initialize하도록 VK_MEM_Memset 함수를 사용함 */
	VK_MEM_Memset((void *)&encryptedCW, 0x00, sizeof(NEXUS_SecurityEncryptedControlWord));

#if defined(USE_INTERNAL_HANDLE)
	i = DescId;

	if (s_stDscSlotInfo[i].s_ulUseCWPK == TRUE)
	{
		useCWPK = TRUE;
	}
	else
	{
		useCWPK = FALSE;
	}

    slot_index = i;
#else
	for (i = 0; i < DI_DSC_SLOTNUM_MAXIMUM; i++)
	{
		if (s_stDscSlotInfo[i].keyHandle == (NEXUS_KeySlotHandle)DescId)
		{
			if (s_stDscSlotInfo[i].s_ulUseCWPK == TRUE)
			{
				useCWPK = TRUE;
			}
			else
			{
				useCWPK = FALSE;
			}

            slot_index = i;

			break;
		}
	}
#endif
	if (slot_index == DI_DSC_SLOTNUM_MAXIMUM)
	{
		PrintError("[DI_DSC_SetKey] There isn't matched slot!!\r\n");

		VK_SEM_Release(s_ulDscSemId);
		return DI_ERR_ERROR;
	}


	if ((key_length != 0) && (p_key_data != NULL))
	{
#ifdef GRAP_KEY
		/*if (s_stDscSlotInfo[i].ulDemuxId == 0)*/
		{
			DRV_VIDEO_GetCurrentStc(0, &pcr);

	//		PrintError("[DI_DSC_SetKey] 0x%08X %02X %04X \n", pcr, s_stDscSlotInfo[slot_index].ulDemuxId, s_stDscSlotInfo[slot_index].pid);

			if ((tempFileHandle != NULL) && (saveFileHandle == NULL))
			{
				if(writeflag[i] == 0)
				{
					VK_fprintf(tempFileHandle, "PID %01X %04X \n", s_stDscSlotInfo[i].ulDemuxId, s_stDscSlotInfo[i].pid);

					writeflag[i] = 1;
				}

				if (tempFileHandle == NULL)
				{
					writeflag[i] = 0;
				}

				if (useCWPK == TRUE)
				{
					if(cwpkFileHandle != NULL)
					{
						if(s_stSessInfo.useful == 1)
						{
							VK_fprintf(cwpkFileHandle, "CWPK %04X %08X %02X", s_stDscSlotInfo[i].pid,pcr,s_stSessInfo.key_length);

							for (i = 0; i < s_stSessInfo.key_length; i++)
							{
								VK_fprintf(cwpkFileHandle, " %02X", s_stSessInfo.keyData[i]);
							}

							VK_fprintf(cwpkFileHandle, "\n");
						}
					}
				}
				else
				{
					if(cwpkFileHandle != NULL)
					{
						fclose(cwpkFileHandle);
						cwpkFileHandle = NULL;
					}

				}
				VK_fprintf(tempFileHandle, "%08X %01X %04X", pcr, s_stDscSlotInfo[slot_index].ulDemuxId, s_stDscSlotInfo[slot_index].pid);
			}

		}
#endif

		if (!VK_memcmp((void *)s_stDscSlotInfo[slot_index].keyData[parity], (void *)p_key_data, key_length))
		{
			PrintDebug("Same key\n");

			VK_SEM_Release(s_ulDscSemId);
			return DI_ERR_OK;
		}
		else
		{
			VK_MEM_Memcpy((void *)s_stDscSlotInfo[slot_index].keyData[parity], (void *)p_key_data, key_length);
		}

#if defined(CONFIG_SECURITY_EXTENSION)
		NEXUS_Security_GetDefaultControlWordSettings(&encryptedCW);
#else
		VK_MEM_Memset((void *)&encryptedCW, 0x0, sizeof(NEXUS_SecurityEncryptedControlWord));
#endif

		if (useCWPK == TRUE)
		{
			PrintDebug("[DI_DSC_SetKey] (useCWPK == TRUE), key_length(%d)\r\n", key_length);
#if ((BCHP_CHIP == 7346 || BCHP_CHIP == 7344 || BCHP_CHIP == 7358) && (defined(CONFIG_CAS_IR)|| defined(CONFIG_CAS_NA))) || defined(CONFIG_CAS_ASKM)
			/* Do not use keyladderID for BCM7356 Irdeto */
			encryptedCW.keyladderID 		= NEXUS_SecurityKeyladderID_eA;
#else
			encryptedCW.keyladderID 		= NEXUS_SecurityKeyladderID_eB;
#endif
			encryptedCW.keyladderType 		= NEXUS_SecurityKeyladderType_e3Des;
			encryptedCW.operation 			= NEXUS_SecurityOperation_eDecrypt;
			encryptedCW.bRouteKey 			= true;
			encryptedCW.keySize 			= key_length;
#if defined(CONFIG_SECURITY_EXTENSION)
			encryptedCW.keyDestIVType		= NEXUS_SecurityKeyIVType_eNoIV;
			encryptedCW.keyGenCmdID			= NEXUS_SecurityKeyGenCmdID_eKeyGen;
			encryptedCW.bSwapAESKey			= false;
#endif
#if ((BCHP_CHIP == 7346 || BCHP_CHIP == 7344) && defined(CONFIG_CAS_IR)) || defined(CONFIG_CAS_ASKM)
			encryptedCW.custSubMode 		= NEXUS_SecurityCustomerSubMode_eGeneric_CA_64_45;
#elif defined(CONFIG_CAS_NA) && defined(CONFIG_CAS_NA_NOCS_1X)
			encryptedCW.custSubMode 		= NEXUS_SecurityCustomerSubMode_eGeneric_CA_64_5;
#else
			encryptedCW.custSubMode 		= NEXUS_SecurityCustomerSubMode_eGeneric_CA_64_4;
#endif

#if defined(CONFIG_CAS_ASKM) || defined(CONFIG_CAS_CX)
			encryptedCW.virtualKeyLadderID	= NEXUS_SecurityVirtualKeyladderID_eVKL2;
#else
			encryptedCW.virtualKeyLadderID 	= NEXUS_SecurityVirtualKeyladderID_eMax;
#endif

			if (parity == DI_DSC_DESC_ODD)
			{
				encryptedCW.keyEntryType = NEXUS_SecurityKeyType_eOdd;
			}
			else
			{
				encryptedCW.keyEntryType = NEXUS_SecurityKeyType_eEven;
			}

#if defined(CONFIG_CAS_NA) && defined(CONFIG_CAS_NA_NOCS_1X)
			encryptedCW.rootKeySrc = NEXUS_SecurityRootKeySrc_eOtpKeyA;
			encryptedCW.swizzleType = NEXUS_SecuritySwizzleType_eNone;
			encryptedCW.keyMode = NEXUS_SecurityKeyMode_eRegular;
			encryptedCW.bASKMMode = true;
#endif
			VK_MEM_Memset((void *)encryptedCW.keyData, 0, NEXUS_SECURITY_KEYLADDER_KEY_SIZE);
			VK_MEM_Memcpy((void *)encryptedCW.keyData, (void *)p_key_data, encryptedCW.keySize);

#if defined(USE_INTERNAL_HANDLE)
#if defined(CONFIG_CAS_NA) && defined(CONFIG_CAS_NA_NOCS_1X)
			rc = NEXUS_Security_GenerateKey5((NEXUS_KeySlotHandle)s_stDscSlotInfo[slot_index].keyHandle , &encryptedCW);
#else
			rc = NEXUS_Security_GenerateControlWord((NEXUS_KeySlotHandle)s_stDscSlotInfo[slot_index].keyHandle, &encryptedCW);
#endif
#else
#if defined(CONFIG_CAS_NA) && defined(CONFIG_CAS_NA_NOCS_1X)
			rc = NEXUS_Security_GenerateKey5((NEXUS_KeySlotHandle)DescId , &encryptedCW);
#else
			rc = NEXUS_Security_GenerateControlWord((NEXUS_KeySlotHandle)DescId, &encryptedCW);
#endif
#endif
			if ( rc != NEXUS_SUCCESS )
			{
				PrintError("[DI_DSC_SetKey] Error NEXUS_Security_GenerateControlWord 0x%x\r\n", rc);
				VK_SEM_Release(s_ulDscSemId);
				return DI_ERR_ERROR;
			}
#ifdef GRAP_KEY
		/*if(s_stDscSlotInfo[i].ulDemuxId == 0)*/
		{

			if (encryptedCW.keyEntryType == NEXUS_SecurityKeyType_eOdd)
			{
				PrintDebug("ODD\t::");

				if ((tempFileHandle != NULL) && (saveFileHandle == NULL))
				{
					VK_fprintf(tempFileHandle, " ODD");
				}
			}
			else
			{
				PrintDebug("EVEN\t::");

				if ((tempFileHandle != NULL) && (saveFileHandle == NULL))
				{
					VK_fprintf(tempFileHandle, " EVEN");
				}
			}

			for (i = 0; i < key_length; i++)
			{
				PrintDebug(" 0x%02X", p_key_data[i]);
			}

			PrintDebug("\n");

			if ((tempFileHandle != NULL) && (saveFileHandle == NULL))
			{
				VK_fprintf(tempFileHandle, " %02X", key_length);

				for (i = 0; i < key_length; i++)
				{
					VK_fprintf(tempFileHandle, " %02X", p_key_data[i]);
				}

				VK_fprintf(tempFileHandle, "\n");
			}
		}
#endif
		}
		else
		{
			PrintDebug("[DI_DSC_SetKey] (useCWPK == FALSE)\r\n");

			if (parity == DI_DSC_DESC_ODD)
			{
				key.keyEntryType = NEXUS_SecurityKeyType_eOdd;
			}
			else
			{
				key.keyEntryType = NEXUS_SecurityKeyType_eEven;
			}

			key.keySize = key_length;

			VK_MEM_Memcpy((void *)key.keyData, (void *)p_key_data, key.keySize);

#if defined(USE_INTERNAL_HANDLE)
			rc = NEXUS_Security_LoadClearKey((NEXUS_KeySlotHandle)s_stDscSlotInfo[slot_index].keyHandle, &key);
#else
			rc = NEXUS_Security_LoadClearKey((NEXUS_KeySlotHandle)DescId, &key);
#endif
			if (rc != NEXUS_SUCCESS)
			{
				PrintError("[DI_DSC_SetKey] Error NEXUS_Security_LoadClearKey 0x%x\r\n", rc);

				VK_SEM_Release(s_ulDscSemId);
				return DI_ERR_ERROR;
			}

			for (i = 0; i < key_length; i++)
			{
				PrintDebug(" 0x%02X", p_key_data[i]);
			}

			PrintDebug("\n");

#ifdef GRAP_KEY
			/*if(s_stDscSlotInfo[i].ulDemuxId == 0)*/
			{
				if (key.keyEntryType == NEXUS_SecurityKeyType_eOdd)
				{
					PrintDebug("ODD\t::");

					if ((tempFileHandle != NULL) && (saveFileHandle == NULL))
					{
						VK_fprintf(tempFileHandle, " ODD");
					}
				}
				else
				{
					PrintDebug("EVEN\t::");

					if ((tempFileHandle != NULL) && (saveFileHandle == NULL))
					{
						VK_fprintf(tempFileHandle, " EVEN");
					}
				}

				for (i = 0; i < key_length; i++)
				{
					PrintDebug(" 0x%02X", p_key_data[i]);
				}

				PrintDebug("\n");

				if ((tempFileHandle != NULL) && (saveFileHandle == NULL))
				{
					VK_fprintf(tempFileHandle, " %02X", key_length);

					for (i = 0; i < key_length; i++)
					{
						VK_fprintf(tempFileHandle, " %02X", p_key_data[i]);
					}

					VK_fprintf(tempFileHandle, "\n");
				}
			}
#endif
		}
	}

	if ((vector_length != 0) && (p_init_vector != NULL))
	{
		PrintDebug("[DI_DSC_SetKey] IV Setting\r\n");
		NEXUS_Security_GetDefaultClearKey(&key);
		if (parity == DI_DSC_DESC_ODD)
		{
			key.keyEntryType = NEXUS_SecurityKeyType_eOdd;
		}
		else
		{
			key.keyEntryType = NEXUS_SecurityKeyType_eEven;
		}

#if (NEXUS_VERSION >= 1100)
		key.keyIVType		= NEXUS_SecurityKeyIVType_eIV;
#endif
		key.keySize 		= vector_length;

		VK_MEM_Memcpy((void *)key.keyData, (void *)p_init_vector, vector_length);
#if defined(USE_INTERNAL_HANDLE)
		rc = NEXUS_Security_LoadClearKey((NEXUS_KeySlotHandle)s_stDscSlotInfo[slot_index].keyHandle, &key);
#else
		rc = NEXUS_Security_LoadClearKey((NEXUS_KeySlotHandle)DescId, &key);
#endif
		if (rc != NEXUS_SUCCESS)
		{
			PrintError("[DI_DSC_SetKey] Error NEXUS_Security_LoadClearKey 0x%x\r\n", rc);

			VK_SEM_Release(s_ulDscSemId);
			return DI_ERR_ERROR;
		}
	}

	VK_SEM_Release(s_ulDscSemId);
	PrintExit("DI_DSC_SetKey");

	return DI_ERR_OK;
}

/* 20090701 :: POOH :: 사용시 DI_DSC_SetKey 이전에 불러야 됨. */
DI_ERR_CODE DI_DSC_SetProtectionKey(HUINT32 DescId, HUINT8 *pCWPK, HUINT32 key_length)
{
	NEXUS_SecurityEncryptedSessionKey 	encryptedSessionkey;
#if defined(CONFIG_CAS_NA) && defined(CONFIG_CAS_NA_NOCS_1X)
	NEXUS_SecurityEncryptedControlWord	CW;
	/* Initialize the L2 protected keys, it is defined as L2ProtectingKey in the CC_BCM7356Family_DALImplementationGuidelines_030002.pdf and CC_BCM7358Family_DALImplementationGuidelines_030001.pdf documents.
	this key should only be used for bcm7356 chipset, and it was confirmed from Nagra. */
	const unsigned char L2ProtectionKey[16] = {0xdf, 0x72, 0xe9, 0x15, 0x9a, 0x30, 0xc6, 0x13, 0xec, 0x5b, 0x00, 0x42, 0xb8, 0xed, 0x5e, 0x0a};
#endif
	NEXUS_Error 						rc = NEXUS_SUCCESS;
    int 								slot_index = DI_DSC_SLOTNUM_MAXIMUM;
	int 								i = 0;

	PrintEnter("DI_DSC_SetProtectionKey");

	PrintDebug("[DI_DSC_SetProtectionKey] DescId(0x%x) key_length(%d)\n",
				DescId,
				key_length);

	if((pCWPK == NULL)||(key_length == 0))
		return DI_ERR_INVALID_PARAM;

	VK_SEM_Get(s_ulDscSemId);

#if defined(USE_INTERNAL_HANDLE)
	i = DescId;
	slot_index = i;
#else
	for (i = 0; i < DI_DSC_SLOTNUM_MAXIMUM; i++)
	{
		if (s_stDscSlotInfo[i].keyHandle == (NEXUS_KeySlotHandle)DescId)
		{
			PrintDebug("[DI_DSC_SetProtectionKey] It be found slot(%d)\r\n", i);

            slot_index = i;

			break;
		}
	}
#endif
	if (slot_index == DI_DSC_SLOTNUM_MAXIMUM)
	{
		PrintError("[DI_DSC_SetProtectionKey] There isn't matched slot!!\r\n");
		VK_SEM_Release(s_ulDscSemId);
		return DI_ERR_ERROR;
	}

	if (pCWPK != NULL)
	{
		/* Generate session key */
#if defined(CONFIG_SECURITY_EXTENSION)
		NEXUS_Security_GetDefaultSessionKeySettings(&encryptedSessionkey);
#else
		VK_MEM_Memset((void *)&encryptedSessionkey, 0x0, sizeof(NEXUS_SecurityEncryptedSessionKey));
#endif

#if ((BCHP_CHIP == 7346 || BCHP_CHIP == 7344 || BCHP_CHIP == 7358) && (defined(CONFIG_CAS_IR)|| defined(CONFIG_CAS_NA))) || defined(CONFIG_CAS_ASKM)
		/* Do not use keyladderID for BCM7356 Irdeto */
		encryptedSessionkey.keyladderID			= NEXUS_SecurityKeyladderID_eA;
#else
		encryptedSessionkey.keyladderID			= NEXUS_SecurityKeyladderID_eB;
#endif
		encryptedSessionkey.keyladderType		= NEXUS_SecurityKeyladderType_e3Des;
		encryptedSessionkey.swizzleType			= NEXUS_SecuritySwizzleType_eNone;
		encryptedSessionkey.cusKeyL				= 0x0;
		encryptedSessionkey.cusKeyH				= 0x0;
#if defined(CONFIG_CAS_CX)
		encryptedSessionkey.cusKeyVarL			= 0x77;
		encryptedSessionkey.cusKeyVarH			= 0x40;
#else
		encryptedSessionkey.cusKeyVarL			= 0x0;
		encryptedSessionkey.cusKeyVarH			= 0xFF;
#endif
#if ((BCHP_CHIP == 7346 || BCHP_CHIP == 7344) && defined(CONFIG_CAS_IR)) || defined(CONFIG_CAS_ASKM)
		encryptedSessionkey.rootKeySwap			= true;
#endif

#if defined(CONFIG_SECURITY_EXTENSION)
		encryptedSessionkey.keyGenCmdID			= NEXUS_SecurityKeyGenCmdID_eKeyGen;
		encryptedSessionkey.sessionKeyOp		= NEXUS_SecuritySessionKeyOp_eNoProcess;
#if (((BCHP_CHIP == 7346)|| (BCHP_CHIP == 7344)) && (defined(CONFIG_CAS_IR) || defined(CONFIG_CAS_NA))) || ((BCHP_CHIP == 7358) && defined(CONFIG_CAS_NA))|| defined(CONFIG_CAS_ASKM)
		encryptedSessionkey.bASKMMode			= true;
		encryptedSessionkey.rootKeySrc			= NEXUS_SecurityRootKeySrc_eOtpKeyA;
#else
		encryptedSessionkey.bASKMMode			= false;
		encryptedSessionkey.rootKeySrc			= NEXUS_SecurityRootKeySrc_eOtpKeyB;
#endif
		encryptedSessionkey.bSwapAESKey			= false;
		encryptedSessionkey.keyDestIVType		= NEXUS_SecurityKeyIVType_eNoIV;
#else
        encryptedSessionkey.rootKeySrc			= NEXUS_SecurityRootKeySrc_eOtpKeyB;
#endif
		encryptedSessionkey.bRouteKey			= false;
		encryptedSessionkey.operation			= NEXUS_SecurityOperation_eDecrypt;
		encryptedSessionkey.operationKey2		= NEXUS_SecurityOperation_eEncrypt;
#if defined(CONFIG_CAS_CX)
		encryptedSessionkey.keyEntryType		= NEXUS_SecurityKeyType_eOddAndEven;
#else
		encryptedSessionkey.keyEntryType		= NEXUS_SecurityKeyType_eOdd;
#endif
#if ((BCHP_CHIP == 7346 || BCHP_CHIP == 7344) && defined(CONFIG_CAS_IR)) || defined(CONFIG_CAS_ASKM)
		encryptedSessionkey.custSubMode 		= NEXUS_SecurityCustomerSubMode_eGeneric_CA_64_45;
#elif defined(CONFIG_CAS_NA) && defined(CONFIG_CAS_NA_NOCS_1X)
		encryptedSessionkey.custSubMode 		= NEXUS_SecurityCustomerSubMode_eGeneric_CA_64_5;
#else
		encryptedSessionkey.custSubMode 		= NEXUS_SecurityCustomerSubMode_eGeneric_CA_64_4;
#endif

#if defined(CONFIG_CAS_ASKM) || defined(CONFIG_CAS_CX)
		encryptedSessionkey.virtualKeyLadderID 	= NEXUS_SecurityVirtualKeyladderID_eVKL2;
#else
		encryptedSessionkey.virtualKeyLadderID 	= NEXUS_SecurityVirtualKeyladderID_eMax;
#endif
		encryptedSessionkey.keyMode				= NEXUS_SecurityKeyMode_eRegular;

		VK_MEM_Memset((void *)encryptedSessionkey.keyData, 0, NEXUS_SECURITY_KEYLADDER_KEY_SIZE);

#if ((BCHP_CHIP == 7346 || BCHP_CHIP == 7344) && defined(CONFIG_CAS_IR)) || defined(CONFIG_CAS_ASKM)
		/* BCM7356 for irdeto, do not swap session key */
		VK_MEM_Memcpy((void *)encryptedSessionkey.keyData, (void *)pCWPK, key_length);
#elif defined(CONFIG_CAS_NA) && defined(CONFIG_CAS_NA_NOCS_1X)
		VK_MEM_Memcpy(&(encryptedSessionkey.keyData[0]), &(L2ProtectionKey[8]), sizeof(HUINT8)*8);
		VK_MEM_Memcpy(&(encryptedSessionkey.keyData[8]), &(L2ProtectionKey[0]), sizeof(HUINT8)*8);
#else
		if (key_length == 16)
		{
			VK_MEM_Memcpy(&(encryptedSessionkey.keyData[0]), &(pCWPK[8]), sizeof(HUINT8)*8);
			VK_MEM_Memcpy(&(encryptedSessionkey.keyData[8]), &(pCWPK[0]), sizeof(HUINT8)*8);
		}
		else
		{
			VK_MEM_Memcpy((void *)encryptedSessionkey.keyData, (void *)pCWPK, key_length);
		}
#endif
#if defined(USE_INTERNAL_HANDLE)
		rc = NEXUS_Security_GenerateSessionKey((NEXUS_KeySlotHandle)s_stDscSlotInfo[slot_index].keyHandle, &encryptedSessionkey);
#else
		rc = NEXUS_Security_GenerateSessionKey((NEXUS_KeySlotHandle)DescId, &encryptedSessionkey);
#endif
		if (rc != NEXUS_SUCCESS)
		{
			PrintError("[DI_DSC_SetProtectionKey] NEXUS_Security_GenerateSessionKey rc=0x%x\r\n",rc);
			VK_SEM_Release(s_ulDscSemId);
			return DI_ERR_ERROR;
		}

#if defined(CONFIG_CAS_NA) && defined(CONFIG_CAS_NA_NOCS_1X)
		NEXUS_Security_GetDefaultControlWordSettings(&CW);
		CW.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CA_64_5;
		CW.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eMax;
		CW.keyladderID = NEXUS_SecurityKeyladderID_eA;
		CW.keyGenCmdID   = NEXUS_SecurityKeyGenCmdID_eKeyGen;
		CW.keyladderType = NEXUS_SecurityKeyladderType_e3Des;
		CW.rootKeySrc = NEXUS_SecurityRootKeySrc_eOtpKeyA;
		CW.keyEntryType = NEXUS_SecurityKeyType_eOdd;
		CW.keyDestIVType = NEXUS_SecurityKeyIVType_eNoIV;
		CW.swizzleType = NEXUS_SecuritySwizzleType_eNone;
		CW.bASKMMode = true;
		CW.keyMode = NEXUS_SecurityKeyMode_eRegular;
		CW.operation = NEXUS_SecurityOperation_eDecrypt;
		CW.bSwapAESKey = false;
		CW.bRouteKey = false;
		CW.keySize = 16;

		VK_MEM_Memset((void *)CW.keyData, 0, NEXUS_SECURITY_KEYLADDER_KEY_SIZE);
		VK_MEM_Memcpy(&(CW.keyData[0]), &(pCWPK[8]), sizeof(HUINT8)*8);
		VK_MEM_Memcpy(&(CW.keyData[8]), &(pCWPK[0]), sizeof(HUINT8)*8);
#if defined(USE_INTERNAL_HANDLE)
		rc = NEXUS_Security_GenerateControlWord((NEXUS_KeySlotHandle)s_stDscSlotInfo[slot_index].keyHandle, &CW);
#else
		rc = NEXUS_Security_GenerateControlWord((NEXUS_KeySlotHandle)DescId, &CW);
#endif
		if (rc != NEXUS_SUCCESS)
		{
			PrintError("[DI_DSC_SetProtectionKey] NEXUS_Security_GenerateControlWord rc=0x%x\r\n",rc);
			VK_SEM_Release(s_ulDscSemId);
			return DI_ERR_ERROR;
		}
#endif
		s_stDscSlotInfo[slot_index].s_ulUseCWPK = TRUE;

#ifdef GRAP_KEY
		if ((cwpkFileHandle != NULL)&&(tempFileHandle != NULL) && (saveFileHandle == NULL))
		{

			if(s_stSessInfo.useful == 0xff)
			{
				VK_MEM_Memset((void *)s_stSessInfo.keyData, 0, NEXUS_SECURITY_KEYLADDER_KEY_SIZE);
				for (i = 0; i < 16; i++)
				{
					s_stSessInfo.keyData[i] = pCWPK[i];
				}

				s_stSessInfo.useful  = 1;
				s_stSessInfo.key_length = 16;

			}
			else
			{

				VK_MEM_Memset((void *)s_stSessInfo.keyData, 0, NEXUS_SECURITY_KEYLADDER_KEY_SIZE);

				for (i = 0; i < 16; i++)
				{
					s_stSessInfo.keyData[i] = pCWPK[i];
				}

				s_stSessInfo.useful  = 1;

				s_stSessInfo.key_length = 16;
			}
		}
#endif

	}
	else
	{
		PrintError("[DI_DSC_SetProtectionKey] pCWPK is NULL\r\n");
		VK_SEM_Release(s_ulDscSemId);

		return DI_ERR_ERROR;
	}

	VK_SEM_Release(s_ulDscSemId);
	PrintExit("DI_DSC_SetProtectionKey");

	return DI_ERR_OK;
}

DI_ERR_CODE DI_DSC_ClearProtectionKey(HUINT32 DescId)
{
	int slot_index = DI_DSC_SLOTNUM_MAXIMUM;
	int i = 0;

	PrintDebug("[DI_DSC_ClearProtectionKey] DescId(0x%x)\n",
				DescId);

	VK_SEM_Get(s_ulDscSemId);

#if defined(USE_INTERNAL_HANDLE)
	i = DescId;
	slot_index = i;
#else
	for (i = 0; i < DI_DSC_SLOTNUM_MAXIMUM; i++)
	{
		if (s_stDscSlotInfo[i].keyHandle == (NEXUS_KeySlotHandle)DescId)
		{
			PrintDebug("[DI_DSC_SetProtectionKey] It be found slot(%d)\r\n", i);

			slot_index = i;

			break;
		}
	}
#endif
	if (slot_index == DI_DSC_SLOTNUM_MAXIMUM)
	{
		VK_SEM_Release(s_ulDscSemId);
		return DI_ERR_ERROR;
	}

	if (s_stDscSlotInfo[slot_index].s_ulUseCWPK == TRUE)
	{
		s_stDscSlotInfo[slot_index].s_ulUseCWPK = FALSE;
	}

	VK_SEM_Release(s_ulDscSemId);
	return DI_ERR_OK;
}


DI_ERR_CODE DI_DSC_SetMulti2SysKey(HUINT32 DescId, DI_DSC_EncryptionSettings *encryptionSetting)
{
	NEXUS_SecurityMulti2Settings    config_multi2;
	NEXUS_Error rc = NEXUS_SUCCESS;
	int 		i = 0;

	HAPPY(rc);
	PrintError("###### %s ###\r\n", __FUNCTION__);
	PrintError("[%s] DescId(0x%x)\n", __FUNCTION__, DescId);

	VK_SEM_Get(s_ulDscSemId);

#if defined(USE_INTERNAL_HANDLE)
	i = DescId;
#else
	for (i = 0; i < DI_DSC_SLOTNUM_MAXIMUM; i++)
	{
		if (s_stDscSlotInfo[i].keyHandle == (NEXUS_KeySlotHandle)DescId)
		{
			PrintDebug("[%s] It be found slot(%d)\r\n", __FUNCTION__, i);
			PrintDebug("[%s] s_stDscSlotInfo[%d].keyHandle = %x\r\n", __FUNCTION__, i, s_stDscSlotInfo[i].keyHandle);
			PrintDebug("[%s] DescId = %x\r\n", __FUNCTION__, DescId);
			PrintDebug("[%s] s_stDscSlotInfo[%d].bAllocated = %d \r\n", __FUNCTION__, i, s_stDscSlotInfo[i].bAllocated);
			PrintDebug("[%s] s_stDscSlotInfo[%d].lUsedCount = %d \r\n", __FUNCTION__, i, s_stDscSlotInfo[i].lUsedCount);
			break;
		}
	}
#endif

	if ( i == DI_DSC_SLOTNUM_MAXIMUM )
	{
		PrintError("[%s] There isn't matched slot!!\r\n", __FUNCTION__);
		VK_SEM_Release(s_ulDscSemId);
		return DI_ERR_ERROR;
	}

	if (s_stDscSlotInfo[i].bAllocated == TRUE)
	{
		PrintDebug("[%s] DI_DSC_DESC_MULTI2\r\n", __FUNCTION__);

		NEXUS_Security_GetDefaultMulti2Settings(&config_multi2);

		config_multi2.multi2KeySelect = encryptionSetting->multi2KeySelect;
		config_multi2.multi2Rounds = encryptionSetting->multi2Rounds;

		VK_MEM_Memcpy((void *)config_multi2.multi2SysKey, (void *)encryptionSetting->multi2SysKey, 32);

		if ( NEXUS_Security_ConfigMulti2((NEXUS_KeySlotHandle)s_stDscSlotInfo[i].keyHandle, &config_multi2) != 0 )
		{
			PrintError("[%s] Error NEXUS_Security_ConfigMulti2 0x%x\r\n", __FUNCTION__, rc);
			VK_SEM_Release(s_ulDscSemId);
			return DI_ERR_ERROR;
		}
	}

	VK_SEM_Release(s_ulDscSemId);
	PrintExit("%s", __FUNCTION__);
	return DI_ERR_OK;
}


DRV_Error DRV_DSC_ClearWithOnlyPid( HUINT16 pid )
{
	NEXUS_Error 	rc = NEXUS_SUCCESS;
	int 			i = 0;
	DRV_Error		nRet = DRV_OK;
	unsigned int	ulGetPidChannel = INVALID_HANDLE;
	unsigned int	ulNumChannel = 0;

	PrintEnter("DRV_DSC_ClearWithOnlyPid");

	PrintDebug("[DRV_DSC_ClearWithOnlyPid]  pid(0x%x)\n",pid);

	VK_SEM_Get(s_ulDscSemId);

	for (i = 0; i < DI_DSC_SLOTNUM_MAXIMUM; i++)
	{
		if (s_stDscSlotInfo[i].pid == pid)
		{
			PrintDebug("[DRV_DSC_ClearWithOnlyPid] It be found slot(%d)\r\n", i);
			PrintDebug("[DRV_DSC_ClearWithOnlyPid] s_stDscSlotInfo[%d].keyHandle = %x\r\n", i, s_stDscSlotInfo[i].keyHandle);
			PrintDebug("[DRV_DSC_ClearWithOnlyPid] s_stDscSlotInfo[%d].bAllocated = %d \r\n", i, s_stDscSlotInfo[i].bAllocated);
			PrintDebug("[DRV_DSC_ClearWithOnlyPid] s_stDscSlotInfo[%d].lUsedCount = %d \r\n", i, s_stDscSlotInfo[i].lUsedCount);

			break;
		}
	}

	if (i == DI_DSC_SLOTNUM_MAXIMUM)
	{
		PrintError("[DRV_DSC_ClearWithOnlyPid] There isn't matched slot!!\r\n");

		VK_SEM_Release(s_ulDscSemId);
		return DRV_ERR;
	}

	if (s_stDscSlotInfo[i].lUsedCount == 0)
	{
		PrintError("[DRV_DSC_ClearWithOnlyPid] (s_stDscSlotInfo[i].lUsedCount==0)\r\n");

		VK_SEM_Release(s_ulDscSemId);
		return DRV_ERR;
	}

	if(s_stDscSlotInfo[i].s_ulUseCWPK )
	{
		DI_DSC_ClearProtectionKey((HUINT32)s_stDscSlotInfo[i].keyHandle);
	}


	if (s_stDscSlotInfo[i].bAllocated == TRUE)
	{
/* 20100811 :: POOH :: IRDETO :: Add one DVB stream PID to load for the descrambler channel. */
/* 					It can load multiple PIDs with the same control word to one descrambler. */
/*					s_stDscSlotInfo[i].ulPidChannel을 사용하지 않음. */
		ulNumChannel = DRV_DEMUX_GetPIDChannelForDSC(s_stDscSlotInfo[i].ulDemuxId, pid, &ulGetPidChannel);

		PrintDebug("[DRV_DSC_ClearWithOnlyPid] pid = %x ulGetPidChannel = %x ulNumChannel = %d \r\n",pid, ulGetPidChannel,ulNumChannel);

		if ((ulGetPidChannel == INVALID_HANDLE) || (ulNumChannel < 1))
		{
			PrintError("[DRV_DSC_ClearWithOnlyPid] Get invalid pidchannel = %x\r\n", ulGetPidChannel);
			VK_SEM_Release(s_ulDscSemId);
			return DRV_ERR;
		}

		if (ulNumChannel == 1)
		{
			rc = NEXUS_Security_RemovePidChannelFromKeySlot(s_stDscSlotInfo[i].keyHandle, ulGetPidChannel);
			if ( rc != NEXUS_SUCCESS )
			{
				PrintError("[DRV_DSC_ClearWithOnlyPid] Error NEXUS_Security_RemovePidChannelFromKeySlot 0x%x\r\n", rc);
				VK_SEM_Release(s_ulDscSemId);
				return DRV_ERR;
			}
		}

		nRet = DRV_DEMUX_DisconnectPIDChannelFromDSC(ulGetPidChannel);
		if (nRet != DRV_OK)
		{
			PrintError("[DRV_DSC_ClearWithOnlyPid] nRet= %x\r\n", nRet);

			VK_SEM_Release(s_ulDscSemId);
			return DRV_ERR;
		}

		if (s_stDscSlotInfo[i].lUsedCount == 1)
		{
			s_stDscSlotInfo[i].ulPidChannel = INVALID_HANDLE;
		}

		s_stDscSlotInfo[i].lUsedCount--;
	}


	PrintExit("DRV_DSC_ClearWithOnlyPid");

	VK_SEM_Release(s_ulDscSemId);
	return DRV_OK;
}


/*****************************************************************
*
* Function Name:	DRV_DSC_CloseWithOnlyPid
* Inputs
* Outputs
* Returns
* Description
*
*****************************************************************/
DRV_Error DRV_DSC_CloseWithOnlyPid(HUINT16 pid)
{
	int i = 0;

	PrintEnter("DRV_DSC_CloseWithOnlyPid");

	PrintDebug("[DRV_DSC_CloseWithOnlyPid] PID(%x)\n",pid);

	VK_SEM_Get(s_ulDscSemId);

	for (i = 0; i < DI_DSC_SLOTNUM_MAXIMUM; i++)
	{
		if (s_stDscSlotInfo[i].pid == pid)
		{
			PrintDebug("[DRV_DSC_CloseWithOnlyPid] It be found slot(%d)\r\n", i);
			PrintDebug("[DRV_DSC_CloseWithOnlyPid] s_stDscSlotInfo[%d].keyHandle = %x\r\n",i,s_stDscSlotInfo[i].keyHandle);
			break;
		}
	}

	if (i == DI_DSC_SLOTNUM_MAXIMUM)
	{
		PrintError("[DI_DSC_Close] There isn't matched slot!!\r\n");

		VK_SEM_Release(s_ulDscSemId);
		return DRV_ERR;
	}

	if (s_stDscSlotInfo[i].lUsedCount != 0)
	{
		PrintError("[DRV_DSC_CloseWithOnlyPid] (s_stDscSlotInfo[i].lUsedCount != 0)\r\n");

		VK_SEM_Release(s_ulDscSemId);
		return DRV_ERR;
	}

	if (s_stDscSlotInfo[i].bAllocated == TRUE)
	{
		NEXUS_Security_FreeKeySlot(s_stDscSlotInfo[i].keyHandle);

		s_stDscSlotInfo[i].bAllocated = FALSE;
		s_stDscSlotInfo[i].keyHandle = NULL;
		s_stDscSlotInfo[i].s_ulUseCWPK = FALSE;
		s_stDscSlotInfo[i].lUsedCount = 0;
	}
	else
	{
		PrintError("[DRV_DSC_CloseWithOnlyPid] Didn't allocate \r\n");
		VK_SEM_Release(s_ulDscSemId);
		return DRV_ERR;
	}

	PrintExit("DRV_DSC_CloseWithOnlyPid");
	VK_SEM_Release(s_ulDscSemId);
	return DRV_ERR;
}


NEXUS_KeySlotHandle DRV_DSC_GetKeySlotHandle(HUINT32 ulDemuxId, HUINT16 uspid)
{
	int i = 0;

	PrintDebug("[%s] demuxid(%d), pid(0x%x)\n", __func__, ulDemuxId, uspid);

	VK_SEM_Get(s_ulDscSemId);

	for(i = 0; i < DI_DSC_SLOTNUM_MAXIMUM; i++)
	{
		PrintDebug("[%s] s_stDscSlotInfo[%d].ulDemuxId(0x%x), s_stDscSlotInfo[i].pid(0x%x)\r\n", __func__, i, s_stDscSlotInfo[i].ulDemuxId, s_stDscSlotInfo[i].pid);
		if((s_stDscSlotInfo[i].ulDemuxId == ulDemuxId) && (s_stDscSlotInfo[i].pid == uspid))
		{
			PrintError("[%s] It be found slot(%d)\r\n", __func__, i);
			PrintError("[%s] s_stDscSlotInfo[%d].keyHandle = %x\r\n", __func__, i, s_stDscSlotInfo[i].keyHandle);
			break;
		}
	}

	if (i == DI_DSC_SLOTNUM_MAXIMUM)
	{
		PrintError("[%s] There isn't matched slot!!\r\n", __func__);

		VK_SEM_Release(s_ulDscSemId);
		return NULL;
	}

	if((s_stDscSlotInfo[i].bAllocated == TRUE) && (s_stDscSlotInfo[i].keyHandle != NULL))
	{
		VK_SEM_Release(s_ulDscSemId);
		return s_stDscSlotInfo[i].keyHandle;
	}
	else
	{
		PrintError("[%s] Didn't allocate \r\n", __func__);
		VK_SEM_Release(s_ulDscSemId);
		return NULL;
	}
}

#if defined(CONFIG_CAS_VE)
DI_ERR_CODE BRCM_SetVmxDemuxId(HUINT32 ulDemuxId)
{
	PrintDebug("[BRCM_SetVmxDemuxId] dmxId(%d -> %d)", s_ulVmxDemuxId, ulDemuxId);
	s_ulVmxDemuxId = ulDemuxId;
	return DI_ERR_OK;
}

NEXUS_KeySlotHandle BRCM_GetStreamHandle(HUINT16 usStreamPid)
{
	HUINT32 i =  0;

	NEXUS_KeySlotHandle 			keyHandle = NULL;

	if (usStreamPid == 0x2000)	// check NULL_PID
	{
		PrintError("skip this pid(0x%X) \n", usStreamPid);
		return NULL;
	}

	PrintDebug("\n\t[Enter] usStreamPid(0x%08x)\n",  usStreamPid);

	VK_SEM_Get(s_ulDscSemId);

	for (i = 0; i < DI_DSC_SLOTNUM_MAXIMUM; i++)
	{
		if ((s_stDscSlotInfo[i].bAllocated == TRUE) && (s_stDscSlotInfo[i].keyHandle != NULL)
				&& (s_stDscSlotInfo[i].pid ==  usStreamPid) && (s_stDscSlotInfo[i].ulDemuxId == s_ulVmxDemuxId))
		{
			keyHandle = s_stDscSlotInfo[i].keyHandle;
			PrintError("OK............idx(%d) keyHanle(%p) dmx(%d) pid(0x%X) \n", i, keyHandle, s_stDscSlotInfo[i].ulDemuxId, s_stDscSlotInfo[i].pid);
			break;
		}
	}

	VK_SEM_Release(s_ulDscSemId);

	if (keyHandle == NULL)
	{
		PrintError("Can't find keyslot!! pid(0x%X) curDmxId(%d) \n", usStreamPid, s_ulVmxDemuxId);
	}

	PrintDebug("\n\t[Exit] keyHandle(0x%08x)\n",  keyHandle);

	return keyHandle;
}

DI_ERR_CODE BRCM_SetAlgorithmConfig(HUINT8 ucSvcIdx, HUINT8 ucEncAlgo)
{
	PrintDebug("************************ DRV : BRCM_SetAlgorithmConfig. ************************ \n");
	PrintDebug("\t[Enter] bServiceIdx(0x%x ulSecAlgorithm(%d))\n",  ucSvcIdx, ucEncAlgo);

	if ((ucSvcIdx < VMX_SVCIDX_START) || (ucSvcIdx > VMX_SVCIDX_END))
	{
		PrintError("Error! Invalid service index(0x%X)\n", ucSvcIdx);
		return DI_ERR_ERROR;
	}

	s_aucVmxEncAlgo[ucSvcIdx - VMX_SVCIDX_START] = ucEncAlgo;
	return DI_ERR_OK;
}

HINT32 BRCM_GetAlgorithmConfig(HUINT8 ucServiceIdx,
				                             NEXUS_SecurityAlgorithm *alg,
				                             NEXUS_SecurityAlgorithmVariant *algVar,
				                             NEXUS_SecurityTerminationMode *termMode,
				                             NEXUS_SecurityIVMode *ivMode,
				                             NEXUS_SecuritySolitarySelect *solSelect,
				                             NEXUS_SecurityAesCounterSize *aesCounterSize,
				                             NEXUS_SecurityDvbScrambleLevel *dvbDescrambleLevel,
				                             NEXUS_SecurityAlgorithmConfigDestination *configDest
				                           )
{
	PrintDebug("************************ DRV : BRCM_GetAlgorithmConfig. ************************ \n");
	PrintDebug("\t[Enter] ucServiceIdx(0x%x) \n",  ucServiceIdx);

	if ((ucServiceIdx < VMX_SVCIDX_START) || (ucServiceIdx > VMX_SVCIDX_END))
	{
		PrintError("Error! Invalid service index(0x%X)\n", ucServiceIdx);
		return 0;
	}

	PrintDebug("\t encAlgo(%d)\n",  s_aucVmxEncAlgo[ucServiceIdx - VMX_SVCIDX_START]);

	switch(s_aucVmxEncAlgo[ucServiceIdx - VMX_SVCIDX_START])
	{
		case 2:		/* eEncAlg_AESECBT */
			PrintDebug("\t eEncAlg_AESECBT\n");
			if( alg ) 					*alg = NEXUS_SecurityAlgorithm_eAes;
			if( algVar ) 				*algVar = NEXUS_SecurityAlgorithmVariant_eEcb;
			if( termMode ) 				*termMode = NEXUS_SecurityTerminationMode_eClear;
			if( ivMode ) 				*ivMode = NEXUS_SecurityIVMode_eRegular;
			if( solSelect ) 			*solSelect = NEXUS_SecuritySolitarySelect_eClear;
			if( dvbDescrambleLevel ) 	*dvbDescrambleLevel = NEXUS_SecurityDvbScrambleLevel_eTs;
			break;

		case 7: /* eEncAlg_AESCBC1 */
			PrintDebug("\t eEncAlg_AESCBC1\n");
			if( alg ) 					*alg = NEXUS_SecurityAlgorithm_eAes;
			if( algVar ) 				*algVar = NEXUS_SecurityAlgorithmVariant_eCbc;
			if( termMode ) 				*termMode = NEXUS_SecurityTerminationMode_eBlock;
			if( ivMode ) 				*ivMode = NEXUS_SecurityIVMode_eRegular;
			if( solSelect ) 			*solSelect = NEXUS_SecuritySolitarySelect_eClear;
			if( dvbDescrambleLevel ) 	*dvbDescrambleLevel = NEXUS_SecurityDvbScrambleLevel_eTs;
			break;

		default:	/* CSA와 동일하게.. */
		case 5: 	/* eEncAlg_DVBCSA */
			PrintDebug("\t eEncAlg_DVBCSA\n");
			if( alg ) 					*alg = NEXUS_SecurityAlgorithm_eDvb;
			if( algVar ) 				*algVar = NEXUS_SecurityAlgorithmVariant_eEcb;
			if( termMode ) 				*termMode = NEXUS_SecurityTerminationMode_eClear;
			if( ivMode ) 				*ivMode = NEXUS_SecurityIVMode_eRegular;
			if( solSelect ) 			*solSelect = NEXUS_SecuritySolitarySelect_eClear;
			if( dvbDescrambleLevel ) 	*dvbDescrambleLevel = NEXUS_SecurityDvbScrambleLevel_eTs;
			break;
	}

	PrintDebug("\t[Exit]\n");

	HAPPY(aesCounterSize);
	HAPPY(configDest);

	/*
		Returns:
		True if STB middleware populate and set the algorithm
		False unknown Service Index or NULL pi
	*/
	return 1;
}
#endif


#if defined(CONFIG_PVR_INLINE_CRYPTO_DSC_KEYSLOT_SHARE)

DRV_Error DRV_DSC_OpenKeySlotForRecord(HUINT32 ulChIndex, NEXUS_KeySlotHandle *pKeySlotHandle)
{
	NEXUS_KeySlotHandle	keyHandle = NULL;
	NEXUS_Error	rc = NEXUS_SUCCESS;
	DRV_Error	drvErr = DRV_OK;
	NEXUS_SecurityKeySlotSettings   keySlotSettings;
	HUINT32	recindex, dscindex, i;
	HUINT32	ulDemuxId;
	HUINT16 usPid;

	PrintError("[%s] PID channel index: %x.\n", __FUNCTION__, ulChIndex);

	VK_SEM_Get(s_ulDscSemId);

	*pKeySlotHandle = NULL;
	drvErr = DRV_DEMUX_GetRecordPidChannelInfo(ulChIndex, &ulDemuxId, &usPid);
	if(drvErr != DRV_OK)
	{
		PrintError("[%s] Error, DRV_DEMUX_GetRecordPidChannelInfo() : ulChIndex(%d)\n\r", __FUNCTION__, ulChIndex);
		VK_SEM_Release(s_ulDscSemId);
		return DRV_ERR;
	}
	else
	{
		PrintError("[%s] ulDemuxId(%d), usPid(%x)\n", __FUNCTION__, ulDemuxId, usPid);
		/* Check whether keyslot is opened before for record. */
		for (recindex = 0; recindex < DI_DSC_SLOTNUM_MAXIMUM; recindex++)
		{
			if ((s_stDscSlotInfo[recindex].ulDemuxId == ulDemuxId) && (s_stDscSlotInfo[recindex].pid == usPid))
			{
				PrintError("[%s] Found keyslot(%x) using same pid(%x) in s_stDscSlotInfo[%d].\r\n", __FUNCTION__, \
						s_stDscSlotInfo[recindex].keyHandle, s_stDscSlotInfo[recindex].pid, recindex);
				break;
			}
		}

		/* if it did not find keyslot for record,*/
		if (recindex == DI_DSC_SLOTNUM_MAXIMUM)
		{
			PrintError("[%s] Not find keyslot in record!\r\n", __FUNCTION__);
			/* Find keyslot with same pid in descrambler. */
			for (dscindex = 0; dscindex < DI_DSC_SLOTNUM_MAXIMUM; dscindex++)
			{
				if (s_stDscSlotInfo[dscindex].pid == usPid)
				{
					keyHandle = s_stDscSlotInfo[dscindex].keyHandle;
					PrintError("[%s] Found keyslot(%x) using same pid(%x) in s_stDscSlotInfo[%d].\r\n", __FUNCTION__, \
							s_stDscSlotInfo[dscindex].keyHandle, s_stDscSlotInfo[dscindex].pid, dscindex);
					break;
				}
			}

			/* Open new keyslot to use for record.*/
			if (dscindex == DI_DSC_SLOTNUM_MAXIMUM)
			{
				PrintError("[%s] Open new keyslot for record!\r\n", __FUNCTION__);
				NEXUS_Security_GetDefaultKeySlotSettings(&keySlotSettings);
				keySlotSettings.keySlotEngine = NEXUS_SecurityEngine_eCaCp;

				keyHandle = NEXUS_Security_AllocateKeySlot(&keySlotSettings);
				if (keyHandle == NULL)
				{
					PrintError("[%s] Error!, NEXUS_Security_AllocateCaCpKeySlot error code: 0x%x\n", __FUNCTION__, rc);
					VK_SEM_Release(s_ulDscSemId);
					return DRV_ERR;
				}
			}

			/* Search free s_stDscSlotInfo[]. */
			for (i = 0; i < DI_DSC_SLOTNUM_MAXIMUM; i++)
			{
				if (s_stDscSlotInfo[i].bAllocated == FALSE)
				{
					s_stDscSlotInfo[i].ulDemuxId 	= ulDemuxId;
					s_stDscSlotInfo[i].pid			= usPid;
					s_stDscSlotInfo[i].keyHandle    = keyHandle;
					s_stDscSlotInfo[i].bAllocated 	= TRUE;
					VK_MEM_Memset((void *)s_stDscSlotInfo[i].keyData, 0x0, 2 * MAX_KEY_LENGTH);
					break;
				}
			}

			if (i == DI_DSC_SLOTNUM_MAXIMUM)
			{
				PrintError("[%d] There is not free s_stDscSlotInfo[] to store new keyslot info!\r\n", __FUNCTION__);
				VK_SEM_Release(s_ulDscSemId);
				return DRV_ERR;
			}

			/* If keyslot is opend for use in the descrambler for the first time, store index number in order to reference it for closing.*/
			if (dscindex != DI_DSC_SLOTNUM_MAXIMUM)
			{
				PrintError("[%s] Link dscindex(%d) with %d!!\n", __FUNCTION__, dscindex, i);
				s_stDscSlotInfo[i].ulLinked = dscindex;
				s_stDscSlotInfo[dscindex].ulLinked = i;
			}

			s_stDscSlotInfo[i].eUsedBy |= P_DSC_USED_BY_REC;
			*pKeySlotHandle = s_stDscSlotInfo[i].keyHandle;
		}
		else
		{
			s_stDscSlotInfo[recindex].eUsedBy |= P_DSC_USED_BY_REC;
			*pKeySlotHandle = s_stDscSlotInfo[recindex].keyHandle;
		}
	}

	PrintError("[%s] demux(%d), pid(%x) keyHandle: %x\r\n", __FUNCTION__, ulDemuxId, usPid, *pKeySlotHandle);
	VK_SEM_Release(s_ulDscSemId);
	return DRV_OK;
}


DRV_Error DRV_DSC_CloseKeySlotForRecord(HUINT32 ulChIndex, NEXUS_KeySlotHandle *pKeySlotHandle)
{
	NEXUS_Error rc = NEXUS_SUCCESS;
	DRV_Error drvErr = DRV_OK;
	NEXUS_SecurityInvalidateKeySettings tSecurityInvalidateKeySettings;
	HUINT32	ulDemuxId, i;
	HUINT16 usPid;

	PrintError("[%s] PID channel index: %x.\n", __FUNCTION__, ulChIndex);

	VK_SEM_Get(s_ulDscSemId);

	*pKeySlotHandle = NULL;
	drvErr = DRV_DEMUX_GetRecordPidChannelInfo(ulChIndex, &ulDemuxId, &usPid);
	if(drvErr != DRV_OK)
	{
		PrintError("[%s] Error, DRV_DEMUX_GetRecordPidChannelInfo() : ulChIndex(%d)\n\r", __FUNCTION__, ulChIndex);
		VK_SEM_Release(s_ulDscSemId);
		return DRV_ERR;
	}
	else
	{
		PrintError("[%s] ulDemuxId(%d), usPid(%x)\n", __FUNCTION__, ulDemuxId, usPid);
		/* Check whether keyslot is opened before for record. */
		for (i = 0; i < DI_DSC_SLOTNUM_MAXIMUM; i++)
		{
			if ((s_stDscSlotInfo[i].ulDemuxId == ulDemuxId) && (s_stDscSlotInfo[i].pid == usPid))
			{
				PrintError("[%s] Found keyslot(%x) using same demuxid(%d) and pid(%x) in s_stDscSlotInfo[%d].\r\n", __FUNCTION__, \
						s_stDscSlotInfo[i].keyHandle, s_stDscSlotInfo[i].ulDemuxId, s_stDscSlotInfo[i].pid, i);
				break;
			}
		}

		/* if it did not find keyslot for record,*/
		if (i == DI_DSC_SLOTNUM_MAXIMUM)
		{
			PrintError("[%s] Error, Not find keyslot in s_stDscSlotInfo[]: ulDemuxId(%d), usPid(%x)!\r\n", __FUNCTION__, ulDemuxId, usPid);
			VK_SEM_Release(s_ulDscSemId);
			return DRV_ERR;
		}


		if (s_stDscSlotInfo[i].keyHandle != NULL)
		{
			*pKeySlotHandle = s_stDscSlotInfo[i].keyHandle;
			/* If descrambler is also using the keyslot of same demux_id, delete a indication that record is in use. */
			if ((s_stDscSlotInfo[i].eUsedBy & P_DSC_USED_BY_DSC) != FALSE)
			{
				PrintError("[%s] delete indication of record! s_stDscSlotInfo[%d].eUsedBy: %x\n", __FUNCTION__, i, s_stDscSlotInfo[i].eUsedBy);
				s_stDscSlotInfo[i].eUsedBy &= ~P_DSC_USED_BY_REC;
			}
			else
			{
				if (s_stDscSlotInfo[i].ulLinked != P_DSC_NOT_LINKED)
				{
					PrintError("[%s] Disconnect linked keyslot. s_stDscSlotInfo[%d].ulLinked: %x\n", __FUNCTION__, i, s_stDscSlotInfo[i].ulLinked);
					s_stDscSlotInfo[s_stDscSlotInfo[i].ulLinked].ulLinked = P_DSC_NOT_LINKED;
					s_stDscSlotInfo[i].ulLinked = P_DSC_NOT_LINKED;
				}
				else
				{
					PrintError("[%s] This keyslot(%x) is used only for descrambler.\n", __FUNCTION__, i, s_stDscSlotInfo[i].keyHandle);
					NEXUS_Security_GetDefaultInvalidateKeySettings(&tSecurityInvalidateKeySettings);
					tSecurityInvalidateKeySettings.keyDestEntryType = NEXUS_SecurityKeyType_eOddAndEven;
					tSecurityInvalidateKeySettings.invalidateKeyType = NEXUS_SecurityInvalidateKeyFlag_eDestKeyOnly;
					tSecurityInvalidateKeySettings.keyDestBlckType = NEXUS_SecurityAlgorithmConfigDestination_eCa;
					tSecurityInvalidateKeySettings.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKLDummy;

					rc = NEXUS_Security_InvalidateKey((NEXUS_KeySlotHandle)s_stDscSlotInfo[i].keyHandle, &tSecurityInvalidateKeySettings);
					if( rc != NEXUS_SUCCESS )
					{
						PrintError("[DI_DSC_Close] Error NEXUS_Security_InvalidateKey 0x%x\r\n", rc);
					}

					NEXUS_Security_FreeKeySlot((NEXUS_KeySlotHandle)s_stDscSlotInfo[i].keyHandle);
				}
				s_stDscSlotInfo[i].bAllocated = FALSE;
				s_stDscSlotInfo[i].ulDemuxId = 0;
				s_stDscSlotInfo[i].keyHandle = NULL;
				s_stDscSlotInfo[i].pid = 0;
				s_stDscSlotInfo[i].ulPidChannel = INVALID_HANDLE;
				s_stDscSlotInfo[i].s_ulUseCWPK = FALSE;
				s_stDscSlotInfo[i].ulLinked = P_DSC_NOT_LINKED;
				s_stDscSlotInfo[i].eUsedBy = 0;
				s_stDscSlotInfo[i].lUsedCount = 0;
			}
		}
		else
		{
			PrintError("[%s] Error, s_stDscSlotInfo[%d].keyHandle is NULL!\n", __FUNCTION__, i, s_stDscSlotInfo[i].keyHandle);
			VK_SEM_Release(s_ulDscSemId);
			return DRV_ERR;
		}
	}

	PrintError("[%s] Found keyHandle is %x!\n", __FUNCTION__, *pKeySlotHandle);
	VK_SEM_Release(s_ulDscSemId);
	return DRV_OK;
}


DRV_Error DRV_DSC_DumpKeySlot(HUINT32 ulTestCase)
{
	HUINT32	i = 0;

	if (ulTestCase == 0)
	{
		for (i=0; i<DI_DSC_SLOTNUM_MAXIMUM; i++)
		{
			if (s_stDscSlotInfo[i].bAllocated == TRUE)
			{
				PrintError("s_stDscSlotInfo[%d]: ulDemuxId: %d, PID: 0x%x, keyhandle: 0x%x, usedby: %s%s, linked: %x\n", \
						i, s_stDscSlotInfo[i].ulDemuxId, s_stDscSlotInfo[i].pid, (HUINT32)s_stDscSlotInfo[i].keyHandle, \
						((s_stDscSlotInfo[i].eUsedBy & P_DSC_USED_BY_DSC)? "DSC":"_"), ((s_stDscSlotInfo[i].eUsedBy & P_DSC_USED_BY_REC)? "REC":"_"), \
						s_stDscSlotInfo[i].ulLinked);
			}
		}
	}
	else if (ulTestCase == 1)
	{
		for (i=0; i<DI_DSC_SLOTNUM_MAXIMUM; i++)
		{
			PrintError("s_stDscSlotInfo[%d]: ulDemuxId: %d, PID: 0x%x, keyhandle: 0x%x, usedby: %s%s, linked: %x\n", \
					i, s_stDscSlotInfo[i].ulDemuxId, s_stDscSlotInfo[i].pid, (HUINT32)s_stDscSlotInfo[i].keyHandle, \
					((s_stDscSlotInfo[i].eUsedBy & P_DSC_USED_BY_DSC)? "DSC":"_"), ((s_stDscSlotInfo[i].eUsedBy & P_DSC_USED_BY_REC)? "REC":"_"), \
					s_stDscSlotInfo[i].ulLinked);
		}
	}
	else
	{

	}

	return DRV_OK;
}


DRV_Error DRV_DSC_GetKeySlotByPidNum(HUINT16 pid)
{
	NEXUS_KeySlotHandle KeyslotHandle;
	int 			i = 0;
	unsigned int	ulGetPidChannel = INVALID_HANDLE;
	unsigned int	ulNumChannel = 0;

	PrintEnter("DRV_DSC_GetKeySlotByPidNum");

	PrintDebug("[%s]  pid(0x%x)\n", __FUNCTION__, pid);

	VK_SEM_Get(s_ulDscSemId);

	for (i = 0; i < DI_DSC_SLOTNUM_MAXIMUM; i++)
	{
		if (s_stDscSlotInfo[i].pid == pid)
		{
			PrintError("[DRV_DSC_GetKeySlotByPidNum] It be found slot(%d)\r\n", i);
			PrintError("[DRV_DSC_GetKeySlotByPidNum] s_stDscSlotInfo[%d].keyHandle = %x\r\n", i, s_stDscSlotInfo[i].keyHandle);
			PrintError("[DRV_DSC_GetKeySlotByPidNum] s_stDscSlotInfo[%d].bAllocated = %d \r\n", i, s_stDscSlotInfo[i].bAllocated);
			PrintError("[DRV_DSC_GetKeySlotByPidNum] s_stDscSlotInfo[%d].lUsedCount = %d \r\n", i, s_stDscSlotInfo[i].lUsedCount);
			break;
		}
	}

	if (i == DI_DSC_SLOTNUM_MAXIMUM)
	{
		PrintError("[%s] There isn't matched slot!!\r\n", __FUNCTION__);
		VK_SEM_Release(s_ulDscSemId);
		return DRV_ERR;
	}

	if (s_stDscSlotInfo[i].lUsedCount == 0)
	{
		PrintError("[%s] (s_stDscSlotInfo[i].lUsedCount==0)\r\n", __FUNCTION__);
		VK_SEM_Release(s_ulDscSemId);
		return DRV_ERR;
	}

	if (s_stDscSlotInfo[i].bAllocated == TRUE)
	{
		ulNumChannel = DRV_DEMUX_GetPIDChannelForDSC(s_stDscSlotInfo[i].ulDemuxId, pid, &ulGetPidChannel);

		PrintError("[%s] pid = %x ulGetPidChannel = %x ulNumChannel = %d \r\n", __FUNCTION__, pid, ulGetPidChannel,ulNumChannel);

		if ((ulGetPidChannel == INVALID_HANDLE) || (ulNumChannel < 1))
		{
			PrintError("[%s] Get invalid pidchannel = %x\r\n", __FUNCTION__, ulGetPidChannel);
			VK_SEM_Release(s_ulDscSemId);
			return DRV_ERR;
		}

		KeyslotHandle = NEXUS_Security_LocateCaKeySlotAssigned(ulGetPidChannel);
		if (KeyslotHandle == NULL)
		{
			PrintError("[%s] pid(%d) is not allocate to keyslot\r\n", __FUNCTION__, pid);
			VK_SEM_Release(s_ulDscSemId);
			return DRV_ERR;
		}

		PrintError("[%s] pid(%d) is allocate to keyslot: %x\r\n", __FUNCTION__, pid, KeyslotHandle);
	}

	PrintExit("DRV_DSC_GetKeySlotByPidNum");

	VK_SEM_Release(s_ulDscSemId);
	return DRV_OK;
}


static DRV_Error P_DSC_OpenKeySlot(HUINT32 ulDscId, HUINT16 usPid, NEXUS_KeySlotHandle *pKeySlotHandle)
{
	NEXUS_KeySlotHandle	keyHandle = NULL;
	NEXUS_Error	rc = NEXUS_SUCCESS;
	DRV_Error	drvErr = DRV_OK;
	NEXUS_SecurityKeySlotSettings 	keySlotSettings;
	HUINT32	i;

	PrintError("[%s] ulDscId(%d), usPid(%x)\n", __FUNCTION__, ulDscId, usPid);

	/* Check whether keyHandle has been used before */
	if ((s_stDscSlotInfo[ulDscId].keyHandle != NULL) && (s_stDscSlotInfo[ulDscId].pid == usPid))
	{
		PrintError("Found keyHandle (%x)\n", s_stDscSlotInfo[ulDscId].keyHandle);
	}
	else
	{
		/* Check whether keyslot was opened before in record. */
		for (i = 0; i < DI_DSC_SLOTNUM_MAXIMUM; i++)
		{
			if ((s_stDscSlotInfo[i].pid == usPid) && (s_stDscSlotInfo[i].ulDemuxId == s_stDscSlotInfo[ulDscId].ulDemuxId))
			{
				PrintError("[%s] Found keyslot(%x) using same pid(%x) in %d.\r\n", __FUNCTION__, s_stDscSlotInfo[i].keyHandle, s_stDscSlotInfo[i].pid, i);
				break;
			}
		}

		/* if we can not find keyslot in descrambler, open new keyslot. */
		if (i == DI_DSC_SLOTNUM_MAXIMUM)
		{
			PrintError("[%s] There is not matched keyslot in descrambler!\r\n", __FUNCTION__);

			/* Open new keyHandle */
			NEXUS_Security_GetDefaultKeySlotSettings(&keySlotSettings);
			keySlotSettings.keySlotEngine = NEXUS_SecurityEngine_eCaCp;
			if(s_stDscSlotInfo[ulDscId].eDscType == DI_DSC_DESC_AES_CLEARMODE)
			{
				keySlotSettings.keySlotType = NEXUS_SecurityKeySlotType_eType2;
			}

			keyHandle = NEXUS_Security_AllocateKeySlot(&keySlotSettings);
			if (keyHandle == NULL)
			{
				PrintError("[%s] Error!, NEXUS_Security_AllocateCaCpKeySlot error code: 0x%x\n", __FUNCTION__, rc);
				return DRV_ERR;
			}
		}
		else
		{
			keyHandle = s_stDscSlotInfo[i].keyHandle;
			/* If keyslot is opend for use in the recording for the first time, store index number in order to reference it for closing.*/
			if (s_stDscSlotInfo[ulDscId].bAllocated == TRUE)
			{
				PrintError("[%s] Link keyslot(%d) with %d!!\n", __FUNCTION__, ulDscId, i);
				s_stDscSlotInfo[ulDscId].ulLinked = i;
				s_stDscSlotInfo[i].ulLinked = ulDscId;
			}
		}

		/* Save the newly opened keyHand to s_stDscSlotInfo[].*/
		if (s_stDscSlotInfo[ulDscId].bAllocated == TRUE)
		{
			s_stDscSlotInfo[ulDscId].keyHandle = keyHandle;
			PrintError("[%s] Opened new keyHandle(%x).\n", __FUNCTION__, s_stDscSlotInfo[ulDscId].keyHandle);
		}
		else
		{
			PrintError("[%s] Error!, DSC(%d) is not allocated or new pid(%x) is not match with previous pid(%x).\n", __FUNCTION__, ulDscId, usPid, s_stDscSlotInfo[ulDscId].pid);
			return DRV_ERR;
		}

		/* Setup algorithm to use for descrambler. */
		drvErr = P_DSC_SetConfigAlgorithm(s_stDscSlotInfo[ulDscId].keyHandle, s_stDscSlotInfo[ulDscId].eDscType);
		if (drvErr != DRV_OK)
		{
			PrintError("[%s] Error!, P_DSC_SetConfigAlgorithm.\n", __FUNCTION__);
			return DRV_ERR;
		}
	}

	s_stDscSlotInfo[ulDscId].eUsedBy |= P_DSC_USED_BY_DSC;
	*pKeySlotHandle = s_stDscSlotInfo[ulDscId].keyHandle;
	PrintError("[%s] demuxId(%d) s_stDscSlotInfo[%d].keyHandle: %x\r\n", __FUNCTION__, s_stDscSlotInfo[ulDscId].ulDemuxId, ulDscId, s_stDscSlotInfo[ulDscId].keyHandle);
	return DRV_OK;
}


static DRV_Error P_DSC_SetConfigAlgorithm(NEXUS_KeySlotHandle keyHandle, DI_DSC_DESC_TYPE eDscType)
{
	NEXUS_SecurityAlgorithmSettings AlgConfig;
	NEXUS_Error 					rc = NEXUS_SUCCESS;

	PrintError("[%s] keyHandle(%x), eDscType(%d)\n", __FUNCTION__, keyHandle, eDscType);

	NEXUS_Security_GetDefaultAlgorithmSettings(&AlgConfig);
	AlgConfig.askmModuleID	= NEXUS_SecurityAskmModuleID_eModuleID_3;
	AlgConfig.dest = NEXUS_SecurityAlgorithmConfigDestination_eCa;

	switch (eDscType)
	{
		case DI_DSC_DESC_DVB:
			AlgConfig.algorithm 		= NEXUS_SecurityAlgorithm_eDvb;
			AlgConfig.dvbScramLevel 	= NEXUS_SecurityDvbScrambleLevel_eTs;
			AlgConfig.scValue[NEXUS_SecurityPacketType_eGlobal] 	= NEXUS_SecurityAlgorithmScPolarity_eClear;
			AlgConfig.scValue[NEXUS_SecurityPacketType_eRestricted] = NEXUS_SecurityAlgorithmScPolarity_eClear;
			AlgConfig.modifyScValue[NEXUS_SecurityPacketType_eGlobal] = true;
			AlgConfig.modifyScValue[NEXUS_SecurityPacketType_eRestricted] = true;
			break;

		case DI_DSC_DESC_AES:
			AlgConfig.algorithm 		= NEXUS_SecurityAlgorithm_eAes;
			AlgConfig.algorithmVar 		= NEXUS_SecurityAlgorithmVariant_eCbc;
#if defined(CONFIG_CAS_ARIB)
			AlgConfig.solitarySelect	= NEXUS_SecuritySolitarySelect_eCbcXorIv;
			AlgConfig.terminationMode	=  NEXUS_SecurityTerminationMode_eCbcResidual;
#else
			AlgConfig.terminationMode 	= NEXUS_SecurityTerminationMode_eBlock;
#endif
			AlgConfig.scValue[NEXUS_SecurityPacketType_eGlobal] 	= NEXUS_SecurityAlgorithmScPolarity_eClear;
			AlgConfig.scValue[NEXUS_SecurityPacketType_eRestricted] = NEXUS_SecurityAlgorithmScPolarity_eClear;
			AlgConfig.modifyScValue[NEXUS_SecurityPacketType_eGlobal] = true;
			AlgConfig.modifyScValue[NEXUS_SecurityPacketType_eRestricted] = true;
			break;

		case DI_DSC_DESC_DES:
			AlgConfig.algorithm 		= NEXUS_SecurityAlgorithm_eDes;
			AlgConfig.dvbScramLevel 	= NEXUS_SecurityDvbScrambleLevel_eTs;
			AlgConfig.scValue[NEXUS_SecurityPacketType_eGlobal] 	= NEXUS_SecurityAlgorithmScPolarity_eClear;
			AlgConfig.scValue[NEXUS_SecurityPacketType_eRestricted] = NEXUS_SecurityAlgorithmScPolarity_eClear;
			AlgConfig.modifyScValue[NEXUS_SecurityPacketType_eGlobal] = true;
			AlgConfig.modifyScValue[NEXUS_SecurityPacketType_eRestricted] = true;
			break;

		case DI_DSC_DESC_AES_CLEARMODE:
			AlgConfig.algorithm 		= NEXUS_SecurityAlgorithm_eAes;
			AlgConfig.algorithmVar 		= NEXUS_SecurityAlgorithmVariant_eCbc;
			AlgConfig.scValue[NEXUS_SecurityPacketType_eGlobal] 	= NEXUS_SecurityAlgorithmScPolarity_eClear;
			AlgConfig.scValue[NEXUS_SecurityPacketType_eRestricted] = NEXUS_SecurityAlgorithmScPolarity_eClear;
			AlgConfig.modifyScValue[NEXUS_SecurityPacketType_eGlobal] = true;
			AlgConfig.modifyScValue[NEXUS_SecurityPacketType_eRestricted] = true;
			break;

		case DI_DSC_DESC_DES_CLEARMODE:
			AlgConfig.algorithm 		= NEXUS_SecurityAlgorithm_eDes;
			AlgConfig.dvbScramLevel 	= NEXUS_SecurityDvbScrambleLevel_eTs;
			AlgConfig.scValue[NEXUS_SecurityPacketType_eGlobal] 	= NEXUS_SecurityAlgorithmScPolarity_eClear;
			AlgConfig.scValue[NEXUS_SecurityPacketType_eRestricted] = NEXUS_SecurityAlgorithmScPolarity_eClear;
			AlgConfig.modifyScValue[NEXUS_SecurityPacketType_eGlobal] = true;
			AlgConfig.modifyScValue[NEXUS_SecurityPacketType_eRestricted] = true;
			break;

		case DI_DSC_DESC_3DESABA:
			AlgConfig.algorithm 		= NEXUS_SecurityAlgorithm_e3DesAba;
			AlgConfig.dvbScramLevel 	= NEXUS_SecurityDvbScrambleLevel_eTs;
			AlgConfig.terminationMode 	= NEXUS_SecurityTerminationMode_eCipherStealing;
			AlgConfig.scValue[NEXUS_SecurityPacketType_eGlobal] 	= NEXUS_SecurityAlgorithmScPolarity_eClear;
			AlgConfig.scValue[NEXUS_SecurityPacketType_eRestricted] = NEXUS_SecurityAlgorithmScPolarity_eClear;
			AlgConfig.modifyScValue[NEXUS_SecurityPacketType_eGlobal] = true;
			AlgConfig.modifyScValue[NEXUS_SecurityPacketType_eRestricted] = true;
			break;

		case DI_DSC_DESC_MULTI2:
			break;

		default:
			PrintError("[%s] Not support descramble type(%d).\r\n", __FUNCTION__, eDscType);
			NEXUS_Security_FreeKeySlot(keyHandle);
			return DRV_ERR;
	}

	rc = NEXUS_Security_ConfigAlgorithm(keyHandle, &AlgConfig);
	if (rc != NEXUS_SUCCESS)
	{
		PrintError("[%s] Error!, NEXUS_Security_ConfigAlgorithm error code: 0x%x\n", __FUNCTION__, rc);
		return DRV_ERR;
	}

	return DRV_OK;
}

static DRV_Error P_DSC_CloseKeySlot(HUINT32 ulDscId)
{
	NEXUS_Error	rc = NEXUS_SUCCESS;
	NEXUS_SecurityInvalidateKeySettings tSecurityInvalidateKeySettings;

	PrintError("[%s] ulDscId(%d), ulDemuxId(%d), usPid(%x)\n", __FUNCTION__, ulDscId, s_stDscSlotInfo[ulDscId].ulDemuxId,  s_stDscSlotInfo[ulDscId].pid);

	/* Check whether current keyslot is connected with outer keyslot. */
	if (s_stDscSlotInfo[ulDscId].keyHandle != NULL)
	{

		/* If descrambler is also using the keyslot of same demux_id, delete a indication that record is in use. */
		if ((s_stDscSlotInfo[ulDscId].eUsedBy & P_DSC_USED_BY_REC) != FALSE)
		{
			PrintError("[%s] delete indication of descrambler! s_stDscSlotInfo[%d].eUsedBy: %x\n", __FUNCTION__, ulDscId, s_stDscSlotInfo[ulDscId].eUsedBy);
			s_stDscSlotInfo[ulDscId].eUsedBy &= ~P_DSC_USED_BY_DSC;
		}
		else
		{
			if (s_stDscSlotInfo[ulDscId].ulLinked != P_DSC_NOT_LINKED)
			{
				PrintError("[%s] Disconnect linked keyslot. s_stDscSlotInfo[%d].ulLinked: %x\n", __FUNCTION__, ulDscId, s_stDscSlotInfo[ulDscId].ulLinked);
				s_stDscSlotInfo[s_stDscSlotInfo[ulDscId].ulLinked].ulLinked = P_DSC_NOT_LINKED;
				s_stDscSlotInfo[ulDscId].ulLinked = P_DSC_NOT_LINKED;
			}
			else
			{
				PrintError("[%s] This keyslot(%x) is used only for descrambler.\n", __FUNCTION__, ulDscId, s_stDscSlotInfo[ulDscId].keyHandle);
				NEXUS_Security_GetDefaultInvalidateKeySettings(&tSecurityInvalidateKeySettings);
				tSecurityInvalidateKeySettings.keyDestEntryType = NEXUS_SecurityKeyType_eOddAndEven;
				tSecurityInvalidateKeySettings.invalidateKeyType = NEXUS_SecurityInvalidateKeyFlag_eDestKeyOnly;
				tSecurityInvalidateKeySettings.keyDestBlckType = NEXUS_SecurityAlgorithmConfigDestination_eCa;
				tSecurityInvalidateKeySettings.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKLDummy;

				rc = NEXUS_Security_InvalidateKey((NEXUS_KeySlotHandle)s_stDscSlotInfo[ulDscId].keyHandle, &tSecurityInvalidateKeySettings);
				if( rc != NEXUS_SUCCESS )
				{
					PrintError("[DI_DSC_Close] Error NEXUS_Security_InvalidateKey 0x%x\r\n", rc);
				}

				NEXUS_Security_FreeKeySlot((NEXUS_KeySlotHandle)s_stDscSlotInfo[ulDscId].keyHandle);
			}
			s_stDscSlotInfo[ulDscId].bAllocated = FALSE;
			s_stDscSlotInfo[ulDscId].ulDemuxId = 0;
			s_stDscSlotInfo[ulDscId].keyHandle = NULL;
			s_stDscSlotInfo[ulDscId].pid = 0;
			s_stDscSlotInfo[ulDscId].ulPidChannel = INVALID_HANDLE;
			s_stDscSlotInfo[ulDscId].s_ulUseCWPK = FALSE;
			s_stDscSlotInfo[ulDscId].ulLinked = P_DSC_NOT_LINKED;
			s_stDscSlotInfo[ulDscId].eUsedBy = 0;
			s_stDscSlotInfo[ulDscId].lUsedCount = 0;
		}
	}
	else
	{
		PrintError("[%s] Error, s_stDscSlotInfo[%d].keyHandle is NULL!\n", __FUNCTION__, ulDscId, s_stDscSlotInfo[ulDscId].keyHandle);
		return DRV_ERR;
	}
	return DRV_OK;
}

#else

DRV_Error DRV_DSC_OpenKeySlotForRecord(HUINT32 ulChIndex, NEXUS_KeySlotHandle *pKeySlotHandle)
{
	UNUSED_PARAM(ulChIndex);
	UNUSED_PARAM(pKeySlotHandle);
	PrintError("[%s] This function does not support under 28nm chips.\n", __FUNCTION__);
	return DRV_OK;
}


DRV_Error DRV_DSC_CloseKeySlotForRecord(HUINT32 ulChIndex, NEXUS_KeySlotHandle *pKeySlotHandle)
{
	UNUSED_PARAM(ulChIndex);
	UNUSED_PARAM(pKeySlotHandle);
	PrintError("[%s] This function does not support under 28nm chips.\n", __FUNCTION__);
	return DRV_OK;
}


DRV_Error DRV_DSC_DumpKeySlot(HUINT32 ulTestCase)
{
	UNUSED_PARAM(ulTestCase);
	PrintError("[%s] This function does not support under 28nm chips.\n", __FUNCTION__);
	return DRV_OK;
}


DRV_Error DRV_DSC_GetKeySlotByPidNum(HUINT16 usPid)
{
	UNUSED_PARAM(usPid);
	PrintError("[%s] This function does not support under 28nm chips.\n", __FUNCTION__);
	return DRV_OK;
}
#endif


