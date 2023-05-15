/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pal_channel_emu.c
	@brief	  Octo pal channel emulator part

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <vkernel.h>
#include <octo_common.h>

#include <pal_channel.h>

#include <di_channel.h>
#include <di_channel_s.h>
#include <di_channel_c.h>
#include <di_channel_t.h>
#include <di_pvr.h>
#include "_pal_channel.h"
#include "_pal_channel_emu.h"

/*******************************************************************/
/************************     Definition   * ***********************/
/*******************************************************************/
#define PAL_CH_EMU_USABLE_PBID_FIRST		0
#define PAL_CH_EMU_NUM_TUNER				2
#define PAL_CH_EMU_NUM_RFINPUT				2

#define PAL_CH_EMU_MOUNT_DIR				"/Share_Ts"
#define PAL_CH_EMU_MOUNT_RESULT				"/tmp/.ch_emu_mount"

#define PAL_CH_EMU_SETUP_FILE				"/channel_emul.json"
#define PAL_CH_EMU_INDEX_FILE				PAL_CH_EMU_MOUNT_DIR"/index.json"

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eTUNER_EMU_STATE_Idle,

	eTUNER_EMU_STATE_Tuned,

	eTUNER_EMU_STATE_EndOfCase
} INT_PAL_CH_EMU_TunerState_e;

typedef enum
{
	eCH_EMU_CMD_None,

	eCH_EMU_CMD_RewindPlayback,
	eCH_EMU_CMD_NotifyTuneResult,

	eCH_EMU_CMD_EndOfCase
} INT_PAL_CH_EMU_Cmd_e;

typedef struct
{
	HCHAR			 szSmbPath[256];
	HCHAR			 szUserName[32];
	HCHAR			 szPassword[64];
} INT_PAL_CH_EMU_SmbConf_t;

typedef struct
{
	DI_CH_Type_e			 eChType;
	union
	{
		CH_SAT_TuneParam_t	 stSat;
		CH_CAB_TuneParam_t	 stCab;
		CH_TER_TuneParam_t	 stTer;
	} unDiParam;

	HCHAR				 szFileName[256];

	// Playing Status
	HINT32				 nCurrTime;
	HINT32				 nTotalTime;
} INT_PAL_CH_EMU_TpInfo_t;

typedef struct
{
	DxDeliveryType_b				 eSupportDeliType;
	INT_PAL_CH_EMU_TunerState_e		 eState;

	HUINT32							 ulPbDevId;
	HUINT32							 ulRequestId;

	HUINT32							 ulTpInfoIndex;
	INT_PAL_CH_EMU_TpInfo_t			 stTunedTp;

	PalCh_SatTunerCapacity_t		 stSatTunerInfo;
	HINT32							 nRfInputId;
} INT_PAL_CH_EMU_Tuner_t;

typedef struct
{
	INT_PAL_CH_EMU_Cmd_e	 eCmd;

	HUINT32					 ulParam1;
	HUINT32					 ulParam2;
	HUINT32					 ulParam3;
	HUINT32					 ulParam4;
} INT_PAL_CH_EMU_MSG_t;

typedef struct
{
	pfnDI_CH_NotifySignalInfo	 pfnNotifier;
	INT_PAL_CH_EMU_SmbConf_t	 stSmbConf;

	HBOOL						 bSmbMounted;

	HUINT32						 ulTpNum;
	INT_PAL_CH_EMU_TpInfo_t		*pstTpArray;

	HUINT32						 ulTunerNum;
	INT_PAL_CH_EMU_Tuner_t		*pstTunerArray;

	HUINT32						 ulRfInputNum;
	PalCh_SatRfInput_t			*pstRfInputArray;

	unsigned long				 ulMsgQID;
	unsigned long				 ulTaskId;
} INT_PAL_CH_EMU_Context_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC INT_PAL_CH_EMU_Context_t		 s_stPalChEmu_Context;

/*******************************************************************/
/******************* Static Function Prototypes  *************************/
/*******************************************************************/

/*****************************************************************
*************************** Functions ****************************
*****************************************************************/
#define ___PRIVATE_FUNCTIONS___
STATIC INT_PAL_CH_EMU_Context_t *pal_ch_emu_GetContext (void)
{
	return &s_stPalChEmu_Context;
}

STATIC INT_PAL_CH_EMU_Tuner_t *pal_ch_emu_GetTuner (HUINT32 ulTunerId)
{
	INT_PAL_CH_EMU_Context_t	*pstContext = pal_ch_emu_GetContext();

	if ((ulTunerId < pstContext->ulTunerNum) && (NULL != pstContext->pstTunerArray))
	{
		return &(pstContext->pstTunerArray[ulTunerId]);
	}

	return NULL;
}

STATIC INT_PAL_CH_EMU_Tuner_t *pal_ch_emu_GetTunerByPbDevId (HUINT32 ulPbDevId)
{
	HUINT32			 			 ulCnt;
	INT_PAL_CH_EMU_Context_t	*pstContext = pal_ch_emu_GetContext();

	if (NULL != pstContext->pstTunerArray)
	{
		for (ulCnt = 0; ulCnt < pstContext->ulTunerNum; ulCnt++)
		{
			INT_PAL_CH_EMU_Tuner_t		*pstTuner = &(pstContext->pstTunerArray[ulCnt]);

			if (ulPbDevId == pstTuner->ulPbDevId)
			{
				return pstTuner;
			}
		}
	}

	return NULL;
}

STATIC PalCh_SatRfInput_t *pal_ch_emu_GetRfInput (HUINT32 ulRfInputId)
{
	INT_PAL_CH_EMU_Context_t	*pstContext = pal_ch_emu_GetContext();

	if ((ulRfInputId < pstContext->ulRfInputNum) && (NULL != pstContext->pstRfInputArray))
	{
		return &(pstContext->pstRfInputArray[ulRfInputId]);
	}

	return NULL;
}

STATIC INT_PAL_CH_EMU_TpInfo_t *pal_ch_emu_GetTpInfo (HUINT32 ulTpIdx)
{
	INT_PAL_CH_EMU_Context_t	*pstContext = pal_ch_emu_GetContext();

	if ((ulTpIdx < pstContext->ulTpNum) && (NULL != pstContext->pstTpArray))
	{
		return &(pstContext->pstTpArray[ulTpIdx]);
	}

	return NULL;
}

STATIC HERROR pal_ch_emu_SendCommand (INT_PAL_CH_EMU_Cmd_e eCmd, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3, HUINT32 ulParam4)
{
	INT_PAL_CH_EMU_Context_t	*pstContext = pal_ch_emu_GetContext();
	INT_PAL_CH_EMU_MSG_t		 stMsg;
	HERROR						 hErr;

	stMsg.eCmd		= eCmd;
	stMsg.ulParam1	= ulParam1;
	stMsg.ulParam2	= ulParam2;
	stMsg.ulParam3	= ulParam3;
	stMsg.ulParam4	= ulParam4;

	hErr = VK_MSG_SendTimeout(pstContext->ulMsgQID, (const void *)&stMsg, sizeof(INT_PAL_CH_EMU_MSG_t), 0);
	if (VK_OK != hErr)
	{
		HxLOG_Critical("VK_MSG_SendTimeout err: pstContext->ulMsgQID(0x%08x), eCmd(%d)\n", pstContext->ulMsgQID, eCmd);
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC void pal_ch_emu_CbForDiPb (void *pMsg)
{
	DI_PVR_MSG_t  			*pstDiMsg = NULL;
	INT_PAL_CH_EMU_Tuner_t	*pstTuner;
	INT_PAL_CH_EMU_TpInfo_t	*pstTpInfo;

	pstDiMsg = (DI_PVR_MSG_t *)pMsg;
	if (NULL == pstDiMsg)
	{
		HxLOG_Error("pstDiMsg NULL\n");
		return;
	}

	pstTuner = pal_ch_emu_GetTunerByPbDevId(pstDiMsg->ulInstance);
	if (NULL == pstTuner)
	{
		HxLOG_Error("No EMU-Tuner for PbDevId(%d)\n", pstDiMsg->ulInstance);
		return;
	}

	pstTpInfo = pal_ch_emu_GetTpInfo(pstTuner->ulTpInfoIndex);
	if (NULL == pstTpInfo)
	{
		HxLOG_Error("No EMU-TP INFO for index (%d)\n", pstTuner->ulTpInfoIndex);
		return;
	}

	switch (pstDiMsg->ulEvent)
	{
	case DI_PVR_EVENT_EOF:
		(void)pal_ch_emu_SendCommand(eCH_EMU_CMD_RewindPlayback, pstDiMsg->ulInstance, 0, 0, 0);
		pstTpInfo->nTotalTime = pstTpInfo->nCurrTime;
		break;

	case DI_PVR_EVENT_TIMECODE_CHANGED:
		pstTpInfo->nCurrTime = (HINT32)pstDiMsg->ulParam1;
		if (pstTpInfo->nCurrTime > pstTpInfo->nTotalTime)
		{
			pstTpInfo->nTotalTime = pstTpInfo->nCurrTime;
		}

		break;

	default:
		break;
	}

	return;
}

STATIC HERROR pal_ch_emu_ParseSmbConf (HxJSON_t jsSetup, INT_PAL_CH_EMU_SmbConf_t *pstSmbConf)
{
	HCHAR			*szSmbPath = NULL, *szUserName = NULL, *szPassword = NULL;
	HxJSON_t		 jsSmbPath, jsUserName, jsPassword;

	// SMB Path:
	jsSmbPath = HxJSON_Object_Get(jsSetup, "smb_path");
	if (NULL == jsSmbPath)
	{
		HxLOG_Error("No smb_path data in JSON:\n");
		return ERR_FAIL;
	}

	szSmbPath = (HCHAR *)HxJSON_String_Get(jsSmbPath);
	if ((NULL == szSmbPath) || ('\0' == *szSmbPath))
	{
		HxLOG_Error("No smb_path data in JSON:\n");
		return ERR_FAIL;
	}

	// User Name:
	jsUserName = HxJSON_Object_Get(jsSetup, "smb_user");
	if (NULL == jsUserName)
	{
		HxLOG_Error("No smb_user data in JSON:\n");
		return ERR_FAIL;
	}

	szUserName = (HCHAR *)HxJSON_String_Get(jsUserName);
	if ((NULL == szUserName) || ('\0' == *szUserName))
	{
		HxLOG_Error("No smb_user data in JSON:\n");
		return ERR_FAIL;
	}

	// Password:
	jsPassword = HxJSON_Object_Get(jsSetup, "smb_pass");
	if (NULL != jsPassword)
	{
		szPassword = (HCHAR *)HxJSON_String_Get(jsPassword);
	}

	HxSTD_StrNCpy(pstSmbConf->szSmbPath, szSmbPath, 256);
	HxSTD_StrNCpy(pstSmbConf->szUserName, szUserName, 32);
	if (NULL != szPassword)
	{
		HxSTD_StrNCpy(pstSmbConf->szPassword, szPassword, 64);
	}

	return ERR_OK;
}

STATIC HERROR pal_ch_emu_WriteJsonSmbConf (INT_PAL_CH_EMU_SmbConf_t *pstSmbConf)
{
	HUINT32			 ulSetupLen;
	HCHAR			*szSetupText = NULL;
	HxJSON_t		 jsSetup = NULL;
	HERROR			 hErr, hResult = ERR_FAIL;

	jsSetup = HxJSON_Object_New();
	if (NULL == jsSetup)
	{
		HxLOG_Error("HxJSON_Object_New err:\n");
		goto END_FUNC;
	}

	HxJSON_Object_Set(jsSetup, "smb_path", HxJSON_String_New(pstSmbConf->szSmbPath));
	HxJSON_Object_Set(jsSetup, "smb_user", HxJSON_String_New(pstSmbConf->szUserName));
	if ('\0' != pstSmbConf->szPassword)
	{
		HxJSON_Object_Set(jsSetup, "smb_pass", HxJSON_String_New(pstSmbConf->szPassword));
	}

	// Encode the string:
	szSetupText = HLIB_JSON_Encode(jsSetup, NULL, 0);
	if (NULL == szSetupText)
	{
		HxLOG_Error("HLIB_JSON_Encode err:\n");
		goto END_FUNC;
	}

	ulSetupLen = HxSTD_StrLen(szSetupText);
	if (0 == ulSetupLen)
	{
		HxLOG_Error("szSetupText something wrong...\n");
		goto END_FUNC;
	}

	hErr = HLIB_FILE_WriteFile((const HUINT8 *)PAL_CH_EMU_SETUP_FILE, (void *)szSetupText, ulSetupLen);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("HLIB_FILE_WriteFile err: szSetupText [%s]\n", szSetupText);
		goto END_FUNC;
	}

	hResult = ERR_OK;

END_FUNC:
	if (NULL != jsSetup)
	{
		HLIB_JSON_Delete(jsSetup);
	}

	if (NULL != szSetupText)
	{
		HLIB_JSON_FreeMemory(szSetupText);
	}

	return hResult;
}

STATIC HERROR pal_ch_emu_InitTuner (HUINT32 ulTunerNum)
{
	HUINT32						 ulIdx;
	INT_PAL_CH_EMU_Context_t	*pstContext = pal_ch_emu_GetContext();
	INT_PAL_CH_EMU_Tuner_t		*pstTunerArray = NULL;
	HERROR						 hResult = ERR_FAIL;

	pstTunerArray = (INT_PAL_CH_EMU_Tuner_t *)OxPAL_Malloc(sizeof(INT_PAL_CH_EMU_Tuner_t) * ulTunerNum);
	if (NULL == pstTunerArray)
	{
		HxLOG_Error("memory allocation failed:\n");
		goto END_FUNC;
	}

	HxSTD_MemSet(pstTunerArray, 0, sizeof(INT_PAL_CH_EMU_Tuner_t) * ulTunerNum);

	for (ulIdx = 0; ulIdx < ulTunerNum; ulIdx++)
	{
		INT_PAL_CH_EMU_Tuner_t		*pstTuner = &(pstTunerArray[ulIdx]);

		pstTuner->eState = eTUNER_EMU_STATE_Idle;
		pstTuner->ulRequestId = (HUINT32)-1;
		pstTuner->ulPbDevId = ulIdx + PAL_CH_EMU_USABLE_PBID_FIRST;
		pstTuner->nRfInputId = -1;
		pstTuner->stSatTunerInfo.ulTunerGroupId = ulIdx;
		pstTuner->ulTpInfoIndex = (HUINT32)-1;

		(void)DI_PVR_PLAY_RegisterEventCallback(pstTuner->ulPbDevId, DI_PVR_EVENT_EOF, pal_ch_emu_CbForDiPb);
		(void)DI_PVR_PLAY_RegisterEventCallback(pstTuner->ulPbDevId, DI_PVR_EVENT_BOF, pal_ch_emu_CbForDiPb);
		(void)DI_PVR_PLAY_RegisterEventCallback(pstTuner->ulPbDevId, DI_PVR_EVENT_TIMECODE_CHANGED, pal_ch_emu_CbForDiPb);
	}

	pstContext->ulTunerNum = ulTunerNum;
	pstContext->pstTunerArray = pstTunerArray;
	pstTunerArray = NULL;

	hResult = ERR_OK;

END_FUNC:
	if (NULL != pstTunerArray)					{ OxPAL_Free(pstTunerArray); }

	return hResult;
}

STATIC HERROR pal_ch_emu_InitRfInput (HUINT32 ulRfInputNum)
{
	INT_PAL_CH_EMU_Context_t	*pstContext = pal_ch_emu_GetContext();
	PalCh_SatRfInput_t			*pstRfInputArray = NULL;
	HERROR						 hResult = ERR_FAIL;

	pstRfInputArray = (PalCh_SatRfInput_t *)OxPAL_Malloc(sizeof(PalCh_SatRfInput_t) * ulRfInputNum);
	if (NULL == pstRfInputArray)
	{
		HxLOG_Error("memory allocation failed:\n");
		goto END_FUNC;
	}

	HxSTD_MemSet(pstRfInputArray, 0, sizeof(PalCh_SatRfInput_t) * ulRfInputNum);

	pstContext->ulRfInputNum = ulRfInputNum;
	pstContext->pstRfInputArray = pstRfInputArray;
	pstRfInputArray = NULL;

	hResult = ERR_OK;

END_FUNC:
	if (NULL != pstRfInputArray)					{ OxPAL_Free(pstRfInputArray); }

	return hResult;
}

STATIC HERROR pal_ch_emu_LoadTunerSetting (INT_PAL_CH_EMU_Context_t *pstContext, HxJSON_t jsSetup)
{
	HINT32				 nCnt, nTunerNum;
	HxJSON_t			 jsTunerArray, jsTuner;

	HERROR				 hErr;

	jsTunerArray = HLIB_JSON_Object_GetArray(jsSetup, "tuners");
	if (NULL == jsTunerArray)
	{
		HxLOG_Error("no [tuner] array in the json\n");
		return ERR_FAIL;
	}

	nTunerNum = HLIB_JSON_GetSize(jsTunerArray);
	if (0 >= nTunerNum)
	{
		HxLOG_Error("HHLIB_JSON_GetSize 0....\n");
		return ERR_FAIL;
	}

	hErr = pal_ch_emu_InitTuner((HUINT32)nTunerNum);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("pal_ch_emu_InitTuner err:\n");
		return ERR_FAIL;
	}

	for (nCnt = 0; nCnt < nTunerNum; nCnt++)
	{
		INT_PAL_CH_EMU_Tuner_t	*pstTuner = pal_ch_emu_GetTuner(nCnt);

		pstTuner->eSupportDeliType = 0;

		jsTuner = HxJSON_Array_Get(jsTunerArray, nCnt);
		if ((NULL != jsTuner) && (HxJSON_IsString(jsTuner)))
		{
			HCHAR		*szDeliType = (HCHAR *)HxJSON_String_Get(jsTuner);

			if (NULL != szDeliType)
			{
				if (HxSTD_StrStr(szDeliType, "sat") != NULL)
				{
					pstTuner->eSupportDeliType |= eDxDELIVERY_TYPE_SAT;
				}

				if (HxSTD_StrStr(szDeliType, "cab") != NULL)
				{
					pstTuner->eSupportDeliType |= eDxDELIVERY_TYPE_CAB;
				}

				if (HxSTD_StrStr(szDeliType, "ter") != NULL)
				{
					pstTuner->eSupportDeliType |= eDxDELIVERY_TYPE_TER;
				}
			}
		}
	}

	return ERR_OK;
}

STATIC HERROR pal_ch_emu_LoadRfInputSetting (INT_PAL_CH_EMU_Context_t *pstContext, HxJSON_t jsSetup)
{
	HINT32				 nCount;
	HINT32				 nRfInputIdx, nRfInputNum;
	HxJSON_t			 jsRfInputArray;
	HERROR				 hErr;

	jsRfInputArray = HLIB_JSON_Object_GetArray(jsSetup, "rfinputs");
	if (NULL == jsRfInputArray)
	{
		HxLOG_Error("no [rfinputs] array in json...\n");
		return ERR_FAIL;
	}

	nRfInputNum = HLIB_JSON_GetSize(jsRfInputArray);
	if (0 >= nRfInputNum)
	{
		HxLOG_Error("[rfinputs] array is 0 in json...\n");
		return ERR_FAIL;
	}

	hErr = pal_ch_emu_InitRfInput((HUINT32)nRfInputNum);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("pal_ch_emu_InitRfInput err:\n");
		return ERR_FAIL;
	}

	for (nRfInputIdx = 0, nCount = 0; nRfInputIdx < nRfInputNum; nRfInputIdx++)
	{
		HINT32				 nTunerCnt, nConnectable;
		HxJSON_t			 jsRfInput = HxJSON_Array_Get(jsRfInputArray, nRfInputIdx);
		PalCh_SatRfInput_t	*pstRfInput = pal_ch_emu_GetRfInput(nCount);
		HxJSON_t			 jsConnectable;

		if ((NULL == jsRfInput) || (NULL == pstRfInput))					{ continue; }

		jsConnectable = HLIB_JSON_Object_GetArray(jsRfInput, "connectable_tuner");
		if (NULL == jsConnectable)											{ continue; }

		nConnectable = HLIB_JSON_GetSize(jsConnectable);
		if (0 >= nConnectable)												{ continue; }

		for (nTunerCnt = 0; nTunerCnt < nConnectable; nTunerCnt++)
		{
			HUINT32					 ulTunerId;
			HxJSON_t				 jsTunerId = HxJSON_Array_Get(jsConnectable, nTunerCnt);
			INT_PAL_CH_EMU_Tuner_t	*pstTuner;

			if ((NULL != jsTunerId) && (HxJSON_IsInteger(jsTunerId)))
			{
				ulTunerId = (HUINT32)HxJSON_Integer_Get(jsTunerId);
				pstTuner = pal_ch_emu_GetTuner(ulTunerId);

				if (NULL != pstTuner)
				{
					pstTuner->stSatTunerInfo.aulRfInputId[pstTuner->stSatTunerInfo.ulNumRfInput] = (HUINT32)nRfInputIdx;
					pstTuner->stSatTunerInfo.ulNumRfInput++;

					pstRfInput->bAvailable = TRUE;
					pstRfInput->aulConnectableTunerId[pstRfInput->ulConnectableTunerNum] = ulTunerId;
					pstRfInput->ulConnectableTunerNum++;
				}
			}
		}

		if (TRUE == pstRfInput->bAvailable)
		{
			nCount++;
		}
 	}

	pstContext->ulRfInputNum = (nCount > 0) ? (HUINT32)nCount : 0;
	return (nCount > 0) ? ERR_OK : ERR_FAIL;
}

STATIC HERROR pal_ch_emu_LoadSetup (void)
{
	HBOOL						 bMounted = FALSE;
	HUINT32						 ulCount, ulFileLen = 0;
	HCHAR						*szFileData = NULL, *szTextData = NULL, *szCmdData = NULL;
	INT_PAL_CH_EMU_Context_t	*pstContext = pal_ch_emu_GetContext();
	INT_PAL_CH_EMU_SmbConf_t	 stSmbConf;
	HxJSON_t					 jsSetup = NULL;
	HERROR						 hErr, hResult = ERR_FAIL;

	HxSTD_MemSet(&stSmbConf, 0, sizeof(INT_PAL_CH_EMU_SmbConf_t));

	hErr = HLIB_FILE_ReadFile(PAL_CH_EMU_SETUP_FILE, (void **)&szFileData, &ulFileLen);
	if ((ERR_OK != hErr) || (NULL == szFileData) || (0 == ulFileLen))
	{
		HxLOG_Error("HLIB_FILE_ReadFile err:\n");
		goto END_FUNC;
	}

	szTextData = (HCHAR *)HLIB_STD_MemAlloc(ulFileLen + 1);
	if (NULL == szTextData)
	{
		HxLOG_Error("Memory allocation failed:\n");
		goto END_FUNC;
	}

	HxSTD_MemCopy(szTextData, szFileData, ulFileLen);
	szTextData[ulFileLen] = '\0';

	jsSetup = HLIB_JSON_Decode((const HCHAR *)szTextData);
	if (NULL == jsSetup)
	{
		HxLOG_Error("HLIB_JSON_Decode err: [ %s ]\n", szTextData);
		goto END_FUNC;
	}

	// Tuner Setting:
	hErr = pal_ch_emu_LoadTunerSetting(pstContext, jsSetup);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("pal_ch_emu_LoadTunerSetting err: [ %s ]\n", szTextData);
		goto END_FUNC;
	}

#if defined(CONFIG_MW_CH_SATELLITE)
	// RF-Input Setting
	hErr = pal_ch_emu_LoadRfInputSetting(pstContext, jsSetup);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("pal_ch_emu_LoadRfInputSetting err: [ %s ]\n", szTextData);
		goto END_FUNC;
	}
#endif

#if 0
{
	HUINT32				 ulSubCnt;

	HLIB_CMD_Printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");

	HLIB_CMD_Printf("Tuner Num : %d\n", pstContext->ulTunerNum);
	for (ulCount = 0; ulCount < pstContext->ulTunerNum; ulCount++)
	{
		INT_PAL_CH_EMU_Tuner_t		*pstTuner = pal_ch_emu_GetTuner(ulCount);

		HLIB_CMD_Printf("- Tuner (%d)\n", ulCount);
		HLIB_CMD_Printf("   >> Connectable RF-Input : [");
		for (ulSubCnt = 0; ulSubCnt < pstTuner->stSatTunerInfo.ulNumRfInput; ulSubCnt++)
		{
			if (ulSubCnt != 0)			{ HLIB_CMD_Printf(","); }
			HLIB_CMD_Printf("%d", pstTuner->stSatTunerInfo.aulRfInputId[ulSubCnt]);
		}
		HLIB_CMD_Printf("]\n\n\n");

	}

	HLIB_CMD_Printf("RF-Input Num : %d\n", pstContext->ulRfInputNum);
	for (ulCount = 0; ulCount < pstContext->ulRfInputNum; ulCount++)
	{
		PalCh_SatRfInput_t		*pstRfInput = pal_ch_emu_GetRfInput(ulCount);

		HLIB_CMD_Printf("- RF-Input (%d)\n", ulCount);
		HLIB_CMD_Printf("   >> Connectable Tuner : [");
		for (ulSubCnt = 0; ulSubCnt < pstRfInput->ulConnectableTunerNum; ulSubCnt++)
		{
			if (ulSubCnt != 0)			{ HLIB_CMD_Printf(","); }
			HLIB_CMD_Printf("%d", pstRfInput->aulConnectableTunerId[ulSubCnt]);
		}
		HLIB_CMD_Printf("]\n\n\n");

	}
	HLIB_CMD_Printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
}
#endif

	// Load SMB data
	hErr = pal_ch_emu_ParseSmbConf(jsSetup, &stSmbConf);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("pal_ch_emu_ParseSmbConf err:\n");
		goto END_FUNC;
	}

	// mount the SMB
	szCmdData = (HCHAR *)HLIB_STD_MemCalloc(2048);
	if (NULL == szCmdData)
	{
		HxLOG_Error("Memory allocation failed:\n");
		goto END_FUNC;
	}

	if (TRUE == pstContext->bSmbMounted)
	{
		HxSTD_PrintToStrN(szCmdData, 2048, "umount %s", PAL_CH_EMU_MOUNT_DIR);
		VK_SYSTEM_Command((const char *)szCmdData);
		pstContext->bSmbMounted = FALSE;
		VK_TASK_Sleep(50);
	}

	if ('\0' == stSmbConf.szPassword[0])
	{
		HxSTD_PrintToStrN(szCmdData, 2048, "mount.cifs //%s %s -o user=%s,nounix,noserverino,iocharset=utf8,ro > %s",
						stSmbConf.szSmbPath, PAL_CH_EMU_MOUNT_DIR, stSmbConf.szUserName, PAL_CH_EMU_MOUNT_RESULT);
	}
	else
	{
		HxSTD_PrintToStrN(szCmdData, 2048, "mount.cifs //%s %s -o user=%s,pass=%s,nounix,noserverino,iocharset=utf8,ro > %s",
						stSmbConf.szSmbPath, PAL_CH_EMU_MOUNT_DIR, stSmbConf.szUserName, stSmbConf.szPassword, PAL_CH_EMU_MOUNT_RESULT);
	}

	HLIB_DIR_Make(PAL_CH_EMU_MOUNT_DIR);
	VK_SYSTEM_Command((const char *)szCmdData);

	for (ulCount = 0; ulCount < 10; ulCount++)
	{
		HxFILE_t		 fpResultFile;

		VK_TASK_Sleep(100);

		fpResultFile = HLIB_FILE_Open(PAL_CH_EMU_MOUNT_RESULT, "r");
		if (NULL == fpResultFile)
		{
			continue;
		}

		bMounted = TRUE;
		while (HLIB_FILE_GetString(fpResultFile, (const HUINT8 *)szCmdData, sizeof(szCmdData)))
		{
			if (HxSTD_StrStr(szCmdData, "error") != NULL)
			{
				bMounted = FALSE;
				break;
			}
		}

		HLIB_FILE_Close(fpResultFile);
		break;
	}

	if (TRUE == bMounted)
	{
		if (HLIB_FILE_Exist(PAL_CH_EMU_INDEX_FILE) == TRUE)
		{
			pstContext->bSmbMounted = TRUE;
			HxSTD_MemCopy(&(pstContext->stSmbConf), &stSmbConf, sizeof(INT_PAL_CH_EMU_SmbConf_t));

			hResult = ERR_OK;
		}
	}

END_FUNC:
	if (NULL != szFileData)					{ HLIB_FILE_FreeBuffer(szFileData); }
	if (NULL != szTextData)					{ HLIB_STD_MemFree(szTextData); }
	if (NULL != szCmdData)					{ HLIB_STD_MemFree(szCmdData); }
	if (NULL != jsSetup)					{ HLIB_JSON_Delete(jsSetup); }

	return hResult;
}


STATIC HERROR pal_ch_emu_checkRfInputConnectStatus (void)
{
	HUINT32						 ulRfInputId, ulTunerId;
	INT_PAL_CH_EMU_Context_t	*pstContext = pal_ch_emu_GetContext();
	INT_PAL_CH_EMU_Tuner_t		*pstTuner;
	PalCh_SatRfInput_t			*pstRfInput;

	for (ulRfInputId = 0; ulRfInputId < pstContext->ulRfInputNum; ulRfInputId++)
	{
		pstRfInput = pal_ch_emu_GetRfInput(ulRfInputId);
		pstRfInput->ulConnectedTunerNum = 0;
	}

	for (ulTunerId = 0; ulTunerId < pstContext->ulTunerNum; ulTunerId++)
	{
		pstTuner = pal_ch_emu_GetTuner(ulTunerId);
		pstRfInput = pal_ch_emu_GetRfInput((HUINT32)pstTuner->nRfInputId);

		if (NULL != pstRfInput)
		{
			pstRfInput->aulConnectedTunerId[pstRfInput->ulConnectedTunerNum] = ulTunerId;
			pstRfInput->ulConnectedTunerNum ++;
		}
	}

	return ERR_OK;
}

#if defined(CONFIG_MW_CH_SATELLITE)
STATIC HERROR pal_ch_emu_InitSatTransponder (CH_SAT_TuneParam_t *pstTuneParam)
{
	HxSTD_MemSet(pstTuneParam, 0, sizeof(CH_SAT_TuneParam_t));

	pstTuneParam->b22kTone = (HBOOL)-1;
	pstTuneParam->etFecCodeRate = DI_CH_SCODERATE_AUTO;
	pstTuneParam->etPolarization = DI_CH_POLAR_AUTO;
	return ERR_OK;
}

STATIC HERROR pal_ch_emu_ParseSatTransponder (HxJSON_t jsTpInfo, CH_SAT_TuneParam_t *pstTuneParam)
{
	HINT32			 nValue;
	HCHAR			*szValue;
	HCHAR			*szAntType = NULL;

	HxSTD_MemSet(pstTuneParam, 0, sizeof(CH_SAT_TuneParam_t));

	szAntType = (HCHAR *)HLIB_JSON_Object_GetString(jsTpInfo, "anttype");
	if (NULL == szAntType)
	{
		HxLOG_Error("No 'anttype' in the TPINFO index\n");
		return ERR_FAIL;
	}

	if (HxSTD_StrCmp(szAntType, "lnb") == 0)
	{
		// Nothing in the LNB
	}
	else
	{
		HxLOG_Error("anttype [%s] not supported\n");
		return ERR_FAIL;
	}

	// Frequency:
	nValue = HLIB_JSON_Object_GetInteger(jsTpInfo, "frequency");
	if (nValue <= 0)
	{
		HxLOG_Error("No 'frequency' in the TPINFO index\n");
		return ERR_FAIL;
	}
	pstTuneParam->ulFrequency = (HUINT32)nValue;

	// Symbol Rate
	nValue = HLIB_JSON_Object_GetInteger(jsTpInfo, "symbolrate");
	if (nValue <= 0)
	{
		HxLOG_Error("No 'symbolrate' in the TPINFO index\n");
		return ERR_FAIL;
	}
	pstTuneParam->ulSymbolRate = (HUINT32)nValue;

	// Code Rate : (Auto 가능)
	pstTuneParam->etFecCodeRate = DI_CH_SCODERATE_AUTO;
	szValue = (HCHAR *)HLIB_JSON_Object_GetString(jsTpInfo, "coderate");
	if ((NULL != szValue) && ('\0' != *szValue))
	{
		if (HxSTD_StrCmp(szValue, "1/2") == 0)					{ pstTuneParam->etFecCodeRate = DI_CH_SCODERATE_1_2; }
		else if (HxSTD_StrCmp(szValue, "2/3") == 0)				{ pstTuneParam->etFecCodeRate = DI_CH_SCODERATE_2_3; }
		else if (HxSTD_StrCmp(szValue, "3/4") == 0)				{ pstTuneParam->etFecCodeRate = DI_CH_SCODERATE_3_4; }
		else if (HxSTD_StrCmp(szValue, "5/6") == 0)				{ pstTuneParam->etFecCodeRate = DI_CH_SCODERATE_5_6; }
		else if (HxSTD_StrCmp(szValue, "7/8") == 0)				{ pstTuneParam->etFecCodeRate = DI_CH_SCODERATE_7_8; }
		else if (HxSTD_StrCmp(szValue, "3/5") == 0)				{ pstTuneParam->etFecCodeRate = DI_CH_S2CODERATE_3_5; }
		else if (HxSTD_StrCmp(szValue, "4/5") == 0)				{ pstTuneParam->etFecCodeRate = DI_CH_S2CODERATE_4_5; }
		else if (HxSTD_StrCmp(szValue, "8/9") == 0)				{ pstTuneParam->etFecCodeRate = DI_CH_S2CODERATE_8_9; }
		else if (HxSTD_StrCmp(szValue, "9/10") == 0)			{ pstTuneParam->etFecCodeRate = DI_CH_S2CODERATE_9_10; }
		else if (HxSTD_StrCmp(szValue, "5/11") == 0)			{ pstTuneParam->etFecCodeRate = DI_CH_SCODERATE_5_11; }
		else if (HxSTD_StrCmp(szValue, "6/7") == 0)				{ pstTuneParam->etFecCodeRate = DI_CH_SCODERATE_6_7; }
	}

	// Polarization : (Auto 가능)
	pstTuneParam->etPolarization = DI_CH_POLAR_AUTO;
	szValue = (HCHAR *)HLIB_JSON_Object_GetString(jsTpInfo, "polar");
	if ((NULL != szValue) && ('\0' != *szValue))
	{
		if (HxSTD_StrCmp(szValue, "hor") == 0)					{ pstTuneParam->etPolarization = DI_CH_POLAR_HOR; }
		else if (HxSTD_StrCmp(szValue, "ver") == 0)				{ pstTuneParam->etPolarization = DI_CH_POLAR_VER; }
		else if (HxSTD_StrCmp(szValue, "left") == 0)			{ pstTuneParam->etPolarization = DI_CH_POLAR_LEFT; }
		else if (HxSTD_StrCmp(szValue, "right") == 0)			{ pstTuneParam->etPolarization = DI_CH_POLAR_RIGHT; }
	}

	// 22K Tone : (emul 한정 Auto 가능)
	pstTuneParam->b22kTone = (HBOOL)HLIB_JSON_Object_GetIntegerWithDefaultValue(jsTpInfo, "22ktone", -1);

	return ERR_OK;
}

STATIC HBOOL pal_ch_emu_CompareSatTpInfo (CH_SAT_TuneParam_t *pstTableTp, CH_SAT_TuneParam_t *pstReqTp)
{
	HINT32			 nDiffFreq;

	if (pstTableTp->etAntennaType != pstReqTp->etAntennaType)				{ return FALSE; }

	switch (pstTableTp->etAntennaType)
	{
	case DI_CH_ANT_TYPE_LNB:
		break;

	case DI_CH_ANT_TYPE_DISEQC:
		if (pstTableTp->etDiseqcInput != pstReqTp->etDiseqcInput)
		{
			return FALSE;
		}

		if (pstTableTp->etDiseqcVersion != pstReqTp->etDiseqcVersion)
		{
			return FALSE;
		}

		break;

	case DI_CH_ANT_TYPE_SCD:
		if (pstTableTp->etDiseqcInput != pstReqTp->etDiseqcInput)
		{
			return FALSE;
		}

		if (pstTableTp->ulScdUserBandNum != pstReqTp->ulScdUserBandNum)
		{
			return FALSE;
		}

		if (pstTableTp->ulScdUserBandFreq != pstReqTp->ulScdUserBandFreq)
		{
			return FALSE;
		}

		break;

	default:
		return FALSE;
	}

	if ((DI_CH_PSK_AUTO != pstTableTp->etModulation) && (DI_CH_PSK_AUTO != pstReqTp->etModulation) &&
		(pstTableTp->etModulation != pstReqTp->etModulation))
	{
		return FALSE;
	}

	if ((DI_CH_SCODERATE_AUTO != pstTableTp->etFecCodeRate) && (DI_CH_SCODERATE_AUTO != pstReqTp->etFecCodeRate) &&
		(pstTableTp->etFecCodeRate != pstReqTp->etFecCodeRate))
	{
		return FALSE;
	}

	if ((DI_CH_POLAR_AUTO != pstTableTp->etPolarization) && (DI_CH_POLAR_AUTO != pstReqTp->etPolarization) &&
		(pstTableTp->etPolarization != pstReqTp->etPolarization))
	{
		return FALSE;
	}

	if (pstTableTp->ulSymbolRate != pstReqTp->ulSymbolRate)
	{
		return FALSE;
	}

	nDiffFreq = (HINT32)pstTableTp->ulFrequency - (HINT32)pstReqTp->ulFrequency;
	if ((nDiffFreq > 5) || (nDiffFreq < -5))
	{
		return FALSE;
	}

	return TRUE;
}

#endif

#if defined(CONFIG_MW_CH_CABLE)
STATIC HERROR pal_ch_emu_ParseCabTransponder (HxJSON_t jsTpInfo)
{
	return ERR_OK;
}

STATIC HBOOL pal_ch_emu_CompareCabTpInfo (CH_CAB_TuneParam_t *pstTableTp, CH_CAB_TuneParam_t *pstReqTp)
{
	HINT32			 nDiffFreq;

	if (pstTableTp->ulSymbolRate != pstReqTp->ulSymbolRate)
	{
		return FALSE;
	}

	nDiffFreq = (HINT32)pstTableTp->ulFrequency - (HINT32)pstReqTp->ulFrequency;
	if ((nDiffFreq > 2) || (nDiffFreq < -2))
	{
		return FALSE;
	}

	return TRUE;
}
#endif

#if defined(CONFIG_MW_CH_TERRESTRIAL)
STATIC HERROR pal_ch_emu_ParseTerTransponder (HxJSON_t jsTpInfo)
{
	return ERR_OK;
}

STATIC HBOOL pal_ch_emu_CompareTerTpInfo (CH_TER_TuneParam_t *pstTableTp, CH_TER_TuneParam_t *pstReqTp)
{
	HINT32			 nDiffFreq;

	nDiffFreq = (HINT32)pstTableTp->ulFrequency - (HINT32)pstReqTp->ulFrequency;
	if ((nDiffFreq > 2) || (nDiffFreq < -2))
	{
		return FALSE;
	}

	return TRUE;
}
#endif

STATIC HERROR pal_ch_emu_LoadTsIndexFile (void)
{
	HINT32						 nTpIdx, nTpNum;
	HUINT32						 ulFileLen = 0;
	HCHAR						*szFileData = NULL, *szTextData = NULL;
	INT_PAL_CH_EMU_Context_t	*pstContext = pal_ch_emu_GetContext();
	HxJSON_t					 jsIndexRoot = NULL;
	HxJSON_t					 jsTpArray;
	HxList_t					*pstTpList = NULL;
	HERROR						 hErr, hResult = ERR_FAIL;

	hErr = HLIB_FILE_ReadFile(PAL_CH_EMU_INDEX_FILE, (void **)&szFileData, &ulFileLen);
	if ((ERR_OK != hErr) || (NULL == szFileData) || (0 == ulFileLen))
	{
		HxLOG_Error("HLIB_FILE_ReadFile err:\n");
		goto END_FUNC;
	}

	szTextData = (HCHAR *)HLIB_STD_MemAlloc(ulFileLen + 1);
	if (NULL == szTextData)
	{
		HxLOG_Error("Memory allocation failed:\n");
		goto END_FUNC;
	}

	HxSTD_MemCopy(szTextData, szFileData, ulFileLen);
	szTextData[ulFileLen] = '\0';

	jsIndexRoot = HLIB_JSON_Decode((const HCHAR *)szTextData);
	if (NULL == jsIndexRoot)
	{
		HxLOG_Error("HLIB_JSON_Decode err: [ %s ]\n", szTextData);
		goto END_FUNC;
	}

	jsTpArray = HLIB_JSON_Object_GetArray(jsIndexRoot, "transponders");
	if (NULL == jsTpArray)
	{
		HxLOG_Error("No transponders objects:\n");
		goto END_FUNC;
	}

	nTpNum = HLIB_JSON_GetSize(jsTpArray);
	for (nTpIdx = 0; nTpIdx < nTpNum; nTpIdx++)
	{
		HCHAR			*szDelivery, *szFileName;
		HxJSON_t		 jsTpInfo = HxJSON_Array_Get(jsTpArray, nTpIdx);
		INT_PAL_CH_EMU_TpInfo_t	 stEmuTpInfo;

		if (NULL == jsTpInfo)			{ continue; }

		szDelivery = (HCHAR *)HLIB_JSON_Object_GetString(jsTpInfo, "delivery");
		if ((NULL != szDelivery) && ('\0' != *szDelivery))
		{
			hErr = ERR_FAIL;

#if defined(CONFIG_MW_CH_SATELLITE)
			if (HxSTD_StrCmp(szDelivery, "sat") == 0)
			{
				CH_SAT_TuneParam_t		 stTuneParam;

				hErr = pal_ch_emu_ParseSatTransponder(jsTpInfo, &stTuneParam);
				if (ERR_OK == hErr)
				{
					stEmuTpInfo.eChType = DI_CH_TYPE_SAT;
					HxSTD_MemCopy(&(stEmuTpInfo.unDiParam.stSat), &stTuneParam, sizeof(CH_SAT_TuneParam_t));
				}
			} else
#endif
#if defined(CONFIG_MW_CH_CABLE)
			if (HxSTD_StrCmp(szDelivery, "cab") == 0)
			{
				CH_CAB_TuneParam_t		 stTuneParam;

				hErr = pal_ch_emu_ParseCabTransponder(jsTpInfo, &stTuneParam);
				if (ERR_OK == hErr)
				{
					stEmuTpInfo.eChType = DI_CH_TYPE_CAB;
					HxSTD_MemCopy(&(stEmuTpInfo.unDiParam.stCab), &stTuneParam, sizeof(CH_CAB_TuneParam_t));
				}
			} else
#endif
#if defined(CONFIG_MW_CH_TERRESTRIAL)
			if (HxSTD_StrCmp(szDelivery, "ter") == 0)
			{
				CH_TER_TuneParam_t		 stTuneParam;

				hErr = pal_ch_emu_ParseTerTransponder(jsTpInfo, &stTuneParam);
				if (ERR_OK == hErr)
				{
					stEmuTpInfo.eChType = DI_CH_TYPE_TER;
					HxSTD_MemCopy(&(stEmuTpInfo.unDiParam.stTer), &stTuneParam, sizeof(CH_TER_TuneParam_t));
				}
			} else
#endif
			{
				hErr = ERR_FAIL;
			}

			if (ERR_OK == hErr)
			{
				szFileName = (HCHAR *)HLIB_JSON_Object_GetString(jsTpInfo, "file");
				if ((NULL != szFileName) && ('\0' != *szFileName))
				{
					INT_PAL_CH_EMU_TpInfo_t		*pstTpObj;

					HxSTD_PrintToStrN(stEmuTpInfo.szFileName, 256, "%s/%s", PAL_CH_EMU_MOUNT_DIR, szFileName);

					pstTpObj = (INT_PAL_CH_EMU_TpInfo_t *)HLIB_STD_MemAlloc(sizeof(INT_PAL_CH_EMU_TpInfo_t));
					if (NULL != pstTpObj)
					{
						HxSTD_MemCopy(pstTpObj, &stEmuTpInfo, sizeof(INT_PAL_CH_EMU_TpInfo_t));
						pstTpList = HLIB_LIST_Append(pstTpList, (void *)pstTpObj);
					}
				}
			}
		}
	}

	nTpNum = (HINT32)HLIB_LIST_Length(pstTpList);
	if (nTpNum > 0)
	{
		HUINT32 					 ulArrayLen = 0;
		INT_PAL_CH_EMU_TpInfo_t		*pstNewArray = (INT_PAL_CH_EMU_TpInfo_t *)OxPAL_Malloc(sizeof(INT_PAL_CH_EMU_TpInfo_t) * nTpNum);

		if (NULL != pstNewArray)
		{
			HxList_t		*pstListItem;

			for (pstListItem = pstTpList; NULL != pstListItem; pstListItem = pstListItem->next)
			{
				INT_PAL_CH_EMU_TpInfo_t		*pstSrcTp = (INT_PAL_CH_EMU_TpInfo_t *)HLIB_LIST_Data(pstListItem);

				if (NULL != pstSrcTp)
				{
					HxSTD_MemCopy(&(pstNewArray[ulArrayLen]), pstSrcTp, sizeof(INT_PAL_CH_EMU_TpInfo_t));
					ulArrayLen++;
				}
			}
		}

		if (NULL != pstContext->pstTpArray)				{ OxPAL_Free(pstContext->pstTpArray); }

		pstContext->ulTpNum = ulArrayLen;
		pstContext->pstTpArray = pstNewArray;
	}

	hResult = ERR_OK;

	// For Debug Logging
#if 0
{
	HUINT32			 ulCount;

	HLIB_CMD_Printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
	HLIB_CMD_Printf("=================  Emulator TP ======================\n");
	for (ulCount = 0; ulCount < pstContext->ulTpNum; ulCount++)
	{
		HLIB_CMD_Printf(" - TP (%04d) : Freq(%d), Name(%s)\n", ulCount, pstContext->pstTpArray[ulCount].unDiParam.stSat.ulFrequency, pstContext->pstTpArray[ulCount].szFileName);
	}
	HLIB_CMD_Printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
}
#endif


END_FUNC:
	if (NULL != szFileData)					{ HLIB_FILE_FreeBuffer(szFileData); }
	if (NULL != szTextData)					{ HLIB_STD_MemFree(szTextData); }
	if (NULL != jsIndexRoot)				{ HLIB_JSON_Delete(jsIndexRoot); }
	if (NULL != pstTpList)
	{
		HLIB_LIST_RemoveAllFunc(pstTpList, HLIB_STD_MemFree_CB);
	}

	return hResult;
}

STATIC INT_PAL_CH_EMU_TpInfo_t *pal_ch_emu_GetVirtualTpFromTuningParam (DI_CH_Type_e eChType, void *pvDiParam, HUINT32 *pulTpListIndex)
{
	HBOOL						 bFound = FALSE;
	HUINT32						 ulIndex;
	INT_PAL_CH_EMU_Context_t	*pstContext = pal_ch_emu_GetContext();

	if ((NULL != pstContext->pstTpArray) && (0 < pstContext->ulTpNum))
	{
		for (ulIndex = 0; ulIndex < pstContext->ulTpNum; ulIndex++)
		{
			INT_PAL_CH_EMU_TpInfo_t		*pstTpInfo = &(pstContext->pstTpArray[ulIndex]);

			if (eChType == pstTpInfo->eChType)
			{
				switch (eChType)
				{
#if defined(CONFIG_MW_CH_SATELLITE)
				case DI_CH_TYPE_SAT:
					bFound = pal_ch_emu_CompareSatTpInfo(&(pstTpInfo->unDiParam.stSat), (CH_SAT_TuneParam_t *)pvDiParam);
					break;
#endif

#if defined(CONFIG_MW_CH_CABLE)
				case DI_CH_TYPE_CAB:
					bFound = pal_ch_emu_CompareCabTpInfo(&(pstTpInfo->unDiParam.stCab), (CH_CAB_TuneParam_t *)pvDiParam);
					break;
#endif

#if defined(CONFIG_MW_CH_TERRESTRIAL)
				case DI_CH_TYPE_TER:
					bFound = pal_ch_emu_CompareTerTpInfo(&(pstTpInfo->unDiParam.stTer), (CH_TER_TuneParam_t *)pvDiParam);
					break;
#endif

				default:
					bFound = FALSE;
					break;
				}

				if (TRUE == bFound)
				{
					if (NULL != pulTpListIndex)
					{
						*pulTpListIndex = ulIndex;
					}

					return pstTpInfo;
				}
			}
		}
	}

	return NULL;
}
static void pal_ch_emu_CmdTask (void *arg)
{
	INT_PAL_CH_EMU_Context_t	*pstContext = pal_ch_emu_GetContext();
	INT_PAL_CH_EMU_MSG_t		 stMsg;
	HERROR						 hErr;

	while (1)
	{
		hErr = VK_MSG_Receive(pstContext->ulMsgQID, &stMsg, sizeof(INT_PAL_CH_EMU_MSG_t));
		if (ERR_OK == hErr)
		{
			switch(stMsg.eCmd)
			{
			case eCH_EMU_CMD_RewindPlayback:
				DI_PVR_PLAY_SetPosition(stMsg.ulParam1, 0);
				VK_TASK_Sleep(10);
				DI_PVR_PLAY_Resume(stMsg.ulParam1);
				break;

			case eCH_EMU_CMD_NotifyTuneResult:
				if (NULL != pstContext->pfnNotifier)
				{
					pstContext->pfnNotifier(stMsg.ulParam1, stMsg.ulParam2, stMsg.ulParam3, stMsg.ulParam4);
				}

			default:
				break;
			}
		}
		else
		{
			VK_TASK_Sleep(100);
		}
	}
}

STATIC HERROR pal_ch_emu_InitThread (void)
{
	unsigned long				 ulMsgQ = 0;
	unsigned long				 ulTaskId = 0;
	INT_PAL_CH_EMU_Context_t	*pstContext = pal_ch_emu_GetContext();
	HERROR						 hErr;

	hErr = VK_MSG_Create(64, sizeof(INT_PAL_CH_EMU_MSG_t), "CH_EMU_Q", &ulMsgQ, VK_SUSPENDTYPE_FIFO);
	if (VK_OK != hErr)
	{
		HxLOG_Error("VK_MSG_Create err:\n");
		goto ERROR;
	}

	hErr = VK_TASK_Create(pal_ch_emu_CmdTask, VK_TASK_PRIORITY_MW_REF, SIZE_32K, "CH_EMU_TASK", (void *)pstContext, &ulTaskId, 0);
	if (VK_OK != hErr)
	{
		HxLOG_Error("VK_TASK_Create err:\n");
		goto ERROR;
	}

	hErr = VK_TASK_Start(ulTaskId);
	if (VK_OK != hErr)
	{
		HxLOG_Error("VK_TASK_Start err:\n");
		goto ERROR;
	}

	pstContext->ulMsgQID = ulMsgQ;
	pstContext->ulTaskId = ulTaskId;

	return ERR_OK;

ERROR:
	if (0 != ulMsgQ)
	{
		VK_MSG_Destroy(ulMsgQ);
	}

	if (0 != ulTaskId)
	{
		VK_TASK_Destroy(ulTaskId);
	}

	return ERR_FAIL;
}

#define _____DEBUG_FUNCTIONS_____
STATIC INT_PAL_CH_EMU_SmbConf_t		 s_stPalChEmu_CmdSetup;

STATIC INT_PAL_CH_EMU_SmbConf_t *pal_ch_emu_GetCmdSmbConf (void)
{
	return &s_stPalChEmu_CmdSetup;
}

STATIC HERROR pal_ch_emu_PrintSmbConf (INT_PAL_CH_EMU_SmbConf_t *pstSmbConf)
{
	HLIB_CMD_Printf("   - SMB path  : [%s]\n", pstSmbConf->szSmbPath);
	HLIB_CMD_Printf("   - User Name : [%s]\n", pstSmbConf->szUserName);
	HLIB_CMD_Printf("   - Password  : [%s]\n", pstSmbConf->szPassword);

	return ERR_OK;
}

STATIC HINT32 pal_ch_emu_CmdSetSmbConf (void *pvArg)
{
	HCHAR		*szArg, *szValue;
	HCHAR		*szArgList = (HCHAR *)pvArg;
	INT_PAL_CH_EMU_SmbConf_t	*pstSmbConf = pal_ch_emu_GetCmdSmbConf();


	szArg = (char *)HLIB_CMD_GetNextArg(&szArgList);
	if (NULL == szArg)
	{
		return HxCMD_ERR;
	}

	if (HLIB_STD_StrCaseCmp(szArg, "smb_path") == 0)
	{
		szValue = (char *)HLIB_CMD_GetNextArg(&szArgList);
		if ((NULL == szValue) || ('\0' == *szValue))
		{
			return HxCMD_ERR;
		}

		HxSTD_StrNCpy(pstSmbConf->szSmbPath, szValue, 256);
		pstSmbConf->szSmbPath[255] = '\0';
	}
	else if (HLIB_STD_StrCaseCmp(szArg, "smb_user") == 0)
	{
		szValue = (char *)HLIB_CMD_GetNextArg(&szArgList);
		if ((NULL == szValue) || ('\0' == *szValue))
		{
			return HxCMD_ERR;
		}

		HxSTD_StrNCpy(pstSmbConf->szUserName, szValue, 32);
		pstSmbConf->szUserName[31] = '\0';
	}
	else if (HLIB_STD_StrCaseCmp(szArg, "smb_pass") == 0)
	{
		szValue = (char *)HLIB_CMD_GetNextArg(&szArgList);
		if ((NULL == szValue) || ('\0' == *szValue))
		{
			return HxCMD_ERR;
		}

		HxSTD_StrNCpy(pstSmbConf->szPassword, szValue, 64);
		pstSmbConf->szPassword[63] = '\0';
	}
	else
	{
		HLIB_CMD_Printf(">> Undetermined Arg: [%s]\n", szArg);
		return HxCMD_ERR;
	}

	return HxCMD_OK;
}

STATIC HINT32 pal_ch_emu_CmdPrintCmdSmbConf (void *pvArg)
{
	INT_PAL_CH_EMU_SmbConf_t	*pstSmbConf = pal_ch_emu_GetCmdSmbConf();

	HLIB_CMD_Printf(">> Current CMD Setup Values:\n");

	pal_ch_emu_PrintSmbConf(pstSmbConf);
	return HxCMD_OK;
}

STATIC HINT32 pal_ch_emu_CmdWriteSmbConf (void *pvArg)
{
	INT_PAL_CH_EMU_SmbConf_t	*pstSmbConf = pal_ch_emu_GetCmdSmbConf();

	if ('\0' == pstSmbConf->szSmbPath)
	{
		HLIB_CMD_Printf("> SMB Path NULL... failed\n");
		return HxCMD_ERR;
	}

	if ('\0' == pstSmbConf->szUserName)
	{
		HLIB_CMD_Printf("> SMB User Name NULL... failed\n");
		return HxCMD_ERR;
	}

	if (pal_ch_emu_WriteJsonSmbConf(pstSmbConf) != ERR_OK)
	{
		HLIB_CMD_Printf("> pal_ch_emu_WriteJsonSmbConf err:\n");
		return HxCMD_ERR;
	}

	return HxCMD_OK;
}

STATIC HINT32 pal_ch_emu_CmdWriteTpIndexTemplate (void *pvArg)
{
	HINT32		 nJWriter = 0;
	HINT32		 nTpListTxtLen = 0;
	HCHAR		*szTpListJsonTxt = NULL;

	nJWriter = HLIB_JSONWRITER_Open();
	if (0 == nJWriter)
	{
		HLIB_CMD_Printf("> HLIB_JSONWRITER_Open NULL\n");
		return HxCMD_ERR;
	}

	HLIB_JSONWRITER_SetEncodeExceptionToken(nJWriter, ":/ ");
	HLIB_JSONWRITER_Clear(nJWriter);

	HxJSONWRITER_ObjectBegin(nJWriter);
	{
		HxJSONWRITER_Array(nJWriter, "transponders");
		{
#if defined(CONFIG_MW_CH_SATELLITE)
			{
				HxJSONWRITER_ObjectBegin(nJWriter);
				HxJSONWRITER_String(nJWriter, "delivery", "sat");
				HxJSONWRITER_String(nJWriter, "anttype", "lnb");
				HxJSONWRITER_Integer(nJWriter, "frequency", 2004);
				HxJSONWRITER_Integer(nJWriter, "symbolrate", 27500);
				HxJSONWRITER_Integer(nJWriter, "22ktone", 0);
				HxJSONWRITER_String(nJWriter, "file", "TS_1A.ts");
				HxJSONWRITER_ObjectEnd(nJWriter);
			}
			{
				HxJSONWRITER_ObjectBegin(nJWriter);
				HxJSONWRITER_String(nJWriter, "delivery", "sat");
				HxJSONWRITER_String(nJWriter, "anttype", "diseqc");
				HxJSONWRITER_String(nJWriter, "diseqc", "a");
				HxJSONWRITER_Integer(nJWriter, "frequency", 2004);
				HxJSONWRITER_Integer(nJWriter, "symbolrate", 27500);
				HxJSONWRITER_Integer(nJWriter, "22ktone", 0);
				HxJSONWRITER_String(nJWriter, "file", "TS_1A.ts");
				HxJSONWRITER_ObjectEnd(nJWriter);
			}

#endif
#if defined(CONFIG_MW_CH_CABLE)
			{
				HxJSONWRITER_ObjectBegin(nJWriter);
				HxJSONWRITER_String(nJWriter, "delivery", "cab");
				HxJSONWRITER_ObjectEnd(nJWriter);
			}
#endif
#if defined(CONFIG_MW_CH_TERRESTRIAL)
			{
				HxJSONWRITER_ObjectBegin(nJWriter);
				HxJSONWRITER_String(nJWriter, "delivery", "ter");
				HxJSONWRITER_ObjectEnd(nJWriter);
			}
#endif
		}
		HxJSONWRITER_ArrayEnd(nJWriter);
	}
	HxJSONWRITER_ObjectEnd(nJWriter);


	szTpListJsonTxt = (HCHAR *)HLIB_JSONWRITER_GetJSON(nJWriter);
	nTpListTxtLen = HLIB_JSONWRITER_GetLength(nJWriter);
	if ((NULL != szTpListJsonTxt) && (nTpListTxtLen > 0))
	{
		(void)HLIB_FILE_WriteFile("/index.json", (void *)szTpListJsonTxt, nTpListTxtLen);
	}

	HLIB_JSONWRITER_Close(nJWriter);

	return HxCMD_OK;
}

STATIC HERROR pal_ch_emu_InitDbgCmd (void)
{
#define	hCmdHandle		"pal_channel_emul"
	HxSTD_MemSet(&s_stPalChEmu_CmdSetup, 0, sizeof(INT_PAL_CH_EMU_SmbConf_t));

	HLIB_CMD_RegisterWord(hCmdHandle,
						pal_ch_emu_CmdSetSmbConf,
						"smbconf",
						"Setting the smbconf values.",
						"smbconf [smb_path|smb_user|smb_pass] <value>");

	HLIB_CMD_RegisterWord(hCmdHandle,
						pal_ch_emu_CmdPrintCmdSmbConf,
						"print_smbconf",
						"Print the contents of the smbconf data in the cmd tool...",
						"print_smbconf");


	HLIB_CMD_RegisterWord(hCmdHandle,
						pal_ch_emu_CmdWriteSmbConf,
						"write_smbconf",
						"Write the contents of the setup data to file...",
						"write_smbconf");

	HLIB_CMD_RegisterWord(hCmdHandle,
						pal_ch_emu_CmdWriteTpIndexTemplate,
						"write_tp_template",
						"Write the tp index file template",
						"write_tp_template");

	return ERR_OK;
}

#define ___MEMBER_FUNCTIONS___

HERROR PAL_CH_EMU_Init (void *pvCbFunc)
{
	INT_PAL_CH_EMU_Context_t	*pstContext = pal_ch_emu_GetContext();
	HERROR						 hErr;

	HxSTD_MemSet(pstContext, 0, sizeof(INT_PAL_CH_EMU_Context_t));

	hErr = pal_ch_emu_InitDbgCmd();
	if (ERR_OK != hErr)
	{
		HxLOG_Error("pal_ch_emu_InitDbgCmd err:\n");
		return ERR_FAIL;
	}

	hErr = pal_ch_emu_LoadSetup();
	if (ERR_OK != hErr)
	{
		HxLOG_Error("pal_ch_emu_LoadSetup err:\n");
		return ERR_FAIL;
	}

	hErr = pal_ch_emu_LoadTsIndexFile();
	if (ERR_OK != hErr)
	{
		HxLOG_Error("pal_ch_emu_LoadTsIndexFile err:\n");
		return ERR_FAIL;
	}

	hErr = pal_ch_emu_InitThread();
	if (ERR_OK != hErr)
	{
		HxLOG_Error("pal_ch_emu_InitThread err:\n");
		return ERR_FAIL;
	}

	pstContext->pfnNotifier = (pfnDI_CH_NotifySignalInfo)pvCbFunc;
	return ERR_OK;
}

HBOOL PAL_CH_EMU_CheckIpTuneReady (void)
{
	INT_PAL_CH_EMU_Context_t	*pstContext = pal_ch_emu_GetContext();

	return (NULL != pstContext) ? pstContext->bSmbMounted : FALSE;
//	return FALSE;
}

HINT32 PAL_CH_EMU_GetFirstPlayerIdForFakeTuner (void)
{
	if (PAL_CH_EMU_CheckIpTuneReady() == TRUE)
	{
		return PAL_CH_EMU_USABLE_PBID_FIRST;
	}

	return -1;
}

HERROR PAL_CH_EMU_GetSatTunerCapacity (HUINT32 ulTunerGroupId, PalCh_SatTunerCapacity_t *pstSatTunerCap)
{
	INT_PAL_CH_EMU_Tuner_t		*pstTuner = pal_ch_emu_GetTuner(ulTunerGroupId);

	if (NULL == pstTuner)
	{
		HxLOG_Error("Wrong TunerId(%d) ? \n", ulTunerGroupId);
		return ERR_FAIL;
	}

	HxSTD_MemCopy(pstSatTunerCap, &(pstTuner->stSatTunerInfo), sizeof(PalCh_SatTunerCapacity_t));
	return ERR_OK;
}

HUINT32 PAL_CH_EMU_CountSatRfInput (void)
{
	INT_PAL_CH_EMU_Context_t	*pstContext = pal_ch_emu_GetContext();

	return pstContext->ulRfInputNum;
}

HERROR PAL_CH_EMU_GetSatRfInput (HUINT32 ulRfInputIdx, PalCh_SatRfInput_t *pstRfInput)
{
	PalCh_SatRfInput_t 			*pstSrcRfInput = pal_ch_emu_GetRfInput(ulRfInputIdx);

	if (NULL == pstSrcRfInput)
	{
		HxLOG_Error("Wrong RF Input ID(%d) ? \n", ulRfInputIdx);
		return ERR_FAIL;
	}

	HxSTD_MemCopy (pstRfInput, pstSrcRfInput, sizeof(PalCh_SatRfInput_t));
	return ERR_OK;
}

HERROR PAL_CH_EMU_SetSatRfInputConnection (HUINT32 ulTunerGroupId, HUINT32 ulRfInputId)
{
	INT_PAL_CH_EMU_Tuner_t		*pstTuner = pal_ch_emu_GetTuner(ulTunerGroupId);
	PalCh_SatRfInput_t 			*pstRfInput = pal_ch_emu_GetRfInput(ulRfInputId);

	if ((NULL == pstTuner) || (NULL == pstRfInput))
	{
		HxLOG_Error("TunerID(%d), RFINPUTID(%d) Wrong? \n", ulTunerGroupId, ulRfInputId);
		return ERR_FAIL;
	}

	if ((HINT32)ulRfInputId == pstTuner->nRfInputId)
	{
		return ERR_OK;
	}

	pstTuner->nRfInputId = (HINT32)ulRfInputId;
	(void)pal_ch_emu_checkRfInputConnectStatus();

	return ERR_OK;
}

HERROR PAL_CH_EMU_ResetSatRfInputConnection (HUINT32 ulTunerGroupId)
{
	INT_PAL_CH_EMU_Tuner_t		*pstTuner = pal_ch_emu_GetTuner(ulTunerGroupId);

	if (NULL == pstTuner)
	{
		HxLOG_Error("TunerID(%d) Wrong? \n", ulTunerGroupId);
		return ERR_FAIL;
	}

	if (-1 == pstTuner->nRfInputId)
	{
		return ERR_OK;
	}

	pstTuner->nRfInputId = -1;
	(void)pal_ch_emu_checkRfInputConnectStatus();

	return ERR_OK;
}

HERROR PAL_CH_EMU_StartTune(HINT32 nDiChType, HUINT32 ulTunerGroupId, HUINT32 ulRequestId, void *pvDiParam)
{
	HUINT32						 ulTpIndex = (HUINT32)-1;
	HUINT32						 ulDiMsg;
	INT_PAL_CH_EMU_TpInfo_t		*pstTpInfo = NULL;
	INT_PAL_CH_EMU_Tuner_t		*pstTuner = pal_ch_emu_GetTuner(ulTunerGroupId);
	DI_PVR_PLAY_SETUP_t			 stDiSetup;
	HERROR						 hErr, hResult = ERR_FAIL;

	if (PAL_CH_EMU_CheckIpTuneReady() != TRUE)
	{
		HxLOG_Error("pal_ch_emu_GetVirtualTpFromTuningParam err:\n");
		goto END_FUNC;
	}

	if (NULL == pstTuner)
	{
		HxLOG_Error("Tuner (%d)Error: wrong ID\n", ulTunerGroupId);
		goto END_FUNC;
	}

	if (eTUNER_EMU_STATE_Idle != pstTuner->eState)
	{
		hErr = PAL_CH_EMU_StopTune(nDiChType, ulTunerGroupId);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("Tuner (%d)Error: Not Idle state and cannot stop\n", ulTunerGroupId);
			goto END_FUNC;
		}
	}

	pstTpInfo = pal_ch_emu_GetVirtualTpFromTuningParam((DI_CH_Type_e)nDiChType, pvDiParam, &ulTpIndex);
	if (NULL == pstTpInfo)
	{
		HxLOG_Error("pal_ch_emu_GetVirtualTpFromTuningParam err:\n");
		goto END_FUNC;
	}

	HxSTD_memset(&stDiSetup, 0, sizeof(DI_PVR_PLAY_SETUP_t));

	stDiSetup.ulDemuxId = 0;
	stDiSetup.eFileType = eDI_PVR_FileType_Linear;
	stDiSetup.eStreamType = eDI_PVR_Stream_Video;
	HxSTD_memcpy (stDiSetup.aucFileName, pstTpInfo->szFileName, DI_PVR_MAX_NAME_LEN);

	if (pstTpInfo->nCurrTime > 0)
	{
		stDiSetup.eStartCondition = DI_PVR_PLAY_START_MODE_LASTVIEW;
		stDiSetup.ulStartTime = (HUINT32)pstTpInfo->nCurrTime;
	}
	else
	{
		stDiSetup.eStartCondition = DI_PVR_PLAY_START_MODE_BEGIN;
		stDiSetup.ulStartTime = 0;
	}

	stDiSetup.bIsRecording = FALSE;
	stDiSetup.stDecryptSettings.eEncType = DI_PVR_ENCRYPTION_TYPE_NONE;
	stDiSetup.ulPacketSize = PACKET_SIZE_TS;
	stDiSetup.bIgnoreIndexFile = TRUE;


	hErr = DI_PVR_PLAY_Setup(pstTuner->ulPbDevId, &stDiSetup);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("DI_PVR_PLAY_Setup err: PbDevId(%d)\n", pstTuner->ulPbDevId);
		goto END_FUNC;
	}

	hErr = DI_PVR_PLAY_Start(pstTuner->ulPbDevId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("DI_PVR_PLAY_Setup err: PbDevId(%d)\n", pstTuner->ulPbDevId);
		goto END_FUNC;
	}

	pstTuner->eState = eTUNER_EMU_STATE_Tuned;
	pstTuner->ulTpInfoIndex = ulTpIndex;
	hResult = ERR_OK;

END_FUNC:
	ulDiMsg = (ERR_OK == hResult) ? DI_CH_SIGNAL_LOCKED : DI_CH_SIGNAL_UNLOCK;

	(void)pal_ch_emu_SendCommand(eCH_EMU_CMD_NotifyTuneResult, ulTunerGroupId, ulRequestId, nDiChType, ulDiMsg);
	return hResult;
}

HERROR PAL_CH_EMU_StopTune(HINT32 nDiChType, HUINT32 ulTunerGroupId)
{
	INT_PAL_CH_EMU_Tuner_t		*pstTuner = pal_ch_emu_GetTuner(ulTunerGroupId);
	HERROR						 hErr;

	if (PAL_CH_EMU_CheckIpTuneReady() != TRUE)
	{
		HxLOG_Error("pal_ch_emu_GetVirtualTpFromTuningParam err:\n");
		return ERR_FAIL;
	}

	if (NULL == pstTuner)
	{
		HxLOG_Error("Tuner (%d)Error: wrong ID\n", ulTunerGroupId);
		return ERR_FAIL;
	}

	if (eTUNER_EMU_STATE_Idle == pstTuner->eState)
	{
		HxLOG_Error("Tuner (%d)Error: Already Idle state\n", ulTunerGroupId);
		return ERR_FAIL;
	}

	hErr = DI_PVR_PLAY_Stop(pstTuner->ulPbDevId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("DI_PVR_PLAY_Setup err: PbDevId(%d)\n", pstTuner->ulPbDevId);
		return ERR_FAIL;
	}

	pstTuner->eState = eTUNER_EMU_STATE_Idle;
	pstTuner->ulTpInfoIndex = (HUINT32)-1;
	return ERR_OK;
}

void PAL_CH_EMU_GetSupportDeliveryType (HUINT32 ulTunerGroupId, HUINT32 *pulSupportDeliveryType)
{
	INT_PAL_CH_EMU_Tuner_t		*pstTuner = pal_ch_emu_GetTuner(ulTunerGroupId);

	if ((NULL != pstTuner) && (NULL != pulSupportDeliveryType))
	{
		*pulSupportDeliveryType = pstTuner->eSupportDeliType;
	}
}

HUINT32 PAL_CH_EMU_GetNumOfTunerGroup (void)
{
	INT_PAL_CH_EMU_Context_t	*pstContext = pal_ch_emu_GetContext();

	return pstContext->ulTunerNum;
}

HERROR PAL_CH_EMU_GetLockedInfoSat (HUINT32 ulTunerGroupId, HUINT32 ulTpFreq, HUINT32 	ulLnbFreq, HUINT32 *pulActualTpFreq, DxSat_Polarization_e *pActualPolar, DxSat_CodeRate_e *pActualCodeRate, DxSat_PskModulation_e *pActualPskMod, DxSat_Pilot_e *pActualPilot)
{
	HUINT32						 ulRealLnbFreq;
	HUINT32						 ulPalFreq1, ulPalFreq2;
	HINT32						 nFreqDiff1, nFreqDiff2;
	INT_PAL_CH_EMU_Tuner_t		*pstTuner = pal_ch_emu_GetTuner(ulTunerGroupId);
	INT_PAL_CH_EMU_TpInfo_t		*pstTpInfo;

	if ((NULL == pstTuner) || (eTUNER_EMU_STATE_Tuned != pstTuner->eState))
	{
		return ERR_FAIL;
	}

	pstTpInfo = pal_ch_emu_GetTpInfo(pstTuner->ulTpInfoIndex);
	if (NULL == pstTpInfo)
	{
		HxLOG_Error("No EMU-TP INFO for index (%d)\n", pstTuner->ulTpInfoIndex);
		return ERR_FAIL;
	}

	/* 주의 : DB, LNB info, MW 의 주파수는 MHz 단위임. DDI는 kHz 단위임 */
	if (ulLnbFreq == CH_UNIVERSAL_LNB_FREQ)
	{
		ulRealLnbFreq = PAL_CH_LNB_GetActualLnbFreq(ulTpFreq);
	}
	else if(ulLnbFreq >= (HUINT32)CH_MDU4_LNB_FREQ && ulLnbFreq <= (HUINT32)CH_MDU1_LNB_FREQ)
	{
		ulRealLnbFreq = PAL_CH_LNB_GetActualMduLnbFreq(ulLnbFreq, pstTpInfo->unDiParam.stSat.etPolarization, ulTpFreq);
	}
	else
	{
		ulRealLnbFreq = ulLnbFreq;
	}

	ulPalFreq1 = ulRealLnbFreq + pstTpInfo->unDiParam.stSat.ulFrequency;
	ulPalFreq2 = ulRealLnbFreq - pstTpInfo->unDiParam.stSat.ulFrequency;
	nFreqDiff1 = (HUINT32)abs((HINT32)ulTpFreq - (HINT32)ulPalFreq1);
	nFreqDiff2 = (HUINT32)abs((HINT32)ulTpFreq - (HINT32)ulPalFreq2);

	*pulActualTpFreq = (nFreqDiff2 > nFreqDiff1) ? ulPalFreq1 : ulPalFreq2;

	*pActualPolar = pal_ch_sat_ConvDI2PAL_Polarization(pstTpInfo->unDiParam.stSat.etPolarization);
	*pActualCodeRate = pal_ch_sat_ConvDI2PAL_CodeRate(pstTpInfo->unDiParam.stSat.etFecCodeRate);
	*pActualPskMod = pal_ch_sat_ConvDI2PAL_Modulation(pstTpInfo->unDiParam.stSat.etModulation);
	*pActualPilot = pal_ch_sat_ConvDI2PAL_Pilot(pstTpInfo->unDiParam.stSat.etPilot);

	return ERR_OK;
}


