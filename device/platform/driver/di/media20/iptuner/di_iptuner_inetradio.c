/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   di_iptuner_inetradio.c  $
 * Version:			$Revision:   $
 * Original Author:
 * Current Author:	$Author: parkjh4@humaxdigital.com $
 * Date:			$Date:
 * File Description:	Internet Radio
 * Module:
 * Remarks:
 */

/**
 * @defgroup INTERNET_RADIO
 * @brief Internet radio such as ICECAST(aka ICY) processor
 */

/**
 * @author Jeehune Park (parkjh4@humaxdigital.com)
 * @date 1 Jun 2012
 */

/**
 * @brief Internet radio such as ICECAST(aka ICY) processor
 */

/**
 * @note
 * Copyright (C) 2012 Humax Corporation. All Rights Reserved. <br>
 * This software is the confidential and proprietary information
 * of Humax Corporation. You may not use or distribute this software
 * except in compliance with the terms and conditions of any applicable license
 * agreement in writing between Humax Corporation and you.
 */

 /*@{*/

/**
 * @file di_iptuner_inetradio.c
 */

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
/* Start Including Header Files */
#include "di_iptuner_config.h"

#include <inttypes.h>
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "bstd.h"
#include "vfio20.h"

#include "vkernel.h"
#include "taskdef.h"

#include "di_media20.h"
#include "di_uart.h"
#include "drv_media20.h"

#include "util_url.h"
#include "di_iptuner20.h"
#include "di_iptuner_pool.h"
#include "di_iptuner_http.h"
#include "di_iptuner_inetradio.h"
#include "stream_global.h"
#include "stream_http.h"
#include "adp_buffermgr.h"

/* End Including Headers */

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */
//#define DI_IPTUNER_INETRADIO_DEBUG

#define PrintFunc				DI_UART_Print
#ifdef IPTUNER_ALL_DEBUG
#ifdef DI_IPTUNER_INETRADIO_DEBUG
#define PrintData(...)			PrintFunc(PRINT_GREEN	"%s(%d) ", __FUNCTION__,__LINE__);PrintFunc(__VA_ARGS__);PrintFunc(PRINT_DEFAULT)
#define PrintDebug(...)			PrintFunc(				"%s(%d) ", __FUNCTION__,__LINE__);PrintFunc(__VA_ARGS__)
#define PrintEnter				PrintFunc(PRINT_GREY	"(+)%s\n"	PRINT_DEFAULT, __FUNCTION__)
#define PrintExit				PrintFunc(PRINT_GREY	"(-)%s\n"	PRINT_DEFAULT, __FUNCTION__)
#define PrintError(...)			PrintFunc(PRINT_MAGENTA "%s(%d) ", __FUNCTION__,__LINE__);PrintFunc(__VA_ARGS__);PrintFunc(PRINT_DEFAULT)
#define PrintCritical(fmt,...)	PrintFunc(PRINT_RED		"%s(%d) "	fmt PRINT_DEFAULT, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug		while (0) ((int (*)(char *, ...)) 0)
#define PrintEnter
#define PrintExit
#define PrintError(...)			PrintFunc(PRINT_MAGENTA "%s(%d) ", __FUNCTION__,__LINE__);PrintFunc(__VA_ARGS__);PrintFunc(PRINT_DEFAULT)
#endif

#else

#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug		while (0) ((int (*)(char *, ...)) 0)
#define PrintError		while (0) ((int (*)(char *, ...)) 0)
#define PrintEnter
#define PrintExit
#endif /* END DEBUG DEFINE */

/* Color definition */
#define PRINT_GREY						"\033[1;30;40m"		/* ENTER FUNCTION */
#define PRINT_GREEN             		"\033[1;32;40m"		/* DATA */
#define PRINT_RED             			"\033[1;31;40m"		/* CRITICAL ERR */
#define PRINT_MAGENTA       	      	"\033[1;35;40m"		/* ERROR */
#define PRINT_YELLOW             		"\033[1;33;40m"		/* CUSTOM #1 */
#define PRINT_BLUE             			"\033[1;34;40m"		/* CUSTOM #2 */
#define PRINT_CYAN       	      		"\033[1;36;40m"		/* CUSTOM #3 */
#define PRINT_WHITE             		"\033[1;37;40m"		/* CUSTOM #4 */
#define PRINT_DEFAULT                	"\033[00m"

#define UNUSED_PARAM(x)  ((void)x)

#define MAXIMUM_CONNECT_TRY	10


static int P_STREAM_INETRADIO_ReadICYMeta( STREAM_ACCESS_t *pAccess, DI_IPTUNER_t* pHTuner)
{
	PrintEnter;
    HINT8 *p=NULL;
    HINT8 *psz_meta=NULL;
	HINT8 *p_title=NULL;
    HUINT8 buffer = 0;
	HINT32 nRetry = 0;
    HINT32 i_read = 0;
	HINT32 szRead = 0;
    HINT32 szTotal = 0;

    do
    {
    	i_read = STREAM_HTTP_Read(pAccess, &buffer, 1);
        if( i_read <= 0 )
    	{
    		PrintError("ICY meta data read failed\n");
            nRetry++;
    	}
        if(pHTuner->bExitRead == TRUE)
        {
            return 1;
        }
        if(nRetry == MAXIMUM_CONNECT_TRY)
        {
            PrintError("Cannot read ICY meta data\n");
            return 0;
        }
    }
    while(i_read != 1);

	// increase i_pos adjust value
    pHTuner->nInetRadioAdjustOffset++;
	nRetry = 0;

    if( buffer == 0 )
	{
		PrintDebug("No metadata\n");
		PrintExit;
        return DI_IPT_SUCCESS;
	}

    i_read = buffer << 4;
    PrintDebug ("ICY meta size=%u\n", i_read);

    psz_meta = DD_MEM_Alloc ( i_read + 1 );
    if( psz_meta == NULL )
	{
		PrintExit;
        return DI_IPT_EGENERIC;
	}

    do
    {
	    szRead = STREAM_HTTP_Read(pAccess, (HUINT8*)psz_meta + szTotal, i_read - szTotal);
        if(szRead != i_read - szTotal)
        {
            PrintError("ICY meta data read failed\n");
            nRetry++;
        }
        if(pHTuner->bExitRead == TRUE)
        {
            return 1;
        }
        if(nRetry == MAXIMUM_CONNECT_TRY)
        {
            PrintError("Cannot read ICY meta data\n");
            return 0;
        }
        szTotal += szRead;
    }
    while( szTotal != i_read );

	pHTuner->nInetRadioAdjustOffset += i_read;

	psz_meta[i_read] = '\0'; /* Just in case */

    PrintDebug ("icy-meta=%s\n", psz_meta );

    /* Now parse the meta */
    /* Look for StreamTitle= */
    p = strcasestr ( psz_meta, "StreamTitle=" );

    if( p )
    {
        p += VK_strlen  ( "StreamTitle=" );
        if( *p == '\'' || *p == '"' )
        {
            char closing[] = { p[0], ';', '\0' };
            char *psz = VK_strstr( &p[1], closing );
            if( !psz )
			{
                psz = VK_strchr( &p[1], ';' );
			}

            if( psz )
			{
				*psz = '\0';
			}
        }
        else
        {
            char *psz = VK_strchr( &p[1], ';' );
            if( psz )
			{
				*psz = '\0';
			}
        }

		HTTP_ACCESS_SYS_t *pHttpSys = (HTTP_ACCESS_SYS_t *)pAccess->p_sys;
        if( !pHttpSys->psz_icy_title || VK_strcmp( pHttpSys->psz_icy_title, &p[1] ) )
        {
            DD_MEM_Free ( pHttpSys->psz_icy_title );
			p_title = strdup( &p[1] );
            pHttpSys->psz_icy_title = UTIL_EnsureUTF8(p_title);
			if(pHttpSys->psz_icy_title == NULL)
			{
				DD_MEM_Free(p_title);
				p_title = NULL;
				PrintDebug("New title detected, but invalid title!\n");
			}
			else
			{
            	PrintDebug ("New Title=%s\n", pHttpSys->psz_icy_title );
			}
#if 0 // for meta
            pAccess->info.i_update |= INPUT_UPDATE_META;
#endif
        }
    }

    if(psz_meta != NULL)
    {
   	 DD_MEM_Free ( psz_meta );
	    psz_meta = NULL;
    }

	PrintExit;
    return DI_IPT_SUCCESS;
}

static HUINT64 P_STREAM_INETRADIO_Download(STREAM_ACCESS_t* pAccess, HINT8* pBuffer, HINT32 szDownload, HUINT32 szDownloadUnit, DI_IPTUNER_t *pHTuner)
{
	HINT32 nReadSize = 0;
	HINT32 nRetry = 0;
	size_t szRemaining = 0;
	size_t szTry = 0;
	size_t szReal = 0;
	size_t szRealTotal = 0;
	HINT8 *pDownload = NULL;
	HTTP_ACCESS_SYS_t *pHttpSys = NULL;
	PrintEnter;

	if(pAccess == NULL)
	{
		PrintError("pAccess is NULL\n");
		return 0;
	}
	pHttpSys = (HTTP_ACCESS_SYS_t *)pAccess->p_sys;
	if(pBuffer == NULL)
	{
		PrintError("pBuffer is NULL\n");
		return 0;
	}
	pDownload = VK_MEM_Alloc(szDownloadUnit);
	if(pDownload == NULL)
	{
		PrintError("Not enough Memory\n");
		return 0;
	}

	VK_MEM_Memset(pBuffer, 0, szDownload);
	szRemaining = szDownload;


	do
	{
		if(szRemaining > szDownloadUnit)
		{

			if( (pAccess->info.i_pos - pHttpSys->i_icy_offset) % szDownloadUnit == 0)
			{
				szTry = szDownloadUnit;
			}
			else
			{
				// if current position is not at boundary, get stream from current to next boundary.
				szTry = szDownloadUnit - (pAccess->info.i_pos - pHttpSys->i_icy_offset - pHTuner->nInetRadioAdjustOffset) % szDownloadUnit;
			}
		}
		else
		{
			szTry = szRemaining;
		}
		szReal = 0;

		/* Metadata read */
		if( pHttpSys->i_icy_meta > 0 && pAccess->info.i_pos-pHttpSys->i_icy_offset > 0 )
		{
			PrintDebug ("i_icy_meta =%d, i_icy_offset = %"PRId64", i_pos = %"PRId64", i_size = %"PRId64"\n",
										pHttpSys->i_icy_meta, pHttpSys->i_icy_offset, pAccess->info.i_pos, pAccess->info.i_size);

			HINT64 i_next = pHttpSys->i_icy_meta - (pAccess->info.i_pos - pHttpSys->i_icy_offset - pHTuner->nInetRadioAdjustOffset) % pHttpSys->i_icy_meta;

			if( i_next == pHttpSys->i_icy_meta )
			{
				PrintDebug(PRINT_BLUE"read Meta! (current pos : %"PRId64"\n"PRINT_DEFAULT, pAccess->info.i_pos);
				if( P_STREAM_INETRADIO_ReadICYMeta( pAccess, pHTuner ) )
				{
					PrintDebug ("Error INT_HTTP_ReadICYMeta\n", __FUNCTION__,__LINE__);

					pAccess->info.b_eof = TRUE;
					PrintDebug ("stream_read returns for EOF\n",__FUNCTION__,__LINE__);
					VK_MEM_Free(pDownload);
					pDownload = NULL;
					PrintExit;
					return 0;
				}
			}
		}

		do
		{
			if(pHTuner->bExitRead == TRUE)
			{
				VK_MEM_Free(pDownload);
				pDownload = NULL;
				PrintExit;
				return szRealTotal;
			}
			/* Read stream data */
			nReadSize = STREAM_HTTP_Read(pAccess, (HUINT8*)pDownload, szTry-szReal);
			if(nReadSize <= 0 || nReadSize > (HINT32)szTry-(HINT32)szReal)
			{
				if(nRetry >= MAXIMUM_CONNECT_TRY)
				{
					PrintError("Stop!! STREAM_HTTP_Read Fail !!!\n");
					VK_MEM_Free(pDownload);
					pDownload = NULL;
					PrintExit;
					return 0;
				}
				PrintError("STREAM_HTTP_Read Fail... Retry...!!!\n");
				nRetry++;
				continue;
			}
			VK_MEM_Memcpy(pBuffer + szRealTotal, pDownload, nReadSize);
#if DEBUG_DUMP
			FILE *fp = fopen("/download_icy.bin","a+");
			if(fp==NULL) exit(1);
			fwrite((void*)pDownload, sizeof(HINT8), nReadSize, fp);
			VK_fflush(fp);
			fclose(fp);
#endif
			szReal += nReadSize;
			szRealTotal += nReadSize;
			nReadSize = 0;
		}
		while(szReal < szTry);
		szRemaining = szRemaining - szReal;
	}
	while(szRemaining > 0);

	VK_MEM_Free(pDownload);
	pDownload = NULL;

	PrintExit;
	return szRealTotal;
}

/**
 * @b Function @b Description <br>
 * Probe for internet radio <br>
 * This does not perfome real probing, just fill the internet radio structure with constant information<br>
 *
 * @param[in] pHTuner	: Tuner handle
 * @param[out] stMediaInfo : Media info structure
 * @param[in] bNeedHttpConnect : If Tuner has already valid connection, this shall be false
 *
 * @return  #  Error code <br>
 */
DI_ERR_CODE DI_STREAM_INETRADIO_Probe(DI_IPTUNER_t *pHTuner, DI_MEDIA_INFO_t *stMediaInfo, HBOOL bNeedHttpConnect)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	HTTP_ACCESS_SYS_t *pstHttpSys=NULL;
	STREAM_ACCESS_t *ptHttpAccess=NULL;
	VFIO_Handle_t stHandle;

	PrintEnter;
	UNUSED_PARAM(pHTuner);

	/* check Tuner Handle */
	if ( NULL == pHTuner )
	{
		PrintError ("Error> Tuner Handle is NULL !\n");
		nRet = DI_ERR_INVALID_PARAM;
		return nRet;
	}

	/* check Tuner url */
	if ( NULL == pHTuner->pUrl)
	{
		PrintError ("Error> Tuner Handle has invalid url (pUrl is NULL)\n");
		nRet = DI_ERR_INVALID_PARAM;
		return nRet;
	}

	if ( NULL == stMediaInfo)
	{
		PrintError ("Error> Media Info is NULL !\n");
		nRet = DI_ERR_INVALID_PARAM;
		return nRet;
	}

	PrintData ("TunerHandle=[%p] URL=[%s]\n", pHTuner, pHTuner->pUrl);

	if(bNeedHttpConnect)
	{
		VK_MEM_Memset(&stHandle, 0x0, sizeof(stHandle));
	}

	pHTuner->tInetRadioInfo.ulInetRadioSemId = 0;
	pHTuner->tInetRadioInfo.lInetRadioOffset = 0;
	pHTuner->tInetRadioInfo.nInetRadioReadSize = 0;
	pHTuner->tInetRadioInfo.nStartOffset = 0;
	pHTuner->tInetRadioInfo.nEndOffset = 0;
	pHTuner->tInetRadioInfo.nCurrentBufferOffset = 0;
	pHTuner->tInetRadioInfo.pcInetRadioReadBuffer=NULL;
	pHTuner->tInetRadioInfo.pcInetRadioReadTempBuffer=NULL;

	if(bNeedHttpConnect)
	{
		/*	http open to get server infomation */
		if(VFIO_ERROR_SUCCESS != DI_STREAM_HTTP_Open(pHTuner->pUrl, NULL, &stHandle))
		{
			PrintError("Error> DI_STREAM_HTTP_Open fail.\n");
			nRet = DI_ERR_ERROR;
			return nRet;
		}

		if(pHTuner->pHStream == NULL)
		{
			PrintError("Error> stHandle.pDnpUserData is null.\n");
			DI_STREAM_HTTP_Close(stHandle);
			PrintExit;
			return DI_ERR_ERROR;
		}
	}

	ptHttpAccess = (STREAM_ACCESS_t*) pHTuner->pHStream;
	pstHttpSys = ptHttpAccess->p_sys;


	if(bNeedHttpConnect)
	{
		if(pstHttpSys == NULL)
		{
			PrintError("Error> pstHttpSys is null.\n");
			DI_STREAM_HTTP_Close(stHandle);
			PrintExit;
			return DI_ERR_ERROR;
		}
	}

	PrintData (" : i_update		= %d \n",ptHttpAccess->info.i_update);
	PrintData (" : i_size 		= %"PRId64"\n",ptHttpAccess->info.i_size);
	PrintData (" : i_pos			= %"PRId64"\n",ptHttpAccess->info.i_pos);
	PrintData (" : b_eof			= %d \n",ptHttpAccess->info.b_eof);
	PrintData (" : i_title		= %d \n",ptHttpAccess->info.i_title);
	PrintData (" : i_seekpoint	= %d \n",ptHttpAccess->info.i_seekpoint);
	PrintData (" : b_prebuffered	= %d \n",ptHttpAccess->info.b_prebuffered);
	PrintData (" : fd 			 = %d \n",pstHttpSys->fd);
	PrintData (" : i_vresion		 = %d \n",pstHttpSys->i_version);
	PrintData (" : i_remaining	 = %"PRId64"\n",pstHttpSys->i_remaining);
	PrintData (" : b_seekable 	 = %d \n",pstHttpSys->b_seekable);
	PrintData (" : b_reconnect	 = %d \n",pstHttpSys->b_reconnect);
	PrintData (" : b_continuous	 = %d \n",pstHttpSys->b_continuous);
	PrintData (" : b_pace_control  = %d \n",pstHttpSys->b_pace_control);
	PrintData (" : b_persist		 = %d \n",pstHttpSys->b_persist);

	PrintData (" : b_icecast				= %d \n",pstHttpSys->b_icecast);
	PrintData (" : i_icy_meta 			 = %d \n",pstHttpSys->i_icy_meta);
	PrintData (" : i_icy_offset		= %"PRId64" \n",pstHttpSys->i_icy_offset);
	PrintData (" : psz_icy_name	  = %s\n",pstHttpSys->psz_icy_name);
	PrintData (" : psz_icy_genre		= %s \n",pstHttpSys->psz_icy_genre);
	PrintData (" : psz_icy_title	   = %s \n",pstHttpSys->psz_icy_title);
	PrintData (" : e_inetradio_codec  = %d \n",pstHttpSys->e_inetradio_codec);
	PrintData (" : i_icy_br	   = %d \n",pstHttpSys->i_icy_br);
	PrintData (" : i_icy_pub	   = %d \n",pstHttpSys->i_icy_pub);
	PrintData (" : i_icy_url	   = %s \n",pstHttpSys->i_icy_url);

	/* Common configuration */
	stMediaInfo->stream_type = DI_MEDIA_MPEG_TYPE_ES;
	stMediaInfo->duration = 0xFFFFFFFF; 	  /* duration of stream, in milliseconds or 0 if unknown */
	stMediaInfo->nprograms =1;		/* total number of programs */
	stMediaInfo->ntracks = 1;		 /* total number of tracks */
	stMediaInfo->video_pes_id = 0;
	stMediaInfo->video_stream_id = 0;	 /* or Video PID */
	stMediaInfo->video_codec = DI_VIDEO_FORMAT_NONE; //DI_MEDIA_VIDEO_FORMAT_VC1;
	stMediaInfo->audio_pes_id = 192;
	stMediaInfo->audio_stream_id = 1;	 /* or Audio PID */
	stMediaInfo->audio_codec = pHTuner->tInetRadioInfo.eInetRadioCodec;

	if(pHTuner->tInetRadioInfo.eInetRadioCodec == 0)
	{
		stMediaInfo->audio_codec = DI_AUDFORMAT_MP3;
		stMediaInfo->max_bitrate = 0;	 /* maximum stream bitreate, bps or 0 if unknown  */
	}
	else
	{
		stMediaInfo->audio_codec = pHTuner->tInetRadioInfo.eInetRadioCodec;
	}

	stMediaInfo->ucTotalAudioTrack = 1;
	stMediaInfo->stAudioTrack[0].audio_pes_id = stMediaInfo->audio_pes_id;
	stMediaInfo->stAudioTrack[0].audio_stream_id = stMediaInfo->audio_stream_id;
	stMediaInfo->stAudioTrack[0].audio_codec = stMediaInfo->audio_codec;

	stMediaInfo->support_trick = 0;
	stMediaInfo->support_seek = 0;
	stMediaInfo->pcr_pid = 8191;		/* for TS stream */

	/* Specific Configuration */
	switch(pHTuner->eInetRadioType)
	{
		case DI_IPTUNER_INETRADIO_TYPE_GENERIC:
			stMediaInfo->isIcyProtocol = TRUE;
			stMediaInfo->max_bitrate = 0;		//Unknown bitrate
			pHTuner->tInetRadioInfo.nReadUnit = INETRADIO_GENERIC_READ_SIZE;
			break;

		case DI_IPTUNER_INETRADIO_TYPE_ICY:
			stMediaInfo->isIcyProtocol = TRUE;
			stMediaInfo->max_bitrate = pHTuner->tInetRadioInfo.nInetRadioBitrate*1024;
			if(pHTuner->tInetRadioInfo.nReadUnit == 0)
			{
				pHTuner->tInetRadioInfo.nReadUnit = INETRADIO_GENERIC_READ_SIZE;
			}
			if(pHTuner->pInetRadioTitle != NULL)
			{
				VK_MEM_Free(pHTuner->pInetRadioTitle);
				pHTuner->pInetRadioTitle = NULL;
			}
			pHTuner->pInetRadioTitle = VK_MEM_Alloc(MAX_TITLE_LENGTH);
			VK_memset (pHTuner->pInetRadioTitle, 0, MAX_TITLE_LENGTH);

			break;

		case DI_IPTUNER_INETRADIO_TYPE_NONE:
			stMediaInfo->stream_type = DI_MEDIA_MPEG_TYPE_ASF;
			stMediaInfo->max_bitrate = 875042;	  /* maximum stream bitreate, bps or 0 if unknown	*/
			stMediaInfo->duration = 304833;    /* duration of stream, in milliseconds or 0 if unknown */
			stMediaInfo->nprograms =1;	 /* total number of programs */
			stMediaInfo->ntracks = 2;		  /* total number of tracks */
			stMediaInfo->video_pes_id = 224;
			stMediaInfo->video_stream_id = 2;	  /* or Video PID */
			stMediaInfo->video_codec = 9; //DI_MEDIA_VIDEO_FORMAT_VC1;
			stMediaInfo->audio_pes_id = 192;
			stMediaInfo->audio_stream_id = 1;	  /* or Audio PID */
			stMediaInfo->audio_codec = 14;
			stMediaInfo->support_trick = 0;
			stMediaInfo->support_seek = 1;
			stMediaInfo->pcr_pid = 8191;		 /* for TS stream */
			stMediaInfo->isIcyProtocol = FALSE;
			break;

		default:
			break;
	}

	if(bNeedHttpConnect)
	{
		DI_STREAM_HTTP_Close(stHandle);
	}

	PrintExit;
	return nRet;
}

DI_ERR_CODE DI_STREAM_INETRADIO_Generic_Open(const HINT8 *pUrl, const HINT8 *mode, VFIO_Handle_t *ptHandle)
{
		UNUSED_PARAM(pUrl);
		UNUSED_PARAM(mode);
		UNUSED_PARAM(ptHandle);

		/* This function is not used */
		// 1. Internet radio is recognized by HTTP connection check - in HTTP protocol procedure
		// 2. If internet radio, change protocol type and probe
		// 3. While probe is working, media structure is filled manually
		// 4. So this open function(for probe) is not called

		PrintExit;
		return DI_IPT_SUCCESS;
}

DI_ERR_CODE DI_STREAM_INETRADIO_Generic_Close(VFIO_Handle_t tHandle)
{
		HINT32 vRet = VFIO_ERROR_SUCCESS;
		STREAM_ACCESS_t *pAccess=NULL;
		DI_IPTUNER_t *pHTuner = NULL;

		PrintEnter;

		pHTuner = (DI_IPTUNER_t*)tHandle.pTunerHandle;

		if((unsigned int)pHTuner->nData == tHandle.tFile.callback.unHandle)
		{
			PrintDebug("StreamData release\n");
			if((STREAM_ACCESS_t*)pHTuner->pHStreamData != NULL)
			{
				pAccess = (STREAM_ACCESS_t*)pHTuner->pHStreamData;
				pHTuner->pHStreamData = NULL;
				pHTuner->nData = 0;
			}
			else
			{
				PrintError("Error> tHandle->pHStream is null \n");
				PrintExit;
				return VFIO_ERROR_IN_DEVICE;
			}

			if(pHTuner->tInetRadioInfo.ulInetRadioSemId != 0)
			{
				(void)VK_SEM_Get(pHTuner->tInetRadioInfo.ulInetRadioSemId);
			}

			if(pHTuner->tInetRadioInfo.pcInetRadioReadBuffer != NULL)
			{
				VK_MEM_Free (pHTuner->tInetRadioInfo.pcInetRadioReadBuffer);
				pHTuner->tInetRadioInfo.pcInetRadioReadBuffer = NULL;
			}
			if(pHTuner->pInetRadioTitle != NULL)
			{
				VK_MEM_Free(pHTuner->pInetRadioTitle);
				pHTuner->pInetRadioTitle = NULL;
			}
			if(pHTuner->tInetRadioInfo.ulInetRadioSemId != 0)
			{
				(void)VK_SEM_Release(pHTuner->tInetRadioInfo.ulInetRadioSemId);
				(void)VK_SEM_Destroy(pHTuner->tInetRadioInfo.ulInetRadioSemId);
				pHTuner->tInetRadioInfo.ulInetRadioSemId = 0;
			}
		}
		else if((unsigned int)pHTuner->nIndex == tHandle.tFile.callback.unHandle)
		{
			PrintDebug("Indexer release\n");
			if((STREAM_ACCESS_t*)pHTuner->pHStreamIndex != NULL)
			{
				pAccess = (STREAM_ACCESS_t*)pHTuner->pHStreamIndex;
				pHTuner->pHStreamIndex = NULL;
				pHTuner->nIndex = 0;
			}
			else
			{
				PrintError("Error> tHandle->pHStream is null \n");
				PrintExit;
				return VFIO_ERROR_IN_DEVICE;
			}
		}
		else
		{
			PrintDebug("Common Stream handler release\n");
			if(pHTuner->pHStream != NULL)
			{
				pAccess = (STREAM_ACCESS_t *)pHTuner->pHStream;
				pHTuner->pHStream = NULL;
			}
			else
			{
				PrintError ("Error> tHandle->pHStream is null \n");
				return VFIO_ERROR_IN_DEVICE;
			}
		}

		PrintDebug("Release Complete\n");
		/* In multi-open case, the http session should be closed only once. */
		if(pAccess->b_alive == FALSE)
		{
			PrintDebug (": HTTP Session is already closed.\n");
			return VFIO_ERROR_IN_DEVICE;
		}

		(void) STREAM_HTTP_Close (pAccess);

		pAccess->b_alive = FALSE;

		if(pAccess->psz_path != NULL)
		{
			PrintDebug("psz_path : %s\n", pAccess->psz_path);
			VK_MEM_Free(pAccess->psz_path);
			pAccess->psz_path = NULL;
		}
		if(pAccess->psz_user_agent != NULL)
		{
			PrintDebug("psz_user_agent : %s\n", pAccess->psz_user_agent);
			VK_MEM_Free(pAccess->psz_user_agent);
			pAccess->psz_user_agent = NULL;
		}
		/* else { continue; } */
#if SUPPORT_HUMAX_COOKIE
		if(pAccess->psz_cookie != NULL)
		{
			PrintDebug("psz_cookie : %s\n", pAccess->psz_cookie);
			VK_MEM_Free(pAccess->psz_cookie);
			pAccess->psz_cookie = NULL;
		}
		/* else { continue; } */
#endif

#if 1
		if(pAccess != NULL)
		{
			PrintDebug("pAccess = %x\n", pAccess);
			VK_MEM_Free(pAccess);
			pAccess = NULL;
		}
		/* else { continue; } */
#endif

		PrintExit;
		return vRet;

}

DI_ERR_CODE DI_STREAM_INETRADIO_Generic_Read(VFIO_Handle_t tHandle, void *ptr, size_t size, size_t nmemb, ssize_t *ptReadSize)
{
    UNUSED_PARAM (tHandle);
    STREAM_ACCESS_t *pAccess=NULL;
    DI_IPTUNER_t *pHTuner=NULL;
    HUINT32 unReadSize = 0;
    HUINT32 unRequestReadSize = 0;
    HUINT32 unRemainReadSize = 0;
    HINT32  nTempReadSize = 0;
    HUINT32 unErrrorCount = 0;
    PrintEnter;

    pHTuner = (DI_IPTUNER_t*)tHandle.pTunerHandle;
    if((unsigned int)pHTuner->nData == tHandle.tFile.callback.unHandle)
    {
        if((STREAM_ACCESS_t*)pHTuner->pHStreamData != NULL)
        {
            pAccess = (STREAM_ACCESS_t*)pHTuner->pHStreamData;
        }
        else
        {
            PrintError ("%s(%d) Error> tHandle->pHStreamData is null \n",__FUNCTION__,__LINE__);
            return VFIO_ERROR_IN_DEVICE;
        }
    }
    else if((unsigned int)pHTuner->nIndex == tHandle.tFile.callback.unHandle)
    {
        if((STREAM_ACCESS_t*)pHTuner->pHStreamIndex != NULL)
        {
            pAccess = (STREAM_ACCESS_t*)pHTuner->pHStreamIndex;
        }
        else
        {
            PrintError ("%s(%d) Error> tHandle->pHStreamIndex is null \n",__FUNCTION__,__LINE__);
            return VFIO_ERROR_IN_DEVICE;
        }
    }
    else
    {
        if(pHTuner->pHStream != NULL)
        {
            pAccess = (STREAM_ACCESS_t *)pHTuner->pHStream;
        }
        else
        {
            PrintError ("%s(%d) Error> tHandle->pHStream is null \n",__FUNCTION__,__LINE__);
            return VFIO_ERROR_IN_DEVICE;
        }
    }

    (void)VK_SEM_Get(pHTuner->tInetRadioInfo.ulInetRadioSemId);

    /* already checked */
    unReadSize = size*nmemb;

    /* unReadSize : read size requested by player */
    PrintData("%s(%d) : size(%u), nmemb(%u), unReadSize(%u)\n",__FUNCTION__,__LINE__, size, nmemb, unReadSize);
    PrintData("%s(%d) : i_pos(%"PRId64"), lInetRadioOffset(%"PRId64")\n",__FUNCTION__,__LINE__, pAccess->info.i_pos, pHTuner->tInetRadioInfo.lInetRadioOffset);

    /*
        i_pos : current Stream_access offset
        lInetRadioOffset : current Internet Radio offset
        현재 내부에서 버퍼링 된 오프셋 보다 http position이 더 큰 경우 (normal)
    */
    if(pAccess->info.i_pos > pHTuner->tInetRadioInfo.lInetRadioOffset)
    {
        /* known InetRadio data size is less than remaining buffer size */
        if((pHTuner->tInetRadioInfo.lInetRadioOffset + unReadSize) <= pHTuner->tInetRadioInfo.nEndOffset)
        {
            VK_MEM_Memcpy(ptr, pHTuner->tInetRadioInfo.pcInetRadioReadBuffer+(pHTuner->tInetRadioInfo.lInetRadioOffset-pHTuner->tInetRadioInfo.nStartOffset), unReadSize);

            *ptReadSize = unReadSize;

            PrintData("%s(%d) : tryRead(%d) realRead(%d)\n",__FUNCTION__,__LINE__,unReadSize, *ptReadSize);

            (void)VK_SEM_Release(pHTuner->tInetRadioInfo.ulInetRadioSemId);
            PrintExit;
            return VFIO_ERROR_SUCCESS;
        }
        /* greater than buffer size */
        else
        {
            unRemainReadSize = (pHTuner->tInetRadioInfo.lInetRadioOffset + unReadSize) - pHTuner->tInetRadioInfo.nEndOffset;
            VK_MEM_Memcpy(ptr, pHTuner->tInetRadioInfo.pcInetRadioReadBuffer+(pHTuner->tInetRadioInfo.lInetRadioOffset-pHTuner->tInetRadioInfo.nStartOffset), (unReadSize-unRemainReadSize));
        }
    }

    else if(pAccess->info.i_pos < pHTuner->tInetRadioInfo.lInetRadioOffset)
    {
        PrintError("%s (%d) : Error> Not Support Seek\n",__FUNCTION__,__LINE__);
        *ptReadSize = 0;
        (void)VK_SEM_Release(pHTuner->tInetRadioInfo.ulInetRadioSemId);
        return VFIO_ERROR_FAIL;
    }
    else
    {
        unRemainReadSize = unReadSize;
    }

    /* buffer size validation */
    if((pHTuner->tInetRadioInfo.nEndOffset - pHTuner->tInetRadioInfo.nStartOffset) == (HUINT32)pHTuner->tInetRadioInfo.nReadUnit*MAX_READ_BUFFER_SIZE_IN_BUFFER)
    {
        pHTuner->tInetRadioInfo.pcInetRadioReadTempBuffer = VK_MEM_Alloc(pHTuner->tInetRadioInfo.nReadUnit*MAX_READ_BUFFER_SIZE_IN_BUFFER/2);
        if(pHTuner->tInetRadioInfo.pcInetRadioReadTempBuffer == NULL)
        {
            PrintError("%s (%d) : Error> Fail Mem Alloc\n",__FUNCTION__,__LINE__);
            *ptReadSize = 0;
            (void)VK_SEM_Release(pHTuner->tInetRadioInfo.ulInetRadioSemId);
            return VFIO_ERROR_FAIL;
        }

        VK_MEM_Memset(pHTuner->tInetRadioInfo.pcInetRadioReadTempBuffer, 0x0, pHTuner->tInetRadioInfo.nReadUnit*MAX_READ_BUFFER_SIZE_IN_BUFFER/2);
        VK_MEM_Memcpy(pHTuner->tInetRadioInfo.pcInetRadioReadTempBuffer, pHTuner->tInetRadioInfo.pcInetRadioReadBuffer+pHTuner->tInetRadioInfo.nReadUnit*MAX_READ_BUFFER_SIZE_IN_BUFFER/2, pHTuner->tInetRadioInfo.nReadUnit*MAX_READ_BUFFER_SIZE_IN_BUFFER/2);
        VK_MEM_Memset(pHTuner->tInetRadioInfo.pcInetRadioReadBuffer, 0x0, pHTuner->tInetRadioInfo.nReadUnit*MAX_READ_BUFFER_SIZE_IN_BUFFER);
        VK_MEM_Memcpy(pHTuner->tInetRadioInfo.pcInetRadioReadBuffer, pHTuner->tInetRadioInfo.pcInetRadioReadTempBuffer, pHTuner->tInetRadioInfo.nReadUnit*MAX_READ_BUFFER_SIZE_IN_BUFFER/2);
        pHTuner->tInetRadioInfo.nStartOffset = pHTuner->tInetRadioInfo.nStartOffset + pHTuner->tInetRadioInfo.nReadUnit*MAX_READ_BUFFER_SIZE_IN_BUFFER/2;
        pHTuner->tInetRadioInfo.nCurrentBufferOffset = pHTuner->tInetRadioInfo.nReadUnit*MAX_READ_BUFFER_SIZE_IN_BUFFER/2;

        VK_MEM_Free(pHTuner->tInetRadioInfo.pcInetRadioReadTempBuffer);
        pHTuner->tInetRadioInfo.pcInetRadioReadTempBuffer = NULL;
    }

    /* STREAM_HTTP_Read함수 호출시 nReadUnit값과 같은 Data Size가 읽히거나 아니면 0이 되어야 함. 아닌 경우는 Fail */
    unRequestReadSize = pHTuner->tInetRadioInfo.nReadUnit;
    pHTuner->tInetRadioInfo.nInetRadioReadSize = 0;

    /* Read size of nReadUnit */
    while(pHTuner->tInetRadioInfo.nInetRadioReadSize != pHTuner->tInetRadioInfo.nReadUnit)
    {
        nTempReadSize = STREAM_HTTP_Read (pAccess, pHTuner->tInetRadioInfo.pcInetRadioReadBuffer+pHTuner->tInetRadioInfo.nCurrentBufferOffset, unRequestReadSize );
        if(pHTuner->bExitRead == TRUE)
        {
            PrintDebug("Exit request\n");
            (void)VK_SEM_Release(pHTuner->tInetRadioInfo.ulInetRadioSemId);
            PrintExit;
            return VFIO_ERROR_SUCCESS;
        }
        if(nTempReadSize == DI_IPT_ETIMEOUT)
        {
            if(unErrrorCount == 2)
            {
                pHTuner->tInetRadioInfo.nInetRadioReadSize = -1;
                break;
            }

            unErrrorCount++;
        }
        else if(nTempReadSize == 0)
        {
            /* if chunked-TE and last-chunk, */
            if(pAccess->info.b_eof == TRUE)
            {
                PrintDebug("%s(%d) Debug> EndOfStream Occured in Steram HTTP Controller !!\n", __FUNCTION__, __LINE__);
                break;
            }
            else
            {
                if(unErrrorCount == 10)
                {
                    pHTuner->tInetRadioInfo.nInetRadioReadSize = -1;
                    break;
                }
                VK_TASK_Sleep(100);
                unErrrorCount++;
            }
        }
        else if(nTempReadSize < 0)
        {
            pHTuner->tInetRadioInfo.nInetRadioReadSize = -1;
            break;
        }
        else
        {
            unErrrorCount = 0;
        }

        unRequestReadSize = unRequestReadSize - nTempReadSize;
        pHTuner->tInetRadioInfo.nCurrentBufferOffset = pHTuner->tInetRadioInfo.nCurrentBufferOffset+nTempReadSize;
        pHTuner->tInetRadioInfo.nInetRadioReadSize = pHTuner->tInetRadioInfo.nInetRadioReadSize + nTempReadSize;
    }

    if(pHTuner->tInetRadioInfo.nInetRadioReadSize == pHTuner->tInetRadioInfo.nReadUnit)
    {
        if(unReadSize == unRemainReadSize)
        {
            if(unRemainReadSize < (HUINT32)pHTuner->tInetRadioInfo.nReadUnit)
            {
                (void)VK_MEM_Memcpy(ptr, pHTuner->tInetRadioInfo.pcInetRadioReadBuffer+(pHTuner->tInetRadioInfo.nCurrentBufferOffset-pHTuner->tInetRadioInfo.nInetRadioReadSize), unRemainReadSize);
                /* 읽으려고 하는 InetRadio Data Size가 nReadUnit보다 작을 경우 수행 */
                *ptReadSize = unReadSize;
            }
            else
            {
                (void)VK_MEM_Memcpy(ptr, pHTuner->tInetRadioInfo.pcInetRadioReadBuffer+(pHTuner->tInetRadioInfo.nCurrentBufferOffset-pHTuner->tInetRadioInfo.nInetRadioReadSize), pHTuner->tInetRadioInfo.nInetRadioReadSize);

                /* 읽으려고 하는 InetRadio Data Size가 nReadUnit보다 같거나 큰 경우 수행 */
                *ptReadSize = pHTuner->tInetRadioInfo.nInetRadioReadSize;
            }
        }
        else
        {
            if(unRemainReadSize < (HUINT32)pHTuner->tInetRadioInfo.nReadUnit)
            {
                (void)VK_MEM_Memcpy((char *)ptr+(unReadSize-unRemainReadSize), pHTuner->tInetRadioInfo.pcInetRadioReadBuffer+(pHTuner->tInetRadioInfo.nCurrentBufferOffset-pHTuner->tInetRadioInfo.nInetRadioReadSize), unRemainReadSize);
                /* 읽으려고 하는 InetRadio Data Size가 nReadUnit보다 작을 경우 수행 */
                *ptReadSize = unReadSize;
            }
            else
            {
                (void)VK_MEM_Memcpy((char *)ptr+(unReadSize-unRemainReadSize), pHTuner->tInetRadioInfo.pcInetRadioReadBuffer+(pHTuner->tInetRadioInfo.nCurrentBufferOffset-pHTuner->tInetRadioInfo.nInetRadioReadSize), pHTuner->tInetRadioInfo.nInetRadioReadSize);

                /* 읽으려고 하는 Icy Data Size가 IcyMetaInt보다 같거나 큰 경우 수행 */
                *ptReadSize = pHTuner->tInetRadioInfo.nInetRadioReadSize+(unReadSize-unRemainReadSize);
            }
        }

        /* End offset update */
        pHTuner->tInetRadioInfo.nEndOffset = pHTuner->tInetRadioInfo.nEndOffset+pHTuner->tInetRadioInfo.nInetRadioReadSize;
    }
    else if(pHTuner->tInetRadioInfo.nInetRadioReadSize < 0)
    {
        PrintError("%s (%d) : Error> Read Error \n",__FUNCTION__,__LINE__);
        *ptReadSize = 0;
        (void)VK_SEM_Release(pHTuner->tInetRadioInfo.ulInetRadioSemId);
        return VFIO_ERROR_FAIL;
    }
    else
    {
        PrintError("%s(%d) : Error> STREAM_HTTP_Read Fail.\n",__FUNCTION__,__LINE__);
        *ptReadSize = 0;
    }

    PrintData("%s(%d) : tryRead(%d) realRead(%d)\n",__FUNCTION__,__LINE__,unReadSize, *ptReadSize);

    (void)VK_SEM_Release(pHTuner->tInetRadioInfo.ulInetRadioSemId);
    PrintExit;

    return VFIO_ERROR_SUCCESS;

}
DI_ERR_CODE DI_STREAM_INETRADIO_Generic_Seek(VFIO_Handle_t tHandle, off_t offset, int whence)
{
	HINT32 nRet;
	STREAM_ACCESS_t *pAccess=NULL;
	DI_IPTUNER_t *pHTuner=NULL;

	UNUSED_PARAM (tHandle);
	PrintEnter;

	PrintData (" : offset=%"PRId64", whence=%d\n", offset, whence);

	pHTuner = (DI_IPTUNER_t*)tHandle.pTunerHandle;

	if((unsigned int)pHTuner->nData == tHandle.tFile.callback.unHandle)
	{
		if(pHTuner->pHStreamData != NULL)
		{
			pAccess = (STREAM_ACCESS_t *)pHTuner->pHStreamData;
		}
		else
		{
			PrintError ("Error> tHandle->pHStream is null \n");
			return VFIO_ERROR_IN_DEVICE;
		}
	}
	else if((unsigned int)pHTuner->nIndex == tHandle.tFile.callback.unHandle)
	{
		if(pHTuner->pHStreamIndex != NULL)
		{
			pAccess = (STREAM_ACCESS_t *)pHTuner->pHStreamIndex;
		}
		else
		{
			PrintError ("Error> tHandle->pHStream is null \n");
			return VFIO_ERROR_IN_DEVICE;
		}
	}
	else
	{
		if((STREAM_ACCESS_t *)pHTuner->pHStream != NULL)
		{
			pAccess = (STREAM_ACCESS_t *)pHTuner->pHStream;
		}
		else
		{
			PrintError ("Error> tHandle->pHStream is null \n");
			return VFIO_ERROR_IN_DEVICE;
		}
	}

	nRet = STREAM_HTTP_Seek (pAccess, offset );
	if(nRet != DI_IPT_SUCCESS)
	{
		PrintError ("Error> STREAM_HTTP_Seek fail \n");
		return -1;
	}
	PrintExit;
	return offset;
}

DI_ERR_CODE DI_STREAM_INETRADIO_Generic_Tell(VFIO_Handle_t tHandle, off_t *plCurOffset)
{
	HINT32 nRet = VFIO_ERROR_SUCCESS;

	UNUSED_PARAM (tHandle);
	UNUSED_PARAM (plCurOffset);

	PrintEnter;
	PrintExit;
	return nRet;

}

DI_ERR_CODE DI_STREAM_INETRADIO_Generic_IsEOF(VFIO_Handle_t tHandle)
{
	HINT32 nRet = VFIO_ERROR_SUCCESS;
	STREAM_ACCESS_t *pAccess=NULL;
	DI_IPTUNER_t *pHTuner=NULL;

	UNUSED_PARAM (tHandle);
	PrintEnter;

	pHTuner = (DI_IPTUNER_t*)tHandle.pTunerHandle;

	if(pHTuner == NULL)
	{
		PrintError ("Error> tHandle.pTunerHandle is null \n");
		return VFIO_ERROR_IN_DEVICE;
	}

	if((unsigned int)pHTuner->nData == tHandle.tFile.callback.unHandle)
	{
		if(pHTuner->pHStreamData != NULL)
		{
			pAccess = (STREAM_ACCESS_t *)pHTuner->pHStreamData;
		}
		else
		{
			PrintError ("Error> tHandle->pHStream is null \n");
			return VFIO_ERROR_IN_DEVICE;
		}
	}
	else if((unsigned int)pHTuner->nIndex == tHandle.tFile.callback.unHandle)
	{
		if(pHTuner->pHStreamIndex != NULL)
		{
			pAccess = (STREAM_ACCESS_t *)pHTuner->pHStreamIndex;
		}
		else
		{
			PrintError ("Error> tHandle->pHStream is null \n");
			return VFIO_ERROR_IN_DEVICE;
		}
	}
	else
	{
		if((STREAM_ACCESS_t *)pHTuner->pHStream != NULL)
		{
			pAccess = (STREAM_ACCESS_t *)pHTuner->pHStream;
		}
		else
		{
			PrintError ("Error> tHandle->pHStream is null \n");
			return VFIO_ERROR_IN_DEVICE;
		}
	}

    if(TRUE == pAccess->info.b_eof)
	{
		nRet = -1; /* must be -1 like feof*/
	}

	PrintExit;
	return nRet;
}

DI_ERR_CODE DI_STREAM_INETRADIO_Generic_OpenFromNexus(const HINT8 *pUrl, const unsigned int flags, VFIO_Handle_t *ptHandle)
{
	UNUSED_PARAM(flags);
	UNUSED_PARAM(ptHandle);
	HINT32 nRet = DI_IPT_SUCCESS;
	HINT32 result = 0;
	DI_IPTUNER_t *pHTuner = NULL;
	HTTP_ACCESS_SYS_t *pstHttpSys = NULL;
	STREAM_ACCESS_t *pAccess = NULL;
#if SUPPORT_HUMAX_COOKIE
		char *psCookie = NULL;
		URL_t tUrl;
#endif

	PrintEnter;
	pHTuner = DI_IPTUNER_POOL_FindByUrl((HINT8*)pUrl);
	if(pHTuner == NULL)
	{
		ptHandle->ucAvailable = 0;
		PrintError(" : Error> can't find iptuner handler by this URL(%s)\n", pUrl);
		return VFIO_ERROR_IN_DEVICE;
	}
	if(pUrl==NULL)
	{
		ptHandle->ucAvailable = 0;
		PrintError(" : Error> invalid pUrl(%s)\n",pUrl);
		return VFIO_ERROR_IN_DEVICE;
	}

	pAccess = (STREAM_ACCESS_t*)VK_MEM_Alloc(sizeof(STREAM_ACCESS_t));
	if(pAccess == NULL)
	{
		PrintError("Error> can't allocate STREAM handler\n");
		return VFIO_ERROR_IN_DEVICE;
	}
	else
	{
		VK_MEM_Memset(pAccess,0,sizeof(STREAM_ACCESS_t));
	}

#if SUPPORT_HUMAX_COOKIE
	VK_MEM_Memset(&tUrl, 0x0, sizeof(tUrl));
	UTIL_UrlParse(&tUrl, (const char *)pUrl, 0);
	if(tUrl.psz_cookie != NULL)
	{
		psCookie = VK_StrDup(tUrl.psz_cookie);
	}
	UTIL_UrlClean(&tUrl);
#endif

	ptHandle->pTunerHandle = (void*) pHTuner;

	/** psz_path must be free */
	pAccess->psz_path = VK_StrDup ( (void *) UTIL_GetOriginalURL ((const char *)pUrl) );

#if SUPPORT_HUMAX_COOKIE
	if(psCookie != NULL)
	{
		pAccess->psz_cookie = psCookie;
	}
#endif

	/* support YouView Specific UserAgent String */
	if(pHTuner->pUserAgentString != NULL)
	{
		pAccess->psz_user_agent = VK_StrDup ((void *)pHTuner->pUserAgentString);
	}

	if((pHTuner->nData == 0))
	{
		pHTuner->pHStreamData = (STREAM_ACCESS_t *)pAccess;
	}
	else if((pHTuner->nData != 0) && (pHTuner->nIndex == 0))	/* open for Indexer */
	{
		pHTuner->pHStreamIndex = (STREAM_ACCESS_t *)pAccess;
	}
	else
	{
		PrintError(" : Invalid Open.\n");
#if SUPPORT_HUMAX_COOKIE
		if(psCookie != NULL)
		{
			VK_MEM_Free(psCookie);
			psCookie = NULL;
		}
#endif
		if(pAccess->psz_path != NULL)
		{
			VK_MEM_Free(pAccess->psz_path);
		}

		if(pAccess != NULL)
		{
			VK_MEM_Free(pAccess);
		}

		return VFIO_ERROR_IN_DEVICE;
	}

	nRet = STREAM_HTTP_Open (pAccess);
	if(nRet != DI_IPT_SUCCESS)
	{
		PrintError(" : Error> STREAM_HTTP_Open Fail.\n");
		ptHandle->ucAvailable = 0;
	}
	else
	{
		pstHttpSys = (HTTP_ACCESS_SYS_t *)pAccess->p_sys;
		ptHandle->tFile.callback.unHandle = (HUINT32) pstHttpSys->fd;
		if(pHTuner->nData == 0) 	/* open for Player */
		{
			pAccess->b_alive = TRUE;
			pHTuner->nData = (HUINT32) pstHttpSys->fd;
			result = VK_SEM_Create(&pHTuner->tInetRadioInfo.ulInetRadioSemId,"inetradio", DEFAULT_SUSPEND_TYPE);

			if( result != VK_OK )
			{
				PrintError("[%s] [%d] Error(0x%x) \n\r", result);
			}
			else
			{
				pHTuner->tInetRadioInfo.pcInetRadioReadBuffer = VK_MEM_Alloc((pHTuner->tInetRadioInfo.nReadUnit*MAX_READ_BUFFER_SIZE_IN_BUFFER)+1);
				if( pHTuner->tInetRadioInfo.pcInetRadioReadBuffer == NULL )
				{
					PrintError("[%s] [%d] Error> Fail Mem alloc \n\r",__FUNCTION__,__LINE__);
					nRet = VFIO_ERROR_IN_DEVICE;
				}
				else
				{
					VK_MEM_Memset(pHTuner->tInetRadioInfo.pcInetRadioReadBuffer, 0x0, (pHTuner->tInetRadioInfo.nReadUnit*MAX_READ_BUFFER_SIZE_IN_BUFFER)+1);
					pHTuner->tInetRadioInfo.nStartOffset = 0;
					pHTuner->tInetRadioInfo.nEndOffset = 0;
					pHTuner->tInetRadioInfo.nCurrentBufferOffset = 0;
				}
			}
		}
		else if((pHTuner->nData != 0) && (pHTuner->nIndex == 0)) 	/* open for Indexer */
		{
			pAccess->b_alive = TRUE;
			pHTuner->nIndex = (HUINT32) pstHttpSys->fd;
		}
		ptHandle->ucAvailable = 1;
		PrintData (" : callback handle=0x%x\n", ptHandle->tFile.callback.unHandle);
		PrintDebug("pAccess = %x\n", pAccess);
	}

	if(nRet != VFIO_ERROR_SUCCESS)
	{
		if(pAccess != NULL)
		{
			STREAM_HTTP_Close(pAccess);
		}

		if(pAccess->psz_path != NULL)
		{
			VK_MEM_Free(pAccess->psz_path);
			pAccess->psz_path =NULL;
		}
		if(pAccess->psz_user_agent != NULL)
		{
			VK_MEM_Free(pAccess->psz_user_agent);
			pAccess->psz_user_agent =NULL;
		}
		/* else { continue; } */
#if SUPPORT_HUMAX_COOKIE
		if(pAccess->psz_cookie != NULL)
		{
			VK_MEM_Free(pAccess->psz_cookie);
			pAccess->psz_cookie =NULL;
		}
		/* else { continue; } */
#endif
	}

	PrintExit;
	return VFIO_ERROR_SUCCESS;
}

off_t DI_STREAM_INETRADIO_Generic_SeekForNexus(VFIO_Handle_t tHandle, off_t offset, int whence)
{
	UNUSED_PARAM (tHandle);
	UNUSED_PARAM (whence);

	DI_IPTUNER_t *pHTuner=NULL;

	PrintEnter;
	PrintData (" : offset=%"PRId64", whence=%d\n", offset, whence);

	pHTuner = (DI_IPTUNER_t*)tHandle.pTunerHandle;

	if(pHTuner == NULL)
	{
		PrintError ("Error> tHandle.pTunerHandle is null \n");
		return VFIO_ERROR_IN_DEVICE;
	}

	(void)VK_SEM_Get(pHTuner->tInetRadioInfo.ulInetRadioSemId);

	//this is only used when not used buffer manager
	pHTuner->tInetRadioInfo.lInetRadioOffset = offset;

	(void)VK_SEM_Release(pHTuner->tInetRadioInfo.ulInetRadioSemId);

	PrintExit;

	return pHTuner->tInetRadioInfo.lInetRadioOffset;
}

DI_ERR_CODE DI_STREAM_INETRADIO_Generic_BoundsForNexus(VFIO_Handle_t tHandle, off_t *file_size)
{
	HINT32 nRet = VFIO_ERROR_SUCCESS;
	STREAM_ACCESS_t *pAccess=NULL;
	DI_IPTUNER_t *pHTuner=NULL;

	UNUSED_PARAM (tHandle);
	PrintEnter;

	pHTuner = (DI_IPTUNER_t*)tHandle.pTunerHandle;

	if(pHTuner == NULL)
	{
		PrintError ("Error> tHandle.pTunerHandle is null \n");
		return VFIO_ERROR_IN_DEVICE;
	}

	if((unsigned int)pHTuner->nData == tHandle.tFile.callback.unHandle)
	{
		if(pHTuner->pHStreamData != NULL)
		{
			pAccess = (STREAM_ACCESS_t *)pHTuner->pHStreamData;
		}
		else
		{
			PrintError ("Error> tHandle->pHStream is null \n");
			return VFIO_ERROR_IN_DEVICE;
		}
	}
	else if((unsigned int)pHTuner->nIndex == tHandle.tFile.callback.unHandle)
	{
		if(pHTuner->pHStreamIndex != NULL)
		{
			pAccess = (STREAM_ACCESS_t *)pHTuner->pHStreamIndex;
		}
		else
		{
			PrintError ("Error> tHandle->pHStream is null \n");
			return VFIO_ERROR_IN_DEVICE;
		}
	}
	else
	{
		if((STREAM_ACCESS_t *)pHTuner->pHStream != NULL)
		{
			pAccess = (STREAM_ACCESS_t *)pHTuner->pHStream;
		}
		else
		{
			PrintError ("Error> tHandle->pHStream is null \n");
			return VFIO_ERROR_IN_DEVICE;
		}
	}

	PrintData (" : i_size=%"PRId64"\n", pAccess->info.i_size);

	*file_size = pAccess->info.i_size;

	PrintExit;
	return nRet;

}

DI_ERR_CODE DI_STREAM_INETRADIO_ICY_Read(VFIO_Handle_t tHandle, void *ptr, size_t size, size_t nmemb, ssize_t *ptReadSize)
{
		UNUSED_PARAM (tHandle);
		HTTP_ACCESS_SYS_t *pHttpSys = NULL;
		STREAM_ACCESS_t *pAccess=NULL;
		DI_IPTUNER_t *pHTuner=NULL;
		HUINT32 unReadSize = 0;
		HUINT32 unRequestReadSize = 0;
		HUINT32 unRemainReadSize = 0;
		HINT32 	nTempReadSize = 0;
		HUINT32 unErrrorCount = 0;
		PrintEnter;

		pHTuner = (DI_IPTUNER_t*)tHandle.pTunerHandle;

		if((unsigned int)pHTuner->nData == tHandle.tFile.callback.unHandle)
		{
			if((STREAM_ACCESS_t*)pHTuner->pHStreamData != NULL)
			{
				pAccess = (STREAM_ACCESS_t*)pHTuner->pHStreamData;
			}
			else
			{
				PrintError ("Error> tHandle->pHStreamData is null \n");
				PrintExit;
				return VFIO_ERROR_IN_DEVICE;
			}
		}
		else if((unsigned int)pHTuner->nIndex == tHandle.tFile.callback.unHandle)
		{
			if((STREAM_ACCESS_t*)pHTuner->pHStreamIndex != NULL)
			{
				pAccess = (STREAM_ACCESS_t*)pHTuner->pHStreamIndex;
				PrintExit;
				return VFIO_ERROR_SUCCESS;
			}
			else
			{
				PrintError ("Error> tHandle->pHStreamIndex is null \n");
				PrintExit;
				return VFIO_ERROR_IN_DEVICE;
			}
		}
		else
		{
			if(pHTuner->pHStream != NULL)
			{
				pAccess = (STREAM_ACCESS_t *)pHTuner->pHStream;
			}
			else
			{
				PrintError ("Error> tHandle->pHStream is null \n");
				PrintExit;
				return VFIO_ERROR_IN_DEVICE;
			}
		}
		(void)VK_SEM_Get(pHTuner->tInetRadioInfo.ulInetRadioSemId);

		/* Required data size by player*/
		unReadSize = size*nmemb;

		PrintData(" : size(%u), nmemb(%u), unReadSize(%u)\n", size, nmemb, unReadSize);
		PrintData(" : i_pos(%"PRId64"), lInetRadioOffset(%"PRId64")\n", pAccess->info.i_pos, pHTuner->tInetRadioInfo.lInetRadioOffset);

		if(pAccess->info.i_pos > pHTuner->tInetRadioInfo.lInetRadioOffset +  pHTuner->nInetRadioAdjustOffset)
		{
			/* known icy data size is less than remaining buffer size */
			if((pHTuner->tInetRadioInfo.lInetRadioOffset + unReadSize) <= pHTuner->tInetRadioInfo.nEndOffset)
			{
				VK_MEM_Memcpy(ptr, pHTuner->tInetRadioInfo.pcInetRadioReadBuffer+(pHTuner->tInetRadioInfo.lInetRadioOffset-pHTuner->tInetRadioInfo.nStartOffset), unReadSize);

				*ptReadSize = unReadSize;

				PrintData("%s(%d) : tryRead(%d) realRead(%d)\n",__FUNCTION__,__LINE__,unReadSize, *ptReadSize);

				(void)VK_SEM_Release(pHTuner->tInetRadioInfo.ulInetRadioSemId);
				PrintExit;
				return VFIO_ERROR_SUCCESS;
			}
			/* greater than buffer size */
			else
			{
				unRemainReadSize = (pHTuner->tInetRadioInfo.lInetRadioOffset + unReadSize) - pHTuner->tInetRadioInfo.nEndOffset;
				VK_MEM_Memcpy(ptr, pHTuner->tInetRadioInfo.pcInetRadioReadBuffer+(pHTuner->tInetRadioInfo.lInetRadioOffset-pHTuner->tInetRadioInfo.nStartOffset), (unReadSize-unRemainReadSize));
			}
		}
		else if(pAccess->info.i_pos < pHTuner->tInetRadioInfo.lInetRadioOffset)
		{
			PrintError("%s (%d) : Error> Not Support Seek\n",__FUNCTION__,__LINE__);
			*ptReadSize = 0;
			(void)VK_SEM_Release(pHTuner->tInetRadioInfo.ulInetRadioSemId);
			return VFIO_ERROR_FAIL;
		}
		else
		{
			unRemainReadSize = unReadSize;
		}

		if((pHTuner->tInetRadioInfo.nEndOffset - pHTuner->tInetRadioInfo.nStartOffset) == (HUINT32)pHTuner->tInetRadioInfo.nReadUnit*MAX_READ_BUFFER_SIZE_IN_BUFFER)
		{
			pHTuner->tInetRadioInfo.pcInetRadioReadTempBuffer = VK_MEM_Alloc(pHTuner->tInetRadioInfo.nReadUnit*MAX_READ_BUFFER_SIZE_IN_BUFFER/2);
			if(pHTuner->tInetRadioInfo.pcInetRadioReadTempBuffer == NULL)
			{
				PrintError("%s (%d) : Error> Fail Mem Alloc\n",__FUNCTION__,__LINE__);
				*ptReadSize = 0;
				(void)VK_SEM_Release(pHTuner->tInetRadioInfo.ulInetRadioSemId);
				return VFIO_ERROR_FAIL;
			}

			VK_MEM_Memset(pHTuner->tInetRadioInfo.pcInetRadioReadTempBuffer, 0x0, pHTuner->tInetRadioInfo.nReadUnit*MAX_READ_BUFFER_SIZE_IN_BUFFER/2);
			VK_MEM_Memcpy(pHTuner->tInetRadioInfo.pcInetRadioReadTempBuffer, pHTuner->tInetRadioInfo.pcInetRadioReadBuffer+pHTuner->tInetRadioInfo.nReadUnit*MAX_READ_BUFFER_SIZE_IN_BUFFER/2, pHTuner->tInetRadioInfo.nReadUnit*MAX_READ_BUFFER_SIZE_IN_BUFFER/2);
			VK_MEM_Memset(pHTuner->tInetRadioInfo.pcInetRadioReadBuffer, 0x0, pHTuner->tInetRadioInfo.nReadUnit*MAX_READ_BUFFER_SIZE_IN_BUFFER);
			VK_MEM_Memcpy(pHTuner->tInetRadioInfo.pcInetRadioReadBuffer, pHTuner->tInetRadioInfo.pcInetRadioReadTempBuffer, pHTuner->tInetRadioInfo.nReadUnit*MAX_READ_BUFFER_SIZE_IN_BUFFER/2);
			pHTuner->tInetRadioInfo.nStartOffset = pHTuner->tInetRadioInfo.nStartOffset + pHTuner->tInetRadioInfo.nReadUnit*MAX_READ_BUFFER_SIZE_IN_BUFFER/2;
			pHTuner->tInetRadioInfo.nCurrentBufferOffset = pHTuner->tInetRadioInfo.nReadUnit*MAX_READ_BUFFER_SIZE_IN_BUFFER/2;

			VK_MEM_Free(pHTuner->tInetRadioInfo.pcInetRadioReadTempBuffer);
			pHTuner->tInetRadioInfo.pcInetRadioReadTempBuffer = NULL;
		}

		/* STREAM_HTTP_Read함수 호출시 nReadUnit값과 같은 Data Size가 읽히거나 아니면 0이 되어야 함. 아닌 경우는 Fail */
		unRequestReadSize = pHTuner->tInetRadioInfo.nReadUnit;
		pHTuner->tInetRadioInfo.nInetRadioReadSize = 0;

		/* Read size of nReadUnit */
		while(pHTuner->tInetRadioInfo.nInetRadioReadSize != pHTuner->tInetRadioInfo.nReadUnit)
		{
			//nTempReadSize = STREAM_HTTP_Read (pAccess, pHTuner->tInetRadioInfo.pcInetRadioReadBuffer+pHTuner->tInetRadioInfo.nCurrentBufferOffset, unRequestReadSize );
			nTempReadSize = P_STREAM_INETRADIO_Download(pAccess, pHTuner->tInetRadioInfo.pcInetRadioReadBuffer+pHTuner->tInetRadioInfo.nCurrentBufferOffset, unRequestReadSize, pHTuner->tInetRadioInfo.nReadUnit, pHTuner);
            if(pHTuner->bExitRead == TRUE)
            {
                PrintDebug("Exit request\n");
                (void)VK_SEM_Release(pHTuner->tInetRadioInfo.ulInetRadioSemId);
                PrintExit;
                return VFIO_ERROR_SUCCESS;
            }

			if(nTempReadSize == DI_IPT_ETIMEOUT)
			{
				if(unErrrorCount == 2)
				{
					pHTuner->tInetRadioInfo.nInetRadioReadSize = -1;
					break;
				}

				unErrrorCount++;
			}
			else if(nTempReadSize == 0)
			{
				/* if chunked-TE and last-chunk, */
				if(pAccess->info.b_eof == TRUE)
				{
					PrintDebug("%s(%d) Debug> EndOfStream Occured in Steram HTTP Controller !!\n", __FUNCTION__, __LINE__);
					break;
				}
				else
				{
					if(unErrrorCount == 10)
					{
						pHTuner->tInetRadioInfo.nInetRadioReadSize = -1;
						break;
					}
					VK_TASK_Sleep(100);
					unErrrorCount++;
				}
			}
			else if(nTempReadSize < 0)
			{
				pHTuner->tInetRadioInfo.nInetRadioReadSize = -1;
				break;
			}
			else
			{
				unErrrorCount = 0;
			}
#ifdef DOWNLOAD_DUMP
			FILE *fp = fopen("/download_dump.bin","a+");
			if(fp==NULL) exit(1);
			fwrite((void*)pHTuner->tInetRadioInfo.pcInetRadioReadBuffer+pHTuner->tInetRadioInfo.nCurrentBufferOffset,
				sizeof(HINT8), unRequestReadSize, fp);
			VK_fflush(fp);
			fclose(fp);
#endif
			unRequestReadSize = unRequestReadSize - nTempReadSize;
			pHTuner->tInetRadioInfo.nCurrentBufferOffset = pHTuner->tInetRadioInfo.nCurrentBufferOffset + nTempReadSize;
			pHTuner->tInetRadioInfo.nInetRadioReadSize = pHTuner->tInetRadioInfo.nInetRadioReadSize + nTempReadSize;
		}

		pHttpSys = (HTTP_ACCESS_SYS_t *)pAccess->p_sys;
		if(pHttpSys->psz_icy_title != NULL)
		{
			/* If title is changed, update title string in Tuner */
			if(VK_strcmp(pHTuner->pInetRadioTitle, pHttpSys->psz_icy_title) != 0)
			{
				PrintData("%s(%d) : pHttpSys.psz_icy_title=%s, s_tTitle=%s #####\n", __FUNCTION__,__LINE__, pHttpSys->psz_icy_title, pHTuner->pInetRadioTitle);
				if(VK_strlen(pHttpSys->psz_icy_title) <= MAX_TITLE_LENGTH)
				{
					VK_MEM_Memset(pHTuner->pInetRadioTitle, 0x0, MAX_TITLE_LENGTH);
					VK_MEM_Memcpy(pHTuner->pInetRadioTitle, pHttpSys->psz_icy_title, VK_strlen(pHttpSys->psz_icy_title));
					if(pHTuner->tInetRadioChangeTitleCallback != NULL)
					{
						pHTuner->tInetRadioChangeTitleCallback((HUINT32)pHTuner);
					}
				}
				else
				{
					PrintError("%s (%d) : Error> New Title size is more than MAX_TITLE_LENGTH.(New Title Size= %d)\n",__FUNCTION__,__LINE__, VK_strlen(pHttpSys->psz_icy_title));
				}
			}
		}

		if(pHTuner->tInetRadioInfo.nInetRadioReadSize == pHTuner->tInetRadioInfo.nReadUnit)
		{
			if(unReadSize == unRemainReadSize)
			{
				if(unRemainReadSize < (HUINT32)pHTuner->tInetRadioInfo.nReadUnit)
				{
					(void)VK_MEM_Memcpy(ptr, pHTuner->tInetRadioInfo.pcInetRadioReadBuffer+(pHTuner->tInetRadioInfo.nCurrentBufferOffset-pHTuner->tInetRadioInfo.nInetRadioReadSize), unRemainReadSize);
					/* 읽으려고 하는 Icy Data Size가 IcyMetaInt보다 작을 경우 수행 */
					*ptReadSize = unReadSize;
				}
				else
				{
					(void)VK_MEM_Memcpy(ptr, pHTuner->tInetRadioInfo.pcInetRadioReadBuffer+(pHTuner->tInetRadioInfo.nCurrentBufferOffset-pHTuner->tInetRadioInfo.nInetRadioReadSize), pHTuner->tInetRadioInfo.nInetRadioReadSize);

					/* 읽으려고 하는 Icy Data Size가 IcyMetaInt보다 같거나 큰 경우 수행 */
					*ptReadSize = pHTuner->tInetRadioInfo.nInetRadioReadSize;
				}
			}
			else
			{
				if(unRemainReadSize < (HUINT32)pHTuner->tInetRadioInfo.nReadUnit)
				{
					(void)VK_MEM_Memcpy((char *)ptr+(unReadSize-unRemainReadSize), pHTuner->tInetRadioInfo.pcInetRadioReadBuffer+(pHTuner->tInetRadioInfo.nCurrentBufferOffset-pHTuner->tInetRadioInfo.nInetRadioReadSize), unRemainReadSize);
					/* 읽으려고 하는 Icy Data Size가 IcyMetaInt보다 작을 경우 수행 */
					*ptReadSize = unReadSize;
				}
				else
				{
					(void)VK_MEM_Memcpy((char *)ptr+(unReadSize-unRemainReadSize), pHTuner->tInetRadioInfo.pcInetRadioReadBuffer+(pHTuner->tInetRadioInfo.nCurrentBufferOffset-pHTuner->tInetRadioInfo.nInetRadioReadSize), pHTuner->tInetRadioInfo.nInetRadioReadSize);

					/* 읽으려고 하는 Icy Data Size가 IcyMetaInt보다 같거나 큰 경우 수행 */
					*ptReadSize = pHTuner->tInetRadioInfo.nInetRadioReadSize+(unReadSize-unRemainReadSize);
				}
			}

			pHTuner->tInetRadioInfo.nEndOffset = pHTuner->tInetRadioInfo.nEndOffset+pHTuner->tInetRadioInfo.nInetRadioReadSize;
		}
		else if(pHTuner->tInetRadioInfo.nInetRadioReadSize < 0)
		{
			PrintError("%s (%d) : Error> Read Error \n",__FUNCTION__,__LINE__);
			*ptReadSize = -1;
			(void)VK_SEM_Release(pHTuner->tInetRadioInfo.ulInetRadioSemId);
			return VFIO_ERROR_FAIL;
		}
		else
		{
			PrintError("%s(%d) : Error> STREAM_HTTP_Read Fail.\n",__FUNCTION__,__LINE__);
			*ptReadSize = 0;
		}

		PrintData("%s(%d) : tryRead(%d) realRead(%d)\n",__FUNCTION__,__LINE__,unReadSize, *ptReadSize);

		(void)VK_SEM_Release(pHTuner->tInetRadioInfo.ulInetRadioSemId);

		PrintExit;

		return VFIO_ERROR_SUCCESS;


}

void DI_STREAM_INETRADIO_Generic_RegisterCallback (const char *pUrl)
{
	VFIO_Callback_t stCBFunction;

	/* 1. VFIO에 callback 함수 등록 */
	(void)VK_memset (&stCBFunction, 0x0, sizeof(VFIO_Callback_t));

	stCBFunction.Open			= (VFIOOpenCB)	DI_STREAM_INETRADIO_Generic_Open;
	stCBFunction.Close			= (VFIOCloseCB)	DI_STREAM_INETRADIO_Generic_Close;
	stCBFunction.Read			= (VFIOReadCB)	DI_STREAM_INETRADIO_Generic_Read;
	stCBFunction.Seek			= (VFIOSeekCB) 	DI_STREAM_INETRADIO_Generic_Seek;
	stCBFunction.Tell			= (VFIOTellCB) 	DI_STREAM_INETRADIO_Generic_Tell;
	stCBFunction.IsEOF			= (VFIOIsEOFCB) DI_STREAM_INETRADIO_Generic_IsEOF;
	stCBFunction.OpenNexus		= (VFIOOpenFromNexusCB)	 DI_STREAM_INETRADIO_Generic_OpenFromNexus;
	stCBFunction.SeekNexus		= (VFIOSeekForNexusCB) 	 DI_STREAM_INETRADIO_Generic_SeekForNexus;
	stCBFunction.BoundsNexus	= (VFIOBoundsForNexusCB) DI_STREAM_INETRADIO_Generic_BoundsForNexus;

	(void) VFIO_RegisterCallback (pUrl, stCBFunction);

	return;
}

void DI_STREAM_INETRADIO_Generic_UnregisterCallback (const char *pUrl)
{
	PrintEnter;

	(void)VFIO_UnregisterCallback (pUrl);

	PrintExit;
	return;
}

void DI_STREAM_INETRADIO_ICY_RegisterCallback (const char *pUrl)
{
	VFIO_Callback_t stCBFunction;

	/* 1. VFIO에 callback 함수 등록 */
	(void)VK_memset (&stCBFunction, 0x0, sizeof(VFIO_Callback_t));

	stCBFunction.Open			= (VFIOOpenCB)	DI_STREAM_INETRADIO_Generic_Open;
	stCBFunction.Close			= (VFIOCloseCB)	DI_STREAM_INETRADIO_Generic_Close;
	stCBFunction.Read			= (VFIOReadCB)	DI_STREAM_INETRADIO_ICY_Read;
	stCBFunction.Seek			= (VFIOSeekCB) 	DI_STREAM_INETRADIO_Generic_Seek;
	stCBFunction.Tell			= (VFIOTellCB) 	DI_STREAM_INETRADIO_Generic_Tell;
	stCBFunction.IsEOF			= (VFIOIsEOFCB) DI_STREAM_INETRADIO_Generic_IsEOF;
	stCBFunction.OpenNexus		= (VFIOOpenFromNexusCB)	 DI_STREAM_INETRADIO_Generic_OpenFromNexus;
	stCBFunction.SeekNexus		= (VFIOSeekForNexusCB) 	 DI_STREAM_INETRADIO_Generic_SeekForNexus;
	stCBFunction.BoundsNexus	= (VFIOBoundsForNexusCB) DI_STREAM_INETRADIO_Generic_BoundsForNexus;

	(void) VFIO_RegisterCallback (pUrl, stCBFunction);

	return;
}

void DI_STREAM_INETRADIO_ICY_UnregisterCallback (const char *pUrl)
{
	PrintEnter;

	(void)VFIO_UnregisterCallback (pUrl);

	PrintExit;
	return;
}

