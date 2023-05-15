/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Application
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/

/*******************************************************************************
* header and definitions
*******************************************************************************/
/* global header files */

/* chip manufacture's  header files */

/* humax header files */
#include "fvk.h"
#include "fai_common.h"
#include "fai_testmenu.h"
#include "fai_channel.h"
#include "fai_tmd.h"
#include "fai_av.h"
#include "fdi_err.h"
#include "fdi_video.h"
#include "fdi_audio.h"
#include "fdi_demux.h"
#include "fdi_channel.h"
#include "fdi_scart.h"
#include "fdi_panel.h"

/* model specific configuration header */
#include "factorysw_config.h"

/*******************************************************************************
* Debug level
*******************************************************************************/

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/
#define FAI_AV_VIDEO_DEVICE_ID		0
#define FAI_AV_AUDIO_DEVICE_ID		0
#define FAI_AV_DEMUX_DEVICE_ID		0
#define FAI_AV_NUM_OF_LOCK_RETRY	30
#define FAI_AV_LOCK_RETRY_DELAY		300 //150
#define FAI_AV_PLAY_DELAY			1000

#define AVPLAY_PLAY_NEW 1 /* 임시 cfg로 옮겨야 됨 */

#if defined(AVPLAY_PLAY_NEW)
#define FAI_AV_MAX_TMD_TEST_WAIT	7*1000 
#else
#define FAI_AV_MAX_TMD_TEST_WAIT	3*1000 //factory2.0에서는 3초로 정함. 
#endif

#if defined(AVPLAY_PLAY_NEW)
typedef enum
{
	FAI_AV_TEST_SD = 0,
	FAI_AV_TEST_HD,
	FAI_AV_TEST_START,
	FAI_AV_TEST_TMD_QUICK,
	FAI_AV_TEST_TMD_READY,
} FAI_AV_TEST_TYPE;
#else
typedef enum
{
	FAI_AV_TEST_SD = 0,
	FAI_AV_TEST_HD,
	FAI_AV_TEST_TMD_NORMAL,
	FAI_AV_TEST_TMD_QUICK,
} FAI_AV_TEST_TYPE;
#endif

typedef enum
{
	FAI_AV_QUICK_PLAY_SAT_COLORBAR_SD = 0,
	FAI_AV_QUICK_PLAY_SAT_MOVING_SD,
	FAI_AV_QUICK_PLAY_SAT_HD,
	FAI_AV_QUICK_PLAY_SAT_TV4_CI,
	FAI_AV_QUICK_PLAY_SAT_TRAVEL_CI,
	FAI_AV_QUICK_PLAY_CAB_COLORBAR_SD,
	FAI_AV_QUICK_PLAY_CAB_MOVING_SD,
	FAI_AV_QUICK_PLAY_CAB_HD,
	FAI_AV_QUICK_PLAY_CAB_TV4_CI,
	FAI_AV_QUICK_PLAY_CAB_TRAVEL_CI,
	FAI_AV_QUICK_PLAY_TER_COLORBAR_SD,
	FAI_AV_QUICK_PLAY_TER_MOVING_SD,
	FAI_AV_QUICK_PLAY_TER_HD,

#if defined(FACTORY_USE_CHANNEL_TER_T2) && (FACTORY_USE_CHANNEL_TER_T2==YES)
	FAI_AV_QUICK_PLAY_TER_HD_DVBT2,
#endif
	FAI_AV_QUICK_PLAY_TER_TV4_CI,
	FAI_AV_QUICK_PLAY_TER_TRAVEL_CI,
	FAI_AV_QUICK_PLAY_DSS_COLORBAR,
	FAI_AV_QUICK_PLAY_DSS_MOVING,
	FAI_AV_QUICK_PLAY_DIRECTV_DVBS2,
	FAI_AV_QUICK_PLAY_NTSC_COLORBAR,
	FAI_AV_QUICK_PLAY_NTSC_MOVING,
	FAI_AV_QUICK_PLAY_SAT_MOVING_SD_CI,
	FAI_AV_QUICK_PLAY_CAB_MOVING_SD_CI,
	FAI_AV_QUICK_PLAY_TER_MOVING_SD_CI,	
} FAI_AV_QUICK_PLAY_STREAM_INFO_e;

#if defined(AVPLAY_PLAY_NEW)
typedef struct FAI_VIDEO_MSG
{	
	BOOL bIsDecoding;
} FAI_VIDEO_MSG_t;

typedef struct FAI_AUDIO_MSG
{	
	BOOL bIsDecoding;
} FAI_AUDIO_MSG_t;
#else
typedef enum
{
	FAI_AV_VIDEO_DECODER = 0, 
	FAI_AV_AUDIO_DECODER,
} FAI_AV_DECODER_TYPE;

typedef struct FAI_AV_MSG
{
	FAI_AV_DECODER_TYPE eDecoderType;
	BOOL bIsDecoding;
} FAI_AV_MSG_t;
#endif

/*******************************************************************************
* External variables and functions
*******************************************************************************/
/* external variables  and functions */

/*******************************************************************************
* Global variables and structures
*******************************************************************************/
/* global variables and structures */
#if defined(AVPLAY_PLAY_NEW)
unsigned long g_ulVideoMsgId, g_ulAudioMsgId, g_ulAvTaskId;
#else
unsigned long g_ulAvMsgId, g_ulAvTaskId;
#endif
unsigned long ulPcrChannelId, ulVideoChannelId, ulAudioChannelId;
static BOOL g_bIsAvPlaying;
static BOOL g_bIsBeepPlaying;
FAI_AV_TEST_TYPE g_eAvTestType;
unsigned long g_ulTimerId;

static FAI_ERR_CODE INTRN_FAI_AV_BeepOnOff(BOOL bOnOff);
static FAI_ERR_CODE INTRN_FAI_AV_Play(unsigned long ulTunerID, FDI_VIDEO_FORMAT eVideoFormat, FDI_AUDIO_FORMAT eAudioFormat,
									  unsigned short usVideoPid, unsigned short usAudioPid, unsigned short usPcrPid, FDI_DEMUX_CI_PATH_e eCiPath);
static FAI_ERR_CODE INTRN_FAI_AV_Stop();
static void INTRN_FAI_AV_Task(void *pData);
static FDI_ERR_CODE INTRN_FAI_AV_VideoEventCallback(FDI_VIDEO_NOTIFY_EVT eEvent, FDI_VIDEO_RUNNING_STATE eState);
static FDI_ERR_CODE INTRN_FAI_AV_AudioEventCallback(FDI_AUDIO_NOTIFY_EVT eEvent, FDI_AUDIO_RUNNING_STATE eState);

#if defined(FACTORY_USE_CHANNEL_SAT)
static FAI_ERR_CODE INTRN_FAI_AV_S_QuickPlay(unsigned char ucTunerId, unsigned char ucStreamId);
#endif
#if defined(FACTORY_USE_CHANNEL_CAB)
static FAI_ERR_CODE INTRN_FAI_AV_C_QuickPlay(unsigned char ucTunerId, unsigned char ucStreamId);
#endif
#if defined(FACTORY_USE_CHANNEL_TER)
static FAI_ERR_CODE INTRN_FAI_AV_T_QuickPlay(unsigned char ucTunerId, unsigned char ucStreamId);
#endif

static FAI_ERR_CODE INTRN_FAI_AV_DefaultVideoSetting(void);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if defined(AVPLAY_PLAY_NEW)
static FVK_ERR_CODE INTRN_FAI_AV_TimerCallback(unsigned long ulTimerId, void *pvParams)
{
	FAI_TMD_ERR eTmdErr = FAI_TMD_ERR_OK;
	FAI_TMD_RESULT_T tTMDResult;
	unsigned char ucTmdErrCode[2];

	FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_AV_TimerCallback] : AV PLAY Timeout ERROR!!! \n"));
	
	FVK_MEM_Set( (void *)&tTMDResult, 0x00, sizeof(tTMDResult));

	tTMDResult.eResult= FAI_TMD_ERR_FAIL;

	if(g_eAvTestType == FAI_AV_TEST_START)
	{
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_AVPLAY;
		ucTmdErrCode[1] = FAI_AV_TMD_ERR_PLAY_FAIL;
	}
	else if(g_eAvTestType == FAI_AV_TEST_TMD_QUICK)
	{
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_AVPLAY;
		ucTmdErrCode[1] = FAI_AV_TMD_ERR_QUICK_PLAY_FAIL;
	}
	FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
	FAI_TMD_SetResult(&tTMDResult);	
}
#else
static FVK_ERR_CODE INTRN_FAI_AV_TimerCallback(unsigned long ulTimerId, void *pvParams)
{
	FAI_TMD_ERR eTmdErr = FAI_TMD_ERR_OK;
	FAI_TMD_RESULT_T tTMDResult;
	unsigned char ucTmdErrCode[2];

	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));

	tTMDResult.eResult= FAI_TMD_ERR_FAIL;

	ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_AVPLAY;
	ucTmdErrCode[1] = FAI_AV_TMD_ERR_PLAY_FAIL;
	FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	

	FAI_TMD_SetResult(&tTMDResult);

/*
	필요할 경우 g_eAvTestType 에 따라 구분
*/	
}
#endif

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_AV_Init(void)
{
	g_bIsAvPlaying = FALSE;
	g_bIsBeepPlaying = FALSE;

#if defined(AVPLAY_PLAY_NEW)
	if( FVK_MSG_Create(FVK_TASK_DEFAULT_QUEUE_SIZE, sizeof(FAI_VIDEO_MSG_t), (char*)"VideoMsg", &g_ulVideoMsgId) != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_Init] : FVK_Video_MSG_Create \n"));
		return FVK_ERR;
	}
	if( FVK_MSG_Create(FVK_TASK_DEFAULT_QUEUE_SIZE, sizeof(FAI_AUDIO_MSG_t), (char*)"AudioMsg", &g_ulAudioMsgId) != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_Init] : FVK_Audio_MSG_Create \n"));
		return FVK_ERR;
	}	
#else
	if( FVK_MSG_Create(FVK_TASK_DEFAULT_QUEUE_SIZE, sizeof(FAI_AV_MSG_t), (char*)"AvMsg", &g_ulAvMsgId) != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_Init] : FVK_MSG_Create \n"));
		return FVK_ERR;
	}
#endif
	if( FVK_TASK_Create(INTRN_FAI_AV_Task, FVK_TASK_DEFAULT_PRIORITY, FVK_TASK_DEFAULT_STACK_SIZE, "Av Task", NULL, &g_ulAvTaskId) != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_Init] : FVK_TASK_Create Error!!\n"));
		return FVK_ERR;
	}
	if( FVK_TASK_Start(g_ulAvTaskId) != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_Init] : FVK_TASK_Start Error!!\n"));
		return FVK_ERR;
	}

	FDI_VIDEO_RegisterEventCallback(FAI_AV_VIDEO_DEVICE_ID, INTRN_FAI_AV_VideoEventCallback);
	FDI_AUDIO_RegisterEventCallback(FAI_AV_AUDIO_DEVICE_ID, INTRN_FAI_AV_AudioEventCallback);

	INTRN_FAI_AV_DefaultVideoSetting();

#if defined(AVPLAY_PLAY_NEW)	
	g_eAvTestType = FAI_AV_TEST_TMD_READY;
#endif
	return FAI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (AVPLAY_PLAY_ENABLE==YES)
#if defined(AVPLAY_PLAY_NEW)
FAI_ERR_CODE FAI_AV_Play(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
    unsigned char ucTunerId, ucVideoFormat, ucAudioFormat, aucVideoPid[2], aucAudioPid[2], aucPcrPid[2];
	FDI_AUDIO_FORMAT eAudioFormat;
	FDI_VIDEO_FORMAT eVideoFormat;
	unsigned short usVideoPid, usAudioPid, usPcrPid;
	FDI_ERR_CODE eFdiErr;
	FAI_TMD_RESULT_T tTMDResult;
	unsigned char ucTmdErrCode[2];
    
	FVK_MEM_Set( (void *)&tTMDResult, 0x00, sizeof(tTMDResult));
	if( ucArgc != 6 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_Play] : Argument Count Error=%d\n", ucArgc));
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
        FAI_TMD_SetResult(&tTMDResult);
		return FAI_ERR;
	}

	/* get params */
	FAI_TMD_GetParam(&ucTunerId, pucArgList, 0);
	FAI_TMD_GetParam(&ucVideoFormat, pucArgList, 1);
	FAI_TMD_GetParam(&ucAudioFormat, pucArgList, 2);
	FAI_TMD_GetParam(aucVideoPid, pucArgList, 3);
	FAI_TMD_GetParam(aucAudioPid, pucArgList, 4);
	FAI_TMD_GetParam(aucPcrPid, pucArgList, 5);

	FAI_PRINT(0, ("[FAI_AV_Play] : ucTunerId=%d\n", ucTunerId));
	FAI_PRINT(0, ("[FAI_AV_Play] : ucVideoFormat=%d\n", ucVideoFormat));
	FAI_PRINT(0, ("[FAI_AV_Play] : ucAudioFormat=%d\n", ucAudioFormat));
	FAI_PRINT(0, ("[FAI_AV_Play] : aucVideoPid=%d.%d\n", aucVideoPid[0], aucVideoPid[1]));
	FAI_PRINT(0, ("[FAI_AV_Play] : aucAudioPid=%d.%d\n", aucAudioPid[0], aucAudioPid[1]));
	FAI_PRINT(0, ("[FAI_AV_Play] : aucPcrPid=%d.%d\n", aucPcrPid[0], aucPcrPid[1]));

	/* Convert video format */
	switch(ucVideoFormat)
	{
		case 0:
			eVideoFormat = FDI_VIDEO_FORMAT_MPEG2;
			break;
		case 1:
			eVideoFormat = FDI_VIDEO_FORMAT_MPEG4Part2;
			break;
		case 2:
			eVideoFormat = FDI_VIDEO_FORMAT_H264;
			break;
		default:
			eVideoFormat = FDI_VIDEO_FORMAT_MPEG2;
			break;
	}

	/* Convert audio format */
	switch(ucAudioFormat)
	{
		case 0:
			eAudioFormat = FDI_AUDIO_FORMAT_MPEG;
			break;
		case 1:
			eAudioFormat = FDI_AUDIO_FORMAT_PCM;
			break;
		case 2:
			eAudioFormat = FDI_AUDIO_FORMAT_AC3;
			break;
		case 3:
			eAudioFormat = FDI_AUDIO_FORMAT_AAC;
			break;
		case 4:
			eAudioFormat = FDI_AUDIO_FORMAT_MP3;
			break;
		default:
			eAudioFormat = FDI_AUDIO_FORMAT_MPEG;
			break;
	}

	/* get pids */
	usVideoPid = ((aucVideoPid[0]<<8) | aucVideoPid[1]);
	usAudioPid = ((aucAudioPid[0]<<8) | aucAudioPid[1]);
	usPcrPid = ((aucPcrPid[0]<<8) | aucPcrPid[1]);

	/* If av is playing, stop it */
	if( g_bIsAvPlaying == TRUE )
	{
		INTRN_FAI_AV_Stop();
	}

	if( INTRN_FAI_AV_Play(ucTunerId, eVideoFormat, eAudioFormat, usVideoPid, usAudioPid, usPcrPid, FDI_DEMUX_CI_BYPASS) != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_Play] : INTRN_FAI_AV_Play(FDI_DEMUX_CI_BYPASS) Error!!\n"));
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_AVPLAY;
		ucTmdErrCode[1] = FAI_AV_TMD_ERR_PLAY_FAIL;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
		return FAI_ERR;
	}
	else
	{
		g_eAvTestType = FAI_AV_TEST_START;	
		FVK_TIMER_CreateStart(FAI_AV_MAX_TMD_TEST_WAIT, INTRN_FAI_AV_TimerCallback, NULL, 0, FVK_TIMER_ONCE, &g_ulTimerId);
	}

	return FAI_NO_ERR;
}
#else
FAI_ERR_CODE FAI_AV_Play(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
    unsigned char ucTunerId, ucVideoFormat, ucAudioFormat, aucVideoPid[2], aucAudioPid[2], aucPcrPid[2];
	FDI_AUDIO_FORMAT eAudioFormat;
	FDI_VIDEO_FORMAT eVideoFormat;
	unsigned short usVideoPid, usAudioPid, usPcrPid;
	FDI_ERR_CODE eFdiErr;
	FAI_TMD_RESULT_T tTMDResult;
	FAI_TMD_ERR eTmdErr = FAI_TMD_ERR_OK;
	unsigned char ucTmdErrCode[2];

	FVK_MEM_Set( (void *)&tTMDResult, 0x00, sizeof(tTMDResult));
	if( ucArgc != 6 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_Play] : Argument Count Error=%d\n", ucArgc));
		eTmdErr = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
		goto TEST_FAIL;
	}

	/* get params */
	FAI_TMD_GetParam(&ucTunerId, pucArgList, 0);
	FAI_TMD_GetParam(&ucVideoFormat, pucArgList, 1);
	FAI_TMD_GetParam(&ucAudioFormat, pucArgList, 2);
	FAI_TMD_GetParam(aucVideoPid, pucArgList, 3);
	FAI_TMD_GetParam(aucAudioPid, pucArgList, 4);
	FAI_TMD_GetParam(aucPcrPid, pucArgList, 5);

	FAI_PRINT(0, ("[FAI_AV_Play] : ucTunerId=%d\n", ucTunerId));
	FAI_PRINT(0, ("[FAI_AV_Play] : ucVideoFormat=%d\n", ucVideoFormat));
	FAI_PRINT(0, ("[FAI_AV_Play] : ucAudioFormat=%d\n", ucAudioFormat));
	FAI_PRINT(0, ("[FAI_AV_Play] : aucVideoPid=%d.%d\n", aucVideoPid[0], aucVideoPid[1]));
	FAI_PRINT(0, ("[FAI_AV_Play] : aucAudioPid=%d.%d\n", aucAudioPid[0], aucAudioPid[1]));
	FAI_PRINT(0, ("[FAI_AV_Play] : aucPcrPid=%d.%d\n", aucPcrPid[0], aucPcrPid[1]));

	/* Convert video format */
	switch(ucVideoFormat)
	{
		case 0:
			eVideoFormat = FDI_VIDEO_FORMAT_MPEG2;
			break;
		case 1:
			eVideoFormat = FDI_VIDEO_FORMAT_MPEG4Part2;
			break;
		case 2:
			eVideoFormat = FDI_VIDEO_FORMAT_H264;
			break;
		case 3:
			eVideoFormat = FDI_VIDEO_FORMAT_H265;
			break;			
		default:
			eVideoFormat = FDI_VIDEO_FORMAT_MPEG2;
			break;
	}

	/* Convert audio format */
	switch(ucAudioFormat)
	{
		case 0:
			eAudioFormat = FDI_AUDIO_FORMAT_MPEG;
			break;
		case 1:
			eAudioFormat = FDI_AUDIO_FORMAT_PCM;
			break;
		case 2:
			eAudioFormat = FDI_AUDIO_FORMAT_AC3;
			break;
		case 3:
			eAudioFormat = FDI_AUDIO_FORMAT_AAC;
			break;
		case 4:
			eAudioFormat = FDI_AUDIO_FORMAT_MP3;
			break;
		default:
			eAudioFormat = FDI_AUDIO_FORMAT_MPEG;
			break;
	}

	/* get pids */
	usVideoPid = ((aucVideoPid[0]<<8) | aucVideoPid[1]);
	usAudioPid = ((aucAudioPid[0]<<8) | aucAudioPid[1]);
	usPcrPid = ((aucPcrPid[0]<<8) | aucPcrPid[1]);

	/* If av is playing, stop it */
	if( g_bIsAvPlaying == TRUE )
	{
		INTRN_FAI_AV_Stop();
	}
	if( INTRN_FAI_AV_Play(ucTunerId, eVideoFormat, eAudioFormat, usVideoPid, usAudioPid, usPcrPid, FDI_DEMUX_CI_BYPASS) != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_Play] : INTRN_FAI_AV_Play(FDI_DEMUX_CI_BYPASS) Error!!\n"));
		eTmdErr = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_AVPLAY;
		ucTmdErrCode[1] = FAI_AV_TMD_ERR_PLAY_FAIL;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
		goto TEST_FAIL;
	}

	g_eAvTestType = FAI_AV_TEST_TMD_NORMAL;	
#if(0)	/* mangyu_temp : send a twice tmd command for AV */
	FVK_TIMER_CreateStart(FAI_AV_MAX_TMD_TEST_WAIT, INTRN_FAI_AV_TimerCallback, NULL, 0, FVK_TIMER_ONCE, &g_ulTimerId);
	tTMDResult.eResult = FAI_TMD_ERR_OK;
	FAI_TMD_SetResult(&tTMDResult);
#endif
	return FAI_NO_ERR;

TEST_FAIL:
	tTMDResult.eResult = eTmdErr;
	FAI_TMD_SetResult(&tTMDResult);
	return FAI_ERR;
}
#endif
#endif

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (AVPLAY_STOP_ENABLE==YES)
#if defined(AVPLAY_PLAY_NEW)
FAI_ERR_CODE FAI_AV_Stop(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	FAI_TMD_RESULT_T tTMDResult;
	FAI_TMD_ERR eTmdErr = FAI_TMD_ERR_OK;
	FDI_ERR_CODE eFdiErr;
	unsigned char ucTmdErrCode[2];
	
	FVK_MEM_Set( (void *)&tTMDResult, 0x00, sizeof(tTMDResult));

	if( ucArgc != 0 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Argument Count Error=%d\n",  __func__, ucArgc));
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
		FAI_TMD_SetResult(&tTMDResult);
		return FAI_ERR;
	}

	eFdiErr = INTRN_FAI_AV_Stop();	
	if(eFdiErr != FAI_NO_ERR) 
	{
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_AVPLAY;
		ucTmdErrCode[1] = FAI_AV_TMD_ERR_STOP_FAIL;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));		
		FAI_TMD_SetResult(&tTMDResult);
	} 
	else 
	{
		FDI_OSD_Show();		
		tTMDResult.eResult = FAI_TMD_ERR_OK;
		FAI_TMD_SetResult(&tTMDResult);
	}
	return FAI_NO_ERR;
}
#else
FAI_ERR_CODE FAI_AV_Stop(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	FAI_TMD_RESULT_T tTMDResult;
	FAI_TMD_ERR eTmdErr = FAI_TMD_ERR_OK;
	FDI_ERR_CODE eFdiErr;
	unsigned char ucTmdErrCode[2];
	
	FVK_MEM_Set( (void *)&tTMDResult, 0x00, sizeof(tTMDResult));

	if( ucArgc != 0 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Argument Count Error=%d\n",  __func__, ucArgc));
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
		FAI_TMD_SetResult(&tTMDResult);
		return FAI_ERR;
	}

	INTRN_FAI_AV_Stop();
	//FDI_CHANNEL_C_CancelLock(1);

	FDI_OSD_Show();

	if(g_bIsAvPlaying == FALSE) 
	{
		tTMDResult.eResult = FAI_TMD_ERR_OK;
	} 
	else 
	{
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_AVPLAY;
		ucTmdErrCode[1] = FAI_AV_TMD_ERR_STOP_FAIL;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
	}
	FAI_TMD_SetResult(&tTMDResult);

	return FAI_NO_ERR;
}
#endif
#endif

#if(RESOLUTION_CHANGE_ENABLE==YES)
FAI_ERR_CODE FAI_AV_SetDisplayStandard(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	FAI_ERR_CODE eFaiErr = FAI_ERR;
	FAI_TMD_RESULT_T tTMDResult;
	FAI_TMD_ERR eTmdErr = FAI_TMD_ERR_OK;
	FDI_ERR_CODE eFdiErr;
	unsigned char ucTmdErrCode[2];

	unsigned char ucResolution;

	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));
	
	if( ucArgc != 1 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_QuickPlay] : Argument Count Error=%d\n", ucArgc));
		eTmdErr = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
		goto TEST_FAIL;
	}

	FAI_TMD_GetParam(&ucResolution, pucArgList, 0);

	eFaiErr = FDI_VIDEO_SetDisplayStandard(0, (FDI_VIDEO_RESOLUTION)ucResolution);

	if( eFaiErr != FAI_NO_ERR )
	{
		eTmdErr = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_FAIL;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	 
		 
		goto TEST_FAIL;
	}

	tTMDResult.eResult = FAI_TMD_ERR_OK;
	FAI_TMD_SetResult(&tTMDResult);
	return FAI_NO_ERR;

	TEST_FAIL:
    tTMDResult.eResult = eTmdErr;
    FAI_TMD_SetResult(&tTMDResult);
    return FAI_ERR;

}
#endif

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (AVPLAY_QUICK_PLAY_ENABLE==YES)
#if defined(AVPLAY_PLAY_NEW)
FAI_ERR_CODE FAI_AV_QuickPlay(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	FAI_ERR_CODE eFaiErr = FAI_ERR;
	unsigned char ucTunerId, ucStreamId;
	FAI_TMD_RESULT_T tTMDResult;
	FAI_TMD_ERR eTmdErr = FAI_TMD_ERR_OK;
	FDI_ERR_CODE eFdiErr;
	unsigned char ucTmdErrCode[2];

    FVK_MEM_Set( (void *)&tTMDResult, 0x00, sizeof(tTMDResult));

	if( ucArgc != 2 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_QuickPlay] : Argument Count Error=%d\n", ucArgc));
		eTmdErr = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
		goto TEST_FAIL;
	}

	FAI_TMD_GetParam(&ucTunerId, pucArgList, 0);
	FAI_TMD_GetParam(&ucStreamId, pucArgList, 1);

	/* If av is playing, stop it */
	if( g_bIsAvPlaying == TRUE )
	{
		INTRN_FAI_AV_Stop();
	}

	switch(ucStreamId)
	{
		case FAI_AV_QUICK_PLAY_SAT_COLORBAR_SD:
		case FAI_AV_QUICK_PLAY_SAT_MOVING_SD:
		case FAI_AV_QUICK_PLAY_SAT_HD:
		case FAI_AV_QUICK_PLAY_SAT_TV4_CI:
		case FAI_AV_QUICK_PLAY_SAT_TRAVEL_CI:
#if defined(FACTORY_USE_CHANNEL_SAT) && (FACTORY_USE_CHANNEL_SAT==YES)
			eFaiErr = INTRN_FAI_AV_S_QuickPlay(ucTunerId, ucStreamId);
#endif
			break;
		case FAI_AV_QUICK_PLAY_CAB_COLORBAR_SD:
		case FAI_AV_QUICK_PLAY_CAB_MOVING_SD:
		case FAI_AV_QUICK_PLAY_CAB_HD:
		case FAI_AV_QUICK_PLAY_CAB_TV4_CI:
		case FAI_AV_QUICK_PLAY_CAB_TRAVEL_CI:
#if defined(FACTORY_USE_CHANNEL_CAB) && (FACTORY_USE_CHANNEL_CAB==YES)
			eFaiErr = INTRN_FAI_AV_C_QuickPlay(ucTunerId, ucStreamId);
#endif
			break;
		case FAI_AV_QUICK_PLAY_TER_COLORBAR_SD:
		case FAI_AV_QUICK_PLAY_TER_MOVING_SD:
		case FAI_AV_QUICK_PLAY_TER_HD:
		case FAI_AV_QUICK_PLAY_TER_TV4_CI:
		case FAI_AV_QUICK_PLAY_TER_TRAVEL_CI:
			
#if defined(FACTORY_USE_CHANNEL_TER_T2) && (FACTORY_USE_CHANNEL_TER_T2==YES)
		case FAI_AV_QUICK_PLAY_TER_HD_DVBT2:
#endif
#if defined(FACTORY_USE_CHANNEL_TER) && (FACTORY_USE_CHANNEL_TER==YES)
			eFaiErr = INTRN_FAI_AV_T_QuickPlay(ucTunerId, ucStreamId);
#endif
			break;
		case FAI_AV_QUICK_PLAY_DSS_COLORBAR:
		case FAI_AV_QUICK_PLAY_DSS_MOVING:
		case FAI_AV_QUICK_PLAY_DIRECTV_DVBS2:
			/* add */
			break;
		case FAI_AV_QUICK_PLAY_NTSC_COLORBAR:
		case FAI_AV_QUICK_PLAY_NTSC_MOVING:
			break;
		default:
			break;
	}

	if( eFaiErr != FAI_NO_ERR )
	{
         eTmdErr = FAI_TMD_ERR_FAIL;
		 ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_AVPLAY;
		 ucTmdErrCode[1] = FAI_AV_TMD_ERR_QUICK_PLAY_FAIL;
		 FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	 
		 
         goto TEST_FAIL;
	}

	g_eAvTestType = FAI_AV_TEST_TMD_QUICK;
	FVK_TIMER_CreateStart(FAI_AV_MAX_TMD_TEST_WAIT, INTRN_FAI_AV_TimerCallback, NULL, 0, FVK_TIMER_ONCE, &g_ulTimerId);
	return FAI_NO_ERR;

TEST_FAIL:
    tTMDResult.eResult = eTmdErr;
    FAI_TMD_SetResult(&tTMDResult);
    return FAI_ERR;
}
#else
FAI_ERR_CODE FAI_AV_QuickPlay(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	FAI_ERR_CODE eFaiErr = FAI_ERR;
	unsigned char ucTunerId, ucStreamId;
	FAI_TMD_RESULT_T tTMDResult;
	FAI_TMD_ERR eTmdErr = FAI_TMD_ERR_OK;
	FDI_ERR_CODE eFdiErr;
	unsigned char ucTmdErrCode[2];

    FVK_MEM_Set( (void *)&tTMDResult, 0x00, sizeof(tTMDResult));

	if( ucArgc != 2 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_QuickPlay] : Argument Count Error=%d\n", ucArgc));
		eTmdErr = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
		goto TEST_FAIL;
	}

	FAI_TMD_GetParam(&ucTunerId, pucArgList, 0);
	FAI_TMD_GetParam(&ucStreamId, pucArgList, 1);

	/* If av is playing, stop it */
	if( g_bIsAvPlaying == TRUE )
	{
		INTRN_FAI_AV_Stop();
	}

	switch(ucStreamId)
	{
		case FAI_AV_QUICK_PLAY_SAT_COLORBAR_SD:
		case FAI_AV_QUICK_PLAY_SAT_MOVING_SD:
		case FAI_AV_QUICK_PLAY_SAT_HD:
		case FAI_AV_QUICK_PLAY_SAT_TV4_CI:
		case FAI_AV_QUICK_PLAY_SAT_TRAVEL_CI:
#if defined(FACTORY_USE_CHANNEL_SAT) && (FACTORY_USE_CHANNEL_SAT==YES)
			eFaiErr = INTRN_FAI_AV_S_QuickPlay(ucTunerId, ucStreamId);
#endif
			break;
		case FAI_AV_QUICK_PLAY_CAB_COLORBAR_SD:
		case FAI_AV_QUICK_PLAY_CAB_MOVING_SD:
		case FAI_AV_QUICK_PLAY_CAB_HD:
		case FAI_AV_QUICK_PLAY_CAB_TV4_CI:
		case FAI_AV_QUICK_PLAY_CAB_TRAVEL_CI:
#if defined(FACTORY_USE_CHANNEL_CAB) && (FACTORY_USE_CHANNEL_CAB==YES)
			eFaiErr = INTRN_FAI_AV_C_QuickPlay(ucTunerId, ucStreamId);
#endif
			break;
		case FAI_AV_QUICK_PLAY_TER_COLORBAR_SD:
		case FAI_AV_QUICK_PLAY_TER_MOVING_SD:
		case FAI_AV_QUICK_PLAY_TER_HD:
		case FAI_AV_QUICK_PLAY_TER_TV4_CI:
		case FAI_AV_QUICK_PLAY_TER_TRAVEL_CI:
			
#if defined(FACTORY_USE_CHANNEL_TER_T2) && (FACTORY_USE_CHANNEL_TER_T2==YES)
		case FAI_AV_QUICK_PLAY_TER_HD_DVBT2:
#endif
#if defined(FACTORY_USE_CHANNEL_TER) && (FACTORY_USE_CHANNEL_TER==YES)
			eFaiErr = INTRN_FAI_AV_T_QuickPlay(ucTunerId, ucStreamId);
#endif
			break;
		case FAI_AV_QUICK_PLAY_DSS_COLORBAR:
		case FAI_AV_QUICK_PLAY_DSS_MOVING:
		case FAI_AV_QUICK_PLAY_DIRECTV_DVBS2:
			/* add */
			break;
		case FAI_AV_QUICK_PLAY_NTSC_COLORBAR:
		case FAI_AV_QUICK_PLAY_NTSC_MOVING:
			break;
		default:
			break;
	}

	if( eFaiErr != FAI_NO_ERR )
	{
         eTmdErr = FAI_TMD_ERR_FAIL;
		 ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_AVPLAY;
		 ucTmdErrCode[1] = FAI_AV_TMD_ERR_QUICK_PLAY_FAIL;
		 FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	 
		 
         goto TEST_FAIL;
	}

	g_eAvTestType = FAI_AV_TEST_TMD_QUICK;
	FVK_TIMER_CreateStart(FAI_AV_MAX_TMD_TEST_WAIT, INTRN_FAI_AV_TimerCallback, NULL, 0, FVK_TIMER_ONCE, &g_ulTimerId);
	return FAI_NO_ERR;

TEST_FAIL:
    tTMDResult.eResult = eTmdErr;
    FAI_TMD_SetResult(&tTMDResult);
    return FAI_ERR;
}
#endif
#endif

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (COLORBAR_BEEP_ONOFF_ENABLE==YES)
FAI_ERR_CODE FAI_AV_ColorbarBeep(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	FAI_TMD_RESULT_T tTMDResult;
	FAI_TMD_ERR eTmdErr = FAI_TMD_ERR_OK;
	FAI_ERR_CODE eFaiErr;
	unsigned char ucTmdErrCode[2];
	unsigned char ucOnOff, ucResolution;

	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));

	if( ucArgc != 2 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_ColorbarBeep] : Argument Count Error=%d\n", ucArgc));
		eTmdErr = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
		goto END;
	}

	FAI_TMD_GetParam(&ucOnOff, pucArgList, 0);
	FAI_TMD_GetParam(&ucResolution, pucArgList, 1);

	eFaiErr = INTRN_FAI_AV_BeepOnOff((BOOL)ucOnOff);
	if(eFaiErr != FAI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_ColorbarBeep] : INTRN_FAI_AV_BeepOnOff Error\n"));
		eTmdErr = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_AVPLAY;
		ucTmdErrCode[1] = FAI_AV_TMD_ERR_COLOR_BEEP_FAIL;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
		goto END;
	}
	
END:
	tTMDResult.eResult= eTmdErr;
	FAI_TMD_SetResult(&tTMDResult);
	return FAI_NO_ERR;
}
#endif

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (COLORBAR_BEEP_ONOFF_UI_ENABLE==YES)
FAI_ERR_CODE FAI_AV_ColorbarBeepTest_UI(void)
{
	FDI_ERR_CODE eFdiErrRet = FDI_NO_ERR;

	/* Check BEEP are already turn on  */
	if( g_bIsBeepPlaying == TRUE )
	{
		INTRN_FAI_AV_BeepOnOff(FALSE);
	}

	/* Check AV are already playing  */
	if( g_bIsAvPlaying == TRUE )
	{
		INTRN_FAI_AV_Stop();
	}

	if( GET_STEP() == 0 )
	{
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "Colorbar", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "&Beep", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_4, "ON", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);

		/* 1. Change scart output to S-Video mode */
#if defined(FACTORY_USE_SCART) && (COLORBAR_BEEP_ONOFF_UI_SVIDEO_SUPPORT == YES)
		FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_TV, FDI_SCART_VFORMAT_SVHS);
		FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_VCR, FDI_SCART_VFORMAT_SVHS);
		FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_TV, FDI_SCART_AR_4_3);
		FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_VCR, FDI_SCART_AR_4_3);
#endif
		/* 2. Display Colorbar & Beep sound */
		INTRN_FAI_AV_BeepOnOff(TRUE);

		/* 3. Display to front */
#if defined(FACTORY_USE_SCART) && (COLORBAR_BEEP_ONOFF_UI_SVIDEO_SUPPORT == YES)
		FDI_PANEL_Display(FAI_AV_COLORBARBEEPTEST_STEP1_STR);
#else
		FDI_PANEL_Display(FAI_AV_COLORBARBEEPTEST_STEP2_STR);
#endif

		SET_STEP(1);
	}
#if defined(FACTORY_USE_SCART) && (COLORBAR_BEEP_ONOFF_UI_SVIDEO_SUPPORT == YES)
	else if( GET_STEP() == 1 )
	{
		/* 1. Change scart output to RGB mode */
		FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_TV, FDI_SCART_VFORMAT_RGB);
		FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_VCR, FDI_SCART_VFORMAT_CVBS);
		FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_TV, FDI_SCART_AR_4_3);
		FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_VCR, FDI_SCART_AR_4_3);

		/* 2. Display Colorbar & Beep sound */
		INTRN_FAI_AV_BeepOnOff(TRUE);
		
		/* 3. Display to front */
		FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_GREEN);
		FDI_PANEL_Display(FAI_AV_COLORBARBEEPTEST_STEP2_STR);
		SET_STEP(2);
	}
	else if( GET_STEP() == 2 )
	{
		/* 1. Change scart output to RGB mode */
		FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_TV, FDI_SCART_VFORMAT_RGB);
		FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_VCR, FDI_SCART_VFORMAT_CVBS);
		FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_TV, FDI_SCART_AR_4_3);
		FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_VCR, FDI_SCART_AR_4_3);
		
#else
	else if( GET_STEP() == 1 )
	{
#endif
		INTRN_FAI_AV_BeepOnOff(FALSE);

		/* 4. Display to front */
		FDI_PANEL_Display(FAI_AV_COLORBARBEEPTEST_STEP3_STR);

		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "Colorbar", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "&Beep", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_4, "OFF", RGB_COLOR_BLACK);
#if defined(FACTORY_USE_SCART) && (COLORBAR_BEEP_ONOFF_UI_SVIDEO_SUPPORT == YES)
		FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_YELLOW);
#else
		FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_GREEN);
#endif
		SET_STEP(0);
	}

	return FAI_NO_ERR;
}
#endif

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (SAT_SD_LOCK_UI_ENABLE==YES)
FAI_ERR_CODE FAI_AV_S_PlaySD_UI(void)
{
	FDI_CH_SAT_TUNE_PARAM_t tSatTuneParam;
	unsigned short usVideoPid, usAudioPid, usPcrPid;
	unsigned long ulTunerId = 0;
	BOOL bIsLocked;
	int i;
	FDI_AUDIO_FORMAT eAudioFormat;
	FDI_VIDEO_FORMAT eVideoFormat;
	unsigned char aucResult[2];
	unsigned char ucVideoFormat, ucAudioFormat;
	unsigned char szDisplayStr[20];

	/* Select tuner id */
#if (SAT_SD_LOCK_UI_SCENARIO&ONE_TUNER_SCENARIO)
	ulTunerId = 0;
#endif
#if (SAT_SD_LOCK_UI_SCENARIO&TWO_TUNER_SCENARIO)
	if( GET_STEP() == 0 )		/* channel 1 */
	{
		ulTunerId = 0;
	}
	else if( GET_STEP() == 1 ) 	/* channel 2 */
	{
		ulTunerId = 1;
	}
#endif

	/* If av is playing, stop it */
	if( g_bIsAvPlaying == TRUE )
	{
		INTRN_FAI_AV_Stop();
		FDI_CHANNEL_S_CancelLock(ulTunerId);
	}

	if( GET_STEP() == 0 )		/* channel 1 */
	{
#if defined(FACTORY_USE_SCART)
		FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_TV, FDI_SCART_VFORMAT_RGB);
		FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_TV, FDI_SCART_AR_4_3);
		FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_VCR, FDI_SCART_VFORMAT_CVBS);
		FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_VCR, FDI_SCART_AR_4_3);
#endif
#if (TV_STANDARD_OUTPUT_TYPE == TV_STANDARD_OUTPUT_PAL)
		FDI_VIDEO_SetDisplayStandard(FAI_AV_VIDEO_DEVICE_ID, FDI_VIDEO_RESOLUTION_576I);
#elif (TV_STANDARD_OUTPUT_TYPE == TV_STANDARD_OUTPUT_NTSC)
		FDI_VIDEO_SetDisplayStandard(FAI_AV_VIDEO_DEVICE_ID, FDI_VIDEO_RESOLUTION_480I);
#if ((DCONNECTOR_OUTPUT_ENABLE==YES)||(DCONNECTOR_SVIDEO_UI_ENABLE==YES))
		FDI_VIDEO_ControlDPort(0, FDI_VIDEO_RESOLUTION_480I, FDI_VIDEO_ASPECTRATIO_16_9);
#endif
#endif
		usVideoPid =  SAT_SD_LOCK_UI_VIDEO_PID_1;
		usAudioPid =  SAT_SD_LOCK_UI_AUDIO_PID_1;
		usPcrPid = SAT_SD_LOCK_UI_PCR_PID_1;
		ucVideoFormat = SAT_SD_LOCK_UI_VIDEO_FORMAT_1;
		ucAudioFormat = SAT_SD_LOCK_UI_AUDIO_FORMAT_1;
		SET_STEP(1);
	}
#if (SAT_SD_LOCK_UI_SCENARIO&TWO_STREAM_SCENARIO)
	else if( GET_STEP() == 1 ) 	/* channel 2 */
	{
#if defined(FACTORY_USE_SCART)
		FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_TV, FDI_SCART_VFORMAT_CVBS);
		FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_TV, FDI_SCART_AR_4_3);
		FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_VCR, FDI_SCART_VFORMAT_CVBS);
		FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_VCR, FDI_SCART_AR_4_3);
#endif
#if (TV_STANDARD_OUTPUT_TYPE == TV_STANDARD_OUTPUT_PAL)
		FDI_VIDEO_SetDisplayStandard(FAI_AV_VIDEO_DEVICE_ID, FDI_VIDEO_RESOLUTION_576I);
#elif (TV_STANDARD_OUTPUT_TYPE == TV_STANDARD_OUTPUT_NTSC)
		FDI_VIDEO_SetDisplayStandard(FAI_AV_VIDEO_DEVICE_ID, FDI_VIDEO_RESOLUTION_480I);
#if ((DCONNECTOR_OUTPUT_ENABLE==YES)||(DCONNECTOR_SVIDEO_UI_ENABLE==YES))
		FDI_VIDEO_ControlDPort(0, FDI_VIDEO_RESOLUTION_480I, FDI_VIDEO_ASPECTRATIO_16_9);
#endif
#endif
		usVideoPid =  SAT_SD_LOCK_UI_VIDEO_PID_2;
		usAudioPid =  SAT_SD_LOCK_UI_AUDIO_PID_2;
		usPcrPid = SAT_SD_LOCK_UI_PCR_PID_2;
		ucVideoFormat = SAT_SD_LOCK_UI_VIDEO_FORMAT_2;
		ucAudioFormat = SAT_SD_LOCK_UI_AUDIO_FORMAT_2;
		SET_STEP(2);
	}
#endif
	else 						/* stop channel */
	{
		INTRN_FAI_AV_DefaultVideoSetting();
	
		FDI_OSD_Show();
		SET_STEP(0);

		return FAI_NO_ERR;
	}

	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "SD A/V Play", RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Testing", RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);

	/* Channel Locking */
	tSatTuneParam.etAntennaType= FDI_CH_ANT_TYPE_DISEQC;
	tSatTuneParam.ulFrequency = SAT_SD_LOCK_UI_FREQUENCY;
	tSatTuneParam.ulSymbolRate = SAT_SD_LOCK_UI_SYMBOL_RATE;
	tSatTuneParam.etFecCodeRate = SAT_SD_LOCK_UI_CODERATE;
	tSatTuneParam.etPolarization = SAT_SD_LOCK_UI_POLARIZATION;
	tSatTuneParam.etTransSpec = SAT_SD_LOCK_UI_TRANS_SPEC;
	tSatTuneParam.etDiseqcInput = SAT_SD_LOCK_UI_DISEQC_INPUT;
	tSatTuneParam.b22kTone = SAT_SD_LOCK_UI_LNB22K_TONE;
	tSatTuneParam.etModulation = SAT_SD_LOCK_UI_MODULATION;
	tSatTuneParam.etPilot = SAT_SD_LOCK_UI_PILOT;
	tSatTuneParam.etRollOff = SAT_SD_LOCK_UI_ROLL_OFF;
	tSatTuneParam.etLnbVoltage = SAT_SD_LOCK_UI_LNB_VOLTAGE;
	tSatTuneParam.etDiseqcVersion = SAT_SD_LOCK_UI_DISEQC_VER;
	tSatTuneParam.ulScdUserBandNum = 0;
	tSatTuneParam.ulScdUserBandFreq = 0;

	if( FDI_CHANNEL_S_RequestLock(ulTunerId, &tSatTuneParam) != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_PlaySD_UI] : FDI_CHANNEL_S_RequestLock Error!!\n"));
		goto TEST_FAIL;
	}

	/* Locking check */
	for( i = 0; i < FAI_AV_NUM_OF_LOCK_RETRY; i++ )
	{
		FDI_CHANNEL_S_IsLocked(ulTunerId, &bIsLocked);
		if(bIsLocked == TRUE)
		{
			break;
		}
		FVK_TASK_Sleep(FAI_AV_LOCK_RETRY_DELAY);
	}
	if( bIsLocked == FALSE )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_PlaySD_UI] : Channel Unlock!!\n"));
		goto TEST_FAIL;
	}

	/* Channel lock succeed, so display it */
	sprintf(szDisplayStr, "T%d_SD_Lock", ulTunerId);
	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, szDisplayStr, RGB_COLOR_BLACK);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);
	(ulTunerId==0) ? FDI_PANEL_Display(FAI_AV_T0_PLAYSD_PASS_STR) : FDI_PANEL_Display(FAI_AV_T1_PLAYSD_PASS_STR);

	FVK_TASK_Sleep(FAI_AV_PLAY_DELAY);

	/* Convert video format */
	switch(ucVideoFormat)
	{
		case VIDEO_FORMAT_MPEG2:
			eVideoFormat = FDI_VIDEO_FORMAT_MPEG2;
			break;
		case VIDEO_FORMAT_MPEG4:
			eVideoFormat = FDI_VIDEO_FORMAT_MPEG4Part2;
			break;
		case VIDEO_FORMAT_H_264:
			eVideoFormat = FDI_VIDEO_FORMAT_H264;
			break;
		case VIDEO_FORMAT_H_265:
			eVideoFormat = FDI_VIDEO_FORMAT_H265;
			break;			
		default:
			eVideoFormat = FDI_VIDEO_FORMAT_MPEG2;
			break;
	}

	/* Convert audio format */
	switch(ucAudioFormat)
	{
		case AUDIO_FORMAT_MPEG:
			eAudioFormat = FDI_AUDIO_FORMAT_MPEG;
			break;
		case AUDIO_FORMAT_PCM:
			eAudioFormat = FDI_AUDIO_FORMAT_PCM;
			break;
		case AUDIO_FORMAT_AC3:
			eAudioFormat = FDI_AUDIO_FORMAT_AC3;
			break;
		case AUDIO_FORMAT_AAC:
			eAudioFormat = FDI_AUDIO_FORMAT_AAC;
			break;
		case AUDIO_FORMAT_MP3:
			eAudioFormat = FDI_AUDIO_FORMAT_MP3;
			break;
		default:
			eAudioFormat = FDI_AUDIO_FORMAT_MPEG;
			break;
	}
	if( INTRN_FAI_AV_Play(ulTunerId, eVideoFormat, eAudioFormat, usVideoPid, usAudioPid, usPcrPid, FDI_DEMUX_CI_BYPASS) != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_PlaySD_UI] : INTRN_FAI_AV_Play(FDI_DEMUX_CI_BYPASS) Error!!\n"));
		goto TEST_FAIL;
	}
	return FAI_NO_ERR;

TEST_FAIL:
	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_RED);
	sprintf(szDisplayStr, "T%d_SD_Unlock", ulTunerId);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, szDisplayStr, RGB_COLOR_RED);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);
	(ulTunerId==0) ? FDI_PANEL_Display(FAI_AV_T0_PLAYSD_FAIL_STR) : FDI_PANEL_Display(FAI_AV_T1_PLAYSD_FAIL_STR);
	FDI_OSD_Show();
	return FAI_ERR;
}
#endif

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (SAT_HD_LOCK_UI_ENABLE==YES)
FAI_ERR_CODE FAI_AV_S_PlayHD_UI(void)
{
	FDI_CH_SAT_TUNE_PARAM_t tSatTuneParam;
	unsigned short usVideoPid, usAudioPid, usPcrPid;
	unsigned long ulTunerId = 0;
	BOOL bIsLocked;
	int i;
	FDI_AUDIO_FORMAT eAudioFormat;
	FDI_VIDEO_FORMAT eVideoFormat;
	unsigned char aucResult[2];
	unsigned char ucVideoFormat, ucAudioFormat;
	unsigned char szDisplayStr[20];

	/* Select tuner id */
#if (SAT_HD_LOCK_UI_SCENARIO&ONE_TUNER_SCENARIO)
	ulTunerId = 0;
#endif
#if (SAT_HD_LOCK_UI_SCENARIO&TWO_TUNER_SCENARIO)
	if( GET_STEP() == 0 )		/* channel 1 */
	{
		ulTunerId = 0;
	}
	else if( GET_STEP() == 1 )	/* channel 2 */
	{
		ulTunerId = 1;
	}
#endif

	/* If av is playing, stop it */
	if( g_bIsAvPlaying == TRUE )
	{
		INTRN_FAI_AV_Stop();
		FDI_CHANNEL_S_CancelLock(ulTunerId);
	}

	if( GET_STEP() == 0 )		/* channel 1 */
	{
#if defined(FACTORY_USE_SCART)
		FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_TV, FDI_SCART_VFORMAT_CVBS);
		FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_TV, FDI_SCART_AR_16_9);
		FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_VCR, FDI_SCART_VFORMAT_CVBS);
		FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_VCR, FDI_SCART_AR_16_9);
#endif
#if defined(TV_STANDARD_OUTPUT)
#if (TV_STANDARD_OUTPUT_TYPE == TV_STANDARD_OUTPUT_PAL)
		FDI_VIDEO_SetDisplayStandard(FAI_AV_VIDEO_DEVICE_ID, FDI_VIDEO_RESOLUTION_576I);
#elif (TV_STANDARD_OUTPUT_TYPE == TV_STANDARD_OUTPUT_NTSC)
		FDI_VIDEO_SetDisplayStandard(FAI_AV_VIDEO_DEVICE_ID, FDI_VIDEO_RESOLUTION_480I);
#endif
#else
		FDI_VIDEO_SetDisplayStandard(FAI_AV_VIDEO_DEVICE_ID, FDI_VIDEO_RESOLUTION_MAX);
#endif
#if ((DCONNECTOR_OUTPUT_ENABLE==YES)||(DCONNECTOR_SVIDEO_UI_ENABLE==YES))
		FDI_VIDEO_ControlDPort(0, FDI_VIDEO_RESOLUTION_MAX, FDI_VIDEO_ASPECTRATIO_16_9);
#endif

 		usVideoPid = SAT_HD_LOCK_UI_VIDEO_PID_1;
		usAudioPid = SAT_HD_LOCK_UI_AUDIO_PID_1;
		usPcrPid = SAT_HD_LOCK_UI_PCR_PID_1;
		ucVideoFormat = SAT_HD_LOCK_UI_VIDEO_FORMAT_1;
		ucAudioFormat = SAT_HD_LOCK_UI_AUDIO_FORMAT_1;

		SET_STEP(1);
	}
#if (SAT_HD_LOCK_UI_SCENARIO&TWO_STREAM_SCENARIO)
	else if( GET_STEP() == 1 )	/* channel 2 */
	{
#if defined(FACTORY_USE_SCART)
		FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_TV, FDI_SCART_VFORMAT_RGB);
		FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_TV, FDI_SCART_AR_16_9);
		FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_VCR, FDI_SCART_VFORMAT_CVBS);
		FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_VCR, FDI_SCART_AR_16_9);
#endif
#if defined(TV_STANDARD_OUTPUT)
#if (TV_STANDARD_OUTPUT_TYPE == TV_STANDARD_OUTPUT_PAL)
		FDI_VIDEO_SetDisplayStandard(FAI_AV_VIDEO_DEVICE_ID, FDI_VIDEO_RESOLUTION_576I);
#elif (TV_STANDARD_OUTPUT_TYPE == TV_STANDARD_OUTPUT_NTSC)
		FDI_VIDEO_SetDisplayStandard(FAI_AV_VIDEO_DEVICE_ID, FDI_VIDEO_RESOLUTION_480I);
#endif
#else
		FDI_VIDEO_SetDisplayStandard(FAI_AV_VIDEO_DEVICE_ID, FDI_VIDEO_RESOLUTION_720P);		
#endif	
#if ((DCONNECTOR_OUTPUT_ENABLE==YES)||(DCONNECTOR_SVIDEO_UI_ENABLE==YES))
		FDI_VIDEO_ControlDPort(0, FDI_VIDEO_RESOLUTION_720P, FDI_VIDEO_ASPECTRATIO_16_9);
#endif

 		usVideoPid = SAT_HD_LOCK_UI_VIDEO_PID_2;
		usAudioPid = SAT_HD_LOCK_UI_AUDIO_PID_2;
		usPcrPid = SAT_HD_LOCK_UI_PCR_PID_2;
		ucVideoFormat = SAT_HD_LOCK_UI_VIDEO_FORMAT_2;
		ucAudioFormat = SAT_HD_LOCK_UI_AUDIO_FORMAT_2;
		SET_STEP(2);
	}
#endif
	else						/* stop channel */
	{
		INTRN_FAI_AV_DefaultVideoSetting();
	
		FDI_OSD_Show();
		SET_STEP(0);
		return FAI_NO_ERR;
	}

	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "HD A/V Play", RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Testing", RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);

	/* Channel Locking */
	tSatTuneParam.etAntennaType= FDI_CH_ANT_TYPE_DISEQC;
	tSatTuneParam.ulFrequency = SAT_HD_LOCK_UI_FREQUENCY;
	tSatTuneParam.ulSymbolRate = SAT_HD_LOCK_UI_SYMBOL_RATE;
	tSatTuneParam.etFecCodeRate = SAT_HD_LOCK_UI_CODERATE;
	tSatTuneParam.etPolarization = SAT_HD_LOCK_UI_POLARIZATION;
	tSatTuneParam.etTransSpec = SAT_HD_LOCK_UI_TRANS_SPEC;
	tSatTuneParam.etDiseqcInput = SAT_HD_LOCK_UI_DISEQC_INPUT;
	tSatTuneParam.b22kTone = SAT_HD_LOCK_UI_LNB22K_TONE;
	tSatTuneParam.etModulation = SAT_HD_LOCK_UI_MODULATION;
	tSatTuneParam.etPilot = SAT_HD_LOCK_UI_PILOT;
	tSatTuneParam.etRollOff = SAT_HD_LOCK_UI_ROLL_OFF;
	tSatTuneParam.etLnbVoltage = SAT_HD_LOCK_UI_LNB_VOLTAGE;
	tSatTuneParam.etDiseqcVersion = SAT_HD_LOCK_UI_DISEQC_VER;
	tSatTuneParam.ulScdUserBandNum = 0;
	tSatTuneParam.ulScdUserBandFreq = 0;

	if( FDI_CHANNEL_S_RequestLock(ulTunerId, &tSatTuneParam) != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_PlayHD_UI] : FDI_CHANNEL_S_RequestLock Error!!\n"));
		goto TEST_FAIL;
	}

	/* Locking check */
	for( i = 0; i < FAI_AV_NUM_OF_LOCK_RETRY; i++ )
	{
		FDI_CHANNEL_S_IsLocked(ulTunerId, &bIsLocked);
		if(bIsLocked == TRUE)
		{
			break;
		}
		FVK_TASK_Sleep(FAI_AV_LOCK_RETRY_DELAY);
	}
	if( bIsLocked == FALSE )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_PlayHD_UI] : Channel Unlock!!\n"));
		goto TEST_FAIL;
	}

	/* Channel lock succeed, so display it */
	sprintf(szDisplayStr, "T%d_HD_Lock", ulTunerId);
	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, szDisplayStr, RGB_COLOR_BLACK);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);
	(ulTunerId==0) ? FDI_PANEL_Display(FAI_AV_T0_PLAYHD_PASS_STR) : FDI_PANEL_Display(FAI_AV_T1_PLAYHD_PASS_STR);

	FVK_TASK_Sleep(FAI_AV_PLAY_DELAY);

	/* Convert video format */
	switch(ucVideoFormat)
	{
		case VIDEO_FORMAT_MPEG2:
			eVideoFormat = FDI_VIDEO_FORMAT_MPEG2;
			break;
		case VIDEO_FORMAT_MPEG4:
			eVideoFormat = FDI_VIDEO_FORMAT_MPEG4Part2;
			break;
		case VIDEO_FORMAT_H_264:
			eVideoFormat = FDI_VIDEO_FORMAT_H264;
			break;
		case VIDEO_FORMAT_H_265:
			eVideoFormat = FDI_VIDEO_FORMAT_H265;
			break;			
		default:
			eVideoFormat = FDI_VIDEO_FORMAT_MPEG2;
			break;
	}

	/* Convert audio format */
	switch(ucAudioFormat)
	{
		case AUDIO_FORMAT_MPEG:
			eAudioFormat = FDI_AUDIO_FORMAT_MPEG;
			break;
		case AUDIO_FORMAT_PCM:
			eAudioFormat = FDI_AUDIO_FORMAT_PCM;
			break;
		case AUDIO_FORMAT_AC3:
			eAudioFormat = FDI_AUDIO_FORMAT_AC3;
			break;
		case AUDIO_FORMAT_AAC:
			eAudioFormat = FDI_AUDIO_FORMAT_AAC;
			break;
		case AUDIO_FORMAT_MP3:
			eAudioFormat = FDI_AUDIO_FORMAT_MP3;
			break;
		default:
			eAudioFormat = FDI_AUDIO_FORMAT_MPEG;
			break;
	}
	
	if( INTRN_FAI_AV_Play(ulTunerId, eVideoFormat, eAudioFormat, usVideoPid, usAudioPid, usPcrPid, FDI_DEMUX_CI_BYPASS) != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_S_PlayHD_UI] : INTRN_FAI_AV_Play(FDI_DEMUX_CI_BYPASS) Error!!\n"));
		goto TEST_FAIL;
	}
	return FAI_NO_ERR;

TEST_FAIL:
	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_RED);
	sprintf(szDisplayStr, "T%d_HD_Unlock", ulTunerId);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, szDisplayStr, RGB_COLOR_RED);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);
	(ulTunerId==0) ? FDI_PANEL_Display(FAI_AV_T0_PLAYHD_FAIL_STR) : FDI_PANEL_Display(FAI_AV_T1_PLAYHD_FAIL_STR);

	FDI_OSD_Show();
	return FAI_ERR;
}
#endif

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (SAT_CI_LOCK_UI_ENABLE==YES)
FAI_ERR_CODE FAI_AV_S_CI_AvTest_UI(void)
{
	FDI_CH_SAT_TUNE_PARAM_t tSatTuneParam;
	unsigned short usVideoPid, usAudioPid, usPcrPid;
	unsigned long ulTunerId;
	BOOL bIsLocked;
	int i;
	FDI_AUDIO_FORMAT eAudioFormat;
	FDI_VIDEO_FORMAT eVideoFormat;
	unsigned char aucResult[2];
	unsigned char ucVideoFormat, ucAudioFormat;

	/* Select tuner id */
	ulTunerId = 0;

	/* If av is playing, stop it */
	if( g_bIsAvPlaying == TRUE )
	{
		INTRN_FAI_AV_Stop();
		FDI_CHANNEL_S_CancelLock(ulTunerId);
	}

	if( GET_STEP() == 0 )		/* channel 1 */
	{
#if defined(FACTORY_USE_SCART)
		FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_TV, FDI_SCART_VFORMAT_RGB);
		FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_TV, FDI_SCART_AR_4_3);
		FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_VCR, FDI_SCART_VFORMAT_CVBS);
		FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_VCR, FDI_SCART_AR_4_3);
#endif
#if (TV_STANDARD_OUTPUT_TYPE == TV_STANDARD_OUTPUT_PAL)
		FDI_VIDEO_SetDisplayStandard(FAI_AV_VIDEO_DEVICE_ID, FDI_VIDEO_RESOLUTION_576I);
#elif (TV_STANDARD_OUTPUT_TYPE == TV_STANDARD_OUTPUT_NTSC)
		FDI_VIDEO_SetDisplayStandard(FAI_AV_VIDEO_DEVICE_ID, FDI_VIDEO_RESOLUTION_480I);
#if ((DCONNECTOR_OUTPUT_ENABLE==YES)||(DCONNECTOR_SVIDEO_UI_ENABLE==YES))
		FDI_VIDEO_ControlDPort(0, FDI_VIDEO_RESOLUTION_480I, FDI_VIDEO_ASPECTRATIO_16_9);
#endif
#endif
		usVideoPid 		= SAT_CI_LOCK_UI_VIDEO_PID;
		usAudioPid 		= SAT_CI_LOCK_UI_AUDIO_PID;
		usPcrPid 		= SAT_CI_LOCK_UI_PCR_PID;
		ucVideoFormat 	= SAT_CI_LOCK_UI_VIDEO_FORMAT;
		ucAudioFormat 	= SAT_CI_LOCK_UI_AUDIO_FORMAT;
		SET_STEP(1);
	}
	else 						/* stop channel */
	{
		INTRN_FAI_AV_DefaultVideoSetting();

		FDI_OSD_Show();
		SET_STEP(0);

		return FAI_NO_ERR;
	}

	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "CI TS locking", RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Testing", RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);

	/* Channel Locking */
	tSatTuneParam.etAntennaType		= FDI_CH_ANT_TYPE_DISEQC;
	tSatTuneParam.ulFrequency 		= SAT_CI_LOCK_UI_FREQUENCY;
	tSatTuneParam.ulSymbolRate 		= SAT_CI_LOCK_UI_SYMBOL_RATE;
	tSatTuneParam.etFecCodeRate 	= SAT_CI_LOCK_UI_CODERATE;
	tSatTuneParam.etPolarization 	= SAT_CI_LOCK_UI_POLARIZATION;
	tSatTuneParam.etTransSpec 		= SAT_CI_LOCK_UI_TRANS_SPEC;
	tSatTuneParam.etDiseqcInput 	= SAT_CI_LOCK_UI_DISEQC_INPUT;
	tSatTuneParam.b22kTone 			= SAT_CI_LOCK_UI_LNB22K_TONE;
	tSatTuneParam.etModulation 		= SAT_CI_LOCK_UI_MODULATION;
	tSatTuneParam.etPilot 			= SAT_CI_LOCK_UI_PILOT;
	tSatTuneParam.etRollOff 		= SAT_CI_LOCK_UI_ROLL_OFF;
	tSatTuneParam.etLnbVoltage 		= SAT_CI_LOCK_UI_LNB_VOLTAGE;
	tSatTuneParam.etDiseqcVersion 	= SAT_CI_LOCK_UI_DISEQC_VER;
	tSatTuneParam.ulScdUserBandNum 	= 0;
	tSatTuneParam.ulScdUserBandFreq = 0;

	if( FDI_CHANNEL_S_RequestLock(ulTunerId, &tSatTuneParam) != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_PlaySD_UI] : FDI_CHANNEL_S_RequestLock Error!!\n"));
		goto TEST_FAIL;
	}

	/* Locking check */
	for( i = 0; i < FAI_AV_NUM_OF_LOCK_RETRY; i++ )
	{
		FDI_CHANNEL_S_IsLocked(ulTunerId, &bIsLocked);
		if(bIsLocked == TRUE)
		{
			break;
		}
		VK_TASK_Sleep(FAI_AV_LOCK_RETRY_DELAY);
	}
	if( bIsLocked == FALSE )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_PlaySD_UI] : Channel Unlock!!\n"));
		goto TEST_FAIL;
	}

	/* Channel lock succeed, so display it */
	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "CI_Lock", RGB_COLOR_BLACK);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);
	FDI_PANEL_Display(FAI_CI_AV_LOCK_STR);

	VK_TASK_Sleep(FAI_AV_PLAY_DELAY);

	/* Convert video format */
	switch(ucVideoFormat)
	{
		case VIDEO_FORMAT_MPEG2:
			eVideoFormat = FDI_VIDEO_FORMAT_MPEG2;
			break;
		case VIDEO_FORMAT_MPEG4:
			eVideoFormat = FDI_VIDEO_FORMAT_MPEG4Part2;
			break;
		case VIDEO_FORMAT_H_264:
			eVideoFormat = FDI_VIDEO_FORMAT_H264;
			break;
		case VIDEO_FORMAT_H_265:
			eVideoFormat = FDI_VIDEO_FORMAT_H265;
			break;						
		default:
			eVideoFormat = FDI_VIDEO_FORMAT_MPEG2;
			break;
	}

	/* Convert audio format */
	switch(ucAudioFormat)
	{
		case AUDIO_FORMAT_MPEG:
			eAudioFormat = FDI_AUDIO_FORMAT_MPEG;
			break;
		case AUDIO_FORMAT_PCM:
			eAudioFormat = FDI_AUDIO_FORMAT_PCM;
			break;
		case AUDIO_FORMAT_AC3:
			eAudioFormat = FDI_AUDIO_FORMAT_AC3;
			break;
		case AUDIO_FORMAT_AAC:
			eAudioFormat = FDI_AUDIO_FORMAT_AAC;
			break;
		case AUDIO_FORMAT_MP3:
			eAudioFormat = FDI_AUDIO_FORMAT_MP3;
			break;
		default:
			eAudioFormat = FDI_AUDIO_FORMAT_MPEG;
			break;
	}
	if( INTRN_FAI_AV_Play(ulTunerId, eVideoFormat, eAudioFormat, usVideoPid, usAudioPid, usPcrPid, FDI_DEMUX_CI_PASSTHROUGH) != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_PlaySD_UI] : INTRN_FAI_AV_Play(FDI_DEMUX_CI_PASSTHROUGH) Error!!\n"));
		goto TEST_FAIL;
	}
	return FAI_NO_ERR;

TEST_FAIL:
	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_RED);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "CI_Unlock", RGB_COLOR_RED);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);
	FDI_PANEL_Display(FAI_CI_AV_UNLOCK_STR);

	FDI_OSD_Show();
	return FAI_ERR;
}
#endif

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (CAB_SD_LOCK_UI_ENABLE==YES)
FAI_ERR_CODE FAI_AV_C_PlaySD_UI(void)
{
	FDI_CH_CAB_TUNE_PARAM_t tCabTuneParam;
	unsigned short usVideoPid, usAudioPid, usPcrPid;
	unsigned long ulTunerId = 0;
	BOOL bIsLocked;
	int i;
	FDI_AUDIO_FORMAT eAudioFormat;
	FDI_VIDEO_FORMAT eVideoFormat;
	unsigned char aucResult[2];
	unsigned char ucVideoFormat, ucAudioFormat;
	unsigned char szDisplayStr[20];

	/* Select tuner id */
#if (CAB_SD_LOCK_UI_SCENARIO&ONE_TUNER_SCENARIO)
	ulTunerId = 0;
#endif
#if (CAB_SD_LOCK_UI_SCENARIO&TWO_TUNER_SCENARIO)
	if( GET_STEP() == 0 )		/* channel 1 */
	{
		ulTunerId = 0;
	}
	else if( GET_STEP() == 1 )	/* channel 2 */
	{
		ulTunerId = 1;
	}
#endif

	/* If av is playing, stop it */
	if( g_bIsAvPlaying == TRUE )
	{
		INTRN_FAI_AV_Stop();
		FDI_CHANNEL_C_CancelLock(ulTunerId);
	}

	if( GET_STEP() == 0 )		/* channel 1 */
	{
#if defined(FACTORY_USE_SCART)
		FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_TV, FDI_SCART_VFORMAT_RGB);
		FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_TV, FDI_SCART_AR_4_3);
		FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_VCR, FDI_SCART_VFORMAT_CVBS);
		FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_VCR, FDI_SCART_AR_4_3);
#endif
#if (TV_STANDARD_OUTPUT_TYPE == TV_STANDARD_OUTPUT_PAL)
		FDI_VIDEO_SetDisplayStandard(FAI_AV_VIDEO_DEVICE_ID, FDI_VIDEO_RESOLUTION_576I);
#elif (TV_STANDARD_OUTPUT_TYPE == TV_STANDARD_OUTPUT_NTSC)
		FDI_VIDEO_SetDisplayStandard(FAI_AV_VIDEO_DEVICE_ID, FDI_VIDEO_RESOLUTION_480I);
#if ((DCONNECTOR_OUTPUT_ENABLE==YES)||(DCONNECTOR_SVIDEO_UI_ENABLE==YES))
		FDI_VIDEO_ControlDPort(0, FDI_VIDEO_RESOLUTION_480I, FDI_VIDEO_ASPECTRATIO_16_9);
#endif
#endif
		usVideoPid =  CAB_SD_LOCK_UI_VIDEO_PID_1;
		usAudioPid =  CAB_SD_LOCK_UI_AUDIO_PID_1;
		usPcrPid = CAB_SD_LOCK_UI_PCR_PID_1;
		ucVideoFormat = CAB_SD_LOCK_UI_VIDEO_FORMAT_1;
		ucAudioFormat = CAB_SD_LOCK_UI_AUDIO_FORMAT_1;

		SET_STEP(2);
	}
#if (CAB_SD_LOCK_UI_SCENARIO&TWO_STREAM_SCENARIO)
	else if( GET_STEP() == 1 ) 	/* channel 2 */
	{
#if defined(FACTORY_USE_SCART)
		FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_TV, FDI_SCART_VFORMAT_CVBS);
		FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_TV, FDI_SCART_AR_4_3);
		FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_VCR, FDI_SCART_VFORMAT_CVBS);
		FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_VCR, FDI_SCART_AR_4_3);
#endif
#if (TV_STANDARD_OUTPUT_TYPE == TV_STANDARD_OUTPUT_PAL)
		FDI_VIDEO_SetDisplayStandard(FAI_AV_VIDEO_DEVICE_ID, FDI_VIDEO_RESOLUTION_576I);
#elif (TV_STANDARD_OUTPUT_TYPE == TV_STANDARD_OUTPUT_NTSC)
		FDI_VIDEO_SetDisplayStandard(FAI_AV_VIDEO_DEVICE_ID, FDI_VIDEO_RESOLUTION_480I);
#if ((DCONNECTOR_OUTPUT_ENABLE==YES)||(DCONNECTOR_SVIDEO_UI_ENABLE==YES))
		FDI_VIDEO_ControlDPort(0, FDI_VIDEO_RESOLUTION_480I, FDI_VIDEO_ASPECTRATIO_16_9);
#endif
#endif
		usVideoPid =  CAB_SD_LOCK_UI_VIDEO_PID_2;
		usAudioPid =  CAB_SD_LOCK_UI_AUDIO_PID_2;
		usPcrPid = CAB_SD_LOCK_UI_PCR_PID_2;
		ucVideoFormat = CAB_SD_LOCK_UI_VIDEO_FORMAT_2;
		ucAudioFormat = CAB_SD_LOCK_UI_AUDIO_FORMAT_2;

		SET_STEP(2);
	}
#endif
	else 						/* stop channel */
	{
		INTRN_FAI_AV_DefaultVideoSetting();
	
		FDI_OSD_Show();
		SET_STEP(0);

		return FAI_NO_ERR;
	}

	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "SD A/V Play", RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Testing", RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);

	/* Channel Locking */
	tCabTuneParam.ulFrequency = CAB_SD_LOCK_UI_FREQUENCY;
	tCabTuneParam.ulSymbolRate = CAB_SD_LOCK_UI_SYMBOL_RATE;
	tCabTuneParam.etModulation = FDI_CH_CMOD_AUTO;
	tCabTuneParam.etSpectrum = FDI_CH_INVERSION_AUTO;

	if( FDI_CHANNEL_C_RequestLock(ulTunerId, &tCabTuneParam) != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_PlaySD_UI] : FDI_CHANNEL_C_RequestLock Error!!\n"));
		goto TEST_FAIL;
	}

	/* Locking check */
	for( i = 0; i < FAI_AV_NUM_OF_LOCK_RETRY; i++ )
	{
		FDI_CHANNEL_C_IsLocked(ulTunerId, &bIsLocked);
		if(bIsLocked == TRUE)
		{
			break;
		}
		FVK_TASK_Sleep(FAI_AV_LOCK_RETRY_DELAY);
	}
	if( bIsLocked == FALSE )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_PlaySD_UI] : Channel Unlock!!\n"));
		goto TEST_FAIL;
	}

	/* Channel lock succeed, so display it */
	sprintf(szDisplayStr, "T%d_SD_Lock", ulTunerId);
	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, szDisplayStr, RGB_COLOR_BLACK);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);
	(ulTunerId==0) ? FDI_PANEL_Display(FAI_AV_T0_PLAYSD_PASS_STR) : FDI_PANEL_Display(FAI_AV_T1_PLAYSD_PASS_STR);

	FVK_TASK_Sleep(FAI_AV_PLAY_DELAY);

	/* Convert video format */
	switch(ucVideoFormat)
	{
		case VIDEO_FORMAT_MPEG2:
			eVideoFormat = FDI_VIDEO_FORMAT_MPEG2;
			break;
		case VIDEO_FORMAT_MPEG4:
			eVideoFormat = FDI_VIDEO_FORMAT_MPEG4Part2;
			break;
		case VIDEO_FORMAT_H_264:
			eVideoFormat = FDI_VIDEO_FORMAT_H264;
			break;
		case VIDEO_FORMAT_H_265:
			eVideoFormat = FDI_VIDEO_FORMAT_H265;
			break;						
		default:
			eVideoFormat = FDI_VIDEO_FORMAT_MPEG2;
			break;
	}

	/* Convert audio format */
	switch(ucAudioFormat)
	{
		case AUDIO_FORMAT_MPEG:
			eAudioFormat = FDI_AUDIO_FORMAT_MPEG;
			break;
		case AUDIO_FORMAT_PCM:
			eAudioFormat = FDI_AUDIO_FORMAT_PCM;
			break;
		case AUDIO_FORMAT_AC3:
			eAudioFormat = FDI_AUDIO_FORMAT_AC3;
			break;
		case AUDIO_FORMAT_AAC:
			eAudioFormat = FDI_AUDIO_FORMAT_AAC;
			break;
		case AUDIO_FORMAT_MP3:
			eAudioFormat = FDI_AUDIO_FORMAT_MP3;
			break;
		default:
			eAudioFormat = FDI_AUDIO_FORMAT_MPEG;
			break;
	}
	if( INTRN_FAI_AV_Play(ulTunerId, eVideoFormat, eAudioFormat, usVideoPid, usAudioPid, usPcrPid, FDI_DEMUX_CI_BYPASS) != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_PlaySD_UI] : INTRN_FAI_AV_Play(FDI_DEMUX_CI_BYPASS) Error!!\n"));
		goto TEST_FAIL;
	}
	return FAI_NO_ERR;

TEST_FAIL:
	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_RED);
	sprintf(szDisplayStr, "T%d_SD_Unlock", ulTunerId);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, szDisplayStr, RGB_COLOR_RED);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);
	(ulTunerId==0) ? FDI_PANEL_Display(FAI_AV_T0_PLAYSD_FAIL_STR) : FDI_PANEL_Display(FAI_AV_T1_PLAYSD_FAIL_STR);

	FDI_OSD_Show();
	return FAI_ERR;
}
#endif

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (CAB_HD_LOCK_UI_ENABLE==YES)
FAI_ERR_CODE FAI_AV_C_PlayHD_UI(void)
{
	FDI_CH_CAB_TUNE_PARAM_t tCabTuneParam;
	unsigned short usVideoPid, usAudioPid, usPcrPid;
	unsigned long ulTunerId = 0;
	BOOL bIsLocked;
	int i;
	FDI_AUDIO_FORMAT eAudioFormat;
	FDI_VIDEO_FORMAT eVideoFormat;
	unsigned char aucResult[2];
	unsigned char ucVideoFormat, ucAudioFormat;
	unsigned char szDisplayStr[20];

	/* Select tuner id */
#if (CAB_HD_LOCK_UI_SCENARIO&ONE_TUNER_SCENARIO)
	ulTunerId = 0;
#endif
#if (CAB_HD_LOCK_UI_SCENARIO&TWO_TUNER_SCENARIO)
	if( GET_STEP() == 0 )		/* channel 1 */
	{
		ulTunerId = 0;
	}
	else if( GET_STEP() == 1 )	/* channel 2 */
	{
		ulTunerId = 1;
	}
#endif

	/* If av is playing, stop it */
	if( g_bIsAvPlaying == TRUE )
	{
		INTRN_FAI_AV_Stop();
		FDI_CHANNEL_C_CancelLock(ulTunerId);
	}

	if( GET_STEP() == 0 )		/* channel 1 */
	{
#if defined(FACTORY_USE_SCART)
		FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_TV, FDI_SCART_VFORMAT_CVBS);
		FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_TV, FDI_SCART_AR_16_9);
		FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_VCR, FDI_SCART_VFORMAT_CVBS);
		FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_VCR, FDI_SCART_AR_16_9);
#endif
#if defined(TV_STANDARD_OUTPUT)		
#if (TV_STANDARD_OUTPUT_TYPE == TV_STANDARD_OUTPUT_PAL)
		FDI_VIDEO_SetDisplayStandard(FAI_AV_VIDEO_DEVICE_ID, FDI_VIDEO_RESOLUTION_576I);
#elif (TV_STANDARD_OUTPUT_TYPE == TV_STANDARD_OUTPUT_NTSC)
		FDI_VIDEO_SetDisplayStandard(FAI_AV_VIDEO_DEVICE_ID, FDI_VIDEO_RESOLUTION_480I);
#endif
#else
		FDI_VIDEO_SetDisplayStandard(FAI_AV_VIDEO_DEVICE_ID, FDI_VIDEO_RESOLUTION_MAX);
#endif
#if ((DCONNECTOR_OUTPUT_ENABLE==YES)||(DCONNECTOR_SVIDEO_UI_ENABLE==YES))
		FDI_VIDEO_ControlDPort(0, FDI_VIDEO_RESOLUTION_MAX, FDI_VIDEO_ASPECTRATIO_16_9);
#endif

		usVideoPid = CAB_HD_LOCK_UI_VIDEO_PID_1;
		usAudioPid = CAB_HD_LOCK_UI_AUDIO_PID_1;
		usPcrPid = CAB_HD_LOCK_UI_PCR_PID_1;
		ucVideoFormat = CAB_HD_LOCK_UI_VIDEO_FORMAT_1;
		ucAudioFormat = CAB_HD_LOCK_UI_AUDIO_FORMAT_1;

		SET_STEP(1);
	}
#if (CAB_HD_LOCK_UI_SCENARIO&TWO_STREAM_SCENARIO)
	else if( GET_STEP() == 1 )	/* channel 2 */
	{
#if defined(FACTORY_USE_SCART)
		FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_TV, FDI_SCART_VFORMAT_RGB);
		FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_TV, FDI_SCART_AR_16_9);
		FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_VCR, FDI_SCART_VFORMAT_CVBS);
		FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_VCR, FDI_SCART_AR_16_9);
#endif
#if defined(TV_STANDARD_OUTPUT)
#if (TV_STANDARD_OUTPUT_TYPE == TV_STANDARD_OUTPUT_PAL)
		FDI_VIDEO_SetDisplayStandard(FAI_AV_VIDEO_DEVICE_ID, FDI_VIDEO_RESOLUTION_576I);
#elif (TV_STANDARD_OUTPUT_TYPE == TV_STANDARD_OUTPUT_NTSC)
		FDI_VIDEO_SetDisplayStandard(FAI_AV_VIDEO_DEVICE_ID, FDI_VIDEO_RESOLUTION_480I);
#endif
#else
		FDI_VIDEO_SetDisplayStandard(FAI_AV_VIDEO_DEVICE_ID, FDI_VIDEO_RESOLUTION_720P);
#endif	
#if ((DCONNECTOR_OUTPUT_ENABLE==YES)||(DCONNECTOR_SVIDEO_UI_ENABLE==YES))
		FDI_VIDEO_ControlDPort(0, FDI_VIDEO_RESOLUTION_720P, FDI_VIDEO_ASPECTRATIO_16_9);
#endif

 		usVideoPid = CAB_HD_LOCK_UI_VIDEO_PID_2;
		usAudioPid = CAB_HD_LOCK_UI_AUDIO_PID_2;
		usPcrPid = CAB_HD_LOCK_UI_PCR_PID_2;
		ucVideoFormat = CAB_HD_LOCK_UI_VIDEO_FORMAT_2;
		ucAudioFormat = CAB_HD_LOCK_UI_AUDIO_FORMAT_2;
		SET_STEP(2);
	}
#endif
	else						/* stop channel */
	{
		INTRN_FAI_AV_DefaultVideoSetting();

		FDI_OSD_Show();
		SET_STEP(0);
		return FAI_NO_ERR;
	}

	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "HD A/V Play", RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Testing", RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);

	/* Channel Locking */
	tCabTuneParam.ulFrequency = CAB_HD_LOCK_UI_FREQUENCY;
	tCabTuneParam.ulSymbolRate = CAB_HD_LOCK_UI_SYMBOL_RATE;
	tCabTuneParam.etModulation = FDI_CH_CMOD_AUTO;
	tCabTuneParam.etSpectrum = FDI_CH_INVERSION_AUTO;

	if( FDI_CHANNEL_C_RequestLock(ulTunerId, &tCabTuneParam) != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_PlayHD_UI] : FDI_CHANNEL_C_RequestLock Error!!\n"));
		goto TEST_FAIL;
	}

	/* Locking check */
	for( i = 0; i < FAI_AV_NUM_OF_LOCK_RETRY; i++ )
	{
		FDI_CHANNEL_C_IsLocked(ulTunerId, &bIsLocked);
		if(bIsLocked == TRUE)
		{
			break;
		}
		FVK_TASK_Sleep(FAI_AV_LOCK_RETRY_DELAY);
	}
	if( bIsLocked == FALSE )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_PlayHD_UI] : Channel Unlock!!\n"));
		goto TEST_FAIL;
	}

	/* Channel lock succeed, so display it */
	sprintf(szDisplayStr, "T%d_HD_Lock", ulTunerId);
	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, szDisplayStr, RGB_COLOR_BLACK);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);
	(ulTunerId==0) ? FDI_PANEL_Display(FAI_AV_T0_PLAYHD_PASS_STR) : FDI_PANEL_Display(FAI_AV_T1_PLAYHD_PASS_STR);

	FVK_TASK_Sleep(FAI_AV_PLAY_DELAY);

	/* Convert video format */
	switch(ucVideoFormat)
	{
		case VIDEO_FORMAT_MPEG2:
			eVideoFormat = FDI_VIDEO_FORMAT_MPEG2;
			break;
		case VIDEO_FORMAT_MPEG4:
			eVideoFormat = FDI_VIDEO_FORMAT_MPEG4Part2;
			break;
		case VIDEO_FORMAT_H_264:
			eVideoFormat = FDI_VIDEO_FORMAT_H264;
			break;
		case VIDEO_FORMAT_H_265:
			eVideoFormat = FDI_VIDEO_FORMAT_H265;
			break;						
		default:
			eVideoFormat = FDI_VIDEO_FORMAT_MPEG2;
			break;
	}

	/* Convert audio format */
	switch(ucAudioFormat)
	{
		case AUDIO_FORMAT_MPEG:
			eAudioFormat = FDI_AUDIO_FORMAT_MPEG;
			break;
		case AUDIO_FORMAT_PCM:
			eAudioFormat = FDI_AUDIO_FORMAT_PCM;
			break;
		case AUDIO_FORMAT_AC3:
			eAudioFormat = FDI_AUDIO_FORMAT_AC3;
			break;
		case AUDIO_FORMAT_AAC:
			eAudioFormat = FDI_AUDIO_FORMAT_AAC;
			break;
		case AUDIO_FORMAT_MP3:
			eAudioFormat = FDI_AUDIO_FORMAT_MP3;
			break;
		default:
			eAudioFormat = FDI_AUDIO_FORMAT_MPEG;
			break;
	}
	if( INTRN_FAI_AV_Play(ulTunerId, eVideoFormat, eAudioFormat, usVideoPid, usAudioPid, usPcrPid, FDI_DEMUX_CI_BYPASS) != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_C_PlayHD_UI] : INTRN_FAI_AV_Play(FDI_DEMUX_CI_BYPASS) Error!!\n"));
		goto TEST_FAIL;
	}
	return FAI_NO_ERR;

TEST_FAIL:
	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_RED);
	sprintf(szDisplayStr, "T%d_HD_Unlock", ulTunerId);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, szDisplayStr, RGB_COLOR_RED);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);
	(ulTunerId==0) ? FDI_PANEL_Display(FAI_AV_T0_PLAYHD_FAIL_STR) : FDI_PANEL_Display(FAI_AV_T1_PLAYHD_FAIL_STR);

	FDI_OSD_Show();
	return FAI_ERR;
}
#endif

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (CAB_CI_LOCK_UI_ENABLE==YES)
FAI_ERR_CODE FAI_AV_C_CI_AvTest_UI(void)
{
	FDI_CH_CAB_TUNE_PARAM_t tCabTuneParam;
	unsigned short usVideoPid, usAudioPid, usPcrPid;
	unsigned long ulTunerId;
	BOOL bIsLocked;
	int i;
	FDI_AUDIO_FORMAT eAudioFormat;
	FDI_VIDEO_FORMAT eVideoFormat;
	unsigned char aucResult[2];
	unsigned char ucVideoFormat, ucAudioFormat;

	/* Select tuner id */
	ulTunerId = 0;

	/* If av is playing, stop it */
	if( g_bIsAvPlaying == TRUE )
	{
		INTRN_FAI_AV_Stop();
		FDI_CHANNEL_C_CancelLock(ulTunerId);
	}

	if( GET_STEP() == 0 )		/* channel 1 */
	{
#if defined(FACTORY_USE_SCART)
		FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_TV, FDI_SCART_VFORMAT_RGB);
		FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_TV, FDI_SCART_AR_4_3);
		FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_VCR, FDI_SCART_VFORMAT_CVBS);
		FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_VCR, FDI_SCART_AR_4_3);
#endif
#if (TV_STANDARD_OUTPUT_TYPE == TV_STANDARD_OUTPUT_PAL)
		FDI_VIDEO_SetDisplayStandard(FAI_AV_VIDEO_DEVICE_ID, FDI_VIDEO_RESOLUTION_576I);
#elif (TV_STANDARD_OUTPUT_TYPE == TV_STANDARD_OUTPUT_NTSC)
		FDI_VIDEO_SetDisplayStandard(FAI_AV_VIDEO_DEVICE_ID, FDI_VIDEO_RESOLUTION_480I);
#if ((DCONNECTOR_OUTPUT_ENABLE==YES)||(DCONNECTOR_SVIDEO_UI_ENABLE==YES))
		FDI_VIDEO_ControlDPort(0, FDI_VIDEO_RESOLUTION_480I, FDI_VIDEO_ASPECTRATIO_16_9);
#endif
#endif
		usVideoPid 		= CAB_CI_LOCK_UI_VIDEO_PID;
		usAudioPid 		= CAB_CI_LOCK_UI_AUDIO_PID;
		usPcrPid 		= CAB_CI_LOCK_UI_PCR_PID;
		ucVideoFormat 	= CAB_CI_LOCK_UI_VIDEO_FORMAT;
		ucAudioFormat 	= CAB_CI_LOCK_UI_AUDIO_FORMAT;
		SET_STEP(1);
	}
	else 						/* stop channel */
	{
		INTRN_FAI_AV_DefaultVideoSetting();

		FDI_OSD_Show();
		SET_STEP(0);

		return FAI_NO_ERR;
	}

	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "CI TS locking", RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Testing", RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);

	/* Channel Locking */
	tCabTuneParam.ulFrequency = CAB_CI_LOCK_UI_FREQUENCY;
	tCabTuneParam.ulSymbolRate = CAB_CI_LOCK_UI_SYMBOL_RATE;
	tCabTuneParam.etModulation = FDI_CH_CMOD_AUTO;
	tCabTuneParam.etSpectrum = FDI_CH_INVERSION_AUTO;

	if( FDI_CHANNEL_C_RequestLock(ulTunerId, &tCabTuneParam) != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_PlaySD_UI] : FDI_CHANNEL_S_RequestLock Error!!\n"));
		goto TEST_FAIL;
	}

	/* Locking check */
	for( i = 0; i < FAI_AV_NUM_OF_LOCK_RETRY; i++ )
	{
		FDI_CHANNEL_C_IsLocked(ulTunerId, &bIsLocked);
		if(bIsLocked == TRUE)
		{
			break;
		}
		VK_TASK_Sleep(FAI_AV_LOCK_RETRY_DELAY);
	}
	if( bIsLocked == FALSE )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_PlaySD_UI] : Channel Unlock!!\n"));
		goto TEST_FAIL;
	}

	/* Channel lock succeed, so display it */
	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "CI_Lock", RGB_COLOR_BLACK);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);
	FDI_PANEL_Display(FAI_CI_AV_LOCK_STR);

	VK_TASK_Sleep(FAI_AV_PLAY_DELAY);

	/* Convert video format */
	switch(ucVideoFormat)
	{
		case VIDEO_FORMAT_MPEG2:
			eVideoFormat = FDI_VIDEO_FORMAT_MPEG2;
			break;
		case VIDEO_FORMAT_MPEG4:
			eVideoFormat = FDI_VIDEO_FORMAT_MPEG4Part2;
			break;
		case VIDEO_FORMAT_H_264:
			eVideoFormat = FDI_VIDEO_FORMAT_H264;
			break;
		case VIDEO_FORMAT_H_265:
			eVideoFormat = FDI_VIDEO_FORMAT_H265;
			break;						
		default:
			eVideoFormat = FDI_VIDEO_FORMAT_MPEG2;
			break;
	}

	/* Convert audio format */
	switch(ucAudioFormat)
	{
		case AUDIO_FORMAT_MPEG:
			eAudioFormat = FDI_AUDIO_FORMAT_MPEG;
			break;
		case AUDIO_FORMAT_PCM:
			eAudioFormat = FDI_AUDIO_FORMAT_PCM;
			break;
		case AUDIO_FORMAT_AC3:
			eAudioFormat = FDI_AUDIO_FORMAT_AC3;
			break;
		case AUDIO_FORMAT_AAC:
			eAudioFormat = FDI_AUDIO_FORMAT_AAC;
			break;
		case AUDIO_FORMAT_MP3:
			eAudioFormat = FDI_AUDIO_FORMAT_MP3;
			break;
		default:
			eAudioFormat = FDI_AUDIO_FORMAT_MPEG;
			break;
	}
	if( INTRN_FAI_AV_Play(ulTunerId, eVideoFormat, eAudioFormat, usVideoPid, usAudioPid, usPcrPid, FDI_DEMUX_CI_PASSTHROUGH) != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_PlaySD_UI] : INTRN_FAI_AV_Play(FDI_DEMUX_CI_PASSTHROUGH) Error!!\n"));
		goto TEST_FAIL;
	}
	return FAI_NO_ERR;

TEST_FAIL:
	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_RED);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "CI_Unlock", RGB_COLOR_RED);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);
	FDI_PANEL_Display(FAI_CI_AV_UNLOCK_STR);

	FDI_OSD_Show();
	return FAI_ERR;
}
#endif

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (TER_SD_LOCK_UI_ENABLE==YES)
FAI_ERR_CODE FAI_AV_T_PlaySD_UI(void)
{
	FDI_CH_TER_TUNE_PARAM_t tTerTuneParam;
	unsigned short usVideoPid, usAudioPid, usPcrPid;
	unsigned long ulTunerId = 0;
	BOOL bIsLocked;
	int i;
	FDI_AUDIO_FORMAT eAudioFormat;
	FDI_VIDEO_FORMAT eVideoFormat;
	unsigned char aucResult[2];
	unsigned char ucVideoFormat, ucAudioFormat;
	unsigned char szDisplayStr[20];

	/* Select tuner id */
#if (TER_SD_LOCK_UI_SCENARIO&ONE_TUNER_SCENARIO)
	ulTunerId = 0;
#endif
#if (TER_SD_LOCK_UI_SCENARIO&TWO_TUNER_SCENARIO)
	if( GET_STEP() == 0 )		/* channel 1 */
	{
		ulTunerId = 0;
	}
	else if( GET_STEP() == 1 )	/* channel 2 */
	{
		ulTunerId = 1;
	}
#endif

	/* If av is playing, stop it */
	if( g_bIsAvPlaying == TRUE )
	{
		INTRN_FAI_AV_Stop();
		FDI_CHANNEL_T_CancelLock(ulTunerId);
	}

	if( GET_STEP() == 0 )		/* channel 1 */
	{
#if defined(FACTORY_USE_SCART)
		FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_TV, FDI_SCART_VFORMAT_RGB);
		FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_TV, FDI_SCART_AR_4_3);
		FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_VCR, FDI_SCART_VFORMAT_CVBS);
		FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_VCR, FDI_SCART_AR_4_3);
#endif
#if (TV_STANDARD_OUTPUT_TYPE == TV_STANDARD_OUTPUT_PAL)
		FDI_VIDEO_SetDisplayStandard(FAI_AV_VIDEO_DEVICE_ID, FDI_VIDEO_RESOLUTION_576I);
#elif (TV_STANDARD_OUTPUT_TYPE == TV_STANDARD_OUTPUT_NTSC)
		FDI_VIDEO_SetDisplayStandard(FAI_AV_VIDEO_DEVICE_ID, FDI_VIDEO_RESOLUTION_480I);
#if ((DCONNECTOR_OUTPUT_ENABLE==YES)||(DCONNECTOR_SVIDEO_UI_ENABLE==YES))
		FDI_VIDEO_ControlDPort(0, FDI_VIDEO_RESOLUTION_480I, FDI_VIDEO_ASPECTRATIO_16_9);
#endif
#endif
		usVideoPid =	TER_SD_LOCK_UI_VIDEO_PID_1;
		usAudioPid =	TER_SD_LOCK_UI_AUDIO_PID_1;
		usPcrPid = TER_SD_LOCK_UI_PCR_PID_1;
		ucVideoFormat = TER_SD_LOCK_UI_VIDEO_FORMAT_1;
		ucAudioFormat = TER_SD_LOCK_UI_AUDIO_FORMAT_1;

		SET_STEP(1);
	}
#if (TER_SD_LOCK_UI_SCENARIO&TWO_STREAM_SCENARIO)
	else if( GET_STEP() == 1 )	/* channel 2 */
	{
#if defined(FACTORY_USE_SCART)
		FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_TV, FDI_SCART_VFORMAT_CVBS);
		FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_TV, FDI_SCART_AR_4_3);
		FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_VCR, FDI_SCART_VFORMAT_CVBS);
		FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_VCR, FDI_SCART_AR_4_3);
#endif
#if (TV_STANDARD_OUTPUT_TYPE == TV_STANDARD_OUTPUT_PAL)
		FDI_VIDEO_SetDisplayStandard(FAI_AV_VIDEO_DEVICE_ID, FDI_VIDEO_RESOLUTION_576I);
#elif (TV_STANDARD_OUTPUT_TYPE == TV_STANDARD_OUTPUT_NTSC)
		FDI_VIDEO_SetDisplayStandard(FAI_AV_VIDEO_DEVICE_ID, FDI_VIDEO_RESOLUTION_480I);
#if ((DCONNECTOR_OUTPUT_ENABLE==YES)||(DCONNECTOR_SVIDEO_UI_ENABLE==YES))
		FDI_VIDEO_ControlDPort(0, FDI_VIDEO_RESOLUTION_480I, FDI_VIDEO_ASPECTRATIO_16_9);
#endif
#endif
		usVideoPid =	TER_SD_LOCK_UI_VIDEO_PID_2;
		usAudioPid =	TER_SD_LOCK_UI_AUDIO_PID_2;
		usPcrPid = TER_SD_LOCK_UI_PCR_PID_2;
		ucVideoFormat = TER_SD_LOCK_UI_VIDEO_FORMAT_2;
		ucAudioFormat = TER_SD_LOCK_UI_AUDIO_FORMAT_2;
		SET_STEP(2);
	}
#endif
	else						/* stop channel */
	{
		INTRN_FAI_AV_DefaultVideoSetting();
	
		FDI_OSD_Show();
		SET_STEP(0);

		return FAI_NO_ERR;
	}

	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "SD A/V Play", RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Testing", RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);

	/* Channel Locking */
	tTerTuneParam.ulFrequency = TER_SD_LOCK_UI_FREQUENCY;
	tTerTuneParam.etBandWidth = TER_SD_LOCK_UI_BANDWIDTH;
	tTerTuneParam.etCodeRate = FDI_CH_TCODERATE_AUTO;
	tTerTuneParam.etGuardInterval = FDI_CH_GUARDINTERVAL_AUTO;
	tTerTuneParam.etHierarchy = FDI_CH_HIERARCHY_AUTO;
	tTerTuneParam.etModulation = TER_SD_LOCK_UI_MODULATION;
	tTerTuneParam.etOffset = FDI_CH_OFFSET_AUTO;
	tTerTuneParam.etTransMode = FDI_CH_TRANSMODE_AUTO;
	tTerTuneParam.etStream = FDI_CH_STREAM_HP;
	tTerTuneParam.etTerSystem = TER_SD_LOCK_UI_SYSTEM;

	if( FDI_CHANNEL_T_RequestLock(ulTunerId, &tTerTuneParam) != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_PlaySD_UI] : FDI_CHANNEL_C_RequestLock Error!!\n"));
		goto TEST_FAIL;
	}

	/* Locking check */
	for( i = 0; i < FAI_AV_NUM_OF_LOCK_RETRY; i++ )
	{
		FDI_CHANNEL_T_IsLocked(ulTunerId, &bIsLocked);
		if(bIsLocked == TRUE)
		{
			break;
		}
		FVK_TASK_Sleep(FAI_AV_LOCK_RETRY_DELAY);
	}
	if( bIsLocked == FALSE )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_PlaySD_UI] : Channel Unlock!!\n"));
		goto TEST_FAIL;
	}

	/* Channel lock succeed, so display it */
	sprintf(szDisplayStr, "T%d_SD_Lock", ulTunerId);
	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, szDisplayStr, RGB_COLOR_BLACK);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);
	(ulTunerId==0) ? FDI_PANEL_Display(FAI_AV_T0_PLAYSD_PASS_STR) : FDI_PANEL_Display(FAI_AV_T1_PLAYSD_PASS_STR);

	FVK_TASK_Sleep(FAI_AV_PLAY_DELAY);

	/* Convert video format */
	switch(ucVideoFormat)
	{
		case VIDEO_FORMAT_MPEG2:
			eVideoFormat = FDI_VIDEO_FORMAT_MPEG2;
			break;
		case VIDEO_FORMAT_MPEG4:
			eVideoFormat = FDI_VIDEO_FORMAT_MPEG4Part2;
			break;
		case VIDEO_FORMAT_H_264:
			eVideoFormat = FDI_VIDEO_FORMAT_H264;
			break;
		case VIDEO_FORMAT_H_265:
			eVideoFormat = FDI_VIDEO_FORMAT_H265;
			break;						
		default:
			eVideoFormat = FDI_VIDEO_FORMAT_MPEG2;
			break;
	}

	/* Convert audio format */
	switch(ucAudioFormat)
	{
		case AUDIO_FORMAT_MPEG:
			eAudioFormat = FDI_AUDIO_FORMAT_MPEG;
			break;
		case AUDIO_FORMAT_PCM:
			eAudioFormat = FDI_AUDIO_FORMAT_PCM;
			break;
		case AUDIO_FORMAT_AC3:
			eAudioFormat = FDI_AUDIO_FORMAT_AC3;
			break;
		case AUDIO_FORMAT_AAC:
			eAudioFormat = FDI_AUDIO_FORMAT_AAC;
			break;
		case AUDIO_FORMAT_MP3:
			eAudioFormat = FDI_AUDIO_FORMAT_MP3;
			break;
		default:
			eAudioFormat = FDI_AUDIO_FORMAT_MPEG;
			break;
	}
	if( INTRN_FAI_AV_Play(ulTunerId, eVideoFormat, eAudioFormat, usVideoPid, usAudioPid, usPcrPid, FDI_DEMUX_CI_BYPASS) != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_T_PlaySD_UI] : INTRN_FAI_AV_Play(FDI_DEMUX_CI_BYPASS) Error!!\n"));
		goto TEST_FAIL;
	}
	return FAI_NO_ERR;

TEST_FAIL:
	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_RED);
	sprintf(szDisplayStr, "T%d_SD_Unlock", ulTunerId);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, szDisplayStr, RGB_COLOR_RED);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);
	(ulTunerId==0) ? FDI_PANEL_Display(FAI_AV_T0_PLAYSD_FAIL_STR) : FDI_PANEL_Display(FAI_AV_T1_PLAYSD_FAIL_STR);

	FDI_OSD_Show();
	return FAI_ERR;
}
#endif

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (TER_HD_LOCK_UI_ENABLE==YES)
FAI_ERR_CODE FAI_AV_T_PlayHD_UI(void)
{
	FDI_CH_TER_TUNE_PARAM_t tTerTuneParam;
	unsigned short usVideoPid, usAudioPid, usPcrPid;
	unsigned long ulTunerId = 0;
	BOOL bIsLocked;
	int i;
	FDI_AUDIO_FORMAT eAudioFormat;
	FDI_VIDEO_FORMAT eVideoFormat;
	unsigned char aucResult[2];
	unsigned char ucVideoFormat, ucAudioFormat;
	unsigned char szDisplayStr[20];

	/* Select tuner id */
#if (TER_HD_LOCK_UI_SCENARIO&ONE_TUNER_SCENARIO)
	ulTunerId = 0;
#endif
#if (TER_HD_LOCK_UI_SCENARIO&TWO_TUNER_SCENARIO)
	if( GET_STEP() == 0 )		/* channel 1 */
	{
		ulTunerId = 0;
	}
	else if( GET_STEP() == 1 )	/* channel 2 */
	{
		ulTunerId = 1;
	}
#endif

	/* If av is playing, stop it */
	if( g_bIsAvPlaying == TRUE )
	{
		INTRN_FAI_AV_Stop();
		FDI_CHANNEL_T_CancelLock(ulTunerId);
	}

	if( GET_STEP() == 0 )		/* channel 1 */
	{
#if defined(FACTORY_USE_SCART)
		FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_TV, FDI_SCART_VFORMAT_CVBS);
		FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_TV, FDI_SCART_AR_16_9);
		FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_VCR, FDI_SCART_VFORMAT_CVBS);
		FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_VCR, FDI_SCART_AR_16_9);
#endif
#if defined(TV_STANDARD_OUTPUT)		
#if (TV_STANDARD_OUTPUT_TYPE == TV_STANDARD_OUTPUT_PAL)
		FDI_VIDEO_SetDisplayStandard(FAI_AV_VIDEO_DEVICE_ID, FDI_VIDEO_RESOLUTION_576I);
#elif (TV_STANDARD_OUTPUT_TYPE == TV_STANDARD_OUTPUT_NTSC)
		FDI_VIDEO_SetDisplayStandard(FAI_AV_VIDEO_DEVICE_ID, FDI_VIDEO_RESOLUTION_480I);
#endif
#else
		FDI_VIDEO_SetDisplayStandard(FAI_AV_VIDEO_DEVICE_ID, FDI_VIDEO_RESOLUTION_MAX);		
#endif
#if ((DCONNECTOR_OUTPUT_ENABLE==YES)||(DCONNECTOR_SVIDEO_UI_ENABLE==YES))
		FDI_VIDEO_ControlDPort(0, FDI_VIDEO_RESOLUTION_MAX, FDI_VIDEO_ASPECTRATIO_16_9);
#endif

		usVideoPid =	TER_HD_LOCK_UI_VIDEO_PID_1;
		usAudioPid =	TER_HD_LOCK_UI_AUDIO_PID_1;
		usPcrPid = TER_HD_LOCK_UI_PCR_PID_1;
		ucVideoFormat = TER_HD_LOCK_UI_VIDEO_FORMAT_1;
		ucAudioFormat = TER_HD_LOCK_UI_AUDIO_FORMAT_1;

		SET_STEP(1);
	}
#if (TER_HD_LOCK_UI_SCENARIO&TWO_STREAM_SCENARIO)
	else if( GET_STEP() == 1 )	/* channel 2 */
	{
#if defined(FACTORY_USE_SCART)
		FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_TV, FDI_SCART_VFORMAT_RGB);
		FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_TV, FDI_SCART_AR_16_9);
		FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_VCR, FDI_SCART_VFORMAT_CVBS);
		FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_VCR, FDI_SCART_AR_16_9);
#endif
#if defined(TV_STANDARD_OUTPUT)		
#if (TV_STANDARD_OUTPUT_TYPE == TV_STANDARD_OUTPUT_PAL)
		FDI_VIDEO_SetDisplayStandard(FAI_AV_VIDEO_DEVICE_ID, FDI_VIDEO_RESOLUTION_576I);
#elif (TV_STANDARD_OUTPUT_TYPE == TV_STANDARD_OUTPUT_NTSC)
		FDI_VIDEO_SetDisplayStandard(FAI_AV_VIDEO_DEVICE_ID, FDI_VIDEO_RESOLUTION_480I);
#endif
#else
		FDI_VIDEO_SetDisplayStandard(FAI_AV_VIDEO_DEVICE_ID, FDI_VIDEO_RESOLUTION_720P);
#endif		
#if ((DCONNECTOR_OUTPUT_ENABLE==YES)||(DCONNECTOR_SVIDEO_UI_ENABLE==YES))
		FDI_VIDEO_ControlDPort(0, FDI_VIDEO_RESOLUTION_720P, FDI_VIDEO_ASPECTRATIO_16_9);
#endif

		usVideoPid =	TER_HD_LOCK_UI_VIDEO_PID_2;
		usAudioPid =	TER_HD_LOCK_UI_AUDIO_PID_2;
		usPcrPid = TER_HD_LOCK_UI_PCR_PID_2;
		ucVideoFormat = TER_HD_LOCK_UI_VIDEO_FORMAT_2;
		ucAudioFormat = TER_HD_LOCK_UI_AUDIO_FORMAT_2;
		SET_STEP(2);
	}
#endif
	else						/* stop channel */
	{
		INTRN_FAI_AV_DefaultVideoSetting();
	
		FDI_OSD_Show();
		SET_STEP(0);

		return FAI_NO_ERR;
	}

	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "HD A/V Play", RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Testing", RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);

	/* Channel Locking */
	tTerTuneParam.ulFrequency = TER_HD_LOCK_UI_FREQUENCY;
	tTerTuneParam.etBandWidth = TER_HD_LOCK_UI_BANDWIDTH;
	tTerTuneParam.etCodeRate = FDI_CH_TCODERATE_AUTO;
	tTerTuneParam.etGuardInterval = FDI_CH_GUARDINTERVAL_AUTO;
	tTerTuneParam.etHierarchy = FDI_CH_HIERARCHY_AUTO;
	tTerTuneParam.etModulation = TER_HD_LOCK_UI_MODULATION;
	tTerTuneParam.etOffset = FDI_CH_OFFSET_AUTO;
	tTerTuneParam.etTransMode = FDI_CH_TRANSMODE_AUTO;
	tTerTuneParam.etStream = FDI_CH_STREAM_HP;
	tTerTuneParam.etTerSystem = TER_HD_LOCK_UI_SYSTEM;

	if( FDI_CHANNEL_T_RequestLock(ulTunerId, &tTerTuneParam) != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_PlaySD_UI] : FDI_CHANNEL_C_RequestLock Error!!\n"));
		goto TEST_FAIL;
	}

	/* Locking check */
	for( i = 0; i < FAI_AV_NUM_OF_LOCK_RETRY; i++ )
	{
		FDI_CHANNEL_T_IsLocked(ulTunerId, &bIsLocked);
		if(bIsLocked == TRUE)
		{
			break;
		}
		FVK_TASK_Sleep(FAI_AV_LOCK_RETRY_DELAY);
	}
	if( bIsLocked == FALSE )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_PlaySD_UI] : Channel Unlock!!\n"));
		goto TEST_FAIL;
	}

	/* Channel lock succeed, so display it */
	sprintf(szDisplayStr, "T%d_HD_Lock", ulTunerId);
	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, szDisplayStr, RGB_COLOR_BLACK);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);
	(ulTunerId==0) ? FDI_PANEL_Display(FAI_AV_T0_PLAYHD_PASS_STR) : FDI_PANEL_Display(FAI_AV_T1_PLAYHD_PASS_STR);

	FVK_TASK_Sleep(FAI_AV_PLAY_DELAY);

	/* Convert video format */
	switch(ucVideoFormat)
	{
		case VIDEO_FORMAT_MPEG2:
			eVideoFormat = FDI_VIDEO_FORMAT_MPEG2;
			break;
		case VIDEO_FORMAT_MPEG4:
			eVideoFormat = FDI_VIDEO_FORMAT_MPEG4Part2;
			break;
		case VIDEO_FORMAT_H_264:
			eVideoFormat = FDI_VIDEO_FORMAT_H264;
			break;
		case VIDEO_FORMAT_H_265:
			eVideoFormat = FDI_VIDEO_FORMAT_H265;
			break;						
		default:
			eVideoFormat = FDI_VIDEO_FORMAT_MPEG2;
			break;
	}

	/* Convert audio format */
	switch(ucAudioFormat)
	{
		case AUDIO_FORMAT_MPEG:
			eAudioFormat = FDI_AUDIO_FORMAT_MPEG;
			break;
		case AUDIO_FORMAT_PCM:
			eAudioFormat = FDI_AUDIO_FORMAT_PCM;
			break;
		case AUDIO_FORMAT_AC3:
			eAudioFormat = FDI_AUDIO_FORMAT_AC3;
			break;
		case AUDIO_FORMAT_AAC:
			eAudioFormat = FDI_AUDIO_FORMAT_AAC;
			break;
		case AUDIO_FORMAT_MP3:
			eAudioFormat = FDI_AUDIO_FORMAT_MP3;
			break;
		default:
			eAudioFormat = FDI_AUDIO_FORMAT_MPEG;
			break;
	}
	if( INTRN_FAI_AV_Play(ulTunerId, eVideoFormat, eAudioFormat, usVideoPid, usAudioPid, usPcrPid, FDI_DEMUX_CI_BYPASS) != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_T_PlayHD_UI] : INTRN_FAI_AV_Play(FDI_DEMUX_CI_BYPASS) Error!!\n"));
		goto TEST_FAIL;
	}
	return FAI_NO_ERR;

TEST_FAIL:
	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_RED);
	sprintf(szDisplayStr, "T%d_HD_Unlock", ulTunerId);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, szDisplayStr, RGB_COLOR_RED);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);
	(ulTunerId==0) ? FDI_PANEL_Display(FAI_AV_T0_PLAYHD_FAIL_STR) : FDI_PANEL_Display(FAI_AV_T1_PLAYHD_FAIL_STR);

	FDI_OSD_Show();
	return FAI_ERR;
}
#endif

#if (TER2_HD_LOCK_UI_ENABLE==YES)
FAI_ERR_CODE FAI_AV_T2_PlayHD_UI(void)
{
	FDI_CH_TER_TUNE_PARAM_t tTerTuneParam;
	unsigned short usVideoPid, usAudioPid, usPcrPid;
	unsigned long ulTunerId = 0;
	BOOL bIsLocked;
	int i;
	FDI_AUDIO_FORMAT eAudioFormat;
	FDI_VIDEO_FORMAT eVideoFormat;
	unsigned char aucResult[2];
	unsigned char ucVideoFormat, ucAudioFormat;
	unsigned char szDisplayStr[20];

	/* Select tuner id */
#if (TER_HD_LOCK_UI_SCENARIO&ONE_TUNER_SCENARIO)
	ulTunerId = 0;
#endif
#if (TER_HD_LOCK_UI_SCENARIO&TWO_TUNER_SCENARIO)
	if( GET_STEP() == 0 )		/* channel 1 */
	{
		ulTunerId = 0;
	}
	else if( GET_STEP() == 1 )	/* channel 2 */
	{
		ulTunerId = 1;
	}
#endif

	/* If av is playing, stop it */
	if( g_bIsAvPlaying == TRUE )
	{
		INTRN_FAI_AV_Stop();
		FDI_CHANNEL_T_CancelLock(ulTunerId);
	}

	if( GET_STEP() == 0 )		/* channel 1 */
	{
#if defined(FACTORY_USE_SCART)
		FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_TV, FDI_SCART_VFORMAT_CVBS);
		FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_TV, FDI_SCART_AR_16_9);
		FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_VCR, FDI_SCART_VFORMAT_CVBS);
		FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_VCR, FDI_SCART_AR_16_9);
#endif
#if defined(TV_STANDARD_OUTPUT)		
#if (TV_STANDARD_OUTPUT_TYPE == TV_STANDARD_OUTPUT_PAL)
		FDI_VIDEO_SetDisplayStandard(FAI_AV_VIDEO_DEVICE_ID, FDI_VIDEO_RESOLUTION_576I);
#elif (TV_STANDARD_OUTPUT_TYPE == TV_STANDARD_OUTPUT_NTSC)
		FDI_VIDEO_SetDisplayStandard(FAI_AV_VIDEO_DEVICE_ID, FDI_VIDEO_RESOLUTION_480I);
#endif
#else
		FDI_VIDEO_SetDisplayStandard(FAI_AV_VIDEO_DEVICE_ID, FDI_VIDEO_RESOLUTION_MAX);		
#endif
#if ((DCONNECTOR_OUTPUT_ENABLE==YES)||(DCONNECTOR_SVIDEO_UI_ENABLE==YES))
		FDI_VIDEO_ControlDPort(0, FDI_VIDEO_RESOLUTION_MAX, FDI_VIDEO_ASPECTRATIO_16_9);
#endif

		usVideoPid =	TER2_HD_LOCK_UI_VIDEO_PID_1;
		usAudioPid =	TER2_HD_LOCK_UI_AUDIO_PID_1;
		usPcrPid = TER2_HD_LOCK_UI_PCR_PID_1;
		ucVideoFormat = TER2_HD_LOCK_UI_VIDEO_FORMAT_1;
		ucAudioFormat = TER2_HD_LOCK_UI_AUDIO_FORMAT_1;

		SET_STEP(1);
	}
#if (TER_HD_LOCK_UI_SCENARIO&TWO_STREAM_SCENARIO)
	else if( GET_STEP() == 1 )	/* channel 2 */
	{
#if defined(FACTORY_USE_SCART)
		FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_TV, FDI_SCART_VFORMAT_RGB);
		FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_TV, FDI_SCART_AR_16_9);
		FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_VCR, FDI_SCART_VFORMAT_CVBS);
		FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_VCR, FDI_SCART_AR_16_9);
#endif
#if defined(TV_STANDARD_OUTPUT)		
#if (TV_STANDARD_OUTPUT_TYPE == TV_STANDARD_OUTPUT_PAL)
		FDI_VIDEO_SetDisplayStandard(FAI_AV_VIDEO_DEVICE_ID, FDI_VIDEO_RESOLUTION_576I);
#elif (TV_STANDARD_OUTPUT_TYPE == TV_STANDARD_OUTPUT_NTSC)
		FDI_VIDEO_SetDisplayStandard(FAI_AV_VIDEO_DEVICE_ID, FDI_VIDEO_RESOLUTION_480I);
#endif
#else
		FDI_VIDEO_SetDisplayStandard(FAI_AV_VIDEO_DEVICE_ID, FDI_VIDEO_RESOLUTION_720P);		
#endif
#if ((DCONNECTOR_OUTPUT_ENABLE==YES)||(DCONNECTOR_SVIDEO_UI_ENABLE==YES))
		FDI_VIDEO_ControlDPort(0, FDI_VIDEO_RESOLUTION_720P, FDI_VIDEO_ASPECTRATIO_16_9);
#endif

		usVideoPid =	TER2_HD_LOCK_UI_VIDEO_PID_2;
		usAudioPid =	TER2_HD_LOCK_UI_AUDIO_PID_2;
		usPcrPid = TER2_HD_LOCK_UI_PCR_PID_2;
		ucVideoFormat = TER2_HD_LOCK_UI_VIDEO_FORMAT_2;
		ucAudioFormat = TER2_HD_LOCK_UI_AUDIO_FORMAT_2;
		SET_STEP(2);
	}
#endif
	else						/* stop channel */
	{
		INTRN_FAI_AV_DefaultVideoSetting();
	
		FDI_OSD_Show();
		SET_STEP(0);

		return FAI_NO_ERR;
	}

	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "T2 HD A/V Play", RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Testing", RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);

	/* Channel Locking */
	tTerTuneParam.ulFrequency = TER2_HD_LOCK_UI_FREQUENCY;
	tTerTuneParam.etBandWidth = TER2_HD_LOCK_UI_BANDWIDTH;
	tTerTuneParam.etCodeRate = FDI_CH_TCODERATE_AUTO;
	tTerTuneParam.etGuardInterval = FDI_CH_GUARDINTERVAL_AUTO;
	tTerTuneParam.etHierarchy = FDI_CH_HIERARCHY_AUTO;
	tTerTuneParam.etModulation = TER2_HD_LOCK_UI_MODULATION;
	tTerTuneParam.etOffset = FDI_CH_OFFSET_AUTO;
	tTerTuneParam.etTransMode = FDI_CH_TRANSMODE_AUTO;
	tTerTuneParam.etStream = FDI_CH_STREAM_HP;
	tTerTuneParam.etTerSystem = TER2_HD_LOCK_UI_SYSTEM;

	if( FDI_CHANNEL_T_RequestLock(ulTunerId, &tTerTuneParam) != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_PlaySD_UI] : FDI_CHANNEL_C_RequestLock Error!!\n"));
		goto TEST_FAIL;
	}

	/* Locking check */
	for( i = 0; i < FAI_AV_NUM_OF_LOCK_RETRY; i++ )
	{
		FDI_CHANNEL_T_IsLocked(ulTunerId, &bIsLocked);
		if(bIsLocked == TRUE)
		{
			break;
		}
		FVK_TASK_Sleep(FAI_AV_LOCK_RETRY_DELAY);
	}
	if( bIsLocked == FALSE )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_PlaySD_UI] : Channel Unlock!!\n"));
		goto TEST_FAIL;
	}

	/* Channel lock succeed, so display it */
	sprintf(szDisplayStr, "T2-%d_HD_Lock", ulTunerId);
	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, szDisplayStr, RGB_COLOR_BLACK);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);
	(ulTunerId==0) ? FDI_PANEL_Display(FAI_AV_T0_PLAYHD_PASS_STR) : FDI_PANEL_Display(FAI_AV_T1_PLAYHD_PASS_STR);

	FVK_TASK_Sleep(FAI_AV_PLAY_DELAY);

	/* Convert video format */
	switch(ucVideoFormat)
	{
		case VIDEO_FORMAT_MPEG2:
			eVideoFormat = FDI_VIDEO_FORMAT_MPEG2;
			break;
		case VIDEO_FORMAT_MPEG4:
			eVideoFormat = FDI_VIDEO_FORMAT_MPEG4Part2;
			break;
		case VIDEO_FORMAT_H_264:
			eVideoFormat = FDI_VIDEO_FORMAT_H264;
			break;
		default:
			eVideoFormat = FDI_VIDEO_FORMAT_MPEG2;
			break;
	}

	/* Convert audio format */
	switch(ucAudioFormat)
	{
		case AUDIO_FORMAT_MPEG:
			eAudioFormat = FDI_AUDIO_FORMAT_MPEG;
			break;
		case AUDIO_FORMAT_PCM:
			eAudioFormat = FDI_AUDIO_FORMAT_PCM;
			break;
		case AUDIO_FORMAT_AC3:
			eAudioFormat = FDI_AUDIO_FORMAT_AC3;
			break;
		case AUDIO_FORMAT_AAC:
			eAudioFormat = FDI_AUDIO_FORMAT_AAC;
			break;
		case AUDIO_FORMAT_MP3:
			eAudioFormat = FDI_AUDIO_FORMAT_MP3;
			break;
		default:
			eAudioFormat = FDI_AUDIO_FORMAT_MPEG;
			break;
	}
	if( INTRN_FAI_AV_Play(ulTunerId, eVideoFormat, eAudioFormat, usVideoPid, usAudioPid, usPcrPid, FDI_DEMUX_CI_BYPASS) != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_T_PlayHD_UI] : INTRN_FAI_AV_Play(FDI_DEMUX_CI_BYPASS) Error!!\n"));
		goto TEST_FAIL;
	}
	return FAI_NO_ERR;

TEST_FAIL:
	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_RED);
	sprintf(szDisplayStr, "T%d_HD_Unlock", ulTunerId);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, szDisplayStr, RGB_COLOR_RED);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);
	(ulTunerId==0) ? FDI_PANEL_Display(FAI_AV_T0_PLAYHD_FAIL_STR) : FDI_PANEL_Display(FAI_AV_T1_PLAYHD_FAIL_STR);

	FDI_OSD_Show();
	return FAI_ERR;
}
#endif

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (TER_CI_LOCK_UI_ENABLE==YES)
FAI_ERR_CODE FAI_AV_T_CI_AvTest_UI(void)
{
	FDI_CH_TER_TUNE_PARAM_t tTerTuneParam;
	unsigned short usVideoPid, usAudioPid, usPcrPid;
	unsigned long ulTunerId;
	BOOL bIsLocked;
	int i;
	FDI_AUDIO_FORMAT eAudioFormat;
	FDI_VIDEO_FORMAT eVideoFormat;
	unsigned char aucResult[2];
	unsigned char ucVideoFormat, ucAudioFormat;

	/* Select tuner id */
	ulTunerId = 0;

	/* If av is playing, stop it */
	if( g_bIsAvPlaying == TRUE )
	{
		INTRN_FAI_AV_Stop();
		FDI_CHANNEL_T_CancelLock(ulTunerId);
	}

	if( GET_STEP() == 0 )		/* channel 1 */
	{
#if defined(FACTORY_USE_SCART)
		FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_TV, FDI_SCART_VFORMAT_RGB);
		FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_TV, FDI_SCART_AR_4_3);
		FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_VCR, FDI_SCART_VFORMAT_CVBS);
		FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_VCR, FDI_SCART_AR_4_3);
#endif
#if (TV_STANDARD_OUTPUT_TYPE == TV_STANDARD_OUTPUT_PAL)
		FDI_VIDEO_SetDisplayStandard(FAI_AV_VIDEO_DEVICE_ID, FDI_VIDEO_RESOLUTION_576I);
#elif (TV_STANDARD_OUTPUT_TYPE == TV_STANDARD_OUTPUT_NTSC)
		FDI_VIDEO_SetDisplayStandard(FAI_AV_VIDEO_DEVICE_ID, FDI_VIDEO_RESOLUTION_480I);
#if ((DCONNECTOR_OUTPUT_ENABLE==YES)||(DCONNECTOR_SVIDEO_UI_ENABLE==YES))
		FDI_VIDEO_ControlDPort(0, FDI_VIDEO_RESOLUTION_480I, FDI_VIDEO_ASPECTRATIO_16_9);
#endif
#endif

		usVideoPid 		= TER_CI_LOCK_UI_VIDEO_PID;
		usAudioPid 		= TER_CI_LOCK_UI_AUDIO_PID;
		usPcrPid 		= TER_CI_LOCK_UI_PCR_PID;
		ucVideoFormat 	= TER_CI_LOCK_UI_VIDEO_FORMAT;
		ucAudioFormat 	= TER_CI_LOCK_UI_AUDIO_FORMAT;

		SET_STEP(1);
	}
	else 						/* stop channel */
	{
		INTRN_FAI_AV_DefaultVideoSetting();

		FDI_OSD_Show();
		SET_STEP(0);

		return FAI_NO_ERR;
	}

	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "CI TS locking", RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Testing", RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);

	/* Channel Locking */
	tTerTuneParam.ulFrequency 	= TER_CI_LOCK_UI_FREQUENCY;
	tTerTuneParam.etBandWidth 	= TER_CI_LOCK_UI_BANDWIDTH;
	tTerTuneParam.etCodeRate 		= FDI_CH_TCODERATE_AUTO;
	tTerTuneParam.etGuardInterval 	= FDI_CH_GUARDINTERVAL_AUTO;
	tTerTuneParam.etHierarchy 		= FDI_CH_HIERARCHY_AUTO;
	tTerTuneParam.etModulation 	= TER_CI_LOCK_UI_MODULATION;
	tTerTuneParam.etOffset			= FDI_CH_OFFSET_AUTO;
	tTerTuneParam.etTransMode 	= FDI_CH_TRANSMODE_AUTO;
	tTerTuneParam.etStream 		= FDI_CH_STREAM_HP;
	tTerTuneParam.etTerSystem 	= TER_CI_LOCK_UI_SYSTEM;

	if( FDI_CHANNEL_T_RequestLock(ulTunerId, &tTerTuneParam) != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_PlaySD_UI] : FDI_CHANNEL_C_RequestLock Error!!\n"));
		goto TEST_FAIL;
	}

	/* Locking check */
	for( i = 0; i < FAI_AV_NUM_OF_LOCK_RETRY; i++ )
	{
		FDI_CHANNEL_T_IsLocked(ulTunerId, &bIsLocked);
		if(bIsLocked == TRUE)
		{
			break;
		}
		VK_TASK_Sleep(FAI_AV_LOCK_RETRY_DELAY);
	}


	if( bIsLocked == FALSE )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_PlaySD_UI] : Channel Unlock!!\n"));
		goto TEST_FAIL;
	}

	/* Channel lock succeed, so display it */
	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "CI_Lock", RGB_COLOR_BLACK);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);
	FDI_PANEL_Display(FAI_CI_AV_LOCK_STR);

	VK_TASK_Sleep(FAI_AV_PLAY_DELAY);

	/* Convert video format */
	switch(ucVideoFormat)
	{
		case VIDEO_FORMAT_MPEG2:
			eVideoFormat = FDI_VIDEO_FORMAT_MPEG2;
			break;
		case VIDEO_FORMAT_MPEG4:
			eVideoFormat = FDI_VIDEO_FORMAT_MPEG4Part2;
			break;
		case VIDEO_FORMAT_H_264:
			eVideoFormat = FDI_VIDEO_FORMAT_H264;
			break;
		case VIDEO_FORMAT_H_265:
			eVideoFormat = FDI_VIDEO_FORMAT_H265;
			break;						
		default:
			eVideoFormat = FDI_VIDEO_FORMAT_MPEG2;
			break;
	}

	/* Convert audio format */
	switch(ucAudioFormat)
	{
		case AUDIO_FORMAT_MPEG:
			eAudioFormat = FDI_AUDIO_FORMAT_MPEG;
			break;
		case AUDIO_FORMAT_PCM:
			eAudioFormat = FDI_AUDIO_FORMAT_PCM;
			break;
		case AUDIO_FORMAT_AC3:
			eAudioFormat = FDI_AUDIO_FORMAT_AC3;
			break;
		case AUDIO_FORMAT_AAC:
			eAudioFormat = FDI_AUDIO_FORMAT_AAC;
			break;
		case AUDIO_FORMAT_MP3:
			eAudioFormat = FDI_AUDIO_FORMAT_MP3;
			break;
		default:
			eAudioFormat = FDI_AUDIO_FORMAT_MPEG;
			break;
	}
	if( INTRN_FAI_AV_Play(ulTunerId, eVideoFormat, eAudioFormat, usVideoPid, usAudioPid, usPcrPid, FDI_DEMUX_CI_PASSTHROUGH) != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_PlaySD_UI] : INTRN_FAI_AV_Play(FDI_DEMUX_CI_PASSTHROUGH) Error!!\n"));
		goto TEST_FAIL;
	}
	return FAI_NO_ERR;

TEST_FAIL:
	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_RED);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "CI_Unlock", RGB_COLOR_RED);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);
	FDI_PANEL_Display(FAI_CI_AV_UNLOCK_STR);

	FDI_OSD_Show();
	return FAI_ERR;
}
#endif

#if defined(FACTORY_USE_CHANNEL_SAT)
/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE INTRN_FAI_AV_S_QuickPlay(unsigned char ucTunerId, unsigned char ucStreamId)
{
	FDI_CH_SAT_TUNE_PARAM_t tSatTuneParam;
	BOOL bIsLocked;
	int i;
	FDI_AUDIO_FORMAT eAudioFormat;
	FDI_VIDEO_FORMAT eVideoFormat;
	unsigned short usVideoPid, usAudioPid, usPcrPid;
	FDI_DEMUX_CI_PATH_e demuxCIPath = FDI_DEMUX_CI_BYPASS;

	switch(ucStreamId)
	{
		case FAI_AV_QUICK_PLAY_SAT_COLORBAR_SD:
			tSatTuneParam.etAntennaType= FDI_CH_ANT_TYPE_DISEQC;
			tSatTuneParam.ulFrequency = 1084;
			tSatTuneParam.ulSymbolRate = 27500;
			tSatTuneParam.etFecCodeRate = FDI_CH_SCODERATE_5_6;
			tSatTuneParam.etPolarization = FDI_CH_POLAR_HOR;
			tSatTuneParam.etTransSpec = FDI_CH_TRANS_DVBS;
			tSatTuneParam.etDiseqcInput = FDI_CH_DISEQC_INPUT_NONE;
			tSatTuneParam.b22kTone = TRUE;
			tSatTuneParam.etModulation = FDI_CH_PSK_QPSK;
			tSatTuneParam.etPilot = FDI_CH_PILOT_AUTO;
			tSatTuneParam.etRollOff = FDI_CH_ROLL_020;
			tSatTuneParam.etLnbVoltage = FDI_CH_LNB_VOLT_STD;
			tSatTuneParam.etDiseqcVersion = FDI_CH_DISEQC_VER_1_0;
			tSatTuneParam.ulScdUserBandNum = 0;
			tSatTuneParam.ulScdUserBandFreq = 0;

			eVideoFormat = FDI_VIDEO_FORMAT_MPEG2;
			eAudioFormat = FDI_AUDIO_FORMAT_MPEG;
			usVideoPid = 1536;
			usAudioPid = 1537;
			usPcrPid = 1536;
			break;
		case FAI_AV_QUICK_PLAY_SAT_MOVING_SD:
			tSatTuneParam.etAntennaType= FDI_CH_ANT_TYPE_DISEQC;
			tSatTuneParam.ulFrequency = 1084;
			tSatTuneParam.ulSymbolRate = 27500;
			tSatTuneParam.etFecCodeRate = FDI_CH_SCODERATE_5_6;
			tSatTuneParam.etPolarization = FDI_CH_POLAR_HOR;
			tSatTuneParam.etTransSpec = FDI_CH_TRANS_DVBS;
			tSatTuneParam.etDiseqcInput = FDI_CH_DISEQC_INPUT_NONE;
			tSatTuneParam.b22kTone = TRUE;
			tSatTuneParam.etModulation = FDI_CH_PSK_QPSK;
			tSatTuneParam.etPilot = FDI_CH_PILOT_AUTO;
			tSatTuneParam.etRollOff = FDI_CH_ROLL_020;
			tSatTuneParam.etLnbVoltage = FDI_CH_LNB_VOLT_STD;
			tSatTuneParam.etDiseqcVersion = FDI_CH_DISEQC_VER_1_0;
			tSatTuneParam.ulScdUserBandNum = 0;
			tSatTuneParam.ulScdUserBandFreq = 0;

			eVideoFormat = FDI_VIDEO_FORMAT_MPEG2;
			eAudioFormat = FDI_AUDIO_FORMAT_MPEG;
			usVideoPid = 1024;
			usAudioPid = 1025;
			usPcrPid = 1024;
			break;
		case FAI_AV_QUICK_PLAY_SAT_HD:
			tSatTuneParam.etAntennaType= FDI_CH_ANT_TYPE_DISEQC;
			tSatTuneParam.ulFrequency = 1170;
			tSatTuneParam.ulSymbolRate = 27500;
			tSatTuneParam.etFecCodeRate = FDI_CH_S2CODERATE_9_10;
			tSatTuneParam.etPolarization = FDI_CH_POLAR_HOR;
			tSatTuneParam.etTransSpec = FDI_CH_TRANS_DVBS2;
			tSatTuneParam.etDiseqcInput = FDI_CH_DISEQC_INPUT_NONE;
			tSatTuneParam.b22kTone = TRUE;
			tSatTuneParam.etModulation = FDI_CH_PSK_QPSK;
			tSatTuneParam.etPilot = FDI_CH_PILOT_AUTO;
			tSatTuneParam.etRollOff = FDI_CH_ROLL_020;
			tSatTuneParam.etLnbVoltage = FDI_CH_LNB_VOLT_STD;
			tSatTuneParam.etDiseqcVersion = FDI_CH_DISEQC_VER_1_0;
			tSatTuneParam.ulScdUserBandNum = 0;
			tSatTuneParam.ulScdUserBandFreq = 0;

			eVideoFormat = FDI_VIDEO_FORMAT_H264;
			eAudioFormat = FDI_AUDIO_FORMAT_AC3;
			usVideoPid = 1023;
			usAudioPid = 1027;
			usPcrPid = 1023;
			break;
		case FAI_AV_QUICK_PLAY_SAT_TV4_CI:
			break;
		case FAI_AV_QUICK_PLAY_SAT_TRAVEL_CI:
			break;
		case FAI_AV_QUICK_PLAY_SAT_MOVING_SD_CI:
			tSatTuneParam.etAntennaType= FDI_CH_ANT_TYPE_DISEQC;
			tSatTuneParam.ulFrequency = 1084;
			tSatTuneParam.ulSymbolRate = 27500;
			tSatTuneParam.etFecCodeRate = FDI_CH_SCODERATE_5_6;
			tSatTuneParam.etPolarization = FDI_CH_POLAR_HOR;
			tSatTuneParam.etTransSpec = FDI_CH_TRANS_DVBS;
			tSatTuneParam.etDiseqcInput = FDI_CH_DISEQC_INPUT_NONE;
			tSatTuneParam.b22kTone = TRUE;
			tSatTuneParam.etModulation = FDI_CH_PSK_QPSK;
			tSatTuneParam.etPilot = FDI_CH_PILOT_AUTO;
			tSatTuneParam.etRollOff = FDI_CH_ROLL_020;
			tSatTuneParam.etLnbVoltage = FDI_CH_LNB_VOLT_STD;
			tSatTuneParam.etDiseqcVersion = FDI_CH_DISEQC_VER_1_0;
			tSatTuneParam.ulScdUserBandNum = 0;
			tSatTuneParam.ulScdUserBandFreq = 0;

			eVideoFormat = FDI_VIDEO_FORMAT_MPEG2;
			eAudioFormat = FDI_AUDIO_FORMAT_MPEG;
			usVideoPid = 1024;
			usAudioPid = 1025;
			usPcrPid = 1024;

			demuxCIPath = FDI_DEMUX_CI_PASSTHROUGH;		/* CI Passthrough */

			break;

		default:
			FAI_PRINT(FAI_PRT_ERROR, ("[%s] : param error Error!! ucStreamId(%d)\n", __func__, ucStreamId));
			return FAI_ERR;
	}

	/* channel lock */
	if( FDI_CHANNEL_S_RequestLock(ucTunerId, &tSatTuneParam) != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_AV_S_QuickPlay] : FDI_CHANNEL_S_RequestLock Error!!\n"));
		return FAI_ERR;
	}

	/* Locking check */
	for( i = 0; i < FAI_CHANNEL_LOCK_CHECK_RETRY_NUM; i++ )
	{
		FDI_CHANNEL_S_IsLocked(ucTunerId, &bIsLocked);
		if(bIsLocked == TRUE)
		{
			break;
		}
		FVK_TASK_Sleep(FAI_CHANNEL_LOCK_RETRY_DELAY);
	}

	if( INTRN_FAI_AV_Play(ucTunerId, eVideoFormat, eAudioFormat, usVideoPid, usAudioPid, usPcrPid, demuxCIPath) != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_AV_S_QuickPlay] : INTRN_FAI_AV_Play Error!!\n"));
		return FAI_ERR;
	}

	return FAI_NO_ERR;
}
#endif

#if defined(FACTORY_USE_CHANNEL_CAB)
/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE INTRN_FAI_AV_C_QuickPlay(unsigned char ucTunerId, unsigned char ucStreamId)
{
	FDI_CH_CAB_TUNE_PARAM_t tCabTuneParam;
	BOOL bIsLocked;
	int i;
	FDI_AUDIO_FORMAT eAudioFormat;
	FDI_VIDEO_FORMAT eVideoFormat;
	unsigned short usVideoPid, usAudioPid, usPcrPid;
	FDI_DEMUX_CI_PATH_e demuxCIPath = FDI_DEMUX_CI_BYPASS;

	switch(ucStreamId)
	{
		case FAI_AV_QUICK_PLAY_CAB_COLORBAR_SD:

			tCabTuneParam.ulFrequency = 506 * 1000;
			tCabTuneParam.ulSymbolRate = 6547;
			tCabTuneParam.etModulation = FDI_CH_CMOD_128QAM;
			tCabTuneParam.etSpectrum = FDI_CH_INVERSION_AUTO;

			eVideoFormat = FDI_VIDEO_FORMAT_MPEG2;
			eAudioFormat = FDI_AUDIO_FORMAT_MPEG;
			usVideoPid = 1536;
			usAudioPid = 1537;
			usPcrPid = 1536;
			break;
		case FAI_AV_QUICK_PLAY_CAB_MOVING_SD:

			tCabTuneParam.ulFrequency = 506 * 1000;
			tCabTuneParam.ulSymbolRate = 6547;
			tCabTuneParam.etModulation = FDI_CH_CMOD_128QAM;
			tCabTuneParam.etSpectrum = FDI_CH_INVERSION_AUTO;

			eVideoFormat = FDI_VIDEO_FORMAT_MPEG2;
			eAudioFormat = FDI_AUDIO_FORMAT_MPEG;
			usVideoPid = 1024;
			usAudioPid = 1025;
			usPcrPid = 1024;
			break;
		case FAI_AV_QUICK_PLAY_CAB_HD:

			tCabTuneParam.ulFrequency = 490 * 1000;
			tCabTuneParam.ulSymbolRate = 6648;
			tCabTuneParam.etModulation = FDI_CH_CMOD_256QAM;
			tCabTuneParam.etSpectrum = FDI_CH_INVERSION_AUTO;

			eVideoFormat = FDI_VIDEO_FORMAT_H264;
			eAudioFormat = FDI_AUDIO_FORMAT_AC3;
			usVideoPid = 1023;
			usAudioPid = 1027;
			usPcrPid = 1023;
			break;
		case FAI_AV_QUICK_PLAY_SAT_TV4_CI:
			break;
		case FAI_AV_QUICK_PLAY_SAT_TRAVEL_CI:
			break;
		case FAI_AV_QUICK_PLAY_CAB_MOVING_SD_CI:

			tCabTuneParam.ulFrequency = 506 * 1000;
			tCabTuneParam.ulSymbolRate = 6547;
			tCabTuneParam.etModulation = FDI_CH_CMOD_128QAM;
			tCabTuneParam.etSpectrum = FDI_CH_INVERSION_AUTO;

			eVideoFormat = FDI_VIDEO_FORMAT_MPEG2;
			eAudioFormat = FDI_AUDIO_FORMAT_MPEG;
			usVideoPid = 1024;
			usAudioPid = 1025;
			usPcrPid = 1024;

			demuxCIPath = FDI_DEMUX_CI_PASSTHROUGH;		/* CI Passthrough */
			
			break;
			
		default:
			FAI_PRINT(FAI_PRT_ERROR, ("[%s] : param error Error!! ucStreamId(%d)\n", __func__, ucStreamId));
			return FAI_ERR;
	}

	/* channel lock */
	if( FDI_CHANNEL_C_RequestLock(ucTunerId, &tCabTuneParam) != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_AV_CabQuickPlay] : FDI_CHANNEL_C_RequestLock Error!!\n"));
		return FAI_ERR;
	}

	/* Locking check */
	for( i = 0; i < FAI_CHANNEL_LOCK_CHECK_RETRY_NUM; i++ )
	{
		FDI_CHANNEL_C_IsLocked(ucTunerId, &bIsLocked);
		if(bIsLocked == TRUE)
		{
			break;
		}
		FVK_TASK_Sleep(FAI_CHANNEL_LOCK_RETRY_DELAY);
	}
	if( bIsLocked == FALSE )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Locking Fail~!\n",__func__));
		return FAI_ERR;
	}

	if( INTRN_FAI_AV_Play(ucTunerId, eVideoFormat, eAudioFormat, usVideoPid, usAudioPid, usPcrPid, demuxCIPath) != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_AV_SatQuickPlay] : INTRN_FAI_AV_Play Error!!\n"));
		return FAI_ERR;
	}

	return FAI_NO_ERR;
}
#endif

#if defined(FACTORY_USE_CHANNEL_TER)
/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE INTRN_FAI_AV_T_QuickPlay(unsigned char ucTunerId, unsigned char ucStreamId)
{
	FDI_CH_TER_TUNE_PARAM_t tTerTuneParam;
	BOOL bIsLocked;
	int i;
	FDI_AUDIO_FORMAT eAudioFormat;
	FDI_VIDEO_FORMAT eVideoFormat;
	unsigned char ucVideoFormat, ucAudioFormat;
	unsigned short usVideoPid, usAudioPid, usPcrPid;

	FAI_PRINT(FAI_PRT_DBGINFO, ("[INTRN_FAI_AV_T_QuickPlay]++ : tunerID %d, StreamID %d\n", ucTunerId, ucStreamId));

	switch(ucStreamId)
	{
		case FAI_AV_QUICK_PLAY_TER_COLORBAR_SD:

			tTerTuneParam.ulFrequency = TER_SD_LOCK_UI_FREQUENCY;
			tTerTuneParam.etBandWidth = TER_SD_LOCK_UI_BANDWIDTH;
			tTerTuneParam.etCodeRate = FDI_CH_TCODERATE_AUTO;
			tTerTuneParam.etGuardInterval = FDI_CH_GUARDINTERVAL_AUTO;
			tTerTuneParam.etHierarchy = FDI_CH_HIERARCHY_AUTO;
			tTerTuneParam.etModulation = TER_SD_LOCK_UI_MODULATION;
			tTerTuneParam.etOffset = FDI_CH_OFFSET_AUTO;
			tTerTuneParam.etTransMode = FDI_CH_TRANSMODE_AUTO;
			tTerTuneParam.etStream = FDI_CH_STREAM_HP;
			tTerTuneParam.etTerSystem = TER_SD_LOCK_UI_SYSTEM;

			usVideoPid = TER_SD_LOCK_UI_VIDEO_PID_1;
			usAudioPid = TER_SD_LOCK_UI_AUDIO_PID_1;
			usPcrPid = TER_SD_LOCK_UI_PCR_PID_1;
			ucVideoFormat = TER_SD_LOCK_UI_VIDEO_FORMAT_1;
			ucAudioFormat = TER_SD_LOCK_UI_AUDIO_FORMAT_1;
			break;
		case FAI_AV_QUICK_PLAY_TER_MOVING_SD:

			tTerTuneParam.ulFrequency = TER_SD_LOCK_UI_FREQUENCY;
			tTerTuneParam.etBandWidth = TER_SD_LOCK_UI_BANDWIDTH;
			tTerTuneParam.etCodeRate = FDI_CH_TCODERATE_AUTO;
			tTerTuneParam.etGuardInterval = FDI_CH_GUARDINTERVAL_AUTO;
			tTerTuneParam.etHierarchy = FDI_CH_HIERARCHY_AUTO;
			tTerTuneParam.etModulation = TER_SD_LOCK_UI_MODULATION;
			tTerTuneParam.etOffset = FDI_CH_OFFSET_AUTO;
			tTerTuneParam.etTransMode = FDI_CH_TRANSMODE_AUTO;
			tTerTuneParam.etStream = FDI_CH_STREAM_HP;
			tTerTuneParam.etTerSystem = TER_SD_LOCK_UI_SYSTEM;

			usVideoPid = TER_SD_LOCK_UI_VIDEO_PID_2;
			usAudioPid = TER_SD_LOCK_UI_AUDIO_PID_2;
			usPcrPid = TER_SD_LOCK_UI_PCR_PID_2;
			ucVideoFormat = TER_SD_LOCK_UI_VIDEO_FORMAT_2;
			ucAudioFormat = TER_SD_LOCK_UI_AUDIO_FORMAT_2;
			break;
		case FAI_AV_QUICK_PLAY_TER_HD:

			tTerTuneParam.ulFrequency = TER_HD_LOCK_UI_FREQUENCY;
			tTerTuneParam.etBandWidth = TER_HD_LOCK_UI_BANDWIDTH;
			tTerTuneParam.etCodeRate = FDI_CH_TCODERATE_7_8;
			tTerTuneParam.etGuardInterval = FDI_CH_GUARDINTERVAL_1_32;
			tTerTuneParam.etHierarchy = FDI_CH_HIERARCHY_AUTO;
			tTerTuneParam.etModulation = TER_HD_LOCK_UI_MODULATION;
			tTerTuneParam.etOffset = FDI_CH_OFFSET_AUTO;
			tTerTuneParam.etTransMode = FDI_CH_TRANSMODE_8K;
			tTerTuneParam.etStream = FDI_CH_STREAM_HP;
			tTerTuneParam.etTerSystem = TER_HD_LOCK_UI_SYSTEM;

			usVideoPid = TER_HD_LOCK_UI_VIDEO_PID_1;
			usAudioPid = TER_HD_LOCK_UI_AUDIO_PID_1;
			usPcrPid = TER_HD_LOCK_UI_PCR_PID_1;
			ucVideoFormat = TER_HD_LOCK_UI_VIDEO_FORMAT_1;
			ucAudioFormat = TER_HD_LOCK_UI_AUDIO_FORMAT_1;
			break;
#if defined(FACTORY_USE_CHANNEL_TER_T2) && (FACTORY_USE_CHANNEL_TER_T2==YES)
		case FAI_AV_QUICK_PLAY_TER_HD_DVBT2:
			tTerTuneParam.ulFrequency = TER2_HD_LOCK_UI_FREQUENCY;
			tTerTuneParam.etBandWidth = TER2_HD_LOCK_UI_BANDWIDTH;
			tTerTuneParam.etCodeRate = FDI_CH_TCODERATE_7_8;
			tTerTuneParam.etGuardInterval = FDI_CH_GUARDINTERVAL_1_32;
			tTerTuneParam.etHierarchy = FDI_CH_HIERARCHY_AUTO;
			tTerTuneParam.etModulation = TER2_HD_LOCK_UI_MODULATION;
			tTerTuneParam.etOffset = FDI_CH_OFFSET_AUTO;
			tTerTuneParam.etTransMode = FDI_CH_TRANSMODE_8K;
			tTerTuneParam.etStream = FDI_CH_STREAM_HP;
			tTerTuneParam.etTerSystem = TER2_HD_LOCK_UI_SYSTEM;

			usVideoPid = TER2_HD_LOCK_UI_VIDEO_PID_1;
			usAudioPid = TER2_HD_LOCK_UI_AUDIO_PID_1;
			usPcrPid = TER2_HD_LOCK_UI_PCR_PID_1;
			ucVideoFormat = TER2_HD_LOCK_UI_VIDEO_FORMAT_1;
			ucAudioFormat = TER2_HD_LOCK_UI_AUDIO_FORMAT_1;
			break;
#endif
		default:
			FAI_PRINT(FAI_PRT_ERROR, ("[%s] : param error Error!! ucStreamId(%d)\n", __func__, ucStreamId));
			return FAI_ERR;

	}

	/* channel lock */
	if( FDI_CHANNEL_T_RequestLock(ucTunerId, &tTerTuneParam) != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_AV_CabQuickPlay] : FDI_CHANNEL_C_RequestLock Error!!\n"));
		return FAI_ERR;
	}

	/* Locking check */
	for( i = 0; i < FAI_CHANNEL_LOCK_CHECK_RETRY_NUM; i++ )
	{
		FDI_CHANNEL_T_IsLocked(ucTunerId, &bIsLocked);
		if(bIsLocked == TRUE)
		{
			break;
		}
		FVK_TASK_Sleep(FAI_CHANNEL_LOCK_RETRY_DELAY);
	}
	if( bIsLocked == FALSE )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Locking Fail~!\n",__func__));
		return FAI_ERR;
	}

	/* Convert video format */
	switch(ucVideoFormat)
	{
		case VIDEO_FORMAT_MPEG2:
			eVideoFormat = FDI_VIDEO_FORMAT_MPEG2;
			break;
		case VIDEO_FORMAT_MPEG4:
			eVideoFormat = FDI_VIDEO_FORMAT_MPEG4Part2;
			break;
		case VIDEO_FORMAT_H_264:
			eVideoFormat = FDI_VIDEO_FORMAT_H264;
			break;
		case VIDEO_FORMAT_H_265:
			eVideoFormat = FDI_VIDEO_FORMAT_H265;
			break;						
		default:
			eVideoFormat = FDI_VIDEO_FORMAT_MPEG2;
			break;
	}

	/* Convert audio format */
	switch(ucAudioFormat)
	{
		case AUDIO_FORMAT_MPEG:
			eAudioFormat = FDI_AUDIO_FORMAT_MPEG;
			break;
		case AUDIO_FORMAT_PCM:
			eAudioFormat = FDI_AUDIO_FORMAT_PCM;
			break;
		case AUDIO_FORMAT_AC3:
			eAudioFormat = FDI_AUDIO_FORMAT_AC3;
			break;
		case AUDIO_FORMAT_AAC:
			eAudioFormat = FDI_AUDIO_FORMAT_AAC;
			break;
		case AUDIO_FORMAT_MP3:
			eAudioFormat = FDI_AUDIO_FORMAT_MP3;
			break;
		default:
			eAudioFormat = FDI_AUDIO_FORMAT_MPEG;
			break;
	}

	if( INTRN_FAI_AV_Play(ucTunerId, eVideoFormat, eAudioFormat, usVideoPid, usAudioPid, usPcrPid, FDI_DEMUX_CI_BYPASS) != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_AV_SatQuickPlay] : INTRN_FAI_AV_Play Error!!\n"));
		return FAI_ERR;
	}
	return FAI_NO_ERR;
}
#endif



/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE INTRN_FAI_AV_DefaultVideoSetting(void)
{

#if defined(FACTORY_USE_SCART)
	FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_TV, FDI_SCART_VFORMAT_RGB);
	FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_TV, FDI_SCART_AR_4_3);
	FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_VCR, FDI_SCART_VFORMAT_CVBS);
	FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_VCR, FDI_SCART_AR_4_3);
#endif
#if (TV_STANDARD_OUTPUT_TYPE == TV_STANDARD_OUTPUT_PAL)
	FDI_VIDEO_SetDisplayStandard(FAI_AV_VIDEO_DEVICE_ID, FDI_VIDEO_RESOLUTION_576I);
#elif (TV_STANDARD_OUTPUT_TYPE == TV_STANDARD_OUTPUT_NTSC)
	FDI_VIDEO_SetDisplayStandard(FAI_AV_VIDEO_DEVICE_ID, FDI_VIDEO_RESOLUTION_480I);
#if ((DCONNECTOR_OUTPUT_ENABLE==YES)||(DCONNECTOR_SVIDEO_UI_ENABLE==YES))
	FDI_VIDEO_ControlDPort(0, FDI_VIDEO_RESOLUTION_480I, FDI_VIDEO_ASPECTRATIO_16_9);
#endif
#endif
	return FAI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE INTRN_FAI_AV_BeepOnOff(BOOL bOnOff)
{
	if(g_bIsBeepPlaying != bOnOff)
	{
		/* 1. beep tone */
		FDI_AUDIO_SetBeepSound(FAI_AV_AUDIO_DEVICE_ID, bOnOff);

		/* 2. Color Bar */
#if defined(COLORBAR_BEEP_ONOFF_UI_SOURCE) && (COLORBAR_BEEP_ONOFF_UI_SOURCE==COLORBAR_SOURCE_NORMAL)
		FDI_VIDEO_DisplayColorbar(FAI_AV_VIDEO_DEVICE_ID, bOnOff);
#endif
#if defined(COLORBAR_BEEP_ONOFF_UI_SOURCE) && (COLORBAR_BEEP_ONOFF_UI_SOURCE==COLORBAR_SOURCE_OSD)
		FDI_OSD_DisplayColorbar(bOnOff);
#endif

		if( bOnOff == TRUE )
		{
	       	FDI_AUDIO_SetMute(FAI_AV_AUDIO_DEVICE_ID, FALSE);
			FDI_AUDIO_SetVolume(FAI_AV_AUDIO_DEVICE_ID, 100);
		}
	}
	g_bIsBeepPlaying = bOnOff;

	return FAI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if defined(AVPLAY_PLAY_NEW)
FAI_ERR_CODE INTRN_FAI_AV_Play(unsigned long ulTunerID, FDI_VIDEO_FORMAT eVideoFormat, FDI_AUDIO_FORMAT eAudioFormat,
								unsigned short usVideoPid, unsigned short usAudioPid, unsigned short usPcrPid, FDI_DEMUX_CI_PATH_e eCiPath)
{
	FDI_ERR_CODE eFdiErr;


	/* Check AV or BEEP already playing  */
	if( g_bIsBeepPlaying == TRUE )
	{
		INTRN_FAI_AV_BeepOnOff(FALSE);
	}

	eFdiErr = FDI_DEMUX_SetPath(FAI_AV_DEMUX_DEVICE_ID, ulTunerID, eCiPath);
	if(eFdiErr != FDI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_PlaySD_UI] : FDI_DEMUX_SetPath Error!!\n"));
		if (eCiPath == FDI_DEMUX_CI_PASSTHROUGH)
		{
			FDI_DEMUX_ReleasePath(FAI_AV_DEMUX_DEVICE_ID);
			return eFdiErr;
		}
	}

	eFdiErr = FDI_DEMUX_StartPID(FAI_AV_DEMUX_DEVICE_ID, FDI_DEMUX_CHANNELTYPE_PCR, usPcrPid, &ulPcrChannelId);
	if(eFdiErr != FDI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_PlaySD_UI] : FDI_DEMUX_StartPID Error!!\n"));
		return FDI_ERR;
	}
	eFdiErr = FDI_DEMUX_StartPID(FAI_AV_DEMUX_DEVICE_ID, FDI_DEMUX_CHANNELTYPE_VIDEO, usVideoPid, &ulVideoChannelId);
	if(eFdiErr != FDI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_PlaySD_UI] : FDI_DEMUX_StartPID Error!!\n"));
		return FDI_ERR;
	}
	eFdiErr = FDI_DEMUX_StartPID(FAI_AV_DEMUX_DEVICE_ID, FDI_DEMUX_CHANNELTYPE_AUDIO_MAIN, usAudioPid, &ulAudioChannelId);
	if(eFdiErr != FDI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_PlaySD_UI] : FDI_DEMUX_StartPID Error!!\n"));
		return FDI_ERR;
	}
	eFdiErr = FDI_AUDIO_Start(FAI_AV_AUDIO_DEVICE_ID, eAudioFormat);
	if(eFdiErr != FDI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_PlaySD_UI] : FDI_AUDIO_Start Error!!\n"));
		return FDI_ERR;
	}
	eFdiErr = FDI_VIDEO_Start(FAI_AV_VIDEO_DEVICE_ID, eVideoFormat);
	if(eFdiErr != FDI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_PlaySD_UI] : FDI_VIDEO_Start Error!!\n"));
		return FDI_ERR;
	}
	eFdiErr = FDI_VIDEO_Show(FAI_AV_VIDEO_DEVICE_ID, TRUE);
	if(eFdiErr != FDI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_PlaySD_UI] : FDI_VIDEO_Show Error!!\n"));
		return FDI_ERR;
	}
	eFdiErr = FDI_AUDIO_SetMute(FAI_AV_AUDIO_DEVICE_ID, FALSE);
	if(eFdiErr != FDI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_PlaySD_UI] : FDI_AUDIO_SetMute Error!!\n"));
		return FDI_ERR;
	}
	eFdiErr = FDI_AUDIO_SetVolume(FAI_AV_AUDIO_DEVICE_ID, 100);
	if(eFdiErr != FDI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_PlaySD_UI] : FDI_AUDIO_SetVolume Error!!\n"));
		return FDI_ERR;
	}
	eFdiErr = FDI_OSD_Hide();
	if(eFdiErr != FDI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_PlaySD_UI] : FDI_OSD_Hide Error!!\n"));
		return FDI_ERR;
	}
	g_bIsAvPlaying = TRUE;

	return FDI_NO_ERR;
}
#else
FAI_ERR_CODE INTRN_FAI_AV_Play(unsigned long ulTunerID, FDI_VIDEO_FORMAT eVideoFormat, FDI_AUDIO_FORMAT eAudioFormat,
								unsigned short usVideoPid, unsigned short usAudioPid, unsigned short usPcrPid, FDI_DEMUX_CI_PATH_e eCiPath)
{
	FDI_ERR_CODE eFdiErr;


	/* Check AV or BEEP already playing  */
	if( g_bIsBeepPlaying == TRUE )
	{
		INTRN_FAI_AV_BeepOnOff(FALSE);
	}

	eFdiErr = FDI_DEMUX_SetPath(FAI_AV_DEMUX_DEVICE_ID, ulTunerID, eCiPath);
	if(eFdiErr != FDI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_PlaySD_UI] : FDI_DEMUX_SetPath Error!!\n"));
		if (eCiPath == FDI_DEMUX_CI_PASSTHROUGH)
		{
			FDI_DEMUX_ReleasePath(FAI_AV_DEMUX_DEVICE_ID);
			return eFdiErr;
		}
	}

	eFdiErr = FDI_DEMUX_StartPID(FAI_AV_DEMUX_DEVICE_ID, FDI_DEMUX_CHANNELTYPE_PCR, usPcrPid, &ulPcrChannelId);
	if(eFdiErr != FDI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_PlaySD_UI] : FDI_DEMUX_StartPID Error!!\n"));
	}
	eFdiErr = FDI_DEMUX_StartPID(FAI_AV_DEMUX_DEVICE_ID, FDI_DEMUX_CHANNELTYPE_VIDEO, usVideoPid, &ulVideoChannelId);
	if(eFdiErr != FDI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_PlaySD_UI] : FDI_DEMUX_StartPID Error!!\n"));
	}
	eFdiErr = FDI_DEMUX_StartPID(FAI_AV_DEMUX_DEVICE_ID, FDI_DEMUX_CHANNELTYPE_AUDIO_MAIN, usAudioPid, &ulAudioChannelId);
	if(eFdiErr != FDI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_PlaySD_UI] : FDI_DEMUX_StartPID Error!!\n"));
	}
	eFdiErr = FDI_AUDIO_Start(FAI_AV_AUDIO_DEVICE_ID, eAudioFormat);
	if(eFdiErr != FDI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_PlaySD_UI] : FDI_AUDIO_Start Error!!\n"));
	}
	eFdiErr = FDI_VIDEO_Start(FAI_AV_VIDEO_DEVICE_ID, eVideoFormat);
	if(eFdiErr != FDI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_PlaySD_UI] : FDI_VIDEO_Start Error!!\n"));
	}
	eFdiErr = FDI_VIDEO_Show(FAI_AV_VIDEO_DEVICE_ID, TRUE);
	if(eFdiErr != FDI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_PlaySD_UI] : FDI_VIDEO_Show Error!!\n"));
	}
	eFdiErr = FDI_AUDIO_SetMute(FAI_AV_AUDIO_DEVICE_ID, FALSE);
	if(eFdiErr != FDI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_PlaySD_UI] : FDI_AUDIO_SetMute Error!!\n"));
	}
	eFdiErr = FDI_AUDIO_SetVolume(FAI_AV_AUDIO_DEVICE_ID, 100);
	if(eFdiErr != FDI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_PlaySD_UI] : FDI_AUDIO_SetVolume Error!!\n"));
	}
	eFdiErr = FDI_OSD_Hide();
	if(eFdiErr != FDI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_PlaySD_UI] : FDI_OSD_Hide Error!!\n"));
	}
	g_bIsAvPlaying = TRUE;


	return eFdiErr;
}
#endif
/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if defined(AVPLAY_PLAY_NEW)
FAI_ERR_CODE INTRN_FAI_AV_Stop()
{
	FDI_ERR_CODE eFdiErr;


	/* Check AV or BEEP already playing */
	if( g_bIsBeepPlaying == TRUE )
	{
		INTRN_FAI_AV_BeepOnOff(FALSE);
	}

	eFdiErr = FDI_VIDEO_Show(FAI_AV_VIDEO_DEVICE_ID, FALSE);
	if(eFdiErr != FDI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_AV_Stop] : FDI_VIDEO_Show Error!!\n"));
		return FDI_ERR;
	}
	eFdiErr = FDI_AUDIO_SetMute(FAI_AV_AUDIO_DEVICE_ID, TRUE);
	if(eFdiErr != FDI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_AV_Stop] : FDI_AUDIO_SetMute Error!!\n"));
		return FDI_ERR;
	}
	eFdiErr = FDI_VIDEO_Stop(FAI_AV_VIDEO_DEVICE_ID);
	if(eFdiErr != FDI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_AV_Stop] : FDI_VIDEO_Stop Error!!\n"));
		return FDI_ERR;
	}
	eFdiErr = FDI_AUDIO_Stop(FAI_AV_AUDIO_DEVICE_ID);
	if(eFdiErr != FDI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_AV_Stop] : FDI_AUDIO_Stop Error!!\n"));
		return FDI_ERR;
	}
	eFdiErr = FDI_DEMUX_StopPID(FAI_AV_DEMUX_DEVICE_ID, ulPcrChannelId);
	if(eFdiErr != FDI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_AV_Stop] : FDI_DEMUX_StopPID Error!!\n"));
		return FDI_ERR;
	}
	eFdiErr = FDI_DEMUX_StopPID(FAI_AV_DEMUX_DEVICE_ID, ulVideoChannelId);
	if(eFdiErr != FDI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_AV_Stop] : FDI_DEMUX_StopPID Error!!\n"));
		return FDI_ERR;
	}
	eFdiErr = FDI_DEMUX_StopPID(FAI_AV_DEMUX_DEVICE_ID, ulAudioChannelId);
	if(eFdiErr != FDI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_AV_Stop] : FDI_DEMUX_StopPID Error!!\n"));
		return FDI_ERR;
	}
	eFdiErr = FDI_DEMUX_ReleasePath(FAI_AV_DEMUX_DEVICE_ID);
	if(eFdiErr != FDI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_AV_Stop] : FDI_DEMUX_ReleasePath Error!!\n"));
		return FDI_ERR;
	}
	g_bIsAvPlaying = FALSE;

	return FDI_NO_ERR;
}
#else
FAI_ERR_CODE INTRN_FAI_AV_Stop()
{
	FDI_ERR_CODE eFdiErr;


	/* Check AV or BEEP already playing */
	if( g_bIsBeepPlaying == TRUE )
	{
		INTRN_FAI_AV_BeepOnOff(FALSE);
	}

	eFdiErr = FDI_VIDEO_Show(FAI_AV_VIDEO_DEVICE_ID, FALSE);
	if(eFdiErr != FDI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_PlaySD_UI] : FDI_VIDEO_Show Error!!\n"));
	}
	eFdiErr = FDI_AUDIO_SetMute(FAI_AV_AUDIO_DEVICE_ID, TRUE);
	if(eFdiErr != FDI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_PlaySD_UI] : FDI_AUDIO_SetMute Error!!\n"));
	}
	eFdiErr = FDI_VIDEO_Stop(FAI_AV_VIDEO_DEVICE_ID);
	if(eFdiErr != FDI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_PlaySD_UI] : FDI_VIDEO_Stop Error!!\n"));
	}
	eFdiErr = FDI_AUDIO_Stop(FAI_AV_AUDIO_DEVICE_ID);
	if(eFdiErr != FDI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_PlaySD_UI] : FDI_AUDIO_Stop Error!!\n"));
	}
	eFdiErr = FDI_DEMUX_StopPID(FAI_AV_DEMUX_DEVICE_ID, ulPcrChannelId);
	if(eFdiErr != FDI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_PlaySD_UI] : FDI_DEMUX_StopPID Error!!\n"));
	}
	eFdiErr = FDI_DEMUX_StopPID(FAI_AV_DEMUX_DEVICE_ID, ulVideoChannelId);
	if(eFdiErr != FDI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_PlaySD_UI] : FDI_DEMUX_StopPID Error!!\n"));
	}
	eFdiErr = FDI_DEMUX_StopPID(FAI_AV_DEMUX_DEVICE_ID, ulAudioChannelId);
	if(eFdiErr != FDI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_PlaySD_UI] : FDI_DEMUX_StopPID Error!!\n"));
	}
	eFdiErr = FDI_DEMUX_ReleasePath(FAI_AV_DEMUX_DEVICE_ID);
	if(eFdiErr != FDI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_PlaySD_UI] : FDI_DEMUX_ReleasePath Error!!\n"));
	}
	g_bIsAvPlaying = FALSE;

	return eFdiErr;
}
#endif
/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if defined(AVPLAY_PLAY_NEW)
void INTRN_FAI_AV_Task(void *pData)
{
	FAI_VIDEO_MSG_t tVideoMsg;
	FAI_AUDIO_MSG_t tAudioMsg;
	FVK_ERR_CODE eVkRet;
	BOOL bAudioDecode, bVideoDecode;
	FAI_TMD_RESULT_T tTMDResult;
	FAI_TMD_ERR eTmdErr = FAI_TMD_ERR_OK;
	FDI_ERR_CODE eFdiErr;
	unsigned char ucTmdErrCode[2];

	bVideoDecode = bAudioDecode = FALSE;

	while(1)
	{
		if(FVK_MSG_Receive(g_ulVideoMsgId, &tVideoMsg, sizeof(FAI_VIDEO_MSG_t)) != FVK_NO_ERR)		
		{
			FAI_PRINT(FAI_PRT_WARNING, ("[INTRN_FAI_AV_Task] : Error FVK_Video_MSG_Receive() \n"));
			continue;
		}

		if(FVK_MSG_Receive(g_ulAudioMsgId, &tAudioMsg, sizeof(FAI_AUDIO_MSG_t)) != FVK_NO_ERR)		
		{
			FAI_PRINT(FAI_PRT_WARNING, ("[INTRN_FAI_AV_Task] : Error FVK_Video_MSG_Receive() \n"));
			continue;
		}

		if( tVideoMsg.bIsDecoding == TRUE )
		{
			bVideoDecode = TRUE;
		}
		else
		{
			bVideoDecode = FALSE;
		}

		if( tAudioMsg.bIsDecoding == TRUE )
		{
			bAudioDecode = TRUE;
		}
		else
		{
			bAudioDecode = FALSE;
		}
		
		FVK_MEM_Set((void *)&tTMDResult, 0x00, sizeof(tTMDResult));
	
		if(g_eAvTestType == FAI_AV_TEST_START)
		{			
			if((bVideoDecode == TRUE) && (bAudioDecode == TRUE))
			{			
				tTMDResult.eResult = FAI_TMD_ERR_OK;							
				FAI_TMD_SetResult(&tTMDResult);
			}
			else
			{					
				tTMDResult.eResult = FAI_TMD_ERR_FAIL;			
				ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_AVPLAY;
				ucTmdErrCode[1] = FAI_AV_TMD_ERR_PLAY_FAIL;
				FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
				FAI_TMD_SetResult(&tTMDResult);	
			}		
		}
		else if(g_eAvTestType == FAI_AV_TEST_TMD_QUICK)
		{
			if((bVideoDecode == TRUE) && (bAudioDecode == TRUE))
			{			
				tTMDResult.eResult = FAI_TMD_ERR_OK;	
				FAI_TMD_SetResult(&tTMDResult);
			}
			else
			{					
				tTMDResult.eResult = FAI_TMD_ERR_FAIL;						 
				ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_AVPLAY;
				ucTmdErrCode[1] = FAI_AV_TMD_ERR_QUICK_PLAY_FAIL;
				FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
				FAI_TMD_SetResult(&tTMDResult);	
			}		
		}
			bVideoDecode = bAudioDecode = FALSE;
			FVK_TIMER_Destory(g_ulTimerId);
			g_eAvTestType = FAI_AV_TEST_TMD_READY;
	}		
}
#else
void INTRN_FAI_AV_Task(void *pData)
{
	FAI_AV_MSG_t tAvMsg;
	FVK_ERR_CODE eVkRet;
	BOOL bAudioDecode, bVideoDecode;
	FAI_TMD_RESULT_T tTMDResult;
	FAI_TMD_ERR eTmdErr = FAI_TMD_ERR_OK;
	FDI_ERR_CODE eFdiErr;

	bAudioDecode = bVideoDecode = FALSE;
    FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));
      
	while(1)
	{
		eVkRet = FVK_MSG_Receive(g_ulAvMsgId, &tAvMsg, sizeof(FAI_AV_MSG_t));
		if (eVkRet != FVK_NO_ERR)
		{
			FAI_PRINT(FAI_PRT_WARNING, ("[INTRN_FAI_AV_Task] : Error FVK_MSG_Receive() %d\n", eVkRet));
			continue;
		}

		if( tAvMsg.eDecoderType == FAI_AV_VIDEO_DECODER )	// video
		{
			if( tAvMsg.bIsDecoding == TRUE )
			{
				bVideoDecode = TRUE;
			}
			else
			{
				bVideoDecode = FALSE;
			}
		}
		else if( tAvMsg.eDecoderType == FAI_AV_AUDIO_DECODER ) // audio
		{
			if( tAvMsg.bIsDecoding == TRUE )
			{
				bAudioDecode = TRUE;
			}
			else
			{
				bAudioDecode = FALSE;
			}
		}

		if( bVideoDecode == TRUE && bAudioDecode == TRUE )
		{
			FVK_TASK_Sleep(FAI_AV_MAX_TMD_TEST_WAIT);
			if( g_eAvTestType == FAI_AV_TEST_TMD_NORMAL )
			{
				tTMDResult.eResult = FAI_TMD_ERR_OK;
				FAI_TMD_SetResult(&tTMDResult);
			}
			else if( g_eAvTestType == FAI_AV_TEST_TMD_QUICK )
			{
				tTMDResult.eResult = FAI_TMD_ERR_OK;
				FAI_TMD_SetResult(&tTMDResult);
			}
			FVK_TIMER_Destory(g_ulTimerId);
			bAudioDecode = bVideoDecode = FALSE;
		}
 	}
}
#endif

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/

#if defined(AVPLAY_PLAY_NEW)
FDI_ERR_CODE INTRN_FAI_AV_VideoEventCallback(FDI_VIDEO_NOTIFY_EVT eEvent, FDI_VIDEO_RUNNING_STATE eState)
{
	FVK_ERR_CODE eVkRet;
	FAI_VIDEO_MSG_t tAvMsg;

	if( eEvent == FDI_VIDEO_NOTIFY_EVT_RUNNING_STATE_CHANGED )
	{
		if( eState == FDI_VIDEO_RUNNING_STATE_DECODING )
		{
			tAvMsg.bIsDecoding = TRUE;
		}
		else if( eState == FDI_VIDEO_RUNNING_STATE_NODATA )
		{
			tAvMsg.bIsDecoding = FALSE;
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_AV_VideoEventCallback] : FDI_VIDEO_RUNNING_STATE_NODATA \n"));			
		}
		else
		{
			return FDI_ERR;
		}
	}
	else
	{
		return FDI_ERR;
	}

	eVkRet = FVK_MSG_Send(g_ulVideoMsgId, &tAvMsg, sizeof(FAI_VIDEO_MSG_t));
	if (eVkRet != FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_AV_VideoEventCallback] : Error FVK_MSG_Send() : %d\n", eVkRet));
	}					
	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE INTRN_FAI_AV_AudioEventCallback(FDI_AUDIO_NOTIFY_EVT eEvent, FDI_AUDIO_RUNNING_STATE eState)
{
	FVK_ERR_CODE eVkRet;
	FAI_AUDIO_MSG_t tAvMsg;

	if( eEvent == FDI_AUDIO_RUNNING_STATE_CHANGED )
	{		
		if( eState == FDI_AUDIO_STATE_DECODING )
		{
			tAvMsg.bIsDecoding = TRUE;
		}
		else if( eState == FDI_AUDIO_STATE_NODATA )
		{
			tAvMsg.bIsDecoding = FALSE;	
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_AV_AudioEventCallback] : FAI_AV_TEST_START or QUICK PLAY -> FDI_AUDIO_STATE_NODATA \n"));		
			return FDI_ERR;
		}
		else
		{
			return FDI_ERR;
		}
	}
	else
	{
		return FDI_ERR;
	}

	eVkRet = FVK_MSG_Send(g_ulAudioMsgId, &tAvMsg, sizeof(FAI_AUDIO_MSG_t));
	if (eVkRet != FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_AV_AudioEventCallback] : Error FVK_MSG_Send() : %d\n", eVkRet));
	}		
	return FDI_NO_ERR;
}
#else
/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE INTRN_FAI_AV_VideoEventCallback(FDI_VIDEO_NOTIFY_EVT eEvent, FDI_VIDEO_RUNNING_STATE eState)
{
	FVK_ERR_CODE eVkRet;
	FAI_AV_MSG_t tAvMsg;

	tAvMsg.eDecoderType = FAI_AV_VIDEO_DECODER;
	if( eEvent == FDI_VIDEO_NOTIFY_EVT_RUNNING_STATE_CHANGED )
	{
		if( eState == FDI_VIDEO_RUNNING_STATE_DECODING )
		{
			tAvMsg.bIsDecoding = TRUE;
		}
		else if( eState == FDI_VIDEO_RUNNING_STATE_NODATA )
		{
			tAvMsg.bIsDecoding = FALSE;
		}
		else
		{
			return FDI_ERR;
		}
	}
	else
	{
		return FDI_ERR;
	}

	eVkRet = FVK_MSG_Send(g_ulAvMsgId, &tAvMsg, sizeof(FAI_AV_MSG_t));
	if (eVkRet != FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_AV_VideoEventCallback] : Error FVK_MSG_Send() : %d\n", eVkRet));
	}
	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE INTRN_FAI_AV_AudioEventCallback(FDI_AUDIO_NOTIFY_EVT eEvent, FDI_AUDIO_RUNNING_STATE eState)
{
	FVK_ERR_CODE eVkRet;
	FAI_AV_MSG_t tAvMsg;

	tAvMsg.eDecoderType = FAI_AV_AUDIO_DECODER;
	if( eEvent == FDI_AUDIO_RUNNING_STATE_CHANGED )
	{
		if( eState == FDI_AUDIO_STATE_DECODING )
		{
			tAvMsg.bIsDecoding = TRUE;
		}
		else if( eState == FDI_AUDIO_STATE_NODATA )
		{
			tAvMsg.bIsDecoding = FALSE;
		}
		else
		{
			return FDI_ERR;
		}
	}
	else
	{
		return FDI_ERR;
	}

	eVkRet = FVK_MSG_Send(g_ulAvMsgId, &tAvMsg, sizeof(FAI_AV_MSG_t));
	if (eVkRet != FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_AV_AudioEventCallback] : Error FVK_MSG_Send() : %d\n", eVkRet));
	}
	return FDI_NO_ERR;
}
#endif
