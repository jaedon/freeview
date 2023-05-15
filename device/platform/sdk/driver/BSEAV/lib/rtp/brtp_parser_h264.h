/***************************************************************************
 *     Copyright (c) 2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: brtp_parser_h264.h $
 * $brcm_Revision: 4 $
 * $brcm_Date: 9/18/06 4:39p $
 *
 * Module Description:
 *
 * RTP parser library
 *   RFC 3984 module
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/rtp/brtp_parser_h264.h $
 * 
 * 4   9/18/06 4:39p vsilyaev
 * PR 23826: More work for RTP timing
 * 
 * 3   8/28/06 4:35p vsilyaev
 * PR 23826: Added audio+video muxing
 * 
 * 2   8/25/06 2:10p vsilyaev
 * PR 23826: Added MPEG-4 AAC audio parser
 * 
 * 1   8/22/06 4:35p vsilyaev
 * PR 23826: RTP H.264 library
 * 
 *******************************************************************************/
#ifndef _BRTP_PARSER_H264_H__
#define _BRTP_PARSER_H264_H__

#include "brtp.h"
#include "brtp_parser.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct brtp_parser_h264_cfg {
	size_t meta_len;
	void *meta;
}brtp_parser_h264_cfg;

extern const brtp_parser_type *brtp_parser_h264;

void brtp_parser_h264_default_cfg(brtp_parser_h264_cfg *cfg);
brtp_parser_t brtp_parser_h264_create(const brtp_parser_h264_cfg *cfg);

#ifdef __cplusplus
}
#endif

#endif /* _BRTP_PARSER_H264_H__ */

