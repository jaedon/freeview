/******************************************************************************
 *    (c)2008-2012 Broadcom Corporation
 *
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 * USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 * ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: namevalue.c $
 * $brcm_Revision: 6 $
 * $brcm_Date: 9/28/12 5:33p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/utils/namevalue.c $
 * 
 * 6   9/28/12 5:33p jgarrett
 * SW7425-2075: Adding MLP
 * 
 * 5   8/1/12 11:36a erickson
 * SW7425-2981: add #if NEXUS_HAS wrappers
 * 
 * 4   6/12/12 4:47p erickson
 * SW7425-2981: move bmedia conversion code out for reuse
 * 
 * 3   5/15/12 10:10a jgarrett
 * SW7425-3065: Adding LPCM-BD
 * 
 * 2   5/2/12 4:16p erickson
 * SW7425-2981: add lookup_name
 * 
 * 1   5/2/12 2:40p erickson
 * SW7425-2981: refactor namevalue_t arrays and functions into reusable
 *  files
 * 
 *****************************************************************************/
#include "namevalue.h"
#include "nexus_types.h"
#include "nexus_stc_channel.h"
#if NEXUS_HAS_PLAYBACK
#include "nexus_playback.h"
#endif
#if NEXUS_HAS_VIDEO_DECODER
#include "nexus_video_decoder_types.h"
#endif
#if NEXUS_HAS_DISPLAY
#include "nexus_video_window.h"
#endif
#if NEXUS_HAS_VIDEO_ENCODER
#include "nexus_video_encoder.h"
#endif
#if NEXUS_HAS_FRONTEND
#include "nexus_frontend.h"
#endif
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

const namevalue_t g_videoFormatStrs[] = {
    {"ntsc",      NEXUS_VideoFormat_eNtsc},
    {"480i",      NEXUS_VideoFormat_eNtsc},
    {"pal",       NEXUS_VideoFormat_ePal},
    {"576i",      NEXUS_VideoFormat_ePal},
    {"1080i",     NEXUS_VideoFormat_e1080i},
    {"1080i50",   NEXUS_VideoFormat_e1080i50hz},
    {"720p",      NEXUS_VideoFormat_e720p},
    {"480p",      NEXUS_VideoFormat_e480p},
    {"576p",      NEXUS_VideoFormat_e576p},
    {"1080p",     NEXUS_VideoFormat_e1080p},
    {"1080p24",   NEXUS_VideoFormat_e1080p24hz},
    {"1080p25",   NEXUS_VideoFormat_e1080p25hz},
    {"1080p30",   NEXUS_VideoFormat_e1080p30hz},
    {"720p3D",    NEXUS_VideoFormat_e3D_720p},
    {"1080p3D",   NEXUS_VideoFormat_e3D_1080p_24Hz},
    {NULL, 0}
};

const namevalue_t g_videoFrameRateStrs[] = {
    {"0",      NEXUS_VideoFrameRate_eUnknown},
    {"23.976", NEXUS_VideoFrameRate_e23_976},
    {"24",     NEXUS_VideoFrameRate_e24},
    {"25",     NEXUS_VideoFrameRate_e25},
    {"29.97",  NEXUS_VideoFrameRate_e29_97},
    {"30",     NEXUS_VideoFrameRate_e30},
    {"50",     NEXUS_VideoFrameRate_e50},
    {"59.94",  NEXUS_VideoFrameRate_e59_94},
    {"60",     NEXUS_VideoFrameRate_e60},
    {"14.985", NEXUS_VideoFrameRate_e14_985},
    {"7.493",  NEXUS_VideoFrameRate_e7_493},
    {NULL, 0}
};

const namevalue_t g_transportTypeStrs[] = {
    {"ts",  NEXUS_TransportType_eTs},
    {"pes", NEXUS_TransportType_eMpeg2Pes},
    {"es",  NEXUS_TransportType_eEs},
    {"vob", NEXUS_TransportType_eVob},
    {"mp4", NEXUS_TransportType_eMp4},
    {"mkv", NEXUS_TransportType_eMkv},
    {"avi", NEXUS_TransportType_eAvi},
    {"asf", NEXUS_TransportType_eAsf},
    {"wav", NEXUS_TransportType_eWav},
    {"aiff", NEXUS_TransportType_eAiff},
    {"mp4f", NEXUS_TransportType_eMp4Fragment},
    {"rmff", NEXUS_TransportType_eRmff},
    {"flv", NEXUS_TransportType_eFlv},
    {"ogg", NEXUS_TransportType_eOgg},
    {"dss", NEXUS_TransportType_eDssEs},
    {"dss_es", NEXUS_TransportType_eDssEs}, /* alias. used for SD. */
    {"dss_pes", NEXUS_TransportType_eDssPes}, /* used for HD */
    {"mp1", NEXUS_TransportType_eMpeg1Ps},
    {NULL, 0}
};

const namevalue_t g_videoCodecStrs[] = {
    {"mpeg2", NEXUS_VideoCodec_eMpeg2},
    {"mpeg", NEXUS_VideoCodec_eMpeg2},
    {"mpeg1", NEXUS_VideoCodec_eMpeg1},
    {"avc", NEXUS_VideoCodec_eH264},
    {"h264", NEXUS_VideoCodec_eH264},
    {"svc", NEXUS_VideoCodec_eH264_Svc},
    {"mvc", NEXUS_VideoCodec_eH264_Mvc},
    {"h263", NEXUS_VideoCodec_eH263},
    {"h263", NEXUS_VideoCodec_eH263},
    {"avs", NEXUS_VideoCodec_eAvs},
    {"vc1", NEXUS_VideoCodec_eVc1},
    {"vc1sm", NEXUS_VideoCodec_eVc1SimpleMain},
    {"divx", NEXUS_VideoCodec_eMpeg4Part2},
    {"mpeg4", NEXUS_VideoCodec_eMpeg4Part2},
    {"divx311", NEXUS_VideoCodec_eDivx311},
    {"divx3", NEXUS_VideoCodec_eDivx311},
    {"rv40", NEXUS_VideoCodec_eRv40},
    {"vp6", NEXUS_VideoCodec_eVp6},
    {"vp7", NEXUS_VideoCodec_eVp7},
    {"vp8", NEXUS_VideoCodec_eVp8},
    {"spark", NEXUS_VideoCodec_eSpark},
    {"mjpeg", NEXUS_VideoCodec_eMotionJpeg},
    {NULL, 0}
};

const namevalue_t g_videoCodecProfileStrs[] = {
#if NEXUS_HAS_VIDEO_ENCODER
    {"simple", NEXUS_VideoCodecProfile_eSimple},
    {"main", NEXUS_VideoCodecProfile_eMain},
    {"high", NEXUS_VideoCodecProfile_eHigh},
    {"advanced", NEXUS_VideoCodecProfile_eAdvanced},
    {"jizhun", NEXUS_VideoCodecProfile_eJizhun},
    {"snrscl", NEXUS_VideoCodecProfile_eSnrScalable},
    {"spatialscl", NEXUS_VideoCodecProfile_eSpatiallyScalable},
    {"advsimple", NEXUS_VideoCodecProfile_eAdvancedSimple},
    {"baseline", NEXUS_VideoCodecProfile_eBaseline},
#endif    
    {NULL, 0}
};

const namevalue_t g_videoCodecLevelStrs[] = {
#if NEXUS_HAS_VIDEO_ENCODER
    {"0.0", NEXUS_VideoCodecLevel_e00},
    {"1.0", NEXUS_VideoCodecLevel_e10},
    {"1.b", NEXUS_VideoCodecLevel_e1B},
    {"1.1", NEXUS_VideoCodecLevel_e11},
    {"1.2", NEXUS_VideoCodecLevel_e12},
    {"1.3", NEXUS_VideoCodecLevel_e13},
    {"2.0", NEXUS_VideoCodecLevel_e20},
    {"2.1", NEXUS_VideoCodecLevel_e21},
    {"2.2", NEXUS_VideoCodecLevel_e22},
    {"3.0", NEXUS_VideoCodecLevel_e30},
    {"3.1", NEXUS_VideoCodecLevel_e31},
    {"3.2", NEXUS_VideoCodecLevel_e32},
    {"4.0", NEXUS_VideoCodecLevel_e40},
    {"4.1", NEXUS_VideoCodecLevel_e41},
    {"4.2", NEXUS_VideoCodecLevel_e42},
    {"5.0", NEXUS_VideoCodecLevel_e50},
    {"5.1", NEXUS_VideoCodecLevel_e51},
    {"6.0", NEXUS_VideoCodecLevel_e60},
    {"6.2", NEXUS_VideoCodecLevel_e62},
    {"low", NEXUS_VideoCodecLevel_eLow},
    {"main", NEXUS_VideoCodecLevel_eMain},
    {"high", NEXUS_VideoCodecLevel_eHigh},
    {"high1440", NEXUS_VideoCodecLevel_eHigh1440},
#endif
    {NULL, 0}
};

const namevalue_t g_audioCodecStrs[] = {
    {"mpeg", NEXUS_AudioCodec_eMpeg},
    {"mp3", NEXUS_AudioCodec_eMp3},
    {"ac3", NEXUS_AudioCodec_eAc3},
    {"ac3plus", NEXUS_AudioCodec_eAc3Plus},
    {"aac", NEXUS_AudioCodec_eAac},
    {"aacplus", NEXUS_AudioCodec_eAacPlus},
    {"wma", NEXUS_AudioCodec_eWmaStd},
    {"wmastd", NEXUS_AudioCodec_eWmaStd},
    {"wmats", NEXUS_AudioCodec_eWmaStdTs},
    {"wmapro", NEXUS_AudioCodec_eWmaPro},
    {"pcm", NEXUS_AudioCodec_ePcmWav},
    {"dra", NEXUS_AudioCodec_eDra},
    {"dts_legacy", NEXUS_AudioCodec_eDtsLegacy},
    {"dts", NEXUS_AudioCodec_eDts},
    {"dtshd", NEXUS_AudioCodec_eDtsHd},
    {"cook", NEXUS_AudioCodec_eCook},
    {"amr", NEXUS_AudioCodec_eAmr},
    {"adpcm", NEXUS_AudioCodec_eAdpcm},
    {"vorbis", NEXUS_AudioCodec_eVorbis},
    {"lpcm_1394", NEXUS_AudioCodec_eLpcm1394},
    {"lpcm_bd", NEXUS_AudioCodec_eLpcmBluRay},
    {"g711", NEXUS_AudioCodec_eG711},
    {"g726", NEXUS_AudioCodec_eG726},
    {"g729", NEXUS_AudioCodec_eG729},
    {"g723.1", NEXUS_AudioCodec_eG723_1},
    {"flac", NEXUS_AudioCodec_eFlac},
    {"mlp", NEXUS_AudioCodec_eMlp},
    {"ape", NEXUS_AudioCodec_eApe},
    {NULL, 0}
};

const namevalue_t g_stcChannelMasterStrs[] = {
    {"first", NEXUS_StcChannelAutoModeBehavior_eFirstAvailable},
    {"video", NEXUS_StcChannelAutoModeBehavior_eVideoMaster},
    {"audio", NEXUS_StcChannelAutoModeBehavior_eAudioMaster},
    {NULL, 0}
};

const namevalue_t g_endOfStreamActionStrs[] = {
#if NEXUS_HAS_PLAYBACK
    {"loop",  NEXUS_PlaybackLoopMode_eLoop},
    {"pause", NEXUS_PlaybackLoopMode_ePause},
    {"play",  NEXUS_PlaybackLoopMode_ePlay},
#endif
    {NULL, 0}
};

const namevalue_t g_tsTimestampType[] = {
    {"none", NEXUS_TransportTimestampType_eNone},
    {"mod300", NEXUS_TransportTimestampType_eMod300},
    {"binary", NEXUS_TransportTimestampType_eBinary},
    {NULL, 0}
};

const namevalue_t g_contentModeStrs[] = {
#if NEXUS_HAS_DISPLAY
    {"zoom", NEXUS_VideoWindowContentMode_eZoom},
    {"box", NEXUS_VideoWindowContentMode_eBox},
    {"panscan", NEXUS_VideoWindowContentMode_ePanScan},
    {"full", NEXUS_VideoWindowContentMode_eFull},
    {"nonlinear", NEXUS_VideoWindowContentMode_eFullNonLinear},
    /* {"panscan_no_cor", NEXUS_VideoWindowContentMode_ePanScanWithoutCorrection}, */
#endif
    {NULL, 0}
};

const namevalue_t g_vsbModeStrs[] = {
#if NEXUS_HAS_FRONTEND
    {"8", NEXUS_FrontendVsbMode_e8},
    {"16", NEXUS_FrontendVsbMode_e16},
#endif
    {NULL, 0}
};

const namevalue_t g_qamModeStrs[] = {
#if NEXUS_HAS_FRONTEND
    {"16", NEXUS_FrontendQamMode_e16},
    {"32", NEXUS_FrontendQamMode_e32},
    {"64", NEXUS_FrontendQamMode_e64},
    {"128", NEXUS_FrontendQamMode_e128},
    {"256", NEXUS_FrontendQamMode_e256},
    {"512", NEXUS_FrontendQamMode_e512},
    {"1024", NEXUS_FrontendQamMode_e1024},
    {"2048", NEXUS_FrontendQamMode_e2048},
    {"4096", NEXUS_FrontendQamMode_e4096},
    {"Auto_64_256", NEXUS_FrontendQamMode_eAuto_64_256},
#endif
    {NULL, 0}
};

const namevalue_t g_satModeStrs[] = {
#if NEXUS_HAS_FRONTEND
    {"dvb", NEXUS_FrontendSatelliteMode_eDvb},
    {"dss", NEXUS_FrontendSatelliteMode_eDss},
#endif
    {NULL, 0}
};

const namevalue_t g_videoErrorHandling[] = {
#if NEXUS_HAS_VIDEO_DECODER
    {"none", NEXUS_VideoDecoderErrorHandling_eNone},
    {"picture", NEXUS_VideoDecoderErrorHandling_ePicture},
    {"prognostic", NEXUS_VideoDecoderErrorHandling_ePrognostic},
#endif
    {NULL, 0}
};

const namevalue_t g_videoOrientation[] = {
    {"2D", NEXUS_VideoOrientation_e2D},
    {"LeftRight", NEXUS_VideoOrientation_e3D_LeftRight},   
    {"3D", NEXUS_VideoOrientation_e3D_LeftRight},
    {"OverUnder", NEXUS_VideoOrientation_e3D_OverUnder},
    {NULL, 0}
};

const namevalue_t g_sourceOrientation[] = {
#if NEXUS_HAS_VIDEO_DECODER
    {"2D", NEXUS_VideoDecoderSourceOrientation_e2D},
    {"LeftRight", NEXUS_VideoDecoderSourceOrientation_e3D_LeftRight},
    {"OverUnder", NEXUS_VideoDecoderSourceOrientation_e3D_OverUnder},
    {"LeftRightFull", NEXUS_VideoDecoderSourceOrientation_e3D_LeftRightFullFrame},
    {"RightLeftFull", NEXUS_VideoDecoderSourceOrientation_e3D_RightLeftFullFrame},
    {"LeftRightEnhanced", NEXUS_VideoDecoderSourceOrientation_e3D_LeftRightFullFrame},
#endif
    {NULL, 0}    
};

const char *lookup_name(const namevalue_t *table, int value)
{
    unsigned i;
    for (i=0;table[i].name;i++) {
        if (table[i].value == value) {
            return table[i].name;
        }
    }
    return NULL;
}

unsigned lookup(const namevalue_t *table, const char *name)
{
    unsigned i;
    unsigned value;
    char *endptr;
    for (i=0;table[i].name;i++) {
        if (!strcasecmp(table[i].name, name)) {
            return table[i].value;
        }
    }
    value = strtol(name, &endptr, 0);
    if(!endptr || *endptr) { /* if valid, *endptr = '\0' */
        value = table[0].value;
    }
    printf("Unknown cmdline param '%s', using %u as value\n", name, value);
    return value;
}

void print_list(const namevalue_t *table)
{
    unsigned i;
    const char *sep=" {";
    for (i=0;table[i].name;i++) {
        /* skip aliases */
        if (i > 0 && table[i].value == table[i-1].value) continue;
        printf("%s%s",sep,table[i].name);
        sep = ",";
    }
    printf("}");
}

void print_list_option(const char *name, const namevalue_t *list)
{
    printf("  -%s ", name);
    print_list(list);
    printf("\n");
}

struct {
    NEXUS_VideoCodec nexus;
    bvideo_codec settop;
} g_videoCodec[] = {
    {NEXUS_VideoCodec_eUnknown, bvideo_codec_none},
    {NEXUS_VideoCodec_eUnknown, bvideo_codec_unknown},
    {NEXUS_VideoCodec_eMpeg1, bvideo_codec_mpeg1},
    {NEXUS_VideoCodec_eMpeg2, bvideo_codec_mpeg2},
    {NEXUS_VideoCodec_eMpeg4Part2, bvideo_codec_mpeg4_part2},
    {NEXUS_VideoCodec_eH263, bvideo_codec_h263},
    {NEXUS_VideoCodec_eH264, bvideo_codec_h264},
    {NEXUS_VideoCodec_eH264_Svc, bvideo_codec_h264_svc},
    {NEXUS_VideoCodec_eH264_Mvc, bvideo_codec_h264_mvc},
    {NEXUS_VideoCodec_eVc1, bvideo_codec_vc1},
    {NEXUS_VideoCodec_eVc1SimpleMain, bvideo_codec_vc1_sm},
    {NEXUS_VideoCodec_eDivx311, bvideo_codec_divx_311},
    {NEXUS_VideoCodec_eRv40, bvideo_codec_rv40},
    {NEXUS_VideoCodec_eVp6, bvideo_codec_vp6},
    {NEXUS_VideoCodec_eVp8, bvideo_codec_vp8},
    {NEXUS_VideoCodec_eSpark, bvideo_codec_spark},
    {NEXUS_VideoCodec_eMotionJpeg, bvideo_codec_mjpeg},
    {NEXUS_VideoCodec_eAvs, bvideo_codec_avs}
};

struct {
    NEXUS_AudioCodec nexus;
    baudio_format settop;
} g_audioCodec[] = {
   {NEXUS_AudioCodec_eUnknown, baudio_format_unknown},
   {NEXUS_AudioCodec_eMpeg, baudio_format_mpeg},
   {NEXUS_AudioCodec_eMp3, baudio_format_mp3},
   {NEXUS_AudioCodec_eAac, baudio_format_aac},
   {NEXUS_AudioCodec_eAacPlus, baudio_format_aac_plus},
   {NEXUS_AudioCodec_eAacPlusAdts, baudio_format_aac_plus_adts},
   {NEXUS_AudioCodec_eAacPlusLoas, baudio_format_aac_plus_loas},
   {NEXUS_AudioCodec_eAc3, baudio_format_ac3},
   {NEXUS_AudioCodec_eAc3Plus, baudio_format_ac3_plus},
   {NEXUS_AudioCodec_eDts, baudio_format_dts},
   {NEXUS_AudioCodec_eLpcmHdDvd, baudio_format_lpcm_hddvd},
   {NEXUS_AudioCodec_eLpcmBluRay, baudio_format_lpcm_bluray},
   {NEXUS_AudioCodec_eDtsHd, baudio_format_dts_hd},
   {NEXUS_AudioCodec_eWmaStd, baudio_format_wma_std},
   {NEXUS_AudioCodec_eWmaPro, baudio_format_wma_pro},
   {NEXUS_AudioCodec_eLpcmDvd, baudio_format_lpcm_dvd},
   {NEXUS_AudioCodec_eAvs, baudio_format_avs},
   {NEXUS_AudioCodec_eAmr, baudio_format_amr},
   {NEXUS_AudioCodec_eDra, baudio_format_dra},
   {NEXUS_AudioCodec_eCook, baudio_format_cook},
   {NEXUS_AudioCodec_ePcmWav, baudio_format_pcm},
   {NEXUS_AudioCodec_eAdpcm, baudio_format_adpcm},
   {NEXUS_AudioCodec_eAdpcm, baudio_format_dvi_adpcm},
   {NEXUS_AudioCodec_eVorbis, baudio_format_vorbis},
   {NEXUS_AudioCodec_eLpcm1394, baudio_format_lpcm_1394},
   {NEXUS_AudioCodec_eFlac, baudio_format_flac},
   {NEXUS_AudioCodec_eApe, baudio_format_ape},
   {NEXUS_AudioCodec_eDtsLegacy, baudio_format_dts_cd},
   {NEXUS_AudioCodec_eMlp, baudio_format_mlp},
};

struct {
    NEXUS_TransportType nexus;
    unsigned settop;
} g_mpegType[] = {
    {NEXUS_TransportType_eTs, bstream_mpeg_type_unknown},
    {NEXUS_TransportType_eEs, bstream_mpeg_type_es},
    {NEXUS_TransportType_eTs, bstream_mpeg_type_bes},
    {NEXUS_TransportType_eMpeg2Pes, bstream_mpeg_type_pes},
    {NEXUS_TransportType_eTs, bstream_mpeg_type_ts},
    {NEXUS_TransportType_eDssEs, bstream_mpeg_type_dss_es},
    {NEXUS_TransportType_eDssPes, bstream_mpeg_type_dss_pes},
    {NEXUS_TransportType_eVob, bstream_mpeg_type_vob},
    {NEXUS_TransportType_eAsf, bstream_mpeg_type_asf},
    {NEXUS_TransportType_eAvi, bstream_mpeg_type_avi},
    {NEXUS_TransportType_eMpeg1Ps, bstream_mpeg_type_mpeg1},
    {NEXUS_TransportType_eMp4, bstream_mpeg_type_mp4},
    {NEXUS_TransportType_eMkv, bstream_mpeg_type_mkv},
    {NEXUS_TransportType_eWav, bstream_mpeg_type_wav},
    {NEXUS_TransportType_eAiff, bstream_mpeg_type_aiff},
    {NEXUS_TransportType_eMp4Fragment, bstream_mpeg_type_mp4_fragment},
    {NEXUS_TransportType_eRmff, bstream_mpeg_type_rmff},
    {NEXUS_TransportType_eFlv, bstream_mpeg_type_flv},
    {NEXUS_TransportType_eOgg, bstream_mpeg_type_ogg},
    {NEXUS_TransportType_eFlac, bstream_mpeg_type_flac},
    {NEXUS_TransportType_eApe, bstream_mpeg_type_ape}
};

#define CONVERT(g_struct) \
    unsigned i; \
    for (i=0;i<sizeof(g_struct)/sizeof(g_struct[0]);i++) { \
        if (g_struct[i].settop == settop_value) { \
            return g_struct[i].nexus; \
        } \
    } \
    printf("unable to find value %d in %s\n", settop_value, #g_struct); \
    return g_struct[0].nexus

NEXUS_VideoCodec b_videocodec2nexus(bvideo_codec settop_value)
{
    CONVERT(g_videoCodec);
}

NEXUS_AudioCodec b_audiocodec2nexus(baudio_format settop_value)
{
    CONVERT(g_audioCodec);
}

NEXUS_TransportType b_mpegtype2nexus(bstream_mpeg_type settop_value)
{
    CONVERT(g_mpegType);
}
