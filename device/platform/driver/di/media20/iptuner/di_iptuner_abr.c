/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   di_iptuner_abr.c  $
 * Version:			$Revision:   $
 * Original Author:	SungYong Jeon
 * Current Author:	$Author: ykkim3@humaxdigital.com $
 * Date:			$Date:   $
 * File Description:	     ABR-MPD Controller for Streaming
 * Module:
 * Remarks:
 */

/*******************************************************************/
/* Copyright (c) 2011 HUMAX Co., Ltd. 								*/
/* All rights reserved.												*/
/*******************************************************************/


/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
/* Start Including Header Files */

#include "di_iptuner_config.h"

#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <inttypes.h>	 /** for 64bit MACRO ex>PRId64 */
#include <sys/time.h> 	 /** for checking delay */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "htype.h"

#include "vkernel.h"
#include "taskdef.h"

#include "di_iptuner20.h"
#include "di_iptuner_http.h"
#include "di_iptuner_abr.h"
#include "abr_queue.h"
#include "abr_mpdcontroller.h"
#include "stream_http.h"
#include "vfio20.h"

/* End Including Headers */
/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */
//#define IPTUNER_ALL_DEBUG
#define DI_IPTUNER_ABR_FAKE_TEST 0

//#define DI_IPTUNER_ABR_DEBUG
//#define FILE_TEST

//#define INTEGRITY_STREAM_CHECK_WITH_COMPARING_LOCAL_FILE 0 /* set 1 if you want to check buffering iptuner with local file */
//#define INTEGRITY_STREAM_CHECK_WITH_COMPARING_LOCAL_FILE_NAME "/fanta1.wmv"

/* DEBUG DEFINE */
#ifdef IPTUNER_ALL_DEBUG
#ifdef DI_IPTUNER_ABR_DEBUG
#define PrintData		DI_UART_Print
#define PrintDebug		DI_UART_Print
#define PrintEnter		DI_UART_Print("(+)%s\n", __FUNCTION__)
#define PrintExit		DI_UART_Print("(-)%s\n", __FUNCTION__)
#define PrintError		DI_UART_Print
#else
#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug		while (0) ((int (*)(char *, ...)) 0)
#define PrintEnter		while (0) ((int (*)(char *, ...)) 0)
#define PrintExit		while (0) ((int (*)(char *, ...)) 0)
#define PrintError		DI_UART_Print
#endif

#else

#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug		while (0) ((int (*)(char *, ...)) 0)
#define PrintError		while (0) ((int (*)(char *, ...)) 0)
#define PrintEnter
#define PrintExit
#endif /* END DEBUG DEFINE */

#define UNUSED_PARAM(x)  ((void)x)

// #define MAX_HTTP_RECV_BUFF_SIZE (2048*3)	/** 6KB */
// #define NEXUS_ALIGN_BUFFER_SIZE (1024*10)
// #define DI_IPTUNER_HTTP_SEEK_RETRY_COUNT (20)

#if 0 /* OLD */
#ifdef SUPPORT_TITLE
#define MAX_TITLE_LENGTH	(256)
#endif
#endif
/* End #define */
/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */
//typedef struct timeval TIMEVAL_t;
/* End typedef */

/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/
/* Start global variable */
/* End global variable */

/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
/* Start static variable */

/* End static variable */
/*******************************************************************/
/************************ static funtions **************************/
/*******************************************************************/
#if __________p__________
#endif

/**
 * @brief       get Manifest data using http
 * @param       pHTuner : IPTuner Handle
 * @param	pMpdBuff : buffer for storing manifest data
 * @return     	size of manifest data
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/

HINT32  P_STREAM_ABR_GetManifest(DI_IPTUNER_t *pTunerHandle, HINT8 **pMpdBuff)
{
	HINT64 	nSumLength=0;
	HINT32	nPos = 0;
	HINT32	nRet = DI_ERR_OK;
	HINT64  nContentLength = 0;
	HINT8 	*pBuffer=NULL;
	HINT32 	nRecvLength=0;
#if ABR_MPD_CONFIG_MEMORY_PARSE

#else
	HINT32 	nRc = -1;
	HINT32	flags = O_CREAT|O_TRUNC|O_WRONLY|O_SYNC;
	HINT32	nFd = -1;
#endif
	HTTP_ACCESS_SYS_t *pstHttpSys=NULL;
	STREAM_ACCESS_t *pAccess=NULL;
	DI_IPTUNER_Msg_t s_Msg;
#if SUPPORT_HUMAX_COOKIE
	char *psCookie = NULL;
	URL_t tUrl;
#endif
	/*******************************************
	 * Open Protocol Controller
	 * 	: After the pVfioHandle created,
	 * 			IPTUNER handle has to link with it
	 *******************************************/
	(void) VK_memset (&s_Msg, 0x0, sizeof(s_Msg));
	if( pTunerHandle->eProtocolType != DI_IPTUNER_PROTOCOL_ABR)
	{
		PrintError ("%s (%d) Error>  Invalid Tuner Type \n",__FUNCTION__,__LINE__);
		nRet = DI_ERR_ERROR;
		/* TODO : Resource check */
		return 0;
	}
	else
	{
		pAccess = (STREAM_ACCESS_t*)DD_MEM_Alloc(sizeof(STREAM_ACCESS_t));
		if(pAccess == NULL)
		{
			PrintError("%s(%d) Error> can't allocate STREAM handler\n",__FUNCTION__,__LINE__);
			return VFIO_ERROR_IN_DEVICE;
		}
		VK_memset(pAccess,0,sizeof(STREAM_ACCESS_t));

#if SUPPORT_HUMAX_COOKIE
		VK_MEM_Memset(&tUrl, 0x0, sizeof(tUrl));
		UTIL_UrlParse(&tUrl, (const char *)pTunerHandle->pUrl, 0);
		if(tUrl.psz_cookie != NULL)
		{
			psCookie = VK_StrDup(tUrl.psz_cookie);
		}
		UTIL_UrlClean(&tUrl);
#endif

		/** psz_path must be free */
		if(pAccess->psz_path != NULL)
		{
			VK_MEM_Free(pAccess->psz_path);
			pAccess->psz_path = NULL;
		}
		pAccess->psz_path = VK_StrDup ( (const char *) UTIL_GetOriginalURL (pTunerHandle->pUrl) );
#if SUPPORT_HUMAX_COOKIE
		if(psCookie != NULL)
		{
			pAccess->psz_cookie = psCookie;
		}
#endif
		if(pAccess->psz_path == NULL)
		{
			PrintError("%s(%d) : Error> Fail Mem alloc.\n",__FUNCTION__,__LINE__);

			if(pAccess->psz_path != NULL)
			{
				DD_MEM_Free(pAccess->psz_path);
				pAccess->psz_path =NULL;
			}
			/* else { continue; } */
#if SUPPORT_HUMAX_COOKIE
			if(pAccess->psz_cookie != NULL)
			{
				DD_MEM_Free(pAccess->psz_cookie);
				pAccess->psz_cookie =NULL;
			}
			/* else { continue; } */
#endif
			PrintExit;
			return VFIO_ERROR_IN_DEVICE;
		}

		/* Try to open the STREAM */
		if (DI_IPT_SUCCESS != STREAM_HTTP_Open (pAccess) )
		{
			if(pAccess->psz_path != NULL)
			{
				DD_MEM_Free(pAccess->psz_path);
				pAccess->psz_path =NULL;
			}
			/* else { continue; } */
#if SUPPORT_HUMAX_COOKIE
			if(pAccess->psz_cookie != NULL)
			{
				DD_MEM_Free(pAccess->psz_cookie);
				pAccess->psz_cookie =NULL;
			}
			/* else { continue; } */
#endif
			if(pAccess->psz_path != NULL)
			{
				DD_MEM_Free  (pAccess->psz_path);
				pAccess->psz_path =NULL;
			}
			/* else { continue; } */

			PrintError("%s (%d) : Error> STREAM_HTTP_Open Fail.\n",__FUNCTION__,__LINE__);
			PrintExit;
			return VFIO_ERROR_IN_DEVICE;
		}
		else
		{
			pstHttpSys = (HTTP_ACCESS_SYS_t *)pAccess->p_sys;
		}
	}
	nContentLength = pAccess->info.i_size;
	PrintDebug ("%s(%d): Open %s\n",__FUNCTION__,__LINE__,pTunerHandle->pUrl);
	PrintDebug ("%s(%d): nContentLength (%lld)bytes \n",__FUNCTION__,__LINE__,nContentLength);
#if ABR_MPD_CONFIG_MEMORY_PARSE
	*pMpdBuff = (HINT8*) DD_MEM_Alloc (nContentLength+ABR_MPD_CONST_PADDING);
	if(*pMpdBuff==NULL)
	{
		PrintError ("%s(%d) Error> Heap Memory Allocation Fail. size (%"PRId64") !!\n",__FUNCTION__,__LINE__,nContentLength+ABR_MPD_CONST_PADDING);
		return 0;
	}
	VK_memset(*pMpdBuff,0x00,nContentLength+ABR_MPD_CONST_PADDING);
#else
	nFd = open (ABR_MPD_CONFIG_TEMP_CACHE, flags, 0666);
	if(nFd <0)
	{
		PrintError ("%s(%d) :  Error> pHTuner->pSaveFileName open fail (HTTP_DNP) Filename: %s\n",__FUNCTION__,__LINE__,pTunerHandle->pSaveFileName);
		return 0;
	}
#endif
	/* heap allocation */
	pBuffer = (HINT8*) DD_MEM_Alloc (ABR_MPD_CONST_HTTP_RECV_BUFFSIZE);
	if(pBuffer==NULL)
	{
		PrintError ("%s(%d) Error> Heap Memory Allocation Fail. size (%d) !!\n",__FUNCTION__,__LINE__,ABR_MPD_CONST_HTTP_RECV_BUFFSIZE);
		return 0;
	}

	do
	{
		nRecvLength = 0;
		nRecvLength = STREAM_HTTP_Read ( pAccess, (HUINT8 *)pBuffer, ABR_MPD_CONST_HTTP_RECV_BUFFSIZE );
		if(nRecvLength <= 0)
		{
			continue;
		}
		/* normal case */
		else
		{
#if ABR_MPD_CONFIG_MEMORY_PARSE
			if(nContentLength < nPos + nRecvLength)
			{
				PrintError ("%s(%d) : Error > BufferOverFlow !!Content : %lld, Pos : %d, RecvLength : %d \n",__FUNCTION__,__LINE__,nContentLength,nPos,nRecvLength);
				VK_MEM_Memcpy(*pMpdBuff+nPos, pBuffer, (nPos+nRecvLength) - nContentLength);
				break;
			}
			else
			{
				VK_MEM_Memcpy(*pMpdBuff+nPos, pBuffer, nRecvLength);
				nPos+=nRecvLength;
				nSumLength+=nRecvLength;
			}
#else
			nRc = write (nFd, pBuffer, nRecvLength);
			nPos+=nRecvLength;
			nSumLength+=nRecvLength;
#endif
			usleep(0);
		}
	}
	while (nSumLength < nContentLength );

	if(pBuffer != NULL)
	{
		VK_MEM_Free(pBuffer);
		pBuffer = NULL;
	}
#if ABR_MPD_CONFIG_MEMORY_PARSE
#else
	close(nFd);
#endif
	PrintDebug ("%s(%d) : Content : %lld, Pos : %d, RecvLength : %d \n",__FUNCTION__,__LINE__,nContentLength,nPos,nSumLength);
	(void) STREAM_HTTP_Close ( pAccess );
	pAccess->b_alive = FALSE;
	if(pAccess->psz_path != NULL)
	{
		DD_MEM_Free  (pAccess->psz_path);
		pAccess->psz_path =NULL;
	}
	/* else { continue; } */
#if SUPPORT_HUMAX_COOKIE
	if(pAccess->psz_cookie != NULL)
	{
		DD_MEM_Free(pAccess->psz_cookie);
		pAccess->psz_cookie =NULL;
	}
	/* else { continue; } */
#endif

	return nContentLength;
}

/**
 * @brief       setting virtual position
 * @param       pTunerHandle : IP Tuner handle
 * @param       nPos : virtual position
 * @return      di err code
 * @date        2011/02/23
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
DI_ERR_CODE P_STREAM_ABR_ChangeUrl(DI_IPTUNER_t *pTunerHandle, HINT8 *pUrl)
{
	HTTP_ACCESS_SYS_t	*pHttpSys = NULL;
	STREAM_ACCESS_t *pAccess = NULL;
	pAccess = (STREAM_ACCESS_t *)pTunerHandle->pHStream;
	if (NULL == pAccess)
	{
		PrintError("%s(%d) Error> Tuner handle doesn't include correct STREAM Handler. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return VFIO_ERROR_FAIL;
	}

	if(pTunerHandle == NULL)
	{
		PrintError("%s(%d) : Error> pTunerHandle is NULL.\n",__FUNCTION__,__LINE__);
		return ABR_FAIL;
	}
	if(pTunerHandle->hAbr == NULL)
	{
		PrintError("%s(%d) : Error> hAbr is NULL.\n",__FUNCTION__,__LINE__);
		return ABR_FAIL;
	}

	pHttpSys = (HTTP_ACCESS_SYS_t*)pAccess->p_sys;

	UTIL_UrlParse ( &pHttpSys->url, (const char *)pUrl, 0 );

	/* close current connection */

	(void) STREAM_HTTP_Close ( pAccess );

	pAccess->b_alive = FALSE;

	if(pAccess->psz_path != NULL)
	{
		DD_MEM_Free  (pAccess->psz_path);
		pAccess->psz_path =NULL;
	}
	/* else { continue; } */

#if SUPPORT_HUMAX_COOKIE
	if(pAccess->psz_cookie != NULL)
	{
		DD_MEM_Free(pAccess->psz_cookie);
		pAccess->psz_cookie =NULL;
	}
	/* else { continue; } */
#endif

	/* open new connection with next segment */

#if SUPPORT_HUMAX_COOKIE
	if(pHttpSys->url.psz_cookie != NULL)
	{
		if(pAccess->psz_cookie != NULL)
		{
			VK_MEM_Free(pAccess->psz_cookie);
			pAccess->psz_cookie = NULL;
		}
		pAccess->psz_cookie = VK_StrDup(pHttpSys->url.psz_cookie);
	}
#endif

	/* open for player */
	/** psz_path must be free */
	if(pAccess->psz_path != NULL)	{
		VK_MEM_Free(pAccess->psz_path);
		pAccess->psz_path = NULL;
	}

	pAccess->psz_path = VK_StrDup ( (const char *) UTIL_GetOriginalURL (pUrl) );

	/* Try to open the STREAM */
	if (DI_IPT_SUCCESS != STREAM_HTTP_Open (pAccess) )
	{
		if(pAccess->psz_path != NULL)
		{
			DD_MEM_Free(pAccess->psz_path);
			pAccess->psz_path =NULL;
		}
		/* else { continue; } */
#if SUPPORT_HUMAX_COOKIE
		if(pAccess->psz_cookie != NULL)
		{
			DD_MEM_Free(pAccess->psz_cookie);
			pAccess->psz_cookie =NULL;
		}
		/* else { continue; } */
#endif
		if(pAccess->psz_path != NULL)
		{
			DD_MEM_Free  (pAccess->psz_path);
			pAccess->psz_path =NULL;
		}
		/* else { continue; } */

		PrintError("%s (%d) : Error> STREAM_HTTP_Open Fail.\n",__FUNCTION__,__LINE__);
		PrintExit;
		return VFIO_ERROR_IN_DEVICE;
	}
	return ABR_OK;
}

/**
 * @brief       setting virtual position
 * @param       pTunerHandle : IP Tuner handle
 * @param       nPos : virtual position
 * @return      di err code
 * @date        2011/02/23
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
DI_ERR_CODE P_STREAM_ABR_SetVCurrentPosition(DI_IPTUNER_t *pTunerHandle, HINT64 nPos)
{

#if 0 /* NOT USE CURRENT POSITION */
	UNUSED_PARAM(pTunerHandle);
	UNUSED_PARAM(nPos);
#else /* USE CURRENT POSITION */
	if(pTunerHandle == NULL)
	{
		PrintError("%s(%d) : Error> pTunerHandle is NULL.\n",__FUNCTION__,__LINE__);
		return ABR_FAIL;
	}
	if(pTunerHandle->hAbr == NULL)
	{
		PrintError("%s(%d) : Error> hAbr is NULL.\n",__FUNCTION__,__LINE__);
		return ABR_FAIL;
	}
	ABR_MPD_SetCurrentPosition(pTunerHandle->hAbr,nPos);
#endif

	return ABR_OK;
}

/**
 * @brief       get virtual position
 * @param       pTunerHandle : IP Tuner handle
 * @return      virtual position
 * @date        2011/02/23
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
HINT64 P_STREAM_ABR_GetVCurrentPosition(DI_IPTUNER_t *pTunerHandle)
{
#if 0 /* NOT USE CURRENT POSITION */
	UNUSED_PARAM(pTunerHandle);
#else /* USE CURRENT POSITION */

	if(pTunerHandle == NULL)
	{
		PrintError("%s(%d) : Error> pTunerHandle is NULL.\n",__FUNCTION__,__LINE__);
		return 0;
	}
	if(pTunerHandle->hAbr == NULL)
	{
		PrintError("%s(%d) : Error> hAbr is NULL.\n",__FUNCTION__,__LINE__);
		return 0;
	}
	return ABR_MPD_GetCurrentPosition(pTunerHandle->hAbr);
#endif
	return 0;
}


/**
 * @brief       setting virtual length of stream
 * @param       pTunerHandle : IP Tuner handle
 * @param       nLength : virtual length of stream
 * @return      di err code
 * @date        2011/02/23
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
DI_ERR_CODE P_STREAM_ABR_SetVStreamLength(DI_IPTUNER_t *pTunerHandle, HINT64 nLength )
{
	if(pTunerHandle == NULL)
	{
		PrintError("%s(%d) : Error> pTunerHandle is NULL.\n",__FUNCTION__,__LINE__);
		return ABR_FAIL;
	}
	if(pTunerHandle->hAbr == NULL)
	{
		PrintError("%s(%d) : Error> hAbr is NULL.\n",__FUNCTION__,__LINE__);
		return ABR_FAIL;
	}
	//pTunerHandle->tABRCtrlInfo.nVirtualStreamLength = nLength;
	ABR_MPD_SetStreamLength(pTunerHandle->hAbr , nLength);
	return ABR_OK;
}


/**
 * @brief       get virtual length of stream
 * @param       pTunerHandle : IP Tuner handle
 * @return      virtual length of stream
 * @date        2011/02/23
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
HINT64 P_STREAM_ABR_GetVStreamLength(DI_IPTUNER_t *pTunerHandle)
{
	if(pTunerHandle == NULL)
	{
		PrintError("%s(%d) : Error> pTunerHandle is NULL.\n",__FUNCTION__,__LINE__);
		return 0;
	}
	if(pTunerHandle->hAbr == NULL)
	{
		PrintError("%s(%d) : Error> hAbr is NULL.\n",__FUNCTION__,__LINE__);
		return 0;
	}
	return ABR_MPD_GetStreamLength(pTunerHandle->hAbr);
//	return pTunerHandle->tABRCtrlInfo.nVirtualStreamLength;
}

/**
 * @brief       setting time for seek
 * @param       pTunerHandle : IP Tuner handle
 * @param       ullTime : time for seek
 * @return      di err code
 * @date        2011/02/23
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
DI_ERR_CODE P_STREAM_ABR_SetTimeForSeek(DI_IPTUNER_t *pTunerHandle, HUINT64 ullTime)
{
	if(pTunerHandle == NULL)
	{
		PrintError("%s(%d) : Error> pTunerHandle is NULL.\n",__FUNCTION__,__LINE__);
		return ABR_FAIL;
	}
	if(pTunerHandle->hAbr == NULL)
	{
		PrintError("%s(%d) : Error> hAbr is NULL.\n",__FUNCTION__,__LINE__);
		return ABR_FAIL;
	}
	ABR_MPD_SetTimeForSeek(pTunerHandle->hAbr, ullTime);
	//pTunerHandle->tABRCtrlInfo.ullTimeForSeek = ullTime;
	return ABR_OK;
}


/**
 * @brief       get time for seek
 * @param       pTunerHandle : IP Tuner handle
 * @return      time for seek
 * @date        2011/02/23
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
HUINT64 P_STREAM_ABR_GetTimeForSeek(DI_IPTUNER_t *pTunerHandle)
{
	if(pTunerHandle == NULL)
	{
		PrintError("%s(%d) : Error> pTunerHandle is NULL.\n",__FUNCTION__,__LINE__);
		return 0;
	}
	if(pTunerHandle->hAbr == NULL)
	{
		PrintError("%s(%d) : Error> hAbr is NULL.\n",__FUNCTION__,__LINE__);
		return 0;
	}
	return ABR_MPD_GetTimeForSeek(pTunerHandle->hAbr);
//	return pTunerHandle->tABRCtrlInfo.ullTimeForSeek;
}

/**
 * @brief       setting next segment
 * @param       tHandle : VFIO handle
 * @return      di err code
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
DI_ERR_CODE P_STREAM_ABR_NextSegment(VFIO_Handle_t tHandle)
{
	ABR_QUEUE_NODE_t stNode;
	HINT32 nRet = DI_ERR_OK;
	HUINT64 ullBandwidth = 0;
	DI_IPTUNER_t *pTunerHandle = NULL;
	HINT64 sz_File=0;
	STREAM_ACCESS_t *pAccess = NULL;
	HINT8 szUrl[ABR_MPD_CONST_URL_LENGTH]={0};
	pTunerHandle = tHandle.pTunerHandle;
	pAccess = (STREAM_ACCESS_t *)pTunerHandle->pHStream;
	if (NULL == pAccess)
	{
		PrintError("%s(%d) Error> Tuner handle doesn't include correct STREAM Handler. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return VFIO_ERROR_FAIL;
	}
	nRet = ABR_MPD_NextSegment(pTunerHandle->hAbr);
	if(nRet != DI_ERR_OK)
	{
		nRet = -1;
		PrintExit;
		return nRet;
	}

	ABR_MPD_MeasureBandwidth(pTunerHandle->hAbr, &ullBandwidth);

	ABR_MPD_SetLowerBitrate (pTunerHandle->hAbr, ABR_MPD_GetCurrentBandwidth(pTunerHandle->hAbr));

	nRet = ABR_MPD_GetSegmentUrl(pTunerHandle->hAbr,szUrl,ABR_MPD_CONST_URL_LENGTH);
	if(nRet != DI_ERR_OK)
	{
		nRet = -1;
		PrintExit;
		return nRet;
	}
	P_STREAM_ABR_ChangeUrl(pTunerHandle,szUrl);

	VK_MEM_Memset(&stNode,0x00,sizeof(ABR_QUEUE_NODE_t));
	stNode.nId = ABR_MPD_GetCurrentSegmentId(pTunerHandle->hAbr);
	stNode.range = pAccess->info.i_size;
	stNode.ullBitrate = ABR_MPD_GetCurrentBitrate(pTunerHandle->hAbr);
	ABR_QUEUE_PushBack(ABR_MPD_GetQueue(pTunerHandle->hAbr),stNode);

	//ABR_MPD_Queue_PushBack(pTunerHandle->hAbr,,sz_File,ABR_MPD_GetCurrentBitrate(pTunerHandle->hAbr));
	//DI_STREAM_HTTP_BoundsForNexus(tHandle,&sz_File);
	//ABR_MPD_Queue_PushBack(pTunerHandle->hAbr,ABR_MPD_GetCurrentSegmentId(pTunerHandle->hAbr),sz_File,ABR_MPD_GetCurrentBitrate(pTunerHandle->hAbr));
	PrintExit;
	return nRet;
}

#if 0 /* P_STREAM_ABR_PrevSegment is not used */
DI_ERR_CODE P_STREAM_ABR_PrevSegment(VFIO_Handle_t tHandle)
{
	HINT32 nRet = DI_ERR_OK;
	DI_IPTUNER_t *pTunerHandle = NULL;
	size_t sz_File=0;
	HINT8 szUrl[ABR_MPD_CONST_URL_LENGTH]={0};
	pTunerHandle = tHandle.pTunerHandle;
	nRet = ABR_MPD_PrevSegment(pTunerHandle->hAbr);
	if(nRet != DI_ERR_OK)
	{
		// nRet = -1;
		PrintExit;
		return nRet;
	}
	nRet = ABR_MPD_GetSegmentUrl(pTunerHandle->hAbr,szUrl,ABR_MPD_CONST_URL_LENGTH);
	if(nRet != DI_ERR_OK)
	{
		// nRet = -1;
		PrintExit;
		return nRet;
	}
	nRet = DI_STREAM_HTTP_ChangeUrl(tHandle, szUrl);
	if(nRet != DI_ERR_OK)
	{
		// nRet = -1;
		PrintExit;
		return nRet;
	}
	DI_STREAM_HTTP_BoundsForNexus(tHandle,&sz_File);
	nRet = ABR_MPD_Queue_PushFront(pTunerHandle->hAbr,ABR_MPD_GetCurrentSegmentId(pTunerHandle->hAbr),sz_File,ABR_MPD_GetCurrentBitrate(pTunerHandle->hAbr));
	if(nRet != DI_ERR_OK)
	{
		// nRet = -1;
		PrintExit;
		return nRet;
	}
	PrintExit;
	return nRet;
}
#endif

#if 0 /* P_STREAM_ABR_N_Segment is not used */
DI_ERR_CODE P_STREAM_ABR_N_Segment(VFIO_Handle_t tHandle, HINT32 nSegment)
{
	HINT32 nRet = DI_ERR_OK;
	DI_IPTUNER_t *pTunerHandle = NULL;
	size_t sz_File=0;
	HINT8 szUrl[ABR_MPD_CONST_URL_LENGTH]={0};
	pTunerHandle = tHandle.pTunerHandle;
	nRet = ABR_MPD_SetSegment(pTunerHandle->hAbr,nSegment);
	if(nRet != DI_ERR_OK)
	{
		// nRet = -1;
		PrintExit;
		return nRet;
	}
	nRet = ABR_MPD_GetSegmentUrl(pTunerHandle->hAbr,szUrl,ABR_MPD_CONST_URL_LENGTH);
	if(nRet != DI_ERR_OK)
	{
		// nRet = -1;
		PrintExit;
		return nRet;
	}
	nRet = DI_STREAM_HTTP_ChangeUrl(tHandle, szUrl);
	if(nRet != DI_ERR_OK)
	{
		// nRet = -1;
		PrintExit;
		return nRet;
	}
	//DI_STREAM_HTTP_BoundsForNexus(tHandle,&sz_File);
	// ABR_MPD_Queue_Reset(pTunerHandle->hAbr);
	// ABR_MPD_Queue_PushBack(pTunerHandle->hAbr,ABR_MPD_GetCurrentSegmentId(pTunerHandle->hAbr),sz_File,ABR_MPD_GetCurrentBitrate(pTunerHandle->hAbr));
	PrintExit;
	return nRet;
}
#endif

/**
 * @brief       setting 'n'th segment by offset
 * @param       tHandle : VFIO handle
 * @param	offset : target offset
 * @return      real offset in 'n'th segment
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/

HINT64 P_STREAM_ABR_N_SegmentByOffset(VFIO_Handle_t tHandle, HINT64 offset)
{
	HINT32 nRet = 0;
	HINT32 nQueueRet = 0;
	DI_IPTUNER_t *pTunerHandle = NULL;
	HINT64 sz_Real=0;
	HINT64 sz_File=0;
	HINT8 szUrl[ABR_MPD_CONST_URL_LENGTH]={0};
	HUINT64 ullTime = 0;
	HUINT32 ulDuration = 0;
	HUINT64 ullBitrate = 0;
	HINT32 nSegment = 0;
	HINT32 nNodeId = 0;
	ABR_QUEUE_NODE_t stNode;
	STREAM_ACCESS_t *pAccess = NULL;
	PrintEnter;
	pTunerHandle = tHandle.pTunerHandle;
	if(pTunerHandle == NULL)
	{
		PrintError ("%s (%d) Error>  IP Tuner Handle is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return nRet;
	}
	pAccess = (STREAM_ACCESS_t *)pTunerHandle->pHStream;
	if (NULL == pAccess)
	{
		PrintError("%s(%d) Error> Tuner handle doesn't include correct STREAM Handler. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return VFIO_ERROR_FAIL;
	}

#ifdef DI_IPTUNER_ABR_CONFIG_POSIX_NOINDEX /* posix_open with no index */
    ullBitrate = pTunerHandle->tMediaInfo.max_bitrate; /* 1. Max Bitrate Only */
#else /* posix_open with index */
    ullBitrate = ABR_MPD_GetCurrentBitrate(pTunerHandle->hAbr); /* 2. Current Bitrate  */
#endif
	if(ullBitrate <= 0)
	{
		PrintError ("%s (%d) Error>  Bitrate is Zero\n",__FUNCTION__,__LINE__);
		PrintExit;
		return nRet;
	}
	ullTime = P_STREAM_ABR_GetTimeForSeek(pTunerHandle);
	nRet = P_STREAM_ABR_SetTimeForSeek(pTunerHandle, 0);
	if(nRet != ABR_OK)
	{
		PrintError ("%s (%d) Error>  Reset TimeForSeek Fail.\n",__FUNCTION__,__LINE__);
		PrintExit;
		return nRet;
	}
	PrintError ("%s (%d) Debug>  ullTime is %lld\n",__FUNCTION__,__LINE__,ullTime);
	if(ullTime == 0 && offset > 0)
	{
		VK_MEM_Memset(&stNode,0x00,sizeof(ABR_QUEUE_NODE_t));
		nQueueRet = ABR_QUEUE_GetNodeInfoByOffset(ABR_MPD_GetQueue(pTunerHandle->hAbr),offset,&stNode);
		if( nQueueRet != ABR_OK )
		{
			ullTime = ceil(((double)(offset * 8)/ullBitrate)) * 1000; /* ceil() 소수점 올림 */
			PrintDebug ("%s (%d) Debug> Measure Time : %lld(Bitrate: %lld) \n",__FUNCTION__,__LINE__,ullTime,ullBitrate);
			nSegment = ABR_MPD_GetSegmentByTime(pTunerHandle->hAbr,ullTime);
			if(nSegment < 0)
			{
				nRet = -1;
				PrintExit;
				return nRet;
			}
			// nQueueRet = ABR_MPD_Queue_GetNodeInfoBySegment(pTunerHandle->hAbr,nSegment,&nNodeId,&sz_Real,&ullBitrate);
		}
		nRet = ABR_MPD_SetBitrate(pTunerHandle->hAbr,ullBitrate);
		if(nRet != DI_ERR_OK)
		{
			nRet = -1;
			PrintExit;
			return nRet;
		}
	}
	else
	{
		nSegment = ABR_MPD_GetSegmentByTime(pTunerHandle->hAbr,ullTime);
		if(nSegment < 0)
		{
			nRet = -1;
			PrintExit;
			return nRet;
		}
		VK_MEM_Memset(&stNode,0x00,sizeof(ABR_QUEUE_NODE_t));
		nQueueRet = ABR_QUEUE_GetNodeInfoBySegment(ABR_MPD_GetQueue(pTunerHandle->hAbr),nSegment,0,&stNode);
		//nQueueRet = ABR_MPD_Queue_GetNodeInfoBySegment(pTunerHandle->hAbr,nSegment,&nNodeId,&sz_Real,&ullBitrate);
		/*if(nQueueRet == DI_ERR_OK)
		{
			ABR_MPD_SetBitrate(pTunerHandle->hAbr,ullBitrate);
		}*/
	}
	nRet = ABR_MPD_SetSegment(pTunerHandle->hAbr,nSegment);
	if(nRet != DI_ERR_OK)
	{
		nRet = -1;
		PrintExit;
		return nRet;
	}
	nRet = ABR_MPD_GetSegmentUrl(pTunerHandle->hAbr,szUrl,ABR_MPD_CONST_URL_LENGTH);
	if(nRet != DI_ERR_OK)
	{
		nRet = -1;
		PrintExit;
		return nRet;
	}
	nRet = P_STREAM_ABR_ChangeUrl(pTunerHandle,szUrl);
	if(nRet != DI_ERR_OK)
	{
		nRet = -1;
		PrintExit;
		return nRet;
	}
	if( offset == 0 )
	{
		sz_Real = 0;
	}
	else if(ullTime != 0)
	{

		ulDuration = ABR_MPD_GetSegmentDuration(pTunerHandle->hAbr);
		ullBitrate = ABR_MPD_GetCurrentBitrate(pTunerHandle->hAbr);
		// ullTime /= 1000;
		PrintDebug ("%s (%d) Debug> ullTime : %lld, ulDuration : %lu, nSegment : %d, ullBitrate : %lld \n",__FUNCTION__,__LINE__,ullTime,ulDuration,nSegment,ullBitrate);
		// ullTime -= ((ulDuration/1000) * (nSegment+1));
		// sz_Real = ullTime * (ullBitrate/8);
		//sz_Real = ((offset - ((HFLOAT64)ulDuration/1000 * nSegment) * (ullBitrate/8)));
		sz_Real = ((HFLOAT64)(ullTime - (ulDuration * nSegment))/1000) * (ullBitrate/8);
		if(sz_Real < 0)
		{
			sz_Real = 0;
		}
	}
	else
	{
		sz_Real = offset - stNode.offset;
		if(sz_Real < 0)
		{
			sz_Real = 0;
		}
	}
	PrintDebug ("%s (%d) Debug> Additional Offset : %"PRId64"\n",__FUNCTION__,__LINE__,sz_Real);
	if(nQueueRet == DI_ERR_OK)
	{
		VK_MEM_Memset(&stNode,0x00,sizeof(ABR_QUEUE_NODE_t));
		stNode.nId = ABR_MPD_GetCurrentSegmentId(pTunerHandle->hAbr);
		stNode.range = pAccess->info.i_size;
		stNode.ullBitrate = ABR_MPD_GetCurrentBitrate(pTunerHandle->hAbr);
		nRet = ABR_QUEUE_Realign(pTunerHandle->hAbr,offset-sz_Real,stNode);
		//nRet = ABR_MPD_Queue_Replace(pTunerHandle->hAbr,offset-sz_Real,ABR_MPD_GetCurrentSegmentId(pTunerHandle->hAbr),sz_File,ABR_MPD_GetCurrentBitrate(pTunerHandle->hAbr));
	}
	else
	{
		ABR_QUEUE_Reset(ABR_MPD_GetQueue(pTunerHandle->hAbr),offset-sz_Real);
		// sz_File = pAccess->info.i_size;
		VK_MEM_Memset(&stNode,0x00,sizeof(ABR_QUEUE_NODE_t));
		stNode.nId = ABR_MPD_GetCurrentSegmentId(pTunerHandle->hAbr);
		stNode.range = pAccess->info.i_size;
		stNode.ullBitrate = ABR_MPD_GetCurrentBitrate(pTunerHandle->hAbr);
		nRet = ABR_QUEUE_PushBack(ABR_MPD_GetQueue(pTunerHandle->hAbr),stNode);
		// nRet = ABR_MPD_Queue_PushBack(pTunerHandle->hAbr,ABR_MPD_GetCurrentSegmentId(pTunerHandle->hAbr),sz_File,ABR_MPD_GetCurrentBitrate(pTunerHandle->hAbr));
	}
	PrintExit;
	return sz_Real;
}


int P_STREAM_ABR_OpenForProbe  (const HINT8 *pUrl, const HINT8 *mode, VFIO_Handle_t *ptHandle )
{
	HINT32 nContentLength = 0;
	HINT8 *pMpdBuff = NULL;
	HTTP_ACCESS_SYS_t *pstHttpSys=NULL;
	STREAM_ACCESS_t *pAccess=NULL;
	HINT32 nRet = DI_ERR_OK;
	UNUSED_PARAM(mode);
	DI_IPTUNER_t *pTunerHandle = NULL;
	HINT8 szUrl[ABR_MPD_CONST_URL_LENGTH]={0};
#if SUPPORT_HUMAX_COOKIE
	char *psCookie = NULL;
	URL_t tUrl;
#endif
	PrintEnter;
	if(pUrl==NULL)
	{
		ptHandle->ucAvailable = 0;
		PrintError("%s(%d) : Error> invalid pUrl(%s)\n",__FUNCTION__,__LINE__,pUrl);
		return VFIO_ERROR_IN_DEVICE;
	}
	pTunerHandle = DI_IPTUNER_POOL_FindByUrl(pUrl);
	if(pTunerHandle == NULL)
	{
		PrintError("%s(%d) : Error> pTunerHandle is NULL.\n",__FUNCTION__,__LINE__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	pAccess = (STREAM_ACCESS_t*)DD_MEM_Alloc(sizeof(STREAM_ACCESS_t));
	if(pAccess == NULL)
	{
		PrintError("%s(%d) Error> can't allocate STREAM handler\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}
	VK_memset(pAccess,0,sizeof(STREAM_ACCESS_t));

	/*
	 * We must store a STREAM Handler in iptuner handle .
	 * If not so, we can't know correct stream handle in vfio function set.(VFIO_Read/VFIO_xxx etc)
	 *
	 * iptuner_handler --  VFIO Handler (include file and socket descriptor)
	 *				   --  STREAM Handler ( include protocol controller)
	 */
	ptHandle->pTunerHandle = (void*) pTunerHandle;
	pTunerHandle->pVfioHandle = (void*) ptHandle;
	pTunerHandle->pHStream = (void*) pAccess;

	nContentLength = P_STREAM_ABR_GetManifest(pTunerHandle,&pMpdBuff);
	/*
		TODO: Find out manifest type(MPD,SMTH, etc... ) from Buffer
	*/
	pTunerHandle->hAbr = ABR_MPD_Open(pMpdBuff,nContentLength);
	if( pTunerHandle->hAbr == NULL)
	{
		PrintError ("%s(%d) Error> Manifest Open Fail !!\n",__FUNCTION__,__LINE__);
		nRet = DI_ERR_ERROR;
		PrintExit;
		return nRet;
	}
	/*
		CHECK: SMTH does not probe and have not initialize segment
	*/
	ABR_MPD_GetInitSegUrl(pTunerHandle->hAbr, szUrl, ABR_MPD_CONST_URL_LENGTH);

#if SUPPORT_HUMAX_COOKIE
	VK_MEM_Memset(&tUrl, 0x0, sizeof(tUrl));
	UTIL_UrlParse(&tUrl, (const char *)szUrl, 0);
	if(tUrl.psz_cookie != NULL)
	{
		psCookie = VK_StrDup(tUrl.psz_cookie);
	}
	UTIL_UrlClean(&tUrl);
#endif

	/* open for player */
	/** psz_path must be free */
	if(pAccess->psz_path != NULL)	{
		VK_MEM_Free(pAccess->psz_path);
		pAccess->psz_path = NULL;
	}
	/* TODO : Does this szUrl includes TID String. If so, we can use the real url string. Please check defination "UNIQUE_URL"*/
	pAccess->psz_path = VK_StrDup (szUrl);
#if SUPPORT_HUMAX_COOKIE
	if(psCookie != NULL)
	{
		pAccess->psz_cookie = psCookie;
	}
#endif
	if(pAccess->psz_path == NULL)
	{
		PrintError("%s(%d) : Error> Fail Mem alloc.\n",__FUNCTION__,__LINE__);
		ptHandle->ucAvailable = 0;

		if(pAccess->psz_path != NULL)
		{
			DD_MEM_Free(pAccess->psz_path);
			pAccess->psz_path =NULL;
		}
		/* else { continue; } */
#if SUPPORT_HUMAX_COOKIE
		if(pAccess->psz_cookie != NULL)
		{
			DD_MEM_Free(pAccess->psz_cookie);
			pAccess->psz_cookie =NULL;
		}
		/* else { continue; } */
#endif
		PrintExit;
		return VFIO_ERROR_IN_DEVICE;
	}

	/* Try to open the STREAM */
	if (DI_IPT_SUCCESS != STREAM_HTTP_Open (pAccess) )
	{
		ptHandle->ucAvailable = 0;

		if(pAccess->psz_path != NULL)
		{
			DD_MEM_Free(pAccess->psz_path);
			pAccess->psz_path =NULL;
		}
		/* else { continue; } */
#if SUPPORT_HUMAX_COOKIE
		if(pAccess->psz_cookie != NULL)
		{
			DD_MEM_Free(pAccess->psz_cookie);
			pAccess->psz_cookie =NULL;
		}
		/* else { continue; } */
#endif
		if(pAccess->psz_path != NULL)
		{
			DD_MEM_Free  (pAccess->psz_path);
			pAccess->psz_path =NULL;
		}
		/* else { continue; } */

		PrintError("%s (%d) : Error> STREAM_HTTP_Open Fail.\n",__FUNCTION__,__LINE__);
		PrintExit;
		return VFIO_ERROR_IN_DEVICE;
	}
	else
	{
		pstHttpSys = (HTTP_ACCESS_SYS_t *)pAccess->p_sys;
		ptHandle->tFile.callback.unHandle = (HUINT32) pstHttpSys->fd;
		ptHandle->ucAvailable = 1;
		PrintData ("%s(%d) : callback handle=0x%x\n",__FUNCTION__,__LINE__, ptHandle->tFile.callback.unHandle);
	}

	if( VK_OK != VK_SEM_Create(&pAccess->ulStreamSema,"httpSema", DEFAULT_SUSPEND_TYPE))
	{
		(void)STREAM_HTTP_Close (pAccess);
		if(pAccess->psz_path != NULL)
		{
			DD_MEM_Free(pAccess->psz_path);
			pAccess->psz_path =NULL;
		}
		/* else { continue; } */
#if SUPPORT_HUMAX_COOKIE
		if(pAccess->psz_cookie != NULL)
		{
			DD_MEM_Free(pAccess->psz_cookie);
			pAccess->psz_cookie =NULL;
		}
		/* else { continue; } */
#endif
		PrintError ("%s(%d) Error> can't create semaphore httpSema",__FUNCTION__,__LINE__);
		PrintExit;
		return VFIO_ERROR_IN_DEVICE;
	}
	PrintExit;
	return nRet;
}

int P_STREAM_ABR_ReadForProbe (VFIO_Handle_t tHandle, void *ptr, size_t size, size_t nmemb, ssize_t *ptReadSize)
{
	size_t sz_Real = 0;
	HINT32 n_ReturnedValue = 0;
	DI_IPTUNER_t *pTunerHandle = NULL;
	size_t sz_Try = size*nmemb;
	int ret = VFIO_ERROR_SUCCESS;
	STREAM_ACCESS_t *pAccess = NULL;
//	PrintEnter;
	pTunerHandle = (DI_IPTUNER_t*)tHandle.pTunerHandle;
	if (pTunerHandle == NULL)
	{
		tHandle.ucAvailable = 0;
		PrintError ("%s (%d) Error>  Invalid Tuner Handle in VFIO Handle\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}
	if(pTunerHandle->hAbr == NULL)
	{
		PrintError("%s(%d) : Error> hAbr is NULL.\n",__FUNCTION__,__LINE__);
	}
	/* Now we just get the Stream handler */
	pAccess = (STREAM_ACCESS_t *)pTunerHandle->pHStream;
	if (NULL == pAccess)
	{
		PrintError("%s(%d) Error> Tuner handle doesn't include correct STREAM Handler. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return VFIO_ERROR_FAIL;
	}
	if(pAccess->info.i_size < pAccess->info.i_pos + sz_Try)
	{
		sz_Try = pAccess->info.i_size - pAccess->info.i_pos;
	}
	(void)VK_SEM_Get(pAccess->ulStreamSema);
	do
	{
		n_ReturnedValue = STREAM_HTTP_Read ( pAccess, (HUINT8 *)ptr+sz_Real, sz_Try - sz_Real );
		if(n_ReturnedValue < 0)
		{
			*ptReadSize = n_ReturnedValue;
			goto stream_read_error;
		}
		sz_Real += n_ReturnedValue;
	}
	while (sz_Real < sz_Try && !pAccess->info.b_eof);

	*ptReadSize = sz_Real;

	stream_read_error:
	stream_read_success:
	(void)VK_SEM_Release (pAccess->ulStreamSema);
//	PrintExit;
	return ret;
}

int P_STREAM_ABR_SeekForProbe (VFIO_Handle_t tHandle, off_t offset, int whence)
{
	DI_IPTUNER_t *pTunerHandle = NULL;
	int nRet = VFIO_ERROR_SUCCESS;
	STREAM_ACCESS_t *pAccess = NULL;
	UNUSED_PARAM(whence);
	pTunerHandle = (DI_IPTUNER_t*)tHandle.pTunerHandle;
	if (pTunerHandle == NULL)
	{
		tHandle.ucAvailable = 0;
		PrintError ("%s (%d) Error>  Invalid Tuner Handle in VFIO Handle\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}
	if(pTunerHandle->hAbr == NULL)
	{
		PrintError("%s(%d) : Error> hAbr is NULL.\n",__FUNCTION__,__LINE__);
	}
	/* Now we just get the Stream handler */
	pAccess = (STREAM_ACCESS_t *)pTunerHandle->pHStream;
	if (NULL == pAccess)
	{
		PrintError("%s(%d) Error> Tuner handle doesn't include correct STREAM Handler. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return VFIO_ERROR_FAIL;
	}
	(void)VK_SEM_Get(pAccess->ulStreamSema);
	nRet = STREAM_HTTP_Seek ( pAccess, offset );
	if(nRet != DI_IPT_SUCCESS)
	{
		PrintError ("%s(%d) Error> STREAM_HTTP_Seek fail \n",__FUNCTION__,__LINE__);
		PrintExit;
		(void)VK_SEM_Release (pAccess->ulStreamSema);
		return -1;
	}
	else
	{
		PrintExit;
		(void)VK_SEM_Release (pAccess->ulStreamSema);
		return offset;
	}
	(void)VK_SEM_Release (pAccess->ulStreamSema);
	return offset;
}

int P_STREAM_ABR_BoundsForProbe(VFIO_Handle_t tHandle, off_t *file_size)
{
	PrintEnter;
	DI_IPTUNER_t *pTunerHandle = NULL;
	int nRet = VFIO_ERROR_SUCCESS;
	STREAM_ACCESS_t *pAccess = NULL;
	pTunerHandle = (DI_IPTUNER_t*)tHandle.pTunerHandle;
	if (pTunerHandle == NULL)
	{
		tHandle.ucAvailable = 0;
		PrintError ("%s (%d) Error>  Invalid Tuner Handle in VFIO Handle\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}
	if(pTunerHandle->hAbr == NULL)
	{
		PrintError("%s(%d) : Error> hAbr is NULL.\n",__FUNCTION__,__LINE__);
	}
	/* Now we just get the Stream handler */
	pAccess = (STREAM_ACCESS_t *)pTunerHandle->pHStream;
	if (NULL == pAccess)
	{
		PrintError("%s(%d) Error> Tuner handle doesn't include correct STREAM Handler. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return VFIO_ERROR_FAIL;
	}
	*file_size = pAccess->info.i_size;
	PrintExit;
	return nRet;
}

/**
 * @brief       "OpenFromNexus"
 * @param	path : path of file
 * @param       flags :
 * @param	ptHandle : VFIO Handle
 * @return     	VFIO err code
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
/* Open From Nexus */
int P_STREAM_ABR_OpenFromNexus (const HINT8 *pUrl, const unsigned int flags, VFIO_Handle_t *ptHandle)
{
	ABR_QUEUE_NODE_t stNode;
	HTTP_ACCESS_SYS_t *pstHttpSys=NULL;
	STREAM_ACCESS_t *pAccess=NULL;
	HINT64 sz_File=0;
	HINT32 nRet = DI_ERR_OK;
	UNUSED_PARAM (flags);
	DI_IPTUNER_t *pTunerHandle = NULL;
	HINT8 szUrl[ABR_MPD_CONST_URL_LENGTH]={0};
#if SUPPORT_HUMAX_COOKIE
	char *psCookie = NULL;
	URL_t tUrl;
#endif
	PrintEnter;
	if(pUrl==NULL)
	{
		ptHandle->ucAvailable = 0;
		PrintError("%s(%d) : Error> invalid pUrl(%s)\n",__FUNCTION__,__LINE__,pUrl);
		return VFIO_ERROR_IN_DEVICE;
	}
	pTunerHandle = DI_IPTUNER_POOL_FindByUrl(pUrl);
	if(pTunerHandle == NULL)
	{
		PrintError("%s(%d) : Error> pTunerHandle is NULL.\n",__FUNCTION__,__LINE__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	if(pTunerHandle->hAbr == NULL)
	{
		PrintError("%s(%d) : Error> hAbr is NULL.\n",__FUNCTION__,__LINE__);
		PrintExit;
		return DI_ERR_ERROR;
	}
	pAccess = (STREAM_ACCESS_t*)DD_MEM_Alloc(sizeof(STREAM_ACCESS_t));
	if(pAccess == NULL)
	{
		PrintError("%s(%d) Error> can't allocate STREAM handler\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}
	VK_memset(pAccess,0,sizeof(STREAM_ACCESS_t));

	/*
	 * We must store a STREAM Handler in iptuner handle .
	 * If not so, we can't know correct stream handle in vfio function set.(VFIO_Read/VFIO_xxx etc)
	 *
	 * iptuner_handler --  VFIO Handler (include file and socket descriptor)
	 *				   --  STREAM Handler ( include protocol controller)
	 */
	ptHandle->pTunerHandle = (void*) pTunerHandle;
	pTunerHandle->pVfioHandle = (void*) ptHandle;
	pTunerHandle->pHStream = (void*) pAccess;

	ABR_MPD_GetSegmentUrl(pTunerHandle->hAbr,szUrl,ABR_MPD_CONST_URL_LENGTH);

#if SUPPORT_HUMAX_COOKIE
	VK_MEM_Memset(&tUrl, 0x0, sizeof(tUrl));
	UTIL_UrlParse(&tUrl, (const char *)szUrl, 0);
	if(tUrl.psz_cookie != NULL)
	{
		psCookie = VK_StrDup(tUrl.psz_cookie);
	}
	UTIL_UrlClean(&tUrl);
#endif

	/* open for player */
	/** psz_path must be free */
	if(pAccess->psz_path != NULL)
	{
		VK_MEM_Free(pAccess->psz_path);
		pAccess->psz_path = NULL;
	}
	/* TODO : Does this szUrl includes TID String. If so, we can use the real url string. Please check defination "UNIQUE_URL"*/
	pAccess->psz_path = VK_StrDup (szUrl);
#if SUPPORT_HUMAX_COOKIE
	if(psCookie != NULL)
	{
		pAccess->psz_cookie = psCookie;
	}
#endif
	if(pAccess->psz_path == NULL)
	{
		PrintError("%s(%d) : Error> Fail Mem alloc.\n",__FUNCTION__,__LINE__);
		ptHandle->ucAvailable = 0;

		if(pAccess->psz_path != NULL)
		{
			DD_MEM_Free(pAccess->psz_path);
			pAccess->psz_path =NULL;
		}
		/* else { continue; } */
#if SUPPORT_HUMAX_COOKIE
		if(pAccess->psz_cookie != NULL)
		{
			DD_MEM_Free(pAccess->psz_cookie);
			pAccess->psz_cookie =NULL;
		}
		/* else { continue; } */
#endif
		PrintExit;
		return VFIO_ERROR_IN_DEVICE;
	}

	/* Try to open the STREAM */
	if (DI_IPT_SUCCESS != STREAM_HTTP_Open (pAccess) )
	{
		ptHandle->ucAvailable = 0;

		if(pAccess->psz_path != NULL)
		{
			DD_MEM_Free(pAccess->psz_path);
			pAccess->psz_path =NULL;
		}
		/* else { continue; } */
#if SUPPORT_HUMAX_COOKIE
		if(pAccess->psz_cookie != NULL)
		{
			DD_MEM_Free(pAccess->psz_cookie);
			pAccess->psz_cookie =NULL;
		}
		/* else { continue; } */
#endif
		if(pAccess->psz_path != NULL)
		{
			DD_MEM_Free  (pAccess->psz_path);
			pAccess->psz_path =NULL;
		}
		/* else { continue; } */

		PrintError("%s (%d) : Error> STREAM_HTTP_Open Fail.\n",__FUNCTION__,__LINE__);
		PrintExit;
		return VFIO_ERROR_IN_DEVICE;
	}
	else
	{
		pstHttpSys = (HTTP_ACCESS_SYS_t *)pAccess->p_sys;
		ptHandle->tFile.callback.unHandle = (HUINT32) pstHttpSys->fd;
		ptHandle->ucAvailable = 1;
		PrintData ("%s(%d) : callback handle=0x%x\n",__FUNCTION__,__LINE__, ptHandle->tFile.callback.unHandle);
	}

	if( VK_OK != VK_SEM_Create(&pAccess->ulStreamSema,"httpSema", DEFAULT_SUSPEND_TYPE))
	{
		(void)STREAM_HTTP_Close (pAccess);
		if(pAccess->psz_path != NULL)
		{
			DD_MEM_Free(pAccess->psz_path);
			pAccess->psz_path =NULL;
		}
		/* else { continue; } */
#if SUPPORT_HUMAX_COOKIE
		if(pAccess->psz_cookie != NULL)
		{
			DD_MEM_Free(pAccess->psz_cookie);
			pAccess->psz_cookie =NULL;
		}
		/* else { continue; } */
#endif
		PrintError ("%s(%d) Error> can't create semaphore httpSema",__FUNCTION__,__LINE__);
		PrintExit;
		return VFIO_ERROR_IN_DEVICE;
	}

	/* queuing first file */
	ABR_QUEUE_Reset(ABR_MPD_GetQueue(pTunerHandle->hAbr),0);
	VK_MEM_Memset(&stNode,0x00,sizeof(ABR_QUEUE_NODE_t));
	stNode.nId = ABR_MPD_GetCurrentSegmentId(pTunerHandle->hAbr);
	stNode.range = pAccess->info.i_size;
	stNode.ullBitrate = ABR_MPD_GetCurrentBitrate(pTunerHandle->hAbr);
	nRet = ABR_QUEUE_PushBack(ABR_MPD_GetQueue(pTunerHandle->hAbr),stNode);

	//ABR_MPD_Queue_PushBack(pTunerHandle->hAbr,ABR_MPD_GetCurrentSegmentId(pTunerHandle->hAbr),sz_File,ABR_MPD_GetCurrentBitrate(pTunerHandle->hAbr));
	PrintExit;
	return nRet;
}

/**
 * @brief       "SeekFromNexus"
 * @param	tHandle : VFIO Handle
 * @param       offset : offset for seeking
 * @param	whence : whence
 * @return     	VFIO err code
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
/* Seek From Nexus : Long jump 인 경우 어느정도는 block 되어도 된다.
 * 단 Read 함수에서는 block 되면 waiting IO error, 0 return 시 endofStream, -1 return시 errorofstream 이 뜬다.*/
off_t P_STREAM_ABR_SeekForNexus (VFIO_Handle_t tHandle, off_t offset, int whence)
{
	HINT64 nStreamLength = 0;
	// HINT32 nSegment = 0;
	HINT32 nQueueRet = DI_ERR_OK;
	HINT32 nCurSegment = 0;
	// HUINT64 ullBitrate = 0;
	ABR_QUEUE_NODE_t stNode;
	STREAM_ACCESS_t *pAccess = NULL;
	off_t ret = 0;
	off_t realOffset = 0;
	UNUSED_PARAM(whence);

	DI_IPTUNER_t *pTunerHandle = NULL;
//	PrintEnter;
	pTunerHandle = (DI_IPTUNER_t *)tHandle.pTunerHandle;
	if(pTunerHandle == NULL)
	{
		PrintError("%s(%d) : Error> pTunerHandle is NULL.\n",__FUNCTION__,__LINE__);
		return ret;
	}
	if(pTunerHandle->hAbr == NULL)
	{
		PrintError("%s(%d) : Error> hAbr is NULL.\n",__FUNCTION__,__LINE__);
	}
	/* Now we just get the Stream handler */
	pAccess = (STREAM_ACCESS_t *)pTunerHandle->pHStream;
	if (NULL == pAccess)
	{
		PrintError("%s(%d) Error> Tuner handle doesn't include correct STREAM Handler. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return VFIO_ERROR_FAIL;
	}
	//nStreamLength = ABR_MPD_GetStreamLength(pTunerHandle->hAbr);
	/* checking for "end of stream" */
	nStreamLength = P_STREAM_ABR_GetVStreamLength(pTunerHandle);
	if(nStreamLength > 0 && nStreamLength <= offset)
	{
		PrintError("%s(%d) End of Stream.\n",__FUNCTION__,__LINE__);
		return ret;
	}
	(void)VK_SEM_Get(pAccess->ulStreamSema);
	PrintDebug ("%s(%d) Trying offset %"PRId64"\n",__FUNCTION__,__LINE__, offset);
#if 1 /* NOT USE CURRENT POSITION for CANVAS by ykkim */
	nCurSegment = ABR_MPD_GetCurrentSegmentId(pTunerHandle->hAbr);
	// PrintError("%s(%d) : Debug> Virtual Read Position.\n",__FUNCTION__,__LINE__);
	// nQueueRet = ABR_MPD_Queue_GetNodeInfo(pTunerHandle->hAbr,offset,&nSegment,&realOffset,&ullBitrate);
	VK_MEM_Memset(&stNode,0x00,sizeof(ABR_QUEUE_NODE_t));
	nQueueRet = ABR_QUEUE_GetNodeInfoByOffset(ABR_MPD_GetQueue(pTunerHandle->hAbr),offset,&stNode);
	if(nCurSegment != stNode.nId)
	{
		PrintDebug ("%s(%d) Current : %d , stNode : %d\n",__FUNCTION__,__LINE__, nCurSegment, stNode.nId);
		/* if target offset is equal to the offset of rear node in playlist queue, need to read next segment */
		if((nCurSegment + 1) == stNode.nId && nQueueRet == DI_ERR_OK)
		{
			ret = P_STREAM_ABR_NextSegment(tHandle);
			if(ret != DI_ERR_OK)
			{
				VK_MEM_Memset(&stNode,0x00,sizeof(ABR_QUEUE_NODE_t));
				ABR_QUEUE_GetCurNodeInfo(ABR_MPD_GetQueue(pTunerHandle->hAbr),&stNode);
				P_STREAM_ABR_SetVStreamLength(pTunerHandle, stNode.offset);
				PrintError("%s(%d) : Debug> Set EndOfStream = %"PRId64" \n",__FUNCTION__,__LINE__,stNode.offset);
				(void)VK_SEM_Release (pAccess->ulStreamSema);
				return 0;
			}
			VK_MEM_Memset(&stNode,0x00,sizeof(ABR_QUEUE_NODE_t));
			nQueueRet = ABR_QUEUE_GetNodeInfoByOffset(ABR_MPD_GetQueue(pTunerHandle->hAbr),offset,&stNode);
			//nQueueRet = ABR_MPD_Queue_GetNodeInfo(pTunerHandle->hAbr,offset,&nSegment,&realOffset,&ullBitrate);
			realOffset = offset - (stNode.offset - stNode.range);
			if(realOffset < 0)
			{
				realOffset = 0;
			}
		}
		else
		{
			PrintDebug("%s(%d) Seek by setPosition/setSpeed.\n",__FUNCTION__,__LINE__);
			realOffset = P_STREAM_ABR_N_SegmentByOffset(tHandle,offset);
			/**
			 * should reset virtual length of stream
			 * because stored virtual length of stream will be inaccuracy after seek or trick to back
			*/
			P_STREAM_ABR_SetVStreamLength(pTunerHandle,0);
			if(realOffset < 0)
			{
				realOffset = 0;
			}
		}
	}
	else
	{
		realOffset = offset - (stNode.offset - stNode.range);
		if(realOffset < 0)
		{
			realOffset = 0;
		}
	}
	ret = STREAM_HTTP_Seek ( pAccess, realOffset );
	if(ret != DI_IPT_SUCCESS)
	{
		PrintError ("%s(%d) Error> STREAM_HTTP_Seek fail \n",__FUNCTION__,__LINE__);
		PrintExit;
		(void)VK_SEM_Release (pAccess->ulStreamSema);
		return -1;
	}
	P_STREAM_ABR_SetVCurrentPosition(pTunerHandle,offset);
	(void)VK_SEM_Release (pAccess->ulStreamSema);
	//	PrintExit;
#else /* USE CURRENT POSITION for CANVAS by ykkim */
	PrintDebug ("%s(%d) Trying offset %"PRId64" (Virtual Offset %"PRId64")\n",__FUNCTION__,__LINE__, offset,P_STREAM_ABR_GetVCurrentPosition(pTunerHandle));
	/* if current position is equal to target offset, "seek for nexus" is linear seek(Single TS Streaming) */
	if(P_STREAM_ABR_GetVCurrentPosition(pTunerHandle) == offset)
	{
		nCurSegment = ABR_MPD_GetCurrentSegmentId(pTunerHandle->hAbr);
		// PrintError("%s(%d) : Debug> Virtual Read Position.\n",__FUNCTION__,__LINE__);
		nQueueRet = ABR_MPD_Queue_GetNodeInfo(pTunerHandle->hAbr,offset,&nSegment,&realOffset,&ullBitrate);
		if(nCurSegment != nSegment)
		{
			/* if target offset is equal to the offset of rear node in playlist queue, need to read next segment */
			if((nCurSegment + 1) == nSegment && nQueueRet == DI_ERR_OK)
			{
				ret = P_STREAM_ABR_NextSegment(tHandle);
				if(ret != DI_ERR_OK)
				{
					ABR_MPD_Queue_GetCurNodeInfo(pTunerHandle->hAbr,NULL,&realOffset,NULL);
					//ABR_MPD_SetStreamLength(pTunerHandle->hAbr,sz_voffset);
					P_STREAM_ABR_SetVStreamLength(pTunerHandle, realOffset);
					PrintError("%s(%d) : Debug> Set EndOfStream = %"PRId64" \n",__FUNCTION__,__LINE__,realOffset);
					(void)VK_SEM_Release (pAccess->ulStreamSema);
					return 0;
				}
				nQueueRet = ABR_MPD_Queue_GetNodeInfo(pTunerHandle->hAbr,offset,&nSegment,&realOffset,&ullBitrate);
				realOffset = offset - realOffset;
				if(realOffset < 0)
				{
					realOffset = 0;
				}
			}
#if 0
			else if(nQueueRet == DI_ERR_OK)
			{
				// TODO: Set Segment using playlist queue
			}
#endif
			else
			{
/*
				if(nQueueRet > 0)
				{
					while(nQueueRet != DI_ERR_OK)
					{
						P_STREAM_ABR_NextSegment(tHandle);
						nQueueRet = ABR_MPD_Queue_GetNodeInfo(pTunerHandle->hAbr,offset,&nSegment,&realOffset,&ullBitrate);
						realOffset = offset - realOffset;
						if(realOffset < 0)
						{
							realOffset = 0;
						}
					}
				}
				else
*/
				{
					PrintError("%s(%d) : Warn> re-calcurate offset(forced).\n",__FUNCTION__,__LINE__);
					realOffset = P_STREAM_ABR_N_SegmentByOffset(tHandle,offset);
					if(realOffset < 0)
					{
						realOffset = 0;
					}
				}
			}
		}
		else
		{
			realOffset = offset - realOffset;
			if(realOffset < 0)
			{
				realOffset = 0;
			}
		}
	}
	else
	{
#if 0 	/* for Fragmented MP4 */
		nCurSegment = ABR_MPD_GetCurrentSegmentId(pTunerHandle->hAbr);
		// PrintError("%s(%d) : Debug> Virtual Read Position.\n",__FUNCTION__,__LINE__);
		nQueueRet = ABR_MPD_Queue_GetNodeInfo(pTunerHandle->hAbr,offset,&nSegment,&realOffset,&ullBitrate);
		if(nCurSegment != nSegment)
		{
			if((nCurSegment + 1) == nSegment && nQueueRet == DI_ERR_OK)
			{
				ret = P_STREAM_ABR_NextSegment(tHandle);
				if(ret != DI_ERR_OK)
				{
					ABR_MPD_Queue_GetCurNodeInfo(pTunerHandle->hAbr,NULL,&realOffset,NULL);
					//ABR_MPD_SetStreamLength(pTunerHandle->hAbr,sz_voffset);
					P_STREAM_ABR_SetVStreamLength(pTunerHandle, realOffset);
					PrintDebug("%s(%d) : Debug> Set EndOfStream = %"PRId64" \n",__FUNCTION__,__LINE__,realOffset);
					return 0;
				}

				nQueueRet = ABR_MPD_Queue_GetNodeInfo(pTunerHandle->hAbr,offset,&nSegment,&realOffset,&ullBitrate);
				realOffset = offset - realOffset;
				if(realOffset < 0)
				{
					realOffset = 0;
				}
			}
		}
		else
#endif
		{
			PrintDebug("%s(%d) : Debug> Seek by setPosition/setSpeed.\n",__FUNCTION__,__LINE__);
			realOffset = P_STREAM_ABR_N_SegmentByOffset(tHandle,offset);
			/**
			 * should reset virtual length of stream
			 * because stored virtual length of stream will be inaccuracy after seek or trick to back
			*/
			P_STREAM_ABR_SetVStreamLength(pTunerHandle,0);
			if(realOffset < 0)
			{
				realOffset = 0;
			}
		}
	}
	// PrintDebug ("%s(%d) Trying Real Offset %"PRId64"\n",__FUNCTION__,__LINE__, realOffset);

	ret = STREAM_HTTP_Seek ( pAccess, realOffset );
	if(ret != DI_IPT_SUCCESS)
	{
		PrintError ("%s(%d) Error> STREAM_HTTP_Seek fail \n",__FUNCTION__,__LINE__);
		PrintExit;
		(void)VK_SEM_Release (pAccess->ulStreamSema);
		return -1;
	}
	P_STREAM_ABR_SetVCurrentPosition(pTunerHandle,offset);
	(void)VK_SEM_Release (pAccess->ulStreamSema);
#endif /* NOT USE/USE CURRENT POSITION for CANVAS by ykkim */
	return offset;
}

/**
 * @brief       "BoundsFromNexus"
 * @param	tHandle : VFIO Handle
 * @param       file_size : size of content
 * @return     	VFIO err code
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
/* Bounds From Nexus */
int P_STREAM_ABR_BoundsForNexus (VFIO_Handle_t tHandle, off_t *file_size)
{
	HINT32 nRet = VFIO_ERROR_SUCCESS;
	HUINT32 ulDuration = 0;
	HUINT32 ulMaxBitrate = 0;
	DI_IPTUNER_t *pTunerHandle = NULL;
	pTunerHandle = (DI_IPTUNER_t *)tHandle.pTunerHandle;
	if(pTunerHandle == NULL)
	{
		PrintError("%s(%d) : Error> pTunerHandle is NULL.\n",__FUNCTION__,__LINE__);
		return nRet;
	}
	if(pTunerHandle->hAbr == NULL)
	{
		PrintError("%s(%d) : Error> hAbr is NULL.\n",__FUNCTION__,__LINE__);
	}
	*file_size = P_STREAM_ABR_GetVStreamLength(pTunerHandle);
	if(*file_size <= 0)
	{
		ulDuration = ABR_MPD_GetMediaDuration(pTunerHandle->hAbr);
		ulDuration /= 1000;
		ulMaxBitrate = ABR_MPD_GetMaxBitrate(pTunerHandle->hAbr,ABR_MPD_GetCurrentPeriodId (pTunerHandle->hAbr));
		ulMaxBitrate /= 8;
		*file_size = (((ulDuration + DI_IPTUNER_ABR_CONST_BOUNDS_PADDING) * ulMaxBitrate) * 2);
	}
	return nRet;
}

/**
 * @brief       "Open" From User Application and bmedia
 * @param	path : path of file
 * @param       mode :
 * @param	ptHandle : VFIO Handle
 * @return     	VFIO err code
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
/** Open From User Application and bmedia*/
int P_STREAM_ABR_Open  (const HINT8 *path, const HINT8 *mode, VFIO_Handle_t *ptHandle )
{
	HINT32 vRet = VFIO_ERROR_SUCCESS;
	PrintEnter;
	P_STREAM_ABR_OpenFromNexus(path,mode,ptHandle);
	PrintExit;
	return vRet;
}

/** */
int P_STREAM_ABR_Close (VFIO_Handle_t tHandle)
{
	HINT32 vRet = VFIO_ERROR_SUCCESS;
	STREAM_ACCESS_t *pAccess = NULL;
	DI_IPTUNER_t *pTunerHandle = NULL;
	PrintEnter;
	pTunerHandle = tHandle.pTunerHandle;
	if (pTunerHandle == NULL)
	{
		tHandle.ucAvailable = 0;
		PrintError ("%s (%d) Error>  Invalid Tuner Handle in VFIO Handle\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}
	/* Now we just get the Stream handler */
	pAccess = (STREAM_ACCESS_t *)pTunerHandle->pHStream;
	if (NULL == pAccess)
	{
		PrintError("%s(%d) Error> Tuner handle doesn't include correct STREAM Handler. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return VFIO_ERROR_FAIL;
	}

/* In multi-open case, the http session should be closed only once. */
	if(pAccess->b_alive == FALSE)
	{
		PrintDebug ("%s(%d) : HTTP Session is already closed.\n",__FUNCTION__,__LINE__);
		goto success;
	}

	(void)VK_SEM_Get(pAccess->ulStreamSema);

	(void) STREAM_HTTP_Close ( pAccess );

	pAccess->b_alive = FALSE;

	if(pAccess->psz_path != NULL)
	{
		DD_MEM_Free  (pAccess->psz_path);
		pAccess->psz_path =NULL;
	}
	/* else { continue; } */

#if SUPPORT_HUMAX_COOKIE
	if(pAccess->psz_cookie != NULL)
	{
		DD_MEM_Free(pAccess->psz_cookie);
		pAccess->psz_cookie =NULL;
	}
	/* else { continue; } */
#endif
	(void)VK_SEM_Release (pAccess->ulStreamSema);
	(void)VK_SEM_Destroy(pAccess->ulStreamSema);
	pAccess->ulStreamSema = 0;

success:
	PrintExit;
	return vRet;
}

/**
 * @brief       "Read" From User Application and bmedia
 * @param	tHandle : VFIO Handle
 * @param       ptr : pointer of buffer
 * @param	size : size of memory for read
 * @param	nmemb : size of memory for read
 * @param	ptReadSize : size of byte
 * @return     	VFIO err code
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
int P_STREAM_ABR_Read (VFIO_Handle_t tHandle, void *ptr, size_t size, size_t nmemb, ssize_t *ptReadSize)
{
	ABR_QUEUE_NODE_t stNode;
	size_t sz_Real = 0;
	HINT32 n_ReturnedValue = 0;
	//HINT64 sz_voffset = 0;
	TIMEVAL_t tvstart,tvend;
	DI_IPTUNER_t *pTunerHandle = NULL;
	size_t sz_Try = size*nmemb;
	int ret = VFIO_ERROR_SUCCESS;
	int nRet = VFIO_ERROR_SUCCESS;
	STREAM_ACCESS_t *pAccess = NULL;
//	PrintEnter;
	pTunerHandle = (DI_IPTUNER_t*)tHandle.pTunerHandle;
	if (pTunerHandle == NULL)
	{
		tHandle.ucAvailable = 0;
		PrintError ("%s (%d) Error>  Invalid Tuner Handle in VFIO Handle\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}
	if(pTunerHandle->hAbr == NULL)
	{
		PrintError("%s(%d) : Error> hAbr is NULL.\n",__FUNCTION__,__LINE__);
	}
	/* Now we just get the Stream handler */
	pAccess = (STREAM_ACCESS_t *)pTunerHandle->pHStream;
	if (NULL == pAccess)
	{
		PrintError("%s(%d) Error> Tuner handle doesn't include correct STREAM Handler. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return VFIO_ERROR_FAIL;
	}
	gettimeofday(&tvstart,NULL);
	(void)VK_SEM_Get(pAccess->ulStreamSema);
	do
	{
		n_ReturnedValue = STREAM_HTTP_Read ( pAccess, (HUINT8 *)ptr+sz_Real, sz_Try - sz_Real );
		if(n_ReturnedValue < 0)
		{
			*ptReadSize = n_ReturnedValue;
			goto stream_read_error;
		}
		sz_Real += n_ReturnedValue;
		if( pAccess->info.i_pos >= pAccess->info.i_size || TRUE == pAccess->info.b_eof)
		{
			nRet = P_STREAM_ABR_NextSegment(tHandle);
			if(nRet != DI_ERR_OK)
			{
				*ptReadSize = sz_Real;
				ABR_QUEUE_GetCurNodeInfo(ABR_MPD_GetQueue(pTunerHandle->hAbr),&stNode);
				P_STREAM_ABR_SetVStreamLength(pTunerHandle, stNode.offset);
				PrintError("%s(%d) Set EndOfStream = %"PRId64" \n",__FUNCTION__,__LINE__,stNode.offset);
				goto stream_read_success;
			}
		}
	}
	while (sz_Real < sz_Try);

	*ptReadSize = sz_Real;

	stream_read_error:
	stream_read_success:
#if 1 /* USE CURRENT POSITION for CANVAS by ykkim */
	P_STREAM_ABR_SetVCurrentPosition(pTunerHandle, P_STREAM_ABR_GetVCurrentPosition(pTunerHandle) + (*ptReadSize));
#endif
	gettimeofday(&tvend,NULL);
	ABR_MPD_SetThroughput(pTunerHandle->hAbr,*ptReadSize,tvstart,tvend);
//	PrintDebug("%s(%d) : handle (0x%x) tryRead(%d) realRead(%d)\n",__FUNCTION__,__LINE__, tHandle.tFile.callback.unHandle,sz_Try, sz_Real);
	(void)VK_SEM_Release (pAccess->ulStreamSema);
	return ret;
}

/**
 * @brief       "Seek" From User Application and bmedia
 * @param	tHandle : VFIO Handle
 * @param       offset : offset for seeking
 * @param	whence : whence
 * @return     	VFIO err code
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
/** This Function called by bmedia probe only.*/
int P_STREAM_ABR_Seek (VFIO_Handle_t tHandle, off_t offset, int whence)
{
	HINT32 nRet = VFIO_ERROR_SUCCESS;
//	VFIO_Callback_t *pVfioCb = NULL;
//	PrintEnter;
//	UNUSED_PARAM (tHandle);
#if 0
	pVfioCb = (VFIO_Callback_t *)(((DI_IPTUNER_t *)tHandle.pTunerHandle)->pProtocolCallbacks);
	nRet = pVfioCb->Seek(tHandle, offset, whence);
#endif
	nRet = P_STREAM_ABR_SeekForNexus(tHandle,offset,whence);
	//PrintExit;
	return nRet;
}

/**
 * @brief       "Tell" From User Application and bmedia
 * @param	tHandle : VFIO Handle
 * @param       plCurOffset : current offset
 * @return     	VFIO err code
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
int P_STREAM_ABR_Tell (VFIO_Handle_t tHandle, off_t *plCurOffset)
{
	PrintEnter;
	DI_IPTUNER_t *pTunerHandle = NULL;
	STREAM_ACCESS_t *pAccess = NULL;
	pTunerHandle = tHandle.pTunerHandle;
	if (pTunerHandle == NULL)
	{
		tHandle.ucAvailable = 0;
		PrintError ("%s (%d) Error>  Invalid Tuner Handle in VFIO Handle\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}
	// TODO: must implement for ABR
	/* Now we just get the Stream handler */
	pAccess = (STREAM_ACCESS_t *)pTunerHandle->pHStream;
	if (NULL == pAccess)
	{
		PrintError("%s(%d) Error> Tuner handle doesn't include correct STREAM Handler. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return VFIO_ERROR_FAIL;
	}
	*plCurOffset = pAccess->info.i_pos;
	//	PrintDebug ("%s(%d) : i_pos (%"PRId64") nRet(%d)\n",__FUNCTION__,__LINE__,*plCurOffset, nRet);
	PrintExit;
	return VFIO_ERROR_SUCCESS;
}

/**
 * @brief       "IsEOF" From User Application and bmedia
 * @param	tHandle : VFIO Handle
 * @return     	bool
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/

/** if EOF is true, must be return -1 like feof*/
int P_STREAM_ABR_IsEOF (VFIO_Handle_t tHandle)
{
	HINT32 nRet = VFIO_ERROR_SUCCESS;
	DI_IPTUNER_t *pTunerHandle = NULL;
	STREAM_ACCESS_t *pAccess = NULL;
//	PrintEnter;
	pTunerHandle = tHandle.pTunerHandle;
	if (pTunerHandle == NULL)
	{
		tHandle.ucAvailable = 0;
		PrintError ("%s (%d) Error>  Invalid Tuner Handle in VFIO Handle\n",__FUNCTION__,__LINE__);
		return VFIO_ERROR_IN_DEVICE;
	}
	/* Now we just get the Stream handler */
	pAccess = (STREAM_ACCESS_t *)pTunerHandle->pHStream;
	if (NULL == pAccess)
	{
		PrintError("%s(%d) Error> Tuner handle doesn't include correct STREAM Handler. Check This !!\n"
		, __FUNCTION__, __LINE__);
		return VFIO_ERROR_FAIL;
	}
    if(TRUE == pAccess->info.b_eof)
	{
		nRet = -1; /* must be -1 like feof*/
	}
//	PrintExit;
	return nRet;
}


#if __________task__________
#endif

#if __________drv__________
#endif

#if __________di__________
#endif

/**
 * @brief       register callback
 * @return      void
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
void DI_STREAM_ABR_RegisterProbeCallback (const char *pUrl)
{
	VFIO_Callback_t stCBFunction;
	PrintEnter;
	/* 1. VFIO에 callback 함수 등록 */
	(void)VK_memset (&stCBFunction, 0x0, sizeof(VFIO_Callback_t));

	stCBFunction.Open 			= (VFIOOpenCB)	P_STREAM_ABR_OpenForProbe;
	stCBFunction.Close			= (VFIOCloseCB)	P_STREAM_ABR_Close;
	stCBFunction.Read			= (VFIOReadCB)	P_STREAM_ABR_ReadForProbe;
	stCBFunction.Seek			= (VFIOSeekCB) 	P_STREAM_ABR_SeekForProbe;
	stCBFunction.Tell			= (VFIOTellCB) 	P_STREAM_ABR_Tell;
	stCBFunction.IsEOF			= (VFIOIsEOFCB) P_STREAM_ABR_IsEOF;
	stCBFunction.OpenNexus		= (VFIOOpenFromNexusCB)	 P_STREAM_ABR_OpenFromNexus;
	stCBFunction.SeekNexus		= (VFIOSeekForNexusCB) 	 P_STREAM_ABR_SeekForNexus;
	stCBFunction.BoundsNexus	= (VFIOBoundsForNexusCB) P_STREAM_ABR_BoundsForProbe;

	(void) VFIO_RegisterCallback (pUrl, stCBFunction);
	PrintExit;
	return;
}

/**
 * @brief       unregister callback
 * @return      void
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
void DI_STREAM_ABR_UnregisterProbeCallback (const char *pUrl)
{
	PrintEnter;

	PrintError("%s(%d) : Error> VFIO_UnregisterCallback : %d\n",__FUNCTION__,__LINE__,VFIO_UnregisterCallback (pUrl));

	PrintExit;
	return;
}

/**
 * @brief       register callback
 * @return      void
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
void DI_STREAM_ABR_RegisterCallback (const char *pUrl)
{
	VFIO_Callback_t stCBFunction;
	PrintEnter;
	/* 1. VFIO에 callback 함수 등록 */
	(void)VK_memset (&stCBFunction, 0x0, sizeof(VFIO_Callback_t));

	stCBFunction.Open 			= (VFIOOpenCB)	P_STREAM_ABR_Open;
	stCBFunction.Close			= (VFIOCloseCB)	P_STREAM_ABR_Close;
	stCBFunction.Read			= (VFIOReadCB)	P_STREAM_ABR_Read;
	stCBFunction.Seek			= (VFIOSeekCB) 	P_STREAM_ABR_Seek;
	stCBFunction.Tell			= (VFIOTellCB) 	P_STREAM_ABR_Tell;
	stCBFunction.IsEOF			= (VFIOIsEOFCB) P_STREAM_ABR_IsEOF;
	stCBFunction.OpenNexus		= (VFIOOpenFromNexusCB)	 P_STREAM_ABR_OpenFromNexus;
	stCBFunction.SeekNexus		= (VFIOSeekForNexusCB) 	 P_STREAM_ABR_SeekForNexus;
	stCBFunction.BoundsNexus	= (VFIOBoundsForNexusCB) P_STREAM_ABR_BoundsForNexus;
	(void) VFIO_RegisterCallback (pUrl, stCBFunction);
	PrintExit;
	return;
}

/**
 * @brief       unregister callback
 * @return      void
 * @date        2011/02/19
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
void DI_STREAM_ABR_UnregisterCallback (const char *pUrl)
{
	PrintEnter;
	(void)VFIO_UnregisterCallback (pUrl);
	PrintExit;
	return;
}
/*
void DI_STREAM_ABR_HTTP_RegisterProtocolCB(DI_IPTUNER_t *pTuner)
{
	pTuner->pProtocolCallbacks = (void*)VK_MEM_Alloc(sizeof(VFIO_Callback_t));
	VFIO_Callback_t *pCallbacks = (VFIO_Callback_t *)pTuner->pProtocolCallbacks;
	pCallbacks->Open 			= (VFIOOpenCB)	DI_STREAM_HTTP_Open;
	pCallbacks->Close			= (VFIOCloseCB)	DI_STREAM_HTTP_Close;
	pCallbacks->Read			= (VFIOReadCB)	DI_STREAM_HTTP_Read;
	pCallbacks->Seek			= (VFIOSeekCB) 	DI_STREAM_HTTP_Seek;
	pCallbacks->Tell			= (VFIOTellCB) 	DI_STREAM_HTTP_Tell;
	pCallbacks->IsEOF			= (VFIOIsEOFCB) DI_STREAM_HTTP_IsEOF;
	pCallbacks->OpenNexus		= (VFIOOpenFromNexusCB)	 DI_STREAM_HTTP_OpenFromNexus;
	pCallbacks->SeekNexus		= (VFIOSeekForNexusCB) 	 DI_STREAM_HTTP_SeekForNexus;
	pCallbacks->BoundsNexus		= (VFIOBoundsForNexusCB) DI_STREAM_HTTP_BoundsForNexus;
}

void DI_STREAM_ABR_HTTP_UnregisterProtocolCB(DI_IPTUNER_t *pTuner)
{
	VFIO_Callback_t *pCallbacks = (VFIO_Callback_t *)pTuner->pProtocolCallbacks;
	if(pCallbacks != NULL)
	{
		VK_MEM_Free(pCallbacks);
	}
}
*/
/*
int DI_STREAM_ABR_SetPosition(DI_IPTUNER_t *pTuner, HUINT64 *pullTime)
{
	int nRet = DI_ERR_OK;
	size_t sz_File=0;
	VFIO_Handle_t *pVfio = NULL;
	nRet = ABR_MPD_SetSegmentByTime(pTuner->hAbr, pullTime);
	if(nRet != DI_ERR_OK)
	{
		return nRet;
	}
	pVfio = (VFIO_Handle_t *)pTuner->pVfioHandle;
	nRet = P_STREAM_ABR_N_Segment(*pVfio, ABR_MPD_GetCurrentSegmentId(pTuner->hAbr));
	DI_STREAM_HTTP_BoundsForNexus(*pVfio,&sz_File);
	ABR_MPD_Queue_Reset(pTuner->hAbr, 0);
	ABR_MPD_Queue_PushBack(pTuner->hAbr,ABR_MPD_GetCurrentSegmentId(pTuner->hAbr),sz_File,ABR_MPD_GetCurrentBitrate(pTuner->hAbr));
	return nRet;
}*/

int DI_STREAM_ABR_SetTimeForSeek(DI_IPTUNER_t *pTunerHandle, HUINT64 ullTime, DI_MEDIA_TIME_TYPE eTimeType)
{
	PrintEnter;
	if(pTunerHandle == NULL)
	{
		PrintError("%s(%d) : Error> pTunerHandle is NULL.\n",__FUNCTION__,__LINE__);
		return ABR_FAIL;
	}
	if(pTunerHandle->hAbr == NULL)
	{
		PrintError("%s(%d) : Error> hAbr is NULL.\n",__FUNCTION__,__LINE__);
		return ABR_FAIL;
	}
	if(eTimeType == DI_MEDIA_TIME_SEC)
	{
		ullTime *= 1000;
	}
	else if(eTimeType == DI_MEDIA_TIME_USEC)
	{
		if(ullTime > 0)
		{
			ullTime /= 1000;
		}
	}
	P_STREAM_ABR_SetTimeForSeek(pTunerHandle,ullTime);
	PrintExit;
	return ABR_OK;
}

/**
 * @brief
 * @param
 * @param
 * @param
 * @return
 * @date        2011/03/09
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
int DI_STREAM_ABR_EnableWaitForDecision(DI_IPTUNER_t *pTunerHandle, HBOOL bWaitForDecision)
{
	PrintEnter;
	if(pTunerHandle == NULL)
	{
		PrintError("%s(%d) : Error> pTunerHandle is NULL.\n",__FUNCTION__,__LINE__);
		return ABR_FAIL;
	}
	if(pTunerHandle->hAbr == NULL)
	{
		PrintError("%s(%d) : Error> hAbr is NULL.\n",__FUNCTION__,__LINE__);
		return ABR_FAIL;
	}
	// pTunerHandle->tABRCtrlInfo.bWaitForDecision = bWaitForDecision;
	PrintExit;
	return ABR_OK;
}

/**
 * @brief
 * @param
 * @param
 * @param
 * @return
 * @date        2011/03/09
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
int DI_STREAM_ABR_SetABRStream(DI_IPTUNER_t *pTunerHandle, HINT32 nStreamIndex, HBOOL bDeferred)
{
	PrintEnter;
	if(pTunerHandle == NULL)
	{
		PrintError("%s(%d) : Error> pTunerHandle is NULL.\n",__FUNCTION__,__LINE__);
		return ABR_FAIL;
	}
	if(pTunerHandle->hAbr == NULL)
	{
		PrintError("%s(%d) : Error> hAbr is NULL.\n",__FUNCTION__,__LINE__);
		return ABR_FAIL;
	}
	if(bDeferred == TRUE)
	{
		/* if representations are not sorted, must change below method "ABR_MPD_SetRepresent" to another method */
		ABR_MPD_SetRepresent( pTunerHandle->hAbr, nStreamIndex);
	}
	else
	{
		//TODO:
	}
	PrintExit;
	return ABR_OK;
}

/**
 * @brief
 * @param
 * @param
 * @param
 * @return
 * @date        2011/03/09
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
int DI_STREAM_ABR_GetABRStatus(DI_IPTUNER_t *pTunerHandle) /* not implemented */
{
	PrintEnter;
	if(pTunerHandle == NULL)
	{
		PrintError("%s(%d) : Error> pTunerHandle is NULL.\n",__FUNCTION__,__LINE__);
		return ABR_FAIL;
	}
	if(pTunerHandle->hAbr == NULL)
	{
		PrintError("%s(%d) : Error> hAbr is NULL.\n",__FUNCTION__,__LINE__);
		return ABR_FAIL;
	}
	PrintExit;
	return ABR_OK;
}

/**
 * @brief
 * @param
 * @param
 * @param
 * @return
 * @date        2011/03/09
 * @author      ykkim3@humaxdigital.com
 * @remark
 * @see
 **/
int DI_STREAM_ABR_GetABRStreamSet(DI_IPTUNER_t *pTunerHandle) /* not implemented */
{
	PrintEnter;
	if(pTunerHandle == NULL)
	{
		PrintError("%s(%d) : Error> pTunerHandle is NULL.\n",__FUNCTION__,__LINE__);
		return ABR_FAIL;
	}
	if(pTunerHandle->hAbr == NULL)
	{
		PrintError("%s(%d) : Error> hAbr is NULL.\n",__FUNCTION__,__LINE__);
		return ABR_FAIL;
	}
	PrintExit;
	return ABR_OK;
}
