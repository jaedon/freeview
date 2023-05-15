

/*******************************************************************/
//	* @author	   Junho Park
//	* @file 		   di_mic.c
/*******************************************************************/



/*******************************************************************/
// Header Files													     */
/*******************************************************************/

#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif

#include "linden_trace.h"

#include "nexus_platform.h"
//#include "nexus_audio_playback.h"

#include "vkernel.h"
#include "taskdef.h"

#include <sys/mman.h>
#include <linux/soundcard.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/soundcard.h>

#include "di_mic.h"
#include "di_audio.h"
#include "di_uart.h"

#include "drv_audio.h"
#include "drv_mic.h"

#include "drv_err.h"
#include "drv_hotplug.h"


/*******************************************************************/
// Macro Definition													     */
/*******************************************************************/
#define MIC_NUM_MAX		CONFIG_MIC_NUM
#define MIC_MSG_SIZE		MSGQ_SIZE_NORMAL
#define DEBUG_MODULE		TRACE_MODULE_DI_AUDIO

#define OSS_MIXER
#define MIC_DEFAULT_MIXER_VOLUME 0

#define MIC_LEVEL_PERCENT_MAX	100

//linear volume 0~0x800000, over 0x800000 value is not affect to volume.
#define MIC_LEVEL_LINEAR_UNIT (NEXUS_AUDIO_VOLUME_LINEAR_NORMAL) / (MIC_LEVEL_PERCENT_MAX)

/* PCM Data Size per Sec */
/* Bit Rate = Sample Rate x Ch x bits (48000 x 16 x 2 / 8 = 192Kbps) */
#define MIC_PCM_SAMPLERATE 	48000			/* the sampling rate */
#define MIC_PCM_BITS 		16			/*  sample size: 8 or 16 bits */
#define MIC_PCM_CHANNELS	2			/* 1 = mono 2 = stereo */
#define MIC_MAX_FREGMENT_SIZE 	64*1024 		/* a half of a default fifo size */
#define MIC_PLAYBACK_FIFO_SIZE 	4*1024 			/* audio playback fifo size */

/* fregment size e,g 512 = 0x9, 1024 = 0xa, 2048 = 0xb */
#define MIC_FREGMENT_ATTR 0x100009 			/* 4 fregment buffers (high nibble, min 2, max 7fff)
												*  and 4096 bytes per each buffer (low nibble)
												*/

/*******************************************************************/
// Typedef														     */
/*******************************************************************/

typedef enum
{
	DI_MIC_STATE_DISCONNECTED,
	DI_MIC_STATE_CONNECTED_0,
	DI_MIC_STATE_CONNECTED_1,
	DI_MIC_STATE_CONNECTED_2,
	DI_MIC_STAET_CONNECTED_MAX
}DI_MIC_STATE_e;

typedef enum
{
	DI_MIC_EVT_CONN,
	DI_MIC_EVT_DISCONN,
	DI_MIC_EVT_ERR,
	DI_MIC_EVT_MAX
}DI_MIC_EVT_e;

typedef struct
{
	HULONG				ulCmmand;
	HUINT32				busnum;

}MIC_MSG;

typedef enum
{
	AUDIO_MIC_STREAM_START = 0,
	AUDIO_MIC_STREAM_STOP,
 	AUDIO_MIC_STREAM_DISCONN
} MIC_COMMAND_e;


//callback type
typedef void (*pfnMIC_EVT_CALLBACK )(int);

typedef struct
{
	pfnMIC_EVT_CALLBACK pfnCallback;
} MIC_CALLBACK_INFO_t;


typedef	struct
{
	NEXUS_AudioPlaybackHandle		audioPlayback;
	HBOOL							bStart;
	HINT32							audioFd;
	HINT32							mixerFd;
	HBOOL							bIsUsed;
	HUINT32							busnum;
} MIC_PLAY_INSTANCE;


typedef struct
{
	HUINT32							ulTask;
	HUINT32							ulSema;
	DI_MIC_EVT_e					eMsg;
	DI_MIC_STATE_e					eMicState;
	MIC_CALLBACK_INFO_t				stCallbackInfo[1];
} DRV_MIC_TASKINFO;



/*******************************************************************/
// Static Variables												    	     */
/*******************************************************************/

static HUINT32	ulMicPrevVolume = MIC_LEVEL_PERCENT_MAX;
static HUINT32	ulMicPrevMixerVolume = MIC_DEFAULT_MIXER_VOLUME;

static HULONG	ulMicEventId;
static HULONG	ulMicHplugTId;
static HULONG	s_ulMicMsgId;

static HULONG	s_ulMicDISemId;
static HULONG	s_ulMicStreamTId;

static DRV_MIC_TASKINFO	s_stMicInstance;
static MIC_PLAY_INSTANCE	g_astMicPlayInst[MIC_NUM_MAX];

#if 0
static HINT8				*dspname;
#endif

#if 1

#if (CONFIG_MIC_NUM == 3)
static HINT8 *dspname[3] = {"/dev/dsp0", "/dev/dsp1", "/dev/dsp2"};
#if defined(OSS_MIXER)
static HINT8 *mixername[3] = {"/dev/mixer0", "/dev/mixer1", "/dev/mixer2"};
#endif

#elif (CONFIG_MIC_NUM == 2)
static HINT8 *dspname[2] = {"/dev/dsp0", "/dev/dsp1"};
#if defined(OSS_MIXER)
static HINT8 *mixername[2] = {"/dev/mixer0", "/dev/mixer1"};
#endif

#elif (CONFIG_MIC_NUM == 1)
static HINT8 *dspname[1] = {"/dev/dsp0"};
#if defined(OSS_MIXER)
static HINT8 *mixername[1] = {"/dev/mixer0"};
#endif

#else
#error CONFIG_FRONTEND_TER is not defined.

#endif

#endif


/*******************************************************************/
// Function Prototype											    	     */
/*******************************************************************/

static void DRV_MIC_HpTask(void *pvParam)
{
	// variable declaration
	HULONG					ulEvent = 0;
	HINT32 					ret = 0;
	hp_evt_t 				*php_evt;
	DRV_HP_CONNECT_EVT 			eConEvt = DRV_HP_DISCONN;
	HUINT32					busnum = 0;
	HINT32					i=0;

	BSTD_UNUSED(pvParam);

	// main task loop
	while(1)
	{
		ulEvent = 0;

		// receive event from hotplug task
#if defined(SUPPORT_EVENT_TIMEOUT)
		ret=VK_EVENT_ReceiveTimeout(ulMicEventId, &ulEvent, 100);
#else
		ret=VK_EVENT_Receive(ulMicEventId, &ulEvent);
#endif

		if(ret != 0)
		{
			// event is not received
			continue;
		}

		// received event is not null
		php_evt=(hp_evt_t *)ulEvent;

		if(php_evt == NULL)
		{
			continue;
		}

		eConEvt = php_evt->eConnEvt;
		busnum = php_evt->busnum;
		PrintDebug("[%s] DRV_MIC_HpTask Receive\n", __FUNCTION__);

		switch(eConEvt)
		{
		/* CONNECT event */
		case DRV_HP_CONN:
			if(s_stMicInstance.eMicState >= MIC_NUM_MAX)
			{
				PrintError("not support!, The number of supported usb : %d\n", MIC_NUM_MAX);
			}
			else
			{

				DI_UART_Print("HP Connect! : eMicState : %d\n",s_stMicInstance.eMicState);
				DI_UART_Print("HP Connect! : bus num   : %d\n",busnum);

				s_stMicInstance.eMicState++;	// mic state change

				DRV_MIC_InitDevice(busnum);
				DRV_MIC_StartStream(busnum);
			}
			break;

		/* DISCONNECT event */
		case DRV_HP_DISCONN:
			for(i=0;i<MIC_NUM_MAX;i++)
			{
				if((g_astMicPlayInst[i].busnum == busnum)
				&& (g_astMicPlayInst[i].bIsUsed == TRUE))
				{


					DI_UART_Print("HP DisConnect! : eMicState : %d\n",s_stMicInstance.eMicState);
					DI_UART_Print("HP DisConnect! : bus num   : %d\n",busnum);

					DRV_MIC_StopStream(busnum);
					DRV_MIC_CloseDevice(busnum);

					s_stMicInstance.eMicState--;	// mic state change
				}
			}
			//DRV_MIC_StopPlayStreamTask();
			break;

		default:
			break;

		}
	}
}


static void P_MIC_PlayStreamTask(void *param)
{
	NEXUS_Error 				nResult;
	HUINT8 * 					buffer; 		//buffer start point by NEXUS GetBuffer
	HINT32						buffer_size;	//buffer size by NEXUS GetBuffer
	HINT32						vResult;
	MIC_MSG 					micMsg;
	HINT32						i;
	HBOOL						bIsStart = FALSE;
	HINT32						timeout = 100;

	BSTD_UNUSED(param);

	struct timeval time;
	fd_set fds;
	HINT32 ret;
	int SelectFd = -1;

	PrintEnter();

	while(1)
	{

		vResult = VK_MSG_ReceiveTimeout(s_ulMicMsgId, &micMsg, sizeof(MIC_MSG), timeout);

		if((vResult != VK_ERROR) && (vResult != VK_TIMEOUT))
		{

			PrintDebug("[%s].%d MSG Rcv : Cmd %d, bnum %d\n",__FUNCTION__,__LINE__,micMsg.ulCmmand, micMsg.busnum);
			PrintDebug("micMsg.ulCmmand : %d\n", micMsg.ulCmmand);
			switch(micMsg.ulCmmand)
			{
				case AUDIO_MIC_STREAM_START:
					timeout = 1;
					bIsStart = TRUE;
					break;

				case AUDIO_MIC_STREAM_STOP:
					timeout = 100;
					bIsStart = FALSE;
					break;

				case AUDIO_MIC_STREAM_DISCONN:
					if(bIsStart == TRUE)
					{
						timeout = 1;
					}
					else
					{
						timeout = 100;
					}
					PrintError("MIC Disconnected\n");
					VK_MSG_Clear(s_ulMicMsgId);

					for(i = 0; i < MIC_NUM_MAX; i++)
					{
						if(g_astMicPlayInst[i].busnum == micMsg.busnum)
						{
							/* reset audio mic device  */
							g_astMicPlayInst[i].bIsUsed = FALSE;
							if( ioctl(g_astMicPlayInst[i].audioFd, SNDCTL_DSP_RESET, NULL) == -1 )
							{
								PrintError("audioFd[%d] Reset Fail\n", i);
							}

						}
					}
#if 0
					if(s_stMicInstance.eMicState == DI_MIC_STATE_CONNECTED_0)
					{
						/* scenario */
						micMsg.ulCmmand = AUDIO_MIC_STREAM_STOP;
						VK_MSG_SendTimeout(s_ulMicMsgId,&micMsg,sizeof(MIC_MSG),0);
						PrintDebug("[%s].%d MSG Snd : Cmd %d, bnum %d\n",__FUNCTION__,__LINE__,micMsg.ulCmmand, micMsg.busnum);
					}
#endif
					break;

				default:
					bIsStart = FALSE;
					break;

			}

		}


		if(bIsStart == TRUE)
		{

			FD_ZERO (&fds);
			SelectFd = -1;

			for(i = 0; i < MIC_NUM_MAX; i++)
			{
				if(g_astMicPlayInst[i].bIsUsed == TRUE)
				{
					PrintDebug("[%d]%s Set Index=%d \n",__LINE__,__func__,i);
					FD_SET (g_astMicPlayInst[i].audioFd, &fds);

					if(SelectFd < g_astMicPlayInst[i].audioFd)
					{
						SelectFd = g_astMicPlayInst[i].audioFd;
					}
				}
			}

			time.tv_sec = 1;
			time.tv_usec = 0;

			if ((ret = select (SelectFd + 1, &fds, NULL, NULL, &time)) == -1)
			{
				PrintError("select Failed!\n");
				continue;
			}

			if (ret == 0)
			{
				PrintDebug("Timeout!\n");
				continue;
			}

			for(i = 0; i < MIC_NUM_MAX; i++)
			{

				if (FD_ISSET (g_astMicPlayInst[i].audioFd, &fds))
				{
					PrintDebug("[%s].%d FD_ISSET [%d] Select\n\n",__FUNCTION__,__LINE__,i);
					HINT32 read_bytes, write_bytes;
					struct audio_buf_info info;

					write_bytes = 0;

					nResult = NEXUS_AudioPlayback_GetBuffer(g_astMicPlayInst[i].audioPlayback, (void **)&buffer, (size_t *)&buffer_size);
					if(nResult != NEXUS_SUCCESS)
					{
						PrintError("NEXUS_AudioPlayback_GetBuffer Failed!\n");
						continue;
					}

					VK_MEM_Memset(buffer, 0, buffer_size);

					if (ioctl (g_astMicPlayInst[i].audioFd, SNDCTL_DSP_GETISPACE, &info) == -1)
					{
						PrintDebug("SNDCTL_DSP_GETISPACE Failed!\n");
						continue;
					}

					write_bytes = info.bytes;	/* How much */

					PrintDebug("[%s] info.bytes : %d, buffer_size : %d\n", __FUNCTION__, write_bytes, buffer_size);

					if(buffer_size < write_bytes)
						write_bytes = buffer_size;

					if ((read_bytes = read(g_astMicPlayInst[i].audioFd, buffer, write_bytes)))
					{
						PrintDebug("[%s] read_bytes : %d\n", __FUNCTION__, read_bytes);

						if(read_bytes < 0)
						{
							PrintError("Audio Read fail\n");
							continue;
						}

						nResult = NEXUS_AudioPlayback_ReadComplete(g_astMicPlayInst[i].audioPlayback, read_bytes);
						if(nResult != NEXUS_SUCCESS)
						{
							PrintError("NEXUS_AudioPlayback_ReadComplete failed!\n");
							continue;
						}
					}

				}

			}

		}

	}

	PrintExit();

}


DRV_Error P_MIC_StartPlayStream(void)
{
	MIC_MSG micMsg;

	PrintEnter();

	micMsg.ulCmmand = AUDIO_MIC_STREAM_START;
	VK_MSG_SendTimeout(s_ulMicMsgId,&micMsg,sizeof(MIC_MSG),0);

	PrintDebug("[%s].%d [START] MSG send! : Command %d, busnum %d\n",__FUNCTION__,__LINE__,micMsg.ulCmmand, micMsg.busnum);
	PrintDebug("[%s] P_MIC_StartPlayStream success\n", __FUNCTION__);

	PrintExit();
	return DRV_OK;
}

DRV_Error P_MIC_StopPlayStream(HUINT32 busnum)
{
	MIC_MSG micMsg;

	PrintEnter();

	micMsg.ulCmmand = AUDIO_MIC_STREAM_DISCONN;
	micMsg.busnum = busnum;

	VK_MSG_SendTimeout(s_ulMicMsgId,&micMsg,sizeof(MIC_MSG),0);

	PrintDebug("[%s].%d   [STOP] MSG send! : Command %d, busnum %d\n",__FUNCTION__,__LINE__,micMsg.ulCmmand, micMsg.busnum);
	PrintDebug("[%s] P_MIC_StopPlayStream\n", __FUNCTION__);

	PrintExit();
	return DRV_OK;
}

DI_ERR_CODE DI_MIC_Start(void)
{
	DRV_Error dResult;

	DRV_MIC_PlaybackStart();

	DI_UART_Print("[%s].%d mic playback start\n",__FUNCTION__,__LINE__);

	dResult = P_MIC_StartPlayStream();
	if ( dResult != DRV_OK )
	{
		PrintError("P_MIC_StartPlayStream failed!\n");
		return DI_ERR_ERROR;
	}

	dResult = DRV_AUDIO_SetAllOutputPCM();
	if ( dResult != DRV_OK )
	{
		PrintError("DRV_AUDIO_SetAllOutputPCM Already Set!\n");
	}

	return DI_ERR_OK;

}

DI_ERR_CODE DI_MIC_Stop(void)
{
	DRV_Error 	dResult;
	MIC_MSG 	micMsg;
	HINT32		i;

	PrintEnter();

	micMsg.ulCmmand = AUDIO_MIC_STREAM_STOP;
	VK_MSG_SendTimeout(s_ulMicMsgId,&micMsg,sizeof(MIC_MSG),0);

#if 1
	for(i = 0; i < MIC_NUM_MAX; i++)
	{
		if(g_astMicPlayInst[i].bStart == TRUE)
		{
			NEXUS_AudioPlayback_Stop(g_astMicPlayInst[i].audioPlayback);
			g_astMicPlayInst[i].bStart = FALSE;
			DI_UART_Print("audioPlayback[%d] Stop, busnum : %d, bStart : %d\n", i, g_astMicPlayInst[i].busnum, g_astMicPlayInst[i].bStart);
		}

	}
#endif

	dResult = DRV_AUDIO_UnsetAllOutputPCM();
	if ( dResult != DRV_OK )
	{
	 PrintError("DRV_AUDIO_UnsetAllOutputPCM failed!\n");
	 return DI_ERR_ERROR;
	}


	PrintDebug("[%s].%d [START] MSG send! : Command %d, busnum %d\n",__FUNCTION__,__LINE__,micMsg.ulCmmand, micMsg.busnum);
	PrintDebug("[%s] P_MIC_StartPlayStream success\n", __FUNCTION__);

	PrintExit();

	return DI_ERR_OK;

}

DI_ERR_CODE DI_MIC_SetVolume(HUINT32 ulMicVolume)
{
	HINT32		vResult;
	DRV_Error	drvResult;

	vResult = VK_SEM_Get(s_ulMicDISemId);
	if(vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DI_ERR_ERROR;
	}

	drvResult = DRV_MIC_SetVolume(ulMicVolume);
	if(drvResult != DRV_OK)
	{
		PrintError("DRV_AUDIO_GetStreamInfo failed!\n");
		VK_SEM_Release(s_ulMicDISemId);
		return DI_ERR_ERROR;
	}

	ulMicPrevVolume = ulMicVolume;

	VK_SEM_Release(s_ulMicDISemId);
	return DI_ERR_OK;
}

DI_ERR_CODE DI_MIC_SetMixerVolume(HUINT32 ulMicMixerVolume)
{
	HINT32		vResult;
	DRV_Error	drvResult;

	vResult = VK_SEM_Get(s_ulMicDISemId);
	if(vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DI_ERR_ERROR;
	}

	drvResult = DRV_MIC_SetMixerVolume(ulMicMixerVolume);
	if(drvResult != DRV_OK)
	{
		PrintError("DRV_AUDIO_GetStreamInfo failed!\n");
		VK_SEM_Release(s_ulMicDISemId);
		return DI_ERR_ERROR;
	}

	ulMicPrevMixerVolume = ulMicMixerVolume;

	VK_SEM_Release(s_ulMicDISemId);
	return DI_ERR_OK;
}

DRV_Error DRV_MIC_SetVolume(HUINT32 ulMicVolume)
{
	HUINT32 					ulVolume;
	HINT32 						i = 0;
	DRV_Error 					dResult = DRV_OK;
	NEXUS_Error 				nResult = NEXUS_SUCCESS;
	NEXUS_AudioPlaybackSettings	playbackStartSettings;

	PrintEnter();

	if(ulMicVolume > MIC_LEVEL_PERCENT_MAX)
	{
		PrintError("Mic Playback Volume value is out of range, Parameter was %d\n", ulMicVolume);
		ulVolume = MIC_LEVEL_PERCENT_MAX;
	}
	else
	{
		ulVolume = ulMicVolume;
	}

	for(i = 0; i < MIC_NUM_MAX; i++)
	{
		if(g_astMicPlayInst[i].bStart == TRUE)
		{

			NEXUS_AudioPlayback_GetSettings(g_astMicPlayInst[i].audioPlayback, &playbackStartSettings);

			playbackStartSettings.leftVolume = MIC_LEVEL_LINEAR_UNIT * ulVolume;
			playbackStartSettings.rightVolume = MIC_LEVEL_LINEAR_UNIT * ulVolume;

			nResult = NEXUS_AudioPlayback_SetSettings(g_astMicPlayInst[i].audioPlayback, &playbackStartSettings);
			if(nResult != NEXUS_SUCCESS)
			{
				PrintError("NEXUS_AudioDecoder_SetSettings Failed!\n");
				dResult = DRV_ERR;
			}
			DI_UART_Print("[%s].%d Set MicPlayInst[%d] Volume : %d\n",__FUNCTION__, __LINE__, i, ulMicVolume);
		}
	}

	PrintExit();
	return dResult;

}


DRV_Error DRV_MIC_Init(void)
{
	HINT32 ret;
	HINT32 vResult;
	HINT32 i;

	NEXUS_AudioPlaybackOpenSettings OpenSettings;

	VK_MEM_Memset(&s_stMicInstance, 0x00, sizeof(s_stMicInstance));

	PrintDebug("MIC_NUM_MAX : %d\n", MIC_NUM_MAX);

	/* init mic state */
	s_stMicInstance.eMicState = DI_MIC_STATE_DISCONNECTED;

	vResult = VK_SEM_Create(&s_ulMicDISemId,"di_mic", DEFAULT_SUSPEND_TYPE);
	if(vResult != VK_OK)
	{
		PrintError("[%s:%d] failed!\n", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	/* create MicEventId event for Hplg */
	ret = VK_EVENT_Create(&ulMicEventId, "MicHotplugEvent");
	if(ret != VK_OK)
	{
		PrintError("%s[%d] : VK_EVENT_Create() error\n",__func__,__LINE__);
		return DRV_ERR;
	}

	/* create message queue for StreamTask */
	vResult = VK_MSG_Create(MIC_MSG_SIZE, sizeof(MIC_MSG), \
			"MicMsgQ", &s_ulMicMsgId, VK_SUSPENDTYPE_FIFO);

	if(vResult != VK_OK)
	{
		PrintError(" Error(0x%x) \n\r", vResult);
		return DRV_ERR;
	}

	/* create hotplug task */
	ret=VK_TASK_Create(DRV_MIC_HpTask,
			HOTPLUG_TASK_PRIORITY,
			HOTPLUG_TASK_STACK_SIZE,
			"Mic_hotplug_Task",
			NULL,
			&ulMicHplugTId,
			FALSE);

	if(ret != VK_OK)
	{
		PrintError("[Mic_hotplug_Task] Error(0x%x) in VK_TASK_Create()\n", ret);
		return DRV_ERR;
	}

	// webcam hotplug task start
	VK_TASK_Start(ulMicHplugTId);

	// regist event
	DRV_HOTPLUG_EventRegister(DRV_HP_MIC, ulMicEventId);

	NEXUS_AudioPlayback_GetDefaultOpenSettings(&OpenSettings);
	OpenSettings.fifoSize = MIC_PLAYBACK_FIFO_SIZE;
	OpenSettings.threshold = 1;

	/* Playback Init. & connection with mixer. */
//	dspname = VK_MEM_Alloc(sizeof(dspname) * MIC_NUM_MAX);

	for(i = 0; i < MIC_NUM_MAX; i++)
	{

#if 0
		VK_snprintf(dspname[i], 10, "/dev/dsp%i", i);
		VK_printf("open dspname[%d] : %s\n", i, *dspname[i]);
#endif
		g_astMicPlayInst[i].bStart = FALSE;
		g_astMicPlayInst[i].bIsUsed = FALSE;
		g_astMicPlayInst[i].audioPlayback = NEXUS_AudioPlayback_Open(i + 2, &OpenSettings);	// already used 2 in audio
		DRV_AUDIO_ConnectMicToMixer(g_astMicPlayInst[i].audioPlayback); 					//code in di_audio.c
		PrintDebug("\n[%s] NEXUS_AudioPlayback_Open Success\n", __FUNCTION__);
	}

	PrintDebug("\n[%s] DRV_MIC_Init Success\n", __FUNCTION__);

	DRV_MIC_CreatePlayStreamTask();

	return DRV_OK;

}


DRV_Error DRV_MIC_OpenDevice(HUINT32 busnum)
{
	HINT32 fd, value, flag;
	HINT32 i;
	PrintEnter();

	/* fd open */
	for(i = 0; i < MIC_NUM_MAX ; i++)
	{
		/* fd open */
		if(g_astMicPlayInst[i].bIsUsed == FALSE)
		{

			if ( (fd = open (dspname[i], O_RDONLY, 0)) == -1 )
			{
				perror (dspname[i]);
				exit (-1);
			}
			else
			{
				/* nonblocking */
				value = fcntl(fd, F_GETFL, 0);
				value |= O_NONBLOCK;

				flag = fcntl(fd, F_SETFL, value);
				if(flag == -1)
				{
					PrintError("failed!\n");
					close(fd);
					return DRV_ERR;
				}
				PrintDebug("[%s] success %d's fd nonblocking\n", __FUNCTION__, i);

				g_astMicPlayInst[i].audioFd = fd;
				g_astMicPlayInst[i].busnum = busnum;
				g_astMicPlayInst[i].bIsUsed = TRUE;
#if defined (OSS_MIXER)
				{
					int mixerFd=0;
					if ( (mixerFd = open(mixername[i], O_RDWR)) == -1)
					{
						g_astMicPlayInst[i].mixerFd = -1;
						PrintError(" Mixer %d is not supported \n",i);
					}
					else
					{
						g_astMicPlayInst[i].mixerFd = mixerFd;
						PrintDebug(" Mixer %d is supported \n",i);
					}
				}
#endif

				PrintDebug("[%s] %d's device open success!\n", __FUNCTION__, i);
				DI_UART_Print("MIC : open /dev/dsp[%d]\n", i);
				break;
			}

		}
	}

	PrintDebug("[%s] DRV_MIC_OpenDevice\n", __FUNCTION__);
	PrintExit();
	return	DRV_OK;
}

DRV_Error DRV_MIC_CloseDevice(HUINT32 busnum)
{

	HINT32 i, j;
	PrintEnter();

	for(i = 0; i < MIC_NUM_MAX; i++)
	{
		if(g_astMicPlayInst[i].busnum == busnum)
		{
#if defined (OSS_MIXER)
			if(g_astMicPlayInst[i].mixerFd > 0)
			{
				close(g_astMicPlayInst[i].mixerFd);
				DI_UART_Print("MIC : close /dev/mixer[%d]\n\n", i);
			}
#endif
			close(g_astMicPlayInst[i].audioFd);
			g_astMicPlayInst[i].bIsUsed = FALSE;
			g_astMicPlayInst[i].busnum = 0;
			j = i;
			DI_UART_Print("g_astMicPlayInst[%d].bIsUsed : %d\n", i, g_astMicPlayInst[i].bIsUsed);
		}
	}

	DI_UART_Print("MIC : close /dev/dsp[%d]\n\n", j);

	PrintExit();
	return	DRV_OK;
}


DRV_Error DRV_MIC_SetDeviceStatus(HUINT32 busnum)
{
	HINT32 tmp;
	HINT32 devcaps;
	HINT32 frag;
	HINT32 fragsize;
	HINT32 i=0, audioFd;

#if defined(OSS_MIXER)
	HINT32 mixerFd=0;
#endif
	PrintEnter();

	for(i = 0; i < MIC_NUM_MAX; i++)
	{
		if(g_astMicPlayInst[i].busnum == busnum)
		{
			audioFd = g_astMicPlayInst[i].audioFd;
#if defined(OSS_MIXER)
			mixerFd = g_astMicPlayInst[i].mixerFd;
#endif
			break;
		}
	}

	if (ioctl (audioFd, SNDCTL_DSP_GETCAPS, &devcaps) == -1)
	{
		PrintError("SNDCTL_DSP_GETCAPS\n");
		return DRV_ERR;
	}

	frag = MIC_FREGMENT_ATTR;

	if (ioctl (audioFd, SNDCTL_DSP_SETFRAGMENT, &frag) == -1)
	{
		PrintError("SNDCTL_DSP_SETFRAGMENT\n");
		return DRV_ERR;
	}

	tmp = MIC_PCM_CHANNELS;
	if (ioctl (audioFd, SNDCTL_DSP_CHANNELS, &tmp) == -1)
	{
		PrintError("SNDCTL_DSP_CHANNELS\n");
		return DRV_ERR;
	}

	if (tmp != MIC_PCM_CHANNELS)
	{
		PrintError("%s doesn't support stereo (%d)\n", g_astMicPlayInst[i].busnum, tmp);
		return DRV_ERR;
	}

	tmp = AFMT_S16_LE;
	if (ioctl (audioFd, SNDCTL_DSP_SETFMT, &tmp) == -1)
	{
		PrintError("SNDCTL_DSP_SETFMT\n");
		return DRV_ERR;
	}

	tmp = MIC_PCM_SAMPLERATE;
	if (ioctl (audioFd, SNDCTL_DSP_SPEED, &tmp) == -1)
	{
		PrintError("SNDCTL_DSP_SPEED\n");
		return DRV_ERR;
	}

	if (tmp != MIC_PCM_SAMPLERATE)
	{
		PrintError("%s doesn't support requested rate %d (%d)\n", g_astMicPlayInst[i].busnum, MIC_PCM_SAMPLERATE, tmp);
		return DRV_ERR;
	}

	if (ioctl (audioFd, SNDCTL_DSP_GETBLKSIZE, &fragsize) == -1)
	{
		PrintError("SNDCTL_DSP_GETBLKSIZE\n");
		return DRV_ERR;
	}


	if (fragsize > MIC_MAX_FREGMENT_SIZE)//readMicBuf size = 32 *1024
	{
		PrintError("Too large fragment size %d\n", fragsize);
		return DRV_ERR;

	}

#if defined(OSS_MIXER)
	if (mixerFd > 0)
	{
		int outvol = MIC_DEFAULT_MIXER_VOLUME;

		if (ioctl (mixerFd,SOUND_MIXER_WRITE_MIC,&outvol) == -1)
		{
			PrintError("Set MIC volume error\n");
		}
		PrintError(" Set MIC volume = 0x%x\n", outvol);
	}
#endif
	PrintDebug("[%s] DRV_MIC_SetDeviceStatus\n\n\n", __FUNCTION__);

	PrintExit();
	return	DRV_OK;

}


DRV_Error DRV_MIC_CreatePlayStreamTask(void)
{
	HINT32 	vResult;
	PrintEnter();

	/* create stream task */
	vResult = VK_TASK_Create( P_MIC_PlayStreamTask,
			USER_PRIORITY7,
			128*1024,
			"MicStreamRead",
			(void *) 0,
			&s_ulMicStreamTId,
			FALSE);

	if(vResult != VK_OK)
	{
		PrintError("P_MIC_PlayStreamTask failed\n");
		return DRV_ERR;
	}

	/* start Tast */
	vResult = VK_TASK_Start(s_ulMicStreamTId);
	if(vResult != VK_OK)
	{
		PrintError("P_MIC_PlayStreamTask Start failed!\n");
		return DRV_ERR;
	}

	PrintDebug("[%s] DRV_MIC_CreatePlayStreamTask\n", __FUNCTION__);
	PrintExit();
	return DRV_OK;
}


DRV_Error DRV_MIC_StopPlayStreamTask(void)
{
	HINT32 	vResult;
	PrintEnter();

	// P_MIC_PlayStreamTask stop
	vResult = VK_TASK_Stop(s_ulMicStreamTId);
	if(vResult != VK_OK)
	{
		PrintError("P_MIC_PlayStreamTask Stop failed!\n");
		return DRV_ERR;
	}

	vResult = VK_TASK_Destroy(s_ulMicStreamTId);
	if(vResult != VK_OK)
	{
		PrintError("P_MIC_PlayStreamTask Destroy failed!\n");
		return DRV_ERR;
	}

	PrintExit();
	return DRV_OK;
}

DRV_Error DRV_MIC_StartStream(HUINT32 busnum)
{
	HINT32 result = 0;
	PrintEnter();

	// exceptional case
	if(s_stMicInstance.eMicState == DI_MIC_STATE_DISCONNECTED)
	{
		PrintError("mic state is not CONNECTED\n");
		return DI_ERR_ERROR;
	}

	result = DRV_MIC_GetPlaybackStatus(busnum);
	if(result == DRV_ERR)
	{
		PrintError("Get AudioPlayback Handle failed!\n");
		return DI_ERR_ERROR;
	}

	PrintDebug("[%s] Get AudioPlayback Handle\n", __FUNCTION__);

	//P_MIC_StartPlayStream();
	PrintDebug("[%s] P_MIC_StartPlayStream\n", __FUNCTION__);

	PrintExit();
	return DRV_OK;
}

DRV_Error DRV_MIC_StopStream(HUINT32 busnum)
{

	NEXUS_AudioPlaybackStatus status;
	HINT32 i;

	PrintEnter();

	DI_UART_Print("[%s].%d Send Stop Stream\n\n",__FUNCTION__,__LINE__);

	/* msgQ send to P_MIC_StartPlayStream */
	P_MIC_StopPlayStream(busnum);

#if 0
        DRV_AUDIO_UnsetAllOutputPCM();
#endif

	/* stop audioplayback after processing audioplayback remained queuedBytes  */

	for(i = 0; i < MIC_NUM_MAX; i++)
	{
		if(g_astMicPlayInst[i].busnum == busnum)
		{

			for ( ;; )
			{
				NEXUS_AudioPlayback_GetStatus(g_astMicPlayInst[i].audioPlayback, &status);

				if ( status.queuedBytes > 0 )
				{
					VK_TASK_Sleep(10);
				}
				else
				{
					break;
				}
			}

			NEXUS_AudioPlayback_Stop(g_astMicPlayInst[i].audioPlayback);
			g_astMicPlayInst[i].bStart = FALSE;

			/* wait playback status changing "not used" */
			for ( ;; )
			{
				NEXUS_AudioPlayback_GetStatus(g_astMicPlayInst[i].audioPlayback, &status);

				if ( status.started != FALSE )
				{
					VK_TASK_Sleep(10);
				}
				else
				{
					break;
				}
			}

		}

	}


	PrintExit();
	return DRV_OK;

}

DRV_Error DRV_MIC_InitDevice(HUINT32 busnum)
{
	PrintEnter();

	// exceptional case
	if(s_stMicInstance.eMicState == DI_MIC_STATE_DISCONNECTED)
	{
		PrintError("audio MIC state is not connected!!!\n");
		return DRV_ERR;
	}

	// mic init sequence
	DRV_MIC_OpenDevice(busnum);
	DRV_MIC_SetDeviceStatus(busnum);

	PrintDebug("[%s] DI_WEBCAM_InitAudio Success\n", __FUNCTION__);
	PrintExit();
	return DI_ERR_OK;
}

void data_callback(void *pParam1, int param2) //TODO
{
	/*
	VK_printf("Data callback - channel 0x%08x\n", (unsigned)pParam1);
	*/
	pParam1=pParam1;    /*unused*/
	BKNI_SetEvent((BKNI_EventHandle)param2);
}


DRV_Error DRV_MIC_GetPlaybackStatus(HUINT32 busnum)
{
	HINT32					vResult;
	NEXUS_Error 				nResult;
	NEXUS_AudioPlaybackStartSettings	playbackStartSettings;
	HINT32 					i = 0;

	PrintEnter();

	/*BKNI_CreateEvent(&mic_event); // TODO*/

	vResult = VK_SEM_Get(s_ulMicDISemId);
	if(vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DRV_ERR;
	}

	/* NEXUS AudioPlayback setting */
	NEXUS_AudioPlayback_GetDefaultStartSettings(&playbackStartSettings);
	playbackStartSettings.sampleRate = MIC_PCM_SAMPLERATE;
	playbackStartSettings.bitsPerSample = MIC_PCM_BITS;
	playbackStartSettings.signedData = true;
	playbackStartSettings.stereo = true;

	DRV_AUDIO_SetAllOutputPCM();

	for(i = 0; i < MIC_NUM_MAX; i++)
	{
		PrintDebug("[%d]%s index = %d localbus = %d busnum=%d\n",__LINE__,__func__,i,g_astMicPlayInst[i].busnum,busnum);
		if(g_astMicPlayInst[i].busnum == busnum)
		{
			playbackStartSettings.dataCallback.context = g_astMicPlayInst[i].audioPlayback;
			PrintDebug("[%d]%s index = %d busnum = %d\n",__LINE__,__func__,i,busnum);

			nResult = NEXUS_AudioPlayback_Start(g_astMicPlayInst[i].audioPlayback, &playbackStartSettings);
			if(nResult != NEXUS_SUCCESS)
			{
				PrintError("NEXUS_AudioPlayback_Start failed!\n");
				g_astMicPlayInst[i].bStart = FALSE;
				VK_SEM_Release(s_ulMicDISemId);
				return DRV_ERR;
				//goto out;
			}
			g_astMicPlayInst[i].bStart = TRUE;
			break;
		}
	}

	DRV_MIC_SetVolume(ulMicPrevVolume);
#if defined(OSS_MIXER)
	DRV_MIC_SetMixerVolume(ulMicPrevMixerVolume);
#endif

	VK_SEM_Release(s_ulMicDISemId);

	PrintExit();
	return DRV_OK;

}

DRV_Error DRV_MIC_PlaybackStart(void)
{
	HINT32					vResult;
	NEXUS_Error 				nResult;
	NEXUS_AudioPlaybackStartSettings	playbackStartSettings;
	HINT32 					i = 0;

	PrintEnter();

	vResult = VK_SEM_Get(s_ulMicDISemId);
	if(vResult != VK_OK)
	{
		PrintError("VK_SEM_Get failed!\n");
		return DRV_ERR;
	}

	/* NEXUS AudioPlayback setting */
	NEXUS_AudioPlayback_GetDefaultStartSettings(&playbackStartSettings);
	playbackStartSettings.sampleRate = MIC_PCM_SAMPLERATE;
	playbackStartSettings.bitsPerSample = MIC_PCM_BITS;
	playbackStartSettings.signedData = true;
	playbackStartSettings.stereo = true;

	DRV_AUDIO_SetAllOutputPCM();

	for(i = 0; i < MIC_NUM_MAX; i++)
	{
		PrintDebug("[%d]%s index = %d localbus = %d\n",__LINE__,__func__,i,g_astMicPlayInst[i].busnum);
		if( (g_astMicPlayInst[i].busnum != 0) && (g_astMicPlayInst[i].bStart == FALSE) )
		{
			playbackStartSettings.dataCallback.context = g_astMicPlayInst[i].audioPlayback;
			PrintDebug("[%d]%s index = %d\n",__LINE__,__func__,i);

			nResult = NEXUS_AudioPlayback_Start(g_astMicPlayInst[i].audioPlayback, &playbackStartSettings);
			if(nResult != NEXUS_SUCCESS)
			{
				PrintError("NEXUS_AudioPlayback_Start failed!\n");
				g_astMicPlayInst[i].bStart = FALSE;
				VK_SEM_Release(s_ulMicDISemId);
				return DRV_ERR;
				//goto out;
			}
			g_astMicPlayInst[i].bStart = TRUE;

			DI_UART_Print("g_astMicPlayInst[%d].audioPlayback Start\n", i);
		}
		else
		{
			PrintDebug("g_astMicPlayInst[%d] have already busnum : %d, bStart : %d\n", i, g_astMicPlayInst[i].busnum, g_astMicPlayInst[i].bStart);
		}
	}

	DRV_MIC_SetVolume(ulMicPrevVolume);
#if defined(OSS_MIXER)
	DRV_MIC_SetMixerVolume(ulMicPrevMixerVolume);
#endif
	VK_SEM_Release(s_ulMicDISemId);
	PrintExit();
	return DRV_OK;

}

DRV_Error DRV_MIC_SetMixerVolume(HUINT32 ulMicMixerVolume)
{

	DRV_Error 		dResult = DRV_OK;
#if defined(OSS_MIXER)
	HUINT32 		ulVolume;
	HINT32 			i = 0;

	PrintEnter();

	if(ulMicMixerVolume > MIC_LEVEL_PERCENT_MAX)
	{
		PrintError("Mic Mixer Volume value is out of range, Parameter was %d\n", ulMicMixerVolume);
		ulVolume = MIC_LEVEL_PERCENT_MAX;
	}
	else
	{
		ulVolume = ulMicMixerVolume;
	}

	for(i = 0; i < MIC_NUM_MAX; i++)
	{
		if( (g_astMicPlayInst[i].bStart == TRUE) && (g_astMicPlayInst[i].mixerFd > 0))
		{
			if (ioctl (g_astMicPlayInst[i].mixerFd,SOUND_MIXER_WRITE_MIC,&ulVolume) == -1)
			{
				PrintError(" Set Mic Mixer volume error\n");
			}
			else
			{
				DI_UART_Print("[%s].%d Set MicPlayInst[%d] Mixer Volume : 0x%x\n",__FUNCTION__, __LINE__, i, ulVolume);
			}
		}
	}

	PrintExit();
#else
	HAPPY(ulMicMixerVolume);
#endif
	return dResult;
}
