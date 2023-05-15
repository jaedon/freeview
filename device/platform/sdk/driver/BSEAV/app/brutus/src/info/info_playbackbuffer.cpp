/***************************************************************************
 *     Copyright (c) 2003-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: info_playbackbuffer.cpp $
 * $brcm_Revision: 14 $
 * $brcm_Date: 4/6/07 2:33p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/info/info_playbackbuffer.cpp $
 * 
 * 14   4/6/07 2:33p erickson
 * PR29587: remove PTS/STC msg in favor of Settop API monitor
 * 
 * 13   2/27/07 7:10p erickson
 * PR26400: print 2nd audio channel pts/stc
 * 
 * 12   2/7/07 4:01p erickson
 * PR26400: readded 2nd audio channel support
 * 
 * 11   1/26/07 6:18p vsilyaev
 * PR 26400: Use dedicated functions and structured for video and audio
 * status
 * 
 * 10   12/19/06 10:53a erickson
 * PR26644: added check for bdecode_pts_type
 * 
 * 9   11/15/06 7:34p bandrews
 * PR25456: Added audio passthru channel buffer display
 * 
 * 8   5/31/06 4:20p tokushig
 * PR21891: Merging updated picture viewer into mainline.
 * 
 * SanDiego_Brutus_Skin/4   5/30/06 4:39p tokushig
 * merge from main
 * 
 * 7   5/23/06 11:29a erickson
 * PR21163: use correct 45 KHz units for PTS
 * 
 * SanDiego_Brutus_Skin/3   5/16/06 11:33a tokushig
 * merge from main
 * 
 * 6   4/17/06 5:06p jgarrett
 * PR 20951: Merging skinned UI into mainline
 * 
 * SanDiego_Brutus_Skin/2   2/23/06 2:27p tokushig
 * merge from 97400 prerel label
 * 
 * 5   1/26/06 9:47a erickson
 * PR19003: convert TSM monitor to units of msec for MPEG2/DSS support
 * 
 * SanDiego_Brutus_Skin/1   2/13/06 4:38p tokushig
 * added partial skin ability.  note that this is primarily for cable show
 * demo purposes and will be expanded to be fully skinnable in the
 * future.
 * 
 * 4   1/6/06 5:27p erickson
 * PR17108: reworked TSM monitor
 * 
 * 3   12/20/05 6:23a erickson
 * PR18256: moved progress bar code into reuseable file
 * 
 * 2   9/19/05 1:52p erickson
 * PR16882: added dbg
 * 
 * 1   2/7/05 8:09p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/19   2/4/05 10:48a erickson
 * PR12728: cleanup for -pedantic warnings and errors
 * 
 * Irvine_BSEAVSW_Devel/18   10/15/04 10:51a erickson
 * PR13014: infobar rework
 * 
 * Irvine_BSEAVSW_Devel/17   5/5/04 1:00p erickson
 * PR10832: don't fail if bdecode_get_status fails
 * 
 * Irvine_BSEAVSW_Devel/16   4/30/04 12:06p erickson
 * PR10832: rework of brutus to support single AVManager and
 * ChannelManager for system
 * 
 * Irvine_BSEAVSW_Devel/15   2/19/04 3:32p bandrews
 * PR9723: Implemented ARGB1555 support and tested ARGB8888 support
 * 
 * Irvine_BSEAVSW_Devel/14   2/6/04 1:47p erickson
 * PR9107: buffer bars are opaque
 * 
 * Irvine_BSEAVSW_Devel/13   1/29/04 2:26p erickson
 * PR9107: merging back from BRUTUS_PR9107 branch for bwin development
 * 
 * Irvine_BSEAVSW_Devel/12   12/17/03 11:48p erickson
 * PR8850: If size or depth is greater than 1M, use megabytes, not
 * kilobytes
 * 
 * Irvine_BSEAVSW_Devel/11   12/4/03 2:26p erickson
 * PR8850: need to compile 7038 with no PVR_SUPPORT or ENCODER_SUPPORT for
 * now
 * 
 * Irvine_BSEAVSW_Devel/10   12/4/03 9:39a erickson
 * PR8813: complete rework of info UI to support vertical bar
 * 
 * Irvine_BSEAVSW_Devel/9   10/28/03 11:26a erickson
 * settop api reworks after internal design view
 * 
 * Irvine_BSEAVSW_Devel/8   10/10/03 1:43p erickson
 * converted brutus frontend code so that it can support all frontend
 * types simultaneously. this
 * is in preperation for 97038 and others.
 *
 *************************************************************/
#include "info_playbackbuffer.h"
#include "control.h"
#include <mapplication.h>
#include "bsettop.h"
#include "bstd.h"
#include "mprogressbar.h"
BDBG_MODULE(brutus_info_playbackbuffer);

PlaybackBufferPanel::PlaybackBufferPanel(InfoPanel *infoPanel, const MRect &rect) :
	InfoPanelPage(infoPanel, rect), _timer(infoPanel->fb()->app())
{
	const char *label[4] = {"Video FIFO", "Audio 0 FIFO", "Audio 1 FIFO", "Playback"};
    MLabel * progresstitle[4];
	for (int i=0;i<4;i++) {
#define BUFFERBAR_SPACING 45
#define BUFFERBAR_HEIGHT 20
        progresstitle[i] = new MLabel(this, MRect(0,i*BUFFERBAR_SPACING,width(),20), label[i]);
		_depths[i] = new MLabel(this, MRect(width()-100,i*BUFFERBAR_SPACING,50,20));
		_sizes[i] = new MLabel(this, MRect(width()-50,i*BUFFERBAR_SPACING,50,20));
		_bar[i] = new MProgressBar(this, MRect(0,i*BUFFERBAR_SPACING+20,width(),BUFFERBAR_HEIGHT), label[i]);
	}

	int w = width()/2;
	int y = 200;
    MLabel * l1 = NULL;
    MLabel * l2 = NULL;
    MLabel * l3 = NULL;
    MLabel * l4 = NULL;
    MLabel * l5 = NULL;
    l1 = new MLabel(this, MRect(0,y,140,25), "TSM Monitor");
    l2 = new MLabel(this, MRect(w+70,y,140,25), "PTS-STC");
	y += 25;
    l3 = new MLabel(this, MRect(0,y,80,20), "Video");
	_bar[4] = new MProgressBar(this, MRect(75,y,w-10,20));
	_videoPTS = new MLabel(this, MRect(w+70,y,120,20), "");
	y += 25;
    l4 = new MLabel(this, MRect(0,y,80,20), "Audio 0");
	_bar[5] = new MProgressBar(this, MRect(75,y,w-10,20));
	_primAudioPTS = new MLabel(this, MRect(w+70,y,120,20), "");
	y += 25;
    l5 = new MLabel(this, MRect(0,y,80,20), "Audio 1");
	_bar[6] = new MProgressBar(this, MRect(75,y,w-10,20));
	_secAudioPTS = new MLabel(this, MRect(w+70,y,120,20), "");

    Skin * pSkin = ((InfoPanel *)parent())->control()->getSkin();
    if (pSkin->isValid(XML_SCREEN_TV))
    {
        //get list box attributes from skin
        MString         textColor;

        pSkin->getAttrValue(XML_SCREEN_TV, 
                            XML_ATT_INFO_TEXT_COLOR,         textColor);

        //set text color if necessary
        if (!textColor.isEmpty())
        {
            for (int i=0;i<4;i++) 
            {
                progresstitle[i]->setTextColor(pSkin->getColorValue(textColor.s()));
                _depths[i]->setTextColor(pSkin->getColorValue(textColor.s()));
                _sizes[i]->setTextColor(pSkin->getColorValue(textColor.s()));
                _bar[i]->setTextColor(pSkin->getColorValue(textColor.s()));
            }
               
            l1->setTextColor(pSkin->getColorValue(textColor.s()));
            l2->setTextColor(pSkin->getColorValue(textColor.s()));
            l3->setTextColor(pSkin->getColorValue(textColor.s()));
            l4->setTextColor(pSkin->getColorValue(textColor.s()));
            _videoPTS->setTextColor(pSkin->getColorValue(textColor.s()));
            _primAudioPTS->setTextColor(pSkin->getColorValue(textColor.s()));
            _secAudioPTS->setTextColor(pSkin->getColorValue(textColor.s()));
            _bar[4]->setTextColor(pSkin->getColorValue(textColor.s()));
            _bar[5]->setTextColor(pSkin->getColorValue(textColor.s()));
            _bar[6]->setTextColor(pSkin->getColorValue(textColor.s()));
        }
    }
}

void PlaybackBufferPanel::start() {
	_timer.addHandler(this);
	_timer.start(333, MTimer::eForever);
	startTick = app()->tickCount();
	bytesAtStartTick = 0;
}

void PlaybackBufferPanel::stop() {
	_timer.stop();
}

void PlaybackBufferPanel::onTimeout(MTimer *timer)
{
	update();
}

void PlaybackBufferPanel::update() {
	char buf[30];
	bplayback_status status;
	bstream_t stream;
	bstream_status stream_status;

	DecodeEngine *decengine = control()->decode();
	DecodeEngine::Settings settings;
	if (!decengine) return;
	decengine->getSettings(&settings);

	bdecode_t decode = settings.decode;
	memset(&status, 0, sizeof(status));
#ifdef PVR_SUPPORT
	bplayback_t playback = settings.playEngine?settings.playEngine->handle():NULL;
	if (playback) {
		/* don't fail just because it's stopped */
		bplayback_get_status(playback, &status);
	}
#endif

	/* It's ok if get_status fails. If it only gets some values, that's ok. */
	baudio_decode_status audio0_status, audio1_status;
	bvideo_decode_status video_status;
	
	BKNI_Memset(&video_status, 0, sizeof(video_status));
	BKNI_Memset(&audio0_status, 0, sizeof(video_status));
	BKNI_Memset(&audio1_status, 0, sizeof(video_status));

	baudio_decode_t audio = bdecode_get_audio(decode, B_ID(0));
	if (audio) {
		baudio_decode_get_status(audio, &audio0_status);
	}
	audio = bdecode_get_audio(decode, B_ID(1));
	if (audio) {
		baudio_decode_get_status(audio, &audio1_status);
	}
	bvideo_decode_t video = bdecode_get_video(decode, B_ID(0));
	if (video) {
		bvideo_decode_get_status(video, &video_status);
	}

	stream = decengine->getStream();
	if (!stream || bstream_get_mpeg_parameters(stream, &stream_status)) {
		bstream_mpeg_init(&stream_status.mpeg);
	}

#define SETDEPTH(INDEX,DEPTH,SIZE) \
	 _bar[INDEX]->setDepth(DEPTH,SIZE); \
	if (DEPTH < 1024*1024) \
		sprintf(buf, "%ldK", DEPTH/1024); \
	else \
		sprintf(buf, "%0.1fM", (DEPTH/1024.0)/1024); \
	if (!_depths[INDEX]->text() || strcmp(buf, _depths[INDEX]->text())) \
		_depths[INDEX]->setText(buf); \
	if (SIZE < 1024*1024) \
		sprintf(buf, "/%ldK", SIZE/1024); \
	else \
		sprintf(buf, "/%0.1fM", (SIZE/1024.0)/1024); \
	if (!_sizes[INDEX]->text() || strcmp(buf, _sizes[INDEX]->text())) \
		_sizes[INDEX]->setText(buf)

	SETDEPTH(0,video_status.common.fifo_depth, video_status.common.fifo_size);
	SETDEPTH(1,audio0_status.common.fifo_depth, audio0_status.common.fifo_size);
	SETDEPTH(2,audio1_status.common.fifo_depth, audio1_status.common.fifo_size);
	SETDEPTH(3,status.fifo_depth, status.fifo_size);

	setStcPtsDiff(true/*prim audio*/, true, audio0_status.common.pts-audio0_status.common.stc, IS_DSS(stream_status.mpeg.mpeg_type),
		audio0_status.common.pts_type != bdecode_pts_type_invalid);
	setStcPtsDiff(true/*sec audio*/, false, audio1_status.common.pts-audio1_status.common.stc, IS_DSS(stream_status.mpeg.mpeg_type),
		audio1_status.common.pts_type != bdecode_pts_type_invalid);
	setStcPtsDiff(false/*video*/, false, video_status.common.pts-video_status.common.stc, IS_DSS(stream_status.mpeg.mpeg_type),
		video_status.common.pts_type != bdecode_pts_type_invalid);
}

void PlaybackBufferPanel::setStcPtsDiff(bool is_audio, bool is_primary, int diff, bool is_dss, bool valid)
{
	char buf[30];
	
	if (!valid) {
		sprintf(buf, "N/A");
	}
	else {
		/* convert to milliseconds */
		if (is_dss) {
			diff /= 27000000/1000;
		}
		else {
			diff /= 45000 / 1000;
		}
		_bar[4+(is_audio?(is_primary?1:2):0)]->setDiff(150,diff); // range up to +/- 150 msec
		sprintf(buf, "%d ms", diff);
	}
	(is_audio?(is_primary?_primAudioPTS:_secAudioPTS):_videoPTS)->setText(buf);
}
