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
#include <pal_pipe.h>
#include <util.h>
#include <linkedlist.h>
#include <db_svc.h>
#include <pal_sef.h>

/** @brief local header file  */
#include <svc_download.h>
#include <_svc_download.h>

#include "../local_include/_xsvc_download.h"
#include <hlib.h>
/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define __USE_LIBDSMCC__ /* by Dylan - libdsmcc */

#ifdef __USE_LIBDSMCC__ /* by Dylan - libdsmcc */
#include "libdsmcc.h"
#endif

#define SECTION_MESSAGEID_POS				(10)
#define DDB_HEADER_LEN						(12)
#define MAX_OBJECTCAROUSEL_FILTER			(3)	/*refer /plugin/3rdparty_port/browser/port/hbbtv/browser_dsmcc.c */
#define MAX_PIDFILTER_OF_EACH_OC			(3) /*refer /plugin/3rdparty_port/browser/port/hbbtv/browser_dsmcc.c */
#define MAX_FILTERING_NUM					(MAX_OBJECTCAROUSEL_FILTER * MAX_PIDFILTER_OF_EACH_OC)
#define INVALID_INFO_INDEX					(99)

#define SATREC_BOOKING_HEADER_SIZE			(6)
#define SATREC_BOOKING_RECINFO_SIZE			(4)
#define	SATREC_BOOKING_IDENTIFIER_SIZE		(10)
#define SATREC_BOOKING_SIZE					(SATREC_BOOKING_RECINFO_SIZE + SATREC_BOOKING_IDENTIFIER_SIZE)
/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eDSI_MODE_RECEIVING,
	eDSI_MODE_MONITORING,
	eDSI_MODE_CHANGED
} XsvcDownload_DsmccDsiMode_e;

typedef enum
{
	eFILE_TYPE_FASTBOOKINGDATA,
	eFILE_TYPE_BOOKINGDATA,
	eFILE_TYPE_MAX
} XsvcDownload_SatrecFileType_e;

typedef struct
{
	XsvcDownload_SatrecFileType_e	eFileType;
	HUINT8				ucVersion;
	HUINT16				usPid;
	int				nFileSize;
	HCHAR				szFileFullName[FILE_PATH_NAME_LENGTH];
} XsvcDownload_DsmccFileInfo_t;

typedef struct
{
	HUINT16					usPid;
	HUINT16					usDsiFilterId, usDdbFilterId;
	HUINT32					ulDsiCrc32, ulDiiCrc32, uiCarouselId;
	HBOOL					bDsiParsed, bDiiParsed;
	HBOOL					bDdbAllReceived;
#ifdef __USE_LIBDSMCC__ /* by Dylan - libdsmcc */
	struct dsmcc_status 	*pstDsmccStatus;
	/* ssyoo : dsmcc lib에서 사용하는 자원에 대해서 동시에 접근되지 않도록 관리해야함 */
	unsigned long			uiSemaForResource;
#endif
#if defined(HXLIST_CHANGE_TO_LINKEDLST)
	HxList_t					*pstFileInfoList;
#else
	POINTER_LIST_T			*pstFileInfoList;
#endif
	XsvcDownload_DsmccDsiMode_e 		eDsiMode;
} XsvcDownload_DsmccDnInfo_t;

typedef struct
{
	HUINT16				usPid;
	HCHAR				szStoragePath[FILE_PATH_NAME_LENGTH];
} XsvcDownload_DsmccPidBuffer_t;

typedef struct
{
	HUINT8		ucVersion;

	HUINT8		ucPartNumber;
	HUINT8		ucLastPartNum;

	HUINT16		usPollingPeriod;

	HUINT8		ucBookingIdFormat;
	HUINT16		usBookingIdListCnt;
}XsvcDownload_SatRecHearder_t;

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HUINT8 						 		s_ucaChannel[256];
STATIC SvcDownload_DsmccContents_t			*s_pstContents		= NULL;		/* callback notification을 받으면 DnInfo 확인을 위해 사용. */
STATIC XsvcDownload_DsmccPidBuffer_t 		s_stPidBuffer[MAX_FILTERING_NUM];

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
void DOWN_SatRecord (SvcDownload_Msg_t *pstMsg, void *pvData);
STATIC void	 xsvc_download_SatRecordFreeListItem (void *pvItem);
STATIC void xsvc_download_FreeDsmccContainer_SatRecord (SvcDownload_DsmccContents_t *pstContents);
STATIC XsvcDownload_DsmccDnInfo_t *xsvc_download_AllocDownloadInfo_SatRecord (void);
STATIC void xsvc_download_FreeDownloadInfo_SatRecord (XsvcDownload_DsmccDnInfo_t *pstInfo);
STATIC XsvcDownload_DsmccDnInfo_t *xsvc_download_GetDownloadInfo_SatRecord (SvcDownload_DsmccContents_t *pstContents);
STATIC HERROR xsvc_download_ClearDnInfo_SatRecord (SvcDownload_DsmccContents_t *pstContents, HUINT32 ulIdx, SvcDownload_Msg_t *pstMsg );
STATIC HBOOL xsvc_download_FilterFileInfoSameName_SatRecord (void *pvItem, void *pvFilter);
STATIC HERROR xsvc_download_AppendFileInfo_SatRecord (XsvcDownload_DsmccDnInfo_t *pstDnInfo, XsvcDownload_DsmccFileInfo_t *pstFileInfo);
static HERROR xsvc_download_SetFileInfoList_SatRecord (HUINT16 usPid, HUINT8 ucVersion, int nFileSize, HCHAR *pstFullFileName, HCHAR *pstFileName);
STATIC HERROR xsvc_download_CountFileInfo_SatRecord (XsvcDownload_DsmccDnInfo_t *pstDnInfo, HUINT16 *pulNumTsInfo);
STATIC HERROR xsvc_download_GetNthFileInfo_SatRecord (XsvcDownload_DsmccDnInfo_t *pstDnInfo, HUINT16 usIndex, XsvcDownload_DsmccFileInfo_t **ppstFileInfo);
STATIC HERROR xsvc_download_GetVersionInfo_SatRecord (SvcDownload_DsmccContents_t	*pstContents, HUINT32 *pulDataNum, HUINT8 **ppvDataArray);
STATIC HERROR xsvc_download_GetPollingPeriodFromFile(Handle_t hFileHandle, HUINT16	*pnPollingPeriod);
STATIC HERROR xsvc_download_GetRecData_SatRecord (SvcDownload_DsmccContents_t	*pstContents, SvcDownload_DataType_e eDataType, HUINT32 *pulDataNum, SvcDownload_SatRecData_t **ppvDataArray);
STATIC HERROR xsvc_download_GetFastBookingDataFromFile(Handle_t hFileHandle, HUINT32 ulFileLength, HxList_t **ppvDataList);
STATIC HERROR xsvc_download_GetBookingDataFromFile(Handle_t hFileHandle, HUINT32 ulFileLength, HxList_t **ppvDataList);
STATIC HERROR xsvc_download_GetSatBookingIdentifierFromFBD(Handle_t hFileHandle, SvcDownload_SatRecData_t *pstBookingData, HUINT32 orgReceiverId);
STATIC HERROR xsvc_download_GetSatBookingIdentifierFromBD(Handle_t hFileHandle, SvcDownload_SatRecData_t *pstBookingData, HUINT32 orgReceiverId);
STATIC HERROR xsvc_download_ParseSatIdentifier(HUINT8 *pSrcBuffer, SvcDownload_SatRecBooking_t *pstBookingData);
STATIC HERROR xsvc_download_FileLength_SatRecord(HUINT8 *pucFileName, HUINT32 *pulFileLength);
STATIC HERROR xsvc_download_Fopen_SatRecord(HUINT8 *pszFileName, Handle_t *phFileHandle);
STATIC HERROR xsvc_download_Fread_SatRecord(void *pBuffer, HUINT32 ulUnitSize, Handle_t hFileHandle, HxFILE_Seek_e eMode, HUINT32 *pulReadSize, HUINT64 offset);
STATIC HERROR xsvc_download_Fclose_SatRecord(Handle_t hFileHandle);
#ifdef __USE_LIBDSMCC__ /* by Dylan - libdsmcc */
STATIC HERROR xsvc_download_CorrectFilePathString_SatRecord (HCHAR *szPath);
STATIC int xsvc_download_WriteFileCallback_SatRecord (char *szFileName, char *szFilePath, char *szChanName, int nPid, int nFileSize, unsigned char *pucFileBuffer);
STATIC int xsvc_download_WriteDirCallback_SatRecord (char *szDirName, char *szDirPath, char *szChanName, int nPid);
STATIC void xsvc_download_CompleteCallback_SatRecord(HINT32 nPid, ObjectCarousel_t *car);
STATIC HERROR xsvc_download_ClearLibResource_SatRecord ( SvcDownload_DsmccContents_t *pstContents, unsigned int uiIndex );
#endif
STATIC void xsvc_download_ParseDsi_SatRecord (HUINT8 *pucRaw, Handle_t hAction, HUINT32 ulFilterId);
STATIC void xsvc_download_ParseDii_SatRecord (HUINT8 *pucRaw, Handle_t hAction, HUINT32 ulFilterId);
STATIC void xsvc_download_ParseDdb_SatRecord (HUINT8 *pucRaw, Handle_t hAction, HUINT32 ulFilterId);
STATIC void xsvc_download_CallbackDsiDii_SatRecord (HUINT32 ulTableIdAndResult, HUINT32 ulfilterAndUniqId, HUINT32 ulParam, Handle_t hAction);
STATIC void xsvc_download_CallbackDdb_SatRecord (HUINT32 ulTableIdAndResult, HUINT32 ulfilterAndUniqId, HUINT32 ulParam, Handle_t hAction);
STATIC HERROR xsvc_download_StartDsiDiiFiltering_SatRecord (SvcDownload_DsmccContents_t *pstContents, SvcDownload_Msg_t *pstMsg );
STATIC HERROR xsvc_download_StartDdbFiltering_SatRecord (SvcDownload_DsmccContents_t *pstContents, HUINT32 ulIdx);
STATIC HERROR xsvc_download_StartDownloadDdb_SatRecord (SvcDownload_DsmccContents_t *pstContents, SvcDownload_Msg_t *pstMsg);
STATIC HERROR xsvc_download_StopDsiDiiFiltering_SatRecord (SvcDownload_DsmccContents_t *pstContents, HUINT32 ulIdx, SvcDownload_Msg_t *pstMsg );
STATIC HERROR xsvc_download_StopDdbFiltering_SatRecord (SvcDownload_DsmccContents_t *pstContents, HUINT32 ulIdx, SvcDownload_Msg_t *pstMsg );
STATIC HERROR xsvc_download_StopFiltering_SatRecord (SvcDownload_DsmccContents_t *pstContents, SvcDownload_Msg_t *pstMsg );
STATIC HERROR xsvc_download_RestartDsiDiiFilter_SatRecord (SvcDownload_DsmccContents_t *pstContents, SvcDownload_Msg_t *pstMsg );
STATIC HERROR xsvc_download_AddPidOnContentTable_SatRecord (XsvcDownload_DsmccDnInfo_t	*pstDnInfo, SvcDownload_Msg_t *pstMsg );
STATIC HERROR xsvc_download_SetDownloadContents_SatRecord (SvcDownload_DsmccContents_t *pstContents, SvcDownload_Msg_t *pstMsg);

#define _____LOCAL_FUNCTIONS_____

STATIC void	 xsvc_download_SatRecordFreeListItem (void *pvItem)
{
	if (pvItem != NULL)
	{
		OxMW_Free (pvItem);
	}
}

STATIC void xsvc_download_FreeDsmccContainer_SatRecord (SvcDownload_DsmccContents_t *pstContents)
{
	XsvcDownload_DsmccDnInfo_t			*pstDnInfo;

	if (pstContents != NULL)
	{
		pstDnInfo = xsvc_download_GetDownloadInfo_SatRecord (pstContents);
		xsvc_download_FreeDownloadInfo_SatRecord (pstDnInfo);
	}

	return;
}

STATIC XsvcDownload_DsmccDnInfo_t *xsvc_download_AllocDownloadInfo_SatRecord (void)
{
	XsvcDownload_DsmccDnInfo_t *pstDnInfo;

	pstDnInfo = (XsvcDownload_DsmccDnInfo_t *)OxMW_Malloc (sizeof(XsvcDownload_DsmccDnInfo_t)*MAX_FILTERING_NUM);
	if (pstDnInfo != NULL)
	{
		VK_memset (pstDnInfo, 0, sizeof(XsvcDownload_DsmccDnInfo_t)*MAX_FILTERING_NUM);
	}
	return pstDnInfo;
}

STATIC void xsvc_download_FreeDownloadInfo_SatRecord (XsvcDownload_DsmccDnInfo_t *pstInfo)
{
	if (pstInfo != NULL)
	{
		OxSI_Free (pstInfo);
	}
}

STATIC XsvcDownload_DsmccDnInfo_t *xsvc_download_GetDownloadInfo_SatRecord (SvcDownload_DsmccContents_t *pstContents)
{
	XsvcDownload_DsmccDnInfo_t		*pstDownloadInfo;

	if (pstContents != NULL)
	{
		pstDownloadInfo = (XsvcDownload_DsmccDnInfo_t *)pstContents->pvDownloadInfo;
		return pstDownloadInfo;
	}

	return NULL;
}

STATIC HERROR xsvc_download_ClearDnInfo_SatRecord (SvcDownload_DsmccContents_t *pstContents, HUINT32 ulIdx, SvcDownload_Msg_t *pstMsg )
{
	XsvcDownload_DsmccDnInfo_t			*pstDnInfo	= NULL;

	HxLOG_Print ("[%s:%d] (+)\n", __FUNCTION__, __LINE__);

	pstDnInfo = xsvc_download_GetDownloadInfo_SatRecord (pstContents);

	pstDnInfo[ulIdx].usPid 			= 0;
	pstDnInfo[ulIdx].usDsiFilterId 		= FILTER_ID_NULL;
	pstDnInfo[ulIdx].usDdbFilterId 		= FILTER_ID_NULL;
	pstDnInfo[ulIdx].bDsiParsed		= FALSE;
	pstDnInfo[ulIdx].bDiiParsed		= FALSE;
	pstDnInfo[ulIdx].bDdbAllReceived	= FALSE;

	HxLOG_Print ("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

	return ERR_DOWN_RESULT_OK;
}

STATIC HBOOL xsvc_download_FilterFileInfoSameName_SatRecord (void *pvItem, void *pvFilter)
{
	XsvcDownload_DsmccFileInfo_t		*pstItem, *pstFilter;

	pstItem = (XsvcDownload_DsmccFileInfo_t *)pvItem;
	pstFilter = (XsvcDownload_DsmccFileInfo_t *)pvFilter;

	if ((pstItem->usPid == pstFilter->usPid) &&
		HxSTD_StrCmp(pstItem->szFileFullName, pstFilter->szFileFullName) == 0)
	{
		return TRUE;
	}

	return FALSE;
}

STATIC HERROR xsvc_download_CountFileInfo_SatRecord (XsvcDownload_DsmccDnInfo_t *pstDnInfo, HUINT16 *pulNumTsInfo)
{
	if (pulNumTsInfo == NULL)				{ return ERR_DOWN_TARGET_NULL; }
#if defined(HXLIST_CHANGE_TO_LINKEDLST)
	*pulNumTsInfo = HLIB_LIST_Length(pstDnInfo->pstFileInfoList);
#else
	*pulNumTsInfo = UTIL_LINKEDLIST_GetNumListItemsInPointerList (pstDnInfo->pstFileInfoList, NULL, NULL);
#endif
	return ERR_OK;
}

STATIC HERROR xsvc_download_GetNthFileInfo_SatRecord (XsvcDownload_DsmccDnInfo_t *pstDnInfo, HUINT16 usIndex, XsvcDownload_DsmccFileInfo_t **ppstFileInfo)
{
	XsvcDownload_DsmccFileInfo_t *pstSrcFileInfo;
#if defined(HXLIST_CHANGE_TO_LINKEDLST)
	HxList_t						*pstItem = NULL;
#else
	POINTER_LIST_T						*pstItem;
#endif
	if (ppstFileInfo == NULL)				{ return ERR_DOWN_TARGET_NULL; }

	if (pstDnInfo->pstFileInfoList != NULL)
	{
#if defined(HXLIST_CHANGE_TO_LINKEDLST)
		pstItem = HLIB_LIST_GetListItem(pstDnInfo->pstFileInfoList, usIndex);
#else
  		pstItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList (pstDnInfo->pstFileInfoList, usIndex, NULL, NULL);
#endif
		if (pstItem != NULL)
		{
#if defined(HXLIST_CHANGE_TO_LINKEDLST)
			pstSrcFileInfo = (XsvcDownload_DsmccFileInfo_t *)HLIB_LIST_Data (pstItem);
#else
			pstSrcFileInfo = (XsvcDownload_DsmccFileInfo_t *)UTIL_LINKEDLIST_GetContents (pstItem);
#endif
			if (pstSrcFileInfo != NULL)
			{
				*ppstFileInfo = pstSrcFileInfo;
				return ERR_OK;
			}
		}
	}

	return ERR_FAIL;
}

STATIC HERROR xsvc_download_AppendFileInfo_SatRecord (XsvcDownload_DsmccDnInfo_t *pstDnInfo, XsvcDownload_DsmccFileInfo_t *pstFileInfo)
{
	XsvcDownload_DsmccFileInfo_t		stFilterFileInfo;
	XsvcDownload_DsmccFileInfo_t		*pstSatRecFileInfo;
#if defined(HXLIST_CHANGE_TO_LINKEDLST)
	HxList_t						*pstItem = NULL;
#else
	POINTER_LIST_T						*pstItem;
#endif
	VK_memset (&stFilterFileInfo, 0, sizeof(XsvcDownload_DsmccFileInfo_t));
	stFilterFileInfo = *pstFileInfo;
	
#if defined(HXLIST_CHANGE_TO_LINKEDLST)
	pstItem = HLIB_LIST_FindEx(pstDnInfo->pstFileInfoList, (const void *)&stFilterFileInfo, xsvc_download_FilterFileInfoSameName_SatRecord);
#else
	pstItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList (pstDnInfo->pstFileInfoList, 0,
															xsvc_download_FilterFileInfoSameName_SatRecord,
															(void *)&stFilterFileInfo);
#endif
	if (pstItem == NULL)
	{
		pstSatRecFileInfo = (XsvcDownload_DsmccFileInfo_t *)OxAP_Malloc (sizeof(XsvcDownload_DsmccFileInfo_t));
		if (pstSatRecFileInfo == NULL)			{ return ERR_FAIL; }

		*pstSatRecFileInfo = stFilterFileInfo;
		
#if defined(HXLIST_CHANGE_TO_LINKEDLST)
		pstDnInfo->pstFileInfoList = HLIB_LIST_Append(pstDnInfo->pstFileInfoList,(void *)pstSatRecFileInfo);
#else
		pstDnInfo->pstFileInfoList = UTIL_LINKEDLIST_AppendListItemToPointerList (pstDnInfo->pstFileInfoList,(void *)pstSatRecFileInfo);
#endif
	}

	return ERR_OK;
}


static HERROR xsvc_download_SetFileInfoList_SatRecord (HUINT16 usPid, HUINT8 ucVersion, int nFileSize, HCHAR *pstFullFileName, HCHAR *pstFileName)
{
	HERROR								hErr;
	XsvcDownload_DsmccDnInfo_t			*pstDnInfo	= NULL;
	XsvcDownload_DsmccFileInfo_t		stFileInfo;

	int i = 0;

	pstDnInfo = xsvc_download_GetDownloadInfo_SatRecord (s_pstContents);

	if (pstDnInfo == NULL)
	{
		hErr = ERR_DOWN_TARGET_NULL;
	}

	for (i = 0 ; i < MAX_FILTERING_NUM ; i ++)
	{
		if(&pstDnInfo[i] == NULL)
			continue;

		if ( pstDnInfo[i].usPid == usPid )
		{
			break;
		}
	}

	if(i == MAX_FILTERING_NUM)
	{
		HxLOG_Error ("Error:\n");
		return ERR_DOWN_RESULT_FAIL;
	}

	VK_memset (&stFileInfo, 0, sizeof(XsvcDownload_DsmccFileInfo_t));
	stFileInfo.usPid = usPid;
	stFileInfo.nFileSize = nFileSize;
	stFileInfo.ucVersion = ucVersion;

	if(HxSTD_StrNCmp(pstFileName, "rr.fd", 5) == 0)
	{
		stFileInfo.eFileType = eFILE_TYPE_FASTBOOKINGDATA;
	}
	else
	{
		stFileInfo.eFileType = eFILE_TYPE_BOOKINGDATA;
	}

	HxSTD_StrNCpy(stFileInfo.szFileFullName, pstFullFileName, FILE_PATH_NAME_LENGTH-1);

	hErr = xsvc_download_AppendFileInfo_SatRecord(&pstDnInfo[i], &stFileInfo);

	return hErr;
}

STATIC HERROR xsvc_download_GetVersionInfo_SatRecord (SvcDownload_DsmccContents_t	*pstContents, HUINT32 *pulDataNum, HUINT8 **ppvDataArray)
{
	HERROR		hErr = ERR_OK;
	HUINT16		nFile, nCntFile, i, nDataNum = 0;

	XsvcDownload_DsmccDnInfo_t			*pstDnInfo	= NULL;
	XsvcDownload_DsmccFileInfo_t		*pstFileInfo;

	HUINT8		*pstSatRecInfoArray= NULL;
	HBOOL		bSameVersion = TRUE;

	pstDnInfo = xsvc_download_GetDownloadInfo_SatRecord (pstContents);
	if (pstDnInfo == NULL)
	{
		return ERR_DOWN_TARGET_NULL;
	}

	for (i = 0 ; i < MAX_FILTERING_NUM ; i ++)
	{
		if ( pstDnInfo[i].usPid == pstContents->usPid)
		{
			break;
		}
	}

	if(i == MAX_FILTERING_NUM)
	{
		HxLOG_Error ("Error:\n");
		return ERR_FAIL;
	}

	hErr = xsvc_download_CountFileInfo_SatRecord(&pstDnInfo[i], &nFile);
	if(hErr != ERR_OK || nFile == 0)
	{
		HxLOG_Error ("Error: File doesn't Exist\n");
		return ERR_FAIL;
	}

	pstSatRecInfoArray = (HUINT8 *)OxMW_Malloc (sizeof(HUINT8) * eFILE_TYPE_MAX);
	VK_memset(pstSatRecInfoArray, 0xFF, sizeof(HUINT8)*eFILE_TYPE_MAX);

	for(nCntFile = 0; nCntFile < nFile; nCntFile++)
	{
		hErr = xsvc_download_GetNthFileInfo_SatRecord(&pstDnInfo[i], nCntFile, &pstFileInfo);

		if(hErr != ERR_OK)
		{
			HxLOG_Error ("Error: Fail get %d th file info\n", nCntFile);
			continue;
		}

		if(pstFileInfo->eFileType == eFILE_TYPE_FASTBOOKINGDATA)
		{
			pstSatRecInfoArray[eFILE_TYPE_FASTBOOKINGDATA] = pstFileInfo->ucVersion;
			nDataNum++;
		}
		else
		{
			if(pstSatRecInfoArray[eFILE_TYPE_BOOKINGDATA] == 0xFF)
			{
				pstSatRecInfoArray[eFILE_TYPE_BOOKINGDATA] = pstFileInfo->ucVersion;
				nDataNum++;
			}
			else
			{
				if(pstFileInfo->ucVersion != pstSatRecInfoArray[eFILE_TYPE_BOOKINGDATA])
				{
					bSameVersion = FALSE;
				}
			}
		}
	}

	if(bSameVersion == FALSE || nDataNum == 0)
	{
		if (pstSatRecInfoArray != NULL) 			{ OxMW_Free (pstSatRecInfoArray); }
		return ERR_FAIL;
	}

	if(pulDataNum != NULL)
	{
		*pulDataNum = nDataNum;
	}

	if (ppvDataArray != NULL)
	{
		*ppvDataArray = pstSatRecInfoArray;
		pstSatRecInfoArray = NULL;
	}

	if (pstSatRecInfoArray != NULL) 			{ OxMW_Free (pstSatRecInfoArray); }

	return hErr;
}

STATIC HERROR xsvc_downlaod_GetPollingInfo_SatRecord(SvcDownload_DsmccContents_t	*pstContents, HUINT32 *pulDataNum, HUINT16 **ppvDataArray)
{
	HERROR		hErr = ERR_OK;
	HBOOL		bFound = FALSE;
	HUINT16 	nFile, nCntFile, i;
	HUINT16		*pstPollingPeriod = NULL;
	Handle_t	hFileHandle = (Handle_t)NULL;

	XsvcDownload_DsmccDnInfo_t			*pstDnInfo	= NULL;
	XsvcDownload_DsmccFileInfo_t		*pstFileInfo;

	pstDnInfo = xsvc_download_GetDownloadInfo_SatRecord (pstContents);
	if (pstDnInfo == NULL)
	{
		return ERR_DOWN_TARGET_NULL;
	}

	for (i = 0 ; i < MAX_FILTERING_NUM ; i ++)
	{
		if ( pstDnInfo[i].usPid == pstContents->usPid)
		{
			break;
		}
	}

	if(i == MAX_FILTERING_NUM)
	{
		HxLOG_Error ("Error:\n");
		return ERR_FAIL;
	}


	hErr = xsvc_download_CountFileInfo_SatRecord(&pstDnInfo[i], &nFile);
	if(hErr != ERR_OK || nFile == 0)
	{
		HxLOG_Error ("Error: File doesn't Exist\n");
		return ERR_FAIL;
	}

	pstPollingPeriod = (HUINT16 *)OxMW_Malloc (sizeof(HUINT16));
	VK_memset(pstPollingPeriod, 0x0, sizeof(HUINT16));

	for(nCntFile = 0; nCntFile < nFile; nCntFile++)
	{
		hErr = xsvc_download_GetNthFileInfo_SatRecord(&pstDnInfo[i], nCntFile, &pstFileInfo);
		HxLOG_Print("[%s:%d](%d) Booking data file is %s\n",  __FUNCTION__, __LINE__,nCntFile, pstFileInfo->szFileFullName);

		if(hErr != ERR_OK || pstFileInfo->eFileType != eFILE_TYPE_BOOKINGDATA)
		{
			continue;
		}

		hErr = xsvc_download_Fopen_SatRecord((HUINT8 *)pstFileInfo->szFileFullName, &hFileHandle);
		if(hErr != ERR_OK)
		{
			HxLOG_Error ("Fail Open %d th file\n", nCntFile);
			continue;
		}

		hErr = xsvc_download_GetPollingPeriodFromFile(hFileHandle, pstPollingPeriod);
		if(hErr == ERR_OK)
		{
			bFound = TRUE;
			HxLOG_Print("[%s:%d] Success get polling period (0x%x) in %d th file\n", __FUNCTION__, __LINE__, *pstPollingPeriod, nCntFile);
		}

		hErr = xsvc_download_Fclose_SatRecord(hFileHandle);
		if(hErr != ERR_OK)
		{
			HxLOG_Error ("Fail Close %d th file\n", nCntFile);
		}

		if(bFound)	break;
	}

	if (ppvDataArray != NULL)
	{
		*ppvDataArray = pstPollingPeriod;
		pstPollingPeriod = NULL;
	}

	if (pstPollingPeriod != NULL) 			{ OxMW_Free (pstPollingPeriod); }

	return ERR_OK;
}

STATIC HERROR xsvc_download_GetRecData_SatRecord (SvcDownload_DsmccContents_t	*pstContents, SvcDownload_DataType_e eDataType, HUINT32 *pulDataNum, SvcDownload_SatRecData_t **ppvDataArray)
{
	HERROR		hErr = ERR_OK;
	HUINT16 	nFile, nCntFile, i;
	HUINT32 	nFileLength;
	HUINT32 	nTotalData= 0, usCntData = 0;

	XsvcDownload_DsmccDnInfo_t			*pstDnInfo	= NULL;
	XsvcDownload_DsmccFileInfo_t		*pstFileInfo;
	XsvcDownload_SatrecFileType_e		eRequestFileType;
	SvcDownload_SatRecData_t			*pstBookingData = NULL, *pstBookingDataArray = NULL;

	Handle_t	hFileHandle = (Handle_t)NULL;
	HxList_t	*pvDataList = NULL, *pstList = NULL;

	pstDnInfo = xsvc_download_GetDownloadInfo_SatRecord (pstContents);
	if (pstDnInfo == NULL)
	{
		return ERR_DOWN_TARGET_NULL;
	}

	if(eDataType == eDOWN_DATA_SATRECORD_FASTBOOKINGDATA)
	{
		eRequestFileType = eFILE_TYPE_FASTBOOKINGDATA;
	}
	else if(eDataType == eDOWN_DATA_SATRECORD_BOOKINGDATA)
	{
		eRequestFileType = eFILE_TYPE_BOOKINGDATA;
	}
	else
	{
		return ERR_FAIL;
	}

	for (i = 0 ; i < MAX_FILTERING_NUM ; i ++)
	{
		if ( pstDnInfo[i].usPid == pstContents->usPid)
		{
			break;
		}
	}

	if(i == MAX_FILTERING_NUM)
	{
		HxLOG_Error ("Error:\n");
		return ERR_FAIL;
	}

	hErr = xsvc_download_CountFileInfo_SatRecord(&pstDnInfo[i], &nFile);
	if(hErr != ERR_OK || nFile == 0)
	{
		HxLOG_Error ("Error: File doesn't Exist\n");
		return ERR_FAIL;
	}

	for(nCntFile = 0; nCntFile < nFile; nCntFile++)
	{
		hErr = xsvc_download_GetNthFileInfo_SatRecord(&pstDnInfo[i], nCntFile, &pstFileInfo);
		HxLOG_Print("[%s:%d](%d) Booking data file is %s\n",  __FUNCTION__, __LINE__,nCntFile, pstFileInfo->szFileFullName);

		if(hErr != ERR_OK || pstFileInfo->eFileType != eRequestFileType)
		{
			continue;
		}

		hErr = xsvc_download_FileLength_SatRecord((HUINT8 *)pstFileInfo->szFileFullName, &nFileLength);
		if(hErr != ERR_OK)
		{
			HxLOG_Error ("Fail get %d th file length\n", nCntFile);
		}

		hErr = xsvc_download_Fopen_SatRecord((HUINT8 *)pstFileInfo->szFileFullName, &hFileHandle);
		if(hErr != ERR_OK)
		{
			HxLOG_Error ("Fail Open %d th file\n", nCntFile);
			continue;
		}

		if(eRequestFileType == eFILE_TYPE_FASTBOOKINGDATA)
		{
			hErr = xsvc_download_GetFastBookingDataFromFile(hFileHandle, nFileLength, &pvDataList);
		}
		else
		{
			hErr = xsvc_download_GetBookingDataFromFile(hFileHandle, nFileLength, &pvDataList);
		}

		hErr = xsvc_download_Fclose_SatRecord(hFileHandle);
		if(hErr != ERR_OK)
		{
			HxLOG_Error ("Fail Close %d th file\n", nCntFile);
		}
	}
	nTotalData = HLIB_LIST_Length(pvDataList);

	if(nTotalData > 0)
	{
		pstBookingDataArray = (SvcDownload_SatRecData_t*)OxMW_Malloc(sizeof(SvcDownload_SatRecData_t) * nTotalData);
		for(i = 0; i < nTotalData; i++)
		{
			pstList = HLIB_LIST_GetListItem(pvDataList, i);

			if (pstList == NULL)
			{
				HxLOG_Critical ("HLIB_LIST_GetListItem() failed.. len(%d) curPos(%d)\n", nTotalData, i);
				continue;
			}

			pstBookingData = (SvcDownload_SatRecData_t *)HLIB_LIST_Data(pstList);
			if (pstBookingData == NULL)
			{
				HxLOG_Critical ("HLIB_LIST_Data() failed.. curPos(%d)\n", i);
				continue;
			}

			HxSTD_MemCopy(&pstBookingDataArray[i], pstBookingData, sizeof(SvcDownload_SatRecData_t));
			usCntData++;
		}

		if(pulDataNum != NULL)
		{
			*pulDataNum = usCntData;
		}

		if(ppvDataArray != NULL)
		{
			*ppvDataArray = pstBookingDataArray;
			pstBookingDataArray = NULL;
		}
	}
	else
	{
		*pulDataNum = 0;
		hErr = ERR_FAIL;
	}

	if (pvDataList != NULL) 			{ HLIB_LIST_RemoveAllFunc(pvDataList, xsvc_download_SatRecordFreeListItem); }
	if (pstBookingDataArray != NULL)				{ OxMW_Free (pstBookingDataArray); }

	return hErr;

}

STATIC HERROR xsvc_download_GetFastBookingDataFromFile(Handle_t hFileHandle, HUINT32 ulFileLength, HxList_t **ppvDataList)
{
	HERROR			hErr = ERR_FAIL;
	HUINT32			ulReadSize = 0;
	HUINT32			ulOffset = 0;
	HBOOL			bFoundBookingData = FALSE;
	HUINT32			orgReceiverId;

	HUINT8			*pstTempBuffer = NULL;
	HUINT16			i;
	SvcDownload_SatRecData_t stBookingData;
	SvcDownload_SatRecData_t	*pstBookingData = NULL;
	XsvcDownload_SatRecHearder_t		stFileHeader;


	pstTempBuffer = (HUINT8*)OxMW_Malloc(SATREC_BOOKING_HEADER_SIZE);
	if(pstTempBuffer == NULL)
	{
		HxLOG_Error("OxMW_Malloc fail!!\n");
		return ERR_FAIL;
	}

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_SATREC_RECEIVERID, (HUINT32 *)&orgReceiverId, 0);
	if(hErr != ERR_OK || orgReceiverId == 0)
	{
		HxLOG_Error("Fail : Get Receiver id (receiver id doesn't set)!!\n");
		if(pstTempBuffer != NULL)		OxMW_Free(pstTempBuffer);

		return ERR_FAIL;
	}

	while(ulFileLength > ulOffset + SATREC_BOOKING_HEADER_SIZE)
	{
		VK_memset (pstTempBuffer, 0, SATREC_BOOKING_HEADER_SIZE);
		hErr = xsvc_download_Fread_SatRecord(pstTempBuffer, SATREC_BOOKING_HEADER_SIZE, hFileHandle,eHxFILE_SEEK_SET, &ulReadSize, ulOffset);

		if(hErr != ERR_OK)
		{
			break;
		}

		stFileHeader.ucVersion = pstTempBuffer[0];
		stFileHeader.ucPartNumber = pstTempBuffer[1];
		stFileHeader.ucLastPartNum = pstTempBuffer[2];
		stFileHeader.ucBookingIdFormat = pstTempBuffer[3];
		stFileHeader.usBookingIdListCnt =(pstTempBuffer[4] << 8) | pstTempBuffer[5];

		HxLOG_Print("VERSION (0x%02x) Part Number (0x%02x), ucLastPartNum (0x%02x), ucBookingIdFormat(0x%02x), usBookingIdListCnt (0x%04x)\n",
			stFileHeader.ucVersion, stFileHeader.ucPartNumber,stFileHeader.ucLastPartNum, stFileHeader.ucBookingIdFormat, stFileHeader.usBookingIdListCnt);

		if(stFileHeader.ucBookingIdFormat != 0x01)
		{
			HxLOG_Error("Booking ID Format is Wrong!!\n");
			ulOffset += SATREC_BOOKING_HEADER_SIZE + (SATREC_BOOKING_SIZE * stFileHeader.usBookingIdListCnt);
			continue;
		}

		for(i = 0; i < stFileHeader.usBookingIdListCnt; i++)
		{
			//get sat booking identifyer
			hErr = xsvc_download_GetSatBookingIdentifierFromFBD(hFileHandle, &stBookingData, orgReceiverId);

			if(hErr == ERR_DOWN_SOURCE_NULL)
			{
				break;
			}

			if(hErr != ERR_OK)
			{
				continue;
			}

			pstBookingData = (SvcDownload_SatRecData_t*)OxMW_Malloc(sizeof(SvcDownload_SatRecData_t));
			if(pstBookingData != NULL)
			{
				bFoundBookingData = TRUE;
				HxSTD_memcpy(pstBookingData, &stBookingData, sizeof(SvcDownload_SatRecData_t));
				*ppvDataList = HLIB_LIST_Append(*ppvDataList, (void *)pstBookingData);
			}
		}

		ulOffset += SATREC_BOOKING_HEADER_SIZE + (SATREC_BOOKING_SIZE * stFileHeader.usBookingIdListCnt);

	}

	if(pstTempBuffer != NULL)		OxMW_Free(pstTempBuffer);

	return (bFoundBookingData == TRUE)? ERR_OK:ERR_FAIL;
}

STATIC HERROR xsvc_download_GetBookingDataFromFile(Handle_t hFileHandle, HUINT32 ulFileLength, HxList_t **ppvDataList)
{
	HERROR			hErr = ERR_FAIL;
	HUINT32			orgReceiverId;
	HUINT32 		ulReadSize = 0, ulOffset = 0;

	HUINT8			*pstTempBuffer = NULL, *pstCurPosBuf = NULL;
	HUINT16 		i;
	HBOOL			bFoundBookingData = FALSE;

	XsvcDownload_SatRecHearder_t		stFileHeader;
	SvcDownload_SatRecData_t			stBookingData;
	SvcDownload_SatRecData_t			*pstBookingData = NULL;

	pstTempBuffer = (HUINT8*)OxMW_Malloc(sizeof(XsvcDownload_SatRecHearder_t));
	if(pstTempBuffer == NULL)
	{
		HxLOG_Error("OxMW_Malloc fail!!\n");
		return ERR_FAIL;
	}

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_SATREC_RECEIVERID, (HUINT32 *)&orgReceiverId, 0);
	if(hErr != ERR_OK || orgReceiverId == 0)
	{
		HxLOG_Error("Fail : Get Receiver id (receiver id doesn't set)!!\n");
		if(pstTempBuffer != NULL)		OxMW_Free(pstTempBuffer);
		return ERR_FAIL;
	}
//	while(ulFileLength > ulOffset + SATREC_BOOKING_HEADER_SIZE)		//temp
	{
		VK_memset (pstTempBuffer, 0, sizeof(XsvcDownload_SatRecHearder_t));

		hErr = xsvc_download_Fread_SatRecord(pstTempBuffer, SATREC_BOOKING_HEADER_SIZE, hFileHandle,eHxFILE_SEEK_SET, &ulReadSize, ulOffset);

		if(hErr != ERR_OK)
		{
			if(pstTempBuffer != NULL)		OxMW_Free(pstTempBuffer);
			return ERR_FAIL;
		}

		pstCurPosBuf = pstTempBuffer;

		stFileHeader.ucVersion = *pstCurPosBuf; 	pstCurPosBuf++;
		stFileHeader.ucPartNumber = *pstCurPosBuf;		pstCurPosBuf++;
		stFileHeader.ucLastPartNum = *pstCurPosBuf; 	pstCurPosBuf++;

		if(stFileHeader.ucPartNumber == 0)
		{
			stFileHeader.usPollingPeriod = get16bit(pstCurPosBuf);		pstCurPosBuf += 2;
			//TODO: Save DB : real polling time is usPollingPeriod * 2;

			stFileHeader.ucBookingIdFormat = *pstCurPosBuf; 	pstCurPosBuf++;

			hErr = xsvc_download_Fread_SatRecord(pstCurPosBuf, sizeof(HUINT16), hFileHandle,eHxFILE_SEEK_CUR, &ulReadSize, 0);
			if(hErr == ERR_OK)
			{
				stFileHeader.usBookingIdListCnt = get16bit(pstCurPosBuf);
			}
			else
			{
				stFileHeader.usBookingIdListCnt = 0;
			}
		}
		else
		{
			stFileHeader.usPollingPeriod = 0;
			stFileHeader.ucBookingIdFormat = *pstCurPosBuf; 	pstCurPosBuf++;
			stFileHeader.usBookingIdListCnt = get16bit(pstCurPosBuf);
		}

		// TODO: compare version with DB's version

		if(stFileHeader.ucBookingIdFormat != 0x01)
		{
			HxLOG_Error("Booking ID Format is Wrong!!\n");
			if(pstTempBuffer != NULL)		OxMW_Free(pstTempBuffer);
			return ERR_OK;
//			ulOffset += SATREC_BOOKING_HEADER_SIZE + (SATREC_BOOKING_SIZE * stFileHeader.usBookingIdListCnt); 변경되어야 함.
//			continue;
		}

		for(i = 0; i < stFileHeader.usBookingIdListCnt; i++)
		{
			//get sat booking identifyer
			hErr = xsvc_download_GetSatBookingIdentifierFromBD(hFileHandle, &stBookingData, orgReceiverId);

			if(hErr == ERR_DOWN_SOURCE_NULL)
			{
				break;
			}

			if(hErr != ERR_OK)
			{
				continue;
			}

			pstBookingData = (SvcDownload_SatRecData_t*)OxMW_Malloc(sizeof(SvcDownload_SatRecData_t));
			if(pstBookingData != NULL)
			{
				HxSTD_memcpy(pstBookingData, &stBookingData, sizeof(SvcDownload_SatRecData_t));
				*ppvDataList = HLIB_LIST_Append(*ppvDataList, (void *)pstBookingData);
				bFoundBookingData = TRUE;
			}
		}

//		ulOffset += SATREC_BOOKING_HEADER_SIZE + (SATREC_BOOKING_SIZE * stFileHeader.usBookingIdListCnt); 변경되어야 함.

	}

	if(pstTempBuffer != NULL)		OxMW_Free(pstTempBuffer);

	return (bFoundBookingData == TRUE)? ERR_OK:ERR_FAIL;
}

STATIC HERROR xsvc_download_GetPollingPeriodFromFile(Handle_t hFileHandle, HUINT16	*pnPollingPeriod)
{
	HERROR			hErr = ERR_FAIL;
	HUINT8			ucPartNumber;
	HUINT16			usPollingPeriod = 0;
	HUINT32 		ulReadSize = 0;
	HUINT8			*pstTempBuffer = NULL, *pstCurPosBuf = NULL;

	pstTempBuffer = (HUINT8*)OxMW_Malloc(sizeof(XsvcDownload_SatRecHearder_t));
	if(pstTempBuffer == NULL)
	{
		HxLOG_Error("OxMW_Malloc fail!!\n");
		return ERR_FAIL;
	}

	VK_memset (pstTempBuffer, 0, sizeof(XsvcDownload_SatRecHearder_t));

	hErr = xsvc_download_Fread_SatRecord(pstTempBuffer, SATREC_BOOKING_HEADER_SIZE, hFileHandle,eHxFILE_SEEK_SET, &ulReadSize, 0);

	if(hErr != ERR_OK)
	{
		HxLOG_Error("Fail to read file!\n");
		goto ERROR;
	}

	pstCurPosBuf = pstTempBuffer++; //ucVersion -> part number
	ucPartNumber = *pstCurPosBuf;

	if(ucPartNumber != 0)
	{
		HxLOG_Print("[%s:%d] file doesn't include polling_period data!!\n", __FUNCTION__, __LINE__);
		goto ERROR;
	}

	pstCurPosBuf = pstCurPosBuf + 2; //part number -> last part number -> polling period
	usPollingPeriod = get16bit(pstCurPosBuf);		pstCurPosBuf += 2;

	ERROR:
	if(pstTempBuffer != NULL)		OxMW_Free(pstTempBuffer);

	*pnPollingPeriod = usPollingPeriod;
	return hErr;

}

STATIC HERROR xsvc_download_GetSatBookingIdentifierFromFBD(Handle_t hFileHandle, SvcDownload_SatRecData_t *pstBookingData, HUINT32 orgReceiverId)
{
	HERROR			hError = ERR_FAIL;
	HUINT8			*pstTempBuffer = NULL, *pstCurPosBuf = NULL;
	HUINT32			ulReadSize = 0;
	HUINT32			receiverId;

	if(pstBookingData == NULL)
	{
		HxLOG_Error("OxMW_Malloc fail!!\n");
		return ERR_FAIL;
	}

	pstTempBuffer = (HUINT8*)OxMW_Malloc(sizeof(HUINT8)*SATREC_BOOKING_SIZE);

	if(pstTempBuffer == NULL)
	{
		HxLOG_Error("OxMW_Malloc fail!!\n");
		return ERR_FAIL;
	}

	VK_memset (pstTempBuffer, 0, sizeof(HUINT8)*SATREC_BOOKING_SIZE);
	hError = xsvc_download_Fread_SatRecord(pstTempBuffer, sizeof(HUINT8)*SATREC_BOOKING_SIZE, hFileHandle,eHxFILE_SEEK_CUR, &ulReadSize, 0);
	if(hError != ERR_OK)
	{
		hError = ERR_DOWN_SOURCE_NULL;
		goto ERROR;
	}

	receiverId = (unsigned int)(((*pstTempBuffer & 0x0F) << 24) | (*(pstTempBuffer + 1) << 16) | (*(pstTempBuffer + 2) << 8) | (*(pstTempBuffer + 3)));

	if(receiverId != orgReceiverId)
	{
		HxLOG_Print("[%s:%d] This data is not for STB!!\n", __FUNCTION__, __LINE__);

		hError = ERR_FAIL;
		goto ERROR;
	}

	pstCurPosBuf = pstTempBuffer;
	pstBookingData->bSeriesFlag = (*pstCurPosBuf >> 7) & 0x01;
	pstBookingData->bRecordCancelFlag = ((*pstCurPosBuf >> 6) & 0x01)?TRUE:FALSE;

	pstCurPosBuf += 4;

	hError = xsvc_download_ParseSatIdentifier(pstCurPosBuf, &(pstBookingData->stBookingData));

	ERROR:
	if(pstTempBuffer != NULL)		OxMW_Free(pstTempBuffer);

	return ERR_OK;

}

STATIC HERROR xsvc_download_GetSatBookingIdentifierFromBD(Handle_t hFileHandle, SvcDownload_SatRecData_t *pstBookingData, HUINT32 orgReceiverId)
{
	HERROR			hError = ERR_FAIL;
	HUINT8			*pstTempBuffer = NULL;
	HUINT32			ulReadSize = 0;
	HUINT16			usCntReceiverIdList, i;
	HUINT32			receiverId;
	HBOOL			bFoundReceiverID = FALSE;

	if(pstBookingData == NULL)
	{
		HxLOG_Error("OxMW_Malloc fail!!\n");
		return ERR_FAIL;
	}

	pstTempBuffer = (HUINT8*)OxMW_Malloc(sizeof(HUINT8)*SATREC_BOOKING_IDENTIFIER_SIZE);

	VK_memset (pstTempBuffer, 0, sizeof(HUINT8)*SATREC_BOOKING_IDENTIFIER_SIZE);
	hError = xsvc_download_Fread_SatRecord(pstTempBuffer, sizeof(HUINT16), hFileHandle,eHxFILE_SEEK_CUR, &ulReadSize, 0);
	if(hError != ERR_OK)
	{
		hError = ERR_DOWN_SOURCE_NULL;
		goto ERROR;
	}

	usCntReceiverIdList = get16bit(pstTempBuffer);

	for(i = 0; i < usCntReceiverIdList; i++)
	{
		VK_memset (pstTempBuffer, 0, sizeof(HUINT8)*SATREC_BOOKING_IDENTIFIER_SIZE);
		bFoundReceiverID = FALSE;

		hError = xsvc_download_Fread_SatRecord(pstTempBuffer, SATREC_BOOKING_RECINFO_SIZE, hFileHandle,eHxFILE_SEEK_CUR, &ulReadSize, 0);
		if(hError != ERR_OK)
		{
			hError = ERR_DOWN_SOURCE_NULL;
			goto ERROR;
		}

		receiverId = (unsigned int)(((*pstTempBuffer & 0x0F) << 24) | (*(pstTempBuffer + 1) << 16) | (*(pstTempBuffer + 2) << 8) | (*(pstTempBuffer + 3)));

		if(receiverId == orgReceiverId)
		{
			pstBookingData->bSeriesFlag = (*pstTempBuffer >> 7) & 0x01;
			pstBookingData->bRecordCancelFlag = ((*pstTempBuffer >> 6) & 0x01)?TRUE:FALSE;
			bFoundReceiverID = TRUE;
			break;
		}
	}

	if(bFoundReceiverID)
	{
		VK_memset (pstTempBuffer, 0, sizeof(HUINT8)*SATREC_BOOKING_IDENTIFIER_SIZE);
		hError = xsvc_download_Fread_SatRecord(pstTempBuffer, SATREC_BOOKING_IDENTIFIER_SIZE, hFileHandle,eHxFILE_SEEK_CUR,
			&ulReadSize, SATREC_BOOKING_RECINFO_SIZE * (usCntReceiverIdList -1 - i));

		if (hError != ERR_OK )
		{
			HxLOG_Error("[%s] xsvc_download_Fread_SatRecord fail!!\n",__FUNCTION__);
			hError = ERR_DOWN_SOURCE_NULL;
			goto ERROR;
		}

		hError = xsvc_download_ParseSatIdentifier(pstTempBuffer, &(pstBookingData->stBookingData));
	}
	else
	{
		hError = HLIB_FILE_Seek((HxFILE_t)hFileHandle, SATREC_BOOKING_IDENTIFIER_SIZE, eHxFILE_SEEK_CUR, NULL);
		if (hError != ERR_OK )
		{
			HxLOG_Error("[%s] HLIB_FILE_Seek fail!!\n",__FUNCTION__);
			hError = ERR_DOWN_SOURCE_NULL;
			goto ERROR;
		}

		hError = ERR_FAIL;
	}

	ERROR:
		if(pstTempBuffer != NULL)		OxMW_Free(pstTempBuffer);

	return hError;
}

STATIC HERROR xsvc_download_ParseSatIdentifier(HUINT8 *pSrcBuffer, SvcDownload_SatRecBooking_t *pstBookingData)
{
	HUINT8			*pstCurPosBuf = NULL;

	if(pSrcBuffer == NULL || pstBookingData == NULL)
	{
		return ERR_FAIL;
	}

	pstCurPosBuf = pSrcBuffer;

	pstBookingData->usOnId = (HINT16)get16bit (pstCurPosBuf); pstCurPosBuf += 2;
	pstBookingData->usTsId = (HINT16)get16bit (pstCurPosBuf); pstCurPosBuf += 2;
	pstBookingData->usSvcId = (HINT16)get16bit (pstCurPosBuf);	pstCurPosBuf += 2;

	pstBookingData->bEventIdFlag = (*pstCurPosBuf >> 7) & 0x01;

	if(pstBookingData->bEventIdFlag)
	{
		pstCurPosBuf += 2;
		pstBookingData->usEventId = (HINT16)get16bit (pstCurPosBuf);
		HxLOG_Print("event id [0x%04]\n", pstBookingData->usEventId);
	}
	else
	{
		pstBookingData->ucMonth = (*pstCurPosBuf >> 3) & 0x0F;
		pstBookingData->ucDay = ((*pstCurPosBuf << 2) & 0x1C) | ((*(pstCurPosBuf + 1) >> 6) & 0x03);			pstCurPosBuf += 1;
		pstBookingData->ucStartHour = (*pstCurPosBuf >> 1) & 0x1F;
		pstBookingData->ucStartMinute = ((*pstCurPosBuf << 5) & 0x20) | ((*(pstCurPosBuf + 1) >> 3) & 0x1F);	pstCurPosBuf += 1;
		pstBookingData->ucDurHour = ((*pstCurPosBuf << 2) & 0x1C) | ((*(pstCurPosBuf + 1) >> 6) & 0x03);		pstCurPosBuf += 1;
		pstBookingData->ucDurMinutes = *pstCurPosBuf & 0x3F;

		HxLOG_Print("%d/%d, %d:%d (%dh %dm)\n", pstBookingData->ucMonth, pstBookingData->ucDay, pstBookingData->ucStartHour,
			pstBookingData->ucStartMinute, pstBookingData->ucDurHour,pstBookingData->ucDurMinutes);
	}

	return ERR_OK;
}

#define _____FILE_FUNCTIONS_____

STATIC HERROR xsvc_download_FileLength_SatRecord(HUINT8 *pucFileName, HUINT32 *pulFileLength)
{
	HERROR				hError = ERR_FAIL;
	HxFILE_t				hFile;
	HUINT32				ulSize;

	if( pucFileName == NULL || pulFileLength == NULL)
	{
		return ERR_FAIL;
	}

	*pulFileLength = 0;


	hFile = HLIB_FILE_Open((const HUINT8 *)pucFileName,(const HUINT8 *) "r");
	if (hFile == NULL)
	{
		HxLOG_Error("[%s] HLIB_FILE_Open fail!!\n",__FUNCTION__);
		return ERR_FAIL;
	}
	hError = HLIB_FILE_Size32(hFile, &ulSize);
	if (hError != ERR_OK)
	{
		HxLOG_Error("[%s] HLIB_FILE_Size32 fail!!\n",__FUNCTION__);
		HLIB_FILE_Close(hFile);
		return ERR_FAIL;
	}
	HLIB_FILE_Close(hFile);

	*pulFileLength = ulSize;
	return ERR_OK;
}

STATIC HERROR xsvc_download_Fopen_SatRecord(HUINT8 *pszFileName, Handle_t *phFileHandle)
{
	HxFILE_t		hFile;

	if(pszFileName == NULL || phFileHandle == NULL)
	{
		HxLOG_Error("[xsvc_download_Fopen_SatRecord] Param NULL!!\n");
		return ERR_FAIL;
	}

	*phFileHandle = (Handle_t)NULL;

	hFile = HLIB_FILE_Open((const HUINT8 *)pszFileName, (const HUINT8 *)"r");
	if (hFile == NULL)
	{
		HxLOG_Error("[xsvc_download_Fopen_SatRecord] HLIB_FILE_Open fail!!\n");
		return ERR_FAIL;
	}

	*phFileHandle =(Handle_t)hFile ;
	return ERR_OK;
}

STATIC HERROR xsvc_download_Fread_SatRecord(void *pBuffer, HUINT32 ulUnitSize, Handle_t hFileHandle, HxFILE_Seek_e eMode, HUINT32 *pulReadSize, HUINT64 offset)
{
	HERROR			hError = ERR_FAIL;
	HUINT32 		ulReadSize;
	HxFILE_t		hFile = (HxFILE_t)hFileHandle;

	if(pBuffer == NULL || hFileHandle == (Handle_t)NULL || pulReadSize == NULL)
	{
		HxLOG_Error("[xsvc_download_Fread_SatRecord] Param NULL!!\n");
		return ERR_FAIL;
	}
	*pulReadSize = 0;

	hError = HLIB_FILE_Seek(hFile, offset, eMode, NULL);
	if (hError != ERR_OK )
	{
		HxLOG_Error("[xsvc_download_Fread_SatRecord] HLIB_FILE_Seek fail!!\n");
		return ERR_FAIL;
	}

	ulReadSize = HLIB_FILE_Read(hFile, (HUINT8*)pBuffer, 1, ulUnitSize);
	if (ulReadSize == 0)
	{
		HxLOG_Error("[xsvc_download_Fread_SatRecord] HLIB_FILE_Read fail!!\n");
		return ERR_DOWN_SOURCE_NULL;
	}

	*pulReadSize = ulReadSize;
	return ERR_OK;
}

STATIC HERROR xsvc_download_Fclose_SatRecord(Handle_t hFileHandle)
{
	if( hFileHandle == (Handle_t)0)
	{
		HxLOG_Error("[xsvc_download_Fclose_SatRecord] Param NULL!!\n");
		return ERR_FAIL;
	}

	HLIB_FILE_Close((HxFILE_t)hFileHandle);
	return ERR_OK;
}

#define _____LIBDSMCC_FUNCTIONS____
STATIC char s_szSatRecord_FileFullName[256];

STATIC HERROR xsvc_download_CorrectFilePathString_SatRecord (HCHAR *szPath)
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

		if (pucDst != pucSrc)
		{
			*pucDst = *pucSrc;
		}

		bSlashed = FALSE;
		pucDst++;
	}

	return ERR_OK;

}

STATIC int xsvc_download_WriteFileCallback_SatRecord (char *szFileName, char *szFilePath, char *szChanName, int nPid, int nFileSize, unsigned char *pucFileBuffer)
{
	HINT32		 nCount;
	HCHAR		*szDirPath = NULL;
	HxFILE_t		pDataFile;

	HxLOG_Print ("[%s:%d] (+)\n", __FUNCTION__, __LINE__);

	// Index를 PID Buffer에서 찾아낸다.
	if (pucFileBuffer == NULL || nFileSize <= 0)
	{
		return ERR_FAIL;
	}

	if (szFileName == NULL || szFileName[0] == '\0')
	{
		return ERR_FAIL;
	}

	HxLOG_Debug ("[%s][%s][%s](%d)\n", szFilePath, szFileName, szChanName, nFileSize);

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
	HxSTD_PrintToStrN (s_szSatRecord_FileFullName, 256, "%s", szDirPath);

	if (HLIB_STD_StrEndWith((const HCHAR *)szDirPath, (const HCHAR *)"/") == FALSE)
	{
		HxSTD_StrNCat (s_szSatRecord_FileFullName, "/", 255);
	}

	HxLOG_Debug("Default Path [%s] >>>\n", s_szSatRecord_FileFullName);

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

			HxSTD_StrNCat (s_szSatRecord_FileFullName, pszNewFilePath, 255);

			HxLOG_Debug("Default Path + File Path [%s] >>>\n", s_szSatRecord_FileFullName);
		}

		if (HLIB_STD_StrEndWith((const HCHAR *)s_szSatRecord_FileFullName, (const HCHAR *)"/") == FALSE)
		{
			HxSTD_StrNCat (s_szSatRecord_FileFullName, "/", 255);
			HxLOG_Debug("Add \"/\" at the end of string [%s] >>>\n", s_szSatRecord_FileFullName);
		}
	}

	HxSTD_StrNCat (s_szSatRecord_FileFullName, szFileName, 255);
	HxLOG_Debug("DSMCC Full File Name [%s] >>>\n", s_szSatRecord_FileFullName);

	xsvc_download_CorrectFilePathString_SatRecord (s_szSatRecord_FileFullName);
	HxLOG_Debug("DSMCC Full File Name After Correction [%s] >>>\n", s_szSatRecord_FileFullName);

	pDataFile = HLIB_FILE_Open((const HUINT8 *)s_szSatRecord_FileFullName, (const HUINT8 *)"wb");
	if (pDataFile != NULL)
	{
		HLIB_FILE_Write(pDataFile, pucFileBuffer, 1, nFileSize);
		HLIB_FILE_Close(pDataFile);

		xsvc_download_SetFileInfoList_SatRecord((HUINT16)nPid, pucFileBuffer[0], nFileSize, s_szSatRecord_FileFullName, szFileName);
	}

	HxLOG_Print ("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

	return ERR_OK;
}

STATIC int xsvc_download_WriteDirCallback_SatRecord (char *szDirName, char *szDirPath, char *szChanName, int nPid)
{
	HINT32		 nCount;
	HCHAR		*szUpperDirPath = NULL;

	HxLOG_Print ("[%s:%d] (+)\n", __FUNCTION__, __LINE__);
	HxLOG_Debug ("[%s][%s][%s]\n", szDirPath, szDirName, szChanName);

	for (nCount = 0; nCount < MAX_FILTERING_NUM; nCount++)
	{
		if ((int)s_stPidBuffer[nCount].usPid == nPid)
		{
			szUpperDirPath = s_stPidBuffer[nCount].szStoragePath;
			break;
		}
	}

	HxSTD_PrintToStrN (s_szSatRecord_FileFullName, 256, "%s/", szUpperDirPath);

	if (szDirPath != NULL && szDirPath[0] != '\0')
	{
		HxSTD_StrNCat (s_szSatRecord_FileFullName, szDirPath, 255);
		HxSTD_StrNCat (s_szSatRecord_FileFullName, "/", 255);
	}

	xsvc_download_CorrectFilePathString_SatRecord (s_szSatRecord_FileFullName);
	HLIB_DIR_Make(s_szSatRecord_FileFullName);

	HxLOG_Print ("[%s:%d] (-)\n", __FUNCTION__, __LINE__);
	return ERR_OK;
}

STATIC void xsvc_download_CompleteCallback_SatRecord(HINT32 nPid, ObjectCarousel_t *car)
{
	XsvcDownload_DsmccDnInfo_t			*pstDnInfo	= NULL;
	HUINT32				ulInfoIdx	= INVALID_INFO_INDEX;
	HUINT32				i		= 0;

	HxLOG_Print ("[%s:%d] (+) PID (0x%04)\n", __FUNCTION__, __LINE__, nPid);
	if (car != NULL)
	{
		HxLOG_Debug ("Carousel ID (%d)\n", car->downloadId_inObjectCarousel);
	}

	/* parser->libdsmcc->callback notification 구조로 ENTER_CRITICAL_DNLDCNTS는 사용할 필요 없다. */
	pstDnInfo = xsvc_download_GetDownloadInfo_SatRecord (s_pstContents); /* Init 시점에서 설정 */
	if (pstDnInfo != NULL)
	{
		for (i = 0 ; i < MAX_FILTERING_NUM ; i++)
		{
			if (pstDnInfo[i].usPid == nPid && pstDnInfo[i].usPid != 0)
			{
				ulInfoIdx = i;
				break;
			}
		}

		if (ulInfoIdx != INVALID_INFO_INDEX)
		{
			pstDnInfo[ulInfoIdx].bDdbAllReceived = TRUE;
			svc_download_SendSectionMessage (DOWNCB_DDB_TABLE, s_pstContents->hAction, eSI_TABLE_RECEIVED, (HUINT32)pstDnInfo[ulInfoIdx].usDdbFilterId);
		}
	}
	else
	{
		HxLOG_Error ("Error:\n");
	}

	HxLOG_Print ("[%s:%d] (-)\n", __FUNCTION__, __LINE__);
}

STATIC HERROR xsvc_download_ClearLibResource_SatRecord ( SvcDownload_DsmccContents_t *pstContents, unsigned int uiIndex )
{
	HUINT32				f_ResourceIsFree = 0;
	XsvcDownload_DsmccDnInfo_t		*pstDnInfo	= NULL;

	HxLOG_Print ("[%s:%d] (+)\n", __FUNCTION__, __LINE__);

	pstDnInfo = xsvc_download_GetDownloadInfo_SatRecord (pstContents);

	VK_SEM_Get( pstDnInfo[uiIndex].uiSemaForResource );
	if ( pstDnInfo[uiIndex].pstDsmccStatus != NULL)
	{
		dsmcc_close( pstDnInfo[uiIndex].pstDsmccStatus );
		pstDnInfo[uiIndex].pstDsmccStatus = NULL;
		f_ResourceIsFree = 1;
	}
	VK_SEM_Release( pstDnInfo[uiIndex].uiSemaForResource );

	if ( f_ResourceIsFree )
	{
		if ( pstDnInfo[uiIndex].uiSemaForResource != (unsigned long)NULL )
		{
			if ( VK_SEM_Destroy( pstDnInfo[uiIndex].uiSemaForResource ) != ERR_OK )
			{
				HxLOG_Error ("VK_SEM_Destroy failed\n");
			}

			pstDnInfo[uiIndex].uiSemaForResource = (unsigned long)NULL;
		}
	}

	HxLOG_Print ("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

	return ERR_DOWN_RESULT_OK;
}
#define _____SECTION_PARSE_____

STATIC void xsvc_download_ParseDsi_SatRecord (HUINT8 *pucRaw, Handle_t hAction, HUINT32 ulFilterId)
{
	HUINT32					i = 0, ulInfoIdx = INVALID_INFO_INDEX, ulCrc32;
	HINT32			 		nSecLen;
	SvcDownload_DsmccContents_t	*pstContents;
	XsvcDownload_DsmccDnInfo_t			*pstDnInfo;
	HERROR					hErr;

	pstContents = NULL;

	ENTER_CRITICAL_DNLDCNTS;

	HxLOG_Print ("[%s:%d] (+) FilterId(%d)\n", __FUNCTION__, __LINE__, ulFilterId);

	hErr = svc_download_GetContextData (hAction, (void **)(&pstContents));
	if (hErr != ERR_OK || pstContents == NULL)
	{
		HxLOG_Error ("Error:\n");
		goto Error;
	}

	pstDnInfo = xsvc_download_GetDownloadInfo_SatRecord (pstContents);
	if (pstDnInfo == NULL)
	{
		HxLOG_Error ("Error:\n");
		goto Error;
	}

	for (i = 0 ; i < MAX_FILTERING_NUM ; i++)
	{
		if (pstDnInfo[i].usDsiFilterId == ulFilterId)
		{
			ulInfoIdx = i;
			break;
		}
	}

	if (ulInfoIdx == INVALID_INFO_INDEX)
	{
		HxLOG_Error ("Error:\n");
		goto Error;
	}

	nSecLen = (HINT32)get12bit (pucRaw + 1) + 3;
	ulCrc32 = (HUINT32)get32bit (pucRaw + nSecLen - 4);

	switch (pstDnInfo[ulInfoIdx].eDsiMode)
	{
		case eDSI_MODE_RECEIVING:
			if (pstDnInfo[ulInfoIdx].bDsiParsed == TRUE)
			{
				goto Error;
			}

#ifdef __USE_LIBDSMCC__ /* by Dylan - libdsmcc */
			VK_SEM_Get( pstDnInfo[ulInfoIdx].uiSemaForResource );
			if( pstDnInfo[ulInfoIdx].pstDsmccStatus != NULL )
			{
				dsmcc_receive_fromdata( pstDnInfo[ulInfoIdx].pstDsmccStatus, pucRaw, nSecLen, pstDnInfo[ulInfoIdx].usPid );
			}
			VK_SEM_Release( pstDnInfo[ulInfoIdx].uiSemaForResource );
#endif
			pstDnInfo[ulInfoIdx].bDsiParsed = TRUE;
			pstDnInfo[ulInfoIdx].ulDsiCrc32 = ulCrc32;
			break;

		case eDSI_MODE_MONITORING:
			if (pstDnInfo[ulInfoIdx].ulDsiCrc32 != ulCrc32)
			{
				HxLOG_Print("[%s:%d] DSI TABLE IS CHANGED in DSI. Origin CRC(0x%x) -> New CRC(0x%x)!!!!\n", __FUNCTION__, __LINE__, pstDnInfo[ulInfoIdx].ulDiiCrc32, ulCrc32);
				svc_download_SendSectionMessage (DOWNCB_DSI_TABLE, hAction, eSI_TABLE_CHANGED, (HUINT32)pstDnInfo[ulInfoIdx].usDsiFilterId);
			}
			break;

		default:
			break;
	}

	HxLOG_Print ("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

Error:
	LEAVE_CRITICAL_DNLDCNTS;

	return;
}

STATIC void xsvc_download_ParseDii_SatRecord (HUINT8 *pucRaw, Handle_t hAction, HUINT32 ulFilterId)
{
	HUINT32					i = 0, ulInfoIdx = INVALID_INFO_INDEX, ulCrc32;
	HINT32			 		nSecLen;
	SvcDownload_DsmccContents_t	*pstContents;
	XsvcDownload_DsmccDnInfo_t			*pstDnInfo;
	HERROR					hErr;

	HUINT32					ulCnt = 0;
	HBOOL					bGotDsi = FALSE;

	pstContents = NULL;

	ENTER_CRITICAL_DNLDCNTS;

	HxLOG_Print ("[%s:%d] (+) FilterID(%d)\n", __FUNCTION__, __LINE__, ulFilterId);

	hErr = svc_download_GetContextData (hAction, (void **)(&pstContents));
	if (hErr != ERR_OK || pstContents == NULL)
	{
		HxLOG_Error ("Error:\n");
		goto Error;
	}

	pstDnInfo = xsvc_download_GetDownloadInfo_SatRecord (pstContents);
	if (pstDnInfo == NULL)
	{
		HxLOG_Error ("Error:\n");
		goto Error;
	}

	/* 두개 이상의 DSMCC PID를 거는 경우, DSI는 sharing된다.
	     이경우 처리가 되지 않아 DDB를 받을 수 없는 문제 해결임. By MOONSG */
	for (i = 0 ; i < MAX_FILTERING_NUM ; i++)
	{
		if (pstDnInfo[i].usDsiFilterId != 0)
			ulCnt++;

		if ( (bGotDsi == FALSE) && (pstDnInfo[i].bDsiParsed == TRUE) )
			bGotDsi = TRUE;
	}

	if (ulCnt == 0)
	{
		HxLOG_Error ("Error:\n");
		goto Error;
	}

	for (i = 0 ; i < MAX_FILTERING_NUM ; i++)
	{
		if (pstDnInfo[i].usDsiFilterId == ulFilterId)
		{
			ulInfoIdx = i;
			break;
		}
	}

	if (ulInfoIdx == INVALID_INFO_INDEX)
	{
		HxLOG_Error ("Error:\n");
		goto Error;
	}

	nSecLen = (HINT32)get12bit (pucRaw + 1) + 3;
	ulCrc32 = (HUINT32)get32bit (pucRaw + nSecLen - 4);

	switch (pstDnInfo[ulInfoIdx].eDsiMode)
	{
		case eDSI_MODE_RECEIVING:
			if (pstDnInfo[ulInfoIdx].bDsiParsed == FALSE)
			{
				if ( (ulCnt < 2) || (bGotDsi == FALSE) )
					goto Error;
			}

			if (pstDnInfo[ulInfoIdx].bDiiParsed == TRUE)
			{
				goto Error;
			}

#ifdef __USE_LIBDSMCC__ /* by Dylan - libdsmcc */
			VK_SEM_Get( pstDnInfo[ulInfoIdx].uiSemaForResource );
			if( pstDnInfo[ulInfoIdx].pstDsmccStatus != NULL )
			{
				dsmcc_receive_fromdata( pstDnInfo[ulInfoIdx].pstDsmccStatus, pucRaw, nSecLen, pstDnInfo[ulInfoIdx].usPid );
			}
			VK_SEM_Release( pstDnInfo[ulInfoIdx].uiSemaForResource );
#endif
			pstDnInfo[ulInfoIdx].bDiiParsed = TRUE;
			pstDnInfo[ulInfoIdx].ulDiiCrc32 = ulCrc32;
			svc_download_SendSectionMessage (DOWNCB_DSI_TABLE, hAction, eSI_TABLE_RECEIVED, (HUINT32)pstDnInfo[ulInfoIdx].usDsiFilterId);
			break;

		case eDSI_MODE_MONITORING:
			if (pstDnInfo[ulInfoIdx].ulDiiCrc32 != ulCrc32)
			{
				HxLOG_Print("[%s:%d] DSI TABLE IS CHANGED in DII. Origin CRC(0x%x) -> New CRC(%d)!!!!\n", __FUNCTION__, __LINE__, pstDnInfo[ulInfoIdx].ulDiiCrc32, ulCrc32);
				svc_download_SendSectionMessage (DOWNCB_DSI_TABLE, hAction, eSI_TABLE_CHANGED, (HUINT32)pstDnInfo[ulInfoIdx].usDsiFilterId);
			}
			break;

		default:
			break;
	}

	HxLOG_Print ("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

Error:
	LEAVE_CRITICAL_DNLDCNTS;

	return;
}

STATIC void xsvc_download_ParseDdb_SatRecord (HUINT8 *pucRaw, Handle_t hAction, HUINT32 ulFilterId)
{
	HUINT32					i = 0, ulInfoIdx = INVALID_INFO_INDEX;
	HINT32					nSecLen;
	SvcDownload_DsmccContents_t	*pstContents;
	XsvcDownload_DsmccDnInfo_t			*pstDnInfo;
	HERROR					hErr;

	pstContents = NULL;

	ENTER_CRITICAL_DNLDCNTS;

	HxLOG_Print ("[%s:%d] (+) FilterId(%d)\n", __FUNCTION__, __LINE__, ulFilterId);

	hErr = svc_download_GetContextData (hAction, (void **)(&pstContents));
	if (hErr != ERR_OK || pstContents == NULL)
	{
		HxLOG_Error ("Error:\n");
		goto Error;
	}

	pstDnInfo = xsvc_download_GetDownloadInfo_SatRecord (pstContents);
	if (pstDnInfo == NULL)
	{
		HxLOG_Error ("Error:\n");
		goto Error;
	}

	for (i = 0 ; i < MAX_FILTERING_NUM ; i++)
	{
		if (pstDnInfo[i].usDdbFilterId == ulFilterId)
		{
			ulInfoIdx = i;
			break;
		}
	}

	if (ulInfoIdx == INVALID_INFO_INDEX)
	{
		HxLOG_Error ("Error:\n");
		goto Error;
	}

	/* TODO : Set the dsmcc info */
	if (pstDnInfo[ulInfoIdx].bDdbAllReceived == TRUE)
	{
		HxLOG_Error ("Error:\n");
		goto Error;
	}

	nSecLen = (HINT32)get12bit (pucRaw + 1);

	if (nSecLen < DDB_HEADER_LEN)
	{
		return;
	}

#ifdef __USE_LIBDSMCC__ /* by Dylan - libdsmcc */
	VK_SEM_Get( pstDnInfo[ulInfoIdx].uiSemaForResource );
	if( pstDnInfo[ulInfoIdx].pstDsmccStatus != NULL )
	{
		dsmcc_receive_fromdata( pstDnInfo[ulInfoIdx].pstDsmccStatus, pucRaw, nSecLen, pstDnInfo[ulInfoIdx].usPid );
	}
	VK_SEM_Release( pstDnInfo[ulInfoIdx].uiSemaForResource );
#endif

	HxLOG_Print ("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

Error:

	LEAVE_CRITICAL_DNLDCNTS;

	return;
}

#define _____SECTION_CALLBACK_____

STATIC void xsvc_download_CallbackDsiDii_SatRecord (HUINT32 ulTableIdAndResult, HUINT32 ulfilterAndUniqId, HUINT32 ulParam, Handle_t hAction)
{
	HUINT16		 	usMessageId	= 0;
	HUINT32		 	ulResult, ulFilterId;
	HUINT8			*pucRaw;
	PalSef_RawData_t		*pstSiRawData;

	HxLOG_Print ("[%s:%d] (+)\n", __FUNCTION__, __LINE__);

	ulResult = ulTableIdAndResult & 0x0000ffff;
	ulFilterId = (ulfilterAndUniqId & 0xffff0000) >> 16;

	if (ulResult != (HUINT32)eSiFilterResult_Complete)
	{
		HxLOG_Error ("Error:\n");
		svc_download_SendSectionMessage (DOWNCB_DSI_TABLE, hAction, eSI_TABLE_FAIL, ulFilterId);
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

	usMessageId = get16bit (pucRaw + SECTION_MESSAGEID_POS);

	if (usMessageId == DSI_MESSAGE_ID)
	{
		HxLOG_Debug ("DSI RECEIVED, MSGID(0x%04x)\n", usMessageId);
		xsvc_download_ParseDsi_SatRecord (pucRaw, hAction, ulFilterId);
	}
	else if (usMessageId == DII_MESSAGE_ID)
	{
		HxLOG_Debug ("DII RECEIVED, MSGID(0x%04x)\n", usMessageId);
		xsvc_download_ParseDii_SatRecord (pucRaw, hAction, ulFilterId);
	}
	else
	{
		HxLOG_Error ("Unknown RECEIVED, MSGID(0x%04x)\n", usMessageId);
	}

	HxLOG_Print ("[%s:%d] (-)\n", __FUNCTION__, __LINE__);
	return;
}

STATIC void xsvc_download_CallbackDdb_SatRecord (HUINT32 ulTableIdAndResult, HUINT32 ulfilterAndUniqId, HUINT32 ulParam, Handle_t hAction)
{
	HUINT16			 usMessageId;
	HUINT32			 ulResult, ulFilterId;
	HUINT8			*pucRaw;
	PalSef_RawData_t		*pstSiRawData;

	HxLOG_Print ("[%s:%d] (+)\n", __FUNCTION__, __LINE__);

	ulResult = ulTableIdAndResult & 0x0000ffff;
	ulFilterId = (ulfilterAndUniqId & 0xffff0000) >> 16;

	if (ulResult != (HUINT32)eSiFilterResult_Complete)
	{
		HxLOG_Error ("Error:\n");
		svc_download_SendSectionMessage (DOWNCB_DDB_TABLE, hAction, eSI_TABLE_FAIL, ulFilterId);
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

	usMessageId = get16bit (pucRaw + SECTION_MESSAGEID_POS);

	if (usMessageId == DDB_MESSAGE_ID)
	{
		HxLOG_Debug ("DDB RECEIVED, MSGID(0x%04x)\n", usMessageId);
		xsvc_download_ParseDdb_SatRecord (pucRaw, hAction, ulFilterId);
	}
	else
	{
		HxLOG_Error ("Error:\n");
	}

	HxLOG_Print ("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

	return;
}

#define _____FILTERING_FUNCTIONS_____
STATIC HERROR xsvc_download_StartDsiDiiFiltering_SatRecord (SvcDownload_DsmccContents_t *pstContents, SvcDownload_Msg_t *pstMsg )
{
	SiRetrieveParam_t		stParam;
	XsvcDownload_DsmccDnInfo_t			*pstDnInfo	= NULL;
	HERROR					hErr, hRtnErr		= ERR_DOWN_RESULT_OK;
	HUINT16 			usFilterId	= 0;
	HUINT32 			i		= 0;

	if (pstContents == NULL || pstMsg == NULL )
	{
		hRtnErr = ERR_DOWN_TARGET_NULL;
	}
	else
	{
		HxLOG_Print ("[%s:%d] PID 0x%04x\n", __FUNCTION__, __LINE__, pstMsg->unArg.stStartDown.usPid);

		pstDnInfo = xsvc_download_GetDownloadInfo_SatRecord(pstContents);
		if (pstDnInfo == NULL)
		{
			hRtnErr = ERR_DOWN_TARGET_NULL;
		}
		else
		{
			HxSTD_memset (&stParam, 0, sizeof(SiRetrieveParam_t));
			for (i = 0 ; i < MAX_FILTERING_NUM ; i ++)
			{
				if ( pstDnInfo[i].usPid == pstMsg->unArg.stStartDown.usPid && pstDnInfo[i].usDsiFilterId == FILTER_ID_NULL )
				{
					stParam.hAction 	= pstContents->hAction;
					stParam.usTsUniqId	= pstContents->usTsUniqId;
					stParam.ucDemuxId	= (HUINT8)pstContents->ulDemuxId;

					stParam.usPid		= pstDnInfo[i].usPid;
					stParam.ucMode		= eSiFilterMode_Continue;
					stParam.bTableFilter	= FALSE;
					stParam.bUseExtId	= FALSE;

					pstDnInfo[i].eDsiMode	= eDSI_MODE_RECEIVING;

					hErr = xsvc_download_DmxRetrieveDsiSection(&stParam, (void *)xsvc_download_CallbackDsiDii_SatRecord, NULL, &usFilterId);
					if (hErr == ERR_OK)
					{
						pstDnInfo[i].usDsiFilterId = usFilterId;
					}
					else
					{
						pstDnInfo[i].usDsiFilterId = FILTER_ID_NULL;
					}

					HxLOG_Debug ("(%d)-(%04x)(%04x)\n", i, pstDnInfo[i].usPid, pstDnInfo[i].usDsiFilterId);
					break;
				}
			}
		}
	}

	HxLOG_Print ("[%s:%d] (-) Return Err:0x%04x\n", __FUNCTION__, __LINE__, hRtnErr);

	return hRtnErr;
}

STATIC HERROR xsvc_download_StartDdbFiltering_SatRecord (SvcDownload_DsmccContents_t *pstContents, HUINT32 ulIdx)
{
	HUINT16	 			usFilterId;
	SiRetrieveParam_t	 	stParam;
	XsvcDownload_DsmccDnInfo_t			*pstDnInfo;
	HERROR				hErr;
	HERROR				hRtnErr		= ERR_DOWN_RESULT_OK;

	HxLOG_Print ("[%s:%d] Index %d\n", __FUNCTION__, __LINE__, ulIdx);

	if (pstContents == NULL)
	{
		hRtnErr = ERR_DOWN_TARGET_NULL;
	}
	else
	{
		pstDnInfo = xsvc_download_GetDownloadInfo_SatRecord (pstContents);
		if (pstDnInfo == NULL)
		{
			hRtnErr = ERR_DOWN_TARGET_NULL;
		}
		else
		{
			HxSTD_memset (&stParam, 0, sizeof(SiRetrieveParam_t));
			stParam.hAction	 	= pstContents->hAction;
			stParam.usTsUniqId 	= pstContents->usTsUniqId;
			stParam.ucDemuxId 	= (HUINT8)pstContents->ulDemuxId;

			stParam.usPid	 	= pstDnInfo[ulIdx].usPid;
			stParam.ucMode	 	= eSiFilterMode_Continue;
			stParam.bTableFilter 	= FALSE;
			stParam.bUseExtId 	= FALSE;
			stParam.ucTimeoutMode = eContinueTimeout_Once;

			hErr = xsvc_download_DmxRetrieveDdbSection(&stParam, (void *)xsvc_download_CallbackDdb_SatRecord, NULL, &usFilterId);
			if (hErr == ERR_OK)
			{
				pstDnInfo[ulIdx].usDdbFilterId = usFilterId;
			}
			else
			{
				pstDnInfo[ulIdx].usDdbFilterId = FILTER_ID_NULL;
			}

			HxLOG_Debug ("(%d)-(%04x)(%04x)\n", ulIdx, pstDnInfo[ulIdx].usPid, pstDnInfo[ulIdx].usDdbFilterId);
		}
	}

	HxLOG_Print ("[%s:%d] (-) Return Err:0x%04x\n", __FUNCTION__, __LINE__, hRtnErr);

	return hRtnErr;
}

STATIC HERROR xsvc_download_StartDownloadDdb_SatRecord (SvcDownload_DsmccContents_t *pstContents, SvcDownload_Msg_t *pstMsg)
{
	HBOOL			bReady = FALSE;
	XsvcDownload_DsmccDnInfo_t	*pstDnInfo = NULL;
	HUINT32			i = 0, ulInfoIdx = INVALID_INFO_INDEX;

	HxLOG_Print ("[%s:%d] (+) DSI/DII ulFilterId(%d)\n", __FUNCTION__, __LINE__, pstMsg->unArg.stSection.ulFilterId);

	pstDnInfo = xsvc_download_GetDownloadInfo_SatRecord (pstContents);
	if (pstDnInfo != NULL)
	{
		for (i = 0 ; i < MAX_FILTERING_NUM ; i ++)
		{
			/* xsvc_download_StartDownloadDdb_SatRecord API는 DSI recv 이후 호출되며 ulFilterId가 stSection param으로 전달된다. */
			if (pstDnInfo[i].usDsiFilterId == pstMsg->unArg.stSection.ulFilterId)
			{
				ulInfoIdx = i;
				bReady = TRUE;
				break;
			}
		}
	}

	if (bReady == TRUE)
	{
	// TODO: temp code
	#if 1
		pstDnInfo[ulInfoIdx].eDsiMode = eDSI_MODE_MONITORING;
	#else
		if ( pstDnInfo[i].usDsiFilterId != FILTER_ID_NULL )
		{
			xsvc_download_DmxStopOneSection(pstDnInfo[i].usDsiFilterId);
			pstDnInfo[i].usDsiFilterId = FILTER_ID_NULL;
		}
	#endif

		xsvc_download_StartDdbFiltering_SatRecord (pstContents, ulInfoIdx);
	}

#ifdef TRACE_FILTER
	for (i = 0 ; i < MAX_FILTERING_NUM ; i ++)
	{
		HxLOG_Debug ("Filter(%d) PID(%04X) DSI=%08x DDB=%08x\n", i, pstDnInfo[i].usPid, pstDnInfo[i].usDsiFilterId, pstDnInfo[i].usDdbFilterId);
	}
#endif

	HxLOG_Print ("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

	return ERR_OK;
}

STATIC HERROR xsvc_download_StopDsiDiiFiltering_SatRecord (SvcDownload_DsmccContents_t *pstContents, HUINT32 ulIdx, SvcDownload_Msg_t *pstMsg )
{
	XsvcDownload_DsmccDnInfo_t			*pstDnInfo 	= NULL;

	HxLOG_Print ("[%s:%d] (+)\n", __FUNCTION__, __LINE__);

	if (pstContents == NULL)
	{
		return ERR_DOWN_TARGET_NULL;
	}

	pstDnInfo = xsvc_download_GetDownloadInfo_SatRecord (pstContents);

	if ( pstMsg->ulMsgClass == CMD_DOWN_START ||
		 pstMsg->ulMsgClass == CMD_DOWN_STOP )
	{
		if ( ( pstDnInfo[ulIdx].usDsiFilterId != FILTER_ID_NULL ) &&
			 ( pstMsg->unArg.stStopDown.usPid != 0 ) && ( pstDnInfo[ulIdx].usPid == pstMsg->unArg.stStopDown.usPid ) )
		{
			HxLOG_Debug ("Stoped by PID(%d)-PID(%04x)FILTER(%08x)\n",
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
			HxLOG_Debug ("Stoped by PID(%d)-PID(%04x)FILTER(%08x)\n",
											pstMsg->unArg.stStopDown.usPid, pstDnInfo[ulIdx].usPid, pstDnInfo[ulIdx].usDsiFilterId);

			xsvc_download_DmxStopOneSection(pstDnInfo[ulIdx].usDsiFilterId);
			pstDnInfo[ulIdx].usDsiFilterId = FILTER_ID_NULL;
			return ERR_DOWN_RESULT_OK;
		}
	}

	HxLOG_Print ("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

	return ERR_DOWN_RESULT_FAIL;
}

STATIC HERROR xsvc_download_StopDdbFiltering_SatRecord (SvcDownload_DsmccContents_t *pstContents, HUINT32 ulIdx, SvcDownload_Msg_t *pstMsg )
{
	XsvcDownload_DsmccDnInfo_t			*pstDnInfo	= NULL;

	HxLOG_Print ("[%s:%d] (+)\n", __FUNCTION__, __LINE__);

	if (pstContents == NULL)
	{
		return ERR_DOWN_TARGET_NULL;
	}

	pstDnInfo = xsvc_download_GetDownloadInfo_SatRecord (pstContents);

	if ( pstMsg->ulMsgClass == CMD_DOWN_START ||
		 pstMsg->ulMsgClass == CMD_DOWN_STOP )
	{
		if ( ( pstDnInfo[ulIdx].usDdbFilterId != FILTER_ID_NULL ) &&
			 ( pstMsg->unArg.stStopDown.usPid != 0 ) && ( pstDnInfo[ulIdx].usPid == pstMsg->unArg.stStopDown.usPid ) )
		{
			HxLOG_Debug ("Stoped by PID(%d)-PID(%04x)FILTER(%08x)\n",
											pstMsg->unArg.stStopDown.usPid, pstDnInfo[ulIdx].usPid, pstDnInfo[ulIdx].usDdbFilterId);

			xsvc_download_DmxStopOneSection (pstDnInfo[ulIdx].usDdbFilterId);
			pstDnInfo[ulIdx].usDdbFilterId 	= FILTER_ID_NULL;
			return ERR_DOWN_RESULT_OK;
		}
	}

	if ( pstMsg->ulMsgClass == DOWNCB_DDB_TABLE )
	{
		if ( pstMsg->unArg.stSection.ulFilterId == pstDnInfo[ulIdx].usDdbFilterId && pstDnInfo[ulIdx].usDdbFilterId != 0 )
		{
			HxLOG_Debug ("Stoped by PID(%d)-PID(%04x)FILTER(%08x)\n",
											pstMsg->unArg.stStopDown.usPid, pstDnInfo[ulIdx].usPid, pstDnInfo[ulIdx].usDdbFilterId);

			xsvc_download_DmxStopOneSection (pstDnInfo[ulIdx].usDdbFilterId);
			pstDnInfo[ulIdx].usDdbFilterId 	= FILTER_ID_NULL;
			return ERR_DOWN_RESULT_OK;
		}
	}

	HxLOG_Print ("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

	return ERR_DOWN_RESULT_FAIL;
}

STATIC HERROR xsvc_download_StopFiltering_SatRecord (SvcDownload_DsmccContents_t *pstContents, SvcDownload_Msg_t *pstMsg )
{
	HUINT32			i = 0;
	XsvcDownload_DsmccDnInfo_t	*pstDnInfo	= NULL;
	HBOOL			 bStopAll = FALSE;

	HxLOG_Print ("[%s:%d] (+) StopAll(%d)\n", __FUNCTION__, __LINE__, (pstMsg->unArg.stStopDown.usPid == 0xFFFF) ? TRUE : FALSE);

	if (pstContents == NULL)
	{
		return ERR_DOWN_TARGET_NULL;
	}

	bStopAll = (pstMsg->unArg.stStopDown.usPid == 0xFFFF) ? TRUE : FALSE;

	pstDnInfo = xsvc_download_GetDownloadInfo_SatRecord (pstContents);

	for (i = 0 ; i < MAX_FILTERING_NUM ; i ++)
	{
		if ( bStopAll ) /* flag to stop all filters */
		{
			if( pstMsg->ulMsgClass == CMD_DOWN_START ||
		 		pstMsg->ulMsgClass == CMD_DOWN_STOP )
			{
				pstMsg->unArg.stStopDown.usPid = pstDnInfo[i].usPid;
			}
			else if( pstMsg->ulMsgClass == DOWNCB_DSI_TABLE )
			{
				pstMsg->unArg.stSection.ulFilterId = pstDnInfo[i].usDsiFilterId;
			}
			else if( pstMsg->ulMsgClass == DOWNCB_DDB_TABLE )
			{
				pstMsg->unArg.stSection.ulFilterId = pstDnInfo[i].usDdbFilterId;
			}
		}
		xsvc_download_StopDsiDiiFiltering_SatRecord (pstContents, i, pstMsg);
		xsvc_download_StopDdbFiltering_SatRecord (pstContents, i, pstMsg);
		if( pstDnInfo[i].usDsiFilterId == FILTER_ID_NULL &&
			pstDnInfo[i].usDdbFilterId == FILTER_ID_NULL )
		{
			xsvc_download_ClearDnInfo_SatRecord (pstContents, i, pstMsg);
			/* TODO : free the resource that was allocated for the libdsmcc but this call should be block if download wasn't start */
			xsvc_download_ClearLibResource_SatRecord( pstContents, i );
		}
	}

#ifdef TRACE_FILTER
	for (i = 0 ; i < MAX_FILTERING_NUM ; i ++)
	{
		HxLOG_Debug ("Filter(%d) PID(%04X) DSI=%08x DDB=%08x\n",
										i, pstDnInfo[i].usPid, pstDnInfo[i].usDsiFilterId, pstDnInfo[i].usDdbFilterId);
	}
#endif

	HxLOG_Print ("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

	return ERR_DOWN_RESULT_OK;
}


STATIC HERROR xsvc_download_RestartDsiDiiFilter_SatRecord (SvcDownload_DsmccContents_t *pstContents, SvcDownload_Msg_t *pstMsg )
{
	HUINT32			i = 0;
	XsvcDownload_DsmccDnInfo_t	*pstDnInfo	= NULL;

	HxLOG_Print ("[%s:%d] (+) FilterID(%d)\n", __FUNCTION__, __LINE__, pstMsg->unArg.stSection.ulFilterId);

	pstDnInfo = xsvc_download_GetDownloadInfo_SatRecord (pstContents);

	for( i = 0; i < MAX_FILTERING_NUM; i++ )
	{
		if ( pstDnInfo[i].usDsiFilterId == pstMsg->unArg.stSection.ulFilterId )
		{
			break;
		}
	}

	if ( i == MAX_FILTERING_NUM )
	{
		HxLOG_Error ("Error:\n");
		return ERR_DOWN_RESULT_FAIL;
	}

	if ( pstDnInfo[i].usDsiFilterId != FILTER_ID_NULL )
	{
		xsvc_download_DmxStopOneSection (pstDnInfo[i].usDsiFilterId);
		pstDnInfo[i].usDsiFilterId = FILTER_ID_NULL;
	}

	if ( pstDnInfo[i].usDdbFilterId != FILTER_ID_NULL )
	{
		xsvc_download_DmxStopOneSection (pstDnInfo[i].usDdbFilterId);
		pstDnInfo[i].usDdbFilterId = FILTER_ID_NULL;
	}

#ifdef __USE_LIBDSMCC__ /* by Dylan - libdsmcc */
	xsvc_download_ClearLibResource_SatRecord( pstContents, i );


	/* ssyoo : if the library resource is cleared, the semaphore is also cleared. so we have to set semaphore again */
	if ( pstDnInfo[i].uiSemaForResource == (unsigned long)NULL )
	{
		if ( VK_SEM_Create( &pstDnInfo[i].uiSemaForResource, "SemLibdsmcc", VK_SUSPENDTYPE_PRIORITY ) != ERR_OK )
		{
			HxLOG_Print ("[%s:%d] (-) VK_SEM_Create failed\n", __FUNCTION__, __LINE__);
			return ERR_FAIL;
		}
	}
#endif
	svc_download_UtilPostMsgToMgr (eSEVT_DOWN_DOWNLOAD_CHANGED, pstContents->hAction, (HUINT32)pstContents->eActType, (HINT32)(pstDnInfo[i].usPid), (HINT32)pstDnInfo[i].uiCarouselId);

	pstDnInfo[i].usPid = 0;
#ifdef TRACE_FILTER
	for (i = 0 ; i < MAX_FILTERING_NUM ; i ++)
	{
		HxLOG_Debug ("Filter(%d) PID(%04X) DSI=%08x DDB=%08x\n",
										i, pstDnInfo[i].usPid, pstDnInfo[i].usDsiFilterId, pstDnInfo[i].usDdbFilterId);
	}
#endif

	HxLOG_Print ("[%s:%d] (-) FilterID(%d)\n", __FUNCTION__, __LINE__, pstMsg->unArg.stSection.ulFilterId);

	return ERR_DOWN_RESULT_OK;
}

#define _____START_FUNCTIONS_____
STATIC HERROR xsvc_download_AddPidOnContentTable_SatRecord (XsvcDownload_DsmccDnInfo_t	*pstDnInfo, SvcDownload_Msg_t *pstMsg )
{
	HUINT32  i;
	HCHAR	*szPath = NULL;
	FILE	*pDbgFp = NULL;

	HxLOG_Print ("[%s:%d] (+)\n", __FUNCTION__, __LINE__);

	if ( pstDnInfo == NULL || pstMsg == NULL )
	{
		return	ERR_DOWN_RESULT_FAIL;
	}

	for (i = 0 ; i < MAX_FILTERING_NUM ; i ++)
	{
		if ( pstDnInfo[i].usPid == pstMsg->unArg.stStartDown.usPid )
		{
			HxLOG_Print ("[%s:%d] (-) PID %04X is droped, table has this PID on (%d)\n", __FUNCTION__, __LINE__, pstMsg->unArg.stStartDown.usPid, i);
			return ERR_DOWN_RESULT_FAIL;
		}

		if (pstDnInfo[i].usPid == 0)
		{
			break;
		}
	}

	if( i == MAX_FILTERING_NUM )
	{
		HxLOG_Print ("[%s:%d] (-) PID %04X is droped, no slot\n", __FUNCTION__, __LINE__, pstMsg->unArg.stStartDown.usPid);
		return ERR_DOWN_RESULT_FAIL;
	}

	HxLOG_Print ("[%s:%d] PID %04X is added on (%d)\n", __FUNCTION__, __LINE__, pstMsg->unArg.stStartDown.usPid, i);

	pstDnInfo[i].usPid = pstMsg->unArg.stStartDown.usPid;
	pstDnInfo[i].usDsiFilterId = FILTER_ID_NULL;
	pstDnInfo[i].usDdbFilterId = FILTER_ID_NULL;
	pstDnInfo[i].uiCarouselId = pstMsg->unArg.stStartDown.ulExtId1;

	szPath = (HCHAR *)pstMsg->unArg.stStartDown.ulExtId3;

#ifdef __USE_LIBDSMCC__ /* by Dylan - libdsmcc */
	if ( pstDnInfo[i].uiSemaForResource == (unsigned long)NULL )
	{
		if ( VK_SEM_Create( &pstDnInfo[i].uiSemaForResource, "SemLibdsmcc", VK_SUSPENDTYPE_PRIORITY ) != ERR_OK )
		{
			HxLOG_Print ("[%s:%d] (-) VK_SEM_Create failed\n", __FUNCTION__, __LINE__);
			return ERR_FAIL;
		}
	}

	VK_SEM_Get( pstDnInfo[i].uiSemaForResource );
	if ( pstDnInfo[i].pstDsmccStatus == NULL )
	{
		/* memset(s_ucaChannel, 0x0, 256);
		pDbgFp = fopen("dbg.out","a+"); */
		HxLOG_Print ("[%s:%d] init libdsmcc\n", __FUNCTION__, __LINE__);
		pstDnInfo[i].pstDsmccStatus = dsmcc_open ( (const char *)s_ucaChannel, pDbgFp,
												   xsvc_download_WriteFileCallback_SatRecord,
												   xsvc_download_WriteDirCallback_SatRecord,
												   NULL,
												   NULL,
												   xsvc_download_CompleteCallback_SatRecord);
	}
	VK_SEM_Release( pstDnInfo[i].uiSemaForResource );
#endif

	s_stPidBuffer[i].usPid = pstDnInfo[i].usPid;

	if ((NULL != szPath) && ('\0' != szPath[0]))
	{
		HLIB_STD_StrUtf8NCpy (s_stPidBuffer[i].szStoragePath, szPath, FILE_PATH_NAME_LENGTH);
	}
	else
	{
		// OCTO 1.0의 기존 코드에 따른 Path Naming으로 만든다.
		HxSTD_PrintToStrN (s_stPidBuffer[i].szStoragePath, FILE_PATH_NAME_LENGTH, "/tmp/cache/%u/%u", i, s_stPidBuffer[i].usPid);
	}

	s_stPidBuffer[i].szStoragePath[FILE_PATH_NAME_LENGTH - 1] = '\0';


	HxLOG_Debug ("(%u) - pid(%04X) path(%s)\n", i, s_stPidBuffer[i].usPid, s_stPidBuffer[i].szStoragePath);
	HxLOG_Print ("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

	return ERR_DOWN_RESULT_OK;
}

STATIC HERROR xsvc_download_SetDownloadContents_SatRecord (SvcDownload_DsmccContents_t *pstContents, SvcDownload_Msg_t *pstMsg)
{
	HERROR				hErr;
	HERROR				hRtnErr 	= ERR_DOWN_RESULT_OK;
	HINT32				nDmxId		= 0;
	HUINT32 			unActId 	= 0, i = 0;

	XsvcDownload_DsmccDnInfo_t		*pstDnInfo	= NULL;

	HxLOG_Print ("[%s:%d] (+)\n", __FUNCTION__, __LINE__);

	if ( pstContents == NULL )
	{
		return ERR_DOWN_TARGET_NULL;
	}

	if ( pstMsg == NULL )
	{
		return ERR_DOWN_SOURCE_NULL;
	}

	/* check msg has unvalid PID value */
	if ( pstMsg->unArg.stStartDown.usPid == 0)
	{
		return ERR_DOWN_RESULT_FAIL;
	}

	/* check that current contents table has downloading contents,
	   다운중인 컨텐츠가 있다면, hAction, eActType, usTsUniqId, usPid, ulDemuxId 를 변경하지 않고 PID만 추가 하고, demux필터링을 실행한다.
	   dsmcc의 백그라운드 채널에 대한 다운로드는 없다는 전제, ex ) 2 channel record */
	pstDnInfo = xsvc_download_GetDownloadInfo_SatRecord (pstContents);

	if ( pstDnInfo == NULL )
	{
		return ERR_DOWN_RESULT_FAIL;
	}

	for( i = 0; i < MAX_FILTERING_NUM ; i++ )
	{
		if ( pstDnInfo[i].usPid == 0 )
		{
			break;
		}
	}

	if ( i == MAX_FILTERING_NUM )
	{
		/* 여유 슬롯이 없음 */
		HxLOG_Print ("[%s:%d] (-) There is no free slot PID(0x%04X) is dropped\n", __FUNCTION__, __LINE__, pstMsg->unArg.stStartDown.usPid);
		return ERR_DOWN_RESULT_FAIL;
	}

	if ( i == 0 )
	{
		/* 다운중인 PID가 없으므로 hAction, eActType, usTsUniqId, usPid, ulDemuxId 설정 */
		unActId = PAL_PIPE_GetActionId (pstMsg->hActHandle);
		hErr = PAL_PIPE_GetResourceId (unActId, eRxRSCTYPE_DEMUX, (HUINT32 *)&nDmxId);
		if (hErr != ERR_OK)
		{
			HxLOG_Error ("Error:\n");
			HxLOG_Print ("[%s:%d] (+) PAL_PIPE_GetResourceId failed\n", __FUNCTION__, __LINE__);
			hRtnErr = ERR_DOWN_SOURCE_NULL;
		}
		else
		{
			pstContents->hAction		= pstMsg->hActHandle;
			pstContents->eActType		= (SvcDownload_ActionType_e)pstMsg->unArg.stStartDown.ulActType;
			pstContents->usTsUniqId 	= pstMsg->unArg.stStartDown.usTsUniqId;
			pstContents->usPid			= pstMsg->unArg.stStartDown.usPid;
			pstContents->ulDemuxId		= (HUINT32)nDmxId;

			HxLOG_Debug ("ActHandle (%08x), ActId (%u)\n", pstMsg->hActHandle, unActId);
			HxLOG_Debug ("ActType (%d)\n", pstContents->eActType);
			HxLOG_Debug ("DMX ID (%d)\n", pstContents->ulDemuxId);
			HxLOG_Debug ("TS ID (0x%04x)\n", pstContents->usTsUniqId);
		}
	}

	xsvc_download_AddPidOnContentTable_SatRecord( pstDnInfo, pstMsg );

#ifdef TRACE_FILTER
	for( i = 0; i < MAX_FILTERING_NUM ; i++ )
	{
		HxLOG_Debug ("Filter(%d) PID(%04X) DSI=%08x DDB=%08xn", i, pstDnInfo[i].usPid, pstDnInfo[i].usDsiFilterId, pstDnInfo[i].usDdbFilterId);
	}
#endif

	HxLOG_Print ("[%s:%d] (-) Ret:0x%08x\n", __FUNCTION__, __LINE__, hRtnErr);
	return hRtnErr;
}



void DOWN_SatRecord (SvcDownload_Msg_t *pstMsg, void *pvData)
{
	HERROR						hErr;
	HUINT32 					i = 0, ulCurnDdbCnt = 0, ulCurnDdbPid = 0, uiCarouselId = 0;
	SvcDownload_DsmccContents_t *pstContents	= NULL;
	XsvcDownload_DsmccDnInfo_t	*pstDnInfo	= NULL;

	if (pstMsg == NULL || pvData == NULL)
	{
		HxLOG_Error ("Error:\n");
		return ;
	}

	pstContents = (SvcDownload_DsmccContents_t *)pvData;

	switch (pstMsg->ulMsgClass)
	{
		case CMD_DOWN_START :
			HxLOG_Print ("[%s:%d] CMD_DOWN_START\n", __FUNCTION__, __LINE__);

			hErr = xsvc_download_SetDownloadContents_SatRecord (pstContents, pstMsg);

			if (hErr == ERR_DOWN_RESULT_OK)
			{
				xsvc_download_StartDsiDiiFiltering_SatRecord(pstContents, pstMsg);

#ifdef TRACE_FILTER
				pstDnInfo = xsvc_download_GetDownloadInfo_SatRecord (pstContents);
				for (i = 0 ; i < MAX_FILTERING_NUM ; i ++)
				{
					HxLOG_Debug ("Filter(%d) PID(%04X) DSI=%08x DDB=%08x\n", i, pstDnInfo[i].usPid, pstDnInfo[i].usDsiFilterId, pstDnInfo[i].usDdbFilterId);
				}
#endif
			}
			else
			{
				HxLOG_Print ("[%s:%d] Ignored new filter\n", __FUNCTION__, __LINE__);
			}

			break;

		case CMD_DOWN_STOP :
			HxLOG_Print ("[%s:%d] CMD_DOWN_STOP\n", __FUNCTION__, __LINE__);
			xsvc_download_StopFiltering_SatRecord (pstContents, pstMsg);
			break;

		case DOWNCB_DSI_TABLE:
			HxLOG_Print ("[%s:%d] DOWNCB_DSI_TABLE, TableStatus (0x%02x)\n", __FUNCTION__, __LINE__, pstMsg->unArg.stSection.ulTableStatus);
			if (pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_RECEIVED)
			{
				xsvc_download_StartDownloadDdb_SatRecord (pstContents, pstMsg);
			}
			else if (pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_CHANGED)
			{
				/* the version of DSI table is changed, so stop section filters with same filter ID.
				  then inform EVT_DOWN_DOWNLOAD_CHANGED to app. Ap will restart download routine :ssyoo */
				xsvc_download_RestartDsiDiiFilter_SatRecord (pstContents, pstMsg);
			}

			break;

			case DOWNCB_DDB_TABLE:
				HxLOG_Print ("[%s:%d] DOWNCB_DDB_TABLE, TableStatus (0x%02x)\n", __FUNCTION__, __LINE__, pstMsg->unArg.stSection.ulTableStatus);
				pstDnInfo = xsvc_download_GetDownloadInfo_SatRecord (pstContents);
				/* DDB filter의 경우 recieve fail/success 두경우 이므로 DOWNCB_DDB_TABLE 메시지 일 경우 DDB필터를 무조건 stop함.
				   *DDB섹션은 버전관리를 하지 않는다. DSI의 버전이 틀릴경우 전체가재시작됨. : ssyoo*/
				for (i = 0 ; i < MAX_FILTERING_NUM ; i ++)
				{
					if (pstDnInfo[i].usDdbFilterId == pstMsg->unArg.stSection.ulFilterId && pstDnInfo[i].usDdbFilterId != 0)
					{
						ulCurnDdbPid	= pstDnInfo[i].usPid;
						uiCarouselId = pstDnInfo[i].uiCarouselId;
						(void) xsvc_download_StopDdbFiltering_SatRecord (pstContents, i, pstMsg);
					}

					if (pstDnInfo[i].usDdbFilterId != FILTER_ID_NULL || pstDnInfo[i].usDsiFilterId != FILTER_ID_NULL)
					{
						ulCurnDdbCnt ++;
					}
				}

#ifdef TRACE_FILTER
				for (i = 0 ; i < MAX_FILTERING_NUM ; i ++)
				{
					HxLOG_Debug ("Filter(%d) PID(%04X) DSI=%08x DDB=%08x\n", i, pstDnInfo[i].usPid, pstDnInfo[i].usDsiFilterId, pstDnInfo[i].usDdbFilterId);
				}
#endif

				if (pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_RECEIVED)
				{
					/* TODO : Object Carousel Parsing */
					HxLOG_Print ("[%s:%d] DSMCC Download Complete : PID (%u)\n", __FUNCTION__, __LINE__, ulCurnDdbPid);
					svc_download_UtilPostMsgToMgr (eSEVT_DOWN_DOWNLOAD_SUCCESS, pstContents->hAction, pstContents->eActType, ulCurnDdbPid, uiCarouselId);

#ifdef __USE_LIBDSMCC__ /* by Dylan - libdsmcc */
					if ( ulCurnDdbCnt == 0)
					{
						for (i = 0 ; i < MAX_FILTERING_NUM ; i ++)
						{
							xsvc_download_ClearLibResource_SatRecord (pstContents, i);
						}
					}
#endif
				}

				break;

		default :
			break;
	}
}

#define _____GLOBAL_FUNCTIONS____
HERROR xsvc_download_InitAction_SatRecord (void **ppvAction, void **ppvData, void *pvSiMsg)
{
	STATIC HBOOL			 s_bFirst = TRUE;
	SvcDownload_DsmccContents_t *pstContents;
	XsvcDownload_DsmccDnInfo_t	*pstDnInfo;
	HERROR					 hErr;

	HxLOG_Print ("[%s:%d] (+)\n", __FUNCTION__, __LINE__);

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

	pstDnInfo = xsvc_download_AllocDownloadInfo_SatRecord();
	if (pstDnInfo == NULL)
	{
		HxLOG_Error ("Error:\n");
		return ERR_DOWN_MEM_ALLOC_FAIL;
	}

	hErr = svc_download_AllocDsmccContents(&pstContents);
	if (hErr != ERR_OK)
	{
		HxLOG_Error ("Error:\n");
		xsvc_download_FreeDownloadInfo_SatRecord (pstDnInfo);
		return ERR_DOWN_MEM_ALLOC_FAIL;
	}

	pstContents->hAction		= HANDLE_NULL;
	pstContents->ulDemuxId		= RxRSCID_NULL;
	pstContents->pvDownloadInfo = (void *)pstDnInfo;
	*ppvAction	= (void *)DOWN_SatRecord;
	*ppvData	= (void *)pstContents;
	s_pstContents = pstContents;	/* libdsmcc calback에서 사용 */

	return ERR_OK;
}

HERROR xsvc_download_TermAction_SatRecord (void **ppvAction, void **ppvData)
{
	SvcDownload_DsmccContents_t	*pstContents;

	HxLOG_Print ("[%s:%d] (+)\n", __FUNCTION__, __LINE__);

	if (ppvAction == NULL || ppvData == NULL)
	{
    	return ERR_DOWN_TARGET_NULL;
	}

	*ppvAction = NULL;

	if (*ppvData != NULL)
	{
		pstContents = (SvcDownload_DsmccContents_t *)(*ppvData);
		xsvc_download_FreeDsmccContainer_SatRecord (pstContents);
		svc_download_FreeDsmccContents (pstContents);
    	*ppvData = NULL;
	}

	HxLOG_Print ("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

	return ERR_DOWN_RESULT_OK;
}

HERROR xsvc_download_GetData_SatRecord (void *pvData, SvcDownload_DataType_e eDataType, HUINT32 *pulDataNum, void **ppvDataArray)
{
	HERROR							hErr;
	SvcDownload_DsmccContents_t 	*pstContents;

	if (pulDataNum == NULL || ppvDataArray == NULL)
	{
		return ERR_FAIL;
	}

	pstContents = (SvcDownload_DsmccContents_t *)pvData;
	if (pstContents == NULL)
	{
		return ERR_FAIL;
	}

	switch (eDataType)
	{
		case eDOWN_DATA_SATRECORD_VERSION:
			hErr = xsvc_download_GetVersionInfo_SatRecord(pstContents, pulDataNum, (HUINT8 **)ppvDataArray);
			break;

		case eDOWN_DATA_SATRECORD_POLLINGPERIOD:
			hErr = xsvc_downlaod_GetPollingInfo_SatRecord(pstContents, pulDataNum, (HUINT16 **)ppvDataArray);
			break;

		case eDOWN_DATA_SATRECORD_FASTBOOKINGDATA:
		case eDOWN_DATA_SATRECORD_BOOKINGDATA:
			hErr = xsvc_download_GetRecData_SatRecord(pstContents, eDataType, pulDataNum, (SvcDownload_SatRecData_t **)ppvDataArray);
			break;

		default:
			return ERR_FAIL;
	}

	return hErr;
}

HERROR xsvc_download_FreeData_SatRecord (SvcDownload_DataType_e eDataType, HUINT32 ulDataNum, void *pvDataArray)
{
	if (ulDataNum == 0 || pvDataArray == NULL)
	{
		return ERR_FAIL;
	}

	OxSI_Free (pvDataArray);
	return ERR_OK;
}

/*********************** End of File ******************************/

