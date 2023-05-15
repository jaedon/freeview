/*
 * (c) 2011-2012 Humax Co., Ltd.
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
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/
/*
	@file     cxcore_session.c
	@brief    cxcore_session.c (CONAX MAIN / Session)

	Description:  session table 관리 등등 \n
	Module: MW/CAS/ CONAX \n
	Remarks :\n

	Copyright (c) 2009 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <util.h>
#include <cxmgr_action.h>

#include "../local_include/cxcore_main.h"
#include "../local_include/cxcore_os.h"
#include "../local_include/cxcore_session.h"
#include "../local_include/cxcore_stream.h"
#include "../local_include/cxcore_emm.h"
#include "../local_include/cxcore_ecm.h"
#include "../local_include/cxcore_sc.h"
#include "../local_include/cxcore_ui.h"
#if defined(CONFIG_MW_MM_PVR)
#include "../local_include/cxcore_pvr.h"
#endif


/*******************************************************************/
/********************      Global Variables 	********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines  	   *************************/
/*******************************************************************/
// 외부에서 전달된 pmt/cat 등의 처리를 바로 하려면 아래 define 사용. message를 사용하여 task를 구분하려면 comment 처리
//#define CX_USE_DIRECT_CALL

#define CONAX_NUM_OF_LIVE_SESSION			(NUM_VIEW_ACTION - 1)	// View + Playback...
#define CONAX_NUM_OF_TSR_SESSION			(NUM_TSRREC_ACTION)
#define CONAX_NUM_OF_REC_SESSION			(NUM_REC_ACTION)
#define CONAX_NUM_OF_PB_SESSION				(NUM_VIEW_ACTION - CONAX_NUM_OF_LIVE_SESSION)

#define CONAX_SESSION_URI_WAIT_TIME		5000

/*******************************************************************/
/********************      Functions	   *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef  	   *************************/
/*******************************************************************/

typedef struct
{
	HINT32 nStartRange;
	HINT32 nEndRange;
} CxSessionRange_t;

// session 상태 (쓰이고 있는지 아닌지..)
typedef enum
{
	eCxSession_Init,		// reset 전
	eCxSession_Ready,		// reset 된 후
	eCxSession_Used,		// scrambled service 에 사용
} CxSessionState_t;

// information about session (각 session에 service (action)을 할당한다. 즉, session단위로 모든 처리를 한다)
typedef struct
{
	Handle_t			hStbHandle;
	Handle_t			hPrevStbHandle;
	HINT32				nSessionIdx;
	CxSessionState_t	eState;
	HINT32				nScSlot;				// = -1 이면 s/c extracted 상태
	HBOOL				bSelected;				// 선택되어야만 ecm filter 사용하도록
	HUINT16				usUniqueTsId;			// 각 session별로 CAT가 들어올 수도 있어서 같은 것들을 구분하려면 필요.
	HUINT8				*pucCat;
	SvcCas_PmtInfo_t* 	pstPmtInfo;
	CxEsPidInfo_t		stEsPidInfo;
#if defined(CONFIG_SUPPORT_FCC)
	HBOOL				bMainSession;
#endif
#if defined(CONFIG_MW_MM_PVR)
	HBOOL				bCaPinPass;
#endif
#if defined(CONFIG_MW_CAS_CONAX_URI)
	HULONG				ulUriTimerId;
	DxCopyrightControl_ConaxDrmInfo_t	stUri;
#endif
} CxSession_t;

/*******************************************************************/
/********************      Static Variables 	********************/
/*******************************************************************/
STATIC HULONG		s_ulCxSessionSemId;
STATIC CxSession_t	*s_pstCxSession[CX_MAX_NUM_OF_SESSION];

/*******************************************************************/
/********************      Functions	   *************************/
/*******************************************************************/

#define INIT_________________________________________________

HERROR CXSESSION_InitCore(void)
{
	int		i;
	HINT32	nResult			= 0;
	CxSession_t	*pstSession;

	ENTER_FUNCTION;

	nResult = CX_SEM_Create("CxCSessSem", &s_ulCxSessionSemId);
	if (nResult != ERR_OK)
	{
		HxLOG_Error("Failure in Semaphore Creation \r\n");
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	for (i = 0; i < CX_MAX_NUM_OF_SESSION; i++)
	{
		pstSession = CX_MEM_Alloc(sizeof(CxSession_t));
		if(pstSession != NULL)
		{
			pstSession->hStbHandle = HANDLE_NULL;
			pstSession->nSessionIdx = CX_INVALID_SESSION_NUM;
			pstSession->eState = eCxSession_Init;
			pstSession->nScSlot = CX_SC_NONE;
			pstSession->bSelected = FALSE;
			pstSession->usUniqueTsId = 0;
			pstSession->pucCat = NULL;
			pstSession->pstPmtInfo = NULL;
			pstSession->stEsPidInfo.ulPidNum = 0;
#if defined(CONFIG_MW_CAS_CONAX_URI)
			pstSession->ulUriTimerId = VK_TIMERID_NULL;
			CX_memset(&pstSession->stUri, 0xff, sizeof(DxCopyrightControl_ConaxDrmInfo_t));
#endif
		}

		s_pstCxSession[i] = pstSession;
	}

#if defined(CONFIG_SUPPORT_FCC)
	s_pstCxSession[0]->bMainSession = TRUE;
#endif

	LEAVE_FUNCTION;

	return ERR_OK;
}

#if defined(CONFIG_SUPPORT_S3MODE_FASTBOOT)
HERROR CXSESSION_DeInitCore(void)
{
	int		i;
	CxSession_t	*pstSession;

	for (i = 0; i < CX_MAX_NUM_OF_SESSION; i++)
	{
		pstSession = s_pstCxSession[i];

		if(pstSession != NULL)
		{
			if(pstSession->pucCat != NULL)
			{
				CX_MEM_Free(pstSession->pucCat);
				pstSession->pucCat = NULL;
			}
		}
	}

	return ERR_OK;
}
#endif

#define SESSION_TABLE______________________________________________

STATIC void local_cxsession_GetSessionRange(SvcCas_ActionType_e eActionType, CxSessionRange_t *pstRange)
{
	if(pstRange != NULL)
	{
		switch(eActionType)
		{
			case eCAS_ActionType_Live:
				pstRange->nStartRange = 0;
				pstRange->nEndRange = CONAX_NUM_OF_LIVE_SESSION;
				break;

			case eCAS_ActionType_Tsr_Rec:
				pstRange->nStartRange = CONAX_NUM_OF_LIVE_SESSION;
				pstRange->nEndRange = pstRange->nStartRange + CONAX_NUM_OF_TSR_SESSION;
				break;

			case eCAS_ActionType_Playback:
			case eCAS_ActionType_Tsr_Play:
				pstRange->nStartRange = CX_MAX_NUM_OF_SESSION - CONAX_NUM_OF_PB_SESSION;
				pstRange->nEndRange = pstRange->nStartRange + CONAX_NUM_OF_PB_SESSION;
				break;

			default:
				pstRange->nStartRange = CONAX_NUM_OF_LIVE_SESSION + CONAX_NUM_OF_TSR_SESSION;
				pstRange->nEndRange = pstRange->nStartRange + CONAX_NUM_OF_REC_SESSION;
				break;
		}
	}
}

STATIC void local_cxsession_DeleteEmm(CxSession_t *pstSession, HUINT16 usUniqueTsId)
{
	int		i;
	HBOOL	bTsIdNotFound	= TRUE;

	for (i = 0; i < CX_MAX_NUM_OF_SESSION; i++)
	{
		if(s_pstCxSession[i] == pstSession)
		{
			// 자기 자신의 정보는 초기화 하고...
			pstSession->usUniqueTsId = 0;
			if(pstSession->pucCat != NULL)
			{
				CX_MEM_Free(pstSession->pucCat);
				pstSession->pucCat = NULL;
			}
			continue;
		}

		if (s_pstCxSession[i]->usUniqueTsId == usUniqueTsId)
		{
			bTsIdNotFound = FALSE;
		}
	}

	if (bTsIdNotFound)
	{
		// 동일 TS ID를 참조하는 session이 없다면 EMM 삭제
		CXEMM_DeleteByTsId(usUniqueTsId);
	}
}

STATIC void local_cxsession_RestartEmm(CxSession_t *pstSession, HUINT16 usUniqueTsId)
{
	int		i;
	HBOOL	bTsIdNotFound	= TRUE;

	for (i = 0; i < CX_MAX_NUM_OF_SESSION; i++)
	{
		if(s_pstCxSession[i]->bSelected != TRUE)
		{
			continue;
		}

		if(s_pstCxSession[i] == pstSession)
		{
			continue;
		}

		if (s_pstCxSession[i]->usUniqueTsId == usUniqueTsId)
		{
			bTsIdNotFound = FALSE;
			break;
		}
	}

	if (bTsIdNotFound == FALSE)
	{
		// 동일 TS ID를 참조하는 session이 있으면 해당 TP로 EMM Filter Restart
		CXEMM_RestartFilterByTsId((Handle_t)s_pstCxSession[i], usUniqueTsId);
	}
}

STATIC CxSession_t* local_cxsession_GetSessionByHandle(Handle_t hSession)
{
	int i;

	for (i = 0; i < CX_MAX_NUM_OF_SESSION; i++)
	{
		if(s_pstCxSession[i] == (CxSession_t *)hSession)
		{
			return s_pstCxSession[i];
		}
	}

	return NULL;
}

STATIC CxSession_t* local_cxsession_GetSession(Handle_t hStbHandle)
{
	int			i;

	ENTER_FUNCTION;

	for (i = 0; i < CX_MAX_NUM_OF_SESSION; i++)
	{
		if (s_pstCxSession[i]->hStbHandle == hStbHandle)
		{
			return s_pstCxSession[i];
		}
	}

	LEAVE_FUNCTION;
	return NULL;
}

STATIC HINT32 local_cxsession_FindIndexByStbHandle(Handle_t hStbHandle)
{
	HINT32	nIdx;

	ENTER_FUNCTION;
	for (nIdx = 0; nIdx < CX_MAX_NUM_OF_SESSION; nIdx++)
	{
		if (s_pstCxSession[nIdx]->hStbHandle == hStbHandle)
		{
			LEAVE_FUNCTION;
			return nIdx;
		}
	}

	LEAVE_FUNCTION;
	return -1;
}

#if defined(CONFIG_MW_CAS_CONAX_URI)
STATIC void local_cxsession_cbUriTimer(unsigned long ulUriTimerId, void* pvParam)
{
	CxSession_t *pstSession = *((CxSession_t **)pvParam);
	if(pstSession == NULL)
		return;

	CX_SEM_Get(s_ulCxSessionSemId);

	if(pstSession->ulUriTimerId != ulUriTimerId)
	{
		CX_SEM_Release(s_ulCxSessionSemId);
		return;
	}

	pstSession->ulUriTimerId = VK_TIMERID_NULL;
	CXCORE_SendMsg(eCxMsg_UriTimeout, (Handle_t)pstSession, 0, 0);

	CX_SEM_Release(s_ulCxSessionSemId);
}

STATIC HERROR local_cxsession_UriWait(CxSession_t *pstSession)
{
	if(pstSession == NULL)
		return ERR_FAIL;

	if(pstSession->ulUriTimerId != VK_TIMERID_NULL)
		return ERR_FAIL;

	VK_TIMER_EventAfter(CONAX_SESSION_URI_WAIT_TIME, local_cxsession_cbUriTimer, (void*)&pstSession, sizeof(CxSession_t *), &pstSession->ulUriTimerId);

	return ERR_OK;
}

STATIC HERROR local_cxsession_ApplyUri(CxSession_t *pstSession)
{
	Handle_t hAction;

	switch(CXACT_GetActionTypeFromStbHandle(pstSession->hStbHandle))
	{
		case eCAS_ActionType_Recording:
		case eCAS_ActionType_Tsr_Rec:
		#if defined(CONFIG_MW_MM_PVR)
			CXAPI_PVR_UpdateUriToMetaFile(CXACT_GetActionHandle(pstSession->hStbHandle), &pstSession->stUri);
		#endif
			break;

		default:
			break;
	}

	hAction = CXACT_GetActionHandle(pstSession->hStbHandle);

	if(CXACT_UpdateDrmInfo(hAction, &pstSession->stUri) != ERR_OK)
	{
		HxLOG_Error("CXACT_UpdateDrmInfo(0x%x)) error\n", hAction);
		return ERR_FAIL;
	}

	return ERR_OK;
}
#endif

STATIC HERROR local_cxsession_UpdateCat(CxSession_t *pstSession, HUINT8 *pucCat)
{
	HUINT16 usOldCat, usNewCat;

	if(pstSession->pucCat == NULL)
		return ERR_OK;

	usOldCat = get12bit(pstSession->pucCat + 1) + 3;
	usNewCat = get12bit(pucCat + 1) + 3;

	if(usOldCat != usNewCat)
		return ERR_OK;

	if(CX_memcmp(pstSession->pucCat, pucCat, sizeof(usNewCat)) != 0)
		return ERR_OK;

	return ERR_FAIL;
}

STATIC HERROR local_cxsession_CheckPmt(CxSession_t *pstSession)
{
	HBOOL bScrambled	= FALSE;

	// 지금은 smart card가 없으면 stream/ecm 처리도 안하게 되어 있음.
	if (CXSTREAM_ParsePmt((Handle_t)pstSession) != ERR_OK)
	{
		HxLOG_Error("CXSTREAM_ParsePmt()\n");
		return ERR_FAIL;
	}

	if (CXSTREAM_IsScrambledSvc((Handle_t)pstSession, &bScrambled) != ERR_OK)
	{
		HxLOG_Error("CXSTREAM_IsScrambledSvc()\n");
		goto _EXIT;
	}

	if (bScrambled == TRUE)
	{
		//scrambled svc
		pstSession->eState = eCxSession_Used;
	}
	else
	{
		// free svc
		if (pstSession->eState == eCxSession_Used)
		{
			// scrambed -> free상황이므로 reset session 필요함
			CXSC_ResetSession(pstSession->nScSlot, pstSession->nSessionIdx);
		}
		pstSession->eState = eCxSession_Ready;
	}

_EXIT:
	// ecm filter start (해보는데, s/c가 없거나 used 선택이 안되면 filter start가 안될 거임.. try의 개념)
	CXSTREAM_StartAllEcmFilter((Handle_t)pstSession);

	return ERR_OK;
}

STATIC CxSession_t *local_cxsession_GetMainHandle(void)
{
	CxSession_t *pstSession = NULL;
	CxSessionRange_t stRange;
	int i;

	local_cxsession_GetSessionRange(eCAS_ActionType_Live, &stRange);

	for(i = stRange.nStartRange ; i < stRange.nEndRange ; i++)
	{
#if defined(CONFIG_SUPPORT_FCC)
		if(s_pstCxSession[i]->bMainSession == TRUE)
		{
			pstSession = s_pstCxSession[i];
		}
#else
		pstSession = s_pstCxSession[i];
#endif
	}

	return pstSession;
}

#define __________STATIC_FUNCTION__________

// nScSlot == CX_SC_NONE: SC와 상관없이 모든 session 얻어감.
HUINT32 CXSESSION_GetAllSessionByScSlot(HINT32 nScSlot, Handle_t* phList)
{
	int		i;
	HUINT32	ulCount;

	ENTER_FUNCTION;

	CX_SEM_Get(s_ulCxSessionSemId);

	for (i = 0, ulCount = 0; i < CX_MAX_NUM_OF_SESSION; i++)
	{
		if (s_pstCxSession[i]->hStbHandle != HANDLE_NULL &&	//
			(s_pstCxSession[i]->nScSlot == nScSlot || nScSlot == CX_SC_NONE))
		{
			phList[ulCount++] = (Handle_t)s_pstCxSession[i];
		}
	}

	CX_SEM_Release(s_ulCxSessionSemId);
	LEAVE_FUNCTION;
	return ulCount;
}

Handle_t CXSESSION_GetMainHandle(void)
{
	CxSession_t *pstSession = NULL;
	CxSessionRange_t stRange;
	int i;

	local_cxsession_GetSessionRange(eCAS_ActionType_Live, &stRange);

	for(i = stRange.nStartRange ; i < stRange.nEndRange ; i++)
	{
#if defined(CONFIG_SUPPORT_FCC)
		if(s_pstCxSession[i]->bMainSession == TRUE)
		{
			pstSession = s_pstCxSession[i];
		}
#else
		pstSession = s_pstCxSession[i];
#endif
	}

	if(pstSession == NULL)
	{
		return HANDLE_NULL;
	}

	return (Handle_t)pstSession;
}

Handle_t CXSESSION_GetHandleByStbHandle(Handle_t hStbHandle)
{
	CxSession_t* 					pSession;

	CX_SEM_Get(s_ulCxSessionSemId);

	pSession = local_cxsession_GetSession(hStbHandle);

	CX_SEM_Release(s_ulCxSessionSemId);

	if (pSession == NULL)
	{
		return HANDLE_NULL;
	}

	return (Handle_t)pSession;
}

Handle_t CXSESSION_GetStbHandle(Handle_t hSession)
{
	CxSession_t* 					pSession;

	ENTER_FUNCTION;
	pSession = local_cxsession_GetSessionByHandle(hSession);
	if (pSession == NULL)
	{
		LEAVE_FUNCTION;
		return HANDLE_NULL;
	}

	LEAVE_FUNCTION;
	return pSession->hStbHandle;
}

HUINT32 CXSESSION_GetStbActionId(Handle_t hSession)
{
	CxSession_t* 					pSession;

	ENTER_FUNCTION;
	pSession = local_cxsession_GetSessionByHandle(hSession);
	if (pSession == NULL)
	{
		LEAVE_FUNCTION;
		return HANDLE_NULL;
	}

	LEAVE_FUNCTION;
	return (pSession->hStbHandle >> 24);
}

SvcCas_ActionType_e CXSESSION_GetActionType(Handle_t hSession)
{
	CxSession_t *pstSession = NULL;

	pstSession = local_cxsession_GetSessionByHandle(hSession);
	if (pstSession == NULL)
	{
		HxLOG_Error("pstSession NULL\n");
		return eCAS_ActionType_None;
	}

	return CXACT_GetActionTypeFromStbHandle(pstSession->hStbHandle);
}

SvcCas_PmtInfo_t* CXSESSION_GetPmtInfo(Handle_t hSession)
{
	CxSession_t* 					pSession;

	ENTER_FUNCTION;
	pSession = local_cxsession_GetSessionByHandle(hSession);
	if (pSession == NULL)
	{
		LEAVE_FUNCTION;
		return NULL;
	}

	LEAVE_FUNCTION;
	return pSession->pstPmtInfo;
}

HINT32 CXSESSION_GetScSlot(Handle_t hSession)
{
	CxSession_t* 					pSession;

	ENTER_FUNCTION;
	pSession = local_cxsession_GetSessionByHandle(hSession);
	if (pSession == NULL)
	{
		LEAVE_FUNCTION;
		return CX_SC_NONE;
	}

	LEAVE_FUNCTION;
	return pSession->nScSlot;
}

HINT32 CXSESSION_GetSessionCnt(void)
{
	int		i;
	HINT32	nCnt	= 0;

	for (i = 0; i < CX_MAX_NUM_OF_SESSION; i++)
	{
		if (s_pstCxSession[i]->hStbHandle != HANDLE_NULL)
		{
			nCnt++;
		}
	}
	return nCnt;
}

HINT32 CXSESSION_GetSessionIdx(Handle_t hSession)
{
	CxSession_t* 					pSession;

	ENTER_FUNCTION;
	pSession = local_cxsession_GetSessionByHandle(hSession);
	if (pSession == NULL)
	{
		LEAVE_FUNCTION;
		return CX_INVALID_SESSION_NUM;
	}

	LEAVE_FUNCTION;
	return pSession->nSessionIdx;
}

HBOOL CXSESSION_IsSelected(Handle_t hSession)
{
	CxSession_t* 					pSession;

	ENTER_FUNCTION;
	pSession = local_cxsession_GetSessionByHandle(hSession);
	if (pSession == NULL)
	{
		LEAVE_FUNCTION;
		return FALSE;
	}

	LEAVE_FUNCTION;
	return pSession->bSelected;
}

HBOOL CXSESSION_IsTsrSession(Handle_t hSession)
{
	CxSession_t* 					pSession;
	HBOOL		bIsTsr;

	ENTER_FUNCTION;
	pSession = local_cxsession_GetSessionByHandle(hSession);
	if (pSession == NULL)
	{
		LEAVE_FUNCTION;
		return FALSE;
	}

	bIsTsr = CXACT_IsTsrActionTypeFromStbHandle(pSession->hStbHandle);
	LEAVE_FUNCTION;
	return bIsTsr;
}

HBOOL CXSESSION_IsRecordSession(Handle_t hSession)
{
	CxSession_t* 					pSession;
	HBOOL		bIsRecord;

	ENTER_FUNCTION;
	pSession = local_cxsession_GetSessionByHandle(hSession);
	if (pSession == NULL)
	{
		LEAVE_FUNCTION;
		return FALSE;
	}

	bIsRecord = CXACT_IsRecActionTypeFromStbHandle(pSession->hStbHandle);

	LEAVE_FUNCTION;
	return bIsRecord;
}

HERROR CXSESSION_ResetAllSession()
{
	HERROR	hError	= ERR_OK;
	int		i;

	for (i = 0 ; i < CX_MAX_NUM_OF_SESSION ; i++)
	{
		if (CXACT_IsRecActionTypeFromStbHandle(s_pstCxSession[i]->hStbHandle) == FALSE)
		{
			hError |= CXSC_ResetSession(s_pstCxSession[i]->nScSlot, s_pstCxSession[i]->nSessionIdx);
		}
	}

	return hError;
}

HERROR CXSESSION_ResetSession(Handle_t hStbHandle)
{
	CxSession_t *pstSession;

	if(hStbHandle == HANDLE_NULL)
	{
		return ERR_FAIL;
	}

	CX_SEM_Get(s_ulCxSessionSemId);

	pstSession = local_cxsession_GetSession(hStbHandle);
	if(pstSession == NULL)
	{
		CX_SEM_Release(s_ulCxSessionSemId);
		return ERR_FAIL;
	}

	if(pstSession->nSessionIdx == CX_INVALID_SESSION_NUM)
	{
		CX_SEM_Release(s_ulCxSessionSemId);
		return ERR_FAIL;
	}

	if(pstSession->eState != eCxSession_Used)
	{
		CX_SEM_Release(s_ulCxSessionSemId);
		return ERR_FAIL;
	}

	CXSC_ResetSession(pstSession->nScSlot, pstSession->nSessionIdx);

	CX_SEM_Release(s_ulCxSessionSemId);

	return ERR_OK;
}

HERROR CXSESSION_Select(Handle_t hStbHandle, HBOOL bSelect)
{
	CxSession_t* 					pSession;

	ENTER_FUNCTION;

	CX_SEM_Get(s_ulCxSessionSemId);

	pSession = local_cxsession_GetSession(hStbHandle);
	if (pSession == NULL)
	{
		CX_SEM_Release(s_ulCxSessionSemId);
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	pSession->bSelected = bSelect;

	if (bSelect == TRUE)
	{
		CXCORE_SendMsg(eCxMsg_Ecm_FilterRestart, (Handle_t)pSession, (HUINT32)0, (HUINT32)0);
	}

	CX_SEM_Release(s_ulCxSessionSemId);
	LEAVE_FUNCTION;
	return ERR_OK;
}

HERROR CXSESSION_StartSvc(Handle_t hStbHandle)
{
	CxSession_t *pstSession = NULL, *pstMainSession = NULL;
	CxSessionRange_t stRange;
	SvcCas_ActionType_e eActionType;
	HBOOL bNotFound = TRUE;
	int	i = 0;

	ENTER_FUNCTION;

	CX_SEM_Get(s_ulCxSessionSemId);

	eActionType = CXACT_GetActionTypeFromStbHandle(hStbHandle);

	local_cxsession_GetSessionRange(eActionType, &stRange);

	for(i = stRange.nStartRange ; i < stRange.nEndRange ; i++)
	{
		if(s_pstCxSession[i]->hStbHandle == HANDLE_NULL)
		{
			if(s_pstCxSession[i]->hPrevStbHandle == hStbHandle)
			{
				pstSession = s_pstCxSession[i];
				bNotFound = FALSE;
				break;
			}
		}
	}

	if(pstSession == NULL)
	{
		for(i = stRange.nStartRange ; i < stRange.nEndRange ; i++)
		{
			if(s_pstCxSession[i]->hStbHandle == HANDLE_NULL)
			{
				pstSession = s_pstCxSession[i];
				break;
			}
		}
	}

	if(pstSession == NULL)
	{
		CX_SEM_Release(s_ulCxSessionSemId);
		return ERR_FAIL;
	}

	pstSession->hStbHandle = hStbHandle;
	if(bNotFound == TRUE)
	{
		if (pstSession->pstPmtInfo)
		{
			CX_MEM_Free(pstSession->pstPmtInfo);
			pstSession->pstPmtInfo = NULL;
		}
#if defined(CONFIG_MW_MM_PVR)
		pstSession->bCaPinPass = FALSE;
#endif
	}
#if defined(CONFIG_MW_CAS_CONAX_PRETEST)
	else
	{
  #if defined(CONFIG_MW_MM_PVR)
		if(eActionType == eCAS_ActionType_Tsr_Rec)
		{
			if(pstSession->bCaPinPass)
			{
				CXSC_ResetSession(pstSession->nScSlot, pstSession->nSessionIdx);
				pstSession->bCaPinPass = FALSE;
			}
		}
  #endif
	}
#endif

	switch(eActionType)
	{
		case eCAS_ActionType_Tsr_Play:
			pstMainSession = local_cxsession_GetMainHandle();
			if(pstMainSession != NULL)
			{
				pstSession->bCaPinPass = pstMainSession->bCaPinPass;
			}
			break;

		default:
			break;
	}

	CX_SEM_Release(s_ulCxSessionSemId);

	HxLOG_Info("stbHandle(0x%X) ssIdx(%d) \n", hStbHandle, i);

	LEAVE_FUNCTION;
	return ERR_OK;
}

// bNextSvcExist; 모를때는 TRUE로 준다.
HERROR CXSESSION_StopSvc(Handle_t hStbHandle, HBOOL bNextSvcExist)
{
	HINT32	nIdx;
	SvcCas_ActionType_e eActionType;
	HUINT16 usBackupUniqueId;

	ENTER_FUNCTION;

	CX_SEM_Get(s_ulCxSessionSemId);

	nIdx = local_cxsession_FindIndexByStbHandle(hStbHandle);

	if (nIdx < 0)
	{
		CX_SEM_Release(s_ulCxSessionSemId);
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	HxLOG_Info("stbHandle(0x%X) \n", hStbHandle);

	// session에 해당하는 stream을 제거하면 link된 ecm들은 내부적으로 자동으로 정리
	CXSTREAM_DeleteAll((Handle_t)s_pstCxSession[nIdx]);

#if defined(CONFIG_SUPPORT_FCC)
#else
	//## UI에 stop svc 알려줌... ui 정리 (ecm으로 시작된 모든 ui clear)
	CXUI_SessionStop((Handle_t)s_pstCxSession[nIdx]);
#endif

	// EsPidInfo 초기화
	CX_memset(&s_pstCxSession[nIdx]->stEsPidInfo, 0, sizeof(CxEsPidInfo_t));

	s_pstCxSession[nIdx]->hStbHandle = HANDLE_NULL;
	s_pstCxSession[nIdx]->bSelected = FALSE;

	if (s_pstCxSession[nIdx]->pstPmtInfo)
	{
		CX_MEM_Free(s_pstCxSession[nIdx]->pstPmtInfo);
		s_pstCxSession[nIdx]->pstPmtInfo = NULL;
	}

#if defined(CONFIG_MW_CAS_CONAX_URI)
	if(s_pstCxSession[nIdx]->ulUriTimerId != VK_TIMERID_NULL)
	{
		VK_TIMER_Cancel(s_pstCxSession[nIdx]->ulUriTimerId);
		s_pstCxSession[nIdx]->ulUriTimerId = VK_TIMERID_NULL;
	}

	CX_memset(&s_pstCxSession[nIdx]->stUri, 0xff, sizeof(DxCopyrightControl_ConaxDrmInfo_t));
#endif

	eActionType = CXACT_GetActionTypeFromStbHandle(hStbHandle);

	switch(eActionType)
	{
		case eCAS_ActionType_Playback:
			s_pstCxSession[nIdx]->bCaPinPass = FALSE;
			break;

		case eCAS_ActionType_Recording:
			if(s_pstCxSession[nIdx]->eState == eCxSession_Used)
			{
				CXSC_ResetSession(s_pstCxSession[nIdx]->nScSlot, s_pstCxSession[nIdx]->nSessionIdx);
				s_pstCxSession[nIdx]->eState = eCxSession_Ready;
			}
			break;

		default:
			break;
	}

	switch(eActionType)
	{
		case eCAS_ActionType_Playback:
		case eCAS_ActionType_Tsr_Play:
			// SMARTCARD SESSION을 이용하지 않으며 CAT handling도 하지 않는다.
			// CAT handling도 하지 않는다...
			break;

		default:
			s_pstCxSession[nIdx]->hPrevStbHandle = hStbHandle;

			usBackupUniqueId = s_pstCxSession[nIdx]->usUniqueTsId;
			if (bNextSvcExist == FALSE)
			{
				// 다음 svc가 없으면 해당 TP는 더이상 사용하지 않는 것으로 간주
				local_cxsession_DeleteEmm(s_pstCxSession[nIdx], usBackupUniqueId);
			}

			local_cxsession_RestartEmm(s_pstCxSession[nIdx], usBackupUniqueId);

			if(s_pstCxSession[nIdx]->pucCat)
			{
				CX_MEM_Free(s_pstCxSession[nIdx]->pucCat);
				s_pstCxSession[nIdx]->pucCat = NULL;
			}
			break;
	}

	CX_SEM_Release(s_ulCxSessionSemId);

	LEAVE_FUNCTION;
	return ERR_OK;
}

HERROR CXSESSION_SetPid(Handle_t hStbHandle, CxEsPidInfo_t* pstEsPidInfo)
{
	CxSession_t* 					pSession;
	//	HUINT32 		ulIdx;
	HUINT16		usPid, usNewPid;
	int			i;

	ENTER_FUNCTION;

	if(pstEsPidInfo == NULL)
	{
		HxLOG_Error("pstEsPidInfo NULL\n");
		return ERR_FAIL;
	}

	CX_SEM_Get(s_ulCxSessionSemId);

	pSession = local_cxsession_GetSession(hStbHandle);
	if (pSession == NULL)
	{
		CX_SEM_Release(s_ulCxSessionSemId);
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	// 기존에 들고 있던 pid 비교
	for (i = 0; i < pSession->stEsPidInfo.ulPidNum; i++)
	{
		usPid = pSession->stEsPidInfo.usPid[i];
		usNewPid = pstEsPidInfo->usPid[i];

		if (usPid != usNewPid)
		{
			//pid가 바뀌었으니 기존 거는 지우고, 새로운 것을 등록
			if (usPid != PID_NULL && usPid != 0x0)
			{
				CXSTREAM_SetPid((Handle_t)pSession, usPid, CX_PID_DELETE);
			}

			if (usNewPid != PID_NULL && usNewPid != 0x0)
			{
				CXSTREAM_SetPid((Handle_t)pSession, usNewPid, CX_PID_ADD);
			}

			pSession->stEsPidInfo.usPid[i] = usNewPid;
		}
	}

	// 새로 추가된 pid
	for (; i < pstEsPidInfo->ulPidNum; i++)
	{
		usNewPid = pstEsPidInfo->usPid[i];

		if (usNewPid != PID_NULL && usNewPid != 0x0)
		{
			CXSTREAM_SetPid((Handle_t)pSession, usNewPid, CX_PID_ADD);
		}

		// 새로운 pid를 기록
		pSession->stEsPidInfo.usPid[i] = usNewPid;
	}

	// 새로운 pidNum 기록
	pSession->stEsPidInfo.ulPidNum = pstEsPidInfo->ulPidNum;

	CX_MEM_Free(pstEsPidInfo);

	CX_SEM_Release(s_ulCxSessionSemId);
	LEAVE_FUNCTION;
	return ERR_OK;
}

HERROR CXSESSION_UpdatePmt(Handle_t hStbHandle, SvcCas_PmtInfo_t* pstPmtInfo)
{
	CxSession_t* 					pSession;
	HBOOL		bScrambled		= FALSE;

	ENTER_FUNCTION;

	if (pstPmtInfo == NULL)
	{
		LEAVE_FUNCTION;
		HxLOG_Error("pstPmtInfo NULL\n");
		return ERR_FAIL;
	}

	CX_SEM_Get(s_ulCxSessionSemId);

	pSession = local_cxsession_GetSession(hStbHandle);
	if (pSession == NULL)
	{
		CX_SEM_Release(s_ulCxSessionSemId);
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	// pmt 저장
	if (pSession->pstPmtInfo == NULL)
	{
		pSession->pstPmtInfo = CX_MEM_Alloc(sizeof(SvcCas_PmtInfo_t));
	}

	if (pSession->pstPmtInfo != NULL)
	{
		CX_memcpy(pSession->pstPmtInfo, pstPmtInfo, sizeof(SvcCas_PmtInfo_t));
	}

	//pmt 내용 분석 하셔서 stream table, ecm table 생성 해주세요.
	if (CXSTREAM_ParsePmt((Handle_t)pSession) == ERR_OK)
	{
		if (CXSTREAM_IsScrambledSvc((Handle_t)pSession, &bScrambled) == ERR_OK)
		{
			/*
				Session Reset Rule

				= CONAX SPEC
					- CA Service -> FTA Service로 Service 변경시...
					- Smartcard Session을 더이상 사용하지 않을 시...
				= HUMAX RULE
					- 마지막으로 CaPin이 Override된 Service가 동일한 session으로 할당되었을 때...
			*/

			if (bScrambled == TRUE)
			{
				//scrambled svc
				pSession->eState = eCxSession_Used;
			}
			else
			{
				// free svc
				if (pSession->eState == eCxSession_Used)
				{
					// scrambed -> free상황이므로 reset session 필요함
					CXSC_ResetSession(pSession->nScSlot, pSession->nSessionIdx);
				}
				pSession->eState = eCxSession_Ready;
			}
		}

		// ecm filter start (해보는데, s/c가 없거나 used 선택이 안되면 filter start가 안될 거임.. try의 개념)
		CXSTREAM_StartAllEcmFilter((Handle_t)pSession);
	}

	if (bScrambled || CXSTREAM_IsConaxStream((Handle_t)pSession))
	{
		CXUI_SessionStart((Handle_t)pSession);
#if defined(CONFIG_MW_CAS_CONAX_URI)
		local_cxsession_UriWait(pSession);
#endif
	}

	CX_SEM_Release(s_ulCxSessionSemId);
	LEAVE_FUNCTION;

	return ERR_OK;
}

HERROR CXSESSION_UpdateCat(Handle_t hStbHandle, HUINT16 usUniqueTsId, HUINT8* pucCat)
{
	CxSession_t* 					pSession;
	HUINT16		usBackupUniqueTsId;

	ENTER_FUNCTION;

	CX_SEM_Get(s_ulCxSessionSemId);
	pSession = local_cxsession_GetSession(hStbHandle);
	if (pSession == NULL)
	{
		CX_SEM_Release(s_ulCxSessionSemId);
		CX_MEM_Free(pucCat);
		return ERR_FAIL;
	}

	usBackupUniqueTsId = pSession->usUniqueTsId;
	if(pSession->usUniqueTsId != usUniqueTsId)
	{
		// 기존 저장된 uniqueTsId와 다르면 기존 ts id는 닫도록 요청
		local_cxsession_DeleteEmm(pSession, usBackupUniqueTsId);
	}

	if(pSession->pucCat != NULL)
	{
		CX_MEM_Free(pSession->pucCat);
	}

	pSession->pucCat = pucCat;
	pSession->usUniqueTsId = usUniqueTsId;

	CXEMM_UpdateCat((Handle_t)pSession, usUniqueTsId, pSession->pucCat);

	CX_SEM_Release(s_ulCxSessionSemId);

	LEAVE_FUNCTION;
	return ERR_OK;
}

#if defined(CONFIG_MW_MM_DELAYED_REC)
HERROR CXSESSION_MoveSvc(Handle_t hStbHandle)
{
	CxSession_t *pstTsrSession = NULL, *pstRecSession = NULL;
	CxSessionRange_t stRange;
	int i, j;

	CX_SEM_Get(s_ulCxSessionSemId);

	local_cxsession_GetSessionRange(eCAS_ActionType_Tsr_Rec, &stRange);

	for(i = stRange.nStartRange ; i < stRange.nEndRange ; i++)
	{
		if(s_pstCxSession[i]->hStbHandle == hStbHandle)
		{
			pstTsrSession = s_pstCxSession[i];
			break;
		}
	}

	local_cxsession_GetSessionRange(eCAS_ActionType_Recording, &stRange);

	for(j = stRange.nStartRange ; j < stRange.nEndRange ; j++)
	{
		if(s_pstCxSession[j]->hStbHandle == HANDLE_NULL)
		{
			pstRecSession = s_pstCxSession[j];
			break;
		}
	}

	if(!pstTsrSession || !pstRecSession)
	{
		HxLOG_Error("pstTsrSession (0x%p), pstRecSession (0x%p)\n", pstTsrSession, pstRecSession);
		CX_SEM_Release(s_ulCxSessionSemId);
		return ERR_FAIL;
	}

	if(pstTsrSession->eState == eCxSession_Used)
	{
		CXSC_ResetSession(pstTsrSession->nScSlot, pstTsrSession->nSessionIdx);
		pstTsrSession->eState = eCxSession_Ready;
	}

	s_pstCxSession[i] = pstRecSession;
	pstRecSession->nSessionIdx = i;

	s_pstCxSession[j] = pstTsrSession;
	pstTsrSession->nSessionIdx = j;

	CX_SEM_Release(s_ulCxSessionSemId);

	return ERR_OK;
}
#endif

#if defined(CONFIG_SUPPORT_FCC)
HERROR CXSESSION_SetMainService(Handle_t hMainStbHandle, Handle_t hPrevStbHandle)
{
	CxSession_t *pstSession;
	CxSessionRange_t stRange;
	int i;

	if(hMainStbHandle == HANDLE_NULL)
	{
		HxLOG_Error("hMainStbHandle HANDLE_NULL\n");
		return ERR_FAIL;
	}

	CX_SEM_Get(s_ulCxSessionSemId);

	local_cxsession_GetSessionRange(eCAS_ActionType_Live, &stRange);

	for(i = stRange.nStartRange ; i < stRange.nEndRange ; i++)
	{
		s_pstCxSession[i]->bMainSession = FALSE;
		if(s_pstCxSession[i]->hStbHandle == hMainStbHandle)
		{
			s_pstCxSession[i]->bMainSession = TRUE;
		}
	}

	pstSession = local_cxsession_GetSession(hPrevStbHandle);
	if(pstSession == NULL)
	{
		HxLOG_Error("pstSession NULL\n");
		CX_SEM_Release(s_ulCxSessionSemId);
		return ERR_FAIL;
	}

	// MR lock service인 경우 다시 이전채널로 돌아갈 때 PIN을 물어야 한다.
	if(pstSession->bCaPinPass)
	{
		CXSC_ResetSession(pstSession->nScSlot, pstSession->nSessionIdx);
		pstSession->bCaPinPass = FALSE;
	}

	/*
		FCC 모델인 경우 CW 유효시간내에 previous view로 다시 전환시 AV가 잠시 보이는 현상이 발생할 수 있기에
		ECM filter를 다시 걸어서 PIN 등을 다시 받도록 한다.
	*/

	CXCORE_SendMsg(eCxMsg_Ecm_FilterRestart, (Handle_t)pstSession, (HUINT32)0, (HUINT32)0);

	CX_SEM_Release(s_ulCxSessionSemId);

	return ERR_OK;
}
#endif

HERROR CXSESSION_SetCaPinPass(Handle_t hSession, HBOOL bCaPinPass)
{
	CxSession_t *pstSession;

	CX_SEM_Get(s_ulCxSessionSemId);

	pstSession = local_cxsession_GetSessionByHandle(hSession);
	if(pstSession == NULL)
	{
		HxLOG_Error("pstSession NULL\n");
		CX_SEM_Release(s_ulCxSessionSemId);
		return ERR_FAIL;
	}

	pstSession->bCaPinPass = bCaPinPass;

	CX_SEM_Release(s_ulCxSessionSemId);

	return ERR_OK;
}

HBOOL CXSESSION_GetCaPinPass(Handle_t hSession)
{
	CxSession_t *pstSession;
	HBOOL bCaPinPass = FALSE;

	CX_SEM_Get(s_ulCxSessionSemId);

	pstSession = local_cxsession_GetSessionByHandle(hSession);
	if(pstSession == NULL)
	{
		HxLOG_Error("pstSession NULL\n");
		CX_SEM_Release(s_ulCxSessionSemId);
		return bCaPinPass;
	}

	bCaPinPass = pstSession->bCaPinPass;

	CX_SEM_Release(s_ulCxSessionSemId);

	return bCaPinPass;
}

HERROR CXAPI_StartSvc(Handle_t hStbHandle)
{
	ENTER_FUNCTION;

#if defined(CX_USE_DIRECT_CALL)
	CXSESSION_StartSvc(hStbHandle);
#else
	CXCORE_SendMsg(eCxMsg_Svc_Start, hStbHandle, (HUINT32)0, (HUINT32)0);
#endif

	LEAVE_FUNCTION;
	return ERR_OK;
}

HERROR CXAPI_StopSvc(Handle_t hStbHandle, HBOOL bNextSvcExist)
{
	ENTER_FUNCTION;

#if defined(CX_USE_DIRECT_CALL)
	CXSESSION_StopSvc(hStbHandle, bNextSvcExist);
#else
	CXCORE_SendMsg(eCxMsg_Svc_Stop, hStbHandle, (HUINT32)bNextSvcExist, (HUINT32)0);
#endif

	LEAVE_FUNCTION;
	return ERR_OK;
}

// pstEsPidInfo에서 EsType은 사용하지 않는다. 대신, 각 esType은 항상 같은 index에 들어와야 한다.
// 예를 들어, (V, A1, A2, A3, S, T)로 쓰기로 결정했다면 항상 이러한 순서와 갯수를 맞춰서 들어와야 한다.
// 하나의 pid만 바뀌었다고 하나만 보내면 다른 pid는 사용하지 않는 것으로 간주한다.
HERROR CXAPI_SetPid(Handle_t hStbHandle, CxEsPidInfo_t* pstEsPidInfo)
{
	ENTER_FUNCTION;

#if defined(CX_USE_DIRECT_CALL)
	CXSESSION_SetPid(hStbHandle, pstEsPidInfo);
#else
	CXCORE_SendMsg(eCxMsg_Svc_SetPid, hStbHandle, (HUINT32)pstEsPidInfo, (HUINT32)0);
#endif

	LEAVE_FUNCTION;
	return ERR_OK;
}

HERROR CXAPI_UpdatePmt(Handle_t hStbHandle, SvcCas_PmtInfo_t* pstPmtInfo)
{
	ENTER_FUNCTION;

#if defined(CX_USE_DIRECT_CALL)
	CXSESSION_UpdatePmt(hStbHandle, pstPmtInfo);
#else
	CXCORE_SendMsg(eCxMsg_Svc_UpdatePmt, hStbHandle, (HUINT32)pstPmtInfo, (HUINT32)0);
#endif

	LEAVE_FUNCTION;
	return ERR_OK;
}

HERROR CXAPI_UpdateCat(Handle_t hStbHandle, HUINT16 usUniqueTsId, HUINT8* pucCat)
{
	HUINT16	usLen;
	HUINT8* pucCatRaw;

	ENTER_FUNCTION;

	if (pucCat == NULL)
	{
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	usLen = get12bit(pucCat + 1) + 3;

	pucCatRaw = (HUINT8 *)CX_MEM_Alloc(usLen);
	if (pucCatRaw == NULL)
	{
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	CX_memcpy(pucCatRaw, pucCat, usLen);

#if defined(CX_USE_DIRECT_CALL)
	CXSESSION_UpdateCat(hStbHandle, usUniqueTsId, pucCatRaw);
#else
	CXCORE_SendMsg(eCxMsg_Svc_UpdateCat, hStbHandle, (HUINT32)usUniqueTsId, (HUINT32)pucCatRaw);
#endif

	LEAVE_FUNCTION;

	return ERR_OK;
}

HERROR CXAPI_Select(Handle_t hStbHandle, HBOOL bSelect)
{
	ENTER_FUNCTION;

#if defined(CX_USE_DIRECT_CALL)
	CXSESSION_Select(hStbHandle, bSelect);
#else
	CXCORE_SendMsg(eCxMsg_Svc_Select, hStbHandle, (HUINT32)bSelect, (HUINT32)0);
#endif

	LEAVE_FUNCTION;
	return ERR_OK;
}

#if defined(CONFIG_MW_MM_DELAYED_REC)
HERROR CXAPI_MoveSvc(Handle_t hStbHandle)
{
#if defined(CX_USE_DIRECT_CALL)
	CXSESSION_MoveSvc(hStbHandle, bSelect);
#else
	CXCORE_SendMsg(eCxMsg_Svc_Move, hStbHandle, (HUINT32)0, (HUINT32)0);
#endif

	return ERR_OK;
}
#endif

#if defined(CONFIG_SUPPORT_FCC)
HERROR CXAPI_SetMainHandle(Handle_t hMainStbHandle, Handle_t hPrevStbHandle)
{
#if defined(CX_USE_DIRECT_CALL)
	CXSESSION_SetMainService(hMainStbHandle, hPrevStbHandle);
#else
	CXCORE_SendMsg(eCxMsg_Svc_SetMainView, hMainStbHandle, (HUINT32)hPrevStbHandle, 0);
#endif

	return ERR_OK;
}
#endif

#define SMARTCARD______________________________________________

// reset session을 하고 나서 호출된 것으로 가정함
HERROR CXSESSION_ScInserted(HINT32 nScSlot)
{
	int		i;
	HINT32	nCnt;
	HINT32	nSessionCount;

	ENTER_FUNCTION;

	CX_SEM_Get(s_ulCxSessionSemId);

#if 0	// SC recovery를 logic을 수정하면서 아래 부분이 필요 없어지는 코드다.
	// 이미 등록된 smartcard 아닌가 확인
	for (i = 0; i < CX_MAX_NUM_OF_SESSION; i++)
	{
		if (s_pstCxSession[i]->nScSlot == nScSlot)
		{
			CXEMM_SendCat(nScSlot);// s/c force reset 등에 의해서 reset 될 경우 cat만 보내보자...
			CX_SEM_Release(s_ulCxSessionSemId);
			LEAVE_FUNCTION;
			return ERR_FAIL;
		}
	}
#endif

	nSessionCount = CXSC_GetSessionCount(nScSlot);
	if (nSessionCount == CX_ERR_INVALID_PARAMETER)
	{
		CX_SEM_Release(s_ulCxSessionSemId);
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	// 일단 그냥 앞에서부터 늘어 놓는다
	for (i = 0, nCnt = 0; i < CX_MAX_NUM_OF_SESSION && nCnt < nSessionCount; i++)
	{
		if (s_pstCxSession[i]->nScSlot == CX_SC_NONE)
		{
			s_pstCxSession[i]->nScSlot = nScSlot;
			s_pstCxSession[i]->nSessionIdx = nCnt++;
			s_pstCxSession[i]->eState = eCxSession_Ready;	// reset session을 하고 온 것으로 가정 !!

			switch(CXACT_GetActionTypeFromStbHandle(s_pstCxSession[i]->hStbHandle))
			{
				case eCAS_ActionType_Live:
				case eCAS_ActionType_Tsr_Rec:
				case eCAS_ActionType_Recording:
					CXEMM_UpdateCat((Handle_t)s_pstCxSession[i], s_pstCxSession[i]->usUniqueTsId, s_pstCxSession[i]->pucCat);
					local_cxsession_CheckPmt(s_pstCxSession[i]);
					break;

				case eCAS_ActionType_Playback:
				case eCAS_ActionType_Tsr_Play:
					CXAPI_PVR_ScInserted((Handle_t)s_pstCxSession[i]);
					break;

				default:
					break;
			}
		}
	}

	CX_SEM_Release(s_ulCxSessionSemId);

	if (nCnt < nSessionCount)
	{
		// 빈 slot이 없어서 등록할 수 없음
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	LEAVE_FUNCTION;
	return ERR_OK;
}

HERROR CXSESSION_ScExtracted(HINT32 nScSlot)
{
	int	i;

	ENTER_FUNCTION;

	CX_SEM_Get(s_ulCxSessionSemId);

	for (i = 0; i < CX_MAX_NUM_OF_SESSION; i++)
	{
		if (s_pstCxSession[i]->nScSlot == nScSlot)
		{
			// emm filter를 stop 시킴
			CXEMM_Delete(nScSlot);

			if (s_pstCxSession[i]->hStbHandle != HANDLE_NULL)
			{
				// ecm filter를 stop 시킴
				CXSTREAM_StopAllEcmFilter((Handle_t)s_pstCxSession[i]);
			}

			s_pstCxSession[i]->nScSlot = CX_SC_NONE;
			s_pstCxSession[i]->nSessionIdx = CX_INVALID_SESSION_NUM;
			s_pstCxSession[i]->eState = eCxSession_Init;
#if defined(CONFIG_MW_MM_PVR)
			s_pstCxSession[i]->bCaPinPass = FALSE;
#endif
		}
	}

	// emm queue 삭제
	CXEMM_QueueClear();

	// ecm queue 삭제
	CXECM_QueueClear();

	CX_SEM_Release(s_ulCxSessionSemId);

	LEAVE_FUNCTION;
	return ERR_OK;
}

// error때문에 reset 하고 난후의 처리
HERROR CXSESSION_ScErrorHandler(HINT32 nScSlot)
{
	int	i;

	ENTER_FUNCTION;

	CX_SEM_Get(s_ulCxSessionSemId);

	for (i = 0; i < CX_MAX_NUM_OF_SESSION; i++)
	{
		if (s_pstCxSession[i]->nScSlot == nScSlot)
		{
			// emm filter를 stop 시킴
			CXEMM_Delete(nScSlot);

			if (s_pstCxSession[i]->hStbHandle != HANDLE_NULL)
			{
				// ecm filter를 stop 시킴
				CXSTREAM_StopAllEcmFilter((Handle_t)s_pstCxSession[i]);
			}

			s_pstCxSession[i]->nScSlot = CX_SC_NONE;
			s_pstCxSession[i]->nSessionIdx = CX_INVALID_SESSION_NUM;
			s_pstCxSession[i]->eState = eCxSession_Init;
		}
	}

	CX_SEM_Release(s_ulCxSessionSemId);

	//## 이때도 queue를 다 clear하고 filter restart하고 해야할까?
	// emm / ecm queue 삭제
	CXEMM_QueueClear();
	CXECM_QueueClear();

	LEAVE_FUNCTION;
	return ERR_OK;
}

#if defined(CONFIG_MW_CAS_CONAX_URI)
HERROR CXSESSION_UriWait(Handle_t hSession)
{
	CxSession_t *pstSession;

	CX_SEM_Get(s_ulCxSessionSemId);

	pstSession = local_cxsession_GetSessionByHandle(hSession);
	if(pstSession == NULL)
	{
		CX_SEM_Release(s_ulCxSessionSemId);
		return ERR_FAIL;
	}

	if(local_cxsession_UriWait(pstSession) != ERR_OK)
	{
		CX_SEM_Release(s_ulCxSessionSemId);
		return ERR_FAIL;
	}

	CX_SEM_Release(s_ulCxSessionSemId);

	return ERR_OK;
}

HERROR CXSESSION_UriTimeout(Handle_t hSession)
{
	CxSession_t *pstSession;
	HUINT8 ucScMrLevel = CX_MATURITY_RATE_OFF;

	CX_SEM_Get(s_ulCxSessionSemId);

	pstSession = local_cxsession_GetSessionByHandle(hSession);
	if(pstSession == NULL)
	{
		HxLOG_Error("pstSession NULL\n");
		CX_SEM_Release(s_ulCxSessionSemId);
		return ERR_FAIL;
	}

	CXECM_GetAccessStatus(CXECM_GetHandleBySession(hSession), &ucScMrLevel);
	CXSC_GetDefaultUri(&pstSession->stUri);

	pstSession->stUri.eMR = CXSC_ConvertScMrToUriMr(ucScMrLevel);

	local_cxsession_ApplyUri(pstSession);

	CX_SEM_Release(s_ulCxSessionSemId);

	return ERR_OK;
}

HERROR CXSESSION_UriReceived(Handle_t hSession, DxCopyrightControl_ConaxDrmInfo_t *pstDrmInfo)
{
	CxSession_t *pstSession;

	CX_SEM_Get(s_ulCxSessionSemId);

	pstSession = local_cxsession_GetSessionByHandle(hSession);
	if(pstSession == NULL)
	{
		HxLOG_Error("pstSession NULL\n");
		CX_SEM_Release(s_ulCxSessionSemId);
		return ERR_FAIL;
	}

	if(pstSession->ulUriTimerId != VK_TIMERID_NULL)
	{
		VK_TIMER_Cancel(pstSession->ulUriTimerId);
		pstSession->ulUriTimerId = VK_TIMERID_NULL;
	}

	if(CX_memcmp(&pstSession->stUri, pstDrmInfo, sizeof(DxCopyrightControl_ConaxDrmInfo_t)) == 0)
	{
		CX_SEM_Release(s_ulCxSessionSemId);
		return ERR_OK;
	}

	CX_memcpy(&pstSession->stUri, pstDrmInfo, sizeof(DxCopyrightControl_ConaxDrmInfo_t));

	local_cxsession_ApplyUri(pstSession);

	CX_SEM_Release(s_ulCxSessionSemId);

	return ERR_OK;
}
#endif

#if defined(CONFIG_DEBUG)
STATIC HUINT8* local_cxsession_DEBUG_GetActionType(SvcCas_ActionType_e eActionType)
{
	switch (eActionType)
	{
		ENUM_TO_STR(eCAS_ActionType_None)
		ENUM_TO_STR(eCAS_ActionType_Live)
		ENUM_TO_STR(eCAS_ActionType_Playback)
		ENUM_TO_STR(eCAS_ActionType_Recording)
		ENUM_TO_STR(eCAS_ActionType_Descramble_a_File)
		ENUM_TO_STR(eCAS_ActionType_Tsr_Play)
		ENUM_TO_STR(eCAS_ActionType_Tsr_Rec)
		ENUM_TO_STR(eCAS_ActionType_Data)
		ENUM_TO_STR(eCAS_ActionType_EngineeringTS)
		ENUM_TO_STR(eCAS_ActionType_RfVod)
		ENUM_TO_STR(eCAS_ActionType_Max)
	}

	return "Unknown";

}

STATIC HUINT8* local_cxsession_DEBUG_GetSessionState(CxSessionState_t eState)
{
	switch (eState)
	{
		ENUM_TO_STR(eCxSession_Init)
		ENUM_TO_STR(eCxSession_Ready)
		ENUM_TO_STR(eCxSession_Used)
	}

	return "Unknown";
}

STATIC HBOOL local_cxsession_GetMainAction(Handle_t hStbHandle)
{
	int i;

	for (i = 0; i < CX_MAX_NUM_OF_SESSION; i++)
	{
		if(hStbHandle == HANDLE_NULL)
			continue;

		if(CXACT_GetActionTypeFromStbHandle(hStbHandle) != eCAS_ActionType_Live)
			continue;

		if(CXACT_GetActionHandle(HANDLE_NULL) == CXACT_GetActionHandle(hStbHandle))
			return TRUE;
	}

	return FALSE;
}

void CMD_CXSESSION_PrintSessionContext(void)
{
	int i;

	for (i = 0; i < CX_MAX_NUM_OF_SESSION; i++)
	{
		HxLOG_Warning(HxANSI_COLOR_GREEN("CONAX SESSION (%d)\n"), i);

		if(local_cxsession_GetMainAction(s_pstCxSession[i]->hStbHandle))
			HxLOG_Warning(HxANSI_COLOR_YELLOW("\t hAction (0x%x, %s)\n"), CXACT_GetActionHandle(s_pstCxSession[i]->hStbHandle), local_cxsession_DEBUG_GetActionType(CXACT_GetActionTypeFromStbHandle(s_pstCxSession[i]->hStbHandle)));
		else
			HxLOG_Warning("\t hAction (0x%x, %s)\n", CXACT_GetActionHandle(s_pstCxSession[i]->hStbHandle), local_cxsession_DEBUG_GetActionType(CXACT_GetActionTypeFromStbHandle(s_pstCxSession[i]->hStbHandle)));

		HxLOG_Warning("\t hStbHandle (0x%x)\n", s_pstCxSession[i]->hStbHandle);
		HxLOG_Warning("\t hPrevStbHandle (0x%x)\n", s_pstCxSession[i]->hPrevStbHandle);
#if defined(CONFIG_SUPPORT_FCC)
		HxLOG_Warning("\t bMainSession (%d)\n", s_pstCxSession[i]->bMainSession);
#endif
		if(s_pstCxSession[i]->hStbHandle == HANDLE_NULL)
			continue;

		HxLOG_Warning("\t nSessionIdx (%d)\n", s_pstCxSession[i]->nSessionIdx);
		HxLOG_Warning("\t eState (%s)\n", local_cxsession_DEBUG_GetSessionState(s_pstCxSession[i]->eState));
		HxLOG_Warning("\t nScSlot (%d)\n", s_pstCxSession[i]->nScSlot);
		HxLOG_Warning("\t bSelected (%s)\n", s_pstCxSession[i]->bSelected ? "TRUE" : "FALSE");
		HxLOG_Warning("\t usUniqueTsId (%d)\n", s_pstCxSession[i]->usUniqueTsId);
		HxLOG_Warning("\t pucCat (%p)\n", s_pstCxSession[i]->pucCat);
		HxLOG_Warning("\t pstPmtInfo (%p)\n", s_pstCxSession[i]->pstPmtInfo);
#if defined(CONFIG_MW_MM_PVR)
		HxLOG_Warning("\t bCaPinPass (%d)\n", s_pstCxSession[i]->bCaPinPass);
#endif
	}
}
#endif
/*********************** End of File ******************************/

