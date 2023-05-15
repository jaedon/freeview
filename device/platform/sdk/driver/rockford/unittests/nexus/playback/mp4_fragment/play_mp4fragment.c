/******************************************************************************
 *    (c)2008-2009 Broadcom Corporation
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
 * $brcm_Workfile: play_mp4fragment.c $
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 5/26/11 4:25p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/playback/mp4_fragment/play_mp4fragment.c $
 * 
 * Hydra_Software_Devel/7   5/26/11 4:25p vsilyaev
 * SW7425-615: Added H264 stream
 * 
 * Hydra_Software_Devel/6   5/19/11 4:51p vsilyaev
 * SW7405-4329: Added 1080p source
 * 
 * Hydra_Software_Devel/5   4/22/11 12:47p vsilyaev
 * SW7420-1821: Removed [outtdated] sample of AAC/H264 content
 * 
 * Hydra_Software_Devel/4   5/11/10 5:08p vsilyaev
 * SW7405-3773: Added support for WMA Pro audio
 * 
 * Hydra_Software_Devel/3   5/10/10 6:41p vsilyaev
 * SW7405-3773: Use 64 bit number for time portion of the file
 * 
 * Hydra_Software_Devel/2   5/10/10 6:28p vsilyaev
 * SW7405-3773: Added support for WMC1 video
 * 
 * Hydra_Software_Devel/1   5/7/10 1:25p vsilyaev
 * SW7405-3773: Unittest for playback of fragmented of fragmented MP4 file
 * 
 *****************************************************************************/
/* Nexus otfpvr unit test app: playback and trickmoes */

#include "nexus_platform.h"
#include "nexus_video_decoder.h"
#include "nexus_audio_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#include "nexus_video_adj.h"
#include "nexus_component_output.h"
#include "nexus_playpump.h"
#include "nexus_audio_dac.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_output.h"
#include "nexus_audio_input.h"
#include "nexus_spdif_output.h"


#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"

typedef struct mem_file {
    void *buf;
    size_t buf_len;
    size_t length;
    size_t offset;
} mem_file;

static void mem_file_write(mem_file *file,const void *ptr, size_t size)
{
    BDBG_ASSERT(file->offset + size<= file->buf_len);
    BKNI_Memcpy((uint8_t *)file->buf + file->offset, ptr, size);
    file->offset += size;
    if(file->length<file->offset) {
        file->length = file->offset;
    }
    return ;
}

static size_t mem_file_tell(mem_file *file)
{
    return file->offset;
}

static void mem_file_seek(mem_file *file, size_t offset)
{
    file->offset = offset;
    return;
}
#define BMP4_FRAGMENT_MAKE_EMBEDDED 1
#define FILE mem_file
#define fwrite(p,s,n,f) mem_file_write(f,p,(s*n))
#define fseek(f, o, w) mem_file_seek(f, o)
#define ftell(f) mem_file_tell(f)
#include "bmp4_fragment_make_segment.c"
#undef FILE
#undef fwrite
#undef fseek
#undef ftell

struct track_info {
    const char *name;
    const char fourcc[5];  
    unsigned codec;
};

struct stream_info {
    unsigned bitrate;
    const char *codec_private;
};
#if 1
/* BigBuckBunny_1080p.ism */
static const struct track_info video_track = {
    "video", "WVC1", NEXUS_VideoCodec_eVc1
};


static const struct stream_info video_streams[]= {
    {4930000, "250000010FDBBE3BF21B8A3BF886E804508ACB395E07C00000010E5AE7F840"}, /* 1920x1080 */
    {3675000, "250000010FDBBE31F1BF8A31F86FE8045082F0264617C00000010E5AE7F840"}, /* 1600x986 */
    {2750000, "250000010FD3BE27F1678A27F859E804508253EBE8E6C00000010E5AE7F840"}, /* 1280x720 */
    {2040000, "250000010FD3BE20F1278A20F849E80450823E414DD1400000010E5AE7F840"}, /* 1056x592 */
    {1520000, "250000010FCBAE1A70EF8A1A783BE8045081AE62F3F7400000010E5AE7F840"}, /* 848x480 */
    {1130000, "250000010FCBA215F0C78A15F831E8045081A27BD635C00000010E5AE7F840"}, /* 704x400 */
    {845000, "250000010FCB9A11F09F8A11F827E804508199C94077400000010E5AE7F840"},  /* 576x320 */
    {630000, "250000010FCB920DF07F8A0DF81FE804508113396020C00000010E5AE7F840"}, /* 448x256 */
    {470000, "250000010FC38E0B70678A0B7819E80450810E5747B6C00000010E5AE7F840"}, /* 368x208 */
    {350000, "250000010FC38A09F0578A09F815E80450808AADEACF400000010E5AE7F840"} /* 320x176 */
};

static const struct track_info audio_track = {
    "audio", "WMAP", NEXUS_AudioCodec_eWmaPro
};

static const struct stream_info audio_streams[]= {
    {64000, "6201020044AC0000451F0000CF05100012001000030000000000000000000000E00042C0"}
};

static const unsigned video_duration[] = {
20000000,20000000,20000000,20000000,20000000,20000000,20000000,
20000000,20000000,20000000,20000000,20000000,20000000,20000000,
20000000,20000000,20000000,20000000,20000000,20000000,20000000,
20000000,20000000,20000000,20000000,20000000,20000000,20000000,
20000000,20000000,20000000,20000000,20000000,20000000,20000000,
20000000,20000000,20000000,20000000,20000000,20000000,20000000,
20000000,20000000,20000000,20000000,20000000,20000000,20000000,
20000000,20000000,20000000,20000000,20000000,20000000,20000000,
20000000,20000000,20000000,20000000,20000000,20000000,20000000,
20000000,20000000,20000000,20000000,20000000,20000000,20000000,
20000000,20000000,20000000,20000000,20000000,20000000,20000000,
20000000,20000000,20000000,20000000,20000000,20000000,20000000,
20000000,20000000,20000000,20000000,20000000,20000000,20000000,
20000000,20000000,20000000,20000000,20000000,20000000,20000000,
20000000,20000000,20000000,20000000,20000000,20000000,20000000,
20000000,20000000,20000000,20000000,20000000,20000000,20000000,
20000000,20000000,20000000,20000000,20000000,20000000,20000000,
20000000,20000000,20000000,20000000,20000000,20000000,20000000,
20000000,20000000,20000000,20000000,20000000,20000000,20000000,
20000000,20000000,20000000,20000000,20000000,20000000,20000000,
20000000,20000000,20000000,20000000,20000000,20000000,20000000,
20000000,20000000,20000000,20000000,20000000,20000000,20000000,
20000000,20000000,20000000,20000000,20000000,20000000,20000000,
20000000,20000000,20000000,20000000,20000000,20000000,20000000,
20000000,20000000,20000000,20000000,20000000,20000000,20000000,
20000000,20000000,20000000,20000000,20000000,20000000,20000000,
20000000,20000000,20000000,20000000,20000000,20000000,20000000,
20000000,20000000,20000000,20000000,20000000,20000000,20000000,
20000000,20000000,20000000,20000000,20000000,20000000,20000000,
20000000,20000000,20000000,20000000,20000000,20000000,20000000,
20000000,20000000,20000000,20000000,20000000,20000000,20000000,
20000000,20000000,20000000,20000000,20000000,20000000,20000000,
20000000,20000000,20000000,20000000,20000000,20000000,20000000,
20000000,20000000,20000000,20000000,20000000,20000000,20000000,
20000000,20000000,20000000,20000000,20000000,20000000,20000000,
20000000,20000000,20000000,20000000,20000000,20000000,20000000,
20000000,20000000,20000000,20000000,20000000,20000000,20000000,
20000000,20000000,20000000,20000000,20000000,20000000,20000000,
20000000,20000000,20000000,20000000,20000000,20000000,20000000,
20000000,20000000,20000000,20000000,20000000,20000000,20000000,
20000000,20000000,20000000,20000000,20000000,20000000,20000000,
20000000,20000000,20000000,20000000,20000000,20000000,20000000,
20000000,20000000,20000000,5000001
};

static const unsigned audio_duration[] = {
20433560,19969161,19969161,20433560,20433560,20317460,18692064,
21362358,19388662,20433560,19620862,19504762,19504762,19969161,
19969161,20433560,19969161,20897959,20433560,19969161,18575964,
20433560,19969161,20433560,19504762,19504762,19969161,20433560,
19969161,21362358,19040363,20433560,18924263,20549660,20433560,
20433560,19504762,20433560,19040363,21362358,18111565,21710658,
18692063,20897959,19040363,19969161,20897959,18924263,21014059,
20433560,18575964,19969161,21362358,19504762,19040363,21362358,
20433560,18459864,21014059,19969161,19969161,19969161,20897959,
18575964,21826757,17647166,19969161,20433560,20317460,20085261,
19040363,20433560,19969161,20897959,19504762,20781860,20085260,
18575964,19795011,20143311,19751473,20999548,20085260,19969161,
19969161,21362358,17647166,20433560,20433560,19853061,20433561,
20085260,20897959,18111565,20433560,20897959,19040363,20215873,
21115646,18111565,19969161,20781860,19620861,20433560,20433560,
20317461,18575963,20549660,20433560,19969161,19969161,19388663,
21014058,20433560,19504762,19504762,20433560,18575964,19969161,
21362358,20433560,18575964,20781860,19040363,21014058,20433560,
19040363,19504762,20897959,19504762,19969161,20897959,19040363,
20433560,19504762,20433560,21623583,17850340,20897959,20433560,
18575964,21826757,20433561,19040362,19853062,19882086,19707936,
19388663,20085260,21826758,19504761,18575964,20897959,19504762,
19969161,19504762,20897959,19040363,19969161,20897959,20897960,
20317460,19156462,19504762,20230386,20636735,19504761,18924264,
21942857,19040362,19040363,21362359,19504761,20433561,19040362,
19388663,21304308,19678911,19504762,20897959,20433561,19969161,
19504761,19969161,19040363,20897959,19504762,19504762,19969161,
20897959,19504762,19504762,20259411,20143310,19795012,22000907,
18575963,20433561,19969161,20433560,19388662,20085261,20433560,
19504762,20897959,19040363,19969161,19040362,19969161,21826758,
18822675,20535148,20085261,19504762,19969161,20781859,19620862,
19040362,20897960,20433560,19969161,19969161,21362358,18924263,
18474376,19722449,20433560,20433561,21362358,18111564,20433561,
19969161,20897959,19504762,19504762,20897959,19040363,19504761,
20317461,21478458,19504762,19969161,19504762,20897959,19040363,
20433560,18924263,20549660,20259410,19678912,19969161,21362358,
18575964,21246258,19620862,19040363,19969161,19969161,19969161,
19969161,20433560,19969161,19853061,20085261,19969161,19969161,
20897959,18575964,21362358,19969161,18575964,20897959,20433560,
19504762,20433560,19504762,20433560,19969161,19969161,20433560,
19504762,20433560,19969161,18575964,20433560,21826757,18459864,
20085261,19969161,20897959,19504762,18924263,20549660,20897959,
19969161,19040363,20433560,20433560,4393197
};
#else
/* Elephants_Dream_1024-h264-st-aac.ism */
static const struct track_info video_track = {
    "video", "H264", NEXUS_VideoCodec_eH264
};

static const struct stream_info video_streams[]= {
    /* awk -F '"' '/Name/ {selected = ($2=="video")} /Bitrate|CodecPrivateData|FourCC|Max/{if(selected) print $2 ;}' manifest_utf8.txt */
    {6000000, /* 1920 1080 */ "0000000167640032AC2CA501E0089F97FF0400040052020202800233938066FF3031500016E360002DC6FF8C718A8000B71B00016E37FC6387684894580000000168E9093525"},
    {5027000, /* 1600 900  */ "0000000167640028AC2CA501901CFE7FFC100010014808080A0008CE4E019BFCC0C540004CB480009969FE31C62A000265A40004CB4FF18E1DA12251600000000168E9093525"},
    {2962000, /* 1280 720 */ "000000016764001FAC2CA5014016EFFC100010014808080A0008CE4E019BFCC0C100005A648000B4C9FE31C6080002D3240005A64FF18E1DA12251600000000168E9093525"},
    {2056000, /* 992 560 */ "000000016764001FAC2CA503E047BFF040003FC52020202800233938066FF303030003EBE8000FAFAFE31C6060007D7D0001F5F5FC6387684894580000000168E9093525"},
    {1427000, /* 768 432 */ "000000016764001EAC2CA50300DEFFC100010014808080A0008CE4E019BFCC0C0C000AE300002B8C7F8C718180015C600005718FF18E1DA12251600000000168E9093525"},
    {991000, /* 592 332 */ "0000000167640015AC2CA50250AFEFFF03FD0400520C0C0C800233938066FF3030200078F80003C7C7F8C71810003C7C0001E3E3FC6387684894580000000168E9093525"},
    {688000, /* 448 252 */ "0000000167640015AC2CA507021FBFFC10001001483030320008CE4E019BFCC0C080014FF0000A7F8FE31C604000A7F800053FC7F18E1DA12251600000000168E9093525"},
    {477000, /* 368 208 */ "000000016764000DAC2CA505C6EFFC10000FED483030320008CE4E019BFCC0C04003A3A0003A3A7F8C718080074740007474FF18E1DA1225160000000168E9093525"},
    {331000, /* 284 160 */ "000000016764000DAC2CA504857BFFF040003FE520C0C0C800233938066FF30301000A198000A19FFE31C6020014330001433FFC6387684894580000000168E9093525"},
    {230000, /* 224 128 */ "000000016764000DAC2CA50E11BFF040003F0520C0C0C800233938066FF30300001C1200038273F8C7180000E090001C139FC63876848945800000000168E9093525"}
};

static const struct track_info audio_track = {
    "audio_eng", "AACL", NEXUS_AudioCodec_eAacAdts
};


static const struct stream_info audio_streams[]= {
    /*  awk -F '"' '/Name/ {selected = ($2=="audio_eng")} /Bitrate|CodecPrivateData|FourCC|Max/{if(selected) print $2 ;}' manifest_utf8.txt */
    {160000, "121008C400002000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"}
};

static const unsigned video_duration[] = {
    /* awk -F '"' '/Name/ {selected = ($2=="video")} /d=/{if(selected) print $2 ",";}' manifest_utf8.txt */
23750000,
20000000,
20000000,
20000000,
20000000,
20410000,
20000000,
20000000,
20420000,
20000000,
20000000,
20000000,
20420000,
20000000,
20000000,
20000000,
20000000,
20410000,
20000000,
20000000,
20000000,
20000000,
20420000,
20000000,
20000000,
20000000,
20000000,
20420000,
20000000,
20000000,
20000000,
20000000,
20000000,
20410000,
20000000,
20000000,
20000000,
20000000,
20420000,
20000000,
20000000,
20000000,
20000000,
20420000,
20000000,
20000000,
20000000,
20000000,
20410000,
20000000,
20000000,
20000000,
20000000,
20420000,
20000000,
20000000,
20000000,
20000000,
20000000,
20420000,
20000000,
20000000,
20000000,
20000000,
20410000,
20000000,
20000000,
20000000,
20000000,
20420000,
20000000,
20000000,
20000000,
20000000,
40000000,
20000000,
20000000,
20000000,
20420000,
20000000,
20000000,
20000000,
20000000,
20410000,
20000000,
20000000,
20000000,
20000000,
20420000,
20000000,
20000000,
20000000,
20000000,
20000000,
20420000,
20000000,
20000000,
20000000,
20000000,
20410000,
20000000,
20000000,
20000000,
20000000,
20420000,
20000000,
20000000,
20000000,
20000000,
20420000,
20000000,
20000000,
20000000,
20000000,
20000000,
20410000,
20000000,
20000000,
20000000,
20000000,
20420000,
20000000,
20000000,
20000000,
20000000,
20420000,
20000000,
20000000,
20000000,
20000000,
20000000,
20410000,
20000000,
20000000,
20000000,
20000000,
20420000,
20000000,
20000000,
20000000,
20000000,
20420000,
20000000,
20000000,
37500000,
20000000,
20000000,
20000000,
20410000,
20000000,
20000000,
20000000,
20420000,
20420000,
20000000,
20000000,
20000000,
20000000,
20000000,
20410000,
20000000,
20000000,
20000000,
20000000,
20420000,
20000000,
20000000,
20000000,
20000000,
20420000,
20000000,
20000000,
20000000,
20000000,
20410000,
20000000,
20000000,
20000000,
20000000,
20420000,
20000000,
20000000,
20000000,
20000000,
20000000,
20420000,
20000000,
20000000,
20000000,
21660000,
20000000,
20000000,
20000000,
20000000,
20420000,
20000000,
20000000,
20000000,
20000000,
20420000,
20000000,
20000000,
20000000,
20000000,
20000000,
20410000,
20000000,
20000000,
20000000,
20000000,
20420000,
22500000,
20420000,
20000000,
20000000,
20000000,
20000000,
20410000,
20000000,
20000000,
20000000,
20000000,
20420000,
20000000,
20000000,
20000000,
20000000,
20420000,
20000000,
20000000,
20000000,
20000000,
20000000,
20410000,
20420000,
20000000,
20000000,
20000000,
20420000,
20000000,
20000000,
20000000,
20000000,
20000000,
20410000,
20000000,
20000000,
20000000,
20000000,
20420000,
20000000,
20000000,
20000000,
20000000,
20420000,
20000000,
20000000,
20000000,
20000000,
20000000,
20410000,
20000000,
20000000,
20000000,
20000000,
20420000,
20000000,
20000000,
20000000,
20000000,
20420000,
20000000,
20000000,
20000000,
20000000,
20410000,
20000000,
20000000,
20000000,
21250000,
35000000,
20840000,
25830000,
28330000,
23750000,
20000000,
20000000,
20420000,
20000000,
22500000,
20000000,
20000000,
20420000,
20000000,
20000000,
20000000,
20000000,
20410000,
20000000,
20000000,
20000000,
20000000,
20420000,
20000000,
20000000,
20000000,
20000000,
20000000,
20420000,
20000000,
20000000,
20000000,
20000000,
20410000,
20000000,
40840000,
24160000,
52920000,
56670001
};

static const unsigned audio_duration[] = {
20201360,
20201361,
19969161,
20201360,
19969161,
20201361,
19969161,
20201360,
19969161,
20201361,
19969161,
20201360,
19969161,
20201361,
19969161,
20201360,
19969161,
20201361,
20201361,
19969160,
20201361,
19969161,
20201361,
19969161,
20201360,
19969161,
20201361,
19969161,
20201360,
19969161,
20201361,
19969161,
20201360,
19969161,
20201361,
19969161,
20201360,
20201361,
19969161,
20201361,
19969161,
20201360,
19969161,
20201360,
19969162,
20201360,
19969161,
20201360,
19969162,
20201360,
19969161,
20201361,
19969161,
20201360,
20201361,
19969161,
20201360,
19969161,
20201361,
19969161,
20201360,
19969162,
20201360,
19969161,
20201360,
19969162,
20201360,
19969161,
20201361,
19969160,
20201361,
19969161,
20201361,
20201360,
19969161,
20201361,
19969161,
20201360,
19969161,
20201361,
19969161,
20201360,
19969161,
20201361,
19969161,
20201360,
19969161,
20201361,
19969161,
20201361,
20201360,
19969161,
20201361,
19969161,
20201360,
19969161,
20201361,
19969161,
20201360,
19969161,
20201361,
19969161,
20201360,
19969161,
20201361,
19969161,
20201360,
19969161,
20201361,
20201360,
19969162,
20201360,
19969161,
20201360,
19969162,
20201360,
19969161,
20201361,
19969161,
20201360,
19969161,
20201361,
19969161,
20201360,
19969161,
20201361,
20201360,
19969162,
20201360,
19969161,
20201360,
19969162,
20201360,
19969161,
20201361,
19969160,
20201361,
19969161,
20201361,
19969161,
20201360,
19969161,
20201361,
19969161,
20201360,
20201361,
19969161,
20201360,
19969161,
20201361,
19969161,
20201360,
19969161,
20201361,
19969161,
20201361,
19969160,
20201361,
19969161,
20201361,
19969161,
20201360,
20201361,
19969161,
20201360,
19969161,
20201361,
19969161,
20201360,
19969161,
20201361,
19969161,
20201360,
19969161,
20201361,
19969161,
20201361,
19969160,
20201361,
19969161,
20201361,
20201360,
19969161,
20201361,
19969161,
20201360,
19969161,
20201361,
19969161,
20201360,
19969161,
20201361,
19969161,
20201360,
19969161,
20201361,
19969161,
20201361,
20201360,
19969161,
20201360,
19969161,
20201361,
19969161,
20201361,
19969160,
20201361,
19969161,
20201361,
19969161,
20201360,
19969161,
20201361,
19969161,
20201360,
19969161,
20201361,
20201360,
19969161,
20201361,
19969161,
20201361,
19969160,
20201361,
19969161,
20201361,
19969161,
20201360,
19969161,
20201361,
19969161,
20201360,
19969161,
20201361,
20201360,
19969161,
20201361,
19969161,
20201360,
19969161,
20201361,
19969161,
20201361,
19969160,
20201361,
19969161,
20201361,
19969161,
20201360,
19969161,
20201361,
19969161,
20201360,
20201361,
19969161,
20201360,
19969161,
20201361,
19969161,
20201360,
19969161,
20201361,
19969161,
20201361,
19969160,
20201361,
19969161,
20201360,
19969162,
20201360,
20201361,
19969160,
20201361,
19969161,
20201361,
19969161,
20201360,
19969161,
20201361,
19969161,
20201360,
19969161,
20201361,
19969161,
20201360,
19969162,
20201360,
19969161,
20201360,
20201361,
19969161,
20201361,
19969161,
20201360,
19969161,
20201361,
19969161,
20201360,
19969161,
20201361,
19969161,
20201360,
19969161,
20201361,
19969161,
20201360,
20201361,
19969161,
20201361,
19969160,
20201361,
19969161,
20201361,
19969161,
20201360,
19969161,
20201361,
19969161,
20201360,
19969161,
20201361,
19969161,
20201360,
19969161,
20201361,
20201360,
19969161,
13235375
};
#endif



static void play_callback(void *context, int param)
{
    BSTD_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);
}

#define MAX_READ (188*256)

static void 
print_status(NEXUS_VideoDecoderHandle videoDecoder, NEXUS_AudioDecoderHandle audioDecoder, off_t position, const char *state)
{
    NEXUS_Error rc;
    NEXUS_VideoDecoderStatus videoStatus;
    NEXUS_AudioDecoderStatus audioStatus;

    fprintf(stderr,"\r");
    rc=NEXUS_VideoDecoder_GetStatus(videoDecoder, &videoStatus);
    if(rc==NEXUS_SUCCESS) {
        fprintf(stderr, "Video PTS: %10u(%u) video:%ux%u", videoStatus.pts, videoStatus.ptsType, videoStatus.source.width, videoStatus.source.height);
    }
    rc = NEXUS_AudioDecoder_GetStatus(audioDecoder, &audioStatus);
    if(rc==NEXUS_SUCCESS) {
        fprintf(stderr, " Audio PTS: %10u(%u)", audioStatus.pts, audioStatus.ptsType);
    }
    fprintf(stderr, " file: %10u: %-15s\t\t\r", (unsigned)position, state);
    return;
}

static void
wait_for_drain(NEXUS_PlaypumpHandle playpump, NEXUS_VideoDecoderHandle videoDecoder, NEXUS_AudioDecoderHandle audioDecoder, off_t position)
{
    NEXUS_Error rc;
    NEXUS_PlaypumpStatus playpumpStatus;
    NEXUS_VideoDecoderStatus decoderStatus;

    for(;;) {
        rc=NEXUS_Playpump_GetStatus(playpump, &playpumpStatus);
        if(rc!=NEXUS_SUCCESS) {
            break;
        }
        if(playpumpStatus.fifoDepth==0) {
            break;
        }
        /* fprintf(stderr, "---P:%u\n", playpumpStatus.fifoDepth); */
        BKNI_Sleep(100);
        print_status(videoDecoder, audioDecoder, position, "drain playpump");
    }
    for(;;) {
        rc=NEXUS_VideoDecoder_GetStatus(videoDecoder, &decoderStatus);
        if(rc!=NEXUS_SUCCESS) {
            break;
        }
        if(decoderStatus.queueDepth==0) {
            break;
        }
        /* fprintf(stderr, "---D:%u\n", decoderStatus.queueDepth); */
        BKNI_Sleep(100);
        print_status(videoDecoder, audioDecoder, position, "drain decoder");
    }
    return;
}

static int
send_data(const uint8_t *buf, size_t bytes, NEXUS_PlaypumpHandle playpump, BKNI_EventHandle event, NEXUS_VideoDecoderHandle videoDecoder, NEXUS_AudioDecoderHandle audioDecoder, off_t position)
{
    while(bytes>0) {
        void *buffer;
        size_t buffer_size;
        NEXUS_Error rc;

        if (NEXUS_Playpump_GetBuffer(playpump, &buffer, &buffer_size)) {
            fprintf(stderr, "NEXUS_Playpump_GetBuffer failed");
            return -1;
        }
        if (buffer_size == 0) {
            BKNI_WaitForEvent(event, 100/*BKNI_INFINITE*/);
            if(rc==BERR_TIMEOUT) {
                fprintf(stderr, "send_data:timeout\n");
            }
            print_status(videoDecoder, audioDecoder, position, "send_data");
            continue;
        }
        if(buffer_size>bytes) {
            buffer_size = bytes;
        }
        /* fprintf(stderr, "buffer_size %u\n", buffer_size); */
        BKNI_Memcpy(buffer, buf, buffer_size);
        rc = NEXUS_Playpump_ReadComplete(playpump, 0, buffer_size);
        BDBG_ASSERT(!rc);
        bytes-=buffer_size;
        buf += buffer_size;
    }
    return 0;
}

static int
make_fragment(const struct track_info *track_kind, const struct stream_info *track, const unsigned *fragments, unsigned track_id, unsigned fragment_no, mem_file *fout)
{
    size_t offset;
    int codec_private_data_len;
    uint8_t buf[512];
    uint64_t timescale = 10000000;
    uint64_t start_time;
    unsigned i;
    char fname[128];
    FILE *fin;
    const char *kind = track_kind->name;

    for(start_time=0,i=0;i<fragment_no;i++) {
        start_time += fragments[i];
    }
    BKNI_Snprintf(fname, sizeof(fname), "Fragments/%s_%u_%llu", kind, (unsigned)track->bitrate, start_time);
    fprintf(stderr, "\nplaying fragment:%s\n", fname);
    mem_file_seek(fout, 0);
    fout->length = 0;
    offset = start_mp4_box(fout, "bmp4");
    codec_private_data_len = hex2bin(track->codec_private, buf, sizeof(buf));
    write_bhed_box(fout, timescale, start_time, track_kind->fourcc);
    write_bdat_box(fout, buf, codec_private_data_len);
    write_trex_box(fout,  track_id, 0, 0, 0, 0);
    fin = fopen(fname, "rb");
    if(fin==NULL) {
        perror(fname);
        BDBG_ASSERT(fin);
    }
    for(;;) {
        int rc;
        rc = fread(buf, 1, sizeof(buf), fin);
        if(rc<=0) {
            break;
        }
        write_data(fout, buf, rc);
    }
    fclose(fin);
    finish_mp4_box(fout, offset);
    return 0;
}


int main(int argc, char **argv)  {
    NEXUS_PlaypumpSettings playpumpSettings;
    NEXUS_PlaypumpHandle playpump;
    BKNI_EventHandle event;
    const char *fname;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_PidChannelHandle videoPidChannel, audioPidChannel;
    NEXUS_DisplayHandle display;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    uint16_t videoPid, audioPid;
    unsigned i;
    NEXUS_VideoDecoderOpenSettings videoDecoderOpenSettings;
    static const uint8_t video_seq[] = {0,1,2,3,4,5,6,7,8,9,8,7,6,4,3,2,1,0,2,4,6,8,1,3,5,7,9,8,6,4,2,0,9,7,5,3,1,0,5,4,8,7,3};
    static const uint8_t audio_seq[] = {0};
    mem_file buf;
    off_t position;
    NEXUS_AudioDecoderHandle pcmDecoder;
    NEXUS_AudioDecoderStartSettings audioProgram;
    NEXUS_PlaypumpOpenPidChannelSettings videoPidSettings, audioPidSettings;
    NEXUS_PlaypumpOpenSettings playpumpOpenSettings;
    NEXUS_VideoWindowSettings windowSettings;
    NEXUS_VideoWindowMadSettings madSettings;
    NEXUS_VideoWindowScalerSettings scalerSettings;
    NEXUS_DisplaySettings displaySettings;

    if (argc > 1) {
        fname = argv[1];
    }
    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    BKNI_CreateEvent(&event);

    buf.length = 0;
    buf.offset = 0;
    buf.buf_len = 4*1024*1024;
    buf.buf = BKNI_Malloc(buf.buf_len);
    BDBG_ASSERT(buf.buf);
    NEXUS_Playpump_GetDefaultOpenSettings(&playpumpOpenSettings);
    playpumpOpenSettings.fifoSize = buf.buf_len;
    playpump = NEXUS_Playpump_Open(0, &playpumpOpenSettings);

    NEXUS_Playpump_GetSettings(playpump, &playpumpSettings);
    playpumpSettings.dataCallback.callback = play_callback;
    playpumpSettings.dataCallback.context = event;
    playpumpSettings.transportType = NEXUS_TransportType_eMp4Fragment;
    NEXUS_Playpump_SetSettings(playpump, &playpumpSettings);

    NEXUS_Playpump_Start(playpump);
    
    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_eAuto;
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    /* Bring up video decoder and display. No audio to keep the example simple. */
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    /* displaySettings.format = NEXUS_VideoFormat_eNtsc; */
    displaySettings.format = NEXUS_VideoFormat_e720p;
    display = NEXUS_Display_Open(0, &displaySettings);
#if NEXUS_DTV_PLATFORM
    NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
    NEXUS_BoardCfg_ConfigurePanel(true, true, true);
#else
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
    if(displaySettings.format == NEXUS_VideoFormat_eNtsc) {
#if NEXUS_NUM_COMPOSITE_OUTPUTS
        NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
#endif
#if NEXUS_NUM_SVIDEO_OUTPUTS    
        if (platformConfig.outputs.svideo[0]) {
            NEXUS_Display_AddOutput(display, NEXUS_SvideoOutput_GetConnector(platformConfig.outputs.svideo[0]));
        }
#endif    
    }
#endif /* NEXUS_DTV_PLATFORM */
    window = NEXUS_VideoWindow_Open(display, 0);

    NEXUS_VideoWindow_GetSettings(window, &windowSettings);
    windowSettings.scaleFactorRounding.enabled = false;
    windowSettings.minimumSourceFormat = NEXUS_VideoFormat_e1080p;
    NEXUS_VideoWindow_SetSettings(window, &windowSettings);

    NEXUS_VideoWindow_GetScalerSettings(window, &scalerSettings);
    scalerSettings.bandwidthEquationParams.bias = NEXUS_ScalerCaptureBias_eScalerBeforeCapture;
    scalerSettings.bandwidthEquationParams.delta = 1*1000*1000;
    NEXUS_VideoWindow_SetScalerSettings(window, &scalerSettings);

    NEXUS_VideoWindow_GetMadSettings(window, &madSettings);
    madSettings.deinterlace = false;
    NEXUS_VideoWindow_SetMadSettings(window, &madSettings);

    NEXUS_VideoDecoder_GetDefaultOpenSettings(&videoDecoderOpenSettings); /* take default capabilities */
    if(argc>4) {
        videoDecoderOpenSettings.fifoSize = 1024*strtol(argv[4],NULL,0);
    }
    videoDecoder = NEXUS_VideoDecoder_Open(0, &videoDecoderOpenSettings); 
    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));
    /* BKNI_Sleep(500); */

    videoPid = 1;
    if(argc>2) {
        videoPid = strtol(argv[2],NULL,0);
    }
    audioPid = 2;
    if(argc>3) {
        audioPid = strtol(argv[3],NULL,0);
    }

    NEXUS_Playpump_GetDefaultOpenPidChannelSettings(&videoPidSettings);
    NEXUS_Playpump_GetDefaultOpenPidChannelSettings(&audioPidSettings);
    videoPidSettings.pidType = NEXUS_PidType_eVideo;
    audioPidSettings.pidType = NEXUS_PidType_eAudio;
    videoPidChannel = NEXUS_Playpump_OpenPidChannel(playpump, videoPid, &videoPidSettings);
    audioPidChannel = NEXUS_Playpump_OpenPidChannel(playpump, audioPid, &audioPidSettings);

    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = video_track.codec;
    videoProgram.pidChannel = videoPidChannel;
    videoProgram.stcChannel = stcChannel;
    videoProgram.progressiveOverrideMode = NEXUS_VideoDecoderProgressiveOverrideMode_eDisable;
    videoProgram.timestampMode = video_track.codec == NEXUS_VideoCodec_eVc1?NEXUS_VideoDecoderTimestampMode_eDecode:NEXUS_VideoDecoderTimestampMode_eDisplay;

    NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
    audioProgram.codec = audio_track.codec;
    audioProgram.pidChannel = audioPidChannel;
    audioProgram.stcChannel = stcChannel;

    pcmDecoder = NEXUS_AudioDecoder_Open(0, NULL);
    NEXUS_AudioOutput_AddInput(
        NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
        NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));

    NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
    NEXUS_AudioDecoder_Start(pcmDecoder, &audioProgram);
    for(position=0,i=0;;i++) {
        unsigned stream,fragment;
        unsigned video_fragments=sizeof(video_duration)/sizeof(*video_duration);
        unsigned audio_fragments=sizeof(audio_duration)/sizeof(*audio_duration);
        unsigned nvideo_streams = sizeof(video_streams)/sizeof(*video_streams);

        
        stream = video_seq[i%sizeof(video_seq)];
        /* stream = stream + i/nvideo_streams;  */
        stream = stream % nvideo_streams;
        stream = nvideo_streams - 1 - stream;
        fragment = i%video_fragments;
        make_fragment(&video_track, &video_streams[stream], video_duration, 1, fragment, &buf);
        send_data(buf.buf, buf.length, playpump, event, videoDecoder, pcmDecoder, position); 
        position += buf.length;

        stream = audio_seq[i%sizeof(audio_seq)];
        BDBG_ASSERT(stream<sizeof(audio_streams)/sizeof(*audio_streams));
        fragment = i%audio_fragments;
        make_fragment(&audio_track, &audio_streams[stream], audio_duration, 2, fragment, &buf);
        send_data(buf.buf, buf.length, playpump, event, videoDecoder, pcmDecoder, position);
        position += buf.length;
        if(fragment+1==audio_fragments) {
            wait_for_drain(playpump, videoDecoder, pcmDecoder, position);
            fprintf(stderr, "\nloop\n");
        }
    }
    /* loops forever */

    NEXUS_AudioDecoder_Stop(pcmDecoder);
    NEXUS_VideoDecoder_Stop(videoDecoder);
    NEXUS_Playpump_Stop(playpump);
    NEXUS_Playpump_Close(playpump); 
    NEXUS_Platform_Uninit();
    return 0;

}
