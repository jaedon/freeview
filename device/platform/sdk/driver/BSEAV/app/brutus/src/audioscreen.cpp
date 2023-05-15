/***************************************************************************
 *     Copyright (c) 2002-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: audioscreen.cpp $
 * $brcm_Revision: 11 $
 * $brcm_Date: 9/15/08 12:40p $
 *
 * Module Description: Config is pvrgui's registry
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/audioscreen.cpp $
 * 
 * 11   9/15/08 12:40p tokushig
 * PR43426: merge to main
 * 
 * BRUTUS_97405_97043/2   9/12/08 5:37p tokushig
 * PR43426: partial merge to branch from main
 * 
 * BRUTUS_97405_97043/1   8/29/08 10:48a tokushig
 * PR43596: fix bus error on exit with blue skin.  also fixes intermittent
 * assertion in bwin (during quit)
 * 
 * 10   8/18/08 12:33p erickson
 * PR42789: update
 *
 * 9   8/15/08 10:08a erickson
 * PR42789: bluetooth update
 *
 * 8   5/7/08 11:51a erickson
 * PR42329: fix minor BKNI memory leaks
 *
 * 7   2/21/08 2:09p ahulse
 * PR25541: Add ability to play indexed audio file, plus list out audio
 * files
 *
 * 6   2/28/07 7:12p gmohile
 * PR 27930: if no file is selected then play first file in list
 *
 * 5   2/27/07 4:12p erickson
 * PR24971: remove vertical scrollbar buttons which weren't connected and
 * were hard to navigate to anyway
 *
 * 4   5/31/06 4:21p tokushig
 * PR21891: Merging updated picture viewer into mainline.
 *
 * SanDiego_Brutus_Skin/3   5/16/06 11:33a tokushig
 * merge from main
 *
 * 3   4/17/06 5:06p jgarrett
 * PR 20951: Merging skinned UI into mainline
 *
 ****************************************************************************/
#include "audioscreen.h"
#include "cfgsettings.h"
#include "audiocontrol.h"
#include "control.h"
#include "mfileview.h"
#include <unistd.h> // file io
#include <fcntl.h>

#include "bstd.h"
BDBG_MODULE(brutus_audioscreen);

#define PIXMAPPATH (MString(_cfg->get(Config::IMAGES_PATH)) + "/audio/")

#define GREEN 0xFF117711

class AudioBufferBar : public MLabel {
public:
    AudioBufferBar(MWidget *parent, const MRect &rect) :
        MLabel(parent, rect)
    {
        _barColor = 0xFF117711;
        _bkColor = 0xFF888888;
        _amp = 0;
        reset();
        setTransparent(false); /* we're going to repaint the entire region */
    }
    /* this must be called when first showing the widget */
    void reset() {
        _lastpos = -1;
    }
    void setValue(unsigned long amp) {
        _amp = amp;
        repaint();
    }
protected:
    int _lastpos;
    unsigned long _barColor, _bkColor, _amp;
    void draw(const MRect &cliprect);
};

void AudioBufferBar::draw(const MRect &cliprect) {
    MPainter ptr(this);
#if 1
    unsigned long f = _amp;
    f *= width();
    f /= 32768;
#endif
#if 0
#define BASE_LEVEL 3000
    static float base = log10(BASE_LEVEL);
    static float max = log10(32768.0);
    float f = _amp > BASE_LEVEL ? log10(_amp) - base : 0;
    f /= max;
    f *= width();
#endif
#if 0
    static float max = log(32768.0);
    float f = log(_amp);
    f /= max;
#endif
#if 0
#define BASE_LEVEL 3000
    static float max = log(32768.0);
    float f = log(_amp);
    f /= max;
    f *= width();
    f *= width();
#endif
    int pos = (int)f;

    /* Draw only the pixels that have changed */
    if (_lastpos == -1) {
        /* draw everything because we've reset */
        ptr.setBrush(_barColor);
        ptr.fillRect(MRect(0,0,pos,height()));
        ptr.setBrush(_bkColor);
        ptr.fillRect(MRect(pos,0,width()-pos,height()));
    }
    else if (pos > _lastpos) {
        ptr.setBrush(_barColor);
        ptr.fillRect(MRect(_lastpos,0,pos-_lastpos, height()));
    }
    else if (pos < _lastpos) {
        ptr.setBrush(_bkColor);
        ptr.fillRect(MRect(pos,0,_lastpos-pos,height()));
    }
    _lastpos = pos;
}

#if BLUETOOTH_SUPPORT
#include "easy_bt.h"

/* 5 seconds after the last PCM sample from a bluetooth device, we will re-enable audio decode */
#define BLUETOOTH_AUDIO_MUTE_TIME 5000
static unsigned g_lastBluetoothTime;

/* this is a non-blocking call. this must be non-blocking, otherwise the BT stack will get messed up.
also, there's no harm in making this non-blocking because the incoming PCM should be rate-controlled
from the BT device. if we overflow, it's best to throw away the data and keep on going. */
void bt_pcm_playback_write(void *data, unsigned target_size, void *context)
{
    unsigned total = 0;
    AudioControl *audioControl = (AudioControl *)context;

    g_lastBluetoothTime = MApplication::tickCount();

    /* even though we are non-blocking, we still need to loop in order to handle wrap around */
    while (total < target_size) {
        void *buffer;
        unsigned size;
        bresult rc;

        rc = bpcm_play_get_buffer(audioControl->pcm_play(), &buffer, &size);
        if (rc) return;

        /* here's the part that makes it non-blocking */
        if (size == 0) break;

        if (size > target_size - total) {
            size = target_size - total;
        }

        BKNI_Memcpy(buffer, data, size);
        rc = bpcm_play_write_complete(audioControl->pcm_play(), size);
        if (rc) return;

        total += size;
        data = (uint8_t*)data + size;
    }

    return;
}

#if BT_HAS_AUDIO_SOURCE
#if 0
#include "math.h"
unsigned bt_pcm_capture_read(void *data, unsigned size, void *context)
{
    static int cnt = 0;
    unsigned i;

    size -= size%4;

    for (i=0;i<size/4;i++) {
        uint16_t val = (sin(cnt++/100.0) * 16000);
        val -= 8000;
        ((uint32_t*)data)[i] = val<<16 | val;
        if (cnt == 100) cnt = 0;
    }
    printf("capture %d\n", size);
    return size;
}
#else
extern "C" {
int b_punch_ac_get_buffer(void **buffer, unsigned *bufferSize);
int b_punch_ac_write_complete(unsigned bufferSize);
}
unsigned bt_pcm_capture_read(void *data, unsigned size, void *context)
{
    void *buffer;
    unsigned bufferSize;
    if (b_punch_ac_get_buffer(&buffer, &bufferSize)) {
        printf("capture failed\n");
        return 0;
    }
    if (bufferSize > 30000) {
        /* can't overflow raptor, so just eat it all */
        b_punch_ac_write_complete(bufferSize);
        return 0;
    }
    else if (bufferSize >= size) {
        printf("send %d of possible %d\n", size, bufferSize);
        bufferSize = size;
        memcpy(data, buffer, bufferSize);
        b_punch_ac_write_complete(bufferSize);
        return bufferSize;
    }
    else {
        return 0;
    }
}
#endif
#endif
#endif


AudioScreen::AudioScreen(Control *acontrol) :
    GenericScreen(acontrol, NOTV), _timer(acontrol->fb()->app())
{
    uint32_t             nBevelListBox        = 2;
    MPainter::BevelStyle styleBevel           = MPainter::bsRaised;
    int32_t              nListTextColor       = -1;
    int32_t              nListTextColorFocus  = -1;
    MString              listScrollbarImage;
    MString              listScrollIndImage;

    _cfg = acontrol->cfg();

    _audioControl = acontrol->audioControl();
    _timer.addHandler(this);

#if BLUETOOTH_SUPPORT
    /* TODO: disable other MP3 playback because of this */
    if (cfg()->getBool(Config::BT_AUDIO_SINK_ENABLED)) {
        bpcm_play_settings settings;
        bresult rc;

        bpcm_play_settings_init(&settings, _audioControl->pcm_play());

        /* PR 45599 - PCM playback only works for 48KHz, no audio for 32KHz or 44.1KHz */
        settings.pcm.sample_rate = 48000;

        /* start playback and keep it going. we will poll. no callback needed. */
        rc = bpcm_play_start(_audioControl->pcm_play(), control()->display()->display, &settings);
        if (!rc) {
            EasyBtSettings btSettings;
            EasyBt_GetSettings(&btSettings);
            btSettings.audioSink.callback = bt_pcm_playback_write;
            btSettings.audioSink.callbackContext = _audioControl;
            EasyBt_SetSettings(&btSettings);
        }
        else {
            BDBG_ERR(("Unable to start Bluetooth PCM playback"));
        }
    }
/* BT_HAS_AUDIO_SOURCE is temp */
#if BT_HAS_AUDIO_SOURCE
    if (cfg()->get(Config::BT_AUDIO_SOURCE_ADDRESS)) {
        EasyBtSettings btSettings;
        EasyBt_GetSettings(&btSettings);
        btSettings.audioSource.callback = bt_pcm_capture_read;
        btSettings.audioSource.callbackContext = acontrol;
        EasyBt_SetSettings(&btSettings);
    }
#endif
#endif

    Skin * pSkin = control()->getSkin();
    if (pSkin->isValid(XML_SCREEN_AUDIO))
    {
        pSkin->createWidgets(XML_SCREEN_AUDIO, this);

        //get list box attributes from skin
        MString         listBevelStyle;
        MString         listTextColor;
        MString         listTextColorFocus;
        uint32_t        listX               = 0;
        uint32_t        listY               = 0;
        uint32_t        listWidth           = 0;
        uint32_t        listHeight          = 0;
        uint32_t        listBevelWidth      = 0;
        uint32_t        listDropShadow      = 0;

        pSkin->getAttrValue(XML_SCREEN_AUDIO,
                            XML_ATT_GEOM_LIST_X,            listX);
        pSkin->getAttrValue(XML_SCREEN_AUDIO,
                            XML_ATT_GEOM_LIST_Y,            listY);
        pSkin->getAttrValue(XML_SCREEN_AUDIO,
                            XML_ATT_GEOM_LIST_H,            listHeight);
        pSkin->getAttrValue(XML_SCREEN_AUDIO,
                            XML_ATT_GEOM_LIST_W,            listWidth);
        pSkin->getAttrValue(XML_SCREEN_AUDIO,
                            XML_ATT_BEVEL_LIST_WIDTH,       listBevelWidth);
        pSkin->getAttrValue(XML_SCREEN_AUDIO,
                            XML_ATT_BEVEL_LIST_STYLE,       listBevelStyle);
        pSkin->getAttrValue(XML_SCREEN_AUDIO,
                            XML_ATT_LIST_TEXT_COLOR,        listTextColor);
        pSkin->getAttrValue(XML_SCREEN_AUDIO,
                            XML_ATT_LIST_TEXT_COLOR_FOCUS,  listTextColorFocus);
        pSkin->getAttrValue(XML_SCREEN_AUDIO,
                            XML_ATT_LIST_SCROLLBAR_IMAGE,   listScrollbarImage);
        pSkin->getAttrValue(XML_SCREEN_AUDIO,
                            XML_ATT_LIST_SCROLLIND_IMAGE,   listScrollIndImage);
        pSkin->getAttrValue(XML_SCREEN_AUDIO,
                            XML_ATT_LIST_DROP_SHADOW,       listDropShadow);

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

        bevel = new MLabel(this, MRect(listX, listY, listWidth, listHeight),
                           (const char *)NULL, "bevel");
        bevel->setBevel(nBevelListBox, styleBevel);
        bevel->setTransparent(true);

        fileview = new MFileView(bevel, MRect(2, 2, bevel->width()-4,bevel->height()-4),"fileview");
        fileview->addHandler(this);
        fileview->vScrollbar()->setButtons(false); // hide the buttons

        fileview->setTextColor(nListTextColor);
        fileview->setTextColorFocus(nListTextColorFocus);
        fileview->setDropShadow(listDropShadow);
        fileview->viewport()->setTextColor(nListTextColor);
        fileview->viewport()->setTextColorFocus(nListTextColorFocus);
        fileview->viewport()->setDropShadow(listDropShadow);

        fileview->setHomeDir(_cfg->get(Config::AUDIO_PATH));

        bevel->raise();
        _leftbtn = NULL;
        _middlebtn = NULL;
        _rightbtn = NULL;
        _stopbtn = NULL;
        _playbtn = NULL;
        _playallbtn = NULL;
    }
    else
    {
        int x = 4;
        int y = 5;

        _leftbtn = new MImage(app(), PIXMAPPATH + "leftbtn.png");
        _middlebtn = new MImage(app(), PIXMAPPATH + "middlebtn.png");
        _rightbtn = new MImage(app(), PIXMAPPATH + "rightbtn.png");
        _stopbtn = new MImage(app(), PIXMAPPATH + "stop.png");
        _playbtn = new MImage(app(), PIXMAPPATH + "play.png");
        _playallbtn = new MImage(app(), PIXMAPPATH + "playall.png");

        _title = new MLabel(this, MRect(x,y,120,50), _leftbtn, _middlebtn, _rightbtn);
        _title->setFont(MFont(app(), _cfg->get(Config::SCREEN_TITLE_FONT)));
        x += _title->width() + 5;
        _stop = new MPushButton(this, x, y, _stopbtn);
        _stop->addHandler(this);
        x += 55;
        _play = new MPushButton(this, x, y, _playbtn);
        _play->addHandler(this);
        x += 55;
        _playall = new MPushButton(this, x, y, _playallbtn);
        _playall->addHandler(this);
        x += 55;
        MLabel *l = new MLabel(this, MRect(x,y,335,50), _leftbtn, _middlebtn, _rightbtn);
        _lbar = new AudioBufferBar(l, MRect(25,8,285,13));
        _rbar = new AudioBufferBar(l, MRect(25,25,285,13));

        x = 4;
        y = 55;
        new MLabel(this, MRect(x,y,80,20), "Location:");
        _currentDir = new MLabel(this, MRect(x+80,y,630-x,20), "/", "curdir");
        y += 25;

        bevel = new MLabel(this, MRect(x,y,630-x,340), (const char *)NULL, "bevel");
        bevel->setBevel(2, MPainter::bsRaised);

        fileview = new MFileView(bevel,MRect(2,2,bevel->width()-4,bevel->height()-4),"fileview");
        fileview->addHandler(this);
        fileview->setHomeDir(_cfg->get(Config::AUDIO_PATH));
        fileview->vScrollbar()->setButtons(false); // hide the buttons
    }
}

AudioScreen::~AudioScreen()
{
    if (_leftbtn)
    {
    delete _leftbtn;
    }
    if (_rightbtn)
    {
    delete _rightbtn;
    }
    if (_middlebtn)
    {
    delete _middlebtn;
    }
    if (_stopbtn)
    {
    delete _stopbtn;
    }
    if (_playbtn)
    {
    delete _playbtn;
    }
    if (_playallbtn)
    {
    delete _playallbtn;
}
}


void AudioScreen::onTimeout(MTimer *sender)
{
#if 0 && BLUETOOTH_SUPPORT
    if (MApplication::tickCount() < g_lastBluetoothTime + BLUETOOTH_AUDIO_MUTE_TIME)
        control()->disableAudio();
    else
        control()->enableAudio();
#endif
}

int AudioScreen::readDisk() {
    // go to home dir and refresh.
    //TODO: it would be nice to change directory ONLY if the directory
    // no longer exists.
    fileview->gotoDir(NULL);
    return 0;
}

void AudioScreen::show() {
    if (_cfg->getBool(Config::ALWAYS_REFRESH_MP3S))
        readDisk();
    GenericScreen::show();

    Skin * pSkin = control()->getSkin();
    if (!(pSkin->isValid(XML_SCREEN_AUDIO)))
    {
        _lbar->reset();
        _rbar->reset();
    }
}

void AudioScreen::onLayout(MListView *sender)
{
    MListViewItem *item = NULL;
        item = fileview->findItem(_selected);
    if (item)
        sender->setSelected(item);
}

void AudioScreen::onClick(MListView *sender) {
    if (!fileview->currentFileIsDir())
    {
        sender->setSelected(fileview->current());
        _selected = fileview->currentFile();
        playAudio();
    }
    else
        setCurrentDir();
}

void AudioScreen::onClick(MButton *self) {
    if (self == _stop || (self->action() == eActionAudioStop))
        stopAudio();
    else if (self == _play || (self->action() == eActionAudioPlay)) {
        playAudio();
    }
    else if (self == _playall || (self->action() == eActionAudioPlayAll)) {
        playAll();
    }
    else
        GenericScreen::onClick(self);
}

void AudioScreen::listAll() {
    bool rc = false;
    MStringList list;
    int count = 0;
        MString dir = fileview->currentDir();
        dir += "/";
		BKNI_Printf("\n");
        for (MListViewItem *item = fileview->first(); item; item = fileview->next()) {
            if (item->label()[0] == '[')
                continue;
            list.add(dir + item->description());
            count++;
			BKNI_Printf("%3d %s\n", count, item->description().s() );
        }
        }

void AudioScreen::playAll() {
    bool rc = false;
    MStringList list;
    int count = 0;
        MString dir = fileview->currentDir();
        dir += "/";
        for (MListViewItem *item = fileview->first(); item; item = fileview->next()) {
            if (item->label()[0] == '[')
                continue;
            list.add(dir + item->description());
            count++;
    }

    if (count) {
        rc = _audioControl->playAudio(list);
        if (rc)
            startMeter();
    }
    else
        rc = true;
}

void AudioScreen::setCurrentDir() {
        const char *curdir = fileview->currentDir();
        int lencurdir = strlen(curdir);
        int lenhomedir = strlen(fileview->homeDir());

        Skin * pSkin = control()->getSkin();
        if (!(pSkin->isValid(XML_SCREEN_AUDIO)))
        {
            if (lencurdir == lenhomedir)
                _currentDir->setText("/");
            else
                _currentDir->setText(curdir + lenhomedir);

        _currentDir->drawNow();
    }
}

void AudioScreen::playAudio(int idx) {
	int count=0;

		MListViewItem *itemLast = fileview->first();
        MString dir = fileview->currentDir();
        dir += "/";
		if( idx ) idx--;

        for (MListViewItem *item = fileview->first(); item && count < idx ; item = fileview->next()) {
			itemLast = item;
            if (item->label()[0] == '[')
                continue;
            count++;
        }
		fileview->setSelected(itemLast);

		if ( _audioControl->playAudio(fileview->file(itemLast)) )
			startMeter();
}

void AudioScreen::playAudio() {
		MListViewItem *item = fileview->current();
		bool isDir = fileview->currentFileIsDir();
		if (!item) {
			item = fileview->selected();
			isDir = false;
		}
		if(!item){
			item = fileview->first();
			isDir = false;
		}
		if (!item)
			return;
		fileview->setSelected(item);

		if (!isDir) {
			if (_audioControl->playAudio(fileview->file(item)))
				startMeter();
		}
		else {
			fileview->addToPath(item->description());
			setCurrentDir();
			// draw now because of download message
			fileview->drawNow();
			playAll();
    }
}

void AudioScreen::stopAudio() {
    _audioControl->stopAudio();
    stopMeter();
}

bool AudioScreen::focus() {
    return fileview->focus();
}

void AudioScreen::startMeter()
{
    _timer.start(200, MTimer::eForever);
}

void AudioScreen::stopMeter()
{
    _timer.stop();
}


