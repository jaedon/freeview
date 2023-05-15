/***************************************************************************
 *     Copyright (c) 2006-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: brtp_parser_h264.c $
 * $brcm_Revision: 8 $
 * $brcm_Date: 11/18/11 2:25p $
 *
 * Module Description:
 *
 * RTP parser library
 *   RFC 3984 module
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/rtp/brtp_parser_h264.c $
 * 
 * 8   11/18/11 2:25p vsilyaev
 * SW7425-1796: Merge to mainline
 * 
 * SW7425-1796/1   11/18/11 10:19a chengs
 * SW7425-1796: Fix typo of h.264 payload rtp processing.
 * 
 * 7   9/25/06 3:20p vsilyaev
 * PR 23826: Added throttled sync
 * 
 * 6   9/22/06 4:50p vsilyaev
 * PR 23826: Added simple RTSP player
 * 
 * 5   9/18/06 4:39p vsilyaev
 * PR 23826: More work for RTP timing
 * 
 * 4   9/12/06 6:34p vsilyaev
 * PR 23826: Fixed sprop parsing
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
#include "bstd.h"
#include "bkni.h"
#include "brtp_parser_h264.h"
#include "brtp_packet.h"
#include "bbase64.h"

BDBG_MODULE(brtp_parser_h264);

BDBG_OBJECT_ID(brtp_parser_h264_t);

struct brtp_parser_h264 {
	struct brtp_parser parent; /* must be the first element */
	BDBG_OBJECT(brtp_parser_h264_t)
	uint8_t *nal_header;
	brtp_parser_h264_cfg cfg;
	enum {brtp_state_single,  brtp_state_multi} state;
	unsigned multi_len;
	const uint8_t *meta_base;
	size_t meta_len;
	enum {brtp_nal_state_none, brtp_nal_state_idr} nal_state;
};
typedef struct brtp_parser_h264 *brtp_parser_h264_t;

static const brtp_parser_type b_rtp_parser_h264 = {
	"H.264",
	brtp_stream_video
};

const brtp_parser_type *brtp_parser_h264 = &b_rtp_parser_h264;


static bool
b_rtp_parser_h264_nal(brtp_parser_h264_t parser)
{
	brtp_io_vec *vec = brtp_parser_next(&parser->parent);
	if (!vec) {return true;}
	vec->packet = NULL;
	vec->data = parser->nal_header;
	vec->len = 3;
	return false;
}

static bool
b_rtp_parser_h264_data(brtp_parser_h264_t parser, brtp_packet_t packet, const void *data, size_t len)
{
	return brtp_parser_add_packet(&parser->parent, packet, data, len);
}

static void 
b_rtp_parser_h264_flush_multi(brtp_parser_h264_t parser)
{
	/* flush multi-packet payload if have received single payload packet  */
	if (parser->state == brtp_state_multi) {
		BDBG_WRN(("b_rtp_parser_h264_flush_multi: %#lx dropping accumulated data", (unsigned long)parser));
		brtp_parser_abort(&parser->parent);
		brtp_parser_begin(&parser->parent);
		parser->state = brtp_state_single;
		parser->multi_len = 0;
	}
}

static void
b_rtp_parser_h264_add_meta(brtp_parser_h264_t parser)
{
	brtp_io_vec *vec;

	if (parser->meta_len) {
		BDBG_MSG(("b_rtp_parser_h264_add_meta:%#lx adding %u bytes of meda data", (unsigned long)parser, parser->meta_len));
		vec = brtp_parser_next(&parser->parent);
		if (vec) {
			vec->len = parser->meta_len;
			vec->data = parser->meta_base;
			vec->packet = NULL;
		}
	}
	return;
}

#define B_H264_NAL_TYPE_IDR	5

static void
b_rtp_parser_h264_check_nal(brtp_parser_h264_t parser, unsigned nal)
{
	unsigned nal_type = B_GET_BITS(nal, 4, 0);

	if (nal_type==B_H264_NAL_TYPE_IDR) {
		if (parser->nal_state!=brtp_nal_state_idr) {
			/* add meta data on non IDR -> IDR transition */
			b_rtp_parser_h264_add_meta(parser);
		}
		parser->nal_state=brtp_nal_state_idr;
	} else {
		parser->nal_state=brtp_nal_state_none;
	}
	return;
}

static bool
b_rtp_parser_h264_stap(brtp_parser_h264_t parser, brtp_packet_t pkt, const void *data, size_t len, unsigned off)
{
	unsigned nal_len;
	bool overflow;
	const void *pkt_data;

	for(off=1;off<len;) {
		nal_len = B_RTP_LOAD16(data, off);
		/* BDBG_ERR(("%d %d %d", off, len, nal_len));  */
		off += 2;
		pkt_data = (uint8_t *)data+off;
		off += nal_len;
		if (off > len )  {
			BDBG_WRN(("partial NAL %u %u %u", len, (off-nal_len), nal_len));
			break;
		}
		b_rtp_parser_h264_check_nal(parser, *(uint8_t *)pkt_data);
		b_rtp_parser_h264_nal(parser);
		overflow = b_rtp_parser_h264_data(parser, pkt, pkt_data, nal_len);
		if (overflow) { return true; }
	}
	return false;
}


static bool
b_rtp_parser_h264_fu(brtp_parser_h264_t parser, brtp_packet_t packet, const void *data, size_t len, unsigned off)
{
	uint8_t fu_hdr = B_RTP_LOAD8(data, 1);
	bool abort;

	BDBG_ASSERT(off>1);

	if (B_RTP_GET_BIT(fu_hdr,0)) { /* start bit */
		uint8_t nal;

		BDBG_ASSERT(B_RTP_GET_BIT(fu_hdr,1)==0); /* end bit shall not be set */
		b_rtp_parser_h264_flush_multi(parser);
		parser->state = brtp_state_multi;

		nal = (fu_hdr & 0x3F) | (B_RTP_LOAD8(data, 0)&0xE0); /* construct NAL in place of H.264/RTP header */
		BDBG_MSG(("b_rtp_parser_h264_fu: %#lx fu_hdr %02x rtp_hdr %02x nal %02x", (unsigned long)parser, fu_hdr, B_RTP_LOAD8(data, 0), nal));
		((uint8_t *)data)[off-1] = nal;
		b_rtp_parser_h264_check_nal(parser, nal);
		b_rtp_parser_h264_nal(parser);
		parser->multi_len = len - (off-1);
		abort = b_rtp_parser_h264_data(parser, packet, (uint8_t *)data+(off-1), len-(off-1));
		return false;
	}
	if (parser->state == brtp_state_single) {
		return true; /* just abort */
	}
	parser->multi_len += len-off;
	abort = b_rtp_parser_h264_data(parser, packet, (uint8_t *)data+off, len-off);
	if (B_RTP_GET_BIT(fu_hdr,1)) { /* end bit */
		parser->state = brtp_state_single; /* will cause a commit */
		parser->multi_len = 0;
	} else if (parser->multi_len > 16 * 1024 ) { /* send packet to the mux */
		BDBG_MSG(("b_rtp_parser_h264_fu: %#lx commiting %u bytes to the decoder", (unsigned long)parser, parser->multi_len));
		parser->multi_len = 0;
		brtp_parser_commit(&parser->parent);
		abort = brtp_parser_begin(&parser->parent);
	}
	return abort;
}


static void 
b_rtp_parser_h264_packet(brtp_parser_t parser_, brtp_packet_t pkt, const void *data, size_t len)
{
	brtp_parser_h264_t parser = (brtp_parser_h264_t)parser_;
	uint8_t nal;
	unsigned type;
	bool abort;

	BDBG_OBJECT_ASSERT(parser, brtp_parser_h264_t);

	BDBG_ASSERT(parser->parent.stream.mux);

	nal = B_RTP_LOAD8(data,0);

	type = B_RTP_GET_BITS(nal, 3, 7);
	BDBG_MSG(("NAL: 0x%02x F:%u NRI:%u Type:%u %u", *(uint8_t *)data, B_RTP_GET_BIT(nal,0), B_RTP_GET_BITS(nal,1,2), type, len));
	if (parser->state == brtp_state_single) {
		brtp_parser_begin(&parser->parent);
	}
	abort = true;
	if (type==0) {
		goto discard;

	} else if (type <= 23) { /* NAL unit */
		b_rtp_parser_h264_flush_multi(parser);
		b_rtp_parser_h264_check_nal(parser, *(uint8_t *)data);
		b_rtp_parser_h264_nal(parser);
		abort = b_rtp_parser_h264_data(parser, pkt, data, len);

	} else if (type == 24) { /* STAP-A */
		b_rtp_parser_h264_flush_multi(parser);
		abort = b_rtp_parser_h264_stap(parser, pkt, data, len, 1);

	} else if (type == 25) { /* STAP-B */
		unsigned don = B_RTP_LOAD16(data, 1);
		BDBG_WRN(("don %d", don));
		b_rtp_parser_h264_flush_multi(parser);
		abort = b_rtp_parser_h264_stap(parser, pkt, data, len, 3);

	} else if (type == 28) { /* FU-A */
		abort = b_rtp_parser_h264_fu(parser, pkt, data, len, 2);

	} else if (type == 29) { /* FU-B */
		unsigned don = B_RTP_LOAD16(data, 2);

		BSTD_UNUSED(don);
		abort = b_rtp_parser_h264_fu(parser, pkt, data, len, 3);

	} else {
		BDBG_ERR(("b_rtp_parser_h264_packet: %#lx unknown type %d", (unsigned long)parser, type));
		BDBG_ASSERT(0);
		goto discard;
	}
	if (abort) {
		brtp_parser_abort(&parser->parent);
		parser->state = brtp_state_single;
		parser->multi_len = 0;
	} else {
		if (parser->state == brtp_state_single) {
			brtp_parser_commit(&parser->parent);
		}
	}
	return;
discard:
	brtp_parser_abort(&parser->parent);
	BDBG_WRN(("discard type %d", type));
	BDBG_WRN(("NAL: 0x%02x F:%u NRI:%u Type:%u %u", *(uint8_t *)data, B_RTP_GET_BIT(nal,0), B_RTP_GET_BITS(nal,1,2), type, len));
	return;
}

static void 
b_rtp_parser_h264_discontinuity(brtp_parser_t parser_)
{
	brtp_parser_h264_t parser = (brtp_parser_h264_t)parser_;
	BDBG_OBJECT_ASSERT(parser, brtp_parser_h264_t);
	if (parser->state == brtp_state_multi) { /* accumulating packets */
		BDBG_WRN(("b_rtp_parser_h264_discontinuity: %#lx dropping accumulated data", (unsigned long)parser));
		brtp_parser_abort(&parser->parent);
		parser->state = brtp_state_single;
	}
	return;
}

brtp_parser_h264_cfg b_rtp_parser_h264_default_cfg = {
	4+128,/* meta_len nal_hdr, SPS + PPS */
	NULL 	/* meta */
};

void
brtp_parser_h264_default_cfg(brtp_parser_h264_cfg *cfg)
{
	*cfg = b_rtp_parser_h264_default_cfg;
	return;
}



static const char *
b_strchr(const char *str, char ch)
{
	char str_ch;

	while( '\0'!= (str_ch=*str)) {
		if (str_ch==ch) {
			return str;
		}
		str++;
	}
	return NULL;
}

#if 0
static size_t 
b_strlen(const char *str) 
{
	size_t len;
	for(len=0;str[len];len++) { }
	return len;
}
#endif

static brtp_parser_mux_stream_t
b_rtp_parser_h264_start(brtp_parser_t parser_, brtp_parser_mux_t mux, const brtp_parser_mux_stream_cfg *cfg, const void *sprop, size_t sprop_len)
{
	brtp_parser_h264_t parser = (brtp_parser_h264_t)parser_;
	const char *sprop1;
	uint8_t *meta = (uint8_t *)parser->cfg.meta + 3;
	size_t meta_left = parser->cfg.meta_len - 3;
	int data_len;

	BDBG_OBJECT_ASSERT(parser, brtp_parser_h264_t);
	BDBG_ASSERT(sprop);
	BDBG_ASSERT(cfg);
	brtp_parser_mux_attach(mux, parser_, cfg);
	parser->meta_base = meta;

	brtp_parser_begin(&parser->parent);
	sprop1 = b_strchr(sprop, ',');
	if (sprop1 && meta_left>3) {
		meta[0] = 0;
		meta[1] = 0;
		meta[2] = 1;
		meta+=3;
		meta_left-=3;
		data_len = bbase64_decode(sprop, sprop1-(const char*)sprop, meta, meta_left);
		BDBG_MSG(("sprop1 '%s' len %i", sprop, data_len));
		if (data_len>0) {
			meta+=data_len;
			meta_left-=data_len;
		} else {
			BDBG_WRN(("corrupted meta information[1]"));
		}
		sprop_len -= 1+sprop1 - (const char *)sprop;
		sprop = sprop1+1; /* skip comma */
	} else {
		BDBG_WRN(("incomplete meta information[1]"));
	}

	if (meta_left>3) {
		meta[0] = 0;
		meta[1] = 0;
		meta[2] = 1;
		meta+=3;
		meta_left-=3;
		data_len = bbase64_decode(sprop, sprop_len, meta, meta_left);
		BDBG_MSG(("sprop2'%s' len %i",  sprop, data_len));
		if (data_len>0) {
			meta+=data_len;
			meta_left-=data_len;
		} else {
			BDBG_WRN(("corrupted meta information[2]"));
		}
	} else {
		BDBG_WRN(("incomplete meta information[2]"));
	}
	parser->meta_len = meta - parser->meta_base;
	parser->nal_state = brtp_nal_state_none;
	BDBG_MSG(("meta %d bytes", parser->meta_len));
	if(parser->meta_len) {
		brtp_parser_begin(&parser->parent);
		b_rtp_parser_h264_add_meta(parser);
		brtp_parser_commit(&parser->parent);
	}
	parser->state = brtp_state_single;
	parser->multi_len = 0;

	return  &parser->parent.stream;
}

static void 
b_rtp_parser_h264_stop(brtp_parser_t parser_)
{
	brtp_parser_h264_t parser = (brtp_parser_h264_t)parser_;
	BDBG_OBJECT_ASSERT(parser, brtp_parser_h264_t);
	brtp_parser_stop(&parser->parent);
	parser->state = brtp_state_single;
	return;
}

static void
b_rtp_parser_h264_destroy(brtp_parser_t parser_)
{
	brtp_parser_h264_t parser = (brtp_parser_h264_t)parser_;
	BDBG_OBJECT_ASSERT(parser, brtp_parser_h264_t);
	b_rtp_parser_h264_stop(parser_);
	BDBG_OBJECT_DESTROY(parser, brtp_parser_h264_t);
	BKNI_Free(parser);
	return;
}

brtp_parser_t 
brtp_parser_h264_create(const brtp_parser_h264_cfg *cfg)
{
	brtp_parser_h264_t parser;

	BDBG_CASSERT(B_RTP_GET_BIT((uint8_t)0x80, 0));
	BDBG_CASSERT(B_RTP_GET_BIT((uint8_t)0x40, 0)==0);
	BDBG_CASSERT(B_RTP_GET_BITS((uint8_t)0x80, 0,1)==2);

	BDBG_ASSERT(cfg);
	BDBG_ASSERT(cfg->meta);
	parser = BKNI_Malloc(sizeof(*parser));
	if (!parser) {
		BDBG_ERR(("out of memory in brtp_parser_h264_create"));
		return NULL;
	}
	BDBG_OBJECT_INIT(parser, brtp_parser_h264_t);
	brtp_parser_init(&parser->parent);
	parser->parent.packet = b_rtp_parser_h264_packet;
	parser->parent.discontinuity = b_rtp_parser_h264_discontinuity;
	parser->parent.start = b_rtp_parser_h264_start;
	parser->parent.stop = b_rtp_parser_h264_stop;
	parser->parent.destroy = b_rtp_parser_h264_destroy;
	parser->parent.type = brtp_parser_h264;
	parser->cfg = *cfg;
	parser->nal_header = cfg->meta;
	parser->nal_header[0] = 0;
	parser->nal_header[1] = 0;
	parser->nal_header[2] = 1;
	parser->state = brtp_state_single;
	return &parser->parent;
}


