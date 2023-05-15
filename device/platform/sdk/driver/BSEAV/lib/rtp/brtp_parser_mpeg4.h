/***************************************************************************
 *     Copyright (c) 2006-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: brtp_parser_mpeg4.h $
 * $brcm_Revision: 3 $
 * $brcm_Date: 12/8/11 3:37p $
 *
 * Module Description:
 *
 * RTP parser library. 
 *   RFC 3640 module
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/rtp/brtp_parser_mpeg4.h $
 * 
 * 3   12/8/11 3:37p vsilyaev
 * SW7425-1797: Merged G711 support for rtp parser mpge4.
 * 
 * SW7425-1797/1   11/18/11 11:11a chengs
 * SW7425-1797: Add G711 support for rtp parser mpge4.
 * 
 * 2   8/28/06 4:35p vsilyaev
 * PR 23826: Added audio+video muxing
 * 
 * 1   8/25/06 2:07p vsilyaev
 * PR 23826: Added MPEG-4 AAC audio parser
 * 
 *******************************************************************************/
#ifndef _BRTP_PARSER_MPEG4_H__
#define _BRTP_PARSER_MPEG4_H__

#include "brtp.h"
#include "brtp_parser.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum brtp_parser_mpeg4_mode 
{
	brtp_parser_mpeg4_aac_hbr,
	brtp_parser_mpeg4_g711,
} brtp_parser_mpeg4_mode;

typedef struct brtp_parser_mpeg4_stream_cfg {
	brtp_parser_mpeg4_mode mode;
	unsigned sizelength;
	unsigned indexlength;
	unsigned headerlength;
	uint8_t config[8];
	size_t config_len;
	unsigned profile;
}brtp_parser_mpeg4_stream_cfg;

typedef uint8_t brtp_parser_mpeg4_cfg ;

void brtp_parser_mpeg4_default_cfg(brtp_parser_mpeg4_cfg *cfg);
brtp_parser_t brtp_parser_mpeg4_create(const brtp_parser_mpeg4_cfg *cfg);
void brtp_parser_mpeg4_default_stream_cfg(brtp_parser_mpeg4_mode mode, brtp_parser_mpeg4_stream_cfg *cfg);


#ifdef __cplusplus
}
#endif

#endif /* _BRTP_PARSER_MPEG4_H__ */

