#ifndef	___MGR_RECORDER_H___
#define	___MGR_RECORDER_H___

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <svc_rec.h>
#include <svc_meta.h>
#include <svc_si.h>

#include <mgr_common.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#define MAX_REC_FOLDER_NAME_LEN			256
#define MAX_REC_DEVICEHANDLE_LEN		257
#define MAX_REC_FILE_NAME_LEN			50

enum
{
	eMEVT_RECORDER			= eMEVT_RECORDER_START,


	/* - Description : recorder app의 녹화 시작을 알림
	   - Parameters Usage :
		   handle : action handle
		   p1 : recording type
		   p2 : cotentId
		   p3 : NA */
	eMEVT_PVR_NOTIFY_RECORD_STARTED,

	/* - Description : recorder app의 녹화 종료를 알림
	   - Parameters Usage :
		   handle : NA
		   p1 : recording type
		   p2 : cotentId
		   p3 : NA */
	eMEVT_PVR_NOTIFY_RECORD_STOPPED,

	/* - Description : recorder app의 endtime 을 Update 해달라는 메시지
	   - Parameters Usage :
		   handle : action handle
		   p1 : endtime
		   p2 : NA
		   p3 : NA */
	eMEVT_PVR_RECORD_ENDTIME_UPDATE,

	/* - Description : recorder app에서 녹화 파일 splitting 을 알리는 메시지
	   - Parameters Usage :
		   handle : action handle
		   p1 : reason
		   p2 : NA
		   p3 : NA */
	eMEVT_PVR_NOTIFY_RECORD_SPLIT_FILE,

	/* - Description : tsr record 를 delayed record 로 변경하라는 메시지
	   - Parameters Usage :
		   handle : action handle
		   p1 : tsr_rec playtime
		   p2 : playtime by current time
		   p3 : NA */
	eMEVT_PVR_RECORD_CHANGE_TSR_REC_TO_DELAYED_RECORD,

	/* - Description : tsr record 를 delayed record 로 변경하라는 메시지에 결과
	   - Parameters Usage :
		   handle : action handle
		   p1 : slot
		   p2 : result (HERROR)
		   p3 : NA */
	eMEVT_PVR_RECORD_CHANGE_TSR_REC_TO_DELAYED_RECORD_RESULT,


	/* - Description : Record 를 start하라는 메세지
	   - Parameters Usage :
		   handle : NULL
		   p1 : NA
		   p2 : NA
		   p3 : NA */
	eMEVT_PVR_RECORD_START_REC,

	/* - Description : Record 를 stop하라는 메세지
	   - Parameters Usage :
		   handle : NULL
		   p1 : NA
		   p2 : NA
		   p3 : NA */
	eMEVT_PVR_RECORD_STOP_REC,

	/* - Description : TSR을 수동으로 시작하라고 하는 메시지
	   - Parameters Usage :
		   handle : NULL
		   p1 : NA
		   p2 : NA
		   p3 : NA */
	eMEVT_PVR_RECORD_START_TSR,

	/* - Description : TSR을 수동으로 멈추라고 하는 메시지
	   - Parameters Usage :
		   handle : NULL
		   p1 : NA
		   p2 : NA
		   p3 : NA */
	eMEVT_PVR_RECORD_STOP_TSR,

	/* - Description : TSR Set (on/off)를 하라는 메시지
	   - Parameters Usage :
		   handle : NULL
		   p1 : NA
		   p2 : NA
		   p3 : NA */
	eMEVT_PVR_TSR_SET_LIVE_TSR,

	/* - Description : Record 시도 후 실패함을 나타내는 메시지
	   - Parameters Usage :
		   handle : NULL
		   p1 : fail reason
		   p2 : slot
		   p3 : NA */
	eMEVT_PVR_NOTIFY_RECORD_SETUP_FAILED,



	/* - Description : Message for transcoding
	   - Parameters Usage :
		   handle : HUINT32 ulTag
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eMEVT_PVR_TRREC_MGR_START,	// cmd test 용도로 만들었음.나중에 실제 동작에서 이용되면 이 주석 지워주세요



	/* - Description : Message for transcoding
	   - Parameters Usage :
		   handle : HUINT32 ulTag
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eMEVT_PVR_TRREC_STOP,

	/* - Description : Message for transcoding
	   - Parameters Usage :
		   handle : HUINT32 ulTag
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eMEVT_PVR_TRREC_MGR_STOP,

	/* - Description : Message for transcoding
	   - Parameters Usage :
		   handle : HUINT32 ulTag
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eMEVT_PVR_TRREC_SECTION_GATHERING_FINISHED,





	/* transcoding을 중지 하라는 메시지  */
	eMEVT_PVR_STOP_TRANSCODING,


	/* - Description : Media Next FILE 메세지.
	   - Parameters Usage :
		   handle :
		   p1 : NA
		   p2 : NA
		   p3 : NA */
	eMEVT_PVR_MEDIA_NEXT_FILE,

	/* - Description : Medialist(Video) 화면을 Update
	   - Parameters Usage :
		   handle : NA
		   p1 : recording type
		   p2 : cotentId
		   p3 : NA */
	eMEVT_PVR_MEDIALIST_UPDATE,

	/* - Description : TimeStamp Update
	   - Parameters Usage :
		   handle : action handle
		   p1 : endtime
		   p2 : NA
		   p3 : NA */
	eMEVT_PVR_RECORD_TIMESTAMP_UPDATE,
	/* - Description : TSR buffer copy complete
	   - Parameters Usage :
		   handle : action handle
		   p1 : NA
		   p2 : NA
		   p3 : NA */
	eMEVT_PVR_RECORD_TSR_COPYEND,

	/* - Description : TSR paused by drm
	   - Parameters Usage :
		   handle : action handle
		   p1 : MgrRec_TsrPausedReason_e
		   p2 : paused time
		   p3 : paused tag name */
	eMEVT_PVR_RECORD_TSR_PAUSED,

	/* - Description : TSR resumed by drm
	   - Parameters Usage :
		   handle : action handle
		   p1 : MgrRec_TsrPausedReason_e
		   p2 : NA
		   p3 : NA */
	eMEVT_PVR_RECORD_TSR_RESUMED,


	/* - Description : Series Record Meta Files Lock info
	   - Parameters Usage :
		   handle : HANDLE
		   p1 : content ID
		   p2 : # of contents that are locked with the same series ID
		   p3 : # of contents with the same series ID */
	eMEVT_PVR_RECORD_SERIES_LOCK_FOUND,
};


typedef enum
{
	eMgrRec_STS_RECORD_START = 0,
	eMgrRec_STS_RECORDING,
	eMgrRec_STS_RECORDED,
	eMgrRec_STS_INCOMPLETED
} MgrRec_Status_e;

typedef enum
{
	eMgrRecOutput_HDD					= 0,	/**< Internal/External HDD (File System) */
	eMgrRecOutput_EXT_TSLINE,					/**< External TS Line */
	eMgrRecOutput_MEM_RINGBUF,					/** < Memory Ring Buffer */
} MgrRec_OutputType_e;

typedef enum
{
	eMgrRec_CasType_None			= 0x00000000,

	eMgrRec_CasType_CiPlus 			= 0x00000001,
	eMgrRec_CasType_Nagra 			= 0x00000002,
	eMgrRec_CasType_NagraSES		= 0x00000004,
	eMgrRec_CasType_Irdeto 			= 0x00000008,
	eMgrRec_CasType_Viaccess		= 0x00000010,
	eMgrRec_CasType_Conax			= 0x00000020,
	eMgrRec_CasType_Verimatrix		= 0x00000040,
	eMgrRec_CasType_End
} MgrRec_CasType_e;

typedef enum
{
	eMgrRecType_TSR				= 0,	/**< time-shift recording */
	eMgrRecType_INSTANT,				/**< instant recording */
	eMgrRecType_TBR,					/**< recording by scheduled service Time-Based */
	eMgrRecType_EBR,					/**< recording by scheduled service Event-Based */
	eMgrRecType_SBR,					/**< recording by scheduled service Series-Based */
	eMgrRecType_DELAYED,				/**< tsr playback(buffered) + instant recording */
	eMgrRecType_SCR2DESC,				/**< desramble a file, convert scramble to descramble */
	eMgrRecType_KWD_EBR,				/**< recording by keyword-based+event based */
	eMgrRecType_KWD_TBR,				/**< recording by keyword-based+time based */
	eMgrRecType_TRREC,
	eMgrRecType_MEMREC,
	eMgrRecType_FULLTSREC,
	eMgrRecType_UNKNOWN
} MgrRec_RecordType_e;

typedef enum
{
	eMgrRecFailed_NOFAIL			= 0,	/**< Recording Successful */
	eMgrRecFailed_HDDFULL,				/**< HDD 용량이 full 인 경우로 녹화 시작을 하지 못함 */
	eMgrRecFailed_MAXNUM,					/**< Over than max record number */
	eMgrRecFailed_MAXRECORDLISTNUM,		/**< Over than max record_id number  1024 */
	eMgrRecFailed_LESS30SEC,				/**< Recording(less than 30 seconds) may not be stored */
	eMgrRecFailed_NOSIGNAL,				/**< No signal */
	eMgrRecFailed_NONSERVICE,				/**< Non-Service, Empty-Service */
	eMgrRecFailed_SEARCHING,				/**< 채널 검색중인 경우 */
	eMgrRecFailed_CONFLICT_TP,			/**< Conflict with higher priority recording */
	eMgrRecFailed_DRM,					/**< No Recording by DRM content */
	eMgrRecFailed_NOSC_SCR,				/**< scramble 서비스인데 smartcard 가 없는 경우 */
	eMgrRecFailed_NORIGHT_SCR,			/**< scramble 서비스인데 descramble 할 권한이 없는 경우 */
	eMgrRecFailed_NOHDD,					/**< HDD 가 없거나 Format 이 안 된 경우, Format 중인 경우 */
	eMgrRecFailed_INCOMPLETE_HDDFULL	,			/**< recording 도중 HDD 용량이 full 인 된 경우 */
	eMgrRecFailed_UNKNOWN,					/**< Unknown reason */
	eMgrRecFailed_NOUSBHDD,				/**< USB HDD 가 없어 녹화를 못한 경우, external pvr 용 */
	eMgrRecFailed_DISCONNECTUSBHDD,		/**< USB HDD 가 녹화 도중에 disconnect 된 경우 */
	eMgrRecFailed_EXTUSB_CopyDel,
	eMgrRecFailed_SAME_SVC_RECORDING,		/**< 이미 동일한 서비스가 녹화 진행중 */
	eMgrRecFailed_HIGHER_PRIORITY,		/* Auto Tracking Mode일 때 잠시라도 Pending된 경우  */
	eMgrRecFailed_UNABLE_TRACKING,		/* Auto Tracking Mode일 때 Tracking 실패 */
	eMgrRecFailed_NO_VIDEO_AUDIO,		/** data broadcasting without valid video */
	eMgrRecFailed_SI_TIMEOUT, 			/**< si timeout (PMT timeout, pmt pid remove)*/
	eMgrRecFailed_POWER_OFF,			/** < ac power off 된 경우 */
} MgrRec_EndReasonType_e;

typedef enum
{
	eMgrRecState_IDLE			= 0,
	eMgrRecState_TUNE,					/**< 레코딩 시작을 위해 Tuning 중인 상태 */
	eMgrRecState_NO_SIGNAL,			/**< 튜닝중인 서비스의 신호가 없는 상태 */
	eMgrRecState_WAIT,					/** wait flag(PMT, CAS, DRM ...)정보를 기다리는 상태 */
	/*************** 이 지점을 경계로 PMT가 valid 한 경우와 아닌 경우로 나뉨 ****************/
	eMgrRecState_BLOCKED,				/** pause flag(CAS, DRM ...)에 의하여 레코딩 제한되어 있는 상태 */
	eMgrRecState_RECORDING,			/**< 정상적인 레코딩 상태 */
	eMgrRecState_STOPPED,

	eMgrRecState_Num,
} MgrRec_State_e;

typedef enum
{
	eMgrRecSession_NORMAL,			/**< Normal session */
	eMgrRecSession_MHEG_QUIET,		/**< MHEG, MHP등 data 방송에서 요청한 silent tuning */
	eMgrRecSession_REPLACE,		/**< Free CA mode/running status change에 의한 replacement service tuning */
	eMgrRecSession_SUBFEED			/**< Sub-feed service session */
} MgrRec_SessionType_e;

typedef enum
{
	eMgrRecStopReason_NORMAL,
	eMgrRecStopReason_NO_AV,
} MgrRec_StopReason_e;

typedef enum
{
	eMgrRec_MIN_REQUIRED_RECORD_TIME,
	eMgrRec_RECORD_DURATION_CHECK_TIME,
	eMgrRec_DEFAULT_DURATION_TSR_RECORD
} MgrRec_ParamType_e;

typedef enum
{
	eMgrRec_DelayedCopy_Init		= 0,
	eMgrRec_DelayedCopy_Completed	= 100,
	eMgrRec_DelayedCopy_Error		= 200,
}MgrRec_DelayedCopyStatus_e;

typedef enum
{
	eMgrRec_TsrPausedReason_None	= 0,
	eMgrRec_TsrPausedReason_ByDrm	= 1,
}MgrRec_TsrPausedReason_e;

typedef struct {
	HBOOL			bIsUsbFlashRecord;
	HUINT32 		ulLimitTimeSecond;
	HUINT64 		ullLimitDataSize;
	HUINT64 		ullLimitIndexSize;
}MgrRec_TsrLimitForUsbFlash_t; // USB TSR을 지원할 경우 LimitSize를 설정

typedef struct
{
	MgrRec_OutputType_e		eRecDst;			// 레코딩 매체 (HDD/EXT TsLine/Etc)
	MgrRec_RecordType_e		eRecType;			// 레코딩 종류
	HINT32					eSessionType;		// 레코딩할 서비스의 session type, watchtv 참조,  Normal, Subfeed 등
	Handle_t				hMasterSvc;			// 레코딩할 서비스의 masterhandle, watchtv 참조, Normal 인경우 hCurrSvc 와 같으며, subfeed 일경우 masterfeed 의 servicehandle 임.
	Handle_t				hCurrSvc;			// 레코딩할 service handle
	HUINT32					ulLiveActionId;		// TSR_REC 을 시작할 당시의 WTV 의 actionID , TSR_REC 을 destroy 시 사용
	HUINT32					ulSlot;				// Schedule recording 에서의 정보를 가져오기 위한 변수
	HUINT32					ulSourceContentId;	// Scramble to Descramble Recording 시 Service 정보를 가져 오기 위한 playback ContentId
	HUINT32					ulPvrIdx;			// 레코딩할 Pvr Storage index
	HBOOL					bStartPauseMode;	// record start 하면서 pause 상태로 시작 할지 고려. default : false 임. false 이면 pause 하지 않고 바로 시작.
	HBOOL					bWaitPpvPay; 		// PPV pay 를 지다리는 동안 발생한 CAS fail event 에 대한 end reason 설정을 무시하기 위해, PPV event 는 반드시 한번 cas fail 이 올라 오므로.
	HUINT32 				ulRecMode;			// for transcode recording

	Handle_t				hMemBuf;			// 레코딩 매체가 Memery Buffer 일 경우 Ring Buffer용 Handle

	HUINT8					szDisplayName[MAX_REC_FOLDER_NAME_LEN];
	HUINT8					szDeviceHandle[MAX_REC_DEVICEHANDLE_LEN];
	HUINT8					szDestinationId[MAX_REC_DEVICEHANDLE_LEN];

	MgrRec_TsrLimitForUsbFlash_t stUsbFlash; // USB TSR을 지원할 경우 LimitSize를 설정

	HBOOL					bUseUserEncKey;
	HUINT8			 		aucKey[PAL_PVR_CRYPTO_KEY_LEN];
} MgrRec_RecSetup_t;

typedef struct
{
	HUINT32					ulContentId;	/**< booting 이후의 ts 파일 재생/녹화를 위한 고유의 ID */
	MgrRec_OutputType_e		eRecDst;
	MgrRec_RecordType_e		eRecType;		/**< recording method */
	MgrRec_State_e			eRecState;		/**< record status */
	Handle_t				hSvc;			/**< service handle to record */
	Handle_t				hMasterSvc;		/**< Supplement service 일 경우에 master handle */
	HINT32					nEventId;		/** <EBR의 Event ID  */
	HINT32					eSessionType;	/**< Normal/Quiet/Replacement/MultiFeed 등 WTV session tuning type */
	HINT32 					nChNum;			/**< channel service number */
	UNIXTIME				ulRecTime;		/**< recording time, duration */
	UNIXTIME				ulStartTime;	/**< recording start time */
	UNIXTIME				ulEndTime;		/**< recording end time */
	UNIXTIME				ulOrgStartTime;	/**< user's first recording start time */
	UNIXTIME				ulLastTime;		/**< Lastest recording time */
	HUINT32					ulSlot;			/**< schedule slot number */
	HUINT32					ulLiveActionId;	/**< TSR_REC 을 시작할 당시의 WTV 의 actionID , TSR_REC 을 destroy 시 사용 */
	Handle_t				hAction;		/**< session version이 포함된 action handle */
	HUINT32					ulPvrIdx;		/*PVR Storage Idx*/
	HINT32					nBranchNum; 	/**< channel service sub number */
	Handle_t				hMemBuf;
	HINT32					nRecId;
} MgrRec_RecInfo_t;

HERROR			MGR_RECORDER_Init(void);
HERROR			MGR_RECORDER_GetRecSiSpec (SvcSi_RecSpec_t *pstSiSpec);
HERROR 			MGR_RECORDER_GetRecCasTypes (HUINT32 *pulCasTypes);
HERROR			MGR_RECORDER_SetRecSetup(HINT32 nRecIdx, MgrRec_RecSetup_t *pstRecSetup);
BUS_Callback_t	MGR_RECORDER_GetProcFunc (HUINT32 ulActionId);
HCHAR*			MGR_RECORDER_GetProcName(HUINT32 ulActionId);
HERROR			MGR_RECORDER_FindTsrRecordingSvcInfo(Handle_t hSvc, HUINT32 *pulActionId, MgrRec_RecInfo_t *pstInfo);
HERROR			MGR_RECORDER_FindRecordingSvcInfo(Handle_t hSvcHandle, HUINT32 *pulActionId, MgrRec_RecInfo_t *pstInfo);
HERROR			MGR_RECORDER_GetRecordingSvcInfo(HUINT32 ulActionId, MgrRec_RecInfo_t *pstInfo);
HERROR			MGR_RECORDER_GetRecordSvcHandleByContentId(HUINT32 ulContentId, Handle_t *phHandle);
HBOOL			MGR_RECORDER_IsDuringDelayedRecCopy(Handle_t hAction);
HUINT8*			MGR_RECORDER_GetRecordPath(HUINT32 ulPvrIdx);
HBOOL 			MGR_RECORDER_IsTsrLockForDelayedRecord(HUINT32 ulContentId, HINT32 nEventId, HUINT16 nParental);
HBOOL			MGR_RECORDER_IsScrambleToDescrambleRecord(MgrRec_RecInfo_t *pstInfo);
HERROR			MGR_RECORDER_CreateRecordFailReason(MgrRec_RecSetup_t *pstRecSetup, MgrRec_EndReasonType_e eEndReason, DxSchedule_t	 *pstSchedule);
HERROR			MGR_RECORDER_SetRecordFailReason(HUINT32 ulContentsId, MgrRec_EndReasonType_e eEndReason);
HERROR			MGR_RECORDER_GetRecordFailReason(HUINT32 ulContentsId, MgrRec_EndReasonType_e *peEndReason);
HUINT32			MGR_RECORDER_GetRecorderParam (MgrRec_ParamType_e eType);
HERROR 			MGR_RECORDER_GetMakeSectionSiSpec (SvcSi_MakeSectionSpec_t *pstSiSpec);
HBOOL			MGR_RECORDER_IsDelayedRecCopyInProgress(void);
HERROR			MGR_RECORDER_SetRecordSetup(MgrRec_RecSetup_t *pstSetup, SvcRec_Setup_t *pstSvcRecSetup, SvcMeta_Record_t * pstMetaInfo, Handle_t hAction);
HBOOL			MGR_RECORDER_GetEncryptionMode(MgrRec_RecSetup_t *pstSetup);


#endif /* ___MGR_RECORDER_H___ */
