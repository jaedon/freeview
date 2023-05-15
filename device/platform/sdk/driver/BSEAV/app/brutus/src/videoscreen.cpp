/***************************************************************************
 *     Copyright (c) 2002-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: videoscreen.cpp $
 * $brcm_Revision: 15 $
 * $brcm_Date: 3/26/10 5:22p $
 *
 * Module Description: Collection of screens for pvrgui demo app
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/videoscreen.cpp $
 * 
 * 15   3/26/10 5:22p erickson
 * SW7405-3625: refactor constructor params for ButtonListBox to allow for
 * more generic use
 *
 * 14   10/8/08 11:53a tokushig
 * PR43426: merge to main
 *
 * BRUTUS_97405_97043/6   10/8/08 10:50a tokushig
 * PR43426: move global _buttonActionList to a class attribute for
 * homescreen and videoscreen.  this will allow the code to work properly
 * when 2 instances of brutus exist as well as avoid a global constructor
 * issue in vxworks.
 *
 * BRUTUS_97405_97043/5   10/6/08 3:28p tokushig
 * initial check in for brutus transcode record using filemode
 *
 * 13   10/6/08 9:52a tokushig
 * PR45076: fix problem with button creation in homescreen and videoscreen
 *
 * 12   9/29/08 6:00p jtna
 * PR47417: Set focus on Play button if stream delete succeeds
 *
 * 11   9/15/08 12:40p tokushig
 * PR43426: merge to main
 *
 * BRUTUS_97405_97043/4   9/12/08 5:37p tokushig
 * PR43426: partial merge to branch from main
 *
 * BRUTUS_97405_97043/3   8/28/08 10:16p tokushig
 * PR43426: fix incorrect local variable initialization
 *
 * BRUTUS_97405_97043/2   8/21/08 4:06p tokushig
 * PR43426: added scrolling button listboxes to both homescreen and
 * videoscreen.  separated buttonlistbox class to common file for reuse
 * (buttonlistbox.cpp/h)
 *
 * BRUTUS_97405_97043/1   7/31/08 5:38p tokushig
 * PR45076: added button scrolling on homescreen and blue skin support.
 *
 * 10   7/30/08 1:21p shyam
 * PR 45076 : Add trasncode support for playback streams
 *
 * 9   3/14/07 10:16a erickson
 * PR26267: refactor Brutus into separate VideoList and PlaybackScreen
 *
 * 8   7/27/06 5:12p bandrews
 * PR20199: Switched order of start playback and goto fullscreen so that
 * we goto full screen before we start playback --> sync will not unmute
 * too early
 *
 * 7   5/31/06 4:19p tokushig
 * PR21891: Merging updated picture viewer into mainline.
 *
 * SanDiego_Brutus_Skin/3   5/16/06 11:33a tokushig
 * merge from main
 *
 * 6   4/17/06 5:06p jgarrett
 * PR 20951: Merging skinned UI into mainline
 *
 * SanDiego_Brutus_Skin/2   3/10/06 11:47a tokushig
 * moved printf of xml action to Skin::processAction() and converted it to
 * BDBG_MSG()
 *
 * SanDiego_Brutus_Skin/1   1/26/06 9:52a tokushig
 * add skinning capability
 *
 * 5   10/13/05 5:25p erickson
 * PR17541: moved general purpose code to mediatypes.c
 *
 * 4   8/22/05 9:25a erickson
 * PR16747: added Rename button
 *
 * 3   6/14/05 2:14p erickson
 * PR15216: converted to BKNI_Snprintf
 *
 * 2   2/23/05 9:32a erickson
 * PR14180: BUFSIZE is used by vxworks header file
 *
 * 1   2/7/05 8:18p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/10   1/4/05 9:43a erickson
 * PR13097: removed description from GUI because it's of little value
 *
 * Irvine_BSEAVSW_Devel/9   11/19/04 3:27p erickson
 * PR13256: added more information
 *
 * Irvine_BSEAVSW_Devel/8   7/28/04 4:49p erickson
 * PR11771: video->mpeg.mpeg_type is guaranteed to be uptodate until we
 * actually play
 *
 * Irvine_BSEAVSW_Devel/7   1/29/04 2:26p erickson
 * PR9107: merging back from BRUTUS_PR9107 branch for bwin development
 *
 * Irvine_BSEAVSW_Devel/6   10/28/03 11:25a erickson
 * settop api reworks after internal design view
 *
 * Irvine_BSEAVSW_Devel/5   10/22/03 11:22a erickson
 * settop api pass-through type conversion
 *
 * Irvine_BSEAVSW_Devel/4   8/22/03 5:21p erickson
 * venom2 settop api work
 *
 * Irvine_BSEAVSW_Devel/3   8/18/03 11:03a erickson
 * initial conversion of brutus to settop api, still in progress
 *
 * Irvine_BSEAVSW_Devel/2   7/23/03 4:01p erickson
 * renamed from pvrgui to brutus
 *
 * Irvine_BSEAVSW_Devel/3   4/30/03 9:12a erickson
 * audio rework
 *
 * Irvine_BSEAVSW_Devel/2   4/16/03 12:35p erickson
 * implemented font-string support
 *
 * Irvine_BSEAVSW_Devel/1   4/9/03 11:22a erickson
 * rework of pvrgui files and class names
 *
 **************************************************************************/
#include "videoscreen.h"
#include "control.h"
#include "mmessagebox.h"
#include "cfgsettings.h"
#include "playbackscreen.h"
#include "buttonlistbox.h"

#include "bdbg.h"

BDBG_MODULE(brutus_videoscreen) ;

#define playbackScreen()    ((PlaybackScreen *)control()->screen(Control::ePlayback))


/********************************************
*
* VideoScreen
*
********************************************/
VideoScreen::VideoScreen(Control *acontrol, MRect rectTVWin) :
    GenericScreen(acontrol, rectTVWin) //,SMALLTV //, SMALLPIP)
{
    Skin                 * pSkin                = control()->getSkin();
    MRect                  rectListBox(445, 190, 150, 250);
    uint32_t               nBevelListBox        = 2;
    uint32_t               nRows                = 4;
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

    if (control()->getSkin()->isValid(XML_SCREEN_VIDEO))
    {
        control()->getSkin()->createWidgets(XML_SCREEN_VIDEO, this);

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
        uint32_t        listButtonVSpacing  = 0;

        pSkin->getAttrValue(XML_SCREEN_VIDEO,
                            XML_ATT_GEOM_LIST_X,            listX);
        pSkin->getAttrValue(XML_SCREEN_VIDEO,
                            XML_ATT_GEOM_LIST_Y,            listY);
        pSkin->getAttrValue(XML_SCREEN_VIDEO,
                            XML_ATT_GEOM_LIST_H,            listHeight);
        pSkin->getAttrValue(XML_SCREEN_VIDEO,
                            XML_ATT_GEOM_LIST_W,            listWidth);
        pSkin->getAttrValue(XML_SCREEN_VIDEO,
                            XML_ATT_BEVEL_LIST_WIDTH,       listBevelWidth);
        pSkin->getAttrValue(XML_SCREEN_VIDEO,
                            XML_ATT_BEVEL_LIST_STYLE,       listBevelStyle);
        pSkin->getAttrValue(XML_SCREEN_VIDEO,
                            XML_ATT_LIST_COLUMNS,           listColumns);
        pSkin->getAttrValue(XML_SCREEN_VIDEO,
                            XML_ATT_LIST_VISIBLE_ROWS,      listRows);
        pSkin->getAttrValue(XML_SCREEN_VIDEO,
                            XML_ATT_LIST_TEXT_COLOR,        listTextColor);
        pSkin->getAttrValue(XML_SCREEN_VIDEO,
                            XML_ATT_LIST_TEXT_COLOR_FOCUS,  listTextColorFocus);
        pSkin->getAttrValue(XML_SCREEN_VIDEO,
                            XML_ATT_LIST_TEXT_ALIGN,        listTextAlign);
        pSkin->getAttrValue(XML_SCREEN_VIDEO,
                            XML_ATT_LIST_FOCUS_IMAGE,       listFocusImage);
        pSkin->getAttrValue(XML_SCREEN_VIDEO,
                            XML_ATT_LIST_SCROLLBAR_IMAGE,   listScrollbarImage);
        pSkin->getAttrValue(XML_SCREEN_VIDEO,
                            XML_ATT_LIST_SCROLLIND_IMAGE,   listScrollIndImage);
        pSkin->getAttrValue(XML_SCREEN_VIDEO,
                            XML_ATT_LIST_DROP_SHADOW,       listDropShadow);
        pSkin->getAttrValue(XML_SCREEN_VIDEO,
                            XML_ATT_LIST_BUTTON_V_SPACING,  listButtonVSpacing);

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

        buttonVSpacing = listButtonVSpacing;

        iconColumnWidth = 50;
        showButtons     = false;
    }
    else
    {
        int x = 60-38;
        int y = 25;
        l1 = new MLabel(this, MRect(x,y,350,40));
        l1->setFont(MFont(app(), control()->cfg()->get(Config::SCREEN_TITLE_FONT)));
        y += 50;
    #if 0
        l2 = new MLabel(this, MRect(x,y,300,70));
        l2->setWrapMode(MLabel::WordWrap);
    #endif
        y += 80;
        x += 20;
        l3 = new MLabel(this, MRect(x,y,400,170));
        l3->setWrapMode(MLabel::NewLineWrap);

        x -= 20;
        _back = new MPushButton(this, MRect(x,375,120,40), "Back");
        _back->addHandler(this);
    }

    //required widgets - these widgets must always exist for this screen so
    //we create them here.

    int totalbtns = 0;
    MPushButton *button;

    _listbox = new ButtonListBox(this, rectListBox, 15, buttonVSpacing, cfg()->getInt(Config::PVRBAR_PLAYBACK_COLOR), "button_listbox");
    _listbox->setTextColor(nListTextColor);
    _listbox->setTextColorFocus(nListTextColorFocus);
    _listbox->scrollBar()->setButtons(false);
    _listbox->scrollBar()->setIndicator(true);
    _listbox->setTotalVisible(nRows);
    _listbox->captureKeys(true, true);
    _listbox->setIndent(10);

    _listbox->setFocusImage(pSkin->getImage(listFocusImage.s()));
    _listbox->scrollBar()->setImage(pSkin->getImage(listScrollbarImage.s()));
    _listbox->scrollBar()->setIndicatorImage(pSkin->getImage(listScrollIndImage.s()));
    _listbox->setDropShadow(listDropShadow);

    _listbox->addHandler(this);

    _listbox->addColumn(iconColumnWidth);   // icon
    _listbox->addColumn(rectListBox.width()-30, textAlign);    // name

    ButtonListBox * pButtonListBox = (ButtonListBox *)_listbox;
    pButtonListBox->showButtons(showButtons);

#ifdef PVR_SUPPORT
	if (control()->cfg()->getBool(Config::PLAYBACK_ENABLED)) {
        createButton(pButtonListBox, totalbtns, "Play",
                     pSkin->getImage("imgIconPlay"), eActionPlayVideo);
        totalbtns++;
    }
#endif

#ifdef B_HAS_TRANSCODE
    createButton(pButtonListBox, totalbtns, "Transcode",
                 pSkin->getImage("imgIconTranscode"), eActionPlayVideoTranscode);
	totalbtns++;
#endif

#ifdef PVR_SUPPORT
    if (control()->cfg()->getBool(Config::RESUME_ENABLED)) {
        createButton(pButtonListBox, totalbtns, "Resume",
                     pSkin->getImage("imgIconResume"), eActionResumeVideo);
        totalbtns++;
    }
#endif

    createButton(pButtonListBox, totalbtns, "Rename",
                 pSkin->getImage("imgIconRename"), eActionRenameVideo);
    totalbtns++;

    createButton(pButtonListBox, totalbtns, "Delete",
                 pSkin->getImage("imgIconDelete"), eActionDeleteVideo);
    totalbtns++;

    if (control()->getSkin()->isValid(XML_SCREEN_VIDEO))
    {
        createButton(pButtonListBox, totalbtns, "Back to Playlist",
                     pSkin->getImage("imgIconBack2Play"), eActionShowPlaybackWin);
        totalbtns++;
    }
}

void VideoScreen::createButton(ButtonListBox * pListBox, int index, const char * name, MImage * icon, eSkinAction action)
{
    MString strTmp(index);
    eSkinAction * pActionTmp = NULL;

    strTmp += "\t";
    strTmp += name;
    pListBox->insert(index, strTmp.s(), icon);
    pActionTmp = new eSkinAction(action);
    _buttonActionList.insert(index, pActionTmp);
}

void VideoScreen::setVideo(Video *v) {
    // Even if Video object is the same, file size, length, resume position may change.
    // Therefore always refresh.
    _video = v;


    if (control()->getSkin()->isValid(XML_SCREEN_VIDEO))
    {
        Skin * pSkin = control()->getSkin();

        //update skin based dynamic labels
        pSkin->processActionLabel(name(), eActionVideoTitle);
        pSkin->processActionLabel(name(), eActionVideoDate);
        pSkin->processActionLabel(name(), eActionVideoFilename);
        pSkin->processActionLabel(name(), eActionVideoSize);
        pSkin->processActionLabel(name(), eActionVideoLength);
        pSkin->processActionLabel(name(), eActionVideoIndex);
        pSkin->processActionLabel(name(), eActionVideoVFormat);
        pSkin->processActionLabel(name(), eActionVideoAFormat);
        pSkin->processActionLabel(name(), eActionVideoVPid);
        pSkin->processActionLabel(name(), eActionVideoAPid);
        pSkin->processActionLabel(name(), eActionVideoEncryption);

        return;
    }

    if (_video) {
        l1->setText(_video->title);
#if 0
        l2->setText(_video->description);
#endif

        char vid_size[32];
        b_print_size(vid_size, 32, _video->size());
        char vid_length[32];
        b_print_time_period(vid_length, 32, _video->length(), B_TIME_PERIOD_FORMAT_COMPACT);
        char rec_time[32];
        b_print_calendar_time(rec_time, 32, _video->recordtime);

#define VIDEO_BUFSIZE 1024
        char buf[VIDEO_BUFSIZE];
        int n = BKNI_Snprintf(buf, VIDEO_BUFSIZE,
            "Recorded: %s\n"
            "File: %s\n"
            "Size: %s\n"
            "Length: %s\n"
            "Index: %s\n"
            "Video Format: %s, pid 0x%x\n"
            "Audio Format: %s, pid 0x%x\n"
            "Encryption: %s\n",
            rec_time,
            (const char *)_video->mediafile, vid_size,
            vid_length,
            _video->indexFormat() == bindex_format_bcm ? "Broadcom (NAV)":
                _video->indexFormat() == bindex_format_sct ? "Start Code":
                "None",
            b_vidxpt_str(_video->mpeg.video[0].format, _video->mpeg.mpeg_type),
            _video->mpeg.video[0].pid,
            b_audtype_str(_video->mpeg.audio[0].format),
            _video->mpeg.audio[0].pid,
            g_encryptionStr[_video->mpeg.encryption.type]);

        if (control()->cfg()->getBool(Config::RESUME_ENABLED)) {
            char resume_pos[32];
            b_print_time_period(resume_pos, 32, _video->indexToTime(_video->resumeindex),
                B_TIME_PERIOD_FORMAT_LONG);
            BKNI_Snprintf(&buf[n], VIDEO_BUFSIZE-n, "Resume Position: %s\n", resume_pos);
        }
        l3->setText(buf);
    }
    else {
        l1->setText(NULL);
#if 0
        l2->setText(NULL);
#endif
        l3->setText(NULL);
    }
}

void VideoScreen::onItemClicked(MListBox *_listbox) {
    int currentAction = 0;

    if (_buttonActionList.get(_listbox->currentIndex()))
    {
        currentAction = *(_buttonActionList.get(_listbox->currentIndex()));
    }

#if B_HAS_TRANSCODE
    if (currentAction == eActionPlayVideoTranscode) {
        control()->transcodePlaybackVideo(_video, true);
    }
    else
#endif
    if (currentAction == eActionPlayVideo)
    {
        playVideo(false);
    }
    else
    if (currentAction == eActionResumeVideo)
    {
        playVideo(true);
    }
    else
    if (currentAction == eActionRenameVideo)
    {
        renameVideo(_video);
    }
    else
    if (currentAction == eActionDeleteVideo)
    {
        if (deleteVideo(_video))
            _listbox->setCurrentIndex(0); /* set focus on Play button if delete succeeds */
    }
    else
        control()->getSkin()->processAction(NULL, currentAction);
}

void VideoScreen::onClick(MButton *self) {

    if (self == _back) {
        control()->showScreen(Control::ePlayback);
    }
    else
    if (control()->getSkin()->isValid(XML_SCREEN_VIDEO))
    {
        //handle generic skin actions not specific to this screen
        control()->getSkin()->processAction(self);
    }
    else
        GenericScreen::onClick(self);
}

void VideoScreen::playVideo(bool resume) {
/* PR20199 20060727 bandrews - For playback, we were starting decode
before we switched to full screen, which resulted in sync code unmuting
unconditionally (because we were not full screen) and then we switch to full
screen and hear a dropout */
#if 1
    control()->showScreen(Control::eTV);
    if (!control()->playVideo(_video, resume)) {
	    control()->showScreen(Control::eVideo);
    }
#else
    if (control()->playVideo(_video, resume)) {
        control()->showScreen(Control::eTV);
    }
#endif
}

#if B_HAS_TRANSCODE
void VideoScreen::transcodePlaybackVideo(void)
{
    control()->showScreen(Control::eTV);
    if (!control()->transcodePlaybackVideo(_video)) {
	    control()->showScreen(Control::eVideo);
    }
}
#endif

void VideoScreen::renameVideo(Video * v) {
    if (v && control()->verifyAdminPassword()) {
        MString response;
        if (MMessageBox::input(fb(), "New video name", 200, MMessageBox::bOK|MMessageBox::bCancel,
            response) == MMessageBox::bOK)
        {
            v->title = response;
            v->write();
            setVideo(v);
        }
    }
}

bool VideoScreen::deleteVideo(Video * v) {
    bool rc = playbackScreen()->deleteVideo(v);
    if (rc)
        control()->showScreen(Control::ePlayback);
    return rc;
}

bool VideoScreen::focus() {

    //get default focused widget from skin if valid
    if (control()->getSkin()->isValid(XML_SCREEN_VIDEO))
    {
        Skin * pSkin = control()->getSkin();
        MString         strFocus;

        //get screen's default focused widget
        pSkin->getAttrValue(XML_SCREEN_VIDEO, XML_ATT_FOCUS, strFocus);

        if (!strFocus.isEmpty())
        {
            MWidget * pWidget = NULL;
            pWidget = pSkin->getWidget(strFocus.s());

            if (pWidget)
            {
                return pWidget->focus();
            }
        }
    }

    return _listbox->focus();
}
