/***************************************************************************
 *     Copyright (c) 2003-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_namevalue_pairs.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 6/19/07 3:20p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/utils/common/bsettop_namevalue_pairs.h $
 * 
 * 2   6/19/07 3:20p erickson
 * PR32352: add bvideo_frame_rate_pairs
 * 
 * 1   6/19/07 2:59p erickson
 * PR32352: make reusable name/value pair mappings for Settop API enums
 * 
 ********************************************************/
#ifndef BSETTOP_NAMEVALUE_PAIRS_H__
#define BSETTOP_NAMEVALUE_PAIRS_H__

#include "b_namevalue_pairs.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
Common Settop API enum name/value pairs.
See b_namevalue_pairs.h for lookup interface.
*/

extern b_namevalue_pair bdisplay_aspect_ratio_pairs[];
extern b_namevalue_pair bdisplay_content_mode_pairs[];
extern b_namevalue_pair bvideo_format_pairs[];
extern b_namevalue_pair bvideo_codec_pairs[];
extern b_namevalue_pair bvideo_frame_rate_pairs[];
extern b_namevalue_pair baudio_format_pairs[];
extern b_namevalue_pair btuner_sds_mode_pairs[];
extern b_namevalue_pair bmpeg_type_pairs[];
extern b_namevalue_pair bencryption_type_pairs[];

#ifdef __cplusplus
}
#endif

#endif
