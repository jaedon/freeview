/***************************************************************************
*     (c)2004-2012 Broadcom Corporation
*
*  This program is the proprietary software of Broadcom Corporation and/or its licensors,
*  and may only be used, duplicated, modified or distributed pursuant to the terms and
*  conditions of a separate, written license agreement executed between you and Broadcom
*  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
*  no license (express or implied), right to use, or waiver of any kind with respect to the
*  Software, and Broadcom expressly reserves all rights in and to the Software and all
*  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
*  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
*  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
*
*  Except as expressly set forth in the Authorized License,
*
*  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
*  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
*  and to use this information only in connection with your use of Broadcom integrated circuit products.
*
*  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
*  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
*  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
*  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
*  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
*  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
*  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
*  USE OR PERFORMANCE OF THE SOFTWARE.
*
*  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
*  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
*  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
*  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
*  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
*  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
*  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
*  ANY LIMITED REMEDY.
*
* $brcm_Workfile: nexus_frontend_qam.h $
* $brcm_Revision: 35 $
* $brcm_Date: 10/5/12 3:11p $
*
* API Description:
*   API name: Frontend QAM
*    Generic APIs for QAM tuning.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/frontend/common/include/nexus_frontend_qam.h $
* 
* 35   10/5/12 3:11p erickson
* SW7425-4038: clarify NEXUS_FrontendQamSettings.lockCallback behavior
* 
* 34   3/9/12 11:31a erickson
* SW3128-128: restore equalizerGain, fix comment
* 
* 33   3/7/12 4:59p erickson
* SW3128-128: add gain fields to status
* 
* 32   3/7/12 12:35p erickson
* SW7552-183: improve BER comments
* 
* 31   2/7/12 2:30p erickson
* SW3461-133: standardize signal strength to units of 1/10 dBmV
* 
* 30   12/8/11 4:32p erickson
* SW7552-154: clarify that NEXUS_FrontendQamSettings.scan can be used in
*  eAuto mode
* 
* 29   12/5/11 3:47p erickson
* SW7552-154: add NEXUS_FrontendQamSettings.scan.frequencyOffset
* 
* 28   11/17/11 4:33p erickson
* SW3461-96: Remove GetLockStatus and replace it with GetFastStatus
* 
* 27   11/1/11 4:04p erickson
* SW3461-50, SW3128-16: change
*  NEXUS_FrontendQamScanStatus.frequencyOffset to signed int
* 
* 26   10/12/11 10:30a erickson
* SW3461-50, SW3128-16: add qam scan
* 
* 25   5/27/11 1:46p erickson
* SW7231-180: convert NEXUS_FrontendQamFrequencyOffset to unsigned with
*  backward-compat macros
* 
* 24   5/11/11 10:58a erickson
* SW3128-10: add asyncStatusReadyCallback
* 
* 23   11/9/10 5:11p erickson
* SW7125-726: add NEXUS_FrontendQamAnnex_eC
* 
* 22   10/21/10 5:51p mphillip
* SW7420-1020: Add asynchronous status API
* 
* 21   9/13/10 12:18p erickson
* SW35230-1306: deprecate NEXUS_FrontendXxxBandwidth. use unsigned
*  integer in units of Hz instead.
*
* 20   8/5/10 4:32p erickson
* SW7405-4722: Add support for "Frequency offset" and "Null packets" for
*  ADS and expose "auto acquire" in nexus for AOB
*
* 19   4/21/10 9:31a jgarrett
* SW7550-388: Merging to main branch
*
* 18   3/5/10 4:36p bandrews
* SW7405-3995: fix mistype
*
* 17   3/5/10 3:28p bandrews
* SW7405-3995: added spectrum mode and spectral inversion settings
*
* SW7550_236/1   4/15/10 1:43p farshidf
* SW7550_236: add the LNA AGD element in the status struct for 7550
*
* 16   1/19/10 5:39p mphillip
* SW7405-3722: Flag power management change on correct JIRA
*
* 15   1/15/10 1:53p mphillip
* SW7550-135: Add power management flag
*
* 14   12/31/09 4:13p erickson
* SW7550-135: add NEXUS_FrontendQamBandwidth
*
* 13   12/15/09 11:53a mphillip
* SW7405-3596: Expose QAM auto acquire setting as a user-configurable
*  parameter
*
* 12   7/20/09 12:47p erickson
* PR56816: add viterbiErrorRate status and related fields, improve
*  comments
*
* 11   6/30/09 9:10a erickson
* PR56070: changed type of NEXUS_FrontendQamStatus.dsChannelPower
*
* 10   5/21/09 10:23a erickson
* PR55095: added NEXUS_FrontendQamStatus.interleaveDepth
*
* 9   3/5/09 10:42a jgarrett
* PR 52817: Converting offset values to signed
*
* 8   10/14/08 3:33p erickson
* PR47842: added postRsBer fields to NEXUS_FrontendQamStatus
*
* 7   9/23/08 9:16a jgarrett
* PR 46513: Merging 97459 support to mainline
*
* 6   7/22/08 11:02a jgarrett
* PR 45000: Adding ifFrequency for QAM
*
* PR44830/1   7/22/08 7:47p anilmm
* PR44830: Add to QAM tuning status
*
* 5   5/1/08 11:19a erickson
* PR42376: default the QAM symbolRate based on the mode and annex if 0
*
* 4   2/12/08 2:34p jgarrett
* PR 39009: Unifying status
*
* 3   1/23/08 8:37p vobadm
* PR35457: update docs
*
* 2   1/23/08 5:16p erickson
* PR35457: update docs
*
* 1   1/18/08 2:19p jgarrett
* PR 38808: Merging to main branch
*
* Nexus_Devel/2   12/20/07 2:30p jgarrett
* PR 38019: API touchup
*
* Nexus_Devel/1   10/4/07 2:41p jgarrett
* PR 35551: Adding initial version
*
***************************************************************************/
#ifndef NEXUS_FRONTEND_QAM_H__
#define NEXUS_FRONTEND_QAM_H__

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
Summary:
QAM tuning modes
***************************************************************************/
typedef enum NEXUS_FrontendQamMode
{
    NEXUS_FrontendQamMode_e16,
    NEXUS_FrontendQamMode_e32,
    NEXUS_FrontendQamMode_e64,
    NEXUS_FrontendQamMode_e128,
    NEXUS_FrontendQamMode_e256,
    NEXUS_FrontendQamMode_e512,
    NEXUS_FrontendQamMode_e1024,
    NEXUS_FrontendQamMode_e2048,
    NEXUS_FrontendQamMode_e4096,
    NEXUS_FrontendQamMode_eAuto_64_256, /* Automatically scan both QAM-64 and QAM-256.
                                           Not available on all chipsets. */
    NEXUS_FrontendQamMode_eMax
} NEXUS_FrontendQamMode;

/***************************************************************************
Summary:
QAM Annex
***************************************************************************/
typedef enum NEXUS_FrontendQamAnnex
{
    NEXUS_FrontendQamAnnex_eA,
    NEXUS_FrontendQamAnnex_eB,
    NEXUS_FrontendQamAnnex_eC,
    NEXUS_FrontendQamAnnex_eMax
} NEXUS_FrontendQamAnnex;

/***************************************************************************
Summary:
Determines if QAM spectrum mode.
***************************************************************************/
typedef enum NEXUS_FrontendQamSpectrumMode
{
    NEXUS_FrontendQamSpectrumMode_eAuto,
    NEXUS_FrontendQamSpectrumMode_eManual,
    NEXUS_FrontendQamSpectrumMode_eMax
} NEXUS_FrontendQamSpectrumMode;

/***************************************************************************
Summary:
Determines if QAM spectral inversion is enabled or not
***************************************************************************/
typedef enum NEXUS_FrontendQamSpectralInversion
{
    NEXUS_FrontendQamSpectralInversion_eNormal,
    NEXUS_FrontendQamSpectralInversion_eInverted,
    NEXUS_FrontendQamSpectralInversion_eMax
} NEXUS_FrontendQamSpectralInversion;

/***************************************************************************
Summary:
Determines the acquisition type.
***************************************************************************/
typedef enum NEXUS_FrontendQamAcquisitionMode
{
    NEXUS_FrontendQamAcquisitionMode_eAuto,
    NEXUS_FrontendQamAcquisitionMode_eFast,
    NEXUS_FrontendQamAcquisitionMode_eSlow,
    NEXUS_FrontendQamAcquisitionMode_eScan,
    NEXUS_FrontendQamAcquisitionMode_eMax
} NEXUS_FrontendQamAcquisitionMode;

/***************************************************************************
The NEXUS_FrontendQamBandwidth enum has been deprecated. It is an unsigned integer in units of Hz now.
The following #defines are for backward compatibility only.
***************************************************************************/
#define NEXUS_FrontendQamBandwidth unsigned
#define NEXUS_FrontendQamBandwidth_e5Mhz 5000000
#define NEXUS_FrontendQamBandwidth_e6Mhz 6000000
#define NEXUS_FrontendQamBandwidth_e7Mhz 7000000
#define NEXUS_FrontendQamBandwidth_e8Mhz 8000000
#define NEXUS_FrontendQamBandwidth_eMax  4

/***************************************************************************
The NEXUS_FrontendQamFrequencyOffset enum has been deprecated. It is an unsigned integer in units of Hz now.
The following #defines are for backward compatibility only.
***************************************************************************/
#define NEXUS_FrontendQamFrequencyOffset unsigned
#define NEXUS_FrontendQamFrequencyOffset_e125Khz 125000
#define NEXUS_FrontendQamFrequencyOffset_e180Khz 180000 /* Applies only to Annex_B. */
#define NEXUS_FrontendQamFrequencyOffset_e200Khz 200000 /* Applies only to Annex_A. */
#define NEXUS_FrontendQamFrequencyOffset_e250Khz 250000
#define NEXUS_FrontendQamFrequencyOffset_eMax 4

/***************************************************************************
Summary:
QAM tuning parameters
***************************************************************************/
typedef struct NEXUS_FrontendQamSettings
{
    NEXUS_FrontendQamMode mode;
    NEXUS_FrontendQamAnnex annex;
    unsigned frequency;                 /* In Hz */
    unsigned ifFrequency;               /* IF Frequency in Hz */
    unsigned symbolRate;                /* In Baud. The default value is 0, which causes Nexus to select default symbol rates based on the QamMode. */
    bool fastAcquisition;               /* True=fast acquisition mode, false=default */
    bool terrestrial;                   /* Receiver mode: true=terrestrial, false=cable */
    NEXUS_CallbackDesc lockCallback;    /* Lock changed callback. This is called when demod locks or unlocks. You must get status to determine lock status.
                                           After tuning to a new frequency, it is normal to get two callbacks: one because the previous freq was unlocked
                                           and a second when the new freq is locked. The first callback and its unlocked status does not mean that the new
                                           freq will not be acquired later. Because callbacks can be collapsed under system load, an application should not 
                                           count callbacks. Instead, it should only rely on what get status returns. See tune_qam.c for an example algorithm. */
    bool autoAcquire;                   /* True = Enable, False = Disable. */
    unsigned bandwidth;                 /* Tuner frequency bandwidth in Hz */
    bool enablePowerMeasurement;        /* True = enables calculation of downstream channel power for some frontends like 31xx. For others it is always enabled. False = Disable. */
    NEXUS_FrontendQamSpectrumMode spectrumMode; /* Spectrum mode. */
    NEXUS_FrontendQamSpectralInversion spectralInversion; /* Spectral inversion. */
    unsigned frequencyOffset; /* Automatic Frequency offset range from the tuned frequency in Hz. */
    bool enableNullPackets; /* Enables/disables improved locking mechanism for Annex_A signals containing >98% null packets.*/
    NEXUS_CallbackDesc asyncStatusReadyCallback;   /* Callback will be called when the async qam status is ready. */
    NEXUS_FrontendQamAcquisitionMode acquisitionMode; /* Acquisition mode. */
    struct {
        unsigned    upperBaudSearch;    /* Upper baud search range in Hz. Applicable only in scan mode. */ 
        unsigned    lowerBaudSearch;    /*Lower baud search range in Hz. Applicable only in scan mode. */
        bool        mode[NEXUS_FrontendQamAnnex_eMax][NEXUS_FrontendQamMode_eMax]; /* Enable/Disable QAM mode search in scan mode. */
        unsigned    frequencyOffset;    /* in NEXUS_FrontendQamAcquisitionMode_eAuto, frontend uses non-scan frequencyOffset, then uses the scan.frequencyOffset. */
    } scan; /* only applies if NEXUS_FrontendQamAcquisitionMode_eScan (or eAuto mode when it internally goes into eScan mode) */
} NEXUS_FrontendQamSettings;

/***************************************************************************
Summary:
    Initialize a QAM settings structure to defaults
***************************************************************************/
void NEXUS_Frontend_GetDefaultQamSettings(
    NEXUS_FrontendQamSettings *pSettings
    );

/***************************************************************************
Summary:
    Tune to a QAM channel
***************************************************************************/
NEXUS_Error NEXUS_Frontend_TuneQam(
    NEXUS_FrontendHandle handle,
    const NEXUS_FrontendQamSettings *pSettings
    );

/***************************************************************************
Summary:
QAM tuning status
***************************************************************************/
typedef struct NEXUS_FrontendQamStatus
{
    NEXUS_FrontendQamSettings settings; /* Settings provided at last call to NEXUS_Frontend_TuneQam */

    bool receiverLock;          /* Do we have QAM lock? */
    bool fecLock;               /* Is the FEC locked? */
    bool opllLock;              /* Is the output PLL locked? */
    bool spectrumInverted;      /* Is the spectrum inverted? */

    unsigned symbolRate;        /* Baud. received symbol rate (in-band) */
    int      symbolRateError;   /* symbol rate error in Baud */

    int berEstimate;            /* deprecated */

    unsigned ifAgcLevel;        /* IF AGC level in units of 1/10 percent */
    unsigned rfAgcLevel;        /* tuner AGC level in units of 1/10 percent */
    unsigned intAgcLevel;       /* Internal AGC level in units of 1/10 percent */
    unsigned snrEstimate;       /* snr estimate in 1/100 dB (in-Band). */

    unsigned fecCorrected;      /* FEC corrected block count, accumulated since tune or NEXUS_Frontent_ResetStatus */
    unsigned fecUncorrected;    /* FEC uncorrected block count, accumulated since tune or NEXUS_Frontent_ResetStatus */
    unsigned fecClean;          /* FEC clean block count, accumulated since tune or NEXUS_Frontent_ResetStatus */
    unsigned bitErrCorrected;   /* deprecated: cumulative bit correctable errors. same as viterbiUncorrectedBits. */
    unsigned reacquireCount;    /* cumulative reacquisition count */

    unsigned viterbiUncorrectedBits; /* uncorrected error bits output from Viterbi, accumulated since tune or NEXUS_Frontent_ResetStatus */
    unsigned viterbiTotalBits;       /* total number of bits output from Viterbi, accumulated since tune or NEXUS_Frontent_ResetStatus */
    uint32_t viterbiErrorRate;       /* viterbi bit error rate (BER) in 1/2147483648 th units. This is pre-reed-solomon.
                                        This is the ratio of uncorrected bits / total bits since the last GetQamStatus call.
                                        Convert to floating point by dividing by 2147483648.0 */

    int      carrierFreqOffset; /* carrier frequency offset in 1/1000 Hz */
    int      carrierPhaseOffset;/* carrier phase offset in 1/1000 Hz */

    unsigned goodRsBlockCount;  /* reset on every read */
    unsigned berRawCount;       /* reset on every read */

    int      dsChannelPower;    /* units of 1/10 dBmV */
    unsigned mainTap;           /* Channel main tap coefficient */
    unsigned postRsBer;         /* post reed-solomon bit error rate. Same units and behavior as viterbiErrorRate. */
    unsigned postRsBerElapsedTime; /* time used in postRsBer calculation. In units of milliseconds. */
    uint16_t interleaveDepth;   /* Used in DOCSIS */
    unsigned lnaAgcLevel;       /* LNA AGC level in units of 1/10 percent */
    
    int equalizerGain;          /* Channel equalizer gain value in dB */
    int frontendGain;           /* in 1/100th of a dB. Is the accumulated gain of the tuner/ufe/wfe. */
    int digitalAgcGain;         /* in 1/100th of a dB. Is the AGC gain in the demod core. */
    int highResEqualizerGain;   /* in 1/100th of a dB. Is the equalizer gain in the demod core. higher resolution than previous equalizerGain. */
} NEXUS_FrontendQamStatus;

/***************************************************************************
Summary:
    Get the synchronous status of a QAM tuner
***************************************************************************/
NEXUS_Error NEXUS_Frontend_GetQamStatus(
    NEXUS_FrontendHandle handle,
    NEXUS_FrontendQamStatus *pStatus /* [out] */
    );

/***************************************************************************
Summary:
    Request the asynchronous status of a QAM tuner
***************************************************************************/
NEXUS_Error NEXUS_Frontend_RequestQamAsyncStatus(
    NEXUS_FrontendHandle handle
    );

/***************************************************************************
Summary:
    Get the asynchronous status of a QAM tuner
***************************************************************************/
NEXUS_Error NEXUS_Frontend_GetQamAsyncStatus(
    NEXUS_FrontendHandle handle,
    NEXUS_FrontendQamStatus *pStatus /* [out] */
    );

typedef enum NEXUS_FrontendQamAcquisitionStatus
{
    NEXUS_FrontendQamAcquisitionStatus_eNoSignal,
    NEXUS_FrontendQamAcquisitionStatus_eNoLock,
    NEXUS_FrontendQamAcquisitionStatus_eLockedFast,
    NEXUS_FrontendQamAcquisitionStatus_eLockedSlow,
    NEXUS_FrontendQamAcquisitionStatus_eMax
}NEXUS_FrontendQamAcquisitionStatus;

typedef enum NEXUS_FrontendQamInterleaver
{
    NEXUS_FrontendQamInterleaver_eI128_J1,
    NEXUS_FrontendQamInterleaver_eI128_J2,
    NEXUS_FrontendQamInterleaver_eI128_J3,
    NEXUS_FrontendQamInterleaver_eI128_J4,
    NEXUS_FrontendQamInterleaver_eI64_J2,
    NEXUS_FrontendQamInterleaver_eI32_J4,
    NEXUS_FrontendQamInterleaver_eI16_J8,
    NEXUS_FrontendQamInterleaver_eI8_J16,
    NEXUS_FrontendQamInterleaver_eI4_J32,
    NEXUS_FrontendQamInterleaver_eI2_J64,
    NEXUS_FrontendQamInterleaver_eI1_J128,
    NEXUS_FrontendQamInterleaver_eI12_J17,
    NEXUS_FrontendQamInterleaver_eUnsupported,
    NEXUS_FrontendQamInterleaver_eMax
}NEXUS_FrontendQamInterleaver;

/***************************************************************************
Summary:
QAM scan status
***************************************************************************/
typedef struct NEXUS_FrontendQamScanStatus
{
    unsigned symbolRate;                      /* Detected symbol rate. */
    int frequencyOffset;                      /* Detected carrier frequency offset.  */
    NEXUS_FrontendQamMode mode;               /* Detected QAM mode.  */
    NEXUS_FrontendQamAnnex annex;             /* Detected annex. */
    NEXUS_FrontendQamInterleaver interleaver; /* Detected interleaver. */
    NEXUS_FrontendQamSpectralInversion spectrumInverted;  /* Is spectrum inverted? */
    NEXUS_FrontendQamAcquisitionStatus acquisitionStatus; /* Acquisition status. */
} NEXUS_FrontendQamScanStatus;


/***************************************************************************
Summary:
    Get the scan status of a QAM tuner
Description:
    For scan mode (NEXUS_Frontend_TuneQam with mode set to NEXUS_FrontendQamAcquisitionType_eScan),
    initially NEXUS_Frontend_GetFastStatus can be used to determine the lock status faster.
    Once locked, NEXUS_Frontend_GetQamScanStatus can be used to get the complete scan status.
    This way the scan time can be improved vs calling NEXUS_Frontend_GetQamScanStatus everytime.
***************************************************************************/
NEXUS_Error NEXUS_Frontend_GetQamScanStatus(
    NEXUS_FrontendHandle handle,
    NEXUS_FrontendQamScanStatus *pScanStatus /* [out] */
    );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef NEXUS_FRONTEND_QAM_H__ */

