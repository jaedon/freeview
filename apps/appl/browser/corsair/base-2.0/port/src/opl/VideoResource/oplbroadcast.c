/**************************************************************************************/
/**
 * @file bplbroadcast.c
 *
 * Video Broadcast
 *
 **************************************************************************************
 **/

#include "bpldb.h"

#include "oplbroadcast.h"
#include "oplfavourite.h"
#include "oplmetadatasearch.h"
#include "oplavcomponent.h"
#include "oplvideoutil.h"

#include "oplscheduler.h"
#include "oplscheduledprogramme.h"
#include "oplmetadatasearch.h"
#include "opljlabslocalsystem.h"
#include "oplchannel.h"
#include "opltuner.h"
#include "oplrecorditem.h"

#if defined( CONFIG_OP_JAPAN)
#include "oplparser_japan.h"
#endif

#include "oapi.h"
#include <hlib.h>
#include <hapi.h>
#include <thapi.h>
#include <stdlib.h>
#include <string.h>
#include <vkernel.h>
#include "apk.h"

/******************************************************************
	Define
******************************************************************/


#define	RETURN_IF_VOID(expr)	do{if(expr){HxLOG_Debug("[%s:%d] %s failed!!\n", __FUNCTION__, __HxLINE__, #expr);return;}}while(0)
#define	RETURN_IF(expr, err)	do{if(expr){HxLOG_Debug("[%s:%d] %s failed!!\n", __FUNCTION__, __HxLINE__, #expr);return (err);}}while(0)

#define BPLBROADCAST_WINDOW_WIDTH			(1280)
#define BPLBROADCAST_WINDOW_HEIGHT			(720)
#define BPLBROADCAST_MAX_SOURCE_LEN			(1024)
#define BPLBROADCAST_MAX_MIMETYPE_LEN		(64)
#define MAX_SPEED_NUM						(24)

#if defined(CONFIG_OP_UK_DTT)
#define BPLBROADCAST_SAFE_TSR_START_OFFSET 	(20)
#define BPLBROADCAST_TSR_2HOURS				(7200)
#endif

#if !defined(CONFIG_OP_JAPAN)
#define BPLBROADCAST_AUDIO_DUAL_MONO		1
#endif

#define	IsHardOfHearingSubtitle(type)			(((type) == 0x05) || (0x20 <= (type) && (type) <= 0x24))
/******************************************************************
	Typedef
******************************************************************/
typedef enum
{
	eOplBroadcastMctrlMode_None				= 0,

	eOplBroadcastMctrlMode_Live,
	eOplBroadcastMctrlMode_TsrPb,

	eOplBroadcastMctrlMode_EndOfCase
} xOplBroadcastMctrlMode_e;

typedef enum
{
	eOplBroadcastBufType_None				= 0,

	eOplBroadcastBufType_Tsr,
	eOplBroadcastBufType_Usr,

	eOplBroadcastBufType_EndOfCase
} xOplBroadcastBufferingType_e;

typedef struct	{
	/*OIPF규격은 string이긴 하지만 contentID가 정확히 뭘 의미하는지 몰라 임시로 HUINT32로 선언. 필요한 경우 문자열로 만들어 쓴다.*/
	HUINT32	contentId;
	HBOOL 	bBlocked;
	HUINT32	ratingValue;
	HUINT32	caSystemId;
#if defined(CONFIG_OP_SES)
	HBOOL	bBlockedByCas;
#endif
}OplBroadcastParentalRatingStatus_t;

typedef struct	{
	HUINT32 	errorState;
	HCHAR*	contentId;
	HCHAR*	rightsIssuerUrl;
	HCHAR*	drmSystemId;
}OplBroadcastDrmRightsStatus_t;

typedef	struct {

	HUINT32	ulViewId;
	HUINT32	ulRecSessionId;

	OplBroadcastPlayState_e			 ePlayState;
	eOplRecorderState				 eRecState;

	HINT32							 nSvcIndex;
	BPL_BOOL						 bFullScreen;

	OPL_HANDLE						 hVbcastHandle;
	OPL_HANDLE						 hQueryHandle;
	OPL_ProgrammeHandle				 ahPF[12];
	HINT32							 nPFLen;
	HUINT32							ulSessionHandle;
	OplBroadcastSessionState_e			eSessionState;

	// Recording 관련
	HBOOL							 bRecordNotifyEnabled;

	OxMediaPlay_Event_e			 	 eLastLiveEvent;
	OxMediaPlay_LiveViewState_e	 	eViewState, eLastViewState;
	OxMediaPlay_LiveLockState_e	 	eLockState, eLastLockState;

	OplBroadcastListener_t			liveEventListener;
	OplBroadcastListener_t			tsrEventListener;
	OplBroadcastListener_t			casEventListener;
	OplBroadcastListener_t			recEventListener;
	OplBroadcastListener_t			mailEventListener;
	OplBroadcastListener_t			bmlEventListener;
	OplBroadcastListener_t			mhegEventListener;

	OplRecorder_t			*recorder;

	// mail
	OPL_MailHandle					hMail;

	// event realy
	HINT32							eventRelaySvcUid;

	// ews - 긴급 경보 방송
	HINT32							ewsSvcUid;
	HINT32							ewsCurSvcUid;

	eOplPlaybackError				 ePlaybackError;

	// bml
	char 							*pcBMLName;
	char 							*pcBMLParam;

	// pinctrl
	OplBroadcastParentalRatingStatus_t		stParentalRatingStatus;

	// drm
	OplBroadcastDrmRightsStatus_t		stDrmRightsStatus;

	// storage right
	OplBroadcastStorageRight_e			eStorageRight;

	// store the last window size
	HINT32							nLastWindowPositionX;
	HINT32							nLastWindowPositionY;
	HINT32							nLastWindowPositionWidth;
	HINT32							nLastWindowPositionHeight;

	// play position
	HINT32				nTsrPlaytime;
	HINT32				nTsrPlaytimeTick;

	HINT32				nTsrStarttime;
	HINT32				nTsrStarttimeTick;
	/* this is added to memory TSR triggering */
	HBOOL               bIsTsrPlaying;

	Handle_t			hCasMsgSession;

	// RCT
	HUINT32 			nLinkCount;
	HUINT32 			nImgSize;

	//NCD
	HUINT32			nStartTimeofChange;
	HUINT32			nASODuration;
	HUINT8			*pszNetworkMsg;

	//MHEG
	HUINT8				*pszPincodeMsg;
	HINT32				nMhegSvcUid;
	OxMediaPlay_TuningMode_e				eMhegTuneMode;
	OPL_Channel_t 		hMhegChannel;
	HBOOL				bInterruptible;
	HBOOL				bMhegIcsStatus;

	//NETWORK URI
	HUINT8			*pszNetworkUri;

	// COMPONENT
	OxMediaPlay_Component_e	eChangedComponent;
} xOplBroadcast_t;

/******************************************************************
	Static variables
******************************************************************/
static HxList_t 			*s_pstBcastObjList = NULL;

#if defined(CONFIG_OP_UK_DTT)
/* TSR Buffer가 꽉찬 상황에서 MW에서는 자동으로 Start를 하고, BOF를 받은 OPL은 Pause를 하여
   Pause -> Start -> Pause -> Start를 무한 반복하는 문제 수정.
   Release를 앞두고 급하게 고치느라 static 변수로 처리. */
static HUINT32		ulTsrMaxOffset = 0;
#endif
/**
 * Workaround...
 * xOplBroadcast_t.nSvcIndex는 multiple 인스턴스로 관리되기 때문에, 서로 다른 index를 가리킴..
 * 일단 아래와 같이 처리하고, 난중에 지대로 수정해 주셔요.
 */
static HCHAR				 s_szOplBroadcast_CurrSvcCcid[32];
static HUINT32				 s_ulOplBroadcast_LastMasterSvcUid = 0, s_ulOplBroadcast_LastSuppleSvcUid = 0;
static void				*s_chHandleOfSetchannel = NULL;


/******************************************************************
	Static function prototypes
******************************************************************/
static void 		opl_broadcast_notifyRecFunc ( HUINT32 ulSvcUid, HUINT32 ulSessionId, OxMediaRec_Event_e eEvent, OxMediaRec_NotifierData_t *pstNotifyData, void *pvUserData );

static void 		opl_broadcast_notifyCasMessageFunc(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);
static BPLStatus	opl_broadcast_enableRecNotifier (xOplBroadcast_t *hBcast, DxService_t *pstService);
static BPLStatus	opl_broadcast_disableRecNotifier (xOplBroadcast_t *hBcast);
static void 		opl_broadcast_notifyDrmFunc(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);
static void 		opl_broadcast_notifyBsAutoMsgFunc(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);
static void 		opl_broadcast_notifyCaLinkMsgFunc(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);
static BPLStatus 	opl_broadcast_memorizeLastChannelByNetwork (OPL_Channel_t hChannel);
static BPLStatus	opl_broadcast_getParentalRatingStatusFromJson(OplBroadcastParentalRatingStatus_t *pstStatus, HxJSON_t jsArgOthers);
static BPLStatus	opl_broadcast_getDrmRightsStatusFromJson(OplBroadcastDrmRightsStatus_t *pstStatus, HUINT32 error_status);
STATIC BPLStatus	opl_broadcast_getErrorFromNotifier (xOplBroadcast_t *hBcast, OxMediaPlay_Event_e eCurrEvent, HBOOL *pbStateChanged);
STATIC eOplPlaybackError opl_broadcast_getNextErrorByStates (xOplBroadcast_t *hBcast, OxMediaPlay_Event_e eCurrEvent);
static void 		opl_broadcast_notifyMailMessageRcv(int argc, void **argv);
static void 		opl_broadcast_notifyMailMessageAsync(APK_HANDLE handle, HINT32 event, const HINT32 *args, void *userdata);
static void 		opl_broadcast_notifyMailMessage(APK_HANDLE handle, HINT32 event, const HINT32 *args, void *userdata);

static HERROR opl_broadcast_CheckChannelChangable (OplBroadcast_t *hBroadcast, OPL_Channel_t channel, HBOOL *pbChangable);
static OPL_Channel_t	opl_broadcast_getChangbleChannel_inTheRecording(OplBroadcast_t *hBcast);
static BPLStatus opl_broadcast_getCurrentProgramme (xOplBroadcast_t *hBroadcast, OPL_ProgrammeHandle *programme);

/******************************************************************
	Static functions
******************************************************************/
inline static xOplBroadcast_t *xbroadcast(OplBroadcast_t *hBroadcast)
{
	return (xOplBroadcast_t *)hBroadcast;
}

static void opl_broadcast_initialize(void)
{
	HINT32					 lastSvcUid;
	HCHAR					 szCcid[32] = { 0, };
	OPL_ChannelManager_t	 chManager;
	OPL_Channel_t			 hChannel = NULL;
	HERROR					 hErr;

	HxSTD_MemSet (s_szOplBroadcast_CurrSvcCcid, 0, 32);

	chManager = OPL_Channel_GetManager();
	if (NULL == chManager)
	{
		return;
	}

	//	OCTO가 먼가를 Play하고 있으면 OPL단의 Last Channel을 변경한다.
	APK_MEDIA_PLAY_GetRequestId( 0 /*AV0 default*/, eOxMP_MEDIATYPE_LIVE, (HUINT32*)&lastSvcUid);
	if (lastSvcUid >= 0)
	{
		OPL_Channel_t	tChannel;
		tChannel = OPL_Channel_FindChannelByUID(OPL_Channel_GetManager(), (int)lastSvcUid);
		if (tChannel)
		{
			OPL_Channel_GetCCID(tChannel, szCcid, 32);
			APK_DB_SetStr(USERDB_SVC_LAST_CHANNEL, szCcid);
			opl_broadcast_memorizeLastChannelByNetwork(tChannel);
		}
	}

	hErr = APK_DB_GetStr((const HCHAR *)USERDB_SVC_LAST_CHANNEL, szCcid, 32);
	if (ERR_OK == hErr)
	{
		hChannel = OPL_Channel_FindChannel (chManager, szCcid);
	}

	if (NULL == hChannel)
	{
		hChannel = OPL_Channel_FindChannelAt (chManager, 0);
		if (NULL == hChannel)
		{
			return;
		}
	}

	OPL_Channel_GetCCID (hChannel, s_szOplBroadcast_CurrSvcCcid, 32);
	s_ulOplBroadcast_LastMasterSvcUid = s_ulOplBroadcast_LastSuppleSvcUid = OPL_Channel_GetUniqueID (hChannel);
}

static HERROR opl_broadcast_startLive(HUINT32 ulViewId, HUINT32 ulUID, HUINT32 *pulSessionId)
{
	HERROR hErr	=	ERR_FAIL;
	OPL_Channel_t	pChannel	=	NULL;

	if( NULL == pulSessionId )
		return ERR_FAIL;

	pChannel = OPL_Channel_FindChannelByUID(NULL, ulUID);

	if( NULL != pChannel )
	{

		OxMediaPlay_StartInfo_t 	 unStart = {0,};

		unStart.stLive.eType			= eOxMP_MEDIATYPE_LIVE;
		unStart.stLive.eLiveType		= eOxMP_LIVETYPE_MAIN;
		unStart.stLive.ulMasterSvcUid	= ulUID;
		unStart.stLive.ulSuppleSvcUid	= ulUID;
		unStart.stLive.eSuppSvcType		= eOxMP_SUPPLESVCTYPE_Normal;
		unStart.stLive.ulMajorCHNum		= OPL_Channel_GetMajorChannel(pChannel);
		unStart.stLive.eTuneMode		= 0x00;

		hErr = APK_MEDIA_PLAY_Start (ulViewId, &unStart, pulSessionId);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("[%s:%d] APK_MEDIA_PLAY_Start err: ViewId=%d\n", __FUNCTION__, __LINE__, ulViewId);
		}
	}
	else
	{
		HxLOG_Error ("[%s:%d] OPL_Channel_FindChannelByUID err: ViewId=%d\n", __FUNCTION__, __LINE__, ulViewId);
	}

	return hErr;
}

static HERROR opl_broadcast_startLiveFcc(HUINT32 ulViewId, HUINT32 ulUID, HUINT32 *pulSessionId)
{
	HERROR hErr	=	ERR_FAIL;
	OPL_Channel_t	pChannel	=	NULL;

	if( NULL == pulSessionId )
	{
		return ERR_FAIL;
	}

	pChannel = OPL_Channel_FindChannelByUID(NULL, ulUID);

	if( NULL != pChannel )
	{

		OxMediaPlay_StartInfo_t 	 unStart = {0,};

		unStart.stLive.eType			= eOxMP_MEDIATYPE_LIVE;
		unStart.stLive.eLiveType		= eOxMP_LIVETYPE_FCC;
		unStart.stLive.eTuneMode		= 0x00;
		unStart.stLive.ulMasterSvcUid	= ulUID;
		unStart.stLive.ulSuppleSvcUid	= ulUID;
		unStart.stLive.eSuppSvcType		= eOxMP_SUPPLESVCTYPE_Normal;
		unStart.stLive.ulMajorCHNum		= OPL_Channel_GetMajorChannel(pChannel);

		hErr = APK_MEDIA_PLAY_Start (ulViewId, &unStart, pulSessionId);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("[%s:%d] APK_MEDIA_PLAY_Start err: ViewId=%d\n", __FUNCTION__, __LINE__, ulViewId);
		}
	}
	else
	{
		HxLOG_Error ("[%s:%d] OPL_Channel_FindChannelByUID err: ViewId=%d\n", __FUNCTION__, __LINE__, ulViewId);
	}

	return hErr;
}

static HERROR opl_broadcast_startLiveByMheg(HUINT32 ulViewId, HUINT32 ulUID, HUINT32 TuneMode, HUINT32 *pulSessionId)
{
	HERROR hErr	=	ERR_FAIL;
	OPL_Channel_t	pChannel	=	NULL;

	if( NULL == pulSessionId )
		return ERR_FAIL;


	pChannel = OPL_Channel_FindChannelByUID(NULL, ulUID);
	if( NULL != pChannel )
	{

		OxMediaPlay_StartInfo_t 	 unStart = {0,};

		unStart.stLive.eType			= eOxMP_MEDIATYPE_LIVE;
		unStart.stLive.eLiveType		= eOxMP_LIVETYPE_MAIN;
		unStart.stLive.ulMasterSvcUid	= ulUID;
		unStart.stLive.ulSuppleSvcUid	= ulUID;
		unStart.stLive.eSuppSvcType		= eOxMP_SUPPLESVCTYPE_Normal;
		unStart.stLive.ulMajorCHNum		= OPL_Channel_GetMajorChannel(pChannel);
		unStart.stLive.eTuneMode		= TuneMode;

		hErr = APK_MEDIA_PLAY_Start (ulViewId, &unStart, pulSessionId);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("[%s:%d] APK_MEDIA_PLAY_Start err: ViewId=%d\n", __FUNCTION__, __LINE__, ulViewId);
		}
	}
	else
	{
		HxLOG_Error ("[%s:%d] OPL_Channel_FindChannelByUID err: ViewId=%d\n", __FUNCTION__, __LINE__, ulViewId);
	}

	return hErr;
}

static HUINT32 opl_broadcast_getSvcUidByCurrOffset (HUINT32 ulViewId, HINT32 nOffset)
{
#if 1
{
	HINT32					 nSvcIndex = 0, nChanCount = 0;
	OPL_Channel_t			 hChannel;
	OPL_ChannelManager_t	 chManager;

	chManager = OPL_Channel_GetManager();
	if (NULL == chManager)
	{
		return 0;
	}

	hChannel = OPL_Channel_FindChannel (chManager, s_szOplBroadcast_CurrSvcCcid);
	if (NULL == hChannel)
	{
		return 0;
	}

	nSvcIndex = OPL_Channel_IndexOf (chManager, hChannel);
	if (nSvcIndex < 0)
	{
		return 0;
	}

	nChanCount = OPL_Channel_Count(chManager);
	if (nChanCount < 0)
	{
		return 0;
	}

	nSvcIndex += nOffset;

	while (nSvcIndex < 0)
	{
		nSvcIndex += nChanCount;
	}

	while (nSvcIndex >= nChanCount)
	{
		nSvcIndex -= nChanCount;
	}

	hChannel = OPL_Channel_FindChannelAt (chManager, nSvcIndex);
	if (NULL == hChannel)
	{
		return 0;
	}
	return (HUINT32)OPL_Channel_GetUniqueID(hChannel);
}
#else
{
	HUINT32					 ulSvcUid = 0;
	OPL_ChannelManager_t	 chManager;
	OPL_FAVLIST_t			 favList;

	chManager = OPL_Channel_GetManager();
	if (NULL == chManager)
	{
		return 0;
	}

	ulSvcUid = OPL_Channel_FindChannel (chManager, s_szOplBroadcast_CurrSvcCcid);
	if (0 == ulSvcUid)
	{
		return 0;
	}

	// 현재 activate된 favourite list(Channel Group)을 기준으로 재퓡
	favList = OPL_FAVMGR_GetCurrentList();
	if (NULL == favList)
	{
		return 0;
	}
	return (HUINT32)OPL_FAVLIST_FindChannel(favList, (int)ulSvcUid, (int)nOffset);
}
#endif
}

static HUINT32 opl_broadcast_getAvailableSvcUidByCurrOffset (HUINT32 ulViewId, HINT32 nOffset)
{
#if defined(CONFIG_OP_JAPAN)
	HINT32					 nSvcIndex = 0, nChanCount = 0, nCurrIndex;
	OPL_Channel_t			 hChannel, hCurrentChannel;
	OPL_ChannelManager_t	 chManager;
	HBOOL					bPossible = FALSE;
	HINT32					tuningRange = eTuningRange_All;
	HBOOL					bSeamless = TRUE;
	chManager = OPL_Channel_GetManager();
	if (NULL == chManager)
	{
		return 0;
	}

	hChannel = OPL_Channel_FindChannel (chManager, s_szOplBroadcast_CurrSvcCcid);
	if (NULL == hChannel)
	{
		return 0;
	}

	nSvcIndex = OPL_Channel_IndexOf (chManager, hChannel);
	if (nSvcIndex < 0)
	{
		return 0;
	}

	nCurrIndex = nSvcIndex;

	nChanCount = OPL_Channel_Count(chManager);
	if (nChanCount < 0)
	{
		return 0;
	}
	hCurrentChannel = hChannel;
	APK_DB_GetInt(USERDB_JLABS_TUNING_RANGE, &tuningRange);
	OPL_JlabsLocalSystem_GetSeamlessEnable(&bSeamless);

	do
	{
		nSvcIndex += nOffset;
		while (nSvcIndex < 0)
		{
			nSvcIndex += nChanCount;
		}

		while (nSvcIndex >= nChanCount)
		{
			nSvcIndex -= nChanCount;
		}

		hChannel = OPL_Channel_FindChannelAt (chManager, nSvcIndex);
		if (NULL == hChannel)
		{
			return 0;
		}
		if(bSeamless == FALSE)
		{
			if(OPL_Channel_IsSameNetwork(hCurrentChannel, hChannel) == FALSE)
			{
				bPossible = FALSE;
				continue;
			}
		}

		if (eTuningRange_All >= tuningRange && eTuningRange_TV <=tuningRange)
		{
			bPossible = OPL_Channel_IsPossibleToTuneByUpDown(hChannel, tuningRange);
		}
		else
		{
			bPossible = OPL_Channel_IsPossibleToTuneByUpDown(hChannel, eTuningRange_All);
		}
	}while((bPossible == FALSE) && (nCurrIndex != nSvcIndex));

	return (HUINT32)OPL_Channel_GetUniqueID(hChannel);
#else
	return 0;
#endif
}

static BPLStatus opl_broadcast_memorizeLastChannelByNetwork (OPL_Channel_t hChannel)
{
	HCHAR			szCcid[32] = { 0, };
	OPL_NetworkType_e	netType = eOPL_NETWORK_START;
	if(hChannel == NULL)
		return BPL_STATUS_ERROR;
	netType = OPL_Channel_GetNetTypeByOnID(OPL_Channel_GetOrgNetworkID(hChannel));
	OPL_Channel_GetCCID (hChannel, szCcid, 32);
	if(szCcid[0] != '\0')
	{
		switch(netType)
		{
#if defined(CONFIG_OP_JAPAN)
			case eOPL_DTT_NETWORK:
				APK_DB_SetStr(USERDB_JLABS_DTT_LAST_CHANNEL, szCcid);
				break;
			case eOPL_BS_NETWORK:
				APK_DB_SetStr(USERDB_JLABS_BS_LAST_CHANNEL, szCcid);
				break;
			case eOPL_CATV_NETWORK:
				APK_DB_SetStr(USERDB_JLABS_CATV_LAST_CHANNEL, szCcid);
				break;
			case eOPL_CS1_NETWORK:
				APK_DB_SetStr(USERDB_JLABS_CS1_LAST_CHANNEL, szCcid);
				break;
			case eOPL_CS2_NETWORK:
				APK_DB_SetStr(USERDB_JLABS_CS2_LAST_CHANNEL, szCcid);
				break;
			case eOPL_JCHITS_NETWORK:
				APK_DB_SetStr(USERDB_JLABS_JCHITS_LAST_CHANNEL, szCcid);
				break;
#endif
			default:
				break;
		}
	}
	return BPL_STATUS_OK;
}

static BPLStatus opl_broadcast_memorizeCurrentChannel (HUINT32 ulSvcUid)
{
	HCHAR					 szCcid[32] = { 0, };
	OPL_Channel_t			 hChannel;

	hChannel = OPL_Channel_FindChannelByUID (OPL_Channel_GetManager(), (int)ulSvcUid);
	if (NULL == hChannel)					{ return BPL_STATUS_ERROR; }

	OPL_Channel_GetCCID (hChannel, szCcid, 32);

	if ((HxSTD_StrNCmp (s_szOplBroadcast_CurrSvcCcid, szCcid, 32) != 0) &&
		('\0' != szCcid))
	{
		HxSTD_MemCopy(s_szOplBroadcast_CurrSvcCcid, szCcid, 32);
		APK_DB_SetStr(USERDB_SVC_LAST_CHANNEL, s_szOplBroadcast_CurrSvcCcid);
		opl_broadcast_memorizeLastChannelByNetwork(hChannel);
		return BPL_STATUS_OK;
	}

	return BPL_STATUS_ERROR;
}

static void 		opl_broadcast_PfMonitor (OPL_HANDLE search, OPL_ProgrammeHandle *pf, HINT32 n, xOplBroadcast_t *self)
{
	HINT32	i, c, startindex =0;

	if (n > 0 && self->nPFLen == n)
	{
		for (i = 0 ; i < n ; i++)
		{
			if (!OPL_Programme_Equals(self->ahPF[i], pf[i]))
				break;
		}
		if (i == n)
		{
			HxLOG_Print("\t%s duplicated!!\n\n", __FUNCTION__);
#if !defined( CONFIG_OP_JAPAN )
			return;
#endif
		}
	}

	for (i = 0 ; i < self->nPFLen ; i++)
	{
		OPL_Programme_Delete(self->ahPF[i]);
	}
	self->nPFLen = 0;

#if defined( CONFIG_OP_JAPAN )
{
	/* 일본향 미정도 걸러내야하고 현재 이벤트가 무조건 pf[0]에 위치 시키기 위해서 */
	HUINT32 		ulStartTime =0, ulDuration =0, ulCurTime =0, curIndex =0, tempIndex =0, ulTempStartTime =0, ulCurStartTime =0;
	HBOOL		bIsUndefined = FALSE;
	HINT32		nMasterSvcUid = 0, nPfSvcUid =0;
	OPL_Channel_t	pstChannel = NULL;

	APK_MEDIA_PLAY_GetRequestId (self->ulViewId, eOxMP_MEDIATYPE_LIVE, &nMasterSvcUid);

	ulCurTime = HLIB_STD_GetSystemTime();

	for (i = 0; i < n ; i++)
	{
		pstChannel = OPL_Programme_GetChannel( pf[i] );
		if( pstChannel )
			nPfSvcUid = OPL_Channel_GetUniqueID( pstChannel );

		// 같은 채널이 아니면 보낼 필요가 없다.- 그 사이에 변경된 것이다.
		if (nPfSvcUid != nMasterSvcUid)
		{
			HxLOG_Error ("Current Service is not same...!!!\n");
			return;
		}

		ulStartTime = OPL_Programme_GetStartTime( pf[i] );
		ulDuration = OPL_Programme_GetDuration( pf[i] );

		if( ulStartTime > ulCurTime ) /* 미래 이벤트는 건너 뛴다. */
			continue;

		if( (ulStartTime <= ulCurTime )&& (ulDuration == 0 || ulDuration == 0xffffff) )
		{/*마지막 미정이 현재 이벤트임 */
			bIsUndefined = TRUE;
			tempIndex = i;
			ulTempStartTime = ulStartTime;
			continue;
		}
		else if( (ulStartTime <= ulCurTime )&& ( (ulStartTime + ulDuration) > ulCurTime ) )
		{/* current event */
			curIndex = i;
			ulCurStartTime = ulStartTime;
		}
		else
		{ /* do nothing */ }
	}

	if( ( bIsUndefined == TRUE ) && ( ulTempStartTime >= ulCurStartTime ))
	{/* 마지막 미정인 것인 current event */
		curIndex = tempIndex;
	}

	startindex = curIndex;
	i =0;
}
#endif

	for (i = startindex, c = 0 ; c < 12 && i < n ; i++)
	{
		self->ahPF[c] = OPL_Programme_Clone(pf[i]);
		if (self->ahPF[c])
		{
			HxLOG_Print("\t%s() PF[%d] %s\n", __FUNCTION__, c, OPL_Programme_GetName(self->ahPF[c]));
			c++;
		}
	}
	self->nPFLen = c;

	if (search && self->liveEventListener)
	{
		self->liveEventListener(self->hVbcastHandle, OPLBROADCAST_EVENT_TYPE_PROGRAMMES_CHANGED);
	}
}

static BPLStatus	opl_broadcast_BindChannel (xOplBroadcast_t *self, int uid, unsigned int tuneInfo)
{
	HINT32			nMasterSvcUid, nSuppleSvcUid;
	OPL_Channel_t	channel = OPL_Channel_FindChannelByUID(NULL, uid);
	HERROR			hErr;
	NOT_USED_PARAM(nSuppleSvcUid);

	HxLOG_Assert(channel);
	if (NULL == channel)
	{
		HxLOG_Error ("channel (0x%08x)\n", channel);
		return BPL_STATUS_ERROR;
	}

	OxMediaPlay_PlayState_e eState;
	OxMediaPlay_MediaType_e eOapiType;

	APK_MEDIA_PLAY_GetPlayType( self->ulViewId, &eOapiType );

	switch ( eOapiType )
	{
		case eOxMP_MEDIATYPE_LIVE:
		{
			APK_MEDIA_PLAY_GetRequestId ( self->ulViewId, eOxMP_MEDIATYPE_LIVE, (HUINT32*)&nMasterSvcUid);
			// 같은 채널을 보고 있으면 명령을 내리지 않는다.
			if ( uid == nMasterSvcUid )
			{
				/*	this case means that stop() is called at the last time.
					but the Appkit status still has PLAYING.
				 	so we must have to wait until changing the status to STOP.
				 	to fix this issue. Mediactrl of Appkit shall be fixed
				 */

				APK_MEDIA_PLAY_GetPlayState( self->ulViewId, &eState );
				if ( ( eState == eOxMP_PLAYSTATE_STOPPING || eState == eOxMP_PLAYSTATE_PLAYING ) && self->ePlayState == OPLBROADCAST_PLAY_STATE_STOPPED )
				{
					int retry_cnt = 0;
					while( eState != eOxMP_PLAYSTATE_STOPPED && retry_cnt < 10 )
					{
						VK_TASK_Sleep(200);
						APK_MEDIA_PLAY_GetPlayState( self->ulViewId, &eState );
						retry_cnt++;
					}
				}
				else
				{
#if defined(CONFIG_SUPPORT_MHEGPLAYER_APP)
					if((eOxMP_TUNEMODE_FORCE_TUNE & tuneInfo) == 0)
					{
						HxLOG_Error ("Current Service is same...(0x%x) \n", eOxMP_TUNEMODE_FORCE_TUNE & tuneInfo);
						return BPL_STATUS_OK;
					}
					else
					{
						HxLOG_Error ("Current Service is same...But Forced Tune (0x%x) \n", eOxMP_TUNEMODE_FORCE_TUNE & tuneInfo);
					}
#else
					HxLOG_Error ("Current Service is same...\n");

#if !defined(CONFIG_PRISM_ZAPPING)
                	return BPL_STATUS_OK;
#endif

#endif
				}
			}
			// Channel Zapping 속도를 위해 Stop을 내리지 않는다.
			hErr = ERR_OK;

			break;
		}
		case eOxMP_MEDIATYPE_PVRPLAYBACK:
		case eOxMP_MEDIATYPE_TSRPLAYBACK:
		{
			hErr = APK_MEDIA_PLAY_Stop (self->ulViewId);
			break;
		}
		case eOxMP_MEDIATYPE_MEDIAPLAY:
		{
#if defined(CONFIG_OP_UK_DTT)
			(void)APK_MEDIA_PLAY_Stop (self->ulViewId);
			hErr = ERR_OK;
#else
			hErr = APK_MEDIA_PLAY_Stop (self->ulViewId);
#endif
			break;
		}
		default:
			hErr = ERR_OK;
			break;
	}

	if (ERR_OK != hErr)
	{
		HxLOG_Error ("Cannot stop the service : ulViewId=0x%08x, SVCUID=%d\n", self->ulViewId, uid);
		return BPL_STATUS_ERROR;
	}

	opl_broadcast_disableRecNotifier (self);

	if (tuneInfo > 0)
	{
		HUINT32  ulSessionId = 0;

		hErr = opl_broadcast_startLiveByMheg(self->ulViewId, ((DxService_t *)channel)->uid, tuneInfo, &ulSessionId);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("[%s:%d] opl_broadcast_startLive err: ViewId=%d\n", __FUNCTION__, __LINE__, self->ulViewId);
			return BPL_STATUS_ERROR;
		}
	}
	else
	{
		HUINT32  ulSessionId = 0;

		hErr = opl_broadcast_startLive(self->ulViewId, ((DxService_t *)channel)->uid, &ulSessionId);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("[%s:%d] opl_broadcast_startLive err: ViewId=%d\n", __FUNCTION__, __LINE__, self->ulViewId);
			return BPL_STATUS_ERROR;
		}
	}


#if !defined( CONFIG_OP_JAPAN )
	/* 일본향은 eit  changed 메시지 이후에 작업한다. */
	if (self->hQueryHandle)
	{
		while (self->nPFLen--)
		{
			OPL_Programme_Delete(self->ahPF[self->nPFLen]);
		}
		self->nPFLen = 0;

		OPL_METADATASEARCH_SearchPF(self->hQueryHandle, uid, (OPL_SearchPfMonitor_t)opl_broadcast_PfMonitor, (void *)self);
	}
#endif

	self->ePlayState = OPLBROADCAST_PLAY_STATE_CONNECTING;

	opl_broadcast_enableRecNotifier (self, (DxService_t *)channel);

#if defined(CONFIG_OP_UK_DTT)
	ulTsrMaxOffset = 0;
#endif

	return BPL_STATUS_OK;
}


static HERROR opl_broadcast_StartRec(xOplBroadcast_t *hBcast, OPL_Channel_t channel, HINT32 nStartOffset, HUINT32 ulDuration, OxMediaRec_TargetStorage_e eStorageType, HUINT8 *szStorageId, HUINT32 *pulSessionId, char *pszRecordID)
{
	HERROR hErr;
	HUINT32 ulCurrTime = 0, ulStartTime = 0, ulRecTime = 0, uluid;
	OxMediaRec_StartInfo_t stStart;
	HCHAR					szName[256];
	HINT32					eventId = 0;

	HxLOG_Trace();

	if( (NULL == channel) || (NULL == pulSessionId) )
		return ERR_FAIL;

	HxSTD_MemSet( &stStart, 0, sizeof(OxMediaRec_StartInfo_t) );
	uluid = OPL_Channel_GetUniqueID(channel);

	stStart.ulMasterSvcUid	= uluid;
	stStart.ulSuppleSvcUid	= uluid;
	stStart.eSuppSvcType	= eOxMP_SUPPLESVCTYPE_Normal;
	stStart.nStartOffset		= nStartOffset;
	stStart.eStorageType	= eStorageType;
	ulCurrTime 			= HLIB_STD_GetSystemTime();

	/* Setup starttime*/
	if(nStartOffset<=0)
	{
		// Instant Rec ...
		(void)HLIB_DATETIME_ConvertUnixTimeToDateTime(ulCurrTime, &(stStart.stStartTime));
		ulStartTime = ulCurrTime;
	}
	else
	{
		// Delayed Rec ...
		OPL_Broadcast_GetMaxOffset((OplBroadcast_t *)hBcast, &ulRecTime);
		ulStartTime			= ulCurrTime - ulRecTime + nStartOffset;
		HxLOG_Debug("[%s:%d] ulRecTime(%d), nStartOffset(%d), ulStartTime(%d)\n", __FUNCTION__, __LINE__, ulRecTime, nStartOffset, ulStartTime);
		(void)HLIB_DATETIME_ConvertUnixTimeToDateTime(ulStartTime, &(stStart.stStartTime));
	}

	HxSTD_MemSet(stStart.szStorageId, 0, sizeof(stStart.szStorageId));

	if( NULL != szStorageId )
		HLIB_STD_StrNCpySafe(stStart.szStorageId, (HCHAR*)szStorageId, sizeof(stStart.szStorageId));

	stStart.ulDuration        =    ulDuration;

#if 1
	HxSTD_MemSet(szName, 0, 128);
	hErr = APK_META_QUERY_FindEventName(uluid, ulStartTime, ulStartTime + ulDuration, szName, eventId);
	if(hErr == ERR_OK)
	{
		if(HxSTD_StrLen(szName)>0)
		{
			HxLOG_Error ("[%s:%d] APK_META_QUERY_GetEventNameByStartTime (Event Name: %s) \n", __FUNCTION__, __LINE__, szName);
			HLIB_STD_StrNCpySafe(stStart.szName, szName, sizeof(stStart.szName));
			stStart.nEvtId = eventId;
		}
		else
		{
			HxLOG_Error ("[%s:%d] APK_META_QUERY_GetEventNameByStartTime Error (Svcname: %s) \n", __FUNCTION__, __LINE__, OPL_Channel_GetName(channel));
			HLIB_STD_StrNCpySafe(stStart.szName, OPL_Channel_GetName(channel), sizeof(stStart.szName));
		}
	}
	else
	{
		HxLOG_Error ("[%s:%d] APK_META_QUERY_GetEventNameByStartTime Error (Svcname: %s) \n", __FUNCTION__, __LINE__, OPL_Channel_GetName(channel));
		HLIB_STD_StrNCpySafe(stStart.szName, OPL_Channel_GetName(channel), sizeof(stStart.szName));
	}

#else
	{
		const char *pszName = NULL;
		OPL_ProgrammeHandle programme = NULL;

	if( BPL_STATUS_OK == opl_broadcast_getCurrentProgramme( hBcast, &programme) )
	{
		if( NULL != programme )
		{
			pszName = OPL_Programme_GetName( programme );
			if (NULL != pszName)
			HLIB_STD_StrNCpySafe(stStart.szName, pszName, sizeof(stStart.szName));

			stStart.nEvtId = OPL_Programme_GetEventId( programme );
		}
	}
	else
	{
		HxLOG_Error ("[%s:%d] opl_broadcast_getCurrentProgramme err: \n", __FUNCTION__, __LINE__);
		HLIB_STD_StrNCpySafe(stStart.szName, pstService->name, sizeof(stStart.szName));
	}

		if( NULL != programme )
			OPL_Programme_Delete (programme);

    }
#endif


#if defined(CONFIG_PROD_IR4000HD)
	/*
	 *	Note : IR4000HD은 Delayed Recoding 을 지원하지 않는다.
 	 */
	stStart.nStartOffset	=	-1;
#endif

	hErr = APK_MEDIA_RECORD_Start ( &stStart, &(hBcast->ulRecSessionId), pszRecordID);
	if( ERR_OK != hErr )
	{
		HxLOG_Error ("[%s:%d] APK_MEDIA_RECORD_Start err: \n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	if (NULL != pulSessionId)
	{
		*pulSessionId = hBcast->ulRecSessionId;
	}

    return ERR_OK;
}



static void opl_broadcast_notifyMailMessage(APK_HANDLE handle, HINT32 event, const HINT32 *args, void *userdata)
{
	xOplBroadcast_t 	*hBcast 	= NULL;
	HxList_t			*pstListItem;
	DxMail_t 			*pstSrcMail = (DxMail_t *)args, *pstMail = NULL;

	/* for warning */
	handle = handle;
	event = event;
	userdata = userdata;

	if( pstSrcMail == NULL )
	{
		HxLOG_Error ("[%s,%d] add mail is null\n", __FUNCTION__, __LINE__);
		return;
	}

	// 각 Broadcast Object에 돌려야 하는 것들
	for (pstListItem = s_pstBcastObjList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		hBcast = (xOplBroadcast_t *)HLIB_LIST_Data (pstListItem);
		if (NULL != hBcast)
		{
			pstMail = NULL;
			pstMail = (DxMail_t *)APK_META_MAIL_Clone(pstSrcMail);
			/* 여기서 복사된 메일은 web에서 garbage collection이 불리면서 해제 된다. */

			if (NULL == pstMail)
			{
				HxLOG_Error ("[%s,%d] pstMail (0x%08x)\n", __FUNCTION__, __LINE__, pstMail);
			}
			else
			{
				HxLOG_Print("[%s,%d] handle : %d \n", __FUNCTION__ , __LINE__ , pstMail);
				HxLOG_Print("[%s,%d] id : %d \n", __FUNCTION__ , __LINE__, pstMail->ulId);
				HxLOG_Print("[%s,%d] type : %d \n", __FUNCTION__ , __LINE__, (HINT32)pstMail->eMailType);

				hBcast->hMail = (OPL_MailHandle)pstMail;

				if( hBcast->hVbcastHandle != NULL && hBcast->mailEventListener != NULL )
				{
					hBcast->mailEventListener(hBcast->hVbcastHandle, OPLBROADCAST_EVENT_TYPE_MAIL_NEW_MEESAGE_EVENT);
				}
			}
		}
	}

	if( pstSrcMail )
	{
		APK_META_MAIL_Delete( pstSrcMail );
	}
}

static void opl_broadcast_bmlVod(void *userdata, HCHAR	* pBMLName, HCHAR * pBMLParam)
{
	xOplBroadcast_t *self 	= (xOplBroadcast_t *)userdata;

	HxLOG_Assert(self);
	HxLOG_Assert(pBMLName);
	HxLOG_Assert(pBMLParam);

	self->pcBMLName = pBMLName;
	self->pcBMLParam = pBMLParam;

	if(( self->hVbcastHandle != NULL ) && (self->bmlEventListener != NULL))
	{
		self->bmlEventListener(self->hVbcastHandle, OPLBROADCAST_EVENT_TYPE_BML_VOD);
	}
}


// Last Service : 최후에 Tuning 한 Service
static BPLStatus opl_broadcast_getLastService (HUINT32 ulViewId, DxService_t *pstService)
{
	HUINT32			 ulSvcUid;

	HxSTD_MemSet (pstService, 0, sizeof(DxService_t));

	ulSvcUid = opl_broadcast_getSvcUidByCurrOffset (ulViewId, 0);
	if (0 != ulSvcUid)
	{
		OPL_Channel_t hChannel = OPL_Channel_FindChannelByUID(OPL_Channel_GetManager(), (int)ulSvcUid);

		pstService->uid = ulSvcUid;
		if (NULL != hChannel)
		{
			pstService->onid  = OPL_Channel_GetOrgNetworkID (hChannel);
			pstService->tsid  = OPL_Channel_GetTransponderID (hChannel);
			pstService->svcid = OPL_Channel_GetServiceID (hChannel);
		}

		return BPL_STATUS_OK;
	}

	return BPL_STATUS_ERROR;
}

// Current Service : 현재 Tuning 된 Service
static BPLStatus opl_broadcast_getCurrentMasterService (HUINT32	ulViewId, DxService_t *pstService)
{
// queit tune 관련 코드를 위한 임시 block.
	HERROR  hErr;
	HINT32	nSvcUid = -1;

	hErr = APK_MEDIA_PLAY_GetRequestId (ulViewId, eOxMP_MEDIATYPE_LIVE, (HUINT32*)&nSvcUid);

	if ( (ERR_OK == hErr) && (nSvcUid > 0) )
	{
		OPL_Channel_t hChannel = OPL_Channel_FindChannelByUID (OPL_Channel_GetManager(), (int)nSvcUid);

		pstService->uid = nSvcUid;
		if (NULL != hChannel)
		{
			pstService->onid  = OPL_Channel_GetOrgNetworkID (hChannel);
			pstService->tsid  = OPL_Channel_GetTransponderID (hChannel);
			pstService->svcid = OPL_Channel_GetServiceID (hChannel);
		}

		return BPL_STATUS_OK;
	}

	return BPL_STATUS_ERROR;
}


static BPLStatus opl_broadcast_getCurrentSupplementaryService (HUINT32 ulViewId, DxService_t *pstService)
{
// queit tune 관련 코드를 위한 임시 block.
	HINT32			 nSvcUid = -1;
	OxMediaPlay_MediaType_e eType;

	APK_MEDIA_PLAY_GetPlayType (ulViewId, &eType);

	HxSTD_MemSet (pstService, 0, sizeof(DxService_t));

	switch (eType)
	{
	case eOxMP_MEDIATYPE_LIVE:
	case eOxMP_MEDIATYPE_TSRPLAYBACK:
		nSvcUid = APK_MEDIA_PLAY_GetRequestId( ulViewId, eOxMP_MEDIATYPE_LIVE, (HUINT32*)&nSvcUid);
		break;

	default:
		break;
	}

	if (nSvcUid > 0)
	{
		OPL_Channel_t hChannel = OPL_Channel_FindChannelByUID (OPL_Channel_GetManager(), (int)nSvcUid);

		pstService->uid = nSvcUid;
		if (NULL != hChannel)
		{
			pstService->onid  = OPL_Channel_GetOrgNetworkID (hChannel);
			pstService->tsid  = OPL_Channel_GetTransponderID (hChannel);
			pstService->svcid = OPL_Channel_GetServiceID (hChannel);
		}

		return BPL_STATUS_OK;
	}

	return BPL_STATUS_ERROR;
}


static void live_proc(unsigned int ulViewId, unsigned int session, OxMediaPlay_Event_e eEvent, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	HBOOL							bStateChanged;
	HxList_t							*pstListItem;
	OxMediaPlay_LiveViewState_e	 	eViewState;
	OxMediaPlay_LiveLockState_e	 	eLockState;
	OplBroadcastPlayState_e			eLastPlayState;
	eOplBroadcastEventType			eOipfEvent = -1, eOipfAdditionalEvent = -1;
	HUINT32							nSvcUid=0;

#if defined (CONFIG_PROD_HMS1000T) || defined(CONFIG_PROD_DIGIMAXT2)
	static HBOOL						bNoSignal = FALSE;
#endif
	static HBOOL						bNokAntenna = FALSE;

	// oplbroadcast 내부의 status 관련 처리 :
	switch (eEvent)
	{
	case eOxMP_EVENT_LIVE_SvcChanged:
		{
			HUINT32 ulMainViewId = 0;
			APK_MEDIA_PLAY_GetMainViewId(&ulMainViewId);

			if( ulMainViewId == ulViewId )
			{
				APK_MEDIA_PLAY_GetRequestId (ulViewId, eOxMP_MEDIATYPE_LIVE, (HUINT32*)&nSvcUid);
				if (nSvcUid > 0)
				{
					opl_broadcast_memorizeCurrentChannel ((HUINT32)nSvcUid);
					s_ulOplBroadcast_LastMasterSvcUid = (HUINT32)nSvcUid;
				}
/*
				APK_MEDIA_PLAY_GetRequestId (ulViewId, &nSvcUid); // supple change
				if (nSvcUid > 0)
				{
					s_ulOplBroadcast_LastSuppleSvcUid = (HUINT32)nSvcUid;
				}
				else
				{
					s_ulOplBroadcast_LastSuppleSvcUid = s_ulOplBroadcast_LastMasterSvcUid;
				}
*/
			}
		}
		break;

/*
	case eOxMP_EVENT_TUNE_Locked:
		eOipfEvent = OPLBROADCAST_EVENT_TYPE_CHANNEL_CHANGE_SUCCEED;
		break;

	case eOxMP_EVENT_TUNE_NoSignal:
		eOipfEvent = OPLBROADCAST_EVENT_TYPE_CHANNEL_CHANGE_ERROR;
		break;
*/
	case eOxMP_EVENT_SI_EIT_CHANGED:
		/* sbkim, do nothing - PF 표시 Timing 때문에 혹시 만들어 둔다.
		eOipfEvent = OPLBROADCAST_EVENT_TYPE_PROGRAMMES_CHANGED;*/
		break;

	case eOxMP_EVENT_LIVE_EventRelay:
		eOipfEvent = OPLBROADCAST_EVENT_TYPE_EVENT_RELAY_NOTIFY;
		break;

	case eOxMP_EVENT_LIVE_Ews:
		eOipfEvent = OPLBROADCAST_EVENT_TYPE_EWS_NOTIFY;
		break;

	case eOxMP_EVENT_PinCtrlChanged:
	case eOxMP_EVENT_LIVE_PinCtrlChangedByBCAS:
		eOipfEvent = OPLBROADCAST_EVENT_TYPE_PARENTAL_RATING_CHANGE;
		break;

	case eOxMP_EVENT_RequestPincode:
		eOipfEvent = OPLBROADCAST_EVENT_TYPE_REQUEST_PINCODE;
		break;

	case eOxMP_EVENT_TrickRestrictModeChanged:
		eOipfEvent = OPLBROADCAST_EVENT_TYPE_TRICK_RESTRICT_MODE_CHANGED;
		break;

	case eOxMP_EVENT_COMPONENT_SELECTED:
		{
			if (eOxMP_COMPONENT_SUBTITLE == ulParam1)
			{
				HINT32	nTrackIndex = ulParam2;

				if (nTrackIndex >= 0)
				{
					OxMediaPlay_Component_t	stComponent;
					HINT32 nlValue;

					if (APK_MEDIA_PLAY_GetComponent(ulViewId, eOxMP_COMPONENT_SUBTITLE, nTrackIndex, &stComponent) == ERR_OK)
					{
						HAPI_BroadcastSignal("signal:onSubtitleTypeChanged", TRUE, "i", stComponent.stSubtitle.eSubtitleType);

						APK_DB_GetInt(USERDB_SUBTITLE_DISPLAY, &nlValue);
						HAPI_BroadcastSignal("signal:onSubtitleState", 1, "i", nlValue ? 1 : 0);
						HAPI_BroadcastSignal("signal:onSubtitleStateChekForMhegIcs", TRUE, "i", 1); /* work around: MHE ICS023 scene5: no video 상태 에서 subtitle이 나온다. */
					}
				}
			}
		}
		break;
	case eOxMP_EVENT_NAGRA_PRM_UPDATE:
		HAPI_BroadcastSignal("signal:onNagraPRMstart", 0, "i", 0);
		break;
	default:
		break;
	}

	// 각 Broadcast Object에 돌려야 하는 것들
	for (pstListItem = s_pstBcastObjList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		xOplBroadcast_t		*hBcast = (xOplBroadcast_t *)HLIB_LIST_Data (pstListItem);

		if ((NULL != hBcast) && (ulViewId == hBcast->ulViewId))
		{
			switch (eEvent)
			{
/*
 * note : UI Playbar 의 position 값을 polling 하는 방법을 개선한
 *		  position을 바로 올려주는 방법으로 할 경우 Live의 Playbar를 위한 코드임
 *		  현재 UI는 polling방식을 사용함으로 아래 코드는 Skip한다.
 */
#if 0
				case eOxMP_EVENT_LIVE_TSRTimecodeChanged:
					/*
					 * LIVE 시청중에 PLAYBAR 정보를 올려주기 위한 Event 처리
					 */
					eOipfEvent = OPLBROADCAST_EVENT_TYPE_PLAY_POSITION_CHANGED;
					break;
#endif
				case eOxMP_EVENT_STARTED:
					/*clean TSR value*/
					//hBcast->nTsrPlaytime		=	0;
					hBcast->nTsrPlaytimeTick	=	0;
					//hBcast->nTsrStarttime		=	0;
					hBcast->nTsrStarttimeTick	=	0;
					hBcast->bIsTsrPlaying       =   0;
					break;

				case eOxMP_EVENT_COMPONENT_VIDEO_CHANGED:
					hBcast->eChangedComponent = eOxMP_COMPONENT_VIDEO;
					eOipfEvent = OPLBROADCAST_EVENT_TYPE_COMPONENT_CHANGED;
					break;
				case eOxMP_EVENT_COMPONENT_AUDIO_CHANGED:
					hBcast->eChangedComponent = eOxMP_COMPONENT_AUDIO;
					eOipfEvent = OPLBROADCAST_EVENT_TYPE_COMPONENT_CHANGED;
					break;

				case eOxMP_EVENT_LIVE_SvcChanged:
					hBcast->eSessionState = OPLBROADCAST_SESSION_STATE_INITIALIZED;
					hBcast->ulSessionHandle = 0;

					hBcast->eViewState	= hBcast->eLastViewState	= eOxMP_LiveViewState_NONE;
					hBcast->eLockState	= hBcast->eLastLockState	= eOxMP_LiveLockState_NONE;
					hBcast->ePlaybackError	= OPLBROADCAST_CHANNEL_ERROR_CHANNEL_NOT_SUPPORTED;

					/*state는 바꾸되 play state change event는 보내지 않는다.*/
					hBcast->ePlayState = OPLBROADCAST_PLAY_STATE_CONNECTING;

					/*clean TSR value*/
					hBcast->nTsrPlaytime		=	0;
					hBcast->nTsrPlaytimeTick	=	0;
					hBcast->nTsrStarttime		=	0;
					hBcast->nTsrStarttimeTick	=	0;
					hBcast->bIsTsrPlaying       =   0;
					break;

				case eOxMP_EVENT_SI_EIT_CHANGED:
#if defined( CONFIG_OP_JAPAN )
					{
						BPLStatus	hErr = BPL_STATUS_ERROR;
						DxService_t 	stSvcInfo;

						HxSTD_MemSet( &stSvcInfo, 0x00, sizeof( DxService_t ));

						hErr = opl_broadcast_getCurrentMasterService (hBcast->ulViewId, &stSvcInfo);
						if (BPL_STATUS_OK == hErr)
						{
								if (hBcast->hQueryHandle)
								{
									while (hBcast->nPFLen--)
									{
										OPL_Programme_Delete(hBcast->ahPF[hBcast->nPFLen]);
									}
									hBcast->nPFLen = 0;
									OPL_METADATASEARCH_SearchPF(hBcast->hQueryHandle, stSvcInfo.uid, (OPL_SearchPfMonitor_t)opl_broadcast_PfMonitor, (void *)hBcast);
								}
						}
					}
#endif
					break;

				case eOxMP_EVENT_SI_RCT_CHANGED:
					{
						hBcast->nLinkCount = ulParam1;
						hBcast->nImgSize = ulParam2;
						printf("\n##### sh ##### [%s][%d] hBcast->nLinkCount=[%d], hBcast->nImgSize=[%d]\n",__FUNCTION__,__LINE__,hBcast->nLinkCount,hBcast->nImgSize);
						eOipfEvent = OPLBROADCAST_EVENT_TYPE_SI_RCT_CHANGED;
					}
					break;
				case eOxMP_EVENT_SI_NETWORK_CHANGED:
					hBcast->nStartTimeofChange = ulParam1;
					hBcast->nASODuration = (HUINT32)ulParam2;
					hBcast->pszNetworkMsg = (HUINT8*)ulParam3;
					eOipfEvent = OPLBROADCAST_EVENT_TYPE_NCD_CHANGED;
					break;
				case eOxMP_EVENT_SI_NETWORK_URI_CHANGED:
					{
						hBcast->pszNetworkUri = (HUINT8*)ulParam1;
						eOipfEvent = OPLBROADCAST_EVENT_TYPE_NETWORK_URI_CHANGED;
					}
					break;
				case eOxMP_EVENT_LIVE_EventRelay:
					hBcast->eventRelaySvcUid = ulParam1;
					break;

				case eOxMP_EVENT_LIVE_Ews:
					hBcast->ewsSvcUid	 = ulParam1;
					hBcast->ewsCurSvcUid = ulParam2;
					break;

				case eOxMP_EVENT_PinCtrlChanged:
					hBcast->stParentalRatingStatus.contentId 	= 0;
					hBcast->stParentalRatingStatus.bBlocked		= ulParam1;
					hBcast->stParentalRatingStatus.ratingValue	= ulParam2;
#if defined(CONFIG_OP_SES)
					hBcast->stParentalRatingStatus.bBlockedByCas = ulParam3;
#else
					hBcast->stParentalRatingStatus.caSystemId	= ulParam3;
#endif


#if defined( CONFIG_OP_JAPAN )
					/*cas 상태인 경우에는 eMctrlEvent_LIVE_PinCtrlChangedByBCAS 에 의해 rating 이 control된다.*/
					if(eOxMP_LiveViewState_CAS == hBcast->eViewState)
#else
					if ((eOxMP_LiveViewState_CAS != hBcast->eViewState) && (eOxMP_LiveViewState_OK != hBcast->eViewState))
#endif
					{
						continue;
					}
					break;

				case eOxMP_EVENT_RequestPincode:
					hBcast->pszPincodeMsg = (HUINT8*)HLIB_STD_StrDup((const HCHAR *)ulParam1);
					break;

				case eOxMP_EVENT_LIVE_PinCtrlChangedByBCAS:
					hBcast->stParentalRatingStatus.contentId = 0;
					hBcast->stParentalRatingStatus.bBlocked		= ulParam1;
					hBcast->stParentalRatingStatus.ratingValue	= ulParam2;
					hBcast->stParentalRatingStatus.caSystemId 	= ulParam3;

					if ((eOxMP_LiveViewState_CAS != hBcast->eViewState) && (eOxMP_LiveViewState_OK != hBcast->eViewState))
					{
						continue;
					}
					break;

				case eOxMP_EVENT_TUNE_Locked:
					// 어떤 경우이건 Tune Locked/Fail 이 첫 시작이어야 한다.
					// View / Lock State에 따라 동작해야 한다.
#if defined (CONFIG_PROD_HMS1000T) || defined(CONFIG_PROD_DIGIMAXT2)
					bNoSignal = FALSE;
#endif
					eLastPlayState = hBcast->ePlayState;

					if (bNokAntenna == TRUE)
						break;

					if (OPLBROADCAST_SESSION_STATE_INITIALIZED == hBcast->eSessionState)
					{
						hBcast->eSessionState = OPLBROADCAST_SESSION_STATE_STARTED;

						if ((eOxMP_LiveViewState_NONE == hBcast->eViewState) || (eOxMP_LiveLockState_NONE == hBcast->eLockState))
						{
							eOipfEvent = OPLBROADCAST_EVENT_TYPE_CHANNEL_CHANGE_SUCCEED;
							hBcast->ePlayState		= OPLBROADCAST_PLAY_STATE_CONNECTING;
							hBcast->ePlaybackError	= OPLBROADCAST_CHANNEL_ERROR_NO_ERROR;
						}
						else if ((eOxMP_LiveViewState_OK == hBcast->eViewState) && (eOxMP_LiveLockState_OK == hBcast->eLockState))
						{
							eOipfEvent = OPLBROADCAST_EVENT_TYPE_CHANNEL_CHANGE_SUCCEED;
							hBcast->ePlayState		= OPLBROADCAST_PLAY_STATE_PRESENTING;
							hBcast->ePlaybackError	= OPLBROADCAST_CHANNEL_ERROR_NO_ERROR;
						}
						else
						{
							eOipfEvent = OPLBROADCAST_EVENT_TYPE_CHANNEL_CHANGE_ERROR;
							hBcast->ePlayState		= OPLBROADCAST_PLAY_STATE_CONNECTING;
							hBcast->ePlaybackError	= opl_broadcast_getNextErrorByStates (hBcast, eOxMP_EVENT_TUNE_Locked);
						}

							eOipfAdditionalEvent = OPLBROADCAST_EVENT_TYPE_PLAYSTATE_CHANGE;
						}
					else
					{
						// Presenting 중에 또 Tune Locked가 온다고 하면 그것은 무시해도 된다.
						if (OPLBROADCAST_PLAY_STATE_PRESENTING != hBcast->ePlayState)
						{
							/*  CATAL TEST Error TC #122 */
							//hBcast->ePlayState = OPLBROADCAST_PLAY_STATE_CONNECTING;
							hBcast->ePlayState = OPLBROADCAST_PLAY_STATE_PRESENTING;
						}

						hBcast->ePlaybackError = OPLBROADCAST_CHANNEL_ERROR_NO_ERROR;

						if (eLastPlayState != hBcast->ePlayState)
						{
							eOipfEvent = OPLBROADCAST_EVENT_TYPE_PLAYSTATE_CHANGE;
						}
					}
					break;

				case eOxMP_EVENT_TUNE_NoSignal:
#if defined (CONFIG_PROD_HMS1000T) || defined(CONFIG_PROD_DIGIMAXT2)
					bNoSignal = TRUE;
#endif
					eLastPlayState = hBcast->ePlayState;

					if (bNokAntenna == TRUE)
						break;

#if defined (CONFIG_PROD_FVP4000T)
					//if (OPLBROADCAST_PLAY_STATE_CONNECTING != hBcast->ePlayState) /*	BBC Catal TC#122 - when signal is unpluged, playstate is 0.  */
					{
						//hBcast->ePlayState = OPLBROADCAST_PLAY_STATE_CONNECTING;
						hBcast->ePlayState = OPLBROADCAST_PLAY_STATE_UNREALIZED;
					}
#else
					if (OPLBROADCAST_PLAY_STATE_CONNECTING != hBcast->ePlayState) /*  OIPF Spec  */
					{
						hBcast->ePlayState = OPLBROADCAST_PLAY_STATE_CONNECTING;
					}
#endif
					if (OPLBROADCAST_SESSION_STATE_INITIALIZED == hBcast->eSessionState)
					{
						eOipfEvent = OPLBROADCAST_EVENT_TYPE_CHANNEL_CHANGE_ERROR;
						hBcast->eSessionState = OPLBROADCAST_SESSION_STATE_STARTED;
					}

					/*state가 connecting->connecting 이어도 error가 달라졌다면 event를 발생시킨다.*/
					if ((eLastPlayState != hBcast->ePlayState)
						||((eLastPlayState == OPLBROADCAST_PLAY_STATE_CONNECTING)
						&&(hBcast->ePlaybackError != OPLBROADCAST_CHANNEL_ERROR_CANNOT_TUNE)))
					{
						hBcast->ePlaybackError = OPLBROADCAST_CHANNEL_ERROR_CANNOT_TUNE;
						if (-1 == eOipfEvent)
						{
							eOipfEvent = OPLBROADCAST_EVENT_TYPE_PLAYSTATE_CHANGE;
						}
						else
						{
							eOipfAdditionalEvent = OPLBROADCAST_EVENT_TYPE_PLAYSTATE_CHANGE;
						}
					}
					break;

#if defined (CONFIG_PROD_HMS1000T) || defined(CONFIG_PROD_DIGIMAXT2)
				case eOxMP_EVENT_TUNE_AntennaOk:
					bNokAntenna = FALSE;
					eLastPlayState = hBcast->ePlayState;

					if (bNoSignal == TRUE)
					{
						hBcast->ePlaybackError = OPLBROADCAST_CHANNEL_ERROR_CANNOT_TUNE;
						eOipfEvent = OPLBROADCAST_EVENT_TYPE_PLAYSTATE_CHANGE;
						eOipfAdditionalEvent = OPLBROADCAST_EVENT_TYPE_PLAYSTATE_CHANGE;
					}
					break;

					if (OPLBROADCAST_SESSION_STATE_INITIALIZED == hBcast->eSessionState)
					{
						hBcast->eSessionState = OPLBROADCAST_SESSION_STATE_STARTED;

						if ((eOxMP_LiveViewState_NONE == hBcast->eViewState) || (eOxMP_LiveLockState_NONE == hBcast->eLockState))
						{
							eOipfEvent = OPLBROADCAST_EVENT_TYPE_CHANNEL_CHANGE_SUCCEED;
							hBcast->ePlayState		= OPLBROADCAST_PLAY_STATE_CONNECTING;
							hBcast->ePlaybackError	= OPLBROADCAST_CHANNEL_ERROR_NO_ERROR;
						}
						else if ((eOxMP_LiveViewState_OK == hBcast->eViewState) && (eOxMP_LiveLockState_OK == hBcast->eLockState))
						{
							eOipfEvent = OPLBROADCAST_EVENT_TYPE_CHANNEL_CHANGE_SUCCEED;
							hBcast->ePlayState		= OPLBROADCAST_PLAY_STATE_PRESENTING;
							hBcast->ePlaybackError	= OPLBROADCAST_CHANNEL_ERROR_NO_ERROR;
						}
						else
						{
							eOipfEvent = OPLBROADCAST_EVENT_TYPE_CHANNEL_CHANGE_ERROR;
							hBcast->ePlayState		= OPLBROADCAST_PLAY_STATE_CONNECTING;
							hBcast->ePlaybackError	= opl_broadcast_getNextErrorByStates (hBcast, eOxMP_EVENT_TUNE_AntennaOk);
						}

						if (-1 == eOipfEvent)
						{
							eOipfEvent = OPLBROADCAST_EVENT_TYPE_PLAYSTATE_CHANGE;
						}
						else
						{
							eOipfAdditionalEvent = OPLBROADCAST_EVENT_TYPE_PLAYSTATE_CHANGE;
						}
					}
					else
					{
						// Presenting 중에 또 Tune Locked가 온다고 하면 그것은 무시해도 된다.
						if (OPLBROADCAST_PLAY_STATE_PRESENTING != hBcast->ePlayState)
						{
							hBcast->ePlayState = OPLBROADCAST_PLAY_STATE_CONNECTING;
						}

						hBcast->ePlaybackError = OPLBROADCAST_CHANNEL_ERROR_NO_ERROR;

						if (eLastPlayState != hBcast->ePlayState)
						{
							eOipfEvent = OPLBROADCAST_EVENT_TYPE_PLAYSTATE_CHANGE;
						}
					}
					break;

				case eOxMP_EVENT_TUNE_AntennaNok:
					bNokAntenna = TRUE;
					eLastPlayState = hBcast->ePlayState;
					if (OPLBROADCAST_PLAY_STATE_CONNECTING != hBcast->ePlayState)
					{
						hBcast->ePlayState = OPLBROADCAST_PLAY_STATE_CONNECTING;
					}

					if (OPLBROADCAST_SESSION_STATE_INITIALIZED == hBcast->eSessionState)
					{
						eOipfEvent = OPLBROADCAST_EVENT_TYPE_CHANNEL_CHANGE_ERROR;
						hBcast->eSessionState = OPLBROADCAST_SESSION_STATE_STARTED;
					}

					/*state가 connecting->connecting 이어도 error가 달라졌다면 event를 발생시킨다.*/
					if ((eLastPlayState != hBcast->ePlayState)
						||((eLastPlayState == OPLBROADCAST_PLAY_STATE_CONNECTING)
						&&(hBcast->ePlaybackError != OPLBROADCAST_CHANNEL_ERROR_NOK_ANTENNA)))
					{
						hBcast->ePlaybackError = OPLBROADCAST_CHANNEL_ERROR_NOK_ANTENNA;

						if (-1 == eOipfEvent)
						{
							eOipfEvent = OPLBROADCAST_EVENT_TYPE_PLAYSTATE_CHANGE;
						}
						else
						{
							eOipfAdditionalEvent = OPLBROADCAST_EVENT_TYPE_PLAYSTATE_CHANGE;
						}
					}
					break;
#endif

				case eOxMP_EVENT_LIVE_ViewStateChanged:
					eOipfEvent = eOipfAdditionalEvent = -1;
					eViewState = ulParam1;

					if (hBcast->eViewState == eViewState)
					{
						HxLOG_Error ("[%s:%d] ViewState not changed: \n", __FUNCTION__, __LINE__);
						break;
					}

					hBcast->eLastViewState	= hBcast->eViewState;
					hBcast->eViewState		= eViewState;

					if (OPLBROADCAST_SESSION_STATE_INITIALIZED == hBcast->eSessionState)
					{
						break;
					}

					bStateChanged = FALSE;

					opl_broadcast_getErrorFromNotifier (hBcast, eEvent, &bStateChanged);
					if (TRUE == bStateChanged)
					{
						eOipfEvent = OPLBROADCAST_EVENT_TYPE_PLAYSTATE_CHANGE;
						/*  DIKIM - CATAL TEST  */
					//	hBcast->ePlayState = OPLBROADCAST_PLAY_STATE_UNREALIZED;
					}
					break;

				case eOxMP_EVENT_LIVE_LockStateChanged:
					eOipfEvent = eOipfAdditionalEvent = -1;
					eLockState = ulParam1;

					if (hBcast->eLockState == eLockState)
					{
						HxLOG_Error ("[%s:%d] LockState not changed: \n", __FUNCTION__, __LINE__);
						break;
					}

					hBcast->eLastLockState	= hBcast->eLockState;
					hBcast->eLockState		= eLockState;

					if (OPLBROADCAST_SESSION_STATE_INITIALIZED == hBcast->eSessionState)
					{
						break;
					}

					bStateChanged = FALSE;

					opl_broadcast_getErrorFromNotifier (hBcast, eEvent, &bStateChanged);
					if (TRUE == bStateChanged)
					{
						eOipfEvent = OPLBROADCAST_EVENT_TYPE_PLAYSTATE_CHANGE;
					}
					break;

				case eOxMP_EVENT_LIVE_SvcStopped:
					eLastPlayState = hBcast->ePlayState;

					if (OPLBROADCAST_PLAY_STATE_STOPPED != hBcast->ePlayState)
					{
						hBcast->ePlayState = OPLBROADCAST_PLAY_STATE_STOPPED;
					}
					eOipfEvent = OPLBROADCAST_EVENT_TYPE_PLAYSTATE_CHANGE;
					if (OPLBROADCAST_SESSION_STATE_INITIALIZED == hBcast->eSessionState)
					{
						hBcast->eSessionState = OPLBROADCAST_SESSION_STATE_STARTED;
					}
					hBcast->ePlaybackError = OPLBROADCAST_CHANNEL_ERROR_NO_ERROR;
					break;

				case eOxMP_EVENT_TSR_PAUSED:
				case eOxMP_EVENT_TSR_RESUMED:
					eOipfEvent = OPLBROADCAST_EVENT_TYPE_TSR_STATE_CHANGED;
					break;

                case eOxMP_EVENT_TrickRestrictModeChanged:
                    hBcast->eStorageRight = (OplBroadcastStorageRight_e)ulParam2;
                    break;

				default:
					break;
			}

			if (NULL != hBcast->liveEventListener)
			{
#if defined(CONFIG_SUPPORT_MHEGPLAYER_APP)
				// mheg quiet tune 인 경우에 appl 로 noti를 보내면 안됨.
				if(eOipfEvent == OPLBROADCAST_EVENT_TYPE_CHANNEL_CHANGE_SUCCEED)
				{
					OxMediaPlay_SuppleSvcType_e eSuppleSvcType = eOxMP_SUPPLESVCTYPE_Normal;

					APK_MEDIA_PLAY_GetSuppleType(ulViewId, &eSuppleSvcType);
					if(eSuppleSvcType == eOxMP_SUPPLESVCTYPE_SilentTune)
					{
						HxLOG_Debug("Mheg Quiet Tune , So don't Notify to Application eSuppleSvcType(%d) "C_CODE_RESET"\n", eSuppleSvcType);
						return;
					}
				}
#endif
				if (eOipfEvent != -1)
				{
					(*(hBcast->liveEventListener))(hBcast->hVbcastHandle, eOipfEvent);			// _broadcast_onLiveChange
				}

				if(eOipfAdditionalEvent != -1)
				{
					(*(hBcast->liveEventListener))(hBcast->hVbcastHandle, eOipfAdditionalEvent);			// _broadcast_onLiveChange
				}
			}
		}
	}
}

static void tsr_proc(unsigned int ulViewId, unsigned int session, OxMediaPlay_Event_e eEvent, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	HxList_t						*pstListItem;
	HERROR							 hErr;
	eOplBroadcastEventType			 eOipfEvent = -1;
	eOplRecorderState				 eRecState = -1;


	// OIPF 2.1 - 192 page, 7.13.2.2
	/*	If the playback reaches the beginning of the time-shift buffer at rewind playback speed, then the play
		state is changed to 2 (‘paused’) and a PlaySpeedChanged event with a speed of 0 is generated. If the
		playback reaches the end of the time-shift buffer at fast-forward playback speed, then the play speed is
		set to 1.0 and a PlaySpeedChanged event is generated. */
	//  ==> RW : start pos -> play state = paused, generate the PlaySpeedChanged event (speed = 0)
	//  ==> FF : end pos   -> generate the PlaySpeedChanged event, play speed = 1

	switch (eEvent)
	{
	case eOxMP_EVENT_PLAYSTATE:
		{
			OxMediaPlay_PlayState_e eState;

			APK_MEDIA_PLAY_GetPlayState (ulViewId, &eState);
			switch( eState )
			{
				case eOxMP_PLAYSTATE_STOPPED:
					eRecState = OPLRECORDER_REC_STATE_UNREALIZED;

					HxLOG_Error ("HSSEO : OPLRECORDER_REC_STATE_UNREALIZED Error\n");
					break;
				default :
					break;
			}
			break;
		}
	case eOxMP_EVENT_STARTED:
		eRecState = OPLRECORDER_REC_STATE_TS_STARTED;
		break;

	case eOxMP_EVENT_STOPPED:
		eRecState = OPLRECORDER_REC_STATE_UNREALIZED;
		break;

	case eOxMP_EVENT_BOF:
		/* HMS1000S, IR4000HD : SPEED 100 , After BOF*/
#if 1
		hErr = APK_MEDIA_PLAY_SetPlaySpeed (ulViewId, 100);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("APK_MEDIA_PLAY_SetPlaySpeed Error\n");
		}
#else
		hErr = APK_MEDIA_PLAY_SetPlaySpeed (ulViewId, 0);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("APK_MEDIA_PLAY_SetPlaySpeed Error\n");
		}
#endif
		break;

	case eOxMP_EVENT_EOF:
		/* HMS1000S, IR4000HD : SPEED 100 , After EOF*/
#if 1
		eRecState = OPLRECORDER_REC_STATE_UNREALIZED;
#else
		hErr = APK_MEDIA_PLAY_SetPlaySpeed (ulViewId, 100);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("APK_MEDIA_PLAY_SetPlaySpeed Error\n");
		}
#endif
		break;

	case eOxMP_EVENT_SPEED_CHANGED:
		eOipfEvent = OPLBROADCAST_EVENT_TYPE_PLAY_SPEED_CHANGED;
		break;
	case eOxMP_EVENT_PLAYTIME:
		eOipfEvent = OPLBROADCAST_EVENT_TYPE_PLAY_POSITION_CHANGED;
		break;
	case eOxMP_EVENT_PinCtrlChanged:
		eOipfEvent = OPLBROADCAST_EVENT_TYPE_PARENTAL_RATING_CHANGE;
		break;
	case eOxMP_EVENT_DRM_BLOCK:
		eOipfEvent = OPLBROADCAST_EVENT_TYPE_DRM_RIGHTS_ERROR_EVENT;
		break;
	case eOxMP_EVENT_SupportedSpeedChanged:
		eOipfEvent = OPLBROADCAST_EVENT_TYPE_SUPPORTED_PLAY_SPEED_CHANGED;
		break;
	case eOxMP_EVENT_TrickRestrictModeChanged:
		eOipfEvent = OPLBROADCAST_EVENT_TYPE_TRICK_RESTRICT_MODE_CHANGED;
		break;

	case eOxMP_EVENT_PLAYERROR:
		eOipfEvent = OPLBROADCAST_EVENT_TYPE_PLAYSTATE_CHANGE;
		break;

	case eOxMP_EVENT_COMPONENT_SELECTED:
		{
			if (eOxMP_COMPONENT_SUBTITLE == ulParam1)
			{
				HINT32	nTrackIndex = ulParam2;

				if (nTrackIndex >= 0)
				{
					OxMediaPlay_Component_t stComponent;
					HINT32 					nlValue;
					if (APK_MEDIA_PLAY_GetComponent(ulViewId, eOxMP_COMPONENT_SUBTITLE, nTrackIndex, &stComponent) == ERR_OK)
					{
						HAPI_BroadcastSignal("signal:onSubtitleTypeChanged", TRUE, "i", stComponent.stSubtitle.eSubtitleType);
						APK_DB_GetInt(USERDB_SUBTITLE_DISPLAY, &nlValue);
						HAPI_BroadcastSignal("signal:onSubtitleState", 1, "i", nlValue ? 1 : 0);
					}
				}
			}
		}
		break;

	default:
		break;
	}

	// 각 Broadcast Object에 돌려야 하는 것들
	for (pstListItem = s_pstBcastObjList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		xOplBroadcast_t		*hBcast = (xOplBroadcast_t *)HLIB_LIST_Data (pstListItem);
		if ((NULL != hBcast) && (ulViewId == hBcast->ulViewId))
		{
			switch (eEvent)
			{
				case eOxMP_EVENT_PinCtrlChanged:
					hBcast->stParentalRatingStatus.contentId = 0;
					hBcast->stParentalRatingStatus.bBlocked		= ulParam1;
					hBcast->stParentalRatingStatus.ratingValue	= ulParam2;
#if defined(CONFIG_OP_SES)
					hBcast->stParentalRatingStatus.bBlockedByCas = ulParam3;
#else
					hBcast->stParentalRatingStatus.caSystemId	= ulParam3;
#endif


					if ((eOxMP_LiveViewState_CAS != hBcast->eViewState) && (eOxMP_LiveViewState_OK != hBcast->eViewState))
					{
						continue;
					}
					break;
				case eOxMP_EVENT_DRM_BLOCK:
					if(opl_broadcast_getDrmRightsStatusFromJson(&(hBcast->stDrmRightsStatus), ulParam1) == BPL_STATUS_ERROR)
					{
						continue;
					}
					if ((eOxMP_LiveViewState_CAS != hBcast->eViewState) && (eOxMP_LiveViewState_OK != hBcast->eViewState))
					{
						continue;
					}
					break;

				case eOxMP_EVENT_PLAYERROR:
					hBcast->ePlaybackError = OPLBROADCAST_CHANNEL_ERROR_INSUFFICIENT_RESOURCE;
					break;

                case eOxMP_EVENT_TrickRestrictModeChanged:
                    hBcast->eStorageRight = (OplBroadcastStorageRight_e)ulParam2;
                    break;

				default:
					break;
			}
			// Playback related
			if ((NULL != hBcast->tsrEventListener) && (-1 != eOipfEvent))
			{
				(*(hBcast->tsrEventListener))(hBcast->hVbcastHandle, eOipfEvent);			// _broadcast_onTsrChange
			}

			// Recording related
			if ((hBcast->eRecState < OPLRECORDER_REC_STATE_SCHEDULED) ||
				(hBcast->eRecState > OPLRECORDER_REC_STATE_REC_UPDATED) ||
				(eRecState == OPLRECORDER_REC_STATE_TS_STARTED) )
			{
				if ((-1 != eRecState) && (hBcast->eRecState != eRecState))
				{
					hBcast->eRecState = eRecState;

					if (NULL != hBcast->recEventListener)
					{
						(*(hBcast->recEventListener))(hBcast->hVbcastHandle, OPLBROADCAST_EVENT_TYPE_RECORDING_EVENT);			// _broadcast_onTsrChange
					}

				}
			}
		}
	}
}

static HBOOL opl_broadcast_eventFilter (HUINT32 ulViewId, OxMediaPlay_MediaType_e eType, OxMediaPlay_Event_e eEvent, HBOOL *updated)
{
	*updated = FALSE;

	if ((eOxMP_MEDIATYPE_LIVE == eType) || (eOxMP_MEDIATYPE_TSRPLAYBACK == eType) || (eOxMP_MEDIATYPE_PVRPLAYBACK == eType))
	{
		*updated = TRUE;
	}

	if (0)
	{
		if ((eOxMP_EVENT_MEDIA_ProbeErrored <= eEvent) && (eEvent <= eOxMP_EVENT_VIDEOSTREAMINFO))
		{
			*updated = TRUE;
		}
	}

	return TRUE;
}

static void opl_broadcast_notifyFunc ( HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eType, OxMediaPlay_Event_e eEvent, OxMediaPlay_NotifierData_t *pstNotifyData, void *pvUserData )
{
	HUINT32 ulParam1 = 0;
	HUINT32 ulParam2 = 0;
	HUINT32 ulParam3 = 0;

	if (NULL != pstNotifyData)
	{
#if defined(CONFIG_TIMESTAMP_64)
		ulParam1	=	(HUINT32)pstNotifyData->stEventNotify.ullParam1;
		ulParam2	=	(HUINT32)pstNotifyData->stEventNotify.ullParam2;
		ulParam3	=	(HUINT32)pstNotifyData->stEventNotify.ullParam3;
#else
		ulParam1	=	pstNotifyData->stEventNotify.ulParam1;
		ulParam2	=	pstNotifyData->stEventNotify.ulParam2;
		ulParam3	=	pstNotifyData->stEventNotify.ulParam3;
#endif
	}
	switch( eType )
	{
		case eOxMP_MEDIATYPE_LIVE:
		{
			live_proc(ulViewId, ulSessionId, eEvent, ulParam1, ulParam2, ulParam3);
			break;
		}
		case eOxMP_MEDIATYPE_TSRPLAYBACK:
		{
			// Chase를 PVR PLAYBACK으로 간주하면...여기도..
			tsr_proc(ulViewId, ulSessionId,	eEvent, ulParam1, ulParam2, ulParam3);
			break;
		}
		case eOxMP_MEDIATYPE_MEDIAPLAY:
		default :
			break;
	}
}

static void opl_broadcast_notifyCasMessageFunc(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	HxList_t					*pstListItem = NULL;
	xOplBroadcast_t				*hBcast = NULL;
	DxCAS_MMI_SessionEventType_e		eEventType = eDxCASMMI_EVENTTYPE_NONE;
	DxCAS_MmiSessionType_e	eApkSessionType = eDxCAS_SESSIONTYPE_NONE;
	Handle_t						hSession;

	eEventType = (DxCAS_MMI_SessionEventType_e)ulParam1;
	eApkSessionType = (DxCAS_MmiSessionType_e)ulParam2;
	hSession = (Handle_t)ulParam3;

	switch(eEventType)
	{
		case eDxCASMMI_EVENTTYPE_DISPLAYUI:
			switch(eApkSessionType)
			{
				case eDxCAS_SESSIONTYPE_BANNER:
					if(s_pstBcastObjList == NULL)
					{
						HxLOG_Error("Bcast Object List NULL\n");
						return;
					}
					pstListItem = s_pstBcastObjList;

					/* broadcast cas system message */
					do
					{
						hBcast = (xOplBroadcast_t *)HLIB_LIST_Data(pstListItem);

						if((hBcast != NULL) && (hBcast->casEventListener != NULL))
						{
							hBcast->hCasMsgSession = hSession;
							(*(hBcast->casEventListener))(hBcast->hVbcastHandle, OPLBROADCAST_EVENT_TYPE_CAS_MESSAGE_EVENT);
						}

						pstListItem = pstListItem->next;
					}while(pstListItem != NULL);
					break;

				default:
					break;
			}
			break;
		case eDxCASMMI_EVENTTYPE_REQUESTSESSIONSTART:
			switch(eApkSessionType)
			{
				case eDxCAS_SESSIONTYPE_EXTENSION:
					if(s_pstBcastObjList == NULL)
					{
						HxLOG_Error("Bcast Object List NULL\n");
						return;
					}

					pstListItem = s_pstBcastObjList;
					do
					{
						hBcast = (xOplBroadcast_t *)HLIB_LIST_Data(pstListItem);

						if((hBcast != NULL) && (hBcast->casEventListener != NULL))
						{
							hBcast->hCasMsgSession = hSession;
							(*(hBcast->casEventListener))(hBcast->hVbcastHandle, OPLBROADCAST_EVENT_TYPE_CAS_UI_TRIGGERED);
						}
						pstListItem = pstListItem->next;
					}while(pstListItem != NULL);
					break;

				default:
					break;
			}
			break;
#if 0// TODO: banner만 여기서 받는데, banner는 한번 열고 안 닫으니 상관이 없겠지만...
		case eDxCASMMI_EVENTTYPE_REQUESTSESSIONSTOP:
			break;
#endif

#if 0// TODO: 임시
		case eDxCASMMI_EVENTTYPE_SESSIONSTARTED:
			s_hSessionOplBroadCast[eApkSessionType] = hSession;
			break;

#endif
		default:
			break;
	}

	return;
}


void opl_broadcast_notifyRecFunc ( HUINT32 ulSvcUid, HUINT32 ulSessionId, OxMediaRec_Event_e eEvent, OxMediaRec_NotifierData_t *pstNotifyData, void *pvUserData )
{
	static HxList_t 				*pstListItem = NULL;
	eOplRecorderState 				eRecState = -1;
	eOplBroadcastEventType 			eOipfEvent = -1;

	// common scenario(follow OIPF Scenario) have to added in here
	switch (eEvent)
	{
	case eOxMR_EVENT_REC_STARTED:
			eRecState	= OPLRECORDER_REC_STATE_REC_STARTED;
		eOipfEvent	= OPLBROADCAST_EVENT_TYPE_RECORDING_EVENT;
		break;

	case eOxMR_EVENT_REC_STOPPED:
		eRecState	= OPLRECORDER_REC_STATE_REC_COMPLETED;
		eOipfEvent	= OPLBROADCAST_EVENT_TYPE_RECORDING_EVENT;
		break;
	case eOxMR_EVENT_REC_TSRBUFFER_COPYEND:
		eRecState	= OPLRECORDER_REC_STATE_TSRBUFFER_COPYEND;
		eOipfEvent	= OPLBROADCAST_EVENT_TYPE_RECORDING_EVENT;
		break;
	default:
		return;
	}

	// this have to notify to each Broadcast Objects
	for (pstListItem = s_pstBcastObjList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		xOplBroadcast_t		*hBcast = (xOplBroadcast_t *)HLIB_LIST_Data (pstListItem);

		if (NULL != hBcast)
		{
			HUINT32			 ulLiveSvcUid = 0;

			APK_MEDIA_PLAY_GetRequestId(hBcast->ulViewId, eOxMP_MEDIATYPE_LIVE, (HUINT32*)&ulLiveSvcUid);
			if(ulSvcUid == ulLiveSvcUid)
			{
				hBcast->ulRecSessionId = ulSessionId;
				if ((-1 != eRecState) && (hBcast->eRecState != eRecState))
				{
					hBcast->eRecState = eRecState;

					if ((NULL != hBcast->recEventListener) && (-1 != eOipfEvent))
					{
						(*(hBcast->recEventListener))(hBcast->hVbcastHandle, eOipfEvent);			// _broadcast_onRecChange
					}
				}
			}
		}
	}
}

static HERROR opl_broadcase_mhegNonQuietTune(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HxList_t					*pstListItem= NULL;
	xOplBroadcast_t			*hBcast = NULL;
	HUINT32 				eMhegTuneMode;
	HINT32					nMhegSvcUid;

	HxLOG_Assert(nArgc == 2);

	if(s_pstBcastObjList == NULL)
	{
		HxLOG_Error("Bcast Object List NULL\n");
		return ERR_FAIL;
	}

	nMhegSvcUid = HxOBJECT_INT(apArgv[0]);
	eMhegTuneMode = HxOBJECT_INT(apArgv[1]);

	pstListItem = s_pstBcastObjList;
	do
	{
		hBcast = (xOplBroadcast_t *)HLIB_LIST_Data(pstListItem);

		if((hBcast != NULL) && (hBcast->mhegEventListener != NULL))
		{
			OPL_Channel_t hChannel = NULL;


			hChannel = OPL_Channel_FindChannelByUID(NULL, nMhegSvcUid);
			if(hChannel != NULL)
			{
				hBcast->nMhegSvcUid= nMhegSvcUid;
				hBcast->eMhegTuneMode = (OxMediaPlay_TuningMode_e) eMhegTuneMode;
				hBcast->hMhegChannel = hChannel;

				printf("\n\n\t## ysmoon ## [%s, %d] nSvcUid(0x%x) , eTuneMode(0x%x) hMhegChannel(%p)\n\n", __FUNCTION__, __LINE__, hBcast->nMhegSvcUid, hBcast->eMhegTuneMode, hBcast->hMhegChannel);

				(*(hBcast->mhegEventListener))(hBcast->hVbcastHandle,OPLBROADCAST_EVENT_TYPE_MHEG_REQUEST_TUNE);
			}
		}
		pstListItem = pstListItem->next;
	}while(pstListItem != NULL);

	return ERR_OK;
}

static HERROR opl_broadcase_NotifyMhegEvent(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HxList_t					*pstListItem= NULL;
	xOplBroadcast_t			*hBcast = NULL;
	HUINT32 					eMhegTuneMode, ulMhegEvnet, ulInterruptible, ulMhegIcsStatus = 0;
	HINT32					nMhegSvcUid;
	eOplBroadcastEventType		eOplBroadCastEvent;

	if(s_pstBcastObjList == NULL)
	{
		HxLOG_Error("Bcast Object List NULL\n");
		return ERR_FAIL;
	}

	ulMhegEvnet = HxOBJECT_INT(apArgv[0]);

	HxLOG_Debug("ulMhegEvnet(0x%x) \n", ulMhegEvnet);

	switch(ulMhegEvnet)
	{
	case 30: // OPLBROADCAST_EVENT_TYPE_MHEG_REQUEST_TUNE;
			{
				nMhegSvcUid = HxOBJECT_INT(apArgv[1]);
				eMhegTuneMode = HxOBJECT_INT(apArgv[2]);
				eOplBroadCastEvent= OPLBROADCAST_EVENT_TYPE_MHEG_REQUEST_TUNE;
				HxLOG_Debug("OPLBROADCAST_EVENT_TYPE_MHEG_REQUEST_TUNE >>>>>>>>>>> nMhegSvcUid(0x%x) eMhegTuneMode(0x%x)\n", nMhegSvcUid, eMhegTuneMode);
			}
			break;

		case 31:// OPLBROADCAST_EVENT_TYPE_MHEG_INTERRUPTIBLE
			{
				ulInterruptible = HxOBJECT_INT(apArgv[1]);
				eOplBroadCastEvent= OPLBROADCAST_EVENT_TYPE_MHEG_INTERRUPTIBLE;
				HxLOG_Debug("OPLBROADCAST_EVENT_TYPE_MHEG_INTERRUPTIBLE >>>>>>>>>>> ulInterruptible(0x%x) \n", ulInterruptible);
			}
			break;

		case 35:// OPLBROADCAST_EVENT_TYPE_MHEG_ICS_STATUS
			{
				ulMhegIcsStatus = HxOBJECT_INT(apArgv[1]);
				eOplBroadCastEvent= OPLBROADCAST_EVENT_TYPE_MHEG_ICS_STATUS;
				HxLOG_Debug("OPLBROADCAST_EVENT_TYPE_MHEG_ICS_STATUS >>>>>>>>>>> ulMhegIcsStatus(0x%x) \n", ulMhegIcsStatus);
			}
			break;

		default:
			HxLOG_Debug("Can Not Process MhegEvent >>>>>>>>>>> ulMhegEvnet (0x%x) \n", ulMhegEvnet);
			return ERR_FAIL;
	}

	pstListItem = s_pstBcastObjList;
	do
	{
		hBcast = (xOplBroadcast_t *)HLIB_LIST_Data(pstListItem);

		if((hBcast != NULL) && (hBcast->mhegEventListener != NULL))
		{
			if(eOplBroadCastEvent == OPLBROADCAST_EVENT_TYPE_MHEG_REQUEST_TUNE)
			{
				OPL_Channel_t hChannel = NULL;

				hChannel = OPL_Channel_FindChannelByUID(NULL, nMhegSvcUid);
				if(hChannel != NULL)
				{
					hBcast->nMhegSvcUid= nMhegSvcUid;
					hBcast->eMhegTuneMode = (OxMediaPlay_TuningMode_e) eMhegTuneMode;
					hBcast->hMhegChannel = hChannel;

					HxLOG_Debug(" Send OPLBROADCAST_EVENT_TYPE_MHEG_REQUEST_TUNE :: nSvcUid(0x%x) , eTuneMode(0x%x) hMhegChannel(%p)\n",
						hBcast->nMhegSvcUid, hBcast->eMhegTuneMode, hBcast->hMhegChannel);

					(*(hBcast->mhegEventListener))(hBcast->hVbcastHandle,eOplBroadCastEvent);
				}
			}
			else if(eOplBroadCastEvent == OPLBROADCAST_EVENT_TYPE_MHEG_INTERRUPTIBLE)
			{
				hBcast->bInterruptible= ulInterruptible;

				HxLOG_Debug(" Send OPLBROADCAST_EVENT_TYPE_MHEG_INTERRUPTIBLE :: bInterruptible(%d) \n",hBcast->bInterruptible);

				(*(hBcast->mhegEventListener))(hBcast->hVbcastHandle,eOplBroadCastEvent);

			}
			else if(eOplBroadCastEvent == OPLBROADCAST_EVENT_TYPE_MHEG_ICS_STATUS)
			{
				hBcast->bMhegIcsStatus= ulMhegIcsStatus;

				HxLOG_Debug(" Send OPLBROADCAST_EVENT_TYPE_MHEG_ICS_STATUS :: bMhegIcsStatus(%d) \n",hBcast->bMhegIcsStatus);

				(*(hBcast->mhegEventListener))(hBcast->hVbcastHandle,eOplBroadCastEvent);
			}
			else
			{

			}
		}
		pstListItem = pstListItem->next;
	}while(pstListItem != NULL);

	return ERR_OK;
}

static BPLStatus opl_broadcast_getBufferingTypeAndTime (OplBroadcast_t *hBroadcast, xOplBroadcastBufferingType_e *peType, HUINT32 *pulBufTime)
{
	/*
	 * 현 Live 채널에서 Buffer가 ChasePlayback, TSRPlayback 인지 판단.
	 */

	HUINT32					 ulTime = 0;
	HUINT32 				 ulRsvSlot = 0;
	xOplBroadcast_t			*hBcast = xbroadcast(hBroadcast);

	HERROR	hErr;
	HBOOL	bRecording;
	DxService_t	stSvcInfo={0,};

	if( (NULL == pulBufTime) || (NULL == peType) )
		return BPL_STATUS_ERROR;

	*pulBufTime	= 0;
	*peType		= eOplBroadcastBufType_None;

	hErr = opl_broadcast_getCurrentMasterService (hBcast->ulViewId, &stSvcInfo);
	if( ERR_OK != hErr )
	{
		HxLOG_Error("[%s:%d] opl_broadcast_getCurrentMasterService Error: \n", __FUNCTION__, __LINE__);
		goto EXIT_PROC;
	}

	hErr = APK_MEDIA_RECORD_IsRecording(stSvcInfo.uid ,&bRecording);
	if( ERR_OK != hErr )
	{
		HxLOG_Error("[%s:%d] APK_MEDIA_RECORD_IsRecording Error: \n", __FUNCTION__, __LINE__);
		goto EXIT_PROC;
	}

	if (TRUE == bRecording)
	{
		ulTime = 0;
	  // TBR Recording 시  hBcast->ulRecsessionId를 찾을 수 없다 ( APK_MEDIA_RECORD_Start를 이용하지 않았기 때문)
    // svc uid를 통해 Sama의 slot id를 가져와 APK_MEDIA_RECORD_GetRecTime에 넣어준다
		hErr = APK_MEDIA_RECORD_GetRsvSlotByUID(stSvcInfo.uid, &ulRsvSlot);

		if( ERR_OK != hErr )
		{
			HxLOG_Error("[%s:%d] APK_MEDIA_RECORD_GetRsvSlotByUID: \n", __FUNCTION__, __LINE__);
			goto EXIT_PROC;
		}

		hErr = APK_MEDIA_RECORD_GetRecTime (ulRsvSlot, &ulTime);

		if( ERR_OK != hErr )
		{
			HxLOG_Error("[%s:%d] APK_MEDIA_RECORD_GetRecTime Error: \n", __FUNCTION__, __LINE__);
			goto EXIT_PROC;
		}

		*peType = eOplBroadcastBufType_Usr;
	}
	else
	{
		hErr = APK_MEDIA_PLAY_GetTsrRecTime (hBcast->ulViewId, &ulTime);
		if( ERR_OK != hErr )
		{
			HxLOG_Error("[%s:%d] APK_MEDIA_PLAY_GetTsrRecTime Error: \n", __FUNCTION__, __LINE__);
			goto EXIT_PROC;
		}

		*peType = eOplBroadcastBufType_Tsr;
	}

	*pulBufTime = ulTime;


EXIT_PROC:
	return BPL_STATUS_OK;
}

static BPLStatus opl_broadcast_getStartTime (OplBroadcast_t *hBroadcast, HUINT32 *pulBufTime)
{
	xOplBroadcast_t			*hBcast = xbroadcast(hBroadcast);

	if (0 != hBcast->nTsrStarttime)
	{
		if ( (HLIB_STD_GetSystemTick() - hBcast->nTsrStarttimeTick) >= 1000 )
		{
			hBcast->nTsrStarttime ++;
			hBcast->nTsrStarttimeTick	= HLIB_STD_GetSystemTick();
		}
	}
	else
	{
		HUINT32 				 ulTime = 0;
			APK_MEDIA_PLAY_GetTsrStartTime (hBcast->ulViewId, &ulTime);
			hBcast->nTsrStarttime		= (HINT32)ulTime;
			hBcast->nTsrStarttimeTick	= HLIB_STD_GetSystemTick();
	}

	*pulBufTime = hBcast->nTsrStarttime;

	if (1 < hBcast->nTsrStarttime)
		*pulBufTime -= 1;

	return BPL_STATUS_OK;
}

static BPLStatus opl_broadcast_enableRecNotifier (xOplBroadcast_t *hBcast, DxService_t *pstService)
{
	HERROR								 hErr;

	hErr = APK_MEDIA_RECORD_RegisterNotifier (pstService->uid, (OxMediaRec_Notifier_t)opl_broadcast_notifyRecFunc, NULL);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("[%s:%d] Error\n", __FUNCTION__, __LINE__);
		return BPL_STATUS_ERROR;
	}

	return BPL_STATUS_OK;
}

static BPLStatus opl_broadcast_disableRecNotifier (xOplBroadcast_t *hBcast)
{
	DxService_t						 stService;
	HERROR								 hErr;

	hErr = opl_broadcast_getCurrentMasterService (hBcast->ulViewId, &stService);
	if (BPL_STATUS_OK != hErr)
	{
		HxLOG_Error ("[%s:%d] Error\n", __FUNCTION__, __LINE__);
		return BPL_STATUS_ERROR;
	}
	hErr = APK_MEDIA_RECORD_UnregisterNotifier(stService.uid, (OxMediaRec_Notifier_t)opl_broadcast_notifyRecFunc, NULL);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("[%s:%d] Error\n", __FUNCTION__, __LINE__);
		return BPL_STATUS_ERROR;
	}

	return BPL_STATUS_OK;
}


static BPLStatus opl_broadcast_getCurrentProgramme (xOplBroadcast_t *hBroadcast, OPL_ProgrammeHandle *programme)
{
	xOplBroadcast_t	*self = hBroadcast;
	int				i =0, isUndefined =0, curIndex =0, tempIndex =0;
	unsigned int		ulStartTime =0, ulDuration =0, ulCurTime = 0, ulTempStartTime = 0, ulCurStartTime =0;
	HBOOL               	bFound = FALSE;

	if (self == NULL)
		return BPL_STATUS_ERROR;

	if (self->nPFLen < 1)
	{
		DxService_t service;
		if (opl_broadcast_getCurrentMasterService(self->ulViewId, &service) == BPL_STATUS_OK)
		{
			OPL_METADATASEARCH_SearchPF(NULL, service.uid, (OPL_SearchPfMonitor_t)opl_broadcast_PfMonitor, (void *)self);
			if (self->nPFLen == 0)
				return BPL_STATUS_ERROR;
		}
		else
		{
			return BPL_STATUS_ERROR;
		}
	}

	ulCurTime = HLIB_STD_GetSystemTime();

	for (i = 0; i < self->nPFLen ; i++)
	{
		ulStartTime = OPL_Programme_GetStartTime( self->ahPF[i] );
		ulDuration = OPL_Programme_GetDuration( self->ahPF[i] );

		if( ulStartTime > ulCurTime ) /* skil futuer event.*/
			continue;

		if( (ulStartTime <= ulCurTime )&& (ulDuration == 0 || ulDuration == 0xffffff) )
		{/*마지막 미정이 현재 이벤트임 */
			isUndefined = 1;
			tempIndex = i;
			ulTempStartTime = ulStartTime;
			continue;
		}
		else if( (ulStartTime <= ulCurTime )&& ( (ulStartTime + ulDuration) > ulCurTime ) )
		{/* current event */
			curIndex = i;
			ulCurStartTime = ulStartTime;
			bFound = TRUE;
		}
		else { /* do nothing */ }
	}

	if( ( isUndefined == 1 ) && ( ulTempStartTime >= ulCurStartTime ))
	{/* 마지막 미정인 것인 current event */
		curIndex = tempIndex;
		bFound = TRUE;
	}

	if(bFound == FALSE)
	{
		return BPL_STATUS_ERROR;
	}

	if( curIndex < self->nPFLen )
	{
		*programme = OPL_Programme_Clone(self->ahPF[curIndex]);
	}

	return BPL_STATUS_OK;
}
#if defined(CONFIG_OP_JAPAN)

static void opl_broadcast_notifyDrmFunc(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{

	HxList_t					*pstListItem = NULL;
	HxJSON_t					jsParam = (HxJSON_t)ulParam1;
	HUINT32					ulSessionHandle = 0, ulLatestSessionHandle = 0;
	HERROR					hErr = ERR_FAIL;
	eOplBroadcastEventType		eOipfEvent = -1;
	HINT32					ulViewId;
	APKE_CasBcCtrl_Event_e		eLastReceivedEvent;

	APKS_CASBC_CasStatus_t	stStatus;
	HxLOG_Debug("+ [%s:%d]\n", __FUNCTION__, __LINE__);

	hErr = APK_CAS_BC_ParseDrmArgs(jsParam, &ulViewId, &ulSessionHandle, &stStatus);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("APK_CAS_BC_ParseDrmArgs returned err\n");
		return;
	}

	APK_MEDIA_PLAY_GetSessionId(ulViewId, &ulLatestSessionHandle);
	if(ulLatestSessionHandle > ulSessionHandle)
	{
		HxLOG_Print(" cas event !!! differrent sessionHandle\n ulViewId %d ulLatestSessionHandle %d, ulSessionHandle %d\n",ulViewId,ulLatestSessionHandle ,ulSessionHandle);
		return;
	}

	eLastReceivedEvent = stStatus.eLastReceivedEvent;
	// 각 Broadcast Object에 돌려야 하는 것들
	for (pstListItem = s_pstBcastObjList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		xOplBroadcast_t		*hBcast = (xOplBroadcast_t *)HLIB_LIST_Data (pstListItem);
		if ((NULL != hBcast) && (ulViewId == hBcast->ulViewId))
		{
			HxLOG_Print("ulViewId %d, ulViewId %d	[%s:%d]\n",ulViewId, hBcast->ulViewId,__FUNCTION__, __LINE__);
			switch (eLastReceivedEvent)
			{
			case eCasBcEvent_CasFail:
			case eCasBcEvent_CasOK:
				if (NULL != hBcast->casEventListener)
				{
					switch(stStatus.ulError)
					{
						case 0x8000E208:
						case 0x8001E208:
						case 0x8002E208:
						case 0x8003E208:
							eOipfEvent = OPLBROADCAST_EVENT_TYPE_CAS_BC_BS_CONTROL_START;
							HxLOG_Print("call  _broadcast_onCasBcEvent [%s:%d]\n",__FUNCTION__, __LINE__);
							(*(hBcast->casEventListener))(hBcast->hVbcastHandle, eOipfEvent);			// _broadcast_onLiveChange
							break;
						default:
							eOipfEvent = OPLBROADCAST_EVENT_TYPE_DRM_RIGHTS_ERROR_EVENT;
							HxLOG_Print("call  _broadcast_onCasBcEvent [%s:%d]\n",__FUNCTION__, __LINE__);
							(*(hBcast->casEventListener))(hBcast->hVbcastHandle, eOipfEvent);			// _broadcast_onLiveChange
							eOipfEvent = OPLBROADCAST_EVENT_TYPE_CAS_BC_DRM_ERROR_NOTIFY;
							HxLOG_Print("call  _broadcast_onCasBcEvent [%s:%d]\n",__FUNCTION__, __LINE__);
							(*(hBcast->casEventListener))(hBcast->hVbcastHandle, eOipfEvent);			// _broadcast_onLiveChange
							break;
					}
				}
				break;
			default:
				break;
			}

		}
	}

	return;
}


static void opl_broadcast_notifyBsAutoMsgFunc(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	HxJSON_t					jsParam = (HxJSON_t)ulParam1;
	HUINT32					ulSessionHandle = 0, ulLatestSessionHandle = 0;
	HERROR					hErr = ERR_FAIL;
	HxList_t						*pstListItem = NULL;
	eOplBroadcastEventType			eOipfEvent = -1;

	HUINT32	ulViewId = 0;
	APKE_CasBcCtrl_Event_e		eLastReceivedEvent = ulParam2;

	APKS_CASBC_BSAutoMsgInfo_t	stBsMsg;
	HxLOG_Debug("+ [%s:%d]\n", __FUNCTION__, __LINE__);
	hErr = APK_CAS_BC_ParseBsAutoMsgArgs(jsParam, &ulViewId, &ulSessionHandle, &stBsMsg);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("APK_CAS_BC_ParseBsAutoMsgArgs returned err\n");
		return;
	}

	APK_MEDIA_PLAY_GetSessionId(ulViewId, &ulLatestSessionHandle);
	if(ulLatestSessionHandle != ulSessionHandle)
	{
		HxLOG_Print("bs auto msg !!! differrent sessionHandle\n ulViewId %d ulLatestSessionHandle %d, ulSessionHandle %d\n",ulViewId,ulLatestSessionHandle ,ulSessionHandle);
		return;
	}
	eLastReceivedEvent = stBsMsg.eLastEvent;

	// 각 Broadcast Object에 돌려야 하는 것들
	for (pstListItem = s_pstBcastObjList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		xOplBroadcast_t		*hBcast = (xOplBroadcast_t *)HLIB_LIST_Data (pstListItem);
		if ((NULL != hBcast) && (ulViewId == hBcast->ulViewId))
		{
			HxLOG_Print("ulViewId %d, ulViewId %d	[%s:%d]\n",ulViewId, hBcast->ulViewId,__FUNCTION__, __LINE__);
			switch (eLastReceivedEvent)
			{
			case eCasBcEvent_ShowBsAutoMsg:
			case eCasBcEvent_ClearBsAutoMsg:
				if (NULL != hBcast->casEventListener)
				{
					HxLOG_Print("autoDispMsg Event Type %d  [%s:%d]\n",eLastReceivedEvent,__FUNCTION__, __LINE__);
					eOipfEvent = OPLBROADCAST_EVENT_TYPE_CAS_BC_EMM_AUTO_DISP_MSG_NOTIFY;
					HxLOG_Print("call  _broadcast_onCasBcEvent [%s:%d]\n",__FUNCTION__, __LINE__);
					(*(hBcast->casEventListener))(hBcast->hVbcastHandle, eOipfEvent);			// _broadcast_onLiveChange
				}
				break;
			default:
				break;
			}

		}
	}

	return;
}


static void opl_broadcast_notifyCaLinkMsgFunc(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	HxJSON_t					jsParam = (HxJSON_t)ulParam1;
	HUINT32					ulSessionHandle = 0, ulLatestSessionHandle = 0;
	HERROR					hErr = ERR_FAIL;

	HxList_t						*pstListItem = NULL;
	eOplBroadcastEventType			eOipfEvent = -1;
	HUINT32 	ulViewId = 0;
	APKE_CasBcCtrl_Event_e		eLastReceivedEvent = ulParam2;
	APKS_CASBC_CaLinkMsg_t	stCaMsg;
	HxLOG_Debug("+ [%s:%d]\n", __FUNCTION__, __LINE__);

	hErr = APK_CAS_BC_ParseCaLinkMsgArgs(jsParam, &ulViewId, &ulSessionHandle, &stCaMsg);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("APK_CAS_BC_ParseCaLinkMsgArgs returned err\n");
		return;
	}
	APK_MEDIA_PLAY_GetSessionId(ulViewId, &ulLatestSessionHandle);
	if(ulLatestSessionHandle != ulSessionHandle)
	{
		HxLOG_Print("ca link msg !!! differrent sessionHandle\n ulViewId %d ulLatestSessionHandle %d, ulSessionHandle %d\n",ulViewId,ulLatestSessionHandle ,ulSessionHandle);
		return;
	}
	eLastReceivedEvent = stCaMsg.eLastEvent;

	// 각 Broadcast Object에 돌려야 하는 것들
	for (pstListItem = s_pstBcastObjList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		xOplBroadcast_t		*hBcast = (xOplBroadcast_t *)HLIB_LIST_Data (pstListItem);
		if ((NULL != hBcast) && (ulViewId == hBcast->ulViewId))
		{
			HxLOG_Print("ulViewId %d, ulViewId %d	[%s:%d]\n",ulViewId, hBcast->ulViewId,__FUNCTION__, __LINE__);
			switch (eLastReceivedEvent)
			{
			case eCasBcEvent_ShowCaLinkMsg:
				if (NULL != hBcast->casEventListener)
				{
					HxLOG_Print("caLinkMsg Event Type %d  [%s:%d]\n",eLastReceivedEvent,__FUNCTION__, __LINE__);
					eOipfEvent = OPLBROADCAST_EVENT_TYPE_CAS_BC_CA_LINK_MSG_NOTIFY;
					HxLOG_Print("call  _broadcast_onCasBcEvent [%s:%d]\n",__FUNCTION__, __LINE__);
					(*(hBcast->casEventListener))(hBcast->hVbcastHandle, eOipfEvent);			// _broadcast_onLiveChange
				}
				break;
			default:
				break;
			}

		}
	}

	return;
}

#endif
#if defined(CONFIG_OP_FREESAT)
STATIC BPLStatus opl_broadcast_getCurrentProgramme (OplBroadcast_t *hBroadcast, OPL_ProgrammeHandle *programme)
{
	xOplBroadcast_t	*self = xbroadcast(hBroadcast);
	int i;

	if (self == NULL)
		return BPL_STATUS_ERROR;

	if (self->nPFLen < 2)
	{
		DxService_t service;

		if (opl_broadcast_getCurrentMasterService(self->ulViewId, &service) == BPL_STATUS_OK)
		{
			OPL_METADATASEARCH_SearchPF(NULL, service.uid, (OPL_SearchPfMonitor_t)opl_broadcast_PfMonitor, (void *)self);
			if (self->nPFLen == 0)
				return BPL_STATUS_ERROR;
		}
	}

	if(self->nPFLen > 0)
	{
		for(i = 0;i<self->nPFLen;i++)
		{
			HULONG		sysTime,startTime,duration;

			if (ERR_OK != VK_CLOCK_GetTime((unsigned long *)&sysTime))
			{
				*programme = OPL_Programme_Clone(self->ahPF[0]);
				HxLOG_Error("[%s:%d] Fail to Get the Current Time !\n", __FUNCTION__, __LINE__);
				break;
			}
			else
			{
				startTime = OPL_Programme_GetStartTime(self->ahPF[i]);
				duration = OPL_Programme_GetDuration(self->ahPF[i]);
				if(startTime  < sysTime && (startTime+duration)>= sysTime)
				{
					*programme = OPL_Programme_Clone(self->ahPF[i]);
					HxLOG_Error("\t%s() PF[%d] %s\n", __FUNCTION__, i, OPL_Programme_GetName(self->ahPF[i]));
					break;
				}
			}

		}
	}

	return BPL_STATUS_OK;
}
#endif
static eOplAVComponentSubType opl_broadcast_convertAudioComponentTypeToOplSubType (OxMediaPlay_AudioMode_e eAudioComp)
{
	switch (eAudioComp)
	{
	case eOxMP_AUDMODE_SINGLEMONO:
		return OPLAVCOMPONENT_SUBTYPE_AUDIO_SINGLEMONO;

	case eOxMP_AUDMODE_DUALMONO:
		return OPLAVCOMPONENT_SUBTYPE_AUDIO_DUALMONO;

	case eOxMP_AUDMODE_STEREO:
		return OPLAVCOMPONENT_SUBTYPE_AUDIO_STEREO;

	case eOxMP_AUDMODE_MULTILINGUAL:
		return OPLAVCOMPONENT_SUBTYPE_AUDIO_MULTILINGUAL;

	case eOxMP_AUDMODE_SURROUND:
		return OPLAVCOMPONENT_SUBTYPE_AUDIO_SURROUND;

	case eOxMP_AUDMODE_DOLBY:
		return OPLAVCOMPONENT_SUBTYPE_AUDIO_DOLBY;

	default:
		break;
	}

	return OPLAVCOMPONENT_SUBTYPE_AUDIO_STEREO;
}

static HCHAR *opl_broadcast_convertAudioCodecToOplEncoding (DxAudioCodec_e eCodec)
{
	switch (eCodec)
	{
	case eDxAUDIO_CODEC_MPEG:
		return "MPEG1_L2";

	case eDxAUDIO_CODEC_MP3:
		return "MPEG1_L3";

	case eDxAUDIO_CODEC_DOLBY_AC3:
		return "AC3";

	case eDxAUDIO_CODEC_DOLBY_AC3P:
		return "E-AC3";

	case eDxAUDIO_CODEC_PCM:
		return "WAV";

	case eDxAUDIO_CODEC_AAC:
	case eDxAUDIO_CODEC_AAC_LOAS:
	case eDxAUDIO_CODEC_AAC_PLUS:
	case eDxAUDIO_CODEC_AAC_PLUS_ADTS:
		return "HEAAC";

	case eDxAUDIO_CODEC_DTS:
	case eDxAUDIO_CODEC_DTS_HD:
		return "DTS";

	default:
		return "UNKNOWN";
	}
}

static HUINT32	opl_broadcast_convertAudioTypeToAudioChannels (HINT32 nComponentType)
{
	switch( nComponentType )
	{
		// 300 468 Table 26: Stream_content and component_type
		case 0x01:		// single mono
			return 1;

		case 0x02:		// dual mono
		case 0x03:		// stereo
			return 2;


		case 0x04:
		case 0x05:
			return 3;


		case 0x06:
		case 0x07:
			return 4;


		case 0x08:
		case 0x09:
			return 5;


		default:
			return 0;

	}
}

static HCHAR *opl_broadcast_convertVideoTypeToOplEncoding (HINT32 nComponentType)
{
	switch (nComponentType)
	{
	case 0x01:
	case 0x02:
	case 0x03:
	case 0x04:
		return "MPEG2_SD_25";
	case 0x05:
	case 0x06:
	case 0x07:
	case 0x08:
		return "MPEG2_SD_30";
	case 0x09:
	case 0x0A:
	case 0x0B:
	case 0x0C:
		return "MPEG2_HD_25";
	case 0x0D:
	case 0x0E:
	case 0x0F:
	case 0x10:
		return "MPEG2_HD_30";

	default:
		return "UNKNOWN_VIDEO";
	}
}

static BPLStatus opl_broadcast_getDrmRightsStatusFromJson(OplBroadcastDrmRightsStatus_t *pstStatus, HUINT32 error_status)
{
	if( pstStatus == NULL )
		return BPL_STATUS_ERROR;

	eOplBroadcastDrmRightsError error;
	switch ( error_status )
	{
	case eOxMP_DRMERROR_ValidLicense:
		error = OPLBROADCAST_DRM_RIGHTS_VALID_LICENSE;
		break;
	case eOxMP_DRMERROR_InvalidLicense:
		error = OPLBROADCAST_DRM_RIGHTS_INVALID_LICENSE;
		break;
	case eOxMP_DRMERROR_NoLicense:
		error = OPLBROADCAST_DRM_RIGHTS_NO_LICENSE;
	default:
		return BPL_STATUS_ERROR;
	}
	pstStatus->errorState = (HUINT32)error;
	pstStatus->contentId = NULL;
	pstStatus->drmSystemId = NULL;
	pstStatus->rightsIssuerUrl = NULL;
	return BPL_STATUS_OK;
}

static void UpdateInitialState(xOplBroadcast_t* hBcast)
{
	APK_MEDIA_PLAY_GetViewState(hBcast->ulViewId, &(hBcast->eViewState));
	APK_MEDIA_PLAY_GetLockState(hBcast->ulViewId, &(hBcast->eLockState));
	if ((eOxMP_LiveViewState_NONE == hBcast->eViewState) || (eOxMP_LiveLockState_NONE == hBcast->eLockState))
	{
		hBcast->eSessionState = OPLBROADCAST_SESSION_STATE_STARTED;
		hBcast->ePlayState		= OPLBROADCAST_PLAY_STATE_CONNECTING;
	}
	else if ((eOxMP_LiveViewState_OK == hBcast->eViewState) && (eOxMP_LiveLockState_OK == hBcast->eLockState))
	{
		hBcast->eSessionState = OPLBROADCAST_SESSION_STATE_STARTED;
		hBcast->ePlayState		= OPLBROADCAST_PLAY_STATE_PRESENTING;
	}
}

/******************************************************************
	Interfaces
******************************************************************/
OplBroadcast_t	*OPL_Broadcast_Create (OPL_HANDLE hVbcastObj, int aResourceId)
{
	static HBOOL					 s_bInited = FALSE;
	xOplBroadcast_t					*hBcast = HLIB_STD_MemAlloc(sizeof(*hBcast));

#if defined(CONFIG_OP_JAPAN)
	APK_HANDLE						hCasuiBcHandle;
#endif
	if (TRUE != s_bInited)
	{
		opl_broadcast_initialize();
		s_bInited = TRUE;
	}

	if ( hBcast == NULL )
	{
		HxLOG_Error("Insufficient system memory.\n");
		return NULL;
	}

	hBcast->hVbcastHandle = hVbcastObj;

	hBcast->liveEventListener		= NULL;
	hBcast->tsrEventListener		= NULL;
	hBcast->casEventListener		= NULL;
	hBcast->recEventListener		= NULL;
	hBcast->mailEventListener		= NULL;
	hBcast->bmlEventListener		= NULL;
	hBcast->mhegEventListener		= NULL;

	hBcast->recorder	= NULL;
	hBcast->ePlayState	= OPLBROADCAST_PLAY_STATE_UNREALIZED;
	hBcast->eSessionState = OPLBROADCAST_SESSION_STATE_INITIALIZED;
	HxSTD_MemSet(&(hBcast->stParentalRatingStatus), 0x00, sizeof( OplBroadcastParentalRatingStatus_t));

    hBcast->bFullScreen = TRUE;
	hBcast->nLastWindowPositionX = 0;
	hBcast->nLastWindowPositionY = 0;
	hBcast->nLastWindowPositionWidth = 0;
	hBcast->nLastWindowPositionHeight = 0;

	/*clean TSR value*/
	hBcast->nTsrPlaytime		=	0;
	hBcast->nTsrPlaytimeTick	=	0;
	hBcast->nTsrStarttime		=	0;
	hBcast->nTsrStarttimeTick	=	0;
	hBcast->bIsTsrPlaying       =   0;

#if defined(CONFIG_OP_JAPAN)
	hCasuiBcHandle = APK_CAS_BC_GetHandle (eCasBcCtrlType_Drm, hBcast->ulViewId);
	APK_CAS_BC_RegisterNotifier (hCasuiBcHandle, opl_broadcast_notifyDrmFunc);
	hCasuiBcHandle = APK_CAS_BC_GetHandle (eCasBcCtrlType_BsAutoMsg, hBcast->ulViewId);
	APK_CAS_BC_RegisterNotifier (hCasuiBcHandle, opl_broadcast_notifyBsAutoMsgFunc);
	hCasuiBcHandle = APK_CAS_BC_GetHandle (eCasBcCtrlType_CaLinkMsg, hBcast->ulViewId);
	APK_CAS_BC_RegisterNotifier (hCasuiBcHandle, opl_broadcast_notifyCaLinkMsgFunc);

	(void)opl_broadcast_notifyCasMessageFuncASync;
	(void)opl_broadcast_notifyCasEventFuncASync;
#else
	if (NULL == s_pstBcastObjList)
	{
		APK_CAS_MMI_RegisterSessionNotifier(eDxCAS_SESSIONTYPE_BANNER, opl_broadcast_notifyCasMessageFunc);
	}
#endif

	s_pstBcastObjList = HLIB_LIST_Append (s_pstBcastObjList, (void *)hBcast);

	{
		HERROR	hErr;

		hBcast->ulViewId = aResourceId;

		hErr = APK_MEDIA_PLAY_RegisterNotifier (hBcast->ulViewId, opl_broadcast_notifyFunc, opl_broadcast_eventFilter, NULL);
		if( ERR_OK != hErr )
			HxLOG_Error("[%s:%d] APK_MEDIA_PLAY_RegisterNotifier Error: \n", __FUNCTION__, __LINE__);
	}

	// Mail
	APK_META_MAIL_RegisterNotifier( (APK_META_MAIL_Notifier_t)opl_broadcast_notifyMailMessage, NULL );

	hBcast->hQueryHandle = OPL_METADATASEARCH_New(eOPL_SEARCH_TARGET_SCHEDULED);
	hBcast->nPFLen  = 0;

#if defined(CONFIG_OP_JAPAN)
//	APK_EXE_BML_RegisterNotifier(opl_broadcast_bmlVod, hBcast);
#endif
	UpdateInitialState(hBcast);

	HxLOG_Print("Creating the broadcast player instance is successful.\n");

	HAPI_ConnectSignal("signal:MhegEventNotify",      NULL, opl_broadcase_NotifyMhegEvent);

	return (OplBroadcast_t *)hBcast;
}

void OPL_Broadcast_Destroy(OplBroadcast_t *hBroadcast)
{
	xOplBroadcast_t *hBcast = xbroadcast(hBroadcast);
	if ( hBcast == NULL )
	{
		HxLOG_Error("The handle of broadcast player is NULL.\n");
		return;
	}

	if ( hBcast->recorder )
	{
		OPL_Recorder_Destroy(hBcast->recorder);
		hBcast->recorder = NULL;
	}

	s_pstBcastObjList = HLIB_LIST_Remove (s_pstBcastObjList, (void *)hBcast);

	if (hBcast->hQueryHandle)
		OPL_METADATASEARCH_Delete(hBcast->hQueryHandle);

	while (hBcast->nPFLen--)
	{
		OPL_Programme_Delete(hBcast->ahPF[hBcast->nPFLen]);
	}

	OPL_Broadcast_SetFullScreen(hBcast, TRUE);

	HLIB_STD_MemFree(hBcast);
	hBcast = NULL;

	HxLOG_Print("Destroying the broadcast player instance is successful.\n");
}

int OPL_Broadcast_GetResourceId(OplBroadcast_t *hBroadcast)
{
	xOplBroadcast_t *hBcast = xbroadcast(hBroadcast);
	if ( hBcast == NULL )
	{
		HxLOG_Error("The handle of broadcast player is NULL.\n");
		return 0;
	}
	return (int)hBcast->ulViewId;
}

void OPL_Broadcast_SetLiveEventListener(OplBroadcast_t *hBroadcast, OplBroadcastListener_t listener)
{
	xOplBroadcast_t *hBcast = xbroadcast(hBroadcast);
	/* TODO: remove this code, we need listener clearing routine
	if ( hBcast == NULL )
	{
		HxLOG_Error("The handle of broadcast player is NULL.\n");
		return;
	}
	*/

	hBcast->liveEventListener = listener;
}



void OPL_Broadcast_SetTsrEventListener(OplBroadcast_t *hBroadcast, OplBroadcastListener_t listener)
{
	xOplBroadcast_t *hBcast = xbroadcast(hBroadcast);
	/* TODO: remove this code, we need listener clearing routine
	if ( hBcast == NULL )
	{
		HxLOG_Error("The handle of broadcast player is NULL.\n");
		return;
	}
	*/
	hBcast->tsrEventListener = listener;
}

void OPL_Broadcast_SetCasEventListener(OplBroadcast_t *hBroadcast, OplBroadcastListener_t listener)
{
	xOplBroadcast_t *hBcast = xbroadcast(hBroadcast);
	/* TODO: remove this code, we need listener clearing routine
	if ( hBcast == NULL )
	{
		HxLOG_Error("The handle of broadcast player is NULL.\n");
		return;
	}
	*/

	hBcast->casEventListener = listener;
}

void OPL_Broadcast_SetRecEventListener(OplBroadcast_t *hBroadcast, OplBroadcastListener_t listener)
{
	xOplBroadcast_t *hBcast = xbroadcast(hBroadcast);
	/* TODO: remove this code, we need listener clearing routine
	if ( hBcast == NULL )
	{
		HxLOG_Error("The handle of broadcast player is NULL.\n");
		return;
	}
	*/

	hBcast->recEventListener = listener;
}

void OPL_Broadcast_SetMailEventListener( OplBroadcast_t *hBroadcast , OplBroadcastListener_t listener)
{
	xOplBroadcast_t *hBcast = xbroadcast(hBroadcast);
	/* TODO: remove this code, we need listener clearing routine
	if ( hBcast == NULL )
	{
		HxLOG_Error("The handle of broadcast player is NULL.\n");
		return;
	}
	*/

	hBcast->mailEventListener = listener;
}

void OPL_Broadcast_SetBmlEventListener( OplBroadcast_t *hBroadcast , OplBroadcastListener_t listener)
{
	xOplBroadcast_t *hBcast = xbroadcast(hBroadcast);
	/* TODO: remove this code, we need listener clearing routine
	if ( hBcast == NULL )
	{
		HxLOG_Error("The handle of broadcast player is NULL.\n");
		return;
	}
	*/

	hBcast->bmlEventListener = listener;
}

void OPL_Broadcast_SetMhegEventListener(OplBroadcast_t *hBroadcast, OplBroadcastListener_t listener)
{
	xOplBroadcast_t *hBcast = xbroadcast(hBroadcast);
	/* TODO: remove this code, we need listener clearing routine
	if ( hBcast == NULL )
	{
		HxLOG_Error("The handle of broadcast player is NULL.\n");
		return;
	}
	*/

	hBcast->mhegEventListener = listener;
}

BPLStatus OPL_Broadcast_GetFullScreen(OplBroadcast_t *hBroadcast, BPL_BOOL *pbFullScreen)
{
	xOplBroadcast_t *hBcast = xbroadcast(hBroadcast);
	if ( hBcast == NULL )
	{
		HxLOG_Error("The handle of broadcast player is NULL.\n");
		return BPL_STATUS_ERROR;
	}

	if ( pbFullScreen == NULL )
	{
		HxLOG_Error("The buffer is invalid.\n");
		return BPL_STATUS_ERROR;
	}

	*pbFullScreen = hBcast->bFullScreen;

	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_SetFullScreen(OplBroadcast_t *hBroadcast, BPL_BOOL bFullScreen)
{
	BPLStatus hErr = BPL_STATUS_ERROR;
	xOplBroadcast_t *hBcast = xbroadcast(hBroadcast);
	if ( hBcast == NULL )
	{
		HxLOG_Error("The handle of broadcast player is NULL.\n");
		return hErr;
	}

	if ( bFullScreen == TRUE )
	{
		// Width/Height 0 may trigger full screen from OCTO size
		hErr = APK_MEDIA_PLAY_SetVideoSize(hBcast->ulViewId, 0, 0, 0, 0);
		hBcast->bFullScreen = bFullScreen;
	}
	else
	{
		hErr = APK_MEDIA_PLAY_SetVideoSize(hBcast->ulViewId,
										  hBcast->nLastWindowPositionX,
										  hBcast->nLastWindowPositionY,
										  hBcast->nLastWindowPositionWidth,
										  hBcast->nLastWindowPositionHeight );
	}

	if( hErr == BPL_STATUS_OK )
	{
		hBcast->bFullScreen = bFullScreen;
	}
	return hErr;
}

BPLStatus OPL_Broadcast_PrevChannel(OplBroadcast_t *hBroadcast)
{
	HUINT32			 ulSvcUid;
	xOplBroadcast_t *hBcast = xbroadcast(hBroadcast);

	if (NULL == hBcast)
	{
		HxLOG_Error("The handle of broadcast player is NULL.\n");
		return BPL_STATUS_ERROR;
	}
#if defined(CONFIG_OP_JAPAN)
	ulSvcUid = opl_broadcast_getAvailableSvcUidByCurrOffset(hBcast->ulViewId, -1);
#else
	ulSvcUid = opl_broadcast_getSvcUidByCurrOffset (hBcast->ulViewId, -1);
#endif
	if (0 == ulSvcUid)
	{
		HxLOG_Error("opl_broadcast_getChannelByCurrOffset Error.\n");
		return BPL_STATUS_ERROR;
	}

	if (BPL_STATUS_OK == opl_broadcast_BindChannel(hBcast, ulSvcUid, 0))
	{
		HUINT32 ulMainViewId = 0;
		if(APK_MEDIA_PLAY_GetMainViewId(&ulMainViewId) == ERR_OK)
		{
			if(hBcast->ulViewId == ulMainViewId)
			{
				opl_broadcast_memorizeCurrentChannel (ulSvcUid);
			}
		}
		return BPL_STATUS_OK;
	}

	return BPL_STATUS_ERROR;
}

BPLStatus OPL_Broadcast_NextChannel(OplBroadcast_t *hBroadcast)
{
	HUINT32			 ulSvcUid;
	xOplBroadcast_t *hBcast = xbroadcast(hBroadcast);

	if (NULL == hBcast)
	{
		HxLOG_Error("The handle of broadcast player is NULL.\n");
		return BPL_STATUS_ERROR;
	}
#if defined(CONFIG_OP_JAPAN)
	ulSvcUid = opl_broadcast_getAvailableSvcUidByCurrOffset(hBcast->ulViewId, 1);
#else
	ulSvcUid = opl_broadcast_getSvcUidByCurrOffset (hBcast->ulViewId, 1);
#endif
	if (0 == ulSvcUid)
	{
		HxLOG_Error("opl_broadcast_getChannelByCurrOffset Error.\n");
		return BPL_STATUS_ERROR;
	}

	if (BPL_STATUS_OK == opl_broadcast_BindChannel(hBcast, ulSvcUid, 0))
	{
		HUINT32 ulMainViewId = 0;
		if(APK_MEDIA_PLAY_GetMainViewId(&ulMainViewId) == ERR_OK)
		{
			if(hBcast->ulViewId == ulMainViewId)
			{
				opl_broadcast_memorizeCurrentChannel (ulSvcUid);
			}
		}
		return BPL_STATUS_OK;
	}

	return BPL_STATUS_ERROR;
}

BPLStatus OPL_Broadcast_BindToCurrentChannel(OplBroadcast_t *hBroadcast)
{
	xOplBroadcast_t					*hBcast = xbroadcast(hBroadcast);
	DxService_t					 stService;
	BPLStatus						 hErr;

	if (NULL == hBcast)
	{
		HxLOG_Error("hBcast NULL\n");
		return BPL_STATUS_ERROR;
	}

	OxMediaPlay_MediaType_e eType;
	APK_MEDIA_PLAY_GetPlayType(hBcast->ulViewId, &eType);

	switch (eType)
	{
	case eOxMP_MEDIATYPE_LIVE:
        /* this case is to avoid when js code call vb.stop() and as soon as vb.bindtoCurrentChannel() */
		if ( hBcast->ePlayState == OPLBROADCAST_PLAY_STATE_CONNECTING)
		{
			HxLOG_Error("OPL_Broadcast_BindToCurrentChannel Error invalid playState (hBcast->ePlayState:0x%X)\n", hBcast->ePlayState);

			// DTV_Labs 0.8 Error fix
			hBcast->ePlayState = OPLBROADCAST_PLAY_STATE_CONNECTING;
			    (*(hBcast->liveEventListener))(hBcast->hVbcastHandle,  OPLBROADCAST_EVENT_TYPE_PLAYSTATE_CHANGE);
			break;
		}
		else if (hBcast->ePlayState == OPLBROADCAST_PLAY_STATE_PRESENTING )
		{
			HxLOG_Error("OPL_Broadcast_BindToCurrentChannel Error invalid playState (hBcast->ePlayState:0x%X)\n", hBcast->ePlayState);
			// DTV_Labs 0.8 Error fix
			hBcast->ePlayState = OPLBROADCAST_PLAY_STATE_CONNECTING;
			(*(hBcast->liveEventListener))(hBcast->hVbcastHandle,  OPLBROADCAST_EVENT_TYPE_PLAYSTATE_CHANGE);
			hBcast->ePlayState = OPLBROADCAST_PLAY_STATE_PRESENTING;
			(*(hBcast->liveEventListener))(hBcast->hVbcastHandle,  OPLBROADCAST_EVENT_TYPE_PLAYSTATE_CHANGE);
			break;
		}
	case eOxMP_MEDIATYPE_NONE:
	default:
		hErr = opl_broadcast_getLastService (hBcast->ulViewId, &stService);
		if (BPL_STATUS_OK != hErr)
		{
			HxLOG_Error("opl_broadcast_getCurrentMasterService Error\n");
			return BPL_STATUS_ERROR;
		}

		/* The last channel is invalid when the conflict by scheduled record */
		HBOOL tunable = FALSE;
		hErr = opl_broadcast_CheckChannelChangable(hBroadcast, (OPL_Channel_t)&stService, &tunable);
		if ( hErr == ERR_OK && tunable )
		{
			hErr = opl_broadcast_BindChannel (hBcast, stService.uid, 0);
			if (BPL_STATUS_OK != hErr)
			{
				HxLOG_Error("opl_broadcast_BindChannel Error\n");
				return BPL_STATUS_ERROR;
			}
		}
		else
		{
			OPL_Channel_t channel = opl_broadcast_getChangbleChannel_inTheRecording(hBroadcast);
			if ( !channel )
			{
				HxLOG_Error("opl_broadcast_getChangbleChannel_inTheRecording: there is no channel in the recording\n");
				break;
			}

			/* start LIVE */
			{
				HUINT32  ulSessionId = 0;
				hErr = opl_broadcast_startLive(hBcast->ulViewId, ((DxService_t *)channel)->uid, &ulSessionId);
				if (ERR_OK != hErr)
				{
					HxLOG_Error ("[%s:%d] opl_broadcast_startLive err: ViewId=%d\n", __FUNCTION__, __LINE__, hBcast->ulViewId);
					return BPL_STATUS_ERROR;
				}
			}

		}
		break;
	}

	return BPL_STATUS_OK;
}
BPLStatus OPL_Broadcast_SetChannelHandle(OPL_Channel_t channel)
{
	s_chHandleOfSetchannel = (void *)channel;

	return BPL_STATUS_OK;
}
BPLStatus OPL_Broadcast_GetChannelHandle(void **channel)
{
	*channel = s_chHandleOfSetchannel;

	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_SetChannelByTripleID(OplBroadcast_t *hBroadcast, unsigned short usServiceID, unsigned short usTSID, unsigned short usNetID)
{
	xOplBroadcast_t *hBcast;
	OPL_Channel_t	 hChannel;

	hBcast = xbroadcast(hBroadcast);
	if ( hBcast == NULL )
	{
		HxLOG_Error("The handle of broadcast player is NULL.\n");
		return BPL_STATUS_ERROR;
	}

	hChannel = OPL_Channel_FindChannelByTriplet(OPL_Channel_GetManager(), (int)usNetID, (int)usTSID, (int)usServiceID);
	if (hChannel == NULL)
	{
		HxLOG_Error("Cannot set channel by triplet\n");
		return BPL_STATUS_ERROR;
	}

	return OPL_Broadcast_SetChannelByChannelObject(hBroadcast, hChannel);
}

BPLStatus OPL_Broadcast_SetChannelByChannelObject(OplBroadcast_t *hBroadcast, OPL_Channel_t channel)
{
	BPLStatus		status;
	xOplBroadcast_t *hBcast;
	int 			uid;

	hBcast = xbroadcast(hBroadcast);
	if ( hBcast == NULL )
	{
		HxLOG_Error("The handle of broadcast player is NULL.\n");
		return BPL_STATUS_ERROR;
	}

	uid    = OPL_Channel_GetUniqueID(channel);
	status = opl_broadcast_BindChannel(hBcast, uid, 0);
	if (status == BPL_STATUS_OK)
	{
		HUINT32 ulMainViewId = 0;
		if(APK_MEDIA_PLAY_GetMainViewId(&ulMainViewId) == ERR_OK)
		{
			if(hBcast->ulViewId == ulMainViewId)
			{
				opl_broadcast_memorizeCurrentChannel (uid);
			}
		}
		return BPL_STATUS_OK;
	}

	return BPL_STATUS_ERROR;
}

BPLStatus OPL_Broadcast_SetChannelByUIDWithTuneInfo(OplBroadcast_t *hBroadcast, OPL_Channel_t channel, unsigned int tuneInfo)
{
	BPLStatus		status;
	xOplBroadcast_t *hBcast;
	int 			uid;

	hBcast = xbroadcast(hBroadcast);
	if ( hBcast == NULL )
	{
		HxLOG_Error("The handle of broadcast player is NULL.\n");
		return BPL_STATUS_ERROR;
	}

	uid    = OPL_Channel_GetUniqueID(channel);

	status = opl_broadcast_BindChannel(hBcast, uid, tuneInfo);
	if (status == BPL_STATUS_OK)
	{
		HUINT32 ulMainViewId = 0;
		if(APK_MEDIA_PLAY_GetMainViewId(&ulMainViewId) == ERR_OK)
		{
			if(hBcast->ulViewId == ulMainViewId)
			{
				opl_broadcast_memorizeCurrentChannel (uid);
			}
		}
		return BPL_STATUS_OK;
	}

	return BPL_STATUS_ERROR;
}

BPLStatus OPL_Broadcast_SetChannelByChannelObjectFCC(OplBroadcast_t *hBroadcast, OPL_Channel_t channel, unsigned int ulViewID)
{
	xOplBroadcast_t *hBcast;
	int 			uid;

	hBcast = xbroadcast(hBroadcast);
	if ( hBcast == NULL )
	{
		HxLOG_Error("The handle of broadcast player is NULL.\n");
		return BPL_STATUS_ERROR;
	}

	uid = OPL_Channel_GetUniqueID(channel);
	if(uid != 0)
	{
		HUINT32 ulSessionId = 0;
		HERROR	hErr = opl_broadcast_startLiveFcc(ulViewID, uid, &ulSessionId);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("[%s:%d] opl_broadcast_startLiveFcc err: ViewId=%d\n", __FUNCTION__, __LINE__, ulViewID);
//			return BPL_STATUS_ERROR;
		}
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_SetVisibility(OplBroadcast_t *hBroadcast, HBOOL bVisible)
{
    HERROR	hErr = ERR_FAIL;
	xOplBroadcast_t *hBcast;

	hBcast = xbroadcast(hBroadcast);
	if ( hBcast == NULL )
	{
		HxLOG_Error("The handle of broadcast player is NULL.\n");
		return BPL_STATUS_ERROR;
	}

	HxLOG_Critical("setVisibility resourceId=%d bVisible=%d\n", hBcast->ulViewId, bVisible);

	hErr = APK_MEDIA_PLAY_SetVideoHide(hBcast->ulViewId, !bVisible);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("[%s:%d] OPL_Broadcast_SetVisibility Err. ViewId=%d\n", __FUNCTION__, __LINE__, hBcast->ulViewId);
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_StopByViewID(OplBroadcast_t *hBroadcast, unsigned int ulViewID)
{
	HERROR	hErr = ERR_FAIL;
	xOplBroadcast_t *hBcast;

	// 호출되는 곳을 보니, FCC를 위해서 만들어진 채널로 보임..그래서 View ID가 0이 아닐 때만 처리 ?
	if(ulViewID == 0)
	{
		HxLOG_Error("not allow Video broad cast ID.\n");
		return BPL_STATUS_ERROR;
	}

	hBcast = xbroadcast(hBroadcast);
	if ( hBcast == NULL )
	{
		HxLOG_Error("The handle of broadcast player is NULL.\n");
		return BPL_STATUS_ERROR;
	}

	hErr = APK_MEDIA_PLAY_Stop(ulViewID);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("[%s:%d] OPL_Broadcast_StopByViewID Err. ViewId=%d\n", __FUNCTION__, __LINE__, ulViewID);
//			return BPL_STATUS_ERROR;
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_Stop (OplBroadcast_t *hBroadcast)
{
	xOplBroadcast_t *hBcast;
	HERROR				 hErr;

	hBcast = xbroadcast(hBroadcast);
	if ( hBcast == NULL )
	{
		HxLOG_Error("The handle of broadcast player is NULL.\n");
		return BPL_STATUS_ERROR;
	}

	OxMediaPlay_MediaType_e eType;
	APK_MEDIA_PLAY_GetPlayType(hBcast->ulViewId, &eType);

	switch (eType)
	{
	case eOxMP_MEDIATYPE_LIVE:
	case eOxMP_MEDIATYPE_TSRPLAYBACK:
		hErr = APK_MEDIA_PLAY_Stop (hBcast->ulViewId);
		break;

	default:
		hErr = ERR_OK;
		break;
	}

	if (ERR_OK != hErr)
	{
		HxLOG_Error("Stop Action Failed.\n");
	}

	switch (hBcast->ePlayState)
	{
	case OPLBROADCAST_PLAY_STATE_CONNECTING:
	case OPLBROADCAST_PLAY_STATE_PRESENTING:
		hBcast->ePlayState = OPLBROADCAST_PLAY_STATE_STOPPED;
		break;

	default:
		break;
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_GetVolume(OplBroadcast_t *hBroadcast, unsigned int *pulVolume)
{
	xOplBroadcast_t *hBcast = xbroadcast(hBroadcast);
	if ( hBcast == NULL )
	{
		HxLOG_Error("The handle of broadcast player is NULL.\n");
		return BPL_STATUS_ERROR;
	}

	if ( pulVolume == NULL )
	{
		HxLOG_Error("The buffer is invalid.\n");
		return BPL_STATUS_ERROR;
	}

	{
		HUINT32	nVolume;

		APK_OUTPUT_AUDIO_GetVolume(&nVolume);
		*pulVolume = nVolume;
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_SetVolume(OplBroadcast_t *hBroadcast, unsigned int ulVolume)
{
	if ( APK_OUTPUT_AUDIO_SetVolume(ulVolume) != ERR_OK )
	{
		HxLOG_Error("The handle of broadcast player is NULL.\n");
		return BPL_STATUS_ERROR;
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_GetMute(OplBroadcast_t *hBroadcast, int *pnMute)
{
	if ( pnMute == NULL )
	{
	    HxLOG_Error("Invalid Parameter\n");
	    return BPL_STATUS_ERROR;
	}

	if ( APK_OUTPUT_AUDIO_GetMasterMute(pnMute) != ERR_OK )
	{
	    HxLOG_Error("failed to calling function \n");
	    return BPL_STATUS_ERROR;
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_SetMute(OplBroadcast_t *hBroadcast, int nMute)
{
	if ( APK_OUTPUT_AUDIO_SetMasterMute(nMute) != ERR_OK )
	{
		return BPL_STATUS_ERROR;
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_GetWindowSize(OplBroadcast_t *hBroadcast, int *pX, int *pY, int *pWidth, int *pHeight, BPL_BOOL *pbIsFullScreen)
{
	xOplBroadcast_t *hBcast = xbroadcast(hBroadcast);
	if ( hBcast == NULL )
	{
		HxLOG_Error("The handle of broadcast player is NULL.\n");
		return BPL_STATUS_ERROR;
	}
	if(pX == NULL || pY == NULL || pWidth == NULL ||pHeight == NULL ||pbIsFullScreen == NULL)
	{
		HxLOG_Error("params are NULL.\n");
		return BPL_STATUS_ERROR;
	}
	if (ERR_OK != APK_MEDIA_PLAY_GetVideoSize(hBcast->ulViewId, pX, pY, pWidth, pHeight))
	{
		HxLOG_Error("APK_MEDIA_PLAY_GetVideoSize ret Error!!!!\n");
		return BPL_STATUS_ERROR;
	}
	HxLOG_Print(" hBcast->bFullScreen %d [%s : %d] ", hBcast->bFullScreen,__FUNCTION__,__LINE__);
	*pbIsFullScreen = hBcast->bFullScreen;
	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_SetVideoPosition(OplBroadcast_t *hBroadcast, int x, int y, int width, int height)
{
	xOplBroadcast_t *hBcast = xbroadcast(hBroadcast);
	if ( hBcast == NULL )
	{
		HxLOG_Error("The handle of broadcast player is NULL.\n");
		return BPL_STATUS_ERROR;
	}

    int adjusted_x = 0, adjusted_y = 0, adjusted_w = 0, adjusted_h = 0;
	int input_x = 0, input_y = 0, input_w = 0, input_h = 0;
    int output_x = 0, output_y = 0, output_w = 0, output_h = 0;
    int video_x = 0, video_y = 0, video_w = 0, video_h = 0;
    //int aspect_ratio = 1;
    float _float_video_ratio = 0; /* this is for replace video_ratio value as float */
    DxAspectRatio_e video_ratio = eDxASPECTRATIO_UNKNOWN;

    HxLOG_Print("[%s,%d] Requested position x %d y %d w %d h %d \n", __FUNCTION__, __LINE__, x, y, width, height );

    if ( x > OSD_WIDTH || y > OSD_HEIGHT || width == 0 || height == 0 )
    {
        HxLOG_Error("Over range request, this shall be ignored. !!!!\n");
        return BPL_STATUS_OK;
    }

    if ( x == 0 && y == 0 && width == OSD_WIDTH && height == OSD_HEIGHT )
    {
        HxLOG_Print("Maybe User want to show it as fullscreen !!!!\n");
        HERROR eRet = APK_MEDIA_PLAY_SetVideoSize(hBcast->ulViewId /*ulViewId*/, 0, 0, 0, 0);
		if ( eRet != ERR_OK )
		{
		    HxLOG_Error("APK_MEDIA_PLAY_SetVideoSize called as fullscreen but ret Error!!!!\n");
		    return BPL_STATUS_ERROR;
		}
        return BPL_STATUS_OK;
    }

    if (ERR_OK != APK_MEDIA_PLAY_GetStreamVideoSize(hBcast->ulViewId, (unsigned int*)&video_w, (unsigned int*)&video_h))
    {
        HxLOG_Error("APK_MEDIA_PLAY_GetVideoSize ret Error!!!!\n");
    }

    if ( video_w == 0 )
        video_w = OSD_WIDTH;
    if ( video_h == 0 )
        video_h = OSD_HEIGHT;

    if (ERR_OK != APK_MEDIA_PLAY_GetStreamAspectRatio(hBcast->ulViewId, &video_ratio))
    {
        HxLOG_Error("APK_MEDIA_PLAY_GetStreamAspectRatio ret Error!!!!\n");
    }

    switch( video_ratio )
    {
         case eDxASPECTRATIO_4X3:
            _float_video_ratio = (float) 4/ (float) 3;
            break;

         case eDxASPECTRATIO_16X9:
            _float_video_ratio = (float) 16/ (float) 9;
            break;

         case eDxASPECTRATIO_14X9:
            _float_video_ratio = (float) 14/ (float) 9;
            break;

         case eDxASPECTRATIO_1X1:
            _float_video_ratio = (float) 1;
            break;

         default:
            break;
    }

    OplVideoUtil_getFittedPosition( x, y, width, height, \
                                    _float_video_ratio, \
                                    video_x, video_y, video_w, video_h, \
                                    &adjusted_x, &adjusted_y, &adjusted_w, &adjusted_h );
    OplVideoUtil_getVideoOutputWindow( adjusted_x, adjusted_y, adjusted_w, adjusted_h,
                                       &output_x, &output_y, &output_w, &output_h );
    OplVideoUtil_getVideoInputWindow( adjusted_x, adjusted_y, adjusted_w, adjusted_h, \
                                      video_x, video_y, video_w, video_h,
                                      &input_x, &input_y, &input_w, &input_h );

    HxLOG_Print("[%s,%d] Video size x %d y %d w %d h %d \n", __FUNCTION__, __LINE__, video_x, video_y, video_w, video_h );
    HxLOG_Print("[%s,%d] Video ratio %s\n", __FUNCTION__, __LINE__, \
                                           (eDxASPECTRATIO_16X9 == video_ratio) ? "16:9" : \
                                           (( eDxASPECTRATIO_4X3 == video_ratio ) ? "4:3" : "unknow") );
    HxLOG_Print("[%s,%d] VideoOutputWindow x %d y %d w %d h %d \n", __FUNCTION__, __LINE__, output_x, output_y, output_w, output_h );
    HxLOG_Print("[%s,%d] VideoInputWindow x %d y %d w %d h %d \n", __FUNCTION__, __LINE__, input_x, input_y, input_w, input_h );
    HxLOG_Print("[%s,%d] Request to Platform\n", __FUNCTION__, __LINE__);

    APK_MEDIA_PLAY_SetPigRect( hBcast->ulViewId, 0,
                               input_x, input_y, input_w, input_h, video_w, video_h,
                               output_x, output_y, output_w, output_h, 1280, 720,
                               eDxASPECTRATIO_16X9, /* osd ratio, shall be 16:9 */
                               eDxDFC_SELECT_UNKNOWN, /* video output status like full/letterbox/pillerbox */
                               video_ratio );/* shall keep the information of video resource */

	hBcast->nLastWindowPositionX = x;
	hBcast->nLastWindowPositionY = y;
	hBcast->nLastWindowPositionWidth = width;
	hBcast->nLastWindowPositionHeight = height;
	hBcast->bFullScreen = FALSE;

	return BPL_STATUS_OK;
}

OPL_Channel_t	 OPL_Broadcast_GetCurrentChannel(OplBroadcast_t *hBroadcast)
{
	DxService_t				 stService;
	xOplBroadcast_t				*hBcast = xbroadcast(hBroadcast);
	BPLStatus					 hErr;

	if ( hBcast == NULL )
	{
		HxLOG_Error ("The handle of broadcast player is NULL.\n");
		return 0;
	}
	hErr = opl_broadcast_getCurrentMasterService (hBcast->ulViewId, &stService);
	if (BPL_STATUS_OK != hErr)
	{
		HxLOG_Error ("opl_broadcast_getCurrentMasterService Err.\n");
		return 0;
	}
	return OPL_Channel_FindChannelByUID(NULL, (int)stService.uid);
}

OPL_Channel_t	 OPL_Broadcast_GetLatestChannel(OplBroadcast_t *hBroadcast)
{
	HCHAR					 szCcid[32] = { 0, };
	OPL_ChannelManager_t	 chManager;
	OPL_Channel_t			 hChannel = NULL;
	HERROR					 hErr;

	chManager = OPL_Channel_GetManager();
	if (NULL == chManager)
	{
		return NULL;
	}

	hErr = APK_DB_GetStr((HCHAR *)USERDB_SVC_LAST_CHANNEL, szCcid, 32);
	if (ERR_OK == hErr)
	{
		hChannel = OPL_Channel_FindChannel (chManager, szCcid);
	}

	if (NULL == hChannel)
	{
		hChannel = OPL_Channel_FindChannelAt (chManager, 0);
		if (NULL == hChannel)
		{
			return NULL;
		}
	}
    return hChannel;
}


BPLStatus OPL_Broadcast_GetAVOutputsCount(OplBroadcast_t *hBroadcast, int *pnCount)
{
	return BPL_STATUS_OK;
}

OplAVOutput_t *OPL_Broadcast_GetAVOutputByIndex(OplBroadcast_t *hBroadcast, int nIndex)
{
	return NULL;
}

void OPL_Broadcast_FreeAVOutput(OplBroadcast_t *hBroadcast, OplAVOutput_t *pOutput)
{
}

OplRecorder_t *OPL_Broadcast_GetRecorder(OplBroadcast_t *hBroadcast)
{
	xOplBroadcast_t *hBcast = xbroadcast(hBroadcast);
	if ( hBcast == NULL )
	{
		HxLOG_Error("The handle of broadcast player is NULL.\n");
		return NULL;
	}

	if ( hBcast->recorder ) return hBcast->recorder;

	hBcast->recorder = OPL_Recorder_Create();
	if ( hBcast->recorder == NULL )
	{
		return NULL;
	}

	return hBcast->recorder;
}

OPL_Channel_t	 OPL_Broadcast_CreateChannelObject(OplBroadcast_t *hBroadcast, unsigned short usMajorChannel, unsigned short usMinorChannel)
{
	xOplBroadcast_t 	*hBcast = xbroadcast(hBroadcast);

	if ( hBcast == NULL )
	{
		HxLOG_Error("The handle of broadcast player is NULL.\n");
		return 0;
	}

	return OPL_Channel_FindChannelByNumber(OPL_Channel_GetManager(), (int)usMajorChannel, (int)usMinorChannel);
}

OPL_Channel_t	 OPL_Broadcast_CreateChannelObjectByTripleID(OplBroadcast_t *hBroadcast, int onid, int tsid, int sid)
{
	xOplBroadcast_t 	*hBcast = xbroadcast(hBroadcast);

	if ( hBcast == NULL )
	{
		HxLOG_Error("The handle of broadcast player is NULL.\n");
		return 0;
	}

	return OPL_Channel_FindChannelByTriplet(OPL_Channel_GetManager(), onid, tsid, sid);
}


STATIC HBOOL				 s_bOplBcTvMemorized = FALSE;
STATIC HINT32				 s_nOplBcTvMemorizedUid = -1;

STATIC OPL_Channel_t	opl_broadcast_getChangbleChannel_inTheRecording(OplBroadcast_t *hBcast)
{
	HBOOL				bChangable 			= FALSE;
	HUINT32				ulRunningRecordings = 0, i;
	OPL_Collection_t	*runningRecordings 	= OPL_RecordItem_GetRunningRecordings();
	HERROR				hErr				= ERR_FAIL;

	if (runningRecordings)
	{
		ulRunningRecordings = OPL_Collection_Length(runningRecordings);
		for (i = 0; i < ulRunningRecordings; ++i)
		{
			OPL_ScheduledItem_t	*item = OPL_Collection_ItemAt(runningRecordings, i);
			OPL_Channel_t	recChannel= OPL_Channel_FindChannel(NULL, item->channelID);
			hErr = opl_broadcast_CheckChannelChangable(hBcast, recChannel, &bChangable);
			{
				if (ERR_OK == hErr && bChangable)
				{	// can channel change
					OPL_Collection_Delete(runningRecordings);
					return recChannel;
				}
			}
		}
	}

	OPL_Collection_Delete(runningRecordings);
	return NULL;
}

static HERROR opl_broadcast_tvOn (xOplBroadcast_t *hBcast)
{
	HERROR					 hErr;
	HBOOL					 bChangable = FALSE;

	OxMediaPlay_MediaType_e eType;
	hErr = APK_MEDIA_PLAY_GetPlayType(hBcast->ulViewId, &eType);


	// TV Off (LIVE, TSRPB 가 아닌 경우) 일 경우에만 동작 한다.
	// TV 상태가 아니고 None이 아니면 (MediaPB, USRPB) 일단 다 끈다 !
	switch (eType)
	{
	case eOxMP_MEDIATYPE_LIVE:
	case eOxMP_MEDIATYPE_TSRPLAYBACK:
		return ERR_OK;

	case eOxMP_MEDIATYPE_PVRPLAYBACK:
	case eOxMP_MEDIATYPE_MEDIAPLAY:
		APK_MEDIA_PLAY_Stop (hBcast->ulViewId);
		break;

	default:
		break;
	}

	if (TRUE == s_bOplBcTvMemorized)
	{
		if (s_nOplBcTvMemorizedUid > 1)
		{
			OPL_Channel_t	channel = OPL_Channel_FindChannelByUID (NULL, s_nOplBcTvMemorizedUid);

			hErr = opl_broadcast_CheckChannelChangable(hBcast, channel, &bChangable);
			if (ERR_OK == hErr && bChangable)
			{	// can channel change

				/* start LIVE */
				{
					HUINT32  ulSessionId = 0;
					hErr = opl_broadcast_startLive(hBcast->ulViewId, ((DxService_t *)channel)->uid, &ulSessionId);
					if (ERR_OK != hErr)
					{
						HxLOG_Error ("[%s:%d] opl_broadcast_startLive err: ViewId=%d\n", __FUNCTION__, __LINE__, hBcast->ulViewId);
						return BPL_STATUS_ERROR;
					}
				}
			}
			else
			{	// check the recording channel
				OPL_Channel_t channel = opl_broadcast_getChangbleChannel_inTheRecording(hBcast);

				HxLOG_Print ("[%s:%d] stored channel can't changable! get the alt channel 0x(%x) in the recording channels \n"
					, __FUNCTION__, __LINE__, channel);

				/* start LIVE */
				{
					HUINT32  ulSessionId = 0;
					hErr = opl_broadcast_startLive(hBcast->ulViewId, ((DxService_t *)channel)->uid, &ulSessionId);
					if (ERR_OK != hErr)
					{
						HxLOG_Error ("[%s:%d] opl_broadcast_startLive err: ViewId=%d\n", __FUNCTION__, __LINE__, hBcast->ulViewId);
						return BPL_STATUS_ERROR;
					}
				}
			}
		}
		else
		{
			HxLOG_Error ("[%s:%d] when tvOff, there was no TV Live....\n", __FUNCTION__, __LINE__);
		}
	}
	else
	{
		DxService_t				 stService;

		if (opl_broadcast_getLastService (hBcast->ulViewId, &stService) == BPL_STATUS_OK)
		{
			OPL_Channel_t	channel = (OPL_Channel_t)&stService;
			hErr = opl_broadcast_CheckChannelChangable(hBcast, channel, &bChangable);
			if (ERR_OK == hErr && bChangable)
			{
				/* start LIVE */
				{
					HUINT32  ulSessionId = 0;
					hErr = opl_broadcast_startLive(hBcast->ulViewId, ((DxService_t *)channel)->uid, &ulSessionId);
					if (ERR_OK != hErr)
					{
						HxLOG_Error ("[%s:%d] opl_broadcast_startLive err: ViewId=%d\n", __FUNCTION__, __LINE__, hBcast->ulViewId);
						return BPL_STATUS_ERROR;
					}
				}
			}
			else
			{	// check the recording channel
				OPL_Channel_t channel = opl_broadcast_getChangbleChannel_inTheRecording(hBcast);
				HxLOG_Print ("[%s:%d] stored channel can't changable! get the alt channel 0x(%x) in the recording channels \n"
					, __FUNCTION__, __LINE__, channel);

				/* start LIVE */
				{
					HUINT32  ulSessionId = 0;
					hErr = opl_broadcast_startLive(hBcast->ulViewId, ((DxService_t *)channel)->uid, &ulSessionId);
					if (ERR_OK != hErr)
					{
						HxLOG_Error ("[%s:%d] opl_broadcast_startLive err: ViewId=%d\n", __FUNCTION__, __LINE__, hBcast->ulViewId);
						return BPL_STATUS_ERROR;
					}
				}
			}
		}
		else
		{
			HxLOG_Error ("[%s:%d] Error\n", __FUNCTION__, __LINE__);
		}
	}

	s_bOplBcTvMemorized = FALSE;
	s_nOplBcTvMemorizedUid = -1;

	return ERR_OK;
}

static HERROR opl_broadcast_tvOff (xOplBroadcast_t *hBcast)
{
	HINT32					 nSvcUid;
	OxMediaPlay_MediaType_e eType;

	APK_MEDIA_PLAY_GetPlayType(hBcast->ulViewId, &eType);


	// TV On (LIVE, TSRPB) 일 경우에만 동작 한다.
	switch (eType)
	{
	case eOxMP_MEDIATYPE_LIVE:
	case eOxMP_MEDIATYPE_TSRPLAYBACK:
		APK_MEDIA_PLAY_GetRequestId( hBcast->ulViewId, eOxMP_MEDIATYPE_LIVE, (HUINT32*)&nSvcUid);
		APK_MEDIA_PLAY_Stop (hBcast->ulViewId);
		break;
	default:
		nSvcUid = s_ulOplBroadcast_LastMasterSvcUid;
		break;
	}

	if ((FALSE == s_bOplBcTvMemorized) || (nSvcUid > 0))
	{
		s_bOplBcTvMemorized = TRUE;
		s_nOplBcTvMemorizedUid = nSvcUid;
	}

	return ERR_OK;
}

BPLStatus OPL_Broadcast_TvOn(OplBroadcast_t *hBroadcast)
{
	xOplBroadcast_t 		*hBcast = xbroadcast (hBroadcast);
	HERROR					 hErr;

	if (NULL == hBcast)				{ return BPL_STATUS_ERROR; }

	hErr = opl_broadcast_tvOn (hBcast);
	return (ERR_OK == hErr) ? BPL_STATUS_OK : BPL_STATUS_ERROR;

}

BPLStatus OPL_Broadcast_TvOff(OplBroadcast_t *hBroadcast)
{
	xOplBroadcast_t 		*hBcast = xbroadcast (hBroadcast);
	HERROR					 hErr;

	if (NULL == hBcast)				{ return BPL_STATUS_ERROR; }

	hErr = opl_broadcast_tvOff (hBcast);
	return (ERR_OK == hErr) ? BPL_STATUS_OK : BPL_STATUS_ERROR;
}

#define _____COMPONENTS_____

OxMediaPlay_Component_e opl_broadcast_cvtComponentType(eOplAVComponentType eType)
{
	OxMediaPlay_Component_e	 eApkCompType	= eOxMP_COMPONENT_NONE;

	switch (eType)
	{
		case OPLAVCOMPONENT_TYPE_VIDEO:		eApkCompType	=	eOxMP_COMPONENT_VIDEO;		break;
		case OPLAVCOMPONENT_TYPE_AUDIO:		eApkCompType	=	eOxMP_COMPONENT_AUDIO;		break;
		case OPLAVCOMPONENT_TYPE_SUBTITLE:	eApkCompType	=	eOxMP_COMPONENT_SUBTITLE;	break;
		case OPLAVCOMPONENT_TYPE_DATA:		eApkCompType	=	eOxMP_COMPONENT_DATA;		break;

		default:
			break;
	}

	return eApkCompType;
}

eOplAVComponentType opl_broadcast_cvtApk2OplComponentType(OxMediaPlay_Component_e eType)
{
	eOplAVComponentType	 eOplCompType	= OPLAVCOMPONENT_TYPE_MAX;

	switch (eType)
	{
		case eOxMP_COMPONENT_VIDEO:		eOplCompType	=	OPLAVCOMPONENT_TYPE_VIDEO;		break;
		case eOxMP_COMPONENT_AUDIO:		eOplCompType	=	OPLAVCOMPONENT_TYPE_AUDIO;		break;
		case eOxMP_COMPONENT_SUBTITLE:	eOplCompType	=	OPLAVCOMPONENT_TYPE_SUBTITLE;	break;
		case eOxMP_COMPONENT_DATA:		eOplCompType	=	OPLAVCOMPONENT_TYPE_DATA;		break;

		default:
			break;
	}

	return eOplCompType;
}

STATIC HERROR opl_broadcast_cvtVideoComponent (OxMediaPlay_VideoComp_t *pstDstVideo, OplAVComponent_t *pstSrcComp)
{
	if ( (NULL == pstDstVideo) || (NULL == pstSrcComp) )
		return ERR_FAIL;

	{
		HCHAR *encoding = NULL;

		pstSrcComp->ulComponentTag	= (HUINT32)pstDstVideo->nComponentTag;
		pstSrcComp->ulPid				= (HUINT32)pstDstVideo->nPid;
		pstSrcComp->eType			= OPLAVCOMPONENT_TYPE_VIDEO;

		encoding = opl_broadcast_convertVideoTypeToOplEncoding(pstDstVideo->nComponentType);
		HLIB_STD_StrNCpySafe (pstSrcComp->encoding, (HCHAR*)encoding, HxSTD_StrLen(encoding) + 1);

		if ( pstDstVideo->nComponentType == 0x01 || pstDstVideo->nComponentType == 0x05 || pstDstVideo->nComponentType == 0x09 ||
			 pstDstVideo->nComponentType == 0x0D )
		{
			pstSrcComp->fAspectRatio = ((HFLOAT32)4/3);
		}
		else if ( pstDstVideo->nComponentType == 0x02 || pstDstVideo->nComponentType == 0x03 || pstDstVideo->nComponentType == 0x04 ||
			 pstDstVideo->nComponentType == 0x06 || pstDstVideo->nComponentType == 0x07 || pstDstVideo->nComponentType == 0x08 ||
			 pstDstVideo->nComponentType == 0x0A || pstDstVideo->nComponentType == 0x0B || pstDstVideo->nComponentType == 0x0C ||
			 pstDstVideo->nComponentType == 0x0E || pstDstVideo->nComponentType == 0x0F || pstDstVideo->nComponentType == 0x10 )
		{
			pstSrcComp->fAspectRatio = ((HFLOAT32)16/9);
		}
		else
		{
			HxLOG_Warning("Can't get the aspect ratio. unknown component type(0x%x) \n",pstDstVideo->nComponentType);
		}

#if defined( CONFIG_OP_JAPAN)
		{
			/* 일본향 스펙상 PMT 데이타를 기준으로 EIT 정보를 주어야 한다. */
			OplAVComponent_t		*tempAVComponent = NULL;
			OPL_ProgrammeHandle 	programme= NULL;
			OPL_Collection_t		*coll = NULL;
			HINT32				i =0, j =0, collLen =0 ;

			hErr = opl_broadcast_getCurrentProgramme( hBcast ,&programme );
			if( ERR_OK == hErr && programme != NULL )
			{
				coll = OPL_Programme_GetComponents (programme , OPLAVCOMPONENT_TYPE_VIDEO );
				if( coll != NULL )
				{
					collLen = OPL_Collection_Length(coll);
					for( i =0; i<collLen ; i++ )
					{
						tempAVComponent = (OplAVComponent_t *)OPL_Collection_ItemAt(coll, i);
						if(  (tempAVComponent != NULL) && (tempAVComponent->ulComponentTag == pstSrcComp->ulComponentTag) )
						{/* 모자란 정보는 EIT에서 체워 준다. */
							pstSrcComp->ulComponentType = tempAVComponent->ulComponentType;
							pstSrcComp->fAspectRatio = tempAVComponent->fAspectRatio;

							if( tempAVComponent->szText )
							{
								HLIB_STD_StrNCpySafe( pstSrcComp->szText, tempAVComponent->szText , OPLAVCOMPONENT_TEXT_SIZE );
							}

							if( tempAVComponent->nMultiViewNum > 0 )
							{
								for( j=0; j< tempAVComponent->nMultiViewNum; j++ )
								{
									HLIB_STD_StrNCpySafe( pstSrcComp->szMultiView[j], tempAVComponent->szMultiView[j], OPLAVCOMPONENT_MULTIVIEW_SIZE );
								}
							}
							break;
						}
					}

					if( i >= collLen )
					{
						//막으면 PMT 우선 처리 , hResult = ERR_FAIL;
					}
				}
				else
				{
					//막으면 PMT 우선 처리 , hResult = BPL_STATUS_ERROR;
				}
			}
			else
			{
				//막으면 PMT 우선 처리 , hResult = BPL_STATUS_ERROR;
			}

			if( programme != NULL )
			{
				OPL_Programme_Delete( programme );
			}
		}
#endif
	}

	return ERR_OK;
}

//#define	OPL_AUDIO_IS_AD_BM_TYPE(eAudType, ucMixType, eSubAudType)		((eOxMP_AUDIOTYPE_VISUAL_IMPAIRED == eAudType) ||(((eOxMP_AUDIOTYPE_VISUAL_IMPAIRED == eAudType) || ( eOxMP_AUDIOTYPE_UNDEFINED == eAudType) || (eOxMP_AUDIOTYPE_CLEAN_EFFECTS == eAudType)) && ((ucMixType == 1) && (eOxMP_AUDIOTYPE_SUPP_AD_VISUAL_IMPARED == eSubAudType))))
//#define	OPL_AUDIO_IS_AD_RM_TYPE(eAudType, ucMixType, eSubAudType)		((eOxMP_AUDIOTYPE_VISUAL_IMPAIRED == eAudType) || ((eOxMP_AUDIOTYPE_VISUAL_IMPAIRED == eAudType) && ((ucMixType == 0) && (eOxMP_AUDIOTYPE_SUPP_AD_VISUAL_IMPARED == eSubAudType))))

STATIC HERROR opl_broadcast_cvtAudioComponent (OxMediaPlay_AudioComp_t *pstDstAudio, OplAVComponent_t *pstSrcComp)
{
#if defined( CONFIG_OP_JAPAN)||defined(CONFIG_OP_FREESAT)
	HERROR					 hErr, hResult = BPL_STATUS_ERROR;
#endif

	if ( (NULL == pstDstAudio) || (NULL == pstSrcComp) )
		return ERR_FAIL;

	{
		HINT32 i;
		HCHAR *encoding = NULL;

		pstSrcComp->ulComponentTag	= (HUINT32)pstDstAudio->nComponentTag;
		pstSrcComp->ulPid				= (HUINT32)pstDstAudio->nPid;
		pstSrcComp->eType			= OPLAVCOMPONENT_TYPE_AUDIO;
		pstSrcComp->eSubType			= opl_broadcast_convertAudioComponentTypeToOplSubType (pstDstAudio->eMode);
		pstSrcComp->ulAudioChannels 	= opl_broadcast_convertAudioTypeToAudioChannels(pstDstAudio->eCompType);
		encoding 						= opl_broadcast_convertAudioCodecToOplEncoding(pstDstAudio->eCodec);

		HLIB_STD_StrNCpySafe (pstSrcComp->encoding, (HCHAR*)encoding, HxSTD_StrLen(encoding) + 1);

		for( i=0 ; i<(OxMP_AUDIO_LANG_NUM-1) ; i++ )
		{
#if defined(BPLBROADCAST_AUDIO_DUAL_MONO)
			if (pstDstAudio->astLangInfo[i].szLangCode[0] != '\0' && pstDstAudio->astLangInfo[i+1].szLangCode[0] != '\0')
			{
				HLIB_STD_StrNCpySafe (pstSrcComp->szLanguage, (HCHAR*)pstDstAudio->astLangInfo[i].szLangCode, 4);
				HLIB_STD_StrNCpySafe (pstSrcComp->szLanguage2, (HCHAR*)pstDstAudio->astLangInfo[i+1].szLangCode, 4);
				pstSrcComp->eSubType = OPLAVCOMPONENT_SUBTYPE_AUDIO_DUALMONO;
				break;
			}
			else
			{
				if (pstDstAudio->astLangInfo[i].szLangCode[0] != '\0')
				{
					HLIB_STD_StrNCpySafe (pstSrcComp->szLanguage, (HCHAR*)pstDstAudio->astLangInfo[i].szLangCode, 4);
					break;
				}
			}
#else
			if (pstDstAudio->astLangInfo[i].szLangCode[0] != '\0')
			{
				HLIB_STD_StrNCpySafe (pstSrcComp->szLanguage, (HCHAR*)pstDstAudio->astLangInfo[i].szLangCode, 4);
				break;
			}
#endif
		}

		pstSrcComp->bAudioDescription =  OAPI_AUDIO_IS_AD_BM_TYPE(pstDstAudio->astLangInfo[0].eAudType, pstDstAudio->ucMixType, pstDstAudio->eSupAudType);
		pstSrcComp->bAudioDescription |= OAPI_AUDIO_IS_AD_BM_TYPE(pstDstAudio->astLangInfo[1].eAudType, pstDstAudio->ucMixType, pstDstAudio->eSupAudType);
		pstSrcComp->bAudioDescription |= OAPI_AUDIO_IS_AD_RM_TYPE(pstDstAudio->astLangInfo[0].eAudType, pstDstAudio->ucMixType, pstDstAudio->eSupAudType);
		pstSrcComp->bAudioDescription |= OAPI_AUDIO_IS_AD_RM_TYPE(pstDstAudio->astLangInfo[1].eAudType, pstDstAudio->ucMixType, pstDstAudio->eSupAudType);

		//printf(C_CODE_F_BLACK_B_GREEN" ## ysmoon ## pstSrcComp->szLanguage(%s) \n"C_CODE_RESET"\n", pstSrcComp->szLanguage);
		//printf(C_CODE_F_BLACK_B_GREEN" ## ysmoon ## pstSrcComp->bAudioDescription(%d) \n"C_CODE_RESET"\n", pstDstAudio->astLangInfo[0].eAudType, pstDstAudio->astLangInfo[1].eAudType, pstDstAudio->ucMixType, pstDstAudio->eSupAudType);
		//printf(C_CODE_F_BLACK_B_GREEN" ## ysmoon ## pstSrcComp->bAudioDescription(%d) \n"C_CODE_RESET"\n", pstSrcComp->bAudioDescription);
		//printf(C_CODE_F_BLACK_B_GREEN" ## ysmoon ## pstDstAudio->nPid(0x%x) pstSrcComp->bAudioDescription(%d) \n"C_CODE_RESET"\n", pstDstAudio->nPid, pstSrcComp->bAudioDescription);

#if defined( CONFIG_OP_JAPAN)
		OxMediaPlay_MediaType_e eType;
		APK_MEDIA_PLAY_GetPlayType(hBcast->ulViewId, &eType);

		switch (eType)
		{
			case eOxMP_MEDIATYPE_LIVE:
			{
				/* 일본향 스펙상 PMT 데이타를 기준으로 EIT 정보를 주어야 한다. */
				OplAVComponent_t		*tempAVComponent = NULL;
				OPL_ProgrammeHandle 	programme= NULL;
				OPL_Collection_t		*coll = NULL;
				HINT32				i =0, j =0, collLen =0 ;

				hErr = opl_broadcast_getCurrentProgramme( hBcast , &programme );
				if( ERR_OK == hErr && programme != NULL )
				{
					coll = OPL_Programme_GetComponents (programme , OPLAVCOMPONENT_TYPE_AUDIO );
					if( coll != NULL )
					{
						collLen = OPL_Collection_Length(coll);
						for( i =0; i<collLen ; i++ )
						{
							tempAVComponent = (OplAVComponent_t *)OPL_Collection_ItemAt(coll, i);
							if(  (tempAVComponent != NULL) && (tempAVComponent->ulComponentTag == pstSrcComp->ulComponentTag) )
							{/* 모자란 정보는 EIT에서 체워 준다. */
								pstSrcComp->ulComponentType = tempAVComponent->ulComponentType;
								pstSrcComp->ulStreamType = tempAVComponent->ulStreamType;
								pstSrcComp->bAudioDescription = tempAVComponent->bAudioDescription;
								pstSrcComp->ulAudioChannels = tempAVComponent->ulAudioChannels;

								if( tempAVComponent->szLanguage)
								{
									HLIB_STD_StrNCpySafe( pstSrcComp->szLanguage, tempAVComponent->szLanguage , OPLAVCOMPONENT_LANG_SIZE );
								}

								if( tempAVComponent->szLanguage2)
								{
									HLIB_STD_StrNCpySafe( pstSrcComp->szLanguage2, tempAVComponent->szLanguage2 , OPLAVCOMPONENT_LANG_SIZE );
								}

								if( tempAVComponent->szText)
								{
									HLIB_STD_StrNCpySafe( pstSrcComp->szText, tempAVComponent->szText , OPLAVCOMPONENT_TEXT_SIZE );
								}

								if( tempAVComponent->nMultiViewNum > 0 )
								{
									for( j=0; j< tempAVComponent->nMultiViewNum; j++ )
									{
										HLIB_STD_StrNCpySafe( pstSrcComp->szMultiView[j], tempAVComponent->szMultiView[j], OPLAVCOMPONENT_MULTIVIEW_SIZE );
									}
								}
								break;
							}
						}

						if( i >= collLen )
						{
							//막으면 PMT 우선 처리 , hResult = ERR_FAIL;
						}
					}
					else
					{
						//막으면 PMT 우선 처리 ,hResult = BPL_STATUS_ERROR;
					}
				}
				else
				{
					//막으면 PMT 우선 처리 ,hResult = BPL_STATUS_ERROR;
				}

				if( programme != NULL )
				{
					OPL_Programme_Delete( programme );
				}

			}
			break;

			case eOxMP_MEDIATYPE_TSRPLAYBACK:
				if (pstDstAudio->astLangInfo[1].szLangCode )
					HLIB_STD_StrNCpySafe(pstSrcComp->szLanguage2, (HCHAR *)pstDstAudio->astLangInfo[1].szLangCode, OPLAVCOMPONENT_LANG_SIZE);
				if (pstDstAudio->aucStreamName )
					HLIB_STD_StrNCpySafe (pstSrcComp->szText, (HCHAR *)pstDstAudio->aucStreamName, OPLAVCOMPONENT_TEXT_SIZE);
				break;

			default:
				hErr = ERR_FAIL;
				break;
		}
#elif defined(CONFIG_OP_FREESAT)
		{
			/* PMT 데이타를 기준으로 EIT 정보를 주어야 한다. */
			DxEventComponent_t		*comp = NULL;
			OPL_ProgrammeHandle 	programme= NULL;
			OPL_Collection_t		*coll = NULL;
			HINT32				i =0, collLen =0 ;

			if(pstDstAudio->eCodec == eDxAUDIO_CODEC_DOLBY_AC3 || pstDstAudio.eCodec == eDxAUDIO_CODEC_DOLBY_AC3P)
			{
				pstSrcComp->eSubType = OPLAVCOMPONENT_SUBTYPE_AUDIO_DOLBY;
			}

			hErr = opl_broadcast_getCurrentProgramme( hBcast , &programme );
			if( ERR_OK == hErr && programme != NULL )
			{
				coll = OPL_Programme_GetAudioComponent (programme);
				if( coll != NULL )
				{
					collLen = OPL_Collection_Length(coll);

					for( i =0; i<collLen ; i++ )
					{
						comp = (DxEventComponent_t *)OPL_Collection_ItemAt(coll, i);
						if((comp != NULL) && (comp->tag == pstSrcComp->ulComponentTag) )
						{

							/* ETSI EN 300 468 - Stream_content and component_type */
							pstSrcComp->eSubType = OPLAVCOMPONENT_SUBTYPE_AUDIO_STEREO;
							switch (comp->content)
							{
							case 0x02:
								switch (comp->type)
								{
								case 0x01:
									pstSrcComp->eSubType = OPLAVCOMPONENT_SUBTYPE_AUDIO_SINGLEMONO;
									break;

								case 0x02:
									pstSrcComp->eSubType = OPLAVCOMPONENT_SUBTYPE_AUDIO_DUALMONO;
									break;

								case 0x03:
									pstSrcComp->eSubType = OPLAVCOMPONENT_SUBTYPE_AUDIO_STEREO;
									break;

								case 0x04:
									pstSrcComp->eSubType = OPLAVCOMPONENT_SUBTYPE_AUDIO_MULTILINGUAL;
									break;

								case 0x05:
									pstSrcComp->eSubType = OPLAVCOMPONENT_SUBTYPE_AUDIO_SURROUND;
									break;

								default:
									break;
								}
								break;

							case 0x04:
								pstSrcComp->eSubType = OPLAVCOMPONENT_SUBTYPE_AUDIO_DOLBY;
								break;

							default:
								break;
							}
							break;
						}
					}
				}
				else
				{
					hResult = BPL_STATUS_ERROR;
				}
			}
			else
			{
				hResult = BPL_STATUS_ERROR;
			}

			if( programme != NULL )
			{
				OPL_Programme_Delete( programme );
			}
			//coll 은 안의 내용이어서 collection delete를 하면 안된다.
			if(coll)
				HLIB_STD_MemFree(coll);
		}

#endif
	}

	return ERR_OK;
}

STATIC HERROR opl_broadcast_cvtSubtitleComponent (OxMediaPlay_SubtitleComp_t *pstDstSubtitle, OplAVComponent_t *pstSrcComp)
{
	if ( (NULL == pstDstSubtitle) || (NULL == pstSrcComp) )
		return ERR_FAIL;

	{
		/* common (DVB, EBU)*/
		pstSrcComp->eType	= OPLAVCOMPONENT_TYPE_SUBTITLE;
		pstSrcComp->ulPid	= (HUINT32)pstDstSubtitle->nPid;

		switch (pstDstSubtitle->eSubtitleType)
		{
			case eOxMP_SUBTITLETYPE_Dvb:
				pstSrcComp->eSubType			= OPLAVCOMPONENT_SUBTYPE_SUBTITLE_DVB;
				pstSrcComp->ulComponentTag	= (HUINT32)pstDstSubtitle->utArg.stDvbInfo.nComponentTag;

				pstSrcComp->bHardOfHearing_Impaired	= IsHardOfHearingSubtitle((HUINT32)pstDstSubtitle->utArg.stDvbInfo.nTypeValue);
				HLIB_STD_StrNCpySafe (pstSrcComp->szLanguage, pstDstSubtitle->utArg.stDvbInfo.szLangCode, OPLAVCOMPONENT_LANG_SIZE);

#if defined( CONFIG_OP_JAPAN )
				{
					/* 일본향 스펙상 PMT 데이타를 기준으로 EIT 정보를 주어야 한다. */
					OplAVComponent_t		*tempAVComponent = NULL;
					OPL_ProgrammeHandle 	programme= NULL;
					OPL_Collection_t		*coll = NULL;
					HINT32				collLen = 0, i =0;

					hResult = BPL_STATUS_OK;

					if( pstDstSubtitle->utArg.stDvbInfo.nComponentTag < 0x30 || pstDstSubtitle->utArg.stDvbInfo.nComponentTag > 0x37 )
					{
						hResult = ERR_FAIL;	/* caption 만.. */
					}
					else
					{

						hErr = opl_broadcast_getCurrentProgramme( hBcast , &programme );
						if( ERR_OK == hErr && programme != NULL )
						{/* EIT에 정보가 없으면 PMT 정보도 믿으면 안된다. 무조건 caption 올라오는 거 있다. */
							coll = OPL_Programme_GetComponents (programme , OPLAVCOMPONENT_TYPE_SUBTITLE );
							if( coll )
								collLen = OPL_Collection_Length(coll);

							if( collLen > 0 )
							{
								for( i =0; i<collLen ; i++ )
								{
									tempAVComponent = (OplAVComponent_t *)OPL_Collection_ItemAt(coll, i);
									if(  (tempAVComponent != NULL) && (tempAVComponent->ulComponentTag == pstSrcComp->ulComponentTag) )
									{/* 모자란 정보는 EIT에서 체워 준다. */
										pstSrcComp->ulComponentType = tempAVComponent->ulComponentType;

										if( tempAVComponent->szLanguage)
										{
											HLIB_STD_StrNCpySafe( pstSrcComp->szLanguage, tempAVComponent->szLanguage , OPLAVCOMPONENT_LANG_SIZE );
										}
										break;
									}
								}

								if( i >= collLen )
								{
									hResult = ERR_FAIL;
								}
							}
							else
							{
								hResult = ERR_FAIL;
							}
						}
						else
						{
							hResult = ERR_FAIL;
						}

						if( programme != NULL )
						{
							OPL_Programme_Delete( programme );
						}
					}
				}
#endif
				break;

			case eOxMP_SUBTITLETYPE_Ebu:
				pstSrcComp->eSubType		= OPLAVCOMPONENT_SUBTYPE_SUBTITLE_EBU;
				pstSrcComp->ulComponentTag	= (HUINT32)pstDstSubtitle->utArg.stEbuInfo.nComponentTag;

				HLIB_STD_StrNCpySafe (pstSrcComp->szLanguage, pstDstSubtitle->utArg.stEbuInfo.szLangCode, 4);
				break;

		default:
			break;
		}
	}

	return ERR_OK;
}

STATIC HERROR opl_broadcast_cvtDataComponent (OxMediaPlay_DataComp_t *pstDstData, OplAVComponent_t *pstSrcComp)
{
	if ( (NULL == pstDstData) || (NULL == pstSrcComp) )
		return ERR_FAIL;

	{
		pstSrcComp->eType				= OPLAVCOMPONENT_TYPE_DATA;
		pstSrcComp->ulComponentTag		= (HUINT32)pstDstData->nComponentTag;
		pstSrcComp->ulPid				= (HUINT32)pstDstData->nPid;

#if defined( CONFIG_OP_JAPAN)
		{
			/* 일본향 스펙상 PMT 데이타를 기준으로 EIT 정보를 주어야 한다. */
			OplAVComponent_t		*tempAVComponent = NULL;
			OPL_ProgrammeHandle 	programme= NULL;
			OPL_Collection_t		*coll = NULL;
			HINT32				i =0, j =0, collLen =0 ;

			/* EIT 와 연계 되었을 때만 TRUE */
			pstSrcComp->bDataEntryComponentFlag = FALSE;

			if( (pstSrcComp->ulComponentTag < 0x40) || (pstSrcComp->ulComponentTag  > 0x4f) )
			{/* 원래 40 ~ 7f 까지인데,, 기존 모델 JC 시리즈가 4f까지만 해서 동일하게 한다. */
				hResult = BPL_STATUS_ERROR;
			}
			else
			{
				hErr = opl_broadcast_getCurrentProgramme( hBcast , &programme );
				if( ERR_OK == hErr && programme != NULL )
				{
					/* component 는 COMPONENT TAG로 넘어가서 VIDEO쪽으로 접근한다. */
					coll = OPL_Programme_GetComponents (programme , OPLAVCOMPONENT_TYPE_DATA );
					if( coll != NULL )
					{
						collLen = OPL_Collection_Length(coll);
						for( i =0; i<collLen ; i++ )
						{
							tempAVComponent = (OplAVComponent_t *)OPL_Collection_ItemAt(coll, i);
							if(  (tempAVComponent != NULL) && (tempAVComponent->ulComponentTag == pstSrcComp->ulComponentTag) )
							{/* 모자란 정보는 EIT에서 체워 준다. */
								pstSrcComp->ulComponentTag = tempAVComponent->ulComponentTag;
								pstSrcComp->bAssociatedContentFlag = tempAVComponent->bAssociatedContentFlag;
								pstSrcComp->bDataEntryComponentFlag = tempAVComponent->bDataEntryComponentFlag;
								pstSrcComp->nDigitalCopyControl = tempAVComponent->nDigitalCopyControl;

								if( tempAVComponent->szText )
								{
									HLIB_STD_StrNCpySafe( pstSrcComp->szText, tempAVComponent->szText , OPLAVCOMPONENT_TEXT_SIZE );
								}

								if( tempAVComponent->nMultiViewNum > 0 )
								{
									for( j=0; j< tempAVComponent->nMultiViewNum; j++ )
									{
										HLIB_STD_StrNCpySafe( pstSrcComp->szMultiView[j], tempAVComponent->szMultiView[j], OPLAVCOMPONENT_MULTIVIEW_SIZE );
									}
								}
								break;
							}
						}

						if( i >= collLen )
						{
							//막으면 PMT 우선 처리 ,hResult = BPL_STATUS_ERROR;
						}
					}
					else
					{
						//막으면 PMT 우선 처리 ,hResult = BPL_STATUS_ERROR;
					}
				}
				else
				{
					//막으면 PMT 우선 처리 ,hResult = BPL_STATUS_ERROR;
				}

				if( programme != NULL )
				{
					OPL_Programme_Delete( programme );
				}
			}
		}
#endif


	}

	return ERR_OK;
}

HERROR opl_broadcast_getComponent(HUINT32 ulViewId, OxMediaPlay_Component_e  eApkCompType, HINT32 nApkCompIndex, OplAVComponent_t *pstComponent)
{
	HERROR hErr =	ERR_FAIL;
	OxMediaPlay_Component_t stApkComponent;

	if (NULL == pstComponent)
		return ERR_FAIL;

	HxSTD_MemSet (&stApkComponent, 0, sizeof(OxMediaPlay_Component_t));

	hErr = APK_MEDIA_PLAY_GetComponent (ulViewId, eApkCompType, (HINT32)nApkCompIndex, &stApkComponent);
	if( ERR_OK != hErr )
	{
		HxLOG_Error("[%s:%d] APK_MEDIA_PLAY_GetComponent Error : \n", __FUNCTION__, __LINE__ );
		return ERR_FAIL;
	}

	/* OK */
	HxSTD_MemSet (pstComponent, 0, sizeof(OplAVComponent_t));

	switch (eApkCompType)
	{
		case eOxMP_COMPONENT_VIDEO: 	hErr = opl_broadcast_cvtVideoComponent(&(stApkComponent.stVideo), pstComponent);	break;
		case eOxMP_COMPONENT_AUDIO: 	hErr = opl_broadcast_cvtAudioComponent(&(stApkComponent.stAudio), pstComponent);	break;
		case eOxMP_COMPONENT_SUBTITLE:	hErr = opl_broadcast_cvtSubtitleComponent(&(stApkComponent.stSubtitle), pstComponent);	break;
		case eOxMP_COMPONENT_DATA:		hErr = opl_broadcast_cvtDataComponent(&(stApkComponent.stData), pstComponent);	break;
		default:
			return ERR_FAIL;
	}

	if( ERR_OK != hErr )
	{
		HxLOG_Error("[%s:%d] can't convert component (APK->OPL) Error : \n", __FUNCTION__, __LINE__ );
		return ERR_FAIL;
	}

	return ERR_OK;
}

#define _____Dual_Mono_____
#if 0//defined(BPLBROADCAST_AUDIO_DUAL_MONO)
BPLStatus	opl_broadcast_CountComponentDualMonoAudio(OplBroadcast_t *hBroadcast, eOplAVComponentType eType, HUINT32 ulCompNum,HUINT32 *pulAudioNum)
{
	HERROR 					hErr =	ERR_FAIL;
	HUINT32					i = 0, j = 0, ulCount = 0;
	xOplBroadcast_t 		*hBcast = xbroadcast(hBroadcast);
	OxMediaPlay_Component_e	eApkCompType	= eOxMP_COMPONENT_NONE;
	OxMediaPlay_Component_t stApkComponent;

	eApkCompType = opl_broadcast_cvtComponentType (eType);

	for (i=0; i<ulCompNum; i++)
	{
		HxSTD_MemSet (&stApkComponent, 0, sizeof(OxMediaPlay_Component_t));
		hErr = APK_MEDIA_PLAY_GetComponent (hBcast->ulViewId, eApkCompType, (HINT32)i, &stApkComponent);
		if( ERR_OK != hErr )
		{
			HxLOG_Error("[%s:%d] APK_MEDIA_PLAY_GetComponent Error : \n", __FUNCTION__, __LINE__ );
			return ERR_FAIL;
		}

		for (j=0; j<OxMP_AUDIO_LANG_NUM; j++)
		{
			if (stApkComponent.stAudio.astLangInfo[j].szLangCode[0] != '\0')
			{
				ulCount++;
				HxLOG_Print("[%s:%d] Audio Language(%d):[%s]\n", __FUNCTION__, __LINE__, ulCount, stApkComponent.stAudio.astLangInfo[j].szLangCode);
			}
		}
	}

	*pulAudioNum = ulCount;

	return ERR_OK;
}

#endif


BPLStatus OPL_Broadcast_CountMutliViewComponent (OplBroadcast_t *hBroadcast, HUINT32 *pulCompNum)
{
#if defined( CONFIG_OP_JAPAN)
	xOplBroadcast_t 		*hBcast = xbroadcast(hBroadcast);
	HERROR				 hErr = ERR_FAIL;

	*pulCompNum = 0;


	{
		OPL_ProgrammeHandle 				programme= NULL;
		HxVector_t 						*descList = NULL;
		HINT32							count = 0;
		APKS_EventComponentGroupDes_t		*multiViewComp = NULL;

		hErr = opl_broadcast_getCurrentProgramme( hBcast ,&programme );
		if( hErr == ERR_OK  && programme != NULL )
		{
			hErr = OPL_Programme_GetMultiViewComponent( programme , 	descList );
			if( hErr == ERR_OK && descList != NULL )
			{
				count = HLIB_VECTOR_Length(descList);
				if(  count > 0 )
				{/* 1개 뿐이 없다. */
					multiViewComp = (APKS_EventComponentGroupDes_t	*)HLIB_VECTOR_ItemAt( descList , 0 );

					if( multiViewComp != NULL )
					{
						*pulCompNum = multiViewComp->ucNumOfGroup;
						hErr = ERR_OK;
					}
				}
			}

			if( descList )
			{
				HLIB_VECTOR_Delete( descList );
			}
		}

		if( programme != NULL )
		{
			OPL_Programme_Delete( programme );
		}
	}

	return (ERR_OK == hErr) ? BPL_STATUS_OK : BPL_STATUS_ERROR;
#else
	return BPL_STATUS_OK;
#endif
}

BPLStatus OPL_Broadcast_GetMultiViewComponent (OplBroadcast_t *hBroadcast, HINT32 index, OplMultiViewComponent_t *pstComponent)
{
#if defined( CONFIG_OP_JAPAN)
	HERROR				 hErr = ERR_FAIL;
	xOplBroadcast_t 		*hBcast = xbroadcast(hBroadcast);

	{
		OPL_ProgrammeHandle 				programme= NULL;
		HxVector_t 						*descList = NULL;
		HINT32							i =0, j=0, k =0, compLoop =0, n =0, count = 0;
		APKS_EventComponentGroupDes_t		*multiViewComp = NULL;
		HINT32							arrnComponntCount[OPLAVCOMPONENT_TYPE_MAX];
		OplAVComponent_t					arrstComponents[OPLAVCOMPONENT_TYPE_MAX][OPL_MULTI_VIEW_MAX_COMPONENT];

		hErr = opl_broadcast_getCurrentProgramme( hBcast ,&programme );
		if( hErr == ERR_OK  && programme != NULL )
		{
			hErr = OPL_Programme_GetMultiViewComponent( programme , 	descList );
			if( hErr == ERR_OK && descList != NULL )
			{
				count = HLIB_VECTOR_Length(descList);
				if(  count > 0 )
				{/* 1개 뿐이 없다. */
					multiViewComp = (APKS_EventComponentGroupDes_t	*)HLIB_VECTOR_ItemAt( descList , 0 );
					if( (multiViewComp != NULL) && (multiViewComp->ucNumOfGroup > 0))
					{
						/* 비교할 컴포넌트 다 불러온다. */
						for( compLoop =0; compLoop<OPLAVCOMPONENT_TYPE_MAX; compLoop++)
						{
							arrnComponntCount[compLoop] = 0;
							OPL_Broadcast_CountComponent( hBroadcast , compLoop, (HUINT32*)&arrnComponntCount[compLoop] );
							for( n =0; n < arrnComponntCount[compLoop]; n++ )
							{
								OPL_Broadcast_GetComponent( hBroadcast, compLoop, n, &arrstComponents[compLoop][n] );
							}
						}

						for( i =0; i< multiViewComp->ucNumOfGroup; i++ )
						{
							/* copy */
							if( i != index )
								continue;

							pstComponent->index =  index;
							HxSTD_StrNCpy( pstComponent->szName, (HCHAR*)multiViewComp->pstGroup[i].pucTextChar, OPL_MULTI_VIEW_COMPONENT_TEXT_SIZE );

							for( j =0; j< multiViewComp->pstGroup[i].ucNumOfCaUnit; j++ )
							{
								for( k =0; k< multiViewComp->pstGroup[i].pstCaUnit[j].ucNumOfComponent; k++)
								{
									for( compLoop =0; compLoop<OPLAVCOMPONENT_TYPE_MAX; compLoop++)
									{
										for( n =0; n < arrnComponntCount[compLoop]; n++ )
										{
											/* 같은게 있으면 추가해둠 */
											if( arrstComponents[compLoop][n].ulComponentTag  == multiViewComp->pstGroup[i].pstCaUnit[j].pucComponentTag[k] )
											{
												HxSTD_MemCopy( &(pstComponent->hComponents[pstComponent->compNum++]), &arrstComponents[compLoop][n], sizeof( OplAVComponent_t ));
											}
										}
									}
								}

								if( k < multiViewComp->pstGroup[i].pstCaUnit[j].ucNumOfComponent )
								{
									break;
								}
							}

							if( j < multiViewComp->pstGroup[i].ucNumOfCaUnit )
							{
								break;
							}
						}

						if( i >= multiViewComp->ucNumOfGroup )
						{/* not found */
							hErr = ERR_FAIL;
						}
					}

				}
			}

			if( descList )
			{
				HLIB_VECTOR_Delete( descList );
			}
		}

		if( programme != NULL )
		{
			OPL_Programme_Delete( programme );
		}
	}
	return (ERR_OK == hErr) ? BPL_STATUS_OK : BPL_STATUS_ERROR;
#else
	return BPL_STATUS_ERROR;
#endif
}

BPLStatus OPL_Broadcast_CountComponent (OplBroadcast_t *hBroadcast, eOplAVComponentType eType, HUINT32 *pulCompNum)
{
	xOplBroadcast_t 		*hBcast =	xbroadcast(hBroadcast);
	HERROR					 hErr	=	ERR_FAIL;

	OxMediaPlay_Component_e	 eApkCompType	=	eOxMP_COMPONENT_NONE;

	if ((NULL == hBcast) || (NULL == pulCompNum))				{ return BPL_STATUS_ERROR; }

	/* take a appkit component-type */
	eApkCompType = opl_broadcast_cvtComponentType (eType);


	/* get a count of component */
	if (eOxMP_COMPONENT_NONE != eApkCompType)
	{
		HUINT32	ulApkCompNum = 0;
		hErr = APK_MEDIA_PLAY_GetComponentNum (hBcast->ulViewId, eApkCompType, &ulApkCompNum);
		if( ERR_OK != hErr )
		{
			HxLOG_Error("[%s:%d] APK_MEDIA_PLAY_GetComponentNum Error : \n", __FUNCTION__, __LINE__ );
			return BPL_STATUS_ERROR;
		}

		/* OK */
		*pulCompNum	= ulApkCompNum;
		HxLOG_Debug("[%s:%d] Component-Type : (%d), Count : (%d)\n", __FUNCTION__, __LINE__,  eApkCompType, ulApkCompNum);
	}

	return (ERR_OK == hErr) ? BPL_STATUS_OK : BPL_STATUS_ERROR;
}

BPLStatus OPL_Broadcast_GetComponent (OplBroadcast_t *hBroadcast, eOplAVComponentType eType, unsigned int ulTrackIndex, OplAVComponent_t *pstComponent)
{
	xOplBroadcast_t 		*hBcast = xbroadcast(hBroadcast);
	HERROR					 hErr	= ERR_FAIL;
	OxMediaPlay_Component_e	 eApkCompType	= eOxMP_COMPONENT_NONE;

	if ((NULL == hBcast) || (NULL == pstComponent))				{ return BPL_STATUS_ERROR; }

	/* take a appkit component-type */
	eApkCompType = opl_broadcast_cvtComponentType (eType);

	/* get a component by index */
	hErr = opl_broadcast_getComponent (hBcast->ulViewId, eApkCompType, ulTrackIndex, pstComponent);
	if( ERR_OK != hErr )
	{
		HxLOG_Error("[%s:%d] opl_broadcast_getComponent Error : \n", __FUNCTION__, __LINE__ );
		return BPL_STATUS_ERROR;
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_GetCurrentComponent (OplBroadcast_t *hBroadcast, eOplAVComponentType eType, OplAVComponent_t *pstComponent)
{
	xOplBroadcast_t 		*hBcast = xbroadcast(hBroadcast);
	HERROR					 hErr		 	= ERR_FAIL;
	HINT32					 nApkCompIndex	= 0,nApkSubCompIndex = -1;
	OxMediaPlay_Component_e	 eApkCompType	= eOxMP_COMPONENT_NONE;

	if ((NULL == hBcast) || (NULL == pstComponent))				{ return BPL_STATUS_ERROR; }

	/* take a appkit component-type */
	eApkCompType = opl_broadcast_cvtComponentType (eType);


	/* get a current component-index */
	hErr = APK_MEDIA_PLAY_GetComponentIndex (hBcast->ulViewId, eApkCompType, &nApkCompIndex, &nApkSubCompIndex);
	if( ERR_OK != hErr )
	{
		HxLOG_Error("[%s:%d] APK_MEDIA_PLAY_GetComponentIndex Error : \n", __FUNCTION__, __LINE__ );
		return BPL_STATUS_ERROR;
	}


#if defined(CONFIG_OP_UK_DTT)
	if(nApkSubCompIndex != -1 && eApkCompType == eOxMP_COMPONENT_AUDIO)
	{
		/* get a component by index */
		hErr = opl_broadcast_getComponent (hBcast->ulViewId, eApkCompType, nApkSubCompIndex, pstComponent);
		if( ERR_OK != hErr )
		{
			HxLOG_Error("[%s:%d] opl_broadcast_getComponent Error : \n", __FUNCTION__, __LINE__ );
			return BPL_STATUS_ERROR;
		}
	}
	else
#endif
	{
		/* get a component by index */
		hErr = opl_broadcast_getComponent (hBcast->ulViewId, eApkCompType, nApkCompIndex, pstComponent);
		if( ERR_OK != hErr )
		{
			HxLOG_Error("[%s:%d] opl_broadcast_getComponent Error : \n", __FUNCTION__, __LINE__ );
			return BPL_STATUS_ERROR;
		}
	}


	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_SetCurrentComponent (OplBroadcast_t *hBroadcast, eOplAVComponentType eType, HUINT32 ulPid, HUINT8 *pszLang)
{
	xOplBroadcast_t 		*hBcast = xbroadcast(hBroadcast);
	HERROR					 hErr	= ERR_FAIL;

	OxMediaPlay_Component_e	 eApkCompType	= eOxMP_COMPONENT_NONE;
	HUINT32		ulCompNum = 0;

	if (NULL == hBcast)								{ return BPL_STATUS_ERROR; }

	/* take a appkit component-type */
	eApkCompType = opl_broadcast_cvtComponentType (eType);

	/* get a count of component */
	hErr = APK_MEDIA_PLAY_GetComponentNum (hBcast->ulViewId, eApkCompType, &ulCompNum);
	if( ERR_OK != hErr )
	{
		HxLOG_Error("[%s:%d] APK_MEDIA_PLAY_GetComponentNum Error: \n", __FUNCTION__, __LINE__ );
		return BPL_STATUS_ERROR;
	}

	/* find */
	{
		HINT32 i;
		OplAVComponent_t stComponent;

		for (i=0 ; i<ulCompNum ; i++ )
		{
			hErr =  opl_broadcast_getComponent(hBcast->ulViewId, eApkCompType, i, &stComponent);
			if( ERR_OK != hErr )
			{
				HxLOG_Error("[%s:%d] opl_broadcast_getComponent (%d) Error : \n", __FUNCTION__, __LINE__, i);
				return BPL_STATUS_ERROR;
			}

			/* compare */
			if (stComponent.ulPid == ulPid)
			{
				/* ext compare */
				if ((eOxMP_COMPONENT_AUDIO		== eApkCompType) ||
					(eOxMP_COMPONENT_SUBTITLE	== eApkCompType) )
				{
					if( NULL != pszLang )
					{
						if(HxSTD_StrNCmp((HCHAR*)stComponent.szLanguage, (HCHAR*)pszLang, 3) != 0)
							continue;
					}
				}

#if defined(CONFIG_OP_UK_DTT)

				if(eApkCompType == eOxMP_COMPONENT_SUBTITLE)
				{
					HBOOL	bEnabled = FALSE;

					APK_DB_GetInt(USERDB_SUBTITLE_DISPLAY, (HINT32*)&bEnabled);
					if(bEnabled != TRUE)
					{
						hErr = APK_SCENARIO_SetSubtitleSetting(TRUE);
						if (ERR_OK != hErr)
						{
							HxLOG_Error(" APK_SCENARIO_SetSubtitleSetting ret Error:0x%X.\n",hErr);
						}

						APK_DB_SetInt(USERDB_SUBTITLE_DISPLAY, (HUINT32)TRUE);
					}
				}
#endif
				hErr = APK_MEDIA_PLAY_SetComponentIndex (hBcast->ulViewId, eApkCompType, i);
				if( ERR_OK != hErr )
				{
					HxLOG_Error("[%s:%d] APK_MEDIA_PLAY_SetComponentIndex Error : \n", __FUNCTION__, __LINE__);
					return BPL_STATUS_ERROR;
				}

				break;
			}
		}
	}

	return ERR_OK;
}

BPLStatus OPL_Broadcast_GetChangedComponent (OplBroadcast_t *hBroadcast, eOplAVComponentType *eType)
{
	xOplBroadcast_t 		*hBcast = xbroadcast(hBroadcast);

	*eType = opl_broadcast_cvtApk2OplComponentType(hBcast->eChangedComponent);

	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_offComponent (OplBroadcast_t *hBroadcast, eOplAVComponentType eType, HUINT32 ulP1)
{
	xOplBroadcast_t 		*hBcast = xbroadcast(hBroadcast);
	HERROR					 hErr = ERR_OK;

 	OxMediaPlay_Component_e	 eApkCompType	= eOxMP_COMPONENT_NONE;

	if (NULL == hBcast)								{ return BPL_STATUS_ERROR; }

	/* take a appkit component-type */
	eApkCompType = opl_broadcast_cvtComponentType (eType);

	/* set */
	hErr = APK_MEDIA_PLAY_SetComponentIndex (hBcast->ulViewId, eApkCompType, -1);

	/* ext action */
	switch (eType)
	{
		case OPLAVCOMPONENT_TYPE_SUBTITLE:
			if(ERR_OK == hErr)
			{
#if defined(CONFIG_OP_UK_DTT)
				hErr = APK_SCENARIO_SetSubtitleSetting(FALSE);
				if (ERR_OK != hErr)
				{
					HxLOG_Error(" APK_SCENARIO_SetSubtitleSetting ret Error:0x%X.\n",hErr);
					return BPL_STATUS_ERROR;
				}

				APK_DB_SetInt(USERDB_SUBTITLE_DISPLAY, (HUINT32)FALSE);
#endif
				HAPI_BroadcastSignal("signal:onSubtitleState", 1, "i", 0);
			}
			break;

	 	default:
	 		return BPL_STATUS_ERROR;
	}

	return BPL_STATUS_OK;
}

#define _____VIEW_____
BPLStatus OPL_Broadcast_GetWidth (OplBroadcast_t *hBroadcast, HUINT32 *pulWidth)
{
	HINT32					 nWidth = 0;
	xOplBroadcast_t 		*hBcast = xbroadcast(hBroadcast);
	HERROR					 hErr;

	if ((NULL != hBcast) && (NULL != pulWidth))
	{
		hErr = APK_MEDIA_PLAY_GetVideoSize (hBcast->ulViewId, NULL, NULL, &nWidth, NULL);
		if (ERR_OK == hErr)
		{
			*pulWidth = (HUINT32)nWidth;
			return BPL_STATUS_OK;
		}
	}

	return BPL_STATUS_ERROR;
}

BPLStatus OPL_Broadcast_GetHeight (OplBroadcast_t *hBroadcast, HUINT32 *pulHeight)
{
	HINT32					 nHeight = 0;
	xOplBroadcast_t 		*hBcast = xbroadcast(hBroadcast);
	HERROR					 hErr;

	if ((NULL != hBcast) && (NULL != pulHeight))
	{
		hErr = APK_MEDIA_PLAY_GetVideoSize (hBcast->ulViewId, NULL, NULL, NULL, &nHeight);
		if (ERR_OK == hErr)
		{
			*pulHeight = (HUINT32)nHeight;
			return BPL_STATUS_OK;
		}
	}

	return BPL_STATUS_ERROR;
}

/*
	추후에 UI 시나리오가 달라지면 여기의 State 우선이 달라질 가능성이 높습니다.
	그래서 일단 함수로 따로 만들어 놓았고 추후 이에 따라 분기 여부를 결정하도록 하세요.
*/
STATIC eOplPlaybackError opl_broadcast_getNextErrorByStates (xOplBroadcast_t *hBcast, OxMediaPlay_Event_e eCurrEvent)
{
	// 지상파 경우 antenna short-circuit 이 0 순위 가짐 : MOON
	if (eOxMP_LiveViewState_NOK_ANTENNA == hBcast->eViewState)
	{
		return OPLBROADCAST_CHANNEL_ERROR_NOK_ANTENNA;
	}

	// 우선순위 1: Channel Unlock
	if ((eOxMP_LiveViewState_NO_SIGNAL == hBcast->eViewState) ||
		(eOxMP_LiveViewState_NO_SIGNAL_TIMEOUT == hBcast->eViewState))
	{
		return OPLBROADCAST_CHANNEL_ERROR_CANNOT_TUNE;
	}

	if (eOxMP_LiveViewState_MOTOR_MOVING == hBcast->eViewState)
	{
		return OPLBROADCAST_CHANNEL_ERROR_MOTOR_MOVING;
	}

	// 우선순위 2: Empty Service
	if (eOxMP_LiveViewState_EMPTY_SVC == hBcast->eViewState)
	{
		return OPLBROADCAST_CHANNEL_ERROR_EMPTY_CHANNEL;
	}

	// 우선순위 3: User Lock 관련
	if (eOxMP_LiveLockState_LOCK_PIN == hBcast->eLockState)
	{
		return OPLBROADCAST_CHANNEL_ERROR_PARENTAL_LOCK;
	}


	if (eOxMP_LiveLockState_LOCK_PIN_DENIED == hBcast->eLockState)
	{
		return OPLBROADCAST_CHANNEL_ERROR_PIN_DENIED;
	}

	// eOxMP_LiveViewState_REMOVE_CH 관련은 어느 우선순위에 있는지 구현해 주십시오.
	/*
	if (eOxMP_LiveViewState_REMOVE_CH == hBcast->eViewState)
	{
	}
	*/

	// 우선순위 4: CAS
	if (eOxMP_LiveViewState_CAS == hBcast->eViewState)
	{
		return OPLBROADCAST_CHANNEL_ERROR_CAS_BLOCK;
	}

	// 우선순위 5: SVC Lock 외의 Lock State들:
	switch (hBcast->eLockState)
	{
	case eOxMP_LiveLockState_RATING_PIN:
	case eOxMP_LiveLockState_SC_PIN:
		return OPLBROADCAST_CHANNEL_ERROR_PARENTAL_LOCK;

	case eOxMP_LiveLockState_RATING_PIN_DENIED:
	case eOxMP_LiveLockState_SC_PIN_DENIED:
		return OPLBROADCAST_CHANNEL_ERROR_PIN_DENIED;

	case eOxMP_LiveLockState_NO_EIT:
		return OPLBROADCAST_CHANNEL_ERROR_NO_EIT;

	default:
		break;
	}

	// 우선순위 6: No SVC, A/V 관련
	switch (hBcast->eViewState)
	{
	case eOxMP_LiveViewState_AV_UNDER_RUN:
		return OPLBROADCAST_CHANNEL_ERROR_AV_UNDERRUN;

	case eOxMP_LiveViewState_HDMI_BLOCKED:
		return OPLBROADCAST_CHANNEL_ERROR_HDMI_BLOCK;

	case eOxMP_LiveViewState_NO_VIDEO:
		return OPLBROADCAST_CHANNEL_ERROR_NO_VIDEO;

	case eOxMP_LiveViewState_NO_AV:
		return OPLBROADCAST_CHANNEL_ERROR_NO_AV;

	case eOxMP_LiveViewState_NOT_RUNNING:
		return OPLBROADCAST_CHANNEL_ERROR_NOT_RUNNING;

	default:
		break;
	}

	if (((eOxMP_LiveViewState_OK == hBcast->eViewState) || (eOxMP_LiveViewState_NONE == hBcast->eViewState)) &&
		((eOxMP_LiveLockState_OK == hBcast->eLockState) || (eOxMP_LiveLockState_NONE == hBcast->eLockState)))
	{
		return OPLBROADCAST_CHANNEL_ERROR_NO_ERROR;
	}

	return OPLBROADCAST_CHANNEL_ERROR_UNDEFINED;
}

STATIC BPLStatus opl_broadcast_getErrorFromNotifier (xOplBroadcast_t *hBcast, OxMediaPlay_Event_e eCurrEvent, HBOOL *pbStateChanged)
{
	//HBOOL								 bChanged = FALSE;
	HBOOL								 bPlayStateChanged = FALSE;
	eOplPlaybackError					 eNextError;
	OplBroadcastPlayState_e				 eNextPlayState;

	/*휴지중인 경우에는 lock state가 항상 none일 것이기 때문에 휴지중인 경우는 lock state와 상관 없이 따로 check한다.*/
	if ((eOxMP_LiveViewState_NONE == hBcast->eViewState) || (eOxMP_LiveLockState_NONE == hBcast->eLockState))
	{
		if(eOxMP_LiveViewState_EMPTY_SVC == hBcast->eViewState)
		{
			eNextPlayState	= OPLBROADCAST_PLAY_STATE_CONNECTING;
			eNextError		= opl_broadcast_getNextErrorByStates (hBcast, eCurrEvent);
		}
		else
		{
			eNextPlayState	= OPLBROADCAST_PLAY_STATE_CONNECTING;
			eNextError		= OPLBROADCAST_CHANNEL_ERROR_NO_ERROR;
		}
	}
	else if ((eOxMP_LiveViewState_OK == hBcast->eViewState) && (eOxMP_LiveLockState_OK == hBcast->eLockState))
	{
		eNextPlayState	= OPLBROADCAST_PLAY_STATE_PRESENTING;
		eNextError		= OPLBROADCAST_CHANNEL_ERROR_NO_ERROR;
	}
	else
	{
		eNextPlayState	= OPLBROADCAST_PLAY_STATE_CONNECTING;
		eNextError		= opl_broadcast_getNextErrorByStates (hBcast, eCurrEvent);
	}

	if (hBcast->ePlayState != eNextPlayState)
	{
		bPlayStateChanged	= TRUE;
	}
	else
	{
		bPlayStateChanged	= FALSE;

		// State가 Connecting일지어도, Error가 NO_ERROR <-> ERROR 면
		// Tune OK -> Error 로 가는 것이기 때문에 여기서 ERROR 경우도 체크해줘야 한다.
		if (OPLBROADCAST_PLAY_STATE_CONNECTING == eNextPlayState)
		{
			//if ((OPLBROADCAST_CHANNEL_ERROR_NO_ERROR == hBcast->ePlaybackError) && (OPLBROADCAST_CHANNEL_ERROR_NO_ERROR != eNextError))
			if (hBcast->ePlaybackError != eNextError)
			{
				bPlayStateChanged = TRUE;
			}
			/*
			else if ((OPLBROADCAST_CHANNEL_ERROR_NO_ERROR != hBcast->ePlaybackError) && (OPLBROADCAST_CHANNEL_ERROR_NO_ERROR == eNextError))
			{
				bPlayStateChanged = TRUE;
			}
			*/
		}
	}
	/*rating pin에 대한 error는 parentalRatingChanged event로 올려줘야한다.*/
	if(((eNextError == OPLBROADCAST_CHANNEL_ERROR_PARENTAL_LOCK)||(eNextError == OPLBROADCAST_CHANNEL_ERROR_PIN_DENIED))
	&&((hBcast->eLockState == eOxMP_LiveLockState_RATING_PIN)	||(hBcast->eLockState == eOxMP_LiveLockState_RATING_PIN_DENIED)))
	{
		bPlayStateChanged = FALSE;
	}
	else
	{
		hBcast->ePlayState		= eNextPlayState;
		hBcast->ePlaybackError	= eNextError;
	}

	if (NULL != pbStateChanged)						{ *pbStateChanged = bPlayStateChanged; }
	return BPL_STATUS_OK;
}

eOplPlaybackError OPL_Broadcast_GetErrorType (OplBroadcast_t *hBroadcast)
{
	xOplBroadcast_t 		*hBcast = xbroadcast(hBroadcast);
	eOplPlaybackError		errorType = OPLBROADCAST_CHANNEL_ERROR_UNDEFINED;

	if (NULL != hBcast)
	{
		errorType = hBcast->ePlaybackError;
	}

#if defined(CONFIG_OP_JAPAN)
	switch(errorType)
	{
		case OPLBROADCAST_CHANNEL_ERROR_EMPTY_CHANNEL:
			errorType = OPLBROADCAST_CHANNEL_ERROR_PMT_TIMEOUT;
			break;
		case OPLBROADCAST_CHANNEL_ERROR_CAS_BLOCK:
			errorType = OPLBROADCAST_CHANNEL_ERROR_ENCRYPTED_CHANNEL;
			break;
		case OPLBROADCAST_CHANNEL_ERROR_CHANNEL_NOT_SUPPORTED:
		case OPLBROADCAST_CHANNEL_ERROR_CANNOT_TUNE:
		case OPLBROADCAST_CHANNEL_ERROR_TUNER_LOCK:
		case OPLBROADCAST_CHANNEL_ERROR_PARENTAL_LOCK:
		case OPLBROADCAST_CHANNEL_ERROR_ENCRYPTED_CHANNEL:
		case OPLBROADCAST_CHANNEL_ERROR_UNKNOWN_CHANNEL:
		case OPLBROADCAST_CHANNEL_ERROR_CHANNEL_SWITCH_INTERRUPTED:
		case OPLBROADCAST_CHANNEL_ERROR_CANNOT_TUNE_DUETO_RECORD:
		case OPLBROADCAST_CHANNEL_ERROR_CANNOT_RESOLVE_IP_CHANNEL:
		case OPLBROADCAST_CHANNEL_ERROR_BANDWIDTH:
		case OPLBROADCAST_CHANNEL_ERROR_CANNOT_SUPPORT_CHANNEL_LIST:
		case OPLBROADCAST_CHANNEL_ERROR_INSUFFICIENT_RESOURCE:
		case OPLBROADCAST_CHANNEL_ERROR_NOT_FOUND_IN_TRANSPORT:
		case OPLBROADCAST_CHANNEL_ERROR_NO_ERROR:
			break;
		case OPLBROADCAST_CHANNEL_ERROR_PIN_DENIED:
			errorType = OPLBROADCAST_CHANNEL_ERROR_PARENTAL_LOCK;
			break;
		default:
			errorType = OPLBROADCAST_CHANNEL_ERROR_UNDEFINED;
			break;
	}
#endif

	return errorType;
}

BPLStatus OPL_Broadcast_SetErrorType (OplBroadcast_t *hBroadcast, eOplPlaybackError type)
{
	xOplBroadcast_t 		*hBcast = xbroadcast(hBroadcast);

	if (NULL != hBcast)
	{
		hBcast->ePlaybackError = type;
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_GetPlayState (OplBroadcast_t *hBroadcast, OplBroadcastPlayState_e *peState)
{
	xOplBroadcast_t						*hBcast = xbroadcast(hBroadcast);

	if ((NULL == hBcast) || (NULL == peState))
	{
		HxLOG_Error("hBcast:0x%08x, peState:0x%08x\n", (HUINT32)hBcast, (HUINT32)peState);
		return BPL_STATUS_ERROR;
	}

	*peState = hBcast->ePlayState;
	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_SetPlayState (OplBroadcast_t *hBroadcast, OplBroadcastPlayState_e peState)
{
	xOplBroadcast_t						*hBcast = xbroadcast(hBroadcast);

	if (NULL == hBcast)
	{
		HxLOG_Error("hBcast:0x%08x, peState:0x%08x\n", (HUINT32)hBcast, (HUINT32)peState);
		return BPL_STATUS_ERROR;
	}

	hBcast->ePlayState = peState;
	return BPL_STATUS_OK;
}


BPLStatus OPL_Broadcast_GetPlaySpeed (OplBroadcast_t *hBroadcast, signed int *pnSpeed)
{
	xOplBroadcast_t		*hBcast = xbroadcast(hBroadcast);
	HERROR				 hErr;

	if ((NULL == hBcast) || (NULL == pnSpeed))
	{
		HxLOG_Error("hBcast:0x%08x, pulSpeed:0x%08x\n", (HUINT32)hBcast, (HUINT32)pnSpeed);
		return BPL_STATUS_ERROR;
	}

	*pnSpeed = 0;

	hErr = APK_MEDIA_PLAY_GetPlaySpeed (hBcast->ulViewId, pnSpeed);
	if (hErr != ERR_OK)
	{
		HxLOG_Error ("APK_MEDIA_PLAY_GetPlaySpeed Err.\n");
		return BPL_STATUS_OK;
	}
	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_SetPlaySpeed (OplBroadcast_t *hBroadcast, signed int nSpeed)
{
	xOplBroadcast_t		*hBcast = xbroadcast(hBroadcast);
	BPLStatus			 hBPLErr;
	OxMediaPlay_MediaType_e eType;
	HBOOL				bStartPaused = FALSE;

	if (NULL == hBcast)
	{
		HxLOG_Error("hBcast:0x%08x\n", (HUINT32)hBcast);
		return BPL_STATUS_ERROR;
	}

	APK_MEDIA_PLAY_GetPlayType(hBcast->ulViewId, &eType);

	if( eOxMP_MEDIATYPE_LIVE == eType )
	{
		// 현재가 Live 중이라면 FF X1 이상은 무리이고, 반드시 Trick으로 가야 함....
		if (nSpeed >= 100)
		{
			HxLOG_Error ("SetSpeed (%d) impossible for the Live Mode\n", nSpeed);
			return BPL_STATUS_ERROR;
		}

#if defined (CONFIG_TSR_IMMEDIATELY_PAUSE)
		if(nSpeed <= 0)
		{
			// Support Immediately Start&Pause and Start&FR
			bStartPaused = TRUE;
		}
#endif

		hBPLErr = OPL_Broadcast_SetOffset (hBroadcast, 3, 2, bStartPaused, TRUE);

		if(nSpeed != 0)
		{
			// for start with FR
			bStartPaused = FALSE;
		}

		if (BPL_STATUS_OK != hBPLErr)
		{
			HxLOG_Error ("OPL_Broadcast_SetOffset Error\n");
			return BPL_STATUS_ERROR;
		}
	}

	if( bStartPaused == FALSE )
	{
		if ( ERR_OK != APK_MEDIA_PLAY_SetPlaySpeed(hBcast->ulViewId, nSpeed) )
		{

		}
	}

    hBcast->bIsTsrPlaying = TRUE;

	return BPL_STATUS_OK;
}

/*
		PVR Functions
*/
BPLStatus OPL_Broadcast_GetTimeShiftMode (OplBroadcast_t *hBroadcast, unsigned int *pulMode)
{
	BPL_BOOL		 bTsr;
	xOplBroadcast_t *hBcast = xbroadcast(hBroadcast);

	if ( hBcast == NULL )
	{
		HxLOG_Error("The handle of broadcast player is NULL.\n");
		return BPL_STATUS_ERROR;
	}

	if ( pulMode == NULL )
	{
		HxLOG_Error("The buffer is invalid.\n");
		return BPL_STATUS_ERROR;
	}

	APK_DB_GetInt(USERDB_RECORDING_TSR_ONOFF, (HINT32*)&bTsr);
	if (FALSE != bTsr)
	{
		*pulMode = 1;			// 1 : Timeshift shall use "local resource"
	}
	else
	{
		*pulMode = 0;			// 0 : Timeshift is turned off
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_SetTimeShiftMode (OplBroadcast_t *hBroadcast, unsigned int ulMode)
{
	xOplBroadcast_t *hBcast = xbroadcast(hBroadcast);
	if ( hBcast == NULL )
	{
		HxLOG_Error("The handle of broadcast player is NULL.\n");
		return BPL_STATUS_ERROR;
	}

	switch (ulMode)
	{
	case 0:
		APK_DB_SetInt(USERDB_RECORDING_TSR_ONOFF, FALSE);
		APK_MEDIA_PLAY_SetTSREnable (FALSE);
		break;

	case 1:
		APK_DB_SetInt(USERDB_RECORDING_TSR_ONOFF, TRUE);
		APK_MEDIA_PLAY_SetTSREnable (TRUE);
		break;

	default:
		HxLOG_Error ("Other mode is not implemented yet.\n");
		break;
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_GetCurrentTimeShiftMode (OplBroadcast_t *hBroadcast, unsigned int *pulMode)
{
	xOplBroadcast_t					*hBcast = xbroadcast(hBroadcast);
	DxService_t						 svcInfo;
	BPLStatus						 hErr;
	HBOOL							 bTSREnable;
	OxMediaPlay_MediaType_e 			eType = eOxMP_MEDIATYPE_LIVE;

	if ((NULL == hBcast) || (NULL == pulMode))
	{
		HxLOG_Error("hBcast:0x%08x, pulMode:0x%08x\n", (HUINT32)hBcast, (HUINT32)pulMode);
		return BPL_STATUS_ERROR;
	}

	*pulMode = 0;

	hErr = APK_MEDIA_PLAY_GetPlayType(hBcast->ulViewId, &eType);
	if(hErr != ERR_OK || eType == eOxMP_MEDIATYPE_MEDIAPLAY)
	{
		return BPL_STATUS_OK;
	}

	hErr = opl_broadcast_getCurrentMasterService (hBcast->ulViewId, &svcInfo);
	if (BPL_STATUS_OK != hErr)
	{
		HxLOG_Error ("opl_broadcast_getCurrentMasterService Err.\n");
		return BPL_STATUS_OK;
	}

	// TSR 이 동작하거나 현재 채널이 녹화중이면 Time Shift 가능하다.
	hErr = APK_MEDIA_PLAY_GetTSREnable( &bTSREnable );
	if( (ERR_OK == hErr) && (TRUE == bTSREnable) )
	{
		*pulMode = 1;		// Timeshift using "local resource"
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_IsTimeshiftEnabled (OplBroadcast_t *hBroadcast, HBOOL *bEnabled)
{
    xOplBroadcast_t					*hBcast = xbroadcast(hBroadcast);

    if ((NULL == hBcast) || (NULL == bEnabled))
	{
		HxLOG_Error("hBcast:0x%08x, pulMode:0x%08x\n", (HUINT32)hBcast, (HUINT32)bEnabled);
		return BPL_STATUS_ERROR;
	}

	*bEnabled = hBcast->bIsTsrPlaying;

	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_GetPlaybackOffset (OplBroadcast_t *hBroadcast, unsigned int *pulPbOffset)
{
#if defined(CONFIG_TIMESTAMP_64)
	HUINT64				ullTime = 0;
	HUINT64 				ullDurationTime = 0;
#endif
	HUINT32				 ulTime = 0;
	HUINT32 				 ulRsvSlot = 0;
	xOplBroadcast_t			*hBcast = xbroadcast(hBroadcast);
	DxService_t			 stSvcInfo;
	BPLStatus				 hErr;
	OxMediaPlay_MediaType_e eType;

	if ((NULL == hBcast) || (NULL == pulPbOffset))
	{
		HxLOG_Error("hBcast:0x%08x, pulPbOffset:0x%08x\n", (HUINT32)hBcast, (HUINT32)pulPbOffset);
		return BPL_STATUS_ERROR;
	}

	hErr = APK_MEDIA_PLAY_GetPlayType(hBcast->ulViewId, &eType);

	switch (eType)
	{
	case eOxMP_MEDIATYPE_LIVE:
		*pulPbOffset = 0;
		hErr = opl_broadcast_getCurrentMasterService (hBcast->ulViewId, &stSvcInfo);
		if (BPL_STATUS_OK == hErr)
		{
			HBOOL bIsRecording;
			APK_MEDIA_RECORD_IsRecording (stSvcInfo.uid, &bIsRecording);
			if (TRUE == bIsRecording )
			{
				ulTime = 0;

				// TBR Recording 시  hBcast->ulRecsessionId를 찾을 수 없다 ( APK_MEDIA_RECORD_Start를 이용하지 않았기 때문)
				// svc uid를 통해 Sama의 slot id를 가져와 APK_MEDIA_RECORD_GetRecTime에 넣어준다
				if ( ERR_OK == APK_MEDIA_RECORD_GetRsvSlotByUID(stSvcInfo.uid, &ulRsvSlot))
				{
					if (ERR_OK == APK_MEDIA_RECORD_GetRecTime (ulRsvSlot, &ulTime))
					{
						*pulPbOffset = ulTime - hBcast->nTsrStarttime;
						hBcast->nTsrPlaytime = ulTime;
					}
				}
				else
				{
					HxLOG_Error("[%s] [%d] APK_MEDIA_RECORD_GetRsvSlotByUID Error:\n", __FUNCTION__, __LINE__);
					*pulPbOffset = 0;
				}
			}
			else
			{
				if (ERR_OK == APK_MEDIA_PLAY_GetTsrRecTime (hBcast->ulViewId, &ulTime))
				{
					*pulPbOffset = ulTime;
				}
			}
		}
		break;

	case eOxMP_MEDIATYPE_TSRPLAYBACK:
		*pulPbOffset = 0;

		{
			HINT32 nPlayTime = 0;
			HUINT32 nStartTime = 0;
			HBOOL bGetStartTime = FALSE;
			HBOOL bIsRecording = FALSE;

			// 가장 최근의 TsrStartTime 을 읽어 온다.
			hErr = opl_broadcast_getCurrentMasterService (hBcast->ulViewId, &stSvcInfo);
			if (BPL_STATUS_OK == hErr)
			{
				APK_MEDIA_RECORD_IsRecording (stSvcInfo.uid, &bIsRecording);
				if (TRUE == bIsRecording )
				{
					if ( ERR_OK == APK_MEDIA_RECORD_GetRsvSlotByUID(stSvcInfo.uid, &ulRsvSlot))
					{
						if (ERR_OK == APK_MEDIA_RECORD_GetStartTime (ulRsvSlot, &ulTime))
						{
							HxLOG_Debug("[%s] [%d] succeeded to get Recording Start Time\n", __FUNCTION__, __LINE__);
							hBcast->nTsrStarttime = ulTime;
							bGetStartTime = TRUE;
						}
					}
				}
			}

			if(bGetStartTime == FALSE)
			{
				if(APK_MEDIA_PLAY_GetTsrStartTime(hBcast->ulViewId, &nStartTime) == ERR_OK)
				{
					HxLOG_Debug("[%s][%d] [TSR_PB]hBcast->nTsrStarttime=%d, nStartTime=%d\n",
						__FUNCTION__, __LINE__, hBcast->nTsrStarttime, nStartTime);
					hBcast->nTsrStarttime = nStartTime;
				}
			}
#if defined(CONFIG_TIMESTAMP_64)
			if(APK_MEDIA_PLAY_GetPlayPosition (hBcast->ulViewId, &ullTime)==ERR_OK)
			{
				hBcast->nTsrPlaytime		= (HINT32)ullTime;
			}
#else
			if(APK_MEDIA_PLAY_GetPlayPosition (hBcast->ulViewId, &ulTime)==ERR_OK)
			{
				hBcast->nTsrPlaytime		= (HINT32)ulTime;
			}
#endif
			else
			{
				hBcast->nTsrPlaytime ++;
			}

			nPlayTime = hBcast->nTsrPlaytime - hBcast->nTsrStarttime;
			/* exception */
			{
				HUINT32 ulDurationTime = 0;
				if(bIsRecording == TRUE)
				{
					APK_MEDIA_RECORD_GetRecTime(ulRsvSlot, &ulDurationTime);
				}
				else
				{
#if defined(CONFIG_TIMESTAMP_64)
					APK_MEDIA_PLAY_GetDurationTime (hBcast->ulViewId, &ullDurationTime);
					ulDurationTime = (HUINT32)ullDurationTime;
#else
					APK_MEDIA_PLAY_GetDurationTime (hBcast->ulViewId, &ulDurationTime);
#endif				
				}

				if (nPlayTime > (HINT32)ulDurationTime)
					nPlayTime = (HINT32)ulDurationTime;
				// nTsrPlaytime, nTsrStarttime 가 바로 바로 update 가 되지 않아 값이 역전되는 현상 발생
				// nPlayTime 은 0 보다 커야
				else if ( nPlayTime < 1 )
					nPlayTime = 1;
				HxLOG_Debug("[%s][%d] [TSR_PB]ulTsrStartTime=%d, nTsrPlaytime=%d, ulDurationTime=%d, ulPosition(playtime)=%d\n",
					__FUNCTION__, __LINE__, hBcast->nTsrStarttime, hBcast->nTsrPlaytime, ulDurationTime, nPlayTime);
			}

			*pulPbOffset = nPlayTime;
		}
		break;

	case eOxMP_MEDIATYPE_NONE:
		// LIVE -> TSR 전환시 NONE 상태가 순간 발생. 이때 타이밍 이슈로 Offset이 0이면 TSR 명령이 무시될 수 있다.
		*pulPbOffset = 1;
		break;

	default:
		HxLOG_Error ("[%s:%d] Current MediaPlay Type(%d) is invalid\n", __FUNCTION__, __LINE__, eType);
		*pulPbOffset = 0;
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_GetPlayPosition (OplBroadcast_t *hBroadcast, unsigned int *pulPlayPosition)
{
#if defined(CONFIG_TIMESTAMP_64)
	HUINT64 					ullTime = 0;
#else
	HUINT32 					ulTime = 0;
#endif
	xOplBroadcast_t			*hBcast = xbroadcast(hBroadcast);
	OxMediaPlay_MediaType_e eType;

	if ((NULL == hBcast) || (NULL == pulPlayPosition))
	{
		HxLOG_Error("hBcast:0x%08x, pulPbOffset:0x%08x\n", (HUINT32)hBcast, (HUINT32)pulPlayPosition);
		return BPL_STATUS_ERROR;
	}

	APK_MEDIA_PLAY_GetPlayType(hBcast->ulViewId, &eType);

	switch (eType)
	{
	case eOxMP_MEDIATYPE_LIVE:
		*pulPlayPosition = 0;
		break;

	case eOxMP_MEDIATYPE_TSRPLAYBACK:
#if defined(CONFIG_TIMESTAMP_64)
		if(APK_MEDIA_PLAY_GetPlayPosition (hBcast->ulViewId, &ullTime)==ERR_OK)
			*pulPlayPosition = (HUINT32)(ullTime * 1000);
#else
		if(APK_MEDIA_PLAY_GetPlayPosition (hBcast->ulViewId, &ulTime)==ERR_OK)
			*pulPlayPosition = ulTime * 1000;
#endif
		else
			*pulPlayPosition = 0;
		break;

	default:
		*pulPlayPosition = 0;
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_GetMaxOffset (OplBroadcast_t *hBroadcast, unsigned int *pulPbOffset)
{
	HUINT32							 ulTime = 0;
	xOplBroadcast_t					*hBcast = xbroadcast(hBroadcast);
	xOplBroadcastBufferingType_e	 eBufType = eOplBroadcastBufType_None;
	BPLStatus						 hErr;

	if ((NULL == hBcast) || (NULL == pulPbOffset))
	{
		HxLOG_Error("hBcast:0x%08x, pulPbOffset:0x%08x\n", (HUINT32)hBcast, (HUINT32)pulPbOffset);
		return BPL_STATUS_ERROR;
	}

	hErr = opl_broadcast_getBufferingTypeAndTime (hBroadcast, &eBufType, &ulTime);
	if ((BPL_STATUS_OK != hErr) || (eOplBroadcastBufType_None == eBufType))
	{
		HxLOG_Error("No buffering...\n");
		*pulPbOffset = 0;
		return BPL_STATUS_OK;
	}

	*pulPbOffset = ulTime;
	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_GetStartOffset (OplBroadcast_t *hBroadcast, unsigned int *pulPbOffset)
{
	HUINT32							 ulTime = 0;
	xOplBroadcast_t					*hBcast = xbroadcast(hBroadcast);
	BPLStatus						 hErr;

	if ((NULL == hBcast) || (NULL == pulPbOffset))
	{
		HxLOG_Error("hBcast:0x%08x, pulPbOffset:0x%08x\n", (HUINT32)hBcast, (HUINT32)pulPbOffset);
		return BPL_STATUS_ERROR;
	}

	hErr = opl_broadcast_getStartTime (hBroadcast, &ulTime);
	if ((BPL_STATUS_OK != hErr))
	{
		HxLOG_Error("No buffering...\n");
		*pulPbOffset = 0;
		return BPL_STATUS_OK;
	}

	*pulPbOffset = ulTime;
	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_SetOffset (OplBroadcast_t *hBroadcast, int nOffset, unsigned int ulReference, int bStartPaused, int bUseLatestPbTime)
{
	HBOOL							 bPlayBuffer = FALSE;
	HINT32							 nAbsoluteOffset = 0;
	HUINT32							 ulCurrOffset = 0, ulMaxOffset = 0;
	xOplBroadcast_t					*hBcast = xbroadcast(hBroadcast);
	xOplBroadcastBufferingType_e	 eBufType = eOplBroadcastBufType_None;
	DxService_t					 	 stMasterSvc, stSuppleSvc;
	BPLStatus						 hErr;
	OxMediaPlay_MediaType_e eType;

	if (NULL == hBcast)
	{
		HxLOG_Error("hBcast NULL\n");
		return BPL_STATUS_ERROR;
	}

	HxSTD_MemSet (&stMasterSvc, 0, sizeof(DxService_t));

	hErr = opl_broadcast_getCurrentMasterService (hBcast->ulViewId, &stMasterSvc);
	if (BPL_STATUS_OK != hErr)
	{
		HxLOG_Error("Current Master Service None:\n");
		return BPL_STATUS_ERROR;
	}

	hErr = opl_broadcast_getCurrentSupplementaryService (hBcast->ulViewId, &stSuppleSvc);
	if (BPL_STATUS_OK != hErr)
	{
		HxSTD_MemCopy (&stSuppleSvc, &stMasterSvc, sizeof(DxService_t));
	}

	hErr = opl_broadcast_getBufferingTypeAndTime (hBroadcast, &eBufType, &ulMaxOffset);
	if ((BPL_STATUS_OK != hErr) || (eOplBroadcastBufType_None == eBufType))
	{
		HxLOG_Error("No buffering...\n");
		return BPL_STATUS_ERROR;
	}

	// nOffset 은 현재 Position 기준 상대값...앞으로 당기면 음수가 된다...
	switch (ulReference)
	{
	case 1:					// POSITION_CURRENT
		hErr = OPL_Broadcast_GetPlaybackOffset (hBroadcast, &ulCurrOffset);
		if (BPL_STATUS_OK != hErr)
		{
			HxLOG_Error ("cannot get the max offset:\n");
			return BPL_STATUS_ERROR;
		}

		nAbsoluteOffset = (HINT32)ulCurrOffset + nOffset;
		break;

	case 2:					// POSITION_END
		nAbsoluteOffset = (HINT32)ulMaxOffset - nOffset;
		break;

	case 0:					// POSITION_START
	default:
		nAbsoluteOffset = nOffset;
		break;
	}

	if (nAbsoluteOffset < 0)								{ nAbsoluteOffset = 0; }
	else if (nAbsoluteOffset > (HINT32)ulMaxOffset)			{ nAbsoluteOffset = (HINT32)ulMaxOffset; }

	// 안전을 위해 Absolute Offset이 MAX보다 2 더 적지 않으면 Buffering Play를 안 하도록 한다.
	bPlayBuffer = ((HUINT32)nAbsoluteOffset + 2 < ulMaxOffset) ? TRUE : FALSE;

	APK_MEDIA_PLAY_GetPlayType(hBcast->ulViewId, &eType);

	switch (eType)
	{
	case eOxMP_MEDIATYPE_LIVE:
		// 현재 Live 상태 : Play Buffer 일 때에만 동작하자.
		if (TRUE == bPlayBuffer)
		{
			switch (eBufType)
			{
				case eOplBroadcastBufType_Usr:
				case eOplBroadcastBufType_Tsr:
					{
						HUINT32 					 ulSessionId;
						OxMediaPlay_StartInfo_t 	 unStart = {0,};
						unStart.stTsrPb.eType			= eOxMP_MEDIATYPE_TSRPLAYBACK;
						unStart.stTsrPb.ulMasterSvcUid	= stMasterSvc.uid;
						unStart.stTsrPb.ulSuppleSvcUid	= stMasterSvc.uid;
						unStart.stTsrPb.ulStartTime 	= nAbsoluteOffset;
						unStart.stTsrPb.bStartPaused	= bStartPaused;
						unStart.stTsrPb.bUseLatestPbTime	= bUseLatestPbTime;

						hErr = APK_MEDIA_PLAY_Start (hBcast->ulViewId, &unStart, &ulSessionId);
						if( ERR_OK != hErr )
						{
							HxLOG_Error("[%s:%d] APK_MEDIA_PLAY_Start Error :\n", __FUNCTION__, __LINE__ );
							return BPL_STATUS_ERROR;
						}
					}
					break;

				default:
					return BPL_STATUS_ERROR;
			}
		}

		break;

	case eOxMP_MEDIATYPE_TSRPLAYBACK:
		if (TRUE == bPlayBuffer)
		{
			switch (eBufType)
			{
			case eOplBroadcastBufType_Tsr:
				hBcast->nTsrPlaytime	= nAbsoluteOffset + hBcast->nTsrStarttime;
#if defined(CONFIG_TIMESTAMP_64)
				APK_MEDIA_PLAY_SetPlayPosition (hBcast->ulViewId, (HUINT64)hBcast->nTsrPlaytime);
#else
				APK_MEDIA_PLAY_SetPlayPosition (hBcast->ulViewId, hBcast->nTsrPlaytime);
#endif
				break;
			case eOplBroadcastBufType_Usr:
				hBcast->nTsrPlaytime	= nAbsoluteOffset;
#if defined(CONFIG_TIMESTAMP_64)
				APK_MEDIA_PLAY_SetPlayPosition (hBcast->ulViewId, (HUINT64)hBcast->nTsrPlaytime);
#else
				APK_MEDIA_PLAY_SetPlayPosition (hBcast->ulViewId, hBcast->nTsrPlaytime);
#endif
				break;

			// TSR Playback 중 TSR 외의 Buffering이 존재한다는 것 자체가 있을 수 없다.
			default:
				return BPL_STATUS_ERROR;
			}
		}
		else
		{
			APK_MEDIA_PLAY_Stop (hBcast->ulViewId);

			/* start LIVE */ // StartSupplementaryService 수정 필요
			{
				HUINT32  ulSessionId = 0;
				hErr = opl_broadcast_startLive(hBcast->ulViewId, stMasterSvc.uid, &ulSessionId);
				if (ERR_OK != hErr)
				{
					HxLOG_Error ("[%s:%d] opl_broadcast_startLive err: ViewId=%d\n", __FUNCTION__, __LINE__, hBcast->ulViewId);
					return BPL_STATUS_ERROR;
				}
			}
		}

		break;

	case eOxMP_MEDIATYPE_NONE:
	default:
		if (TRUE != bPlayBuffer)
		{
			/* start LIVE */
			{
				HUINT32  ulSessionId = 0;
				hErr = opl_broadcast_startLive(hBcast->ulViewId, stMasterSvc.uid, &ulSessionId);
				if (ERR_OK != hErr)
				{
					HxLOG_Error ("[%s:%d] opl_broadcast_startLive err: ViewId=%d\n", __FUNCTION__, __LINE__, hBcast->ulViewId);
					return BPL_STATUS_ERROR;
				}
			}
		}

		break;
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_StopTimeshift (OplBroadcast_t *hBroadcast)
{
	DxService_t						 stMasterSvc, stSuppleSvc;
	xOplBroadcast_t					*hBcast = xbroadcast(hBroadcast);
	BPLStatus						 hErr;
	OxMediaPlay_MediaType_e eType;

	if (NULL == hBcast)
	{
		HxLOG_Error ("hBcast:NULL\n");
		return BPL_STATUS_ERROR;
	}

	hErr = APK_MEDIA_PLAY_GetPlayType(hBcast->ulViewId, &eType);

	if (eOxMP_MEDIATYPE_TSRPLAYBACK != eType)
	{
		HxLOG_Error ("Current MediaType not TSRPB\n");
		return BPL_STATUS_ERROR;
	}

	HxSTD_MemSet (&stMasterSvc, 0, sizeof(DxService_t));

	hErr = opl_broadcast_getCurrentMasterService (hBcast->ulViewId, &stMasterSvc);
	if (BPL_STATUS_OK != hErr)
	{
		HxLOG_Error("Current Master Service None:\n");
		stMasterSvc.uid = s_ulOplBroadcast_LastMasterSvcUid;
	}

	hErr = opl_broadcast_getCurrentSupplementaryService (hBcast->ulViewId, &stSuppleSvc);
	if (BPL_STATUS_OK != hErr)
	{
		HxSTD_MemCopy (&stSuppleSvc, &stMasterSvc, sizeof(DxService_t));
	}

	if (ERR_OK != APK_MEDIA_PLAY_Stop (hBcast->ulViewId))
	{
		HxLOG_Error ("Error in [APK_MEDIA_PLAY_Stop]\n");
		return BPL_STATUS_ERROR;
	}

	/* start LIVE */ // StartSupplementaryService 수정 필요
	{
		HUINT32  ulSessionId = 0;
		hErr = opl_broadcast_startLive(hBcast->ulViewId, stMasterSvc.uid, &ulSessionId);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("[%s:%d] opl_broadcast_startLive err: ViewId=%d\n", __FUNCTION__, __LINE__, hBcast->ulViewId);
			return BPL_STATUS_ERROR;
		}
	}

    hBcast->bIsTsrPlaying = 0;
	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_RecordNow (OplBroadcast_t *hBroadcast, OPL_Channel_t channel, unsigned int ulDurationSec, char *pszRecordID)
{
	xOplBroadcast_t						*hBcast = xbroadcast(hBroadcast);
	DxService_t						 stService = {0, };
	DxService_t							stMasterSvc = {0, };
	HERROR								 hErr;

	// tuning 된 이후에만 즉시 녹화
	if((hBcast->eSessionState == OPLBROADCAST_SESSION_STATE_STARTED) &&
		((hBcast->ePlayState == OPLBROADCAST_PLAY_STATE_CONNECTING) || (hBcast->ePlayState == OPLBROADCAST_PLAY_STATE_PRESENTING)))
	{
		if (NULL != pszRecordID)
		{
			pszRecordID[0] = '\0';
		}

		HxSTD_MemSet (&stService, 0, sizeof(DxService_t));

		// 이건 보험...
		stService.uid = OPL_Channel_GetUniqueID(channel);

		// 서비스를 가져와야...이벤트를 못 가져왔을 때 서비스 이름으로 파일을 만든다..
		hErr = opl_broadcast_getCurrentMasterService (hBcast->ulViewId, &stMasterSvc);
		if (BPL_STATUS_OK != hErr)
		{
			HxLOG_Error("Fail to Get Current Master Service.\n");
			HxSTD_MemSet (&stMasterSvc, 0, sizeof(DxService_t));
		}

		hErr = opl_broadcast_getCurrentSupplementaryService (hBcast->ulViewId, &stService);
		if (BPL_STATUS_OK != hErr)
		{
			HxLOG_Error("Fail to Get Current Supplement Service.\n");
			HxSTD_MemCopy (&stService, &stMasterSvc, sizeof(DxService_t));
		}

		hErr = opl_broadcast_StartRec( hBcast, channel, 0, ulDurationSec, eOxMR_TargetStorage_IntHdd, NULL, &(hBcast->ulRecSessionId) , pszRecordID);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("[%s:%d] Error\n", __FUNCTION__, __LINE__);
			return BPL_STATUS_ERROR;
		}

		hBcast->eRecState = OPLRECORDER_REC_STATE_REC_PRESTART;
	}
	else
	{
		HxLOG_Error ("[%s:%d] Not Record!!! eSessionState=%d ePlayState=%d\n", __FUNCTION__, __LINE__, hBcast->eSessionState, hBcast->ePlayState);
		return BPL_STATUS_ERROR;
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_RecordNowAt (OplBroadcast_t *hBroadcast, OPL_Channel_t channel, unsigned int ulDurationSec, unsigned int ulTarget, char *pszRecordID)
{
	xOplBroadcast_t						*hBcast = xbroadcast(hBroadcast);
	DxService_t						 stService;
	HERROR								 hErr;

	if (NULL != pszRecordID)
	{
		pszRecordID[0] = '\0';
	}

	HxSTD_MemSet (&stService, 0, sizeof(DxService_t));
	stService.uid = OPL_Channel_GetUniqueID(channel);

	hErr = opl_broadcast_StartRec( hBcast, channel, 0, ulDurationSec, ulTarget, NULL, &(hBcast->ulRecSessionId) , pszRecordID);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("[%s:%d] Error\n", __FUNCTION__, __LINE__);
		return BPL_STATUS_ERROR;
	}

	hBcast->eRecState = OPLRECORDER_REC_STATE_REC_PRESTART;

	return BPL_STATUS_OK;
}
BPLStatus OPL_Broadcast_RecordDelayed (OplBroadcast_t *hBroadcast, OPL_Channel_t channel, unsigned int ulStartPos, unsigned int ulDurationSec, char *pszRecordID)
{
	xOplBroadcast_t			*hBcast = xbroadcast(hBroadcast);
	DxService_t				stService;
	HERROR					 hErr;
	HUINT32 				ulMaxOffset = 0;

	if (NULL != pszRecordID)
	{
		pszRecordID[0] = '\0';
	}

	HxSTD_MemSet (&stService, 0, sizeof(DxService_t));
	stService.uid = OPL_Channel_GetUniqueID(channel);


	if(ulStartPos == 0)
	{
		//TSR 의 현재 Offset 가져옴....
		OPL_Broadcast_GetMaxOffset(hBroadcast, &ulMaxOffset);
		// TSR Full이 아니라면 항상 0이다....
		if(ulMaxOffset>=10)
		{
			// TSR 이 10초 이상 저장된 상태면...Delayed ... Web에서 처리중이지만...한번 더...
			ulStartPos = 1;
		}
	}

	hErr = opl_broadcast_StartRec( hBcast, channel, (HINT32)ulStartPos, ulDurationSec, eOxMR_TargetStorage_IntHdd, NULL, &(hBcast->ulRecSessionId) , pszRecordID);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("[%s:%d] Error\n", __FUNCTION__, __LINE__);
		return BPL_STATUS_ERROR;
	}

	hBcast->eRecState = OPLRECORDER_REC_STATE_REC_PRESTART;

	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_RecordDelayedAt (OplBroadcast_t *hBroadcast, OPL_Channel_t channel, unsigned int ulStartPos, unsigned int ulDurationSec, unsigned int ulTarget, char *pszRecordID)
{
	xOplBroadcast_t			*hBcast = xbroadcast(hBroadcast);
	DxService_t				stService;
	HERROR					 hErr;
	HUINT32 				ulMaxOffset = 0;

	if (NULL != pszRecordID)
	{
		pszRecordID[0] = '\0';
	}

	HxSTD_MemSet (&stService, 0, sizeof(DxService_t));
	stService.uid = OPL_Channel_GetUniqueID(channel);

	//TSR 의 현재 Offset 가져옴....
	OPL_Broadcast_GetMaxOffset(hBroadcast, &ulMaxOffset);
	if(ulStartPos == 0)
	{
		// TSR Full이 아니라면 항상 0이다....
		if(ulMaxOffset>=10)
		{
			// TSR 이 10초 이상 저장된 상태면...Delayed ... Web에서 처리중이지만...한번 더...
			ulStartPos = 1;
		}
	}

	hErr = opl_broadcast_StartRec( hBcast, channel, (HINT32)ulStartPos, ulDurationSec, ulTarget, NULL, &(hBcast->ulRecSessionId) , pszRecordID);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("[%s:%d] Error\n", __FUNCTION__, __LINE__);
		return BPL_STATUS_ERROR;
	}

	hBcast->eRecState = OPLRECORDER_REC_STATE_REC_PRESTART;

	return BPL_STATUS_OK;
}
BPLStatus OPL_Broadcast_StopRecording (OplBroadcast_t *hBroadcast, OPL_Channel_t channel)
{
	xOplBroadcast_t			*hBcast = xbroadcast(hBroadcast);
	DxService_t			 stService;
	HERROR					 hErr;

	HxSTD_MemSet (&stService, 0, sizeof(DxService_t));
	stService.uid = OPL_Channel_GetUniqueID(channel);

	hErr = APK_MEDIA_RECORD_Stop (hBcast->ulRecSessionId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("[%s:%d] Error\n", __FUNCTION__, __LINE__);
		return BPL_STATUS_ERROR;
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_GetRecordId (OplBroadcast_t *hBroadcast, char *pszRecordID)
{
	xOplBroadcast_t			*hBcast = xbroadcast(hBroadcast);
	DxService_t			 stService;
	HERROR					 hErr;

	if (NULL == pszRecordID)
	{
		HxLOG_Error ("[%s:%d] Error\n", __FUNCTION__, __LINE__);
		return BPL_STATUS_ERROR;
	}

	if (BPL_STATUS_OK != opl_broadcast_getCurrentMasterService (hBcast->ulViewId, &stService))
	{
		HxLOG_Error ("[%s:%d] Error\n", __FUNCTION__, __LINE__);
		return BPL_STATUS_ERROR;
	}

	hErr = APK_MEDIA_RECORD_GetURL (hBcast->ulRecSessionId, (HCHAR *)pszRecordID);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("[%s:%d] Error\n", __FUNCTION__, __LINE__);
		return BPL_STATUS_ERROR;
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_GetRecordingState (OplBroadcast_t *hBroadcast, eOplRecorderState *peState)
{
	xOplBroadcast_t						*hBcast = xbroadcast(hBroadcast);

	if ((NULL == hBcast) || (NULL == peState))
	{
		HxLOG_Error("hBcast:0x%08x, peState:0x%08x\n", (HUINT32)hBcast, (HUINT32)peState);
		return BPL_STATUS_ERROR;
	}

	*peState = hBcast->eRecState;
	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_ControlLive (OplBroadcast_t *hBroadcast, unsigned int bControl)
{

	HERROR hErr = (!bControl) ? APK_MEDIA_PLAY_MemorizeMediaState (xbroadcast(hBroadcast)->ulViewId, TRUE)\
		: APK_MEDIA_PLAY_RestoreMediaState (xbroadcast(hBroadcast)->ulViewId);

	return (hErr == ERR_OK ) ? BPL_STATUS_OK : BPL_STATUS_ERROR;

	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_GetCasMessage(OplBroadcast_t *hBroadcast, unsigned int *pzModule, unsigned int *pzSlot, unsigned int *pulSession, char **ppszMessage, unsigned int *pulMessageLength)
{
	BPLStatus				eStatus = BPL_STATUS_OK;
	xOplBroadcast_t			*hBcast = xbroadcast(hBroadcast);
	DxCAS_MMI_Event_t		*pstEvent = NULL;

	if (NULL == hBcast)
	{
		HxLOG_Error("hBcast:0x%08x\n", (HUINT32)hBcast);
		return BPL_STATUS_ERROR;
	}

	(*pzModule) = 0;
	(*pzSlot) = 0;
	(*pulSession) = (unsigned int)hBcast->hCasMsgSession;

	APK_CAS_MMI_GetUiEvent(eDxCAS_SESSIONTYPE_BANNER, hBcast->hCasMsgSession, &pstEvent);
	if(pstEvent == NULL)
	{
		eStatus = BPL_STATUS_ERROR;
		(*ppszMessage) = NULL;
	}
	else
	{
		(*pzModule) = pstEvent->eModule;
		(*pzSlot) = pstEvent->nSlot;

		if(pstEvent->pszEncodedUiEvent == NULL && pstEvent->nEncodedMessageLength > 0)
		{
			(*ppszMessage) = NULL;
			(*pulMessageLength) = 0;
		}
		else
		{
			(*ppszMessage) = (char*)HLIB_STD_MemDup(pstEvent->pszEncodedUiEvent, pstEvent->nEncodedMessageLength);
			(*pulMessageLength) = pstEvent->nEncodedMessageLength;
		}

		APK_CAS_MMI_RemoveUiEvent(eDxCAS_SESSIONTYPE_BANNER, pstEvent);
	}

	return eStatus;
}

BPLStatus OPL_Broadcast_GetCasUiEvtInfo(OplBroadcast_t *hBroadcast, unsigned int *pzEvt, unsigned int *pzModule, unsigned int *pzSlot, unsigned int *pulSession)
{
	xOplBroadcast_t				*hBcast = xbroadcast(hBroadcast);
	DxCAS_MMI_Event_t			*pstEvent = NULL;

	if (NULL == hBcast)
	{
		HxLOG_Error("hBcast:0x%08x\n", (HUINT32)hBcast);
		return BPL_STATUS_ERROR;
	}

	if((pzEvt == NULL) || (pzModule == NULL) || (pzSlot == NULL) || (pulSession == NULL))
	{
		return BPL_STATUS_ERROR;
	}

	APK_CAS_MMI_GetEvtInfo(&pstEvent);
	if(pstEvent != NULL)
	{
		(*pzEvt) = pstEvent->eventType;
		(*pzModule) = pstEvent->eModule;
		(*pzSlot) = pstEvent->nSlot;
		(*pulSession) = pstEvent->hSession;
		APK_CAS_MMI_RemoveEvtInfo(pstEvent);
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_ReleaseCasMessage(OplBroadcast_t *hBroadcast, char *pszMessage)
{
	xOplBroadcast_t 	*hBcast = xbroadcast(hBroadcast);

	if (NULL == hBcast)
	{
		HxLOG_Error("hBcast:0x%08x\n", (HUINT32)hBcast);
		return BPL_STATUS_ERROR;
	}

	if(pszMessage != NULL)
	{
		HLIB_STD_MemFree(pszMessage);
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_GetDrmError(OplBroadcast_t *hBroadcast, HUINT32 *pulDrmError, HCHAR **ppszCaSystemId)
{
#if defined(CONFIG_OP_JAPAN)
	xOplBroadcast_t				*hBcast = xbroadcast(hBroadcast);
	APKS_CASBC_DrmData_t	*pstEvent = NULL;
	HERROR						hErr = ERR_FAIL;
	BPLStatus					bplStatus = BPL_STATUS_ERROR;
	HxLOG_Debug("+ [%s:%d]\n", __FUNCTION__, __LINE__);
	if ((NULL == hBcast) || (pulDrmError == NULL) || (ppszCaSystemId == NULL))
	{
		HxLOG_Debug("-[%s:%d]\n", __FUNCTION__, __LINE__);
		return BPL_STATUS_ERROR;
	}

	(*ppszCaSystemId) = NULL;

	hErr = APK_CAS_BC_GetDrm(hBcast->ulViewId, &pstEvent);
	if((hErr == ERR_OK) && (pstEvent != NULL))
	{
		*pulDrmError = pstEvent->ulError;
		if(pstEvent->pszCaSystemId != NULL)
		{
			(*ppszCaSystemId) = HLIB_STD_StrDup((HCHAR *)pstEvent->pszCaSystemId);
		}
		HxLOG_Print("ppszCaSystemId %s [%s:%d]\n",*ppszCaSystemId,__FUNCTION__, __LINE__);
		HxLOG_Print("*pulDrmError 0x%x [%s:%d]\n", *pulDrmError,__FUNCTION__, __LINE__);
		bplStatus = BPL_STATUS_OK;
	}
	else
	{

		bplStatus = BPL_STATUS_ERROR;
	}
	if(pstEvent != NULL)
	{
		APK_CAS_BC_FreeDrm(&pstEvent);
	}
	HxLOG_Debug("-[%s:%d]\n", __FUNCTION__, __LINE__);
	return bplStatus;
#else
	return BPL_STATUS_OK;
#endif
}

BPLStatus OPL_Broadcast_ReleaseDrmError(OplBroadcast_t *hBroadcast, HCHAR *pszCaSystemId)
{
#if defined(CONFIG_OP_JAPAN)
	xOplBroadcast_t 	*hBcast = xbroadcast(hBroadcast);
	HxLOG_Debug("+ [%s:%d] \n",__FUNCTION__,__LINE__);

	if (NULL == hBcast)
	{
		HxLOG_Error("hBcast:0x%08x [%s:%d] \n", (HUINT32)hBcast,__FUNCTION__,__LINE__ );
		return BPL_STATUS_ERROR;
	}

	if(pszCaSystemId != NULL)
	{
		HLIB_STD_MemFree(pszCaSystemId);
	}
	HxLOG_Debug("- [%s:%d] \n",__FUNCTION__,__LINE__);
#endif
	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_GetDrmRightError(OplBroadcast_t *hBroadcast, HUINT32 *pulErrorState,
												HCHAR** ppszContentId, HCHAR **ppszCaSystemId,
												HCHAR **ppszRightsIssuerURL)
{
#if defined(CONFIG_OP_JAPAN)
{
	xOplBroadcast_t				*hBcast = xbroadcast(hBroadcast);
	APKS_CASBC_DrmData_t	*pstEvent = NULL;
	HERROR						hErr = ERR_FAIL;
	BPLStatus					bplStatus = BPL_STATUS_ERROR;
	HxLOG_Debug("+ [%s:%d]\n", __FUNCTION__, __LINE__);
	if ((NULL == hBcast) || (pulErrorState == NULL) || (ppszCaSystemId == NULL)
	||(ppszContentId == NULL) ||(ppszRightsIssuerURL == NULL))
	{
		HxLOG_Debug("-[%s:%d]\n", __FUNCTION__, __LINE__);
		return BPL_STATUS_ERROR;
	}

	(*ppszCaSystemId) = NULL;
	(*ppszContentId) = NULL;
	(*ppszRightsIssuerURL) = NULL;

	hErr = APK_CAS_BC_GetDrm(hBcast->ulViewId, &pstEvent);
	if((hErr == ERR_OK) && (pstEvent != NULL))
	{
		*pulErrorState = pstEvent->ulErrorState;
		if(pstEvent->pszCaSystemId != NULL)
		{
			(*ppszCaSystemId) = HLIB_STD_StrDup((HCHAR *)pstEvent->pszCaSystemId);
		}
		if(pstEvent->pszContentId != NULL)
		{
			(*ppszContentId) = HLIB_STD_StrDup((HCHAR *)pstEvent->pszContentId);
		}
		if(pstEvent->pszRightsIssuerURL != NULL)
		{
			(*ppszRightsIssuerURL) = HLIB_STD_StrDup((HCHAR *)pstEvent->pszRightsIssuerURL);
		}
		HxLOG_Print("ppszCaSystemId %s [%s:%d]\n",*ppszCaSystemId,__FUNCTION__, __LINE__);
		HxLOG_Print("ppszContentId %s [%s:%d]\n",*ppszContentId,__FUNCTION__, __LINE__);
		HxLOG_Print("ppszRightsIssuerURL %s [%s:%d]\n",*ppszRightsIssuerURL,__FUNCTION__, __LINE__);
		HxLOG_Print("*pulErrorState %d [%s:%d]\n", *pulErrorState,__FUNCTION__, __LINE__);
		bplStatus = BPL_STATUS_OK;
	}
	else
	{

		bplStatus = BPL_STATUS_ERROR;
	}
	if(pstEvent != NULL)
	{
		APK_CAS_BC_FreeDrm(&pstEvent);
	}
	HxLOG_Debug("-[%s:%d]\n", __FUNCTION__, __LINE__);
	return bplStatus;
}
#elif  defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE) || defined(CONFIG_PROD_HMS1000T) || defined(CONFIG_PROD_DIGIMAXT2)
{
	xOplBroadcast_t				*hBcast = xbroadcast (hBroadcast);
	if ((NULL == hBcast) || (pulErrorState == NULL) || (ppszCaSystemId == NULL)
	||(ppszContentId == NULL) ||(ppszRightsIssuerURL == NULL))
	{
		HxLOG_Debug("-[%s:%d]\n", __FUNCTION__, __LINE__);
		return BPL_STATUS_ERROR;
	}
	/*contentID는 어떤 형태로 쓰는지 모르겠음. 필요한 경우 만들어서 쓰세요*/
	*ppszContentId = NULL;
	*pulErrorState = hBcast->stDrmRightsStatus.errorState;
	if(hBcast->stDrmRightsStatus.drmSystemId != NULL)
	{
		(*ppszCaSystemId) = HLIB_STD_StrDup((HCHAR *)hBcast->stDrmRightsStatus.drmSystemId);
	}
	if(hBcast->stDrmRightsStatus.contentId != NULL)
	{
		(*ppszContentId) = HLIB_STD_StrDup((HCHAR *)hBcast->stDrmRightsStatus.contentId);
	}
	if(hBcast->stDrmRightsStatus.rightsIssuerUrl != NULL)
	{
		(*ppszRightsIssuerURL) = HLIB_STD_StrDup((HCHAR *)hBcast->stDrmRightsStatus.rightsIssuerUrl);
	}
	HxLOG_Print("ppszCaSystemId %s [%s:%d]\n",*ppszCaSystemId,__FUNCTION__, __LINE__);
	HxLOG_Print("ppszContentId %s [%s:%d]\n",*ppszContentId,__FUNCTION__, __LINE__);
	HxLOG_Print("ppszRightsIssuerURL %s [%s:%d]\n",*ppszRightsIssuerURL,__FUNCTION__, __LINE__);
	HxLOG_Print("*pulErrorState %d [%s:%d]\n", *pulErrorState,__FUNCTION__, __LINE__);
	return BPL_STATUS_OK;
}
#else
	return BPL_STATUS_OK;
#endif
}

BPLStatus OPL_Broadcast_ReleaseDrmRightError(OplBroadcast_t *hBroadcast, HCHAR *pszContentId,
												HCHAR *pszCaSystemId, HCHAR *pszRightsIssuerURL)
{
#if defined(CONFIG_OP_JAPAN) || defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE) || defined(CONFIG_PROD_HMS1000T) || defined(CONFIG_PROD_DIGIMAXT2)
	xOplBroadcast_t 	*hBcast = xbroadcast(hBroadcast);
	HxLOG_Debug("+ [%s:%d] \n",__FUNCTION__,__LINE__);

	if (NULL == hBcast)
	{
		HxLOG_Error("hBcast:0x%08x [%s:%d] \n", (HUINT32)hBcast,__FUNCTION__,__LINE__ );
		return BPL_STATUS_ERROR;
	}

	if(pszContentId != NULL)
	{
		HLIB_STD_MemFree(pszContentId);
	}
	if(pszCaSystemId != NULL)
	{
		HLIB_STD_MemFree(pszCaSystemId);
	}
	if(pszRightsIssuerURL != NULL)
	{
		HLIB_STD_MemFree(pszRightsIssuerURL);
	}
	HxLOG_Debug("- [%s:%d] \n",__FUNCTION__,__LINE__);
#endif
	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_GetNewMailMsg(OplBroadcast_t *hBroadcast, OPL_MailHandle *hMail)
{
	xOplBroadcast_t 				*hBcast = xbroadcast(hBroadcast);
	BPLStatus						bplStatus = BPL_STATUS_ERROR;

	if( hBcast && hBcast->hMail )
	{
		*hMail = hBcast->hMail;
		bplStatus = BPL_STATUS_OK;
	}

	return bplStatus;
}

BPLStatus OPL_Broadcast_GetAutoDispMessage(OplBroadcast_t *hBroadcast, OPL_AutoDispMsg_t **pphAutoDispMsg)
{
#if defined(CONFIG_OP_JAPAN)
	xOplBroadcast_t 				*hBcast = xbroadcast(hBroadcast);
	APKS_CASBC_BsAutoMsgData_t 	*pstAutoDispMsg = NULL;
	HERROR						hErr = ERR_FAIL;
	BPLStatus					bplStatus = BPL_STATUS_ERROR;
	HxLOG_Debug("+ [%s:%d]\n", __FUNCTION__, __LINE__);
	if ((NULL == hBcast) || (pphAutoDispMsg == NULL))
	{
		HxLOG_Debug("-[%s:%d]\n", __FUNCTION__, __LINE__);
		bplStatus = BPL_STATUS_ERROR;
		goto funcExit;
	}

	hErr = APK_CAS_BC_GetAutoDispMsg(hBcast->ulViewId, &pstAutoDispMsg);
	if((hErr == ERR_OK) && (pstAutoDispMsg != NULL))
	{
		*pphAutoDispMsg = (OPL_AutoDispMsg_t *)pstAutoDispMsg;
		bplStatus = BPL_STATUS_OK;
	}
	else
	{
		bplStatus = BPL_STATUS_ERROR;
	}
funcExit:
	HxLOG_Debug("-[%s:%d]\n", __FUNCTION__, __LINE__);
	return bplStatus;
#else
	return BPL_STATUS_OK;
#endif

}

BPLStatus OPL_Broadcast_GetCaLinkMessage(OplBroadcast_t *hBroadcast, HUINT32 *pOnId, HUINT32 *pTsId, HUINT32 *pSvcId, HUINT8 **ppszCaLinkMsg)
{
#if defined(CONFIG_OP_JAPAN)
	xOplBroadcast_t 				*hBcast = xbroadcast(hBroadcast);
	HERROR						hErr = ERR_FAIL;
	BPLStatus					bplStatus = BPL_STATUS_ERROR;
	HxLOG_Debug("+ [%s:%d]\n", __FUNCTION__, __LINE__);
	if ((NULL == hBcast) || (pOnId == NULL) || (pTsId == NULL) || (pSvcId == NULL) || (ppszCaLinkMsg == NULL))
	{
		HxLOG_Error("invalid param -[%s:%d]\n", __FUNCTION__, __LINE__);
		bplStatus = BPL_STATUS_ERROR;
		goto funcExit;
	}

	hErr = APK_CAS_BC_GetCaLinkMsg(hBcast->ulViewId, pOnId, pTsId, pSvcId, ppszCaLinkMsg);
	if(hErr == ERR_OK)
	{
		bplStatus = BPL_STATUS_OK;
	}
	else
	{
		bplStatus = BPL_STATUS_ERROR;
	}
funcExit:
	HxLOG_Debug("-[%s:%d]\n", __FUNCTION__, __LINE__);
	return bplStatus;
#else
	return BPL_STATUS_OK;
#endif

}


BPLStatus OPL_Broadcast_GetProgrammes (OplBroadcast_t *hBroadcast, int *count, OPL_ProgrammeHandle **programmes)
{
	xOplBroadcast_t	*self = xbroadcast(hBroadcast);
	int	i, c;

	if (self == NULL)
		return BPL_STATUS_ERROR;

#if defined( CONFIG_OP_JAPAN )
	if (self->nPFLen < 1)		/* 일본향 미정 이라는 방송 type 때문에 following이 없을 수 도 있다. */
#else
	if (self->nPFLen < 2)
#endif
	{
		DxService_t service;

		if (opl_broadcast_getCurrentMasterService(self->ulViewId, &service) == BPL_STATUS_OK)
		{
			OPL_METADATASEARCH_SearchPF(NULL, service.uid, (OPL_SearchPfMonitor_t)opl_broadcast_PfMonitor, (void *)self);
			if (self->nPFLen == 0)
				return BPL_STATUS_ERROR;
			}
		}

	*programmes = (OPL_ProgrammeHandle *)HLIB_STD_MemAlloc(sizeof(OPL_ProgrammeHandle) * self->nPFLen);
	if (*programmes == NULL)
		return BPL_STATUS_ERROR;

	for (i = 0, c = 0 ; i < self->nPFLen ; i++)
	{
		(*programmes)[c] = OPL_Programme_Clone(self->ahPF[i]);
		if ((*programmes)[c])
			c++;
	}
	if (c == 0)
	{
		HLIB_STD_MemFree(*programmes);
		return BPL_STATUS_ERROR;
	}
	*count = c;
	return BPL_STATUS_OK;
}


STATIC HERROR opl_broadcast_CheckChannelChangable (OplBroadcast_t *hBroadcast, OPL_Channel_t channel, HBOOL *pbChangable)
{
	xOplBroadcast_t				*hBcast = xbroadcast (hBroadcast);
	DxService_t					*pstSvc = (DxService_t *)channel;
	OxMediaPlay_StartInfo_t 	unStart = {0,};

	HERROR						 hErr;

	if ((NULL == hBcast) || (NULL == pstSvc) || (NULL == pbChangable))
	{
		HxLOG_Error ("The handle of broadcast player is NULL.\n");
		return ERR_FAIL;
	}

	*pbChangable = FALSE;

	unStart.eType	=	eOxMP_MEDIATYPE_LIVE;
	unStart.stLive.ulMasterSvcUid	=	pstSvc->uid;
	unStart.stLive.ulSuppleSvcUid	=	pstSvc->uid;

	hErr = APK_MEDIA_PLAY_CheckPlayChangable (hBcast->ulViewId, eOxMP_MEDIATYPE_LIVE, &unStart, pbChangable);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_MEDIA_PLAY_CheckPlayChangable err: ulViewId(0x%04x), svcuid(%d).\n", hBcast->ulViewId, pstSvc->uid);
		return ERR_FAIL;
	}
	return ERR_OK;
}

STATIC HERROR opl_broadcast_CheckChannelRecordable (OplBroadcast_t *hBroadcast, OPL_Channel_t channel, HBOOL bIgnoreLive, HBOOL *pbChangable)
{
	HUINT32						 ulRecActId = 0xFFFF;
	HUINT32						 ulCheckActId;
	HUINT32						 ulIgnorableActPriority;
	DxService_t					*pstSvc = (DxService_t *)channel;
	Handle_t					 hReqHandle = (Handle_t)NULL;
	OxRsc_InitialInfo_t			 stInitInfo;
	RxTrialArg_t				 unTrialArg;
	DxTransponder_t				*pstTs = NULL;
	HERROR						 hErr, hResult = ERR_FAIL;

	if ((NULL == pstSvc) || (NULL == pbChangable))
	{
		HxLOG_Error("argument is NULL.\n");
		goto END_FUNC;
	}

	*pbChangable = FALSE;

	hErr = APK_RSC_GetInitialInfo(&stInitInfo);
	if ((ERR_OK != hErr) || (0 == stInitInfo.ulTotalActionNum))
	{
		HxLOG_Error("APK_RSC_GetInitialInfo err\n");
		goto END_FUNC;
	}

	ulIgnorableActPriority = (TRUE == bIgnoreLive) ? stInitInfo.ulRecActPriority : stInitInfo.ulViewActPriority;

	hErr = APK_RSC_GetEmptyRecordActionId(&ulRecActId);
	if ((ERR_OK != hErr) || (ulRecActId >= stInitInfo.ulTotalActionNum))
	{
		HxLOG_Error("APK_RSC_GetEmptyRecordActionId err:\n");
		hResult = ERR_OK;
		goto END_FUNC;
	}

	pstTs = APK_META_SVC_GetTransponder(pstSvc->tsuid);
	if (NULL == pstTs)
	{
		HxLOG_Error("APK_META_SVC_GetTransponder err: tsuid(%d)\n", pstSvc->tsuid);
		goto END_FUNC;
	}

	hReqHandle = APK_RSC_MakeResourceRequestHandle(ulRecActId, stInitInfo.ulRecActPriority, FALSE);
	if ((Handle_t)NULL == hReqHandle)
	{
		HxLOG_Error("APK_RSC_MakeResourceRequestHandle err: ulRecActId(%d), ulRecPriority(%d)\n", ulRecActId, stInitInfo.ulRecActPriority);
		goto END_FUNC;
	}

#if 0
	// Set RF-Input:
	if (eDxDELIVERY_TYPE_SAT == pstTs->deliType)
	{
		hErr = APK_RSC_AddRequiredResourceToRequestHandle(hReqHandle, eRxRSCTYPE_RFINPUT, RxRSCID_DYNAMIC, TRUE, eRxUSAGETYPE_NONE);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("APK_RSC_AddRequiredResourceToRequestHandle eRxRSCTYPE_RFINPUT err:\n");
		}
	}
#endif

	// Set Tuner
	hErr = APK_RSC_AddRequiredResourceToRequestHandle(hReqHandle, eRxRSCTYPE_TUNER_GROUP, RxRSCID_DYNAMIC, TRUE, eRxUSAGETYPE_NONE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("APK_RSC_AddRequiredResourceToRequestHandle eRxRSCTYPE_TUNER_GROUP err:\n");
	}

#if 0
	// Set Record
	hErr = APK_RSC_AddRequiredResourceToRequestHandle(hReqHandle, eRxRSCTYPE_HDD_REC, RxRSCID_DYNAMIC, TRUE, eRxUSAGETYPE_NONE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("APK_RSC_AddRequiredResourceToRequestHandle eRxRSCTYPE_HDD_REC err:\n");
	}
#endif

	// Set Tuning Parameter
	unTrialArg.eArgType = eRxARGTYPE_TUNEPARAM;
	unTrialArg.stTuneParam.stTuneParam = pstTs->tuningParam;
	hErr = APK_RSC_AddArgumentToRequestHandle(hReqHandle, &unTrialArg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("APK_RSC_AddArgumentToRequestHandle err:\n");
	}

	// Set the removable (ignorable) actions:
	for (ulCheckActId = 0; ulCheckActId < stInitInfo.ulTotalActionNum; ulCheckActId++)
	{
		HUINT32				 ulCurrActPriority = 0;

		hErr = APK_RSC_GetActionPriority(ulCheckActId, &ulCurrActPriority);
		if ((ERR_OK == hErr) && (ulCurrActPriority < ulIgnorableActPriority))
		{
			hErr = APK_RSC_AddIgnorableActionIdToRequestHandle(hReqHandle, ulCheckActId);
			if (ERR_OK != hErr)
			{
				HxLOG_Error("APK_RSC_AddRemovableActionIdToRequestHandle: actionId(%d), priority(%d)\n", ulCheckActId, ulCurrActPriority);
			}
		}
	}

	hErr = APK_RSC_TryResourceRequest(hReqHandle);
	*pbChangable = (ERR_OK == hErr) ? TRUE : FALSE;

	hResult = ERR_OK;

END_FUNC:
	if ((Handle_t)NULL != hReqHandle)
	{
		hErr = APK_RSC_DestroyResourceRequestHandle(hReqHandle);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("APK_RSC_DestroyResourceRequestHandle err:\n");
		}
	}

	if (NULL != pstTs)
	{
		APK_META_SVC_Delete(pstTs);
	}

	return hResult;
}

BPLStatus OPL_Broadcast_CheckAitSection(OplBroadcast_t *hBroadcast)
{
	xOplBroadcast_t 			*hBcast = xbroadcast (hBroadcast);

	HxLOG_Trace();

	if (NULL == hBcast)
	{
		HxLOG_Error ("The handle of broadcast player is NULL.\n");
		return BPL_STATUS_ERROR;
	}

	APK_MEDIA_PLAY_CheckAitSection(hBcast->ulViewId);
	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_CheckChannelChangable (OplBroadcast_t *hBroadcast, OPL_Channel_t channel, HBOOL *pbChangable)
{
	return (ERR_OK == opl_broadcast_CheckChannelChangable(hBroadcast, channel, pbChangable))
		? BPL_STATUS_OK : BPL_STATUS_ERROR;
}

BPLStatus OPL_Broadcast_CheckChannelRecordable (OplBroadcast_t *hBroadcast, OPL_Channel_t channel, HBOOL bIgnoreLive, HBOOL *pbChangable)
{
	return (ERR_OK == opl_broadcast_CheckChannelRecordable(hBroadcast, channel, bIgnoreLive, pbChangable))
		? BPL_STATUS_OK : BPL_STATUS_ERROR;
}

BPLStatus OPL_Broadcast_GetEventRelayChannel (OplBroadcast_t *hBroadcast, OPL_Channel_t *eventRelayChannel)
{
	xOplBroadcast_t				*hBcast = xbroadcast (hBroadcast);
	OPL_Channel_t					relayChannel = NULL;

	relayChannel = OPL_Channel_FindChannelByUID( NULL , hBcast->eventRelaySvcUid );

	if (!relayChannel)
	{
		return BPL_STATUS_ERROR;
	}
	else
	{
		*eventRelayChannel = (OPL_Channel_t)OPL_Channel_Clone(relayChannel);
	}
	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_GetEwsChannel (OplBroadcast_t *hBroadcast, OPL_Channel_t *ewsChannel)
{
	xOplBroadcast_t				*hBcast = xbroadcast (hBroadcast);
	OPL_Channel_t					oplEwsChannel = NULL;

	oplEwsChannel = OPL_Channel_FindChannelByUID( NULL , hBcast->ewsSvcUid );

	if (!oplEwsChannel)
	{
		return BPL_STATUS_ERROR;
	}
	else
	{
		*ewsChannel = (OPL_Channel_t)OPL_Channel_Clone(oplEwsChannel);
	}
	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_GetEwsTuner (OplBroadcast_t *hBroadcast, HINT32 *tunerId)
{
	xOplBroadcast_t				*hBcast = xbroadcast (hBroadcast);
	OPL_Channel_t					oplCurChannel = NULL;
	BPLStatus						result = BPL_STATUS_ERROR;
	HINT32						onId = 0, tsId = 0, svcId = 0;

	oplCurChannel = OPL_Channel_FindChannelByUID( NULL , hBcast->ewsCurSvcUid);

	if (oplCurChannel)
	{
		onId = OPL_Channel_GetOrgNetworkID(oplCurChannel);
		tsId = OPL_Channel_GetTransponderID(oplCurChannel);
		svcId = OPL_Channel_GetServiceID(oplCurChannel);

		*tunerId = -1;
		result = OPL_Tuner_GetTunerIdByTripleId(onId, tsId, svcId, tunerId);
	}

	return result;
}

BPLStatus OPL_Broadcast_GetBmlStartFlag (OplBroadcast_t *hBroadcast, HBOOL *pbAutoFlag)
{
	xOplBroadcast_t				*hBcast = xbroadcast (hBroadcast);

	*pbAutoFlag = 0;
	NOT_USED_PARAM(hBcast);
	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_GetBmlEndParam (OplBroadcast_t *hBroadcast, int *pnCause, char *pcParam)
{
	xOplBroadcast_t				*hBcast = xbroadcast (hBroadcast);

	*pnCause = 0;
	*pcParam = '\0';
	NOT_USED_PARAM(hBcast);
	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_ShowVideoMessage (OplBroadcast_t *hBroadcast, int type)
{
	xOplBroadcast_t *hBcast = xbroadcast(hBroadcast);
	if ( hBcast == NULL )
	{
		HxLOG_Error("The handle of broadcast player is NULL.\n");
		return BPL_STATUS_ERROR;
	}

	if (ERR_OK != APK_MEDIA_PLAY_SetVideoHide (hBcast->ulViewId, TRUE))
	{
		HxLOG_Error("APK_MEDIA_PLAY_SetVideoHide ret Error!!!!\n");
		return BPL_STATUS_ERROR;
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_HideVideoMessage (OplBroadcast_t *hBroadcast, int type)
{
	xOplBroadcast_t *hBcast = xbroadcast(hBroadcast);
	if ( hBcast == NULL )
	{
		HxLOG_Error("The handle of broadcast player is NULL.\n");
		return BPL_STATUS_ERROR;
	}

	if (ERR_OK != APK_MEDIA_PLAY_SetVideoHide (hBcast->ulViewId, FALSE))
	{
		HxLOG_Error("APK_MEDIA_PLAY_SetVideoHide ret Error!!!!\n");
		return BPL_STATUS_ERROR;
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_StartBML (OplBroadcast_t *hBroadcast)
{
	xOplBroadcast_t				*hBcast = xbroadcast (hBroadcast);
	NOT_USED_PARAM(hBcast);

	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_EndBML (OplBroadcast_t *hBroadcast)
{
	xOplBroadcast_t				*hBcast = xbroadcast (hBroadcast);
	NOT_USED_PARAM(hBcast);

	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_SetBMLKeyControlMode (OplBroadcast_t *hBroadcast, HBOOL permit, int *retVal)
{
	xOplBroadcast_t				*hBcast = xbroadcast (hBroadcast);
	NOT_USED_PARAM(hBcast);
#ifndef EMUL
//	APK_EXE_BML_Set(permit);
#endif
	*retVal = 0; // 실패할 이유가 없다. octo 가 죽어서?
	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_GetBmlHandle(OplBroadcast_t *hBroadcast, OPL_HANDLE *hBml)
{
	xOplBroadcast_t 				*hBcast = xbroadcast(hBroadcast);
	BPLStatus					bplStatus = BPL_STATUS_ERROR;

	if ( hBcast == NULL )
	{
		HxLOG_Error("The handle of broadcast player is NULL.\n");
		return bplStatus;
	}

	if( hBcast && hBcast->pcBMLName )
	{
		*hBml = hBcast->pcBMLName;	// BML Hnadle을  BMLNAME으로 변 경.
		bplStatus = BPL_STATUS_OK;
	}

	return bplStatus;
}

BPLStatus OPL_Broadcast_GetBmlVodName (OplBroadcast_t *hBroadcast, char **pcName)
{
	xOplBroadcast_t				*hBcast = xbroadcast (hBroadcast);
#ifndef EMUL
	*pcName = hBcast->pcBMLName;
#endif
	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_GetBmlVodParam (OplBroadcast_t *hBroadcast, char **pcParam)
{
	xOplBroadcast_t				*hBcast = xbroadcast (hBroadcast);
#ifndef EMUL
	*pcParam = hBcast->pcBMLParam;
#endif
	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_GetParentalRatingChangeStatus (OplBroadcast_t *hBroadcast,
																char **ppszContentId,
																HBOOL *pbBlocked,
																unsigned int *pRating,
																char **ppszSystemId)
{
	xOplBroadcast_t				*hBcast = xbroadcast (hBroadcast);
	if((ppszContentId == NULL) || (pbBlocked == NULL) || (pRating == NULL) ||(ppszSystemId == NULL))
	{
		return BPL_STATUS_ERROR;
	}
	/*contentID는 어떤 형태로 쓰는지 모르겠음. 필요한 경우 만들어서 쓰세요*/
	*ppszContentId = NULL;
	*pbBlocked = hBcast->stParentalRatingStatus.bBlocked;
	*pRating = hBcast->stParentalRatingStatus.ratingValue;
#if defined(CONFIG_OP_JAPAN)
	/*ppszSystemId will be free*/
	APK_CAS_BC_MakeDrmSystemIdStr(ppszSystemId, hBcast->stParentalRatingStatus.caSystemId);
#else
	/*marlin?*/
#if defined(CONFIG_OP_SES)
	if (hBcast->stParentalRatingStatus.bBlockedByCas == TRUE)
	{
		*ppszSystemId = HLIB_STD_StrDup("BlockedByCas");
	}
	else
#endif
	{
		*ppszSystemId = HLIB_STD_StrDup("urn:dvb:casystemid:19188");
	}
#endif
	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_ReleaseParentalRatingChangeStatus (OplBroadcast_t *hBroadcast, char *pszContentId, char *pszSystemId)
{
	if(pszContentId != NULL)
		HLIB_STD_MemFree(pszContentId);
	if(pszSystemId != NULL)
		HLIB_STD_MemFree(pszSystemId);
	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_GetRequestPincodeMsg (OplBroadcast_t *hBroadcast, const char **ppszMsg)
{
	xOplBroadcast_t				*hBcast = xbroadcast (hBroadcast);

	if(ppszMsg == NULL)
	{
		return BPL_STATUS_ERROR;
	}

	*ppszMsg = HLIB_STD_StrDup((const HCHAR *)hBcast->pszPincodeMsg);

	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_ReleaseRequestPincodeMsg (OplBroadcast_t *hBroadcast, const char *pszMsg)
{
	if(pszMsg != NULL)
		HLIB_STD_MemFree((char *)pszMsg);

	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_GetNcdChangedMsg (OplBroadcast_t *hBroadcast, const char **ppszMsg,int *pStartTime,int *pDuration)
{
	xOplBroadcast_t				*hBcast = xbroadcast (hBroadcast);

	if(ppszMsg == NULL)
	{
		return BPL_STATUS_ERROR;
	}
	if((pStartTime == NULL) || (pDuration == NULL) )
	{
		return BPL_STATUS_ERROR;
	}

	*ppszMsg = HLIB_STD_StrDup((const HCHAR *)hBcast->pszNetworkMsg);
	*pStartTime = (int)hBcast->nStartTimeofChange;
	*pDuration = (int)hBcast->nASODuration;

	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_ReleaseNcdChangedMsg (OplBroadcast_t *hBroadcast, const char *pszMsg)
{
	if(pszMsg != NULL)
		HLIB_STD_MemFree((void *)pszMsg);

	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_GetNetworkUri (OplBroadcast_t *hBroadcast, const char **ppszUri)
{
	xOplBroadcast_t				*hBcast = xbroadcast (hBroadcast);

	if(ppszUri == NULL)
	{
		return BPL_STATUS_ERROR;
	}

	*ppszUri = HLIB_STD_StrDup((const HCHAR *)hBcast->pszNetworkUri);

	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_ReleaseNetworkUri (OplBroadcast_t *hBroadcast, const char *pszUri)
{
	if(pszUri != NULL)
		HLIB_STD_MemFree((void *)pszUri);

	return BPL_STATUS_OK;
}

const char* OPL_Broadcast_GetPlayingThumbnail(OplBroadcast_t *hBroadcast, unsigned int pos)
{
	char *thumbnail = NULL;
	HUINT32 capturedTime = 0;
	xOplBroadcast_t					*hBcast = xbroadcast(hBroadcast);
	if (NULL == hBcast)
	{
		HxLOG_Error("hBcast NULL\n");
		return NULL;
	}
	thumbnail = APK_MEDIA_PLAY_GetThumbnail(hBcast->ulViewId, pos, &capturedTime);
	return thumbnail;
}

const char* OPL_Broadcast_requestThumbnail(OplBroadcast_t *hBroadcast, const char* url, HUINT32 time)
{
	xOplBroadcast_t 		*hBcast = xbroadcast(hBroadcast);
	HCHAR					*p = NULL, stUrl[1024] = {0,};
	HCHAR					*pstRet = NULL;
	if (NULL == hBcast)								{ return p; }

	p = (HCHAR*)THAPI_GetThumbnailURL(url, time,NULL, TRUE);
	HxLOG_Debug("[%s:%d]url[%s],time[%d], p[%s]\n",__FUNCTION__,__LINE__,url,time,p);
	if(p)
	{
		HxSTD_PrintToStrN(stUrl, 1024, "file:///%s", p);
		pstRet = HLIB_STD_StrDup(stUrl);
		return pstRet;
	}

	return NULL;
}

BPLStatus OPL_Broadcast_ResultPincode (OplBroadcast_t *hBroadcast, HBOOL permit, int *retVal)
{
	xOplBroadcast_t				*hBcast = xbroadcast (hBroadcast);
	NOT_USED_PARAM(hBcast);

	APK_MEDIA_PLAY_ResultPincode(permit);
	*retVal = 0;
	return BPL_STATUS_OK;
}




BPLStatus OPL_Broadcast_GetPlaySpeeds(OplBroadcast_t *hBroadcast, int **speeds_out, int *count_out)
{
	int i = 0;
	int speeds[MAX_SPEED_NUM] = {0, };
	xOplBroadcast_t *hBcast = xbroadcast(hBroadcast);

	if ( !count_out ) return BPL_STATUS_ERROR;

	HERROR hErr = APK_MEDIA_PLAY_GetSupportedSpeeds(hBcast->ulViewId, (HUINT32 *)count_out, MAX_SPEED_NUM, speeds);
	if ( hErr != ERR_OK ) return BPL_STATUS_ERROR;

	*speeds_out = HLIB_STD_MemCalloc(sizeof(int)*(*count_out));
	if ( !*speeds_out ) return BPL_STATUS_ERROR;

	if ( *count_out >= MAX_SPEED_NUM ) *count_out = MAX_SPEED_NUM;

	for ( i=0; i<*count_out; ++i ) {
		(*speeds_out)[i] = speeds[i]/100;
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_GetTrickRestrictMode(OplBroadcast_t *hBroadcast, OplBroadcastTrickRestrictMode_e *peMode)
{
	xOplBroadcast_t				*hBcast = xbroadcast(hBroadcast);
	HERROR						hErr;

	if ((NULL == hBcast) || (NULL == peMode))
	{
		HxLOG_Error("hBcast:0x%08x\n", (HUINT32)hBcast);
		return BPL_STATUS_ERROR;
	}

	hErr = APK_MEDIA_PLAY_GetTrickRestrictMode(hBcast->ulViewId, peMode);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_MEDIA_PLAY_GetTrickRestrictMode err: ulViewId(0x%04x)\n", hBcast->ulViewId);
		return BPL_STATUS_ERROR;
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_GetTsrPausedPoints(OplBroadcast_t *hBroadcast, HUINT32 *pulNum, OPL_Bookmark_t ***pppstBookmarks)
{
	xOplBroadcast_t				*hBcast = xbroadcast(hBroadcast);
	HERROR						hErr;
	HUINT32						ulNum = 0;
	OxMediaPlay_Bookmark_t		*pstBookmarks = NULL;
	HUINT32						i;

	if ((NULL == hBcast) || (NULL == pulNum) || (NULL == pppstBookmarks))
	{
		HxLOG_Error("hBcast:0x%08x\n", (HUINT32)hBcast);
		return BPL_STATUS_ERROR;
	}

	hErr = APK_MEDIA_PLAY_GetTsrPausedPoints(hBcast->ulViewId, &ulNum, &pstBookmarks);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_MEDIA_PLAY_GetTsrPausedPoints err: ulViewId(0x%04x)\n", hBcast->ulViewId);
		*pulNum = 0;
		*pppstBookmarks = NULL;
		return BPL_STATUS_ERROR;
	}

	if (ulNum != 0)
	{
		*pppstBookmarks = (OPL_Bookmark_t **)HLIB_STD_MemCalloc(ulNum*sizeof(OPL_Bookmark_t *));
		if (*pppstBookmarks == NULL)
		{
			*pulNum = 0;
			*pppstBookmarks = NULL;
			if (pstBookmarks != NULL)
			{
				HLIB_STD_MemFree(pstBookmarks);
			}

			return BPL_STATUS_ERROR;
		}

		*pulNum = ulNum;
		for (i=0; i<ulNum; i++)
		{
			(*pppstBookmarks)[i] = OPL_Bookmark_New(pstBookmarks[i].ulPos, pstBookmarks[i].szName);
		}
	}
	else
	{
		*pulNum = 0;
		*pppstBookmarks = NULL;
	}

	if (pstBookmarks != NULL)
	{
		HLIB_STD_MemFree(pstBookmarks);
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_GetTsrPaused(OplBroadcast_t *hBroadcast, HBOOL *pbIsPaused)
{
	xOplBroadcast_t				*hBcast = xbroadcast(hBroadcast);
	HERROR						hErr;

	if ((NULL == hBcast) || (NULL == pbIsPaused))
	{
		HxLOG_Error("hBcast:0x%08x\n", (HUINT32)hBcast);
		return BPL_STATUS_ERROR;
	}

	hErr = APK_MEDIA_PLAY_GetTsrPaused(hBcast->ulViewId, pbIsPaused);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_MEDIA_PLAY_GetTsrPaused err: ulViewId(0x%04x)\n", hBcast->ulViewId);
		return BPL_STATUS_ERROR;
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_GetStorageRight(OplBroadcast_t *hBroadcast, OplBroadcastStorageRight_e *peRight)
{
	xOplBroadcast_t				*hBcast = xbroadcast(hBroadcast);

	if ((NULL == hBcast) || (NULL == peRight))
	{
		HxLOG_Error("hBcast:0x%08x\n", (HUINT32)hBcast);
		return BPL_STATUS_ERROR;
	}
	*peRight = hBcast->eStorageRight;

	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_GetMaxDuration (OplBroadcast_t *hBroadcast, unsigned int *pulMaxDuration)
{
	xOplBroadcast_t				*hBcast = xbroadcast(hBroadcast);
	HERROR						hErr;
#if defined(CONFIG_TIMESTAMP_64)
	HUINT64						ullDuration = 0;
#endif

	if ((NULL == hBcast) || (NULL == pulMaxDuration))
	{
		HxLOG_Error("hBcast:0x%08x, pulMaxDuration:0x%08x\n", (HUINT32)hBcast, (HUINT32)pulMaxDuration);
		return BPL_STATUS_ERROR;
	}

	*pulMaxDuration = 0;

#if defined(CONFIG_TIMESTAMP_64)
	hErr = APK_MEDIA_PLAY_GetDurationTime(hBcast->ulViewId, &ullDuration);
	if (hErr != ERR_OK)
	{
		HxLOG_Error ("APK_MEDIA_PLAY_GetDurationTime Err.\n");
		return BPL_STATUS_ERROR;
	}
	*pulMaxDuration = (HUINT32)ullDuration;
#else
	hErr = APK_MEDIA_PLAY_GetDurationTime(hBcast->ulViewId, pulMaxDuration);
	if (hErr != ERR_OK)
	{
		HxLOG_Error ("APK_MEDIA_PLAY_GetDurationTime Err.\n");
		return BPL_STATUS_ERROR;
	}
#endif
	return BPL_STATUS_OK;
}


BPLStatus OPL_Broadcast_GetRctInfoCount (OplBroadcast_t *hBroadcast, HUINT32 *totalLinkCount, HUINT32 *imgDataByteSize)
{
	xOplBroadcast_t				*hBcast = xbroadcast(hBroadcast);
//	HERROR						hErr;

	if((NULL == hBcast) || (NULL == totalLinkCount) || (NULL == imgDataByteSize))
	{
		HxLOG_Error("hBcast:0x%08x, totalLinkCount:0x%08x, imgDataByteSize:0x%08x\n", (HUINT32)hBcast, (HUINT32)totalLinkCount, (HUINT32)imgDataByteSize);
		return BPL_STATUS_ERROR;
	}

	*totalLinkCount = hBcast->nLinkCount;
	*imgDataByteSize = hBcast->nImgSize;
/*
	hErr = APK_MEDIA_PLAY_GetRctInfoCount(hBcast->ulViewId, &totalLinkCount, &imgDataByteSize);

	if (hErr != ERR_OK)
	{
		HxLOG_Error ("APK_MEDIA_PLAY_GetRctInfoCount Err.\n");
		return BPL_STATUS_ERROR;
	}
*/
	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_GetRctInfo (OplBroadcast_t *hBroadcast, void **rctInfo)
{
	xOplBroadcast_t				*hBcast = xbroadcast(hBroadcast);
	HERROR						hErr;

	printf("##### sh ##### [%s][%d] nLinkCount=[%d]\n",__FUNCTION__,__LINE__,hBcast->nLinkCount);

	if(hBcast->nLinkCount)
	{
		*rctInfo = (DxService_RctInfo_t *)HLIB_STD_MemAlloc(sizeof(DxService_RctInfo_t));
		((DxService_RctInfo_t *)(*rctInfo))->pstLinkInfoList = (DxService_RctLinkInfo_t *)HLIB_STD_MemAlloc(sizeof(DxService_RctLinkInfo_t) * hBcast->nLinkCount);
		((DxService_RctInfo_t *)(*rctInfo))->stImageIconInfo.pucDatabyte = (HUINT8 *)HLIB_STD_MemAlloc(sizeof(HUINT8) * hBcast->nImgSize);

		hErr = APK_MEDIA_PLAY_GetRctInfo(hBcast->ulViewId, *rctInfo);
		if(hErr != ERR_OK)
		{
			HxLOG_Error ("APK_MEDIA_PLAY_GetRctInfo Err.\n");
			return BPL_STATUS_ERROR;
		}
		printf("##### sh ##### [%s][%d] ulTotalLinkCnt=[%d]\n",__FUNCTION__,__LINE__,((DxService_RctInfo_t *)(*rctInfo))->ulTotalLinkCnt);
	}
	else
	{
		*rctInfo = NULL;
	}
	return BPL_STATUS_OK;

}

BPLStatus OPL_Broadcast_ReleaseRctInfo(void *rctInfo)
{
	HERROR						hErr = BPL_STATUS_ERROR;
	hErr = APK_MEDIA_PLAY_FreeRctInfo(rctInfo);
	if(hErr != ERR_OK)
	{
		HxLOG_Error ("APK_MEDIA_PLAY_GetRctInfo Err.\n");
		return BPL_STATUS_ERROR;
	}
	return BPL_STATUS_OK;
}


BPLStatus OPL_Broadcast_GetTuneInfoByMheg (OplBroadcast_t *hBroadcast, HUINT32 *ulMhegSvcUid, HUINT32 *ulTuneMode, void **hMhegChannel )
{
	xOplBroadcast_t				*hBcast = xbroadcast(hBroadcast);

	if(NULL == hBcast)
	{
		printf("hBcast:0x%08x\n", (HUINT32)hBroadcast);
		return BPL_STATUS_ERROR;
	}

	*ulMhegSvcUid = hBcast->nMhegSvcUid;
	*ulTuneMode = hBcast->eMhegTuneMode;
	*hMhegChannel = hBcast->hMhegChannel;

	printf("## ysmoon ## [%s, %d] MhegSvcUid:%d, TuneMode:0x%x, hMhegChannel: %p, %p\n", __FUNCTION__, __LINE__, *ulMhegSvcUid, *ulTuneMode, hBcast->hMhegChannel , *hMhegChannel);

	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_GetMhegInterruptFlag(OplBroadcast_t *hBroadcast, HUINT32 *ulInterruptibleFlag)
{
	xOplBroadcast_t				*hBcast = xbroadcast(hBroadcast);

	if(NULL == hBcast)
	{
		printf("hBcast:0x%08x\n", (HUINT32)hBroadcast);
		return BPL_STATUS_ERROR;
	}

	*ulInterruptibleFlag = (HUINT32)hBcast->bInterruptible;

	printf("## ysmoon ## [%s, %d] *InterruptibleFlag:%d \n", __FUNCTION__, __LINE__, *ulInterruptibleFlag);

	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_GetMhegIcsFlag(OplBroadcast_t *hBroadcast, HUINT32 *ulMhegIcsStatus)
{
	xOplBroadcast_t				*hBcast = xbroadcast(hBroadcast);

	if(NULL == hBcast)
	{
		HxLOG_Error("hBcast:0x%08x\n", (HUINT32)hBroadcast);
		return BPL_STATUS_ERROR;
	}

	*ulMhegIcsStatus = (HUINT32)hBcast->bMhegIcsStatus;

//	printf("## ysmoon ## [%s, %d] *ulMhegIcsStatus:%d \n", __FUNCTION__, __LINE__, *ulInterruptibleFlag);

	return BPL_STATUS_OK;
}

BPLStatus OPL_Broadcast_HasSDTeletext()
{

	HUINT32 					 ulViewId =0;
	HINT32						 nSvcUid =0;
	DxService_t 				*pService = NULL;
	DxRecListData_t				*pstRecord = NULL;
	OxMediaPlay_MediaType_e 	 eMediaType;
	BPLStatus					 hasTTX = BPL_STATUS_ERROR;

	if(APK_MEDIA_PLAY_GetMainViewId( &ulViewId ) == ERR_OK)
	{
		if(	APK_MEDIA_PLAY_GetPlayType(ulViewId, &eMediaType) == ERR_OK)
		{
			if(APK_MEDIA_PLAY_GetRequestId ( ulViewId, eMediaType, (HUINT32*)&nSvcUid) == ERR_OK)
			{
				if(eMediaType == eOxMP_MEDIATYPE_PVRPLAYBACK)
				{
					pstRecord = APK_META_RECORD_GetByCID(nSvcUid);

					if(pstRecord != NULL)
					{
						if(pstRecord->stMeta.usTtxPid != 0x2000)
						{
							hasTTX = BPL_STATUS_OK;
						}

						APK_META_RECORD_Release(pstRecord);
					}
				}
				else // eOxMP_MEDIATYPE_LIVE, eOxMP_MEDIATYPE_TSRPLAYBACK
				{
					pService = APK_META_SVC_GetService (nSvcUid);

					if(pService != NULL)
					{
						if(pService->ttxPid != 0x2000)
						{
							hasTTX = BPL_STATUS_OK;
						}

						APK_META_SVC_Delete (pService);
					}
				}
			}
		}
	}

	return hasTTX;
}

BPLStatus OPL_Broadcast_PauseHbbtvMonitoring(OplBroadcast_t *hBroadcast)
{
    xOplBroadcast_t *hBcast = xbroadcast(hBroadcast);

    if(NULL == hBcast)
    {
    	HxLOG_Error("hBcast:0x%08x\n", (HUINT32)hBroadcast);
    	return BPL_STATUS_ERROR;
    }

    if(APK_MEDIA_PLAY_StopHbbtvMonitoring(hBcast->ulViewId) == ERR_OK)
    {
        HxLOG_Error("APK_MEDIA_PLAY_StopHbbtvMonitoring Err. \n");
        return BPL_STATUS_ERROR;
    }

    return BPL_STATUS_OK;
}

/* EOF */
