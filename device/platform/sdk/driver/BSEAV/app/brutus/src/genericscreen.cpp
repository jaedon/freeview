/***************************************************************************
 *     Copyright (c) 2002-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: genericscreen.cpp $
 * $brcm_Revision: 14 $
 * $brcm_Date: 3/29/10 10:46a $
 *
 * Module Description: GUI for pvr state
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/genericscreen.cpp $
 * 
 * 14   3/29/10 10:46a erickson
 * SW7405-3625: add TVBEVEL_TEXT_DISABLED option
 *
 * 13   2/8/07 5:18p erickson
 * PR22660: use initialized memory
 *
 * 12   9/28/06 7:00p arbisman
 * PR24289: Back-out changes
 *
 * 10   5/31/06 4:20p tokushig
 * PR21891: Merging updated picture viewer into mainline.
 *
 * SanDiego_Brutus_Skin/5   5/16/06 11:33a tokushig
 * merge from main
 *
 * 9   4/17/06 5:06p jgarrett
 * PR 20951: Merging skinned UI into mainline
 *
 * SanDiego_Brutus_Skin/4   2/23/06 2:27p tokushig
 * merge from 97400 prerel label
 *
 * 8   2/14/06 11:13a erickson
 * PR18018: added bstd.h
 *
 * SanDiego_Brutus_Skin/3   2/3/06 4:43p tokushig
 * add dropshadow to tvwindow font
 *
 * SanDiego_Brutus_Skin/2   1/26/06 10:32a tokushig
 * set default action for tv window to go to full screen tv when using
 * skin
 *
 * SanDiego_Brutus_Skin/1   1/3/06 10:25a tokushig
 * Added XML based skin capability to brutus.  "-skin <name>" command line
 * option
 * allows user to specify skin on startup.  Note that running brutus
 * without the
 * -skin option will use the default GUI.
 *
 * 7   8/12/05 10:41a jjordan
 * PR16669: add IP STB playback from HDD capability
 *
 * 6   6/14/05 2:14p erickson
 * PR15216: converted to BKNI_Snprintf
 *
 * 5   4/18/05 3:06p erickson
 * PR14593: make incremental channel change appear on UI immediately
 *
 * 4   4/9/05 12:17a erickson
 * PR14701: added b_print_chnum
 *
 * 3   2/16/05 9:14a erickson
 * PR13387: fixed background color test
 *
 * 2   2/15/05 5:22p erickson
 * PR13387: added primitive palettized color support based on MStyle's
 * palette trick
 *
 * 1   2/7/05 8:07p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/16   1/12/05 4:40p erickson
 * PR13097: don't print "Ch 0" in PIG
 *
 * Irvine_BSEAVSW_Devel/15   1/7/05 10:41a erickson
 * PR11077: added BACKGROUND_IMAGE support
 *
 * Irvine_BSEAVSW_Devel/14   12/3/04 3:19p erickson
 * PR13256: moved pip gfx size logic to tvscreen
 *
 * Irvine_BSEAVSW_Devel/13   7/20/04 10:38a erickson
 * PR11682: updated tvscreen ui
 *
 * Irvine_BSEAVSW_Devel/12   3/19/04 5:16p erickson
 * PR10166: while debugging fill issues, I simplified the code to use
 * MPainter instead of a custom bevel operation
 *
 * Irvine_BSEAVSW_Devel/11   2/23/04 4:25p bandrews
 * PR9723: Fixed call to drawRoundedBevel with 32-bit solid black color
 * constant.
 *
 * Irvine_BSEAVSW_Devel/10   2/19/04 1:43p erickson
 * PR9684: added name to GenericScreen for debugging
 *
 * Irvine_BSEAVSW_Devel/9   2/13/04 4:07p erickson
 * PR8850: don't resize decode window to NOTV dimensions
 *
 * Irvine_BSEAVSW_Devel/8   1/29/04 2:26p erickson
 * PR9107: merging back from BRUTUS_PR9107 branch for bwin development
 *
 * Irvine_BSEAVSW_Devel/7   10/22/03 11:22a erickson
 * settop api pass-through type conversion
 *
 * Irvine_BSEAVSW_Devel/6   9/17/03 12:31p erickson
 * converted from original debug interface to magnum debug interface
 *
 * Irvine_BSEAVSW_Devel/5   8/19/03 11:55a erickson
 * conversion to settop api in progress
 *
 * Irvine_BSEAVSW_Devel/4   8/18/03 11:03a erickson
 * initial conversion of brutus to settop api, still in progress
 *
 * Irvine_BSEAVSW_Devel/3   8/13/03 10:43a erickson
 * fixed default key processing
 *
 * Irvine_BSEAVSW_Devel/2   7/23/03 4:00p erickson
 * renamed from pvrgui to brutus
 *
 * Irvine_BSEAVSW_Devel/3   4/16/03 12:35p erickson
 * implemented font-string support
 *
 * Irvine_BSEAVSW_Devel/2   4/15/03 12:39p erickson
 * 7115 work, added more build conditionals
 *
 * Irvine_BSEAVSW_Devel/1   4/9/03 11:20a erickson
 * rework of pvrgui files and class names
 *
 ****************************************************************************/
#include "bstd.h"
#include "genericscreen.h"
#include "cfgsettings.h"
#include "control.h"
#include "inputengine.h"
#include "mpainter.h"
#include "mapplication.h"
#include "bstd.h"
BDBG_MODULE(brutus_genericscreen);

TVBevel::TVBevel(GenericScreen *parent, Config *cfg, const MRect &rect) :
    MPushButton(parent, rect, "", "TVBevel")
{
    _cfg = cfg;
    if (!x() && !y())
        setFocusable(false);
    setFont(MFont(app(), _cfg->get(Config::TVSCREEN_MESSAGE_FONT)));
}

void TVBevel::draw(const MRect &cliprect) {
    int tvbevel = _cfg->getInt(Config::TVBEVEL_WIDTH);
    MPainter ptr(this, cliprect);
    unsigned long backgroundColor = app()->style()->color(MStyle::BACKGROUND);

    ptr.setBrush(app()->style()->color(MStyle::TRANSPARENT));
    ptr.fillRect(clientRect());
    if (backgroundColor < 256) {
        /* this is almost certainly a palettized surface, so do it differently */
    }
    else {
        uint32_t colorBevel = _cfg->getInt(Config::TVBEVEL_COLOR);

        if (!_cfg->get(Config::TVBEVEL_COLOR))
        {
            //if no bevel color specified, just use background color
            colorBevel = _cfg->getInt(Config::BACKGROUND_COLOR);
        }

        ptr.drawRoundedBevel(clientRect(), tvbevel, false, colorBevel, 0xFF000000);
    }

    if (hasFocus())
        ptr.drawFocus(clientRect());

    if (!_cfg->get(Config::TVBEVEL_TEXT_DISABLED)) {
        Control *control = ((GenericScreen*)parent())->control();
        char buf[20];
        if (control->isPlaying())
#ifdef B_HAS_IP
            // avoid incorrect "play" string shown in PIG during decode of IP channels
            buf[0] = 0;
#else
            sprintf(buf, "Play");
#endif
        else {
            /* If there are no channels, it's better to print nothing in the PIG */
            buf[0] = 0;
            ChannelMap *map = control->channelManager()->map();
            if (map->totalChannels()) {
                int inc_ch, inc_digits;
                control->getIncrementalChannel(&inc_ch, &inc_digits);
                if (inc_digits) {
                    BKNI_Snprintf(buf, 4, "%0*d\n", inc_digits, inc_ch);
                }
                else {
                    ChannelMap::Channel channel;
                    if (!map->getChannel(control->channel(), &channel)) {
                        b_print_chnum(buf, 20, &channel);
                    }
                }
            }
        }
        ptr.setFont(currentFont());

        if (_cfg->getBool(Config::TVSCREEN_MESSAGE_FONT_DROPSHADOW))
        {
            ptr.setPen(0xff000000);
            ptr.drawText(tvbevel+4+1,tvbevel+1, buf);
        }

        ptr.setPen(textColor());
        ptr.drawText(tvbevel+4,tvbevel, buf);
    }
}

//////////////////////////////////////////

GenericScreen::GenericScreen(Control *acontrol, const MRect &tvrect, const char *name) :
    MWidget(acontrol->fb(), name), _tvbevel(this, acontrol->cfg(), tvrect)
{
    _control = acontrol;
    _tvbevel.addHandler(this);

    init(false);
}

GenericScreen::GenericScreen(Control *acontrol, const char *name) :
    MWidget(acontrol->fb(), name), _tvbevel(this, acontrol->cfg(), NOTV)
{
    _control = acontrol;
    _tvbevel.hide();

    init(true); /* use color key for full-screen TV */
}

void GenericScreen::init(bool useColorKey)
{
    if (useColorKey) {
        setBackgroundColor(app()->style()->color(MStyle::TRANSPARENT));
    }
    else {
        if (cfg()->get(Config::BACKGROUND_IMAGE)) {
            static MImage *bg = NULL;
            if (!bg)
            {
                bg = new MImage(app(), cfg()->get(Config::BACKGROUND_IMAGE));
            }

            setBackgroundImage(bg);
        }
        else {
            setBackgroundColor(app()->style()->color(MStyle::BACKGROUND));
        }
    }
    setGeometry(MRect(0,0,control()->fb()->screenWidth(),control()->fb()->screenHeight()));
    setFocusable(true);
    _lastFocus = NULL;

    if (control()->getSkin()->isValid(NULL))
    {
        _tvbevel.setAction(eActionShowInfoWin);
    }
}

void GenericScreen::onClick(MButton *self) {
    if (self == &_tvbevel) {
        control()->showScreen(Control::eTV);
    }
}

/**
* remembering focus is a VERY VERY dangerous thing. If you have a screen
* that creates and deletes widgets, you need to either CLEAR _lastFocus, or
* override rememberFocus() so that it doesn't get called. Certain death is
* the alternative.
**/
void GenericScreen::rememberFocus() {
    MWidget *fw = app()->focusedWidget();
    if (fw && fw->hasParent(this)) {
        _lastFocus = fw;
    }
    else
        _lastFocus = NULL;
}

bool GenericScreen::focus() {
    if (_lastFocus && _lastFocus->focus())
        return true;
    else if (focusUpperLeft())
        return true;
    else
        return MWidget::focus();
}

void GenericScreen::repaintTVBevel() {
    if (visible())
        _tvbevel.repaint();
}

Config *GenericScreen::cfg() const {return _control->cfg();}

bool GenericScreen::hasTV()
{
    return _tvbevel.width() > 1;
}
