/**************************************************************************************/
/**
 * @file bplait.c
 *
 * AIT Interfaces
 *
 **************************************************************************************
 **/
#include <hlib.h>

#include <apk.h>

#include "bplait.h"
#include "bpldb.h"

/******************************************************************
	Define
******************************************************************/
#if 0
#define _bpl_ait_get32Bit(q) (unsigned int)(((*(unsigned char *)(q)) << 24) | (*((unsigned char *)(q)+1) << 16) | (*((unsigned char *)(q)+2) << 8) | (*((unsigned char *)(q)+3)) )
#endif


/******************************************************************
	Typedef
******************************************************************/
#if 0
typedef struct
{
	APKE_MEDIADEC_AitState_e		 eState;
	HUINT32							 ulSecNum;
	HUINT32							*pulSecCrcArray;
} _bpl_aitChecker_t;
#endif

/******************************************************************
	Static variables
******************************************************************/
static BplAitListener_t			 s_fnAitListener = NULL;
#if 0
static _bpl_aitChecker_t		*s_pstPrevAitChecker = NULL;
#endif

/******************************************************************
	Static functions
******************************************************************/
#if 0
static HUINT32 _bpl_ait_getRawCrc32 (HUINT8 *pucRaw, HUINT32 ulSecLen)
{
	if ((NULL != pucRaw) && (ulSecLen > 4))
	{
		return _bpl_ait_get32Bit (pucRaw + ulSecLen - 1);
	}

	return 0;
}

static _bpl_aitChecker_t *_bpl_ait_makeCurrAitChecker (void)
{
	APKE_MediaCtrl_Destination_e	 eDest = APK_MEDIACTRL_GetMainViewDestination();
	_bpl_aitChecker_t				*pstAitCheck;

	pstAitCheck = (_bpl_aitChecker_t *)HLIB_STD_MemCalloc (sizeof(_bpl_aitChecker_t));
	if (NULL == pstAitCheck)			{ return NULL; }

	APK_MEDIACTRL_LIVE_GetAitState (eDest, &(pstAitCheck->eState));
	APK_MEDIACTRL_LIVE_CountAitSection (eDest, &(pstAitCheck->ulSecNum));

	if ((eApkMctrlAitState_Found == pstAitCheck->eState) && (pstAitCheck->ulSecNum > 0))
	{
		HUINT32			 ulIndex, ulSecSize;
		HUINT32			*pulCrcArray, *pucRawSec;
		HERROR			 hErr;

		pulCrcArray = (HUINT32 *)HLIB_STD_MemCalloc (sizeof(HUINT32) * pstAitCheck->ulSecNum);
		if (NULL == pulCrcArray)
		{
			HLIB_STD_MemFree (pstAitCheck);
			return NULL;
		}

		for (ulIndex = 0; ulIndex < pstAitCheck->ulSecNum; ulIndex++)
		{
			ulSecSize = 0;
			pucRawSec = NULL;

			hErr = APK_MEDIACTRL_LIVE_GetAitSection (eDest, ulIndex, &pucRawSec, &ulSecSize);
			if (NULL != pucRawSec)
			{
				pulCrcArray[ulIndex] = _bpl_ait_getRawCrc32 (pucRawSec, ulSecSize);
				HLIB_STD_MemFree (pucRawSec);
			}
		}

		pstAitCheck->pulSecCrcArray = pulCrcArray;
	}

	return pstAitCheck;
}

static void _bpl_ait_freeAitChecker (_bpl_aitChecker_t *pstAitCheck)
{
	if (NULL != pstAitCheck)
	{
		if (NULL != pstAitCheck->pulSecCrcArray)
		{
			HLIB_STD_MemFree(pstAitCheck->pulSecCrcArray);
		}

		HLIB_STD_MemFree(pstAitCheck);
	}
}

static HBOOL _bpl_ait_compareAitChecker (_bpl_aitChecker_t *pstAit1, _bpl_aitChecker_t *pstAit2)
{
	HUINT32			 ulCount;

	if ((NULL == pstAit1) || (NULL == pstAit2))
	{
		return FALSE;
	}

	if (pstAit1->eState != pstAit2->eState)
	{
		return FALSE;
	}

	if (eApkMctrlAitState_Found == pstAit1->eState)
	{
		if ((pstAit1->ulSecNum != pstAit2->ulSecNum) || (0 == pstAit1->ulSecNum))
		{
			return FALSE;
		}

		for (ulCount = 0; ulCount < pstAit1->ulSecNum; ulCount++)
		{
			if (pstAit1->pulSecCrcArray[ulCount] != pstAit2->pulSecCrcArray[ulCount])
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}
#endif

static HBOOL _bpl_ait_CheckMediaPlayEvent(HUINT32 ulViewId, OxMediaPlay_MediaType_e eType, OxMediaPlay_Event_e eEvent, HBOOL *updated)
{
	*updated = FALSE;

	if (0)
	{
		if (eOxMP_MEDIATYPE_LIVE == eType)
		{
			*updated = TRUE;
		}
	}

	if ((eEvent == eOxMP_EVENT_LIVE_SvcChanged) ||
		(eEvent == eOxMP_EVENT_LIVE_SvcStopped) ||
		(eEvent == eOxMP_EVENT_SI_AIT_CHANGED) ||
		(eEvent == eOxMP_EVENT_TUNE_NoSignal))
	{
		*updated = TRUE;
	}

	return TRUE;
}

static void _bpl_ait_notifyMediaPlay ( HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eType, OxMediaPlay_Event_e eEvent, OxMediaPlay_NotifierData_t *pstNotifyData, void *pvUserData )
{
    HUINT32							 ulAitNum = 0;
    HERROR							 hErr;

    if (NULL != s_fnAitListener)
    {
    	switch( eEvent )
    	{
            case eOxMP_EVENT_LIVE_SvcChanged:
#if defined(CONFIG_TIMESTAMP_64)
                if ( eOxMP_TUNEMODE_NDT_TUNE & pstNotifyData->stEventNotify.ullParam2 )
#else
                if ( eOxMP_TUNEMODE_NDT_TUNE & pstNotifyData->stEventNotify.ulParam2 )
#endif
                {
                    (s_fnAitListener)(BPLAIT_EVENT_AIT_CHAN_CHANGED_BY_MHEG_NDT, ulSessionId);
                }
                else
                {
                    (s_fnAitListener)(BPLAIT_EVENT_AIT_CHAN_CHANGED, ulSessionId);
                }
            	break;

            case eOxMP_EVENT_LIVE_SvcStopped:
            	(s_fnAitListener)(BPLAIT_EVENT_AIT_CHAN_STOPPED, ulSessionId);
            	break;

            case eOxMP_EVENT_SI_AIT_CHANGED:
            	hErr = APK_MEDIA_PLAY_CountAitSection (ulViewId, &ulAitNum);
            	if (ERR_OK == hErr)
            	{
            		if (ulAitNum > 0)
            		{
            			(s_fnAitListener)(BPLAIT_EVENT_AIT, ulSessionId);
            		}
            		else
            		{
            			(s_fnAitListener)(BPLAIT_EVENT_AIT_NONE, ulSessionId);
            		}
            	}
            	break;

            case eOxMP_EVENT_TUNE_NoSignal:
            	(s_fnAitListener)(BPLAIT_EVENT_AIT_NOSIGNAL, ulSessionId);
            	break;

            default:
            	break;
    	}
    }
}

/******************************************************************
	Interfaces
******************************************************************/
BPLStatus BPL_AIT_SetListener (BplAitListener_t listener)
{
	HERROR	hErr;
	HUINT32 	ulViewId;

	hErr = APK_MEDIA_PLAY_GetMainViewId(&ulViewId);
	if( ERR_OK == hErr )
	{
		if (NULL != listener)
		{
			s_fnAitListener = listener;
			hErr = APK_MEDIA_PLAY_RegisterNotifier (ulViewId, _bpl_ait_notifyMediaPlay, _bpl_ait_CheckMediaPlayEvent, NULL);
			if( ERR_OK != hErr )
			{
				HxLOG_Error("[%s:%d] APK_MEDIA_PLAY_RegisterNotifier Error: \n", __FUNCTION__, __LINE__);
			}

			APK_MEDIA_PLAY_SetAitListenerStatus(ulViewId, TRUE);

		}
	}

	return BPL_STATUS_OK;
}

BPLStatus BPL_AIT_GetCount(unsigned int *pulCount)
{
	HUINT32							 ulAitNum = 0;
	HUINT32 						 ulViewId;
	HERROR							 hErr;

	hErr = APK_MEDIA_PLAY_GetMainViewId(&ulViewId);
	if (ERR_OK != hErr)
	{
		*pulCount = 0;
		HxLOG_Error("[%s:%d] APK_MEDIA_PLAY_GetMainViewId Error: \n", __FUNCTION__, __LINE__);
		return BPL_STATUS_ERROR;
	}

	hErr = APK_MEDIA_PLAY_CountAitSection (ulViewId, &ulAitNum);
	if (ERR_OK != hErr)
	{
		*pulCount = 0;
		HxLOG_Error("[%s:%d] APK_MEDIA_PLAY_CountAitSection Error: \n", __FUNCTION__, __LINE__);
		return BPL_STATUS_ERROR;
	}

	*pulCount = ulAitNum;
	return BPL_STATUS_OK;
}

BPLStatus BPL_AIT_GetSectionData (BplAitSectionData_t **ppAitInfo)
{
	HUINT32							ulAitCnt, ulAitNum;
	HUINT32							ulTotalSize = 0;
	HUINT32							ulSecSize;
	BplAitSectionData_t				*pstAitInfo = NULL;
	HERROR							 hErr;
	HUINT32 						 ulViewId;

	hErr = APK_MEDIA_PLAY_GetMainViewId(&ulViewId);

	*ppAitInfo = NULL;

	hErr = APK_MEDIA_PLAY_CountAitSection (ulViewId, &ulAitNum);
	if ((ERR_OK != hErr) || (0 == ulAitNum))
	{
		return BPL_STATUS_ERROR;
	}

	pstAitInfo = (BplAitSectionData_t *)HLIB_STD_MemCalloc (sizeof(BplAitSectionData_t));
	if (NULL == pstAitInfo)
	{
		goto ERROR;
	}
	HxSTD_MemSet(pstAitInfo,0,sizeof(BplAitSectionData_t));

	pstAitInfo->stSiTable.ulSecCnt = ulAitNum;
	for (ulAitCnt = 0; ulAitCnt < ulAitNum; ulAitCnt++)
	{
		ulSecSize = 0;

		hErr = APK_MEDIA_PLAY_GetAitSection (ulViewId, ulAitCnt, &(pstAitInfo->stSiTable.arSecData[ulAitCnt]), &ulSecSize);
		if ((ERR_OK != hErr) || (NULL == pstAitInfo->stSiTable.arSecData[ulAitCnt]) || (0 == ulSecSize))
		{
			goto ERROR;
		}

		ulTotalSize += ulSecSize;
	}

	*ppAitInfo = pstAitInfo;

	return BPL_STATUS_OK;

ERROR:
	if (NULL != pstAitInfo)					{ BPL_AIT_FreeSectionData (pstAitInfo); }

	return BPL_STATUS_ERROR;
}

BPLStatus BPL_AIT_FreeSectionData(BplAitSectionData_t *pAitInfo)
{
	if (NULL != pAitInfo)
	{
		{
			int i,cnt = pAitInfo->stSiTable.ulSecCnt;

			for (i=0; i < cnt; i++)
			{
				if (NULL != pAitInfo->stSiTable.arSecData[i])
				{
					APK_MEDIA_PLAY_FreeAitSection(pAitInfo->stSiTable.arSecData[i]);
					pAitInfo->stSiTable.arSecData[i] = NULL;
				}
			}
		}
		HLIB_STD_MemFree(pAitInfo);
	}

	return BPL_STATUS_OK;
}

BPLStatus BPL_AIT_GetServiceInfo( BplAitService_t *pService )
{
	DxService_t 					*pstService = NULL;
	HUINT32							 nSvcUID = 0;
	HERROR							 hErr;
	HUINT32 						 ulViewId;

	hErr =  APK_MEDIA_PLAY_GetMainViewId(&ulViewId);
	hErr |= APK_MEDIA_PLAY_GetRequestId (ulViewId, eOxMP_MEDIATYPE_LIVE, &nSvcUID);

	if( ERR_OK != hErr )
		return BPL_STATUS_ERROR;

	pstService = APK_META_SVC_GetService(nSvcUID);
	if (NULL == pstService)
	{
		return BPL_STATUS_ERROR;
	}

	pService->usOnID = pstService->onid;
	pService->usTSID = pstService->tsid;
	pService->usSID	= pstService->svcid;

	APK_META_SVC_Delete(pstService);

	return BPL_STATUS_OK;
}

BPLStatus BPL_AIT_GetHbbTVEnabled( int *pnEnabled )
{
	HINT32	enabled = 0;

	if(APK_DB_GetInt(USERDB_HBBTV_ENABLE, &enabled) != ERR_OK )
	{
		return BPL_STATUS_ERROR;
	}

	*pnEnabled = enabled;

	return BPL_STATUS_OK;
}

/* end of file */

