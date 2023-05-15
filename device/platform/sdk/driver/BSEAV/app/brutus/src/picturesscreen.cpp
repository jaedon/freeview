/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: picturesscreen.cpp $
 * $brcm_Revision: 27 $
 * $brcm_Date: 3/19/12 6:44p $
 *
 * Module Description: Collection of screens for pvrgui demo app
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/picturesscreen.cpp $
 * 
 * 27   3/19/12 6:44p erickson
 * SW7425-2664: purge old code
 * 
 * 26   3/16/09 4:48p randyjew
 * PR42789: updates for BLUETOOTH_SUPPORT
 * 
 * 25   9/15/08 12:40p tokushig
 * PR43426: merge to main
 * 
 * BRUTUS_97405_97043/2   9/12/08 5:37p tokushig
 * PR43426: partial merge to branch from main
 * 
 * BRUTUS_97405_97043/1   8/29/08 10:48a tokushig
 * PR43596: fix bus error on exit with blue skin.  also fixes intermittent
 * assertion in bwin (during quit)
 * 
 * 24   8/15/08 10:08a erickson
 * PR42789: bluetooth update
 *
 * 23   8/13/08 10:12a erickson
 * PR42789: add BLUETOOTH_SUPPORT demo code
 *
 * 22   3/17/08 3:17p erickson
 * PR40307: remove old, unused code before adding new
 * 
 * 21   2/21/08 2:11p ahulse
 * PR25542: Add ability to display a picture from name of file, plus list
 * picture files on disk
 * 
 * 20   8/22/07 12:11p tokushig
 * PR23293: centering of full screen picture for non-skinned brutus,
 * caused missing background image for skinned brutus.  fixed bug to
 * restore background.
 * 
 * 19   4/17/07 3:37p rjlewis
 * PR28644: needs cast to select correct version of overloaded function.
 * 
 * 18   3/22/07 11:58a erickson
 * PR28727: _title is not used in skin mode
 * 
 * 17   3/13/07 9:54a erickson
 * PR28644: use opaque black pixel in pictures screen full-screen mode
 * border. don't rely on alpha 0 and no video.
 * 
 * 16   3/13/07 9:40a erickson
 * PR28342: removed unused MQueue data structure. see PR for design notes.
 * 
 * 15   2/27/07 4:12p erickson
 * PR24971: remove vertical scrollbar buttons which weren't connected and
 * were hard to navigate to anyway
 * 
 * 14   2/27/07 9:27a erickson
 * PR23293: center picture, black-out border
 * 
 * 13   2/26/07 5:10p erickson
 * PR27780: added image decode time printout
 * 
 * 11   2/2/07 10:06a erickson
 * PR26661: call MApplication::clearModalState after leaving modal loop
 * 
 * 10   1/26/07 3:53p erickson
 * PR26796: full screen Pictures pause should be consumed, not passed on
 * 
 * 8   11/22/06 3:57p ahulse
 * PR25543: Add script support to display image with script command
 * show_img(
 * 
 * 7   11/3/06 2:49p tokushig
 * PR25522: disallow entering full screen mode if no pictures exist.
 * 
 * 6   8/25/06 1:26p tokushig
 * PR22347: in non-skinned brutus, images w/o embedded thumbnails will
 * display full image as thumb.  if using a skin, small images w/o
 * embedded thumbnails will display full image as thumb but large images
 * w/o embedded thumbnails will display a placeholder icon instead
 * (images are currently considered large if they have more than 640 *
 * 480 = 307200pixels.)  thumbnail please wait popup was replaced by
 * sequential drawing of thumbs which provides better feedback to users.
 * implemented image caching on thumbs (default: last 30) and for full
 * screen image view (default: last 3).
 * 
 * SanDiego_Brutus_Skin/21   8/23/06 5:04p tokushig
 * added nothumb icon handling from skin, removed thumbnails loading
 * popup, display main picturesscreen before loading thumbnails
 * 
 * SanDiego_Brutus_Skin/20   8/17/06 5:01p tokushig
 * added mcache support for fullscreen view
 * 
 * SanDiego_Brutus_Skin/19   8/3/06 9:31a tokushig
 * added image caching for fullscreenview
 * 
 * 5   6/6/06 11:48a tokushig
 * PR20349: ensure all dynamically allocated bwin framebuffers are
 * destroyed before deleting display
 * 
 * SanDiego_Brutus_Skin/18   6/6/06 11:11a tokushig
 * clean up dynamically allocated objects in destructor
 * 
 * 4   5/31/06 4:19p tokushig
 * PR21891: Merging updated picture viewer into mainline.
 * 
 * SanDiego_Brutus_Skin/17   5/30/06 3:04p tokushig
 * add alphabetical ordering for thumbnail list
 * 
 * SanDiego_Brutus_Skin/16   5/22/06 10:37a tokushig
 * enabled progress bar in thumbnail loading mmessagebox.  moved pixmap
 * creation from draw() to setImage().  added #define to specify 1)
 * random transitions or 2) ordered transitions (defaulted to ordered).
 * fixed onscreen status messages (next, prev, play, pause) to appear
 * immediately after corresponding key press.
 * 
 * SanDiego_Brutus_Skin/15   5/18/06 12:45p tokushig
 * remove swap transition during slideshow
 * 
 * SanDiego_Brutus_Skin/14   5/18/06 10:04a tokushig
 * thumbnail cache is turned on in
 * mlistview.cpp@/main/SanDiego_Brutus_Skin/9 so limit max number of
 * photos to 100
 * 
 * SanDiego_Brutus_Skin/13   5/16/06 10:17a tokushig
 * Merge from main
 * 
 * SanDiego_Brutus_Skin/12   5/12/06 12:44p tokushig
 * enable limited set of transitions for legacy platforms
 * 
 * SanDiego_Brutus_Skin/11   5/11/06 11:04a tokushig
 * increase maximum number of pictures
 * 
 * SanDiego_Brutus_Skin/10   5/10/06 3:48p tokushig
 * add status popups for loading thumbnails and when loading an image
 * between thumbnail view and full screen view
 * 
 * SanDiego_Brutus_Skin/9   5/10/06 2:34p tokushig
 * Turn off transitions for legacy platforms since there will be no
 * hardware support for copyrect and blit.
 * 
 * SanDiego_Brutus_Skin/8   5/9/06 3:01p tokushig
 * Add transition support to full screen view slideshow
 * 
 * SanDiego_Brutus_Skin/7   5/4/06 2:32p tokushig
 * removed EXIF and thumbnail handling from picturescreen.  this is now
 * handled transparently at the bwin level.
 * 
 * SanDiego_Brutus_Skin/6   4/27/06 10:52a tokushig
 * added exif thumbnail support - note that thumbs are read in when the
 * pictures directory is scanned.  this will change in the future as we
 * do not need to fetch the thumbs until they are to be displayed.
 * 
 * 3   4/17/06 5:06p jgarrett
 * PR 20951: Merging skinned UI into mainline
 * 
 * SanDiego_Brutus_Skin/5   4/4/06 4:56p tokushig
 * never load jpeg images into MImages - only open file and maintain file
 * handle.  decompressor will read file directly.
 * 
 * SanDiego_Brutus_Skin/4   4/4/06 11:09a tokushig
 * add code to force pictures screen to decompress jpeg images directly
 * from the files.  compressed images are not saved in memory.  this
 * allows us to display large jpeg images when under tight memory
 * constraints.  currently disabled pending further testing.
 * 
 * SanDiego_Brutus_Skin/3   2/28/06 1:23p tokushig
 * add dropshadow handling for picture icon text
 * 
 * SanDiego_Brutus_Skin/2   2/17/06 3:26p tokushig
 * added skinning capability to scrollbar
 * 
 * SanDiego_Brutus_Skin/1   2/15/06 5:36p tokushig
 * add partial skin capability to picturescreen
 * 
 * 2   3/25/05 1:55p erickson
 * PR14593: renamed label, made transparent, removed unused thumbnail code
 * (implemented in MListView)
 * 
 * 1   2/7/05 8:15p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/12   2/4/05 10:47a erickson
 * PR12728: cleanup for -pedantic warnings and errors
 * 
 * Irvine_BSEAVSW_Devel/10   9/9/04 1:58p erickson
 * PR12608: updated for bwin
 * 
 * Irvine_BSEAVSW_Devel/9   4/19/04 2:02p erickson
 * PR10471: made all filepaths configurable using brutus.cfg
 * 
 * Irvine_BSEAVSW_Devel/8   2/19/04 3:36p bandrews
 * PR9723: Implemented ARGB1555 support and tested ARGB8888 support
 * 
 * Irvine_BSEAVSW_Devel/7   1/29/04 2:26p erickson
 * PR9107: merging back from BRUTUS_PR9107 branch for bwin development
 * 
 * Irvine_BSEAVSW_Devel/6   1/7/04 2:10p erickson
 * PR8366: "Play" will now start from the current picture when entering
 * fullscreen mode
 * 
 * Irvine_BSEAVSW_Devel/5   12/11/03 12:37a erickson
 * PR8708: uclibc compiler is stricter about duplicate default values
 * 
 * Irvine_BSEAVSW_Devel/4   9/17/03 12:31p erickson
 * converted from original debug interface to magnum debug interface
 * 
 * Irvine_BSEAVSW_Devel/3   8/21/03 9:43a erickson
 * venom2 settop api work
 * 
 * Irvine_BSEAVSW_Devel/2   7/23/03 4:01p erickson
 * renamed from pvrgui to brutus
 * 
 * SanJose_Linux_Devel/13   5/6/03 9:16a erickson
 * added \n to status msg
 * 
 * SanJose_Linux_Devel/12   4/23/03 3:30p erickson
 * printf output for jpeg decode
 * 
 * SanJose_Linux_Devel/11   4/16/03 12:35p erickson
 * implemented font-string support
 * 
 * SanJose_Linux_Devel/10   4/15/03 12:40p erickson
 * 7115 work, added more build conditionals
 *
 * SanJose_Linux_Devel/9   4/9/03 11:21a erickson
 * rework of pvrgui files and class names
 *
 * SanJose_Linux_Devel/8   3/11/03 3:58p erickson
 * reworked debug to use debug interface
 *
 *************************************************************************/
#include "picturesscreen.h"
#include "cfgsettings.h"
#include "input_devicecodes.h"
#include "control.h"

#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>

#include <mmessagebox.h>
#include <mapplication.h>

#include "bstd.h"
#ifdef BLUETOOTH_SUPPORT
#include "easy_bt.h"
#include "bt_hook_priv.h"
#endif
BDBG_MODULE(pictures);

//#define PIXELFORMAT unsigned short // rgb565
#define SLIDESHOW_TIMER 4000 //2 sec per picture

#define VIEW_WIDTH 600

class Picture
{
public:
    Picture()
    {
        filename.clear();
        name.clear();
        full = NULL;
    };

    ~Picture()
    {
        if (full)
        {
            delete full;
            full = NULL;
        }
    };
    MString filename;
    MString name;
    MImage *full;

    int deleteFile()
    {
        return unlink((char *)(const char *)filename);
    }
};

/////////////////////////////////////////////////

void FullScreenView::setImage(const Picture *pic, 
                              MPainter::DrawImageMode drawImageMode, 
                              MPainter::TransitionMode transitionMode)
{
    if (!pic)
    {
        BDBG_WRN(("Invalid picture"));
        return;
    }
    
    MImage  * image      = pic->full;
    MPixmap * pixmapNew = NULL;
    
    _transition = transitionMode;
    
    //check cache first!  may already contain our image
    if ((pixmapNew = _cacheFullSize.reserve(pic->name)) == NULL)
    {
        //cache miss!
        
        //convert current image to dynamically allocated pixmap for cache
        MRect result = MPainter::fitRect(image->width(), image->height(), 
            MRect(0, 0, width(), height()),
            MPainter::eMaximize, 
            MPainter::alCenter, 
            MPainter::valCenter);
        pixmapNew = new MPixmap(fb(), width(), height());
        MPainter p2(pixmapNew);
        
        p2.setPen(0xFF000000);
        p2.fillRect(MRect(0, 0, width(), height()));
        
        int starttime = app()->tickCount();
        p2.drawImage(*image, 
            result.x(), result.y(),
            result.width(), result.height(), 
            MPainter::eMaximize, 
            MPainter::alCenter, MPainter::valCenter);
        BDBG_WRN(("%d msec, %s (%dx%d)", app()->tickCount() - starttime, (const char *)pic->name, 
            pic->full->width(), pic->full->height()));
        
        //add to image cache
        _cacheFullSize.add(pic->name, pixmapNew);
    }
    
    //no longer need old image
    _cacheFullSize.unreserve(_oldImageName);
    
    //reserve images for draw() - cannot be deleted from image cache
    if (_transition != MPainter::eSwap)
    {
        //reserve image if required to perform transition
        _cacheFullSize.reserve(_newImageName);
    }
    else
    {
        //unreserve image - not needed for next transition
        _cacheFullSize.unreserve(_newImageName);
    }
    
    //debugging help
    //_cacheFullSize.print();
    
    //keep track of image names for draw()
    _oldImageName = _newImageName;
    _newImageName = pic->name;
    
    MLabel::setImage(image, drawImageMode);
}

void FullScreenView::clear()
{
    _transition = MPainter::eSwap;
    _cacheFullSize.clear(true);
}

void FullScreenView::hide()
{
    _cacheFullSize.unreserveAll();
    MWidget::hide();
}

void FullScreenView::draw(const MRect &cliprect)
{
    MPainter p(this, cliprect);
    
    //these images should already be reserved (see setImage()) at this point
    MPixmap * pixmapNew = _cacheFullSize.get(_newImageName);
    MPixmap * pixmapOld = _cacheFullSize.get(_oldImageName);
    
    if (!pixmapNew) return;
    
    if (pixmapNew != pixmapOld && pixmapOld && MPainter::eSwap != _transition)
    {
        BDBG_MSG(("FullScreenView: drawing pixmap with transition: %d", _transition));
        //draw pixmaps with transition
        p.drawPixmapWithTransition(MPoint(_bevelWidth, _bevelWidth), 
            *pixmapOld, *pixmapNew, 
            MRect(0, 0, width(), height()), 
            _transition);
    }
    else
    {
        BDBG_MSG(("FullScreenView: drawing pixmap - no transition"));
        //draw pixmap without transition
        //p.drawPixmap(*pixmapNew);
        p.drawPixmap(MPoint(_bevelWidth, _bevelWidth), *pixmapNew, 
            MRect(0, 0, width(), height()));
    }
    
    _transition = MPainter::eSwap;
}


/////////////////////////////////////////////////

#if BLUETOOTH_SUPPORT
#include "easy_bt.h"

static MMessageBox *_msgBT = NULL;
static struct {
    bool state_changed;
    int state;
    MString filename;
    unsigned progress;
} bt;

/* bluetooth callback function */
void bt_ops_callback(int state, const char *filename, unsigned progress, void *context)
{
    /* It's important that we don't try to sync with the UI here. it will kill the bluetooth state machine.
    This opens us up to a big race condition. */
    bt.state_changed = true;
    bt.state = state;
    if (filename) {
        bt.filename = filename;
    }
    bt.progress = progress;
}

/* bt_ui is called at idle time from PicturesScreen::onTimeout */
void bt_ui(PicturesScreen * screen)
{
    if (!bt.state_changed) return;

    /* Because of the crude BT/Brutus integration, we may miss the btOpenConnection event. This ensures we step through it. */
    if (bt.state != 0 && !_msgBT) {
        bt.state = 0;
    }

    switch (bt.state) {
    case 0:
        {
            screen->control()->showScreen(Control::eTV);
            if (_msgBT)
            {
                delete _msgBT;
                _msgBT = NULL;
            }
            /* set up bluetooth status message box */
            _msgBT = new MMessageBox(screen->fb(), 300, 100);

            fprintf(stderr, "downloading: %s\n", (const char *)bt.filename);
            _msgBT->set((const char *)bt.filename, 0, -1, -1, 250);
            _msgBT->show();
            _msgBT->drawNow();
        }
        break;

    case 1:
        {
            MMessageBox * msgDone = new MMessageBox(screen->fb(), 300, 100);
            msgDone->set("Download Complete", 0, -1, -1, -1);
            msgDone->show();
            msgDone->drawNow();
            sleep(2);

            BDBG_ASSERT(_msgBT);
            delete _msgBT;
            _msgBT = NULL;
            delete msgDone;

            screen->control()->showScreen(Control::ePictures);
        }
        break;

    case 2:
        BDBG_ASSERT(_msgBT);
        _msgBT->setProgress(bt.progress);
        break;

    default:
        break;
    }

    bt.state_changed  = false;
}

/////////////////////////////////////////////////
/////////////////////////////////////////////////

#if 0
/* TODO: this must be reworked like the bt_ops_callback */
/* bluetooth callback  for SMS function */
void bt_sms_callback(void * data)
{
    PicturesScreen * screen = (PicturesScreen *)data;

    if (!screen)
    {
        fprintf(stderr, "ERROR - received bluetooth callback without data!\n");
        assert(1);
        return;
    }


    screen->app()->acquireIdleLock();

    switch (btBrutusSMSStatus)
    {
    case btSMSSent:
        {
            screen->control()->showScreen(Control::eTV);

            if (_msgBT)
            {
                delete _msgBT;
                _msgBT = NULL;
            }
            /* set up bluetooth status message box */
            _msgBT = new MMessageBox(screen->fb(), 300, 100);

            MString strMsg;
            //strMsg += "Downloading ";
            //strMsg += btBrutusDownloadFilename;
            strMsg = btBrutusSMSMesg;
            fprintf(stderr, "SMS sent: %s\n", strMsg.s());
            _msgBT->set(strMsg.s(), 0, -1, -1, -1);
   /*
            Skin * pSkin = screen->control()->getSkin();
            if (pSkin->isValid(XML_SCREEN_PICTURES))
            {
                MString         strImgProgressInd;
                pSkin->getAttrValue(XML_SCREEN_PICTURES,
                                    XML_ATT_PROGRESS_IND_IMAGE,
                                    strImgProgressInd);
                //set scrollbar indicator image if necessary
                if (!strImgProgressInd.isEmpty())
                {
                    _msgBT->progressBar()->setGeometry(
                        MRect(_msgBT->progressBar()->x(),
                        _msgBT->progressBar()->y(),
                        _msgBT->progressBar()->width(),
                        pSkin->getImage(strImgProgressInd.s())->height()));

                    _msgBT->progressBar()->setIndicatorImage(
                        pSkin->getImage(strImgProgressInd.s()));
                }
            }
*/
            _msgBT->show();
            _msgBT->drawNow();
         sleep(4);
            if (_msgBT)
            {
                delete _msgBT;
                _msgBT = NULL;
            }

        }
        break;

    case btSMSRecv:
        {
         screen->control()->showScreen(Control::eTV);

            if (_msgBT)
            {
                delete _msgBT;
                _msgBT = NULL;
            }
            /* set up bluetooth status message box */
            _msgBT = new MMessageBox(screen->fb(), 300, 100);

            MString strMsg;
            //strMsg += "Downloading ";
            //strMsg += btBrutusDownloadFilename;
            strMsg = btBrutusSMSMesg;

            fprintf(stderr, "SMS recvd: %s\n", strMsg.s());
            _msgBT->set(strMsg.s(), 0, -1, -1, -1);
   /*
            Skin * pSkin = screen->control()->getSkin();
            if (pSkin->isValid(XML_SCREEN_PICTURES))
            {
                MString         strImgProgressInd;
                pSkin->getAttrValue(XML_SCREEN_PICTURES,
                                    XML_ATT_PROGRESS_IND_IMAGE,
                                    strImgProgressInd);
                //set scrollbar indicator image if necessary
                if (!strImgProgressInd.isEmpty())
                {
                    _msgBT->progressBar()->setGeometry(
                        MRect(_msgBT->progressBar()->x(),
                        _msgBT->progressBar()->y(),
                        _msgBT->progressBar()->width(),
                        pSkin->getImage(strImgProgressInd.s())->height()));

                    _msgBT->progressBar()->setIndicatorImage(
                        pSkin->getImage(strImgProgressInd.s()));
                }
            }
*/
            _msgBT->show();
            _msgBT->drawNow();
              sleep(4);
            if (_msgBT)
            {
                delete _msgBT;
                _msgBT = NULL;
            }
        }
        break;

    case btSMSVoice:
    {
            screen->control()->showScreen(Control::eTV);

            if (_msgBT)
            {
                delete _msgBT;
                _msgBT = NULL;
            }
            /* set up bluetooth status message box */
            _msgBT = new MMessageBox(screen->fb(), 300, 100);

            MString strMsg;
            //strMsg += "Downloading ";
            //strMsg += btBrutusDownloadFilename;
            strMsg = btBrutusSMSMesg;
            fprintf(stderr, "Voice: %s\n", strMsg.s());
            _msgBT->set(strMsg.s(), 0, -1, -1, -1);
   /*
            Skin * pSkin = screen->control()->getSkin();
            if (pSkin->isValid(XML_SCREEN_PICTURES))
            {
                MString         strImgProgressInd;
                pSkin->getAttrValue(XML_SCREEN_PICTURES,
                                    XML_ATT_PROGRESS_IND_IMAGE,
                                    strImgProgressInd);
                //set scrollbar indicator image if necessary
                if (!strImgProgressInd.isEmpty())
                {
                    _msgBT->progressBar()->setGeometry(
                        MRect(_msgBT->progressBar()->x(),
                        _msgBT->progressBar()->y(),
                        _msgBT->progressBar()->width(),
                        pSkin->getImage(strImgProgressInd.s())->height()));

                    _msgBT->progressBar()->setIndicatorImage(
                        pSkin->getImage(strImgProgressInd.s()));
                }
            }
*/
            _msgBT->show();
            _msgBT->drawNow();
              sleep(3);
            if (_msgBT)
            {
                delete _msgBT;
                _msgBT = NULL;
            }
        }
        break;

    default:
        break;
    }

    screen->app()->releaseIdleLock();
}
#endif /* SMS */
#endif /* BLUETOOTH_SUPPORT */

PicturesScreen::PicturesScreen(Control *acontrol) :
    GenericScreen(acontrol, NOTV), _slideShowTimer(acontrol->fb()->app()),
        _statusTimer(acontrol->fb()->app()),
        _idleTimer(acontrol->fb()->app()),
        _deltimer(acontrol->fb()->app())
{
#if BLUETOOTH_SUPPORT
    /* register callbacks for bluetooth events */
#if EASY_BT
    EasyBtSettings btSettings;
    EasyBt_GetSettings(&btSettings);
    btSettings.picturePush.tempCallback = bt_ops_callback;
    btSettings.picturePush.callbackContext = this;
    EasyBt_SetSettings(&btSettings);
#else
     BtHookOpsSettings btOpsSettings;
    BtHook_GetOpsSettings(&btOpsSettings);
    btOpsSettings.tempCallback = bt_ops_callback;
    btOpsSettings.callbackContext = this; 
    BtHook_SetOpsSettings(&btOpsSettings); 
#endif

#endif

    srand(time(NULL));
    _slideShowTimer.addHandler(this);
    _statusTimer.addHandler(this);
    _deltimer.addHandler(this);
    _idleTimer.addHandler(this);
    _idleTimer.start(0, MTimer::eForever);
    _mode = MPainter::eScaleDownToMax;
    _maxPictures = control()->cfg()->getInt(Config::MAX_PICTURES, 100);
    _path = control()->cfg()->get(Config::PICTURES_PATH);
    _lastTransition = MPainter::eSwap;

    Skin * pSkin = control()->getSkin();
    if (pSkin->isValid(XML_SCREEN_PICTURES))
    {
        pSkin->createWidgets(XML_SCREEN_PICTURES, this);
        _title = NULL;
        //return; //do not return here...we must continue creating required widgets
    }
    else
    {
        _title = new MLabel(this, MRect(15,25,250,40), "Pictures");
        _title->setFont(MFont(app(), control()->cfg()->get(Config::SCREEN_TITLE_FONT)));
    }

    _bevel = new MLabel(this, MRect(17,75,VIEW_WIDTH,340), "", "pic_bevel");
    _bevel->setBevel(2, MPainter::bsRaised);
    _view = new MListView(_bevel, MRect(2,2,_bevel->width()-4,_bevel->height()-4), "pic_listview");
    _view->viewport()->setBackgroundColor(cfg()->getInt(Config::BACKGROUND_COLOR));
    _view->addHandler(this);
    _view->vScrollbar()->setButtons(false); // hide the buttons

    /* decrease width/height to make picture appear more centered. because viewable area can vary, this will
    never be perfect. */
    if (pSkin->isValid(XML_SCREEN_PICTURES))
    {
        _fullScreenView = new FullScreenView(this, MRect(0,0,width(),height()), "", "pic_fullscreen");
    }
    else
    {
        _fullScreenView = new FullScreenView(this, MRect(0,0,width()-25,height()-8), "", "pic_fullscreen");
    }

    _fullScreenView->setAlignment(MPainter::alCenter);
    _fullScreenView->setVAlignment(MPainter::valCenter);
    _fullScreenView->setFocusable(true);
    _fullScreenView->hide();

    _fullScreenStatus = new MLabel(_fullScreenView, MRect(50,50,80,40));
    _fullScreenStatus->hide();
    _fullScreenStatus->setAlignment(MPainter::alCenter);
    _fullScreenStatus->setVAlignment(MPainter::valCenter);
    _fullScreenStatus->setBevel(3);

    if (pSkin->isValid(XML_SCREEN_PICTURES))
    {
        //get list box attributes from skin
        MString         textColor; 
        MString         strImgScrollBar; 
        MString         strImgScrollInd; 
        MString         strImgNoThumb; 
        uint32_t        dropshadow;
        uint32_t        iconWidth;
        uint32_t        iconBorder;

        pSkin->getAttrValue(XML_SCREEN_PICTURES, 
                            XML_ATT_COLOR_TEXT,             textColor);
        pSkin->getAttrValue(XML_SCREEN_PICTURES, 
                            XML_ATT_LIST_SCROLLBAR_IMAGE,   strImgScrollBar);
        pSkin->getAttrValue(XML_SCREEN_PICTURES, 
                            XML_ATT_LIST_SCROLLIND_IMAGE,   strImgScrollInd);
        pSkin->getAttrValue(XML_SCREEN_PICTURES, 
                            XML_ATT_LIST_ICON_NOTHUMB,      strImgNoThumb);
        pSkin->getAttrValue(XML_SCREEN_PICTURES, 
                            XML_ATT_LIST_DROP_SHADOW,       dropshadow);
        pSkin->getAttrValue(XML_SCREEN_PICTURES, 
                            XML_ATT_ICON_WIDTH,             iconWidth);
        pSkin->getAttrValue(XML_SCREEN_PICTURES, 
                            XML_ATT_ICON_BORDER,            iconBorder);

        //set text color if necessary
        if (!textColor.isEmpty())
        {
            int32_t nCol = pSkin->getColorValue(textColor);
            setTextColor(nCol);
            _view->setTextColor(nCol);
            _view->viewport()->setTextColor(nCol);
            _fullScreenView->setTextColor(nCol);
            _fullScreenStatus->setTextColor(nCol);
        }

        if (dropshadow > 0)
        {
            setDropShadow(dropshadow);
            _view->setDropShadow(dropshadow);
            _view->viewport()->setDropShadow(dropshadow);
            _fullScreenView->setDropShadow(dropshadow);
            _fullScreenStatus->setDropShadow(dropshadow);
        }

        //set scrollbar image if necessary
        if (!strImgScrollBar.isEmpty())
        {
            _view->vScrollbar()->setImage(pSkin->getImage(strImgScrollBar));
            _view->hScrollbar()->setImage(pSkin->getImage(strImgScrollBar));
        }

        //set scrollbar indicator image if necessary
        if (!strImgScrollInd.isEmpty())
        {
            _view->vScrollbar()->setIndicatorImage(pSkin->getImage(strImgScrollInd));
            _view->hScrollbar()->setIndicatorImage(pSkin->getImage(strImgScrollInd));
        }

        //set no thumbnail image if necessary
        if (!strImgNoThumb.isEmpty())
        {
            //adjust thumb width so when scaled (non-proportional scale) and 
            // displayed onscreen, it maintains proper proportions
            bdisplay_settings displaySettings;
            int               ratioScreenWidth  = 16;
            int               ratioScreenHeight = 9;
            if (control()->getOutput(&displaySettings))
            {
                if ((displaySettings.format != bvideo_format_576p)       &&
                    (displaySettings.format != bvideo_format_1080i)      &&
                    (displaySettings.format != bvideo_format_1080i_50hz) &&
                    (displaySettings.format != bvideo_format_720p)       &&
                    (displaySettings.format != bvideo_format_720p_50hz))
                {
                    //SD format
                    //now check aspect_ratio field (only valid if format is SD)
                    if (displaySettings.aspect_ratio == bdisplay_aspect_ratio_4x3)
                    {
                        ratioScreenWidth  = 4;
                        ratioScreenHeight = 3;
                    }
                }
            }

            MImage * pImg = pSkin->getImage(strImgNoThumb);
            pImg->setWidth(pImg->width() *
                           control()->display()->view_rect.width * 
                           ratioScreenHeight / ratioScreenWidth / 
                           control()->display()->view_rect.height);

            _view->setNoThumbImage(pImg);
        }

        //set icon size and spacing for thumbnail view
        if ((iconWidth > 0) && (iconBorder > 0))
        {
            _view->setIconWidth(iconWidth, iconBorder);
        }

        _bevel->setGeometry((720-47-VIEW_WIDTH)/2, 80, VIEW_WIDTH, 340);
        _bevel->setBevel(0, MPainter::bsRaised);
        _bevel->setBackgroundColor(-1);
        _view->viewport()->setBackgroundColor(-1);
        _view->vScrollbar()->setButtons(false);
        _view->hScrollbar()->setButtons(false);

        _bevel->raise();
        _view->raise();
        _fullScreenView->raise();
        _fullScreenView->setBackgroundColor(0xff000000);
    }
}

PicturesScreen::~PicturesScreen()
{
    if (_fullScreenStatus)
    {
        delete _fullScreenStatus;
    }
    if (_fullScreenView)
    {
        delete _fullScreenView;
    }
    if (_view)
    {
        delete _view;
    }
    if (_bevel)
    {
        delete _bevel;
    }
}

void PicturesScreen::hide()
{
    control()->brutus()->setFullscreenGraphics(true);
    cleanup();
    GenericScreen::hide();
}

void PicturesScreen::show()
{
    // hide all thumbs
    _view->hide();
    // show must come first so that set focus will work
    GenericScreen::show();
    setFullScreenMode(false);
    // show picturesscreen first without any thumbs to give user 
    // instant visual feedback
    drawNow();
    // now load/show thumbnail images
    _view->show();
    readDisk(_path);
}

void PicturesScreen::setFullScreenMode(bool on)
{
    Skin * pSkin = control()->getSkin();
    if (on) {
        MMessageBox * msg = new MMessageBox(fb(), 300, 100);
        
        if (!fullScreenMode())
        {
            _indexLast = _index;
            _index = _view->currentIndex();
            
            msg->set("Loading image...", 0);
            msg->show();
            msg->drawNow();
        }
        else if (_index == -1) {
            advance(1);
        }
        
        if (!pSkin->isValid(XML_SCREEN_PICTURES))
        {
            setBackgroundColor(0xff000000);
        }
        control()->brutus()->setFullscreenGraphics((uint32_t)0xFF000000); /* black */
        _bevel->hide();
        if (_title) _title->hide();
        showStatus(NULL);
        updateFullScreenImage();
        
        if (!fullScreenMode())
        {
            _fullScreenView->clear();
        }
        _fullScreenView->show();
        _fullScreenView->focus();
        
        if (msg)
        {
            delete msg;
        }
    }
    else {
        _slideShowTimer.stop();
        
        if (!pSkin->isValid(XML_SCREEN_PICTURES))
        {
            setBackgroundColor(cfg()->getInt(Config::BACKGROUND_COLOR));
        }
        control()->brutus()->setFullscreenGraphics(true); /* background color (e.g. Brutus blue) */
        if (_title) _title->show();
        _bevel->show();
        MListViewItem *item = _view->item(_index);
        _fullScreenView->hide();
        if (item)
            item->focus();
        else
            focus();
    }
}

bool PicturesScreen::fullScreenMode() const
{
    return !_bevel->visible();
}

int PicturesScreen::totalPictures()
{
    return  _list.total();
}

const char *PicturesScreen::getMediaFileName(int idx)
{
    Picture *pic=NULL;

    if ( idx >= _list.total())
        idx = 0;
    else if ( idx < 0)
        idx = _list.total()-1;

    pic = _list.at(idx);

    return pic->filename;
}

int PicturesScreen::showPicture(const char *filename)
{
    Picture *pic=NULL;
    int idx;

    for( idx=0; idx < _list.total(); idx++ ) {
        pic = _list.at(idx);

        if( strstr( pic->filename.s(), filename ) ) break;
        else pic = NULL;
    }
    if( pic == NULL ) 
        return -1;

    _indexLast = _index;
    _index = idx;

    _bevel->hide();
    showStatus(NULL);
    updateFullScreenImage();

    _fullScreenView->show();
    _fullScreenView->focus();

    return 0;
}

int PicturesScreen::showPicture(int idx)
{
    _indexLast = _index;
    _index = idx;
    if ( idx >= _list.total())
        _index = 0;
    else if ( idx < 0)
        _index = _list.total()-1;

    _bevel->hide();
    showStatus(NULL);
    updateFullScreenImage();

    _fullScreenView->show();
    _fullScreenView->focus();

    return 0;
}

void PicturesScreen::advance(int inc)
{
    _indexLast = _index;
    _index += inc;
    if (_index >= _list.total())
        _index = 0;
    else if (_index < 0)
        _index = _list.total()-1;

    if (fullScreenMode())
    {
        updateFullScreenImage();
    }
}

#undef RAND_TRANSITION
void PicturesScreen::updateFullScreenImage()
{
    int nTransition = MPainter::eSwap;
    Picture *pic = _list.at(_index);
    if (pic)
    {
        if (_slideShowTimer.running()) 
        {
#ifdef RAND_TRANSITION
            //do random transition
            nTransition = rand() % MPainter::eLastTransition;
#else
            nTransition = (_lastTransition + 1) % MPainter::eLastTransition;
#endif

            //if hardware acceleration available, omit the boring transitions
            while (MPainter::eSwap == nTransition)
            {
#ifdef RAND_TRANSITION
                nTransition = rand() % MPainter::eLastTransition;
#else
                nTransition = (nTransition + 1) % MPainter::eLastTransition;
#endif
            }
        }

        _fullScreenView->setImage(pic, _mode, 
                                  (MPainter::TransitionMode)nTransition);
    }
    else
    {
        _fullScreenView->setImage(NULL);
    }

    _lastTransition = (MPainter::TransitionMode)nTransition;
}


static bool isJpeg(const char *filename)
{
    FILE * file    = NULL;
    bool   bResult = false;

    file = fopen(filename, "rb");

    if (file)
    {
        char buf[11];
        BKNI_Memset(buf, 0, sizeof(buf));
        fread(buf, sizeof(*buf), sizeof(buf), file);
        bResult = ((BKNI_Memcmp(&buf[6], "JFIF", 5) && 
                    BKNI_Memcmp(&buf[6], "Exif", 4)) == 0);
        fclose(file);
    }

    return bResult;
}

void PicturesScreen::addPicture(const char *name, const char *filename, int index)
{
    Picture *pic = new Picture();
    pic->name = name;
    pic->filename = filename;

    bool bLoadToMem = !(isJpeg(filename));
    
    pic->full = new MImage(app(), pic->filename, bLoadToMem);
    if (!pic->full || !pic->full->id())
        return;

    bdisplay_settings displaySettings;
    int               ratioScreenWidth  = 16;
    int               ratioScreenHeight = 9;
    if (control()->getOutput(&displaySettings))
    {
        if ((displaySettings.format != bvideo_format_576p)       &&
            (displaySettings.format != bvideo_format_1080i)      &&
            (displaySettings.format != bvideo_format_1080i_50hz) &&
            (displaySettings.format != bvideo_format_720p)       &&
            (displaySettings.format != bvideo_format_720p_50hz))
        {
            //SD format
            //now check aspect_ratio field (only valid if format is SD)
            if (displaySettings.aspect_ratio == bdisplay_aspect_ratio_4x3)
            {
                ratioScreenWidth  = 4;
                ratioScreenHeight = 3;
            }
        }
    }

    //adjust thumb width so when scaled (non-proportional scale) and displayed 
    // onscreen, it maintains proper proportions
    pic->full->setWidthThumb(pic->full->widthThumb() *
                             control()->display()->view_rect.width * 
                             ratioScreenHeight / ratioScreenWidth / 
                             control()->display()->view_rect.height);

    //adjust image width so when scaled (non-proportional scale) and displayed 
    // onscreen, it maintains proper proportions
    pic->full->setWidth(pic->full->width() *
                        control()->display()->view_rect.width * 
                        ratioScreenHeight / ratioScreenWidth / 
                        control()->display()->view_rect.height);
    
    BDBG_MSG(("add picture: %s, full:%dx%d thumb:%dx%d",
              (const char *)pic->filename, 
              pic->full->width(), pic->full->height(),
              pic->full->widthThumb(), pic->full->heightThumb()));
    addPicture(pic, index);
}

void PicturesScreen::addPicture(Picture *pic, int index)
{
#if 1
    /* This version just doesn't add the picture to the list -- we're full! */
    if (_maxPictures != -1 && _list.total() >= _maxPictures)
    {
        delete pic;
        return;
    }
#else
    /* This version deletes the last image loaded from the list AND deletes the picture file!!! */
    /* This was done for a marketing demo that helped them minimize the amount of junk files accumulating in thier pictures folder. */
    if (_maxPictures != -1 && _list.total() >= _maxPictures)
    {
        Picture *delpic = _list.at(_list.total()-1);
        if (delpic)
            deletePicture(delpic);
    }
#endif
    _list.insert(index, pic);
    
    MListViewItem *item = _view->insert(index);
    item->setIcon(pic->full);
    item->setLabel(pic->name);
    item->setTextColor(_view->textColor());
}

int PicturesScreen::deletePicture(Picture *pic)
{
    if (pic->deleteFile())
    {
        MMessageBox::info(fb(), "Unable to delete file " + pic->filename);
        return -1;
    }

    int index = _list.index(pic);
    MListViewItem *item = _view->item(index);
    assert(item);
    _view->remove(item);
    _list.remove(pic);
    delete pic;

    if (index == _view->total())
        index--;
    item = _view->item(index);
    if (item)
    {
        if (fullScreenMode())
        {
            _index = index;
            updateFullScreenImage();
        }
        else
            item->focus();
    }
    else
    {
        setFullScreenMode(false);
    }
    return 0;
}

int my_sort_cmp(MString * str1, MString * str2)
{
    //printf("compare: %s and %s = %d\n", str1->s(), str2->s(), str1->strncmp(*str2));
    int retval = str1->strncmp(*str2);
    if (retval > 0)
    {
        //MVoidList::sort() is stupid...
        retval = 1;
    }

    return retval;
}

void PicturesScreen::cleanup()
{
    _view->clear();
    _list.clear();
    _index = -1;
    _indexLast = -1;
}

int PicturesScreen::readDisk(const char *path)
{
    cleanup();

    Skin * pSkin = control()->getSkin();
    if (pSkin->isValid(XML_SCREEN_PICTURES))
    {
        MString         strImgProgressInd; 
        pSkin->getAttrValue(XML_SCREEN_PICTURES, 
                            XML_ATT_PROGRESS_IND_IMAGE,   strImgProgressInd);
    }

        DIR *dir = opendir((char *)path);
    if (!dir)
    {
        BDBG_WRN(("No pictures directory found"));
        return -1;
    }

    _view->beginUpdates();

    struct dirent *d;
    int totalcount = 0;

    //note: autodelete only works with MList::clear();
    MList<MString> listImageName(true); 

    while ((d = readdir(dir)))
    {
        if (!strcmp(d->d_name, ".") ||
            !strcmp(d->d_name, ".."))
            continue;

        listImageName.add(new MString(d->d_name));
        totalcount++;
    }

    //sort list of image names
    listImageName.sort(my_sort_cmp);

    for (int i = listImageName.total() - 1; i >= 0; i--)
    {
        MString strName(*listImageName[i]);
        int idxDot = strName.find('.', -1);
        if (idxDot > 0)
        {
            strName.truncate(idxDot);
        }
        addPicture(strName, MString(path) + "/" + *listImageName[i], 0);
    }
    listImageName.clear();

    _view->endUpdates();

    closedir(dir);

    return 0;
}

bool PicturesScreen::processCommand(int cmd)
{
    if (!app()->inModalState())
    switch (cmd)
    {
    case IND_PLAY:
        if (fullScreenMode())
        {
            showStatus("Play", 1000);
            advance(1);
        }
        setFullScreenMode(true);
        _slideShowTimer.start(SLIDESHOW_TIMER, MTimer::eForever);
        return true;
    case IND_STOP:
        setFullScreenMode(false);
        return true;
    case IND_PAUSE:
        if (fullScreenMode()) 
        {
            if (!_slideShowTimer.running())
            {
                _slideShowTimer.start(SLIDESHOW_TIMER, MTimer::eForever);
                showStatus("Play", 1000);
            }
            else
            {
                _slideShowTimer.stop();
                showStatus("Pause", 1000);
            }
            return true;
        }
        return false;
    case IND_FWD:
        if (fullScreenMode())
        {
            advance(1);
            return true;
        }
        return false;
    case IND_REW:
        if (fullScreenMode())
        {
            advance(-1);
            return true;
        }
        return false;
    }
    return false;
}

bool PicturesScreen::keyDown(int ascii, int modifiers, int scancode)
{
    if (!app()->inModalState())
    switch (ascii)
    {
    case MWKEY_ENTER: // full screen
        if (currentPicture()) {
            setFullScreenMode(!fullScreenMode());
            return true;
        }
        return false;
    case MWKEY_RIGHT:
        if (fullScreenMode())
        {
            showStatus("Next", 1000);
            advance(1);
            return true;
        }
        return false;
    case MWKEY_LEFT:
        if (fullScreenMode())
        {
            showStatus("Prev", 1000);
            advance(-1);
            return true;
        }
        return false;
    case '1': // rename
        renamePicture(currentPicture());
        return true;
    case '2': // delete
        deletePictureDeferred(currentPicture(), false);
        return true;
    case '3': // send
        sendPicture(currentPicture());
        return true;
    case '7': // read camera
        readCamera();
        return true;
    }
    return GenericScreen::keyDown(ascii, modifiers, scancode);
}

Picture *PicturesScreen::currentPicture()
{
    return _list.at(fullScreenMode()?_index:_view->currentIndex());
}

int PicturesScreen::renamePicture(Picture *pic)
{
    MString response;
    if (pic && MMessageBox::input(fb(), "New Name",170,MMessageBox::bOK|MMessageBox::bCancel,
        response) == MMessageBox::bOK)
    {
        pic->name = response;
        MListViewItem *item = _view->item(_list.index(pic));
        if (item)
            item->setLabel(response);
        return 0;
    }
    else
        return -1;
}

int PicturesScreen::deletePictureDeferred(Picture *pic, bool prompt)
{
    if (pic &&
        (!prompt || MMessageBox::show(fb(), "Delete picture from disk?") == MMessageBox::bYes))
    {
        _deletePic = pic;
        _deltimer.start(0, MTimer::eOnce);
        return 0;
    }
    else
        return -1;
}

int PicturesScreen::sendPicture(Picture *pic)
{
    return -1;
}

void PicturesScreen::onClick(MListView *sender)
{
    int i = _view->currentIndex();
    if (i != _index)
        advance(i - _index);
    setFullScreenMode(true);
}

void PicturesScreen::setDrawImageMode(MPainter::DrawImageMode mode)
{
    _mode = mode;
    if (fullScreenMode())
        repaint();
}

void PicturesScreen::onTimeout(MTimer *sender)
{
    if (sender == &_slideShowTimer) {
        advance(1);
    }
    else if (sender == &_deltimer) {
        deletePicture(_deletePic);
    }
    else if (sender == &_statusTimer) {
        _fullScreenStatus->hide();
    }

#if BLUETOOTH_SUPPORT
    if (sender == &_idleTimer) {
        bt_ui(this);
    }
#endif
}

void PicturesScreen::showStatus(const char *text, int timeout)
{
    if (text)
    {
        _fullScreenStatus->setText(text);
        _fullScreenStatus->show();
        _fullScreenStatus->drawNow();
        if (timeout == -1)
            _statusTimer.stop();
        else
            _statusTimer.start(timeout);
    }
    else
    {
        _statusTimer.stop();
        _fullScreenStatus->hide();
    }
}

#define CAMERA_MOUNTPOINT "/root/mnt/camera"
#define CAMERA_DIR CAMERA_MOUNTPOINT "/dcim/100olymp"

int PicturesScreen::readCamera()
{
    int count = 0;
    
    MMessageBox *msg = new MMessageBox(NULL, 300,100);
    msg->set("Connecting to camera...", 0);
    msg->show();
    msg->drawNow();

    // mount, copy, delete, umount
    if (system("mount -t vfat /dev/sda1 " CAMERA_MOUNTPOINT))
    {
        delete msg;
        MMessageBox::info(fb(), "Unable to find camera");
        return 0;
    }

    msg->set("Loading pictures...", 0);
    msg->drawNow();
    _view->beginUpdates();

    DIR *dir = opendir(CAMERA_DIR);
    if (!dir)
    {
        BDBG_WRN(("No pictures directory found"));
        return 0;
    }
    struct dirent *d;
    while ((d = readdir(dir)))
    {
        if (!strcmp(d->d_name, ".") ||
            !strcmp(d->d_name, ".."))
            continue;

        char buf[300];
        MString file, name;
        newPictureName(name, file);
        sprintf(buf, "mv %s/%s %s", CAMERA_DIR, d->d_name, (const char *)file);

        msg->set(name, 0);
        msg->drawNow();

        if (!system(buf))
        {
            addPicture(name, file, 0);
            count++;
        }
    }
    closedir(dir);
    system("umount " CAMERA_MOUNTPOINT);
    delete msg;

    char buf[40];
    sprintf(buf, "%d picture%s loaded", count, count!=1?"s":"");
    MMessageBox::show(fb(), buf, 0, 0, 1500);

    _view->endUpdates();
    setFullScreenMode(false);
    //_view->focus();

    return count;
}

void PicturesScreen::newPictureName(MString &name, MString &file)
{
    // I first made "lasttry" a static, but now I think it's
    // best to count from 1 every time and fill in the holes.
    int lasttry = 0;
    struct stat s;
    char buf[100];
    do
    {
        sprintf(buf, "%s/pic%d.jpg", picturePath(), ++lasttry);
    } while (!stat(buf, &s));

    // capture the full filename
    file = buf;

    // and the base filename
    sprintf(buf, "pic%d.jpg", lasttry);
    name = buf;
}
