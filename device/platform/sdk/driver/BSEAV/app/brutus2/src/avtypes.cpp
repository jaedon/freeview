/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: avtypes.cpp $
* $brcm_Revision: 8 $
* $brcm_Date: 10/7/12 3:15p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/avtypes.cpp $
* 
* 8   10/7/12 3:15p tokushig
* SW7231-749: add video format to string lookup
* 
* 7   5/15/12 4:50p jrubio
* SW7346-644: add bmedia to nexus codec functions
* 
* 6   4/15/12 9:56p tokushig
* SW7231-749: added conversions from various nexus audio/video decoder
* types to string equivalents for decoder info window
* 
* 5   3/14/12 2:57p jrubio
* SW7346-644: make WRN in MSG
* 
* 4   3/12/12 9:02p tokushig
* SW7405-5581: fix problem with out of sync codec to string conversion.
* changed conversion of av types code to use hash tables instead of enum
* based array mappings to avoid out of sync lookup problems that can
* occur when NEXUS_AudioCodec and NEXUS_VideoCodec enums do not match
* lookup array mappings.  if a new nexus enum is added, existing lookup
* will still work...lookups involving the new enum value will map to
* unknown.  if a nexus enum is removed, the compiler will catch the
* inconsistency.
* 
* 3   3/9/12 3:21p jrubio
* SW7346-644: add another type of PID
* 
* 2   3/7/12 10:09a jrubio
* SW7346-644: add some functions
* 
* 1   2/29/12 11:54a jrubio
* SW7346-644: add avtypes
* 
*********************************************************************************/
#include "avtypes.h"
#include "mstringhash.h"
#include "nexus_audio_types.h"
#include "nexus_video_types.h"

BDBG_MODULE(brutus_avtypes);


struct g_videoCodec{
    NEXUS_VideoCodec nexus;
    bvideo_codec bmedia;
}g_videoCodec[] = {
          {NEXUS_VideoCodec_eUnknown, bvideo_codec_none},
          {NEXUS_VideoCodec_eUnknown, bvideo_codec_unknown},
          {NEXUS_VideoCodec_eMpeg1, bvideo_codec_mpeg1},
          {NEXUS_VideoCodec_eMpeg2, bvideo_codec_mpeg2},
          {NEXUS_VideoCodec_eMpeg4Part2, bvideo_codec_mpeg4_part2},
          {NEXUS_VideoCodec_eH263, bvideo_codec_h263},
          {NEXUS_VideoCodec_eH264, bvideo_codec_h264},
          {NEXUS_VideoCodec_eVc1, bvideo_codec_vc1},
          {NEXUS_VideoCodec_eVc1SimpleMain, bvideo_codec_vc1_sm},
          {NEXUS_VideoCodec_eDivx311, bvideo_codec_divx_311},
          {NEXUS_VideoCodec_eH264_Svc,bvideo_codec_h264_svc},    
          {NEXUS_VideoCodec_eH264_Mvc,bvideo_codec_h264_mvc},
          {NEXUS_VideoCodec_eAvs,bvideo_codec_avs},
          {NEXUS_VideoCodec_eSpark, bvideo_codec_spark},
          {NEXUS_VideoCodec_eVp6, bvideo_codec_vp6},
          {NEXUS_VideoCodec_eRv40, bvideo_codec_rv40},
          {NEXUS_VideoCodec_eVp8, bvideo_codec_vp8}
};

struct g_audioCodec {
    NEXUS_AudioCodec nexus;
    baudio_format bmedia;
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
           {NEXUS_AudioCodec_ePcmWav, baudio_format_pcm},
           {NEXUS_AudioCodec_eCook, baudio_format_cook},
           {NEXUS_AudioCodec_eVorbis, baudio_format_vorbis},
           {NEXUS_AudioCodec_eApe, baudio_format_ape}
 };

#define CONVERT(g_struct) \
    unsigned i; \
    for (i=0;i<sizeof(g_struct)/sizeof(g_struct[0]);i++) { \
        if (g_struct[i].bmedia == bmedia_value) { \
            return g_struct[i].nexus; \
        } \
    } \
    BDBG_ERR(("unable to find BMedia value %d in %s", bmedia_value, #g_struct)); \
    return g_struct[0].nexus


NEXUS_AudioCodec bmediaToAudioCodec(baudio_format bmedia_value)
{
    
    CONVERT(g_audioCodec);
}


NEXUS_VideoCodec bmediaToVideoCodec(bvideo_codec bmedia_value)
{

   CONVERT(g_videoCodec);
}

NEXUS_AudioCodec stringToAudioCodec(MString strAudio)
{
    static MStringHash hash;
    static bool        firstRun = true;

    if (true == firstRun)
    {
        firstRun = false;

        /* on first access fill out hash table */

        /* these entries must match NEXUS_AudioCodec */
        hash.add("unknown",     MString(NEXUS_AudioCodec_eUnknown).s());
        hash.add("mpeg",        MString(NEXUS_AudioCodec_eMpeg).s());
        hash.add("mp3",         MString(NEXUS_AudioCodec_eMp3).s());
        hash.add("aac",         MString(NEXUS_AudioCodec_eAac).s());
        hash.add("aacadts",     MString(NEXUS_AudioCodec_eAacAdts).s());
        hash.add("aacloas",     MString(NEXUS_AudioCodec_eAacLoas).s());
        hash.add("aacplus",     MString(NEXUS_AudioCodec_eAacPlus).s());
        hash.add("aacplusloas", MString(NEXUS_AudioCodec_eAacPlusLoas).s());
        hash.add("aacplusadts", MString(NEXUS_AudioCodec_eAacPlusAdts).s());
        hash.add("ac3",         MString(NEXUS_AudioCodec_eAc3).s());
        hash.add("ac3plus",     MString(NEXUS_AudioCodec_eAc3Plus).s());
        hash.add("dts",         MString(NEXUS_AudioCodec_eDts).s());
        hash.add("lpcmdvd",     MString(NEXUS_AudioCodec_eLpcmDvd).s());
        hash.add("lpcmhddvd",   MString(NEXUS_AudioCodec_eLpcmHdDvd).s());
        hash.add("lpcmbluray",  MString(NEXUS_AudioCodec_eLpcmBluRay).s());
        hash.add("dtshd",       MString(NEXUS_AudioCodec_eDtsHd).s());
        hash.add("wmastd",      MString(NEXUS_AudioCodec_eWmaStd).s());
        hash.add("wmastdts",    MString(NEXUS_AudioCodec_eWmaStdTs).s());
        hash.add("wmapro",      MString(NEXUS_AudioCodec_eWmaPro).s());
        hash.add("avs",         MString(NEXUS_AudioCodec_eAvs).s());
        hash.add("pcm",         MString(NEXUS_AudioCodec_ePcm).s());
        hash.add("pcmwav",      MString(NEXUS_AudioCodec_ePcmWav).s());
        hash.add("amr",         MString(NEXUS_AudioCodec_eAmr).s());
        hash.add("dra",         MString(NEXUS_AudioCodec_eDra).s());
        hash.add("cook",        MString(NEXUS_AudioCodec_eCook).s());
        hash.add("adpcm",       MString(NEXUS_AudioCodec_eAdpcm).s());
        hash.add("sbc",         MString(NEXUS_AudioCodec_eSbc).s());
        hash.add("dtslegacy",   MString(NEXUS_AudioCodec_eDtsLegacy).s());
        hash.add("vorbis",      MString(NEXUS_AudioCodec_eVorbis).s());
        hash.add("lpcm1394",    MString(NEXUS_AudioCodec_eLpcm1394).s());
        hash.add("g711",        MString(NEXUS_AudioCodec_eG711).s());
        hash.add("g723_1",      MString(NEXUS_AudioCodec_eG723_1).s());
        hash.add("g726",        MString(NEXUS_AudioCodec_eG726).s());
        hash.add("g729",        MString(NEXUS_AudioCodec_eG729).s());
        hash.add("flac",        MString(NEXUS_AudioCodec_eFlac).s());
        hash.add("invalid",     MString(NEXUS_AudioCodec_eMax).s());
    }

    /* look up audio codec */
    NEXUS_AudioCodec audioCodec = (NEXUS_AudioCodec)hash.get(strAudio).toInt();

    BDBG_MSG(("%s: string %s to codec:%d", __FUNCTION__, strAudio.s(), audioCodec));

    if ((NEXUS_AudioCodec_eUnknown == audioCodec) || (NEXUS_AudioCodec_eMax == audioCodec))
    {
        BDBG_WRN((" Audio Codec %s not Supported", strAudio.s()));
    }

    return audioCodec;
}

NEXUS_VideoCodec stringToVideoCodec(MString strVideo)
{
    static MStringHash hash;
    static bool        firstRun = true;

    if (true == firstRun)
    {
        firstRun = false;

        /* on first access fill out hash table */

        /* these entries must match NEXUS_VideoCodec */
        hash.add("unknown",       MString(NEXUS_VideoCodec_eUnknown).s());
        hash.add("none",          MString(NEXUS_VideoCodec_eNone).s());
        hash.add("mpeg1",         MString(NEXUS_VideoCodec_eMpeg1).s());
        hash.add("mpeg2",         MString(NEXUS_VideoCodec_eMpeg2).s());
        hash.add("mpeg4part2",    MString(NEXUS_VideoCodec_eMpeg4Part2).s());
        hash.add("h263",          MString(NEXUS_VideoCodec_eH263).s());
        hash.add("h264",          MString(NEXUS_VideoCodec_eH264).s());
        hash.add("h264_svc",      MString(NEXUS_VideoCodec_eH264_Svc).s());
        hash.add("h264_mvc",      MString(NEXUS_VideoCodec_eH264_Mvc).s());
        hash.add("vc1",           MString(NEXUS_VideoCodec_eVc1).s());
        hash.add("vc1simplemain", MString(NEXUS_VideoCodec_eVc1SimpleMain).s());
        hash.add("divx311",       MString(NEXUS_VideoCodec_eDivx311).s());
        hash.add("avs",           MString(NEXUS_VideoCodec_eAvs).s());
        hash.add("rv40",          MString(NEXUS_VideoCodec_eRv40).s());
        hash.add("vp6",           MString(NEXUS_VideoCodec_eVp6).s());
        hash.add("vp7",           MString(NEXUS_VideoCodec_eVp7).s());
        hash.add("vp8",           MString(NEXUS_VideoCodec_eVp8).s());
        hash.add("spark",         MString(NEXUS_VideoCodec_eSpark).s());
        hash.add("motionjpeg",    MString(NEXUS_VideoCodec_eMotionJpeg).s());
        hash.add("invalid",       MString(NEXUS_VideoCodec_eMax).s());
    }

    /* look up video codec */
    NEXUS_VideoCodec videoCodec = (NEXUS_VideoCodec)hash.get(strVideo).toInt();
    
    BDBG_MSG(("%s: string %s to codec:%d", __FUNCTION__, strVideo.s(), videoCodec));

    if ((NEXUS_VideoCodec_eUnknown == videoCodec) || (NEXUS_VideoCodec_eMax == videoCodec))
    {
        BDBG_WRN((" Video Codec %s not Supported", strVideo.s()));
    }

    return videoCodec;
}

MString audioCodecToString(NEXUS_AudioCodec audioCodec)
{
    static MStringHash hash;
    static bool        firstRun = true;

    if (true == firstRun)
    {
        firstRun = false;

        /* on first access fill out hash table */

        /* these entries must match NEXUS_AudioCodec - since we are converting
           from codec to string we can ignore adding duplicate NEXUS_AudioCodec enum values
        */
        hash.add(MString(NEXUS_AudioCodec_eUnknown).s(),     "unknown");
        hash.add(MString(NEXUS_AudioCodec_eMpeg).s(),        "mpeg");
        hash.add(MString(NEXUS_AudioCodec_eMp3).s(),         "mp3");
        hash.add(MString(NEXUS_AudioCodec_eAac).s(),         "aac");
        /*hash.add(MString(NEXUS_AudioCodec_eAacAdts).s(),     "aacadts"); DUPLICATE */
        hash.add(MString(NEXUS_AudioCodec_eAacLoas).s(),     "aacloas");
        hash.add(MString(NEXUS_AudioCodec_eAacPlus).s(),     "aacplus");
        /*hash.add(MString(NEXUS_AudioCodec_eAacPlusLoas).s(), "aacplusloas"); DUPLICATE */
        hash.add(MString(NEXUS_AudioCodec_eAacPlusAdts).s(), "aacplusadts");
        hash.add(MString(NEXUS_AudioCodec_eAc3).s(),         "ac3");
        hash.add(MString(NEXUS_AudioCodec_eAc3Plus).s(),     "ac3plus");
        hash.add(MString(NEXUS_AudioCodec_eDts).s(),         "dts");
        hash.add(MString(NEXUS_AudioCodec_eLpcmDvd).s(),     "lpcmdvd");
        hash.add(MString(NEXUS_AudioCodec_eLpcmHdDvd).s(),   "lpcmhddvd");
        hash.add(MString(NEXUS_AudioCodec_eLpcmBluRay).s(),  "lpcmbluray");
        hash.add(MString(NEXUS_AudioCodec_eDtsHd).s(),       "dtshd");
        hash.add(MString(NEXUS_AudioCodec_eWmaStd).s(),      "wmastd");
        hash.add(MString(NEXUS_AudioCodec_eWmaStdTs).s(),    "wmastdts");
        hash.add(MString(NEXUS_AudioCodec_eWmaPro).s(),      "wmapro");
        hash.add(MString(NEXUS_AudioCodec_eAvs).s(),         "avs");
        hash.add(MString(NEXUS_AudioCodec_ePcm).s(),         "pcm");
        hash.add(MString(NEXUS_AudioCodec_ePcmWav).s(),      "pcmwav");
        hash.add(MString(NEXUS_AudioCodec_eAmr).s(),         "amr");
        hash.add(MString(NEXUS_AudioCodec_eDra).s(),         "dra");
        hash.add(MString(NEXUS_AudioCodec_eCook).s(),        "cook");
        hash.add(MString(NEXUS_AudioCodec_eAdpcm).s(),       "adpcm");
        hash.add(MString(NEXUS_AudioCodec_eSbc).s(),         "sbc");
        hash.add(MString(NEXUS_AudioCodec_eDtsLegacy).s(),   "dtslegacy");
        hash.add(MString(NEXUS_AudioCodec_eVorbis).s(),      "vorbis");
        hash.add(MString(NEXUS_AudioCodec_eLpcm1394).s(),    "lpcm1394");
        hash.add(MString(NEXUS_AudioCodec_eG711).s(),        "g711");
        hash.add(MString(NEXUS_AudioCodec_eG723_1).s(),      "g723_1");
        hash.add(MString(NEXUS_AudioCodec_eG726).s(),        "g726");
        hash.add(MString(NEXUS_AudioCodec_eG729).s(),        "g729");
        hash.add(MString(NEXUS_AudioCodec_eFlac).s(),        "flac");
        hash.add(MString(NEXUS_AudioCodec_eMax).s(),         "invalid");
    }

    /* look up audio codec */
    MString strCodec = hash.get(MString(audioCodec).s());

    BDBG_MSG(("%s: codec:%d to string:%s", __FUNCTION__, audioCodec, strCodec.s()));

    if ((strCodec == "unknown") || (strCodec == "invalid"))
    {
        BDBG_WRN((" Audio Codec %d not Supported", audioCodec));
    }

    return strCodec;
}

MString audioAc3ToNumChannelsString(NEXUS_AudioAc3Acmod mode, bool lfe)
{
    static MStringHash hash;
    static MStringHash hashLFE;
    static bool        firstRun = true;

    if (true == firstRun)
    {
        firstRun = false;

        /* on first access fill out hash table */

        /* these entries must match NEXUS_AudioAc3Acmod - since we are converting
           from codec to string we can ignore adding duplicate NEXUS_AudioAc3mod enum values
        */
        hash.add(MString(NEXUS_AudioAc3Acmod_eTwoMono_1_ch1_ch2).s(),               "1+1");
        hash.add(MString(NEXUS_AudioAc3Acmod_eOneCenter_1_0_C).s(),                 "1/0.0");
        hash.add(MString(NEXUS_AudioAc3Acmod_eTwoChannel_2_0_L_R).s(),              "2/0.0");
        hash.add(MString(NEXUS_AudioAc3Acmod_eThreeChannel_3_0_L_C_R).s(),          "3/0.0");
        hash.add(MString(NEXUS_AudioAc3Acmod_eThreeChannel_2_1_L_R_S).s(),          "2/1.0");
        hash.add(MString(NEXUS_AudioAc3Acmod_eFourChannel_3_1_L_C_R_S).s(),         "3/1.0");
        hash.add(MString(NEXUS_AudioAc3Acmod_eFourChannel_2_2_L_R_SL_SR).s(),       "2/2.0");
        hash.add(MString(NEXUS_AudioAc3Acmod_eFiveChannel_3_2_L_C_R_SL_SR).s(),     "3/2.0");
        hash.add(MString(NEXUS_AudioAc3Acmod_eMax).s(),                             "unknown");

        hashLFE.add(MString(NEXUS_AudioAc3Acmod_eTwoMono_1_ch1_ch2).s(),               "1+1");
        hashLFE.add(MString(NEXUS_AudioAc3Acmod_eOneCenter_1_0_C).s(),                 "1/0.1");
        hashLFE.add(MString(NEXUS_AudioAc3Acmod_eTwoChannel_2_0_L_R).s(),              "2/0.1");
        hashLFE.add(MString(NEXUS_AudioAc3Acmod_eThreeChannel_3_0_L_C_R).s(),          "3/0.1");
        hashLFE.add(MString(NEXUS_AudioAc3Acmod_eThreeChannel_2_1_L_R_S).s(),          "2/1.1");
        hashLFE.add(MString(NEXUS_AudioAc3Acmod_eFourChannel_3_1_L_C_R_S).s(),         "3/1.1");
        hashLFE.add(MString(NEXUS_AudioAc3Acmod_eFourChannel_2_2_L_R_SL_SR).s(),       "2/2.1");
        hashLFE.add(MString(NEXUS_AudioAc3Acmod_eFiveChannel_3_2_L_C_R_SL_SR).s(),     "3/2.1");
        hashLFE.add(MString(NEXUS_AudioAc3Acmod_eMax).s(),                             "unknown");
    }

    /* look up num channels */
    MString strChannels;
    if (true == lfe) 
    {
        strChannels = hashLFE.get(MString(mode).s());
    }
    else
    {
        strChannels = hash.get(MString(mode).s());
    }

    BDBG_MSG(("%s: audio ac3 mode:%d lfe:%d to string:%s", __FUNCTION__, mode, lfe, strChannels.s()));

    if ((strChannels == "unknown") || (strChannels == "invalid"))
    {
        BDBG_WRN((" audio ac3 mode:%d lfe:%d not Supported", mode, lfe));
    }

    return strChannels;
}

MString audioAacToNumChannelsString(NEXUS_AudioAacAcmod mode, bool lfe)
{
    static MStringHash hash;
    static MStringHash hashLFE;
    static bool        firstRun = true;

    if (true == firstRun)
    {
        firstRun = false;

        /* on first access fill out hash table */

        /* these entries must match NEXUS_AudioAacAcmod - since we are converting
           from codec to string we can ignore adding duplicate NEXUS_AudioAacmod enum values
        */
        hash.add(MString(NEXUS_AudioAacAcmod_eTwoMono_1_ch1_ch2).s(),               "1+1");
        hash.add(MString(NEXUS_AudioAacAcmod_eOneCenter_1_0_C).s(),                 "1/0.0");
        hash.add(MString(NEXUS_AudioAacAcmod_eTwoChannel_2_0_L_R).s(),              "2/0.0");
        hash.add(MString(NEXUS_AudioAacAcmod_eThreeChannel_3_0_L_C_R).s(),          "3/0.0");
        hash.add(MString(NEXUS_AudioAacAcmod_eThreeChannel_2_1_L_R_S).s(),          "2/1.0");
        hash.add(MString(NEXUS_AudioAacAcmod_eFourChannel_3_1_L_C_R_S).s(),         "3/1.0");
        hash.add(MString(NEXUS_AudioAacAcmod_eFourChannel_2_2_L_R_SL_SR).s(),       "2/2.0");
        hash.add(MString(NEXUS_AudioAacAcmod_eFiveChannel_3_2_L_C_R_SL_SR).s(),     "3/2.0");
        hash.add(MString(NEXUS_AudioAacAcmod_eMax).s(),                             "unknown");

        hashLFE.add(MString(NEXUS_AudioAacAcmod_eTwoMono_1_ch1_ch2).s(),               "1+1");
        hashLFE.add(MString(NEXUS_AudioAacAcmod_eOneCenter_1_0_C).s(),                 "1/0.1");
        hashLFE.add(MString(NEXUS_AudioAacAcmod_eTwoChannel_2_0_L_R).s(),              "2/0.1");
        hashLFE.add(MString(NEXUS_AudioAacAcmod_eThreeChannel_3_0_L_C_R).s(),          "3/0.1");
        hashLFE.add(MString(NEXUS_AudioAacAcmod_eThreeChannel_2_1_L_R_S).s(),          "2/1.1");
        hashLFE.add(MString(NEXUS_AudioAacAcmod_eFourChannel_3_1_L_C_R_S).s(),         "3/1.1");
        hashLFE.add(MString(NEXUS_AudioAacAcmod_eFourChannel_2_2_L_R_SL_SR).s(),       "2/2.1");
        hashLFE.add(MString(NEXUS_AudioAacAcmod_eFiveChannel_3_2_L_C_R_SL_SR).s(),     "3/2.1");
        hashLFE.add(MString(NEXUS_AudioAacAcmod_eMax).s(),                             "unknown");
    }

    /* look up num channels */
    MString strChannels;
    if (true == lfe) 
    {
        strChannels = hashLFE.get(MString(mode).s());
    }
    else
    {
        strChannels = hash.get(MString(mode).s());
    }

    BDBG_MSG(("%s: audio Aac mode:%d lfe:%d to string:%s", __FUNCTION__, mode, lfe, strChannels.s()));

    if ((strChannels == "unknown") || (strChannels == "invalid"))
    {
        BDBG_WRN((" audio Aac mode:%d lfe:%d not Supported", mode, lfe));
    }

    return strChannels;
}

MString audioMpegToNumChannelsString(NEXUS_AudioMpegChannelMode mode)
{
    static MStringHash hash;
    static bool        firstRun = true;

    if (true == firstRun)
    {
        firstRun = false;

        /* on first access fill out hash table */

        /* these entries must match NEXUS_VideoCodec - since we are converting
           from codec to string we can ignore adding duplicate NEXUS_VideoCodec enum values
        */
        hash.add(MString(NEXUS_AudioMpegChannelMode_eStereo).s(),           "2.0");
        hash.add(MString(NEXUS_AudioMpegChannelMode_eIntensityStereo).s(),  "2.0!");
        hash.add(MString(NEXUS_AudioMpegChannelMode_eDualChannel).s(),      "1+1");
        hash.add(MString(NEXUS_AudioMpegChannelMode_eSingleChannel).s(),    "1.0");
        hash.add(MString(NEXUS_AudioMpegChannelMode_eMax).s(),              "unknown");
    }

    /* look up num channels */
    MString strChannels = hash.get(MString(mode).s());

    BDBG_MSG(("%s: audio Mpeg mode:%d to string:%s", __FUNCTION__, mode, strChannels.s()));

    if ((strChannels == "unknown") || (strChannels == "invalid"))
    {
        BDBG_WRN((" audio Mpeg mode:%d not Supported", mode));
    }

    return strChannels;
}

MString audioDtsToNumChannelsString(NEXUS_AudioDtsAmode mode)
{
    static MStringHash hash;
    static bool        firstRun = true;

    if (true == firstRun)
    {
        firstRun = false;

        /* on first access fill out hash table */

        /* these entries must match NEXUS_VideoCodec - since we are converting
           from codec to string we can ignore adding duplicate NEXUS_VideoCodec enum values
        */
        hash.add(MString(NEXUS_AudioDtsAmode_eOneCh_A).s(),                             "1.0");
        hash.add(MString(NEXUS_AudioDtsAmode_eTwoCh_A_B).s(),                           "2.0");
        hash.add(MString(NEXUS_AudioDtsAmode_eTwoCh_L_R).s(),                           "2.0");
        hash.add(MString(NEXUS_AudioDtsAmode_eTwoCh_LpR_LmR).s(),                       "2.0");
        hash.add(MString(NEXUS_AudioDtsAmode_eTwoCh_LT_RT).s(),                         "2.0");
        hash.add(MString(NEXUS_AudioDtsAmode_eThreeCh_C_L_R).s(),                       "2.1");
        hash.add(MString(NEXUS_AudioDtsAmode_eThreeCh_L_R_S).s(),                       "2.1");
        hash.add(MString(NEXUS_AudioDtsAmode_eFourCh_C_L_R_S).s(),                      "2.1.1");
        hash.add(MString(NEXUS_AudioDtsAmode_eFourCh_L_R_SL_SR).s(),                    "2.2");
        hash.add(MString(NEXUS_AudioDtsAmode_eFiveCh_C_L_R_SL_SR).s(),                  "5.1");
        hash.add(MString(NEXUS_AudioDtsAmode_eSixCh_CL_CR_L_R_SL_SR).s(),               "5.1");
        hash.add(MString(NEXUS_AudioDtsAmode_eSixCh_C_L_R_LR_RR_OV).s(),                "5.1");
        hash.add(MString(NEXUS_AudioDtsAmode_eSixCh_CF_CR_LF_RF_Lr_Rr).s(),             "5.1");
        hash.add(MString(NEXUS_AudioDtsAmode_eSevenCh_CL_C_CR_L_R_SL_SR).s(),           "7.1");
        hash.add(MString(NEXUS_AudioDtsAmode_eEightCh_CL_CR_L_R_SL1_SL2_SR1_SR2).s(),   "7.1");
        hash.add(MString(NEXUS_AudioDtsAmode_eEightCh_CL_C_CR_L_R_SL_S_SR).s(),         "7.1");
        hash.add(MString(NEXUS_AudioDtsAmode_eUserDefined).s(),                         "user defined");
        hash.add(MString(NEXUS_AudioDtsAmode_eMax).s(),                                 "unknown");
    }

    /* look up num channels */
    MString strChannels = hash.get(MString(mode).s());

    BDBG_MSG(("%s: audio Dts mode:%d to string:%s", __FUNCTION__, mode, strChannels.s()));

    if ((strChannels == "unknown") || (strChannels == "invalid"))
    {
        BDBG_WRN((" audio Dts mode:%d not Supported", mode));
    }

    return strChannels;
}

MString audioDraToNumChannelsString(NEXUS_AudioDraAcmod mode)
{
    static MStringHash hash;
    static bool        firstRun = true;

    if (true == firstRun)
    {
        firstRun = false;

        /* on first access fill out hash table */

        /* these entries must match NEXUS_VideoCodec - since we are converting
           from codec to string we can ignore adding duplicate NEXUS_VideoCodec enum values
        */
        hash.add(MString(NEXUS_AudioDraAcmod_e1_0_C).s(),               "1.1");
        hash.add(MString(NEXUS_AudioDraAcmod_e2_0_LR).s(),              "2.0");
        hash.add(MString(NEXUS_AudioDraAcmod_e2_1_LRS).s(),             "2.1");
        hash.add(MString(NEXUS_AudioDraAcmod_e2_2_LRLrRr).s(),          "2.2");
        hash.add(MString(NEXUS_AudioDraAcmod_e3_2_LRLrRrC).s(),         "3.2");
        hash.add(MString(NEXUS_AudioDraAcmod_e3_3_LRLrRrCrC).s(),       "3.3");
        hash.add(MString(NEXUS_AudioDraAcmod_e5_2_LRLrRrLsRsC).s(),     "5.2");
        hash.add(MString(NEXUS_AudioDraAcmod_e5_3_LRLrRrLsRsCrC).s(),   "5.3");
        hash.add(MString(NEXUS_AudioDraAcmod_eMax).s(),                 "unknown");
    }

    /* look up num channels */
    MString strChannels = hash.get(MString(mode).s());

    BDBG_MSG(("%s: audio Dra mode:%d to string:%s", __FUNCTION__, mode, strChannels.s()));

    if ((strChannels == "unknown") || (strChannels == "invalid"))
    {
        BDBG_WRN((" audio Dra mode:%d not Supported", mode));
    }

    return strChannels;
}

MString audioDecodeStatusToNumChannelsString(NEXUS_AudioDecoderStatus * pStatus)
{
    MString strChannels = "unknown";
    BDBG_ASSERT(NULL != pStatus);

    switch(pStatus->codec)
    {
    case NEXUS_AudioCodec_eAc3:
    case NEXUS_AudioCodec_eAc3Plus:
        strChannels = audioAc3ToNumChannelsString(pStatus->codecStatus.ac3.acmod, 
                                                  pStatus->codecStatus.ac3.lfe);
        break;

    case NEXUS_AudioCodec_eAac:
    case NEXUS_AudioCodec_eAacPlus:
        strChannels = audioAacToNumChannelsString(pStatus->codecStatus.aac.acmod, 
                                                  pStatus->codecStatus.aac.lfe);
        break;

    case NEXUS_AudioCodec_eMpeg:
    case NEXUS_AudioCodec_eMp3:
        strChannels = audioMpegToNumChannelsString(pStatus->codecStatus.mpeg.channelMode);
        break;

    case NEXUS_AudioCodec_eDts:        
    case NEXUS_AudioCodec_eDtsHd:
        strChannels = audioDtsToNumChannelsString(pStatus->codecStatus.dts.amode);
        break;

    case NEXUS_AudioCodec_eDra:
        strChannels = audioDraToNumChannelsString(pStatus->codecStatus.dra.acmod);
        break;

    default:
        strChannels = "unknown";
        break;
    }

    return strChannels;
}

MString videoCodecToString (NEXUS_VideoCodec videoCodec)
{
    static MStringHash hash;
    static bool        firstRun = true;

    if (true == firstRun)
    {
        firstRun = false;

        /* on first access fill out hash table */

        /* these entries must match NEXUS_VideoCodec - since we are converting
           from codec to string we can ignore adding duplicate NEXUS_VideoCodec enum values
        */
        hash.add(MString(NEXUS_VideoCodec_eUnknown).s(),       "unknown");
        /*hash.add(MString(NEXUS_VideoCodec_eNone).s(),          "none"); DUPLICATE */
        hash.add(MString(NEXUS_VideoCodec_eMpeg1).s(),         "mpeg1");
        hash.add(MString(NEXUS_VideoCodec_eMpeg2).s(),         "mpeg2");
        hash.add(MString(NEXUS_VideoCodec_eMpeg4Part2).s(),    "mpeg4part2");
        hash.add(MString(NEXUS_VideoCodec_eH263).s(),          "h263");
        hash.add(MString(NEXUS_VideoCodec_eH264).s(),          "h264");
        hash.add(MString(NEXUS_VideoCodec_eH264_Svc).s(),      "h264_svc");
        hash.add(MString(NEXUS_VideoCodec_eH264_Mvc).s(),      "h264_mvc");
        hash.add(MString(NEXUS_VideoCodec_eVc1).s(),           "vc1");
        hash.add(MString(NEXUS_VideoCodec_eVc1SimpleMain).s(), "vc1simplemain");
        hash.add(MString(NEXUS_VideoCodec_eDivx311).s(),       "divx311");
        hash.add(MString(NEXUS_VideoCodec_eAvs).s(),           "avs");
        hash.add(MString(NEXUS_VideoCodec_eRv40).s(),          "rv40");
        hash.add(MString(NEXUS_VideoCodec_eVp6).s(),           "vp6");
        hash.add(MString(NEXUS_VideoCodec_eVp7).s(),           "vp7");
        hash.add(MString(NEXUS_VideoCodec_eVp8).s(),           "vp8");
        hash.add(MString(NEXUS_VideoCodec_eSpark).s(),         "spark");
        hash.add(MString(NEXUS_VideoCodec_eMotionJpeg).s(),    "motionjpeg");
        hash.add(MString(NEXUS_VideoCodec_eMax).s(),           "invalid");
    }

    /* look up video codec */
    MString strCodec = hash.get(MString(videoCodec).s());

    BDBG_MSG(("%s: codec:%d to string:%s", __FUNCTION__, videoCodec, strCodec.s()));

    if ((strCodec == "unknown") || (strCodec == "invalid"))
    {
        BDBG_WRN((" Video Codec %d not Supported", videoCodec));
    }

    return strCodec;
}

MString videoFrameRateToString (NEXUS_VideoFrameRate videoFrameRate)
{
    static MStringHash hash;
    static bool        firstRun = true;

    if (true == firstRun)
    {
        firstRun = false;

        /* on first access fill out hash table */

        /* these entries must match NEXUS_VideoFrameRate - since we are converting
           from codec to string we can ignore adding duplicate NEXUS_VideoFrameRate enum values
        */
        hash.add(MString(NEXUS_VideoFrameRate_eUnknown).s(),    "unknown");
        hash.add(MString(NEXUS_VideoFrameRate_e23_976).s(),     "23.976");
        hash.add(MString(NEXUS_VideoFrameRate_e24).s(),         "24");
        hash.add(MString(NEXUS_VideoFrameRate_e25).s(),         "25");
        hash.add(MString(NEXUS_VideoFrameRate_e29_97).s(),      "29.97");
        hash.add(MString(NEXUS_VideoFrameRate_e30).s(),         "30");
        hash.add(MString(NEXUS_VideoFrameRate_e50).s(),         "50");
        hash.add(MString(NEXUS_VideoFrameRate_e59_94).s(),      "59.94");
        hash.add(MString(NEXUS_VideoFrameRate_e60).s(),         "60");
        hash.add(MString(NEXUS_VideoFrameRate_e14_985).s(),     "14.985");
        hash.add(MString(NEXUS_VideoFrameRate_e7_493).s(),      "7.493");
        hash.add(MString(NEXUS_VideoFrameRate_eMax).s(),        "invalid");
    }

    /* look up video frame rate */
    MString strFrameRate = hash.get(MString(videoFrameRate).s());

    BDBG_MSG(("%s: framerate:%d to string:%s", __FUNCTION__, videoFrameRate, strFrameRate.s()));

    if ((strFrameRate == "unknown") || (strFrameRate == "invalid"))
    {
        BDBG_WRN((" Video Frame Rate %d not Supported", videoFrameRate));
    }

    return strFrameRate;
}


MString videoFormatToString (NEXUS_VideoFormat videoFormat)
{
    static MStringHash hash;
    static bool        firstRun = true;

    if (true == firstRun)
    {
        firstRun = false;

        /* on first access fill out hash table */

        /* these entries must match NEXUS_VideoFormat - since we are converting
           from codec to string we can ignore adding duplicate NEXUS_VideoFormat enum values
        */
        hash.add(MString(NEXUS_VideoFormat_eUnknown).s(),       "unknown");
        hash.add(MString(NEXUS_VideoFormat_eNtsc).s(),          "NTSC");
        hash.add(MString(NEXUS_VideoFormat_eNtscJapan).s(),     "NTSC Japan");
        hash.add(MString(NEXUS_VideoFormat_ePalM).s(),          "Pal-M");
        hash.add(MString(NEXUS_VideoFormat_ePalN).s(),          "Pal-N");
        hash.add(MString(NEXUS_VideoFormat_ePalNc).s(),         "Pal-Nc");
        hash.add(MString(NEXUS_VideoFormat_ePalB).s(),          "Pal-B");
        hash.add(MString(NEXUS_VideoFormat_ePalB1).s(),         "Pal-B1");
        hash.add(MString(NEXUS_VideoFormat_ePalD).s(),          "Pal-D");
        hash.add(MString(NEXUS_VideoFormat_ePalD1).s(),         "Pal-D1");
        hash.add(MString(NEXUS_VideoFormat_ePalG).s(),          "Pal-G");
        hash.add(MString(NEXUS_VideoFormat_ePal).s(),           "Pal");
        hash.add(MString(NEXUS_VideoFormat_ePalH).s(),          "Pal-H");
        hash.add(MString(NEXUS_VideoFormat_ePalK).s(),          "Pal-K");
        hash.add(MString(NEXUS_VideoFormat_ePalI).s(),          "Pal-I");
        hash.add(MString(NEXUS_VideoFormat_e480p).s(),          "480p");
        hash.add(MString(NEXUS_VideoFormat_e576p).s(),          "576p");
        hash.add(MString(NEXUS_VideoFormat_e1080i).s(),         "1080i");
        hash.add(MString(NEXUS_VideoFormat_e1080i50hz).s(),     "1080i 50Hz");
        hash.add(MString(NEXUS_VideoFormat_e720p).s(),          "720p");
        hash.add(MString(NEXUS_VideoFormat_e720p50hz).s(),      "720p 50Hz");
        hash.add(MString(NEXUS_VideoFormat_e1080p).s(),         "1080p");
        hash.add(MString(NEXUS_VideoFormat_e1080p50hz).s(),     "1080p 50Hz");
        hash.add(MString(NEXUS_VideoFormat_e1080p24hz).s(),     "1080p 24Hz");
        hash.add(MString(NEXUS_VideoFormat_e1080p25hz).s(),     "1080p 25Hz");
        hash.add(MString(NEXUS_VideoFormat_e1080p30hz).s(),     "1080p 30Hz");
        hash.add(MString(NEXUS_VideoFormat_eMax).s(),           "invalid");
    }

    /* look up video format */
    MString strFormat = hash.get(MString(videoFormat).s());

    BDBG_MSG(("%s: format:%d to string:%s", __FUNCTION__, videoFormat, strFormat.s()));

    if ((strFormat == "unknown") || (strFormat == "invalid"))
    {
        BDBG_WRN((" Video Format %d not Supported", videoFormat));
    }

    return strFormat;
}


MString aspectRatioToString (NEXUS_AspectRatio aspectRatio)
{
    static MStringHash hash;
    static bool        firstRun = true;

    if (true == firstRun)
    {
        firstRun = false;

        /* on first access fill out hash table */

        /* these entries must match NEXUS_AspectRatio - since we are converting
           from codec to string we can ignore adding duplicate NEXUS_AspectRatio enum values
        */
        hash.add(MString(NEXUS_AspectRatio_eUnknown).s(),        "unknown");
        hash.add(MString(NEXUS_AspectRatio_eSquarePixel).s(),    "square");
        hash.add(MString(NEXUS_AspectRatio_e4x3).s(),            "4x3");       
        hash.add(MString(NEXUS_AspectRatio_e16x9).s(),           "16x9");     
        hash.add(MString(NEXUS_AspectRatio_e221x1).s(),          "2.21x1");   
        hash.add(MString(NEXUS_AspectRatio_e15x9).s(),           "15x9");   
        hash.add(MString(NEXUS_AspectRatio_eSar).s(),            "sar");   
        hash.add(MString(NEXUS_AspectRatio_eMax).s(),            "invalid");
    }

    /* look up aspect ratio */
    MString strAspectRatio = hash.get(MString(aspectRatio).s());

    BDBG_MSG(("%s: aspect ratio:%d to string:%s", __FUNCTION__, aspectRatio, strAspectRatio.s()));

    if ((strAspectRatio == "unknown") || (strAspectRatio == "invalid"))
    {
        BDBG_WRN((" Aspect ratio %d not Supported", aspectRatio));
    }

    return strAspectRatio;
}

