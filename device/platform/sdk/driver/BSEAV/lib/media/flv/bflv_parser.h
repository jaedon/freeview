/***************************************************************************
 *     Copyright (c) 2007-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bflv_parser.h $
 * $brcm_Revision: 7 $
 * $brcm_Date: 6/29/11 8:55p $
 *
 * Module Description:
 *
 * Media parser library,  FLV (Flash Video) format
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/flv/bflv_parser.h $
 * 
 * 7   6/29/11 8:55p vsilyaev
 * SW7231-268: Added support for AAC audio and improved timestamp handling
 * for AVC video
 * 
 * 6   6/6/11 4:30p vsilyaev
 * SW7422-454: Added S263 support
 * 
 * 5   5/3/11 5:02p vsilyaev
 * SWBLURAY-22800: Refactored support of H.264 in FLV
 * 
 * 4   5/3/11 11:53a vsilyaev
 * SWBLURAY-22800: Merged H.264 in FLV to mainline
 * 
 * dev_bdvd_v3.0_SWBLURAY-22800/1   4/25/11 1:20p ialauder
 * Added ability to extract the ES from a FLV when generating the PES
 * rather than the FLV tags
 * 
 * 3   11/23/10 3:19p vsilyaev
 * SW7422-107: Added support for encapsulation of VP6 payloads
 * 
 * 2   7/11/08 4:01p vsilyaev
 * PR 31188: Added support for FLV probe
 * 
 * 1   5/18/07 12:33a vsilyaev
 * PR 31188: FLV parser library
 * 
 *******************************************************************************/
#ifndef _BFLV_PARSER_H__
#define _BFLV_PARSER_H__

#include "bmedia_pes.h"
#include "blst_slist.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define B_FLV_TAG_AUDIO 8
#define B_FLV_TAG_VIDEO 9

#define B_FLV_CODECID_S263 2
#define B_FLV_CODECID_ON2_VP6   4
#define B_FLV_CODECID_ON2_VP6_ALPHA   5
#define B_FLV_CODECID_H264 7

#define B_FLV_SOUNDFORMAT_MP3_8KHZ  14
#define B_FLV_SOUNDFORMAT_MP3  2
#define B_FLV_SOUNDFORMAT_AAC   10

typedef struct bflv_parser *bflv_parser_t;

typedef enum bflv_parser_action {
	bflv_parser_action_none, /* no special action needed to perform */
	bflv_parser_action_return /* FLV parser shall return control as soon as possible */
} bflv_parser_action;


typedef struct bflv_parser_handler bflv_parser_handler;
typedef	bflv_parser_action (*bflv_parser_handler_cb)(
		bflv_parser_handler *handler, /* pointer passed ot bflv_install_handler function */
		batom_t object, /* object payload */
        uint8_t meta /* meta information from the payload */
		);

/* type for the user defined object parser */
struct bflv_parser_handler {
	BLST_S_ENTRY(bflv_parser_handler) link; /* field that is used to link handlers together */
	uint8_t tag_type;
	bflv_parser_handler_cb handler; /* handler to call when object with specfied ID is found */
};

typedef struct bflv_parser_cfg {
	balloc_iface_t alloc;
} bflv_parser_cfg;

size_t bflv_parser_feed(bflv_parser_t parser, batom_pipe_t pipe);
void bflv_parser_default_cfg(bflv_parser_cfg *cfg);
bflv_parser_t bflv_parser_create(batom_factory_t factory, const bflv_parser_cfg *cfg);
void bflv_parser_destroy(bflv_parser_t parser);
void bflv_parser_reset(bflv_parser_t parser);
void bflv_parser_install_handler(bflv_parser_t parser, bflv_parser_handler *handler, uint8_t tag_type, bflv_parser_handler_cb object_handler);
void bflv_parser_remove_handler(bflv_parser_t parser, bflv_parser_handler *handler);
void bflv_parser_get_stream_cfg(bflv_parser_t parser, bmedia_pes_stream_cfg *cfg);

#ifdef __cplusplus
}
#endif

#endif /* _BFLV_PARSER_H__ */


