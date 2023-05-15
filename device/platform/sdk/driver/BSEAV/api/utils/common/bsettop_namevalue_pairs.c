/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_namevalue_pairs.c $
 * $brcm_Revision: 13 $
 * $brcm_Date: 12/16/11 7:18p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/utils/common/bsettop_namevalue_pairs.c $
 * 
 * 13   12/16/11 7:18p jgarrett
 * SW7425-1406: Adding OGG/Vorbis
 * 
 * 12   5/18/11 7:24p jgarrett
 * SW7231-148: Adding RMFF and Cook support
 * 
 * 11   2/15/11 2:13p nickh
 * SW7422-213: Add SVC/MVC codecs
 * 
 * 11   2/15/11 2:10p nickh
 * SW7422-213: Add SVC/MVC codecs
 * 
 * 10   7/15/09 7:42p vsilyaev
 * PR 55653: Added WAV format
 * 
 * 9   6/25/09 3:34p katrep
 * PR55809: Add support for PCM audio in AVI files
 * 
 * 8   3/18/09 12:31p erickson
 * PR53340: add 1080p 50hz support
 * 
 * 7   12/9/08 10:36a gmohile
 * PR 43467 : Add DRA support
 * 
 * 6   11/15/08 7:10a agin
 * PR48985: Support LDPC and Turbo at the same time.
 * 
 * 5   9/2/08 3:57p katrep
 * PR44143: AAC plus loas is same as AAC plus with code 0x11. ADTS should
 * be 0x12.
 * 
 * 4   3/3/08 12:56p vsilyaev
 * PR 39818: Added MKV support
 * 
 * 3   1/25/08 3:20p katrep
 * PR38429: Added support for AVS
 * 
 * 2   6/19/07 3:20p erickson
 * PR32352: add bvideo_frame_rate_pairs
 * 
 * 1   6/19/07 2:59p erickson
 * PR32352: make reusable name/value pair mappings for Settop API enums
 * 
 ********************************************************/
#include "b_namevalue_pairs.h"
#include "bsettop.h"

/*
Common Settop API enum name/value pairs

- There is not a one-for-one mapping. First match wins.
- Because first match wins, we list the most common name first.
- The index of the name is not the same as the value. Using enums and array indices is inherently dangerous.
*/

b_namevalue_pair bvideo_format_pairs[] = {
    {"NTSC", bvideo_format_ntsc},
    {"PAL", bvideo_format_pal},
    {"PAL_M", bvideo_format_pal_m},
    {"PAL_N", bvideo_format_pal_n},
    {"PAL_NC", bvideo_format_pal_nc},
    {"SECAM", bvideo_format_secam},
    {"480p", bvideo_format_480p},
    {"576p", bvideo_format_576p},
    {"1080i", bvideo_format_1080i},
    {"1080ix50", bvideo_format_1080i_50hz},
    {"1080p", bvideo_format_1080p},
    {"1080px24", bvideo_format_1080p_24hz},
    {"1080px25", bvideo_format_1080p_25hz},
    {"1080px30", bvideo_format_1080p_30hz},
    {"1080px50", bvideo_format_1080p_50hz},
    {"1250ix50", bvideo_format_1250i_50hz},
    {"720p", bvideo_format_720p},
    {"720px50", bvideo_format_720p_50hz},
    {"720px24", bvideo_format_720p_24hz},
    {NULL, 0}
};

b_namevalue_pair baudio_format_pairs[] = {
    {"MPEG", baudio_format_mpeg},
    {"MPEG1", baudio_format_mpeg},
    {"MPEG1L2", baudio_format_mpeg},
    {"MP3", baudio_format_mp3},
    {"MPEG1L3", baudio_format_mp3},
    {"AC3", baudio_format_ac3},
    {"AAC", baudio_format_aac},
    {"AAC+", baudio_format_aac_plus}, /* loas is same as aac plus */
    {"AAC+ADTS", baudio_format_aac_plus_adts}, 
    {"DTS", baudio_format_dts},
    {"AC3+", baudio_format_ac3_plus},
    {"DDP", baudio_format_ac3_plus},
    {"DTSHD", baudio_format_dts_hd},
    {"WMA", baudio_format_wma_std},
    {"WMASTD", baudio_format_wma_std},
    {"WMAPRO", baudio_format_wma_pro},
    {"LPCM_DVD", baudio_format_lpcm_dvd},
    {"LPCM_HDDVD", baudio_format_lpcm_hddvd},
    {"LPCM_BLURAY", baudio_format_lpcm_bluray},
    {"AVS", baudio_format_avs},
    {"PCM", baudio_format_pcm},
    {"DRA", baudio_format_dra},
    {"COOK", baudio_format_cook},
    {"VORBIS", baudio_format_vorbis},
    {NULL, 0}
};

b_namevalue_pair bvideo_codec_pairs[] = {
    {"MPEG", bvideo_codec_mpeg2},
    {"MPEG2", bvideo_codec_mpeg2},
    {"MPEG1", bvideo_codec_mpeg1},
    {"AVC", bvideo_codec_h264},
    {"H.264", bvideo_codec_h264},
    {"H264", bvideo_codec_h264},
    {"SVC", bvideo_codec_h264_svc},
    {"MVC", bvideo_codec_h264_mvc},
    {"H263", bvideo_codec_h263},
    {"H.263",bvideo_codec_h263},
    {"VC1", bvideo_codec_vc1},
    {"VC-1", bvideo_codec_vc1},
    {"VC1-SM", bvideo_codec_vc1_sm},
    {"VC1SM", bvideo_codec_vc1_sm},
    {"VC-1SM", bvideo_codec_vc1_sm},
    {"VC-1-SM", bvideo_codec_vc1_sm},
    {"DivX3", bvideo_codec_divx_311},
    {"DivX-3", bvideo_codec_divx_311},
    {"MPEG4", bvideo_codec_mpeg4_part2},
    {"MPEG4-2", bvideo_codec_mpeg4_part2},
    {"AVS", bvideo_codec_avs},
    {NULL, 0}
};

b_namevalue_pair btuner_sds_mode_pairs[] = {
    {"DVB", btuner_sds_mode_qpsk_dvb},
    {"DSS", btuner_sds_mode_qpsk_dss},
    {"DCII", btuner_sds_mode_qpsk_dcii},
    {"QPSK_TURBO", btuner_sds_mode_qpsk_turbo},
    {"8PSK_TURBO", btuner_sds_mode_8psk_turbo},
    {"TURBO", btuner_sds_mode_turbo},
    {"8PSK_LDPC",btuner_sds_mode_8psk_ldpc},
    {"8PSK",btuner_sds_mode_8psk_ldpc},
    {"QPSK_LDPC",btuner_sds_mode_qpsk_ldpc},
    {"LDPC",btuner_sds_mode_ldpc},
    {NULL, 0}
};

b_namevalue_pair bmpeg_type_pairs[] = {
    {"ES", bstream_mpeg_type_es},
    {"BES", bstream_mpeg_type_bes},
    {"DSS", bstream_mpeg_type_dss_es},
    {"DSS_ES", bstream_mpeg_type_dss_es},
    {"DSS_PES", bstream_mpeg_type_dss_pes},
    {"PES", bstream_mpeg_type_pes},
    {"VOB", bstream_mpeg_type_vob},
    {"TS", bstream_mpeg_type_ts},
    {"ASF", bstream_mpeg_type_asf},
    {"AVI", bstream_mpeg_type_avi},
    {"MPEG1", bstream_mpeg_type_mpeg1},
    {"VCD", bstream_mpeg_type_mpeg1},
    {"MP4", bstream_mpeg_type_mp4},
    {"FLV", bstream_mpeg_type_flv},
    {"MKV", bstream_mpeg_type_mkv},
    {"WAV", bstream_mpeg_type_wav},
    {"RMFF", bstream_mpeg_type_rmff},
    {"OGG", bstream_mpeg_type_ogg},
    {NULL, 0}
};

b_namevalue_pair bencryption_type_pairs[] = {
    {"DES", bencryption_type_des},
    {"3DES", bencryption_type_3des},
    {"NONE", bencryption_type_none},
    {"AES", bencryption_type_aes},
    {"DVB", bencryption_type_dvb},
    {"SHA1", bencryption_type_sha1},
    {"MD5", bencryption_type_md5},
    {"RSA", bencryption_type_rsa},
    {"DSA", bencryption_type_dsa},
    {"RC4", bencryption_type_rc4},
    {"C2", bencryption_type_c2},
    {"CSS", bencryption_type_css},
    {NULL, 0}
};

b_namevalue_pair bdisplay_aspect_ratio_pairs[] = {
    {"4x3", bdisplay_aspect_ratio_4x3},
    {"16x9", bdisplay_aspect_ratio_16x9},
    {NULL, 0}
};

b_namevalue_pair bdisplay_content_mode_pairs[] = {
    {"box", bdisplay_content_mode_box},
    {"zoom", bdisplay_content_mode_zoom},
    {"panscan", bdisplay_content_mode_panscan},
    {NULL, 0}
};

b_namevalue_pair bvideo_frame_rate_pairs[] = {
    {"23.976", bvideo_frame_rate_23_976},
    {"24", bvideo_frame_rate_24},
    {"25", bvideo_frame_rate_25},
    {"29.97", bvideo_frame_rate_29_97},
    {"30", bvideo_frame_rate_30},
    {"50", bvideo_frame_rate_50},
    {"59.94", bvideo_frame_rate_59_94},
    {"60", bvideo_frame_rate_60},
    {NULL, 0}
};

