/*
	Viaccess Promgram Information API
		Driver API Manual ACS 3.0 - Doc. Ref. 2217, p.75
*/

/********************************************************************
*	Including Headers
********************************************************************/
#include "va_def.h"

#include "vamgr_main.h"

#include "va_pi_api.h"
#include "va_dscr_api.h"

#include "va_util.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
//#define DEBUG_PID_CHANGE
//#define DEBUG_PRINT_DESCRIPTOR_AT_PMT
//#define DEBUG_PRINT_DESCRIPTOR_AT_CAT


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/



#define VA_UNKNOWN			0xFFFFFFFF

// Viaccess library 에서 호출하는 manufacturer API 에 semaphore guard 를 한다.
// Viaccess Driver API Manual 의 API function calls in manufacturer and ACS threads 설명을 보면 이런 처리는 불필요하지만 Viaccess library 의 PI 모듈이 이렇게 동작하지 않는 것으로 생각된다.
// 아래 define 은 Viaccess library 에서 호출하는 함수에서만 사용하도록 하고 자체적으로 호출하는 함수들에서는 사용하지 않도록 한다.
#define USE_SEMA_TO_ALL_PI_FUNCS_CALLED_BY_VA

/********************************************************************
*	Global variables
********************************************************************/
VA_PI_AcsInstance *pstVaAcsInstance[VA_PI_MAX_ACS_INSTANCE];

UINT32 gVaPiSemId;

/********************************************************************
*	Static functions Prototypes
*	The following functions will be used as inner functions
********************************************************************/
#if defined(CONFIG_DEBUG)
#ifdef __VIACCESS50__
static HUINT8 *VA_PI_INT_GetAcsModeString(tVA_CTRL_AcsMode eMode)
#else
static HUINT8 *VA_PI_INT_GetAcsModeString(tVA_PI_AcsMode eMode)
#endif
{
	switch(eMode)
	{
	case eLIVE:													return "eLIVE";
	case ePERSISTENT_RECORD:									return "ePERSISTENT_RECORD";
	case ePERSISTENT_PLAYBACK:									return "ePERSISTENT_PLAYBACK";
	case eTIMESHIFT_RECORD:										return "eTIMESHIFT_RECORD";
	case eTIMESHIFT_PLAYBACK:									return "eTIMESHIFT_PLAYBACK";
	case eLIVE_AND_PERSISTENT_RECORD:							return "eLIVE_AND_PERSISTENT_RECORD";
	case eLIVE_AND_TIMESHIFT_RECORD:							return "eLIVE_AND_TIMESHIFT_RECORD";
	case eLIVE_AND_TIMESHIFT_RECORD_AND_PERSISTENT_RECORD:	return "eLIVE_AND_TIMESHIFT_RECORD_AND_PERSISTENT_RECORD";
	case eTIMESHIFT_RECORD_AND_PERSISTENT_RECORD:				return "eTIMESHIFT_RECORD_AND_PERSISTENT_RECORD";
	case eTIMESHIFT_PLAYBACK_AND_PERSISTENT_RECORD:			return "eTIMESHIFT_PLAYBACK_AND_PERSISTENT_RECORD";
	default:		break;
	}

	return "UNKNOWN MODE";
}

static HUINT8 *VA_PI_INT_GetProgramStateString(VA_PI_PROGRAM_STATE_ENUM nProgramState)
{
	switch(nProgramState)
	{
	case VA_PI_PROGRAM_SWITCH_OFF:	return "SWITCH_OFF";
	case VA_PI_PROGRAM_SWITCH_ON:	return "SWITCH_ON";
	case VA_PI_PROGRAM_UPDATE:		return "UPDATE";
	case VA_PI_PROGRAM_READY:		return "READY";
	default:		break;
	}

	return "UNKNOWN STATE";
}
#endif

static int local_va_pi_GetStreamTypeFromStreamHandle(UINT32 lStbStreamHandle);

VA_UI_INFO *VA_PI_GetUiInfoByAcsId(HUINT32 ulAcsIdx) // 다른 함수에 의해 데이터가 overwrite 될 수 있는 pointer 를 리턴함으로 인해서 semaphore guard 를 하더라도 문제가 될 소지가 있음.
{
	VA_UI_INFO *pstUiInfo = NULL;

	VA_ASSERT(gVaPiSemId);
	VA_SEM_Get(gVaPiSemId);

	if (ulAcsIdx < VA_PI_MAX_ACS_INSTANCE)
	{
		pstUiInfo = &pstVaAcsInstance[ulAcsIdx]->stUiInfo;
	}
	else
	{
		HxLOG_Error("Index wrong (%d)\n", ulAcsIdx);
	}

	VA_SEM_Release(gVaPiSemId);

	return pstUiInfo;
}

VA_ES_TYPE VA_PI_GetStreamStateForUi(HUINT32 ulAcsIdx)
{
	VA_PI_AcsInstance *pAcs;
	VA_ES_TYPE eVaEsState = VA_ES_NONE;
	EsType_t eEsType;
	int i, nEs;
	HUINT32 ulPidNum;
	SvcCas_PidInfo_t *pPidInfo;
	UINT32 ulEsNum;
	VA_PI_Stream *pEs;
	HBOOL bPidSelected, bRecordAcs = FALSE;

	// dwStbStreamHandle doesn't correspond to an added stream
	if (ulAcsIdx < VA_PI_MAX_ACS_INSTANCE)
	{
		VA_ASSERT(gVaPiSemId);
		VA_SEM_Get(gVaPiSemId);

		pAcs = pstVaAcsInstance[ulAcsIdx];

#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
		bRecordAcs = VA_PI_Adapt_IsRecord(ulAcsIdx);
#endif

		ulPidNum = pAcs->stPidInfo.ulPidNum;
		pPidInfo = pAcs->stPidInfo.stPidInfo;

		ulEsNum = pAcs->stStreamInfo.ulEsNum;
		pEs = pAcs->stStreamInfo.astEs;

		for (nEs = 0; nEs < ulEsNum; nEs++)
		{
			if (bRecordAcs)
			{
				bPidSelected= TRUE;
			}
			else
			{
				bPidSelected = FALSE;

				for (i = 0; i < ulPidNum; i++)
				{
					if (pEs[nEs].usPid == pPidInfo[i].usPid)
					{
						bPidSelected = TRUE;
						break;
					}
				}
			}
			HxLOG_Info("[%d] stream %x's , PidSelected = %d\n", nEs, pEs[nEs].usPid, bPidSelected);

			// 선택된 pid만 처리
#ifdef __VIACCESS50__
			if (bPidSelected &&
				(pEs[nEs].nDesState== eVIACCESS_NOT_APPLICABLE ||
				pEs[nEs].nDesState == eVIACCESS_NOT_DESCRAMBLING))
#else
			if (bPidSelected &&
				(pEs[nEs].nState == eVIACCESS_NOT_APPLICABLE ||
				pEs[nEs].nState == eVIACCESS_NOT_DESCRAMBLING))
#endif
			{
				// descramble_ko 인 stream만 표시

				eEsType = local_va_pi_GetStreamTypeFromStreamHandle(pEs[nEs].ulStbStreamHandle); // pEs[nEs].esType 를 바로 처리하면 안되는 건가?

				if (eEsType == eEsType_Video)
				{
					if (eVaEsState == VA_ES_NONE)
						eVaEsState = VA_ES_VIDEO;
					if (eVaEsState == VA_ES_AUDIO)
						eVaEsState = VA_ES_ALL;
				}
				else if (eEsType == eEsType_Audio)
				{
					if (eVaEsState == VA_ES_NONE)
						eVaEsState = VA_ES_AUDIO;
					if (eVaEsState == VA_ES_VIDEO)
						eVaEsState = VA_ES_ALL;
				}
			}
		}
		VA_SEM_Release(gVaPiSemId);
	}
	else
	{
		HxLOG_Error("Index wrong (%d)\n", ulAcsIdx);
	}

	return eVaEsState;
}

static HUINT32 local_va_pi_GetStreamHandle(HUINT8 ucAcsIdx, HUINT8 ucESIndex)
{
	static unsigned int nSequence = 0;

	if (nSequence == 0xFFFF)
		nSequence = 0;
	else
		nSequence++;

	return ((nSequence & 0xFFFF) << 16) | ((ucAcsIdx & 0xFF) << 8) | (ucESIndex & 0xFF);
}

#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
HBOOL VA_PI_Adapt_IsProgramSwitchOn(Handle_t hVaAction)
{
	int i;
	HBOOL bFlag = FALSE;

	VA_SEM_Get(gVaPiSemId);

	for (i = 0; i < VA_PI_MAX_ACS_INSTANCE; i++)
	{
		if (pstVaAcsInstance[i]->hVaAction == hVaAction &&
			(pstVaAcsInstance[i]->nProgramState == VA_PI_PROGRAM_SWITCH_ON ||
			pstVaAcsInstance[i]->nProgramState == VA_PI_PROGRAM_UPDATE))
		{
			bFlag = TRUE;
			break;
		}
	}

	VA_SEM_Release(gVaPiSemId);

	return bFlag;
}

HUINT32 VA_PI_Adapt_GetAcsIdByVaActionHandle(Handle_t hVaAction)
{
	int i;
	HUINT32 ulRet = VA_INVALID_HANDLE;

	VA_SEM_Get(gVaPiSemId);

	for (i = 0; i < VA_PI_MAX_ACS_INSTANCE; i++)
	{
		if (pstVaAcsInstance[i]->hVaAction == hVaAction)
		{
			ulRet = (HUINT32)i;
			break;
		}
	}

	VA_SEM_Release(gVaPiSemId);

	return ulRet;
}
#endif

Handle_t VA_PI_Adapt_GetVaActionHandleByAcsId(HUINT32 ulAcsIdx) // 파라미터 타입 int -> HUINT32 로 수정. 0xffff... 대신에 -1 이 넘어오는 경우가 있기 때문에...
{
	Handle_t hVaAction = HANDLE_NULL;

#if !defined(CONFIG_MW_CAS_VIACCESS_DRIVER_TEST)
// PVR driver test통과를 위해서 task를 거치지 않고 처리하도록 한 부분때문에 여기서 semaphore lock 발생 -> driver test시에는 semaphore 없이 동작하도록..
	VA_ASSERT(gVaPiSemId);
	VA_SEM_Get(gVaPiSemId);
#endif

	if (ulAcsIdx < VA_PI_MAX_ACS_INSTANCE)
	{
		hVaAction = pstVaAcsInstance[ulAcsIdx]->hVaAction;
	}
	else
	{
		HxLOG_Info("Index wrong (%d)\n", ulAcsIdx);
	}

#if !defined(CONFIG_MW_CAS_VIACCESS_DRIVER_TEST)
	VA_SEM_Release(gVaPiSemId);
#endif

	return hVaAction;
}

static VA_PI_AcsInstance *local_va_pi_GetNewAcsInst(Handle_t hVaAction)
{
	int nAcsIdx, i, nErr;
	VA_PI_AcsInstance *pAcs;
	SvcCas_ActionType_e eActionType;

	eActionType = VAMGR_GetActType(hVaAction);
//	HxLOG_Info("%s\n", svc_cas_DbgMakeStrActionType(eActionType));

	// ACS = 0 은 live / playback 에 우선권을 줄거다..
	if (eActionType == eCAS_ActionType_Live ||
		eActionType == eCAS_ActionType_Playback ||
		eActionType == eCAS_ActionType_Tsr_Play)
	{
		nAcsIdx = 0;
	}
	else
	{
		nAcsIdx = 1;
	}

	for (i = 0; i < VA_PI_MAX_ACS_INSTANCE; i++, nAcsIdx++)
	{
		if (nAcsIdx >= VA_PI_MAX_ACS_INSTANCE)
			nAcsIdx = 0;

		pAcs = pstVaAcsInstance[nAcsIdx];
		if (pAcs->nProgramState == VA_PI_PROGRAM_SWITCH_OFF)
		{
			#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
			// acs mode가 live인 경우, close하지 않는다. 그런데 acs mode가 playback으로 바뀌어야 한다면, close했다가 아래에서 다시 open한다
			if (pAcs->nAcsState == VA_PI_ACS_OPEN)
			{
				if ((pAcs->nAcsMode == eLIVE && eActionType != eCAS_ActionType_Live) ||
					(pAcs->nAcsMode != eLIVE && eActionType == eCAS_ActionType_Live))
				{
#ifdef __VIACCESS50__
					nErr = VA_CTRL_CloseAcsInstance(pAcs->nAcsIdx);
#else
					nErr = VA_PI_CloseAcs(pAcs->nAcsIdx);
#endif
					if (nErr == kVA_OK)
					{
						HxLOG_Print("ACS closed(%d,%d)\n", pAcs->nAcsIdx, pAcs->nAcsMode);
						pAcs->nAcsState = VA_PI_ACS_CLOSE;
					}
					else
					{
						HxLOG_Info("fail to close ACS (%x, %d, %d)\n", nErr, pAcs->nAcsIdx, pAcs->nAcsMode);
					}
				}
			}
			#endif

			// 만약 acs open되어있지 않다면, open한다.
			if (pAcs->nAcsState == VA_PI_ACS_CLOSE)
			{
				#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
				switch (eActionType)
				{
				case eCAS_ActionType_Live:
					pAcs->nAcsMode = eLIVE;
					break;

				case eCAS_ActionType_Playback:
					pAcs->nAcsMode =ePERSISTENT_PLAYBACK;
					break;

				case eCAS_ActionType_Recording:
#if defined(CONFIG_MW_CAS_VIACCESS_DRIVER_TEST)
					pAcs->nAcsMode = ePERSISTENT_RECORD;
#else
					pAcs->nAcsMode = eLIVE_AND_PERSISTENT_RECORD;
#endif
					break;

				case eCAS_ActionType_Tsr_Play:
					pAcs->nAcsMode = eTIMESHIFT_PLAYBACK;
					break;

				case eCAS_ActionType_Tsr_Rec:
					pAcs->nAcsMode = eLIVE_AND_TIMESHIFT_RECORD;
					break;

				default:
					//## 암것도 안한다.
					HxLOG_Info("wrong action type(%d)\n", eActionType);
					break;
				}
				#else
				// VA-PVR feature를 사용하지 않는다면 eLive로 통일
				pAcs->nAcsMode = eLIVE;
				#endif
#ifdef __VIACCESS50__
				nErr = VA_CTRL_OpenAcsInstance(pAcs->nAcsIdx, pAcs->nAcsMode);
#else
				nErr = VA_PI_OpenAcs(pAcs->nAcsIdx, pAcs->nAcsMode);
#endif
#if defined(CONFIG_DEBUG)
				HxLOG_Info("VA_PI_OpenAcs (%d,%d,%d(%s))\n", nErr, pAcs->nAcsIdx, pAcs->nAcsMode, VA_PI_INT_GetAcsModeString(pAcs->nAcsMode));
#endif
				if (nErr == kVA_OK)
				{
					pAcs->nAcsState = VA_PI_ACS_OPEN;
				}
			}

			return pAcs;
		}
	}

	HxLOG_Error("not enough ACS instance(%d)\n", VA_PI_MAX_ACS_INSTANCE);

	return NULL;
}

static VA_PI_AcsInstance *local_va_pi_GetAcsInst(Handle_t hVaAction)
{
	int nAcsIdx;
	VA_PI_AcsInstance *pAcs;

	for (nAcsIdx = 0; nAcsIdx < VA_PI_MAX_ACS_INSTANCE; nAcsIdx++)
	{
		pAcs = pstVaAcsInstance[nAcsIdx];
		if (pAcs->nProgramState != VA_PI_PROGRAM_SWITCH_OFF && pAcs->hVaAction == hVaAction)
		{
			return pAcs;
		}
	}

	HxLOG_Error("wrong hVaAction(%x)\n", hVaAction);

	return NULL;
}

HBOOL VA_PI_Adapt_IsRecord(HUINT32 ulAcsIdx)
{
#if defined (CONFIG_MW_CAS_VIACCESS_PVR)
	VA_PI_AcsInstance *pAcs;

	if (ulAcsIdx < VA_PI_MAX_ACS_INSTANCE)
	{
		pAcs = pstVaAcsInstance[ulAcsIdx];
		if (pAcs->nAcsMode == ePERSISTENT_RECORD ||
			pAcs->nAcsMode == eLIVE_AND_PERSISTENT_RECORD ||
			pAcs->nAcsMode == eTIMESHIFT_RECORD ||
			pAcs->nAcsMode == eLIVE_AND_TIMESHIFT_RECORD)
			return TRUE;
	}
#else
	VA_PI_AcsInstance *pAcs;
	SvcCas_ActionType_e eActionType;

	if (ulAcsIdx < VA_PI_MAX_ACS_INSTANCE)
	{
		pAcs = pstVaAcsInstance[ulAcsIdx];

		eActionType = VAMGR_GetActType(pAcs->hVaAction);

		if (eActionType == eCAS_ActionType_Recording ||
			eActionType == eCAS_ActionType_Tsr_Rec)
			return TRUE;
	}
#endif
	return FALSE;
}

HBOOL local_va_pi_IsActive(UINT32 lStbStreamHandle)
{
	int nAcsId, nStreamId;
	VA_PI_AcsInstance *pAcs;
	VA_PI_Stream *pStream;
	int i;
	HBOOL bFlag = FALSE;
#if !defined (CONFIG_MW_CAS_VIACCESS_PVR)
	SvcCas_ActionType_e eActionType;
#endif

HxLOG_Print("[Enter]\n");

	nAcsId = VA_PI_GetAcsId(lStbStreamHandle);
	nStreamId = VA_PI_GetStreamId(lStbStreamHandle);

	// dwStbStreamHandle doesn't correspond to an added stream
	VA_PI_IfNotValidAcsId(nAcsId, FALSE);
	VA_PI_IfNotValidStreamId(nStreamId, FALSE);

	pAcs = pstVaAcsInstance[nAcsId];
	pStream = &pAcs->stStreamInfo.astEs[nStreamId];

	if (pStream->usPid == VA_INVALID_PID || pStream->ulVaStreamHandle == VA_INVALID_HANDLE)
	{
		HxLOG_Info("[VA_PI_Adapt_GetPidFromStreamHandle] stream pid or VA handle is invalid\n");
		return FALSE;
	}

#if defined (CONFIG_MW_CAS_VIACCESS_PVR)
	if (pAcs->nAcsMode == eLIVE)
#else
	eActionType = VAMGR_GetActType(pAcs->hVaAction);

	if (eActionType == eCAS_ActionType_Live)
#endif
	{
		// live의 경우는 setPid로 선택된 pid만 유효하다.. tsr/recording은 전부..
		for (i = 0; i < pAcs->stPidInfo.ulPidNum; i++)
		{
			if (pStream->usPid == pAcs->stPidInfo.stPidInfo[i].usPid)
			{
				bFlag = TRUE;
				break;
			}
		}

		HxLOG_Print("[%s(%x)]\n", bFlag?"Active":"NotActive", pStream->usPid);
	}
	else
	{
		bFlag = TRUE;
	}

HxLOG_Print("[Exit]\n");
	return bFlag;
}

HBOOL VA_PI_Adapt_IsActive(UINT32 lStbStreamHandle)
{
	HBOOL bRet;

HxLOG_Print("[Enter]\n");

// DSCR/PI가 cross로 semaphore 걸려서 deadlock 발생
//	VA_SEM_Get(gVaPiSemId);

	bRet = local_va_pi_IsActive(lStbStreamHandle);

//	VA_SEM_Release(gVaPiSemId);

HxLOG_Print("[Exit]\n");
	return bRet;
}


#if defined(CONFIG_MW_CAS_VIACCESS_PVR)

HINT32 VA_PI_Adapt_GetRecordModeAcs(HUINT32 ulAcsIdx)
{
	int i, nRet = -1;
	VA_PI_AcsInstance *pAcs;

	if (ulAcsIdx < VA_PI_MAX_ACS_INSTANCE)
	{
		VA_SEM_Get(gVaPiSemId);

		for (i = 0; i < VA_PI_MAX_ACS_INSTANCE; i++)
		{
			pAcs = pstVaAcsInstance[i];
			if (pAcs->nAcsIdx == ulAcsIdx)
				continue;

			if (pAcs->nAcsMode == ePERSISTENT_RECORD ||
				pAcs->nAcsMode == eLIVE_AND_PERSISTENT_RECORD ||
				pAcs->nAcsMode == eTIMESHIFT_RECORD ||
				pAcs->nAcsMode == eLIVE_AND_TIMESHIFT_RECORD)
			{
				// live와 같은 서비스를 처리하는지 확인..
				if (VAMGR_IsSameSvcWithLive(pAcs->hVaAction) == TRUE)
				{
					nRet = i;
					break;
				}
			}
		}

		VA_SEM_Release(gVaPiSemId);
	}

	return nRet;
}

HBOOL VA_PI_Adapt_IsLive(HUINT32 ulAcsIdx)
{
	VA_PI_AcsInstance *pAcs;

	if (ulAcsIdx < VA_PI_MAX_ACS_INSTANCE)
	{
		pAcs = pstVaAcsInstance[ulAcsIdx];
		if (pAcs->nAcsMode == eLIVE && pAcs->nProgramState != VA_PI_PROGRAM_SWITCH_OFF)
			return TRUE;
	}

	return FALSE;
}

HINT32 VA_PI_Adapt_GetPlaybackAcsId()
{
	int i;
	VA_PI_AcsInstance *pAcs;

	for (i = 0; i < VA_PI_MAX_ACS_INSTANCE; i++)
	{
		pAcs = pstVaAcsInstance[i];
		if (pAcs->nAcsMode == ePERSISTENT_PLAYBACK || pAcs->nAcsMode == eTIMESHIFT_PLAYBACK)
			return i;
	}

	return -1;
}

HBOOL VA_PI_Adapt_IsPlayback(HUINT32 ulAcsIdx)
{
	VA_PI_AcsInstance *pAcs;

	if (ulAcsIdx < VA_PI_MAX_ACS_INSTANCE)
	{
		pAcs = pstVaAcsInstance[ulAcsIdx];
		if (pAcs->nAcsMode == ePERSISTENT_PLAYBACK || pAcs->nAcsMode == eTIMESHIFT_PLAYBACK)
			return TRUE;
	}

	return FALSE;
}

HBOOL VA_PI_Adapt_IsTimeshiftRecord(HUINT32 ulAcsIdx)
{
	VA_PI_AcsInstance *pAcs;

	if (ulAcsIdx < VA_PI_MAX_ACS_INSTANCE)
	{
		pAcs = pstVaAcsInstance[ulAcsIdx];
		if (pAcs->nAcsMode == eTIMESHIFT_RECORD ||
			pAcs->nAcsMode == eLIVE_AND_TIMESHIFT_RECORD)
			return TRUE;
	}

	return FALSE;
}

VA_PI_AcsInstance *VA_PI_Adapt_GetAcsInstById(HUINT32 ulAcsIdx)
{
	VA_PI_AcsInstance *pAcs;

	if (ulAcsIdx < VA_PI_MAX_ACS_INSTANCE)
	{
		return pstVaAcsInstance[ulAcsIdx];
	}

	return NULL;
}


HERROR VA_PI_Adapt_SetRecordForbidden(HUINT32 ulAcsId)
{
	VA_PI_AcsInstance *pAcs;

	if (ulAcsId >= VA_PI_MAX_ACS_INSTANCE)
	{
		HxLOG_Info("wrong dwAcsId\n");
		return ERR_FAIL;
	}

	pAcs = pstVaAcsInstance[ulAcsId];

// 주의)) timeshift는 별개임..
	if (pAcs->nAcsMode == ePERSISTENT_RECORD || pAcs->nAcsMode == eLIVE_AND_PERSISTENT_RECORD)
	{
		if (pAcs->bRecordForbidden != TRUE)
		{
			pAcs->bRecordForbidden = TRUE;

			VAACT_UpdateDrmInfo(VAMGR_GetActionHandle(pAcs->hVaAction), pAcs->bRecordForbidden, pAcs->bTsrForbidden);
			HxLOG_Info("VAACT_UpdateDrmInfo(%x,%x,%x)\n", pAcs->hVaAction, pAcs->bRecordForbidden, pAcs->bTsrForbidden);
		}
	}

	return ERR_OK;
}
#endif

int VA_PI_Adapt_GetStreamTypeFromStreamHandle(UINT32 lStbStreamHandle)
{
	int nRet;

HxLOG_Print("[Enter]\n");

	VA_ASSERT(gVaPiSemId);
	VA_SEM_Get(gVaPiSemId);

	nRet = local_va_pi_GetStreamTypeFromStreamHandle(lStbStreamHandle);

	VA_SEM_Release(gVaPiSemId);

HxLOG_Print("[Exit]\n");
	return nRet;
}

static int local_va_pi_GetStreamTypeFromStreamHandle(UINT32 lStbStreamHandle)
{
	int nAcsId, nStreamId;
	VA_PI_AcsInstance *pAcs;
	VA_PI_Stream *pStream;
	EsType_t eEsType = eEsType_None;
	HUINT32 ulPidNum;
	SvcCas_PidInfo_t *pPidInfo;
	int i;
#if !defined (CONFIG_MW_CAS_VIACCESS_PVR)
	SvcCas_ActionType_e eActionType;
#endif

	nAcsId = VA_PI_GetAcsId(lStbStreamHandle);
	nStreamId = VA_PI_GetStreamId(lStbStreamHandle);

	// dwStbStreamHandle doesn't correspond to an added stream
	VA_PI_IfNotValidAcsId(nAcsId, VA_ERR);
	VA_PI_IfNotValidStreamId(nStreamId, VA_ERR);

	pAcs = pstVaAcsInstance[nAcsId];
	pStream = &pAcs->stStreamInfo.astEs[nStreamId];

	if (pStream->usPid == VA_INVALID_PID || pStream->ulVaStreamHandle == VA_INVALID_HANDLE)
	{
		HxLOG_Error("[%x,%x,%x] stream %x is invalid (%x,%x)\n",
			lStbStreamHandle, nAcsId, nStreamId, pStream->usPid, pStream->ulStbStreamHandle, pStream->ulVaStreamHandle);
		return VA_ERR;
	}

#if defined (CONFIG_MW_CAS_VIACCESS_PVR)
	if (pAcs->nAcsMode == eLIVE)
#else
	eActionType = VAMGR_GetActType(pAcs->hVaAction);

	if (eActionType == eCAS_ActionType_Live)
#endif
	{
		ulPidNum = pAcs->stPidInfo.ulPidNum;
		pPidInfo = pAcs->stPidInfo.stPidInfo;

		// live의 경우는 setPid로 선택된 pid만 유효하다.. tsr/recording은 전부..
		for (i = 0; i < ulPidNum; i++)
		{
			if (pStream->usPid == pPidInfo[i].usPid)
			{
				eEsType = pStream->esType;
				break;
			}
		}
	}
	else
	{
		eEsType = pStream->esType;
	}

#if defined (CONFIG_MW_CAS_VIACCESS_PVR)
	HxLOG_Info("%d : stream %x\n", pAcs->nAcsMode, pStream->usPid);
#else
	HxLOG_Info(" :: stream %x\n", pStream->usPid);
#endif

	return (int)eEsType;
}

UINT16 VA_PI_Adapt_GetPidFromStreamHandle(UINT32 lStbStreamHandle)
{
	int nAcsId, nStreamId;
	VA_PI_AcsInstance *pAcs;
	VA_PI_Stream *pStream;
	UINT16 usPid = VA_INVALID_PID;

HxLOG_Print("[Enter]\n");

	nAcsId = VA_PI_GetAcsId(lStbStreamHandle);
	nStreamId = VA_PI_GetStreamId(lStbStreamHandle);

	// dwStbStreamHandle doesn't correspond to an added stream
	VA_PI_IfNotValidAcsId(nAcsId, VA_INVALID_PID);
	VA_PI_IfNotValidStreamId(nStreamId, VA_INVALID_PID);

// DSCR와 PI간 cross로 semaphore 걸려서 deadlock 걸리는 경우 있음..
#if !defined(CONFIG_MW_CAS_VIACCESS_PVR)
	VA_ASSERT(gVaPiSemId);
	VA_SEM_Get(gVaPiSemId);
#endif

	pAcs = pstVaAcsInstance[nAcsId];
	pStream = &pAcs->stStreamInfo.astEs[nStreamId];

	if (pStream->usPid == VA_INVALID_PID || pStream->ulVaStreamHandle == VA_INVALID_HANDLE)
	{
// DSCR와 PI간 cross로 semaphore 걸려서 deadlock 걸리는 경우 있음..
#if !defined(CONFIG_MW_CAS_VIACCESS_PVR)
		VA_SEM_Release(gVaPiSemId);
#endif
		HxLOG_Error("[%x,%x,%x] stream %x is invalid (%x,%x)\n",
			lStbStreamHandle, nAcsId, nStreamId, pStream->usPid, pStream->ulStbStreamHandle, pStream->ulVaStreamHandle);
		return VA_INVALID_PID;
	}

	usPid = pStream->usPid;

// DSCR와 PI간 cross로 semaphore 걸려서 deadlock 걸리는 경우 있음..
#if !defined(CONFIG_MW_CAS_VIACCESS_PVR)
	VA_SEM_Release(gVaPiSemId);
#endif

HxLOG_Print("[Exit]\n");
	return usPid;
}

Handle_t VA_PI_Adapt_GetVaActionHandleByStreamHandle(UINT32 lStbStreamHandle)
{
	int nAcsId, nStreamId;
	VA_PI_AcsInstance *pAcs;
	VA_PI_Stream *pStream;
	Handle_t hVaAction = HANDLE_NULL;

HxLOG_Print("[Enter]\n");

	nAcsId = VA_PI_GetAcsId(lStbStreamHandle);

	// dwStbStreamHandle doesn't correspond to an added stream
	VA_PI_IfNotValidAcsId(nAcsId, HANDLE_NULL);

// DSCR와 PI간 cross로 semaphore 걸려서 deadlock 걸리는 경우 있음..
// local_va_dscr_ChangePid() 함수에서 호출시에 Deadlock 발생함.
#if 0//!defined(CONFIG_MW_CAS_VIACCESS_PVR)
	VA_ASSERT(gVaPiSemId);
	VA_SEM_Get(gVaPiSemId);
#endif

	pAcs = pstVaAcsInstance[nAcsId];
	hVaAction = pAcs->hVaAction;

// DSCR와 PI간 cross로 semaphore 걸려서 deadlock 걸리는 경우 있음..
// local_va_dscr_ChangePid() 함수에서 호출시에 Deadlock 발생함.
#if 0//!defined(CONFIG_MW_CAS_VIACCESS_PVR)
	VA_SEM_Release(gVaPiSemId);
#endif

HxLOG_Print("[Exit]\n");
	return hVaAction;
}

/*
	initialize variables and start task
*/
void VA_PI_Adapt_Init()
{
	int i, j;
	int nErr;
	VA_PI_AcsInstance *pAcs;

	HxLOG_Print("[Enter]\n");

	nErr = VA_SEM_Create(VA_PI_SEM_NAME, &gVaPiSemId);
	if (nErr)
	{
		HxLOG_Error("failed to create semaphore (%d)\n", nErr);
		VA_ASSERT(nErr == VK_OK);
	}

	// init acs instance
	for (i = 0; i < VA_PI_MAX_ACS_INSTANCE; i++)
	{
		pAcs = (VA_PI_AcsInstance *)VA_MEM_Alloc(sizeof(VA_PI_AcsInstance));
		VA_ASSERT(pAcs != NULL);
		pstVaAcsInstance[i] = pAcs;

		pAcs->nAcsIdx = i;
		pAcs->nAcsState = VA_PI_ACS_CLOSE;
		pAcs->nAcsMode = eLIVE;
		pAcs->nProgramState = VA_PI_PROGRAM_SWITCH_OFF;
		pAcs->bCatReceived = FALSE;

		pAcs->usUniqueTsId = 0;
		pAcs->stStreamInfo.ulEsNum = 0;
		for (j = 0; j < VA_PI_MAX_ES_TYPE; j++)
		{
			pAcs->stStreamInfo.astEs[j].ulVaStreamHandle = VA_INVALID_HANDLE;
			pAcs->stStreamInfo.astEs[j].ulStbStreamHandle = 0;
			pAcs->stStreamInfo.astEs[j].usPid = VA_INVALID_PID;
			pAcs->stStreamInfo.astEs[j].pPmtElement = NULL;
		}
		pAcs->pstPmtInfo = NULL;

		pAcs->ulCatLength = 0;

		pAcs->stUiInfo.ulTimerId = 0;
		pAcs->stUiInfo.nReceivedEvtNum = 0;
		pAcs->stUiInfo.stLastEvent.eEventType = VA_UI_UNKNOWN;
		pAcs->stUiInfo.stLastEvent.ulAcsId = VA_INVALID_HANDLE;
		pAcs->stUiInfo.stLastEvent.ulTimerId = 0;

	}

	HxLOG_Print("[Exit]\n");
}

void VA_PI_Adapt_StartProgram(Handle_t hVaAction, UINT16 usUniqueId)	//	UINT16 usTsId, UINT16 usProgNum, UINT16 usVideoPid, UINT16 usAudioPid, UINT16 usSubtitlePid, UINT16 usTeletextPid)
{
	VA_PI_AcsInstance *pAcs;
	HERROR nErr;
#if !defined (CONFIG_MW_CAS_VIACCESS_PVR)
	SvcCas_ActionType_e eActionType;
#endif

	HxLOG_Print("[Enter][stbHandle:%x]\n", hVaAction);

	VA_ASSERT(gVaPiSemId);
	VA_SEM_Get(gVaPiSemId);

	pAcs = local_va_pi_GetNewAcsInst(hVaAction);	// get free instance

	if (pAcs == NULL)
	{
		VA_SEM_Release(gVaPiSemId);
		HxLOG_Error("not enough ACS instance(%d)\n", VA_PI_MAX_ACS_INSTANCE);
		return;
	}

	HxLOG_Info("[Get ACS Instance(%d)]\n", pAcs->nAcsIdx);

	pAcs->nProgramState = VA_PI_PROGRAM_READY;
	pAcs->hVaAction = hVaAction;

	pAcs->stStreamInfo.ulEsNum = 0;

	pAcs->stPidInfo.ulPidNum = 0;

	if (pAcs->pstPmtInfo)
	{
		HxLOG_Error("pmt not freed\n");
		VA_MEM_Free(pAcs->pstPmtInfo);
		pAcs->pstPmtInfo = NULL;
	}

#if defined (CONFIG_MW_CAS_VIACCESS_PVR)
	HxLOG_Print("[UniqueID](mode:%d, orig=%x, new=%x)\n", pAcs->nAcsMode , pAcs->usUniqueTsId, usUniqueId);

	if (pAcs->nAcsMode == eLIVE && pAcs->usUniqueTsId != usUniqueId)
#else
	eActionType = VAMGR_GetActType(pAcs->hVaAction);
	HxLOG_Info("UniqueID : mode = %d, orig = %x, new = %x\n", pAcs->nAcsMode, pAcs->usUniqueTsId, usUniqueId);

	if (eActionType == eCAS_ActionType_Live && pAcs->usUniqueTsId != usUniqueId)
#endif
	{
		pAcs->usUniqueTsId = usUniqueId;

		HxLOG_Info("VA_PI_TsChanged(%d) calling...\n", pAcs->nAcsIdx);
#ifdef __VIACCESS50__
		nErr = VA_CTRL_TsChanged(pAcs->nAcsIdx);
#else
		nErr = VA_PI_TsChanged(pAcs->nAcsIdx);
#endif
		if (nErr != kVA_OK)
		{
			HxLOG_Error("VA_PI_TsChanged(%d) return %d\n", pAcs->nAcsIdx, nErr);
		}
	}

#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
	// VA_PI_Adapt_StartProgram()에서 처리를 하면, fta 채널의 playback이 block되는 문제 있음...
	pAcs->bRecordForbidden = VA_UNKNOWN;
	pAcs->bTsrForbidden = VA_UNKNOWN;
	pAcs->bPlaybackStarted = FALSE;

	if (pAcs->nAcsMode == ePERSISTENT_PLAYBACK || pAcs->nAcsMode == eTIMESHIFT_PLAYBACK)
	{
		VA_PVR_OpenPbFile(hVaAction);

		// 일단 av 출력를 막는다.. 권한 확인 전에 화면 나오는 것 방지
#if !defined(CONFIG_MW_CAS_VIACCESS_DRIVER_TEST)
		VA_OutputMute(TRUE, VA_MUTE_PB_VIDEO | VA_MUTE_PB_AUDIO);
#endif
	}
#endif

	VA_SEM_Release(gVaPiSemId);
	HxLOG_Print("[Exit]\n");
}

void VA_PI_Adapt_StopProgram(Handle_t hVaAction)
{
	int nErr;
	int i;
	VA_PI_AcsInstance *pAcs;
	UINT32 ulEsNum;
	VA_PI_Stream *pEs;

	HxLOG_Print("[Enter][stbHandle:%x]\n", hVaAction);

	VA_ASSERT(gVaPiSemId);
	VA_SEM_Get(gVaPiSemId);

	pAcs = local_va_pi_GetAcsInst(hVaAction);

	if (pAcs == NULL)
	{
		VA_SEM_Release(gVaPiSemId);
		HxLOG_Error("wrong hVaAction(%x)\n", hVaAction);
		return;
	}

	if (pAcs->pstPmtInfo)
	{
		VA_MEM_Free(pAcs->pstPmtInfo);
		pAcs->pstPmtInfo = NULL;
	}

	// stream 제거
	ulEsNum = pAcs->stStreamInfo.ulEsNum;
	pEs = pAcs->stStreamInfo.astEs;
	for (i = 0; i  < ulEsNum; i++)
	{
		if (pEs[i].ulVaStreamHandle != VA_INVALID_HANDLE)
		{
			HxLOG_Info("[%d] VA_PI_RemoveStream(%x(%x)) with pid:%x calling...\n",
				i, pEs[i].ulVaStreamHandle, pEs[i].ulStbStreamHandle, pEs[i].usPid);
#ifdef __VIACCESS50__
			nErr = VA_CTRL_RemoveStream(pEs[i].ulVaStreamHandle);
#else
			nErr = VA_PI_RemoveStream(pEs[i].ulVaStreamHandle);
#endif
			if (nErr != kVA_OK)
			{
				HxLOG_Error("[%d] VA_PI_RemoveStream(%x(%x)) with pid:%x return %d\n",
					i, pEs[i].ulVaStreamHandle, pEs[i].ulStbStreamHandle, pEs[i].usPid, nErr);
			}
			else
			{
				pEs[i].ulVaStreamHandle = VA_INVALID_HANDLE;
			}
#if defined(CONFIG_MW_CAS_VIACCESS_PVR) && defined(CONFIG_MW_CAS_VIACCESS_DRIVER_TEST)
			if (pAcs->nAcsMode == ePERSISTENT_RECORD)
				VA_DSCR_Close(pAcs->stStreamInfo.astEs[i].ulStbDscrHandle);
#endif
			//이게 필요하려나??
			//## VA_DSCR_ChangePid(local_va_pi_GetStreamHandle(pAcs->nAcsIdx ,i), pEs[i].usPid, TRUE);
		}
	}

	pAcs->stStreamInfo.ulEsNum = 0;

	if (pAcs->nProgramState == VA_PI_PROGRAM_SWITCH_ON || pAcs->nProgramState == VA_PI_PROGRAM_UPDATE)
	{
		HxLOG_Info("VA_PI_SwitchOffProgram(%d) calling...\n", pAcs->nAcsIdx);
#ifdef __VIACCESS50__
		nErr = VA_CTRL_SwitchOffProgram(pAcs->nAcsIdx);
#else
		nErr = VA_PI_SwitchOffProgram(pAcs->nAcsIdx);
#endif
		if (nErr != kVA_OK && nErr != kVA_ALREADY_DONE)
		{
			HxLOG_Error("VA_PI_SwitchOffProgram(%d) return %d\n", pAcs->nAcsIdx, nErr);
		}
	}

	// UI메시지 꼬이는걸 방지..
	pAcs->stUiInfo.ulTimerId = 0;

	pAcs->nProgramState = VA_PI_PROGRAM_SWITCH_OFF;

#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
	if (pAcs->nAcsMode == ePERSISTENT_PLAYBACK || pAcs->nAcsMode == eTIMESHIFT_PLAYBACK)
	{
		// playback이 끝났으므로 PB mute는 해제
#if !defined(CONFIG_MW_CAS_VIACCESS_DRIVER_TEST)
		VA_OutputMute(FALSE, VA_MUTE_PB_VIDEO | VA_MUTE_PB_AUDIO);
//VA_OS_Printf("[%d]VA_OutputMute(%d,%d)\n", __LINE__, FALSE, VA_MUTE_PB_VIDEO | VA_MUTE_PB_AUDIO);
#endif

		VA_PVR_ClosePbFile(hVaAction);
	}
	else
	{
		if (VA_PI_Adapt_IsRecord(pAcs->nAcsIdx))
		{
			VA_PVR_CloseEcmFile(pAcs->nAcsIdx);
		}
	}

//## live가 아닌경우엔 close한다.
	if (pAcs->nAcsMode != eLIVE)
	{
#ifdef __VIACCESS50__
		nErr = VA_CTRL_CloseAcsInstance(pAcs->nAcsIdx);
#else
		nErr = VA_PI_CloseAcs(pAcs->nAcsIdx);
#endif
		if (nErr == kVA_OK)
		{
			HxLOG_Print("ACS closed(%d,%d)\n", pAcs->nAcsIdx, pAcs->nAcsMode);
			pAcs->nAcsState = VA_PI_ACS_CLOSE;
		}
		else
		{
			HxLOG_Info("fail to close ACS (%x, %d, %d)\n", nErr, pAcs->nAcsIdx, pAcs->nAcsMode);
		}
	}
#endif

	pAcs->hVaAction = HANDLE_NULL;

	VA_SEM_Release(gVaPiSemId);
	HxLOG_Print("[Exit]\n");
}

void VA_PI_Adapt_Select(Handle_t hVaAction, HBOOL bSelect)
{
	VA_PI_AcsInstance *pAcs;

	HxLOG_Print("[Enter][stbHandle:%x]\n", hVaAction);

	VA_ASSERT(gVaPiSemId);
	VA_SEM_Get(gVaPiSemId);

	pAcs = local_va_pi_GetAcsInst(hVaAction);

	if (pAcs == NULL)
	{
		VA_SEM_Release(gVaPiSemId);
		HxLOG_Error("wrong hVaAction(%x)\n", hVaAction);
		return;
	}

//사용안함	pAcs->bSelected = bSelect;

#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
	// VA_PI_Adapt_StartProgram()에서 처리를 하면, fta 채널의 playback이 block되는 문제 있음...
//	pAcs->bRecordForbidden = VA_UNKNOWN;
//	pAcs->bTsrForbidden = VA_UNKNOWN;
//	pAcs->bPlaybackStarted = FALSE;

	if (bSelect && VA_PI_Adapt_IsRecord(pAcs->nAcsIdx))
	{
		VA_PVR_OpenEcmFile(pAcs->nAcsIdx, VA_PI_Adapt_IsTimeshiftRecord(pAcs->nAcsIdx));
	}
#endif
	VA_SEM_Release(gVaPiSemId);
	HxLOG_Print("[Exit]\n");
}

static void local_va_pi_adapt_StreamChange(VA_PI_AcsInstance *pAcs, HBOOL bAllowUpdateStream)
{

	int nErr;
	int i, j;
	SvcCas_PmtElement_t *pElement;
	HBOOL bPidChanged, bPidSelected;

	HxLOG_Print("[Enter]\n");

	if (pAcs->pstPmtInfo == NULL)
	{
		HxLOG_Info("pstPmtInfo is NULL\n");
		return;
	}

	/*---------------------------------------------------------------------------------------------------------------------------------------
		Managing a PMT update :

		calls the VA_PI_PmtUpdated() function -> VA_PI_Adapt_PmtChange() 에서 함.
		calls the VA_PI_AddStream() function for each supplementary stream handle if the number of streams	has increased.
		calls the VA_PI_RemoveStream() function for each stream handle no longer in use if the number of streams has decreased.
		If the amount of streams does not change, the STB only calls the VA_PI_UpdateStream() function.
		The STB must not use VA_PI_RemoveStream() followed by VA_PI_AddStream() instead of the VA_PI_UpdateStream() function even if the type of the stream changes.
		Removing a stream implies closing the descrambler channel.
		It is therefore not possible to create a seamless transition by calling VA_PI_RemoveStream().
	---------------------------------------------------------------------------------------------------------------------------------------*/

// ACS와는 pmt 기준으로 처리..
	// 기존에 들고 있던 pid 비교
	for (i = 0; i  < pAcs->stStreamInfo.ulEsNum; i++)
	{
		if (i >= pAcs->pstPmtInfo->ulNumOfElement)
		{
			// 새로운 pmt에서는 없어졌다
			if (pAcs->stStreamInfo.astEs[i].ulVaStreamHandle != VA_INVALID_HANDLE)
			{
				HxLOG_Info("VA_PI_RemoveStream1(%x)\n", pAcs->stStreamInfo.astEs[i].ulVaStreamHandle);
#ifdef __VIACCESS50__
				nErr = VA_CTRL_RemoveStream(pAcs->stStreamInfo.astEs[i].ulVaStreamHandle);
#else
				nErr = VA_PI_RemoveStream(pAcs->stStreamInfo.astEs[i].ulVaStreamHandle);
#endif
				if (nErr == kVA_OK)
				{
					pAcs->stStreamInfo.astEs[i].ulVaStreamHandle = VA_INVALID_HANDLE;
#ifdef __VIACCESS50__
					pAcs->stStreamInfo.astEs[i].nDesState = VA_INVALID_HANDLE;
#else
					pAcs->stStreamInfo.astEs[i].nState = VA_INVALID_HANDLE;
#endif
				}

#if defined(CONFIG_MW_CAS_VIACCESS_PVR) && defined(CONFIG_MW_CAS_VIACCESS_DRIVER_TEST)
				if (pAcs->nAcsMode == ePERSISTENT_RECORD)
					VA_DSCR_Close(pAcs->stStreamInfo.astEs[i].ulStbDscrHandle);
#endif
			}

			continue;
		}

		pElement = &pAcs->pstPmtInfo->stElementInfo[i];

		HxLOG_Info("[%d, COMP_PID:%x -> %x]\n", i, pAcs->stStreamInfo.astEs[i].usPid, pElement->usPid);

		bPidChanged = FALSE;
		if (pAcs->stStreamInfo.astEs[i].usPid != pElement->usPid)
		{
			bPidChanged = TRUE;
			pAcs->stStreamInfo.astEs[i].usPid = pElement->usPid;
			pAcs->stStreamInfo.astEs[i].esType = pElement->eEsType;	//swlee3 : humax ES type을 저장... ES level banner 출력용...
		}

		pAcs->stStreamInfo.astEs[i].pPmtElement = pElement;

		bPidSelected = TRUE;

		if (pAcs->stStreamInfo.astEs[i].ulVaStreamHandle == VA_INVALID_HANDLE)
		{
			// recording이 아니라면 set pid에 포함되었는지 확인하여 있으면 add 아니면 skip
			if (VA_PI_Adapt_IsRecord(pAcs->nAcsIdx) == FALSE)
			{
				bPidSelected = FALSE;

				for (j = 0; j < pAcs->stPidInfo.ulPidNum; j++)
				{
					if (pAcs->stPidInfo.stPidInfo[j].usPid == pAcs->stStreamInfo.astEs[i].usPid)
					{
						bPidSelected = TRUE;
						break;
					}
				}
			}

#if !defined(CONFIG_MW_CAS_VIACCESS_DRIVER_TEST)
			if (bPidSelected)
#endif
			{
				// va handle을 못받은 상태이면 add
#ifdef __VIACCESS50__
/*	TODO: hjlee3, pfVaStreamNotificationCallback 을 어케 할지 확인하자.	*/
				pAcs->stStreamInfo.astEs[i].nDesState = VA_INVALID_HANDLE;
				pAcs->stStreamInfo.astEs[i].ulVaStreamHandle = VA_CTRL_AddStream(pAcs->nAcsIdx, pAcs->stStreamInfo.astEs[i].ulStbStreamHandle,
																					pAcs->stStreamInfo.astEs[i].usPid,pAcs->cbCtrlStreamNotiCallback);
#else
				pAcs->stStreamInfo.astEs[i].nState = VA_INVALID_HANDLE;
				pAcs->stStreamInfo.astEs[i].ulVaStreamHandle = VA_PI_AddStream(pAcs->nAcsIdx, pAcs->stStreamInfo.astEs[i].ulStbStreamHandle);
#endif
				HxLOG_Info("[SetPid:VA_PI_AddStream](handle:%x, pid:%x) return:%x\n"
					, pAcs->stStreamInfo.astEs[i].ulStbStreamHandle, pAcs->stStreamInfo.astEs[i].usPid, pAcs->stStreamInfo.astEs[i].ulVaStreamHandle);

#if defined(CONFIG_MW_CAS_VIACCESS_PVR) && defined(CONFIG_MW_CAS_VIACCESS_DRIVER_TEST)
				if (pAcs->nAcsMode == ePERSISTENT_RECORD)
					pAcs->stStreamInfo.astEs[i].ulStbDscrHandle = VA_DSCR_Open(pAcs->stStreamInfo.astEs[i].ulStbStreamHandle);
#endif
			}
		}
		else
		{
			// recording이 아니라면 set pid에 포함되었는지 확인하여 있으면 update 아니면 remove
			if (VA_PI_Adapt_IsRecord(pAcs->nAcsIdx) == FALSE)
			{
				bPidSelected = FALSE;

				for (j = 0; j < pAcs->stPidInfo.ulPidNum; j++)
				{
					if (pAcs->stPidInfo.stPidInfo[j].usPid == pAcs->stStreamInfo.astEs[i].usPid)
					{
						bPidSelected = TRUE;
						break;
					}
				}
			}

#if !defined(CONFIG_MW_CAS_VIACCESS_DRIVER_TEST)
			if (bPidSelected)
#endif
			{
				if (bAllowUpdateStream)
				{
					// va handle이 있으면 update
#ifdef __VIACCESS50__
					pAcs->stStreamInfo.astEs[i].nDesState= VA_INVALID_HANDLE;
					nErr = VA_CTRL_UpdateStream(pAcs->stStreamInfo.astEs[i].ulVaStreamHandle,pAcs->stStreamInfo.astEs[i].usPid);
#else
					pAcs->stStreamInfo.astEs[i].nState = VA_INVALID_HANDLE;
					nErr = VA_PI_UpdateStream(pAcs->stStreamInfo.astEs[i].ulVaStreamHandle);
#endif
					HxLOG_Info("VA_PI_UpdateStream(%x):ret=%x\n", pAcs->stStreamInfo.astEs[i].ulVaStreamHandle, nErr);
				if (nErr != kVA_OK)
				{
						pAcs->stStreamInfo.astEs[i].ulVaStreamHandle = VA_INVALID_HANDLE;
				}
				else
				{
					if (bPidChanged)
					{
						// 동일 ES가 pid만 바뀐경우
						VA_DSCR_ChangePid(pAcs->stStreamInfo.astEs[i].ulStbStreamHandle,
											local_va_pi_IsActive(pAcs->stStreamInfo.astEs[i].ulStbStreamHandle),
											pAcs->stStreamInfo.astEs[i].usPid);
					}
				}
			}
		}
#if !defined(CONFIG_MW_CAS_VIACCESS_DRIVER_TEST)
			else
		{
				HxLOG_Info("VA_PI_RemoveStream2(%x)\n", pAcs->stStreamInfo.astEs[i].ulVaStreamHandle);
#ifdef __VIACCESS50__
				nErr = VA_CTRL_RemoveStream(pAcs->stStreamInfo.astEs[i].ulVaStreamHandle);
#else
				nErr = VA_PI_RemoveStream(pAcs->stStreamInfo.astEs[i].ulVaStreamHandle);
#endif
				if (nErr == kVA_OK)
				{
					pAcs->stStreamInfo.astEs[i].ulVaStreamHandle = VA_INVALID_HANDLE;
#ifdef __VIACCESS50__
					pAcs->stStreamInfo.astEs[i].nDesState = VA_INVALID_HANDLE;
#else
					pAcs->stStreamInfo.astEs[i].nState = VA_INVALID_HANDLE;
#endif
				}
		}
#endif
		}

//		if (bPidChanged)
//		{
//			// 동일 ES가 pid만 바뀐경우
//			VA_DSCR_ChangePid(pAcs->stStreamInfo.astEs[i].ulStbStreamHandle, pAcs->stStreamInfo.astEs[i].ulStbStreamHandle, local_va_pi_IsActive(pAcs->stStreamInfo.astEs[i].ulStbStreamHandle), pAcs->stStreamInfo.astEs[i].usPid);
//		}
	}

	// 새로 추가된 pid
	for (; i < pAcs->pstPmtInfo->ulNumOfElement && i < VA_PI_MAX_ES_TYPE; i++)
	{
		pElement = &pAcs->pstPmtInfo->stElementInfo[i];

		HxLOG_Info("[%d, NEW_PID:%x]\n", i, pElement->usPid);
#ifdef __VIACCESS50__
		pAcs->stStreamInfo.astEs[i].nDesState= VA_INVALID_HANDLE;
#else
		pAcs->stStreamInfo.astEs[i].nState = VA_INVALID_HANDLE;
#endif
		if (VALID_PID(pElement->usPid))
		{
			// 새로운 pid를 기록
			pAcs->stStreamInfo.astEs[i].usPid = pElement->usPid;
			pAcs->stStreamInfo.astEs[i].esType = pElement->eEsType;	//swlee3 : humax ES type을 저장... ES level banner 출력용...

			pAcs->stStreamInfo.astEs[i].pPmtElement = pElement;

			pAcs->stStreamInfo.astEs[i].ulStbStreamHandle = local_va_pi_GetStreamHandle(pAcs->nAcsIdx, i);

			// recording이 아니라면 set pid에 포함되었는지 확인하여 있으면 add 아니면 skip
			if (VA_PI_Adapt_IsRecord(pAcs->nAcsIdx) == FALSE)
			{
				bPidSelected = FALSE;

				for (j = 0; j < pAcs->stPidInfo.ulPidNum; j++)
				{
					if (pAcs->stPidInfo.stPidInfo[j].usPid == pAcs->stStreamInfo.astEs[i].usPid)
					{
						bPidSelected = TRUE;
						break;
					}
				}
			}
			else
			{
				bPidSelected = TRUE;
			}

#if !defined(CONFIG_MW_CAS_VIACCESS_DRIVER_TEST)
			if (bPidSelected)
#endif
		{
#ifdef __VIACCESS50__
/*	TODO: hjlee3,
  		  pAcs->cbCtrlStreamNotiCallback viaccess 의 상태를 알려준다. ????	*/
			pAcs->stStreamInfo.astEs[i].ulVaStreamHandle = VA_CTRL_AddStream(pAcs->nAcsIdx, pAcs->stStreamInfo.astEs[i].ulStbStreamHandle,
																				pAcs->stStreamInfo.astEs[i].usPid, pAcs->cbCtrlStreamNotiCallback);
#else
			pAcs->stStreamInfo.astEs[i].ulVaStreamHandle = VA_PI_AddStream(pAcs->nAcsIdx, pAcs->stStreamInfo.astEs[i].ulStbStreamHandle);
#endif
			HxLOG_Info("[SetPid(2):VA_PI_AddStream](handle:%x, pid:%x) return:%x\n"
						, pAcs->stStreamInfo.astEs[i].ulStbStreamHandle, pAcs->stStreamInfo.astEs[i].usPid, pAcs->stStreamInfo.astEs[i].ulVaStreamHandle);

#if defined(CONFIG_MW_CAS_VIACCESS_PVR) && defined(CONFIG_MW_CAS_VIACCESS_DRIVER_TEST)
				if (pAcs->nAcsMode == ePERSISTENT_RECORD)
					pAcs->stStreamInfo.astEs[i].ulStbDscrHandle = VA_DSCR_Open(pAcs->stStreamInfo.astEs[i].ulStbStreamHandle);
#endif
			}
#if !defined(CONFIG_MW_CAS_VIACCESS_DRIVER_TEST)
			else
			{
				pAcs->stStreamInfo.astEs[i].ulVaStreamHandle = VA_INVALID_HANDLE;
			}
#endif
		}
		else
		{
			// 처음부터 null인 pid는 등록하지 말자.
			pAcs->stStreamInfo.astEs[i].usPid = VA_INVALID_PID;
			pAcs->stStreamInfo.astEs[i].ulVaStreamHandle = VA_INVALID_HANDLE;
		}
	}

	// 새로운 pidNum 기록
	pAcs->stStreamInfo.ulEsNum = i;
}

void VA_PI_Adapt_PmtChange(Handle_t hVaAction, SvcCas_PmtInfo_t *pstPmtInfo, HUINT8 *pmtRaw)
{
	VA_PI_AcsInstance *pAcs;
	HUINT32 		   nLength;
	int nErr;
	HxLOG_Print("[Enter][stbHandle:%x]\n", hVaAction);

	if (pstPmtInfo == NULL)
	{
		HxLOG_Error("pstPmtInfo is NULL\n");
		return;
	}

	VA_ASSERT(gVaPiSemId);
	VA_SEM_Get(gVaPiSemId);

	pAcs = local_va_pi_GetAcsInst(hVaAction);

	if (pAcs == NULL)
	{
		VA_SEM_Release(gVaPiSemId);
		HxLOG_Error("wrong hVaAction(%x)\n", hVaAction);
		return;
	}
#if defined(CONFIG_DEBUG)
	HxLOG_Info("nProgramState = %s\n", VA_PI_INT_GetProgramStateString(pAcs->nProgramState));
#endif

#ifdef __VIACCESS50__
	nLength = Get12bit(pmtRaw + 1) + 3;
	VA_memcpy(pAcs->aucPmt, pmtRaw, nLength);
	pAcs->ulPmtLength = nLength;
#endif

	// VA_PI_SwitchOnProgram() should be called when (1) the previous program was switched off "AND" (2) the PMT of the new program is available.
	if (pAcs->nProgramState == VA_PI_PROGRAM_READY)
	{
		pAcs->nProgramState = VA_PI_PROGRAM_SWITCH_ON;

		HxLOG_Info("VA_PI_SwitchOnProgram(%d) calling...\n", pAcs->nAcsIdx);
#ifdef __VIACCESS50__
		nErr = VA_CTRL_SwitchOnProgram(pAcs->nAcsIdx , pAcs->ulPmtLength, pAcs->aucPmt);
#else
		nErr = VA_PI_SwitchOnProgram(pAcs->nAcsIdx);
#endif
		if (nErr != kVA_OK /* && nErr != kVA_ALREADY_DONE */)
		{
			VA_SEM_Release(gVaPiSemId);
			HxLOG_Error("VA_PI_SwitchOnProgram(%d) return %d\n", pAcs->nAcsIdx, nErr);
			return;
		}
	}
	// VA_PI_PmtUpdated() should be called when (1) the version number of the PMT table changes "OR" (2) the program changes
	// After it calls the VA_PI_PmtUpdated(), the STB must:
	//   - call the VA_PI_UpdateStream() for all of the streams still in use
	//   - call the VA_PI_AddStream() for all of the new streams
	//   - call the VA_PI_RemoveStream() for all of the streams no longer in use if the number of streams has decreased
	else if (pAcs->nProgramState == VA_PI_PROGRAM_SWITCH_ON || pAcs->nProgramState == VA_PI_PROGRAM_UPDATE)
	{
		pAcs->nProgramState = VA_PI_PROGRAM_UPDATE;

		HxLOG_Info("VA_PI_PmtUpdated(%d) calling...\n", pAcs->nAcsIdx);
#ifdef __VIACCESS50__
		nErr = VA_CTRL_PmtUpdated(pAcs->nAcsIdx, pAcs->ulPmtLength, pAcs->aucPmt);
#else
		nErr = VA_PI_PmtUpdated(pAcs->nAcsIdx);
#endif
		if (nErr != kVA_OK)
		{
			VA_SEM_Release(gVaPiSemId);
			HxLOG_Error("VA_PI_PmtUpdated(%d) return %d\n", pAcs->nAcsIdx, nErr);
			return;
		}
	}
	else
	{
		VA_SEM_Release(gVaPiSemId);
		HxLOG_Error("wrong prog state:%x\n", pAcs->nProgramState);
		return;
	}

#ifdef DEBUG_PID_CHANGE
	{
		HUINT16 usServiceId;
		HUINT8 ucVerNum;
		HUINT32	ulNumOfElement, ulNewNumOfElement;
		SvcCas_PmtElement_t *pElementInfo, *pNewElementInfo;
		UINT32 ulEsNum;
		VA_PI_Stream *pEs;
		int i;

		if (pAcs->pstPmtInfo)
		{
			usServiceId = pAcs->pstPmtInfo->usServiceId;
			ucVerNum = pAcs->pstPmtInfo->ucVerNum;
			ulNumOfElement = pAcs->pstPmtInfo->ulNumOfElement;
			pElementInfo = pAcs->pstPmtInfo->stElementInfo;
		}
		else
		{
			usServiceId = 0;
			ucVerNum = 0;
			ulNumOfElement = 0;
			pElementInfo = NULL;
		}

		ulNewNumOfElement = pstPmtInfo->ulNumOfElement;
		pNewElementInfo = pstPmtInfo->stElementInfo;

		ulEsNum = pAcs->stStreamInfo.ulEsNum;
		pEs = pAcs->stStreamInfo.astEs;

		HxLOG_Print("=== VA_PI_Adapt_PmtChange() ======================\n");

		HxLOG_Print(" ** acsIndex = %d, nProgramState = %s\n", pAcs->nAcsIdx, VA_PI_INT_GetProgramStateString(pAcs->nProgramState));

		HxLOG_Print(" ** stStreamInfo : EsNum = %d\n", ulEsNum);
		for (i = 0; i < ulEsNum; i++)
		{
			HxLOG_Print("\t %d : PID = %x, dwStbStreamHandle = %x, dwVaStreamHandle = %x\n",
				i, pEs[i].usPid, pEs[i].ulStbStreamHandle, pEs[i].ulVaStreamHandle);
		}

		HxLOG_Print(" ** pstPmtInfo (old) : NumOfElement = %d, ServiceId = %x, VerNum = %x\n", ulNumOfElement, usServiceId, ucVerNum);
		for (i = 0; i < ulNumOfElement; i++)
		{
			HxLOG_Print("\t %d : PID = %x (%d)\n", i, pElementInfo[i].usPid, pElementInfo[i].eEsType);
		}

		HxLOG_Print(" ** pstPmtInfo (new) : NumOfElement = %d, ServiceId = %x, VerNum = %x\n", ulNewNumOfElement, pstPmtInfo->usServiceId, pstPmtInfo->ucVerNum);
		for (i = 0; i < ulNewNumOfElement; i++)
		{
			HxLOG_Print("\t %d : PID = %x (%d)\n", i, pNewElementInfo[i].usPid, pNewElementInfo[i].eEsType);
		}

		HxLOG_Print("==================================================\n");
	}
#endif

	if (pAcs->pstPmtInfo == NULL)
	{
		if ((pAcs->pstPmtInfo = VA_MEM_Alloc(sizeof(SvcCas_PmtInfo_t))) == NULL)
		{
			VA_SEM_Release(gVaPiSemId);
			HxLOG_Error("can't allocate memory\n");
			return;
		}
	}

	// PMT 저장
	VA_memcpy(pAcs->pstPmtInfo, pstPmtInfo, sizeof(SvcCas_PmtInfo_t));
	local_va_pi_adapt_StreamChange(pAcs, TRUE);

	VA_SEM_Release(gVaPiSemId);
	HxLOG_Print("[Exit]\n");
}

void VA_PI_Adapt_SetPid(Handle_t hVaAction, SvcCas_SetPidInfo_t *pstEsPidInfo)
{
	VA_PI_AcsInstance *pAcs;
	HxLOG_Print("[Enter][stbHandle:%x]\n", hVaAction);

#if 0 // local_vaact_SetPid() 에서 체크함.
	if (pstEsPidInfo == NULL)
	{
		HxLOG_Error("pstEsPidInfo is NULL\n");
		return;
	}
#endif

	VA_ASSERT(gVaPiSemId);
	VA_SEM_Get(gVaPiSemId);

	pAcs = local_va_pi_GetAcsInst(hVaAction);

	if (pAcs == NULL)
	{
		VA_SEM_Release(gVaPiSemId);
		HxLOG_Error("wrong hVaAction(%x)\n", hVaAction);
		return;
	}
#if defined(CONFIG_DEBUG)
	HxLOG_Info("nProgramState = %s\n", VA_PI_INT_GetProgramStateString(pAcs->nProgramState));
#endif
	if (pAcs->nProgramState == VA_PI_PROGRAM_SWITCH_OFF)
	{
		VA_SEM_Release(gVaPiSemId);
		HxLOG_Error("program is not switched on\n");
		return;
	}

#ifdef DEBUG_PID_CHANGE
 	{
		HUINT32 ulPidNum, ulNewPidNum;
		SvcCas_PidInfo_t *pPidInfo, *pNewPidInfo;
		UINT32 ulEsNum;
		VA_PI_Stream *pEs;
		int i;

		ulPidNum = pAcs->stPidInfo.ulPidNum;
		pPidInfo = pAcs->stPidInfo.stPidInfo;

		ulNewPidNum = pstEsPidInfo->ulPidNum;
		pNewPidInfo = pstEsPidInfo->stPidInfo;

		ulEsNum = pAcs->stStreamInfo.ulEsNum;
		pEs = pAcs->stStreamInfo.astEs;

		HxLOG_Print("=== VA_PI_Adapt_SetPid() =========================\n");

		HxLOG_Print(" ** acsIndex = %d, nProgramState = %s\n", pAcs->nAcsIdx, VA_PI_INT_GetProgramStateString(pAcs->nProgramState));

		HxLOG_Print(" ** stStreamInfo : EsNum = %d\n", ulEsNum);
		for (i = 0; i < ulEsNum; i++)
		{
			HxLOG_Print("\t %d : PID = %x, dwStbStreamHandle = %x, dwVaStreamHandle = %x\n",
				i, pEs[i].usPid, pEs[i].ulStbStreamHandle, pEs[i].ulVaStreamHandle);
		}

		HxLOG_Print(" ** stPidInfo (old) : PidNum = %d\n", ulPidNum);
		for (i = 0; i < ulPidNum; i++)
		{
			HxLOG_Print("\t %d : PID = %x\n", i, pPidInfo[i].usPid);
		}

		HxLOG_Print(" ** stPidInfo (new) : PidNum = %d\n", ulNewPidNum);
		for (i = 0; i < ulNewPidNum; i++)
		{
			HxLOG_Print("\t %d : PID = %x\n", i, pNewPidInfo[i].usPid);
		}

		HxLOG_Print("==================================================\n");
	}
#endif

//## 아래코드는 merge후에 확인이 필요하네요..
	// SetPid 저장
	#if 0 // org
	VA_memcpy(&pAcs->stPidInfo, pstEsPidInfo, sizeof(SvcCas_SetPidInfo_t));
	#else // INVALID_PID 는 처음부터 저장하지 않도록 한다.
	{
		HUINT32 ulPidNum, ulNewPidNum;
		SvcCas_PidInfo_t *pPidInfo, *pNewPidInfo;
		int i;

		ulPidNum = 0;
		pPidInfo = pAcs->stPidInfo.stPidInfo;

		ulNewPidNum = pstEsPidInfo->ulPidNum;
		pNewPidInfo = pstEsPidInfo->stPidInfo;

		for (i = 0; i < ulNewPidNum; i++)
		{
			if (VALID_PID(pNewPidInfo[i].usPid))
			{
				VA_memcpy(&pPidInfo[ulPidNum++], &pNewPidInfo[i], sizeof(SvcCas_PidInfo_t));
			}
		}
		pAcs->stPidInfo.ulPidNum = ulPidNum;
	}
	#endif

	local_va_pi_adapt_StreamChange(pAcs, FALSE);

	VA_SEM_Release(gVaPiSemId);

//이거 호출안되고 있는데... 빼버리자..
//		VA_UI_Adapt_SendLastEvent(pAcs->nAcsIdx);

	HxLOG_Print("[Exit]\n");
}

void VA_PI_Adapt_CatChange(Handle_t hVaAction, UINT16 usUniqueId, UINT8 *pucCat)
{
	int nLength, nErr;
	VA_PI_AcsInstance *pAcs;
#if !defined(CONFIG_MW_CAS_VIACCESS_PVR)
	SvcCas_ActionType_e eActionType;
#endif

	HxLOG_Print("[Enter][stbHandle:%x]\n", hVaAction);

	VA_ASSERT(gVaPiSemId);
	VA_SEM_Get(gVaPiSemId);

	pAcs = local_va_pi_GetAcsInst(hVaAction);

	if (pAcs == NULL)
	{
		VA_SEM_Release(gVaPiSemId);
		HxLOG_Error("wrong hVaAction(%x)\n", hVaAction);
		return;
	}

#if defined (CONFIG_MW_CAS_VIACCESS_PVR)
	if (pAcs->nAcsMode != eLIVE)
#else
	eActionType = VAMGR_GetActType(hVaAction);
//	HxLOG_Info("%s\n", svc_cas_DbgMakeStrActionType(eActionType));

	if (eActionType != eCAS_ActionType_Live)
#endif
	{
		VA_SEM_Release(gVaPiSemId);
		HxLOG_Error("Recording service..\n");
		return;
	}

	if (pAcs->usUniqueTsId != usUniqueId)
	{
		VA_SEM_Release(gVaPiSemId);
		HxLOG_Error("wrong UniqueId (%x,%x)\n", pAcs->usUniqueTsId, usUniqueId);
		return;
	}

	nLength = Get12bit(pucCat + 1) + 3;

#if 0 // local_vaact_UpdateCat() 에서 체크하도록 함.
	if (pucCat[0] != 0x01 || nLength > VA_SI_SECTION_SIZE || !(pucCat[5] & 0x1))
	{
		VA_SEM_Release(gVaPiSemId);
		HxLOG_Error("wrong CAT (%x, %d, %d)\n", pucCat[0], nLength, pucCat[5] & 0x1);
		return;
	}
#endif

	VA_memcpy(pAcs->aucCat, pucCat, nLength);
	pAcs->ulCatLength = nLength;

	pAcs->bCatReceived = TRUE;

	HxLOG_Info("VA_PI_CatUpdated(%d) calling...\n", pAcs->nAcsIdx);
#ifdef __VIACCESS50__
	nErr = VA_CTRL_CatUpdated(pAcs->nAcsIdx, pAcs->ulCatLength, pAcs->aucCat);
#else
	nErr = VA_PI_CatUpdated(pAcs->nAcsIdx);
#endif
	if (nErr != kVA_OK)
	{
		HxLOG_Error("VA_PI_CatUpdated(%d) return %d\n", pAcs->nAcsIdx, nErr);
	}

	VA_SEM_Release(gVaPiSemId);
	HxLOG_Print("[Exit]\n");
}


/////////////////////////////////////////////////////////////////////
// Viaccess API
/////////////////////////////////////////////////////////////////////

/*
	VA_PI_ParseCatDescriptors
		parse descriptors of CAT table
	arguments
		dwAcsId
	return
		kVA_OK
		kVA_INVALID_PARAMETER
		kVA_OPEN_ACS_FIRST
		kVA_DATA_NOT_AVAILABLE
*/
#if !defined(__VIACCESS50__)
INT VA_PI_ParseCatDescriptors(DWORD dwAcsId)
{
	VA_PI_AcsInstance *pAcs;
	UINT8 *pucDesc, *pucEnd;
	int nCnt, nErr;

	HxLOG_Print("[Enter]\n");

	// wrong dwAcsId
	VA_PI_IfNotValidAcsId(dwAcsId, kVA_INVALID_PARAMETER);

#if defined(USE_SEMA_TO_ALL_PI_FUNCS_CALLED_BY_VA)
	VA_ASSERT(gVaPiSemId);
	VA_SEM_Get(gVaPiSemId);
#endif

	pAcs = pstVaAcsInstance[dwAcsId];

	// ACS is not opened
	if (pAcs->nAcsState == VA_PI_ACS_CLOSE)
	{
#if defined(USE_SEMA_TO_ALL_PI_FUNCS_CALLED_BY_VA)
		VA_SEM_Release(gVaPiSemId);
#endif
		HxLOG_Error("ACS is not opened\n");
		return kVA_OPEN_ACS_FIRST;
	}

	// CAT is not received yet
	if (pAcs->bCatReceived == FALSE)
	{
#if defined(USE_SEMA_TO_ALL_PI_FUNCS_CALLED_BY_VA)
		VA_SEM_Release(gVaPiSemId);
#endif
		HxLOG_Error("No CAT is available\n");
		return kVA_DATA_NOT_AVAILABLE;
	}

	// parse cat
	// call VA_PI_CatDescParsingFct() for each descriptor
	// aucCat has raw CAT data (header / descriptors / CRC)
	pucDesc = pAcs->aucCat + 8;
	pucEnd  = pAcs->aucCat + pAcs->ulCatLength - 4; // excluding crc(4)

	for (nCnt = 0; pucDesc+pucDesc[1]+2 <= pucEnd; pucDesc += (pucDesc[1]+2), nCnt++)
	{
		#ifdef DEBUG_PRINT_DESCRIPTOR_AT_CAT
 		{
			UINT8 *ptr = pucDesc;
			UINT8 idx, no;

			no = *(ptr+1) + 2;
			HxLOG_Print("CAT (%d) :", no);
			for (idx = 0; idx < no; idx++)
				HxLOG_Print(" %02X", *ptr++);
			HxLOG_Print("\n");
		}
		#endif

		HxLOG_Info("VA_PI_CatDescParsingFct(%d) calling...\n", dwAcsId);
		nErr = VA_PI_CatDescParsingFct(dwAcsId, pucDesc);
		if (nErr == kVA_DESCRIPTOR_CORRUPTED)
		{
			#ifdef CONFIG_MW_CAS_VIACCESS_DRIVER_TEST // VA driver test 중 abnoral test case 시에 의도된 에러 상황에 불필요하게 로그 찍지 않도록...
			HxLOG_Info("VA report corrupt\n");
			#else
			HxLOG_Error("VA report corrupt\n");
			#endif
		}
		else if (nErr == kVA_NEXT_DESCRIPTOR)
		{
			HxLOG_Info("VA ask next\n");
		}
		else if (nErr == kVA_NO_MORE_DESCRIPTOR)
		{
			HxLOG_Info("VA ask to stop\n");
			break;
		}
		else
		{
			HxLOG_Error("VA_PI_CatDescParsingFct(%d) return %d\n", dwAcsId, nErr);
			break;
		}
	}


#if defined(USE_SEMA_TO_ALL_PI_FUNCS_CALLED_BY_VA)
	VA_SEM_Release(gVaPiSemId);
#endif

	HxLOG_Print("[Exit]\n");
	return kVA_OK;
}

/*
	VA_PI_ParsePmtProgramDescriptors
		parse PMT descriptors in proram level
	arguments
		dwAcsId
	return
		kVA_OK
		kVA_INVALID_PARAMETER
		kVA_OPEN_ACS_FIRST
		kVA_SWITCH_ON_PROGRAM_FIRST
*/
INT VA_PI_ParsePmtProgramDescriptors(DWORD dwAcsId)
{
	VA_PI_AcsInstance *pAcs;
	int i, nErr;

	HxLOG_Print("[Enter]\n");

	// wrong dwAcsId
	VA_PI_IfNotValidAcsId(dwAcsId, kVA_INVALID_PARAMETER);

#if defined(USE_SEMA_TO_ALL_PI_FUNCS_CALLED_BY_VA)
	VA_ASSERT(gVaPiSemId);
	VA_SEM_Get(gVaPiSemId);
#endif

	pAcs = pstVaAcsInstance[dwAcsId];

	// ACS is not opened
	if (pAcs->nAcsState == VA_PI_ACS_CLOSE)
	{
#if defined(USE_SEMA_TO_ALL_PI_FUNCS_CALLED_BY_VA)
		VA_SEM_Release(gVaPiSemId);
#endif
		HxLOG_Error("ACS is not opened\n");
		return kVA_OPEN_ACS_FIRST;
	}

	// current program is switched off
	if (pAcs->nProgramState == VA_PI_PROGRAM_SWITCH_OFF)
	{
#if defined(USE_SEMA_TO_ALL_PI_FUNCS_CALLED_BY_VA)
		VA_SEM_Release(gVaPiSemId);
#endif
		HxLOG_Error("current program is switched off\n");
		return kVA_SWITCH_ON_PROGRAM_FIRST;
	}

	// parse pmt
	// call VA_PI_PmtProgramDescParsingFct() for each descriptor
	if (pAcs->pstPmtInfo)
	{
		for (i = 0; i < pAcs->pstPmtInfo->ulNumOfProgramCaDescriptor; i++)
		{
			#ifdef DEBUG_PRINT_DESCRIPTOR_AT_PMT
 			{
				UINT8 *ptr = pAcs->pstPmtInfo->stProgramCaDescriptor[i].pucRaw;
				UINT8 idx, no;

				no = *(ptr+1) + 2;
				HxLOG_Print("[%d/%d] [%X,%X] PMT (prog)(%d) :",
					i+1, pAcs->pstPmtInfo->ulNumOfProgramCaDescriptor, pAcs->pstPmtInfo->stProgramCaDescriptor[i].usCaSysId, pAcs->pstPmtInfo->stProgramCaDescriptor[i].usCaPid, no);
				for (idx = 0; idx < no; idx++)
					HxLOG_Print(" %02X", *ptr++);
				HxLOG_Print("\n");
			}
			#endif

			HxLOG_Info("VA_PI_PmtProgramDescParsingFct(%d) calling...\n", dwAcsId);
			nErr = VA_PI_PmtProgramDescParsingFct(dwAcsId, pAcs->pstPmtInfo->stProgramCaDescriptor[i].pucRaw);
			if (nErr == kVA_DESCRIPTOR_CORRUPTED)
			{
				#ifdef CONFIG_MW_CAS_VIACCESS_DRIVER_TEST // VA driver test 중 abnoral test case 시에 의도된 에러 상황에 불필요하게 로그 찍지 않도록...
				HxLOG_Info("VA report corrupt\n");
				#else
				HxLOG_Error("VA report corrupt\n");
				#endif
			}
			else if (nErr == kVA_NEXT_DESCRIPTOR)
			{
				HxLOG_Info("VA ask next\n");
			}
			else if (nErr == kVA_NO_MORE_DESCRIPTOR)
			{
				HxLOG_Info("VA ask to stop\n");
				break;
			}
			else
			{
				HxLOG_Error("VA_PI_PmtProgramDescParsingFct(%d) return %d\n", dwAcsId, nErr);
				break;
			}
		}
	}

#if defined(USE_SEMA_TO_ALL_PI_FUNCS_CALLED_BY_VA)
	VA_SEM_Release(gVaPiSemId);
#endif

	HxLOG_Print("[Exit]\n");
	return kVA_OK;
}

/*
	VA_PI_NotifyStreamEvent
		notify the STB about events occuring in a given stream
	arguments
		wStbStreamHandle: assigned by STB and given by VA_PI_AddStream()
		eStreamEvent
	return
		kVA_OK
		kVA_INVALID_PARAMETER
*/
INT VA_PI_NotifyStreamEvent(DWORD wStbStreamHandle, tVA_PI_StreamEvent eStreamEvent)
{
	int nAcsId, nStreamId;
	VA_PI_AcsInstance *pAcs;
	VA_PI_Stream *pStream;

	HxLOG_Print("[Enter][%x][%x]\n", wStbStreamHandle, eStreamEvent);

	nAcsId = VA_PI_GetAcsId(wStbStreamHandle);
	nStreamId = VA_PI_GetStreamId(wStbStreamHandle);

	// dwStbStreamHandle doesn't correspond to an added stream
	VA_PI_IfNotValidAcsId(nAcsId, kVA_INVALID_PARAMETER);
	VA_PI_IfNotValidStreamId(nStreamId, kVA_INVALID_PARAMETER);

#if defined(USE_SEMA_TO_ALL_PI_FUNCS_CALLED_BY_VA)
	VA_ASSERT(gVaPiSemId);
	VA_SEM_Get(gVaPiSemId);
#endif

	pAcs = pstVaAcsInstance[nAcsId];
	pStream = &pAcs->stStreamInfo.astEs[nStreamId];

	if (pStream->usPid == VA_INVALID_PID || pStream->ulVaStreamHandle == VA_INVALID_HANDLE)
	{
#if defined(USE_SEMA_TO_ALL_PI_FUNCS_CALLED_BY_VA)
		VA_SEM_Release(gVaPiSemId);
#endif
		HxLOG_Error("[%x,%x,%x] stream %x is invalid (%x,%x)\n",
			wStbStreamHandle, nAcsId, nStreamId, pStream->usPid, pStream->ulStbStreamHandle, pStream->ulVaStreamHandle);

		return kVA_INVALID_PARAMETER;
	}

	// eStreamEvent is not a value from tVA_PI_StreamEvent enumeration
	if (eStreamEvent > eQUERY_YES)
	{
#if defined(USE_SEMA_TO_ALL_PI_FUNCS_CALLED_BY_VA)
		VA_SEM_Release(gVaPiSemId);
#endif
		#ifdef CONFIG_MW_CAS_VIACCESS_DRIVER_TEST // VA driver test 중 abnoral test case 시에 의도된 에러 상황에 불필요하게 로그 찍지 않도록...
		HxLOG_Info("wrong eStreamEvent\n");
		#else
		HxLOG_Error("wrong eStreamEvent\n");
		#endif

		return kVA_INVALID_PARAMETER;
	}

	// handle event
	if (pStream->nState != eStreamEvent)
	{
		pStream->nState = eStreamEvent;
		HxLOG_Info("[%x] stream %x's nState is %d\n", wStbStreamHandle, pStream->usPid, pStream->nState);

#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
		if (pAcs->nAcsMode == ePERSISTENT_PLAYBACK || pAcs->nAcsMode == eTIMESHIFT_PLAYBACK)
		{
			int i;

			if (0)
			{
				HERROR nErr;
				VA_UI_PLAYBACK_INFO stInfo;

				nErr = VA_UI_Adapt_GetPlaybackInfo(pAcs->nAcsIdx, &stInfo);
				if (nErr == VA_ERR_OK)
					VA_OS_Printf("**** PLAYBACK INFO (cnt:%d, record:%s, play:%s) ******\n", stInfo.ulPlaybackCounter, stInfo.szRecordDate, stInfo.szFirstPlaybackDate);
				else
					VA_OS_Printf("**** PLAYBACK INFO (ERR:%d)  ******\n", nErr);
			}
			if (eStreamEvent == eVIACCESS_NOT_DESCRAMBLING)
			{
//VA_OS_Printf("-->PLAYBACK RIGHT KO (%d, %x)\n", pAcs->nAcsMode, pStream->esType);
				// ## 안보여준다.
				for (i = 0; i < pAcs->stPidInfo.ulPidNum; i++)
				{
					if (pStream->usPid == pAcs->stPidInfo.stPidInfo[i].usPid)
					{
						// 선택된 pid만 처리
						if (pStream->esType == eEsType_Video)
						{
							VA_OutputMute(TRUE, VA_MUTE_PB_VIDEO);
						}
						if (pStream->esType == eEsType_Audio)
						{
							VA_OutputMute(TRUE, VA_MUTE_PB_AUDIO);
						}
						break;
					}
				}
			}
			else if (eStreamEvent == eVIACCESS_DESCRAMBLING)
			{
//VA_OS_Printf("-->PLAYBACK RIGHT OK (%d, %x)\n", pAcs->nAcsMode, pStream->esType);
				//## 보여준다.ㅋ
				for (i = 0; i < pAcs->stPidInfo.ulPidNum; i++)
				{
					if (pStream->usPid == pAcs->stPidInfo.stPidInfo[i].usPid)
					{
						// 선택된 pid만 처리
						if (pStream->esType == eEsType_Video)
						{
							VA_OutputMute(FALSE, VA_MUTE_PB_VIDEO);
						}
						if (pStream->esType == eEsType_Audio)
						{
							VA_OutputMute(FALSE, VA_MUTE_PB_AUDIO);
						}
						break;
					}
				}

				if (pAcs->nAcsMode == ePERSISTENT_PLAYBACK && pAcs->bPlaybackStarted == FALSE)
				{
//VA_OS_Printf("VA_UI_Adapt_IncrementPlaybackCount()\n");
					pAcs->bPlaybackStarted = TRUE;
					// playback 시킬 수 있는 상황이니 play count를 증가시킨다.
					//VA_UI_Adapt_IncrementPlaybackCount(pAcs->nAcsIdx);
					VA_PVR_IncrementPlaybackCount(pAcs->nAcsIdx);
				}
			}
			else
			{
				//## 나머지는 무시
			}
		}
#endif
	}

#if defined(USE_SEMA_TO_ALL_PI_FUNCS_CALLED_BY_VA)
	VA_SEM_Release(gVaPiSemId);
#endif

	HxLOG_Print("[Exit]\n");
	return kVA_OK;
}

/*
	VA_PI_NotifyStreamRecordPolicy
		supply the STB with the record policy associated to a given elementary stream (only used when ACS mode includes recording)
	arguments
		wStbStreamHandle: assigned by STB and given by VA_PI_AddStream()
		eStreamRecordPolicy
	return
		kVA_OK
		kVA_INVALID_PARAMETER
*/
INT VA_PI_NotifyStreamRecordPolicy(DWORD wStbStreamHandle, tVA_PI_StreamRecordPolicy eStreamRecordPolicy)
{
	int nAcsId = 0, nStreamId;
	VA_PI_AcsInstance *pAcs;
	VA_PI_Stream *pStream;
	HERROR nErr;

	HxLOG_Print("[Enter]\n");

	nAcsId = VA_PI_GetAcsId(wStbStreamHandle);
	nStreamId = VA_PI_GetStreamId(wStbStreamHandle);

	// dwStbStreamHandle doesn't correspond to an added stream
	VA_PI_IfNotValidAcsId(nAcsId, kVA_INVALID_PARAMETER);
	VA_PI_IfNotValidStreamId(nStreamId, kVA_INVALID_PARAMETER);

#if defined(USE_SEMA_TO_ALL_PI_FUNCS_CALLED_BY_VA)
	VA_ASSERT(gVaPiSemId);
	VA_SEM_Get(gVaPiSemId);
#endif

	pAcs = pstVaAcsInstance[nAcsId];
	pStream = &pAcs->stStreamInfo.astEs[nStreamId];

	if (pStream->usPid == VA_INVALID_PID || pStream->ulVaStreamHandle == VA_INVALID_HANDLE)
	{
#if defined(USE_SEMA_TO_ALL_PI_FUNCS_CALLED_BY_VA)
		VA_SEM_Release(gVaPiSemId);
#endif
		HxLOG_Error("[%x,%x,%x] stream %x is invalid (%x,%x)\n",
			wStbStreamHandle, nAcsId, nStreamId, pStream->usPid, pStream->ulStbStreamHandle, pStream->ulVaStreamHandle);
		return kVA_INVALID_PARAMETER;
	}

	// eStreamRecordPolicy is not a value from tVA_PI_StreamRecordPolicy enumeration
	if (eStreamRecordPolicy > eTIMESHIFT_FORBIDDEN)
	{
#if defined(USE_SEMA_TO_ALL_PI_FUNCS_CALLED_BY_VA)
		VA_SEM_Release(gVaPiSemId);
#endif
		HxLOG_Error("wrong eStreamRecordPolicy\n");
		return kVA_INVALID_PARAMETER;
	}

	// eStreamRecordPolicy is not compatible with ACS operating mode (e.g. eTIMESHIDFT_SCRAMBLED is not compatible with eLIVE_AND_PERSISTENT_RECORD)
	switch (pAcs->nAcsMode)
	{
		case ePERSISTENT_RECORD:
		case ePERSISTENT_PLAYBACK:
		case eLIVE_AND_PERSISTENT_RECORD:
			if (eStreamRecordPolicy != eRECORD_SCRAMBLED && eStreamRecordPolicy != eRECORD_FORBIDDEN)
			{
#if defined(USE_SEMA_TO_ALL_PI_FUNCS_CALLED_BY_VA)
				VA_SEM_Release(gVaPiSemId);
#endif
				HxLOG_Error("eStreamRecordPolicy[%d], AcsMode[%d] mismatch\n", eStreamRecordPolicy, pAcs->nAcsMode);
				return kVA_INVALID_PARAMETER;
			}

#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
			if (eStreamRecordPolicy == eRECORD_FORBIDDEN)
			{
				//## record 제한되었음..처리 바람
				if (pAcs->bRecordForbidden != TRUE)
				{
					pAcs->bRecordForbidden = TRUE;

					nErr = VAACT_UpdateDrmInfo(VAMGR_GetActionHandle(pAcs->hVaAction), pAcs->bRecordForbidden, pAcs->bTsrForbidden);
					HxLOG_Info("[VA_PI_NotifyStreamRecordPolicy] VAACT_UpdateDrmInfo(%x,%x,%x)\n", pAcs->hVaAction, pAcs->bRecordForbidden, pAcs->bTsrForbidden);
				}
			}
			else
			{
				if (pAcs->bRecordForbidden != FALSE)
				{
					pAcs->bRecordForbidden = FALSE;

					nErr = VAACT_UpdateDrmInfo(VAMGR_GetActionHandle(pAcs->hVaAction), pAcs->bRecordForbidden, pAcs->bTsrForbidden);
					HxLOG_Info("[VA_PI_NotifyStreamRecordPolicy] VAACT_UpdateDrmInfo(%x,%x,%x)\n", pAcs->hVaAction, pAcs->bRecordForbidden, pAcs->bTsrForbidden);
				}
			}
#endif
			break;

		case eTIMESHIFT_RECORD:
		case eTIMESHIFT_PLAYBACK:
		case eLIVE_AND_TIMESHIFT_RECORD:
			if (eStreamRecordPolicy != eTIMESHIFT_SCRAMBLED && eStreamRecordPolicy != eTIMESHIFT_FORBIDDEN)
			{
#if defined(USE_SEMA_TO_ALL_PI_FUNCS_CALLED_BY_VA)
				VA_SEM_Release(gVaPiSemId);
#endif
				HxLOG_Error("eStreamRecordPolicy[%d], AcsMode[%d] mismatch\n", eStreamRecordPolicy, pAcs->nAcsMode);
				return kVA_INVALID_PARAMETER;
			}

#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
			if (eStreamRecordPolicy == eTIMESHIFT_FORBIDDEN)
			{
				//## TSR 제한되었음..처리 바람
				if (pAcs->bTsrForbidden != TRUE)
				{
					pAcs->bTsrForbidden = TRUE;

					nErr = VAACT_UpdateDrmInfo(VAMGR_GetActionHandle(pAcs->hVaAction), pAcs->bRecordForbidden, pAcs->bTsrForbidden);
					HxLOG_Info("[VA_PI_NotifyStreamRecordPolicy] VAACT_UpdateDrmInfo(%x,%x,%x)\n", pAcs->hVaAction, pAcs->bRecordForbidden, pAcs->bTsrForbidden);
				}
			}
			else
			{
				if (pAcs->bTsrForbidden != FALSE)
				{
					pAcs->bTsrForbidden = FALSE;

					nErr = VAACT_UpdateDrmInfo(VAMGR_GetActionHandle(pAcs->hVaAction), pAcs->bRecordForbidden, pAcs->bTsrForbidden);
					HxLOG_Info("[VA_PI_NotifyStreamRecordPolicy] VAACT_UpdateDrmInfo(%x,%x,%x)\n", pAcs->hVaAction, pAcs->bRecordForbidden, pAcs->bTsrForbidden);
				}
			}
#endif
			break;

		case eLIVE_AND_TIMESHIFT_RECORD_AND_PERSISTENT_RECORD:
		case eTIMESHIFT_RECORD_AND_PERSISTENT_RECORD:
		case eTIMESHIFT_PLAYBACK_AND_PERSISTENT_RECORD:
			break;

		case eLIVE:
#if defined(USE_SEMA_TO_ALL_PI_FUNCS_CALLED_BY_VA)
			VA_SEM_Release(gVaPiSemId);
#endif
			HxLOG_Info("eStreamRecordPolicy[%d], AcsMode[%d] mismatch\n", eStreamRecordPolicy, pAcs->nAcsMode);
			return kVA_INVALID_PARAMETER;

		default:
#if defined(USE_SEMA_TO_ALL_PI_FUNCS_CALLED_BY_VA)
			VA_SEM_Release(gVaPiSemId);
#endif
			HxLOG_Info("unknown eStreamRecordPolicy[%d]\n", eStreamRecordPolicy);
			return kVA_INVALID_PARAMETER;
	}

	//* handle event
	//* this function can be called twice if ACS mode includes two kinds of recording (persistence and time-shifting)

#if defined(USE_SEMA_TO_ALL_PI_FUNCS_CALLED_BY_VA)
	VA_SEM_Release(gVaPiSemId);
#endif

	HxLOG_Print("[Exit]\n");
	return kVA_OK;
}

/*
	VA_PI_ParsePmtStreamDescriptors
		parse PMT descriptors in elementary stream level
	arguments
		wStbStreamHandle: assigned by STB and given by VA_PI_AddStream()
	return
		kVA_OK
		kVA_INVALID_PARAMETER
*/
INT VA_PI_ParsePmtStreamDescriptors(DWORD wStbStreamHandle)
{
	int nAcsId, nStreamId;
	VA_PI_AcsInstance *pAcs;
	VA_PI_Stream *pStream;
	int i, nErr;

	HxLOG_Print("[Enter]\n");

	nAcsId = VA_PI_GetAcsId(wStbStreamHandle);
	nStreamId = VA_PI_GetStreamId(wStbStreamHandle);

	// dwStbStreamHandle doesn't correspond to an added stream
	VA_PI_IfNotValidAcsId(nAcsId, kVA_INVALID_PARAMETER);
	VA_PI_IfNotValidStreamId(nStreamId, kVA_INVALID_PARAMETER);

#if defined(USE_SEMA_TO_ALL_PI_FUNCS_CALLED_BY_VA)
	VA_ASSERT(gVaPiSemId);
	VA_SEM_Get(gVaPiSemId);
#endif

	pAcs = pstVaAcsInstance[nAcsId];
	pStream = &pAcs->stStreamInfo.astEs[nStreamId];

	if (pStream->usPid == VA_INVALID_PID || pStream->ulVaStreamHandle == VA_INVALID_HANDLE)
	{
#if defined(USE_SEMA_TO_ALL_PI_FUNCS_CALLED_BY_VA)
		VA_SEM_Release(gVaPiSemId);
#endif
		HxLOG_Error("[%x,%x,%x] stream %x is invalid (%x,%x)\n",
			wStbStreamHandle, nAcsId, nStreamId, pStream->usPid, pStream->ulStbStreamHandle, pStream->ulVaStreamHandle);
		return kVA_INVALID_PARAMETER;
	}

	// parse pmt
	// call VA_PI_PmtStreamDescParsingFct() for each descriptor
	if (pAcs->pstPmtInfo)
	{
		for (i = 0; i < pStream->pPmtElement->ulNumOfCaDescriptor; i++)
		{
			#ifdef DEBUG_PRINT_DESCRIPTOR_AT_PMT
 			{
				UINT8 *ptr = pStream->pPmtElement->stElementCaDescriptor[i].pucRaw;
				UINT8 idx, no;

				no = *(ptr+1) + 2;
				HxLOG_Print("[%d/%d] [%X,%X] PMT (stream)(%d) :",
					i+1, pStream->pPmtElement->ulNumOfCaDescriptor, pStream->pPmtElement->stElementCaDescriptor[i].usCaSysId, pStream->pPmtElement->stElementCaDescriptor[i].usCaPid, no);
				for (idx = 0; idx < no; idx++)
					HxLOG_Print(" %02X", *ptr++);
				HxLOG_Print("\n");
			}
			#endif

			HxLOG_Info("VA_PI_PmtStreamDescParsingFct(%x) calling...\n", pStream->ulVaStreamHandle);
			nErr = VA_PI_PmtStreamDescParsingFct(pStream->ulVaStreamHandle, pStream->pPmtElement->stElementCaDescriptor[i].pucRaw);
			if (nErr == kVA_DESCRIPTOR_CORRUPTED)
			{
				#ifdef CONFIG_MW_CAS_VIACCESS_DRIVER_TEST // VA driver test 중 abnoral test case 시에 의도된 에러 상황에 불필요하게 로그 찍지 않도록...
				HxLOG_Info("VA report corrupt\n");
				#else
				HxLOG_Error("VA report corrupt\n");
				#endif
			}
			else if (nErr == kVA_NEXT_DESCRIPTOR)
			{
				HxLOG_Info("VA ask next\n");
			}
			else if (nErr == kVA_NO_MORE_DESCRIPTOR)
			{
				HxLOG_Info("VA ask to stop\n");
				break;
			}
			else
			{
				HxLOG_Error("VA_PI_PmtStreamDescParsingFct(%d) return %d\n", pStream->ulVaStreamHandle, nErr);
				break;
			}
		}
	}

#if defined(USE_SEMA_TO_ALL_PI_FUNCS_CALLED_BY_VA)
	VA_SEM_Release(gVaPiSemId);
#endif

	HxLOG_Print("[Exit]\n");
	return kVA_OK;
}
#endif
/*
	VA_PI_GetStreamInformation
		retrieve information on a given stream
	arguments
		wStbStreamHandle: assigned by STB and given by VA_PI_AddStream()
		pStreamInfo: allocated and freed by VA
	return
		kVA_OK
		kVA_INVALID_PARAMETER
*/
#if 1//defined(_VIACCESS50_DISABLECODE_BEFORE_RELEASE_)
// hjlee3, temp
// va lib 에서 직접 호출한다.
// 릴리즈시에 막는다.

INT VA_PI_GetStreamInformation(DWORD wStbStreamHandle, tVA_PI_StreamInformation *pStreamInfo)
{
	int nAcsId = 0, nStreamId;
	VA_PI_AcsInstance *pAcs;
	VA_PI_Stream *pStream;

	HxLOG_Print("[Enter][%x]\n", wStbStreamHandle);

	if (pStreamInfo == NULL)
	{
		#ifdef CONFIG_MW_CAS_VIACCESS_DRIVER_TEST // VA driver test 중 abnoral test case 시에 의도된 에러 상황에 불필요하게 로그 찍지 않도록...
		HxLOG_Info("pStreamInfo = NULL\n");
		#else
		HxLOG_Error("pStreamInfo = NULL\n");
		#endif
		return kVA_INVALID_PARAMETER;
	}

	nAcsId = VA_PI_GetAcsId(wStbStreamHandle);
	nStreamId = VA_PI_GetStreamId(wStbStreamHandle);

	// dwStbStreamHandle doesn't correspond to an added stream
	VA_PI_IfNotValidAcsId(nAcsId, kVA_INVALID_PARAMETER);
	VA_PI_IfNotValidStreamId(nStreamId, kVA_INVALID_PARAMETER);

#if defined(USE_SEMA_TO_ALL_PI_FUNCS_CALLED_BY_VA)
	VA_ASSERT(gVaPiSemId);
	VA_SEM_Get(gVaPiSemId);
#endif

	pAcs = pstVaAcsInstance[nAcsId];
	pStream = &pAcs->stStreamInfo.astEs[nStreamId];

	/*
		pStream->pPmtElement의 address가 NULL인 경우가 있다??? 이러면 안되는데...
	*/
	if (pStream->usPid == VA_INVALID_PID || pStream->ulVaStreamHandle == VA_INVALID_HANDLE || pStream->pPmtElement == NULL)
	{
#if defined(USE_SEMA_TO_ALL_PI_FUNCS_CALLED_BY_VA)
		VA_SEM_Release(gVaPiSemId);
#endif
		HxLOG_Error("[%x,%x,%x] stream %x is invalid (%x,%x,%x)\n",
			wStbStreamHandle, nAcsId, nStreamId, pStream->usPid, pStream->ulStbStreamHandle, pStream->ulVaStreamHandle, pStream->pPmtElement);
		return kVA_INVALID_PARAMETER;
	}

	// get stream info
	pStreamInfo->byType = pStream->pPmtElement->ucStreamType; // stream type in PMT
	pStreamInfo->wPid = pStream->usPid;

#if defined(USE_SEMA_TO_ALL_PI_FUNCS_CALLED_BY_VA)
	VA_SEM_Release(gVaPiSemId);
#endif

	HxLOG_Info("[handle:%x, id:%x, type:%d, pid:%x]\n", wStbStreamHandle, nStreamId, pStreamInfo->byType, pStreamInfo->wPid);
	HxLOG_Print("[Exit]\n");
	return kVA_OK;
}

INT VA_PI_GetProgramInformation(DWORD dwAcsId, tVA_PI_ProgramInformation *pProgramInfo)
{
	VA_PI_AcsInstance *pAcs;
	HUINT16 usOrgNetId, usTsId, usUniqueId, usServiceId;
	HERROR nErr;

	HxLOG_Print("[Enter][%x]\n", dwAcsId);

	// wrong dwAcsId
	VA_PI_IfNotValidAcsId(dwAcsId, kVA_INVALID_PARAMETER);

	if (pProgramInfo == NULL)
	{
		#ifdef CONFIG_MW_CAS_VIACCESS_DRIVER_TEST // VA driver test 중 abnoral test case 시에 의도된 에러 상황에 불필요하게 로그 찍지 않도록...
		HxLOG_Info("pProgramInfo = NULL\n");
		#else
		HxLOG_Error("pProgramInfo = NULL\n");
		#endif
		return kVA_INVALID_PARAMETER;
	}

#if defined(USE_SEMA_TO_ALL_PI_FUNCS_CALLED_BY_VA)
	VA_ASSERT(gVaPiSemId);
	VA_SEM_Get(gVaPiSemId);
#endif

	pAcs = pstVaAcsInstance[dwAcsId];

	// ACS is not opened
	if (pAcs->nAcsState == VA_PI_ACS_CLOSE)
	{
#if defined(USE_SEMA_TO_ALL_PI_FUNCS_CALLED_BY_VA)
		VA_SEM_Release(gVaPiSemId);
#endif
		HxLOG_Error("ACS is not opened\n");
		return kVA_OPEN_ACS_FIRST;
	}

	// current program is switched off
	if (pAcs->nProgramState == VA_PI_PROGRAM_SWITCH_OFF)
	{
#if defined(USE_SEMA_TO_ALL_PI_FUNCS_CALLED_BY_VA)
		VA_SEM_Release(gVaPiSemId);
#endif
		HxLOG_Error("current program is switched off\n");
		return kVA_SWITCH_ON_PROGRAM_FIRST;
	}

#if 0 // 아래의 상황은 가정하지 않는다.
	if (0)
	{
		HxLOG_Info("[VA_PI_GetProgramInformation] prog info not available yet\n");
		VA_SEM_Release(gVaPiSemId);
		return kVA_DATA_NOT_AVAILABLE;
	}
#endif

	// get program info
	nErr = VAMGR_GetTripleID(pAcs->hVaAction, &usOrgNetId, &usTsId, &usUniqueId, &usServiceId);
	if (nErr)
	{
		//* information is not received yet (e.g. NIT)
#if defined(USE_SEMA_TO_ALL_PI_FUNCS_CALLED_BY_VA)
		VA_SEM_Release(gVaPiSemId);
#endif
		HxLOG_Error("prog info not available yet\n");
		return kVA_DATA_NOT_AVAILABLE;
	}

	pProgramInfo->wNetworkId = usOrgNetId;		//이거는 이리 해도 되는지 나중에 다시 확인해봐야 할 듯 ...
	pProgramInfo->wOriginalNetworkId = usOrgNetId;
	pProgramInfo->wTransportStreamId = usTsId;
	pProgramInfo->wServiceId = usServiceId;

#if defined(USE_SEMA_TO_ALL_PI_FUNCS_CALLED_BY_VA)
	VA_SEM_Release(gVaPiSemId);
#endif

	HxLOG_Info("[NID:%x, ONID:%x, TSID:%x, SrvID:%x]\n", pProgramInfo->wNetworkId, pProgramInfo->wOriginalNetworkId, pProgramInfo->wTransportStreamId, pProgramInfo->wServiceId);
	HxLOG_Print("[Exit]\n");
	return kVA_OK;
}
#endif

