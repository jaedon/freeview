/**************************************************************
 *	@file		ipepg.c
 *	IP EPG downloader for OCTO 2.0
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/08/01
 *	@author			humax
 *	@brief			IP EPG Downloader for OCTO 2.0 Project
 **************************************************************/
#ifndef _IPEPG_H_
#define _IPEPG_H_
#include	<hlib.h>
#include   <dlib.h>
#include	<apk.h>

#define IPEPG_USE_COMPRESS

#include "ipepg_config.h"
#define XML_TAG_MAX_LEN		          			(512)
#define IPEPG_URL_MAX_LEN						(512)
#define IPEPG_MACADDRESS_MAX_LEN				(20)			// XX:XX:XX:XX:XX \0
#define IPEPG_MAX_PPVER_LEN					36

typedef enum
{
	eIPEPG_MSG_STOP = 0,

	// Start
	eIPEPG_MSG_START_ALL,				 // Restart, SVC Upload, SVC&EPG download, RP
	eIPEPG_MSG_START_ONLY_SVCLIST,	 	 // SVC Upload
	eIPEPG_MSG_START_DOWNLOAD,			 // EPG download, RP
	eIPEPG_MSG_START_UPDATE_SERVICELIST, // SVC Upload, SVC&EPG download, RP
	eIPEPG_MSG_START_RSV_DOWNLOAD,		 // RP
	eIPEPG_MSG_START_CHLOGO_DOWNLOAD,		// Channel Logo Download Start

	// Done
	eIPEPG_MSG_DONE_DOWNLOAD,			 // SVC&EPG download done
	eIPEPG_MSG_DONE_UPDATE_SERVICELIST,	 // SVC upload done
	eIPEPG_MSG_DONE_STB_UPLOAD,			 // RP done

	// ETC
	eIPEPG_MSG_REQUEST_SHUTDOWN_NOTI,
	eIPEPG_MSG_PF_MERGE,

	// FVC
	eIPEPG_MSG_START_DOWNLOAD_FVC_SCHEDULE,	// FVC EPG download
	eIPEPG_MSG_START_DOWNLOAD_FVC_SERIES_SCHEDULE,	 	 // EPG download for specific service & time
	eIPEPG_MSG_DONE_DOWNLOAD_FVC_SCHEDULE,
	eIPEPG_MSG_DONE_DOWNLOAD_FVC_SERIES_SCHEDULE,
	eIPEPG_MSG_DONE_WAITING_RETRY,

	eIPEPG_MSG_MAX
} IPEPG_MSG_e;

typedef enum
{
	eIPEPG_STOPREASON_NOTHING = 0,
	eIPEPG_STOPREASON_GETALL,
 	eIPEPG_STOPREASON_NETWORK,
	eIPEPG_STOPREASON_FACTORYRESET,
	eIPEPG_STOPREASON_FORMAT,
	eIPEPG_STOPREASON_ONOFF,
	eIPEPG_STOPREASON_REBOOT,
	eIPEPG_STOPREASON_HDD_OFF,
	eIPEPG_STOPREASON_MAX
} IPEPG_STOP_REASON_e;


typedef enum
{
	eIPEPG_NONE			= 0x00000000,	// Non Processing
	eIPEPG_DOWN_START 		= 0x00000001,	// processing Epg Download
	eIPEPG_UPLOAD_START 	= 0x00000002,	// processing servicelist Upload
	eIPEPG_CHLOGO_DOWN_START	= 0x00000004,	// processing Channel Logo Download
	eIPEPG_STOP 			= 0x00000010,	// Stop Command
	eIPEPG_FVC_START		= 0x00000020,	// processing FVC EPG download
	eIPEPG_FVC_SERIES_EPG_START = 0x00000040,	// processing FVC Series EPG download
	eIPEPG_RP_START			= 0x00000080,
	eIPEPG_DOWN_MAX
} IPEPG_DOWNLOAD_e;

typedef	enum
{
	eIPEPG_ITEM_SERVERURL = 0,
	eIPEPG_ITEM_SYS_ID,
	eIPEPG_ITEM_SYS_VER,
	eIPEPG_ITEM_MAC,
	eIPEPG_ITEM_SVCLIST,
	eIPEPG_ITEM_NETWORK,
	eIPEPG_ITEM_UPDATE_TIME,
	eIPEPG_ITEM_HDD_STATUS,
	eIPEPG_ITEM_FLAG_SVC_UPDATED,
	eIPEPG_ITEM_STAMP_TIME,
	eIPEPG_ITEM_LAST_UPDATE_TIME,
	eIPEPG_ITEM_DOWNLOADPROGRESS,
	eIPEPG_ITEM_FLAG_IPEPG_ONOFF,
	eIPEPG_ITEM_SCHEDULE_LOADED,
	eIPEPG_ITEM_FLAG_RP_ONOFF,
	eIPEPG_ITEM_RP_UPDATE_INTERVAL,
	eIPEPG_ITEM_RP_PERIOD_START,
	eIPEPG_ITEM_RP_PERIOD_END,
	eIPEPG_ITEM_RP_UPDATE_TIME,
	eIPEPG_ITEM_SVC_COUNT,
	eIPEPG_ITEM_EVT_COUNT,
	eIPEPG_ITEM_PRIVACY_POLICY_VER,
/* Freeview connected */
	eIPEPG_ITEM_FVC_BASE_URL,
	eIPEPG_ITEM_FVC_AUTH_HASH,
	eIPEPG_ITEM_FVC_AUTH_TIME_STAMP,
	eIPEPG_ITEM_FVC_SERIES_EVENT_LIST,

	eIPEPG_ITEM_MAX
} IPEPG_Item_e;

typedef	enum
{
	eIPEPG_DATATYPE_BOOL,
	eIPEPG_DATATYPE_VALUE,
	eIPEPG_DATATYPE_STRING,
	eIPEPG_DATATYPE_HASH,
	eIPEPG_DATATYPE_LIST,
} IPEPG_DATATYPE_e;

typedef struct
{
	IPEPG_Item_e		eItem;
	IPEPG_DATATYPE_e 	eItemType;
	HUINT32 			itemValue;
	HUINT8 				*pucData;
	HxList_t			*list;
	HxHASH_t		*hash;
} IPEPG_ItemInfo_t;


typedef struct
{
	HUINT32				ulWriteSema;
 	IPEPG_ItemInfo_t	pIpEPG[eIPEPG_ITEM_MAX];
 } IPEPG_DATAMGR_t;

typedef struct
{
	HUINT16	 			usOnId;
	HUINT16	 			usTsId;
	HUINT16	 			usSvcId;
} IPEPG_TripleId_t;


typedef struct
{
	IPEPG_TripleId_t	stTripleId;
	HINT32				ulUid;
	HUINT32				ulLcn;
	HUINT8				ucName[DxNAME_LEN];

}IPEPG_SvcInfo_t;

typedef struct
{
	HUINT32				uid;
	HUINT16				usSvcId;
	HUINT32				ulStartTime;
	HUINT32				ulHttpStatusCode;	// status code from fvc
}IPEPG_FvcSeriesEvent_t;

typedef	enum
{
	eIPEPG_EVENT_TYPE_UPDATE,
	eIPEPG_EVENT_TYPE_DELETE,
	eIPEPG_EVENT_TYPE_MAX
} IPEPG_EVENT_TYPE_e;

typedef struct
{
	DxIpEvent_t *		pIpEvent;
	IPEPG_EVENT_TYPE_e	eUpdateType;
}	IPEPG_UpdateEvent_t;

typedef struct tagEpgUrl
{
	HUINT8 				*pucHeader;		// FTP:// HTTPS://
	HUINT8 				*pucHost;
	HUINT8 				*pucPort;
	HUINT8 				*pucPath;
	HxList_t			*pQuery;
}EPG_URL_PARSER_t;

typedef	enum
{
	eIPEPG_URL_HEAD,
	eIPEPG_URL_HOST,
	eIPEPG_URL_PORT,
	eIPEPG_URL_PATH,
	eIPEPG_URL_QUERY
} IPEPG_URL_STEP_e;


typedef struct tagEpgUrlQuery
{
	HUINT8 				*pucKey;		// FTP:// HTTPS://
	HUINT8 				*pucValue;
}EPG_URL_Query_t;




typedef struct tagEpgUrlInfo
{
	IPEPG_TripleId_t	stTripleId;
	HUINT32				ulSvcUid;
	HUINT8 				pucUrl[IPEPG_URL_MAX_LEN];
	HUINT8 				pucThumbnailUrl[IPEPG_URL_MAX_LEN];
	HUINT32				ulEventCount;
	HBOOL				bSupportMedia;
}EPG_URL_INFO_t;

typedef struct
{
	HUINT32				ulMainTaskId;
	HUINT32				ulMainMsgQid;
	HUINT32				ulUploadSvcTaskId;		// For Uploading Svclist DATA
	HUINT32				ulUploadSvcMsgQId;
	HUINT32				ulEpgDownloadTaskId;	// For Downloading EPG DATA
	HUINT32				ulEpgDownloadTaskQId;
	HUINT32				ulRsvTaskId;			// For Checking RP DATA
	HUINT32				ulRsvMsgQid;
	HUINT32				ulTimerTaskId;			// For Starting IP EPG
	HUINT32				ulChlogoTaskId;			// For Download Channel Logo
	HUINT32				ulChlogoMsgQId;
	IPEPG_DOWNLOAD_e	eStatus;
	IPEPG_STOP_REASON_e	eStopReason;
	IPEPG_DATAMGR_t	 	*pstData;

#if defined(CONFIG_OP_UK_DTT)
	IPEPG_FVC_STATE_e	eCurState;
	IPEPG_FVC_LEFT_JOB_e	eLeftJobs;

	HUINT32				ulCurrentRetry;			// D-Book 7.0 part b 2.4.6.3
	HBOOL				bIsWaitingNow;			// True if waiting for currentRetry
#endif
} IPEPG_TASK_MGR_t;


typedef struct
{
	HUINT32				ulType;
	HUINT32				ulMsg;
	HUINT32				ulParam1;
	HUINT32				ulParam2;
	HUINT32				ulParam3;
} IpEpgMsg_t;


typedef	enum
{
	eIPEPG_SVC_NO_CHANGED,
	eIPEPG_SVC_UID_CHANGED,
	eIPEPG_SVC_UDATA_CHANGED
} IPEPG_COMP_RESULT_e;


void 			IPEPG_GetServicelistFromDama(void * param1, void * param2, HUINT32 size);
void			IPEPG_SetItem(IPEPG_Item_e eItem, void *pData);
void			*IPEPG_GetItemPoint(IPEPG_Item_e eItem);
HUINT32 		IPEPG_GetItemValue(IPEPG_Item_e eItem);
HERROR			IPEPG_AddItemToPoint (IPEPG_Item_e eItem, void *userItem);
HERROR			IPEPG_DeleteItemFromPoint (IPEPG_Item_e eItem, void *userItem);

void			IPEPG_SetAPKDBItem(IPEPG_Item_e eItem, void *pData);


void inline 	IPEPG_MemFree(void *mem);

#define 		IPEPG_MemAlloc(size)	HLIB_STD_MemAlloc(size)
#define 		IPEPG_MemCalloc(size)		HLIB_STD_MemCalloc(size)
#define			IPEPG_StrDup(str)	HLIB_STD_StrDup(str)
#define			IPEPG_MemDup(data, size)	HLIB_STD_MemDup(data, size)
#define			IPEPG_MemAlloc_CB(size)		HLIB_MEM_Malloc_CB(size)
#define			IPEPG_MemFree_CB(size)		HLIB_MEM_Free_CB(size)
#define			IPEPG_StrCmp(str1, str2)	HLIB_STD_StrCaseCmp(str1, str2)

IPEPG_SvcInfo_t *IPEPG_GetSvc(HINT32 uid);
void			IPEPG_SetAPKDBItem(IPEPG_Item_e eItem, void *pData);


#if defined(CONFIG_DEBUG) || defined(CFG_OPEN_DEBUG)

typedef enum
{
	eIPEPG_DNLD_DBG_ENDED = 0,
	eIPEPG_DNLD_DBG_STARTED,
	eIPEPG_DNLD_DBG_SERVICE_COUNT,
	eIPEPG_DNLD_DBG_CHLOGSENDTODAMA_COUNT,
	eIPEPG_DNLD_DBG_DOWNLOAD_COUNT,
	eIPEPG_DNLD_DBG_DOWNLOADEVENT_COUNT,
	eIPEPG_DNLD_DBG_DOWNLOADEVENT_TIME,
	eIPEPG_DNLD_DBG_PARTOFTIME,
	eIPEPG_DNLD_DBG_TOTALTIME,
	eIPEPG_DNLD_DBG_STARTTIME,
	eIPEPG_DNLD_DBG_ENDTIME,
	eIPEPG_DNLD_DBG_MAX
}IPEPG_DOWNLOAD_DBG_e;



HUINT32 IPEPG_GetDownloadDebugInfo(IPEPG_DOWNLOAD_DBG_e e_paramType);
void IPEPG_SetDownloadDebugInfo(IPEPG_DOWNLOAD_DBG_e e_paramType, void *pArg);

#if defined (IPEPG_DEBUG_DOWNLOAD_XML)
void IPEPG_DEBUG_SaveXML_Init(HBOOL bForwardEPG, HUINT32 ulSystemTime, HBOOL bDbSnapshot);
void IPEPG_DEBUG_SaveXML(const DxIpService_t *pIpSvcInfo, const HUINT8 * pUrl, void * pdata, HINT32 lSizeData);
void IPEPG_DEBUG_SaveDeleteEventXML(const DxIpService_t *pIpSvcInfo, const HUINT8 * pUrl, void * pdata, HINT32 lSizeData);
void IPEPG_DEBUG_SaveModifyEventXML(const DxIpService_t *pIpSvcInfo, const HUINT8 * pUrl, void * pdata, HINT32 lSizeData);
#endif

// 1 : ipepg download fail situation, 0: ipepg download success situation
HINT32 IPEPG_GetNetworkTestValue(void);


#define	IPEPG_RETURN_IF_VOID(expr)	do{if(expr){HxLOG_Debug(HxANSI_COLOR_RED(" %s failed!!\n"), #expr);return;}}while(0)
#define	IPEPG_RETURN_IF(expr, err)	do{if(expr){HxLOG_Debug(HxANSI_COLOR_RED(" %s failed!!\n"), #expr);return (err);}}while(0)
#define	IPEPG_GOTO_IF(expr, pos)	do{if(expr){HxLOG_Debug(HxANSI_COLOR_RED(" %s failed!!\n"), #expr);goto pos;}}while(0)
#define	IPEPG_GOTO_IF_NO_PRINT(expr, pos)	do{if(expr){goto pos;}}while(0)

#else
#define	IPEPG_RETURN_IF_VOID(expr)	do{if(expr){return;}}while(0)
#define	IPEPG_RETURN_IF(expr, err)	do{if(expr){return (err);}}while(0)
#define	IPEPG_GOTO_IF(expr, pos)	do{if(expr){goto pos;}}while(0)
#define	IPEPG_GOTO_IF_NO_PRINT(expr, pos)	do{if(expr){goto pos;}}while(0)



#endif

#endif

