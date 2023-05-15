/***************************************************************************
 *     Copyright (c) 2002-2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: infopanel.cpp $
 * $brcm_Revision: 3 $
 * $brcm_Date: 5/31/06 4:19p $
 *
 * Module Description: Collection of screens for pvrgui demo app
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/info/infopanel.cpp $
 * 
 * 3   5/31/06 4:19p tokushig
 * PR21891: Merging updated picture viewer into mainline.
 * 
 * SanDiego_Brutus_Skin/3   5/16/06 11:33a tokushig
 * merge from main
 * 
 * 2   4/17/06 5:06p jgarrett
 * PR 20951: Merging skinned UI into mainline
 * 
 * SanDiego_Brutus_Skin/2   2/15/06 10:50a tokushig
 * add bevel style for mlabel to info skin
 * 
 * SanDiego_Brutus_Skin/1   2/13/06 4:37p tokushig
 * added partial skin ability.  note that this is primarily for cable show
 * demo purposes and will be expanded to be fully skinnable in the
 * future.
 * 
 * 1   2/7/05 8:10p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/20   10/15/04 10:51a erickson
 * PR13014: infobar rework
 * 
 * Irvine_BSEAVSW_Devel/19   9/13/04 1:54p erickson
 * PR11081: refactored info panel controls to be more minimal, support
 * more options
 * 
 * Irvine_BSEAVSW_Devel/18   5/18/04 2:59p bandrews
 * PR10148: Removed global image and replaced with global string
 * 
 * Irvine_BSEAVSW_Devel/17   2/19/04 3:38p bandrews
 * PR9723: Changed background color from 0 to 0xff000000
 * 
 * Irvine_BSEAVSW_Devel/16   2/17/04 2:28p erickson
 * PR9465: sds and vsb on 7038 need more delay, but qam doesn't. so it
 * needs to be dynamic.
 * 
 * Irvine_BSEAVSW_Devel/15   1/29/04 2:26p erickson
 * PR9107: merging back from BRUTUS_PR9107 branch for bwin development
 * 
 * Irvine_BSEAVSW_Devel/14   12/19/03 10:42a erickson
 * PR8991: assert is invalid. I removed it.
 * 
 * Irvine_BSEAVSW_Devel/13   12/10/03 10:27p erickson
 * PR8813: improve default background (black w/ border)
 * 
 * Irvine_BSEAVSW_Devel/12   12/10/03 1:36p erickson
 * PR8813: info UI rework
 * 
 * Irvine_BSEAVSW_Devel/11   12/4/03 9:39a erickson
 * PR8813: complete rework of info UI to support vertical bar
 * 
 * Irvine_BSEAVSW_Devel/10   11/7/03 1:32a erickson
 * PR8582: changed qam ds status structure in order to get FEC corr/uncorr
 * values
 * 
 * Irvine_BSEAVSW_Devel/9   10/10/03 1:43p erickson
 * converted brutus frontend code so that it can support all frontend
 * types simultaneously. this
 * is in preperation for 97038 and others.
 * 
 * Irvine_BSEAVSW_Devel/8   9/17/03 12:32p erickson
 * converted from original debug interface to magnum debug interface
 * 
 * Irvine_BSEAVSW_Devel/7   9/9/03 5:30p erickson
 * 7110 dual record/single decode support. this involves added
f * currentDecode which is different
 * from currentSource. This changes the logic a lot. Still in progress,
 * but it basically works.
 *
 * Irvine_BSEAVSW_Devel/6   9/8/03 5:21p erickson
 * support no PVR_SUPPORT case, and don't start panel timers when not
 * visible
 * 
 * Irvine_BSEAVSW_Devel/5   8/21/03 9:43a erickson
 * venom2 settop api work
 * 
 * Irvine_BSEAVSW_Devel/4   8/19/03 11:55a erickson
 * conversion to settop api in progress
 *
 * Irvine_BSEAVSW_Devel/3   8/18/03 11:03a erickson
 * initial conversion of brutus to settop api, still in progress
 *
 * Irvine_BSEAVSW_Devel/2   7/23/03 4:00p erickson
 * renamed from pvrgui to brutus
 *
 * SanJose_Linux_Devel/35   4/28/03 11:49a erickson
 * timer handler should only be added once
 *
 * SanJose_Linux_Devel/34   4/16/03 4:00p erickson
 * added pts/pcr bars
 *
 * SanJose_Linux_Devel/33   4/15/03 4:10p erickson
 * fixed uninitialized variable
 *
 * SanJose_Linux_Devel/32   4/15/03 12:39p erickson
 * 7115 work, added more build conditionals
 *
 * SanJose_Linux_Devel/31   4/10/03 4:22p erickson
 * info panel rework
 * added edit channels and outputs save-state feature
 *
 *************************************************************************/
#include "infopanel.h"
#include "infobar.h" // background
#include "mpainter.h"
#include "mwidget.h"
#include <assert.h>
#include "bstd.h"
#include "skin.h"
BDBG_MODULE(brutus_infopanel);

#define BORDER 4
#define INDENT (BORDER+4)

InfoPanelPage::InfoPanelPage(InfoPanel *parent, const MRect &rect) :
    MLabel(parent, rect)
{
    _infopanel = parent;
}

InfoBarPopup::InfoBarPopup(InfoBar *infoBar, MWidget *parent, const MRect &rect) :
    MLabel(parent, rect)
{
    _infoBar = infoBar;
}

/////////////////////////////////

InfoPanelStatusPage::InfoPanelStatusPage(InfoPanel *parent, const MRect &rect) :
    InfoPanelPage(parent, rect)
{
    setBackgroundColor(0xFF000000);
    totallabels = 0;

    Skin * pSkin = parent->control()->getSkin();
    if (pSkin->isValid(XML_SCREEN_TV))
    {
        setBackgroundColor(0xAA000000);
    }
}

InfoPanelStatusPage::~InfoPanelStatusPage()
{
    clearLabels();
}


void InfoPanelStatusPage::changeLabel(int index, const char *lbl)
{
    for (int i=0;i<totallabels;i++)
        if (labels[i]->tag() == index) {
            if (strcmp(labels[i]->text(), lbl))
                labels[i]->setText(lbl);
        }
}

void InfoPanelStatusPage::clearLabels()
{
    for (int i=0;i<totallabels;i++) {
        if (labels[i]) {
            delete labels[i];
            labels[i] = NULL;
        }
        if (values[i]) {
            delete values[i];
            values[i] = NULL;
        }
    }
    totallabels = 0;
}

void InfoPanelStatusPage::addLabel(int index, const char *lbl)
{
    int h = MFontMetrics(currentFont()).height() - 2; /* squish it slightly */
    int half = width()/2;
    int tl = totallabels;

    assert(tl<MAX_INFOPANELPAGE_LABELS);

#define BORDER 4
    labels[tl] = new MLabel(this, MRect(BORDER,BORDER+tl*h,half-BORDER,h), lbl, "infolabel");
    labels[tl]->setTag(index);
    labels[tl]->show();
    values[tl] = new MLabel(this, MRect(half,BORDER+tl*h,half-BORDER,h), "", lbl);
    values[tl]->show();
    totallabels++;

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
            labels[tl]->setTextColor(pSkin->getColorValue(textColor.s()));
            values[tl]->setTextColor(pSkin->getColorValue(textColor.s()));
        }
    }
}

void InfoPanelStatusPage::setLabelValue(int index, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    for (int i=0;i<totallabels;i++)
        if (labels[i]->tag() == index) {
            char buf[256];
            vsprintf(buf, format, ap);
            if (!values[i]->text() || strcmp(values[i]->text(), buf)) {
                //printf("setLabelValue hit %s\n", buf);
                values[i]->setText(buf);
            }
            break;
        }
    va_end(ap);
}

///////////////////////////////////////

InfoPanel::InfoPanel(InfoBar *infoBar, const MRect &rect) :
    MLabel(infoBar->topLevelWidget(), rect), _timer(infoBar->app())
{
    _infoBar = infoBar;
    setBackgroundColor(style()->color(MStyle::BUTTON_FACE));
    setBevel(3);

#define PAGELABEL_HEIGHT 30
    _pageLabel = new MLabel(this,MRect(0,height()-PAGELABEL_HEIGHT,width(),PAGELABEL_HEIGHT), "infolabel");
    _pageLabel->setAlignment(MPainter::alCenter);

    _currentPage = 0;
    _timer.addHandler(this);

    Skin * pSkin = infoBar->control()->getSkin();
    if (pSkin->isValid(XML_SCREEN_TV))
    {
        //get list box attributes from skin
        MString         screenColor;
        MString         textColor;
        MString         bevelStyle;
        uint32_t        bevelWidth       = 0;

        pSkin->getAttrValue(XML_SCREEN_TV, 
                            XML_ATT_INFO_COLOR,              screenColor);
        pSkin->getAttrValue(XML_SCREEN_TV, 
                            XML_ATT_INFO_TEXT_COLOR,         textColor);
        pSkin->getAttrValue(XML_SCREEN_TV, 
                            XML_ATT_INFO_BEVEL_WIDTH,        bevelWidth);
        pSkin->getAttrValue(XML_SCREEN_TV, 
                            XML_ATT_INFO_BEVEL_STYLE,        bevelStyle);

        //set panel background color if necessary
        if (!screenColor.isEmpty())
        {
            setBackgroundColor(pSkin->getColorValue(screenColor.s()));
        }
 
        //set text color if necessary
        if (!textColor.isEmpty())
        {
            _pageLabel->setTextColor(pSkin->getColorValue(textColor.s()));
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
}

void InfoPanel::onTimeout(MTimer *self) {
    update();
}

void InfoPanel::show() {
    setPage(_currentPage);
    MLabel::show();
}

void InfoPanel::hide() {
    MLabel::hide();
    _timer.stop();
}

void InfoPanel::setPage(int page)
{
    if (page != _currentPage && _currentPage < _pages.total()) {
        _pages[_currentPage]->hide();
    }
    _currentPage = page;
    if (_currentPage < _pages.total())
        _pages[_currentPage]->show();
    _timer.start(_pagetimers[_currentPage], MTimer::eForever);
    updateLabel();
    update();
}

void InfoPanel::updateLabel()
{
#if 0
    for (MButton *b = _tabs.first(); b; b = _tabs.next()) {
    //TODO: change button face? we need a better tab widget anyway
        b->setBackgroundColor(b->tag() == _currentPage ? 0x0000FF : 0xFF0000);
    }
#else
    char buf[30];
    sprintf(buf, "< Page %d of %d >", _currentPage+1, _pages.total());
    _pageLabel->setText(buf);
#endif
}

void InfoPanel::incPage(int inc) {
    if (_currentPage + inc < 0)
        setPage(_pages.total()-1);
    else if (_currentPage + inc >= _pages.total())
        setPage(0);
    else
        setPage(_currentPage + inc);
}

#define TABHEIGHT 30
#define TABWIDTH 100

MRect InfoPanel::infoClientRect() {
    int w = width()-INDENT*2;
    return MRect(INDENT,INDENT,w,w - TABHEIGHT);
}

void InfoPanel::addPage(MWidget *page, const char *name, int msec)
{
#if 0
    MPushButton *b = new MPushButton(this,
        MRect(INDENT+(TABWIDTH+15) * _pages.total(),
            height()-TABHEIGHT-INDENT/2, TABWIDTH, TABHEIGHT), name);
    b->setTag(_pages.total());
    b->addHandler(this);
    _tabs.add(b);
#endif
    _pagetimers[_pages.total()] = msec;
    _pages.add(page);
}

void InfoPanel::setPageTimer(int pageIndex, int msec)
{
    assert(pageIndex >= 0 && pageIndex < _pages.total());
    _pagetimers[pageIndex] = msec;
}

void InfoPanel::onClick(MButton *button)
{
    setPage(button->tag());
}

