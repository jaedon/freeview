#ifndef	__DLNA_INT_H__
#define	__DLNA_INT_H__
#include <hlib.h>
#include <apk_deprecated.h>
#include <apk_network.h>

/*-----------------------------------------------------------------------------
 *  common
 *-----------------------------------------------------------------------------*/
typedef void (*APK_DLNA_COMMON_CheckHommaStatusCallback)(HBOOL bConnected);
extern HERROR APK_DLNA_COMMON_RegisterNotifier(APK_DLNA_COMMON_CheckHommaStatusCallback fnCallback);
extern HERROR APK_DLNA_COMMON_UnRegisterNotifier(void);

/*-----------------------------------------------------------------------------
 *  dmp
 *-----------------------------------------------------------------------------*/

#define APKD_DLNA_DMP_MAX_TITLE			(128)
#define APKD_DLNA_DMP_MAX_URI			(1024)
#define APKD_DLNA_DMP_MAX_UDN			(48)
#define APKD_DLNA_DMP_MAX_CID			(512)

#define APKD_DLNA_DMP_MAX_CLASS			(128)
#define APKD_DLNA_DMP_MAX_OBJECTTYPE	(16)
#define APKD_DLNA_DMP_MAX_DATE			(16)
#define APKD_DLNA_DMP_MAX_DURATION		(32)
#define APKD_DLNA_DMP_MAX_RESOLUTION	(32)
#define APKD_DLNA_DMP_MAX_DIGITALCOPLY	(16)
#define APKD_DLNA_DMP_MAX_GENRE			(32)

#define APKD_DLNA_DMP_MAX_PARENTAL		(3)
#define APKD_DLNA_DMP_MAX_BOOKMARK		(32)
#define APKD_DLNA_DMP_MAX_DESC			(256)
#define APKD_DLNA_DMP_MAX_LONG_DESC		(256 * 8)
#define APKD_DLNA_DMP_MAX_UNKNOWN		(128)
#define APKD_DLNA_DMP_MAX_UNKNOWN_CAP	(8)
#define APKD_DLNA_DMP_MAX_SORTORDER_STR (128)

#define APKD_DLNA_DMP_LANG_ISO_639_LEN	(3)
#define APKD_DLNA_DMP_BOOKMARK_NAME_LEN (48)

#define	APKD_DLNA_DMP_MAX_RECORD_DST	(8)

typedef enum
{
	eDLNA_DMP_ServerAdd			= 0,
	eDLNA_DMP_ServerRemove,
	eDLNA_DMP_ServerList,
	eDLNA_DMP_ContentAdd,
	eDLNA_DMP_ContentEnd,
	eDLNA_DMP_SearchResult,
	eDLNA_DMP_SearchAllResult,

	eDLNA_DMP_StartCmd, 	/* Dmp Start */
	eDLNA_DMP_StopCmd, 		/* Dmp Stop */
	eDLNA_DMP_SearchCmd, 	/* Dmp Browse/Search */
	eDLNA_DMP_RemoveCmd,	/* Item remove(delete) */
	eDLNA_DMP_RefreshDms,	/* Dms Refresh */

	eDLNA_DMP_GetRecordDestination,
	eDLNA_DMP_GetDmsDiskInfo,
	eDLNA_DMP_GetUploadStatus,

	eDLNA_DMP_BrowseError,
	eDLNA_DMP_UnknownType

} APKE_DLNA_DMP_Property_e;

typedef struct
{
	HCHAR	szUDN[APKD_DLNA_DMP_MAX_UDN];
	HCHAR	szCID[APKD_DLNA_DMP_MAX_CID + 128];
	HCHAR	szSort[APKD_DLNA_DMP_MAX_SORTORDER_STR];
	HINT32	nStartIdx;
	HINT32	nRequestNum;
} APKS_DLNA_DMP_SearchTerms_t;

typedef struct
{
	HCHAR	szId[128];
	HCHAR	szName[128];
} APKS_DLNA_DMP_RecordDestination_t;

typedef struct
{
	HUINT64  Total; /* Byte */
	HUINT64  Available; /* Byte */
} APKS_DLNA_DMP_Storage_t;

typedef struct
{
	HCHAR	szUDN[APKD_DLNA_DMP_MAX_UDN];
	HCHAR	szCID[APKD_DLNA_DMP_MAX_CID + 128];
	HINT32	nRetVal;
} APKS_DLNA_DMP_RemoveItemResult_t;


typedef struct
{
	HCHAR	szTitle[APKD_DLNA_DMP_MAX_TITLE]; /* FriendlyName */
	HCHAR	szUDN[APKD_DLNA_DMP_MAX_UDN];
	HCHAR	szIP[APKD_NETWORK_MAX_IP4ADDR];

	HINT32	nDeviceHandle;
	HCHAR	szMACAddress[APKD_DLNA_DMP_MAX_UNKNOWN];
	HCHAR	szTotalStroageSize[APKD_DLNA_DMP_MAX_UNKNOWN];

	HBOOL	bIsSupportSrs;

	HUINT8	ucSortCapNum;
	HUINT8	ucSearchCapNum;
	HUINT8	ucDlnaCapNum;
	HUINT8	ucJlabsCapNum;

	HCHAR	szSortCAP[APKD_DLNA_DMP_MAX_UNKNOWN_CAP][APKD_DLNA_DMP_MAX_UNKNOWN];
	HCHAR	szSearchCAP[APKD_DLNA_DMP_MAX_UNKNOWN_CAP][APKD_DLNA_DMP_MAX_UNKNOWN];
	HCHAR	szDlnaCAP[APKD_DLNA_DMP_MAX_UNKNOWN_CAP][APKD_DLNA_DMP_MAX_UNKNOWN];
	HCHAR	szJlabsCAP[APKD_DLNA_DMP_MAX_UNKNOWN_CAP][APKD_DLNA_DMP_MAX_UNKNOWN];
} APKS_DLNA_DMP_ServerDevice_t;

typedef struct
{
	HCHAR	aucIsoCountryCode[APKD_DLNA_DMP_LANG_ISO_639_LEN+1];
	HUINT8	ucParentalRating;
}APKS_DLNA_DMP_PARENTAL_RATING_t;

typedef struct
{
	HUINT32 ulPos;
	HCHAR   szName[APKD_DLNA_DMP_BOOKMARK_NAME_LEN];
} APKS_DLNA_DMP_Bookmark_t;

typedef struct
{
	HCHAR	szTitle[APKD_DLNA_DMP_MAX_TITLE];
	HCHAR	szClass[APKD_DLNA_DMP_MAX_CLASS];
	HCHAR	szObjectType[APKD_DLNA_DMP_MAX_OBJECTTYPE];
	HCHAR	szUDN[APKD_DLNA_DMP_MAX_UDN];
	HCHAR	szCID[APKD_DLNA_DMP_MAX_CID];
	HCHAR	szURI[APKD_DLNA_DMP_MAX_URI];
	HCHAR	szThumnail[APKD_DLNA_DMP_MAX_URI];

	HBOOL	bByteSeekAble;
	HBOOL	bTimeSeekAble;

	HUINT32 ulTotalMatches;
	HUINT32 ulChildCount;
	HUINT32	ulFileSize;
	HUINT32	ulBitRate;
	HCHAR	szDuration[APKD_DLNA_DMP_MAX_DURATION];
	HUINT32	ulAudioChannels;
	HUINT32	ulSampleRateFreq;
	HCHAR	szAlbumTitle[APKD_DLNA_DMP_MAX_TITLE];
	HCHAR	szGenre[APKD_DLNA_DMP_MAX_GENRE];
	HCHAR	szArtist[APKD_DLNA_DMP_MAX_TITLE];
	HCHAR	szResolution[APKD_DLNA_DMP_MAX_RESOLUTION];

	HBOOL	restricted;
	HCHAR	szDate[APKD_DLNA_DMP_MAX_DATE];
	HCHAR	szAuthor[APKD_DLNA_DMP_MAX_UNKNOWN];
	HCHAR	szObjectID[APKD_DLNA_DMP_MAX_UNKNOWN];
	HCHAR	szChannelNum[APKD_DLNA_DMP_MAX_UNKNOWN];
	HCHAR	szChannelName[APKD_DLNA_DMP_MAX_UNKNOWN];
	HCHAR	szRadioBand[APKD_DLNA_DMP_MAX_UNKNOWN];
	HCHAR	szDigitalCopyControl[APKD_DLNA_DMP_MAX_UNKNOWN];
	HCHAR	szAnalogCopyControl[APKD_DLNA_DMP_MAX_UNKNOWN];
	HCHAR	szProfile[APKD_DLNA_DMP_MAX_UNKNOWN];
	HCHAR	szParentId[APKD_DLNA_DMP_MAX_UNKNOWN];

	HUINT8	ucPRNum;
	HUINT16	usNumOfBookmark;
	APKS_DLNA_DMP_PARENTAL_RATING_t		astParentalRating[APKD_DLNA_DMP_MAX_PARENTAL];
	APKS_DLNA_DMP_Bookmark_t			astBookmarks[APKD_DLNA_DMP_MAX_BOOKMARK];

	HCHAR	szDescription[APKD_DLNA_DMP_MAX_DESC];
	HCHAR	szLongDescription[APKD_DLNA_DMP_MAX_LONG_DESC];
} APKS_DLNA_DMP_ContentItem_t;

typedef struct
{
	HINT32 nReqId;
	HINT32 nTotalMatch;
	HINT32 nReturnCnt;  // num of item
	HINT32 nErrCode;  // num of item

	HCHAR   szUDN[APKD_DLNA_DMP_MAX_UDN];
} APKS_DLNA_DMP_ContentResult_t;

typedef struct
{
	HCHAR	szUDN[APKD_DLNA_DMP_MAX_UDN];
} APKS_DLNA_DMP_RefreshDms_t;

typedef void 	(*APKS_DLNA_CbHandler) (HUINT32 eEvent, HUINT32 ulParam1, HUINT32 ulParam2);

extern void		APK_DLNADMP_Init( void );
extern void		APK_DLNADMP_DeInit( void );

extern void		APK_DLNADMP_RegisterNotifier(APKS_DLNA_CbHandler pfnObjectHandler);
extern void		APK_DLNADMP_UnRegisterNotifier(APKS_DLNA_CbHandler pfnObjectHandler);

extern HERROR	APK_DLNADMP_Start( void );
extern HERROR	APK_DLNADMP_Stop( void );
extern HERROR 	APK_DLNADMP_getListOfDMS( HUINT32 *pCount, APKS_DLNA_DMP_ServerDevice_t **pResultData );
extern HERROR	APK_DLNADMP_BrowsingChildItem( const HCHAR *udn, const HCHAR *cid, const HCHAR *sort, HINT32 startIdx, HINT32 nRequest);

extern HERROR  APK_DLNADMP_SearchingItem( const HCHAR *udn, const HCHAR *cid, const HCHAR *category, const HCHAR *keyword, const HCHAR *sort, HINT32 startIdx, HINT32 nRequest, HINT32 reqId );
extern HERROR  APK_DLNADMP_SearchingAll( const HCHAR *cid, const HCHAR *category, const HCHAR *keyword, const HCHAR *sort, HINT32 reqId );

extern HERROR	APK_DLNADMP_GetRecordDestinations( const HCHAR *device );
extern HERROR	APK_DLNADMP_GetRecordDiskInfo( const HCHAR *udn, const HCHAR *id);
extern HERROR	APK_DLNADMP_RemoveItem( const HCHAR *udn, const HCHAR *id, char **retval);
extern HERROR	APK_DLNADMP_GetIsIPDubbing(HBOOL *val);
extern HERROR	APK_DLNADMP_RefreshDms( const HCHAR *pUdn );

/*-----------------------------------------------------------------------------
 *  dmr
 *-----------------------------------------------------------------------------*/

#define APKD_DLNA_DMR_MAX_CID			48
#define APKD_DLNA_DMR_MAX_URI			1024
#define APKD_DLNA_DMR_MAX_DATE			12
#define APKD_DLNA_DMR_MAX_RESOLUTION 	12
#define APKD_DLNA_DMR_MAX_GENRE			32

#define APKD_DLNA_DMR_MAX_NAME			512
#define APKD_DLNA_DMR_MAX_TITLE			128

typedef enum
{
	eDLNA_DMR_MEDIATYPE_Container,
	eDLNA_DMR_MEDIATYPE_Video,
	eDLNA_DMR_MEDIATYPE_Music,
	eDLNA_DMR_MEDIATYPE_Photo,

	eDLNA_DMR_MEDIATYPE_Unknown,
} APKE_DLNA_DMR_MEDIATYPE_e;

typedef	enum
{
	eDLNA_DMR_DoNothing,
	eDLNA_DMR_StartCmd,
	eDLNA_DMR_StopCmd,
	eDLNA_DMR_SetFriendlyNameCmd,
	eDLNA_DMR_MediaChangeCmd,
	eDLNA_DMR_RequestBringIn, /* Request SetMediaChange(uri) to webapp */

	/* UPnP actions */
	eDLNA_DMR_RequestSetUri,
	eDLNA_DMR_RequestPlay,
	eDLNA_DMR_RequestStop,
	eDLNA_DMR_RequestPause,
	eDLNA_DMR_RequestSeek,
	eDLNA_DMR_RequestSetVolume,
	eDLNA_DMR_RequestSetMute,

	eDLNA_DMR_OperationModeCmd,

	eDLNA_DMR_UnknownType,
} APKE_DLNA_DMR_Property_e;

typedef struct
{
	HCHAR	szFriendlyName[ APKD_DLNA_DMR_MAX_NAME ];
	HCHAR	szMediaURI[ APKD_DLNA_DMR_MAX_URI ];
} APKS_DLNA_DMR_DmrName_t;

typedef struct
{
	HINT32						nPlaySpeed;
	HCHAR						szObjectID[ APKD_DLNA_DMR_MAX_NAME ];
	APKE_DLNA_DMR_MEDIATYPE_e	eMediaType;
	HCHAR						szMediaUri[ APKD_DLNA_DMR_MAX_URI ];
	HCHAR						szThumbnailUri[ APKD_DLNA_DMR_MAX_URI ];
	HCHAR						szTitle[ APKD_DLNA_DMR_MAX_TITLE ];
	HCHAR						szDate[ 12 ];
	HINT32						nDuration;
	HCHAR						szResolution[ 12 ];
	HBOOL						bCopyControl;
	HCHAR						szGenre[ 32 ];
	HCHAR						szAlbum[ APKD_DLNA_DMR_MAX_TITLE ];
	HCHAR						szArtist[ APKD_DLNA_DMR_MAX_TITLE ];
} APKS_DLNA_DMR_ReqSetUriInfo_t;

typedef struct
{
	HINT32	nPlaySpeed;
} APKS_DLNA_DMR_ReqPlayInfo_t;

typedef struct
{
	HLONG	nPosition;
} APKS_DLNA_DMR_ReqSeekInfo_t;

typedef struct
{
	HINT32	nVolume;
} APKS_DLNA_DMR_ReqSetVolumeInfo_t;

typedef struct
{
	HBOOL	bMute;
} APKS_DLNA_DMR_ReqSetMuteInfo_t;

extern void		APK_DLNADMR_Init( void );
extern void		APK_DLNADMR_DeInit( void );

extern HERROR	APK_DLNADMR_Start( HCHAR *pFriendlyName );
extern HERROR	APK_DLNADMR_Stop( void );
extern HERROR	APK_DLNADMR_GetDefaultFriendlyName( HCHAR* pFriendlyName, HINT32 nLen );
extern HERROR	APK_DLNADMR_SetFriendlyName( const HCHAR *pFriendlyName );
extern HERROR	APK_DLNADMR_SetBringInUri( const HCHAR *pUrl );
extern HERROR	APK_DLNADMR_SetOperationMode( HINT32 nMode );
extern void		APK_DLNADMR_UnRegisterNotifier( APKS_DLNA_CbHandler pfnObjectHandler );
extern void		APK_DLNADMR_RegisterNotifier( APKS_DLNA_CbHandler pfnObjectHandler );

/*-----------------------------------------------------------------------------
 *  dms
 *-----------------------------------------------------------------------------*/
#define APKD_DLNA_DMS_MAX_URI			1024
#define APKD_DLNA_DMS_MAX_MIME			256

typedef enum
{
	eDLNA_DMS_StartCmd,
	eDLNA_DMS_StopCmd,

	eDLNA_DMS_SetFriendlyName,
	eDLNA_DMS_GetDmsInfo,
	eDLNA_DMS_NotiRunState,

	/* WoonServer Cmd */
	eDLNA_DMS_WoonServer_TestConnection, /* 4 */

	eDLNA_DMS_UnknownType

} APKE_DLNA_DMS_Property_e;

typedef struct
{
	HCHAR	szFriendlyName[128];
	HBOOL	bBootTimeStart;
} APKS_DLNA_DMS_DmsName_t;

typedef struct
{
	HCHAR		szFriendlyName[128];
	HCHAR		szUDN[ 48 ];

	HCHAR		szModelName[ 32 ];
	HCHAR		szModelDescription[ 128 ];
	HCHAR		szModelURL[ 512 ];
	HCHAR		szModelNumber[ 32 ];

	HINT32		upnp_port;
	HINT32		webserver_port;
	HINT32		upnp_dtcp_port;
	HINT32		uploadserver_port;

} APKS_DLNA_DMS_DmsInfo_t;

typedef struct
{
	HCHAR			szRunState[ 32 ];
	HINT32			nErrCode;
} APKS_DLNA_DMS_RunState_t;


/* APKS_DLNA_DMS_LiveStreamInfo_t -> HOMxDMS_LiveStreamInfo_t -> PL_DMS_LiveStreamInfo_t */
typedef struct
{
	HCHAR	szURI[ APKD_DLNA_DMS_MAX_URI ];
	HCHAR	szMIME[ APKD_DLNA_DMS_MAX_MIME ];
} APKS_DLNA_DMS_LiveStreamInfo_t;

extern void		APK_DLNADMS_Init( void );
extern void		APK_DLNADMS_DeInit( void );

extern HERROR	APK_DLNADMS_Start( HCHAR *pFriendlyName, HBOOL bBootTimeStart );
extern HERROR	APK_DLNADMS_Stop( void );
extern HERROR	APK_DLNADMS_GetDefaultFriendlyName( HCHAR* pFriendlyName, HINT32 nLen );
extern HERROR	APK_DLNADMS_SetFriendlyName( const HCHAR *pFriendlyName );
extern void		APK_DLNADMS_RegisterNotifier( APKS_DLNA_CbHandler pfnObjectHandler );
extern void		APK_DLNADMS_UnRegisterNotifier( APKS_DLNA_CbHandler pfnObjectHandler );
extern HERROR 	APK_DLNADMS_GetLiveStreamInfo( HINT32 hSvc, HUINT32 *pCount, APKS_DLNA_DMS_LiveStreamInfo_t **pResultData );
extern HERROR	APK_DLNADMS_FreeLiveStreamInfo(HUINT32 nCount, APKS_DLNA_DMS_LiveStreamInfo_t *pResultData );


/*-----------------------------------------------------------------------------
 *  dial
 *-----------------------------------------------------------------------------*/
typedef enum
{
	eDLNA_DIAL_ReqAppLaunch,
	eDLNA_DIAL_ReqAppStop,
	eDLNA_DIAL_ReqAppStatus,

	eDLNA_DIAL_Started,
	eDLNA_DIAL_Stopped,

	eDLNA_DIAL_CmdEnd,
} APKE_DIAL_Property_e;

extern void		APK_DIAL_Init( void );
extern void		APK_DIAL_DeInit( void );
extern void		APK_DIAL_RegisterNotifier( APKS_DLNA_CbHandler pfnObjectHandler );
extern void		APK_DIAL_UnRegisterNotifier( APKS_DLNA_CbHandler pfnObjectHandler );
extern HERROR	APK_DIAL_Start( HCHAR *pFriendlyName );
extern HERROR	APK_DIAL_Stop( void );
extern HERROR	APK_DIAL_SetFriendlyName(const HCHAR *pFriendlyName );
extern HERROR	APK_DIAL_AppRegist(const HCHAR *app);
extern HERROR	APK_DIAL_AppUnregist(const HCHAR *app);
extern HERROR	APK_DIAL_SetAppState(const HCHAR *app, HINT32 state);
extern HERROR	APK_DIAL_SetCookie(const HCHAR * cookie);
extern HERROR	APK_DIAL_GetDialStatus(HBOOL * pDialActivated);
extern HERROR	APK_DIAL_GetFriendlyName(HCHAR * pFriendlyName, HUINT32 size);
extern HERROR	APK_DIAL_GetAdditionalDataUrl(HCHAR * pAdditionalDataUrl, HUINT32 size, HCHAR * pAppName);

/*-----------------------------------------------------------------------------
 *  woon server
 *-----------------------------------------------------------------------------*/
typedef enum
{
	eWOONSERVER_StartCmd,
	eWOONSERVER_StopCmd,
	eWOONSERVER_SetFriendlyName,
	eWOONSERVER_TestConnection,
	eWOONSERVER_NotifyPinCode,
	eWOONSERVER_UnknownType,
} APKE_WOONSERVER_Property_e;

typedef struct
{
	HCHAR	szFriendlyName[128];
} APKS_WOONSERVER_Name_t;

typedef struct
{
	HINT32	isStarted;
} APKS_WOONSERVER_Started_t;

typedef struct
{
	HINT32	isStopped;
} APKS_WOONSERVER_Stopped_t;

typedef struct
{
	HINT32	isRenamed;
} APKS_WOONSERVER_Renamed_t;

typedef struct
{
	HINT32	bOpen;
	HCHAR	szPinCode[5];
} APKS_WOONSERVER_PinCode_t;

typedef struct
{
	HINT32	nState;
	HCHAR 	szLocalIp[128];
	HCHAR 	szExtIp[128];
	HCHAR 	szPubIp[128];
	HINT32 	nPubPort;
	HINT32 	nExtPort;
} APKS_WOONSERVER_Info_t;

extern void		APKI_WOONSERVER_Init( void );
extern void		APKI_WOONSERVER_DeInit( void );

extern HERROR	APK_WOONSERVER_Start(HCHAR *pFriendlyName);
extern HERROR	APK_WOONSERVER_Stop(void);
extern HERROR	APK_WOONSERVER_GetDefaultFriendlyName( HCHAR* pFriendlyName, HINT32 nLen );
extern HERROR	APK_WOONSERVER_SetFriendlyName( const HCHAR *pFriendlyName );
extern HERROR 	APK_WOONSERVER_TestConnection(void);

extern void		APK_WOONSERVER_RegisterNotifier( APKS_DLNA_CbHandler pfnObjectHandler );
extern void		APK_WOONSERVER_UnRegisterNotifier( APKS_DLNA_CbHandler pfnObjectHandler );

/*-----------------------------------------------------------------------------
 *  SAT>IP Server
 *-----------------------------------------------------------------------------*/
typedef enum
{
	eSATIP_StartCmd,
	eSATIP_StopCmd,
	eSATIP_GetStatus,

	eSATIP_Started,
	eSATIP_Stopped,

	eSATIP_UnknownType
} APKE_SATIP_Property_e;

extern void		APK_SATIP_Init( void );
extern void		APK_SATIP_DeInit( void );

extern HERROR	APK_SATIP_Start( HCHAR *pFriendlyName);
extern HERROR	APK_SATIP_Stop( void );

extern HERROR	APK_SATIP_GetDefaultFriendlyName( HCHAR* pFriendlyName, HINT32 nLen );
extern HERROR	APK_SATIP_SetFriendlyName( const HCHAR *pFriendlyName );

extern void		APK_SATIP_RegisterNotifier( APKS_DLNA_CbHandler pfnObjectHandler );
extern void		APK_SATIP_UnRegisterNotifier( APKS_DLNA_CbHandler pfnObjectHandler );

/*-----------------------------------------------------------------------------
 *  rc
 *-----------------------------------------------------------------------------*/
typedef enum
{
	eDLNA_RC_Started,
	eDLNA_RC_Stopped,
	eDLNA_RC_Paired,
	eDLNA_RC_Unpaired,
	eDLNA_RC_Message,
	eDLNA_RC_ReqAppLaunch,
	eDLNA_RC_SetFriendlyName,
	eDLNA_RC_CmdEnd,
} APKE_RC_Property_e;

extern void		APK_RC_Init( void );
extern void		APK_RC_DeInit( void );
extern void		APK_RC_RegisterNotifier( APKS_DLNA_CbHandler pfnObjectHandler );
extern void		APK_RC_UnRegisterNotifier( APKS_DLNA_CbHandler pfnObjectHandler );
extern HERROR	APK_RC_Start( HCHAR *pFriendlyName );
extern HERROR	APK_RC_Stop( void );
extern HERROR	APK_RC_SetFriendlyName( HCHAR *pFriendlyName );
extern HERROR	APK_RC_UnpairClient( void );


/*-----------------------------------------------------------------------------
 *  ruis
 *-----------------------------------------------------------------------------*/
typedef enum
{
	eRUIS_ERROR_500 = 0,
	eRUIS_ERROR_Unknown,
	eRUIS_ERROR_Max,

} APKE_RUIS_ErrorCode_e;

extern void		APK_DLNARUIS_RegisterNotifier( APKS_DLNA_CbHandler pfnObjectHandler );
extern void		APK_DLNARUIS_UnRegisterNotifier( APKS_DLNA_CbHandler pfnObjectHandler );
extern HERROR	APK_DLNARUIS_SendMessage( HINT32 nDevHandle, HINT32 nReqHandle, HCHAR *pszHeader, HCHAR *szMessage  );
extern HERROR	APK_DLNARUIS_SendMulticast( HINT32 nDevHandle, HINT32 nEvtLevel, HCHAR *pszNotifCEHTML, HCHAR *pszFriendlyName, HCHAR * pszProfileList  );
extern HERROR	APK_DLNARUIS_SendErrorCode( HINT32 nDevHandle, HINT32 nReqHandle, APKE_RUIS_ErrorCode_e eError  );

/*-----------------------------------------------------------------------------
 *  homma <-> media server(in the obama)
 *-----------------------------------------------------------------------------*/

typedef enum
{
	eDLNA_LIVESTRM_SOURCE_Live,
	eDLNA_LIVESTRM_SOURCE_RecordFile,
	eDLNA_LIVESTRM_SOURCE_MediaFile,

	eDLNA_LIVESTRM_SOURCE_EndOfCase
} APKE_DLNA_LIVESTREAM_SourceType_e;

typedef enum
{
	eDLNA_LIVESTRM_OUTPUT_RingBuf,
	eDLNA_LIVESTRM_OUTPUT_File,

	eDLNA_LIVESTRM_OUTPUT_EndOfCase
} APKE_DLNA_LIVESTREAM_OutputType_e;

typedef struct
{
	HBOOL   bTranscode;
	HUINT32	svc_handle;
	HINT32  profile_id;
	HBOOL   bPcrTsPacketInsert;

	APKE_DLNA_LIVESTREAM_SourceType_e source;
	APKE_DLNA_LIVESTREAM_OutputType_e output;
} APKS_DLNA_LIVESTREAM_SETUP_t;

typedef struct
{
	HBOOL	is_transcode;
	HUINT32	rating;
} APKS_DLNA_LIVESTREAM_STATUS_t;

extern void		APK_DLNA_OBAMA_Init( void );
extern void		APK_DLNA_OBAMA_DeInit( void );

extern void		APK_DLNA_OBAMA_RegisterNotifier( APKS_DLNA_CbHandler pfnObjectHandler );
extern void		APK_DLNA_OBAMA_UnRegisterNotifier( APKS_DLNA_CbHandler pfnObjectHandler );
extern HERROR	APK_DLNA_OBAMA_LiveStream_Start( APKS_DLNA_LIVESTREAM_SETUP_t *setup );
extern HERROR	APK_DLNA_OBAMA_LiveStream_Stop( HINT32 handle );
extern HERROR	APK_DLNA_OBAMA_LiveStream_ItemindexByProfile( HCHAR *profilename, HINT32 *index );
extern HERROR	APK_DLNA_OBAMA_LiveStream_Status( HINT32 handle, HINT32 type, APKS_DLNA_LIVESTREAM_STATUS_t *status );
#endif
