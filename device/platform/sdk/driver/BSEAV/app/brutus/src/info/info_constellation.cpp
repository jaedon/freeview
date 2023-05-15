/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: info_constellation.cpp $
 * $brcm_Revision: 11 $
 * $brcm_Date: 9/25/12 5:03p $
 *
 * Module Description: GUI for demod state
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/info/info_constellation.cpp $
 * 
 * 11   9/25/12 5:03p vishk
 * SW7435-372: Make a single pi call by making the requested softdecisions
 * from Brutus to be the same as the number pi returns.
 * 
 * 10   3/17/08 3:18p erickson
 * PR40307: remove old, unused code before adding new
 * 
 * 9   5/31/06 4:20p tokushig
 * PR21891: Merging updated picture viewer into mainline.
 * 
 * SanDiego_Brutus_Skin/2   5/16/06 11:33a tokushig
 * merge from main
 * 
 * 8   4/17/06 5:06p jgarrett
 * PR 20951: Merging skinned UI into mainline
 * 
 * SanDiego_Brutus_Skin/1   2/13/06 4:37p tokushig
 * added partial skin ability.  note that this is primarily for cable show
 * demo purposes and will be expanded to be fully skinnable in the
 * future.
 * 
 * 7   10/3/05 11:18a erickson
 * PR17108: fixed linux 2.6 compilation warnings
 * 
 * 6   9/8/05 10:01a erickson
 * PR17048: switch individual chip defines to B_HAS_VDC for all 7038 and
 * beyond platforms
 * 
 * 5   7/18/05 4:07p erickson
 * PR16138: sometimes there's no tuner
 * 
 * 4   4/8/05 10:37a mphillip
 * PR14593: Removed a doubled line introduced earlier during debugging
 * 
 * 3   3/29/05 12:17p erickson
 * PR14593: 3560 support
 * 
 * 2   3/24/05 1:21p erickson
 * PR14593: removed old platform
 * 
 * 1   2/7/05 8:08p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/23   2/4/05 10:48a erickson
 * PR12728: cleanup for -pedantic warnings and errors
 * 
 * Irvine_BSEAVSW_Devel/22   9/15/04 2:41p erickson
 * PR9683: use explicity _FLAVOR or _FAMILY instead of BCM7### defines
 * 
 * Irvine_BSEAVSW_Devel/21   9/15/04 1:53p erickson
 * PR9683: convert from B_CHIP to BCHP_CHIP in brutus
 * 
 * Irvine_BSEAVSW_Devel/20   2/23/04 4:24p bandrews
 * PR9723: Fixed call to setBrush to use 32-bit color constant for solid
 * black.
 * 
 * Irvine_BSEAVSW_Devel/19   2/19/04 3:32p bandrews
 * PR9723: Implemented ARGB1555 support and tested ARGB8888 support
 * 
 * Irvine_BSEAVSW_Devel/18   2/11/04 12:36p erickson
 * PR9691: need to sync after drawing dots (also, don't draw if not
 * visible)
 * 
 * Irvine_BSEAVSW_Devel/17   1/29/04 2:26p erickson
 * PR9107: merging back from BRUTUS_PR9107 branch for bwin development
 * 
 * Irvine_BSEAVSW_Devel/16   12/17/03 10:47p erickson
 * PR8850: 7038 constellation support
 * 
 * Irvine_BSEAVSW_Devel/15   12/10/03 1:36p erickson
 * PR8813: info UI rework
 * 
 * Irvine_BSEAVSW_Devel/14   12/4/03 9:39a erickson
 * PR8813: complete rework of info UI to support vertical bar
 * 
 * Irvine_BSEAVSW_Devel/13   11/3/03 12:51p erickson
 * normalized constellation for 73xx. Need to test on other platforms.
 * 
 * Irvine_BSEAVSW_Devel/12   9/17/03 12:32p erickson
 * converted from original debug interface to magnum debug interface
 * 
 * Irvine_BSEAVSW_Devel/11   8/27/03 9:20a erickson
 * 73xx support
 * 
 * Irvine_BSEAVSW_Devel/10   8/26/03 11:33a erickson
 * added 7320 support and added ERR msg for bad values
 * 
 * Irvine_BSEAVSW_Devel/9   8/22/03 5:20p erickson
 * venom2 settop api work
 * 
 * Irvine_BSEAVSW_Devel/8   8/21/03 9:43a erickson
 * venom2 settop api work
 *
 * Irvine_BSEAVSW_Devel/7   8/18/03 11:03a erickson
 * initial conversion of brutus to settop api, still in progress
 *
 * Irvine_BSEAVSW_Devel/6   7/17/03 8:54a erickson
 * handler should only be added on init
 *
 * Irvine_BSEAVSW_Devel/5   7/16/03 8:53a erickson
 * 7328 support
 *
 * Irvine_BSEAVSW_Devel/4   6/27/03 10:11a erickson
 * fixed 3125 constellation
 *
 * Irvine_BSEAVSW_Devel/1   4/10/03 4:22p erickson
 * info panel rework
 * added edit channels and outputs save-state feature
 *
 ****************************************************************************/
#include "info_constellation.h"
#include "mapplication.h"
#include <assert.h>
#include "bstd.h"
#include "skin.h"
#include "infopanel.h"
BDBG_MODULE(brutus_info_constellation);

#define BORDER 4


Constellation::Constellation(MWidget *parent, const MRect &rect) :
    MLabel(parent, rect)
{
    _xborder = _yborder = BORDER;
    _borderColor = style()->color(MStyle::BUTTON_FACE);
    _lastrefresh = 0;

    _buffer = NULL;
    _buffersize = 0;
    resizeBuffer();

    Skin * pSkin = ((InfoPanel *)parent)->control()->getSkin();
    if (pSkin->isValid(XML_SCREEN_TV))
    {
        //get list box attributes from skin
        MString         screenColor;
        MString         textColor;

        pSkin->getAttrValue(XML_SCREEN_TV, 
                            XML_ATT_INFO_COLOR,              screenColor);
        pSkin->getAttrValue(XML_SCREEN_TV, 
                            XML_ATT_INFO_TEXT_COLOR,         textColor);

        //set panel background color if necessary
        if (!screenColor.isEmpty())
        {
            _borderColor = pSkin->getColorValue(screenColor.s());
            //setBackgroundColor(pSkin->getColorValue(screenColor.s()));
        }
 
        //set text color if necessary
        if (!textColor.isEmpty())
        {
            setTextColor(pSkin->getColorValue(textColor.s()));
        }
    }
}

Constellation::~Constellation() {
    if (_buffer)
        delete _buffer;
}

void Constellation::setGeometry(const MRect &rect) {
    bool resize = false;
    if (rect.width() != width() || rect.height() != height())
        resize = true; // don't call resizeBuffer until width() and height() are changed
    MLabel::setGeometry(rect);
    if (resize)
        resizeBuffer();
}

void Constellation::resizeBuffer() {
    if (_buffer)
        delete _buffer;
    _buffersize = width()*height();
    _buffer = new char[_buffersize];
}

void Constellation::set(btuner_t t) {
    _tuner = t;
}

void Constellation::draw(const MRect &cliprect) {
    MPainter ptr(this);
    ptr.setBrush(0xFF000000);

    Skin * pSkin = ((InfoPanel *)parent())->control()->getSkin();
    if (pSkin->isValid(XML_SCREEN_TV))
    {
        ptr.setBrush(0xAA000000);
    }

    ptr.fillRect(MRect(BORDER,BORDER,width()-BORDER*2,height()-BORDER*2));
    
    ptr.drawBevel(clientRect(), 4, _borderColor,_borderColor,_borderColor,_borderColor, 2);
    ptr.setPen(_borderColor);
    ptr.drawLine(BORDER,height()/2,width()-BORDER,height()/2);
    ptr.drawLine(BORDER,height()/2+1,width()-BORDER,height()/2+1);
    ptr.drawLine(width()/2,BORDER,width()/2,height()-BORDER);
    ptr.drawLine(width()/2+1,BORDER,width()/2+1,height()-BORDER);

    _lastrefresh = app()->tickCount();

    memset(_buffer, 0, _buffersize);
}

#define TOTALCOLORS 5
static int g_dotColors[TOTALCOLORS] = {
0xFF777700,
0xFF999944,
0xFFBBBB99,
0xFFDDDDbb,
0xFFFFFFFF
};

#define NUMPOINTS 30
#define REFRESH_COUNT (15*1000) // 15 seconds
void Constellation::update() {
    btuner_soft_decision sd[NUMPOINTS];

    /* clear the constellation regularly */
    if (_lastrefresh + REFRESH_COUNT <= (int)app()->tickCount()) {
        repaint();
        return;
    }

    if (!visible())
        return;
        
    if (!_tuner || btuner_get_soft_decisions(_tuner, sd, NUMPOINTS))
        return; /* this will fail for analog channels */

    MPainter ptr(this);
    int w,h;
    /* Hardcoding 256 instead of actual w/h makes constellation more continuous. */
    w = h = 256;
    for (int i=0;i<NUMPOINTS;i++) {
        // find the center
        int x = width()/2;
        int y = height()/2;

        // convert i/q from 32768/-32767 to +/- width/2 and height/2
        x += sd[i].i * w / 65536;
        y += sd[i].q * h / 65536;
        BDBG_MSG(("%d,%d => %d,%d", sd[i].i,sd[i].q,x,y));
        unsigned index = y*width() + x;

        /* prevent the app from crashing if our algorithm isn't quite right */
        if (x >= (int)width() || y >= (int)height() || x < 0 || y < 0) {
            BDBG_ERR(("Invalid constellation value: %d %d", x, y));
            continue;
        }

        assert(index < _buffersize);
        if (_buffer[index] < TOTALCOLORS) {
            ptr.setBrush(g_dotColors[(int)_buffer[index]++]);
            ptr.fillRect(MRect(x,y,2,2));
        }
    }

    /* we're drawing outside of the paint event */
    ptr.sync();
}
