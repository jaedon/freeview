/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_decode.h $
 * $brcm_Revision: 50 $
 * $brcm_Date: 4/25/12 11:14a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/include/bsettop_decode.h $
 * 
 * 50   4/25/12 11:14a sgadara
 * SW7358-272: [7358] Support AVL on Brutus
 * 
 * 49   2/16/10 5:09p vsilyaev
 * SW3556-1051: Added bdecode_timestamp_order
 * 
 * 48   1/27/10 10:01a jgarrett
 * SW7550-213: Fixing name of stereo dual mono mode
 * 
 * 47   12/1/09 6:08p vsilyaev
 * SW7405-3515: Added left/right AAC downmix
 * 
 * 46   11/19/09 8:02p jgarrett
 * SW7405-3357: Adding audio output routing for decode and pcm playback
 * prior to start
 * 
 * 45   9/29/09 11:48a jgarrett
 * SW7405-3100: Adding display underflows and display drops to video
 * decode status
 * 
 * 44   6/30/09 10:47a jgarrett
 * PR 52051: Add a dialog normalization enable / disable to
 * dolby_settings.
 * 
 * 43   5/15/09 3:21p jgarrett
 * PR52687: Add method to set AAC downmix type
 * 
 * PR52687/2   5/14/09 2:14p jjordan
 * PR52687: improve AAC downmix API
 * 
 * PR52687/1   5/12/09 3:12p jjordan
 * PR52687: Add method to set audio downmix type
 * 
 * 42   5/6/09 4:11p jgarrett
 * PR 50291: Adding wide GA mode
 * 
 * PR50291/1   5/6/09 3:17p jjordan
 * PR50291: add Wide GA mode (config from brutus.cfg)
 * 
 * 41   3/30/09 1:17p vsilyaev
 * PR 52736: Added preroll_rate to the video decoder settings
 * 
 * SanDiego_Mot_DSR/PR52051/3   6/23/09 6:44p mward
 * PR 52051: Update.
 * 
 * 43   5/15/09 3:21p jgarrett
 * PR52687: Add method to set AAC downmix type
 * 
 * PR52687/2   5/14/09 2:14p jjordan
 * PR52687: improve AAC downmix API
 * 
 * PR52687/1   5/12/09 3:12p jjordan
 * PR52687: Add method to set audio downmix type
 * 
 * 42   5/6/09 4:11p jgarrett
 * PR 50291: Adding wide GA mode
 * 
 * PR50291/1   5/6/09 3:17p jjordan
 * PR50291: add Wide GA mode (config from brutus.cfg)
 * 
 * 41   3/30/09 1:17p vsilyaev
 * PR 52736: Added preroll_rate to the video decoder settings
 * 
 * SanDiego_Mot_DSR/PR52051/2   3/27/09 3:26p mward
 * PR52051:  Branch for testing dialog normalization switch with new RAP
 * release.
 * 
 * 40   3/13/09 3:13p jgarrett
 * PR 49827: Merge to main branch
 * 
 * PR49827/4   2/25/09 12:17p mward
 * PR52490: Change name of SRS VIQ to SRS TrueVolume
 * 
 * PR49827/3   1/13/09 5:11p mward
 * PR50613: Add support for SRS Volume IQ.
 * 
 * PR49827/2   12/5/08 2:40p mward
 * PR49827: Separate Dolby post-processing and codec settings.
 * 
 * PR49827/1   12/1/08 12:14p mward
 * PR49827: Support for Dolby volume post-processing.
 * 
 * 39   6/9/08 5:44p vsilyaev
 * PR 32994: Added dual-mono audio settings
 * 
 * 38   3/31/08 6:26p jgarrett
 * PR 40987: Updating documentation on set audio program
 * 
 * 37   12/20/07 5:51p vsilyaev
 * PR38004: PR38004: Add video decoder freeze/resume API
 * 
 * PR38004/2   12/20/07 5:21p jtna
 * PR38004: Add video decoder freeze/resume API
 * 
 * PR38004/1   12/18/07 5:53p jtna
 * PR38004: Add video decoder freeze/resume API
 * 
 * 36   10/25/07 9:20a jgarrett
 * PR 28004: Duplicate structure member in bvideo_decode_status for Pan-
 * Scan status.
 * 
 * PR28004/2   9/20/07 4:11p mward
 * PR 28004: Duplicate structure member in bvideo_decode_status for Pan-
 * Scan status.
 * 
 * 35   9/10/07 5:59p vsilyaev
 * PR33851:  Adding audio master mode
 * 
 * 33   8/22/07 5:29p jgarrett
 * PR 32044: Merging to main branch
 * 
 * 7405_pip_support/1   8/21/07 2:38p katrep
 * PR34204: Added mosaic mode support for 7405.
 * 
 * 32   7/23/07 6:09p vsilyaev
 * PR 33340: Added audio/video pts_error_cnt
 * 
 * 31   6/27/07 5:42p vsilyaev
 * PR 32282: Added number of queued compressed auduo frames
 * 
 * 30   6/26/07 6:08p vsilyaev
 * PR 24848: Added configuration for boost/cut audio postprocessing
 * 
 * PR32044/1   6/26/07 5:54p jgarrett
 * PR 32044: Adding Mosaic Mode APIs
 * 
 * 29   6/7/07 1:54p jgarrett
 * PR 31914: Adding audio/video source changed callbacks.
 * 
 * 28   4/13/07 3:51p vsilyaev
 * PR 29840: Added API to render graphics surface as the decoder output
 * 
 * 27   1/26/07 6:18p vsilyaev
 * PR 26400: Use dedicated functions and structured for video and audio
 * status
 * 
 * 26   1/19/07 7:20p vsilyaev
 * PR 27245: Moved audio/video status into related file
 * 
 * 25   12/8/06 1:56p erickson
 * PR26128: added bdecode_settings.audio.secondary_audio_program. This is
 * temporary.
 * 
 * 24   11/22/06 10:20a erickson
 * PR25519: added bdecode_status.video_picture_queue
 * 
 * 23   11/15/06 7:36p bandrews
 * PR25456: Added passthrough audio status, and cleaned up status design
 * (maintaining bw compat)
 * 
 * 22   10/18/06 6:33p bandrews
 * PR24848: Updated with fixed nested param structure in settings and
 * retrieval of rap handle does not require decode to be started
 * 
 * 21   10/17/06 5:53p bandrews
 * PR24848: Added Dolby config params
 * 
 * 20   10/2/06 4:27p vsilyaev
 * PR 23826: Added functions to allow user control STC
 * 
 * 19   6/29/06 7:00p erickson
 * PR21721: added bdecode_stop_mode to bdecode_settings
 * 
 * 18   6/23/06 9:51a erickson
 * PR21510: add baudio_downmix to bdecode_settings
 * 
 * 17   6/12/06 11:18a erickson
 * PR21646: added bvideo_frame_rate
 * 
 * 16   3/31/06 3:58p jgarrett
 * PR 20236: Adding graphics context to bdecode_acquire_capture_buffer
 * 
 * 15   3/14/06 6:53p dlwin
 * PR 20206: Removed B_HAS_IP use.  Now it always incude
 * 
 * 14   3/10/06 10:48a erickson
 * PR17108: fix prototypes for proxy build
 * 
 * 13   3/2/06 12:26p dlwin
 * PR15200: Merge to main-line, rework STC steering for 97398
 * 
 * PROD_IPSTB_REFSW_Devel/1   3/2/06 11:27a jjordan
 * PR15200 - rework STC steering for 97398
 * 
 * 12   2/1/06 4:07p vle
 * PR 18538: add feature to indicate PTS type when call
 * bdecode_get_status()
 * 
 * 11   1/6/06 2:52p erickson
 * PR17108: added bool *top_field param to bdecode_acquire_capture_buffer
 * 
 * 10   12/15/05 9:55a erickson
 * PR18616: added still picture code for 7401
 * 
 * 9   12/13/05 4:52p erickson
 * PR18439: added bdecode_acquire_capture_buffer et al to return capture
 * buffer to application
 * 
 * 8   11/14/05 2:59p vsilyaev
 * PR 17727: Fixed typo
 * 
 * 7   10/17/05 2:11p erickson
 * PR17650: added bdecode_read_metadata
 * 
 * 6   10/12/05 4:05p erickson
 * PR15251: added interlaced and video_codec to bdecode_status
 * 
 * 5   8/24/05 5:15p erickson
 * PR16819: added audio_program and video_program to decode status
 * 
 * 4   5/12/05 7:08p dlwin
 * PR 15253: Remove conditional compile for NOOS
 * 
 * 3   5/9/05 8:44p agin
 * PR15253: video format auto detection for non-os.
 * 
 * 2   2/12/05 11:06a dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/25   2/8/05 9:45a jjordan
 * PR13959: Initial checkin of IP STB
 * 
 * Irvine_BSEAVSW_Devel/24   1/28/05 6:03p mphillip
 * PR13967: Changed const char * to char[16], changed the matching
 * assignments
 * 
 * Irvine_BSEAVSW_Devel/23   12/14/04 5:17p marcusk
 * PR13528: Added additional audio status, stream type, number of
 * channels, downmix mode, and sample rate.
 * 
 * Irvine_BSEAVSW_Devel/22   9/21/04 3:53p erickson
 * PR12728: fixed -pedantic warnings
 * 
 * Irvine_BSEAVSW_Devel/21   8/4/04 4:03p vsilyaev
 * PR 11472 : Added timecode filed into the status structure
 * 
 * Irvine_BSEAVSW_Devel/20   6/25/04 9:15a erickson
 * PR11135: updated comments
 * 
 * Irvine_BSEAVSW_Devel/19   5/27/04 4:19p dlwin
 * PR 10864:  Added code to return the MPEG decoder Pan/Scan status.
 * This status is based on decoding Pan/Scan vectors and using it.
 * 
 * Irvine_BSEAVSW_Devel/18   5/7/04 12:19p vsilyaev
 * PR 10864:  Added field video_aspect_ratio.
 * 
 * Irvine_BSEAVSW_Devel/17   11/7/03 4:28p erickson
 * PR8583: removing, fixing or implementing TODO's
 * 
 * Irvine_BSEAVSW_Devel/16   11/4/03 2:42p erickson
 * documentation changes
 * 
 * Irvine_BSEAVSW_Devel/15   10/31/03 11:58a erickson
 * settop api changes after internal review
 * 
 * Irvine_BSEAVSW_Devel/14   10/30/03 9:47a erickson
 * Copied dbg interface so that we can compile out BRCM_DBG_MSG
 * 
 * Irvine_BSEAVSW_Devel/13   10/28/03 11:26a erickson
 * settop api reworks after internal design view
 * 
 * Irvine_BSEAVSW_Devel/12   10/22/03 11:22a erickson
 * settop api pass-through type conversion
 * 
 * Irvine_BSEAVSW_Devel/11   9/30/03 1:13a erickson
 * updated docjet documentation
 * 
 * Irvine_BSEAVSW_Devel/10   9/17/03 5:04p erickson
 * updated documentation for DocJet
 * 
 * Irvine_BSEAVSW_Devel/9   8/19/03 2:48p erickson
 * implemented attach/detach audio
 * 
 * Irvine_BSEAVSW_Devel/8   8/18/03 11:01a erickson
 * initial bringup for brutus 7320, still in progress
 * 
 * Irvine_BSEAVSW_Devel/7   8/13/03 11:37a erickson
 * added more examples
 * 
 * Irvine_BSEAVSW_Devel/6   8/12/03 9:42a erickson
 * changes after pre-review meeting
 * 
 * Irvine_BSEAVSW_Devel/5   7/30/03 2:08p erickson
 * removed all return value comments because we're standardized and they
 * add nothing
 * 
 * Irvine_BSEAVSW_Devel/4   7/29/03 6:02p erickson
 * more rework
 * 
 * Irvine_BSEAVSW_Devel/3   7/25/03 4:22p erickson
 * in the middle of rework, does not compile
 * 
 * Irvine_BSEAVSW_Devel/2   7/9/03 5:19p erickson
 * rework for new bband_t model
 *
 * Irvine_BSEAVSW_Devel/1   6/13/03 3:09p vsilyaev
 * SetTop reference API.
 *
 *
 ***************************************************************************/
#ifndef BSETTOP_DECODE_H__
#define BSETTOP_DECODE_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*=*************************
The decode interface is used to decode both digital and analog
streams. It controls both video and audio decode.

The source for a decode is a stream (bstream_t). Streams can generated either from a tuner (btuner_t), 
a playback engine (bplayback_t) or streamer (bstreamer_t).
The destination of a decode is a decode window (bdecode_window_t) which is connected to a
display (bdisplay_t).

For PIP or dual output systems, there are two decoders. Otherwise there is only one
decoder.
****************************/

/*
Summary:
    Open a decode engine.
Description:
*/
bdecode_t bdecode_open(
    bobject_t decode_id /* decode object id */
    );

/*
Summary:
    Settings for a mosaic decoder.
Description:
*/
typedef struct bdecode_mosaic_settings {
        unsigned width;      /* Maximum width of a stream to be decoded by this channel */
        unsigned height;     /* Maximum height of a stream to be decoded by this channel */
        unsigned num_codecs; /* Number of entries in the video_codecs_supported array */
        bvideo_codec video_codecs_supported[BSETTOP_MAX_VIDEO_CODECS]; /* Array of codecs to be supported by this channel. 
                                                                          This will be initialized to match your primary 
                                                                          decoder settings by default.  */
} bdecode_mosaic_settings;

/*
Summary:
    Initialize mosaic settings to defaults.
Description:
*/
void bdecode_mosaic_settings_init(
    bdecode_t parent,                 /* Parent video decoder */
    bdecode_mosaic_settings *settings /* Settings for this mosaic channel */
    );

/*
Summary:
    Open a mosaic decode engine.
Description:
*/
bdecode_t bdecode_open_mosaic(
    bdecode_t parent,                       /* Parent video decoder.  Decoder must be stopped for this call to succeed. */
    bobject_t decode_id,                    /* mosaic object id, this must be unique across all mosaics. */
    const bdecode_mosaic_settings *settings /* Settings for this mosaic channel. */
    );

/*
Summary:
    Close a decode engine.
Description:
    The decode should be stopped before closing.
    After closing, the bdecode_t handle is invalid.
*/
void bdecode_close(
    bdecode_t decode /* handle returned by bdecode_open */
    );

/*
Summary:
Dynamic Range Compression Mode

Description:
This corresponds to the eCompMode field of BRAP_DSPCHN_Ac3*ConfigParams.  The 
original is an enum with four possibilities: Custom A, Custom D, Rf, and Line.  
All four values can cause changes in the output volume level.  The most 
noticeable change is the difference between Line and Rf.  With the compression 
mode set to Rf, input PCM samples are scaled up by 12 dB.  In Line mode, input 
PCM samples are not scaled up at all.  The two most commonly used modes are line
and Rf, in that order.  Thus this enum offers only those two modes.
*/
typedef enum baudio_dolby_drc_mode
{
    baudio_dolby_drc_mode_line,
    baudio_dolby_drc_mode_rf
} baudio_dolby_drc_mode;

/*
Summary:
Stereo Downmix Mode

Description:
This enum represents the formulas available for doing stereo downmixing.  It
only applies when the downmix mode is stereo.  The enum corresponds to the 
eStereoMode field of BRAP_DSPCHN_Ac3*ConfigParams.  It is an enum with three 
possibilities: Automatic, LtRt, and LoRo.  LoRo is the standard stereo downmix 
formula.  LtRt is a Dolby Surround compatible stereo downmix formula.  Automatic 
means that which stereo downmix formula chosen is determined by parameters 
in the stream (as specified by Annex D of the AC-3 specification).
*/
typedef enum baudio_dolby_stereo_downmix_mode
{
    baudio_dolby_stereo_downmix_mode_automatic,
    baudio_dolby_stereo_downmix_mode_dolby_surround_compatible,
    baudio_dolby_stereo_downmix_mode_standard
} baudio_dolby_stereo_downmix_mode;

/*
Summary:
Dolby audio decoder settings

Description:
These settings control the parameters involved in the decode of Dolby-based 
streams.  They only apply when the audio type is Dolby Digital (AC3) or Dolby 
Digital Plus (AC3Plus). The variable "cut" determines the percentage cut of 
louder sounds from the normal value. The variable "boost" determines the 
percentage boost of quiter sounds from the normal value.
*/

typedef struct baudio_dolby_settings
{
    baudio_dolby_drc_mode drc_mode;
    baudio_dolby_stereo_downmix_mode stereo_downmix_mode;
    unsigned short cut;
    unsigned short boost;
    bool lfe;
    bool dialog_norm; /* This should be true for normal operation */
} baudio_dolby_settings;

/*
Summary:
Dolby audio post-processing settings

Description:
These settings control the parameters involved in the post-processing of Dolby-based 
streams.  They only apply when the audio type is Dolby Digital (AC3) or Dolby 
Digital Plus (AC3Plus).
*/

typedef struct baudio_dolby_postproc_settings
{
    bool volume_proc_enable;
} baudio_dolby_postproc_settings;

/*
Summary:
SRS audio post-processing settings

Description:
These settings control the parameters involved in the SRS TruVolume post-processing.  
*/

typedef struct baudio_srs_postproc_settings
{
    bool tru_volume_enable;
} baudio_srs_postproc_settings;

/*
Summary:
AVL audio post-processing settings

Description:
These settings control the parameters involved in the Auto Volume Level post-processing.  
*/

typedef struct baudio_avl_postproc_settings
{ 
    bool proc_enable;
} baudio_avl_postproc_settings;

/*
Summary:
Audio downmix modes

Description:
These modes are all "if applicable." The actual audio downmixing and output depend on the number of
source channels and the capabilities of the hardware for each platform. 

Changes are only guaranteed to be applied when restarting audio decode. This can be done with
either bdecode_stop/start or bdecode_detach/attach_audio. Some changes can be made 
on-the-fly (without restarting audio decode), but exact behavior may vary by platform.
*/
typedef enum baudio_downmix {
    baudio_downmix_stereo, /* If applicable, down mix source to stereo output [default]. */
    baudio_downmix_multichannel, /* Output decoded data as-is. If number of channels exceeds HW capabilities, 
        downmix to match maximum number of output channels. No audio will be lost. */
    baudio_downmix_none, /* Output decoded data as-is. If number of channels exceeds HW capabilities,
        additional audio channels will be lost. */
    baudio_downmix_left, /* Take left channel and duplicate to left/right outputs. */
    baudio_downmix_right, /* Take right channel and duplicate to left/right outputs. */
    baudio_downmix_monomix  /* Mix left and right channels and output result to left/right outputs. This only applies if the stream
        is a dual mono stream. If not, it will be equivalent to baudio_downmix_stereo. */
} baudio_downmix;

/**
Summary:
Control behavior of decoder when stopping decode.
**/
typedef enum bdecode_stop_mode {
    bdecode_stop_mode_mute, /* mute the picture when stopped. decode window will be black. */
    bdecode_stop_mode_last_picture /* display last decoded picture when stopped. */
} bdecode_stop_mode;

/**
Summary:
Controls behavior of decoder when using timestamps from the decoded stream.
**/
typedef enum bdecode_timestamp_order {
    bdecode_timestamp_order_display, /* Timestamps in display order (default) */
    bdecode_timestamp_order_decode   /* Timestamps in decode order */
} bdecode_timestamp_order;

/*
Summary:

Dual Mono channel select modes
*/
typedef enum baudio_dualmono {
    baudio_dualmono_left,   /* Selects left source channel of dual mono audio */
    baudio_dualmono_right,  /* Selects right source channel of dual mono audio */
    baudio_dualmono_stereo, /* Pass-Thru */   
    baudio_dualmono_monomix /* Mixes left and right channels */
} baudio_dualmono;
/*
Summary:
Controls how the tsm in the decoders will work
*/
typedef enum bdecode_tsm_mode 
{
    bdecode_tsm_mode_auto,                  /* system will attempt to pick the best sync mode for the best robustness, error handling, and quality given the current stream */
    bdecode_tsm_mode_stc_master,            /* STC is seeded with either the video or audio PTS in playback mode, depending on which decoder makes an STC request first.  In live mode, the PCR is used to seed the STC. */
    bdecode_tsm_mode_video_master,          /* STC is seeded with the video PTS */
    bdecode_tsm_mode_audio_master,          /* STC is seeded with the audio PTS, audio frames are played out if within sync limit */
    bdecode_tsm_mode_output_master          /* TSM is not performed.  Data is pulled through the decoder by the output clocks. (also called VSYNC mode) */
} bdecode_tsm_mode;


/*
Summary:
Controls the downmix type used by the AAC audio decoder
*/
typedef enum baudio_aac_downmix
{
    baudio_aac_downmix_matrix,  /* BRCM-style downmix */
    baudio_aac_downmix_arib,     /* ARIB-style downmix */
    baudio_aac_downmix_ltrt      /* left right downmix */
} baudio_aac_downmix;

/*
Summary:
AAC audio decoder settings

Description:
These settings control the parameters involved in the decode of AAC-based 
streams.  They only apply when the audio type is AAC or AACPlus. The variable
"downmix" determines the downmix coefficients that are used.
*/
typedef struct baudio_aac_settings
{
    baudio_aac_downmix downmix;
} baudio_aac_settings;

typedef struct bdecode_settings {
    struct {
        baudio_downmix downmix;
        baudio_dualmono dualmono;
        struct {
            baudio_dolby_settings dolby;
            baudio_aac_settings aac;
        } codec;
        int secondary_audio_program; /* If not -1, then perform simultaneous decode of one program and passthrough of
            another program. This requires the following settings which are not automatically coordinated:
            1) bsettop_mpeg.audio[audio_program] will be decoded.
            2) bsettop_mpeg.audio[secondary_audio_program] will be passed through as compressed.
            3) You must set either SPDIF or HDMI to compressed (!pcm) using boutput_spdif_set or boutput_dvi_set. If you 
                do not, it will fail.
            Also, this API is temporary. It is likely to be changed in the near future.
            */
        bsettop_callback source_changed; /* Source change includes sample rate, channel encoding (i.e. stereo, mono, etc.) */
        struct {
            baudio_dolby_postproc_settings dolby;
            baudio_srs_postproc_settings srs;
            baudio_avl_postproc_settings avl;            
        } postproc;
        bdisplay_t display;           /* The display that will receive audio from this bdecode_t.  On some chips, audio input/output
                                         mapping cannot be changed while audio is started, setting this variable prior to start will
                                         associate the audio decoder with the proper display.  Optional, pass NULL to use the display from
                                         bdecode_start()'s window instead. */
    } audio;
    struct {
        bool freeze;    /* Flag to indicate whether video decode is in freeze state. Applicable only after decoder has been started. Auto-reset to false on decoder stop. */
        bsettop_callback source_changed; /* Source change includes size, frame rate, interlaced/progressive */
        unsigned preroll_rate;           /* Slow motion rate (in units of BPLAYPUMP_NORMALPLAY_SPEED) to be applied until decoded picture passes the TSM test */
    } video;
    bdecode_stop_mode stop_mode;         /* Behavior of the decoder when stopped (i.e. mute, hold last picture)*/
    bsettop_callback still_picture_done; /* called when still picture decode is complete */
    void *callback_context;              /* Context passed to source_changed callbacks and still_picture_done */
    struct
    {
        bdecode_tsm_mode mode;          /* the way tsm will be handled */
        unsigned sync_limit;                    /* the limit under which the tsm mode will be applied */
        bool wga_mode_enable;              /* increase the GA threshold to avoid audio dropout due to errored PTS value (up to 1x AAU period error) */
    } tsm;
} bdecode_settings;

/**
Summary:
Get current decode settings
**/
void bdecode_get(
    bdecode_t decode, 
    bdecode_settings *settings /* [out] */
    );

/**
Summary:
Set new decode settings
**/
bresult bdecode_set(
    bdecode_t decode, 
    const bdecode_settings *settings
    );

/*
Summary:
    Start display a surface through video buffer.
*/

bresult bdecode_display_surface(
    bdecode_t decode, 
    bsurface_t surface,
    bdecode_window_t window
    );

/*
Summary:
    Start decoding a stream.
*/
bresult bdecode_start(
    bdecode_t decode, /* handle returned by bdecode_open */
    bstream_t source, /* source for the decode, either analog or digital */
    bdecode_window_t window /* window to render decode */
    );

/*
Summary:
    Stop decoding a stream.
Description:
    The stream remains valid after decode is stopped. Decode could be restarted
    without retuning or restarting playback.
*/
void bdecode_stop(
    bdecode_t decode /* handle returned by bdecode_open */
    );

/*
Summary:
    Stop the audio decode and detach it so that it can be reconfigured or
    assigned to another decode.

Description:
    This is used to switch a single audio decoder between main and PIP decodes.

    It's also used to switch the MPEG program being decoded while not stopping
    video decode. This could be used to support alternative digital audio programs.
*/
baudio_decode_t bdecode_detach_audio(
    bdecode_t decode /* handle returned by bdecode_open */
    );


/*
Summary:
    Attach an audio decode to a decoder.

Description:
    If the decode object has been started, audio decode will start immediately.

    After calling bdecode_attach_audio, the baudio_decode_t can no longer be used.
    Therefore, you can only attach a baudio_decode_t to one decoder object.
*/
bresult bdecode_attach_audio(
    bdecode_t decode, /* handle returned by bdecode_open */
    baudio_decode_t audio /* handle returned by bdecode_detach_audio */
    );

/*
Summary:
   Set the bstream_mpeg program index for audio decode.

Description:
The new program index will be applied immediately.

This should not be used for channel change. This is only used when changing the audio
stream without changing the video stream. If you want to change channel, you need
to stop decode, get a new stream by using a new bstream_mpeg structure, and 
start decode again.
*/
bresult bdecode_set_audio_program(
    bdecode_t decode, /* handle returned by bdecode_open */
    unsigned index /* index into bstream_mpeg.audio[] */
    );

/*
Summary:
    Returns audio decoder connected the decode

Description:
    This is used to obtain audio decoder connected to a decoder. Then 
    audio decoder could be used to  obtain decoder status. During this operation
    state of the deocder wouldn't change or otherwise disturbed.

    It's possible that more than one audio decoder is connected to a single decoder.
    For example dedicated decoder could be used for compressed SPDIF output.
*/
baudio_decode_t bdecode_get_audio(
    bdecode_t decode, /* handle returned by bdecode_open */
    bobject_t audio_id /* audio_decode object id */
    );

/*
Summary:
    Stop the video decode and detach it so that it can be reconfigured or
    assigned to another decode.

Description:
    This is used to switch the MPEG program being decoded while not stopping
    audio decode. This could be used to support multiple viewing angles or
    SD / HD switches.
*/
bvideo_decode_t bdecode_detach_video(
        bdecode_t decode /* handle returned by bdecode_open */
    );

/*
Summary:
    Attach a video decode and start decoding.

Description:
    If the decode object has been started, video decode will start immediately.

    After calling bdecode_attach_video, the bvideo_decode_t can no longer be used.
    Therefore, you can only attach a bvideo_decode_t to one decoder object.      
*/
bresult bdecode_attach_video(
        bdecode_t decode, /* handle returned by bdecode_open */
        bvideo_decode_t video /* handle returned by bdecode_detach_video */
    );

/*
Summary:
   Set the bstream_mpeg program index for video decode.

Description:
The new program index will only get applied on the next bdecode_start or
bdecode_attach_video.

This should not be used for channel change. This is only used when changing the video
stream without changing the audio stream. If you want to change channel, you need
to stop decode, get a new stream by using a new bstream_mpeg structure, and 
start decode again.
*/
bresult bdecode_set_video_program(
        bdecode_t decode, /* handle returned by bdecode_open */
        unsigned index /* index into bstream_mpeg.video[] */
    );

/*
Summary:
    Returns video decoder connected the decode

Description:
    This is used to obtain video decoder connected to a decoder. Then 
    audio decoder could be used to  obtain decoder status. During this operation
    state of the deocder wouldn't change or otherwise disturbed.

    It's possible that more than one video decoder is connected to a single decoder.
*/
bvideo_decode_t bdecode_get_video(
    bdecode_t decode, /* handle returned by bdecode_open */
    bobject_t video_id /* video_decode object id */
    );


/*
Summary:
    Information of the current pan/scan mode of decoder.
Description:
    Describes if decoder is decoding pan/scan vector.  Decoder is consider
    to be decoding pan/scan vectors when decoder is enabled to decode pan/scan
    and MPEG stream contains pan/scan vectors.
*/
typedef enum bdecode_panscan_status {
   bdecode_panscan_status_unknown,      /* decoder panscan status is unknown
                                           (decoder doesn't report panscan status) */
   bdecode_panscan_status_enabled,      /* decoder panscan status is enabled 
                                           (decoder is using stream's panscan information) */
   bdecode_panscan_status_disabled      /* decoder panscan status is disabled */
} bdecode_panscan_status;

/*
Summary:
   Information of the last GOP time code found in the MPEG stream
Description:
   Mpeg stream could carry time code in the GOP stream, this structure would
   contain last time code found in the stream or all members would be 0 if timecode
   wasn't found in the stream
*/
typedef struct bdecode_time_code {
    unsigned short hours; /* ISO 13818-2 time_code_hours field */
    unsigned short minutes;/* ISO 13818-2 time_code_minutes field */
    unsigned short seconds; /* ISO 13818-2 time_code_seconds field */
    unsigned short pictures; /* ISO 13818-2 time_code_pictures field */
} bdecode_time_code;

/*
Summary:
   Information of the pts type
Description:
   Describes the validity of PTS
*/
typedef enum bdecode_pts_type   {
    bdecode_pts_type_invalid=0,
    bdecode_pts_type_interpolated,
    bdecode_pts_type_coded
}   bdecode_pts_type;

/*
Summary:
    Common codec status returned by baudio_decode_get_status and bvideo_decode_get_status
*/
typedef struct bcodec_status {
    unsigned fifo_depth; /* depth in bytes of the compressed buffer */
    unsigned fifo_size; /* size in bytes of the compressed buffer */
    uint32_t pts; /* current PTS of the audio decoder */
    bdecode_pts_type pts_type;  /* current PTS type of the codec */
    uint32_t stc; /* current STC used by the codec */
    unsigned short program; /* current program being decoded. see bdecode_set_audio_program. */
    bool    started; /* true if codec was started */
    unsigned pts_error_cnt; /* counter for number of PTS errors since start of decode */  
} bcodec_status;


/*
Summary:
    Audio decode status returned by baudio_decode_get_status
*/
typedef struct baudio_decode_status {
    bcodec_status common; /* codec status */
    baudio_format codec; /* audio stream format */
    unsigned sample_rate; /* sample rate in hz */
    unsigned short bitrate; /* in Kbps */
    unsigned short frame_queue; /* number of audio frames accumulated in the compressed decoder buffer */
    bool compressed_spdif_output; /* true = compressed spdif output, false = pcm or disabled */
    char channels[16]; /* number of discrete audio source channels (x.x format) */
    char downmix_mode[16]; /* downmix mode string */
    char layer[16]; /* audio layer mode string */
} baudio_decode_status;

/*
Summary:
    Video decode status returned by bvideo_decode_get_status
*/
typedef struct bvideo_decode_status {
    bcodec_status common; /* codec status */
    bvideo_codec codec; /* if bvideo_codec_none, it is analog or other external source */
    bvideo_frame_rate frame_rate;
    bdecode_panscan_status panscan_status; /* current video panscan status of decoder */
    unsigned short source_width;    /* width in pixels of the source video */
    unsigned short source_height;   /* height in pixels of the source video */
    unsigned short picture_queue; /* return the number of decoded pictures ready to be displayed */
    bool interlaced;
    unsigned aspect_ratio; /* value of aspect_ratio_information field from decoded MPEG stream (see ISO/IEC 13818-2) */
    bdecode_time_code time_code;
    unsigned display_drops; /* Number of video display drops since start of decode */
    unsigned display_underflows;  /* Number of video display underflows since start of decode */
} bvideo_decode_status;


/*
Summary:
    Decode status returned by bdecode_get_status
*/
typedef struct bdecode_status {
    unsigned int source_width;  /* width in pixels of the source video */
    unsigned int source_height; /* height in pixels of the source video */
    bool interlaced;
    bvideo_codec video_codec; /* if bvideo_codec_none, it is analog or other external source */
    bvideo_frame_rate video_frame_rate;
    
    unsigned long video_fifo_depth; /* depth in bytes of the compressed video buffer (VBV) */
    unsigned long video_fifo_size; /* size in bytes of the compressed video buffer (VBV) */
    unsigned long video_pts; /* current PTS of the video decoder */
    bdecode_pts_type video_pts_type;    /* current PTS type of the video decoder    */
    unsigned long video_stc; /* current STC used by the video decoder */
    unsigned video_aspect_ratio; /* value of aspect_ratio_information field from decoded MPEG stream (see ISO/IEC 13818-2) */
    unsigned video_program; /* current video program being decoded. see bdecode_set_video_program. */
    unsigned video_picture_queue; /* return the number of decoded pictures ready to be displayed */
    unsigned video_pts_error_cnt; /* counter for number of PTS errors since start of decode */
    unsigned video_display_drops; /* Number of video display drops since start of decode */
    unsigned video_display_underflows;  /* Number of video display underflows since start of decode */
    
    unsigned long audio_fifo_depth; /* depth in bytes of the compressed audio buffer */
    unsigned long audio_fifo_size; /* size in bytes of the compressed audio buffer */
    unsigned long audio_pts; /* current PTS of the audio decoder */
    bdecode_pts_type audio_pts_type;    /* current PTS type of the audio decoder */
    unsigned long audio_stc; /* current STC used by the audio decoder */
    unsigned audio_program; /* current audio program being decoded. see bdecode_set_audio_program. */
    baudio_format audio_format; /* audio stream format */
    unsigned int audio_sample_rate; /* sample rate in hz */
    char audio_channels[16]; /* number of discrete audio source channels (x.x format) */
    char audio_downmix_mode[16]; /* downmix mode string */
    char audio_layer[16]; /* audio layer mode string */
    bool audio_compressed_spdif_output; /* true = compressed spdif output, false = pcm or disabled */
    unsigned short audio_frame_queue; /* number of audio frames accumulated in the compressed decoder buffer */
    bdecode_panscan_status video_panscan_status; /* current video panscan status of decoder */
    bdecode_time_code time_code;
    bool disco_event; /* indicates a discontinuity has occured since the last time bdecode_get_status() was called */
    bool underflow_event; /* indicates an underflow has occured since the last time bdecode_get_status() was called */
    unsigned long audio_bitrate; /* in Kbps */
    /* PR25456 20061115 bandrews - Added for audio passthrough status and better design, should deprecate other stati */
    unsigned audio_pts_error_cnt; /* counter for number of PTS errors since start of decode */
} bdecode_status;

/*
Summary:
    Get the status of the audio decoder
*/
void baudio_decode_get_status(
    baudio_decode_t audio, /* audio decoder handle */
    baudio_decode_status *status /* [out] status to be populated */
    );

/*
Summary:
    Get the status of the video decoder
*/
void bvideo_decode_get_status(
    bvideo_decode_t video, /* video decoder handle */
    bvideo_decode_status *status/* [out] status to be populated */
    );

/*
Summary:
    Get the status of the decoder and current source.
*/
bresult bdecode_get_status(
    bdecode_t decode,  /* handle returned by bdecode_open */
    bdecode_status *status  /* [out] status to be populated */
    );

/*
Summary:
    Set ADP (mpegaudio decoder) volume.
*/
bresult bdecode_set_audio_volume(
    bdecode_t decode,
    const baudio_volume *volume
    );

/*
Summary:
    Get ADP (mpegaudio decoder) volume.
*/
bresult bdecode_get_audio_volume(
    bdecode_t decode,
    baudio_volume *volume   /* [out] */
    );

/* 
Summary:
   This function is used to retriece current value of the System Time Counter (STC)
   associated with the decoder channel
*/
bresult bdecode_get_stc(bdecode_t decode, uint32_t *stc);

/* 
Summary:
   This function is used to set the System Time Counter (STC)
   for the decoder channel
*/
bresult bdecode_set_stc(bdecode_t decode, uint32_t stc);

/*
Summary:
    Slow the clock to ensure buffer is filling
*/
void bdecode_slow_clock(
    bdecode_t decode,
    bool slow
    );

/**
Summary:
Read raw data from decoder for debug or external processing.

Description:
This is used for capturing CRC data for decode verification.
It may also be used in the future for capturing raw userdata or other data.

The bytestream returned has a format of a header and sized payload. This allows for multiplexing of different data 
along with future-proofing for new types. Applications should only parse the types they understand and 
skip all other types. The format is:

  uint32_t type;
  uint32_t payload_size;
  uint8_t  payload[payload_size];
  
Each call to bdecode_read_metadata is not guaranteed to start with the type field. You can do partial 
reads of blocks. All data is in host endianness.

For CRC data, type is 0. The payload is a stream of the following format:

  uint32_t luma;
  uint32_t chroma;

**/
bresult bdecode_read_metadata(
    bdecode_t decode,
    void *buffer, /* [size_is(size)] memory buffer where metadata will be read into */
    unsigned size, /* size of buffer */
    unsigned *amount_read /* [out] amount of data read into buffer */
    );

/**
Summary:
Have display path allocate an extra capture buffer so that the app can call bdecode_get_capture_buffer.
**/
bresult bdecode_allocate_capture_buffers(
    bdecode_t decode, 
    unsigned count /* 0 is disabled, otherwise it's the number of user capture buffers that are allowable at a time. */
    );

/**
Summary:
Return a current capture buffer to an application.
Description:
You can capture up to a certain number of buffers, specified by the count passed into bdecode_allocate_capture_buffer.
Every buffer you capture, you must return with bdecode_return_capture_buffer.
**/
bsurface_t bdecode_acquire_capture_buffer(
    bdecode_t decode,
    bgraphics_t graphics,
    bool *top_field /* [out] returns true if bsurface_t is for a top field of an interlaced source. */
    );
 
/**
Summary:
Return a bsurface_t capture buffer which was received from bdecode_get_capture_buffer.
**/
bresult bdecode_release_capture_buffer(
    bdecode_t decode, 
    bsurface_t surface
    );
    
/* End of Decode API  */

#ifdef __cplusplus
}
#endif


#endif /* BSETTOP_DECODE_H__ */
