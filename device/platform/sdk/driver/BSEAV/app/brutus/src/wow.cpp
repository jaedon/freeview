/***************************************************************************
 *     Copyright (c) 2005-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: wow.cpp $
 * $brcm_Revision: 11 $
 * $brcm_Date: 3/11/09 5:26p $
 *
 * Module Description: Window-On-Window implementation
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/wow.cpp $
 * 
 * 11   3/11/09 5:26p jgarrett
 * PR 53103: Updating WXGA main screen to 1/4 height to meet RTS
 * 
 * 10   7/16/08 10:08a erickson
 * PR44848: add 1366x768 option
 *
 * 9   3/17/08 3:17p erickson
 * PR40307: remove old, unused code before adding new
 *
 * 8   6/13/06 5:25p jgarrett
 * PR 22057: fixing PIP coordinates for 640x480 VESA
 *
 * 7   5/31/06 4:19p tokushig
 * PR21891: Merging updated picture viewer into mainline.
 *
 * SanDiego_Brutus_Skin/4   5/16/06 11:34a tokushig
 * merge from main
 *
 * 6   4/17/06 5:09p jgarrett
 * PR 20951: Merging skinned UI into mainline
 *
 * SanDiego_Brutus_Skin/3   4/13/06 3:16p tokushig
 * merge from main to SanDiego_Brutus_Skin branch
 *
 * SanDiego_Brutus_Skin/2   2/23/06 2:27p tokushig
 * merge from 97400 prerel label
 *
 * 5   2/23/06 4:14p bandrews
 * PR16055: fixed for non VDC plats on pipSbsDisplaySizes
 *
 * 4   2/21/06 10:41a mphillip
 * PR16055: Handle non-VDC cases as well.
 *
 * SanDiego_Brutus_Skin/1   2/22/06 3:54p tokushig
 * added ability to modify pip window geometry in skin
 *
 * 3   1/19/06 10:29a erickson
 * PR16055: change some setColor(-1) calls to show/hide, which is required
 * to coordinate with other GUI elements on tvscreen
 *
 * 2   1/13/06 1:53p mphillip
 * PR19051: Replaced a pip frame resize call from prior to the sbs/wow
 * rework of pip.
 *
 * 1   12/20/05 5:30p mphillip
 * PR16055: Abstract class and two sample classes to handle PIP and Side
 * By Side in one place
 *
 *
 ****************************************************************************/
#include "genericscreen.h"
#include "control.h"
#include "cfgsettings.h"
#include <mapplication.h>
#include <mpainter.h>
#include <mwidget.h>
#include "wow.h"
#include "tvscreen.h"
#include "control.h"

#include "bstd.h"

BDBG_MODULE(brutus_wow);

/* Base class.  This should never be the direct implementation */
WindowOnWindow::WindowOnWindow()
{
    _changeable = false;
    _input = false;
    _control = NULL;
}

WindowOnWindow::WindowOnWindow(Control *acontrol)
{
    _changeable = false;
    _input = false;
    _control = acontrol;
}

WindowOnWindow::WindowOnWindow(GenericScreen *screen, Control *acontrol)
{
    _changeable = false;
    _input = false;
    _control = acontrol;
}

WindowOnWindow::~WindowOnWindow()
{
}

MRect WindowOnWindow::MainRect(eScreenType type)
{
    BDBG_MSG(("WindowOnWindow::MainRect(%d)",type));
    return MRect(0,0,0,0);
}

MRect WindowOnWindow::MainRect(eScreenType type, bdisplay_settings *display_settings)
{
    BDBG_MSG(("WindowOnWindow::MainRect(%d,%p)",type,display_settings));
    return MRect(0,0,0,0);
}

MRect WindowOnWindow::WowRect(eScreenType type)
{
    BDBG_MSG(("WindowOnWindow::WowRect(%d)",type));
    return MRect(0,0,0,0);
}

MRect WindowOnWindow::WowRect(eScreenType type, bdisplay_settings *display_settings)
{
    BDBG_MSG(("WindowOnWindow::WowRect(%d,%p)",type,display_settings));
    return MRect(0,0,0,0);
}

bresult WindowOnWindow::ShowWow(bool show, bool selected)
{
    BDBG_MSG(("WindowOnWindow::ShowWow(%d,%d)",show,selected));
    return b_ok;
}

void WindowOnWindow::Resized()
{
    BDBG_MSG(("WindowOnWindow::Resized"));
}

bool WindowOnWindow::IsChangeable()
{
    BDBG_MSG(("WindowOnWindow::IsChangeable (returning '%s')", _changeable ? "true" : "false"));
    return _changeable;
}

bool WindowOnWindow::HandlesInput()
{
    BDBG_MSG(("WindowOnWindow::HandlesInput"));
    return _input;
}

void WindowOnWindow::SetControl(Control *acontrol)
{
    BDBG_MSG(("WindowOnWindow::SetControl"));
    _control = acontrol;
}

MWidget *WindowOnWindow::WowWidget()
{
    BDBG_MSG(("WindowOnWindow::WowWidget"));
    return NULL;
}

void WindowOnWindow::ProcessCommand(BrutusCommand cmd)
{
    BDBG_MSG(("WindowOnWindow::ProcessCommand"));
    return;
}

/* Helper class to draw the basic (normal) PIP outline */
class WowPipOutline : public MWidget {
public:
    WowPipOutline(GenericScreen *parent) : MWidget(parent) {
        _color = -1; // invisible
        setBackgroundColor(app()->style()->color(MStyle::TRANSPARENT));
        hide(); /* by default, it is hidden */
    }
    void setColor(int color) {_color = color; repaint();}
    void setColor(bool selected) {setColor(selected?app()->style()->color(MStyle::FOCUS):(int)0xff000000); }

protected:
    void draw(const MRect &cliprect);
    int _color;
};

void WowPipOutline::draw(const MRect &cliprect) {
    if (_color != -1) {
        MPainter ptr(this);
        ptr.drawBevel(MRect(0,0,width(),height()), 4,
            _color, _color, _color, _color, 0, false);
    }
}

struct PipData {
    WowPipOutline *_outline;
};


/*********************************
* Normal PIP decode window sizes
*
* There are 3 dimensions to consider:
*
* outputmode:   480i/p, 1080i, 576i/p
* picture-in-graphics window:   full, large, small
* decode window: main, pip
*
* The best way to find these numbers is to hack brutus_display.c and give the main
* graphics screen some alpha. Then you can see exactly how much decode window is
* overlapping with graphics.
*
* It's good to overlap some because it cuts off video that normally isn't viewable
* anyway.
*
* The PIP scalar can sometimes be different, but if not, use -1's so you don't
* have redundant numbers.
*
* Because graphics are scaled, the decode window for PIP on full screen TV must be
* moved to fit the graphics.
**/
struct SRect {
    int x,y,w,h;
};
struct DisplaySizes {
    SRect ntsc;
    SRect hd1080i;
    SRect pal;
    SRect hd720p;
    SRect vesa640x480;
    SRect hd1366x768p;
};

DisplaySizes mainDisplaySizes[] = {
// screentype = full
    {{0,0,720,480},{0,0,1920,1080},{0,0,720,576},
    {0,0,1280,720},{0,0,640,480},
    {0,0,1366,768}},
// screentype = large
    {{260,115,360,360*3/4},{700,275,940,988*9/16},{260,115,360,420*576/720},
    {480,188,600,648*720/1280},{232,120,306,306*480/580},
    {510,200,628,384}},
// screentype = small
    {{440,40,220,220*3/4},{1180,106,580,610*9/16},{440,40,220,240*576/720},
    {800,76,360,386*720/1280},{392,48,190,190*480/580},
    {852,82,382,230}}
};

DisplaySizes mainSbsDisplaySizes[] = {
// screentype = full
    {{0,120,360,240},{0,270,960,540},{0,144,260,288},
    {0,180,640,360},{0,120,320,240},
    {0,180,640,360}},
// screentype = large
    {{260,115,360,360*3/4},{700,275,940,988*9/16},{260,115,360,420*576/720},
    {480,188,600,648*720/1280},{232,120,306,306*480/580},
    {480,188,600,648*720/1280}},
// screentype = small
    {{440,40,220,220*3/4},{1180,106,580,610*9/16},{440,40,220,240*576/720},
    {800,76,360,386*720/1280},{392,48,190,190*480/580},
    {800,76,360,386*720/1280}}
};

// -1's means pip is the same as main
DisplaySizes pipDisplaySizes[] = {
// screentype = full
    {{370,30,284,214}, {986,64,756,480},{345,42,320,348},
    {660,44,500,320},{328,30,252,214},
    {660,44,500,320}},
// screentype = large
#ifdef LIMITED_PIP_SIZE
/* if limited, "large" is too large. use exactly 1/4 size instead. it looks
a little funny, but it's close. */
    {{260,115,720/2,480/2},{700,275,1920/2,1080/2},{260,115,720/2,576/2},
    {480,188,1280/2,720/2},{232,120,640/2,480/2},
    {480,188,1280/2,720/2}},
#else
{{-1,-1,-1,-1},{-1,-1,-1,-1},{-1,-1,-1,-1},{-1,-1,-1,-1},{-1,-1,-1,-1},{-1,-1,-1,-1}},
#endif
// screentype = small
{{-1,-1,-1,-1},{-1,-1,-1,-1},{-1,-1,-1,-1},{-1,-1,-1,-1},{-1,-1,-1,-1},{-1,-1,-1,-1}}
};

DisplaySizes pipSbsDisplaySizes[] = {
// screentype = full
    {{360,120,360,240},{960,270,960,540},{260,144,260,288},
    {640,180,640,360},{300,120,320,240},
    {640,180,640,360}},
// screentype = large
#ifdef LIMITED_PIP_SIZE
/* if limited, "large" is too large. use exactly 1/4 size instead. it looks
a little funny, but it's close. */
    {{260,115,720/2,480/2},{700,275,1920/2,1080/2},{260,115,720/2,576/2},
    {480,188,1280/2,720/2},{232,120,640/2,480/2},
    {480,188,1280/2,720/2}},
#else
{{-1,-1,-1,-1},{-1,-1,-1,-1},{-1,-1,-1,-1},{-1,-1,-1,-1},{-1,-1,-1,-1},{-1,-1,-1,-1}},
#endif
// screentype = small
{{-1,-1,-1,-1},{-1,-1,-1,-1},{-1,-1,-1,-1},{-1,-1,-1,-1},{-1,-1,-1,-1},{-1,-1,-1,-1}}
};

static
MRect getWindowSize(bool pip, bool sbs, eScreenType screentype, bdisplay_settings *display_settings)
{
    DisplaySizes *d;
    if (sbs) {
        d = !pip ? mainSbsDisplaySizes : pipSbsDisplaySizes;
    } else {
        d = !pip ? mainDisplaySizes : pipDisplaySizes;
    }
    SRect v;
    bvideo_format_settings format_settings;

    bdisplay_get_video_format_settings(display_settings, &format_settings);
    switch (format_settings.height) {
    case 720:
        v = d[screentype].hd720p;
        break;
    case 1080:
        v = d[screentype].hd1080i;
        break;
    case 576:
        v = d[screentype].pal;
        break;
    default: /* 480, 482 */
        if (format_settings.width == 640) {
            v = d[screentype].vesa640x480;
        }
        else
        {
            v = d[screentype].ntsc;
        }
        break;
    case 768:
        v = d[screentype].hd1366x768p;
        break;
    }
    if (pip && v.x == -1)
        return getWindowSize(0, sbs, screentype, display_settings);
    else {
        /* guarantee no odd window dimensions */
        v.w -= v.w % 2;
        v.h -= v.h % 2;
        return MRect(v.x,v.y,v.w,v.h);
    }
}
static
MRect _WowGetBasicPipOutline(bool sbs, bvideo_format_settings *format_settings) {

    BDBG_MSG(("_WowGetBasicPipOutline('%s',%p)[%d]",sbs?"true":"false",format_settings,format_settings->height));

    switch (format_settings->height) {
    case 1080:
    case 720:
    default: //480
        if (sbs) return MRect(322,93,364,244);
        return MRect(330,3,288,216); /* NOTE: This results in a 16x9 PIP because
            we're rendering a 4x3 PIP into a 4x3 framebuffer, then scaling
            the framebuffer to 16x9. The result is a PIP which is 16x9.
            Also, it's important that the graphics size be the same for all
            formats, otherwise HDSD_SINGLE mode breaks. */
    case 576:
        // for some content, there is a little black INSIDE the box, but that
        // is better than having anything OUTSIDE the box.
        if (sbs) return MRect(306,9,322,294);
        return MRect(306,9,322,294);
    }
    return MRect(0,0,0,0);
}

PipWindowOnWindow::PipWindowOnWindow()
{
    BDBG_MSG(("PipWindowOnWindow::PipWindowOnWindow()"));
    _changeable = false;
    _input = false;
    _control = NULL;
    _private = new PipData;
    _private->_outline = new WowPipOutline(control()->screen(Control::eTV));
}

PipWindowOnWindow::PipWindowOnWindow(Control *acontrol)
{
    BDBG_MSG(("PipWindowOnWindow::PipWindowOnWindow(%p)",acontrol));
    _changeable = false;
    _input = false;
    _control = acontrol;
    _private = new PipData;
    _private->_outline = new WowPipOutline(control()->screen(Control::eTV));
}

PipWindowOnWindow::PipWindowOnWindow(GenericScreen *screen, Control *acontrol)
{
    BDBG_MSG(("PipWindowOnWindow::PipWindowOnWindow(%p,%p)",screen,acontrol));
    _changeable = false;
    _input = false;
    _control = acontrol;
    _private = new PipData;
    _private->_outline = new WowPipOutline(screen);
}

PipWindowOnWindow::~PipWindowOnWindow()
{
    if (_private->_outline)
        delete _private->_outline;
}

MRect PipWindowOnWindow::MainRect(eScreenType type)
{
    bdisplay_settings display_settings;
    bvideo_format_settings format_settings;
    BDBG_MSG(("PipWindowOnWindow::MainRect(%d)",type));
    control()->getOutput(&display_settings);
    bdisplay_get_video_format_settings(&display_settings, &format_settings);
    return getWindowSize(false, false, type, &display_settings);
}

MRect PipWindowOnWindow::MainRect(eScreenType type, bdisplay_settings *display_settings)
{
    BDBG_MSG(("PipWindowOnWindow::MainRect(%d,%p)",type,display_settings));
    return getWindowSize(false, false, type, display_settings);
}

MRect PipWindowOnWindow::WowRect(eScreenType type)
{
    bdisplay_settings display_settings;
    bvideo_format_settings format_settings;
    BDBG_MSG(("PipWindowOnWindow::WowRect(%d,%p)",type));
    _control->getOutput(&display_settings);
    bdisplay_get_video_format_settings(&display_settings, &format_settings);
    return getWindowSize(true, false, type, &display_settings);
}

MRect PipWindowOnWindow::WowRect(eScreenType type, bdisplay_settings *display_settings)
{
    BDBG_MSG(("PipWindowOnWindow::WowRect(%d,%p)",type,display_settings));
    return getWindowSize(true, false, type, display_settings);
}

bresult PipWindowOnWindow::ShowWow(bool show, bool selected)
{
    bdisplay_settings display_settings;
    bvideo_format_settings format_settings;
    BDBG_MSG(("PipWindowOnWindow::ShowWow(%d,%d)",show,selected));
    BDBG_MSG((" pipwow: ShowWow(): _private: %p",_private));
    BDBG_MSG((" pipwow: ShowWow(): _private->_outline: %p",_private->_outline));
    BDBG_MSG((" pipwow: ShowWow(): _control: %p",_control));
    BDBG_MSG((" pipwow: ShowWow(): control(): %p",control()));
    _control->getOutput(&display_settings);
    bdisplay_get_video_format_settings(&display_settings, &format_settings);

    MRect rectPIP(0, 0, 0, 0);
    Skin * pSkin = _control->getSkin();
    if (pSkin->isValid(XML_SCREEN_TV))
    {
        rectPIP = pSkin->getPIPWindowSize(XML_SCREEN_TV, NULL);
    }

    if (rectPIP.isNull())
    {
        rectPIP = _WowGetBasicPipOutline(false, &format_settings);
    }

    BDBG_MSG((" pipwow: ShowWow(): rectPIP x:%d y:%d w:%d h:%d",
              rectPIP.x(), rectPIP.y(), rectPIP.width(), rectPIP.height()));
    _private->_outline->setGeometry(rectPIP);
    if (show) {
        /* Display the border, yellow if selected, black otherwise */
        _private->_outline->setColor(selected);
        _private->_outline->show();
    } else {
        /* Hide the border */
        _private->_outline->hide();
    }
    return b_ok;
}

void PipWindowOnWindow::Resized()
{
    bdisplay_settings display_settings;
    bvideo_format_settings format_settings;
    BDBG_MSG(("PipWindowOnWindow::Resized"));
    _control->getOutput(&display_settings);
    bdisplay_get_video_format_settings(&display_settings, &format_settings);

    MRect rectPIP(0, 0, 0, 0);
    Skin * pSkin = _control->getSkin();
    if (pSkin->isValid(XML_SCREEN_TV))
    {
        rectPIP = pSkin->getPIPWindowSize(XML_SCREEN_TV, NULL);
    }

    if (rectPIP.isNull())
    {
        rectPIP = _WowGetBasicPipOutline(false, &format_settings);
    }

    BDBG_MSG((" pipwow: Resized(): rectPIP x:%d y:%d w:%d h:%d",
              rectPIP.x(), rectPIP.y(), rectPIP.width(), rectPIP.height()));
    _private->_outline->setGeometry(rectPIP);
}

MWidget *PipWindowOnWindow::WowWidget()
{
    BDBG_MSG(("PipWindowOnWindow::WowWidget"));
    if (_private && _private->_outline)
        return _private->_outline;
    else
        return NULL;
}

/* Helper class just to display a dash of info in demonstrations */
class WowInfoBox : public MLabel {
public:
    WowInfoBox(GenericScreen *parent, Control *control, int idx, const MRect &rect) : MLabel(parent, rect) {
        _control = control;
        _index = idx;
        setBackgroundColor(style()->color(MStyle::BUTTON_FACE));
        setBevel(3);
        setBorder(3);
        setWrapMode(NewLineWrap);
        MLabel::hide();
    }
    void selectBox(bool selected) { setTextColor(app()->style()->color(MStyle::TEXT,selected)); }
    void resetText();
    void show() { resetText(); MLabel::show(); }

protected:
    Control *_control;
    int _index;
};

void WowInfoBox::resetText() {
    static char buf[256];
    int which_index = _control->areWindowsSwapped() ? 1-_index : _index;

    if (_control->isPlaying(which_index)) {
#ifdef PVR_SUPPORT
        Video *video = NULL;
        PlayEngine *play = _control->playEngine(which_index);
        if (play) {
            video = play->video();
        }
        sprintf(buf,"Playback");
        if (play && video) {
            if (video->length()) {
                float bitrate = (float)video->size() * 8 / (video->length() / 1000) / 1024 / 1024;
                sprintf(buf, "%0.1f Mbps\n%s",bitrate,b_vidxpt_str(video->mpeg.video[0].format, video->mpeg.mpeg_type));
            }
        }
#endif
    }
    else {
#if 1
        if (which_index)
            sprintf(buf,"PIP");
        else
            sprintf(buf,"main");
#else
        sprintf(buf, "%s",b_vidxpt_str(mpeg.video[0].format, mpeg_type));
#endif
    }
    setText(buf);
}

struct SbsData {
    WowPipOutline *_outline;
    WowInfoBox *_mainInfobox;
    WowInfoBox *_wowInfobox;
    bool _toggled;
    bool _fullscreen;
    int _center;
};

#define SBS_MAIN_INFOBOX_RECT MRect(0,355,100,60)
#define SBS_WOW_INFOBOX_RECT MRect(530,355,100,60)
SbsWindowOnWindow::SbsWindowOnWindow()
{
    _changeable = true;
    _input = true;
    _control = NULL;
    _private = new SbsData;
    _private->_outline = new WowPipOutline(control()->screen(Control::eTV));
    _private->_mainInfobox = new WowInfoBox(control()->screen(Control::eTV), NULL, 0, SBS_MAIN_INFOBOX_RECT);
    _private->_wowInfobox = new WowInfoBox(control()->screen(Control::eTV), NULL, 1, SBS_WOW_INFOBOX_RECT);
    _private->_toggled = false;
    _private->_fullscreen = false;
    _private->_center = 0;
}

SbsWindowOnWindow::SbsWindowOnWindow(Control *acontrol)
{
    _changeable = true;
    _input = true;
    _control = acontrol;
    _private = new SbsData;
    _private->_outline = new WowPipOutline(control()->screen(Control::eTV));
    _private->_mainInfobox = new WowInfoBox(control()->screen(Control::eTV), acontrol, 0, SBS_MAIN_INFOBOX_RECT);
    _private->_wowInfobox = new WowInfoBox(control()->screen(Control::eTV), acontrol, 1, SBS_WOW_INFOBOX_RECT);
    _private->_toggled = false;
    _private->_fullscreen = false;
    _private->_center = 0;
}

SbsWindowOnWindow::SbsWindowOnWindow(GenericScreen *screen, Control *acontrol)
{
    _changeable = true;
    _input = true;
    _control = acontrol;
    _private = new SbsData;
    _private->_outline = new WowPipOutline(screen);
    _private->_mainInfobox = new WowInfoBox(screen, acontrol, 0, SBS_MAIN_INFOBOX_RECT);
    _private->_wowInfobox = new WowInfoBox(screen, acontrol, 1, SBS_WOW_INFOBOX_RECT);
    _private->_toggled = false;
    _private->_fullscreen = false;
    _private->_center = 0;
}

SbsWindowOnWindow::~SbsWindowOnWindow()
{
    if (_private->_outline)
        delete _private->_outline;
}

MRect SbsWindowOnWindow::MainRect(eScreenType type)
{
    bdisplay_settings display_settings;
    bvideo_format_settings format_settings;
    BDBG_MSG(("SbsWindowOnWindow::MainRect(%d)",type));
    control()->getOutput(&display_settings);
    bdisplay_get_video_format_settings(&display_settings, &format_settings);
    return MainRect(type, &display_settings);
}

MRect SbsWindowOnWindow::MainRect(eScreenType type, bdisplay_settings *display_settings)
{
    BDBG_MSG(("SbsWindowOnWindow::MainRect(%d,%p)",type,display_settings));
    _private->_mainInfobox->resetText();
    _private->_wowInfobox->resetText();
    if (type == eFullScreenTV && _private->_toggled && _private->_fullscreen) {
        bvideo_format_settings format_settings;
        int sc; /* Scaled Center, allows _private->_center to be resolution independent */
        bdisplay_get_video_format_settings(display_settings, &format_settings);

        /* fullscreen is a simple calculation */
        switch (format_settings.height) {
        case 720:
            sc = _private->_center * 8;
            return MRect(0,0,640+sc,720);
        case 1080:
            sc = _private->_center * 12;
            return MRect(0,0,960+sc,1080);
        case 576:
            sc = (int)(_private->_center * 4.5);
            return MRect(0,0,360+sc,576);
        default: /* 480, 482 */
            if (format_settings.width == 640) {
                sc = _private->_center * 4;
                return MRect(0,0,320+sc,640);
            }
            else
            {
                sc = (int)(_private->_center * 4.5);
                return MRect(0,0,360+sc,480);
            }
            break;
        case 768:
            sc = (int)(_private->_center * 6.4);
            return MRect(0,0,512+sc,768);
        }
    } else if (type == eFullScreenTV && _private->_toggled) {
        bvideo_format_settings format_settings;
        bdisplay_get_video_format_settings(display_settings, &format_settings);
        MRect src = getWindowSize(false, _private->_toggled, type, display_settings);
        int sc, sy, sw, sh; /* Scaled Center, X, and Y */
        /* Figure out the scale */
        switch (format_settings.height) {
        case 720:
            sc = _private->_center * 8;
            sh = src.height() + (int)(_private->_center * 5.333);
            break;
        case 1080:
            sc = _private->_center * 12;
            sh = src.height() + (int)(_private->_center * 6.75);
            break;
        case 576:
            sc = (int)(_private->_center * 4.5);
            sh = src.height() + (int)(_private->_center * 3.6);
            break;
        default: /* 480, 482 */
            if (format_settings.width == 640) {
                sc = _private->_center * 4;
                sh = src.height() + _private->_center * 3;
            }
            else
            {
                sc = (int)(_private->_center * 4.5);
                sh = src.height() + _private->_center * 3;
            }
            break;
        case 768:
            sc = (int)(_private->_center * 6.4);
            sh = src.height() + (int)(_private->_center * 4.8);
            break;
        }
        /* Scale it up or down based on the current center */
        sy = (format_settings.height - sh)/2;
        sw = src.width()+sc;
        return MRect(0,sy,sw,sh);
    } else
        return getWindowSize(false, _private->_toggled, type, display_settings);
}

MRect SbsWindowOnWindow::WowRect(eScreenType type)
{
    bdisplay_settings display_settings;
    bvideo_format_settings format_settings;
    BDBG_MSG(("SbsWindowOnWindow::WowRect(%d,%p)",type));
    control()->getOutput(&display_settings);
    bdisplay_get_video_format_settings(&display_settings, &format_settings);
    return WowRect(type, &display_settings);
}

MRect SbsWindowOnWindow::WowRect(eScreenType type, bdisplay_settings *display_settings)
{
    BDBG_MSG(("SbsWindowOnWindow::WowRect(%d,%p)",type,display_settings));
    _private->_mainInfobox->resetText();
    _private->_wowInfobox->resetText();
    if (type == eFullScreenTV && _private->_toggled && _private->_fullscreen) {
        bvideo_format_settings format_settings;
        int sc; /* Scaled Center, allows _private->_center to be resolution independent */
        bdisplay_get_video_format_settings(display_settings, &format_settings);
        switch (format_settings.height) {
        case 720:
            sc = _private->_center * 8;
            return MRect(640+sc,0,640-sc,720);
        case 1080:
            sc = _private->_center * 12;
            return MRect(960+sc,0,960-sc,1080);
        case 576:
            sc = (int)(_private->_center * 4.5);
            return MRect(360+sc,0,360-sc,576);
        default: /* 480, 482 */
            if (format_settings.width == 640) {
                sc = _private->_center * 4;
                return MRect(320+sc,0,320-sc,640);
            }
            else
            {
                sc = (int)(_private->_center * 4.5);
                return MRect(360+sc,0,360-sc,480);
            }
            break;
        case 768:
            sc = (int)(_private->_center * 6.4);
            return MRect(512+sc,0,512-sc,768);
        }
    } else if (type == eFullScreenTV && _private->_toggled) {
        bvideo_format_settings format_settings;
        bdisplay_get_video_format_settings(display_settings, &format_settings);
        MRect src = getWindowSize(false, _private->_toggled, type, display_settings);
        int sc, sx, sy, sw, sh; /* Scaled Center, X, Y, Width, and Height */
        /* Figure out the scale */
        switch (format_settings.height) {
        case 720:
            sc = _private->_center * 8;
            sh = src.height() - (int)(_private->_center * 5.333);
            break;
        case 1080:
            sc = _private->_center * 12;
            sh = src.height() - (int)(_private->_center * 6.75);
            break;
        case 576:
            sc = (int)(_private->_center * 4.5);
            sh = src.height() - (int)(_private->_center * 3.6);
            break;
        default: /* 480, 482 */
            if (format_settings.width == 640) {
                sc = _private->_center * 4;
                sh = src.height() - _private->_center * 3;
            }
            else
            {
                sc = (int)(_private->_center * 4.5);
                sh = src.height() - _private->_center * 3;
            }
            break;
        case 768:
            sc = (int)(_private->_center * 6.4);
            sh = src.height() - (int)(_private->_center * 4.8);
            break;
        }
        /* Scale it up or down based on the current center */
        sx = (format_settings.width)/2 + sc;
        sy = (format_settings.height - sh)/2;
        sw = src.width()-sc;
        return MRect(sx,sy,sw,sh);
    } else
        return getWindowSize(true, _private->_toggled, type, display_settings);
}

bresult SbsWindowOnWindow::ShowWow(bool show, bool selected)
{
    BDBG_MSG(("SbsWindowOnWindow::ShowWow(%d,%d)",show,selected));
    if (show) {
        /* Display the border, yellow if selected, black otherwise */
        //_private->_outline->setGeometry(_WowGetBasicPipOutline(true));
        //_private->_outline->setColor(selected);
        BDBG_MSG(("_toggled = true"));
        _private->_toggled = true;
        if (selected) {
            _private->_mainInfobox->selectBox(false);
            _private->_wowInfobox->selectBox(true);
        } else {
            _private->_mainInfobox->selectBox(true);
            _private->_wowInfobox->selectBox(false);
        }
        _private->_mainInfobox->show();
        _private->_wowInfobox->show();
    } else {
        /* Hide the border */
        //_private->_outline->setGeometry(_WowGetBasicPipOutline(false));
        //_private->_outline->setColor(-1);
        BDBG_MSG(("_toggled = false"));
        _private->_toggled = false;
        _private->_mainInfobox->hide();
        _private->_wowInfobox->hide();
        _private->_mainInfobox->resetText();
        _private->_wowInfobox->resetText();
    }
    return b_ok;
}

MWidget *SbsWindowOnWindow::WowWidget()
{
    BDBG_MSG(("SbsWindowOnWindow::WowWidget"));
    if (_private && _private->_outline)
        return _private->_outline;
    else
        return NULL;
}

void SbsWindowOnWindow::ProcessCommand(BrutusCommand cmd)
{
    BDBG_MSG(("SbsWindowOnWindow::ProcessCommand"));
    _private->_mainInfobox->resetText();
    _private->_wowInfobox->resetText();
    switch(cmd) {
    case IND_SELECT:
        BDBG_MSG(("  SBS toggling fullscreen"));
        _private->_fullscreen = !_private->_fullscreen;
        break;
    case IND_LEFT:
        BDBG_MSG(("  SBS shifting left"));
        _private->_center--;
        break;
    case IND_RIGHT:
        BDBG_MSG(("  SBS shifting right"));
        _private->_center++;
        break;
    default:
        break;
    }
    return;
}

