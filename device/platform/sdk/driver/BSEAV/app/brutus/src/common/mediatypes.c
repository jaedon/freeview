/***************************************************************************
 *     Copyright (c) 2002-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mediatypes.c $
 * $brcm_Revision: 30 $
 * $brcm_Date: 2/15/11 2:21p $
 * $brcm_Revision: 30 $ *
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/common/mediatypes.c $
 * 
 * 30   2/15/11 2:21p nickh
 * SW7422-213: Add SVC/MVC media types
 * 
 * 30   2/15/11 2:20p nickh
 * SW7422-213: Add SVC/MVC media types
 * 
 * 30   2/15/11 2:18p nickh
 * SW7422-213: Add SVC/MVC media types
 * 
 * 29   5/11/09 5:12p erickson
 * PR54915: remove unused and misleading code
 *
 * 28   11/26/08 6:40p vsilyaev
 * PR 49704: Bypass hardoded video_xpt table for new combinations of
 * container/video codec
 *
 * 27   9/3/08 2:42p vsilyaev
 * PR 46412: Added AVI/AVC combination
 *
 * 26   3/4/08 12:50p vsilyaev
 * PR 39818: Added MKV format
 *
 * 25   1/25/08 3:20p katrep
 * PR38429: Added support for AVS
 *
 * 24   9/7/07 6:30p mward
 * PR34089: Using C99 standard needs includes to avoid implicit
 * declaration warnings.
 *
 * 22   6/19/07 3:02p erickson
 * PR32352: use bsettop_namevalue_pairs instead of maintaining another
 * mapping
 *
 * 21   6/7/07 6:24p gmohile
 * PR 25109 : add MP4 container type
 *
 * 20   6/5/07 2:42p erickson
 * PR31728: add baudio_format_aac_plus_loas
 *
 * 19   3/21/07 11:57a erickson
 * PR26201: fix up decode info page
 *
 * 18   3/2/07 6:07p vsilyaev
 * PR 25701: Added DIVX support
 *
 * 17   2/7/07 12:28p erickson
 * PR22543: improve Brutus Playback UI for audio-only streams
 *
 * 16   1/29/07 5:30p gmohile
 * PR 25704: add divx 3.11 support
 *
 * 15   12/19/06 1:10p erickson
 * PR23714: finish the job
 *
 * 14   12/19/06 11:49a erickson
 * PR23714: added H263 types to Brutus
 *
 * 13   11/3/06 2:55p erickson
 * PR25109: fix VC1_SM types, change DIVX to more correct MP4P2 (mpeg4
 * part2)
 *
 * 12   10/30/06 4:48p erickson
 * PR25109: added DivX support
 *
 * 11   9/8/06 5:21p tokushig
 * PR20685: add support for simple/main profile in brutus
 *
 * SanDiego_DRM_ASF/5   9/7/06 6:21p tokushig
 * merge from main
 *
 * SanDiego_DRM_ASF/4   9/7/06 5:26p tokushig
 * added handling of simple/main asf profile
 *
 * 10   9/1/06 3:51p erickson
 * PR22526: fixed VC1_ASF text
 *
 * 9   8/3/06 5:38p rjlewis
 * PR23249: compiler warnings.
 *
 * 8   7/26/06 2:15p tokushig
 * PR20685: added DRM support to brutus
 *
 * SanDiego_DRM_ASF/2   7/20/06 3:26p tokushig
 * added WMA_STD media type
 *
 * SanDiego_DRM_ASF/1   6/19/06 5:11p tokushig
 * fixed asf bstream type in video types table
 *
 * 7   6/26/06 1:19p dlwin
 * PR 22343: Added support VC1_ASF media type plus WMA (Standard Format)
 * type.
 *
 * 6   3/15/06 10:37p erickson
 * PR20158: convert bad strcasecmp to strncasecmp
 *
 * 5   1/30/06 5:13p erickson
 * PR19202: switch DVD formats to VC1_VOB and AVC_VOB
 *
 * 4   1/20/06 12:42p erickson
 * PR18840: added LPCM_HDDVD and LPCM_BLURAY to brutus
 *
 * 3   1/18/06 1:37p erickson
 * PR17108: fix segfault bug
 *
 * 2   1/17/06 11:46a erickson
 * PR18651: allow audio type names (i.e. AC3,MPEG) to be used in video's
 * audtype param
 *
 * 1   10/13/05 5:24p erickson
 * PR17541: moved general purpose C code
 *
 **************************************************************************/
#include <stdlib.h>
#include <string.h>
#include "mediatypes.h"
#include "bsettop_namevalue_pairs.h"

const char *g_qualityStr[4] = {
    "Best",
    "High",
    "Medium",
    "Basic"
};

/* TODO: replace w/ standard function */
const char *g_encryptionStr[5] = {
    "NONE",
    "3DES",
    "DES",
    "AES",
    "DVB"
};

const char *b_audtype_str(baudio_format audtype)
{
    const char *name;
    if (!b_get_name(baudio_format_pairs, (unsigned)audtype, &name))
        return name;
    return "Unknown";
}

const char *b_xpttype_str(bstream_mpeg_type xpttype)
{
    const char *name;
    if (!b_get_name(bmpeg_type_pairs, (unsigned)xpttype, &name))
        return name;
    return "Unknown";
}

baudio_format b_lookup_audtype(const char *text, char **endptr)
{
    unsigned value;
    /* TODO: allow more delimiters */
    char *end = (char*)strchr(text, ',');
    if (!end) end = (char*)text+strlen(text); /* point to terminating \0 */

    if (!b_get_value_n_i(baudio_format_pairs, text, end-text, &value)) {
        if (endptr) *endptr = end;
        return value;
    }

    /* take the hex value */
    return strtoul(text, endptr, 0);
}

bstream_mpeg_type b_lookup_xpttype(const char *text, char **endptr)
{
    unsigned value;
    /* TODO: allow more delimiters */
    char *end = (char*)strchr(text, ',');
    if (!end) end = (char*)text+strlen(text); /* point to terminating \0 */

    if (!b_get_value_n_i(bmpeg_type_pairs, text, end-text, &value)) {
        if (endptr) *endptr = end;
        return value;
    }

    /* take the hex value */
    return strtoul(text, endptr, 0);
}

bvideo_codec b_lookup_vidtype(const char *text, char **endptr)
{
    unsigned value;
    /* TODO: allow more delimiters */
    char *end = (char*)strchr(text, ',');
    if (!end) end = (char*)text+strlen(text); /* point to terminating \0 */

    if (!b_get_value_n_i(bvideo_codec_pairs, text, end-text, &value)) {
        if (endptr) *endptr = end;
        return value;
    }

    /* take the hex value */
    return strtoul(text, endptr, 0);
}

const char *b_vidtype_str(bvideo_codec vidtype)
{
    const char *name;
    if (!b_get_name(bvideo_codec_pairs, (unsigned)vidtype, &name))
        return name;
    return "Unknown";
}

/* TODO: this is bad. we should convert to separate video and XPT types. */
static struct vidtypes g_vidtypes[] = {
{bvideo_codec_mpeg1, bstream_mpeg_type_es,  "MPEG1", "MPEG1"},
{bvideo_codec_mpeg2, bstream_mpeg_type_es,  "MPEG2 ES", "ES"},
{bvideo_codec_mpeg2, bstream_mpeg_type_bes, "MPEG2 BES", "BES"},
{bvideo_codec_mpeg2, bstream_mpeg_type_pes, "MPEG2 PES", "PES"},
{bvideo_codec_mpeg2, bstream_mpeg_type_ts,  "MPEG2 TS", "TS"},
{bvideo_codec_mpeg2, bstream_mpeg_type_dss_es,  "DirecTV", "DIRECT_TV_ES"},
{bvideo_codec_mpeg2, bstream_mpeg_type_dss_pes, "DirecTV(PES)", "DIRECT_TV_PES"},
{bvideo_codec_mpeg2, bstream_mpeg_type_vob,"MPEG2 DVD", "VOB"},
{bvideo_codec_h264, bstream_mpeg_type_es,  "AVC_ES",  "AVC_ES"},
{bvideo_codec_h264, bstream_mpeg_type_pes, "AVC_PES", "AVC_PES"},
{bvideo_codec_h264, bstream_mpeg_type_ts,  "AVC_TS",  "AVC_TS"},
{bvideo_codec_h264, bstream_mpeg_type_vob, "AVC_VOB", "AVC_VOB"},
{bvideo_codec_h264, bstream_mpeg_type_mp4, "AVC_MP4", "AVC_MP4"},
{bvideo_codec_h264, bstream_mpeg_type_mkv, "AVC_MKV", "AVC_MKV"},
{bvideo_codec_h264, bstream_mpeg_type_avi, "AVC_AVI", "AVC_AVI"},
{bvideo_codec_h264_svc, bstream_mpeg_type_ts, "SVC_TS", "SVC_TS"},
{bvideo_codec_h264_mvc, bstream_mpeg_type_ts, "MVC_TS", "MVC_TS"},
{bvideo_codec_h263, bstream_mpeg_type_es,  "H263_ES",  "H263_ES"},
{bvideo_codec_h263, bstream_mpeg_type_pes, "H263_PES", "H263_PES"},
{bvideo_codec_h263, bstream_mpeg_type_ts,  "H263_TS",  "H263_TS"},
{bvideo_codec_h263, bstream_mpeg_type_vob, "H263_VOB", "H263_VOB"},
{bvideo_codec_vc1, bstream_mpeg_type_es,   "VC1_ES",  "VC1_ES"},
{bvideo_codec_vc1, bstream_mpeg_type_pes,  "VC1_PES", "VC1_PES"},
{bvideo_codec_vc1, bstream_mpeg_type_ts,   "VC1_TS",  "VC1_TS"},
{bvideo_codec_vc1, bstream_mpeg_type_vob,  "VC1_VOB", "VC1_VOB"},
{bvideo_codec_vc1, bstream_mpeg_type_asf,  "VC1_ASF", "VC1_ASF"},
{bvideo_codec_vc1_sm, bstream_mpeg_type_es,   "VC1_SM_ES",  "VC1_SM_ES"},
{bvideo_codec_vc1_sm, bstream_mpeg_type_pes,  "VC1_SM_PES", "VC1_SM_PES"},
{bvideo_codec_vc1_sm, bstream_mpeg_type_ts,   "VC1_SM_TS",  "VC1_SM_TS"},
{bvideo_codec_vc1_sm, bstream_mpeg_type_vob,  "VC1_SM_VOB", "VC1_SM_VOB"},
{bvideo_codec_vc1_sm, bstream_mpeg_type_asf,  "VC1_SM_ASF", "VC1_SM_ASF"},
{bvideo_codec_mpeg4_part2, bstream_mpeg_type_es, "MP4P2_ES", "MP4P2_ES"},
{bvideo_codec_mpeg4_part2, bstream_mpeg_type_pes, "MP4P2_PES", "MP4P2_PES"},
{bvideo_codec_mpeg4_part2, bstream_mpeg_type_ts, "MP4P2_TS", "MP4P2_TS"},
{bvideo_codec_mpeg4_part2, bstream_mpeg_type_vob, "MP4P2_VOB", "MP4P2_VOB"},
{bvideo_codec_mpeg4_part2, bstream_mpeg_type_asf, "MP4P2_ASF", "MP4P2_ASF"},
{bvideo_codec_mpeg4_part2, bstream_mpeg_type_mp4, "MP4P2_MP4", "MP4P2_MP4"},
{bvideo_codec_divx_311, bstream_mpeg_type_es, "DIVX_311_ES", "DIVX_311_ES"},
{bvideo_codec_divx_311, bstream_mpeg_type_pes, "DIVX_311_PES", "DIVX_311_PES"},
{bvideo_codec_divx_311, bstream_mpeg_type_ts, "DIVX_311_TS", "DIVX_311_TS"},
{bvideo_codec_divx_311, bstream_mpeg_type_vob, "DIVX_311_VOB", "DIVX_311_VOB"},
{bvideo_codec_divx_311, bstream_mpeg_type_asf, "DIVX_311_ASF", "DIVX_311_ASF"},
{bvideo_codec_divx_311, bstream_mpeg_type_avi, "DIVX_311", "DIVX_311"},
{bvideo_codec_mpeg4_part2, bstream_mpeg_type_avi, "DIVX", "DIVX"},
{bvideo_codec_avs, bstream_mpeg_type_es,  "AVS_ES", "AVS_ES"},
{bvideo_codec_avs, bstream_mpeg_type_pes, "AVS_PES", "AVS_PES"},
{bvideo_codec_avs, bstream_mpeg_type_ts,  "AVS_TS", "AVS_TS"}
};

struct vidtypes *b_lookup_vidxpt_type(bvideo_codec format, bstream_mpeg_type mpeg_type)
{
    unsigned i;
    for (i=0;i<sizeof(g_vidtypes)/sizeof(g_vidtypes[0]);i++) {
        if (g_vidtypes[i].vidtype == format && g_vidtypes[i].mpeg_type == mpeg_type) {
            return &g_vidtypes[i];
        }
    }
    return NULL;
}

struct vidtypes *b_lookup_vidxpt_type_by_infofile(const char *infofile_name)
{
    unsigned i;
    for (i=0;i<sizeof(g_vidtypes)/sizeof(g_vidtypes[0]);i++) {
        if (!strcasecmp(infofile_name, g_vidtypes[i].infofile_name)) {
            return &g_vidtypes[i];
        }
    }
    return NULL;
}

const char *b_vidxpt_str(bvideo_codec format, bstream_mpeg_type mpeg_type)
{
    struct vidtypes *vt = b_lookup_vidxpt_type(format, mpeg_type);
    return vt?vt->name:b_xpttype_str(mpeg_type);
}

