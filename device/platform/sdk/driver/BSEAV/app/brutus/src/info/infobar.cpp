/***************************************************************************
 *     Copyright (c) 2003-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: infobar.cpp $
 * $brcm_Revision: 33 $
 * $brcm_Date: 7/9/09 11:53a $
 *
 * Module Description: GUI for pvr state
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/info/infobar.cpp $
 * 
 * 33   7/9/09 11:53a jtna
 * PR56522: Remove multi-decode fast channel change feature. To be
 * replaced with video primer FCC.
 * 
 * 32   9/15/08 12:40p tokushig
 * PR43426: merge to main
 * 
 * BRUTUS_97405_97043/1   7/31/08 5:38p tokushig
 * PR45076: added button scrolling on homescreen and blue skin support.
 * 
 * 31   8/13/08 7:00p shyam
 * PR43426 : Changes for modal window
 * 
 * 30   5/27/08 4:43p jtna
 * PR42200: Multi-decode fast channel change feature
 * 
 * 29   3/17/08 3:18p erickson
 * PR40307: remove old, unused code before adding new
 * 
 * 28   1/29/07 11:18a ssood
 * PR24918: Channel Map Redesign to make its access Thread-safe
 * 
 * 27   1/2/07 8:57a erickson
 * PR26834: 26834
 * 
 * 26   12/1/06 10:29a ssood
 * PR26252: IP STB SW unification w/ Generic Brutus release
 * 
 * 25   10/12/06 7:01p arbisman
 * PR23775: Show IP address and port of IP channel in infobar
 * 
 * 24   9/28/06 7:02p arbisman
 * PR24289: Back-out changes
 * 
 * 22   8/1/06 9:44a erickson
 * PR23130: enable PEP for 7401
 * 
 * 21   7/28/06 4:02p erickson
 * PR23057: only print bitrate and length if length is valid
 * 
 * 20   7/13/06 4:12p ahulse
 * PR16324: use record/playback status of currently recording / playing
 * file if active
 * 
 * 19   6/19/06 3:47p erickson
 * PR19526: added ChannelMapEntry::setType to force initialization of
 * Settop API frontend params structures when type changes
 * 
 * 18   5/31/06 4:20p tokushig
 * PR21891: Merging updated picture viewer into mainline.
 * 
 * SanDiego_Brutus_Skin/4   5/16/06 11:33a tokushig
 * merge from main
 * 
 * 17   4/17/06 5:07p jgarrett
 * PR 20951: Merging skinned UI into mainline
 * 
 * SanDiego_Brutus_Skin/3   2/15/06 10:50a tokushig
 * add bevel style for mlabel to info skin
 * 
 * SanDiego_Brutus_Skin/2   2/14/06 5:28p tokushig
 * add skin capability to info screen banner - color only
 * 
 * SanDiego_Brutus_Skin/1   2/13/06 4:38p tokushig
 * added partial skin ability.  note that this is primarily for cable show
 * demo purposes and will be expanded to be fully skinnable in the
 * future.
 * 
 * 16   12/12/05 1:58p erickson
 * PR17108: remove PEP ui from 7401
 * 
 * 15   10/13/05 5:25p erickson
 * PR17541: moved general purpose code to mediatypes.c
 * 
 * 14   9/8/05 10:01a erickson
 * PR17048: switch individual chip defines to B_HAS_VDC for all 7038 and
 * beyond platforms
 * 
 * 13   9/2/05 3:56p erickson
 * PR16639: refactored b_set_bstream_mpeg so that it can set every field
 * in bstream_mpeg (including mpeg_type)
 * 
 * 12   8/25/05 3:27p erickson
 * PR16819: named Config to Video, and moved BTSC SAP to new Audio panel
 * 
 * 11   8/24/05 5:14p erickson
 * PR16819: initial checkin of audio info panel
 * 
 * 10   8/23/05 12:36p erickson
 * PR16660: ProgramGuide now supports subch
 * 
 * 9   8/12/05 10:43a jjordan
 * PR16669:add IP STB playback from HDD capability
 * 
 * 8   6/29/05 4:21p erickson
 * PR15966: use MAINCH for program guide. this means subchannels will all
 * have the same name, which isn't great, but at least they get a name.
 * 
 * 7   6/14/05 2:15p erickson
 * PR15216: converted to BKNI_Snprintf
 * 
 * 6   4/18/05 3:07p erickson
 * PR14593: make incremental channel change appear on UI immediately
 * 
 * 5   4/9/05 12:22a erickson
 * PR14701: added b_print_chnum
 * 
 * 4   2/23/05 9:32a erickson
 * PR14180: BUFSIZE is used by vxworks header file
 * 
 * 3   2/15/05 12:42p erickson
 * PR11077: simplified infobar autohide timing and added some dbg for
 * future debugging
 * 
 * 2   2/14/05 4:08p erickson
 * PR11077: use tuner() to determine live decode state
 * 
 * 1   2/7/05 8:10p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/25   2/4/05 10:48a erickson
 * PR12728: cleanup for -pedantic warnings and errors
 * 
 * Irvine_BSEAVSW_Devel/24   1/13/05 2:35p erickson
 * PR13821: fixed show/hide of infopanel in certain state
 * 
 * Irvine_BSEAVSW_Devel/23   1/12/05 4:40p erickson
 * PR13097: don't print Ch 0 on info panel
 * 
 * Irvine_BSEAVSW_Devel/22   1/7/05 10:34a erickson
 * PR13256: info panel was being hidden at the wrong time
 * 
 * Irvine_BSEAVSW_Devel/21   1/4/05 9:48a erickson
 * PR13256: removed linefeed from record name
 * 
 * Irvine_BSEAVSW_Devel/20   12/17/04 2:12p erickson
 * PR12290: modified func name
 * 
 * Irvine_BSEAVSW_Devel/19   12/16/04 3:48p erickson
 * PR13256: added info panel overrides for demo flexibility
 * 
 * Irvine_BSEAVSW_Devel/18   12/3/04 3:20p erickson
 * PR13256: when info panel is hidden, stop the timer so that it isn't
 * shown again
 * 
 * Irvine_BSEAVSW_Devel/17   12/3/04 9:38a erickson
 * PR13256: added include file
 * 
 * Irvine_BSEAVSW_Devel/16   11/19/04 3:29p erickson
 * PR13256: added bitrate and allowed panel to remain visible for AVC demo
 * 
 * Irvine_BSEAVSW_Devel/15   10/15/04 10:51a erickson
 * PR13014: infobar rework
 * 
 * Irvine_BSEAVSW_Devel/14   9/16/04 3:38p erickson
 * PR11081: more work on info panel display
 * 
 * Irvine_BSEAVSW_Devel/13   9/13/04 1:54p erickson
 * PR11081: refactored info panel controls to be more minimal, support
 * more options
 * 
 * Irvine_BSEAVSW_Devel/12   5/18/04 2:59p bandrews
 * PR10148: Removed global image and replaced with global string
 * 
 * Irvine_BSEAVSW_Devel/11   4/30/04 12:06p erickson
 * PR10832: rework of brutus to support single AVManager and
 * ChannelManager for system
 * 
 * Irvine_BSEAVSW_Devel/10   4/19/04 2:02p erickson
 * PR10471: made all filepaths configurable using brutus.cfg
 * 
 * Irvine_BSEAVSW_Devel/9   4/9/04 12:17p bandrews
 * PR10469: setActive(true) raises plugin z-order, setActive(false) lowers
 * it
 * 
 * Irvine_BSEAVSW_Devel/8   2/6/04 1:47p erickson
 * PR9107: switch gif's to png's
 * 
 * Irvine_BSEAVSW_Devel/7   1/29/04 2:27p erickson
 * PR9107: merging back from BRUTUS_PR9107 branch for bwin development
 * 
 * Irvine_BSEAVSW_Devel/6   12/16/03 12:25a erickson
 * PR8731: POD ui work
 * 
 * Irvine_BSEAVSW_Devel/5   12/10/03 10:25p erickson
 * PR8813: default to .gif, not .jpg
 * 
 * Irvine_BSEAVSW_Devel/4   12/10/03 1:36p erickson
 * PR8813: info UI rework
 *
 * Irvine_BSEAVSW_Devel/3   12/8/03 3:12p erickson
 * PR8813: still working on getting new info UI smooth
 *
 * Irvine_BSEAVSW_Devel/2   12/8/03 11:30a erickson
 * PR8813: only accept focus on the infobar if there are plugins AND it's
 * visible
 *
 * Irvine_BSEAVSW_Devel/1   12/4/03 9:39a erickson
 * PR8813: complete rework of info UI to support vertical bar
 *
 ****************************************************************************/
#include "infobar.h"
#include "mstyle.h"
#include "mapplication.h"
#include "cfgsettings.h"

#include "info_outputs.h"
#include "info_status.h"
#include "info_video.h"
#include "info_audio.h"
#include "info_misc.h" /* exit button */

#include "video.h"

#include "programguide.h"
#define TVMESSAGE_WIDTH 200
#define TVMESSAGE_HEIGHT 120

#define BTN_HEIGHT 30
#define BTN_VGAP 8
#define BTN_HGAP 8

BDBG_MODULE(infobar);

#define PIXMAPPATH (MString(cfg->get(Config::IMAGES_PATH)) + "/info/")

InfoBarPlugin::InfoBarPlugin(InfoBar *parent, const char *name) :
    MIconRoundedLabel(parent, MRect(0,0,10,10), name)
{
    _infobar = parent;
    _active = false;
    _linkedWidget = NULL;
    setAlignment(MPainter::alCenter);
    setVAlignment(MPainter::valCenter);
    setPanelBackgroundColor(style()->color(MStyle::BUTTON_FACE));

    Skin * pSkin = _infobar->control()->getSkin();
    if (pSkin->isValid(XML_SCREEN_TV))
    {
        //get list box attributes from skin
        MString         strHAlign;
        MString         strVAlign;
        MString         screenColor;
        MString         imgFocus;
        MString         textColor;
        MString         textColorFocus;

        pSkin->getAttrValue(XML_SCREEN_TV, 
                            XML_ATT_ALIGN_H,                 strHAlign);
        pSkin->getAttrValue(XML_SCREEN_TV, 
                            XML_ATT_ALIGN_V,                 strVAlign);
        pSkin->getAttrValue(XML_SCREEN_TV, 
                            XML_ATT_INFO_COLOR,              screenColor);
        pSkin->getAttrValue(XML_SCREEN_TV, 
                            XML_ATT_INFO_IMAGE_FOCUS,        imgFocus);
        pSkin->getAttrValue(XML_SCREEN_TV, 
                            XML_ATT_INFO_TEXT_COLOR,         textColor);
        pSkin->getAttrValue(XML_SCREEN_TV, 
                            XML_ATT_INFO_TEXT_COLOR_FOCUS,   textColorFocus);

        //set horizontal alignment if necessary
        if (!strHAlign.isEmpty())
        {
            MPainter::Alignment align = MPainter::alLeft;

            if (strHAlign == XML_ALIGN_RIGHT)
            {
                align = MPainter::alRight;
            }
            else
            if (strHAlign == XML_ALIGN_CENTER)
            {
                align = MPainter::alCenter;
            }

            setAlignment(align);
        }

        //set vertical alignment if necessary
        if (!strVAlign.isEmpty())
        {
            MPainter::VAlignment align = MPainter::valTop;

            if (strHAlign == XML_ALIGN_BOTTOM)
            {
                align = MPainter::valBottom;
            }
            else
            if (strHAlign == XML_ALIGN_CENTER)
            {
                align = MPainter::valCenter;
            }

            setVAlignment(align);
        }

        //set panel background color if necessary
        if (!screenColor.isEmpty())
        {
            setPanelBackgroundColor(pSkin->getColorValue(screenColor.s()));
        }

        //set focus image if necessary
        if (!imgFocus.isEmpty())
        {
            setImageFocus(pSkin->getImage(imgFocus.s()));
        }

        //set text color if necessary
        if (!textColor.isEmpty())
        {
            setTextColor(pSkin->getColorValue(textColor.s()));
        }

        //set focus text color if necessary
        if (!textColorFocus.isEmpty())
        {
            setTextColorFocus(pSkin->getColorValue(textColorFocus.s()));
        }
    }

    setFocusable(true);

    parent->addPlugin(this);
}

bool InfoBarPlugin::focus()
{
    /* focus on a plugin causes it to activate. But we need to deactivate
    whatever other plugin may have been active */
    _infobar->activate(this);
    return MLabel::focus();
}

void InfoBarPlugin::setActive(bool active)
{
    BDBG_MSG(("%s setActive %d ==> %d", name(), _active, active));
    if (active != _active) {
        _active = active;
        if (_active) {
            setPanelBackgroundColor(style()->color(MStyle::BUTTON_FACE_FOCUSED));
            setTextColor(style()->color(MStyle::TEXT_FOCUSED));
            if (_linkedWidget)
            {
                _linkedWidget->raise();
                _linkedWidget->show();
            }
        }
        else {
            setPanelBackgroundColor(style()->color(MStyle::BUTTON_FACE));
            setTextColor(parent()->textColor());

            if (_linkedWidget)
            {
                _linkedWidget->hide();
                _linkedWidget->lower();
            }
        }

        Skin * pSkin = _infobar->control()->getSkin();
        if (pSkin->isValid(XML_SCREEN_TV))
        {
            //get attributes from skin
            MString         screenColor;
            MString         textColor;
            MString         textColorFocus;

            pSkin->getAttrValue(XML_SCREEN_TV, 
                                XML_ATT_INFO_COLOR,              screenColor);
            pSkin->getAttrValue(XML_SCREEN_TV, 
                                XML_ATT_INFO_TEXT_COLOR,         textColor);
            pSkin->getAttrValue(XML_SCREEN_TV, 
                                XML_ATT_INFO_TEXT_COLOR_FOCUS,   textColorFocus);

            //set panel background color if necessary
            if (!screenColor.isEmpty())
            {
                if (!_active)
                {
                    setPanelBackgroundColor(pSkin->getColorValue(screenColor.s()));
                }
                else
                {
                    setPanelBackgroundColor(
                        pSkin->changeAlpha(pSkin->getColorValue(screenColor.s()), 
                                           0xff));
                }
            }

            //set text color if necessary
            if (!textColor.isEmpty())
            {
                if (_active)
                {
                    setTextColorFocus(pSkin->getColorValue(textColorFocus.s()));
                }
                else
                {
                    setTextColor(pSkin->getColorValue(textColor.s()));
                }
            }
        }
    }
}

/////////////////////////////////////////////////

#include "cfgsettings.h"

InfoBar::InfoBar(Control *control, MWidget *parent, const MRect &rect) :
    MLabel(parent, rect), _timer(parent->fb()->app())
{
    _control = control;
    _lastactive = NULL;
    _timer.addHandler(this);
    _mode = eCompact;
    _autohide = 0;

    _label = new MLabel(this, MRect(5,5,TVMESSAGE_WIDTH-10,TVMESSAGE_HEIGHT-10));
    _label->setWrapMode(NewLineWrap);

    setBackgroundColor(style()->color(MStyle::BUTTON_FACE));

    setBevel(3);
    setBorder(3);

    Skin * pSkin = control->getSkin();
    if (pSkin->isValid(XML_SCREEN_TV))
    {
        //get list box attributes from skin
        MString         bannerColor;
        MString         bannerBevelStyle;
        MString         bevelStyle;
        MString         screenColor;
        MString         textColor;
        uint32_t        bevelWidth             = 0;
        uint32_t        bannerBevelWidth       = 0;

        pSkin->getAttrValue(XML_SCREEN_TV, 
                            XML_ATT_INFO_BANNER_COLOR,       bannerColor);
        pSkin->getAttrValue(XML_SCREEN_TV, 
                            XML_ATT_INFO_BANNER_BEVEL_STYLE, bannerBevelStyle);
        pSkin->getAttrValue(XML_SCREEN_TV, 
                            XML_ATT_INFO_BEVEL_STYLE,        bevelStyle);
        pSkin->getAttrValue(XML_SCREEN_TV, 
                            XML_ATT_INFO_COLOR,              screenColor);
        pSkin->getAttrValue(XML_SCREEN_TV, 
                            XML_ATT_INFO_TEXT_COLOR,         textColor);
        pSkin->getAttrValue(XML_SCREEN_TV, 
                            XML_ATT_INFO_BEVEL_WIDTH,        bevelWidth);
        pSkin->getAttrValue(XML_SCREEN_TV, 
                            XML_ATT_INFO_BANNER_BEVEL_WIDTH, bannerBevelWidth);

        //set background color if necessary
        if (!screenColor.isEmpty())
        {
            setBackgroundColor(pSkin->getColorValue(screenColor.s()));
        }

        //set banner color if necessary
        if (!bannerColor.isEmpty())
        {
            _label->setBackgroundColor(pSkin->getColorValue(bannerColor.s()));
        }

        //set text color if necessary
        if (!textColor.isEmpty())
        {
            _label->setTextColor(pSkin->getColorValue(textColor.s()));
        }

        //set banner bevel if necessary
        if ((bannerBevelWidth > 0) && (!bannerBevelStyle.isEmpty()))
        {
            MPainter::BevelStyle bevelType = MPainter::bsRaised;
            if (bannerBevelStyle == XML_BEVEL_SUNKEN)
            {
                bevelType = MPainter::bsSunken;
            }
            if (bannerBevelStyle == XML_BEVEL_UPDOWN)
            {
                bevelType = MPainter::bsUpDown;
            }

            _label->setBevel(bannerBevelWidth, bevelType);
        }

        //set bevel if necessary
        if ((bevelWidth > 0) && (!bevelStyle.isEmpty()))
        {
            MPainter::BevelStyle bevelType = MPainter::bsRaised;
            if (bevelStyle == XML_BEVEL_SUNKEN)
            {
                bevelType = MPainter::bsSunken;
            }
            if (bevelStyle == XML_BEVEL_UPDOWN)
            {
                bevelType = MPainter::bsUpDown;
            }

            setBevel(bevelWidth, bevelType);
        }
    }

    /* Create the plugins in order of appearance */
    new OutputsInfoBarPlugin(this);
    new StatusInfoBarPlugin(this);
    new VideoInfoBarPlugin(this);
    new AudioInfoBarPlugin(this);
    new ExitInfoBarPlugin(this);
}

void InfoBar::layout()
{
    int w = width() - BTN_HGAP * 2;
    int y = TVMESSAGE_HEIGHT;
    for (InfoBarPlugin *p = _plugins.first();p;p = _plugins.next()) {
        p->setGeometry(MRect(BTN_HGAP,y,w,BTN_HEIGHT));
        y += BTN_HEIGHT + BTN_VGAP;
    }
    setSize(width(), y);
}

void InfoBar::addPlugin(InfoBarPlugin *plugin)
{
    _plugins.add(plugin);
    plugin->show();
    layout();
}

bool InfoBar::focus()
{
    if (visible() && _plugins.total()) {
        if (_plugins.index(_lastactive) != -1)
            return _lastactive->focus();
        else
            return _plugins[0]->focus();
    }
    return false;
}

void InfoBar::hide()
{
    activate(NULL); // deactivate current plugin
    _timer.stop();
    MLabel::hide();
}

void InfoBar::activate(InfoBarPlugin *plugin)
{
    if (_mode != eFull)
        return;
        
    /* deactivate all others then activate this one */
    for (InfoBarPlugin *p = _plugins.first();p;p = _plugins.next())
        if (p != plugin)
            p->setActive(false);
    if (plugin) {
        plugin->setActive(true);
        _lastactive = plugin;
    }
}

void InfoBar::setCurrent(int index)
{
    if (index < 0 || index >= _plugins.total())
        return;
    _plugins[index]->focus();
}

bool InfoBar::keyDown(int ascii, int modifiers, int scancode)
{
    if (_mode == eFull)
        switch (ascii) {
        case MWKEY_UP:
            /* looping around the top */
            if (_plugins.total() && _plugins[0]->hasFocus()) {
                _plugins.last()->focus();
                return true;
            }
            break;
        case MWKEY_DOWN:
            /* looping around the bottom */
            if (_plugins.total() && _plugins.last()->hasFocus()) {
                _plugins.first()->focus();
                return true;
            }
            break;
        }
    return MLabel::keyDown(ascii, modifiers, scancode);
}

void InfoBar::setMode(InfoBar::Mode mode)
{
    _mode = mode;
    if (_mode == eFull) {
        layout(); // dynamically compute height
    }
    else {
        setSize(TVMESSAGE_WIDTH,TVMESSAGE_HEIGHT);
    }
}

#if 0
#define PRINT_MPEG(ptr, mpeg) \
	ptr += sprintf(ptr, "PIDs v=%x,a=%x\n", \
		mpeg.video[0].pid, \
		mpeg.audio[0].pid)
#else
#define PRINT_MPEG(ptr, mpeg) \
	ptr += sprintf(ptr, "%s,%s\n", \
		mpeg.video[0].pid?b_vidxpt_str(mpeg.video[0].format, mpeg.mpeg_type):"no video",\
		mpeg.audio[0].pid?b_audtype_str(mpeg.audio[0].format):"no audio")
#endif

void InfoBar::show()
{
    static char buf[256]; /* TODO: we're hoping for no buffer overrun, but
        this is dangerous */
    char *ptr = buf;
    baudio_volume vol;

    buf[0] = 0;
#ifdef PVR_SUPPORT
    if (_control->isPlaying()) {
        PlayEngine *play = _control->playEngine();
        if (!play) goto settext;
        Video *video = play->video();
        if (!video) goto settext;

#define INFO_BUFSIZE 64
        char bitrate_str[INFO_BUFSIZE];

        /* calculate bitrate in Mbps (megabits per second) if there's a length */
        if ( video->length(_control->playEngine()->handle() )) {
	        int length = video->length( _control->playEngine()->handle() );
            if (length) {
	            int n;
            	float bitrate = (float)video->size() * 8 / (length / 1000) / 1024 / 1024;
	            n = BKNI_Snprintf(bitrate_str, INFO_BUFSIZE, "%0.1f Mbps, ", bitrate);
    	        b_print_time_period(&bitrate_str[n], INFO_BUFSIZE-n, length, B_TIME_PERIOD_FORMAT_COMPACT);
	        }
        }
        else {
            b_print_size(bitrate_str, INFO_BUFSIZE, video->size());
        }

        ptr += sprintf(ptr, "Play: %s\n", (const char *)video->title);
        ptr += sprintf(ptr, "%s\n", bitrate_str);
        PRINT_MPEG(ptr, video->mpeg);
        if (video->mpeg.encryption.type != bencryption_type_none) {
            ptr += sprintf(ptr, "%s PVR encryption\n",
                g_encryptionStr[video->mpeg.encryption.type]);
        }

#ifdef B_HAS_TRANSCODE
        {
            AVStream * pStream = _control->getLiveStream();
            if (pStream && pStream->isTranscoding())
            {
                ptr += sprintf(ptr, "Transcoding\n");
            }
        }
#endif
    }
    else
#endif
    {
        const char *prog;

        int ch = _control->channel();
        ChannelMap::Channel channel;
        if (_control->channelMap()->getChannel(ch, &channel)) 
        {
            sprintf(buf, "Ch %d: Unknown", ch);
            goto settext;
        }

        if (_control->cfg()->get("BRUTUS_INFO1")) {
            ptr += sprintf(ptr, "%s\n", _control->cfg()->get("BRUTUS_INFO1"));
        }
        else {
            int inc_ch, inc_digits;
            _control->getIncrementalChannel(&inc_ch, &inc_digits);
            if (inc_digits) {
                ptr += BKNI_Snprintf(ptr, 4, "%0*d\n", inc_digits, inc_ch);
            }
            else {
                ptr += b_print_chnum(ptr, 20, &channel);
                
                prog = _control->guide()->programName(ch);
                ptr += sprintf(ptr, "%s: %s\n",
                    (_control->currentWindow())?"(PIP)":"",
                    prog?prog:"");
            }
        }

        if (_control->cfg()->get("BRUTUS_INFO2")) {
            ptr += sprintf(ptr, "%s\n", _control->cfg()->get("BRUTUS_INFO2"));
        }
        else {
            ptr += b_print_channel_description(ptr, 128 /* guess */, &channel.entry);
            ptr += sprintf(ptr, "\n");
        }

        /* must handle streamer as separate case */
        if (channel.entry.getType() != btuner_streamer_stream &&
			channel.entry.getType() != btuner_ip_stream &&
            !_control->tuner() && 
            !_control->deferredSetChannelPending()) 
        {
            ptr += sprintf(ptr, "No tuner available\n");
        }

        if (_control->cfg()->get("BRUTUS_INFO3")) {
            ptr += sprintf(ptr, "%s\n", _control->cfg()->get("BRUTUS_INFO3"));
        }
        else {
            if (channel.entry.getType() != btuner_analog_stream && channel.validProgramInfo == true) {
                bstream_mpeg mpeg;
#if B_HAS_TRANSCODE
                AVStream * pStream = _control->getLiveStream();
                if (pStream && pStream->isTranscoding())
                {
                    mpeg.video[0].pid = 0x21 ;
                    mpeg.video[0].format = bvideo_codec_h264 ;
                    mpeg.mpeg_type = bstream_mpeg_type_ts ;
                    mpeg.audio[0].pid = 0 ;
                }
                else
#endif
                b_set_bstream_mpeg(&mpeg, &channel);
                PRINT_MPEG(ptr, mpeg);
            }
        }

        if (channel.entry.pod) {
            ptr += sprintf(ptr, "POD Channel\n");
        }
    }

    bdisplay_get_dac_audio_volume(_control->display()->display, &vol);
    if (vol.muted) {
        ptr += sprintf(ptr, "Mute\n");
    }
    else {
        int level = (vol.left + 100) / 20;
        ptr += sprintf(ptr, "Vol: %d\n", level);
    }

#ifdef B_HAS_TRANSCODE
    {
        AVStream * pStream = _control->getLiveStream();
        if (pStream && pStream->isTranscoding())
        {
            ptr += sprintf(ptr, "Transcoding\n");
        }
    }
#endif

settext:
    _label->setText(buf);
    MLabel::show();

    /* If full info panel up, or sticky set, never autohide */  
    if (_mode == eFull || _control->cfg()->getBool(Config::INFOPANEL_STICKY)) {
        _timer.stop();
        _autohide = 0;
    }
    else {
        /* Otherwise update the panel for a fixed period, then hide */
        _timer.start(1000, MTimer::eForever);
        _autohide = 10;
    }
}

void InfoBar::onTimeout(MTimer *timer) {
    BDBG_MSG(("info autohide %d", _autohide));
    if (_autohide && --_autohide == 0) {
        hide();
        _timer.stop();
    }
    else {
        int save = _autohide;
        show(); // this performs an update of data too, but don't reset autohide
        _autohide = save;
    }
}
