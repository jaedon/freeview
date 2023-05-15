/**************************************************************************************/
/**
 * @file bpldsmcc.c
 *
 * DSM-CC Interfaces
 *
 **************************************************************************************
 **/
#include <hlib.h>
#include <apk.h>
#include "bpldsmcc.h"

/******************************************************************
	Define
******************************************************************/


/******************************************************************
	Typedef
******************************************************************/

typedef enum
{
	eCACHE_STREAM_EVT	= 0,
	eCACHE_STREAM_DESC_EVT,
	eCACHE_TYPE_MAX
} _BPL_DSMCC_CacheType_e;

#define	BPL_MAX_CACHE_PATH_LENGTH			1024

typedef struct
{
	HUINT32					ulCarouselId;
	HCHAR					szCachePath[BPL_MAX_CACHE_PATH_LENGTH];
} _BPL_DSMCC_CarouselInfo_t;

typedef struct
{
	Handle_t						 hDsmcc;
	HUINT32							 ulSessionId;
	HCHAR							 szDefaultCachePath[eDxDSMCC_MAX_PATH_LENGTH];
	BplDsmccListener_t				 pfnDsmccListener;
	HxList_t						*pstList;			// _BPL_DSMCC_CarouselInfo_t
} _BPL_DSMCC_Info_t;

STATIC _BPL_DSMCC_Info_t	s_stBplDsmccInfo = { 0, 0, };


/******************************************************************
	Static variables
******************************************************************/

/******************************************************************
	Static functions
******************************************************************/

STATIC void _bpl_dsmcc_ExecuteListener(eBplDsmccDownloadEvent eEvent, void *pvData)
{
	if (s_stBplDsmccInfo.pfnDsmccListener)
	{
		(* s_stBplDsmccInfo.pfnDsmccListener)(eEvent, pvData);
	}
}

STATIC HBOOL _bpl_dsmcc_map_CB_FindByCarouselId(void *pvUserData, void *pvListData)
{
	HUINT32						 ulCarouselId = (HUINT32)pvUserData;
	_BPL_DSMCC_CarouselInfo_t	*pstInfo = (_BPL_DSMCC_CarouselInfo_t *)pvListData;

	if (NULL == pvListData)
	{
		return FALSE;
	}

	ulCarouselId	= (HUINT32)pvUserData;
	pstInfo			= (_BPL_DSMCC_CarouselInfo_t *)pvListData;

	return (ulCarouselId == pstInfo->ulCarouselId) ? TRUE : FALSE;
}

STATIC HBOOL _bpl_dsmcc_map_FindInfoByCarouselId(HxList_t *pstList, HUINT32 ulCarouselId, _BPL_DSMCC_CarouselInfo_t **ppstInfo)
{
	HxList_t					*pstFound;

	if (NULL == pstList)
	{
		HxLOG_Error("[%s:%04d] NULL == pstList \n", __FUNCTION__, __LINE__);
		return FALSE;
	}

	pstFound = HLIB_LIST_FindEx(pstList, (const void *)&ulCarouselId, _bpl_dsmcc_map_CB_FindByCarouselId);

	if (ppstInfo)
	{
		*ppstInfo = (_BPL_DSMCC_CarouselInfo_t *)HLIB_LIST_Data(pstFound);
	}

	return (pstFound) ? TRUE : FALSE;
}

STATIC HERROR _bpl_dsmcc_map_Create(HxList_t **ppstList, HUINT32 ulCarouselId, _BPL_DSMCC_CarouselInfo_t **ppstInfo)
{
	_BPL_DSMCC_CarouselInfo_t		*pstInfo;

	if (NULL == ppstList)
	{
		HxLOG_Error("[%s:%04d] NULL == ppstList \n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	pstInfo = HLIB_STD_MemAlloc(sizeof(_BPL_DSMCC_CarouselInfo_t));
	if (NULL == pstInfo)
	{
		HxLOG_Error("[%s:%04d] NULL == pstInfo \n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	HxSTD_MemSet(pstInfo, 0, sizeof(_BPL_DSMCC_CarouselInfo_t));
	pstInfo->ulCarouselId = ulCarouselId;

	*ppstList = HLIB_LIST_Append(*ppstList, (void *)pstInfo);

	if (ppstInfo)
	{
		*ppstInfo = pstInfo;
	}

	return ERR_OK;
}

STATIC HERROR _bpl_dsmcc_map_Add(HxList_t **ppstInfoList, HUINT32 ulCarouselId, const HCHAR *pszCachePath)
{
	_BPL_DSMCC_CarouselInfo_t	*pstInfo;

	HxLOG_Debug("[%s:%04d] ulCarouselId [0x%08x] \n", __FUNCTION__, __LINE__, ulCarouselId);

	if (NULL == ppstInfoList)
	{
		HxLOG_Error("[%s:%04d] NULL == ppstInfoList \n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	pstInfo = NULL;
	_bpl_dsmcc_map_FindInfoByCarouselId(*ppstInfoList, ulCarouselId, &pstInfo);
	if (NULL == pstInfo)
	{
		_bpl_dsmcc_map_Create(ppstInfoList, ulCarouselId, &pstInfo);
	}

	if (NULL == pstInfo)
	{
		HxLOG_Error("[%s:%04d] NULL == pstInfo \n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	//HxLOG_Assert(pstInfo->ulCarouselId != ulCarouselId);

	if (pszCachePath)
	{
		HLIB_STD_StrNCpySafe(pstInfo->szCachePath, pszCachePath, BPL_MAX_CACHE_PATH_LENGTH);
	}
	else
	{
		pstInfo->szCachePath[0] = '\0';
	}

	HxLOG_Debug("[%s:%04d] pstInfo->ulCarouselId [0x%08x], ulCarouselId [0x%08x] >>> \n", __FUNCTION__, __LINE__, pstInfo->ulCarouselId, ulCarouselId);
	HxLOG_Debug("[%s:%04d] szCachePath [%s] \n", __FUNCTION__, __LINE__, pstInfo->szCachePath);

	return ERR_OK;
}

STATIC HERROR _bpl_dsmcc_map_Remove(HxList_t **ppstInfoList, HUINT32 ulCarouselId)
{
	_BPL_DSMCC_CarouselInfo_t		*pstInfo;

	HxLOG_Debug("[%s:%04d] ulCarouselId [0x%08x] \n", __FUNCTION__, __LINE__, ulCarouselId);

	if (NULL == ppstInfoList)
	{
		HxLOG_Error("[%s:%04d] NULL == ppstInfoList !!\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	pstInfo = NULL;
	_bpl_dsmcc_map_FindInfoByCarouselId(*ppstInfoList, ulCarouselId, &pstInfo);
	if (NULL == pstInfo)
	{
		HxLOG_Error("[%s:%04d] NULL == pstInfo \n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	*ppstInfoList = HLIB_LIST_Remove(*ppstInfoList, (void *)pstInfo);
	HLIB_STD_MemFree(pstInfo);

	return ERR_OK;
}

STATIC HERROR _bpl_dsmcc_map_FreeCarouselInfo(_BPL_DSMCC_CarouselInfo_t *pstInfo)
{
	// TODO: Do Something with Carousel Id
	// pstInfo->ulCarouselId

	return ERR_OK;
}

STATIC HERROR _bpl_dsmcc_map_Clear(HxList_t *pstInfoList)
{
	HxList_t					*pstList;
	_BPL_DSMCC_CarouselInfo_t	*pstInfo;

	pstList = HLIB_LIST_First(pstInfoList);

	while (pstList)
	{
		pstInfo = (_BPL_DSMCC_CarouselInfo_t *)HLIB_LIST_Data(pstList);
		if (pstInfo)
		{
			_bpl_dsmcc_map_FreeCarouselInfo(pstInfo);
			HLIB_STD_MemFree(pstInfo);
		}
		pstList = pstList->next;
	}

	HLIB_LIST_RemoveAll(pstInfoList);

	return ERR_OK;
}

STATIC HBOOL _bpl_dsmcc_stream_evt_CB_FindByEventName(void *pvUserData, void *pvListData)
{
	HCHAR					*pszEventName = (HCHAR *)pvUserData;
	BplDsmccStreamEvent_t	*pstEvent = (BplDsmccStreamEvent_t *)pvListData;

	if (NULL == pszEventName || NULL == pstEvent || NULL == pstEvent->szEventName)
	{
		return FALSE;
	}

	return (HxSTD_StrCmp(pszEventName, pstEvent->szEventName) == 0) ? TRUE : FALSE;
}

STATIC void _bpl_dsmcc_stream_evt_FreeItem(BplDsmccStreamEvent_t *pstItem)
{
	if (pstItem)
	{
		if (pstItem->szUrl)				HLIB_STD_MemFree(pstItem->szUrl);
		if (pstItem->szDescription)		HLIB_STD_MemFree(pstItem->szDescription);
		if (pstItem->szEventName)		HLIB_STD_MemFree(pstItem->szEventName);
		if (pstItem->pucObjInfoByte)	HLIB_STD_MemFree(pstItem->pucObjInfoByte);
	}
}

STATIC HERROR _bpl_dsmcc_carousel_list_Free(BplDsmccObjectCarousel_t *astList, unsigned int ulNum)
{
	HUINT32 	ulCount;

	HxLOG_Assert(astList);

	for (ulCount=0; ulCount<ulNum; ulCount++)
	{
		if (astList[ulCount].Es)
		{
			HLIB_STD_MemFree(astList[ulCount].Es);
		}
	}

	HLIB_STD_MemFree(astList);

	return ERR_OK;
}

STATIC HERROR _bpl_dsmcc_carousel_list_Convert(DxDSMCC_CarouselList_t *pstSrc, BplDsmccObjectCarousel_t **pastList, unsigned int *pulCount)
{
	HUINT32						 ulCount, ulIdx;
	HUINT32						 ulEsNum;
	DxDSMCC_StreamInfo_t	*pstEsInfo;
	BplDsmccObjectCarousel_t	*astDest = NULL;
	BplDsmccES_t				*pstEs;

	HxLOG_Assert(pastList);
	HxLOG_Assert(pulCount);

	*pastList = NULL;
	*pulCount = 0;

	HxLOG_Debug("[%s:%04d] ListNum [%d] >>> \n", __FUNCTION__, __LINE__, pstSrc->ulNum);
	if (pstSrc->ulNum)
	{
		astDest = (BplDsmccObjectCarousel_t *)HLIB_STD_MemAlloc(sizeof(BplDsmccObjectCarousel_t)*pstSrc->ulNum);
	}

	if (NULL == astDest)
	{
		HxLOG_Error("[%s:%04d] NULL == astDest \n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	HxSTD_MemSet(astDest, 0, sizeof(BplDsmccObjectCarousel_t)*pstSrc->ulNum);

	for (ulCount=0; ulCount < pstSrc->ulNum; ulCount++)
	{
		astDest[ulCount].ulCarouselId = pstSrc->pstCarouselInfo[ulCount].ulCarouselId;

		ulEsNum = pstSrc->pstCarouselInfo[ulCount].ulNum;

		if (ulEsNum > eDxDSMCC_MAX_ES_STREAM_NUM)
			ulEsNum = eDxDSMCC_MAX_ES_STREAM_NUM;

		astDest[ulCount].Es = (BplDsmccES_t *)HLIB_STD_MemAlloc(sizeof(BplDsmccES_t)*ulEsNum);
		if (astDest[ulCount].Es)
		{
			HxSTD_MemSet(astDest[ulCount].Es, 0, sizeof(BplDsmccES_t)*ulEsNum);

			for (ulIdx=0; ulIdx<ulEsNum; ulIdx++)
			{
				pstEs = &astDest[ulCount].Es[ulIdx];
				pstEsInfo = &pstSrc->pstCarouselInfo[ulCount].astEsInfo[ulIdx];

				pstEs->ucComponentTag	= pstEsInfo->ucComponentTag;
				pstEs->ulStreamType		= pstEsInfo->ucStreamType;
			}
			astDest[ulCount].uiEsCount = ulEsNum;
		}
	}

	*pastList = astDest;
	*pulCount = pstSrc->ulNum;

	return ERR_OK;
}

STATIC HERROR _bpl_dsmcc_carousel_list_Get(BplDsmccObjectCarousel_t **pastList, unsigned int *pulCount)
{
	DxDSMCC_CarouselList_t	stList;
	HERROR				 			hErr;

	HxLOG_Assert(pastList);
	HxLOG_Assert(pulCount);

	stList.pstCarouselInfo = NULL;
	stList.ulNum = 0;

//	HxLOG_Debug("[%s:%04d] APK_DL_DSMCC_GetCarouselList(%d) (+) >>> \n", __FUNCTION__, __LINE__, s_stBplDsmccInfo.hDsmcc);
	hErr = APK_DSMCC_GetCarouselList(s_stBplDsmccInfo.hDsmcc, &stList);
//	HxLOG_Debug("[%s:%04d] APK_DSMCC_GetCarouselList() (-) >>> \n", __FUNCTION__, __LINE__);
	if (ERR_OK == hErr)
	{
		if ((0 == stList.ulNum) || (NULL == stList.pstCarouselInfo))
		{
			HxLOG_Error("[%s:%04d] 0 == stList.ulNum || NULL == stList.astList \n", __FUNCTION__, __LINE__, hErr);
			hErr = ERR_FAIL;
			goto free_list;
		}

		hErr = _bpl_dsmcc_carousel_list_Convert(&stList, pastList, pulCount);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("[%s:%04d] _bpl_dsmcc_carousel_info_Convert() Error [0x%x] \n", __FUNCTION__, __LINE__, hErr);
			goto free_list;
		}
	}

free_list:
	if (stList.pstCarouselInfo)
	{
		APK_DSMCC_FreeCarouselInfo(stList.pstCarouselInfo);
	}

	return hErr;
}

#if defined(CONFIG_DEBUG)
STATIC void _bpl_dsmcc_stream_evt_Print(BplDsmccStreamEvent_t *pstBplStreamEvent, HCHAR *pszPrefix, HBOOL bDetail)
{
	if (pstBplStreamEvent)
	{
		if (NULL == pszPrefix)
			pszPrefix = "[StreamEvent]\t";

		HxLOG_Debug("%s =========================================================\n", pszPrefix);
		HxLOG_Debug("%s = Event Name [%s], Event Id [0x%04x]\n", pszPrefix, pstBplStreamEvent->szEventName, pstBplStreamEvent->nEventId);
		HxLOG_Debug("%s =========================================================\n", pszPrefix);
		HxLOG_Debug("%s = ATag : [0x%04x]\n",		pszPrefix, pstBplStreamEvent->usTag);
		HxLOG_Debug("%s = PID  : [0x%04x]\n",		pszPrefix, pstBplStreamEvent->ulPid);
		HxLOG_Debug("%s = URL  : [%s]\n",			pszPrefix, (pstBplStreamEvent->szUrl) ? pstBplStreamEvent->szUrl : "Null");
		HxLOG_Debug("%s = DESC : [%s(%p)](%d)\n",	pszPrefix, pstBplStreamEvent->szDescription, pstBplStreamEvent->szDescription, pstBplStreamEvent->ulDescLen);
		if (bDetail && pstBplStreamEvent->szDescription && pstBplStreamEvent->ulDescLen)
		{
			HLIB_LOG_Dump((HUINT8 *)pstBplStreamEvent->szDescription, pstBplStreamEvent->ulDescLen, 0, FALSE);
		}
		HxLOG_Debug("%s = Info : [%s(%p)](%d)\n",	pszPrefix, pstBplStreamEvent->pucObjInfoByte, pstBplStreamEvent->pucObjInfoByte, pstBplStreamEvent->ulObjInfoByteLen);
		if (bDetail && pstBplStreamEvent->ulObjInfoByteLen && pstBplStreamEvent->pucObjInfoByte)
		{
			HLIB_LOG_Dump((HUINT8 *)pstBplStreamEvent->pucObjInfoByte, pstBplStreamEvent->ulObjInfoByteLen, 0, FALSE);
		}
		HxLOG_Debug("%s =========================================================\n", pszPrefix);
	}
}
#endif

STATIC void _bpl_dsmcc_DsmccEventListener(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	Handle_t						 hDsmcc = (Handle_t)ulParam1;
	DxDSMCC_Event_e			 eEvent = (DxDSMCC_Event_e)ulParam2;
	DxDSMCC_EventParam_t		*punParam = (DxDSMCC_EventParam_t *)ulParam3;
	HUINT32							 ulCarouselId = 0;
	HERROR							 hErr;

	if (s_stBplDsmccInfo.hDsmcc != hDsmcc)
	{
		HxLOG_Error("[%s:%04d] s_stBplDsmccInfo.hDsmcc[0x%x] != hDsmcc[0x%x] \n", __FUNCTION__, __LINE__, s_stBplDsmccInfo.hDsmcc, hDsmcc);
		return;
	}

	HxLOG_Debug("[%s:%04d] hDsmcc:[%d], eEvent:[%d]\n", __FUNCTION__, __LINE__, hDsmcc, eEvent);

	switch(eEvent)
	{
	case eDxDSMCC_EVENT_Created:
		HxLOG_Debug("[%s:%04d] eDSMCC_EVENT_Created \n", __FUNCTION__, __LINE__);
		break;

	case eDxDSMCC_EVENT_Started:
		HxLOG_Debug("[%s:%04d] eDSMCC_EVENT_Started \n", __FUNCTION__, __LINE__);
		break;

	case eDxDSMCC_EVENT_Stopped:
		HxLOG_Debug("[%s:%04d] eDSMCC_EVENT_Stopped \n", __FUNCTION__, __LINE__);
		break;

	case eDxDSMCC_EVENT_Progress:
		HxLOG_Debug("[%s:%04d] eDSMCC_EVENT_Progress \n", __FUNCTION__, __LINE__);
		break;

	case eDxDSMCC_EVENT_Changed:
		HxLOG_Debug("[%s:%04d] eDSMCC_EVENT_Changed \n", __FUNCTION__, __LINE__);
		if (punParam)
		{
			ulCarouselId = punParam->stDownload.ulParam1;
			HxLOG_Debug("[%s:%04d] BPLDSMCC_EVENT_VERSION_CHANGED (0x%08x) >>> \n", __FUNCTION__, __LINE__, ulCarouselId);
			_bpl_dsmcc_ExecuteListener(BPLDSMCC_EVENT_VERSION_CHANGED, (void *)ulCarouselId);
		}
		break;

	case eDxDSMCC_EVENT_Fail:
		HxLOG_Debug("[%s:%04d] eDSMCC_EVENT_Fail \n", __FUNCTION__, __LINE__);
		if (punParam)
		{
			ulCarouselId = punParam->stDownload.ulParam1;
			HxLOG_Debug("[%s:%04d] BPLDSMCC_EVENT_DOWNLOAD_FAIL (0x%08x) >>> \n", __FUNCTION__, __LINE__, ulCarouselId);
			_bpl_dsmcc_ExecuteListener(BPLDSMCC_EVENT_DOWNLOAD_FAIL, (void *)ulCarouselId);
		}
		break;

	case eDxDSMCC_EVENT_Success:
		HxLOG_Debug("[%s:%04d] eDSMCC_EVENT_Success \n", __FUNCTION__, __LINE__);
		if (punParam)
		{
			ulCarouselId = punParam->stDownload.ulParam1;
			HxLOG_Debug("[%s:%04d] BPLDSMCC_EVENT_DOWNLOADED (0x%08x) >>> \n", __FUNCTION__, __LINE__, ulCarouselId);
			_bpl_dsmcc_ExecuteListener(BPLDSMCC_EVENT_DOWNLOADED, (void *)ulCarouselId);
		}
		break;

	//case eDxDSMCC_EVENT_StreamEvtFound:
	case eDxDSMCC_EVENT_StreamDescriptorEvt:		// APKS_DL_DSMCC_EVENT_Stream_t
	case eDxDSMCC_EVENT_StreamEvtList:
		if (punParam)
		{
			DxDSMCC_StreamEventList_t		*pstEvtList = &punParam->stStreamEvent;
			HxList_t							*pstList;
			DxDSMCC_StreamEvent_t			*pstStreamEvent;
			BplDsmccStreamEvent_t				*pstBplStreamEvent;

			HxLOG_Debug("[%s:%04d] eEvent : [%d] \n", __FUNCTION__, __LINE__, eEvent);
			HxLOG_Debug("[%s:%04d] Stream Event Num : [%d] \n", __FUNCTION__, __LINE__, HLIB_LIST_Length(pstEvtList->pstEventList));

			pstList = HLIB_LIST_First(pstEvtList->pstEventList);
			while(pstList)
			{
				pstStreamEvent = (DxDSMCC_StreamEvent_t *)HLIB_LIST_Data(pstList);
				if (pstStreamEvent)
				{
					// Alloc -> Append
					pstBplStreamEvent = HLIB_STD_MemAlloc(sizeof(BplDsmccStreamEvent_t));
					if (pstBplStreamEvent)
					{
						HxSTD_MemSet(pstBplStreamEvent, 0, sizeof(BplDsmccStreamEvent_t));

						HUINT32		ulLength = 0;
						pstBplStreamEvent->nEventId			= pstStreamEvent->nEventId;
						pstBplStreamEvent->usTag			= pstStreamEvent->usTag;
						pstBplStreamEvent->ulPid			= pstStreamEvent->usPid;

						// Event Name
						if (pstStreamEvent->szEventName)
						{
    						HxLOG_Debug("[%s:%04d] Source: Event Name [%s]\n", __FUNCTION__, __LINE__, (NULL != pstStreamEvent->szEventName) ? (char *)pstStreamEvent->szEventName : "Null");
    						ulLength = HxSTD_StrLen((char *)pstStreamEvent->szEventName);
    						if (ulLength)
    						{
    							pstBplStreamEvent->szEventName = HLIB_STD_MemAlloc(ulLength + 1);
    							if (pstBplStreamEvent->szEventName)
    							{
    								HxSTD_MemCopy(pstBplStreamEvent->szEventName, pstStreamEvent->szEventName, ulLength);
    								pstBplStreamEvent->szEventName[ulLength] ='\0';
    							}
    						}
                        }
                        
						// Target URL
						if (pstStreamEvent->pszUrl)
						{
							HxLOG_Debug("[%s:%04d] Source: URL [%s]\n", __FUNCTION__, __LINE__, (NULL != pstStreamEvent->pszUrl) ? (char *)pstStreamEvent->pszUrl : "Null");
							ulLength = HxSTD_StrLen((char *)pstStreamEvent->pszUrl);
							if (ulLength)
							{
								pstBplStreamEvent->szUrl = HLIB_STD_MemAlloc(ulLength + 1);
								if (pstBplStreamEvent->szUrl)
								{
									HxSTD_MemCopy(pstBplStreamEvent->szUrl, pstStreamEvent->pszUrl, ulLength);
									pstBplStreamEvent->szUrl[ulLength] ='\0';
								}
							}
						}

						// Event Description
						if (pstStreamEvent->szDescription && pstStreamEvent->ulDescLen)
						{
							ulLength = pstBplStreamEvent->ulDescLen = pstStreamEvent->ulDescLen;
							if (ulLength)
							{
								pstBplStreamEvent->szDescription = HLIB_STD_MemAlloc(ulLength + 1);
								if (pstBplStreamEvent->szDescription)
								{
									HxSTD_MemCopy(pstBplStreamEvent->szDescription, pstStreamEvent->szDescription, ulLength);
									pstBplStreamEvent->szDescription[ulLength] ='\0';
								}
							}
						}

						// Obj Info Byte
						if (pstStreamEvent->ulObjInfoByteLen && pstStreamEvent->pucObjInfoByte)
						{
							ulLength =  pstBplStreamEvent->ulObjInfoByteLen = pstStreamEvent->ulObjInfoByteLen;
							if (ulLength)
							{
								pstBplStreamEvent->pucObjInfoByte = HLIB_STD_MemAlloc(ulLength + 1);
								if (pstBplStreamEvent->pucObjInfoByte)
								{
									HxSTD_MemCopy(pstBplStreamEvent->pucObjInfoByte, pstStreamEvent->pucObjInfoByte, ulLength);
									pstBplStreamEvent->pucObjInfoByte[ulLength] ='\0';
								}
							}
						}

						HxLOG_Debug("\n\n\n\n\n[%s:%04d] BPLDSMCC_EVENT_STREAM_EVENT_RECEIVED >>> (+) \n", __FUNCTION__, __LINE__);
#if defined(CONFIG_DEBUG)
						_bpl_dsmcc_stream_evt_Print(pstBplStreamEvent, "\t[Listener:Before]", FALSE);
#endif
                        if (eEvent==eDxDSMCC_EVENT_StreamDescriptorEvt)
						    _bpl_dsmcc_ExecuteListener(BPLDSMCC_EVENT_STREAM_EVENT_DESCRIPTOR_UPDATED, (void *)pstBplStreamEvent);
						else
						    _bpl_dsmcc_ExecuteListener(BPLDSMCC_EVENT_STREAM_EVENT_MESSAGE_UPDATED, (void *)pstBplStreamEvent);
#if defined(CONFIG_DEBUG) && 0
						_bpl_dsmcc_stream_evt_Print(pstBplStreamEvent, "\t[Listener:After]", FALSE);
#endif
						HxLOG_Debug("[%s:%04d] BPLDSMCC_EVENT_STREAM_EVENT_RECEIVED >>> (-) \n\n\n\n\n\n", __FUNCTION__, __LINE__);
					}
				}
				pstList = pstList->next;
			}
		}
		break;

	case eDxDSMCC_EVENT_CarouselList:
		if (punParam)
		{
			DxDSMCC_CarouselList_t		*pstList = &punParam->stCarousel;

			HxLOG_Debug("[%s:%04d] eDSMCC_EVENT_CarouselList \n", __FUNCTION__, __LINE__);
			if (pstList->pstCarouselInfo  && pstList->ulNum)
			{
				BplDsmccObjectCarouselList_t	stList;

				HxSTD_MemSet(&stList, 0, sizeof(BplDsmccObjectCarouselList_t));
				HxLOG_Debug("[%s:%04d] ListNum [%d] >>> \n", __FUNCTION__, __LINE__, pstList->ulNum);
				hErr = _bpl_dsmcc_carousel_list_Convert(pstList, &stList.pstItem, &stList.ulCarouselCount);
				if (ERR_OK == hErr)
				{
					HxLOG_Debug("[%s:%04d] BPLDSMCC_EVENT_SESSIONDATA_UPDATED >>> \n", __FUNCTION__, __LINE__);
					HxLOG_Debug("[%s:%04d] _bpl_dsmcc_ExecuteListener(BPLDSMCC_EVENT_SESSIONDATA_UPDATED) (+) >>> \n", __FUNCTION__, __LINE__);
					_bpl_dsmcc_ExecuteListener(BPLDSMCC_EVENT_SESSIONDATA_UPDATED, (void *)&stList);
					HxLOG_Debug("[%s:%04d] _bpl_dsmcc_ExecuteListener() (-) >>> \n", __FUNCTION__, __LINE__);

					HxLOG_Debug("[%s:%04d] _bpl_dsmcc_carousel_list_Free(%p, %d) (+) >>> \n", __FUNCTION__, __LINE__, stList.pstItem, stList.ulCarouselCount);
					_bpl_dsmcc_carousel_list_Free(stList.pstItem, stList.ulCarouselCount);
					HxLOG_Debug("[%s:%04d] _bpl_dsmcc_carousel_list_Free() (-) >>> \n", __FUNCTION__, __LINE__);
				}
			}
		}
		break;

	default:
		HxLOG_Error("[%s:%04d] Invalid Event [%d] \n", __FUNCTION__, __LINE__, eEvent);
		break;
	}
}

static HBOOL _bpl_dsmcc_MediaPlayEventFilter(HUINT32 ulViewId, OxMediaPlay_MediaType_e eType, OxMediaPlay_Event_e eEvent, HBOOL *updated)
{
	*updated = FALSE;

	if (eOxMP_MEDIATYPE_LIVE == eType)
	{
		*updated = TRUE;
	}

	if (0)
	{
		if (eOxMP_EVENT_LIVE_SvcChanged == eEvent)
		{
			*updated = TRUE;
		}
	}

	return TRUE;
}

static void _bpl_dsmcc_MediaPlayEventListener ( HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eType, OxMediaPlay_Event_e eEvent, OxMediaPlay_NotifierData_t *pstNotifyData, void *pvUserData )
{
	if (s_stBplDsmccInfo.pfnDsmccListener )
	{

		switch (eEvent)
		{
			case eOxMP_EVENT_LIVE_SvcChanged:
				(s_stBplDsmccInfo.pfnDsmccListener)(BPLDSMCC_EVENT_SESSIONID_UPDATED, (void *)ulSessionId);
				break;

			default:
				break;
		}
	}
}

/******************************************************************
	Interfaces
******************************************************************/

void BPL_Dsmcc_Init(void)
{
	HBOOL		 hErr;
	Handle_t	 hDsmcc = HANDLE_NULL;
	HCHAR		*pszDefaultCachePath = NULL;

	HxSTD_MemSet(&s_stBplDsmccInfo, 0, sizeof(_BPL_DSMCC_Info_t));

	hErr = APK_DSMCC_Create(eDxDSMCC_TYPE_HBBTV, &hDsmcc, &pszDefaultCachePath);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("[%s:%04d] APK_DSMCC_Create() Error : hErr [0x%x] \n", __FUNCTION__, __LINE__, hErr);
		goto free_path;
	}

	if (HANDLE_NULL == hDsmcc)
	{
		HxLOG_Error("[%s:%04d] HANDLE_NULL == hDsmcc \n", __FUNCTION__, __LINE__);
		goto free_path;
	}

	s_stBplDsmccInfo.hDsmcc = hDsmcc;
	if (pszDefaultCachePath)
	{
		HLIB_STD_StrNCpySafe(s_stBplDsmccInfo.szDefaultCachePath, pszDefaultCachePath, eDxDSMCC_MAX_PATH_LENGTH);
	}

	HxLOG_Debug("[%s:%04d] s_stBplDsmccInfo.szDefaultCachePath [%s] \n", __FUNCTION__, __LINE__, s_stBplDsmccInfo.szDefaultCachePath);

	s_stBplDsmccInfo.ulSessionId = HANDLE_NULL;

	APK_DSMCC_RegisterNotifier(hDsmcc, _bpl_dsmcc_DsmccEventListener);

	{
		HERROR	hErr;
		HUINT32 	ulViewId;
		
		hErr = APK_MEDIA_PLAY_GetMainViewId(&ulViewId);
		if( ERR_OK == hErr )
		{
			hErr = APK_MEDIA_PLAY_RegisterNotifier (ulViewId, _bpl_dsmcc_MediaPlayEventListener, _bpl_dsmcc_MediaPlayEventFilter, NULL);
			if( ERR_OK != hErr )
				HxLOG_Error("[%s:%d] APK_MEDIA_PLAY_RegisterNotifier Error: \n", __FUNCTION__, __LINE__);		
		}
	}

free_path:
	if (pszDefaultCachePath)
		APK_DSMCC_FreePath(pszDefaultCachePath);
}

void BPL_Dsmcc_Uninit(void)
{
	// Unregister Listener
	APK_DSMCC_UnRegisterNotifier(s_stBplDsmccInfo.hDsmcc, _bpl_dsmcc_DsmccEventListener);
	// Close
	APK_DSMCC_Destroy(s_stBplDsmccInfo.hDsmcc);
	// Clear Carousel List
	_bpl_dsmcc_map_Clear(s_stBplDsmccInfo.pstList);

	HxSTD_MemSet(&s_stBplDsmccInfo, 0, sizeof(_BPL_DSMCC_Info_t));
	s_stBplDsmccInfo.ulSessionId = HANDLE_NULL;
}

BPLStatus BPL_Dsmcc_SetListener(BplDsmccListener_t listener)
{
	s_stBplDsmccInfo.pfnDsmccListener = listener;

	return BPL_STATUS_OK;
}

/* Listener 등록 후 필요한 Event를 받을 수 있다. */
BPLStatus BPL_Dsmcc_Open(unsigned int ulSessionId)
{
	HERROR	hErr;

	HxLOG_Trace();

	if ((HANDLE_NULL == s_stBplDsmccInfo.hDsmcc) || (0 == s_stBplDsmccInfo.hDsmcc))
	{
		HxLOG_Error("[%s:%04d] Invalid DSMCC Handle [%d]\n", __FUNCTION__, __LINE__, s_stBplDsmccInfo.hDsmcc);
		return BPL_STATUS_ERROR;
	}

	if (s_stBplDsmccInfo.ulSessionId == ulSessionId)
	{
		HxLOG_Error("[%s:%04d] Session [0x08x] is not changed!!!\n", __FUNCTION__, __LINE__, ulSessionId);
	}

	HxLOG_Debug("[%s:%04d] APK_DSMCC_Open(0x%08x) Call!\n", __FUNCTION__, __LINE__, ulSessionId);
	hErr = APK_DSMCC_Open(s_stBplDsmccInfo.hDsmcc, (Handle_t)ulSessionId);
	if (ERR_OK == hErr)
	{
		HxLOG_Debug("[%s:%04d] Old Session [0x%08x] => New Session [0x%08x]\n", __FUNCTION__, __LINE__, s_stBplDsmccInfo.ulSessionId, ulSessionId);
		s_stBplDsmccInfo.ulSessionId = ulSessionId;
		// TODO: Init Map
	}

	return (hErr == ERR_OK) ? BPL_STATUS_OK : BPL_STATUS_ERROR;
}

/* Download 중인 Carousel Stop, Event Cache 삭제, Event도 발생시키지 않는다.. */
BPLStatus BPL_Dsmcc_Close(void)
{
	HERROR	hErr = ERR_FAIL;

	if ((HANDLE_NULL == s_stBplDsmccInfo.hDsmcc) || (0 == s_stBplDsmccInfo.hDsmcc))
	{
		HxLOG_Error("[%s:%04d] Invalid DSMCC Handle [%d]\n", __FUNCTION__, __LINE__, s_stBplDsmccInfo.hDsmcc);
		return BPL_STATUS_ERROR;
	}

	if (HANDLE_NULL == s_stBplDsmccInfo.ulSessionId)
	{
		HxLOG_Error("[%s:%04d] Invalid SessionId [0x%08x]\n", __FUNCTION__, __LINE__, s_stBplDsmccInfo.ulSessionId);
		return BPL_STATUS_ERROR;
	}

	HxLOG_Debug("[%s:%04d] Session to Close : [0x%08x]\n", __FUNCTION__, __LINE__, s_stBplDsmccInfo.ulSessionId);
	hErr = APK_DSMCC_Close(s_stBplDsmccInfo.hDsmcc, (Handle_t)s_stBplDsmccInfo.ulSessionId);

	// Clear Carousel List
	hErr = _bpl_dsmcc_map_Clear(s_stBplDsmccInfo.pstList);
	HxLOG_Debug("[%s:%04d] _bpl_dsmcc_map_Clear() result: [0x%08x]\n", __FUNCTION__, __LINE__, hErr);
	s_stBplDsmccInfo.pstList = NULL;

	s_stBplDsmccInfo.ulSessionId = HANDLE_NULL;

	return (hErr == ERR_OK) ? BPL_STATUS_OK : BPL_STATUS_ERROR;
}

/* Down 시작을 위한 Caoursel Id와 caching path를 지정 */
BPLStatus BPL_Dsmcc_Down( unsigned int uiCarouselId, char *szCachePath )
{
	HERROR				 hErr;

	HxLOG_Debug("[%s:%04d] uiCarouselId [0x%08x], szCachePath [%s] \n", __FUNCTION__, __LINE__, uiCarouselId, szCachePath ? szCachePath : "Null");

	if (szCachePath)
	{
		HxLOG_Debug("[%s:%04d] szCachePath [%s] \n", __FUNCTION__, __LINE__, s_stBplDsmccInfo.szDefaultCachePath ? s_stBplDsmccInfo.szDefaultCachePath : "Null");
		if (NULL == HxSTD_StrStr(szCachePath, s_stBplDsmccInfo.szDefaultCachePath))
		{
			HxLOG_Error("[%s:%04d] Invalid Cache Path [%s], Default Path is [%s] \n", __FUNCTION__, __LINE__, szCachePath, s_stBplDsmccInfo.szDefaultCachePath);
			return BPL_STATUS_ERROR;
		}
	}

	hErr = APK_DSMCC_Start(s_stBplDsmccInfo.hDsmcc, (Handle_t)uiCarouselId, (const HCHAR *)szCachePath);
	if (ERR_OK == hErr)
	{
		hErr = _bpl_dsmcc_map_Add(&s_stBplDsmccInfo.pstList, uiCarouselId, (const HCHAR *)szCachePath);
		if (ERR_OK != hErr) HxLOG_Error("[%s:%04d] _bpl_dsmcc_map_Add() Error \n", __FUNCTION__, __LINE__);
	}

	return (hErr == ERR_OK) ? BPL_STATUS_OK : BPL_STATUS_ERROR;
}


/* Down 정지 */
BPLStatus BPL_Dsmcc_Cancel( unsigned int uiCarouselId )
{
	HERROR				 hErr;

	HxLOG_Debug("[%s:%04d] uiCarouselId [0x%08x] \n", __FUNCTION__, __LINE__, uiCarouselId);
	hErr = APK_DSMCC_Stop(s_stBplDsmccInfo.hDsmcc, (Handle_t)uiCarouselId);
	if (ERR_OK == hErr)
	{
		// Remove Carousel List
		hErr = _bpl_dsmcc_map_Remove(&s_stBplDsmccInfo.pstList, uiCarouselId);
	}
	return (hErr == ERR_OK) ? BPL_STATUS_OK : BPL_STATUS_ERROR;
}

/* 특정 Carousel Down 재시작 */
BPLStatus BPL_Dsmcc_Restart( unsigned int uiCarouselId )
{
	HERROR				 hErr;

	HxLOG_Debug("[%s:%04d] uiCarouselId [0x%08x] \n", __FUNCTION__, __LINE__, uiCarouselId);

	hErr = APK_DSMCC_Restart(s_stBplDsmccInfo.hDsmcc, (Handle_t)uiCarouselId);

	return (hErr == ERR_OK) ? BPL_STATUS_OK : BPL_STATUS_ERROR;
}

BPLStatus BPL_Dsmcc_RequestStreamEventStart(void)
{
	HERROR				 hErr;

	hErr = APK_DSMCC_RequestEvent(s_stBplDsmccInfo.hDsmcc, TRUE);

	return (hErr == ERR_OK) ? BPL_STATUS_OK : BPL_STATUS_ERROR;
}

BPLStatus BPL_Dsmcc_RequestStreamEventStop(void)
{
	HERROR				 hErr;

	hErr = APK_DSMCC_RequestEvent(s_stBplDsmccInfo.hDsmcc, FALSE);

	return (hErr == ERR_OK) ? BPL_STATUS_OK : BPL_STATUS_ERROR;
}

/* default path를 얻기위한 함수, 모듈 초기화 시 한번만 호출 */
BPLStatus BPL_Dsmcc_GetCachePath( char *pszPath, unsigned int uiBufferLength )
{
	HUINT32		ulStrLength = 0;

	if (NULL == pszPath)
	{
		HxLOG_Error("[%s:%04d] NULL == pszPath \n", __FUNCTION__, __LINE__);
		return BPL_STATUS_ERROR;
	}

	HxLOG_Debug("[%s:%04d] szCachePath [%s] \n", __FUNCTION__, __LINE__, s_stBplDsmccInfo.szDefaultCachePath ? s_stBplDsmccInfo.szDefaultCachePath : "Null");

	ulStrLength = HxSTD_StrLen(s_stBplDsmccInfo.szDefaultCachePath);
	if (ulStrLength >= uiBufferLength)
	{
		HxLOG_Error("[%s:%04d] ulStrLength[%d] >= uiBufferLength[%d] \n", __FUNCTION__, __LINE__, ulStrLength, uiBufferLength);
		return BPL_STATUS_ERROR;
	}

	HLIB_STD_StrNCpySafe(pszPath, (const HCHAR *)s_stBplDsmccInfo.szDefaultCachePath, uiBufferLength);

	HxLOG_Debug("[%s:%04d] pszPath [%s] \n", __FUNCTION__, __LINE__, pszPath ? pszPath : "Null");

	return BPL_STATUS_OK;
}

/* PMT의 Carousel정볼르 얻기위한 함수 */
BPLStatus BPL_Dsmcc_GetCarouselList(BplDsmccObjectCarousel_t **pastList, unsigned int *pulCount)
{
	HERROR	hErr;

	if (NULL == pastList)
	{
		HxLOG_Error("[%s:%04d] NULL == pastList \n", __FUNCTION__, __LINE__);
		return BPL_STATUS_ERROR;
	}

	if (NULL == pulCount)
	{
		HxLOG_Error("[%s:%04d] NULL == pulCount \n", __FUNCTION__, __LINE__);
		return BPL_STATUS_ERROR;
	}

	hErr = _bpl_dsmcc_carousel_list_Get(pastList, pulCount);

	return (ERR_OK == hErr) ? BPL_STATUS_OK : BPL_STATUS_ERROR;
}

BPLStatus BPL_Dsmcc_FreeCarouselList(BplDsmccObjectCarousel_t *astList, unsigned int ulCount)
{
	HERROR	hErr;

	if (NULL == astList)
	{
		HxLOG_Error("[%s:%04d] NULL == astList \n", __FUNCTION__, __LINE__);
		return BPL_STATUS_ERROR;
	}


	hErr = _bpl_dsmcc_carousel_list_Free(astList, ulCount);

	return (hErr == ERR_OK) ? BPL_STATUS_OK : BPL_STATUS_ERROR;
}

BPLStatus BPL_Dsmcc_GetServiceInfo( BplDsmccService_t *pService )
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
