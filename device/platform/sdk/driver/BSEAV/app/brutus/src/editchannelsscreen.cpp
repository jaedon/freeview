/***************************************************************************
 *     Copyright (c) 2002-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: editchannelsscreen.cpp $
 * $brcm_Revision: 37 $
 * $brcm_Date: 8/8/11 1:34p $
 *
 * Module Description: Config is pvrgui's registry
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/editchannelsscreen.cpp $
 * 
 * 37   8/8/11 1:34p xhuang
 * SW7552-70: Keep the dvbt/isdbt status
 * 
 * SW7552-70/1   8/5/11 6:23p jianweiz
 * SW7552-70: Keep the dvbt/isdbt status
 * 
 * 36   4/12/10 7:33p nitinb
 * SW7550-339: Enable "channel edit" functionality in Brutus for OFDM
 * input
 *
 * 35   11/15/08 7:11a agin
 * PR48985: Support LDPC and Turbo at the same time.
 *
 * 34   2/1/08 4:59p katrep
 * PR39180: Added support for ofdm/DVB-T THD tuner
 *
 * 33   1/28/08 11:43a jtna
 * PR29193: Rework channeleditscreen behavior
 *
 * 32   9/4/07 11:08a yrajakar
 * PR34578: Adding support for RTP FEC
 *
 * 31   5/3/07 2:17p mward
 * PR30301: add QAM16/32 support
 *
 * 30   4/19/07 6:10p jrubio
 * PR30010: add IP_HTTP
 *
 * 29   2/27/07 6:53p gmohile
 * PR 27894: divide the default symbol rate value returned for QAM by
 * MEGA_F
 *
 * 28   2/2/07 10:06a erickson
 * PR26661: call MApplication::clearModalState after leaving modal loop
 *
 * 27   1/29/07 11:17a ssood
 * PR24918: Channel Map Redesign to make its access Thread-safe
 *
 * 26   12/1/06 9:58a ssood
 * PR26252: IP STB SW unification w/ Generic Brutus release
 *
 * 25   9/28/06 7:00p arbisman
 * PR24289: Back-out changes
 *
 * 23   8/3/06 5:44p rjlewis
 * PR23249: compiler warnings.
 *
 * 23   8/3/06 5:41p rjlewis
 * PR23249: compiler warnings.
 *
 * 23   8/3/06 5:38p rjlewis
 * PR23249: compiler warnings.
 *
 * 22   7/25/06 5:23p jjordan
 * PR22906: Integrate liveMedia open-source for RTP and RTCP support
 *
 * 21   6/19/06 3:47p erickson
 * PR19526: added ChannelMapEntry::setType to force initialization of
 * Settop API frontend params structures when type changes
 *
 * 20   6/8/06 5:44p vle
 * PR 19699: fix incorrect usage on indexes. Change variable name to
 * prevent further confusion.
 *
 * 19   4/20/06 1:42p jjordan
 * PR17547: add basic RTP support to NET_IF DMA kernel-mode driver
 *
 * 18   2/22/06 4:42p erickson
 * PR17108: remove warnings
 *
 * 17   2/14/06 11:42a rjlewis
 * PR19044: can't use non-constant initializers in old vxworks compiler.
 * Also lined up column headings better.
 *
 * 16   2/8/06 11:40a jgarrett
 * PR 19439: Fixing stack corruption on BCM7400 when creating edit channel
 * screen.
 *
 * 15   12/19/05 5:00p rjlewis
 * PR17610: made edit screen focusable.
 *
 * 14   10/12/05 5:34p jjordan
 * PR17547: Add support for RTP protocols
 *
 * 13   10/3/05 11:18a erickson
 * PR17108: fixed linux 2.6 compilation warnings
 *
 * 12   8/24/05 6:14p arbisman
 * PR16678: Add support for WinCE
 *
 * 11   8/15/05 4:22p erickson
 * PR16494: mark modified flag on rearrange
 *
 * 10   8/2/05 11:35a erickson
 * PR16201: added streamer to listbox
 *
 * 9   6/29/05 9:04a jjordan
 * PR16042: Add support for "IP channels"
 *
 * 8   6/9/05 3:53p erickson
 * PR15524: removed Load and added confirm dialog to Save
 *
 * 7   4/19/05 3:46p erickson
 * PR14701: make digital subchannel support standard for all platforms
 *
 * 6   4/14/05 2:31p erickson
 * PR14696: cleaned up warnings
 *
 * 5   4/4/05 1:30p erickson
 * PR13823: removed B_HAS_XXXX frontend #defines. use bconfig for runtime
 * check if really needed.
 *
 * 4   3/29/05 10:34p erickson
 * PR14593: fixed Edit Channels for 3560
 *
 * 3   3/29/05 4:58p erickson
 * PR14593: 3560 work
 *
 * 2   3/28/05 4:58p erickson
 * PR11759: added multiple linein support
 *
 * 1   2/7/05 8:06p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/43   2/4/05 10:47a erickson
 * PR12728: cleanup for -pedantic warnings and errors
 *
 * Irvine_BSEAVSW_Devel/42   8/18/04 11:31a erickson
 * PR10832: updated default QAM symbol rate code
 *
 * Irvine_BSEAVSW_Devel/41   8/17/04 4:41p erickson
 * PR11770: removed brutus inputband support. added streamer as a
 * registered frontend resource, with a mapping between index number and
 * settop api id.
 *
 * Irvine_BSEAVSW_Devel/40   8/9/04 3:48p erickson
 * PR12104: extended SDS to handle DSS_PES and DSS_ES, along with STREAMER
 * and INPUTBAND
 *
 * Irvine_BSEAVSW_Devel/39   7/28/04 12:00p erickson
 * PR11768: added widget names for debug
 *
 * Irvine_BSEAVSW_Devel/38   7/21/04 3:17p erickson
 * PR11682: added ChannelMapEntry constructor
 *
 * Irvine_BSEAVSW_Devel/37   7/21/04 10:09a erickson
 * PR11682: move channels.txt support out of ChannelMap and into
 * PSIChannelScan
 *
 * Irvine_BSEAVSW_Devel/36   7/14/04 1:36a erickson
 * PR11287: refactor linux-specific code into brutus_linux.cpp for easier
 * porting
 *
 * Irvine_BSEAVSW_Devel/35   6/28/04 8:48a erickson
 * PR11135: fixed edit channels ui
 *
 * Irvine_BSEAVSW_Devel/34   5/28/04 8:45a erickson
 * PR11243: amodified STREAMER and STREAMER_DSS to support streamer
 * indexes, not just INPUTBAND synonym
 *
 * Irvine_BSEAVSW_Devel/33   4/30/04 12:05p erickson
 * PR10832: rework of brutus to support single AVManager and
 * ChannelManager for system
 *
 * Irvine_BSEAVSW_Devel/32   3/23/04 5:35p erickson
 * PR9753: added raise() call to set zorder for dialog box. also, made
 * atof's safer.
 *
 * Irvine_BSEAVSW_Devel/31   3/3/04 6:33p erickson
 * PR9937: make sure SDS-only gui components are consistently SDS-only
 *
 * Irvine_BSEAVSW_Devel/30   2/13/04 4:19p erickson
 * PR9465: added VSB support to ui
 *
 * Irvine_BSEAVSW_Devel/29   2/13/04 12:05p erickson
 * PR9465: updated channelmgr naming and comments
 *
 * Irvine_BSEAVSW_Devel/28   2/9/04 5:05p erickson
 * PR8850: acqpanel is transparent, also I shorted some ui-only names
 *
 * Irvine_BSEAVSW_Devel/27   1/29/04 2:26p erickson
 * PR9107: merging back from BRUTUS_PR9107 branch for bwin development
 *
 * Irvine_BSEAVSW_Devel/26   1/7/04 11:37a erickson
 * PR8995: Used 20.00 to prevent appearance of change
 *
 * Irvine_BSEAVSW_Devel/25   1/7/04 11:26a erickson
 * PR8363: extended one of the widgets to not truncate AnnexA/B text
 *
 * Irvine_BSEAVSW_Devel/24   1/7/04 11:18a erickson
 * PR8363: The user is now warned any time the EditChannels screen is
 * exited if the channel map hasn't been saved. Before it only worked
 * with the "Back" button.
 *
 * Irvine_BSEAVSW_Devel/23   12/11/03 12:56p erickson
 * PR8868: don't print a frequency for linein
 *
 * Irvine_BSEAVSW_Devel/22   11/25/03 2:10p erickson
 * PR8583: when adapting to the new api change, I didn't need to regard
 * stream->float as Hz. It's still MHz.
 *
 * Irvine_BSEAVSW_Devel/21   11/19/03 4:57p erickson
 * PR8583: resolved some naming issues, especially bvideo_format
 *
 * Irvine_BSEAVSW_Devel/20   11/6/03 5:12p erickson
 * PR8570: initial vxworks migration
 *
 * Irvine_BSEAVSW_Devel/PR8570/1   11/6/03 5:05p erickson
 * initial vxworks migration
 *
 * Irvine_BSEAVSW_Devel/19   10/31/03 11:58a erickson
 * settop api changes after internal review
 *
 * Irvine_BSEAVSW_Devel/18   10/29/03 10:30a erickson
 * added QAM1024 support
 *
 * Irvine_BSEAVSW_Devel/17   10/28/03 11:25a erickson
 * settop api reworks after internal design view
 *
 * Irvine_BSEAVSW_Devel/16   10/22/03 11:22a erickson
 * settop api pass-through type conversion
 *
 * Irvine_BSEAVSW_Devel/15   10/16/03 4:54p erickson
 * PR8363: fixed support for modified flag
 *
 * Irvine_BSEAVSW_Devel/14   10/13/03 11:47a erickson
 * fixed polarity radio buttons
 * need to hide annex for sds
 *
 * Irvine_BSEAVSW_Devel/13   10/13/03 10:35a erickson
 * hide the center freq for linein and set it to 0
 *
 * Irvine_BSEAVSW_Devel/12   10/13/03 9:48a erickson
 * set the stream->type for new channels
 *
 * Irvine_BSEAVSW_Devel/11   10/10/03 1:43p erickson
 * converted brutus frontend code so that it can support all frontend
 * types simultaneously. this
 * is in preperation for 97038 and others.
 *
 * Irvine_BSEAVSW_Devel/10   9/19/03 1:05p erickson
 * when adding a channel, hide the delete button and cancel should remove
 * the added channel
 *
 * Irvine_BSEAVSW_Devel/9   8/22/03 5:20p erickson
 * venom2 settop api work
 *
 * Irvine_BSEAVSW_Devel/8   8/21/03 9:43a erickson
 * venom2 settop api work
 *
 * Irvine_BSEAVSW_Devel/7   8/12/03 3:46p erickson
 * fixed problems on other compilers
 *
 * Irvine_BSEAVSW_Devel/6   7/23/03 4:00p erickson
 * renamed from pvrgui to brutus
 *
 * Irvine_BSEAVSW_Devel/5   7/18/03 11:49a erickson
 * channelmgr rework to support new scan methods, specifying programs or
 * pids in the channel map
 *
 * Irvine_BSEAVSW_Devel/4   7/16/03 8:53a erickson
 * 7328 support
 *
 * Irvine_BSEAVSW_Devel/3   6/9/03 1:24p erickson
 * must set isDigital flag for 71xx
 *
 * SanJose_Linux_Devel/14   5/13/03 2:35p erickson
 * New 73xx channels are digital
 * if user cancels save, don't refresh and scan
 *
 * SanJose_Linux_Devel/13   4/15/03 12:39p erickson
 * 7115 work, added more build conditionals
 *
 * SanJose_Linux_Devel/12   4/11/03 2:17p erickson
 * 7115 work
 *
 * SanJose_Linux_Devel/11   4/10/03 4:22p erickson
 * info panel rework
 * added edit channels and outputs save-state feature
 *
 * SanJose_Linux_Devel/10   4/9/03 11:20a erickson
 * rework of pvrgui files and class names
 *
 ****************************************************************************/
#include "editchannelsscreen.h"
#include "control.h"
#include "channelmap.h"
#include <mmessagebox.h>
#include <mapplication.h>
#include <mradiobuttongroup.h>
#include "mlistviewdialog.h"
#include "marrowbar.h"
#include "cfgsettings.h"

#ifndef _WIN32_WCE
    #include <unistd.h>
#endif

#undef MAP
#define MAP() control()->channelMap()
#define SCAN() control()->channelManager()->channelScan()
#define CHANNELFILE control()->cfg()->get(Config::CHANNEL_MAP)
#define BEVEL_WIDTH 5

#define MEGA   1000000
#define MEGA_F 1000000.0

typedef struct {
    btuner_stream_type type;
    long value;
    const char *name;
    int ui_index; // index in the widget, -1 if not used
} stream_select_t;

stream_select_t g_stream_select[] = {
    {btuner_sds_stream, btuner_sds_mode_qpsk_dvb, "QPSK_DVB", -1},
    {btuner_sds_stream, btuner_sds_mode_qpsk_dss, "QPSK_DSS", -1},
    /* TODO: to support QPSK_DSS_PES, we'll need to refactor this.
    The sds tuner type isn't enough to specify the mpeg_type. */
    {btuner_sds_stream, btuner_sds_mode_qpsk_ldpc, "QPSK_LDPC", -1},
    {btuner_sds_stream, btuner_sds_mode_8psk_ldpc, "8PSK_LDPC", -1},
    {btuner_sds_stream, btuner_sds_mode_ldpc, "LDPC", -1},
    {btuner_sds_stream, btuner_sds_mode_qpsk_turbo, "QPSK_TURBO", -1},
    {btuner_sds_stream, btuner_sds_mode_8psk_turbo, "8PSK_TURBO", -1},
    {btuner_sds_stream, btuner_sds_mode_turbo, "TURBO", -1},
    {btuner_qam_stream, 16, "QAM16", -1},
    {btuner_qam_stream, 32, "QAM32", -1},
    {btuner_qam_stream, 64, "QAM64", -1},
    {btuner_qam_stream, 256, "QAM256", -1},
    {btuner_qam_stream, 1024, "QAM1024", -1},
    {btuner_analog_stream, -1, "ANALOG", -1},
    /* note: this is replicated from 'channelmap.cpp' due to a vxworks compiler problem.
       on error: "initializer element is not constant" it sets this entire array to null. */
#if BCHP_CHIP == 7038
    {btuner_analog_stream, 0, "COMPOSITE", -1},
    {btuner_analog_stream, 1, "SVIDEO",    -1},
    {btuner_analog_stream, 2, "COMPONENT", -1},
#elif BCHP_CHIP == 3560
    {btuner_analog_stream, 0, "COMPOSITE1", -1},
    {btuner_analog_stream, 1, "COMPOSITE2", -1},
    {btuner_analog_stream, 2, "COMPOSITE3", -1},
    {btuner_analog_stream, 3, "COMPOSITE4", -1},
    {btuner_analog_stream, 4, "SVIDEO1",    -1},
    {btuner_analog_stream, 5, "SVIDEO2",    -1},
    {btuner_analog_stream, 6, "COMPONENT",  -1},
#else
    {btuner_analog_stream, 0, "LINEINPUT", -1},
#endif
    {btuner_ip_stream, bsettop_ip_protocol_udp, "IP_UDP", -1},
    {btuner_ip_stream, bsettop_ip_protocol_rtp, "IP_RTP", -1},
    {btuner_ip_stream, bsettop_ip_protocol_http, "IP_HTTP", -1},
    {btuner_ip_stream, bsettop_ip_protocol_rtp_fec, "IP_FEC", -1},
    {btuner_vsb_stream, 8, "VSB8", -1},
    {btuner_vsb_stream, 16, "VSB16", -1},
    {btuner_streamer_stream, btuner_streamer_stream, "STREAMER", -1},
    {btuner_ofdm_stream, 0, "OFDM", -1}
};
#define TOTAL_STREAM_SELECTIONS sizeof(g_stream_select)/sizeof(stream_select_t)

static int find_index(int listbox_index) {
    for (unsigned i=0;i<TOTAL_STREAM_SELECTIONS;i++)
        if (g_stream_select[i].ui_index == listbox_index)
            return i;
    return -1;
}

class EditChannel : public MLabel, public MButtonEventHandler,
    public MRadioButtonGroupEventHandler,
    public MListBoxEventHandler {
public:
    EditChannel(EditChannelsScreen *scr);
    void edit(int index, bool addmode = false);
    void setDefaultSymbolRate();

    // events
    void onClick(MButton *sender);
    void onClick(MRadioButtonGroup *sender);
    void onItemClicked(MListBox *self);

protected:
    bool _done, _addmode;
    EditChannelsScreen *_scr;
    int _index;

    MListBox *_mode;
    MCheckBox *_enabled;
    MLineEdit *_freq, *_symbolRate, *_band;
    MLabel *_symbolRateLabel, *_freqLabel, *_bandwidthLabel;
    int *_streamerpanelIndex;
    MRadioButtonGroup *_polarity;
    MCheckBox *_tone;
    MRadioButtonGroup *_annex;
    MRadioButtonGroup *_mpegtype;
    MArrowBar *_bandwidth;
    MRadioButtonGroup *_ofdmMode;
    MCheckBox *_tunePriority;
    MWidget _acqpanel, _streamerpanel;
    MPushButton _delete, _ok, _cancel;
};

#define PANELSIZE MRect(160,45,275,155)
EditChannel::EditChannel(EditChannelsScreen *scr) :
    MLabel(scr->fb(), MRect(110,60,450,250), NULL, "editchannel"),
    _acqpanel(this, PANELSIZE),
    _streamerpanel(this, PANELSIZE),
    _delete(this, MRect(10,200,120,40), "Delete", "deletebtn"),
    _ok(this, MRect(180,200,120,40), "OK", "okbtn"),
    _cancel(this, MRect(310,200,120,40), "Cancel", "cancelbtn")
{
    setBackgroundColor(scr->cfg()->getInt(Config::BACKGROUND_COLOR));
    _ok.addHandler(this);
    _cancel.addHandler(this);
    _delete.addHandler(this);
    _scr = scr;
    setBevel(5);
    setFocusable(true);

    // _acqpanel panel, covers QAM, SDS and VSB
    _freq = new MLineEdit(&_acqpanel, MRect(0,0,110,30), "freq"),
    _freq->setDeleteDigitsOnly(true);
    _symbolRate = new MLineEdit(&_acqpanel, MRect(0,40,110,30), "symbolrate"),
    _symbolRate->setDeleteDigitsOnly(true);

    // satellite only
    _polarity = new MRadioButtonGroup(&_acqpanel, MRect(0,80,70,60), "polarity"),
    _polarity->add("13V");
    _polarity->add("18V");
    _tone = new MCheckBox(&_acqpanel, MRect(75,80,130,30), "22KHz", "tone");

    // analog/qam only
    _annex = new MRadioButtonGroup(&_acqpanel, MRect(0,80,115,60), "annex"),
    _annex->addHandler(this);

    _freqLabel = new MLabel(&_acqpanel, MRect(_freq->right()+5,_freq->y()+2,120,_freq->height()), "Center Freq");
    _symbolRateLabel = new MLabel(&_acqpanel,
        MRect(_symbolRate->right()+5,_symbolRate->y()+2,120,_symbolRate->height()), "Symbol Rate");

    // inputband panel
    _band = new MLineEdit(&_streamerpanel, MRect(0,0,120,30), "streamer"),
    _band->setText("0");
    new MLabel(&_streamerpanel, MRect(_band->right()+5,_band->y()+2,100,_band->height()), "Streamer");
    _streamerpanel.hide();
    _mpegtype = new MRadioButtonGroup(&_streamerpanel, MRect(0,60,150,80), "mpegtype"),
    _mpegtype->add("TS");
    _mpegtype->add("DSS_ES");
    _mpegtype->add("DSS_PES");

    // not on a panel
    _enabled = new MCheckBox(this, MRect(160,15,120,30), "Enabled", "editchanenabled"),
    _mode = new MListBox(this, MRect(10,35,130,150), 15, "mode"),
    new MLabel(this, MRect(_mode->x(), _mode->y()-25, _mode->width(), 25), "Mode");

    // OFDM only
    _bandwidth = new MArrowBar(&_acqpanel, MRect(0,40,110,30));
    _bandwidth->addHandler(this);
    _bandwidth->addItem("5 MHz");
    _bandwidth->addItem("6 MHz");
    _bandwidth->addItem("7 MHz");
    _bandwidth->addItem("8 MHz");

    _bandwidthLabel = new MLabel(&_acqpanel,
        MRect(_symbolRate->right()+5,_symbolRate->y()+2,120,_symbolRate->height()), "Bandwidth");       
    _ofdmMode = new MRadioButtonGroup(&_acqpanel, MRect(120,80,110,60), "ofdmMode"),
    _ofdmMode->add("dvbt");
    _ofdmMode->add("isdbt");
    _ofdmMode->addHandler(this);
    
    _tunePriority = new MCheckBox(this, MRect(_enabled->right()+5,15,150,30), "High Priority", "tune priority");
    
  // this makes sure the MListBox comes right before all its items
    _mode->setTabOrder(-1);
    _mode->scrollBar()->setButtons(false);
    _mode->scrollBar()->setIndicator(true);
    _mode->setTotalVisible(7);
    _mode->setHighlightCurrent(true);
    _mode->setRequireClickToMakeCurrent(true);
    _mode->captureKeys(true, true);
    _mode->addHandler(this);

    for (unsigned i=0;i<TOTAL_STREAM_SELECTIONS;i++) {
        bool use = false;
        switch (g_stream_select[i].type) {
        case btuner_sds_stream:
            if (bconfig->features.sds) use = true;
            break;
        case btuner_qam_stream:
            if (bconfig->features.qam) use = true;
            break;
        case btuner_analog_stream:
            if (bconfig->features.analog) use = true;
            break;
        case btuner_vsb_stream:
            if (bconfig->features.vsb) use = true;
            break;
        case btuner_ofdm_stream:
            if (bconfig->features.ofdm) use = true;
            break;
        case btuner_streamer_stream:
            use = true;
            break;
                case btuner_ip_stream:
                        use = true;
                        break;
        }
        if (!use) {
            g_stream_select[i].ui_index = -1;
        }
        else {
            g_stream_select[i].ui_index = _mode->total();
            _mode->add(g_stream_select[i].name);
        }
    }
}

void EditChannel::edit(int index, bool addmode)
{
    char buf[50];
    _index = index;
    _addmode = addmode;
    ChannelMap::ChannelMapEntry entry;
    if (_scr->MAP()->getEntry(index, &entry) == false)
                return;

    if (addmode)
        _delete.hide();
    else
        _delete.show();

    // look up the entry in the list
    bool found = false;
    for (unsigned i=0;i<TOTAL_STREAM_SELECTIONS;i++) {
        if (g_stream_select[i].type == entry.getType()) {
            switch (entry.getType()) {
            case btuner_sds_stream:
                found = entry.sds.params.mode == g_stream_select[i].value;
                break;
            case btuner_qam_stream:
                found = entry.qam.mode == g_stream_select[i].value;
                break;
            case btuner_streamer_stream:
                found = entry.getType() == g_stream_select[i].value;
                break;
            case btuner_analog_stream:
                found = entry.analog.linein == g_stream_select[i].value;
                break;
            case btuner_vsb_stream:
                found = entry.vsb.mode == g_stream_select[i].value;
                break;
            case btuner_ofdm_stream:
                found = 1;/* entry.ofdm.bandwidth == g_stream_select[i].value; */
                break;
            case btuner_ip_stream:
                found = entry.ip.params.open_params.protocol == g_stream_select[i].value;
                break;
            default: return; /* compiler warning */
            }
            if (found) {
                _mode->setCurrentIndex(g_stream_select[i].ui_index); 
                break;
            }
        }
    }
    onItemClicked(_mode); // set Annex radiobuttongroup
    _enabled->setChecked(!entry.disabled);

    switch (entry.getType()) {
    case btuner_sds_stream:
        sprintf(buf, "%0.2f", entry.sds.params.symbol_rate / MEGA_F);
        _symbolRate->setText(buf);
        sprintf(buf, "%0.2f", entry.freq);
        _freq->setText(buf);
        _polarity->click(entry.sds.params.diseqc_polarity==13?0:1);
        _tone->setChecked(entry.sds.params.diseqc_tone);
        break;
    case btuner_vsb_stream:
        sprintf(buf, "%f", entry.vsb.symbol_rate / MEGA_F);
        _symbolRate->setText(buf);
        sprintf(buf, "%0.2f", entry.freq);
        _freq->setText(buf);
        break;
    case btuner_ofdm_stream:
        sprintf(buf, "%d", entry.ofdm.bandwidth);
        sprintf(buf, "%0.2f", entry.freq);
        _freq->setText(buf);
        _ofdmMode->click(entry.ofdm.mode);
        _annex->click(entry.ofdm.cci_mode);
        _tunePriority->setChecked(entry.ofdm.priority == btuner_ofdm_priority_high?1:0);
        _bandwidth->setCurrent(entry.ofdm.bandwidth % btuner_ofdm_bandwidth_5Mhz);
        break;
    case btuner_qam_stream:
        sprintf(buf, "%f", entry.qam.symbol_rate / MEGA_F);
        _symbolRate->setText(buf);
        sprintf(buf, "%0.2f", entry.freq);
        _freq->setText(buf);
        _annex->click(entry.qam.annex);
        break;
    case btuner_analog_stream:
        sprintf(buf, "%0.2f", entry.freq);
        _freq->setText(buf);
        _annex->click(entry.analog.params.video_format==bvideo_format_pal?0:1);
        break;
    case btuner_streamer_stream:
        switch (entry.streamer.mpeg_type) {
        case bstream_mpeg_type_dss_es: _mpegtype->click(1); break;
        case bstream_mpeg_type_dss_pes: _mpegtype->click(2); break;
        default: _mpegtype->click(0); break;
        }
        break;
    case btuner_ip_stream:
        printf("Editing a IP Channel is not yet supported\n");
        return;
    default: return; /* compiler warning */
    }

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

void EditChannel::setDefaultSymbolRate() {
    const char *str = NULL;
    char buf[20];
    int index = find_index(_mode->currentIndex());
    switch (g_stream_select[index].type) {
    case btuner_qam_stream:
        {
        int symrate = b_get_default_qam_symrate(
            _annex->checked() == 0?btuner_qam_annex_a:btuner_qam_annex_b,
            (btuner_qam_mode)(g_stream_select[index].value));
        sprintf(buf, "%f", symrate/MEGA_F);
        str = buf;
        }
        break;
    case btuner_sds_stream:
        str = "20.00";
        break;
    case btuner_vsb_stream:
        str = "5.38119";
        break;
    default: return; /* compiler warning */
    }
    _symbolRate->setText(str);
}

/* when the annex changes, change the default symbolerate */
void EditChannel::onClick(MRadioButtonGroup *sender) {
    setDefaultSymbolRate();
}

#define SET_VISIBLE(PWIDGET,SHOW) \
    do {if (SHOW) (PWIDGET)->show(); else (PWIDGET)->hide();} while (0)

/* when the mode changes, change the right hand panel */
void EditChannel::onItemClicked(MListBox *self) {
    int index = find_index(_mode->currentIndex());
    stream_select_t *select = &g_stream_select[index];
    btuner_stream_type type = select->type;
    bool islinein = type == btuner_analog_stream && select->value;
    bool acq = type == btuner_sds_stream ||
        type == btuner_qam_stream ||
        type == btuner_vsb_stream;

    SET_VISIBLE(&_streamerpanel, type == btuner_streamer_stream);
    SET_VISIBLE(_symbolRate, acq);
    SET_VISIBLE(_symbolRateLabel, acq);
    SET_VISIBLE(&_acqpanel, acq || type == btuner_analog_stream || type == btuner_ofdm_stream);
    SET_VISIBLE(_polarity, type == btuner_sds_stream);
    SET_VISIBLE(_tone, type == btuner_sds_stream);
    SET_VISIBLE(_annex, type != btuner_sds_stream || type == btuner_ofdm_stream);
    SET_VISIBLE(_freq, !islinein || type == btuner_ofdm_stream);
    SET_VISIBLE(_freqLabel, !islinein || type == btuner_ofdm_stream);
    SET_VISIBLE(_bandwidth, type == btuner_ofdm_stream);
    SET_VISIBLE(_bandwidthLabel, type == btuner_ofdm_stream);
    SET_VISIBLE(_tunePriority, type == btuner_ofdm_stream);
    SET_VISIBLE(_ofdmMode, type == btuner_ofdm_stream);

    int i = _annex->checked();
    _annex->clear();
    // this makes sure the MRadioButtonGroup comes right before all its items
    _annex->setTabOrder(-1); // put at end

    switch (type) {
    case btuner_analog_stream:
        _annex->add("PAL");
        _annex->add("NTSC");
        break;
    case btuner_qam_stream:
        _annex->add("AnnexA");
        _annex->add("AnnexB");
        break;
    case btuner_ofdm_stream:
        _annex->add("CCI off");
        _annex->add("CCI Auto");
        // Set the default options
        /*_ofdmMode->click(btuner_ofdm_mode_dvbt);*/
        _bandwidth->setCurrent(btuner_ofdm_bandwidth_8Mhz % btuner_ofdm_bandwidth_5Mhz);
        _tunePriority->setChecked(true);
        i = btuner_ofdm_cci_mode_none;
        break;
    default: return; /* compiler warning */
    }
    if (i < 0)
        i = 0;
    _annex->click(i);

    setDefaultSymbolRate();

    if (islinein) _freq->setText("0.0");
}

void EditChannel::onClick(MButton *sender) {
    if (sender == &_ok) {
        ChannelMap::ChannelMapEntry entry;
        if (_scr->editChannel(_index, &entry) == false)
                        return;
        entry.disabled = !_enabled->checked();

        // input band
        int index = find_index(_mode->currentIndex());
        stream_select_t *select = &g_stream_select[index];
        entry.setType(select->type);
        switch (entry.getType()) {
        case btuner_streamer_stream:
            switch (_mpegtype->checked()) {
            case 1: entry.streamer.mpeg_type = bstream_mpeg_type_dss_es; break;
            case 2: entry.streamer.mpeg_type = bstream_mpeg_type_dss_pes; break;
            default: entry.streamer.mpeg_type = bstream_mpeg_type_ts; break;
            }
            entry.streamer.index = atoi(_band->text());
            break;
        case btuner_sds_stream:
            if (_freq->text())
                entry.freq = atof(_freq->text());
            if (_symbolRate->text())
                entry.sds.params.symbol_rate = (unsigned)(atof(_symbolRate->text()) * MEGA);
            entry.sds.params.mode = (btuner_sds_mode)select->value;
            entry.sds.params.diseqc_polarity = (btuner_sds_diseqc_polarity)(_polarity->checked()?18:13);
            entry.sds.params.diseqc_tone = _tone->checked();
            break;
        case btuner_vsb_stream:
            if (_freq->text())
                entry.freq = atof(_freq->text());
            if (_symbolRate->text())
                entry.vsb.symbol_rate = (unsigned)(atof(_symbolRate->text()) * MEGA);
            entry.vsb.mode = (btuner_vsb_mode)select->value;
            break;
        case btuner_ofdm_stream:
            if (_freq->text())
                entry.freq = atof(_freq->text());
            entry.ofdm.bandwidth = (btuner_ofdm_bandwidth)(_bandwidth->current() + btuner_ofdm_bandwidth_5Mhz);
            entry.ofdm.mode = _ofdmMode->checked()?btuner_ofdm_mode_isdbt:btuner_ofdm_mode_dvbt;
            entry.ofdm.cci_mode = _annex->checked()?btuner_ofdm_cci_mode_auto:btuner_ofdm_cci_mode_none;
            entry.ofdm.priority = _tunePriority->checked()?btuner_ofdm_priority_high:btuner_ofdm_priority_low;	
            break;
        case btuner_qam_stream:
            entry.qam.mode = (btuner_qam_mode)select->value;
            if (_freq->text())
                entry.freq = atof(_freq->text());
            if (_symbolRate->text())
                entry.qam.symbol_rate = (unsigned)(atof(_symbolRate->text()) * MEGA);
            entry.qam.annex = _annex->checked()?
                btuner_qam_annex_b:btuner_qam_annex_a;
            break;
        case btuner_analog_stream:
            entry.analog.linein = select->value;
            if (_freq->text())
                entry.freq = atof(_freq->text());
            entry.analog.params.video_format = _annex->checked()?
                bvideo_format_ntsc:bvideo_format_pal;
            break;
        case btuner_ip_stream:
            entry.ip.params.open_params.protocol = (bsettop_ip_protocol)select->value;
            break;
        default: return; /* compiler warning */
        }

        /* now write back the modified entry */
        _scr->MAP()->updateEntry(_index, &entry);
        _scr->refresh();
        _done = true;
        _scr->_modified = true;
    }
    else if (sender == &_cancel) {
        _done = true;
        if (_addmode)
            _scr->deleteChannel(_index);
    }
    else if (sender == &_delete) {
        _scr->_modified = true;
        _scr->deleteChannel(_index);
        _done = true;
    }
}

//////////////////////////////////////////////////////

EditChannelsScreen::EditChannelsScreen(Control *acontrol) :
    GenericScreen(acontrol, NOTV, "editchannelsscreen"),
#define BTNWIDTH 100
#define BTNGAP (BTNWIDTH+15)
    _back(this,  MRect(80+BTNGAP*0,375,BTNWIDTH,40), "Back"),
    _save(this,  MRect(80+BTNGAP*1,375,BTNWIDTH,40), "Save"),
    _reset(this, MRect(80+BTNGAP*2,375,BTNWIDTH,40), "Reset"),
    _add(this,   MRect(80+BTNGAP*3,375,BTNWIDTH,40), "Add")
{
    _mgr = control()->channelManager();

    _ec = new EditChannel(this);

    new MLabel(this, MRect( 55,10,600,25), "Mode");
    new MLabel(this, MRect(180,10,600,25), "On");
    new MLabel(this, MRect(215,10,600,25), "Freq");
    new MLabel(this, MRect(315,10,600,25), "SymbolRate");
    new MLabel(this, MRect(445,10,600,25), "Type");

    MLabel *bevel = new MLabel(this, MRect(10,35,600,300));
    bevel->setBevel(BEVEL_WIDTH);

    _list = new MColumnListBox(bevel,
        MRect(BEVEL_WIDTH,BEVEL_WIDTH,bevel->width()-2*BEVEL_WIDTH,bevel->height()-2*BEVEL_WIDTH));
    _list->scrollBar()->setButtons(false);
    _list->scrollBar()->setIndicator(true);
    _list->setTotalVisible(7);
    _list->captureKeys(true, true);
    _list->setIndent(10);
    _list->addColumn(30);   // number
    _list->addColumn(130);  // mode
    _list->addColumn(30);   // disabled
    _list->addColumn(100);  // freq
    _list->addColumn(130);  // symbolrate
    _list->addColumn(70);   // annex or sdsmode
    new MLabel(this, MRect(20,335,600,25), "Use Chan +/- to rearrange");

    _list->addHandler(this);
    _back.addHandler(this);
    _save.addHandler(this);
    _reset.addHandler(this);
    _add.addHandler(this);
    _modified = false;

    refresh();
}

EditChannelsScreen::~EditChannelsScreen()
{
    delete _ec;
}

void EditChannelsScreen::checkModified()
{
    if (_modified)
        MMessageBox::info(fb(), "You have not saved or reset the channel map.");
}

void EditChannelsScreen::onClick(MButton *sender)
{
    if (sender == &_back) {
        control()->showScreen(Control::eAdmin);
    }
    else if (sender == &_save) {
        save();
        onClick(&_reset);
    }
    else if (sender == &_reset) {
        // reread from disk
        SCAN()->read(CHANNELFILE);
        // then refresh the gui
        refresh();
    }
    else if (sender == &_add) {
        /* Because ChannelMapEntry uses a union, if we add a new channel on a
        QAM/SDS platform (i.e. 97038), we're going to have undesirable defaults.
        Right now, SDS takes precedence in the default, so QAM defaults to
        Annex A. Maybe we should drop the union. */
        ChannelMap::ChannelMapEntry ch;
        MAP()->add(&ch);
        refresh();
        _ec->edit(_list->total()-1, true);
    }
}

void EditChannelsScreen::onItemClicked(MListBox *self)
{
    _ec->edit(_list->currentIndex());
}

void EditChannelsScreen::rearrange(int inc)
{
    int cur = _list->currentIndex();
    if (cur > 0 && inc < 0 || cur < _list->total()-1 && inc > 0) {
        MAP()->rearrange(cur, inc);
        refresh();
        _modified = true;
        _list->setCurrentIndex(cur+inc);
    }
}

void EditChannelsScreen::deleteChannel(int index)
{
    MAP()->deleteEntry(index);
    _list->remove(index);
}

bool EditChannelsScreen::editChannel(int index, ChannelMap::ChannelMapEntry *entry)
{
    return MAP()->getEntry(index, entry);
}

int EditChannelsScreen::refresh()
{
        int idx;

    _list->beginUpdates();
    _list->clear();
    _modified = false;
    for (int i=0;i<MAP()->total();i++) {
        char buf[512];
        ChannelMap::ChannelMapEntry entry;
        if (MAP()->getEntry(i, &entry) == false)
                        return -1;

        switch (entry.getType()) {
        case btuner_streamer_stream:
            snprintf(buf, sizeof(buf), "%d\tSTREAMER\t%c\t%s\t%d (TS%d_IN)",
                    i,
                    entry.disabled?'N':'Y',
                    bstream_mpeg_type_str(entry.streamer.mpeg_type),
                    entry.streamer.index,
                    entry.streamer.index+1
                    );
                break;
        case btuner_vsb_stream:
            snprintf(buf, sizeof(buf), "%d\t%s\t%c\t%0.2f\t%f",
                i,
                btuner_vsb_str(entry.vsb.mode),
                entry.disabled?'N':'Y',
                entry.freq,
                entry.vsb.symbol_rate / MEGA_F
                );
            break;
        case btuner_ofdm_stream:
            snprintf(buf, sizeof(buf), "%d\t%s\t%c\t%0.2f\t\t%s",
                i,
                btuner_ofdm_str(),
                entry.disabled?'N':'Y',
                entry.freq,
                btuner_ofdm_mode_str(entry.ofdm.mode)
                );
            break;
        case btuner_sds_stream:
            snprintf(buf, sizeof(buf), "%d\t%s\t%c\t%0.2f\t%0.2f",
                i,
                btuner_sds_str(entry.sds.params.mode, entry.sds.mpeg_type),
                entry.disabled?'N':'Y',
                entry.freq,
                entry.sds.params.symbol_rate / MEGA_F
                );
            break;
        case btuner_qam_stream:
            snprintf(buf, sizeof(buf), "%d\t%s\t%c\t%0.2f\t%f\t%s",
                i,
                btuner_qam_str(entry.qam.mode),
                entry.disabled?'N':'Y',
                entry.freq,
                entry.qam.symbol_rate / MEGA_F,
                btuner_qam_annex_str(entry.qam.annex));
            break;
        case btuner_analog_stream:
            if (entry.analog.linein >= 0)
                snprintf(buf, sizeof(buf), "%d\t%s\t\t(LINEINPUT\t%d)\t%s",
                    i,
                    b_lineinput_str[entry.analog.linein],
                    entry.analog.linein,
                    btuner_analog_format_str(entry.analog.params.video_format));
            else
                snprintf(buf, sizeof(buf), "%d\t%s\t%c\t%0.2f\t\t%s",
                    i,
                    btuner_analog_str(&entry),
                    entry.disabled?'N':'Y',
                    entry.freq,
                    btuner_analog_format_str(entry.analog.params.video_format));
            break;
        case btuner_ip_stream:
            idx = snprintf(buf, sizeof(buf), "%d\t%s\t%c\t",
                                i,
                                (entry.ip.params.open_params.protocol == bsettop_ip_protocol_udp)?"IP_UDP":
                                (entry.ip.params.open_params.protocol == bsettop_ip_protocol_rtp_fec)?"IP_FEC":
                           "IP_RTP",
                    entry.disabled?'N':'Y'
                    );
                        if (entry.ip.params.open_params.protocol == bsettop_ip_protocol_rtsp)
                        {
                    snprintf(buf+idx, sizeof(buf), "%s", entry.ip.params.open_params.url);
                        }
                        else
                        {
                    snprintf(buf+idx, sizeof(buf), "%s\t%d",
                                                entry.ip.params.open_params.ip_addr,
                                                entry.ip.params.open_params.port
                    );
                        }
            break;
        default: return -1; /* compiler warning */
        }
        buf[sizeof(buf)-1]='\0';
        _list->add(buf);
    }
    _list->endUpdates();
    return 0;
}

bool EditChannelsScreen::focus() {
    return _list->total()?_list->focus():_add.focus();
}

void EditChannelsScreen::save() {
#if 0
    const char *selectfile = control()->cfg()->get(Config::CHANNEL_MAP_SELECT_FILE);
    MString channelmap = control()->cfg()->get(Config::CHANNEL_MAP);
    if (selectfile) {
        char buf[256];
        sprintf(buf, "Do you want to save the channel map as '%s' (YES) or pick another name (NO)?",
            channelmap.s());
        if (MMessageBox::show(fb(), buf,
            MMessageBox::bYes|MMessageBox::bNo,MMessageBox::bYes) ==
            MMessageBox::bNo)
        {
            // pick name
            if (MMessageBox::input(fb(), "Enter channel map name", 200,
                MMessageBox::bOK|MMessageBox::bCancel, channelmap, -1) == MMessageBox::bOK)
            {
                MStringList list;
                MString liststr = control()->cfg()->get(Config::CHANNEL_MAP_LIST);

                MStringList::split(liststr, ',', list);
                if (list.index(channelmap) == -1)
                    list.add(channelmap);
                liststr = list.join(',').s();

                control()->cfg()->set(Config::CHANNEL_MAP, channelmap);
                control()->cfg()->set(Config::CHANNEL_MAP_LIST, liststr);
                writeChannelMapSelectFile();
            }
        }
    }
#endif

    if (SCAN()->write(CHANNELFILE)) {
        MMessageBox::info(fb(), "Error writing channel map");
    }
    else {
        control()->scan();
        _modified = false;
        MMessageBox::info(fb(), "Channel map saved");
    }
}

#if 0
void EditChannelsScreen::load() {
    const char *liststr = control()->cfg()->get(Config::CHANNEL_MAP_LIST);
    MString channelmap = control()->cfg()->get(Config::CHANNEL_MAP);

    if (liststr) {
        MListViewDialog dialog(this);
        MStringList list;

        MStringList::split(liststr, ',', list);
        for (const char *s = list.first(); s; s = list.next())
            dialog.add(s);
        const char *select = dialog.select();
        if (!select)
            return;

        channelmap = select;
        control()->cfg()->set(Config::CHANNEL_MAP, channelmap);
        writeChannelMapSelectFile();
    }

    // read from disk
    SCAN()->read(CHANNELFILE);
    // refresh the gui then scan
    refresh();
}

void EditChannelsScreen::writeChannelMapSelectFile() {
    const char *selectfile = control()->cfg()->get(Config::CHANNEL_MAP_SELECT_FILE);
    const char *channelmap = control()->cfg()->get(Config::CHANNEL_MAP);
    const char *channelmaplist = control()->cfg()->get(Config::CHANNEL_MAP_LIST);

    FILE *f = fopen(selectfile, "w+");
    fprintf(f, "CHANNEL_MAP=%s\n", channelmap);
    fprintf(f, "CHANNEL_MAP_LIST=%s\n", channelmaplist);
    fclose(f);
    b_sync_disk();
}
#endif

