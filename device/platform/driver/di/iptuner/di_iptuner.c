/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   di_iptuner.c  $
 * Version:			$Revision:   1.0  $
 * Original Author:	SungYong Jeon
 * Current Author:	$Author: junsy@humaxdigital.com $
 * Date:			$Date:   Tue May 20 21:49:01 KST 2009 $
 * File Description:	     IP tuner Main API
 * Module:
 * Remarks:
 */

/*******************************************************************/
/* Copyright (c) 2009 HUMAX Co., Ltd. 								*/
/* All rights reserved.												*/
/*******************************************************************/


/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
/* Start Including Header Files */
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif

#include "di_iptuner_config.h"
#include "htype.h"

#include "vkernel.h"
#include "di_uart.h"
#include "taskdef.h"

#include "di_media.h"
#include "di_iptuner_dnp.h"
#include "di_iptuner_stream.h"
#include "di_iptuner_dlna.h"

#ifdef CONFIG_MEDIA_SSTR
#include "di_iptuner_sstr.h" /* for SSTR */
#endif

#include "di_iptuner_hls.h" /* for HLS */

#include "di_iptuner.h"
#include "di_iptuner_http.h"
#include "stream_http.h"
#include "mms_http.h"
#include "util_url.h"
#include "drv_err.h"

#include "di_iptuner_underflow.h"

/* End Including Headers */
/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */
//#define DI_IPTUNER_DEBUG
//#define DI_IPTUNER_CRITICAL_DEBUG

#ifdef IPTUNER_ALL_DEBUG
#ifdef DI_IPTUNER_DEBUG
#define PrintData		DI_UART_Print
#define PrintDebug		DI_UART_Print
#define PrintError		DI_UART_Print
#define PrintEnter		DI_UART_Print("(+)%s\n", __FUNCTION__)
#define PrintExit		DI_UART_Print("(-)%s\n", __FUNCTION__)
#else
#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug		while (0) ((int (*)(char *, ...)) 0)
#define PrintEnter
#define PrintExit
#define PrintError		DI_UART_Print
#endif
#else
#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug		while (0) ((int (*)(char *, ...)) 0)
#define PrintError		while (0) ((int (*)(char *, ...)) 0)
#define PrintEnter
#define PrintExit
#endif


#ifdef DI_IPTUNER_CRITICAL_DEBUG
#define PrintCriticalDebug		DI_UART_Print
#else
#define PrintCriticalDebug		while (0) ((int (*)(char *, ...)) 0)
#endif

#define UNUSED_PARAM(x)  ((void)x)
/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/

/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/

/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
static DI_IPTUNER_t stIPTuner [DI_IPTUNER_MAX_NUM];
DI_IPTUNER_CustomCallback_t g_tCustomCallbacks; /* Global Custom Callback */

/************************ external functions *************************/
/* Caution to warning: implicit declaration of function 'DRV_IPTUNER_HTTP_Init' */
extern DRV_Error DRV_IPTUNER_HTTP_Init(void);


#if __________p__________
#endif

/* example url : mms://username:password@xxx.xxx.xxx.xxx:port_number/PublishingPoint1/test.asf */

static DI_IPTUNER_Index_e P_IPTUNER_GetTunerIndex (DI_IPTUNER_DownloadType_e nDownloadType, HINT8 *url)
{
	DI_IPTUNER_Index_e nTunerIndex = DI_IPTUNER_MAX_NUM;
	DI_IPTUNER_ProtocolType_e nProtocolType = DI_IPTUNER_PROTOCOL_UNKNOWN;
	URL_t stTmpUrl;

	PrintEnter;
	/* TODO  : junsy
	 * Protocol Type은 입력되는 URL 을 Parsing 하여 얻는다.
	 * 또한 URL_t 정보를 활용할 방법이 필요함.
	 */
	/** Check URI */
	VK_memset (&stTmpUrl,0,sizeof(URL_t));

	if(nDownloadType != DI_IPTUNER_DLNA)
	{
		UTIL_UrlParse (&stTmpUrl, (const char*)url, 0);

		if (!VK_strncmp (stTmpUrl.psz_protocol,"http",4))
		{
			nProtocolType = DI_IPTUNER_PROTOCOL_HTTP;
			/* for SSTR */
			if(stTmpUrl.psz_path != NULL)
			{
				if(NULL != VK_strstr(stTmpUrl.psz_path,".ism/Manifest") || NULL != VK_strstr(stTmpUrl.psz_path,".ism/manifest")
				|| NULL != VK_strstr(stTmpUrl.psz_path,".isml/Manifest") || NULL != VK_strstr(stTmpUrl.psz_path,".isml/manifest")
				|| NULL != VK_strstr(stTmpUrl.psz_path,".smil/Manifest") || NULL != VK_strstr(stTmpUrl.psz_path,".smil/manifest"))
				{
					nProtocolType = DI_IPTUNER_PROTOCOL_SSTR;
				}
				if(NULL != VK_strstr(stTmpUrl.psz_path,".m3u8"))
				{
					nProtocolType = DI_IPTUNER_PROTOCOL_HLS;
				}
			}
		}
		else if (!VK_strcmp(stTmpUrl.psz_protocol,"mms"))
		{
			nProtocolType = DI_IPTUNER_PROTOCOL_MMS;
		}
		else if (!VK_strcmp(stTmpUrl.psz_protocol,"rtsp"))
		{
			nProtocolType = DI_IPTUNER_PROTOCOL_RTSP;
		}
		else if (!VK_strcmp(stTmpUrl.psz_protocol,"ftp"))
		{
			nProtocolType = DI_IPTUNER_PROTOCOL_FTP;
		}
		UTIL_UrlClean (&stTmpUrl);
	}

	switch (nDownloadType)
	{
		case DI_IPTUNER_FULL_DOWNLOAD :
			PrintError ("[%s] [%d] %d FULL Download support in the future.\n",__FUNCTION__,__LINE__, nDownloadType);
			break;
		case DI_IPTUNER_PLAYABLE_DOWNLOAD:
			switch ( nProtocolType )
			{
				case DI_IPTUNER_PROTOCOL_HTTP :
					nTunerIndex = DI_IPTUNER_DNP_HTTP;
					break;
				case DI_IPTUNER_PROTOCOL_MMS :
					nTunerIndex = DI_IPTUNER_DNP_MMS;
					break;
				case DI_IPTUNER_PROTOCOL_RTSP :
					nTunerIndex = DI_IPTUNER_DNP_RTSP;
					break;
				case DI_IPTUNER_PROTOCOL_FTP :
					nTunerIndex = DI_IPTUNER_DNP_FTP;
					break ;
				case DI_IPTUNER_PROTOCOL_UNKNOWN :
					PrintError (" %s (%d) Error> Invalid Protocol Type\n",__FUNCTION__,__LINE__ );
					break;
				default :
					PrintError (" %s (%d) Error> %d Protocol Type is not supported(DNP)\n",__FUNCTION__,__LINE__, nProtocolType );
					break;
			}
			break;
		case DI_IPTUNER_STREAMING :
			switch ( nProtocolType )
			{
				case DI_IPTUNER_PROTOCOL_HTTP :
					nTunerIndex = DI_IPTUNER_STREAM_HTTP;
					break;
				case DI_IPTUNER_PROTOCOL_MMS :
					nTunerIndex = DI_IPTUNER_STREAM_MMS;
					break;
				case DI_IPTUNER_PROTOCOL_RTSP :
					nTunerIndex = DI_IPTUNER_STREAM_RTSP;
					break ;
#ifdef CONFIG_MEDIA_SSTR
				case DI_IPTUNER_PROTOCOL_SSTR : /* for SSTR */
					nTunerIndex = DI_IPTUNER_STREAM_SSTR;
					break ;
#endif
				case DI_IPTUNER_PROTOCOL_HLS :
					nTunerIndex = DI_IPTUNER_STREAM_HLS;
					break ;
				case DI_IPTUNER_PROTOCOL_HTTP_CHNK : /* for HTTP Chunked Live Streaming */
					nTunerIndex = DI_IPTUNER_STREAM_HTTP_CHNK;
					break;
				case DI_IPTUNER_PROTOCOL_UNKNOWN :
					PrintError (" %s (%d) Error> Invalid Protocol Type\n",__FUNCTION__,__LINE__ );
					break;
				default :
					PrintError (" %s (%d) Error> %d Protocol Type is not supported (STREAMING)\n",__FUNCTION__,__LINE__, nProtocolType );
					break;
			}
			break;
		case DI_IPTUNER_DLNA:
			nTunerIndex = DI_IPTUNER_DLNA_DNP;
			break;
		default :
			PrintError ("%s (%d) Error> Download Type(%d) is not supported(IPTUNER)\n",__FUNCTION__,__LINE__, nDownloadType );
			break;
	} /* End of switch */

	PrintExit;
	return nTunerIndex ;
}


#if __________task__________
#endif

#if __________drv__________
#endif

#if __________di__________
#endif

/**
 * @brief IPTUNER 를 초기화한다.
 * @param 	None
 * @return 	HAAL Error Code
 * @date 	2009/05/24
 * @author 	junsy@humaxdigital.com
 * @remark 	IPTUNER Index별로 DI_IPTUNER_t 정보를 저장할 배열을 초기화한다.
 * @see 	DI_IPTUNER_t
 */

DI_ERR_CODE DI_IPTUNER_Init (void)
{
	VK_memset (stIPTuner,0,sizeof(DI_IPTUNER_t)*(DI_IPTUNER_MAX_NUM) );
	VK_memset (&g_tCustomCallbacks, 0x00, sizeof(DI_IPTUNER_CustomCallback_t));
	DRV_IPTUNER_HTTP_Init();
	/* TODO : do not action */
	return DI_ERR_OK;
}

/**
 * Open IP Tuner
 *
 * @brief 		IPTUNER 를 초기화한다.
 * @param[in] 	Download Type, Protocol Type, URL String
 * @param[out] 	Media Information
 * @return 		IPTUNER Index (IPTUNER Handler)
 * @date 		2009/05/24
 * @author 		junsy@humaxdigital.com
 * @remark 		지정된 Input value에 따른 IPTUNER를 생성하며
 * 				Index를 할당하여 상위단에 IPTUNER Identifier를 알려준다.
 * 				Open된 이후 IPTUNER의 Control은 Index Number를 통해 수행하며
 * 				Open시 제공되는 Media-Information은 Media Player의 구동시 사용가능하다.
 * @see 		DI_IPTUNER_Close
 */
DI_IPTUNER_Index_e  DI_IPTUNER_Open ( DI_IPTUNER_DownloadType_e nDownloadType, HINT8 *url, DI_MEDIA_INFO_t *pMfpInfo )
{
	DI_ERR_CODE nRet = DI_ERR_ERROR;
	DI_IPTUNER_Index_e nTunerIndex=DI_IPTUNER_MAX_NUM;

	PrintEnter;
	/* Check Tuner Resources */
	if( (nDownloadType != DI_IPTUNER_FULL_DOWNLOAD)
			&&  (nDownloadType != DI_IPTUNER_STREAMING)
			&&  (nDownloadType != DI_IPTUNER_PLAYABLE_DOWNLOAD)
			&&  (nDownloadType != DI_IPTUNER_DLNA) )
	{
		PrintError ("%s(%d) : Invalid Download Type %d.\n",__FUNCTION__,__LINE__,nDownloadType);
		return nTunerIndex;
	}

	if(NULL==url)
	{
		PrintError ("%s(%d) : Invalid URL Parameter.\n",__FUNCTION__,__LINE__);
		return nTunerIndex;
	}

	PrintDebug ("%s(%d) Open Url %s\n",__FUNCTION__,__LINE__, url);
	PrintDebug ("%s(%d) nDownloadType :0x%X\n",__FUNCTION__,__LINE__, nDownloadType);

	/* TODO : by junsy
	 * must be consider flexibility (unlimited nTunerIndex) in point of view large resource.
	 * Using Linked List? */
	nTunerIndex = P_IPTUNER_GetTunerIndex (nDownloadType, url);
	if ( DI_IPTUNER_MAX_NUM == nTunerIndex )
	{
		PrintError ("%s(%d) : Error> MAX Tuner number is %d.\n",__FUNCTION__,__LINE__,DI_IPTUNER_MAX_NUM);
		return nTunerIndex;
	}

	if ((pMfpInfo != NULL) && (pMfpInfo->isHttpChunked == TRUE))
	{
		/* Chante tunerIndex if HTTP chunked live streaming */
		nTunerIndex = DI_IPTUNER_STREAM_HTTP_CHNK;
		PrintError ("%s(%d) : Changed Tuner type:%d.\n",__FUNCTION__,__LINE__,nTunerIndex);
	}

	switch ( nTunerIndex )
	{
		case DI_IPTUNER_DNP_HTTP :
		case DI_IPTUNER_DNP_MMS 	:
			nRet = DI_IPTUNER_DnpOpen (nTunerIndex, url, pMfpInfo);
			if ( DI_ERR_OK != nRet)
			{
				PrintError ("%s(%d) Error> DNP Tuner Open Error.\n",__FUNCTION__,__LINE__);
				return DI_IPTUNER_MAX_NUM;
			}
			break;
		case DI_IPTUNER_STREAM_MMS 	:
		case DI_IPTUNER_STREAM_HTTP :
#ifdef CONFIG_MEDIA_SSTR
		case DI_IPTUNER_STREAM_SSTR :
#endif
		case DI_IPTUNER_STREAM_HLS :
		case DI_IPTUNER_STREAM_HTTP_CHNK :
			nRet = DI_IPTUNER_StreamOpen (nTunerIndex, url, pMfpInfo);
			if ( DI_ERR_OK != nRet)
			{
				PrintError ("%s(%d) Error> STREAM Tuner Open Error.\n",__FUNCTION__,__LINE__);
				return DI_IPTUNER_MAX_NUM;
			}
			break;
		case DI_IPTUNER_DLNA_DNP:
			nRet = DI_IPTUNER_OpenDLNA(nTunerIndex, url, pMfpInfo);
			if ( DI_ERR_OK != nRet)
			{
				PrintError ("%s(%d) Error> DLNA Tuner Open Error.\n",__FUNCTION__,__LINE__);
				return DI_IPTUNER_MAX_NUM;
			}
			break;
		case DI_IPTUNER_STREAM_RTSP 	:
		case DI_IPTUNER_DNP_RTSP 	:
		case DI_IPTUNER_DNP_FTP 	:
		default  :
			PrintError ("[%s] [%d] nTunerIndex %d is not supported\n",__FUNCTION__,__LINE__, nTunerIndex);
			break;
	}

	/** store local */
	if( nRet == DI_ERR_OK)
	{
		PrintDebug ("%s(%d) Create Static IPTUNER Entry. index(%d) \n",__FUNCTION__,__LINE__, nTunerIndex);
		if(stIPTuner[nTunerIndex].url != NULL)
		{
			DD_MEM_Free (stIPTuner[nTunerIndex].url);
			stIPTuner[nTunerIndex].url = NULL;
		}
		/* else { continue } */

		/* update static information */
		stIPTuner[nTunerIndex].isAvailable = TRUE;
		stIPTuner[nTunerIndex].url = (HINT8*) strdup ((const char*)url);
		stIPTuner[nTunerIndex].downType = nDownloadType;
		stIPTuner[nTunerIndex].mediaInfo = *pMfpInfo;
	}

	PrintExit;

	return nTunerIndex;
}

/**
 * Close IP Tuner
 *
 * @brief 		IPTUNER Resource를 해제한다.
 * @param[in] 	IPTUNER Index (IPTUNER Handler)
 * @param[out] 	none
 * @return 		DI_ERR_CODE
 * @date 		2009/05/24
 * @author 		junsy@humaxdigital.com
 * @remark 		IPTUNER Index 에 해당하는 IPTUNER 를 Close한다.
 * @see 		DI_IPTUNER_Open
 */
DI_ERR_CODE DI_IPTUNER_Close (DI_IPTUNER_Index_e nTunerIndex)
{
	DI_ERR_CODE nRet = DI_ERR_ERROR;

	PrintEnter;

	switch ( nTunerIndex )
	{
		case DI_IPTUNER_DNP_HTTP :
		case DI_IPTUNER_DNP_MMS 	:
			nRet = DI_IPTUNER_DnpClose (nTunerIndex);
			if ( DI_ERR_OK != nRet)
			{
				PrintError ("%s(%d) Error> DNP Tuner Close Error.\n",__FUNCTION__,__LINE__);
				return nRet;
			}
			break;
		case DI_IPTUNER_STREAM_HTTP 	:
		case DI_IPTUNER_STREAM_MMS 	:
#ifdef CONFIG_MEDIA_SSTR
		case DI_IPTUNER_STREAM_SSTR :
#endif
		case DI_IPTUNER_STREAM_HLS :
		case DI_IPTUNER_STREAM_HTTP_CHNK :
			nRet = DI_IPTUNER_StreamClose (nTunerIndex);
			if ( DI_ERR_OK != nRet)
			{
				PrintError ("%s(%d) Error> STREAM Tuner Close Error.\n",__FUNCTION__,__LINE__);
				return nRet;
			}
			break;
		case DI_IPTUNER_DLNA_DNP:
			nRet = DI_IPTUNER_CloseDLNA(nTunerIndex);
			if ( DI_ERR_OK != nRet)
			{
				PrintError ("%s(%d) Error> DLNA Tuner Close Error.\n",__FUNCTION__,__LINE__);
				return nRet;
			}
			break;
		case DI_IPTUNER_DNP_RTSP 	:
		case DI_IPTUNER_DNP_FTP 	:
		case DI_IPTUNER_STREAM_RTSP 	:
		default  :
			PrintError ("%s(%d) Error> nTunerIndex %d is not supported\n",__FUNCTION__,__LINE__, nTunerIndex);
			break;
	}

	/** delete tuner info entry */
	if( nRet == DI_ERR_OK)
	{
		/* else { continue } */

		/* update static information */
		if(stIPTuner[nTunerIndex].url != NULL)
		{
			VK_MEM_Free (stIPTuner[nTunerIndex].url);
			stIPTuner[nTunerIndex].url = NULL;
		}
		/* else { continue } */

		VK_memset (&stIPTuner[nTunerIndex],0,sizeof(DI_IPTUNER_t));

		PrintDebug ("%s(%d) Clean Static IPTUNER Entry. index(%d) \n",__FUNCTION__,__LINE__, nTunerIndex);
	}

	PrintExit;

	return DI_ERR_OK;
}

/**
 * Start Download
 *
 * @brief 		Open된 IPTUNER 의 Data 수신을 시작한다.
 * @param[in] 	IPTUNER Index (IPTUNER Handler)
 * @param[out] 	none
 * @return 		DI_ERR_CODE
 * @date 		2009/05/24
 * @author 		junsy@humaxdigital.com
 * @remark 		Open된 IPTUNER 의 Start Function을 통해 Stream & File Download 를  시작한다.
 * @see 		DI_IPTUNER_Stop
 */
DI_ERR_CODE DI_IPTUNER_Start (DI_IPTUNER_Index_e nTunerIndex)
{
	DI_ERR_CODE nRet = DI_ERR_OK;

	PrintEnter;
	switch ( nTunerIndex )
	{
		case DI_IPTUNER_DNP_HTTP :
		case DI_IPTUNER_DNP_MMS :
			nRet = DI_IPTUNER_DnpStart (nTunerIndex);
			if ( DI_ERR_OK != nRet)
			{
				PrintError ("%s(%d) Error> DNP Tuner Close Error.\n",__FUNCTION__,__LINE__);
				return nRet;
			}
			break;
		case DI_IPTUNER_STREAM_HTTP :
		case DI_IPTUNER_STREAM_MMS 	:
#if 0
			nRet = DI_IPTUNER_StreamStart (nTunerIndex);
			if ( DI_ERR_OK != nRet)
			{
				PrintError ("%s(%d) Error> STREAM Tuner Close Error.\n",__FUNCTION__,__LINE__);
				return nRet;
			}
			break;
#endif
		case DI_IPTUNER_DNP_RTSP 	:
		case DI_IPTUNER_DNP_FTP 	:
		case DI_IPTUNER_STREAM_RTSP 	:
		default  :
			PrintError ("[%s] [%d] nTunerIndex %d is not supported\n",__FUNCTION__,__LINE__, nTunerIndex);
			break;
	}
	PrintExit;
	return DI_ERR_OK;
}

/**
 * Stop Download
 *
 * @brief 		Open된 IPTUNER 의 Data 수신을 정지한다.
 * @param[in] 	IPTUNER Index (IPTUNER Handler)
 * @param[out] 	none
 * @return 		DI_ERR_CODE
 * @date 		2009/05/24
 * @author 		junsy@humaxdigital.com
 * @remark 		Open된 IPTUNER 의 Stop Function을 통해 Stream & File Download 를  정지한다.
 * 				Download Task를 정지하며 추후 DI_IPTUNER_Start를 통해 재시작 가능하다.
 * @see 		DI_IPTUNER_Start
 */
DI_ERR_CODE DI_IPTUNER_Stop (DI_IPTUNER_Index_e nTunerIndex)
{
	DI_ERR_CODE nRet = DI_ERR_OK;

	PrintEnter;
	switch ( nTunerIndex )
	{
		case DI_IPTUNER_DNP_HTTP 	:
		case DI_IPTUNER_DNP_MMS 	:
			nRet = DI_IPTUNER_DnpStop (nTunerIndex);
			if ( DI_ERR_OK != nRet)
			{
				PrintError ("%s(%d) Error> DNP Tuner Stop Error.\n",__FUNCTION__,__LINE__);
				return nRet;
			}
			/* else { continue; } */
			break;
		case DI_IPTUNER_STREAM_HTTP 	:
		case DI_IPTUNER_STREAM_MMS 	:
#if 0
			nRet = DI_IPTUNER_StreamStop (nTunerIndex);
			if ( DI_ERR_OK != nRet)
			{
				PrintError ("%s(%d) Error> STREAM Tuner Stop Error.\n",__FUNCTION__,__LINE__);
				return nRet;
			}
			break;
#endif
		case DI_IPTUNER_DNP_RTSP 	:
		case DI_IPTUNER_DNP_FTP 	:
		case DI_IPTUNER_STREAM_RTSP 	:
		default  :
			PrintError ("[%s] [%d] nTunerIndex %d is not supported\n",__FUNCTION__,__LINE__, nTunerIndex);
			break;
	}
	PrintExit;
	return DI_ERR_OK;
}

/**
 * Start Download
 *
 * @brief 		Open된 IPTUNER 의 Data 수신을 시작한다.
 * @param[in] 	IPTUNER Index (IPTUNER Handler)
 * @param[out] 	none
 * @return 		DI_ERR_CODE
 * @date 		2009/05/24
 * @author 		junsy@humaxdigital.com
 * @remark 		Open된 IPTUNER 의 Start Function을 통해 Stream & File Download 를  시작한다.
 * @see 		DI_IPTUNER_Stop
 */
DI_ERR_CODE DI_IPTUNER_DnpPauseReceive (DI_IPTUNER_Index_e nTunerIndex)
{
	DI_ERR_CODE nRet = DI_ERR_OK;

	PrintEnter;
	switch ( nTunerIndex )
	{
		case DI_IPTUNER_DNP_HTTP :
		case DI_IPTUNER_DNP_MMS :
			nRet = DI_IPTUNER_DnpPause (nTunerIndex);
			if ( DI_ERR_OK != nRet)
			{
				PrintError ("%s(%d) Error> DNP Tuner Close Error.\n",__FUNCTION__,__LINE__);
				return nRet;
			}
			break;
		case DI_IPTUNER_STREAM_HTTP :
		case DI_IPTUNER_STREAM_MMS 	:
		case DI_IPTUNER_DNP_RTSP 	:
		case DI_IPTUNER_DNP_FTP 	:
		case DI_IPTUNER_STREAM_RTSP 	:
		default  :
			PrintError ("[%s] [%d] nTunerIndex %d is not supported\n",__FUNCTION__,__LINE__, nTunerIndex);
			break;
	}
	PrintExit;
	return DI_ERR_OK;
}

/**
 * Stop Download
 *
 * @brief 		Open된 IPTUNER 의 Data 수신을 정지한다.
 * @param[in] 	IPTUNER Index (IPTUNER Handler)
 * @param[out] 	none
 * @return 		DI_ERR_CODE
 * @date 		2009/05/24
 * @author 		junsy@humaxdigital.com
 * @remark 		Open된 IPTUNER 의 Stop Function을 통해 Stream & File Download 를  정지한다.
 * 				Download Task를 정지하며 추후 DI_IPTUNER_Start를 통해 재시작 가능하다.
 * @see 		DI_IPTUNER_Start
 */
DI_ERR_CODE DI_IPTUNER_DnpResumeReceive (DI_IPTUNER_Index_e nTunerIndex)
{
	DI_ERR_CODE nRet = DI_ERR_OK;

	PrintEnter;
	switch ( nTunerIndex )
	{
		case DI_IPTUNER_DNP_HTTP 	:
		case DI_IPTUNER_DNP_MMS 	:
			nRet = DI_IPTUNER_DnpResume (nTunerIndex);
			if ( DI_ERR_OK != nRet)
			{
				PrintError ("%s(%d) Error> DNP Tuner Stop Error.\n",__FUNCTION__,__LINE__);
				return nRet;
			}
			/* else { continue; } */
			break;
		case DI_IPTUNER_STREAM_HTTP 	:
		case DI_IPTUNER_STREAM_MMS 	:
		case DI_IPTUNER_DNP_RTSP 	:
		case DI_IPTUNER_DNP_FTP 	:
		case DI_IPTUNER_STREAM_RTSP 	:
		default  :
			PrintError ("[%s] [%d] nTunerIndex %d is not supported\n",__FUNCTION__,__LINE__, nTunerIndex);
			break;
	}
	PrintExit;
	return DI_ERR_OK;
}

/**
 * Control IP Tuner
 *
 * @brief 		IPTUNER 의 세부동작을 제어한다.
 * @param[in] 	IPTUNER Index (IPTUNER Handler)
 * @param[out] 	none
 * @return 		DI_ERR_CODE
 * @date 		2009/05/24
 * @author 		junsy@humaxdigital.com
 * @remark 		IP Tuner Index를 통해 Tuner의 세부적인 기능을 제어하며
 * 				Remote Trick, Pause, Resume 등의 기능을 Tuner Type에 따라 제어한다.
 * @see
 */
DI_ERR_CODE DI_IPTUNER_Control (DI_IPTUNER_Index_e nTunerIndex, DI_IPTUNER_CMD_e cmd, HUINT64 val, HUINT64 val2)
{
	DI_ERR_CODE nRet = DI_ERR_OK;

	PrintEnter;
	switch ( nTunerIndex )
	{
		case DI_IPTUNER_DNP_HTTP :
		case DI_IPTUNER_DNP_MMS 	:
			nRet = DI_IPTUNER_DnpControl (nTunerIndex, cmd, val);
			if ( DI_ERR_OK != nRet)
			{
				PrintError ("%s(%d) Error> DNP Tuner Control Error.\n",__FUNCTION__,__LINE__);
				return nRet;
			}
			break;
		case DI_IPTUNER_STREAM_HTTP 	:
		case DI_IPTUNER_STREAM_MMS 	:
#ifdef CONFIG_MEDIA_SSTR
		case DI_IPTUNER_STREAM_SSTR : /* for SSTR */
#endif
		case DI_IPTUNER_STREAM_HLS :
			nRet = DI_IPTUNER_StreamControl (nTunerIndex, cmd, val, val2);
			if ( DI_ERR_OK != nRet)
			{
				PrintError ("%s(%d) Error> STREAM Tuner Control Error.\n",__FUNCTION__,__LINE__);
				return nRet;
			}
			break;
		case DI_IPTUNER_DNP_RTSP 	:
		case DI_IPTUNER_DNP_FTP 	:
		case DI_IPTUNER_STREAM_RTSP 	:
		case DI_IPTUNER_STREAM_HTTP_CHNK	: /* HTTP Chunked LiveStreaming can't support contorol */
		default  :
			PrintError ("[%s] [%d] nTunerIndex %d is not supported\n",__FUNCTION__,__LINE__, nTunerIndex);
			break;
	}
	PrintExit;
	return DI_ERR_OK;
}

DI_ERR_CODE DI_IPTUNER_SetAdaptiveProperty(DI_IPTUNER_Index_e nTunerIndex, DI_IPTUNER_ADAPTIVE_PROPERTY_e eCmd, DI_IPTUNER_ADAPTIVE_PROPERTY_DATA_t *pData)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	PrintEnter;
	switch ( nTunerIndex )
	{

#ifdef CONFIG_MEDIA_SSTR
		case DI_IPTUNER_STREAM_SSTR : /* for SSTR */
			nRet = DI_STREAM_SSTR_SetProperty(eCmd,pData);
			break;
#endif
		case DI_IPTUNER_STREAM_HLS :
		default  :
			UNUSED_PARAM(eCmd);
			UNUSED_PARAM(pData);
			PrintError ("[%s] [%d] nTunerIndex %d is not supported\n",__FUNCTION__,__LINE__, nTunerIndex);
			break;
	}
	PrintExit;
	return nRet;
}

DI_ERR_CODE DI_IPTUNER_GetAdaptiveProperty(DI_IPTUNER_Index_e nTunerIndex, DI_IPTUNER_ADAPTIVE_PROPERTY_e eCmd, DI_IPTUNER_ADAPTIVE_PROPERTY_DATA_t *pData)
{
	DI_ERR_CODE nRet = DI_ERR_OK;

	PrintEnter;
	switch ( nTunerIndex )
	{

#ifdef CONFIG_MEDIA_SSTR
		case DI_IPTUNER_STREAM_SSTR : /* for SSTR */
			nRet = DI_STREAM_SSTR_GetProperty(eCmd,pData);
			break;
#endif
		case DI_IPTUNER_STREAM_HLS :
		default  :
			UNUSED_PARAM(eCmd);
			UNUSED_PARAM(pData);
			PrintError ("[%s] [%d] nTunerIndex %d is not supported\n",__FUNCTION__,__LINE__, nTunerIndex);
			break;
	}
	PrintExit;
	return nRet;
}


/**
 * Get IP Tuner Information
 *
 * @brief 		IPTUNER 의 세부동작을 제어한다.
 * @param[in]
 * @param[out]
 * @return 		DI_ERR_CODE
 * @date 		2009/05/24
 * @author 		junsy@humaxdigital.com
 * @remark
 * @see
 */


DI_ERR_CODE DI_IPTUNER_GetInfo (DI_IPTUNER_Index_e nTunerIndex, DI_IPTUNER_Info_t *pInfo )
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	PrintEnter;

	switch ( nTunerIndex )
	{
		case DI_IPTUNER_DNP_HTTP 	:
		case DI_IPTUNER_DNP_MMS 	:
			/* specific information */
			nRet = DI_IPTUNER_DnpGetInfo (nTunerIndex,pInfo);
			if ( DI_ERR_OK != nRet)
			{
				PrintError ("%s(%d) Error> DNP Tuner GetInfo Error.\n",__FUNCTION__,__LINE__);
				return nRet;
			}
			/** tuner information */
			pInfo->ptIPTuner = &stIPTuner[nTunerIndex];

			break;
		case DI_IPTUNER_STREAM_HTTP 	:
		case DI_IPTUNER_STREAM_HTTP_CHNK	:
		case DI_IPTUNER_STREAM_MMS 	:
		case DI_IPTUNER_STREAM_SSTR	:
		case DI_IPTUNER_STREAM_HLS	:
			/* specific information */
			nRet = DI_IPTUNER_StreamGetInfo (nTunerIndex,pInfo);
			if ( DI_ERR_OK != nRet)
			{
				PrintError ("%s(%d) Error> STREAM Tuner GetInfo Error.\n",__FUNCTION__,__LINE__);
				return nRet;
			}
			/** tuner information */
			pInfo->ptIPTuner = &stIPTuner[nTunerIndex];
			break;
		case DI_IPTUNER_DNP_RTSP 	:
		case DI_IPTUNER_DNP_FTP 	:
		case DI_IPTUNER_STREAM_RTSP 	:
		default  :
			PrintError ("[%s] [%d] nTunerIndex %d is not supported\n",__FUNCTION__,__LINE__, nTunerIndex);
			break;
	}

	PrintExit;
	return DI_ERR_OK;
}

DI_ERR_CODE DI_IPTUNER_InstallReadyStateCallback(HUINT32 ulDeviceId, fn_readystate_callback pFn)
{
#if defined(USE_HDD_SMART) && !defined(USE_ESATA)
	DI_IPTUNER_DnpSetReadyStateCallback(ulDeviceId, pFn);
#endif
#ifdef CONFIG_HLS_UNDERFLOW_MONITORING_FOR_BBC_CATAL
	DI_IPTUNER_SetUnderflowHlsReadyStateCallback(ulDeviceId, pFn);
#endif
	return DI_IPTUNER_SetUnderflowReadyStateCallback(ulDeviceId, pFn);
}

#ifdef CONFIG_HLS_UNDERFLOW_MONITORING_FOR_BBC_CATAL
DI_ERR_CODE DI_IPTUNER_InstallHlsReadyStateCallback(HUINT32 ulDeviceId, fn_readystate_callback pFn)
{
	return DI_IPTUNER_SetUnderflowHlsReadyStateCallback(ulDeviceId, pFn);
}
#endif

/**
 * @brief 	DLNA File Function을 install한다.
 * @param 	stDLNAFileFn	DLNA File Function List
 * @return 	DI_ERR_CODE
 * @date 	2009/07/15
 * @author 	kimdh1@humaxdigital.com
 * @remark
 * @see
 */
DI_ERR_CODE DI_IPTUNER_InstallDLNAFunction (DLNA_FileFunction_t stDLNAFileFn)
{
	DI_ERR_CODE nRet = DI_ERR_OK;

	PrintEnter;

	nRet = DI_IPTUNER_DLNA_InstallDLNAFunction(stDLNAFileFn);
	if ( DI_ERR_OK != nRet)
	{
		PrintError ("%s(%d) Error> Fail DI_IPTUNER_DLNA_InstallDLNAFunction.\n",__FUNCTION__,__LINE__);
		return nRet;
	}

	PrintExit;

	return DI_ERR_OK;
}

DI_ERR_CODE DI_IPTUNER_ICY_RegisterEventCallback (DI_IPTUNER_Index_e nTunerIndex, DI_IPTUNER_EventCallback_t *pCallback)
{
	DI_ERR_CODE nRet = DI_ERR_OK;

	PrintEnter;

#ifdef SUPPORT_TITLE
	switch ( nTunerIndex )
	{
		case DI_IPTUNER_STREAM_HTTP :
		case DI_IPTUNER_STREAM_MMS 	:
			nRet = DI_IPTUNER_ICY_SreamRegisterEventCallback (nTunerIndex, pCallback);
			if ( DI_ERR_OK != nRet)
			{
				PrintError ("%s(%d) Error> STREAM Tuner GetInfo Error.\n",__FUNCTION__,__LINE__);
				return nRet;
			}
			break;
		case DI_IPTUNER_DNP_HTTP 	:
		case DI_IPTUNER_DNP_MMS 	:
		case DI_IPTUNER_DNP_RTSP 	:
		case DI_IPTUNER_DNP_FTP 	:
		case DI_IPTUNER_STREAM_RTSP 	:
		case DI_IPTUNER_STREAM_HTTP_CHNK :
		default  :
			PrintError ("[%s] [%d] nTunerIndex %d is not supported\n",__FUNCTION__,__LINE__, nTunerIndex);
			break;
	}
#endif

	PrintExit;
	return DI_ERR_OK;
}

void DI_IPTUNER_ICY_UnregisterEventCallback (DI_IPTUNER_Index_e nTunerIndex)
{
	PrintEnter;

#ifdef SUPPORT_TITLE
	switch ( nTunerIndex )
	{
		case DI_IPTUNER_STREAM_HTTP :
		case DI_IPTUNER_STREAM_MMS 	:
			DI_IPTUNER_ICY_StreamUnregisterEventCallback ();
			break;
		case DI_IPTUNER_DNP_HTTP 	:
		case DI_IPTUNER_DNP_MMS 	:
		case DI_IPTUNER_DNP_RTSP 	:
		case DI_IPTUNER_DNP_FTP 	:
		case DI_IPTUNER_STREAM_RTSP 	:
		default  :
			PrintError ("[%s] [%d] nTunerIndex %d is not supported\n",__FUNCTION__,__LINE__, nTunerIndex);
			break;
	}
#endif

	PrintExit;
	return;
}

DI_ERR_CODE DI_IPTUNER_RegisterCustomCallback ( DI_IPTUNER_CUSTOM_e eType, DI_IPTUNER_CustomCallback_t Callback)
{
	DI_ERR_CODE nRet = DI_ERR_ERROR;
	PrintEnter;
	g_tCustomCallbacks.eType = eType;
	switch ( eType )
	{
			case DI_IPTUNER_CUSTOM_VMX :
				g_tCustomCallbacks.cbVmxKeyRetrv = Callback.cbVmxKeyRetrv;
				break;
#ifdef USE_NAGRAPRM
			case DI_IPTUNER_CUSTOM_NAGRAPRM:
				g_tCustomCallbacks.cbNAGRAPRMKeyRetrv = Callback.cbNAGRAPRMKeyRetrv;
				break;
#endif
			default  :
				PrintError ("[%s] [%d] Type : %d is not supported\n",__FUNCTION__,__LINE__, eType);
				g_tCustomCallbacks.eType = DI_IPTUNER_CUSTOM_NONE;
				PrintExit;
				return DI_ERR_ERROR;
	}
	PrintExit;
	return nRet;
}

void DI_IPTUNER_UnregisterCustomCallback (DI_IPTUNER_CUSTOM_e eType)
{
	PrintEnter;
	switch ( eType )
	{
			case DI_IPTUNER_CUSTOM_VMX :
				g_tCustomCallbacks.cbVmxKeyRetrv = NULL;
				break;
#ifdef USE_NAGRAPRM
			case DI_IPTUNER_CUSTOM_NAGRAPRM:
					g_tCustomCallbacks.cbNAGRAPRMKeyRetrv = NULL;
				break;
#endif
			default  :
				PrintError ("[%s] [%d] Type : %d is not supported\n",__FUNCTION__,__LINE__, eType);
				PrintExit;
				return;
	}

	g_tCustomCallbacks.eType = DI_IPTUNER_CUSTOM_NONE;
	PrintExit;
	return;
}

DI_ERR_CODE DI_IPTUNER_RegisterBufferingCallback (DI_IPTUNER_Index_e nTunerIndex, DI_IPTUNER_PlayCallback_t *pCallback)
{
	DI_ERR_CODE nRet = DI_ERR_OK;

	PrintEnter;

	switch ( nTunerIndex )
	{
#if STREAM_BUFFERING
		case DI_IPTUNER_STREAM_HTTP :
		case DI_IPTUNER_STREAM_MMS 	:
			nRet = DI_IPTUNER_StreamRegisterCallback (nTunerIndex, pCallback);
			if ( DI_ERR_OK != nRet)
			{
				PrintError ("%s(%d) Error> STREAM Tuner GetInfo Error.\n",__FUNCTION__,__LINE__);
				return nRet;
			}
			break;
#endif
		case DI_IPTUNER_STREAM_HTTP_CHNK :
			/*
			 * This is separated although HTTP and HTTP_CHNK(chunked) also call the same buffering function,
			 * because special treatment is needed because of build flag (HTTP use STREAM_BUFFERING flag)
			 */
			nRet = DI_IPTUNER_StreamRegisterCallback (nTunerIndex, pCallback);
			if ( DI_ERR_OK != nRet)
			{
				PrintError ("%s(%d) Error> STREAM Tuner GetInfo Error.\n",__FUNCTION__,__LINE__);
				return nRet;
			}
			break;
		case DI_IPTUNER_DNP_HTTP 	:
		case DI_IPTUNER_DNP_MMS 	:
		case DI_IPTUNER_DNP_RTSP 	:
		case DI_IPTUNER_DNP_FTP 	:
		case DI_IPTUNER_STREAM_RTSP 	:
		default  :
			/* nRet = DI_ERR_NOT_SUPPORTED..?? */
			PrintError ("[%s] [%d] nTunerIndex %d is not supported\n",__FUNCTION__,__LINE__, nTunerIndex);
			break;
	}

	PrintExit;
	return nRet;
}

void DI_IPTUNER_UnregisterBufferingCallback (DI_IPTUNER_Index_e nTunerIndex)
{
	PrintEnter;

	switch ( nTunerIndex )
	{
#if STREAM_BUFFERING
		case DI_IPTUNER_STREAM_HTTP :
			DI_IPTUNER_StreamUnregisterCallback (nTunerIndex);
			break;
#endif
		case DI_IPTUNER_STREAM_HTTP_CHNK :
			/*
			 * This is separated although HTTP and HTTP_CHNK(chunked) also call the same buffering function,
			 * because special treatment is needed because of build flag (HTTP use STREAM_BUFFERING flag)
			 */
			DI_IPTUNER_StreamUnregisterCallback (nTunerIndex);
			break;
		case DI_IPTUNER_STREAM_MMS 	:
		case DI_IPTUNER_DNP_HTTP 	:
		case DI_IPTUNER_DNP_MMS 	:
		case DI_IPTUNER_DNP_RTSP 	:
		case DI_IPTUNER_DNP_FTP 	:
		case DI_IPTUNER_STREAM_RTSP 	:
		default  :
			PrintError ("[%s] [%d] nTunerIndex %d is not supported\n",__FUNCTION__,__LINE__, nTunerIndex);
			break;
	}

	PrintExit;
	return;
}

DI_ERR_CODE DI_IPTUNER_StreamingMode (DI_IPTUNER_STREAMING_MODE_e *pnStreamingMode)
{
	DI_ERR_CODE nRet = DI_ERR_OK;

	PrintEnter;

	if(NULL==pnStreamingMode)
	{
		PrintError ("%s(%d) : Invalid URL Parameter.\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

#if STREAM_BUFFERING
	*pnStreamingMode = DI_IPTUNER_STREAMING_MODE_RAMBUFFER;
#else
#if defined(USE_HDD_SMART) && !defined(USE_ESATA)
	*pnStreamingMode = DI_IPTUNER_STREAMING_MODE_STORAGE;
#else
	*pnStreamingMode = DI_IPTUNER_STREAMING_MODE_REALTIME;
#endif
#endif

	PrintExit;
	return nRet;
}

DI_ERR_CODE DI_IPTUNER_ICYProbe (const HINT8 *path,  DI_MEDIA_INFO_t *pstMediaInfo, HBOOL *pbIsICY)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	PrintEnter;

	if((NULL==path) || (NULL==pstMediaInfo) || (NULL==pbIsICY))
	{
		PrintError ("%s(%d) : Error> Path or pstMediaInfo or pbIsICY is null.\n",__FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	nRet = DI_STREAM_HTTP_ICYProbe(path, pstMediaInfo, pbIsICY);
	if ( DI_ERR_OK != nRet)
	{
		PrintError ("%s(%d) Error> Fail DI_STREAM_HTTP_ICYProbe.\n",__FUNCTION__,__LINE__);
		return nRet;
	}
	else
	{
		(void) DI_STREAM_HTTP_IcyVfioRegisterCallback ();
	}

	PrintExit;
	return nRet;
}

DI_ERR_CODE DI_IPTUNER_SetUserAgentInfo (HINT8 *pUserAgentInfo)
{
	DI_ERR_CODE nRet = DI_ERR_OK;

	PrintEnter;

	(void)STREAM_HTTP_SetUserAgentInfo(pUserAgentInfo);

	(void)MMSH_SetUserAgentInfo(pUserAgentInfo);

	PrintExit;
	return nRet;
}

HINT32 DI_IPTUNER_GetUnderflowState(void)
{
	return DI_IPTUNER_UnderflowGetReadyState();
}
void DI_IPTUNER_SetUnderflowState(HINT32 eUnderflowState)
{
	DI_IPTUNER_UnderflowSetReadyState(eUnderflowState);
}

DI_ERR_CODE DI_IPTUNER_GetTimeBySegment(DI_IPTUNER_Index_e nTunerIndex,const HUINT64 pullSegmentId, HUINT64 * ullTime)
{

	DI_ERR_CODE nRet = DI_ERR_ERROR;
#if defined(CONFIG_SES_HDPLUS_SMARTTV)
	if(nTunerIndex == DI_IPTUNER_STREAM_HLS)
	{
		nRet = DI_STREAM_HLS_GetTimeBySegment(pullSegmentId, ullTime);
		if ( DI_ERR_OK != nRet)
		{
			PrintError("[%s:%d:%d] DI_STREAM_HLS_GetTimeBySegment\n", __func__, __LINE__, nRet);
		}
	}
#else
	UNUSED_PARAM(nTunerIndex);
	UNUSED_PARAM(pullSegmentId);
	UNUSED_PARAM(ullTime);
#endif
	return nRet;
}

/*  bitmask of protocol value
	DI_IPTUNER_PROTOCOL_HTTP 		= 0x00000001,
	DI_IPTUNER_PROTOCOL_HTTP_CHNK 	= 0x00000002,
	DI_IPTUNER_PROTOCOL_RTSP 		= 0x00000004,
	DI_IPTUNER_PROTOCOL_MMS 		= 0x00000008,
	DI_IPTUNER_PROTOCOL_FTP		= 0x00000010,
	DI_IPTUNER_PROTOCOL_SMB 		= 0x00000020,
	DI_IPTUNER_PROTOCOL_SSTR 		= 0x00000040,
	DI_IPTUNER_PROTOCOL_HLS 		= 0x00000080,
*/
HUINT32 DI_IPTUNER_GetAvailableProtocol(void)
{
	HINT32 nProtocol = 0;

	nProtocol |= 1 << DI_IPTUNER_PROTOCOL_HTTP;
/* not implemented
	nProtocol |= 1 << DI_IPTUNER_PROTOCOL_HTTP_CHNK;
	nProtocol |= 1 << DI_IPTUNER_PROTOCOL_RTSP;
*/
	nProtocol |= 1 << DI_IPTUNER_PROTOCOL_MMS;
/* not implemented
	nProtocol |= 1 << DI_IPTUNER_PROTOCOL_FTP;
	nProtocol |= 1 << DI_IPTUNER_PROTOCOL_SMB;
*/
	nProtocol |= 1 << DI_IPTUNER_PROTOCOL_SSTR;
/* not implemented
	nProtocol |= 1 << DI_IPTUNER_PROTOCOL_HLS;
*/

	return nProtocol;
}

HULONG DI_IPTUNER_GetStartingTime(DI_IPTUNER_Index_e nTunerIndex)
{
	switch ( nTunerIndex )
	{
#ifdef CONFIG_MEDIA_SSTR
		case DI_IPTUNER_STREAM_SSTR:
			return DI_STREAM_SSTR_TimerGetStartingTime();
#endif
		case DI_IPTUNER_DNP_HTTP :
		case DI_IPTUNER_DNP_MMS:
		case DI_IPTUNER_DNP_RTSP:
		case DI_IPTUNER_DNP_FTP:
		case DI_IPTUNER_DLNA_DNP:
		case DI_IPTUNER_STREAM_HTTP:
		case DI_IPTUNER_STREAM_MMS:
		case DI_IPTUNER_STREAM_RTSP:
		case DI_IPTUNER_STREAM_HLS:
		case DI_IPTUNER_STREAM_HTTP_CHNK:
		default  :
			PrintError("[%s:%d] Not support", __FUNCTION__, __LINE__);
			break;
	}

	return 0;
}

HULONG DI_IPTUNER_GetLoadingTime(DI_IPTUNER_Index_e nTunerIndex)
{
	switch ( nTunerIndex )
	{
#ifdef CONFIG_MEDIA_SSTR
		case DI_IPTUNER_STREAM_SSTR:
			return DI_STREAM_SSTR_TimerGetLoadingTime();
#endif
		case DI_IPTUNER_DNP_HTTP :
		case DI_IPTUNER_DNP_MMS:
		case DI_IPTUNER_DNP_RTSP:
		case DI_IPTUNER_DNP_FTP:
		case DI_IPTUNER_DLNA_DNP:
		case DI_IPTUNER_STREAM_HTTP:
		case DI_IPTUNER_STREAM_MMS:
		case DI_IPTUNER_STREAM_RTSP:
		case DI_IPTUNER_STREAM_HLS:
		case DI_IPTUNER_STREAM_HTTP_CHNK:
		default  :
			PrintError("[%s:%d] Not support", __FUNCTION__, __LINE__);
			break;
	}

	return 0;
}

HUINT32 DI_IPTUNER_GetBandwidth(DI_IPTUNER_Index_e nTunerIndex)
{
	switch ( nTunerIndex )
	{
#ifdef CONFIG_MEDIA_SSTR
		case DI_IPTUNER_STREAM_SSTR:
			return DI_STREAM_SSTR_TimerGetBandwidth();
#endif
		case DI_IPTUNER_DNP_HTTP :
		case DI_IPTUNER_DNP_MMS:
		case DI_IPTUNER_DNP_RTSP:
		case DI_IPTUNER_DNP_FTP:
		case DI_IPTUNER_DLNA_DNP:
		case DI_IPTUNER_STREAM_HTTP:
		case DI_IPTUNER_STREAM_MMS:
		case DI_IPTUNER_STREAM_RTSP:
		case DI_IPTUNER_STREAM_HLS:
		case DI_IPTUNER_STREAM_HTTP_CHNK:
		default  :
			PrintError("[%s:%d] Not support", __FUNCTION__, __LINE__);
			break;
	}

	return 0;
}

HUINT32 DI_IPTUNER_GetMaxBandwidth(DI_IPTUNER_Index_e nTunerIndex)
{
	switch ( nTunerIndex )
	{
#ifdef CONFIG_MEDIA_SSTR
		case DI_IPTUNER_STREAM_SSTR:
			return DI_STREAM_SSTR_TimerGetMaxBandwidth();
#endif
		case DI_IPTUNER_DNP_HTTP :
		case DI_IPTUNER_DNP_MMS:
		case DI_IPTUNER_DNP_RTSP:
		case DI_IPTUNER_DNP_FTP:
		case DI_IPTUNER_DLNA_DNP:
		case DI_IPTUNER_STREAM_HTTP:
		case DI_IPTUNER_STREAM_MMS:
		case DI_IPTUNER_STREAM_RTSP:
		case DI_IPTUNER_STREAM_HLS:
		case DI_IPTUNER_STREAM_HTTP_CHNK:
		default  :
			PrintError("[%s:%d] Not support", __FUNCTION__, __LINE__);
			break;
	}

	return 0;
}

HUINT32 DI_IPTUNER_GetCurBitrate(DI_IPTUNER_Index_e nTunerIndex)
{
	switch ( nTunerIndex )
	{
#ifdef CONFIG_MEDIA_SSTR
		case DI_IPTUNER_STREAM_SSTR:
			return DI_STREAM_SSTR_GetCurBitrate();
#endif
		case DI_IPTUNER_DNP_HTTP :
		case DI_IPTUNER_DNP_MMS:
		case DI_IPTUNER_DNP_RTSP:
		case DI_IPTUNER_DNP_FTP:
		case DI_IPTUNER_DLNA_DNP:
		case DI_IPTUNER_STREAM_HTTP:
		case DI_IPTUNER_STREAM_MMS:
		case DI_IPTUNER_STREAM_RTSP:
		case DI_IPTUNER_STREAM_HLS:
		case DI_IPTUNER_STREAM_HTTP_CHNK:
		default  :
			PrintError("[%s:%d] Not support", __FUNCTION__, __LINE__);
			break;
	}

	return 0;
}

HUINT32 DI_IPTUNER_GetCurFragmentIndex(DI_IPTUNER_Index_e nTunerIndex)
{
	switch ( nTunerIndex )
	{
#ifdef CONFIG_MEDIA_SSTR
		case DI_IPTUNER_STREAM_SSTR:
			return DI_STREAM_SSTR_GetCurFragmentIndex();
#endif
		case DI_IPTUNER_DNP_HTTP :
		case DI_IPTUNER_DNP_MMS:
		case DI_IPTUNER_DNP_RTSP:
		case DI_IPTUNER_DNP_FTP:
		case DI_IPTUNER_DLNA_DNP:
		case DI_IPTUNER_STREAM_HTTP:
		case DI_IPTUNER_STREAM_MMS:
		case DI_IPTUNER_STREAM_RTSP:
		case DI_IPTUNER_STREAM_HLS:
		case DI_IPTUNER_STREAM_HTTP_CHNK:
		default  :
			PrintError("[%s:%d] Not support", __FUNCTION__, __LINE__);
			break;
	}

	return 0;
}

HUINT64 DI_IPTUNER_GetCurFragmentTime(DI_IPTUNER_Index_e nTunerIndex)
{
	switch ( nTunerIndex )
	{
#ifdef CONFIG_MEDIA_SSTR
		case DI_IPTUNER_STREAM_SSTR:
			return DI_STREAM_SSTR_GetCurFragmentTime();
#endif
		case DI_IPTUNER_DNP_HTTP :
		case DI_IPTUNER_DNP_MMS:
		case DI_IPTUNER_DNP_RTSP:
		case DI_IPTUNER_DNP_FTP:
		case DI_IPTUNER_DLNA_DNP:
		case DI_IPTUNER_STREAM_HTTP:
		case DI_IPTUNER_STREAM_MMS:
		case DI_IPTUNER_STREAM_RTSP:
		case DI_IPTUNER_STREAM_HLS:
		case DI_IPTUNER_STREAM_HTTP_CHNK:
		default  :
			PrintError("[%s:%d] Not support", __FUNCTION__, __LINE__);
			break;
	}

	return 0;
}

DI_ERR_CODE DI_IPTUNER_GetCurFragmentUrl(DI_IPTUNER_Index_e nTunerIndex, char** pUrl)
{
	switch ( nTunerIndex )
	{
#ifdef CONFIG_MEDIA_SSTR
		case DI_IPTUNER_STREAM_SSTR:
			return DI_STREAM_SSTR_GetCurFragmentUrl(pUrl);
#endif
		case DI_IPTUNER_DNP_HTTP :
		case DI_IPTUNER_DNP_MMS:
		case DI_IPTUNER_DNP_RTSP:
		case DI_IPTUNER_DNP_FTP:
		case DI_IPTUNER_DLNA_DNP:
		case DI_IPTUNER_STREAM_HTTP:
		case DI_IPTUNER_STREAM_MMS:
		case DI_IPTUNER_STREAM_RTSP:
		case DI_IPTUNER_STREAM_HLS:
		case DI_IPTUNER_STREAM_HTTP_CHNK:
		default  :
			PrintError("[%s:%d] Not support", __FUNCTION__, __LINE__);
			break;
	}

	return DI_ERR_ERROR;
}

/*
*	This function is to return the count of underflow.
*/
HUINT32 DI_IPTUNER_GetBufferingCount(DI_IPTUNER_Index_e nTunerIndex)
{
	switch ( nTunerIndex )
	{
#ifdef CONFIG_MEDIA_SSTR
		case DI_IPTUNER_STREAM_SSTR:
			return DI_STREAM_SSTR_GetBufferingCount();
#endif
		case DI_IPTUNER_DNP_HTTP :
		case DI_IPTUNER_DNP_MMS:
		case DI_IPTUNER_DNP_RTSP:
		case DI_IPTUNER_DNP_FTP:
		case DI_IPTUNER_DLNA_DNP:
		case DI_IPTUNER_STREAM_HTTP:
		case DI_IPTUNER_STREAM_MMS:
		case DI_IPTUNER_STREAM_RTSP:
		case DI_IPTUNER_STREAM_HLS:
		case DI_IPTUNER_STREAM_HTTP_CHNK:
		default  :
			PrintError("[%s:%d] Not support", __FUNCTION__, __LINE__);
			break;
	}

	return DI_ERR_ERROR;
}

/*
*	This function is to return the time while the player status has been set the underflow.
*/
HUINT32 DI_IPTUNER_GetBufferingTime(DI_IPTUNER_Index_e nTunerIndex)
{
	switch ( nTunerIndex )
	{
#ifdef CONFIG_MEDIA_SSTR
		case DI_IPTUNER_STREAM_SSTR:
			return (HUINT32)DI_STREAM_SSTR_GetBufferingTime();
#endif
		case DI_IPTUNER_DNP_HTTP :
		case DI_IPTUNER_DNP_MMS:
		case DI_IPTUNER_DNP_RTSP:
		case DI_IPTUNER_DNP_FTP:
		case DI_IPTUNER_DLNA_DNP:
		case DI_IPTUNER_STREAM_HTTP:
		case DI_IPTUNER_STREAM_MMS:
		case DI_IPTUNER_STREAM_RTSP:
		case DI_IPTUNER_STREAM_HLS:
		case DI_IPTUNER_STREAM_HTTP_CHNK:
		default  :
			PrintError("[%s:%d] Not support", __FUNCTION__, __LINE__);
			break;
	}

	return DI_ERR_ERROR;
}


void DI_IPTUNRER_SetStreamInfo(HUINT32 uDeviceId, DI_VIDEO_FORMAT eVideoFormat, DI_AUDIO_FORMAT eAudioFormat)
{
	DI_STREAM_HLS_SetDecoderInfo(uDeviceId, eVideoFormat, eAudioFormat);
}



