/***************************************************************************
 *     Copyright (c) 2007-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmedia_probe_util.c $
 * $brcm_Revision: 6 $
 * $brcm_Date: 8/9/12 9:38a $
 *
 * Module Description:
 *
 * BMedia library, misc utilities
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bmedia_probe_util.c $
 * 
 * 6   8/9/12 9:38a vsilyaev
 * SW7346-970: Fixed typo
 * 
 * 5   3/6/12 11:56a vsilyaev
 * SW7425-2544: Fixed warning
 * 
 * 4   3/5/12 5:44p vsilyaev
 * SW7425-2544: Added code to extract 'ct_type' from the H264 headers
 * 
 * 3   3/30/10 6:52p vsilyaev
 * SW7335-698: Validate SPS information prior to using it
 * 
 * 2   6/5/09 1:42p vsilyaev
 * PR 55759: Fixed hanlding of rogue data when parsing VLC codes
 * 
 * 1   11/26/08 12:10p vsilyaev
 * PR 47650: H264 parsing utilities
 * 
 * 
 *******************************************************************************/
#include "bstd.h"
#include "biobits.h"
#include "bvlc.h"
#include "bmedia_probe_util.h"
#include "bkni.h"

BDBG_MODULE(bmedia_probe_util);

#define BDBG_MSG_TRACE(x)   /* BDBG_MSG(x) */

    
void 
bmedia_vlc_decode_init(bmedia_vlc_decode *vlc, const uint8_t *buf, size_t buf_size)
{
    BDBG_ASSERT(vlc);
    vlc->buf = buf;
    vlc->size = buf_size;
    vlc->index = 0;
    vlc->bit = 7;
    return;
} 

int 
bmedia_vlc_decode_read(bmedia_vlc_decode *vlc)
{
    int bit;
    BDBG_ASSERT(vlc->index < vlc->size);
    bit = b_vlc_decode(vlc->buf, vlc->size, vlc->index, vlc->bit, &vlc->index, &vlc->bit);
    BDBG_ASSERT(vlc->index < vlc->size);
    return bit;
}

int
bmedia_vlc_decode_skip(bmedia_vlc_decode *vlc)
{
    int eof ;
    BDBG_ASSERT(vlc->index < vlc->size);
    eof = b_vlc_skip(vlc->buf, vlc->size, vlc->index, vlc->bit, &vlc->index, &vlc->bit);
    BDBG_ASSERT(vlc->index < vlc->size);
    return eof;
}


int 
bmedia_vlc_decode_bit(bmedia_vlc_decode *vlc)
{
    int result;

    result = (vlc->buf[vlc->index] >> vlc->bit)&1;
    if(vlc->bit) {
        vlc->bit--;
    } else {
        if(vlc->index<vlc->size) {
            vlc->index++;
            vlc->bit = 7;
        } else {
            return -1;
        }
    }
    return result;
}

int 
bmedia_vlc_decode_bits(bmedia_vlc_decode *vlc, unsigned bits)
{
    int result;

    for(result=0;bits>0;bits--) {
        int val = bmedia_vlc_decode_bit(vlc);
        if(val<0) { return val;}
        result = (result << 1) | (val&1);
    }
    return result;
}

bool
b_h264_video_sps_parse(b_h264_video_sps *sps, const uint8_t *buf, size_t size)
{
    BDBG_ASSERT(buf);
    BDBG_ASSERT(sps);
    sps->compliant.baseline = false;
    sps->compliant.main = false;
    sps->compliant.extended = false;
    sps->compliant.high = false;
    sps->valid = false;
    if(size<3) {
        return false;
    }
    sps->valid = true;
    sps->profile = buf[0];
    sps->level = buf[2];
    if(B_GET_BIT(buf[1],7) && sps->profile != 66) {
        sps->compliant.baseline = true;
    }
    if(B_GET_BIT(buf[1],6) && sps->profile != 77) {
        sps->compliant.main = true;
    }
    if(B_GET_BIT(buf[1],5) && sps->profile != 88) {
        sps->compliant.extended = true;
    }
    if(B_GET_BIT(buf[1],5) && sps->profile != 100) {
        sps->compliant.high = true;
    }
    return true;
}

int 
b_h264_video_sps_to_string(const b_h264_video_sps *sps, char *buf, size_t size)
{
    const char *profile;
    char profiles[64];
    size_t profiles_offset=1;
    int rc;
    char profile_number[8];

    if(!sps->valid) {
        *buf = '\0';
        return 0;
    }
    switch(sps->profile) {
    case b_h264_profile_baseline: profile="Baseline";break;
    case b_h264_profile_main: profile="Main";break;
    case b_h264_profile_extended: profile="Extended";break;
    case b_h264_profile_high: profile="High";break;
    case b_h264_profile_high10: profile="High 10";break;
    case b_h264_profile_highd422: profile="High 422";break;
    case b_h264_profile_highd444: profile="High 444";break;
    default:
        BKNI_Snprintf(profile_number, sizeof(profile_number), "%d", sps->profile);
        profile = profile_number;
        break;
    }
    if(sps->compliant.baseline && profiles_offset<sizeof(profiles)) {
       rc = BKNI_Snprintf(profiles+profiles_offset, sizeof(profiles)-profiles_offset, "%s ", "Baseline");
       if(rc>0) {
           profiles_offset+=rc;
       }
    }
    if(sps->compliant.main && profiles_offset<sizeof(profiles)) {
       rc = BKNI_Snprintf(profiles+profiles_offset, sizeof(profiles)-profiles_offset, "%s ", "Main");
       if(rc>0) {
           profiles_offset+=rc;
       }
    }
    if(sps->compliant.extended && profiles_offset<sizeof(profiles)) {
       rc = BKNI_Snprintf(profiles+profiles_offset, sizeof(profiles)-profiles_offset, "%s ", "Extended");
       if(rc>0) {
           profiles_offset+=rc;
       }
    }
    if(sps->compliant.high && profiles_offset<sizeof(profiles)) {
       rc = BKNI_Snprintf(profiles+profiles_offset, sizeof(profiles)-profiles_offset, "%s ", "High");
       if(rc>0) {
           profiles_offset+=rc;
       }
    }
    if(profiles_offset>1) {
       profiles[0]='['; 
        if(profiles_offset<sizeof(profiles)) {
            profiles[profiles_offset-1]=']';
        }
    } else {
        profiles[0]='\0';
    }
    return BKNI_Snprintf(buf, size, "%s%s %u.%u", profile, profiles, sps->level/10, sps->level%10);
}

