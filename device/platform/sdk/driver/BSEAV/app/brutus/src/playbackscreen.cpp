/***************************************************************************
 *     Copyright (c) 2002-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: playbackscreen.cpp $
 * $brcm_Revision: 17 $
 * $brcm_Date: 8/17/10 2:28p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/playbackscreen.cpp $
 * 
 * 17   8/17/10 2:28p erickson
 * SW7408-91: support video-only, which is a common case
 *
 * 16   8/15/10 5:50p vishk
 * SW7408-91: print video and audio pids on playback screen detail panel
 * so that multi-program streams can be differentiated
 *
 * 15   6/1/09 7:55p jtna
 * PR43001: added record-related brutus script commands
 *
 * 14   7/1/08 3:36p erickson
 * PR41934: avoid infinite loop on focus control if no Videos in thumbnail
 * mode
 *
 * 13   6/13/08 6:02p jgarrett
 * PR 43208: Initializing thumbnail manager to NULL by default
 *
 * 12   3/24/08 10:07a erickson
 * PR40307: fix non-thumbnail mode
 *
 * 11   3/22/08 2:10a erickson
 * PR40307: thumbnail demo update
 *
 * 10   3/20/08 1:49a erickson
 * PR40307: update
 *
 * 9   3/19/08 9:35p erickson
 * PR40307: thumbnail update
 *
 * 8   3/18/08 4:05p erickson
 * PR40307: added thumbnail feature
 *
 * 7   2/13/08 4:52p tokushig
 * PR39534: added bluetooth remote capabilities to brutus
 *
 * SanDiego_Brutus_3D/1   12/6/07 11:14a tokushig
 * do not redraw currently focused widget unless the playback screen is
 * dislayed.
 *
 * 6   8/2/07 5:40p gmohile
 * PR 33552 : check for video before setting text
 *
 * 5   7/27/07 9:41a erickson
 * PR33552: added Video index number to description to assist in writing
 * scripts
 *
 * 4   7/26/07 8:41p erickson
 * PR33552: added title of Video to top of screen so we can read long
 * names
 *
 * 3   4/13/07 4:52p erickson
 * PR29756: restore focus after refreshing listbox
 *
 * 2   3/28/07 4:51p ahulse
 * PR28951: Increase resolution of reported disk space to 2 decimal places
 *
 * 1   3/14/07 10:15a erickson
 * PR26267: refactor Brutus into separate VideoList and PlaybackScreen
 *
 * 25   2/7/07 12:28p erickson
 * PR22543: improve Brutus Playback UI for audio-only streams
 *
 * 24   12/6/06 4:47p ptimariu
 * PR26366: fix stream object endian
 *
 * 24   12/6/06 4:45p ptimariu
 * PR26366: fix asf stream object endian
 *
 * 23   11/2/06 11:54a erickson
 * PR24770: write indexfile to ASF .info file
 *
 * 22   10/27/06 11:47a erickson
 * PR24374: add play(++) support, echo script commands to console
 *
 ***************************************************************************/
#include "playbackscreen.h"
#include "videoscreen.h"
#include "control.h"
#ifndef _WIN32_WCE
#include <sys/stat.h>
#include <dirent.h>
#endif
#include <mmessagebox.h>
#include "cfgsettings.h"
#include "bstd.h"

BDBG_MODULE(brutus_playbackscreen);

#define abs(x) ((x)<0?-(x):(x))

/********************************************
*
* LibraryListBox - a custom list box entry for the Playback screen
*
********************************************/
class LibraryListBox : public MColumnListBox {
public:
    LibraryListBox(MWidget *parent, Config *cfg, VideoList *videoList, const BrutusDisplay *display,
        const MRect &rect, int scrollBarWidth, const char *name = NULL) :
        MColumnListBox(parent, rect, scrollBarWidth, name)
    {
        _cfg = cfg;
        _videoList = videoList;
        _display = display;
    }
protected:
    class LibraryListItem : public MColumnListItem {
    public:
        LibraryListItem(LibraryListBox *listbox, const char *name = NULL) :
            MColumnListItem(listbox, name)
        {
            _libraryListBox = listbox;
        }
    protected:
        void drawColumn(
            MPainter &ptr, int col, const char *text, int textlen, const MRect &rect,
            MPainter::Alignment al, MPainter::VAlignment val);
        LibraryListBox *_libraryListBox;
    };
    MListItem *createItem();
    Config *_cfg;
    VideoList *_videoList;
    const BrutusDisplay *_display;
    void drawPlayIcon(MPainter &ptr, int midx, int midy);
    void drawRecIcon(MPainter &ptr, int midx, int midy);
    void drawBuyIcon(MPainter &ptr, const MRect &rect);
};

void LibraryListBox::drawPlayIcon(MPainter &ptr, int midx, int midy)
{
    for (int y=midy-7;y<=midy+7;y++) {
        int width = (7-abs(y - midy)) * 3 / 2;
        if (width) {
            ptr.setPen(_cfg->getInt(Config::PVRBAR_PLAYBACK_COLOR));
            ptr.drawLine(midx-7, y, midx-7 + width, y);
        }
        ptr.setPen(textColor());
        ptr.point(midx-7-1, y);
        ptr.point(midx-7-2, y);
        ptr.point(midx-7 + width, y);
        ptr.point(midx-7 + width+1, y);
    }
}

void LibraryListBox::drawRecIcon(MPainter &ptr, int midx, int midy)
{
    ptr.setBrush(_cfg->getInt(Config::PVRBAR_RECORDING_COLOR));
    ptr.fillEllipse(midx, midy, 7);
    ptr.setPen(textColor());
    ptr.drawEllipse(midx, midy, 8);
    ptr.drawEllipse(midx, midy, 9);
}

void LibraryListBox::drawBuyIcon(MPainter &ptr, const MRect &rect)
{
    MRect rectBkgnd = rect;
    int   adjustment = rect.width() / 2;

    ptr.setBrush(_cfg->getInt(Config::PVRBAR_RECORDING_COLOR));
    rectBkgnd.setWidth(adjustment);
    rectBkgnd.setHeight(adjustment);
    rectBkgnd.setX(rect.midX() - (adjustment / 2));
    rectBkgnd.setY(rect.midY() - (adjustment / 2));
    ptr.fillRect(rectBkgnd);
    ptr.setPen(textColor());
    ptr.drawText(rect,"$",1,MPainter::alCenter,MPainter::valCenter);
}

void LibraryListBox::LibraryListItem::drawColumn(
    MPainter &ptr, int col, const char *text, int textlen, const MRect &rect,
    MPainter::Alignment al, MPainter::VAlignment val)
{
    if (col == 3 && text && *text == 'P') {
        _libraryListBox->drawPlayIcon(ptr, rect.midX(), rect.midY());
    }
    else if (col == 2 && text && *text == 'R') {
        _libraryListBox->drawRecIcon(ptr, rect.midX(), rect.midY());
    }
    else if (col == 1 && text && *text == 'E') {
        _libraryListBox->drawBuyIcon(ptr, rect); /* pay/encrypted icon */
    }
    else {
        /* call the base class */
        MColumnListItem::drawColumn(ptr, col, text, textlen, rect, al, val);
    }
}

MListBox::MListItem *LibraryListBox::createItem() {
    return new LibraryListItem(this, "libraryitem");
}

/********************************************
*
* PlaybackScreen
*
********************************************/

PlaybackScreen::PlaybackScreen(Control *acontrol, VideoList *videos, MRect rectTVWin) :
    GenericScreen(acontrol, acontrol->cfg()->getBool(Config::THUMBNAIL_MODE)?NOTV:rectTVWin),
    _timer(acontrol->fb()->app())
{
    MRect                rectListBox(17, 200, 600, 200);
    uint32_t             nBevelListBox        = 2;
    uint32_t             nRows                = 5;
    MPainter::BevelStyle styleBevel           = MPainter::bsRaised;
    int32_t              nListTextColor       = -1;
    int32_t              nListTextColorFocus  = -1;
    MString              listFocusImage;
    MString              listScrollbarImage;
    MString              listScrollIndImage;
    uint32_t             listDropShadow       = 0;

    _videos = videos;
    _videos->addHandler(this);

    _timer.addHandler(this);
    _timer.start(1000, MTimer::eForever); // update the UI for recording files

    _diskFree = NULL;
    _description = NULL;
    _thumbnailPanel = NULL;

    Skin * pSkin = control()->getSkin();
    if (pSkin->isValid(XML_SCREEN_PLAYBACK))
    {
        pSkin->createWidgets(XML_SCREEN_PLAYBACK, this);
        //return; //do not return here...we must continue creating required widgets

        //get list box attributes from skin
        MString         listBevelStyle;
        MString         listColumns;
        MString         listTextColor;
        MString         listTextColorFocus;
        uint32_t        listX               = 0;
        uint32_t        listY               = 0;
        uint32_t        listWidth           = 0;
        uint32_t        listHeight          = 0;
        uint32_t        listBevelWidth      = 0;
        uint32_t        listRows            = 0;

        pSkin->getAttrValue(XML_SCREEN_PLAYBACK,
                            XML_ATT_GEOM_LIST_X,            listX);
        pSkin->getAttrValue(XML_SCREEN_PLAYBACK,
                            XML_ATT_GEOM_LIST_Y,            listY);
        pSkin->getAttrValue(XML_SCREEN_PLAYBACK,
                            XML_ATT_GEOM_LIST_H,            listHeight);
        pSkin->getAttrValue(XML_SCREEN_PLAYBACK,
                            XML_ATT_GEOM_LIST_W,            listWidth);
        pSkin->getAttrValue(XML_SCREEN_PLAYBACK,
                            XML_ATT_BEVEL_LIST_WIDTH,       listBevelWidth);
        pSkin->getAttrValue(XML_SCREEN_PLAYBACK,
                            XML_ATT_BEVEL_LIST_STYLE,       listBevelStyle);
        pSkin->getAttrValue(XML_SCREEN_PLAYBACK,
                            XML_ATT_LIST_COLUMNS,           listColumns);
        pSkin->getAttrValue(XML_SCREEN_PLAYBACK,
                            XML_ATT_LIST_VISIBLE_ROWS,      listRows);
        pSkin->getAttrValue(XML_SCREEN_PLAYBACK,
                            XML_ATT_LIST_TEXT_COLOR,        listTextColor);
        pSkin->getAttrValue(XML_SCREEN_PLAYBACK,
                            XML_ATT_LIST_TEXT_COLOR_FOCUS,  listTextColorFocus);
        pSkin->getAttrValue(XML_SCREEN_PLAYBACK,
                            XML_ATT_LIST_FOCUS_IMAGE,       listFocusImage);
        pSkin->getAttrValue(XML_SCREEN_PLAYBACK,
                            XML_ATT_LIST_SCROLLBAR_IMAGE,   listScrollbarImage);
        pSkin->getAttrValue(XML_SCREEN_PLAYBACK,
                            XML_ATT_LIST_SCROLLIND_IMAGE,   listScrollIndImage);
        pSkin->getAttrValue(XML_SCREEN_PLAYBACK,
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

        //TODO: handle listColumns
    }
    else
    {
        if (!cfg()->getBool(Config::THUMBNAIL_MODE)) {
            l1 = new MLabel(this, MRect(14,25,350,40), "Playback", "librarylabel");
            l1->setFont(MFont(app(), control()->cfg()->get(Config::SCREEN_TITLE_FONT)));

            _diskFree = new MLabel(this, MRect(22,85,300,20));
            setDiskFree();

            _description = new MLabel(this, MRect(22,120,375,80), "", "description");
        }
        else {
            l1 = new MLabel(this, MRect(14,10,350,40), "Playback", "librarylabel");
            l1->setFont(MFont(app(), control()->cfg()->get(Config::SCREEN_TITLE_FONT)));

            _description = new MLabel(this, MRect(22,55,375,20), "", "description");
            _thumbnailPanel = new MThumbnailPanel(this, control(), MRect(22,85,630,80));
            _thumbnailPanel->setFullView(5,control()->cfg()->getInt(Config::BACKGROUND_COLOR));
        }

        _description->setWrapMode(MLabel::NewLineWrap);

        new MLabel(this, MRect(27,180,100,20), "Name");
        new MLabel(this, MRect(337,180,100,20), "Format");
        new MLabel(this, MRect(447,180,100,20), "Mbps");
        new MLabel(this, MRect(507,180,100,20), "Length");
    }

    //required widgets - these widgets must always exist for this screen so
    //we create them here.

    MLabel *bevel = new MLabel(this, rectListBox);
    bevel->setBevel(nBevelListBox, styleBevel);
    listbox = new LibraryListBox(bevel, cfg(), control()->videos(), control()->display(),
        MRect(2,2,bevel->width()-4,bevel->height()-4), 15, "library_listbox");
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

    listbox->addColumn(180);    // name
    listbox->addColumn(40);     // encrypted icon
    listbox->addColumn(40);     // record icon
    listbox->addColumn(50);     // play icon
    listbox->addColumn(110);    // mpeg format
    listbox->addColumn(60);     // bitrate
    listbox->addColumn(80);     // length

    listbox->addHandler(this);
    bevel->raise();

    // call addDir() for each dir, then refreshFromDisk()
}

void PlaybackScreen::setDiskFree() {
    unsigned long total, avail;

    if (!_diskFree)
    {
        return;
    }

    if (b_get_disk_usage(_videos->defaultVideoDir(), &total, &avail))
        _diskFree->setText("");
    else {
        char buf[64];
        BKNI_Snprintf(buf, 64, "Disk Size: %0.2fG, Free: %0.2fG",
            total / 1024.0 / 1024, avail / 1024.0 / 1024);
        _diskFree->setText(buf);
    }
}

void PlaybackScreen::onUpdate(VideoList *videoList)
{
    refreshList();
}

void PlaybackScreen::update(Video *vid, int index)
{
    char fmt[64];

    if (vid->mpeg.video[0].pid) {
        BKNI_Snprintf(fmt, 64, "%s",
            b_vidxpt_str(vid->mpeg.video[0].format, vid->mpeg.mpeg_type));
    }
    else {
        BKNI_Snprintf(fmt, 64, "%s %s",
            b_audtype_str(vid->mpeg.audio[0].format),
            b_xpttype_str(vid->mpeg.mpeg_type));
    }

#define LIB_BUFSIZE 64
    char bitrate_str[LIB_BUFSIZE];

    /* calculate bitrate in Mbps (megabits per second) if there's a length */
    unsigned int seconds = vid->length() / 1000;
    if (seconds) {
#ifdef __vxworks
        /* The vxworks compiler chokes on int64_t -> float conversion. So do things in a different
        order to keep things in bounds. */
        float bitrate = vid->size() / 1024 / 1024.0 * 8;
        bitrate /= seconds;
#else
        float bitrate = (float)vid->size() * 8 / seconds / 1024 / 1024;
#endif
        int n = BKNI_Snprintf(bitrate_str, LIB_BUFSIZE, "%0.1f\t", bitrate);
        b_print_time_period(&bitrate_str[n], LIB_BUFSIZE-n, vid->length(), B_TIME_PERIOD_FORMAT_COMPACT);
    }
    else {
        int n = BKNI_Snprintf(bitrate_str, LIB_BUFSIZE, "\t");
        b_print_size(&bitrate_str[n], LIB_BUFSIZE-n, vid->size());
    }

#define BIG_BUFSIZE 256
    char buf[BIG_BUFSIZE];

    BKNI_Snprintf(buf, BIG_BUFSIZE, "%s%s\t%s\t%s\t%s\t%s\t%s",
        (const char *)vid->title,
        control()->cfg()->getBool(Config::SHOW_VIDEODIR_LABELS)?
            (const char *)(" ("+vid->dirLabel+")"):"",
        vid->isForPurchase?"E":"",
        vid->record?"R":"",
        vid->playRefCnt?"P":"",
        fmt,
        bitrate_str);

    if (index >= listbox->total() || index < 0) {
        BDBG_MSG(("add Video: %d", index));
        listbox->insert(index, buf);
    }
    else {
        BDBG_MSG(("change Video: %d", index));
        listbox->change(index, buf);
    }
}

/**
Resync with the disk. This will add/remove Videos according to what's
on disk.
**/
void PlaybackScreen::refreshFromDisk() {
    int cur = listbox->currentIndex();
    listbox->clear();
    _videos->clear();

    _videos->refreshFromDisk();

    // this may be an invalid index at this point, but it's better than just
    // going to index 0 everytime.
    listbox->setCurrentIndex(cur);
}

void PlaybackScreen::show() {
    if (control()->cfg()->getInt(Config::ALWAYS_REFRESH_SHOWS))
        refreshFromDisk();
    else
        refreshList();
    setDiskFree();
    GenericScreen::show();
}

/**
Refresh the Videos that are already in the list.
This will update size/bitrate info, but won't add/remove Videos.
**/
void PlaybackScreen::refreshList(bool onlyRecording) {
    int cur = listbox->currentIndex();
    int top = listbox->topIndex();
    int i = 0;
    Video *video;
    MWidget *focused = app()->focusedWidget();

    listbox->beginUpdates();

    /* update these */
    if (_thumbnailPanel) {
        _thumbnailPanel->setVisible(NULL, false, false);
    }
    for (video = _videos->get(i); video; video = _videos->get(++i)) {
        if (_thumbnailPanel) {
            _thumbnailPanel->setVisible(video, false, (i >= top) && (i < top+listbox->totalVisible()));
        }
        if (!onlyRecording || video->record) {
            update(video, i);
        }
    }

    /* get rid of extras */
    for (;i<listbox->total();i++) {
        BDBG_MSG(("remove Video: %d", i));
        listbox->remove(i);
    }

    listbox->setCurrentIndex(cur);
    listbox->setTopIndex(top);
    if (focused) focused->focus();
    listbox->endUpdates();

}

void PlaybackScreen::onClick(MButton *button) {
    if (control()->getSkin()->isValid(XML_SCREEN_PLAYBACK))
    {
        control()->getSkin()->processAction(button);
        //library screen has no screen specific actions in onClick() so return...
        return;
    }

    GenericScreen::onClick(button);
}

void PlaybackScreen::onCurrentItemChanged(MListBox *listbox)
{
    Video *video = currentVideo();
    if (!video) return;

    if (_description) {
        char buf[256];
        if (video->mpeg.video[0].pid && video->mpeg.audio[0].pid) {
            BKNI_Snprintf(buf, 256,
                "%d: %s\n"
                "Video Pid: 0x%x, Audio Pid: 0x%x\n",
                listbox->currentIndex()+1,
                (const char *)video->title,
                video->mpeg.video[0].pid,
                video->mpeg.audio[0].pid);
        }
        else if (video->mpeg.video[0].pid) {
            BKNI_Snprintf(buf, 256,
                "%d: %s\n"
                "Video Pid: 0x%x\n",
                listbox->currentIndex()+1,
                (const char *)video->title,
                video->mpeg.video[0].pid);
        }
        else if (video->mpeg.audio[0].pid) {
            BKNI_Snprintf(buf, 256,
                "%d: %s\n"
                "Audio Pid: 0x%x\n",
                listbox->currentIndex()+1,
                (const char *)video->title,
                video->mpeg.audio[0].pid);
        }
        else {
            BKNI_Snprintf(buf, 256,
                "%d: %s\n"
                "Unknown pids\n",
                listbox->currentIndex()+1,
                (const char *)video->title);
        }
        _description->setText(buf);
    }


    /* update thumbnails */
    if (_thumbnailPanel) {
        _thumbnailPanel->setVideo(video);
    }
}

void PlaybackScreen::onItemClicked(MListBox *listbox) {
    selectVideo();
}

Video *PlaybackScreen::currentVideo() const {
    return _videos->get(listbox->currentIndex());
}

bool PlaybackScreen::selectVideo(bool play) {
    Video *vid = currentVideo();
    if (vid) {
        if (play) {
            /* TODO: if video is already playing, just go to the tv screen.
            However...on pip systems, we might have to show pip and make
            a display current. This can be done later. */
            return control()->playVideo(vid);
        }
        else {
            ((VideoScreen*)control()->screen(Control::eVideo))->setVideo(vid);
            control()->showScreen(Control::eVideo);
            return true;
        }
    }
    return false;
}

void PlaybackScreen::onInsertItem(MListBox *self) {
    control()->showScreen(Control::eRecord);
}

void PlaybackScreen::onDeleteItem(MListBox *self) {
    deleteVideo(currentVideo());
}

bool PlaybackScreen::deleteVideo(int index)
{
    Video *vid = _videos->get(index);
    if (vid) {
        return deleteVideo(vid, false);
    }
}

bool PlaybackScreen::deleteVideo(Video *vid, bool prompt) {
    if (!vid) return false;

    // check to make sure this video isn't being recording
    if (vid->record || vid->playRefCnt) {
        MMessageBox::info(fb(), "This video is currently in use.");
        return false;
    }

    if (!prompt ||
        MMessageBox::show(fb(), "Delete this video?",
        MMessageBox::bYes|MMessageBox::bNo,
        MMessageBox::bNo) == MMessageBox::bYes)
    {
        if (!prompt
            ||
            control()->verifyAdminPassword())
        {
            if (!_videos->deleteVideo(vid)) {
                MMessageBox::info(fb(), "Unable to delete");
                return false;
            }
            return true;
        }
    }
    return false;
}


bool PlaybackScreen::focus() {
    // don't set focus if it could be thrown back at us
    return listbox->total()?listbox->focus():
        cfg()->getBool(Config::THUMBNAIL_MODE) ? false :  /* no tv bevel in thumbnail mode */
        _tvbevel.focus();
}

void PlaybackScreen::onTimeout(MTimer *timer)
{
    if ( control()->screen( control()->currentScreen() ) == this )
        refreshList(true); /* only recording */
}
