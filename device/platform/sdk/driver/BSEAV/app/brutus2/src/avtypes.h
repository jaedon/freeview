/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: avtypes.h $
* $brcm_Revision: 6 $
* $brcm_Date: 10/7/12 3:14p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/avtypes.h $
* 
* 6   10/7/12 3:14p tokushig
* SW7231-749: add video format to string lookup
* 
* 5   5/15/12 4:50p jrubio
* SW7346-644: add bmedia to nexus codec functions
* 
* 4   4/15/12 9:56p tokushig
* SW7231-749: added conversions from various nexus audio/video decoder
* types to string equivalents for decoder info window
* 
* 3   3/12/12 9:02p tokushig
* SW7405-5581: fix problem with out of sync codec to string conversion.
* changed conversion of av types code to use hash tables instead of enum
* based array mappings to avoid out of sync lookup problems that can
* occur when NEXUS_AudioCodec and NEXUS_VideoCodec enums do not match
* lookup array mappings.  if a new nexus enum is added, existing lookup
* will still work...lookups involving the new enum value will map to
* unknown.  if a nexus enum is removed, the compiler will catch the
* inconsistency.
* 
* 2   3/7/12 10:09a jrubio
* SW7346-644: add some functions
* 
* 1   2/29/12 11:54a jrubio
* SW7346-644: add avtypes
* 
* 
*********************************************************************************/
#ifndef AVTYPES_H__
#define AVTYPES_H__

#include "brutus.h"
#include "mstring.h"
#include "nexus_video_types.h"
#include "nexus_audio_types.h"
#include "nexus_audio_decoder_types.h"
#include "bmedia_types.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const char *g_audioCodecs[NEXUS_AudioCodec_eMax];		/* index: audio Codecs */
extern const char *g_videoCodecs[NEXUS_VideoCodec_eMax];	/* index: Video Codecs */
 
NEXUS_AudioCodec stringToAudioCodec(MString strAudio);
NEXUS_VideoCodec stringToVideoCodec(MString strVideo);

NEXUS_AudioCodec bmediaToAudioCodec(baudio_format audio);
NEXUS_VideoCodec bmediaToVideoCodec(bvideo_codec video);

MString audioCodecToString(NEXUS_AudioCodec audioCodec);
MString audioAc3ToNumChannelsString(NEXUS_AudioAc3Acmod mode, bool lfe);
MString audioAacToNumChannelsString(NEXUS_AudioAacAcmod mode, bool lfe);
MString audioMpegToNumChannelsString(NEXUS_AudioMpegChannelMode mode);
MString audioDtsToNumChannelsString(NEXUS_AudioDtsAmode mode);
MString audioDraToNumChannelsString(NEXUS_AudioDraAcmod mode);
MString audioDecodeStatusToNumChannelsString(NEXUS_AudioDecoderStatus * pStatus);

MString videoCodecToString (NEXUS_VideoCodec videoCodec);
MString videoFrameRateToString (NEXUS_VideoFrameRate videoFrameRate);
MString videoFormatToString (NEXUS_VideoFormat videoFormat);
MString aspectRatioToString (NEXUS_AspectRatio aspectRatio);


#ifdef __cplusplus
}
#endif

#endif /* AVTYPES_H__ */
