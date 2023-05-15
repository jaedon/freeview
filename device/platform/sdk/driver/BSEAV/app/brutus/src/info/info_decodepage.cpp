/***************************************************************************
 *     Copyright (c) 2003-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: info_decodepage.cpp $
 * $brcm_Revision: 13 $
 * $brcm_Date: 3/17/08 3:18p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/info/info_decodepage.cpp $
 * 
 * 13   3/17/08 3:18p erickson
 * PR40307: remove old, unused code before adding new
 * 
 * 12   6/19/07 3:19p erickson
 * PR32352: convert to bvideo_frame_rate_pairs
 * 
 * 11   3/21/07 11:57a erickson
 * PR26201: fix up decode info page
 * 
 * 10   6/12/06 11:18a erickson
 * PR21646: added bvideo_frame_rate
 * 
 * 9   3/10/06 11:19a erickson
 * PR19106: added SAR
 * 
 * 8   2/9/06 1:35p erickson
 * PR17108: use a/v type strings instead of numbers
 * 
 * 7   12/8/05 11:33a erickson
 * PR18316: remove strings for bad BFMT_AspectRatio values
 * 
 * 6   10/12/05 4:04p erickson
 * PR15241: added interlaced flag
 * 
 * 5   9/9/05 3:02p erickson
 * PR17046: extended list of aspect ratios
 * 
 * 4   9/2/05 3:55p erickson
 * PR16639: added vidtype to status panel
 * 
 * 3   8/25/05 4:18p erickson
 * PR16819: make DecodePage responsive to Control Start/StopDecode events,
 * and handle analog and no decode explicitly
 * 
 * 2   8/24/05 11:09a erickson
 * PR16819: simplify code using AVStream::getStream
 * 
 * 1   2/7/05 8:08p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/23   1/4/05 9:13p rjain
 * PR13528: including video.h
 * 
 * Irvine_BSEAVSW_Devel/22   12/14/04 5:17p marcusk
 * PR13528: Added additional audio status, stream type, number of
 * channels, downmix mode, and sample rate.
 * 
 * Irvine_BSEAVSW_Devel/21   11/2/04 3:44p erickson
 * PR12897: added aspect ratio to ui, removed unrunable 7115 test code
 * 
 * Irvine_BSEAVSW_Devel/20   10/15/04 10:51a erickson
 * PR13014: infobar rework
 * 
 * Irvine_BSEAVSW_Devel/19   9/15/04 2:41p erickson
 * PR9683: use explicity _FLAVOR or _FAMILY instead of BCM7### defines
 * 
 * Irvine_BSEAVSW_Devel/18   5/4/04 10:16a erickson
 * PR10832: remember to getSettings!
 * 
 * Irvine_BSEAVSW_Devel/17   4/30/04 12:06p erickson
 * PR10832: rework of brutus to support single AVManager and
 * ChannelManager for system
 * 
 * Irvine_BSEAVSW_Devel/16   12/4/03 9:39a erickson
 * PR8813: complete rework of info UI to support vertical bar
 * 
 * Irvine_BSEAVSW_Devel/15   11/4/03 2:41p erickson
 * changed from bstream_get_status to bstream_get_mpeg_parameters
 * 
 * Irvine_BSEAVSW_Devel/14   10/28/03 11:25a erickson
 * settop api reworks after internal design view
 * 
 * Irvine_BSEAVSW_Devel/13   10/22/03 11:22a erickson
 * settop api pass-through type conversion
 * 
 * Irvine_BSEAVSW_Devel/12   9/19/03 2:24p erickson
 * don't allow register access...extend the api if this is needed
 *
 * Irvine_BSEAVSW_Devel/11   9/17/03 12:32p erickson
 * converted from original debug interface to magnum debug interface
 *
 * Irvine_BSEAVSW_Devel/10   9/9/03 5:30p erickson
 * 7110 dual record/single decode support. this involves added
 * currentDecode which is different
 * from currentSource. This changes the logic a lot. Still in progress,
 * but it basically works.
 *
 * Irvine_BSEAVSW_Devel/9   8/25/03 9:18a erickson
 * fixed register read
 *
 * Irvine_BSEAVSW_Devel/8   8/22/03 5:20p erickson
 * venom2 settop api work
 *
 ************************************************************************/
#include "info_decodepage.h"
#include "control.h"
#include "bsettop_namevalue_pairs.h"
BDBG_MODULE(brutus_decodepage);

DecodePage::DecodePage(InfoPanel *infoPanel, const MRect &rect) :
	InfoPanelStatusPage(infoPanel, rect)
{
	addLabel(2,  "PCR PID:");
	addLabel(0,  "Video PID:");
	addLabel(1,  "    codec:");
	addLabel(5,  "    size:");
	addLabel(6,  "    rate:");
	addLabel(7,  "    A/R:");
	addLabel(3,  "Audio PID:");
	addLabel(4,  "    codec:");
	addLabel(8,  "    channels:");
	addLabel(9,  "    SPDIF:");
	addLabel(11, "    sample rate:");
	addLabel(12, "    downmix:");
	
	control()->addControlEventHandler(this);
}

/* From magnum's BFMT_AspectRatio in bfmt.h or legacy's MpegVideo PI. */
static const char *g_aspect_ratio[] = {
	"Unknown",
	"Square Pixel",
	"4:3",
	"16:9",
	"2.21:1",
	"15:9",
	"Source"
};

const char *b_frame_rate_str(bvideo_frame_rate frame_rate) {
	const char *name;
	if (!b_get_name(bvideo_frame_rate_pairs, frame_rate, &name))
		return name;
	return "Unknown";
}

void DecodePage::onControlEvent(Control *control, Control::Event event)
{
	switch (event) {
	case Control::evMoveWindowControl:
	case Control::evStopDecode:
	case Control::evStartDecode:
		read();
	}
}

void DecodePage::read()
{
	bool is_analog = false;
	DecodeEngine *decengine = control()->decode();
	if (!decengine) 
		return;
		
	/* get mpeg params from stream, either live digital or pvr */
	bstream_t stream;
	bstream_status status;
	bstream_mpeg_init(&status.mpeg);
	stream = decengine->getStream();
	
	if (stream) {
		if (bstream_get_mpeg_parameters(stream, &status)) {
			is_analog = true; /* this is a bit cheap */
		}
	}

	DecodeEngine::Settings settings;
	bdecode_status decode_status;
	decengine->getSettings(&settings);
	BDBG_ASSERT(settings.decode);
	bdecode_get_status(settings.decode, &decode_status);

	if (!stream) {
		setLabelValue(0, "No decode");
		setLabelValue(1, "");
		setLabelValue(2, "");
		setLabelValue(3, "");
		setLabelValue(4, "");
	}
	else if (is_analog) {
		setLabelValue(0, "Analog");
		setLabelValue(1, "");
		setLabelValue(2, "");
		setLabelValue(3, "");
		setLabelValue(4, "");
	}
	else {
		setLabelValue(0, "0x%x", status.mpeg.video[decode_status.video_program].pid);
		setLabelValue(1, b_vidtype_str(status.mpeg.video[decode_status.video_program].format));
		setLabelValue(2, "0x%x", status.mpeg.pcr_pid);
		setLabelValue(3, "0x%x", status.mpeg.audio[decode_status.audio_program].pid);
		setLabelValue(4, "%s %s", 
			b_audtype_str(status.mpeg.audio[decode_status.audio_program].format),
			decode_status.audio_layer);
	}
	setLabelValue(5, "%ldx%ld (%c)", decode_status.source_width, decode_status.source_height, decode_status.interlaced?'i':'p');
	setLabelValue(6, "%s Hz", b_frame_rate_str(decode_status.video_frame_rate));
	setLabelValue(7, "%s", g_aspect_ratio[decode_status.video_aspect_ratio]);
	setLabelValue(8, "%s", decode_status.audio_channels);
	setLabelValue(9, "%s", decode_status.audio_compressed_spdif_output?"Compressed":"PCM");
	setLabelValue(11, "%ld", decode_status.audio_sample_rate);
	setLabelValue(12, "%s", decode_status.audio_downmix_mode);
}
