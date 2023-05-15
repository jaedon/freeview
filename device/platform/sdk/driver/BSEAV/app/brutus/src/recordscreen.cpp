/***************************************************************************
 *     Copyright (c) 2002-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: recordscreen.cpp $
 * $brcm_Revision: 52 $
 * $brcm_Date: 3/3/11 9:45a $
 *
 * Module Description: Config is pvrgui's registry
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/recordscreen.cpp $
 * 
 * 52   3/3/11 9:45a erickson
 * SWDEPRECATED-3869: rename brutus "timeshifting" to be "playback &
 * record"
 *
 * 51   6/11/10 3:18p rjain
 * SW7550-380: For 7550, dont show AES button in Brutus record
 *
 * 50   4/16/10 4:06p erickson
 * SW7405-4089: make brutus resilient to config with zero decoders
 *
 * 49   6/12/09 11:12a jtna
 * PR43001: support TS/TTS record
 *
 * 48   5/29/09 2:40p jtna
 * PR43001: move record logic to control_pvr
 *
 * 47   5/11/09 5:12p erickson
 * PR54915: remove unused and misleading code
 *
 * 46   2/23/09 11:11a erickson
 * PR52034: added comment for future work
 *
 * 45   9/15/08 12:40p tokushig
 * PR43426: merge to main
 *
 * BRUTUS_97405_97043/1   8/29/08 10:48a tokushig
 * PR43596: fix bus error on exit with blue skin.  also fixes intermittent
 * assertion in bwin (during quit)
 *
 * 44   6/27/08 3:53p mphillip
 * PR42901: Fix 3DES key length, replace DES with AES for encrypted PVR
 *
 * 43   3/19/08 9:35p erickson
 * PR40307: thumbnail update
 *
 * 42   2/6/08 3:41p jrubio
 * PR39315: cleanup required
 *
 * 41   2/4/08 11:43a jrubio
 * PR39315: removed addchannelentry and writing of info file for NetRecord
 *
 * 40   12/6/07 2:08p erickson
 * PR37038: add CONTINUOUS_RECORD_DURATION, remove unused fixedSize and
 * CIRCFILE.
 *
 * 39   12/6/07 12:50p erickson
 * PR37959: check _startNetrecord before using it. fix indenting.
 *
 * 38   11/14/07 1:29p erickson
 * PR36068: cleaned up indenting, removed redundant timeshifting check
 *
 * 37   10/3/07 3:58p jrubio
 * PR34613: ignore Disk space when doing a network record
 *
 * 36   9/20/07 5:37p jrubio
 * PR34613: fixed channelmap issue with network record
 *
 * 35   9/17/07 5:02p jrubio
 * PR34613: Do not support timeshift when netrecord is chosen
 *
 * 34   9/17/07 12:34p jrubio
 * PR34613: Added ability to record info file for netrecord
 * and ability to append CLIENT ID to network files along wth
 * adding new netrecord to channelmap screen.
 *
 * 33   8/10/07 10:24a jrubio
 * PR33830: Adding Network Record capabilities
 *
 * 32   5/24/07 4:59p mward
 * PR28583: Initialize unused controls to NULL.
 *
 * 31   4/12/07 3:53p mward
 * PR29595: Disable Timeshift if a playback is not available for it.
 *
 * 30   4/4/07 11:51a gmohile
 * PR 29188: enable PVR encryption for 7403
 *
 * 29   3/28/07 4:53p ahulse
 * PR28951: Don't start record if less than 30MB space on disk
 *
 * 28   3/14/07 10:16a erickson
 * PR26267: refactor Brutus into separate VideoList and PlaybackScreen
 *
 * 27   2/9/07 3:39p jgarrett
 * PR 27751: Disabling encryption radio buttons if not supported
 *
 * 26   2/2/07 10:06a erickson
 * PR26661: call MApplication::clearModalState after leaving modal loop
 *
 * 25   2/1/07 11:37a mphillip
 * PR20035: Don't hide the encryption selection on 7400/7401
 *
 * 24   1/29/07 11:17a ssood
 * PR24918: Channel Map Redesign to make its access Thread-safe
 *
 * 23   12/20/06 3:37p erickson
 * PR25109: add 7403 support. as part of this, I removed 7401 A0 support
 * instead of fixing the #if
 *
 * 22   9/26/06 11:29a erickson
 * PR24374: convert to DUAL_CMP_SUPPORT
 *
 * 21   8/14/06 2:23p arbisman
 * PR23415: PVR timeshift is not available in 97401 B0 IPSTB release
 *
 * 20   7/28/06 3:20p arbisman
 * PR21282: Disable PVR timeshifting for 97401 A0 IPSTB - Use BCHP_VER
 *
 * 19   7/11/06 5:47p mphillip
 * PR19544: Initial commit of bcrypt_ stubs
 *
 * 18   5/31/06 4:18p tokushig
 * PR21891: Merging updated picture viewer into mainline.
 *
 * SanDiego_Brutus_Skin/4   5/30/06 4:39p tokushig
 * merge from main
 *
 * 17   5/25/06 11:04a erickson
 * PR20035: disable UI until feature is ready for 74xx
 *
 * SanDiego_Brutus_Skin/3   5/16/06 11:33a tokushig
 * merge from main
 *
 * 16   5/4/06 3:33p erickson
 * PR21047: RecordScreen::canRecord depends on digital or encoder
 *
 * 15   4/28/06 1:00p arbisman
 * PR21282: Disable PVR timeshifting for 97401 A0 IPSTB
 *
 * 13   4/17/06 5:06p jgarrett
 * PR 20951: Merging skinned UI into mainline
 *
 * SanDiego_Brutus_Skin/2   4/13/06 3:16p tokushig
 * merge from main to SanDiego_Brutus_Skin branch
 *
 * SanDiego_Brutus_Skin/1   2/13/06 4:36p tokushig
 * add partial skin ability to record screen.  note that this is primarily
 * for cable show demo purposes and will be expanded to be fully
 * skinnable in the future.
 *
 * 12   4/7/06 1:27p ahulse
 * PR20504: Be more explicit in assigning encodeQuality value according to
 * whether or not VPP is enabled
 *
 * 11   4/7/06 10:52a ahulse
 * PR20504: Merge Vpp Modifications into main branch
 *
 * Vpp_7042_Demo/4   4/5/06 6:12p ahulse
 * PR20504: Remove debug warnings / unused code
 *
 * Vpp_7042_Demo/3   4/5/06 9:37a ahulse
 * PR20504: When in vpp mode adjust quality setting to correct one
 *
 * Vpp_7042_Demo/2   4/4/06 1:18p ahulse
 * PR20504: Use a modal box to set vpp settings. Modal box is only active
 * if VPP_ENABLED is set
 *
 * Vpp_7042_Demo/1   3/29/06 5:51p ahulse
 * PR20504: Enable support for changeing 7042's vpp settings when encoding
 * analog
 *
 * 10   3/21/06 4:49p mphillip
 * PR19786: Don't access an uninitialized pointer in
 * CONTINUOUS_RECORD_ENABLED mode
 *
 * 9   3/15/06 4:04p mphillip
 * PR19786: Initial implementation.  Add CONTINUOUS_RECORD_ENABLED=1 to
 * brutus.cfg to enable
 *
 * 8   10/20/05 9:55a jrubio
 * PR17632: Workaround to record-timeshift problem.. Wait two seconds
 *
 * 7   8/24/05 6:15p arbisman
 * PR16678: Add support for WinCE
 *
 * 6   8/23/05 12:36p erickson
 * PR16660: ProgramGuide now supports subch
 *
 * 5   8/17/05 5:25p erickson
 * PR16719: rework Video and Media to never return MString as return value
 *
 * 4   6/29/05 4:21p erickson
 * PR15966: use MAINCH for program guide. this means subchannels will all
 * have the same name, which isn't great, but at least they get a name.
 *
 * 3   6/9/05 10:20a erickson
 * PR13199: if record name is blank, add default name
 *
 * 2   4/9/05 12:17a erickson
 * PR14701: added b_print_chnum
 *
 * 1   2/7/05 8:16p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/35   1/12/05 11:49a erickson
 * PR13097: if record succeeds but play fails (maybe no data), allow the
 * Video entry to still be created. keeps the gui consistent.
 *
 * Irvine_BSEAVSW_Devel/34   11/19/04 3:26p erickson
 * PR13199: if program guide is empty, use the channel name
 *
 * Irvine_BSEAVSW_Devel/33   9/21/04 2:22p erickson
 * PR12706: allowed record source label to wrap and removed worthless
 * display label
 *
 * Irvine_BSEAVSW_Devel/32   8/17/04 1:14p erickson
 * PR12257: changed message when starting timeshifting
 *
 * Irvine_BSEAVSW_Devel/31   7/23/04 3:07p erickson
 * PR11819: Changed labels for clarity
 *
 * Irvine_BSEAVSW_Devel/30   7/14/04 1:36a erickson
 * PR11287: refactor linux-specific code into brutus_linux.cpp for easier
 * porting
 *
 * Irvine_BSEAVSW_Devel/29   4/30/04 12:06p erickson
 * PR10832: rework of brutus to support single AVManager and
 * ChannelManager for system
 *
 * Irvine_BSEAVSW_Devel/28   4/21/04 10:12a erickson
 * PR10711: moved lock file into default video dir to allow for readonly
 * filesystem
 *
 * Irvine_BSEAVSW_Devel/27   4/16/04 5:27p erickson
 * PR10623: set keysize in brutus
 *
 * Irvine_BSEAVSW_Devel/26   3/19/04 12:04p erickson
 * PR10159: added PROMPT_ENABLED and used it in a few key places related
 * to record and playback
 *
 * Irvine_BSEAVSW_Devel/25   3/19/04 11:49a erickson
 * PR10149: if there's no index, fail the playback (with a UI message),
 * but keep record going
 *
 * Irvine_BSEAVSW_Devel/24   3/4/04 1:39p erickson
 * PR9465: completed refactoring of channelmgr by moving channel string
 * generation into Brutus
 *
 * Irvine_BSEAVSW_Devel/23   2/5/04 12:15p bandrews
 * PR9497: Added include "bkni.h"
 *
 * Irvine_BSEAVSW_Devel/22   2/5/04 11:49a erickson
 * PR9497: vxworks doesn't support usleep
 *
 ****************************************************************************/
#include "recordscreen.h"
#include "cfgsettings.h"
#include "channelmgr.h"
#include "control.h"
#include "programguide.h"
#include "mmessagebox.h"
#include "bstd.h"
#include "bkni.h"
#include "ip.h"
#include <unistd.h>

#ifdef _WIN32_WCE
#include <windows.h>
#include <Mmsystem.h>
#endif

#if BCHP_CHIP!=7550
#define AES_SUPPORTED 1
#endif

BDBG_MODULE(brutus_recordscreen);

class EditVpp : public MLabel, public MButtonEventHandler,
    public MRadioButtonGroupEventHandler,
    public MListBoxEventHandler {
public:
    EditVpp(RecordScreen *scr);
    void edit( );
    int getVppSettings( );
    bool focus() { return _vppSettings->focus(); }
    // events
    void onClick(MButton *sender);
protected:
    bool _done, _addmode;
    RecordScreen *_scr;
    int _index;

    MRadioButtonGroup *_vppSettings;

    MPushButton _ok;
};



#define PANELSIZE MRect(200,45,205,155)

EditVpp::EditVpp(RecordScreen *scr) :
        MLabel(scr->fb(), MRect(200,60,250,250), NULL, "editvpp"),
        _ok(this, MRect(60,200,120,40), "OK", "okbtn")
{
    setBackgroundColor(scr->cfg()->getInt(Config::BACKGROUND_COLOR));
    _ok.addHandler(this);
    _scr = scr;
    setBevel(5);
    setFocusable(true);

    _vppSettings = new MRadioButtonGroup(this, MRect(40,20,200,180), "Vpp"),
    _vppSettings->add("No VPP");
    _vppSettings->add("Temporal Only");
    _vppSettings->add("Spatial  Only");
    _vppSettings->add("Impulse  Only");
    _vppSettings->add("Medium");
    _vppSettings->add("Default");
    _vppSettings->click(5);
}

void EditVpp::onClick(MButton *sender)
{
    if (sender == &_ok) {
        _done = true;
    }
}

void EditVpp::edit( void )
{
    raise();
    show();
    focus();

    _done = false;
    // process all messages while we wait for the result
    while (!_done && app()->processEvent(true));
    app()->clearModalState();

    hide();
    _scr->focus();
}

int EditVpp::getVppSettings( void ) {
    return _vppSettings->checked();
}

RecordScreen::RecordScreen(Control *acontrol) :
    GenericScreen(acontrol, SMALLTV) //, SMALLPIP)
{
    _prevCh = -1;
    _canRecord = false;
    int x = 60-38;
    int y = 25;

    _eVpp = new EditVpp(this);


    Skin * pSkin = control()->getSkin();
    if (pSkin->isValid(XML_SCREEN_RECORD))
    {

        pSkin->createWidgets(XML_SCREEN_RECORD, this);
        //return; //do not return here...we must continue creating required widgets
        y += 30;
    }

    bool bSkinValid = (pSkin->isValid(XML_SCREEN_RECORD));

    if (!bSkinValid)
    {
        l1 = new MLabel(this, MRect(x,y,300,40), "Record");
        l1->setFont(MFont(app(), control()->cfg()->get(Config::SCREEN_TITLE_FONT)));
    }
    else
        l1 = NULL;

    x += 20;
    y += 60;
    l2 = new MLabel(this, MRect(x,y,300,20), "Name");
    if (bSkinValid)
    {
        l2->setTextColor(0xffcccccc);
    }
    y += 20;
    _le = new MLineEdit(this, "record_input");
    _le->setGeometry(MRect(x, y, 250, 40));
    x += 260;
    if (!bSkinValid)
    {
        _setGuide = new MPushButton(this, MRect(x,y+5,50,30), "Save");
        _setGuide->addHandler(this);
        _setGuide->setFont(MFont(app(), control()->cfg()->get(Config::TINY_FONT)));
    }
    else
        _setGuide = NULL;

    x -= 260;

    y += 70;

    l3 = new MLabel(this, MRect(x,y,300,20), "Encode Quality");
    if (bSkinValid)
    {
        l3->setTextColor(0xffcccccc);
    }
    y += 20;
    _quality = new MRadioButtonGroup(this, MRect(x,y,110,120));
    if (bSkinValid)
    {
        _quality->setTextColor(0xffcccccc);
    }
    _quality->add("Best");
    if( !cfg()->getBool(Config::VPP_ENABLED) ) {
        /* No High setting if VPP ENABLED */
        _quality->add("High");
    }
    _quality->add("Medium");
    _quality->add("Basic");
    _quality->click(0);
    x += 160;
    y -= 20;

    int savey = y;
    if (!(control()->cfg()->getBool(Config::CONTINUOUS_RECORD_ENABLED))) {
        _startPlayback = new MCheckBox(this, MRect(x,y,200,30), "Start playback");
        _startPlayback->click();
        if (bSkinValid)
        {
            _startPlayback->setTextColor(0xffcccccc);
        }

        /* enable box for IP Record */
        y+=35;
        _startNetrecord = new MCheckBox(this, MRect(x,y,200,30), "Network Record");
        if (bSkinValid)
        {
            _startNetrecord->setTextColor(0xffcccccc);
        }

    }
    else {
        /* These UI elements don't exist for continuous record mode. Be careful to check before dereferencing them later. */
        _startNetrecord = NULL;
        _startPlayback = NULL;
    }

    y += 46;
    l4 = new MLabel(this, MRect(x,y,300,20), "PVR Encryption");
    if (bSkinValid)
    {
        l4->setTextColor(0xffcccccc);
    }
    y += 20;
    _encrypt = new MRadioButtonGroup(this, MRect(x,y,200,80), "Encryption");
    if (bSkinValid)
    {
        _encrypt->setTextColor(0xffcccccc);
    }
    _encrypt->add("Clear");
    _encrypt->add("3DES");
#if AES_SUPPORTED==1
    _encrypt->add("AES");
#endif
    /*_encrypt->add("DES");*/
    _encrypt->click(0);
    _encrypt->setEnabled(bconfig->features.pvr_encryption);

    y = savey;
    x += 210;
    x = 60-38;
    if (!bSkinValid)
    {
        _back = new MPushButton(this, MRect(x,375,120,40), "Back");
        _back->addHandler(this);
        _record = new MPushButton(this, MRect(510-38,375,120,40), "Record");
        _record->addHandler(this);
    }
    else
        _back = _record = NULL;

    if (bSkinValid)
    {
        //make sure all required components are on top
        _encrypt->raise();

        if (_startPlayback) _startPlayback->raise();
        _quality->raise();
        l2->raise();
        l3->raise();
        l4->raise();
    }

    _currentVideo = NULL;
}

RecordScreen::~RecordScreen()
{
    if (_eVpp)
    {
        delete _eVpp;
    }
}

void RecordScreen::setSource() {
    bool encoder_enabled = control()->cfg()->getBool(Config::ENCODER_ENABLED);
    // must get current channel from Control because of deferral
    int ch = control()->channel();
    bool isDigital = control()->channelMap()->isDigital(ch);

    _canRecord = isDigital || encoder_enabled;
    Skin * pSkin = control()->getSkin();
    if (!pSkin->isValid(XML_SCREEN_RECORD))
    {
        _record->setEnabled(_canRecord);
    }
    _quality->setEnabled(!isDigital && encoder_enabled);

    // only change the recording name if the channel changes
    if (_currentVideo && ch != _prevCh) {
        char buf[256];
        const char *title;

        _prevCh = ch;
        title = control()->guide()->programName(ch);
        if (!title) {
            ChannelMap::Channel channel;
            if (!control()->channelMap()->getChannel(ch, &channel)) {
                title = buf;
                char *ptr = buf;
                ptr += b_print_chnum(ptr, 256, &channel);
                ptr += sprintf(ptr, ", ");
                b_print_channel_description(ptr, 200, &channel.entry);
            }
            else {
                /* We're completely in the dark, so just pick a name.
                We should be dead, actually. */
                title = "Recording";
            }
        }
         _currentVideo->title = title;
        _le->setText(_currentVideo->title);
    }
}

void RecordScreen::newVideo() {
    if (_startPlayback) {
        DecodeEngine::Settings decodeSettings;
        DecodeEngine *decode = control()->decode();
        PlayEngine *playEngine = NULL;

        if (decode) {
            decode->getSettings(&decodeSettings);
            if (decodeSettings.playManager) {
                playEngine = decodeSettings.playManager->checkOut();
            }
        }
        if (playEngine && decodeSettings.playManager) {
            decodeSettings.playManager->checkIn(playEngine);
            _startPlayback->setEnabled(true);
            _startPlayback->setChecked(true);
        }
        else {
            _startPlayback->setChecked(false);
            _startPlayback->setEnabled(false);
        }
    }

    if (_currentVideo)
        return;
    _currentVideo = new Video();

    // this sets input name
    _prevCh = -1;

    setSource();
}

void RecordScreen::onClick(MButton *self) {
    int sd;

    if ((self == _back) || (self->action() == eActionShowMenuWin)) {
        control()->showScreen(Control::eHome);
    }
    else if ((self == _record) || (self->action() == eActionRecord)) {
        /* check for vpp settings */
        bool encoder_enabled = control()->cfg()->getBool(Config::ENCODER_ENABLED);
        bool isDigital = control()->channelMap()->isDigital( control()->channel() );
        bool vpp_enabled = control()->cfg()->getBool(Config::VPP_ENABLED);

        if( !isDigital && encoder_enabled && vpp_enabled ) {
            _eVpp->edit();
            switch ( _eVpp->getVppSettings() ) {
            case 0:_currentVideo->encodingVpp = bencode_vpp_off; break;    /* VPP is off */
            case 1:_currentVideo->encodingVpp = bencode_vpp_temporal_only; break;   /* Temporal Only */
            case 2:_currentVideo->encodingVpp = bencode_vpp_spatial_only; break;    /* Spatial Only */
            case 3:_currentVideo->encodingVpp = bencode_vpp_impulse_only; break;    /* Impulse Only */
            case 4:_currentVideo->encodingVpp = bencode_vpp_medium; break;  /* Medium */
            case 5:_currentVideo->encodingVpp = bencode_vpp_default; break; /* Default */
            default: _currentVideo->encodingVpp = bencode_vpp_none; break;
            }
        }

        startRecord();
    }
    else if ((self == _setGuide) || (self->action() == eActionSave)) {
        if (control()->guide()->setProgramName(control()->channel(), _le->text()))
            MMessageBox::info(fb(), "Program guide saved");
        else
            MMessageBox::info(fb(), "Error saving guide");
    }
    else if (control()->getSkin()->isValid(XML_SCREEN_RECORD))
    {
        control()->getSkin()->processAction(self);
    }
    else {
        GenericScreen::onClick(self);
    }
}

bool RecordScreen::startRecord() {
    bool rc = true;
    int sd;
    int nextid, sortIndex;
    unsigned long total, avail;
    char * f = NULL;
    char post[64];

    BDBG_ASSERT(_currentVideo);

    if (_le->text()[0]) {
        _currentVideo->title = _le->text();
    }
    else {
        _currentVideo->title = "Unknown";
    }

    if (_quality->enabled()) {
        _currentVideo->isEncoded = true;
        if ( !control()->cfg()->getBool(Config::VPP_ENABLED) && _currentVideo->encodingQuality ) {
            switch (_quality->checked()) {
            case 0:_currentVideo->encodingQuality = bencode_quality_best; break;
            case 1:_currentVideo->encodingQuality = bencode_quality_high; break;
            case 2:_currentVideo->encodingQuality = bencode_quality_medium; break;
            case 3:_currentVideo->encodingQuality = bencode_quality_basic; break;
            }
        }
        else {
            /* Reduced video bitrate options if VPP Enabled */
            switch (_quality->checked()) {
            case 0:_currentVideo->encodingQuality = bencode_quality_best; break;
            case 1:_currentVideo->encodingQuality = bencode_quality_medium; break;
            case 2:_currentVideo->encodingQuality = bencode_quality_basic; break;
            }
        }
    }
    else {
        _currentVideo->isEncoded = false;
    }

    /* the keys will be filled in by RecordEngine */
    switch (_encrypt->checked()) {
    case 0: _currentVideo->mpeg.encryption.type = bencryption_type_none; _currentVideo->mpeg.encryption.key_length = 0; break;
    case 1: _currentVideo->mpeg.encryption.type = bencryption_type_3des; _currentVideo->mpeg.encryption.key_length = 128; break;
#if AES_SUPPORTED==1
    case 2: _currentVideo->mpeg.encryption.type = bencryption_type_aes; _currentVideo->mpeg.encryption.key_length = 128; break;
#endif
    /*case 3: _currentVideo->mpeg.encryption.type = bencryption_type_des; _currentVideo->mpeg.encryption.key_length = 64; break;*/
    }

    if (_startNetrecord && _startNetrecord->checked() &&
        (control()->cfg()->get(Config::IPSERVER_IPV4)) &&
        (control()->cfg()->getInt(Config::IPSERVER_REC_PORT)) &&
        (control()->cfg()->get(Config::IPSERVER_CLIENT_ID))) {
        _currentVideo->isNetRecord = true;
    }

    if (_startPlayback && _startPlayback->checked()) {
        _currentVideo->startPlaybackOfRecord = true;
    }


    /* We've populated Video with all the information from the recordscreen.
    It will now be passed to Control which will complete the process
    and actually start recording. */

    MMessageBox *msg = NULL;
    /* if _currentVideo->startPlaybackOfRecord is true, control_pvr.cpp will put up a "Starting Record & Playback msgbox */
    if (!_currentVideo->startPlaybackOfRecord && !_currentVideo->isNetRecord) {
        msg = new MMessageBox(fb(), 350, 100);
         /* check first to see if Network Record is checked */
        if(_currentVideo->isNetRecord) {
            msg->set("Starting Network Record...", 0);
        }
        else {
            msg->set("Starting Record...", 0);
        }
        msg->show();
        msg->drawNow();
    }

    if (control()->recordVideoPrepare(_currentVideo)==true) {
        control()->recordVideo(_currentVideo, control()->channel());
    }

    // clearing _currentVideo means the memory will be handled outside of
    // RecordScreen and RecordScreen will allocate a new Video when it is
    // displayed again
    // This presents a problem with NetRecord when trying to add the channel
    //  in the channel Map entry. So we removed that!
    _currentVideo = NULL;
    control()->showScreen(Control::eTV);

    if (msg) delete msg;
    return rc;
}

bool RecordScreen::keyDown(int ascii, int modifiers, int scancode) {
    switch (ascii) {
    case MWKEY_ENTER:
        startRecord();
        return true;
    }
    return GenericScreen::keyDown(ascii, modifiers, scancode);
}

void RecordScreen::show() {
    Skin * pSkin = control()->getSkin();
    if (!pSkin->isValid(XML_SCREEN_RECORD))
    {
        control()->isAdmin()?_setGuide->show():_setGuide->hide();
    }
    newVideo();
    GenericScreen::show();
}
