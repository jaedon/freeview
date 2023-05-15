/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: homescreen.cpp $
 * $brcm_Revision: 34 $
 * $brcm_Date: 3/21/12 1:53p $
 *
 * Module Description: Collection of screens for pvrgui demo app
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/homescreen.cpp $
 * 
 * 34   3/21/12 1:53p erickson
 * SW7425-2664: remove old code
 * 
 * 33   4/6/11 2:30p gmohile
 * SW7420-1792 : Add power standby support
 * 
 * 32   8/21/10 3:12p gskerl
 * SW7125-237: Reverted back version 30 to re-enable passive standby on UI
 * 
 * 
 * 31   8/6/10 5:21p gskerl
 * SW7125-237: Temporarily disable Passive Standby on the 7125 until it is
 * completed
 * 
 * 30   3/26/10 5:22p erickson
 * SW7405-3625: refactor constructor params for ButtonListBox to allow for
 * more generic use
 *
 * 29   12/11/09 6:12p mphillip
 * SW7550-112: Merge 7550 changes to main branch
 *
 * Refsw_7550/1   11/18/09 4:27p chengli
 * SW7550-64 : enable channel map screen even B_HAS_IP is not defined
 *
 * 28   7/8/09 6:06p katrep
 * PR56373: fixed brutus shutdown segfault
 *
 * 27   7/8/09 11:43a erickson
 * PR56373: fix mem leak
 *
 * 26   7/2/09 5:47p erickson
 * PR56373: add standby button
 *
 * 25   5/26/09 6:45p mward
 * PR53373: Second Brutus does not have HDMI.
 *
 * 24   5/11/09 7:41p vle
 * PR53373: Add HDMI 1.3a support, deep color, to Brutus/nexus
 *
 * PR53373_DeepColor/1   4/16/09 3:57p vle
 * PR53373: Add HDMI 1.3a support, deep color, to Brutus/nexus
 *
 * 23   2/23/09 6:13p vle
 * PR 48785: Add HDMI CEC screen to BRUTUS
 * Merge to main branch
 *
 * HDMI_TX_Plugfest11_200811/1   11/7/08 4:56p vle
 * PR48785: Add HDMI CEC screen to BRUTUS demo app
 *
 * 22   10/8/08 11:53a tokushig
 * PR43426: merge to main
 *
 * BRUTUS_97405_97043/9   10/8/08 10:49a tokushig
 * PR43426: move global _buttonActionList to a class attribute for
 * homescreen and videoscreen.  this will allow the code to work properly
 * when 2 instances of brutus exist as well as avoid a global constructor
 * issue in vxworks.
 *
 * BRUTUS_97405_97043/8   10/6/08 3:28p tokushig
 * initial check in for brutus transcode record using filemode
 *
 * 21   10/6/08 9:48a tokushig
 * PR45076: fix problem with button creation in homescreen and videoscreen
 *
 * 20   9/23/08 3:35p tokushig
 * PR47233: updated implementation to leverage inheiritance with
 * genericscreen class
 *
 * 19   9/23/08 3:24p tokushig
 * PR47233: pressing select on homescreen's decimated video window will
 * navigate to fullscreen tv.  this was inadvertantly removed when
 * scrolling button box was added.
 *
 * 18   9/15/08 12:40p tokushig
 * PR43426: merge to main
 *
 * BRUTUS_97405_97043/7   8/28/08 10:18p tokushig
 * PR43426: fix incorrect local variable initialization
 *
 * BRUTUS_97405_97043/6   8/21/08 4:06p tokushig
 * PR43426: added scrolling button listboxes to both homescreen and
 * videoscreen.  separated buttonlistbox class to common file for reuse
 * (buttonlistbox.cpp/h)
 *
 * BRUTUS_97405_97043/5   8/15/08 12:27p tokushig
 * PR43426: remove debug printouts and dead code
 *
 * BRUTUS_97405_97043/4   8/6/08 2:08p tokushig
 * PR43426: fix problem where non-skin brutus button presses failed
 *
 * 17   8/20/08 5:48p lwhite
 * PR42739: replaced B_HAS_NETACCEL with B_HAS_IP
 *
 * 16   7/30/08 8:03p shyam
 * PR43426 : Add dynamic toggling between transcode and decode
 *
 * 15   6/25/08 6:08p shyam
 * PR 43426 : Make room for trenscode button
 *
 * 14   6/25/08 12:54p shyam
 * PR43426 : Add 7043 Transcode support
 *
 * 13   5/7/08 11:51a erickson
 * PR42329: fix minor BKNI memory leaks
 *
 * 12   8/10/07 1:22p ssood
 * PR33786: Net DMA & IP Streamer refactoring: replace STREAMER_SUPPORT
 * variable by NETACCEL_SUPPORT
 *
 * 11   5/1/07 5:02p jrubio
 * PR30010: reuse the drm-nd skin+button
 *
 * 10   3/14/07 10:15a erickson
 * PR26267: refactor Brutus into separate VideoList and PlaybackScreen
 *
 * 9   3/9/07 2:12p piyushg
 * PR28588: Add DRM-NDR support in brutus
 *
 * 8   5/31/06 4:20p tokushig
 * PR21891: Merging updated picture viewer into mainline.
 *
 * SanDiego_Brutus_Skin/8   5/16/06 11:33a tokushig
 * merge from main
 *
 * 7   4/17/06 5:06p jgarrett
 * PR 20951: Merging skinned UI into mainline
 *
 * SanDiego_Brutus_Skin/7   2/23/06 2:27p tokushig
 * merge from 97400 prerel label
 *
 * 6   2/14/06 11:13a erickson
 * PR18018: added bstd.h
 *
 * 5   1/19/06 3:18p erickson
 * PR17108: but still show info for info button
 *
 * SanDiego_Brutus_Skin/6   2/1/06 6:09p tokushig
 * changed xml screen names to match control class _screenname
 *
 * SanDiego_Brutus_Skin/5   2/1/06 3:21p tokushig
 * added support for dynamic label update for platform name and
 * description
 *
 * SanDiego_Brutus_Skin/4   1/26/06 9:54a tokushig
 * added handling for additional optional skinning actions
 * this will allow the home screen to be expanded with additional
 * functionality
 * in the future (from skin xml file)
 *
 * SanDiego_Brutus_Skin/3   1/19/06 12:48p tokushig
 * update merge from main
 *
 * SanDiego_Brutus_Skin/2   1/11/06 11:19a tokushig
 * do not process skin for home screen unless xml actually contains a home
 * screen tag
 *
 * SanDiego_Brutus_Skin/1   1/3/06 10:25a tokushig
 * Added XML based skin capability to brutus.  "-skin <name>" command line
 * option
 * allows user to specify skin on startup.  Note that running brutus
 * without the
 * -skin option will use the default GUI.
 *
 * 4   1/6/06 4:46p erickson
 * PR17108: clicking on tvbevel shouldn't show info panel
 *
 * 3   12/20/05 6:22a erickson
 * PR18256: reworked HomeScreen to use Config enum and eliminate #defines
 * in the headerfile
 *
 * 2   3/25/05 1:54p erickson
 * PR14593: renamed Pictures button
 *
 * 1   2/7/05 8:07p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/8   11/2/04 3:45p arbisman
 * PR13142: Moved DSG status screen to a separate screen
 *
 * Irvine_BSEAVSW_Devel/7   10/15/04 10:50a erickson
 * PR13014: infobar rework
 *
 * Irvine_BSEAVSW_Devel/6   9/13/04 1:54p erickson
 * PR11081: replaced Outputs screen with Info panel button
 *
 * Irvine_BSEAVSW_Devel/5   9/9/04 1:54p erickson
 * PR12608: convert JPEG_SUPPORT to PICTURES_SUPPORT
 *
 * Irvine_BSEAVSW_Devel/4   1/29/04 2:26p erickson
 * PR9107: merging back from BRUTUS_PR9107 branch for bwin development
 *
 * Irvine_BSEAVSW_Devel/3   9/17/03 12:32p erickson
 * converted from original debug interface to magnum debug interface
 *
 * Irvine_BSEAVSW_Devel/2   7/23/03 4:00p erickson
 * renamed from pvrgui to brutus
 *
 * SanJose_Linux_Devel/87   4/16/03 12:35p erickson
 * implemented font-string support
 *
 * SanJose_Linux_Devel/86   4/15/03 12:39p erickson
 * 7115 work, added more build conditionals
 *
 * SanJose_Linux_Devel/85   4/9/03 11:21a erickson
 * rework of pvrgui files and class names
 ***************************************************************************/
#include "bstd.h"
#include "homescreen.h"
#include "tvscreen.h" // Outputs button goes to info panel
#include "cfgsettings.h"
#include "control.h"
#include "mmessagebox.h"
#include "buttonlistbox.h"

BDBG_MODULE(brutus_homescreen);

/********************************************
*
* HomeScreen
*
********************************************/
HomeScreen::HomeScreen(Control *acontrol, MRect rectTVWin) :
    GenericScreen(acontrol, rectTVWin)
{
    Skin                 * pSkin                = control()->getSkin();
    MRect                  rectListBox(46, 105, 150, 280);
    uint32_t               nBevelListBox        = 2;
    uint32_t               nRows                = 5;
    MPainter::BevelStyle   styleBevel           = MPainter::bsRaised;
    MPainter::Alignment    textAlign            = MPainter::alCenter;
    int32_t                nListTextColor       = -1;
    int32_t                nListTextColorFocus  = -1;
    MString                listFocusImage;
    MString                listScrollbarImage;
    MString                listScrollIndImage;
    uint32_t               listDropShadow       = 0;
    int32_t                iconColumnWidth      = 0;
    bool                   showButtons          = true;
    int32_t                buttonVSpacing       = 10;

    _buttonActionList.clear();

    if (pSkin->isValid(XML_SCREEN_MENU))
    {
        pSkin->createWidgets(XML_SCREEN_MENU, this);

        //update skin based dynamic labels
        //pSkin->processActionLabel(name(), eActionPlatformName);
        pSkin->processActionLabel(name(), eActionPlatformNameLong);
        pSkin->processActionLabel(name(), eActionPlatformDesc);

        //get list box attributes from skin
        MString         listBevelStyle;
        MString         listColumns;
        MString         listTextColor;
        MString         listTextColorFocus;
        MString         listTextAlign;
        uint32_t        listX               = 0;
        uint32_t        listY               = 0;
        uint32_t        listWidth           = 0;
        uint32_t        listHeight          = 0;
        uint32_t        listBevelWidth      = 0;
        uint32_t        listRows            = 0;

        pSkin->getAttrValue(XML_SCREEN_MENU,
                            XML_ATT_GEOM_LIST_X,            listX);
        pSkin->getAttrValue(XML_SCREEN_MENU,
                            XML_ATT_GEOM_LIST_Y,            listY);
        pSkin->getAttrValue(XML_SCREEN_MENU,
                            XML_ATT_GEOM_LIST_H,            listHeight);
        pSkin->getAttrValue(XML_SCREEN_MENU,
                            XML_ATT_GEOM_LIST_W,            listWidth);
        pSkin->getAttrValue(XML_SCREEN_MENU,
                            XML_ATT_BEVEL_LIST_WIDTH,       listBevelWidth);
        pSkin->getAttrValue(XML_SCREEN_MENU,
                            XML_ATT_BEVEL_LIST_STYLE,       listBevelStyle);
        pSkin->getAttrValue(XML_SCREEN_MENU,
                            XML_ATT_LIST_COLUMNS,           listColumns);
        pSkin->getAttrValue(XML_SCREEN_MENU,
                            XML_ATT_LIST_VISIBLE_ROWS,      listRows);
        pSkin->getAttrValue(XML_SCREEN_MENU,
                            XML_ATT_LIST_TEXT_COLOR,        listTextColor);
        pSkin->getAttrValue(XML_SCREEN_MENU,
                            XML_ATT_LIST_TEXT_COLOR_FOCUS,  listTextColorFocus);
        pSkin->getAttrValue(XML_SCREEN_MENU,
                            XML_ATT_LIST_TEXT_ALIGN,        listTextAlign);
        pSkin->getAttrValue(XML_SCREEN_MENU,
                            XML_ATT_LIST_FOCUS_IMAGE,       listFocusImage);
        pSkin->getAttrValue(XML_SCREEN_MENU,
                            XML_ATT_LIST_SCROLLBAR_IMAGE,   listScrollbarImage);
        pSkin->getAttrValue(XML_SCREEN_MENU,
                            XML_ATT_LIST_SCROLLIND_IMAGE,   listScrollIndImage);
        pSkin->getAttrValue(XML_SCREEN_MENU,
                            XML_ATT_LIST_DROP_SHADOW,       listDropShadow);

        rectListBox.setX(listX);
        rectListBox.setY(listY);

        if (listHeight > 0)
        {
            rectListBox.setHeight(listHeight);
        }

        if (listWidth > 0)
        {
            rectListBox.setWidth(listWidth);
        }

        nBevelListBox = listBevelWidth;

        if (listBevelStyle == XML_BEVEL_RAISED)
        {
            styleBevel = MPainter::bsRaised;
        }
        else
        if (listBevelStyle == XML_BEVEL_SUNKEN)
        {
            styleBevel = MPainter::bsSunken;
        }
        else
        if (listBevelStyle == XML_BEVEL_UPDOWN)
        {
            styleBevel = MPainter::bsUpDown;
        }

        if (listTextAlign == XML_ALIGN_LEFT)
        {
            textAlign = MPainter::alLeft;
        }
        else
        if (listTextAlign == XML_ALIGN_RIGHT)
        {
            textAlign = MPainter::alRight;
        }
        else
        if (listTextAlign == XML_ALIGN_CENTER)
        {
            textAlign = MPainter::alCenter;
        }


        if (listRows > 0)
        {
            nRows = listRows;
        }

        //set text color if necessary
        if (!listTextColor.isEmpty())
        {
            nListTextColor = pSkin->getColorValue(listTextColor.s());
        }

        //set text color when focused if necessary
        if (!listTextColorFocus.isEmpty())
        {
            nListTextColorFocus = pSkin->getColorValue(listTextColorFocus.s());
        }

        iconColumnWidth = 50;
        showButtons     = false;
        buttonVSpacing  = 0;

        _homelogo = NULL;
    }
    else
    {
        MLabel *label;

        _homelogo = new MImage(app(), control()->cfg()->get(Config::HOME_LOGO));
        label = new MLabel(this, MRect(40-38,5,225,110), _homelogo);
        label->setAlignment(MPainter::alCenter);
        label->setVAlignment(MPainter::valCenter);

        label = new MLabel(this, MRect(265-38,25,340,60), control()->cfg()->get(Config::HOME_TITLE));
        label->setAlignment(MPainter::alCenter);
        label->setFont(MFont(app(), control()->cfg()->get(Config::MAINMENU_TITLE_FONT)));
    }

    //required widgets - these widgets must always exist for this screen so
    //we create them here.

    int x = 52;
    int y = 125;
    int yspacing = 48;
    int totalbtns = 0;
    MPushButton *button;

    listbox = new ButtonListBox(this, rectListBox, 15, buttonVSpacing, cfg()->getInt(Config::PVRBAR_PLAYBACK_COLOR), "button_listbox");
    listbox->setTextColor(nListTextColor);
    listbox->setTextColorFocus(nListTextColorFocus);
    listbox->scrollBar()->setButtons(false);
    listbox->scrollBar()->setIndicator(true);
    listbox->setTotalVisible(nRows);
    listbox->captureKeys(true, true);
    listbox->setIndent(10);

    listbox->setFocusImage(pSkin->getImage(listFocusImage.s()));
    listbox->scrollBar()->setImage(pSkin->getImage(listScrollbarImage.s()));
    listbox->scrollBar()->setIndicatorImage(pSkin->getImage(listScrollIndImage.s()));
    listbox->setDropShadow(listDropShadow);

    listbox->addHandler(this);

    listbox->addColumn(iconColumnWidth);   // icon
    listbox->addColumn(120, textAlign);    // name

    ButtonListBox * pButtonListBox = (ButtonListBox *)listbox;
    pButtonListBox->showButtons(showButtons);

#ifdef PVR_SUPPORT
    if (control()->cfg()->getBool(Config::PLAYBACK_ENABLED)) {
        createButton(pButtonListBox, totalbtns, "Playback",
                     pSkin->getImage("imgIconPlayback"), eActionShowPlaybackWin);
        totalbtns++;
    }

    if (control()->cfg()->getBool(Config::RECORD_ENABLED)) {
        createButton(pButtonListBox, totalbtns, "Record",
                     pSkin->getImage("imgIconRecord"), eActionShowRecordWin);
        totalbtns++;
    }
#endif
#ifdef AUDIO_SUPPORT
    if (control()->cfg()->getBool(Config::MP3_ENABLED)) {
        createButton(pButtonListBox, totalbtns, "MP3's",
                     pSkin->getImage("imgIconAudio"), eActionShowAudioWin);
        totalbtns++;
    }
#endif

#ifdef PICTURES_SUPPORT
    if (control()->cfg()->getBool(Config::PICTURES_ENABLED)) {
        createButton(pButtonListBox, totalbtns, "Pictures",
                     pSkin->getImage("imgIconPhotos"), eActionShowPhotoWin);
        totalbtns++;
    }
#endif
#ifdef  B_HAS_DRM_ND
    createButton(pButtonListBox, totalbtns, "Find Server",
                 pSkin->getImage("imgIconExtServer"), eActionShowExtServerWin);
    totalbtns++;
#endif
    createButton(pButtonListBox, totalbtns, "Find Server",
                 pSkin->getImage("imgIconChan"), eActionShowChanWin);
    totalbtns++;
#ifdef B_HAS_TRANSCODE
    createButton(pButtonListBox, totalbtns, "Transcode",
                 pSkin->getImage("imgIconTranscode"), eActionTranscode);
    totalbtns++;
#endif
#ifdef INFOPANEL_SUPPORT
    createButton(pButtonListBox, totalbtns, "Info",
                 pSkin->getImage("imgIconInfo"), eActionShowInfoWin);
    totalbtns++;
#endif

#ifdef HDMI_SUPPORT
    if (!(cfg()->getBool(Config::SECOND_DISPLAY)))
    {
        /* Add HDMI button to home screen */
        createButton(pButtonListBox, totalbtns, "HDMI",
                     pSkin->getImage("imgIconHdmi"), eActionShowHdmiWin);
        totalbtns++;
    }
#endif

#ifdef ADMIN_SUPPORT
    createButton(pButtonListBox, totalbtns, "Admin",
                 pSkin->getImage("imgIconAdmin"), eActionShowAdminWin);
    totalbtns++;
#endif

#if POWERSTANDBY_SUPPORT
    _standbyLogo = new MImage(app(), "images/standby.png");
    _standby = new MPushButton(this, 575, 360, _standbyLogo);
    _standby->addHandler(this);
#else
    _standbyLogo = NULL;
    _standby = NULL;
#endif
}

HomeScreen::~HomeScreen()
{
    if (_homelogo) {
        delete _homelogo;
    }
    if (_standbyLogo) {
        delete _standbyLogo;
    }
}

void HomeScreen::createButton(ButtonListBox * pListBox, int index, const char * name, MImage * icon, eSkinAction action)
{
    MString strTmp(index);
    eSkinAction * pActionTmp = NULL;

    strTmp += "\t";
    strTmp += name;
    pListBox->insert(index, strTmp.s(), icon);
    pActionTmp = new eSkinAction(action);
    _buttonActionList.insert(index, pActionTmp);
}

void HomeScreen::onItemClicked(MListBox *listbox) {
    int currentAction = 0;

    if (_buttonActionList.get(listbox->currentIndex()))
    {
        currentAction = *(_buttonActionList.get(listbox->currentIndex()));
    }

#if B_HAS_TRANSCODE
    if (currentAction == eActionTranscode) {
        if (control()->isPlaying())
        {
            /* playback */
            /* TODO: not sure how to detect if transcoding in playback mode */
        }
        else
        {
            /* live tv */
            AVStream * pStream = control()->getLiveStream();
            if (pStream && pStream->isTranscoding())
            {
                if (MMessageBox::show(fb(),"Stop Transcoding?") == MMessageBox::bYes)
                {
                    control()->transcodeChannel(false);
                }
            }
            else
            {
                if (MMessageBox::show(fb(),"Start Transcoding?") == MMessageBox::bYes)
                {
                    control()->transcodeChannel(true);
                }
            }
        }
    }
    else
#endif
        control()->getSkin()->processAction(NULL, currentAction);
}

void HomeScreen::onClick(MButton *button) {
    if (control()->getSkin()->isValid(XML_SCREEN_MENU))
    {
        //handle skin actions
        control()->getSkin()->processAction(button);
    }
#if POWERSTANDBY_SUPPORT
    else if (button == _standby) {
        control()->standby(brutus_standby_state_ePassiveStandby);
    }
#endif
    else {
        GenericScreen::onClick(button);
    }
}
