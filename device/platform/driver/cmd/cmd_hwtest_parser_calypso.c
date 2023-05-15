#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <time.h>

#include "cso.h"
#include "htype.h"
#include "bstd_defs.h"
#include "vkernel.h"
#include "taskdef.h"
#include "cmdtool.h"
#include "di_err.h"
#include "di_media_types.h"
#include "cmd_hwtest_parser_calypso.h"
#include "cmd_hwtest_parser_media_scriptengine.h"
#include "di_audio.h"
#include "di_video.h"
#include "nexus_video_decoder.h"
#include "nexus_audio_decoder.h"
#include "di_media20.h"

#define HWTEST_CMD		szCmd
#define HWTEST_PARAM	szParam
#define HWTEST_PARAM1	szParam1
#define HWTEST_PARAM2	szParam2
#define GET_ARGS		int iResult=CMD_ERR; \
						char *HWTEST_CMD=CMD_NextArg((char **)&szArgList); \
						char *HWTEST_PARAM=CMD_NextArg((char **)&szArgList); \
						char *HWTEST_PARAM1=CMD_NextArg((char **)&szArgList); \
						char *HWTEST_PARAM2=CMD_NextArg((char **)&szArgList);
#define CMD_IS(x)		(HWTEST_CMD!=NULL && VK_strcmp(HWTEST_CMD, x)==0)
#define PARAM_IS(x)		(HWTEST_PARAM!=NULL && VK_strcmp(HWTEST_PARAM, x)==0)
#define PARAM1_IS(x)	(HWTEST_PARAM1!=NULL && VK_strcmp(HWTEST_PARAM1, x)==0)
#define PARAM2_IS(x)	(HWTEST_PARAM2!=NULL && VK_strcmp(HWTEST_PARAM2, x)==0)
#define READABLE_IN_HEX(sz,val) CMD_ReadAsHex(sz, &val)
#define READABLE_IN_DEC(sz,val) CMD_ReadAsDec(sz, &val)

#define GET_NEXT_ARG(variable)		char *variable = CMD_NextArg((char **)&szArgList)

#define AssertDI(arg1, arg2) if(arg1 != DI_ERR_OK) {CMD_Printf(\
					"ERROR code : 0x%x, At %s() function, FILE: %s, LINE: %d\n",arg1, __func__, __FILE__,__LINE__); return arg2;}

/********************
*	 test related
*********************/
#define MAX_SESSION 100
#define CMD_MEDIA_MAX_CMD 1024
#define CMD_MEDIA_MAX_ASSET 64
#define CMD_MEDIA_KEYBUFSIZE 2048

#define LOG_TARGET_FILE

#ifdef LOG_TARGET_FILE
#define LOGMSG(...) 						\
	{ FILE *logfp = fopen("/test.log","a+");	\
	if(logfp==NULL) exit(1);					\
	VK_fprintf(logfp,__VA_ARGS__);				\
	VK_fflush(logfp); 							\
	CMD_Printf(__VA_ARGS__);				\
	fclose(logfp); 							\
	}
#else
#define LOGMSG(...) CMD_Printf(__VA_ARGS__)
#endif

#define CSO_BUFFERING_PATH_HDD		/* 정의되지 않으면 NFS 경로 (/home/[bufferingfile])로 버퍼링 파일을 생성 */
//#define CSO_STRESS_TEST				/* script test 시 scenario를 반복함 */
//#define CSO_MEMORY_DEBUG			/* memory leakage 검사 수행 및 case 시작 시 system memory 현황을 출력 */
#ifdef CSO_MEMORY_DEBUG
#include "util_memtool.h"
#endif

//#define HLS_DISCONTINUITY
//test mode and part
enum { CSO_PLAYMODE_DNP = 0, CSO_PLAYMODE_RT, CSO_PLAYMODE_ADAPTIVE, NR_CSO_PLAYMODE };
//sub command of trick
enum { CSO_TRICK_FF, CSO_TRICK_RW, CSO_TRICK_PAUSE, CSO_TRICK_SEEK, NR_CSO_TRICK};
//state
enum { CSO_STATE_NOT_PLAYING, CSO_STATE_PLAYING, NR_CSO_STATE };

typedef struct CSO_CMDSET_s {
	CMD_MEDIA_LINE_t cmd;
	HUINT64 time;
} CSO_CMDSET_t;

typedef struct CSO_INI_s {
	HBOOL bLoad;
	HUINT8 *pUrl;
	HINT32 nPlayMode;
	/* cmd manage */
	HUINT32 nCmdCursor;
	CSO_CMDSET_t CmdSet[CMD_MEDIA_MAX_CMD];
	/* URL List */
	HUINT32 nUrlCnt;
	HUINT8 *urls[CMD_MEDIA_MAX_ASSET];
} CSO_INI_t;

typedef struct CSO_INFO_s {
	HUINT8 *url;
	HUINT32 ulSessionId;
	HUINT32 uiPumpId;
//	SESSION_HANDLE hSession;
	HUINT8 unMediaId;
	CSO_ProbeInfo_t ProbeInfo;
//	CSO_BufferCallback_t BufferCallback;
//	CSO_StreamEventCallback_t StreamEventCallback;
	HUINT8 *ucBufferPath;
	bool isOpened;
	bool isPlaying;
	bool isBuffering;

#ifdef HLS_DISCONTINUITY
	bool bNeedRestart;
	bool bEofReached;
#endif
	unsigned long long ullPosition;
	int nTrick;
	bool isSeek;

}CSO_INFO_t;


typedef struct TASK_MSG_COMMAND {
	HINT32	MSGTYPE;
	HINT32	PARAM1;
	HINT32	PARAM2;
}CMD_TASK_MSG_t;

typedef struct timeHandle_s
{
    HBOOL bStart;
    char pString[50];
    struct timeval begin;
    struct timeval finish;
} timeHandle_t;

typedef struct
{
	void* pPlaypumpList;
	void* pDmaList;
	HUINT32 uiNumOfPlaypump;
	HUINT32 uiNumOfDma;
} CMD_MEDIA_Nexus_Resource_t;

//global variable
static CSO_INFO_t *g_curCso = NULL;
static CSO_INI_t *g_pIni = NULL;
static CSO_CMDSET_t cmdset;
static HINT8 g_keyBuf[CMD_MEDIA_KEYBUFSIZE]={0,};
static HINT32 g_nCmdTaskId;
static HINT32 g_nScriptTaskId;
static HINT32 g_nEventTaskId;
static HUINT8 g_nNRSession = 1;
static HINT8  g_nCurSessionCount = 0;
static bool   g_bMediaEventWaiting=false;
static bool   g_bMediaPlaying=false;
static bool   g_bFlagEOS=false;
static HINT32 g_nEOSmediaId;
static HUINT8 *g_ucScriptPath;
static timeHandle_t	start = {FALSE, "create~start"};
static timeHandle_t	probe = {FALSE, "probe"};
static HUINT8 nMediaTracks;
static HBOOL g_bCsoInit = FALSE;
static CSO_Settings_t g_CsoSettings;
static CMD_MEDIA_Nexus_Resource_t s_ResourceMap;
static void *gpHSession;
static HBOOL g_bProbeFailure[MAX_SESSION] = {0,};

#define DEFAULT_CSO_DOWNLOAD_PATH "/mnt/hd3/content-download"
#define DEFAULT_CSO_INI_PATH "/cso.ini"

#define IS_CSO_PLAYING (g_nCurSessionCount != CSO_STATE_NOT_PLAYING)

static unsigned long long _atohui64(char *nptr)
{
	char *s = (char *)nptr;
	unsigned long long acc = 0;
	while(isspace((int)*s))
		s++;
	while (VK_isdigit((int)*s))
	{
		acc = 10 * acc + ((int)*s - '0');
		s++;
	}
	return acc;
}

static void local_Media_Callback(HUINT32 ulDeviceId, HUINT32 ulEvent, HUINT32 Value);
static void __cmd_hwtest_media_audio_callback(void *pvParam);
static void __cmd_hwtest_media_video_callback(void *pvParam);
static DI_ERR_CODE P_MediaTrick(HUINT8 TRICKCMD, HUINT64 uiSpeed, HUINT32 unSessionId, HUINT32 unMediaId);

extern CMD_MEDIA_TAG_TABLE_t g_TagTable[];

static int P_CONVERT_DI_AUDIO_CODEC (unsigned int csocodec)
{
	int codec = 0;

	switch (csocodec) {
		case CSO_ACODEC_MP3:
			codec = DI_AUDFORMAT_MP3;
			break;
		case CSO_ACODEC_AAC:
			codec = DI_AUDFORMAT_AAC;
			break;
		case CSO_ACODEC_AAC_PLUS_LOAS:
			codec = DI_AUDFORMAT_AAC_PLUS_LOAS;
			break;
		case CSO_ACODEC_AC3:
			codec = DI_AUDFORMAT_AC3;
			break;
		case CSO_ACODEC_AC3_PLUS:
			codec = DI_AUDFORMAT_AC3_PLUS;
			break;
		case CSO_ACODEC_MPEG:
			codec = DI_AUDFORMAT_MPEG;
			break;
		case CSO_ACODEC_WMA_STD:
			codec = DI_AUDFORMAT_WMA_STD;
			break;
		case CSO_ACODEC_WMA_PRO:
			codec = DI_AUDFORMAT_WMA_PRO;
			break;
		case CSO_ACODEC_PCM_S16BE:
		case CSO_ACODEC_PCM_S16LE:
		case CSO_ACODEC_PCM_S24BE:
		case CSO_ACODEC_PCM_S24LE:
		case CSO_ACODEC_PCM_S32BE:
		case CSO_ACODEC_PCM_S32LE:
		case CSO_ACODEC_ADPCM_MS:
			codec = DI_AUDFORMAT_PCMWAVE;
			break;
		default :
			codec = DI_AUDFORMAT_NONE;
			break;
	}
	return codec;
}

static int P_CONVERT_DI_VIDEO_CODEC (unsigned int csocodec)
{
	int codec = 0;

	switch (csocodec) {
#if defined(CONFIG_VIDEO_DEC_H265)
		case CSO_VCODEC_H265:
			codec = DI_VIDEO_FORMAT_MEDIA_H265;
			break;
#endif
		case CSO_VCODEC_H264:
			codec = DI_VIDEO_FORMAT_MEDIA_H264;
			break;
		case CSO_VCODEC_MPEG1:
			codec = DI_VIDEO_FORMAT_MEDIA_MPEG1;
			break;
		case CSO_VCODEC_MPEG2:
			codec = DI_VIDEO_FORMAT_MEDIA_MPEG2;
			break;
		case CSO_VCODEC_MPEG4Part2:
			codec = DI_VIDEO_FORMAT_MEDIA_MPEG4Part2;
			break;
		case CSO_VCODEC_VC1:
			codec = DI_VIDEO_FORMAT_MEDIA_VC1;
			break;
		case CSO_VCODEC_UNKNOWN:
			codec = DI_VIDEO_FORMAT_NONE;
			break;
		case CSO_VCODEC_VC1SimpleMain:
			codec = DI_VIDEO_FORMAT_MEDIA_VC1SimpleMain;
			break;
		case CSO_VCODEC_MSMPEGV3:
			codec = DI_VIDEO_FORMAT_MEDIA_DIVX_311;
			break;
		default :
			codec = DI_VIDEO_FORMAT_NONE;
			break;
	}
	return codec;
}

static void P_MeasureTime(timeHandle_t *pHandle)
{
    if(pHandle == NULL)
    {
        LOGMSG("time handle is invalid\n");
        return;
    }

    if(pHandle->bStart == FALSE)
    {
        gettimeofday(&pHandle->begin, NULL);
        LOGMSG("\033[1;32;40m (%s) start measuring \033[00m \n ", pHandle->pString);
        pHandle->bStart = TRUE;
    }
    else
    {
        gettimeofday(&pHandle->finish, NULL);
        pHandle->bStart = FALSE;
        LOGMSG("\033[1;32;40m (%s) finish measuring \033[00m \n", pHandle->pString);
    }
	return;
}

static void P_PrintTime(timeHandle_t *pHandle)
{
    if(pHandle == NULL)
    {
        LOGMSG("time handle is invalid\n");
        return;
    }
    HINT32 sec = pHandle->finish.tv_sec - pHandle->begin.tv_sec;
    HINT32 msec = (pHandle->finish.tv_usec - pHandle->begin.tv_usec)/1000;
    if(msec < 0)
    {
        sec--;
        msec = 1000+msec;
    }
    LOGMSG("\033[1;33;40m (%s) measured time : %d.%d sec \033[00m \n", pHandle->pString, sec, msec);

    return;
}

static void dnpPause(HUINT32 ulTunerId)
{
	CSO_CTRL_PARAM_t tParam;
	LOGMSG("-------------------------------------------------\n");
	LOGMSG("<EVENT : DNP> PAUSE    --------------------------\n");
	LOGMSG("-------------------------------------------------\n");
	tParam.nTrick = 0;
	CSO_Control(g_curCso->ulSessionId, eCSO_CTRL_TRICK, &tParam);
	return;
}
static void dnpResume(HUINT32 ulTunerId)
{
	CSO_CTRL_PARAM_t tParam;
	LOGMSG("-------------------------------------------------\n");
	LOGMSG("<EVENT : DNP> RESUME   --------------------------\n");
	LOGMSG("-------------------------------------------------\n");
	tParam.nTrick = 1;
	CSO_Control(g_curCso->ulSessionId, eCSO_CTRL_TRICK, &tParam);
	return;
}

#ifdef HLS_DISCONTINUITY
static void csoRestart()
{
	int err = 0;

#if ((CALYPSO_VERSION_MAJOR >= 1) && (CALYPSO_VERSION_MINOR >= 1) && (CALYPSO_VERSION_MICRO >= 8) && (CALYPSO_VERSION_BCODE == 1 || CALYPSO_VERSION_BCODE == 10))
	err = CSO_Stop(g_curCso->ulSessionId,-1);
#else
	err = CSO_Stop(g_curCso->ulSessionId);
#endif
	if ( err != CSO_ERR_OK)
	{
		LOGMSG("[#ERROR] DI_CSO_StopSession failed!\n");
	}

/************************
 * *		Free Slot
 * ************************/
	err = DI_PVR_PLAY_ReleasePlaypumpByIndex(g_curCso->uiPumpId);
	if(err != ERR_OK)
	{
		LOGMSG("[#ERROR] Release Resource failed!\n");
	}
	err = DI_AUDIO_Stop(DI_AUDIO_DECODER_MAIN);
	if(err != DI_ERR_OK)
	{
		CMD_Printf("[#ERROR] DI_AUDIO_Stop failed!\n");
	}
	err = DI_VIDEO_Stop(DI_VIDEO_DECODER_MAIN);
	if(err != DI_ERR_OK)
	{
		CMD_Printf("[#ERROR] DI_VIDEO_Stop failed!\n");
	}

	CSO_Reset(g_curCso->ulSessionId);

	err = CSO_Probe(g_curCso->ulSessionId);
	if ( err != CSO_ERR_OK)
	{
		CMD_Printf("[%s](L:%d) Error - MEDIAPB_IPC_CSO_Probe\n", __FUNCTION__, __LINE__);
	}

	while((err = CSO_ProbeInfo(g_curCso->ulSessionId, &(g_curCso->ProbeInfo))) != CSO_ERR_OK)
	{
		if(g_bProbeFailure[g_curCso->ulSessionId] == TRUE)
		{
			CMD_Printf("[#ERROR] Probe failed!\n");
			return;
		}
		if(err == CSO_ERR_NOT_READY)
		{
//			CMD_Printf("[#DEBUG] Session is not probed yet\n");
		}
		else
		{
			CMD_Printf("[#ERROR] DI_CSO_GetProbeInfo failed!\n");
			return;
		}
		VK_TASK_Sleep(100);
	}

	err = DI_PVR_PLAY_GetPlaypumpIndex(&g_curCso->uiPumpId);
	if(err != ERR_OK)
	{
		CMD_Printf("[%s][%d] DI_PVR_GetPlaypumpIndex failed!\n", __FUNCTION__,__LINE__);
		return -1;
	}

	DI_CSO_SetDecoderIndex(gpHSession, DI_MEDIA_VIDEO_DECODER_MAIN, DI_MEDIA_AUDIO_DECODER_MAIN);

	/********************
	 *	Set the Current Media Instance and initialize the internal status
	 ********************/
	//		(void)DI_CSO_SetCurrentMediaInstance(gpHSession);

	/********************
	 *	Let DI_CSO_Dummy know the duration of contents
	 ********************/
//	(void)DI_CSO_SetDuration(gpHSession, g_curCso->ProbeInfo.duration);

	err = DI_CSO_SetCurrentPlaypump(gpHSession, g_curCso->uiPumpId);
	if(err != DI_ERR_OK)
	{
		CMD_Printf("DI_CSO_SetCurrentPlaypump failed!\n");
		return -1;
	}

	//startsession
	err = CSO_Start(g_curCso->ulSessionId, g_curCso->uiPumpId);
	if(err != DI_ERR_OK)
	{
		CMD_Printf("[#ERROR] DI_CSO_StartSession failed!\n");
		return 0;
	}

	g_curCso->nTrick = 100; /* 1FF */

	err = DI_AUDIO_SetMediaType(DI_AUDIO_DECODER_MAIN, DI_AUDIOTYPE_MEDIA);
	if(err != DI_ERR_OK)
	{
		CMD_Printf("[#ERROR] DI_AUDIO_SetMediaType failed!\n");
		return 0;
	}

	if(g_curCso->ProbeInfo.ucTotalAudioTrack != 0)
	{
		if(g_curCso->ProbeInfo.audio_codec != 0)
		{
			nMediaTracks++;
			DI_CSO_SetActiveAudioTrack(gpHSession, P_CONVERT_DI_AUDIO_CODEC(g_curCso->ProbeInfo.audio_codec), 0x101);
			err = DI_AUDIO_Start(DI_AUDIO_DECODER_MAIN, P_CONVERT_DI_AUDIO_CODEC(g_curCso->ProbeInfo.audio_codec));
			if(err != DI_ERR_OK)
			{
				CMD_Printf("[#ERROR] DI_AUDIO_Start failed!\n");
				return 0;
			}
		}
		else
		{
			CMD_Printf("Not support track\n");
		}
	}
	else
	{
		CMD_Printf("No audio track\n");
	}

	if(g_curCso->ProbeInfo.ucTotalVideoTrack != 0)
	{
		if(g_curCso->ProbeInfo.video_codec != 0)
		{
			nMediaTracks++;
			DI_CSO_SetActiveVideoTrack(gpHSession, P_CONVERT_DI_VIDEO_CODEC(g_curCso->ProbeInfo.video_codec), 0x100);
			err = DI_VIDEO_Start(DI_VIDEO_DECODER_MAIN, P_CONVERT_DI_VIDEO_CODEC(g_curCso->ProbeInfo.video_codec));
			if(err != DI_ERR_OK)
			{
				CMD_Printf("[#ERROR] DI_VIDEO_Start failed!\n");
				return 0;
			}
		}
		else
		{
			CMD_Printf("Not support track\n");
		}
	}
	else
	{
		CMD_Printf("No Video track\n");
	}
	CMD_Printf("Start decorders\n");
}
#endif
static void eventCallback(HUINT32 ulTunerId, HINT32 ev, HINT64 param1)
{
	LOGMSG("-------------------------------------------------\n");
	LOGMSG(" <MEDIA EVENT(%u) ",ulTunerId);
	if(ev == CSO_EVENT_PROBE_COMPLETE )
	{
		LOGMSG("PROBE COMPLETE\n");
	}
	else if(ev == CSO_EVENT_PROBE_FAILURE )
	{
		LOGMSG("PROBE FAILURE\n");
		g_bProbeFailure[ulTunerId] = TRUE;
	}
	else if(ev == CSO_EVENT_BUFFERING_START )
	{
		LOGMSG("BUFFERING START\n");
	}
	else if(ev == CSO_EVENT_BUFFERING_ENOUGH )
	{
		LOGMSG("BUFFERING ENOUGH\n");
	}
	else if(ev == CSO_EVENT_BUFFERING_NOTENOUGH )
	{
		LOGMSG("BUFFERING NOT ENOUGH\n");
	}
	else if(ev == CSO_EVENT_BUFFERING_COMPLETE )
	{
		LOGMSG("BUFFERING COMPLETE\n");
	}
	else if(ev == CSO_EVENT_BUFFERING_FAILURE )
	{
		LOGMSG("BUFFERING FAILURE\n");
	}
	else if(ev == CSO_EVENT_MEDIA_RESUME )
	{
		LOGMSG("RESUME\n");
	}
	else if(ev == CSO_EVENT_MEDIA_SEEK_COMPLETE)
	{
		CSO_CTRL_PARAM_t tParam;
		DI_ERR_CODE dResult;
		CMD_Printf("\033[1;32;40m ======>>> Recv. SEEK Complete! (related request pos(%lld))\033[00m\n", param1);

		if(DI_VIDEO_Flush(DI_VIDEO_DECODER_MAIN) != DI_ERR_OK)
		{
			CMD_Printf("[#ERROR] DI_VIDEO_Flush failed \n");
			return ERR_FAIL;
		}

		if(DI_AUDIO_Flush(DI_AUDIO_DECODER_MAIN) != DI_ERR_OK)
		{
			CMD_Printf("[#ERROR] DI_AUDIO_Flush failed \n");
			return ERR_FAIL;
		}

		dResult = DI_CSO_Flush(gpHSession);
		if(dResult != DI_ERR_OK)
		{
			CMD_Printf("[#ERROR] DI_CSO_Flush failed!\n");
			return;
		}

		if(g_curCso->nTrick != 100 && g_curCso->nTrick != 0)
		{
			CMD_Printf("++ g_curCso->nTrick: %d\n", g_curCso->nTrick);
			dResult = DI_CSO_TrickMode(gpHSession, FALSE);
			if(dResult != DI_ERR_OK)
			{
				CMD_Printf("[#ERROR] DI_CSO_TrickMode failed!\n");
				return;
			}
			g_curCso->nTrick == 100;
		}
		CMD_Printf("\033[1;32;40m ======>>> Flush all decoders!\n\033[00m");

		dResult = CSO_Control(g_curCso->ulSessionId, eCSO_CTRL_RESUME, &tParam);
		if(dResult != CSO_ERR_OK)
		{
			CMD_Printf("[#ERROR] DI_IPTUNER_Control failed!\n");
			return;
		}
		CMD_Printf("\033[1;32;40m ======>>> CSO_EVENT_MEDIA_SEEK_COMPLETE and Resume request!\033[00m\n");
		g_bMediaEventWaiting = FALSE;
	}
	else if(ev == CSO_EVENT_MEDIA_TRICK_COMPLETE)
	{
		CSO_CTRL_PARAM_t tParam;
		DI_ERR_CODE dResult;
		CMD_Printf("\033[1;32;40m ======>>> Recv. TRICK Complete! (related request rate(%lld))\033[00m\n", param1);

		static int bPrevRate = 100;
		static HBOOL bTricking = FALSE;

		if(bPrevRate == 100 && param1 != 100)
		{
			bTricking = FALSE;
		}
		else if(bPrevRate != 100 && param1 == 100)
		{
			bTricking = FALSE;
		}
		else
		{
			bTricking = TRUE;
		}

		bPrevRate = param1;
		/* If not tricking mode, flush decoders */
		if(bTricking == FALSE)
		{
			CMD_Printf("++ CSO_EVENT_MEDIA_SEEK_COMPLETE \n");
			if(DI_VIDEO_Flush(DI_VIDEO_DECODER_MAIN) != DI_ERR_OK)
			{
				CMD_Printf("[#ERROR] DI_VIDEO_Flush failed \n");
				return ERR_FAIL;
			}

			if(DI_AUDIO_Flush(DI_AUDIO_DECODER_MAIN) != DI_ERR_OK)
			{
				CMD_Printf("[#ERROR] DI_VIDEO_Flush failed \n");
				return ERR_FAIL;
			}

			dResult = DI_CSO_Flush(gpHSession);
			if(dResult != DI_ERR_OK)
			{
				CMD_Printf("[#ERROR] DI_CSO_Flush failed!\n");
				return;
			}
		}
		else
		{
			CMD_Printf("TRICK RATE CHANGE\n");
		}

		if(g_curCso->nTrick != 100 && g_curCso->nTrick != 0)
		{
			CMD_Printf("++ eventCallback ++ g_curCso->nTrick: %d\n", g_curCso->nTrick);
			dResult = DI_CSO_TrickMode(gpHSession, TRUE);
			if(dResult != DI_ERR_OK)
			{
				CMD_Printf("[#ERROR] DI_CSO_TrickMode failed!\n");
				return;
			}
		}
		else
		{
			dResult = DI_CSO_TrickMode(gpHSession, FALSE);
			if(dResult != DI_ERR_OK)
			{
				CMD_Printf("[#ERROR] DI_CSO_TrickMode failed!\n");
				return;
			}
		}
		CMD_Printf("\033[1;32;40m ======>>> Flush all decoders!\n\033[00m");
		if(g_curCso->nTrick != 0)
		{
			dResult = CSO_Control(g_curCso->ulSessionId, eCSO_CTRL_RESUME, &tParam);
			if(dResult != CSO_ERR_OK)
			{
				CMD_Printf("[#ERROR] DI_IPTUNER_Control failed!\n");
				return;
			}
			CMD_Printf("\033[1;32;40m ======>>> CSO_EVENT_MEDIA_TRICK_COMPLETE and resume request!\n\033[00m");
		}
	}
	else if(ev == CSO_EVENT_ERROR_TRICK_FAIL)
	{
		CMD_Printf("\033[1;32;40m ======>>> Recv. TRICK Fail! (related request rate(%llu))\033[00m\n", param1);
	}
	else if(ev == CSO_EVENT_ERROR_SEEK_FAIL)
	{
		CMD_Printf("\033[1;32;40m ======>>> Recv. SEEK Fail! (related request pos(%llu))\033[00m\n", param1);
	}
	else if(ev == CSO_EVENT_MEDIA_PAUSE )
	{
		LOGMSG("PAUSED\n");
	}
	else if(ev == CSO_EVENT_MEDIA_SELECTED_VIDEO_TRACK_COMPLETE )
	{
		CSO_CTRL_PARAM_t tParam;
		DI_ERR_CODE dResult;
		LOGMSG("++ CSO_EVENT_MEDIA_SELECTED_VIDEO_TRACK_COMPLETE\n");
	}
	else if(ev == CSO_EVENT_MEDIA_SELECTED_AUDIO_TRACK_COMPLETE )
	{
		CSO_CTRL_PARAM_t tParam;
		DI_ERR_CODE dResult;
		LOGMSG("++  CSO_EVENT_MEDIA_SELECTED_AUDIO_TRACK_COMPLETE\n");
	}
	else if(ev == CSO_EVENT_MEDIA_BEGINOFSTREAM )
	{
		LOGMSG("BEGIN OF STREAM\n");
	}
	else if(ev == CSO_EVENT_MEDIA_ENDOFSTREAM )
	{
		LOGMSG("END OF STREAM\n");
#ifdef HLS_DISCONTINUITY
		g_curCso->bEofReached = TRUE;
/*
		if ( param != pstMediaPlay->ullDuration)
		{
			HxLOG_Error("Got EOF with New Duration from CSO\n");
			if( g_curCso->bNeedRestart != TRUE )
			{
				pstMediaPlay->ullDuration = param;
				(void)SVC_CSO_SetDuration(pstMediaPlay->pMediaPlayer, pstMediaPlay->ullDuration);
				om_media_play_mediapb_EvtPlayDurationChanged (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), pstMediaPlay->ullDuration);
			}
		}

		pstMediaPlay->bCsoEofReached = TRUE;
		pstMediaPlay->ulCsoEofReachedTime = time(NULL);
		HxLOG_Error("Got EOF from CSO\n");

		if ( pstMediaPlay->stCsoStatus.eStatus == eCSO_JUMP_TRICKING)
		{
			if( pstMediaPlay->bNeedRestart != TRUE )
			{
				om_media_play_mediapb_EvtEndOfStream (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, 0, 0);
			}
		}
		break;
	*/
#endif
	}
	else if(ev == CSO_EVENT_MEDIA_ERROROFSTREAM )
	{
		LOGMSG("ERROR OF STREAM\n");
	}
#ifdef HLS_DISCONTINUITY
	else if(ev == CSO_EVENT_MEDIA_NEEDRESTART)
	{
		LOGMSG("NEED RESTART\n");
		g_curCso->bNeedRestart = TRUE;
	}
#endif
	else
	{
		LOGMSG("UNKNOWN EVENT\n");
	}
	LOGMSG("-------------------------------------------------\n");
	return;
}


static void P_WriteCommandResult(FILE* fd, CMD_MEDIA_COM_e eCmd, char* pucResult, HUINT32 uiLength)
{
	char* pucLog=NULL;

	if(fd == NULL)
	{
		LOGMSG("Invalid parameter!\n");
		return;
	}

	pucLog = VK_MEM_Alloc(256);
	if(pucLog == NULL)
	{
		LOGMSG("Out of memory!\n");
		return;
	}

	if(eCmd == CMD_MEDIA_COM_CREATE)
	{
		VK_snprintf(pucLog, uiLength, pucResult);
	}
	else if(eCmd == CMD_MEDIA_COM_STARTBUF)
	{
		VK_snprintf(pucLog, uiLength, pucResult);
	}
	else if(eCmd == CMD_MEDIA_COM_START)
	{
		VK_snprintf(pucLog, uiLength, pucResult);
	}
	else if(eCmd == CMD_MEDIA_COM_STOP)
	{
		VK_snprintf(pucLog, uiLength, pucResult);
	}
	else if(eCmd == CMD_MEDIA_COM_STOPBUF)
	{
		VK_snprintf(pucLog, uiLength, pucResult);
	}
	else if(eCmd == CMD_MEDIA_COM_DESTROY)
	{
		VK_snprintf(pucLog, uiLength, pucResult);
	}
	else if(eCmd == CMD_MEDIA_COM_SEEK)
	{
		VK_snprintf(pucLog, uiLength, pucResult);
	}
	else if(eCmd == CMD_MEDIA_COM_TRICK)
	{
		VK_snprintf(pucLog, uiLength, pucResult);
	}
	else if(eCmd == CMD_MEDIA_COM_DELAY)
	{
		VK_snprintf(pucLog, uiLength, pucResult);
	}
	else
	{
		VK_snprintf(pucLog, uiLength, pucResult);
	}

	fwrite(pucLog, uiLength, 1, fd);
	VK_fflush(fd);
	VK_MEM_Free(pucLog);

	return;
}

static void P_WriteTime(FILE* fd, HBOOL bStart)
{
	char* pucLog=NULL;
	struct timeval tv;

	if(fd == NULL)
	{
		LOGMSG("Invalid parameter!\n");
		return;
	}

	pucLog = VK_MEM_Alloc(256);
	if(pucLog == NULL)
	{
		LOGMSG("Out of memory!\n");
		return;
	}
	VK_MEM_Memset(pucLog, 0, 256);

	gettimeofday(&tv, NULL);
	if(bStart == TRUE)
	{
		VK_snprintf(pucLog, 36, "\nStart: %08d sec %08d usec\n", tv.tv_sec, tv.tv_usec);
		fwrite(pucLog, 36, 1, fd);
	}
	else
	{
		VK_snprintf(pucLog, 34, "\nEnd: %08d sec %08d usec\n", tv.tv_sec, tv.tv_usec);
		fwrite(pucLog, 34, 1, fd);
	}
	VK_fflush(fd);
	VK_MEM_Free(pucLog);

	return;
}

static void P_WriteCommand(FILE* fd, HUINT32 uiSessionId, CMD_MEDIA_COM_e eCmd)
{
	HUINT32 uiLength=0;
	char* pucLog=NULL;

	if(fd == NULL)
	{
		LOGMSG("Invalid parameter!\n");
		return;
	}

	pucLog = VK_MEM_Alloc(256);
	if(pucLog == NULL)
	{
		LOGMSG("Out of memory!\n");
		return;
	}

/*************************
*	1. Command
*************************/
	if(eCmd == CMD_MEDIA_COM_CREATE)
	{
		uiLength += 9;
		VK_snprintf(pucLog, uiLength, "create: ");
	}
	else if(eCmd == CMD_MEDIA_COM_STARTBUF)
	{
		uiLength += 11;
		VK_snprintf(pucLog, uiLength, "startbuf: ");
	}
	else if(eCmd == CMD_MEDIA_COM_START)
	{
		uiLength += 8;
		VK_snprintf(pucLog, uiLength, "start: ");
	}
	else if(eCmd == CMD_MEDIA_COM_STOP)
	{
		uiLength += 7;
		VK_snprintf(pucLog, uiLength, "stop: ");
	}
	else if(eCmd == CMD_MEDIA_COM_STOPBUF)
	{
		uiLength += 10;
		VK_snprintf(pucLog, uiLength, "stopbuf: ");
	}
	else if(eCmd == CMD_MEDIA_COM_DESTROY)
	{
		uiLength += 10;
		VK_snprintf(pucLog, uiLength, "destroy: ");
	}
	else if(eCmd == CMD_MEDIA_COM_SEEK)
	{
		uiLength += 7;
		VK_snprintf(pucLog, uiLength, "seek: ");
	}
	else if(eCmd == CMD_MEDIA_COM_TRICK)
	{
		uiLength += 8;
		VK_snprintf(pucLog, uiLength, "trick: ");
	}
	else if(eCmd == CMD_MEDIA_COM_DELAY)
	{
		uiLength += 8;
		VK_snprintf(pucLog, uiLength, "delay: ");
	}
	else
	{
		VK_MEM_Free(pucLog);
		return;
	}

	fwrite(pucLog, uiLength, 1, fd);

/*************************
*	2. Session Id
*************************/
	VK_MEM_Memset(pucLog, 0, 256);
	VK_snprintf(pucLog, 8, "id= %02d ", uiSessionId);
	fwrite(pucLog, uiLength, 1, fd);
	VK_fflush(fd);

	VK_MEM_Free(pucLog);

	return;
}

//Task for script

static int P_CMD_MEDIA_ReadLine(char** pLine, char* pBuf, unsigned int unSize)
{
	char *p = pBuf;
	unsigned int unLineLen = 0;
	char *pEnd = NULL;
	if(p == NULL)
	{
		*pLine = NULL;
		return 0;
	}
	pEnd = p + unSize;
	while(p < pEnd)
	{
		if( *p == '\n' || *p == '\r')
		{
			if(unLineLen == 0)
			{
				*pLine = NULL;
				return 1;
			}
			unLineLen++;
			break;
		}
		else
		{
			p++;
			unLineLen++;
		}
	}
	if(unLineLen == 0)
	{
		p++;
		*pLine = NULL;
		return 0;
	}
	*pLine = VK_malloc(unLineLen+1);
	VK_memset(*pLine,0x00,unLineLen+1);
	VK_memcpy(*pLine, pBuf, p - pBuf);
	return unLineLen;
}

static void P_CMD_MEDIA_RegisterCommand(CSO_INI_t *pIni, CSO_CMDSET_t cmdset)
{
	if(pIni != NULL && pIni->nCmdCursor < CMD_MEDIA_MAX_CMD)
	{
		pIni->CmdSet[pIni->nCmdCursor].cmd.eCmd = cmdset.cmd.eCmd;
		pIni->CmdSet[pIni->nCmdCursor].cmd.id = cmdset.cmd.id;
		pIni->CmdSet[pIni->nCmdCursor].cmd.param1 = cmdset.cmd.param1;
		pIni->CmdSet[pIni->nCmdCursor].cmd.param2 = cmdset.cmd.param2;
		pIni->CmdSet[pIni->nCmdCursor].cmd.param3 = cmdset.cmd.param3;
		pIni->CmdSet[pIni->nCmdCursor].time = cmdset.time;
		pIni->nCmdCursor++;
	}
}
static void P_CMD_MEDIA_ToScript(FILE *fp, CSO_INI_t *pIni)
{
	VK_fprintf(fp,"#test\n");
	VK_fprintf(fp,"#asset\n");
	VK_fprintf(fp,"%s\n",pIni->pUrl);
	VK_fprintf(fp,"#asset-end\n");
	VK_fprintf(fp,"#command\n");

	for(unsigned int i = 0 ; i < pIni->nCmdCursor ; i++)
	{
		switch(pIni->CmdSet[i].cmd.eCmd)
		{
			case CMD_MEDIA_COM_CREATE:
				VK_fprintf(fp,"#create:num=%d\n",1);
				break;
			case CMD_MEDIA_COM_STARTBUF:
				VK_fprintf(fp,"#startbuf:id=%d\n",pIni->CmdSet[i].cmd.id);
				break;
			case CMD_MEDIA_COM_START:
				VK_fprintf(fp,"#start:id=%d\n",pIni->CmdSet[i].cmd.id);
				break;
			case CMD_MEDIA_COM_STOP:
				VK_fprintf(fp,"#stop:id=%d\n",pIni->CmdSet[i].cmd.id);
				break;
			case CMD_MEDIA_COM_STOPBUF:
				VK_fprintf(fp,"#stopbuf:id=%d\n",pIni->CmdSet[i].cmd.id);
				break;
			case CMD_MEDIA_COM_DESTROY:
				VK_fprintf(fp,"#destroy\n");
				break;
			case CMD_MEDIA_COM_SETAUDIO:
				VK_fprintf(fp,"#setaudio:id=%d,num=%d\n",pIni->CmdSet[i].cmd.id,pIni->CmdSet[i].cmd.param1);
				break;
			case CMD_MEDIA_COM_SEEK:
				VK_fprintf(fp,"#seek:id=%d,time=%llu,origin=start\n",pIni->CmdSet[i].cmd.id,pIni->CmdSet[i].cmd.param2);
				break;
			case CMD_MEDIA_COM_TRICK:
				VK_fprintf(fp,"#trick:id=%d,speed=%d\n",pIni->CmdSet[i].cmd.id,pIni->CmdSet[i].cmd.param1);
				break;
			case CMD_MEDIA_COM_DELAY:
				VK_fprintf(fp,"#delay:time=%llu\n",pIni->CmdSet[i].cmd.param2);
				break;
			case CMD_MEDIA_COM_WAIT:
				VK_fprintf(fp,"#wait:id=%d\n",pIni->CmdSet[i].cmd.id);
				break;
			case CMD_MEDIA_COM_MAX:
				break;
			default :
				break;
		}
	}
	VK_fprintf(fp,"#command-end\n");
	VK_fprintf(fp,"#test-end\n");
	return;
}

static CSO_INI_t* P_CMD_MEDIA_ENVManager(HBOOL bSave)
{
	FILE *fp = NULL;
	HINT32 nSize = 0;
	HINT8 *pBuf = NULL;
	HINT8 *pTemp = NULL;
	HINT8 *pLine = NULL;
	if(bSave == TRUE)
	{
		if(g_pIni != NULL)
		{
			CMD_Printf("Ini Save.\n");
			fp=fopen(DEFAULT_CSO_INI_PATH,"w+b");
			if(fp==NULL)
			{
				CMD_Printf("File not found\n");
				return NULL;
			}
			VK_fprintf(fp,"- MEDIA INI FILE ---- \n");
			VK_fprintf(fp,"@URL=%s\n",g_pIni->pUrl);
			VK_fprintf(fp,"@MODE=%d\n",g_pIni->nPlayMode);
			for(int i = 0; i < g_pIni->nUrlCnt ; i++)
			{
				VK_fprintf(fp,"@ASSET=%s\n\r",g_pIni->urls[i]);
			}
			VK_fprintf(fp,"\n@SCRIPT\n");
			CMD_Printf("Write Script.\n");
			P_CMD_MEDIA_ToScript(fp,g_pIni);
			fclose(fp);
		}
	}
	else
	{
		if(g_pIni == NULL)
		{
			g_pIni = VK_MEM_Alloc(sizeof(CSO_INI_t));
			if(g_pIni == NULL)
			{
				CMD_Printf("Memory Allocation Fail\n");
				return NULL;
			}
			VK_MEM_Memset(g_pIni,0x00,sizeof(CSO_INI_t));
		}
		for(int i = 0 ; i < CMD_MEDIA_MAX_ASSET ; i++)
		{
			if(g_pIni->urls[i] != NULL)
			{
				VK_MEM_Free(g_pIni->urls[i]);
			}
			g_pIni->urls[i] = NULL;
		}
		if(g_pIni->pUrl != NULL)
		{
			VK_MEM_Free(g_pIni->pUrl);
		}
		VK_MEM_Memset(g_pIni,0x00,sizeof(CSO_INI_t));
		fp=fopen(DEFAULT_CSO_INI_PATH,"r+b");
		if(fp==NULL)
		{
			CMD_Printf("File not found\n");
			return g_pIni;
		}
		fseek(fp,0,SEEK_END);
		nSize = ftell(fp);
		fseek(fp,0,SEEK_SET);
		pBuf = VK_MEM_Alloc(nSize);
		if(pBuf == NULL)
		{
			CMD_Printf("Buffer Allocation Fail\n");
			fclose(fp);
			return g_pIni;
		}
		fread(pBuf,nSize,1,fp);
		fclose(fp);
		CMD_Printf("File Read.\n");
		pTemp = VK_strstr(pBuf,"@URL=");
		if(pTemp != NULL)
		{
			pTemp+=VK_strlen("@URL=");
			(void)P_CMD_MEDIA_ReadLine(&pLine,pTemp,nSize);
			if(g_pIni->pUrl != NULL)
			{
				VK_MEM_Free(g_pIni->pUrl);
			}
			g_pIni->pUrl = VK_StrDup(pLine);
			VK_MEM_Free(pLine);
		}
		CMD_Printf("URL : %s\n",g_pIni->pUrl);
		pTemp = VK_strstr(pBuf,"@MODE=");
		if(pTemp != NULL)
		{
			pTemp+=VK_strlen("@MODE=");
			(void)P_CMD_MEDIA_ReadLine(&pLine,pTemp,pTemp - pBuf);
			g_pIni->nPlayMode = VK_atoi(pLine);
			VK_MEM_Free(pLine);
		}
		CMD_Printf("Playmode : %d\n",g_pIni->nPlayMode);
		pTemp = pBuf;
		while((pTemp = VK_strstr(pTemp,"@ASSET="))!=NULL)
		{
			pTemp+=VK_strlen("@ASSET=");
			(void)P_CMD_MEDIA_ReadLine(&pLine,pTemp,pTemp - pBuf);
			g_pIni->urls[g_pIni->nUrlCnt++]=VK_StrDup(pLine);
			VK_MEM_Free(pLine);
		}
		CMD_Printf("Asset Load.\n");
		g_pIni->bLoad = TRUE;
		VK_MEM_Free(pBuf);
	}
	return g_pIni;
}

static int P_CMD_GetResourceMap(void** pMap)
{
	DI_ERR_CODE dResult;

	*pMap = VK_malloc(sizeof(CMD_MEDIA_Nexus_Resource_t*));
	if(*pMap == NULL)
	{
		CMD_Printf("Out of Memory!\n");
		return -1;
	}

/*************************
*	1. Getting Playpump Map
*************************/
	dResult = DI_PVR_PLAY_GetPlaypumpResourceMap(&s_ResourceMap.pPlaypumpList, &s_ResourceMap.uiNumOfPlaypump);
	if(dResult != DI_ERR_OK)
	{
		CMD_Printf("DI_PVR_GetPlaypumpMap failed!\n");
		return -1;
	}

	*pMap = &s_ResourceMap;
	return 0;
}

static void P_EventTask(void* args)
{
	HBOOL bBOF, bEOF;
	DI_ERR_CODE dResult;
#ifdef HLS_DISCONTINUITY
	HUINT64 ullCurrentOffset = 0;
	HUINT64 ullPrevOffset = 0;
	HUINT32 ulEofDetectTime = 0;
#endif

	while(1)
	{
		if(IS_CSO_PLAYING)
		{
			dResult = DI_CSO_GetCurrentStatus(gpHSession, &bBOF, &bEOF);
			if(dResult != DI_ERR_OK)
			{
				CMD_Printf("[%s][%d] DI_CSO_GetCurrentStatus failed!\n", __FUNCTION__,__LINE__);
			}

			if(bBOF == TRUE)
			{
				CMD_Printf("========================================BOF==================================================\n");
			}

			if(bEOF == TRUE)
			{
				CMD_Printf("========================================EOF==================================================\n");
			}
#ifdef HLS_DISCONTINUITY
			if( g_curCso->bEofReached == TRUE)
			{
				dResult = DI_CSO_GetPosition(gpHSession, g_curCso->unMediaId, &ullCurrentOffset);
				if(dResult != DI_ERR_OK)
				{
					LOGMSG("[#ERROR] DI_CSO_GetPosition failed!\n");
				}
				if(ullCurrentOffset != ullPrevOffset)
				{
					ullPrevOffset = ullCurrentOffset;
				}
				else
				{
					if(ulEofDetectTime != 0)
					{
						if( time(NULL) - ulEofDetectTime > 2/*sec*/)
						{
							if( g_curCso->bNeedRestart == TRUE )
							{
								g_curCso->bNeedRestart = FALSE;
								g_curCso->bEofReached = FALSE;
								ulEofDetectTime = 0;
								ullPrevOffset = 0;
								ullCurrentOffset = 0;
								//	om_media_play_mediapb_cso_Reset( pstContext );
								// TODO : Reset
								csoRestart();
							}
						}
					}
					else
					{
						ulEofDetectTime = time(NULL);
					}
				}
			}
#endif
		}
		VK_TASK_Sleep(100);
	}
}

static void P_ScriptCmdTask(void* args)
{
	FILE* fp;
	FILE* fResultLog=NULL;
	CMD_MEDIA_LINE_t tData;
	HINT8 *pBuf = NULL;
	HINT32 size = 0;
	HUINT32 nURLIdx=0;
	HUINT32 uiWaitingDuration;
	HUINT32 uiSessionId;
	HUINT32 uiErrorCheck;
	HUINT64 ullTargetOffset;
	HUINT64 ullCurrentOffset;
	HUINT64 ullPreviousPosition[MAX_SESSION];
	HUINT64 ullCreatedMedia;
	HUINT64 ullDelayTime;
	DI_ERR_CODE dResult;
	HINT32 eSpeed;
	CMD_MEDIA_SEEKDIR_e eOrigin;
	HUINT32 uiIndex=0;
	HUINT8* ucPath;
	HUINT8* ucPathIdx;
	char* pucTemp=NULL;
    HUINT32 ulVideoCaller = 0;
    HUINT32 ulAudioCaller = 0;
	CSO_INFO_t *MediaSession = NULL;
	HBOOL bSkipTrick = FALSE;
	HERROR hResult;
	void* pMap = NULL;
	CSO_ERR cResult;

	fp=fopen(g_ucScriptPath,"rb");
	if(fp==NULL)
	{
		CMD_Printf("File not found\n");
		return;
	}

	fseek(fp,0,SEEK_END);
	size = ftell(fp);
	fseek(fp,0,SEEK_SET);
	pBuf = VK_malloc(size + 1);
	VK_memset(pBuf,0x0,size+1);
	fread(pBuf,size,1,fp);
	fclose(fp);
	VK_MEM_Memset(&tData,0x00,sizeof(CMD_MEDIA_LINE_t));
	CMD_MEDIA_Script_Init(pBuf,size);


	for(uiIndex=0; uiIndex<MAX_SESSION; uiIndex++)
	{
		ullPreviousPosition[uiIndex] = 0;
	}

	fResultLog = fopen("/result.txt", "wb");
	if(fResultLog == NULL)
	{
		CMD_Printf("fopen failed!\n");
		return;
	}

	MediaSession = (CSO_INFO_t*)VK_MEM_Alloc(sizeof(CSO_INFO_t)*MAX_SESSION);
	if(MediaSession == NULL)
	{
		LOGMSG("[#ERROR] Out of Memory!\n");
		return;
	}
	VK_MEM_Memset(MediaSession, 0x00, sizeof(CSO_INFO_t)*MAX_SESSION);
	VK_MEM_Memset(g_bProbeFailure, 0x00, sizeof(g_bProbeFailure));
	pucTemp = VK_MEM_Alloc(256);
	if(pucTemp == NULL)
	{
		LOGMSG("[#ERROR] Out of Memory!\n");
		return;
	}

#ifdef CSO_MEMORY_DEBUG
	EIKI_MEM_Init();
#endif
	if( g_bCsoInit == FALSE )
	{
		VK_memset(&g_CsoSettings,0x00,sizeof(CSO_Settings_t));
		CSO_GetDefaultSettings(&g_CsoSettings);
		g_CsoSettings.pSaveFilePath = DEFAULT_CSO_DOWNLOAD_PATH;
		g_CsoSettings.cbMediaEvent = eventCallback;
		CSO_SYS_Init();
		if(CSO_Init(&g_CsoSettings) != CSO_ERR_OK)
		{
			CSO_SetDefaultSettings(&g_CsoSettings);
		}
		g_bCsoInit = TRUE;

		hResult = P_CMD_GetResourceMap(&pMap);
		if(hResult != ERR_OK)
		{
			CMD_Printf("[%s][%d] CSO_MP_GetResourceMap failed!\n", __FUNCTION__,__LINE__);
			return -1;
		}

		cResult = CSO_SetResourceMap(pMap);
		if(cResult != CSO_ERR_OK)
		{
			CMD_Printf("[%s][%d] CSO_SetResourceMap failed!\n", __FUNCTION__,__LINE__);
			return -1;
		}

		dResult = VK_TASK_Create(P_EventTask, 40, 0x8000, "CSO_EVENT_TASK", NULL, &g_nEventTaskId, 0);
		if(dResult != VK_OK)
		{
			LOGMSG("[#ERROR] VK_TASK_Create failed\n");
			return dResult;
		}
		else
		{
			LOGMSG("script task created : %d\n", g_nEventTaskId);
			VK_TASK_Start(g_nEventTaskId);
		}

/*************************
*	Player Create
*************************/
		dResult = DI_CSO_CreateSession(NULL, &gpHSession);
		if(dResult != DI_ERR_OK)
		{
			CMD_Printf("[%s][%d] DI_CSO_CreateSession failed!\n", __FUNCTION__,__LINE__);
			return -1;
		}
	}

	//do Start
	VK_snprintf(pucTemp, 44,"Command:	   Result:	   param1:	   param2");
	P_WriteCommandResult(fResultLog, tData.eCmd, pucTemp, 44);
	while(TRUE)
	{
		CMD_MEDIA_ERR_e eResult = CMD_MEDIA_Script_GetLine(&tData);

		uiSessionId=tData.id;
		eSpeed=tData.param1;
		ullDelayTime = tData.param2;
		eOrigin=tData.param3;

		if(eResult != CMD_MEDIA_ERR_OK)
		{
			LOGMSG("Test script [%d] : ",eResult);

			if(eResult == CMD_MEDIA_ERR_EOF)
			{
				LOGMSG("  ===> EOF reached\n");
#ifdef CSO_STRESS_TEST //for stress test
				LOGMSG("       reinitialize\n");
				CMD_MEDIA_Script_Uninit();
//				fseek(fp,0,SEEK_SET);
#endif
#ifdef CSO_MEMORY_DEBUG
				EIKI_MEM_PrintAll();
				EIKI_MEM_Uninit();
#ifdef CSO_STRESS_TEST
				EIKI_MEM_Init();
#endif
#endif
#ifdef CSO_STRESS_TEST //for stress test
				VK_MEM_Memset(&tData,0x00,sizeof(CMD_MEDIA_LINE_t));
				CMD_MEDIA_Script_Init(pBuf,size);
				continue;
#else
				break;
#endif
			}
			else if(eResult == CMD_MEDIA_ERR_PARSE || eResult == CMD_MEDIA_ERR_SYNTAX)
			{
				LOGMSG("  ===> Parsing error or Syntax error\n");
			}
			else if(eResult == CMD_MEDIA_ERR_FAIL)
			{
				LOGMSG("  ===> failed\n");
			}
			else
			{
				LOGMSG("  ===> Unknown error\n");
			}

			break;	//exit from task
		}

		switch(tData.eCmd)
		{
			/* no need to process */
			case CMD_MEDIA_COM_ASSET:
			case CMD_MEDIA_COM_COMMAND:
			case CMD_MEDIA_COM_COMMANDEND:
				break;

			case CMD_MEDIA_COM_TEST:
				LOGMSG("Test case started\n");
#ifdef CSO_MEMORY_DEBUG
				system("echo 3 > /proc/sys/vm/drop_caches");
				VK_TASK_Sleep(1500);
				system("free");
#endif
				break;

			case CMD_MEDIA_COM_LOG:
				LOGMSG("[Script] %s\n",tData.param4);
				P_WriteTime(fResultLog, TRUE);
				P_WriteCommand(fResultLog, uiSessionId, tData.eCmd);
				P_WriteCommandResult(fResultLog, tData.eCmd, (char*) tData.param4, VK_strlen((char*)tData.param4)+1);
				P_WriteTime(fResultLog, FALSE);
				VK_MEM_Free(tData.param4);
				tData.param4 = NULL;
				break;

			case CMD_MEDIA_COM_TESTEND:
				LOGMSG("Test case finished\n");

				for(HINT32 idx=0; idx < nURLIdx; idx++)
				{
					VK_MEM_Free(MediaSession[idx].url);
					MediaSession[idx].url = NULL;
				}

				nURLIdx = 0;
				break;

			case CMD_MEDIA_COM_URL:
				LOGMSG("Asset Section[%d] : %s....", nURLIdx, tData.param4);
				MediaSession[nURLIdx].url = tData.param4;
				MediaSession[nURLIdx].nTrick = 100;	/* normal rate */
				nURLIdx++;

				LOGMSG("registered\n", tData.param4);
				break;

			case CMD_MEDIA_COM_ASSETEND:
				//fill url in remaining empty session
				LOGMSG("Insert url for remaining empty sessions\n");
				for(HINT32 idx=nURLIdx; idx < MAX_SESSION; idx++)
				{
					LOGMSG("Asset Section[%d] : %s....", idx, MediaSession[idx%nURLIdx].url);
					MediaSession[idx].url = MediaSession[idx%nURLIdx].url;
					LOGMSG("registered\n", tData.param4);
				}
				break;

			case CMD_MEDIA_COM_CREATE:
				P_WriteTime(fResultLog, TRUE);
				P_WriteCommand(fResultLog, uiSessionId, tData.eCmd);

				ullCreatedMedia = tData.param2;
				LOGMSG("Open Session : #%d\n",ullCreatedMedia);
				for( HINT32 idx=0; idx < ullCreatedMedia; idx++)
				{
					LOGMSG("Session %d....", idx);
					dResult = CSO_Open(MediaSession[idx].url,eCSO_MODE_PLAY, &MediaSession[idx].ulSessionId);
					if(dResult != CSO_ERR_OK)
					{
						LOGMSG("[#ERROR] CSO_Open failed!\n");
						P_WriteCommandResult(fResultLog, tData.eCmd, "create_fail", 12);

						VK_MEM_Memset(pucTemp,0,256);
						VK_snprintf(pucTemp, VK_strlen(MediaSession[idx].url)+6, "	 %s", MediaSession[idx].url);
						P_WriteCommandResult(fResultLog, tData.eCmd, pucTemp, VK_strlen(MediaSession[idx].url)+6);
						P_WriteTime(fResultLog, FALSE);
						break;
					}
					dResult = CSO_Probe(MediaSession[idx].ulSessionId);
					if(dResult != CSO_ERR_OK)
					{
						LOGMSG("[#ERROR] CSO_Probe failed!\n");
						P_WriteCommandResult(fResultLog, tData.eCmd, "probe_fail", 12);

						VK_MEM_Memset(pucTemp,0,256);
						VK_snprintf(pucTemp, VK_strlen(MediaSession[idx].url)+6, "	 %s", MediaSession[idx].url);
						P_WriteCommandResult(fResultLog, tData.eCmd, pucTemp, VK_strlen(MediaSession[idx].url)+6);
						P_WriteTime(fResultLog, FALSE);
						break;
					}
					LOGMSG("created\n");
				}

				P_WriteCommandResult(fResultLog, tData.eCmd, "success", 8);
				P_WriteTime(fResultLog, FALSE);
				break;

			case CMD_MEDIA_COM_STARTBUF:
				P_WriteTime(fResultLog, TRUE);
				P_WriteCommand(fResultLog, uiSessionId, tData.eCmd);
                g_curCso = &MediaSession[uiSessionId];
#ifdef	CSO_BUFFERING_PATH_HDD
				ucPath = VK_MEM_Alloc(sizeof(HUINT8)*26);
				VK_strncpy(ucPath, "dvr:///mnt/hd2/download", 24);
#else
				ucPath = VK_MEM_Alloc(sizeof(HUINT8)*23);
				VK_strncpy(ucPath, "dvr:///home/download", 21);
#endif
				ucPathIdx = (HUINT8*)VK_MEM_Alloc(sizeof(HUINT8)*3);
				ucPathIdx[0] = '0'+uiSessionId/10;
				ucPathIdx[1] = '0'+uiSessionId%10;
				ucPathIdx[2] = '\0';

				VK_strcat(ucPath, ucPathIdx);
				VK_MEM_Free(ucPathIdx);

				if(MediaSession[uiSessionId].ucBufferPath != NULL)
				{
					VK_MEM_Free(MediaSession[uiSessionId].ucBufferPath);
					MediaSession[uiSessionId].ucBufferPath = NULL;
				}
				MediaSession[uiSessionId].ucBufferPath = ucPath;
				LOGMSG("Buffering path : %s\n", MediaSession[uiSessionId].ucBufferPath);
				CSO_GetSettings(MediaSession[uiSessionId].ulSessionId,&g_CsoSettings);
				g_CsoSettings.pSaveFilePath = MediaSession[uiSessionId].ucBufferPath;
				CSO_SetSettings(MediaSession[uiSessionId].ulSessionId,&g_CsoSettings);

				LOGMSG("Session %d...", uiSessionId);
				dResult = CSO_Control(MediaSession[uiSessionId].ulSessionId, eCSO_CTRL_STARTBUFFERING,NULL);
				if(dResult != CSO_ERR_OK)
				{
				   LOGMSG("[#ERROR] CSO_Control(StartBuffering) failed!\n");
					P_WriteCommandResult(fResultLog, tData.eCmd, "startbuf_fail", 14);
					P_WriteTime(fResultLog, FALSE);
					break;
				}
				MediaSession[uiSessionId].isBuffering = true;
				LOGMSG("Buffering start\n");
				P_WriteCommandResult(fResultLog, tData.eCmd, "success", 8);
				P_WriteTime(fResultLog, FALSE);
				break;

			case CMD_MEDIA_COM_START:
				P_WriteTime(fResultLog, TRUE);
				P_WriteCommand(fResultLog, uiSessionId, tData.eCmd);
				g_curCso = &MediaSession[uiSessionId];

				LOGMSG("Session %d....(GetProbeInfo)\n", uiSessionId);
				while((dResult = CSO_ProbeInfo(MediaSession[uiSessionId].ulSessionId, &(MediaSession[uiSessionId].ProbeInfo))) != CSO_ERR_OK)
				{
					if(g_bProbeFailure[uiSessionId] == TRUE)
					{
						CMD_Printf("[#ERROR] Probe failed!\n");
						return 0;
					}
					if(dResult == CSO_ERR_NOT_READY)
					{
		//				CMD_Printf("[#DEBUG] Session is not probed yet\n");
					}
					else
					{
						CMD_Printf("[#ERROR] DI_CSO_GetProbeInfo failed!\n");
						return 0;
					}
					VK_TASK_Sleep(100);
				}

				{
					int i;
					for(i=0; i < MediaSession[uiSessionId].ProbeInfo.ucTotalVideoTrack; i++)
					{
						CMD_Printf("[CMD] Video[%d] codec(%d) pid(%d) detected\n", i
							,MediaSession[uiSessionId].ProbeInfo.stVideoTrack[i].video_codec
							,MediaSession[uiSessionId].ProbeInfo.stVideoTrack[i].video_stream_id);
					}
					for(i=0; i < MediaSession[uiSessionId].ProbeInfo.ucTotalAudioTrack; i++)
					{
						CMD_Printf("[CMD] Video[%d] codec(%d) pid(%d) lang(%s) detected\n", i
							,MediaSession[uiSessionId].ProbeInfo.stAudioTrack[i].audio_codec
							,MediaSession[uiSessionId].ProbeInfo.stAudioTrack[i].audio_stream_id
							,MediaSession[uiSessionId].ProbeInfo.stAudioTrack[i].uacAudioLang);
					}
				}

/********************
*	Get the available playpump index
********************/
				hResult = DI_PVR_PLAY_GetPlaypumpIndex(&MediaSession[uiSessionId].uiPumpId);
				if(hResult != ERR_OK)
				{
					CMD_Printf("[%s][%d] DI_PVR_GetPlaypumpIndex failed!\n", __FUNCTION__,__LINE__);
					return -1;
				}

				DI_CSO_SetDecoderIndex(gpHSession, DI_MEDIA_VIDEO_DECODER_MAIN, DI_MEDIA_AUDIO_DECODER_MAIN);
#ifdef HLS_DISCONTINUITY /* live duration is 0(zero)*/
				if(MediaSession[uiSessionId].ProbeInfo.duration == 0x4189374BC6A7)
				{
					(void)DI_CSO_SetDuration(gpHSession, 0);
				}
				else
#endif
				(void)DI_CSO_SetDuration(gpHSession, MediaSession[uiSessionId].ProbeInfo.duration);
				(void)DI_CSO_StartSession(gpHSession, MediaSession[uiSessionId].ulSessionId,0);

				dResult = DI_CSO_SetCurrentPlaypump(gpHSession, MediaSession[uiSessionId].uiPumpId);
				if(dResult != DI_ERR_OK)
				{
					CMD_Printf("DI_CSO_SetCurrentPlaypump failed!\n");
					return -1;
				}

				dResult = CSO_Start(MediaSession[uiSessionId].ulSessionId, MediaSession[uiSessionId].uiPumpId);
				if(dResult != CSO_ERR_OK)
				{
					LOGMSG("[#ERROR] DI_CSO_SetupSession failed!\n");
					P_WriteCommandResult(fResultLog, tData.eCmd, "setup_fail", 11);
					P_WriteTime(fResultLog, FALSE);
					break;
				}

				dResult = DI_AUDIO_SetMediaType(DI_AUDIO_DECODER_MAIN, DI_AUDIOTYPE_MEDIA);
				if(dResult != DI_ERR_OK)
				{
					LOGMSG("[#ERROR] DI_AUDIO_SetMediaType failed!\n");
					P_WriteCommandResult(fResultLog, tData.eCmd, "setmediatype_fail", 18);
					P_WriteTime(fResultLog, FALSE);
					break;
				}

				if(MediaSession[uiSessionId].ProbeInfo.ucTotalAudioTrack != 0)
				{
					if(MediaSession[uiSessionId].ProbeInfo.audio_codec != 0)
					{
						DI_CSO_SetActiveAudioTrack(gpHSession, P_CONVERT_DI_AUDIO_CODEC(MediaSession[uiSessionId].ProbeInfo.audio_codec), 0x101);
						dResult = DI_AUDIO_Start(DI_AUDIO_DECODER_MAIN, P_CONVERT_DI_AUDIO_CODEC(MediaSession[uiSessionId].ProbeInfo.audio_codec));
						if(dResult != DI_ERR_OK)
						{
							LOGMSG("[#ERROR] DI_AUDIO_Start failed!\n");
							P_WriteCommandResult(fResultLog, tData.eCmd, "decoder_fail", 13);
							P_WriteTime(fResultLog, FALSE);
							break;
						}
					}
					else
					{
						LOGMSG("not support track\n");
					}
				}
				else
				{
					LOGMSG("No audio track\n");
				}

				if(MediaSession[uiSessionId].ProbeInfo.ucTotalVideoTrack != 0)
				{
					if(MediaSession[uiSessionId].ProbeInfo.video_codec != 0)
					{
						DI_CSO_SetActiveVideoTrack(gpHSession, P_CONVERT_DI_VIDEO_CODEC(MediaSession[uiSessionId].ProbeInfo.video_codec), 0x100);
						dResult = DI_VIDEO_Start(DI_VIDEO_DECODER_MAIN, P_CONVERT_DI_VIDEO_CODEC(MediaSession[uiSessionId].ProbeInfo.video_codec));
						if(dResult != DI_ERR_OK)
						{
							LOGMSG("[#ERROR] DI_VIDEO_Start failed!\n");
							P_WriteCommandResult(fResultLog, tData.eCmd, "decoder_fail", 13);
							P_WriteTime(fResultLog, FALSE);
							break;
						}
						LOGMSG("Start decorders\n");
					}
					else
					{
						LOGMSG("not support track\n");
					}
				}
				else
				{
					LOGMSG("No Video track\n");
				}

				g_bMediaEventWaiting = true;
				uiWaitingDuration = 1;
				uiErrorCheck = 0;
				while(uiWaitingDuration > 0)
				{
					while(TRUE)
					{
						if(DI_CSO_GetPosition(gpHSession,MediaSession[uiSessionId].uiPumpId,&ullCurrentOffset) != DI_ERR_OK)
						{
							LOGMSG("GetPosition Error--\n");
							break;
						}
						else
						{
//								LOGMSG("----------%llu sec------------\n", ullCurrentOffset);
						}
						if(ullCurrentOffset >= 1000)
						{
							uiWaitingDuration--;
							if(uiWaitingDuration > 0)
							{
								VK_TASK_Sleep(500);
								continue;
							}
							else
							{
								break;
							}
						}
					}
					uiErrorCheck++;
					if(uiErrorCheck >= 600)
					{
						LOGMSG("[#ERROR] Start is delayed for 1min, Just let it go.\n");
						break;
					}
					else
					{
						VK_TASK_Sleep(100);
					}
				}
				g_bMediaPlaying = TRUE;
				LOGMSG("started\n");
				P_WriteCommandResult(fResultLog, tData.eCmd, "success", 8);
				P_WriteTime(fResultLog, FALSE);
				MediaSession[uiSessionId].isPlaying = true;
				break;

			case CMD_MEDIA_COM_STOP:
				P_WriteTime(fResultLog, TRUE);
				P_WriteCommand(fResultLog, uiSessionId, tData.eCmd);
				LOGMSG("Stop Msg\n", uiSessionId);
				LOGMSG("Stop Session....\n");

				bSkipTrick = FALSE;
				g_bFlagEOS = false;
#if ((CALYPSO_VERSION_MAJOR >= 1) && (CALYPSO_VERSION_MINOR >= 1) && (CALYPSO_VERSION_MICRO >= 8) && (CALYPSO_VERSION_BCODE == 1 || CALYPSO_VERSION_BCODE == 10))
				dResult = CSO_Stop(MediaSession[uiSessionId].ulSessionId,-1);
#else
				dResult = CSO_Stop(MediaSession[uiSessionId].ulSessionId, NULL);
#endif
				if(dResult != DI_ERR_OK)
				{
					LOGMSG("[#ERROR] DI_CSO_StopSession failed!\n");
					P_WriteCommandResult(fResultLog, tData.eCmd, "stop_fail", 10);
					P_WriteTime(fResultLog, FALSE);
					break;
				}
/************************
*	Free Slot
************************/
				hResult = DI_PVR_PLAY_ReleasePlaypumpByIndex(MediaSession[uiSessionId].uiPumpId);
				if(hResult != ERR_OK)
				{
					CMD_Printf("[%s][%d] DI_PVR_FreePlaypumpIndex failed!\n", __FUNCTION__,__LINE__);
				}

/************************
*	Free Media Handle
************************/
//				(void)DI_CSO_SetCurrentMediaInstance(NULL);

				LOGMSG("Stop Decpders....\n");

//				if(MediaSession[uiSessionId].ProbeInfo.ucTotalAudioTrack != 0)
				{
					dResult = DI_AUDIO_Stop(DI_AUDIO_DECODER_MAIN);
					if(dResult != DI_ERR_OK)
					{
						LOGMSG("[#ERROR] DI_AUDIO_Stop failed!\n");
						P_WriteCommandResult(fResultLog, tData.eCmd, "decoder_fail", 13);
						P_WriteTime(fResultLog, FALSE);
//						break;
					}
				}

//				if(MediaSession[uiSessionId].ProbeInfo.ucTotalVideoTrack != 0)
				{
					dResult = DI_VIDEO_Stop(DI_VIDEO_DECODER_MAIN);
					if(dResult != DI_ERR_OK)
					{
						LOGMSG("[#ERROR] DI_VIDEO_Stop failed!\n");
						P_WriteCommandResult(fResultLog, tData.eCmd, "decoder_fail", 13);
						P_WriteTime(fResultLog, FALSE);
//						break;
					}
				}
				g_bMediaPlaying = FALSE;

				break;

			case CMD_MEDIA_COM_STOPBUF:
				P_WriteTime(fResultLog, TRUE);
				P_WriteCommand(fResultLog, uiSessionId, tData.eCmd);
				LOGMSG("Session %d....", uiSessionId);
				dResult = CSO_Control(MediaSession[uiSessionId].ulSessionId, eCSO_CTRL_STOPBUFFERING,NULL);
				if(dResult != CSO_ERR_OK)
				{
					LOGMSG("[#ERROR] CSO_Control(StopBuffering) failed!\n");
					P_WriteCommandResult(fResultLog, tData.eCmd, "stopbuf_fail", 14);
					P_WriteTime(fResultLog, FALSE);
					break;
				}
				LOGMSG("Stop Buffering\n");
				P_WriteCommandResult(fResultLog, tData.eCmd, "success", 8);
				P_WriteTime(fResultLog, FALSE);

				VK_MEM_Free(MediaSession[uiSessionId].ucBufferPath);
				MediaSession[uiSessionId].ucBufferPath = NULL;
				MediaSession[uiSessionId].isBuffering = false;
				break;

			case CMD_MEDIA_COM_DESTROY:
				for(HINT32 idx = 0; idx < ullCreatedMedia; idx++)
				{
					if(MediaSession[idx].ucBufferPath != NULL)
					{
						VK_MEM_Free(MediaSession[idx].ucBufferPath);
						MediaSession[idx].ucBufferPath = NULL;
					}
					P_WriteTime(fResultLog, TRUE);
					P_WriteCommand(fResultLog, uiSessionId, tData.eCmd);
					LOGMSG("Session %d....", idx);
					dResult = CSO_Close(MediaSession[idx].ulSessionId);
					if(dResult != CSO_ERR_OK)
					{
						LOGMSG("[#ERROR] DI_CSO_DestroySession failed!\n");
						P_WriteCommandResult(fResultLog, tData.eCmd, "destroy_fail", 13);
						P_WriteTime(fResultLog, FALSE);
						break;
					}
					LOGMSG("Session Destroyed\n");
				}
// 				g_curCso = NULL;
				P_WriteCommandResult(fResultLog, tData.eCmd, "success", 8);
				P_WriteTime(fResultLog, FALSE);
				break;
			case CMD_MEDIA_COM_SETAUDIO:
#if 0
				P_WriteTime(fResultLog, TRUE);
				P_WriteCommand(fResultLog, uiSessionId, tData.eCmd);
				LOGMSG("Session %d.... Audio Track Change %d",uiSessionId,tData.param1);
				dResult = DI_CSO_SetAudioTrack(MediaSession[uiSessionId].hSession, MediaSession[uiSessionId].unMediaId,tData.param1);
				if(dResult != DI_ERR_OK)
				{
					LOGMSG("[#ERROR] DI_CSO_SetAudioTrack failed!\n");
					P_WriteCommandResult(fResultLog, tData.eCmd, "audiotrack_fail", 15);
					P_WriteTime(fResultLog, FALSE);
					break;
				}
				dResult = DI_AUDIO_Stop(DI_AUDIO_DECODER_MAIN);
				if(dResult != DI_ERR_OK)
				{
					LOGMSG("[#ERROR] DI_AUDIO_Stop failed!\n");
					P_WriteCommandResult(fResultLog, tData.eCmd, "audiostop_fail", 14);
					P_WriteTime(fResultLog, FALSE);
					break;
				}
				dResult = DI_AUDIO_SetMediaType(DI_AUDIO_DECODER_MAIN, DI_AUDIOTYPE_MEDIA);
				if(dResult != DI_ERR_OK)
				{
					LOGMSG("[#ERROR] DI_AUDIO_SetMediaType failed!\n");
					P_WriteCommandResult(fResultLog, tData.eCmd, "setmediatype_fail", 18);
					P_WriteTime(fResultLog, FALSE);
					break;
				}
				if(MediaSession[uiSessionId].ProbeInfo.ucTotalAudioTrack != 0)
				{
					dResult = DI_AUDIO_Start(DI_AUDIO_DECODER_MAIN, MediaSession[uiSessionId].ProbeInfo.audio_codec);
					if(dResult != DI_ERR_OK)
					{
						LOGMSG("[#ERROR] DI_AUDIO_Start failed!\n");
						P_WriteCommandResult(fResultLog, tData.eCmd, "decoder_fail", 13);
						P_WriteTime(fResultLog, FALSE);
						break;
					}
				}
				else
				{
					LOGMSG("No audio track\n");
				}
				P_WriteCommandResult(fResultLog, tData.eCmd, "success", 8);
				P_WriteTime(fResultLog, FALSE);
#endif
				break;
			case CMD_MEDIA_COM_SEEK:
				P_WriteTime(fResultLog, TRUE);
				P_WriteCommand(fResultLog, uiSessionId, tData.eCmd);

/*************************
*	1. Set the offset
*************************/
				if(eOrigin == CMD_MEDIA_SEEKDIR_START)
				{
					ullTargetOffset = tData.param2;
					LOGMSG("START: %llu\n", ullTargetOffset);
					if(ullTargetOffset == 0)
					{
						g_bFlagEOS = false;
						bSkipTrick = FALSE;
					}
				}
				else if(eOrigin == CMD_MEDIA_SEEKDIR_CURFW)
				{
					ullTargetOffset = ullPreviousPosition[uiSessionId]+tData.param2;
					LOGMSG("CURFW: %llu\n", ullTargetOffset);
				}
				else if(eOrigin == CMD_MEDIA_SEEKDIR_CURBW)
				{
					ullTargetOffset = ullPreviousPosition[uiSessionId]-tData.param2;
					LOGMSG("CURBW: %llu\n", ullTargetOffset);
				}
				else if(eOrigin == CMD_MEDIA_SEEKDIR_END)
				{
					dResult = CSO_ProbeInfo(MediaSession[uiSessionId].ulSessionId, &(MediaSession[uiSessionId].ProbeInfo));
					if(dResult != DI_ERR_OK)
					{
						LOGMSG("[#ERROR] DI_CSO_GetProbeInfo failed!\n");
						break;
					}

					if(MediaSession[uiSessionId].ProbeInfo.duration != 0)
					{
						ullTargetOffset = MediaSession[uiSessionId].ProbeInfo.duration - tData.param2;
					}
					else
					{
						LOGMSG("[#ERROR] duration is 0!\n");
						break;
					}
					LOGMSG("END: %llu\n", ullTargetOffset);
				}
				else if(eOrigin == CMD_MEDIA_SEEKDIR_RAND)
				{
					dResult = CSO_ProbeInfo(MediaSession[uiSessionId].ulSessionId, &(MediaSession[uiSessionId].ProbeInfo));
					if(dResult != DI_ERR_OK)
					{
						LOGMSG("[#ERROR] DI_CSO_GetProbeInfo failed!\n");
						break;
					}

					srand(time(NULL));
					ullTargetOffset = rand();

					if(MediaSession[uiSessionId].ProbeInfo.duration != 0)
					{
						ullTargetOffset = ullTargetOffset % MediaSession[uiSessionId].ProbeInfo.duration;
					}
					else
					{
						LOGMSG("[#ERROR] duration is 0!\n");
						break;
					}
					LOGMSG("RAND: %llu\n", ullTargetOffset);
				}
				else
				{
					LOGMSG("[#ERROR] Invalid option %d\n", eOrigin);
					break;
				}
				CSO_CTRL_PARAM_t tParam;
				tParam.ullPosition = ullTargetOffset;
				dResult = CSO_Control(MediaSession[uiSessionId].ulSessionId, eCSO_CTRL_SEEK, &tParam);
				if(dResult != CSO_ERR_OK)
				{
					LOGMSG("[#ERROR] DI_CSO_SetPosition failed!\n");
					break;
				}

/*************************
*	2. Wait for 2 time events
*************************/
				g_bMediaEventWaiting = TRUE;
				uiWaitingDuration = 2;
				uiErrorCheck = 0;
				while(uiWaitingDuration > 0)
				{
					if(g_bFlagEOS == TRUE)
					{
						break;
					}
					if(g_bMediaPlaying == false)
					{
						break;
					}
					if(g_bMediaEventWaiting == false)
					{
//						LOGMSG("----------%d sec------------\n", uiWaitingDuration);
						g_bMediaEventWaiting = TRUE;
						while(TRUE)
						{
							if(DI_CSO_GetPosition(gpHSession,MediaSession[uiSessionId].uiPumpId,&ullCurrentOffset) != 0)
							{
								LOGMSG("[#ERROR] DI_CSO_GetPosition failed!\n");
								break;
							}
							else
							{
//								LOGMSG("----------%llu sec------------\n", ullCurrentOffset);
							}
							if(ullCurrentOffset > ullTargetOffset)
							{
								uiWaitingDuration--;
								if(uiWaitingDuration > 0)
								{
									VK_TASK_Sleep(500);
									continue;
								}
								else
								{
									break;
								}
							}
						}
					}
					uiErrorCheck++;
					if(uiErrorCheck >= 300)
					{
						LOGMSG("[#ERROR] Start is delayed for 1min, Just let it go.\n");
						break;
					}
					else
					{
						VK_TASK_Sleep(100);
					}
				}
				if(g_bFlagEOS == TRUE)
				{
					break;
				}

/*************************
*	3. Check the new position
*************************/
#if 1
				dResult = DI_CSO_GetPosition(gpHSession, MediaSession[uiSessionId].unMediaId, &ullCurrentOffset);
				if(dResult != DI_ERR_OK)
				{
					LOGMSG("[#ERROR] DI_CSO_GetPosition failed!\n");
					break;
				}
				LOGMSG("Set: %llu Result: %llu\n", ullTargetOffset, ullCurrentOffset);
#endif

/*************************
*	Keep the current position
*************************/
				ullPreviousPosition[uiSessionId] = ullCurrentOffset;

/*************************
*	4. Log the result
*************************/
				P_WriteCommandResult(fResultLog, tData.eCmd, "success", 8);

				VK_MEM_Memset(pucTemp,0,256);
				VK_snprintf(pucTemp, 27, " 	%08llu	   %08llu", ullTargetOffset, ullCurrentOffset);
				P_WriteCommandResult(fResultLog, tData.eCmd, pucTemp, 27);
				P_WriteTime(fResultLog, FALSE);
				break;

			case CMD_MEDIA_COM_TRICK:
				P_WriteTime(fResultLog, TRUE);
				P_WriteCommand(fResultLog, uiSessionId, tData.eCmd);
				if(g_bFlagEOS == TRUE)
				{
					break;
				}
				if(bSkipTrick == TRUE && eSpeed != 5 && eSpeed != 0)
				{
					LOGMSG("Current media is not support trick, so ignore trick command\n");
				}
				LOGMSG("TRICK eSpeed: %d\n", eSpeed);

				tParam.nTrick = eSpeed;
				dResult = CSO_Control(MediaSession[uiSessionId].ulSessionId, eCSO_CTRL_TRICK, &tParam);
				if(dResult != CSO_ERR_OK)
				{
					bSkipTrick = TRUE;
					LOGMSG("[#ERROR] DI_CSO_SetSpeed failed!\n");
					break;
				}
				P_WriteCommandResult(fResultLog, tData.eCmd, "success", 8);
				P_WriteTime(fResultLog, FALSE);
				break;

			case CMD_MEDIA_COM_DELAY:
				P_WriteTime(fResultLog, TRUE);
				P_WriteCommand(fResultLog, uiSessionId, tData.eCmd);
				if(g_bFlagEOS == TRUE)
				{
					break;
				}
				LOGMSG("Delay : %dms\n", ullDelayTime);
				if(ullDelayTime == 0)
				{
					while(!g_bFlagEOS);
					g_bFlagEOS=false;
					LOGMSG("--------- EOF of current session\n");
				}
				else
				{
					VK_TASK_Sleep(ullDelayTime);
				}
				P_WriteCommandResult(fResultLog, tData.eCmd, "success", 8);
				P_WriteTime(fResultLog, FALSE);
				break;

			case CMD_MEDIA_COM_WAIT:
				LOGMSG("Wait EOF of Session [%d] : ", uiSessionId);
				if(MediaSession[uiSessionId].isPlaying == true)
				{
					LOGMSG("...waiting\n");
					while(!g_bFlagEOS && g_nEOSmediaId == MediaSession[uiSessionId].unMediaId);
					g_bFlagEOS=false;
					g_nEOSmediaId=0;
					LOGMSG("---------- EOF of Session [%d]\n", uiSessionId);
				}
				else
				{
					LOGMSG("The Session is not being played\n");
				}
				break;

			case CMD_MEDIA_COM_MAX:
				break;

			default:
				LOGMSG("Unknown msg\n");
				break;

		}
		VK_memset(&tData,0x00,sizeof(CMD_MEDIA_LINE_t));
	}
	P_WriteCommandResult(fResultLog, tData.eCmd, "\ntest-end\n\n", 12);

	CMD_MEDIA_Script_Uninit();
	VK_free(pBuf);
	fclose(fResultLog);
	VK_MEM_Free(MediaSession);
	VK_MEM_Free(pucTemp);

	g_nScriptTaskId = 0;
	LOGMSG("script task terminated\n");
}
#define TEST_CERT "/clientcert.pem"
#define TEST_KEY "/clientkey.pem"
#define TEST_CA "/rootcacert.pem"

int CMD_HwTest_Parser_CALYPSO(void *szArgList)
{
	DI_ERR_CODE dResult;
	HUINT32 mediaId = 0;
	HINT32 nSel = 0;
	HINT32 nDefSel = 0;
	HBOOL bManual = FALSE;
	HINT32 nPlaymode = CSO_PLAYMODE_DNP;
	CSO_INI_t *pIni = NULL;
	HUINT8* ucPath;
	HUINT8* ucPathIdx;
	HINT32 i = 0;
	HINT32 nDefUrl = 0;
	HUINT64 ullTime = 0;
    HUINT32 ulVideoCaller = 0;
    HUINT32 ulAudioCaller = 0;
    static HBOOL isDIcallbackInstalled = FALSE;
	void* pMap = NULL;
	CSO_ERR cResult;
	HERROR hResult;

	FILE *fp = fopen("/test.log","w");
	if(fp==NULL) exit(1);
	VK_fflush(fp);
	fclose(fp);
	DI_VIDEO_Show(0,TRUE);
	LOGMSG("test.log cleared\n");
	GET_ARGS;

	if(HWTEST_CMD == NULL)
	{
		if(g_nScriptTaskId == NULL)
		{
		#if 0
			CMD_Printf("\n[Environment]\
						\n	  * [add]		: register media list\
						\n	  * [list]		: display all registered media\
						\n	  * [env]		: check current env\
						\n	  * [autocmd]	: autocmd task exection flag\
						\n\
						\n[Single file test]\
						\n	  * [playmode]	: playmode select\
						\n	  * [play]		: play single media file in given mode\
						\n	  * [stop]		: stop current work\
						\n\
						\n[MultiSession]\
						\n	  * [session]	: define the number of session\
						\n	  * [playmode]	: playmode select\
						\n	  * [fg]		: (foreground) start play (with #taskid)\
						\n	  * [bg]		: (background) stop play (with #taskid)\
						\n	  * [stop]		: stop current session\
						\n\
						\n[control]\
						\n	  * ff, rw, pause, seek\
						\n");
		#else
			CMD_Printf("\n[Single file test]\
						\n	  * [play]		: play single media file in given mode\
						\n	  * [stop]		: stop current work\
						\n\
						\n[control]\
						\n	  * ff, rw, pause, resume, seek, audio\
						\n");

		CMD_Printf("\n\n ======================================================================\
					\n =  If you want to seek playback test.                                 =\
					\n =  Insert command                                                     =\
					\n =          \"cso seek +\"  (seek 30s) , \"cso seek ++\"  (seek 60s)       =\
					\n =                                                                     =\
					\n =  If you want to trick playback test.                                =\
					\n =  Insert command                                                     =\
					\n =          \"cso trick 2~8\" (ff 2x~4x), \"cso trick -2~-8\" (ff -2~-8x)  =\
					\n =                                                                     =\
					\n =  If you want to track change test.                                  =\
					\n =  Insert command                                                     =\
					\n =                                                                     =\
					\n ======================================================================\n\n\n");
		#endif
		}
		return;
	}

	if CMD_IS("init")
	{
		if( g_bCsoInit == FALSE )
		{
			VK_memset(&g_CsoSettings,0x00,sizeof(CSO_Settings_t));
			CSO_GetDefaultSettings(&g_CsoSettings);
			g_CsoSettings.pSaveFilePath = DEFAULT_CSO_DOWNLOAD_PATH;
			g_CsoSettings.cbMediaEvent = eventCallback;
			if(CSO_Init(&g_CsoSettings) != CSO_ERR_OK)
			{
				CSO_SetDefaultSettings(&g_CsoSettings);
			}
			g_bCsoInit = TRUE;

/*************************
*	Resource Mapping
*************************/
			hResult = P_CMD_GetResourceMap(&pMap);
			if(hResult != ERR_OK)
			{
				CMD_Printf("[%s][%d] CSO_MP_GetResourceMap failed!\n", __FUNCTION__,__LINE__);
				return -1;
			}

			cResult = CSO_SetResourceMap(pMap);
			if(cResult != CSO_ERR_OK)
			{
				CMD_Printf("[%s][%d] CSO_SetResourceMap failed!\n", __FUNCTION__,__LINE__);
				return -1;
			}

			dResult = VK_TASK_Create(P_EventTask, 40, 0x8000, "CSO_EVENT_TASK", NULL, &g_nEventTaskId, 0);
			if(dResult != VK_OK)
			{
				LOGMSG("[#ERROR] VK_TASK_Create failed\n");
				return dResult;
			}
			else
			{
				LOGMSG("script task created : %d\n", g_nEventTaskId);
				VK_TASK_Start(g_nEventTaskId);
			}

/*************************
*	Player Create
*************************/
			dResult = DI_CSO_CreateSession(NULL, &gpHSession);
			if(dResult != DI_ERR_OK)
			{
				CMD_Printf("[%s][%d] DI_CSO_CreateSession failed!\n", __FUNCTION__,__LINE__);
				return -1;
			}
		}
	}
	else if CMD_IS("list")
	{
//		stSendMsg.MSGTYPE = CMD_MEDIA_MANUAL_SHOWLIST;
	}
	else if CMD_IS("env")
	{
//		stSendMsg.MSGTYPE = CMD_MEDIA_MANUAL_ENV;
//		stSendMsg.PARAM1 = CSO_ENV_PRINT;
	}
	else if CMD_IS("play")
	{
		if(	g_bCsoInit == FALSE)
		{
			VK_memset(&g_CsoSettings,0x00,sizeof(CSO_Settings_t));
			CSO_GetDefaultSettings(&g_CsoSettings);
			g_CsoSettings.pSaveFilePath = DEFAULT_CSO_DOWNLOAD_PATH;
			g_CsoSettings.cbMediaEvent = eventCallback;

			CSO_SYS_Init();
			if(CSO_Init(&g_CsoSettings) != CSO_ERR_OK)
			{
				CSO_SetDefaultSettings(&g_CsoSettings);
			}
			g_bCsoInit = TRUE;
			//DI_CSO_SetCalypsoMode(NULL);

/*************************
*	Resource Mapping
*************************/
			hResult = P_CMD_GetResourceMap(&pMap);
			if(hResult != ERR_OK)
			{
				CMD_Printf("[%s][%d] CSO_MP_GetResourceMap failed!\n", __FUNCTION__,__LINE__);
				return -1;
			}

			cResult = CSO_SetResourceMap(pMap);
			if(cResult != CSO_ERR_OK)
			{
				CMD_Printf("[%s][%d] CSO_SetResourceMap failed!\n", __FUNCTION__,__LINE__);
				return -1;
			}

			dResult = VK_TASK_Create(P_EventTask, 40, 0x8000, "CSO_EVENT_TASK", NULL, &g_nEventTaskId, 0);
			if(dResult != VK_OK)
			{
				LOGMSG("[#ERROR] VK_TASK_Create failed\n");
				return dResult;
			}
			else
			{
				LOGMSG("script task created : %d\n", g_nEventTaskId);
				VK_TASK_Start(g_nEventTaskId);
			}

/*************************
*	Player Create
*************************/
			dResult = DI_CSO_CreateSession(NULL, &gpHSession);
			if(dResult != DI_ERR_OK)
			{
				CMD_Printf("[%s][%d] DI_CSO_CreateSession failed!\n", __FUNCTION__,__LINE__);
				return -1;
			}
		}
		if(g_nScriptTaskId)
		{
			CMD_Printf("<**CmdTask**> Currently, Playing with script. please try later\n");
			return 0;
		}
		if(g_nNRSession == 1)
		{
			if(IS_CSO_PLAYING)
			{
				CMD_Printf("<**CmdTask**> Currently, Media is being played. please try later\n");
				return 0;
			}
		}
		if(g_curCso == NULL)
		{
			g_curCso = (CSO_INFO_t*)VK_MEM_Alloc(sizeof(CSO_INFO_t));
		}
		CSO_INFO_t *mMediaDescriptor = g_curCso;
		pIni = P_CMD_MEDIA_ENVManager(FALSE);
		if(pIni == NULL)
		{
			CMD_Printf("Env Init Fail!\n");
			return 0;
		}
		CMD_Printf("0. Manual (default)\n");
		CMD_Printf("1. Macro \n");
		CMD_Printf(">> ");
		VK_MEM_Memset(g_keyBuf,0x00,CMD_MEDIA_KEYBUFSIZE);
		fgets(g_keyBuf,CMD_MEDIA_KEYBUFSIZE,stdin);
		nSel = VK_atoi(g_keyBuf);
		switch(nSel)
		{
			case 0 :
				bManual = TRUE;
				break;
			case 1 :
				if(pIni->bLoad == FALSE)
				{
					bManual = TRUE;
				}
				else
				{
					bManual = FALSE;
				}
				break;
			default :
				bManual = TRUE;
				break;
		}
		if(bManual == FALSE)
		{
			if(g_ucScriptPath != NULL)
			{
				VK_MEM_Free(g_ucScriptPath);
				g_ucScriptPath = NULL;
			}
			g_ucScriptPath = strdup(DEFAULT_CSO_INI_PATH);
			CMD_Printf("Script Play ");
			goto script_operation;
		}
		else
		{
			CMD_Printf("0. DNP ");
			if(pIni->bLoad == TRUE && pIni->nPlayMode == CSO_PLAYMODE_DNP)
			{
				CMD_Printf("(Default)");
				nDefSel = CSO_PLAYMODE_DNP;
			}
			CMD_Printf("\n1. Realtime ");
			if(pIni->bLoad == TRUE && pIni->nPlayMode == CSO_PLAYMODE_RT)
			{
				CMD_Printf("(Default)");
				nDefSel = CSO_PLAYMODE_RT;
			}
			CMD_Printf("\n>> ");
			VK_MEM_Memset(g_keyBuf,0x00,CMD_MEDIA_KEYBUFSIZE);
			fgets(g_keyBuf,CMD_MEDIA_KEYBUFSIZE,stdin);
			if(g_keyBuf[0] == 10)
			{
				nSel = 2;
			}
			else
				nSel = VK_atoi(g_keyBuf);
			switch(nSel)
			{
				case 0 :
					nPlaymode = CSO_PLAYMODE_DNP;
					break;
				case 1 :
					nPlaymode = CSO_PLAYMODE_RT;
					break;
				default :
					nPlaymode = nDefSel;
					break;
			}
			pIni->nPlayMode = nPlaymode;
			if(pIni->bLoad == TRUE && pIni->pUrl != NULL)
			{
				CMD_Printf("%d. %s (Default)\n",i,pIni->pUrl);
				nDefUrl++;
			}
			for(i = 0; i < pIni->nUrlCnt ;i++)
			{
				CMD_Printf("%d. %s\n",i+nDefUrl,pIni->urls[i]);
			}
			CMD_Printf("%d. ADD URL",i+nDefUrl);
			CMD_Printf("\n>> ");
			VK_MEM_Memset(g_keyBuf,0x00,CMD_MEDIA_KEYBUFSIZE);
			fgets(g_keyBuf,CMD_MEDIA_KEYBUFSIZE,stdin);
			if(g_keyBuf[0] == 10)
			{
				nSel = 0;
			}
			else
				nSel = VK_atoi(g_keyBuf);

			if( nSel == i+nDefUrl)
			{
				CMD_Printf("URL: ");
				VK_MEM_Memset(g_keyBuf,0x00,CMD_MEDIA_KEYBUFSIZE);
				VK_scanf("%s",g_keyBuf);
				if(pIni->pUrl != NULL)
				{
					VK_MEM_Free(pIni->pUrl);
				}
				pIni->pUrl = VK_StrDup(g_keyBuf);
				if(pIni->nUrlCnt < CMD_MEDIA_MAX_ASSET)
				{
					if(pIni->urls[pIni->nUrlCnt] != NULL)
					{
						VK_MEM_Free(pIni->urls[pIni->nUrlCnt]);
					}
					pIni->urls[pIni->nUrlCnt] = VK_StrDup(pIni->pUrl);
					pIni->nUrlCnt++;
				}
				else
				{
					if(pIni->urls[0] != NULL)
					{
						VK_MEM_Free(pIni->urls[pIni->nUrlCnt]);
					}
					pIni->urls[0] = VK_StrDup(pIni->pUrl);
				}
			}
			else if( nSel == 0 && nDefUrl == 1 )
			{

			}
			else
			{
				if(pIni->pUrl != NULL)
				{
					VK_MEM_Free(pIni->pUrl);
				}
				pIni->pUrl = VK_StrDup(pIni->urls[nSel - nDefUrl]);
			}
		}

		mMediaDescriptor->url = pIni->pUrl;

#ifdef CSO_MEMORY_DEBUG
		EIKI_MEM_Init();
#endif
	        if(isDIcallbackInstalled == FALSE)
        	{
	            isDIcallbackInstalled = TRUE;
        	    DI_AUDIO_RegisterEventCallback(DI_AUDIO_DECODER_MAIN, DI_AUDIO_RUNNING_STATE_CHANGED, __cmd_hwtest_media_audio_callback, &ulAudioCaller);
	            DI_VIDEO_RegisterEventCallback(DI_VIDEO_DECODER_MAIN, DI_VIDEO_NOTIFY_EVT_RUNNING_STATE_CHANGED, __cmd_hwtest_media_video_callback, &ulVideoCaller);
        	}

		//create
		P_MeasureTime(&start);


		dResult = CSO_Open(mMediaDescriptor->url, eCSO_MODE_PLAY, &(mMediaDescriptor->ulSessionId));
		if(dResult != CSO_ERR_OK)
		{
			CMD_Printf("[#ERROR] DI_CSO_CreateSession failed!\n");
			return 0;
		}
		g_bProbeFailure[mMediaDescriptor->ulSessionId] = FALSE;
		CMD_Printf("opened\n");
		//probe
		dResult = CSO_Probe((mMediaDescriptor->ulSessionId));
		if(dResult != CSO_ERR_OK)
		{
			CMD_Printf("[#ERROR] DI_CSO_CreateSession failed!\n");
			return 0;
		}
//		mMediaDescriptor->ulSessionId = 0;
		P_MeasureTime(&probe);

		CMD_Printf("probed\n");
		if(pIni->nPlayMode != CSO_PLAYMODE_RT)
		{
			//startbuf
#ifdef	CSO_BUFFERING_PATH_HDD
			ucPath = VK_MEM_Alloc(sizeof(HUINT8)*26);
			VK_strncpy(ucPath, "dvr:///mnt/hd2/download", 24);
#else if
			ucPath = VK_MEM_Alloc(sizeof(HUINT8)*23);
			VK_strncpy(ucPath, "dvr:///home/download", 21);
#endif
			ucPathIdx = (HUINT8*)VK_MEM_Alloc(sizeof(HUINT8)*3);
			ucPathIdx[0] = '0'+mediaId/10;
			ucPathIdx[1] = '0'+mediaId%10;
			ucPathIdx[2] = '\0';

			VK_strcat(ucPath, ucPathIdx);
			VK_MEM_Free(ucPathIdx);
			mMediaDescriptor->ucBufferPath = ucPath;
			CMD_Printf("Buffering path : %s\n", mMediaDescriptor->ucBufferPath);

			VK_memset(&g_CsoSettings,0x00,sizeof(CSO_Settings_t));
			CSO_GetSettings(mMediaDescriptor->ulSessionId,&g_CsoSettings);
			g_CsoSettings.pSaveFilePath = mMediaDescriptor->ucBufferPath;

			g_CsoSettings.cbMediaEvent = eventCallback;

			CSO_SetSettings(mMediaDescriptor->ulSessionId,&g_CsoSettings);

			g_bCsoInit = TRUE;
			while((dResult = CSO_Control(mMediaDescriptor->ulSessionId, eCSO_CTRL_STARTBUFFERING,NULL)) != CSO_ERR_OK)
			{
				if(dResult == CSO_ERR_NOT_READY)
				{
//					CMD_Printf("[#DEBUG] Session is not probed yet\n");
				}
				else
				{
					CMD_Printf("[#ERROR] StartBuffering failed!\n");
					return 0;
				}
			}
			CMD_Printf("buffering start\n");
		}
#if 0
		//streameventcb
		mMediaDescriptor->StreamEventCallback.cbDummy = NULL;
		dResult = DI_CSO_RegisterStreamEventCallback(mMediaDescriptor->hSession, &(mMediaDescriptor->StreamEventCallback));
		if(dResult != DI_ERR_OK)
		{
			CMD_Printf("[#ERROR] DI_CSO_RegisterStreamEventCallback failed!\n");
			return 0;
		}
		CMD_Printf("StreamEventCB registered\n");

		//if NRSession is GT1, the playtask should be initialized for multi session mode.
		//If initialized, the playtask will not play until receiving "foreground" Msg.
		if(g_nNRSession > 1)
		{
			if(g_nCurSessionCount < g_nNRSession)
			{
				return 0;
			}
			else
			{
				CMD_Printf("<**CmdTask**> MaxSession trigger\n");
				return 0;
			}
		}

		//setupsession
		dResult = DI_CSO_SetupSession(mMediaDescriptor->hSession, &(mMediaDescriptor->unMediaId));
		if(dResult != DI_ERR_OK)
		{
			CMD_Printf("[#ERROR] DI_CSO_SetupSession failed!\n");
			return 0;
		}
		CMD_Printf("Setup\n");

		//mediaeventcb
		dResult = DI_CSO_RegisterMediaEventCallback(mMediaDescriptor->hSession, mMediaDescriptor->unMediaId, local_Media_Callback);
		if(dResult != DI_ERR_OK)
		{
			CMD_Printf("[#ERROR] DI_CSO_RegisterMediaEventCallback failed!\n");
			return 0;
		}
		CMD_Printf("MediaEventCB registered\n");
#endif
		//startdecoder

		while((dResult = CSO_ProbeInfo(mMediaDescriptor->ulSessionId, &(mMediaDescriptor->ProbeInfo))) != CSO_ERR_OK)
		{
			if(g_bProbeFailure[mMediaDescriptor->ulSessionId] == TRUE)
			{
				CMD_Printf("[#ERROR] Probe failed!\n");
				return 0;
			}
			if(dResult == CSO_ERR_NOT_READY)
			{
//				CMD_Printf("[#DEBUG] Session is not probed yet\n");
			}
			else
			{
				CMD_Printf("[#ERROR] DI_CSO_GetProbeInfo failed!\n");
				return 0;
			}
			VK_TASK_Sleep(100);
		}

		{
			int i;
			for(i=0; i < mMediaDescriptor->ProbeInfo.ucTotalVideoTrack; i++)
			{
				CMD_Printf("[CMD] Video[idx:%d] codec(%d) pid(%d) detected\n", i
					,mMediaDescriptor->ProbeInfo.stVideoTrack[i].video_codec
					,mMediaDescriptor->ProbeInfo.stVideoTrack[i].video_stream_id);
			}
			for(i=0; i < mMediaDescriptor->ProbeInfo.ucTotalAudioTrack; i++)
			{
				CMD_Printf("[CMD] Audio[idx:%d] codec(%d) pid(%d) lang(%s) detected\n", i
					,mMediaDescriptor->ProbeInfo.stAudioTrack[i].audio_codec
					,mMediaDescriptor->ProbeInfo.stAudioTrack[i].audio_stream_id
					,mMediaDescriptor->ProbeInfo.stAudioTrack[i].uacAudioLang);
			}
		}
/********************
*	Get the available playpump index
********************/
		hResult = DI_PVR_PLAY_GetPlaypumpIndex(&mMediaDescriptor->uiPumpId);
		if(hResult != ERR_OK)
		{
			CMD_Printf("[%s][%d] DI_PVR_GetPlaypumpIndex failed!\n", __FUNCTION__,__LINE__);
			return -1;
		}

		DI_CSO_SetDecoderIndex(gpHSession, DI_MEDIA_VIDEO_DECODER_MAIN, DI_MEDIA_AUDIO_DECODER_MAIN);

/********************
*	Set the Current Media Instance and initialize the internal status
********************/
//		(void)DI_CSO_SetCurrentMediaInstance(gpHSession);

/********************
*	Let DI_CSO_Dummy know the duration of contents
********************/
		(void)DI_CSO_SetDuration(gpHSession, mMediaDescriptor->ProbeInfo.duration);
		(void)DI_CSO_StartSession(gpHSession, mMediaDescriptor->ulSessionId,0);

		dResult = DI_CSO_SetCurrentPlaypump(gpHSession, mMediaDescriptor->uiPumpId);
		if(dResult != DI_ERR_OK)
		{
			CMD_Printf("DI_CSO_SetCurrentPlaypump failed!\n");
			return -1;
		}

		//startsession
		dResult = CSO_Start(mMediaDescriptor->ulSessionId, mMediaDescriptor->uiPumpId);
		if(dResult != DI_ERR_OK)
		{
			CMD_Printf("[#ERROR] DI_CSO_StartSession failed!\n");
			return 0;
		}

		g_curCso->nTrick = 100; /* 1FF */

		dResult = DI_AUDIO_SetMediaType(DI_AUDIO_DECODER_MAIN, DI_AUDIOTYPE_MEDIA);
		if(dResult != DI_ERR_OK)
		{
			CMD_Printf("[#ERROR] DI_AUDIO_SetMediaType failed!\n");
			return 0;
		}

		if(mMediaDescriptor->ProbeInfo.ucTotalAudioTrack != 0)
		{
			if(mMediaDescriptor->ProbeInfo.audio_codec != 0)
			{
			    nMediaTracks++;
			    DI_CSO_SetActiveAudioTrack(gpHSession, P_CONVERT_DI_AUDIO_CODEC(mMediaDescriptor->ProbeInfo.audio_codec), 0x101);
	 			dResult = DI_AUDIO_Start(DI_AUDIO_DECODER_MAIN, P_CONVERT_DI_AUDIO_CODEC(mMediaDescriptor->ProbeInfo.audio_codec));
				if(dResult != DI_ERR_OK)
				{
					CMD_Printf("[#ERROR] DI_AUDIO_Start failed!\n");
					return 0;
				}
			}
			else
			{
				CMD_Printf("Not support track\n");
			}
		}
		else
		{
			CMD_Printf("No audio track\n");
		}

		if(mMediaDescriptor->ProbeInfo.ucTotalVideoTrack != 0)
		{
			if(mMediaDescriptor->ProbeInfo.video_codec != 0)
			{
				nMediaTracks++;
				DI_CSO_SetActiveVideoTrack(gpHSession, P_CONVERT_DI_VIDEO_CODEC(mMediaDescriptor->ProbeInfo.video_codec), 0x100);
				dResult = DI_VIDEO_Start(DI_VIDEO_DECODER_MAIN, P_CONVERT_DI_VIDEO_CODEC(mMediaDescriptor->ProbeInfo.video_codec));
				if(dResult != DI_ERR_OK)
				{
					CMD_Printf("[#ERROR] DI_VIDEO_Start failed!\n");
					return 0;
				}
			}
			else
			{
				CMD_Printf("Not support track\n");
			}
		}
		else
		{
			CMD_Printf("No Video track\n");
		}
		CMD_Printf("Start decorders\n");

		//set current playing media
		g_nCurSessionCount++;
		ullTime = VK_TIMER_GetSystemTick();
		VK_MEM_Memset(&cmdset,0x00,sizeof(CSO_CMDSET_t));
		cmdset.cmd.eCmd = CMD_MEDIA_COM_CREATE;
		cmdset.cmd.id = 0;
		cmdset.time = ullTime;
		P_CMD_MEDIA_RegisterCommand(pIni, cmdset);
		if(pIni->nPlayMode != CSO_PLAYMODE_RT)
		{
			cmdset.cmd.eCmd = CMD_MEDIA_COM_STARTBUF;
			P_CMD_MEDIA_RegisterCommand(pIni, cmdset);
		}
		cmdset.cmd.eCmd = CMD_MEDIA_COM_START;
		P_CMD_MEDIA_RegisterCommand(pIni, cmdset);
		return 0;
	}
	else if CMD_IS("stop")
	{
//		if(g_nNRSession == 1)
		{
			if(!IS_CSO_PLAYING)
			{
				CMD_Printf("<**CmdTask**> Currently, Media is not being played.\n");
				return 0;
			}

			dResult = DI_AUDIO_Stop(DI_AUDIO_DECODER_MAIN);
			if(dResult != DI_ERR_OK)
			{
				CMD_Printf("[#ERROR] DI_AUDIO_Stop failed!\n");
//				return 0;
			}

			dResult = DI_VIDEO_Stop(DI_VIDEO_DECODER_MAIN);
			if(dResult != DI_ERR_OK)
			{
				CMD_Printf("[#ERROR] DI_VIDEO_Stop failed!\n");
//				break;
			}

#if ((CALYPSO_VERSION_MAJOR >= 1) && (CALYPSO_VERSION_MINOR >= 1) && (CALYPSO_VERSION_MICRO >= 8) && (CALYPSO_VERSION_BCODE == 1 || CALYPSO_VERSION_BCODE == 10))
			dResult = CSO_Stop(g_curCso->ulSessionId,-1);
#else
			dResult = CSO_Stop(g_curCso->ulSessionId, NULL);
#endif
			if(dResult != DI_ERR_OK)
			{
				CMD_Printf("[#ERROR] DI_CSO_StopSession failed!\n");
				return 0;
			}

/************************
*	Free Slot
************************/
			hResult = DI_PVR_PLAY_ReleasePlaypumpByIndex(g_curCso->uiPumpId);
			if(hResult != ERR_OK)
			{
				CMD_Printf("[%s][%d] DI_PVR_FreePlaypumpIndex failed!\n", __FUNCTION__,__LINE__);
			}

/************************
*	Free Media Handle
************************/
//				(void)DI_CSO_SetCurrentMediaInstance(NULL);

			CMD_Printf("Stop Decoders....\n");
#if 0
			dResult = DI_CSO_UnregisterMediaEventCallback(g_curCso->hSession, g_curCso->unMediaId);
			if(dResult != DI_ERR_OK)
			{
				CMD_Printf("[#ERROR] DI_CSO_UnegisterMediaEventCallback failed!\n");
				return 0;
			}
			dResult = DI_CSO_ReleaseSession(g_curCso->hSession, g_curCso->unMediaId);
			if(dResult != DI_ERR_OK)
			{
				CMD_Printf("[#ERROR] DI_CSO_ReleaseSession failed!\n");
				return 0;
			}
			if(g_pIni->nPlayMode != CSO_PLAYMODE_RT)
			{
				dResult = DI_CSO_StopBuffering(g_curCso->hSession);
				if(dResult != DI_ERR_OK)
				{
					CMD_Printf("[#ERROR] DI_CSO_StopBuffering failed!\n");
					return 0;
				}
			}
#endif
			dResult = CSO_Close(g_curCso->ulSessionId);
			if(dResult != CSO_ERR_OK)
			{
				CMD_Printf("[#ERROR] DI_CSO_CloseSession failed!\n");
				return 0;
			}
			g_nCurSessionCount--;
			ullTime = VK_TIMER_GetSystemTick();
			VK_MEM_Memset(&cmdset,0x00,sizeof(CSO_CMDSET_t));
			cmdset.cmd.eCmd = CMD_MEDIA_COM_DELAY;
			cmdset.cmd.id = 0;
			cmdset.cmd.param2 = ullTime - g_pIni->CmdSet[g_pIni->nCmdCursor-1].time;
			cmdset.time = ullTime;
			P_CMD_MEDIA_RegisterCommand(g_pIni, cmdset);
			cmdset.cmd.eCmd = CMD_MEDIA_COM_STOP;
			P_CMD_MEDIA_RegisterCommand(g_pIni, cmdset);
			if(g_pIni->nPlayMode != CSO_PLAYMODE_RT)
			{
				cmdset.cmd.eCmd = CMD_MEDIA_COM_STOPBUF;
				P_CMD_MEDIA_RegisterCommand(g_pIni, cmdset);
			}
			cmdset.cmd.eCmd = CMD_MEDIA_COM_DESTROY;
			P_CMD_MEDIA_RegisterCommand(g_pIni, cmdset);
			if(g_curCso != NULL)
			{
				VK_MEM_Free(g_curCso);
				g_curCso = NULL;
			}
			CMD_Printf("\n0. SAVE (default)");
			CMD_Printf("\n1. DO NOT SAVE ");
			CMD_Printf("\n>> ");
			VK_MEM_Memset(g_keyBuf,0x00,CMD_MEDIA_KEYBUFSIZE);
			fgets(g_keyBuf,CMD_MEDIA_KEYBUFSIZE,stdin);
			nSel = VK_atoi(g_keyBuf);
			if(nSel == 0)
			{
				(void)P_CMD_MEDIA_ENVManager(TRUE);
			}
#ifdef CSO_MEMORY_DEBUG
			EIKI_MEM_PrintAll();
			EIKI_MEM_Uninit();
#endif

		}
		return 0;
	}
#if 0
	else if CMD_IS("ff")
	{
		if(HWTEST_PARAM == NULL) {
			CMD_Printf("you should provide speed value\n");
			return -1;
		}
		else if(IS_CSO_PLAYING == false) {
			CMD_Printf("currently, media is not being played\n");
			return -1;
		}
		dResult = P_MediaTrick(CSO_TRICK_FF,VK_atoi(HWTEST_PARAM),g_curCso->hSession, g_curCso->unMediaId);
		if(dResult != DI_ERR_OK)
		{
			LOGMSG("[#ERROR] DI_CSO_SetSpeed failed!\n");
			return dResult;
		}
		ullTime = VK_TIMER_GetSystemTick();
		VK_MEM_Memset(&cmdset,0x00,sizeof(CSO_CMDSET_t));
		cmdset.cmd.eCmd = CMD_MEDIA_COM_DELAY;
		cmdset.cmd.id = 0;
		cmdset.cmd.param2 = ullTime - g_pIni->CmdSet[g_pIni->nCmdCursor-1].time;
		cmdset.time = ullTime;
		P_CMD_MEDIA_RegisterCommand(g_pIni, cmdset);
		cmdset.cmd.eCmd = CMD_MEDIA_COM_TRICK;
		cmdset.cmd.param2 = 0;
		cmdset.cmd.param1 = VK_atoi(HWTEST_PARAM);
		P_CMD_MEDIA_RegisterCommand(g_pIni, cmdset);
	}
	else if CMD_IS("rw")
	{
		if(HWTEST_PARAM == NULL) {
			CMD_Printf("you should provide speed value");
			return -1;
		}
		else if(IS_CSO_PLAYING == false) {
			CMD_Printf("currently, media is not being played\n");
			return -1;
		}
		dResult = P_MediaTrick(CSO_TRICK_RW,VK_atoi(HWTEST_PARAM),g_curCso->hSession, g_curCso->unMediaId);
		if(dResult != DI_ERR_OK)
		{
			LOGMSG("[#ERROR] DI_CSO_SetSpeed failed!\n");
			return dResult;
		}
		ullTime = VK_TIMER_GetSystemTick();
		VK_MEM_Memset(&cmdset,0x00,sizeof(CSO_CMDSET_t));
		cmdset.cmd.eCmd = CMD_MEDIA_COM_DELAY;
		cmdset.cmd.id = 0;
		cmdset.cmd.param2 = ullTime - g_pIni->CmdSet[g_pIni->nCmdCursor-1].time;
		cmdset.time = ullTime;
		P_CMD_MEDIA_RegisterCommand(g_pIni, cmdset);
		cmdset.cmd.eCmd = CMD_MEDIA_COM_TRICK;
		cmdset.cmd.param2 = 0;
		cmdset.cmd.param1 = -(VK_atoi(HWTEST_PARAM));
		P_CMD_MEDIA_RegisterCommand(g_pIni, cmdset);
	}
	else if CMD_IS("resume")
	{
		if(IS_CSO_PLAYING == false) {
			CMD_Printf("currently, media is not being played\n");
			return -1;
		}
		dResult = P_MediaTrick(CSO_TRICK_FF,5,g_curCso->hSession, g_curCso->unMediaId);
		if(dResult != DI_ERR_OK)
		{
			LOGMSG("[#ERROR] DI_CSO_SetSpeed failed!\n");
			return dResult;
		}
		ullTime = VK_TIMER_GetSystemTick();
		VK_MEM_Memset(&cmdset,0x00,sizeof(CSO_CMDSET_t));
		cmdset.cmd.eCmd = CMD_MEDIA_COM_DELAY;
		cmdset.cmd.id = 0;
		cmdset.cmd.param2 = ullTime - g_pIni->CmdSet[g_pIni->nCmdCursor-1].time;
		cmdset.time = ullTime;
		P_CMD_MEDIA_RegisterCommand(g_pIni, cmdset);
		cmdset.cmd.eCmd = CMD_MEDIA_COM_TRICK;
		cmdset.cmd.param2 = 0;
		cmdset.cmd.param1 = 5;
		P_CMD_MEDIA_RegisterCommand(g_pIni, cmdset);

	}
	else if CMD_IS("pause")
	{
		if(IS_CSO_PLAYING == false) {
			CMD_Printf("currently, media is not being played\n");
			return -1;
		}
		dResult = P_MediaTrick(CSO_TRICK_FF,0,g_curCso->hSession, g_curCso->unMediaId);
		if(dResult != DI_ERR_OK)
		{
			LOGMSG("[#ERROR] DI_CSO_SetSpeed failed!\n");
			return dResult;
		}
		ullTime = VK_TIMER_GetSystemTick();
		VK_MEM_Memset(&cmdset,0x00,sizeof(CSO_CMDSET_t));
		cmdset.cmd.eCmd = CMD_MEDIA_COM_DELAY;
		cmdset.cmd.id = 0;
		cmdset.cmd.param2 = ullTime - g_pIni->CmdSet[g_pIni->nCmdCursor-1].time;
		cmdset.time = ullTime;
		P_CMD_MEDIA_RegisterCommand(g_pIni, cmdset);
		cmdset.cmd.eCmd = CMD_MEDIA_COM_TRICK;
		cmdset.cmd.param2 = 0;
		cmdset.cmd.param1 = 0;
		P_CMD_MEDIA_RegisterCommand(g_pIni, cmdset);
	}
#endif
	else if CMD_IS("trick")
	{
		CMD_Printf("++ CMD_IS - trick \n");
		if(HWTEST_PARAM == NULL) {
			CMD_Printf("you should provide speed value, trick [speed] \n");
			return -1;
		}
		else if(IS_CSO_PLAYING == false) {
			CMD_Printf("currently, media is not being played\n");
			return -1;
		}
		CSO_CTRL_PARAM_t tParam;
		tParam.nTrick = VK_atoi(HWTEST_PARAM)*100;

		CMD_Printf("Set trick rate = %d \n", tParam.nTrick);

		g_curCso->nTrick = tParam.nTrick;

		// TODO current position값 받아서 현재 position에서 trick 하기.

		CSO_Control(g_curCso->ulSessionId, eCSO_CTRL_TRICK, &tParam);
		CMD_Printf("\033[1;32;40m ======>>> Send trick request!! (%d)\n\033[00m", tParam.nTrick);

#if 0
		dResult = CSO_Control(g_curCso->ulSessionId, eCSO_CTRL_TRICK, &tParam);
		if(dResult != CSO_ERR_OK)
		{
			LOGMSG("[#ERROR] DI_IPTUNER_Control failed!\n");
			return dResult;
		}
#else
		g_curCso->ullPosition = tParam.ullPosition;
		g_curCso->nTrick = tParam.nTrick;

		CSO_Control(g_curCso->ulSessionId, eCSO_CTRL_TRICK, &tParam);
		CMD_Printf("\033[1;32;40m ======>>> Send trick request!! (%d)\n\033[00m", tParam.nTrick);
#endif

#if 0
		dResult = DI_AUDIO_Stop(DI_AUDIO_DECODER_MAIN);
		if(dResult != DI_ERR_OK)
		{
			CMD_Printf("[#ERROR] DI_AUDIO_Stop failed!\n");
			return dResult;
		}
		dResult = DI_AUDIO_SetMediaType(DI_AUDIO_DECODER_MAIN, DI_AUDIOTYPE_MEDIA);
		if(dResult != DI_ERR_OK)
		{
			CMD_Printf("[#ERROR] DI_AUDIO_SetMediaType failed!\n");
			return dResult;
		}
		dResult = DI_AUDIO_Start(DI_AUDIO_DECODER_MAIN, g_curCso->ProbeInfo.audio_codec);
		if(dResult != DI_ERR_OK)
		{
			CMD_Printf("[#ERROR] DI_AUDIO_Start failed!\n");
			return dResult;
		}
#if 0
		dResult = DI_VIDEO_Stop(DI_VIDEO_DECODER_MAIN);
		if(dResult != DI_ERR_OK)
		{
			CMD_Printf("[#ERROR] DI_VIDEO_Stop failed!\n");
//				break;
		}
		dResult = DI_VIDEO_Start(DI_VIDEO_DECODER_MAIN, g_curCso->ProbeInfo.video_codec);
		if(dResult != DI_ERR_OK)
		{
			CMD_Printf("[#ERROR] DI_VIDEO_Start failed!\n");
			return 0;
		}
#endif

		DRV_VIDEO_Restart(DI_VIDEO_DECODER_MAIN, 0);
#endif


	}
	else if CMD_IS("stopbuffer")
	{
		if(HWTEST_PARAM == NULL) {
			CMD_Printf("you should provide stopbuffer value, stopbuffer [**] \n");
			return -1;
		}else if(IS_CSO_PLAYING == false) {
			CMD_Printf("currently, media is not being played\n");
			return -1;
		}
		CSO_CTRL_PARAM_t tParam;

		CSO_Control(g_curCso->ulSessionId, eCSO_CTRL_STOPBUFFERING, &tParam);
		CMD_Printf("\033[1;32;40m ======>>> Send stop buffering request!! (%d)\n\033[00m", tParam.pParam3);

	}
	else if CMD_IS("trackchange")
	{
		CSO_CTRL_PARAM_t tParam;
		CSO_ProbeInfo_t ProbeInfo;
		int32_t nVideoTracks = 0;
		int32_t nAudioTracks = 0;
		int32_t track_num = 0;
		int32_t total_track = 0;
		char input;
		int32_t select_audio_stream_id =0;
		int32_t select_video_stream_id =0;


		if(HWTEST_PARAM == NULL) {
			CMD_Printf("you should provide trackchange value, trackchange a or v\n");
			return -1;
		}else if(VK_strcmp(HWTEST_PARAM, "v")==0) {
			CMD_Printf("++ trackchange : command is video track change!! ulSessionId = %d \n", g_curCso->ulSessionId);
			nVideoTracks = g_curCso->ProbeInfo.ucTotalVideoTrack;

			for(total_track=0; total_track<=nVideoTracks; total_track++)
			{
				CMD_Printf("++ VideoTrack[%d].video_codec : %d \n", total_track, g_curCso->ProbeInfo.stVideoTrack[total_track].video_codec);
				CMD_Printf("++ VideoTrack[%d].video_stream_id : %d \n", total_track, g_curCso->ProbeInfo.stVideoTrack[total_track].video_stream_id);
				//CMD_Printf("++ VideoTrack[%d].sample_rate : %d \n", total_track, g_curCso->ProbeInfo.stVideoTrack[total_track].sample_rate);
			}
			if(g_curCso->ProbeInfo.ucTotalVideoTrack == NULL){
				CMD_Printf("++ This content is only Video Track.\n");
			}else {
				CMD_Printf("++ Total Video track ==> %d\n", g_curCso->ProbeInfo.ucTotalVideoTrack);
				CMD_Printf("++ Current Video track index ==> %d \n", g_curCso->ProbeInfo.nCurrentVideoTrack);
				CMD_Printf("++ Select track index ==> ");
				input = VK_getchar();
				CMD_Printf("++ input = %d\n", input);
				track_num = VK_atoi((char *)&input);
				CMD_Printf("++ track_num = %d\n", track_num);
				CMD_Printf("++ video_stream_id :%d \n", g_curCso->ProbeInfo.stVideoTrack[nVideoTracks].video_stream_id);
				dResult = DI_VIDEO_Stop(DI_VIDEO_DECODER_MAIN);
				if(dResult != DI_ERR_OK)
				{
					CMD_Printf("[#ERROR] DI_VIDEO_Stop failed!\n");
					return dResult;
				}

				dResult = DI_CSO_SetActiveVideoTrack(gpHSession, P_CONVERT_DI_VIDEO_CODEC(g_curCso->ProbeInfo.stVideoTrack[track_num].video_codec), g_curCso->ProbeInfo.stVideoTrack[track_num].video_stream_id);
				if(dResult != DI_ERR_OK)
				{
					CMD_Printf("[#ERROR] DI_CSO_SetActiveVideoTrack failed!\n");
					return dResult;
				}

				/*dResult = DI_AUDIO_SetMediaType(DI_VIDEO_DECODER_MAIN, DI_AUDIOTYPE_MEDIA);
				if(dResult != DI_ERR_OK)
				{
					CMD_Printf("[#ERROR] DI_AUDIO_SetMediaType failed!\n");
					return dResult;
				}*/
				dResult = DI_VIDEO_Start(DI_VIDEO_DECODER_MAIN, P_CONVERT_DI_VIDEO_CODEC(g_curCso->ProbeInfo.stVideoTrack[track_num].video_codec));
				if(dResult != DI_ERR_OK)
				{
					CMD_Printf("[#ERROR] DI_VIDEO_Start failed!\n");
					return dResult;
				}
				CMD_Printf("++ VideoTrack[%d].video_stream_id : %d \n", total_track, g_curCso->ProbeInfo.stVideoTrack[total_track].video_stream_id);
			}
			CMD_Printf("++ Select current track index ==> %d \n", tParam.ulParam1);
			CSO_Control(g_curCso->ulSessionId, eCSO_CTRL_SELECT_VIDEO_TRACK, &tParam);
			CMD_Printf("++ g_curCso->ProbeInfo.nCurrentVideoTrack = %d \n", g_curCso->ProbeInfo.nCurrentVideoTrack);
		}
		else if(VK_strcmp(HWTEST_PARAM, "a")==0) {
			CMD_Printf("++ trackchange : command is audio track change!! ulSessionId = %d \n", g_curCso->ulSessionId);
			nAudioTracks = g_curCso->ProbeInfo.ucTotalVideoTrack;

			for(total_track=0; total_track<=nAudioTracks; total_track++)
			{
				CMD_Printf("++ AudioTrack[%d,%d].audio_codec : %d \n", total_track, nAudioTracks, g_curCso->ProbeInfo.stAudioTrack[total_track].audio_codec);
				CMD_Printf("++ AudioTrack[%d,%d].audio_stream_id : %d \n", total_track, nAudioTracks, g_curCso->ProbeInfo.stAudioTrack[total_track].audio_stream_id);
				CMD_Printf("++ AudioTrack[%d,%d].sample_rate : %d \n", total_track, nAudioTracks, g_curCso->ProbeInfo.stAudioTrack[total_track].sample_rate);
			}
			if(g_curCso->ProbeInfo.ucTotalAudioTrack == NULL){
				CMD_Printf("No audio track\n");
			}else {
				CMD_Printf("++ Total Audio track ==> %d\n", g_curCso->ProbeInfo.ucTotalAudioTrack);
				CMD_Printf("++ Current Audio track index ==> %d \n", g_curCso->ProbeInfo.nCurrentAudioTrack);
				CMD_Printf("++ Select track index ==> ");
				input = VK_getchar();
				track_num = VK_atoi((char *)&input);
#if 1
				dResult = DI_AUDIO_Stop(DI_AUDIO_DECODER_MAIN);
				if(dResult != DI_ERR_OK)
				{
					CMD_Printf("[#ERROR] DI_AUDIO_Stop failed!\n");
					return dResult;
				}

				dResult = DI_CSO_SetActiveAudioTrack(gpHSession, P_CONVERT_DI_AUDIO_CODEC(g_curCso->ProbeInfo.stAudioTrack[track_num].audio_codec), g_curCso->ProbeInfo.stAudioTrack[track_num].audio_stream_id);
				if(dResult != DI_ERR_OK)
				{
					CMD_Printf("[#ERROR] DI_CSO_SetAudioTrack failed!\n");
					return dResult;
				}

				dResult = DI_AUDIO_SetMediaType(DI_AUDIO_DECODER_MAIN, DI_AUDIOTYPE_MEDIA);
				if(dResult != DI_ERR_OK)
				{
					CMD_Printf("[#ERROR] DI_AUDIO_SetMediaType failed!\n");
					return dResult;
				}
				dResult = DI_AUDIO_Start(DI_AUDIO_DECODER_MAIN, P_CONVERT_DI_AUDIO_CODEC(g_curCso->ProbeInfo.stAudioTrack[track_num].audio_codec));
				if(dResult != DI_ERR_OK)
				{
					CMD_Printf("[#ERROR] DI_AUDIO_Start failed!\n");
					return dResult;
				}
#endif
				CMD_Printf("++ ProbeInfo.stAudioTrack[%d].audio_stream_id :%d \n", track_num, g_curCso->ProbeInfo.stAudioTrack[track_num].audio_stream_id);
				CMD_Printf("++ ProbeInfo.stAudioTrack[%d].audio_codec :%d \n", track_num, g_curCso->ProbeInfo.stAudioTrack[track_num].audio_codec);

			}
			CMD_Printf("++ Select current audio_codec ==> %d \n", tParam.ulParam1);
			CSO_Control(g_curCso->ulSessionId, eCSO_CTRL_SELECT_AUDIO_TRACK, &tParam);
			CMD_Printf("++ ProbeInfo.nCurrentAudioTrack = %d \n", g_curCso->ProbeInfo.nCurrentAudioTrack);
		}

		else if(IS_CSO_PLAYING == false ) {
			CMD_Printf("currently, media is not being played\n");
			return -1;
		}else {
			CMD_Printf("you should provide trackchange value, trackchange [number] \n");
			return -1;
		}
		CMD_Printf("\033[1;32;40m ======>>> Send track change request!! (%d)\n\033[00m", tParam.ulParam1);
		CMD_Printf("\033[1;32;40m ======>>> Send track change request!! (%d)\n\033[00m", tParam.nParam);
	}
	else if CMD_IS("seek")
	{
		CSO_CTRL_PARAM_t tParam;

		if(IS_CSO_PLAYING == false ) {
			CMD_Printf("currently, media is not being played\n");
			return -1;
		}else if(HWTEST_PARAM == NULL) {
			CMD_Printf("you should provide position value (msec) ex: cso seek 3000 ");
			return -1;
		}else if(VK_strcmp(HWTEST_PARAM, "+")==0){
			CMD_Printf("++ seek : command is + 30sec\n");
			g_curCso->ullPosition = 30000;
			tParam.ullPosition = 30000;
		}else if(VK_strcmp(HWTEST_PARAM, "++")==0){
			CMD_Printf("++ seek : command is + 60sec\n");
			g_curCso->ullPosition = 60000;
			tParam.ullPosition = 60000;
		}else{
		g_curCso->ullPosition = _atohui64(HWTEST_PARAM);
		tParam.ullPosition = _atohui64(HWTEST_PARAM);
		}
		CMD_Printf("++ CMD_IS - seek --> g_curCso->ullPosition : %d\n", g_curCso->ullPosition);
		CMD_Printf("++ CMD_IS - seek --> g_curCso->ulSessionId : %d\n", g_curCso->ulSessionId);
		g_curCso->isSeek = TRUE;

		CSO_Control(g_curCso->ulSessionId, eCSO_CTRL_SEEK, &tParam);
		CMD_Printf("\033[1;32;40m ======>>> Send seek request!! (%lld)\n\033[00m", tParam.ullPosition);
#if 0
		dResult = CSO_Control(g_curCso->ulSessionId, eCSO_CTRL_SEEK, &tParam);
		if(dResult != CSO_ERR_OK)
		{
			LOGMSG("[#ERROR] DI_IPTUNER_Control failed!\n");
			return dResult;
		}
#endif
#if 0
		dResult = DI_AUDIO_Stop(DI_AUDIO_DECODER_MAIN);
		if(dResult != DI_ERR_OK)
		{
			CMD_Printf("[#ERROR] DI_AUDIO_Stop failed!\n");
			return dResult;
		}
		dResult = DI_AUDIO_SetMediaType(DI_AUDIO_DECODER_MAIN, DI_AUDIOTYPE_MEDIA);
		if(dResult != DI_ERR_OK)
		{
			CMD_Printf("[#ERROR] DI_AUDIO_SetMediaType failed!\n");
			return dResult;
		}
		dResult = DI_AUDIO_Start(DI_AUDIO_DECODER_MAIN, g_curCso->ProbeInfo.audio_codec);
		if(dResult != DI_ERR_OK)
		{
			CMD_Printf("[#ERROR] DI_AUDIO_Start failed!\n");
			return dResult;
		}
		dResult = DI_VIDEO_Stop(DI_VIDEO_DECODER_MAIN);
		if(dResult != DI_ERR_OK)
		{
			CMD_Printf("[#ERROR] DI_VIDEO_Stop failed!\n");
//				break;
		}
		dResult = DI_VIDEO_Start(DI_VIDEO_DECODER_MAIN, g_curCso->ProbeInfo.video_codec);
		if(dResult != DI_ERR_OK)
		{
			CMD_Printf("[#ERROR] DI_VIDEO_Start failed!\n");
			return 0;
		}
#endif
		ullTime = VK_TIMER_GetSystemTick();
		VK_MEM_Memset(&cmdset,0x00,sizeof(CSO_CMDSET_t));
		cmdset.cmd.eCmd = CMD_MEDIA_COM_DELAY;
		cmdset.cmd.id = 0;
		cmdset.cmd.param2 = ullTime - g_pIni->CmdSet[g_pIni->nCmdCursor-1].time;
		cmdset.time = ullTime;
		P_CMD_MEDIA_RegisterCommand(g_pIni, cmdset);
		cmdset.cmd.eCmd = CMD_MEDIA_COM_SEEK;
		cmdset.cmd.param1 = 0;
		cmdset.cmd.param2 = (unsigned long long)_atohui64(HWTEST_PARAM);
		cmdset.cmd.param3 = CMD_MEDIA_SEEKDIR_START;
		P_CMD_MEDIA_RegisterCommand(g_pIni, cmdset);
	}
	else if CMD_IS("audio")
	{
		HINT32 nAudio = 0;
		HINT32 idx = 0;
		if(HWTEST_PARAM == NULL) {
			CMD_Printf("you should provide audio stream id\n");
			return  -1;
		}
		else if(IS_CSO_PLAYING == false ) {
			CMD_Printf("currently, media is not being played\n");
			return -1;
		}
		nAudio = VK_atoi(HWTEST_PARAM);
		for(idx = 0; idx < CSO_MAX_AUDIO_TRACK; idx++)
		{
			if(g_curCso->ProbeInfo.stAudioTrack[idx].audio_stream_id == nAudio)
			{
				break;
			}
		}
		if(idx == CSO_MAX_AUDIO_TRACK)
		{
			CMD_Printf("[#ERROR] No matched audio\n");
			return -1;
		}
		dResult = DI_CSO_SetActiveAudioTrack(gpHSession, P_CONVERT_DI_AUDIO_CODEC(g_curCso->ProbeInfo.stAudioTrack[idx].audio_codec), nAudio);
		if(dResult != DI_ERR_OK)
		{
			CMD_Printf("[#ERROR] DI_CSO_SetAudioTrack failed!\n");
			return dResult;
		}
		dResult = DI_AUDIO_Stop(DI_AUDIO_DECODER_MAIN);
		if(dResult != DI_ERR_OK)
		{
			CMD_Printf("[#ERROR] DI_AUDIO_Stop failed!\n");
			return dResult;
		}
		dResult = DI_AUDIO_SetMediaType(DI_AUDIO_DECODER_MAIN, DI_AUDIOTYPE_MEDIA);
		if(dResult != DI_ERR_OK)
		{
			CMD_Printf("[#ERROR] DI_AUDIO_SetMediaType failed!\n");
			return dResult;
		}
		dResult = DI_AUDIO_Start(DI_AUDIO_DECODER_MAIN, P_CONVERT_DI_AUDIO_CODEC(g_curCso->ProbeInfo.stAudioTrack[idx].audio_codec));
		if(dResult != DI_ERR_OK)
		{
			CMD_Printf("[#ERROR] DI_AUDIO_Start failed!\n");
			return dResult;
		}
		else
		{
			CMD_Printf("No audio track\n");
		}
		ullTime = VK_TIMER_GetSystemTick();
		VK_MEM_Memset(&cmdset,0x00,sizeof(CSO_CMDSET_t));
		cmdset.cmd.eCmd = CMD_MEDIA_COM_DELAY;
		cmdset.cmd.id = 0;
		cmdset.cmd.param2 = ullTime - g_pIni->CmdSet[g_pIni->nCmdCursor-1].time;
		cmdset.time = ullTime;
		P_CMD_MEDIA_RegisterCommand(g_pIni, cmdset);
		cmdset.cmd.eCmd = CMD_MEDIA_COM_SETAUDIO;
		cmdset.cmd.param1 = nAudio;
		cmdset.cmd.param2 = 0;
		cmdset.cmd.param3 = 0;
		P_CMD_MEDIA_RegisterCommand(g_pIni, cmdset);
	}
	else if CMD_IS("script")
	{
		HINT32 dResult;
		if(IS_CSO_PLAYING == TRUE) {
			CMD_Printf("currently, media is being played\n");
			return 0;
		}

		if(g_ucScriptPath != NULL)
		{
			VK_MEM_Free(g_ucScriptPath);
			g_ucScriptPath = NULL;
		}

		if(HWTEST_PARAM != NULL)
		{
			g_ucScriptPath = strdup(HWTEST_PARAM);
		}
		else
		{
			g_ucScriptPath = strdup("/test.txt");
		}

script_operation:

		LOGMSG("script file path : %s\n",g_ucScriptPath);

		dResult = VK_TASK_Create(P_ScriptCmdTask, 40, 0x8000, "CSO_SCRIPT_TASK", NULL, &g_nScriptTaskId, 0);
		if(dResult != VK_OK)
		{
			LOGMSG("[#ERROR] VK_TASK_Create failed\n");
			return dResult;
		}
		else
		{
			LOGMSG("script task created : %d\n", g_nScriptTaskId);
			VK_TASK_Start(g_nScriptTaskId);
		}
	}
	else if CMD_IS("info")
	{
#if 0
		HUINT32 uiChunkNum = 0;
		HINT32 *iStartOffsets = 0;
		HINT32 *iEndOffsets = 0;
		HINT32 uiIndex = 0;
		DI_CSO_SessionSettings_t sSettings;

		if(g_curCso == NULL)
		{
			CMD_Printf("Current media info is NULL \n");
			return 0;
		}
		DI_CSO_GetBufferInfo(g_curCso->hSession, &uiChunkNum, &iStartOffsets, &iEndOffsets);
		CMD_Printf("Chunks: %d\n", uiChunkNum);
		for(uiIndex=0; uiIndex<uiChunkNum; uiIndex++)
		{
					CMD_Printf("[%d]th chunk: %d msec ~ %d msec\n", uiIndex, iStartOffsets[uiIndex], iEndOffsets[uiIndex]);
		}
		CMD_Printf("=============================================\n");

		DI_CSO_GetSessionSettings(g_curCso->hSession, &sSettings);
		if(sSettings.eUnderflowMode == DI_CSO_UNDERFLOW_AUTO)
		{
					CMD_Printf("Underflow Mode: AUTO\n");
		}
		else if(sSettings.eUnderflowMode == DI_CSO_UNDERFLOW_THRESHOLD)
		{
					CMD_Printf("Underflow Mode: THRESHOLD\n");
		}

		if(sSettings.eBufferingMode == DI_CSO_MODE_FIXED)
		{
					CMD_Printf("Buffering Mode: FIXED\n");
		}
		else if(sSettings.eBufferingMode == DI_CSO_MODE_UNLIMITED)
		{
					CMD_Printf("Buffering Mode: UNLIMITED\n");
		}
		else if(sSettings.eBufferingMode == DI_CSO_MODE_THRESHOLD)
		{
					CMD_Printf("Buffering Mode: TRHESHOLD\n");
					CMD_Printf("Buffering Threshold: %d msec\n", sSettings.tBufferingParams.nBufferThreshold);
					CMD_Printf("Buffering Start Threshold: %d msec\n", sSettings.tPlaybackStartParams.ulStartThreshold);
					CMD_Printf("Buffering Start Constants: %d msec\n", sSettings.tPlaybackStartParams.ulStartConstant);
		}
#endif
	}
	else
	{
		CMD_Printf("Invalid Command!\n");
	}
	return 0;
}

#if 0
static DI_ERR_CODE P_MediaTrick(HUINT8 TRICKCMD, HUINT64 uiSpeed, SESSION_HANDLE hSession, HUINT32 unMediaId)
{
	DI_CSO_PlaySpeed_e eSpeed;
	DI_ERR_CODE dResult;

	if(TRICKCMD == CSO_TRICK_FF)
	{
		if(uiSpeed == 0)
		{
			LOGMSG("Pause\n");
			eSpeed = eDI_MEDIA_Speed_Pause;
		}
		else if(uiSpeed == 1)
		{
			LOGMSG("1/10 FF\n");
			eSpeed = eDI_MEDIA_Speed_FF_X_1_10;
		}
		else if(uiSpeed == 2)
		{
			LOGMSG("1/8 FF\n");
			eSpeed =  eDI_MEDIA_Speed_FF_X_1_8;
		}
		else if(uiSpeed == 3)
		{
			LOGMSG("1/4 FF\n");
			eSpeed =  eDI_MEDIA_Speed_FF_X_1_4;
		}
		else if(uiSpeed == 4)
		{
			LOGMSG("1/2 FF\n");
			eSpeed =  eDI_MEDIA_Speed_FF_X_1_2;
		}
		else if(uiSpeed == 5)
		{
			LOGMSG("1 FF\n");
			eSpeed =  eDI_MEDIA_Speed_Normal;
		}
		else if(uiSpeed == 6)
		{
			LOGMSG("2 FF\n");
			eSpeed =  eDI_MEDIA_Speed_FF_X2;
		}
		else if(uiSpeed == 7)
		{
			LOGMSG("4 FF\n");
			eSpeed =  eDI_MEDIA_Speed_FF_X4;
		}
		else if(uiSpeed == 8)
		{
			LOGMSG("8 FF\n");
			eSpeed =  eDI_MEDIA_Speed_FF_X8;
		}
		else if(uiSpeed == 9)
		{
			LOGMSG("16 FF\n");
			eSpeed =  eDI_MEDIA_Speed_FF_X16;
		}
		else if(uiSpeed == 10)
		{
			LOGMSG("32FF\n");
			eSpeed =  eDI_MEDIA_Speed_FF_X32;
		}
		else if(uiSpeed == 11)
		{
			LOGMSG("64 FF\n");
			eSpeed =  eDI_MEDIA_Speed_FF_X64;
		}

		dResult = DI_CSO_SetSpeed(hSession, unMediaId, eSpeed);
		if(dResult != DI_ERR_OK)
		{
			LOGMSG("[#ERROR] DI_CSO_SetSpeed failed!\n");
			return dResult;
		}
	}

	else if(TRICKCMD == CSO_TRICK_RW)
	{
		if(uiSpeed == 0)
		{
			LOGMSG("Pause\n");
			eSpeed = eDI_MEDIA_Speed_Pause;
		}
		else if(uiSpeed == 1)
		{
			LOGMSG("1/10 RW\n");
			eSpeed = eDI_MEDIA_Speed_RW_X_1_10;
		}
		else if(uiSpeed == 2)
		{
			LOGMSG("1/8 RW\n");
			eSpeed =  eDI_MEDIA_Speed_RW_X_1_8;
		}
		else if(uiSpeed == 3)
		{
			LOGMSG("1/4 RW\n");
			eSpeed =  eDI_MEDIA_Speed_RW_X_1_4;
		}
		else if(uiSpeed == 4)
		{
			LOGMSG("1/2 RW\n");
			eSpeed =  eDI_MEDIA_Speed_RW_X_1_2;
		}
		else if(uiSpeed == 5)
		{
			LOGMSG("1 RW\n");
			eSpeed =  eDI_MEDIA_Speed_RW_X1;
		}
		else if(uiSpeed == 6)
		{
			LOGMSG("2 RW\n");
			eSpeed =  eDI_MEDIA_Speed_RW_X2;
		}
		else if(uiSpeed == 7)
		{
			LOGMSG("4 RW\n");
			eSpeed =  eDI_MEDIA_Speed_RW_X4;
		}
		else if(uiSpeed == 8)
		{
			LOGMSG("8 RW\n");
			eSpeed =  eDI_MEDIA_Speed_RW_X8;
		}
		else if(uiSpeed == 9)
		{
			LOGMSG("16 RW\n");
			eSpeed =  eDI_MEDIA_Speed_RW_X16;
		}
		else if(uiSpeed == 10)
		{
			LOGMSG("32 RW\n");
			eSpeed =  eDI_MEDIA_Speed_RW_X32;
		}
		else if(uiSpeed == 11)
		{
			LOGMSG("64 RW\n");
			eSpeed =  eDI_MEDIA_Speed_RW_X64;
		}
		dResult = DI_CSO_SetSpeed(hSession, unMediaId, eSpeed);
		if(dResult != DI_ERR_OK)
		{
			LOGMSG("[#ERROR] DI_CSO_SetSpeed failed!\n");
			return dResult;
		}

	}

	else if(TRICKCMD == CSO_TRICK_SEEK)
	{
			LOGMSG("Seek, Time: %llu msec\n", uiSpeed);

			dResult = DI_CSO_SetPosition(hSession, unMediaId, uiSpeed);

			if(dResult != DI_ERR_OK)
			{
				LOGMSG("[#ERROR] DI_IPTUNER_Control failed!\n");
				return dResult;
			}
			return dResult;
	}

	else if(TRICKCMD == CSO_TRICK_PAUSE)
	{
			LOGMSG("pause\n");

			dResult = DI_CSO_SetSpeed(hSession, unMediaId, eDI_MEDIA_Speed_Pause);

			if(dResult != DI_ERR_OK)
			{
				LOGMSG("[#ERROR] DI_IPTUNER_Control failed!\n");
				return dResult;
			}
			return dResult;
	}
}
#endif

static void __cmd_hwtest_media_audio_callback(void *pvParam)
{
    DI_AUDIO_CALLBACK_DATA_t *pstCallback = NULL;
    pstCallback = (DI_AUDIO_CALLBACK_DATA_t*)pvParam;
    switch(pstCallback->nEvent)
    {
        case DI_AUDIO_RUNNING_STATE_CHANGED:
            switch (((DI_AUDIO_RUNNING_STATE_EVT_t*)pstCallback->pvEventData)->state)
            {
                case DI_AUDIO_STATE_DECODING:
                    nMediaTracks--;
                    if(nMediaTracks == 0)
                    {
                        P_MeasureTime(&start);
                        P_PrintTime(&start);
                        P_PrintTime(&probe);
                    }
                    break;
                case DI_AUDIO_STATE_NODATA:
                    break;
            }
            break;

        default:
            break;
    }
}

static void __cmd_hwtest_media_video_callback(void *pvParam)
{
    DI_VIDEO_CALLBACK_DATA_t    *pstCallback = NULL;
    DI_VIDEO_RUNNING_STATE_EVT_t    *pstRunningStateEvt = NULL;

    pstCallback = (DI_VIDEO_CALLBACK_DATA_t*)pvParam;
    switch(pstCallback->eEvent)
    {
        case DI_VIDEO_NOTIFY_EVT_RUNNING_STATE_CHANGED:
            pstRunningStateEvt = (DI_VIDEO_RUNNING_STATE_EVT_t*)pstCallback->pvEventData;
            switch(pstRunningStateEvt->state)
            {
                case DI_VIDEO_RUNNING_STATE_DECODING:
                    nMediaTracks--;
                    if(nMediaTracks == 0)
                    {
                        P_MeasureTime(&start);
                        P_PrintTime(&start);
                        P_PrintTime(&probe);
                    }
                    break;
                case DI_VIDEO_RUNNING_STATE_NODATA:
                    break;
                default:
                    break;
            }
        default:
            break;
    }
}

#if defined(CONFIG_BRCM_MOD_MSDRM_PRDY_25_SUPPORT)
void * Oem_Create(void)
{
	CMD_Printf("%s(%d) This is Dummy Function\n", __FUNCTION__, __LINE__);
    return NULL;
}

void Oem_Destroy(void *f_pOEMContext)
{
    CMD_Printf("%s(%d) This is Dummy Function\n", __FUNCTION__, __LINE__);
    return;
}
#endif

#undef HWTEST_CMD
#undef HWTEST_PARAM
#undef HWTEST_PARAM1
#undef HWTEST_PARAM2
#undef GET_ARGS
#undef PARAM_IS
#undef PARAM1_IS
#undef PARAM2_IS
#undef READABLE_IN_HEX
#undef READABLE_IN_DEC



