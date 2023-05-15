/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: info_video.cpp $
 * $brcm_Revision: 45 $
 * $brcm_Date: 3/20/12 10:33a $
 *
 * Module Description: GUI for frontend state
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/info/info_video.cpp $
 * 
 * 45   3/20/12 10:33a erickson
 * SW7425-2664: remove old option. just expose the sharpness control for
 * all.
 * 
 * 44   11/16/10 12:17p jtna
 * SW7405-4998: more 3D refactoring
 * 
 * 43   10/13/10 7:13p jtna
 * SW7401-4456: 2D/3D switch based on EDID
 * 
 * 42   7/9/10 12:12p jtna
 * SW7405-3999: disable 3DTV GUI items when user starts Brutus in 2D mode
 * 
 * 41   6/25/10 6:16p jtna
 * SW7405-4420: handle 3dtv source/output permutations better
 * 
 * 40   5/12/10 5:53p jtna
 * SW7405-3999: added 3DTV auto mode
 * 
 * 39   5/10/10 11:13a jtna
 * SW7420-527: fix mapping of sharpness slider
 *
 * 38   4/22/10 3:53p jtna
 * SW7405-3999: add 3DTV 2D/3D conversion option
 * 
 * 37   4/8/10 2:20p jtna
 * SW7420-466: more debug messages for video sliders
 * 
 * 36   3/30/10 8:06p jtna
 * SW7420-527: fix y-position
 * 
 * 35   3/30/10 7:49p jtna
 * SW7420-527: add support for libtune uiControl and rearrange GUI items
 * 
 * 34   3/29/10 10:37a erickson
 * SW7405-3625: add 3DTV ui options
 *
 * 33   3/26/10 5:35p erickson
 * SW7405-3625: use MScrollView to create real estate for more video
 * controls. moved BCHP_CHIP lists to brutus_app.h.
 *
 * 32   3/1/10 6:52p jtna
 * SW7420-527: initial Brutus libtune integration
 *
 * 31   11/3/09 2:09p mward
 * SW7125-51:  Enable sharpness control for 7125.
 *
 * 30   9/1/09 5:28p tokushig
 * SW7335-536: Merge to main
 *
 * BRUTUS_97405_SPLIT_SCREEN_PIC_ENHANCE/1   9/1/09 10:22a tokushig
 * SW7335-536: Modified split screen picture quality feature to operate
 * with the info menu hidden.  This is not only more consistent with how
 * the other info menu controls work, but makes it easier/faster to
 * change the split screen setting during a demo.
 *
 * 29   8/19/09 10:56a jrubio
 * PR55232: add 7342/7340 support
 *
 * 28   7/21/09 10:04a jrubio
 * PR56906: Add Sharpness button to 7335/7325
 *
 * 27   3/10/09 7:08p nickh
 * PR52746: Add Picture controls for 7420
 *
 * 26   2/20/09 1:44p erickson
 * PR52355: add ANR support
 *
 * 25   4/3/08 11:54a jtna
 * PR32279: Rework splitscreen demo mode
 *
 * PR32279/1   4/2/08 11:47a jtna
 * PR32279: Rework splitscreen demo mode
 *
 * 24   3/17/08 3:18p erickson
 * PR40307: remove old, unused code before adding new
 *
 * 23   5/24/07 5:02p mward
 * PR28583: Initialize unused controls to NULL.
 *
 * 22   2/27/07 10:11a ahulse
 * PR28036: Remove key repeat on video enhancement sliders, adjust video
 * info offset, fix whitespace
 *
 * 21   2/23/07 3:42p jgarrett
 * PR 28036: Adding extended picture control sliders
 *
 * 20   1/11/07 4:48p ahulse
 * PR26866: Add DNR demo window selection to brutus. Need to define
 * EXPOSE_SETTOPAPI_INTERNALS to activate.
 *
 * 19   11/30/06 7:26p jgarrett
 * PR 24698: Using bconfig variables for dnr control and adding separate
 * controls for bnr,mnr,dcr.
 *
 * 18   11/29/06 11:13a erickson
 * PR24374: clarified PEP and CSC
 *
 * 17   11/21/06 2:35p vle
 * PR 26086: return after enable/disable 3D Comb Filter
 *
 * 16   11/4/06 4:26p dlwin
 * PR 25450: 97398 does not support VDEC, therefore 3D-Comb needs to be
 * disabled.
 *
 * 15   11/3/06 12:14p vle
 * PR 24767: "Reset PEP" should also work on display1 in HDSD_SINGLE mode.
 *
 * 14   10/25/06 11:16a jgarrett
 * PR 25095: Removing DCR controls for 7400A0 until proper support has
 * been added in stetop api for independent DNR controls.
 *
 * 13   8/3/06 12:06p erickson
 * PR21941: remove Sharpness except for 3560. fixed B_TOTAL_VIDEO_CONTROLS based
 * code.
 *
 * 12   8/2/06 4:45p erickson
 * PR21941: moved DCR control from Display to Video panel, converted to
 * slider
 *
 * 11   8/1/06 9:49a erickson
 * PR23130: allow 3D comb only for VDEC systems
 *
 * 10   6/6/06 6:08p jgarrett
 * PR 21965: PEP controls should affect both displays in HDSD_SINGLE
 *
 * 9   5/31/06 4:20p tokushig
 * PR21891: Merging updated picture viewer into mainline.
 *
 * SanDiego_Brutus_Skin/7   5/16/06 11:33a tokushig
 * merge from main
 *
 * 8   4/17/06 5:09p jgarrett
 * PR 20951: Merging skinned UI into mainline
 *
 * SanDiego_Brutus_Skin/6   4/14/06 8:05p tokushig
 * moved info screen location to avoid overscan cutoff
 *
 * SanDiego_Brutus_Skin/5   3/30/06 10:52a tokushig
 * remove prerendering of icon if magnum based.  since we are using the
 * blitter to display pixmaps, any pixmap with alpha values cannot be
 * displayed properly.  legacy platforms will operate as usual
 *
 * SanDiego_Brutus_Skin/4   2/23/06 2:27p tokushig
 * merge from 97400 prerel label
 *
 * SanDiego_Brutus_Skin/3   2/15/06 10:50a tokushig
 * add bevel style for mlabel to info skin
 *
 * SanDiego_Brutus_Skin/2   2/14/06 12:51p tokushig
 * allowed buttons to use images from skin
 *
 * SanDiego_Brutus_Skin/1   2/13/06 4:40p tokushig
 * added partial skin ability.  note that this is primarily for cable show
 * demo purposes and will be expanded to be fully skinnable in the
 * future.
 *
 * 6   9/15/05 6:13p erickson
 * PR17108: handle systems with no windows
 *
 * 5   8/25/05 3:27p erickson
 * PR16819: named Config to Video, and moved BTSC SAP to new Audio panel
 *
 * 4   4/18/05 10:56a erickson
 * PR14593: prerender images for faster draw time on slow platforms
 *
 * 3   3/31/05 6:12p bandrews
 * PR14646: Moved default values to separate header file
 *
 * 2   3/29/05 6:49p ikizyan
 * PR14646: Adding a sharpness control
 *
 * 1   2/7/05 8:07p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/6   2/4/05 10:48a erickson
 * PR12728: cleanup for -pedantic warnings and errors
 *
 * Irvine_BSEAVSW_Devel/5   11/23/04 10:18a erickson
 * PR13336: add msg for picture control
 *
 * Irvine_BSEAVSW_Devel/4   11/23/04 9:54a erickson
 * PR13336: added picture control ui
 *
 * Irvine_BSEAVSW_Devel/3   11/19/04 4:03p erickson
 * PR13304: 3d comb enabled by default and shown on ui
 *
 * Irvine_BSEAVSW_Devel/2   11/19/04 3:28p erickson
 * PR13014: make sure we retune when changing SAP option
 *
 * Irvine_BSEAVSW_Devel/1   10/15/04 10:51a erickson
 * PR13014: infobar rework
 *
 ***********************************************************************/
#include "info_video.h"
#include "control.h"
#include "cfgsettings.h"
#include "mbarcontrol.h"
#include "marrowbar.h"
#include "pep_config.h"
#include "mscrollview.h"
#include <bstd.h>
BDBG_MODULE(info_config);

#define BNR_TAG (10)
#define MNR_TAG (11)
#define DCR_TAG (12)
#define DYNAMIC_CONTRAST_TAG (13)
#define FLESH_TONE_TAG (14)
#define GREEN_STRETCH_TAG (15)
#define BLUE_STRETCH_TAG (16)
#define ANR_TAG (17)
#define SPLIT_SCREEN_TAG (18)
#define B_3DTV_ZNORM_TAG (19)

/* NOTE:
Color space conversion is available on all BVN-based chips.
It accomplishes brightness, contrast, hue, saturation.

PEP (picture enhancement processing) is available on DTV chips (e.g. 3560).
It accomplishes white balance and contrast stretch.

Because of some misunderstanding, some of the code says "PEP" but applies to CSC.
*/

#define PIXMAPPATH (MString(infoBar->control()->cfg()->get(Config::IMAGES_PATH)) + "/info/")

static void ApplyPictureControlValue(int tag, int value, Control *control, bdecode_window_t window);

class VideoInfoPage : public InfoBarPopup, public MButtonEventHandler,
    public ControlEventHandler {
public:
    VideoInfoPage(InfoBar *infoBar, MWidget *parent, const MRect &rect);
    void update() {}
    void updatePictureControl(int tag, int value = -1);

    // overrides
    void show();
    void onClick(MButton *button);
    void onControlEvent(Control *control, Control::Event event);
protected:
    MScrollView *_scrollview;
    MCheckBox *_combFilterEnabled;

#define B_TOTAL_PEP_CONTROLS 5

    MBarRange _pepValues[B_TOTAL_PEP_CONTROLS];
    MBarButton *_pep[B_TOTAL_PEP_CONTROLS];

    MArrowBar *_3dtvOutputMode;

    MBarRange _3dtvZnormValue;
    MBarButton *_3dtvZnorm;

    MCheckBox * _3dSource;

    MBarRange _bnrValue;
    MBarButton *_bnr;

    MBarRange _mnrValue;
    MBarButton *_mnr;

    MBarRange _dcrValue;
    MBarButton *_dcr;

    MBarRange _anrValue;
    MBarButton *_anr;

    MBarControl *_pictureControl;
    MPushButton *_resetPep;

    MBarRange _splitScreenValue;
    MBarButton *_splitScreen;

    MBarRange _dynamicContrastValue;
    MBarButton *_dynamicContrast;

    MBarRange _fleshToneValue;
    MBarButton *_fleshTone;

    MBarRange _greenStretchValue;
    MBarButton *_greenStretch;

    MBarRange _blueStretchValue;
    MBarButton *_blueStretch;

    MArrowBar*_libtuneUsermode;

    void updateUi();
};

static const char *pepLabel[] = {
    "Contrast", "Saturation", "Hue", "Brightness", "Sharpness"
};

static void ApplyPictureControlValue(int tag, int value, Control *control, bdecode_window_t window)
{
    /* handle tags which don't apply to bdecode_window_settings first */
    switch (tag) {
    case B_3DTV_ZNORM_TAG:
        control->brutus()->set3DTVZnorm(value);
        return;
    default:
        break;
    }

    /* the rest apply to bdecode_window_settings */
    bdecode_window_settings ws;
    bdecode_window_get(window, &ws);
    switch (tag) {
    case 0: ws.contrast = value; break;
    case 1: ws.saturation = value; break;
    case 2: ws.hue = value; break;
    case 3: ws.brightness = value; break;
    case 4: ws.sharpness = value; break;
    case MNR_TAG: ws.mnr_level = (bwindow_dnr_level)value; break;
    case BNR_TAG: ws.bnr_level = (bwindow_dnr_level)value; break;
    case DCR_TAG: ws.dcr_level = (bwindow_dnr_level)value; break;
    case ANR_TAG: ws.anr_level = (bwindow_dnr_level)value; break;
    case FLESH_TONE_TAG: ws.auto_flesh_tone = (unsigned)value; break;
    case GREEN_STRETCH_TAG: ws.green_stretch = (unsigned)value; break;
    case BLUE_STRETCH_TAG: ws.blue_stretch = (unsigned)value; break;
    case DYNAMIC_CONTRAST_TAG: ws.dynamic_contrast = (unsigned)value; break;
    case SPLIT_SCREEN_TAG:
        switch (value)
        {
        case 0: ws.splitscreen_mode = bwindow_splitscreen_mode_left; break;
        case 1: ws.splitscreen_mode = bwindow_splitscreen_mode_wholescreen; break;
        case 2: ws.splitscreen_mode = bwindow_splitscreen_mode_right; break;
        default: ws.splitscreen_mode = bwindow_splitscreen_mode_wholescreen; break;
        }
        break;
    default: BDBG_ASSERT(0); return;
    }
    BDBG_MSG(("con=%6d, sat=%6d, hue=%6d, bri=%6d, sha=%6d",
        ws.contrast, ws.saturation, ws.hue, ws.brightness, ws.sharpness));
    BDBG_MSG(("bnr=%d, mnr=%d, dcr=%d, anr=%d, split=%d, dcon=%3d, flesh=%d, green=%d, blue=%d",
        ws.bnr_level, ws.mnr_level, ws.dcr_level, ws.anr_level, ws.splitscreen_mode, 
        ws.dynamic_contrast, ws.auto_flesh_tone, ws.green_stretch, ws.blue_stretch));
    bdecode_window_set(window, &ws);
}

/**
Receive a callback from an MBarControl and apply the change
**/
static void pictureControlCallback(MBarControl *control, void *context)
{
    VideoInfoPage *page = (VideoInfoPage *)context;
    bdecode_window_t window = page->control()->window();

    if (!window) return;

    ApplyPictureControlValue(control->tag(), control->value(), page->control(), window);
    page->updatePictureControl(control->tag(), control->value());

    // Check second window in HDSD_SINGLE
    if ( page->control()->cfg()->getBool(Config::HDSD_SINGLE) )
    {
        int window_num = page->control()->currentDecode() + 2; /* TODO: + 2 logic should be refactored */
        window = page->control()->window(window_num);
        if ( window )
            ApplyPictureControlValue(control->tag(), control->value(), page->control(), window);
    }

#ifdef BRUTUS_LIBTUNE_SUPPORT
    if (control->tag() == 4) { /* sharpness */
        page->control()->Libtune_SetUiControlValue((control->value()+32768)*10/65536); /* map to 10 steps, range 0 to 9 */
        page->control()->Libtune_TuneWindows();
    }   
#endif
}

/**
Update the pictureControl window based on given tag and value*
value is an optional paramter
 **/
void VideoInfoPage::updatePictureControl(int tag, int value)
{
    switch (tag)
    {
    case B_3DTV_ZNORM_TAG:
        _pictureControl->setText("3DTV Z-norm");
        break;
    case MNR_TAG:
        _pictureControl->setText("MNR");
        break;
    case BNR_TAG:
        _pictureControl->setText("BNR");
        break;
    case DCR_TAG:
        _pictureControl->setText("DCR");
        break;
    case ANR_TAG:
        _pictureControl->setText("ANR");
        break;
    case FLESH_TONE_TAG:
        _pictureControl->setText("Auto Flesh Tone");
        break;
    case GREEN_STRETCH_TAG:
        _pictureControl->setText("Green Stretch");
        break;
    case BLUE_STRETCH_TAG:
        _pictureControl->setText("Blue Stretch");
        break;
    case DYNAMIC_CONTRAST_TAG:
        _pictureControl->setText("Dynamic Contrast");
        break;
    case SPLIT_SCREEN_TAG:
        if (0 > value)
        {
            /* no value given so use currently saved value */
            value = _splitScreenValue.value;
        }

        switch(value)
        {
        case 0: _pictureControl->setText("Left Side On"); break;
        case 1: _pictureControl->setText("Whole Screen"); break;
        case 2: _pictureControl->setText("Right Side On"); break;
        default:
            BDBG_WRN(("Invalid SPLIT_SCREEN_TAG value! Defaulting to whole screen"));
            _pictureControl->setText("Whole Screen");
            break;
        }
        break;
    default:
        break;
    }
}

void VideoInfoPage::updateUi()
{
    bdecode_window_t window = control()->window();
    bdecode_window_settings ws;

    if (!window) return;
    bdecode_window_get(window, &ws);
    _pepValues[0].value = ws.contrast;
    _pepValues[1].value = ws.saturation;
    _pepValues[2].value = ws.hue;
    _pepValues[3].value = ws.brightness;
    _pepValues[4].value = ws.sharpness;
    for (int i=0;i<B_TOTAL_PEP_CONTROLS;i++) {
        _pepValues[i].low = -32768;
        _pepValues[i].high = 32767;
        _pep[i]->set(&_pepValues[i]);
    }

    if (_pictureControl->tag() < B_TOTAL_PEP_CONTROLS) {
        _pictureControl->set(&_pepValues[_pictureControl->tag()], (65536/50) /* 50 steps */);
    }

    _3dtvZnormValue.value = control()->display()->s3dTv.znorm;
    _3dtvZnormValue.low = -20;
    _3dtvZnormValue.high = 20;
    _3dtvZnorm->set(&_3dtvZnormValue);

    _3dSource->setChecked(control()->display()->s3dTv.source > Brutus3DTVMode_eOff);
    if (control()->brutus()->display()->s3dTv.automode) {
        _3dtvOutputMode->setCurrent(3);
    }
    else {
        switch (control()->brutus()->display()->s3dTv.output) {
            default: 
            case Brutus3DTVMode_eOff: _3dtvOutputMode->setCurrent(0); break;
            case Brutus3DTVMode_eSBS: _3dtvOutputMode->setCurrent(1); break;
            case Brutus3DTVMode_eOU: _3dtvOutputMode->setCurrent(2); break;
        }
    }

    if ( _bnr )
    {
        _bnrValue.value = ws.bnr_level;
        _bnrValue.low = 0;
        _bnrValue.high = bwindow_dnr_level_max;
        _bnr->set(&_bnrValue);
    }

    if ( _mnr )
    {
        _mnrValue.value = ws.mnr_level;
        _mnrValue.low = 0;
        _mnrValue.high = bwindow_dnr_level_max;
        _mnr->set(&_mnrValue);
    }

    if ( _dcr )
    {
        _dcrValue.value = ws.dcr_level;
        _dcrValue.low = 0;
        _dcrValue.high = bwindow_dnr_level_max;
        _dcr->set(&_dcrValue);
    }

    if ( _anr )
    {
        _anrValue.value = ws.anr_level;
        _anrValue.low = 0;
        _anrValue.high = bwindow_dnr_level_max;
        _anr->set(&_anrValue);
    }

    if ( _fleshTone )
    {
        _fleshToneValue.value = ws.auto_flesh_tone;
        _fleshToneValue.low = 0;
        _fleshToneValue.high = 4;
        _fleshTone->set(&_fleshToneValue);
    }

    if ( _greenStretch )
    {
        _greenStretchValue.value = ws.green_stretch;
        _greenStretchValue.low = 0;
        _greenStretchValue.high = 4;
        _greenStretch->set(&_greenStretchValue);
    }

    if ( _blueStretch )
    {
        _blueStretchValue.value = ws.blue_stretch;
        _blueStretchValue.low = 0;
        _blueStretchValue.high = 4;
        _blueStretch->set(&_blueStretchValue);
    }

    if ( _dynamicContrast )
    {
        _dynamicContrastValue.value = ws.dynamic_contrast;
        _dynamicContrastValue.low = 0;
        _dynamicContrastValue.high = 256;
        _dynamicContrast->set(&_dynamicContrastValue);
    }

    if ( _splitScreen )
    {
        switch ( ws.splitscreen_mode )
        {
        case bwindow_splitscreen_mode_left:            _splitScreenValue.value = 0; break;
        case bwindow_splitscreen_mode_wholescreen:     _splitScreenValue.value = 1; break;
        case bwindow_splitscreen_mode_right:           _splitScreenValue.value = 2; break;
        default: bwindow_splitscreen_mode_wholescreen: _splitScreenValue.value = 1; break;
        }
        _splitScreenValue.low = 0;
        _splitScreenValue.high = 2;
        _splitScreen->set(&_splitScreenValue);
    }

    switch ( _pictureControl->tag() )
    {
    case B_3DTV_ZNORM_TAG:
        _pictureControl->set(&_3dtvZnormValue, 1 /* increment through every value */);
        break;
    case MNR_TAG:
        _pictureControl->set(&_mnrValue, 1 /* increment through every value */);
        break;
    case BNR_TAG:
        _pictureControl->set(&_bnrValue, 1 /* increment through every value */);
        break;
    case DCR_TAG:
        _pictureControl->set(&_dcrValue, 1 /* increment through every value */);
        break;
    case ANR_TAG:
        _pictureControl->set(&_anrValue, 1 /* increment through every value */);
        break;
    case FLESH_TONE_TAG:
        _pictureControl->set(&_fleshToneValue, 1 /* increment through every value */);
        break;
    case GREEN_STRETCH_TAG:
        _pictureControl->set(&_greenStretchValue, 1 /* increment through every value */);
        break;
    case BLUE_STRETCH_TAG:
        _pictureControl->set(&_blueStretchValue, 1 /* increment through every value */);
        break;
    case DYNAMIC_CONTRAST_TAG:
        _pictureControl->set(&_dynamicContrastValue, 32 /* 9 steps (0..256) */);
        break;
    case SPLIT_SCREEN_TAG:
        _pictureControl->set(&_splitScreenValue, 1 /* increment through every value */);
        break;
    default:
        break;
    }
}

void VideoInfoPage::onControlEvent(Control *control, Control::Event event)
{
    if (event == Control::evMoveWindowControl) {
        updateUi();
    }

    /* 3DTV support uses this event to update the ui (e.g. on a hdmi hotplug) */
    if (event == Control::evChangeChannel) {
        updateUi();
    }
}

#define CONTROL_HEIGHT 28

VideoInfoPage::VideoInfoPage(InfoBar *infoBar, MWidget *parent, const MRect &rect) :
    InfoBarPopup(infoBar, parent, rect)
{
    setBackgroundColor(style()->color(MStyle::BUTTON_FACE));
    setBorder(3);
    setBevel(3);

    /* _pictureControl is a general purpose control for adjusting levels while hiding the info panel */
    _pictureControl = new MBarControl(topLevelWidget(),
        MRect((640-200)/2,330,200,50), NULL);
    _pictureControl->addHandler(this);
    _pictureControl->hide();
    _pictureControl->setCallback(pictureControlCallback, this);

    control()->addControlEventHandler(this);

    /* all controls are placed on an MScrollView so we can get more real estate */
    _scrollview = new MScrollView(this, MRect(0,10,rect.width(),rect.height()-50), "scroll");
    new MLabel(this, MRect(55,rect.height()-25,140,20), "Scroll for more"); /* simplistic scroll UI */

    int y = 0;

#if B_HAS_VDEC
    _combFilterEnabled = new MCheckBox(_scrollview->viewport(), MRect(10,y,200,CONTROL_HEIGHT), "3D Comb Filter");
    _combFilterEnabled->addHandler(this);
    y += CONTROL_HEIGHT;
#endif

    for (int i=0;i<B_TOTAL_PEP_CONTROLS;i++) {
        _pep[i] = new MBarButton(_scrollview->viewport(), MRect(10,y,250,CONTROL_HEIGHT), pepLabel[i]);
        _pep[i]->addHandler(this);
        _pep[i]->setTag(i);
        y += CONTROL_HEIGHT;
    }

    /* libtune arrowbar */
    _libtuneUsermode = new MArrowBar(_scrollview->viewport(), MRect(10,y,150,CONTROL_HEIGHT));
    _libtuneUsermode->addHandler(this);
    _libtuneUsermode->addItem("Tune: Std");
    _libtuneUsermode->addItem("Tune: Vivid");
    _libtuneUsermode->setCurrent(0);
    _libtuneUsermode->hide();
#ifdef BRUTUS_LIBTUNE_SUPPORT
    _libtuneUsermode->show();
    y += CONTROL_HEIGHT;
#endif

    /* reset button */
    _resetPep = new MPushButton(_scrollview->viewport(), MRect(10,y,150,CONTROL_HEIGHT), "Reset Controls");
    _resetPep->addHandler(this);
    y += CONTROL_HEIGHT;

    /* 3DTV */
    _3dtvOutputMode = new MArrowBar(_scrollview->viewport(), MRect(10,y,150,CONTROL_HEIGHT));
    _3dtvOutputMode->addHandler(this);
    _3dtvOutputMode->addItem("3DTV: Off");
    _3dtvOutputMode->addItem("3DTV: SBS");
    _3dtvOutputMode->addItem("3DTV: OU");
    _3dtvOutputMode->addItem("3DTV: Auto");

    if (control()->display()->s3dTv.automode) {
        _3dtvOutputMode->setCurrent(3);
    }
    else {
        switch (control()->brutus()->display()->s3dTv.output) {
            default: 
            case Brutus3DTVMode_eOff: _3dtvOutputMode->setCurrent(0); break;
            case Brutus3DTVMode_eSBS: _3dtvOutputMode->setCurrent(1); break;
            case Brutus3DTVMode_eOU: _3dtvOutputMode->setCurrent(2); break;
        }
    }   
    if (control()->display()->s3dTv.output==Brutus3DTVMode_eOff && !control()->display()->s3dTv.automode) {
        _3dtvOutputMode->setEnabled(false);
    }

    y += CONTROL_HEIGHT;

    _3dtvZnorm = new MBarButton(_scrollview->viewport(), MRect(10,y,250,CONTROL_HEIGHT), "3DTV Z-norm");
    _3dtvZnorm->addHandler(this);
    _3dtvZnorm->setTag(B_3DTV_ZNORM_TAG);
    if (control()->display()->s3dTv.output==Brutus3DTVMode_eOff && !control()->display()->s3dTv.automode) {
        _3dtvZnorm->setEnabled(false);
    }
    y += CONTROL_HEIGHT;

    _3dSource = new MCheckBox(_scrollview->viewport(), MRect(5,y,150,CONTROL_HEIGHT), "3D Source");
    _3dSource->setChecked(control()->display()->s3dTv.source > Brutus3DTVMode_eOff);
    _3dSource->addHandler(this);
    if (control()->display()->s3dTv.automode) {
        _3dSource->setEnabled(false);
    }
    y += CONTROL_HEIGHT;

    /* default all to off */
    _bnr = _mnr = _dcr = _anr = _splitScreen = _dynamicContrast = NULL;
    _fleshTone = _greenStretch = _blueStretch = NULL;

    if ( bconfig->features.dnr_bnr )
    {
        _bnr = new MBarButton(_scrollview->viewport(), MRect(10,y,250,28), "BNR");
        _bnr->addHandler(this);
        _bnr->setTag(BNR_TAG);
        y += CONTROL_HEIGHT;
    }

    if ( bconfig->features.dnr_mnr )
    {
        _mnr = new MBarButton(_scrollview->viewport(), MRect(10,y,250,28), "MNR");
        _mnr->addHandler(this);
        _mnr->setTag(MNR_TAG);
        y += CONTROL_HEIGHT;
    }

    if ( bconfig->features.dnr_dcr )
    {
        _dcr = new MBarButton(_scrollview->viewport(), MRect(10,y,250,28), "DCR");
        _dcr->addHandler(this);
        _dcr->setTag(DCR_TAG);
        y += CONTROL_HEIGHT;
    }

    if ( bconfig->features.anr )
    {
        _anr = new MBarButton(_scrollview->viewport(), MRect(10,y,250,28), "ANR");
        _anr->addHandler(this);
        _anr->setTag(ANR_TAG);
        y += CONTROL_HEIGHT;
    }

    if ( bconfig->features.dnr_bnr || bconfig->features.dnr_mnr || bconfig->features.dnr_dcr || bconfig->features.anr || bconfig->features.lab || bconfig->features.cab )
    {
        _splitScreen = new MBarButton(_scrollview->viewport(), MRect(10,y,250,28), "Split Screen Mode");
        _splitScreen->addHandler(this);
        _splitScreen->setTag(SPLIT_SCREEN_TAG);
        y += CONTROL_HEIGHT;
    }
    else
    {
        BDBG_WRN(("Split Screen Demo Mode disabled"));
    }

    if ( bconfig->features.lab )
    {
        _dynamicContrast = new MBarButton(_scrollview->viewport(), MRect(10,y,250,28), "Dynamic Contrast");
        _dynamicContrast->addHandler(this);
        _dynamicContrast->setTag(DYNAMIC_CONTRAST_TAG);
        y += CONTROL_HEIGHT;
    }

    if ( bconfig->features.cab )
    {
        _fleshTone = new MBarButton(_scrollview->viewport(), MRect(10,y,250,28), "Flesh Tone");
        _fleshTone->addHandler(this);
        _fleshTone->setTag(FLESH_TONE_TAG);
        y += CONTROL_HEIGHT;

        _greenStretch = new MBarButton(_scrollview->viewport(), MRect(10,y,250,28), "Green Stretch");
        _greenStretch->addHandler(this);
        _greenStretch->setTag(GREEN_STRETCH_TAG);
        y += CONTROL_HEIGHT;

        _blueStretch = new MBarButton(_scrollview->viewport(), MRect(10,y,250,28), "Blue Stretch");
        _blueStretch->addHandler(this);
        _blueStretch->setTag(BLUE_STRETCH_TAG);
        y += CONTROL_HEIGHT;
    }

    _scrollview->setVScrollBarMode(MScrollView::AlwaysOff);
    _scrollview->setHScrollBarMode(MScrollView::AlwaysOff);

    Skin * pSkin = infoBar->control()->getSkin();
    if (pSkin->isValid(XML_SCREEN_TV))
    {
        //get list box attributes from skin
        MString         screenColor;
        MString         imgFocus;
        MString         textColor;
        MString         textColorFocus;
        MString         buttonOn;
        MString         buttonOff;
        MString         buttonDown;
        MString         bevelStyle;
        uint32_t        bevelWidth       = 0;

        pSkin->getAttrValue(XML_SCREEN_TV,
                            XML_ATT_INFO_COLOR,              screenColor);
        pSkin->getAttrValue(XML_SCREEN_TV,
                            XML_ATT_INFO_IMAGE_FOCUS,        imgFocus);
        pSkin->getAttrValue(XML_SCREEN_TV,
                            XML_ATT_INFO_TEXT_COLOR,         textColor);
        pSkin->getAttrValue(XML_SCREEN_TV,
                            XML_ATT_INFO_TEXT_COLOR_FOCUS,   textColorFocus);
        pSkin->getAttrValue(XML_SCREEN_TV,
                            XML_ATT_INFO_BUTTON_IMAGE_ON,    buttonOn);
        pSkin->getAttrValue(XML_SCREEN_TV,
                            XML_ATT_INFO_BUTTON_IMAGE_OFF,   buttonOff);
        pSkin->getAttrValue(XML_SCREEN_TV,
                            XML_ATT_INFO_BUTTON_IMAGE_DOWN,  buttonDown);
        pSkin->getAttrValue(XML_SCREEN_TV,
                            XML_ATT_INFO_BEVEL_WIDTH,        bevelWidth);
        pSkin->getAttrValue(XML_SCREEN_TV,
                            XML_ATT_INFO_BEVEL_STYLE,        bevelStyle);

        //set panel background color if necessary
        if (!screenColor.isEmpty())
        {
            int32_t nCol = pSkin->getColorValue(screenColor.s());
            setBackgroundColor(nCol);
            _resetPep->setBackgroundColor(nCol);
            _libtuneUsermode->setBackgroundColor(nCol);
        }

        //set focus image if necessary
        if (!imgFocus.isEmpty())
        {
            //setImageFocus(pSkin->getImage(imgFocus.s()));
        }

        //set text color if necessary
        if (!textColor.isEmpty())
        {
            setTextColor(pSkin->getColorValue(textColor.s()));
            _pictureControl->setTextColor(pSkin->getColorValue(textColor.s()));
#if B_HAS_VDEC
            _combFilterEnabled->setTextColor(pSkin->getColorValue(textColor.s()));
#endif
            _resetPep->setTextColor(pSkin->getColorValue(textColor.s()));
            for (int i=0;i<B_TOTAL_PEP_CONTROLS;i++)
            {
                _pep[i]->setTextColor(pSkin->getColorValue(textColor.s()));
            }

            if (_libtuneUsermode)
                _libtuneUsermode->setTextColor(pSkin->getColorValue(textColor.s()));

            if ( _bnr )
                _bnr->setTextColor(pSkin->getColorValue(textColor.s()));

            if ( _mnr )
                _mnr->setTextColor(pSkin->getColorValue(textColor.s()));

            if ( _dcr )
                _dcr->setTextColor(pSkin->getColorValue(textColor.s()));

            if ( _anr )
                _anr->setTextColor(pSkin->getColorValue(textColor.s()));

            if ( _fleshTone )
                _fleshTone->setTextColor(pSkin->getColorValue(textColor.s()));

            if ( _greenStretch )
                _greenStretch->setTextColor(pSkin->getColorValue(textColor.s()));

            if ( _blueStretch )
                _blueStretch->setTextColor(pSkin->getColorValue(textColor.s()));

            if ( _dynamicContrast )
                _dynamicContrast->setTextColor(pSkin->getColorValue(textColor.s()));

            if ( _splitScreen )
                _splitScreen->setTextColor(pSkin->getColorValue(textColor.s()));

        }

        //set focus text color if necessary
        if (!textColorFocus.isEmpty())
        {
            int32_t nCol = pSkin->getColorValue(textColorFocus.s());
            setTextColorFocus(nCol);
            _resetPep->setTextColorFocus(nCol);
            _libtuneUsermode->setTextColorFocus(nCol);
        }

        MImage * imgButtonOn   = NULL;
        MImage * imgButtonOff  = NULL;
        MImage * imgButtonDown = NULL;
        //set button focus image if necessary
        if (!buttonOn.isEmpty())
        {
            imgButtonOn = pSkin->getImage(buttonOn.s());
        }

        //set button unfocus image if necessary
        if (!buttonOff.isEmpty())
        {
            imgButtonOff = pSkin->getImage(buttonOff.s());
        }

        //set button pressed image if necessary
        if (!buttonDown.isEmpty())
        {
            imgButtonDown = pSkin->getImage(buttonDown.s());
        }

        if (imgButtonOn || imgButtonOff || imgButtonDown)
        {
            _resetPep->setImages(imgButtonOff, imgButtonDown, imgButtonOn);
            _libtuneUsermode->setImages(imgButtonOff, imgButtonDown, imgButtonOn);
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

/**
The following code allows repeat left/right on the picture control
**/
#include "input_devicecodes.h"
bool repeatKeyFunc(void *context, int cmd)
{
    MWidget *widget = (MWidget *)context;
    if (widget->hasFocus())
        if (cmd == IND_LEFT || cmd == IND_RIGHT)
            return true;
    return false;
}

void VideoInfoPage::onClick(MButton *button)
{
    bool setRepeatKey=true;
#if B_HAS_VDEC
    if (button == _combFilterEnabled) {
        ChannelManager::Settings settings;
        control()->channelManager()->getSettings(&settings);
        settings.combFilterEnabled = _combFilterEnabled->checked();
        control()->channelManager()->setSettings(&settings);

        /* We have to retune to apply the changes. */
        control()->stopLiveDecode();
        control()->setChannel(control()->channel());
        return;
    }
#endif
    if (button == _pictureControl) {
        _pictureControl->hide();

        /* get back to where we were */
        infoBar()->show();
        infoBar()->setMode(InfoBar::eFull);
        infoBar()->setCurrent(2); /* TODO: this hardcoded 2 is bogus */

        if (_pictureControl->tag() < B_TOTAL_PEP_CONTROLS) {
            _pep[_pictureControl->tag()]->focus();
        }
        else {
            switch ( _pictureControl->tag() )
            {
            case B_3DTV_ZNORM_TAG:
                _3dtvZnorm->focus();
                break;
            case MNR_TAG:
                _mnr->focus();
                break;
            case BNR_TAG:
                _bnr->focus();
                break;
            case DCR_TAG:
                _dcr->focus();
                break;
            case ANR_TAG:
                _anr->focus();
                break;
            case FLESH_TONE_TAG:
                _fleshTone->focus();
                break;
            case GREEN_STRETCH_TAG:
                _greenStretch->focus();
                break;
            case BLUE_STRETCH_TAG:
                _blueStretch->focus();
                break;
            case DYNAMIC_CONTRAST_TAG:
                _dynamicContrast->focus();
                break;
            case SPLIT_SCREEN_TAG:
                _splitScreen->focus();
                break;
            default:
                break;
            }
        }
    }
    else if (button == _resetPep) {
        bdecode_window_t window = control()->window();
        bdecode_window_settings ws;
        if (!window) { BDBG_ERR(("unable to read PEP settings")); return;}
        bdecode_window_get(window, &ws);
        ws.contrast = g_pep_settings.contrast;
        ws.saturation = g_pep_settings.saturation;
        ws.hue = g_pep_settings.hue;
        ws.brightness = g_pep_settings.brightness;
        ws.sharpness = g_pep_settings.sharpness;
        bdecode_window_set(window, &ws);

        if (control()->cfg()->getBool(Config::HDSD_SINGLE))
        {
            int window_num = control()->currentDecode() + 2; /* TODO: + 2 logic should be refactored */
            window = control()->window(window_num);
            if ( window )
            {
                    bdecode_window_get(window, &ws);
                    ws.contrast = g_pep_settings.contrast;
                    ws.saturation = g_pep_settings.saturation;
                    ws.hue = g_pep_settings.hue;
                    ws.brightness = g_pep_settings.brightness;
                    ws.sharpness = g_pep_settings.sharpness;
                    bdecode_window_set(window, &ws);
            }
        }
        updateUi();
    }
#ifdef BRUTUS_LIBTUNE_SUPPORT
    else if (button == _libtuneUsermode) {
        control()->Libtune_SetUserMode(_libtuneUsermode->current());
        control()->Libtune_TuneWindows();
    }
#endif
    else if (button == _3dtvOutputMode) {
        _3dSource->setEnabled(true);
        control()->brutus()->set3DTVAutoMode(false);
        switch (_3dtvOutputMode->current()) {
        case 0:
            control()->brutus()->set3DTVOutput(Brutus3DTVMode_eOff);
            break;
        case 1:
            control()->brutus()->set3DTVOutput(Brutus3DTVMode_eSBS);
            break;
        case 2:
            control()->brutus()->set3DTVOutput(Brutus3DTVMode_eOU);
            break;
        case 3: /* auto */
            {
            control()->brutus()->set3DTVAutoMode(true);
            control()->brutus()->set3DTVSource(control()->display()->s3dTv.message);
            control()->brutus()->set3DTVOutput(control()->display()->s3dTv.message);
            _3dSource->setChecked(control()->display()->s3dTv.message!=Brutus3DTVMode_eOff);
            _3dSource->setEnabled(false); /* gray-out the source checkbox */
            }
        }
    }
    else if (button == _3dSource) {
        Brutus3DTVMode mode = _3dSource->checked()?Brutus3DTVMode_eSBS:Brutus3DTVMode_eOff;
        control()->brutus()->set3DTVSource(mode);

        /* windows and displays need to be reconfigured */
        control()->brutus()->set3DTVOutput(control()->display()->s3dTv.output);
    }
    else {
        /* set the pictureControl using the button's tag */
        if (button->tag() < B_TOTAL_PEP_CONTROLS) {
            _pictureControl->setText(pepLabel[button->tag()]);
        }
        else {
            switch ( button->tag() )
            {
            case B_3DTV_ZNORM_TAG:
            case MNR_TAG:
            case BNR_TAG:
            case DCR_TAG:
            case ANR_TAG:
            case FLESH_TONE_TAG:
            case GREEN_STRETCH_TAG:
            case BLUE_STRETCH_TAG:
            case DYNAMIC_CONTRAST_TAG:
            case SPLIT_SCREEN_TAG:
                setRepeatKey = false;
                break;
            default:
                break;
            }
        }
        _pictureControl->setTag(button->tag());
        updateUi();

        updatePictureControl(button->tag());

        /* bring up the control and hide the info ui */
        _pictureControl->setBackgroundColor(style()->color(MStyle::BUTTON_FACE));
        _pictureControl->raise();
        _pictureControl->show();
        _pictureControl->focus();
        if( setRepeatKey )
            control()->setRepeatKeyFunc(repeatKeyFunc, _pictureControl);
        infoBar()->hide();
    }
}

void VideoInfoPage::show()
{
    /* read current state */
    ChannelManager::Settings settings;
    control()->channelManager()->getSettings(&settings);
#if B_HAS_VDEC
    _combFilterEnabled->setChecked(settings.combFilterEnabled);
#endif

    updateUi();

    InfoBarPopup::show();
}

/////////////////////////////////////

VideoInfoBarPlugin::VideoInfoBarPlugin(InfoBar *infoBar) :
    InfoBarPlugin(infoBar, "config_plugin")
{
    unsigned controlWidth = 260;
    setText("Video");

    _linkedWidget = new VideoInfoPage(infoBar, topLevelWidget(), MRect(230,25,controlWidth,300));
    _linkedWidget->hide();
    _icon = new MImage(app(), PIXMAPPATH + "backend.png");
}

VideoInfoBarPlugin::~VideoInfoBarPlugin()
{
    delete _icon;
}
