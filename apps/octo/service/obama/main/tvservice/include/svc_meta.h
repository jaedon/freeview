
#include "htype.h"


#ifndef __SVC_META_H__
#define __SVC_META_H__

#define SVC_META_NOT_CONTAIN_ALL_CONTENTS

#define PVR_MAX_SERVICE_NAME_LEN			/*CONFIG_DB_SVC_MAX_NAME_BYTES*/DbSvc_GetNameLength()
#define PVR_SHORT_EVENT_NAME_LEN			256
#define PVR_EVENT_TEXT						256
#define PVR_PARENTAL_RATING_MAX				3
#define PVR_EXT_ITEM_MAX					15
#define MAX_AUD_NUM							4
#define PVR_MAX_TITLE_LEN					128
#define	PVR_INVALID_REF_TIME				(0)

#if defined(CONFIG_MW_CI_PLUS_V_1_3)
#define SVC_META_CAMID_LEN	8
#endif

#if defined(CONFIG_OP_UK_DTT)
#define MAX_RECORD_LIST		1500
#endif

typedef enum
{
	eSEVT_META_				= eSEVT_META_START,

	eSEVT_META_ADDED,
	eSEVT_META_UPDATED,
	eSEVT_META_REMOVED
} SvcMeta_EvtMsg_e;

typedef enum
{
	eSvcMeta_Guidance_Type_LANGCODE		= 0,
	eSvcMeta_Guidance_Type_MODE_SPECIFIED,
	eSvcMeta_Guidance_Type_DATA,
	eSvcMeta_Guidance_Type_NONE			= 0XFF
} SvcMeta_GuidanceType_e;

typedef enum
{
	eSvcMeta_UsingActType_None,
	eSvcMeta_UsingActType_Record,
	eSvcMeta_UsingActType_Playback,

	eSvcMeta_UsingActType_EndOfCase
} SvcMeta_UsingActionType_e;

typedef enum
{
	eSvcMeta_Timestamp_START			= 0x00000000,
	eSvcMeta_Timestamp_PMT				= 0x00000001,
	eSvcMeta_Timestamp_EIT				= 0x00000002,
	eSvcMeta_Timestamp_DRM				= 0x00000004,
#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
	eSvcMeta_Timestamp_LICENSE_DRM		= 0x00000008,
#endif

	eSvcMeta_Timestamp_END				= 0xFFFFFFFF
} SvcMeta_TimeStampType_e;

typedef struct
{
	AUDIO_Type_t		eAudType;
	HUINT8			szIso639LangCode[LANG_639_CODE_MAX+1];
} SvcMeta_Iso639LangInfo_t;

typedef struct
{
	HUINT32					ulLangNum;
	SvcMeta_Iso639LangInfo_t	stLangInfo[MAX_AUD_NUM];
} SvcMeta_PvrAudioInfo_t;

typedef struct
{
	HUINT8				ucComponentType;		/** < Teletext: 0x00~0x05, Subtitle: 0x10~0x13, 0x20~0x23 */
	HUINT8				aucLangCode [LANG_639_CODE_MAX+1];
} SvcMeta_SbtlInfo_t;

#if defined(CONFIG_MW_MM_PVR)
typedef struct
{
	HUINT32     		ulItemNum;				/**< Subtitle 개수 */
	SvcMeta_SbtlInfo_t	astSbtlInfo[SvcRec_MAX_SUBTTL_ES_NUM];
} SvcMeta_SbtlInfoList_t;

typedef struct
{
	HUINT32     		ulItemNum;
	SvcMeta_PvrAudioInfo_t	astAudioInfo[SvcRec_MAX_AUDIO_ES_NUM];
} SvcMeta_AudioInfoList_t;
#endif


typedef struct {
	HINT32				nChNumber;
	HUINT8				aucServiceName[PVR_MAX_SERVICE_NAME_LEN];

	HUINT16				usSvcId;           /**< 서치시 할당됨. 변경 불가. service id */
	HUINT16				usTsId;            /**< 서치시 할당됨. 변경 불가. transport stream id */
	HUINT16				usOnId;            /**< 서치시 할당됨. 변경 불가. original network id */
#if defined(CONFIG_OP_FREESAT)
	HUINT32				nMasterChNumber;
	HUINT8				aucMasterServiceName[PVR_MAX_SERVICE_NAME_LEN];
	HUINT16				usMasterSvcId;
	HUINT16				usMasterTsId;
	HUINT16				usMasterOnId;
#endif
	/*----------------------------------------------------------------------------------------------------
		PID & CODEC
	----------------------------------------------------------------------------------------------------*/
	HUINT16				usPmtPid;				/**< pmt pid, pat에 정의됨 */
	HUINT16				usPcrPid;				/**< pcr pid from pmt */
	HUINT16				usTtxPid;				/**< teletext pid from pmt */
	HUINT16				usCaptionPid;			/**< caption pid from pmt */

	HUINT16				usVideoPid;				/**< video pid from pmt */
	DxVideoCodec_e		eVideoCodec;			/**< 해당 es의 video codec, db file에 1 byte로 저장됨 */

	HUINT16				usAudioPid;				/**< audio pid from pmt */
	DxAudioCodec_e		eAudioCodec;			/**< 해당 es의 audio codec, db file에 1 byte로 저장됨 */

	HUINT16				usAudioAuxPid;			/**< for audio descriptor or dolby */
	DxAudioCodec_e		eAudioAuxCodec;			/**< 해당 es의 audio codec, db file에 1 byte로 저장됨 */

	HUINT16				usDolbyPid;				/**< dolby audio pid from pmt */
	DxAudioCodec_e		eDolbyCodec;			/**< 해당 es의 audio codec, db file에 1 byte로 저장됨 */
	HBOOL				bDolbyFlag;				/**< main audio가 dolby인 경우 TRUE로 세팅 */
												/*----------------------------------------------------------------------
													bDolbyFlag이 TRUE가 되는 경우는,
													(1) sound track 창에서 dolby audio를 선택하거나,
													(2) menu의 dolby priority가 설정되고, dolby audio가 들어오는 서비스
														-> MW SI에서 판단하여 설정됨
												-----------------------------------------------------------------------*/
	/*----------------------------------------------------------------------------------------------------
		다음은 service 속성들이다.
	----------------------------------------------------------------------------------------------------*/
	DxSvcType_e			eSvcType;				/**< Service Type defined by Humax, all/tv/radio/data --> SDT based */
	DxVideoType_e		eVideoType;				/**< all/sd/hd */	// NOTE 일본향에서는 해당 Field 사용하지 않고, stAribControlData.u8VideoResolution 을 이용한다. [20120911]
	DxCasType_e			eCasType;				/**< ca system id에 따라 미리 정의된 값으로 OR 해서 여러개의 CAS가 동시에 지정될 수 있다. all/fta/nagra/irdeto/nds/ ... */

	/*----------------------------------------------------------------------------------------------------
		user에 의한 설정값들
	----------------------------------------------------------------------------------------------------*/
	HUINT8				ucSubttlIdx;			/**< user에 의해 선택 된 Subtitle Index */
	HBOOL				bSubttlUserFlag;		/**< user에 의해 선택 되었는지를 나타내는 Flag */
	DxStereoSelect_e		eSoundMode;				/**< sound mode : STEREO, MONO L, MONO R, MONO L/R */
	DxDualMonoSelect_e	eDualMono;				/* Stereo가 아닌 Dualmono관리 현재는 필요없지만 먼 훗날 Content별 DualMono 선택을 관리 할 경우 필요해서 추가함 */
	HBOOL				bAudUserSetFlag;		/**< sound track menu에서 user가 다른 언어나 dolby audio 등을 선택했는지 안했는 지의 여부를 나타냄 */

	// subtitle & audio info
#if defined(CONFIG_MW_MM_PVR)
	SvcMeta_AudioInfoList_t	stAudioInfoList;
	SvcMeta_SbtlInfoList_t	stSbtlInfoList;
#endif

	// 기존 모델과 호환성을 위해서 처리함..
#if defined(SVC_AUDIO_LANGUAGE_CODE) && defined(SVC_SUBTITLE_LANGUAGE_CODE)
	HUINT8			audioLanguage[DxLANG_CODE_LEN];
	HBOOL			audioDescription;

	HUINT8			ucSubtitleLanguage[DxLANG_CODE_LEN];
	HBOOL			bHardOfHearing;

	HUINT8			aucReserved[16];
#else
	HUINT8			aucReserved[32];
#endif
} SvcMeta_Service_t;

#define SVC_PVR_BOOKMARK_NAME_LEN	32

typedef	struct
{
	HUINT32 ulPos;
	HCHAR 	szName[SVC_PVR_BOOKMARK_NAME_LEN+1];
} SvcMeta_Bookmark_t;

typedef struct
{
	// Record
	HUINT8			aucPath[FS_MAX_FILE_NAME_LEN];
	HUINT8			aucFileName[FS_MAX_FILE_NAME_LEN];
	UNIXTIME		ulStartTime;
	UNIXTIME		ulEndTime;
	UNIXTIME		ulRecTime;
	UNIXTIME		ulOrgDuration;
	UNIXTIME		ulLastTime;

	HINT32			ucRecordStatus;
	HBOOL			u2cEventBase;
	HINT32			nEventId;
	HBOOL			u1cLocked;
	HINT32			u1cWatched;
	HBOOL			u1cScrambled; // exist any scramble position in record file
	HBOOL			u1cEncryption;
	HBOOL			u1cAutoDeleted;

	HUINT8			ucEndCondition;			// reason to Record Fail, Recocrd Split
	HUINT32			ulLastWatchtime;
	HBOOL			bUserLocked;			// true means locked at the record menu (or record list)
											// u1cLocked means service lock or parental lock or watch limited
	HBOOL			bWillBeDeleted;
	HBOOL			bDoNotDelete;
	HUINT32			ulDoNotDeleteSetTime;
	HUINT16			usNumOfBookmark;
	HUINT16			usNumOfPausedPoints;
	SvcMeta_Bookmark_t	astBookmarks[DxPVR_NUM_BOOKMARK];
	SvcMeta_Bookmark_t	astPausedPoints[DxPVR_NUM_BOOKMARK];
	HUINT8			aucDisplayEvtName[PVR_MAX_TITLE_LEN];			// Medialist Display Name

	DxCopyrightControl_t		stDrmInfo;

	HUINT8			bSeriesRec;
	HUINT8			szSeriesId[EPG_TVA_CRID_TOTAL_LEN];
	HUINT32			ulEpisodeNum;
	HUINT16			ulSeason;
	HUINT8          szLongText[DxRECLIST_EXT_TEXT_BUFSIZE];
	// Model Specific Data
#if defined(CONFIG_OP_FREESAT) || defined (CONFIG_OP_UK_DTT)
	HUINT32			ulFtaCmVal;// FTA content management reference (PVR_FTA_CopyControl_e value)
	HUINT8			ucCopyTime;
	HUINT32			ulTimeOffsetAtRec;
#endif

#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	DxPVR_TvaCrid_t	stCrid[DxMAX_CRID_DATA_NUM];

#if defined(CONFIG_OP_TDC)
	HUINT8			aucSeriesTitle[DxMAX_SERIES_TITLE_LEN];
#endif

#endif
	// Transcode module의 공용화를 위해 일본향에서 사용하는 meta를 임시로 공통으로 가지게 한다.
	HBOOL		bTranscoded;
	HUINT32 		ulRecMode;			 // 1 : DR,  2 : 2배,	3 : 3배, 5 : 5배,	7 : 7배
	HUINT16		usTrRecState;			// 0: not started, 1: started, 2: finished (TR중에 HDD 제거 혹은 전원 off시 판단하기위한 field)

#if defined(CONFIG_MW_CI)
	HUINT32			ulInstType;
	HUINT16			usSlotId;
  #if defined(CONFIG_MW_CI_PLUS_V_1_3) || defined(CONFIG_MW_CI_PLUS_CC_V_2)
	HBOOL			bCamNeedContent;
	HUINT8			aucBindedCamId[SVC_META_CAMID_LEN];
  #endif
#endif
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	HUINT8			ucSCPinCodeFlag;
	HUINT8			aucReserved[31];
#else
	HUINT8			aucReserved[32];
#endif

#if defined(CONFIG_MW_CAS_CONAX)
	HCHAR			szConaxMetaName[FS_MAX_FILE_NAME_LEN];
#endif
} SvcMeta_Record_t;

typedef struct
{
	HUINT8					aucIsoCountryCode[LANG_639_CODE_MAX + 1];
	HUINT8					ucParentalRating;
} SvcMeta_Rating_t;

typedef struct
{
	HUINT8					ucStreamContent;
	HUINT8					ucComponentType;
	HUINT8					aucIsoCountryCode[LANG_639_CODE_MAX + 1];
} SvcMeta_Component_t;

typedef struct
{
	HUINT8	*pszDescription;
	HUINT8	*pszItem;
} SvcMeta_ExtEvtItem_t;

typedef struct tagPVR_EXT_EVT
{
	HUINT8	ucDesNum;
	HUINT8	ucLastDesNum;
	HUINT8	szIso639LangCode[4];
	HUINT8	ucItemNum;
	SvcMeta_ExtEvtItem_t	astItem[PVR_EXT_ITEM_MAX];
	HUINT8	*pszChar;

	struct tagPVR_EXT_EVT *pNext;
} SvcMeta_ExtEvt_t;

typedef struct tagPVR_EVT
{
	UNIXTIME		ulStartTime;
	UNIXTIME		ulDuration;
	HINT32			u3RunnigStatus;
	HINT32			u1FreeCaMode;
	HINT32			ucReserved;

	HUINT8			ucParentalRatingNum;
	SvcMeta_Rating_t	astParentalRating[PVR_PARENTAL_RATING_MAX];
	HUINT8			ucContentNibble;
	HUINT8			ucUserNibble;

	HUINT8			aucEventName[PVR_SHORT_EVENT_NAME_LEN];
	HUINT8			aucEventText[PVR_EVENT_TEXT];
	HINT32			nEventId;
	HUINT8			aucReserved[32];

	HBOOL					bUseFtaCntMgmt;
	HBOOL					bUseGuidance;
#if defined(CONFIG_OP_FREESAT)
	DxFreesat_PVR_FtaCntMgmt_t	ftaCntMgmt;
#endif
#if defined(CONFIG_OP_UK_DTT)
	DxUkdtt_Pvr_FtaCntMgmt_t	stFtaCntMgmt;
	DxUkdtt_Pvr_Guidance_t		stGuidance;
#endif
	SvcMeta_Component_t stComponent[EPG_COMPONENT_DESC_MAX];

	SvcMeta_ExtEvt_t		*pstExtEvt;

// 향후 지원 예정 (by leeyh) : ulOffsetOfPlayTime, pNext
	HUINT32				ulOffsetOfPlayTime;
	struct tagPVR_EVT	*pNext;

}SvcMeta_Event_t;

#if defined(SVC_META_NOT_CONTAIN_ALL_CONTENTS)
typedef enum
{
	eSVC_META_CONTENTS_OBJTYPE_FullSize		= 0,
	eSVC_META_CONTENTS_OBJTYPE_Brief,

	eSVC_META_CONTENTS_TYPE_EndOfCase
} SvcMeta_ContentsType_e;

typedef struct
{
	SvcMeta_ContentsType_e	 eObjectType;

	HUINT32				 ulContentsId;
	HCHAR				*pszUrl;
	HCHAR				*pszHjmPath;
} SvcMeta_BriefContents_t;
#endif

typedef struct
{
#if defined(SVC_META_NOT_CONTAIN_ALL_CONTENTS)
	SvcMeta_ContentsType_e	 eObjectType;
#endif
	HUINT32				ulContentsId;
	HBOOL				bDeleteSkip;
	HCHAR				szURL[DxRECLIST_URL];
	HUINT32				ulChipId;

	HBOOL				bHasRecord;
	HBOOL				bHasService;
	HBOOL				bHasEvent;

	SvcMeta_Record_t		stRecordInfo;  // Record Info
	SvcMeta_Service_t		stServiceInfo; // Service Info
	SvcMeta_Event_t		stEventInfo;	 // Event Info
} SvcMeta_Contents_t;


typedef struct {
	SvcMeta_TimeStampType_e			eType;
	HUINT32						ulTime;
	HUINT32						ulPosition;
	HUINT32						ulSize;
	HBOOL						bIsUsed;
	HUINT8						aucReserved[16];
}SvcMeta_Timestamp_t;


typedef struct {
	SvcMeta_Timestamp_t 				stTimeStamp;
	void							*pData;
}SvcMeta_TimestampData_t;


typedef struct {
	HUINT32								ulNumber;
	HUINT32								ulRefTime;
	HUINT32								ulPmtRefTime, ulEitRefTime, ulDrmRefTime, ulDrmUriRefTime, ulDrmLicenseRefTime;
	HUINT32								ulDrmUriRefIdx, ulDrmPcRefIdx, ulDrmLicenseRefIdx;
	eDxCopyrightControl_CiPlusDrmType_e	eCiPlusDrmType;
	SvcMeta_TimestampData_t				*pstData;
}SvcMeta_TimestampList_t;

typedef void* (*SvcMeta_AllocFunc)(HUINT32 ulSize);

void		SVC_META_MuteEvent (HBOOL mute);
HBOOL		SVC_META_IsTsrContents (HUINT32 ulContentsId);
HERROR		SVC_META_AddContentsInfo(Handle_t hSvc, SvcMeta_Record_t *pstRecordInfo, HUINT32 *pulContentsId);
HERROR		SVC_META_GetServiceInfo( HUINT32 ulContentsId, SvcMeta_Service_t *pstInfo );
HERROR		SVC_META_SetServiceInfo( HUINT32 ulContentsId, SvcMeta_Service_t *pstInfo, HBOOL bSave );
HERROR		SVC_META_GetEventList( HUINT32 ulContentsId, SvcMeta_Event_t **ppstInfo, SvcMeta_AllocFunc cbAlloc );
HERROR		SVC_META_GetEventInfo( HUINT32 ulContentsId, SvcMeta_Event_t *pstInfo );
HERROR		SVC_META_SetEventInfo( HUINT32 ulContentsId, SvcMeta_Event_t *pstInfo, HBOOL bSave);
HERROR		SVC_META_SetEventTimeStamp( HUINT32 ulContentsId, HUINT32 ulTime, SvcMeta_Event_t *pstInfo );
HERROR		SVC_META_SetDrmTimeStamp( HUINT32 ulContentsId, HUINT32 ulTime, DxCopyrightControl_t *pstInfo );
HERROR		SVC_META_UpdateDrmTimeStampByIndex( HUINT32 ulContentsId, DxCopyrightControl_t *pstInfo, HUINT32 ulMetaIndex );
HERROR		SVC_META_SetServiceTimeStamp( HUINT32 ulContentsId, HUINT32 ulTime, SvcMeta_Service_t *pstInfo );
HERROR		SVC_META_LoadTimeStamp( HUINT32 ulContentsId, SvcMeta_TimestampList_t *pstTimeStampList );
HERROR		SVC_META_UnloadTimeStamp( SvcMeta_TimestampList_t *pstList );
HERROR		SVC_META_IsEncrypt( HUINT8 *pucFileName, HBOOL  *pbEncrypt );
HERROR		SVC_META_GetDrmInfoByName( HUINT8 *pucFileName, HUINT8  *pucDrm, HUINT32 *pulExpired );
HERROR		SVC_META_ReadRecordInfo( HUINT8 *pucFileName, SvcMeta_Record_t *pstInfo );
HERROR		SVC_META_ReadContentInfo(HUINT8 *pucFileName, SvcMeta_Record_t *pstInfo, SvcMeta_Service_t *pstSvcInfo, SvcMeta_Event_t *pstEvtInfo );
HERROR 		SVC_META_WriteRecordInfo( HUINT8 *pucFileName, SvcMeta_Record_t *pstInfo );
HERROR		SVC_META_GetRecordUrl( HUINT32 ulContentsId, HCHAR *pszUrl, HUINT32	ulBufLen);
HERROR		SVC_META_GetRecordInfo( HUINT32 ulContentsId, SvcMeta_Record_t *pstInfo );
HERROR		SVC_META_SetRecordInfo( HUINT32 ulContentsId, SvcMeta_Record_t *pstInfo);
HERROR		SVC_META_SetRecordInfoWithOption( HUINT32 ulContentsId, SvcMeta_Record_t *pstInfo, HBOOL bSave, HBOOL bEmitEvent);
HERROR		SVC_META_GetThumbnail( HUINT32 ulContentsId, HUINT32 **pulData );
HERROR		SVC_META_ReleaseThumbnail( HUINT32 *pulData );
HERROR		SVC_META_GetDRMInfo( HUINT32 ulContentsId, DxCopyrightControl_t *pstInfo );
HERROR		SVC_META_SetDRMInfo( HUINT32 ulContentsId, DxCopyrightControl_t *pstInfo, HBOOL bSave );
HERROR 		SVC_META_CheckExpiredByRecordInfo( SvcMeta_Record_t *pstRecInfo, HBOOL *pbExpired, UNIXTIME *pulExpiredTime );
HERROR		SVC_META_DeleteContents( HUINT32 ulContentsId );
HBOOL		SVC_META_IsExistTSFileContents( HUINT32 ulContentsId );
HERROR		SVC_META_GetLastPlayTime( HUINT32 ulContentsId, HUINT32 *pulLastPlayTime	);
HERROR		SVC_META_SetLastPlayTime( HUINT32 ulContentsId, HUINT32 ulLastPlayTime );
HERROR		SVC_META_AddBookmark( HUINT32 ulContentsId, HCHAR *szName, HUINT32 ulPosition, HBOOL bUseUpdateEvent);
HERROR		SVC_META_AddPausedPoint( HUINT32 ulContentsId, HCHAR *szName, HUINT32 ulPosition, HBOOL bUseUpdateEvent);
HERROR		SVC_META_GetBookmarks( HUINT32 ulContentsId, HUINT16 *pulNumOfBookmark, SvcMeta_Bookmark_t **ppstBookmarks);
HERROR		SVC_META_GetPausedPoints( HUINT32 ulContentsId, HUINT16 *pulNumOfPausedPoints, SvcMeta_Bookmark_t **ppstPausedPoints);
void		SVC_META_FreeBookmarks (SvcMeta_Bookmark_t *pstBookmarks);
HERROR		SVC_META_RemoveBookmark( HUINT32 ulContentsId, SvcMeta_Bookmark_t	*pstBookmark, HBOOL bUseUpdateEvent);
SvcMeta_ExtEvt_t *	SVC_META_CloneExtEvt(const SvcMeta_ExtEvt_t *pstSrc);
void		SVC_META_FreeExtEvt(SvcMeta_ExtEvt_t *pstExtEvt);
HERROR		SVC_META_LoadContentsInfo(const HCHAR *szPath);
HERROR		SVC_META_UnloadContentsInfo( void );
HERROR		SVC_META_UnloadContentsInfoByPath(const HCHAR *szPath);
HERROR		SVC_META_UnloadAllContentsInfo( void );
HUINT64		SVC_META_GetRecycleBinSize (const HCHAR *szMountPath);
HERROR		SVC_META_GetContentsNumber( const HCHAR *szPath, HUINT32 *pulNumber );
HERROR		SVC_META_CountContentsIdList (HUINT32 *pulNumber);
HERROR		SVC_META_GetContentsIdList( HUINT32 *pulNumber, HUINT32 **pulContentsList	);
HERROR		SVC_META_FreeContentsIdList( HUINT32 *pulContentsList );
HERROR		SVC_META_GetContentsIdFromUrl( HUINT32 *pulContentsId, const HCHAR *pszUrl);
HERROR		SVC_META_GetContentsIdFromFullname( HUINT32 *pulContentsId, HCHAR *pszFullNameWithoutExt);
HERROR		SVC_META_MakeFileName( UNIXTIME ulTime, const HCHAR *pucSourceName, HCHAR *pucTargetName, HUINT32 n);
HERROR		SVC_META_SetAsDeleteSkipContent ( HUINT32 ulContentsId, HBOOL bDeleteSkip );
HERROR		SVC_META_GetService( Handle_t hAct, SvcMeta_Service_t *pstInfo );
HERROR		SVC_META_MakeMetaSvcBySvcInfo(DbSvc_Info_t * svc, SvcMeta_Service_t * MetaSvc);
HERROR		SVC_META_MakeSvcInfoByMetaSvc(SvcMeta_Service_t * MetaSvc, DbSvc_Info_t * svc);
HERROR		SVC_META_GetEvent( Handle_t hAct, SvcMeta_Event_t *pstInfo );
HERROR		SVC_META_CopyContents( HUINT32 ulContentsId, const HCHAR *szPath, HxReturnFunc_t cbDone, void *pvUserdata);
HERROR		SVC_META_MoveContents( HUINT32 ulContentsId, const HCHAR *szPath, HxReturnFunc_t cbDone, void *pvUserdata );
HERROR		SVC_META_RenameContents( HUINT32 ulContentsId, const HCHAR *szPath, const HCHAR *szNewFileName, const HCHAR *szNewEventName);
HERROR		SVC_META_CancelCopyOperation (HUINT32 ulContentsId);
HERROR		SVC_META_CancelMoveOperation (HUINT32 ulContentsId);

SvcMeta_ExtEvt_t *SVC_META_GetMergedExtEvtInfo(SvcMeta_ExtEvt_t *pstExtEvt);

void		SVC_META_PrintServiceInfo( const SvcMeta_Service_t *pstSvc );

HERROR		SVC_META_SaveTimeStampTsrToDelayedRec( HUINT32 ulContentsId, UNIXTIME ulTimeoffset, HUINT8 *pucPathName);
HERROR		SVC_META_CheckDrmExpired(DxCopyrightControl_t *pstDrmInfo, UNIXTIME ulStartTime, HBOOL *pbExpired, UNIXTIME *pulExpiredTime );
void		SVC_META_Decrypt (const HCHAR *szPath);
HERROR		SVC_META_GetDRMInfoByTime( HUINT32 ulContentsId, eDxCopyrightControl_CiPlusDrmType_e eDrmType, HUINT32 ulSourceTime, DxCopyrightControl_t *pstInfo );
HERROR		SVC_META_ConvertEpgExtendedTextToString(SvcMeta_ExtEvt_t* pstExtEvt, HUINT8 * * ppucString);
HERROR		SVC_META_ConvertEpgExtEvtItemTextToString(SvcMeta_ExtEvt_t* pstExtEvt, HUINT8 ** ppucDescString, HUINT8 ** ppucItemString);


HERROR		SVC_META_RegisterPvrAction(Handle_t hAction, HUINT32 ulContentId, SvcMeta_UsingActionType_e	ePvrActionType);
HERROR		SVC_META_UnRegisterPvrAction(Handle_t hAction);
HERROR		SVC_META_GetPlaybackActionByContentId( HUINT32 ulContentId, Handle_t *phAction );
HERROR 		SVC_META_GetRecActionByContentId( HUINT32 ulContentId, Handle_t *phAction );
HERROR		SVC_META_GetContentIdByhAction( Handle_t hAction, HUINT32 *pulContentId );
HERROR 		SVC_META_Init(PostMsgToMgrCb_t fnPostMsgToMgr);


#endif	// __SVC_META_H__
