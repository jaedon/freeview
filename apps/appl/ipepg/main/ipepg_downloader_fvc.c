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
#include <hapi.h>
#include "../include/ipepg.h"
#include "ipepg_manager.h"
#include "ipepg_downloader.h"
#include "ipepg_fvc.h"
#include "ipepg_parser.h"

static HUINT8		*s_epg_buffer = NULL;
static HINT32		s_epgdata_size = 0;
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

#define	FVC_SERVER_ROOTCA_FILE		"/etc/pki/tls/GlobalSign_Root_CA.pem"
#define	FVC_CLIENT_CA_FILE			"/usr/browser/opera_dir/certs/client/hdr4000t_fvc_mds_20141204.pem"
#define	FVC_CLIENT_CAKEY_FILE		"/usr/browser/opera_dir/certs/client/hdr4000t_fvc_mds_20141204.key.pem"

extern const HCHAR*	THAPI_GetThumbnailURL(const HCHAR *pszURL, HUINT32 ulTime, const HCHAR *pszMountPath, HBOOL bRetExpectURL);

#define _____________________Parse_Module

static HERROR	ipepg_ParseHeader(HUINT8 *pRawBuffer, DxFvcExpiry_t *pstFvcExpiry)
{
	return IPEPG_FvcParseHeader(pRawBuffer, pstFvcExpiry);
}


static HERROR	ipepg_ParseServiceList(HUINT8 *pRawBuffer, HUINT32 ulBufferSize, HxList_t **ListOfEventURL)
{
	return IPEPG_FvcParseServiceList(pRawBuffer, ulBufferSize, ListOfEventURL);
}

static HERROR	ipepg_ParseEventList(HUINT8 *pRawBuffer, HUINT32 ulBufferSize, HxList_t **ListOfEventURL,HINT32 ulSvcUid)
{
	return IPEPG_FvcParseEventList(pRawBuffer, ulBufferSize, ListOfEventURL, ulSvcUid);
}

#define _____________________DownLoad_Module
static HERROR	ipepg_SettingHttpGetHeaderAndBody(const HINT8 * szUrl, HINT8 **ppcHeader,HINT8 **ppcBody, HFLOAT64 *pfDownloadSize, DxFvcExpiry_t	*pstFvcExpiry)
{
	HERROR							hError = ERR_FAIL;
	HERROR							hResult = ERR_FAIL;
	HxTRANSPORT_Http_t			*pHttp = NULL;
	HxTRANSPORT_Httpheader_t		*pHttpHeader = NULL;
	HCHAR						szHeaderBuffer[128] = {0,};
	HUINT8						*pAuthHash = NULL;
	HUINT8						*pAuthTimeStamp = NULL;

    HxLOG_Trace();
	IPEPG_GOTO_IF(szUrl == NULL, EXIT_FUNC);


	pHttp = HLIB_TRANSPORT_HttpOpen();
	IPEPG_GOTO_IF(pHttp == NULL, EXIT_FUNC);

	pHttpHeader = HLIB_TRANSPORT_HttpAppendHeader(pHttpHeader, "Content-Type: text/xml");
	IPEPG_GOTO_IF(pHttpHeader == NULL, EXIT_FUNC);

#if defined(IPEPG_USE_COMPRESS)
	pHttpHeader = HLIB_TRANSPORT_HttpAppendHeader(pHttpHeader, "Accept-Encoding : GZIP");
	IPEPG_GOTO_IF(pHttpHeader == NULL, EXIT_FUNC);
#endif

	pHttpHeader = HLIB_TRANSPORT_HttpAppendHeader(pHttpHeader, "Accept: application/vnd.fvc.v1+xml");
	IPEPG_GOTO_IF(pHttpHeader == NULL, EXIT_FUNC);

	if ( !HxSTD_StrEmpty(pstFvcExpiry->lastModifiedTime))
	{
		HxSTD_snprintf(szHeaderBuffer,64,"%s: %s",IPEPG_FVC_HEADER_IF_MODIFIED_SINCE, pstFvcExpiry->lastModifiedTime);
		pHttpHeader = HLIB_TRANSPORT_HttpAppendHeader(pHttpHeader, szHeaderBuffer);
		IPEPG_GOTO_IF(pHttpHeader == NULL, EXIT_FUNC);
	}

	pAuthHash = IPEPG_GetItemPoint(eIPEPG_ITEM_FVC_AUTH_HASH);
	if (pAuthHash != NULL)
	{
		HxSTD_snprintf(szHeaderBuffer,128,"%s: %s",IPEPG_FVC_HEADER_AUTH_HASH, pAuthHash);
		pHttpHeader = HLIB_TRANSPORT_HttpAppendHeader(pHttpHeader, szHeaderBuffer);
		IPEPG_GOTO_IF(pHttpHeader == NULL, EXIT_FUNC);
	}

	pAuthTimeStamp = IPEPG_GetItemPoint(eIPEPG_ITEM_FVC_AUTH_TIME_STAMP);
	if (pAuthTimeStamp != NULL)
	{
		HxSTD_snprintf(szHeaderBuffer,128,"%s: %s",IPEPG_FVC_HEADER_AUTH_TIMESTAMP, pAuthTimeStamp);
		pHttpHeader = HLIB_TRANSPORT_HttpAppendHeader(pHttpHeader, szHeaderBuffer);
		IPEPG_GOTO_IF(pHttpHeader == NULL, EXIT_FUNC);
	}

	hError = HLIB_TRANSPORT_HttpSetHeaderToTP(pHttp, pHttpHeader);
	IPEPG_GOTO_IF(hError != ERR_OK, EXIT_FUNC);

    if (IPEPG_HTTP_TIMEOUT != 0)
    {
		hError = HLIB_TRANSPORT_HttpSetTimeout(pHttp, IPEPG_HTTP_TIMEOUT);
        IPEPG_GOTO_IF(hError != ERR_OK, EXIT_FUNC);
    }

	hError = HLIB_TRANSPORT_HttpGetHeaderAndBody(pHttp, szUrl, ppcHeader , IPEPG_HEADER_MAX_SIZE , ppcBody , IPEPG_EVENT_DATA_MAX_SIZE , pfDownloadSize);
	IPEPG_GOTO_IF(hError != ERR_OK, EXIT_FUNC);

	hResult = ERR_OK;
EXIT_FUNC :
#if defined(CONFIG_DEBUG)
{
	HxTRANSPORT_HttpResponse_e	resCode;

	if ( hResult != ERR_OK )
	{
		HLIB_TRANSPORT_HttpGetHttpResponseCode(pHttp, &resCode);
		HxLOG_Debug("[%s] failed. errCode (%d)\n",  __FUNCTION__ ,resCode);
	}
}
#endif
	if(pHttpHeader != NULL) HLIB_TRANSPORT_HttpFreeHeader(pHttpHeader);
	if(pHttp != NULL) HLIB_TRANSPORT_HttpClose(pHttp);
	if(pAuthHash != NULL) IPEPG_MemFree(pAuthHash);
	if(pAuthTimeStamp != NULL) IPEPG_MemFree(pAuthTimeStamp);

	return hResult;
}


static HERROR	ipepg_SettingHttpHeadHeader(const HINT8 * szUrl, HINT8 **ppcHeader)
{
	HERROR							hError = ERR_FAIL;
	HERROR							hResult = ERR_FAIL;
	HxTRANSPORT_Http_t				*pHttp = NULL;
	HxTRANSPORT_Httpheader_t		*pHttpHeader = NULL;

    HxLOG_Trace();
	IPEPG_GOTO_IF(szUrl == NULL, EXIT_FUNC);

	pHttp = HLIB_TRANSPORT_HttpOpen();
	IPEPG_GOTO_IF(pHttp == NULL, EXIT_FUNC);

	hError = HLIB_TRANSPORT_HttpSetCACertificateFile(pHttp, FVC_SERVER_ROOTCA_FILE);
	IPEPG_GOTO_IF(hError != ERR_OK, EXIT_FUNC);

	hError = HLIB_TRANSPORT_HttpSetCertificateFile(pHttp, FVC_CLIENT_CA_FILE, eHxTP_HTTP_CERTIFICATE_PEM, NULL);
	IPEPG_GOTO_IF(hError != ERR_OK, EXIT_FUNC);

	hError = HLIB_TRANSPORT_HttpSetCertificateKeyFile(pHttp, FVC_CLIENT_CAKEY_FILE, eHxTP_HTTP_CERTIFICATE_PEM);
	IPEPG_GOTO_IF(hError != ERR_OK, EXIT_FUNC);

	hError = HLIB_TRANSPORT_HttpSetPeerVeryfication(pHttp, TRUE);
	IPEPG_GOTO_IF(hError != ERR_OK, EXIT_FUNC);

	hError = HLIB_TRANSPORT_HttpSetHostVeryfication(pHttp, TRUE);
	IPEPG_GOTO_IF(hError != ERR_OK, EXIT_FUNC);

	hError = HLIB_TRANSPORT_HttpSetHeaderToTP(pHttp, pHttpHeader);
	IPEPG_GOTO_IF(hError != ERR_OK, EXIT_FUNC);

    if (IPEPG_HTTP_TIMEOUT != 0)
    {
		hError = HLIB_TRANSPORT_HttpSetTimeout(pHttp, IPEPG_HTTP_TIMEOUT);
        IPEPG_GOTO_IF(hError != ERR_OK, EXIT_FUNC);
    }

	hError = HLIB_TRANSPORT_HttpHead(pHttp, szUrl, ppcHeader , IPEPG_HEADER_MAX_SIZE);
	IPEPG_GOTO_IF(hError != ERR_OK, EXIT_FUNC);

	hResult = ERR_OK;
EXIT_FUNC :
#if defined(CONFIG_DEBUG)
{
	HxTRANSPORT_HttpResponse_e		resCode = 0;

	if ( hResult != ERR_OK )
	{
		HLIB_TRANSPORT_HttpGetHttpResponseCode(pHttp, &resCode);
		HxLOG_Debug("[%s] failed. errCode (%d)\n",  __FUNCTION__ ,resCode);
		HxLOG_Debug("[%s] failed. hError (%d)\n",  __FUNCTION__ ,hError);
	}
}
#endif
	if(pHttpHeader != NULL) HLIB_TRANSPORT_HttpFreeHeader(pHttpHeader);
	if(pHttp != NULL) HLIB_TRANSPORT_HttpClose(pHttp);

	return hResult;
}

static HERROR	ipepg_SettingHttpGetWithCallback(const HINT8 * szUrl, HxHttpGet_Callback *callback, HUINT32 ulTimeout, DxFvcExpiry_t *pstFvcExpiry)
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

#if defined(CONFIG_OP_UK_DTT)
	pHttpHeader = HLIB_TRANSPORT_HttpAppendHeader(pHttpHeader, "Accept: application/vnd.fvc.v1+xml");
#endif

	IPEPG_GOTO_IF(HLIB_TRANSPORT_HttpSetHeaderToTP(pHttp, pHttpHeader) != ERR_OK, EXIT_FUNC);

    if(ulTimeout != 0)
    {
        IPEPG_GOTO_IF(HLIB_TRANSPORT_HttpSetTimeout(pHttp, ulTimeout) != ERR_OK, EXIT_FUNC);
    }

	IPEPG_GOTO_IF(HLIB_TRANSPORT_HttpGetWithCallback(pHttp, szUrl, callback, &ulRealSize) != ERR_OK, EXIT_FUNC);

	// TODO: get header expiry, last modified time

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

//#define IPEPG_PRINT_DUMP
static void ipepg_DumpPrint(DxIpEvent_t * pEvent)
{
#if defined(IPEPG_PRINT_DUMP)
	HUINT32 j = 0;
	HUINT8 * testBuf = (HUINT8 *)pEvent;
#endif

	return;

	if(pEvent)
	{
		return;

		HxLOG_Debug("type                    size[%d] data[%x]\n", sizeof(pEvent->stBase.type), pEvent->stBase.type);
		HxLOG_Debug("ucKind                  size[%d] data[%x]\n", sizeof(pEvent->stBase.ucKind), pEvent->stBase.ucKind);
		HxLOG_Debug("usOnId                  size[%d] data[%x]\n", sizeof(pEvent->stBase.usOnId), pEvent->stBase.usOnId);
		HxLOG_Debug("usTsId                  size[%d] data[%x]\n", sizeof(pEvent->stBase.usTsId), pEvent->stBase.usTsId);
		HxLOG_Debug("usSvcId                 size[%d] data[%x]\n", sizeof(pEvent->stBase.usSvcId), pEvent->stBase.usSvcId);
		HxLOG_Debug("ulUniqueId              size[%d] data[%x]\n", sizeof(pEvent->stBase.ulUniqueId), pEvent->stBase.ulUniqueId);
		HxLOG_Debug("usSvcUId                 size[%d] data[%x]\n", sizeof(pEvent->stBase.ulSvcUid), pEvent->stBase.ulSvcUid);
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

		{
			HUINT32 i = 0, j = 0;
			for(i = 0; i< pEvent->ulStrCount; i++)
			{
				HxLOG_Debug("INDEX[%d] size[%d] type[%x] [%s]\n",i, pEvent->stText[i].usSize, pEvent->stText[i].eType, pEvent->stText[i].ucStr);
#if defined(IPEPG_PRINT_DUMP)
				for(j = 0 ;  j < pEvent->stText[i].usSize; j++)
				{
					if(j%16 == 0)
					{
						HxLOG_Debug("\n");
					}
					HxLOG_Debug("%02x ", pEvent->stText[i].ucStr[j]);
				}
				HxLOG_Debug("\n");
#endif
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
					HxLOG_Debug("	IMAGES[%d] URLSIZE [%s] \n", j , pEvent->stMedia[i].stMediaImage[j].ulUrlSize);
				}
			}
		}
#if defined(IPEPG_PRINT_DUMP)
		for(i = 0; i < sizeof(DxIpEvent_t); i++)
		{
			if(i%16 == 0)
				HxLOG_Debug("\n");

			HxLOG_Debug("%02x ", testBuf[i]);
		}
#endif
	}
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
				THAPI_GetThumbnailURL(pEvent->stText[i].ucStr, 0, "/mnt/hd1", FALSE);
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

static void ipepg_StreamWrite_DxIpUKDTTEventStr(HUINT32 ulSD, HUINT16 usSize, DxIpUKDTTEventStr_t * pString)
{
	HLIB_RWSTREAM_Write(ulSD,(void *)&usSize , sizeof(HUINT16));
	if(pString->usSize)
	{
		HLIB_RWSTREAM_Write(ulSD, (void *)&pString->eType, sizeof(DxIPEPG_UKDTT_TEXT_e));
		HLIB_RWSTREAM_Write(ulSD, (void *)pString->ucLang, EPG_HUMAX_LANG_LEN);
		HLIB_RWSTREAM_Write(ulSD, (void *)pString->ucStr, pString->usSize+1);
	}
}

static void ipepg_SetDownloadProgress(HBOOL bBackwardSupport, HBOOL bForwardMode,HUINT32 ulCurrentIndex, HUINT32 ulTotalCount)
{
	HUINT32 ulPercent = 0;
	static HUINT32 ulPrevPercent = 0;

	// percent
	ulCurrentIndex = ulCurrentIndex +1;	// zero base value
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
	//IPEPG_SetItem(eIPEPG_ITEM_DOWNLOADPROGRESS, (void *)51);
	if(0 != ulPercent && ulPrevPercent != ulPercent)
	{
		IPEPG_SetItem(eIPEPG_ITEM_DOWNLOADPROGRESS, (void *)ulPercent);
		ulPrevPercent = ulPercent;
	}
}

static void 	ipepg_SetIpSvc (void *pvListData)
{
	EPG_URL_INFO_t	*pSvcInfo = (EPG_URL_INFO_t *)pvListData;
	DxIpService_t 	ipSvcInfo;

	IPEPG_RETURN_IF_VOID(pSvcInfo==NULL);

	ipSvcInfo.usOnId = pSvcInfo->stTripleId.usOnId;
	ipSvcInfo.usTsId = pSvcInfo->stTripleId.usTsId;
	ipSvcInfo.usSvcId = pSvcInfo->stTripleId.usSvcId;
	ipSvcInfo.ulSvcUid = pSvcInfo->ulSvcUid;
	ipSvcInfo.ulMediaSupport = pSvcInfo->bSupportMedia;

	APK_META_QUERY_SetIpSvc(ipSvcInfo.ulSvcUid, &ipSvcInfo, sizeof(DxIpService_t));
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
			//if(pEvent->ulMediaCount > 0)
			//	ipepg_DumpPrint(pEvent);

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

			// extension binaries
#if defined(CONFIG_PROD_FVP4000T)
			// CRID
			ulBinSize += sizeof(pEvent->ext.ukdtt.usCRIDCount);
			for (i=0; i<pEvent->ext.ukdtt.usCRIDCount; i++ )
			{
				if(pEvent->ext.ukdtt.stCRID[i].usSize)	ulBinSize += (pEvent->ext.ukdtt.stCRID[i].usSize+1+sizeof(HUINT16)+sizeof(DxIPEPG_UKDTT_TEXT_e)+EPG_HUMAX_LANG_LEN);
				else ulBinSize+= sizeof(HUINT16);
			}

			// Guidance Type
			ulBinSize += sizeof(pEvent->ext.ukdtt.ulGuidanceType);

			// Guidance
			ulBinSize += sizeof(pEvent->ext.ukdtt.usGuidanceCount);
			for (i=0; i<pEvent->ext.ukdtt.usGuidanceCount; i++ )
			{
				if(pEvent->ext.ukdtt.stGuidance.usSize)	ulBinSize += (pEvent->ext.ukdtt.stGuidance.usSize+1+sizeof(HUINT16)+sizeof(DxIPEPG_UKDTT_TEXT_e)+EPG_HUMAX_LANG_LEN);
				else ulBinSize+= sizeof(HUINT16);
			}

			// HD Linkage
			ulBinSize += sizeof(pEvent->ext.ukdtt.usHDLinkageOnId);
			ulBinSize += sizeof(pEvent->ext.ukdtt.usHDLinkageTsId);
			ulBinSize += sizeof(pEvent->ext.ukdtt.usHDLinkageSvcId);
			ulBinSize += sizeof(pEvent->ext.ukdtt.usHDLinkageEvtId);

			// Component
			ulBinSize += sizeof(pEvent->ext.ukdtt.eComponent);

			// OnDemand
			ulBinSize += sizeof(pEvent->ext.ukdtt.ulOnDemandCount);
			for (i=0; i<pEvent->ext.ukdtt.ulOnDemandCount; i++)
			{
				DxIpUKDTTEventStr_t * pString = NULL;

				ulBinSize += sizeof(pEvent->ext.ukdtt.stFvcOnDemand.ulStartOfAvailability);
				ulBinSize += sizeof(pEvent->ext.ukdtt.stFvcOnDemand.ulEndOfAvailability);
				ulBinSize += sizeof(pEvent->ext.ukdtt.stFvcOnDemand.ulPublishedDuration);
				ulBinSize += sizeof(pEvent->ext.ukdtt.stFvcOnDemand.ucFree);
				ulBinSize += sizeof(pEvent->ext.ukdtt.stFvcOnDemand.ucIsHD);
				ulBinSize += sizeof(pEvent->ext.ukdtt.stFvcOnDemand.ulOnDemandGenre);

				ulBinSize += sizeof(pEvent->ext.ukdtt.stFvcOnDemand.usOnDemandStrCount);
				for (j=0; j<pEvent->ext.ukdtt.stFvcOnDemand.usOnDemandStrCount; j++ )
				{
					pString = &pEvent->ext.ukdtt.stFvcOnDemand.stOnDemandStr[j];
					if(pString->usSize) ulBinSize += (pString->usSize+1+sizeof(HUINT16)+sizeof(DxIPEPG_UKDTT_TEXT_e)+EPG_HUMAX_LANG_LEN);
					else ulBinSize+= sizeof(HUINT16);
				}
			}

			// reserved
			ulBinSize += sizeof(pEvent->ext.ukdtt.ulUkdttReserved);
			ulBinSize += sizeof(pEvent->ext.ukdtt.ulUkdttReserved2);
#endif
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
					//HxLOG_Debug("Index[%d] eType[%x] ucLang[%c][%c][%c] str[%s]\n", index, pEvent->stText[i].eType, pEvent->stText[i].ucLang[0], pEvent->stText[i].ucLang[1], pEvent->stText[i].ucLang[2], pEvent->stText[i].ucStr);
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

#if defined(CONFIG_PROD_FVP4000T)
			// Write CRID
			HLIB_RWSTREAM_Write(ulSD, (void *)&(pEvent->ext.ukdtt.usCRIDCount) , sizeof(pEvent->ext.ukdtt.usCRIDCount));
			for (i=0; i<pEvent->ext.ukdtt.usCRIDCount; i++)
			{
				HUINT16 usSize = 0;
				DxIpUKDTTEventStr_t * pString = NULL;

				// CRID
				pString = &pEvent->ext.ukdtt.stCRID[i];
				usSize = pString->usSize?(pString->usSize+1):0;
				ipepg_StreamWrite_DxIpUKDTTEventStr(ulSD, usSize, pString);
			}

			// Write Guidance Type
			HLIB_RWSTREAM_Write(ulSD, (void *)&(pEvent->ext.ukdtt.ulGuidanceType) , sizeof(pEvent->ext.ukdtt.ulGuidanceType));

			// Write Guidance
			HLIB_RWSTREAM_Write(ulSD, (void *)&(pEvent->ext.ukdtt.usGuidanceCount) , sizeof(pEvent->ext.ukdtt.usGuidanceCount));
			if (pEvent->ext.ukdtt.usGuidanceCount > 0)
			{
				HUINT16 usSize = 0;
				DxIpUKDTTEventStr_t * pString = NULL;

				// Guidance
				pString = &pEvent->ext.ukdtt.stGuidance;
				usSize = pString->usSize?(pString->usSize+1):0;
				ipepg_StreamWrite_DxIpUKDTTEventStr(ulSD, usSize, pString);
			}

			// Write HD Linkage
			HLIB_RWSTREAM_Write(ulSD, (void *)&pEvent->ext.ukdtt.usHDLinkageOnId, sizeof(pEvent->ext.ukdtt.usHDLinkageOnId));
			HLIB_RWSTREAM_Write(ulSD, (void *)&pEvent->ext.ukdtt.usHDLinkageTsId, sizeof(pEvent->ext.ukdtt.usHDLinkageTsId));
			HLIB_RWSTREAM_Write(ulSD, (void *)&pEvent->ext.ukdtt.usHDLinkageSvcId, sizeof(pEvent->ext.ukdtt.usHDLinkageSvcId));
			HLIB_RWSTREAM_Write(ulSD, (void *)&pEvent->ext.ukdtt.usHDLinkageEvtId, sizeof(pEvent->ext.ukdtt.usHDLinkageEvtId));

			// Write Component
			HLIB_RWSTREAM_Write(ulSD, (void *)&pEvent->ext.ukdtt.eComponent, sizeof(pEvent->ext.ukdtt.eComponent));

			// Write OnDemand
			HLIB_RWSTREAM_Write(ulSD, (void *)&(pEvent->ext.ukdtt.ulOnDemandCount) , sizeof(pEvent->ext.ukdtt.ulOnDemandCount));
			for (i=0; i<pEvent->ext.ukdtt.ulOnDemandCount; i++)
			{
				DxIpEventFvcOnDemand_t *pOnDemand = NULL;

				pOnDemand = &(pEvent->ext.ukdtt.stFvcOnDemand);
				HLIB_RWSTREAM_Write(ulSD, (void *)&pOnDemand->ulStartOfAvailability, sizeof(pOnDemand->ulStartOfAvailability));
				HLIB_RWSTREAM_Write(ulSD, (void *)&pOnDemand->ulEndOfAvailability, sizeof(pOnDemand->ulEndOfAvailability));
				HLIB_RWSTREAM_Write(ulSD, (void *)&pOnDemand->ulPublishedDuration, sizeof(pOnDemand->ulPublishedDuration));
				HLIB_RWSTREAM_Write(ulSD, (void *)&pOnDemand->ucFree, sizeof(pOnDemand->ucFree));
				HLIB_RWSTREAM_Write(ulSD, (void *)&pOnDemand->ucIsHD, sizeof(pOnDemand->ucIsHD));
				HLIB_RWSTREAM_Write(ulSD, (void *)&pOnDemand->ulOnDemandGenre, sizeof(pOnDemand->ulOnDemandGenre));

				HLIB_RWSTREAM_Write(ulSD, (void *)&pOnDemand->usOnDemandStrCount, sizeof(pOnDemand->usOnDemandStrCount));
				for (j=0; j<pOnDemand->usOnDemandStrCount; j++)
				{
					HUINT16	usSize = 0;
					DxIpUKDTTEventStr_t * pString = NULL;

					pString = &pOnDemand->stOnDemandStr[j];
					usSize = pString->usSize?(pString->usSize+1):0;
					ipepg_StreamWrite_DxIpUKDTTEventStr(ulSD, usSize, pString);
				}
			}

			// Write reserved
			HLIB_RWSTREAM_Write(ulSD, (void *)&pEvent->ext.ukdtt.ulUkdttReserved, sizeof(pEvent->ext.ukdtt.ulUkdttReserved));
			HLIB_RWSTREAM_Write(ulSD, (void *)&pEvent->ext.ukdtt.ulUkdttReserved2, sizeof(pEvent->ext.ukdtt.ulUkdttReserved2));
#endif
			//HxLOG_Print("eventUid[%lld] SIZE BASE[%d] EVENT[%d] binsize[%d]\n",eventUid, sizeof(DxIpEventBase_t), sizeof(DxIpEvent_t), ulBinSize);

			if(APK_META_QUERY_SetIpEvent(eventUid, HLIB_RWSTREAM_GetBuf(ulSD), ulBinSize) != ERR_OK)
			{
				HxLOG_Error("DAPI SET FAIL~~~~~~~~~~~~~~~~~~~~~~~~\n");
			}
			HLIB_RWSTREAM_Close(ulSD);
		}
		else if(pUpdateEvent->eUpdateType == eIPEPG_EVENT_TYPE_DELETE)
		{
			HxLOG_Print("DELETE eventUid[%lld]\n", eventUid);
			APK_META_QUERY_ResetIpEvent(eventUid);
		}
	}
	return FALSE;
}

#if defined(IPEPG_USE_COMPRESS)
static HERROR	ipepg_DecompressData(HUINT8 * pcReceivedData, HUINT32 ulSize, HUINT8 ** DecompData, HUINT32 * pulSize)
{
	IPEPG_RETURN_IF(pcReceivedData == NULL || ulSize == 0, ERR_FAIL);

	//HxLOG_Debug("File Size [%d]\n", ulSize);
	HLIB_FILE_Delete(IPEPG_TEMP_ZIP_FILE);
	HLIB_FILE_Delete(IPEPG_TEMP_DECOMPRESS_FILE);

	IPEPG_RETURN_IF(HLIB_FILE_WriteFile(IPEPG_TEMP_ZIP_FILE, pcReceivedData, ulSize) != ERR_OK, ERR_FAIL);
	IPEPG_RETURN_IF(HLIB_FILE_Exist(IPEPG_TEMP_ZIP_FILE) != TRUE, ERR_FAIL);

	if(HLIB_GZ_Uncompress(IPEPG_TEMP_ZIP_FILE, IPEPG_TEMP_DECOMPRESS_FILE) == ERR_OK)
	{
		IPEPG_RETURN_IF(HLIB_FILE_Exist(IPEPG_TEMP_DECOMPRESS_FILE) != TRUE, ERR_FAIL);
		IPEPG_MemFree(pcReceivedData);
		IPEPG_RETURN_IF(HLIB_FILE_ReadFile(IPEPG_TEMP_DECOMPRESS_FILE, (void **)DecompData, pulSize) != ERR_OK, ERR_FAIL);
	}

	HLIB_FILE_Delete(IPEPG_TEMP_ZIP_FILE);
	HLIB_FILE_Delete(IPEPG_TEMP_DECOMPRESS_FILE);

	return ERR_OK;
}
#endif

static HERROR	ipepg_ChLogoSendToDama(HxList_t *phSvcList, EPG_URL_INFO_t	*EventURLInfo)
{
	HxList_t * pTempList = NULL;
	HUINT8 * pURL = NULL;

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
						if(pstChannelLogo->ucServerLogoUrl) THAPI_GetThumbnailURL(pstChannelLogo->ucServerLogoUrl, 0, IPEPG_CHANNEL_LOGO_MOUNT_PATH, FALSE);
						APK_META_QUERY_SetIpLogo(ipSvcInfo->ulUid, pstChannelLogo, sizeof(DxChannelLogo_t));
					}
					IPEPG_MemFree(pstChannelLogo);
				}
			}
		}
		pTempList = pTempList->next;
	}
	return ERR_OK;
}




static void ipepg_MemFreeIpEvent (void * mem)
{
	HUINT32 i = 0, j = 0;
	DxIpEvent_t * event	= (DxIpEvent_t *)mem;

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
#if defined(CONFIG_PROD_FVP4000T)
		IPEPG_MemFree(event->ext.ukdtt.stProgramId.ucStr);
		for (i=0; i <event->ext.ukdtt.usCRIDCount; i++)
		{
			IPEPG_MemFree(event->ext.ukdtt.stCRID[i].ucStr);
		}
		if (event->ext.ukdtt.usGuidanceCount >0 )
		{
			IPEPG_MemFree(event->ext.ukdtt.stGuidance.ucStr);
		}
#endif

		IPEPG_MemFree(event);
	}
}

static void ipepg_MemFreeEvent(void *mem)
{
	if(mem)
	{
		IPEPG_UpdateEvent_t * pUpdateEvent = (IPEPG_UpdateEvent_t *)mem;
		DxIpEvent_t * event = NULL;
		if(pUpdateEvent)
		{
			event = pUpdateEvent->pIpEvent;
			ipepg_MemFreeIpEvent(event);
			IPEPG_MemFree(pUpdateEvent);
		}
	}
}

void IPEPG_MemFreeEvent (void *mem)
{
	ipepg_MemFreeEvent(mem);
}

void IPEPG_MemFreeIpEvent (void *mem)
{
	ipepg_MemFreeIpEvent(mem);
}

static HUINT32	ipepg_DownloadEventList_v2 (HUINT8 *pURL
											, HINT8 **ppcReceivedHeader
											, HINT8 **ppcReceivedBody
											, HFLOAT64 *pflReceivedBodySize
											, DxFvcExpiry_t	*pstFvcExpiry
											)
{
	HERROR		hError = ERR_FAIL;

	IPEPG_FUNC_START();

	hError = ipepg_SettingHttpGetHeaderAndBody(pURL, ppcReceivedHeader, ppcReceivedBody, pflReceivedBodySize, pstFvcExpiry);
	if(hError != ERR_OK)
	{
		HxLOG_Debug(HxANSI_COLOR_RED("Download Failed. url=[%s]"),pURL);
		return ERR_FAIL;
	}
	 // ----------------------------------------------------------------------------------------


	//*pulReceivedBodySize = (HUINT32)s_epgdata_size;

	//HxLOG_Debug("SERVICE LIST DATA: [%d]\n", fReceivedSize);
#if 0 //defined(IPEPG_USE_COMPRESS)
	if(ipepg_DecompressData(pucReceivedBody, *pulReceivedBodySize, &pucReceivedBody, pulReceivedBodySize) != ERR_OK)
		HxLOG_Error("Service list decomp Error\n");
	else
		HxLOG_Debug("[%d] [%s] \n", *pulReceivedBodySize, pucReceivedBody);
#else
	//HxLOG_Debug("header [%s] \n", *ppcReceivedHeader);
	//HxLOG_Debug("body [%f] [%s] \n", *pflReceivedBodySize, *ppcReceivedBody);
#endif

	IPEPG_FUNC_END();
	return ERR_OK;
}

static HERROR	ipepg_DownloadSvcList(IPEPG_DATAMGR_t 	* pstData
											, HINT8 **ppcReceivedHeader
											, HINT8 **ppcReceivedBody
											, HFLOAT64 *pflReceivedBodySize
											, DxFvcExpiry_t	*pstFvcExpiry
											)
{
	HERROR		hError = ERR_FAIL;
	HUINT8		pServiceListUrl[IPEPG_URL_MAX_LEN]={0,};
	HUINT8		*pServerURL = NULL;

	HxVector_t	*pstNetworkVector = NULL;
	HINT32		onId;
	HINT32		nListCount;
	DxNetwork_t	*pstNetwork;

	IPEPG_FUNC_START();

	pServerURL = IPEPG_GetItemPoint(eIPEPG_ITEM_FVC_BASE_URL);
	IPEPG_GOTO_IF(pServerURL == NULL, END_DOWNLOADSVC);

	pstNetworkVector = APK_META_SVC_GetNetworkList();
	IPEPG_GOTO_IF(pstNetworkVector == NULL, END_DOWNLOADSVC);

	nListCount = HLIB_VECTOR_Length(pstNetworkVector);
	IPEPG_GOTO_IF(nListCount < 1, END_DOWNLOADSVC);

	pstNetwork = (DxNetwork_t *)HLIB_VECTOR_ItemAt(pstNetworkVector, 0);
	IPEPG_GOTO_IF(pstNetwork == NULL, END_DOWNLOADSVC);

	onId = pstNetwork->onid;

	HxSTD_snprintf(pServiceListUrl, IPEPG_URL_MAX_LEN, "%s/%s?%s=%d", pServerURL, IPEPG_SERVICELIST_URL,IPEPG_NETWORK_ID_URL,onId);
	HxLOG_Debug("URL : %s \n", pServiceListUrl);

	hError = ipepg_SettingHttpGetHeaderAndBody(pServiceListUrl, ppcReceivedHeader, ppcReceivedBody, pflReceivedBodySize,pstFvcExpiry);
	IPEPG_GOTO_IF(hError != ERR_OK, END_DOWNLOADSVC);

	 // ----------------------------------------------------------------------------------------


	//*pulReceivedBodySize = (HUINT32)s_epgdata_size;

	//HxLOG_Debug("SERVICE LIST DATA: [%d]\n", fReceivedSize);
#if 0 //defined(IPEPG_USE_COMPRESS)
	if(ipepg_DecompressData(pucReceivedBody, *pulReceivedBodySize, &pucReceivedBody, pulReceivedBodySize) != ERR_OK)
		HxLOG_Error("Service list decomp Error\n");
	else
		HxLOG_Debug("[%d] [%s] \n", *pulReceivedBodySize, pucReceivedBody);
#else
	//HxLOG_Debug("header [%s] \n", *ppcReceivedHeader);
	//HxLOG_Debug("body [%f] [%s] \n", *pflReceivedBodySize, *ppcReceivedBody);
#endif

	hError = ERR_OK;

END_DOWNLOADSVC:
	if (pstNetworkVector) {HLIB_VECTOR_Delete(pstNetworkVector);}
	if (pServerURL) {IPEPG_MemFree(pServerURL);}
	IPEPG_FUNC_END();

	return hError;
}

void	IPEPG_DeleteAllData(void)
{
	APK_META_QUERY_ResetIpEventTable();
	APK_META_QUERY_ResetIpSvcTable();
	APK_META_QUERY_ResetIpLogoTable();
	APK_META_QUERY_ResetFvcExpiryTable();
	HxLOG_Warning("IPEPG_DeleteAllData Finished\n");
}


void ipepg_FreeQuery(void *mem)
{
	if(mem)
	{
		EPG_URL_Query_t * query = mem;
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
			UrlParser->pQuery = HLIB_LIST_Remove(UrlParser->pQuery,(void *)FindQuery);
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
	HUINT8 * tempURL = pURL;
	HxList_t * list = NULL;
	EPG_URL_Query_t * pQuery = NULL;

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

static HERROR __ipepg_makeDownloadEventURL(
	HUINT32				usOnId,
	HUINT8 			* 	pucCommand,
	HUINT32 			ulTotalStart,
	HUINT8 			* 	pucTargetEventURL, /*MUST USE IPEPG_URL_MAX_LEN*/
	IPEPG_FVC_URL_TYPE_e eUrlType
)
{
	EPG_URL_PARSER_t 	*UrlParser = NULL;
	HUINT8				pTempURL[IPEPG_URL_MAX_LEN] ={0,};
	HUINT8 				pDateValue[12] = {0,};
	HUINT8				*pucServerURL = NULL;

	if (NULL == pucCommand || NULL == pucTargetEventURL)
	{
		HxLOG_Debug("[%s] param fail. pucCommand(%p) pucTargetEventURL(%p)\n",
			__FUNCTION__, pucCommand, pucTargetEventURL);
		return ERR_FAIL;
	}

	pucServerURL = IPEPG_GetItemPoint(eIPEPG_ITEM_FVC_BASE_URL);

	HxSTD_MemSet(pTempURL, 0, IPEPG_URL_MAX_LEN);
	HxSTD_snprintf(pTempURL, IPEPG_URL_MAX_LEN, "%s/%s?", pucServerURL, (HUINT8 *)pucCommand);
	IPEPG_MemFree(pucServerURL);
	UrlParser = ipepg_UrlParse(pTempURL);
	if(NULL == UrlParser)
	{
		HxLOG_Debug("[%s] ipepg_urlParse failed. (%s)\n", __FUNCTION__, pTempURL);
		return ERR_FAIL;
	}

	// Start Time
	HxSTD_MemSet(pDateValue, 0, 12);
	HxSTD_snprintf(pDateValue, 12, "%d", ulTotalStart);
	ipepg_MakeQuery(UrlParser, IPEPG_STARTTIME_URL, pDateValue);

	// End Time
	HxSTD_MemSet(pDateValue, 0, 12);
	HxSTD_snprintf(pDateValue, 12, "%d", ulTotalStart + 60*60*IPEPG_PART_OF_TIME);
	ipepg_MakeQuery(UrlParser, IPEPG_ENDTIME_URL, pDateValue);

	if (eUrlType == eIPEPG_FVC_URL_EXPIRY_ONID)
	{
	HxSTD_MemSet(pDateValue, 0, 12);
	HxSTD_snprintf(pDateValue, 12, "%d", usOnId);
	ipepg_MakeQuery(UrlParser, IPEPG_NETWORK_ID_URL, pDateValue);
	}
	else if (eUrlType == eIPEPG_FVC_URL_SERIES_SVCID)
	{
		HxSTD_MemSet(pDateValue, 0, 12);
		HxSTD_snprintf(pDateValue, 12, "%d", usOnId);
		ipepg_MakeQuery(UrlParser, IPEPG_SERVICEID_URL, pDateValue);
	}

	ipepg_MakeURLWithParser(UrlParser, pucTargetEventURL);
	ipepg_FreeUrlParser(UrlParser);

	return ERR_OK;
}

HERROR	ipepg_ProcessExpiryItem (DxFvcExpiry_t *pstFvcExpiry,HINT8 *pcReceivedData, HINT8 *pcReceivedHeader)
{
	HFLOAT64			flReceivedSize;

	HUINT8			pEventURL[IPEPG_URL_MAX_LEN] ={0,};
	HBOOL			bAbleToUseHdd = FALSE;
	HxList_t		*pEventList = NULL;
	HINT32			dummySvcUid = 0;
	HUINT32 		ulEventCount;

	HERROR			hErr = ERR_FAIL;
	HERROR			hResult = ERR_FAIL;

	DxFvcExpiry_t	stFvcExpiry;
	stFvcExpiry.statusCode = 500;

	IPEPG_GOTO_IF(pstFvcExpiry == NULL, END_FUNC);

	bAbleToUseHdd = (HBOOL)IPEPG_GetItemValue(eIPEPG_ITEM_HDD_STATUS);
	IPEPG_GOTO_IF(bAbleToUseHdd != TRUE, END_FUNC);

	HxSTD_MemSet(pcReceivedData, 0, IPEPG_EVENT_DATA_MAX_SIZE);
	HxSTD_MemSet(pcReceivedHeader, 0, IPEPG_HEADER_MAX_SIZE);

	hErr = __ipepg_makeDownloadEventURL(pstFvcExpiry->onid, (HUINT8 *)IPEPG_EPGLIST_URL, pstFvcExpiry->startTime,
										pEventURL, eIPEPG_FVC_URL_EXPIRY_ONID); //MUST USE IPEPG_URL_MAX_LEN

	HxLOG_Debug("try pURL : %s\n",pEventURL);
	hErr = ipepg_DownloadEventList_v2(pEventURL,&pcReceivedHeader, &pcReceivedData, &flReceivedSize, pstFvcExpiry);
	IPEPG_GOTO_IF(hErr != ERR_OK || flReceivedSize == 0, END_FUNC);

	hErr = ipepg_ParseHeader(pcReceivedHeader, &stFvcExpiry);
	if (hErr == ERR_OK)
	{
		stFvcExpiry.uid	  = pstFvcExpiry->uid;
		stFvcExpiry.onid = pstFvcExpiry->onid;
		stFvcExpiry.startTime = pstFvcExpiry->startTime;
		stFvcExpiry.queryStampTime = (UNIXTIME)HLIB_STD_GetSystemTime();
		stFvcExpiry.type = eDxFvcExpiry_Type_Schedule;
		HxLOG_Info(HxANSI_COLOR_BLUE(" dapi set uid(%lld) expiryTime(%d)\n"),stFvcExpiry.uid, stFvcExpiry.expiryTime);
		DAPI_Set(DxNAMEOF(DxFvcExpiry_t), stFvcExpiry.uid, &stFvcExpiry, sizeof(DxFvcExpiry_t));
	}

	// TODO: statusCode 처리 부분 함수로 묶기
	if (stFvcExpiry.statusCode == IPEPG_FVC_STATUS_CODE_NOT_MODIFIED)
	{
		hResult = ERR_OK;
	}

	IPEPG_GOTO_IF(stFvcExpiry.statusCode!=IPEPG_FVC_STATUS_CODE_OK,END_FUNC);
	hResult = ERR_OK;

	ipepg_ParseEventList(pcReceivedData, flReceivedSize, &pEventList, dummySvcUid);
	IPEPG_GOTO_IF(pEventList == NULL || HLIB_LIST_Length(pEventList) == 0, END_FUNC);

	ulEventCount =	HLIB_LIST_Length(pEventList);
	HxLOG_Debug("ulEventCount : %d\n",ulEventCount);
	HLIB_LIST_ForeachEx(pEventList , ipepg_EventSendToDama, (void *)eIPEPG_EVENT_TYPE_UPDATE);

#if !defined(IPEPG_THUMBNAIL_NON_DOWN)
	if(APK_POWER_IsStandbyMode() == TRUE)
	{
		HLIB_LIST_Foreach(pEventList , ipepg_GetThumbnail);
	}
#endif

END_FUNC:
	if (pEventList)
	{
		HLIB_LIST_Foreach(pEventList , ipepg_MemFreeEvent);
		HLIB_LIST_RemoveAll(pEventList);
		pEventList = NULL;
	}

	pstFvcExpiry->statusCode = stFvcExpiry.statusCode;

	return hResult;

}

HERROR	ipepg_ProcessSeriesEvtItem (IPEPG_FvcSeriesEvent_t *pstFvcSeriesEvt,HINT8 *pcReceivedData, HINT8 *pcReceivedHeader)
{
	HFLOAT64			flReceivedSize;

	HUINT8			pEventURL[IPEPG_URL_MAX_LEN] ={0,};
	HBOOL			bAbleToUseHdd = FALSE;
	HxList_t		*pEventList = NULL;
	HINT32			dummySvcUid = 0;
	HUINT32 		ulEventCount;
	HUINT32			ulStartTime;

	HERROR			hErr = ERR_FAIL;
	HERROR			hResult = ERR_FAIL;

	DxFvcExpiry_t	stFvcExpiry;

	HxSTD_MemSet(&stFvcExpiry, 0x0, sizeof(DxFvcExpiry_t));
	stFvcExpiry.statusCode = 500;

	IPEPG_GOTO_IF(pstFvcSeriesEvt == NULL, END_FUNC);

	bAbleToUseHdd = (HBOOL)IPEPG_GetItemValue(eIPEPG_ITEM_HDD_STATUS);
	IPEPG_GOTO_IF(bAbleToUseHdd != TRUE, END_FUNC);

	HxSTD_MemSet(pcReceivedData, 0, IPEPG_EVENT_DATA_MAX_SIZE);
	HxSTD_MemSet(pcReceivedHeader, 0, IPEPG_HEADER_MAX_SIZE);

	ulStartTime = pstFvcSeriesEvt->ulStartTime - (pstFvcSeriesEvt->ulStartTime%60*60*IPEPG_PART_OF_TIME);
	hErr = __ipepg_makeDownloadEventURL(pstFvcSeriesEvt->usSvcId, (HUINT8 *)IPEPG_EPGLIST_URL, ulStartTime,
										pEventURL,eIPEPG_FVC_URL_SERIES_SVCID); //MUST USE IPEPG_URL_MAX_LEN

	HxLOG_Debug("try pURL : %s\n",pEventURL);
	hErr = ipepg_DownloadEventList_v2(pEventURL,&pcReceivedHeader, &pcReceivedData, &flReceivedSize, &stFvcExpiry);
	IPEPG_GOTO_IF(hErr != ERR_OK || flReceivedSize == 0, END_FUNC);

	hErr = ipepg_ParseHeader(pcReceivedHeader, &stFvcExpiry);
	if (hErr == ERR_OK)
	{
		pstFvcSeriesEvt->ulHttpStatusCode = stFvcExpiry.statusCode;
	}

	// TODO: statusCode 처리 부분 함수로 묶기
	// 200,400들은 더이상 시도하지 않도록 list에 들고 있어야 한다.
	// 401은 auth를 시도한 후 재시도 해야 한다
	// 500은 rule에 의해 재시도 해야 한다
	hResult = ERR_OK;

	// 301은 400처럼 parsing 안 하고 재시도 안한다.
	if (stFvcExpiry.statusCode == IPEPG_FVC_STATUS_CODE_NOT_MODIFIED)
	{
		stFvcExpiry.statusCode = IPEPG_FVC_STATUS_CODE_BAD;
	}

	// 200만 parsing해야 한다.
	IPEPG_GOTO_IF(stFvcExpiry.statusCode!=IPEPG_FVC_STATUS_CODE_OK,END_FUNC);

	ipepg_ParseEventList(pcReceivedData, flReceivedSize, &pEventList, dummySvcUid);
	IPEPG_GOTO_IF(pEventList == NULL || HLIB_LIST_Length(pEventList) == 0, END_FUNC);

	ulEventCount =	HLIB_LIST_Length(pEventList);
	HxLOG_Debug("ulEventCount : %d\n",ulEventCount);
	HLIB_LIST_ForeachEx(pEventList , ipepg_EventSendToDama, (void *)eIPEPG_EVENT_TYPE_UPDATE);

#if !defined(IPEPG_THUMBNAIL_NON_DOWN)
	if(APK_POWER_IsStandbyMode() == TRUE)
	{
		HLIB_LIST_Foreach(pEventList , ipepg_GetThumbnail);
	}
#endif

END_FUNC:
	if (pEventList)
	{
		HLIB_LIST_Foreach(pEventList , ipepg_MemFreeEvent);
		HLIB_LIST_RemoveAll(pEventList);
		pEventList = NULL;
	}

	pstFvcSeriesEvt->ulHttpStatusCode = stFvcExpiry.statusCode;

	return hResult;

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

void	IPEPG_DownloadFvcSvc (void *arg)
{
	HERROR				hErr 				= ERR_OK;
	IPEPG_TASK_MGR_t 	* pstTaskMgr 		= (IPEPG_TASK_MGR_t *)arg;
	IPEPG_DATAMGR_t 	* pstData			= NULL;
	HUINT32 			MsgQId = 0;

	HxList_t			*phSvcList			= NULL;
	HxHASH_t			*phSvcHash			= NULL;
	HxList_t			* ListOfEventURL	= NULL;

	UNIXTIME			utCurrTime			= 0;
	DxFvcExpiry_t		stStoredExpiry = {0,};

	HINT8				*pcReceivedData		= NULL;
	HINT8				*pcReceivedHeader	= NULL;
	HFLOAT64			flReceivedSize;
	DxFvcExpiry_t		stReceivedExpiry = {0, };
	stReceivedExpiry.statusCode = IPEPG_FVC_STATUS_CODE_OK;

	if( pstTaskMgr == NULL
		|| pstTaskMgr->pstData == NULL
		|| (pstTaskMgr->eStatus & eIPEPG_STOP) == eIPEPG_STOP)
	{
#if defined(CONFIG_DEBUG)
		IPEPG_SetDownloadDebugInfo(eIPEPG_DNLD_DBG_ENDED, (void *)TRUE);
		IPEPG_SetDownloadDebugInfo(eIPEPG_DNLD_DBG_ENDTIME,  (void *)NULL);
#endif
		IPEPG_FUNC_END();
		return;
	}

	pcReceivedData = (HUINT8 *)IPEPG_MemCalloc(IPEPG_SERVICE_LIST_MAX_SIZE);
	pcReceivedHeader = (HUINT8 *)IPEPG_MemCalloc(IPEPG_HEADER_MAX_SIZE);
	IPEPG_GOTO_IF(pcReceivedData == NULL || pcReceivedHeader == NULL,END_DOWNLOADDATA);

	utCurrTime = (UNIXTIME)HLIB_STD_GetSystemTime();
	hErr = IPEPG_GetServiceExpiry(&stStoredExpiry);
	if (hErr != ERR_OK)
	{
		HxLOG_Debug("Service Expiry does not exists. start now. \n");
	}
	else if (stStoredExpiry.expiryTime > utCurrTime)
	{
		HxLOG_Error("Service Expiry Time is not passed yet. \n");
		HxLOG_Error("Service Expiry Time=[%25s]. wait (%d) seconds. \n"
			,HLIB_DATETIME_UnixTimeToStringWithOutOffset(stStoredExpiry.expiryTime,NULL,0),stStoredExpiry.expiryTime - utCurrTime);
		goto END_DOWNLOADDATA;
	}

	HxSTD_MemSet(pcReceivedData, 0, IPEPG_SERVICE_LIST_MAX_SIZE);
	HxSTD_MemSet(pcReceivedHeader, 0, IPEPG_HEADER_MAX_SIZE);
	pstData = pstTaskMgr->pstData;
	hErr = ipepg_DownloadSvcList(pstData, &pcReceivedHeader, &pcReceivedData,&flReceivedSize,&stStoredExpiry);
	IPEPG_GOTO_IF(ERR_OK != hErr,END_DOWNLOADDATA);

	hErr = ipepg_ParseHeader(pcReceivedHeader, &stReceivedExpiry);
	if (hErr == ERR_OK)
	{
		stReceivedExpiry.uid	  = DxFVC_SERVICE_UID;
		stReceivedExpiry.queryStampTime = (UNIXTIME)HLIB_STD_GetSystemTime();
		stReceivedExpiry.type = eDxFvcExpiry_Type_Service;
		DAPI_Set(DxNAMEOF(DxFvcExpiry_t), stReceivedExpiry.uid, &stReceivedExpiry, sizeof(DxFvcExpiry_t));
		HxLOG_Debug("Set Next Service Expiry [%s] \n",HLIB_DATETIME_UnixTimeToStringWithOutOffset(stReceivedExpiry.expiryTime,NULL,0));
	}

	// TODO: code로 함수 묶기
	if (stReceivedExpiry.statusCode == IPEPG_FVC_STATUS_CODE_UNAUTHORIZED)
	{
		(void)IPEPG_RequestAuth();
		IPEPG_GOTO_IF(1, END_DOWNLOADDATA);
	}

	IPEPG_GOTO_IF(stReceivedExpiry.statusCode!=IPEPG_FVC_STATUS_CODE_OK,END_DOWNLOADDATA);

	hErr = ipepg_ParseServiceList(pcReceivedData, flReceivedSize, &ListOfEventURL);
	if (hErr != ERR_OK || NULL == ListOfEventURL )
	{
		HxLOG_Debug("ipepg_ParseServiceList failed. \n");
		// TODO: what !?
	}

	phSvcHash = IPEPG_GetItemPoint(eIPEPG_ITEM_SVCLIST);
	phSvcList = HLIB_HASH_GetValues (phSvcHash);

	(void)ipepg_ChLogoSendToDamaAll(pstTaskMgr, phSvcList, ListOfEventURL);
	HLIB_LIST_Foreach(ListOfEventURL, ipepg_SetIpSvc);



END_DOWNLOADDATA:
	if (pcReceivedHeader != NULL) {IPEPG_MemFree(pcReceivedHeader);	}
	if (pcReceivedData != NULL) {IPEPG_MemFree(pcReceivedData);}
	if (ListOfEventURL != NULL)
	{
		HLIB_LIST_Foreach(ListOfEventURL , IPEPG_MemFree);
		HLIB_LIST_RemoveAll(ListOfEventURL);
		ListOfEventURL = NULL;
	}
	if (phSvcList != NULL) {	HLIB_LIST_RemoveAll(phSvcList);	}
	if (phSvcHash != NULL) { HLIB_HASH_Destroy(phSvcHash); }

	if(IPEPG_GetMsgQId(&MsgQId) != ERR_FAIL)
	{
		IpEpgMsg_t	stMsg;

		stMsg.ulType   = 0;
		stMsg.ulMsg    = eIPEPG_MSG_DONE_DOWNLOAD;
		stMsg.ulParam1 = eIPEPG_DOWN_START;

		if (stReceivedExpiry.statusCode == IPEPG_FVC_STATUS_CODE_OK)
		{
			stMsg.ulParam2 = ERR_OK;
		}
		else
		{
			stMsg.ulParam2 = ERR_FAIL;
		}

		VK_MSG_Send(MsgQId, (void*)&stMsg, sizeof(IpEpgMsg_t));
	}

}

void	IPEPG_DownloadFvcEvt(void *arg)
{
	HERROR				hError;
	HUINT32 			MsgQId = 0;
	IPEPG_TASK_MGR_t 	* pstTaskMgr = (IPEPG_TASK_MGR_t *)arg;
	HxHASH_t			*pstExpiryHash		= NULL;
	HxHASH_TableIter_t	iter;
	DxFvcExpiry_t		*iterFvc			= NULL;
	HUINT32				ulHttpStatusCode = IPEPG_FVC_STATUS_CODE_OK;
	HINT8				*pcReceivedData = NULL;
	HINT8				*pcReceivedHeader = NULL;

	IPEPG_FUNC_START();

#if defined(CONFIG_DEBUG)
	IPEPG_SetDownloadDebugInfo(eIPEPG_DNLD_DBG_STARTED, (void *)TRUE);
	IPEPG_SetDownloadDebugInfo(eIPEPG_DNLD_DBG_STARTTIME,  (void *)NULL);
#endif

	if( pstTaskMgr == NULL
		|| pstTaskMgr->pstData == NULL
		|| (pstTaskMgr->eStatus & eIPEPG_STOP) == eIPEPG_STOP)
	{
		#if defined(CONFIG_DEBUG)
		IPEPG_SetDownloadDebugInfo(eIPEPG_DNLD_DBG_ENDED, (void *)TRUE);
		IPEPG_SetDownloadDebugInfo(eIPEPG_DNLD_DBG_ENDTIME,  (void *)NULL);
		#endif
		IPEPG_FUNC_END();
		return;
	}
	pcReceivedData = (HUINT8 *)IPEPG_MemCalloc(IPEPG_EVENT_DATA_MAX_SIZE);
	pcReceivedHeader = (HUINT8 *)IPEPG_MemCalloc(IPEPG_HEADER_MAX_SIZE);
	IPEPG_GOTO_IF(pcReceivedData== NULL || pcReceivedHeader == NULL, END_DOWNLOADDATA);

	pstExpiryHash = HLIB_HASH_NewFull(HLIB_HASH_GetIntKey, HLIB_HASH_IsIntEqual, NULL, HLIB_STD_MemFree_CB);
	IPEPG_GetScheduleExpiryHash(&pstExpiryHash,10);
	HxLOG_Debug(HxANSI_COLOR_BLUE("retrieved size =(%d) \n"),HLIB_HASH_Size(pstExpiryHash));

	DAPI_BatchBegin(DxNAMEOF(DxFvcExpiry_t));
	HLIB_HASH_IterInit(&iter, pstExpiryHash);

	while (HLIB_HASH_IterNext(&iter, NULL, (void **)&iterFvc))
	{
		hError = ipepg_ProcessExpiryItem(iterFvc,pcReceivedData,pcReceivedHeader);
		if (hError != ERR_OK)
		{
			ulHttpStatusCode = iterFvc->statusCode;
			HxLOG_Debug(HxANSI_COLOR_RED("Fail to process expiry. uid=(%lld), statusCode=(%d) \n"),iterFvc->uid,ulHttpStatusCode);
			break;
		}
	}

	DAPI_BatchEnd(DxNAMEOF(DxFvcExpiry_t));

END_DOWNLOADDATA:

	if (pstExpiryHash != NULL) {
		HLIB_HASH_Destroy(pstExpiryHash);
	}
	if (pcReceivedData) {IPEPG_MemFree((void*)pcReceivedData);}
	if (pcReceivedHeader) {IPEPG_MemFree((void*)pcReceivedHeader);}

	IPEPG_FUNC_END();

#if defined(CONFIG_DEBUG)
		IPEPG_SetDownloadDebugInfo(eIPEPG_DNLD_DBG_ENDED, (void *)TRUE);
		IPEPG_SetDownloadDebugInfo(eIPEPG_DNLD_DBG_ENDTIME, (void *)NULL);
#endif

	if(IPEPG_GetMsgQId(&MsgQId) != ERR_FAIL)
	{
		IpEpgMsg_t	stMsg;

		stMsg.ulType   = 0;
		stMsg.ulMsg    = eIPEPG_MSG_DONE_DOWNLOAD_FVC_SCHEDULE;
		stMsg.ulParam1 = eIPEPG_FVC_START;

		if (ulHttpStatusCode == IPEPG_FVC_STATUS_CODE_OK)
		{
			stMsg.ulParam2 = ERR_OK;
		}
		else
		{
			stMsg.ulParam2 = ERR_FAIL;
			stMsg.ulParam3 = ulHttpStatusCode;
		}

		VK_MSG_Send(MsgQId, (void*)&stMsg, sizeof(IpEpgMsg_t));
	}

	return;
}


void	IPEPG_DownloadFvcSeriesEvt (void *arg)
{
	HERROR				hError;
	HUINT32 			MsgQId = 0;
	IPEPG_TASK_MGR_t 	* pstTaskMgr = (IPEPG_TASK_MGR_t *)arg;
	HxList_t			*iter 				= NULL;
	HUINT32				ulHttpStatusCode = IPEPG_FVC_STATUS_CODE_OK;
	HINT8				*pcReceivedData = NULL;
	HINT8				*pcReceivedHeader = NULL;
	HxList_t			*pstSeriesEvtList = NULL;
	IPEPG_FvcSeriesEvent_t *pstSeriesEvt = NULL;
	HBOOL				bIsNeedRetry = FALSE;

	IPEPG_FUNC_START();

	if( pstTaskMgr == NULL
		|| pstTaskMgr->pstData == NULL
		|| (pstTaskMgr->eStatus & eIPEPG_STOP) == eIPEPG_STOP)
	{
		IPEPG_FUNC_END();
		return;
	}
	pcReceivedData = (HUINT8 *)IPEPG_MemCalloc(IPEPG_EVENT_DATA_MAX_SIZE);
	pcReceivedHeader = (HUINT8 *)IPEPG_MemCalloc(IPEPG_HEADER_MAX_SIZE);
	IPEPG_GOTO_IF(pcReceivedData== NULL || pcReceivedHeader == NULL, END_DOWNLOADDATA);

	pstSeriesEvtList = IPEPG_GetItemPoint(eIPEPG_ITEM_FVC_SERIES_EVENT_LIST);
	IPEPG_GOTO_IF(pstSeriesEvtList== NULL, END_DOWNLOADDATA);

	for (iter= HLIB_LIST_First(pstSeriesEvtList); iter != NULL; iter = HLIB_LIST_Next(iter))
	{
		pstSeriesEvt = HLIB_LIST_Data(iter);
		if (pstSeriesEvt == NULL)
		{
			HxLOG_Debug(HxANSI_COLOR_RED("null series evt. iter=(0x%x) \n"),iter);
			continue;
		}

		if (pstSeriesEvt->ulHttpStatusCode == IPEPG_FVC_STATUS_CODE_BAD)
		{
			HxLOG_Debug("Previously got bad response. don't try again. uid=(%d), svcid=(%d), time=(%d) \n"
							,pstSeriesEvt->uid, pstSeriesEvt->usSvcId, pstSeriesEvt->ulStartTime);
			continue;
		}

		if (pstSeriesEvt->ulHttpStatusCode == IPEPG_FVC_STATUS_CODE_OK)
		{
			HxLOG_Debug("Previously got ok response. don't try again. uid=(%d), svcid=(%d), time=(%d) \n"
							,pstSeriesEvt->uid, pstSeriesEvt->usSvcId, pstSeriesEvt->ulStartTime);
			continue;
		}

		if (pstSeriesEvt->ulHttpStatusCode != 0)
		{
			HxLOG_Debug("Previously got %d. try again. uid=(%d), svcid=(%d), time=(%d) \n",pstSeriesEvt->ulHttpStatusCode
							,pstSeriesEvt->uid, pstSeriesEvt->usSvcId, pstSeriesEvt->ulStartTime);
		}
		hError = ipepg_ProcessSeriesEvtItem(pstSeriesEvt,pcReceivedData,pcReceivedHeader);

		// update http status code
		ulHttpStatusCode = pstSeriesEvt->ulHttpStatusCode;
		hError |= IPEPG_DeleteItemFromPoint(eIPEPG_ITEM_FVC_SERIES_EVENT_LIST, pstSeriesEvt);
		hError |= IPEPG_AddItemToPoint(eIPEPG_ITEM_FVC_SERIES_EVENT_LIST, pstSeriesEvt);
		if (hError != ERR_OK)
		{
			HxLOG_Debug(HxANSI_COLOR_RED("Fail to update status. uid=(%lld), statusCode=(%d) \n"),pstSeriesEvt->uid,ulHttpStatusCode);
		}

		// download를 중지하고 재시도 해야 하는 status는 break 한다.
		if ( !(pstSeriesEvt->ulHttpStatusCode == IPEPG_FVC_STATUS_CODE_BAD
			|| pstSeriesEvt->ulHttpStatusCode == IPEPG_FVC_STATUS_CODE_OK) )
		{
			bIsNeedRetry = TRUE;
			HxLOG_Debug(HxANSI_COLOR_RED("Fail to process item. uid=(%lld), statusCode=(%d) \n"),pstSeriesEvt->uid,ulHttpStatusCode);
			break;
		}
	}

END_DOWNLOADDATA:

	if (pstSeriesEvtList != NULL) {	HLIB_LIST_RemoveAll(pstSeriesEvtList);}
	if (pcReceivedData) {IPEPG_MemFree((void*)pcReceivedData);}
	if (pcReceivedHeader) {IPEPG_MemFree((void*)pcReceivedHeader);}

	IPEPG_FUNC_END();

	if (IPEPG_GetMsgQId(&MsgQId) != ERR_FAIL)
	{
		IpEpgMsg_t	stMsg;

		stMsg.ulType   = 0;
		stMsg.ulMsg    = eIPEPG_MSG_DONE_DOWNLOAD_FVC_SERIES_SCHEDULE;
		stMsg.ulParam1 = eIPEPG_FVC_SERIES_EPG_START;

		if (bIsNeedRetry == FALSE)
		{
			stMsg.ulParam2 = ERR_OK;
		}
		else
		{
			stMsg.ulParam2 = ERR_FAIL;
			stMsg.ulParam3 = ulHttpStatusCode;
		}

		VK_MSG_Send(MsgQId, (void*)&stMsg, sizeof(IpEpgMsg_t));
	}

	return;
}

/***********************
* FVC의 경우 하루에 한번 IP EPG를 받는 구조가 아니다
* 그러나 Base의 형상은 하루에 한번 IP EPG를 받는 형상이므로
* 형상을 맞춰주기 위한 Dummy Task를 만든다.
* 실제 Download 동작은 DownloadFvc에 두고
* DownloadData는 msg를 연결하는 dummy로 둔다.
*************************/
void	IPEPG_DownloadData(void *arg)
{
	HUINT32 			MsgQId = 0;
	HBOOL				bDownloadFail = FALSE;
	HERROR				ret = ERR_OK;

	if(IPEPG_GetMsgQId(&MsgQId) != ERR_FAIL)
	{
		IpEpgMsg_t	stMsg;

		stMsg.ulType   = 0;
		stMsg.ulMsg    = eIPEPG_MSG_DONE_DOWNLOAD;
		stMsg.ulParam1 = eIPEPG_DOWN_START;

		if(ret == ERR_FAIL) 		stMsg.ulParam2 = (HUINT32)ERR_FAIL;
		else						stMsg.ulParam2 = (HUINT32)ERR_OK;


		if(bDownloadFail == TRUE)	stMsg.ulParam3 = (HUINT32)ERR_FAIL;
		else						stMsg.ulParam3 = (HUINT32)ERR_OK;


		if(bDownloadFail == TRUE)
		{
			HxLOG_Debug("Ipepg Download Failed.....Set ulParam3 :%d\n", stMsg.ulParam3);
		}


		VK_MSG_Send(MsgQId, (void*)&stMsg, sizeof(IpEpgMsg_t));
	}

}

HERROR	IPEPG_HeadFvcPrefixUrl (HCHAR *szUriPrefix)
{
	HERROR		hError;
	HERROR		hResult = ERR_FAIL;

	HCHAR		*pcHeader = NULL;
	HCHAR 		pucTempValue[FVC_EXPIRY_DATE_LENGTH] = {0,};
	HINT32		nLen;

	HCHAR		*szCurProcessName = NULL;

	IPEPG_FUNC_START();

	szCurProcessName = HLIB_STD_GetCurProcessName();
	HxLOG_Debug("head fvc url [%s] \n",szUriPrefix);

	pcHeader = (HUINT8 *)IPEPG_MemCalloc(IPEPG_HEADER_MAX_SIZE);
	IPEPG_GOTO_IF(pcHeader ==NULL, END_HEAD_FVC_BASE_URL);

	hError = ipepg_SettingHttpHeadHeader((HINT8*)szUriPrefix, (HINT8**)&pcHeader);
	IPEPG_GOTO_IF(hError != ERR_OK, END_HEAD_FVC_BASE_URL);

	nLen = IPEPG_ParseHeaderFieldValue(pucTempValue,pcHeader, IPEPG_FVC_HEADER_BASE_URL, IPEPG_HEADER_MAX_SIZE-10);
	IPEPG_GOTO_IF(nLen == -1, END_HEAD_FVC_BASE_URL);
	HxLOG_Debug("Set eIPEPG_ITEM_FVC_BASE_URL [%s] \n",pucTempValue);
	IPEPG_SetItem(eIPEPG_ITEM_FVC_BASE_URL, pucTempValue);
	HAPI_BroadcastSignal("signal:onFvcBaseUrlChanged", 0, "ss", pucTempValue,szCurProcessName);

	nLen = IPEPG_ParseHeaderFieldValue(pucTempValue,pcHeader, IPEPG_FVC_HEADER_AUTH_HASH, IPEPG_HEADER_MAX_SIZE-10);
	IPEPG_GOTO_IF(nLen == -1, END_HEAD_FVC_BASE_URL);
	HxLOG_Debug("Set eIPEPG_ITEM_FVC_AUTH_HASH [%s] \n",pucTempValue);
	IPEPG_SetItem(eIPEPG_ITEM_FVC_AUTH_HASH, pucTempValue);
	HAPI_BroadcastSignal("signal:onFvcAuthHashChanged", 0, "ss", pucTempValue,szCurProcessName);

	nLen = IPEPG_ParseHeaderFieldValue(pucTempValue,pcHeader, IPEPG_FVC_HEADER_AUTH_TIMESTAMP, IPEPG_HEADER_MAX_SIZE-10);
	IPEPG_GOTO_IF(nLen == -1, END_HEAD_FVC_BASE_URL);
	HxLOG_Debug("Set eIPEPG_ITEM_FVC_AUTH_TIME_STAMP [%s] \n",pucTempValue);
	IPEPG_SetItem(eIPEPG_ITEM_FVC_AUTH_TIME_STAMP, pucTempValue);
	HAPI_BroadcastSignal("signal:onFvcAuthTimeStampChanged", 0, "ss", pucTempValue,szCurProcessName);

	hResult = ERR_OK;
END_HEAD_FVC_BASE_URL:
	if (pcHeader) {IPEPG_MemFree(pcHeader);}
	IPEPG_FUNC_END();

	return hResult;
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
				case eIPEPG_MSG_START_DOWNLOAD_FVC_SCHEDULE:
					IPEPG_DownloadFvcEvt(pstTaskMgr);
					break;
				case eIPEPG_MSG_START_DOWNLOAD_FVC_SERIES_SCHEDULE:
					IPEPG_DownloadFvcSeriesEvt(pstTaskMgr);
					break;
				default :
					HxLOG_Error("IPEPG_DownloadDataTask reveived OtherMessage[%d]\n", stMsg.ulMsg);
					break;
			}
		}
	}
	return;
}

/*********************** End of File ******************************/


