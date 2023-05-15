/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef ASE_COMMON_DATA_TYPES_H
#define ASE_COMMON_DATA_TYPES_H

#include <nrdbase/StdInt.h>
#include <memory>
#include <cassert>
#include <vector>
#include <string>
#include <nrdbase/tr1.h>

#define NEW_MEDIAPRESENTATION

namespace netflix {

// Note: Do not change the values of these ContentProfile's.
// If we need to obsolete one, change the name to PROFILE_OBSOLETE_xxx

#define VIDEO_PROFILE_OFFSET     0x0000
#define AUDIO_PROFILE_OFFSET     0x6000
#define TIMEDTEXT_PROFILE_OFFSET 0xC000

enum ContentProfile
{
    UNKNOWN_PROFILE = -1,

    // video profiles
    VIDEO_PROFILE_START = VIDEO_PROFILE_OFFSET,
    VIDEO_PLAYREADY_MPEG4P2_PIFF,
    VIDEO_PLAYREADY_H264_BPL30_PIFF,
    VIDEO_PLAYREADY_H264_MPL30_PIFF,
    VIDEO_PLAYREADY_H264_MPL31_PIFF,
    VIDEO_PLAYREADY_H264_MPL40_PIFF,
    VIDEO_PLAYREADY_H264_BPL30_DASH,
    VIDEO_PLAYREADY_H264_MPL30_DASH,
    VIDEO_PLAYREADY_H264_MPL31_DASH,
    VIDEO_PLAYREADY_H264_MPL40_DASH,
    VIDEO_PLAYREADY_H264_SHPL30_DASH,
    VIDEO_PLAYREADY_H264_SHPL31_DASH,
    VIDEO_PLAYREADY_H264_SHPL40_DASH,
    VIDEO_NONE_H264_BPL13_DASH,
    VIDEO_NONE_H264_MPL30_DASH,
    VIDEO_NONE_H264_MPL31_DASH,
    VIDEO_NONE_H264_MPL40_DASH,
    VIDEO_PLAYREADY_H264MPL30_M2TS,
    VIDEO_PLAYREADY_H264MPL31_M2TS,
    VIDEO_PLAYREADY_H264MPL40_M2TS,
    VIDEO_PLAYREADY_H264MPL13_M2TS,

    HEVC_MAIN_L20_DASH_CENC,
    HEVC_MAIN_L21_DASH_CENC,
    HEVC_MAIN_L30_DASH_CENC,
    HEVC_MAIN_L31_DASH_CENC,
    HEVC_MAIN_L40_DASH_CENC,
    HEVC_MAIN_L41_DASH_CENC,
    HEVC_MAIN_L50_DASH_CENC,
    HEVC_MAIN_L51_DASH_CENC,

    HEVC_MAIN_L20_L21_DASH_CENC_TL,
    HEVC_MAIN_L21_L30_DASH_CENC_TL,
    HEVC_MAIN_L30_L31_DASH_CENC_TL,
    HEVC_MAIN_L31_L40_DASH_CENC_TL,
    HEVC_MAIN_L40_L41_DASH_CENC_TL,
    HEVC_MAIN_L50_L51_DASH_CENC_TL,

    HEVC_MAIN10_L20_DASH_CENC,
    HEVC_MAIN10_L21_DASH_CENC,
    HEVC_MAIN10_L30_DASH_CENC,
    HEVC_MAIN10_L31_DASH_CENC,
    HEVC_MAIN10_L40_DASH_CENC,
    HEVC_MAIN10_L41_DASH_CENC,
    HEVC_MAIN10_L50_DASH_CENC,
    HEVC_MAIN10_L51_DASH_CENC,
	
    HEVC_MAIN10_L20_L21_DASH_CENC_TL,
    HEVC_MAIN10_L21_L30_DASH_CENC_TL,
    HEVC_MAIN10_L30_L31_DASH_CENC_TL,
    HEVC_MAIN10_L31_L40_DASH_CENC_TL,
    HEVC_MAIN10_L40_L41_DASH_CENC_TL,
    HEVC_MAIN10_L50_L51_DASH_CENC_TL,

#ifdef NRDP_HAS_HDR_VIDEO
    HEVC_DV_MAIN10_L20_DASH_CENC,
    HEVC_DV_MAIN10_L21_DASH_CENC,
    HEVC_DV_MAIN10_L30_DASH_CENC,
    HEVC_DV_MAIN10_L31_DASH_CENC,
    HEVC_DV_MAIN10_L40_DASH_CENC,
    HEVC_DV_MAIN10_L41_DASH_CENC,
    HEVC_DV_MAIN10_L50_DASH_CENC,
    HEVC_DV_MAIN10_L51_DASH_CENC,

    HEVC_HDR_MAIN10_L30_DASH_CENC,
    HEVC_HDR_MAIN10_L31_DASH_CENC,
    HEVC_HDR_MAIN10_L40_DASH_CENC,
    HEVC_HDR_MAIN10_L41_DASH_CENC,
    HEVC_HDR_MAIN10_L50_DASH_CENC,
    HEVC_HDR_MAIN10_L51_DASH_CENC,
#endif

    LAST_VIDEO_PROFILE,

    // audio profiles
    AUDIO_PROFILE_START = AUDIO_PROFILE_OFFSET,
    AUDIO_DDPLUS_5_1_PIFF,
    AUDIO_HEAAC_2_PIFF,
    AUDIO_PLAYREADY_OGGVORBIS_2_PIFF,
    AUDIO_PLAYREADY_OGGVORBIS_2_DASH,
    AUDIO_DDPLUS_5_1_DASH,
    AUDIO_HEAAC_2_DASH,
    AUDIO_PLAYREADY_HEAAC_2_DASH,
    AUDIO_HEACC_2_ELEM,
    AUDIO_DD_5_1_ELEM,
    AUDIO_HEACC_2_M2TS,
    AUDIO_DD_5_1_M2TS,

    LAST_AUDIO_PROFILE,

    // timed text profiles
    TIMEDTEXT_PROFILE_START = TIMEDTEXT_PROFILE_OFFSET,
    TIMEDTEXT_SIMPLE_SDH1,
    TIMEDTEXT_DFXP_LS_SDH
};

inline ContentProfile stringToProfile(const std::string& p)
{
    if (p == "playready-mpeg4p2-piff")      return VIDEO_PLAYREADY_MPEG4P2_PIFF;
    if (p == "playready-h264bpl30-piff")    return VIDEO_PLAYREADY_H264_BPL30_PIFF;
    if (p == "playready-h264mpl30-piff")    return VIDEO_PLAYREADY_H264_MPL30_PIFF;
    if (p == "playready-h264mpl31-piff")    return VIDEO_PLAYREADY_H264_MPL31_PIFF;
    if (p == "playready-h264mpl40-piff")    return VIDEO_PLAYREADY_H264_MPL40_PIFF;
    if (p == "playready-h264bpl30-dash")    return VIDEO_PLAYREADY_H264_BPL30_DASH;
    if (p == "playready-h264mpl30-dash")    return VIDEO_PLAYREADY_H264_MPL30_DASH;
    if (p == "playready-h264mpl31-dash")    return VIDEO_PLAYREADY_H264_MPL31_DASH;
    if (p == "playready-h264mpl40-dash")    return VIDEO_PLAYREADY_H264_MPL40_DASH;
    if (p == "playready-h264shpl30-dash")   return VIDEO_PLAYREADY_H264_SHPL30_DASH;
    if (p == "playready-h264shpl31-dash")   return VIDEO_PLAYREADY_H264_SHPL31_DASH;
    if (p == "playready-h264shpl40-dash")   return VIDEO_PLAYREADY_H264_SHPL40_DASH;
    if (p == "nodrm-h264bpl13-dash")        return VIDEO_NONE_H264_BPL13_DASH;
    if (p == "none-h264mpl30-dash")         return VIDEO_NONE_H264_MPL30_DASH;
    if (p == "none-h264mpl31-dash")         return VIDEO_NONE_H264_MPL31_DASH;
    if (p == "none-h264mpl40-dash")         return VIDEO_NONE_H264_MPL40_DASH;

    if (p == "hevc-main-L20-dash-cenc") return HEVC_MAIN_L20_DASH_CENC;
    if (p == "hevc-main-L21-dash-cenc") return HEVC_MAIN_L21_DASH_CENC;
    if (p == "hevc-main-L30-dash-cenc") return HEVC_MAIN_L30_DASH_CENC;
    if (p == "hevc-main-L31-dash-cenc") return HEVC_MAIN_L31_DASH_CENC;
    if (p == "hevc-main-L40-dash-cenc") return HEVC_MAIN_L40_DASH_CENC;
    if (p == "hevc-main-L41-dash-cenc") return HEVC_MAIN_L41_DASH_CENC;
    if (p == "hevc-main-L50-dash-cenc") return HEVC_MAIN_L50_DASH_CENC;
    if (p == "hevc-main-L51-dash-cenc") return HEVC_MAIN_L51_DASH_CENC;

    if (p == "hevc-main-L20-L21-dash-cenc-tl") return HEVC_MAIN_L20_L21_DASH_CENC_TL;
    if (p == "hevc-main-L21-L30-dash-cenc-tl") return HEVC_MAIN_L21_L30_DASH_CENC_TL;
    if (p == "hevc-main-L30-L31-dash-cenc-tl") return HEVC_MAIN_L30_L31_DASH_CENC_TL;
    if (p == "hevc-main-L31-L40-dash-cenc-tl") return HEVC_MAIN_L31_L40_DASH_CENC_TL;
    if (p == "hevc-main-L40-L41-dash-cenc-tl") return HEVC_MAIN_L40_L41_DASH_CENC_TL;
    if (p == "hevc-main-L50-L51-dash-cenc-tl") return HEVC_MAIN_L50_L51_DASH_CENC_TL;

    if (p == "hevc-main10-L20-dash-cenc") return HEVC_MAIN10_L20_DASH_CENC;
    if (p == "hevc-main10-L21-dash-cenc") return HEVC_MAIN10_L21_DASH_CENC;
    if (p == "hevc-main10-L30-dash-cenc") return HEVC_MAIN10_L30_DASH_CENC;
    if (p == "hevc-main10-L31-dash-cenc") return HEVC_MAIN10_L31_DASH_CENC;
    if (p == "hevc-main10-L40-dash-cenc") return HEVC_MAIN10_L40_DASH_CENC;
    if (p == "hevc-main10-L41-dash-cenc") return HEVC_MAIN10_L41_DASH_CENC;
    if (p == "hevc-main10-L50-dash-cenc") return HEVC_MAIN10_L50_DASH_CENC;
    if (p == "hevc-main10-L51-dash-cenc") return HEVC_MAIN10_L51_DASH_CENC;
	
    if (p == "hevc-main10-L20-L21-dash-cenc-tl") return HEVC_MAIN10_L20_L21_DASH_CENC_TL;
    if (p == "hevc-main10-L21-L30-dash-cenc-tl") return HEVC_MAIN10_L21_L30_DASH_CENC_TL;
    if (p == "hevc-main10-L30-L31-dash-cenc-tl") return HEVC_MAIN10_L30_L31_DASH_CENC_TL;
    if (p == "hevc-main10-L31-L40-dash-cenc-tl") return HEVC_MAIN10_L31_L40_DASH_CENC_TL;
    if (p == "hevc-main10-L40-L41-dash-cenc-tl") return HEVC_MAIN10_L40_L41_DASH_CENC_TL;
    if (p == "hevc-main10-L50-L51-dash-cenc-tl") return HEVC_MAIN10_L50_L51_DASH_CENC_TL;
#ifdef NRDP_HAS_HDR_VIDEO
    if (p == "hevc-dv-main10-L20-dash-cenc") return HEVC_DV_MAIN10_L20_DASH_CENC;
    if (p == "hevc-dv-main10-L21-dash-cenc") return HEVC_DV_MAIN10_L21_DASH_CENC;
    if (p == "hevc-dv-main10-L30-dash-cenc") return HEVC_DV_MAIN10_L30_DASH_CENC;
    if (p == "hevc-dv-main10-L31-dash-cenc") return HEVC_DV_MAIN10_L31_DASH_CENC;
    if (p == "hevc-dv-main10-L40-dash-cenc") return HEVC_DV_MAIN10_L40_DASH_CENC;
    if (p == "hevc-dv-main10-L41-dash-cenc") return HEVC_DV_MAIN10_L41_DASH_CENC;
    if (p == "hevc-dv-main10-L50-dash-cenc") return HEVC_DV_MAIN10_L50_DASH_CENC;
    if (p == "hevc-dv-main10-L51-dash-cenc") return HEVC_DV_MAIN10_L51_DASH_CENC;

    if (p == "hevc-hdr-main10-L30-dash-cenc") return HEVC_HDR_MAIN10_L30_DASH_CENC;
    if (p == "hevc-hdr-main10-L31-dash-cenc") return HEVC_HDR_MAIN10_L31_DASH_CENC;
    if (p == "hevc-hdr-main10-L40-dash-cenc") return HEVC_HDR_MAIN10_L40_DASH_CENC;
    if (p == "hevc-hdr-main10-L41-dash-cenc") return HEVC_HDR_MAIN10_L41_DASH_CENC;
    if (p == "hevc-hdr-main10-L50-dash-cenc") return HEVC_HDR_MAIN10_L50_DASH_CENC;
    if (p == "hevc-hdr-main10-L51-dash-cenc") return HEVC_HDR_MAIN10_L51_DASH_CENC;
#endif
    if (p == "playready-h264mpl30-m2ts")           return VIDEO_PLAYREADY_H264MPL30_M2TS;
    if (p == "playready-h264mpl31-m2ts")           return VIDEO_PLAYREADY_H264MPL31_M2TS;
    if (p == "playready-h264mpl40-m2ts")           return VIDEO_PLAYREADY_H264MPL40_M2TS;
    if (p == "playready-h264mpl13-m2ts")           return VIDEO_PLAYREADY_H264MPL13_M2TS;
    if (p == "ddplus-5.1-piff")             return AUDIO_DDPLUS_5_1_PIFF;
    if (p == "heaac-2-piff")                return AUDIO_HEAAC_2_PIFF;
    if (p == "playready-oggvorbis-2-piff")  return AUDIO_PLAYREADY_OGGVORBIS_2_PIFF;
    if (p == "playready-oggvorbis-2-dash")  return AUDIO_PLAYREADY_OGGVORBIS_2_DASH;
    if (p == "ddplus-5.1-dash")             return AUDIO_DDPLUS_5_1_DASH;
    if (p == "heaac-2-dash")                return AUDIO_HEAAC_2_DASH;
    if (p == "playready-heaac-2-dash")      return AUDIO_PLAYREADY_HEAAC_2_DASH;
    if (p == "heaac-2-elem")                return AUDIO_HEACC_2_ELEM;
    if (p == "dd-5.1-elem")                 return AUDIO_DD_5_1_ELEM;
    if (p == "heaac-2-m2ts")                return AUDIO_HEACC_2_M2TS;
    if (p == "dd-5.1-m2ts")                 return AUDIO_DD_5_1_M2TS;

    if (p == "simplesdh")                   return TIMEDTEXT_SIMPLE_SDH1;
    if (p == "dfxp-ls-sdh")                 return TIMEDTEXT_DFXP_LS_SDH;

    return UNKNOWN_PROFILE;
}

inline std::string profileToString(ContentProfile p)
{
    switch (p)
    {
    case VIDEO_PLAYREADY_MPEG4P2_PIFF:     return "playready-mpeg4p2-piff";
    case VIDEO_PLAYREADY_H264_BPL30_PIFF:  return "playready-h264bpl30-piff";
    case VIDEO_PLAYREADY_H264_MPL30_PIFF:  return "playready-h264mpl30-piff";
    case VIDEO_PLAYREADY_H264_MPL31_PIFF:  return "playready-h264mpl31-piff";
    case VIDEO_PLAYREADY_H264_MPL40_PIFF:  return "playready-h264mpl40-piff";
    case VIDEO_PLAYREADY_H264_BPL30_DASH:  return "playready-h264bpl30-dash";
    case VIDEO_PLAYREADY_H264_MPL30_DASH:  return "playready-h264mpl30-dash";
    case VIDEO_PLAYREADY_H264_MPL31_DASH:  return "playready-h264mpl31-dash";
    case VIDEO_PLAYREADY_H264_MPL40_DASH:  return "playready-h264mpl40-dash";
    case VIDEO_PLAYREADY_H264_SHPL30_DASH: return "playready-h264shpl30-dash";
    case VIDEO_PLAYREADY_H264_SHPL31_DASH: return "playready-h264shpl31-dash";
    case VIDEO_PLAYREADY_H264_SHPL40_DASH: return "playready-h264shpl40-dash";
    case VIDEO_NONE_H264_BPL13_DASH:       return "nodrm-h264bpl13-dash";
    case VIDEO_NONE_H264_MPL30_DASH:       return "none-h264mpl30-dash";
    case VIDEO_NONE_H264_MPL31_DASH:       return "none-h264mpl31-dash";
    case VIDEO_NONE_H264_MPL40_DASH:       return "none-h264mpl40-dash";
    case VIDEO_PLAYREADY_H264MPL30_M2TS:     return "playready-h264mpl30-m2ts";
    case VIDEO_PLAYREADY_H264MPL31_M2TS:     return "playready-h264mpl31-m2ts";
    case VIDEO_PLAYREADY_H264MPL40_M2TS:     return "playready-h264mpl40-m2ts";
    case VIDEO_PLAYREADY_H264MPL13_M2TS:     return "playready-h264mpl13-m2ts";

    case HEVC_MAIN_L20_DASH_CENC: return "hevc-main-L20-dash-cenc";
    case HEVC_MAIN_L21_DASH_CENC: return "hevc-main-L21-dash-cenc";
    case HEVC_MAIN_L30_DASH_CENC: return "hevc-main-L30-dash-cenc";
    case HEVC_MAIN_L31_DASH_CENC: return "hevc-main-L31-dash-cenc";
    case HEVC_MAIN_L40_DASH_CENC: return "hevc-main-L40-dash-cenc";
    case HEVC_MAIN_L41_DASH_CENC: return "hevc-main-L41-dash-cenc";
    case HEVC_MAIN_L50_DASH_CENC: return "hevc-main-L50-dash-cenc";
    case HEVC_MAIN_L51_DASH_CENC: return "hevc-main-L51-dash-cenc";

    case HEVC_MAIN_L20_L21_DASH_CENC_TL: return "hevc-main-L20-L21-dash-cenc-tl";
    case HEVC_MAIN_L21_L30_DASH_CENC_TL: return "hevc-main-L21-L30-dash-cenc-tl";
    case HEVC_MAIN_L30_L31_DASH_CENC_TL: return "hevc-main-L30-L31-dash-cenc-tl";
    case HEVC_MAIN_L31_L40_DASH_CENC_TL: return "hevc-main-L31-L40-dash-cenc-tl";
    case HEVC_MAIN_L40_L41_DASH_CENC_TL: return "hevc-main-L40-L41-dash-cenc-tl";
    case HEVC_MAIN_L50_L51_DASH_CENC_TL: return "hevc-main-L50-L51-dash-cenc-tl";

    case HEVC_MAIN10_L20_DASH_CENC: return "hevc-main10-L20-dash-cenc";
    case HEVC_MAIN10_L21_DASH_CENC: return "hevc-main10-L21-dash-cenc";
    case HEVC_MAIN10_L30_DASH_CENC: return "hevc-main10-L30-dash-cenc";
    case HEVC_MAIN10_L31_DASH_CENC: return "hevc-main10-L31-dash-cenc";
    case HEVC_MAIN10_L40_DASH_CENC: return "hevc-main10-L40-dash-cenc";
    case HEVC_MAIN10_L41_DASH_CENC: return "hevc-main10-L41-dash-cenc";
    case HEVC_MAIN10_L50_DASH_CENC: return "hevc-main10-L50-dash-cenc";
    case HEVC_MAIN10_L51_DASH_CENC: return "hevc-main10-L51-dash-cenc";
	
    case HEVC_MAIN10_L20_L21_DASH_CENC_TL: return "hevc-main10-L20-L21-dash-cenc-tl";
    case HEVC_MAIN10_L21_L30_DASH_CENC_TL: return "hevc-main10-L21-L30-dash-cenc-tl";
    case HEVC_MAIN10_L30_L31_DASH_CENC_TL: return "hevc-main10-L30-L31-dash-cenc-tl";
    case HEVC_MAIN10_L31_L40_DASH_CENC_TL: return "hevc-main10-L31-L40-dash-cenc-tl";
    case HEVC_MAIN10_L40_L41_DASH_CENC_TL: return "hevc-main10-L40-L41-dash-cenc-tl";
    case HEVC_MAIN10_L50_L51_DASH_CENC_TL: return "hevc-main10-L50-L51-dash-cenc-tl";
#ifdef NRDP_HAS_HDR_VIDEO
    case HEVC_DV_MAIN10_L20_DASH_CENC: return "hevc-dv-main10-L20-dash-cenc";
    case HEVC_DV_MAIN10_L21_DASH_CENC: return "hevc-dv-main10-L21-dash-cenc";
    case HEVC_DV_MAIN10_L30_DASH_CENC: return "hevc-dv-main10-L30-dash-cenc";
    case HEVC_DV_MAIN10_L31_DASH_CENC: return "hevc-dv-main10-L31-dash-cenc";
    case HEVC_DV_MAIN10_L40_DASH_CENC: return "hevc-dv-main10-L40-dash-cenc";
    case HEVC_DV_MAIN10_L41_DASH_CENC: return "hevc-dv-main10-L41-dash-cenc";
    case HEVC_DV_MAIN10_L50_DASH_CENC: return "hevc-dv-main10-L50-dash-cenc";
    case HEVC_DV_MAIN10_L51_DASH_CENC: return "hevc-dv-main10-L51-dash-cenc";

    case HEVC_HDR_MAIN10_L30_DASH_CENC: return "hevc-hdr-main10-L30-dash-cenc";
    case HEVC_HDR_MAIN10_L31_DASH_CENC: return "hevc-hdr-main10-L31-dash-cenc";
    case HEVC_HDR_MAIN10_L40_DASH_CENC: return "hevc-hdr-main10-L40-dash-cenc";
    case HEVC_HDR_MAIN10_L41_DASH_CENC: return "hevc-hdr-main10-L41-dash-cenc";
    case HEVC_HDR_MAIN10_L50_DASH_CENC: return "hevc-hdr-main10-L50-dash-cenc";
    case HEVC_HDR_MAIN10_L51_DASH_CENC: return "hevc-hdr-main10-L51-dash-cenc";
#endif
    case AUDIO_DDPLUS_5_1_PIFF:            return "ddplus-5.1-piff";
    case AUDIO_HEAAC_2_PIFF:               return "heaac-2-piff";
    case AUDIO_PLAYREADY_OGGVORBIS_2_PIFF: return "playready-oggvorbis-2-piff";
    case AUDIO_PLAYREADY_OGGVORBIS_2_DASH: return "playready-oggvorbis-2-dash";
    case AUDIO_DDPLUS_5_1_DASH:            return "ddplus-5.1-dash";
    case AUDIO_HEAAC_2_DASH:               return "heaac-2-dash";
    case AUDIO_PLAYREADY_HEAAC_2_DASH:     return "playready-heaac-2-dash";
    case AUDIO_HEACC_2_ELEM:               return "heaac-2-elem";
    case AUDIO_DD_5_1_ELEM:                return "dd-5.1-elem";
    case AUDIO_HEACC_2_M2TS:               return "heaac-2-m2ts";
    case AUDIO_DD_5_1_M2TS:                return "dd-5.1-m2ts";

    case TIMEDTEXT_SIMPLE_SDH1:            return "simplesdh";

    case TIMEDTEXT_DFXP_LS_SDH:            return "dfxp-ls-sdh";

    default:                               return "";
    }
}

inline std::string profileToMimeType(ContentProfile p)
{
    switch (p)
    {
    case VIDEO_PLAYREADY_MPEG4P2_PIFF:
        return "video/mp4";

    case VIDEO_PLAYREADY_H264_BPL30_PIFF:
    case VIDEO_PLAYREADY_H264_MPL30_PIFF:
    case VIDEO_PLAYREADY_H264_MPL31_PIFF:
    case VIDEO_PLAYREADY_H264_MPL40_PIFF:
    case VIDEO_PLAYREADY_H264_BPL30_DASH:
    case VIDEO_PLAYREADY_H264_MPL30_DASH:
    case VIDEO_PLAYREADY_H264_MPL31_DASH:
    case VIDEO_PLAYREADY_H264_MPL40_DASH:
    case VIDEO_PLAYREADY_H264_SHPL30_DASH:
    case VIDEO_PLAYREADY_H264_SHPL31_DASH:
    case VIDEO_PLAYREADY_H264_SHPL40_DASH:
    case VIDEO_NONE_H264_BPL13_DASH:
    case VIDEO_NONE_H264_MPL30_DASH:
    case VIDEO_NONE_H264_MPL31_DASH:
    case VIDEO_NONE_H264_MPL40_DASH:

    case HEVC_MAIN_L20_DASH_CENC:
    case HEVC_MAIN_L21_DASH_CENC:
    case HEVC_MAIN_L30_DASH_CENC:
    case HEVC_MAIN_L31_DASH_CENC:
    case HEVC_MAIN_L40_DASH_CENC:
    case HEVC_MAIN_L41_DASH_CENC:
    case HEVC_MAIN_L50_DASH_CENC:
    case HEVC_MAIN_L51_DASH_CENC:

    case HEVC_MAIN_L20_L21_DASH_CENC_TL:
    case HEVC_MAIN_L21_L30_DASH_CENC_TL:
    case HEVC_MAIN_L30_L31_DASH_CENC_TL:
    case HEVC_MAIN_L31_L40_DASH_CENC_TL:
    case HEVC_MAIN_L40_L41_DASH_CENC_TL:
    case HEVC_MAIN_L50_L51_DASH_CENC_TL:

    case HEVC_MAIN10_L20_DASH_CENC:
    case HEVC_MAIN10_L21_DASH_CENC:
    case HEVC_MAIN10_L30_DASH_CENC:
    case HEVC_MAIN10_L31_DASH_CENC:
    case HEVC_MAIN10_L40_DASH_CENC:
    case HEVC_MAIN10_L41_DASH_CENC:
    case HEVC_MAIN10_L50_DASH_CENC:
    case HEVC_MAIN10_L51_DASH_CENC:
	
    case HEVC_MAIN10_L20_L21_DASH_CENC_TL:
    case HEVC_MAIN10_L21_L30_DASH_CENC_TL:
    case HEVC_MAIN10_L30_L31_DASH_CENC_TL:
    case HEVC_MAIN10_L31_L40_DASH_CENC_TL:
    case HEVC_MAIN10_L40_L41_DASH_CENC_TL:
    case HEVC_MAIN10_L50_L51_DASH_CENC_TL:
#ifdef NRDP_HAS_HDR_VIDEO
    case HEVC_DV_MAIN10_L20_DASH_CENC:
    case HEVC_DV_MAIN10_L21_DASH_CENC:
    case HEVC_DV_MAIN10_L30_DASH_CENC:
    case HEVC_DV_MAIN10_L31_DASH_CENC:
    case HEVC_DV_MAIN10_L40_DASH_CENC:
    case HEVC_DV_MAIN10_L41_DASH_CENC:
    case HEVC_DV_MAIN10_L50_DASH_CENC:
    case HEVC_DV_MAIN10_L51_DASH_CENC:

    case HEVC_HDR_MAIN10_L30_DASH_CENC:
    case HEVC_HDR_MAIN10_L31_DASH_CENC:
    case HEVC_HDR_MAIN10_L40_DASH_CENC:
    case HEVC_HDR_MAIN10_L41_DASH_CENC:
    case HEVC_HDR_MAIN10_L50_DASH_CENC:
    case HEVC_HDR_MAIN10_L51_DASH_CENC:
#endif
        return "video/x-nflx-mp4";

    case VIDEO_PLAYREADY_H264MPL30_M2TS:
    case VIDEO_PLAYREADY_H264MPL31_M2TS:
    case VIDEO_PLAYREADY_H264MPL40_M2TS:
    case VIDEO_PLAYREADY_H264MPL13_M2TS:
        return "video/mpeg";

    case AUDIO_HEACC_2_ELEM:
    case AUDIO_DD_5_1_ELEM:
    case AUDIO_HEACC_2_M2TS:
    case AUDIO_DD_5_1_M2TS:
        return "audio/mpeg";

    case AUDIO_PLAYREADY_OGGVORBIS_2_PIFF:
    case AUDIO_PLAYREADY_OGGVORBIS_2_DASH:
        return "audio/ogg;codec=";

    case AUDIO_DDPLUS_5_1_PIFF:
    case AUDIO_DDPLUS_5_1_DASH:
        return "audio/mp4";

    case AUDIO_HEAAC_2_PIFF:
    case AUDIO_HEAAC_2_DASH:
    case AUDIO_PLAYREADY_HEAAC_2_DASH:
        return "audio/mp4";

    case TIMEDTEXT_SIMPLE_SDH1:
        return "text/simplesdh";

    case TIMEDTEXT_DFXP_LS_SDH:
        return "text/dfxp-ls-sdh";

    default:
        return "";
    }
}

inline std::string profileToMimeCodec(ContentProfile p)
{
    switch (p)
    {
    case VIDEO_PLAYREADY_MPEG4P2_PIFF:
        return "mp4v.20";

    case VIDEO_NONE_H264_BPL13_DASH:
    case VIDEO_PLAYREADY_H264_BPL30_PIFF:
    case VIDEO_PLAYREADY_H264_BPL30_DASH:
        return "avc1.42001E";

    case VIDEO_PLAYREADY_H264_MPL30_PIFF:
    case VIDEO_PLAYREADY_H264_MPL30_DASH:
    case VIDEO_NONE_H264_MPL30_DASH:
        return "avc1.4D021E";

    case VIDEO_PLAYREADY_H264_MPL31_PIFF:
    case VIDEO_PLAYREADY_H264_MPL31_DASH:
    case VIDEO_NONE_H264_MPL31_DASH:
        return "avc1.4D021F";

    case VIDEO_PLAYREADY_H264_MPL40_PIFF:
    case VIDEO_PLAYREADY_H264_MPL40_DASH:
    case VIDEO_NONE_H264_MPL40_DASH:
        return "avc1.4D0228";

    case VIDEO_PLAYREADY_H264_SHPL30_DASH:
        return "avc1.64001E,avc1.80001E";

    case VIDEO_PLAYREADY_H264_SHPL31_DASH:
        return "avc1.64001F,avc1.80001F";

    case VIDEO_PLAYREADY_H264_SHPL40_DASH:
        return "avc1.640028,avc1.800028";

    case VIDEO_PLAYREADY_H264MPL13_M2TS:
        return "avc1.42000D";

    case VIDEO_PLAYREADY_H264MPL30_M2TS:
        return "avc1.4D021E";

    case VIDEO_PLAYREADY_H264MPL31_M2TS:
        return "avc1.4D021F";

    case VIDEO_PLAYREADY_H264MPL40_M2TS:
        return "avc1.4D4028";

        /*
         * 1. "hev1" or "hvc1"
         * 2. "." +
         *    profile_space: no charactor for 0, A for 1, B for 2, C for 3
         *        profile_space is 0 for ISO-23008-2: 201x version.
         *    profile_idc in decimal number
         *        profile_idc is 1 for main, 2 for main10
         * 3. "." +
         *    profile compatibility in hexadecimal (leading zeros omitted)
         * 4. "." +
         *    tier_flag : L for 0, H for 1
         *    level_idc in decimal number
         * 5. each of 6 bytes of the constraint flags,
         *    starting from the byte containing general_progressive_source_flag
         *    each encoded as hexadecimal, each byte separated by "."
         *    ( We are not setting this 6 bytes in mime codec as of 13.2.7.)
         */
    // main
    case HEVC_MAIN_L20_DASH_CENC:
        return "hev1.1.0.L60";

    case HEVC_MAIN_L21_DASH_CENC:
        return "hev1.1.0.L63";

    case HEVC_MAIN_L30_DASH_CENC:
        return "hev1.1.0.L90";

    case HEVC_MAIN_L31_DASH_CENC:
        return "hev1.1.0.L93";

    case HEVC_MAIN_L40_DASH_CENC:
        return "hev1.1.0.L120";

    case HEVC_MAIN_L41_DASH_CENC:
        return "hev1.1.0.L123";

    case HEVC_MAIN_L50_DASH_CENC:
        return "hev1.1.0.L150";

    case HEVC_MAIN_L51_DASH_CENC:
        return "hev1.1.0.L153";

    case HEVC_MAIN_L20_L21_DASH_CENC_TL:
        return "hev1.1.0.L63";

    case HEVC_MAIN_L21_L30_DASH_CENC_TL:
        return "hev1.1.0.L90";

    case HEVC_MAIN_L30_L31_DASH_CENC_TL:
        return "hev1.1.0.L93";

    case HEVC_MAIN_L31_L40_DASH_CENC_TL:
        return "hev1.1.0.L120";

    case HEVC_MAIN_L40_L41_DASH_CENC_TL:
        return "hev1.1.0.L123";

    case HEVC_MAIN_L50_L51_DASH_CENC_TL:
        return "hev1.1.0.L153";

    // main10
    case HEVC_MAIN10_L20_DASH_CENC:
        return "hev1.2.0.L60";

    case HEVC_MAIN10_L21_DASH_CENC:
        return "hev1.2.0.L63";

    case HEVC_MAIN10_L30_DASH_CENC:
        return "hev1.2.0.L90";

    case HEVC_MAIN10_L31_DASH_CENC:
        return "hev1.2.0.L93";

    case HEVC_MAIN10_L40_DASH_CENC:
        return "hev1.2.0.L120";

    case HEVC_MAIN10_L41_DASH_CENC:
        return "hev1.2.0.L123";

    case HEVC_MAIN10_L50_DASH_CENC:
        return "hev1.2.0.L150";

    case HEVC_MAIN10_L51_DASH_CENC:
	return "hev1.2.0.L153";

    case HEVC_MAIN10_L20_L21_DASH_CENC_TL:
        return "hev1.2.0.L63";

    case HEVC_MAIN10_L21_L30_DASH_CENC_TL:
        return "hev1.2.0.L90";

    case HEVC_MAIN10_L30_L31_DASH_CENC_TL:
        return "hev1.2.0.L93";

    case HEVC_MAIN10_L31_L40_DASH_CENC_TL:
        return "hev1.2.0.L120";

    case HEVC_MAIN10_L40_L41_DASH_CENC_TL:
        return "hev1.2.0.L123";

    case HEVC_MAIN10_L50_L51_DASH_CENC_TL:
        return "hev1.2.0.L153";
#ifdef NRDP_HAS_HDR_VIDEO
    case HEVC_DV_MAIN10_L20_DASH_CENC:
    case HEVC_DV_MAIN10_L21_DASH_CENC:
    case HEVC_DV_MAIN10_L30_DASH_CENC:
    case HEVC_DV_MAIN10_L31_DASH_CENC:
    case HEVC_DV_MAIN10_L40_DASH_CENC:
    case HEVC_DV_MAIN10_L41_DASH_CENC:
    case HEVC_DV_MAIN10_L50_DASH_CENC:
    case HEVC_DV_MAIN10_L51_DASH_CENC:
        return "dvhe";

    case HEVC_HDR_MAIN10_L30_DASH_CENC:
    case HEVC_HDR_MAIN10_L31_DASH_CENC:
    case HEVC_HDR_MAIN10_L40_DASH_CENC:
    case HEVC_HDR_MAIN10_L41_DASH_CENC:
    case HEVC_HDR_MAIN10_L50_DASH_CENC:
    case HEVC_HDR_MAIN10_L51_DASH_CENC:
        return "hdra";
#endif
    case AUDIO_PLAYREADY_OGGVORBIS_2_PIFF:
    case AUDIO_PLAYREADY_OGGVORBIS_2_DASH:
        return "ovrb";

    case AUDIO_DDPLUS_5_1_PIFF:
    case AUDIO_DDPLUS_5_1_DASH:
        return "ec-3";

    case AUDIO_HEACC_2_ELEM:
    case AUDIO_HEACC_2_M2TS:
    case AUDIO_HEAAC_2_PIFF:
    case AUDIO_HEAAC_2_DASH:
    case AUDIO_PLAYREADY_HEAAC_2_DASH:
        return "mp4a.40.2";

    case AUDIO_DD_5_1_ELEM:
    case AUDIO_DD_5_1_M2TS:
        return "ac-3";

    case TIMEDTEXT_SIMPLE_SDH1:
        return "simplesdh";

    case TIMEDTEXT_DFXP_LS_SDH:
        return "dfxp-ls-sdh";

    default:
        return "";
    }
}

enum StreamingType
{
    NORMAL_PLAY = 0, // drm playback only
    FAST_PLAY, // a.k.a McQueen. Start from non-drm and transition to drm later
    NONDRM_PLAY // non-drm playback only
};

enum DrmType
{
    NO_DRM = -1,
    PLAY_READY,
    SESSION_DH1,
    WIDEVINE_CEF
};
inline DrmType profileDrmType(ContentProfile p)
{
    switch (p)
    {
    case VIDEO_PLAYREADY_MPEG4P2_PIFF:
    case VIDEO_PLAYREADY_H264_BPL30_PIFF:
    case VIDEO_PLAYREADY_H264_MPL30_PIFF:
    case VIDEO_PLAYREADY_H264_MPL31_PIFF:
    case VIDEO_PLAYREADY_H264_MPL40_PIFF:
    case AUDIO_PLAYREADY_OGGVORBIS_2_PIFF:
    case AUDIO_PLAYREADY_OGGVORBIS_2_DASH:
    case VIDEO_PLAYREADY_H264_BPL30_DASH:
    case VIDEO_PLAYREADY_H264_MPL30_DASH:
    case VIDEO_PLAYREADY_H264_MPL31_DASH:
    case VIDEO_PLAYREADY_H264_MPL40_DASH:
    case VIDEO_PLAYREADY_H264_SHPL30_DASH:
    case VIDEO_PLAYREADY_H264_SHPL31_DASH:
    case VIDEO_PLAYREADY_H264_SHPL40_DASH:
    case AUDIO_PLAYREADY_HEAAC_2_DASH:
    case VIDEO_PLAYREADY_H264MPL30_M2TS:
    case VIDEO_PLAYREADY_H264MPL31_M2TS:
    case VIDEO_PLAYREADY_H264MPL40_M2TS:
    case VIDEO_PLAYREADY_H264MPL13_M2TS:

    // FIXME: do we need other DRM definition for Common Encryption?
    case HEVC_MAIN_L20_DASH_CENC:
    case HEVC_MAIN_L21_DASH_CENC:
    case HEVC_MAIN_L30_DASH_CENC:
    case HEVC_MAIN_L31_DASH_CENC:
    case HEVC_MAIN_L40_DASH_CENC:
    case HEVC_MAIN_L41_DASH_CENC:
    case HEVC_MAIN_L50_DASH_CENC:
    case HEVC_MAIN_L51_DASH_CENC:

    case HEVC_MAIN_L20_L21_DASH_CENC_TL:
    case HEVC_MAIN_L21_L30_DASH_CENC_TL:
    case HEVC_MAIN_L30_L31_DASH_CENC_TL:
    case HEVC_MAIN_L31_L40_DASH_CENC_TL:
    case HEVC_MAIN_L40_L41_DASH_CENC_TL:
    case HEVC_MAIN_L50_L51_DASH_CENC_TL:

    case HEVC_MAIN10_L20_DASH_CENC:
    case HEVC_MAIN10_L21_DASH_CENC:
    case HEVC_MAIN10_L30_DASH_CENC:
    case HEVC_MAIN10_L31_DASH_CENC:
    case HEVC_MAIN10_L40_DASH_CENC:
    case HEVC_MAIN10_L41_DASH_CENC:
    case HEVC_MAIN10_L50_DASH_CENC:
    case HEVC_MAIN10_L51_DASH_CENC:
	
    case HEVC_MAIN10_L20_L21_DASH_CENC_TL:
    case HEVC_MAIN10_L21_L30_DASH_CENC_TL:
    case HEVC_MAIN10_L30_L31_DASH_CENC_TL:
    case HEVC_MAIN10_L31_L40_DASH_CENC_TL:
    case HEVC_MAIN10_L40_L41_DASH_CENC_TL:
    case HEVC_MAIN10_L50_L51_DASH_CENC_TL:
#ifdef NRDP_HAS_HDR_VIDEO
    case HEVC_DV_MAIN10_L20_DASH_CENC:
    case HEVC_DV_MAIN10_L21_DASH_CENC:
    case HEVC_DV_MAIN10_L30_DASH_CENC:
    case HEVC_DV_MAIN10_L31_DASH_CENC:
    case HEVC_DV_MAIN10_L40_DASH_CENC:
    case HEVC_DV_MAIN10_L41_DASH_CENC:
    case HEVC_DV_MAIN10_L50_DASH_CENC:
    case HEVC_DV_MAIN10_L51_DASH_CENC:

    case HEVC_HDR_MAIN10_L30_DASH_CENC:
    case HEVC_HDR_MAIN10_L31_DASH_CENC:
    case HEVC_HDR_MAIN10_L40_DASH_CENC:
    case HEVC_HDR_MAIN10_L41_DASH_CENC:
    case HEVC_HDR_MAIN10_L50_DASH_CENC:
    case HEVC_HDR_MAIN10_L51_DASH_CENC:
#endif
        return PLAY_READY;

    case VIDEO_NONE_H264_BPL13_DASH:
    case VIDEO_NONE_H264_MPL30_DASH:
    case VIDEO_NONE_H264_MPL31_DASH:
    case VIDEO_NONE_H264_MPL40_DASH:
    case AUDIO_DDPLUS_5_1_PIFF:
    case AUDIO_HEAAC_2_PIFF:
    case AUDIO_DDPLUS_5_1_DASH:
    case AUDIO_HEAAC_2_DASH:
    case AUDIO_HEACC_2_ELEM:
    case AUDIO_DD_5_1_ELEM:
    case AUDIO_HEACC_2_M2TS:
    case AUDIO_DD_5_1_M2TS:
        return NO_DRM;

    default:
        return NO_DRM;
    }
}

inline std::string drmTypeToString(DrmType t)
{
    switch(t)
    {
        case PLAY_READY:    return "PlayReady";
        case SESSION_DH1:   return "SessionDH1";
        case WIDEVINE_CEF:  return "WidevineCEF";
        default:            return "";
    }
}

inline DrmType stringToDrmType(const std::string &s)
{
    if (s == "PlayReady")   return PLAY_READY;
    if (s == "SessionDH1")  return SESSION_DH1;
    if (s == "WidevineCEF") return WIDEVINE_CEF;
    return NO_DRM;
}

enum ContainerType
{
    UNKNOWN_CONTAINER = -1,
    PIFF_CONTAINER,
    DASH_CONTAINER,
    HLS_UNMUXED_CONTAINER
};

inline ContainerType profileContainerType(ContentProfile p)
{
    switch(p)
    {
    case VIDEO_PLAYREADY_MPEG4P2_PIFF:
    case VIDEO_PLAYREADY_H264_BPL30_PIFF:
    case VIDEO_PLAYREADY_H264_MPL30_PIFF:
    case VIDEO_PLAYREADY_H264_MPL31_PIFF:
    case VIDEO_PLAYREADY_H264_MPL40_PIFF:
    case AUDIO_HEAAC_2_PIFF:
    case AUDIO_DDPLUS_5_1_PIFF:
    case AUDIO_PLAYREADY_OGGVORBIS_2_PIFF:
        return PIFF_CONTAINER;

    case VIDEO_PLAYREADY_H264_BPL30_DASH:
    case VIDEO_PLAYREADY_H264_MPL30_DASH:
    case VIDEO_PLAYREADY_H264_MPL31_DASH:
    case VIDEO_PLAYREADY_H264_MPL40_DASH:
    case VIDEO_PLAYREADY_H264_SHPL30_DASH:
    case VIDEO_PLAYREADY_H264_SHPL31_DASH:
    case VIDEO_PLAYREADY_H264_SHPL40_DASH:
    case VIDEO_NONE_H264_BPL13_DASH:
    case VIDEO_NONE_H264_MPL30_DASH:
    case VIDEO_NONE_H264_MPL31_DASH:
    case VIDEO_NONE_H264_MPL40_DASH:
    case AUDIO_DDPLUS_5_1_DASH:
    case AUDIO_HEAAC_2_DASH:
    case AUDIO_PLAYREADY_HEAAC_2_DASH:
    case AUDIO_PLAYREADY_OGGVORBIS_2_DASH:

    case HEVC_MAIN_L20_DASH_CENC:
    case HEVC_MAIN_L21_DASH_CENC:
    case HEVC_MAIN_L30_DASH_CENC:
    case HEVC_MAIN_L31_DASH_CENC:
    case HEVC_MAIN_L40_DASH_CENC:
    case HEVC_MAIN_L41_DASH_CENC:
    case HEVC_MAIN_L50_DASH_CENC:
    case HEVC_MAIN_L51_DASH_CENC:

    case HEVC_MAIN_L20_L21_DASH_CENC_TL:
    case HEVC_MAIN_L21_L30_DASH_CENC_TL:
    case HEVC_MAIN_L30_L31_DASH_CENC_TL:
    case HEVC_MAIN_L31_L40_DASH_CENC_TL:
    case HEVC_MAIN_L40_L41_DASH_CENC_TL:
    case HEVC_MAIN_L50_L51_DASH_CENC_TL:

    case HEVC_MAIN10_L20_DASH_CENC:
    case HEVC_MAIN10_L21_DASH_CENC:
    case HEVC_MAIN10_L30_DASH_CENC:
    case HEVC_MAIN10_L31_DASH_CENC:
    case HEVC_MAIN10_L40_DASH_CENC:
    case HEVC_MAIN10_L41_DASH_CENC:
    case HEVC_MAIN10_L50_DASH_CENC:
    case HEVC_MAIN10_L51_DASH_CENC:
	
    case HEVC_MAIN10_L20_L21_DASH_CENC_TL:
    case HEVC_MAIN10_L21_L30_DASH_CENC_TL:
    case HEVC_MAIN10_L30_L31_DASH_CENC_TL:
    case HEVC_MAIN10_L31_L40_DASH_CENC_TL:
    case HEVC_MAIN10_L40_L41_DASH_CENC_TL:
    case HEVC_MAIN10_L50_L51_DASH_CENC_TL:
#ifdef NRDP_HAS_HDR_VIDEO
    case HEVC_DV_MAIN10_L20_DASH_CENC:
    case HEVC_DV_MAIN10_L21_DASH_CENC:
    case HEVC_DV_MAIN10_L30_DASH_CENC:
    case HEVC_DV_MAIN10_L31_DASH_CENC:
    case HEVC_DV_MAIN10_L40_DASH_CENC:
    case HEVC_DV_MAIN10_L41_DASH_CENC:
    case HEVC_DV_MAIN10_L50_DASH_CENC:
    case HEVC_DV_MAIN10_L51_DASH_CENC:

    case HEVC_HDR_MAIN10_L30_DASH_CENC:
    case HEVC_HDR_MAIN10_L31_DASH_CENC:
    case HEVC_HDR_MAIN10_L40_DASH_CENC:
    case HEVC_HDR_MAIN10_L41_DASH_CENC:
    case HEVC_HDR_MAIN10_L50_DASH_CENC:
    case HEVC_HDR_MAIN10_L51_DASH_CENC:
#endif
        return DASH_CONTAINER;

    case VIDEO_PLAYREADY_H264MPL30_M2TS:
    case VIDEO_PLAYREADY_H264MPL31_M2TS:
    case VIDEO_PLAYREADY_H264MPL40_M2TS:
    case VIDEO_PLAYREADY_H264MPL13_M2TS:
    case AUDIO_HEACC_2_ELEM:
    case AUDIO_DD_5_1_ELEM:
    case AUDIO_HEACC_2_M2TS:
    case AUDIO_DD_5_1_M2TS:
        return HLS_UNMUXED_CONTAINER;

    default:
        return UNKNOWN_CONTAINER;
    }
}

enum AudioCodecType
{
    UNKNOWN_AUDIO_CODEC = -1,
    WMA_AUDIO,
    OGG_VORBIS_AUDIO,
    HEAAC_AUDIO,
    DDPLUS_AUDIO,
    DD_AUDIO,
    WAV_AUDIO,
    MP3_AUDIO
};

inline AudioCodecType profileAudioCodecType(ContentProfile p)
{
    switch(p)
    {
        case AUDIO_PLAYREADY_OGGVORBIS_2_PIFF:
        case AUDIO_PLAYREADY_OGGVORBIS_2_DASH:
            return OGG_VORBIS_AUDIO;

        case AUDIO_HEAAC_2_PIFF:
        case AUDIO_HEAAC_2_DASH:
        case AUDIO_PLAYREADY_HEAAC_2_DASH:
        case AUDIO_HEACC_2_ELEM:
        case AUDIO_HEACC_2_M2TS:
            return HEAAC_AUDIO;

        case AUDIO_DDPLUS_5_1_PIFF:
        case AUDIO_DDPLUS_5_1_DASH:
            return DDPLUS_AUDIO;

        case AUDIO_DD_5_1_ELEM:
        case AUDIO_DD_5_1_M2TS:
            return DD_AUDIO;

        default:
            return UNKNOWN_AUDIO_CODEC;
    }
}

inline std::string audioCodecTypeToString(AudioCodecType t)
{
    switch(t)
    {
        case WMA_AUDIO:        return "WMA";
        case OGG_VORBIS_AUDIO: return "OGG_VORBIS";
        case HEAAC_AUDIO:      return "AAC";
        case DDPLUS_AUDIO:     return "DDPLUS";
        case DD_AUDIO:         return "DD";
        case WAV_AUDIO:        return "WAV";
        case MP3_AUDIO:        return "MP3";
        default:               return "";
    }
}

enum VideoCodecType
{
    UNKNOWN_VIDEO_CODEC = -1,
    VC1AP_VIDEO,
    MPEG4P2_VIDEO,
    H264_VIDEO,
    H265_VIDEO,
    H265_10BIT_VIDEO,
    DOLBYVISION_HDR_VIDEO,
    HDRA_HDR_VIDEO
};

inline VideoCodecType profileVideoCodecType(ContentProfile p)
{
    switch(p)
    {
    case VIDEO_PLAYREADY_MPEG4P2_PIFF:
        return MPEG4P2_VIDEO;

    case VIDEO_PLAYREADY_H264_BPL30_PIFF:
    case VIDEO_PLAYREADY_H264_MPL30_PIFF:
    case VIDEO_PLAYREADY_H264_MPL31_PIFF:
    case VIDEO_PLAYREADY_H264_MPL40_PIFF:
    case VIDEO_PLAYREADY_H264_BPL30_DASH:
    case VIDEO_PLAYREADY_H264_MPL30_DASH:
    case VIDEO_PLAYREADY_H264_MPL31_DASH:
    case VIDEO_PLAYREADY_H264_MPL40_DASH:
    case VIDEO_PLAYREADY_H264_SHPL30_DASH:
    case VIDEO_PLAYREADY_H264_SHPL31_DASH:
    case VIDEO_PLAYREADY_H264_SHPL40_DASH:
    case VIDEO_NONE_H264_BPL13_DASH:
    case VIDEO_NONE_H264_MPL30_DASH:
    case VIDEO_NONE_H264_MPL31_DASH:
    case VIDEO_NONE_H264_MPL40_DASH:
    case VIDEO_PLAYREADY_H264MPL30_M2TS:
    case VIDEO_PLAYREADY_H264MPL31_M2TS:
    case VIDEO_PLAYREADY_H264MPL40_M2TS:
    case VIDEO_PLAYREADY_H264MPL13_M2TS:
        return H264_VIDEO;

    case HEVC_MAIN_L20_DASH_CENC:
    case HEVC_MAIN_L21_DASH_CENC:
    case HEVC_MAIN_L30_DASH_CENC:
    case HEVC_MAIN_L31_DASH_CENC:
    case HEVC_MAIN_L40_DASH_CENC:
    case HEVC_MAIN_L41_DASH_CENC:
    case HEVC_MAIN_L50_DASH_CENC:
    case HEVC_MAIN_L51_DASH_CENC:

    case HEVC_MAIN_L20_L21_DASH_CENC_TL:
    case HEVC_MAIN_L21_L30_DASH_CENC_TL:
    case HEVC_MAIN_L30_L31_DASH_CENC_TL:
    case HEVC_MAIN_L31_L40_DASH_CENC_TL:
    case HEVC_MAIN_L40_L41_DASH_CENC_TL:
    case HEVC_MAIN_L50_L51_DASH_CENC_TL:
        return H265_VIDEO;

    case HEVC_MAIN10_L20_DASH_CENC:
    case HEVC_MAIN10_L21_DASH_CENC:
    case HEVC_MAIN10_L30_DASH_CENC:
    case HEVC_MAIN10_L31_DASH_CENC:
    case HEVC_MAIN10_L40_DASH_CENC:
    case HEVC_MAIN10_L41_DASH_CENC:
    case HEVC_MAIN10_L50_DASH_CENC:
    case HEVC_MAIN10_L51_DASH_CENC:
	
    case HEVC_MAIN10_L20_L21_DASH_CENC_TL:
    case HEVC_MAIN10_L21_L30_DASH_CENC_TL:
    case HEVC_MAIN10_L30_L31_DASH_CENC_TL:
    case HEVC_MAIN10_L31_L40_DASH_CENC_TL:
    case HEVC_MAIN10_L40_L41_DASH_CENC_TL:
    case HEVC_MAIN10_L50_L51_DASH_CENC_TL:
        return H265_10BIT_VIDEO;
#ifdef NRDP_HAS_HDR_VIDEO
    case HEVC_DV_MAIN10_L20_DASH_CENC:
    case HEVC_DV_MAIN10_L21_DASH_CENC:
    case HEVC_DV_MAIN10_L30_DASH_CENC:
    case HEVC_DV_MAIN10_L31_DASH_CENC:
    case HEVC_DV_MAIN10_L40_DASH_CENC:
    case HEVC_DV_MAIN10_L41_DASH_CENC:
    case HEVC_DV_MAIN10_L50_DASH_CENC:
    case HEVC_DV_MAIN10_L51_DASH_CENC:
        return DOLBYVISION_HDR_VIDEO;

    case HEVC_HDR_MAIN10_L30_DASH_CENC:
    case HEVC_HDR_MAIN10_L31_DASH_CENC:
    case HEVC_HDR_MAIN10_L40_DASH_CENC:
    case HEVC_HDR_MAIN10_L41_DASH_CENC:
    case HEVC_HDR_MAIN10_L50_DASH_CENC:
    case HEVC_HDR_MAIN10_L51_DASH_CENC:
        return HDRA_HDR_VIDEO;
#endif
    default:
        return UNKNOWN_VIDEO_CODEC;
    }
}

// Returns true if the function handles the frame dimensions for the profile
// type. Returns false o.w.
inline bool getMaxFrameDimensionsForVideoProfile(ContentProfile profile,
                                                 uint32_t& maxWidthForProfile,
                                                 uint32_t& maxHeightForProfile)
{
    switch(profile){

    /*
     * H.264 Baseline
     */
    case VIDEO_NONE_H264_BPL13_DASH:
        maxWidthForProfile = 320;
        maxHeightForProfile = 240;
        break;

    case VIDEO_PLAYREADY_H264_BPL30_PIFF:
    case VIDEO_PLAYREADY_H264_BPL30_DASH:
        maxWidthForProfile = 640;
        maxHeightForProfile = 480;
        break;

    /*
     * H.264 Main or MVC
     */
    case VIDEO_PLAYREADY_H264_MPL30_PIFF:
    case VIDEO_PLAYREADY_H264_MPL30_DASH:
    case VIDEO_PLAYREADY_H264_SHPL30_DASH:
    case VIDEO_NONE_H264_MPL30_DASH:
        maxWidthForProfile = 720;
        maxHeightForProfile = 480;
        break;

    case VIDEO_PLAYREADY_H264_MPL31_PIFF:
    case VIDEO_PLAYREADY_H264_MPL31_DASH:
    case VIDEO_PLAYREADY_H264_SHPL31_DASH:
    case VIDEO_NONE_H264_MPL31_DASH:
        maxWidthForProfile = 1280;
        maxHeightForProfile = 720;
        break;

    case VIDEO_PLAYREADY_H264_MPL40_PIFF:
    case VIDEO_PLAYREADY_H264_MPL40_DASH:
    case VIDEO_PLAYREADY_H264_SHPL40_DASH:
    case VIDEO_NONE_H264_MPL40_DASH:
        maxWidthForProfile = 1920;
        maxHeightForProfile = 1080;
        break;

    /*
     * H.265 MAIN/MAIN10
     */
    case HEVC_MAIN_L20_DASH_CENC:
    case HEVC_MAIN_L20_L21_DASH_CENC_TL:
    case HEVC_MAIN10_L20_DASH_CENC:
    case HEVC_MAIN10_L20_L21_DASH_CENC_TL:
#ifdef NRDP_HAS_HDR_VIDEO
    case HEVC_DV_MAIN10_L20_DASH_CENC:
#endif
        maxWidthForProfile = 240;
        maxHeightForProfile = 180;
        break;

    case HEVC_MAIN_L21_DASH_CENC:
    case HEVC_MAIN_L21_L30_DASH_CENC_TL:
    case HEVC_MAIN10_L21_DASH_CENC:
    case HEVC_MAIN10_L21_L30_DASH_CENC_TL:
        maxWidthForProfile = 512;
        maxHeightForProfile = 384;
        break;

    case HEVC_MAIN_L30_DASH_CENC:
    case HEVC_MAIN_L30_L31_DASH_CENC_TL:
    case HEVC_MAIN10_L30_DASH_CENC:
    case HEVC_MAIN10_L30_L31_DASH_CENC_TL:
#ifdef NRDP_HAS_HDR_VIDEO
    case HEVC_DV_MAIN10_L30_DASH_CENC:
    case HEVC_HDR_MAIN10_L30_DASH_CENC:
#endif
        maxWidthForProfile = 720;
        maxHeightForProfile = 480;
        break;

    case HEVC_MAIN_L31_DASH_CENC:
    case HEVC_MAIN_L31_L40_DASH_CENC_TL:
    case HEVC_MAIN10_L31_DASH_CENC:
    case HEVC_MAIN10_L31_L40_DASH_CENC_TL:
#ifdef NRDP_HAS_HDR_VIDEO
    case HEVC_DV_MAIN10_L31_DASH_CENC:
    case HEVC_HDR_MAIN10_L31_DASH_CENC:
#endif
        maxWidthForProfile = 1280;
        maxHeightForProfile = 720;
        break;

    case HEVC_MAIN_L40_DASH_CENC:
    case HEVC_MAIN_L41_DASH_CENC:
    case HEVC_MAIN_L40_L41_DASH_CENC_TL:
    case HEVC_MAIN10_L40_DASH_CENC:
    case HEVC_MAIN10_L41_DASH_CENC:
    case HEVC_MAIN10_L40_L41_DASH_CENC_TL:
#ifdef NRDP_HAS_HDR_VIDEO
    case HEVC_DV_MAIN10_L40_DASH_CENC:
    case HEVC_DV_MAIN10_L41_DASH_CENC:
    case HEVC_HDR_MAIN10_L40_DASH_CENC:
    case HEVC_HDR_MAIN10_L41_DASH_CENC:
#endif
        maxWidthForProfile = 1920;
        maxHeightForProfile = 1080;
        break;

    case HEVC_MAIN_L50_DASH_CENC:
    case HEVC_MAIN_L51_DASH_CENC:
    case HEVC_MAIN_L50_L51_DASH_CENC_TL:
    case HEVC_MAIN10_L50_DASH_CENC:
    case HEVC_MAIN10_L51_DASH_CENC:
    case HEVC_MAIN10_L50_L51_DASH_CENC_TL:
#ifdef NRDP_HAS_HDR_VIDEO
    case HEVC_DV_MAIN10_L50_DASH_CENC:
    case HEVC_DV_MAIN10_L51_DASH_CENC:
    case HEVC_HDR_MAIN10_L50_DASH_CENC:
    case HEVC_HDR_MAIN10_L51_DASH_CENC:
#endif
        maxWidthForProfile = 3840;
        maxHeightForProfile = 2160;
        break;

    default:
        // Unhandled content profile
        maxWidthForProfile = 1920;
        maxHeightForProfile = 1080;
        return false;
    }

    return true;
}

enum Video3DType
{
    UNKNOWN_3D_TYPE = -1,
    VIDEO_2D,
    VIDEO_3D
};

inline Video3DType video3DTypeFromString(const std::string& s)
{
    if(s == "3D") return VIDEO_3D;
    if(s == "2D") return VIDEO_2D;
    if(s == "default") return VIDEO_2D;
    return UNKNOWN_3D_TYPE;
}

inline Video3DType profileVideo3DType(ContentProfile p)
{
    switch(p)
    {
    case VIDEO_PLAYREADY_MPEG4P2_PIFF:
    case VIDEO_PLAYREADY_H264_BPL30_PIFF:
    case VIDEO_PLAYREADY_H264_MPL30_PIFF:
    case VIDEO_PLAYREADY_H264_MPL31_PIFF:
    case VIDEO_PLAYREADY_H264_MPL40_PIFF:
    case VIDEO_PLAYREADY_H264_BPL30_DASH:
    case VIDEO_PLAYREADY_H264_MPL30_DASH:
    case VIDEO_PLAYREADY_H264_MPL31_DASH:
    case VIDEO_PLAYREADY_H264_MPL40_DASH:
    case VIDEO_NONE_H264_BPL13_DASH:
    case VIDEO_NONE_H264_MPL30_DASH:
    case VIDEO_NONE_H264_MPL31_DASH:
    case VIDEO_NONE_H264_MPL40_DASH:
    case VIDEO_PLAYREADY_H264MPL30_M2TS:
    case VIDEO_PLAYREADY_H264MPL31_M2TS:
    case VIDEO_PLAYREADY_H264MPL40_M2TS:
    case VIDEO_PLAYREADY_H264MPL13_M2TS:
    case HEVC_MAIN_L20_DASH_CENC:
    case HEVC_MAIN_L21_DASH_CENC:
    case HEVC_MAIN_L30_DASH_CENC:
    case HEVC_MAIN_L31_DASH_CENC:
    case HEVC_MAIN_L40_DASH_CENC:
    case HEVC_MAIN_L41_DASH_CENC:
    case HEVC_MAIN_L50_DASH_CENC:
    case HEVC_MAIN_L51_DASH_CENC:

    case HEVC_MAIN_L20_L21_DASH_CENC_TL:
    case HEVC_MAIN_L21_L30_DASH_CENC_TL:
    case HEVC_MAIN_L30_L31_DASH_CENC_TL:
    case HEVC_MAIN_L31_L40_DASH_CENC_TL:
    case HEVC_MAIN_L40_L41_DASH_CENC_TL:
    case HEVC_MAIN_L50_L51_DASH_CENC_TL:

    case HEVC_MAIN10_L20_DASH_CENC:
    case HEVC_MAIN10_L21_DASH_CENC:
    case HEVC_MAIN10_L30_DASH_CENC:
    case HEVC_MAIN10_L31_DASH_CENC:
    case HEVC_MAIN10_L40_DASH_CENC:
    case HEVC_MAIN10_L41_DASH_CENC:
    case HEVC_MAIN10_L50_DASH_CENC:
    case HEVC_MAIN10_L51_DASH_CENC:

    case HEVC_MAIN10_L20_L21_DASH_CENC_TL:
    case HEVC_MAIN10_L21_L30_DASH_CENC_TL:
    case HEVC_MAIN10_L30_L31_DASH_CENC_TL:
    case HEVC_MAIN10_L31_L40_DASH_CENC_TL:
    case HEVC_MAIN10_L40_L41_DASH_CENC_TL:
    case HEVC_MAIN10_L50_L51_DASH_CENC_TL:

#ifdef NRDP_HAS_HDR_VIDEO
    case HEVC_DV_MAIN10_L20_DASH_CENC:
    case HEVC_DV_MAIN10_L21_DASH_CENC:
    case HEVC_DV_MAIN10_L30_DASH_CENC:
    case HEVC_DV_MAIN10_L31_DASH_CENC:
    case HEVC_DV_MAIN10_L40_DASH_CENC:
    case HEVC_DV_MAIN10_L41_DASH_CENC:
    case HEVC_DV_MAIN10_L50_DASH_CENC:
    case HEVC_DV_MAIN10_L51_DASH_CENC:

    case HEVC_HDR_MAIN10_L30_DASH_CENC:
    case HEVC_HDR_MAIN10_L31_DASH_CENC:
    case HEVC_HDR_MAIN10_L40_DASH_CENC:
    case HEVC_HDR_MAIN10_L41_DASH_CENC:
    case HEVC_HDR_MAIN10_L50_DASH_CENC:
    case HEVC_HDR_MAIN10_L51_DASH_CENC:
#endif
        return VIDEO_2D;

    case VIDEO_PLAYREADY_H264_SHPL30_DASH:
    case VIDEO_PLAYREADY_H264_SHPL31_DASH:
    case VIDEO_PLAYREADY_H264_SHPL40_DASH:
        return VIDEO_3D;

    default:
        return UNKNOWN_3D_TYPE;
    }
}

enum VolumeControlType {
    NO_VOLUME_CONTROL,
    TV_VOLUME_CONTROL,
    NETFLIX_APP_VOLUME_CONTROL
};

inline std::string volumeControlTypeToString(VolumeControlType t)
{
    switch(t)
    {
    case NO_VOLUME_CONTROL: return "no_volume_control";
    case TV_VOLUME_CONTROL: return "tv_volume_control";
    case NETFLIX_APP_VOLUME_CONTROL: return "netflix_app_volume_control";
    default: return "";
    }
}

enum PlaybackType
{
    STANDARD_PLAYBACK = 1,
    PREVIEW_PLAYBACK
};

inline std::string playbackTypeToString(PlaybackType t)
{
    switch(t)
    {
        case STANDARD_PLAYBACK: return "standard";
        case PREVIEW_PLAYBACK:  return "preview";
        default:                return "";
    }
}

enum DrmLicenseType {
    // this is in the order of priority
    // standard license has priority over limited duration license
    INVALID_LICENSE = 0,
    LIMITED_DURATION_LICENSE,
    STANDARD_LICENSE
};

inline std::string drmLicenseTypeToString(DrmLicenseType t)
{
    switch(t){
    case INVALID_LICENSE:          return "invalidLicense";
    case LIMITED_DURATION_LICENSE: return "limitedDurationLicense";
    case STANDARD_LICENSE:         return "standardLicense";
    default:                       return "";
    }
}

enum ViewModeType
{
	UNKNOWN_VIEWMODE_TYPE = -1,
	VIEWMODE_2D,
	VIEWMODE_3D
};

inline ViewModeType viewModeTypeFromString(const std::string& s)
{
	if(s == "3D") return VIEWMODE_3D;
	if(s == "2D") return VIEWMODE_2D;
	return UNKNOWN_VIEWMODE_TYPE;
}

inline std::string viewModeTypeToString(ViewModeType t)
{
	switch(t)
	{
		case VIEWMODE_3D: return "3D";
		case VIEWMODE_2D: return "2D";
		default:          return "";
	}
}

enum PlayerType
{
    PLAYER_EXTERNAL = 0,
    PLAYER_ASPLAYER,
    PLAYER_NRDPVIDEO,
    PLAYER_MEDIASOURCEPLAYER,
    PLAYER_INVALID
};

namespace ase {

/** default values for streaming config parameters */
#define DEFAULT_THROUGHPUT_ENTER_THRESHOLD 40000
#define DEFAULT_THROUGHPUT_EXIT_THRESHOLD 34000

#define DEFAULT_MIN_PREBUF_SIZE 4000


#define MIN_NONEMPTY_AUDIO_FRAGMENTS 2

#define DEFAULT_LOW_WATERMARK_LEVEL 15000

#define DEFAULT_ABANDON_THRESHOLD 2000

#define DEFAULT_LOCK_PERIOD_AFTER_PROBE_COMPLETE 60000

#define DEFAULT_THROUGHPUT_MEASURE_WINDOW 15000

#define DEFAULT_PIPELINE_ENABLED 1
#define DEFAULT_MAX_PARALLEL_CONNECTIONS 3

/**
 * @enum StreamingState
 */
enum StreamingState
{
    PLAYER_BUFFERING,
    PLAYER_PLAYING,
    PLAYER_STOPPED
};

/** The media types. */
enum MediaType
{
    MEDIA_UNKNOWN = -1,
    MEDIA_AUDIO,
    MEDIA_VIDEO,
    MEDIA_TEXT,
    MEDIA_HEADERS,
};

/** Type definition for domain key list. */
typedef std::vector<std::string> DomainKeyList;

// Streaming statistics for each media
struct StreamingMediaStat
{
    StreamingMediaStat(MediaType mediaType,
                       uint32_t playbackBitrate, uint32_t streamingBitrate,
                       ullong streamingTime,
                       uint32_t completeBuffer, uint32_t incompleteBuffer,
                       uint32_t usedMediaBuffer, uint32_t availablemediaBuffer);

    MediaType mMediaType;
    uint32_t mPlaybackBitrate; // playback bitrate in kbps
    uint32_t mStreamingBitrate; // streaming bitrate in kbps
    ullong mStreamingTimeInMs; // streaming time in ms
    uint32_t mCompleteBufferInMs; // complete buffer length in ms
    uint32_t mIncompleteBufferInMs; // incomplete buffer length in ms
    uint32_t mUsedMediaBuffer; // used media buffer size in bytes
    uint32_t mAvailableMediaBuffer; // available media buffer size in bytes
};

struct LocationStat
{
    LocationStat(uint32_t bandwidth, double confidence,
                 uint32_t avgConnectTime, std::string const& locationName);
    uint32_t mBandwidth; // current bandwidth for the current location in kbps
    double mConfidence; // the confidence level for the current bandwidth
    uint32_t mAvgConnectTime; // average connect time for the current location
    std::string mLocationName; // the current location/cdn for streaming
};

struct CdnParam
{
    CdnParam() : mConnectionFailureThreshold(3), mHttpConnectTimeout(8000), mHttpStallTimeout(8000)
    {
    }

    uint32_t mConnectionFailureThreshold;
    uint32_t mHttpConnectTimeout;
    uint32_t mHttpStallTimeout;
};


struct StreamingStatReport
{
    ullong mPlaybackTimeInMs; // playback time in ms
    std::vector<shared_ptr<StreamingMediaStat> > mStreamingMediaStatList;
    shared_ptr<LocationStat> mCurrentLocationStatPtr;
};


struct TrackConfiguration
{
    bool        mOpenRange;
    bool        mPipeline;
    uint32_t    mConnections;
    uint32_t    mSocketBufferSize;

    TrackConfiguration()
        : mOpenRange(true)
        , mPipeline(false)
        , mConnections(1)
        , mSocketBufferSize(32 * 1024)
    {
    }

    TrackConfiguration( bool        openRange,
                        bool        pipeline,
                        uint32_t    connections,
                        uint32_t    socketBufferSize )
        : mOpenRange( openRange )
        , mPipeline( pipeline )
        , mConnections( connections )
        , mSocketBufferSize( socketBufferSize )
    {}

};


inline
StreamingMediaStat::StreamingMediaStat(MediaType mediaType,
                       uint32_t playbackBitrate, uint32_t streamingBitrate,
                       ullong streamingTime,
                       uint32_t completeBuffer, uint32_t incompleteBuffer,
                       uint32_t usedMediaBuffer, uint32_t availablemediaBuffer)
                    : mMediaType(mediaType),
                      mPlaybackBitrate(playbackBitrate), mStreamingBitrate(streamingBitrate),
                      mStreamingTimeInMs(streamingTime),
                      mCompleteBufferInMs(completeBuffer), mIncompleteBufferInMs(incompleteBuffer),
                      mUsedMediaBuffer(usedMediaBuffer), mAvailableMediaBuffer(availablemediaBuffer)
{
}

inline
LocationStat::LocationStat(uint32_t bandwidth, double confidence,
                           uint32_t avgConnectTime, std::string const& locationName)
                    : mBandwidth(bandwidth), mConfidence(confidence),
                      mAvgConnectTime(avgConnectTime), mLocationName(locationName)
{
}


}}

#endif

