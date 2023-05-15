/***************************************************************************
 *     Copyright (c) 2007-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmedia_probe_util.h $
 * $brcm_Revision: 3 $
 * $brcm_Date: 3/5/12 5:44p $
 *
 * Module Description:
 *
 * BMedia library, misc utilities
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bmedia_probe_util.h $
 * 
 * 3   3/5/12 5:44p vsilyaev
 * SW7425-2544: Added code to extract 'ct_type' from the H264 headers
 * 
 * 2   6/5/09 1:42p vsilyaev
 * PR 55759: Fixed hanlding of rogue data when parsing VLC codes
 * 
 * 1   11/26/08 12:11p vsilyaev
 * PR 47650: H264 parsing utilities
 * 
 * 
 *******************************************************************************/
#ifndef _BMEDIA_PROBE_UTIL_H__
#define _BMEDIA_PROBE_UTIL_H__

#ifdef __cplusplus
extern "C"
{
#endif


typedef struct bmedia_vlc_decode {
    const uint8_t *buf;
    size_t size;
    unsigned index;
    unsigned bit;
} bmedia_vlc_decode;

void bmedia_vlc_decode_init(bmedia_vlc_decode *vlc, const uint8_t *buf, size_t buf_size);
int bmedia_vlc_decode_read(bmedia_vlc_decode *vlc);
int bmedia_vlc_decode_skip(bmedia_vlc_decode *vlc);
int bmedia_vlc_decode_bit(bmedia_vlc_decode *vlc);
int bmedia_vlc_decode_bits(bmedia_vlc_decode *vlc, unsigned bits);

typedef enum b_h264_profile {
    b_h264_profile_baseline=66,
    b_h264_profile_main=77,
    b_h264_profile_extended=88,
    b_h264_profile_high=100,
    b_h264_profile_high10=110,
    b_h264_profile_highd422=122,
    b_h264_profile_highd444=144
} b_h264_profile;

typedef struct b_h264_video_sps {
    bool valid;
    uint8_t level;
    b_h264_profile profile;
    struct {
        bool baseline;
        bool main;
        bool extended;
        bool high;
    } compliant;
} b_h264_video_sps;

int b_h264_video_sps_to_string(const b_h264_video_sps *sps, char *buf, size_t size);
bool b_h264_video_sps_parse(b_h264_video_sps *sps, const uint8_t *buf, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* _BMEDIA_PROBE_UTIL_H__ */

