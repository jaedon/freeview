/**
	@file     file_name.c File Name을 적습니다.
	@brief    file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
/** @brief global header file  */
#include <htype.h>
#include <vkernel.h>
#include <octo_common.h>
#include <isosvc.h>
#include <util.h>
#include <db_svc.h>
#include <pal_pipe.h>
#include <pal_sef.h>
#include <pal_sys.h>
#include <sys/stat.h>

/** @brief local header file  */
#include <svc_download.h>
#include <_svc_download.h>
#include "../local_include/_xsvc_download.h"

//#define WORKAROUND_LIGADA_MHEG_HBBTV_COEXISTENCE // 일부 Pid 의 DDB가 AIT 와 PMT의  변경 주기보다 늦게 오는 경우가 발생하여 Timeout 값을 조절함.

#if !defined(WIN32) && defined(CONFIG_3RD_OBJ_CAROUSEL)
#define ALLOW_TO_COMPILE_THIS_MODULE
#endif	// !WIN32 && CONFIG_3RD_OBJ_CAROUSEL

#if defined(ALLOW_TO_COMPILE_THIS_MODULE)
/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#ifdef CONFIG_DEBUG
//#define DSI_DUMP
//#define DDB_DUMP
//#define DESC_DUMP
//#define TRACE_FILTER
#endif

#define SECTION_HEADER_LEN					(8)
#define DDB_HEADER_LEN						(12)
#define SECTION_MESSAGEID_POS				(10)
#define MAX_OBJECTCAROUSEL_FILTER			(3)	/*refer /plugin/3rdparty_port/browser/port/hbbtv/browser_dsmcc.c */
#define MAX_PIDFILTER_OF_EACH_OC			(3) /*refer /plugin/3rdparty_port/browser/port/hbbtv/browser_dsmcc.c */
#define MAX_FILTERING_NUM					(MAX_OBJECTCAROUSEL_FILTER * MAX_PIDFILTER_OF_EACH_OC)
#define INVALID_INFO_INDEX					(99)
#define MAX_CACHE_NUM						(12)
#define ONE_BYTE_FILTER						0x000000FF
#define BASE_BIT_POSITION					(24)
#define ONE_BYTE_POSITION					(8)
#define CAROUSEL_TIMEOUT_MIN				(1000)			// ms
#define CAROUSEL_TIMEOUT_MAX				(180 * 1000)	// ms
#define CAROUSEL_TIMEOUT_DEFAULT			(60 * 1000)		// ms

#define DDB_INFO_HASHSIZE					32				// 반드시 2의 급수여야 한다
#define DDB_INFO_HASHINDEX(module_id)		(module_id & (DDB_INFO_HASHSIZE - 1))

#define	DSMCC_DEFAULT_CACHE_PATH			"/tmp/cache/"

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
#define __USE_LIBDSMCC__ /* by Dylan - libdsmcc */

#ifdef __USE_LIBDSMCC__ /* by Dylan - libdsmcc */
#include "libdsmcc.h"
#endif

typedef enum
{
	DSMCC_STREAMTYPE_MULTIPROTOCOL_ENCAPSULATION = 0x0A,
	DSMCC_STREAMTYPE_UN_MESSAGES,
	DSMCC_STREAMTYPE_STREAM_DESCRIPTION,
	DSMCC_STREAMTYPE_SECTION,
}XsvcDownload_DsmccStreamType_e;

typedef enum
{
	eFILTER_MODE_NONE,

	eFILTER_MODE_RECEIVING,
	eFILTER_MODE_MONITORING,
	eFILTER_MODE_CHANGED
} XsvcDownload_FilteringMode_e;

typedef struct
{
	HUINT32							 ulPid;
	HUINT32							 ulDownloadId;
	HUINT32							 ulCrc32;
	HUINT32							 ulModuleNum;
	HUINT32							 ulTotalModuleSize;
} XsvcDownload_DiiInfo_t;

typedef struct
{
	HUINT32							 ulReceivedTick;
	HUINT32							 ulLastSentTick;

	HUINT32							 ulCarouselId;
	HUINT32							 ulModuleId;

	HBOOL							 bSentToMgr;

	HUINT32							 ulObjInfoFromEvt;
	HUINT8							*pucObjInfoFromEvt;

	HUINT32							 ulObjInfoFromDesc;
	HUINT8							*pucObjInfoFromDesc;

	DxDSMCC_StreamEvent_t			 stEvent;
} XsvcDownload_HbbtvEvtInfo_t;

typedef struct
{
//	HUINT16					usCacheIndex;			/* 필요 없어 보여 일단 막는다. */
	HUINT16							 usPid;
	HUINT16							 usDsiFilterId, usDdbFilterId, usDescFilterId;
	XsvcDownload_DsmccStreamType_e	 eStreamType;
	XsvcDownload_FilteringMode_e	 eDsiMode;
	XsvcDownload_FilteringMode_e	 eDiiMode;
	HUINT32							 ulDsiCrc32, ulDiiCrc32, ulDescCrc32, uiCarouselId;
	HBOOL							 bDsiParsed, bDiiParsed;
	HBOOL							 bDdbZeroBlockParsed;
	HBOOL							 bDdbAllReceived;
#ifdef __USE_LIBDSMCC__ /* by Dylan - libdsmcc */
	struct dsmcc_status				*pstDsmccStatus;
	/* ssyoo : dsmcc lib에서 사용하는 자원에 대해서 동시에 접근되지 않도록 관리해야함 */
	//unsigned long					 uiSemaForResource;
#endif

	HUINT16							 usAssocTag;
	HUINT32							 ulDsiCarouselId;
	HUINT32							 ulDownloadId;
	HUINT32							 ulLastDiiTick;
	unsigned long					 ulFinishTimerId;
	HUINT32							 ulTimeoutValue;		// ms
	HINT32							 nDdbSecVer;

	unsigned char                    ucLastDescSectionVersion;

	HxList_t						*pstDiiList;			// XsvcDownload_DiiInfo_t
	HxList_t						*apstDdbLists[DDB_INFO_HASHSIZE];
	HxList_t						*pstEventList;
	HxList_t						*pstEventDescList;

	unsigned long					 ulDsiTimerId;
	unsigned long					 ulDiiTimerId;
} XsvcDownload_DsmccDnInfo_t;

typedef struct
{
	XsvcDownload_DsmccDnInfo_t		*pstDnInfo;

	// !!!!!!!
	// For the HBBTV event monitoring!
	HUINT32 			 ulPrevEvtSize;
	void				*pvPrevEvt;

	HUINT32 			 ulPrevEvtDscSize;
	void				*pvPrevEvtDsc;
} XsvcDownload_DsmccContainer_t;

typedef struct
{
	HUINT16				usPid;
	HCHAR				szStoragePath[FILE_PATH_NAME_LENGTH];
} XsvcDownload_DsmccPidBuffer_t;



/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HUINT8 						 s_ucaChannel[256];
STATIC SvcDownload_DsmccContents_t			*s_pstContents		= NULL;		/* callback notification을 받으면 DnInfo 확인을 위해 사용. */
STATIC XsvcDownload_DsmccPidBuffer_t 			 s_stPidBuffer[MAX_FILTERING_NUM];
/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

STATIC void			xsvc_download_FreeDsmccEvent_Hbbtv (void *pvItem);
STATIC void			xsvc_download_FreeDsmccEventInsideOnly_Hbbtv (void *pvItem);
STATIC void			xsvc_download_FreeDsmccContainer_Hbbtv (SvcDownload_DsmccContents_t *pstContents);
STATIC HERROR		xsvc_download_AppendEvent_Hbbtv (SvcDownload_DsmccContents_t *pstContents, DxDSMCC_StreamEvent_t *pstEvent);
STATIC HERROR		xsvc_download_CopyEventItem_Hbbtv (DxDSMCC_StreamEvent_t *pstDstEvent, DxDSMCC_StreamEvent_t *pstSrcEvent);

STATIC XsvcDownload_DsmccDnInfo_t*	xsvc_download_AllocDownloadInfo_Hbbtv (void);
STATIC void				xsvc_download_FreeDownloadInfo_Hbbtv (XsvcDownload_DsmccDnInfo_t *pstInfo);
STATIC XsvcDownload_DsmccDnInfo_t*	xsvc_download_GetDownloadInfo_Hbbtv (SvcDownload_DsmccContents_t *pstContents);

STATIC HERROR 		xsvc_download_StartDsiDiiFiltering_Hbbtv (SvcDownload_DsmccContents_t *pstContents, HUINT16 usPid);
STATIC HERROR 		xsvc_download_StartDdbFiltering_Hbbtv (SvcDownload_DsmccContents_t *pstContents, HUINT16 usPid);
STATIC HERROR 		xsvc_download_StartDescFiltering_Hbbtv (SvcDownload_DsmccContents_t *pstContents, HUINT16 usPid);

STATIC HERROR 		xsvc_download_StopDsiDiiFiltering_Hbbtv (SvcDownload_DsmccContents_t *pstContents, HUINT32 ulIdx, SvcDownload_Msg_t *pstMsg );
STATIC HERROR 		xsvc_download_StopDdbFiltering_Hbbtv (SvcDownload_DsmccContents_t *pstContents,HUINT32 ulIdx, SvcDownload_Msg_t *pstMsg );
STATIC HERROR 		xsvc_download_StopDescFiltering_Hbbtv (SvcDownload_DsmccContents_t *pstContents,HUINT32 ulIdx, SvcDownload_Msg_t *pstMsg );

STATIC HERROR 		xsvc_download_StopFiltering_Hbbtv (SvcDownload_DsmccContents_t *pstContents, SvcDownload_Msg_t *pstMsg);
STATIC HERROR 		xsvc_download_ClearLibResource_Hbbtv ( SvcDownload_DsmccContents_t *pstContents, unsigned int uiIndex );
STATIC HERROR 		xsvc_download_ClearDnInfo_Hbbtv (SvcDownload_DsmccContents_t *pstContents, HUINT32 ulIdx, SvcDownload_Msg_t *pstMsg );

STATIC void 		xsvc_download_CallbackDsiDii_Hbbtv (HUINT32 ulTableIdAndResult, HUINT32 ulfilterAndUniqId, HUINT32 ulParam, Handle_t hAction);
STATIC void 		xsvc_download_CallbackDdb_Hbbtv (HUINT32 ulTableIdAndResult, HUINT32 ulfilterAndUniqId, HUINT32 ulParam, Handle_t hAction);
STATIC void 		xsvc_download_CallbackDesc_Hbbtv (HUINT32 ulTableIdAndResult, HUINT32 ulfilterAndUniqId, HUINT32 ulParam, Handle_t hAction);
STATIC void 		xsvc_download_ParseDsi_Hbbtv (HUINT8 *pucRaw, Handle_t hAction, HUINT32 ulFilterId);
STATIC void 		xsvc_download_ParseDii_Hbbtv (HUINT8 *pucRaw, Handle_t hAction, HUINT32 ulFilterId);
STATIC void 		xsvc_download_ParseDdb_Hbbtv (HUINT8 *pucRaw, Handle_t hAction, HUINT32 ulFilterId);
STATIC void 		xsvc_download_ParseDesc_Hbbtv (HUINT8 *pucRaw, Handle_t hAction, HUINT32 ulFilterId);

STATIC HERROR		xsvc_download_InformHbbtvEventFound (SvcDownload_DsmccContents_t *pstContents, XsvcDownload_DsmccDnInfo_t *pstDnInfo, XsvcDownload_HbbtvEvtInfo_t *pstEvtInfo, HBOOL bForced);
STATIC HERROR		xsvc_download_InformHbbtvEventDescriptionFound (SvcDownload_DsmccContents_t *pstContents, XsvcDownload_DsmccDnInfo_t *pstDnInfo, XsvcDownload_HbbtvEvtInfo_t *pstEvtInfo);
#ifdef __USE_LIBDSMCC__ /* by Dylan - libdsmcc */
STATIC int xsvc_download_WriteFileCallback_Hbbtv (char *szFileName, char *szFilePath, char *szChanName, int nCarouselId, int nPid, int nModuleId, int nFileSize, unsigned char *pucFileBuffer);
STATIC int xsvc_download_WriteDirCallback_Hbbtv (char *szDirName, char *szDirPath, char *szChanName, int nCarouselId, int nPid, int nModuleId);
STATIC int xsvc_download_WriteEventCallback_Hbbtv (char *szEvtFileName, char *szEvtPath, libdsmcc_event_t *pstEvent, char *szChanName, int nCarouselId, int nPid, int nModuleId);
STATIC int xsvc_download_WriteEventDescriptionCallback_Hbbtv( HUINT16	usEventID, HUINT32 uiEventNPT_MSB, HUINT32 uiEventNPT_LSB,
														   HUINT8 *pucPrivateData, HUINT32 uiPrivateLength, HUINT16 usPid );
STATIC void xsvc_download_CompleteCallback_Hbbtv(HINT32 nPid, ObjectCarousel_t	*car);
#endif

#define _____LOCAL_FUNCTIONS_____

#ifdef __USE_LIBDSMCC__


#endif

STATIC void xsvc_download_FreeDsmccEvent_Hbbtv (void *pvItem)
{
	DxDSMCC_StreamEvent_t	*pstDsmccEvt;

	pstDsmccEvt = (DxDSMCC_StreamEvent_t *)pvItem;
	if (pstDsmccEvt != NULL)
	{
		if (pstDsmccEvt->pszUrl != NULL)
		{
			HLIB_STD_MemFree (pstDsmccEvt->pszUrl);
			pstDsmccEvt->pszUrl = NULL;
		}

		if (pstDsmccEvt->szDescription != NULL)
		{
			HLIB_STD_MemFree (pstDsmccEvt->szDescription);
			pstDsmccEvt->szDescription = NULL;
			pstDsmccEvt->ulDescLen = 0;
		}

		if (pstDsmccEvt->szEventName != NULL)
		{
			HLIB_STD_MemFree (pstDsmccEvt->szEventName);
			pstDsmccEvt->szEventName = NULL;
		}

		if (pstDsmccEvt->pucObjInfoByte != NULL)
		{
			HLIB_STD_MemFree (pstDsmccEvt->pucObjInfoByte);
			pstDsmccEvt->pucObjInfoByte = NULL;
			pstDsmccEvt->ulObjInfoByteLen= 0;
		}

		HLIB_STD_MemFree (pstDsmccEvt);
		pstDsmccEvt = NULL;
	}
}

STATIC void xsvc_download_FreeDsmccEventInsideOnly_Hbbtv (void *pvItem)
{
	DxDSMCC_StreamEvent_t	*pstDsmccEvt;

	pstDsmccEvt = (DxDSMCC_StreamEvent_t *)pvItem;
	if (pstDsmccEvt != NULL)
	{
		if (pstDsmccEvt->pszUrl != NULL)
		{
			HLIB_STD_MemFree (pstDsmccEvt->pszUrl);
			pstDsmccEvt->pszUrl = NULL;
		}

		if (pstDsmccEvt->szDescription != NULL)
		{
			HLIB_STD_MemFree (pstDsmccEvt->szDescription);
			pstDsmccEvt->szDescription = NULL;
			pstDsmccEvt->ulDescLen = 0;
		}

		if (pstDsmccEvt->szEventName != NULL)
		{
			HLIB_STD_MemFree (pstDsmccEvt->szEventName);
			pstDsmccEvt->szEventName = NULL;
		}

		if (pstDsmccEvt->pucObjInfoByte != NULL)
		{
			HLIB_STD_MemFree (pstDsmccEvt->pucObjInfoByte);
			pstDsmccEvt->pucObjInfoByte = NULL;
			pstDsmccEvt->ulObjInfoByteLen= 0;
		}
	}
}

STATIC void xsvc_download_FreeDsmccEventInfo_Hbbtv (void *pvItem)
{
	XsvcDownload_HbbtvEvtInfo_t		*pstEvtInfo = (XsvcDownload_HbbtvEvtInfo_t *)pvItem;

	if (NULL != pstEvtInfo)
	{
		if (NULL != pstEvtInfo->pucObjInfoFromEvt)
		{
			HLIB_STD_MemFree(pstEvtInfo->pucObjInfoFromEvt);
		}

		if (NULL != pstEvtInfo->pucObjInfoFromDesc)
		{
			HLIB_STD_MemFree(pstEvtInfo->pucObjInfoFromDesc);
		}

		xsvc_download_FreeDsmccEventInsideOnly_Hbbtv((void *)&(pstEvtInfo->stEvent));
		HLIB_STD_MemFree(pstEvtInfo);
	}
}

STATIC void xsvc_download_FreeDsmccContainer_Hbbtv (SvcDownload_DsmccContents_t *pstContents)
{
	XsvcDownload_DsmccDnInfo_t			*pstDnInfo;
	XsvcDownload_DsmccContainer_t		*pstDsmccData;

	if (pstContents != NULL)
	{
		pstDsmccData = (XsvcDownload_DsmccContainer_t *)pstContents->pvDownloadInfo;
		if (pstDsmccData != NULL)
		{
			pstDnInfo = xsvc_download_GetDownloadInfo_Hbbtv (pstContents);
			xsvc_download_FreeDownloadInfo_Hbbtv (pstDnInfo);
			if (NULL != pstDsmccData->pvPrevEvt)
			{
				HLIB_STD_MemFree(pstDsmccData->pvPrevEvt);
			}

			if (NULL != pstDsmccData->pvPrevEvtDsc)
			{
				HLIB_STD_MemFree(pstDsmccData->pvPrevEvtDsc);
			}

			HLIB_STD_MemFree (pstDsmccData);
			pstDsmccData = NULL;
		}
	}

	return;
}

STATIC HERROR xsvc_download_AppendEvtInfo_Hbbtv (XsvcDownload_DsmccDnInfo_t *pstDnInfo, XsvcDownload_HbbtvEvtInfo_t *pstEvtInfo)
{
	if ((NULL == pstDnInfo) || (NULL == pstEvtInfo))		{ return ERR_FAIL; }

	pstDnInfo->pstEventList = HLIB_LIST_Append(pstDnInfo->pstEventList, (void *)pstEvtInfo);
	return ERR_OK;
}

STATIC HERROR xsvc_download_AppendEvtDescInfo_Hbbtv (XsvcDownload_DsmccDnInfo_t *pstDnInfo, XsvcDownload_HbbtvEvtInfo_t *pstEvtInfo)
{
	if ((NULL == pstDnInfo) || (NULL == pstEvtInfo))		{ return ERR_FAIL; }

	pstDnInfo->pstEventDescList = HLIB_LIST_Append(pstDnInfo->pstEventDescList, (void *)pstEvtInfo);
	return ERR_OK;
}

STATIC HBOOL xsvc_download_CbFindByEventName_Hbbtv (void *pvUserData, void *pvItem)
{
	XsvcDownload_HbbtvEvtInfo_t	*pstEvtInfo = (XsvcDownload_HbbtvEvtInfo_t *)pvItem;
	HUINT8						*pszEventName = (HUINT8 *)pvUserData;

	if ((NULL != pszEventName) && (NULL != pstEvtInfo) && (NULL != pstEvtInfo->stEvent.szEventName))
	{
		return (0 == HxSTD_StrCmp(pstEvtInfo->stEvent.szEventName, pszEventName)) ? TRUE : FALSE;
	}

	return FALSE;
}

STATIC HERROR xsvc_download_FindEventInfoByName_Hbbtv (XsvcDownload_DsmccDnInfo_t *pstDnInfo, HUINT8 *pszEventName, XsvcDownload_HbbtvEvtInfo_t **ppstEvtInfo)
{
	HxList_t						*pstFound;

	if (pstDnInfo == NULL || pszEventName == NULL || ppstEvtInfo == NULL)			{ return ERR_FAIL; }

	pstFound = HLIB_LIST_FindEx(pstDnInfo->pstEventList, (const void *)pszEventName, xsvc_download_CbFindByEventName_Hbbtv);
	*ppstEvtInfo = (XsvcDownload_HbbtvEvtInfo_t *)HLIB_LIST_Data(pstFound);

	return (pstFound) ? ERR_OK : ERR_FAIL;
}

STATIC HBOOL xsvc_download_CbFindByModuleId_Hbbtv (void *pvUserData, void *pvItem)
{
	XsvcDownload_HbbtvEvtInfo_t	*pstEvtInfo = (XsvcDownload_HbbtvEvtInfo_t *)pvItem;
	HUINT32						 ulModuleId = (HUINT32)pvUserData;

	if (NULL != pstEvtInfo)
	{
		return (pstEvtInfo->ulModuleId == ulModuleId) ? TRUE : FALSE;
	}

	return FALSE;
}

STATIC HERROR xsvc_download_FindEventInfoByModuleId_Hbbtv (XsvcDownload_DsmccDnInfo_t *pstDnInfo, HUINT32 ulModuleId, XsvcDownload_HbbtvEvtInfo_t **ppstEvtInfo)
{
	HxList_t						*pstFound;

	if (pstDnInfo == NULL || ppstEvtInfo == NULL)			{ return ERR_FAIL; }

	pstFound = HLIB_LIST_FindEx(pstDnInfo->pstEventList, (const void *)ulModuleId, xsvc_download_CbFindByModuleId_Hbbtv);
	*ppstEvtInfo = (XsvcDownload_HbbtvEvtInfo_t *)HLIB_LIST_Data(pstFound);

	return (pstFound) ? ERR_OK : ERR_FAIL;
}

STATIC HBOOL xsvc_download_CbFindByEventId_Hbbtv (void *pvUserData, void *pvItem)
{
	XsvcDownload_HbbtvEvtInfo_t	*pstEvtInfo = (XsvcDownload_HbbtvEvtInfo_t *)pvItem;
	HUINT32						 ulEventId = (HUINT32)pvUserData;

	if ((NULL != pstEvtInfo) && (pstEvtInfo->stEvent.nEventId == ulEventId))
	{
		return TRUE;
	}

	return FALSE;
}

STATIC HERROR xsvc_download_FindEventDescInfo_Hbbtv (XsvcDownload_DsmccDnInfo_t *pstDnInfo, HINT32 nEventId, XsvcDownload_HbbtvEvtInfo_t **ppstEvtInfo)
{
	HxList_t						*pstFound;

	if (pstDnInfo == NULL || ppstEvtInfo == NULL)			{ return ERR_FAIL; }

	pstFound = HLIB_LIST_FindEx(pstDnInfo->pstEventDescList, (const void *)(0x0000FFFF&nEventId), xsvc_download_CbFindByEventId_Hbbtv);
	*ppstEvtInfo = (XsvcDownload_HbbtvEvtInfo_t *)HLIB_LIST_Data(pstFound);

	return (pstFound) ? ERR_OK : ERR_FAIL;
}

STATIC HERROR xsvc_download_CopyEventItem_Hbbtv (DxDSMCC_StreamEvent_t *pstDstEvent, DxDSMCC_StreamEvent_t *pstSrcEvent)
{
	HUINT32			 ulStrLen;

	HxSTD_memset (pstDstEvent, 0, sizeof(DxDSMCC_StreamEvent_t));

	// PID
	pstDstEvent->usPid = pstSrcEvent->usPid;

	// Event URL
	ulStrLen = (pstSrcEvent->pszUrl != NULL) ? MWC_UTIL_DvbStrlen (pstSrcEvent->pszUrl) : 0;
	pstDstEvent->pszUrl = (HUINT8 *)HLIB_STD_MemCalloc (ulStrLen + 1);
	if (pstDstEvent->pszUrl == NULL)				{ goto ERROR; }
	if (ulStrLen > 0)
	{
		MWC_UTIL_DvbStrcpy (pstDstEvent->pszUrl, pstSrcEvent->pszUrl);
	}
	else
	{
		pstDstEvent->pszUrl[0] = '\0';
	}

	// Description
	pstDstEvent->ulDescLen = pstSrcEvent->ulDescLen;
	ulStrLen = (pstSrcEvent->szDescription != NULL) ? MWC_UTIL_DvbStrlen (pstSrcEvent->szDescription) : 0;
	pstDstEvent->szDescription = (HUINT8 *)HLIB_STD_MemCalloc (ulStrLen + 1);
	if (pstDstEvent->szDescription == NULL)				{ goto ERROR; }
	if (ulStrLen > 0)
	{
		MWC_UTIL_DvbStrcpy (pstDstEvent->szDescription, pstSrcEvent->szDescription);
	}
	else
	{
		pstDstEvent->szDescription[0] = '\0';
	}

	// Event Name
	ulStrLen = (pstSrcEvent->szEventName != NULL) ? MWC_UTIL_DvbStrlen (pstSrcEvent->szEventName) : 0;
	pstDstEvent->szEventName = (HUINT8 *)HLIB_STD_MemCalloc (ulStrLen + 1);
	if (pstDstEvent->szEventName == NULL)				{ goto ERROR; }
	if (ulStrLen > 0)
	{
		MWC_UTIL_DvbStrcpy (pstDstEvent->szEventName, pstSrcEvent->szEventName);
	}
	else
	{
		pstDstEvent->szEventName[0] = '\0';
	}

	// Object Info Byte
	if (pstSrcEvent->ulObjInfoByteLen > 0 && pstSrcEvent->pucObjInfoByte != NULL)
	{
		pstDstEvent->ulObjInfoByteLen = pstSrcEvent->ulObjInfoByteLen;
		pstDstEvent->pucObjInfoByte = (HUINT8 *)HLIB_STD_MemCalloc (pstDstEvent->ulObjInfoByteLen);
		if (pstDstEvent->pucObjInfoByte == NULL)			{ goto ERROR; }
		HxSTD_memcpy (pstDstEvent->pucObjInfoByte, pstSrcEvent->pucObjInfoByte, pstDstEvent->ulObjInfoByteLen);
	}
	else
	{
		pstDstEvent->ulObjInfoByteLen = 0;
		pstDstEvent->pucObjInfoByte = NULL;
	}

	// Event ID
	pstDstEvent->nEventId = pstSrcEvent->nEventId;

	// Association Tag
	pstDstEvent->usTag = pstSrcEvent->usTag;

	return ERR_OK;

ERROR:
	xsvc_download_FreeDsmccEventInsideOnly_Hbbtv (pstDstEvent);
	HxSTD_memset (pstDstEvent, 0, sizeof(DxDSMCC_StreamEvent_t));

	return ERR_FAIL;
}

STATIC XsvcDownload_DsmccDnInfo_t *xsvc_download_AllocDownloadInfo_Hbbtv (void)
{
	XsvcDownload_DsmccDnInfo_t *pstDnInfo;

	pstDnInfo = (XsvcDownload_DsmccDnInfo_t *)OxSI_Malloc (sizeof(XsvcDownload_DsmccDnInfo_t)*MAX_FILTERING_NUM);
	if (pstDnInfo != NULL)
	{
		HxSTD_memset (pstDnInfo, 0, sizeof(XsvcDownload_DsmccDnInfo_t)*MAX_FILTERING_NUM);
	}
	return pstDnInfo;
}

STATIC void xsvc_download_FreeDownloadInfo_Hbbtv (XsvcDownload_DsmccDnInfo_t *pstInfo)
{
	if (pstInfo != NULL)
	{
		HLIB_STD_MemFree (pstInfo);
		pstInfo = NULL;
	}
}

STATIC XsvcDownload_DsmccDnInfo_t *xsvc_download_GetDownloadInfo_Hbbtv (SvcDownload_DsmccContents_t *pstContents)
{
	XsvcDownload_DsmccContainer_t		*pstDsmccData;

	if (pstContents != NULL)
	{
		pstDsmccData = (XsvcDownload_DsmccContainer_t *)pstContents->pvDownloadInfo;
		if (pstDsmccData != NULL)
		{
			return pstDsmccData->pstDnInfo;
		}
	}

	return NULL;
}

STATIC HERROR xsvc_download_InitDdbListsFromDownloadInfo (XsvcDownload_DsmccDnInfo_t *pstDnInfo)
{
	HUINT32				 ulCnt;

	if (NULL == pstDnInfo)				{ return ERR_FAIL; }

	for (ulCnt = 0; ulCnt < DDB_INFO_HASHSIZE; ulCnt++)
	{
		pstDnInfo->apstDdbLists[ulCnt] = NULL;
	}

	return ERR_OK;
}

STATIC HERROR xsvc_download_FreeDdbListsFromDownloadInfo (XsvcDownload_DsmccDnInfo_t *pstDnInfo)
{
	HUINT32				 ulCnt;

	if (NULL == pstDnInfo)				{ return ERR_FAIL; }

	for (ulCnt = 0; ulCnt < DDB_INFO_HASHSIZE; ulCnt++)
	{
		pstDnInfo->apstDdbLists[ulCnt] = HLIB_LIST_RemoveAllFunc(pstDnInfo->apstDdbLists[ulCnt], HLIB_STD_MemFree_CB);
	}

	return ERR_OK;
}

STATIC DsmccDdbInfo_t *xsvc_download_FindDdbInfoFromDownloadInfo (XsvcDownload_DsmccDnInfo_t *pstDnInfo, HUINT32 ulModuleId)
{
	if (NULL != pstDnInfo)
	{
		HUINT32 			 ulHashIndex = (HUINT32)DDB_INFO_HASHINDEX(ulModuleId);
		HxList_t			*pstItem;
		HxList_t			*pstList = pstDnInfo->apstDdbLists[ulHashIndex];

		for (pstItem = pstList; NULL != pstItem; pstItem = pstItem->next)
		{
			DsmccDdbInfo_t		*pstDdbInfo = (DsmccDdbInfo_t *)HLIB_LIST_Data(pstItem);

			if (NULL != pstDdbInfo)
			{
				if ((HUINT32)pstDdbInfo->module_id == ulModuleId)
				{
					return pstDdbInfo;
				}
			}
		}
	}

	return NULL;
}

STATIC HERROR xsvc_download_AppendDdbInfoFromDownloadInfo (XsvcDownload_DsmccDnInfo_t *pstDnInfo, DsmccDdbInfo_t *pstDdbInfo)
{
	if ((NULL != pstDnInfo) && (NULL != pstDdbInfo))
	{
		HUINT32 			 ulHashIndex = (HUINT32)DDB_INFO_HASHINDEX(pstDdbInfo->module_id);
		DsmccDdbInfo_t		*pstDupDdbInfo;

		pstDupDdbInfo = (DsmccDdbInfo_t *)HLIB_STD_MemAlloc(sizeof(DsmccDdbInfo_t));
		if (NULL != pstDupDdbInfo)
		{
			HxSTD_MemCopy(pstDupDdbInfo, pstDdbInfo, sizeof(DsmccDdbInfo_t));
			pstDnInfo->apstDdbLists[ulHashIndex] = HLIB_LIST_Append(pstDnInfo->apstDdbLists[ulHashIndex], (void *)pstDupDdbInfo);

			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

#define _____FILTERING_FUNCTIONS_____
STATIC void  xsvc_download_CbTimerDsi (unsigned long TimerId, void *params)
{
	HUINT16						 usPid = *((HUINT16 *)params);
	HINT32						 nCnt;
	XsvcDownload_DsmccDnInfo_t	*pstDnInfo_Filtered = NULL, *pstDnArray = NULL;

	pstDnArray = xsvc_download_GetDownloadInfo_Hbbtv(s_pstContents);
	if (NULL != pstDnArray)
	{
		for (nCnt = 0 ; nCnt < MAX_FILTERING_NUM ; nCnt++)
		{
			if (pstDnArray[nCnt].usPid == usPid)
			{
				pstDnInfo_Filtered = &(pstDnArray[nCnt]);
				break;
			}
		}

		if (NULL != pstDnInfo_Filtered)
		{
			for (nCnt = 0 ; nCnt < MAX_FILTERING_NUM ; nCnt++)
			{
				if (pstDnArray[nCnt].uiCarouselId == pstDnInfo_Filtered->uiCarouselId)
				{
					VK_TIMER_Cancel(pstDnArray[nCnt].ulDsiTimerId);
					pstDnArray[nCnt].ulDsiTimerId = 0;
				}
			}

			svc_download_UtilPostMsgToMgr(eSEVT_DOWN_DOWNLOAD_FAIL, s_pstContents->hAction, s_pstContents->eActType, (HUINT32)usPid, pstDnInfo_Filtered->uiCarouselId);
		}
	}
}

STATIC void  xsvc_download_CbTimerDii (unsigned long TimerId, void *params)
{
	HUINT16						 usPid = *((HUINT16 *)params);
	HINT32						 nCnt;
	XsvcDownload_DsmccDnInfo_t	*pstDnInfo_Filtered = NULL, *pstDnArray = NULL;

	pstDnArray = xsvc_download_GetDownloadInfo_Hbbtv(s_pstContents);
	if (NULL != pstDnArray)
	{
		for (nCnt = 0 ; nCnt < MAX_FILTERING_NUM ; nCnt++)
		{
			if (pstDnArray[nCnt].usPid == usPid)
			{
				pstDnInfo_Filtered = &(pstDnArray[nCnt]);
				break;
			}
		}

		if (NULL != pstDnInfo_Filtered)
		{
			for (nCnt = 0 ; nCnt < MAX_FILTERING_NUM ; nCnt++)
			{
				if (pstDnArray[nCnt].uiCarouselId == pstDnInfo_Filtered->uiCarouselId)
				{
					VK_TIMER_Cancel(pstDnArray[nCnt].ulDiiTimerId);
					pstDnArray[nCnt].ulDiiTimerId = 0;
				}
			}

			svc_download_UtilPostMsgToMgr(eSEVT_DOWN_DOWNLOAD_FAIL, s_pstContents->hAction, s_pstContents->eActType, (HUINT32)usPid, pstDnInfo_Filtered->uiCarouselId);
		}
	}
}

STATIC HERROR xsvc_download_StartDsiDiiFiltering_Hbbtv (SvcDownload_DsmccContents_t *pstContents, HUINT16 usPid)
{
	HUINT16						 usFilterId = 0;
	HUINT32						 ulCnt;
	SiRetrieveParam_t	 		 stParam;
	XsvcDownload_DsmccDnInfo_t	*pstDnInfo;
	HERROR						 hErr, hResult = ERR_FAIL;

	HxLOG_Print("(+) usPid(0x%04x)\n", usPid);

	if (NULL == pstContents)
	{
		HxLOG_Error("pstContents NULL\n");
		goto END_FUNC;
	}

	pstDnInfo = xsvc_download_GetDownloadInfo_Hbbtv(pstContents);
	if (NULL == pstDnInfo)
	{
		HxLOG_Error("xsvc_download_GetDownloadInfo_Hbbtv err:\n");
		goto END_FUNC;
	}

	HxSTD_memset(&stParam, 0, sizeof(SiRetrieveParam_t));
	for (ulCnt = 0 ; ulCnt < MAX_FILTERING_NUM ; ulCnt++)
	{
		XsvcDownload_DsmccDnInfo_t	*pstDnItem = &(pstDnInfo[ulCnt]);

		if ((pstDnItem->usPid == usPid) && (pstDnItem->usDsiFilterId == FILTER_ID_NULL))
		{
			stParam.hAction 		= pstContents->hAction;
			stParam.usTsUniqId		= pstContents->usTsUniqId;
			stParam.ucDemuxId		= (HUINT8)pstContents->ulDemuxId;

			stParam.usPid			= pstDnItem->usPid;
			stParam.ucMode			= eSiFilterMode_Continue;
			stParam.bTableFilter	= FALSE;
			stParam.bUseExtId		= FALSE;

			pstDnItem->eDsiMode	= eFILTER_MODE_RECEIVING;
			pstDnItem->eDiiMode	= eFILTER_MODE_RECEIVING;

			if (0 != pstDnItem->ulDsiTimerId)
			{
				VK_TIMER_Cancel(pstDnItem->ulDsiTimerId);
				pstDnItem->ulDsiTimerId = 0;
			}

			if (0 != pstDnItem->ulDiiTimerId)
			{
				VK_TIMER_Cancel(pstDnItem->ulDiiTimerId);
				pstDnItem->ulDiiTimerId = 0;
			}

			hErr = xsvc_download_DmxRetrieveDsiSection(&stParam, (void *)xsvc_download_CallbackDsiDii_Hbbtv, NULL, &usFilterId);
			if (ERR_OK == hErr)
			{
				pstDnItem->usDsiFilterId = usFilterId;
				(void)VK_TIMER_EventAfter((10 * 1000), xsvc_download_CbTimerDsi, &(pstDnItem->usPid), sizeof(HUINT16), &(pstDnItem->ulDsiTimerId));
			}
			else
			{
				pstDnItem->usDsiFilterId = FILTER_ID_NULL;
			}

			HxLOG_Debug("Index(%d) PID(0x%04x) FilterId(%d)\n", ulCnt, pstDnItem->usPid, pstDnItem->usDsiFilterId);

			hResult = ERR_OK;
			goto END_FUNC;
		}
	}

END_FUNC:
	HxLOG_Print("(-) Return Err:0x%04x\n", hResult);
	return hResult;
}

STATIC HERROR xsvc_download_StartDdbFiltering_Hbbtv (SvcDownload_DsmccContents_t *pstContents, HUINT16 usPid)
{
	HUINT16 					 usFilterId = 0;
	HUINT32 					 ulCnt;
	SiRetrieveParam_t			 stParam;
	XsvcDownload_DsmccDnInfo_t	*pstDnInfo;
	HERROR						 hErr, hResult = ERR_FAIL;

	HxLOG_Print("(+) usPid(0x%04x)\n", usPid);

	if (NULL == pstContents)
	{
		HxLOG_Error("pstContents NULL\n");
		goto END_FUNC;
	}

	pstDnInfo = xsvc_download_GetDownloadInfo_Hbbtv(pstContents);
	if (NULL == pstDnInfo)
	{
		HxLOG_Error("xsvc_download_GetDownloadInfo_Hbbtv err:\n");
		goto END_FUNC;
	}

	HxSTD_memset(&stParam, 0, sizeof(SiRetrieveParam_t));
	for (ulCnt = 0 ; ulCnt < MAX_FILTERING_NUM ; ulCnt++)
	{
		XsvcDownload_DsmccDnInfo_t	*pstDnItem = &(pstDnInfo[ulCnt]);

		if ((pstDnItem->usPid == usPid) && (pstDnItem->usDdbFilterId == FILTER_ID_NULL))
		{
			HxSTD_memset (&stParam, 0, sizeof(SiRetrieveParam_t));
			stParam.hAction	 		= pstContents->hAction;
			stParam.usTsUniqId 		= pstContents->usTsUniqId;
			stParam.ucDemuxId 		= (HUINT8)pstContents->ulDemuxId;

			stParam.usPid	 		= pstDnItem->usPid;
			stParam.ucMode	 		= eSiFilterMode_Continue;
			stParam.bTableFilter	= FALSE;
			stParam.bUseExtId 		= FALSE;

			hErr = xsvc_download_DmxRetrieveDdbSection(&stParam, (void *)xsvc_download_CallbackDdb_Hbbtv, NULL, &usFilterId);
			if (ERR_OK == hErr)
			{
				pstDnItem->usDdbFilterId = usFilterId;
			}
			else
			{
				pstDnItem->usDdbFilterId = FILTER_ID_NULL;
			}

			HxLOG_Debug("Index(%d) PID(0x%04x) FilterId(%d)\n", ulCnt, pstDnItem->usPid, pstDnItem->usDdbFilterId);

			hResult = ERR_OK;
			goto END_FUNC;
		}
	}

END_FUNC:
	HxLOG_Print("(-) Return Err:0x%04x\n", hResult);
	return hResult;
}

STATIC HERROR xsvc_download_StartDescFiltering_Hbbtv (SvcDownload_DsmccContents_t *pstContents, HUINT16 usPid)
{
	HUINT16 					 usFilterId = 0;
	HUINT32 					 ulCnt;
	SiRetrieveParam_t			 stParam;
	XsvcDownload_DsmccDnInfo_t	*pstDnInfo;
	HERROR						 hErr, hResult = ERR_FAIL;

	HxLOG_Print("(+) usPid(0x%04x)\n", usPid);

	if (NULL == pstContents)
	{
		HxLOG_Error("pstContents NULL\n");
		goto END_FUNC;
	}

	pstDnInfo = xsvc_download_GetDownloadInfo_Hbbtv(pstContents);
	if (NULL == pstDnInfo)
	{
		HxLOG_Error("xsvc_download_GetDownloadInfo_Hbbtv err:\n");
		goto END_FUNC;
	}

	HxSTD_memset(&stParam, 0, sizeof(SiRetrieveParam_t));
	for (ulCnt = 0 ; ulCnt < MAX_FILTERING_NUM ; ulCnt++)
	{
		XsvcDownload_DsmccDnInfo_t	*pstDnItem = &(pstDnInfo[ulCnt]);

		if ((pstDnItem->usPid == usPid) && (pstDnItem->usDescFilterId == FILTER_ID_NULL))
		{
			HxSTD_memset (&stParam, 0, sizeof(SiRetrieveParam_t));
			stParam.hAction 		= pstContents->hAction;
			stParam.usTsUniqId		= pstContents->usTsUniqId;
			stParam.ucDemuxId		= (HUINT8)pstContents->ulDemuxId;

			stParam.usPid			= pstDnItem->usPid;
			stParam.ucMode			= eSiFilterMode_Continue;
			stParam.bTableFilter	= FALSE;
			stParam.bUseExtId		= FALSE;

			pstDnItem->eDsiMode		= eFILTER_MODE_RECEIVING;
			pstDnItem->eDiiMode		= eFILTER_MODE_RECEIVING;

			hErr = xsvc_download_DmxRetrieveDescSection(&stParam, (void *)xsvc_download_CallbackDesc_Hbbtv, NULL, &usFilterId);
			if (ERR_OK == hErr)
			{
				pstDnItem->usDescFilterId = usFilterId;
			}
			else
			{
				pstDnItem->usDescFilterId = FILTER_ID_NULL;
			}

			HxLOG_Debug("Index(%d) PID(0x%04x) FilterId(%d)\n", ulCnt, pstDnItem->usPid, pstDnItem->usDescFilterId);

			hResult = ERR_OK;
			goto END_FUNC;
		}
	}

END_FUNC:
	HxLOG_Print("(-) Return Err:0x%04x\n", hResult);
	return hResult;
}

STATIC HERROR xsvc_download_StopDsiDiiFiltering_Hbbtv (SvcDownload_DsmccContents_t *pstContents, HUINT32 ulIdx, SvcDownload_Msg_t *pstMsg )
{
	XsvcDownload_DsmccDnInfo_t			*pstDnInfo 	= NULL;

	HxLOG_Print("[%s:%d] (+)\n", __FUNCTION__, __LINE__);

	if (pstContents == NULL)
	{
		return ERR_DOWN_TARGET_NULL;
	}

	pstDnInfo = xsvc_download_GetDownloadInfo_Hbbtv (pstContents);

	if ( pstMsg->ulMsgClass == CMD_DOWN_START ||
		 pstMsg->ulMsgClass == CMD_DOWN_STOP )
	{
		if ( ( pstDnInfo[ulIdx].usDsiFilterId != FILTER_ID_NULL ) &&
			 ( pstMsg->unArg.stStopDown.usPid != 0 ) && ( pstDnInfo[ulIdx].usPid == pstMsg->unArg.stStopDown.usPid ) )
		{
			HxLOG_Debug("Stoped by PID(%d)-PID(%04x)FILTER(%08x)\n",
											pstMsg->unArg.stStopDown.usPid, pstDnInfo[ulIdx].usPid, pstDnInfo[ulIdx].usDsiFilterId);

			xsvc_download_DmxStopOneSection(pstDnInfo[ulIdx].usDsiFilterId);
			pstDnInfo[ulIdx].usDsiFilterId = FILTER_ID_NULL;
			return ERR_DOWN_RESULT_OK;
		}
	}

	if ( pstMsg->ulMsgClass == DOWNCB_DSI_TABLE )
	{
		if ( pstMsg->unArg.stSection.ulFilterId == pstDnInfo[ulIdx].usDsiFilterId && pstDnInfo[ulIdx].usDsiFilterId != 0  )
		{
			HxLOG_Debug("Stoped by PID(%d)-PID(%04x)FILTER(%08x)\n",
											pstMsg->unArg.stStopDown.usPid, pstDnInfo[ulIdx].usPid, pstDnInfo[ulIdx].usDsiFilterId);

			xsvc_download_DmxStopOneSection(pstDnInfo[ulIdx].usDsiFilterId);
			pstDnInfo[ulIdx].usDsiFilterId = FILTER_ID_NULL;
			return ERR_DOWN_RESULT_OK;
		}
	}

	HxLOG_Print("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

	return ERR_DOWN_RESULT_FAIL;
}

STATIC HERROR xsvc_download_StopDdbFiltering_Hbbtv (SvcDownload_DsmccContents_t *pstContents, HUINT32 ulIdx, SvcDownload_Msg_t *pstMsg )
{
 	XsvcDownload_DsmccDnInfo_t			*pstDnInfo = NULL, *pstDnArray = NULL;

	HxLOG_Print("[%s:%d] (+)\n", __FUNCTION__, __LINE__);

	if (NULL == pstContents)					{ return ERR_FAIL; }

	pstDnArray = xsvc_download_GetDownloadInfo_Hbbtv(pstContents);
	if (NULL == pstDnArray)						{ return ERR_FAIL; }

	pstDnInfo = &(pstDnArray[ulIdx]);

	if (FILTER_ID_NULL != pstDnInfo->usDdbFilterId)
	{
		xsvc_download_DmxStopOneSection(pstDnInfo->usDdbFilterId);
		pstDnInfo->usDdbFilterId = FILTER_ID_NULL;
	}

	return ERR_OK;
 }

STATIC HERROR xsvc_download_StopDescFiltering_Hbbtv (SvcDownload_DsmccContents_t *pstContents, HUINT32 ulIdx, SvcDownload_Msg_t *pstMsg )
{
	XsvcDownload_DsmccDnInfo_t			*pstDnInfo	= NULL;

	HxLOG_Print("[%s:%d] (+)\n", __FUNCTION__, __LINE__);

	if (pstContents == NULL)
	{
		return ERR_DOWN_TARGET_NULL;
	}

	pstDnInfo = xsvc_download_GetDownloadInfo_Hbbtv (pstContents);

	if ( pstMsg->ulMsgClass == CMD_DOWN_START ||
		 pstMsg->ulMsgClass == CMD_DOWN_STOP )
	{
		if ( ( pstDnInfo[ulIdx].usDescFilterId != FILTER_ID_NULL ) &&
			 ( pstMsg->unArg.stStopDown.usPid != 0 ) && ( pstDnInfo[ulIdx].usPid == pstMsg->unArg.stStopDown.usPid ) )
		{
			HxLOG_Debug("Stoped by PID(%d)-PID(%04x)FILTER(%08x)\n",
											pstMsg->unArg.stStopDown.usPid, pstDnInfo[ulIdx].usPid, pstDnInfo[ulIdx].usDescFilterId);

			xsvc_download_DmxStopOneSection(pstDnInfo[ulIdx].usDescFilterId);
			pstDnInfo[ulIdx].usDescFilterId 	= FILTER_ID_NULL;
			return ERR_DOWN_RESULT_OK;
		}
	}

	if ( pstMsg->ulMsgClass == DOWNCB_DSI_TABLE ||
		 pstMsg->ulMsgClass == DOWNCB_DDB_TABLE )
	{
		if ( pstMsg->unArg.stSection.ulFilterId == pstDnInfo[ulIdx].usDescFilterId && pstDnInfo[ulIdx].usDescFilterId != 0 )
		{
			HxLOG_Debug("Stoped by PID(%d)-PID(%04x)FILTER(%08x)\n",
											pstMsg->unArg.stStopDown.usPid, pstDnInfo[ulIdx].usPid, pstDnInfo[ulIdx].usDescFilterId);

			xsvc_download_DmxStopOneSection(pstDnInfo[ulIdx].usDescFilterId);
			pstDnInfo[ulIdx].usDescFilterId 	= FILTER_ID_NULL;
			return ERR_DOWN_RESULT_OK;
		}
	}

	HxLOG_Print("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

	return ERR_DOWN_RESULT_FAIL;
}

STATIC HERROR xsvc_download_StopFiltering_Hbbtv (SvcDownload_DsmccContents_t *pstContents, SvcDownload_Msg_t *pstMsg)
{
	HUINT32						 ulCnt;
	HUINT16						 usPid = 0xFFFF;
	XsvcDownload_DsmccDnInfo_t	*pstDnInfo	= NULL;
	HBOOL						 bStopAll = FALSE;
	HERROR						 hResult = ERR_FAIL;

	HxLOG_Print("(+)\n");

	if (pstContents == NULL)
	{
		goto END_FUNC;
	}

	switch (pstMsg->ulMsgClass)
	{
	case CMD_DOWN_START:
	case CMD_DOWN_STOP:
		bStopAll = TRUE;
		break;

	case CMD_DOWN_SET:
		if (eDOWN_SET_HBBTV_REMOVEPID == pstMsg->unArg.stSet.eSetType)
		{
			bStopAll = FALSE;
			usPid = pstMsg->unArg.stSet.stHbbtvRemovePid.usPid;
		}
		else
		{
			goto END_FUNC;
		}

		break;

	default:
		goto END_FUNC;
	}
	pstDnInfo = xsvc_download_GetDownloadInfo_Hbbtv(pstContents);

	for (ulCnt = 0 ; ulCnt < MAX_FILTERING_NUM ; ulCnt++)
	{
		if ((TRUE == bStopAll) || (usPid == pstDnInfo[ulCnt].usPid))
		{
			if (FILTER_ID_NULL != pstDnInfo[ulCnt].usDsiFilterId)
			{
				xsvc_download_DmxStopOneSection(pstDnInfo[ulCnt].usDsiFilterId);
				pstDnInfo[ulCnt].usDsiFilterId = FILTER_ID_NULL;
			}

			if (FILTER_ID_NULL != pstDnInfo[ulCnt].usDdbFilterId)
			{
				xsvc_download_DmxStopOneSection(pstDnInfo[ulCnt].usDdbFilterId);
				pstDnInfo[ulCnt].usDdbFilterId = FILTER_ID_NULL;
			}

			if (FILTER_ID_NULL != pstDnInfo[ulCnt].usDescFilterId)
			{
				xsvc_download_DmxStopOneSection(pstDnInfo[ulCnt].usDescFilterId);
				pstDnInfo[ulCnt].usDescFilterId = FILTER_ID_NULL;
			}

			xsvc_download_ClearDnInfo_Hbbtv(pstContents, ulCnt, pstMsg);
			xsvc_download_ClearLibResource_Hbbtv(pstContents, ulCnt);
		}
	}

	hResult = ERR_OK;

END_FUNC:
	HxLOG_Print("(-) hResult(%d0\n", hResult);
	return hResult;
}

STATIC HERROR xsvc_download_RestartDsiDiiFilter_Hbbtv (SvcDownload_DsmccContents_t *pstContents, SvcDownload_Msg_t *pstMsg )
{
	HINT32						 nCnt;
	XsvcDownload_DsmccDnInfo_t	*pstDnInfo_Filtered = NULL, *pstDnArray;

	HxLOG_Print("[%s:%d] (+) FilterID(%d)\n", __FUNCTION__, __LINE__, pstMsg->unArg.stSection.ulFilterId);

	pstDnArray = xsvc_download_GetDownloadInfo_Hbbtv(pstContents);
	if (NULL == pstDnArray)					{ return ERR_DOWN_RESULT_FAIL; }

	for (nCnt = 0; nCnt < MAX_FILTERING_NUM; nCnt++)
	{
		if (pstDnArray[nCnt].usDsiFilterId == pstMsg->unArg.stSection.ulFilterId )
		{
			pstDnInfo_Filtered = &(pstDnArray[nCnt]);
			break;
		}
	}

	if (NULL == pstDnInfo_Filtered)
	{
		HxLOG_Error ("Error:\n");
		return ERR_DOWN_RESULT_FAIL;
	}

	for (nCnt = 0; nCnt < MAX_FILTERING_NUM; nCnt++)
	{
		XsvcDownload_DsmccDnInfo_t	*pstDnInfo = &(pstDnArray[nCnt]);

		if (pstDnInfo->uiCarouselId == pstDnInfo_Filtered->uiCarouselId)
		{
			if (FILTER_ID_NULL != pstDnInfo->usDsiFilterId)
			{
				xsvc_download_DmxStopOneSection(pstDnInfo->usDsiFilterId);
				pstDnInfo->usDsiFilterId = FILTER_ID_NULL;
			}

			if (FILTER_ID_NULL != pstDnInfo->usDdbFilterId)
			{
				xsvc_download_DmxStopOneSection(pstDnInfo->usDdbFilterId);
				pstDnInfo->usDdbFilterId = FILTER_ID_NULL;
			}

			if (FILTER_ID_NULL != pstDnInfo->usDescFilterId)
			{
				xsvc_download_DmxStopOneSection(pstDnInfo->usDescFilterId);
				pstDnInfo->usDescFilterId = FILTER_ID_NULL;
			}

			pstDnInfo->eDsiMode = eFILTER_MODE_NONE;
			pstDnInfo->eDiiMode = eFILTER_MODE_NONE;

#ifdef __USE_LIBDSMCC__ /* by Dylan - libdsmcc */
			xsvc_download_ClearLibResource_Hbbtv(pstContents, nCnt);
#endif
		}
	}

	svc_download_UtilPostMsgToMgr(eSEVT_DOWN_DOWNLOAD_CHANGED, pstContents->hAction, (HINT32)pstContents->eActType, (HINT32)pstDnInfo_Filtered->usPid, (HINT32)pstDnInfo_Filtered->uiCarouselId);

	HxLOG_Print("[%s:%d] (-) FilterID(%d)\n", __FUNCTION__, __LINE__, pstMsg->unArg.stSection.ulFilterId);
	return ERR_DOWN_RESULT_OK;
}

#ifdef __USE_LIBDSMCC__ /* by Dylan - libdsmcc */
STATIC HERROR xsvc_download_ClearLibResource_Hbbtv ( SvcDownload_DsmccContents_t *pstContents, unsigned int uiIndex )
{
	XsvcDownload_DsmccDnInfo_t		*pstDnInfo	= NULL;

	HxLOG_Print("[%s:%d] (+)\n", __FUNCTION__, __LINE__);

	pstDnInfo = xsvc_download_GetDownloadInfo_Hbbtv (pstContents);

	ENTER_CRITICAL_DNLDCNTS;

	if ( pstDnInfo[uiIndex].pstDsmccStatus != NULL)
	{
		dsmcc_close( pstDnInfo[uiIndex].pstDsmccStatus );
		pstDnInfo[uiIndex].pstDsmccStatus = NULL;
	}

	pstDnInfo[uiIndex].pstDiiList = HLIB_LIST_RemoveAllFunc(pstDnInfo[uiIndex].pstDiiList, HLIB_STD_MemFree_CB);
	pstDnInfo[uiIndex].pstEventList = HLIB_LIST_RemoveAllFunc(pstDnInfo[uiIndex].pstEventList, xsvc_download_FreeDsmccEventInfo_Hbbtv);
	pstDnInfo[uiIndex].pstEventDescList = HLIB_LIST_RemoveAllFunc(pstDnInfo[uiIndex].pstEventDescList, xsvc_download_FreeDsmccEventInfo_Hbbtv);

	(void)xsvc_download_FreeDdbListsFromDownloadInfo(&(pstDnInfo[uiIndex]));

	LEAVE_CRITICAL_DNLDCNTS;

	if (0 != pstDnInfo[uiIndex].ulFinishTimerId)
	{
		VK_TIMER_Cancel(pstDnInfo[uiIndex].ulFinishTimerId);
		pstDnInfo[uiIndex].ulFinishTimerId = 0;
	}

	if (0 != pstDnInfo[uiIndex].ulDsiTimerId)
	{
		VK_TIMER_Cancel(pstDnInfo[uiIndex].ulDsiTimerId);
		pstDnInfo[uiIndex].ulDsiTimerId = 0;
	}

	if (0 != pstDnInfo[uiIndex].ulDiiTimerId)
	{
		VK_TIMER_Cancel(pstDnInfo[uiIndex].ulDiiTimerId);
		pstDnInfo[uiIndex].ulDiiTimerId = 0;
	}

	//pstDnInfo[uiIndex].ucLastDescSectionVersion = 0xff;

	HxLOG_Print("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

	return ERR_DOWN_RESULT_OK;
}
#endif

STATIC HERROR xsvc_download_ClearDnInfo_Hbbtv (SvcDownload_DsmccContents_t *pstContents, HUINT32 ulIdx, SvcDownload_Msg_t *pstMsg )
{
	XsvcDownload_DsmccDnInfo_t			*pstDnInfo	= NULL;

	HxLOG_Print("[%s:%d] (+)\n", __FUNCTION__, __LINE__);

	pstDnInfo = xsvc_download_GetDownloadInfo_Hbbtv (pstContents);

	pstDnInfo[ulIdx].usPid 			= 0;
	pstDnInfo[ulIdx].usDsiFilterId 		= FILTER_ID_NULL;
	pstDnInfo[ulIdx].usDdbFilterId 		= FILTER_ID_NULL;
	pstDnInfo[ulIdx].usDescFilterId 		= FILTER_ID_NULL;
	pstDnInfo[ulIdx].bDsiParsed		= FALSE;
	pstDnInfo[ulIdx].bDiiParsed		= FALSE;
	pstDnInfo[ulIdx].bDdbZeroBlockParsed	= FALSE;
	pstDnInfo[ulIdx].bDdbAllReceived	= FALSE;

	pstDnInfo[ulIdx].usAssocTag			= 0;
	pstDnInfo[ulIdx].ulDsiCarouselId	= 0;
	pstDnInfo[ulIdx].ulDownloadId		= 0;
	pstDnInfo[ulIdx].nDdbSecVer			= -1;

	HxLOG_Print("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

	return ERR_DOWN_RESULT_OK;
}

#define _____SECTION_CALLBACK_____

STATIC void xsvc_download_CallbackDsiDii_Hbbtv (HUINT32 ulTableIdAndResult, HUINT32 ulfilterAndUniqId, HUINT32 ulParam, Handle_t hAction)
{
	HUINT16		 	usMessageId	= 0;
	HUINT32		 	ulResult, ulFilterId;
	HUINT8			*pucRaw;
	PalSef_RawData_t		*pstSiRawData;

	HxLOG_Print("[%s:%d] (+)\n", __FUNCTION__, __LINE__);

	ulResult = ulTableIdAndResult & 0x0000ffff;
	ulFilterId = (ulfilterAndUniqId & 0xffff0000) >> 16;

	if (ulResult != (HUINT32)eSiFilterResult_Complete)
	{
		HxLOG_Error ("Error:\n");
		return;
	}

	pstSiRawData = (PalSef_RawData_t *)ulParam;
	if (pstSiRawData == NULL)
	{
		HxLOG_Error ("Error:\n");
		return;
	}

	pucRaw = pstSiRawData->buf[0];
	if (pucRaw == NULL)
	{
		HxLOG_Error ("Error:\n");
		return;
	}

#ifdef DSI_DUMP
	VK_Print("\nDSI/DII cb============================In==\n");
	for( i=0; i < (HINT32)get12bit (pucRaw + 1) + 3;i++)
	{
		if( i % 16 == 0 )
			VK_Print("\n" );
		VK_Print("%02X ",(unsigned char ) pucRaw[i] );
	}
	VK_Print("\nDSI/DII+ cb===========================Out==\n");
#endif

	usMessageId = get16bit (pucRaw + SECTION_MESSAGEID_POS);

	if (usMessageId == DSI_MESSAGE_ID)
	{
		HxLOG_Debug("DSI RECEIVED, MSGID(0x%04x)\n", usMessageId);
		xsvc_download_ParseDsi_Hbbtv (pucRaw, hAction, ulFilterId);
	}
	else if (usMessageId == DII_MESSAGE_ID)
	{
		HxLOG_Debug("DII RECEIVED, MSGID(0x%04x)\n", usMessageId);
		xsvc_download_ParseDii_Hbbtv (pucRaw, hAction, ulFilterId);
	}
	else
	{
		HxLOG_Error ("Unknown RECEIVED, MSGID(0x%04x)\n", usMessageId);
	}

	HxLOG_Print("[%s:%d] (-)\n", __FUNCTION__, __LINE__);
	return;
}

STATIC void xsvc_download_CallbackDdb_Hbbtv (HUINT32 ulTableIdAndResult, HUINT32 ulfilterAndUniqId, HUINT32 ulParam, Handle_t hAction)
{
	HUINT16			 usMessageId;
	HUINT32			 ulResult, ulFilterId;
	HUINT8			*pucRaw;
	PalSef_RawData_t		*pstSiRawData;

	HxLOG_Print("[%s:%d] (+)\n", __FUNCTION__, __LINE__);

	ulResult = ulTableIdAndResult & 0x0000ffff;
	ulFilterId = (ulfilterAndUniqId & 0xffff0000) >> 16;

	if (ulResult != (HUINT32)eSiFilterResult_Complete)
	{
		HxLOG_Error ("Error:\n");
		return;
	}

	pstSiRawData = (PalSef_RawData_t *)ulParam;
	if (pstSiRawData == NULL)
	{
		HxLOG_Error ("Error:\n");
		return;
	}

	pucRaw = pstSiRawData->buf[0];
	if (pucRaw == NULL)
	{
		HxLOG_Error ("Error:\n");
		return;
	}

#ifdef DDB_DUMP
	VK_Print("\nDDB cb============================In==\n");
	for( i=0; i < (HINT32)get12bit (pucRaw + 1) + 3;i++)
	{
	if( i % 16 == 0 )
			VK_Print("\n" );
		VK_Print("%02X ", (unsigned char )pucRaw[i] );
	}
	VK_Print("\nDDB cb============================Out==\n");
#endif

	usMessageId = get16bit (pucRaw + SECTION_MESSAGEID_POS);

	if (usMessageId == DDB_MESSAGE_ID)
	{
		HxLOG_Debug("DDB RECEIVED, MSGID(0x%04x)\n", usMessageId);
		xsvc_download_ParseDdb_Hbbtv (pucRaw, hAction, ulFilterId);
	}
	else
	{
		HxLOG_Error ("Error:\n");
	}

	HxLOG_Print("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

	return;
}

STATIC void xsvc_download_CallbackDesc_Hbbtv (HUINT32 ulTableIdAndResult, HUINT32 ulfilterAndUniqId, HUINT32 ulParam, Handle_t hAction)
{
	HUINT32		 	ulResult, ulFilterId;
	HUINT8			*pucRaw;
	PalSef_RawData_t		*pstSiRawData;

	HxLOG_Print("[%s:%d] (+)\n", __FUNCTION__, __LINE__);

	ulResult = ulTableIdAndResult & 0x0000ffff;
	ulFilterId = (ulfilterAndUniqId & 0xffff0000) >> 16;

	if (ulResult != (HUINT32)eSiFilterResult_Complete)
	{
		HxLOG_Error ("Error:\n");
		svc_download_SendSectionMessage (DOWNCB_DESC_TABLE, hAction, eSI_TABLE_FAIL, ulFilterId);
		return;
	}

	pstSiRawData = (PalSef_RawData_t *)ulParam;
	if (pstSiRawData == NULL)
	{
		HxLOG_Error ("Error:\n");
		return;
	}

	pucRaw = pstSiRawData->buf[0];
	if (pucRaw == NULL)
	{
		HxLOG_Error ("Error:\n");
		return;
	}

#ifdef DESC_DUMP
	VK_Print("\nDesc cb============================In==\n");
	for( i=0; i < (HINT32)get12bit (pucRaw + 1) + 3;i++)
	{
		if( i % 16 == 0 )
			VK_Print("\n" );
		VK_Print("%02X ",(unsigned char ) pucRaw[i] );
	}
	VK_Print("\nDesc cb===========================Out==\n");
#endif

	HxLOG_Debug("DESC_MESSAGE_ID RECEIVED\n");

	xsvc_download_ParseDesc_Hbbtv (pucRaw, hAction, ulFilterId);

	HxLOG_Print("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

	return;
}

#define _____START_FUNCTIONS_____
STATIC HERROR xsvc_download_AddPidOnContentTable_Hbbtv (XsvcDownload_DsmccDnInfo_t *pstDnInfo, SvcDownload_Msg_t *pstMsg)
{
	HUINT32							 ulCnt;
	HCHAR							*szPath = NULL;
	FILE							*pDbgFp = NULL;
	SvcDownload_SetHbbtvAddPid_t	*pstMsgAddPid = NULL;
	XsvcDownload_DsmccDnInfo_t		*pstDnItem = NULL;
	HERROR							 hResult = ERR_FAIL;

	HxLOG_Print("(+)\n");

	if ((NULL == pstDnInfo) || (NULL == pstMsg))
	{
		goto END_FUNC;
	}

	pstMsgAddPid = &(pstMsg->unArg.stSet.stHbbtvAddPid);

	for (ulCnt = 0; ulCnt < MAX_FILTERING_NUM; ulCnt++)
	{
		if (pstDnInfo[ulCnt].usPid == pstMsgAddPid->usPid)
		{
			HxLOG_Error("PID (0x%04x) already downloading...\n", pstMsgAddPid->usPid);
			goto END_FUNC;
		}
		else if (0 == pstDnInfo[ulCnt].usPid)
		{
			break;
		}
	}

	if (ulCnt >= MAX_FILTERING_NUM)
	{
		HxLOG_Error("Already the filter is MAX\n");
		goto END_FUNC;
	}

	pstDnItem = &(pstDnInfo[ulCnt]);

	HxLOG_Print("PID 0x%04x -> Index (%d)\n", pstMsgAddPid->usPid, ulCnt);
	pstDnItem->usPid = pstMsgAddPid->usPid;
	pstDnItem->eStreamType = pstMsgAddPid->ucStreamType;
	pstDnItem->usDsiFilterId = FILTER_ID_NULL;
	pstDnItem->usDdbFilterId = FILTER_ID_NULL;
	pstDnItem->usDescFilterId = FILTER_ID_NULL;
	pstDnItem->uiCarouselId = pstMsgAddPid->ulCarouselId;

	pstDnItem->ulDownloadId = 0;
	pstDnItem->ulLastDiiTick = 0;
	pstDnItem->ulFinishTimerId = 0;
	pstDnItem->ulTimeoutValue = CAROUSEL_TIMEOUT_DEFAULT;
	//pstDnItem->ucLastDescSectionVersion = 0xff;
	pstDnItem->pstDiiList = NULL;
	pstDnItem->pstEventList = NULL;
	pstDnItem->pstEventDescList = NULL;

	pstDnItem->ulDsiTimerId = 0;
	pstDnItem->ulDiiTimerId = 0;

	(void)xsvc_download_InitDdbListsFromDownloadInfo(pstDnItem);

	szPath = (HCHAR *)pstMsgAddPid->pszCachePath;

#ifdef __USE_LIBDSMCC__ /* by Dylan - libdsmcc */
	ENTER_CRITICAL_DNLDCNTS;
	if (NULL == pstDnItem->pstDsmccStatus)
	{
		/* memset(s_ucaChannel, 0x0, 256);
		pDbgFp = fopen("dbg.out","a+");	*/
		HxLOG_Print("[%s:%d] init libdsmcc\n", __FUNCTION__, __LINE__);
		pstDnItem->pstDsmccStatus = dsmcc_open( (const char *)s_ucaChannel, pDbgFp,
												   xsvc_download_WriteFileCallback_Hbbtv,				/* dsmcc_writefile_callback */
												   xsvc_download_WriteDirCallback_Hbbtv,				/* dsmcc_writedir_callback */
												   xsvc_download_WriteEventCallback_Hbbtv,				/* dsmcc_writeevt_callback */
												   xsvc_download_WriteEventDescriptionCallback_Hbbtv,	/* dsmcc_writedescriptorevt_callback */
												   xsvc_download_CompleteCallback_Hbbtv);				/* dsmcc_complete_callback */
	}
	LEAVE_CRITICAL_DNLDCNTS;
#endif

#if defined(CONFIG_DROP_ROOT_PRIVILEGES)
	if (HLIB_EXTRA_ChownWithParam(DSMCC_DEFAULT_CACHE_PATH, UID_obama, GID_settop) != ERR_OK)
	{
		HxLOG_Error("Fail to chown path = %s \n", DSMCC_DEFAULT_CACHE_PATH);
	}

	if (HLIB_EXTRA_Chmod(DSMCC_DEFAULT_CACHE_PATH, 0770) != ERR_OK)
	{
		HxLOG_Error("Fail to chmod path = %s \n", DSMCC_DEFAULT_CACHE_PATH);
	}
#endif

#if defined(CONFIG_3RD_WEB_BROWSER_OPERA)
	s_stPidBuffer[ulCnt].usPid = 0;
#else
	s_stPidBuffer[ulCnt].usPid = pstDnItem->usPid;
#endif
	if ((NULL != szPath) && ('\0' != szPath[0]))
	{
		HLIB_STD_StrUtf8NCpy (s_stPidBuffer[ulCnt].szStoragePath, szPath, FILE_PATH_NAME_LENGTH);
	}
	else
	{
		// OCTO 1.0의 기존 코드에 따른 Path Naming으로 만든다.
		HxSTD_PrintToStrN (s_stPidBuffer[ulCnt].szStoragePath, FILE_PATH_NAME_LENGTH, "/tmp/cache/%u/%u", ulCnt, s_stPidBuffer[ulCnt].usPid);
	}

	s_stPidBuffer[ulCnt].szStoragePath[FILE_PATH_NAME_LENGTH - 1] = '\0';

	HxLOG_Debug("(%u) - pid(%04X) path(%s)\n", ulCnt, s_stPidBuffer[ulCnt].usPid, s_stPidBuffer[ulCnt].szStoragePath);

	hResult = ERR_OK;

END_FUNC:
	HxLOG_Print("(-) : hResult = %d\n", __FUNCTION__, __LINE__, hResult);
	return hResult;
}
STATIC HERROR xsvc_download_SetDownloadContents_Hbbtv (SvcDownload_DsmccContents_t *pstContents, SvcDownload_Msg_t *pstMsg)
{
	HINT32				nDmxId		= 0;
	HUINT32				unActId		= 0;
	HERROR				hErr;
	HERROR				hRtnErr 	= ERR_DOWN_RESULT_OK;

	HxLOG_Print("[%s:%d] (+)\n", __FUNCTION__, __LINE__);

	if ( pstContents == NULL )
	{
		return ERR_DOWN_TARGET_NULL;
	}

	if ( pstMsg == NULL )
	{
		return ERR_DOWN_SOURCE_NULL;
	}

	unActId = PAL_PIPE_GetActionId (pstMsg->hActHandle);
	hErr = PAL_PIPE_GetResourceId (unActId, eRxRSCTYPE_DEMUX, &nDmxId);
	if (hErr != ERR_OK)
	{
		HxLOG_Error ("Error:\n");
		HxLOG_Print("[%s:%d] (+) PAL_PIPE_GetResourceId failed\n", __FUNCTION__, __LINE__);
		hRtnErr = ERR_DOWN_SOURCE_NULL;
	}
	else
	{
		pstContents->hAction		= pstMsg->hActHandle;
		pstContents->eActType		= (SvcDownload_ActionType_e)pstMsg->unArg.stStartDown.ulActType;
		pstContents->usTsUniqId 	= pstMsg->unArg.stStartDown.usTsUniqId;
		pstContents->ulDemuxId		= (HUINT32)nDmxId;

		HxLOG_Debug("ActHandle (%08x), ActId (%u)\n", pstMsg->hActHandle, unActId);
		HxLOG_Debug("ActType (%d)\n", pstContents->eActType);
		HxLOG_Debug("DMX ID (%d)\n", pstContents->ulDemuxId);
		HxLOG_Debug("TS ID (0x%04x)\n", pstContents->usTsUniqId);
	}

	HxLOG_Print("[%s:%d] (-) Ret:0x%08x\n", __FUNCTION__, __LINE__, hRtnErr);
	return hRtnErr;
}

#define _____SECTION_PARSE_____
STATIC HBOOL xsvc_download_CmpDiiCheckInfo (void *user_data,void *list_data)
{
	XsvcDownload_DiiInfo_t		*pstCheck = (XsvcDownload_DiiInfo_t *)user_data;
	XsvcDownload_DiiInfo_t		*pstItem = (XsvcDownload_DiiInfo_t *)list_data;

	if ((NULL != pstCheck) && (NULL != pstItem))
	{
		if ((pstCheck->ulDownloadId == pstItem->ulDownloadId) &&
			(pstCheck->ulCrc32 == pstItem->ulCrc32))
		{
			return TRUE;
		}
	}

	return FALSE;
}

STATIC void  xsvc_download_CbCarouselTimeout (unsigned long TimerId, void *params)
{
	HUINT16			 usPid = *((HUINT16 *)params);

	svc_download_SendSectionMessage(DOWNCB_CAROUSEL_TIMEOUT, s_pstContents->hAction, eSI_TABLE_CHANGED, (HUINT32)usPid);
}

STATIC void xsvc_download_ParseDsi_Hbbtv (HUINT8 *pucRaw, Handle_t hAction, HUINT32 ulFilterId)
{
	HUINT32 					 nCnt, ulCrc32;
	HINT32						 nSecLen;
	SvcDownload_DsmccContents_t *pstContents;
	XsvcDownload_DsmccDnInfo_t	*pstDnInfo_Filtered = NULL, *pstDnInfo_DsiParsed = NULL, *pstDnArray;
	DsmccDsiInfo_t				 stDsiInfo;
	HERROR						 hErr;

	nSecLen = (HINT32)get12bit (pucRaw + 1) + 3;
	ulCrc32 = (HUINT32)get32bit (pucRaw + nSecLen - 4);

	hErr = dsmcc_analyse_dsi(pucRaw, nSecLen, &stDsiInfo);
	if (0 != hErr)
	{
		HxLOG_Error("dsmcc_analyse_dsi err: wrong DSI?\n");
		return;
	}

	pstContents = NULL;

	ENTER_CRITICAL_DNLDCNTS;

	hErr = svc_download_GetContextData(hAction, (void **)(&pstContents));
	if (hErr != ERR_OK || pstContents == NULL)
	{
		HxLOG_Error("Context Data Failed\n");
		goto Error;
	}

	pstDnArray = xsvc_download_GetDownloadInfo_Hbbtv(pstContents);
	if (pstDnArray == NULL)
	{
		HxLOG_Error("Download Info Failed\n");
		goto Error;
	}

	for (nCnt = 0 ; nCnt < MAX_FILTERING_NUM ; nCnt++)
	{
		if ((pstDnArray[nCnt].usDsiFilterId == ulFilterId) && (stDsiInfo.biop_objloc_carouselid == pstDnArray[nCnt].uiCarouselId))
		{
			pstDnInfo_Filtered = &(pstDnArray[nCnt]);
			break;
		}
	}

	if ((NULL == pstDnInfo_Filtered) || (eFILTER_MODE_NONE == pstDnInfo_Filtered->eDsiMode))
	{
		HxLOG_Error ("Error:\n");
		goto Error;
	}

	// 제대로 된 DSI가 왔으면 그 Carousel 관련 DSI timer는 stop시켜야 한다.
	for (nCnt = 0 ; nCnt < MAX_FILTERING_NUM ; nCnt++)
	{
		if (pstDnInfo_Filtered->uiCarouselId == pstDnArray[nCnt].uiCarouselId)
		{
			if (0 != pstDnArray[nCnt].ulDsiTimerId)
			{
				VK_TIMER_Cancel(pstDnArray[nCnt].ulDsiTimerId);
				pstDnArray[nCnt].ulDsiTimerId = 0;
			}
		}
	}

	// DSI가 여러 곳에서 올 경우, 같은 Carousel ID일 경우에는 모아서 주도록 하자.
	for (nCnt = 0 ; nCnt < MAX_FILTERING_NUM ; nCnt++)
	{
		if ((TRUE == pstDnArray[nCnt].bDsiParsed) && (stDsiInfo.biop_objloc_carouselid == pstDnArray[nCnt].ulDsiCarouselId))
		{
			pstDnInfo_DsiParsed = &(pstDnArray[nCnt]);
			break;
		}
	}

	switch (pstDnInfo_Filtered->eDsiMode)
	{
		case eFILTER_MODE_RECEIVING:
			if (pstDnInfo_Filtered->bDsiParsed == TRUE)
			{
				goto Error;
			}

			// DSI가 여러 곳에서 올 경우, 같은 Carousel ID일 경우에는 모아서 주도록 하자.
			// 즉, 같은 Carousel ID로 온 게 전에 없을 때에만 DSI를 제대로 사용한다.
			if (NULL == pstDnInfo_DsiParsed)
			{
				unsigned long			 ulTimerId = 0;

#ifdef __USE_LIBDSMCC__ /* by Dylan - libdsmcc */
				//VK_SEM_Get(pstDnInfo_Filtered->uiSemaForResource );
				if (pstDnInfo_Filtered->pstDsmccStatus != NULL)
				{
					dsmcc_receive_fromdata(pstDnInfo_Filtered->pstDsmccStatus, pucRaw, nSecLen, pstDnInfo_Filtered->usPid);
				}
				//VK_SEM_Release(pstDnInfo_Filtered->uiSemaForResource);
#endif
				pstDnInfo_Filtered->bDsiParsed = TRUE;
				pstDnInfo_Filtered->usAssocTag = stDsiInfo.biop_connbinder_assoctag;
				pstDnInfo_Filtered->ulDsiCarouselId = stDsiInfo.biop_objloc_carouselid;


#if defined(WORKAROUND_LIGADA_MHEG_HBBTV_COEXISTENCE)
				pstDnInfo_Filtered->ulTimeoutValue = 15 * 1000;
#else
				if ((stDsiInfo.biop_connbinder_timeout >= (CAROUSEL_TIMEOUT_MIN * 1000)) &&
					(stDsiInfo.biop_connbinder_timeout <= (CAROUSEL_TIMEOUT_MAX * 1000)))
				{
					pstDnInfo_Filtered->ulTimeoutValue = stDsiInfo.biop_connbinder_timeout / 1000;
				}
				else
				{
					pstDnInfo_Filtered->ulTimeoutValue = CAROUSEL_TIMEOUT_DEFAULT;
				}
#endif
				if (0 != pstDnInfo_Filtered->ulFinishTimerId)
				{
					VK_TIMER_Cancel(pstDnInfo_Filtered->ulFinishTimerId);
					pstDnInfo_Filtered->ulFinishTimerId = 0;
				}

				ulTimerId = 0;
				hErr = VK_TIMER_EventAfter(pstDnInfo_Filtered->ulTimeoutValue, xsvc_download_CbCarouselTimeout, (void *)&(pstDnInfo_Filtered->usPid), sizeof(HUINT16), &ulTimerId);
				if ((VK_OK == hErr) && (0 != ulTimerId))
				{
					pstDnInfo_Filtered->ulFinishTimerId = ulTimerId;
				}

				if (0 != pstDnInfo_Filtered->ulDiiTimerId)
				{
					VK_TIMER_Cancel(pstDnInfo_Filtered->ulDiiTimerId);
					pstDnInfo_Filtered->ulDiiTimerId = 0;
				}

				ulTimerId = 0;
				hErr = VK_TIMER_EventAfter((10 * 1000), xsvc_download_CbTimerDii, (void *)&(pstDnInfo_Filtered->usPid), sizeof(HUINT16), &ulTimerId);
				if ((VK_OK == hErr) && (0 != ulTimerId))
				{
					pstDnInfo_Filtered->ulDiiTimerId = ulTimerId;
				}
			}

			pstDnInfo_Filtered->ulDsiCrc32 = ulCrc32;
			pstDnInfo_Filtered->eDsiMode = eFILTER_MODE_MONITORING;
			break;

		case eFILTER_MODE_MONITORING:
			if (pstDnInfo_Filtered->ulDsiCrc32 != ulCrc32)
			{
				pstDnInfo_Filtered->eDsiMode = eFILTER_MODE_NONE;
				pstDnInfo_Filtered->eDiiMode = eFILTER_MODE_NONE;
				svc_download_SendSectionMessage(DOWNCB_DSI_TABLE, hAction, eSI_TABLE_CHANGED, (HUINT32)pstDnInfo_Filtered->usDsiFilterId);
			}
			break;

		default:
			break;
	}

	HxLOG_Print("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

Error:
	LEAVE_CRITICAL_DNLDCNTS;

	return;
}

STATIC void xsvc_download_ParseDii_Hbbtv (HUINT8 *pucRaw, Handle_t hAction, HUINT32 ulFilterId)
{
 	HUINT32 					 i = 0, ulCrc32;
	HINT32						 nSecLen;
	HxList_t					*pstListItem = NULL;
	SvcDownload_DsmccContents_t *pstContents = NULL;
	XsvcDownload_DsmccDnInfo_t	*pstDnInfo_Dsi = NULL, *pstDnInfo_Filtered = NULL, *pstDnArray;
	DsmccDiiInfo_t				 stDiiInfo;
	XsvcDownload_DiiInfo_t		*pstDiiListItem = NULL, *pstSameDiiInfo = NULL;
	XsvcDownload_DiiInfo_t		 stCmpDiiListItem;		// Memory allocation 을 줄여 performance를 노린다. (용량도 작음)
	HERROR						 hErr;

	nSecLen = (HINT32)get12bit (pucRaw + 1) + 3;
	// ucVer = (pucRaw[5] & 0x3E) >> 1;
	ulCrc32 = (HUINT32)get32bit (pucRaw + nSecLen - 4);

	hErr = dsmcc_analyse_dii(pucRaw, nSecLen, &stDiiInfo);
	if (0 != hErr)
	{
		HxLOG_Error("dsmcc_analyse_dii err: wrong DII?\n");
		return;
	}

	ENTER_CRITICAL_DNLDCNTS;

	HxLOG_Print("[%s:%d] (+) FilterID(%d)\n", __FUNCTION__, __LINE__, ulFilterId);

	hErr = svc_download_GetContextData(hAction, (void **)(&pstContents));
	if (hErr != ERR_OK || pstContents == NULL)
	{
		HxLOG_Error("Context Data Failed\n");
		goto Error;
	}

	pstDnArray = xsvc_download_GetDownloadInfo_Hbbtv(pstContents);
	if (pstDnArray == NULL)
	{
		HxLOG_Error("Download Info Failed\n");
		goto Error;
	}

	//printf("[%s:%d]\n", __FUNCTION__, __LINE__);
	for (i = 0; i < MAX_FILTERING_NUM; i++)
	{
		if (pstDnArray[i].usDsiFilterId == ulFilterId)
		{
			pstDnInfo_Filtered = &(pstDnArray[i]);
			break;
		}
	}

	if ((NULL == pstDnInfo_Filtered) || (eFILTER_MODE_NONE == pstDnInfo_Filtered->eDiiMode))
	{
		HxLOG_Error ("Error:\n");
		goto Error;
	}

	for (i = 0; i < MAX_FILTERING_NUM; i++)
	{
		if (TRUE == pstDnArray[i].bDsiParsed)
		{
			if (stDiiInfo.download_id == pstDnArray[i].ulDsiCarouselId)
			{
				pstDnInfo_Dsi = &(pstDnArray[i]);
				break;
			}
		}
	}

	if (NULL == pstDnInfo_Dsi)
	{
		HxLOG_Error("Error: DII DownloadID(%d), not carousel ID\n", stDiiInfo.download_id);
		goto Error;
	}

	HxSTD_MemSet(&stCmpDiiListItem, 0, sizeof(XsvcDownload_DiiInfo_t));

	stCmpDiiListItem.ulDownloadId = stDiiInfo.download_id;
	stCmpDiiListItem.ulCrc32 = ulCrc32;
	stCmpDiiListItem.ulPid = (HUINT32)pstDnInfo_Filtered->usPid;
	stCmpDiiListItem.ulModuleNum = (HUINT32)stDiiInfo.num_module;
	stCmpDiiListItem.ulTotalModuleSize = (HUINT32)stDiiInfo.module_total_size;

	switch (pstDnInfo_Dsi->eDiiMode)
	{
	case eFILTER_MODE_RECEIVING:
		if (pstDnInfo_Dsi->bDsiParsed == FALSE)
		{
			goto Error;
		}

		// 제대로 된 DII가 왔으면 그 Carousel 관련 DII timer는 stop시켜야 한다.
		for (i = 0 ; i < MAX_FILTERING_NUM ; i++)
		{
			if (pstDnInfo_Filtered->uiCarouselId == pstDnArray[i].uiCarouselId)
			{
				if (0 != pstDnArray[i].ulDiiTimerId)
				{
					VK_TIMER_Cancel(pstDnArray[i].ulDiiTimerId);
					pstDnArray[i].ulDiiTimerId = 0;
				}
			}
		}

		pstListItem = HLIB_LIST_FindEx(pstDnInfo_Dsi->pstDiiList, (const void *)&stCmpDiiListItem, xsvc_download_CmpDiiCheckInfo);
		pstSameDiiInfo = (XsvcDownload_DiiInfo_t *)HLIB_LIST_Data(pstListItem);

		if (NULL != pstSameDiiInfo)
		{
			goto Error;
		}

#ifdef __USE_LIBDSMCC__ /* by Dylan - libdsmcc */
		if (pstDnInfo_Dsi->pstDsmccStatus != NULL)
		{
			dsmcc_receive_fromdata(pstDnInfo_Dsi->pstDsmccStatus, pucRaw, nSecLen, pstDnInfo_Dsi->usPid);
		}
#endif

		pstDnInfo_Dsi->bDiiParsed = TRUE;
		pstDnInfo_Dsi->ulDownloadId = stDiiInfo.download_id;

		if(pstDnInfo_Dsi->ulLastDiiTick ==0)
		{
			pstDnInfo_Dsi->ulLastDiiTick = HLIB_STD_GetSystemTick();
		}

		pstDiiListItem = (XsvcDownload_DiiInfo_t *)HLIB_STD_MemAlloc(sizeof(XsvcDownload_DiiInfo_t));
		if (NULL == pstDiiListItem)
		{
			HxLOG_Error("Memory Allocation Failed\n");
			goto Error;
		}

		HxSTD_MemCopy(pstDiiListItem, &stCmpDiiListItem, sizeof(XsvcDownload_DiiInfo_t));

		pstDnInfo_Dsi->pstDiiList = HLIB_LIST_Append(pstDnInfo_Dsi->pstDiiList, (void *)pstDiiListItem);
		pstDiiListItem = NULL;

		break;

	case eFILTER_MODE_MONITORING:
		if (HLIB_LIST_FindEx(pstDnInfo_Dsi->pstDiiList, (const void *)&stCmpDiiListItem, xsvc_download_CmpDiiCheckInfo) == NULL)
		{
			pstDnInfo_Filtered->eDsiMode = eFILTER_MODE_NONE;
			pstDnInfo_Filtered->eDiiMode = eFILTER_MODE_NONE;
			svc_download_SendSectionMessage(DOWNCB_DSI_TABLE, hAction, eSI_TABLE_CHANGED, (HUINT32)pstDnInfo_Dsi->usDsiFilterId);
		}
		break;

	default:
		break;
	}

	HxLOG_Print("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

Error:
	LEAVE_CRITICAL_DNLDCNTS;

	if (NULL != pstDiiListItem)				{ HLIB_STD_MemFree(pstDiiListItem); }

	return;
}

STATIC void xsvc_download_ParseDdb_Hbbtv (HUINT8 *pucRaw, Handle_t hAction, HUINT32 ulFilterId)
{
	HUINT32 					 i = 0;
	HINT32						 nSecLen;
	SvcDownload_DsmccContents_t *pstContents;
	XsvcDownload_DsmccDnInfo_t	*pstDnInfo_Dsi = NULL, *pstDnInfo_Filtered = NULL, *pstDnArray;
	DsmccDdbInfo_t				 stDdbInfo;
	DsmccDdbInfo_t				*pstDdbInfo = NULL;
	XsvcDownload_HbbtvEvtInfo_t *pstEvtInfo = NULL;
	HERROR						 hErr;

	pstContents = NULL;

	nSecLen = (HINT32)get12bit (pucRaw + 1) + 3;
	if (nSecLen < DDB_HEADER_LEN)
	{
		HxLOG_Error("Section Length Too Short: nSecLen(%d)\n", nSecLen);
		return;
	}

	hErr = dsmcc_analyse_ddb(pucRaw, nSecLen, &stDdbInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("dsmcc_analyse_ddb err: wrong DDB?\n");
		return;
	}

	ENTER_CRITICAL_DNLDCNTS;

	HxLOG_Print("[%s:%d] (+) FilterID(%d)\n", __FUNCTION__, __LINE__, ulFilterId);

	hErr = svc_download_GetContextData (hAction, (void **)(&pstContents));
	if (hErr != ERR_OK || pstContents == NULL)
	{
		//HxLOG_Error ("Error:\n");
		goto Error;
	}

	pstDnArray = xsvc_download_GetDownloadInfo_Hbbtv(pstContents);
	if (pstDnArray == NULL)
	{
		HxLOG_Error ("Error:\n");
		goto Error;
	}

	for (i = 0; i < MAX_FILTERING_NUM; i++)
	{
		if (pstDnArray[i].usDdbFilterId== ulFilterId)
		{
			pstDnInfo_Filtered = &(pstDnArray[i]);
			break;
		}
	}

	if ((NULL == pstDnInfo_Filtered) || (eFILTER_MODE_NONE == pstDnInfo_Filtered->eDiiMode))
	{
		//HxLOG_Error ("Error:\n");
		goto Error;
	}

	for (i = 0; i < MAX_FILTERING_NUM; i++)
	{
		// Check DSI processed:
		if ((TRUE == pstDnArray[i].bDsiParsed) &&
			(pstDnInfo_Filtered->uiCarouselId == pstDnArray[i].ulDsiCarouselId))
		{
			// Check DII processed:
			if ((TRUE == pstDnArray[i].bDiiParsed) &&
				(stDdbInfo.download_id == pstDnArray[i].ulDownloadId))
			{
				if ((0 != pstDnArray[i].ulLastDiiTick) &&
					((HLIB_STD_GetSystemTick() - pstDnArray[i].ulLastDiiTick) > (2 * 1000)))
				{
					pstDnInfo_Dsi = &(pstDnArray[i]);
				}

				break;
			}
		}
	}

	if (NULL == pstDnInfo_Dsi)
	{
		//HxLOG_Error ("Error:\n");
		goto Error;
	}

//	pstDdbInfo = xsvc_download_FindDdbInfoFromDownloadInfo(pstDnInfo_Dsi, (HUINT32)stDdbInfo.module_id);

	if (pstDnInfo_Dsi->bDdbAllReceived == TRUE)
	{
		if (NULL != pstDdbInfo)
		{
			if ((pstDdbInfo->module_version != stDdbInfo.module_version) && (NULL == pstEvtInfo))
			{
				pstDnInfo_Filtered->eDsiMode = eFILTER_MODE_NONE;
				pstDnInfo_Filtered->eDiiMode = eFILTER_MODE_NONE;
				svc_download_SendSectionMessage(DOWNCB_DSI_TABLE, hAction, eSI_TABLE_CHANGED, (HUINT32)pstDnInfo_Dsi->usDsiFilterId);
			}
		}

		goto Error;
	}

#if 0
	if (NULL == pstDdbInfo)
	{
		(void)xsvc_download_AppendDdbInfoFromDownloadInfo(pstDnInfo_Dsi, &stDdbInfo);
	}
	else
	{
		if (pstDdbInfo->module_version != stDdbInfo.module_version)
		{
			HxSTD_MemCopy(pstDdbInfo, &stDdbInfo, sizeof(DsmccDdbInfo_t));
		}
	}
#endif

	//printf(">>> DDB CRC(0x%08x)\n", 000);

#ifdef __USE_LIBDSMCC__ /* by Dylan - libdsmcc */
	//VK_SEM_Get(pstDnInfo_Dsi->uiSemaForResource);
	if (pstDnInfo_Dsi->pstDsmccStatus != NULL)
	{
		dsmcc_receive_fromdata(pstDnInfo_Dsi->pstDsmccStatus, pucRaw, nSecLen, pstDnInfo_Dsi->usPid);
	}
	//VK_SEM_Release(pstDnInfo_Dsi->uiSemaForResource);
#endif

	HxLOG_Print("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

Error:

	LEAVE_CRITICAL_DNLDCNTS;

	return;
}

STATIC void xsvc_download_ParseDesc_Hbbtv (HUINT8 *pucRaw, Handle_t hAction, HUINT32 ulFilterId)
{
	HUINT32							 nCnt;
	HINT32							 nSecLen;
	SvcDownload_DsmccContents_t		*pstContents = NULL;
	XsvcDownload_DsmccDnInfo_t		*pstDnInfo_Dsi = NULL, *pstDnInfo_Filtered = NULL, *pstDnArray;
	HERROR							 hErr;

    nSecLen = (HINT32)get12bit (pucRaw + 1) + 3;
	if (nSecLen < DDB_HEADER_LEN)
	{
		HxLOG_Error("Section Length Too Short: nSecLen(%d)\n", nSecLen);
		return;
	}

	// nEvtId = (HINT32)get16bit(pucRaw + 10);

	ENTER_CRITICAL_DNLDCNTS;
	HxLOG_Print("[%s:%d] (+)\n", __FUNCTION__, __LINE__, ulFilterId);

	hErr = svc_download_GetContextData(hAction, (void **)(&pstContents));
	if (hErr != ERR_OK || pstContents == NULL)
	{
		HxLOG_Error("Context Data Failed\n");
		goto Error;
	}

	pstDnArray = xsvc_download_GetDownloadInfo_Hbbtv (pstContents);
	if (pstDnArray == NULL)
	{
		HxLOG_Error("Download Info Failed\n");
		goto Error;
	}

	for (nCnt = 0 ; nCnt < MAX_FILTERING_NUM ; nCnt++)
	{
		if (pstDnArray[nCnt].usDescFilterId == ulFilterId)
		{
			pstDnInfo_Filtered = &(pstDnArray[nCnt]);
			break;
		}
	}

	if ((NULL == pstDnInfo_Filtered) || (eFILTER_MODE_NONE == pstDnInfo_Filtered->eDiiMode))
	{
		HxLOG_Error ("Error:\n");
		goto Error;
	}

	for (nCnt = 0 ; nCnt < MAX_FILTERING_NUM ; nCnt++)
	{
		// Check DSI processed:
		if ((TRUE == pstDnArray[nCnt].bDsiParsed) &&
			(pstDnInfo_Filtered->uiCarouselId == pstDnArray[nCnt].ulDsiCarouselId))
		{
			pstDnInfo_Dsi = &(pstDnArray[nCnt]);
		}
	}

	if (NULL == pstDnInfo_Dsi)
	{
		//HxLOG_Error ("Error:\n");
		goto Error;
	}

    if ( pstDnInfo_Dsi->ucLastDescSectionVersion != (pucRaw[5] & 0x3e) )
    {
        pstDnInfo_Dsi->pstEventDescList = HLIB_LIST_RemoveAllFunc(pstDnInfo_Dsi->pstEventDescList, xsvc_download_FreeDsmccEventInfo_Hbbtv);
        pstDnInfo_Dsi->ucLastDescSectionVersion = (pucRaw[5] & 0x3e);
    }

#ifdef __USE_LIBDSMCC__ /* by Dylan - libdsmcc */
	//VK_SEM_Get(pstDnInfo_Dsi->uiSemaForResource);
	if (pstDnInfo_Dsi->pstDsmccStatus != NULL)
	{
		dsmcc_receive_fromdata(pstDnInfo_Dsi->pstDsmccStatus, pucRaw, nSecLen, pstDnInfo_Dsi->usPid);
	}
	//VK_SEM_Release(pstDnInfo_Dsi->uiSemaForResource);
#endif

Error:
	HxLOG_Print("[%s:%d] (-)\n", __FUNCTION__, __LINE__);
	LEAVE_CRITICAL_DNLDCNTS;

	return;
}

#define _____LIBDSMCC_FUNCTIONS____

STATIC char s_szHbbtv_FileFullName[256];

STATIC HERROR xsvc_download_CorrectFilePathString_Hbbtv (HCHAR *szPath)
{
	HBOOL		 bSlashed = FALSE;
	HCHAR		*pucDst, *pucSrc;

	if (NULL == szPath)					{ return ERR_FAIL; }

	pucDst = pucSrc = szPath;

	for (pucDst = pucSrc = szPath; '\0' != *pucSrc; pucSrc++)
	{
		if ('/' == *pucSrc)
		{
			if (bSlashed == TRUE)					{ continue; }
			bSlashed = TRUE;
		}
		else
		{
			bSlashed = FALSE;
		}

		if (pucDst != pucSrc)
		{
			*pucDst = *pucSrc;
		}

 		pucDst++;
	}

	*pucDst = '\0';
	return ERR_OK;

}

STATIC int xsvc_download_WriteFileCallback_Hbbtv (char *szFileName, char *szFilePath, char *szChanName, int nCarouselId, int nPid, int nModuleId, int nFileSize, unsigned char *pucFileBuffer)
{
	HINT32		 nCount;
	HCHAR		*szDirPath = NULL;
	FILE		*pDataFile;

	HxLOG_Print("[%s:%d] (+)\n", __FUNCTION__, __LINE__);

	// Index를 PID Buffer에서 찾아낸다.
	if (pucFileBuffer == NULL || nFileSize < 0)
	{
		return ERR_FAIL;
	}

	if (szFileName == NULL || szFileName[0] == '\0')
	{
		return ERR_FAIL;
	}

	HxLOG_Debug("[%s][%s][%s](%d)\n", szFilePath, szFileName, szChanName, nFileSize);

	for (nCount = 0; nCount < MAX_FILTERING_NUM; nCount++)
	{
		if ((int)s_stPidBuffer[nCount].usPid == nPid)
		{
			szDirPath = s_stPidBuffer[nCount].szStoragePath;
			break;
		}
	}

	if (NULL == szDirPath)
	{
		szDirPath = s_stPidBuffer[0].szStoragePath;
	}

	if ('\0' == szDirPath[0])
	{
		HxLOG_Critical ("Something wrong in the Storage Path\n");
		return ERR_FAIL;
	}

	// Make the full file name
	HxSTD_PrintToStrN (s_szHbbtv_FileFullName, 256, "%s", szDirPath);

	if (HLIB_STD_StrEndWith((const HCHAR *)szDirPath, (const HCHAR *)"/") == FALSE)
	{
		HxSTD_StrNCat (s_szHbbtv_FileFullName, "/", 255);
	}

	HxLOG_Debug("Default Path [%s] >>>\n", s_szHbbtv_FileFullName);

	if (szFilePath != NULL && szFilePath[0] != '\0')
	{
		if (HxSTD_StrCmp((const HCHAR *)szFilePath, (const HCHAR *)"/"))
		{
			HCHAR *pstTmp, *pszNewFilePath;

			pszNewFilePath = pstTmp = szFilePath;
			while(pstTmp && *pstTmp != '\0')
			{
				if (HLIB_STD_StrStartWith((const HCHAR *)pstTmp, (const HCHAR *)"/"))
				{
					pszNewFilePath = pstTmp + 1;
					pstTmp++;
				}
				else
				{
					break;
				}
			}
			HxLOG_Debug("pszNewFilePath [%s] >>>\n", pszNewFilePath);

			HxSTD_StrNCat (s_szHbbtv_FileFullName, pszNewFilePath, 255);

			HxLOG_Debug("Default Path + File Path [%s] >>>\n", s_szHbbtv_FileFullName);
		}

		if (HLIB_STD_StrEndWith((const HCHAR *)s_szHbbtv_FileFullName, (const HCHAR *)"/") == FALSE)
		{
			HxSTD_StrNCat (s_szHbbtv_FileFullName, "/", 255);
			HxLOG_Debug("Add \"/\" at the end of string [%s] >>>\n", s_szHbbtv_FileFullName);
		}
	}

	HxSTD_StrNCat (s_szHbbtv_FileFullName, szFileName, 255);
	HxLOG_Debug("DSMCC Full File Name [%s] >>>\n", s_szHbbtv_FileFullName);

	xsvc_download_CorrectFilePathString_Hbbtv (s_szHbbtv_FileFullName);

	HxLOG_Debug("DSMCC Full File Name After Correction [%s] >>>\n", s_szHbbtv_FileFullName);

	pDataFile = fopen (s_szHbbtv_FileFullName, "wb");
	if (pDataFile != NULL)
	{
		if (nFileSize > 0)
		{
			fwrite (pucFileBuffer, 1, nFileSize, pDataFile);
		}
		fclose (pDataFile);
	}

	HxLOG_Print("[%s:%d] (-)\n", __FUNCTION__, __LINE__);
	return ERR_OK;
}

STATIC HUINT8 *xsvc_download_GetTextBufferForEvent (char *szFileName, char *szEvtPath, libdsmcc_event_t *pstEvent)
{
	HUINT32		 ulTotalLen = 0;

	// Event URL
	ulTotalLen += ((NULL != szFileName) ? HxSTD_StrLen(szFileName) + 2: 4);
	ulTotalLen += ((NULL != szEvtPath) ? HxSTD_StrLen(szEvtPath) + 2: 4);
	ulTotalLen += 4;

	if (NULL != pstEvent)
	{
		// Description
		if ((pstEvent->nDescriptionLen > 0) && (NULL != pstEvent->szDescription))
		{
			ulTotalLen += (pstEvent->nDescriptionLen + 2);
		}

		// Event Name
		if (NULL != pstEvent->szEventName)
		{
			ulTotalLen += (HxSTD_StrLen(pstEvent->szEventName) + 2);
		}

		// Object Info Byte
		if ((pstEvent->nObjInfoByteLen > 0) && (NULL != pstEvent->pucObjInfoByte))
		{
			ulTotalLen += (pstEvent->nObjInfoByteLen + 2);
		}
	}

	// Margin...
	ulTotalLen += 4;
	return HLIB_STD_MemCalloc(ulTotalLen);
}

STATIC int xsvc_download_EventConv_hbbtv (DxDSMCC_StreamEvent_t *pstDestEvt, HUINT8 *pucBuffer, char *szFileName, char *szEvtPath, libdsmcc_event_t *pstEvent, int nPid)
{
	HUINT32		 ulFileNameLen, ulEvtPathLen, ulFullUrlLen;
	HUINT8		*pucTemp = pucBuffer;

	if ((NULL == pstDestEvt) || (NULL == pucBuffer))					{ return ERR_FAIL; }

	// PID
	pstDestEvt->usPid = nPid;

	// Event URL
	ulFileNameLen = (NULL != szFileName) ? HxSTD_StrLen(szFileName) : 0;
	ulEvtPathLen = (NULL != szEvtPath) ? HxSTD_StrLen(szEvtPath) : 0;

	if (ulEvtPathLen == 0 && ulFileNameLen == 0)
	{
		pstDestEvt->pszUrl = pucTemp;
		pucTemp += 4;

		pstDestEvt->pszUrl[0] = '\0';
	}
	else if (ulFileNameLen == 0)
	{
		pstDestEvt->pszUrl = pucTemp;
		pucTemp += (ulEvtPathLen + 4);

		HxSTD_StrCpy(pstDestEvt->pszUrl, szEvtPath);
	}
	else if (ulEvtPathLen == 0)
	{
		pstDestEvt->pszUrl = pucTemp;
		pucTemp += (ulFileNameLen + 4);

		HxSTD_StrCpy(pstDestEvt->pszUrl, szFileName);
	}
	else
	{
		ulFullUrlLen = ulEvtPathLen + ulFileNameLen;
		pstDestEvt->pszUrl = pucTemp;
		pucTemp += (ulFullUrlLen + 4);

		if ('/' == szEvtPath[ulEvtPathLen - 1])
		{
			HxSTD_PrintToStrN(pstDestEvt->pszUrl, ulFullUrlLen + 4, "%s%s", szEvtPath, szFileName);
		}
		else
		{
			HxSTD_PrintToStrN(pstDestEvt->pszUrl, ulFullUrlLen + 4, "%s/%s", szEvtPath, szFileName);
		}
	}

	if (NULL != pstEvent)
	{
		// Event ID
		pstDestEvt->nEventId = pstEvent->nEvtId;

		// Association Tag
		pstDestEvt->usTag = pstEvent->nTag;

		// Description
		if ((pstEvent->nDescriptionLen > 0) && (NULL != pstEvent->szDescription))
		{
			pstDestEvt->szDescription = pucTemp;
			pucTemp += (pstEvent->nDescriptionLen + 2);

			HxSTD_MemCopy(pstDestEvt->szDescription, pstEvent->szDescription, pstEvent->nDescriptionLen);
			pstDestEvt->ulDescLen = HxSTD_StrLen(pstDestEvt->szDescription);
		}

		// Event Name
		if (NULL != pstEvent->szEventName)
		{
			HUINT32 	 ulEventNameLen = HxSTD_StrLen(pstEvent->szEventName);

			pstDestEvt->szEventName = pucTemp;
			pucTemp += (ulEventNameLen + 2);

			HxSTD_StrNCpy(pstDestEvt->szEventName, pstEvent->szEventName, ulEventNameLen);
			pstDestEvt->szEventName[ulEventNameLen] = '\0';
		}

		// Object Info Byte
		if ((pstEvent->nObjInfoByteLen > 0) && (NULL != pstEvent->pucObjInfoByte))
		{
			pstDestEvt->pucObjInfoByte = pucTemp;
			pucTemp += (pstEvent->nObjInfoByteLen + 2);

			HxSTD_MemCopy(pstDestEvt->pucObjInfoByte, pstEvent->pucObjInfoByte, pstEvent->nObjInfoByteLen);
			pstDestEvt->ulObjInfoByteLen = (HUINT32)pstEvent->nObjInfoByteLen;
		}
	}

	return ERR_OK;
}

STATIC HERROR xsvc_download_DataCheckForSerialize(DxDSMCC_StreamEvent_t *pstStreamEvent, HUINT32 *ulDataFmat)
{
	HUINT32	ulformat = eDxDSMCC_EventData_None;

	if( pstStreamEvent->pszUrl != NULL && HxSTD_StrLen(pstStreamEvent->pszUrl)>0 )
	{
		HxLOG_Debug("pszUrl: %s \n", pstStreamEvent->pszUrl);
		ulformat |= eDxDSMCC_EventData_Url;
	}
	if( pstStreamEvent->szEventName != NULL && HxSTD_StrLen(pstStreamEvent->szEventName)>0 )
	{
		HxLOG_Debug("szEventName: %s \n", pstStreamEvent->szEventName);
		ulformat |= eDxDSMCC_EventData_EventName;
	}
	if( pstStreamEvent->szDescription != NULL && HxSTD_StrLen(pstStreamEvent->szDescription)>0  && pstStreamEvent->ulDescLen > 0)
	{
		HxLOG_Debug("szDescription: %s \n", pstStreamEvent->szDescription);
		ulformat |= eDxDSMCC_EventData_Description;
	}
	if( pstStreamEvent->pucObjInfoByte != NULL && pstStreamEvent->ulObjInfoByteLen > 0)
	{
		HxLOG_Debug("pucObjInfoByte: %s \n", (HCHAR *)pstStreamEvent->pucObjInfoByte);
		ulformat |= eDxDSMCC_EventData_ObjectInfo;
	}

	HxLOG_Debug("\t ulDataformat   : 0x%08x\n", ulformat);

	*ulDataFmat = ulformat;

	return ERR_OK;
}

STATIC HERROR xsvc_download_SerializeData(DxDSMCC_StreamEvent_t *pstStreamEvent,HINT32 nSrzr, HUINT32 ulDataFmat)
{
	switch(ulDataFmat)
	{
		case 15: // url + eventname + description + objectinfo
			HLIB_SERIALIZER_MakeSerializedData(nSrzr, "iibibbibii",
											(HINT32)ulDataFmat,
											(HINT32)pstStreamEvent->usPid,
											pstStreamEvent->pszUrl,
											(HINT32)HxSTD_StrLen(pstStreamEvent->pszUrl) + 1,
											(HINT32)pstStreamEvent->ulDescLen,
											pstStreamEvent->szDescription,
											(HINT32)pstStreamEvent->ulDescLen,
											pstStreamEvent->szEventName,
											(HINT32)HxSTD_StrLen(pstStreamEvent->szEventName) + 1,
											(HINT32)pstStreamEvent->ulObjInfoByteLen,
											pstStreamEvent->pucObjInfoByte,
											(HINT32)pstStreamEvent->ulObjInfoByteLen + 1,
											(HINT32)pstStreamEvent->nEventId,
											(HINT32)pstStreamEvent->usTag);
			break;
		case 14: // eventname + description + objectinfo
			HLIB_SERIALIZER_MakeSerializedData(nSrzr, "iiibbibii",
											(HINT32)ulDataFmat,
											(HINT32)pstStreamEvent->usPid,
											(HINT32)pstStreamEvent->ulDescLen,
											pstStreamEvent->szDescription,
											(HINT32)pstStreamEvent->ulDescLen,
											pstStreamEvent->szEventName,
											(HINT32)HxSTD_StrLen(pstStreamEvent->szEventName) + 1,
											(HINT32)pstStreamEvent->ulObjInfoByteLen,
											pstStreamEvent->pucObjInfoByte,
											(HINT32)pstStreamEvent->ulObjInfoByteLen + 1,
											(HINT32)pstStreamEvent->nEventId,
											(HINT32)pstStreamEvent->usTag);
			break;
		case 13:// url + description + objectinfo
			HLIB_SERIALIZER_MakeSerializedData(nSrzr, "iibibibii",
											(HINT32)ulDataFmat,
											(HINT32)pstStreamEvent->usPid,
											pstStreamEvent->pszUrl,
											(HINT32)HxSTD_StrLen(pstStreamEvent->pszUrl) + 1,
											(HINT32)pstStreamEvent->ulDescLen,
											pstStreamEvent->szDescription,
											(HINT32)pstStreamEvent->ulDescLen,
											(HINT32)pstStreamEvent->ulObjInfoByteLen,
											pstStreamEvent->pucObjInfoByte,
											(HINT32)pstStreamEvent->ulObjInfoByteLen + 1,
											(HINT32)pstStreamEvent->nEventId,
											(HINT32)pstStreamEvent->usTag);
			break;
		case 12:// description + objectinfo
			HLIB_SERIALIZER_MakeSerializedData(nSrzr, "iiibibii",
											(HINT32)ulDataFmat,
											(HINT32)pstStreamEvent->usPid,
											(HINT32)pstStreamEvent->ulDescLen,
											pstStreamEvent->szDescription,
											(HINT32)pstStreamEvent->ulDescLen,
											(HINT32)pstStreamEvent->ulObjInfoByteLen,
											pstStreamEvent->pucObjInfoByte,
											(HINT32)pstStreamEvent->ulObjInfoByteLen + 1,
											(HINT32)pstStreamEvent->nEventId,
											(HINT32)pstStreamEvent->usTag);
			break;
		case 11:// url + eventname + objectinfo
			HLIB_SERIALIZER_MakeSerializedData(nSrzr, "iibibibii",
											(HINT32)ulDataFmat,
											(HINT32)pstStreamEvent->usPid,
											pstStreamEvent->pszUrl,
											(HINT32)HxSTD_StrLen(pstStreamEvent->pszUrl) + 1,
											(HINT32)pstStreamEvent->ulDescLen,
											pstStreamEvent->szEventName,
											(HINT32)HxSTD_StrLen(pstStreamEvent->szEventName) + 1,
											(HINT32)pstStreamEvent->ulObjInfoByteLen,
											pstStreamEvent->pucObjInfoByte,
											(HINT32)pstStreamEvent->ulObjInfoByteLen + 1,
											(HINT32)pstStreamEvent->nEventId,
											(HINT32)pstStreamEvent->usTag);
			break;

		case 10:// eventname + objectinfo
			HLIB_SERIALIZER_MakeSerializedData(nSrzr, "iiibibii",
											(HINT32)ulDataFmat,
											(HINT32)pstStreamEvent->usPid,
											(HINT32)pstStreamEvent->ulDescLen,
											pstStreamEvent->szEventName,
											(HINT32)HxSTD_StrLen(pstStreamEvent->szEventName) + 1,
											(HINT32)pstStreamEvent->ulObjInfoByteLen,
											pstStreamEvent->pucObjInfoByte,
											(HINT32)pstStreamEvent->ulObjInfoByteLen + 1,
											(HINT32)pstStreamEvent->nEventId,
											(HINT32)pstStreamEvent->usTag);
			break;

		case 9:// url + objectinfo
			HLIB_SERIALIZER_MakeSerializedData(nSrzr, "iibiibii",
											(HINT32)ulDataFmat,
											(HINT32)pstStreamEvent->usPid,
											pstStreamEvent->pszUrl,
											(HINT32)HxSTD_StrLen(pstStreamEvent->pszUrl) + 1,
											(HINT32)pstStreamEvent->ulDescLen,
											(HINT32)pstStreamEvent->ulObjInfoByteLen,
											pstStreamEvent->pucObjInfoByte,
											(HINT32)pstStreamEvent->ulObjInfoByteLen + 1,
											(HINT32)pstStreamEvent->nEventId,
											(HINT32)pstStreamEvent->usTag);
			break;

		case 8:// objectinfo
			HLIB_SERIALIZER_MakeSerializedData(nSrzr, "iiiibii",
											(HINT32)ulDataFmat,
											(HINT32)pstStreamEvent->usPid,
											(HINT32)pstStreamEvent->ulDescLen,
											(HINT32)pstStreamEvent->ulObjInfoByteLen,
											pstStreamEvent->pucObjInfoByte,
											(HINT32)pstStreamEvent->ulObjInfoByteLen + 1,
											(HINT32)pstStreamEvent->nEventId,
											(HINT32)pstStreamEvent->usTag);
			break;

		case 7:// url + eventname + description
			HLIB_SERIALIZER_MakeSerializedData(nSrzr, "iibibbiii",
											(HINT32)ulDataFmat,
											(HINT32)pstStreamEvent->usPid,
											pstStreamEvent->pszUrl,
											(HINT32)HxSTD_StrLen(pstStreamEvent->pszUrl) + 1,
											(HINT32)pstStreamEvent->ulDescLen,
											pstStreamEvent->szDescription,
											(HINT32)pstStreamEvent->ulDescLen,
											pstStreamEvent->szEventName,
											(HINT32)HxSTD_StrLen(pstStreamEvent->szEventName) + 1,
											(HINT32)pstStreamEvent->ulObjInfoByteLen,
											(HINT32)pstStreamEvent->nEventId,
											(HINT32)pstStreamEvent->usTag);
			break;

		case 6:// eventname + description
			HLIB_SERIALIZER_MakeSerializedData(nSrzr, "iiibbiii",
											(HINT32)ulDataFmat,
											(HINT32)pstStreamEvent->usPid,
											(HINT32)pstStreamEvent->ulDescLen,
											pstStreamEvent->szDescription,
											(HINT32)pstStreamEvent->ulDescLen,
											pstStreamEvent->szEventName,
											(HINT32)HxSTD_StrLen(pstStreamEvent->szEventName) + 1,
											(HINT32)pstStreamEvent->ulObjInfoByteLen,
											(HINT32)pstStreamEvent->nEventId,
											(HINT32)pstStreamEvent->usTag);
			break;

		case 5:// url + description
			HLIB_SERIALIZER_MakeSerializedData(nSrzr, "iibibiii",
											(HINT32)ulDataFmat,
											(HINT32)pstStreamEvent->usPid,
											pstStreamEvent->pszUrl,
											(HINT32)HxSTD_StrLen(pstStreamEvent->pszUrl) + 1,
											(HINT32)pstStreamEvent->ulDescLen,
											pstStreamEvent->szDescription,
											(HINT32)pstStreamEvent->ulDescLen,
											(HINT32)pstStreamEvent->ulObjInfoByteLen,
											(HINT32)pstStreamEvent->nEventId,
											(HINT32)pstStreamEvent->usTag);
			break;

		case 4://  description
			HLIB_SERIALIZER_MakeSerializedData(nSrzr, "iiibiii",
											(HINT32)ulDataFmat,
											(HINT32)pstStreamEvent->usPid,
											(HINT32)pstStreamEvent->ulDescLen,
											pstStreamEvent->szDescription,
											(HINT32)pstStreamEvent->ulDescLen,
											(HINT32)pstStreamEvent->ulObjInfoByteLen,
											(HINT32)pstStreamEvent->nEventId,
											(HINT32)pstStreamEvent->usTag);
			break;

		case 3:// url + eventname
			HLIB_SERIALIZER_MakeSerializedData(nSrzr, "iibibiii",
											(HINT32)ulDataFmat,
											(HINT32)pstStreamEvent->usPid,
											pstStreamEvent->pszUrl,
											(HINT32)HxSTD_StrLen(pstStreamEvent->pszUrl) + 1,
											(HINT32)pstStreamEvent->ulDescLen,
											pstStreamEvent->szEventName,
											(HINT32)HxSTD_StrLen(pstStreamEvent->szEventName) + 1,
											(HINT32)pstStreamEvent->ulObjInfoByteLen,
											(HINT32)pstStreamEvent->nEventId,
											(HINT32)pstStreamEvent->usTag);
			break;

		case 2:// eventname
			HLIB_SERIALIZER_MakeSerializedData(nSrzr, "iiibiii",
											(HINT32)ulDataFmat,
											(HINT32)pstStreamEvent->usPid,
											(HINT32)pstStreamEvent->ulDescLen,
											pstStreamEvent->szEventName,
											(HINT32)HxSTD_StrLen(pstStreamEvent->szEventName) + 1,
											(HINT32)pstStreamEvent->ulObjInfoByteLen,
											(HINT32)pstStreamEvent->nEventId,
											(HINT32)pstStreamEvent->usTag);
			break;

		case 1:// url
			HLIB_SERIALIZER_MakeSerializedData(nSrzr, "iibiiii",
											(HINT32)ulDataFmat,
											(HINT32)pstStreamEvent->usPid,
											pstStreamEvent->pszUrl,
											(HINT32)HxSTD_StrLen(pstStreamEvent->pszUrl) + 1,
											(HINT32)pstStreamEvent->ulDescLen,
											(HINT32)pstStreamEvent->ulObjInfoByteLen,
											(HINT32)pstStreamEvent->nEventId,
											(HINT32)pstStreamEvent->usTag);
			break;

		case 0:
			HLIB_SERIALIZER_MakeSerializedData(nSrzr, "iiiiii",
											(HINT32)ulDataFmat,
											(HINT32)pstStreamEvent->usPid,
											(HINT32)pstStreamEvent->ulDescLen,
											(HINT32)pstStreamEvent->ulObjInfoByteLen,
											(HINT32)pstStreamEvent->nEventId,
											(HINT32)pstStreamEvent->usTag);
			break;

	}

	return ERR_OK;
}

// TS 102 809 : B.2.4.3.5 Encoding of table id extension
#define	CHECK_DO_IT_NOW_EVNET(event_id)		(((0x0000FFFF&event_id)>=0x0001)&&((0x0000FFFF&event_id)<=0x3FFF))

STATIC HBOOL xsvc_download_CmpBinary (HUINT8 *pucData1, HUINT8 ulDataLen1, HUINT8 *pucData2, HUINT8 ulDataLen2)
{
	if ((NULL == pucData1) && (NULL == pucData2))
	{
		return TRUE;
	}
	else if ((NULL != pucData1) && (NULL != pucData2))
	{
		if ((ulDataLen1 > 0) && (ulDataLen1 == ulDataLen2))
		{
			if (HxSTD_MemCmp(pucData1, pucData2, ulDataLen1) == 0)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

STATIC HBOOL xsvc_download_CmpString (HCHAR *szStr1, HCHAR *szStr2)
{
	if ((NULL == szStr1) && (NULL == szStr2))
	{
		return TRUE;
	}
	else if ((NULL != szStr1) && (NULL != szStr2))
	{
		if (HxSTD_StrCmp(szStr1, szStr2) == 0)
		{
			return TRUE;
		}
	}

	return FALSE;
}

STATIC HBOOL xsvc_download_CmpDsmccEventWithoutEvtObjInfo (DxDSMCC_StreamEvent_t *pstEvt1, DxDSMCC_StreamEvent_t *pstEvt2)
{
	if ((NULL == pstEvt1) || (NULL == pstEvt2))					{ return FALSE; }

	if (pstEvt1->usPid != pstEvt2->usPid)
	{
		return FALSE;
	}

	if (xsvc_download_CmpString(pstEvt1->pszUrl, pstEvt2->pszUrl) == FALSE)
	{
		return FALSE;
	}

	if (xsvc_download_CmpBinary(pstEvt1->szDescription, pstEvt1->ulDescLen, pstEvt2->szDescription, pstEvt2->ulDescLen) == FALSE)
	{
		return FALSE;
	}

	if (xsvc_download_CmpString(pstEvt1->szEventName, pstEvt2->szEventName) == FALSE)
	{
		return FALSE;
	}

	if (pstEvt1->nEventId != pstEvt2->nEventId)
	{
		return FALSE;
	}


	if (pstEvt1->usTag != pstEvt2->usTag)
	{
		return FALSE;
	}

	return TRUE;
}

STATIC HERROR xsvc_download_DupDsmccEvent (DxDSMCC_StreamEvent_t *pstDstEvt, DxDSMCC_StreamEvent_t *pstSrcEvt)
{
	HUINT32			 ulTxtLen = 0;

	if ((NULL == pstDstEvt) || (NULL == pstSrcEvt))								{ return ERR_FAIL; }

	HxSTD_MemSet(pstDstEvt, 0, sizeof(DxDSMCC_StreamEvent_t));


	pstDstEvt->usPid = pstSrcEvt->usPid;
	if (NULL != pstSrcEvt->pszUrl)
	{
		ulTxtLen = HxSTD_StrLen(pstSrcEvt->pszUrl);
		pstDstEvt->pszUrl = (HCHAR *)HLIB_STD_MemAlloc(ulTxtLen + 1);
		if (NULL == pstDstEvt->pszUrl)
		{
			HxLOG_Error("Memory Allocation in pstDstEvt->pszUrl\n");
			goto ERROR;
		}

		HxSTD_StrNCpy(pstDstEvt->pszUrl, pstSrcEvt->pszUrl, ulTxtLen);
		pstDstEvt->pszUrl[ulTxtLen] = '\0';
	}

	// Description
	if ((pstSrcEvt->ulDescLen > 0) && (NULL != pstSrcEvt->szDescription))
	{
		pstDstEvt->szDescription = (HUINT8 *)HLIB_STD_MemAlloc(pstSrcEvt->ulDescLen + 1);
		if (NULL == pstDstEvt->szDescription)
		{
			HxLOG_Error("Memory Allocation in pstDstEvt->szDescription\n");
			goto ERROR;
		}

		pstDstEvt->ulDescLen = pstSrcEvt->ulDescLen;
		HxSTD_MemCopy(pstDstEvt->szDescription, pstSrcEvt->szDescription, pstSrcEvt->ulDescLen);
		pstDstEvt->szDescription[pstSrcEvt->ulDescLen] = '\0';
	}

	// Event Name
	if (NULL != pstSrcEvt->szEventName)
	{
		ulTxtLen = HxSTD_StrLen(pstSrcEvt->szEventName);
		pstDstEvt->szEventName = (HUINT8 *)HLIB_STD_MemAlloc(ulTxtLen + 1);
		if (NULL == pstDstEvt->szEventName)
		{
			HxLOG_Error("Memory Allocation in pstDstEvt->szEventName\n");
			goto ERROR;
		}

		HxSTD_StrNCpy(pstDstEvt->szEventName, pstSrcEvt->szEventName, ulTxtLen);
		pstDstEvt->szEventName[ulTxtLen] = '\0';
	}

	// Object Info Byte
	if ((pstSrcEvt->ulObjInfoByteLen > 0) && (NULL != pstSrcEvt->pucObjInfoByte))
	{
		pstDstEvt->pucObjInfoByte = (HUINT8 *)HLIB_STD_MemAlloc(pstSrcEvt->ulObjInfoByteLen + 1);
		if (NULL == pstDstEvt->pucObjInfoByte)
		{
			HxLOG_Error("Memory Allocation in pstDstEvt->pucObjInfoByte\n");
			goto ERROR;
		}

		pstDstEvt->ulObjInfoByteLen = pstSrcEvt->ulObjInfoByteLen;
		HxSTD_MemCopy(pstDstEvt->pucObjInfoByte, pstSrcEvt->pucObjInfoByte, pstSrcEvt->ulObjInfoByteLen);
		pstDstEvt->pucObjInfoByte[pstSrcEvt->ulObjInfoByteLen] = '\0';
	}

	// Event ID
	pstDstEvt->nEventId = pstSrcEvt->nEventId;

	// Association TAG
	pstDstEvt->usTag = pstSrcEvt->usTag;

	return ERR_OK;

ERROR:
	xsvc_download_FreeDsmccEventInsideOnly_Hbbtv((void *)pstDstEvt);
	return ERR_FAIL;
}

STATIC HERROR xsvc_download_UpdateObjInfoByte (HUINT32 *pulDestObjInfoLen, HUINT8 **ppucDestObjInfoByte, HUINT32 ulSrcObjInfoLen, HUINT8 *pucSrcObjInfoByte)
{
	if ((NULL == pulDestObjInfoLen) || (NULL == ppucDestObjInfoByte))			{ return ERR_FAIL; }

	*pulDestObjInfoLen = 0;
	if (NULL != *ppucDestObjInfoByte)
	{
		HLIB_STD_MemFree(*ppucDestObjInfoByte);
		*ppucDestObjInfoByte = NULL;
	}

	if ((ulSrcObjInfoLen > 0) && (NULL != pucSrcObjInfoByte))
	{
		HUINT8			*pucDupData = (HUINT8 *)HLIB_STD_MemAlloc(ulSrcObjInfoLen + 2);

		if (NULL != pucDupData)
		{
			HxSTD_MemCopy(pucDupData, pucSrcObjInfoByte, ulSrcObjInfoLen);
			pucDupData[ulSrcObjInfoLen] = '\0';

			*ppucDestObjInfoByte = pucDupData;
			*pulDestObjInfoLen = ulSrcObjInfoLen;
		}
	}

	return ERR_OK;
}

STATIC HERROR xsvc_download_GetHighPriorObjectInfoByte (XsvcDownload_HbbtvEvtInfo_t *pstEvtInfo, HUINT32 *pulDestObjInfoLen, HUINT8 **ppucDestObjInfoByte)
{
	if ((NULL == pstEvtInfo) || (NULL == pulDestObjInfoLen) || (NULL == ppucDestObjInfoByte))
	{
		return ERR_FAIL;
	}

	// The Object Info Bytes from the event has higher priority.
	if ((0 != pstEvtInfo->ulObjInfoFromEvt) && (NULL != pstEvtInfo->pucObjInfoFromEvt))
	{
		*pulDestObjInfoLen = pstEvtInfo->ulObjInfoFromEvt;
		*ppucDestObjInfoByte = pstEvtInfo->pucObjInfoFromEvt;
	}
	else
	{
		*pulDestObjInfoLen = pstEvtInfo->ulObjInfoFromDesc;
		*ppucDestObjInfoByte = pstEvtInfo->pucObjInfoFromDesc;
	}

	return ERR_OK;
}

STATIC HERROR xsvc_download_InformHbbtvEventFound (SvcDownload_DsmccContents_t *pstContents, XsvcDownload_DsmccDnInfo_t *pstDnInfo, XsvcDownload_HbbtvEvtInfo_t *pstEvtInfo, HBOOL bForced)
{
	HINT32			 nSrzr = 0;
	HUINT32			 ulSize = 0;
	HUINT32			 ulDataFmat;
	void			*pvData = NULL, *pvSendData = NULL;
	HERROR			 hResult = ERR_FAIL;

    nSrzr = HLIB_SERIALIZER_Open();
	if (0 == nSrzr)
	{
		HxLOG_Error("HLIB_SERIALIZER_Open err:\n");
		goto END_FUNC;
	}

	(void)xsvc_download_DataCheckForSerialize(&(pstEvtInfo->stEvent), &ulDataFmat);
	(void)xsvc_download_SerializeData(&(pstEvtInfo->stEvent), nSrzr, ulDataFmat);
	pvData = HLIB_SERIALIZER_Pack(nSrzr, &ulSize);
	if ((NULL == pvData) && (0 == ulSize))
	{
		HxLOG_Error("HLIB_SERIALIZER_Pack err:\n");
		goto END_FUNC;
	}

	pvSendData = HLIB_STD_MemDup(pvData, ulSize);
	if (NULL == pvSendData)
	{
		HxLOG_Error("HLIB_STD_MemDup err: ulSize(%d)\n", ulSize);
		goto END_FUNC;
	}

	(void)svc_download_UtilPostDataToMgr(eSEVT_DOWN_DSMCC_EVENT_FOUND, pstContents->hAction, (HUINT32)eDOWN_DOWNLOAD_HBBTV, ulSize, (void*)pvSendData, ulSize);

	HLIB_SERIALIZER_Close(nSrzr);

	nSrzr = 0;
	pvSendData = NULL;
	hResult = ERR_OK;

END_FUNC:
	if (0 != nSrzr)					{ HLIB_SERIALIZER_Close(nSrzr); }
	if (NULL != pvSendData)			{ HLIB_STD_MemFree(pvSendData); }

	return hResult;
}

STATIC HERROR xsvc_download_InformHbbtvEventDescriptionFound (SvcDownload_DsmccContents_t *pstContents, XsvcDownload_DsmccDnInfo_t *pstDnInfo, XsvcDownload_HbbtvEvtInfo_t *pstEvtInfo)
{
	HINT32			 nSrzr = 0;
	HUINT32			 ulSize = 0;
//	HUINT32			 ulCurrTick = HLIB_STD_GetSystemTick();
	HUINT32			 ulDataFmat;
	void			*pvData = NULL, *pvSendData = NULL;
	HERROR			 hResult = ERR_FAIL;

	nSrzr = HLIB_SERIALIZER_Open();
	if (0 == nSrzr)
	{
		HxLOG_Error("HLIB_SERIALIZER_Open err:\n");
		goto END_FUNC;
	}

	(void)xsvc_download_DataCheckForSerialize(&(pstEvtInfo->stEvent), &ulDataFmat);
	(void)xsvc_download_SerializeData(&(pstEvtInfo->stEvent), nSrzr, ulDataFmat);
	pvData = HLIB_SERIALIZER_Pack(nSrzr, &ulSize);
	if ((NULL == pvData) && (0 == ulSize))
	{
		HxLOG_Error("HLIB_SERIALIZER_Pack err:\n");
		goto END_FUNC;
	}

	pvSendData = HLIB_STD_MemDup(pvData, ulSize);
	if (NULL == pvSendData)
	{
		HxLOG_Error("HLIB_STD_MemDup err: ulSize(%d)\n", ulSize);
		goto END_FUNC;
	}

	(void)svc_download_UtilPostDataToMgr(eSEVT_DOWN_DSMCC_STREAM_DESCRIPTOR_EVENT, pstContents->hAction, (HUINT32)eDOWN_DOWNLOAD_HBBTV, ulSize, (void*)pvSendData, ulSize);

	HLIB_SERIALIZER_Close(nSrzr);

	nSrzr = 0;
	pvSendData = NULL;
	hResult = ERR_OK;

END_FUNC:
	if (0 != nSrzr)					{ HLIB_SERIALIZER_Close(nSrzr); }
	if (NULL != pvSendData)			{ HLIB_STD_MemFree(pvSendData); }

	return hResult;
}


STATIC int xsvc_download_WriteEventCallback_Hbbtv (char *szEvtFileName, char *szEvtPath, libdsmcc_event_t *pstEvent, char *szChanName, int nCarouselId, int nPid, int nModuleId)
{
	HBOOL						 bSameObjInfo = FALSE, bEventNoChange = TRUE;
	HINT32						 nCnt;
	HUINT8						*pucTempBuffer = NULL;
	SvcDownload_DsmccContents_t	*pstContents;
	XsvcDownload_DsmccDnInfo_t	*pstDnInfo_Dsi = NULL, *pstDnArray;
	XsvcDownload_HbbtvEvtInfo_t	*pstEvtInfo = NULL;
 	DxDSMCC_StreamEvent_t		 stRcvEvent;
	HERROR						 hErr, hResult = ERR_FAIL;

	HxLOG_Print("[%s:%d] (+)\n", __FUNCTION__, __LINE__);

	pstContents = s_pstContents;

	if (NULL == szEvtFileName)
	{
		HxLOG_Error("[%s:%04d] NULL == szEvtFileName\n");
		goto END_FUNC;
	}

	if (NULL == pstEvent)
	{
		HxLOG_Error("[%s:%04d] NULL == pstEvent\n");
		goto END_FUNC;
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Debug("[%s] Stream Event \n", __FUNCTION__);
	HxLOG_Debug("[%s] ====================================================\n", __FUNCTION__);
	HxLOG_Debug("[%s] PID [0x%04x] \n", __FUNCTION__, nPid);
	HxLOG_Debug("[%s] szEvtFileName================%s\n", __FUNCTION__, (char *)(szEvtFileName ? szEvtFileName : "[Null]"));
	HxLOG_Debug("[%s] szEvtPath====================%s\n", __FUNCTION__, (char *)(szEvtPath ? szEvtPath : "[Null]"));
	HxLOG_Debug("[%s] pstEvent->szEventName========%s\n", __FUNCTION__, ((NULL != pstEvent->szEventName) ? (char *)(pstEvent->szEventName) : "[Null]"));
	HxLOG_Debug("[%s] pstEvent->nEvtId=============%#06x\n", __FUNCTION__, pstEvent->nEvtId);
	HxLOG_Debug("[%s] pstEvent->szDescription======%s\n", __FUNCTION__, ((NULL != pstEvent->szDescription) ? (char *)pstEvent->szDescription : "[Null]"));
	HxLOG_Debug("[%s] pstEvent->nObjInfoByteLen====%d\n", __FUNCTION__, pstEvent->nObjInfoByteLen);
	if (pstEvent->nObjInfoByteLen && pstEvent->pucObjInfoByte)
	{
		//HLIB_LOG_Dump(pstEvent->pucObjInfoByte, pstEvent->nObjInfoByteLen, 0, FALSE);
	}
	HxLOG_Debug("[%s] pstEvent->nTag===============%#x\n", __FUNCTION__, pstEvent->nTag);
	HxLOG_Debug("[%s] ====================================================\n", __FUNCTION__);
#endif

	if (FALSE == CHECK_DO_IT_NOW_EVNET(pstEvent->nEvtId))
	{
		HxLOG_Error("pstEvent->nEvtId [0x%x] is Scheduled Event\n", pstEvent->nEvtId);
		goto END_FUNC;
	}

	pucTempBuffer = xsvc_download_GetTextBufferForEvent(szEvtFileName, szEvtPath, pstEvent);
	if (NULL == pucTempBuffer)
	{
		HxLOG_Error("xsvc_download_GetTextBufferForEvent NULL\n");
		goto END_FUNC;
	}

	pstDnArray = xsvc_download_GetDownloadInfo_Hbbtv(pstContents);
	if (pstDnArray == NULL)
	{
		HxLOG_Error("Download Info Failed\n");
		goto END_FUNC;
	}

	for (nCnt = 0 ; nCnt < MAX_FILTERING_NUM ; nCnt++)
	{
		if (pstDnArray[nCnt].usPid == (HUINT16)nPid)
		{
			// libdsmcc module에 넣을 때 DSI 를 받은 곳에 넣어 주었다
			pstDnInfo_Dsi = &(pstDnArray[nCnt]);
			break;
		}
	}

	if (NULL == pstDnInfo_Dsi)
	{
		HxLOG_Error("Download Info Failed\n");
		goto END_FUNC;
	}

	HxSTD_MemSet(&stRcvEvent, 0, sizeof(DxDSMCC_StreamEvent_t));
	hErr = xsvc_download_EventConv_hbbtv(&stRcvEvent, pucTempBuffer, szEvtFileName, szEvtPath, pstEvent, nCarouselId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("xsvc_download_EventConv_hbbtv err: szEvtFileName[%s], szEvtPath[%s], nCarouselId[%d]\n", szEvtFileName, szEvtPath, nCarouselId);
		goto END_FUNC;
	}

	xsvc_download_FindEventInfoByName_Hbbtv(pstDnInfo_Dsi, (HUINT8 *)pstEvent->szEventName, &pstEvtInfo);
	if (pstEvtInfo)
	{
		// If the corresponding event description is downloaded, and there is no object info bytes in the event,
		// then the object infor bytes shall be sustained.
		bEventNoChange = xsvc_download_CmpDsmccEventWithoutEvtObjInfo(&stRcvEvent, &(pstEvtInfo->stEvent));
	}
	else
	{
		pstEvtInfo = (XsvcDownload_HbbtvEvtInfo_t *)HLIB_STD_MemCalloc(sizeof(XsvcDownload_HbbtvEvtInfo_t));
		if (NULL == pstEvtInfo)
		{
			HxLOG_Error("Memory allocation failed: XsvcDownload_HbbtvEvtInfo_t pstEvtInfo\n");
			goto END_FUNC;
		}

		bEventNoChange = FALSE;

		pstEvtInfo->ulReceivedTick	= HLIB_STD_GetSystemTick();
		pstEvtInfo->ulCarouselId	= (HUINT32)nCarouselId;
		pstEvtInfo->ulModuleId		= (HUINT32)nModuleId;

		hErr = xsvc_download_DupDsmccEvent(&(pstEvtInfo->stEvent), &stRcvEvent);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("xsvc_download_DupDsmccEvent err:\n");
		}

		hErr = xsvc_download_AppendEvtInfo_Hbbtv(pstDnInfo_Dsi, pstEvtInfo);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("xsvc_download_AppendEvtInfo_Hbbtv err:\n");
		}
	}

	// Now check the object info byte from the event is changed and update it.
	bSameObjInfo = xsvc_download_CmpBinary(stRcvEvent.pucObjInfoByte, stRcvEvent.ulObjInfoByteLen, pstEvtInfo->pucObjInfoFromEvt, pstEvtInfo->ulObjInfoFromEvt);
	if (TRUE != bSameObjInfo)
	{
		HUINT32					 ulObjInfoByteLen = 0;
		HUINT8					*pucObjInfoByte = NULL;

		(void)xsvc_download_UpdateObjInfoByte(&(pstEvtInfo->ulObjInfoFromEvt), &(pstEvtInfo->pucObjInfoFromEvt), stRcvEvent.ulObjInfoByteLen, stRcvEvent.pucObjInfoByte);
		(void)xsvc_download_GetHighPriorObjectInfoByte(pstEvtInfo, &ulObjInfoByteLen, &pucObjInfoByte);

		bSameObjInfo = xsvc_download_CmpBinary(pucObjInfoByte, ulObjInfoByteLen, pstEvtInfo->stEvent.pucObjInfoByte, pstEvtInfo->stEvent.ulObjInfoByteLen);
		if (TRUE != bSameObjInfo)
		{
			(void)xsvc_download_UpdateObjInfoByte(&(pstEvtInfo->stEvent.ulObjInfoByteLen), &(pstEvtInfo->stEvent.pucObjInfoByte), ulObjInfoByteLen, pucObjInfoByte);
			bEventNoChange = FALSE;
		}
	}

	if (TRUE != bEventNoChange)
	{
		(void)xsvc_download_InformHbbtvEventFound(pstContents, pstDnInfo_Dsi, pstEvtInfo, TRUE);
	}

	HxLOG_Print("[%s:%d] (-)\n", __FUNCTION__, __LINE__);
	hResult = ERR_OK;

END_FUNC:
	if (NULL != pucTempBuffer)								{ HLIB_STD_MemFree(pucTempBuffer); }

	HxLOG_Print("[%s:%d] (-)\n", __FUNCTION__, __LINE__);
	return hResult;
}

STATIC int xsvc_download_SetEventInfoByte_Hbbtv(DxDSMCC_StreamEvent_t *pstDsmccEvt, HINT32	nEventId, HUINT16 usPid, HUINT8 *pucPrivateData, HUINT32 ulPrivateLength)
{
	HxLOG_Assert(pstDsmccEvt);

	pstDsmccEvt->nEventId = nEventId;
	pstDsmccEvt->usPid = usPid;
	pstDsmccEvt->ulObjInfoByteLen = ulPrivateLength;
	if ( ulPrivateLength > 0 )
	{
		pstDsmccEvt->pucObjInfoByte = (HUINT8 *)HLIB_STD_MemCalloc (ulPrivateLength + 1);
		if ( pstDsmccEvt->pucObjInfoByte == NULL )
		{
			return ERR_FAIL;
		}
		HxSTD_memcpy( pstDsmccEvt->pucObjInfoByte, pucPrivateData, ulPrivateLength );
		pstDsmccEvt->pucObjInfoByte[ulPrivateLength] = '\0';
	}

	return ERR_OK;
}


STATIC int xsvc_download_WriteEventDescriptionCallback_Hbbtv(HUINT16 usEventID, HUINT32 uiEventNPT_MSB, HUINT32 uiEventNPT_LSB, HUINT8 *pucPrivateData, HUINT32 uiPrivateLength, HUINT16 usPid )
{
	HBOOL						 bSameObjInfo = FALSE, bEventNoChange = TRUE;
	HINT32						 nCnt;
	SvcDownload_DsmccContents_t	*pstContents;
	XsvcDownload_DsmccDnInfo_t	*pstDnInfo_Dsi = NULL, *pstDnArray;
	XsvcDownload_HbbtvEvtInfo_t	*pstEvtInfo = NULL;
	HERROR						 hErr, hResult = ERR_FAIL;

	pstContents = s_pstContents;

	if (NULL == pucPrivateData || 0 == uiPrivateLength)
	{
		//even the payload is empty, it should be passed up to application.
		//HxLOG_Error("NULL == pucPrivateData || 0 == uiPrivateLength \n");
		//return ERR_FAIL;
	}

	HxLOG_Debug("[%s] usEventID [%#08x] \n", __FUNCTION__, usEventID);
	if (FALSE == CHECK_DO_IT_NOW_EVNET(usEventID))
	{
		HxLOG_Error("usEventID [0x%04x] is Scheduled Event\n", usEventID);
		return ERR_FAIL;
	}

#if defined(CONFIG_DEBUG) && 0
	HxLOG_Debug("[%s] uiPrivateLength [%d] \n", __FUNCTION__, uiPrivateLength);
	if (pucPrivateData && uiPrivateLength)
	{
		HLIB_LOG_Dump(pucPrivateData, uiPrivateLength, 0, FALSE);
	}
#endif

	hErr = ERR_FAIL;

	pstDnArray = xsvc_download_GetDownloadInfo_Hbbtv(pstContents);
	if (pstDnArray == NULL)
	{
		HxLOG_Error("Download Info Failed\n");
		return ERR_FAIL;
	}

	for (nCnt = 0 ; nCnt < MAX_FILTERING_NUM ; nCnt++)
	{
		if (pstDnArray[nCnt].usPid == (HUINT16)usPid)
		{
			// libdsmcc module에 넣을 때 DSI 를 받은 곳에 넣어 주었다
			pstDnInfo_Dsi = &(pstDnArray[nCnt]);
			break;
		}
	}

	if (NULL == pstDnInfo_Dsi)
	{
		HxLOG_Error("Download Info Failed\n");
		return ERR_FAIL;
	}

    if ( pstDnInfo_Dsi->pstEventList == NULL || HLIB_LIST_Length(pstDnInfo_Dsi->pstEventList) == 0 )
    {
        HxLOG_Error("Event Message Empty\n");
        return ERR_FAIL;
    }

	(void)xsvc_download_FindEventDescInfo_Hbbtv(pstDnInfo_Dsi, (HINT32)usEventID, &pstEvtInfo);
	if (NULL == pstEvtInfo)
	{
		pstEvtInfo = (XsvcDownload_HbbtvEvtInfo_t *)HLIB_STD_MemCalloc (sizeof(XsvcDownload_HbbtvEvtInfo_t));
		if (NULL == pstEvtInfo)
		{
			HxLOG_Error("Memory allocation failed: XsvcDownload_HbbtvEvtInfo_t pstEvtInfo\n");
			goto END_FUNC;
		}

		pstEvtInfo->ulReceivedTick = HLIB_STD_GetSystemTick();

		pstEvtInfo->stEvent.usPid = usPid;
		pstEvtInfo->stEvent.pszUrl = NULL;
		pstEvtInfo->stEvent.ulDescLen = 0;
		pstEvtInfo->stEvent.szDescription = NULL;
		pstEvtInfo->stEvent.szEventName = NULL;
		if ((0 != uiPrivateLength) && (NULL != pucPrivateData))
		{
			pstEvtInfo->stEvent.pucObjInfoByte = (HUINT8 *)HLIB_STD_MemCalloc(uiPrivateLength + 1);
			if (NULL != pstEvtInfo->stEvent.pucObjInfoByte)
			{
				pstEvtInfo->stEvent.ulObjInfoByteLen = uiPrivateLength;
				HxSTD_MemCopy(pstEvtInfo->stEvent.pucObjInfoByte, pucPrivateData, uiPrivateLength);
				pstEvtInfo->stEvent.pucObjInfoByte[uiPrivateLength] = '\0';
			}
		}
		pstEvtInfo->stEvent.nEventId = usEventID;
		pstEvtInfo->stEvent.usTag = 0;

		hErr = xsvc_download_AppendEvtDescInfo_Hbbtv(pstDnInfo_Dsi, pstEvtInfo);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("xsvc_download_AppendEvtInfo_Hbbtv err:\n");
		}
	}

	bSameObjInfo = xsvc_download_CmpBinary(pucPrivateData, uiPrivateLength, pstEvtInfo->pucObjInfoFromDesc, pstEvtInfo->ulObjInfoFromDesc);
	if (TRUE != bSameObjInfo)
	{
	 	HUINT32					 ulObjInfoByteLen = 0;
		HUINT8					*pucObjInfoByte = NULL;
		(void)xsvc_download_UpdateObjInfoByte(&(pstEvtInfo->ulObjInfoFromDesc), &(pstEvtInfo->pucObjInfoFromDesc), uiPrivateLength, pucPrivateData);
		(void)xsvc_download_GetHighPriorObjectInfoByte(pstEvtInfo, &ulObjInfoByteLen, &pucObjInfoByte);
		(void)xsvc_download_UpdateObjInfoByte(&(pstEvtInfo->stEvent.ulObjInfoByteLen), &(pstEvtInfo->stEvent.pucObjInfoByte), ulObjInfoByteLen, pucObjInfoByte);

		bEventNoChange = FALSE;
	}

    if (TRUE != bEventNoChange)
	{
	    (void)xsvc_download_InformHbbtvEventDescriptionFound(pstContents, pstDnInfo_Dsi, pstEvtInfo);
	}

	HxLOG_Print("[%s:%d] (-)\n", __FUNCTION__, __LINE__);
	hResult = ERR_OK;

END_FUNC:
	return hResult;
}

STATIC int xsvc_download_WriteDirCallback_Hbbtv (char *szDirName, char *szDirPath, char *szChanName, int nCarouselId, int nPid, int nModuleId)
{
	HINT32		 nCount;
	HCHAR		*szUpperDirPath = NULL;

	HxLOG_Print("[%s:%d] (+)\n", __FUNCTION__, __LINE__);
	HxLOG_Debug("[%s][%s][%s]\n", szDirPath, szDirName, szChanName);

	for (nCount = 0; nCount < MAX_FILTERING_NUM; nCount++)
	{
		if ((int)s_stPidBuffer[nCount].usPid == nPid)
		{
			szUpperDirPath = s_stPidBuffer[nCount].szStoragePath;
			break;
		}
	}

	HxSTD_PrintToStrN (s_szHbbtv_FileFullName, 256, "%s/", szUpperDirPath);

	if (szDirPath != NULL && szDirPath[0] != '\0')
	{
		HxSTD_StrNCat (s_szHbbtv_FileFullName, szDirPath, 255);
		HxSTD_StrNCat (s_szHbbtv_FileFullName, "/", 255);
	}

	xsvc_download_CorrectFilePathString_Hbbtv (s_szHbbtv_FileFullName);
	HLIB_DIR_Make(s_szHbbtv_FileFullName);

#if defined(CONFIG_DROP_ROOT_PRIVILEGES)
	if (HLIB_EXTRA_ChownWithParam(s_szHbbtv_FileFullName, UID_obama, GID_settop) != ERR_OK)
	{
		HxLOG_Error("Fail to chown path = %s \n", s_szHbbtv_FileFullName);
	}

	if (HLIB_EXTRA_Chmod(s_szHbbtv_FileFullName, 0770) != ERR_OK)
	{
		HxLOG_Error("Fail to chmod path = %s \n", s_szHbbtv_FileFullName);
	}
#endif

	HxLOG_Print("[%s:%d] (-)\n", __FUNCTION__, __LINE__);
	return ERR_OK;
}

STATIC void xsvc_download_CompleteCallback_Hbbtv(HINT32 nPid, ObjectCarousel_t *car)
{
	HUINT32								 ulCnt;
	XsvcDownload_DsmccDnInfo_t			*pstDnInfo_Carousel = NULL, *pstDnArray = NULL;

	HxLOG_Print("[%s:%d] (+) PID (0x%04)\n", __FUNCTION__, __LINE__, nPid);
	if (car != NULL)
	{
		HxLOG_Debug("Carousel ID (%d)\n", car->downloadId_inObjectCarousel);
	}

	/* parser->libdsmcc->callback notification 구조로 ENTER_CRITICAL_DNLDCNTS는 사용할 필요 없다. */
	pstDnArray = xsvc_download_GetDownloadInfo_Hbbtv(s_pstContents); /* Init 시점에서 설정 */
	if (pstDnArray != NULL)
	{
		for (ulCnt = 0 ; ulCnt < MAX_FILTERING_NUM ; ulCnt++)
		{
			if (car != NULL && pstDnArray[ulCnt].ulDsiCarouselId == car->downloadId_inObjectCarousel)
			{
				pstDnInfo_Carousel = &(pstDnArray[ulCnt]);
				break;
			}
		}

		if (NULL != pstDnInfo_Carousel)
		{
			svc_download_SendSectionMessage(DOWNCB_DDB_TABLE, s_pstContents->hAction, eSI_TABLE_RECEIVED, (HUINT32)pstDnInfo_Carousel->usDdbFilterId);
		}
	}
	else
	{
		HxLOG_Error ("Error:\n");
	}

	HxLOG_Print("[%s:%d] (-)\n", __FUNCTION__, __LINE__);
}

STATIC void xsvc_download_FinishCarousel (SvcDownload_DsmccContents_t *pstContents, HUINT16 usPid)
{
	HINT32						 nCnt;
	XsvcDownload_DsmccDnInfo_t	*pstDnInfo = NULL, *pstDnArray;

	pstDnArray = xsvc_download_GetDownloadInfo_Hbbtv(pstContents);
	if (NULL == pstDnArray)
	{
		HxLOG_Error("no pstDnArray found...\n");
		return;
	}

	for (nCnt = 0; nCnt < MAX_FILTERING_NUM; nCnt++)
	{
		if (pstDnArray[nCnt].usPid == usPid)
		{
			pstDnInfo = &(pstDnArray[nCnt]);
			break;
		}
	}

	if (NULL == pstDnInfo)
	{
		HxLOG_Error("Cannot find the DnInfo with the PID (0x%04x)\n", usPid);
		return;
	}

	// All the PIDs with the same carousel ID shall be changed into the monitoring mode.
	for (nCnt = 0; nCnt < MAX_FILTERING_NUM; nCnt++)
	{
		if (pstDnArray[nCnt].uiCarouselId == pstDnInfo->uiCarouselId)
		{
			pstDnArray[nCnt].eDiiMode = eFILTER_MODE_MONITORING;
			pstDnArray[nCnt].bDdbAllReceived = TRUE;

			if (0 != pstDnArray[nCnt].ulFinishTimerId)
			{
				VK_TIMER_Cancel(pstDnArray[nCnt].ulFinishTimerId);
				pstDnArray[nCnt].ulFinishTimerId = 0;
			}
		}
	}

	svc_download_UtilPostMsgToMgr(eSEVT_DOWN_DOWNLOAD_SUCCESS, pstContents->hAction, pstContents->eActType, pstDnInfo->usPid, pstDnInfo->uiCarouselId);
}

#define _____GLOBAL_FUNCTIONS____

void DOWN_HbbTv (SvcDownload_Msg_t *pstMsg, void *pvData)
{
	SvcDownload_DsmccContents_t		*pstContents = NULL;
	XsvcDownload_DsmccDnInfo_t		*pstDnInfo	= NULL;
	HERROR							 hErr;
	HUINT32							 i = 0, ulCurnDdbPid = 0, uiCarouselId = 0;

 	if (pstMsg == NULL || pvData == NULL)
	{
		HxLOG_Error ("Error:\n");
		return ;
	}

	pstContents = (SvcDownload_DsmccContents_t *)pvData;

	switch (pstMsg->ulMsgClass)
	{
		case CMD_DOWN_START :
			HxLOG_Info("CMD_DOWN_START\n");
			(void)xsvc_download_SetDownloadContents_Hbbtv(pstContents, pstMsg);
			break;

		case CMD_DOWN_STOP :
			HxLOG_Info("CMD_DOWN_STOP\n");
			xsvc_download_StopFiltering_Hbbtv(pstContents, pstMsg);
			break;

		case CMD_DOWN_SET:
			HxLOG_Info("CMD_DOWN_SET eSetType(%d)\n", pstMsg->unArg.stSet.eSetType);
			switch (pstMsg->unArg.stSet.eSetType)
			{
			case eDOWN_SET_HBBTV_ADDPID:
				pstDnInfo = xsvc_download_GetDownloadInfo_Hbbtv(pstContents);
				if (NULL != pstDnInfo)
				{
					hErr = xsvc_download_AddPidOnContentTable_Hbbtv(pstDnInfo, pstMsg);
					if (ERR_OK == hErr)
					{
						SvcDownload_SetHbbtvAddPid_t	 *pstHbbtvAddPid = &(pstMsg->unArg.stSet.stHbbtvAddPid);

						switch (pstHbbtvAddPid->ucStreamType)
						{
						case DSMCC_STREAMTYPE_UN_MESSAGES:
							xsvc_download_StartDsiDiiFiltering_Hbbtv(pstContents, pstHbbtvAddPid->usPid);
							xsvc_download_StartDdbFiltering_Hbbtv(pstContents, pstHbbtvAddPid->usPid);
							break;

						case DSMCC_STREAMTYPE_STREAM_DESCRIPTION:
							xsvc_download_StartDescFiltering_Hbbtv(pstContents, pstHbbtvAddPid->usPid);
							break;

						default:
							HxLOG_Error("Unsupported Stream Type(0x%02x) PID(0x%04x)\n", pstHbbtvAddPid->ucStreamType, pstHbbtvAddPid->usPid);
							break;
						}

					}
				}

				break;

			case eDOWN_SET_HBBTV_REMOVEPID:
				xsvc_download_StopFiltering_Hbbtv(pstContents, pstMsg);
				break;

			default:
				break;
			}

			break;

		case DOWNCB_DSI_TABLE:
			HxLOG_Info("DOWNCB_DSI_TABLE, TableStatus (0x%02x)\n", pstMsg->unArg.stSection.ulTableStatus);
			if (pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_RECEIVED)
			{
				// DO Nothing in DSI...
 			}
			else if (pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_CHANGED)
			{
				/* the version of DSI table is changed, so stop section filters with same filter ID.
				  then inform eSEVT_DOWN_DOWNLOAD_CHANGED to app. Ap will restart download routine :ssyoo */
				xsvc_download_RestartDsiDiiFilter_Hbbtv (pstContents, pstMsg);
			}
			else
			{
				/*xsvc_download_StopFiltering_Hbbtv (pstContents);
				svc_download_UtilPostMsgToMgr (eSEVT_DOWN_DOWNLOAD_FAIL, pstContents->hAction, (HUINT32)pstContents->eActType, (HUINT32)pstContents->usPid, 0);*/
			}
			break;

		case DOWNCB_DDB_TABLE:
			HxLOG_Info("DOWNCB_DDB_TABLE, TableStatus (0x%02x)\n", pstMsg->unArg.stSection.ulTableStatus);
			pstDnInfo = xsvc_download_GetDownloadInfo_Hbbtv (pstContents);

 #ifdef TRACE_FILTER
 			for (i = 0 ; i < MAX_FILTERING_NUM ; i ++)
			{
				HxLOG_Debug("Filter(%d) PID(%04X) DSI=%08x DDB=%08x DESC=%08x\n", i, pstDnInfo[i].usPid, pstDnInfo[i].usDsiFilterId, pstDnInfo[i].usDdbFilterId, pstDnInfo[i].usDescFilterId);
			}
#endif

			if (pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_RECEIVED)
			{
 				uiCarouselId = (HUINT32)-1;

				for (i = 0 ; i < MAX_FILTERING_NUM ; i ++)
				{
					if (pstDnInfo[i].usDdbFilterId == pstMsg->unArg.stSection.ulFilterId && pstDnInfo[i].usDdbFilterId != 0)
					{
						uiCarouselId = pstDnInfo[i].uiCarouselId;
						ulCurnDdbPid = pstDnInfo[i].usPid;
						break;
					}
				}
 				/* TODO : Object Carousel Parsing */
				HxLOG_Print("[%s:%d] DSMCC Download Complete : PID (%u)\n", __FUNCTION__, __LINE__, ulCurnDdbPid);
				svc_download_UtilPostMsgToMgr (eSEVT_DOWN_DOWNLOAD_SUCCESS, pstContents->hAction, pstContents->eActType, ulCurnDdbPid, uiCarouselId);
 			}
			else
			{
				/* svc_download_UtilPostMsgToMgr (eSEVT_DOWN_DOWNLOAD_FAIL, pstContents->hAction, (HUINT32)pstContents->eActType, ulCurnDdbPid, 0);*/
			}
			break;

		case DOWNCB_CAROUSEL_TIMEOUT:
			HxLOG_Info("DOWNCB_CAROUSEL_TIMEOUT\n");
			{
				HUINT16			 usPid = (HUINT16)pstMsg->unArg.stSection.ulFilterId;

				// Finish the download and go to the monitoring mode.
				ENTER_CRITICAL_DNLDCNTS;
				xsvc_download_FinishCarousel(pstContents, usPid);
				LEAVE_CRITICAL_DNLDCNTS;
			}

			break;

		default :
			break;
	}
}
#endif

HERROR xsvc_download_InitAction_Hbbtv (void **ppvAction, void **ppvData, void *pvSiMsg)
{
#if defined(ALLOW_TO_COMPILE_THIS_MODULE)
	STATIC HBOOL			 s_bFirst = TRUE;
	SvcDownload_DsmccContents_t	*pstContents;
	XsvcDownload_DsmccContainer_t		*pstDsmccData;
	XsvcDownload_DsmccDnInfo_t			*pstDnInfo;
	HERROR					 hErr;

	HxLOG_Print("[%s:%d] (+)\n", __FUNCTION__, __LINE__);

	if (TRUE == s_bFirst)
	{
		HxSTD_MemSet (s_stPidBuffer, 0, sizeof(XsvcDownload_DsmccPidBuffer_t) * MAX_FILTERING_NUM);
		s_bFirst = FALSE;
	}

	// for evading the warning message :
	(void)pvSiMsg;

	if (ppvAction == NULL || ppvData == NULL)
	{
		return ERR_DOWN_TARGET_NULL;
	}

	pstDsmccData = (XsvcDownload_DsmccContainer_t *)HLIB_STD_MemCalloc (sizeof(XsvcDownload_DsmccContainer_t));
	if (pstDsmccData == NULL)
	{
		HxLOG_Error ("Error:\n");
		return ERR_DOWN_MEM_ALLOC_FAIL;
	}

//	HLIB_LOG_SetLevel(HxLOG_DOMAIN, HxLOG_FATAL);		//	HLIB_LOG_SetLevel(HxLOG_DOMAIN, HxLOG_FATAL|HxLOG_DEBUG);

	HxSTD_memset (pstDsmccData, 0, sizeof(XsvcDownload_DsmccContainer_t));

	pstDnInfo = xsvc_download_AllocDownloadInfo_Hbbtv();
	if (pstDnInfo == NULL)
	{
		HxLOG_Error ("Error:\n");

		HLIB_STD_MemFree(pstDsmccData);
		pstDsmccData = NULL;
		return ERR_DOWN_MEM_ALLOC_FAIL;
	}

	hErr = svc_download_AllocDsmccContents(&pstContents);
	if (hErr != ERR_OK)
	{
		HxLOG_Error ("Error:\n");
		xsvc_download_FreeDownloadInfo_Hbbtv (pstDnInfo);
		HLIB_STD_MemFree(pstDsmccData);
		pstDsmccData = NULL;
		return ERR_DOWN_MEM_ALLOC_FAIL;
	}

	pstDsmccData->pstDnInfo		= pstDnInfo;
	pstContents->hAction		= HANDLE_NULL;
	pstContents->ulDemuxId		= RxRSCID_NULL;
	pstContents->pvDownloadInfo	= (void *)pstDsmccData;
	*ppvAction	= (void *)DOWN_HbbTv;
	*ppvData	= (void *)pstContents;
	s_pstContents = pstContents;	/* libdsmcc calback에서 사용 */
#else
	(void)ppvAction;
	(void)ppvData;
	(void)pvSiMsg;
#endif

	HxLOG_Print("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

	return ERR_DOWN_RESULT_OK;
}

HERROR xsvc_download_TermAction_Hbbtv (void **ppvAction, void **ppvData)
{
#if defined(ALLOW_TO_COMPILE_THIS_MODULE)
	SvcDownload_DsmccContents_t	*pstContents;
//	XsvcDownload_DsmccDnInfo_t			*pstDnInfo;

	HxLOG_Print("[%s:%d] (+)\n", __FUNCTION__, __LINE__);

	if (ppvAction == NULL || ppvData == NULL)
	{
    	return ERR_DOWN_TARGET_NULL;
	}

	*ppvAction = NULL;

	if (*ppvData != NULL)
	{
		pstContents = (SvcDownload_DsmccContents_t *)(*ppvData);
//		pstDnInfo = xsvc_download_GetDownloadInfo_Hbbtv (pstContents);
		xsvc_download_FreeDsmccContainer_Hbbtv (pstContents);
		svc_download_FreeDsmccContents (pstContents);
    	*ppvData = NULL;
	}
#else
	(void)ppvAction;
	(void)ppvData;
#endif

	HxLOG_Print("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

	return ERR_DOWN_RESULT_OK;
}

HERROR xsvc_download_GetData_Hbbtv (void *pvData, SvcDownload_DataType_e eDataType, HUINT32 *pulDataNum, void **ppvDataArray)
{
#if defined(ALLOW_TO_COMPILE_THIS_MODULE)
	HUINT32									 ulIndex, ulDataNum;
	HxList_t								*pstListItem;
	SvcDownload_DsmccContents_t				*pstContents;
	XsvcDownload_DsmccContainer_t			*pstDsmccData;
	DxDSMCC_StreamEvent_t					 *pstDsmccArray;

	if (pulDataNum == NULL || ppvDataArray == NULL)
	{
		return ERR_FAIL;
	}

	pstContents = (SvcDownload_DsmccContents_t *)pvData;
	if (pstContents == NULL)
	{
		return ERR_FAIL;
	}

	pstDsmccData = (XsvcDownload_DsmccContainer_t *)pstContents->pvDownloadInfo;
	if (pstDsmccData == NULL)
	{
		return ERR_FAIL;
	}

	switch (eDataType)
	{
		case eDOWN_DATA_HBBTV_EVENT:
			{
				XsvcDownload_DsmccDnInfo_t		*pstDnArray;

				pstDnArray = xsvc_download_GetDownloadInfo_Hbbtv(pstContents);
				if (pstDnArray == NULL)
				{
					HxLOG_Error("Download Info Failed\n");
					return ERR_FAIL;
				}

				ulDataNum = 0;
				for (ulIndex = 0 ; ulIndex < MAX_FILTERING_NUM ; ulIndex++)
				{
					ulDataNum += HLIB_LIST_Length(pstDnArray[ulIndex].pstEventList);
				}

				if (ulDataNum == 0)
				{
					HxLOG_Error("DataNum 0\n");
					return ERR_FAIL;
				}

				pstDsmccArray = (DxDSMCC_StreamEvent_t *)HLIB_STD_MemCalloc (sizeof(DxDSMCC_StreamEvent_t) * ulDataNum);
				if (pstDsmccArray == NULL)
				{
					HxLOG_Error("Memory Allocation Failed\n");
					return ERR_FAIL;
				}

				for (ulIndex = 0 ; ulIndex < MAX_FILTERING_NUM ; ulIndex++)
				{
					for (pstListItem = pstDnArray[ulIndex].pstEventList; NULL != pstListItem; pstListItem = pstListItem->next)
					{
						XsvcDownload_HbbtvEvtInfo_t		*pstEvtItem = (XsvcDownload_HbbtvEvtInfo_t *)HLIB_LIST_Data(pstListItem);

						if (NULL != pstEvtItem)
						{
							xsvc_download_CopyEventItem_Hbbtv(&(pstDsmccArray[ulIndex++]), &(pstEvtItem->stEvent));
							if (ulIndex >= ulDataNum)
							{
								break;
							}
						}
					}
				}
			}

			*pulDataNum		= ulIndex;
			*ppvDataArray	= (void *)pstDsmccArray;
			return ERR_OK;
		default:
			return ERR_FAIL;
	}

	return ERR_OK;
#else
	return ERR_FAIL;
#endif
}

HERROR xsvc_download_FreeData_Hbbtv (SvcDownload_DataType_e eDataType, HUINT32 ulDataNum, void *pvDataArray)
{
#if defined(ALLOW_TO_COMPILE_THIS_MODULE)
	HUINT32						 ulIndex;
	DxDSMCC_StreamEvent_t		*pstDsmccArray;

	if (ulDataNum == 0 || pvDataArray == NULL)
	{
		return ERR_FAIL;
	}

	switch (eDataType)
	{
		case eDOWN_DATA_HBBTV_EVENT:
			pstDsmccArray = (DxDSMCC_StreamEvent_t *)pvDataArray;
			for (ulIndex = 0; ulIndex < ulDataNum; ulIndex++)
			{
				xsvc_download_FreeDsmccEventInsideOnly_Hbbtv (&(pstDsmccArray[ulIndex]));
			}
			HLIB_STD_MemFree (pstDsmccArray);
			pstDsmccArray = NULL;
			return ERR_OK;

		default:
			return ERR_FAIL;
	}

	return ERR_OK;
#else
	return ERR_FAIL;
#endif
}


/*********************** End of File ******************************/
