/***************************************************************************
 *     Copyright (c) 2006-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: basf_stream.h $
 * $brcm_Revision: 14 $
 * $brcm_Date: 1/21/10 5:56p $
 *
 * Module Description:
 *
 * ASF parser library
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/asf/basf_stream.h $
 * 
 * 14   1/21/10 5:56p vsilyaev
 * SW3556-1003: Added option to disable timestamp reordering inside bmedia
 * 
 * 13   11/4/08 9:57a vsilyaev
 * PR 48608: Merged code that alligns audio/video streams after seek.
 * 
 * bdvd_v1.4/bdvd_v1.42/fix_bdvd_v1.42_pr11402/1   11/1/08 7:42p gooslin
 * Cleanup A/V sync modifications
 * 
 * 12   7/20/07 4:17p vsilyaev
 * PR 32846: Removed unused callback and fixed payload and media_object
 * callbacks
 * 
 * 11   7/9/07 3:38p vsilyaev
 * PR 32846: Updated ASF code to work with batom/SPF framework
 * 
 * 10   3/6/07 1:51p ptimariu
 * PR20685: added replicated data to media object
 * 
 * 9   1/12/07 10:26a gmullen
 * PR24746: Added hook to intercept completed media object
 * 
 * 8   9/13/06 1:11p vsilyaev
 * PR 24056: Added code to contruct 'PTS Marked' PES stream for VC1
 * Simple&Main Profiles
 * 
 * 7   7/10/06 4:30p vsilyaev
 * PR 22578: Added support for the 'CodeIn' PES format
 * 
 * 6   5/1/06 2:58p vsilyaev
 * PR 20683: Improved the asf_player support
 * 
 * 5   4/13/06 3:14p vsilyaev
 * PR 20308: Improved API description
 * 
 * 4   4/12/06 10:33a vsilyaev
 * PR 20308: Improved parsing of compressed payload
 * 
 * 3   4/7/06 12:29p vsilyaev
 * PR 20683: added basf_player
 * 
 * 2   4/5/06 4:18p vsilyaev
 * PR 20308: Rerranged data flow to accumulate entire media object, before
 * converting to PES
 * 
 * 1   4/4/06 6:47p vsilyaev
 * PR 20577: Separated ASF parser and ASF stream
 * 
 * 
 *******************************************************************************/
#ifndef _BASF_STREAM_H__
#define _BASF_STREAM_H__

#include "basf_parser.h"
#include "bmedia_util.h"
#include "bmedia_pes.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* opaque type for the ASF demux */
typedef struct basf_demux *basf_demux_t;
/* opaque type for the ASF stream */
typedef struct basf_stream *basf_stream_t;

/* status of the ASF demux */
typedef struct basf_demux_status {
	size_t nstreams; /* number of parsed streams */
} basf_demux_status;


typedef struct basf_demux_cfg {
	unsigned stream_hdr_size;
} basf_demux_cfg;
/* this function shall be used to initialized basf_demux_cfg type */
void basf_demux_default_cfg(basf_demux_cfg *cfg);

/* this function is used to create instance of the demux */
basf_demux_t basf_demux_create(basf_parser_t parser, batom_factory_t factory, balloc_iface_t alloc, const basf_demux_cfg *cfg);

/* this function is free all resources alloced by the ASF demux */
void basf_demux_destroy(basf_demux_t demux);

/* this function is used to flush all data without affecting ASF demux state */
void basf_demux_flush(basf_demux_t demux);

/* this function is used to return the ASF demux to initial state */
void basf_demux_reset(basf_demux_t demux);

/* this function is used to enable passing of only key frames  */
void basf_demux_set_keyframeonly(basf_demux_t demux, bool key_frame_only);

/* this function returns status of the ASF demux */
void basf_demux_status_get(basf_demux_t demux, basf_demux_status *status);

typedef struct basf_stream_cfg {   
	void *stream_cnxt; /* context passed into the stream consumer */
	void (*payload)(void *stream_cnxt, batom_accum_t source, batom_cursor *payload, batom_cursor *replicated_data, const struct basf_payload_info *info); /* function called for each ASF payload */
	void (*media_object) (void *stream_cnxt, batom_cursor *data, const struct basf_payload_info *info); /* Called for each completed media object, and it's used to preprocess data by an application */
    bool reorder_timestamps; /* reorder timestamps from display into the decode order */
} basf_stream_cfg;

/* this function is  used to initialize basf_stream_cfg type */
void basf_stream_initialize(basf_stream_cfg *cfg);

/* this function is used to attach stream to the ASF demux */
basf_stream_t basf_stream_attach(basf_demux_t demux, batom_pipe_t pipe, unsigned stream_number, const basf_stream_cfg *cfg);

/* this function is used to detach stream from the parser */
void basf_stream_detach(basf_demux_t demux, basf_stream_t stream);

void basf_demux_set_timescale(basf_demux_t demux, bmedia_time_scale time_scale);
void basf_stream_get_stream_cfg(basf_stream_t stream, bmedia_pes_stream_cfg *cfg);

#if defined (HUMAX_PLATFORM_BASE)
#define HDR_LICGET_ACTION_VALUE 		"\"http://schemas.microsoft.com/DRM/2007/03/protocols/AcquireLicense\""
#define HDR_LICACK_ACTION_VALUE 		"\"http://schemas.microsoft.com/DRM/2007/03/protocols/AcknowledgeLicense\""

typedef int32_t (*httpclient_license_post_callback_t)(char* url, char* chall, uint8_t non_quiet, 
                               uint32_t app_sec, char* action, unsigned char** resp, 
                               unsigned char** resp_start, uint32_t* out_resp_len);
typedef int32_t (*get_secure_clock_callback_t)(void *pAppContext);

void b_asf_register_post_callback(httpclient_license_post_callback_t soap_callback, httpclient_license_post_callback_t default_callback);
void b_asf_register_get_secure_clock(get_secure_clock_callback_t callback);
#endif
#ifdef __cplusplus
}
#endif

#endif /* _BASF_STREAM_H__ */

