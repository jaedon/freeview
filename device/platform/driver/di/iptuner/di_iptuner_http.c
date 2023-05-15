/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   di_iptuner_http.c  $
 * Version:			$Revision:   1.0  $
 * Original Author:	SungYong Jeon
 * Current Author:	$Author: junsy@humaxdigital.com $
 * Date:			$Date:   Tue May 20 21:49:01 KST 2009 $
 * File Description:	     HTTP DNP Receiver & HTTP VFIO for Streaming
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

#include "di_iptuner_config.h"

#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <inttypes.h>	 /** for 64bit MACRO ex>PRId64 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "htype.h"
#include "di_uart.h"

#include "vkernel.h"
#include "taskdef.h"

#include "di_iptuner.h"
#include "di_iptuner_stream.h"
#include "di_iptuner_dnp.h"
#include "di_iptuner_http.h"

#include "vfio.h"
#include "stream_http.h"
#include "drv_err.h"

/* End Including Headers */
/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */
//#define DI_IPTUNER_HTTP_DEBUG
//#define FILE_TEST

#define INTEGRITY_STREAM_CHECK_WITH_COMPARING_LOCAL_FILE 0 /* set 1 if you want to check buffering iptuner with local file */
#define INTEGRITY_STREAM_CHECK_WITH_COMPARING_LOCAL_FILE_NAME "/fanta1.wmv"

/* DEBUG DEFINE */
#ifdef IPTUNER_ALL_DEBUG

#ifdef DI_IPTUNER_HTTP_DEBUG
#define PrintData		DI_UART_Print
#define PrintDebug		DI_UART_Print
#define PrintEnter		DI_UART_Print("(+)%s\n", __FUNCTION__)
#define PrintExit		DI_UART_Print("(-)%s\n", __FUNCTION__)
#define PrintError		DI_UART_Print
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
#endif /* END DEBUG DEFINE */

#define UNUSED_PARAM(x)  ((void)x)

#define MAX_HTTP_RECV_BUFF_SIZE (2048*3)	/** 6KB */
#define NEXUS_ALIGN_BUFFER_SIZE (1024*10)
#define DI_IPTUNER_HTTP_SEEK_RETRY_COUNT (3)

#if defined(HDR_FOX_T2)
#define MAX_PDL_BUFFERING_PERCENT	5999
#else
#define MAX_PDL_BUFFERING_PERCENT	99999
#endif

/* End #define */
/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */
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
#if defined(USE_RAMBUFFERING_MOOVBOX_CACHING_SUPPORT) || defined(USE_MOOVBOX_CACHING)
static MOOVBOX_INSTANCE_t stMoovBox;
#endif
#if REAL_TIME_CHUNKBUFFER_SUPPORT || REAL_TIME_INDEXBUFFER_SUPPORT || REAL_TIME_MULTISOCKET_SUPPORT
static STREAMING_MEDIA_INASTANCE_t	s_tMP4MoovInstance;
static STREAMING_MEDIA_INASTANCE_t	s_tWMVInstance;
static DI_MEDIA_STREAM_TYPE_e s_eStream_type = DI_MEDIA_MPEG_TYPE_UNKNOWN;
static STREAMING_MULTISOCKET_HANDLE_t s_tMultiHandle;

#if REAL_TIME_CHUNKBUFFER_SUPPORT
static STREAMING_MEDIA_INASTANCE_t 	tRealtime_Buffer_Chunk[REALTIME_PLAYBUFFER_CHUNK_TOTAL];
#endif

#if REAL_TIME_INDEXBUFFER_SUPPORT
static STREAMING_MEDIA_INASTANCE_t 	tRealtime_ProbeBuffer_Chunk[REALTIME_PROBEBUFFER_CHUNK_TOTAL];
static HINT64 	s_ullCurrent_ProbeBuffer_Index=0;
static HUINT32 	s_nProbeBuffer_TotalUsedMemory=0;
static HUINT32 	s_nMoovBuffer_TotalUsedMemory=0;
#endif
static HUINT8	s_bDrmMode = FALSE;
static HINT64 	s_ullCurrent_PlayCurPosition=0;
static HINT64 	s_ullCurrent_IndexCurPosition=0;
static char *pcRedirectionUrl = NULL;
#endif

#if STREAM_BUFFERING
#define PROBE_OPEN 0
#define NEXUS_OPEN 1
static HINT32 gOpenStatus = PROBE_OPEN;
static HINT32 sOpenFromNexusCounter = 0;
static STREAM_ACCESS_t stHttpAccessForIndexer ;
static HUINT8 	*g_HttpBuffer=NULL;
static HINT32	LOWEST_HTTP_BUFFERING_THRESHOLD;
static HINT32	RESUME_HTTP_BUFFERING_THRESHOLD;
static HINT32	HIGHEST_HTTP_BUFFERING_THRESHOLD;
static HINT32 	HTTP_BUF_START;
static HINT32 	HTTP_BUF_END;
static HUINT8 	*g_httpptrW=NULL;
static HUINT8 	*g_httpptrR=NULL;
static HINT64	sOffsetStart=0;
static HINT64	sOffsetEnd=0;
static HINT32	g_HTTP_BUFFERING_SIZE;
#define HTTP_READ_BLOCK_SIZE (64*1024) // 64Kbytes
static unsigned long	s_ulPreBufferingLock;
static HINT64 sStreamBufferingRequestedSeekPoint;
typedef enum {
	DI_IPTUNER_HTTP_PREBUFFERING_TASK_STATE_STARTED,
	DI_IPTUNER_HTTP_PREBUFFERING_TASK_STATE_KILLING,
	DI_IPTUNER_HTTP_PREBUFFERING_TASK_STATE_DONE
} DI_IPTUNER_HTTP_PREBUFFERING_TASK_STATE_e;
static DI_IPTUNER_HTTP_PREBUFFERING_TASK_STATE_e s_ePreBufferingTaskState = DI_IPTUNER_HTTP_PREBUFFERING_TASK_STATE_DONE;
static DI_IPTUNER_HTTP_PREBUFFERING_TASK_STATE_e s_eChangePreBufferingTaskState = DI_IPTUNER_HTTP_PREBUFFERING_TASK_STATE_DONE;
static HBOOL s_NetworkError = FALSE;
#endif

/* 기존에는 STREAM_BUFFER인 경우만 사용했으나, REAL TIME인 경우에도 사용할 수 있도록 이동 */
static HINT64	s_nFileLength = 0;
/* 현재의 Read Position을 구해서, 컨텐츠의 끝 부분을 재생할 때 언더플로우에 빠지는 조건을 방지 */
static HINT64	s_llCurrentReadPosition = 0;

#if	INTEGRITY_STREAM_CHECK_WITH_COMPARING_LOCAL_FILE
int fd;
#endif

static STREAM_ACCESS_t stHttpAccess ;
static HINT32 s_nIsEventSend=0;
static HINT64 nSumLength=0;
static HINT32 s_nHttpStopReceiving=FALSE;
static HINT32 s_nHttpPauseReceiving=FALSE;

static unsigned long	s_ulHttpSemId=0;

#if 1	/* JWKIM, workaround: for Maxdome Service */
static int s_nIsPrebufferingCompleted = 1;
#endif

#ifdef ICY_PROTOCOL
static STREAMING_ICYInstance_t s_tIcyInstance;	/* Icy Protocol Instance */
#endif

#ifdef FILE_TEST
static int	s_TempFd;
static HUINT8	s_bFileWriteComplete=FALSE;
#endif

static int s_nWfd = -1;
#ifdef SUPPORT_TITLE
static HINT8 s_tTitle[MAX_TITLE_LENGTH];
#endif


static DI_IPTUNER_EventCallback_t s_tEventCallback;


#if STREAM_BUFFERING
static DI_IPTUNER_PlayCallback_t stHttpBufferingCallback;
static HBOOL bPausedForBuffering=FALSE;
static HUINT32 gUnderFlowLimit;
#if 0 /* Commented: warning: 'gPreBufferingTaskId' defined but not used */
static HULONG gPreBufferingMsgId;
static HULONG gPreBufferingTaskId;
#endif
#define DI_HTTP_STREAM_MSG_SIZE MSGQ_SIZE_NORMAL
#endif

static HBOOL P_STREAM_HTTP_UseHttpsProtocol(STREAM_ACCESS_t *pAccess)
{
	if ( pAccess == NULL ) return FALSE;
	if ( pAccess->psz_path == NULL ) return FALSE;

	return ( VK_strncmp(pAccess->psz_path, "https", 5) == 0 ) ? TRUE : FALSE;
}

/* End static variable */
/*******************************************************************/
/************************ static funtions **************************/
/*******************************************************************/
#if __________p__________
#endif

#define LENGTH_OF_BOX_SIZE 4
#define LENGTH_OF_BOX_TYPE 4
#define CACHING_TIMEOUT_RETRYCOUNT 10
#define CACHING_UNIT_SIZE 60*1024

#if 0
HUINT8 uiBoxTypes[10][4] = {{'f','t','y','p'},
							{'p','d','i','n'},
							{'m','o','o','v'},
							{'m','o','o','f'},
							{'m','f','r','a'},
							{'m','d','a','t'},
							{'f','r','e','e'},
							{'s','k','i','p'},
							{'m','e','t','a'},
							{'m','e','c','o'}};
void P_IPTUNER_HTTP_MoovCaching(HUINT8* pUrl)
{
	HBOOL bMoovFound=FALSE;
	HINT32 iResult;
	size_t sz_Try=0;
	size_t sz_Real=0;
	HUINT8* pSizeTypeBuf=NULL;
	HUINT8* pBuf=NULL;
	HUINT8* pTempBuf=NULL;
	HUINT32 uiBoxSize=0;
	HUINT32 uiErrorCount=0;
	HUINT32 uiIndex=0;
	HUINT32 uiOffset=0;
	HUINT32 uiCachedSize=0;
	STREAM_ACCESS_t tAccess;

	if(stMoovBox.eState == MOOVBOX_CACHING_STATE_CACHED)
	{
		return;
	}

/*************************
*	Connect
*************************/
	VK_memset(&tAccess, 0, sizeof(STREAM_ACCESS_t));
	tAccess.psz_path = VK_StrDup(pUrl);
	iResult = STREAM_HTTP_Open(&tAccess);
	if(iResult != DI_IPT_SUCCESS)
	{
		PrintDebug ("STREAM_HTTP_Open failed!\n");
		return;
	}

/*************************
*	Fine the moov box
*************************/
	pSizeTypeBuf = (HUINT8*)VK_MEM_Alloc(LENGTH_OF_BOX_SIZE + LENGTH_OF_BOX_TYPE);
	if(pSizeTypeBuf == NULL)
	{
		PrintError("Out of memory!\n");
		return;
	}

	while(uiOffset < tAccess.info.i_size)
	{
		sz_Try = LENGTH_OF_BOX_SIZE+LENGTH_OF_BOX_TYPE;
		sz_Real = 0;
		VK_MEM_Memset(pSizeTypeBuf, 0x00, LENGTH_OF_BOX_SIZE + LENGTH_OF_BOX_TYPE);
		do
		{
			iResult = STREAM_HTTP_Read(&tAccess, (HUINT8 *)pSizeTypeBuf+sz_Real, sz_Try - sz_Real);
			if(iResult >= 0)
			{
				sz_Real += iResult;
			}
			else if(iResult == DI_IPT_ETIMEOUT)
			{
				PrintDebug ("Read timeout!\n");
				if(uiErrorCount < CACHING_TIMEOUT_RETRYCOUNT)
				{
					uiErrorCount++;
					continue;
				}
				else
				{
					uiErrorCount = 0;
					sz_Real = 0;
					break;
				}
			}
			else
			{
				PrintDebug ("STREAM_HTTP_Read failed!\n");
				uiErrorCount = 0;
				sz_Real = 0;
				break;
			}
		}while (sz_Real < sz_Try);

		uiBoxSize = 0;
		for(uiIndex=0; uiIndex < 10; uiIndex++)
		{
			if((pSizeTypeBuf[4] == uiBoxTypes[uiIndex][0]) &&
				(pSizeTypeBuf[5] == uiBoxTypes[uiIndex][1]) &&
				(pSizeTypeBuf[6] == uiBoxTypes[uiIndex][2])&&
				(pSizeTypeBuf[7] == uiBoxTypes[uiIndex][3]))
			{
				uiBoxSize = (HUINT64)(pSizeTypeBuf[0]<<24) | (pSizeTypeBuf[1]<<16) | (pSizeTypeBuf[2]<<8) | pSizeTypeBuf[3];
				break;
			}
		}

		if((uiBoxTypes[uiIndex][0] == 'm') &&
			(uiBoxTypes[uiIndex][1] == 'o') &&
			(uiBoxTypes[uiIndex][2] == 'o') &&
			(uiBoxTypes[uiIndex][3] == 'v'))
		{
			bMoovFound = TRUE;
			iResult = STREAM_HTTP_Seek(&tAccess, uiOffset);
			if(iResult != DI_IPT_SUCCESS)
			{
				PrintDebug ("STREAM_HTTP_Seek failed!\n");
				VK_MEM_Free(pSizeTypeBuf);
				return;
			}
			break;
		}
		else
		{
			if(uiBoxSize == 0)
			{
				PrintDebug ("Invalid Box Size!\n");
				VK_MEM_Free(pSizeTypeBuf);
				return;
			}
			else
			{
				uiOffset += uiBoxSize;
				iResult = STREAM_HTTP_Seek(&tAccess, uiOffset);
				if(iResult != DI_IPT_SUCCESS)
				{
					PrintDebug ("STREAM_HTTP_Seek failed!\n");
					VK_MEM_Free(pSizeTypeBuf);
					return;
				}
			}
		}
	}

/*************************
*	Caching
*************************/
	if(bMoovFound == TRUE)
	{
		stMoovBox.eState = MOOVBOX_CACHING_STATE_CACHED;
		stMoovBox.ulStartOffset = uiOffset;
		stMoovBox.unDataLength = uiBoxSize;
		stMoovBox.lCursorPos = 0;
		if(stMoovBox.pucMoov != NULL)
		{
			DD_MEM_Free(stMoovBox.pucMoov);
		}
		stMoovBox.pucMoov = DD_MEM_Alloc(uiBoxSize);
		if(stMoovBox.pucMoov == NULL)
		{
			PrintError("Out of memory!\n");
			VK_MEM_Free(pSizeTypeBuf);
			return;
		}

		pTempBuf = VK_MEM_Alloc(CACHING_UNIT_SIZE);
		if(pTempBuf == NULL)
		{
			PrintError("Out of memory!\n");
			VK_MEM_Free(pSizeTypeBuf);
			VK_MEM_Free(stMoovBox.pucMoov);
			return;
		}

		VK_MEM_Memset(pTempBuf, 0x00, CACHING_UNIT_SIZE);
		sz_Try = CACHING_UNIT_SIZE;
		uiCachedSize = 0;

		while(uiCachedSize < stMoovBox.unDataLength)
		{
			if((uiCachedSize + sz_Try) > stMoovBox.unDataLength)
			{
				sz_Try = stMoovBox.unDataLength - uiCachedSize;
			}
			sz_Real = 0;

			do
			{
				iResult = STREAM_HTTP_Read(&tAccess, (HUINT8 *)pTempBuf+sz_Real, sz_Try - sz_Real);
				if(iResult >= 0)
				{
					sz_Real += iResult;
				}
				else if(iResult == DI_IPT_ETIMEOUT)
				{
					PrintDebug ("Read timeout!\n");
					if(uiErrorCount < CACHING_TIMEOUT_RETRYCOUNT)
					{
						uiErrorCount++;
						continue;
					}
					else
					{
						uiErrorCount = 0;
						sz_Real = 0;
						break;
					}
				}
				else
				{
					PrintDebug ("STREAM_HTTP_Read failed!\n");
					uiErrorCount = 0;
					sz_Real = 0;
					break;
				}
			}while(sz_Real < sz_Try);

			VK_MEM_Memcpy(stMoovBox.pucMoov + uiCachedSize, pTempBuf, sz_Real);
			uiCachedSize += sz_Real;
		}
	}
	PrintDebug ("From 0x%llx To 0x%llx Caching Done!\n", stMoovBox.ulStartOffset, stMoovBox.ulStartOffset + stMoovBox.unDataLength);
	VK_MEM_Free(pSizeTypeBuf);
	VK_MEM_Free(pTempBuf);
	return;
}
#endif

#if STREAM_BUFFERING /* Caution to warning: 'P_STREAM_HTTP_GetBufferSize' declared 'static' but never defined */
static int P_STREAM_HTTP_GetBufferSize (void);
#endif

#if defined(USE_RAMBUFFERING_MOOVBOX_CACHING_SUPPORT) || defined (USE_MOOVBOX_CACHING)
void P_IPTUNER_HTTP_MoovboxCaching(HINT8 *pUrl)
{
	HBOOL bVaildMp4 = FALSE;
	HUINT32 uOff = 0;
	size_t sz_Real=0;
	size_t sz_Try = 0;
	HINT32 n_ReturnedValue = 0;
#if 0 /* warning: unused variable */
	HINT32 nIndex = 0;
#endif
	HINT32	error_retrycount =0;
	int 	error_Timeoutcount =0;
	STREAM_ACCESS_t tAccess;
	HTTP_ACCESS_SYS_t *pstHttpSys=NULL;
#if SUPPORT_HUMAX_COOKIE
	char *psCookie = NULL;
	URL_t tUrl;
#endif
	if(stMoovBox.eState == MOOVBOX_CACHING_STATE_CACHED)
	{
		return;
	}
	VK_memset(&tAccess,0,sizeof(STREAM_ACCESS_t));

#if SUPPORT_HUMAX_COOKIE
	VK_MEM_Memset(&tUrl, 0x0, sizeof(tUrl));
	UTIL_UrlParse(&tUrl, (const char *)pUrl, 0);
	if(tUrl.psz_cookie != NULL)
	{
		psCookie = VK_StrDup(tUrl.psz_cookie);
	}
	UTIL_UrlClean(&tUrl);
#endif

	tAccess.psz_path = VK_StrDup (pUrl);
#if SUPPORT_HUMAX_COOKIE
	if(psCookie != NULL)
	{
		tAccess.psz_cookie = psCookie;
	}
#endif
	if(tAccess.psz_path == NULL)
	{
		PrintError("%s(%d) : Error> Fail Mem alloc.\n",__FUNCTION__,__LINE__);

		if(tAccess.psz_path != NULL)
		{
			DD_MEM_Free(tAccess.psz_path);
			tAccess.psz_path =NULL;
		}
		/* else { continue; } */
#if SUPPORT_HUMAX_COOKIE
		if(tAccess.psz_cookie != NULL)
		{
			DD_MEM_Free(tAccess.psz_cookie);
			tAccess.psz_cookie =NULL;
		}
		/* else { continue; } */
#endif
		PrintExit;
		return; /*0;*/
	}

	/* Try to open the STREAM */
	if (DI_IPT_SUCCESS != STREAM_HTTP_Open (&tAccess) )
	{
		if(tAccess.psz_path != NULL)
		{
			DD_MEM_Free(tAccess.psz_path);
			tAccess.psz_path =NULL;
		}
		/* else { continue; } */
#if SUPPORT_HUMAX_COOKIE
		if(tAccess.psz_cookie != NULL)
		{
			DD_MEM_Free(tAccess.psz_cookie);
			tAccess.psz_cookie =NULL;
		}
		/* else { continue; } */
#endif
		if(tAccess.psz_path != NULL)
		{
			DD_MEM_Free  (tAccess.psz_path);
			tAccess.psz_path =NULL;
		}
		/* else { continue; } */

		PrintError("%s (%d) : Error> STREAM_HTTP_Open Fail.\n",__FUNCTION__,__LINE__);
		PrintExit;
		return; /*0;*/
	}

	pstHttpSys = (HTTP_ACCESS_SYS_t*) tAccess.p_sys;
//	if(VK_strstr(pstHttpSys->psz_mime,"mp4"))
	{
		HUINT8 *pBoxBuffer;
		HBOOL bValidMoovBox = FALSE;
		HUINT32 uBoxLen = 0;
#if 0 /* warning: unused variable */
		HUINT8 ucBoxType[4] = {0};
		HUINT32 uBoxOffset = 0;
#endif
		PrintData ("%s(%d) : psz_mime		= %s \n",__FUNCTION__,__LINE__,pstHttpSys->psz_mime);
		pBoxBuffer = DD_MEM_Alloc(MOOVBOX_CACHING_READ_BLOCK);

		if(pBoxBuffer != NULL)
		{
			VK_MEM_Memset(pBoxBuffer, 0x00, MOOVBOX_CACHING_READ_BLOCK);
			sz_Try=MP4BOX_DEFAULT_SIZE; /* Box Size */
			while(!bValidMoovBox)
			{
				sz_Real=0;
				error_retrycount = 0;
				VK_MEM_Memset(pBoxBuffer, 0x00, MOOVBOX_CACHING_READ_BLOCK);
				do
				{
					n_ReturnedValue = STREAM_HTTP_Read ( &tAccess, (HUINT8 *)pBoxBuffer+sz_Real, sz_Try - sz_Real );
					if(n_ReturnedValue >= 0)
					{
						sz_Real += n_ReturnedValue;
					}
					else if(n_ReturnedValue == DI_IPT_ETIMEOUT)
					{
						if(error_Timeoutcount < CACHING_ERROR_RETRYCOUNT)
						{
							error_Timeoutcount++;
							continue;
						}
						else
						{
							error_Timeoutcount = 0;
							sz_Real = 0;
							break;
						}
					}
					else
					{
						error_Timeoutcount = 0;
						sz_Real = 0;
						break;
					}
				} while (sz_Real < sz_Try);

				if(sz_Real == sz_Try)
				{
					{ /* Get Box Length */
						uBoxLen =
							(((HUINT32)pBoxBuffer[0])<<24) |
							(((HUINT32)pBoxBuffer[1])<<16) |
							(((HUINT32)pBoxBuffer[2])<<8) |
										pBoxBuffer[3];
					}
					if( uBoxLen > 0 && ((uOff+uBoxLen) > uOff) && (tAccess.info.i_size >= (uOff+uBoxLen)))
					{
						//PrintDebug ("%c%c%c%c\n",pBoxBuffer[4],pBoxBuffer[5],pBoxBuffer[6],pBoxBuffer[7]);
						if(!bVaildMp4)
						{
							if(pBoxBuffer[4] == 'f' && pBoxBuffer[5] == 't' && pBoxBuffer[6] == 'y' && pBoxBuffer[7] == 'p')
							{
								bVaildMp4 = TRUE;
								uOff += uBoxLen;
								if(DI_IPT_SUCCESS != STREAM_HTTP_Seek ( &tAccess, uOff ))
								{
									bValidMoovBox = FALSE;
									break;
								}
							}
							else
							{
								bVaildMp4 = FALSE;
								bValidMoovBox = FALSE;
								break;
							}
						}
						else if(pBoxBuffer[4] == 'm' && pBoxBuffer[5] == 'o' && pBoxBuffer[6] == 'o' && pBoxBuffer[7] == 'v')
						{
							bValidMoovBox = TRUE;
							if(DI_IPT_SUCCESS != STREAM_HTTP_Seek ( &tAccess, uOff ))
							{
								bValidMoovBox = FALSE;
								break;
							}
							break;
						}
						/*
						else if(pBoxBuffer[4] == 'm' && pBoxBuffer[5] == 'd' && pBoxBuffer[6] == 'a' && pBoxBuffer[7] == 't')
						{
							bValidMoovBox = FALSE;
							break;
						}*/
						else
						{
							uOff += uBoxLen;
							if(DI_IPT_SUCCESS != STREAM_HTTP_Seek ( &tAccess, uOff ))
							{
								bValidMoovBox = FALSE;
								break;
							}
						}
					}
					else
					{
						bVaildMp4 = FALSE;
						bValidMoovBox = FALSE;
						break;
					}

				}
				else
				{
					bVaildMp4 = FALSE;
					bValidMoovBox = FALSE;
				}
			}
			if(bValidMoovBox == TRUE)
			{
				stMoovBox.lCursorPos = 0;
				stMoovBox.ulStartOffset = uOff;
				stMoovBox.unDataLength = uBoxLen;
				stMoovBox.pucMoov = DD_MEM_Alloc(uBoxLen);
				//PrintDebug ("MOOV BOX(%u)\n",uBoxLen);
				//PrintDebug ("MOOV BOX ulStartOffset(%u)\n",uOff);
				uOff = 0;
				if(stMoovBox.pucMoov != NULL)
				{
					VK_MEM_Memset(stMoovBox.pucMoov, 0x00, uBoxLen);
					while(uBoxLen > 0)
					{
						sz_Try= MOOVBOX_CACHING_READ_BLOCK; /* Box Size */
						sz_Real = 0;
						if(sz_Try > uBoxLen)
						{
							sz_Try = uBoxLen;
						}
						VK_MEM_Memset(pBoxBuffer, 0x0, MOOVBOX_CACHING_READ_BLOCK);
						do
						{
							n_ReturnedValue = STREAM_HTTP_Read ( &tAccess, (HUINT8 *)pBoxBuffer+sz_Real, sz_Try - sz_Real );
							if(n_ReturnedValue >= 0)
							{
								sz_Real += n_ReturnedValue;
							}

							if(n_ReturnedValue == DI_IPT_ETIMEOUT)
							{
								if(error_Timeoutcount != CACHING_ERROR_RETRYCOUNT)
								{
									error_Timeoutcount++;
									continue;
								}
								else
								{
									error_Timeoutcount = 0;
									bValidMoovBox = FALSE;
									if(stMoovBox.pucMoov != NULL)
									{
										DD_MEM_Free(stMoovBox.pucMoov);
										stMoovBox.pucMoov = NULL;
									}
									VK_MEM_Memset(&stMoovBox,0x00,sizeof(MOOVBOX_INSTANCE_t));
									stMoovBox.eState = MOOVBOX_CACHING_STATE_NONE;
									if(pBoxBuffer != NULL)
									{
										DD_MEM_Free(pBoxBuffer );
									}
									STREAM_HTTP_Close(&tAccess);
									PrintDebug("[%s][%d] MOOVBOX CACHING FAILED!!\n",__FUNCTION__,__LINE__);
									return;
								}
							}
						}
						while (sz_Real < sz_Try);
						VK_MEM_Memcpy(stMoovBox.pucMoov+uOff, pBoxBuffer, sz_Real);
						uBoxLen -= sz_Real;
						uOff += sz_Real;
					}
				}
			}
			if(bValidMoovBox == TRUE)
			{
				stMoovBox.eState = MOOVBOX_CACHING_STATE_CACHED;
				PrintDebug("[%s][%d] MOOVBOX CACHING COMPLETE!!\n",__FUNCTION__,__LINE__);
				//PrintDebug ("FOURCC %c%c%c%c !!\n",*(stMoovBox.pucMoov+4),*(stMoovBox.pucMoov+5),*(stMoovBox.pucMoov+6),*(stMoovBox.pucMoov+7));
			}
			else
			{
				if(stMoovBox.pucMoov != NULL)
				{
					DD_MEM_Free(stMoovBox.pucMoov);
					stMoovBox.pucMoov = NULL;
				}
				VK_MEM_Memset(&stMoovBox,0x00,sizeof(MOOVBOX_INSTANCE_t));
				stMoovBox.eState = MOOVBOX_CACHING_STATE_NONE;
				PrintDebug("[%s][%d] MOOVBOX CACHING FAILED!!\n",__FUNCTION__,__LINE__);
			}
		}
		if(pBoxBuffer != NULL)
		{
			DD_MEM_Free(pBoxBuffer );
		}
		STREAM_HTTP_Close(&tAccess);
	}
}
#endif

void P_IPTUNER_HTTP_CheckUnderflow(DI_IPTUNER_Index_e nTunerIndex)
{
#if STREAM_BUFFERING
	HINT32 iCurrentDataSize=0;

	iCurrentDataSize=P_STREAM_HTTP_GetBufferSize();

	if ( iCurrentDataSize < LOWEST_HTTP_BUFFERING_THRESHOLD
		&& s_ePreBufferingTaskState != DI_IPTUNER_HTTP_PREBUFFERING_TASK_STATE_DONE)
	{
		if(bPausedForBuffering == FALSE)
		{
			if(gUnderFlowLimit < 100)
			{
				gUnderFlowLimit++;
			}
			else
			{
				if ( NULL != stHttpBufferingCallback.cbPauseCallback)
				{
					stHttpBufferingCallback.cbPauseCallback (nTunerIndex);
					bPausedForBuffering=TRUE;
					gUnderFlowLimit=0;
				}
				else
				{
					PrintError("cbPauseCallback is NULL!!\n");
				}
			}
		}
	}
	else if(iCurrentDataSize > RESUME_HTTP_BUFFERING_THRESHOLD
		|| s_ePreBufferingTaskState == DI_IPTUNER_HTTP_PREBUFFERING_TASK_STATE_DONE)
	{
		gUnderFlowLimit=0;
		if(bPausedForBuffering == TRUE)
		{
			if ( NULL != stHttpBufferingCallback.cbPlayCallback)
			{
				stHttpBufferingCallback.cbPlayCallback (nTunerIndex);
				bPausedForBuffering=FALSE;
			}
			else
			{
				PrintError("cbPlayCallback is NULL!!\n");
			}
		}
	}
#else
	/* Removed warning: unused parameter 'nTunerIndex'                     */
	/* Dont use UNUSED_PARAM(X). Just post debug message for removing warn */
	/* UNUSED_PARAM(nTunerIndex);                                          */
	PrintData("%s(%d) [tunerIndex:%d]\n", __FUNCTION__, __LINE__, nTunerIndex);
#endif
}

#if __________task__________
#endif

#if 0 /* Removed for warning: 'P_IPTUNER_HTTP_MonitorTask' defined but not used */
static void P_IPTUNER_HTTP_MonitorTask(void* param)
{
#if STREAM_BUFFERING
	param=param;
	DI_IPTUNER_Index_e eMsg;

	while(1)
	{
		VK_MSG_Receive(gPreBufferingMsgId, &eMsg, sizeof(DI_IPTUNER_Index_e));
		P_IPTUNER_HTTP_CheckUnderflow(eMsg);
	}
#endif
}
#endif /* Removed for warning: 'P_IPTUNER_HTTP_MonitorTask' defined but not used */


#if __________drv__________
#endif
DRV_Error DRV_IPTUNER_HTTP_Init(void)
{
#if 0
/*************************
*	1. MSG setting
*************************/
	VK_MSG_Create(DI_HTTP_STREAM_MSG_SIZE,
		sizeof(DI_IPTUNER_Index_e),
		"PreBufferingMsg",
		&gPreBufferingMsgId,
		VK_SUSPENDTYPE_FIFO);

/*************************
*	2. Task setting
*************************/
	VK_TASK_Create( P_IPTUNER_HTTP_MonitorTask,
								MEDIA_TIME_TASK_PRIORITY,
								MEDIA_STREAMPLAY_TASK_STACK_SIZE,
								"PreBuffering",
								NULL,
								&gPreBufferingTaskId,
								FALSE);
	VK_TASK_Start(gPreBufferingTaskId);
#endif
#if defined(CONFIG_IPTUNER_OPENSSL_CERT_MANAGER)
		INT_Load_ClientInfo();
#endif
	return DRV_OK;
}

#if __________di__________
#endif


DI_ERR_CODE DI_STREAM_HTTP_GetProtoStatusCode (HINT32 *pHttpCode)
{
	(void) STREAM_HTTP_GetProtoStatusCode (pHttpCode);
	return DI_ERR_OK;
}
/**
 * @brief IPTUNER 를 초기화한다.
 * @param 	None
 * @return 	HAAL Error Code
 * @date 	2009/05/24
 * @author 	junsy@humaxdigital.com
 * @remark 	IPTUNER Index별로 DI_IPTUNER_t 정보를 저장할 배열을 초기화한다.
 * @see 	DI_IPTUNER_t
 */
void DI_STREAM_HTTP_DnpStartReceiving (void)
{
	PrintEnter;
	s_nHttpStopReceiving = FALSE;
	s_nHttpPauseReceiving = FALSE;
	PrintExit;
	return;
}
void DI_STREAM_HTTP_DnpStopReceiving (void)
{
	PrintEnter;
	s_nHttpStopReceiving = TRUE;
	PrintExit;
	return;
}

void DI_STREAM_HTTP_DnpPauseReceiving (void)
{
	PrintEnter;
	s_nHttpPauseReceiving = TRUE;
	PrintExit;
	return;
}

void DI_STREAM_HTTP_DnpResumeReceiving (void)
{
	PrintEnter;
	s_nHttpPauseReceiving = FALSE;
	PrintExit;
	return;
}

int DI_STREAM_HTTP_DnpReceive (VFIO_Handle_t tHandle)
{
	HINT8 *pBuffer=NULL;
	HINT32 nRecvLength=0;
	HINT64 nContentLength = stHttpAccess.info.i_size;
	HINT32 flags = O_CREAT|O_TRUNC|O_WRONLY|O_SYNC;
	HINT32 nRc = -1;
	DNP_USER_DATA_t * pDnpUserData;

	DI_ERR_CODE nRet = DI_ERR_OK;
	HULONG ulStartClock = 0;
	HUINT32 ulLoopCount = 0;

	/* prdn vars */
	HINT32 nReadBufferSize = 0;
	HINT32 nPrdnDownloadStatusArraySize;
	HINT32 nchunkoffLastWritten = 0;
	HINT32 nchunkoffWritingCur = 0;
	HINT32 nSumofRecvLength = 0;
	HUINT32 nBufferingSizeInChunkBeforeProbe = 0;
	/* prdn info vars */
	g_nPrdnInfoLastReadyStateSentTime = 0;
	g_nPrdnInfoBufferSizeInChunkCount = 256; /* 8KB * 256 = 2MB */
	g_nPrdnInfoDownloadedChunkCount = 0;
	g_nchunkoffLastPrebufferIdx = 0;

#if 1	/* JWKIM, workaround: for Maxdome Service */
	s_nIsPrebufferingCompleted = 0;
#endif

	nSumLength = 0;

	PrintEnter;

	if(tHandle.pDnpUserData == NULL)
	{
		PrintError("Error > %s(%d) pGeneral Ptr is NULL\n", __FUNCTION__, __LINE__);
		return -1;
	}
	else
	{
		pDnpUserData = (DNP_USER_DATA_t *)tHandle.pDnpUserData;
	}


#ifdef ENABLE_MAPPED_DNP
	/* init progressive down extern variable */
	g_nchunkoffPrdnEof = PRDN_GET_CHUNK_OFF(nContentLength - 1); /* count -> zero base index -> chunk_off */
	nPrdnDownloadStatusArraySize = PRDN_GET_BYTE_OFF(g_nchunkoffPrdnEof) + 1; /* chunk_off -> byte_offset -> byte_count */

	g_bPrdnDownCompleted = FALSE;
	g_nchunkoffIsCompletedStart = 0;
	g_bPrdnForwardPlay = TRUE;
#if 1	/* JWKIM, workardound: for Maxdome Service */
	g_bPrdnPlayFFTrick = FALSE;
#endif
	g_nchunkoffOldPrdnRequestedSeekPoint = -1;
	g_nchunkoffPrdnRequestedSeekPoint = 0; /* start point */
	nReadBufferSize  = (HINT32)PRDN_GET_RAW_OFF_FROM_CHUNK_OFF(1);
	nBufferingSizeInChunkBeforeProbe = 0;

	/* if the size of chunk is 64Kbytes */
	if(g_nchunkoffPrdnEof <= (12800/8))/* lesser than 100MB */
	{
		if ( P_STREAM_HTTP_UseHttpsProtocol(&stHttpAccess) == TRUE )
		{
			nBufferingSizeInChunkBeforeProbe = 16*3; /* 3MB */
		}
		else
		{
			nBufferingSizeInChunkBeforeProbe = 16; /* 1MB */
		}
	}
	else if(g_nchunkoffPrdnEof <= (256000/8))/* lesser than 2GB */
	{
		if ( P_STREAM_HTTP_UseHttpsProtocol(&stHttpAccess) == TRUE )
		{
			nBufferingSizeInChunkBeforeProbe = 32*3; /* 6MB */
		}
		else
		{
			nBufferingSizeInChunkBeforeProbe = 32; /* 2MB */
		}
	}
	else /* larger than 2GB */
	{
		if ( P_STREAM_HTTP_UseHttpsProtocol(&stHttpAccess) == TRUE )
		{
			/* 0.1% e.g.) 4GB need 16MB */
			nBufferingSizeInChunkBeforeProbe = (g_nchunkoffPrdnEof/1000)*3;
		}
		else
		{
			/* 0.1% e.g.) 4GB need 4MB */
			nBufferingSizeInChunkBeforeProbe = (g_nchunkoffPrdnEof/1000);
		}
	}
	PrintDebug("Before Probe Buffering Size is %uKB\n", nBufferingSizeInChunkBeforeProbe * 8);

	if(g_pucPrdnChunkDownloadStatusArray != NULL)
	{
		PrintDebug("@@@@@ [%s:%d] g_pucPrdnChunkDownloadStatusArray is Free\n", __func__, __LINE__);
		DD_MEM_Free(g_pucPrdnChunkDownloadStatusArray);
		g_pucPrdnChunkDownloadStatusArray = NULL;
	}

	g_pucPrdnChunkDownloadStatusArray = (HUINT8 *) DD_MEM_Alloc ( nPrdnDownloadStatusArraySize );
	if(g_pucPrdnChunkDownloadStatusArray == NULL)
	{
		PrintError ("%s(%d) Error> PRDN Progress Array Heap Memory Allocation Fail. size (%d) !!\n",__FUNCTION__,__LINE__,nPrdnDownloadStatusArraySize);
		goto exit_stop;
	}
	VK_memset(g_pucPrdnChunkDownloadStatusArray, 0, nPrdnDownloadStatusArraySize); /* clear status */

	/* show some debug info */
	PrintDebug("nPrdnDownloadStatusArraySize = %d\n", nPrdnDownloadStatusArraySize);
	PrintDebug("g_nchunkoffPrdnEof = %d\n", g_nchunkoffPrdnEof);

	/* 1. Open DNP FilePath */
	s_nWfd = open (DI_IPTUNER_DNP_FILE_PATH, flags, 0666);
	if(s_nWfd<0)
	{
		PrintError ("%s(%d) :  Error> DI_IPTUNER_DNP_FILE_PATH open fail (HTTP_DNP) \n",__FUNCTION__,__LINE__);
		return -1;
	}

	PrintDebug ("%s(%d): Open %s.\n",__FUNCTION__,__LINE__,DI_IPTUNER_DNP_FILE_PATH);
	PrintDebug ("%s(%d): nContentLength (%"PRId64")bytes \n",__FUNCTION__,__LINE__,nContentLength);

	/* heap allocation */
	pBuffer = (HINT8*) DD_MEM_Alloc (nReadBufferSize);
	if(pBuffer==NULL)
	{
		PrintError ("%s(%d) Error> Heap Memory Allocation Fail. size (%d) !!\n",__FUNCTION__,__LINE__,nReadBufferSize);
		goto exit_stop;
	}

	(void) DI_IPTUNER_DnpPutFileLength (nContentLength);

	/** HTTP DNP Main Download Loop */
	do
	{
		if ( s_nIsEventSend == 0
				 && (HUINT32)g_nPrdnInfoDownloadedChunkCount >= nBufferingSizeInChunkBeforeProbe )
		{
			s_nIsEventSend = 1;
			DI_IPTUNER_DnpSendEvent ();
		}

		if ( TRUE == s_nHttpStopReceiving)
		{
			PrintDebug("%s(%d) : User Stoped Receiving !!\n",__FUNCTION__,__LINE__);
			goto exit_stop;
		}

		if ( TRUE == s_nHttpPauseReceiving)
		{
			PrintDebug("%s(%d) : User Paused Receiving !!\n",__FUNCTION__,__LINE__);
			VK_TASK_Sleep(100);
			continue;
		}

		if(g_nchunkoffPrdnRequestedSeekPoint != -1)
		{
			//PrintDebug("================ recv SEEK  %d\n", g_nchunkoffPrdnRequestedSeekPoint);
			nchunkoffWritingCur = g_nchunkoffPrdnRequestedSeekPoint;
			g_nchunkoffPrdnRequestedSeekPoint = -1;
		}

		if(!PRDN_ISVALID(nchunkoffWritingCur))
		{
			/*XXX: Error check */
			if(nchunkoffLastWritten + 1 != nchunkoffWritingCur)
			{
				PrintDebug("================BEFORE  SEEK \n");
				{
					HUINT32 nNetworkRetryCount = 0;
					while(DI_STREAM_HTTP_Seek(tHandle, PRDN_GET_RAW_OFF_FROM_CHUNK_OFF(nchunkoffWritingCur), SEEK_SET) == -1)
					{
						nNetworkRetryCount++;
						PrintError("Seek Failed, Retrying (%u)...\n", nNetworkRetryCount);
						VK_TASK_Sleep(100);
						if(nNetworkRetryCount > DI_IPTUNER_DNP_RETRY_COUNT_ON_NET_ERROR)
						{
							PrintError("Send Failed code To DNP Reader(Player,Indexer)\n");
							pDnpUserData->bDNPNetworkError = TRUE;
							s_nHttpStopReceiving = TRUE;
							VK_TASK_Sleep(100);

							goto exit_stop;

						}
						if ( TRUE == s_nHttpStopReceiving)
						{
							PrintDebug("%s(%d) : User Stoped Receiving !!\n",__FUNCTION__,__LINE__);
							goto exit_stop;
						}

						if ( TRUE == s_nHttpPauseReceiving)
						{
							PrintDebug("%s(%d) : User Paused Receiving !!\n",__FUNCTION__,__LINE__);
							VK_TASK_Sleep(100);
							continue;
						}
					}
				}

				PrintDebug("================SEEK  DONE \n");
				PrintDebug("================SEEK  DONE \n");
				PrintDebug("================SEEK  DONE \n");
				PrintDebug("seeked chunk i = %d\n", nchunkoffWritingCur);
			}

			/* fill chunk */
			{
				HUINT32 nNetworkRetryCount2 = 0;
				nRecvLength = 0;
				VK_memset  (pBuffer,0,nReadBufferSize);
				nSumofRecvLength = 0;

				if(ulLoopCount == 0)
				{
					nRet = DI_IPTUNER_DnpInitDownloadSpeed(&ulStartClock);
					if(nRet != DI_ERR_OK)
					{
						PrintError("@@@@@ [%s:%d:%d] Initializing DownloadSpeed is failed\n", __func__, __LINE__, nRet);
					}
				}
				ulLoopCount++;

				while(nSumofRecvLength < nReadBufferSize)
				{

					(void) DI_STREAM_HTTP_Read (tHandle, pBuffer + nSumofRecvLength, nReadBufferSize - nSumofRecvLength, 1, (size_t *)&nRecvLength);
#ifdef DI_IPTUNER_HTTP_DEBUG
					//PrintError("Bandwith Shaping is Activated !!!\n");
					//VK_TASK_Sleep(30); /* simulate low connection bandwidth about to 210KBps */
#endif
					if ( TRUE == s_nHttpStopReceiving)
					{
						PrintDebug("%s(%d) : User Stoped Receiving !!\n",__FUNCTION__,__LINE__);
						goto exit_stop;
					}

					if(nRecvLength <= 0)
					{
						if(nchunkoffWritingCur == g_nchunkoffPrdnEof)
						{
							/* meet end of stream? -> partial chunk */
							if(nSumofRecvLength >= nContentLength - PRDN_GET_RAW_OFF_FROM_CHUNK_OFF(g_nchunkoffPrdnEof))
							{
								break; /* go to success case */
							}
						}
						/* retry */
						nNetworkRetryCount2++;
						PrintError("%s(%d) Network Read Error(%d). Retrying Count = %u Reconnect...\n", __FUNCTION__, __LINE__, nRecvLength, nNetworkRetryCount2);
						{
							HUINT32 nNetworkRetryCount3 = 0;
							while(DI_STREAM_HTTP_Seek(tHandle, PRDN_GET_RAW_OFF_FROM_CHUNK_OFF(nchunkoffWritingCur) + nSumofRecvLength, SEEK_SET) == -1)
							{
								nNetworkRetryCount3++;
								PrintError("Seek Failed, Retrying (%u)...\n", nNetworkRetryCount3);
								VK_TASK_Sleep(100);
								if(nNetworkRetryCount3 > DI_IPTUNER_DNP_RETRY_COUNT_ON_NET_ERROR)
								{
									PrintError("Send Failed code To DNP Reader(Player,Indexer)\n");
									pDnpUserData->bDNPNetworkError = TRUE;
									s_nHttpStopReceiving = TRUE;
									VK_TASK_Sleep(100);

									goto exit_stop;
								}
								if ( TRUE == s_nHttpStopReceiving)
								{
									PrintDebug("%s(%d) : User Stoped Receiving !!\n",__FUNCTION__,__LINE__);
									goto exit_stop;
								}

								if ( TRUE == s_nHttpPauseReceiving)
								{
									PrintDebug("%s(%d) : User Paused Receiving !!\n",__FUNCTION__,__LINE__);
									VK_TASK_Sleep(100);
									continue;
								}
							}
						}
						if(nNetworkRetryCount2 > DI_IPTUNER_DNP_RETRY_COUNT_ON_NET_ERROR)
						{
							PrintError("Send Failed code To DNP Reader(Player,Indexer)\n");
							pDnpUserData->bDNPNetworkError = TRUE;
							s_nHttpStopReceiving = TRUE;
							VK_TASK_Sleep(100);

							goto exit_stop;
						}

						if ( TRUE == s_nHttpStopReceiving)
						{
							PrintDebug("%s(%d) : User Stoped Receiving !!\n",__FUNCTION__,__LINE__);
							goto exit_stop;
						}

						if ( TRUE == s_nHttpPauseReceiving)
						{
							PrintDebug("%s(%d) : User Paused Receiving !!\n",__FUNCTION__,__LINE__);
							VK_TASK_Sleep(100);
							continue;
						}
						PrintError("Retrying to network read...\n");
						continue;
					}
					nSumofRecvLength += nRecvLength;
				}

				nRet = DI_IPTUNER_DnpUpdateDownloadSpeed(ulStartClock, nRecvLength, ulLoopCount);
				if(nRet != DI_ERR_OK)
				{
					PrintError("@@@@@ [%s:%d:%d] Updating DownloadSpeed is failed\n", __func__, __LINE__, nRet);
				}
				else
				{
					ulLoopCount = 0;
				}

			}

			/* success case */
			lseek(s_nWfd, PRDN_GET_RAW_OFF_FROM_CHUNK_OFF(nchunkoffWritingCur), SEEK_SET);
			nRc = write (s_nWfd, pBuffer, nSumofRecvLength);
			if(nRc<0)
			{
				PrintError ("%s(%d) : Error > HDD Write Error Occured !! \n",__FUNCTION__,__LINE__);
				PrintError("Send Failed code To DNP Reader(Player,Indexer)\n");
				pDnpUserData->bDNPNetworkError = TRUE;
				continue;
			}
			//fsync(s_nWfd);

			PRDN_SETVALID(nchunkoffWritingCur);

			nchunkoffLastWritten = nchunkoffWritingCur;
			g_nPrdnInfoDownloadedChunkCount ++;

			//PrintData ("%s (%d) : sum(%"PRId64") remained(%"PRId64")\n",__FUNCTION__,__LINE__,off_t_continuous_end, nContentLength-off_t_continuous_end);

			usleep(0); /* may yield thread control */
		}

		if(pDnpUserData->uiContentDuration != 0)
		{
			nRet = DI_IPTUNER_DnpCalcBufferingInfo(pDnpUserData->uiContentDuration);
			if(nRet != DI_ERR_OK)
			{
				PrintError("@@@@@ [%s:%d] Calcurating BufferingInfo is Failed\n", __func__, __LINE__);
			}
							}

				{
			nchunkoffWritingCur++;

			if(g_bPrdnForwardPlay == FALSE && nchunkoffWritingCur > g_nchunkoffLastPrebufferIdx
					&& g_nchunkoffLastPrebufferIdx != 0)
			{
				nchunkoffWritingCur = g_nchunkoffLastPrebufferIdx - PRDN_GET_CHUNK_OFF(DEFAULT_PRDN_BACKTRICK_PREBUFFERING_SIZE) * 2;
				g_nchunkoffLastPrebufferIdx -= PRDN_GET_CHUNK_OFF(DEFAULT_PRDN_BACKTRICK_PREBUFFERING_SIZE);
				if(nchunkoffWritingCur < 0)
				{
					nchunkoffWritingCur = 0;
				}
				if(g_nchunkoffLastPrebufferIdx < 0)
				{
					g_nchunkoffLastPrebufferIdx = 0;
				}
			}
			if(nchunkoffWritingCur > g_nchunkoffPrdnEof)
			{
				PrintDebug("PRDN Meets EOF. writing cur = 0\n");
				if(PRDN_IsCompleted())
				{
					break;
				}
				nchunkoffWritingCur = 0;
			}
		}
	} while ( TRUE );

#else

	nSumLength = 0;

	PrintEnter;
	/* 1. Open DNP FilePath */
	s_nWfd = open (DI_IPTUNER_DNP_FILE_PATH, flags, 0666);
	if(s_nWfd<0)
	{
		PrintError ("%s(%d) :  Error> DI_IPTUNER_DNP_FILE_PATH open fail (HTTP_DNP) \n",__FUNCTION__,__LINE__);
		return -1;
	}

	PrintDebug ("%s(%d): Open %s.\n",__FUNCTION__,__LINE__,DI_IPTUNER_DNP_FILE_PATH);
	PrintDebug ("%s(%d): nContentLength (%"PRId64")bytes \n",__FUNCTION__,__LINE__,nContentLength);

	/* heap allocation */
	pBuffer = (HINT8*) DD_MEM_Alloc (MAX_HTTP_RECV_BUFF_SIZE);
	if(pBuffer==NULL)
	{
		PrintError ("%s(%d) Error> Heap Memory Allocation Fail. size (%d) !!\n",__FUNCTION__,__LINE__,MAX_HTTP_RECV_BUFF_SIZE);
		goto exit_stop;
	}

	(void) DI_IPTUNER_DnpPutFileLength (nContentLength);

	/** HTTP DNP Main Download Loop */
	do
	{
		if ( TRUE == s_nHttpStopReceiving)
		{
			PrintDebug("%s(%d) : User Stoped Receiving !!\n",__FUNCTION__,__LINE__);
			goto exit_stop;
		}

		if ( TRUE == s_nHttpPauseReceiving)
		{
			PrintDebug("%s(%d) : User Paused Receiving !!\n",__FUNCTION__,__LINE__);
			VK_TASK_Sleep(100);
			continue;
		}

		nRecvLength = 0;
		VK_memset  (pBuffer,0,MAX_HTTP_RECV_BUFF_SIZE);

		/*XXX: Error check */
		(void) DI_STREAM_HTTP_Read (tHandle, pBuffer, MAX_HTTP_RECV_BUFF_SIZE, 1, (size_t *)&nRecvLength);

#if 0
		PrintData ("%s (%d) : total(%"PRId64") recv(%d)\n",__FUNCTION__,__LINE__,nContentLength, nRecvLength);
#endif
		if(nRecvLength <= 0)
		{
#if 0
			if (stHttpAccess.info.b_eof == TRUE )
			{
				DI_IPTUNER_DnpSendEvent();

			}

			/* check network status */
			if (TRUE == DI_IPTUNER_GetNetworkStatus())
			{
				PrintError ("%s(%d) : Error> Socket Error in Network Device \n",__FUNCTION__,__LINE__);
				return -1;
			}
#endif
			pDnpUserData->bDNPNetworkError = TRUE;
			continue;
		}
		/* normal case */
		else
		{
			nRc = write (s_nWfd, pBuffer, nRecvLength);
			if(nRc<0)
			{
				PrintError ("%s(%d) : Error > Write Error Occured !! \n",__FUNCTION__,__LINE__);
			}

			nSumLength += nRecvLength;
			/* Send Event for Probing (64Kbytes) */
			if ( (s_nIsEventSend == 0) &&
				((nSumLength > (2048*64*2)) || (nSumLength == nContentLength)) )
			{
				s_nIsEventSend = 1;
				DI_IPTUNER_DnpSendEvent ();
			}
			/* else { continue; } */

#if 0
			PrintData ("%s (%d) : sum(%"PRId64") remained(%"PRId64")\n",__FUNCTION__,__LINE__,nSumLength, nContentLength-nSumLength);
#endif
			usleep(0);
		}
	} while ( nSumLength < nContentLength );
#endif

exit_stop:

	g_pReadyStateCallbackFunc = NULL;

#ifdef ENABLE_MAPPED_DNP
	if ( s_nIsEventSend == 0 ) /* if the probe is not started, start probe */
	{
		s_nIsEventSend = 1;
		DI_IPTUNER_DnpSendEvent ();
	}

	g_bPrdnDownCompleted = TRUE;
	if(g_pucPrdnChunkDownloadStatusArray != NULL)
	{
		DD_MEM_Free(g_pucPrdnChunkDownloadStatusArray);
		g_pucPrdnChunkDownloadStatusArray = NULL;
	}

#endif

	if(pBuffer != NULL)
	{
		DD_MEM_Free(pBuffer);
		pBuffer = NULL;
	}

	close(s_nWfd);
	s_nWfd = -1;
	s_nIsEventSend = 0;

	PrintExit;
	return 0;
}


DI_ERR_CODE DI_STREAM_HTTP_ICYProbe (const HINT8 *path,  DI_MEDIA_INFO_t *pstMediaInfo, HBOOL *pbIsICY)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	HTTP_ACCESS_SYS_t *pstHttpSys=NULL;
#if REAL_TIME_MULTISOCKET_SUPPORT
	STREAM_ACCESS_t *ptHttpAccess=NULL;
#endif
	VFIO_Handle_t stHandle;

	PrintEnter;

	VK_MEM_Memset(&stHandle, 0x0, sizeof(stHandle));

	PrintDebug("%s (%d) REMOTE PROBE >>>> path=(%s)\n",__FUNCTION__,__LINE__,path);
	if( VFIO_ERROR_SUCCESS != DI_STREAM_HTTP_Open (path, NULL ,&stHandle))
	{
		PrintError("%s (%d) Error> DI_STREAM_HTTP_Open fail.\n",__FUNCTION__,__LINE__);
		nRet = DI_ERR_ERROR;
	}
	else
	{
#if REAL_TIME_MULTISOCKET_SUPPORT
		if(stHandle.pDnpUserData == NULL)
		{
			PrintError("%s (%d) Error> stHandle.pDnpUserData is null.\n",__FUNCTION__,__LINE__);
			DI_STREAM_HTTP_Close(stHandle);
			PrintExit;
			return DI_ERR_ERROR;
		}

		ptHttpAccess = (STREAM_ACCESS_t*) stHandle.pDnpUserData;
		pstHttpSys = ptHttpAccess->p_sys;
#else
		pstHttpSys = (HTTP_ACCESS_SYS_t*) stHttpAccess.p_sys;
#endif
		if(pstHttpSys == NULL)
		{
			PrintError("%s (%d) Error> pstHttpSys is null.\n",__FUNCTION__,__LINE__);
			DI_STREAM_HTTP_Close(stHandle);
			PrintExit;
			return DI_ERR_ERROR;
		}

#if REAL_TIME_MULTISOCKET_SUPPORT
		PrintData ("%s(%d) : i_update       = %d \n",__FUNCTION__,__LINE__,ptHttpAccess->info.i_update);
		PrintData ("%s(%d) : i_size         = %"PRId64"\n",__FUNCTION__,__LINE__,ptHttpAccess->info.i_size);
		PrintData ("%s(%d) : i_pos          = %"PRId64"\n",__FUNCTION__,__LINE__,ptHttpAccess->info.i_pos);
		PrintData ("%s(%d) : b_eof          = %d \n",__FUNCTION__,__LINE__,ptHttpAccess->info.b_eof);
		PrintData ("%s(%d) : i_title        = %d \n",__FUNCTION__,__LINE__,ptHttpAccess->info.i_title);
		PrintData ("%s(%d) : i_seekpoint    = %d \n",__FUNCTION__,__LINE__,ptHttpAccess->info.i_seekpoint);
		PrintData ("%s(%d) : b_prebuffered  = %d \n",__FUNCTION__,__LINE__,ptHttpAccess->info.b_prebuffered);
#else
		PrintData ("%s(%d) : i_update       = %d \n",__FUNCTION__,__LINE__,stHttpAccess.info.i_update);
		PrintData ("%s(%d) : i_size         = %"PRId64"\n",__FUNCTION__,__LINE__,stHttpAccess.info.i_size);
		PrintData ("%s(%d) : i_pos          = %"PRId64"\n",__FUNCTION__,__LINE__,stHttpAccess.info.i_pos);
		PrintData ("%s(%d) : b_eof          = %d \n",__FUNCTION__,__LINE__,stHttpAccess.info.b_eof);
		PrintData ("%s(%d) : i_title        = %d \n",__FUNCTION__,__LINE__,stHttpAccess.info.i_title);
		PrintData ("%s(%d) : i_seekpoint    = %d \n",__FUNCTION__,__LINE__,stHttpAccess.info.i_seekpoint);
		PrintData ("%s(%d) : b_prebuffered  = %d \n",__FUNCTION__,__LINE__,stHttpAccess.info.b_prebuffered);
#endif
		PrintData ("%s(%d) : fd              = %d \n",__FUNCTION__,__LINE__,pstHttpSys->fd);
		PrintData ("%s(%d) : i_vresion       = %d \n",__FUNCTION__,__LINE__,pstHttpSys->i_version);
		PrintData ("%s(%d) : i_remaining     = %"PRId64"\n",__FUNCTION__,__LINE__,pstHttpSys->i_remaining);
		PrintData ("%s(%d) : b_seekable      = %d \n",__FUNCTION__,__LINE__,pstHttpSys->b_seekable);
		PrintData ("%s(%d) : b_reconnect     = %d \n",__FUNCTION__,__LINE__,pstHttpSys->b_reconnect);
		PrintData ("%s(%d) : b_continuous    = %d \n",__FUNCTION__,__LINE__,pstHttpSys->b_continuous);
		PrintData ("%s(%d) : b_pace_control  = %d \n",__FUNCTION__,__LINE__,pstHttpSys->b_pace_control);
		PrintData ("%s(%d) : b_persist       = %d \n",__FUNCTION__,__LINE__,pstHttpSys->b_persist);

#ifdef ICY_PROTOCOL
		PrintData ("%s(%d) : b_icecast              = %d \n",__FUNCTION__,__LINE__,pstHttpSys->b_icecast);
		PrintData ("%s(%d) : i_icy_meta              = %d \n",__FUNCTION__,__LINE__,pstHttpSys->i_icy_meta);
		PrintData ("%s(%d) : i_icy_offset       = %"PRId64" \n",__FUNCTION__,__LINE__,pstHttpSys->i_icy_offset);
		PrintData ("%s(%d) : psz_icy_name     = %s\n",__FUNCTION__,__LINE__,pstHttpSys->psz_icy_name);
		PrintData ("%s(%d) : psz_icy_genre      = %s \n",__FUNCTION__,__LINE__,pstHttpSys->psz_icy_genre);
		PrintData ("%s(%d) : psz_icy_title     = %s \n",__FUNCTION__,__LINE__,pstHttpSys->psz_icy_title);
		PrintData ("%s(%d) : e_icy_codec      = %d \n",__FUNCTION__,__LINE__,pstHttpSys->e_icy_codec);
		PrintData ("%s(%d) : i_icy_br      = %d \n",__FUNCTION__,__LINE__,pstHttpSys->i_icy_br);
		PrintData ("%s(%d) : i_icy_pub     = %d \n",__FUNCTION__,__LINE__,pstHttpSys->i_icy_pub);
		PrintData ("%s(%d) : i_icy_url     = %s \n",__FUNCTION__,__LINE__,pstHttpSys->i_icy_url);

		if(pstHttpSys->b_icecast == TRUE)
		{
			pstMediaInfo->stream_type = DI_MEDIA_MPEG_TYPE_ES;
			pstMediaInfo->max_bitrate = pstHttpSys->i_icy_br*1024;    /* maximum stream bitreate, bps or 0 if unknown  */
			pstMediaInfo->duration = 0xFFFFFFFF;       /* duration of stream, in milliseconds or 0 if unknown */
			pstMediaInfo->nprograms =1;      /* total number of programs */
			pstMediaInfo->ntracks = 1;        /* total number of tracks */
			pstMediaInfo->video_pes_id = 0;
			pstMediaInfo->video_stream_id = 0;    /* or Video PID */
			pstMediaInfo->video_codec = DI_MEDIA_VIDEO_FORMAT_NONE; //DI_MEDIA_VIDEO_FORMAT_VC1;
			pstMediaInfo->audio_pes_id = 192;
			pstMediaInfo->audio_stream_id = 1;    /* or Audio PID */
			if(pstHttpSys->e_icy_codec == 0)
			{
				pstMediaInfo->audio_codec = DI_MEDIA_AUDIO_FORMAT_MP3;
			}
			else
			{
				pstMediaInfo->audio_codec = pstHttpSys->e_icy_codec;
			}
			pstMediaInfo->ucTotalAudioTrack = 1;
			pstMediaInfo->stAudioTrack[0].audio_pes_id = pstMediaInfo->audio_pes_id;
			pstMediaInfo->stAudioTrack[0].audio_stream_id = pstMediaInfo->audio_stream_id;
			pstMediaInfo->stAudioTrack[0].audio_codec = pstMediaInfo->audio_codec;

			pstMediaInfo->support_trick = 0;
			pstMediaInfo->support_seek = 0;
			pstMediaInfo->pcr_pid = 8191;        /* for TS stream */
			pstMediaInfo->isIcyProtocol = TRUE;

			/* Get HTTP chunked related infos */
			pstMediaInfo->isHttpChunked = pstHttpSys->b_chunked;
			pstMediaInfo->isRangeAccepted = pstHttpSys->b_range_accepted;
			pstMediaInfo->isRangeResponse = pstHttpSys->b_range_response;

			*pbIsICY = TRUE;
			s_tIcyInstance.nIcyMetaInt = pstHttpSys->i_icy_meta;
			if(s_tIcyInstance.nIcyMetaInt == 0)
			{
				s_tIcyInstance.nIcyMetaInt = 16*1024;
			}
		}
		else
		{
#endif
			pstMediaInfo->stream_type = DI_MEDIA_MPEG_TYPE_ASF;
			pstMediaInfo->max_bitrate = 875042;    /* maximum stream bitreate, bps or 0 if unknown  */
			pstMediaInfo->duration = 304833;       /* duration of stream, in milliseconds or 0 if unknown */
			pstMediaInfo->nprograms =1;      /* total number of programs */
			pstMediaInfo->ntracks = 2;        /* total number of tracks */
			pstMediaInfo->video_pes_id = 224;
			pstMediaInfo->video_stream_id = 2;    /* or Video PID */
			pstMediaInfo->video_codec = 9; //DI_MEDIA_VIDEO_FORMAT_VC1;
			pstMediaInfo->audio_pes_id = 192;
			pstMediaInfo->audio_stream_id = 1;    /* or Audio PID */
			pstMediaInfo->audio_codec = 14;
			pstMediaInfo->support_trick = 0;
			pstMediaInfo->support_seek = 1;
			pstMediaInfo->pcr_pid = 8191;        /* for TS stream */
			pstMediaInfo->isIcyProtocol = FALSE;

			/* Get HTTP chunked related infos */
			pstMediaInfo->isHttpChunked = pstHttpSys->b_chunked;
			pstMediaInfo->isRangeAccepted = pstHttpSys->b_range_accepted;
			pstMediaInfo->isRangeResponse = pstHttpSys->b_range_response;

			*pbIsICY = FALSE;
#ifdef ICY_PROTOCOL
		}
#endif
		DI_STREAM_HTTP_Close(stHandle);

#if REAL_TIME_MULTISOCKET_SUPPORT
#if defined(USE_MOOVBOX_CACHING)
#else
		if(pcRedirectionUrl != NULL)
		{
			DD_MEM_Free(pcRedirectionUrl);
			pcRedirectionUrl = NULL;
		}
#endif
#endif
	}

	PrintExit;
	return nRet;
}

void DI_STREAM_HTTP_RegisterCallback (void)
{
	VFIO_Callback_t stCBFunction;

	/* 1. VFIO에 callback 함수 등록 */
	(void)VK_memset (&stCBFunction, 0x0, sizeof(VFIO_Callback_t));

	stCBFunction.Open 			= (VFIOOpenCB)	DI_STREAM_HTTP_Open;
	stCBFunction.Close			= (VFIOCloseCB)	DI_STREAM_HTTP_Close;
	stCBFunction.Read			= (VFIOReadCB)	DI_STREAM_HTTP_Read;
	stCBFunction.Seek			= (VFIOSeekCB) 	DI_STREAM_HTTP_Seek;
	stCBFunction.Tell			= (VFIOTellCB) 	DI_STREAM_HTTP_Tell;
	stCBFunction.IsEOF			= (VFIOIsEOFCB) DI_STREAM_HTTP_IsEOF;
	stCBFunction.OpenNexus		= (VFIOOpenFromNexusCB)	 DI_STREAM_HTTP_OpenFromNexus;
	stCBFunction.SeekNexus		= (VFIOSeekForNexusCB) 	 DI_STREAM_HTTP_SeekForNexus;
	stCBFunction.BoundsNexus	= (VFIOBoundsForNexusCB) DI_STREAM_HTTP_BoundsForNexus;

	(void) VFIO_RegisterCallback (VFIO_CALLBACK_TYPE_STREAMING_HTTP, stCBFunction);

#if STREAM_BUFFERING
	sOpenFromNexusCounter = 0;
	sStreamBufferingRequestedSeekPoint = -1;
#endif

#if REAL_TIME_MULTISOCKET_SUPPORT
	VK_MEM_Memset(&s_tMultiHandle, 0x0, sizeof(s_tMultiHandle));
#if defined(USE_MOOVBOX_CACHING)
#else
	if(pcRedirectionUrl != NULL)
	{
		DD_MEM_Free(pcRedirectionUrl);
		pcRedirectionUrl = NULL;
	}
#endif
#endif
#if defined(USE_RAMBUFFERING_MOOVBOX_CACHING_SUPPORT)
	stMoovBox.eState = MOOVBOX_CACHING_STATE_READY;
#endif
	return;
}

void DI_STREAM_HTTP_IcyVfioRegisterCallback (void)
	{
	VFIO_Callback_t stCBFunction;

	/* 1. VFIO에 callback 함수 등록 */
	(void)VK_memset (&stCBFunction, 0x0, sizeof(VFIO_Callback_t));

		stCBFunction.Open 			= (VFIOOpenCB)	DI_STREAM_HTTP_ICY_Open;
		stCBFunction.Close			= (VFIOCloseCB)	DI_STREAM_HTTP_ICY_Close;
		stCBFunction.Read			= (VFIOReadCB)	DI_STREAM_HTTP_ICY_Read;
		stCBFunction.Seek			= (VFIOSeekCB) 	DI_STREAM_HTTP_ICY_Seek;
		stCBFunction.Tell			= (VFIOTellCB) 	DI_STREAM_HTTP_ICY_Tell;
		stCBFunction.IsEOF			= (VFIOIsEOFCB) DI_STREAM_HTTP_ICY_IsEOF;
		stCBFunction.OpenNexus		= (VFIOOpenFromNexusCB)	 DI_STREAM_HTTP_ICY_OpenFromNexus;
		stCBFunction.SeekNexus		= (VFIOSeekForNexusCB) 	 DI_STREAM_HTTP_ICY_SeekForNexus;
		stCBFunction.BoundsNexus	= (VFIOBoundsForNexusCB) DI_STREAM_HTTP_ICY_BoundsForNexus;

	(void) VFIO_RegisterCallback (VFIO_CALLBACK_TYPE_STREAMING_HTTP, stCBFunction);

	/* MetaInt 값은 Icy Probe시 변경됨에 葯라 그 이외에 부분만 여기서 초기화함. */
	s_tIcyInstance.ulICYSemId = 0;
	s_tIcyInstance.lIcyOffset = 0;
	s_tIcyInstance.nIcyReadSize = 0;
	s_tIcyInstance.nStartOffset = 0;
	s_tIcyInstance.nEndOffset = 0;
	s_tIcyInstance.nCurrentBufferOffset = 0;
	s_tIcyInstance.pcIcyReadBuffer=NULL;
	s_tIcyInstance.pcIcyReadTempBuffer=NULL;

	return;
}

void DI_STREAM_HTTP_UnregisterCallback (void)
{
#if REAL_TIME_MULTISOCKET_SUPPORT
	HUINT32 nWaitingTime = 0;
#if REAL_TIME_INDEXBUFFER_SUPPORT
	HUINT32 nIndex=0;
#endif
#endif

	PrintEnter;

#if REAL_TIME_MULTISOCKET_SUPPORT
	while((s_tMultiHandle.PlayerHandle != 0) || (s_tMultiHandle.IndexHandle != 0) || (s_tMultiHandle.DrmHandle != 0))
	{
		if(nWaitingTime != 5000)
		{
			PrintDebug("[%s] wait cloase(DI_STREAM_HTTP_UnregisterCallback)\n", __FUNCTION__);
			VK_TASK_Sleep(100);
			nWaitingTime=nWaitingTime+100;
		}
		else
		{
			PrintError("Fail close DI_STREAM_HTTP_UnregisterCallback\n");
			break;
		}
	}

	PrintDebug("[%s] PlayerHandle=%d, IndexHandle=%d DrmHandle=%d\n", __FUNCTION__, s_tMultiHandle.PlayerHandle, s_tMultiHandle.IndexHandle, s_tMultiHandle.DrmHandle);
#endif

	(void)VFIO_UnregisterCallback (VFIO_CALLBACK_TYPE_STREAMING_HTTP);

#if REAL_TIME_MULTISOCKET_SUPPORT
#if REAL_TIME_INDEXBUFFER_SUPPORT
	if(s_tMP4MoovInstance.pcData != NULL)
	{
		DD_MEM_Free(s_tMP4MoovInstance.pcData);
		s_tMP4MoovInstance.pcData = NULL;
		s_tMP4MoovInstance.lBuffer_CursorPosition = 0;
		s_tMP4MoovInstance.lBuffer_ReadPosition = 0;
	}
	else
	{
		for(nIndex=0;nIndex<REALTIME_PROBEBUFFER_CHUNK_TOTAL;nIndex++)
		{
			if(tRealtime_ProbeBuffer_Chunk[nIndex].pcData != NULL)
			{
				PrintDebug("##### [%s] MemFree(%d) #####\n", __FUNCTION__, nIndex);

				DD_MEM_Free(tRealtime_ProbeBuffer_Chunk[nIndex].pcData);
				s_nProbeBuffer_TotalUsedMemory -= tRealtime_ProbeBuffer_Chunk[nIndex].unData_Length+1;
				tRealtime_ProbeBuffer_Chunk[nIndex].pcData = NULL;
				tRealtime_ProbeBuffer_Chunk[nIndex].unData_Length = 0;
			}
		}
	}

	s_nMoovBuffer_TotalUsedMemory = 0;
#endif
#if defined(USE_MOOVBOX_CACHING)
#else
	if(pcRedirectionUrl != NULL)
	{
		DD_MEM_Free(pcRedirectionUrl);
		pcRedirectionUrl = NULL;
	}
#endif
	s_bDrmMode = FALSE;
#endif

#ifdef FILE_TEST
	s_bFileWriteComplete = TRUE;
#endif
#if defined(USE_RAMBUFFERING_MOOVBOX_CACHING_SUPPORT)
	if(stMoovBox.eState == MOOVBOX_CACHING_STATE_CACHED)
	{
		if(stMoovBox.pucMoov != NULL)
		{
			DD_MEM_Free(stMoovBox.pucMoov);
			stMoovBox.pucMoov = NULL;
		}
		VK_MEM_Memset(&stMoovBox,0x00,sizeof(MOOVBOX_INSTANCE_t));
	}
	stMoovBox.eState = MOOVBOX_CACHING_STATE_NONE;
#endif
	PrintExit;
	return;
}

DI_ERR_CODE DI_STREAM_HTTP_Control ( DI_IPTUNER_CMD_e cmd, HUINT64 val, HUINT64 val2 )
{
	DI_ERR_CODE nRet=DI_ERR_OK;
	PrintEnter;
/* XXX : This function does not work on HTTP Controller yet. So .... */
#if 0
	nRet = STREAM_HTTP_Control (&stHttpAccess, cmd , val, val2);
	if(nRet != DI_ERR_OK)
	{
		PrintError("%s (%d) Error> STREAM_HTTP_Control Fail \n",__FUNCTION__,__LINE__);
	}
#else
	UNUSED_PARAM(cmd);
	UNUSED_PARAM(val);
	UNUSED_PARAM(val2);
#endif

	PrintExit;

	return nRet;
}

/** Open From User Application and bmedia*/
int DI_STREAM_HTTP_Open  (const HINT8 *path, const HINT8 *mode, VFIO_Handle_t *ptHandle )
{
	HINT32 nRet = DI_IPT_SUCCESS;
	HINT32 vRet = VFIO_ERROR_SUCCESS;
	HTTP_ACCESS_SYS_t *pstHttpSys=NULL;
#if SUPPORT_COOKIE
	char *psCookie = NULL;
	URL_t tUrl;
#endif
#if REAL_TIME_MULTISOCKET_SUPPORT
	STREAM_ACCESS_t *ptHttpAccess=NULL;
#if REAL_TIME_INDEXBUFFER_SUPPORT
#if !defined(USE_MOOVBOX_CACHING)
	size_t sz_Real=0;
	size_t sz_Try = 0;
	HINT32 n_ReturnedValue = 0;
	HINT32 nIndex = 0;
	HINT32 	error_retrycount =0;
	int 	error_Timeoutcount =0;
	char *pBuffer=NULL;
#endif
#endif
#endif

	UNUSED_PARAM (mode);
	PrintEnter;

#if STREAM_BUFFERING
	/* 이 function은 Probe 를 위한 bmedia 에 의해 호출된다. */
	gOpenStatus = PROBE_OPEN;
#endif

#if REAL_TIME_MULTISOCKET_SUPPORT
	ptHttpAccess = DD_MEM_Alloc(sizeof(STREAM_ACCESS_t));
	if(ptHttpAccess == NULL)
	{
		PrintError("%s(%d) : Error> Fail Mem alloc.\n",__FUNCTION__,__LINE__);
		ptHandle->ucAvailable = 0;

		nRet = VFIO_ERROR_IN_DEVICE;
		PrintExit;
		return nRet;
	}
	else
	{
		VK_MEM_Memset(ptHttpAccess, 0x0, sizeof(STREAM_ACCESS_t));
	}
#endif

#if STREAM_BUFFERING
	PrintDebug("=== Probe Open\n");
	DNP_USER_DATA_t * pDnpUserData = DD_MEM_Alloc(sizeof(DNP_USER_DATA_t));
	pDnpUserData->indexer_last_seek_cursor = 0;
	pDnpUserData->p_align_buffer = NULL;
	pDnpUserData->align_buffer_start_index = -1;
	pDnpUserData->align_buffer_end_index = -1;
	ptHandle->pDnpUserData = pDnpUserData;

#if defined(USE_RAMBUFFERING_MOOVBOX_CACHING_SUPPORT)
	if(	stMoovBox.eState == MOOVBOX_CACHING_STATE_READY)
	{
		P_IPTUNER_HTTP_MoovboxCaching(path);
	}
#endif

#endif

#if SUPPORT_COOKIE
	VK_MEM_Memset(&tUrl, 0x0, sizeof(tUrl));
	UTIL_UrlParse(&tUrl, (const char *)path, 0);
	if(tUrl.psz_cookie != NULL)
	{
		psCookie = VK_StrDup(tUrl.psz_cookie);
	}
	UTIL_UrlClean(&tUrl);
#endif

#if REAL_TIME_MULTISOCKET_SUPPORT
	if(s_bDrmMode != FALSE)
	{
		/** psz_path must be free */
		if(pcRedirectionUrl != NULL)
		{
			ptHttpAccess->psz_path = VK_StrDup(pcRedirectionUrl);
		}
		else
		{
			ptHttpAccess->psz_path = VK_StrDup((char *)path);
		}

#if SUPPORT_COOKIE
		if(psCookie != NULL)
		{
			ptHttpAccess->psz_cookie = psCookie;
		}
#endif

		if(ptHttpAccess->psz_path == NULL)
		{
			PrintError("%s(%d) : Error> Fail Mem alloc.\n",__FUNCTION__,__LINE__);
			ptHandle->ucAvailable = 0;
			vRet = VFIO_ERROR_IN_DEVICE;
		}
		else
		{
			nRet = STREAM_HTTP_Open (ptHttpAccess);
			if(nRet != DI_IPT_SUCCESS)
			{
				PrintError("%s(%d) : Error> HTTP_Open Fail.\n",__FUNCTION__,__LINE__);
				ptHttpAccess->b_alive = TRUE;
				ptHttpAccess = NULL;
				ptHandle->ucAvailable = 0;
				vRet = VFIO_ERROR_IN_DEVICE;
			}
			else
			{
				PrintData ("%s(%d) : i_update       = %d \n",__FUNCTION__,__LINE__,ptHttpAccess->info.i_update);
				PrintData ("%s(%d) : i_size         = %"PRId64"\n",__FUNCTION__,__LINE__,ptHttpAccess->info.i_size);
				PrintData ("%s(%d) : i_pos          = %"PRId64"\n",__FUNCTION__,__LINE__,ptHttpAccess->info.i_pos);
				PrintData ("%s(%d) : b_eof          = %d \n",__FUNCTION__,__LINE__,ptHttpAccess->info.b_eof);
				PrintData ("%s(%d) : i_title        = %d \n",__FUNCTION__,__LINE__,ptHttpAccess->info.i_title);
				PrintData ("%s(%d) : i_seekpoint    = %d \n",__FUNCTION__,__LINE__,ptHttpAccess->info.i_seekpoint);
				PrintData ("%s(%d) : b_prebuffered  = %d \n",__FUNCTION__,__LINE__,ptHttpAccess->info.b_prebuffered);

				pstHttpSys = (HTTP_ACCESS_SYS_t*) ptHttpAccess->p_sys;
				ptHandle->ucAvailable = 1;
				ptHandle->tFile.callback.unHandle = (HUINT32) pstHttpSys->fd; /** http socket descriptor */
				s_tMultiHandle.DrmHandle = (HUINT32) pstHttpSys->fd;

				PrintData ("%s(%d) : callback handle=0x%x\n",__FUNCTION__,__LINE__, ptHandle->tFile.callback.unHandle);
			}

			if(s_ulHttpSemId == 0)
			{
				PrintData ("%s(%d) : sem create\n",__FUNCTION__,__LINE__);
				nRet = VK_SEM_Create(&s_ulHttpSemId,"httpSema", DEFAULT_SUSPEND_TYPE);
				if( nRet != VK_OK )
				{
					PrintError("%s(%d) Error(0x%x) \n\r",__FUNCTION__,__LINE__, nRet);
					vRet = VFIO_ERROR_IN_DEVICE;
				}
			}
		}

		if( vRet != VFIO_ERROR_SUCCESS)
		{
			if(ptHttpAccess != NULL)
			{
				(void)STREAM_HTTP_Close (ptHttpAccess);

				if(ptHttpAccess->psz_path != NULL)
				{
					DD_MEM_Free(ptHttpAccess->psz_path);
					ptHttpAccess->psz_path =NULL;
				}
				/* else { continue; } */
#if SUPPORT_COOKIE
				if(ptHttpAccess->psz_cookie != NULL)
				{
					DD_MEM_Free(ptHttpAccess->psz_cookie);
					ptHttpAccess->psz_cookie =NULL;
				}
				/* else { continue; } */
#endif
				if(ptHttpAccess != NULL)
				{
					DD_MEM_Free(ptHttpAccess);
					ptHttpAccess =NULL;
				}
			}
		}
		else
		{
			ptHandle->pDnpUserData = ptHttpAccess;
		}

		PrintExit;
		return vRet;
	}

	/** psz_path must be free */
#if defined(USE_MOOVBOX_CACHING)
	if(pcRedirectionUrl != NULL)
	{
		ptHttpAccess->psz_path = VK_StrDup(pcRedirectionUrl);
		PrintDebug ("Using Redirected URL [%s]\n", pcRedirectionUrl);
	}
	else
	{
		ptHttpAccess->psz_path = VK_StrDup((char *)path);
	}
#else
	ptHttpAccess->psz_path = VK_StrDup((char *)path);
#endif

#if SUPPORT_COOKIE
	if(psCookie != NULL)
	{
		ptHttpAccess->psz_cookie = psCookie;
	}
#endif
	if(ptHttpAccess->psz_path == NULL)
	{
		PrintError("%s(%d) : Error> Fail Mem alloc.\n",__FUNCTION__,__LINE__);
		ptHandle->ucAvailable = 0;
		vRet = VFIO_ERROR_IN_DEVICE;
	}
	else
	{
		nRet = STREAM_HTTP_Open (ptHttpAccess);
		if(nRet != DI_IPT_SUCCESS)
		{
			PrintError("%s(%d) : Error> HTTP_Open Fail.\n",__FUNCTION__,__LINE__);
			ptHttpAccess->b_alive = TRUE;
			ptHandle->ucAvailable = 0;

			if(ptHttpAccess->psz_path != NULL)
			{
				DD_MEM_Free  (ptHttpAccess->psz_path);
				ptHttpAccess->psz_path =NULL;
			}
			/* else { continue; } */

			vRet = VFIO_ERROR_IN_DEVICE;
		}
		else
		{
#if REAL_TIME_MULTISOCKET_SUPPORT
			if(VK_strcmp(ptHttpAccess->psz_path, path))
			{
#if defined(USE_MOOVBOX_CACHING)
				if(pcRedirectionUrl == NULL)
				{
					pcRedirectionUrl = VK_StrDup(ptHttpAccess->psz_path);
				}
#else
				pcRedirectionUrl = VK_StrDup(ptHttpAccess->psz_path);
				PrintError("### %s(%d) : Redirection Url=%s ###\n",__FUNCTION__,__LINE__, pcRedirectionUrl);
#endif
			}
#endif
			PrintData ("%s(%d) : i_update       = %d \n",__FUNCTION__,__LINE__,ptHttpAccess->info.i_update);
			PrintData ("%s(%d) : i_size         = %"PRId64"\n",__FUNCTION__,__LINE__,ptHttpAccess->info.i_size);
			PrintData ("%s(%d) : i_pos          = %"PRId64"\n",__FUNCTION__,__LINE__,ptHttpAccess->info.i_pos);
			PrintData ("%s(%d) : b_eof          = %d \n",__FUNCTION__,__LINE__,ptHttpAccess->info.b_eof);
			PrintData ("%s(%d) : i_title        = %d \n",__FUNCTION__,__LINE__,ptHttpAccess->info.i_title);
			PrintData ("%s(%d) : i_seekpoint    = %d \n",__FUNCTION__,__LINE__,ptHttpAccess->info.i_seekpoint);
			PrintData ("%s(%d) : b_prebuffered  = %d \n",__FUNCTION__,__LINE__,ptHttpAccess->info.b_prebuffered);

			s_nFileLength = ptHttpAccess->info.i_size;

			/* Skyplayer에서 작은 파일(21kbyte)을 실행시 rebooting문제 때문에 임시 적용. 차후 검토후 제거 예정 */
			if((ptHttpAccess->info.i_size > 0) && (ptHttpAccess->info.i_size < ((1024*1024)/40)))
			{
				PrintError("%s(%d) : Error> HTTP_Open Fail.\n",__FUNCTION__,__LINE__);
				PrintError("%s(%d) : Error> Invalid File Size(File Size = %"PRId64".\n",__FUNCTION__,__LINE__, ptHttpAccess->info.i_size);
				ptHttpAccess->b_alive = TRUE;
				ptHandle->ucAvailable = 0;

				if(pcRedirectionUrl != NULL)
				{
					DD_MEM_Free(pcRedirectionUrl);
					pcRedirectionUrl = NULL;
				}
				/* else { continue; } */

				vRet = VFIO_ERROR_IN_DEVICE;
			}
			else
			{
				pstHttpSys = (HTTP_ACCESS_SYS_t*) ptHttpAccess->p_sys;
				ptHandle->ucAvailable = 1;
				ptHandle->tFile.callback.unHandle = (HUINT32) pstHttpSys->fd; /** http socket descriptor */

#ifdef FILE_TEST
				HINT64 sz_RemainSize=0;
				HINT64 sz_RealTemp=0;
				HINT64 sz_TryTemp=0;

				if(s_bFileWriteComplete == FALSE)
				{
					pBuffer = DD_MEM_Alloc(64*1024);
					VK_MEM_Memset(pBuffer, 0x0, 64*1024);
					s_TempFd = open ( "/mnt/hd2/Test.mp4" , O_RDWR|O_CREAT, 0666);
					sz_RemainSize = ptHttpAccess->info.i_size;

					do
					{
						if(sz_RemainSize > 60*1024)
						{
							sz_TryTemp = 60*1024;
						}
						else
						{
							sz_TryTemp = sz_RemainSize;
						}

						sz_RealTemp=0;

						do
						{
							n_ReturnedValue = STREAM_HTTP_Read ( ptHttpAccess, (HUINT8 *)pBuffer+sz_RealTemp, sz_TryTemp - sz_RealTemp );
							sz_RealTemp += n_ReturnedValue;
						}
						while (sz_RealTemp < sz_TryTemp);

						sz_RemainSize = sz_RemainSize-sz_RealTemp;
						write(s_TempFd, pBuffer, sz_RealTemp);
						VK_MEM_Memset(pBuffer, 0x0, 64*1024);
					}
					while(sz_RemainSize > 0);

					close (s_TempFd);

					if(pBuffer != NULL)
					{
						DD_MEM_Free(pBuffer);
						pBuffer = NULL;
					}

					s_bFileWriteComplete = TRUE;
				}
#endif

#if REAL_TIME_INDEXBUFFER_SUPPORT
				s_ullCurrent_ProbeBuffer_Index = 0;
				VK_MEM_Memset(&tRealtime_ProbeBuffer_Chunk, 0x0, sizeof(tRealtime_ProbeBuffer_Chunk));

				if((pstHttpSys->url.psz_path != NULL) && (pstHttpSys->psz_mime != NULL) && (pstHttpSys->b_icecast != TRUE))
				{
					if((VK_strstr(pstHttpSys->psz_mime,"mp4") !=NULL) || (VK_strstr(pstHttpSys->url.psz_path,".mp4") != NULL))
					{
#if defined(USE_MOOVBOX_CACHING)
						P_IPTUNER_HTTP_MoovboxCaching((HINT8*) ptHttpAccess->psz_path);
#else
						PrintData ("%s(%d) : psz_mime       = %s \n",__FUNCTION__,__LINE__,pstHttpSys->psz_mime);
						pBuffer = DD_MEM_Alloc(256);
						if(pBuffer != NULL)
						{
							VK_MEM_Memset(pBuffer, 0x0, 256);

							sz_Try = 256;
							sz_Real=0;
							error_retrycount = 0;

							do
							{
								n_ReturnedValue = STREAM_HTTP_Read ( ptHttpAccess, (HUINT8 *)pBuffer+sz_Real, sz_Try - sz_Real );
								if(n_ReturnedValue >= 0)
								{
									sz_Real += n_ReturnedValue;
									error_retrycount++;
								}

								if(n_ReturnedValue == DI_IPT_ETIMEOUT)
								{
									if(error_Timeoutcount != 2)
									{
										error_Timeoutcount++;
										continue;
									}
									else
									{
										error_Timeoutcount = 0;
									}
								}

								if(error_retrycount == 40)
								{
									break;
								}
							}
							while (sz_Real < sz_Try);

							if(sz_Real == sz_Try)
							{
								nIndex = 0;
								n_ReturnedValue = 0;

								do
								{
									if((pBuffer[nIndex] == 'm') && (pBuffer[nIndex+1] == 'o') && (pBuffer[nIndex+2] == 'o') && (pBuffer[nIndex+3] == 'v'))
									{
										n_ReturnedValue = 1;
										break;
									}

									nIndex++;
								}
								while(nIndex < (256-5));

								if(n_ReturnedValue != 0)
								{
									/* redmin:#61193 Youtube memory leak bug fix */
									if( s_tMP4MoovInstance.pcData != NULL )
									{
										DD_MEM_Free( s_tMP4MoovInstance.pcData );
										s_tMP4MoovInstance.pcData = NULL;
									}

									if(ptHttpAccess->info.i_size < 200*1024*1024)
									{
										s_tMP4MoovInstance.pcData = DD_MEM_Alloc(MP4MOOV_BLOCK_MAX/6*4);
									}
									else
									{
										s_tMP4MoovInstance.pcData = DD_MEM_Alloc(MP4MOOV_BLOCK_MAX);
									}

									if(s_tMP4MoovInstance.pcData != NULL)
									{
										s_tMP4MoovInstance.lBuffer_CursorPosition = 0;
										s_tMP4MoovInstance.lBuffer_ReadPosition = 0;
									}
								}
								else
								{
									PrintError("%s(%d) Not Found Moov => Probe Buffer\n\r",__FUNCTION__,__LINE__);
									if(s_tMP4MoovInstance.pcData)
									{
										DD_MEM_Free(s_tMP4MoovInstance.pcData);
										s_tMP4MoovInstance.pcData=NULL;
									}
								}
							}
							else
							{
								if(s_tMP4MoovInstance.pcData)
								{
									DD_MEM_Free(s_tMP4MoovInstance.pcData);
									s_tMP4MoovInstance.pcData=NULL;
								}
							}

							if(pBuffer != NULL)
							{
								DD_MEM_Free(pBuffer);
								pBuffer = NULL;
							}

							if(STREAM_HTTP_Seek( ptHttpAccess, 0) != DI_IPT_SUCCESS)
							{
								PrintError("%s(%d) Change Mode Moov Buffer => Probe Buffer\n\r",__FUNCTION__,__LINE__);
								if(s_tMP4MoovInstance.pcData != NULL)
								{
									DD_MEM_Free(s_tMP4MoovInstance.pcData);
									s_tMP4MoovInstance.pcData = NULL;
								}
							}
						}
#endif
					}
				}
				else
				{
					if(s_tMP4MoovInstance.pcData)
					{
						DD_MEM_Free(s_tMP4MoovInstance.pcData);
						s_tMP4MoovInstance.pcData=NULL;
					}
				}
#else
#if REAL_TIME_CHUNKBUFFER_SUPPORT
				if(s_tMP4MoovInstance.pcData)
				{
					DD_MEM_Free(s_tMP4MoovInstance.pcData);
					s_tMP4MoovInstance.pcData=NULL;
				}
#endif
#endif
				PrintData ("%s(%d) : callback handle=0x%x\n",__FUNCTION__,__LINE__, ptHandle->tFile.callback.unHandle);

				nRet = VK_SEM_Create(&s_ulHttpSemId,"httpSema", DEFAULT_SUSPEND_TYPE);
				if( nRet != VK_OK )
				{
					PrintError("%s(%d) Error(0x%x) \n\r",__FUNCTION__,__LINE__, nRet);
					vRet = VFIO_ERROR_IN_DEVICE;
				}
				else
				{
					s_tMultiHandle.ProbeHandle = (HUINT32) pstHttpSys->fd;
					ptHandle->pDnpUserData = ptHttpAccess;
				}
			}
		}
	}

	if( vRet != VFIO_ERROR_SUCCESS)
	{
		PrintError("%s(%d) Error(0x%x) \n\r",__FUNCTION__,__LINE__, nRet);

		if(ptHttpAccess != NULL)
		{
			(void)STREAM_HTTP_Close (ptHttpAccess);
		}

		if(ptHttpAccess->psz_path != NULL)
		{
			DD_MEM_Free(ptHttpAccess->psz_path);
			ptHttpAccess->psz_path =NULL;
		}
		/* else { continue; } */
#if SUPPORT_COOKIE
		if(ptHttpAccess->psz_cookie != NULL)
		{
			DD_MEM_Free(ptHttpAccess->psz_cookie);
			ptHttpAccess->psz_cookie =NULL;
		}
		/* else { continue; } */
#endif
		if(ptHttpAccess != NULL)
		{
			DD_MEM_Free(ptHttpAccess);
			ptHttpAccess =NULL;
		}
		/* else { continue; } */
		PrintExit;
		return VFIO_ERROR_IN_DEVICE;
	}

	PrintExit;
	return vRet;
#endif /* REAL_TIME_MULTISOCKET_SUPPORT */

	/** psz_path must be free */
	VK_memset (&stHttpAccess, 0, sizeof (STREAM_ACCESS_t));

	stHttpAccess.psz_path = VK_StrDup((char *)path);

#if SUPPORT_COOKIE
	if(psCookie != NULL)
	{
		stHttpAccess.psz_cookie = psCookie;
	}
#endif
	if(stHttpAccess.psz_path == NULL)
	{
		PrintError("%s(%d) : Error> Fail Mem alloc.\n",__FUNCTION__,__LINE__);
		ptHandle->ucAvailable = 0;
		vRet = VFIO_ERROR_IN_DEVICE;
	}
	else
	{
		nRet = STREAM_HTTP_Open (&stHttpAccess);
		if(nRet != DI_IPT_SUCCESS)
		{
			PrintError("%s(%d) : Error> HTTP_Open Fail.\n",__FUNCTION__,__LINE__);
			stHttpAccess.b_alive = TRUE;
			ptHandle->ucAvailable = 0;

			if(stHttpAccess.psz_path != NULL)
			{
				DD_MEM_Free(stHttpAccess.psz_path);
				stHttpAccess.psz_path =NULL;
			}
			/* else { continue; } */
#if SUPPORT_COOKIE
			if(stHttpAccess.psz_cookie != NULL)
			{
				DD_MEM_Free(stHttpAccess.psz_cookie);
				stHttpAccess.psz_cookie =NULL;
			}
			/* else { continue; } */
#endif
			vRet = VFIO_ERROR_IN_DEVICE;
		}
		else
		{
			PrintData ("%s(%d) : i_update       = %d \n",__FUNCTION__,__LINE__,stHttpAccess.info.i_update);
			PrintData ("%s(%d) : i_size         = %"PRId64"\n",__FUNCTION__,__LINE__,stHttpAccess.info.i_size);
			PrintData ("%s(%d) : i_pos          = %"PRId64"\n",__FUNCTION__,__LINE__,stHttpAccess.info.i_pos);
			PrintData ("%s(%d) : b_eof          = %d \n",__FUNCTION__,__LINE__,stHttpAccess.info.b_eof);
			PrintData ("%s(%d) : i_title        = %d \n",__FUNCTION__,__LINE__,stHttpAccess.info.i_title);
			PrintData ("%s(%d) : i_seekpoint    = %d \n",__FUNCTION__,__LINE__,stHttpAccess.info.i_seekpoint);
			PrintData ("%s(%d) : b_prebuffered  = %d \n",__FUNCTION__,__LINE__,stHttpAccess.info.b_prebuffered);

			if((stHttpAccess.info.i_size > 0) && (stHttpAccess.info.i_size < 4*1024))
			{
				PrintError("%s(%d) : Error> HTTP_Open Fail.\n",__FUNCTION__,__LINE__);
				PrintError("%s(%d) : Error> Invalid File Size(File Size = %"PRId64".\n",__FUNCTION__,__LINE__, stHttpAccess.info.i_size);
				stHttpAccess.b_alive = TRUE;
				ptHandle->ucAvailable = 0;

				(void)STREAM_HTTP_Close (&stHttpAccess);

				if(stHttpAccess.psz_path != NULL)
				{
					DD_MEM_Free(stHttpAccess.psz_path);
					stHttpAccess.psz_path =NULL;
				}
				/* else { continue; } */
#if SUPPORT_COOKIE
				if(stHttpAccess.psz_cookie != NULL)
				{
					DD_MEM_Free(stHttpAccess.psz_cookie);
					stHttpAccess.psz_cookie =NULL;
				}
				/* else { continue; } */
#endif
				vRet = VFIO_ERROR_IN_DEVICE;
			}
			else
			{
				pstHttpSys = (HTTP_ACCESS_SYS_t*) stHttpAccess.p_sys;
				ptHandle->ucAvailable = 1;
				ptHandle->tFile.callback.unHandle = (HUINT32) pstHttpSys->fd; /** http socket descriptor */
#if STREAM_BUFFERING
				s_nFileLength = stHttpAccess.info.i_size;
#endif

				PrintData ("%s(%d) : callback handle=0x%x\n",__FUNCTION__,__LINE__, ptHandle->tFile.callback.unHandle);
			}
		}

		nRet = VK_SEM_Create(&s_ulHttpSemId,"httpSema", DEFAULT_SUSPEND_TYPE);
		if( nRet != VK_OK )
		{
			PrintError("%s(%d) Error(0x%x) \n\r",__FUNCTION__,__LINE__, nRet);

			(void)STREAM_HTTP_Close (&stHttpAccess);

			if(stHttpAccess.psz_path != NULL)
			{
				DD_MEM_Free(stHttpAccess.psz_path);
				stHttpAccess.psz_path =NULL;
			}
			/* else { continue; } */
#if SUPPORT_COOKIE
			if(stHttpAccess.psz_cookie != NULL)
			{
				DD_MEM_Free(stHttpAccess.psz_cookie);
				stHttpAccess.psz_cookie =NULL;
			}
			/* else { continue; } */
#endif
			PrintExit;
			return VFIO_ERROR_IN_DEVICE;
		}
	}

	PrintExit;
	return vRet;
}

#if STREAM_BUFFERING
static int P_STREAM_HTTP_GetBufferSize (void)
{
	int nRet=0;

	if((int)g_httpptrW > (int)g_httpptrR)
	{
		nRet = (int)g_httpptrW - (int)g_httpptrR;
	}
	else if ((int)g_httpptrW == (int)g_httpptrR)
	{
		/** 초기 state를 제외하고는 g_httpptrW 와 ptrR이 동일할 수 없다. */
		nRet = 0;
	}
	else
	{
		nRet = g_HTTP_BUFFERING_SIZE - ((int)g_httpptrR - (int)g_httpptrW);
	}

	return nRet;
}

static int P_STREAM_HTTP_ClearBuffer (HINT64 off_start)
{
	int nRet = 0;
	PrintEnter;

	(void) VK_SEM_Get (s_ulBufSema);
	if (NULL != g_HttpBuffer)
	{
		//VK_memset (g_HttpBuffer, 0, sizeof(g_HTTP_BUFFERING_SIZE));
#if 0 /* warning: assignment makes pointer from integer without a cast */
		g_httpptrW = HTTP_BUF_START;
		g_httpptrR = HTTP_BUF_START;
#else
		// HTTP_BUF_START = (HINT32)g_HttpBuffer;
		g_httpptrW = (HUINT8 *)HTTP_BUF_START;
		g_httpptrR = (HUINT8 *)HTTP_BUF_START;
#endif
	}
	else nRet = -1;
	sOffsetStart = off_start;
	sOffsetEnd = off_start;

	PrintExit;
	(void) VK_SEM_Release (s_ulBufSema);
	return nRet;
}

static int P_STREAM_HTTP_WriteBuffer ( HUINT8 *pBuffer, size_t len)
{
	size_t space =0;
	int size1=0;
	int size2=0;
	int i=0;

	//PrintEnter;

	space = (g_HTTP_BUFFERING_SIZE - P_STREAM_HTTP_GetBufferSize()) - 1;

	if(space < len)
	{
		while ((space = (g_HTTP_BUFFERING_SIZE - P_STREAM_HTTP_GetBufferSize()) -1) < len )
		{
			if(stHttpAccess.b_alive == FALSE)
			{
				PrintDebug("%s(%d) Session is canceled.\n",__FUNCTION__,__LINE__);
				return -1;
			}
			else
			{
				if(stHttpAccess.info.b_prebuffered == TRUE)
				{
					PrintDebug("%s(%d) Reset Prebuffering\n",__FUNCTION__,__LINE__);
					return -1;
				}

				PrintDebug("%s(%d) HTTP_WRITE Waiting IO for %d sec.\n",__FUNCTION__,__LINE__,i++);
				VK_TASK_Sleep(100); /* 1000 -> 100 for better respose time */
			}
		}
	}

	(void) VK_SEM_Get (s_ulBufSema);

	if ( ((int)g_httpptrW + (int)len) > (int)HTTP_BUF_END)
	{
		size1 = (int)HTTP_BUF_END - (int)g_httpptrW;
		VK_memcpy (g_httpptrW,pBuffer,size1);

		size2 = (int)len-(int)size1;
		VK_memcpy((void*)HTTP_BUF_START, pBuffer+size1,size2);

		g_httpptrW = (HUINT8*)(HTTP_BUF_START + size2);
	}
	else
	{
		VK_memcpy(g_httpptrW,pBuffer,len);
		g_httpptrW += len;
	}

	/* Update the END offset of window */
	sOffsetEnd += len;

	(void) VK_SEM_Release (s_ulBufSema);

//	PrintDatata("%s(%d) Write Buffer s(%"PRId64") e(%"PRId64")\n",__FUNCTION__,__LINE__,sOffsetStart, sOffsetEnd);

#if DUMP_PREBUFFER
	{
		HUINT8 *p=HTTP_BUF_START;
		PrintDebug ("\n");
		for(;p<HTTP_BUF_END;p++)
		{
			PrintDebug ("0x%x ",p[0]);
			if(!(p-HTTP_BUF_START)%32) PrintDebug ("\n");
		}
		PrintDebug ("\n");
	}
#endif

	//PrintExit;
	return len;
}


/** Buffering Function */
void STREAM_HTTP_PreBuffering (DI_IPTUNER_Index_e nTunerIndex)
{
	HUINT8 pBuffer [HTTP_READ_BLOCK_SIZE];
	int nRead=0;
#if 0 /* warning: unused variable */
	HINT32 szBuf=0;
	DI_IPTUNER_Index_e eMsg = nTunerIndex;
#else
	UNUSED_PARAM(nTunerIndex);
#endif

	PrintEnter;
	s_ePreBufferingTaskState = DI_IPTUNER_HTTP_PREBUFFERING_TASK_STATE_STARTED;
	s_eChangePreBufferingTaskState = DI_IPTUNER_HTTP_PREBUFFERING_TASK_STATE_STARTED;

#if 1
	PrintDebug("[JLEE:%s:%d] --> +P_STREAM_HTTP_ClearBuffer()\n", __func__, __LINE__);
	P_STREAM_HTTP_ClearBuffer(0);
	PrintDebug("[JLEE:%s:%d] --> -P_STREAM_HTTP_ClearBuffer()\n", __func__, __LINE__);
#endif

	while (1)
	{
reset_buffer:
		//VK_SEM_Get (s_ulPreBufferingLock); 	//--------------------------------	Critical Section
		if(g_HttpBuffer == NULL)
		{
			PrintError("g_HttpBuffer is NULL\n");
			break; /* FreeBuffer is called by stream close */
		}

		if(s_eChangePreBufferingTaskState == DI_IPTUNER_HTTP_PREBUFFERING_TASK_STATE_KILLING)
		{
			PrintDebug("Break PreBuffering Main While\n");
			break;
		}

		if(sStreamBufferingRequestedSeekPoint != -1)
		{
			VK_SEM_Get(s_ulPreBufferingLock);
			PrintDebug("==================\n============\nRequestedSeekPoint Received\n==============\n");
			/* reset window pos */
			P_STREAM_HTTP_ClearBuffer(sStreamBufferingRequestedSeekPoint);


			{
				HUINT32 nNetworkRetryCount = 0;
				while(DI_IPT_SUCCESS != STREAM_HTTP_Seek ( &stHttpAccess, sOffsetStart ))
				{
					nNetworkRetryCount++;
					PrintError ("%s(%d) Error> STREAM_HTTP_Seek fail Retrying...\n",__FUNCTION__,__LINE__);
					VK_TASK_Sleep(100);
					if(nNetworkRetryCount > DI_IPTUNER_DNP_RETRY_COUNT_ON_NET_ERROR)
					{
						PrintError("Send Failed code To Buffering Reader(Player,Indexer)\n");
						s_NetworkError = TRUE;
						VK_SEM_Release(s_ulPreBufferingLock);
						goto exit_stop;
					}
				}
			}

			sStreamBufferingRequestedSeekPoint = -1; /* seek request is done */
			VK_SEM_Release(s_ulPreBufferingLock);

			PrintDebug("===============\nSeek Done\n");
		}

		if(HIGHEST_HTTP_BUFFERING_THRESHOLD > P_STREAM_HTTP_GetBufferSize())
		{
			/***********************************************************************
			 *  1. General Receiving Control
			 *     VFIO 가 Data를 소비하는 도중에 Buffer를 유동적으로 채워주는 루틴.
			 *     ReadBuffer 와 동시에 호출된다.
			 ***********************************************************************/
			VK_memset  (pBuffer,0,HTTP_READ_BLOCK_SIZE);
			{
				if(stHttpAccess.info.i_size < stHttpAccess.info.i_pos)
				{
					PrintError ("%s(%d) i_size = %"PRId64", i_pos = %"PRId64"\n",__FUNCTION__,__LINE__, stHttpAccess.info.i_size, stHttpAccess.info.i_pos);
					break;
				}

				HUINT32 nNetworkRetryCount = 0;
				while((nRead = STREAM_HTTP_Read ( &stHttpAccess, pBuffer, HTTP_READ_BLOCK_SIZE)) <= 0)
				{
#if 0 /* warning: unused variable */
					off_t cur = stHttpAccess.info.i_pos;
#endif
					if(stHttpAccess.info.b_eof == TRUE)
					{
						//PrintError ("%s(%d) Finish Buffering (End of Stream) (%d)\n",__FUNCTION__,__LINE__,nRead);
						goto reset_buffer;
					}
					if(s_eChangePreBufferingTaskState == DI_IPTUNER_HTTP_PREBUFFERING_TASK_STATE_KILLING)
					{
						PrintDebug("Break PreBuffering Main While\n");
						goto exit_stop;
						//break;
					}
					nNetworkRetryCount++;
					PrintError ("%s(%d) Error> STREAM_HTTP_Read fail Retrying... ( %d ) (%"PRId64" : %"PRId64")\n",__FUNCTION__,__LINE__,nNetworkRetryCount,stHttpAccess.info.i_pos,stHttpAccess.info.i_size);
#if 0
					if(DI_IPT_SUCCESS !=STREAM_HTTP_Seek ( &stHttpAccess, cur ))
					{
						PrintError ("%s(%d) Error> STREAM_HTTP_Seek fail...\n",__FUNCTION__,__LINE__);
					}
#endif
					VK_TASK_Sleep(100);

					if(nNetworkRetryCount > DI_IPTUNER_DNP_RETRY_COUNT_ON_NET_ERROR)
					{
						PrintError("Send Failed code To Buffering Reader(Player,Indexer)\n");
						s_NetworkError = TRUE;
						goto exit_stop;
					}
				}
			}

			(void) P_STREAM_HTTP_WriteBuffer (pBuffer,nRead);
			if(stHttpAccess.b_alive == FALSE)
			{
				PrintError ("%s(%d) Session is canceled.\n",__FUNCTION__,__LINE__);
				break;
			}
			if(stHttpAccess.info.b_eof == TRUE)
			{
				//PrintDebug ("%s(%d) Finish Buffering (End of Stream) (%d)\n",__FUNCTION__,__LINE__,nRead);
				VK_TASK_Sleep(100);
				continue;
			}

			if(stHttpAccess.info.b_prebuffered == TRUE)
			{
				stHttpAccess.info.b_prebuffered = FALSE;
				//VK_SEM_Release(s_ulPreBufferingLock);  //--------------------------------  Critical Section
				//VK_TASK_Sleep(100);
				goto reset_buffer;
			}

/*************************
*	Send Msg to Monitoring task
*************************/
//				VK_MSG_Send(gPreBufferingMsgId, &eMsg, sizeof(DI_IPTUNER_Index_e));
			}
	} /* end while */

	//VK_SEM_Release(s_ulPreBufferingLock); 	//--------------------------------	Critical Section
exit_stop:
	s_ePreBufferingTaskState = DI_IPTUNER_HTTP_PREBUFFERING_TASK_STATE_DONE;
	PrintExit;
	return;
}

/* Blocking Read */
ssize_t P_STREAM_HTTP_ReadBuffer (STREAM_ACCESS_t *pAccess, HUINT8 *pBuffer, size_t i_len)
{
	int size1=0;
	int size2=0;
	int szBufSnapShot=0;

	szBufSnapShot = P_STREAM_HTTP_GetBufferSize();

	//PrintEnter;
	if( szBufSnapShot < (int)i_len )
	{
		if(pAccess->info.b_eof == TRUE)
		{
			i_len = szBufSnapShot;
		}
		else
		{
			while ((szBufSnapShot = P_STREAM_HTTP_GetBufferSize()) < (int)i_len)
			{
				if (s_NetworkError || (pAccess->info.i_size <= (pAccess->info.i_pos + szBufSnapShot)) ||
					(s_eChangePreBufferingTaskState == DI_IPTUNER_HTTP_PREBUFFERING_TASK_STATE_KILLING))
				{
					if(szBufSnapShot < (int)i_len)
					{
						i_len = szBufSnapShot;
					}

					PrintDebug("%s(%d) Info> !!!!!!!! Exit Waiting !!!!!!!! \n", __FUNCTION__,__LINE__);
					break;
				}

				PrintError ("%s(%d) Waiting> PreBuffer hasn't enough data .... szContent(%"PRId64") szBuffer(%"PRId64")\n"
				,__FUNCTION__
				,__LINE__
				,pAccess->info.i_size
				,pAccess->info.i_pos+szBufSnapShot);

				/* We need to wait some period.*/
				VK_TASK_Sleep(100);
			} /* end of while */
		}
	}

	/* The Belows are in Critical Section */
	(void) VK_SEM_Get (s_ulBufSema);
	if ( (int)(g_httpptrR+i_len) > (int)HTTP_BUF_END)
	{
		size1 = (int)HTTP_BUF_END - (int)g_httpptrR;
		VK_memcpy (pBuffer, g_httpptrR, size1);
		VK_memset  (g_httpptrR,0,size1);
		size2 = (i_len-size1);

		VK_memcpy ((pBuffer+size1), (const void*)HTTP_BUF_START, size2);
		VK_memset  ((void*)HTTP_BUF_START,0,size2);

		g_httpptrR = (HUINT8 *)(HTTP_BUF_START+size2);
	}
	else
	{
		VK_memcpy (pBuffer, g_httpptrR, i_len);
		VK_memset  (g_httpptrR,0,i_len);

		g_httpptrR += i_len;
	}

#if 0
	{
		int i=0;
		for(i=1;i<(i_len+1);i++)
		{
			PrintDebug ("0x%x ",pBuffer[i-1]);
			if(!(i%32)) PrintDebug ("\n");
		}
	}
#endif

	/* Update the start offset of window */
	sOffsetStart += i_len;

	(void) VK_SEM_Release (s_ulBufSema);
	//PrintExit;

	PrintDebug ("%s(%d) : Exit Info> EOF : i_len (%d)\n",__FUNCTION__,__LINE__, i_len);
	return i_len;
}

DI_ERR_CODE DI_STREAM_HTTP_CreateBuffer (DI_MEDIA_INFO_t *pstMediaInfo)
{
	DI_ERR_CODE nRet= DI_ERR_OK;
	HUINT32 nBufSize = 0;
	HINT64 nTotalSize = stHttpAccess.info.i_size;
#if 0 /* warning: unused variable */
	HUINT32 nDuration = pstMediaInfo->duration;
#else
	UNUSED_PARAM(pstMediaInfo);
#endif

	PrintEnter;

	s_NetworkError = FALSE;

	if ( nTotalSize < PREBUFFERING_ONOFF_THRESHOLD )
	{
		PrintDebug ("%s(%d) Info> This content has lower size than %d Mbytes. So we should switchover the buffering mode to RealTime!!\n"
		,__FUNCTION__
		,__LINE__
		,PREBUFFERING_ONOFF_THRESHOLD);
		return DI_ERR_ERROR;
	}

#if 0
	nBufSize = STREAM_BUFFERING_SECS * (HUINT32)(nTotalSize/(nDuration/1000));
#else
	nBufSize = PREBUFFERING_BUFFER_SIZE;
#endif

	nRet = VK_SEM_Create(&s_ulPreBufferingLock,"httpPreBufferingSema", DEFAULT_SUSPEND_TYPE);
	if( nRet != VK_OK )
	{
		PrintError("%s(%d) Error(0x%x) \n\r",__FUNCTION__,__LINE__, nRet);
	}

#if 0
	/* 1.5Mbyte Under .... */
	if(nBufSize < (HUINT32)(1.5*1024*1024))
	{
		PrintError ("%s (%d) Error> Buffering Size is Too Small!!!. We are using default buf size 10MB ========> (%d)bytes \n",__FUNCTION__,__LINE__, nBufSize);
		nBufSize = 10*1024*1024;
		//return DI_ERR_ERROR;
	}
	/* 20 Mbyte Under .... */
	else if (nBufSize > (HUINT32)20*1024*1024)
	{
		PrintError ("%s (%d) Error> Buffering Size is Too BIG!!!. We are using default buf size 10MB ==========> (%d)bytes \n",__FUNCTION__,__LINE__, nBufSize);
		nBufSize = 10*1024*1024;
		//return DI_ERR_ERROR;
	}
#endif

	g_HttpBuffer =(HUINT8 *)DD_MEM_Alloc ((size_t)nBufSize);
	if(g_HttpBuffer == NULL)
	{
		PrintError ("%s (%d) Error> BUFFER Malloc Fail. Not enough Memory !!!(%d)\n",__FUNCTION__,__LINE__, nBufSize);
		return DI_ERR_ERROR;
	}
	/* else { continue; } */

	/** Assign Pointer & Thresholds value*/
	g_HTTP_BUFFERING_SIZE = nBufSize;
	g_httpptrW = g_HttpBuffer;
	g_httpptrR = g_HttpBuffer;
	HTTP_BUF_START = (HINT32)g_HttpBuffer;
	HTTP_BUF_END = (HINT32) g_HttpBuffer + g_HTTP_BUFFERING_SIZE - 1 ;
	LOWEST_HTTP_BUFFERING_THRESHOLD = g_HTTP_BUFFERING_SIZE * 5 / 100;
	RESUME_HTTP_BUFFERING_THRESHOLD = g_HTTP_BUFFERING_SIZE * 50 / 100;
	HIGHEST_HTTP_BUFFERING_THRESHOLD = g_HTTP_BUFFERING_SIZE * 90 / 100;
	sOffsetStart = 0;
	sOffsetEnd = 0;

	PrintData ("--------------------------------------------------\n");
	PrintData ("STREAM_BUFFERING_SECS            %d secs\n",STREAM_BUFFERING_SECS);
	PrintData ("STREAMING_BUFFERING              %d bytes\n",g_HTTP_BUFFERING_SIZE);
	PrintData ("g_httpptrW                       %p\n",g_httpptrW);
	PrintData ("g_httpptrR                       %p\n",g_httpptrR);
	PrintData ("HTTP_BUF_START                   0x%x\n",HTTP_BUF_START);
	PrintData ("HTTP_BUF_END                     0x%x\n",HTTP_BUF_END);
	PrintData ("LOWEST_HTTP_BUFFERING_THRESHOLD  %d bytes\n",LOWEST_HTTP_BUFFERING_THRESHOLD);
	PrintData ("HIGHEST_HTTP_BUFFERING_THRESHOLD %d bytes\n",HIGHEST_HTTP_BUFFERING_THRESHOLD);
	PrintData ("--------------------------------------------------\n");

	PrintExit;


	return nRet;
}

DI_ERR_CODE DI_STREAM_HTTP_FreeBuffer (void)
{
	DI_ERR_CODE nRet= DI_ERR_OK;
	HUINT32 nWaitingTime = 0;

	if (g_HttpBuffer == NULL ) /* in the case of realtime streaming */
	{
		return nRet;
	}

	s_eChangePreBufferingTaskState = DI_IPTUNER_HTTP_PREBUFFERING_TASK_STATE_KILLING;
	while(s_ePreBufferingTaskState != DI_IPTUNER_HTTP_PREBUFFERING_TASK_STATE_DONE)
	{
		stHttpAccess.info.b_prebuffered = TRUE; /* break out WriteBuffer Waiting State */
		PrintError("Wait For Killing Prebuffering Task\n");
		if(nWaitingTime == 1000)
		{
			s_eChangePreBufferingTaskState = DI_IPTUNER_HTTP_PREBUFFERING_TASK_STATE_KILLING;
			nWaitingTime = 0;
		}
		else
		{
			VK_TASK_Sleep(100);
			nWaitingTime=nWaitingTime+100;
		}
	}

	PrintDebug("DI_IPTUNER_HTTP_PREBUFFERING_TASK_STATE_DONE\n");

	(void)VK_SEM_Destroy(s_ulPreBufferingLock);


	/* In multi-open case, the http session should be closed only once. */
	if(stHttpAccess.b_alive == FALSE)
	{
		PrintDebug ("%s(%d) : HTTP Session is already closed.\n",__FUNCTION__,__LINE__);
		goto success1;
	}

	(void) STREAM_HTTP_Close ( &stHttpAccess);

	stHttpAccess.b_alive = FALSE;

	if(stHttpAccess.psz_path != NULL)
	{
		DD_MEM_Free(stHttpAccess.psz_path);
		stHttpAccess.psz_path = NULL;
	}
	/* else { continue; } */

success1:




	if(stHttpAccessForIndexer.b_alive == FALSE)
	{
		PrintDebug ("%s(%d) : HTTP Session is already closed.\n",__FUNCTION__,__LINE__);
		goto success2;
	}

	(void) STREAM_HTTP_Close ( &stHttpAccessForIndexer);

	stHttpAccessForIndexer.b_alive = FALSE;

	if(stHttpAccessForIndexer.psz_path != NULL)
	{
		DD_MEM_Free(stHttpAccessForIndexer.psz_path);
		stHttpAccessForIndexer.psz_path = NULL;
	}

success2:

	(void)VK_SEM_Destroy(s_ulHttpSemId);


	if(g_HttpBuffer != NULL)
	{
		DD_MEM_Free (g_HttpBuffer);
		g_HttpBuffer = NULL;
	}

	return nRet;
}
#endif /* STREAM_BUFFERING */

#if REAL_TIME_MULTISOCKET_SUPPORT
int DI_STREAM_HTTP_SetDrmMode (HUINT8 bIsDRM)
{
	HINT32 nRet = DI_IPT_SUCCESS;

	PrintEnter;

	s_bDrmMode = bIsDRM;
	PrintDebug("[%s] s_bDrmMode=%d\n", __FUNCTION__, s_bDrmMode);

	PrintExit;
	return nRet;
}

#if REAL_TIME_MULTISOCKET_SUPPORT
int DI_STREAM_HTTP_SetContentFormat (DI_MEDIA_STREAM_TYPE_e eStream_type)
{
	HINT32 nRet = DI_IPT_SUCCESS;

	PrintEnter;

	s_eStream_type = eStream_type;
	PrintDebug("[%s] s_eStream_type=%d\n", __FUNCTION__, s_eStream_type);

	PrintExit;
	return nRet;
}
#endif

#if REAL_TIME_CHUNKBUFFER_SUPPORT
static int P_STREAM_HTTP_UpdateRealTimeBuffer(void *ptr, HUINT32 nRead_Length, HUINT64 ulCurrent_Index)
{
	int nRet = 0;
	int nCount = 0;
	PrintEnter;

	if((NULL == ptr) || (CHUNK_BUUFER_DEFAULT_SIZE < nRead_Length))
	{
		PrintError("######### [%s] Critical Section: Invalid Address = ptr #######\n", __FUNCTION__);
		return VFIO_ERROR_FAIL;
	}

//	PrintDebug("tRealtime_Buffer_Chunk[0].ulStartindex = %ld, nRead_Length=%u, ulCurrent_Index=%ld \n\n",
//		tRealtime_Buffer_Chunk[0].ulStartindex, nRead_Length, ulCurrent_Index );

	for(nCount = (REALTIME_PLAYBUFFER_CHUNK_TOTAL-2); nCount >= 0; nCount--)
	{
//		PrintDebug("tRealtime_Buffer_Chunk[%d].ulStartindex = %ld, tRealtime_Buffer_Chunk[%d].ulStartindex=%ld \n\n", nCount+1,
//			tRealtime_Buffer_Chunk[nCount+1].ulStartindex, nCount, tRealtime_Buffer_Chunk[nCount].ulStartindex );
		if(tRealtime_Buffer_Chunk[nCount].ulStartindex != 0)
		{
			VK_MEM_Memcpy(tRealtime_Buffer_Chunk[nCount+1].pcData, tRealtime_Buffer_Chunk[nCount].pcData, tRealtime_Buffer_Chunk[nCount].unData_Length);
			tRealtime_Buffer_Chunk[nCount+1].ulStartindex = tRealtime_Buffer_Chunk[nCount].ulStartindex;
			tRealtime_Buffer_Chunk[nCount+1].unData_Length = tRealtime_Buffer_Chunk[nCount].unData_Length;
		}
	}

	VK_MEM_Memcpy(tRealtime_Buffer_Chunk[0].pcData, ptr, nRead_Length);
	tRealtime_Buffer_Chunk[0].ulStartindex = ulCurrent_Index;
	tRealtime_Buffer_Chunk[0].unData_Length = nRead_Length;

	PrintExit;
	return nRet;
}
#endif

#if REAL_TIME_CHUNKBUFFER_SUPPORT || REAL_TIME_INDEXBUFFER_SUPPORT
/** */
int P_STREAM_HTTP_ReadforRealtimeBuffer (VFIO_Handle_t tHandle, void *ptr, size_t size, size_t nmemb, size_t *ptReadSize)
{
	size_t sz_Real=0;
	size_t sz_Try = size*nmemb;
	HINT32 n_ReturnedValue = 0;
	int ret = VFIO_ERROR_SUCCESS;
	HINT32 	nRealtime_buffercount =0;
	HINT64 	Cureent_Index =0;
	STREAM_ACCESS_t *ptHttpAccess=NULL;
	HINT32 	error_retrycount =0;
	HINT32 	error_Timeoutcount =0;
	HINT32 	ReadFail_retrycount =0;

	UNUSED_PARAM (tHandle);

	PrintEnter;

	if(tHandle.pDnpUserData != NULL)
	{
		ptHttpAccess = (STREAM_ACCESS_t *)tHandle.pDnpUserData;
	}
	else
	{
		PrintError ("%s(%d) Error> tHandle->pDnpUserData is null \n",__FUNCTION__,__LINE__);
		PrintExit;
		return VFIO_ERROR_IN_DEVICE;
	}

	if((unsigned int)s_tMultiHandle.PlayerHandle == tHandle.tFile.callback.unHandle)
	{
		if(ptHttpAccess->info.i_size < ptHttpAccess->info.i_pos + sz_Try)
		{
			sz_Try = ptHttpAccess->info.i_size - ptHttpAccess->info.i_pos;
		}

		Cureent_Index = ptHttpAccess->info.i_pos;

#if REAL_TIME_CHUNKBUFFER_SUPPORT
		if(ptHttpAccess->info.i_pos != s_ullCurrent_PlayCurPosition)
		{
			for(nRealtime_buffercount = 0; nRealtime_buffercount<REALTIME_PLAYBUFFER_CHUNK_TOTAL; nRealtime_buffercount++)
			{
				if((s_ullCurrent_PlayCurPosition == tRealtime_Buffer_Chunk[nRealtime_buffercount].ulStartindex) && (sz_Try <= tRealtime_Buffer_Chunk[nRealtime_buffercount].unData_Length))
				{
					VK_MEM_Memcpy(ptr, tRealtime_Buffer_Chunk[nRealtime_buffercount].pcData, sz_Try);
					s_ullCurrent_PlayCurPosition += sz_Try;
					*ptReadSize = sz_Try;

//					PrintDebug("##### tRealtime_Buffer_Chunk[nRealtime_buffercount].ulStartindex=%ld #####\n", tRealtime_Buffer_Chunk[nRealtime_buffercount].ulStartindex);
					return VFIO_ERROR_SUCCESS;
				}
			}

			if(nRealtime_buffercount == REALTIME_PLAYBUFFER_CHUNK_TOTAL)
			{
				PrintError("######### [%s] Critical Section: Fail Read #######\n", __FUNCTION__);
				PrintError("###########s_ullCurrent_PlayCurPosition=%"PRId64", sz_Try=%d##########################\n", s_ullCurrent_PlayCurPosition, sz_Try);

				if(STREAM_HTTP_Seek (ptHttpAccess, s_ullCurrent_PlayCurPosition))
				{
					PrintError ("%s(%d) Error> STREAM_HTTP_Seek fail \n",__FUNCTION__,__LINE__);
			//		PrintExit;
					return 0;
				}
			}
		}
#endif

		PrintDebug("Read for Player(%"PRId64")\n\n",ptHttpAccess->info.i_pos );

		do
		{
			n_ReturnedValue = STREAM_HTTP_Read ( ptHttpAccess, (HUINT8 *)ptr+sz_Real, sz_Try - sz_Real );
			if((error_retrycount < 10) &&(n_ReturnedValue == 0) && (ptHttpAccess->info.i_httpstatecode == HTTP_STATUS_MOVED_PERMANENTLY	/* 301 */
					|| ptHttpAccess->info.i_httpstatecode == HTTP_STATUS_FOUND 				/* 302 */
					|| ptHttpAccess->info.i_httpstatecode == HTTP_STATUS_SEE_OTHER 			/* 303 */
					|| ptHttpAccess->info.i_httpstatecode == HTTP_STATUS_TEMP_REDIRECT))		/* 307 */
			{
				error_retrycount++;
				continue;
			}

			if(n_ReturnedValue == DI_IPT_ETIMEOUT)
			{
				if(error_Timeoutcount != 2)
				{
					error_Timeoutcount++;
					continue;
				}
				else
				{
					error_Timeoutcount = 0;
				}
			}

			if(n_ReturnedValue < 0)
			{
				if(sz_Real > 0)
				{
					s_ullCurrent_PlayCurPosition += sz_Real;
					*ptReadSize = sz_Real;
				}
				else
				{
					*ptReadSize = n_ReturnedValue;
				}

				goto error;
			}
			else
			{
				sz_Real += n_ReturnedValue;
			}

			if((ReadFail_retrycount == 100)&&(sz_Real == 0))
			{
				PrintError("##### %s(%d) Retry Fail(Read Size = 0)(RetryCount=%d) #####\n\n", __FUNCTION__, __LINE__, ReadFail_retrycount);
				*ptReadSize = sz_Real;
				ReadFail_retrycount = 0;
				goto error;
			}

			if((ReadFail_retrycount == 100)&&(sz_Real > 0))
			{
				PrintError("##### %s(%d) Retry Fail(Read Size != 0)(RetryCount=%d) #####\n\n", __FUNCTION__, __LINE__, ReadFail_retrycount);
				s_ullCurrent_PlayCurPosition += sz_Real;
				*ptReadSize = sz_Real;
				ReadFail_retrycount = 0;
				goto error;
			}

			ReadFail_retrycount++;
		}
		while (sz_Real < sz_Try);
#if REAL_TIME_CHUNKBUFFER_SUPPORT
		if(sz_Real == sz_Try)
		{
			(void)P_STREAM_HTTP_UpdateRealTimeBuffer(ptr, sz_Real, Cureent_Index);
		}
#endif
		s_ullCurrent_PlayCurPosition += sz_Real;

		*ptReadSize = sz_Real;
	}
	else if((unsigned int)s_tMultiHandle.IndexHandle == tHandle.tFile.callback.unHandle)
	{
		if(ptHttpAccess->info.i_size < ptHttpAccess->info.i_pos + sz_Try)
		{
			sz_Try = ptHttpAccess->info.i_size - ptHttpAccess->info.i_pos;
		}

		PrintDebug("Read for Indexer(%"PRId64")\n\n",ptHttpAccess->info.i_pos );

		do
		{
			n_ReturnedValue = STREAM_HTTP_Read ( ptHttpAccess, (HUINT8 *)ptr+sz_Real, sz_Try - sz_Real );
			if((error_retrycount < 10) &&(n_ReturnedValue == 0) && (ptHttpAccess->info.i_httpstatecode == HTTP_STATUS_MOVED_PERMANENTLY	/* 301 */
					|| ptHttpAccess->info.i_httpstatecode == HTTP_STATUS_FOUND 				/* 302 */
					|| ptHttpAccess->info.i_httpstatecode == HTTP_STATUS_SEE_OTHER 			/* 303 */
					|| ptHttpAccess->info.i_httpstatecode == HTTP_STATUS_TEMP_REDIRECT))		/* 307 */
			{
				error_retrycount++;
				continue;
			}

			if(n_ReturnedValue == DI_IPT_ETIMEOUT)
			{
				if(error_Timeoutcount != 2)
				{
					error_Timeoutcount++;
					continue;
				}
				else
				{
					error_Timeoutcount = 0;
				}
			}

			if(n_ReturnedValue < 0)
			{
				if(sz_Real > 0)
				{
					s_ullCurrent_IndexCurPosition += sz_Real;
					*ptReadSize = sz_Real;
				}
				else
				{
					*ptReadSize = n_ReturnedValue;
				}

				goto error;
			}
			else
			{
				sz_Real += n_ReturnedValue;
			}

			if((ReadFail_retrycount == 100)&&(sz_Real == 0))
			{
				PrintError("##### %s(%d) Retry Fail(Read Size = 0)(RetryCount=%d) #####\n\n", __FUNCTION__, __LINE__, ReadFail_retrycount);
				*ptReadSize = sz_Real;
				ReadFail_retrycount = 0;
				goto error;
			}

			if((ReadFail_retrycount == 100)&&(sz_Real > 0))
			{
				PrintError("##### %s(%d) Retry Fail(Read Size != 0)(RetryCount=%d) #####\n\n", __FUNCTION__, __LINE__, ReadFail_retrycount);
				s_ullCurrent_IndexCurPosition += sz_Real;
				*ptReadSize = sz_Real;
				ReadFail_retrycount = 0;
				goto error;
			}

			ReadFail_retrycount++;
		}
		while (sz_Real < sz_Try);

		s_ullCurrent_IndexCurPosition += sz_Real;

		*ptReadSize = sz_Real;
	}
	else
	{
		if(ptHttpAccess->info.i_size < ptHttpAccess->info.i_pos + sz_Try)
		{
			sz_Try = ptHttpAccess->info.i_size - ptHttpAccess->info.i_pos;
		}

		PrintDebug("Current Read Position(%"PRId64") sz_Try=%u\n\n",ptHttpAccess->info.i_pos,sz_Try );

		do
		{
			n_ReturnedValue = STREAM_HTTP_Read ( ptHttpAccess, (HUINT8 *)ptr+sz_Real, sz_Try - sz_Real );
			if((error_retrycount < 10) &&(n_ReturnedValue == 0) && (ptHttpAccess->info.i_httpstatecode == HTTP_STATUS_MOVED_PERMANENTLY	/* 301 */
					|| ptHttpAccess->info.i_httpstatecode == HTTP_STATUS_FOUND 				/* 302 */
					|| ptHttpAccess->info.i_httpstatecode == HTTP_STATUS_SEE_OTHER 			/* 303 */
					|| ptHttpAccess->info.i_httpstatecode == HTTP_STATUS_TEMP_REDIRECT))		/* 307 */
			{
				error_retrycount++;
				continue;
			}
			if((error_retrycount < 10) &&(n_ReturnedValue == 0) && (stHttpAccess.info.i_httpstatecode == HTTP_STATUS_MOVED_PERMANENTLY	/* 301 */
					|| stHttpAccess.info.i_httpstatecode == HTTP_STATUS_FOUND 				/* 302 */
					|| stHttpAccess.info.i_httpstatecode == HTTP_STATUS_SEE_OTHER 			/* 303 */
					|| stHttpAccess.info.i_httpstatecode == HTTP_STATUS_TEMP_REDIRECT))		/* 307 */
			{
				error_retrycount++;
				continue;
			}

			if(n_ReturnedValue == DI_IPT_ETIMEOUT)
			{
				if(error_Timeoutcount != 2)
				{
					error_Timeoutcount++;
					continue;
				}
				else
				{
					error_Timeoutcount = 0;
				}
			}

			if(n_ReturnedValue < 0)
			{
				if(sz_Real > 0)
				{
					*ptReadSize = sz_Real;
				}
				else
				{
					*ptReadSize = n_ReturnedValue;
				}

				goto error;
			}
			else
			{
				sz_Real += n_ReturnedValue;
			}

			if((ReadFail_retrycount == 100)&&(sz_Real == 0))
			{
				PrintError("##### %s(%d) Retry Fail(Read Size = 0)(RetryCount=%d) #####\n\n", __FUNCTION__, __LINE__, ReadFail_retrycount);
				*ptReadSize = sz_Real;
				ReadFail_retrycount = 0;
				goto error;
			}

			if((ReadFail_retrycount == 100)&&(sz_Real > 0))
			{
				PrintError("##### %s(%d) Retry Fail(Read Size != 0)(RetryCount=%d) #####\n\n", __FUNCTION__, __LINE__, ReadFail_retrycount);
				*ptReadSize = sz_Real;
				ReadFail_retrycount = 0;
				goto error;
			}

			ReadFail_retrycount++;
		}
		while (sz_Real < sz_Try);

		*ptReadSize = sz_Real;
	}

error:

	PrintDebug("%s(%d) : handle (0x%x) tryRead(%d) realRead(%u, %u)\n",__FUNCTION__,__LINE__, tHandle.tFile.callback.unHandle,sz_Try, sz_Real, *ptReadSize);

	return ret;
}

/* Seek From Nexus : Long jump 인 경우 어느정도는 block 되어도 된다.
 * 단 Read 함수에서는 block 되면 waiting IO error, 0 return 시 endofStream, -1 return시 errorofstream 이 뜬다.*/
off_t P_STREAM_HTTP_SeekforRealtimeBuffer (VFIO_Handle_t tHandle, off_t offset, int whence)
{
	HINT32 nRet;
	HINT32 tReadSize=0;
#if REAL_TIME_CHUNKBUFFER_SUPPORT || REAL_TIME_INDEXBUFFER_SUPPORT
	HUINT32 nCurrentReadCount = 0;
	HUINT32 nMaxReadCount = 0;
	HUINT32 nTempReadSize = 0;
	HINT64 lBlockSize = 0;
	HINT8 *pcTempBuffer=NULL;
	HUINT32 nIndex=0;
	HUINT8 bUseChunkBuffer=FALSE;
#endif
	STREAM_ACCESS_t *ptHttpAccess=NULL;

//	UNUSED_PARAM (tHandle);
	UNUSED_PARAM (whence);

	PrintEnter;

	if(tHandle.pDnpUserData != NULL)
	{
		ptHttpAccess = (STREAM_ACCESS_t *)tHandle.pDnpUserData;
	}
	else
	{
		PrintError ("%s(%d) Error> tHandle->pDnpUserData is null \n",__FUNCTION__,__LINE__);
		PrintExit;
		return VFIO_ERROR_IN_DEVICE;
	}

	if((unsigned int)s_tMultiHandle.PlayerHandle == tHandle.tFile.callback.unHandle)
	{
		if(ptHttpAccess->info.i_pos == offset)
		{
			s_ullCurrent_PlayCurPosition = offset;
			PrintExit;
			return offset;
		}
#if REAL_TIME_CHUNKBUFFER_SUPPORT
		if(s_tMP4MoovInstance.pcData != NULL)
		{
			bUseChunkBuffer = FALSE;

			for(nIndex = 0; nIndex < REALTIME_PLAYBUFFER_CHUNK_TOTAL; nIndex++)
			{
				if(offset == tRealtime_Buffer_Chunk[nIndex].ulStartindex)
				{
					bUseChunkBuffer = TRUE;
					break;
				}
			}

			if((offset != 0) && (bUseChunkBuffer == TRUE))
			{
				PrintDebug("Not Move(Because Back Buffer)\n");
				PrintDebug("#####offset=%"PRId64", stHttpAccess.info.i_pos=%"PRId64"#####\n", offset,ptHttpAccess->info.i_pos );
				s_ullCurrent_PlayCurPosition = offset;
				PrintExit;
				return offset;
			}

			if(((ptHttpAccess->info.i_pos+BROADCOM_PLAYBACK_READ_BLOCK*30) > offset) && (ptHttpAccess->info.i_pos < offset))
			{
				PrintData ("%s(%d) : stHttpAccess.info.i_pos=%"PRId64", offset=%"PRId64", test=%"PRId64"\n",__FUNCTION__,__LINE__, ptHttpAccess->info.i_pos, offset, (offset-ptHttpAccess->info.i_pos));
				lBlockSize = BROADCOM_PLAYBACK_READ_BLOCK;
				s_ullCurrent_PlayCurPosition = ptHttpAccess->info.i_pos;
				nMaxReadCount = (offset-ptHttpAccess->info.i_pos)/lBlockSize;
				nTempReadSize = (offset-ptHttpAccess->info.i_pos-lBlockSize*nMaxReadCount);
				if(nTempReadSize > 0)
				{
					nMaxReadCount += 1;
				}

				if(nTempReadSize == 0)
				{
					nTempReadSize = BROADCOM_PLAYBACK_READ_BLOCK;
				}

				PrintData ("%s(%d) : nMaxReadCount=%u, lBlockSize=%"PRId64", nTempReadSize=%u\n",
					__FUNCTION__,__LINE__, nMaxReadCount, lBlockSize, nTempReadSize);
			}

			if(nMaxReadCount > 0)
			{
				for(nCurrentReadCount = 0; nCurrentReadCount < nMaxReadCount; nCurrentReadCount++)
				{
					pcTempBuffer = DD_MEM_Alloc(CHUNK_BUUFER_DEFAULT_SIZE);
					if(pcTempBuffer == NULL)
					{
						PrintError("%s(%d) : Error> Fail Mem alloc.\n",__FUNCTION__,__LINE__);
						PrintExit;
						return -1;
					}
					else
					{
						VK_MEM_Memset(pcTempBuffer, 0x0, CHUNK_BUUFER_DEFAULT_SIZE);
					}

					(void)P_STREAM_HTTP_ReadforRealtimeBuffer(tHandle, pcTempBuffer, BROADCOM_PLAYBACK_READ_BLOCK, 1, (size_t *)&tReadSize);
					if(BROADCOM_PLAYBACK_READ_BLOCK != tReadSize)
					{
						PrintDebug("######### [%s] Critical Section: Fail Read #######\n", __FUNCTION__);
					}

					if(pcTempBuffer != NULL)
					{
						DD_MEM_Free(pcTempBuffer);
						pcTempBuffer = NULL;
					}
				}

				PrintExit;
				return offset;
			}
		}
#endif

		PrintDebug("Player seek\n");
		nRet = STREAM_HTTP_Seek (ptHttpAccess, offset );
		if(nRet != DI_IPT_SUCCESS)
		{
			PrintError ("%s(%d) Error> STREAM_HTTP_Seek fail \n",__FUNCTION__,__LINE__);
	//		PrintExit;
			return -1;
		}
		else
		{
			s_ullCurrent_PlayCurPosition = offset;
		}
	}
	else if((unsigned int)s_tMultiHandle.IndexHandle == tHandle.tFile.callback.unHandle)
	{
		if(ptHttpAccess->info.i_pos == offset)
		{
			return offset;
		}

		PrintDebug("Index seek\n");
		nRet = STREAM_HTTP_Seek (ptHttpAccess, offset );
		if(nRet != DI_IPT_SUCCESS)
		{
			PrintError ("%s(%d) Error> STREAM_HTTP_Seek fail \n",__FUNCTION__,__LINE__);
	//		PrintExit;
			return -1;
		}
	}

	PrintExit;
	return offset;
}
#endif
#endif

/** */
int DI_STREAM_HTTP_Close (VFIO_Handle_t tHandle)
{
	HINT32 vRet = VFIO_ERROR_SUCCESS;
#if REAL_TIME_MULTISOCKET_SUPPORT
	STREAM_ACCESS_t *ptHttpAccess=NULL;
#if REAL_TIME_INDEXBUFFER_SUPPORT || REAL_TIME_CHUNKBUFFER_SUPPORT
	HUINT32 nIndex=0;
#endif
#endif

	UNUSED_PARAM (tHandle);
	PrintEnter;

#if STREAM_BUFFERING
	if (g_HttpBuffer == NULL ) /* in the case of realtime streaming */
	{
#endif

#if REAL_TIME_MULTISOCKET_SUPPORT
	(void)VK_SEM_Get(s_ulHttpSemId);

	if(tHandle.pDnpUserData != NULL)
	{
		ptHttpAccess = (STREAM_ACCESS_t *)tHandle.pDnpUserData;
	}
	else
	{
		PrintError ("%s(%d) Error> tHandle->pDnpUserData is null \n",__FUNCTION__,__LINE__);
		PrintExit;
		(void)VK_SEM_Release (s_ulHttpSemId);
		return VFIO_ERROR_IN_DEVICE;
	}

	if((unsigned int)s_tMultiHandle.DrmHandle == tHandle.tFile.callback.unHandle)
	{
		PrintDebug ("%s(%d) : close Drm handle.\n",__FUNCTION__,__LINE__);
		if(ptHttpAccess->b_alive != FALSE)
		{
			(void) STREAM_HTTP_Close (ptHttpAccess);

			ptHttpAccess->b_alive = FALSE;

			if(ptHttpAccess->psz_path != NULL)
			{
				DD_MEM_Free(ptHttpAccess->psz_path);
				ptHttpAccess->psz_path =NULL;
			}
			/* else { continue; } */
#if SUPPORT_COOKIE
			if(ptHttpAccess->psz_cookie != NULL)
			{
				DD_MEM_Free(ptHttpAccess->psz_cookie);
				ptHttpAccess->psz_cookie =NULL;
			}
			/* else { continue; } */
#endif

			if(ptHttpAccess != NULL)
			{
				DD_MEM_Free(ptHttpAccess);
				ptHttpAccess =NULL;
			}
			/* else { continue; } */
		}
		else
		{
			PrintDebug ("%s(%d) : HTTP Session is already closed.\n",__FUNCTION__,__LINE__);
		}

		s_tMultiHandle.DrmHandle = 0;
		s_bDrmMode = FALSE;
		(void)VK_SEM_Release (s_ulHttpSemId);
	}
	else if((unsigned int)s_tMultiHandle.PlayerHandle == tHandle.tFile.callback.unHandle)
	{
		PrintDebug ("%s(%d) : close s_PlayerHandle.\n",__FUNCTION__,__LINE__);

		if(ptHttpAccess->b_alive != FALSE)
		{
			(void) STREAM_HTTP_Close (ptHttpAccess);

			ptHttpAccess->b_alive = FALSE;

			if(ptHttpAccess->psz_path != NULL)
			{
				DD_MEM_Free(ptHttpAccess->psz_path);
				ptHttpAccess->psz_path =NULL;
			}
			/* else { continue; } */
#if SUPPORT_COOKIE
			if(ptHttpAccess->psz_cookie != NULL)
			{
				DD_MEM_Free(ptHttpAccess->psz_cookie);
				ptHttpAccess->psz_cookie =NULL;
			}
			/* else { continue; } */
#endif

			if(ptHttpAccess != NULL)
			{
				DD_MEM_Free(ptHttpAccess);
				ptHttpAccess =NULL;
			}
			/* else { continue; } */
		}
		else
		{
			PrintDebug ("%s(%d) : HTTP Session is already closed.\n",__FUNCTION__,__LINE__);
		}

#if REAL_TIME_CHUNKBUFFER_SUPPORT
		if(s_eStream_type == DI_MEDIA_MPEG_TYPE_ASF)
		{
			s_tWMVInstance.lBuffer_CursorPosition = 0;
			s_tWMVInstance.lBuffer_ReadPosition = 0;
			if(s_tWMVInstance.pcData != NULL)
			{
				DD_MEM_Free(s_tWMVInstance.pcData);
				s_tWMVInstance.pcData = NULL;
			}
			VK_MEM_Memset(&s_tWMVInstance, 0x0, sizeof(s_tWMVInstance));
		}

		for(nIndex=0;nIndex<REALTIME_PLAYBUFFER_CHUNK_TOTAL;nIndex++)
		{
			if(tRealtime_Buffer_Chunk[nIndex].pcData != NULL)
			{
				DD_MEM_Free(tRealtime_Buffer_Chunk[nIndex].pcData);
				tRealtime_Buffer_Chunk[nIndex].pcData= NULL;
			}
		}
#endif

		s_eStream_type = DI_MEDIA_MPEG_TYPE_UNKNOWN;
		s_tMultiHandle.PlayerHandle = 0;

		(void)VK_SEM_Release (s_ulHttpSemId);
	}
	else if((unsigned int)s_tMultiHandle.IndexHandle == tHandle.tFile.callback.unHandle)
	{
		PrintDebug ("%s(%d) : close s_IndexHandle.\n",__FUNCTION__,__LINE__);
		if(ptHttpAccess->b_alive != FALSE)
		{
			(void) STREAM_HTTP_Close (ptHttpAccess);

			ptHttpAccess->b_alive = FALSE;

			if(ptHttpAccess->psz_path != NULL)
			{
				DD_MEM_Free(ptHttpAccess->psz_path);
				ptHttpAccess->psz_path =NULL;
			}
			/* else { continue; } */
#if SUPPORT_COOKIE
			if(ptHttpAccess->psz_cookie != NULL)
			{
				DD_MEM_Free(ptHttpAccess->psz_cookie);
				ptHttpAccess->psz_cookie =NULL;
			}
			/* else { continue; } */
#endif

			if(ptHttpAccess != NULL)
			{
				DD_MEM_Free(ptHttpAccess);
				ptHttpAccess =NULL;
			}
			/* else { continue; } */
		}
		else
		{
			PrintDebug ("%s(%d) : HTTP Session is already closed.\n",__FUNCTION__,__LINE__);
		}

		s_tMultiHandle.IndexHandle = 0;
#if defined (USE_MOOVBOX_CACHING)
		if(pcRedirectionUrl != NULL)
		{
			DD_MEM_Free(pcRedirectionUrl);
			pcRedirectionUrl = NULL;
		}

		if(stMoovBox.eState == MOOVBOX_CACHING_STATE_CACHED)
		{
			if(stMoovBox.pucMoov != NULL)
			{
				DD_MEM_Free(stMoovBox.pucMoov);
				stMoovBox.pucMoov = NULL;
			}
			VK_MEM_Memset(&stMoovBox,0x00,sizeof(MOOVBOX_INSTANCE_t));
		}
		stMoovBox.eState = MOOVBOX_CACHING_STATE_NONE;
#endif
		(void)VK_SEM_Release (s_ulHttpSemId);
	}
	else
	{
		PrintDebug ("%s(%d) : close probe handle.\n",__FUNCTION__,__LINE__);
		if(ptHttpAccess->b_alive != FALSE)
		{
			(void) STREAM_HTTP_Close (ptHttpAccess);

			ptHttpAccess->b_alive = FALSE;

			if(ptHttpAccess->psz_path != NULL)
			{
				DD_MEM_Free(ptHttpAccess->psz_path);
				ptHttpAccess->psz_path =NULL;
			}
			/* else { continue; } */
#if SUPPORT_COOKIE
			if(ptHttpAccess->psz_cookie != NULL)
			{
				DD_MEM_Free(ptHttpAccess->psz_cookie);
				ptHttpAccess->psz_cookie =NULL;
			}
			/* else { continue; } */
#endif

			if(ptHttpAccess != NULL)
			{
				DD_MEM_Free(ptHttpAccess);
				ptHttpAccess =NULL;
			}
			/* else { continue; } */
		}
		else
		{
			PrintDebug ("%s(%d) : HTTP Session is already closed.\n",__FUNCTION__,__LINE__);
		}

		(void)VK_SEM_Release (s_ulHttpSemId);
	}

	if((s_tMultiHandle.ProbeHandle == 0) && (s_tMultiHandle.PlayerHandle == 0) && (s_tMultiHandle.IndexHandle == 0) && (s_tMultiHandle.DrmHandle == 0))
	{
#if REAL_TIME_INDEXBUFFER_SUPPORT
		if(s_tMP4MoovInstance.pcData != NULL)
		{
			DD_MEM_Free(s_tMP4MoovInstance.pcData);
			s_tMP4MoovInstance.pcData = NULL;
			s_tMP4MoovInstance.lBuffer_CursorPosition = 0;
			s_tMP4MoovInstance.lBuffer_ReadPosition = 0;
		}
		else
		{
			for(nIndex=0;nIndex<REALTIME_PROBEBUFFER_CHUNK_TOTAL;nIndex++)
			{
				if(tRealtime_ProbeBuffer_Chunk[nIndex].pcData != NULL)
				{
					PrintDebug("##### [%s] MemFree(%d) #####\n", __FUNCTION__, nIndex);

					DD_MEM_Free(tRealtime_ProbeBuffer_Chunk[nIndex].pcData);
					s_nProbeBuffer_TotalUsedMemory -= tRealtime_ProbeBuffer_Chunk[nIndex].unData_Length+1;
					tRealtime_ProbeBuffer_Chunk[nIndex].pcData = NULL;
					tRealtime_ProbeBuffer_Chunk[nIndex].unData_Length = 0;
				}
			}
		}
#endif

		PrintDebug ("%s(%d) : close NEXUS_OPEN sem \n",__FUNCTION__,__LINE__);
	 	(void)VK_SEM_Destroy(s_ulHttpSemId);
	 	s_ulHttpSemId = 0;
	}
	else if(s_tMultiHandle.ProbeHandle != 0)
	{
		PrintDebug ("%s(%d) : close PROBE_OPEN sem \n",__FUNCTION__,__LINE__);
	 	(void)VK_SEM_Destroy(s_ulHttpSemId);
	 	s_ulHttpSemId = 0;
		s_tMultiHandle.ProbeHandle = 0;
	}
#else
	/* In multi-open case, the http session should be closed only once. */
	if(stHttpAccess.b_alive == FALSE)
	{
		PrintDebug ("%s(%d) : HTTP Session is already closed.\n",__FUNCTION__,__LINE__);
		goto success;
	}

	(void)VK_SEM_Get(s_ulHttpSemId);

	(void) STREAM_HTTP_Close ( &stHttpAccess);

	stHttpAccess.b_alive = FALSE;

	if(stHttpAccess.psz_path != NULL)
	{
		DD_MEM_Free(stHttpAccess.psz_path);
		stHttpAccess.psz_path =NULL;
	}
	/* else { continue; } */

#if SUPPORT_COOKIE
	if(stHttpAccess.psz_cookie != NULL)
	{
		DD_MEM_Free(stHttpAccess.psz_cookie);
		stHttpAccess.psz_cookie =NULL;
	}
	/* else { continue; } */
#endif

	(void)VK_SEM_Release (s_ulHttpSemId);

 	(void)VK_SEM_Destroy(s_ulHttpSemId);
	s_ulHttpSemId = 0;
#endif

#if STREAM_BUFFERING
	}
#endif
#if defined(USE_RAMBUFFERING_MOOVBOX_CACHING_SUPPORT) || defined (USE_MOOVBOX_CACHING)
	stMoovBox.lCursorPos = 0;
#endif

#if !REAL_TIME_MULTISOCKET_SUPPORT
success:
#endif

#if STREAM_BUFFERING
	if(tHandle.pDnpUserData)
	{
		if(((DNP_USER_DATA_t *)tHandle.pDnpUserData)->p_align_buffer)
		{
			DD_MEM_Free(((DNP_USER_DATA_t *)tHandle.pDnpUserData)->p_align_buffer);
			((DNP_USER_DATA_t *)tHandle.pDnpUserData)->p_align_buffer = NULL;
		}

		if(tHandle.pDnpUserData)
		{
			DD_MEM_Free(tHandle.pDnpUserData);
			tHandle.pDnpUserData = NULL;
		}
	}
#endif

#if	INTEGRITY_STREAM_CHECK_WITH_COMPARING_LOCAL_FILE
 	if(fd)
 	{
 		(void)close(fd);
 	}
#endif

	PrintExit;
	return vRet;
}

/** */
int DI_STREAM_HTTP_Read (VFIO_Handle_t tHandle, void *ptr, size_t size, size_t nmemb, size_t *ptReadSize)
{
	size_t sz_Real=0;
	size_t sz_Try = size*nmemb;
#if 1 /* defined(STREAM_BUFFERING) || defined(REAL_TIME_MULTISOCKET_SUPPORT) */
	/* warning: unused variable */
	HINT32 n_ReturnedValue = 0;
	HINT32 	ReadFail_retrycount =0;
	int 	error_Timeoutcount =0;
#endif
	int ret = VFIO_ERROR_SUCCESS;
#if REAL_TIME_MULTISOCKET_SUPPORT
	int 	error_SockErrorcount =0;
	HINT32 	error_retrycount =0;
	STREAM_ACCESS_t *ptHttpAccess=NULL;
#if REAL_TIME_INDEXBUFFER_SUPPORT || REAL_TIME_CHUNKBUFFER_SUPPORT
	HINT64 	Cureent_Index =0;
	HINT32 	nRealtime_buffercount =0;
	HUINT32 nIndex=0;
	HUINT32 nTempReadSize=0;
	HUINT32 nBufferReadPosition=0;
	HUINT8 bIsWMABufferRead=FALSE;
#endif
#endif

	UNUSED_PARAM (tHandle);

	/* warning: unused variable */
	n_ReturnedValue = 0;
	ReadFail_retrycount =0;
	error_Timeoutcount =0;


	PrintEnter;

	(void)VK_SEM_Get(s_ulHttpSemId);

#if REAL_TIME_MULTISOCKET_SUPPORT
	if(tHandle.pDnpUserData != NULL)
	{
		ptHttpAccess = (STREAM_ACCESS_t *)tHandle.pDnpUserData;
	}
	else
	{
		PrintError ("%s(%d) Error> tHandle->pDnpUserData is null \n",__FUNCTION__,__LINE__);
		PrintExit;
		(void)VK_SEM_Release (s_ulHttpSemId);
		return VFIO_ERROR_IN_DEVICE;
	}
#endif

#if STREAM_BUFFERING
	/**/
	/* DNP Tuner 와 Probe OPEN 인경우는 Real-time Reading */
	/**/
	if( g_HttpBuffer == NULL || gOpenStatus == PROBE_OPEN )
	{
		PrintDebug("=== Probe read\n");

#if defined(USE_RAMBUFFERING_MOOVBOX_CACHING_SUPPORT)
		if(stMoovBox.eState == MOOVBOX_CACHING_STATE_CACHED)
		{
//			PrintDebug ("Read For Probing\n");
			// TODO:  If needed  data size exceed range of buffer,  should gather potion of data from network.
			if ((stMoovBox.lCursorPos >= (HINT64)stMoovBox.ulStartOffset) &&
				(HINT64)(stMoovBox.unDataLength) >= (stMoovBox.lCursorPos - (HINT64)stMoovBox.ulStartOffset) + sz_Try )
			{
				VK_MEM_Memcpy((HUINT8*)ptr, stMoovBox.pucMoov + (stMoovBox.lCursorPos - stMoovBox.ulStartOffset), sz_Try);
				*ptReadSize = sz_Try;
				sz_Real = sz_Try;
				stMoovBox.lCursorPos += sz_Try;
				//PrintDebug ("Read Probe End\n");

				/* Removed goto success; */
				PrintDebug("%s(%d) : handle (0x%x) tryRead(%d) realRead(%d)\n",__FUNCTION__,__LINE__, tHandle.tFile.callback.unHandle,sz_Try, sz_Real);
				(void)VK_SEM_Release (s_ulHttpSemId);
				PrintExit;
				return ret;
			}
			else
			{
				if(stHttpAccess.info.i_pos != stMoovBox.lCursorPos)
				{
					(void)STREAM_HTTP_Seek (&stHttpAccess, stMoovBox.lCursorPos);
				}
				//PrintDebug ("Read For Probing Failed!!\n");
			}
		}
#endif

		if(stHttpAccess.info.i_size < stHttpAccess.info.i_pos + sz_Try)
		{
			sz_Try = stHttpAccess.info.i_size - stHttpAccess.info.i_pos;
		}

		do
		{
			sz_Real += STREAM_HTTP_Read ( &stHttpAccess, (HUINT8 *)ptr+sz_Real, sz_Try - sz_Real );
			if(stHttpAccess.info.b_eof == TRUE )
			{
				PrintDebug ("%s(%d) : Info> Cursor reached in end of the file.\n",__FUNCTION__,__LINE__);
				break;
			}
		}
		while (sz_Real < sz_Try);
#if defined(USE_RAMBUFFERING_MOOVBOX_CACHING_SUPPORT)
		if(stMoovBox.eState == MOOVBOX_CACHING_STATE_CACHED)
		{
			stMoovBox.lCursorPos += sz_Real;
		}
#endif
		*ptReadSize = sz_Real;

		/* Removed goto success; */
		PrintDebug("%s(%d) : handle (0x%x) tryRead(%d) realRead(%d)\n",__FUNCTION__,__LINE__, tHandle.tFile.callback.unHandle,sz_Try, sz_Real);
		(void)VK_SEM_Release (s_ulHttpSemId);
		PrintExit;
		return ret;
	}

	/**/
	/* Indexer Read : Real-time read from Indexer Connection */
	/**/
	if(((DNP_USER_DATA_t *)tHandle.pDnpUserData)->nPurpose == 2)
	{
		//HINT32 nRet;
		//PrintDebug ("=== Indexer read\n");
#if defined(USE_RAMBUFFERING_MOOVBOX_CACHING_SUPPORT)
		if(stMoovBox.eState == MOOVBOX_CACHING_STATE_CACHED)
		{
			//PrintDebug ("Read for Indexer\n");
			// TODO:  If needed  data size exceed range of buffer,  should gather potion of data from network.
#if 0 /* warning: comparison between signed and unsigned */
			if( stMoovBox.lCursorPos >= stMoovBox.ulStartOffset &&
			stMoovBox.unDataLength + stMoovBox.ulStartOffset >= stMoovBox.lCursorPos + sz_Try)
#else
			if( stMoovBox.lCursorPos >= (HINT64)stMoovBox.ulStartOffset &&
				(HINT64)(stMoovBox.unDataLength) >= (stMoovBox.lCursorPos - (HINT64)stMoovBox.ulStartOffset) + sz_Try) // <--
#endif
			{
				VK_MEM_Memcpy((HUINT8*)ptr, stMoovBox.pucMoov + (stMoovBox.lCursorPos - stMoovBox.ulStartOffset), sz_Try);
				stMoovBox.lCursorPos += sz_Try;
				*ptReadSize = sz_Try;
				sz_Real = sz_Try;
				//PrintDebug ("Read End\n");
				((DNP_USER_DATA_t *)tHandle.pDnpUserData)->indexer_last_seek_cursor += sz_Real;

				/* Removed goto success; */
				PrintDebug("%s(%d) : handle (0x%x) tryRead(%d) realRead(%d)\n",__FUNCTION__,__LINE__, tHandle.tFile.callback.unHandle,sz_Try, sz_Real);
				(void)VK_SEM_Release (s_ulHttpSemId);
				PrintExit;
				return ret;
			}
			else
			{
				(void)STREAM_HTTP_Seek (&stHttpAccessForIndexer, stMoovBox.lCursorPos);
				//PrintDebug ("Read For Indexer Failed!!\n");
			}
		}
#endif
		{
			/* read with real time */
			if(stHttpAccessForIndexer.info.i_size < stHttpAccessForIndexer.info.i_pos + sz_Try)
			{
				sz_Try = stHttpAccessForIndexer.info.i_size - stHttpAccessForIndexer.info.i_pos;
			}

			do
			{
				sz_Real += STREAM_HTTP_Read ( &stHttpAccessForIndexer, (HUINT8 *)ptr+sz_Real, sz_Try - sz_Real );
				if(stHttpAccessForIndexer.info.b_eof == TRUE )
				{
					PrintDebug ("%s(%d) : Info> Cursor reached in end of the file.\n",__FUNCTION__,__LINE__);
					break;
				}
			}
			while (sz_Real < sz_Try);

			*ptReadSize = sz_Real;
#if defined(USE_RAMBUFFERING_MOOVBOX_CACHING_SUPPORT)
			if(stMoovBox.eState == MOOVBOX_CACHING_STATE_CACHED)
			{
				stMoovBox.lCursorPos += sz_Real;
			}
#endif
		}
		((DNP_USER_DATA_t *)tHandle.pDnpUserData)->indexer_last_seek_cursor += sz_Real;

		/* Removed goto success; */
		PrintDebug("%s(%d) : handle (0x%x) tryRead(%d) realRead(%d)\n",__FUNCTION__,__LINE__, tHandle.tFile.callback.unHandle,sz_Try, sz_Real);
		(void)VK_SEM_Release (s_ulHttpSemId);
		PrintExit;
		return ret;
	}

	/**/
	/* Playback Read : Buffered reading */
	/**/
	{
		off_t offset;
		DNP_USER_DATA_t *pDnpUserData = NULL;
		PrintDebug("=== Stream Buffering read\n");

		if(s_NetworkError == TRUE)
		{
			*ptReadSize = 0;
			ret = VFIO_ERROR_IN_DEVICE;
			goto error;
		}

		pDnpUserData = (DNP_USER_DATA_t *)tHandle.pDnpUserData;
		if(pDnpUserData == NULL)
		{
			PrintError("%s(%d) Error > pDnpUserData is NULL\n", __FUNCTION__, __LINE__);
			ret = VFIO_ERROR_IN_DEVICE;
			goto error;
		}

		offset = pDnpUserData->player_last_seek_cursor;


		/* READ FROM ALIGN BUFFER FOR AVOID FREQUENTLY CLEAR THE BUFFER on Nexus Overlapped Request*/
		if(pDnpUserData->p_align_buffer)
		{
			HINT32 nDisplacement = (HINT32)(pDnpUserData->player_last_read_cursor + pDnpUserData->player_last_read_size - offset);
			//PrintDebug("Displacement is %d\n", nDisplacement);
			if(pDnpUserData->align_buffer_start_index <= offset && offset <= pDnpUserData->align_buffer_end_index)
			{
				HINT32 nCopyableBytesCount = 0;
				//PrintDebug("Nexus Overlapped Request Detected. Use align buffer for avoid outside seek\n");
				nCopyableBytesCount = nDisplacement;
				if((HINT32)sz_Try < nCopyableBytesCount)
				{
					nCopyableBytesCount = sz_Try;
				}
				//PrintDebug ("reuse start = (%"PRId64"), reuse end = (%"PRId64")\n", pDnpUserData->align_buffer_end_index - (nDisplacement - 1),
				//		pDnpUserData->align_buffer_end_index );
				if( ((NEXUS_ALIGN_BUFFER_SIZE - nDisplacement) >= 0) &&  ((NEXUS_ALIGN_BUFFER_SIZE - nDisplacement) < NEXUS_ALIGN_BUFFER_SIZE))
				{
					VK_memcpy(ptr, pDnpUserData->p_align_buffer + NEXUS_ALIGN_BUFFER_SIZE - nDisplacement, nCopyableBytesCount);
					PrintDebug ("Reused bytes from align buffer= %d\n", nCopyableBytesCount);

					offset += nCopyableBytesCount;
					sz_Real += nCopyableBytesCount;
				}
			}
		}

		/* SEEK inside Buffer */
		if ( (offset >= (off_t)sOffsetStart) && (offset <= (off_t)sOffsetEnd) )
		{
			{ /* read till seek point */
				HINT64 prior_data_len = offset - sOffsetStart;
				HUINT8 buffer[1024];

				while(prior_data_len > 0)
				{
					int readcount = 0;
					PrintDebug("Inside Seeking\n");
					if(prior_data_len > 1024)
					{
						readcount = P_STREAM_HTTP_ReadBuffer(&stHttpAccess, buffer, 1024);
					}
					else
					{
						readcount = P_STREAM_HTTP_ReadBuffer(&stHttpAccess, buffer, prior_data_len);
					}
					prior_data_len -= readcount;
				}
			}
		}
		else /* SEEK Outside Prebuffer */
		{
			PrintData ("%s(%d) : offset=%"PRId64", s(%"PRId64") e(%"PRId64") OUTSIDE SEEK..\n"
					,__FUNCTION__
					,__LINE__
					, offset
					,sOffsetStart
					,sOffsetEnd);
			stHttpAccess.info.b_prebuffered = TRUE;
			VK_SEM_Get(s_ulPreBufferingLock);
			sStreamBufferingRequestedSeekPoint = offset; /* 64bit assign is not atomic. so protect it */
			VK_SEM_Release (s_ulPreBufferingLock);
			while(TRUE)
			{
				VK_SEM_Get(s_ulPreBufferingLock);
				if(sStreamBufferingRequestedSeekPoint == -1)
				{
					VK_SEM_Release (s_ulPreBufferingLock);
					break;
				}
				VK_SEM_Release (s_ulPreBufferingLock);

				if(s_NetworkError == TRUE)
				{
					*ptReadSize = 0;
					PrintError("Error> STREAM BUFFERING SEEK %s(%d)\n", __FUNCTION__, __LINE__);
					VK_SEM_Release (s_ulPreBufferingLock);
					ret = VFIO_ERROR_IN_DEVICE;
					goto error;
				}

				PrintDebug("Wait SEEK in SeekForNexus\n");/* wait until seek is done */
				(void) VK_TASK_Sleep (50);
			}
		}

		s_llCurrentReadPosition = pDnpUserData->player_last_seek_cursor;

		/* READ 에서 return 을 안해주게 되면 NEXUS Playback 에서 waitingIO 가 걸린다. */
		sz_Real += P_STREAM_HTTP_ReadBuffer ( &stHttpAccess, (HUINT8 *)ptr + sz_Real, sz_Try - sz_Real);
		*ptReadSize = sz_Real;

		pDnpUserData->player_last_read_cursor = pDnpUserData->player_last_seek_cursor;
		pDnpUserData->player_last_read_size = size * nmemb;

#if INTEGRITY_STREAM_CHECK_WITH_COMPARING_LOCAL_FILE
		{
			HUINT8 * testbuff = DD_MEM_Alloc(pDnpUserData->player_last_read_size);
			if(testbuff)
			{
				off_t fdcur = lseek(fd,0, SEEK_CUR);
				size_t read_size = read(fd, testbuff, pDnpUserData->player_last_read_size);

				if(fdcur != pDnpUserData->player_last_seek_cursor)
				{
					PrintError("Seek Value is error\n");
					PrintError ("file offset=%"PRId64", tuner offset=%"PRId64"\n", fdcur, pDnpUserData->player_last_seek_cursor);
					VK_Panic();
					return VFIO_ERROR_IN_DEVICE;
				}
				if(read_size != pDnpUserData->player_last_read_size)
				{
					PrintError("Read Size Error\n");
					VK_Panic();
					return VFIO_ERROR_IN_DEVICE;
				}
				if(VK_memcmp(ptr, testbuff, pDnpUserData->player_last_read_size) != 0)
				{
					PrintError("Read Data Intigrity Check failed\n");
					VK_Panic();
					return VFIO_ERROR_IN_DEVICE;
				}
				else
				{
					PrintDebug("Data Intigrity is OK\n");
				}
				DD_MEM_Free(testbuff);
			}
		}
#endif


		pDnpUserData->player_last_seek_cursor += pDnpUserData->player_last_read_size; /* increase current seek cursor */

		PrintDebug ("read cur = (%"PRId64"), size = %d\n", pDnpUserData->player_last_read_cursor, pDnpUserData->player_last_read_size);


		/* SAVE to ALIGN BUFFER */
		if(pDnpUserData->p_align_buffer)
		{
			size_t align_buffering_size = pDnpUserData->player_last_read_size;
			if(align_buffering_size > NEXUS_ALIGN_BUFFER_SIZE)
			{
				align_buffering_size = NEXUS_ALIGN_BUFFER_SIZE;
			}

			pDnpUserData->align_buffer_start_index = pDnpUserData->player_last_read_cursor + pDnpUserData->player_last_read_size - align_buffering_size;
			pDnpUserData->align_buffer_end_index = pDnpUserData->align_buffer_start_index + align_buffering_size - 1;
    #if 0 /* Fixed warning: comparison of unsigned expression >= 0 is always true */
			if( ((NEXUS_ALIGN_BUFFER_SIZE - align_buffering_size) >= 0) &&  ((NEXUS_ALIGN_BUFFER_SIZE - align_buffering_size) < NEXUS_ALIGN_BUFFER_SIZE)
			&& (((pDnpUserData->player_last_read_size - (pDnpUserData->align_buffer_end_index - pDnpUserData->align_buffer_start_index + 1)) + ((pDnpUserData->align_buffer_end_index - pDnpUserData->align_buffer_start_index + 1))) <= sz_Try)
			&& ((pDnpUserData->align_buffer_end_index - pDnpUserData->align_buffer_start_index + 1) + (NEXUS_ALIGN_BUFFER_SIZE - align_buffering_size) <= NEXUS_ALIGN_BUFFER_SIZE ))
    #else
			if( ((NEXUS_ALIGN_BUFFER_SIZE - (int)align_buffering_size) >= 0)
					&&  ((NEXUS_ALIGN_BUFFER_SIZE - (int)align_buffering_size) < NEXUS_ALIGN_BUFFER_SIZE)
				&& (((pDnpUserData->player_last_read_size - (pDnpUserData->align_buffer_end_index - pDnpUserData->align_buffer_start_index + 1)) + ((pDnpUserData->align_buffer_end_index - pDnpUserData->align_buffer_start_index + 1))) <= sz_Try)
				&& ((pDnpUserData->align_buffer_end_index - pDnpUserData->align_buffer_start_index + 1) + (NEXUS_ALIGN_BUFFER_SIZE - align_buffering_size) <= NEXUS_ALIGN_BUFFER_SIZE ))
    #endif
			{
    #if 0 /* Fixed warning: pointer of type 'void *' used in arithmetic */
				VK_memcpy(pDnpUserData->p_align_buffer + NEXUS_ALIGN_BUFFER_SIZE - align_buffering_size,
						ptr + pDnpUserData->player_last_read_size - (pDnpUserData->align_buffer_end_index - pDnpUserData->align_buffer_start_index + 1),
						pDnpUserData->align_buffer_end_index - pDnpUserData->align_buffer_start_index + 1);
    #else
				VK_memcpy(pDnpUserData->p_align_buffer + NEXUS_ALIGN_BUFFER_SIZE - align_buffering_size,
						(HUINT8*)ptr + pDnpUserData->player_last_read_size - (pDnpUserData->align_buffer_end_index - pDnpUserData->align_buffer_start_index + 1),
						pDnpUserData->align_buffer_end_index - pDnpUserData->align_buffer_start_index + 1);
    #endif
			}

			//PrintDebug ("align buf start = (%"PRId64"), align buf end = (%"PRId64")\n", pDnpUserData->align_buffer_start_index, pDnpUserData->align_buffer_end_index);
			//PrintDebug ("buff size is %d\n", (HINT32)(pDnpUserData->align_buffer_end_index - pDnpUserData->align_buffer_start_index + 1));
		}
	}/* End Of PlayBack Reading */
#else /* STREAM_BUFFERING */
#if REAL_TIME_MULTISOCKET_SUPPORT
	if((unsigned int)s_tMultiHandle.DrmHandle == tHandle.tFile.callback.unHandle)
	{
		if(ptHttpAccess->info.i_size < ptHttpAccess->info.i_pos + sz_Try)
		{
			sz_Try = ptHttpAccess->info.i_size - ptHttpAccess->info.i_pos;
		}

		PrintDebug("Read for Drm(%"PRId64")\n\n",ptHttpAccess->info.i_pos );

		do
		{
			n_ReturnedValue = STREAM_HTTP_Read (ptHttpAccess, (HUINT8 *)ptr+sz_Real, sz_Try - sz_Real );
			if((error_retrycount < 10) &&(n_ReturnedValue == 0) && (ptHttpAccess->info.i_httpstatecode == HTTP_STATUS_MOVED_PERMANENTLY	/* 301 */
					|| ptHttpAccess->info.i_httpstatecode == HTTP_STATUS_FOUND 				/* 302 */
					|| ptHttpAccess->info.i_httpstatecode == HTTP_STATUS_SEE_OTHER 			/* 303 */
					|| ptHttpAccess->info.i_httpstatecode == HTTP_STATUS_TEMP_REDIRECT))		/* 307 */
			{
				error_retrycount++;
				continue;
			}

			if(n_ReturnedValue == DI_IPT_ETIMEOUT)
			{
				if(error_Timeoutcount != 1)
				{
					error_Timeoutcount++;
					continue;
				}
				else
				{
					error_Timeoutcount = 0;
				}
			}

			if(n_ReturnedValue < 0)
			{
				if(sz_Real > 0)
				{
					*ptReadSize = sz_Real;
				}
				else
				{
					*ptReadSize = n_ReturnedValue;
					ret = VFIO_ERROR_FAIL;
				}

				goto error;
			}
			else
			{
				sz_Real += n_ReturnedValue;
			}

			if((ReadFail_retrycount == 100)&&(sz_Real == 0))
			{
				PrintError("##### %s(%d) Retry Fail(Read Size = 0)(RetryCount=%d) #####\n\n", __FUNCTION__, __LINE__, ReadFail_retrycount);
				*ptReadSize = sz_Real;
				ReadFail_retrycount = 0;
				goto error;
			}

			if((ReadFail_retrycount == 100)&&(sz_Real > 0))
			{
				PrintError("##### %s(%d) Retry Fail(Read Size != 0)(RetryCount=%d) #####\n\n", __FUNCTION__, __LINE__, ReadFail_retrycount);
				*ptReadSize = sz_Real;
				ReadFail_retrycount = 0;
				goto error;
			}

			ReadFail_retrycount++;
		}
		while (sz_Real < sz_Try);

		*ptReadSize = sz_Real;
	}
	else if((unsigned int)s_tMultiHandle.PlayerHandle == tHandle.tFile.callback.unHandle)
	{
		if(tHandle.pDnpUserData != NULL)
		{
			ptHttpAccess = (STREAM_ACCESS_t *)tHandle.pDnpUserData;
		}
		else
		{
			PrintError ("%s(%d) Error> tHandle->pDnpUserData is null \n",__FUNCTION__,__LINE__);
			PrintExit;
			(void)VK_SEM_Release (s_ulHttpSemId);
			return VFIO_ERROR_IN_DEVICE;
		}

		if(ptHttpAccess->info.i_size < ptHttpAccess->info.i_pos + sz_Try)
		{
			sz_Try = ptHttpAccess->info.i_size - ptHttpAccess->info.i_pos;
#if REAL_TIME_CHUNKBUFFER_SUPPORT
			if(sz_Try == 0)
			{
				sz_Try = ptHttpAccess->info.i_size - s_ullCurrent_PlayCurPosition;
				if(sz_Try > size*nmemb)
				{
					sz_Try = size*nmemb;
				}
			}
#endif
		}

#if REAL_TIME_CHUNKBUFFER_SUPPORT
		Cureent_Index = ptHttpAccess->info.i_pos;

		if((s_tWMVInstance.pcData != NULL) && (s_ullCurrent_PlayCurPosition >= s_tWMVInstance.lBuffer_CursorPosition)  && (s_ullCurrent_PlayCurPosition < s_tWMVInstance.lBuffer_ReadPosition))
		{
			nTempReadSize = s_tWMVInstance.lBuffer_ReadPosition-s_ullCurrent_PlayCurPosition;
			nBufferReadPosition = s_tWMVInstance.lBuffer_ReadPosition-nTempReadSize-s_tWMVInstance.lBuffer_CursorPosition;
#if 0
			PrintDebug  ("\n%s(%d) : [+] i_pos=%"PRId64", ReadSize=%ld, s_ullCurrent_PlayCurPosition=%ld \n",__FUNCTION__,__LINE__, ptHttpAccess->info.i_pos, nTempReadSize, s_ullCurrent_PlayCurPosition);
			PrintDebug  ("\n%s(%d) : [+] lBuffer_CursorPosition=%ld, lBuffer_ReadPosition=%ld \n",__FUNCTION__,__LINE__, s_tWMVInstance.lBuffer_CursorPosition, s_tWMVInstance.lBuffer_ReadPosition);
			PrintDebug  ("\n%s(%d) : [+] position for Real read=%ld \n",__FUNCTION__,__LINE__, nBufferReadPosition);
#endif
			if(nTempReadSize < CHUNK_BUUFER_DEFAULT_SIZE)
			{
				VK_MEM_Memcpy(ptr, s_tWMVInstance.pcData+nBufferReadPosition, nTempReadSize);
				sz_Real = nTempReadSize;
				bIsWMABufferRead = TRUE;
			}
			else
			{
				bIsWMABufferRead = FALSE;
			}
		}

		if(s_tMP4MoovInstance.pcData != NULL)
		{
			if(ptHttpAccess->info.i_pos != s_ullCurrent_PlayCurPosition)
			{
				for(nRealtime_buffercount = 0; nRealtime_buffercount<REALTIME_PLAYBUFFER_CHUNK_TOTAL; nRealtime_buffercount++)
				{
					if((s_ullCurrent_PlayCurPosition == tRealtime_Buffer_Chunk[nRealtime_buffercount].ulStartindex) && (sz_Try <= tRealtime_Buffer_Chunk[nRealtime_buffercount].unData_Length))
					{
						VK_MEM_Memcpy(ptr, tRealtime_Buffer_Chunk[nRealtime_buffercount].pcData, sz_Try);
						s_ullCurrent_PlayCurPosition += sz_Try;
						*ptReadSize = sz_Try;
						(void)VK_SEM_Release (s_ulHttpSemId);

//							PrintDebug("##### tRealtime_Buffer_Chunk[nRealtime_buffercount].ulStartindex=%ld #####\n", tRealtime_Buffer_Chunk[nRealtime_buffercount].ulStartindex);
						return VFIO_ERROR_SUCCESS;
					}
				}

				if(nRealtime_buffercount == REALTIME_PLAYBUFFER_CHUNK_TOTAL)
				{
					PrintDebug("######### [%s] Critical Section: Fail Read #######\n", __FUNCTION__);
					if(P_STREAM_HTTP_SeekforRealtimeBuffer (tHandle, s_ullCurrent_PlayCurPosition, 0))
					{
						PrintError ("%s(%d) Error> STREAM_HTTP_Seek fail \n",__FUNCTION__,__LINE__);
						PrintExit;
						(void)VK_SEM_Release (s_ulHttpSemId);
						return 0;
					}
				}
			}
		}
#endif

		s_llCurrentReadPosition = ptHttpAccess->info.i_pos ;

		PrintDebug("Read for Player(%"PRId64")\n\n",ptHttpAccess->info.i_pos);
		do
		{
			n_ReturnedValue = STREAM_HTTP_Read ( ptHttpAccess, (HUINT8 *)ptr+sz_Real, sz_Try - sz_Real );
			if((error_retrycount < 10) &&(n_ReturnedValue == 0) && (ptHttpAccess->info.i_httpstatecode == HTTP_STATUS_MOVED_PERMANENTLY	/* 301 */
					|| ptHttpAccess->info.i_httpstatecode == HTTP_STATUS_FOUND 				/* 302 */
					|| ptHttpAccess->info.i_httpstatecode == HTTP_STATUS_SEE_OTHER 			/* 303 */
					|| ptHttpAccess->info.i_httpstatecode == HTTP_STATUS_TEMP_REDIRECT))		/* 307 */
			{
				error_retrycount++;
				continue;
			}

			if(n_ReturnedValue == DI_IPT_ETIMEOUT)
			{
				if(error_Timeoutcount != 2)
				{
					error_Timeoutcount++;
					continue;
				}
				else
				{
					error_Timeoutcount = 0;
				}
			}

			if(n_ReturnedValue < 0)
			{
				if(sz_Real > 0)
				{
					s_ullCurrent_PlayCurPosition += sz_Real;
					*ptReadSize = sz_Real;
				}
				else
				{
					*ptReadSize = 0;
					ret = VFIO_ERROR_FAIL;
				}

				PrintError("##### %s(%d) Retry SocketRrror #####\n\n", __FUNCTION__, __LINE__);
				if((error_SockErrorcount == 0) &&(STREAM_HTTP_Seek (ptHttpAccess, ptHttpAccess->info.i_pos) == DI_IPT_SUCCESS))
				{
					s_ullCurrent_PlayCurPosition = ptHttpAccess->info.i_pos;
					error_SockErrorcount++;
					continue;
				}

				goto error;
			}
			else
			{
				sz_Real += n_ReturnedValue;
			}

			if((ReadFail_retrycount == 100)&&(sz_Real == 0))
			{
				PrintError("##### %s(%d) Retry Fail(Read Size = 0)(RetryCount=%d) #####\n\n", __FUNCTION__, __LINE__, ReadFail_retrycount);
				*ptReadSize = sz_Real;
				(void)STREAM_HTTP_Seek (ptHttpAccess, ptHttpAccess->info.i_pos );
				s_ullCurrent_PlayCurPosition = ptHttpAccess->info.i_pos;
				ReadFail_retrycount = 0;
				goto error;
			}

			if((ReadFail_retrycount == 100)&&(sz_Real > 0))
			{
				*ptReadSize = sz_Real;
				s_ullCurrent_PlayCurPosition += sz_Real;
				PrintError("##### %s(%d) Retry Fail(Read Size != 0)(RetryCount=%d, sz_Real=%d) #####\n\n", __FUNCTION__, __LINE__, ReadFail_retrycount, sz_Real);
				(void)STREAM_HTTP_Seek (ptHttpAccess, s_ullCurrent_PlayCurPosition );
				ReadFail_retrycount = 0;
				goto error;
			}

			ReadFail_retrycount++;
		}
		while (sz_Real < sz_Try);

#if REAL_TIME_CHUNKBUFFER_SUPPORT
		if(s_tMP4MoovInstance.pcData != NULL)
		{
			(void)P_STREAM_HTTP_UpdateRealTimeBuffer(ptr, sz_Real, Cureent_Index);

			s_ullCurrent_PlayCurPosition += sz_Real;
		}
		else if((s_tWMVInstance.pcData != NULL) && (CHUNK_BUUFER_DEFAULT_SIZE > sz_Real) && (sz_Real > 0))
		{
			VK_MEM_Memcpy(s_tWMVInstance.pcData, ptr, sz_Real);

			if(bIsWMABufferRead == TRUE)
			{
				s_tWMVInstance.lBuffer_CursorPosition = Cureent_Index-nTempReadSize;
				s_tWMVInstance.lBuffer_ReadPosition = Cureent_Index+sz_Real-nTempReadSize;
				bIsWMABufferRead = FALSE;
				s_ullCurrent_PlayCurPosition += sz_Real;
			}
			else
			{
				s_tWMVInstance.lBuffer_CursorPosition = Cureent_Index;
				s_tWMVInstance.lBuffer_ReadPosition = Cureent_Index+sz_Real;
				s_ullCurrent_PlayCurPosition += sz_Real;
			}

			PrintDebug ("\n%s(%d) : lBuffer_CursorPosition=%"PRId64", lBuffer_ReadPosition=%"PRId64" \n",
				__FUNCTION__,__LINE__, s_tWMVInstance.lBuffer_CursorPosition, s_tWMVInstance.lBuffer_ReadPosition);
		}
#endif
		*ptReadSize = sz_Real;
	}
	else if((unsigned int)s_tMultiHandle.IndexHandle == tHandle.tFile.callback.unHandle)
	{
#if defined(USE_MOOVBOX_CACHING)
		if(stMoovBox.eState == MOOVBOX_CACHING_STATE_CACHED)
		{
			if ((stMoovBox.lCursorPos >= (HINT64)stMoovBox.ulStartOffset) &&
				(HINT64)(stMoovBox.unDataLength) >= (stMoovBox.lCursorPos - (HINT64)stMoovBox.ulStartOffset) + sz_Try )
				{
				VK_MEM_Memcpy(ptr, stMoovBox.pucMoov + stMoovBox.lCursorPos - stMoovBox.ulStartOffset, sz_Try);
				*ptReadSize = sz_Try;
				stMoovBox.lCursorPos += sz_Try;
				(void)VK_SEM_Release (s_ulHttpSemId);
				return VFIO_ERROR_SUCCESS;
			}
#if 1
			else
			{
				if(ptHttpAccess->info.i_pos != stMoovBox.lCursorPos)
				{
					(void)STREAM_HTTP_Seek (ptHttpAccess, stMoovBox.lCursorPos);
				}
			}
#endif
		}
#endif
		if(ptHttpAccess->info.i_size < ptHttpAccess->info.i_pos + sz_Try)
		{
			sz_Try = ptHttpAccess->info.i_size - ptHttpAccess->info.i_pos;
		}

#if REAL_TIME_INDEXBUFFER_SUPPORT
		if(s_tMP4MoovInstance.pcData != NULL)
		{
			PrintDebug("[%s:%d] CursorPos:%"PRId64" NextPos:%"PRId64" ReadPos:%"PRId64" sz_Try:%d\n", __FUNCTION__,__LINE__,
				s_tMP4MoovInstance.lBuffer_CursorPosition,(s_tMP4MoovInstance.lBuffer_ReadPosition+sz_Try),
				s_tMP4MoovInstance.lBuffer_ReadPosition,sz_Try);
			if((sz_Try > 0) && ((s_tMP4MoovInstance.lBuffer_ReadPosition+sz_Try) <= s_tMP4MoovInstance.lBuffer_CursorPosition))
			{
				PrintDebug("##### [%s] lBuffer_ReadPosition=%"PRId64", sz_Try=%d #####\n", __FUNCTION__,s_tMP4MoovInstance.lBuffer_ReadPosition,  sz_Try);
				PrintDebug("##### [%s] Inside Read #####\n", __FUNCTION__);

				VK_MEM_Memcpy(ptr, s_tMP4MoovInstance.pcData+s_tMP4MoovInstance.lBuffer_ReadPosition, sz_Try);

				s_tMP4MoovInstance.lBuffer_ReadPosition += sz_Try;

	//			Util_DebugPrint_Dump(ptr, lBuffer_ReadPosition, 0,1);

				sz_Real = sz_Try;
				*ptReadSize = sz_Real;

				ret = VFIO_ERROR_SUCCESS;

				/* goto success; */
				PrintDebug("%s(%d) : handle (0x%x) tryRead(%d) realRead(%d)\n",__FUNCTION__,__LINE__, tHandle.tFile.callback.unHandle,sz_Try, sz_Real);
				(void)VK_SEM_Release (s_ulHttpSemId);
				PrintExit;
				return ret;
			}
			else if((sz_Try > 0) && (s_tMP4MoovInstance.lBuffer_ReadPosition < s_tMP4MoovInstance.lBuffer_CursorPosition))
			{
				PrintError("##### [%s] lBuffer_ReadPosition=%"PRId64", sz_Real=%d #####\n", __FUNCTION__,s_tMP4MoovInstance.lBuffer_ReadPosition,  sz_Try);
				PrintError("##### [%s] Cretical Session : Fail #####\n", __FUNCTION__);
			}
		}
		else
		{
			for(nIndex=0;nIndex<REALTIME_PROBEBUFFER_CHUNK_TOTAL;nIndex++)
			{
				if((tRealtime_ProbeBuffer_Chunk[nIndex].ulStartindex == s_ullCurrent_IndexCurPosition) && (tRealtime_ProbeBuffer_Chunk[nIndex].unData_Length >= sz_Try))
				{
					PrintDebug("##### [%s] Probe Buffer Data Read(%d) #####\n", __FUNCTION__, nIndex);

					VK_MEM_Memcpy(ptr, tRealtime_ProbeBuffer_Chunk[nIndex].pcData, sz_Try);

					PrintDebug("##### ulStartindex=%"PRId64", unData_Length=%d, sz_Real=%d #####\n", tRealtime_ProbeBuffer_Chunk[nIndex].ulStartindex, tRealtime_ProbeBuffer_Chunk[nIndex].unData_Length, sz_Try);

					s_ullCurrent_IndexCurPosition += sz_Try;
					sz_Real = sz_Try;
					*ptReadSize = sz_Try;

					ret = VFIO_ERROR_SUCCESS;

					/* goto success; */
					PrintDebug("%s(%d) : handle (0x%x) tryRead(%d) realRead(%d)\n",__FUNCTION__,__LINE__, tHandle.tFile.callback.unHandle,sz_Try, sz_Real);
					(void)VK_SEM_Release (s_ulHttpSemId);
					PrintExit;
					return ret;
				}
			}

			if(s_ullCurrent_IndexCurPosition != ptHttpAccess->info.i_pos)
			{
				(void)P_STREAM_HTTP_SeekforRealtimeBuffer(tHandle, (off_t)s_ullCurrent_IndexCurPosition, 0);

				if(ptHttpAccess->info.i_size < ptHttpAccess->info.i_pos + sz_Try)
				{
					sz_Try = ptHttpAccess->info.i_size - ptHttpAccess->info.i_pos;
				}

				sz_Real = 0;
			}
		}
#endif
		PrintDebug("Read for Indexer(%u, %"PRId64")\n\n", sz_Try, ptHttpAccess->info.i_pos );

		do
		{
			n_ReturnedValue = STREAM_HTTP_Read (ptHttpAccess, (HUINT8 *)ptr+sz_Real, sz_Try - sz_Real );
			if((error_retrycount < 10) &&(n_ReturnedValue == 0) && (ptHttpAccess->info.i_httpstatecode == HTTP_STATUS_MOVED_PERMANENTLY	/* 301 */
					|| ptHttpAccess->info.i_httpstatecode == HTTP_STATUS_FOUND 				/* 302 */
					|| ptHttpAccess->info.i_httpstatecode == HTTP_STATUS_SEE_OTHER 			/* 303 */
					|| ptHttpAccess->info.i_httpstatecode == HTTP_STATUS_TEMP_REDIRECT))		/* 307 */
			{
				error_retrycount++;
				continue;
			}

			if(n_ReturnedValue == DI_IPT_ETIMEOUT)
			{
				if(error_Timeoutcount != 1)
				{
					error_Timeoutcount++;
					continue;
				}
				else
				{
					error_Timeoutcount = 0;
				}
			}

			if(n_ReturnedValue < 0)
			{
				if(sz_Real > 0)
				{
					*ptReadSize = sz_Real;
#if REAL_TIME_INDEXBUFFER_SUPPORT
					s_ullCurrent_IndexCurPosition += sz_Real;
#endif
				}
				else
				{
					*ptReadSize = 0;
					ret = VFIO_ERROR_FAIL;
				}

				goto error;
			}
			else
			{
				sz_Real += n_ReturnedValue;
			}

			if((ReadFail_retrycount == 100)&&(sz_Real == 0))
			{
				PrintError("##### %s(%d) Retry Fail(Read Size = 0)(RetryCount=%d) #####\n\n", __FUNCTION__, __LINE__, ReadFail_retrycount);
				*ptReadSize = sz_Real;
				ReadFail_retrycount = 0;
				goto error;
			}

			if((ReadFail_retrycount == 100)&&(sz_Real > 0))
			{
				PrintError("##### %s(%d) Retry Fail(Read Size != 0)(RetryCount=%d) #####\n\n", __FUNCTION__, __LINE__, ReadFail_retrycount);
#if REAL_TIME_INDEXBUFFER_SUPPORT
				s_ullCurrent_IndexCurPosition += sz_Real;
#endif
				*ptReadSize = sz_Real;
				ReadFail_retrycount = 0;
				goto error;
			}

			ReadFail_retrycount++;
		}
		while (sz_Real < sz_Try);

#if defined(USE_MOOVBOX_CACHING)
		if(stMoovBox.eState == MOOVBOX_CACHING_STATE_CACHED)
		{
			stMoovBox.lCursorPos += sz_Real;
		}
#endif

#if REAL_TIME_INDEXBUFFER_SUPPORT
		s_ullCurrent_IndexCurPosition += sz_Real;
#endif
		*ptReadSize = sz_Real;
	}
	else
	{
#if defined(USE_MOOVBOX_CACHING)
		if(stMoovBox.eState == MOOVBOX_CACHING_STATE_CACHED)
		{
			if ((stMoovBox.lCursorPos >= (HINT64)stMoovBox.ulStartOffset) &&
				(HINT64)(stMoovBox.unDataLength) >= (stMoovBox.lCursorPos - (HINT64)stMoovBox.ulStartOffset) + sz_Try )
				{
				VK_MEM_Memcpy(ptr, stMoovBox.pucMoov + stMoovBox.lCursorPos - stMoovBox.ulStartOffset, sz_Try);
				*ptReadSize = sz_Try;
				stMoovBox.lCursorPos += sz_Try;
				(void)VK_SEM_Release (s_ulHttpSemId);
				return VFIO_ERROR_SUCCESS;
			}
#if 1
			else
			{
				if(ptHttpAccess->info.i_pos != stMoovBox.lCursorPos)
				{
					(void)STREAM_HTTP_Seek (ptHttpAccess, stMoovBox.lCursorPos);
				}
			}
#endif
		}
#endif
		if(ptHttpAccess->info.i_size < ptHttpAccess->info.i_pos + sz_Try)
		{
			sz_Try = ptHttpAccess->info.i_size - ptHttpAccess->info.i_pos;
		}
#if REAL_TIME_INDEXBUFFER_SUPPORT
		Cureent_Index = ptHttpAccess->info.i_pos;

		if(s_tMP4MoovInstance.pcData != NULL)
		{
			PrintDebug("[%s:%d] CursorPos:%"PRId64" NextPos:%"PRId64" ReadPos:%"PRId64" sz_Try:%d\n", __FUNCTION__,__LINE__,
				s_tMP4MoovInstance.lBuffer_CursorPosition,(s_tMP4MoovInstance.lBuffer_ReadPosition+sz_Try),
				s_tMP4MoovInstance.lBuffer_ReadPosition,sz_Try);
			if((sz_Try > 0) && ((s_tMP4MoovInstance.lBuffer_ReadPosition+sz_Try) < s_tMP4MoovInstance.lBuffer_CursorPosition))
			{
				PrintDebug("##### [%s] lBuffer_ReadPosition=%"PRId64", sz_Try=%d #####\n", __FUNCTION__,s_tMP4MoovInstance.lBuffer_ReadPosition,  sz_Try);
				PrintDebug("##### [%s] Inside Read #####\n", __FUNCTION__);

				VK_MEM_Memcpy(ptr, s_tMP4MoovInstance.pcData+s_tMP4MoovInstance.lBuffer_ReadPosition, sz_Try);

				s_tMP4MoovInstance.lBuffer_ReadPosition += sz_Try;

	//			Util_DebugPrint_Dump(ptr, lBuffer_ReadPosition, 0,1);

				sz_Real = sz_Try;
				*ptReadSize = sz_Real;

				ret = VFIO_ERROR_SUCCESS;

				/* goto success; */
				PrintDebug("%s(%d) : handle (0x%x) tryRead(%d) realRead(%d)\n",__FUNCTION__,__LINE__, tHandle.tFile.callback.unHandle,sz_Try, sz_Real);
				(void)VK_SEM_Release (s_ulHttpSemId);
				PrintExit;
				return ret;
			}
			else if((sz_Try > 0) && (s_tMP4MoovInstance.lBuffer_ReadPosition < s_tMP4MoovInstance.lBuffer_CursorPosition))
			{
				PrintDebug("##### [%s] lBuffer_ReadPosition=%"PRId64", sz_Real=%d #####\n", __FUNCTION__,s_tMP4MoovInstance.lBuffer_ReadPosition,  sz_Try);
				PrintDebug("##### [%s] Cretical Session : Fail #####\n", __FUNCTION__);
			}
		}
#endif

		do
		{
			n_ReturnedValue = STREAM_HTTP_Read (ptHttpAccess, (HUINT8 *)ptr+sz_Real, sz_Try - sz_Real );

			if(n_ReturnedValue == DI_IPT_ETIMEOUT)
			{
				if(error_Timeoutcount != 1)
				{
					error_Timeoutcount++;
					continue;
				}
				else
				{
					error_Timeoutcount = 0;
				}
			}

			if(n_ReturnedValue < 0)
			{
				if(sz_Real > 0)
				{
#if REAL_TIME_INDEXBUFFER_SUPPORT
					s_tMP4MoovInstance.lBuffer_CursorPosition += sz_Real;
					s_nMoovBuffer_TotalUsedMemory += sz_Real;
#endif
					*ptReadSize = sz_Real;
				}
				else
				{
					*ptReadSize = 0;
					ret = VFIO_ERROR_FAIL;
				}

				goto error;
			}
			else
			{
				sz_Real += n_ReturnedValue;
			}

			if((ReadFail_retrycount == 100)&&(sz_Real == 0))
			{
				PrintError("##### %s(%d) Retry Fail(Read Size = 0)(RetryCount=%d) #####\n\n", __FUNCTION__, __LINE__, ReadFail_retrycount);
				*ptReadSize = sz_Real;
				ReadFail_retrycount = 0;
				goto error;
			}

			if((ReadFail_retrycount == 100)&&(sz_Real > 0))
			{
				PrintError("##### %s(%d) Retry Fail(Read Size != 0)(RetryCount=%d) #####\n\n", __FUNCTION__, __LINE__, ReadFail_retrycount);
				*ptReadSize = sz_Real;
#if REAL_TIME_INDEXBUFFER_SUPPORT
				s_tMP4MoovInstance.lBuffer_CursorPosition += sz_Real;
				s_nMoovBuffer_TotalUsedMemory += sz_Real;
#endif
				ReadFail_retrycount = 0;
				goto error;
			}

			ReadFail_retrycount++;
		}
		while (sz_Real < sz_Try);

#if REAL_TIME_CHUNKBUFFER_SUPPORT
		if((s_tMultiHandle.ProbeHandle != 0) && (sz_Real > 0))
		{
			PrintDebug("##### [%s] Cureent_Index=%"PRId64", sz_Real=%d #####\n", __FUNCTION__,Cureent_Index,  sz_Real);

#if REAL_TIME_INDEXBUFFER_SUPPORT
			if(s_tMP4MoovInstance.pcData != NULL)
			{
				PrintDebug("[%s:%d] CursorPos:%"PRId64" ReadPos:%"PRId64" sz_Real:%d\n", __FUNCTION__,__LINE__,
					s_tMP4MoovInstance.lBuffer_CursorPosition,s_tMP4MoovInstance.lBuffer_ReadPosition,sz_Real);

				VK_MEM_Memcpy(s_tMP4MoovInstance.pcData+s_tMP4MoovInstance.lBuffer_CursorPosition, ptr, sz_Real);
				s_tMP4MoovInstance.lBuffer_CursorPosition += sz_Real;
				s_tMP4MoovInstance.lBuffer_ReadPosition = s_tMP4MoovInstance.lBuffer_CursorPosition;

				PrintDebug("[%s:%d] CursorPos:%"PRId64" ReadPos:%"PRId64" \n", __FUNCTION__,__LINE__,
					s_tMP4MoovInstance.lBuffer_CursorPosition,s_tMP4MoovInstance.lBuffer_ReadPosition);

				s_nMoovBuffer_TotalUsedMemory += sz_Real;
				PrintDebug("######### [%s] Success Read(lBuffer_CursorPosition=%"PRId64", s_nProbeBuffer_TotalMemory=%u) #######\n",
					__FUNCTION__, s_tMP4MoovInstance.lBuffer_CursorPosition, s_nMoovBuffer_TotalUsedMemory);
			}
			else
			{
				if(s_ullCurrent_ProbeBuffer_Index < REALTIME_PROBEBUFFER_CHUNK_TOTAL)
				{
					tRealtime_ProbeBuffer_Chunk[s_ullCurrent_ProbeBuffer_Index].ulStartindex = Cureent_Index;
					tRealtime_ProbeBuffer_Chunk[s_ullCurrent_ProbeBuffer_Index].pcData = DD_MEM_Alloc(sz_Real+1);
					if(tRealtime_ProbeBuffer_Chunk[s_ullCurrent_ProbeBuffer_Index].pcData != NULL)
					{
						VK_MEM_Memcpy(tRealtime_ProbeBuffer_Chunk[s_ullCurrent_ProbeBuffer_Index].pcData, ptr, sz_Real);
						tRealtime_ProbeBuffer_Chunk[s_ullCurrent_ProbeBuffer_Index].unData_Length = sz_Real;
						s_nProbeBuffer_TotalUsedMemory += sz_Real+1;
						PrintDebug("##### [%s] MemAlloc(%"PRId64") s_nProbeBuffer_TotalUsedMemory=%d #####\n",
								__FUNCTION__, s_ullCurrent_ProbeBuffer_Index, s_nProbeBuffer_TotalUsedMemory);
						s_ullCurrent_ProbeBuffer_Index++;
					}
				}
			}
#endif
		}

//		Util_DebugPrint_Dump(ptr, sz_Real, 0,1);
#endif
#if defined(USE_MOOVBOX_CACHING)
		if(stMoovBox.eState == MOOVBOX_CACHING_STATE_CACHED)
		{
			stMoovBox.lCursorPos += sz_Real;
		}
#endif
		*ptReadSize = sz_Real;
	}
#else /* REAL_TIME_MULTISOCKET_SUPPORT */
	if(stHttpAccess.info.i_size < stHttpAccess.info.i_pos + sz_Try)
	{
		sz_Try = stHttpAccess.info.i_size - stHttpAccess.info.i_pos;
	}

	do
	{
		n_ReturnedValue = STREAM_HTTP_Read ( &stHttpAccess, (HUINT8 *)ptr+sz_Real, sz_Try - sz_Real );

		if(n_ReturnedValue == DI_IPT_ETIMEOUT)
		{
			if(error_Timeoutcount != 2)
			{
				error_Timeoutcount++;
				continue;
			}
			else
			{
				error_Timeoutcount = 0;
			}
		}

		if(n_ReturnedValue < 0)
		{
			if(sz_Real > 0)
			{
				*ptReadSize = sz_Real;
			}
			else
			{
				*ptReadSize = 0;
				ret = VFIO_ERROR_FAIL;
			}

			goto error;
		}
		else
		{
			sz_Real += n_ReturnedValue;
		}

		if((ReadFail_retrycount == 100)&&(sz_Real == 0))
		{
			PrintError("##### %s(%d) Retry Fail(Read Size = 0)(RetryCount=%d) #####\n\n", __FUNCTION__, __LINE__, ReadFail_retrycount);
			*ptReadSize = sz_Real;
			(void)STREAM_HTTP_Seek (&stHttpAccess, stHttpAccess.info.i_pos);
			ReadFail_retrycount = 0;
			goto error;
		}

		if((ReadFail_retrycount == 100)&&(sz_Real > 0))
		{
			*ptReadSize = sz_Real;
			PrintError("##### %s(%d) Retry Fail(Read Size != 0)(RetryCount=%d, sz_Real=%d) #####\n\n", __FUNCTION__, __LINE__, ReadFail_retrycount, sz_Real);
			(void)STREAM_HTTP_Seek (&stHttpAccess, stHttpAccess.info.i_pos);
			ReadFail_retrycount = 0;
			goto error;
		}

		ReadFail_retrycount++;
	}
	while (sz_Real < sz_Try);

	*ptReadSize = sz_Real;
#endif /* REAL_TIME_MULTISOCKET_SUPPORT */
#endif /* STREAM_BUFFERING */

error:

#if 0 /* Removed warning: label 'success' defined but not used */
success:
#endif

	PrintDebug("%s(%d) : handle (0x%x) tryRead(%d) realRead(%d)\n",__FUNCTION__,__LINE__, tHandle.tFile.callback.unHandle,sz_Try, sz_Real);
	(void)VK_SEM_Release (s_ulHttpSemId);

	PrintExit;

	return ret;
}

/** This Function called by bmedia probe only.*/
int DI_STREAM_HTTP_Seek (VFIO_Handle_t tHandle, off_t offset, int whence)
{
#if REAL_TIME_MULTISOCKET_SUPPORT
	STREAM_ACCESS_t *ptHttpAccess=NULL;
#if REAL_TIME_CHUNKBUFFER_SUPPORT || REAL_TIME_INDEXBUFFER_SUPPORT
	HUINT32 nReadSize = 0;
	HUINT32 nTempReadSize = 0;
	HUINT32 nMaxReadCount = 0;
	HUINT32 nCurrentReadCount = 0;
	HINT64 lBlockSize = 0;
	HINT8 *pcTempBuffer=NULL;
#endif
#endif

	UNUSED_PARAM (tHandle);
	(void)VK_SEM_Get(s_ulHttpSemId);
	PrintEnter;
	PrintData("[%s:%d] : offset=%"PRId64", whence=%d\n",__FUNCTION__,__LINE__, offset, whence);

#if REAL_TIME_MULTISOCKET_SUPPORT
	if(tHandle.pDnpUserData != NULL)
	{
		ptHttpAccess = (STREAM_ACCESS_t *)tHandle.pDnpUserData;
	}
	else
	{
		PrintError ("%s(%d) Error> tHandle->pDnpUserData is null \n",__FUNCTION__,__LINE__);
		PrintExit;
		(void)VK_SEM_Release (s_ulHttpSemId);
		return VFIO_ERROR_IN_DEVICE;
	}

	if(whence == SEEK_END)
	{
		offset = (off_t)ptHttpAccess->info.i_size;
	}
#else
	if(whence == SEEK_END)
	{
		offset = (off_t)stHttpAccess.info.i_size;
	}
#endif

#if STREAM_BUFFERING

	/* null check */
	if(tHandle.pDnpUserData == NULL)
	{
		PrintError("pDnpUserData is NULL\n");
		(void)VK_SEM_Release (s_ulHttpSemId);
		return -1;
	}
#endif
	PrintDebug("== Probe Seek\n");

#if 0
	s_ullCurrent_IndexCurPosition = offset;
	PrintDebug("s_ullCurrent_IndexCurPosition=%ld\n", s_ullCurrent_IndexCurPosition);
	PrintExit;
	(void)VK_SEM_Release (s_ulHttpSemId);
	return VFIO_ERROR_SUCCESS;
#endif

#if REAL_TIME_MULTISOCKET_SUPPORT
	if(ptHttpAccess->info.i_pos == offset)
#else
	if(stHttpAccess.info.i_pos == offset)
#endif
	{
#if REAL_TIME_CHUNKBUFFER_SUPPORT || REAL_TIME_INDEXBUFFER_SUPPORT
		PrintDebug("[%s:%d] CursorPos:%"PRId64" ReadPos:%"PRId64" \n", __FUNCTION__,__LINE__,
				s_tMP4MoovInstance.lBuffer_CursorPosition,s_tMP4MoovInstance.lBuffer_ReadPosition);
		s_tMP4MoovInstance.lBuffer_ReadPosition = offset;
#endif

#if defined(USE_RAMBUFFERING_MOOVBOX_CACHING_SUPPORT)|| defined(USE_MOOVBOX_CACHING)
		if(stMoovBox.eState == MOOVBOX_CACHING_STATE_CACHED)
		{
			stMoovBox.lCursorPos = offset;
		}
#endif
		PrintExit;
		(void)VK_SEM_Release (s_ulHttpSemId);
		return VFIO_ERROR_SUCCESS;
	}

#if REAL_TIME_MULTISOCKET_SUPPORT
#if REAL_TIME_INDEXBUFFER_SUPPORT
	if((s_tMP4MoovInstance.lBuffer_CursorPosition > offset) && (s_tMP4MoovInstance.pcData != NULL))
	{
		s_tMP4MoovInstance.lBuffer_ReadPosition = offset;
		PrintData ("%s(%d) : lBuffer_ReadPosition=%"PRId64", offset=%"PRId64"\n",__FUNCTION__,__LINE__, s_tMP4MoovInstance.lBuffer_ReadPosition, offset);
		PrintExit;
		(void)VK_SEM_Release (s_ulHttpSemId);
		return VFIO_ERROR_SUCCESS;
	}

	if(s_tMP4MoovInstance.pcData == NULL)
	{
		nMaxReadCount = 0;
	}
	else
	{
		if(((ptHttpAccess->info.i_pos+BROADCOM_PLAYBACK_READ_BLOCK*40) > offset) && (ptHttpAccess->info.i_pos < offset))
		{
			PrintData ("%s(%d) : ptHttpAccess->info.i_pos=%"PRId64", offset=%"PRId64", test=%"PRId64"\n",__FUNCTION__,__LINE__, ptHttpAccess->info.i_pos, offset, (offset-ptHttpAccess->info.i_pos));
			lBlockSize = BROADCOM_PLAYBACK_READ_BLOCK;
			nMaxReadCount = (offset-ptHttpAccess->info.i_pos)/lBlockSize;
			nTempReadSize = (offset-ptHttpAccess->info.i_pos-lBlockSize*nMaxReadCount);
			if(nTempReadSize > 0)
			{
				nMaxReadCount += 1;
			}

			if(nTempReadSize == 0)
			{
				nTempReadSize = BROADCOM_PLAYBACK_READ_BLOCK;
			}

			PrintData ("%s(%d) : nMaxReadCount=%u, lBlockSize=%"PRId64", nTempReadSize=%u\n",
				__FUNCTION__,__LINE__, nMaxReadCount, lBlockSize, nTempReadSize);
		}

		if(nMaxReadCount > 0)
		{
			for(nCurrentReadCount = 0; nCurrentReadCount < nMaxReadCount; nCurrentReadCount++)
			{
				if((nMaxReadCount-1) != nCurrentReadCount)
				{
					if(s_tMP4MoovInstance.pcData != NULL)
					{
						(void)P_STREAM_HTTP_ReadforRealtimeBuffer(tHandle, s_tMP4MoovInstance.pcData+s_tMP4MoovInstance.lBuffer_CursorPosition, BROADCOM_PLAYBACK_READ_BLOCK, 1, &nReadSize);
					}
#if REAL_TIME_CHUNKBUFFER_SUPPORT
					else
					{
						pcTempBuffer = DD_MEM_Alloc(CHUNK_BUUFER_DEFAULT_SIZE);
						if(pcTempBuffer == NULL)
						{
							PrintError("%s(%d) : Error> Fail Mem alloc.\n",__FUNCTION__,__LINE__);
							(void)VK_SEM_Release (s_ulHttpSemId);

							PrintExit;
							return VFIO_ERROR_SUCCESS;
						}
						else
						{
							VK_MEM_Memset(pcTempBuffer, 0x0, CHUNK_BUUFER_DEFAULT_SIZE);
						}

						(void)P_STREAM_HTTP_ReadforRealtimeBuffer(tHandle, pcTempBuffer, BROADCOM_PLAYBACK_READ_BLOCK, 1, &nReadSize);

						if(pcTempBuffer != NULL)
						{
							DD_MEM_Free(pcTempBuffer);
							pcTempBuffer = NULL;
						}
					}
#endif
					if(BROADCOM_PLAYBACK_READ_BLOCK != nReadSize)
					{
						PrintDebug("######### [%s] Critical Section: Fail Read #######\n", __FUNCTION__);
						(void)VK_SEM_Release (s_ulHttpSemId);

						PrintExit;
						return VFIO_ERROR_FAIL;
					}
					else
					{
						if(s_tMP4MoovInstance.pcData != NULL)
						{
							s_tMP4MoovInstance.lBuffer_CursorPosition += nReadSize;
							s_nMoovBuffer_TotalUsedMemory += nReadSize;
							PrintDebug("######### [%s] Success Read(lBuffer_CursorPosition=%"PRId64", s_nMoovBuffer_TotalUsedMemory=%u) #######\n",
								__FUNCTION__, s_tMP4MoovInstance.lBuffer_CursorPosition, s_nMoovBuffer_TotalUsedMemory);
						}
					}
				}
				else
				{
					if(s_tMP4MoovInstance.pcData != NULL)
					{
						(void)P_STREAM_HTTP_ReadforRealtimeBuffer(tHandle, s_tMP4MoovInstance.pcData+s_tMP4MoovInstance.lBuffer_CursorPosition, nTempReadSize, 1, &nReadSize);
					}
#if REAL_TIME_CHUNKBUFFER_SUPPORT
					else
					{
						pcTempBuffer = DD_MEM_Alloc(nTempReadSize);
						if(pcTempBuffer == NULL)
						{
							PrintError("%s(%d) : Error> Fail Mem alloc.\n",__FUNCTION__,__LINE__);
							(void)VK_SEM_Release (s_ulHttpSemId);

							PrintExit;
							return VFIO_ERROR_SUCCESS;
						}
						else
						{
							VK_MEM_Memset(pcTempBuffer, 0x0, nTempReadSize);
						}

						(void)P_STREAM_HTTP_ReadforRealtimeBuffer(tHandle, pcTempBuffer, nTempReadSize, 1, &nReadSize);

						if(pcTempBuffer != NULL)
						{
							DD_MEM_Free(pcTempBuffer);
							pcTempBuffer = NULL;
						}
					}
#endif

					if(nTempReadSize != nReadSize)
					{
						PrintDebug("######### [%s] Critical Section: Fail Read(nReadSize=%u, nTempReadSize=%u) #######\n", __FUNCTION__, nReadSize, nTempReadSize);

						(void)VK_SEM_Release (s_ulHttpSemId);

						PrintExit;
						return VFIO_ERROR_FAIL;

					}
					else
					{
						if(s_tMP4MoovInstance.pcData != NULL)
						{
							s_tMP4MoovInstance.lBuffer_CursorPosition += nReadSize;
							s_nMoovBuffer_TotalUsedMemory += nReadSize;
							PrintDebug("######### [%s] Success Read(lBuffer_CursorPosition=%"PRId64", s_nMoovBuffer_TotalUsedMemory=%u) #######\n",
								__FUNCTION__, s_tMP4MoovInstance.lBuffer_CursorPosition, s_nMoovBuffer_TotalUsedMemory);
						}
					}
				}
			}

			s_tMP4MoovInstance.lBuffer_ReadPosition = s_tMP4MoovInstance.lBuffer_CursorPosition;

			(void)VK_SEM_Release (s_ulHttpSemId);

			PrintExit;
			return VFIO_ERROR_SUCCESS;
		}
	}
#endif
#endif
#if defined(USE_MOOVBOX_CACHING)
	if(stMoovBox.eState == MOOVBOX_CACHING_STATE_CACHED)
	{
		if(((off_t)stMoovBox.ulStartOffset <= offset) &&
			((off_t)(stMoovBox.ulStartOffset + stMoovBox.unDataLength)>offset))
		{
			stMoovBox.lCursorPos = offset;
			(void)VK_SEM_Release (s_ulHttpSemId);
			return VFIO_ERROR_SUCCESS;
		}
	}
#endif

	{
		HINT32 nFailCounter = 0;
#if REAL_TIME_MULTISOCKET_SUPPORT
		while(DI_IPT_SUCCESS != STREAM_HTTP_Seek ( ptHttpAccess, offset ))
#else

#if defined(USE_RAMBUFFERING_MOOVBOX_CACHING_SUPPORT)
		if(stMoovBox.eState == MOOVBOX_CACHING_STATE_CACHED)
		{
			if(((off_t)(stMoovBox.unDataLength + stMoovBox.ulStartOffset) >= offset )
				&& (offset >= (off_t)stMoovBox.ulStartOffset))
			{
				//PrintDebug ("Seek\n");
				stMoovBox.lCursorPos = offset;
				(void)VK_SEM_Release (s_ulHttpSemId);
				PrintExit;
				return VFIO_ERROR_SUCCESS;
			}
		}
#endif
		while(DI_IPT_SUCCESS != STREAM_HTTP_Seek ( &stHttpAccess, offset ))
#endif
		{
			nFailCounter++;
			PrintError ("%s(%d) Error> STREAM_HTTP_Seek fail Retrying...\n",__FUNCTION__,__LINE__);

			if(nFailCounter > DI_IPTUNER_HTTP_SEEK_RETRY_COUNT)
			{
				PrintError ("%s(%d) Error> STREAM_HTTP_Seek fail count exceed threadsholuld. Return Error...\n",__FUNCTION__,__LINE__);
				(void)VK_SEM_Release (s_ulHttpSemId);
				PrintExit;
				return -1;
			}
		}

#if defined(USE_RAMBUFFERING_MOOVBOX_CACHING_SUPPORT) || defined(USE_MOOVBOX_CACHING)
		if(stMoovBox.eState == MOOVBOX_CACHING_STATE_CACHED)
		{
			stMoovBox.lCursorPos = offset;
		}
#endif

#if REAL_TIME_MULTISOCKET_SUPPORT
#if REAL_TIME_INDEXBUFFER_SUPPORT
		if(s_tMP4MoovInstance.pcData != NULL)
		{
			s_tMP4MoovInstance.lBuffer_ReadPosition = offset;
			s_tMP4MoovInstance.lBuffer_CursorPosition = offset;
		}
#endif
#endif
	}

	(void)VK_SEM_Release (s_ulHttpSemId);
	PrintExit;
	return VFIO_ERROR_SUCCESS;
}

/** */
int DI_STREAM_HTTP_Tell (VFIO_Handle_t tHandle, off_t *plCurOffset)
{
	HINT32 nRet = VFIO_ERROR_SUCCESS;
#if REAL_TIME_MULTISOCKET_SUPPORT
	STREAM_ACCESS_t *ptHttpAccess=NULL;
#endif

	UNUSED_PARAM (tHandle);

//	PrintEnter;

#if REAL_TIME_MULTISOCKET_SUPPORT
	if(tHandle.pDnpUserData != NULL)
	{
		ptHttpAccess = (STREAM_ACCESS_t *)tHandle.pDnpUserData;
	}
	else
	{
		PrintError ("%s(%d) Error> tHandle->pDnpUserData is null \n",__FUNCTION__,__LINE__);
		PrintExit;
		return VFIO_ERROR_IN_DEVICE;
	}

	if((unsigned int)s_tMultiHandle.DrmHandle == tHandle.tFile.callback.unHandle)
	{
		*plCurOffset = ptHttpAccess->info.i_pos;
	}
	else if((unsigned int)s_tMultiHandle.PlayerHandle == tHandle.tFile.callback.unHandle)
	{
		*plCurOffset = ptHttpAccess->info.i_pos;
	}
	else if((unsigned int)s_tMultiHandle.IndexHandle == tHandle.tFile.callback.unHandle)
	{
#if REAL_TIME_INDEXBUFFER_SUPPORT
		if(s_tMP4MoovInstance.pcData != NULL)
		{
			*plCurOffset = s_tMP4MoovInstance.lBuffer_ReadPosition;
		}
		else
		{
#if defined(USE_MOOVBOX_CACHING)
			*plCurOffset = stMoovBox.lCursorPos;
#else
			*plCurOffset = ptHttpAccess->info.i_pos;
#endif
		}
#else
		*plCurOffset = ptHttpAccess->info.i_pos;
#endif
	}
	else
	{
#if REAL_TIME_INDEXBUFFER_SUPPORT
		if((s_tMultiHandle.ProbeHandle != 0) && (s_tMP4MoovInstance.pcData != NULL))
		{
			*plCurOffset = s_tMP4MoovInstance.lBuffer_ReadPosition;
		}
		else
		{
#if defined(USE_MOOVBOX_CACHING)
			*plCurOffset = stMoovBox.lCursorPos;
#else
			*plCurOffset = ptHttpAccess->info.i_pos;
#endif
		}
#else
		*plCurOffset = ptHttpAccess->info.i_pos;
#endif
	}
#else
	*plCurOffset = stHttpAccess.info.i_pos;
#if defined(USE_RAMBUFFERING_MOOVBOX_CACHING_SUPPORT)
	if(stMoovBox.eState == MOOVBOX_CACHING_STATE_CACHED)
	{
		*plCurOffset = stMoovBox.lCursorPos;
	}
#endif
#endif
//	PrintDebug ("%s(%d) : i_pos (%"PRId64") nRet(%d)\n",__FUNCTION__,__LINE__,*plCurOffset, nRet);
//	PrintExit;
	return nRet;
}

/** if EOF is true, must be return -1 like feof*/
int DI_STREAM_HTTP_IsEOF (VFIO_Handle_t tHandle)
{
	HINT32 nRet = VFIO_ERROR_SUCCESS;
#if REAL_TIME_MULTISOCKET_SUPPORT
	STREAM_ACCESS_t *ptHttpAccess=NULL;
#endif

	UNUSED_PARAM (tHandle);
	PrintEnter;

#if REAL_TIME_MULTISOCKET_SUPPORT
	if(tHandle.pDnpUserData != NULL)
	{
		ptHttpAccess = (STREAM_ACCESS_t *)tHandle.pDnpUserData;
	}
	else
	{
		PrintError ("%s(%d) Error> tHandle->pDnpUserData is null \n",__FUNCTION__,__LINE__);
		PrintExit;
		return VFIO_ERROR_IN_DEVICE;
	}

	if((unsigned int)s_tMultiHandle.DrmHandle == tHandle.tFile.callback.unHandle)
	{
	    if(TRUE == ptHttpAccess->info.b_eof)
		{
			nRet = -1; /* must be -1 like feof*/
		}
	}
	else if((unsigned int)s_tMultiHandle.PlayerHandle == tHandle.tFile.callback.unHandle)
	{
	    if(TRUE == ptHttpAccess->info.b_eof)
		{
			nRet = -1; /* must be -1 like feof*/
		}
	}
	else if((unsigned int)s_tMultiHandle.IndexHandle == tHandle.tFile.callback.unHandle)
	{
	    if(TRUE == ptHttpAccess->info.b_eof)
		{
			nRet = -1; /* must be -1 like feof*/
		}
	}
	else
	{
	    if(TRUE == ptHttpAccess->info.b_eof)
		{
			nRet = -1; /* must be -1 like feof*/
		}
	}
#else
    if(TRUE == stHttpAccess.info.b_eof)
	{
		nRet = -1; /* must be -1 like feof*/
	}
#endif
	PrintExit;
	return nRet;
}

/* Open From Nexus */
int DI_STREAM_HTTP_OpenFromNexus (const HINT8 *path, const unsigned int flags, VFIO_Handle_t *ptHandle)
{
	HINT32 nRet = DI_IPT_SUCCESS;
	HTTP_ACCESS_SYS_t *pstHttpSys=NULL;
#if STREAM_BUFFERING
	DI_IPTUNER_Msg_t s_Msg;
#endif
#if SUPPORT_COOKIE
	char *psCookie = NULL;
	URL_t tUrl;
#endif
#if REAL_TIME_MULTISOCKET_SUPPORT
#if REAL_TIME_CHUNKBUFFER_SUPPORT
	HUINT32 unIndex=0;
#endif
	STREAM_ACCESS_t *ptHttpAccess=NULL;
#endif

	UNUSED_PARAM (flags);

	PrintEnter;

#if REAL_TIME_MULTISOCKET_SUPPORT
	ptHttpAccess = DD_MEM_Alloc(sizeof(STREAM_ACCESS_t));
	if(ptHttpAccess == NULL)
	{
		PrintError("%s(%d) : Error> Fail Mem alloc.\n",__FUNCTION__,__LINE__);
		ptHandle->ucAvailable = 0;

		nRet = VFIO_ERROR_IN_DEVICE;
		PrintExit;
		return nRet;
	}
	else
	{
		VK_MEM_Memset(ptHttpAccess, 0x0, sizeof(STREAM_ACCESS_t));
	}
#endif

#if SUPPORT_COOKIE
	VK_MEM_Memset(&tUrl, 0x0, sizeof(tUrl));
	UTIL_UrlParse(&tUrl, (const char *)path, 0);
	if(tUrl.psz_cookie != NULL)
	{
		psCookie = VK_StrDup(tUrl.psz_cookie);
	}
	UTIL_UrlClean(&tUrl);
#endif

#if STREAM_BUFFERING
	/* Stream Buffering 시 Probe 를 위한 Open 과 Playback을 위한 Open 을 구분하기 위한 용도 */
	gOpenStatus = NEXUS_OPEN;
	DNP_USER_DATA_t * pDnpUserData = DD_MEM_Alloc(sizeof(DNP_USER_DATA_t));
	sOpenFromNexusCounter ++;
	pDnpUserData->nPurpose = sOpenFromNexusCounter;
	pDnpUserData->indexer_last_seek_cursor = 0;
	pDnpUserData->p_align_buffer = NULL;
	pDnpUserData->align_buffer_start_index = -1;
	pDnpUserData->align_buffer_end_index = -1;
	ptHandle->pDnpUserData = pDnpUserData;

	/* open for indexer */
	if(sOpenFromNexusCounter == 2)
	{
		PrintDebug("openFromNexus for Indexer\n\n");
		/** psz_path must be free */
		stHttpAccessForIndexer.psz_path = VK_StrDup ((const char *)path);

#if SUPPORT_COOKIE
		if(psCookie != NULL)
		{
			stHttpAccessForIndexer.psz_cookie = psCookie;
		}
#endif
		nRet = STREAM_HTTP_Open (&stHttpAccessForIndexer);/* indexer connection */
		if(nRet != DI_IPT_SUCCESS)
		{
			PrintError("%s (%d) : Error> STREAM_HTTP_Open Fail.\n",__FUNCTION__,__LINE__);
			ptHandle->ucAvailable = 0;

			if(stHttpAccessForIndexer.psz_path != NULL)
			{
				DD_MEM_Free(stHttpAccessForIndexer.psz_path);
				stHttpAccessForIndexer.psz_path =NULL;
			}
			/* else { continue; } */
#if SUPPORT_COOKIE
			if(stHttpAccessForIndexer.psz_cookie != NULL)
			{
				DD_MEM_Free(stHttpAccessForIndexer.psz_cookie);
				stHttpAccessForIndexer.psz_cookie =NULL;
			}
			/* else { continue; } */
#endif
			nRet = VFIO_ERROR_IN_DEVICE;
			PrintExit;
			return nRet;
		}
		else
		{
			pstHttpSys = (HTTP_ACCESS_SYS_t *)stHttpAccessForIndexer.p_sys;
			ptHandle->tFile.callback.unHandle = (HUINT32) pstHttpSys->fd;
			ptHandle->ucAvailable = 1;

			PrintData ("%s(%d) : callback handle=0x%x\n",__FUNCTION__,__LINE__, ptHandle->tFile.callback.unHandle);
		}
		return VFIO_ERROR_SUCCESS;
	}

	/* open for player */
	if(sOpenFromNexusCounter != 2)
	{
		PrintDebug("openFromNexus for Player (opencounter=%d)\n", sOpenFromNexusCounter);
		if(sOpenFromNexusCounter != 1)
		{
			PrintError("May be another player thread is created\n");
		}
#if INTEGRITY_STREAM_CHECK_WITH_COMPARING_LOCAL_FILE
		fd = open ( INTEGRITY_STREAM_CHECK_WITH_COMPARING_LOCAL_FILE_NAME , O_RDONLY, 0666);
#endif
	}

	/** psz_path must be free */
	stHttpAccess.psz_path = VK_StrDup ((const char *)path);

#if SUPPORT_COOKIE
	if(psCookie != NULL)
	{
		stHttpAccess.psz_cookie = psCookie;
	}
#endif
	nRet = STREAM_HTTP_Open (&stHttpAccess);
	if(nRet != DI_IPT_SUCCESS)
	{
		PrintError("%s (%d) : Error> STREAM_HTTP_Open Fail.\n",__FUNCTION__,__LINE__);
		ptHandle->ucAvailable = 0;
		nRet = VFIO_ERROR_IN_DEVICE;

		if(stHttpAccess.psz_path != NULL)
		{
			DD_MEM_Free(stHttpAccess.psz_path);
			stHttpAccess.psz_path =NULL;
		}
		/* else { continue; } */
#if SUPPORT_COOKIE
		if(stHttpAccess.psz_cookie != NULL)
		{
			DD_MEM_Free(stHttpAccess.psz_cookie);
			stHttpAccess.psz_cookie =NULL;
		}
		/* else { continue; } */
#endif
		PrintExit;
		return nRet;
	}
	else
	{
		pstHttpSys = (HTTP_ACCESS_SYS_t *)stHttpAccess.p_sys;
		ptHandle->tFile.callback.unHandle = (HUINT32) pstHttpSys->fd;
		ptHandle->ucAvailable = 1;

		PrintData ("%s(%d) : callback handle=0x%x\n",__FUNCTION__,__LINE__, ptHandle->tFile.callback.unHandle);
	}

	nRet = VK_SEM_Create(&s_ulHttpSemId,"httpSema", DEFAULT_SUSPEND_TYPE);
	if( nRet != VK_OK )
	{
		PrintError("%s(%d) Error(0x%x) \n\r",__FUNCTION__,__LINE__, nRet);

		(void)STREAM_HTTP_Close (&stHttpAccess);

		if(stHttpAccess.psz_path != NULL)
		{
			DD_MEM_Free(stHttpAccess.psz_path);
			stHttpAccess.psz_path =NULL;
		}
		/* else { continue; } */
#if SUPPORT_COOKIE
		if(stHttpAccess.psz_cookie != NULL)
		{
			DD_MEM_Free(stHttpAccess.psz_cookie);
			stHttpAccess.psz_cookie =NULL;
		}
		/* else { continue; } */
#endif
		PrintExit;
		return VFIO_ERROR_IN_DEVICE;
	}
	//---------------------------------------
#else
#if REAL_TIME_MULTISOCKET_SUPPORT
	/* Stream Buffering 시 Probe 를 위한 Open 과 Playback을 위한 Open 을 구분하기 위한 용도 */
	if(s_tMultiHandle.PlayerHandle == 0) 	/* open for Player */
	{
		PrintDebug("openFromNexus for Player\n\n");
		/* open for player */
		/** psz_path must be free */
		if(pcRedirectionUrl != NULL)
		{
			ptHttpAccess->psz_path = VK_StrDup(pcRedirectionUrl);
		}
		else
		{
			ptHttpAccess->psz_path = VK_StrDup ((char *)path);
		}

#if SUPPORT_COOKIE
		if(psCookie != NULL)
		{
			ptHttpAccess->psz_cookie = psCookie;
		}
#endif

		if(ptHttpAccess->psz_path == NULL)
		{
			PrintError("%s(%d) : Error> Fail Mem alloc.\n",__FUNCTION__,__LINE__);
			nRet = VFIO_ERROR_IN_DEVICE;
		}
		else
		{
			nRet = STREAM_HTTP_Open (ptHttpAccess);
			if(nRet != DI_IPT_SUCCESS)
			{
				PrintError("%s (%d) : Error> STREAM_HTTP_Open Fail.\n",__FUNCTION__,__LINE__);
			}
			else
			{
				pstHttpSys = (HTTP_ACCESS_SYS_t *)ptHttpAccess->p_sys;
				ptHandle->tFile.callback.unHandle = (HUINT32) pstHttpSys->fd;
				s_tMultiHandle.PlayerHandle = (HUINT32) pstHttpSys->fd;
				ptHandle->ucAvailable = 1;

				PrintData ("%s(%d) : callback handle=0x%x\n",__FUNCTION__,__LINE__, ptHandle->tFile.callback.unHandle);

#if REAL_TIME_CHUNKBUFFER_SUPPORT
				VK_MEM_Memset(&tRealtime_Buffer_Chunk, 0x0, sizeof(tRealtime_Buffer_Chunk));

				for(unIndex=0;unIndex<REALTIME_PLAYBUFFER_CHUNK_TOTAL;unIndex++)
				{
					tRealtime_Buffer_Chunk[unIndex].pcData = DD_MEM_Alloc(CHUNK_BUUFER_DEFAULT_SIZE);
					if(tRealtime_Buffer_Chunk[unIndex].pcData == NULL)
					{
						nRet = VFIO_ERROR_IN_DEVICE;
						break;
					}
					else
					{
						VK_MEM_Memset(tRealtime_Buffer_Chunk[unIndex].pcData, 0x0, CHUNK_BUUFER_DEFAULT_SIZE);
					}
				}

				if(VK_strstr(pstHttpSys->psz_mime,"mp4"))
				{
					PrintData ("%s(%d) : psz_mime       = %s \n",__FUNCTION__,__LINE__,pstHttpSys->psz_mime);
					PrintData ("%s(%d) : Not used Chunk Buffer \n",__FUNCTION__,__LINE__);
				}
				else if(s_eStream_type == DI_MEDIA_MPEG_TYPE_ASF)
				{
					VK_MEM_Memset(&s_tWMVInstance, 0x0, sizeof(s_tWMVInstance));
					s_tWMVInstance.lBuffer_CursorPosition = 0;
					s_tWMVInstance.lBuffer_ReadPosition = 0;
					s_tWMVInstance.pcData = NULL;
					s_tWMVInstance.pcData = DD_MEM_Alloc(CHUNK_BUUFER_DEFAULT_SIZE);
				}
				else
				{
					if(s_tMP4MoovInstance.pcData)
					{
						DD_MEM_Free(s_tMP4MoovInstance.pcData);
						s_tMP4MoovInstance.pcData=NULL;
					}
				}
#endif
				s_ullCurrent_PlayCurPosition = 0;

				if(s_ulHttpSemId == 0)
				{
					PrintData ("%s(%d) : sem create \n",__FUNCTION__,__LINE__);
					nRet = VK_SEM_Create(&s_ulHttpSemId,"httpSema", DEFAULT_SUSPEND_TYPE);
					if( nRet != VK_OK )
					{
						PrintError("%s(%d) Error(0x%x) \n\r",__FUNCTION__,__LINE__, nRet);
					}
				}
			}
		}

		if(nRet != VFIO_ERROR_SUCCESS)
		{
#if REAL_TIME_CHUNKBUFFER_SUPPORT
			for(unIndex=0;unIndex<REALTIME_PLAYBUFFER_CHUNK_TOTAL;unIndex++)
			{
				if(tRealtime_Buffer_Chunk[unIndex].pcData != NULL)
				{
					DD_MEM_Free(tRealtime_Buffer_Chunk[unIndex].pcData);
					tRealtime_Buffer_Chunk[unIndex].pcData= NULL;
				}
			}
#endif

			PrintError("%s(%d) : Error> Fail Mem alloc.\n",__FUNCTION__,__LINE__);
			ptHandle->ucAvailable = 0;

			if(ptHttpAccess != NULL)
			{
				(void)STREAM_HTTP_Close (ptHttpAccess);
			}

			if(ptHttpAccess->psz_path != NULL)
			{
				DD_MEM_Free(ptHttpAccess->psz_path);
				ptHttpAccess->psz_path =NULL;
			}
			/* else { continue; } */
#if SUPPORT_COOKIE
			if(ptHttpAccess->psz_cookie != NULL)
			{
				DD_MEM_Free(ptHttpAccess->psz_cookie);
				ptHttpAccess->psz_cookie =NULL;
			}
			/* else { continue; } */
#endif

#if REAL_TIME_CHUNKBUFFER_SUPPORT
			if(s_tWMVInstance.pcData != NULL)
			{
				DD_MEM_Free  (s_tWMVInstance.pcData);
				s_tWMVInstance.pcData =NULL;
			}
			/* else { continue; } */
#endif

			if(ptHttpAccess != NULL)
			{
				DD_MEM_Free(ptHttpAccess);
				ptHttpAccess =NULL;
			}
			/* else { continue; } */

			nRet = VFIO_ERROR_IN_DEVICE;
			PrintExit;
			return nRet;
		}
		else
		{
			ptHandle->pDnpUserData = ptHttpAccess;
		}
		//---------------------------------------
	}
	else if((s_tMultiHandle.PlayerHandle != 0) && (s_tMultiHandle.IndexHandle == 0)) 	/* open for Indexer */
	{
		PrintDebug("openFromNexus for Indexer\n\n");
		/** psz_path must be free */
		if(pcRedirectionUrl != NULL)
		{
			ptHttpAccess->psz_path = VK_StrDup(pcRedirectionUrl);
		}
		else
		{
			ptHttpAccess->psz_path = VK_StrDup ((char *)path);
		}

#if SUPPORT_COOKIE
		if(psCookie != NULL)
		{
			ptHttpAccess->psz_cookie = psCookie;
		}
#endif

		if(ptHttpAccess->psz_path == NULL)
		{
			PrintError("%s(%d) : Error> Fail Mem alloc.\n",__FUNCTION__,__LINE__);
			ptHandle->ucAvailable = 0;
			nRet = VFIO_ERROR_IN_DEVICE;
		}
		else
		{
			nRet = STREAM_HTTP_Open (ptHttpAccess);/* indexer connection */
			if(nRet != DI_IPT_SUCCESS)
			{
				PrintError("%s (%d) : Error> STREAM_HTTP_Open Fail.\n",__FUNCTION__,__LINE__);
				ptHandle->ucAvailable = 0;
			}
		}

		if(nRet != VFIO_ERROR_SUCCESS)
		{
			if(ptHttpAccess->psz_path != NULL)
			{
				DD_MEM_Free(ptHttpAccess->psz_path);
				ptHttpAccess->psz_path =NULL;
			}
			/* else { continue; } */
#if SUPPORT_COOKIE
			if(ptHttpAccess->psz_cookie != NULL)
			{
				DD_MEM_Free(ptHttpAccess->psz_cookie);
				ptHttpAccess->psz_cookie =NULL;
			}
			/* else { continue; } */
#endif

			if(ptHttpAccess != NULL)
			{
				DD_MEM_Free(ptHttpAccess);
				ptHttpAccess =NULL;
			}
			/* else { continue; } */

			nRet = VFIO_ERROR_IN_DEVICE;
			PrintExit;
			return nRet;
		}
		else
		{
			pstHttpSys = (HTTP_ACCESS_SYS_t *)ptHttpAccess->p_sys;
			ptHandle->tFile.callback.unHandle = (HUINT32) pstHttpSys->fd;
			s_tMultiHandle.IndexHandle = (HUINT32) pstHttpSys->fd;
			ptHandle->ucAvailable = 1;

			s_ullCurrent_IndexCurPosition = 0;

			ptHandle->pDnpUserData = ptHttpAccess;
			PrintData ("%s(%d) : callback handle=0x%x\n",__FUNCTION__,__LINE__, ptHandle->tFile.callback.unHandle);
		}

#if REAL_TIME_INDEXBUFFER_SUPPORT
		if(s_tMP4MoovInstance.pcData != NULL)
		{
			s_tMP4MoovInstance.lBuffer_ReadPosition = 0;
		}
#endif
	}
	else
	{
		PrintError("%s (%d) : Invalid Open.\n",__FUNCTION__,__LINE__);
#if SUPPORT_COOKIE
		if(psCookie != NULL)
		{
			DD_MEM_Free(psCookie);
			psCookie = NULL;
		}
#endif
		return VFIO_ERROR_IN_DEVICE;
	}
#else
	/* open for player */
	/** psz_path must be free */
	stHttpAccess.psz_path = VK_StrDup ((const char *)path);

#if SUPPORT_COOKIE
	if(psCookie != NULL)
	{
		stHttpAccess.psz_cookie = psCookie;
	}
#endif
	if(stHttpAccess.psz_path == NULL)
	{
		PrintError("%s(%d) : Error> Fail Mem alloc.\n",__FUNCTION__,__LINE__);
		ptHandle->ucAvailable = 0;

		if(stHttpAccess.psz_path != NULL)
		{
			DD_MEM_Free(stHttpAccess.psz_path);
			stHttpAccess.psz_path =NULL;
		}
		/* else { continue; } */
#if SUPPORT_COOKIE
		if(stHttpAccess.psz_cookie != NULL)
		{
			DD_MEM_Free(stHttpAccess.psz_cookie);
			stHttpAccess.psz_cookie =NULL;
		}
		/* else { continue; } */
#endif
		nRet = VFIO_ERROR_IN_DEVICE;
		PrintExit;
		return nRet;
	}

	nRet = STREAM_HTTP_Open (&stHttpAccess);
	if(nRet != DI_IPT_SUCCESS)
	{
		PrintError("%s (%d) : Error> STREAM_HTTP_Open Fail.\n",__FUNCTION__,__LINE__);
		ptHandle->ucAvailable = 0;

		if(stHttpAccess.psz_path != NULL)
		{
			DD_MEM_Free(stHttpAccess.psz_path);
			stHttpAccess.psz_path =NULL;
		}
		/* else { continue; } */
#if SUPPORT_COOKIE
		if(stHttpAccess.psz_cookie != NULL)
		{
			DD_MEM_Free(stHttpAccess.psz_cookie);
			stHttpAccess.psz_cookie =NULL;
		}
		/* else { continue; } */
#endif
		nRet = VFIO_ERROR_IN_DEVICE;

		if(stHttpAccess.psz_path != NULL)
		{
			DD_MEM_Free  (stHttpAccess.psz_path);
			stHttpAccess.psz_path =NULL;
		}
		/* else { continue; } */

		PrintExit;
		return nRet;
	}
	else
	{
		pstHttpSys = (HTTP_ACCESS_SYS_t *)stHttpAccess.p_sys;
		ptHandle->tFile.callback.unHandle = (HUINT32) pstHttpSys->fd;
		ptHandle->ucAvailable = 1;

		PrintData ("%s(%d) : callback handle=0x%x\n",__FUNCTION__,__LINE__, ptHandle->tFile.callback.unHandle);
	}

	nRet = VK_SEM_Create(&s_ulHttpSemId,"httpSema", DEFAULT_SUSPEND_TYPE);
	if( nRet != VK_OK )
	{
		PrintError("%s(%d) Error(0x%x) \n\r",__FUNCTION__,__LINE__, nRet);

		(void)STREAM_HTTP_Close (&stHttpAccess);

		if(stHttpAccess.psz_path != NULL)
		{
			DD_MEM_Free(stHttpAccess.psz_path);
			stHttpAccess.psz_path =NULL;
		}
		/* else { continue; } */
#if SUPPORT_COOKIE
		if(stHttpAccess.psz_cookie != NULL)
		{
			DD_MEM_Free(stHttpAccess.psz_cookie);
			stHttpAccess.psz_cookie =NULL;
		}
		/* else { continue; } */
#endif

		PrintExit;
		return VFIO_ERROR_IN_DEVICE;
	}
	//---------------------------------------
#endif
#endif

#if STREAM_BUFFERING
	if(g_HttpBuffer != NULL)
	{
		PrintData ("%s (%d) Buffering Start from OpenNexusCallback\n",__FUNCTION__,__LINE__);
		(void) VK_memset (&s_Msg, 0x0, sizeof(s_Msg));
		s_Msg.unMsg = TUNER_STREAM_MSG_HTTP_BUFFERING;
		s_Msg.unParam1 = DI_IPTUNER_STREAM_HTTP;
		(void) VK_MSG_Send ( s_ulStreamMsgQId, (HUINT32 *)&s_Msg, sizeof(DI_IPTUNER_Msg_t) ) ;
	}

	pDnpUserData->p_align_buffer = DD_MEM_Alloc(NEXUS_ALIGN_BUFFER_SIZE);
	if(pDnpUserData->p_align_buffer == NULL)
	{
		PrintError("%s(%d) Error> p_align_buffer Mem alloc error\n", __FUNCTION__, __LINE__);

		if(stHttpAccess.psz_path != NULL)
		{
			DD_MEM_Free(stHttpAccess.psz_path);
			stHttpAccess.psz_path =NULL;
		}
		/* else { continue; } */
#if SUPPORT_COOKIE
		if(stHttpAccess.psz_cookie != NULL)
		{
			DD_MEM_Free(stHttpAccess.psz_cookie);
			stHttpAccess.psz_cookie =NULL;
		}
		/* else { continue; } */
#endif
		PrintExit;
		return VFIO_ERROR_IN_DEVICE;
	}
#endif

	PrintExit;
	return VFIO_ERROR_SUCCESS;
}

/* Seek From Nexus : Long jump 인 경우 어느정도는 block 되어도 된다.
 * 단 Read 함수에서는 block 되면 waiting IO error, 0 return 시 endofStream, -1 return시 errorofstream 이 뜬다.*/
off_t DI_STREAM_HTTP_SeekForNexus (VFIO_Handle_t tHandle, off_t offset, int whence)
{
	HINT32 nRet;
#if REAL_TIME_MULTISOCKET_SUPPORT
	STREAM_ACCESS_t *ptHttpAccess=NULL;
#if REAL_TIME_CHUNKBUFFER_SUPPORT || REAL_TIME_INDEXBUFFER_SUPPORT
	HINT32 tReadSize=0;
	HUINT32 nCurrentReadCount = 0;
	HUINT32 nMaxReadCount = 0;
	HUINT32 nIndex=0;
	HUINT8 bUseChunkBuffer=FALSE;
	HUINT32 nTempReadSize = 0;
	HINT64 lBlockSize = 0;
	HINT8 *pcTempBuffer=NULL;
#endif
#endif

	UNUSED_PARAM (whence);

	/* warning: unused variable */
	nRet = 0;


	/* At, iCordHD+, caution to warning: unused parameter 'tHandle' */
	/* Don't use UNUSED_PARAM (tHandle);                            */
	/* Just post debug message for removing build warn              */


	(void)VK_SEM_Get(s_ulHttpSemId);

	PrintEnter;

#if REAL_TIME_MULTISOCKET_SUPPORT
	if(tHandle.pDnpUserData != NULL)
	{
		ptHttpAccess = (STREAM_ACCESS_t *)tHandle.pDnpUserData;
	}
	else
	{
		PrintError ("%s(%d) Error> tHandle->pDnpUserData is null \n",__FUNCTION__,__LINE__);
		PrintExit;
		(void)VK_SEM_Release (s_ulHttpSemId);
		return VFIO_ERROR_IN_DEVICE;
	}

	if(ptHttpAccess->info.i_pos == offset)
	{
#if defined(USE_MOOVBOX_CACHING)
		stMoovBox.lCursorPos = offset;
#endif
		s_ullCurrent_PlayCurPosition = offset;
		(void)VK_SEM_Release (s_ulHttpSemId);
		PrintExit;
		return offset;
	}
#endif /* REAL_TIME_MULTISOCKET_SUPPORT */

#if STREAM_BUFFERING
	/* null check */
	if(tHandle.pDnpUserData == NULL)
	{
		PrintError("pDnpUserData is NULL\n");
		(void)VK_SEM_Release (s_ulHttpSemId);
		return -1;
	}

	/**/
	/* Realtime SeekForNexus */
	/**/
	if(g_HttpBuffer == NULL)
	{
		PrintDebug("Realtime seek\n");
#if defined(USE_RAMBUFFERING_MOOVBOX_CACHING_SUPPORT)
		if(stMoovBox.eState == MOOVBOX_CACHING_STATE_CACHED)
		{
			if(((off_t)(stMoovBox.unDataLength + stMoovBox.ulStartOffset) >= offset )
				&& (offset >= (off_t)stMoovBox.ulStartOffset))
			{
				//PrintDebug ("SeekNexus\n");
				stMoovBox.lCursorPos = offset;
				(void)VK_SEM_Release (s_ulHttpSemId);
				PrintExit;
				return offset;
			}
		}
#endif
		{
			if(stHttpAccess.info.i_pos != offset)
			{
			PrintData("%s(%d) : handle(0x%x) offset=%"PRId64", whence=%d\n",__FUNCTION__,__LINE__, tHandle.tFile.callback.unHandle, offset, whence);
			{
				HUINT32 nNetworkRetryCount = 0;
				while(DI_IPT_SUCCESS != STREAM_HTTP_Seek ( &stHttpAccess, offset ))
				{
					nNetworkRetryCount++;
					PrintError ("%s(%d) Error> STREAM_HTTP_Seek fail Retrying...\n",__FUNCTION__,__LINE__);
					VK_TASK_Sleep(100);
					if(nNetworkRetryCount > DI_IPTUNER_DNP_RETRY_COUNT_ON_NET_ERROR)
					{
						PrintError("Send Failed code To Buffering Reader(Player,Indexer)\n");
						s_NetworkError = TRUE;
						offset = (off_t)-1;
						break;
					}
				}
			}
		}
		}

#if defined(USE_RAMBUFFERING_MOOVBOX_CACHING_SUPPORT)
		if(stMoovBox.eState == MOOVBOX_CACHING_STATE_CACHED)
		{
			stMoovBox.lCursorPos = offset;
		}
#endif
		//	PrintExit;
		(void)VK_SEM_Release (s_ulHttpSemId);
		return offset;

	}

	/**/
	/* Indexer SeekForNexus */
	/**/
	if(((DNP_USER_DATA_t *)tHandle.pDnpUserData)->nPurpose == 2)
	{
//		PrintDebug ("== Indexer SeekForNexus\n");
#if defined(USE_RAMBUFFERING_MOOVBOX_CACHING_SUPPORT)
		if(stMoovBox.eState == MOOVBOX_CACHING_STATE_CACHED)
		{
			if(((off_t)(stMoovBox.unDataLength + stMoovBox.ulStartOffset) >= offset )
				&& (offset >= (off_t)stMoovBox.ulStartOffset))
			{
				//PrintDebug ("SeekNexus\n");
				stMoovBox.lCursorPos = offset;
				(void)VK_SEM_Release (s_ulHttpSemId);
				PrintExit;
				return offset;
			}
		}
#endif
		{
			HUINT32 nNetworkRetryCount = 0;
			if(stHttpAccessForIndexer.info.i_pos != offset)
			{
			while(DI_IPT_SUCCESS != STREAM_HTTP_Seek ( &stHttpAccessForIndexer, offset ))
			{
				nNetworkRetryCount++;
				PrintError ("%s(%d) Error> STREAM_HTTP_Seek fail Retrying...\n",__FUNCTION__,__LINE__);
				VK_TASK_Sleep(100);
				if(nNetworkRetryCount > DI_IPTUNER_DNP_RETRY_COUNT_ON_NET_ERROR)
				{
					PrintError("Send Failed code To Buffering Reader(Player,Indexer)\n");
					s_NetworkError = TRUE;
					offset = (off_t)-1;
					break;
				}
			}
		}
		}
#if defined(USE_RAMBUFFERING_MOOVBOX_CACHING_SUPPORT)
		if(stMoovBox.eState == MOOVBOX_CACHING_STATE_CACHED)
		{
			stMoovBox.lCursorPos = offset;
		}
#endif
		//	PrintExit;
		(void)VK_SEM_Release (s_ulHttpSemId);
		return offset;
	}


	/**/
	/* Player SeekForNexus */
	/**/
	{
#if 0
		if(sOffsetStart == stHttpAccess.info.i_size)
		{
			((DNP_USER_DATA_t *)tHandle.pDnpUserData)->player_last_seek_cursor = offset;
			(void)VK_SEM_Release (s_ulHttpSemId);
			return offset;
		}
#endif

		PrintDebug("Stream Buffering seek\n");

		/* calc absolute offset */
		if( SEEK_CUR == whence )
		{
			//PrintError("== Seek CUR\n");
			((DNP_USER_DATA_t *)tHandle.pDnpUserData)->player_last_seek_cursor += offset;
		}
		else if( SEEK_SET == whence )
		{
			//PrintError("== Seek SET\n");
			((DNP_USER_DATA_t *)tHandle.pDnpUserData)->player_last_seek_cursor = offset;
		}
		else if( SEEK_END == whence )
		{
			((DNP_USER_DATA_t *)tHandle.pDnpUserData)->player_last_seek_cursor = s_nFileLength - 1;
		}
		else
		{
			PrintError("%s(%d) Invalid whence Parameter %d\n", __FUNCTION__, __LINE__, whence);
			((DNP_USER_DATA_t *)tHandle.pDnpUserData)->player_last_seek_cursor = offset;
		}

		/* bound check */
		if(((DNP_USER_DATA_t *)tHandle.pDnpUserData)->player_last_seek_cursor > s_nFileLength - 1)
		{
			((DNP_USER_DATA_t *)tHandle.pDnpUserData)->player_last_seek_cursor = s_nFileLength - 1;
		}

#if INTEGRITY_STREAM_CHECK_WITH_COMPARING_LOCAL_FILE
		off_t x = lseek(fd, offset, whence);
		if(x != ((DNP_USER_DATA_t *)tHandle.pDnpUserData)->player_last_seek_cursor)
		{
			PrintError("========\n========\nlseek is not matched!!\n");
		}
#endif


		(void)VK_SEM_Release (s_ulHttpSemId);
		return ((DNP_USER_DATA_t *)tHandle.pDnpUserData)->player_last_seek_cursor;


	}

#else /* STREAM_BUFFERING */
#if REAL_TIME_MULTISOCKET_SUPPORT
	if((unsigned int)s_tMultiHandle.DrmHandle == tHandle.tFile.callback.unHandle)
	{
		if(ptHttpAccess->info.i_pos == offset)
		{
			PrintExit;
			(void)VK_SEM_Release (s_ulHttpSemId);
			return offset;
		}

		PrintDebug("Drm seek\n");
		nRet = STREAM_HTTP_Seek (ptHttpAccess, offset );
		if(nRet != DI_IPT_SUCCESS)
		{
			PrintError ("%s(%d) Error> STREAM_HTTP_Seek fail \n",__FUNCTION__,__LINE__);
			PrintExit;
			(void)VK_SEM_Release (s_ulHttpSemId);
			return -1;
		}
		else
		{
			PrintExit;
			(void)VK_SEM_Release (s_ulHttpSemId);
			return offset;
		}
	}
	else if((unsigned int)s_tMultiHandle.PlayerHandle == tHandle.tFile.callback.unHandle)
	{
		if(ptHttpAccess->info.i_pos == offset)
		{
			s_ullCurrent_PlayCurPosition = offset;
			(void)VK_SEM_Release (s_ulHttpSemId);
			PrintExit;
			return offset;
		}

#if REAL_TIME_CHUNKBUFFER_SUPPORT
		if((s_tWMVInstance.pcData != NULL) && (offset >= s_tWMVInstance.lBuffer_CursorPosition)  && (offset < s_tWMVInstance.lBuffer_ReadPosition))
		{
			PrintDebug ("%s(%d) : i_pos=%"PRId64", s_ullCurrent_PlayCurPosition=%"PRId64" \n",__FUNCTION__,__LINE__, ptHttpAccess->info.i_pos, s_ullCurrent_PlayCurPosition);
			PrintDebug ("%s(%d) : offset=%"PRId64" \n",__FUNCTION__,__LINE__, offset);

			s_ullCurrent_PlayCurPosition = offset;
			(void)VK_SEM_Release (s_ulHttpSemId);
			PrintExit;
			return offset;
		}

		if(s_tMP4MoovInstance.pcData != NULL)
		{
			bUseChunkBuffer = FALSE;

			for(nIndex = 0; nIndex < REALTIME_PLAYBUFFER_CHUNK_TOTAL; nIndex++)
			{
				if(offset == tRealtime_Buffer_Chunk[nIndex].ulStartindex)
				{
					bUseChunkBuffer = TRUE;
					break;
				}
			}

			if((offset != 0) && (bUseChunkBuffer == TRUE))
			{
				PrintDebug("Not Move(Because Back Buffer)\n");
				PrintDebug("#####offset=%"PRId64", ptHttpAccess->info.i_pos=%"PRId64"#####\n", offset,ptHttpAccess->info.i_pos );
				s_ullCurrent_PlayCurPosition = offset;
				(void)VK_SEM_Release (s_ulHttpSemId);
				PrintExit;
				return offset;
			}

			if(((ptHttpAccess->info.i_pos+BROADCOM_PLAYBACK_READ_BLOCK*30) > offset) && (ptHttpAccess->info.i_pos < offset))
			{
				PrintData ("%s(%d) : stHttpAccess.info.i_pos=%"PRId64", offset=%"PRId64", test=%"PRId64"\n",__FUNCTION__,__LINE__, ptHttpAccess->info.i_pos, offset, (offset-ptHttpAccess->info.i_pos));
				lBlockSize = BROADCOM_PLAYBACK_READ_BLOCK;
				nMaxReadCount = (offset-ptHttpAccess->info.i_pos)/lBlockSize;
				nTempReadSize = (offset-ptHttpAccess->info.i_pos-lBlockSize*nMaxReadCount);
				if(nTempReadSize > 0)
				{
					nMaxReadCount += 1;
				}

				if(nTempReadSize == 0)
				{
					nTempReadSize = BROADCOM_PLAYBACK_READ_BLOCK;
				}

				s_ullCurrent_PlayCurPosition = ptHttpAccess->info.i_pos;

				PrintData ("%s(%d) : nMaxReadCount=%u, lBlockSize=%"PRId64", nTempReadSize=%u\n",
					__FUNCTION__,__LINE__, nMaxReadCount, lBlockSize, nTempReadSize);
			}

			if(nMaxReadCount > 0)
			{
				for(nCurrentReadCount = 0; nCurrentReadCount < nMaxReadCount; nCurrentReadCount++)
				{
					pcTempBuffer = DD_MEM_Alloc(CHUNK_BUUFER_DEFAULT_SIZE);
					if(pcTempBuffer == NULL)
					{
						PrintError("%s(%d) : Error> Fail Mem alloc.\n",__FUNCTION__,__LINE__);
						(void)VK_SEM_Release (s_ulHttpSemId);

						PrintExit;
						return VFIO_ERROR_SUCCESS;
					}
					else
					{
						VK_MEM_Memset(pcTempBuffer, 0x0, CHUNK_BUUFER_DEFAULT_SIZE);

						if((nMaxReadCount-1) != nCurrentReadCount)
						{
							(void)P_STREAM_HTTP_ReadforRealtimeBuffer(tHandle, pcTempBuffer, BROADCOM_PLAYBACK_READ_BLOCK, 1, (size_t *)&tReadSize);
							if(BROADCOM_PLAYBACK_READ_BLOCK != tReadSize)
							{
								PrintDebug("######### [%s](%d) Critical Section: Fail Read #######\n", __FUNCTION__,__LINE__);
								PrintDebug("%s(%d) Player seek\n",__FUNCTION__,__LINE__);
								nRet = STREAM_HTTP_Seek ( ptHttpAccess, offset );
								if(nRet != DI_IPT_SUCCESS)
								{
									PrintError ("%s(%d) Error> STREAM_HTTP_Seek fail \n",__FUNCTION__,__LINE__);

									if(pcTempBuffer != NULL)
									{
										DD_MEM_Free(pcTempBuffer);
										pcTempBuffer = NULL;
									}

									(void)VK_SEM_Release (s_ulHttpSemId);
									PrintExit;
									return VFIO_ERROR_FAIL;
								}
								else
								{
#if REAL_TIME_CHUNKBUFFER_SUPPORT
									s_ullCurrent_PlayCurPosition = offset;
#endif
									if(pcTempBuffer != NULL)
									{
										DD_MEM_Free(pcTempBuffer);
										pcTempBuffer = NULL;
									}

									break;
								}
							}
						}
						else
						{
							(void)P_STREAM_HTTP_ReadforRealtimeBuffer(tHandle, pcTempBuffer, nTempReadSize, 1, (size_t *)&tReadSize);
							if(nTempReadSize != (HUINT32)tReadSize)
							{
								PrintDebug("######### [%s] Critical Section: Fail Read(nReadSize=%u, nTempReadSize=%u) #######\n", __FUNCTION__, tReadSize, nTempReadSize);
								PrintDebug("%s(%d) Player seek\n",__FUNCTION__,__LINE__);
								nRet = STREAM_HTTP_Seek ( ptHttpAccess, offset );
								if(nRet != DI_IPT_SUCCESS)
								{
									PrintError ("%s(%d) Error> STREAM_HTTP_Seek fail \n",__FUNCTION__,__LINE__);

									if(pcTempBuffer != NULL)
									{
										DD_MEM_Free(pcTempBuffer);
										pcTempBuffer = NULL;
									}

									(void)VK_SEM_Release (s_ulHttpSemId);
									PrintExit;
									return VFIO_ERROR_FAIL;
								}
								else
								{
#if REAL_TIME_CHUNKBUFFER_SUPPORT
									s_ullCurrent_PlayCurPosition = offset;
#endif
									if(pcTempBuffer != NULL)
									{
										DD_MEM_Free(pcTempBuffer);
										pcTempBuffer = NULL;
									}

									break;
								}
							}
						}

						if(pcTempBuffer != NULL)
						{
							DD_MEM_Free(pcTempBuffer);
							pcTempBuffer = NULL;
						}
					}
				}

				(void)VK_SEM_Release (s_ulHttpSemId);

				PrintExit;
				return offset;
			}
		}
#endif /* REAL_TIME_CHUNKBUFFER_SUPPORT */

		PrintDebug("Player seek\n");
		nRet = STREAM_HTTP_Seek ( ptHttpAccess, offset );
		if(nRet != DI_IPT_SUCCESS)
		{
			PrintError ("%s(%d) Error> STREAM_HTTP_Seek fail \n",__FUNCTION__,__LINE__);
			PrintExit;
			(void)VK_SEM_Release (s_ulHttpSemId);
			return -1;
		}
		else
		{
			PrintExit;
			(void)VK_SEM_Release (s_ulHttpSemId);
#if REAL_TIME_CHUNKBUFFER_SUPPORT
			s_ullCurrent_PlayCurPosition = offset;
#endif
			return offset;
		}
	}
	else if((unsigned int)s_tMultiHandle.IndexHandle == tHandle.tFile.callback.unHandle)
	{
#if defined(USE_MOOVBOX_CACHING)
		if(stMoovBox.eState == MOOVBOX_CACHING_STATE_CACHED)
		{
			stMoovBox.lCursorPos = offset;
#if 0
			if(((off_t)stMoovBox.ulStartOffset <= offset) &&
				((off_t)(stMoovBox.ulStartOffset + stMoovBox.unDataLength)>offset))
			{
				(void)VK_SEM_Release (s_ulHttpSemId);
				return offset;
			}

#endif
		}
#endif

		if(ptHttpAccess->info.i_pos == offset)
		{
			PrintExit;
			(void)VK_SEM_Release (s_ulHttpSemId);
			return offset;
		}

#if REAL_TIME_INDEXBUFFER_SUPPORT
		if((s_tMP4MoovInstance.pcData != NULL) &&(s_tMP4MoovInstance.lBuffer_CursorPosition > offset))
		{
			s_tMP4MoovInstance.lBuffer_ReadPosition = offset;
			PrintData ("%s(%d) : lBuffer_ReadPosition=%"PRId64", offset=%"PRId64"\n",__FUNCTION__,__LINE__, s_tMP4MoovInstance.lBuffer_ReadPosition, offset);
			PrintExit;
			(void)VK_SEM_Release (s_ulHttpSemId);
			return s_tMP4MoovInstance.lBuffer_ReadPosition;
		}

		if(s_tMP4MoovInstance.pcData != NULL)
		{
			for(nIndex=0;nIndex<REALTIME_PROBEBUFFER_CHUNK_TOTAL;nIndex++)
			{
				if((tRealtime_ProbeBuffer_Chunk[nIndex].ulStartindex == offset) && (tRealtime_ProbeBuffer_Chunk[nIndex].unData_Length != 0))
				{
					PrintDebug("##### [%s] Return for Probe Buffer Read(%d) #####\n", __FUNCTION__, nIndex);
					s_ullCurrent_IndexCurPosition = offset;
					PrintExit;
					(void)VK_SEM_Release (s_ulHttpSemId);
					return offset;
				}
			}
		}
#endif

		PrintDebug("Index seek\n");
		nRet = STREAM_HTTP_Seek (ptHttpAccess, offset );
		if(nRet != DI_IPT_SUCCESS)
		{
			PrintError ("%s(%d) Error> STREAM_HTTP_Seek fail \n",__FUNCTION__,__LINE__);
			PrintExit;
			(void)VK_SEM_Release (s_ulHttpSemId);
			return -1;
		}
		else
		{
#if REAL_TIME_MULTISOCKET_SUPPORT
#if REAL_TIME_INDEXBUFFER_SUPPORT
			s_ullCurrent_IndexCurPosition = offset;

			if(s_tMP4MoovInstance.pcData != NULL)
			{
				s_tMP4MoovInstance.lBuffer_ReadPosition = offset;
			}
#endif
#endif
			PrintExit;
			(void)VK_SEM_Release (s_ulHttpSemId);
			return offset;
		}
	}
	else
	{
		PrintError ("%s(%d) Error> STREAM_HTTP_Seek fail \n",__FUNCTION__,__LINE__);
		PrintExit;
		(void)VK_SEM_Release (s_ulHttpSemId);
		return -1;
	}
#else /* REAL_TIME_MULTISOCKET_SUPPORT */
    #if 1 /* Removing warning: unused parameter 'tHandle' */
		/* Don't use UNUSED_PARAM (tHandle); --> Can cause another error */
		/* Just use debug message posting for avoiding build warn */
		if(tHandle.pDnpUserData == NULL) {
			PrintData("%s(%d) tHandle->pDnpUserData is null (This is not error)\n",__FUNCTION__,__LINE__);
			;
		}
    #endif

	if(stHttpAccess.info.i_pos != offset)
	{
	nRet = STREAM_HTTP_Seek ( &stHttpAccess, offset );
	if(nRet != DI_IPT_SUCCESS)
	{
		PrintError ("%s(%d) Error> STREAM_HTTP_Seek fail \n",__FUNCTION__,__LINE__);
		PrintExit;
		(void)VK_SEM_Release (s_ulHttpSemId);
		return -1;
	}
	else
	{
		PrintExit;
		(void)VK_SEM_Release (s_ulHttpSemId);
		return offset;
	}
	}
	else
	{
		PrintExit;
		(void)VK_SEM_Release (s_ulHttpSemId);
		return offset;
	}
#endif /* REAL_TIME_MULTISOCKET_SUPPORT */
#endif /* STREAM_BUFFERING */
}

/* Bounds From Nexus */
int DI_STREAM_HTTP_BoundsForNexus (VFIO_Handle_t tHandle, off_t *file_size)
{
	HINT32 nRet = VFIO_ERROR_SUCCESS;
#if REAL_TIME_MULTISOCKET_SUPPORT
	STREAM_ACCESS_t *ptHttpAccess=NULL;
#endif

	UNUSED_PARAM (tHandle);
	PrintEnter;

#if REAL_TIME_MULTISOCKET_SUPPORT
	if(tHandle.pDnpUserData != NULL)
	{
		ptHttpAccess = (STREAM_ACCESS_t *)tHandle.pDnpUserData;
	}
	else
	{
		PrintError ("%s(%d) Error> tHandle->pDnpUserData is null \n",__FUNCTION__,__LINE__);
		PrintExit;
		return VFIO_ERROR_IN_DEVICE;
	}

	if((unsigned int)s_tMultiHandle.DrmHandle == tHandle.tFile.callback.unHandle)
	{
		*file_size = ptHttpAccess->info.i_size;
	}
	else if((unsigned int)s_tMultiHandle.PlayerHandle == tHandle.tFile.callback.unHandle)
	{
		*file_size = ptHttpAccess->info.i_size;
	}
	else if((unsigned int)s_tMultiHandle.IndexHandle == tHandle.tFile.callback.unHandle)
	{
		*file_size = ptHttpAccess->info.i_size;
	}
	else
	{
		*file_size = ptHttpAccess->info.i_size;
	}
#else
	*file_size = stHttpAccess.info.i_size;
#endif

#if 0
	PrintData ("%s(%d) : i_size=%"PRId64"\n",__FUNCTION__,__LINE__, stHttpAccess.info.i_size);
#endif

	PrintExit;
	return nRet;
}
#if STREAM_BUFFERING
DI_ERR_CODE DI_STREAM_HTTP_RegisterBufferingCallback (DI_IPTUNER_PlayCallback_t *pCallback)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	PrintEnter;
	if(pCallback!=NULL)
	{
		VK_memcpy (&stHttpBufferingCallback, pCallback, sizeof(DI_IPTUNER_PlayCallback_t));
	}
	else
	{
		PrintError("%s (%d) Error> pCallback pointer not allocated. value is NULL\n",__FUNCTION__,__LINE__);
		nRet = DI_ERR_ERROR;
	}

	PrintExit;
	return nRet;
}

void DI_STREAM_HTTP_UnregisterBufferingCallback (void)
{
	PrintEnter;

	VK_memset  (&stHttpBufferingCallback, 0x0, sizeof(DI_IPTUNER_PlayCallback_t));

	PrintExit;

	return;
}
#endif

#ifdef ICY_PROTOCOL
/** Open From User Application and bmedia*/
int DI_STREAM_HTTP_ICY_Open  (const HINT8 *path, const HINT8 *mode, VFIO_Handle_t *ptHandle )
{
	HINT32 nRet = DI_IPT_SUCCESS;
	HINT32 vRet = VFIO_ERROR_SUCCESS;
	HTTP_ACCESS_SYS_t *pstHttpSys=NULL;
	STREAM_ACCESS_t *ptHttpAccess=NULL;
#if SUPPORT_COOKIE
	char *psCookie = NULL;
	URL_t tUrl;
#endif

	UNUSED_PARAM (mode);
	PrintEnter;

	ptHttpAccess = DD_MEM_Alloc(sizeof(STREAM_ACCESS_t));
	if(ptHttpAccess == NULL)
	{
		PrintError("%s(%d) : Error> Fail Mem alloc.\n",__FUNCTION__,__LINE__);
		ptHandle->ucAvailable = 0;
		PrintExit;
		return VFIO_ERROR_IN_DEVICE;
	}
	else
	{
		VK_MEM_Memset(ptHttpAccess, 0x0, sizeof(STREAM_ACCESS_t));
	}

#if SUPPORT_COOKIE
	VK_MEM_Memset(&tUrl, 0x0, sizeof(tUrl));
	UTIL_UrlParse(&tUrl, (const char *)path, 0);
	if(tUrl.psz_cookie != NULL)
	{
		psCookie = VK_StrDup(tUrl.psz_cookie);
	}
	UTIL_UrlClean(&tUrl);
#endif

	/** psz_path must be free */
#ifdef SUPPORT_TITLE
	VK_memset (s_tTitle, 0, MAX_TITLE_LENGTH);
#endif

	ptHttpAccess->psz_path = VK_StrDup((char *)path);

#if SUPPORT_COOKIE
	if(psCookie != NULL)
	{
		ptHttpAccess->psz_cookie = psCookie;
	}
#endif
	nRet = STREAM_HTTP_Open (ptHttpAccess);
	if(nRet != DI_IPT_SUCCESS)
	{
		PrintError("%s(%d) : Error> HTTP_Open Fail.\n",__FUNCTION__,__LINE__);
		ptHttpAccess->b_alive = TRUE;
		ptHandle->ucAvailable = 0;

		if(ptHttpAccess->psz_path != NULL)
		{
			DD_MEM_Free(ptHttpAccess->psz_path);
			ptHttpAccess->psz_path =NULL;
		}
		/* else { continue; } */
#if SUPPORT_COOKIE
		if(ptHttpAccess->psz_cookie != NULL)
		{
			DD_MEM_Free(ptHttpAccess->psz_cookie);
			ptHttpAccess->psz_cookie =NULL;
		}
		/* else { continue; } */
#endif

		if(ptHttpAccess != NULL)
		{
			DD_MEM_Free(ptHttpAccess);
			ptHttpAccess =NULL;
		}
		/* else { continue; } */

		vRet = VFIO_ERROR_IN_DEVICE;
	}
	else
	{
		pstHttpSys = (HTTP_ACCESS_SYS_t*) ptHttpAccess->p_sys;
		ptHandle->ucAvailable = 1;
		ptHandle->tFile.callback.unHandle = (HUINT32) pstHttpSys->fd; /** http socket descriptor */
		ptHandle->pDnpUserData = (STREAM_ACCESS_t *)ptHttpAccess;

		PrintData ("%s(%d) : callback handle=0x%x\n",__FUNCTION__,__LINE__, ptHandle->tFile.callback.unHandle);
	}

	PrintExit;
	return vRet;
}

/** */
int DI_STREAM_HTTP_ICY_Close (VFIO_Handle_t tHandle)
{
	HINT32 vRet = VFIO_ERROR_SUCCESS;
	STREAM_ACCESS_t *ptHttpAccess=NULL;

	UNUSED_PARAM (tHandle);
	PrintEnter;

	if(tHandle.pDnpUserData != NULL)
	{
		ptHttpAccess = (STREAM_ACCESS_t *)tHandle.pDnpUserData;
	}
	else
	{
		PrintError ("%s(%d) Error> tHandle->pDnpUserData is null \n",__FUNCTION__,__LINE__);
		PrintExit;
		return VFIO_ERROR_IN_DEVICE;
	}

	/* In multi-open case, the http session should be closed only once. */
	if(ptHttpAccess->b_alive == FALSE)
	{
		PrintDebug ("%s(%d) : HTTP Session is already closed.\n",__FUNCTION__,__LINE__);
		return vRet;
	}

	if(s_tIcyInstance.ulICYSemId != 0)
	{
		(void)VK_SEM_Get(s_tIcyInstance.ulICYSemId);
	}

	(void) STREAM_HTTP_Close (ptHttpAccess);

	if(s_tIcyInstance.pcIcyReadBuffer != NULL)
	{
		DD_MEM_Free (s_tIcyInstance.pcIcyReadBuffer);
		s_tIcyInstance.pcIcyReadBuffer = NULL;
	}

#ifdef FILE_TEST
	close (s_TempFd);
#endif

	ptHttpAccess->b_alive = FALSE;

	if(ptHttpAccess->psz_path != NULL)
	{
		DD_MEM_Free(ptHttpAccess->psz_path);
		ptHttpAccess->psz_path = NULL;
	}
	/* else { continue; } */

#if SUPPORT_COOKIE
	if(ptHttpAccess->psz_cookie != NULL)
	{
		DD_MEM_Free(ptHttpAccess->psz_cookie);
		ptHttpAccess->psz_cookie = NULL;
	}
	/* else { continue; } */
#endif

	if(ptHttpAccess != NULL)
	{
		DD_MEM_Free(ptHttpAccess);
		ptHttpAccess = NULL;
	}
	/* else { continue; } */

	if(s_tIcyInstance.ulICYSemId != 0)
	{
		(void)VK_SEM_Release(s_tIcyInstance.ulICYSemId);
	 	(void)VK_SEM_Destroy(s_tIcyInstance.ulICYSemId);
		s_tIcyInstance.ulICYSemId = 0;
	}

	PrintExit;
	return vRet;
}

/** */
int DI_STREAM_HTTP_ICY_Read (VFIO_Handle_t tHandle, void *ptr, size_t size, size_t nmemb, size_t *ptReadSize)
{
	UNUSED_PARAM (tHandle);
	HTTP_ACCESS_SYS_t *pHttpSys = NULL;
	HUINT32 unReadSize = 0;
	HUINT32 unRequestReadSize = 0;
	HUINT32 unRemainReadSize = 0;
	HINT32 nTempReadSize = 0;
	HUINT32 unErrrorCount = 0;
	STREAM_ACCESS_t *ptHttpAccess=NULL;

	PrintEnter;

	if(tHandle.pDnpUserData != NULL)
	{
		ptHttpAccess = (STREAM_ACCESS_t *)tHandle.pDnpUserData;
	}
	else
	{
		PrintError ("%s(%d) Error> tHandle->pDnpUserData is null \n",__FUNCTION__,__LINE__);
		PrintExit;
		return VFIO_ERROR_IN_DEVICE;
	}

	(void)VK_SEM_Get(s_tIcyInstance.ulICYSemId);

	/* already checked */
	unReadSize = size*nmemb;

	PrintData("%s(%d) : size(%u), nmemb(%u), unReadSize(%u)\n",__FUNCTION__,__LINE__, size, nmemb, unReadSize);
	PrintData("%s(%d) : i_pos(%"PRId64"), lIcyOffset(%"PRId64")\n",__FUNCTION__,__LINE__, ptHttpAccess->info.i_pos, s_tIcyInstance.lIcyOffset);

	if(ptHttpAccess->info.i_pos > s_tIcyInstance.lIcyOffset)
	{
		if((s_tIcyInstance.lIcyOffset + unReadSize) <= s_tIcyInstance.nEndOffset)
		{
			VK_MEM_Memcpy(ptr, s_tIcyInstance.pcIcyReadBuffer+(s_tIcyInstance.lIcyOffset-s_tIcyInstance.nStartOffset), unReadSize);

			*ptReadSize = unReadSize;

			PrintData("%s(%d) : tryRead(%d) realRead(%d)\n",__FUNCTION__,__LINE__,unReadSize, *ptReadSize);

			(void)VK_SEM_Release(s_tIcyInstance.ulICYSemId);
			PrintExit;
			return VFIO_ERROR_SUCCESS;
		}
		else
		{
			unRemainReadSize = (s_tIcyInstance.lIcyOffset + unReadSize) - s_tIcyInstance.nEndOffset;
			VK_MEM_Memcpy(ptr, s_tIcyInstance.pcIcyReadBuffer+(s_tIcyInstance.lIcyOffset-s_tIcyInstance.nStartOffset), (unReadSize-unRemainReadSize));
		}
	}
	else if(ptHttpAccess->info.i_pos < s_tIcyInstance.lIcyOffset)
	{
		PrintError("%s (%d) : Error> Not Support Seek\n",__FUNCTION__,__LINE__);
		*ptReadSize = 0;
		(void)VK_SEM_Release(s_tIcyInstance.ulICYSemId);
		return VFIO_ERROR_FAIL;
	}
	else
	{
		unRemainReadSize = unReadSize;
	}

	if((s_tIcyInstance.nEndOffset - s_tIcyInstance.nStartOffset) == (HUINT32)s_tIcyInstance.nIcyMetaInt*MAX_METAINT_NUM_IN_BUFFER)
	{
		s_tIcyInstance.pcIcyReadTempBuffer = DD_MEM_Alloc(s_tIcyInstance.nIcyMetaInt*MAX_METAINT_NUM_IN_BUFFER/2);
		if(s_tIcyInstance.pcIcyReadTempBuffer == NULL)
		{
			PrintError("%s (%d) : Error> Fail Mem Alloc\n",__FUNCTION__,__LINE__);
			*ptReadSize = 0;
			(void)VK_SEM_Release(s_tIcyInstance.ulICYSemId);
			return VFIO_ERROR_FAIL;
		}

		VK_MEM_Memset(s_tIcyInstance.pcIcyReadTempBuffer, 0x0, s_tIcyInstance.nIcyMetaInt*MAX_METAINT_NUM_IN_BUFFER/2);
		VK_MEM_Memcpy(s_tIcyInstance.pcIcyReadTempBuffer, s_tIcyInstance.pcIcyReadBuffer+s_tIcyInstance.nIcyMetaInt*MAX_METAINT_NUM_IN_BUFFER/2, s_tIcyInstance.nIcyMetaInt*MAX_METAINT_NUM_IN_BUFFER/2);
		VK_MEM_Memset(s_tIcyInstance.pcIcyReadBuffer, 0x0, s_tIcyInstance.nIcyMetaInt*MAX_METAINT_NUM_IN_BUFFER);
		VK_MEM_Memcpy(s_tIcyInstance.pcIcyReadBuffer, s_tIcyInstance.pcIcyReadTempBuffer, s_tIcyInstance.nIcyMetaInt*MAX_METAINT_NUM_IN_BUFFER/2);
		s_tIcyInstance.nStartOffset = s_tIcyInstance.nStartOffset + s_tIcyInstance.nIcyMetaInt*MAX_METAINT_NUM_IN_BUFFER/2;
		s_tIcyInstance.nCurrentBufferOffset = s_tIcyInstance.nIcyMetaInt*MAX_METAINT_NUM_IN_BUFFER/2;

		DD_MEM_Free(s_tIcyInstance.pcIcyReadTempBuffer);
		s_tIcyInstance.pcIcyReadTempBuffer = NULL;
	}

	/* STREAM_HTTP_Read함수 호출시 nIcyMetaInt값과 같은 Data Size가 읽히거나 아니면 0이 되어야 함. 아닌 경우는 Fail */
	unRequestReadSize = s_tIcyInstance.nIcyMetaInt;
	s_tIcyInstance.nIcyReadSize = 0;
	while(s_tIcyInstance.nIcyReadSize != s_tIcyInstance.nIcyMetaInt)
	{
		nTempReadSize = STREAM_HTTP_Read (ptHttpAccess, s_tIcyInstance.pcIcyReadBuffer+s_tIcyInstance.nCurrentBufferOffset, unRequestReadSize );

		if(nTempReadSize == DI_IPT_ETIMEOUT)
		{
			if(unErrrorCount == 2)
			{
				s_tIcyInstance.nIcyReadSize = -1;
				break;
			}

			unErrrorCount++;
			continue;
		}
		else if(nTempReadSize == 0)
		{
			if(unErrrorCount == 10)
			{
				s_tIcyInstance.nIcyReadSize = -1;
				break;
			}

			VK_TASK_Sleep(100);
			unErrrorCount++;
		}
		else if(nTempReadSize < 0)
		{
			s_tIcyInstance.nIcyReadSize = -1;
			break;
		}
		else
		{
			unErrrorCount = 0;
		}

		unRequestReadSize = unRequestReadSize - nTempReadSize;
		s_tIcyInstance.nCurrentBufferOffset = s_tIcyInstance.nCurrentBufferOffset+nTempReadSize;
		s_tIcyInstance.nIcyReadSize = s_tIcyInstance.nIcyReadSize + nTempReadSize;
	}

#ifdef SUPPORT_TITLE
	pHttpSys = (HTTP_ACCESS_SYS_t *)ptHttpAccess->p_sys;
	if(pHttpSys->psz_icy_title != NULL)
	{
		/* 새로운 타이틀이 존재하는지 검사하여 Callback을 호출함 */
		if(VK_strcmp(s_tTitle, pHttpSys->psz_icy_title) != 0)
		{
			PrintData("%s(%d) : pHttpSys.psz_icy_title=%s, s_tTitle=%s #####\n", __FUNCTION__,__LINE__, pHttpSys->psz_icy_title, s_tTitle);
			if(VK_strlen(pHttpSys->psz_icy_title) <= MAX_TITLE_LENGTH)
			{
				VK_MEM_Memset(s_tTitle, 0x0, MAX_TITLE_LENGTH);
				VK_MEM_Memcpy(s_tTitle, pHttpSys->psz_icy_title, VK_strlen(pHttpSys->psz_icy_title));
				if (NULL!=s_tEventCallback.cbChangeTitleCallback)
				{
					s_tEventCallback.cbChangeTitleCallback(DI_IPTUNER_STREAM_HTTP, s_tTitle);
				}
				/* else { continue; } */
			}
			else
			{
				PrintError("%s (%d) : Error> New Title size is more than MAX_TITLE_LENGTH.(New Title Size= %d)\n",__FUNCTION__,__LINE__, VK_strlen(pHttpSys->psz_icy_title));
			}
		}
	}
#endif

	if(s_tIcyInstance.nIcyReadSize == s_tIcyInstance.nIcyMetaInt)
	{
		if(unReadSize == unRemainReadSize)
		{
			if(unRemainReadSize < (HUINT32)s_tIcyInstance.nIcyMetaInt)
			{
				(void)VK_MEM_Memcpy(ptr, s_tIcyInstance.pcIcyReadBuffer+(s_tIcyInstance.nCurrentBufferOffset-s_tIcyInstance.nIcyReadSize), unRemainReadSize);
				/* 읽으려고 하는 Icy Data Size가 IcyMetaInt보다 작을 경우 수행 */
				*ptReadSize = unReadSize;
			}
			else
			{
				(void)VK_MEM_Memcpy(ptr, s_tIcyInstance.pcIcyReadBuffer+(s_tIcyInstance.nCurrentBufferOffset-s_tIcyInstance.nIcyReadSize), s_tIcyInstance.nIcyReadSize);

				/* 읽으려고 하는 Icy Data Size가 IcyMetaInt보다 같거나 큰 경우 수행 */
				*ptReadSize = s_tIcyInstance.nIcyReadSize;
			}
		}
		else
		{
			if(unRemainReadSize < (HUINT32)s_tIcyInstance.nIcyMetaInt)
			{
				(void)VK_MEM_Memcpy((char *)ptr+(unReadSize-unRemainReadSize), s_tIcyInstance.pcIcyReadBuffer+(s_tIcyInstance.nCurrentBufferOffset-s_tIcyInstance.nIcyReadSize), unRemainReadSize);
				/* 읽으려고 하는 Icy Data Size가 IcyMetaInt보다 작을 경우 수행 */
				*ptReadSize = unReadSize;
			}
			else
			{
				(void)VK_MEM_Memcpy((char *)ptr+(unReadSize-unRemainReadSize), s_tIcyInstance.pcIcyReadBuffer+(s_tIcyInstance.nCurrentBufferOffset-s_tIcyInstance.nIcyReadSize), s_tIcyInstance.nIcyReadSize);

				/* 읽으려고 하는 Icy Data Size가 IcyMetaInt보다 같거나 큰 경우 수행 */
				*ptReadSize = s_tIcyInstance.nIcyReadSize+(unReadSize-unRemainReadSize);
			}
		}

#ifdef FILE_TEST
		write(s_TempFd, s_tIcyInstance.pcIcyReadBuffer+(s_tIcyInstance.nCurrentBufferOffset-s_tIcyInstance.nIcyReadSize), s_tIcyInstance.nIcyReadSize);
#endif
		s_tIcyInstance.nEndOffset = s_tIcyInstance.nEndOffset+s_tIcyInstance.nIcyReadSize;
	}
	else if(s_tIcyInstance.nIcyReadSize < 0)
	{
		PrintError("%s (%d) : Error> Read Error \n",__FUNCTION__,__LINE__);
		*ptReadSize = 0;
		(void)VK_SEM_Release(s_tIcyInstance.ulICYSemId);
		return VFIO_ERROR_FAIL;
	}
	else
	{
		PrintError("%s(%d) : Error> STREAM_HTTP_Read Fail.\n",__FUNCTION__,__LINE__);
		*ptReadSize = 0;
	}

	PrintData("%s(%d) : tryRead(%d) realRead(%d)\n",__FUNCTION__,__LINE__,unReadSize, *ptReadSize);

	(void)VK_SEM_Release(s_tIcyInstance.ulICYSemId);

	PrintExit;

	return VFIO_ERROR_SUCCESS;
}

/** */
int DI_STREAM_HTTP_ICY_Seek (VFIO_Handle_t tHandle, off_t offset, int whence)
{
	HINT32 nRet;
	STREAM_ACCESS_t *ptHttpAccess=NULL;

	UNUSED_PARAM (tHandle);
	PrintEnter;

	PrintData ("%s(%d) : offset=%"PRId64", whence=%d\n",__FUNCTION__,__LINE__, offset, whence);

	if(tHandle.pDnpUserData != NULL)
	{
		ptHttpAccess = (STREAM_ACCESS_t *)tHandle.pDnpUserData;
	}
	else
	{
		PrintError ("%s(%d) Error> tHandle->pDnpUserData is null \n",__FUNCTION__,__LINE__);
		PrintExit;
		return VFIO_ERROR_IN_DEVICE;
	}

	nRet = STREAM_HTTP_Seek (ptHttpAccess, offset );
	if(nRet != DI_IPT_SUCCESS)
	{
		PrintError ("%s(%d) Error> STREAM_HTTP_Seek fail \n",__FUNCTION__,__LINE__);
		PrintExit;
		return -1;
	}
	else
	{
		PrintExit;
		return offset;
	}
}

/** */
int DI_STREAM_HTTP_ICY_Tell (VFIO_Handle_t tHandle, off_t *plCurOffset)
{
	HINT32 nRet = VFIO_ERROR_SUCCESS;

	UNUSED_PARAM (tHandle);
	UNUSED_PARAM (plCurOffset);

	PrintEnter;

	PrintExit;
	return nRet;
}

/** if EOF is true, must be return -1 like feof*/
int DI_STREAM_HTTP_ICY_IsEOF (VFIO_Handle_t tHandle)
{
	HINT32 nRet = VFIO_ERROR_SUCCESS;
	STREAM_ACCESS_t *ptHttpAccess=NULL;

	UNUSED_PARAM (tHandle);
	PrintEnter;

	if(tHandle.pDnpUserData != NULL)
	{
		ptHttpAccess = (STREAM_ACCESS_t *)tHandle.pDnpUserData;
	}
	else
	{
		PrintError ("%s(%d) Error> tHandle->pDnpUserData is null \n",__FUNCTION__,__LINE__);
		PrintExit;
		return VFIO_ERROR_IN_DEVICE;
	}

    if(TRUE == ptHttpAccess->info.b_eof)
	{
		nRet = -1; /* must be -1 like feof*/
	}

	PrintExit;
	return nRet;
}

/* Open From Nexus */
int DI_STREAM_HTTP_ICY_OpenFromNexus (const HINT8 *path, const unsigned int flags, VFIO_Handle_t *ptHandle)
{
	HINT32 nRet = DI_IPT_SUCCESS;
	HTTP_ACCESS_SYS_t *pstHttpSys=NULL;
	STREAM_ACCESS_t *ptHttpAccess=NULL;
	int result;
#if SUPPORT_COOKIE
	char *psCookie = NULL;
	URL_t tUrl;
#endif

	UNUSED_PARAM (flags);

	PrintEnter;

	ptHttpAccess = DD_MEM_Alloc(sizeof(STREAM_ACCESS_t));
	if(ptHttpAccess == NULL)
	{
		PrintError("%s(%d) : Error> Fail Mem alloc.\n",__FUNCTION__,__LINE__);
		ptHandle->ucAvailable = 0;
		PrintExit;
		return VFIO_ERROR_IN_DEVICE;
	}
	else
	{
		VK_MEM_Memset(ptHttpAccess, 0x0, sizeof(STREAM_ACCESS_t));
	}

#if SUPPORT_COOKIE
	VK_MEM_Memset(&tUrl, 0x0, sizeof(tUrl));
	UTIL_UrlParse(&tUrl, (const char *)path, 0);
	if(tUrl.psz_cookie != NULL)
	{
		psCookie = VK_StrDup(tUrl.psz_cookie);
	}
	UTIL_UrlClean(&tUrl);
#endif

	/** psz_path must be free */
	ptHttpAccess->psz_path = VK_StrDup ((char *)path);

#if SUPPORT_COOKIE
	if(psCookie != NULL)
	{
		ptHttpAccess->psz_cookie = psCookie;
	}
#endif
	nRet = STREAM_HTTP_Open (ptHttpAccess);
	if(nRet != DI_IPT_SUCCESS)
	{
		PrintError("%s (%d) : Error> STREAM_HTTP_Open Fail.\n",__FUNCTION__,__LINE__);
		ptHandle->ucAvailable = 0;
	}
	else
	{
		pstHttpSys = (HTTP_ACCESS_SYS_t *)ptHttpAccess->p_sys;
		ptHandle->tFile.callback.unHandle = (HUINT32) pstHttpSys->fd;
		ptHandle->pDnpUserData = (STREAM_ACCESS_t *)ptHttpAccess;
		ptHandle->ucAvailable = 1;

		PrintData ("%s(%d) : callback handle=0x%x\n",__FUNCTION__,__LINE__, ptHandle->tFile.callback.unHandle);
	}

#ifdef FILE_TEST
	s_TempFd = open ( "/mnt/hd2/Test.mp3" , O_RDWR|O_CREAT, 0666);
#endif
	result = VK_SEM_Create(&s_tIcyInstance.ulICYSemId,"icy", DEFAULT_SUSPEND_TYPE);
	if( result != VK_OK )
	{
		PrintError("[%s] [%d] Error(0x%x) \n\r",__FUNCTION__,__LINE__, result);
	}
	else
	{
		s_tIcyInstance.pcIcyReadBuffer = DD_MEM_Alloc((s_tIcyInstance.nIcyMetaInt*MAX_METAINT_NUM_IN_BUFFER)+1);
		if( s_tIcyInstance.pcIcyReadBuffer == NULL )
		{
			PrintError("[%s] [%d] Error> Fail Mem alloc \n\r",__FUNCTION__,__LINE__);
			nRet = VFIO_ERROR_IN_DEVICE;
		}
		else
		{
			VK_MEM_Memset(s_tIcyInstance.pcIcyReadBuffer, 0x0, (s_tIcyInstance.nIcyMetaInt*MAX_METAINT_NUM_IN_BUFFER)+1);
			s_tIcyInstance.nStartOffset = 0;
			s_tIcyInstance.nEndOffset = 0;
			s_tIcyInstance.nCurrentBufferOffset = 0;
		}
	}

	if(nRet != VFIO_ERROR_SUCCESS)
	{
		if(ptHttpAccess != NULL)
		{
			STREAM_HTTP_Close(ptHttpAccess);
		}

		if(ptHttpAccess->psz_path != NULL)
		{
			DD_MEM_Free(ptHttpAccess->psz_path);
			ptHttpAccess->psz_path =NULL;
		}
		/* else { continue; } */
#if SUPPORT_COOKIE
		if(ptHttpAccess->psz_cookie != NULL)
		{
			DD_MEM_Free(ptHttpAccess->psz_cookie);
			ptHttpAccess->psz_cookie =NULL;
		}
		/* else { continue; } */
#endif

		if(ptHttpAccess != NULL)
		{
			DD_MEM_Free(ptHttpAccess);
			ptHttpAccess =NULL;
		}
		/* else { continue; } */
	}

	PrintExit;
	return VFIO_ERROR_SUCCESS;
}

/* Seek From Nexus */
off_t DI_STREAM_HTTP_ICY_SeekForNexus (VFIO_Handle_t tHandle, off_t offset, int whence)
{
	UNUSED_PARAM (tHandle);
	UNUSED_PARAM (whence);

	PrintEnter;
	PrintData ("%s(%d) : offset=%"PRId64", whence=%d\n",__FUNCTION__,__LINE__, offset, whence);

	(void)VK_SEM_Get(s_tIcyInstance.ulICYSemId);

	s_tIcyInstance.lIcyOffset = offset;

	(void)VK_SEM_Release(s_tIcyInstance.ulICYSemId);

	PrintExit;

	return s_tIcyInstance.lIcyOffset;
}

/* Bounds From Nexus */
int DI_STREAM_HTTP_ICY_BoundsForNexus (VFIO_Handle_t tHandle, off_t *file_size)
{
	HINT32 nRet = VFIO_ERROR_SUCCESS;
	STREAM_ACCESS_t *ptHttpAccess=NULL;

	UNUSED_PARAM (tHandle);
	PrintEnter;

	if(tHandle.pDnpUserData != NULL)
	{
		ptHttpAccess = (STREAM_ACCESS_t *)tHandle.pDnpUserData;
	}
	else
	{
		PrintError ("%s(%d) Error> tHandle->pDnpUserData is null \n",__FUNCTION__,__LINE__);
		PrintExit;
		return VFIO_ERROR_IN_DEVICE;
	}

	PrintData ("%s(%d) : i_size=%"PRId64"\n",__FUNCTION__,__LINE__, ptHttpAccess->info.i_size);

	*file_size = ptHttpAccess->info.i_size;

	PrintExit;
	return nRet;
}
#endif

#ifdef SUPPORT_TITLE
DI_ERR_CODE DI_STREAM_HTTP_ICY_RegisterEventCallback (DI_IPTUNER_EventCallback_t *pCallback)
{
	DI_ERR_CODE nRet = DI_ERR_OK;

	PrintEnter;

	if(pCallback!=NULL)
	{
		VK_MEM_Memcpy(&s_tEventCallback, pCallback, sizeof(DI_IPTUNER_EventCallback_t));
	}
	else
	{
		PrintError("%s (%d) Error> pCallback pointer not allocated. value is NULL\n",__FUNCTION__,__LINE__);
		nRet = DI_ERR_ERROR;
	}

	PrintExit;

	return nRet;
}

void DI_STREAM_HTTP_ICY_UnregisterEventCallback (void)
{
	PrintEnter;

	VK_MEM_Memset(&s_tEventCallback, 0x0, sizeof(DI_IPTUNER_EventCallback_t));

	PrintExit;

	return;
}
#endif

#if defined (CONFIG_PLAYREADY)
#define ASF_HEADER_GUID_LENGTH 16
#define ASF_HEADER_OBJECT_SIZE_LENGTH 8
HBOOL DI_STREAM_HTTP_GetASFWrmHeader(HUINT8* pucUrl, HUINT8** pucWrmHeader, HUINT32* puiSize, HBOOL *bCocktail)
{
	HBOOL bValidAsfHeader=TRUE;
	HINT32 iResult;
	HUINT32 uiIndex=0;
	HUINT32 uiOffset=0;
	HUINT16 usRecordCount=0;
	HUINT16 usRecordType=0;
	HUINT16 usRecordSize=0;
	HUINT32 uiDataSize=0;
	HUINT64 ullHeaderSize=0;
	HUINT8* pucBuf=NULL;
	long lReadResult=0;
	size_t sz_Try=0;
	size_t sz_Total=0;
	STREAM_ACCESS_t tAccess;
	HUINT8 ucAsfHeaderObject[ASF_HEADER_GUID_LENGTH] = {0x30, 0x26, 0xB2, 0x75, 0x8E, 0x66, 0xCF, 0x11, 0xA6, 0xD9, 0x00, 0xAA, 0x00, 0x62, 0xCE, 0x6C};
	HUINT8 ucAsfPlayReadyHeaderObject[ASF_HEADER_GUID_LENGTH] = {0x10, 0x70, 0x63, 0xF4, 0xC3, 0x03, 0xCD, 0x42, 0xB9, 0x32, 0xB4, 0x8A, 0xDF, 0x3A, 0x6A, 0x54};
	HUINT8 ucAsfExtendedContentEncryptionObject[ASF_HEADER_GUID_LENGTH] = {0x14, 0xE6, 0x8A, 0x29, 0x22, 0x26, 0x17, 0x4C, 0xB9, 0x35, 0xDA, 0xE0, 0x7E, 0xE9, 0x28, 0x9C};

	if((pucUrl == NULL) || (puiSize == NULL))
	{
		PrintError("invalid parameter!\n");
		return FALSE;
	}

/*************************
*	1. Open the connection
*************************/
	VK_MEM_Memset(&tAccess,0,sizeof(STREAM_ACCESS_t));
	tAccess.psz_path = VK_StrDup (pucUrl);
	if(tAccess.psz_path == NULL)
	{
		PrintError("VK_StrDup failed!\n");
		return FALSE;
	}

	iResult = STREAM_HTTP_Open(&tAccess);
	if(iResult != DI_IPT_SUCCESS)
	{
		PrintError("STREAM_HTTP_Open failed!\n");
		VK_MEM_Free(tAccess.psz_path);
		tAccess.psz_path = NULL;
		return FALSE;
	}

/*************************
*	2. Read the header
*	ASF_Header_Object GUID:
*	75B22630-668E-11CF-A6D9-00AA0062CE6C
*************************/
	pucBuf = VK_MEM_Alloc(ASF_HEADER_GUID_LENGTH + ASF_HEADER_OBJECT_SIZE_LENGTH);
	if(pucBuf == NULL)
	{
		PrintError("Out of Memory!\n");
		STREAM_HTTP_Close(&tAccess);
		return FALSE;
	}

	sz_Try = (ASF_HEADER_GUID_LENGTH + ASF_HEADER_OBJECT_SIZE_LENGTH);
	sz_Total = 0;
	while(sz_Total < (ASF_HEADER_GUID_LENGTH + ASF_HEADER_OBJECT_SIZE_LENGTH))
	{
		lReadResult = STREAM_HTTP_Read(&tAccess, (HUINT8 *)pucBuf, sz_Try);
		sz_Total += lReadResult;
		sz_Try -= lReadResult;
	}

	for(uiIndex=0; uiIndex<ASF_HEADER_GUID_LENGTH; uiIndex++)
	{
		if( pucBuf[uiIndex] != ucAsfHeaderObject[uiIndex])
		{
			bValidAsfHeader=FALSE;
		}
	}

	if(bValidAsfHeader == TRUE)
	{
		ullHeaderSize= (pucBuf[ASF_HEADER_GUID_LENGTH]|
							pucBuf[ASF_HEADER_GUID_LENGTH+1]<<8 |
							pucBuf[ASF_HEADER_GUID_LENGTH+2]<<16 |
							pucBuf[ASF_HEADER_GUID_LENGTH+3]<<24);
		/*
							pucBuf[ASF_HEADER_GUID_LENGTH+4]<<32 |
							pucBuf[ASF_HEADER_GUID_LENGTH+5]<<40 |
							pucBuf[ASF_HEADER_GUID_LENGTH+6]<<48 |
							pucBuf[ASF_HEADER_GUID_LENGTH+7]<<56);
		*/
	}
	VK_MEM_Free(pucBuf);
	pucBuf = NULL;

/************************
*	3. Retrieve the WRM Header
*************************/
	if(ullHeaderSize != 0)
	{
		pucBuf = VK_MEM_Alloc(ullHeaderSize);
		if(pucBuf == NULL)
		{
			PrintError("Out of Memory!\n");
			STREAM_HTTP_Close(&tAccess);
			return FALSE;
		}

		iResult = STREAM_HTTP_Seek(&tAccess, 0);
		if(iResult != DI_IPT_SUCCESS)
		{
			PrintError("STREAM_HTTP_Seek failed!\n");
			VK_MEM_Free(pucBuf);
			STREAM_HTTP_Close(&tAccess);
			return FALSE;
		}

		sz_Try = ullHeaderSize;
		sz_Total = 0;
		while(sz_Total < ullHeaderSize)
		{
			lReadResult = STREAM_HTTP_Read(&tAccess, (HUINT8 *)pucBuf+sz_Total, sz_Try);
			sz_Total += lReadResult;
			sz_Try -= lReadResult;
		}

/*************************
*	PlayReady Header GUID
*************************/
		uiIndex = 0;
		*bCocktail = FALSE;
		for(uiOffset=0; uiOffset < ullHeaderSize; uiOffset++)
		{
			for(uiIndex=0; uiIndex < ASF_HEADER_GUID_LENGTH; uiIndex++)
			{
				if(pucBuf[uiOffset+uiIndex] != ucAsfPlayReadyHeaderObject[uiIndex])
				{
					break;
				}
			}
			if(uiIndex == ASF_HEADER_GUID_LENGTH)
			{
				PrintDebug("PlayReady header is found!\n");
				break;
			}

			for(uiIndex=0; uiIndex < ASF_HEADER_GUID_LENGTH; uiIndex++)
			{
				if(pucBuf[uiOffset+uiIndex] != ucAsfExtendedContentEncryptionObject[uiIndex])
				{
					break;
				}
			}

			if(uiIndex == ASF_HEADER_GUID_LENGTH)
			{
				PrintDebug("ASF extended content encryption object is found!\n");
				*bCocktail = TRUE;
				break;
			}
		}

		if(uiIndex != 16)
		{
			PrintDebug ("Encryption guid abscent!\n");
			VK_MEM_Free(pucBuf);
			pucBuf = NULL;
			STREAM_HTTP_Close(&tAccess);
			return FALSE;
		}

		uiOffset += 16;

		if(*bCocktail == TRUE)
		{
/*************************
*	Extended Content Encryption Object
*	(Object Size 64bits)
*************************/
			uiOffset += 8;

/*************************
*	Extended Content Encryption Object
*	(Data Size 32bits)
*************************/
			uiDataSize = pucBuf[uiOffset] | pucBuf[uiOffset+1]<<8 | pucBuf[uiOffset+2]<<16 | pucBuf[uiOffset+3]<<24;
			uiOffset += 4;

			if(*pucWrmHeader != NULL)
			{
				VK_MEM_Free(*pucWrmHeader);
				*pucWrmHeader = NULL;
			}

			*pucWrmHeader = VK_MEM_Alloc(uiDataSize);
			if(*pucWrmHeader == NULL)
			{
				PrintError("Out of Memory!\n");
				VK_MEM_Free(pucBuf);
				pucBuf = NULL;
				STREAM_HTTP_Close(&tAccess);
				return FALSE;
			}

			VK_MEM_Memcpy(*pucWrmHeader, &pucBuf[uiOffset], uiDataSize);
			*puiSize = uiDataSize;
			VK_MEM_Free(pucBuf);
			pucBuf = NULL;
			STREAM_HTTP_Close(&tAccess);
			return TRUE;
		}
		else
		{
/*************************
*	PSI object (System Version 32bits)
*	PSI object (Data Size 32bits)
*************************/
		uiOffset += 8;

/*************************
*	PlayReady Header Objects (Length 32bits)
*	PlayReady Header Objects (Record Count 16bits)
*************************/
		usRecordCount = pucBuf[uiOffset+4] | (pucBuf[uiOffset+5]<<8);
		uiOffset += 6;

/*************************
*	PlayReady Header Objects (Record Type 16bits)
*	PlayReady Header Objects (Record Length 16bits)
*	PlayReady Header Objects (Record Value)
*************************/
		for(uiIndex=0; uiIndex<usRecordCount; uiIndex++)
		{
			usRecordType = pucBuf[uiOffset] | (pucBuf[uiOffset+1]<<8);
			usRecordSize = pucBuf[uiOffset+2] | (pucBuf[uiOffset+3]<<8);
			uiOffset+=4;
			if(usRecordType == 0x1)
			{
				PrintDebug("WRM Header Found!\n");
				break;
			}
			else
			{
				uiOffset+=usRecordSize;
			}
		}

		if(uiIndex == usRecordCount)
		{
			PrintDebug("WRM Header abscent!\n");
			VK_MEM_Free(pucBuf);
			pucBuf = NULL;
			STREAM_HTTP_Close(&tAccess);
			return FALSE;
		}

		if(*pucWrmHeader != NULL)
		{
			VK_MEM_Free(*pucWrmHeader);
			*pucWrmHeader = NULL;
		}

		*pucWrmHeader = VK_MEM_Alloc(usRecordSize);
		if(*pucWrmHeader == NULL)
		{
			PrintError("Out of Memory!\n");
			VK_MEM_Free(pucBuf);
			pucBuf = NULL;
			STREAM_HTTP_Close(&tAccess);
			return FALSE;
		}

		VK_MEM_Memcpy(*pucWrmHeader, &pucBuf[uiOffset], usRecordSize);
		*puiSize = usRecordSize;
		VK_MEM_Free(pucBuf);
		pucBuf = NULL;
		STREAM_HTTP_Close(&tAccess);
		return TRUE;
	}
	}

	STREAM_HTTP_Close(&tAccess);
	return FALSE;
}
#endif

HINT64 DI_STREAM_HTTP_GetStreamSize(void)
{
	return s_nFileLength;
}
HINT64 DI_STREAM_HTTP_GetCurrentReadPosition(void)
{
	return s_llCurrentReadPosition;
}
