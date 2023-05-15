/***************************************************************************
 *     Copyright (c) 2003-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bargs.c $
 * $brcm_Revision: 45 $
 * $brcm_Date: 7/30/09 12:15p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/utils/common/bargs.c $
 * 
 * 45   7/30/09 12:15p vsilyaev
 * PR 57223: Fixed bad use of pointers
 * 
 * 44   3/3/08 12:56p vsilyaev
 * PR 39818: Added MKV support
 * 
 * 43   2/21/08 10:36a jtna
 * PR38303: Update command line help
 * 
 * 42   10/26/07 10:19a rjlewis
 * PR36481: can't use bcrypto init before bsettop_init.
 * 
 * 41   10/18/07 2:17p jtna
 * PR35946: Coverity Defect ID:505 TAINTED_STRING
 * 
 * 40   6/20/07 4:55p erickson
 * PR32352: allow audio_format integers
 * 
 * 39   6/19/07 2:59p erickson
 * PR32352: make reusable name/value pair mappings for Settop API enums
 * 
 ********************************************************/
#include "bsettop.h"
#include "bargs.h"
#include "bsettop_namevalue_pairs.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define B_UNUSED(x) (void)x

struct b_arg b_arg; 
struct bqam_arg bqam_arg;
struct bmpeg_arg bmpeg_arg; 
struct bdisplay_arg bdisplay_arg;
struct banalog_arg banalog_arg;
struct bsds_arg bsds_arg;
struct btuner_arg btuner_arg;
struct bdecode_arg bdecode_arg;
struct bplayback_arg bplayback_arg;
struct brecord_arg brecord_arg;
struct bwindow_arg bwindow_arg;
struct bvolume_arg bvolume_arg;
struct buserio_arg buserio_arg;
struct bstreamer_arg bstreamer_arg;
struct bvsb_arg bvsb_arg;

void bargs_init( void )
{
	memset( &b_arg, 0, sizeof( b_arg ) );
	memset( &btuner_arg, 0, sizeof( btuner_arg ) );
	memset( &bdecode_arg, 0, sizeof( bdecode_arg ) );
	memset( &bplayback_arg, 0, sizeof( bplayback_arg ) );
	memset( &brecord_arg, 0, sizeof( brecord_arg ) );
	memset( &bqam_arg, 0, sizeof( bqam_arg ) );

	memset( &bmpeg_arg, 0, sizeof( bmpeg_arg ) );
	bmpeg_arg.storage.mpeg_type = bstream_mpeg_type_ts;
#if 0 /* can't do this until after 'bsetttop_init' */
	bencryption_params_init( &(bmpeg_arg.storage.encryption) ); 
	bmpeg_arg.storage.encryption.key_length = 64;
#else
	memset( &bmpeg_arg.storage.encryption, 0, sizeof( bmpeg_arg.storage.encryption ) );
#endif

	memset( &bdisplay_arg, 0, sizeof( bdisplay_arg ) );
	bdisplay_arg.storage.component = boutput_component_type_yprpb;
	bdisplay_arg.storage.format    = bvideo_format_ntsc;
	bdisplay_arg.storage.aspect_ratio = bdisplay_aspect_ratio_16x9;
	bdisplay_arg.storage.content_mode = bdisplay_content_mode_zoom;

	memset( &banalog_arg, 0, sizeof( banalog_arg ) );
	banalog_arg.storage.format = bvideo_format_ntsc;

	memset( &bsds_arg, 0, sizeof( bsds_arg ) );
	bsds_arg.storage.mode = btuner_sds_mode_qpsk_dvb;

	memset( &bvsb_arg, 0, sizeof( bvsb_arg ) );
	bvsb_arg.storage.mode = btuner_vsb_mode_vsb8;

	memset( &bwindow_arg, 0, sizeof( bwindow_arg ) );
	memset( &bvolume_arg, 0, sizeof( bvolume_arg ) );
	memset( &buserio_arg, 0, sizeof( buserio_arg ) );
	memset( &bstreamer_arg, 0, sizeof( bstreamer_arg ) );
}

static int
parse_annex(const char *arg, void **dest, void *storage)
{
	btuner_qam_annex annex;

	if (!strcasecmp(arg, "A")) {
		annex = btuner_qam_annex_a;
	} else if (!strcasecmp(arg, "B")) {
		annex = btuner_qam_annex_b;
	} else {
		return -1;
	}
	*(btuner_qam_annex*)storage = annex;
	*dest = storage;
	return 0;
}

int
parse_int(const char *arg, void **dest, void *storage)
{
	int val = (int)strtol(arg, NULL, 0);

	*(int *)storage = val;
	*dest = storage;
	return 0;
}

int
parse_freq(const char *arg, void **dest, void *storage)
{
	double dval = strtod(arg, NULL);
	int val;

	if (dval > 9000000.0) {
		val = (int) dval;
	} else {
		val = (int)(dval*1000000.0);
	}

	*(int *)storage = val;
	*dest = storage;
	return 0;
}

int
parse_set(const char *arg, void **dest, void *storage)
{
	B_UNUSED(arg);
	*(bool *)storage = true;
	*dest = storage;
	return 0;
}

#define DECLARE_PARSE_FUNC(FUNCNAME, ARRAY) \
	static int \
	FUNCNAME(const char *arg, void **dest, void *storage) \
	{ \
		if (b_get_value_i(ARRAY, arg, (unsigned *)storage)) \
			return -1; \
		*dest = storage; \
		return 0; \
	}
	
#define DECLARE_PARSE_FUNC_WITH_PASSTHROUGH(FUNCNAME, ARRAY) \
	static int \
	FUNCNAME(const char *arg, void **dest, void *storage) \
	{ \
		if (b_get_value_i(ARRAY, arg, (unsigned *)storage)) \
			return parse_int(arg, dest, storage); \
		*dest = storage; \
		return 0; \
	}
	
b_namevalue_pair component_type[] = {
	{"RGB", boutput_component_type_rgb},
	{"YPrPb", boutput_component_type_yprpb},
	{NULL, 0}
};
DECLARE_PARSE_FUNC(parse_component, component_type)

b_namevalue_pair hdcp[] = {
	{"hdcp", true},
	{"plain", false},
	{NULL, 0}
};
DECLARE_PARSE_FUNC(parse_hdcp, hdcp)

DECLARE_PARSE_FUNC(parse_aspect_ratio, bdisplay_aspect_ratio_pairs)
DECLARE_PARSE_FUNC(parse_content_mode, bdisplay_content_mode_pairs)
DECLARE_PARSE_FUNC(parse_display_format, bvideo_format_pairs)
/* analog input includes HD-VDEC support, so use all bvideo_format_pairs */
DECLARE_PARSE_FUNC(parse_input_format, bvideo_format_pairs)
DECLARE_PARSE_FUNC_WITH_PASSTHROUGH(parse_audio_type, baudio_format_pairs)
DECLARE_PARSE_FUNC_WITH_PASSTHROUGH(parse_video_type, bvideo_codec_pairs)
DECLARE_PARSE_FUNC(parse_sds_mode, btuner_sds_mode_pairs)
DECLARE_PARSE_FUNC_WITH_PASSTHROUGH(parse_mpeg_type, bmpeg_type_pairs)
DECLARE_PARSE_FUNC(parse_encryption, bencryption_type_pairs)

static int 
parse_nibble(char ch)
{
	if (ch >= '0' && ch <='9' ) return ch - '0';
	else if (ch >= 'A' && ch <= 'F') return (ch - 'A') + 10;
	else if (ch >= 'a' && ch <= 'f') return (ch - 'a') + 10;
	else return -1;
}

static int 
parse_octet(const char *arg) 
{
	int n1,n2;
   	n1 = parse_nibble(arg[0]);
	if(n1<0) return n1;
	n2 = parse_nibble(arg[1]);
	if(n2<0) return n1;

	return (n1<<4)|n2;
}

static int
parse_key(const char *arg, void **dest, void *storage)
{
	uint8_t key[16];
	unsigned i,off;
	bencryption_params *enc_cfg = storage;
	

	for(i=0;i<sizeof(key);i++) {
		key[i]=0;
	}
	for(off=0,i=0;arg[off];) {
		int byte;
		if (i>=sizeof(key)) {
			/* key  is too large */
			return -1;
		}
		if (arg[off]==',' || arg[off]==':') {
			off++; /* skip seperator */
			continue;  
		}
		byte = parse_octet(arg+off);
		if (byte<0) {
			/* invalid character */
			return -1;
		}
		off+=2;
		key[i++]=(uint8_t)byte;
	}
	/* copy key to destination */
	for(i=0;i<16;i++) {
		((uint8_t*)enc_cfg->key)[i] =  key[i];
	}
	*dest = storage;
	return 0;
}

int
parse_key_len(const char *arg, void **dest, void *storage)
{
	int val = (int)strtol(arg, NULL, 0);

	((bencryption_params*)storage)->key_length = val;
	*dest = storage;
	return 0;

}

int
parse_string(const char *arg, void **dest, void *storage)
{
	(void) storage;

	/* for string there is no storage, we just save pointer to the passed string */
	*(const char **)dest = arg;
	return 0;
}


const struct bargs b_args[] = {
	{"symrate", bargInteger, parse_int, "symbol rate", (void *)&b_arg.symbol_rate, &b_arg.storage.symbol_rate },
	{"freq", bargFloat, parse_freq, "tuning frequency", (void *)&b_arg.frequency, &b_arg.storage.frequency},
	{"vbi", bargNone, parse_set, "Enable VBI processing", (void *)&b_arg.vbi, &b_arg.storage.vbi},
	BARGS_TERMINATOR
};

const struct bargs bqam_args[] = {
	{"qam", bargInteger, parse_int, "QAM mode (64,256,1024)", (void *)&bqam_arg.mode, &bqam_arg.storage.mode },
	{"annex", bargString, parse_annex, "QAM annex (A,B)", (void *)&bqam_arg.annex, &bqam_arg.storage.annex },
	BARGS_TERMINATOR
};

const struct bargs bmpeg_args[] = {
	{"video", bargInteger, parse_int, "Video PID", (void *)&bmpeg_arg.video_pid, &bmpeg_arg.storage.video_pid},
	{"audio", bargInteger, parse_int, "Audio PID", (void *)&bmpeg_arg.audio_pid, &bmpeg_arg.storage.audio_pid},
	{"pcr", bargInteger, parse_int, "PCR PID", (void *)&bmpeg_arg.pcr_pid, &bmpeg_arg.storage.pcr_pid},
    {"audio_type", bargString, parse_audio_type, "audio coding type (MPEG1, AC3, AAC, DTS, AC3+, AAC+, WMA, DTSHD, VCD", (void *)&bmpeg_arg.audio_type, &bmpeg_arg.storage.audio_type},
	{"mpeg_type", bargString, parse_mpeg_type, "MPEG stream type (TS, PES, ES, DSS, DSS_ES, DSS_PES, VOB, ASF, AVI, MPEG1, VCD, MP4, FLV, MKV)", (void *)&bmpeg_arg.mpeg_type, &bmpeg_arg.storage.mpeg_type},
	{"encryption", bargString, parse_encryption, "encryption type (DES, 3DES or none)", (void *)&bmpeg_arg.encryption, &bmpeg_arg.storage.encryption},
	{"key", bargString, parse_key, "encryption key", (void *)&bmpeg_arg.encryption, &bmpeg_arg.storage.encryption},
	{"key_len", bargInteger, parse_key_len, "length of the encryption key", (void *)&bmpeg_arg.encryption, &bmpeg_arg.storage.encryption},
	/* TODO: this might replace video,audio,pcr,audio_type */
	{"mpeg", bargString, parse_string, "MPEG info: vid,pcr,aud,audtype", (void *)&bmpeg_arg.mpeg_str, NULL},
    {"video_type", bargString, parse_video_type, "Video coding type(MPEG1, MPEG2, MPEG4, H264, VC1, H263, DivX3, AVS)", (void *)&bmpeg_arg.video_type, &bmpeg_arg.storage.video_type},
	{"timestamp", bargNone, parse_set, "Enable timestamps (MPEG-2/DSS TS stream only)", (void *)&bmpeg_arg.timestamp, &bmpeg_arg.storage.timestamp},
    {"dual_stream", bargNone, parse_set, "Dual Stream case ( audio stream must be the last argument- no indexfile support)", (void *)&bmpeg_arg.dual_stream,&bmpeg_arg.storage.dual_stream },
    {"ignore_timestamp", bargNone, parse_set, "don't use extracted timestamps (MPEG-2/DSS TS stream only)", (void *)&bmpeg_arg.ignore_timestamp, &bmpeg_arg.storage.ignore_timestamp},
	BARGS_TERMINATOR
};

const struct bargs bdisplay_args[] = {
	{"display", bargInteger, parse_int, "Display number", (void *)&bdisplay_arg.display, &bdisplay_arg.storage.display},
	{"composite", bargNone, parse_set, "Enable composite output", (void *)&bdisplay_arg.composite, &bdisplay_arg.storage.composite},
	{"svideo", bargNone, parse_set, "Enable S-Video (Y/C) output", (void *)&bdisplay_arg.svideo, &bdisplay_arg.storage.svideo},
	{"none", bargNone, parse_set, "Disable all outputs", (void *)&bdisplay_arg.none, &bdisplay_arg.storage.none},
	{"component", bargString, parse_component, "Enable Composite output (RGB or YPrPb)", (void *)&bdisplay_arg.component, &bdisplay_arg.storage.component},
	{"dvi", bargString, parse_hdcp, "Enable DVI output ('hdcp' - encrypted or 'plain' - not encrypted)", (void *)&bdisplay_arg.dvi, &bdisplay_arg.storage.dvi},
	{"display_format", bargString, parse_display_format, "Display format (NTSC or PAL)", (void *)&bdisplay_arg.format, &bdisplay_arg.storage.format},
	{"aspect_ratio", bargString, parse_aspect_ratio, "Display aspect ratio (4x3 or 16x9)", (void *)&bdisplay_arg.aspect_ratio, &bdisplay_arg.storage.aspect_ratio},
	{"content_mode", bargString, parse_content_mode, "Display conversion mode box, panscan or zoom", (void *)&bdisplay_arg.content_mode, &bdisplay_arg.storage.content_mode},
	BARGS_TERMINATOR
};

const struct bargs banalog_args[] = {
	{"video_format", bargString, parse_input_format, "Analog input video format", (void *)&banalog_arg.format, &banalog_arg.storage.format},
	{"linein", bargInteger, parse_int, "Linein input number", (void *)&banalog_arg.linein, &banalog_arg.storage.linein},
	{"encode", bargNone, parse_set, "Encode analog signal to MPEG", (void *)&banalog_arg.encode, &banalog_arg.storage.encode},
	{"encode_channel", bargInteger, parse_int, "Encoder channel number", (void *)&banalog_arg.encode_channel, &banalog_arg.storage.encode_channel},
	{"encode_audio_type", bargString, parse_audio_type, "audio coding type", (void *)&banalog_arg.encode_audio_type, &banalog_arg.storage.encode_audio_type},
	BARGS_TERMINATOR
};

const struct bargs bsds_args[] = {
	{"sds", bargString, parse_sds_mode, "SDS mode (DVB or DSS)", (void *)&bsds_arg.mode, &bsds_arg.storage.mode},
	{"diseqc", bargInteger, parse_int, "Diseqc polarity: 13 or 18", (void *)&bsds_arg.diseqc_polarity, &bsds_arg.storage.diseqc_polarity},
	{"diseqc_tone", bargInteger, parse_int, "Diseqc tone: 1 or 0", (void *)&bsds_arg.diseqc_tone, &bsds_arg.storage.diseqc_tone},
	BARGS_TERMINATOR
};

const struct bargs btuner_args[] = {
	{"tuner", bargInteger, parse_int, "Tuner channel", (void *)&btuner_arg.channel, &btuner_arg.storage.channel},
	BARGS_TERMINATOR
};

const struct bargs bdecode_args[] = {
	{"decode", bargInteger, parse_int, "Decoder channel", (void *)&bdecode_arg.channel, &bdecode_arg.storage.channel},
	{"skip", bargNone, parse_set, "Skip this decoder", (void *)&bdecode_arg.skip, &bdecode_arg.storage.skip},
	BARGS_TERMINATOR
};

const struct bargs bplayback_args[] = {
	{"playback", bargInteger, parse_int, "Playback channel", (void *)&bplayback_arg.channel, &bplayback_arg.storage.channel},
	BARGS_TERMINATOR
};

const struct bargs brecord_args[] = {
	{"record", bargInteger, parse_int, "Record channel", (void *)&brecord_arg.channel, &brecord_arg.storage.channel},
	BARGS_TERMINATOR
};

const struct bargs bwindow_args[] = {
	{"x", bargInteger, parse_int, "X position for the decode window", (void *)&bwindow_arg.x, &bwindow_arg.storage.x},
	{"y", bargInteger, parse_int, "Y position for the decode window", (void *)&bwindow_arg.y, &bwindow_arg.storage.y},
	{"width", bargInteger, parse_int, "decode window width", (void *)&bwindow_arg.width, &bwindow_arg.storage.width},
	{"height", bargInteger, parse_int, "decode window height", (void *)&bwindow_arg.height, &bwindow_arg.storage.height},
	{"window", bargInteger, parse_int, "decode window index", (void *)&bwindow_arg.index, &bwindow_arg.storage.index},
	{"clip_x", bargInteger, parse_int, "left clipping for the decode window", (void *)&bwindow_arg.clip_x, &bwindow_arg.storage.clip_x},
	{"clip_y", bargInteger, parse_int, "right clipping for the decode window", (void *)&bwindow_arg.clip_y, &bwindow_arg.storage.clip_y},
	{"clip_width", bargInteger, parse_int, "width of the clipping rectangle", (void *)&bwindow_arg.clip_width, &bwindow_arg.storage.clip_width},
	{"clip_height", bargInteger, parse_int, "height of the clipping rectangle", (void *)&bwindow_arg.clip_height, &bwindow_arg.storage.clip_height},

	/* TODO: this might replace x,y,width,height as individual parameters */
	{"rect", bargString, parse_string, "Window x,y,width,height", (void *)&bwindow_arg.window_rect, NULL},
	BARGS_TERMINATOR
};

const struct bargs bvolume_args[] = {
	{"vol", bargInteger, parse_int, "DAC volume in dB from -100 to 100", (void *)&bvolume_arg.volume, &bvolume_arg.storage.volume },
	{"mute", bargNone, parse_set, "DAC mute", (void *)&bvolume_arg.mute, &bvolume_arg.storage.mute},
	BARGS_TERMINATOR
};

const struct bargs buserio_args[] = {
	{"message", bargString, parse_string, "message to display on front panel", (void *)&buserio_arg.message, NULL },
	{"input", bargInteger, parse_int, "Remote control (0-Remote A, 1-Remote B, 2-Sejin Kbd, 3-Keypad, 4-IR EchoStar, 5-UHF DirecTV, 6-UHF Echostar", (void *)&buserio_arg.input, &buserio_arg.storage.input},
	BARGS_TERMINATOR
};

const struct bargs bstreamer_args[] = {
	{"streamer", bargInteger, parse_int, "Streamer channel", (void *)&bstreamer_arg.channel, &bstreamer_arg.storage.channel},
	BARGS_TERMINATOR
};

const struct bargs bvsb_args[] = {
	{"vsb", bargInteger, parse_int, "vsb mode (8 or 16)", (void *)&bvsb_arg.mode, &bvsb_arg.storage.mode},
	BARGS_TERMINATOR
};

static int
barg_test_type(const char *arg, barg_type type)
{
	char *endptr;

	switch(type) {
	case bargInteger:
		strtol(arg, &endptr, 0);
		if (*arg != '\0' && *endptr =='\0') {
			return 0;
		}
		return -1;
	case bargFloat:
		strtod(arg, &endptr);
		if (*arg != '\0' && *endptr =='\0') {
			return 0;
		}
		return -1;
	case bargString:
	default:
		return 0;
	}
}

/* coverity[ +tainted_string_sanitize_content : arg-0 ] */
int 
barg_parse(const char *argv[], const bargs *args[], int argc_begin, int argc_end)
{
	int i;

	for(i=argc_begin; i<argc_end; i++) {
		const char *arg = argv[i]+1;
		int j,k;

		if (argv[i][0]!='-') {
			return i; /* not an argument */
		}
		if (argv[i][1]=='\0' || (argv[i][1]=='-' && argv[i][2]=='\0')) {
			return i; /* separator */
		}
		for(j=0;args[j]!=NULL;j++) {
			for (k=0;args[j][k].arg!=NULL;k++) {
				if (!strcmp(arg, args[j][k].arg)) { /* Bingo */
					int result;
					switch(args[j][k].type) {
						default:
							return -i;
						case bargNone:
							result = args[j][k].parse(NULL, args[j][k].dest, args[j][k].storage);
							break;

						case bargFloat:
						case bargInteger:
						case bargString:
							if (i==argc_end-1) {
								return -i;
							}
							i++;
							if (barg_test_type(argv[i], args[j][k].type)<0) {
								return -i;
							}
							result =  args[j][k].parse(argv[i], args[j][k].dest, args[j][k].storage);
							break;
					}
					if (result<0) {
						return -i;
					}
					goto next_arg;
				}
			}
		}
		/* keyword not found */
		return -i;
next_arg:;
	}
	return 0;
}

void 
barg_usage(const char *name, const bargs *args[], const char *extra)
{
	int i,j;
	fprintf(stderr, "Usage: %s <args> %s\nWhere args are:\n", name, extra);
	for(i=0;args[i]!=NULL;i++) {
		for (j=0;args[i][j].arg!=NULL;j++) {
			const char *val;
			switch(args[i][j].type) {
				default:
				case bargNone:
					val = "";
					break;

				case bargInteger:
				case bargString:
					val = " <val>";
					break;
			}
			fprintf(stderr, "-%s%s %s\n", args[i][j].arg, val, args[i][j].desc);
		}
	}
	return;
}

void 
barg_clear(const bargs *args[])
{
	int i,j;
	for(i=0;args[i]!=NULL;i++) {
		for (j=0;args[i][j].arg!=NULL;j++) {
			*args[i][j].dest = NULL;
		}
	}
	return;
}

