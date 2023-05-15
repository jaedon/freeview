/***************************************************************************
 *     (c)2012 Broadcom Corporation
 *
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 *  Except as expressly set forth in the Authorized License,
 *
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 *  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: bargs.h $
 * $brcm_Revision: 13 $
 * $brcm_Date: 9/6/12 6:03p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/utils/common/bargs.h $
 * 
 * 13   9/6/12 6:03p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/

#ifndef BARGS_H__
#define BARGS_H__

#ifdef __vxworks
/* This is defined inside Settop API and is exported */
extern int strcasecmp(const char *s1, const char *s2);
#endif

typedef enum {bargNone, bargInteger, bargString, bargFloat} barg_type;
typedef struct bargs {
	const char *arg;
	barg_type type;
	int (*parse)(const char *arg, void **dest, void *storage);
	const char *desc;
	void **dest;
	void *storage;
} bargs;

struct b_arg {
	int *frequency;
	int *symbol_rate;
	bool *vbi;
	struct {
		int frequency;
		int symbol_rate;
		bool vbi;
	} storage;
};

struct btuner_arg {
	int *channel;
	struct {
		int channel;
	} storage;
};

struct bdecode_arg {
	int *channel;
	bool *skip;
	struct {
		int channel;
		bool skip;
	} storage;
};

struct bplayback_arg {
	int *channel;
	struct {
		int channel;
	} storage;
};

struct brecord_arg {
	int *channel;
	struct {
		int channel;
	} storage;
};

struct bqam_arg {
	int *mode;
	char *annex;
	struct {
		int mode; /* 64,256, 1024 */
		char annex; /* a, b */
	} storage;
};

struct bmpeg_arg {
	int *video_pid, *audio_pid, *pcr_pid, *audio_type, *video_type;
	bstream_mpeg_type *mpeg_type;
	const char *mpeg_str;
	bencryption_params *encryption;
	bool *timestamp;
	bool *ignore_timestamp;
    bool *dual_stream;
	struct {
		int video_pid, audio_pid, pcr_pid, audio_type, video_type;
		bstream_mpeg_type mpeg_type;
		bencryption_params encryption;
		bool timestamp;
		bool ignore_timestamp;
        bool dual_stream;
	} storage;
};

struct bdisplay_arg {
	bool *svideo;
	bool *composite;
	bool *dvi;
	bool *none;
	boutput_component_type *component;
	bvideo_format *format;
	bdisplay_aspect_ratio *aspect_ratio;
	bdisplay_content_mode *content_mode;
	int *display;
	struct {
		bool svideo;
		bool composite;
		bool dvi;
		bool none;
		boutput_component_type component;
		bvideo_format format;
		bdisplay_aspect_ratio aspect_ratio;
		bdisplay_content_mode content_mode;
		int display;
	} storage;
};

struct banalog_arg {
	bvideo_format *format;
	int  *linein;
	bool *encode;
	int *encode_channel;
	int *encode_audio_type;
	struct {
		bvideo_format format;
		int linein;
		bool encode;
		int encode_channel;
		int encode_audio_type;
	} storage;
};

struct bsds_arg {
	btuner_sds_mode *mode;
	int *diseqc_polarity;
	int *diseqc_tone;
	struct {
		btuner_sds_mode mode;
		int diseqc_polarity; /* 13,18 */
		int diseqc_tone; /* 13,18 */
	} storage;
};

struct bvsb_arg {
	btuner_vsb_mode *mode;
	struct {
	btuner_vsb_mode mode;
	} storage;
};

struct bwindow_arg {
	unsigned *width, *height, *index;
	int *x, *y;
	const char *window_rect;
	unsigned *clip_x, *clip_y; 
	unsigned *clip_width, *clip_height;
	struct {
		unsigned width, height, index;
		int x, y;
		unsigned clip_x, clip_y, clip_width, clip_height;
	} storage;
};

struct bvolume_arg {
	unsigned *volume;
	bool *mute;
	struct {
		unsigned volume;
		bool mute;
	} storage;
};

struct buserio_arg {
	const char *message;
	unsigned *input;
	struct {
		unsigned input;
	} storage;
};

struct bstreamer_arg {
	unsigned *channel;
	struct {
		unsigned channel;
	} storage;
};

extern struct b_arg b_arg;
extern struct bqam_arg bqam_arg;
extern struct bmpeg_arg bmpeg_arg;
extern struct bdisplay_arg bdisplay_arg;
extern struct banalog_arg banalog_arg;
extern struct bsds_arg bsds_arg;
extern struct btuner_arg btuner_arg;
extern struct bdecode_arg bdecode_arg;
extern struct bplayback_arg bplayback_arg;
extern struct brecord_arg brecord_arg;
extern struct bwindow_arg bwindow_arg;
extern struct bvolume_arg bvolume_arg;
extern struct buserio_arg buserio_arg;
extern struct bstreamer_arg bstreamer_arg;
extern struct bvsb_arg bvsb_arg;

extern const struct bargs b_args[];
extern const struct bargs bqam_args[];
extern const struct bargs bmpeg_args[];
extern const struct bargs bdisplay_args[];
extern const struct bargs banalog_args[];
extern const struct bargs bsds_args[];
extern const struct bargs btuner_args[];
extern const struct bargs bdecode_args[];
extern const struct bargs bplayback_args[];
extern const struct bargs brecord_args[];
extern const struct bargs bwindow_args[];
extern const struct bargs bvolume_args[];
extern const struct bargs buserio_args[];
extern const struct bargs bstreamer_args[];
extern const struct bargs bvsb_args[];

void bargs_init(void);
void barg_clear(const bargs *args[]);
int barg_parse(const char *argv[], const bargs *args[], int argc_begin, int argc_end);
void barg_usage(const char *name, const bargs *args[], const char *extra);

#define BARGS_TERMINATOR {NULL, 0, 0, NULL, NULL, NULL }
/* general purpose bargs parse functions */
int parse_string(const char *arg, void **dest, void *storage);
int parse_int(const char *arg, void **dest, void *storage);
int parse_set(const char *arg, void **dest, void *storage);

#endif /* BARGS_H__ */

