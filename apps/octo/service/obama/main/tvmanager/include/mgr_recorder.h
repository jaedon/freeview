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


	/* - Description : recorder app�� ��ȭ ������ �˸�
	   - Parameters Usage :
		   handle : action handle
		   p1 : recording type
		   p2 : cotentId
		   p3 : NA */
	eMEVT_PVR_NOTIFY_RECORD_STARTED,

	/* - Description : recorder app�� ��ȭ ���Ḧ �˸�
	   - Parameters Usage :
		   handle : NA
		   p1 : recording type
		   p2 : cotentId
		   p3 : NA */
	eMEVT_PVR_NOTIFY_RECORD_STOPPED,

	/* - Description : recorder app�� endtime �� Update �ش޶�� �޽���
	   - Parameters Usage :
		   handle : action handle
		   p1 : endtime
		   p2 : NA
		   p3 : NA */
	eMEVT_PVR_RECORD_ENDTIME_UPDATE,

	/* - Description : recorder app���� ��ȭ ���� splitting �� �˸��� �޽���
	   - Parameters Usage :
		   handle : action handle
		   p1 : reason
		   p2 : NA
		   p3 : NA */
	eMEVT_PVR_NOTIFY_RECORD_SPLIT_FILE,

	/* - Description : tsr record �� delayed record �� �����϶�� �޽���
	   - Parameters Usage :
		   handle : action handle
		   p1 : tsr_rec playtime
		   p2 : playtime by current time
		   p3 : NA */
	eMEVT_PVR_RECORD_CHANGE_TSR_REC_TO_DELAYED_RECORD,

	/* - Description : tsr record �� delayed record �� �����϶�� �޽����� ���
	   - Parameters Usage :
		   handle : action handle
		   p1 : slot
		   p2 : result (HERROR)
		   p3 : NA */
	eMEVT_PVR_RECORD_CHANGE_TSR_REC_TO_DELAYED_RECORD_RESULT,


	/* - Description : Record �� start�϶�� �޼���
	   - Parameters Usage :
		   handle : NULL
		   p1 : NA
		   p2 : NA
		   p3 : NA */
	eMEVT_PVR_RECORD_START_REC,

	/* - Description : Record �� stop�϶�� �޼���
	   - Parameters Usage :
		   handle : NULL
		   p1 : NA
		   p2 : NA
		   p3 : NA */
	eMEVT_PVR_RECORD_STOP_REC,

	/* - Description : TSR�� �������� �����϶�� �ϴ� �޽���
	   - Parameters Usage :
		   handle : NULL
		   p1 : NA
		   p2 : NA
		   p3 : NA */
	eMEVT_PVR_RECORD_START_TSR,

	/* - Description : TSR�� �������� ���߶�� �ϴ� �޽���
	   - Parameters Usage :
		   handle : NULL
		   p1 : NA
		   p2 : NA
		   p3 : NA */
	eMEVT_PVR_RECORD_STOP_TSR,

	/* - Description : TSR Set (on/off)�� �϶�� �޽���
	   - Parameters Usage :
		   handle : NULL
		   p1 : NA
		   p2 : NA
		   p3 : NA */
	eMEVT_PVR_TSR_SET_LIVE_TSR,

	/* - Description : Record �õ� �� �������� ��Ÿ���� �޽���
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
	eMEVT_PVR_TRREC_MGR_START,	// cmd test �뵵�� �������.���߿� ���� ���ۿ��� �̿�Ǹ� �� �ּ� �����ּ���



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





	/* transcoding�� ���� �϶�� �޽���  */
	eMEVT_PVR_STOP_TRANSCODING,


	/* - Description : Media Next FILE �޼���.
	   - Parameters Usage :
		   handle :
		   p1 : NA
		   p2 : NA
		   p3 : NA */
	eMEVT_PVR_MEDIA_NEXT_FILE,

	/* - Description : Medialist(Video) ȭ���� Update
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
	eMgrRecFailed_HDDFULL,				/**< HDD �뷮�� full �� ���� ��ȭ ������ ���� ���� */
	eMgrRecFailed_MAXNUM,					/**< Over than max record number */
	eMgrRecFailed_MAXRECORDLISTNUM,		/**< Over than max record_id number  1024 */
	eMgrRecFailed_LESS30SEC,				/**< Recording(less than 30 seconds) may not be stored */
	eMgrRecFailed_NOSIGNAL,				/**< No signal */
	eMgrRecFailed_NONSERVICE,				/**< Non-Service, Empty-Service */
	eMgrRecFailed_SEARCHING,				/**< ä�� �˻����� ��� */
	eMgrRecFailed_CONFLICT_TP,			/**< Conflict with higher priority recording */
	eMgrRecFailed_DRM,					/**< No Recording by DRM content */
	eMgrRecFailed_NOSC_SCR,				/**< scramble �����ε� smartcard �� ���� ��� */
	eMgrRecFailed_NORIGHT_SCR,			/**< scramble �����ε� descramble �� ������ ���� ��� */
	eMgrRecFailed_NOHDD,					/**< HDD �� ���ų� Format �� �� �� ���, Format ���� ��� */
	eMgrRecFailed_INCOMPLETE_HDDFULL	,			/**< recording ���� HDD �뷮�� full �� �� ��� */
	eMgrRecFailed_UNKNOWN,					/**< Unknown reason */
	eMgrRecFailed_NOUSBHDD,				/**< USB HDD �� ���� ��ȭ�� ���� ���, external pvr �� */
	eMgrRecFailed_DISCONNECTUSBHDD,		/**< USB HDD �� ��ȭ ���߿� disconnect �� ��� */
	eMgrRecFailed_EXTUSB_CopyDel,
	eMgrRecFailed_SAME_SVC_RECORDING,		/**< �̹� ������ ���񽺰� ��ȭ ������ */
	eMgrRecFailed_HIGHER_PRIORITY,		/* Auto Tracking Mode�� �� ��ö� Pending�� ���  */
	eMgrRecFailed_UNABLE_TRACKING,		/* Auto Tracking Mode�� �� Tracking ���� */
	eMgrRecFailed_NO_VIDEO_AUDIO,		/** data broadcasting without valid video */
	eMgrRecFailed_SI_TIMEOUT, 			/**< si timeout (PMT timeout, pmt pid remove)*/
	eMgrRecFailed_POWER_OFF,			/** < ac power off �� ��� */
} MgrRec_EndReasonType_e;

typedef enum
{
	eMgrRecState_IDLE			= 0,
	eMgrRecState_TUNE,					/**< ���ڵ� ������ ���� Tuning ���� ���� */
	eMgrRecState_NO_SIGNAL,			/**< Ʃ������ ������ ��ȣ�� ���� ���� */
	eMgrRecState_WAIT,					/** wait flag(PMT, CAS, DRM ...)������ ��ٸ��� ���� */
	/*************** �� ������ ���� PMT�� valid �� ���� �ƴ� ���� ���� ****************/
	eMgrRecState_BLOCKED,				/** pause flag(CAS, DRM ...)�� ���Ͽ� ���ڵ� ���ѵǾ� �ִ� ���� */
	eMgrRecState_RECORDING,			/**< �������� ���ڵ� ���� */
	eMgrRecState_STOPPED,

	eMgrRecState_Num,
} MgrRec_State_e;

typedef enum
{
	eMgrRecSession_NORMAL,			/**< Normal session */
	eMgrRecSession_MHEG_QUIET,		/**< MHEG, MHP�� data ��ۿ��� ��û�� silent tuning */
	eMgrRecSession_REPLACE,		/**< Free CA mode/running status change�� ���� replacement service tuning */
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
}MgrRec_TsrLimitForUsbFlash_t; // USB TSR�� ������ ��� LimitSize�� ����

typedef struct
{
	MgrRec_OutputType_e		eRecDst;			// ���ڵ� ��ü (HDD/EXT TsLine/Etc)
	MgrRec_RecordType_e		eRecType;			// ���ڵ� ����
	HINT32					eSessionType;		// ���ڵ��� ������ session type, watchtv ����,  Normal, Subfeed ��
	Handle_t				hMasterSvc;			// ���ڵ��� ������ masterhandle, watchtv ����, Normal �ΰ�� hCurrSvc �� ������, subfeed �ϰ�� masterfeed �� servicehandle ��.
	Handle_t				hCurrSvc;			// ���ڵ��� service handle
	HUINT32					ulLiveActionId;		// TSR_REC �� ������ ����� WTV �� actionID , TSR_REC �� destroy �� ���
	HUINT32					ulSlot;				// Schedule recording ������ ������ �������� ���� ����
	HUINT32					ulSourceContentId;	// Scramble to Descramble Recording �� Service ������ ���� ���� ���� playback ContentId
	HUINT32					ulPvrIdx;			// ���ڵ��� Pvr Storage index
	HBOOL					bStartPauseMode;	// record start �ϸ鼭 pause ���·� ���� ���� ���. default : false ��. false �̸� pause ���� �ʰ� �ٷ� ����.
	HBOOL					bWaitPpvPay; 		// PPV pay �� ���ٸ��� ���� �߻��� CAS fail event �� ���� end reason ������ �����ϱ� ����, PPV event �� �ݵ�� �ѹ� cas fail �� �ö� ���Ƿ�.
	HUINT32 				ulRecMode;			// for transcode recording

	Handle_t				hMemBuf;			// ���ڵ� ��ü�� Memery Buffer �� ��� Ring Buffer�� Handle

	HUINT8					szDisplayName[MAX_REC_FOLDER_NAME_LEN];
	HUINT8					szDeviceHandle[MAX_REC_DEVICEHANDLE_LEN];
	HUINT8					szDestinationId[MAX_REC_DEVICEHANDLE_LEN];

	MgrRec_TsrLimitForUsbFlash_t stUsbFlash; // USB TSR�� ������ ��� LimitSize�� ����

	HBOOL					bUseUserEncKey;
	HUINT8			 		aucKey[PAL_PVR_CRYPTO_KEY_LEN];
} MgrRec_RecSetup_t;

typedef struct
{
	HUINT32					ulContentId;	/**< booting ������ ts ���� ���/��ȭ�� ���� ������ ID */
	MgrRec_OutputType_e		eRecDst;
	MgrRec_RecordType_e		eRecType;		/**< recording method */
	MgrRec_State_e			eRecState;		/**< record status */
	Handle_t				hSvc;			/**< service handle to record */
	Handle_t				hMasterSvc;		/**< Supplement service �� ��쿡 master handle */
	HINT32					nEventId;		/** <EBR�� Event ID  */
	HINT32					eSessionType;	/**< Normal/Quiet/Replacement/MultiFeed �� WTV session tuning type */
	HINT32 					nChNum;			/**< channel service number */
	UNIXTIME				ulRecTime;		/**< recording time, duration */
	UNIXTIME				ulStartTime;	/**< recording start time */
	UNIXTIME				ulEndTime;		/**< recording end time */
	UNIXTIME				ulOrgStartTime;	/**< user's first recording start time */
	UNIXTIME				ulLastTime;		/**< Lastest recording time */
	HUINT32					ulSlot;			/**< schedule slot number */
	HUINT32					ulLiveActionId;	/**< TSR_REC �� ������ ����� WTV �� actionID , TSR_REC �� destroy �� ��� */
	Handle_t				hAction;		/**< session version�� ���Ե� action handle */
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
