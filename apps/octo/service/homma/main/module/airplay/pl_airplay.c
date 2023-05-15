/*************************************************************************************************************
	File            : pl_airplay.c
	author          : parkjh4@humaxdigital.com
	comment         :
	date            :
	attention       :

	(c) 2011-2013 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. (Humax) and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED ¢®¡ÆAS IS¢®¡¾ AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
	EITHER EXPRESS, IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
	IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR
	INDIRECT, SPECIAL, CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR
	INABILITY TO USE THE SOFTWARE.

	This License is effective until terminated. You may terminate this License at any time
	by destroying all copies of the Software including all documentation. This License will terminate
	immediately without notice from Humax to you if you fail to comply with any provision of this License.
	Upon termination, you must destroy all copies of the Software and all documentation.

	The laws of the Republic of Korea will apply to any disputes arising out of or relating to
	this Copyright Notice. All claims arising out of or relating to this Copyright Notice will be litigated
	in the Seoul Central District Court, in the Republic of Korea.
*************************************************************************************************************/

#define	________________Header_Files_______________________________

#include <hlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <net/if.h>
#include <vkernel.h>

#include "common.h"
#include "pl_common.h"
#include "pl_airplay.h"
#include "hmxairplay.h"

#define ________________Private_Mecro_and_Defines__________________
#define	__MODULE__	"PL:AIRPLAY"

#define HUINT32_MAX (0xFFFFFFFF)
#define INFO_CACHE_VALID_TIME		(500)	/* in ms */
#define _RAND_BLOCK_SIZE		16

#define	PL_AIRPLAY_SNPRINTF(x, v)			HxSTD_snprintf(x, sizeof(x), "%s", v)

#define ________________Internal_APIs_Prototype____________________


#define ________________Internal_Typedef_and_Structure_____________
#define UNUSED_PARAM(x)	((void)x)
#define SET_FEATURE(val, feature) do { val |= feature; } while(0)
#define UNSET_FEATURE(val, feature) do { val &= ~feature; } while(0)
#define msleep(x)	usleep(x*1000)

#define AIRPLAY_PHOTO_FILE_JPG	"/tmp/airplay_pohto_%d.jpg"
#define AIRPLAY_PHOTO_FILE_PNG	"/tmp/airplay_pohto_%d.png"
#define AIRPLAY_AUDIO_THUMBNAIL_JPG	"/tmp/airplay_thumbnail_%d.jpg"
#define AIRPLAY_AUDIO_THUMBNAIL_PNG	"/tmp/airplay_thumbnail_%d.png"
#define AIRPLAY_SETRATE_SKIP_INTERVAL	(5000)

typedef enum PL_AIRPLAY_PUBLISHER_STATUS_e
{
	ePUBLISHER_ERROR = 0,
	ePUBLISHER_READY,
	ePUBLISHER_RUNNING_REQ,
	ePUBLISHER_RUNNING,
	ePUBLISHER_QUIT,
	ePUBLISHER_QUIT_OK
} PL_AIRPLAY_PUBLISHER_STATUS_t;

typedef enum PL_AIRPLAY_SERVER_STATUS_e
{
	eSERVER_ERROR = 0,
	eSERVER_READY,
	eSERVER_RUNNING_REQ,
	eSERVER_RUNNING,
	eSERVER_QUIT,
	eSERVER_QUIT_OK
} PL_AIRPLAY_SERVER_STATUS_t;

typedef enum PL_AIRPLAY_CACHE_RET_e
{
	eCACHE_VALID = 0,	/* Cached value is valid */
	eCACHE_UPDATED,		/* Cached value is updated */
	eCACHE_NO_CALLBACK,	/* Callback does not exist */
	eCACHE_TIMEOUT,		/* Request to update, but there are no response in certain time */
} PL_AIRPLAY_CACHE_RET_t;

typedef struct
{
	PL_AIRPLAY_PUBLISHER_STATUS_t 	ePUBLISHER_STATUS;
	PL_AIRPLAY_SERVER_STATUS_t 		eHTTPD_STATUS;
	PL_AIRPLAY_SERVER_STATUS_t 		eRTSPD_STATUS;
	PL_AIRPLAY_OperationCallback 	cbOperation;
	HINT32							nAirFeatures;
	HBOOL 							bAirPlayInit;
	HBOOL							bAirPlayStarted;
	HBOOL							bRequestMediaPlay;
	HUINT32							nPublisherTaskId;
	HUINT32							nHTTPDTaskId;
	HUINT32							nRTSPDTaskId;
	PL_AIRPLAY_SERVICE_PARAM_t		tServiceParam;
	PL_COMMON_MacInfo_t 			tMacInfo;
	PL_AIRPLAY_PLAYBACK_INFO_t 		tPlaybackInfoCache;
	HINT64 							ullInfoLastUpdated;	/* This is to control media info cache if need to update or not */
	HUINT32 						unInfoSequenceNum;	/* This is to identify that media info has been updated after request pbinfo */
	HUINT32							ulActivePlayCnt;        /* To prevent fast setrate(0) */
	HUINT32							ulLastPlayProcessTick;  /* To prevent fast setrate(0) */
}PL_AIRPLAY_Context_t;

#define ________________Private_Static_Variables___________________
static PL_AIRPLAY_Context_t s_PL_AIRPLAY_Context = { ePUBLISHER_ERROR,      /* status     */
													 eSERVER_ERROR,         /* status     */
													 eSERVER_ERROR,         /* status     */
													 NULL,                  /* callback   */
													 0,                     /* features   */
													 FALSE,                 /* inited?    */
													 FALSE,                 /* started?   */
													 FALSE,                 /* request?   */
													 0x0,                   /* publ tid   */
													 0x0,                   /* httpd tid  */
													 0x0,                   /* rtspd tid  */
													 {0, 0, "", "", ""},    /* svc params */
													 {""},                  /* mac info   */
													 {0, 0, 0, 0, 0, 0, 0}, /* pb info    */
													 0,                     /* last       */
													 0};                 	/* seqNum     */



#define ________________Internal_Functions_________________________
static inline PL_AIRPLAY_Context_t *__airplay_context( void )
{
	return &s_PL_AIRPLAY_Context;
}

void do_airplay_log_cb(AIRPLAY_LOG_LEVEL logLevel, const char* fmt, ...)
{
	HINT8 va_str[4096];
	va_list va;
	va_start(va, fmt);
	vsprintf(va_str, fmt, va);
	switch(logLevel)
	{
		case AIRPLAY_LOG_DEBUG:
			HxLOG_Debug("[DBG] %s", va_str);
			break;
		case AIRPLAY_LOG_ERROR:
			HxLOG_Error("[ERR] %s", va_str);
			break;
		case AIRPLAY_LOG_CRITICAL:
			HxLOG_Error("[CR] %s", va_str);
			break;
		case AIRPLAY_LOG_INT_MODULE:
			HxLOG_Debug("\t%s", va_str);
			break;
	}

	va_end(va);
}

static void do_httpd_task(void *args)
{
	PL_AIRPLAY_Context_t *hAIRPLAY = __airplay_context();
	UNUSED_PARAM(args);
	while(TRUE)
	{
		while(hAIRPLAY->eHTTPD_STATUS == eSERVER_READY)
		{
			msleep(100);
		}
		if(hAIRPLAY->eHTTPD_STATUS == eSERVER_QUIT)
		{
			HxLOG_Error("[ %s:%d ] Stopping airplay httpd\n", __MODULE__,__LINE__);
			break;
		}

		hAIRPLAY->eHTTPD_STATUS = eSERVER_RUNNING;

		//Polling loop
#ifdef LIBAIRPLAY_VERSION
#if	LIBAIRPLAY_VERSION >= VERSION(1,0,3)
		AIRPLAY_SERVER_Start(AIRPLAY_SERVER_HTTPD);
#endif
#else
		AIRPLAY_SERVER_Start();
#endif

		HxLOG_Print("[ %s:%d ] httpd task : Break polling\n", __MODULE__,__LINE__);
		msleep(100);
	}
	HxLOG_Print("[ %s:%d ] httpd task terminated\n",__MODULE__,__LINE__);
	hAIRPLAY->eHTTPD_STATUS = eSERVER_QUIT_OK;
}

#ifdef LIBAIRPLAY_VERSION
#if	LIBAIRPLAY_VERSION >= VERSION(1,0,3)
static void do_rtspd_task(void *args)
{
	PL_AIRPLAY_Context_t *hAIRPLAY = __airplay_context();
	UNUSED_PARAM(args);
	while(TRUE)
	{
		while(hAIRPLAY->eRTSPD_STATUS == eSERVER_READY)
		{
			msleep(100);
		}
		if(hAIRPLAY->eRTSPD_STATUS == eSERVER_QUIT)
		{
			HxLOG_Error("[ %s:%d ] Stopping airplay rtspd\n", __MODULE__,__LINE__);
			break;
		}

		hAIRPLAY->eRTSPD_STATUS = eSERVER_RUNNING;

		//Polling loop
		AIRPLAY_SERVER_Start(AIRPLAY_SERVER_RTSPD);
		HxLOG_Print("[ %s:%d ] rtspd task : Break polling\n", __MODULE__,__LINE__);
		msleep(100);
	}
	HxLOG_Print("[ %s:%d ] rtspd task terminated\n",__MODULE__,__LINE__);
	hAIRPLAY->eRTSPD_STATUS = eSERVER_QUIT_OK;
}
#endif
#endif

static void do_publisher_task(void *args)
{
	AIRPLAY_RETCODE nRet = AIRPLAY_OK;
	PL_AIRPLAY_Context_t *hAIRPLAY = __airplay_context();

	UNUSED_PARAM(args);

	while(TRUE)
	{
		while(hAIRPLAY->ePUBLISHER_STATUS == ePUBLISHER_READY)
		{
			msleep(100);
		}
		if(hAIRPLAY->ePUBLISHER_STATUS == ePUBLISHER_QUIT)
		{
			HxLOG_Error("[ %s:%d ] Stopping airplay publisher\n", __MODULE__,__LINE__);
			hAIRPLAY->ePUBLISHER_STATUS = ePUBLISHER_QUIT_OK;
			break;
		}

		hAIRPLAY->ePUBLISHER_STATUS = ePUBLISHER_RUNNING;
		
		nRet = AIRPLAY_PUBLISHER_Init(0 /*1*/);
		if(nRet != AIRPLAY_OK)
		{
			HxLOG_Error("[ %s:%d ] Error> publisher init failed\n", __MODULE__,__LINE__);
			return;
		}

		HxLOG_Print("[pl_airplay] +++++++++++publisher Start\n");
		//Polling loop
		AIRPLAY_PUBLISHER_Start(); /* blocking */
		HxLOG_Print("[pl_airplay] -----------publisher Start\n");

		nRet = AIRPLAY_PUBLISHER_Uninit();
		if(nRet != AIRPLAY_OK)
		{
			HxLOG_Error("[ %s:%d ] Error> publisher was not initialized\n",__MODULE__,__LINE__);
			return;
		}
	}

	HxLOG_Print("[ %s:%d ] publisher task terminated\n", __MODULE__,__LINE__);
	hAIRPLAY->ePUBLISHER_STATUS = ePUBLISHER_QUIT_OK;
}

static HBOOL do_issuing_event(PL_AIRPLAY_Context_t *pHandle, PL_AIRPLAY_OP_TYPE reqType, char* pPARAM, HINT32 iPARAM)
{
	if(pHandle != NULL && pHandle->cbOperation != NULL)
	{
		pHandle->cbOperation(reqType, pPARAM, iPARAM);
		return TRUE;
	}
	return FALSE;
}

static HBOOL do_need_cache_refresh(PL_AIRPLAY_Context_t* pCtx)
{
	HUINT64 curTimeMs = 0;
	struct timeval curTime;
	gettimeofday(&curTime, NULL);

	curTimeMs = (HUINT64)((HUINT64)curTime.tv_sec * 1000) + (HUINT64)(curTime.tv_usec / 1000);
	if(curTimeMs > pCtx->ullInfoLastUpdated + INFO_CACHE_VALID_TIME)
	{
		return TRUE;
	}
	return FALSE;
}

static PL_AIRPLAY_CACHE_RET_t do_cache_validation(PL_AIRPLAY_Context_t* pCtx)
{
	PL_AIRPLAY_Context_t *__context = __airplay_context();
	HUINT32 storedCacheVer = __context->unInfoSequenceNum;
	const HUINT32 each_sleep_time = 100;	/* in ms */
	const HUINT32 max_timeout = 50;			/* maximum 50*100(ms) => 5sec */
	HUINT32	cur_timeout = 0;

	/* check cache validation */
	if(do_need_cache_refresh(__context))
	{
		/* update request */
		if(do_issuing_event(__context, PL_AIRPLAY_OP_GET_PLAYBACK_INFO, NULL, 0))
		{
			/* Waiting for new pbinfo incomes */
			while(storedCacheVer == __context->unInfoSequenceNum)
			{
				if(cur_timeout > max_timeout)
				{
					HxLOG_Error("Requested playback info, but there are no response\n");
					return eCACHE_TIMEOUT;
				}
				msleep(each_sleep_time);
				cur_timeout++;
				continue;
			}
		}
		else
		{
			return eCACHE_NO_CALLBACK;
		}
	}
	else
	{
		return eCACHE_VALID;
	}
	return eCACHE_UPDATED;
}

#ifdef LIBAIRPLAY_VERSION
#if	LIBAIRPLAY_VERSION >= VERSION(1,0,5)
/* callbacks for airtuens */
static void cb_airtunes_play ( int8_t * destinationIp, uint32_t dport, uint32_t cport, uint32_t tport, int8_t * aesKey, int8_t * aesIv, int8_t * fmtpStr, int8_t * aCodec )
{
	/* URL should be formed this : hairplay://rtp://192.168.2.151?dport=6000&cport=6001&tport=6002&aeskey=7D628FAC6F76F9191F32787478DA8FDD&aesiv=98D9157588A90CB0E84D28E590BFE0F2&fmtpstr=96 352 0 16 40 10 14 2 255 0 0 44100&acodec=alac */
	HCHAR		Url[2048];
	const HCHAR	*URL_FORM = "hairplay://raop://%s?dport=%d&cport=%d&tport=%d&aeskey=%s&aesiv=%s&acodec=%s&fmtpstr=%s";
	PL_AIRPLAY_Context_t *__context = __airplay_context();
	HOM_FUNC_ENTER;

	if ( dport == 0 || cport == 0 || tport == 0  )
	{
		HxLOG_Error("Ports shouldn't be 0 \n");
		return;
	}

	if ( destinationIp == NULL ||  aesKey == NULL || aesIv == NULL || fmtpStr == NULL || aCodec == NULL )
	{
		HxLOG_Error("Strings should be provided! \n");
		return;
	}

	HxSTD_MemSet(&Url, 0x00, sizeof(Url));

	HxSTD_snprintf(Url, 2048, URL_FORM, destinationIp, dport, cport, tport, aesKey, aesIv, aCodec, fmtpStr); /* fix for prevent */

	HxLOG_Debug("[ %s:%d ] RAOP URL : %s\n", __MODULE__, __LINE__, Url);

	/* TODO this Operation should be changed PL_AIRTUNES_OP_PLAY */
	do_issuing_event(__airplay_context(), PL_AIRTUNES_OP_PLAY, Url, 0);
	__context->ulActivePlayCnt++;
	__context->ulLastPlayProcessTick  = HLIB_STD_GetSystemTick();
	HOM_FUNC_LEAVE;
	return;
}
static void  cb_airtunes_flush (void)
{
	do_issuing_event(__airplay_context(), PL_AIRTUNES_OP_FLUSH, NULL, 0);
	return;
}
static void  cb_airtunes_stop (void)
{
	PL_AIRPLAY_Context_t *__context = __airplay_context();
	HOM_FUNC_ENTER;
	/* TODO this Operation should be changed PL_AIRTUNES_OP_STOP */
	do_issuing_event(__airplay_context(), PL_AIRTUNES_OP_STOP, NULL, 0);
	__context->ulActivePlayCnt--;
	__context->ulLastPlayProcessTick = 0x0;
	HOM_FUNC_LEAVE;
	return;
}

static HBOOL __get_urandom(char *pTarget, HUINT32 i_maxLen) 
{
	FILE *fp = NULL;
	char *m_charset = "0123456789abcdef";
	HUINT32 idx = 0;
	char *pTmp = pTarget;
	HUINT32 i_tmpLen = i_maxLen;
	struct timeval tvTime;
	HINT32 ret = 0;
	pid_t tPid;

	fp = fopen("/dev/urandom","r");
	if (fp == NULL) 
	{
		HxLOG_Error("[ %s:%d ] Error> Can't open /dev/urandom\n", __MODULE__, __LINE__);
		/* Don't retry to /dev/random; This is unconfirmed */
		/* fp = fopen("/dev/random","r"); */
		goto func_error;
	}
	
	if (fp == NULL || (ret = fread(pTarget,i_maxLen,1,fp)) == 0) 
	{
		HxLOG_Error("[ %s:%d ] Error> Can't read /dev/urandom\n", __MODULE__, __LINE__);
		
		/* Use another random generation: time + pid + ... */
		tPid = getpid();
		
		gettimeofday(&tvTime,NULL);
		if (i_tmpLen >= sizeof(tvTime.tv_usec)) {
			memcpy(pTmp,&tvTime.tv_usec,sizeof(tvTime.tv_usec));
			i_tmpLen -= sizeof(tvTime.tv_usec);
			pTmp += sizeof(tvTime.tv_usec);
		}
		if (i_tmpLen >= sizeof(tvTime.tv_sec)) {
			memcpy(pTmp,&tvTime.tv_sec,sizeof(tvTime.tv_sec));
			i_tmpLen -= sizeof(tvTime.tv_sec);
			pTmp += sizeof(tvTime.tv_sec);
		}
		if (i_tmpLen >= sizeof(tPid)) {
			memcpy(pTmp,&tPid,sizeof(tPid));
			i_tmpLen -= sizeof(tPid);
			pTmp += sizeof(tPid);
		}
		
		/* Because of CID #172619, can't use random(); Use fixed value if /dev/urandom is unavailable */
		/* xor with random() output */
		/*
		 * for (idx = 0; idx < i_maxLen; idx++)
		 * {
		 * 	pTarget[idx] ^= random();
		 * }
		 */
	}
	
	/* hex digits taking just 4 bits out of 8 */
	for (idx = 0; idx < i_maxLen; idx++)
	{
		pTarget[idx] = m_charset[pTarget[idx] & 0x0F];
	}
	
	if (fp != NULL)
	{
		fclose(fp);
		fp = NULL;
	}
	return TRUE;
	
func_error:
	if (fp != NULL)
	{
		fclose(fp);
		fp = NULL;
	}
	return FALSE;
}

static HUINT32  __get_random(void)
{
	unsigned char m_randBlock[_RAND_BLOCK_SIZE];
	HUINT32 i_rand = 0x0;
	HUINT32 i_seed = 0x0;	
	struct timeval tvTime;

	memset(m_randBlock, 0x0, _RAND_BLOCK_SIZE);
#if 0 // do not use this function.
	if (__get_urandom(m_randBlock, _RAND_BLOCK_SIZE) == TRUE)
	{
		i_rand = strtol(m_randBlock, NULL, 16);
	}
	else
#endif
	{
		if (gettimeofday(&tvTime, NULL) == 0)
		{
			i_seed = (HUINT32)(tvTime.tv_usec + tvTime.tv_sec);
		}
		else
		{
			i_seed = (HUINT32)time(NULL);
		}
	}
	return i_rand ? i_rand : i_seed;
}

static void  cb_airtunes_set_albumart(int8_t * pData, uint32_t size)
{
	FILE *fp = NULL;
	char temp_filename[128];
	HBOOL bGotImage = FALSE;

	/* bzero */
	HxSTD_memset(temp_filename, 0, sizeof(temp_filename));

#ifdef __DONT_USE_THIS_RAND_FUNCTION__ 
	/* For Random Number */
	srand(time(NULL));
#endif

	/* remove previous images */
	system("rm -f /tmp/airplay_thumbnail*");

	if ( pData && size > 3 )
	{
		if ( (pData[0] == 'P') && (pData[1] == 'N') && (pData[2] == 'G') )
		{
#ifdef __DONT_USE_THIS_RAND_FUNCTION__ 
			HxSTD_snprintf(temp_filename, 128, AIRPLAY_AUDIO_THUMBNAIL_PNG, rand()); /* fix for prevent #172619 */
#else
			HxSTD_snprintf(temp_filename, 128, AIRPLAY_AUDIO_THUMBNAIL_PNG, __get_random());
#endif
			fp = fopen(temp_filename , "wb" );
			if (fp != NULL)
			{
			fwrite(pData, 1, size, fp);
				fclose(fp); fp = NULL;
				bGotImage = TRUE;
			}
		}
		else
		{
#ifdef __DONT_USE_THIS_RAND_FUNCTION__ 
			HxSTD_snprintf(temp_filename, 128, AIRPLAY_AUDIO_THUMBNAIL_JPG, rand()); /* fix for prevent #172619 */
#else
			HxSTD_snprintf(temp_filename, 128, AIRPLAY_AUDIO_THUMBNAIL_JPG, __get_random());
#endif
			fp = fopen(temp_filename , "wb" );
			if (fp != NULL)
			{
			fwrite(pData, 1, size, fp);
				fclose(fp); fp = NULL;
				bGotImage = TRUE;
			}
		}

		if (bGotImage == TRUE)
		{
		do_issuing_event(__airplay_context(), PL_AIRTUNES_OP_SET_ALBUMART, temp_filename, 0);
	}
	else
	{
			HxLOG_Error("Error> Cant get image path\n");
		}
	}
	else
	{
		HxLOG_Error("Error> No data to send...\n");
	}
	return;
}

static void  cb_airtunes_set_metadata(int8_t * title, int8_t * artist, int8_t * album)
{
	PL_AIRPLAY_METADATA_t metadata;

	HOM_MEMSET(&metadata, 0, sizeof(PL_AIRPLAY_METADATA_t));

	HxSTD_StrNCpy(metadata.szAlbum, album, sizeof(metadata.szAlbum)-1);
	HxSTD_StrNCpy(metadata.szArtist, artist, sizeof(metadata.szArtist)-1);
	HxSTD_StrNCpy(metadata.szTitle, title, sizeof(metadata.szTitle)-1);

	HxLOG_Debug("[ %s:%d ] METADATA : title - %s, artist - %s, album - %\n", __MODULE__, __LINE__, metadata.szTitle, metadata.szArtist, metadata.szAlbum);

	if (title && artist && album)
	{
		do_issuing_event(__airplay_context(), PL_AIRTUNES_OP_SET_METADATA, (char*)&metadata, 0);
	}
	else
	{
		HxLOG_Error("No data to send...\n");
	}
	return;
}
#endif
#endif

/* callbacks */
static void cb_play(int8_t* pURL, double position)
{
	/* URL should be formed this : hairplay://http://www.aaa.ccc?START=xx.x */
	HCHAR		*pNewURL = NULL;
	HINT32		szNewURL = 0;
	const HCHAR	*URL_FORM = "hairplay://%s?START=%.1f";
	const HINT32 szURLForm = 19;
	PL_AIRPLAY_Context_t *__context = __airplay_context();
	HOM_FUNC_ENTER;

	if(pURL != NULL)
	{
		 szNewURL = HxSTD_StrLen(pURL) + 5 /* startpos : xxx.x */ + szURLForm + 1;
	}
	else
	{
		HxLOG_Error("[ %s:%d ] ERR> URL is invalid\n",__MODULE__, __LINE__);
		return;
	}
	pNewURL = (HCHAR*)HOMxEX_Malloc(szNewURL);
	if(!pNewURL)
	{
		HxLOG_Error("[ %s:%d ] ERR> Out of memory\n",__MODULE__, __LINE__);
		return;
	}
	HxSTD_snprintf(pNewURL, szNewURL, URL_FORM, pURL, position*100);

	HxLOG_Debug("[ %s:%d ] New URL : %s\n", __MODULE__, __LINE__, pNewURL);
	do_issuing_event(__airplay_context(), PL_AIRPLAY_OP_PLAY, pNewURL, (HINT64)(position * 100));
	__context->ulActivePlayCnt++;
	__context->ulLastPlayProcessTick = HLIB_STD_GetSystemTick();

	/* free allocated URL */
	HOMxEX_Free(pNewURL);
	HOM_FUNC_LEAVE;
	return;
}

static void cb_setpos(double position) /* TODO: There is no callback handle */
{
	HOM_FUNC_ENTER;
	do_issuing_event(__airplay_context(), PL_AIRPLAY_OP_SETPOSITION, NULL, (HINT64)(position * 1000));
	HOM_FUNC_LEAVE;
	return;
}

static void cb_setrate(double rate) /* TODO: There is no callback handle */
{
	HUINT32 uTickDiff = 0x0;
	PL_AIRPLAY_Context_t *__context = __airplay_context();
	HOM_FUNC_ENTER;

	if (0)
{
		if (rate == 0.0) /* Prevent occurred setrate(POST /rate?value=0.000000) immediately after play() */
		{
			uTickDiff = (HLIB_STD_GetSystemTick() - __context->ulLastPlayProcessTick);
			if (__context->ulActivePlayCnt && __context->ulLastPlayProcessTick && (uTickDiff < AIRPLAY_SETRATE_SKIP_INTERVAL))
			{
				HxLOG_Error("Error> Too fast setrate(0) request(diff:%ld, play:%ld)! --> Ignored\n", 
						uTickDiff, __context->ulLastPlayProcessTick);
				goto func_done;
			}
		}
	}
	
	do_issuing_event(__airplay_context(), PL_AIRPLAY_OP_SETRATE, NULL, (HINT64)(rate * 100));

func_done:
	HOM_FUNC_LEAVE;
	return;
}

static void cb_stop(void)
{
	PL_AIRPLAY_Context_t *__context = __airplay_context();
	HOM_FUNC_ENTER;
	do_issuing_event(__airplay_context(), PL_AIRPLAY_OP_STOP, NULL, 0);
	__context->ulActivePlayCnt--;
	__context->ulLastPlayProcessTick = 0x0;
	HOM_FUNC_LEAVE;
	return;
}

static void cb_showphoto(uint8_t * data, uint32_t size)
{
	FILE *fp = NULL;
	char temp_filename[128];
	HBOOL bGotImage = FALSE;
	HOM_FUNC_ENTER;

	HxLOG_Error("[ %s:%d ] got Image... \n",__MODULE__, __LINE__);

	/* bzero */
	HxSTD_memset(temp_filename, 0, 128);

#ifdef __DONT_USE_THIS_RAND_FUNCTION__ 
	/* For Random Number */
	srand(time(NULL));
#endif

	/* remove previous images */
	system("rm -f /tmp/airplay_pohto*");

	/* the data will free after this call...  */
	if ( size > 3 && data )
	{
		if ( (data[0] == 'P') && (data[1] == 'N') && (data[2] == 'G') )
		{
#ifdef __DONT_USE_THIS_RAND_FUNCTION__ 
			HxSTD_snprintf(temp_filename, 128, AIRPLAY_PHOTO_FILE_PNG, rand());
#else
			HxSTD_snprintf(temp_filename, 128, AIRPLAY_PHOTO_FILE_PNG, __get_random()); 
#endif

			fp = fopen( temp_filename , "wb" );
			if (fp != NULL)
			{
			fwrite(data, 1, size, fp);
				fclose(fp); fp = NULL;
				bGotImage = TRUE;
			}
		}
		else
		{
#ifdef __DONT_USE_THIS_RAND_FUNCTION__ 
			HxSTD_snprintf( temp_filename, 128, AIRPLAY_PHOTO_FILE_JPG, rand());
#else
			HxSTD_snprintf(temp_filename, 128, AIRPLAY_PHOTO_FILE_JPG, __get_random());
#endif

			fp = fopen( temp_filename , "wb" );
			if (fp != NULL)
			{
			fwrite(data, 1, size, fp);
				fclose(fp); fp = NULL;
				bGotImage = TRUE;
			}
		}

		if (bGotImage == TRUE)
		{
		do_issuing_event(__airplay_context(), PL_AIRPLAY_OP_SHOWPHOTO, temp_filename, 0);
	}
	else
	{
			HxLOG_Error("Error> Cant get image path\n");
		}
	}
	else
	{
		HxLOG_Error("[ %s:%d ] Error> No Image... \n",__MODULE__, __LINE__);
	}
	HOM_FUNC_LEAVE;
}

static double cb_get_duration(void)
{
	switch(do_cache_validation(__airplay_context()))
	{
		case eCACHE_UPDATED:
		case eCACHE_VALID:
			return (__airplay_context()->tPlaybackInfoCache.ullDuration / (double)1000.);
		case eCACHE_NO_CALLBACK:
		case eCACHE_TIMEOUT:
		default:
			return 0.0;
			/* break; */
	}
}

static double cb_get_position(void)
{
	switch(do_cache_validation(__airplay_context()))
	{
		case eCACHE_UPDATED:
		case eCACHE_VALID:
			return (__airplay_context()->tPlaybackInfoCache.ullPosition / (double)1000.);
			/* break; */
		case eCACHE_NO_CALLBACK:
		case eCACHE_TIMEOUT:
		default:
			return 0.0;
			/* break; */
	}
}

static double cb_get_rate(void)
{
	switch(do_cache_validation(__airplay_context()))
	{
		case eCACHE_UPDATED:
		case eCACHE_VALID:
			return (__airplay_context()->tPlaybackInfoCache.unRate / (double)100.);
			/* break; */
		case eCACHE_NO_CALLBACK:
		case eCACHE_TIMEOUT:
		default:
			return 0.0;
			/* break; */
	}
}

static int8_t cb_get_ready_to_play(void)
{
	switch(do_cache_validation(__airplay_context()))
	{
		case eCACHE_UPDATED:
		case eCACHE_VALID:
			if(__airplay_context()->tPlaybackInfoCache.ePlaybackStatus == PL_AIRPLAY_PLAYBACK_PLAYING)
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}
			/* break; */
		case eCACHE_NO_CALLBACK:
		case eCACHE_TIMEOUT:
		default:
			return FALSE;
			/* break; */
	}
}

static AIRPLAY_PLAYBACK_STATE_t cb_get_playback_state(void)
{
//	AIRPLAY_PLAYBACK_STATE_t tPlaybackState; /*  for warning  */

	switch(do_cache_validation(__airplay_context()))
	{
		case eCACHE_UPDATED:
		case eCACHE_VALID:
			switch(__airplay_context()->tPlaybackInfoCache.ePlaybackStatus)
			{
				case PL_AIRPLAY_PLAYBACK_NONE:
					return AIRPLAY_PLAYBACK_NONE;
				case PL_AIRPLAY_PLAYBACK_PAUSE:
					return AIRPLAY_PLAYBACK_PAUSE;
				case PL_AIRPLAY_PLAYBACK_PLAYING:
					return AIRPLAY_PLAYBACK_PLAYING;
				case PL_AIRPLAY_PLAYBACK_REACHED_EOF:
					return AIRPLAY_PLAYBACK_REACHED_EOF;
				case PL_AIRPLAY_PLAYBACK_LOADING:
					return AIRPLAY_PLAYBACK_LOADING;
				case PL_AIRPLAY_PLAYBACK_STOPPED:
					return AIRPLAY_PLAYBACK_STOPPED;
				case PL_AIRPLAY_PLAYBACK_ERROR:
					return AIRPLAY_PLAYBACK_ERROR;
				default:
					break;
			}
			/* break; */
		case eCACHE_NO_CALLBACK:
		case eCACHE_TIMEOUT:
		default:
			break;
			/* break; */
	}
	return AIRPLAY_PLAYBACK_NONE;
}

static AIRPLAY_PBINFO_RANGE_t cb_get_loaded_time_range(void)
{
	AIRPLAY_PBINFO_RANGE_t range = {0.0,0.0};
	switch(do_cache_validation(__airplay_context()))
	{
		case eCACHE_UPDATED:
		case eCACHE_VALID:
			range.start = (double)(__airplay_context()->tPlaybackInfoCache.ullStartPosition / (double)1000.);
			range.end = (double)(__airplay_context()->tPlaybackInfoCache.ullBufferedRange / (double)1000.);
			/* break; */
		case eCACHE_NO_CALLBACK:
		case eCACHE_TIMEOUT:
		default:
			return range;
			/* break; */
	}
}

static AIRPLAY_PBINFO_RANGE_t cb_get_seekable_time_range(void)
{
	AIRPLAY_PBINFO_RANGE_t range = {0.0,0.0};
	switch(do_cache_validation(__airplay_context()))
	{
		case eCACHE_UPDATED:
		case eCACHE_VALID:
			range.start = (double)(__airplay_context()->tPlaybackInfoCache.ullStartPosition / (double)1000.);
			range.end = (double)(__airplay_context()->tPlaybackInfoCache.ullSeekableRange / (double)1000.);
			/* break; */
		case eCACHE_NO_CALLBACK:
		case eCACHE_TIMEOUT:
		default:
			return range;
			/* break; */
	}
}

#define ________________Public_Functions___________________________
/* Init AIRPLAY Context */
HERROR	PL_AIRPLAY_Init( void )
{
	PL_AIRPLAY_Context_t *hAIRPLAY = __airplay_context();
	AIRPLAY_RETCODE	nRet = AIRPLAY_OK;
	AIRPLAY_PBINFOCB_t pbinfo_cb = { cb_get_playback_state, cb_get_duration, cb_get_position, cb_get_rate,
									cb_get_ready_to_play, NULL, NULL, NULL,
									cb_get_loaded_time_range, cb_get_seekable_time_range };

	#ifdef LIBAIRPLAY_VERSION
#if	LIBAIRPLAY_VERSION >= VERSION(1,0,5)
	AIRTUNES_EVTCB_t event_cb_airtuens = { cb_airtunes_play, cb_airtunes_flush, cb_airtunes_stop, cb_airtunes_set_albumart, cb_airtunes_set_metadata};
	AIRPLAY_EVTCB_t event_cb = { cb_play, cb_setpos, cb_setrate, cb_stop, cb_showphoto, NULL, NULL };
#else
	AIRPLAY_EVTCB_t event_cb = { cb_play, cb_setpos, cb_setrate, cb_stop, NULL, NULL };
#endif
#endif
	HINT32 vkRet = 0;
	HBOOL bStartPublisher = FALSE;
	HBOOL bStartHTTPD = FALSE;
	HBOOL bStartRTSPD = FALSE;
	HOM_FUNC_ENTER;

#ifdef LIBAIRPLAY_VERSION
	HxLOG_Debug("[ %s:%d ] ################################################\n", __MODULE__, __LINE__);
	HxLOG_Debug("[ %s:%d ]      AIRPLAY Init	\n", __MODULE__, __LINE__);
	HxLOG_Debug("[ %s:%d ]         with libairplay Version : %d.%d.%d\n ", __MODULE__, __LINE__, LIBAIRPLAY_MAJOR_VERSION, LIBAIRPLAY_MINOR_VERSION, LIBAIRPLAY_REV_VERSION);
	HxLOG_Debug("[ %s:%d ] ################################################\n", __MODULE__, __LINE__);
#endif

	if( hAIRPLAY->bAirPlayInit == FALSE )
	{
#ifdef LIBAIRPLAY_VERSION
#if LIBAIRPLAY_VERSION >= VERSION(1,0,4)
		nRet = AIRPLAY_RegisterLogCallback(do_airplay_log_cb);
#else
		nRet = AIRPLAY_PUBLISHER_RegisterLogCallback(do_airplay_log_cb);
#endif
#else
		nRet = AIRPLAY_PUBLISHER_RegisterLogCallback(do_airplay_log_cb);
#endif
		if(nRet != AIRPLAY_OK)
		{
			HxLOG_Error("[ %s:%d ] ERR> Failed to register airplay log callback ret = %d\n",__MODULE__, __LINE__, nRet);
			return ERR_FAIL;
		}

		/* Set service feature */
		SET_FEATURE(hAIRPLAY->nAirFeatures,AIRPLAY_FEATURE_VIDEO);
		SET_FEATURE(hAIRPLAY->nAirFeatures,AIRPLAY_FEATURE_PHOTO);
		SET_FEATURE(hAIRPLAY->nAirFeatures,AIRPLAY_FEATURE_SCREEN);
#if 1
		//SET_FEATURE(hAIRPLAY->nAirFeatures,AIRPLAY_FEATURE_VIDEOVOLUMECONTROL);
		SET_FEATURE(hAIRPLAY->nAirFeatures,AIRPLAY_FEATURE_VIDEOFAIRPLAY);
		SET_FEATURE(hAIRPLAY->nAirFeatures,AIRPLAY_FEATURE_SLIDESHOW); /* PUT /slideshows/1 */
#endif
		SET_FEATURE(hAIRPLAY->nAirFeatures,AIRPLAY_FEATURE_VIDEOHTTPLIVESTREAMS);
		SET_FEATURE(hAIRPLAY->nAirFeatures,AIRPLAY_FEATURE_UNKNOWN1);
	
		//hAIRPLAY->nAirFeatures = 0x77;

#ifdef LIBAIRPLAY_VERSION
#if LIBAIRPLAY_VERSION >= VERSION(1,0,7)
        AIRPLAY_SetServiceInformation(hAIRPLAY->tServiceParam.szMacAddr, hAIRPLAY->tServiceParam.aServiceName,
						                                             hAIRPLAY->tServiceParam.aDeviceModel, hAIRPLAY->tServiceParam.szIpAddr, hAIRPLAY->nAirFeatures);
#else
		AIRPLAY_SetServiceInformation(hAIRPLAY->tServiceParam.szMacAddr, hAIRPLAY->tServiceParam.aServiceName,
  											 hAIRPLAY->tServiceParam.aDeviceModel, hAIRPLAY->nAirFeatures);
#endif
#endif
		/* HTTPD related service */
#ifdef LIBAIRPLAY_VERSION
#if	LIBAIRPLAY_VERSION >= VERSION(1,0,3)
		nRet = AIRPLAY_SERVER_Init(hAIRPLAY->tServiceParam.nHttpServicePort, hAIRPLAY->tServiceParam.nRtspServicePort);
#endif
#else
		nRet = AIRPLAY_SERVER_Init(hAIRPLAY->tServiceParam.nHttpServicePort);
#endif
		if(nRet != AIRPLAY_OK)
		{
			HxLOG_Error("[ %s:%d ] ERR> Failed to init airplay receiver\n",__MODULE__, __LINE__);
			return ERR_FAIL;
		}

		/* Playback info callback, AIRPLAY will be request current playback information via below callback */
		nRet = AIRPLAY_RegisterPlaybackInfoCallback(pbinfo_cb);
		if(nRet != AIRPLAY_OK)
		{
			HxLOG_Error("[ %s:%d ] ERR> Failed to register airplay playback info callback\n",__MODULE__, __LINE__);
			return ERR_FAIL;
		}

#ifdef LIBAIRPLAY_VERSION
#if	LIBAIRPLAY_VERSION >= VERSION(1,0,5)
		/* Register event callback : must perfrom this after AIRPLAY_RECEIVER_Init */
		nRet = AIRPLAY_SERVER_RegisterEventCallback(event_cb, event_cb_airtuens);
		if(nRet != AIRPLAY_OK)
		{
			HxLOG_Error("[ %s:%d ] ERR> Failed to register airplay event callback\n",__MODULE__, __LINE__);
			return ERR_FAIL;
		}
#else
		/* Register event callback : must perfrom this after AIRPLAY_RECEIVER_Init */
		nRet = AIRPLAY_SERVER_RegisterEventCallback(event_cb);
		if(nRet != AIRPLAY_OK)
		{
			HxLOG_Error("[ %s:%d ] ERR> Failed to register airplay event callback\n",__MODULE__, __LINE__);
			return ERR_FAIL;
		}
#endif
#endif
		hAIRPLAY->eHTTPD_STATUS = eSERVER_READY;
		hAIRPLAY->eRTSPD_STATUS = eSERVER_READY;
		hAIRPLAY->ePUBLISHER_STATUS = ePUBLISHER_READY;

		/* Create Publisher */
		if (hAIRPLAY->nPublisherTaskId != 0x0)
		{
			HxLOG_Error("[ %s:%d ] Error> invalid publisher tid(%d)\n",__MODULE__, __LINE__, hAIRPLAY->nPublisherTaskId);
		}
		vkRet = VK_TASK_Create(do_publisher_task, VK_TASK_PRIORITY_MW_REF, 512*1024, "airplay_publisher", NULL,
								(long unsigned int*)&hAIRPLAY->nPublisherTaskId, 0);
		if(vkRet != VK_OK)
		{
			HxLOG_Error("[ %s:%d ] ERR> Failed to create publisher\n",__MODULE__, __LINE__);
			hAIRPLAY->ePUBLISHER_STATUS = ePUBLISHER_ERROR;
			goto __INIT_ERR;
		}

		/* Create HTTPD */
		if (hAIRPLAY->nHTTPDTaskId != 0x0)
		{
			HxLOG_Error("[ %s:%d ] Error> invalid httpd tid(%d)\n",__MODULE__, __LINE__, hAIRPLAY->nHTTPDTaskId);
		}
		vkRet = VK_TASK_Create(do_httpd_task, VK_TASK_PRIORITY_MW_REF, 512*1024, "airplay_httpd", NULL,
								(long unsigned int*)&hAIRPLAY->nHTTPDTaskId, 0);
		if(vkRet != VK_OK)
		{
			HxLOG_Error("[ %s:%d ] ERR> Failed to create httpd\n",__MODULE__, __LINE__);
			hAIRPLAY->eHTTPD_STATUS = eSERVER_ERROR;
			goto __INIT_ERR;
		}

#ifdef LIBAIRPLAY_VERSION
#if	LIBAIRPLAY_VERSION >= VERSION(1,0,3)
		/* Create HTTPD */
		if (hAIRPLAY->nRTSPDTaskId != 0x0)
		{
			HxLOG_Error("[ %s:%d ] Error> invalid rtspd tid(%d)\n",__MODULE__, __LINE__, hAIRPLAY->nRTSPDTaskId);
		}
		vkRet = VK_TASK_Create(do_rtspd_task, VK_TASK_PRIORITY_MW_REF, 512*1024, "airplay_rtspd", NULL,
								(long unsigned int*)&hAIRPLAY->nRTSPDTaskId, 0);
		if(vkRet != VK_OK)
		{
			HxLOG_Error("[ %s:%d ] ERR> Failed to create httpd\n",__MODULE__, __LINE__);
			hAIRPLAY->eRTSPD_STATUS = eSERVER_ERROR;
			goto __INIT_ERR;
		}
#endif
#endif
		/* Start Publisher */
		vkRet = VK_TASK_Start(hAIRPLAY->nPublisherTaskId);
		if(vkRet != VK_OK)
		{
			HxLOG_Error("[ %s:%d ] ERR> Failed to start publisher\n",__MODULE__, __LINE__);
			hAIRPLAY->ePUBLISHER_STATUS = ePUBLISHER_ERROR;
			goto __INIT_ERR;
		}
		bStartPublisher = TRUE;

		/* Start HTTPD */
		vkRet = VK_TASK_Start(hAIRPLAY->nHTTPDTaskId);
		if(vkRet != VK_OK)
		{
			HxLOG_Error("[ %s:%d ] ERR> Failed to start httpd\n",__MODULE__, __LINE__);
			hAIRPLAY->eHTTPD_STATUS = eSERVER_ERROR;
			goto __INIT_ERR;
		}

#ifdef LIBAIRPLAY_VERSION
#if	LIBAIRPLAY_VERSION >= VERSION(1,0,3)
		/* Start RTSPD */
		vkRet = VK_TASK_Start(hAIRPLAY->nRTSPDTaskId);
		if(vkRet != VK_OK)
		{
			HxLOG_Error("[ %s:%d ] ERR> Failed to start httpd\n",__MODULE__, __LINE__);
			hAIRPLAY->eHTTPD_STATUS = eSERVER_ERROR;
			goto __INIT_ERR;
		}
#endif
#endif
		bStartHTTPD = TRUE;
		bStartRTSPD = TRUE;
		hAIRPLAY->bAirPlayInit = TRUE;
	}
	else
	{
		HxLOG_Warning("[ %s:%d ] Airplay publisher is already initialized\n",__MODULE__, __LINE__);
	}
	HOM_FUNC_LEAVE;
	return ERR_OK;

__INIT_ERR:
	/* Stop task */
	if(bStartPublisher)
	{
		if(VK_OK != VK_TASK_Stop(hAIRPLAY->nPublisherTaskId))
		{
			HxLOG_Error("[ %s:%d ] Error> Failed to stop publisher\n",__MODULE__, __LINE__);
		}
	}
	if(bStartHTTPD)
	{
		if(VK_OK != VK_TASK_Stop(hAIRPLAY->nHTTPDTaskId))
		{
			HxLOG_Error("[ %s:%d ] Error> Failed to stop httpd\n",__MODULE__, __LINE__);
		}
	}

	if(bStartRTSPD)
	{
#ifdef LIBAIRPLAY_VERSION
#if	LIBAIRPLAY_VERSION >= VERSION(1,0,3)
		if(VK_OK != VK_TASK_Stop(hAIRPLAY->nRTSPDTaskId))
		{
			HxLOG_Error("[ %s:%d ] Error> Failed to stop httpd\n",__MODULE__, __LINE__);
		}
#endif
#endif
	}
	
	/* Destroy task */
	if(hAIRPLAY->nPublisherTaskId != 0)
	{
		if(VK_OK != VK_TASK_Destroy(hAIRPLAY->nPublisherTaskId))
		{
			HxLOG_Error("[ %s:%d ] Error> Failed to destroy publisher\n",__MODULE__, __LINE__);
		}
		hAIRPLAY->nPublisherTaskId = 0x0;
	}
	if(hAIRPLAY->nHTTPDTaskId != 0)
	{
		if(VK_OK != VK_TASK_Destroy(hAIRPLAY->nHTTPDTaskId))
		{
			HxLOG_Error("[ %s:%d ] Error> Failed to destroy httpd\n",__MODULE__, __LINE__);
		}
		hAIRPLAY->nHTTPDTaskId = 0x0;
	}
	HOM_FUNC_LEAVE;
	return ERR_FAIL;
}

/* Deinit AIRPLAY */
HERROR	PL_AIRPLAY_DeInit( void )
{
	AIRPLAY_RETCODE	nRet = AIRPLAY_OK;
	HINT32			vkRet;
	HINT32			nCurDelay = 0;
	const HINT32	nMaxDelay = 5000;	/* 5(sec) */
	PL_AIRPLAY_Context_t *hAIRPLAY = __airplay_context();
	HOM_FUNC_ENTER;

	if ( hAIRPLAY->bAirPlayInit == TRUE )
	{
		if( hAIRPLAY->bAirPlayStarted == TRUE)
		{
			HxLOG_Error("[ %s:%d ] Error> Airplay has not beed stopped, do stop first\n",__MODULE__, __LINE__);
			return ERR_FAIL;
		}

		if(hAIRPLAY->nPublisherTaskId != 0)
		{
			if(hAIRPLAY->ePUBLISHER_STATUS != ePUBLISHER_QUIT_OK)
			{
				hAIRPLAY->ePUBLISHER_STATUS = ePUBLISHER_QUIT;
			}
			while(hAIRPLAY->ePUBLISHER_STATUS != ePUBLISHER_QUIT_OK)
			{
				if(nCurDelay >= nMaxDelay)
				{
					HxLOG_Error("[ %s:%d ] Error> Cannot terminate publisher task...\n",__MODULE__, __LINE__);
					return ERR_FAIL;
				}
				nCurDelay += 100;
				msleep(100);
			}
			vkRet = VK_TASK_Destroy(hAIRPLAY->nPublisherTaskId);
			if(vkRet != VK_OK)
			{
				HxLOG_Error("[ %s:%d ] Error> Failed to destroy publisher\n",__MODULE__, __LINE__);
				return ERR_FAIL;
			}
			hAIRPLAY->nPublisherTaskId = 0;
		}
		nCurDelay = 0;

		if(hAIRPLAY->nHTTPDTaskId != 0)
		{
			if(hAIRPLAY->eHTTPD_STATUS != eSERVER_QUIT_OK)
			{
				hAIRPLAY->eHTTPD_STATUS = eSERVER_QUIT;
			}
			while(hAIRPLAY->eHTTPD_STATUS != eSERVER_QUIT_OK)
			{
				if(nCurDelay >= nMaxDelay)
				{
					HxLOG_Error("[ %s:%d ] Error> Cannot terminate httpd task...\n",__MODULE__, __LINE__);
					return ERR_FAIL;
				}
				nCurDelay += 100;
				msleep(100);
			}
			vkRet = VK_TASK_Destroy(hAIRPLAY->nHTTPDTaskId);
			if(vkRet != VK_OK)
			{
				HxLOG_Error("[ %s:%d ] Error> Failed to destroy httpd\n",__MODULE__, __LINE__);
				return ERR_FAIL;
			}
			hAIRPLAY->nHTTPDTaskId = 0;
		}
#ifdef LIBAIRPLAY_VERSION
#if	LIBAIRPLAY_VERSION >= VERSION(1,0,3)
		if(hAIRPLAY->nRTSPDTaskId != 0)
		{
			if(hAIRPLAY->eRTSPD_STATUS != eSERVER_QUIT_OK)
			{
				hAIRPLAY->eRTSPD_STATUS = eSERVER_QUIT;
			}
			while(hAIRPLAY->eRTSPD_STATUS != eSERVER_QUIT_OK)
			{
				if(nCurDelay >= nMaxDelay)
				{
					HxLOG_Error("[ %s:%d ] Error> Cannot terminate httpd task...\n",__MODULE__, __LINE__);
					return ERR_FAIL;
				}
				nCurDelay += 100;
				msleep(100);
			}
			vkRet = VK_TASK_Destroy(hAIRPLAY->nRTSPDTaskId);
			if(vkRet != VK_OK)
			{
				HxLOG_Error("[ %s:%d ] Error> Failed to destroy rtspd\n",__MODULE__, __LINE__);
				return ERR_FAIL;
			}
			hAIRPLAY->nRTSPDTaskId = 0;
		}
#endif
#endif
		/* HTTPD related service */
		nRet = AIRPLAY_SERVER_UnregisterEventCallback();
		if(nRet != AIRPLAY_OK)
		{
			HxLOG_Error("[ %s:%d ] Error> Failed to uninstall airplay event callback\n",__MODULE__, __LINE__);
			return ERR_FAIL;
		}
		nRet = AIRPLAY_SERVER_Uninit();
		if(nRet != AIRPLAY_OK)
		{
			HxLOG_Error("[ %s:%d ] Error> Failed to uninit airplay server\n",__MODULE__, __LINE__);
			return ERR_FAIL;
		}

		hAIRPLAY->bAirPlayInit = FALSE;
	}
	else
	{
		HxLOG_Print( "[ %s:%d ] Not yet init", __MODULE__, __LINE__);
		return ERR_FAIL;
	}
	HOM_FUNC_LEAVE;
	return ERR_OK;
}

HERROR	PL_AIRPLAY_Start( void)
{
	PL_AIRPLAY_Context_t *hAIRPLAY = __airplay_context();
	HINT32			nCurDelay = 0;
	const HINT32	nMaxDelay = 5000;	/* 5(sec) */
	HOM_FUNC_ENTER;

	if( hAIRPLAY->bAirPlayInit == TRUE)
	{
		if(hAIRPLAY->nPublisherTaskId == 0 || hAIRPLAY->nHTTPDTaskId == 0)
		{
			HxLOG_Error("[ %s:%d ] Error> Airplay task is not created\n",__MODULE__, __LINE__);
			goto func_error;
		}
		if(hAIRPLAY->bAirPlayStarted)
		{
			HxLOG_Error("[ %s:%d ] Error> Airplay task is already started\n",__MODULE__, __LINE__);
			goto func_error;
		}
		if (hAIRPLAY->ePUBLISHER_STATUS != ePUBLISHER_RUNNING)
			hAIRPLAY->ePUBLISHER_STATUS	= ePUBLISHER_RUNNING_REQ;
		if (hAIRPLAY->eHTTPD_STATUS != eSERVER_RUNNING)
			hAIRPLAY->eHTTPD_STATUS = eSERVER_RUNNING_REQ;
		if (hAIRPLAY->eRTSPD_STATUS !=  eSERVER_RUNNING)
			hAIRPLAY->eRTSPD_STATUS = eSERVER_RUNNING_REQ;
		hAIRPLAY->bAirPlayStarted 	= TRUE;

		while(hAIRPLAY->ePUBLISHER_STATUS != ePUBLISHER_RUNNING)
		{
			if(nCurDelay >= nMaxDelay)
			{
				HxLOG_Error("[ %s:%d ] Error> Cannot running publisher task...\n",__MODULE__, __LINE__);
				break;
			}
			nCurDelay += 50;
			msleep(50);
		}
		nCurDelay = 0;
		while(hAIRPLAY->eHTTPD_STATUS != eSERVER_RUNNING)
		{
			if(nCurDelay >= nMaxDelay)
			{
				HxLOG_Error("[ %s:%d ] Error> Cannot running httpd task...\n",__MODULE__, __LINE__);
				break;
			}
			nCurDelay += 50;
			msleep(50);
		}
		nCurDelay = 0;
#ifdef LIBAIRPLAY_VERSION
#if	LIBAIRPLAY_VERSION >= VERSION(1,0,3)
		while(hAIRPLAY->eRTSPD_STATUS != eSERVER_RUNNING)
		{
			if(nCurDelay >= nMaxDelay)
			{
				HxLOG_Error("[ %s:%d ] Error> Cannot running rtspd task...\n",__MODULE__, __LINE__);
				break;
			}
			nCurDelay += 50;
			msleep(50);
		}
#endif
#endif
	}
	else
	{
		HxLOG_Print("[ %s:%d ] Not yet init", __MODULE__,__LINE__);
		goto func_error;
	}
	HOM_FUNC_LEAVE;
	return ERR_OK;

func_error:
	HOM_FUNC_LEAVE;
	return ERR_OK;
}

HERROR	PL_AIRPLAY_Stop( void)
{
	PL_AIRPLAY_Context_t *hAIRPLAY = __airplay_context();
	AIRPLAY_RETCODE nRet = AIRPLAY_OK;
	HOM_FUNC_ENTER;

	if( hAIRPLAY->bAirPlayInit == TRUE)
	{
		if(hAIRPLAY->nPublisherTaskId == 0 || hAIRPLAY->nHTTPDTaskId == 0)
		{
			HxLOG_Error("[ %s:%d ] Error> Airplay task is not created\n",__MODULE__, __LINE__);
			goto func_error;
		}
		if(hAIRPLAY->bAirPlayStarted == FALSE)
		{
			HxLOG_Error("[ %s:%d ] Error> Airplay task is not started\n",__MODULE__, __LINE__);
			goto func_error;
		}

		if(hAIRPLAY->bRequestMediaPlay == TRUE)
		{
			HxLOG_Error("[ %s:%d ] Error> Just requested media play, try stop later\n",__MODULE__, __LINE__);
			goto func_error;
		}

		hAIRPLAY->ePUBLISHER_STATUS = ePUBLISHER_READY;
		hAIRPLAY->eHTTPD_STATUS = eSERVER_READY;
		hAIRPLAY->eRTSPD_STATUS = eSERVER_READY;

		nRet = AIRPLAY_PUBLISHER_Stop();
		if(nRet != AIRPLAY_OK)
		{
			HxLOG_Error("[ %s:%d ] Error> Failed to stop publisher\n",__MODULE__, __LINE__);
			goto func_error;
		}

		nRet = AIRPLAY_SERVER_Stop();
		if(nRet != AIRPLAY_OK)
		{
			HxLOG_Error("[ %s:%d ] Error> Failed to stop httpd\n",__MODULE__, __LINE__);
			goto func_error;
		}

		hAIRPLAY->bAirPlayStarted = FALSE;
	}
	else
	{
		HxLOG_Error("[ %s:%d ] Error> Not yet init", __MODULE__,__LINE__);
		goto func_error;
	}

	HOM_FUNC_LEAVE;
	return ERR_OK;
	
func_error:
	HOM_FUNC_LEAVE;
	return ERR_OK;
}

/* Registration Callback */
HERROR	PL_AIRPLAY_RegisterEventCB(PL_AIRPLAY_OperationCallback cb_op)
{
	PL_AIRPLAY_Context_t *hAIRPLAY = __airplay_context();
	if (cb_op == NULL)
	{
		HxLOG_Error("[ %s:%d ] Error> NULL cb!\n",__MODULE__, __LINE__);
		hAIRPLAY->cbOperation = NULL;
	}
	else
	{
		hAIRPLAY->cbOperation = cb_op;
	}
	return ERR_OK;
}

/* UnRegistration Callback */
HERROR	PL_AIRPLAY_UnRegisterEventCB( void )
{
	PL_AIRPLAY_Context_t *hAIRPLAY = __airplay_context();
	hAIRPLAY->cbOperation = NULL;
	return ERR_OK;
}

HBOOL	PL_AIRPLAY_IsStarted( void )
{
	PL_AIRPLAY_Context_t *hAIRPLAY = __airplay_context();
	return ( hAIRPLAY->bAirPlayStarted) ? TRUE : FALSE;
}

HERROR	PL_AIRPLAY_SetServiceParameter( PL_AIRPLAY_SERVICE_PARAM_t* pServiceParam )
{
	HERROR		hErr = ERR_FAIL;
	PL_COMMON_MacInfo_t mac_info; /* Changed from PL_COMMON_NetworkInfo_t() */
	PL_COMMON_NetworkInfo_t network_info;
	PL_COMMON_ProductInfo_t	product_info;
	PL_AIRPLAY_Context_t *hAIRPLAY = __airplay_context();

	HxSTD_MemSet(&mac_info, 0x00, sizeof(PL_COMMON_MacInfo_t));
	HxSTD_MemSet(&network_info, 0x00, sizeof(PL_COMMON_NetworkInfo_t));

	if ( pServiceParam )
	{
		HOM_MEMCOPY( &hAIRPLAY->tServiceParam, pServiceParam, sizeof(PL_AIRPLAY_SERVICE_PARAM_t) );
	}
	else
	{
		/* Set ServiceParams as a default */
		PL_COMMON_GetProductInfo(&product_info);
		hErr = PL_COMMON_GetActiveNetwork(&network_info); /* PL_COMMON_GetDefaultNetwork() */
		if ((hErr != ERR_OK) || (network_info.ulIpAddr == 0x0))
		{
			HxLOG_Error("Error> Cant get active network(IP:0x%x)!\n", network_info.ulIpAddr);
		hErr = PL_COMMON_GetDefaultNetwork(&network_info);
			if ((hErr != ERR_OK) || (network_info.ulIpAddr == 0x0))
		{
				HxLOG_Error("Error> Cant get valid network(IP:0x%x)!\n", network_info.ulIpAddr);
				goto func_error;
			}
		}

#if 1   /* Source from HMS1000xx series. Here, These API does not exist currently */
		hErr = PL_COMMON_GetEthMac(&mac_info); 
		if (hErr != ERR_OK)
		{
			/* It's situation that should not occur absolutely.  */
			HxLOG_Error("Error> Cant get ethernet MAC!\n");

			/* Dont use this; It can be not ensured that the "Default" is same with "ethernet". */
			HxSTD_MemSet(&mac_info, 0x0, sizeof(PL_COMMON_MacInfo_t));
			hErr = PL_COMMON_GetDefaultMac(&mac_info);
			if (hErr != ERR_OK)
			{
				HxLOG_Error("Error> Cant get default MAC!\n");
				goto func_error;
			}
		}
#else
		hErr = PL_COMMON_GetMacInfo(&mac_info);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("Error> Cant get ethernet MAC!\n");
		}		
#endif

		/* 1. Device infomation */
		HOM_SNPRINTF(hAIRPLAY->tServiceParam.aDeviceModel, MAX_SERVICE_STRING, "%s",
				product_info.szModelName);
		
		/* 2. Service Name */
		HOM_SNPRINTF(hAIRPLAY->tServiceParam.aServiceName, MAX_SERVICE_STRING, "%s(%c%c%c%c%c%c)",
				product_info.szModelName,
				mac_info.szMacAddr[6],
				mac_info.szMacAddr[7],
				mac_info.szMacAddr[8],
				mac_info.szMacAddr[9],
				mac_info.szMacAddr[10],
				mac_info.szMacAddr[11]);

		/* 3. MAC Address  */
		HOM_SNPRINTF(hAIRPLAY->tServiceParam.szMacAddr, 20, "%c%c:%c%c:%c%c:%c%c:%c%c:%c%c",
				mac_info.szMacAddr[0],
				mac_info.szMacAddr[1],
				mac_info.szMacAddr[2],
				mac_info.szMacAddr[3],
				mac_info.szMacAddr[4],
				mac_info.szMacAddr[5],
				mac_info.szMacAddr[6],
				mac_info.szMacAddr[7],
				mac_info.szMacAddr[8],
				mac_info.szMacAddr[9],
				mac_info.szMacAddr[10],
				mac_info.szMacAddr[11]);

		/* 4. IP Address */
		HOM_STRNCPY_SAFE(hAIRPLAY->tServiceParam.szIpAddr, network_info.szIp, 20);

		/* Service Port */
		hAIRPLAY->tServiceParam.nHttpServicePort = 36667;
		hAIRPLAY->tServiceParam.nRtspServicePort = 36666;

		/* basic Network Infomation */
		/* HOM_MEMCOPY( &hAIRPLAY->tNetworkIF, &mac_info, sizeof(PL_COMMON_NetworkInfo_t) ); */
		HOM_MEMCOPY( &hAIRPLAY->tMacInfo, &mac_info, sizeof(PL_COMMON_MacInfo_t) );
	}

	return ERR_OK;

func_error:
	return ERR_FAIL;
}

HULONG	PL_AIRPLAY_GetNumericCurrentIp( void )
{
	HERROR		hErr = ERR_FAIL;
	PL_COMMON_NetworkInfo_t network_info;

	HxSTD_MemSet(&network_info, 0x00, sizeof(PL_COMMON_NetworkInfo_t));
	
	hErr = PL_COMMON_GetActiveNetwork(&network_info);
	if ((hErr != ERR_OK) || (network_info.ulIpAddr == 0x0))
	{
		HxLOG_Error("Error> Cant get active network(IP:0x%x)!\n", network_info.ulIpAddr);
	hErr = PL_COMMON_GetDefaultNetwork(&network_info);
		if ((hErr != ERR_OK) || (network_info.ulIpAddr == 0x0))
	{
			HxLOG_Error("Error> Cant get valid network(IP:0x%x)!\n", network_info.ulIpAddr);
			goto func_error;
		}
	}
	
	if(network_info.bValid == FALSE)
	{
		goto func_error;
	}
	
	return network_info.ulIpAddr;

func_error:
	return 0x0;
}

void	PL_AIRPLAY_SetPlaybackInfo( PL_AIRPLAY_PLAYBACK_INFO_t tPlaybackInfo )
{
	PL_AIRPLAY_Context_t *hAIRPLAY = __airplay_context();
	struct timeval curTime;

	gettimeofday(&curTime, NULL); /* <-- gettimeofday()? tick base? */
	hAIRPLAY->tPlaybackInfoCache = tPlaybackInfo;
	hAIRPLAY->ullInfoLastUpdated = (curTime.tv_sec * 1000) + (curTime.tv_usec / 1000);
	if(hAIRPLAY->unInfoSequenceNum < HUINT32_MAX)
	{
		hAIRPLAY->unInfoSequenceNum++;
	}
	else
	{
		hAIRPLAY->unInfoSequenceNum = 0;
	}
}
/* end of file */
