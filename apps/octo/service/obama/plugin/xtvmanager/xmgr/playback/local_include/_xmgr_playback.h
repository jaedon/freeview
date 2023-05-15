#ifndef	__XMGR_PLAYBACK_INT_H__
#define	__XMGR_PLAYBACK_INT_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <svc_si.h>
#include <mgr_playback.h>


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/* local macros using in proc msg  */
#define PB_PARAM_WARNING_REMOVE(arg1, arg2, arg3, arg4, arg5)	\
			{													\
				(void)arg1;										\
				(void)arg2;										\
				(void)arg3;										\
				(void)arg4;				 						\
				(void)arg5;				 						\
			}

#define PB_POINTER_INVALID_CHECK(pArg, returnValue)				\
			if(pArg == NULL)									\
			{													\
				HxLOG_Critical("\n\n");							\
				return returnValue;								\
			}

#define PB_HERROR_INVALID_CHECK(pArg, returnValue)				\
			if(pArg != ERR_OK)									\
			{													\
				HxLOG_Critical("\n\n");							\
				return returnValue;								\
			}

#define CHECK_PB_ACTION_HANDLE()	\
	do {																	\
		HUINT32			 ulMsgActionId = SVC_PIPE_GetActionId (hAction);		\
		if (SVC_PIPE_GetActionId (pstContext->hAction) != ulMsgActionId)		\
		{																	\
			return MESSAGE_PASS;											\
		}																	\
	} while(0)

#define SCREEN_SAVER_TIMER_ID				200
#define PB_EOF_TIMER_ID						201
#define RADIO_BACKGROUND_IMAGE_ID			202
#define PB_RESERVED_TRICK_START_TIMER_ID	203
#define PB_RESERVED_JUMP_START_TIMER_ID		204
#define PB_EXTRACT_THUMB_TIMER_ID			205

#define PB_EOF_TIMEOUT_DURATION				(30*1000)	/* Timebar �� 30 secs �� �ƹ� �Է� ������ �������. */
#define PB_RESERVED_TRICK_START_TIMEOUT		(850)		// 0.85��
#define PB_RESERVED_JUMP_START_TIMEOUT		(10)		// 0.85��
#define PB_EXTRACT_THUMB_TIME					(500)
#define BOOKMARK_AVAILABLE_POSITION			4

#define PANEL_STRING_TIMER_ID				0x1001		// "      PLAY      " �� 2�ʰ� ������� �Ѵ�.
#define PANEL_STRING_TIME_OUT				2000		// 2 ��
#define JUMP_SKIP_TIME						120
#define JUMP_REPLAY_TIME					15

#define MAX_PB_THUMB					100		// �ӵ� ������ ������ ������ �������� ������ ������ŭ�� thumbnail�� �����Ѵ�.

// for CI+ v1.3
/** @brief [CI+ DRM state: CI+ DRM�� �ɷ� A/V�� ���� �� �ִ� ���� List] */
// or ������ �̿��Ͽ� �������� ���ÿ� ���� ��.
typedef enum
{
	eXmgrPbCiPlusDrmState_None			= 0,

	eXmgrPbCiPlusDrmState_RL			= 0x0001,	// Retention Limit�� �ɷ��ִ� ����
	eXmgrPbCiPlusDrmState_PC			= 0x0002,	// CI+ 1.3 spec�� Parental Control �ó������� �ɷ��ִ� ����
	eXmgrPbCiPlusDrmState_License		= 0x0004,	// CI+ 1.3 spec�� license ���ѿ� �ɷ��ִ� ����
} xmgrPb_CiPlusDrmState_e;

typedef enum
{
	eXmgrPbState_IDLE		= 0,
	eXmgrPbState_STARTED,				/**<  */
	eXmgrPbState_WAIT_PMT,				/**< PMT Pid �� ������ �߻��Ͽ� PMT �� �ٽ� �ɰ� ������ ��ٸ��� ���� */
	eXmgrPbState_STOPPED,
	eXmgrPbState_WAIT_PB_LOCK,			/**< Player �� Lock ������ ����ϴ� ���� */
	eXmgrPbState_WAIT_PB_SETUP,			/**< Player �� setup �Ǳ⸦ ��ٸ��� ���� */
	eXmgrPbState_WAIT_AV_STARTED,		/**< A/V �� start �Ǳ⸦ ��ٸ��� ���� */
	eXmgrPbState_WAIT_RATING,			/**< Rating������ ��ٸ��� ���� */
	eXmgrPbState_LOCK_PIN, 				/**< ���� Lock�� ������ PIN code �Է� ��� ���� */
	eXmgrPbState_RATING_PIN,			/**< Rating PIN code �Է� ��� ���� */
	eXmgrPbState_AV,					/**< AV�� ������ �������� TV��û ���� */
	eXmgrPbState_CAS_BLOCKED,			/**< CAS�� ���Ͽ� ��û ���ѵǾ� �ִ� ���� */
} xmgrPb_State_e;

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	MgrPb_PlaySetup_t		 stSetup;

	HUINT32				ulActionId;					/**< action Id */
	Handle_t			hAction;					/**< �� context�� ���� zapping session version�� ���Ե� action handle */
	HUINT32				ulVideoDecId;
	HUINT32				ulPlayerId;
	HUINT32				ulDemuxId;
	HUINT8				ucPlayIdx;					/**< actual recorder instance number : 0, 1 */
	HUINT32				ulContentId;				/**< booting ������ ts ���� ���/��ȭ�� ���� ������ ID */
	xmgrPb_State_e		ePbState;					/**< context status */

	/*
		Trick�� ���� eTrickState �� eTrickSpeed �� ���� ���־� �ϰ� �ִ��� nPlaySpeed �� ������ ��ģ��.
	*/
	HINT32				nPlaySpeed;
	//
	//
	MgrPvr_TrickState_e		eFirstTrickState;			/**< surfer �󿡼� ó�� trick �����ϰ��� �� trickstate ��. Pause, Slow, FR, play */
	MgrPvr_TrickState_e		eTrickState;				/**< ����� Trick ���� */
	MgrPb_TrickSpeed_e		eTrickSpeed;				/**< ����� Trick ���� */
	MgrPvr_TrickState_e		ePrevTrickState;			/**< PMT Update���� Trick Status */
	MgrPb_TrickSpeed_e		ePrevTrickSpeed;			/**< PMT Update���� Trick Status */
	HBOOL					eValidPrevTrick;			/**< PMT Update�� �ش� Flag Set */

	MgrPb_PlayType_e	ePbType;					/**< �� ���Ͽ� ���� ��� ��� */
	MgrPb_State_e		uiState;					/* �� context�� UI state */
	Handle_t 			hSvc;						/**< TSR_Playback, Chase_Playback ���۽� WTV �� service Handle ���� copy �Ѵ� */
	Handle_t 			hMasterSvc;					/**< TSR_Playback, Chase_Playback ���۽� WTV �� master service Handle ���� copy �Ѵ� */
	ChListUiGroup_t 	eSvcListGroup;				/**< TSR_Playback, Chase_Playback ���۽� WTV �� ChListUiGroup_t ���� copy �Ѵ� */
	UNIXTIME			ulStartTime;				/**< ��� start time */
	UNIXTIME			ulEndTime;					/**< ��� end time */
	UNIXTIME			ulPlayTime;					/**< ���� ����Ǵ� �ð� */
	UNIXTIME			ulOrgStartTime;				/**< Recording �Ǿ��� ���� start time */
	UNIXTIME 			ulStartTimeByDrm;			/** <allowed start time by drm*/
	HINT32 				nChNum;						/**< channel service number */
	SvcMeta_Service_t		svcInfo;					/**< ��ȭ�߿� ����� ���� information ����, playback �� ����Ѵ�. */
	HBOOL				bRadioBackground;			/* True if radio background is enabled */
	HBOOL				bSubtitleAllowed;			/* Subtitle �����ص� �ȴٴ� flag. MENU���� AUTO�̰ų� subtitle list popup�� start�� ��� TRUE */
	HBOOL				bTtxVbiStarted;				/* TELETEXT�� VBI insert������ ���۵�. Page show�� �ƴϸ� subtitle�� enable�� �� ����.
														��, teletext�� �� session�� �ѹ��� start �Ǿ�� ��. */
	HBOOL				bTtxPageEnabled;			/* TELETEXT page�� �����ְ� ����. Subtitle�� enable�� �� ����. */
	SvcSi_SubtitleInfo_t	subtitleInfo;
	HBOOL				bSubTitleOn;				// Subtitle Running ?
	HBOOL				bPmtValid;					/**< PMT�� ���޵�. */
	HBOOL				bRestartAV;
	HBOOL				bLockPinPassed;				/* ������ session���� lock PIN code�� pass �Ǿ��°��� ��Ÿ��. */
	HBOOL				bRatingPinPassed;			/* ������ session���� rating PIN code�� pass �Ǿ��°��� ��Ÿ��. */
	HBOOL				bRatingPassByLockPass;		/* Lock pass�� ���Ͽ� rating�� pass�� ó���϶�� flag */
	HINT8				*pszPinAppStr;				/* PIN code applicatin���� UI�� �����ִ� �޽��� ��Ʈ�� */

	HUINT32				ulEventId;					/* Current event ID */
	HBOOL				bRatingValid;				/* EIT�� rating���� ���԰ų� EIT timeout�� �߻��Ͽ� Rating value�� ��밡���� */
	HUINT32				ulRatingValue;				/* ������ Rating �� */
	HBOOL				bAVHide;					/* AV Hide */
	HBOOL				bAlreadyExistEit;			/* trick �߿� eit timeout ���͵� �����ϱ� ����. */
	HBOOL				bPBStart;
	HBOOL				bFirstPmt;
	HBOOL				bFirstTimecode;

	HBOOL				bIsEOF;						/* EOF ���� üũ */

/* AV decoder management */
	HBOOL				bVideoStarted;
	HBOOL				bAudioStarted;
	HBOOL				bAvUnderrun;				/* AV underrun flag */
	HBOOL				bHdmiBlocked;				/* HDMI was blocked by HDCP fail or etc. */

/* CAS management */
	HBOOL				bCasOk;						/* CAM ���� ��û �㰡�� �޾Ҵ°�? */
	HBOOL				bSCPinCodeValid;			/* Smart card Pin code�� ��� �����Ѱ�? */
	HBOOL				bSCPinPassed;				/* Smart card PIN passed flag. ��翡 ���� �ʿ��ϸ� ���. */
	HINT32				nCasErrCode;				/**< CAS error code ����. �뵵��? */
	HBOOL				bCasStarted;
#if defined(CONFIG_MW_CI_PLUS_V_1_3)
	HBOOL				bCnfPbLicense;			/* CI+ 1.3 : playback license cnf ���¼���. Host(meta) -> Cam ���� license ���� �� Cam -> Host �� license confirm �� ��� true. �ʱ� ���� true ����*/
	xmgrPb_CiPlusDrmState_e	eCiPlusDrmState;			/* CI+ 1.3�� �����ϴ� �𵨿��� PB�� �� ���. CI+ ���ۿ� ���� A/V�� ������ �ϴ� ��� �� ���¸� ���. or �� ���� state�� ���ÿ� check */
#endif
#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
	DxCAS_GroupId_e		nCasSystemId;				/* EVT_CAS_FAIL�޽����� nParam1���� ���޵Ǵ� CAS system ID (Nagra, Ir, Conax, ...) */
	void				*pCasInfo;					/* EVT_CAS_FAIL�޽����� nParam2�� ���޵Ǵ� CAS proc���� ����� data point */
	HINT32				nCasEvt;					/* CAS proc���� �Ѱ��� CAS event (eSEVT_CAS_OK, eSEVT_CAS_FAIL) */
#endif
#if defined(CONFIG_MW_CAS_CONAX)
  #if defined(CONFIG_MW_CAS_CONAX_URI)
	DxCopyrightControl_ConaxDrmInfo_t 		stCxUri;
	HBOOL				bUriValid;
  #endif
	HUINT8				ucPrevProgLevel;
	UNIXTIME			ulCxNextUriTime;			/* next uri start time */
	UNIXTIME			ulUnixStartTime;			/* start time (ex 2014.07.02 22:10:34) */
	UNIXTIME			ulUnixEndTime;				/* end time (ex 2014.07.02 23:10:54) */
	HBOOL				bAvStarted; 				/* if check av started */
	HBOOL				bAllUriExpired; 			/* if check all content period are expired */
	HBOOL				bCardDisabled;				/* if card inserted or removed */

	UNIXTIME			ulPrevPlayTime;				/**< ���� ����Ǵ� �ð� */
	HUINT8				ucLiveProgramLevel;
	HBOOL				bPauseByCAS;
#endif
	HBOOL				bSpeedReserved;					/* AV Enable �Ǳ� ���� Speed �� ���õ� */
	HINT32				nReservedSpeed;					/* AV Enable �Ǳ� ���� ���õ� Speed */

	HBOOL				bJumpReserved;					/* AV Enable �Ǳ� ���� Jump �� ���õ� */
	HINT32				nReservedJump;					/* AV Enable �Ǳ� ���� ���õ� Jump */


/* DRM & Copy protection management */
	HBOOL				bDrmValid;				/* Copy protection ������ detect ��. */
	DxCopyrightControl_t			stDrmInfo;					/**< drm info by metafile(.hmt) */
	HUINT32				ulDrmUriTime;			/**< drm license �� �߻��� �ð� ���� by metafile(.hjtsm) */
	HUINT32				ulDrmLicenseTime;			/**< drm license �� �߻��� �ð� ���� by metafile(.hjtsm) */

/* thumbnail extraction */
	HUINT32				ulThumbInterval;
	HUINT32				ulThumbNum;
	HUINT8				aucThumb[MAX_PB_THUMB];
	HUINT32				ulNextThumbIdx;

	HBOOL				bIsPbStopByError;		/* ����� Error�� �߻� �Ǿ��� �� MSG_PVR_PLAYBACK_NOTIFY_ERROR_STOPPED�� ���� �ϱ� ���� */

	/* timeStamp of Meta */
	SvcMeta_TimestampList_t stTimeStampList;

} xmgrPbContext_t;


#define _________INTERNAL_FUNC__________
/*	xmgr_playback.c
 */
HUINT32 xmgr_playback_GetContextIndexFromActionId (HUINT32 ulActionId);
void xmgr_playback_SetCopyProtection(xmgrPbContext_t *pstContext);

#define _________PLUGIN_FUNC__________
/* BASE */
HERROR			xmgr_playback_Init_Base (void);
HERROR			xmgr_playback_SetPlaySetup_Base (HUINT32 ulActionId, MgrPb_PlaySetup_t *pstPbSetup);
BUS_Callback_t	xmgr_playback_GetProcFunc_Base (HUINT32 ulActionId);
HUINT32			xmgr_playback_GetPlaybackParam_Base (MgrPb_ParamType_e eType);
HERROR			xmgr_playback_SetPlaySpeed_Base (HUINT32 ulActionId, HINT32 nPlaySpeed);
HERROR			xmgr_playback_SetJumpPlay_Base (HUINT32 ulActionId, HUINT32 ulTimeToPlay);
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
HERROR			xmgr_playback_SetPlaySpeed_Ses(HUINT32 ulActionId, HINT32 nPlaySpeed);
HERROR 			xmgr_playback_SetJumpPlay_Ses(HUINT32 ulActionId, HUINT32 ulTimeToPlay);
#endif
HERROR			xmgr_playback_GetPlaybackInfo_Base (HUINT32 ulActionId, MgrPb_Info_t *pstInfo);
HERROR			xmgr_playback_GetPlaybackTimeInfo_Base (HUINT32 ulActionId, MgrPb_Info_t *pstInfo);
HERROR			xmgr_playback_StopPlayback_Base (HUINT32 ulActionId);
HERROR			xmgr_playback_SetAudioInfo_Base (HUINT32 ulActionId, HUINT32 ulIndex, DxStereoSelect_e eSoundMode);
HERROR			xmgr_playback_SetSubTitleDisplay_Base (HUINT32 ulActionId, HBOOL bDisplay);
HERROR			xmgr_playback_SetSubtitleLanguage_Base (HUINT32 ulActionId, HUINT32 ulIndex);
HERROR			xmgr_playback_GetSubtitleDisplay_Base (HUINT32 ulActionId, HBOOL *pbDisplay);
HERROR			xmgr_playback_GetServiceInfo_Base (HUINT32 ulActionId, SvcMeta_Service_t *pstService);
HERROR			xmgr_playback_SetServiceInfo_Base (HUINT32 ulActionId, SvcMeta_Service_t *pstService, HBOOL bUpdate);
HBOOL			xmgr_playback_IsPlaybackId_Base (HUINT32 ulContentId, MgrPb_Info_t *pstInfo);

HERROR 			xmgr_playback_GetMenuSubtitleLangIndex(DbParam_Item_e eSubtitleItem, SvcSi_SubtitleList_t *pstSutitleList, HINT32 *pnIndex);

/* Sub Module CIPLUS BASE */
BUS_Callback_t	xmgr_playback_CiPlusGetProcFunc_Base (HUINT32 ulActionId);

/* Sub Module IRDETO BASE */
BUS_Callback_t	xmgr_playback_IrdetoGetProcFunc_Base (HUINT32 ulActionId);

/* Sub Module NAGRA BASE */
BUS_Callback_t	xmgr_playback_NagraGetProcFunc_Base (HUINT32 ulActionId);

/* Sub Module CONAX BASE */
HERROR			xmgr_playback_ConaxInit_Base(void);
BUS_Callback_t	xmgr_playback_ConaxGetProcFunc_Base (HUINT32 ulActionId);
HERROR			xmgr_playback_ConaxSetJumpPlay_Base (HUINT32 ulActionId, HUINT32 ulTimeToPlay);

/* Sub Module VERIMATRIX BASE */
BUS_Callback_t	xmgr_playback_VerimatrixGetProcFunc_Base (HUINT32 ulActionId);

/* End Of BASE  */


/* SES */
BUS_Callback_t	xmgr_playback_GetProcFunc_Ses (HUINT32 ulActionId);


#endif /* __XMGR_PLAYBACK_INT_H__ */
