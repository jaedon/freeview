#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <time.h>

#include "di_media20.h"
#include "htype.h"
#include "bstd_defs.h"
#include "vkernel.h"
#include "taskdef.h"
#include "cmdtool.h"
#include "di_err.h"
#include "di_media_types.h"
#include "cmd_hwtest_parser_media.h"
#include "cmd_hwtest_parser_media_scriptengine.h"

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
	{ FILE *fp = fopen("/var/lib/humaxtv/test.log","a+");	\
	if(fp==NULL) exit(1);					\
	VK_fprintf(fp,__VA_ARGS__);				\
	VK_fflush(fp); 							\
	CMD_Printf(__VA_ARGS__);				\
	fclose(fp); 							\
	}
#else if
#define LOGMSG(...) CMD_Printf(__VA_ARGS__)
#endif

#define MEDIA_BUFFERING_PATH_HDD		/* 정의되지 않으면 NFS 경로 (/home/[bufferingfile])로 버퍼링 파일을 생성 */
#define MEDIA_STRESS_TEST				/* script test 시 scenario를 반복함 */
//#define MEDIA_MEMORY_DEBUG			/* memory leakage 검사 수행 및 case 시작 시 system memory 현황을 출력 */
#ifdef MEDIA_MEMORY_DEBUG
#include "util_memtool.h"
#endif

//test mode and part
enum { MEDIA_PLAYMODE_DNP = 0, MEDIA_PLAYMODE_RT, MEDIA_PLAYMODE_ADAPTIVE, NR_MEDIA_PLAYMODE };
//sub command of trick
enum { MEDIA_TRICK_FF, MEDIA_TRICK_RW, MEDIA_TRICK_PAUSE, MEDIA_TRICK_SEEK, NR_MEDIA_TRICK};
//state
enum { MEDIA_STATE_NOT_PLAYING, MEDIA_STATE_PLAYING, NR_MEDIA_STATE };

typedef struct MEDIA_CMDSET_s {
	CMD_MEDIA_LINE_t cmd;
	HUINT64 time;
} MEDIA_CMDSET_t;

typedef struct MEDIA_INI_s {
	HBOOL bLoad;
	HUINT8 *pUrl;
	HINT32 nPlayMode;
	/* cmd manage */
	HUINT32 nCmdCursor;
	MEDIA_CMDSET_t CmdSet[CMD_MEDIA_MAX_CMD];
	/* URL List */
	HUINT32 nUrlCnt;
	HUINT8 *urls[CMD_MEDIA_MAX_ASSET];
} MEDIA_INI_t;

typedef struct MEDIA_INFO {
	HUINT8 *url;
	SESSION_HANDLE hSession;
	HUINT8 unMediaId;
	DI_MEDIA_INFO_t ProbeInfo;
	DI_MEDIA_BufferCallback_t BufferCallback;
	DI_MEDIA_StreamEventCallback_t StreamEventCallback;
	HUINT8 *ucBufferPath;
	bool isOpened;
	bool isPlaying;
	bool isBuffering;
	bool isAutoReaply;
}MEDIA_INFO_t;


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

//global variable
static MEDIA_INFO_t *g_curMedia = NULL;
static MEDIA_INI_t *g_pIni = NULL;
static MEDIA_CMDSET_t cmdset;
static HINT8 g_keyBuf[CMD_MEDIA_KEYBUFSIZE]={0,};
static HINT32 g_nCmdTaskId;
static HINT32 g_nScriptTaskId;
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
HBOOL g_bSkipTrick = FALSE;
HBOOL g_bSkipSeek = FALSE;

#define DEFAULT_MEDIA_INI_PATH "/var/lib/humaxtv/media.ini"

#define IS_MEDIA_PLAYING (g_nCurSessionCount != MEDIA_STATE_NOT_PLAYING)

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

void local_Media_Callback(HUINT32 ulDeviceId, HUINT32 ulEvent, HUINT32 Value);
static void __cmd_hwtest_media_audio_callback(void *pvParam);
static void __cmd_hwtest_media_video_callback(void *pvParam);
static DI_ERR_CODE P_MediaTrick(HUINT8 TRICKCMD, HUINT64 uiSpeed, SESSION_HANDLE hSession, HUINT32 unMediaId);

extern CMD_MEDIA_TAG_TABLE_t g_TagTable[];


void P_MeasureTime(timeHandle_t *pHandle)
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

void P_PrintTime(timeHandle_t *pHandle)
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

void dnpFailure(HUINT32 ulTunerId)
{
	/* next case start */
	g_bSkipSeek = TRUE;
	g_bSkipTrick = TRUE;
}

void dnpCmdFailure(HUINT32 ulTunerId)
{

	HUINT32 dResult = 0;

			if(!IS_MEDIA_PLAYING)
			{
				CMD_Printf("<**CmdTask**> Currently, Media is not being played.\n");
				return 0;
			}
			dResult = DI_MEDIA_StopSession(g_curMedia->hSession, g_curMedia->unMediaId);
			if(dResult != DI_ERR_OK)
			{
				CMD_Printf("[#ERROR] DI_MEDIA_StopSession failed!\n");
				return 0;
			}
			CMD_Printf("Stop Decoders....\n");

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

			dResult = DI_MEDIA_UnregisterMediaEventCallback(g_curMedia->hSession, g_curMedia->unMediaId);
			if(dResult != DI_ERR_OK)
			{
				CMD_Printf("[#ERROR] DI_MEDIA_UnegisterMediaEventCallback failed!\n");
				return 0;
			}
			dResult = DI_MEDIA_ReleaseSession(g_curMedia->hSession, g_curMedia->unMediaId);
			if(dResult != DI_ERR_OK)
			{
				CMD_Printf("[#ERROR] DI_MEDIA_ReleaseSession failed!\n");
				return 0;
			}
			if(g_pIni->nPlayMode != MEDIA_PLAYMODE_RT)
			{
				dResult = DI_MEDIA_StopBuffering(g_curMedia->hSession);
				if(dResult != DI_ERR_OK)
				{
					CMD_Printf("[#ERROR] DI_MEDIA_StopBuffering failed!\n");
					return 0;
				}
			}
			dResult = DI_MEDIA_CloseSession(g_curMedia->hSession);
			if(dResult != DI_ERR_OK)
			{
				CMD_Printf("[#ERROR] DI_MEDIA_CloseSession failed!\n");
				return 0;
			}
			dResult = DI_MEDIA_DestroySession(g_curMedia->hSession);
			if(dResult != DI_ERR_OK)
			{
				CMD_Printf("[#ERROR] DI_MEDIA_DestroySession failed!\n");
				return 0;
			}
			g_nCurSessionCount--;

}

void dnpCanPlay(HUINT32 ulTunerId)
{
    return;
}
void dnpPause(HUINT32 ulTunerId)
{
    DI_MEDIA_SetSpeed(g_curMedia->hSession, g_curMedia->unMediaId, 0);
    return;
}
void dnpResume(HUINT32 ulTunerId)
{
    DI_MEDIA_SetSpeed(g_curMedia->hSession, g_curMedia->unMediaId, 5);
    return;
}

void P_WriteCommandResult(FILE* fd, CMD_MEDIA_COM_e eCmd, char* pucResult, HUINT32 uiLength)
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

void P_WriteTime(FILE* fd, HBOOL bStart)
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

void P_WriteCommand(FILE* fd, HUINT32 uiSessionId, CMD_MEDIA_COM_e eCmd)
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

static void P_CMD_MEDIA_RegisterCommand(MEDIA_INI_t *pIni, MEDIA_CMDSET_t cmdset)
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
static void P_CMD_MEDIA_ToScript(FILE *fp, MEDIA_INI_t *pIni)
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

static MEDIA_INI_t* P_CMD_MEDIA_ENVManager(HBOOL bSave)
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
			fp=fopen(DEFAULT_MEDIA_INI_PATH,"w+b");
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
			g_pIni = VK_MEM_Alloc(sizeof(MEDIA_INI_t));
			if(g_pIni == NULL)
			{
				CMD_Printf("Memory Allocation Fail\n");
				return NULL;
			}
			VK_MEM_Memset(g_pIni,0x00,sizeof(MEDIA_INI_t));
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
		VK_MEM_Memset(g_pIni,0x00,sizeof(MEDIA_INI_t));
		fp=fopen(DEFAULT_MEDIA_INI_PATH,"r+b");
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

static void P_ScriptCmdTask(void* args)
{
	FILE* fp;
	FILE* fResultLog=NULL;
	CMD_MEDIA_LINE_t tData;
	HINT8 *pBuf = NULL;
	HINT32 size = 0;

	fp=fopen(g_ucScriptPath,"rb");
	if(fp==NULL)
	{
		CMD_Printf("File not found\n");
		return;
	}

	fseek(fp,0,SEEK_END);
	size = ftell(fp);
	fseek(fp,0,SEEK_SET);
	pBuf = VK_malloc(size);
	fread(pBuf,size,1,fp);
	VK_MEM_Memset(&tData,0x00,sizeof(CMD_MEDIA_LINE_t));
	CMD_MEDIA_Script_Init(pBuf,size);

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
	DI_MEDIA_PlaySpeed_e eSpeed;
	CMD_MEDIA_SEEKDIR_e eOrigin;
	HUINT32 uiIndex=0;
	HUINT8* ucPath;
	HUINT8* ucPathIdx;
	char* pucTemp=NULL;
    HUINT32 ulVideoCaller = 0;
    HUINT32 ulAudioCaller = 0;
	MEDIA_INFO_t *MediaSession = NULL;
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

	MediaSession = (MEDIA_INFO_t*)VK_MEM_Alloc(sizeof(MEDIA_INFO_t)*MAX_SESSION);
	if(MediaSession == NULL)
	{
		LOGMSG("[#ERROR] Out of Memory!\n");
		return;
	}
	VK_MEM_Memset(MediaSession, 0x00, sizeof(MEDIA_INFO_t)*MAX_SESSION);

	pucTemp = VK_MEM_Alloc(256);
	if(pucTemp == NULL)
	{
		LOGMSG("[#ERROR] Out of Memory!\n");
		return;
	}

#ifdef MEDIA_MEMORY_DEBUG
	EIKI_MEM_Init();
#endif

	//do Start
	VK_snprintf(pucTemp, 44,"Command:	   Result:	   param1:	   param2");
	P_WriteCommandResult(fResultLog, tData.eCmd, pucTemp, 44);
	while(true)
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
#ifdef MEDIA_STRESS_TEST //for stress test
				LOGMSG("       reinitialize\n");
				CMD_MEDIA_Script_Uninit();
				fseek(fp,0,SEEK_SET);
#endif
#ifdef MEDIA_MEMORY_DEBUG
				EIKI_MEM_PrintAll();
				EIKI_MEM_Uninit();
#ifdef MEDIA_STRESS_TEST
				EIKI_MEM_Init();
#endif
#endif
#ifdef MEDIA_STRESS_TEST //for stress test

				VK_MEM_Memset(&tData,0x00,sizeof(CMD_MEDIA_LINE_t));
				CMD_MEDIA_Script_Init(pBuf,size);
				continue;
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
#ifdef MEDIA_MEMORY_DEBUG
				VK_SYSTEM_Command("echo 3 > /proc/sys/vm/drop_caches");
				VK_TASK_Sleep(1500);
				VK_SYSTEM_Command("free");
#endif
				break;

			case CMD_MEDIA_COM_LOG:
				LOGMSG("[Script] %s\n",tData.param4);
				P_WriteTime(fResultLog, TRUE);
				P_WriteCommand(fResultLog, uiSessionId, tData.eCmd);
				P_WriteCommandResult(fResultLog, tData.eCmd, (char*) tData.param4, VK_strlen((char*)tData.param4)+1);
				P_WriteTime(fResultLog, FALSE);
				VK_MEM_Free(tData.param4);
				break;

			case CMD_MEDIA_COM_TESTEND:
				LOGMSG("Test case finished\n");

				for(HINT32 idx=0; idx < nURLIdx; idx++)
				{
					VK_MEM_Free(MediaSession[idx].url);
				}

				nURLIdx = 0;
				break;

			case CMD_MEDIA_COM_URL:
				LOGMSG("Asset Section[%d] : %s....", nURLIdx, tData.param4);
				MediaSession[nURLIdx].url = tData.param4;
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
				LOGMSG("Create Session : #%d\n",ullCreatedMedia);
				for( HINT32 idx=0; idx < ullCreatedMedia; idx++)
				{
					LOGMSG("Session %d....", idx);
					dResult = DI_MEDIA_CreateSession(MediaSession[idx].url, &(MediaSession[idx].hSession));
					if(dResult != DI_ERR_OK)
					{
						LOGMSG("[#ERROR] DI_MEDIA_CreateSession failed!\n");
						P_WriteCommandResult(fResultLog, tData.eCmd, "create_fail", 12);

						VK_MEM_Memset(pucTemp,0,256);
						VK_snprintf(pucTemp, VK_strlen(MediaSession[idx].url)+6, "	 %s", MediaSession[idx].url);
						P_WriteCommandResult(fResultLog, tData.eCmd, pucTemp, VK_strlen(MediaSession[idx].url)+6);
						P_WriteTime(fResultLog, FALSE);
						break;
					}
					LOGMSG("created\n");
				}
				for( HINT32 idx=0; idx < ullCreatedMedia; idx++)
				{
					LOGMSG("Session %d....", idx);
					dResult = DI_MEDIA_ProbeSession(MediaSession[idx].hSession);
					if(dResult != DI_ERR_OK)
					{
						LOGMSG("[#ERROR] DI_MEDIA_ProbeSession failed!\n");
						P_WriteCommandResult(fResultLog, tData.eCmd, "probe_fail", 11);

						VK_MEM_Memset(pucTemp,0,256);
						VK_snprintf(pucTemp, VK_strlen(MediaSession[idx].url)+6, "	 %s", MediaSession[idx].url);
						P_WriteCommandResult(fResultLog, tData.eCmd, pucTemp, VK_strlen(MediaSession[idx].url)+6);
						P_WriteTime(fResultLog, FALSE);
						break;
					}
					LOGMSG("probed\n");
				}

				P_WriteCommandResult(fResultLog, tData.eCmd, "success", 8);
				P_WriteTime(fResultLog, FALSE);
				break;

			case CMD_MEDIA_COM_STARTBUF:
				P_WriteTime(fResultLog, TRUE);
				P_WriteCommand(fResultLog, uiSessionId, tData.eCmd);
                g_curMedia = &MediaSession[uiSessionId];
#ifdef	MEDIA_BUFFERING_PATH_HDD
				ucPath = VK_MEM_Alloc(sizeof(HUINT8)*26);
				VK_strncpy(ucPath, "dvr:///mnt/hd2/download", 24);
#else if
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
				}
				MediaSession[uiSessionId].ucBufferPath = ucPath;
				LOGMSG("Buffering path : %s\n", MediaSession[uiSessionId].ucBufferPath);

				LOGMSG("Session %d...", uiSessionId);
				MediaSession[uiSessionId].BufferCallback.cbStart = NULL;
				MediaSession[uiSessionId].BufferCallback.cbUnderflow = NULL;
				MediaSession[uiSessionId].BufferCallback.cbComplete = NULL;
				MediaSession[uiSessionId].BufferCallback.cbFailure = dnpFailure;
				MediaSession[uiSessionId].BufferCallback.cbAdaptiveChange = NULL;
                MediaSession[uiSessionId].BufferCallback.cbCanPlay = dnpCanPlay;
                MediaSession[uiSessionId].BufferCallback.cbPause   = dnpPause;
                MediaSession[uiSessionId].BufferCallback.cbResume  = dnpResume;
				dResult = DI_MEDIA_RegisterBufferingEventCallback(MediaSession[uiSessionId].hSession, &(MediaSession[uiSessionId].BufferCallback));
				if(dResult != DI_ERR_OK)
				{
					LOGMSG("[#ERROR] DI_MEDIA_RegisterBufferingEventCallback failed!\n");
					P_WriteCommandResult(fResultLog, tData.eCmd, "register_event_callback_fail", 29);
					P_WriteTime(fResultLog, FALSE);
					break;
				}
				LOGMSG("BufferingEventCB registered\n");

				LOGMSG("Session %d...", uiSessionId);
				dResult = DI_MEDIA_StartBuffering(MediaSession[uiSessionId].hSession, MediaSession[uiSessionId].ucBufferPath);
				if(dResult != DI_ERR_OK)
				{
				   LOGMSG("[#ERROR] DI_MEDIA_StartBuffering failed!\n");
					P_WriteCommandResult(fResultLog, tData.eCmd, "startbuf_fail", 14);
					P_WriteTime(fResultLog, FALSE);
					break;
				}
				MediaSession[uiSessionId].isBuffering = true;
				LOGMSG("buffering start\n");
				P_WriteCommandResult(fResultLog, tData.eCmd, "success", 8);
				P_WriteTime(fResultLog, FALSE);
				break;

			case CMD_MEDIA_COM_START:
				P_WriteTime(fResultLog, TRUE);
				P_WriteCommand(fResultLog, uiSessionId, tData.eCmd);
                g_curMedia = &MediaSession[uiSessionId];
				if(MediaSession[uiSessionId].isOpened == false)
				{
					LOGMSG("Session %d....", uiSessionId);
					dResult = DI_MEDIA_OpenSession(MediaSession[uiSessionId].hSession);
					if(dResult != DI_ERR_OK)
					{
						LOGMSG("[#ERROR] DI_MEDIA_OpenSession failed!\n");
					P_WriteCommandResult(fResultLog, tData.eCmd, "open_fail", 10);
					P_WriteTime(fResultLog, FALSE);
						break;
					}
					LOGMSG("opened\n");
					MediaSession[uiSessionId].isOpened = true;
				}

				LOGMSG("Session %d....", uiSessionId);
				MediaSession[uiSessionId].StreamEventCallback.cbDummy = NULL;
				dResult = DI_MEDIA_RegisterStreamEventCallback(MediaSession[uiSessionId].hSession, &(MediaSession[uiSessionId].StreamEventCallback));
				if(dResult != DI_ERR_OK)
				{
					LOGMSG("[#ERROR] DI_MEDIA_RegisterStreamEventCallback failed!\n");
					P_WriteCommandResult(fResultLog, tData.eCmd, "register_stream_event_callback_fail", 36);
					P_WriteTime(fResultLog, FALSE);
					break;
				}
				LOGMSG("StreamEventCB registered\n");

				LOGMSG("Session %d....", uiSessionId);
				dResult = DI_MEDIA_SetupSession(MediaSession[uiSessionId].hSession, &(MediaSession[uiSessionId].unMediaId));
				if(dResult != DI_ERR_OK)
				{
					LOGMSG("[#ERROR] DI_MEDIA_SetupSession failed!\n");
					P_WriteCommandResult(fResultLog, tData.eCmd, "setup_fail", 11);
					P_WriteTime(fResultLog, FALSE);
					break;
				}
				LOGMSG("Setup\n");

				LOGMSG("Session %d....", uiSessionId);
				dResult = DI_MEDIA_RegisterMediaEventCallback(MediaSession[uiSessionId].hSession, MediaSession[uiSessionId].unMediaId, local_Media_Callback);
				if(dResult != DI_ERR_OK)
				{
					LOGMSG("[#ERROR] DI_MEDIA_RegisterMediaEventCallback failed!\n");
					P_WriteCommandResult(fResultLog, tData.eCmd, "register_media_event_callback_fail", 35);
					P_WriteTime(fResultLog, FALSE);
					break;
				}
				LOGMSG("MediaEventCB registered\n");

				LOGMSG("Session %d....", uiSessionId);
				dResult = DI_MEDIA_GetProbeInfo(MediaSession[uiSessionId].hSession, &(MediaSession[uiSessionId].ProbeInfo));
				if(dResult != DI_ERR_OK)
				{
					LOGMSG("[#ERROR] DI_MEDIA_GetProbeInfo failed!\n");
					P_WriteCommandResult(fResultLog, tData.eCmd, "getprobeinfo_fail", 18);
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

				if(MediaSession[uiSessionId].ProbeInfo.ucTotalVideoTrack != 0)
				{
                    dResult = DI_VIDEO_Start(DI_VIDEO_DECODER_MAIN, MediaSession[uiSessionId].ProbeInfo.video_codec);
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
					LOGMSG("No Video track\n");
				}

				LOGMSG("Session %d....", uiSessionId);
				dResult = DI_MEDIA_StartSession(MediaSession[uiSessionId].hSession, MediaSession[uiSessionId].unMediaId, FALSE);
				if(dResult != DI_ERR_OK)
				{
					CMD_Printf("[#ERROR] DI_MEDIA_StartSession failed!\n");
					P_WriteCommandResult(fResultLog, tData.eCmd, "start_fail", 11);
					P_WriteTime(fResultLog, FALSE);
					break;
				}

				g_bMediaEventWaiting = true;
				uiWaitingDuration = 1;
				uiErrorCheck = 0;
				while(uiWaitingDuration > 0)
				{
					if(g_bMediaEventWaiting == false)
					{
						LOGMSG("----------%d sec------------\n", uiWaitingDuration);
						g_bMediaEventWaiting = true;
						uiWaitingDuration--;
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

				g_bSkipTrick = FALSE;
				g_bSkipSeek = FALSE;
				g_bFlagEOS = false;
				dResult = DI_MEDIA_StopSession(MediaSession[uiSessionId].hSession, MediaSession[uiSessionId].unMediaId);
				if(dResult != DI_ERR_OK)
				{
					LOGMSG("[#ERROR] DI_MEDIA_StopSession failed!\n");
					P_WriteCommandResult(fResultLog, tData.eCmd, "stop_fail", 10);
					P_WriteTime(fResultLog, FALSE);
					break;
				}
				LOGMSG("Stop Decpders....\n");
				dResult = DI_MEDIA_GetProbeInfo(MediaSession[uiSessionId].hSession, &(MediaSession[uiSessionId].ProbeInfo));
				if(dResult != DI_ERR_OK)
				{
					LOGMSG("[#ERROR] DI_MEDIA_GetProbeInfo failed!\n");
					P_WriteCommandResult(fResultLog, tData.eCmd, "getprobeinfo_fail", 18);
					P_WriteTime(fResultLog, FALSE);
					break;
				}

				if(MediaSession[uiSessionId].ProbeInfo.ucTotalAudioTrack != 0)
				{
					dResult = DI_AUDIO_Stop(DI_AUDIO_DECODER_MAIN);
					if(dResult != DI_ERR_OK)
					{
						LOGMSG("[#ERROR] DI_AUDIO_Stop failed!\n");
						P_WriteCommandResult(fResultLog, tData.eCmd, "decoder_fail", 13);
						P_WriteTime(fResultLog, FALSE);
						break;
					}
				}

				if(MediaSession[uiSessionId].ProbeInfo.ucTotalVideoTrack != 0)
				{
					dResult = DI_VIDEO_Stop(DI_VIDEO_DECODER_MAIN);
					if(dResult != DI_ERR_OK)
					{
						LOGMSG("[#ERROR] DI_VIDEO_Stop failed!\n");
						P_WriteCommandResult(fResultLog, tData.eCmd, "decoder_fail", 13);
						P_WriteTime(fResultLog, FALSE);
						break;
					}
				}
				LOGMSG("MediaEventCallback Uninstall....\n");
				dResult = DI_MEDIA_UnregisterMediaEventCallback(MediaSession[uiSessionId].hSession, MediaSession[uiSessionId].unMediaId);
				if(dResult != DI_ERR_OK)
				{
					LOGMSG("[#ERROR] DI_MEDIA_UnegisterMediaEventCallback failed!\n");
					break;
				}
				LOGMSG("Release Session....\n");
				dResult = DI_MEDIA_ReleaseSession(MediaSession[uiSessionId].hSession, MediaSession[uiSessionId].unMediaId);
				if(dResult != DI_ERR_OK)
				{
					LOGMSG("[#ERROR] DI_MEDIA_ReleaseSession failed!\n");
					P_WriteCommandResult(fResultLog, tData.eCmd, "release_fail", 13);
					P_WriteTime(fResultLog, FALSE);
					break;
				}
				P_WriteCommandResult(fResultLog, tData.eCmd, "success", 8);
				P_WriteTime(fResultLog, FALSE);
				MediaSession[uiSessionId].isPlaying = false;


				if(MediaSession[uiSessionId].isOpened == true)
				{
					LOGMSG("Session %d....", uiSessionId);
					dResult = DI_MEDIA_CloseSession(MediaSession[uiSessionId].hSession);
					if(dResult != DI_ERR_OK)
					{
						LOGMSG("[#ERROR] DI_MEDIA_CloseSession failed!\n");
						P_WriteCommandResult(fResultLog, tData.eCmd, "close_fail", 11);
						P_WriteTime(fResultLog, FALSE);
						break;
					}
					LOGMSG("Session Closed\n");
					MediaSession[uiSessionId].isOpened = false;
				}

				break;

			case CMD_MEDIA_COM_STOPBUF:
				P_WriteTime(fResultLog, TRUE);
				P_WriteCommand(fResultLog, uiSessionId, tData.eCmd);
				LOGMSG("Session %d....", uiSessionId);
				dResult = DI_MEDIA_StopBuffering(MediaSession[uiSessionId].hSession);
				if(dResult != DI_ERR_OK)
				{
					LOGMSG("[#ERROR] DI_MEDIA_StopBuffering failed!\n");
					P_WriteCommandResult(fResultLog, tData.eCmd, "stopbuf_fail", 13);
					P_WriteTime(fResultLog, FALSE);
					break;
				}
				LOGMSG("Stop Buffering\n");
				P_WriteCommandResult(fResultLog, tData.eCmd, "success", 8);
				P_WriteTime(fResultLog, FALSE);

				dResult = DI_MEDIA_UnregisterBufferingEventCallback(MediaSession[uiSessionId].hSession);
				if(dResult != DI_ERR_OK)
				{
					LOGMSG("[#ERROR] DI_MEDIA_UnregisterBufferingEventCallback failed!\n");
					P_WriteCommandResult(fResultLog, tData.eCmd, "unregister_event_callback_fail", 29);
					P_WriteTime(fResultLog, FALSE);
					break;
				}

				VK_MEM_Free(MediaSession[uiSessionId].ucBufferPath);
				MediaSession[uiSessionId].ucBufferPath = NULL;
				MediaSession[uiSessionId].isBuffering = false;
				break;

			case CMD_MEDIA_COM_DESTROY:
				for(HINT32 idx = 0; idx < ullCreatedMedia; idx++)
				{
					if(MediaSession[idx].isBuffering == true)
					{
						P_WriteTime(fResultLog, TRUE);
						P_WriteCommand(fResultLog, uiSessionId, tData.eCmd);
						LOGMSG("Session %d....", idx);
						dResult = DI_MEDIA_StopBuffering(MediaSession[idx].hSession);
						if(dResult != DI_ERR_OK)
						{
							LOGMSG("[#ERROR] DI_MEDIA_StopBuffering failed!\n");
							P_WriteCommandResult(fResultLog, tData.eCmd, "stopbuf_fail", 13);
							P_WriteTime(fResultLog, FALSE);
							continue;
						}
						LOGMSG("Stop Buffering\n");
						dResult = DI_MEDIA_UnregisterBufferingEventCallback(MediaSession[uiSessionId].hSession);
						if(dResult != DI_ERR_OK)
						{
							LOGMSG("[#ERROR] DI_MEDIA_UnregisterBufferingEventCallback failed!\n");
							P_WriteCommandResult(fResultLog, tData.eCmd, "unregister_event_callback_fail", 29);
							P_WriteTime(fResultLog, FALSE);
							break;
						}

						if(MediaSession[idx].ucBufferPath != NULL)
						{
							VK_MEM_Free(MediaSession[idx].ucBufferPath);
							MediaSession[idx].ucBufferPath = NULL;
						}

						MediaSession[idx].isBuffering = false;
						P_WriteCommandResult(fResultLog, tData.eCmd, "success", 8);
						P_WriteTime(fResultLog, FALSE);
					}
					P_WriteTime(fResultLog, TRUE);
					P_WriteCommand(fResultLog, uiSessionId, tData.eCmd);
					LOGMSG("Session %d....", idx);
					dResult = DI_MEDIA_DestroySession(MediaSession[idx].hSession);
					if(dResult != DI_ERR_OK)
					{
						LOGMSG("[#ERROR] DI_MEDIA_DestroySession failed!\n");
						P_WriteCommandResult(fResultLog, tData.eCmd, "destroy_fail", 13);
						P_WriteTime(fResultLog, FALSE);
						break;
					}
					LOGMSG("Session Destroyed\n");
				}
                g_curMedia = NULL;
				P_WriteCommandResult(fResultLog, tData.eCmd, "success", 8);
				P_WriteTime(fResultLog, FALSE);
				break;
			case CMD_MEDIA_COM_SETAUDIO:
				P_WriteTime(fResultLog, TRUE);
				P_WriteCommand(fResultLog, uiSessionId, tData.eCmd);
				LOGMSG("Session %d.... Audio Track Change %d",uiSessionId,tData.param1);
				dResult = DI_MEDIA_SetAudioTrack(MediaSession[uiSessionId].hSession, MediaSession[uiSessionId].unMediaId,tData.param1);
				if(dResult != DI_ERR_OK)
				{
					LOGMSG("[#ERROR] DI_MEDIA_SetAudioTrack failed!\n");
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
				break;
			case CMD_MEDIA_COM_SEEK:
				P_WriteTime(fResultLog, TRUE);
				P_WriteCommand(fResultLog, uiSessionId, tData.eCmd);

				if(g_bSkipSeek == TRUE)
				{
					LOGMSG("Currently, Can not perform seek operation\n");
					break;
				}
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
						g_bSkipTrick = FALSE;
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
					dResult = DI_MEDIA_GetProbeInfo(MediaSession[uiSessionId].hSession, &(MediaSession[uiSessionId].ProbeInfo));
					if(dResult != DI_ERR_OK)
					{
						LOGMSG("[#ERROR] DI_MEDIA_GetProbeInfo failed!\n");
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
					dResult = DI_MEDIA_GetProbeInfo(MediaSession[uiSessionId].hSession, &(MediaSession[uiSessionId].ProbeInfo));
					if(dResult != DI_ERR_OK)
					{
						LOGMSG("[#ERROR] DI_MEDIA_GetProbeInfo failed!\n");
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

				if(g_bSkipSeek == TRUE)
				{
					break;
				}

				dResult = DI_MEDIA_SetPosition(MediaSession[uiSessionId].hSession, MediaSession[uiSessionId].unMediaId, ullTargetOffset);
				if(dResult != DI_ERR_OK)
				{
					LOGMSG("[#ERROR] DI_MEDIA_SetPosition failed!\n");
					break;
				}
				LOGMSG("SetPosition was success\n");
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
					if(g_bSkipSeek == TRUE)
					{
						break;
					}
					if(g_bMediaEventWaiting == false)
					{
						LOGMSG("----------%d sec------------\n", uiWaitingDuration);
						g_bMediaEventWaiting = TRUE;
						uiWaitingDuration--;
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
				dResult = DI_MEDIA_GetPosition(MediaSession[uiSessionId].hSession, MediaSession[uiSessionId].unMediaId, &ullCurrentOffset);
				if(dResult != DI_ERR_OK)
				{
					LOGMSG("[#ERROR] DI_MEDIA_GetPosition failed!\n");
					break;
				}
				LOGMSG("Set: %llu Result: %llu\n", ullTargetOffset, ullCurrentOffset);


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
				if(g_bSkipTrick == TRUE && eSpeed != 5 && eSpeed != 0)
				{
					LOGMSG("Current media is not support trick, so ignore trick command\n");
				}
				LOGMSG("TRICK eSpeed: %d\n", eSpeed);
				dResult = DI_MEDIA_SetSpeed(MediaSession[uiSessionId].hSession, MediaSession[uiSessionId].unMediaId, eSpeed);
				if(dResult != DI_ERR_OK)
				{
					g_bSkipTrick = TRUE;
					LOGMSG("[#ERROR] DI_MEDIA_SetSpeed failed!\n");
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
	fclose(fp);
	fclose(fResultLog);
	VK_MEM_Free(MediaSession);
	VK_MEM_Free(pucTemp);

	g_nScriptTaskId = 0;
	LOGMSG("script task terminated\n");
}

int CMD_HwTest_Parser_MEDIA(void *szArgList)
{
	DI_ERR_CODE dResult;
	HUINT32 mediaId = 0;
	HINT32 nSel = 0;
	HINT32 nDefSel = 0;
	HBOOL bManual = FALSE;
	HINT32 nPlaymode = MEDIA_PLAYMODE_DNP;
	MEDIA_INI_t *pIni = NULL;
	HUINT8* ucPath;
	HUINT8* ucPathIdx;
	HINT32 i = 0;
	HINT32 nDefUrl = 0;
	HUINT64 ullTime = 0;
    HUINT32 ulVideoCaller = 0;
    HUINT32 ulAudioCaller = 0;
    static HBOOL isDIcallbackInstalled = FALSE;
	FILE *fp = fopen("/var/lib/humaxtv/test.log","w");
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
		#endif
		}
		return;
	}

	if CMD_IS("add")
	{

	}
	else if CMD_IS("list")
	{
//		stSendMsg.MSGTYPE = CMD_MEDIA_MANUAL_SHOWLIST;
	}
	else if CMD_IS("env")
	{
//		stSendMsg.MSGTYPE = CMD_MEDIA_MANUAL_ENV;
//		stSendMsg.PARAM1 = MEDIA_ENV_PRINT;
	}
	else if CMD_IS("play")
	{
		if(g_nScriptTaskId)
		{
			CMD_Printf("<**CmdTask**> Currently, Playing with script. please try later\n");
			return 0;
		}
		if(g_nNRSession == 1)
		{
			if(IS_MEDIA_PLAYING)
			{
				CMD_Printf("<**CmdTask**> Currently, Media is being played. please try later\n");
				return 0;
			}
		}
		if(g_curMedia == NULL)
		{
			g_curMedia = (MEDIA_INFO_t*)VK_MEM_Alloc(sizeof(MEDIA_INFO_t));
			if (g_curMedia != NULL)
			{
				VK_memset(g_curMedia, 0, sizeof(MEDIA_INFO_t));
			}
		}
		MEDIA_INFO_t *mMediaDescriptor = g_curMedia;
		pIni = P_CMD_MEDIA_ENVManager(FALSE);
		if(pIni == NULL)
		{
			CMD_Printf("Env Init Fail!\n");
			return 0;
		}


		if( PARAM_IS("0") && PARAM1_IS("1") && PARAM2_IS("0") )
		{
			CMD_Printf("0. Manual (default) : 0\n");
			bManual = TRUE;

			CMD_Printf("1. Realtime\n");
			nPlaymode = MEDIA_PLAYMODE_RT;
			pIni->nPlayMode = nPlaymode;

			VK_MEM_Memset(g_keyBuf,0x00,CMD_MEDIA_KEYBUFSIZE);

			if( nSel == i+nDefUrl)
			{

				int file;
				FILE *f;
				HINT8 *p;
				HINT8 pbListBuf[256] = {0,};
				HINT8 pbFileBuf[256] = {0,};
				HBOOL	isHdd = FALSE;

				f = fopen("/mnt/hd2/playback_list.txt", "r");
				if (f != NULL)
				{
					isHdd = TRUE;
				}
				else
				{
					f = fopen("/media/playback_list.txt", "r");
					if (f != NULL)
					{
						isHdd = FALSE;
					}
					else
					{
						VK_printf("cannot open playback_list.txt\n");
						return 1;
					}
				}

				{
					CMD_Printf("\n====================== * LIST * ======================\n\n");

					while(1)
					{
						if(fgets(pbListBuf,sizeof(pbListBuf),f) == NULL) break;
						if( (p = VK_strchr(pbListBuf,'\r')) )
						{
							*p = '\0';
						}

						if(pbListBuf[0] == '#')
						{
							CMD_Printf("[X] ");
							CMD_Printf("%s\n",pbListBuf);
						}
						else
						{
							CMD_Printf("[O] ");

							if (isHdd)
								VK_strcpy(pbFileBuf,"/mnt/hd2/");
							else
								VK_strcpy(pbFileBuf,"/media/");
							VK_strcat(pbFileBuf,pbListBuf);
							file = fopen(pbFileBuf, "rb");

							if (isHdd)
								VK_strcpy(g_keyBuf,"file://mnt/hd2/");
							else
								VK_strcpy(g_keyBuf,"file://media/");

							VK_strcat(g_keyBuf,pbListBuf);
							CMD_Printf("%s\n",pbListBuf);
						}

					}

					CMD_Printf("\n======================================================\n");

					fclose(f);


				}

				CMD_Printf("\nURL: ");
				CMD_Printf("%s\n",g_keyBuf);

#if 0
				file = fopen("/mnt/hd2/test4_bcm.ts", "rb");
				if (file != 0)
				{
					VK_strcpy(g_keyBuf,"file://mnt/hd2/test4_bcm.ts");
				}
				else
				{
					file = fopen("/media/test4_bcm.ts", "rb");
					if (file != 0)
					{
						VK_strcpy(g_keyBuf,"file://media/test4_bcm.ts");
					}
				}
				if (file != 0)
					fclose(file);
				CMD_Printf("%s\n", g_keyBuf);
#endif
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

			mMediaDescriptor->url = pIni->pUrl;
			mMediaDescriptor->isAutoReaply = TRUE;

		}
		else
		{
#if 0
			if(g_curMedia == NULL)
			{
				g_curMedia = (MEDIA_INFO_t*)VK_MEM_Alloc(sizeof(MEDIA_INFO_t));
			}
			MEDIA_INFO_t *mMediaDescriptor = g_curMedia;
			pIni = P_CMD_MEDIA_ENVManager(FALSE);
			if(pIni == NULL)
			{
				CMD_Printf("Env Init Fail!\n");
				return 0;
			}
#endif
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
				g_ucScriptPath = strdup(DEFAULT_MEDIA_INI_PATH);
				CMD_Printf("Script Play ");
				goto script_operation;
			}
			else
			{
				CMD_Printf("0. DNP ");
				if(pIni->bLoad == TRUE && pIni->nPlayMode == MEDIA_PLAYMODE_DNP)
				{
					CMD_Printf("(Default)");
					nDefSel = MEDIA_PLAYMODE_DNP;
				}
				CMD_Printf("\n1. Realtime ");
				if(pIni->bLoad == TRUE && pIni->nPlayMode == MEDIA_PLAYMODE_RT)
				{
					CMD_Printf("(Default)");
					nDefSel = MEDIA_PLAYMODE_RT;
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
						nPlaymode = MEDIA_PLAYMODE_DNP;
						break;
					case 1 :
						nPlaymode = MEDIA_PLAYMODE_RT;
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

		}

#ifdef MEDIA_MEMORY_DEBUG
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
		dResult = DI_MEDIA_CreateSession(mMediaDescriptor->url, &(mMediaDescriptor->hSession));
		if(dResult != DI_ERR_OK)
		{
			CMD_Printf("[#ERROR] DI_MEDIA_CreateSession failed!\n");
			return 0;
		}
		CMD_Printf("created\n");
		//probe
		P_MeasureTime(&probe);
		dResult = DI_MEDIA_ProbeSession(mMediaDescriptor->hSession);
		if(dResult != DI_ERR_OK)
		{
			CMD_Printf("[#ERROR] DI_MEDIA_ProbeSession failed!\n");
			return 0;
		}
        P_MeasureTime(&probe);

		CMD_Printf("probed\n");

		//startdecoder
		dResult = DI_MEDIA_GetProbeInfo(mMediaDescriptor->hSession, &(mMediaDescriptor->ProbeInfo));
		if(dResult != DI_ERR_OK)
		{
			CMD_Printf("[#ERROR] DI_MEDIA_GetProbeInfo failed!\n");
			return 0;
		}

		if(pIni->nPlayMode != MEDIA_PLAYMODE_RT)
		{
			//startbuf
#ifdef	MEDIA_BUFFERING_PATH_HDD
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

			mMediaDescriptor->BufferCallback.cbStart = NULL;
			mMediaDescriptor->BufferCallback.cbUnderflow = NULL;
			mMediaDescriptor->BufferCallback.cbComplete = NULL;
			mMediaDescriptor->BufferCallback.cbFailure = NULL;
			mMediaDescriptor->BufferCallback.cbAdaptiveChange = NULL;
            mMediaDescriptor->BufferCallback.cbCanPlay = dnpCanPlay;
            mMediaDescriptor->BufferCallback.cbPause   = dnpPause;
            mMediaDescriptor->BufferCallback.cbResume  = dnpResume;
			dResult = DI_MEDIA_RegisterBufferingEventCallback(mMediaDescriptor->hSession, &(mMediaDescriptor->BufferCallback));
			if(dResult != DI_ERR_OK)
			{
				CMD_Printf("[#ERROR] DI_MEDIA_RegisterBufferingEventCallback failed!\n");
				return 0;
			}
			CMD_Printf("BufferingEventCB registered\n");

			dResult = DI_MEDIA_StartBuffering(mMediaDescriptor->hSession, mMediaDescriptor->ucBufferPath);
			if(dResult != DI_ERR_OK)
			{
				CMD_Printf("[#ERROR] DI_MEDIA_StartBuffering failed!\n");
				return 0;
			}

			CMD_Printf("buffering start\n");
			mMediaDescriptor->isBuffering = true;
		}

		//opensession
		dResult = DI_MEDIA_OpenSession(mMediaDescriptor->hSession);
		if(dResult != DI_ERR_OK)
		{
			CMD_Printf("[#ERROR] DI_MEDIA_OpenSession failed!\n");
			return 0;
		}
		CMD_Printf("opened\n");
		mMediaDescriptor->isOpened = true;

		//streameventcb
		mMediaDescriptor->StreamEventCallback.cbDummy = NULL;
		dResult = DI_MEDIA_RegisterStreamEventCallback(mMediaDescriptor->hSession, &(mMediaDescriptor->StreamEventCallback));
		if(dResult != DI_ERR_OK)
		{
			CMD_Printf("[#ERROR] DI_MEDIA_RegisterStreamEventCallback failed!\n");
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
		dResult = DI_MEDIA_SetupSession(mMediaDescriptor->hSession, &(mMediaDescriptor->unMediaId));
		if(dResult != DI_ERR_OK)
		{
			CMD_Printf("[#ERROR] DI_MEDIA_SetupSession failed!\n");
			return 0;
		}
		CMD_Printf("Setup\n");

		//mediaeventcb
		dResult = DI_MEDIA_RegisterMediaEventCallback(mMediaDescriptor->hSession, mMediaDescriptor->unMediaId, local_Media_Callback);
		if(dResult != DI_ERR_OK)
		{
			CMD_Printf("[#ERROR] DI_MEDIA_RegisterMediaEventCallback failed!\n");
			return 0;
		}
		CMD_Printf("MediaEventCB registered\n");

		/* HTTP chunked live streaming */
		/*
		if ((pIni->nPlayMode == MEDIA_PLAYMODE_RT) && (mMediaDescriptor->ProbeInfo.bLiveUnderflowControl == TRUE))
		{
			dResult = DI_MEDIA_RegisterLiveBufferEventCallback(mMediaDescriptor->hSession, mMediaDescriptor->unMediaId, local_Media_Callback);
			if(dResult != DI_ERR_OK)
			{
				CMD_Printf("[#ERROR] DI_MEDIA_RegisterLiveBufferEventCallback failed!\n");
				return 0;
			}
			CMD_Printf("RegisterLiveBufferEventCallback registered\n");
		}
		*/

		dResult = DI_AUDIO_SetMediaType(DI_AUDIO_DECODER_MAIN, DI_AUDIOTYPE_MEDIA);
		if(dResult != DI_ERR_OK)
		{
			CMD_Printf("[#ERROR] DI_AUDIO_SetMediaType failed!\n");
			return 0;
		}



		if(mMediaDescriptor->ProbeInfo.ucTotalAudioTrack != 0)
		{
            nMediaTracks++;
 		    dResult = DI_AUDIO_Start(DI_AUDIO_DECODER_MAIN, mMediaDescriptor->ProbeInfo.audio_codec);
			if(dResult != DI_ERR_OK)
			{
				CMD_Printf("[#ERROR] DI_AUDIO_Start failed!\n");
				return 0;
			}
		}
		else
		{
			CMD_Printf("No audio track\n");
		}

		if(mMediaDescriptor->ProbeInfo.ucTotalVideoTrack != 0)
		{
            nMediaTracks++;
    		dResult = DI_VIDEO_Start(DI_VIDEO_DECODER_MAIN, mMediaDescriptor->ProbeInfo.video_codec);
			if(dResult != DI_ERR_OK)
			{
				CMD_Printf("[#ERROR] DI_VIDEO_Start failed!\n");
				return 0;
			}
		}
		else
		{
			CMD_Printf("No Video track\n");
		}
		CMD_Printf("Start decorders\n");

		//startsession
		dResult = DI_MEDIA_StartSession(mMediaDescriptor->hSession, mMediaDescriptor->unMediaId, FALSE);
		if(dResult != DI_ERR_OK)
		{
			CMD_Printf("[#ERROR] DI_MEDIA_StartSession failed!\n");
			return 0;
		}

		/* HTTP chunked live streaming */
		/*
		if (1)
		{
			if ((pIni->nPlayMode == MEDIA_PLAYMODE_RT) && (mMediaDescriptor->ProbeInfo.bLiveUnderflowControl == TRUE))
			{
				dResult = DI_MEDIA_PauseSession(mMediaDescriptor->hSession, mMediaDescriptor->unMediaId);
			}
		}
		*/

		//set current playing media
		g_nCurSessionCount++;
		ullTime = VK_TIMER_GetSystemTick();
		VK_MEM_Memset(&cmdset,0x00,sizeof(MEDIA_CMDSET_t));
		cmdset.cmd.eCmd = CMD_MEDIA_COM_CREATE;
		cmdset.cmd.id = 0;
		cmdset.time = ullTime;
		P_CMD_MEDIA_RegisterCommand(pIni, cmdset);
		if(pIni->nPlayMode != MEDIA_PLAYMODE_RT)
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
		MEDIA_INFO_t *mMediaDescriptor = g_curMedia;

//		if(g_nNRSession == 1)
		{
			if(!IS_MEDIA_PLAYING)
			{
				CMD_Printf("<**CmdTask**> Currently, Media is not being played.\n");
				return 0;
			}
			dResult = DI_MEDIA_StopSession(g_curMedia->hSession, g_curMedia->unMediaId);
			if(dResult != DI_ERR_OK)
			{
				CMD_Printf("[#ERROR] DI_MEDIA_StopSession failed!\n");
				return 0;
			}
			CMD_Printf("Stop Decoders....\n");

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

			/* HTTP chunked live streaming */
			/* pIni = P_CMD_MEDIA_ENVManager(FALSE); */
			/*
			if ((g_pIni->nPlayMode == MEDIA_PLAYMODE_RT) && (mMediaDescriptor->ProbeInfo.bLiveUnderflowControl == TRUE))
			{
				dResult = DI_MEDIA_UnregisterLiveBufferEventCallback(g_curMedia->hSession, g_curMedia->unMediaId);
				if(dResult != DI_ERR_OK)
				{
					CMD_Printf("[#ERROR] DI_MEDIA_UnegisterLiveBufferEventCallback failed!\n");
				}
			}
			*/

			dResult = DI_MEDIA_UnregisterMediaEventCallback(g_curMedia->hSession, g_curMedia->unMediaId);
			if(dResult != DI_ERR_OK)
			{
				CMD_Printf("[#ERROR] DI_MEDIA_UnegisterMediaEventCallback failed!\n");
				return 0;
			}

			dResult = DI_MEDIA_ReleaseSession(g_curMedia->hSession, g_curMedia->unMediaId);
			if(dResult != DI_ERR_OK)
			{
				CMD_Printf("[#ERROR] DI_MEDIA_ReleaseSession failed!\n");
				return 0;
			}
			if(g_pIni->nPlayMode != MEDIA_PLAYMODE_RT)
			{
				dResult = DI_MEDIA_StopBuffering(g_curMedia->hSession);
				if(dResult != DI_ERR_OK)
				{
					CMD_Printf("[#ERROR] DI_MEDIA_StopBuffering failed!\n");
					return 0;
				}
			}
			dResult = DI_MEDIA_CloseSession(g_curMedia->hSession);
			if(dResult != DI_ERR_OK)
			{
				CMD_Printf("[#ERROR] DI_MEDIA_CloseSession failed!\n");
				return 0;
			}
			dResult = DI_MEDIA_DestroySession(g_curMedia->hSession);
			if(dResult != DI_ERR_OK)
			{
				CMD_Printf("[#ERROR] DI_MEDIA_DestroySession failed!\n");
				return 0;
			}
			g_nCurSessionCount--;
			ullTime = VK_TIMER_GetSystemTick();
			VK_MEM_Memset(&cmdset,0x00,sizeof(MEDIA_CMDSET_t));
			cmdset.cmd.eCmd = CMD_MEDIA_COM_DELAY;
			cmdset.cmd.id = 0;
			cmdset.cmd.param2 = ullTime - g_pIni->CmdSet[g_pIni->nCmdCursor-1].time;
			cmdset.time = ullTime;
			P_CMD_MEDIA_RegisterCommand(g_pIni, cmdset);
			cmdset.cmd.eCmd = CMD_MEDIA_COM_STOP;
			P_CMD_MEDIA_RegisterCommand(g_pIni, cmdset);
			if(g_pIni->nPlayMode != MEDIA_PLAYMODE_RT)
			{
				cmdset.cmd.eCmd = CMD_MEDIA_COM_STOPBUF;
				P_CMD_MEDIA_RegisterCommand(g_pIni, cmdset);
			}
			cmdset.cmd.eCmd = CMD_MEDIA_COM_DESTROY;
			P_CMD_MEDIA_RegisterCommand(g_pIni, cmdset);
			if(g_curMedia != NULL)
			{
				VK_MEM_Free(g_curMedia);
				g_curMedia = NULL;
			}
			CMD_Printf("\n0. SAVE (default)");
			CMD_Printf("\n1. DO NOT SAVE ");
			CMD_Printf("\n>> ");

			if(!PARAM_IS("0"))
			{
				VK_MEM_Memset(g_keyBuf,0x00,CMD_MEDIA_KEYBUFSIZE);
				fgets(g_keyBuf,CMD_MEDIA_KEYBUFSIZE,stdin);
				nSel = VK_atoi(g_keyBuf);
			}
			else
			{
				nSel = 0;
			}
			if(nSel == 0)
			{
				(void)P_CMD_MEDIA_ENVManager(TRUE);
			}
#ifdef MEDIA_MEMORY_DEBUG
			EIKI_MEM_PrintAll();
			EIKI_MEM_Uninit();
#endif

		}
		return 0;
	}
	else if CMD_IS("ff")
	{
		if(HWTEST_PARAM == NULL) {
			CMD_Printf("you should provide speed value\n");
			return -1;
		}
		else if(IS_MEDIA_PLAYING == false) {
			CMD_Printf("currently, media is not being played\n");
			return -1;
		}
		dResult = P_MediaTrick(MEDIA_TRICK_FF,VK_atoi(HWTEST_PARAM),g_curMedia->hSession, g_curMedia->unMediaId);
		if(dResult != DI_ERR_OK)
		{
			LOGMSG("[#ERROR] DI_MEDIA_SetSpeed failed!\n");
			return dResult;
		}
		ullTime = VK_TIMER_GetSystemTick();
		VK_MEM_Memset(&cmdset,0x00,sizeof(MEDIA_CMDSET_t));
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
		else if(IS_MEDIA_PLAYING == false) {
			CMD_Printf("currently, media is not being played\n");
			return -1;
		}
		dResult = P_MediaTrick(MEDIA_TRICK_RW,VK_atoi(HWTEST_PARAM),g_curMedia->hSession, g_curMedia->unMediaId);
		if(dResult != DI_ERR_OK)
		{
			LOGMSG("[#ERROR] DI_MEDIA_SetSpeed failed!\n");
			return dResult;
		}
		ullTime = VK_TIMER_GetSystemTick();
		VK_MEM_Memset(&cmdset,0x00,sizeof(MEDIA_CMDSET_t));
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
		if(IS_MEDIA_PLAYING == false) {
			CMD_Printf("currently, media is not being played\n");
			return -1;
		}
		dResult = P_MediaTrick(MEDIA_TRICK_FF,5,g_curMedia->hSession, g_curMedia->unMediaId);
		if(dResult != DI_ERR_OK)
		{
			LOGMSG("[#ERROR] DI_MEDIA_SetSpeed failed!\n");
			return dResult;
		}
		ullTime = VK_TIMER_GetSystemTick();
		VK_MEM_Memset(&cmdset,0x00,sizeof(MEDIA_CMDSET_t));
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
		if(IS_MEDIA_PLAYING == false) {
			CMD_Printf("currently, media is not being played\n");
			return -1;
		}
		dResult = P_MediaTrick(MEDIA_TRICK_FF,0,g_curMedia->hSession, g_curMedia->unMediaId);
		if(dResult != DI_ERR_OK)
		{
			LOGMSG("[#ERROR] DI_MEDIA_SetSpeed failed!\n");
			return dResult;
		}
		ullTime = VK_TIMER_GetSystemTick();
		VK_MEM_Memset(&cmdset,0x00,sizeof(MEDIA_CMDSET_t));
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
	else if CMD_IS("seek")
	{
		if(HWTEST_PARAM == NULL) {
			CMD_Printf("you should provide position value (sec)");
			return -1;
		}
		else if(IS_MEDIA_PLAYING == false ) {
			CMD_Printf("currently, media is not being played\n");
			return -1;
		}
		dResult = DI_MEDIA_SetPosition(g_curMedia->hSession, g_curMedia->unMediaId, _atohui64(HWTEST_PARAM));
		if(dResult != DI_ERR_OK)
		{
			LOGMSG("[#ERROR] DI_IPTUNER_Control failed!\n");
			return dResult;
		}
		ullTime = VK_TIMER_GetSystemTick();
		VK_MEM_Memset(&cmdset,0x00,sizeof(MEDIA_CMDSET_t));
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
		if(HWTEST_PARAM == NULL) {
			CMD_Printf("you should provide position value (sec)");
			return  -1;
		}
		else if(IS_MEDIA_PLAYING == false ) {
			CMD_Printf("currently, media is not being played\n");
			return -1;
		}
		nAudio = VK_atoi(HWTEST_PARAM);
		dResult = DI_MEDIA_SetAudioTrack(g_curMedia->hSession, g_curMedia->unMediaId,nAudio);
		if(dResult != DI_ERR_OK)
		{
			CMD_Printf("[#ERROR] DI_MEDIA_SetAudioTrack failed!\n");
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
		dResult = DI_AUDIO_Start(DI_AUDIO_DECODER_MAIN, g_curMedia->ProbeInfo.stAudioTrack[nAudio].audio_codec);
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
		VK_MEM_Memset(&cmdset,0x00,sizeof(MEDIA_CMDSET_t));
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
		if(IS_MEDIA_PLAYING == TRUE) {
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
			g_ucScriptPath = strdup("/var/lib/humaxtv/test.txt");
		}

script_operation:

		LOGMSG("script file path : %s\n",g_ucScriptPath);

		dResult = VK_TASK_Create(P_ScriptCmdTask, 40, 0x8000, "MEDIA_SCRIPT_TASK", NULL, &g_nScriptTaskId, 0);
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
		HUINT32 uiChunkNum = 0;
		HINT32 *iStartOffsets = 0;
		HINT32 *iEndOffsets = 0;
		HINT32 uiIndex = 0;
		DI_MEDIA_SessionSettings_t sSettings;

		if(g_curMedia == NULL)
		{
			CMD_Printf("Current media info is NULL \n");
			return 0;
		}
		DI_MEDIA_GetBufferInfo(g_curMedia->hSession, &uiChunkNum, &iStartOffsets, &iEndOffsets);
		CMD_Printf("Chunks: %d\n", uiChunkNum);
		for(uiIndex=0; uiIndex<uiChunkNum; uiIndex++)
		{
					CMD_Printf("[%d]th chunk: %d msec ~ %d msec\n", uiIndex, iStartOffsets[uiIndex], iEndOffsets[uiIndex]);
		}
		CMD_Printf("=============================================\n");

		DI_MEDIA_GetSessionSettings(g_curMedia->hSession, &sSettings);
		if(sSettings.eUnderflowMode == DI_MEDIA_UNDERFLOW_AUTO)
		{
					CMD_Printf("Underflow Mode: AUTO\n");
		}
		else if(sSettings.eUnderflowMode == DI_MEDIA_UNDERFLOW_THRESHOLD)
		{
					CMD_Printf("Underflow Mode: THRESHOLD\n");
		}

		if(sSettings.eBufferingMode == DI_MEDIA_MODE_FIXED)
		{
					CMD_Printf("Buffering Mode: FIXED\n");
		}
		else if(sSettings.eBufferingMode == DI_MEDIA_MODE_UNLIMITED)
		{
					CMD_Printf("Buffering Mode: UNLIMITED\n");
		}
		else if(sSettings.eBufferingMode == DI_MEDIA_MODE_THRESHOLD)
		{
					CMD_Printf("Buffering Mode: TRHESHOLD\n");
					CMD_Printf("Buffering Threshold: %d msec\n", sSettings.tBufferingParams.nBufferThreshold);
					CMD_Printf("Buffering Start Threshold: %d msec\n", sSettings.tPlaybackStartParams.ulStartThreshold);
					CMD_Printf("Buffering Start Constants: %d msec\n", sSettings.tPlaybackStartParams.ulStartConstant);
		}

	}
	else
	{
		CMD_Printf("Invalid Command!\n");
	}
	return 0;
}

static DI_ERR_CODE P_MediaTrick(HUINT8 TRICKCMD, HUINT64 uiSpeed, SESSION_HANDLE hSession, HUINT32 unMediaId)
{
	DI_MEDIA_PlaySpeed_e eSpeed;
	DI_ERR_CODE dResult;

	if(TRICKCMD == MEDIA_TRICK_FF)
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

		dResult = DI_MEDIA_SetSpeed(hSession, unMediaId, eSpeed);
		if(dResult != DI_ERR_OK)
		{
			LOGMSG("[#ERROR] DI_MEDIA_SetSpeed failed!\n");
			return dResult;
		}
	}

	else if(TRICKCMD == MEDIA_TRICK_RW)
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
		dResult = DI_MEDIA_SetSpeed(hSession, unMediaId, eSpeed);
		if(dResult != DI_ERR_OK)
		{
			LOGMSG("[#ERROR] DI_MEDIA_SetSpeed failed!\n");
			return dResult;
		}

	}

	else if(TRICKCMD == MEDIA_TRICK_SEEK)
	{
			LOGMSG("Seek, Time: %llu msec\n", uiSpeed);

			dResult = DI_MEDIA_SetPosition(hSession, unMediaId, uiSpeed);

			if(dResult != DI_ERR_OK)
			{
				LOGMSG("[#ERROR] DI_IPTUNER_Control failed!\n");
				return dResult;
			}
			return dResult;
	}

	else if(TRICKCMD == MEDIA_TRICK_PAUSE)
	{
			LOGMSG("pause\n");

			dResult = DI_MEDIA_SetSpeed(hSession, unMediaId, eDI_MEDIA_Speed_Pause);

			if(dResult != DI_ERR_OK)
			{
				LOGMSG("[#ERROR] DI_IPTUNER_Control failed!\n");
				return dResult;
			}
			return dResult;
	}
}

void local_Media_Callback(HUINT32 ulDeviceId, HUINT32 ulEvent, HUINT32 Value)
{
//	CMD_TASK_MSG_t stSendMsg;
	switch( ulEvent )
	{
		case DI_MEDIA_EVENT_CHANGEPLAYTIME:
			LOGMSG("<#EVENT> MEDIA_EVENT_CHANGEPLAYTIME(%u)\n",Value);
			g_bMediaPlaying = true;
			if(g_bMediaEventWaiting == true)
			{
				g_bMediaEventWaiting = false;
			}
			break;

		case DI_MEDIA_EVENT_BEGINOFSTREAM:
			LOGMSG("<#EVENT> MEDIA_EVENT_BEGINOFSTREAM\n");
			g_bMediaPlaying = true;
			break;

		case DI_MEDIA_EVENT_ERROROFSTREAM:
			LOGMSG("<#EVENT> MEDIA_EVENT_ERROROFSTREAM\n");
			break;

		case DI_MEDIA_EVENT_ENDOFSTREAM:

			if (g_curMedia->isAutoReaply)
			{
				LOGMSG("<#EVENT> MEDIA_EVENT_ENDOFSTREAM\n");
				DI_MEDIA_SetPosition(g_curMedia->hSession, g_curMedia->unMediaId, 0);
				DI_MEDIA_StartSession(g_curMedia->hSession, g_curMedia->unMediaId, 0);
				break;
			}
			if(g_nScriptTaskId)
			{
				g_bFlagEOS = true;
				g_nEOSmediaId = ulDeviceId;
			}
			else
			{
#if 0
				stSendMsg.MSGTYPE = MEDIA_COMMAND_CALLBACK;
				stSendMsg.PARAM1  = ulEvent;
				VK_MSG_Send(g_nQIDMainCmd, &stSendMsg, sizeof(CMD_TASK_MSG_t));
#endif
			}

			if(g_bMediaEventWaiting == true)
			{
				g_bMediaEventWaiting = false;
			}
			g_bMediaPlaying = false;

			LOGMSG("<#EVENT> MEDIA_EVENT_ENDOFSTREAM\n");
			break;

		case DI_MEDIA_EVENT_START:
#if 0
			stSendMsg.MSGTYPE = MEDIA_COMMAND_CALLBACK;
			stSendMsg.PARAM1  = ulEvent;
			VK_MSG_Send(g_nQIDMainCmd, &stSendMsg, sizeof(CMD_TASK_MSG_t));
#endif
			LOGMSG("<#EVENT> MEDIA_EVENT_START\n");
			break;

		case DI_MEDIA_EVENT_PAUSE:
			LOGMSG("<#EVENT> MEDIA_EVENT_PAUSE\n");
			break;

		case DI_MEDIA_EVENT_PLAYING:
			LOGMSG("<#EVENT> MEDIA_EVENT_PLAYING\n");
			break;

		case DI_MEDIA_EVENT_TRICK:
			LOGMSG("<#EVENT> MEDIA_EVENT_TRICK\n");
			break;

		case DI_MEDIA_EVENT_STOP:
			LOGMSG("<#EVENT> MEDIA_EVENT_STOP \n");
			break;

		case DI_MEDIA_EVENT_LIVE_BUFFERING_READY:
			LOGMSG("<#EVENT> MEDIA_EVENT_LIVE_BUFFERING_READY (Value:%d)\n", Value);
			if (Value == 31) // DI_IPTUNER_CLS_BUFSTATE_BUFFERING
			{
				LOGMSG("<#EVENT> DI_IPTUNER_CLS_BUFSTATE_BUFFERING (Value:%d) --> pause\n", Value); /* underflow --> pause */
				DI_MEDIA_PauseSession(g_curMedia->hSession, g_curMedia->unMediaId);
			}
			else if (Value == 33) // DI_IPTUNER_CLS_BUFSTATE_COMPLETED
			{
				LOGMSG("<#EVENT> DI_IPTUNER_CLS_BUFSTATE_COMPLETED (Value:%d) --> resume\n", Value);
				DI_MEDIA_ResumeSession(g_curMedia->hSession, g_curMedia->unMediaId);
			}
			break;

		case DI_MEDIA_EVENT_LIVE_BUFFERING_START:
			LOGMSG("<#EVENT> MEDIA_EVENT_LIVE_BUFFERING_START \n");
			break;

		default:
			LOGMSG("Unknown event(0x%X)n", ulEvent);
			return;
	}
}

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



