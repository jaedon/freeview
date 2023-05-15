/***************************************************************************
 *     Copyright (c) 2006-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: basf_parser.h $
 * $brcm_Revision: 22 $
 * $brcm_Date: 4/23/10 6:54p $
 *
 * Module Description:
 *
 * ASF parser library
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/asf/basf_parser.h $
 * 
 * 22   4/23/10 6:54p vsilyaev
 * SW3548-2908: Skip over various objects when looking for the simple
 * index
 * 
 * 21   2/19/10 6:17p vsilyaev
 * SWBLURAY-18922: Added parsing of the index object to return distance
 * between video and audio streams.
 * 
 * fix_bdvd_v3.0_issue18922/1   2/19/10 3:52p gooslin
 * Return the index object's size and offset
 * 
 * 20   12/29/09 4:25p vsilyaev
 * SW3556-959: Added ranges for the header, data and simple_index objects
 * 
 * 19   10/14/09 5:33p gmohile
 * SW7405-3208 : Add player support for non indexed asf files
 * 
 * 18   1/29/09 3:21p vsilyaev
 * PR 47122: Limited debug output from the stream error, added up-stream
 * notification about encountered errors
 * 
 * 17   11/4/08 9:56a vsilyaev
 * PR 48608: Merged code that alligns audio/video streams after seek.
 * 
 * bdvd_v1.4/bdvd_v1.42/fix_bdvd_v1.42_pr11402/1   11/1/08 7:42p gooslin
 * Cleanup A/V sync modifications
 * 
 * bdvd_v1.4/1   9/23/08 9:50p gooslin
 * PR_10564 [ &nbsp; ].  Fix A/V sync issue when playing ASF files from a
 * point other than the origin
 * 
 * fix_bdvd_v1.4_pr10564/1   9/23/08 4:10p gooslin
 * Parser seek now returns true or false indicating if a seek took place
 * 
 * 16   7/30/07 12:38p vsilyaev
 * PR 33412: Stop parsing stream if detected fatal error inside structure
 * of ASF stream
 * 
 * 15   7/23/07 5:51p vsilyaev
 * PR 32813: Fixed comments and unnecessary code
 * 
 * 14   7/11/07 1:53p vsilyaev
 * PR 32846: PR 29815: Added method to the sink interface that notifies
 * sink about end of data object.
 * 
 * 13   7/9/07 3:38p vsilyaev
 * PR 32846: Updated ASF code to work with batom/SPF framework
 * 
 * 12   12/22/06 12:10p vsilyaev
 * PR 20308: Fixed typos
 * 
 * 11   5/1/06 2:59p vsilyaev
 * PR 20683: Improved the asf_player support
 * 
 * 10   4/27/06 6:13p vsilyaev
 * PR 20683: Added support for the asf_player into the SettopApi
 * 
 * 9   4/14/06 10:07a vsilyaev
 * PR 20308: Added way to pause a ASF parser from a object handler
 * 
 * 8   4/13/06 3:14p vsilyaev
 * PR 20308: Improved API description
 * 
 * 7   4/7/06 12:29p vsilyaev
 * PR 20683: added basf_player
 * 
 * 6   4/5/06 4:18p vsilyaev
 * PR 20308: Rerranged data flow to accumulate entire media object, before
 * converting to PES
 * 
 * 5   4/4/06 6:49p vsilyaev
 * PR 20577: Separated ASF parser and ASF stream
 * 
 * 4   4/3/06 6:00p vsilyaev
 * PR 20577: added support for index parsing
 * 
 * 3   3/31/06 7:00p vsilyaev
 * PR 20308: Fixed handling of WMVA and WMV3 streams
 * 
 * 2   3/30/06 4:04p vsilyaev
 * PR 20308: Support for looped streams
 * 
 * 1   3/28/06 11:01a vsilyaev
 * PR 20308: ASF parser library
 * 
 *******************************************************************************/

#ifndef _BASF_PARSER_H__
#define _BASF_PARSER_H__

#include "basf_util.h"
#include "bmedia_util.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* opaque type for the ASF parser */
typedef struct basf_parser *basf_parser_t;

/* information extracted from the ASF payload */
typedef struct basf_payload_info {
	unsigned stream_number;
	bool key_frame;
	size_t payload_length;
	size_t replicated_data_length;
	basf_dword media_object_size;
	basf_dword offset_into_media_object;
	basf_dword pres_time;
} basf_payload_info;

/* #include "basf_stream.h" */

/* information extracted from header of the ASF Simple Index Object */
typedef struct basf_simple_index {
    basf_qword  object_offset; /* offset of the data object */
    basf_dword  object_size; /* size of the data object */
	basf_qword	offset; /* index offset in the ASF stream */
	basf_qword	index_entry_time_interval;
	basf_dword	maximum_packet_count;
	basf_dword	index_entries_count;
} basf_simple_index;

/* information extracted from header of the ASF Index Object */
typedef struct basf_index {
    basf_qword  object_offset; /* offset of the data object */
    basf_dword  object_size; /* size of the data object */
	basf_qword	offset; /* index offset in the ASF stream */
	basf_dword	index_entry_time_interval;
	basf_word 	index_specifiers_count;
	basf_dword	index_blocks_count;
} basf_index;

/* information extracted from the ASF File Info Object */
typedef struct basf_file_info {
	basf_qword preroll;
	basf_dword pkt_size;
	bool	broadcast;
	bool    seekable;
	basf_dword bitrate;
	basf_qword duration;  
} basf_file_info;

/* information about ASF data object */
typedef struct basf_data_info {
    basf_qword object_offset; /* offset of the data object */
    basf_qword object_size; /* size of the data object */
	basf_qword data_offset; /* payload of data object offset inside ASF stream */
	basf_qword total_data_packets; /* number of data packets in the data object */
	basf_qword data_object_offset; /* data object offset inside the ASF stream */
} basf_data_info;

/* state of the ASF parser */
typedef enum basf_parser_state {
	basf_parser_state_top=200, /* parser handles top level syntax elements */
	basf_parser_state_header_object, /* parser handles the ASF Header object */
	basf_parser_state_data_object /* parser handles the ASF Data object */
} basf_parser_state;


/* information about ASF header object */
typedef struct basf_header_info {
	basf_qword offset; /* payload of the ASF data object */
    basf_word  size; /* size of the ASF header object */
} basf_header_info;

typedef struct basf_parser_info {
	basf_parser_state state; /* current state of the parser */
	bool paused; /* set to true if parser is paused */
	bool stream_error; /* set to true unrecoverable error was detected */
    bool data_discontinuity; /* set to true if some data from the stream has been dropped or lost since the last buffer was fed */
	basf_qword offset; /* current offset in a stream */
	basf_data_info data; /* data object information */ 
	basf_file_info file; /* file object information */
    basf_header_info header; /* stream header information */
    bmedia_parsing_errors errors; /* error statistics */
} basf_parser_info;


typedef enum basf_parser_handler_action {
	basf_parser_handler_action_none, /* no special action needed to perform */
	basf_parser_handler_action_return, /* ASF parser shall return control as soon as possible */
	basf_parser_handler_action_pause  /* ASF parser shall return control as ssoon as possible and stop processing data */
} basf_parser_handler_action;

/* forward declaration of the basf_parser_object_handler structure */
struct basf_parser_object_handler;
/* format of user defined object handler function */
typedef	basf_parser_handler_action (*basf_parser_object_handler_cb)(
		struct basf_parser_object_handler *handler, /* pointer passed to basf_parser_install_object_handler function */
		batom_t data /* atom starts from the first byte after the object size */
		);

/* type for user defined object parser */
typedef struct basf_parser_object_handler {
	BLST_S_ENTRY(basf_parser_object_handler) link; /* field that is used to link handlers together */
	const basf_guid *guid; /* object GUID */
	basf_parser_object_handler_cb handler; /* handler to call when object with specfied GUID is found */
} basf_parser_object_handler;

/* application level configuration for the ASF parser */
typedef struct basf_parser_cfg {
	void *application_cnxt; /* context passed into the application callbacks */
	void (*simple_index)(void *application_cnxt, const basf_simple_index *index); /* function called when the Simple Index Object is found */
	void (*index)(void *application_cnxt, const basf_index *index); /* function called when Index Object is found */
    void (*object)(void *application_cnxt, const basf_guid *guid, basf_qword object_offset,basf_qword  object_size); /* function called on start of each ASF object */
    void (*stream_error)(void *cntx); /* callback that was called when error detectect, this callback can't call back into the asf parser */
} basf_parser_cfg;

typedef struct basf_parser_payload_sink {
	void *sink_cnxt; /* context passed into the application callbacks */
	void (*payload)(void *sink_cnxt, batom_accum_t source, const batom_cursor *payload, batom_cursor *replicated_data, const struct basf_payload_info *info); /* function called for each ASF payload */
	void (*payload_error)(void *sink_cnxt); /* function called when there is an error detected in the ASF packet */
	void (*payload_flush)(void *sink_cnxt); /* function called when there is a discontinuity in the ASF stream */
	void (*payload_end) (void *sink_cnxt); /* function called at the end of data object */
} basf_parser_payload_sink;

/* this function shall be used to initialized basf_parser_cfg type */
void basf_parser_default_cfg(basf_parser_cfg *cfg);

/* this function is used to create instance of the parser */
basf_parser_t basf_parser_create(batom_factory_t factory, const basf_parser_cfg *cfg);

/* this function is used to free all resources allocated by basf_create function */
void basf_parser_destroy(basf_parser_t asf);

/* this function is used to return the ASF parser to the initial state */
void basf_parser_reset(basf_parser_t asf);

/* this function shall be used to initialized basf_parser_payload_sink type */
void basf_parser_default_payload_sink(basf_parser_payload_sink *sink);

/* this function is used to install consumer for the ASF payload */
void basf_parser_set_payload_sink(basf_parser_t asf, const basf_parser_payload_sink *sink);

/* application calls pushes data into the AsfParser using basf_feed function */
size_t basf_parser_feed(
		basf_parser_t asf, /* instance of the basf_parser_t */
		batom_pipe_t pipe /* source for the data */
		);

/* this function is used to retreave information about parser progress */
void basf_parser_get_info(basf_parser_t asf, basf_parser_info *info);

/* this function is called to notify parser that next block of data will come from new offset */
void basf_parser_seek(basf_parser_t asf, basf_qword offset);

/* this function is used to install user defined parser for the ASF object,
 * user could register parsers for the ASF objects inside Header Object including Header Extension object
 * Size of such object is limited to 64K.
 * User could register more that one hander for each object, in later case order which what parsers called is undetermined. 
 */
void basf_parser_install_object_handler(
		basf_parser_t asf,  /* asf parser context */
		basf_parser_object_handler *handler,  /* pointer to the user supplied handler, user responsible to keep that object intact untill call to the basf_parser_remove_object_handler */
		const basf_guid *guid,  /* object guid, user responsible to keep that guid intact untill call to the basf_parser_remove_object_handler */
		basf_parser_object_handler_cb object_handler /* object handler */
		);

/* this function is used to remove handler */
void basf_parser_remove_object_handler(basf_parser_t asf, basf_parser_object_handler *handler);


#ifdef __cplusplus
}
#endif

#endif /* _BASF_PARSER_H__ */

