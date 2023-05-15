/**************************************************************
 *	@file		ipepg_downloader.c
 *	IP EPG downloader for OCTO 2.0
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/08/01
 *	@author			humax
 *	@brief			IP EPG Downloader for OCTO 2.0 Project
 **************************************************************/
#include <unistd.h>
#include <signal.h>
#include <vkernel.h>
#include <dapi.h>
#include "../include/ipepg.h"
#include "ipepg_manager.h"
#include "ipepg_downloader.h"
#include "ipepg_parser.h"

static HUINT8		*s_epg_buffer = NULL;
static HINT32		s_epgdata_size = 0;
static HINT32		s_overflow = 0;

#if defined(CFG_OPEN_DEBUG)
#undef HxLOG_Trace
#define HxLOG_Trace()	printf("[IPEPG] %s %d\n", __FUNCTION__, __LINE__);
#undef HxLOG_Debug
#define HxLOG_Debug		printf
#undef HxLOG_Print
#define HxLOG_Print		printf
#endif



#ifdef IPEPG_FUNCTION_COLOR_TRACE
	#define IPEPG_FUNC_START()	HxLOG_Debug( HxANSI_COLOR_CYAN("[%s][%d] +++ \n") , __FUNCTION__, __LINE__);
	#define IPEPG_FUNC_END()	HxLOG_Debug(  HxANSI_COLOR_CYAN("[%s][%d] --- \n") , __FUNCTION__, __LINE__);
#else
	#define IPEPG_FUNC_START()	HxLOG_Debug("[%s][%d] +++ \n" , __FUNCTION__, __LINE__);
	#define IPEPG_FUNC_END()	HxLOG_Debug("[%s][%d] --- \n" , __FUNCTION__, __LINE__);
#endif

#define CONFIG_USE_CHANNEL_LOGO_TASK
#ifdef CONFIG_USE_CHANNEL_LOGO_TASK

typedef struct
{
	HxList_t			*ListOfEventURL;
} IPEPG_CHLOGO_TASK_Instance_t;

#endif
extern const HCHAR*	THAPI_GetThumbnailURL(const HCHAR *pszURL, HUINT32 ulTime, const HCHAR *pszMountPath, HBOOL bRetExpectURL);

#define _____________________Parse_Module
static HERROR	ipepg_ParseServiceList(HUINT8 *pRawBuffer, HUINT32 ulBufferSize, HxList_t **ListOfEventURL)
{
	return IPEPG_HumaxParseServiceList(pRawBuffer, ulBufferSize, ListOfEventURL);
}

static HERROR	ipepg_ParseEventList(HUINT8 *pRawBuffer, HUINT32 ulBufferSize, HxList_t **ListOfEventURL,HINT32 ulSvcUid)
{
	return IPEPG_HumaxParseEventList(pRawBuffer, ulBufferSize, ListOfEventURL, ulSvcUid);

}

#define _____________________DownLoad_Module
static HERROR	ipepg_SettingHttpGetWithCallback(const HINT8 * szUrl, HxHttpGet_Callback *callback, HUINT32 ulTimeout)
{
	HERROR							hError = ERR_FAIL;
	HxTRANSPORT_Http_t			*pHttp = NULL;
	HxTRANSPORT_Httpheader_t		*pHttpHeader = NULL;
	HxTRANSPORT_HttpResponse_e	resCode;
	HFLOAT64						ulRealSize = 0;

    HxLOG_Trace();
	IPEPG_GOTO_IF(szUrl == NULL, EXIT_FUNC);
	pHttp = HLIB_TRANSPORT_HttpOpen();
	IPEPG_GOTO_IF(pHttp == NULL, EXIT_FUNC);

	pHttpHeader = HLIB_TRANSPORT_HttpAppendHeader(pHttpHeader, "Content-Type: text/xml");
	IPEPG_GOTO_IF(pHttpHeader == NULL, EXIT_FUNC);

#if defined(IPEPG_USE_COMPRESS)
	pHttpHeader = HLIB_TRANSPORT_HttpAppendHeader(pHttpHeader, "Accept-Encoding : GZIP");
#endif
	IPEPG_GOTO_IF(HLIB_TRANSPORT_HttpSetHeaderToTP(pHttp, pHttpHeader) != ERR_OK, EXIT_FUNC);

    if(ulTimeout != 0)
    {
        IPEPG_GOTO_IF(HLIB_TRANSPORT_HttpSetTimeout(pHttp, ulTimeout) != ERR_OK, EXIT_FUNC);
    }

	IPEPG_GOTO_IF(HLIB_TRANSPORT_HttpGetWithCallback(pHttp, szUrl, callback, &ulRealSize) != ERR_OK, EXIT_FUNC);

	hError = ERR_OK;
EXIT_FUNC :

	if ( hError != ERR_OK )
	{
		HLIB_TRANSPORT_HttpGetHttpResponseCode(pHttp, &resCode);
		HxLOG_Debug("[%s] failed. errCode (%d)\n",  __FUNCTION__ ,resCode);
	}

	if(pHttpHeader != NULL) HLIB_TRANSPORT_HttpFreeHeader(pHttpHeader);
	if(pHttp != NULL) HLIB_TRANSPORT_HttpClose(pHttp);

	return hError;
}

static HINT32   ipepg_DownloadCallback(void *ptr, HINT32 size, HINT32 nmemb, void *stream)
{
    HINT32 uiDataSize = 0;

	uiDataSize = size * nmemb;
	if(s_epg_buffer == NULL || s_epgdata_size + uiDataSize > IPEPG_EVENT_DATA_MAX_SIZE)
		return  0;

	HxSTD_MemCopy(s_epg_buffer+s_epgdata_size, ptr, uiDataSize);
	s_epgdata_size += uiDataSize;

    return uiDataSize;
}

static void ipepg_DumpPrint(DxIpEvent_t * pEvent)
{
#if defined(IPEPG_PRINT_DUMP)
	HUINT32 j = 0;
	HUINT8	*testBuf = (HUINT8 *)pEvent;
	HUINT32 i = 0, j = 0;

	if(pEvent)
	{
		HxLOG_Debug("type                    size[%d] data[%x]\n", sizeof(pEvent->stBase.type), pEvent->stBase.type);
		HxLOG_Debug("ucKind                  size[%d] data[%x]\n", sizeof(pEvent->stBase.ucKind), pEvent->stBase.ucKind);
		HxLOG_Debug("usOnId                  size[%d] data[%x]\n", sizeof(pEvent->stBase.usOnId), pEvent->stBase.usOnId);
		HxLOG_Debug("usTsId                  size[%d] data[%x]\n", sizeof(pEvent->stBase.usTsId), pEvent->stBase.usTsId);
		HxLOG_Debug("usSvcId                 size[%d] data[%x]\n", sizeof(pEvent->stBase.usSvcId), pEvent->stBase.usSvcId);
		HxLOG_Debug("ulUniqueId              size[%d] data[%x]\n", sizeof(pEvent->stBase.ulUniqueId), pEvent->stBase.ulUniqueId);
		HxLOG_Debug("usSvcUId                size[%d] data[%x]\n", sizeof(pEvent->stBase.ulSvcUid), pEvent->stBase.ulSvcUid);
		HxLOG_Debug("ulStartTime             size[%d] data[%x]\n", sizeof(pEvent->stBase.ulStartTime), pEvent->stBase.ulStartTime);
		HxLOG_Debug("ulEndTime               size[%d] data[%x]\n", sizeof(pEvent->stBase.ulEndTime), pEvent->stBase.ulEndTime);
		HxLOG_Debug("ucRating                size[%d] data[%x]\n", sizeof(pEvent->stBase.ucRating), pEvent->stBase.ucRating);
		HxLOG_Debug("ucDrm                   size[%d] data[%x]\n", sizeof(pEvent->stBase.ucDrm), pEvent->stBase.ucDrm);
		HxLOG_Debug("ulSeriesId              size[%d] data[%x]\n", sizeof(pEvent->stBase.ulSeriesId), pEvent->stBase.ulSeriesId);
		HxLOG_Debug("ulEpisodeNum            size[%d] data[%x]\n", sizeof(pEvent->stBase.ulEpisodeNum), pEvent->stBase.ulEpisodeNum);
		HxLOG_Debug("ulTotalNum              size[%d] data[%x]\n", sizeof(pEvent->stBase.ulTotalNum), pEvent->stBase.ulTotalNum);
		HxLOG_Debug("ulSeason                size[%d] data[%x]\n", sizeof(pEvent->stBase.ulSeason), pEvent->stBase.ulSeason);
		HxLOG_Debug("ucLinkType              size[%d] data[%x]\n", sizeof(pEvent->stBase.ucLinkType), pEvent->stBase.ucLinkType);
		HxLOG_Debug("ucSeriesType            size[%d] data[%x]\n", sizeof(pEvent->stBase.ucSeriesType), pEvent->stBase.ucSeriesType);
		HxLOG_Debug("ucGenre                 size[%d] data[%x]\n", sizeof(pEvent->stBase.ucGenre), pEvent->stBase.ucGenre);
		HxLOG_Debug("ucRecommendation        size[%d] data[%x]\n", sizeof(pEvent->stBase.ucRecommendation), pEvent->stBase.ucRecommendation);
		HxLOG_Debug("ucFtaContentValue       size[%d] data[%x]\n", sizeof(pEvent->stBase.ucFtaContentValue), pEvent->stBase.ucFtaContentValue);
		HxLOG_Debug("ucLang                  size[%d] data[%x]\n", sizeof(pEvent->stBase.ucLang), pEvent->stBase.ucLang[0]);

			for(i = 0; i< pEvent->ulStrCount; i++)
			{
				HxLOG_Debug("INDEX[%d] size[%d] type[%x] [%s]\n",i, pEvent->stText[i].usSize, pEvent->stText[i].eType, pEvent->stText[i].ucStr);
				for(j = 0 ;  j < pEvent->stText[i].usSize; j++)
				{
					if(j%16 == 0)
					{
						HxLOG_Debug("\n");
					}
					HxLOG_Debug("%02x ", pEvent->stText[i].ucStr[j]);
				}
				HxLOG_Debug("\n");
			}

			for(i = 0; i< pEvent->ulLinkCount; i++)
			{

					HxLOG_Debug("LINK INDEX[%d] prog[%x] stime[%x] etime[%x] episode[%x] season[%x]\n",i, pEvent->stLink[i].ulProgId, pEvent->stLink[i].ulStartTime, pEvent->stLink[i].ulEndTime, pEvent->stLink[i].ulEpisodeNum, pEvent->stLink[i].ulSeason);
					if(pEvent->stLink[i].stSubName.usSize) HxLOG_Debug("  name size[%d] [%s]\n",pEvent->stLink[i].stSubName.usSize ,pEvent->stLink[i].stSubName.ucStr);
			}
			for(i = 0; i< pEvent->ulMediaCount; i++)
			{
				HxLOG_Debug("MEDIA INDEX[%d] id[%s] pdate[%x] edate[%x] tdate[%x] price[%x]\n",i,
					pEvent->stMedia[i].stMediaId.ucStr, pEvent->stMedia[i].ulPubDate, pEvent->stMedia[i].ulExPireDate,
					pEvent->stMedia[i].ulTransmitDate, pEvent->stMedia[i].ulPrice);
				HxLOG_Debug("\t MediaUrlCount[%d] MediaImageCount[%d] \n",pEvent->stMedia[i].ulMediaUrlCount,pEvent->stMedia[i].ulMediaImageCount);

				// MEDIAURLS
				for ( j = 0;  j < pEvent->stMedia[i].ulMediaUrlCount ; j++ )
				{
					if(pEvent->stMedia[i].stMediaUrl[j].stTarget.usSize)	HxLOG_Debug("	MediaURL[%d] TARGET size[%d] [%s] \n", j, pEvent->stMedia[i].stMediaUrl[j].stTarget.usSize, pEvent->stMedia[i].stMediaUrl[j].stTarget.ucStr);
					if(pEvent->stMedia[i].stMediaUrl[j].stType.usSize)		HxLOG_Debug("	MediaURL[%d] TYPE size[%d] [%s] \n", j , pEvent->stMedia[i].stMediaUrl[j].stType.usSize, pEvent->stMedia[i].stMediaUrl[j].stType.ucStr);
					if(pEvent->stMedia[i].stMediaUrl[j].stUrl.usSize)		HxLOG_Debug("	MediaURL[%d] URL size[%d] [%s] \n", j, pEvent->stMedia[i].stMediaUrl[j].stUrl.usSize, pEvent->stMedia[i].stMediaUrl[j].stUrl.ucStr);
				}

				// IMAGES
				for(j = 0; j < pEvent->stMedia[i].ulMediaImageCount ; j++ )
				{
					if(pEvent->stMedia[i].stMediaImage[j].stUrlType.usSize)		HxLOG_Debug("	IMAGES[%d] URLTYPE size[%d] [%s] \n", j , pEvent->stMedia[i].stMediaImage[j].stUrlType.usSize, pEvent->stMedia[i].stMediaImage[j].stUrlType.ucStr);
					if(pEvent->stMedia[i].stMediaImage[j].stImageUrl.usSize)	HxLOG_Debug("	IMAGES[%d] IMAGEURL size[%d] [%s] \n", j , pEvent->stMedia[i].stMediaImage[j].stImageUrl.usSize, pEvent->stMedia[i].stMediaImage[j].stImageUrl.ucStr);
					HxLOG_Debug("	IMAGES[%d] URLSIZE [%d] \n", j , pEvent->stMedia[i].stMediaImage[j].ulUrlSize);
				}
			}
		for(i = 0; i < sizeof(DxIpEvent_t); i++)
		{
			if(i%16 == 0)
				HxLOG_Debug("\n");

			HxLOG_Debug("%02x ", testBuf[i]);
		}
	}
#else
#endif
	return;
}

static void	ipepg_GetThumbnail(void * data)
{

	IPEPG_UpdateEvent_t * pUpdateEvent = (IPEPG_UpdateEvent_t *)data;

	DxIpEvent_t * pEvent = NULL;
	if(pUpdateEvent->eUpdateType != eIPEPG_EVENT_TYPE_UPDATE)
		return;

	pEvent = pUpdateEvent->pIpEvent;

	if(pEvent == NULL) return;
	else
	{
		HUINT32 i = 0;
		for(i = 0; i < pEvent->ulStrCount; i++)
		{
			if(pEvent->stText[i].eType == eDxIPEPG_TEXT_THUMBNAIL && pEvent->stText[i].ucStr)
			{
				THAPI_GetThumbnailURL(pEvent->stText[i].ucStr, 0, IPEPG_EVENT_THUMBNAIL_MOUNT_PATH, FALSE);
				return;
			}
		}
	}
}

static void ipepg_StreamWrite_DxIpEventStr(HUINT32 ulSD, HUINT16 usSize, DxIpEventStr_t * pString)
{
	HLIB_RWSTREAM_Write(ulSD,(void *)&usSize , sizeof(HUINT16));
	if(pString->usSize)
	{
		HLIB_RWSTREAM_Write(ulSD, (void *)&pString->eType, sizeof(DxIPEPG_Text_e));
		HLIB_RWSTREAM_Write(ulSD, (void *)pString->ucLang, EPG_HUMAX_LANG_LEN);
		HLIB_RWSTREAM_Write(ulSD, (void *)pString->ucStr, pString->usSize+1);
	}
}

static void ipepg_SetDownloadProgress(HBOOL bBackwardSupport, HBOOL bForwardMode,HUINT32 ulCurrentIndex, HUINT32 ulTotalCount)
{
	HUINT32 ulPercent = 0;
	static HUINT32 ulPrevPercent = 0;

#if defined(IPEPG_PRIORITY_DOWN)
	/* Increase every 5 percentage. */
#else
	ulCurrentIndex = ulCurrentIndex + 1;	//zero base value
#endif
	ulPercent = (HUINT32)((ulCurrentIndex*100) / ulTotalCount) + 1;

	if(TRUE == bBackwardSupport)
	{
		ulPercent = (HUINT32)(ulPercent/2);
		if(FALSE == bForwardMode)
		{
			ulPercent = ulPercent + 50;
		}
	}

	if(ulPercent > 100)
	{
		ulPercent = 100;
	}

	HxLOG_Debug("Percent %d , curr:%d total:%d\n", ulPercent, ulCurrentIndex, ulTotalCount);
	if(0 != ulPercent && ulPrevPercent != ulPercent)
	{
		IPEPG_SetItem(eIPEPG_ITEM_DOWNLOADPROGRESS, (void *)ulPercent);
		ulPrevPercent = ulPercent;
	}
}

static HBOOL ipepg_updateThumbnailByMediaImage(DxIpEvent_t * pEvent)
{
	HUINT32 ulIndex = 0;
	HUINT32 ulTargetIndex = eDxIPEPG_TEXT_NONE;
	HBOOL bUpdated = FALSE;

	if(pEvent == NULL)
		return FALSE;

	for (ulIndex = eDxIPEPG_TEXT_NONE; ulIndex < eDxIPEPG_TEXT_MAX; ulIndex++)
	{
		if(eDxIPEPG_TEXT_THUMBNAIL == pEvent->stText[ulIndex].eType)
		{
			if(NULL == pEvent->stText[ulIndex].ucStr)
			{
				ulTargetIndex = ulIndex;
				break;
			}
		}
	}
	if(ulTargetIndex > eDxIPEPG_TEXT_NONE)
	{
		return FALSE;
	}

	if( pEvent->ulMediaCount > 0)
	{
		HINT32 mediaImageTargetIndex = -1;	// 0 base
		HINT32 mediaImageCount = 0;
		HINT32 valueOfCandidate = IPEPG_MAX_IMAGE_SIZE;
		HINT32 value = 0;
		HINT32 imageSize = 0;
		HUINT8 * pucMediaImageUrl = NULL;

		// 일단, 첫번째 media에 대해서만 처리한다.
		// 구조적으로는 여러개의 MEDIA가 존재할 것을 가정 하지만, 현재 시나리오에서는 존재할 수 없다.
		mediaImageCount = (HINT32)pEvent->stMedia[0].ulMediaImageCount;

		// IPEPG_MAX_IMAGE_SIZE에 가장 가까운 thumbnail을 찾는다
		for(ulIndex = 0; ulIndex < mediaImageCount; ulIndex++)
		{
			imageSize = (HINT32)pEvent->stMedia[0].stMediaImage[ulIndex].ulUrlSize;

			/*HxLOG_Debug("Update[%d] imageSize:%d, type:%s, url:%s \n", ulIndex, imageSize, pEvent->stMedia[0].stMediaImage[ulIndex].stUrlType.ucStr,pEvent->stMedia[0].stMediaImage[ulIndex].stImageUrl.ucStr);*/
			if ( imageSize>0 && imageSize <= IPEPG_MAX_IMAGE_SIZE )
			{
				value = IPEPG_MAX_IMAGE_SIZE-imageSize;
				if(valueOfCandidate > value)
				{
					mediaImageTargetIndex = ulIndex;
					valueOfCandidate = value;
				}
			}
		}

		if(mediaImageTargetIndex >= 0)
		{
			pucMediaImageUrl = pEvent->stMedia[0].stMediaImage[mediaImageTargetIndex].stImageUrl.ucStr;
			if(pucMediaImageUrl)
			{
				ulTargetIndex = pEvent->ulStrCount;
				pEvent->stText[ulTargetIndex].eType = eDxIPEPG_TEXT_THUMBNAIL;
				pEvent->stText[ulTargetIndex].ucStr = IPEPG_StrDup(pucMediaImageUrl);
				pEvent->stText[ulTargetIndex].usSize= HxSTD_StrLen(pucMediaImageUrl);
				pEvent->ulStrCount++;
				HxLOG_Print("Update thumbnail URL target:%d, : %s \n", ulTargetIndex, pucMediaImageUrl);
				bUpdated = TRUE;
			}
		}
	}
	return bUpdated;
}

static HBOOL	ipepg_EventSendToDama(void * data, void * udata)
{
	IPEPG_UpdateEvent_t * pUpdateEvent = (IPEPG_UpdateEvent_t *)data;

	DxIpEvent_t * pEvent = NULL;

	if(pUpdateEvent)
		pEvent = pUpdateEvent->pIpEvent;
	else
		return FALSE;

	if(pUpdateEvent->eUpdateType != (IPEPG_EVENT_TYPE_e)udata)
		return FALSE;

	if(pEvent != NULL)
	{
#ifdef CONFIG_FUNC_EPG_USE_SVCUID
		HUID	eventUid = DxEVENT_UID(pEvent->stBase.ulSvcUid, pEvent->stBase.ulUniqueId);
#else
		HUID	eventUid = (HUID)(((HUID)eDxEPG_UID_IP) << 48 |  pEvent->stBase.ulUniqueId);
#endif
		if(pUpdateEvent->eUpdateType == eIPEPG_EVENT_TYPE_UPDATE)
		{

			HUINT32 i = 0, j = 0, ulSD = 0;
			HUINT8 * ucBinBuf = NULL;
			HUINT32 ulBinSize = sizeof(DxIpEventBase_t);

			ipepg_DumpPrint(pEvent);

			if(pEvent->ulStrCount > eDxIPEPG_TEXT_MAXCOUNT) HxLOG_Assert("Memory corrupted\n");
			if(pEvent->ulLinkCount > eDxIPEPG_LINK_MAXCOUNT) HxLOG_Assert("Memory corrupted\n");
			if(pEvent->ulMediaCount > eDxIPEPG_MEDIA_MAXCOUNT) HxLOG_Assert("Memory corrupted\n");

#ifdef IPEPG_THUMBNAIL_UDPATE_BY_DEEPLINKINFO	// for ausdtt
// 기본 thumbnail이 없는 경우, MEDIAIMAGES의 thumbnail로 update
			ipepg_updateThumbnailByMediaImage(DxIpEvent_t * pEvent)(pEvent);
#endif
			// String list Count
			ulBinSize += sizeof(pEvent->ulStrCount);
			for(i = 0; i< pEvent->ulStrCount; i++)
			{
				if(pEvent->stText[i].usSize)	ulBinSize += (pEvent->stText[i].usSize+1+sizeof(HUINT16)+sizeof(DxIPEPG_Text_e)+EPG_HUMAX_LANG_LEN);
				else ulBinSize+= sizeof(HUINT16);
			}

			// Link list Count
			ulBinSize += sizeof(pEvent->ulLinkCount);
			for(i = 0; i< pEvent->ulLinkCount; i++)
			{
				DxIpEventStr_t * pString = NULL;

				ulBinSize += sizeof(pEvent->stLink[i].ulProgId);
				ulBinSize += sizeof(pEvent->stLink[i].ulStartTime);
				ulBinSize += sizeof(pEvent->stLink[i].ulEndTime);
				ulBinSize += sizeof(pEvent->stLink[i].ulEpisodeNum);
				ulBinSize += sizeof(pEvent->stLink[i].ulSeason);

				pString = &pEvent->stLink[i].stSubName;
				if(pString->usSize) ulBinSize += (pString->usSize+1+sizeof(HUINT16)+sizeof(DxIPEPG_Text_e)+EPG_HUMAX_LANG_LEN);
				else ulBinSize+= sizeof(HUINT16);
			}

			// Media list Count
			ulBinSize += sizeof(pEvent->ulMediaCount);
			for(i = 0; i< pEvent->ulMediaCount; i++)
			{
				DxIpEventStr_t * pString = NULL;

				ulBinSize += sizeof(pEvent->stMedia[i].ulPubDate);
				ulBinSize += sizeof(pEvent->stMedia[i].ulExPireDate);
				ulBinSize += sizeof(pEvent->stMedia[i].ulTransmitDate);
				ulBinSize += sizeof(pEvent->stMedia[i].ulPrice);

				// MediaID
				pString = &pEvent->stMedia[i].stMediaId;
				if(pString->usSize) ulBinSize += (pString->usSize+1+sizeof(HUINT16)+sizeof(DxIPEPG_Text_e)+EPG_HUMAX_LANG_LEN);
				else ulBinSize+= sizeof(HUINT16);

				// Media URLS
				ulBinSize += sizeof(pEvent->stMedia[i].ulMediaUrlCount);
				for( j = 0; j < pEvent->stMedia[i].ulMediaUrlCount; j++ )
				{
					// TARGET
					pString = &pEvent->stMedia[i].stMediaUrl[j].stTarget;
					if(pString->usSize) ulBinSize += (pString->usSize+1+sizeof(HUINT16)+sizeof(DxIPEPG_Text_e)+EPG_HUMAX_LANG_LEN);
					else ulBinSize+= sizeof(HUINT16);

					// TYPE
					pString = &pEvent->stMedia[i].stMediaUrl[j].stType;
					if(pString->usSize) ulBinSize += (pString->usSize+1+sizeof(HUINT16)+sizeof(DxIPEPG_Text_e)+EPG_HUMAX_LANG_LEN);
					else ulBinSize+= sizeof(HUINT16);

					// URL
					pString = &pEvent->stMedia[i].stMediaUrl[j].stUrl;
					if(pString->usSize) ulBinSize += (pString->usSize+1+sizeof(HUINT16)+sizeof(DxIPEPG_Text_e)+EPG_HUMAX_LANG_LEN);
					else ulBinSize+= sizeof(HUINT16);
				}

				// IMAGES
				ulBinSize += sizeof(pEvent->stMedia[i].ulMediaImageCount);
				for( j = 0; j < pEvent->stMedia[i].ulMediaImageCount; j++ )
				{
					// size
					ulBinSize += sizeof(pEvent->stMedia[i].stMediaImage[j].ulUrlSize);

					// type
					pString = &pEvent->stMedia[i].stMediaImage[j].stUrlType;
					if(pString->usSize)	ulBinSize += (pString->usSize+1+sizeof(HUINT16)+sizeof(DxIPEPG_Text_e)+EPG_HUMAX_LANG_LEN);
					else ulBinSize+= sizeof(HUINT16);

					// url
					pString = &pEvent->stMedia[i].stMediaImage[j].stImageUrl;
					if(pString->usSize) ulBinSize += (pString->usSize+1+sizeof(HUINT16)+sizeof(DxIPEPG_Text_e)+EPG_HUMAX_LANG_LEN);
					else ulBinSize+= sizeof(HUINT16);
				}
			}

			// Open RWSTREAM For Total Size
			ulSD = HLIB_RWSTREAM_Open(ucBinBuf, ulBinSize);

			// Write DxIpEventBase_t Info
			HLIB_RWSTREAM_Write(ulSD, (void *)&(pEvent->stBase) , sizeof(DxIpEventBase_t));

			// Write String list Info
			HLIB_RWSTREAM_Write(ulSD, (void *)&(pEvent->ulStrCount) , sizeof(pEvent->ulStrCount));
			for(i = 0; i< pEvent->ulStrCount; i++)
			{
				HUINT16	usSize = pEvent->stText[i].usSize?(pEvent->stText[i].usSize+1):0;
				HLIB_RWSTREAM_Write(ulSD, (void *)&usSize , sizeof(HUINT16));
				if(pEvent->stText[i].usSize)
				{
					HLIB_RWSTREAM_Write(ulSD, (void *)&pEvent->stText[i].eType, sizeof(DxIPEPG_Text_e));
					HLIB_RWSTREAM_Write(ulSD, (void *)pEvent->stText[i].ucLang, EPG_HUMAX_LANG_LEN);
					HLIB_RWSTREAM_Write(ulSD, (void *)pEvent->stText[i].ucStr , pEvent->stText[i].usSize+1);
					//HxLOG_Print("Index[%d] eType[%x] ucLang[%c][%c][%c] str[%s]\n", index, pEvent->stText[i].eType, pEvent->stText[i].ucLang[0], pEvent->stText[i].ucLang[1], pEvent->stText[i].ucLang[2], pEvent->stText[i].ucStr);
				}
			}

			// Write Link list Info
			HLIB_RWSTREAM_Write(ulSD, (void *)&(pEvent->ulLinkCount) , sizeof(pEvent->ulLinkCount));
			for(i = 0; i< pEvent->ulLinkCount; i++)
			{
				HUINT16 usSize = 0;
				DxIpEventStr_t * pString = NULL;

				HLIB_RWSTREAM_Write(ulSD, (void *)&pEvent->stLink[i].ulProgId, sizeof(pEvent->stLink[i].ulProgId));
				HLIB_RWSTREAM_Write(ulSD, (void *)&pEvent->stLink[i].ulStartTime, sizeof(pEvent->stLink[i].ulStartTime));
				HLIB_RWSTREAM_Write(ulSD, (void *)&pEvent->stLink[i].ulEndTime, sizeof(pEvent->stLink[i].ulEndTime));
				HLIB_RWSTREAM_Write(ulSD, (void *)&pEvent->stLink[i].ulEpisodeNum, sizeof(pEvent->stLink[i].ulEpisodeNum));
				HLIB_RWSTREAM_Write(ulSD, (void *)&pEvent->stLink[i].ulSeason, sizeof(pEvent->stLink[i].ulSeason));

				pString = &pEvent->stLink[i].stSubName;
				usSize = pString->usSize?(pString->usSize+1):0;
				HLIB_RWSTREAM_Write(ulSD, (void *)&usSize , sizeof(HUINT16));
				if(pString->usSize)
				{
					HLIB_RWSTREAM_Write(ulSD, (void *)&pString->eType, sizeof(DxIPEPG_Text_e));
					HLIB_RWSTREAM_Write(ulSD, (void *)pString->ucLang, EPG_HUMAX_LANG_LEN);
					HLIB_RWSTREAM_Write(ulSD, (void *)pString->ucStr , pString->usSize+1);
				}
			}

			// Write Media list Info
			HLIB_RWSTREAM_Write(ulSD, (void *)&(pEvent->ulMediaCount) , sizeof(pEvent->ulMediaCount));
			for(i = 0; i< pEvent->ulMediaCount; i++)
			{
				HUINT16	usSize = 0;
				DxIpEventStr_t * pString = NULL;
				HLIB_RWSTREAM_Write(ulSD, (void *)&pEvent->stMedia[i].ulPubDate, sizeof(pEvent->stMedia[i].ulPubDate));
				HLIB_RWSTREAM_Write(ulSD, (void *)&pEvent->stMedia[i].ulExPireDate, sizeof(pEvent->stMedia[i].ulExPireDate));
				HLIB_RWSTREAM_Write(ulSD, (void *)&pEvent->stMedia[i].ulTransmitDate, sizeof(pEvent->stMedia[i].ulTransmitDate));
				HLIB_RWSTREAM_Write(ulSD, (void *)&pEvent->stMedia[i].ulPrice, sizeof(pEvent->stMedia[i].ulPrice));

				// MediaID
				pString = &pEvent->stMedia[i].stMediaId;
				usSize = pString->usSize?(pString->usSize+1):0;
				ipepg_StreamWrite_DxIpEventStr(ulSD, usSize, pString);

				// MEDIAURLS
				HLIB_RWSTREAM_Write(ulSD, (void*)&pEvent->stMedia[i].ulMediaUrlCount, sizeof(pEvent->stMedia[i].ulMediaUrlCount) );
				for( j = 0; j < pEvent->stMedia[i].ulMediaUrlCount ; j++)
				{
					// TARGET
					pString = &pEvent->stMedia[i].stMediaUrl[j].stTarget;
					usSize = pString->usSize?(pString->usSize+1):0;
					ipepg_StreamWrite_DxIpEventStr(ulSD, usSize, pString);

					// TYPE
					pString = &pEvent->stMedia[i].stMediaUrl[j].stType;
					usSize = pString->usSize?(pString->usSize+1):0;
					ipepg_StreamWrite_DxIpEventStr(ulSD,usSize,pString);

					// URL
					pString = &pEvent->stMedia[i].stMediaUrl[j].stUrl;
					usSize = pString->usSize?(pString->usSize+1):0;
					ipepg_StreamWrite_DxIpEventStr(ulSD,usSize,pString);
				}

				// IMAGES
				HLIB_RWSTREAM_Write(ulSD, (void*)&pEvent->stMedia[i].ulMediaImageCount, sizeof(pEvent->stMedia[i].ulMediaImageCount));
				for( j = 0; j < pEvent->stMedia[i].ulMediaImageCount; j++)
				{
					// SIZE
					HLIB_RWSTREAM_Write(ulSD, (void*)&pEvent->stMedia[i].stMediaImage[j].ulUrlSize, sizeof(pEvent->stMedia[i].stMediaImage[j].ulUrlSize) );

					// TYPE
					pString = &pEvent->stMedia[i].stMediaImage[j].stUrlType;
					usSize = pString->usSize?(pString->usSize+1):0;
					ipepg_StreamWrite_DxIpEventStr(ulSD,usSize,pString);

					// URL
					pString =&pEvent->stMedia[i].stMediaImage[j].stImageUrl;
					usSize = pString->usSize?(pString->usSize+1):0;
					ipepg_StreamWrite_DxIpEventStr(ulSD,usSize,pString);
				}
			}

			//HxLOG_Print("eventUid[%lld] SIZE BASE[%d] EVENT[%d] binsize[%d]\n",eventUid, sizeof(DxIpEventBase_t), sizeof(DxIpEvent_t), ulBinSize);

			if(APK_META_QUERY_SetIpEvent(eventUid, HLIB_RWSTREAM_GetBuf(ulSD), ulBinSize) != ERR_OK)
			{
				HxLOG_Error("DAPI SET FAIL~~~~~~~~~~~~~~~~~~~~~~~~\n");
			}
			HLIB_RWSTREAM_Close(ulSD);
		}
		else if(pUpdateEvent->eUpdateType == eIPEPG_EVENT_TYPE_DELETE)
		{
			//HxLOG_Print("DELETE eventUid[%lld]\n", eventUid);
			APK_META_QUERY_ResetIpEvent(eventUid);
		}

#ifdef IPEPG_EVENT_UPDATE_SLEEP
		VK_TASK_Sleep(IPEPG_EVENT_UPDATE_SLEEP);
#endif
	}

	return FALSE;
}

#if defined(IPEPG_USE_COMPRESS)
static HERROR	ipepg_DecompressData(HUINT8 * pucReceivedData, HUINT32 ulSize, HUINT8 ** DecompData, HUINT32 * pulSize)
{
	IPEPG_RETURN_IF(pucReceivedData == NULL || ulSize == 0, ERR_FAIL);

	//HxLOG_Debug("File Size [%d]\n", ulSize);
	HLIB_FILE_Delete(IPEPG_TEMP_ZIP_FILE);
	HLIB_FILE_Delete(IPEPG_TEMP_DECOMPRESS_FILE);

	IPEPG_RETURN_IF(HLIB_FILE_WriteFile(IPEPG_TEMP_ZIP_FILE, pucReceivedData, ulSize) != ERR_OK, ERR_FAIL);
	IPEPG_RETURN_IF(HLIB_FILE_Exist(IPEPG_TEMP_ZIP_FILE) != TRUE, ERR_FAIL);

	if(HLIB_GZ_Uncompress(IPEPG_TEMP_ZIP_FILE, IPEPG_TEMP_DECOMPRESS_FILE) == ERR_OK)
	{
		IPEPG_RETURN_IF(HLIB_FILE_Exist(IPEPG_TEMP_DECOMPRESS_FILE) != TRUE, ERR_FAIL);
		IPEPG_MemFree(pucReceivedData);
		IPEPG_RETURN_IF(HLIB_FILE_ReadFile(IPEPG_TEMP_DECOMPRESS_FILE, (void **)DecompData, pulSize) != ERR_OK, ERR_FAIL);
	}

	HLIB_FILE_Delete(IPEPG_TEMP_ZIP_FILE);
	HLIB_FILE_Delete(IPEPG_TEMP_DECOMPRESS_FILE);

	return ERR_OK;
}
#endif

static HERROR	ipepg_ChLogoSendToDama(HxList_t *phSvcList, EPG_URL_INFO_t	*EventURLInfo)
{
	HxList_t	*pTempList = NULL;
	HUINT8 		*pURL = NULL;

	IPEPG_RETURN_IF(EventURLInfo == NULL, ERR_FAIL);
	IPEPG_RETURN_IF(phSvcList == NULL, ERR_FAIL);

	pTempList = phSvcList;
	pURL = EventURLInfo->pucThumbnailUrl;
	while(pTempList != NULL)
	{
		IPEPG_SvcInfo_t *ipSvcInfo = HLIB_LIST_Data(pTempList);
		if(ipSvcInfo != NULL)
		{
			if(ipSvcInfo->stTripleId.usSvcId == EventURLInfo->stTripleId.usSvcId
				&& ipSvcInfo->stTripleId.usOnId == EventURLInfo->stTripleId.usOnId
				&& ipSvcInfo->stTripleId.usTsId == EventURLInfo->stTripleId.usTsId
#ifdef CONFIG_FUNC_EPG_USE_SVCUID
				&& ipSvcInfo->ulUid == EventURLInfo->ulSvcUid
#endif
				)
			{
				DxChannelLogo_t * pstChannelLogo = (DxChannelLogo_t *)IPEPG_MemAlloc(sizeof(DxChannelLogo_t));
				//HxLOG_Debug("There is no same data\n");
				if(pstChannelLogo != NULL)
				{
					HBOOL		bAbleToUseHdd = FALSE;
					HxSTD_MemSet(pstChannelLogo, 0, sizeof(DxChannelLogo_t));
					pstChannelLogo->svcuid = ipSvcInfo->ulUid;
					HxSTD_MemCopy(pstChannelLogo->ucServerLogoUrl, pURL, CHANNEL_LOGO_URL_LENGTH);
					bAbleToUseHdd = (HBOOL)IPEPG_GetItemValue(eIPEPG_ITEM_HDD_STATUS);

					if(bAbleToUseHdd == TRUE)
					{
						HxLOG_Debug("UID : %d, SVCID : %04x ONID : %04x TSID : %04x URL[%s]\n", ipSvcInfo->ulUid, EventURLInfo->stTripleId.usSvcId, EventURLInfo->stTripleId.usOnId, EventURLInfo->stTripleId.usTsId, pURL);
						if(pstChannelLogo->ucServerLogoUrl)
						{
							HCHAR *tmpURL;

							tmpURL = (HCHAR *)THAPI_GetThumbnailURL(pstChannelLogo->ucServerLogoUrl, 0, IPEPG_CHANNEL_LOGO_MOUNT_PATH, TRUE);
							if(tmpURL)
							{
								HxSTD_MemSet(pstChannelLogo->ucLocalLogoUrl, 0, CHANNEL_LOGO_URL_LENGTH);
								HxSTD_MemCopy(pstChannelLogo->ucLocalLogoUrl, tmpURL, HxSTD_StrLen(tmpURL));
								HxLOG_Debug("Local URL: %s\n", pstChannelLogo->ucLocalLogoUrl);
							}
						}
						APK_META_QUERY_SetIpLogo(ipSvcInfo->ulUid, pstChannelLogo, sizeof(DxChannelLogo_t));
						HLIB_STD_TaskSleep(20);
					}
					IPEPG_MemFree(pstChannelLogo);
				}
#ifdef CONFIG_FUNC_EPG_USE_SVCUID
				break;
#endif
			}
		}
		pTempList = pTempList->next;
	}
	return ERR_OK;
}

static void ipepg_MemFreeEvent(void *mem)
{
	if(mem)
	{
		HUINT32 i = 0, j = 0;
		IPEPG_UpdateEvent_t * pUpdateEvent = (IPEPG_UpdateEvent_t *)mem;
		DxIpEvent_t * event = NULL;
		if(pUpdateEvent)
			event = pUpdateEvent->pIpEvent;

		if(event)
		{
			for(i = 0; i< event->ulStrCount; i++)
			{
				IPEPG_MemFree(event->stText[i].ucStr);
			}

			for(i = 0; i< event->ulLinkCount; i++)
			{
				IPEPG_MemFree(event->stLink[i].stSubName.ucStr);
			}

			for(i = 0; i < event->ulMediaCount; i++)
			{
				IPEPG_MemFree(event->stMedia[i].stMediaId.ucStr);

				// MEDIAURLS
				for (j = 0; j < event->stMedia[i].ulMediaUrlCount; j++ )
				{
					IPEPG_MemFree(event->stMedia[i].stMediaUrl[j].stTarget.ucStr);
					IPEPG_MemFree(event->stMedia[i].stMediaUrl[j].stType.ucStr);
					IPEPG_MemFree(event->stMedia[i].stMediaUrl[j].stUrl.ucStr);
				}

				// IMAGES
				for (j = 0; j< event->stMedia[i].ulMediaImageCount; j++ )
				{
					IPEPG_MemFree(event->stMedia[i].stMediaImage[j].stImageUrl.ucStr);
					IPEPG_MemFree(event->stMedia[i].stMediaImage[j].stUrlType.ucStr);
				}
			}

			IPEPG_MemFree(event);
		}
		IPEPG_MemFree(pUpdateEvent);
	}
}

static HERROR	ipepg_SetIpSvcCount(HxList_t *ListOfEventURL)
{
	HxList_t	*pListOfEventURL = ListOfEventURL;
	HUINT32		len = 0;

	len = HLIB_LIST_Length(pListOfEventURL);

	IPEPG_SetItem(eIPEPG_ITEM_SVC_COUNT, (void *)len);
	return ERR_OK;
}

static HERROR	ipepg_SetIpEvtCount(HUINT32 ulDownloadCount)
{
	HUINT32	ulDownloadEvtCount = ulDownloadCount;

	IPEPG_SetItem(eIPEPG_ITEM_EVT_COUNT, (void *)ulDownloadEvtCount);
	return ERR_OK;
}

void IPEPG_MemFreeEvent(void *mem)
{
	ipepg_MemFreeEvent(mem);
}

static HUINT32	ipepg_DownloadEventList(HUINT8 *pURL, DxIpService_t *ipSvcInfo, HBOOL * pbDownloadFail)
{
	HUINT32		hError = ERR_FAIL;
	HUINT32 	ulEventCount = 0;
	HUINT8		*pucReceivedData = NULL;
	HUINT32		ulReceivedSize =0;
	HUINT8		pucURL[IPEPG_URL_MAX_LEN]={0,};
	HUINT32		ulRetryCount = 0;

	IPEPG_FUNC_START();
	IPEPG_RETURN_IF(pURL == NULL, ERR_FAIL);
	IPEPG_RETURN_IF(ipSvcInfo == NULL, ERR_FAIL);
	IPEPG_RETURN_IF(pbDownloadFail == NULL, ERR_FAIL);

	HLIB_STD_URLEncode(pucURL,pURL,IPEPG_URL_MAX_LEN);
	pucReceivedData = (HUINT8 *)IPEPG_MemCalloc(IPEPG_EVENT_DATA_MAX_SIZE);
	ulReceivedSize = 0;

RETRY_EPG_DOWNLOAD:
	s_epg_buffer = pucReceivedData;
	s_epgdata_size = 0;
	errno = 0;

	// ----------------------------------------------------------------------------------------
    hError = ipepg_SettingHttpGetWithCallback(pURL, ipepg_DownloadCallback, IPEPG_HTTP_TIMEOUT);
	// ----------------------------------------------------------------------------------------

	#if defined(CONFIG_DEBUG)
	// Network 에러 상태 테스트를 위해서.
	if( IPEPG_GetNetworkTestValue() == 1)
	{
		hError = ERR_FAIL;
		s_epgdata_size = 0;
		HxLOG_Debug("[FAKE TEST] IPEPG_GetNetworkTestValue is 1. download failed reason by network\n");
	}
	#endif

    if ( hError == ERR_OK  && s_epgdata_size > 0 && pucReceivedData != NULL)
	{
		ulReceivedSize = (HUINT32)s_epgdata_size;
	}
	else
	{
#if defined (IPEPG_DEBUG_DOWNLOAD_XML)
		HUINT8 aDebugMessage[1024];
		HxSTD_MemSet(aDebugMessage, 0, 1024);
		HxSTD_sprintf(aDebugMessage, "ipepg download failed. err(%d) count(%d/%d)\n", hError, ulRetryCount, IPEPG_MAX_DOWNLOAD_RETRY_COUNT);
		IPEPG_DEBUG_SaveXML(ipSvcInfo,pURL,NULL,0);
#endif

		HxLOG_Warning("Event List Download fail URL[%s] err(%d) errno(%d) retry Count(%d/%d)\n", pURL, hError, errno, ulRetryCount, IPEPG_MAX_DOWNLOAD_RETRY_COUNT);

		HxSTD_MemSet(pucReceivedData, 0, IPEPG_SERVICE_LIST_MAX_SIZE);
		ulReceivedSize = 0;
		if(ulRetryCount < IPEPG_MAX_DOWNLOAD_RETRY_COUNT)
		{
			ulRetryCount++;
			HxLOG_Warning(" Event List Download failed. %d seconds after will be tried again. (%d/%d)\n", IPEPG_HTTP_DOWNLOAD_RETRY_WAITTIME, ulRetryCount, IPEPG_MAX_DOWNLOAD_RETRY_COUNT);

			// 서버에 부하를 줄이기 위해, 지정된 시간 이후에 재시도.
			HLIB_STD_TaskSleep(IPEPG_HTTP_DOWNLOAD_RETRY_WAITTIME*1000);
			goto RETRY_EPG_DOWNLOAD;
		}
		else
		{
			// 다운로드 중지
			*pbDownloadFail = TRUE;
			HxLOG_Warning(" Event List Download failed. will be stopped the EPG Download and next time will be downloaded. (%d/%d)\n", ulRetryCount, IPEPG_MAX_DOWNLOAD_RETRY_COUNT);
		}
	}

	if(ulReceivedSize != 0)
	{
		HxList_t *ppEventList = NULL;
		HBOOL		bAbleToUseHdd = FALSE;

		HxLOG_Debug(" receivedSize [%d]\n",ulReceivedSize);

#if defined(IPEPG_USE_COMPRESS)
		if(ipepg_DecompressData(pucReceivedData, ulReceivedSize, &pucReceivedData, &ulReceivedSize) != ERR_OK)
		{
			HxLOG_Debug("Event list decomp Error\n");
		}
#endif
		bAbleToUseHdd = (HBOOL)IPEPG_GetItemValue(eIPEPG_ITEM_HDD_STATUS);
		if(bAbleToUseHdd == TRUE)
		{
#if defined (IPEPG_DEBUG_DOWNLOAD_XML)
			IPEPG_DEBUG_SaveXML(ipSvcInfo,pURL,pucReceivedData,ulReceivedSize);
#endif

			ipepg_ParseEventList(pucReceivedData, ulReceivedSize, &ppEventList, ipSvcInfo->ulSvcUid);

			if(ppEventList != NULL)
			{
				// Send data to DAMA
				ulEventCount =	HLIB_LIST_Length(ppEventList);
				if(ulEventCount != 0)
				{
					HxLOG_Debug("pURL : %s\n",pURL);
					HxLOG_Debug("treipl[%d] [%d] [%d] Svcuid[%d] ulEventCount : %d\n",ipSvcInfo->usOnId,ipSvcInfo->usTsId,ipSvcInfo->usSvcId,ipSvcInfo->ulSvcUid,ulEventCount);

					bAbleToUseHdd = (HBOOL)IPEPG_GetItemValue(eIPEPG_ITEM_HDD_STATUS);
					if(bAbleToUseHdd == TRUE)
					{
						HLIB_STD_TaskSleep(IPEPG_DELAYED_TIME);
						APK_META_QUERY_SetIpSvc(ipSvcInfo->ulSvcUid, ipSvcInfo, sizeof(DxIpService_t));
						HLIB_LIST_ForeachEx(ppEventList , ipepg_EventSendToDama, (void *)eIPEPG_EVENT_TYPE_DELETE);
						HLIB_LIST_ForeachEx(ppEventList , ipepg_EventSendToDama, (void *)eIPEPG_EVENT_TYPE_UPDATE);

						// Debugging code for UPDATE event test.
						#if defined (IPEPG_DEBUG_DOWNLOAD_XML)
						{
							HxList_t *list = ppEventList;
							IPEPG_UpdateEvent_t * pUpdateEvent = NULL;
							HBOOL bHasDelete = FALSE;

							while(list)
							{
								pUpdateEvent = (IPEPG_UpdateEvent_t *)list->data;
								if(pUpdateEvent && pUpdateEvent->eUpdateType == eIPEPG_EVENT_TYPE_DELETE)
								{
									bHasDelete = TRUE;
									break;
								}
								list = list->next;
							}
							if(bHasDelete == TRUE)
							{
								IPEPG_DEBUG_SaveModifyEventXML(ipSvcInfo,pURL,pucReceivedData,ulReceivedSize);
							}
						}
						#endif

						#if !defined(IPEPG_THUMBNAIL_NON_DOWN)
							if(APK_POWER_IsStandbyMode() == TRUE)
							{
								HLIB_LIST_Foreach(ppEventList , ipepg_GetThumbnail);
							}
						#endif
					}
					HLIB_LIST_Foreach(ppEventList , ipepg_MemFreeEvent);
				}
				else
				{
					HxLOG_Debug("Event list is NULL CRITICAL CRITICAL\n");
				}
				HLIB_LIST_RemoveAll(ppEventList);
			}
			else
			{
				HxLOG_Debug("Event list is NULL\n");
			}
		}
	}

	IPEPG_MemFree(pucReceivedData);
	IPEPG_FUNC_END()
	return ulEventCount;
}

static HERROR	ipepg_DownloadSvcList(IPEPG_DATAMGR_t 	* pstData, HxList_t **ListOfEventURL)
{
	HERROR		hError = ERR_FAIL;
	HUINT8		pServiceListUrl[IPEPG_URL_MAX_LEN]={0,};

	HUINT8		*pucReceivedData = NULL;
	HUINT32		ulReceivedSize = 0;

	HUINT8		*pucMaccAddr = NULL;
	HUINT8		*pServerURL = NULL;
	IPEPG_FUNC_START();

	pucMaccAddr = IPEPG_GetItemPoint(eIPEPG_ITEM_MAC);
	pServerURL = IPEPG_GetItemPoint(eIPEPG_ITEM_SERVERURL);

	if(pucMaccAddr == NULL || pServerURL == NULL)
	{
		IPEPG_MemFree(pucMaccAddr);
		IPEPG_MemFree(pServerURL);
		IPEPG_FUNC_END();
		return ERR_FAIL;
	}

	HxSTD_snprintf(pServiceListUrl, IPEPG_URL_MAX_LEN, "%s/%s%s", pServerURL, IPEPG_SERVICELIST_URL, pucMaccAddr);

	HxLOG_Debug("URL : %s \n", pServiceListUrl);
	IPEPG_MemFree(pucMaccAddr);
	IPEPG_MemFree(pServerURL);

	pucReceivedData = (HUINT8 *)IPEPG_MemCalloc(IPEPG_SERVICE_LIST_MAX_SIZE);

	HxSTD_MemSet(pucReceivedData, 0, IPEPG_SERVICE_LIST_MAX_SIZE);
	s_epg_buffer = pucReceivedData;
	s_epgdata_size = 0;

	hError = ipepg_SettingHttpGetWithCallback(pServiceListUrl, ipepg_DownloadCallback, IPEPG_HTTP_TIMEOUT);
	if(hError != ERR_OK)
	{
		IPEPG_MemFree(pucReceivedData);
		IPEPG_FUNC_END();
		return ERR_FAIL;
	}

	ulReceivedSize = (HUINT32)s_epgdata_size;

	//HxLOG_Debug("SERVICE LIST DATA: [%d]\n", ulReceivedSize);
#if defined(IPEPG_USE_COMPRESS)
	if(ipepg_DecompressData(pucReceivedData, ulReceivedSize, &pucReceivedData, &ulReceivedSize) != ERR_OK)
	{
		HxLOG_Error("Service list decomp Error\n");
	}
	else
	{
		HxLOG_Debug("[%d] [%s] \n", ulReceivedSize, pucReceivedData);
	}
#endif

	ipepg_ParseServiceList(pucReceivedData, ulReceivedSize, ListOfEventURL);
	IPEPG_MemFree(pucReceivedData);
	IPEPG_FUNC_END();

	if(NULL == ListOfEventURL)
	{
		return ERR_FAIL;
	}
	else
	{
		return ERR_OK;
	}
}

void	IPEPG_DeleteAllData(void)
{
	APK_META_QUERY_ResetIpEventTable();
	APK_META_QUERY_ResetIpSvcTable();
	APK_META_QUERY_ResetIpLogoTable();
	HxLOG_Print("IPEPG_DeleteAllData Finished\n");
}

void ipepg_FreeQuery(void *mem)
{
	if(mem)
	{
		EPG_URL_Query_t *query = mem;
		if(query)
		{
			IPEPG_MemFree(query->pucKey);
			IPEPG_MemFree(query->pucValue);
			IPEPG_MemFree(query);
		}
	}
}

HBOOL ipepg_CompareQuery(void *user_data,void *list_data)
{
	if(HxSTD_StrCmp(((EPG_URL_Query_t *)user_data)->pucKey, ((EPG_URL_Query_t *)list_data)->pucKey) == 0)
		return TRUE;
	return FALSE;
}

HERROR ipepg_AddQuery(EPG_URL_PARSER_t * UrlParser, void *mem)
{
	HxList_t * FindList = NULL;
	EPG_URL_Query_t * FindQuery = NULL;

	if(UrlParser == NULL || mem == NULL) return ERR_FAIL;

	FindList = (HxList_t *)HLIB_LIST_FindEx(UrlParser->pQuery, mem, ipepg_CompareQuery);
	if(FindList != NULL && FindList->data != NULL)
	{
		FindQuery = (EPG_URL_Query_t *)HLIB_LIST_Data(FindList);
		if(FindQuery != NULL)
		{
			UrlParser->pQuery = HLIB_LIST_Remove(UrlParser->pQuery,(const void *)FindQuery);
			ipepg_FreeQuery(FindQuery);
			FindQuery = NULL;
		}	
	}

	UrlParser->pQuery =	HLIB_LIST_Append(UrlParser->pQuery,mem);
	return ERR_OK;
}

HERROR ipepg_MakeQuery(EPG_URL_PARSER_t * UrlParser, HCHAR * pKey, HCHAR * value)
{
	EPG_URL_Query_t *pQuery = NULL;
	if(UrlParser == NULL || pKey == NULL || value == NULL)
		return ERR_FAIL;

	pQuery = (EPG_URL_Query_t *)IPEPG_MemCalloc(sizeof(EPG_URL_Query_t));
	pQuery->pucKey = IPEPG_StrDup(pKey);
	pQuery->pucValue = IPEPG_StrDup(value);
	if(ipepg_AddQuery(UrlParser, pQuery) != ERR_OK)
	{
		ipepg_FreeQuery(pQuery);
		return ERR_FAIL;
	}

	return ERR_OK;
}

void ipepg_MakeURLWithParser(EPG_URL_PARSER_t * parser, HUINT8 *pURL)
{
	HUINT8 		*tempURL = pURL;
	HxList_t 	*list = NULL;
	EPG_URL_Query_t *pQuery = NULL;

	if(parser == NULL || pURL == NULL) return;

	HxSTD_MemSet(tempURL, 0, IPEPG_URL_MAX_LEN);

	if(parser->pucHeader)
	{
		HxSTD_snprintf(tempURL, IPEPG_URL_MAX_LEN,"%s://", parser->pucHeader);
		tempURL = tempURL+HxSTD_StrLen(parser->pucHeader) + 3;
	}

	if(parser->pucHost)
	{
		if(parser->pucPort)
		{
			HxSTD_snprintf(tempURL, IPEPG_URL_MAX_LEN,"%s:%s/", parser->pucHost, parser->pucPort);
			tempURL = tempURL+HxSTD_StrLen(parser->pucHost) + HxSTD_StrLen(parser->pucPort) + 2;
		}
		else
		{
			HxSTD_snprintf(tempURL, IPEPG_URL_MAX_LEN,"%s/", parser->pucHost);
			tempURL = tempURL+HxSTD_StrLen(parser->pucHost) + 1;
		}
	}

	if(parser->pucPath)
	{
		HxSTD_snprintf(tempURL, IPEPG_URL_MAX_LEN,"%s?", parser->pucPath);
		tempURL = tempURL+HxSTD_StrLen(parser->pucPath) + 1;
	}

	list = parser->pQuery;

	while(list)
	{
		pQuery = (EPG_URL_Query_t *)list->data;

		if(pQuery->pucKey && pQuery->pucValue)
		{
			HxSTD_snprintf(tempURL, IPEPG_URL_MAX_LEN,"%s=%s", pQuery->pucKey, pQuery->pucValue);
			tempURL = tempURL + HxSTD_StrLen(pQuery->pucKey) + HxSTD_StrLen(pQuery->pucValue) + 1;
		}

		list = HLIB_LIST_Next(list);
		if(list)
		{
			HxSTD_snprintf(tempURL, IPEPG_URL_MAX_LEN,"&");
			tempURL = tempURL + 1;
		}
	}

	return;
}

static void ipepg_FreeUrlParser(EPG_URL_PARSER_t * parser)
{
	if(parser != NULL)
	{
		if(parser->pQuery)
		{
			HLIB_LIST_Foreach(parser->pQuery,ipepg_FreeQuery);
			HLIB_LIST_RemoveAll(parser->pQuery);
		}
		IPEPG_MemFree(parser->pucHeader);
		IPEPG_MemFree(parser->pucHost);
		IPEPG_MemFree(parser->pucPort);
		IPEPG_MemFree(parser->pucPath);
		IPEPG_MemFree(parser);
	}
}

EPG_URL_PARSER_t *ipepg_UrlParse(HUINT8 * pUrl)
{
#define SIZE_OF_BUF	256
	EPG_URL_PARSER_t 	*pstParser = NULL;
	HUINT32 			ulUrlLength = 0;
	HUINT32 			i = 0, j = 0;
	IPEPG_URL_STEP_e 	eStep = eIPEPG_URL_HEAD;
	HUINT8 				tempBuffer[SIZE_OF_BUF] ={};
	EPG_URL_Query_t 	*query = NULL;


	pstParser = (EPG_URL_PARSER_t *)IPEPG_MemCalloc(sizeof(EPG_URL_PARSER_t));

	if(pUrl == NULL)	return pstParser;

	HxSTD_MemSet(tempBuffer, 0, SIZE_OF_BUF);
	ulUrlLength = HxSTD_StrLen(pUrl);

	for(i = 0; i < ulUrlLength + 1 && j < SIZE_OF_BUF ; i++)
	{
		IPEPG_URL_STEP_e eOldStep = eStep;
		tempBuffer[j] = pUrl[i];

		if(eStep == eIPEPG_URL_HEAD && pUrl[i] == ':')
		{
			tempBuffer[j] = 0;
			pstParser->pucHeader = IPEPG_StrDup(tempBuffer);
			eStep++;
			i+=2;	// '/' skip one more	"://"
		}
		else if(eStep == eIPEPG_URL_HOST
			&& (pUrl[i] == '/' || pUrl[i] == ':'))
		{
			tempBuffer[j] = 0;
			pstParser->pucHost = IPEPG_StrDup(tempBuffer);
			eStep++;

			if(pUrl[i] == '/')	// Next Step is Path. No Port
				eStep++;
		}
		else if(eStep == eIPEPG_URL_PORT && pUrl[i] == '/')
		{
			tempBuffer[j] = 0;
			pstParser->pucPort = IPEPG_StrDup(tempBuffer);
			eStep++;
		}
		else if(eStep == eIPEPG_URL_PATH && pUrl[i] == '?')
		{
			tempBuffer[j] = 0;
			pstParser->pucPath = IPEPG_StrDup(tempBuffer);
			eStep++;
		}
		else if(eStep >= eIPEPG_URL_QUERY
			&&(pUrl[i] == '&' || pUrl[i] == '=' || pUrl[i] == 0))
		{
			tempBuffer[j] = 0;
			if(query == NULL)
			{
				query = (EPG_URL_Query_t *)IPEPG_MemCalloc(sizeof(EPG_URL_Query_t));
			}

			if(query&& tempBuffer[0] != 0x0)
			{
				if(pUrl[i] == '=')
				{
					query->pucKey = IPEPG_StrDup(tempBuffer);
					eStep++;
				}
				else if(pUrl[i] == '&')
				{
					query->pucValue = IPEPG_StrDup(tempBuffer);
					pstParser->pQuery = HLIB_LIST_Append(pstParser->pQuery,query);

					query = NULL;
					eStep++;
				}
				else if(pUrl[i] == 0)
				{
					query->pucValue = IPEPG_StrDup(tempBuffer);
					pstParser->pQuery = HLIB_LIST_Append(pstParser->pQuery,query);

					query = NULL;
				}
			}
		}
		if(eOldStep == eStep)
			j++;
		else
		{
			HxSTD_MemSet(tempBuffer, 0, SIZE_OF_BUF);
			j = 0;
		}
	}

	if(query)
	{
		IPEPG_MemFree(query->pucKey);
		IPEPG_MemFree(query->pucValue);
		IPEPG_MemFree(query);
	}

	if(pstParser)
		return pstParser;
	else
		return NULL;
}

#if defined(IPEPG_PRIORITY_DOWN)
static HERROR	ipepg_LoadXML(HxList_t **ListOfPriority)
{
	HERROR				hError = ERR_FAIL;

	HxXMLDOC			hXmlDocHandle = NULL;

	HxXMLELEMENT		hRoot = NULL;
	HxXMLELEMENT		hRow = NULL;
	HxXMLELEMENT		hOnId = NULL;
	HxXMLELEMENT		hTsId = NULL;
	HxXMLELEMENT		hSvcId = NULL;

	EPG_URL_INFO_t		*EventURLInfo = NULL;

	HxList_t			*pTempListOfPriority = NULL;

	HxList_t			*lRow = NULL,*FirstlRow = NULL;
	HxList_t			*lOnId = NULL;
	HxList_t			*lTsId = NULL;
	HxList_t			*lSvcId = NULL;

	HUINT8				*contents = NULL;
	HUINT8				*fileName = NULL;
	const	HUINT8 		*ptemp = NULL;

	HxCountry_e 		eCountryCode = eCountryID_NONE;
	HUINT8				szIso3166Code[4];
	HUINT8				*country = NULL;

	HUINT32				pos = 0;

	*ListOfPriority = NULL;

	fileName = (HUINT8 *)IPEPG_MemAlloc(128);
	HxSTD_MemSet(fileName , 0, 128);

	APK_SCENARIO_GetCountryCode(&eCountryCode);

	ptemp = HLIB_COUNTRY_IdTo3166_Alpha3((const HxCountry_e)eCountryCode);
	if(NULL != ptemp)
	{
		//Do not add duplicated triple id into the file(/etc/deu.xml)
		HxSTD_MemCopy(szIso3166Code, ptemp, 4);
		country = HLIB_STD_StrLower(szIso3166Code);
		HxSTD_snprintf(fileName, 128, "%s%s%s", IPEPG_PRIORITY_XML_PATH, country, ".xml");
		HxLOG_Print("[%s] XML FileName[%s] \n", __FUNCTION__,fileName);
	}
	else
	{
		goto END_FUNC;
	}

	hError = HLIB_XML_InitParser();
	if(hError != ERR_OK)
	{
		HxLOG_Warning("[%s] HLIB_XML_InitParser error\n", __FUNCTION__);
		goto END_FUNC;
	}

	hError = HLIB_XML_ParseFile(fileName, &hXmlDocHandle);
	if(hError != ERR_OK)
	{
		HxLOG_Warning("[%s] HLIB_XML_ParseFile error\n", __FUNCTION__);
		goto END_FUNC;
	}

	hError = HLIB_XML_GetRootElement(hXmlDocHandle, &hRoot);
	if(hError != ERR_OK)
	{
		HxLOG_Warning("[%s] HLIB_XML_GetRootElement error\n", __FUNCTION__);
		goto END_FUNC;
	}

	HLIB_XML_FindChildElementByName(hRoot, "row", &lRow);

	FirstlRow = lRow;

	while(lRow != NULL)
	{
		EventURLInfo = IPEPG_MemAlloc(sizeof(EPG_URL_INFO_t));
		HxSTD_MemSet(EventURLInfo , 0, sizeof(EPG_URL_INFO_t));
		hRow = (HxXMLELEMENT)HLIB_LIST_Data(lRow);

		HLIB_XML_FindChildElementByName(hRow, "onid", &lOnId);
		hOnId = (HxXMLELEMENT)HLIB_LIST_Data(lOnId);
		HLIB_XML_GetElementContents(hXmlDocHandle,hOnId,&contents);
		EventURLInfo->stTripleId.usOnId = HLIB_STD_StrToUINT32(contents);

		HLIB_XML_FindChildElementByName(hRow, "tsid", &lTsId);
		hTsId = (HxXMLELEMENT)HLIB_LIST_Data(lTsId);
		HLIB_XML_GetElementContents(hXmlDocHandle,hTsId,&contents);
		EventURLInfo->stTripleId.usTsId = HLIB_STD_StrToUINT32(contents);

		HLIB_XML_FindChildElementByName(hRow, "svcid", &lSvcId);
		hSvcId = (HxXMLELEMENT)HLIB_LIST_Data(lSvcId);
		HLIB_XML_GetElementContents(hXmlDocHandle,hSvcId,&contents);
		EventURLInfo->stTripleId.usSvcId = HLIB_STD_StrToUINT32(contents);

		lRow = HLIB_LIST_Next(lRow);
		pTempListOfPriority = HLIB_LIST_Insert(pTempListOfPriority,EventURLInfo,pos);
		pos++;

		if(lOnId != NULL)
		{
			HLIB_LIST_RemoveAll(lOnId);
			lOnId = NULL;
		}

		if(lTsId != NULL)
		{
			HLIB_LIST_RemoveAll(lTsId);
			lTsId = NULL;
		}

		if(lSvcId != NULL)
		{
			HLIB_LIST_RemoveAll(lSvcId);
			lSvcId = NULL;
		}

		HxLOG_Print("[%s] EventURLInfo onid[%x] tsid[%x] svcid[%x] \n", __FUNCTION__,EventURLInfo->stTripleId.usOnId, EventURLInfo->stTripleId.usTsId, EventURLInfo->stTripleId.usSvcId);
	}

	*ListOfPriority = pTempListOfPriority;
	hError = ERR_OK;

	HxLOG_Print("[%s] Priority List Size : [%d]\r\n",__FUNCTION__,HLIB_LIST_Length(pTempListOfPriority));

END_FUNC:
	if(FirstlRow)
	{
		HLIB_LIST_RemoveAll(FirstlRow);
	}
	if(hXmlDocHandle)
	{
		HLIB_XML_FreeDocument(hXmlDocHandle);
	}
	if(fileName)
	{
		IPEPG_MemFree(fileName);
	}

	return hError;
}

static HBOOL ipepg_CompTripleId(void *pstUserTripleId, void* pstListData)
{
	EPG_URL_INFO_t		*pEpg = (EPG_URL_INFO_t *)pstListData;
	IPEPG_TripleId_t	*pTripleId1, *pTripleId2;

	pTripleId1 = &pEpg->stTripleId;
	pTripleId2 = (IPEPG_TripleId_t *)pstUserTripleId;

	if (pTripleId1->usOnId == pTripleId2->usOnId &&
		pTripleId1->usTsId == pTripleId2->usTsId &&
		pTripleId1->usSvcId == pTripleId2->usSvcId)
	{
		return TRUE;
	}

	return FALSE;
}
#ifdef CONFIG_FUNC_EPG_USE_SVCUID
// Warning !! A Function for model supported LCN
static HxList_t *ipepg_RearrangementListOfEventURL(HxList_t *ListOfPriority, HxList_t *ListOfEventURL, HxList_t* phSvcList)
{
	HxList_t		*pListOfEventURL	= ListOfEventURL;	// Channel List from Server
	HxList_t		*pListOfPriority	= ListOfPriority;	// Priority IPEPG Channel List From XML File(/etc/deu.xml)
	HxList_t		*pTempSvcList		= NULL;				// Channel List Made by ipepg_GetServiceFromAPK function
	HxList_t		*pTemp				= NULL;
	HINT32			position			= 0;
	EPG_URL_INFO_t	*EventPriority		= NULL;				// has triple id, no svcuid
	EPG_URL_INFO_t	*EventURL			= NULL;				// has triple id, svcuid
	IPEPG_SvcInfo_t	*ipSvcInfo			= NULL;				// has triple id, svcuid, lcn numver
	IPEPG_SvcInfo_t	stHighPrioritySvcInfo;

	while(pListOfPriority != NULL)
	{
		EventPriority = (EPG_URL_INFO_t *)HLIB_LIST_Data(pListOfPriority);
		if(EventPriority == NULL)
		{
			break;
		}

		if(phSvcList)
		{
			pTempSvcList = phSvcList;

			HxSTD_MemSet(&stHighPrioritySvcInfo, 0, sizeof(IPEPG_SvcInfo_t));

			while(pTempSvcList)
			{
				ipSvcInfo = HLIB_LIST_Data(pTempSvcList);
				if(ipSvcInfo)
				{
					if(ipSvcInfo->stTripleId.usOnId == EventPriority->stTripleId.usOnId
						&& ipSvcInfo->stTripleId.usSvcId == EventPriority->stTripleId.usSvcId
						&& ipSvcInfo->stTripleId.usTsId == EventPriority->stTripleId.usTsId)
					{
						if(stHighPrioritySvcInfo.ulLcn == 0 || stHighPrioritySvcInfo.ulLcn > ipSvcInfo->ulLcn)
						{
							HxSTD_MemCopy(&stHighPrioritySvcInfo, ipSvcInfo, sizeof(IPEPG_SvcInfo_t));
						}
					}
				}
				pTempSvcList = pTempSvcList->next;
			}

			if(stHighPrioritySvcInfo.ulLcn > 0)
			{
				pTemp = pListOfEventURL;
				while(pTemp)
				{
					EventURL = (EPG_URL_INFO_t *)HLIB_LIST_Data(pTemp);
					if(EventURL)
					{
						if(EventURL->ulSvcUid == stHighPrioritySvcInfo.ulUid)
						{
							HxLOG_Debug("[%d] <----- [%d] SvcUid: %d\n", position, stHighPrioritySvcInfo.ulLcn, EventURL->ulSvcUid);
							pListOfEventURL = HLIB_LIST_Remove(pListOfEventURL,(void *)EventURL);
							pListOfEventURL = HLIB_LIST_Insert(pListOfEventURL,EventURL,position);
							position++;
							break;
						}
					}
					pTemp = pTemp->next;
				}
			}
		}
		else
		{
			pTemp = HLIB_LIST_FindEx(pListOfEventURL, (void *)&(EventPriority->stTripleId), ipepg_CompTripleId);

			if(pTemp != NULL)
			{
				EventURL = (EPG_URL_INFO_t *)HLIB_LIST_Data(pTemp);
				pListOfEventURL = HLIB_LIST_Remove(pListOfEventURL,(void *)EventURL);
				pListOfEventURL = HLIB_LIST_Insert(pListOfEventURL,EventURL,position);
				position++;
				pTemp = NULL;
			}
		}

		pListOfPriority = pListOfPriority->next;
	}

	return pListOfEventURL;
}
#else
static HxList_t *ipepg_RearrangementListOfEventURL(HxList_t *ListOfPriority, HxList_t *ListOfEventURL)
{
	HxList_t	*pListOfEventURL = ListOfEventURL;
	HxList_t	*pListOfPriority = ListOfPriority;
	HxList_t	*pTemp = NULL;
	HINT32		position = 0;
	EPG_URL_INFO_t *EventPriority = NULL;
	EPG_URL_INFO_t *EventURL = NULL;

	while(pListOfPriority != NULL)
	{
		EventPriority = (EPG_URL_INFO_t *)HLIB_LIST_Data(pListOfPriority);
		pTemp = HLIB_LIST_FindEx(pListOfEventURL, (void *)&(EventPriority->stTripleId), ipepg_CompTripleId);

		if(pTemp != NULL)
		{
			EventURL = (EPG_URL_INFO_t *)HLIB_LIST_Data(pTemp);
			pListOfEventURL = HLIB_LIST_Remove(pListOfEventURL,(void *)EventURL);
			pListOfEventURL = HLIB_LIST_Insert(pListOfEventURL,EventURL,position);
			position++;
			pTemp = NULL;
		}

		pListOfPriority = pListOfPriority->next;
	}

	return pListOfEventURL;
}
#endif
#endif

#ifdef CONFIG_USE_CHANNEL_LOGO_TASK
static IPEPG_CHLOGO_TASK_Instance_t* ipepg_ChLogoNewInstance(HxList_t* ListOfEventURL)
{
	IPEPG_CHLOGO_TASK_Instance_t	*pstItem = NULL;
	HxList_t						*pEventUrl;
	HxList_t						*NewListOfEventURL = NULL;

	if(ListOfEventURL == NULL)
	{
		return NULL;
	}
	HxLOG_Debug("Create New Instance Enter ...\n");

	pstItem = (IPEPG_CHLOGO_TASK_Instance_t *)HLIB_MEM_Calloc(sizeof(IPEPG_CHLOGO_TASK_Instance_t));
	if (pstItem == NULL)
	{
		return NULL;
	}

	for (pEventUrl = HLIB_LIST_Last(ListOfEventURL); pEventUrl != NULL; pEventUrl = HLIB_LIST_Prev(pEventUrl))
	{
		EPG_URL_INFO_t *pEventUrlInfo = NULL;
		HCHAR *clone = NULL;

		pEventUrlInfo = (EPG_URL_INFO_t *)HLIB_LIST_Data(pEventUrl);
		if (pEventUrlInfo != NULL)
		{
			clone = HLIB_STD_MemDup(pEventUrlInfo, sizeof(EPG_URL_INFO_t));
			NewListOfEventURL = HLIB_LIST_Prepend(NewListOfEventURL, clone);
		}
	}
	HxLOG_Debug("Create New Instance Enter Finished ... \n");

	pstItem->ListOfEventURL	=	NewListOfEventURL;

	return pstItem;
}

static void ipepg_ChLogoTask(void *pstDownMgr)
{
	EPG_URL_INFO_t			*EventURLInfo = NULL;
	IpEpgMsg_t				stMsg;
	IPEPG_TASK_MGR_t		*pstTaskMgr = (IPEPG_TASK_MGR_t *)pstDownMgr;

	while(1)
	{
		if(pstTaskMgr ==NULL)
		{
			HxLOG_Critical("Critical Error, Can not Play Task!!!!\n");
			break;
		}

		if (VK_MSG_ReceiveTimeout(pstTaskMgr->ulChlogoMsgQId, (void*)&stMsg, sizeof(IpEpgMsg_t), 30000) == VK_OK)
		{
			switch (stMsg.ulMsg)
			{
				case eIPEPG_MSG_START_CHLOGO_DOWNLOAD :
					{
						IPEPG_CHLOGO_TASK_Instance_t	*pstInstance = (IPEPG_CHLOGO_TASK_Instance_t *)stMsg.ulParam1;
						HxList_t						*pTmpListOfEventURL;
						HxList_t						*phSvcList = NULL;
						HxList_t						*pTmpSvcList = NULL;
						HxHASH_t						*phSvcHash = NULL;
						HUINT32 						ulCount = 0;

						if(pstInstance)
						{
							phSvcHash = IPEPG_GetItemPoint(eIPEPG_ITEM_SVCLIST);
							phSvcList = HLIB_HASH_GetValues (phSvcHash);
							pTmpSvcList = phSvcList;
							pTmpListOfEventURL	= pstInstance->ListOfEventURL;
							HxLOG_Debug("Request Download Start ...\n");
							pstTaskMgr->eStatus |= eIPEPG_CHLOGO_DOWN_START;
							while(pTmpListOfEventURL != NULL && pTmpSvcList != NULL)
							{
								if(((pstTaskMgr->eStatus & eIPEPG_STOP)==eIPEPG_STOP)||(pstTaskMgr->eStatus == eIPEPG_NONE))
								{
									HxLOG_Debug("Stop !!!\n");
									break;
								}

								if((HBOOL)IPEPG_GetItemValue(eIPEPG_ITEM_FLAG_IPEPG_ONOFF) == FALSE)
								{
									HxLOG_Debug("User Select ... Stop !!!\n");
									break;
								}
								EventURLInfo = (EPG_URL_INFO_t *)HLIB_LIST_Data(pTmpListOfEventURL);

								if(EventURLInfo != NULL)
								{
									HxLOG_Debug("Called ipepg_ChLogoSendToDama ... (pstTaskMgr->eStatus: 0x%x)\n", pstTaskMgr->eStatus);
									ipepg_ChLogoSendToDama(pTmpSvcList, EventURLInfo);
								}
								ulCount++;
								pTmpListOfEventURL = pTmpListOfEventURL->next;
							}
							pstTaskMgr->eStatus &= ~eIPEPG_CHLOGO_DOWN_START;

							if(pstInstance->ListOfEventURL)
							{
								HLIB_LIST_Foreach(pstInstance->ListOfEventURL , IPEPG_MemFree);
								HLIB_LIST_RemoveAll(pstInstance->ListOfEventURL);
								pstInstance->ListOfEventURL = NULL;
							}

							if(phSvcList != NULL)
							{
								// HLIB_LIST_Foreach(phSvcList, IPEPG_MemFree);
								HLIB_LIST_RemoveAll(phSvcList);
							}
							if(phSvcHash != NULL)
							{
								HLIB_HASH_Destroy(phSvcHash);
							}

							HLIB_MEM_Free(pstInstance);
							HxLOG_Debug("Request Download Close ...\n");
						}
#if defined(CONFIG_DEBUG)
						IPEPG_SetDownloadDebugInfo(eIPEPG_DNLD_DBG_CHLOGSENDTODAMA_COUNT, (void *)ulCount);
#endif
					}
					break;
				default :
					HxLOG_Error("ipepg_ChLogoTask reveived OtherMessage[%d]\n", stMsg.ulMsg);
					break;
			}
		}
	}
}

static void ipepg_ChLogoCreateTask(IPEPG_TASK_MGR_t* pstTaskMgr)
{
	if(pstTaskMgr->ulChlogoTaskId == 0)
	{
		HxLOG_Debug("Create Channel Logo Task\n");
		VK_TASK_Create(ipepg_ChLogoTask, VK_TASK_PRIORITY_MW_REF, SIZE_1K, (const char*)"ipepgChLogoTask", (void*)pstTaskMgr, (unsigned long *)&pstTaskMgr->ulChlogoTaskId, 0);
		VK_TASK_Start(pstTaskMgr->ulChlogoTaskId);
	}

	if(pstTaskMgr->ulChlogoMsgQId == 0)
	{
		HxLOG_Debug("Create Channel Logo Msg Q\n");
		if(VK_MSG_Create(16, sizeof(IpEpgMsg_t), "downloadStartMsg", (unsigned long *)&(pstTaskMgr->ulChlogoMsgQId), VK_SUSPENDTYPE_FIFO) != 0)
		{
			pstTaskMgr->ulChlogoMsgQId = 0;
		}
	}
}

static HUINT32	ipepg_ChLogoSendToDamaAll(IPEPG_TASK_MGR_t* pstTaskMgr, HxList_t* ListOfEventURL)
{
	HxList_t				*TempListOfEventURL = ListOfEventURL;
	IpEpgMsg_t				stMsg;
	HUINT32 				ulCount = 0;
	IPEPG_CHLOGO_TASK_Instance_t	*pstItem = ipepg_ChLogoNewInstance(TempListOfEventURL);

	APK_META_QUERY_ResetIpLogoTable();

	if(pstTaskMgr->ulChlogoMsgQId != 0)
	{
		HxSTD_MemSet(&stMsg, 0, sizeof(IpEpgMsg_t));
		stMsg.ulMsg		= eIPEPG_MSG_START_CHLOGO_DOWNLOAD;
		stMsg.ulParam1	= (HUINT32)pstItem;
		HxLOG_Debug("Send Message Channel Logo !!!\n");
		VK_MSG_Send(pstTaskMgr->ulChlogoMsgQId, (void*)&stMsg, sizeof(IpEpgMsg_t));
	}
	else
	{
		if(pstItem)
		{
			if(pstItem->ListOfEventURL)
			{
				HLIB_LIST_Foreach(pstItem->ListOfEventURL , IPEPG_MemFree);
				HLIB_LIST_RemoveAll(pstItem->ListOfEventURL);
				pstItem->ListOfEventURL = NULL;
			}
			HLIB_MEM_Free(pstItem);
		}
	}

	return ulCount;
}
#else
HUINT32	ipepg_ChLogoSendToDamaAll(IPEPG_TASK_MGR_t* pstTaskMgr, HxList_t* phSvcList, HxList_t* ListOfEventURL)
{
	HxList_t* TempListOfEventURL = ListOfEventURL;
	EPG_URL_INFO_t		* EventURLInfo = NULL;
	HUINT32			ulCount = 0;

	APK_META_QUERY_ResetIpLogoTable();
	while(TempListOfEventURL != NULL && pstTaskMgr != NULL && (pstTaskMgr->eStatus & eIPEPG_STOP) != eIPEPG_STOP && phSvcList != NULL)
	{
		EventURLInfo = (EPG_URL_INFO_t *)HLIB_LIST_Data(TempListOfEventURL);

		if(EventURLInfo != NULL)
		{
			ipepg_ChLogoSendToDama(phSvcList, EventURLInfo);
		}
		ulCount++;
		TempListOfEventURL = TempListOfEventURL->next;
	}

	return ulCount;
}
#endif
HUINT32 ipepg_DownloadEventListPart(
	IPEPG_TASK_MGR_t* pstTaskMgr,
	HxList_t* ListOfEventURL,
	HUINT32 ulSystemTime,
	HUINT32 ulStartTime,
	HUINT32 ulEndTime,
	HUINT32 ulIpepgTotalTime,
	HUINT32 ulSvcCount,
	HBOOL* 	pbDownloadFail,
	HUINT32			*pulSkipSvcCount
)
{
	HxList_t	*TempListOfEventURL = NULL;
	HUINT8		*pucMaccAddr = NULL;
	HUINT8		*pServerURL = NULL;
	HUINT32 	ulDownloadCount = 0;
	HUINT32 	ulPriorityCount = 0;
	HUINT32 	ulTotalStart = ulSystemTime+ulStartTime * 3600 + IPEPG_OFFSET_TIME;
	HUINT32 	ulTotalEnd = ulSystemTime+ulEndTime * 3600 + IPEPG_OFFSET_TIME;
 	HUINT32		ulStampTime 		= 0, ulCurStampTime = 0;
	HUINT32 	i = 0, ulSeperatePart = 1;
	HBOOL		bUseTimeStamp = FALSE;
	HUINT32 	ulTotalEventCount = 0;
	HUINT32 	ulEventCount = 0;
	EPG_URL_INFO_t	*EventURLInfo = NULL;
	HUINT8			pEventURL[IPEPG_URL_MAX_LEN] ={0,};
	DxIpService_t 	ipSvcInfo;
	EPG_URL_PARSER_t 	*UrlParser = NULL;
	HUINT8 			pDateValue[12] = {0,};
	HUINT32			ulSkipSvcCount = 0;
#if defined(IPEPG_PRIORITY_DOWN)
	HUINT32 	j = 0;
#endif

#if defined(CONFIG_DEBUG)
	HUINT32 	ulDebugMiddle1 = 0, ulDebugMiddle2 = 0;
#endif
	IPEPG_FUNC_START();

	IPEPG_RETURN_IF(ListOfEventURL == NULL, ERR_FAIL);
	IPEPG_RETURN_IF(pbDownloadFail == NULL, ERR_FAIL);
	IPEPG_RETURN_IF(pulSkipSvcCount == NULL, ERR_FAIL);

	pucMaccAddr = IPEPG_GetItemPoint(eIPEPG_ITEM_MAC);
	pServerURL = (HUINT8 *)IPEPG_GetItemPoint(eIPEPG_ITEM_SERVERURL);
	if(pucMaccAddr == NULL || pServerURL == NULL)
	{
		goto ENDPROC;
	}

	// Init stampTime
	ulStampTime =IPEPG_GetItemValue(eIPEPG_ITEM_STAMP_TIME);
	if(ulStampTime > 0)
	{
		ulCurStampTime = ulStampTime + ulIpepgTotalTime * 3600 + IPEPG_OFFSET_TIME;
	}


	if(ulStampTime && ulTotalStart < ulCurStampTime && ulCurStampTime <= ulTotalEnd)
	{
		HxLOG_Debug("ulSeperatePart is 2, ulStampTime[%d], ulCurStampTIme[%d], ulTotalStart[%d], ulTotalEnd[%d]\n",
			ulStampTime, ulCurStampTime, ulTotalStart, ulTotalEnd);
		ulSeperatePart = 2;
	}

	HxLOG_Print("ipepg_DownloadEventListPart stampTime[%d] Start[%d] End[%d] StampTime[%d] ulSeperatePart[%d]\n", ulStampTime, ulTotalStart , ulTotalEnd, ulCurStampTime, ulSeperatePart);

	for(i = 0; i< ulSeperatePart; i++)
	{
		HxLOG_Print("ulSeperatePart[%d], i[%d]\n", ulSeperatePart, i);
		if(ulSeperatePart == 2)
		{
			if(i == 0)
			{
				ulTotalStart = ulSystemTime+ulStartTime * 3600 + IPEPG_OFFSET_TIME;
				ulTotalEnd = ulCurStampTime;
				bUseTimeStamp = TRUE;
			}
			else
			{
				ulTotalStart = ulCurStampTime;
				ulTotalEnd = ulSystemTime+ulEndTime * 3600 + IPEPG_OFFSET_TIME;
				bUseTimeStamp = FALSE;
			}
		}
		else if(ulSeperatePart == 1)
		{
			if(ulTotalEnd < ulCurStampTime)
			{
				bUseTimeStamp = TRUE;
			}
		}

		ulDownloadCount = 0;
		TempListOfEventURL = ListOfEventURL;

#if defined(IPEPG_PRIORITY_DOWN)
		for(j = 0; j < ulSvcCount; j++)
		{
			if(TempListOfEventURL != NULL)
			{
				TempListOfEventURL = TempListOfEventURL->next;
			}
		}

		ulPriorityCount = 1;
#endif

		while(TempListOfEventURL != NULL)
		{
			HxLOG_Debug("[Count :%d] \n",ulDownloadCount);

			if(pstTaskMgr== NULL ||(pstTaskMgr != NULL && (pstTaskMgr->eStatus & eIPEPG_STOP) == eIPEPG_STOP))
			{
				break;
			}

			EventURLInfo = (EPG_URL_INFO_t *)HLIB_LIST_Data(TempListOfEventURL);
			if(EventURLInfo != NULL)
			{
				ipSvcInfo.usOnId = EventURLInfo->stTripleId.usOnId;
				ipSvcInfo.usTsId = EventURLInfo->stTripleId.usTsId;
				ipSvcInfo.usSvcId = EventURLInfo->stTripleId.usSvcId;
				ipSvcInfo.ulSvcUid = EventURLInfo->ulSvcUid;
				ipSvcInfo.ulMediaSupport = EventURLInfo->bSupportMedia;

				HxLOG_Debug("Svclist TripleId[%x][%x][%x][%d]\n",EventURLInfo->stTripleId.usOnId, EventURLInfo->stTripleId.usTsId, EventURLInfo->stTripleId.usSvcId,EventURLInfo->ulSvcUid);
				HxSTD_MemSet(pEventURL, 0, IPEPG_URL_MAX_LEN);
				HxSTD_snprintf(pEventURL, IPEPG_URL_MAX_LEN, "%s/%s%s", pServerURL, (HUINT8 *)IPEPG_EPGLIST_URL,pucMaccAddr);
				UrlParser = ipepg_UrlParse(pEventURL);

				if(UrlParser)
				{
#ifdef CONFIG_FUNC_EPG_USE_SVCUID
					HxSTD_MemSet(pDateValue, 0, 12);
					HxSTD_snprintf(pDateValue, 12, "%d",EventURLInfo->ulSvcUid);
					ipepg_MakeQuery(UrlParser, IPEPG_HSVC_URL, pDateValue);
#else
					HxSTD_MemSet(pDateValue, 0, 12);
					HxSTD_snprintf(pDateValue, 12, "%d",EventURLInfo->stTripleId.usTsId);
					ipepg_MakeQuery(UrlParser, IPEPG_TSID_URL, pDateValue);

					HxSTD_MemSet(pDateValue, 0, 12);
					HxSTD_snprintf(pDateValue, 12, "%d",EventURLInfo->stTripleId.usOnId);
					ipepg_MakeQuery(UrlParser, IPEPG_ONID_URL, pDateValue);

					HxSTD_MemSet(pDateValue, 0, 12);
					HxSTD_snprintf(pDateValue, 12, "%d",EventURLInfo->stTripleId.usSvcId);
					ipepg_MakeQuery(UrlParser, IPEPG_SVCID_URL, pDateValue);
#endif

					// Stamp Time
					if(bUseTimeStamp)
					{
						HxSTD_MemSet(pDateValue, 0, 12);
						HxSTD_snprintf(pDateValue, 12, "%d", ulStampTime);
						ipepg_MakeQuery(UrlParser, IPEPG_TIMESTAMP_URL, pDateValue);
					}

					// Start Time
					HxSTD_MemSet(pDateValue, 0, 12);
					HxSTD_snprintf(pDateValue, 12, "%d", ulTotalStart);
					ipepg_MakeQuery(UrlParser, IPEPG_STARTTIME_URL, pDateValue);

					// End Time
					HxSTD_MemSet(pDateValue, 0, 12);
					HxSTD_snprintf(pDateValue, 12, "%d", ulTotalEnd);
					ipepg_MakeQuery(UrlParser, IPEPG_ENDTIME_URL, pDateValue);

					ipepg_MakeURLWithParser(UrlParser, pEventURL);
					ipepg_FreeUrlParser(UrlParser);
				}

#if defined(CONFIG_DEBUG)
				VK_CLOCK_GetTime((unsigned long*)&ulDebugMiddle1);
#endif
				ulEventCount = ipepg_DownloadEventList(pEventURL, &ipSvcInfo, pbDownloadFail);
				ulTotalEventCount += ulEventCount;

				if(ulEventCount != 0)
				{
					ulDownloadCount++;
					EventURLInfo->ulEventCount += ulEventCount;
					HLIB_STD_TaskSleep(IPEPG_DELAYED_TIME);
#if defined(CONFIG_DEBUG)
					IPEPG_SetDownloadDebugInfo(eIPEPG_DNLD_DBG_DOWNLOAD_COUNT, (void *)ulDownloadCount);
					IPEPG_SetDownloadDebugInfo(eIPEPG_DNLD_DBG_DOWNLOADEVENT_COUNT, (void *)EventURLInfo->ulEventCount);
#endif
					HxLOG_Debug("Event Count is [%d]. URL[%s]\n",ulEventCount,pEventURL);
				}
				else
				{
					HxLOG_Debug("Event Count is zero. URL[%s]\n",pEventURL);
				}
#if defined(CONFIG_DEBUG)
				VK_CLOCK_GetTime((unsigned long*)&ulDebugMiddle2);
				HxLOG_Debug("Middle Time : %d\n", ulDebugMiddle2 - ulDebugMiddle1);
#endif
				// IPEPG Download 중 실패 한다면, 다운로드를 중지한다.
				if(*pbDownloadFail == TRUE)
				{
					HxLOG_Debug("Download Failed... \n");
					break;
				}
			}
			else
			{
				HxLOG_Trace();
			}
			TempListOfEventURL = TempListOfEventURL->next;

#if defined(IPEPG_PRIORITY_DOWN)
			HxLOG_Debug("Priority Count:[%d/%d]\n", ulPriorityCount, IPEPG_PRIORITY_INTERVAL);

			if(ulPriorityCount < IPEPG_PRIORITY_INTERVAL)
			{
				if(ulEventCount != 0)
				{
					ulPriorityCount++;
				}
				else
				{
#if defined(IPEPG_PRIORITY_LIMIT_SKIP_CNT)
					if(ulSkipSvcCount < IPEPG_PRIORITY_LIMIT_SKIP_CNT)
					{
						ulSkipSvcCount++;
						HxLOG_Debug("ulSkipSvcCount[%d] Find event from the next service and loop again.\n", ulSkipSvcCount);
					}
					else
					{
						HxLOG_Debug("ulSkipSvcCount[%d] > limit(%d). There no services. Ignore this loop.\n", ulSkipSvcCount, IPEPG_PRIORITY_LIMIT_SKIP_CNT);
						ulSkipSvcCount = 0;
						*pulSkipSvcCount = ulSkipSvcCount;
						break;
					}
#else
					ulSkipSvcCount++;
					HxLOG_Debug("ulSkipSvcCount[%d] Find event from the next service and loop again.\n", ulSkipSvcCount);
#endif
				}
			}
			/* Check whether the last loop is failed or not. */
			else if((ulEventCount == 0) && (ulPriorityCount == IPEPG_PRIORITY_INTERVAL))
			{
#if defined(IPEPG_PRIORITY_LIMIT_SKIP_CNT)
				if(ulSkipSvcCount < IPEPG_PRIORITY_LIMIT_SKIP_CNT)
				{
					ulSkipSvcCount++;
					HxLOG_Debug("ulSkipSvcCount[%d] Find event from the next service and loop again.\n", ulSkipSvcCount);
				}
				else
				{
					HxLOG_Debug("ulSkipSvcCount[%d] > limit(%d). There no services. Ignore this loop.\n", ulSkipSvcCount, IPEPG_PRIORITY_LIMIT_SKIP_CNT);
					ulSkipSvcCount = 0;
					*pulSkipSvcCount = ulSkipSvcCount;
					break;
				}
#else
				ulSkipSvcCount++;
				HxLOG_Debug("ulSkipSvcCount[%d] Find event from the next service and loop again.\n", ulSkipSvcCount);
#endif
			}
			else
			{
				*pulSkipSvcCount = ulSkipSvcCount;
				break;
			}
#else
				*pulSkipSvcCount = ulSkipSvcCount;
#endif
		}
	}

ENDPROC:
	IPEPG_MemFree(pServerURL);
	IPEPG_MemFree(pucMaccAddr);
	IPEPG_FUNC_END();

	return ulTotalEventCount;
}

static HERROR __ipepg_makeDownloadEventURL(
	EPG_URL_INFO_t 	* 	pEventURLInfo,
	HUINT8 			* 	pucCommand,
	HUINT8 			*	pucMacAddr,
	HUINT8 			*	pucServerURL,
	HUINT32 			ulTotalStart,
	HUINT32 			ulTotalEnd,
	HUINT8 			* 	pucTargetEventURL /*MUST USE IPEPG_URL_MAX_LEN*/
)
{
	EPG_URL_PARSER_t 	*UrlParser = NULL;
	HUINT8				pTempURL[IPEPG_URL_MAX_LEN] ={0,};
	HUINT8 				pDateValue[12] = {0,};


	if(NULL == pEventURLInfo || NULL == pucCommand || NULL == pucTargetEventURL || NULL == pucMacAddr || NULL == pucServerURL)
	{
		HxLOG_Debug("[%s] param fail. pEventURLInfo(%p) pucCommand(%p) pucTargetEventURL(%p) pucMacAddr(%p) pucServerURL(%p)\n",
			__FUNCTION__, pEventURLInfo, pucCommand, pucTargetEventURL, pucMacAddr,pucServerURL);
		return ERR_FAIL;
	}

	HxSTD_MemSet(pTempURL, 0, IPEPG_URL_MAX_LEN);
	HxSTD_snprintf(pTempURL, IPEPG_URL_MAX_LEN, "%s/%s%s", pucServerURL, (HUINT8 *)pucCommand, pucMacAddr);
	UrlParser = ipepg_UrlParse(pTempURL);
	if(NULL == UrlParser)
	{
		HxLOG_Debug("[%s] ipepg_urlParse failed. (%s)\n", __FUNCTION__, pTempURL);
		return ERR_FAIL;
	}

#ifdef CONFIG_FUNC_EPG_USE_SVCUID
	HxSTD_MemSet(pDateValue, 0, 12);
	HxSTD_snprintf(pDateValue, 12, "%d",pEventURLInfo->ulSvcUid);
	ipepg_MakeQuery(UrlParser, IPEPG_HSVC_URL, pDateValue);
#else
	// TODO: Triple로 하는 API가 서버에 존재하는지 확인 및 테스트 필요
	HxSTD_MemSet(pDateValue, 0, 12);
	HxSTD_snprintf(pDateValue, 12, "%d",pEventURLInfo->stTripleId.usTsId);
	ipepg_MakeQuery(UrlParser, IPEPG_TSID_URL, pDateValue);

	HxSTD_MemSet(pDateValue, 0, 12);
	HxSTD_snprintf(pDateValue, 12, "%d",pEventURLInfo->stTripleId.usOnId);
	ipepg_MakeQuery(UrlParser, IPEPG_ONID_URL, pDateValue);

	HxSTD_MemSet(pDateValue, 0, 12);
	HxSTD_snprintf(pDateValue, 12, "%d",pEventURLInfo->stTripleId.usSvcId);
	ipepg_MakeQuery(UrlParser, IPEPG_SVCID_URL, pDateValue);
#endif

	// Start Time
	HxSTD_MemSet(pDateValue, 0, 12);
	HxSTD_snprintf(pDateValue, 12, "%d", ulTotalStart);
	ipepg_MakeQuery(UrlParser, IPEPG_STARTTIME_URL, pDateValue);

	// End Time
	HxSTD_MemSet(pDateValue, 0, 12);
	HxSTD_snprintf(pDateValue, 12, "%d", ulTotalEnd);
	ipepg_MakeQuery(UrlParser, IPEPG_ENDTIME_URL, pDateValue);

	ipepg_MakeURLWithParser(UrlParser, pucTargetEventURL);
	ipepg_FreeUrlParser(UrlParser);

	return ERR_OK;
}

#if defined (IPEPG_BACKWARD_SUPPORT)
STATIC HUINT32 ipepg_DownloadEventListPartofBackward(
	IPEPG_TASK_MGR_t* pstTaskMgr,
	HxList_t* ListOfEventURL,
	HUINT32 ulSystemTime,
	HUINT32 ulStartTime,
	HUINT32 ulEndTime,
	HUINT32 ulIpepgTotalTime,
	HBOOL* 	pbDownloadFail	// Download 중 실패 한 경우를 확인.
)
{
	HxList_t	*TempListOfEventURL = NULL;
	HUINT32 	ulDownloadCount = 0;
	HUINT32 	ulTotalStart = ulSystemTime+ulStartTime * 3600 + IPEPG_OFFSET_TIME;
	HUINT32 	ulTotalEnd = ulSystemTime+ulEndTime * 3600 + IPEPG_OFFSET_TIME;
	HUINT32 	ulTotalEventCount = 0;
	HUINT8		*pucMacAddr = NULL, *pucServerURL = NULL;

#if defined(CONFIG_DEBUG)
	HUINT32 	ulDebugMiddle1 = 0, ulDebugMiddle2 = 0;
#endif

	IPEPG_FUNC_START();

	HxLOG_Print("[%s] Start[%d] End[%d] \n", __FUNCTION__, ulTotalStart , ulTotalEnd);

	pucMacAddr = IPEPG_GetItemPoint(eIPEPG_ITEM_MAC);
	pucServerURL = (HUINT8 *)IPEPG_GetItemPoint(eIPEPG_ITEM_SERVERURL);
	if(pucMacAddr == NULL || pucServerURL == NULL)
	{
		HxLOG_Debug("[%s] GetItemPoint is failed pucMacAddr(%p) pServerURL(%p)\n", __FUNCTION__, pucMacAddr, pucServerURL);
		goto ENDPROC;
	}

	ulDownloadCount = 0;
	for( TempListOfEventURL = ListOfEventURL; TempListOfEventURL != NULL; TempListOfEventURL = TempListOfEventURL->next)
	{
		HERROR 				err_ret = ERR_OK;
		HUINT8				pEventURL[IPEPG_URL_MAX_LEN] ={0,};
		DxIpService_t 		ipSvcInfo;
		HUINT32 			ulEventCount = 0;
		EPG_URL_INFO_t 		*EventURLInfo = (EPG_URL_INFO_t *)HLIB_LIST_Data(TempListOfEventURL);

		// Check to support media
		if(NULL == EventURLInfo || FALSE == EventURLInfo->bSupportMedia )
		{
			if(EventURLInfo && FALSE == EventURLInfo->bSupportMedia)
			{
				HxLOG_Debug("[%s] dont support media (%s)\n", __FUNCTION__, EventURLInfo->pucUrl);
			}
			HxLOG_Trace();
			continue;
		}

		HxLOG_Debug("Svclist TripleId[%x][%x][%x]\n",EventURLInfo->stTripleId.usOnId, EventURLInfo->stTripleId.usTsId, EventURLInfo->stTripleId.usSvcId);

		HxSTD_memset(pEventURL, 0, IPEPG_URL_MAX_LEN);
		err_ret = __ipepg_makeDownloadEventURL(
			EventURLInfo, (HUINT8 *)IPEPG_EPGMEDIALIST_URL, pucMacAddr,pucServerURL, ulTotalStart, ulTotalEnd,
			pEventURL //MUST USE IPEPG_URL_MAX_LEN
			);
		if(err_ret == ERR_FAIL)
		{
			HxLOG_Debug("[%s] make downloadEventURL failed \n" , __FUNCTION__ );
			continue;
		}

		HxSTD_memset((void*)&ipSvcInfo,0,sizeof(DxIpService_t));
		ipSvcInfo.usOnId = EventURLInfo->stTripleId.usOnId;
		ipSvcInfo.usTsId = EventURLInfo->stTripleId.usTsId;
		ipSvcInfo.usSvcId = EventURLInfo->stTripleId.usSvcId;
		ipSvcInfo.ulSvcUid = EventURLInfo->ulSvcUid;
		ipSvcInfo.ulMediaSupport = EventURLInfo->bSupportMedia;

#if defined(CONFIG_DEBUG)
		VK_CLOCK_GetTime((unsigned long*)&ulDebugMiddle1);
#endif
		// -------------------------------------------------------------
		ulEventCount = ipepg_DownloadEventList(pEventURL, &ipSvcInfo, pbDownloadFail);
		// -------------------------------------------------------------

		ulTotalEventCount += ulEventCount;

		if(ulEventCount != 0)
		{
			ulDownloadCount++;
			EventURLInfo->ulEventCount += ulEventCount;
			HLIB_STD_TaskSleep(IPEPG_DELAYED_TIME);

#if defined(CONFIG_DEBUG)
			IPEPG_SetDownloadDebugInfo(eIPEPG_DNLD_DBG_DOWNLOAD_COUNT, (void *)ulDownloadCount);
			IPEPG_SetDownloadDebugInfo(eIPEPG_DNLD_DBG_DOWNLOADEVENT_COUNT, (void *)EventURLInfo->ulEventCount);
#endif
			HxLOG_Debug("[%s] Event Count is [%d]. URL[%s]\n", __FUNCTION__, ulEventCount,pEventURL);
		}
		else
		{
			HxLOG_Debug("[%s] Event Count is zero. URL[%s]\n",__FUNCTION__, pEventURL);
		}

		if(pstTaskMgr== NULL ||(pstTaskMgr != NULL && (pstTaskMgr->eStatus & eIPEPG_STOP) == eIPEPG_STOP))
		{
			break;
		}

		// IPEPG Download 중 실패 한다면, 다운로드를 중지한다.
		if(*pbDownloadFail == TRUE)
		{
			HxLOG_Debug("Download Failed... \n");
			break;
		}

#if defined(CONFIG_DEBUG)
		VK_CLOCK_GetTime((unsigned long*)&ulDebugMiddle2);
		HxLOG_Debug("Middle Time : %d\n", ulDebugMiddle2 - ulDebugMiddle1);
#endif
	}

ENDPROC:
	IPEPG_MemFree(pucServerURL);
	IPEPG_MemFree(pucMacAddr);

	IPEPG_FUNC_END();
	return ulTotalEventCount;
}
#endif
HUINT32 ipepg_DownloadEventListAll(IPEPG_TASK_MGR_t* pstTaskMgr, HxList_t* phSvcList, HxList_t* ListOfEventURL, HBOOL *pbDownloadFail)
{
	static HUINT32	ulDownloadCount = 0;
	HUINT32			ulStartTime = 0, ulEndTime = 0, ulStampTime = 0;
	UNIXTIME		ulSystemTime = 0;
	HUINT32			ulPartOfTime = 0;
	HBOOL			bGetBackward = FALSE;
	HBOOL			bSuccessfullyEnd = TRUE;
	HUINT32			ulSvcCount = 0;
	HxList_t		*pSortedEventUrls = NULL;
	UNIXTIME		ulTempEndTime = 0;
	HUINT32			ulSvcSkipCount = 0;
#if defined (IPEPG_PRIORITY_DOWN)
	HUINT32			ulTotalSkipSvcCount = 0;
	HUINT32			ulTempSkipSvcCount = 0;
	HxList_t*		ListOfPriority = NULL;

	HERROR			hError = ERR_OK;
	HUINT32			ulEventURLLength = 0;
	HUINT32			priorityLoopCount = 0;
	HUINT32			ulSvcDownloadCount = 0;
	HxList_t		*pEventUrl;
	HxList_t		*NewListOfEventURL = NULL;
#else
	HUINT32 		ulTotalCount = 0;
#endif

	IPEPG_FUNC_START();

	// 과거 EPG를 받아올지 여부
#if defined (IPEPG_BACKWARD_SUPPORT)
	bGetBackward = TRUE;
#endif

	if(VK_CLOCK_GetTime((unsigned long*)&ulSystemTime) != ERR_OK)
	{
		return ERR_FAIL;
	}
#if defined(IPEPG_TEST_DIFFERENT_TIME)	// 현재시간 변경 test 진행.
	{
		HxDATETIME_t dateTime;
		HLIB_DATETIME_ConvertUnixTimeToDateTime(ulSystemTime, &dateTime);
		dateTime.stDate.usYear = CUR_YEAR;		dateTime.stDate.ucMonth = CUR_MONTH;		dateTime.stDate.ucDay = CUR_DAY;
		HLIB_DATETIME_ConvertDateTimeToUnixTime (&dateTime, &ulSystemTime);
	}
#endif

#if 0	// For Test
	ulPartOfTime = IPEPG_TOTAL_TIME;
#else
	if(APK_POWER_IsStandbyMode() == TRUE)
		ulPartOfTime = IPEPG_TOTAL_TIME;
	else
		ulPartOfTime = IPEPG_PART_OF_TIME;
#endif

#if defined(CONFIG_DEBUG)
	IPEPG_SetDownloadDebugInfo(eIPEPG_DNLD_DBG_PARTOFTIME, (void *)ulPartOfTime);
	IPEPG_SetDownloadDebugInfo(eIPEPG_DNLD_DBG_TOTALTIME, (void *)(IPEPG_TOTAL_TIME));
#endif

#if defined(IPEPG_TEST_DIFFERENT_TIME)
	VK_CLOCK_GetTime((unsigned long*)&ulTempEndTime);
	ulTempEndTime +=IPEPG_OFFSET_TIME;
#else
	ulTempEndTime = ulSystemTime + IPEPG_OFFSET_TIME;
#endif

#if defined (IPEPG_BACKWARD_SUPPORT)
	// Backward 이면, 2주전 보다 오래된 것에 대해서만 지운다.
	if(TRUE == bGetBackward)
	{
		ulTempEndTime -= IPEPG_BACKWARD_TOTAL_TIME*60*60;
	}
#endif
	APK_META_QUERY_DeletIpEventByEndTime(ulTempEndTime);

	s_overflow = 0;

	HLIB_STD_TaskSleep(IPEPG_DELAYED_TIME);
	ulStampTime = IPEPG_GetItemValue(eIPEPG_ITEM_STAMP_TIME);

#if defined (IPEPG_DEBUG_DOWNLOAD_XML)
	IPEPG_DEBUG_SaveXML_Init(TRUE, ulSystemTime, TRUE);
#endif

	IPEPG_SetItem(eIPEPG_ITEM_DOWNLOADPROGRESS, (void *)1);
#if !defined(IPEPG_PRIORITY_DOWN)
	ulTotalCount = IPEPG_TOTAL_TIME/ulPartOfTime;
#endif

	pSortedEventUrls = ListOfEventURL;
#if defined(IPEPG_PRIORITY_DOWN)
	hError = ipepg_LoadXML(&ListOfPriority);
	if(hError == ERR_OK)
	{
		// make local copy
		for (pEventUrl = HLIB_LIST_Last(ListOfEventURL); pEventUrl != NULL; pEventUrl = HLIB_LIST_Prev(pEventUrl))
		{
			EPG_URL_INFO_t *pEventUrlInfo = NULL;
			HCHAR *clone = NULL;

			pEventUrlInfo = (EPG_URL_INFO_t *)HLIB_LIST_Data(pEventUrl);
			if (pEventUrlInfo != NULL)
			{
				clone = HLIB_STD_MemDup(pEventUrlInfo, sizeof(EPG_URL_INFO_t));
				NewListOfEventURL = HLIB_LIST_Prepend(NewListOfEventURL, clone);
			}
		}

		// sort local copy
#ifdef CONFIG_FUNC_EPG_USE_SVCUID
		pSortedEventUrls = ipepg_RearrangementListOfEventURL(ListOfPriority, NewListOfEventURL, phSvcList);
#else
		pSortedEventUrls = ipepg_RearrangementListOfEventURL(ListOfPriority, NewListOfEventURL);
#endif
	}

	if(ListOfPriority != NULL)
	{
		HLIB_LIST_Foreach(ListOfPriority , IPEPG_MemFree);
		HLIB_LIST_RemoveAll(ListOfPriority);
		ListOfPriority = NULL;
	}

	ulEventURLLength = HLIB_LIST_Length(pSortedEventUrls);
	priorityLoopCount = ulEventURLLength / IPEPG_PRIORITY_INTERVAL;
	if(ulEventURLLength % IPEPG_PRIORITY_INTERVAL > 0)
	{
		priorityLoopCount += 1;
	}

#if defined(IPEPG_PRIORITY_LIMIT_CNT)
	// limitation count
	if(priorityLoopCount > IPEPG_PRIORITY_LIMIT_CNT/IPEPG_PRIORITY_INTERVAL)
	{
		priorityLoopCount = IPEPG_PRIORITY_LIMIT_CNT/IPEPG_PRIORITY_INTERVAL;
	}
#endif

	ulSvcDownloadCount = IPEPG_GetItemValue(eIPEPG_ITEM_SVC_COUNT);
	HxLOG_Debug("Service Count[%d], priorityLoopCount[%d], ulTotalSkipSvcCount[%d]\n", ulSvcDownloadCount, priorityLoopCount, ulTotalSkipSvcCount);

	for(ulSvcCount = 0; ulSvcCount < priorityLoopCount; ulSvcCount++)
	{
		ulSvcDownloadCount = (ulSvcCount*IPEPG_PRIORITY_INTERVAL);
		HxLOG_Debug(HxANSI_COLOR_YELLOW("+++EventList download start, svc download counts:[%d/%d], time:[%d]+++\n"), ulSvcDownloadCount, IPEPG_PRIORITY_LIMIT_CNT, ulSystemTime);

		for(ulStartTime = 0, ulEndTime = ulPartOfTime; ulEndTime <= IPEPG_TOTAL_TIME; ulStartTime += ulPartOfTime, ulEndTime += ulPartOfTime )
		{
			ulDownloadCount += ipepg_DownloadEventListPart(pstTaskMgr, pSortedEventUrls, ulSystemTime, ulStartTime, ulEndTime, IPEPG_TOTAL_TIME, (ulSvcDownloadCount+ulTotalSkipSvcCount), pbDownloadFail, &ulSvcSkipCount);

			HLIB_STD_TaskSleep(IPEPG_DELAYED_TIME);
			if(*pbDownloadFail == TRUE)
			{
				HxLOG_Debug("[Forward] Ipepg Download Failed...\n");
				break;
			}

			if(pstTaskMgr == NULL || (pstTaskMgr != NULL && (pstTaskMgr->eStatus & eIPEPG_STOP) == eIPEPG_STOP) || *pbDownloadFail == TRUE )
			{
				bGetBackward = FALSE;
				bSuccessfullyEnd = FALSE;
				break;
			}

			if(ulStartTime == 0)
			{
				ulTempSkipSvcCount = ulSvcSkipCount;
				/* In terms of the end time it can be different among every services. Just check the first time's skip count to avoid an unnecessary skip count. */
				HxLOG_Debug("ulDownloadCount[%d], ulTempSkipSvcCount[%d]\n", ulDownloadCount, ulTempSkipSvcCount);
			}
		}

		ulTotalSkipSvcCount += ulTempSkipSvcCount;
		HxLOG_Debug(HxANSI_COLOR_PURPLE("The total skip counts of service [%d]\n"), ulTotalSkipSvcCount);

		if(	pstTaskMgr == NULL || (pstTaskMgr != NULL && (pstTaskMgr->eStatus & eIPEPG_STOP) == eIPEPG_STOP) || *pbDownloadFail == TRUE )
		{
			bGetBackward = FALSE;
			bSuccessfullyEnd = FALSE;
			break;
		}

		if(s_overflow == 1)
		{
			bSuccessfullyEnd = TRUE;
			bGetBackward = FALSE;
			HxLOG_Warning("EPG event count or DB size is overflowd. Finish downloading IP EPG.\n");
			break;
		}

		HxLOG_Debug(HxANSI_COLOR_YELLOW("---EventList download end, svc download counts:[%d/%d], time:[%d]---\n"), ulSvcDownloadCount+IPEPG_PRIORITY_INTERVAL, IPEPG_PRIORITY_LIMIT_CNT, ulSystemTime);
		ipepg_SetDownloadProgress(bGetBackward, TRUE, ulSvcDownloadCount+IPEPG_PRIORITY_INTERVAL, IPEPG_PRIORITY_LIMIT_CNT);
	}
	HxLOG_Debug(HxANSI_COLOR_YELLOW("Download Finish : Total Services[%d], ulDownloadCount[%d], sysemTime[%d], stampTime[%d]\n"), ulSvcDownloadCount+IPEPG_PRIORITY_INTERVAL, ulDownloadCount, ulSystemTime, ulStampTime);
#else
	HxLOG_Debug("\n\n\n[%s] EventList Download [%d] +++ \n",__FUNCTION__ , ulSystemTime);
	for(ulStartTime = 0, ulEndTime = ulPartOfTime; ulEndTime <= IPEPG_TOTAL_TIME; ulStartTime += ulPartOfTime, ulEndTime += ulPartOfTime )
	{
		HxLOG_Debug("\n\n\n[%s] EventList Download [%d/%d] [%d] +++ \n",__FUNCTION__ , ulStartTime/ulPartOfTime, ulTotalCount , ulSystemTime);
		ipepg_SetDownloadProgress(bGetBackward, TRUE, ulStartTime/ulPartOfTime, ulTotalCount);

		ulDownloadCount += ipepg_DownloadEventListPart(
			pstTaskMgr, pSortedEventUrls, ulSystemTime, ulStartTime, ulEndTime, IPEPG_TOTAL_TIME, ulSvcCount, pbDownloadFail, &ulSvcSkipCount);

		HLIB_STD_TaskSleep(IPEPG_DELAYED_TIME);
		if(
			pstTaskMgr== NULL || (pstTaskMgr != NULL && (pstTaskMgr->eStatus & eIPEPG_STOP) == eIPEPG_STOP) ||
			*pbDownloadFail == TRUE	)
		{
			bGetBackward = FALSE;
			bSuccessfullyEnd = FALSE;
			break;
		}
	}
	HxLOG_Debug("\n\n\n[%s] EventList Download [%d] stampTime[%d] --- \n",__FUNCTION__ , ulSystemTime, ulStampTime);
#endif

#if defined (IPEPG_BACKWARD_SUPPORT)
	if(TRUE == bGetBackward)
	{
		HBOOL bFirstDownload = FALSE;

		// stampTime 값이 없으면, 처음부터 다운로드를 수행한다.
		if( 0 == ulStampTime)
		{
			bFirstDownload = TRUE;
		}

		ulSystemTime -= IPEPG_BACKWARD_TOTAL_TIME*60*60;
#if defined (IPEPG_DEBUG_DOWNLOAD_XML)
		IPEPG_DEBUG_SaveXML_Init(FALSE, ulSystemTime,TRUE);
#endif
		HxLOG_Debug("\n\n\n[%s] Start Backward [%d] +++ \n",__FUNCTION__ , ulSystemTime);
		ulTotalCount = IPEPG_BACKWARD_TOTAL_TIME/ulPartOfTime;
		ulEndTime = IPEPG_BACKWARD_TOTAL_TIME;
		ulStartTime = IPEPG_BACKWARD_TOTAL_TIME - ulPartOfTime;
		for( ; (HINT32)ulStartTime >=0  ; 	ulEndTime -= ulPartOfTime, ulStartTime -= ulPartOfTime )
		{
			HxLOG_Debug("\n\n\n[%s] Start Backward [%d/%d] [%d][%d] \n",__FUNCTION__ ,ulEndTime/ulPartOfTime, ulTotalCount, ulStartTime,ulEndTime);
			ipepg_SetDownloadProgress(bGetBackward, FALSE, ulTotalCount-ulEndTime/ulPartOfTime, ulTotalCount);

			if(TRUE == bFirstDownload)
			{
				// 첫번째로 다운로드시, 전체 데이터를 Forward와 동일하게 받는다.
				HxLOG_Trace();
				ulDownloadCount += ipepg_DownloadEventListPart(
					pstTaskMgr, pSortedEventUrls, ulSystemTime, ulStartTime, ulEndTime, IPEPG_BACKWARD_TOTAL_TIME, ulSvcCount, pbDownloadFail, &ulSvcSkipCount);
			}
			else
			{
				// 첫번째 이후로 다운로드시, mediaData만 Update 한다.
				HxLOG_Trace();
				ulDownloadCount += ipepg_DownloadEventListPartofBackward(
					pstTaskMgr, pSortedEventUrls, ulSystemTime, ulStartTime, ulEndTime, IPEPG_BACKWARD_TOTAL_TIME, pbDownloadFail);
			}
			HLIB_STD_TaskSleep(IPEPG_DELAYED_TIME);
			HxLOG_Trace();
			if(
				pstTaskMgr == NULL || (pstTaskMgr != NULL && (pstTaskMgr->eStatus & eIPEPG_STOP) == eIPEPG_STOP) ||
				*pbDownloadFail == TRUE
			  )
			{
				bSuccessfullyEnd = FALSE;
				break;
			}
		}
		HxLOG_Debug("[%s] Start Backward [%d] --- \n\n\n",__FUNCTION__ , ulSystemTime);

		// stamp time으로 저장할 SystemTime을 원래 값으로 복원
		ulSystemTime += IPEPG_BACKWARD_TOTAL_TIME*60*60;
	}
#endif

#if defined(IPEPG_PRIORITY_DOWN)
	// delete local copy
	if (pSortedEventUrls != NULL
		&& pSortedEventUrls != ListOfEventURL) // if made local copy
	{
		HLIB_LIST_Foreach(pSortedEventUrls , IPEPG_MemFree);
		HLIB_LIST_RemoveAll(pSortedEventUrls);
		pSortedEventUrls = NULL;
	}
#endif

	if(TRUE == bSuccessfullyEnd)
	{
		IPEPG_SetItem(eIPEPG_ITEM_STAMP_TIME,	(void *)ulSystemTime);
		IPEPG_SetItem(eIPEPG_ITEM_DOWNLOADPROGRESS, (void *)100);
#if defined (IPEPG_PRIORITY_DOWN)
		IPEPG_SetItem(eIPEPG_ITEM_SVC_COUNT, (void *)ulSvcDownloadCount+IPEPG_PRIORITY_INTERVAL);
		HxLOG_Debug("save service count[%d]\n", ulSvcDownloadCount+IPEPG_PRIORITY_INTERVAL);
#endif
		HxLOG_Debug("sucessfully end\n");
	}
	else
	{
		// 다운로드 실패인 경우, UI상 DOWNLOAD 중인 것 처럼 보이게 하기 위함. 어차피 다시 시작할 것이므로.
		if (*pbDownloadFail != TRUE)
		{
			IPEPG_SetItem(eIPEPG_ITEM_DOWNLOADPROGRESS, (void *)0);
		}
	}
	APK_META_QUERY_SyncIpEventTable();
	IPEPG_FUNC_END();

	return ulDownloadCount;
}

static void ipepg_onOverFlow(const HCHAR *name, DAxSTATUS_e status, void *userdata, ...)
{
	s_overflow = 1;
}

void	IPEPG_PrintSvcInfo(void * data)
{
	EPG_URL_INFO_t	* pUrlInfo = (EPG_URL_INFO_t *)data;

	if(pUrlInfo)
	{
		HxLOG_Print("URL INFO[%x][%x][%x] Logo[%s]\n", pUrlInfo->stTripleId.usOnId, pUrlInfo->stTripleId.usTsId, pUrlInfo->stTripleId.usSvcId, pUrlInfo->pucThumbnailUrl);
	}
	else
	{
		HxLOG_Print("URL INFO is NULL\n");
	}

}

void	IPEPG_DownloadData(void *arg)
{
	IPEPG_DATAMGR_t 	* pstData = NULL;
	HxList_t			* ListOfEventURL = NULL;

	HUINT32 			MsgQId = 0;
	HERROR				ret = ERR_OK;
	IPEPG_TASK_MGR_t 	* pstTaskMgr = (IPEPG_TASK_MGR_t *)arg;
	HxList_t			*phSvcList = NULL;
	HxHASH_t		*phSvcHash = NULL;
#ifndef CONFIG_USE_CHANNEL_LOGO_TASK
	HUINT32 			ulCount2 = 0;
#endif
	HUINT32				ulDownloadCount = 0;
	HBOOL				bDownloadFail = FALSE;
#if defined(CONFIG_DEBUG)
	HUINT32				ulDebugStart = 0, ulDebugEnd = 0;
#endif

	IPEPG_FUNC_START();

#if defined(CONFIG_DEBUG)
	IPEPG_SetDownloadDebugInfo(eIPEPG_DNLD_DBG_STARTED, (void *)TRUE);
	IPEPG_SetDownloadDebugInfo(eIPEPG_DNLD_DBG_STARTTIME,  (void *)NULL);
#endif

	DAPI_AddStatusMonitor(DxNAMEOF(eDxEPG_TYPE_IP), ipepg_onOverFlow, NULL);

	if(pstTaskMgr == NULL || pstTaskMgr->pstData == NULL || (pstTaskMgr->eStatus & eIPEPG_STOP) == eIPEPG_STOP)
	{
		#if defined(CONFIG_DEBUG)
		IPEPG_SetDownloadDebugInfo(eIPEPG_DNLD_DBG_ENDED, (void *)TRUE);
		IPEPG_SetDownloadDebugInfo(eIPEPG_DNLD_DBG_ENDTIME,  (void *)NULL);
		#endif
		IPEPG_FUNC_END();
		return;
	}

	if(ret == ERR_OK)
	{
		pstData = pstTaskMgr->pstData;

		// Get Service list from RP Server
		ret = ipepg_DownloadSvcList(pstData, &ListOfEventURL);
		if( ERR_OK != ret ||  NULL == ListOfEventURL )
		{
			bDownloadFail = TRUE;
			HxLOG_Debug("[%s] ipepg_DownloadSvcList failed. ListOfEventURL(%p)\n",  __FUNCTION__, ListOfEventURL);
		}
		else
		{
			phSvcHash = IPEPG_GetItemPoint(eIPEPG_ITEM_SVCLIST);
			phSvcList = HLIB_HASH_GetValues (phSvcHash);
#if defined(CONFIG_DEBUG)
			HxLOG_Debug("IPEPG_DownloadData chlist Count[%d]\n",  HLIB_LIST_Length(phSvcList));
			HxLOG_Debug("IPEPG_DownloadData EventUrl Count[%d]\n",  HLIB_LIST_Length(ListOfEventURL));
			IPEPG_SetDownloadDebugInfo(eIPEPG_DNLD_DBG_SERVICE_COUNT, (void *)HLIB_LIST_Length(phSvcList) );
			HLIB_LIST_Foreach(ListOfEventURL,IPEPG_PrintSvcInfo);
#endif
#ifdef CONFIG_USE_CHANNEL_LOGO_TASK
			(void)ipepg_ChLogoSendToDamaAll(pstTaskMgr, ListOfEventURL);
#else
			ulCount2 = ipepg_ChLogoSendToDamaAll(pstTaskMgr, phSvcList, ListOfEventURL);
#endif

#if defined(CONFIG_DEBUG)
#ifndef CONFIG_USE_CHANNEL_LOGO_TASK
			IPEPG_SetDownloadDebugInfo(eIPEPG_DNLD_DBG_CHLOGSENDTODAMA_COUNT, (void *)ulCount2 );
#endif
			VK_CLOCK_GetTime((unsigned long*)&ulDebugStart);
#endif
			ipepg_SetIpSvcCount(ListOfEventURL);
			ulDownloadCount = ipepg_DownloadEventListAll(pstTaskMgr, phSvcList, ListOfEventURL, &bDownloadFail);
			ipepg_SetIpEvtCount(ulDownloadCount);
#if defined(CONFIG_DEBUG)
			VK_CLOCK_GetTime((unsigned long*)&ulDebugEnd);
			HxLOG_Debug("TOTAL Time : %d\n", ulDebugEnd - ulDebugStart);
			IPEPG_SetDownloadDebugInfo(eIPEPG_DNLD_DBG_DOWNLOADEVENT_TIME, (void *)(ulDebugEnd - ulDebugStart));
#endif
			HxLOG_Debug("Download Service Count[%d]\n", ulDownloadCount);

			if(phSvcList != NULL)
			{
				HLIB_LIST_RemoveAll(phSvcList);
			}
			if(phSvcHash != NULL)
			{
				HxLOG_Debug("eDxEPG_TYPE_IP Set Sync\n");
				HLIB_HASH_Destroy(phSvcHash);
			}
			if(ListOfEventURL != NULL)
			{
				HLIB_LIST_Foreach(ListOfEventURL , IPEPG_MemFree);
				HLIB_LIST_RemoveAll(ListOfEventURL);
				ListOfEventURL = NULL;
			}
		}

		if (bDownloadFail == TRUE)
		{
		}
	}

#if defined(CONFIG_DEBUG)
	IPEPG_SetDownloadDebugInfo(eIPEPG_DNLD_DBG_ENDED, (void *)TRUE);
	IPEPG_SetDownloadDebugInfo(eIPEPG_DNLD_DBG_ENDTIME, (void *)NULL);
#endif

	IPEPG_FUNC_END();

	if(IPEPG_GetMsgQId(&MsgQId) != ERR_FAIL)
	{
		IpEpgMsg_t	stMsg;

		stMsg.ulType   = 0;
		stMsg.ulMsg    = eIPEPG_MSG_DONE_DOWNLOAD;
		stMsg.ulParam1 = eIPEPG_DOWN_START;

		if(ret == ERR_FAIL)			stMsg.ulParam2 = (HUINT32)ERR_FAIL;
		else						stMsg.ulParam2 = (HUINT32)ERR_OK;


		if(bDownloadFail == TRUE)	stMsg.ulParam3 = (HUINT32)ERR_FAIL;
		else 						stMsg.ulParam3 = (HUINT32)ERR_OK;


		if(bDownloadFail == TRUE)
		{
			HxLOG_Debug("Ipepg Download Failed.....Set ulParam3 :%d\n", stMsg.ulParam3);
		}


		VK_MSG_Send(MsgQId, (void*)&stMsg, sizeof(IpEpgMsg_t));
	}


	return;
}

void IPEPG_DownloadDataTask (void * pstDownMgr)
{
	HUINT32 			ulDownloadEpgMsgQId;
	IpEpgMsg_t			stMsg;
	IPEPG_TASK_MGR_t * pstTaskMgr = NULL;

	HxLOG_Trace();

	pstTaskMgr = (IPEPG_TASK_MGR_t *)pstDownMgr;
	if(pstTaskMgr == NULL || pstTaskMgr->ulEpgDownloadTaskQId == 0)
	{
		return;
	}
	ulDownloadEpgMsgQId = pstTaskMgr->ulEpgDownloadTaskQId;

	while(1)
	{
		if (VK_MSG_ReceiveTimeout(ulDownloadEpgMsgQId, (void*)&stMsg, sizeof(IpEpgMsg_t), 300) == VK_OK)
		{
			switch (stMsg.ulMsg)
			{
				case eIPEPG_MSG_START_DOWNLOAD:
					IPEPG_DownloadData(pstTaskMgr);
					break;
				default :
					HxLOG_Error("IPEPG_DownloadDataTask reveived OtherMessage[%d]\n", stMsg.ulMsg);
					break;
			}
		}
	}
	return;
}

void IPEPG_ChLogoCreateTask(IPEPG_TASK_MGR_t* pstTaskMgr)
{
	ipepg_ChLogoCreateTask(pstTaskMgr);
}


/*********************** End of File ******************************/

