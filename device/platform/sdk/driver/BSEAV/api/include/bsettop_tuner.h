/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_tuner.h $
 * $brcm_Revision: 29 $
 * $brcm_Date: 5/8/12 2:13p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/include/bsettop_tuner.h $
 * 
 * 29   5/8/12 2:13p mphillip
 * SW3462-12: Add dvbc2 tuner type
 * 
 * 28   3/13/12 4:11p mphillip
 * SW7344-263: Update lna and tuner status for 7344/7346
 * 
 * 27   3/12/12 3:48p erickson
 * SW3461-169: add btuner_get_fast_status
 * 
 * 26   1/2/12 2:51p erickson
 * SW7552-156: add btuner_ofdm_status.signal_strength
 * 
 * SW7552-156/1   12/27/11 6:10p bingz
 * SW7552-156: Need to add signal level as opposed to RF and IF
 * 
 * 25   6/6/11 2:22p mphillip
 * SW7231-32: Add 3461 support
 * 
 * 24   5/23/11 3:37p jgarrett
 * SW7346-213: Adding btuner_untune
 * 
 * 23   1/26/11 11:04a jgarrett
 * SW7125-769: Adding annex c
 * 
 * 22   8/5/10 10:25a jgarrett
 * SW7550-512: Merge to main branch
 * 
 * SW7550-512/1   8/5/10 3:03p nitinb
 * SW7550-512: Added enum value btuner_ofdm_mode_dvbt2
 * 
 * 21   6/4/10 11:07a vsilyaev
 * SW7550-236: Extended QAM status
 * 
 * SW7550-236/3   5/21/10 3:04p pinglong
 * SW7550-236: move the postRsBer calculation to info_status.cpp
 * 
 * SW7550-236/2   4/21/10 9:54p pinglong
 * SW7550-236: It's not right for some item for status page of brutus.
 * 
 * SW7550-236/1   4/16/10 7:59p pinglong
 * SW7550-236: It's not right for some item for status page of brutus.
 * 
 * 20   2/5/10 1:51p mphillip
 * CR7550-115: Add QAM/DVB-C support for 7550
 * 
 * 19   12/11/09 4:57p mphillip
 * SW7550-112: Merge 7550 changes to main branch
 * 
 * Refsw_7550/1   11/23/09 2:36p chengli
 * SW7550-64: add enum btuner_ofdm_mode
 * 
 * 18   6/30/09 12:33p vsilyaev
 * PR 56070: Changed units of dsChannelPower field
 * 
 * 17   12/18/08 6:36p jgarrett
 * PR 49983: Adding more items to btuner_sds_status
 * 
 * 16   11/14/08 5:32p jgarrett
 * PR 48985: Separating turbo and ldpc modes
 * 
 * 15   10/15/08 5:26p vsilyaev
 * PR47842: BER calculation software implementation
 * 
 * PROD_DSG_REFSW_Devel/1   10/14/08 7:40p anilmm
 * PR47842: BER calculation software implementation
 * 
 * 14   1/31/08 5:15p jgarrett
 * PR 39180: Adding ofdm tuning
 * 
 * 13   2/22/07 4:41p vsilyaev
 * PR 26335: Added diseqc_message for the diseq configuration
 * 
 * 12   1/30/07 10:32a vsilyaev
 * PR26409: add btuner_qam_us_mode_docsis
 * 
 * PROD_TRINITY_REFSW_Devel/1   1/29/07 6:35p haisongw
 * PR26409: add btuner_qam_us_mode_docsis
 * 
 * 11   10/5/06 1:54p dlwin
 * PR 24762: add two moreAnnex mode for CableCARD OOB DS
 * 
 * 8   9/11/06 12:11p dlwin
 * PR 24033: Updated with additional mode of operation for Qam upstream
 * 
 * 7   8/31/06 2:44p dlwin
 * PR 24033: Create new public API for Qam Upstream
 * 
 * 6   5/3/06 10:02a erickson
 * PR19790: added LDPC support
 * 
 * 5   5/20/05 8:33a dlwin
 * PR 15503: Add support getting Block correctable/uncorrectable and
 * corrected error count.
 * 
 * 4   4/8/05 1:43p erickson
 * PR14721: added lock_callback and wait_for_lock to all digital tune
 * params
 * 
 * 3   3/28/05 1:58p erickson
 * PR14613: updated notes on linein id
 * 
 * 2   2/12/05 11:07a dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/44   2/10/05 3:15p erickson
 * PR13823: updated comments
 * 
 * Irvine_BSEAVSW_Devel/43   11/4/04 2:29p erickson
 * PR13169: updated docs on int/ext_agc_level to normalize along all
 * platforms
 * 
 * Irvine_BSEAVSW_Devel/42   10/25/04 11:52a vsilyaev
 * PR 12576: Added colorspace for analog video.
 * 
 * Irvine_BSEAVSW_Devel/41   9/16/04 3:36p erickson
 * PR11081: added box_detect and deinterlacer to bdecode_window_settings
 * and comb_filter to banalog_params
 * 
 * Irvine_BSEAVSW_Devel/40   7/15/04 5:13p erickson
 * PR11682: added btuner_tune_oob
 * 
 * Irvine_BSEAVSW_Devel/39   7/14/04 3:14p erickson
 * PR11682: added notes on btuner ids
 * 
 * Irvine_BSEAVSW_Devel/38   7/14/04 2:36p erickson
 * PR11682: added QAM OOB api support
 * 
 * Irvine_BSEAVSW_Devel/37   2/3/04 8:08p vsilyaev
 * PR 9465: changed if_freq in VSB parameters to be unsigned.
 * 
 * Irvine_BSEAVSW_Devel/36   1/26/04 8:56p vsilyaev
 * PR 9465: Added VSB support
 * 
 * Irvine_BSEAVSW_Devel/35   12/19/03 4:38p vsilyaev
 * PR 8850: Fixing build of legacy code after merge.
 * 
 * Irvine_BSEAVSW_Devel/Irvine_BSEAVSW_Devel_7038/1   12/4/03 1:27p erickson
 * PR8850: initial Brutus support on 7038
 * 
 * Irvine_BSEAVSW_Devel/33   11/20/03 10:44a erickson
 * PR8582: converted BER to be log10 of ber_estimate. This gives the
 * correct range of information.
 * 
 * Irvine_BSEAVSW_Devel/32   11/20/03 9:38a erickson
 * PR8582: fixed ber_estimate
 * 
 * Irvine_BSEAVSW_Devel/31   11/19/03 4:57p erickson
 * PR8583: resolved some naming issues, especially bvideo_format
 * 
 * Irvine_BSEAVSW_Devel/30   11/7/03 4:28p erickson
 * PR8583: removing, fixing or implementing TODO's
 *
 * Irvine_BSEAVSW_Devel/29   11/7/03 1:32a erickson
 * PR8582: changed qam ds status structure in order to get FEC corr/uncorr
 * values
 * 
 * Irvine_BSEAVSW_Devel/28   11/4/03 2:42p erickson
 * documentation changes
 *
 * Irvine_BSEAVSW_Devel/27   11/3/03 12:51p erickson
 * normalized constellation for 73xx. Need to test on other platforms.
 * 
 * Irvine_BSEAVSW_Devel/26   10/31/03 12:17p erickson
 * added some comments
 * 
 * Irvine_BSEAVSW_Devel/25   10/31/03 11:58a erickson
 * settop api changes after internal review
 * 
 * Irvine_BSEAVSW_Devel/24   10/28/03 11:26a erickson
 * settop api reworks after internal design view
 * 
 * Irvine_BSEAVSW_Devel/23   10/22/03 11:49a erickson
 * settop api pass-through type conversion
 * 
 * Irvine_BSEAVSW_Devel/22   10/22/03 11:22a erickson
 * settop api pass-through type conversion
 * 
 * Irvine_BSEAVSW_Devel/21   10/1/03 10:06a erickson
 * fixed docjet errors
 * 
 * Irvine_BSEAVSW_Devel/20   9/30/03 1:13a erickson
 * updated docjet documentation
 * 
 * Irvine_BSEAVSW_Devel/19   9/17/03 5:03p erickson
 * updated documentation for DocJet
 * 
 * Irvine_BSEAVSW_Devel/18   8/22/03 1:22p erickson
 * fixed comment
 * 
 * Irvine_BSEAVSW_Devel/17   8/21/03 4:30p erickson
 * added analog status for video_present
 * 
 * Irvine_BSEAVSW_Devel/16   8/21/03 9:36a erickson
 * implemented get_soft_decisions and qam status
 * 
 * Irvine_BSEAVSW_Devel/15   8/19/03 11:54a erickson
 * initial brutus bringup in progress
 *
 * Irvine_BSEAVSW_Devel/14   8/18/03 11:02a erickson
 * initial bringup for brutus 7320, still in progress
 * 
 * Irvine_BSEAVSW_Devel/13   8/12/03 9:43a erickson
 * changes after pre-review meeting
 * 
 * Irvine_BSEAVSW_Devel/12   7/30/03 2:35p vsilyaev
 * Moved some header files to the bsettop_types.h
 *
 * Irvine_BSEAVSW_Devel/11   7/30/03 2:08p erickson
 * removed all return value comments because we're standardized and they
 * add nothing
 *
 * Irvine_BSEAVSW_Devel/10   7/29/03 6:03p erickson
 * more rework
 *
 * Irvine_BSEAVSW_Devel/9   7/28/03 4:43p erickson
 * more comment work
 *
 * Irvine_BSEAVSW_Devel/8   7/25/03 8:14p vsilyaev
 * Removed _impl suffix.
 * 
 * Irvine_BSEAVSW_Devel/7   7/25/03 4:22p erickson
 * in the middle of rework, does not compile
 * 
 * Irvine_BSEAVSW_Devel/6   7/22/03 7:09p vsilyaev
 * Renamed qpsk to sds.
 * 
 * Irvine_BSEAVSW_Devel/5   7/9/03 5:19p erickson
 * rework for new bband_t model
 *
 * Irvine_BSEAVSW_Devel/4   6/24/03 5:29p vsilyaev
 * Renamed btumer_MPEG to bsettop_MPEG.
 *
 * Irvine_BSEAVSW_Devel/3   6/20/03 5:56p vsilyaev
 * Added field to store 'special' pids.
 *
 * Irvine_BSEAVSW_Devel/2   6/16/03 8:18p vsilyaev
 * Added pseudo for baseband tuner.
 *
 * Irvine_BSEAVSW_Devel/1   6/13/03 3:08p vsilyaev
 * SetTop reference API.
 *
 *
 ***************************************************************************/
#ifndef BSETTOP_TUNER_H__
#define BSETTOP_TUNER_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*=*************
The tuner inteface represents the front-end of the set-top box.
For digital signals, the tuner API performs both tuning and
acquisition functions.

If you tune an analog signal, you will get a bstream_t handle which you can decode or encode.

If you tune (and acquire) a digital signal, you will get a bband_t handle which you can scan
for message data (bmessage_t) or combine with already-known mpeg data form a digital stream. See bstream_open for details.

****************/

/*
Summary:
Open a tuner.

Description:
On most platforms, the tuner_id is simply an index of the tuners.

On platforms with more frontend options (e.g. 97038), please see bsettop_ids.txt 
for documentation on special numbers.
*/
btuner_t btuner_open(
                     bobject_t tuner_id /* handle used to identify a particular tuner */
                    );

/*
Summary:
    Close a tuner.
*/
void btuner_close(
                  btuner_t tuner /* handle returned by btuner_open */
                 );
/*
Summary:
    Untune a tuner.
*/

void btuner_untune(
    btuner_t tuner /* handle returned by btuner_open */
    );

/*
Summary:
    QAM annex.
*/
typedef enum btuner_qam_annex {
   btuner_qam_annex_a,  /* Used for PAL */
   btuner_qam_annex_b,  /* Used for NTSC */
   btuner_qam_oob_annex_a, /* legacy OOB mode for DAVIC support. When this mode is selected,
        btuner_qam_mode is ignored. */
   btuner_qam_oob_dvs167 = btuner_qam_oob_annex_a,/* annex A is also called DVS167*/
   btuner_qam_oob_dvs178, /* legacy OOB mode for DVS178. When this mode is selected,
        btuner_qam_mode is ignored. */
   btuner_qam_oob_pod_annex_a, /* CableCARD OOB mode for DAVIC support. When this mode is selected,
        btuner_qam_mode is ignored. */
   btuner_qam_oob_pod_dvs167 = btuner_qam_oob_pod_annex_a,/* annex A is also called DVS167*/
   btuner_qam_oob_pod_dvs178, /* CableCARD OOB mode for DVS178. When this mode is selected,
        btuner_qam_mode is ignored. */
    btuner_qam_annex_c  /* Used in Japan */
} btuner_qam_annex;

/*
Summary:
    Supported QAM modes.
*/
typedef enum btuner_qam_mode {
  btuner_qam_mode_16 = 16,
  btuner_qam_mode_32 = 32,
  btuner_qam_mode_64 = 64,
  btuner_qam_mode_128 = 128,
  btuner_qam_mode_256 = 256,
  btuner_qam_mode_1024 = 1024
} btuner_qam_mode;

/*
Summary:
    Colorspace for analog video
*/
typedef enum btuner_analog_colorspace {
  btuner_analog_colorspace_rgb,
  btuner_analog_colorspace_yprpb
} btuner_analog_colorspace;

/*
Summary:
    spectrum setting for QAM
*/
typedef enum btuner_qam_spectral_inversion {
  btuner_qam_spectral_inversion_auto,
  btuner_qam_spectral_inversion_noinverted,
  btuner_qam_spectral_inversion_inverted
} btuner_qam_spectral_inversion;

/***************************************************************************
Summary:
    Enumeration for receiver bandwidth
****************************************************************************/
typedef enum btuner_qam_bandwidth
{
    btuner_qam_bandwidth_5Mhz,
    btuner_qam_bandwidth_6Mhz,
    btuner_qam_bandwidth_7Mhz,
    btuner_qam_bandwidth_8Mhz,
    btuner_qam_bandwidth_Max
} btuner_qam_bandwidth;


/*
Summary:
    QAM parameters to be passed to btuner_tune_qam().
*/
typedef struct btuner_qam_params {
    btuner_qam_mode mode;
    unsigned symbol_rate; /* Baud */
    btuner_qam_annex annex;
    btuner_qam_spectral_inversion spectrum;
    bool wait_for_lock;
    bsettop_callback lock_callback; /* called when the tuner comes in or out
                                    of lock. call btuner_get_qam_status to determine
                                    the state. */
    void *callback_context;         /* passed as parameter to lock_callback */
    bool auto_acquire; /* True = Enable, False = Disable. */
    btuner_qam_bandwidth bandwidth; /* Tuner frequency bandwidth.  */
    bool enable_power_measurement; /* True = enables calculation of downstream channel power for some frontends like 31xx. For others it is always enabled. False = Disable. */
    unsigned frequencyOffset; /* Automatic Frequency offset /carrier range from the tuned frequency in Hz. */
} btuner_qam_params;

/*
Summary:
    Receiver bandwidth
*/
typedef enum btuner_ofdm_bandwidth
{
   btuner_ofdm_bandwidth_5Mhz=5,   /* Selects 5 MHz */
   btuner_ofdm_bandwidth_6Mhz,     /* Selects 6 MHz */
   btuner_ofdm_bandwidth_7Mhz,     /* Selects 7 MHz */
   btuner_ofdm_bandwidth_8Mhz      /* Selects 8 MHz */
}btuner_ofdm_bandwidth;

/*
Summary:
    Receiver CCI (CoChannel Interference Filter) Mode
*/
typedef enum btuner_ofdm_cci_mode
{
   btuner_ofdm_cci_mode_none,    /* No CCI selected */
   btuner_ofdm_cci_mode_auto     /* Auto Selection */
}btuner_ofdm_cci_mode;
/*
Summary:
   
    DVB-T can hierarchically encode streams.  A low-priority stream can
    be embedded within a high priority stream.  Receivers with good reception
    conditions may be able to receive both streams, but receivers with poor 
    reception conditions may only be able to receive the high priority stream.

*/
typedef enum btuner_ofdm_priority
{
   btuner_ofdm_priority_high,
   btuner_ofdm_priority_low
}btuner_ofdm_priority;

/*
Summary:
   
    ISDB-T is a kind of OFDM modulation technology. It is needed to differentiate 
    ISDB-T and DVB-T in the channel map.

*/
typedef enum btuner_ofdm_mode
{
   btuner_ofdm_mode_dvbt,
   btuner_ofdm_mode_dvbt2,
   btuner_ofdm_mode_dvbc2,
   btuner_ofdm_mode_isdbt
}btuner_ofdm_mode;


/* Summary:
    OFDM parameters to be passed to btuner_tune_ofdm().
*/
typedef struct btuner_ofdm_params {
    btuner_ofdm_bandwidth bandwidth; /* Signal Bandwidth */
    btuner_ofdm_cci_mode cci_mode;   /* CCI Mode */
    btuner_ofdm_priority priority;   /* priority */
    btuner_ofdm_mode mode;           /* OFDM mode: dvbt or isdbt */
    bool wait_for_lock;
    bsettop_callback lock_callback; /* called when the tuner comes in or out
                                       of lock. call btuner_get_qam_status to determine
                                       the state. */
    void *callback_context;         /* passed as parameter to lock_callback */
}btuner_ofdm_params;

/*
Summary:
    Required to initialize QAM parameters to defaults
*/
void btuner_qam_params_init(
    btuner_qam_params *qam,     /* [out] */
    btuner_t tuner              /* required for possible resource-dependent defaults */
    );

/*
Summary:
    Required to initialize DVB-T/OFDM demod parameters to defaults
*/
void btuner_ofdm_params_init(
    btuner_ofdm_params *ofdm,   /* [out] */ 
    btuner_t tuner              /* required for possible resource-dependent defaults */
    );

/*
Summary:
    Analog parameters passed to btuner_tune_rf() and btuner_tune_linein().
*/
typedef struct btuner_analog_params {
    bvideo_format video_format; /* format of the source video */
    unsigned audio_rate;    /* sample rate bits for PCM audio in units of
                                bits per second. 32000, 44100, 48000 supported. */
    boutput_rf_btsc_mode btsc_mode; /* BTSC decoder selection. This doesn't affect
                                pass-through. It selects what program will be
                                converted to PCM for encoding or output from
                                the DACs. */
    bool comb_filter;       /* If true, the 3d comb filter is allowed to be
                                used if available and appropriate. */
    btuner_analog_colorspace colorspace; /* input colorspace, applicable only for
                                            certain type of inputs, e.g. component */
} btuner_analog_params;

/*
Summary:
  Required to initialize btuner_analog_params to defaults.
*/
void btuner_analog_params_init(
    btuner_analog_params *analog,   /* [out] */
    btuner_t tuner                  /* required for possible resource-dependent defaults */
    );

/*
Summary:
    Operation Mode for OutOfBand upstream
*/
typedef enum btuner_qam_us_mode {
    btuner_qam_us_mode_annexa,/* Upstream mode for Annex A*/
    btuner_qam_us_mode_dvs167 = btuner_qam_us_mode_annexa,/* Annex A is also called DVS 167*/
    btuner_qam_us_mode_dvs178,/* Upstream mode for DVS178*/
    btuner_qam_us_mode_pod_annexa,/* Upstream mode for CableCARD Annex A*/
    btuner_qam_us_mode_pod_dvs167 = btuner_qam_us_mode_pod_annexa,/* Annex A is also called DVS 167*/
    btuner_qam_us_mode_pod_dvs178, /* Upstream mode for CableCARD DVS178*/ 
    btuner_qam_us_mode_docsis /* Upstream mode for Docsis*/
} btuner_qam_us_mode;

/*
Summary:
    QPSK parameters to be passed to btuner_tune_us().
*/
typedef struct btuner_qam_us_params {
    btuner_qam_us_mode mode;  /* Operation Mode*/
    unsigned symbol_rate; /* Baud */
    unsigned power_level; /* Power Level in dBmV */
} btuner_qam_us_params;

/*
Summary:
    Required to initialize Upstream parameters to defaults
*/
void btuner_qam_us_params_init(
    btuner_qam_us_params *us_param,     /* [out] */
    btuner_t tuner              /* required for possible resource-dependent defaults */
    );

/*
Summary:
    DISEQC polarity for btuner_sds_params.
*/
typedef enum btuner_sds_diseqc_polarity {
    btuner_sds_diseqc_polarity_13v = 13,
    btuner_sds_diseqc_polarity_18v = 18
} btuner_sds_diseqc_polarity;

/*
Summary:
    SDS acquisition mode.
*/
typedef enum btuner_sds_mode {
   btuner_sds_mode_qpsk_dvb,
   btuner_sds_mode_qpsk_dss,
   btuner_sds_mode_qpsk_dcii,
   btuner_sds_mode_qpsk_turbo,
   btuner_sds_mode_8psk_turbo,
   btuner_sds_mode_turbo_qpsk=btuner_sds_mode_qpsk_turbo,
   btuner_sds_mode_turbo_8psk=btuner_sds_mode_8psk_turbo,
   btuner_sds_mode_turbo,
   btuner_sds_mode_qpsk_ldpc,
   btuner_sds_mode_8psk_ldpc,
   btuner_sds_mode_ldpc
} btuner_sds_mode;

/*
Summary:
    SDS spectral inversion mode.
*/
typedef enum btuner_sds_spectral_inversion {
   btuner_sds_spectral_inversion_scan,      /* Scan the stream for the value */
   btuner_sds_spectral_inversion_normal,
   btuner_sds_spectral_inversion_i,
   btuner_sds_spectral_inversion_q
} btuner_sds_spectral_inversion;

/*
Summary:
    Code rate of the sds stream.
*/
typedef struct btuner_sds_coderate {
    unsigned num;   /* numerator of the code rate fraction.
                    If 0, the code rate is scanned. */
    unsigned denom; /* denominator of the code rate fraction.
                    If 0, the code rate is scanned. */
    unsigned bits_per_symbol;   /* x1000. If 0, it uses the default. 
                    For instance 2050 is 2.05. */
} btuner_sds_coderate;

/*
Summary:
    Satellite downstream parameters to be passed to btuner_tune_sds
*/
typedef struct btuner_sds_params {
    btuner_sds_mode mode;
    btuner_sds_coderate coderate;
    unsigned symbol_rate;   /* Baud */
    unsigned carrier_offset;    /* Hz */
    btuner_sds_spectral_inversion spinv;
    unsigned search_range; /* Hz */
    bool diseqc_tone; /* on/off */
    bool ldpc_pilot; /* This option applies only for LDPC modes (low density parity check) */
    bool ldpc_pilot_pll; /* This option applies only for LDPC modes if ldpc_pilot is true */
    btuner_sds_diseqc_polarity diseqc_polarity;

    bool wait_for_lock; /* if true, the tune function will not return until locked or until a certain timeout */
    bsettop_callback lock_callback; /* called when the tuner comes in or out
                                    of lock. call btuner_get_sds_status to determine
                                    the state. */
    void *callback_context;         /* passed as parameter to lock_callback */

    /* NOTE: This diseqc API is under development. It may change. */
    struct {
        bool enabled; /* if true, send a diseqc message */
        bool tone_burst;
        uint8_t diseqc_sat_number;
    } diseqc_message;
} btuner_sds_params;

/*
Summary:
    Required to initialize SDS parameters to defaults
*/
void btuner_sds_params_init(
    btuner_sds_params *sds,     /* [out] */
    btuner_t tuner              /* required for possible resource-dependent defaults */
    );


typedef enum btuner_vsb_mode {
   btuner_vsb_mode_vsb8=8, /* 8-VSB */
   btuner_vsb_mode_vsb16=16 /* 16-VSB */
} btuner_vsb_mode;

typedef struct btuner_vsb_params {
    btuner_vsb_mode mode;
    unsigned if_freq; /* IF Frequency in Hz */
    unsigned symbol_rate; /* Baud */
    signed symbol_rate_offset; /* Baud */
    signed if_freq_offset; /* IF Frequency offset in Hz */
    signed ntsc_offset; /* Hz */
  
    bool wait_for_lock;
    bsettop_callback lock_callback; /* called when the tuner comes in or out
                                    of lock. call btuner_get_vsb_status to determine
                                    the state. */
    void *callback_context;         /* passed as parameter to lock_callback */
} btuner_vsb_params;

/*
Summary:
    Required to initialize VSB parameters to defaults
*/
void btuner_vsb_params_init(
    btuner_vsb_params *vsb,     /* [out] */
    btuner_t tuner              /* required for possible resource-dependent defaults */
    );


/*
Summary:
Open a line-in object.

Description:
The linein_id has a platform specific meaning. The number of lineinputs are
enumerated in bsettop_ids.txt.
*/
btuner_linein_t btuner_linein_open(bobject_t linein_id);

/*
Summary:
    Tune and acquire QAM downstream band.
*/
bband_t btuner_tune_qam(
    btuner_t tuner,
    unsigned freq, /* RF center frequency in Hz */
    const btuner_qam_params *params /* parameters needed to tune and acquire */
    );

/*
Summary:
Tune and acquire out-of-band stream.

Description:
We re-use the btuner_qam_params structure, but the btuner_qam_mode is ignored.
This returns b_ok if lock was obtained.
*/
bresult
btuner_tune_oob(
    btuner_t tuner,
    unsigned freq, /* RF center frequency in Hz */
    const btuner_qam_params *params /* parameters needed to tune and acquire */
    );


/*
Summary:
Tune QAM/QPSK Upstream.
*/
bresult
btuner_tune_qam_us(
    btuner_t tuner,
    unsigned freq, /* RF center frequency in Hz */
    const btuner_qam_us_params *params  /* parameters needed to tune */
    );

/*
Summary:
    Tune and acquire a satellite downstream band
*/
bband_t btuner_tune_sds(
    btuner_t tuner,
    unsigned freq, /* RF center frequency in Hz */
    const btuner_sds_params *params /* parameters needed to tune and acquire */
    );

/*
Summary:
    Tune and acquire a VSB downstream band
*/
bband_t btuner_tune_vsb(
    btuner_t tuner,
    unsigned freq, /* RF center frequency in Hz */
    const btuner_vsb_params *params /* parameters needed to tune and acquire */
    );

/*
Summary:
    Tune and acquire a BVD-T downstream band
*/
bband_t
btuner_tune_ofdm(
    btuner_t tuner, 
    unsigned freq,  /* RF center frequency in Hz */ 
    const btuner_ofdm_params* params /* parameters needed to tune and acquire */
    );


/*
Summary:
    Tune rf (analog cable).
*/
bstream_t btuner_tune_rf(
    btuner_t tuner,
    unsigned carrierfreq,   /* RF frequency for the tuner in Hz */
    const btuner_analog_params *params  /* parameters for the analog video source */
    );

/*
Summary:
    Tune line-in
*/
bstream_t btuner_tune_linein(
    btuner_t tuner,
    btuner_linein_t linein, /* which linein to select */
    const btuner_analog_params *params  /* parameters for the analog video source */
    );

/*
Summary:
    Status information returned by btuner_get_analog_status().
*/
typedef struct btuner_analog_status {
    bool video_present;
} btuner_analog_status;

/*
Summary:
    Get status of an analog signal on a tuner.
*/
bresult btuner_get_analog_status(
    btuner_t tuner, 
    btuner_analog_status *status    /* [out] */
    );


/*
Summary:
    QAM status returned by btuner_get_qam_status.
*/
typedef struct btuner_qam_status {
    bool fec_lock;          /* Is the FEC locked? */
    bool qam_lock;          /* Do we have QAM lock? */
    unsigned symbol_rate;   /* Baud. received symbol rate (in-band) */
    unsigned snr_estimate;  /* snr estimate in dB (in-Band). x256 */
    unsigned  int_agc_level;/* IF AGC level in units of 1/10 percent */
    unsigned  ext_agc_level;/* tuner AGC level in units of 1/10 percent */
    int ber_estimate;       /* Bit error rate as log10 of 0.0-1.0 range.
                                1.0  => 100% => 0
                                0.1  => 10%  => -1
                                0.01 => 1%   => -2
                                0    => 0%   => 1 (special value for NONE) */
    int freq_err;           /* carrier offset in Hz            */
    int freq_er2;           /* ###@@@derotator offset in Hz        */
    unsigned fec_corr_cnt;  /* cumulative FEC correctable errors */
    unsigned fec_uncorr_cnt;/* cumulative FEC uncorrectable errors */
    int dsChannelPower;     /* OCAP DPM support for video channels  in 10s of dBmv. */
    unsigned mainTap;       /* Channel main tap coefficient */
    int equalizerGain;      /* Channel equalizer gain value in dBm */
    unsigned postRsBer;         /* OCAP requires postRsBER for all DS channels. postRsBer will be reset on every channel change.  Same units as ber_estimate*/
    unsigned postRsBerElapsedTime;    /* postRsBER over this time. In units of seconds */  
    unsigned fec_clean_cnt;
    btuner_qam_annex annex;
} btuner_qam_status;

/*
Summary:
    Get QAM demodulation status.
*/
bresult btuner_get_qam_status(
    btuner_t tuner,
    btuner_qam_status *status   /* [out] */
    );

/*
Summary:
    Upstream status returned by btuner_get_us_status.
*/
typedef struct btuner_qam_us_status {
    btuner_qam_us_mode mode;  /* Operation Mode*/
    unsigned symbol_rate; /* Baud */
    unsigned power_level; /* Power Level in dBmV*/
    unsigned freq; /* Frequency in Hz */
} btuner_qam_us_status;

/*
Summary:
    Get Upstream status.
*/
bresult btuner_get_qam_us_status(
    btuner_t tuner,
    btuner_qam_us_status *status    /* [out] Current status of the Upstream mod*/
    );

/*
Summary:
    Summary of sds acquisition returned by btuner_get_sds_status.
*/
typedef struct btuner_sds_status {
    btuner_sds_coderate coderate;
    btuner_sds_spectral_inversion spectral_inversion;
    unsigned sample_frequency;  /* Hz */
    unsigned carrier_error;    /* carrier error in Hz */
    unsigned output_bit_rate;  /* output bit rate in bps */
    unsigned snr_estimate;     /* SNR in dB x256 */
    unsigned ber_estimate;     /* Bit error rate as log10 of 0.0-1.0 range.
                                1.0  => 100% => 0
                                0.1  => 10%  => -1
                                0.01 => 1%   => -2
                                0    => 0%   => 1 (special value for NONE)
                                If bert_locked == false, it's set to 1. */
    unsigned input_power;      /* input power estimate in dB x256 */
    unsigned fec_phase;     /* 0, 90, 180, 270 */
    unsigned rs_corr;       /* cumulative RS correctable errors */
    unsigned rs_uncorr;     /* cumulative RS uncorrectable errors */
    unsigned fec_corr;       /* cumulative FEC correctable errors */
    unsigned fec_uncorr;     /* cumulative FEC uncorrectable errors */
    unsigned fec_clean;     /* cumulative FEC clean */
    unsigned ber_errors;    /* BER error count */
    unsigned pre_viterbi_error_count;  /* accumulated pre-Viterbi error count */
    unsigned mpeg_errors;   /* mpeg frame error count */
    unsigned mpeg_count;    /* total mpeg frame count */
    unsigned if_agc;        /* if agc value from hw unmodified */
    unsigned rf_agc;        /* rf agc value from hw unmodified */
    struct {
        unsigned int_config;    /* info from lna status */
        unsigned ext_config;    /* info from lna status */
        unsigned version;       /* info from lna status */
        unsigned agc0;          /* info from lna status */
        unsigned agc1;          /* info from lna status */
    } lnaStatus;
    unsigned tuner_freq;    /* tuner frequency */
    unsigned channel;       /* channel */
    bool rs_locked;         /* is it locked? */
    bool vit_locked;        /* is it locked? */
    bool bert_locked;       /* is BER tester locked? If so, see ber_estimate. */
    int agf;                    /* AGF integrator value */
} btuner_sds_status;

/*
Summary:
    Get the status of SDS demodulation
*/
bresult btuner_get_sds_status(
    btuner_t tuner,
    btuner_sds_status *status   /* [out] Current status of the SDS demod */
    );



typedef struct btuner_vsb_status {
   btuner_vsb_mode mode;   /* modulation */
   bool      receiver_lock;   /* true if the receiver is locked */
   bool      fec_lock;       /* true if the FEC is locked */
   bool      opll_lock;      /* true if the output PLL is locked */

   unsigned  symbol_rate;       /* standard symbol rate in Baud */
   unsigned  symbol_rate_offset; /* symbol rate offset in Baud */
   signed    symbol_rate_error;  /* symbol rate error in Baud */

   unsigned  if_freq;           /* standard IF frequency in Hz */
   signed    if_freq_offset;    /* IF frequency offset in Hz */
   signed    if_freq_error;     /* IF frequency error in Hz */

   signed    ntsc_filter;       /* location of NTSC filter notch 1 in Hz */
   signed    ntsc_offset;       /* NTSC offset in Hz from host */
   signed    ntsc_error;        /* NTSC error in Hz */
   
   unsigned  int_agc_level;      /* IF AGC level in units of 1/10 percent */
   unsigned  ext_agc_level;     /* tuner AGC level in units of 1/10 percent */
   unsigned  snr_estimate;      /* SNR estimate in units of 1/256 dB */

    /* information for these counters are not available from 3510 */
   unsigned  blk_corr;       /* cumulative Block correctable errors */
   unsigned  blk_uncorr;     /* cumulative Block uncorrectable errors */
   unsigned  err_corr;       /* Bit correctable errors */
} btuner_vsb_status;

/*
Summary:
    Get the status of VSB demodulation
*/
bresult btuner_get_vsb_status(
    btuner_t tuner,
    btuner_vsb_status *status   /* [out] Current status of the SDS demod */
    );

typedef struct btuner_ofdm_status {
   bool      receiver_lock;  /* true if the receiver is locked */
   bool      fec_lock;       /* true if the FEC is locked */
   unsigned  carrier_offset; /* Offset of carrier loop in Hz */
   unsigned  timing_offset;  /* Offset of timing loop in Hz */
   int       snr;            /* SNR value of reciever in 1/100 dB */
   unsigned  blk_corr;       /* accumulated corrected blocks by FEC */
   unsigned  blk_uncorr;     /* accumlated uncorrected blocks */
   unsigned  blk_clean;      /* accumlated clean RS blocks */
   unsigned  acq_count;      /* number of reacquisitions performed */
   int signal_strength;      /* signal strength in dB */
} btuner_ofdm_status;

/*
Summary:
    Get the status of DVB-T demodulation
*/
bresult btuner_get_ofdm_status(
    btuner_t tuner,
    btuner_ofdm_status *status   /* [out] Current status of the SDS demod */
    );

/*
Summary:
    One soft decision returned by btuner_get_soft_decisions.
*/
typedef struct btuner_soft_decision {
    int16_t i;
    int16_t q;
} btuner_soft_decision;

/*
Summary:
    Get an array of soft decisions for QAM or SDS constellation.
Description:
    i and q range from 32767 to -32768 for all platforms.
    The precision varies for each platform.
*/
bresult btuner_get_soft_decisions(
    btuner_t tuner,
    btuner_soft_decision *data, /* [out,size_is(length)] array of soft decisions */
    size_t length /* number of btuner_soft_decision's to get */
    );

typedef enum btuner_lock_status
{           
    btuner_lock_status_unknown,  /* no status available */
    btuner_lock_status_unlocked, /* not locked, but there is a signal. */
    btuner_lock_status_locked,   /* locked */
    btuner_lock_status_noSignal, /* no signal; therefore not locked */
    btuner_lock_status_max 
} btuner_lock_status;

typedef struct btuner_fast_status {
    btuner_lock_status lock_status;
    bool acquire_in_progress; /* if true, you can wait for another lockCallback */
} btuner_fast_status;

bresult btuner_get_fast_status(
    btuner_t tuner,
    btuner_fast_status *status 
    );


#ifdef __cplusplus
}
#endif

#endif /* BSETTOP_TUNER_H__ */

