/***************************************************************************
 *     Copyright (c) 2006-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: brtp_parser_mpeg4.c $
 * $brcm_Revision: 7 $
 * $brcm_Date: 11/22/11 1:18p $
 *
 * Module Description:
 *
 * RTP parser library
 *   RFC 3640 module
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/rtp/brtp_parser_mpeg4.c $
 * 
 * 7   11/22/11 1:18p vsilyaev
 * SW7425-1797: Merged  G711 support for rtp parser mpge4
 * 
 * SW7425-1797/5   11/22/11 11:52a xpl
 * SW7425-1797: Add G711 support for rtp parser mpge4
 * 
 * SW7425-1797/4   11/21/11 5:53p xpl
 * SW7425-1797: Add G711 support for rtp parser mpge4
 * 
 * SW7425-1797/3   11/21/11 2:46p xpl
 * SW7425-1797: Add G711 support for rtp parser mpge4
 * 
 * SW7425-1797/2   11/18/11 11:43a chengs
 * SW7425-1798: Remove marker bit condition to be compliant with 3rd party
 * soft SIP phone
 * 
 * SW7425-1797/1   11/18/11 11:14a chengs
 * SW7425-1797: Add G711 support for rtp parser mpge4.
 * 
 * 6   10/5/06 10:33a vsilyaev
 * PR 23826: Match profile against both  hex and decimal values
 * 
 * 5   9/25/06 3:20p vsilyaev
 * PR 23826: Added throttled sync
 * 
 * 4   9/18/06 4:39p vsilyaev
 * PR 23826: More work for RTP timing
 * 
 * 3   9/7/06 4:34p vsilyaev
 * PR 23826: Added timestamp test
 * 
 * 2   8/28/06 4:35p vsilyaev
 * PR 23826: Added audio+video muxing
 * 
 * 1   8/25/06 2:07p vsilyaev
 * PR 23826: Added MPEG-4 AAC audio parser
 * 
 *******************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "brtp_parser_mpeg4.h"
#include "brtp_packet.h"
#include "biobits.h"
#include "brtp_util.h"

BDBG_MODULE(brtp_parser_mpeg4);

BDBG_OBJECT_ID(brtp_parser_mpeg4_t);

struct brtp_parser_mpeg4 {
	struct brtp_parser parent; /* must be the first element */
	BDBG_OBJECT(brtp_parser_mpeg4_t)
	brtp_parser_mpeg4_stream_cfg stream_cfg;
	unsigned pkt_cnt;
	unsigned pkt_len;
	brtp_parser_mpeg4_cfg cfg;
	union {
		struct {
			uint8_t adts_header[7];
			uint8_t adts_header_3;
		} aac;
		struct {
			uint8_t wav_header[8/*BCMA header*/+18/*WAVEFORMATEX*/];
		} g711;
	} codec;
};

typedef struct brtp_parser_mpeg4 *brtp_parser_mpeg4_t;

static const brtp_parser_type b_rtp_parser_mpeg4= {
	"MPEG4/AAC",
	brtp_stream_audio
};

const brtp_parser_type *brtp_parser_mpeg4 = &b_rtp_parser_mpeg4;



void
brtp_parser_mpeg4_default_cfg(brtp_parser_mpeg4_cfg *cfg)
{
	*cfg = 0;
	return;
}

static const brtp_parser_mpeg4_stream_cfg b_rtp_parser_mpeg4_aac_hbr_cfg = {
	brtp_parser_mpeg4_aac_hbr, /* mode */
	13, /* sizelength */
	3, /* indexlength */
	2, /* headerlength */
	{0}, /* config */
	0, /* config_len */
	15 /* profile */
};

static const brtp_parser_mpeg4_stream_cfg b_rtp_parser_mpeg4_g711_cfg = {
	brtp_parser_mpeg4_g711, /* mode */
	13, /* sizelength */
	0, /* indexlength */
	0, /* headerlength */
	{0}, /* config */
	0, /* config_len */
	15 /* profile */
};

void 
brtp_parser_mpeg4_default_stream_cfg(brtp_parser_mpeg4_mode mode, brtp_parser_mpeg4_stream_cfg *cfg)
{
	BDBG_ASSERT(cfg);
	switch(mode) {
	default:
		BDBG_WRN(("brtp_parser_mpeg4_default_stream_cfg: not supported mode %u", mode));
		cfg->mode = mode;
		break;
	case brtp_parser_mpeg4_aac_hbr:
		*cfg = b_rtp_parser_mpeg4_aac_hbr_cfg;
		break;
	case brtp_parser_mpeg4_g711:
		*cfg = b_rtp_parser_mpeg4_g711_cfg;
		break;
	}
	return;
}

static void
b_rtp_parser_mpeg4_begin(brtp_parser_mpeg4_t parser)
{
	brtp_parser_begin(&parser->parent);
	parser->pkt_cnt = 0;
	parser->pkt_len = 0;
	return;
}

static void
b_rtp_parser_mpeg4_commit(brtp_parser_mpeg4_t parser)
{
	unsigned len;

	switch(parser->stream_cfg.mode) {
	case brtp_parser_mpeg4_aac_hbr: 
		len = parser->pkt_len + sizeof(parser->codec.aac.adts_header);
		/* IS 13818-7 1.2.2	Variable Header of ADTS */
		parser->codec.aac.adts_header[3] = parser->codec.aac.adts_header_3 |
			B_SET_BITS(aac_frame_length[12..11], B_GET_BITS(len, 12, 11), 1, 0);
		parser->codec.aac.adts_header[4] = 
			B_SET_BITS(aac_frame_length[10..3], B_GET_BITS(len, 10, 3), 7, 0);
		parser->codec.aac.adts_header[5] = 
			B_SET_BITS(aac_frame_length[2..0], B_GET_BITS(len, 2, 0), 7, 5) |
			B_SET_BITS(adts_buffer_fullness[10..6], B_GET_BITS( 0x1FF /* VBR */, 10, 6), 4, 0);
		parser->codec.aac.adts_header[6] = 
				B_SET_BITS(adts_buffer_fullness[5..0], B_GET_BITS( 0x1FF /* VBR */, 5, 0), 7, 2) |
				B_SET_BITS(no_raw_data_blocks_in_frame, parser->pkt_cnt - 1, 2, 0);
		break;
    case brtp_parser_mpeg4_g711:
		len = parser->pkt_len;
		parser->codec.g711.wav_header[4] = (len >> 24) & 0xff;
		parser->codec.g711.wav_header[5] = (len >> 16) & 0xff;
		parser->codec.g711.wav_header[6] = (len >> 8) & 0xff;
		parser->codec.g711.wav_header[7] = len & 0xff;
		break;
	}
	brtp_parser_commit(&parser->parent);
	return;
}

static void 
b_rtp_parser_mpeg4_packet(brtp_parser_t parser_, brtp_packet_t pkt, const void *data, size_t len)
{
	brtp_parser_mpeg4_t parser = (brtp_parser_mpeg4_t)parser_;
	bio_bitstream bs;
	bio_cursor cursor;
	bio_array array;
	unsigned size, index;
	unsigned header_len;
	unsigned off=0;
	bool overflow=false;

	BSTD_UNUSED(len);

	BDBG_OBJECT_ASSERT(parser, brtp_parser_mpeg4_t);
	BDBG_ASSERT(parser->parent.stream.mux);
	BDBG_MSG(("b_rtp_parser_mpeg4_packet: %#lx %lx:%u %s %u", (unsigned long)parser, (unsigned long)pkt, len, B_RTP_PKT_MARKER(pkt)?"M":"", B_RTP_PKT_TIMESTAMP(pkt)));
	if (parser->stream_cfg.headerlength) {
		/* read AU header */
		header_len = B_RTP_LOAD16(data,0);
		/* skip the header */
		off = 2 + (header_len+7)/8; /* number of bytes */
		bio_cursor_from_range(&cursor, &array, (uint8_t *)data+2, len-2);
		bio_bitstream_init(&bs, &cursor);
		while(header_len>0) {
			size = bio_bitstream_bits(&bs, parser->stream_cfg.sizelength);
			index = bio_bitstream_bits(&bs, parser->stream_cfg.indexlength);
			BDBG_MSG(("b_rtp_parser_mpeg4_packet: %#lx header_len %u size %u index %u", (unsigned long)parser, header_len, size, index));
			header_len -= parser->stream_cfg.indexlength + parser->stream_cfg.sizelength;
			overflow = brtp_parser_add_packet(&parser->parent, pkt, (uint8_t *)data+off, size);
			if (overflow) {
				brtp_parser_abort(&parser->parent);
				break;
			} 
			off += size;
			parser->pkt_len += size;
			parser->pkt_cnt++;
			if (B_RTP_PKT_MARKER(pkt)) {
				b_rtp_parser_mpeg4_commit(parser);
				b_rtp_parser_mpeg4_begin(parser);
			}
		}
	} else {
		/* just add data as is */
		overflow = brtp_parser_add_packet(&parser->parent, pkt, data, len);
		if (overflow) {
			brtp_parser_abort(&parser->parent);
		} else {
			parser->pkt_len += len;
			parser->pkt_cnt++;
			b_rtp_parser_mpeg4_commit(parser);
			b_rtp_parser_mpeg4_begin(parser);
		}
	}
	BDBG_ASSERT(len > 2);

	return;
}

static void 
b_rtp_parser_mpeg4_discontinuity(brtp_parser_t parser_)
{
	brtp_parser_mpeg4_t parser = (brtp_parser_mpeg4_t)parser_;
	BDBG_OBJECT_ASSERT(parser, brtp_parser_mpeg4_t);
	BDBG_ASSERT(parser->parent.stream.mux);
	brtp_parser_abort(&parser->parent);
	b_rtp_parser_mpeg4_begin(parser);
	return;
}



static brtp_parser_mux_stream_t
b_rtp_parser_mpeg4_start(brtp_parser_t parser_, brtp_parser_mux_t mux, const brtp_parser_mux_stream_cfg *cfg, const void *mpeg4_cfg_, size_t mpeg4_cfg_len)
{
	brtp_parser_mpeg4_t parser = (brtp_parser_mpeg4_t)parser_;
	bio_bitstream bs;
	bio_cursor cursor;
	bio_array array;
	unsigned stream_type;
	unsigned sampling_frequency_index;
	unsigned channel_configuration;
	unsigned profile;
	const brtp_parser_mpeg4_stream_cfg *mpeg4_cfg = mpeg4_cfg_;
	uint8_t *waveheader;

	BDBG_OBJECT_ASSERT(parser, brtp_parser_mpeg4_t);
	BDBG_ASSERT(parser->parent.stream.mux==NULL);
	BDBG_ASSERT(cfg);
	BDBG_ASSERT(mpeg4_cfg);
	BDBG_ASSERT(sizeof(*mpeg4_cfg)==mpeg4_cfg_len);

	parser->stream_cfg = *mpeg4_cfg;
	brtp_parser_mux_attach(mux, parser_, cfg);

	switch(mpeg4_cfg->mode) {
	case brtp_parser_mpeg4_aac_hbr: 
		bio_cursor_from_range(&cursor, &array, mpeg4_cfg->config, mpeg4_cfg->config_len);
		bio_bitstream_init(&bs, &cursor);
		/*  ISO/IEC 14496-3: 6.2.1 */
		stream_type = bio_bitstream_bits(&bs, 5);
		sampling_frequency_index = bio_bitstream_bits(&bs, 4);
		if (sampling_frequency_index==0xF) {
			BDBG_WRN(("brtp_parser_mpeg4_start: %#lx unsupported sampling_frequency_index  %u", (unsigned long)parser, sampling_frequency_index));
			bio_bitstream_drop_bits(&bs, 24);
		}
		channel_configuration = bio_bitstream_bits(&bs, 4);
		switch(mpeg4_cfg->profile) {
		case 0x15: 
		case 15: 
		case 41:
		case 0x41:
			profile = 1; break; /* AAC low complexity profile */
		default:
			BDBG_WRN(("brtp_parser_mpeg4_start: %#lx unknown profile %u", (unsigned long)parser, mpeg4_cfg->profile));
			profile = 0; break;
		}
		/* IS 13818-7  1.2.1	Fixed Header of ADTS  */
		parser->codec.aac.adts_header[0] = 
			0xFF; /* sync word */
		parser->codec.aac.adts_header[1] =
			0xF0 |  /* sync word */
			B_SET_BIT( ID, 1, 3) | /* MPEG-2 AAC */
			B_SET_BITS( "00", 0, 2, 1) |
			B_SET_BIT( protection_absent, 1, 0);
		parser->codec.aac.adts_header[2] =
			B_SET_BITS( profile, profile /* low-complexity profile */ , 7, 6) |
			B_SET_BITS( sampling_frequency_index, sampling_frequency_index, 5, 2 ) |
			B_SET_BIT( private_bit, 0, 1) |
			B_SET_BIT( channel_configuration[2], B_GET_BIT(channel_configuration, 2), 0);

		parser->codec.aac.adts_header_3 = /* 4'th byte is shared */
			B_SET_BITS( channel_configuration[2], B_GET_BITS(channel_configuration, 1, 0), 7, 6) |
			B_SET_BIT( original_copy, 0, 5) |
			B_SET_BIT( home, 0, 4) |
		/* IS 13818-7 1.2.2	Variable Header of ADTS */
			B_SET_BIT( copyright_identification_bit, 0, 3) |
			B_SET_BIT( copyright_identification_start, 0, 2);

		/* bytes 5,6 and 7 depend on the frame length */

		parser->parent.stream.header_len = sizeof(parser->codec.aac.adts_header);
		parser->parent.stream.header = parser->codec.aac.adts_header;
		break;

	case brtp_parser_mpeg4_g711:
		waveheader = parser->codec.g711.wav_header;
		waveheader[0] = 0x42;
		waveheader[1] = 0x43;
		waveheader[2] = 0x4d;
		waveheader[3] = 0x41;
		waveheader[4] = 0x00;
		waveheader[5] = 0x00;
		waveheader[6] = 0x01;
		waveheader[7] = 0x40;
		
		B_RTP_SAVE_UINT16_LE(waveheader+8, 1);
		B_RTP_SAVE_UINT16_LE(waveheader+10, 1);
		B_RTP_SAVE_UINT32_LE(waveheader+12, 8000);
		B_RTP_SAVE_UINT32_LE(waveheader+16, 16000);
		B_RTP_SAVE_UINT16_LE(waveheader+20, 1);
		B_RTP_SAVE_UINT16_LE(waveheader+22, 16);
		B_RTP_SAVE_UINT16_LE(waveheader+24, 0);
		
		parser->parent.stream.header_len = sizeof(parser->codec.g711.wav_header);
		parser->parent.stream.header = (void *) &(parser->codec.g711.wav_header);
		break;

	default:
		BDBG_WRN(("brtp_parser_mpeg4_start: %#lx not supported mode %u", (unsigned long)parser, mpeg4_cfg->mode));
		break;
	}
	b_rtp_parser_mpeg4_begin(parser);
	return  &parser->parent.stream;
}

void 
b_rtp_parser_mpeg4_stop(brtp_parser_t parser_)
{
	brtp_parser_mpeg4_t parser = (brtp_parser_mpeg4_t)parser_;
	BDBG_OBJECT_ASSERT(parser, brtp_parser_mpeg4_t);
	brtp_parser_stop(&parser->parent);
	return;
}

static void
b_rtp_parser_mpeg4_destroy(brtp_parser_t parser_)
{
	brtp_parser_mpeg4_t parser = (brtp_parser_mpeg4_t)parser_;

	BDBG_OBJECT_ASSERT(parser, brtp_parser_mpeg4_t);
	b_rtp_parser_mpeg4_stop(parser_);

	BDBG_OBJECT_DESTROY(parser, brtp_parser_mpeg4_t);
	BKNI_Free(parser);
	return;
}

brtp_parser_t 
brtp_parser_mpeg4_create(const brtp_parser_mpeg4_cfg *cfg)
{
	brtp_parser_mpeg4_t parser;

	BDBG_ASSERT(cfg);
	parser = BKNI_Malloc(sizeof(*parser));
	if (!parser) {
		BDBG_ERR(("brtp_parser_mpeg4_create: out of memory")); 
		return NULL;
	}
	BDBG_OBJECT_INIT(parser, brtp_parser_mpeg4_t);
	brtp_parser_init(&parser->parent);
	parser->parent.packet = b_rtp_parser_mpeg4_packet;
	parser->parent.discontinuity = b_rtp_parser_mpeg4_discontinuity;
	parser->parent.start = b_rtp_parser_mpeg4_start;
	parser->parent.stop = b_rtp_parser_mpeg4_stop;
	parser->parent.destroy = b_rtp_parser_mpeg4_destroy;
	parser->parent.type = brtp_parser_mpeg4;
	parser->cfg = *cfg;
	return &parser->parent;
}


