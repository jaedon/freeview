/**************************************************************
*
*	http://www.humaxdigital.com
*
*	@data			2012/10/24
*	@author			humax
**************************************************************/


/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ⓒ 2011-2012 Humax Co., Ltd.
* This program is produced by Humax Co., Ltd. ("Humax") and
* the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
* non-assignable, non-transferable and non-exclusive license to use this Software.
* You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
* you agree to the responsibility to take all reasonable efforts to protect the any information
* you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
* reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
* If you have no authorized license, discontinue using this Software immediately.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
* IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
* IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/


/**********************************************************************************************************/
#define ____INCLUDE_______________________
/**********************************************************************************************************/
#include "int_streaming.h"
#include "sapi.h"

/**********************************************************************************************************/
#define ____DEFINE________________________
/**********************************************************************************************************/
#define ENTER_GSEM							VK_SEM_Get (s_ulPLStreaming_SemId)
#define LEAVE_GSEM							VK_SEM_Release (s_ulPLStreaming_SemId)
#define ENTER_INST(pstInst)					VK_SEM_Get (pstInst->ulSemId)
#define LEAVE_INST(pstInst)					VK_SEM_Release (pstInst->ulSemId)

#define PL_STREAMING_INST_NUM					16
#define PL_USESTREAMTYPE_MAX_NUM				16


#define PL_STREAMING_HLS_LIVE_VIDEO_PREFIX		"HLSxxLV991"
#define PL_STREAMING_HLS_LIVE_AUDIO_PREFIX		"HLSxxLA991"
#define PL_STREAMING_HLS_FILE_MUSIC_PREFIX		"HLSxxFM991"
#define PL_STREAMING_HLS_FILE_VIDEO_PREFIX		"HLSxxFV991"
#define PL_STREAMING_HLS_FILE_IMAGE_PREFIX		"HLSxxFI991"
#define PL_STREAMING_HLS_FILE_PVR_PREFIX		"HLSxxFP991"
#define PL_STREAMING_HLS_FILE_PVR_AUDIO_PREFIX	"HLSxxFA991"
#define PL_STREAMING_HLS_FILE_TSR_PREFIX		"HLSxxFT991"


#define PL_STREAMING_SATIP_LIVE_PREFIX			"SATIPxL991"
#define PL_STREAMING_SATIP_LIVE_AUTOPID_PREFIX	"SATIPAL991"

#define PL_STREAMING_BYPASS_LIVE_PREFIX			"BYPAxxL991"
#define PL_STREAMING_BYPASS_LIVE_AUTOPID_PREFIX "BYPAxAL991"

#define PL_STREAMING_NTLS_LIVE_VIDEO_PREFIX		"NTLSxxLV991"
#define PL_STREAMING_NTLS_LIVE_AUDIO_PREFIX		"NTLSxxLA991"
#define PL_STREAMING_NTLS_FILE_MUSIC_PREFIX		"NTLSxxFM991"
#define PL_STREAMING_NTLS_FILE_VIDEO_PREFIX		"NTLSxxFV991"
#define PL_STREAMING_NTLS_FILE_IMAGE_PREFIX		"NTLSxxFI991"
#define PL_STREAMING_NTLS_FILE_PVR_PREFIX		"NTLSxxFP991"
#define PL_STREAMING_NTLS_FILE_PVR_AUDIO_PREFIX	"NTLSxxFA991"
#define PL_STREAMING_NTLS_FILE_TSR_PREFIX		"NTLSxxFT991"

/**********************************************************************************************************/
#define ____TYPEDEF________________________
/**********************************************************************************************************/
/* 2014/6/17 mhkang: moves to int_pl_streaming_context.h
typedef struct	_plStreamingInst_t
{
	HULONG					ulSemId;
	HBOOL					bExecuting;

	Handle_t				hStreamingId;
	DxRsvStreamType_e		eStreamType;
	PLStreaming_ModeInst_t	*pstModeInst;
} plStreamingInst_t;
*/
typedef struct	_plUseStreamType_t
{
	DxRsvStreamType_e	eStreamType;
	HBOOL				bIsUse;
} plUseStreamType_t;

/**********************************************************************************************************/
#define ____INTERNAL_STRUCTURES_AND_VARIABLES_______________________
/**********************************************************************************************************/
STATIC unsigned long		 s_ulPLStreaming_SemId = 0;
STATIC plStreamingInst_t	 s_astStreaming_Instance[PL_STREAMING_INST_NUM];
STATIC plUseStreamType_t	 s_astUseStreamType[PL_USESTREAMTYPE_MAX_NUM];
STATIC cbPLStreamingStatus 	 s_cbPLStreamingStatus_ToDLNALib = NULL;


/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/
STATIC plStreamingInst_t *_plstreaming_Context_getEmptyInstance (void)
{
	HUINT32				 ulIndex;
	HERROR				 hErr;

	for (ulIndex = 0; ulIndex < PL_STREAMING_INST_NUM; ulIndex++)
	{
		if (TRUE != s_astStreaming_Instance[ulIndex].bExecuting)
		{
			if (0 == s_astStreaming_Instance[ulIndex].ulSemId)
			{
				unsigned long			 ulSemId;
				HCHAR					 szSemName[16];

				HxSTD_PrintToStrN (szSemName, 16, "Streaming%02dSem", ulIndex);
				hErr = VK_SEM_Create (&ulSemId, szSemName, VK_SUSPENDTYPE_PRIORITY);
				if (VK_OK != hErr)
				{
					HxLOG_Error ("Semaphore failed!!!\n");
					return NULL;
				}

				s_astStreaming_Instance[ulIndex].ulSemId = ulSemId;
			}

			return &(s_astStreaming_Instance[ulIndex]);
		}
	}

	return NULL;
}


STATIC plStreamingInst_t *_plstreaming_Context_getInstanceByIndex (HUINT32 ulInstanceIndex)
{
	if(ulInstanceIndex < PL_STREAMING_INST_NUM)
	{
		return &(s_astStreaming_Instance[ulInstanceIndex]);
	}

	return NULL;
}

STATIC plStreamingInst_t *_plstreaming_Context_getInstance (const Handle_t hStreamingId)
{
	HUINT32				 ulIndex;

	for (ulIndex = 0; ulIndex < PL_STREAMING_INST_NUM; ulIndex++)
	{
		if ((TRUE == s_astStreaming_Instance[ulIndex].bExecuting) && (hStreamingId == s_astStreaming_Instance[ulIndex].hStreamingId))
		{
			return &(s_astStreaming_Instance[ulIndex]);
		}
	}

	return NULL;
}

STATIC plStreamingInst_t *_plstreaming_Context_findInstanceByStreamingKey (const PL_StreamingKey_t *pstStreamingKey)
{
	HUINT32				 ulIndex;

	for (ulIndex = 0; ulIndex < PL_STREAMING_INST_NUM; ulIndex++)
	{
		if(NULL == s_astStreaming_Instance[ulIndex].pstModeInst)
		{
			continue;
		}

		if(PL_StreamingKey_CompareWithoutProfile(pstStreamingKey, s_astStreaming_Instance[ulIndex].pstModeInst->pstStreamingKey) == 0)
		{
			return &(s_astStreaming_Instance[ulIndex]);
		}
	}

	return NULL;
}

/**
	@brief 동일한 streamid를 갖는  plStreamingInst_t 인스턴스를 반환합니다.
	@param isOwner	SAT>IP Owner(true/false)
	@param nStreamId SAT>IP streamid
	@return 성공하면 plStreamingInst_t 구조체 포인터를 반환한다.
*/
void *PL_Streaming_Context_FindStreamingInstanceByStreamId (HBOOL isOwner, const HINT32 nStreamId)
{
	HUINT32				 ulIndex;


	for (ulIndex = 0; ulIndex < PL_STREAMING_INST_NUM; ulIndex++)
	{
		// 2014/6/13 mhkang: streamid 필드는 SAT>IP에서만 사용한다.
		if (s_astStreaming_Instance[ulIndex].eStreamType != DxRSVSTREAM_TYPE_SATIP_LIVE)
		{
			continue;
		}

		if (NULL == s_astStreaming_Instance[ulIndex].pstModeInst)
		{
			continue;
		}

		if ( isOwner == s_astStreaming_Instance[ulIndex].pstModeInst->Mode.stSatIp.bIsOwner  &&
			 nStreamId == s_astStreaming_Instance[ulIndex].pstModeInst->Mode.stSatIp.nStreamId)
		{
			return &(s_astStreaming_Instance[ulIndex]);
		}
	}

	return NULL;
}

STATIC plStreamingInst_t *_plstreaming_Context_findInstanceByDeviceID (const HCHAR *pszDeviceID)
{
	HUINT32				 ulIndex;

	for (ulIndex = 0; ulIndex < PL_STREAMING_INST_NUM; ulIndex++)
	{
		if(NULL == s_astStreaming_Instance[ulIndex].pstModeInst)
		{
			continue;
		}

		if(HxSTD_StrCmp(pszDeviceID, s_astStreaming_Instance[ulIndex].pstModeInst->stSetupInfo.pszDeviceID) == 0)
		{
			return &(s_astStreaming_Instance[ulIndex]);
		}
	}

	return NULL;
}

STATIC HBOOL _plstreaming_Context_isUseStreamType(const DxRsvStreamType_e	eStreamType)
{
	HUINT32				 ulIndex;

	for (ulIndex = 0; ulIndex < PL_USESTREAMTYPE_MAX_NUM; ulIndex++)
	{
		if(eStreamType == s_astUseStreamType[ulIndex].eStreamType)
		{
			return s_astUseStreamType[ulIndex].bIsUse;
		}
	}
	return FALSE;
}

STATIC void _plstreaming_Context_InitUseStreamType(DxRsvStreamType_e	eStreamType, HBOOL bIsUse)
{
	HUINT32				 ulIndex;

	for (ulIndex = 0; ulIndex < PL_USESTREAMTYPE_MAX_NUM; ulIndex++)
	{
		if(eStreamType == s_astUseStreamType[ulIndex].eStreamType)
		{
			s_astUseStreamType[ulIndex].bIsUse = bIsUse;
			break;
		}

		if(DxRSVSTREAM_TYPE_MAX == s_astUseStreamType[ulIndex].eStreamType)
		{
			s_astUseStreamType[ulIndex].eStreamType = eStreamType;
			s_astUseStreamType[ulIndex].bIsUse		= bIsUse;
			break;
		}
	}
}


STATIC void _plstreaming_Context_CopySetupInfo (PLStreaming_SetupInfo_t	*pDst, const PLStreaming_SetupInfo_t *pSrc)
{
	HxSTD_MemCopy(pDst, pSrc, sizeof(PLStreaming_SetupInfo_t));
	pDst->pszIPAddress		= (NULL == pSrc->pszIPAddress)		? NULL : HLIB_STD_StrDup(pSrc->pszIPAddress);
	pDst->pszDeviceID		= (NULL == pSrc->pszDeviceID)		? NULL : HLIB_STD_StrDup(pSrc->pszDeviceID);
	pDst->pszDisplayString	= (NULL == pSrc->pszDisplayString)	? NULL : HLIB_STD_StrDup(pSrc->pszDisplayString);
	pDst->pszStreamingPath	= (NULL == pSrc->pszStreamingPath)	? NULL : HLIB_STD_StrDup(pSrc->pszStreamingPath);
	pDst->pszDeviceName		= (NULL == pSrc->pszDeviceName)		? NULL : HLIB_STD_StrDup(pSrc->pszDeviceName);
}

STATIC HBOOL	_plstreaming_Context_IsIncludeVideo (DxRsvStreamType_e eStreamType)
{
	switch(eStreamType)
	{
	case DxRSVSTREAM_TYPE_HLS_LIVE_AUDIO:
	case DxRSVSTREAM_TYPE_HLS_FILE_MUSIC:
		return FALSE;

	default:
		break;
	}

	return TRUE;
}

STATIC HCHAR *_plstreaming_Context_MakeDisplayString(const PLStreaming_StartInfo_t *pstStartInfo, const HCHAR *pszDefaultStr)
{
	if(NULL != pstStartInfo->pszUserAgent)
	{
		return HLIB_STD_StrDup(pstStartInfo->pszUserAgent);
	}

	if(NULL != pstStartInfo->pszIPAddress)
	{
		return HLIB_STD_StrDup(pstStartInfo->pszIPAddress);
	}

	if(NULL != pstStartInfo->pszDeviceID)
	{
		return HLIB_STD_StrDup(pstStartInfo->pszDeviceID);
	}

	return HLIB_STD_StrDup(pszDefaultStr);
}

STATIC PLStreaming_SetupInfo_t *_plstreaming_Context_CreateSetupInfo (const HCHAR *pszPrefix, const PLStreaming_StartInfo_t *pstStartInfo, PLStreaming_SetupInfo_t *pstSetupInfo)
{
	HINT32						nChunkMultiply = 0;
	HBOOL						bIsIncludeVideo = FALSE;
	PL_MediaProfile_t			*pstMediaProfile = NULL;
	HOM_Result_e 				eHomResult = eHOM_Result_Ok;

	/* not used resource */
	if(NULL == pstStartInfo->pszStreamingPath)
	{
		HxSTD_memset(pstSetupInfo, 0, sizeof(PLStreaming_SetupInfo_t));
		pstSetupInfo->eStreamType		= pstStartInfo->eStreamType;
		pstSetupInfo->bNotUseResource	= TRUE;
		return pstSetupInfo;
	}

	if (pstStartInfo->eStreamType & DxRSVSTREAM_TYPE_HLS_MASK) // Media Profile is used only for HLS.
	{
		/* use resource */
		eHomResult = PL_MediaProfile_Open(pstStartInfo->pszDeviceName, pstStartInfo->pszUserAgent, &pstMediaProfile);
		if(eHOM_Result_Ok != eHomResult)
		{
			HxLOG_Error("Profile Open Error!! pszDeviceName(%s) pszUserAgent(%s)\r\n", pstStartInfo->pszDeviceName, pstStartInfo->pszUserAgent);
			return NULL;
		}

		bIsIncludeVideo = _plstreaming_Context_IsIncludeVideo(pstSetupInfo->eStreamType);
		if(TRUE == bIsIncludeVideo)
		{
			eHomResult = PL_MediaProfile_GetVideoChunkMultiply(pstMediaProfile, &nChunkMultiply);
		}
		else
		{
			eHomResult = PL_MediaProfile_GetAudioChunkMultiply(pstMediaProfile, &nChunkMultiply);
		}
		if(eHOM_Result_Ok != eHomResult)
		{
			HxLOG_Error("Get ChunkMultiply Error!! bIsIncludeVideo(%d) pszDeviceName(%s) pszUserAgent(%s)\r\n", (HINT32)bIsIncludeVideo, pstStartInfo->pszDeviceName, pstStartInfo->pszUserAgent);
			return NULL;
		}
	}

	HxSTD_memset(pstSetupInfo, 0, sizeof(PLStreaming_SetupInfo_t));
	pstSetupInfo->eStreamType		= pstStartInfo->eStreamType;
	pstSetupInfo->nChunkMultiply	= nChunkMultiply;
	pstSetupInfo->pstMediaProfile	= pstMediaProfile;
	pstSetupInfo->pszIPAddress		= (NULL == pstStartInfo->pszIPAddress) 		? NULL : HLIB_STD_StrDup(pstStartInfo->pszIPAddress);
	pstSetupInfo->pszDeviceID		= (NULL == pstStartInfo->pszDeviceID) 		? NULL : HLIB_STD_StrDup(pstStartInfo->pszDeviceID);
	pstSetupInfo->pszStreamingPath	= (NULL == pstStartInfo->pszStreamingPath) 	? NULL : HLIB_STD_StrDup(pstStartInfo->pszStreamingPath);
	pstSetupInfo->eOutType			= pstStartInfo->eOutType;
	pstSetupInfo->bIsIncludeVideo	= bIsIncludeVideo;
	pstSetupInfo->bNotUseResource	= FALSE;
	pstSetupInfo->pszDeviceName     = (NULL == pstStartInfo->pszDeviceName) 		? NULL : HLIB_STD_StrDup(pstStartInfo->pszDeviceName);
	pstSetupInfo->serverfd          = pstStartInfo->serverfd;
	pstSetupInfo->clientfd          = pstStartInfo->clientfd;

	if(NULL == pstSetupInfo->pszDisplayString)
	{
		pstSetupInfo->pszDisplayString	= _plstreaming_Context_MakeDisplayString(pstStartInfo, pszPrefix);
	}

	/* prefix 는 define이다. memory 아님! */
	pstSetupInfo->pszPrefix			= pszPrefix;

	if(HLIB_STD_StrStartWith(pstStartInfo->pszUserAgent,"Apple"))
	{
		pstSetupInfo->etClientType = ePLStreamingClientType_Ios;
	}
	else
	{
		pstSetupInfo->etClientType = ePLStreamingClientType_Unknown;
	}

	return pstSetupInfo;
}

STATIC void _plstreaming_Context_DeleteSetupInfo (PLStreaming_SetupInfo_t	*pstSetupInfo)
{
	ENTER_FUNCTION;

	PL_MediaProfile_Close(pstSetupInfo->pstMediaProfile);

	if(pstSetupInfo->pszIPAddress)		HLIB_STD_MemFree(pstSetupInfo->pszIPAddress);
	if(pstSetupInfo->pszDeviceID)		HLIB_STD_MemFree(pstSetupInfo->pszDeviceID);
	if(pstSetupInfo->pszStreamingPath)	HLIB_STD_MemFree(pstSetupInfo->pszStreamingPath);
	if(pstSetupInfo->pszDisplayString)	HLIB_STD_MemFree(pstSetupInfo->pszDisplayString);

	if(pstSetupInfo->pszDeviceName)		HLIB_STD_MemFree(pstSetupInfo->pszDeviceName);

	HxSTD_memset(pstSetupInfo, 0, sizeof(PLStreaming_SetupInfo_t));

	LEAVE_FUNCTION;
}

STATIC HERROR _plstreaming_Context_clearInstance (plStreamingInst_t *pstInst)
{
	unsigned long			 ulSemId = pstInst->ulSemId;

	HxLOG_Info("hStreamingId:0x%x\n", pstInst->hStreamingId);
	// 반드시 지우면 안되는 Semaphore ID 외에는 다 지운다.

	VK_memset (pstInst, 0, sizeof(plStreamingInst_t));
	pstInst->ulSemId = ulSemId;
	return ERR_OK;
}


STATIC void _plstreaming_Context_deleteStreamingInstance (PLStreaming_ModeInst_t	*pstModeInst)
{
	PLStreaming_SetupInfo_t		*pstSetupInfo = NULL;

	if (pstModeInst == NULL)
	{
		HxLOG_Error("invalid args(pstModeInst is null)\n");
		return;
	}

	pstSetupInfo = &pstModeInst->stSetupInfo;
	PL_Streaming_Resource_Delete(pstModeInst->hResHandle, TRUE);
	VK_TASK_Sleep(10);

	switch((pstSetupInfo->eStreamType & DxRSVSTREAM_TYPE_MASK))
	{
	case DxRSVSTREAM_TYPE_HLS_MASK:
		PL_Streaming_ModeHLS_DeleteInstance(pstModeInst);
		break;

	case DxRSVSTREAM_TYPE_SATIP_MASK:
		PL_Streaming_ModeSatIp_DeleteInstance(pstModeInst);
		break;

	case DxRSVSTREAM_TYPE_NTLS_MASK:
		PL_Streaming_ModeByPass_DeleteInstance(pstModeInst);
		break;

	default:
		HxLOG_Error("new Streaming error!! eStreamType(0x%x)\r\n", pstSetupInfo->eStreamType);
		break;
	}

	_plstreaming_Context_DeleteSetupInfo(pstSetupInfo);

	if (pstModeInst->tunerInfo != NULL)
	{
		HLIB_STD_MemFree(pstModeInst->tunerInfo);
		pstModeInst->tunerInfo = NULL;
	}
	HLIB_STD_MemFree(pstModeInst);
}

STATIC void _plstreaming_Context_deleteStreaming (plStreamingInst_t	*pstInst)
{
	if (NULL == pstInst)
	{
		HxLOG_Error("Invalid args(pstInst is null)\ns");
		return;
	}

	if (pstInst->bExecuting == FALSE)
	{
		HxLOG_Warning("Streaming already ended\n");
		return;
	}

	if(NULL != pstInst->pstModeInst)
	{
		_plstreaming_Context_deleteStreamingInstance(pstInst->pstModeInst);
		pstInst->pstModeInst = NULL;
	}
	_plstreaming_Context_clearInstance (pstInst);

}


STATIC PLStreaming_ModeInst_t *_plstreaming_Context_newStreamingInstance (PLStreaming_SetupInfo_t *pstSetupInfo, Handle_t hStreamingId)
{
	PLStreaming_ModeInst_t *pstModeInst = NULL;
	PLStreaming_ModeInst_t *pstRetInst = NULL;

	pstModeInst = HLIB_STD_MemCalloc(sizeof(PLStreaming_ModeInst_t));
	if(NULL == pstModeInst)
	{
		HxLOG_Error("Error> Out of Memory!!\r\n");
		return NULL;
	}

	HxSTD_MemSet(pstModeInst, 0, sizeof(PLStreaming_ModeInst_t));
	_plstreaming_Context_CopySetupInfo(&pstModeInst->stSetupInfo, pstSetupInfo);
	pstModeInst->hStreamingId	= hStreamingId;
	pstModeInst->hResHandle 	= PL_Streaming_Resource_New(hStreamingId);
	pstModeInst->tunerid		= -1;

	if(HANDLE_NULL == pstModeInst->hResHandle)
	{
		HxLOG_Error("pstModeInst->hResHandle = 0x%x, hStreamingId = 0x%x\n", pstModeInst->hResHandle, hStreamingId);
	}
	else
	{
		HxLOG_Print(HxANSI_COLOR_YELLOW("pstModeInst->hResHandle = 0x%x, hStreamingId = 0x%x\n"), pstModeInst->hResHandle, hStreamingId);
	}

	switch((pstSetupInfo->eStreamType & DxRSVSTREAM_TYPE_MASK))
	{
	case DxRSVSTREAM_TYPE_HLS_MASK:
		pstRetInst =  PL_Streaming_ModeHLS_SetupInstance(pstModeInst);
		break;

	case DxRSVSTREAM_TYPE_SATIP_MASK:
		pstRetInst = PL_Streaming_ModeSatIp_SetupInstance(pstModeInst);
		break;

	case DxRSVSTREAM_TYPE_NTLS_MASK:
		pstRetInst =  PL_Streaming_ModeByPass_SetupInstance(pstModeInst);
		break;

	default:
		HxLOG_Error("Error> new Streaming error!! eStreamType(0x%x)\r\n",pstSetupInfo->eStreamType);
		break;
	}

	if(NULL == pstRetInst)
	{
		HxLOG_Error ("Error> pstRetInst return fail !!!!!!\n");
		_plstreaming_Context_deleteStreamingInstance(pstModeInst);
		pstModeInst = NULL;
		return NULL;
	}

	return pstRetInst;
}

STATIC Handle_t _plstreaming_Context_newStreaming (PLStreaming_SetupInfo_t *pstSetupInfo)
{
	STATIC Handle_t			 s_hLastStreaming = 0x00000991;
	Handle_t				 hStreamingId = (s_hLastStreaming < 0xFFFFFFF0) ? s_hLastStreaming + 1 : 0x00000991;
	plStreamingInst_t		*pstInst;

	if (NULL == pstSetupInfo)
	{
		HxLOG_Error ("Setup Value wrong\n");
		return HANDLE_NULL;
	}

	while (1)
	{
		if (NULL == _plstreaming_Context_getInstance (hStreamingId))
		{
			break;
		}

		hStreamingId = (hStreamingId < 0xFFFFFFF0) ? hStreamingId + 1 : 0x00000991;
		if (s_hLastStreaming == hStreamingId)
		{
			HxLOG_Error ("Something Wrong !!!!!!\n");
			return HANDLE_NULL;
		}
	}

	pstInst = _plstreaming_Context_getEmptyInstance();
	if (NULL == pstInst)
	{
		HxLOG_Error ("No empty instance...\n");
		return HANDLE_NULL;
	}

	pstInst->pstModeInst = _plstreaming_Context_newStreamingInstance(pstSetupInfo, hStreamingId);
	if (NULL == pstInst->pstModeInst)
	{
		HxLOG_Error ("Setup failed, Type(%d)\n", pstInst->eStreamType);
		return HANDLE_NULL;
	}

	pstInst->bExecuting		= TRUE;
	pstInst->hStreamingId	= hStreamingId;
	pstInst->eStreamType	= pstSetupInfo->eStreamType;

	s_hLastStreaming = hStreamingId;

	PL_Streaming_Debug_Print("(-) hStreaming(0x%x) hRingbuf(0x%x)\r\n", hStreamingId, pstInst->pstModeInst->hRingbuf);

	return hStreamingId;

}


STATIC HCHAR *_plstreaming_Context_GetPrefixStringByStreamType (DxRsvStreamType_e eStreamType)
{
	switch(eStreamType)
	{
	case DxRSVSTREAM_TYPE_HLS_LIVE_VIDEO: 		return PL_STREAMING_HLS_LIVE_VIDEO_PREFIX;
	case DxRSVSTREAM_TYPE_HLS_LIVE_AUDIO:		return PL_STREAMING_HLS_LIVE_AUDIO_PREFIX;
	case DxRSVSTREAM_TYPE_HLS_FILE_MEDIA:		return PL_STREAMING_HLS_FILE_VIDEO_PREFIX;
	case DxRSVSTREAM_TYPE_HLS_FILE_TSR:			return PL_STREAMING_HLS_FILE_TSR_PREFIX;
	case DxRSVSTREAM_TYPE_HLS_FILE_PVR:			return PL_STREAMING_HLS_FILE_PVR_PREFIX;
	case DxRSVSTREAM_TYPE_HLS_FILE_PVR_AUDIO:	return PL_STREAMING_HLS_FILE_PVR_AUDIO_PREFIX;
	case DxRSVSTREAM_TYPE_HLS_FILE_MUSIC:		return PL_STREAMING_HLS_FILE_MUSIC_PREFIX;
	case DxRSVSTREAM_TYPE_SATIP_LIVE:			return PL_STREAMING_SATIP_LIVE_PREFIX;
	case DxRSVSTREAM_TYPE_SATIP_LIVE_AUTOPID:	return PL_STREAMING_SATIP_LIVE_AUTOPID_PREFIX;
	case DxRSVSTREAM_TYPE_NTLS_LIVE_VIDEO: 		return PL_STREAMING_NTLS_LIVE_VIDEO_PREFIX;
	case DxRSVSTREAM_TYPE_NTLS_LIVE_AUDIO:		return PL_STREAMING_NTLS_LIVE_AUDIO_PREFIX;
	case DxRSVSTREAM_TYPE_NTLS_FILE_MEDIA:		return PL_STREAMING_NTLS_FILE_VIDEO_PREFIX;
	case DxRSVSTREAM_TYPE_NTLS_FILE_TSR:		return PL_STREAMING_NTLS_FILE_TSR_PREFIX;
	case DxRSVSTREAM_TYPE_NTLS_FILE_PVR:		return PL_STREAMING_NTLS_FILE_PVR_PREFIX;
	case DxRSVSTREAM_TYPE_NTLS_FILE_PVR_AUDIO:	return PL_STREAMING_NTLS_FILE_PVR_AUDIO_PREFIX;
	case DxRSVSTREAM_TYPE_NTLS_FILE_MUSIC:		return PL_STREAMING_NTLS_FILE_MUSIC_PREFIX;
	default:
		HxLOG_Error("new Streaming error!! eStreamType(0x%x)\r\n", eStreamType);
		break;
	}

	return NULL;
}


STATIC HOM_Result_e _plstreaming_Context_BufToFile (DxRsvStreamType_e eStreamType, const HCHAR *pszFileName, HUINT32 ulDataSize, HCHAR *pucData)
{
	HCHAR			*pszName = NULL;
	HxFILE_t		fp = NULL;
	HOM_Result_e	eHomResult = eHOM_Result_Ok;
	const HCHAR		*pFilePath = NULL;
	HINT32			nPathSize = 0;

	if(NULL == pucData || ulDataSize == 0)
	{
		HxLOG_Info("Unknown Data\r\n");
		return eHOM_Result_Error;
	}


	eHomResult = eHOM_Result_Error;
	if(NULL != pszFileName)
	{
		pFilePath = pszFileName;
	}
	else
	{
		pFilePath = _plstreaming_Context_GetPrefixStringByStreamType(eStreamType);
	}

	nPathSize = HxSTD_StrLen(pFilePath) + 1;
	pszName = (HCHAR *)HLIB_STD_MemAlloc(nPathSize);

	HLIB_STD_StrReplaceChar(pszName, pFilePath, ':', '=');
	fp = HLIB_FILE_Open(pszName, "wb");
	if(fp)
	{
		HLIB_FILE_Write(fp, pucData, ulDataSize, 1);
		HLIB_FILE_Close(fp);
		eHomResult = eHOM_Result_Ok;
	}

	HLIB_STD_MemFree(pszName);

	return eHomResult;
}

/**********************************************************************************************************/
#define ____PUBLIC_FUNCTIONS_______________________
/**********************************************************************************************************/



/**
 *
 *
 *
 * @param
 * @return
 */
HOM_Result_e PL_Streaming_Context_Init(void)
{
	HERROR		 	hErr;
	HOM_Result_e	eHomResult = eHOM_Result_Ok;
	HUINT32			ulIndex;

	ENTER_FUNCTION;

	if(s_ulPLStreaming_SemId != 0)
	{
		HxLOG_Print("Error> sema init!\r\n");
		return eHOM_Result_Ok;
	}

	hErr = VK_SEM_Create (&s_ulPLStreaming_SemId, "PLStreamingSem", VK_SUSPENDTYPE_PRIORITY);
	if (VK_OK != hErr)
	{
		HxLOG_Error ("Error> Semaphore failed!!!\n");
		return eHOM_Result_Error;
	}

	HxSTD_memset(s_astStreaming_Instance, 0, sizeof(plStreamingInst_t) * PL_STREAMING_INST_NUM);

	HxSTD_memset(s_astUseStreamType, 0, sizeof(plUseStreamType_t) * PL_USESTREAMTYPE_MAX_NUM);
	for (ulIndex = 0; ulIndex < PL_USESTREAMTYPE_MAX_NUM; ulIndex++)
	{
		s_astUseStreamType[ulIndex].bIsUse 		= FALSE;
		s_astUseStreamType[ulIndex].eStreamType	= DxRSVSTREAM_TYPE_MAX;
	}

	PL_Streaming_Resource_SetConflictCallback((void *)PL_Streaming_Context_ResConflict);

	{ // hls
		eHomResult = PL_Streaming_ModeHLS_Init();
		if(eHOM_Result_Ok != eHomResult)
		{
			HxLOG_Error ("Error> PL_Streaming_ModeHLS_Init failed!!!\n");
			return eHOM_Result_Error;
		}
		_plstreaming_Context_InitUseStreamType(DxRSVSTREAM_TYPE_HLS_MASK, TRUE);
	}

	{ // sat ip
		eHomResult = PL_Streaming_ModeSatIp_Init();
		if(eHOM_Result_Ok != eHomResult)
		{
			HxLOG_Error ("Error> PL_Streaming_ModeSatIp_Init failed!!!\n");
			return eHOM_Result_Error;
		}
		_plstreaming_Context_InitUseStreamType(DxRSVSTREAM_TYPE_SATIP_MASK, TRUE);
	}

	{ // by pass
		eHomResult = PL_Streaming_ModeByPass_Init();
		if(eHOM_Result_Ok != eHomResult)
		{
			HxLOG_Error ("Error> PL_Streaming_ModeByPass_Init failed!!!\n");
			return eHOM_Result_Error;
		}
		_plstreaming_Context_InitUseStreamType(DxRSVSTREAM_TYPE_NTLS_MASK, TRUE);
	}

	LEAVE_FUNCTION;

	return eHOM_Result_Ok;
}


/**
 *
 *
 *
 * @param
 * @return
 */
void 	PL_Streaming_Context_DeInit(void)
{
	ENTER_FUNCTION;

	PL_Streaming_ModeHLS_DeInit();
	PL_Streaming_ModeSatIp_DeInit();
	PL_Streaming_ModeByPass_DeInit();

	VK_SEM_Destroy(s_ulPLStreaming_SemId);
	s_ulPLStreaming_SemId = 0;

	HxSTD_memset(s_astStreaming_Instance, 0, sizeof(plStreamingInst_t) * PL_STREAMING_INST_NUM);
	HxSTD_memset(s_astUseStreamType, 0, sizeof(plUseStreamType_t) * PL_USESTREAMTYPE_MAX_NUM);

	LEAVE_FUNCTION;

	return ;
}


/**
 *
 *
 *
 * @param
 * @return
 */
Handle_t PL_Streaming_Context_New (PLStreaming_SetupInfo_t *pstSetupInfo)
{
	Handle_t			hStreamingId = HANDLE_NULL;

	ENTER_FUNCTION;

	if(NULL == pstSetupInfo)
	{
		HxLOG_Error("Error> Invalid Argument!\r\n");
		return HANDLE_NULL;
	}

	ENTER_GSEM;
	hStreamingId = _plstreaming_Context_newStreaming (pstSetupInfo);
	LEAVE_GSEM;



	LEAVE_FUNCTION;

	return hStreamingId;
}


/**
 *
 *
 *
 * @param
 * @return
 */
void PL_Streaming_Context_Delete (const Handle_t hStreamingId)
{
	plStreamingInst_t			*pstInst;

	HxLOG_Info("hStreamingId:0x%x (+)\n", hStreamingId);

	ENTER_GSEM;
	pstInst = _plstreaming_Context_getInstance (hStreamingId);
	if (NULL != pstInst)
	{
		ENTER_INST (pstInst);
		_plstreaming_Context_deleteStreaming (pstInst);
		LEAVE_INST (pstInst);
	}
	LEAVE_GSEM;

	LEAVE_FUNCTION;
}



/**
 *
 *
 *
 * @param
 * @return
 */
HOM_Result_e PL_Streaming_Context_GetM3U8 (const Handle_t hStreamingId, const HCHAR *pRequestPath, HUINT32 *pulDataSize, HUINT8 **ppucData)
{
	plStreamingInst_t		*pstInst = NULL;
	HOM_Result_e 			eHomResult = eHOM_Result_Ok;

	ENTER_FUNCTION;

	ENTER_GSEM;
	pstInst = _plstreaming_Context_getInstance (hStreamingId);
	LEAVE_GSEM;

	if (NULL != pstInst)
	{
		ENTER_INST (pstInst);

		switch((pstInst->eStreamType & DxRSVSTREAM_TYPE_MASK))
		{
		case DxRSVSTREAM_TYPE_HLS_MASK:
			eHomResult = PL_Streaming_ModeHLS_GetM3U8(pstInst->pstModeInst, pRequestPath, pulDataSize, ppucData);
			if(NULL != pstInst->pstModeInst)
			{
				PL_Streaming_Debug_Print("(-) hStreaming(0x%x) hRingbuf(0x%x)\r\n", hStreamingId, pstInst->pstModeInst->hRingbuf);
				PL_Streaming_Resource_UpdateTime(pstInst->pstModeInst->hResHandle,PL_STREAMING_SCHEDULE_HLS_DURATION, 10);
			}
			break;

		case DxRSVSTREAM_TYPE_SATIP_MASK:
			eHomResult = PL_Streaming_ModeSatIp_GetM3U8(pstInst->pstModeInst, pRequestPath, pulDataSize, ppucData);
			// 2014/6/26 mhkang: SAT>IP에서는 SAMA 스케쥴 업데이트를 Background로 하도록 수정함.
			break;

		case DxRSVSTREAM_TYPE_NTLS_MASK:
			eHomResult = PL_Streaming_ModeByPass_GetM3U8(pstInst->pstModeInst, pRequestPath, pulDataSize, ppucData);
			if(NULL != pstInst->pstModeInst)
			{
				PL_Streaming_Debug_Print("(-) hStreaming(0x%x) hRingbuf(0x%x)\r\n", hStreamingId, pstInst->pstModeInst->hRingbuf);
				PL_Streaming_Resource_UpdateTime(pstInst->pstModeInst->hResHandle, PL_STREAMING_SCHEDULE_NTLS_DURATION, 10);
			}
			break;

		default:
			eHomResult = eHOM_Result_Error;
			HxLOG_Error("new Streaming error!! eStreamType(0x%x)\r\n",pstInst->eStreamType);
			break;
		}

		if(eHOM_Result_Ok == eHomResult)
		{
			if((NULL != pstInst->pstModeInst) && (ePLStreaming_OutType_DumpFile & pstInst->pstModeInst->stSetupInfo.eOutType))
			{
				(void)_plstreaming_Context_BufToFile(pstInst->eStreamType, pRequestPath, *pulDataSize, *ppucData);
			}
		}

		LEAVE_INST (pstInst);
	}

	if( eHOM_Result_Ok != eHomResult|| NULL == pstInst)
	{
		HxLOG_Error("Error> eHomResult(%s) or pstInst(%d) or hStreamingId(0x%x) error\r\n",
				PL_Streaming_Debug_GetHomResultStr(eHomResult), pstInst, hStreamingId);
	}

	LEAVE_FUNCTION;

	return eHomResult;
}

/**
 *
 *
 *
 * @param
 * @return
 */
HOM_Result_e PL_Streaming_Context_ReleaseM3U8 (const Handle_t hStreamingId, const HCHAR *pRequestPath, HUINT8 *pucData)
{
	plStreamingInst_t		*pstInst = NULL;
	HOM_Result_e 			eHomResult = eHOM_Result_Ok;

	ENTER_FUNCTION;

	ENTER_GSEM;
	pstInst = _plstreaming_Context_getInstance (hStreamingId);
	LEAVE_GSEM;

	if (NULL != pstInst)
	{
		ENTER_INST (pstInst);

		switch((pstInst->eStreamType & DxRSVSTREAM_TYPE_MASK))
		{
		case DxRSVSTREAM_TYPE_HLS_MASK:
			eHomResult = PL_Streaming_ModeHLS_ReleaseM3U8(pstInst->pstModeInst, pRequestPath, pucData);
			break;

		case DxRSVSTREAM_TYPE_SATIP_MASK:
			eHomResult = PL_Streaming_ModeSatIp_ReleaseM3U8(pstInst->pstModeInst, pRequestPath, pucData);
			break;

		case DxRSVSTREAM_TYPE_NTLS_MASK:
			eHomResult = PL_Streaming_ModeByPass_ReleaseM3U8(pstInst->pstModeInst, pRequestPath, pucData);
			break;

		default:
			eHomResult = eHOM_Result_Error;
			HxLOG_Error("new Streaming error!! eStreamType(0x%x)\r\n",pstInst->eStreamType);
			break;
		}
		LEAVE_INST (pstInst);
	}

	if( eHOM_Result_Ok != eHomResult|| NULL == pstInst)
	{
		HxLOG_Error("Error> eHomResult(%s) or pstInst(0x%x) or hStreamingId(0x%x) error\r\n",
				PL_Streaming_Debug_GetHomResultStr(eHomResult), pstInst, hStreamingId);
	}

	LEAVE_FUNCTION;

	return eHomResult;
}

/**
 *
 *
 *
 * @param
 * @return
 */
HOM_Result_e PL_Streaming_Context_GetTS (const Handle_t hStreamingId,  const HCHAR *pRequestPath, HUINT32 *pulDataSize, HUINT8 **ppucData)
{
	plStreamingInst_t		*pstInst = NULL;
	HOM_Result_e 			eHomResult = eHOM_Result_Ok;

	ENTER_FUNCTION;

	ENTER_GSEM;
	pstInst = _plstreaming_Context_getInstance (hStreamingId);
	LEAVE_GSEM;

	if (NULL != pstInst)
	{
		ENTER_INST (pstInst);

		switch((pstInst->eStreamType & DxRSVSTREAM_TYPE_MASK))
		{
		case DxRSVSTREAM_TYPE_HLS_MASK:
			eHomResult = PL_Streaming_ModeHLS_GetTS(pstInst->pstModeInst, pRequestPath, pulDataSize, ppucData);

			if(NULL != pstInst->pstModeInst)
			{
				PL_Streaming_Debug_Print("(-) hStreaming(0x%x) hRingbuf(0x%x)\r\n", hStreamingId, pstInst->pstModeInst->hRingbuf);
				PL_Streaming_Resource_UpdateTime(pstInst->pstModeInst->hResHandle, PL_STREAMING_SCHEDULE_HLS_DURATION, 10);
			}
			break;

		case DxRSVSTREAM_TYPE_SATIP_MASK:
			eHomResult = PL_Streaming_ModeSatIp_GetTS(pstInst->pstModeInst, pRequestPath, pulDataSize, ppucData);
			// 2014/6/26 mhkang:
			// SAT>IP에서는 SAMA 스케쥴 업데이트를 Background로 하도록 수정함.
			//=> SETUP 성공후 OPTIONS만 주기적으로 보내는 경우, SAMA 스케쥴이 remove되면서 RTSP Session이 종료되는 문제가 있었음.

			break;

		case DxRSVSTREAM_TYPE_NTLS_MASK:
			eHomResult = PL_Streaming_ModeByPass_GetTS(pstInst->pstModeInst, pRequestPath, pulDataSize, ppucData);

			if(NULL != pstInst->pstModeInst)
			{
				PL_Streaming_Debug_Print("(-) hStreaming(0x%x) hRingbuf(0x%x)\r\n", hStreamingId, pstInst->pstModeInst->hRingbuf);
				PL_Streaming_Resource_UpdateTime(pstInst->pstModeInst->hResHandle, PL_STREAMING_SCHEDULE_NTLS_DURATION, 10);
			}
			break;

		default:
			eHomResult = eHOM_Result_Error;
			HxLOG_Error("new Streaming error!! eStreamType(0x%x)\r\n",pstInst->eStreamType);
			break;
		}


		LEAVE_INST (pstInst);
	}

	if( eHOM_Result_Ok != eHomResult|| NULL == pstInst)
	{
		HxLOG_Error("Error> eHomResult(%s) or pstInst(%d) or hStreamingId(0x%x) error\r\n",
				PL_Streaming_Debug_GetHomResultStr(eHomResult), pstInst, hStreamingId);
	}

	LEAVE_FUNCTION;

	return eHomResult;
}

/**
 *
 *
 *
 * @param
 * @return
 */
HOM_Result_e PL_Streaming_Context_ReleaseTS (const Handle_t hStreamingId,  const HCHAR *pRequestPath, HUINT8 *pucData)
{
	plStreamingInst_t		*pstInst = NULL;
	HOM_Result_e 			eHomResult = eHOM_Result_Ok;

	ENTER_FUNCTION;

	ENTER_GSEM;
	pstInst = _plstreaming_Context_getInstance (hStreamingId);
	LEAVE_GSEM;

	if (NULL != pstInst)
	{
		ENTER_INST (pstInst);

		switch((pstInst->eStreamType & DxRSVSTREAM_TYPE_MASK))
		{
		case DxRSVSTREAM_TYPE_HLS_MASK:
			eHomResult = PL_Streaming_ModeHLS_ReleaseTS(pstInst->pstModeInst, pRequestPath, pucData);
			break;

		case DxRSVSTREAM_TYPE_SATIP_MASK:
			eHomResult = PL_Streaming_ModeSatIp_ReleaseTS(pstInst->pstModeInst, pRequestPath, pucData);
			break;

		case DxRSVSTREAM_TYPE_NTLS_MASK:
			eHomResult = PL_Streaming_ModeByPass_ReleaseTS(pstInst->pstModeInst, pRequestPath, pucData);
			break;

		default:
			eHomResult = eHOM_Result_Error;
			HxLOG_Error("new Streaming error!! eStreamType(0x%x)\r\n",pstInst->eStreamType);
			break;
		}

		LEAVE_INST (pstInst);
	}

	if( eHOM_Result_Ok != eHomResult|| NULL == pstInst)
	{
		HxLOG_Error("Error> eHomResult(%s) or pstInst(%d) or hStreamingId(0x%x) error\r\n",
				PL_Streaming_Debug_GetHomResultStr(eHomResult), pstInst, hStreamingId);
	}

	LEAVE_FUNCTION;

	return eHomResult;
}


/**
 *
 *
 *
 * @param
 * @return
 */
HOM_Result_e PL_Streaming_Context_ExecCmd (const Handle_t hStreamingId,  const HCHAR *pRequestPath)
{
	plStreamingInst_t		*pstInst = NULL;
	HOM_Result_e 			eHomResult = eHOM_Result_Ok;

	ENTER_FUNCTION;

	ENTER_GSEM;
	pstInst = _plstreaming_Context_getInstance (hStreamingId);
	/* 2014/12/15 mhkang: SAT>IP TP Change인 경우  pstInst->pstModeInst 정보가 삭제될 수 있다.
						이때  PL_Streaming_ModeSatIp_Task 함수에서 thread 동기 이슈로 crash발생할 수 있다.
						LEAVE_GSEM는 함수 리턴하기전에 호출하도록 변경함.
	LEAVE_GSEM;
	*/

	if (NULL != pstInst)
	{
		ENTER_INST (pstInst);

		switch((pstInst->eStreamType & DxRSVSTREAM_TYPE_MASK))
		{
		case DxRSVSTREAM_TYPE_HLS_MASK:
			eHomResult = PL_Streaming_ModeHLS_ExecCmd(pstInst->pstModeInst, pRequestPath);
			if(NULL != pstInst->pstModeInst)
			{
				PL_Streaming_Debug_Print("(-) hStreaming(0x%x) hRingbuf(0x%x)\r\n", hStreamingId, pstInst->pstModeInst->hRingbuf);
				PL_Streaming_Resource_UpdateTime(pstInst->pstModeInst->hResHandle,PL_STREAMING_SCHEDULE_HLS_DURATION, 10);
			}
			break;

		case DxRSVSTREAM_TYPE_SATIP_MASK:
			eHomResult = PL_Streaming_ModeSatIp_ExecCmd(pstInst->pstModeInst, pRequestPath);
			// 2014/6/26 mhkang: SAT>IP에서는 SAMA 스케쥴 업데이트를 Background로 하도록 수정함.
			break;

		case DxRSVSTREAM_TYPE_NTLS_MASK:
			eHomResult = PL_Streaming_ModeByPass_ExecCmd(pstInst->pstModeInst, pRequestPath);
			if(NULL != pstInst->pstModeInst)
			{
				PL_Streaming_Debug_Print("(-) hStreaming(0x%x) hRingbuf(0x%x)\r\n", hStreamingId, pstInst->pstModeInst->hRingbuf);
				PL_Streaming_Resource_UpdateTime(pstInst->pstModeInst->hResHandle, PL_STREAMING_SCHEDULE_NTLS_DURATION, 10);
			}
			break;

		default:
			eHomResult = eHOM_Result_Error;
			HxLOG_Error("new Streaming error!! eStreamType(0x%x)\r\n",pstInst->eStreamType);
			break;
		}

		LEAVE_INST (pstInst);
	}

	if( eHOM_Result_Ok != eHomResult|| NULL == pstInst)
	{
		HxLOG_Error("Error> eHomResult(%s) or pstInst(%d) or hStreamingId(0x%x) error\r\n",
				PL_Streaming_Debug_GetHomResultStr(eHomResult), pstInst, hStreamingId);
	}

	// 2014/12/15 mhkang
	LEAVE_GSEM;

	LEAVE_FUNCTION;

	return eHomResult;
}



/**
 *
 *
 *
 * @param
 * @return
 */
HOM_Result_e PL_Streaming_Context_GetInfo ( const Handle_t hStreamingId, PLStreaming_InfoType_e eInfoType, const HCHAR *pRequestPath, HINT32 *pulDataSize, HCHAR **ppucData )
{
	plStreamingInst_t		*pstInst = NULL;
	HOM_Result_e 			eHomResult = eHOM_Result_Ok;

	ENTER_FUNCTION;

	ENTER_GSEM;
	pstInst = _plstreaming_Context_getInstance (hStreamingId);
	LEAVE_GSEM;

	if (NULL != pstInst)
	{
		ENTER_INST (pstInst);

		switch((pstInst->eStreamType & DxRSVSTREAM_TYPE_MASK))
		{
		case DxRSVSTREAM_TYPE_HLS_MASK:
			eHomResult = PL_Streaming_ModeHLS_GetInfo(pstInst->pstModeInst, eInfoType, pRequestPath, pulDataSize, ppucData);
			if(NULL != pstInst->pstModeInst)
			{
				PL_Streaming_Debug_Print("(-) hStreaming(0x%x) hRingbuf(0x%x)\r\n", hStreamingId, pstInst->pstModeInst->hRingbuf);
				PL_Streaming_Resource_UpdateTime(pstInst->pstModeInst->hResHandle, PL_STREAMING_SCHEDULE_HLS_DURATION, 10);
			}
			break;

		case DxRSVSTREAM_TYPE_SATIP_MASK:
			eHomResult = PL_Streaming_ModeSatIp_GetInfo(pstInst->pstModeInst, eInfoType, pRequestPath, pulDataSize, ppucData);
			// 2014/6/26 mhkang: SAT>IP에서는 SAMA 스케쥴 업데이트를 Background로 하도록 수정함.
			break;

		case DxRSVSTREAM_TYPE_NTLS_MASK:
			eHomResult = PL_Streaming_ModeByPass_GetInfo(pstInst->pstModeInst, eInfoType, pRequestPath, pulDataSize, ppucData);
			if(NULL != pstInst->pstModeInst)
			{
				PL_Streaming_Debug_Print("(-) hStreaming(0x%x) hRingbuf(0x%x)\r\n", hStreamingId, pstInst->pstModeInst->hRingbuf);
				PL_Streaming_Resource_UpdateTime(pstInst->pstModeInst->hResHandle,PL_STREAMING_SCHEDULE_NTLS_DURATION, 10);
			}
			break;


		default:
			eHomResult = eHOM_Result_Error;
			HxLOG_Error("new Streaming error!! eStreamType(0x%x)\r\n",pstInst->eStreamType);
			break;
		}



		LEAVE_INST (pstInst);
	}

	if( eHOM_Result_Ok != eHomResult|| NULL == pstInst)
	{
		HxLOG_Error("Error> eHomResult(%s) or pstInst(%d) or hStreamingId(0x%x) error\r\n",
				PL_Streaming_Debug_GetHomResultStr(eHomResult), pstInst, hStreamingId);
	}

	LEAVE_FUNCTION;

	return eHomResult;
}

/**
 *
 *
 *
 * @param
 * @return
 */
HOM_Result_e PL_Streaming_Context_ReleaseInfo ( const Handle_t hStreamingId, PLStreaming_InfoType_e eInfoType, const HCHAR *pRequestPath, HCHAR *pucData )
{
	plStreamingInst_t		*pstInst = NULL;
	HOM_Result_e 			eHomResult = eHOM_Result_Ok;

	ENTER_FUNCTION;

	ENTER_GSEM;
	pstInst = _plstreaming_Context_getInstance (hStreamingId);
	LEAVE_GSEM;

	if (NULL != pstInst)
	{
		ENTER_INST (pstInst);

		switch((pstInst->eStreamType & DxRSVSTREAM_TYPE_MASK))
		{
		case DxRSVSTREAM_TYPE_HLS_MASK:
			eHomResult = PL_Streaming_ModeHLS_ReleaseInfo(pstInst->pstModeInst, eInfoType, pRequestPath, pucData);
			break;

		case DxRSVSTREAM_TYPE_SATIP_MASK:
			eHomResult = PL_Streaming_ModeSatIp_ReleaseInfo(pstInst->pstModeInst, eInfoType, pRequestPath, pucData);
			break;

		case DxRSVSTREAM_TYPE_NTLS_MASK:
			eHomResult = PL_Streaming_ModeByPass_ReleaseInfo(pstInst->pstModeInst, eInfoType, pRequestPath, pucData);
			break;

		default:
			eHomResult = eHOM_Result_Error;
			HxLOG_Error("new Streaming error!! eStreamType(0x%x)\r\n",pstInst->eStreamType);
			break;
		}
		LEAVE_INST (pstInst);
	}

	if( eHOM_Result_Ok != eHomResult|| NULL == pstInst)
	{
		HxLOG_Error("Error> eHomResult(%s) or pstInst(%d) or hStreamingId(0x%x) error\r\n",
				PL_Streaming_Debug_GetHomResultStr(eHomResult), pstInst, hStreamingId);
	}

	LEAVE_FUNCTION;

	return eHomResult;
}

/**
 *
 *
 *
 * @param
 * @return
 */
HCHAR 	*PL_Streaming_Context_GetPrefixStringByStreamType(DxRsvStreamType_e eStreamType)
{
	HCHAR					*pszPrefix = NULL;

	ENTER_FUNCTION;

	pszPrefix = _plstreaming_Context_GetPrefixStringByStreamType(eStreamType);

	if( NULL == pszPrefix )
	{
		HxLOG_Error("szPrefix(0x%x) error\r\n", pszPrefix);
	}

	LEAVE_FUNCTION;

	return pszPrefix;
}


/**
 *
 *
 *
 * @param
 * @return
 */
HOM_Result_e 	PL_Streaming_Context_CreateSetupInfo (DxRsvStreamType_e eStreamType, const PLStreaming_StartInfo_t *pstStartInfo, PLStreaming_SetupInfo_t *ret_pstSetupInfo)
{
	HCHAR						*pszPrefix = NULL;
	HOM_Result_e 				eHomResult = eHOM_Result_Ok;
	PLStreaming_SetupInfo_t		*pstSetupInfo = NULL;

	ENTER_FUNCTION;

	eHomResult = eHOM_Result_Error;
	if(NULL != pstStartInfo)
	{
		pszPrefix 		= _plstreaming_Context_GetPrefixStringByStreamType(eStreamType);
		pstSetupInfo 	= _plstreaming_Context_CreateSetupInfo(pszPrefix, pstStartInfo, ret_pstSetupInfo);
	}

	if(NULL != pszPrefix && NULL != pstSetupInfo )
	{
		switch((pstSetupInfo->eStreamType & DxRSVSTREAM_TYPE_MASK))
		{
		case DxRSVSTREAM_TYPE_HLS_MASK:
			eHomResult = PL_Streaming_ModeHLS_StartInfoToSetupInfo(pszPrefix, pstStartInfo, pstSetupInfo);
			break;

		case DxRSVSTREAM_TYPE_SATIP_MASK:
			eHomResult = PL_Streaming_ModeSatIp_StartInfoToSetupInfo(pszPrefix, pstStartInfo, pstSetupInfo);
			break;

		case DxRSVSTREAM_TYPE_NTLS_MASK:
			eHomResult = PL_Streaming_ModeByPass_StartInfoToSetupInfo(pszPrefix, pstStartInfo, pstSetupInfo);
			break;

		default:
			eHomResult = eHOM_Result_Error;
			HxLOG_Error("new Streaming error!! eStreamType(0x%x)\r\n", eStreamType);
			break;
		}
	}

	if( eHOM_Result_Ok != eHomResult || NULL == pstSetupInfo)
	{
		HxLOG_Error("eHomResult(%s) pszPrefix(%s) error\r\n", PL_Streaming_Debug_GetHomResultStr(eHomResult), pszPrefix);
	}

	LEAVE_FUNCTION;

	return eHomResult;
}


/**
 *
 *
 *
 * @param
 * @return
 */
HOM_Result_e PL_Streaming_Context_CreateSetupInfoByStreamingId(const Handle_t hStreamingId, PLStreaming_SetupInfo_t *ret_pstSetupInfo)
{
	plStreamingInst_t			*pstInst = NULL;
	HOM_Result_e 				eHomResult = eHOM_Result_Ok;

	ENTER_FUNCTION;

	ENTER_GSEM;
	pstInst = _plstreaming_Context_getInstance (hStreamingId);
	LEAVE_GSEM;

	eHomResult = eHOM_Result_Error;
	if (NULL != pstInst)
	{
		ENTER_INST (pstInst);

		if( NULL != pstInst->pstModeInst)
		{
			_plstreaming_Context_CopySetupInfo(ret_pstSetupInfo, &pstInst->pstModeInst->stSetupInfo);
			eHomResult = eHOM_Result_Ok;
		}

		LEAVE_INST (pstInst);
	}

	if( eHOM_Result_Ok != eHomResult|| NULL == pstInst)
	{
		HxLOG_Error("Error> eHomResult(%s) or pstInst(%d) or hStreamingId(0x%x) error\r\n",
				PL_Streaming_Debug_GetHomResultStr(eHomResult), pstInst, hStreamingId);
	}

	LEAVE_FUNCTION;

	return eHomResult;
}



/**
 *
 *
 *
 * @param
 * @return
 */
HOM_Result_e PL_Streaming_Context_DeleteSetupInfo (PLStreaming_SetupInfo_t *pstSetupInfo)
{
	HOM_Result_e			eHomResult = eHOM_Result_Ok;

	ENTER_FUNCTION;

	if(NULL == pstSetupInfo)
	{
		HxLOG_Error("NULL == pstSetupInfo\r\n");
		return eHOM_Result_Error;
	}

	_plstreaming_Context_DeleteSetupInfo(pstSetupInfo);

	if( eHOM_Result_Ok != eHomResult)
	{
		HxLOG_Error("eHomResult(%s)  error\r\n", PL_Streaming_Debug_GetHomResultStr(eHomResult));
	}

	LEAVE_FUNCTION;

	return eHomResult;
}




/**
 *
 *
 *
 * @param
 * @return
 */
Handle_t PL_Streaming_Context_FindByStartInfo (const PLStreaming_StartInfo_t *pstStartInfo)
{
	Handle_t			hResult = HANDLE_NULL;
	plStreamingInst_t	*pstInst = NULL;

	ENTER_FUNCTION;

	ENTER_GSEM;
	pstInst = _plstreaming_Context_findInstanceByDeviceID(pstStartInfo->pszDeviceID);
	LEAVE_GSEM;

	if(NULL != pstInst)
	{
		ENTER_INST (pstInst);

		hResult = pstInst->hStreamingId;

		LEAVE_INST (pstInst);
	}

	LEAVE_FUNCTION;

	return hResult;
}


/**
 *
 *
 *
 * @param
 * @return
 */
Handle_t PL_Streaming_Context_FindByStreamingKey (const PL_StreamingKey_t *pstStreamingKey)
{
	Handle_t			hResult = HANDLE_NULL;
	plStreamingInst_t	*pstInst = NULL;
	HOM_Result_e 		eHomResult = eHOM_Result_Ok;

	ENTER_FUNCTION;


	ENTER_GSEM;
	pstInst = _plstreaming_Context_findInstanceByStreamingKey(pstStreamingKey);
	if(NULL != pstInst)
	{
		hResult = pstInst->hStreamingId;
	}
	LEAVE_GSEM;

#if 0
	eHomResult = eHOM_Result_Error;
	if(NULL != pstInst)
	{
		ENTER_INST (pstInst);

		hResult = pstInst->hStreamingId;
		eHomResult = eHOM_Result_Ok;

		LEAVE_INST (pstInst);
	}
#else
	eHomResult = eHOM_Result_Error;
	if(HANDLE_NULL != hResult && 0 != hResult)
	{
		eHomResult = eHOM_Result_Ok;
	}
#endif
	LEAVE_FUNCTION;

	if( eHOM_Result_Ok != eHomResult|| NULL == pstInst)
	{
		HxLOG_Error("Error> eHomResult(%s) or pstInst(%d) or pstStreamingKey(0x%s)\r\n",
				PL_Streaming_Debug_GetHomResultStr(eHomResult), pstInst, pstStreamingKey);
	}

	return hResult;
}



/**
 *
 *
 *
 * @param
 * @return
 */
HBOOL PL_Streaming_Context_IsIncludeVideo(DxRsvStreamType_e eStreamType)
{
	HBOOL bIsIncludeVideo = FALSE;

	ENTER_FUNCTION;

	bIsIncludeVideo = _plstreaming_Context_IsIncludeVideo(eStreamType);

	LEAVE_FUNCTION;

	return bIsIncludeVideo;
}


/**
 *
 *
 *
 * @param
 * @return
 */
HOM_Result_e PL_Streaming_Context_DupStreamingKey(const Handle_t hStreamingId, PL_StreamingKey_t **ret_ppstStreamingKey)
{
	plStreamingInst_t		*pstInst = NULL;
	HOM_Result_e 			eHomResult = eHOM_Result_Ok;

	ENTER_FUNCTION;

	ENTER_GSEM;
	pstInst = _plstreaming_Context_getInstance (hStreamingId);
	LEAVE_GSEM;

	eHomResult = eHOM_Result_Error;
	if (NULL != pstInst && NULL != pstInst->pstModeInst)
	{
		*ret_ppstStreamingKey = PL_StreamingKey_Dup(pstInst->pstModeInst->pstStreamingKey);
		eHomResult = eHOM_Result_Ok;
	}

	if( eHOM_Result_Ok != eHomResult|| NULL == pstInst)
	{
		HxLOG_Error("Error> eHomResult(%s) or pstInst(%d) or hStreamingId(0x%x) error\r\n",
				PL_Streaming_Debug_GetHomResultStr(eHomResult), pstInst, hStreamingId);
	}

	LEAVE_FUNCTION;

	return eHomResult;
}



/**
 *
 *
 *
 * @param
 * @return
 */
HOM_Result_e PL_Streaming_Context_GetProfileItemHandle(const Handle_t hStreamingId, Handle_t *ret_phProfileItem)
{
	plStreamingInst_t		*pstInst = NULL;
	HOM_Result_e 			eHomResult = eHOM_Result_Ok;

	ENTER_FUNCTION;

	ENTER_GSEM;
	pstInst = _plstreaming_Context_getInstance (hStreamingId);
	LEAVE_GSEM;

	if (NULL != pstInst)
	{
		ENTER_INST (pstInst);

		switch((pstInst->eStreamType & DxRSVSTREAM_TYPE_MASK))
		{
		case DxRSVSTREAM_TYPE_HLS_MASK:
			eHomResult = PL_Streaming_ModeHLS_GetProfileItemHandle(pstInst->pstModeInst, ret_phProfileItem);
			break;

		case DxRSVSTREAM_TYPE_SATIP_MASK:
			eHomResult = PL_Streaming_ModeSatIp_GetProfileItemHandle(pstInst->pstModeInst, ret_phProfileItem);
			break;

		case DxRSVSTREAM_TYPE_NTLS_MASK:
			eHomResult = PL_Streaming_ModeByPass_GetProfileItemHandle(pstInst->pstModeInst, ret_phProfileItem);
			break;

		default:
			eHomResult = eHOM_Result_Error;
			HxLOG_Error("new Streaming error!! eStreamType(0x%x)\r\n",pstInst->eStreamType);
			break;
		}
		LEAVE_INST (pstInst);
	}

	if( eHOM_Result_Ok != eHomResult|| NULL == pstInst)
	{
		HxLOG_Error("Error> eHomResult(%s) or pstInst(%d) or hStreamingId(0x%x) error\r\n",
				PL_Streaming_Debug_GetHomResultStr(eHomResult), pstInst, hStreamingId);
	}

	LEAVE_FUNCTION;

	return eHomResult;
}

/**
 *
 * ret_arrusPIDs의 max는 DxRSV_PIDTABLE_COUNT이다.
 *
 * @param
 * @return
 */
HOM_Result_e PL_Streaming_Context_GetPIDs(const Handle_t hStreamingId, HINT32 *ret_pnPIDsCount, HUINT16 *ret_arrusPIDs)
{
	plStreamingInst_t		*pstInst = NULL;
	HOM_Result_e 			eHomResult = eHOM_Result_Ok;

	ENTER_FUNCTION;

	ENTER_GSEM;
	pstInst = _plstreaming_Context_getInstance (hStreamingId);
	LEAVE_GSEM;

	if (NULL != pstInst)
	{
		ENTER_INST (pstInst);

		switch((pstInst->eStreamType & DxRSVSTREAM_TYPE_MASK))
		{
		case DxRSVSTREAM_TYPE_HLS_MASK:
			eHomResult = PL_Streaming_ModeHLS_GetPIDs(pstInst->pstModeInst, ret_pnPIDsCount, ret_arrusPIDs);
			break;

		case DxRSVSTREAM_TYPE_SATIP_MASK:
			eHomResult = PL_Streaming_ModeSatIp_GetPIDs(pstInst->pstModeInst, ret_pnPIDsCount, ret_arrusPIDs);
			break;

		case DxRSVSTREAM_TYPE_NTLS_MASK:
			eHomResult = PL_Streaming_ModeByPass_GetPIDs(pstInst->pstModeInst, ret_pnPIDsCount, ret_arrusPIDs);
			break;

		default:
			eHomResult = eHOM_Result_Error;
			HxLOG_Error("new Streaming error!! eStreamType(0x%x)\r\n",pstInst->eStreamType);
			break;
		}
		LEAVE_INST (pstInst);
	}

	if( eHOM_Result_Ok != eHomResult|| NULL == pstInst)
	{
		HxLOG_Error("Error> eHomResult(%s) or pstInst(%d) or hStreamingId(0x%x) error\r\n",
				PL_Streaming_Debug_GetHomResultStr(eHomResult), pstInst, hStreamingId);
	}

	LEAVE_FUNCTION;

	return eHomResult;
}


/**
 *
 *
 * @param
 * @return
 */
HOM_Result_e PL_Streaming_Context_GetRingbufHandle(const Handle_t hStreamingId, Handle_t *ret_phRingbuf)
{
	plStreamingInst_t		*pstInst = NULL;
	Handle_t				hRingBuf = HANDLE_NULL;
	HOM_Result_e 			eHomResult = eHOM_Result_Ok;

	ENTER_FUNCTION;

	ENTER_GSEM;
	pstInst = _plstreaming_Context_getInstance (hStreamingId);
	if(NULL != pstInst && NULL != pstInst->pstModeInst)
	{
		hRingBuf = pstInst->pstModeInst->hRingbuf;
	}
	LEAVE_GSEM;

#if 0
	if (NULL != pstInst)
	{
		ENTER_INST (pstInst);

		eHomResult = eHOM_Result_Error;
		if(NULL != pstInst->pstModeInst)
		{
			hRingBuf = pstInst->pstModeInst->hRingbuf;
			eHomResult = eHOM_Result_Ok;
		}

		LEAVE_INST (pstInst);
	}
#else
	eHomResult = eHOM_Result_Error;
	if(HANDLE_NULL != hRingBuf && 0 != hRingBuf)
	{
		eHomResult = eHOM_Result_Ok;
	}
#endif
	if( eHOM_Result_Ok != eHomResult|| NULL == pstInst || HANDLE_NULL == hRingBuf)
	{
		HxLOG_Error("Error> eHomResult(%s) or pstInst(%d) or hStreamingId(0x%x) or hRingBuf(0x%x) error\r\n",
				PL_Streaming_Debug_GetHomResultStr(eHomResult), pstInst, hStreamingId, hRingBuf);
	}

	*ret_phRingbuf = hRingBuf;

	LEAVE_FUNCTION;

	return eHomResult;
}


/**
 *
 *
 *
 * @param
 * @return
 */
HBOOL	 PL_Streaming_Context_IsUseStreamType(const DxRsvStreamType_e	eStreamType)
{
	HBOOL bIsUseStreamType = FALSE;

	ENTER_FUNCTION;

	bIsUseStreamType =  _plstreaming_Context_isUseStreamType((eStreamType & DxRSVSTREAM_TYPE_MASK));

	LEAVE_FUNCTION;

	return bIsUseStreamType;
}


/**
 *
 *
 *
 * @param
 * @return
 */
HBOOL	 PL_Streaming_Context_RegisterStatus(const cbPLStreamingStatus cbStatus)
{
	HBOOL bIsRegist = FALSE;

	ENTER_FUNCTION;

	ENTER_GSEM;
	if(NULL == s_cbPLStreamingStatus_ToDLNALib)
	{
		s_cbPLStreamingStatus_ToDLNALib = cbStatus;
		bIsRegist = TRUE;
	}
	else
	{
		HxLOG_Error("Error> failed to register callback!\r\n");
	}
	LEAVE_GSEM;

	LEAVE_FUNCTION;

	return bIsRegist;
}


/**
 *
 *
 *
 * @param
 * @return
 */
HOM_Result_e	 PL_Streaming_Context_StatusToDLNALib(const Handle_t hStreamingId, PLStreaming_Status_e eState)
{
	plStreamingInst_t		*pstInst = NULL;
	HCHAR					*pszDeviceID = NULL;
	HOM_Result_e 			eHomResult = eHOM_Result_Ok;

	ENTER_FUNCTION;
	HxLOG_Print("++[hStreamingId:0x%x, eState:%d]\n", hStreamingId, eState);

	ENTER_GSEM;
	pstInst = _plstreaming_Context_getInstance (hStreamingId);
	LEAVE_GSEM;

	eHomResult = eHOM_Result_Error;
	if (NULL != pstInst)
	{
		ENTER_INST (pstInst);

		if(NULL != pstInst->pstModeInst)
		{
			if(NULL != pstInst->pstModeInst->stSetupInfo.pszDeviceID)
			{
				pszDeviceID = HLIB_STD_StrDup(pstInst->pstModeInst->stSetupInfo.pszDeviceID);
				eHomResult = eHOM_Result_Ok;
			}
		}

		LEAVE_INST (pstInst);
	}

	if(eHOM_Result_Ok == eHomResult)
	{
		HxLOG_Print(HxANSI_COLOR_CYAN("################################################################\n"));
		HxLOG_Print(HxANSI_COLOR_CYAN("###### status - %d\n"), eState);
		HxLOG_Print(HxANSI_COLOR_CYAN("################################################################\n"));

		eHomResult = s_cbPLStreamingStatus_ToDLNALib(pszDeviceID, eState);
	}

	if( eHOM_Result_Ok != eHomResult|| NULL == pstInst )
	{
		HxLOG_Error("Error> [hStreamingId:0x%x] eHomResult(%s) or pszDeviceID(%s) or pstInst(%d) or hStreamingId(0x%x) error\r\n",
					hStreamingId, PL_Streaming_Debug_GetHomResultStr(eHomResult), pszDeviceID, pstInst, hStreamingId);
	}

	if(NULL != pszDeviceID)
	{
		HLIB_STD_MemFree(pszDeviceID);
		pszDeviceID = NULL;
	}


	LEAVE_FUNCTION;

	return eHomResult;
}


/**
 *
 *
 *
 * @param
 * @return
 */
HOM_Result_e PL_Streaming_Context_ResConnect (Handle_t hStreamingId)
{
	plStreamingInst_t		*pstInst = NULL;
	HOM_Result_e 			eHomResult = eHOM_Result_Ok;

	ENTER_FUNCTION;
	HxLOG_Debug("++[hStreamingId:0x%x]\n", hStreamingId);

	ENTER_GSEM;
	pstInst = _plstreaming_Context_getInstance (hStreamingId);
	LEAVE_GSEM;

	if (NULL != pstInst)
	{
		ENTER_INST (pstInst);

		switch((pstInst->eStreamType & DxRSVSTREAM_TYPE_MASK))
		{
		case DxRSVSTREAM_TYPE_HLS_MASK:
			eHomResult = PL_Streaming_ModeHLS_ResConnect(pstInst->pstModeInst);
			break;

		case DxRSVSTREAM_TYPE_SATIP_MASK:
			eHomResult = PL_Streaming_ModeSatIp_ResConnect(pstInst->pstModeInst);

			// 2015/2/25 mhkang: http://svn.humaxdigital.com:3000/issues/100957
			if (eHomResult != eHOM_Result_Ok)
			{
				ENTER_GSEM;
				PL_Streaming_ModeSatIp_DeleteInstance(pstInst->pstModeInst);
				LEAVE_GSEM;
			}
			break;

		case DxRSVSTREAM_TYPE_NTLS_MASK:
			eHomResult = PL_Streaming_ModeByPass_ResConnect(pstInst->pstModeInst);
			break;

		default:
			eHomResult = eHOM_Result_Error;
			HxLOG_Error("new Streaming error!! eStreamType(0x%x)\r\n",pstInst->eStreamType);
			break;
		}
		LEAVE_INST (pstInst);
	}

	if( eHOM_Result_Ok != eHomResult|| NULL == pstInst)
	{
		HxLOG_Error("Error> eHomResult(%s) or pstInst(%d) or hStreamingId(0x%x) error\r\n",
				PL_Streaming_Debug_GetHomResultStr(eHomResult), pstInst, hStreamingId);
	}

	LEAVE_FUNCTION;

	return eHomResult;

}


/**
 *
 *
 *
 * @param
 * @return
 */
HOM_Result_e PL_Streaming_Context_ResConflict(const Handle_t hStreamingId, HINT32 eNotifyType/*SAPI_NotifyType_e*/, HINT32 nRequestPid, HCHAR *fromProcessName)
{
	plStreamingInst_t		*pstInst = NULL;
	HOM_Result_e 			eHomResult = eHOM_Result_Ok;
	HCHAR					*pszDeviceID = NULL;
	int bPreventStopping = FALSE;

	ENTER_FUNCTION;
	HxLOG_Info("++[hStreamingId:0x%x eNotifyType:%d Process:%s]\n", hStreamingId, eNotifyType, fromProcessName);

	ENTER_GSEM;
	pstInst = _plstreaming_Context_getInstance (hStreamingId);
	LEAVE_GSEM;

	if (NULL != pstInst)
	{
		ENTER_INST (pstInst);

		switch((pstInst->eStreamType & DxRSVSTREAM_TYPE_MASK))
		{
		case DxRSVSTREAM_TYPE_HLS_MASK:
			eHomResult = PL_Streaming_ModeHLS_ResConflict(pstInst->pstModeInst, eNotifyType, nRequestPid, fromProcessName);
			break;

		case DxRSVSTREAM_TYPE_SATIP_MASK:
			eHomResult = PL_Streaming_ModeSatIp_ResConflict(pstInst->pstModeInst, eNotifyType, nRequestPid, fromProcessName);
			break;

		case DxRSVSTREAM_TYPE_NTLS_MASK:
			eHomResult = PL_Streaming_ModeByPass_ResConflict(pstInst->pstModeInst, eNotifyType, nRequestPid, fromProcessName);
			break;

		default:
			eHomResult = eHOM_Result_Error;
			HxLOG_Error("Error> new Streaming error!! eStreamType(0x%x)\r\n",pstInst->eStreamType);
			break;
		}

		if(eHOM_Result_Error_Conflict == eHomResult)
		{
			if(NULL != pstInst->pstModeInst)
			{
				if(NULL != pstInst->pstModeInst->stSetupInfo.pszDeviceID)
				{
					pszDeviceID = HLIB_STD_StrDup(pstInst->pstModeInst->stSetupInfo.pszDeviceID);
				}
			}
		}

#ifdef SUPPORT_PVR_HLS_SEEK
		if (pstInst->pstModeInst->stSetupInfo.eStreamType == DxRSVSTREAM_TYPE_HLS_FILE_PVR)
		{
			if (eNotifyType == eSAPI_NOTIFY_TYPE_ScheduleRemoved)
			{
				if (eHomResult == eHOM_Result_Error_Conflict) // Timeout이 발생하거나 강제 종료하는 경우
				{
					bPreventStopping = FALSE;
					pstInst->pstModeInst->eSetupStatus = ePLStreaming_SetupStatus_End;

					HxLOG_Print(HxANSI_COLOR_GREEN("################################################################\n"));
					HxLOG_Print(HxANSI_COLOR_RED  ("###### End PVR HLS Streaming(pszDeviceID:%s)\n"), pszDeviceID);
					HxLOG_Print(HxANSI_COLOR_GREEN("################################################################\n"));
				}
				else if (eHomResult == eHOM_Result_Ok) // HLS Pausing
				{
					bPreventStopping = TRUE;
					pstInst->pstModeInst->eSetupStatus = ePLStreaming_SetupStatus_HlsPaused;
					HxLOG_Print(HxANSI_COLOR_GREEN("################################################################\n"));
					HxLOG_Print(HxANSI_COLOR_GREEN("###### Pause PVR HLS Streaming(pszDeviceID:%s)\n"), pszDeviceID);
					HxLOG_Print(HxANSI_COLOR_GREEN("################################################################\n"));
				}
				else
				{
					// do nothing
				}
			}

			if (eNotifyType == eSAPI_NOTIFY_TYPE_ScheduleAdded)
			{
				pstInst->pstModeInst->eSetupStatus = ePLStreaming_SetupStatus_Running;
			}
		}
#endif
		LEAVE_INST (pstInst);

		if(NULL != pszDeviceID)
		{
			if (bPreventStopping == FALSE)
			{
				HxLOG_Print(HxANSI_COLOR_CYAN("################################################################\n"));
				HxLOG_Print(HxANSI_COLOR_CYAN("###### ePLStreaming_Status_Conflict (pszDeviceID:%s)\n"), pszDeviceID);
				HxLOG_Print(HxANSI_COLOR_CYAN("################################################################\n"));

				eHomResult = s_cbPLStreamingStatus_ToDLNALib(pszDeviceID, ePLStreaming_Status_Conflict);
			}

			HLIB_STD_MemFree(pszDeviceID);
			pszDeviceID = NULL;
		}
	}

	if( NULL == pstInst)
	{
		HxLOG_Error("Error> NULL instance! pstInst(%d) or hStreamingId(0x%x) error\r\n", pstInst, hStreamingId);

	}

	LEAVE_FUNCTION;

	return eHomResult;
}


/**********************************************************************************************************/
#define ____DEBUG_FUNCTIONS_______________________
/**********************************************************************************************************/
STATIC void _plstreaming_Context_SetupDump (const PLStreaming_SetupInfo_t *pstSetupInfo)
{
	if(NULL != pstSetupInfo)
	{
		HxLOG_Print("pstSetupInfo(0x%x)\n", (HUINT32)pstSetupInfo);
		HxLOG_HexIntDump(pstSetupInfo->eStreamType);
		HxLOG_DecIntDump(pstSetupInfo->nChunkMultiply);
		HxLOG_BoolDump(pstSetupInfo->bIsIncludeVideo);
		if(pstSetupInfo->pszIPAddress)		HxLOG_StringDump(pstSetupInfo->pszIPAddress);
		if(pstSetupInfo->pszDisplayString)	HxLOG_StringDump(pstSetupInfo->pszDisplayString);
		if(pstSetupInfo->pszDeviceID)		HxLOG_StringDump(pstSetupInfo->pszDeviceID);
		if(pstSetupInfo->pszStreamingPath)	HxLOG_StringDump(pstSetupInfo->pszStreamingPath);
		if(pstSetupInfo->pszPrefix)			HxLOG_StringDump(pstSetupInfo->pszPrefix);
		HxLOG_HexIntDump(pstSetupInfo->eOutType);
		HxLOG_BoolDump(pstSetupInfo->bNotUseResource);
		HxLOG_HexIntDump(pstSetupInfo->pstMediaProfile);
		PL_MediaProfile_Dump(pstSetupInfo->pstMediaProfile);
		PL_Streaming_Debug_PrintTuningParam(&pstSetupInfo->stTuningParam, __FUNCTION__, __LINE__);
	}
}

STATIC	void _plstreaming_Context_InstDump (plStreamingInst_t	*pstInst)
{
	if (NULL != pstInst)
	{
		//HxLOG_Warning대신 HLIB_CMD_Printf로 하고 싶은데...
		HLIB_CMD_Printf("hStreamingId(0x%x), pstInst(0x%x)\n", (HUINT32)pstInst->hStreamingId, (HUINT32)pstInst);
		HLIB_CMD_Printf("  + ulSemId           : 0x%x\r\n", pstInst->ulSemId);
		HLIB_CMD_Printf("  + bExecuting        : %s\r\n", ((pstInst->bExecuting == TRUE) ? "TRUE" : "FALSE"));
		HLIB_CMD_Printf("  + hStreamingId      : 0x%x\r\n", pstInst->hStreamingId);
		HLIB_CMD_Printf("  + hInstance         : 0x%x\r\n", pstInst->pstModeInst);
		HLIB_CMD_Printf("  + eStreamType       : 0x%x\r\n", pstInst->eStreamType);
		HLIB_CMD_Printf("--------------------------------------------------\r\n");
		if(NULL == pstInst->pstModeInst)
		{
			HLIB_CMD_Printf("  + pstInst->pstModeInst  : NULL\r\n");
		}
		else
		{
			HLIB_CMD_Printf("  + pstInst->pstModeInst  : 0x%x\r\n", pstInst->pstModeInst);

			if(NULL != pstInst->pstModeInst->pstStreamingKey)
			{
				HxLOG_HexIntDump(pstInst->pstModeInst->pstStreamingKey);
				PL_StreamingKey_Dump(pstInst->pstModeInst->pstStreamingKey);
			}
			if(NULL != pstInst->pstModeInst->pstMediaProfileItem)
			{
				HxLOG_HexIntDump(pstInst->pstModeInst->pstMediaProfileItem);
				PL_MediaProfile_ItemDump(pstInst->pstModeInst->pstMediaProfileItem);
			}

			PL_Streaming_Debug_PrintPids(pstInst->pstModeInst->nPIDsCount, pstInst->pstModeInst->usPIDs, __FUNCTION__, __LINE__);

			HLIB_CMD_Printf("  + pstInst->pstModeInst->stSetupInfo\n");

			_plstreaming_Context_SetupDump(&pstInst->pstModeInst->stSetupInfo);

			HLIB_CMD_Printf("--------------------------------------------------\r\n");

			switch((pstInst->eStreamType & DxRSVSTREAM_TYPE_MASK))
			{
			case DxRSVSTREAM_TYPE_HLS_MASK:
				PL_Streaming_ModeHLS_Dump(pstInst->pstModeInst);
				break;

			case DxRSVSTREAM_TYPE_SATIP_MASK:
				PL_Streaming_ModeSatIp_Dump(pstInst->pstModeInst);
				break;

			case DxRSVSTREAM_TYPE_NTLS_MASK:
				PL_Streaming_ModeByPass_Dump(pstInst->pstModeInst);
				break;

			default:
				HxLOG_Error("new Streaming error!! eStreamType(0x%x)\r\n",pstInst->eStreamType);
				break;
			}
		}
		HLIB_CMD_Printf("============================================================\r\n");


	}

}


/**
 *
 *
 *
 * @param
 * @return
 */
void PL_Streaming_Context_FullDump (void)
{
#if defined(CONFIG_DEBUG)
	HUINT32 			ulIndex = 0;
	plStreamingInst_t	*pInst = NULL;

	while(1)
	{
		ENTER_GSEM;
		pInst = _plstreaming_Context_getInstanceByIndex(ulIndex++);
		LEAVE_GSEM;
		if(NULL == pInst)
		{
			break;
		}

		if(0 != pInst->ulSemId)
			ENTER_INST (pInst);

		_plstreaming_Context_InstDump(pInst);

		if(0 != pInst->ulSemId)
			LEAVE_INST (pInst);
	}

#endif
}

/**
 *
 *
 *
 * @param
 * @return
 */
void PL_Streaming_Context_UseDump (void)
{
#if defined(CONFIG_DEBUG)
	HUINT32 			ulIndex = 0;
	plStreamingInst_t	*pInst = NULL;

	while(1)
	{
		ENTER_GSEM;
		pInst = _plstreaming_Context_getInstanceByIndex(ulIndex++);
		LEAVE_GSEM;
		if(NULL == pInst)
		{
			break;
		}

		// 사용중 dump를 위한 함수. 내부 sem을 사용하지 않는다.
		if(TRUE == pInst->bExecuting)
		{
			_plstreaming_Context_InstDump(pInst);
		}
	}

#endif
}


/**
 *
 *
 *
 * @param
 * @return
 */
void PL_Streaming_Context_Dump (const Handle_t hStreamingId)
{
#if defined(CONFIG_DEBUG)
	plStreamingInst_t		*pstInst = NULL;

	ENTER_GSEM;
	pstInst = _plstreaming_Context_getInstance (hStreamingId);
	LEAVE_GSEM;

	_plstreaming_Context_InstDump(pstInst);
#endif
}


/**
 *
 *
 *
 * @param
 * @return
 */
void PL_Streaming_Context_SetupDump (const PLStreaming_SetupInfo_t *pstSetupInfo)
{
#if defined(CONFIG_DEBUG)
	_plstreaming_Context_SetupDump(pstSetupInfo);
#endif
}


#if defined(CONFIG_DEBUG)
STATIC HINT32   _plcmd_streaming_Context_dump(void *szArgList)
{
	HCHAR   *aucArg=NULL;
	HCHAR	szStr[128] = {0, };

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL )
	{
		PL_Streaming_Context_UseDump();
		return HxCMD_OK;
	}

	if( sscanf(aucArg, "%s", (char *)&szStr) == 0 )
	{
		PL_Streaming_Context_UseDump();
		return HxCMD_OK;
	}

	HLIB_STD_StrUpper(szStr);

	if(VK_strcmp(szStr, "ALL") == 0)
	{
		PL_Streaming_Context_FullDump();
	}
	else if(VK_strcmp(szStr, "USE") == 0)
	{
		PL_Streaming_Context_UseDump();
	}
	else
	{
		PL_Streaming_Context_UseDump();
	}

	return HxCMD_OK;
}
#endif


void	HOMMA_CMDRegister_StreamingContext(const HCHAR *hCmdHandle)
{
#if defined(CONFIG_DEBUG)
	HLIB_CMD_RegisterWord( hCmdHandle, _plcmd_streaming_Context_dump, "dump_context", "all | use", "context dump");
#endif
}

//2014/6/26 mhkang: this function is called every 1000 msec in Resource Task Thread.
void PL_Streaming_Context_Task(void)
{
	HUINT32				 ulIndex;

	for (ulIndex = 0; ulIndex < PL_STREAMING_INST_NUM; ulIndex++)
	{

		ENTER_GSEM;
		if (s_astStreaming_Instance[ulIndex].bExecuting == FALSE)
		{
			LEAVE_GSEM;
			VK_TASK_Sleep(1);
			continue;
		}

		if (NULL == s_astStreaming_Instance[ulIndex].pstModeInst)
		{
			LEAVE_GSEM;
			VK_TASK_Sleep(1);
			continue;
		}

		switch((s_astStreaming_Instance[ulIndex].pstModeInst->stSetupInfo.eStreamType & DxRSVSTREAM_TYPE_MASK))
		{
			case DxRSVSTREAM_TYPE_HLS_MASK:
				PL_Streaming_ModeHLS_Task(&s_astStreaming_Instance[ulIndex]);
				break;

		case DxRSVSTREAM_TYPE_SATIP_MASK:
			PL_Streaming_ModeSatIp_Task(&s_astStreaming_Instance[ulIndex]);
			break;

			case DxRSVSTREAM_TYPE_NTLS_MASK:
				// Do Nothing
				break;

			default:
				HxLOG_Error("new Streaming error!! eStreamType(0x%x)\r\n", s_astStreaming_Instance[ulIndex].pstModeInst->stSetupInfo.eStreamType);
				break;
		}

		LEAVE_GSEM;
	}
}

/*
typedef enum  _PLStreaming_SetupStatus_e
{
	ePLStreaming_SetupStatus_None 		= 0,

	ePLStreaming_SetupStatus_Start,
	ePLStreaming_SetupStatus_Running,
	ePLStreaming_SetupStatus_Loaded,
	ePLStreaming_SetupStatus_Conflict,

	ePLStreaming_SetupStatus_HlsPausing,
	ePLStreaming_SetupStatus_HlsPaused,
	ePLStreaming_SetupStatus_Error,

	ePLStreaming_SetupStatus_End
} PLStreaming_SetupStatus_e;
*/
HCHAR*  PL_Streaming_Context_GetSetupStatusStr(PLStreaming_SetupStatus_e SetupStatus)
{
	switch(SetupStatus)
	{
		case ePLStreaming_SetupStatus_None:
		return "SetupStatus_None";
		case ePLStreaming_SetupStatus_Start:
		return "SetupStatus_Start";
		case ePLStreaming_SetupStatus_Running:
		return "SetupStatus_Running";
		case ePLStreaming_SetupStatus_Loaded:
		return "SetupStatus_Loaded";
		case ePLStreaming_SetupStatus_Conflict:
		return "SetupStatus_Conflict";
		case ePLStreaming_SetupStatus_HlsPausing:
		return "SetupStatus_HlsPausing";
		case ePLStreaming_SetupStatus_HlsPaused:
		return "SetupStatus_HlsPaused";
		case ePLStreaming_SetupStatus_Error:
		return "SetupStatus_Error";
		case ePLStreaming_SetupStatus_End:
		return "SetupStatus_End";
		default:
		return "SetupStatus_Unknown";
	}
}


plStreamingInst_t *PL_Streaming_Context_getInstance (const Handle_t hStreamingId)
{
	return _plstreaming_Context_getInstance(hStreamingId);
}
