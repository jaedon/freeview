/***************************************************************************
 *     Copyright (c) 2006-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bavi_parser.h $
 * $brcm_Revision: 15 $
 * $brcm_Date: 3/2/09 10:26a $
 *
 * Module Description:
 *
 * AVI parser library
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/avi/bavi_parser.h $
 * 
 * 15   3/2/09 10:26a vsilyaev
 * PR 52434: Merge XViD and VC1 AP support in MKV
 * 
 * fix_bdvd_v1.42_pr12986/1   2/18/09 11:21a gooslin
 * Move FourCC defines to bmedia scope
 * 
 * 14   12/4/08 3:29p vsilyaev
 * PR 49993: Added code to inform application about errors detected during
 * stream parsing
 * 
 * 13   11/4/08 9:57a vsilyaev
 * PR 48608: Merged code that alligns audio/video streams after seek.
 * 
 * bdvd_v1.4/bdvd_v1.42/fix_bdvd_v1.42_pr11402/1   11/1/08 7:45p gooslin
 * Cleanup A/V sync modifications
 * 
 * bdvd_v1.4/1   9/23/08 9:51p gooslin
 * PR_10564 [ &nbsp; ].  Fix A/V sync issue when playing ASF files from a
 * point other than the origin
 * 
 * fix_bdvd_v1.4_pr10564/1   9/23/08 4:11p gooslin
 * Parser seek now returns true or false indicating if a seek took place
 * 
 * 12   6/16/08 2:15p vsilyaev
 * PR 39944: Fixed reordering on stream discontinuities
 * 
 * 11   4/25/07 5:11p vsilyaev
 * PR 25701: Fixed documentation
 * 
 * 10   4/23/07 5:35p vsilyaev
 * PR 29921: Added status function
 * 
 * 9   4/23/07 5:31p vsilyaev
 * PR 29921: Added status function
 * 
 * 8   4/19/07 2:26p vsilyaev
 * PR 25701: Removed out-of-date comments
 * 
 * 7   2/28/07 1:11p vsilyaev
 * PR 25701: Added forward and backward seek
 * 
 * 6   2/22/07 5:20p vsilyaev
 * PR 25701: Added code to handle resource starvation
 * 
 * 5   2/9/07 11:16a vsilyaev
 * PR 25701: Renamed bavi_t to bavi_parser_t
 * 
 * 4   2/8/07 11:47p vsilyaev
 * PR 25701: bavi_stream parser
 * 
 * 3   2/6/07 6:48p vsilyaev
 * PR 25701: Modified AVI parser to work with batom framework
 * 
 * 2   12/21/06 7:13p vsilyaev
 * PR 25701: Milestone: functional RIFF/AVI parser
 * 
 * 1   12/18/06 7:23p vsilyaev
 * PR 25701: AVI parser
 * 
 *******************************************************************************/

#ifndef _BAVI_PARSER_H__
#define _BAVI_PARSER_H__

#include "bavi_util.h"
#include "blst_slist.h"

#ifdef __cplusplus
extern "C"
{
#endif


typedef int64_t bavi_off_t;
typedef struct bavi_parser *bavi_parser_t;

typedef struct bavi_parser_status {
    bool data_discontinuity; /* set to true if some data from the stream has been dropped or lost since the last buffer was fed */
	bavi_off_t offset; /* current offset in a stream */
	size_t acc_length; /* number of accumulated bytes */
	size_t obj_length; /* size of the object that parser expects */
	const char *state; /* state of the AVI parser */
    bmedia_parsing_errors errors;
} bavi_parser_status;

typedef enum bavi_parser_action {
	bavi_parser_action_none, /* no special action needed to perform */
	bavi_parser_action_return /* AVI parser shall return control as soon as possible */
} bavi_parser_action;

/* forward declaration of the bavi_object_handler structure */
typedef struct bavi_parser_handler bavi_parser_handler;

/* user defined object handler function */
typedef	bavi_parser_action (*bavi_parser_handler_cb)(
		bavi_parser_handler *handler, /* pointer passed ot bavi_install_object_handler function */
		bavi_fourcc fourcc, /* The FOURCC of the object */
		batom_t object /* object payload */
		);

/* user defined object handler function */
typedef	bavi_parser_action (*bavi_parser_object_begin_cb)(
		void *cntx, /* user context */
		bavi_fourcc fourcc, /* The FOURCC of the object */
		bavi_dword size, 	/* object size */
		bavi_off_t offset	/* offset in the stream */
		);

typedef	bavi_parser_action (*bavi_parser_object_end_cb)(
		void *cntx, /* user context */
		bavi_fourcc fourcc, /* The FOURCC of the object */
		bavi_off_t offset	/* offset in the stream */
		);

/* application level configuration for the AVI parser */
typedef struct bavi_parser_cfg {
	void *user_cntx; /* user context */
	bavi_parser_object_begin_cb object_begin; /* user handler that called at the beginning of the LIST type object */
	bavi_parser_object_end_cb object_end;  /* user handler that called at the end of the LIST type object */
    void (*stream_error)(void *cntx); /* callback that was called when error detectect, this callback can't call back into the avi parser */
} bavi_parser_cfg;

typedef struct bavi_parser_payload_sink {
	void *sink_cntx; /* context passed into the application callbacks */
	void (*payload_flush)(void *sink_cntx); /* function called when there is a discontinuity in the AVI stream */
} bavi_parser_payload_sink;

/* type for user defined object parser */
struct bavi_parser_handler {
	BLST_S_ENTRY(bavi_parser_handler) link; /* field that is used to link handlers together */
	bavi_fourcc	fourcc;	/* FOURCC code of the object */
	bavi_parser_handler_cb handler; /* handler to call when object with specfied FOURCC is found */
};


bavi_parser_t bavi_parser_create(batom_factory_t factory, const bavi_parser_cfg *cfg);
void bavi_parser_reset(bavi_parser_t avi);
void bavi_parser_default_cfg(bavi_parser_cfg *cfg);
void bavi_parser_destroy(bavi_parser_t avi);
size_t bavi_parser_feed(bavi_parser_t avi, batom_pipe_t pipe);
void bavi_parser_reset(bavi_parser_t avi);
int bavi_parser_seek(bavi_parser_t avi, bavi_off_t off);
void bavi_parser_get_status(bavi_parser_t avi, bavi_parser_status *status);

#define BAVI_FOURCC_BEGIN      (bavi_fourcc)0
#define BAVI_FOURCC_END                (bavi_fourcc)(0xFFFFFFFFul)

/* this function is used to install user defined parser for the AVI object,
 * Size of such object is limited to 64K.
 * User could register more that one hander for each object, in later case order which what parsers called is undetermined. 
 */
void bavi_parser_install_handler(
		bavi_parser_t avi,  /* avi parser context */
		bavi_parser_handler *handler,  /* pointer to the user supplied handler, user responsible to keep that object intact untill call to the bavi_remove_object_handler */
		bavi_fourcc fourcc,  /* object id */
		bavi_parser_handler_cb object_handler /* object handler */
		);

/* this function is used to remove handler */
void bavi_parser_remove_handler(bavi_parser_t avi, bavi_parser_handler *handler);

/* this function shall be used to initialized basf_parser_payload_sink type */
void bavi_parser_default_payload_sink(bavi_parser_payload_sink *sink);

/* this function is used to install consumer for the ASF payload */
void bavi_parser_set_payload_sink(bavi_parser_t avi, const bavi_parser_payload_sink *sink);

#ifdef __cplusplus
}
#endif

#endif /* _BAVI_PARSER_H__ */

