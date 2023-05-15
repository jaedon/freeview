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
* $brcm_Workfile: nexus_frontend_oob.h $
* $brcm_Revision: 19 $
* $brcm_Date: 5/7/12 11:59a $
*
* API Description:
*   API name: Frontend OutOfBand
*    Generic APIs for Out-of-band tuning.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/frontend/common/include/nexus_frontend_oob.h $
* 
* 19   5/7/12 11:59a erickson
* SW3128-149: update postRsBer comments
* 
* 18   5/4/12 2:40p erickson
* SW3128-149: add acquisitionTime and totalAcquisitionTime
* 
* 17   4/9/12 12:16p jtna
* SW3128-131: added NEXUS_FrontendOutOfBandOutputMode to
*  NEXUS_3128ConfigSettings
* 
* 16   3/9/12 11:30a erickson
* SW3128-110, SW3128-120: refactor highResEqualizerGain to match other
*  demods
* 
* 15   2/24/12 2:11p erickson
* SW3128-110, SW3128-120: add frontend gain variables, add nyquist filter
*  selection type
* 
* 14   2/7/12 2:30p erickson
* SW3461-133: standardize signal strength to units of 1/10 dBmV
* 
* 13   7/14/11 8:46a erickson
* SW7125-772: add status
* 
* 12   5/11/11 10:58a erickson
* SW3128-10: add asyncStatusReadyCallback
* 
* 11   4/15/11 4:30p erickson
* SW7422-413: add NEXUS_FrontendOutOfBandBertSource
* 
* 10   10/28/10 4:06p mphillip
* SW7420-1020: Add asynchronous status support to oob frontends
* 
* 9   9/8/10 2:23p mphillip
* SW7400-2900: Add power and ber support to oob tuner
* 
* 8   8/5/10 4:32p erickson
* SW7405-4722: Add support for "Frequency offset" and "Null packets" for
*  ADS and expose "auto acquire" in nexus for AOB
* 
* 7   6/26/09 1:58p erickson
* PR56288: add NEXUS_FrontendOutOfBandSettings.openDrain
* 
* 6   3/5/09 10:42a jgarrett
* PR 52817: Correcting SNR units
* 
* 5   9/23/08 9:16a jgarrett
* PR 46513: Merging 97459 support to mainline
* 
* PR44830/1   7/23/08 2:47p anilmm
* PR44830: Add OOB frequency
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
* Nexus_Devel/2   10/5/07 5:51p jgarrett
* PR 35551: successful build
* 
* Nexus_Devel/1   10/4/07 2:41p jgarrett
* PR 35551: Adding initial version
* 
***************************************************************************/
#ifndef NEXUS_FRONTEND_OOB_H__
#define NEXUS_FRONTEND_OOB_H__

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
Summary:
Out of band tuning modes
***************************************************************************/
typedef enum NEXUS_FrontendOutOfBandMode
{
    NEXUS_FrontendOutOfBandMode_eAnnexAQpsk,
    NEXUS_FrontendOutOfBandMode_eDvs167Qpsk = NEXUS_FrontendOutOfBandMode_eAnnexAQpsk,
    NEXUS_FrontendOutOfBandMode_eDvs178Qpsk,
    NEXUS_FrontendOutOfBandMode_ePod_AnnexAQpsk,
    NEXUS_FrontendOutOfBandMode_ePod_Dvs167Qpsk = NEXUS_FrontendOutOfBandMode_ePod_AnnexAQpsk,
    NEXUS_FrontendOutOfBandMode_ePod_Dvs178Qpsk,
    NEXUS_FrontendOutOfBandMode_eMax
} NEXUS_FrontendOutOfBandMode;

/***************************************************************************
Summary:
Spectrum inversion control
***************************************************************************/
typedef enum NEXUS_FrontendOutOfBandSpectrum
{
    NEXUS_FrontendOutOfBandSpectrum_eAuto,
    NEXUS_FrontendOutOfBandSpectrum_eNonInverted,
    NEXUS_FrontendOutOfBandSpectrum_eInverted,
    NEXUS_FrontendOutOfBandSpectrum_eMax
} NEXUS_FrontendOutOfBandSpectrum;

/***************************************************************************
Summary:
    Enumeration for BERT Input Source
****************************************************************************/
typedef enum NEXUS_FrontendOutOfBandBertSource
{
    NEXUS_FrontendOutOfBandBertSource_eIChOutput, /* BERT input source is the receiver I-channel output */
    NEXUS_FrontendOutOfBandBertSource_eQChOutput, /* BERT input source is the receiver Q-channel output */
    NEXUS_FrontendOutOfBandBertSource_eIQChOutputInterleave, /* BERT input source is the receiver I-channel and Q-channel output interleaved */
    NEXUS_FrontendOutOfBandBertSource_eFecOutput, /* BERT input source is the FEC output */
    NEXUS_FrontendOutOfBandBertSource_eMax
} NEXUS_FrontendOutOfBandBertSource;

/***************************************************************************
Summary:
    Enumeration for the kind of Nyquist Filter for out of band.
****************************************************************************/
typedef enum NEXUS_FrontendOutOfBandNyquistFilter
{           
    NEXUS_NyquistFilter_eRaisedCosine50,      /* Raised Cosine Nyquist Filter with 50% Roll Off. Applicable only to Annex-B. */
    NEXUS_NyquistFilter_eRootRaisedCosine50,  /* Root Raised Cosine Nyquist Filter with 50% Roll Off. Applicable only to Annex-B */
    NEXUS_NyquistFilter_eRootRaisedCosine30,  /* Root Raised Cosine Nyquist Filter with 30% Roll Off. Applicable only to Annex-A */
    NEXUS_NyquistFilter_eMax    
} NEXUS_FrontendOutOfBandNyquistFilter;

/***************************************************************************
Summary:
    Enumeration for OOB output modes
****************************************************************************/
typedef enum NEXUS_FrontendOutOfBandOutputMode
{
    NEXUS_FrontendOutOfBandOutputMode_eDifferentialDecoder, /* pre-FEC Transport Stream sent to Cable Card */
    NEXUS_FrontendOutOfBandOutputMode_eFec,                 /* post FEC transport sent to the backend chip */
    NEXUS_FrontendOutOfBandOutputMode_eMax
} NEXUS_FrontendOutOfBandOutputMode;

/***************************************************************************
Summary:
    Out-of-band tuning parameters
***************************************************************************/
typedef struct NEXUS_FrontendOutOfBandSettings
{
    NEXUS_FrontendOutOfBandMode mode;
    unsigned frequency;
    NEXUS_FrontendOutOfBandSpectrum spectrum;
    unsigned symbolRate;                        /* Symbol Rate in Baud */
    NEXUS_CallbackDesc lockCallback;            /* Lock status changed callback */
    bool openDrain;                             /* true = open drain, false = normal mode; not supported for 3255. */
    bool autoAcquire;
    NEXUS_FrontendOutOfBandBertSource bertSource;          /*BERT input source. */
    NEXUS_CallbackDesc asyncStatusReadyCallback;   /* Callback will be called when the async out of band status is ready. */
} NEXUS_FrontendOutOfBandSettings;

/***************************************************************************
Summary:
    Initialize an out-of-band settings structure to defaults
***************************************************************************/
void NEXUS_Frontend_GetDefaultOutOfBandSettings(
    NEXUS_FrontendOutOfBandSettings *pSettings
    );

/***************************************************************************
Summary:
    Tune a frontend to an out-of-band channel
***************************************************************************/
NEXUS_Error NEXUS_Frontend_TuneOutOfBand(
    NEXUS_FrontendHandle handle,
    const NEXUS_FrontendOutOfBandSettings *pSettings
    );

/***************************************************************************
Summary:
Out-of-band tuning status
***************************************************************************/
typedef struct NEXUS_FrontendOutOfBandStatus
{
    NEXUS_FrontendOutOfBandSettings settings; /* Settings provided at last call to NEXUS_Frontend_TuneOutOfBand */    

    NEXUS_FrontendOutOfBandMode mode;
    NEXUS_FrontendOutOfBandSpectrum spectrum;
    unsigned symbolRate;                      /* Symbol Rate in Baud */
    unsigned ifFreq;                          /* in Hertz, IF freq. */
    unsigned loFreq;                          /* in Hertz, LO freq. */
    unsigned sysXtalFreq;                     /* in Hertz, Sys. Xtal freq. */
    bool isFecLocked;
    bool isQamLocked;
    int snrEstimate;                          /* in 1/100 db */
    int agcIntLevel;                          /* in 1/10 percent */
    int agcExtLevel;                          /* in 1/10 percent */
    int carrierFreqOffset;                    /* in 1/1000 Hz */
    int carrierPhaseOffset;                   /* in 1/1000 Hz */
    unsigned uncorrectedCount;                /* not self-clearing  */
    unsigned correctedCount;                  /* not self-clearing*/
    unsigned berErrorCount;                   /* not self-clearing */
    unsigned postRsBer;                       /* Post Reed Solomon bit error rate (BER) in 1/2147483648 th units. This is accumulated bit error rate since the last GetOutOfBandStatus call.
                                                                                    Convert to floating point by dividing by 2147483648.0 */
    unsigned postRsBerElapsedTime;            /* postRsBer over this time. In units of seconds. */
    int fdcChannelPower;                      /* units of 1/10 dBmV */
    int frontendGain;                         /* in 1/100th of a dB. Is the accumulated gain of the tuner/ufe/wfe. */
    int digitalAgcGain;                       /* in 1/100th of a dB. Is the AGC gain in the demod core. */
    int highResEqualizerGain;                 /* in 1/100th of a dB. Is the equalizer gain in the demod core. */
    unsigned acquisitionTime;                 /* time in mS for the last acquisition */
    unsigned totalAcquisitionTime;            /* time in mS for all acquisitions to acquire the signal. This value can be different from acquisitionTime as the number of tries to acquire can be more than one in case of autoAcquire. */
} NEXUS_FrontendOutOfBandStatus;

/***************************************************************************
Summary:
    Get the out-of-band status of a frontend
***************************************************************************/
NEXUS_Error NEXUS_Frontend_GetOutOfBandStatus(
    NEXUS_FrontendHandle handle,
    NEXUS_FrontendOutOfBandStatus *pStatus /* [out] */
    );

/***************************************************************************
Summary:
    Request the asynchronous status of an OutOfBand frontend
***************************************************************************/
NEXUS_Error NEXUS_Frontend_RequestOutOfBandAsyncStatus(
    NEXUS_FrontendHandle handle
    );

/***************************************************************************
Summary:
    Get the asynchronous status of an OutOfBand frontend
***************************************************************************/
NEXUS_Error NEXUS_Frontend_GetOutOfBandAsyncStatus(
    NEXUS_FrontendHandle handle,
    NEXUS_FrontendOutOfBandStatus *pStatus /* [out] */
    );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef NEXUS_FRONTEND_OOB_H__ */

