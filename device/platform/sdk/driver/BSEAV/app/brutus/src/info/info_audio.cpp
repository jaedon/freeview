/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: info_audio.cpp $
 * $brcm_Revision: 71 $
 * $brcm_Date: 5/2/12 6:46p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/info/info_audio.cpp $
 * 
 * 71   5/2/12 6:46p jgarrett
 * SW7425-2802: Removing RAP_XXX build variables for audio post processing
 * in favor of runtime detection
 * 
 * 70   4/25/12 12:16p sgadara
 * SW7358-272: [7358] Support AVL on Brutus
 * 
 * 69   3/1/12 4:44p katrep
 * SW7429-77:correctly set the donmix mode
 * 
 * 68   2/22/12 1:35p jgarrett
 * SW7231-261: Not restarting decoder when changing settings that don't
 * require a restart
 * 
 * 67   2/22/12 1:19p jgarrett
 * SW7346-233: Always allowing compressed SPDIF selection.  If codec
 * doesn't support it, it's blocked in the settop layer
 * 
 * 66   2/2/12 1:57p katrep
 * SW7429-1:add dts encode support for 7429
 * 
 * 65   12/21/11 3:54p mward
 * SW7435-7:  Support Brutus build on (new, 7435-based) platform 97435.
 * 
 * 64   8/17/11 3:00p mward
 * SW7346-233: Reordering SPDIF output format options
 * 
 * 63   8/17/11 1:44p jgarrett
 * SW7346-233: Reordering HDMI output format options
 * 
 * 62   6/27/11 6:49p jgarrett
 * SW7346-233: Not selecting copmressed SPDIF output when disabled.  Also
 * removing unsupported transcode options for HDMI.
 * 
 * 61   6/6/11 1:41p jessem
 * SW7425-417: Enabled only those HDMI TX audio formats supported by the
 * connected HDMI device.
 *
 * 60   5/5/11 1:30p jgarrett
 * SW7425-516: Making compressed and PCM 5.1 options always available for
 * HDMI to avoid UI becoming out of sync.  They're blocked in settop api
 * when required.
 *
 * 59   4/19/11 11:16a jgarrett
 * SW7125-730: Fixing segfault with AC3 transcode option on SPDIF
 *
 * 58   3/7/11 10:26a mward
 * SW7125-687: Fix console message "requesting Compressed audio on SPDIF".
 *
 * 57   2/15/11 6:32p xhuang
 * SW7358-34: Add 7358/7552 support
 *
 * 56   12/28/10 3:17p hongtaoz
 * SW7425-49: add 7425 support;
 *
 * 55   12/15/10 10:42a jrubio
 * SW7344-9: add 7346/7344 support
 *
 * 54   10/21/10 5:00p nickh
 * SW7422-80: Add 7422 audio format support and audio encode
 *
 * 53   10/8/10 4:16p mananp
 *
 * SW7342-184:LPCM-DVD Compressed option is disabled for SPDIF/HDMI
 *
 * 52   10/5/10 11:49a mananp
 * SW7335-600:Disabled "Compressed" option for SPDIF for PCM audio
 *
 * 51   8/20/10 4:09p jgarrett
 * SW7408-52: Adding multichannel support for 7408
 *
 * 50   7/2/10 5:34p mward
 * SW7125-226:  7125 and 734x now support DTS encode.  Re-enable the
 * selection for it.
 *
 * 49   4/12/10 10:39a mward
 * SW7125-226:  (actually) Disabling DTS Encode for 7125, 7340, 7342.
 *
 * 48   4/8/10 5:26p mward
 * SW7125-226:  Disabling DTS Encode for 7125, 7340, 7342.
 *
 * 47   3/22/10 10:13a mward
 * SW7125-226:  Enabling AC3 Encode for 7125, 7340, 7342.
 *
 * 46   3/9/10 11:45a randyjew
 * SW7468-6: Adding 7468 support
 *
 * 45   2/11/10 11:13a jgarrett
 * SW7550-246: Comparing strings for SPDIF/HDMI format instead of indexes.
 *
 * 44   1/27/10 10:01a jgarrett
 * SW7550-213: Reordering dualmono values
 *
 * SW7550-213/1   1/25/10 5:23p jianweiz
 * SW7550-213: Change dual mono index to adapt the correct settop dual
 * mono define
 *
 * 43   1/16/10 3:16p rjain
 * SW7550-147: Enabling PCM 5.1 on HDMI for 7550
 *
 * 42   1/16/10 2:20p rjain
 * SW7550-190: Enabling AC3 Encode for 7550
 *
 * 41   1/11/10 7:22p mward
 * SW7125-162: Enable PCM 5.1 on 7125, 7340, 7342.
 *
 * 40   12/1/09 7:26p katrep
 * SW7405-3515:Add downmix modes for ACC formats
 *
 * 39   9/11/09 11:40a jjordan
 * SW7405-3017: add option to force compressed audio on HDMI
 *
 * 38   8/24/09 4:11p katrep
 * SW7405-2760:Add DTS Broadcast-DTS encode feature.
 *
 * 37   8/10/09 1:40p jrubio
 * PR55232: add 7340/7342
 *
 * 36   7/17/09 6:06p katrep
 * PR56749: No PCM 5.1 audio on PCM AVI streams
 *
 * 35   6/30/09 12:17p mward
 * PR52051: PR52051: PR54311:  PR54312: Dialog Norm. and Compression
 * controls with SRS / Dolby Volume.  Can't set audio settings while
 * audio is detached, have to detach/attach to restart to apply after
 * changing settings.
 * PR52051:  Correcting size and location of panel for compression and
 * dialog normalization controls.
 * PR 52811: AAC audio dosen't output from 7401 board though it connects
 * the TV supporting AAC by HDMI.
 * PR52051:  Branch for testing dialog normalization switch with new RAP
 * release.
 *
 * 34   6/16/09 11:37a vishk
 * PR 52811: AAC audio dosen't output from 7401 board though it connects
 * the TV supporting AAC by HDMI.
 *
 * 33   5/22/09 12:01p nickh
 * PR53563: Add 7420 support for AC3/DTS encode, and PCM 5.1 for HDMI.
 * Also see PR's 53564, 53603.
 *
 * 32   3/26/09 5:41p katrep
 * PR53630: Fixed issue introduced for Ac3/DTS encode by #31
 *
 * 31   3/20/09 3:58p vishk
 * PR 52811: AAC audio dosen't output from 7401 board though it connects
 * the TV supporting AAC by HDMI.
 *
 * 30   3/20/09 1:09p katrep
 * PR52340: Support srs true volume and dolby volume through brutus ans
 * settop api
 *
 * 29   3/13/09 5:40p mward
 * PR49827: PR52490: Merge from main.
 * PR52490: Change name of SRS VIQ to SRS TrueVolume
 * PR50613: Add support for SRS Volume IQ.
 * PR49827: Separate Dolby post-processing and codec settings.
 * PR49827: Add a Dolby Volume checkbox in Info:Audio.
 *
 * PR49827/5   2/26/09 1:29p mward
 * PR52490: Merge from main.
 *
 * 28   2/4/09 10:30a jrubio
 * PR51629: add 7336 support
 *
 * 27   1/8/09 5:11p katrep
 * PR50063: DTS and Ac3 encode are valid only for AAC audio formats
 *
 * 26   1/8/09 12:21p katrep
 * PR50063: Add suppport dor Ac3 encode in Settop shim and burtus.
 *
 * PR49827/4   2/25/09 12:20p mward
 * PR52490: Change name of SRS VIQ to SRS TrueVolume
 *
 * PR49827/3   1/13/09 5:13p mward
 * PR50613: Add support for SRS Volume IQ.
 *
 * PR49827/2   12/5/08 2:41p mward
 * PR49827: Separate Dolby post-processing and codec settings.
 *
 * PR49827/1   12/1/08 4:46p mward
 * PR49827: Add a Dolby Volume checkbox in Info:Audio.
 *
 * 25   11/26/08 9:52a katrep
 * PR47061: Diabling DTS encode for ac3,and wmapro formats
 *
 * 24   9/17/08 6:20p katrep
 * PR47061: Add support for DTS encode
 *
 * 23   9/10/08 6:33p katrep
 * PR45577: Do not enable pcm5.2 for stereo only audio codecs
 *
 * 22   8/13/08 7:08p katrep
 * PR45577: Add supoort for 5.1 pcm audio on hdmi
 *
 * 21   7/14/08 1:53p erickson
 * PR44718: standardize info panel text
 *
 * 20   6/10/08 12:03p vishk
 * PR 32994: Added dual-mono audio settings
 *
 * 19   6/9/08 6:18p vishk
 * PR 32994: Added dual-mono audio settings
 *
 * 18   3/17/08 3:18p erickson
 * PR40307: remove old, unused code before adding new
 *
 *
 * SanDiego_Mot_DSR/3   3/4/09 4:10p mward
 * PR52051:  Add COMPRESSION settings NONE-A and MODERATE-A.
 *
 * SanDiego_Mot_DSR/2   3/3/09 5:54p mward
 * PR52051:  Change DRC mode control to match Motorola use.
 *
 * SanDiego_Mot_DSR/1   3/3/09 4:02p mward
 * PR52051:  Add controls for dialog normalization and DRC mode.
 *
 * 17   8/13/07 2:25p vishk
 * PR 33542: To add selection of mono downmix for stereo (or higher)
 * streams to SETAPI and brutus.
 *
 * 16   6/8/07 5:51p erickson
 * PR26844: If the status is not compressed, but our current setting is
 * compressed, then clearly the Settop API is forcing it to PCM. If so,
 * remove compressed as an option.
 *
 * 15   6/5/07 2:41p erickson
 * PR31728: allow aac_plus passthrough
 *
 * 14   2/23/07 1:51p erickson
 * PR26844: only display SPDIF/HDMI Compressed for audio formats that
 * support it
 *
 * 13   1/29/07 11:18a ssood
 * PR24918: Channel Map Redesign to make its access Thread-safe
 *
 * 12   9/14/06 9:58a jgarrett
 * PR22634: Removing explicit restarts when switching output formats or
 * PIDs
 *
 * PR22634/1   9/11/06 5:35p jgarrett
 * PR 22643: Removing explicit restarts when switching output formats or
 * PIDs
 *
 * 11   8/8/06 5:49p jgarrett
 * PR 22675: Merging with latest baseline.
 *
 * PR22675/1   7/14/06 5:45p jgarrett
 * PR 22675: Adding multichannel support
 *
 * 10   6/19/06 3:47p erickson
 * PR19526: added ChannelMapEntry::setType to force initialization of
 * Settop API frontend params structures when type changes
 *
 * 9   5/31/06 4:20p tokushig
 * PR21891: Merging updated picture viewer into mainline.
 *
 * SanDiego_Brutus_Skin/7   5/16/06 11:34a tokushig
 * merge from main
 *
 * 8   5/10/06 12:03p jgarrett
 * PR 17747: Adding HDMI menu option
 *
 * 7   4/17/06 5:09p jgarrett
 * PR 20951: Merging skinned UI into mainline
 *
 * SanDiego_Brutus_Skin/6   4/14/06 8:05p tokushig
 * moved info screen location to avoid overscan cutoff
 *
 * SanDiego_Brutus_Skin/5   3/30/06 10:53a tokushig
 * remove prerendering of icon if magnum based.  since we are using the
 * blitter to display pixmaps, any pixmap with alpha values cannot be
 * displayed properly.  legacy platforms will operate as usual
 *
 * SanDiego_Brutus_Skin/4   2/23/06 2:27p tokushig
 * merge from 97400 prerel label
 *
 * 6   2/14/06 4:36p rjlewis
 * PR18903: uninitialize after delete.
 *
 * 5   2/13/06 11:31a erickson
 * PR17108: move bdecode calls into DecodeEngine
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
 * 4   9/26/05 4:24p erickson
 * PR17053: added checkbox to explain irregular behavior
 *
 * 3   9/15/05 6:13p erickson
 * PR17108: need video.h for non-pvr systems
 *
 * 2   8/25/05 3:28p erickson
 * PR16819: toggle spdif_settings.pcm based on UI
 *
 * 1   8/24/05 5:14p erickson
 * PR16819: initial checkin of audio info panel
 *
 ***********************************************************************/
#include "info_audio.h"
#include "control.h"
#include "cfgsettings.h"
#include "marrowbar.h"
#include "mmessagebox.h"
#include "video.h"
#include <bstd.h>
BDBG_MODULE(info_config);

#if BCHP_CHIP != 7400 && BCHP_CHIP != 7401 && BCHP_CHIP != 7403 && BCHP_CHIP != 7118 && BCHP_CHIP != 7408 
    #define AUDIO_ENCODE   1
    #define DTS_AUDIO_ENCODE 1
#endif

#define STR_NONE "None"
#define STR_DOLBY_VOLUME "DolbyVolume"
#define STR_TRU_VOLUME "TruVolume"
#define STR_AVL "BRCM AVL"

typedef enum drc_level
{
    drc_level_none,
    drc_level_moderate,
    drc_level_heavy
} drc_level;

#define PIXMAPPATH (MString(infoBar->control()->cfg()->get(Config::IMAGES_PATH)) + "/info/")

class AudioInfoPage : public InfoBarPopup, public MButtonEventHandler,
    public ControlEventHandler {
public:
    AudioInfoPage(InfoBar *infoBar, MWidget *parent, const MRect &rect);
    void update() {}

    // overrides
    void show();
    void onClick(MButton *button);
    void onControlEvent(Control *control, Control::Event event);
protected:
    MCheckBox *_sapEnabled;
    MCheckBox *_dialogNorm;
    MArrowBar *_dac, *_spdif, *_hdmi, *_downmix, *_chselect, *_drclevel, *_cutboost, *_postproc;

    boutput_spdif_t getSpdif();
    boutput_dvi_t getHdmi();
    baudio_dualmono getDualMono();
    baudio_downmix getDownMix();
    drc_level getDrcLevel();

    void read();
};

boutput_spdif_t AudioInfoPage::getSpdif()
{
    const BrutusDisplay *display = control()->display();
    bdisplay_settings display_settings;
    bdisplay_get(display->display, &display_settings);
    return display_settings.spdif;
}

boutput_dvi_t AudioInfoPage::getHdmi()
{
    const BrutusDisplay *display = control()->display();
    bdisplay_settings display_settings;
    bdisplay_get(display->display, &display_settings);
    return display_settings.dvi;
}

baudio_dualmono AudioInfoPage::getDualMono()
{
    DecodeEngine *decengine = control()->decode();
    DecodeEngine::Settings settings;
    if (!decengine) {
        return (baudio_dualmono)_chselect->current();
    }
    decengine->getSettings(&settings);

    bdecode_t decode = settings.decode;

    if (!decode) {
        return (baudio_dualmono)_chselect->current();
    }
    bdecode_settings dec_settings;

    bdecode_get(decode, &dec_settings);

    return dec_settings.audio.dualmono;
}

baudio_downmix AudioInfoPage::getDownMix()
{
    DecodeEngine *decengine = control()->decode();
    DecodeEngine::Settings settings;
    if (!decengine) {
        return (baudio_downmix)_downmix->current();
    }
    decengine->getSettings(&settings);

    bdecode_t decode = settings.decode;

    if (!decode) {
        return (baudio_downmix)_downmix->current();
    }
    bdecode_settings dec_settings;

    bdecode_get(decode, &dec_settings);

    return dec_settings.audio.downmix;
}

drc_level AudioInfoPage::getDrcLevel()
{
    DecodeEngine *decengine = control()->decode();
    DecodeEngine::Settings settings;
    if (!decengine) {
        return (drc_level)_drclevel->current();
    }
    decengine->getSettings(&settings);

    bdecode_t decode = settings.decode;

    if (!decode) {
        return (drc_level)_drclevel->current();
    }
    bdecode_settings dec_settings;

    bdecode_get(decode, &dec_settings);

    if (dec_settings.audio.codec.dolby.drc_mode == baudio_dolby_drc_mode_line)
    {
        return (dec_settings.audio.codec.dolby.boost == 0) ? drc_level_none : drc_level_moderate;
    }

    return drc_level_heavy;
}

void AudioInfoPage::read()
{
    bstream_status stream_status;
    bdecode_status decode_status;
    ChannelMap::Channel channel;
    DecodeEngine *decengine;
    DecodeEngine::Settings decengine_settings;
    bool spdif_use_pcm = false; /* Is SPDIF currently set to support PCM or Compressed? */
    bool is_analog = !control()->isPlaying() &&
        !control()->channelMap()->getChannel(control()->channel(), &channel) &&
        channel.entry.getType() == btuner_analog_stream;
    bool is_linein = is_analog && !channel.entry.analog.linein;
    bstream_t stream = NULL;
    boutput_spdif_settings spdif_settings;

    /* init structures in case we can't read status */
    BKNI_Memset(&decengine_settings, 0, sizeof(decengine_settings));
    BKNI_Memset(&decode_status, 0, sizeof(decode_status));
    bstream_mpeg_init(&stream_status.mpeg); /* the only member of stream_status that will be used is stream_status.mpeg, so init that. */

    decengine = control()->decode();
    if (decengine) {
        decengine->getSettings(&decengine_settings);
        stream = decengine->getStream();
    }

    bdecode_t decode = decengine_settings.decode;
    boutput_spdif_t spdif = getSpdif();

    /* get mpeg params from stream */
    if (stream && !is_analog) {
        bstream_get_mpeg_parameters(stream, &stream_status);
    }

    /* get video/audio program from decode */
    if (!is_analog && decode) {
        bdecode_get_status(decode, &decode_status);
    }

    /* see if spdif output is set to PCM */
    if (spdif) {
        boutput_spdif_get(spdif, &spdif_settings);
        spdif_use_pcm = spdif_settings.pcm;
    }

    /* reprogram dac and spdif pids */
    _spdif->clear();
    _dac->clear();
    _hdmi->clear();
    _chselect->clear();
    _downmix->clear();
    _drclevel->clear();

    bool has_audio_pid = false;
    for (int i=0;i<BSETTOP_MAX_PROGRAMS;i++) {
        if (stream_status.mpeg.audio[i].pid) {
            char buf[64];
            snprintf(buf, 64, "%#x (%s)", stream_status.mpeg.audio[i].pid, b_audtype_str(stream_status.mpeg.audio[i].format));
            _dac->addItem(buf);
/* SUPPORT_SPDIF_AUDIO_PROGRAM requires a new api: bdecode_set_spdif_audio_program(decode,index);
Until then, we'll support passthrough/pcm boolean. */
#ifdef SUPPORT_SPDIF_AUDIO_PROGRAM
            if (spdif)
                _spdif->addItem(buf);
#endif
            has_audio_pid = true;
        } else {
            /* we assume the audio pids are packed, otherwise have to do an arrowbar index <-> audio_program translation */
            break;
        }
    }

    /************************
    * Set DAC
    ****/
    if (has_audio_pid) {
        _dac->setCurrent(decode_status.audio_program);
    } else if (is_analog) {
        _dac->addItem("PCM");
    } else {
        _dac->addItem("None");
    }

    /************************
    * Set SPDIF
    ****/
    if (has_audio_pid && spdif) {
#ifdef SUPPORT_SPDIF_AUDIO_PROGRAM
        _spdif->addItem("PCM");
        if (spdif_use_pcm)
            _spdif->setCurrent(_spdif->total()-1);
        else
            _spdif->setCurrent(decode_status.audio_program);
#else
        _spdif->addItem("Compressed");
        _spdif->addItem("PCM");
        _spdif->setCurrent(spdif_use_pcm?1:0);

        #if AUDIO_ENCODE
        #if DTS_AUDIO_ENCODE
        _spdif->addItem("DTS");
        #endif
        _spdif->addItem("AC3");
        #if DTS_AUDIO_ENCODE
        if( !spdif_use_pcm && spdif_settings.compressed_audio_format == baudio_format_dts)
            _spdif->setCurrent(_spdif->total()-2);
        #endif
        if ( !spdif_use_pcm && spdif_settings.compressed_audio_format == baudio_format_ac3)
            _spdif->setCurrent(_spdif->total()-1);
        #endif
#endif
    } else if (is_analog) {
        _spdif->addItem("PCM");
    } else {
        _spdif->addItem("None");
    }

    /************************
    * Set HDMI
    ****/
    boutput_dvi_t hdmi = getHdmi();
    boutput_hdmi_capabilities hdmi_caps;
    boutput_hdmi_status hdmi_status;

    if (has_audio_pid && hdmi) {

        boutput_hdmi_get_capabilities(hdmi, &hdmi_caps);

        if (hdmi_caps.audio_mode_is_supported[boutput_hdmi_audio_mode_pcm]) {
            _hdmi->addItem("PCM");
        }
        if (hdmi_caps.audio_mode_is_supported[boutput_hdmi_audio_mode_compressed]) {
            _hdmi->addItem("Compressed");
        }
        if (hdmi_caps.audio_mode_is_supported[boutput_hdmi_audio_mode_pcm_6ch]) {
            _hdmi->addItem("PCM 5.1");
        }

#if AUDIO_ENCODE
        #if DTS_AUDIO_ENCODE
        if (hdmi_caps.audio_mode_is_supported[boutput_hdmi_audio_mode_dts]) {
            _hdmi->addItem("DTS");
        }
        #endif
        if (hdmi_caps.audio_mode_is_supported[boutput_hdmi_audio_mode_compressed]) {
            _hdmi->addItem("AC3");
        }
#endif
    } else {
        _hdmi->addItem("None");
    }
    if ( hdmi ) {
        boutput_dvi_settings hdmi_settings;
        boutput_dvi_get(hdmi, &hdmi_settings);
        _hdmi->setEnabled(true);
        _hdmi->setCurrent((int)hdmi_settings.hdmi_audio_mode);
#if AUDIO_ENCODE
        if(hdmi_settings.hdmi_audio_mode != boutput_hdmi_audio_mode_pcm &&
            hdmi_settings.hdmi_audio_mode != boutput_hdmi_audio_mode_pcm_6ch) 
        {
            boutput_hdmi_capabilities hdmi_caps;    
            boutput_hdmi_get_capabilities(hdmi, &hdmi_caps);
            if (hdmi_settings.compressed_audio_format == baudio_format_dts)
            {
                if ( hdmi_caps.audio_mode_is_supported[boutput_hdmi_audio_mode_dts] )
                {
                    _hdmi->setCurrent((int)(_hdmi->total()-(hdmi_caps.audio_mode_is_supported[boutput_hdmi_audio_mode_compressed])?2:1));
                }
            }
            else if ( hdmi_settings.compressed_audio_format == baudio_format_ac3 &&
                      hdmi_caps.audio_mode_is_supported[boutput_hdmi_audio_mode_compressed] ) 
            {
                _hdmi->setCurrent((int)(_hdmi->total()-1));
            }
        }
#endif
    } else
        _hdmi->setEnabled(false);

    /* show and update SAP for analog RF channels */
    if (is_analog && !is_linein) {
        ChannelManager::Settings settings;
        control()->channelManager()->getSettings(&settings);
        _sapEnabled->setChecked(settings.sapEnabled);
        _sapEnabled->show();
    } else {
        _sapEnabled->hide();
    }

    /* show and update post processing */
    if ( _postproc )
    {
        DecodeEngine *decengine = control()->decode();
        DecodeEngine::Settings settings;
        const char *pText = STR_NONE;
        int i;

        if (!decengine) return;
        decengine->getSettings(&settings);

        bdecode_t decode = settings.decode;
        bdecode_settings dec_settings;
        baudio_decode_t audio = NULL;

        bdecode_get(decode, &dec_settings);
        if ( dec_settings.audio.postproc.dolby.volume_proc_enable )
        {
            pText = STR_DOLBY_VOLUME;
        }
        else if ( dec_settings.audio.postproc.srs.tru_volume_enable )
        {
            pText = STR_TRU_VOLUME;
        }
        else if ( dec_settings.audio.postproc.avl.proc_enable )
        {
            pText = STR_AVL;
        }

        _postproc->setCurrent(0);
        for ( i = 0; i < _postproc->total(); i++ )
        {
            if ( !strcmp(pText, _postproc->getItem(i)) )
            {
                _postproc->setCurrent(i);
                break;
            }
        }
        _postproc->show();
    }

    /* show and update Dialog Normalization */
    if ( _dialogNorm )
    {
        DecodeEngine *decengine = control()->decode();
        DecodeEngine::Settings settings;

        if (!decengine) return;
        decengine->getSettings(&settings);

        bdecode_t decode = settings.decode;
        bdecode_settings dec_settings;
        baudio_decode_t audio = NULL;

        bdecode_get(decode, &dec_settings);
        _dialogNorm->setChecked(dec_settings.audio.codec.dolby.dialog_norm);
        _dialogNorm->show();
    }

  /************************
    * Set Dual Mono Select
    ****/
    baudio_dualmono dual_mono;

    dual_mono = getDualMono();

    _chselect->addItem("Left");
    _chselect->addItem("Right");
    _chselect->addItem("Stereo");
    _chselect->addItem("Monomix");


    _chselect->setCurrent(dual_mono);

  /************************
    * Set Downmix Mode
    ****/
    baudio_downmix downmix;

    downmix = getDownMix();

   switch(stream_status.mpeg.audio[0].format)
   {
   case baudio_format_aac:
   case baudio_format_aac_plus:
   case baudio_format_aac_plus_adts:
       _downmix->addItem("Matrix");
       _downmix->addItem("Arib");
       _downmix->addItem("LeftRight");\
       if(downmix > 2)
           downmix = baudio_downmix_stereo;
       break;
   default:
    _downmix->addItem("Stereo");
    _downmix->addItem("Multichannel");
    _downmix->addItem("None");
    _downmix->addItem("Left");
    _downmix->addItem("Right");
    _downmix->addItem("Mono");
    _downmix->setCurrent(downmix);
    break;
   }

    /************************
    * Set DRC Mode
    ****/
    drc_level drclevel;

    drclevel = getDrcLevel();

    _drclevel->addItem("NONE");
    _drclevel->addItem("MODERATE");
    _drclevel->addItem("HEAVY");

    _drclevel->setCurrent(drclevel);

}

void AudioInfoPage::onControlEvent(Control *control, Control::Event event)
{
    switch (event) {
    case Control::evMoveWindowControl:
    case Control::evStopDecode:
    case Control::evStartDecode:
    case Control::evSetOutput:
        read();
        break;
    default:
        break;
    }
}

AudioInfoPage::AudioInfoPage(InfoBar *infoBar, MWidget *parent, const MRect &rect) :
    InfoBarPopup(infoBar, parent, rect)
{
    setBackgroundColor(style()->color(MStyle::BUTTON_FACE));
    setBorder(3);
    setBevel(3);

    control()->addControlEventHandler(this);

    int y = 10;
    MLabel * l1 = NULL;
    MLabel * l2 = NULL;
    MLabel * l3 = NULL;
    MLabel * l4 = NULL;
    MLabel * l5 = NULL;
    MLabel * l6 = NULL;
    MLabel * l7 = NULL;

    /* The original idea was that this "DAC" button was the PID for DAC.
    In reality, it's the PID for DAC/SPDIF/HDMI. When we add multi-pid audio support in brutus,
    we can change this back. */
    l1 = new MLabel(this, MRect(10,y,40,30), "PID");
    _dac = new MArrowBar(this, MRect(200,y,200,30));
    _dac->addHandler(this);
    y += 30;
    l2 = new MLabel(this, MRect(10,y,60,30), "SPDIF");
    _spdif = new MArrowBar(this, MRect(200,y,200,30));
    _spdif->addHandler(this);
    y += 30;
    l3 = new MLabel(this, MRect(10,y,60,30), "HDMI");
    _hdmi = new MArrowBar(this, MRect(200,y,200,30));
    _hdmi->addHandler(this);
    y += 30;
    l4= new MLabel(this, MRect(10,y,90,30), "DOWNMIX");
    _downmix= new MArrowBar(this, MRect(200,y,200,30));
    _downmix->addHandler(this);
    y += 30;
    l5= new MLabel(this, MRect(10,y,280,30), "DUAL MONO");
    _chselect= new MArrowBar(this, MRect(200,y,200,30));
    _chselect->addHandler(this);
    y += 30;
    _sapEnabled = new MCheckBox(this, MRect(10,y,200,30), "BTSC SAP");
    _sapEnabled->addHandler(this);

    l6= new MLabel(this, MRect(10,y,140,30), "COMPRESSION");
    _drclevel= new MArrowBar(this, MRect(200,y,200,30));
    _drclevel->addHandler(this);

    y += 30;
    l7= new MLabel(this, MRect(10,y,140,30), "PROCESSING");
    _postproc = new MArrowBar(this, MRect(200,y,200,30));
    _postproc->addHandler(this);
    _postproc->addItem(STR_NONE);
    if ( bconfig->features.dolby_volume )
    {
        _postproc->addItem(STR_DOLBY_VOLUME);
    }
    if ( bconfig->features.srs_volume )
    {
        _postproc->addItem(STR_TRU_VOLUME);
    }
    if ( bconfig->features.auto_volume_level )
    {
        _postproc->addItem(STR_AVL);
    }

    y += 30;
    _dialogNorm = new MCheckBox(this, MRect(10,y,260,30), "DIALOG NORMALIZATION");
    _dialogNorm->addHandler(this);
    
    Skin * pSkin = infoBar->control()->getSkin();
    if (pSkin->isValid(XML_SCREEN_TV)) {
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
        if (!screenColor.isEmpty()) {
            int32_t nCol = pSkin->getColorValue(screenColor.s());
            setBackgroundColor(nCol);
            _dac->setBackgroundColor(nCol);
            _spdif->setBackgroundColor(nCol);
            _hdmi->setBackgroundColor(nCol);
            _downmix->setBackgroundColor(nCol);
            _chselect->setBackgroundColor(nCol);
            _postproc->setBackgroundColor(nCol);
            _drclevel->setBackgroundColor(nCol);
            _dialogNorm->setBackgroundColor(nCol);
        }

        //set focus image if necessary
        if (!imgFocus.isEmpty()) {
            //setImageFocus(pSkin->getImage(imgFocus.s()));
        }

        //set text color if necessary
        if (!textColor.isEmpty()) {
            int32_t nCol = pSkin->getColorValue(textColor.s());
            setTextColor(nCol);
            l1->setTextColor(nCol);
            l2->setTextColor(nCol);
            l3->setTextColor(nCol);
            l4->setTextColor(nCol);
            l5->setTextColor(nCol);
            l6->setTextColor(nCol);
            l7->setTextColor(nCol);
            _dac->setTextColor(nCol);
            _spdif->setTextColor(nCol);
            _hdmi->setTextColor(nCol);
            _downmix->setTextColor(nCol);
            _chselect->setTextColor(nCol);
            _postproc->setTextColor(nCol); 
            _drclevel->setTextColor(nCol);
            _dialogNorm->setTextColor(nCol);
        }

        //set focus text color if necessary
        if (!textColorFocus.isEmpty())
        {
            int32_t nCol = pSkin->getColorValue(textColorFocus.s());
            setTextColorFocus(nCol);
            _dac->setTextColorFocus(nCol);
            _spdif->setTextColorFocus(nCol);
            _hdmi->setTextColorFocus(nCol);
            _downmix->setTextColorFocus(nCol);
            _chselect->setTextColorFocus(nCol);
            _postproc->setTextColorFocus(nCol);
            _drclevel->setTextColorFocus(nCol);
            _dialogNorm->setTextColorFocus(nCol);
        }

        MImage * imgButtonOn   = NULL;
        MImage * imgButtonOff  = NULL;
        MImage * imgButtonDown = NULL;
        //set button focus image if necessary
        if (!buttonOn.isEmpty()) {
            imgButtonOn = pSkin->getImage(buttonOn.s());
        }

        //set button unfocus image if necessary
        if (!buttonOff.isEmpty()) {
            imgButtonOff = pSkin->getImage(buttonOff.s());
        }

        //set button pressed image if necessary
        if (!buttonDown.isEmpty()) {
            imgButtonDown = pSkin->getImage(buttonDown.s());
        }

        if (imgButtonOn || imgButtonOff || imgButtonDown) {
            _dac->setImages(imgButtonOff, imgButtonDown, imgButtonOn);
            _spdif->setImages(imgButtonOff, imgButtonDown, imgButtonOn);
            _hdmi->setImages(imgButtonOff, imgButtonDown, imgButtonOn);
            _downmix->setImages(imgButtonOff, imgButtonDown, imgButtonOn);
            _chselect->setImages(imgButtonOff, imgButtonDown, imgButtonOn);
            _postproc->setImages(imgButtonOff, imgButtonDown, imgButtonOn);
            _drclevel->setImages(imgButtonOff, imgButtonDown, imgButtonOn);
        }

        //set bevel if necessary
        if ((bevelWidth > 0) && (!bevelStyle.isEmpty())) {
            MPainter::BevelStyle bevelType = MPainter::bsRaised;
            if (bevelStyle == XML_BEVEL_SUNKEN) {
                bevelType = MPainter::bsSunken;
            }
            if (bevelStyle == XML_BEVEL_UPDOWN) {
                bevelType = MPainter::bsUpDown;
            }

            setBevel(bevelWidth, bevelType);
        }
    }
}

void AudioInfoPage::onClick(MButton *button)
{
    if (button == _dac) {
        control()->decode()->setAudioProgram(_dac->current());
    } else if (button == _spdif) {
        /* TODO: SUPPORT_SPDIF_AUDIO_PROGRAM */
        boutput_spdif_t spdif = getSpdif();
        if (spdif) {
            boutput_spdif_settings spdif_settings;
            boutput_spdif_get(spdif, &spdif_settings);
            spdif_settings.pcm = _spdif->current() == 1;

#if AUDIO_ENCODE
            const char *pText = _spdif->getItem();
            spdif_settings.compressed_audio_format = baudio_format_unknown;
            if ( !strcmp(pText, "PCM") )
            {
                BDBG_WRN(("requesting PCM audio on SPDIF"));
                spdif_settings.pcm =true;
            }
            else if ( !strcmp(pText, "Compressed") )
            {
                BDBG_WRN(("requesting compressed audio on SPDIF"));

                spdif_settings.pcm =false;
            }
            else if ( !strcmp(pText, "DTS") )
            {
                spdif_settings.pcm =false;
                spdif_settings.compressed_audio_format = baudio_format_dts;
                BDBG_ERR(("requesting DTS encoded audio on SPDIF"));
            }
            else if ( !strcmp(pText, "AC3") )
            {
                spdif_settings.pcm =false;
                spdif_settings.compressed_audio_format = baudio_format_ac3;
                BDBG_ERR(("requesting AC3 encoded audio on SPDIF"));
            }
            else
            {
                BDBG_WRN(("Unrecognized SPDIF output format.  Using PCM."));
                spdif_settings.pcm =true;
            }
#endif

            boutput_spdif_set(spdif, &spdif_settings);

            /* TODO: can we make spdif change immediate without display get/set? */
            /* get/set required to make change */
            const BrutusDisplay *display = control()->display();
            bdisplay_settings display_settings;
            bdisplay_get(display->display, &display_settings);
            bdisplay_set(display->display, &display_settings);
        }
    } else if (button == _hdmi) {
        boutput_dvi_t hdmi = getHdmi();
        if (hdmi) {
            boutput_dvi_settings hdmi_settings;
            boutput_dvi_get(hdmi, &hdmi_settings);
            hdmi_settings.hdmi_audio_mode = (boutput_hdmi_audio_mode)_hdmi->current();

#if AUDIO_ENCODE
            hdmi_settings.compressed_audio_format = baudio_format_unknown;

            const char *pText = _hdmi->getItem();
            if ( !strcmp(pText, "PCM") )
            {
                BDBG_WRN(("requesting PCM audio on HDMI"));
                hdmi_settings.hdmi_audio_mode = boutput_hdmi_audio_mode_pcm;
            }
            else if ( !strcmp(pText, "Compressed") )
            {
                BDBG_WRN(("requesting compressed audio on HDMI"));
                hdmi_settings.hdmi_audio_mode = boutput_hdmi_audio_mode_compressed;
            }
            else if ( !strcmp(pText, "PCM 5.1") )
            {
                BDBG_WRN(("requesting PCM 5.1 audio on HDMI"));
                hdmi_settings.hdmi_audio_mode = boutput_hdmi_audio_mode_pcm_6ch;
            }
            else if ( !strcmp(pText, "DTS") )
            {
                hdmi_settings.hdmi_audio_mode=boutput_hdmi_audio_mode_compressed;
                hdmi_settings.compressed_audio_format = baudio_format_dts;
                BDBG_ERR(("requesting DTS encoded audio on HDMI"));
            }
            else if ( !strcmp(pText, "AC3") )
            {
                hdmi_settings.hdmi_audio_mode=boutput_hdmi_audio_mode_compressed;
                hdmi_settings.compressed_audio_format = baudio_format_ac3;
                BDBG_ERR(("requesting AC3 encoded audio on HDMI"));
            }
            else
            {
                BDBG_WRN(("Unrecognized HDMI output format.  Using PCM."));
                hdmi_settings.hdmi_audio_mode = boutput_hdmi_audio_mode_pcm;
            }
#endif

            boutput_dvi_set(hdmi, &hdmi_settings);

            /* TODO: can we make hdmi change immediate without display get/set? */
            /* get/set required to make change */
            const BrutusDisplay *display = control()->display();
            bdisplay_settings display_settings;
            bdisplay_get(display->display, &display_settings);
            bdisplay_set(display->display, &display_settings);
#ifndef AUDIO_ENCODE
            /* following code doesn't account for encode settings,presence(or absence make it diffcult
            to correctly predict the index */
            boutput_dvi_get(hdmi, &hdmi_settings);
            _hdmi->setCurrent((int)hdmi_settings.hdmi_audio_mode);
#endif
        }
    } else if (button == _downmix) {
        baudio_downmix downmix = getDownMix();
        bdecode_status decode_status;

        DecodeEngine *decengine = control()->decode();
        DecodeEngine::Settings settings;

        if (!decengine) return;
        decengine->getSettings(&settings);

        bdecode_t decode = settings.decode;
        bdecode_settings dec_settings;
        baudio_decode_t audio = NULL;

        bdecode_get_status(decode,&decode_status);

        audio = bdecode_detach_audio(decode);
        bdecode_get(decode, &dec_settings);
        BDBG_MSG(("codec %d currrent selection %d",decode_status.audio_format,(baudio_aac_downmix)_downmix->current()));
        switch(decode_status.audio_format)
        {
        case baudio_format_aac:
        case baudio_format_aac_plus:
        case baudio_format_aac_plus_adts:
            dec_settings.audio.codec.aac.downmix = (baudio_aac_downmix)_downmix->current();
            break;
        default:
            dec_settings.audio.downmix = (baudio_downmix)_downmix->current();
            break;
        }
        bdecode_set(decode, &dec_settings);
        bdecode_attach_audio(decode, audio);
    } else if (button == _chselect) {
        baudio_dualmono downmix = getDualMono();

        DecodeEngine *decengine = control()->decode();
        DecodeEngine::Settings settings;

        if (!decengine) return;
        decengine->getSettings(&settings);

        bdecode_t decode = settings.decode;
        bdecode_settings dec_settings;
        baudio_decode_t audio = NULL;

        bdecode_get(decode, &dec_settings);
        dec_settings.audio.dualmono = (baudio_dualmono)_chselect->current();
        bdecode_set(decode, &dec_settings);
    } else if (button == _sapEnabled) {
        ChannelManager::Settings settings;
        control()->channelManager()->getSettings(&settings);
        settings.sapEnabled = _sapEnabled->checked();
        control()->channelManager()->setSettings(&settings);

        /* we have to retune to apply this change */
        /* TODO: do we need to extend the Settop API so that this is not required? */
        MMessageBox::info(fb(), "Restarting decode to implement SAP change.");
        control()->stopLiveDecode();
        control()->setChannel(control()->channel());
    } else if (button == _postproc) {
        const char *pText;
        DecodeEngine *decengine = control()->decode();
        DecodeEngine::Settings settings;

        if (!decengine) return;
        decengine->getSettings(&settings);

        bdecode_t decode = settings.decode;
        bdecode_settings dec_settings;
        baudio_decode_t audio = NULL;

        pText = _postproc->getItem();
        BDBG_ASSERT(NULL != pText);

        bdecode_get(decode, &dec_settings);               
        dec_settings.audio.postproc.dolby.volume_proc_enable = false;
        dec_settings.audio.postproc.avl.proc_enable = false;
        dec_settings.audio.postproc.srs.tru_volume_enable = false;
        if ( !strcmp(pText, STR_DOLBY_VOLUME) ) {
            dec_settings.audio.postproc.dolby.volume_proc_enable = true;
        } else if ( !strcmp(pText, STR_AVL) ) {
            dec_settings.audio.postproc.avl.proc_enable = true;
        } else if ( !strcmp(pText, STR_TRU_VOLUME) ) {
            dec_settings.audio.postproc.srs.tru_volume_enable = true;
        }
        bdecode_set(decode, &dec_settings);
    } else if (button == _drclevel) {
        drc_level drclevel = getDrcLevel();

        DecodeEngine *decengine = control()->decode();
        DecodeEngine::Settings settings;

        if (!decengine) return;
        decengine->getSettings(&settings);

        bdecode_t decode = settings.decode;
        bdecode_settings dec_settings;
        baudio_decode_t audio = NULL;

        bdecode_get(decode, &dec_settings);

        switch ((drc_level)_drclevel->current())
        {
            case drc_level_heavy:
                dec_settings.audio.codec.dolby.drc_mode = baudio_dolby_drc_mode_rf;
            break;
            case drc_level_moderate:
                dec_settings.audio.codec.dolby.drc_mode = baudio_dolby_drc_mode_line;
                dec_settings.audio.codec.dolby.boost = 100;
                dec_settings.audio.codec.dolby.cut = 100;
            break;
            case drc_level_none:
                dec_settings.audio.codec.dolby.drc_mode = baudio_dolby_drc_mode_line;
                dec_settings.audio.codec.dolby.boost = 0;
                dec_settings.audio.codec.dolby.cut = 0;
            break;
            default:
                BDBG_ERR(("%s() invalid DRC level %d",__FUNCTION__,(drc_level)_drclevel->current()));
            break;
        }
        bdecode_set(decode, &dec_settings);
    }
    else if (button == _dialogNorm) {
        DecodeEngine *decengine = control()->decode();
        DecodeEngine::Settings settings;

        if (!decengine) return;
        decengine->getSettings(&settings);

        bdecode_t decode = settings.decode;
        bdecode_settings dec_settings;
        baudio_decode_t audio = NULL;

        bdecode_get(decode, &dec_settings);
        dec_settings.audio.codec.dolby.dialog_norm = _dialogNorm->checked();
        bdecode_set(decode, &dec_settings);
    }
}

void AudioInfoPage::show()
{
    read();
    InfoBarPopup::show();
}

/////////////////////////////////////

AudioInfoBarPlugin::AudioInfoBarPlugin(InfoBar *infoBar) :
    InfoBarPlugin(infoBar, "audio_plugin")
{
    unsigned height=260;

    setText("Audio");
    _linkedWidget = new AudioInfoPage(infoBar, topLevelWidget(),
                                      MRect(230,150,410,height));
    _linkedWidget->hide();
    _icon = new MImage(app(), PIXMAPPATH + "backend.png");
}

AudioInfoBarPlugin::~AudioInfoBarPlugin()
{
    delete _icon;
}
