#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif

#include "bstd_defs.h"
#include "vkernel.h"
#include "htype.h"
#include "taskdef.h"
#include "cmdtool.h"
#include "di_err.h"
#include "di_hdmi.h"
#include "di_video.h"
#include "di_demux.h"
#include "di_audio.h"
#include "di_scart.h"
#include "drv_scart.h"
#include "drv_demux.h"

#if defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
#include "drv_cimax.h"
#include "di_ci.h"
#endif

#define IS_SAME(x,y)		(x!=NULL && y!=NULL && VK_strcmp(x, y)==0)


#define ASIA_VIDEO_PID 370
#define ASIA_AUDIO_PID 371
#define ASIA_PCR_PID 370
#define DEFAULT_TUNER 0
#define DEFAULT_VIDEO_CODEC DI_VIDEO_FORMAT_H264
#define DEFAULT_AUDIO_CODEC DI_AUDFORMAT_MPEG

DI_VIDEO_FORMAT  HWTest_AV_GetVideoFormat(char *codec)
{
	if IS_SAME(codec, "h264")
	{
		return DI_VIDEO_FORMAT_H264;
	}
	else if IS_SAME(codec, "h265")
	{
		return DI_VIDEO_FORMAT_H265;
	}
	else if IS_SAME(codec, "mpeg2")
	{
		return DI_VIDEO_FORMAT_MPEG2;
	}
	else
	{
		CMD_Printf("Unsupported Video Codec !!!\n");
		return 0;
	}
}

DI_AUDIO_FORMAT HWTest_AV_GetAudioFormat(char *codec)
{
	if IS_SAME(codec, "mpeg")
	{
		return DI_AUDFORMAT_MPEG;
	}
	else if IS_SAME(codec, "mp3")
	{
		return DI_AUDFORMAT_MP3;
	}
	else if IS_SAME(codec, "aac")
	{
		return DI_AUDFORMAT_AAC;
	}
	else if IS_SAME(codec, "aacplus")
	{
		return DI_AUDFORMAT_AAC_PLUS;
	}
	else if IS_SAME(codec, "ac3")
	{
		return DI_AUDFORMAT_AC3;
	}
	else if IS_SAME(codec, "ac3plus")
	{
		return DI_AUDFORMAT_AC3_PLUS;
	}
	else if IS_SAME(codec, "pcm")
	{
		return DI_AUDFORMAT_PCM;
	}
	else
	{
		CMD_Printf("Unsupported Audio Codec !!!\n");
		return 0;
	}
}

int HWTest_AV_Play(HUINT16 TunerId, HUINT16 VideoPid, HUINT16 AudioPid, HUINT16 PcrPid,DI_VIDEO_FORMAT eVideoCodec,  DI_AUDIO_FORMAT eAudioCodec, int nCI_Id)
{
#if defined(CONFIG_SCART)
	DRV_SCART_OPERATION* pstScartOp;
#endif
	Pid_Info pidInfo;

	pidInfo.pidBitmask = PID_BITMASK_AUDIO_MAIN | PID_BITMASK_VIDEO | PID_BITMASK_PCR;
	pidInfo.usPCRPid = PcrPid;
	pidInfo.usVideoPid = VideoPid;
	pidInfo.usAudioMainPid = AudioPid;
	pidInfo.usAudioSubPid = 0x1fff;

	DI_AUDIO_Stop(0);
	DI_VIDEO_Stop(0);
	DI_DEMUX_StopPID(0,pidInfo.pidBitmask);
	DI_DEMUX_ReleasePath(0);
#if defined(CONFIG_SCART)
	pstScartOp = DRV_SCART_GetOpHandle();
	pstScartOp->pfSet_Normal_Mode();
#endif

	DI_DEMUX_SetPath(0,DI_DEMUX_INPUT_TUNER,TunerId, DI_DEMUX_OUTPUT_DECODER, 0,nCI_Id);

#if defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
	if(nCI_Id != -1)
	{
		DRV_CIMAX_ResetFIFO(nCI_Id);
	}
#endif

	DI_DEMUX_StartPID(0, (void *)&pidInfo);
	DI_VIDEO_Start(0,eVideoCodec);	//DI_VIDEO_CODEC_H264
	DI_AUDIO_Start(0,eAudioCodec);
	DI_VIDEO_Show(0,1);

	return 0;
}

int HWTest_AV_PlayOnly(HUINT16 TunerId, HUINT16 VideoPid, HUINT16 AudioPid, HUINT16 PcrPid,DI_VIDEO_FORMAT eVideoCodec,  DI_AUDIO_FORMAT eAudioCodec, int nCI_Id)
{
#if defined(CONFIG_SCART)
	DRV_SCART_OPERATION* pstScartOp;
#endif
	Pid_Info pidInfo;

	pidInfo.pidBitmask = PID_BITMASK_AUDIO_MAIN | PID_BITMASK_VIDEO | PID_BITMASK_PCR;
	pidInfo.usPCRPid = PcrPid;
	pidInfo.usVideoPid = VideoPid;
	pidInfo.usAudioMainPid = AudioPid;
	pidInfo.usAudioSubPid = 0x1fff;

#if defined(CONFIG_SCART)
	pstScartOp = DRV_SCART_GetOpHandle();
	pstScartOp->pfSet_Normal_Mode();
#endif

	DI_DEMUX_SetPath(0,DI_DEMUX_INPUT_TUNER,TunerId, DI_DEMUX_OUTPUT_DECODER, 0,nCI_Id);

	DI_DEMUX_StartPID(0, (void *)&pidInfo);
	DI_VIDEO_Start(0,eVideoCodec);	//DI_VIDEO_CODEC_H264
	DI_AUDIO_Start(0,eAudioCodec);
	DI_VIDEO_Show(0,1);

	return 0;
}

int HWTest_AV_Play_ASIA(void)
{
	int ret;
	ret = HWTest_AV_Play(DEFAULT_TUNER, ASIA_VIDEO_PID, ASIA_AUDIO_PID, ASIA_PCR_PID,DEFAULT_VIDEO_CODEC, DEFAULT_AUDIO_CODEC, -1);
	return ret;
}

