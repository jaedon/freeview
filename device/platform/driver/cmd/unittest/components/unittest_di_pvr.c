/**
 * unittest_di_pvr.c
*/

/**
 * @author		Larry
*/

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
#include "linden_trace.h"
#include "unittest.h"
#include "di_uart.h"
#include "di_err.h"
#include "di_pvr.h"
#include "vkernel.h"
#include "di_media.h"
#include "di_demux.h"
#include "di_audio.h"
#include "di_video.h"



/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/


/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/

#define MAX_RECORDING_PATH	128
#define MAX_REC_NUM 5
#define MAX_PID_NUM 5
#define DEMUX_ID_RECORDER 1
#define DEMUX_ID_PLAYBACK 2


static char g_recordingPath[MAX_RECORDING_PATH] = "/mnt/hd2";
static HUINT32 g_recPidChHandle[MAX_REC_NUM][MAX_PID_NUM];

unsigned int g_videopid = 0x65;
unsigned int g_videotype = DI_MEDIA_VIDEO_FORMAT_MPEG2;
unsigned int g_audiopid = 0x66;
unsigned int g_audiotype = DI_MEDIA_AUDIO_FORMAT_MPEG;
unsigned int g_pcrpid = 0x65;

char g_buffer[MAX_RECORDING_PATH];


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/


/*******************************************************************/
/*********************** Function Prototypes ***********************/
/*******************************************************************/


static UT_RESULT ut_pvr_rec(void)
{
	UT_RESULT ret = UT_FAIL;
	DI_ERR_CODE result;
	unsigned int ulDeviceId = 0;
	HUINT32 num;
	DI_PVR_REC_SETUP_t recSetup;

	DI_PVR_REC_GetCapability(&num);
	DI_UART_Print("PVR Capability : %d \n", num);

	VK_memset(&recSetup, 0, sizeof(DI_PVR_REC_SETUP_t));
	VK_snprintf(g_buffer, MAX_RECORDING_PATH-1, "%s/unittest_pvr_output", g_recordingPath);
	VK_memcpy((void *)recSetup.aucFileName, (void *)g_buffer, VK_strlen((char *)g_buffer)+1);

	recSetup.eFileType = eDI_PVR_FileType_Linear;
	recSetup.eStreamType = eDI_PVR_Stream_Video;
	recSetup.eVideoType = g_videotype;

	DI_UART_Print("REC Setup !!! \n");
	result = DI_PVR_REC_Setup(ulDeviceId, &recSetup);
	if(result != DI_ERR_OK)
	{
		goto done;
	}

	result = DI_DEMUX_SetPath(DEMUX_ID_RECORDER, DI_DEMUX_INPUT_TUNER, 0, DI_DEMUX_OUTPUT_RECORDER, 0, -1);
	if(result != DI_ERR_OK)
	{
		goto done;
	}

	result = DI_PVR_REC_AddPidChannel(ulDeviceId, g_videopid, DI_PVR_REC_PID_PES_TYPE_VIDEO, &g_recPidChHandle[ulDeviceId][0]);
	if(result != DI_ERR_OK)
	{
		goto done;
	}

	result = DI_PVR_REC_AddPidChannel(ulDeviceId, g_audiopid, DI_PVR_REC_PID_PES_TYPE_AUDIO, &g_recPidChHandle[ulDeviceId][1]);
	if(result != DI_ERR_OK)
	{
		goto done;
	}

	result = DI_PVR_REC_AddPidChannel(ulDeviceId, g_pcrpid, DI_PVR_REC_PID_PES_TYPE_PCR, &g_recPidChHandle[ulDeviceId][2]);
	if(result != DI_ERR_OK)
	{
		goto done;
	}

	DI_UART_Print("REC Start !!! \n");
	result = DI_PVR_REC_Start(ulDeviceId);
	if(result != DI_ERR_OK)
	{
		goto done;
	}

	DI_UART_Print("Waiting for 60s to record...\n");
	VK_TASK_Sleep(60000);//Sleep 60s

	DI_UART_Print("REC Stop !!! \n");
	result = DI_PVR_REC_Stop(ulDeviceId);
	if(result != DI_ERR_OK)
	{
		goto done;
	}

	result = DI_PVR_REC_RemovePidChannel(ulDeviceId, g_recPidChHandle[ulDeviceId][0]);
	if(result != DI_ERR_OK)
	{
		goto done;
	}
	result = DI_PVR_REC_RemovePidChannel(ulDeviceId, g_recPidChHandle[ulDeviceId][1]);
	if(result != DI_ERR_OK)
	{
		goto done;
	}
	result = DI_PVR_REC_RemovePidChannel(ulDeviceId, g_recPidChHandle[ulDeviceId][2]);
	if(result != DI_ERR_OK)
	{
		goto done;
	}

	ret = UT_OK;
done:
	return ret;

}

static UT_RESULT ut_pvr_playback(void)
{
	UT_RESULT ret = UT_FAIL;
	DI_ERR_CODE result;
	unsigned int playbackId;
	unsigned int ulDeviceId = 0;
	HUINT32 num;
	DI_PVR_PLAY_SETUP_t playSetup;
	Pid_Info pidInfo;
	DI_VIDEO_RUNNING_STATE vstate;
	DI_AUDIO_RUNNING_STATE astate;

	pidInfo.pidBitmask = PID_BITMASK_VIDEO | PID_BITMASK_AUDIO_MAIN | PID_BITMASK_PCR;
	pidInfo.usVideoPid = g_videopid;
	pidInfo.usAudioMainPid = g_audiopid;
	pidInfo.usPCRPid = g_pcrpid;

	/* Need to stop decoder 0 to use for playback */
	DI_VIDEO_GetState(0, &vstate);
	if (DI_VIDEO_RUNNING_STATE_DECODING == vstate)
	{
		result = DI_VIDEO_Stop(0);
		if(result != DI_ERR_OK)
		{
			goto done;
		}
	}

	DI_AUDIO_GetState(0, &astate);
	if (DI_AUDIO_STATE_DECODING == astate)
	{
		result = DI_AUDIO_Stop(0);
		if(result != DI_ERR_OK)
		{
			goto done;
		}
	}

	VK_TASK_Sleep(20);

	DI_DEMUX_StopPID(0, pidInfo.pidBitmask);

	DI_DEMUX_ReleasePath(0);

	/* Find demux id for PLayback */
	playbackId = DRV_DEMUX_GetFirstPlaybackParserIndex();

	result = DI_DEMUX_SetPath(playbackId, DI_DEMUX_INPUT_PLAYER, 0, DI_DEMUX_OUTPUT_DECODER, 0, -1);
	if(result != DI_ERR_OK)
	{
		goto done;
	}

	/* Get filename */
	VK_memset(&playSetup, 0, sizeof(DI_PVR_PLAY_SETUP_t));
	VK_snprintf(g_buffer, MAX_RECORDING_PATH-1, "%s/unittest_pvr_output", g_recordingPath);
	VK_memcpy((void *)playSetup.aucFileName, (void *)g_buffer, VK_strlen((char *)g_buffer)+1);

	playSetup.eFileType = eDI_PVR_FileType_Linear;
	playSetup.eStreamType = eDI_PVR_Stream_Video;
	playSetup.eStartCondition = DI_PVR_PLAY_START_MODE_BEGIN;
	playSetup.bIsRecording = FALSE;

	/* Config for playback setup */
	result = DI_PVR_PLAY_Setup(ulDeviceId, &playSetup);
	if(result != DI_ERR_OK)
	{
		goto done;
	}


	result = DI_DEMUX_StartPID(playbackId, &pidInfo);

	result = DI_AUDIO_Start(0,g_audiotype);
	if(result != DI_ERR_OK)
	{
		goto done;
	}

	result = DI_VIDEO_Start(0,g_videotype);
	if(result != DI_ERR_OK)
	{
		goto done;
	}

	/* Start playback */
	result = DI_PVR_PLAY_Start(ulDeviceId);
	if(result != DI_ERR_OK)
	{
		goto done;
	}

	VK_TASK_Sleep(10000);//Sleep 10s to playback

	result = DI_PVR_PLAY_Stop(ulDeviceId);
	if(result != DI_ERR_OK)
	{
		goto done;
	}

	result = DI_AUDIO_Stop(0);
	if(result != DI_ERR_OK)
	{
		goto done;
	}

	result = DI_VIDEO_Stop(0);
	if(result != DI_ERR_OK)
	{
		goto done;
	}

	result = DI_DEMUX_StopPID(playbackId, pidInfo.pidBitmask);
	if(result != DI_ERR_OK)
	{
		goto done;
	}

	result = DI_DEMUX_ReleasePath(playbackId);
	if(result != DI_ERR_OK)
	{
		goto done;
	}

	VK_TASK_Sleep(20);




	/* Return Live play*/

	result = DI_DEMUX_SetPath(0, DI_DEMUX_INPUT_TUNER, 0, DI_DEMUX_OUTPUT_DECODER, 0, -1);
	if(result != DI_ERR_OK)
	{
		goto done;
	}

	result = DI_DEMUX_StartPID(0, &pidInfo);

	result = DI_AUDIO_Start(0, g_audiotype);
	if(result != DI_ERR_OK)
	{
		goto done;
	}

	result = DI_VIDEO_Start(0, g_videotype);
	if(result != DI_ERR_OK)
	{
		goto done;
	}

	VK_TASK_Sleep(20);

	ret = UT_OK;
done:
	return ret;

}


static UT_MENU_ITEM_T	menu_pvr[] =
{
	MENU_START,
	MENU_FUNC(ut_pvr_rec, 60),
	MENU_FUNC(ut_pvr_playback, 10),
	MENU_END,
};

static UT_MENU_ITEM_T	menu_main_pvr[] =
{
	MENU_SUB(menu_pvr),
};

void UT_PVR_Init(void)
{
	HINT32 i;
	HINT32	array_size;

	array_size = sizeof(menu_main_pvr)/sizeof(menu_main_pvr[0]);
	for (i=0; i<array_size; i++)
	{
		UT_MENU_AddMainMenu(&menu_main_pvr[i]);
	}
	array_size = sizeof(menu_pvr)/sizeof(menu_pvr[0]);
	for (i=0; i<array_size; i++)
	{
		UT_MENU_AddMenu(&menu_pvr[i]);
	}
}

