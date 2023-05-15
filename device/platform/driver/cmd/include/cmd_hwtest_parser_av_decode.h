#ifndef __CMD_HWTEST_PARSER_AV_DECODE_H__
#define __CMD_HWTEST_PARSER_AV_DECODE_H__

#include "htype.h"
#include "di_video.h"
#include "di_audio.h"

extern DI_VIDEO_FORMAT  HWTest_AV_GetVideoFormat(char *codec);
extern DI_AUDIO_FORMAT HWTest_AV_GetAudioFormat(char *codec);
extern int HWTest_AV_Play(HUINT16 TunerId, HUINT16 VideoPid, HUINT16 AudioPid, HUINT16 PcrPid,DI_VIDEO_FORMAT eVideoCodec,  DI_AUDIO_FORMAT eAudioCodec, int nCI_Id);
extern int HWTest_AV_Play_ASIA(void);
int HWTest_AV_PlayOnly(HUINT16 TunerId, HUINT16 VideoPid, HUINT16 AudioPid, HUINT16 PcrPid,DI_VIDEO_FORMAT eVideoCodec,  DI_AUDIO_FORMAT eAudioCodec, int nCI_Id);

#endif //__CMD_HWTEST_PARSER_AV_DECODE_H__

