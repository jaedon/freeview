/***************************************************************************
 *     Copyright (c) 2006-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: basf_parser.c $
 * $brcm_Revision: 45 $
 * $brcm_Date: 1/18/11 11:37a $
 *
 * Module Description:
 *
 * ASF parser library
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/asf/basf_parser.c $
 * 
 * 45   1/18/11 11:37a vsilyaev
 * SW7125-800: Don't abort parsing for broadcast files with unbounded size
 * of the data object
 * 
 * 44   4/23/10 6:54p vsilyaev
 * SW3548-2908: Skip over various objects when looking for the simple
 * index
 * 
 * 43   4/19/10 11:53a vsilyaev
 * SW3548-2890: Added missing typecasts where multiplying 32-bit integeres
 * expecting 64-bit result
 * 
 * 42   2/19/10 6:16p vsilyaev
 * SWBLURAY-18922: Added parsing of the index object to return distance
 * between video and audio streams.
 * 
 * fix_bdvd_v3.0_issue18922/1   2/19/10 3:50p gooslin
 * Return the index object's size and offset
 * 
 * 41   12/29/09 4:25p vsilyaev
 * SW3556-959: Added ranges for the header, data and simple_index objects
 * 
 * 40   10/14/09 5:33p gmohile
 * SW7405-3208 : Add player support for non indexed asf files
 * 
 * 39   6/7/09 11:31a mward
 * PR 55785: Coverity Defect ID:14363 USE_AFTER_FREE basf_parser.c
 * Product=97400nexus
 * 
 * 38   4/28/09 5:54p vsilyaev
 * PR 53911: Added support for processing of the ASF hidden tracks
 * 
 * 37   3/6/09 11:17a vsilyaev
 * PR 52903: Remove extra debug output and this resolved false use afte
 * free
 * 
 * 36   1/29/09 3:21p vsilyaev
 * PR 47122: Limited debug output from the stream error, added up-stream
 * notification about encountered errors
 * 
 * 35   11/4/08 9:56a vsilyaev
 * PR 48608: Merged code that alligns audio/video streams after seek.
 * 
 * 34   10/22/08 2:56p vishk
 * PR 48072: Coverity Defect ID:12264 FORWARD_NULL basf_parser.c
 * Product=97401linux
 * 
 * 33   8/5/08 12:54p vsilyaev
 * PR 45288: Added code to handle partial packets
 * 
 * 32   7/17/08 4:02p gmohile
 * PR 44804: Check for broadcast flag and num of packets before sending
 * EOS
 * 
 * 31   12/6/07 5:23p vsilyaev
 * PR 37574: Improved debug messages
 * 
 * 30   10/4/07 4:56p jtna
 * PR35269: LIB-Coverity (CID 363): NEGATIVE_RETURNS. Added Coverity
 * comments to remove defect flags.
 * 
 * 29   7/30/07 12:38p vsilyaev
 * PR 33412: Stop parsing stream if detected fatal error inside structure
 * of ASF stream
 * 
 * 28   7/17/07 4:31p vsilyaev
 * PR 32813: Added MPEG-2 TS support for the probe library
 * 
 * 27   7/11/07 5:27p vsilyaev
 * PR 32846: Fixed some typos
 * 
 * 26   7/11/07 1:53p vsilyaev
 * PR 32846: PR 29815: Added method to the sink interface that notifies
 * sink about end of data object.
 * 
 * 25   7/10/07 5:25p vsilyaev
 * PR 32813: Added function to parse file properties
 * 
 * 24   7/9/07 3:38p vsilyaev
 * PR 32846: Updated ASF code to work with batom/SPF framework
 * 
 * 22   3/29/07 5:20p vsilyaev
 * PR 29199: Better data validation of the ASF packet
 * 
 * 21   1/26/07 4:50p vsilyaev
 * PR 27347: Added recovery code to BASF_ASSERT
 * 
 * 20   12/22/06 12:10p vsilyaev
 * PR 20308: Fixed typos
 * 
 * 19   10/6/06 1:44p vsilyaev
 * PR24127: cleared up the word 'current'
 * 
 * 18   8/22/06 6:35p vsilyaev
 * PR 23791: Optimized test for padding_length
 * 
 * 17   8/22/06 6:31p vsilyaev
 * PR 23791: Infer padding_length from the packet_length
 * 
 * 16   7/10/06 4:32p vsilyaev
 * PR 20308: Added more invariant tests
 * 
 * 15   5/2/06 5:51p vsilyaev
 * PR 20683: Added support for the navigation in ASF files
 * 
 * 14   4/27/06 6:13p vsilyaev
 * PR 20683: Added support for the asf_player into the SettopApi
 * 
 * 13   4/19/06 10:13a vsilyaev
 * PR 20308: Improved audio parsing
 * 
 * 12   4/14/06 10:07a vsilyaev
 * PR 20308: Added way to pause a ASF parser from a object handler
 * 
 * 11   4/13/06 3:14p vsilyaev
 * PR 20308: Improved API description
 * 
 * 10   4/12/06 10:33a vsilyaev
 * PR 20308: Improved parsing of compressed payload
 * 
 * 9   4/7/06 12:29p vsilyaev
 * PR 20683: added basf_player
 * 
 * 8   4/5/06 4:18p vsilyaev
 * PR 20308: Rerranged data flow to accumulate entire media object, before
 * converting to PES
 * 
 * 7   4/4/06 6:49p vsilyaev
 * PR 20577: Separated ASF parser and ASF stream
 * 
 * 6   4/3/06 10:17p vsilyaev
 * PR 20577: Added  basf_parser_seek function
 * 
 * 5   4/3/06 6:00p vsilyaev
 * PR 20577: added support for index parsing
 * 
 * 4   3/31/06 7:00p vsilyaev
 * PR 20308: Fixed handling of WMVA and WMV3 streams
 * 
 * 3   3/31/06 2:09p vsilyaev
 * PR 20308: Improved debug output
 * 
 * 2   3/30/06 4:04p vsilyaev
 * PR 20308: Support for looped streams
 * 
 * 1   3/28/06 11:01a vsilyaev
 * PR 20308: ASF parser library
 * 
 *******************************************************************************/
#include "bstd.h"
#include "basf_parser.h"
#include "bkni.h"
#include "basf_impl.h"
#include "blst_list.h"
#include "biobits.h"

#undef current

BDBG_MODULE(basf_parser);

#define B_ASF_MAX_FEED_ITEMS	128
#define B_ASF_MAX_OBJECT_LEN	65536
#define B_ASF_MIN_PKT_SIZE		64

#ifdef PROFILE
#undef BDBG_MSG
#define BDBG_MSG(x)
#endif
#define BDBG_MSG_FLOW(x)  /* BDBG_MSG(x) */
#define BDBG_MSG_TRACE(x) /* BDBG_MSG(x) */


#define B_ASF_MAX_PACKET_CNT ((size_t)(-1))


enum b_asf_parser_flow_state {
	b_asf_parser_flow_state_header=100,
	b_asf_parser_flow_state_capture_object,
	b_asf_parser_flow_state_skip,
	b_asf_parser_flow_state_object,
	b_asf_parser_flow_state_data_pkt
};

#define B_ASF_SET_FLOW_STATE(asf, new_state) do { BDBG_MSG_FLOW(("%s:%d flow_state(%d)<-%d'%s'", __FILE__, __LINE__, (asf)->state.flow_state, new_state, #new_state)); asf->state.flow_state=new_state;} while(0)
#define B_ASF_SET_PARSER_STATE(asf, new_state) do { BDBG_MSG(("%s:%d parser_state(%d)<-%d'%s'", __FILE__, __LINE__, (asf)->state.parser_state, new_state, #new_state)); asf->state.parser_state=new_state;} while(0)

typedef bool (*b_asf_parser_object)(basf_parser_t asf, batom_cursor *cursor);

BDBG_OBJECT_ID(basf_parser_t);

struct basf_parser {
	BDBG_OBJECT(basf_parser_t)
	basf_parser_cfg cfg;
	bool has_payload_sink;
	basf_parser_payload_sink payload_sink;
	BLST_S_HEAD(b_asf_object_handlers, basf_parser_object_handler) object_handlers;
	batom_accum_t acc;
	struct {
		size_t capture_len;
		basf_qword skip_len;
		enum b_asf_parser_flow_state flow_state;
		enum basf_parser_state parser_state;
		basf_qword acc_off; /* stream offset for accumulator */
		bool paused;
		bool stream_error;
        bool data_discontinuity;
        uint16_t invalid_packet_cnt;
		basf_payload_info info;
        bmedia_parsing_errors errors; 
		struct {
			basf_guid guid;
			basf_qword size;
			b_asf_parser_object parser;
			union {
				struct {
					basf_dword nobjects;
					size_t cur_object;
				} header;
				struct {
					size_t packet;
				} data;
			} u;
		} object;
		basf_data_info data;
		basf_file_info file;
        basf_header_info header; 
		size_t data_npackets;

	} state;
	char guid_str[64];
};

static void 
b_asf_parser_simple_index(void *application_cnxt, const basf_simple_index *index)
{
	BSTD_UNUSED(application_cnxt);
	BSTD_UNUSED(index);
	return;
}

static void 
b_asf_parser_index(void *application_cnxt, const basf_index *index)
{
	BSTD_UNUSED(application_cnxt);
	BSTD_UNUSED(index);
	return;
}

void
b_asf_parser_assert_failed(basf_parser_t asf, batom_cursor *cursor, const char *expr, const char *file, unsigned line)
{
	BSTD_UNUSED(expr);
	BSTD_UNUSED(file);
	BSTD_UNUSED(line);

    asf->state.errors.format_errors++;
    if(asf->state.errors.format_errors<4) {
        BDBG_ERR(("%#lx BASF_ASSERT(%s) Failed at %s:%u %lu", (unsigned long)asf, expr, file, line, (unsigned long)asf->state.acc_off + batom_cursor_pos(cursor)));
        batom_cursor_dump(cursor, "asf buffer"); 
    } else {
        BDBG_MSG(("%#lx BASF_ASSERT(%s) Failed at %s:%u %lu", (unsigned long)asf, expr, file, line, (unsigned long)asf->state.acc_off + batom_cursor_pos(cursor)));
    }
    asf->cfg.stream_error(asf->cfg.application_cnxt);
	return;
}

static void
b_asf_empty_stream_error_cb(void *cntx)
{
	BSTD_UNUSED(cntx);
	return ;
}

static void 
b_asf_empty_object(void *application_cnxt, const basf_guid *guid, basf_qword object_offset,basf_qword  object_size)
{
    BSTD_UNUSED(application_cnxt);
    BSTD_UNUSED(guid);
    BSTD_UNUSED(object_offset);
    BSTD_UNUSED(object_size);
    return;
}


void 
basf_parser_default_cfg(basf_parser_cfg *cfg)
{
	BDBG_ASSERT(cfg);
	BKNI_Memset(cfg, 0, sizeof(*cfg));
	cfg->simple_index = b_asf_parser_simple_index;
	cfg->index = b_asf_parser_index;
    cfg->stream_error = b_asf_empty_stream_error_cb;
    cfg->object = b_asf_empty_object;
	return;
}

static void
b_asf_parser_payload_drop(void *sink_cnxt, batom_accum_t source, const batom_cursor *packet, batom_cursor *replicated_data, const struct basf_payload_info *info)
{
	BSTD_UNUSED(sink_cnxt);
	BSTD_UNUSED(source);
	BSTD_UNUSED(packet);
	BSTD_UNUSED(replicated_data);
	BSTD_UNUSED(info);
	return;
}

static void 
b_asf_parser_payload_ignore(void *sink_cnxt)
{
	BSTD_UNUSED(sink_cnxt);
	return;
}

void 
basf_parser_default_payload_sink(basf_parser_payload_sink *sink)
{
	BDBG_ASSERT(sink);
	sink->sink_cnxt = NULL;
	sink->payload = b_asf_parser_payload_drop;
	sink->payload_error = b_asf_parser_payload_ignore;
	sink->payload_flush = b_asf_parser_payload_ignore;
	sink->payload_end = b_asf_parser_payload_ignore;
	return;
}

void 
basf_parser_set_payload_sink(basf_parser_t asf, const basf_parser_payload_sink *sink)
{
	BDBG_OBJECT_ASSERT(asf, basf_parser_t);
	if(sink) {
		asf->payload_sink = *sink;
		asf->has_payload_sink = true;
	} else {
		asf->has_payload_sink = false;
		basf_parser_default_payload_sink(&asf->payload_sink);
	}
	return;
}

basf_parser_t 
basf_parser_create(batom_factory_t factory, const basf_parser_cfg *cfg)
{
	basf_parser_t asf;

	BDBG_MSG_TRACE(("basf_parser_create>: %#lx", (unsigned long)cfg));

	BDBG_CASSERT(B_GET_BITS(0xDE,7,4)==0xD); 
	BDBG_CASSERT(B_GET_BITS(0xDE,3,0)==0xE); 
	BDBG_CASSERT(B_GET_BITS(0xDE,3,3)==0x1); 
	BDBG_CASSERT(B_GET_BITS(0xDE,1,1)==0x1); 
	BDBG_CASSERT(B_GET_BITS(0xDE,0,0)==0x0); 
	BDBG_CASSERT(B_GET_BITS(0xDE,6,0)==0x5E); 
	BDBG_CASSERT(!B_GET_BIT(0xDE,0)); 
	BDBG_CASSERT(B_GET_BIT(0xDE,1)); 
	BDBG_CASSERT(B_GET_BIT(0xDE,7)); 
	BDBG_CASSERT(!B_GET_BIT(0xDE,5)); 

	BDBG_CASSERT(B_SET_BIT( _ , 1, 0) == 1);
	BDBG_CASSERT(B_SET_BIT( _ , 2, 0) == 1);
	BDBG_CASSERT(B_SET_BIT( _ , 1, 1) == 2);
	BDBG_CASSERT(B_SET_BIT( _ , 1, 31) == (1u<<31));
	BDBG_CASSERT(B_SET_BITS( _ , 1, 0, 0) == 1);
	BDBG_CASSERT(B_SET_BITS( _ , 1, 1, 0) == 1);
	BDBG_CASSERT(B_SET_BITS( _ , 1, 1, 1) == 2);
	BDBG_CASSERT(B_SET_BITS( _ , 0x55, 7, 1) == 0xAA);


	BDBG_CASSERT(sizeof(basf_byte)==1);
	BDBG_CASSERT(sizeof(basf_wchar)==2);
	BDBG_CASSERT(sizeof(basf_word)==2);
	BDBG_CASSERT(sizeof(basf_dword)==4);
	BDBG_CASSERT(sizeof(basf_qword)==8);
	BDBG_CASSERT(sizeof(basf_guid)==16);
	BDBG_CASSERT(B_ASF_MAX_PACKET_CNT > 0xFFFF);

	BDBG_ASSERT(cfg);

	asf = BKNI_Malloc(sizeof(*asf));
	if (!asf) { 
		BDBG_ERR(("basf_parser_create: out of memory %u", sizeof(*asf)));
		goto err_alloc_asf;
	}
	BDBG_OBJECT_INIT(asf, basf_parser_t);

	asf->cfg = *cfg;

	asf->acc = batom_accum_create(factory);
	if(!asf->acc) {
		goto err_acc;
	}
	BLST_S_INIT(&asf->object_handlers);
	basf_parser_set_payload_sink(asf, NULL);
	basf_parser_reset(asf);
	BDBG_MSG_TRACE(("basf_parser_create>: %#lx %#lx", (unsigned long)cfg, (unsigned long)asf));
	return asf;

err_acc:
	BKNI_Free(asf);
err_alloc_asf:
	BDBG_MSG_TRACE(("basf_parser_create>: %#lx %#lx", (unsigned long)cfg, (unsigned long)NULL));
	return NULL;
}

void 
basf_parser_destroy(basf_parser_t asf)
{
	basf_parser_object_handler *handler;
	BDBG_MSG_TRACE(("basf_parser_destroy>: %#lx", (unsigned long)asf));
	BDBG_OBJECT_ASSERT(asf, basf_parser_t);
	for(handler=BLST_S_FIRST(&asf->object_handlers); handler; handler=BLST_S_NEXT(handler, link)) {
		BDBG_WRN(("basf_parser_destroy: %#lx lost object_handler %#lx[%s]", (unsigned long)asf, (unsigned long)handler, b_asf_parser_guid2str(asf, handler->guid)));
	}
	basf_parser_reset(asf);
	batom_accum_destroy(asf->acc);
	BDBG_OBJECT_DESTROY(asf, basf_parser_t);
    BDBG_MSG_TRACE(("basf_parser_destroy<: %#lx", (unsigned long)asf));
	BKNI_Free(asf);
	return;
}


void
basf_parser_reset(basf_parser_t asf) 
{
	BDBG_MSG_TRACE(("basf_parser_reset>: %#lx", (unsigned long)asf));
	BDBG_OBJECT_ASSERT(asf, basf_parser_t);

	BKNI_Memset(&asf->state, 0, sizeof(asf->state));
	batom_accum_clear(asf->acc);
	asf->state.acc_off = 0;
	asf->state.paused = false;
	asf->state.stream_error = false;
    asf->state.data_discontinuity = false;
    asf->state.invalid_packet_cnt = 0;
    BMEDIA_PARSING_ERRORS_INIT(&asf->state.errors);
	asf->state.data_npackets = 0;
	B_ASF_SET_FLOW_STATE(asf, b_asf_parser_flow_state_header);
	B_ASF_SET_PARSER_STATE(asf, basf_parser_state_top);
	BDBG_MSG_TRACE(("basf_parser_reset<: %#lx", (unsigned long)asf));
	return;
}

const char *
b_asf_parser_guid2str(basf_parser_t asf, const basf_guid *guid)
{
	BDBG_OBJECT_ASSERT(asf, basf_parser_t);
	return basf_guid2str(guid, asf->guid_str, sizeof(asf->guid_str)-1);
}


static const basf_guid
  basf_header_extension_guid = 
    {{0x5F, 0xBF, 0x03, 0xB5, 0xA9, 0x2E, 0x11, 0xCF, 0x8E, 0xE3, 0x00, 0xC0, 0x0C, 0x20, 0x53, 0x65}},
  basf_data_guid = 
	{{0x75, 0xB2, 0x26, 0x36, 0x66, 0x8E, 0x11, 0xCF, 0xA6, 0xD9, 0x00, 0xAA, 0x00, 0x62, 0xCE, 0x6C}},
  basf_index_guid = 
    {{0xD6, 0xE2, 0x29, 0xD3, 0x35, 0xDA, 0x11, 0xD1, 0x90, 0x34, 0x00, 0xA0, 0xC9, 0x03, 0x49, 0xBE}};

static bool 
b_asf_parser_set_parser(basf_parser_t asf, const basf_guid *object_guid, b_asf_parser_object object_parser)
{
		if(basf_compare_guid(&asf->state.object.guid, object_guid)==0) {
			BDBG_MSG_FLOW(("set parser %#lx for object '%s' ", (unsigned long)object_parser, b_asf_parser_guid2str(asf, object_guid)));
			asf->state.object.parser = object_parser;
			return true;
		}
		return false;
}



static void
b_asf_parser_header_object_next(basf_parser_t asf)
{
	BDBG_MSG_TRACE(("header_object_next: object %u:%u",  (unsigned)asf->state.object.u.header.cur_object, (unsigned)asf->state.object.u.header.nobjects));

	asf->state.object.u.header.cur_object++;
	if(	asf->state.object.u.header.cur_object <= asf->state.object.u.header.nobjects ) {
		B_ASF_SET_FLOW_STATE(asf, b_asf_parser_flow_state_header);
		return ;
	}
	BDBG_MSG(("end of header object"));
	B_ASF_SET_FLOW_STATE(asf, b_asf_parser_flow_state_header);
	B_ASF_SET_PARSER_STATE(asf, basf_parser_state_top);
	return ;
}

static void
b_asf_parse_object_next(basf_parser_t asf)
{
	switch(asf->state.parser_state) {
	case basf_parser_state_top:
		B_ASF_SET_FLOW_STATE(asf, b_asf_parser_flow_state_header);
		break;
	case basf_parser_state_header_object:
		b_asf_parser_header_object_next(asf);
		break;
	case basf_parser_state_data_object:
		BDBG_ASSERT(false);
		break;
	}
	return ;
}

void 
basf_parser_install_object_handler(basf_parser_t asf, basf_parser_object_handler *handler, const basf_guid *guid, basf_parser_object_handler_cb object_handler)
{
	basf_parser_object_handler *cur, *prev;

	BDBG_OBJECT_ASSERT(asf, basf_parser_t);
	BDBG_MSG_TRACE(("basf_parser_install_object_handler:> %#lx  %#lx[%s]", (unsigned long)asf, (unsigned long)handler, b_asf_parser_guid2str(asf, guid)));

	for(prev=NULL, cur = BLST_S_FIRST(&asf->object_handlers); cur; cur=BLST_S_NEXT(cur, link)) {
		if (basf_compare_guid(guid, cur->guid)<0) {
			break;
		}
		prev = cur;
	}
	handler->guid = guid;
	handler->handler = object_handler;
	if (prev) {
		BLST_S_INSERT_AFTER(&asf->object_handlers, prev, handler, link);
	} else {
		BLST_S_INSERT_HEAD(&asf->object_handlers, handler, link);
	}
#if 0
	for(prev=NULL, cur = BLST_S_FIRST(&asf->object_handlers); cur; cur=BLST_S_NEXT(cur, link)) {
		BDBG_WRN(("%s %s %d", prev?b_asf_parser_guid2str(asf, prev->guid):"-", b_asf_parser_guid2str(asf, cur->guid), prev?basf_compare_guid(prev->guid, cur->guid):0));
		prev = cur;
	}
#endif
	BDBG_MSG_TRACE(("basf_parser_install_object_handler:< %#lx  %#lx[%s]", (unsigned long)asf, (unsigned long)handler, b_asf_parser_guid2str(asf, guid)));
	return ;
}

void 
basf_parser_remove_object_handler(basf_parser_t asf, basf_parser_object_handler *handler)
{
	BDBG_OBJECT_ASSERT(asf, basf_parser_t);
	BDBG_ASSERT(handler);
	BDBG_MSG_TRACE(("basf_parser_remove_object_handler:> %#lx  %#lx[%s]", (unsigned long)asf, (unsigned long)handler, b_asf_parser_guid2str(asf, handler->guid)));
	BDBG_OBJECT_ASSERT(asf, basf_parser_t);
	BLST_S_REMOVE(&asf->object_handlers, handler, basf_parser_object_handler, link);
	BDBG_MSG_TRACE(("basf_parser_remove_object_handler:< %#lx  %#lx[%s]", (unsigned long)asf, (unsigned long)handler, b_asf_parser_guid2str(asf, handler->guid)));
	return;
}

static basf_parser_object_handler *
b_asf_parser_find_userobject_handler(basf_parser_t asf, const basf_guid *guid)
{
	basf_parser_object_handler *cur;
	int cmp;

	for(cur = BLST_S_FIRST(&asf->object_handlers); cur; cur=BLST_S_NEXT(cur, link)) {
		cmp = basf_compare_guid(guid, cur->guid);
		if (cmp<0) {
			break;
		}
		if (cmp==0) {
			return cur;
		} 
	}
	return NULL;
}

static bool
b_asf_parser_with_user_object(basf_parser_t asf, batom_cursor *cursor)
{
	basf_parser_object_handler *handler;
	basf_parser_handler_action action;
	bool want_continue = true;
	batom_t object;
	batom_cursor object_end;
	size_t object_size;

	handler = b_asf_parser_find_userobject_handler(asf, &asf->state.object.guid);
	if (!handler) {
		BDBG_WRN(("b_asf_parser_with_user_object: %#lx lost handler for %s", (unsigned long)asf, b_asf_parser_guid2str(asf, &asf->state.object.guid)));
		goto done;
	}

	BATOM_CLONE(&object_end, cursor);
	object_size=(size_t)asf->state.object.size-B_ASF_HEADER_LENGTH;
	if(object_size!=batom_cursor_skip(&object_end, object_size)) {
		BDBG_WRN(("b_asf_parser_with_user_object: %#lx incomplete object %s:%u", (unsigned long)asf, b_asf_parser_guid2str(asf, &asf->state.object.guid), object_size));
		goto done;
	}
	object = batom_accum_extract(asf->acc, cursor, &object_end, NULL, NULL);
	if(!object) {
		BDBG_WRN(("b_asf_parser_with_user_object: %#lx can't allocate object %s:%u", (unsigned long)asf, b_asf_parser_guid2str(asf, &asf->state.object.guid), object_size));
		goto done;
	}
	do {
		BDBG_MSG(("user_object: '%s':%u -> %#lx", b_asf_parser_guid2str(asf, &asf->state.object.guid), (unsigned)asf->state.object.size, (unsigned long)handler));
		action = handler->handler(handler, object);
		switch(action) {
		case basf_parser_handler_action_none:  
			break;
		case basf_parser_handler_action_return:
			want_continue = false; 
			break;
		case basf_parser_handler_action_pause:
			BDBG_MSG(("b_asf_parser_with_user_object: %#lx pausing  parser, object '%s'", (unsigned long)asf, b_asf_parser_guid2str(asf, &asf->state.object.guid)));
			asf->state.paused = true;
			want_continue = false; 
			break;
		}
		handler = BLST_S_NEXT(handler, link);
	} while(handler && basf_compare_guid(&asf->state.object.guid, handler->guid)==0);
	batom_release(object);

done:
	return want_continue;
}

static bool
b_asf_parser_user_object(basf_parser_t asf, batom_cursor  *c)
{
	bool want_continue = b_asf_parser_with_user_object(asf, c);

	b_asf_parse_object_next(asf);
	return want_continue;
}
	

static bool
b_asf_parser_header_object(basf_parser_t asf, batom_cursor *cursor)
{
	batom_checkpoint chk;
	BATOM_SAVE(cursor, &chk);

    asf->state.header.offset = asf->state.acc_off - B_ASF_HEADER_LENGTH;
    asf->state.header.size = asf->state.object.size;
	asf->state.object.u.header.nobjects = basf_read_dword(cursor);
	batom_cursor_skip(cursor, 
			B_ASF_FIELD_LEN(Reserved1, byte) +
			B_ASF_FIELD_LEN(Reserved2, byte));

	if (BIO_IS_EOF(cursor)) { 
		batom_cursor_rollback(cursor, &chk);
		return false; 
	}
	asf->state.object.u.header.cur_object = 0;
	BDBG_MSG_TRACE(("header_object: count %u", (unsigned)asf->state.object.u.header.nobjects));
	B_ASF_SET_PARSER_STATE(asf, basf_parser_state_header_object);

	b_asf_parser_header_object_next(asf);
	return true;
}

static bool
b_asf_parse_file_properties_object(basf_parser_t asf, batom_cursor *cursor)
{
	basf_file_properties file;

	if(!basf_read_file_properties(cursor, &file)) {
		BASF_ASSERT(false);
	}
	asf->state.file.preroll = file.preroll;

	asf->state.file.broadcast = file.broadcastFlag;
	asf->state.file.seekable = file.seekableFlag;

	BASF_ASSERT(file.minimumDataPacketSize==file.maximumDataPacketSize);
	asf->state.file.pkt_size = file.minimumDataPacketSize;
	asf->state.file.bitrate = file.maximumBitrate;
	asf->state.file.duration = file.playDuration;
	return true;
asf_assert_handler:
	return true;
}

static bool
b_asf_parser_header_extension_object(basf_parser_t asf, batom_cursor *cursor)
{
	basf_guid object_guid;
	basf_qword object_size;
	basf_dword data_size;
	basf_parser_object_handler *handler;
	batom_cursor user_cursor;
	basf_parser_handler_action action;
	bool want_continue = true;
	batom_t atom;

	batom_cursor_skip(cursor, 
			B_ASF_FIELD_LEN(Reserved Field 1, guid) + 
			B_ASF_FIELD_LEN(Reserved Field 2, word)
			);
	data_size = basf_read_dword(cursor);
	BASF_ASSERT(!BIO_IS_EOF(cursor));
	BDBG_MSG_TRACE(("b_asf_parser_header_extension_object>: %#lx data_size %u", (unsigned long)asf, data_size));
	while(data_size>0) {
		basf_read_guid(cursor, &object_guid);
		object_size = basf_read_qword(cursor);
		BDBG_MSG_TRACE(("b_asf_parser_header_extension_object: %#lx object '%s' size %u:%u at %lu", (unsigned long)asf, b_asf_parser_guid2str(asf, &object_guid), (unsigned)object_size, data_size, (unsigned long)asf->state.acc_off + batom_cursor_pos(cursor)-(B_ASF_FIELD_LEN(guid, guid)+B_ASF_FIELD_LEN(size, qword))));
		BASF_ASSERT(object_size >= B_ASF_HEADER_LENGTH);
		BASF_ASSERT(object_size <= data_size);
		data_size -= (unsigned)object_size;
		object_size -= B_ASF_HEADER_LENGTH;

		handler = b_asf_parser_find_userobject_handler(asf, &object_guid);
		while(handler && basf_compare_guid(&object_guid, handler->guid)==0) {
			BIO_CLONE(&user_cursor, cursor);
			if(batom_cursor_skip(&user_cursor, object_size)!=object_size) {
				BDBG_WRN(("b_asf_parser_find_userobject_handler:%lx partial object %s:%u", (unsigned long)asf, b_asf_parser_guid2str(asf, &object_guid), object_size));
				break;
			}
			atom = batom_accum_extract(asf->acc, cursor, &user_cursor, NULL, NULL);
			if(!atom) {
				BDBG_WRN(("b_asf_parser_header_extension_object:%#lx can't create user object", (unsigned long)asf));
				break;
			}

			BDBG_MSG(("extension user_object: '%s':%u -> %#lx", b_asf_parser_guid2str(asf, &object_guid), (unsigned)object_size, (unsigned long)handler));
			action = handler->handler(handler, atom);
			switch(action) {
			case basf_parser_handler_action_none:  
				break;
			case basf_parser_handler_action_return:
				want_continue = false; 
				break;
			case basf_parser_handler_action_pause:
				BDBG_MSG(("b_asf_parser_header_extension_object: %#lx pausing  parser, object '%s'", (unsigned long)asf, b_asf_parser_guid2str(asf, &asf->state.object.guid)));
				asf->state.paused = true;
				want_continue = false; 
				break;
			}
            batom_release(atom);
			handler = BLST_S_NEXT(handler, link);
		}
		batom_cursor_skip(cursor, (size_t)object_size);
	}
	BASF_ASSERT(!BIO_IS_EOF(cursor));
	BDBG_MSG_TRACE(("b_asf_parser_header_extension_object<: %#lx", (unsigned long)asf));
	return want_continue;
asf_assert_handler:
	return true;
}


static bool
b_asf_parse_data_object_next(basf_parser_t asf)
{
	BDBG_MSG_TRACE(("b_asf_parse_data_object_next>: %#lx packet %u total packets %u", (unsigned long)asf, asf->state.object.u.data.packet, asf->state.data_npackets));	
	asf->state.object.u.data.packet++; /* packet counts from 1 */
	/*When Broadcast flag is set, usually number of packets is set to zero. However there are streams where both are set and we dont send any EOS*/
	if((asf->state.file.broadcast && !asf->state.data_npackets)|| asf->state.object.u.data.packet<=asf->state.data_npackets) {
		B_ASF_SET_FLOW_STATE(asf, b_asf_parser_flow_state_data_pkt);
		return true;
	}
	BDBG_MSG(("end of data object"));
	asf->payload_sink.payload_end(asf->payload_sink.sink_cnxt);
	B_ASF_SET_FLOW_STATE(asf, b_asf_parser_flow_state_header);
	B_ASF_SET_PARSER_STATE(asf, basf_parser_state_top);
	return true;
}

static bool
b_asf_parse_data_object(basf_parser_t asf, batom_cursor *cursor)
{
	batom_checkpoint chk;
	basf_guid file_id;

	BATOM_SAVE(cursor, &chk);

    asf->state.data.object_offset = asf->state.acc_off - B_ASF_HEADER_LENGTH;
    asf->state.data.object_size = asf->state.object.size;
	asf->state.data.data_object_offset = asf->state.acc_off + batom_cursor_pos(cursor) - B_ASF_HEADER_LENGTH;
	basf_read_guid(cursor, &file_id);
	asf->state.data.total_data_packets = basf_read_qword(cursor);
	batom_cursor_skip(cursor, B_ASF_FIELD_LEN(Reserved , word));

	if (BIO_IS_EOF(cursor)) { 
		batom_cursor_rollback(cursor, &chk);
		return false; 
	}

	if (asf->state.file.pkt_size<B_ASF_MIN_PKT_SIZE) {
		BDBG_WRN(("b_asf_parse_data_object: %#lx invalid pkt_size %u", (unsigned long)asf, (unsigned)asf->state.file.pkt_size));
		asf->state.skip_len = asf->state.object.size - B_ASF_HEADER_LENGTH - batom_cursor_pos(cursor);
	}

	if (asf->state.data.total_data_packets==0 || asf->state.data.total_data_packets>B_ASF_MAX_PACKET_CNT) {
		BDBG_WRN(("b_asf_parse_data_object: %#lx unbounded  data_object %#lx", (unsigned long)asf, (unsigned long)asf->state.data.total_data_packets));
		asf->state.data_npackets = (size_t)B_ASF_MAX_PACKET_CNT;
	}else {
		asf->state.data_npackets = (size_t)asf->state.data.total_data_packets;
	}
	asf->state.data.data_offset = asf->state.acc_off + batom_cursor_pos(cursor);

	asf->state.object.u.data.packet = 0;
	BDBG_MSG(("data_object: file '%s' pkt_count %u data %lu", b_asf_parser_guid2str(asf, &file_id), (unsigned)asf->state.data_npackets, (unsigned long)asf->state.data.data_offset));
	B_ASF_SET_PARSER_STATE(asf, basf_parser_state_data_object);

	return b_asf_parse_data_object_next(asf);
}

static bool
b_asf_parse_simple_index_object(basf_parser_t asf, batom_cursor *cursor)
{
	batom_checkpoint chk;
	basf_simple_index index;

	BATOM_SAVE(cursor, &chk);
    index.object_offset = asf->state.acc_off - B_ASF_HEADER_LENGTH;
    index.object_size = asf->state.object.size;

	batom_cursor_skip(cursor, B_ASF_FIELD_LEN(File ID, guid));
	index.index_entry_time_interval = basf_read_qword(cursor);
	index.maximum_packet_count = basf_read_dword(cursor);
	index.index_entries_count = basf_read_dword(cursor);
	BDBG_OBJECT_ASSERT(asf, basf_parser_t);

	if (BIO_IS_EOF(cursor)) { 
		batom_cursor_rollback(cursor, &chk);
		return false; 
	}
	index.offset = asf->state.acc_off + batom_cursor_pos(cursor);
	BDBG_MSG(("b_asf_parse_simple_index_object:%#lx %u %u %u %lu", (unsigned long)asf, index.index_entry_time_interval, index.maximum_packet_count, index.index_entries_count, (unsigned long)index.offset));
	asf->cfg.simple_index(asf->cfg.application_cnxt, &index);
	asf->state.skip_len = asf->state.object.size - B_ASF_HEADER_LENGTH - batom_cursor_pos(cursor);
	BDBG_MSG_FLOW(("b_asf_parse_simple_index_object:%#lx discarding payload %u", (unsigned long)asf, (unsigned)asf->state.skip_len));
	B_ASF_SET_FLOW_STATE(asf, b_asf_parser_flow_state_skip);
	return true;
}

static bool
b_asf_parse_index_object(basf_parser_t asf, batom_cursor *cursor)
{
	batom_checkpoint chk;
	basf_index index;

	BATOM_SAVE(cursor, &chk);
    index.object_offset = asf->state.acc_off - B_ASF_HEADER_LENGTH;
    index.object_size = asf->state.object.size;

	index.index_entry_time_interval = basf_read_dword(cursor);
	index.index_specifiers_count = basf_read_word(cursor);
	index.index_blocks_count = basf_read_dword(cursor);

	if (BIO_IS_EOF(cursor)) { 
		batom_cursor_rollback(cursor, &chk);
		return false; 
	}
	index.offset = asf->state.acc_off + batom_cursor_pos(cursor);
	BDBG_MSG(("b_asf_parse_index_object:%#lx %u %u %u %lu", (unsigned long)asf, index.index_entry_time_interval, index.index_specifiers_count, index.index_blocks_count, (unsigned long)index.offset));
	asf->cfg.index(asf->cfg.application_cnxt, &index);
	asf->state.skip_len = asf->state.object.size - B_ASF_HEADER_LENGTH - batom_cursor_pos(cursor);;
	BDBG_MSG_FLOW(("b_asf_parse_index_object:%#lx discarding payload %u", (unsigned long)asf, (unsigned)asf->state.skip_len));
	B_ASF_SET_FLOW_STATE(asf, b_asf_parser_flow_state_skip);
	return true;
}


static bool
b_asf_parse_data_pkt(basf_parser_t asf, batom_cursor *cursor)
{
	basf_byte length_type_flags;
	basf_byte property_flags;
	basf_byte error_correction_flags;

#define B_ASF_Multiple_Payloads_Present B_GET_BIT(length_type_flags,0)
#define B_ASF_Sequence_Type B_GET_BITS(length_type_flags,2,1)
#define B_ASF_Padding_Length_Type B_GET_BITS(length_type_flags,4,3)
#define B_ASF_Packet_Length_Type B_GET_BITS(length_type_flags,6,5)
#define B_ASF_Error_Correction_Present B_GET_BIT(length_type_flags,7)


#define B_ASF_Replicated_Data_Length_Type B_GET_BITS(property_flags,1,0)
#define B_ASF_Offset_Into_Media_Object_Length_Type B_GET_BITS(property_flags,3,2)
#define B_ASF_Media_Object_Number_Length_Type B_GET_BITS(property_flags,5,4)
#define B_ASF_Stream_Number_Length_Type B_GET_BITS(property_flags,7,6)
	basf_dword packet_length;
	basf_dword sequence;
	basf_dword padding_length;
	basf_dword send_time;
	basf_word duration;
	unsigned number_of_payloads;
	unsigned payload_length_type;
	unsigned payload;
	batom_cursor replicated_data;
	batom_checkpoint chk;

	BATOM_SAVE(cursor, &chk);

	BDBG_MSG_TRACE(("b_asf_parse_data_pkt>: %#lx", (unsigned long)asf));
	length_type_flags = basf_read_byte(cursor);
	if (B_ASF_Error_Correction_Present) {
		#define B_ASF_Error_Correction_Data_Length B_GET_BITS(error_correction_flags,3,0)
		#define B_ASF_Opaque_Data_Present B_GET_BIT(error_correction_flags,4)
		#define B_ASF_Error_Correction_Length_Type B_GET_BITS(error_correction_flags,6,5)
		/* #define B_ASF_Error_Correction_Present B_GET_BIT(error_correction_flags,7) */

		error_correction_flags = length_type_flags;

		BDBG_MSG_FLOW(("error_correction: %u %u", B_ASF_Error_Correction_Length_Type, B_ASF_Error_Correction_Data_Length));
		BASF_ASSERT(B_ASF_Error_Correction_Data_Length!=0);
		BASF_ASSERT(B_ASF_Error_Correction_Length_Type==0);
		batom_cursor_skip(cursor, B_ASF_Error_Correction_Data_Length);

		/* reread length_type_flags */
		length_type_flags = basf_read_byte(cursor);
	} 

	property_flags = basf_read_byte(cursor);

	packet_length = basf_read_vword(cursor, B_ASF_Packet_Length_Type);
	sequence = basf_read_vword(cursor, B_ASF_Sequence_Type);
	padding_length = basf_read_vword(cursor, B_ASF_Padding_Length_Type);
	send_time = basf_read_dword(cursor);
	duration = basf_read_word(cursor);

	BDBG_MSG(("packet: %u(%u) at %#lu %s len %u sequence %u padding %u send_time %u duration %u", asf->state.object.u.data.packet, asf->state.data_npackets, (unsigned long)asf->state.acc_off, B_ASF_Multiple_Payloads_Present?"MULTIPLE":"SINGLE", packet_length, sequence, padding_length, send_time, duration));
	if (padding_length==0 && packet_length>0) {
		BASF_ASSERT(packet_length <= asf->state.file.pkt_size);
		padding_length = asf->state.file.pkt_size - packet_length;
		if (padding_length) {
			BDBG_MSG(("inferred padding %u (%u:%u)", padding_length, asf->state.file.pkt_size, packet_length));
		}
	}

	if (B_ASF_Multiple_Payloads_Present) {
		basf_byte payload_flags;

		payload_flags = basf_read_byte(cursor);
		number_of_payloads = B_GET_BITS(payload_flags, 5, 0);
		payload_length_type = B_GET_BITS(payload_flags, 7, 6);
	} else {
		number_of_payloads = 1;
		payload_length_type = 0;
	}
	for(payload=0;payload<number_of_payloads;payload++) {
		basf_byte stream_number;
		#define B_ASF_Stream_Number B_GET_BITS(stream_number, 6, 0)
		#define B_ASF_Key_Frame_Bit B_GET_BIT(stream_number, 7)
		basf_dword media_object_number;
		basf_dword replicated_data_length;


		/* coverity[negative_return_fn] */
		stream_number = basf_read_byte(cursor);	
		asf->state.info.stream_number = B_ASF_Stream_Number;
		asf->state.info.key_frame = B_ASF_Key_Frame_Bit;
		media_object_number = basf_read_vword(cursor, B_ASF_Media_Object_Number_Length_Type);
		asf->state.info.offset_into_media_object = basf_read_vword(cursor, B_ASF_Offset_Into_Media_Object_Length_Type);
		replicated_data_length = basf_read_vword(cursor, B_ASF_Replicated_Data_Length_Type);
		if (replicated_data_length!=1) {
			BASF_ASSERT(replicated_data_length>=8 && replicated_data_length<asf->state.file.pkt_size);
			asf->state.info.media_object_size = basf_read_dword(cursor);
			asf->state.info.pres_time = basf_read_dword(cursor);
			asf->state.info.replicated_data_length = replicated_data_length-
						(B_ASF_FIELD_LEN(Media Object Size, dword) + B_ASF_FIELD_LEN(Pres Time, dword));
			if (asf->state.info.replicated_data_length) {
				/* save a copy of the replicated data */
				batom_cursor_clone(&replicated_data, cursor);
				batom_cursor_skip(cursor, asf->state.info.replicated_data_length);
			} else {
				batom_cursor_empty(&replicated_data);
			}
			if (B_ASF_Multiple_Payloads_Present) {
				asf->state.info.payload_length = basf_read_vword(cursor, payload_length_type);
			} else {
				asf->state.info.payload_length = asf->state.file.pkt_size - (batom_cursor_pos(cursor)+padding_length);
			}
			BDBG_MSG(("payload: %u:%u(%u) at %lu stream:%u %s media_object_size:%u  pres_time:%u offset_into_media_object:%u replicated_data_length:%u", payload, number_of_payloads, asf->state.info.payload_length, (unsigned long)asf->state.acc_off + batom_cursor_pos(cursor), asf->state.info.stream_number, asf->state.info.key_frame?"KEYFRAME":"", asf->state.info.media_object_size, asf->state.info.pres_time, asf->state.info.offset_into_media_object, replicated_data_length));
			BASF_ASSERT(asf->state.info.payload_length<asf->state.file.pkt_size);
			asf->payload_sink.payload(asf->payload_sink.sink_cnxt, asf->acc, cursor, &replicated_data, &asf->state.info);
			batom_cursor_skip(cursor, asf->state.info.payload_length);
		} else {
			/* compressed payload */
			basf_dword presentation_time = asf->state.info.offset_into_media_object;
			basf_byte presentation_time_delta;
			basf_dword payload_length;

			/* coverity[var_assign] */
			presentation_time_delta = basf_read_byte(cursor);
			if (B_ASF_Multiple_Payloads_Present) {
				payload_length = basf_read_vword(cursor, payload_length_type);
			} else {
				payload_length = asf->state.file.pkt_size - (batom_cursor_pos(cursor)+padding_length);
			}
			batom_cursor_empty(&replicated_data);
			asf->state.info.replicated_data_length = 0;
			asf->state.info.pres_time = presentation_time; 
			asf->state.info.offset_into_media_object = 0;
			while(payload_length>0) {

				/* coverity[var_assign] */
				asf->state.info.payload_length = basf_read_byte(cursor);
				/* coverity[alias_assign] */
				asf->state.info.media_object_size = asf->state.info.payload_length;
				BDBG_MSG(("compressed payload: %u:%u(%u:%u) at %lu stream:%u %s media_object_size:%u  pres_time:%u offset_into_media_object:%u replicated_data_length:%u", payload, number_of_payloads, asf->state.info.payload_length, payload_length, (unsigned long)asf->state.acc_off + batom_cursor_pos(cursor), asf->state.info.stream_number, asf->state.info.key_frame?"KEYFRAME":"", asf->state.info.media_object_size, asf->state.info.pres_time, asf->state.info.offset_into_media_object, replicated_data_length));
				BASF_ASSERT(payload_length>=asf->state.info.payload_length);
				asf->payload_sink.payload(asf->payload_sink.sink_cnxt, asf->acc, cursor, &replicated_data, &asf->state.info);
				batom_cursor_skip(cursor, asf->state.info.payload_length);
				payload_length -= asf->state.info.payload_length+1;
				asf->state.info.pres_time += presentation_time_delta;
			}
		}
	}
	BASF_ASSERT(padding_length<asf->state.file.pkt_size);
	batom_cursor_skip(cursor, padding_length);
    payload = batom_cursor_pos(cursor);
    BASF_ASSERT(payload<=asf->state.file.pkt_size);
	if(payload<asf->state.file.pkt_size) {
        batom_cursor_skip(cursor, asf->state.file.pkt_size - payload);
        if(asf->state.invalid_packet_cnt<4) {
            BDBG_WRN(("b_asf_parse_data_pkt: %#lx invalid packet size (%u:%u) [%u/4]", (unsigned long)asf, (unsigned)payload, asf->state.file.pkt_size, asf->state.invalid_packet_cnt));
            asf->state.invalid_packet_cnt++;
        }
    }
	BASF_ASSERT(!BIO_IS_EOF(cursor));
	BDBG_MSG_TRACE(("b_asf_parse_data_pkt<: %#lx", (unsigned long)asf));
	return b_asf_parse_data_object_next(asf);

asf_assert_handler:
	/* skip entire packet */
	batom_cursor_rollback(cursor, &chk);
	batom_cursor_skip(cursor, asf->state.file.pkt_size);
	/* flush demux */
	asf->payload_sink.payload_error(asf->payload_sink.sink_cnxt);
	return b_asf_parse_data_object_next(asf);
}


static bool
b_asf_parser_header(basf_parser_t asf, batom_cursor *cursor)
{
	batom_checkpoint chk;
    basf_qword object_offset;

	BATOM_SAVE(cursor, &chk);

	basf_read_guid(cursor, &asf->state.object.guid);
	asf->state.object.size = basf_read_qword(cursor);
	if (BIO_IS_EOF(cursor)) { 
		batom_cursor_rollback(cursor, &chk);
		return false; 
	}
    object_offset = asf->state.acc_off + batom_cursor_pos(cursor)-(B_ASF_FIELD_LEN(guid, guid)+B_ASF_FIELD_LEN(size, qword));;
	BDBG_MSG(("b_asf_parser_header: %#lx object '%s' size %u at %lu", (unsigned long)asf, b_asf_parser_guid2str(asf, &asf->state.object.guid), (unsigned)asf->state.object.size,  (unsigned long)object_offset));
    if( !(asf->state.object.size==0 && asf->state.file.broadcast && basf_compare_guid(&asf->state.object.guid, &basf_data_guid)==0)) {
	    BASF_ASSERT(asf->state.object.size >= B_ASF_HEADER_LENGTH);
    }
    asf->cfg.object(asf->cfg.application_cnxt, &asf->state.object.guid, object_offset, asf->state.object.size);

	switch(asf->state.parser_state) {
	case basf_parser_state_top:
		if(
			b_asf_parser_set_parser(asf, &basf_header_guid, b_asf_parser_header_object) ||
			b_asf_parser_set_parser(asf, &basf_data_guid, b_asf_parse_data_object)  ||
			b_asf_parser_set_parser(asf, &basf_index_guid, b_asf_parse_index_object) ||
			b_asf_parser_set_parser(asf, &basf_simple_index_guid, b_asf_parse_simple_index_object) 
		  ) {
			B_ASF_SET_FLOW_STATE(asf, b_asf_parser_flow_state_object);
			return true;
		}
		break;
	case basf_parser_state_header_object:
		BDBG_MSG_FLOW(("header_object: object %u:%u",  (unsigned)asf->state.object.u.header.cur_object, (unsigned)asf->state.object.u.header.nobjects));
		if(b_asf_parser_set_parser(asf, &basf_file_properties_guid, b_asf_parse_file_properties_object) || 
		   b_asf_parser_set_parser(asf, &basf_header_extension_guid, b_asf_parser_header_extension_object)
		   ) {

			if (asf->state.object.size > B_ASF_MAX_OBJECT_LEN) {
				BDBG_WRN(("%#lx discarding mandatory object %s due to its size %u>%u", (unsigned long)asf, b_asf_parser_guid2str(asf, &asf->state.object.guid), (unsigned)asf->state.object.size, B_ASF_MAX_OBJECT_LEN));
				break;
			}
			asf->state.capture_len = (size_t)asf->state.object.size-B_ASF_HEADER_LENGTH;
			B_ASF_SET_FLOW_STATE(asf, b_asf_parser_flow_state_capture_object);
			return true;
		}
		break;
	case basf_parser_state_data_object:
		BDBG_ASSERT(false);
		break;
	}
	if (b_asf_parser_find_userobject_handler(asf, &asf->state.object.guid)) {
		if (asf->state.object.size > B_ASF_MAX_OBJECT_LEN) {
			BDBG_WRN(("%#lx discarding user object %s due to its size %u>%u", (unsigned long)asf, b_asf_parser_guid2str(asf, &asf->state.object.guid), (unsigned)asf->state.object.size, B_ASF_MAX_OBJECT_LEN));
		} else {
			asf->state.capture_len = (size_t)asf->state.object.size-B_ASF_HEADER_LENGTH;
			asf->state.object.parser = b_asf_parser_user_object;
			B_ASF_SET_FLOW_STATE(asf, b_asf_parser_flow_state_capture_object);
			return true;
		}
	}
	asf->state.skip_len = asf->state.object.size - B_ASF_HEADER_LENGTH;
	BDBG_MSG_FLOW(("discarding object '%s':%u:%u", b_asf_parser_guid2str(asf, &asf->state.object.guid), (unsigned)asf->state.object.size, (unsigned)asf->state.skip_len));
	B_ASF_SET_FLOW_STATE(asf, b_asf_parser_flow_state_skip);

	return true;
asf_assert_handler:
	/* skip over entire object */
	batom_cursor_rollback(cursor, &chk);
	batom_cursor_skip(cursor, B_ASF_HEADER_LENGTH);
	B_ASF_SET_FLOW_STATE(asf, b_asf_parser_flow_state_skip);
	asf->state.stream_error = true;
	asf->state.skip_len = 64*1024; /* just arbitrary number larger 0 */
	return true;
}


static void
b_asf_parser_trim(basf_parser_t asf, batom_cursor *cursor)
{
	asf->state.acc_off += batom_cursor_pos(cursor);
	batom_accum_trim(asf->acc, cursor);
	return;
}

void 
basf_parser_seek(basf_parser_t asf, basf_qword offset)
{
	basf_qword current;
	BDBG_OBJECT_ASSERT(asf, basf_parser_t);
	BDBG_MSG_TRACE(("basf_parser_seek>: %#lx %lu", (unsigned long)asf, (unsigned long)offset));
	current = asf->state.acc_off + batom_accum_len(asf->acc);
	BDBG_MSG_TRACE(("basf_parser_seek: %#lx %lu[%lu]", (unsigned long)asf, (unsigned long)offset, (unsigned long)current));
	if (current==offset) {
		goto done; /* do nothing */
	} 
	asf->state.acc_off += batom_accum_len(asf->acc);
	batom_accum_clear(asf->acc);
	if(asf->state.flow_state == b_asf_parser_flow_state_skip && offset > current && offset<=current+asf->state.skip_len) {
		BDBG_MSG_TRACE(("basf_parser_seek: %#lx seek while in skip mode", (unsigned long)asf, (unsigned long)(offset-current) ));
 		asf->state.skip_len -= offset-current;
		goto done;
	}
	asf->payload_sink.payload_flush(asf->payload_sink.sink_cnxt);
    asf->state.data_discontinuity = true;

	if(asf->state.data.data_offset==0 || offset < asf->state.data.data_offset) {
		/* not in data, or data wasn't seen, offset shall be zero */
		goto seek_to_start;
	} else {
		if (!asf->state.file.broadcast && offset >= asf->state.data.data_offset + asf->state.file.pkt_size*(basf_qword)asf->state.data_npackets) {
			BDBG_MSG(("basf_parser_seek: %#lx seek past end of data (%lu:%lu)", (unsigned long)asf, (unsigned long)offset, (unsigned long)(asf->state.data.data_offset + asf->state.file.pkt_size*(basf_qword)asf->state.data_npackets)));
			if(asf->state.parser_state == basf_parser_state_data_object) {
				asf->payload_sink.payload_end(asf->payload_sink.sink_cnxt);
			}
			asf->state.acc_off = offset;
			B_ASF_SET_FLOW_STATE(asf, b_asf_parser_flow_state_header);
			B_ASF_SET_PARSER_STATE(asf, basf_parser_state_top);
			goto done;
		} else if (offset >= asf->state.data.data_offset) {
			basf_qword skip;
			unsigned pkt_no;
			asf->state.acc_off = offset;
			pkt_no = (unsigned) ((offset - asf->state.data.data_offset)/asf->state.file.pkt_size);
			asf->state.object.u.data.packet = 1 + pkt_no;
			skip = offset - (asf->state.data.data_offset + (pkt_no*(basf_qword)asf->state.file.pkt_size));
			BDBG_MSG(("basf_parser_seek: %#lx seek to data %u(%u)", (unsigned long)asf, asf->state.object.u.data.packet, (unsigned)skip));
			B_ASF_SET_PARSER_STATE(asf, basf_parser_state_data_object);
			if (skip) {
 				asf->state.skip_len -= skip;
				B_ASF_SET_FLOW_STATE(asf, b_asf_parser_flow_state_skip);
			} else {
				B_ASF_SET_FLOW_STATE(asf, b_asf_parser_flow_state_data_pkt);
			} 
			goto done;
		}
	}
seek_to_start:
	BDBG_MSG(("basf_parser_seek: %#lx seek to start", (unsigned long)asf));
	if (offset!=0) {
		BDBG_WRN(("basf_parser_seek: %#lx invalid offset %#lu shall be 0", (unsigned long)asf, (unsigned long)offset));
	}
	if(asf->state.parser_state == basf_parser_state_data_object) {
		asf->payload_sink.payload_end(asf->payload_sink.sink_cnxt);
	}
	asf->state.acc_off = 0;
	current = 0;
	B_ASF_SET_FLOW_STATE(asf, b_asf_parser_flow_state_header);
	B_ASF_SET_PARSER_STATE(asf, basf_parser_state_top);
done:
	BDBG_MSG_TRACE(("basf_parser_seek<: %#lx %lu[%lu]", (unsigned long)asf, (unsigned long)offset, (unsigned long)current));
	return;
}


size_t
basf_parser_feed(basf_parser_t asf, batom_pipe_t pipe)
{
	bool want_continue;
	size_t size;
	batom_cursor cursor;
	batom_cursor state_cur;
	batom_t atom;
	size_t len=0;

	BDBG_OBJECT_ASSERT(asf, basf_parser_t);
	BDBG_ASSERT(pipe);
	BDBG_MSG_TRACE(("basf_parser_feed>:%#lx %#lu:%u %u", (unsigned long)asf, (unsigned long)asf->state.acc_off, batom_accum_len(asf->acc), len));
	if (asf->state.paused) {
		goto done;
	}

	atom=batom_pipe_pop(pipe);
	do {
		BDBG_MSG(("basf_parser_feed:%#lx atom:%#lx", (unsigned long)asf, (unsigned long)atom));
		if (atom) {
			batom_accum_add_atom(asf->acc, atom);
			len += batom_len(atom);
			batom_release(atom);
		}
		do {
			size_t acc_len = batom_accum_len(asf->acc);
			want_continue = false;
            asf->state.data_discontinuity = false;
			BDBG_MSG_FLOW(("flow_state %d %s", asf->state.flow_state, asf->state.paused?"paused":""));
			batom_cursor_from_accum(&state_cur, asf->acc);

			switch(asf->state.flow_state) {
			case b_asf_parser_flow_state_data_pkt:
				BDBG_MSG_FLOW(("data_pkt: %#lx %u %u", (unsigned long)asf, asf->state.file.pkt_size, acc_len));
				if (acc_len>=asf->state.file.pkt_size) {
					if (asf->has_payload_sink) {
						want_continue = b_asf_parse_data_pkt(asf, &state_cur);
					} else {
						batom_cursor_skip(&state_cur, asf->state.file.pkt_size);
						want_continue = b_asf_parse_data_object_next(asf);
					}
					if (want_continue) {
						BDBG_ASSERT(batom_cursor_pos(&state_cur)==asf->state.file.pkt_size);
						b_asf_parser_trim(asf, &state_cur);
					}
				}
				break;
			case b_asf_parser_flow_state_object:
				BDBG_ASSERT(asf->state.object.parser);
				/* coverity[var_deref_op: FALSE] */
				want_continue = asf->state.object.parser(asf, &state_cur);
				if (want_continue) {
					/* recycle memory if parsing was completed */
					b_asf_parser_trim(asf, &state_cur);
				}
				break;
			case b_asf_parser_flow_state_capture_object:
				BDBG_MSG_FLOW(("capture_object: %#lx %u %u", (unsigned long)asf, asf->state.capture_len, acc_len));
				BDBG_ASSERT(asf->state.capture_len+B_ASF_HEADER_LENGTH == asf->state.object.size);
				if (acc_len>=asf->state.capture_len) {
					BDBG_ASSERT(asf->state.object.parser);
					batom_cursor_from_accum(&cursor, asf->acc);
					/* coverity[var_deref_op: FALSE] */
					want_continue = asf->state.object.parser(asf, &state_cur);
					if(asf->state.object.parser!=b_asf_parser_user_object && b_asf_parser_find_userobject_handler(asf, &asf->state.object.guid)) {
						batom_cursor_from_accum(&cursor, asf->acc);
						if (!b_asf_parser_with_user_object(asf, &cursor)) {
							want_continue = false;
						}
						batom_cursor_from_accum(&cursor, asf->acc);
					}
					/* coverity[assign_zero: FALSE] */
					asf->state.object.parser=NULL;
					batom_cursor_skip(&cursor, asf->state.capture_len);
					BDBG_ASSERT(!BIO_IS_EOF(&cursor));
					b_asf_parser_trim(asf, &cursor);
					batom_cursor_from_accum(&state_cur, asf->acc);
					b_asf_parse_object_next(asf);
				} 
				break;
			case b_asf_parser_flow_state_header:
				want_continue = b_asf_parser_header(asf, &state_cur);
				if (want_continue) {
					b_asf_parser_trim(asf, &state_cur);
				}
				break;
			case b_asf_parser_flow_state_skip: 
				BDBG_MSG_FLOW(("skip: %#lx %u %u", (unsigned long)asf, asf->state.skip_len, acc_len));
				size = acc_len;
				if (size>asf->state.skip_len) {
					size = (size_t) asf->state.skip_len;
				}
				if(size>0) {
					batom_cursor_skip(&state_cur, size);
					BDBG_ASSERT(!BIO_IS_EOF(&state_cur));
					b_asf_parser_trim(asf, &state_cur);
				}
				if(!asf->state.stream_error) {
					asf->state.skip_len -= size;
					if (asf->state.skip_len==0) {
						want_continue = true;
						b_asf_parse_object_next(asf);
					}
				} else {
					want_continue  = size != 0;
				}
				break;
			}
		} while(want_continue);
	} while(!asf->state.paused && NULL!=(atom=batom_pipe_pop(pipe)));
done:
	BDBG_MSG_TRACE(("basf_parser_feed<:%#lx %#lu:%u %u", (unsigned long)asf, (unsigned long)asf->state.acc_off, batom_accum_len(asf->acc), len));
	return len;
}

void
basf_parser_get_info(basf_parser_t asf, basf_parser_info *info)
{
	BDBG_MSG_TRACE(("basf_parser_get_info>:%#lx", (unsigned long)asf));
	BDBG_OBJECT_ASSERT(asf, basf_parser_t);
	BDBG_ASSERT(info);
	BDBG_MSG(("basf_parser_get_info: %#lx accum:%u %s", (unsigned long)asf, batom_accum_len(asf->acc), asf->state.paused?"PAUSED":""));
	info->offset = asf->state.acc_off + batom_accum_len(asf->acc);
	info->paused = asf->state.paused;
	info->stream_error = asf->state.stream_error;
    info->data_discontinuity = asf->state.data_discontinuity;
	info->state = asf->state.parser_state;
	info->data = asf->state.data;
	info->file = asf->state.file;
    info->header = asf->state.header;
    info->errors = asf->state.errors;
	BDBG_MSG_TRACE(("basf_parser_get_info<:%#lx %lu", (unsigned long)asf, (unsigned long)info->offset));
	return;
}

#if 0
void  
basf_attach_demux(basf_parser_t asf, basf_demux_t demux)
{
	BDBG_MSG_TRACE(("basf_attach_demux>:%#lx %#lx", (unsigned long)asf, (unsigned long)demux));
	BDBG_OBJECT_ASSERT(asf, basf_parser_t);
	BDBG_ASSERT(asf);
	BDBG_ASSERT(demux);
	asf->demux = demux;
	BDBG_MSG_TRACE(("basf_attach_demux<:%#lx %#lx", (unsigned long)asf, (unsigned long)demux));
	return;
}
#endif

void 
basf_resume(basf_parser_t asf)
{
	BDBG_MSG_TRACE(("basf_resume>:%#lx", (unsigned long)asf));
	asf->state.paused = false;
	BDBG_MSG_TRACE(("basf_resume<:%#lx", (unsigned long)asf));
	return;
}

