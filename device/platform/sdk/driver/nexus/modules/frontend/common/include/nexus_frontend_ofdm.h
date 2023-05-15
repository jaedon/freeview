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
* $brcm_Workfile: nexus_frontend_ofdm.h $
* $brcm_Revision: 41 $
* $brcm_Date: 12/6/12 11:54a $
*
* API Description:
*   API name: Frontend OFDM
*    Generic APIs for OFDM (Orthogonal Frequency-Division Multiplexing) tuning.
*    This is used in DVB-H and DVB-T environments.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/frontend/common/include/nexus_frontend_ofdm.h $
* 
* 41   12/6/12 11:54a erickson
* SW3472-23: Add SSI and SQI for each "Layer" in HAB ISDBT Status Message
* 
* 40   6/8/12 3:24p erickson
* SW7552-276: add NEXUS_FrontendOfdmModeGuard
* 
* 39   5/7/12 3:25p erickson
* SW3462-12: Enable DVB-C2
* 
* 38   2/7/12 2:30p erickson
* SW3461-133: standardize signal strength to units of 1/10 dBmV
* 
* 37   2/2/12 11:42a erickson
* SW3461-133: fix units
* 
* 36   1/26/12 8:52a erickson
* SW3461-133: add signalLevelPercent and signalQualityPercent
* 
* 35   11/16/11 11:17a erickson
* SW3461-85: add NEXUS_FrontendOfdmTransmissionMode_e1k
* 
* 34   9/26/11 1:55p mphillip
* SW3461-53: Merge changes from branch to main
* 
* SW3461-36/5   9/9/11 3:09p vishk
* SW3461-14: Signal strength returns 0
* 
* SW3461-36/4   9/9/11 2:56p vishk
* SW3461-25: New API required to support streams with multiple PLP's
* 
* SW3461-36/3   9/7/11 8:31a vishk
* SW3461-48: Fix 3461 frontend issues. Fix plpMode usage.
* 
* 32   8/19/11 4:47p mphillip
* SW3461-36: Temporary fix until the merge from the branch SW3461-36 is
*  done
* 
* 31   6/6/11 2:22p mphillip
* SW7231-32: Add 3461 support
* 
* 30   5/26/11 4:36p erickson
* SW7231-32: rename asyncStatusReadyCallback
* 
* 29   5/24/11 6:57p mphillip
* SW7231-32: Add ofdm async status support functions
* 
* 28   5/24/11 5:10p erickson
* SW7231-32: reorder settings, add dvbt2Settings
* 
* 27   12/17/10 4:50p jgarrett
* SW35230-2405: Adding spectral mode
* 
* 26   9/28/10 11:40a erickson
* SW7550-584: merge
*
* SW7550-584/1   9/27/10 11:46a pinglong
* SW7550-584: add blocks counters to ISDB-T three layers status for post
*  RS BER
*
* 25   9/13/10 12:18p erickson
* SW35230-1306: deprecate NEXUS_FrontendXxxBandwidth. use unsigned
*  integer in units of Hz instead.
*
* 24   7/8/10 5:38p jhaberf
* SW35230-562: Added ISDB-T demodulation to OFDM modulation enum
*
* 23   6/18/10 2:02p erickson
* SW7405-4511: add DVB-T2 types
*
* 22   6/4/10 11:29a erickson
* SW7550-447: merge
*
* SW7550-447/1   5/26/10 1:38p farshidf
* SW7550-447: exposed the pre RS correcttion at Nexus Level for all
*  layers ISDB-T mode
*
* 21   5/18/10 6:05p jgarrett
* SW7550-399: Adding option for cable/terrestrial setup in OFDM
*
* SW7550-399/1   5/10/10 2:58p farshidf
* SW7550-38: add the Cable or Ter tuner setting
*
* 20   3/2/10 5:18p jgarrett
* SW7550-290: Adding acquisition mode for OFDM tuning
*
* THD_7550_Auto/2   2/3/10 6:34p farshidf
* SW7550-38: add the scan mode
*
* THD_7550_Auto/1   2/1/10 6:04p farshidf
* SW7550-38: initinal ISDB-T channel scan
*
* 19   11/18/09 9:43a jgarrett
* SW7550-48: Adding nexus hooks for tuner and platform frontend
*  initialization
*
* 18   11/6/09 5:06p jgarrett
* SW7550-48: Adding ISDB-T and EWS
*
* 17   9/25/09 10:48a jgarrett
* SW7405-2945: Changing type for signalStrength
*
* 16   7/20/09 12:46p erickson
* PR56816: improve NEXUS_FrontendOfdmStatus comments
*
* 15   4/28/09 10:03a erickson
* PR54218: add NEXUS_FrontendOfdmStatus.rsPerDelta and .vitBerDelta
*
* 14   3/18/09 2:09p jgarrett
* PR 53095: Adding cell id
*
* 13   3/10/09 6:23p jgarrett
* PR 52629: Adding spectrumInverted to status
*
* 12   11/12/08 1:40p jgarrett
* PR 48626: Adding Pre/Post Viterbi BER
*
* 11   9/17/08 3:33p jgarrett
* PR 47011: Adding no signal detected flag
*
* 10   7/31/08 11:03a jrubio
* PR45002: fix comma at end of list
*
* 9   7/30/08 12:50p jgarrett
* PR 45002: Adding TPS settings
*
* 8   7/21/08 2:18p jgarrett
* PR 44997: Adding IF/RF AGC levels
*
* 7   7/21/08 2:06p jgarrett
* PR 44998: Converting some status values to enums
*
* 6   4/28/08 2:21p arbisman
* PR40927: Add modulation, transmission mode and guard interval
*
* 5   2/28/08 11:08a arbisman
* PR39100: Added extra status fields from THD
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
* Nexus_Devel/2   1/10/08 3:50p jgarrett
* PR 38653: Adding priority
*
* Nexus_Devel/1   1/10/08 2:33p jgarrett
* PR 38653: Initial version
*
***************************************************************************/
#ifndef NEXUS_FRONTEND_OFDM_H__
#define NEXUS_FRONTEND_OFDM_H__

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
Summary:
    Enumeration for receiver mode
****************************************************************************/
typedef enum NEXUS_FrontendOfdmMode
{
    NEXUS_FrontendOfdmMode_eDvbt,       /* DVB-T */
    NEXUS_FrontendOfdmMode_eDvbt2,      /* DVB-T2 */
    NEXUS_FrontendOfdmMode_eDvbc2,      /* DVB-C2 */
    NEXUS_FrontendOfdmMode_eIsdbt,      /* ISDB-T */
    NEXUS_FrontendOfdmMode_eMax
} NEXUS_FrontendOfdmMode;

/***************************************************************************
Summary:
    Enumeration for receiver CCI (CoChannel Interference Filter) Mode
****************************************************************************/
typedef enum NEXUS_FrontendOfdmCciMode
{
    NEXUS_FrontendOfdmCciMode_eNone,    /* No CCI selected */
    NEXUS_FrontendOfdmCciMode_eAuto,    /* Auto Selection */
    NEXUS_FrontendOfdmCciMode_eMax
} NEXUS_FrontendOfdmCciMode;

/***************************************************************************
The NEXUS_FrontendOfdmBandwidth enum has been deprecated. It is an unsigned integer in units of Hz now.
The following #defines are for backward compatibility only.
***************************************************************************/
#define NEXUS_FrontendOfdmBandwidth_e5Mhz 5000000
#define NEXUS_FrontendOfdmBandwidth_e6Mhz 6000000
#define NEXUS_FrontendOfdmBandwidth_e7Mhz 7000000
#define NEXUS_FrontendOfdmBandwidth_e8Mhz 8000000
#define NEXUS_FrontendOfdmBandwidth_eMax  4

/***************************************************************************
Summary:
    Enumeration for stream priority

Description:
    DVB-T can hierarchically encode streams.  A low-priority stream can
    be embedded within a high priority stream.  Receivers with good reception
    conditions may be able to receive both streams, but receivers with poor
    reception conditions may only be able to receive the high priority stream.

****************************************************************************/
typedef enum NEXUS_FrontendOfdmPriority
{
    NEXUS_FrontendOfdmPriority_eHigh,
    NEXUS_FrontendOfdmPriority_eLow,
    NEXUS_FrontendOfdmPriority_eMax
} NEXUS_FrontendOfdmPriority;

/***************************************************************************
Summary:
    OFDM Code Rate
****************************************************************************/
typedef enum NEXUS_FrontendOfdmCodeRate
{
    NEXUS_FrontendOfdmCodeRate_e1_2,      /* Rate 1/2 */
    NEXUS_FrontendOfdmCodeRate_e2_3,      /* Rate 2/3 */
    NEXUS_FrontendOfdmCodeRate_e3_4,      /* Rate 3/4 */
    NEXUS_FrontendOfdmCodeRate_e3_5,      /* Rate 3/5 DVB-T2*/
    NEXUS_FrontendOfdmCodeRate_e4_5,      /* Rate 4/5 DVB-T2*/
    NEXUS_FrontendOfdmCodeRate_e5_6,      /* Rate 5/6 */
    NEXUS_FrontendOfdmCodeRate_e7_8,      /* Rate 7/8 */
    NEXUS_FrontendOfdmCodeRate_eMax
}NEXUS_FrontendOfdmCodeRate;

/***************************************************************************
Summary:
    OFDM Hierarchy
****************************************************************************/
typedef enum NEXUS_FrontendOfdmHierarchy
{
    NEXUS_FrontendOfdmHierarchy_e0,        /* Select Hierarchy 0 */
    NEXUS_FrontendOfdmHierarchy_e1,        /* Select Hierarchy 1 */
    NEXUS_FrontendOfdmHierarchy_e2,        /* Select Hierarchy 2 */
    NEXUS_FrontendOfdmHierarchy_e4,        /* Select Hierarchy 4 */
    NEXUS_FrontendOfdmHierarchy_eMax
}NEXUS_FrontendOfdmHierarchy;

/***************************************************************************
Summary:
    OFDM ISDB-T Time Interleaving Mode
****************************************************************************/
typedef enum NEXUS_FrontendOfdmTimeInterleaving
{
    NEXUS_FrontendOfdmTimeInterleaving_e0x,
    NEXUS_FrontendOfdmTimeInterleaving_e1x,
    NEXUS_FrontendOfdmTimeInterleaving_e2x,
    NEXUS_FrontendOfdmTimeInterleaving_e3x,
    NEXUS_FrontendOfdmTimeInterleaving_eMax
} NEXUS_FrontendOfdmTimeInterleaving;

/***************************************************************************
Summary:
    OFDM modulation mode
****************************************************************************/
typedef enum NEXUS_FrontendOfdmModulation
{
    NEXUS_FrontendOfdmModulation_eQpsk,
    NEXUS_FrontendOfdmModulation_eQam16,
    NEXUS_FrontendOfdmModulation_eQam64,
    NEXUS_FrontendOfdmModulation_eQam256, /* DVB-T2 */
    NEXUS_FrontendOfdmModulation_eDqpsk, /* ISDB-T */
    NEXUS_FrontendOfdmModulation_eMax
}NEXUS_FrontendOfdmModulation;


/***************************************************************************
Summary:
    OFDM modulation mode
****************************************************************************/
typedef enum NEXUS_FrontendOfdmAcquisitionMode
{
    NEXUS_FrontendOfdmAcquisitionMode_eAuto,        /* Default.  Automatic Re-acquisition. */
    NEXUS_FrontendOfdmAcquisitionMode_eManual,      /* Manual Re-acquisition, app must re-acquire if signal is lost. */
    NEXUS_FrontendOfdmAcquisitionMode_eScan,        /* Auto-scan acquisition.  Will re-acquire if signal strength is considered sufficient. */
    NEXUS_FrontendOfdmAcquisitionMode_eMax
}NEXUS_FrontendOfdmAcquisitionMode;

/***************************************************************************
Summary:
    OFDM transmission mode
****************************************************************************/
typedef enum NEXUS_FrontendOfdmTransmissionMode
{
    NEXUS_FrontendOfdmTransmissionMode_e1k,     /* DVB-T2 */
    NEXUS_FrontendOfdmTransmissionMode_e2k,
    NEXUS_FrontendOfdmTransmissionMode_e4k,
    NEXUS_FrontendOfdmTransmissionMode_e8k,
    NEXUS_FrontendOfdmTransmissionMode_e16k,    /* DVB-T2 */
    NEXUS_FrontendOfdmTransmissionMode_e32k,    /* DVB-T2 */
    NEXUS_FrontendOfdmTransmissionMode_eMax
}NEXUS_FrontendOfdmTransmissionMode;

/***************************************************************************
Summary:
    OFDM guard interval
****************************************************************************/
typedef enum NEXUS_FrontendOfdmGuardInterval
{
    NEXUS_FrontendOfdmGuardInterval_e1_32,  /* 1/32 Guard Interval */
    NEXUS_FrontendOfdmGuardInterval_e1_16,  /* 1/16 Guard Interval */
    NEXUS_FrontendOfdmGuardInterval_e1_8,   /* 1/8 Guard Interval */
    NEXUS_FrontendOfdmGuardInterval_e1_4,   /* 1/4 Guard Interval */
    NEXUS_FrontendOfdmGuardInterval_e1_128, /* 1/128 Guard Interval DVB-T2 */
    NEXUS_FrontendOfdmGuardInterval_e19_128,/* 19/128 Guard Interval DVB-T2 */
    NEXUS_FrontendOfdmGuardInterval_e19_256,/* 19/256 Guard Interval DVB-T2 */
    NEXUS_FrontendOfdmGuardInterval_eMax
}NEXUS_FrontendOfdmGuardInterval;

/***************************************************************************
Summary:
    OFDM Pull-In Range
****************************************************************************/
typedef enum NEXUS_FrontendOfdmPullInRange
{
    NEXUS_FrontendOfdmPullInRange_eNarrow,  /* Carrier acquisition range is narrow +/-200kHz */
    NEXUS_FrontendOfdmPullInRange_eWide,    /* carrier acquisition range is wide +/-600kHz */
    NEXUS_FrontendOfdmPullInRange_eMax
}NEXUS_FrontendOfdmPullInRange;

/*************************************************************************** 
Summary:
    OFDM spectral parameters
 ***************************************************************************/
typedef enum NEXUS_FrontendOfdmSpectrum
{
    NEXUS_FrontendOfdmSpectrum_eAuto,
    NEXUS_FrontendOfdmSpectrum_eNonInverted,
    NEXUS_FrontendOfdmSpectrum_eInverted,
    NEXUS_FrontendOfdmSpectrum_eMax
} NEXUS_FrontendOfdmSpectrum;

/*************************************************************************** 
Summary:
    OFDM spectral parameters
 ***************************************************************************/
typedef enum NEXUS_FrontendOfdmModeGuard
{
    NEXUS_FrontendOfdmModeGuard_eManual,
    NEXUS_FrontendOfdmModeGuard_eAutoDvbt,
    NEXUS_FrontendOfdmModeGuard_eAutoIsdbtJapan,
    NEXUS_FrontendOfdmModeGuard_eAutoIsdbtBrazil,
    NEXUS_FrontendOfdmModeGuard_eMax
} NEXUS_FrontendOfdmModeGuard;

/*************************************************************************** 
Summary:
    OFDM tuning parameters
 ***************************************************************************/
typedef struct NEXUS_FrontendOfdmSettings
{
    NEXUS_FrontendOfdmMode mode;
    unsigned bandwidth; /* in Hz */
    unsigned frequency;                     /* In Hz */
    bool terrestrial;                                  /* Receiver mode: true=terrestrial, false=cable */
    NEXUS_FrontendOfdmAcquisitionMode acquisitionMode; /* Acquisition Mode.  Default is auto, not supported on all platforms. */
    NEXUS_FrontendOfdmSpectrum spectrum;               /* Spectral mode.  Default is auto, not supported on all platforms. */
    NEXUS_CallbackDesc lockCallback;                   /* Callback will be called when lock status changes */
    NEXUS_CallbackDesc asyncStatusReadyCallback;       /* Callback will be called when the async ofdm status is ready. */
    
    
    struct
    {
       bool plpMode;    /* This bit controls whether the output  PLP is  manually or automatically selected.  When plpMode is false(manual mode), the user must supply the desired plpId.  */
       uint8_t plpId;   /* In single-PLP applications, this unsigned number specifies the desired PLP for output by the T2 receiver. 
                                              In multiple-PLP applications, this unsigned number specifies the ID for the data  (Type1 or Type2) PLP 
                                              and the common (Type0) PLP is automatically determined. */
    } dvbt2Settings;
    
    struct
    {
       bool plpMode;    /* This bit controls whether the output  PLP is  manually or automatically selected.  When plpMode is false(manual mode), the user must supply the desired plpId.  */
       uint8_t plpId;   /* In single-PLP applications, this unsigned number specifies the desired PLP for output by the C2 receiver. 
                            In multiple-PLP applications, this unsigned number specifies the ID for the data    (Type1 or Type2) PLP 
                            and the common (Type0) PLP is automatically determined. */
    } dvbc2Settings;    

    /* The parameters below are ISDB-T and DVB-T settings. Left as is for legacy reasons. */
    
    unsigned ifFrequency;                   /* In Hz */
    NEXUS_CallbackDesc ewsCallback;                    /* Callback will be called when the ISDB-T EWS (Emergency Warning System) status changes */
    NEXUS_FrontendOfdmCciMode cciMode;
    NEXUS_FrontendOfdmPriority priority;
    NEXUS_FrontendOfdmPullInRange pullInRange;

    bool manualTpsSettings;     /* Set to true if you want to manually specify TPS (Transmission Parameters Signalling) settings for DVB-T */
    struct
    {
        NEXUS_FrontendOfdmCodeRate highPriorityCodeRate;    /* High priority coderate to be used in acquisition.  Ignored if manualTpsSettings = false */
        NEXUS_FrontendOfdmCodeRate lowPriorityCodeRate;     /* Low priority coderate to be used in acquisition.  Ignored if manualTpsSettings = false */
        NEXUS_FrontendOfdmHierarchy hierarchy;              /* Hierarchy to be used in acquisition.  Ignored if manualTpsSettings = false */
        NEXUS_FrontendOfdmModulation modulation;            /* Modulation to be used in acquisition.  Ignored if manualTpsSettings = false */
    } tpsSettings;

    bool manualModeSettings;    /* Set to true, if you want to choose the auto mode particular to a region and standard, or to explicitly set transmission mode and guard interval. */
    struct
    {
		NEXUS_FrontendOfdmModeGuard modeGuard;		   /* Automatically selects a particular subset of transmission modes and guard intervals according to the standard and region. */
		                                               /* If you choose to explicitly set the transmission mode and guard interval, then set this to NEXUS_FrontendOfdmModeGuard_eManual. */
        NEXUS_FrontendOfdmTransmissionMode mode;            /* Transmission mode to be used in acquisition.  Ignored if manualModeSettings = false */
        NEXUS_FrontendOfdmGuardInterval guardInterval;      /* Guard interval to be used in acquisition.  Ignored if manualModeSettings = false */
    } modeSettings;

    bool manualTmccSettings;     /* Set to true if you want to manually specify TMCC (Transmission and Multiplexing Configuration Control) settings */
    struct
    {
        bool                               partialReception;        /* Manually set ISDB-T partial reception. */
        NEXUS_FrontendOfdmModulation       modulationA;             /* Manually set ISDB-T Layer A modulation type. */
        NEXUS_FrontendOfdmCodeRate         codeRateA;               /* Manually set ISDB-T Layer A code rate. */
        NEXUS_FrontendOfdmTimeInterleaving timeInterleavingA;       /* Manually set ISDB-T Layer A time interleaving.*/
        unsigned                           numSegmentsA;            /* Manually set ISDB-T Layer A number of segments. */
        NEXUS_FrontendOfdmModulation       modulationB;             /* Manually set ISDB-T Layer B modulation type. */
        NEXUS_FrontendOfdmCodeRate         codeRateB;               /* Manually set ISDB-T Layer B code rate. */
        NEXUS_FrontendOfdmTimeInterleaving timeInterleavingB;       /* Manually set ISDB-T Layer B time interleaving.*/
        unsigned                           numSegmentsB;            /* Manually set ISDB-T Layer B number of segments. */
        NEXUS_FrontendOfdmModulation       modulationC;             /* Manually set ISDB-T Layer C modulation type. */
        NEXUS_FrontendOfdmCodeRate         codeRateC;               /* Manually set ISDB-T Layer C code rate. */
        NEXUS_FrontendOfdmTimeInterleaving timeInterleavingC;       /* Manually set ISDB-T Layer C time interleaving.*/
        unsigned                           numSegmentsC;            /* Manually set ISDB-T Layer C number of segments. */
    } tmccSettings;
} NEXUS_FrontendOfdmSettings;

/***************************************************************************
Summary:
    Initialize an OFDM settings structure to defaults
 ***************************************************************************/
void NEXUS_Frontend_GetDefaultOfdmSettings(
    NEXUS_FrontendOfdmSettings *pSettings   /* [out] */
    );

/***************************************************************************
Summary:
    Tune to an OFDM channel
 ***************************************************************************/
NEXUS_Error NEXUS_Frontend_TuneOfdm(
    NEXUS_FrontendHandle handle,
    const NEXUS_FrontendOfdmSettings *pSettings
    );

/***************************************************************************
Summary:
    This enumeration indicates the types of the Tx input streams carried 
    within the current DVBT2 super-frame. 
    Applicable only to DVBT2 channel according to ETSI EN 302 755 standard.
****************************************************************************/
typedef enum NEXUS_FrontendOfdmStreamType
{
  NEXUS_FrontendOfdmStreamType_eTS, /* Transport Stream */
  NEXUS_FrontendOfdmStreamType_eGS, /* Generic Stream (GSE [i.2] and/or GFPS and/or GCS) but not TS */
  NEXUS_FrontendOfdmStreamType_eMixed /* Both TS and Generic Stream (i.e. TS and at least one of GSE, GFPS, GCS) */
} NEXUS_FrontendOfdmStreamType;

/***************************************************************************
Summary:
   This enumeration represents the PAPR reduction used. 
   Applicable only to DVBT2 channel according to ETSI EN 302 755 standard.
****************************************************************************/
typedef enum NEXUS_FrontendOfdmPapr
{
  NEXUS_FrontendOfdmPapr_eNone, /* L1-ACE is used and TR is used on P2 symbols only */
  NEXUS_FrontendOfdmPapr_eACE,  /* L1-ACE and ACE only are used */
  NEXUS_FrontendOfdmPapr_eTR,   /* L1-ACE and TR only are used */
  NEXUS_FrontendOfdmPapr_eBoth  /* L1-ACE, ACE and TR are used */
} NEXUS_FrontendOfdmPapr;

/***************************************************************************
Summary:
    This enumeration represents the constellation of the L1-post signalling 
    data block. Applicable only to DVBT2 channel according to ETSI EN 302 755 standard.
****************************************************************************/
typedef enum NEXUS_FrontendOfdmL1Mod
{
  NEXUS_FrontendOfdmL1Mod_eBpsk,  /* BPSK constellation */
  NEXUS_FrontendOfdmL1Mod_eQpsk,  /* QPSK constellation */
  NEXUS_FrontendOfdmL1Mod_e16Qam, /* 16-QAM constellation */
  NEXUS_FrontendOfdmL1Mod_e64Qam  /* 64-QAM constellation */
} NEXUS_FrontendOfdmL1Mod;

/***************************************************************************
Summary:
    This enumeration represents the coding of the L1-post signalling data block. 
    Applicable only to DVBT2 channel according to ETSI EN 302 755 standard.
****************************************************************************/
typedef enum NEXUS_FrontendOfdmL1CodeRate
{
  NEXUS_FrontendOfdmL1CodeRate_e1_2 /* code rate 1/2 */
} NEXUS_FrontendOfdmL1CodeRate;

/***************************************************************************
Summary:
    This enumeration represents type of the L1 FEC used for the L1-post 
    signalling data block. Applicable only to DVBT2 channel according to ETSI EN 302 755 standard.
****************************************************************************/
typedef enum NEXUS_FrontendOfdmFecType
{
  NEXUS_FrontendOfdmFecType_e16K, /* LDPC 16K */
  NEXUS_FrontendOfdmFecType_e64K  /* LDPC 64k */
} NEXUS_FrontendOfdmFecType;

/***************************************************************************
Summary:
    This enumeration represents the scattered pilot pattern used for the data
    OFDM symbols. Applicable only to DVBT2 channel according to ETSI EN 302 755 standard.
****************************************************************************/
typedef enum NEXUS_FrontendOfdmPilotPattern
{
  NEXUS_FrontendOfdmPilotPattern_e1, /* PP1 */
  NEXUS_FrontendOfdmPilotPattern_e2, /* PP2 */
  NEXUS_FrontendOfdmPilotPattern_e3, /* PP3 */
  NEXUS_FrontendOfdmPilotPattern_e4, /* PP4 */
  NEXUS_FrontendOfdmPilotPattern_e5, /* PP5 */
  NEXUS_FrontendOfdmPilotPattern_e6, /* PP6 */
  NEXUS_FrontendOfdmPilotPattern_e7, /* PP7 */
  NEXUS_FrontendOfdmPilotPattern_e8  /* PP8 */
} NEXUS_FrontendOfdmPilotPattern;

/***************************************************************************
Summary:
    This structure contains the complete L1 Pre status. 
    Applicable only to DVBT2 channel according to ETSI EN 302 755 standard.
****************************************************************************/
typedef struct NEXUS_FrontendOfdmL1PreStatus
{
    NEXUS_FrontendOfdmStreamType         streamType;         /* streamType */   
    bool                                 bwtExt;             /* This field indicates whether the extended carrier mode is used in the case of 8K, 16K and 32K FFT 
                                                                                                               sizes. When this field is set to '1', the extended carrier mode is used. If this field is set to '0', 
                                                                                                               the normal carrier mode is used */
    uint8_t                              s1;                 /* This field has the same value as in the P1 signalling. */   
    uint8_t                              s2;                 /* This field has the same value as in the P1 signalling. */   
    bool                                 l1RepetitionFlag;   /* This flag indicates whether the dynamic L1-post signalling is provided also for the next frame. */
    NEXUS_FrontendOfdmGuardInterval      guardInterval;      /* This field indicates the guard interval of the current super-frame */
    NEXUS_FrontendOfdmPapr               papr;               /* This field describes what kind of PAPR reduction is used */
    NEXUS_FrontendOfdmL1Mod              l1Mod;              /* This field field describes the coding of the L1-post signalling data block */
    NEXUS_FrontendOfdmL1CodeRate         l1CodeRate;         /*  This field describes the coding of the L1-post signalling data block */
    NEXUS_FrontendOfdmFecType            l1FecType;          /* This field indicates the type of the L1 FEC used for the L1-post signalling data block */
    NEXUS_FrontendOfdmPilotPattern       pilotPattern;       /* This 4-bit field indicates the scattered pilot pattern used for the data OFDM symbols. */
    uint8_t                              regenFlag;          /* This 3-bit field indicates how many times the DVB-T2 signal has been re-generated */
    bool                                 l1PostExt;          /* This 1-bit field indicates the presence of the L1-post extension field */
    uint8_t                              numRf;              /* This 3-bit field indicates the number of frequencies in the current T2 system */
    uint8_t                              currentRfIndex;     /* If the TFS mode is supported, this 3-bit field indicates the index of the current RF channel
                                                                                                               within its TFS structure, between 0 and numRf-1, otherwise it is set to 0 */
    uint8_t                              txIdAvailability;   /* This 8-bit field is used to signal the availability of transmitter identification signals within
                                                                                                               the current geographic cell. */
    uint8_t                              numT2Frames;        /* This 8-bit field indicates the number of T2-frames per super-frame. The minimum value of
                                                                                                               numT2Frames shall be 2. */
    uint16_t                             numDataSymbols;     /* This 12-bit field indicates Ldata= LF - NP2, the number of data OFDM symbols per
                                                                                                               T2-frame, excluding P1 and P2. */
    uint16_t                             cellId;             /* This is a 16-bit field which uniquely identifies a geographic cell in a DVB-T2 network */
    uint16_t                             networkId;          /* This is a 16-bit field which uniquely identifies the current DVB network */
    uint16_t                             t2SystemId;         /* This 16-bit field uniquely identifies a T2 system within the DVB network */
    uint32_t                             l1PostSize;         /* This 18-bit field indicates the size of the coded and modulated L1-post signalling data block, in OFDM cells. */
    uint32_t                             l1PostInfoSize;     /* This 18-bit field indicates the size of the information part of the L1-post signalling data
                                                                                                                block, in bits, including the extension field, if present, but excluding the CRC */
} NEXUS_FrontendOfdmL1PreStatus;

/***************************************************************************
Summary:
    This enumeration represents the type of the associated PLP. 
    Applicable only to DVBT2 channel according to ETSI EN 302 755 standard.
****************************************************************************/
typedef enum NEXUS_FrontendOfdmPlpType
{
  NEXUS_FrontendOfdmPlpType_eCommon,    /* Common PLP */
  NEXUS_FrontendOfdmPlpType_eDataType1, /* DATA PLP Type 1 */
  NEXUS_FrontendOfdmPlpType_eDataType2  /* DATA PLP Type 2 */
} NEXUS_FrontendOfdmPlpType;

/***************************************************************************
Summary:
    This enumeration represents the type of the payload data carried by the 
    associated PLP. Applicable only to DVBT2 channel according to ETSI EN 302 755 standard.
****************************************************************************/
typedef enum NEXUS_FrontendOfdmPlpPayloadType
{
  NEXUS_FrontendOfdmPlpPayloadType_eGFPS, /* GFPS */
  NEXUS_FrontendOfdmPlpPayloadType_eGCS,  /* GCS */
  NEXUS_FrontendOfdmPlpPayloadType_eGSE,  /* GSE */
  NEXUS_FrontendOfdmPlpPayloadType_eTS    /* TS */
} NEXUS_FrontendOfdmPlpPayloadType;

/***************************************************************************
Summary:
    This enumeration represents the code rate used by the associated PLP.
    Applicable only to DVBT2 channel according to ETSI EN 302 755 standard.
****************************************************************************/
typedef enum NEXUS_FrontendOfdmPlpCodeRate
{
  NEXUS_FrontendOfdmPlpCodeRate_e1_2,
  NEXUS_FrontendOfdmPlpCodeRate_e3_5,
  NEXUS_FrontendOfdmPlpCodeRate_e2_3,
  NEXUS_FrontendOfdmPlpCodeRate_e3_4,
  NEXUS_FrontendOfdmPlpCodeRate_e4_5,
  NEXUS_FrontendOfdmPlpCodeRate_e5_6
} NEXUS_FrontendOfdmPlpCodeRate;

/***************************************************************************
Summary:
    This enumeration represents the modulation used by the associated PLP.
    data block. Applicable only to DVBT2 channel according to ETSI EN 302 755 standard.
****************************************************************************/
typedef enum NEXUS_FrontendOfdmPlpMod
{
  NEXUS_FrontendOfdmPlpMod_eQpsk,   /* QPSK constellation */
  NEXUS_FrontendOfdmPlpMod_e16Qam,  /* 16-QAM constellation */
  NEXUS_FrontendOfdmPlpMod_e64Qam,  /* 64-QAM constellation */
  NEXUS_FrontendOfdmPlpMod_e256Qam  /* 256-QAM constellation */
} NEXUS_FrontendOfdmPlpMod;

/***************************************************************************
Summary:
     This structure is returned when BTC2_GetAsyncStatus() is called.  This
    structure contains the complete L1 Post Configurable PLP parameters of a TC2 channel.
    Applicable only to DVBT2 channel according to ETSI EN 302 755 standard.
****************************************************************************/
typedef struct NEXUS_FrontendOfdmL1PostCfgPlp
{
    uint8_t                              plpId;           /* This 8-bit field identifies uniquely a PLP within the T2 system. */
    uint8_t                              plpGroupId;      /* This 8-bit field identifies with which PLP group within the T2 system the current PLP is associated. */
    NEXUS_FrontendOfdmPlpType            plpType;         /* This 3-bit field indicates the type of the associated PLP */  
    NEXUS_FrontendOfdmPlpPayloadType     plpPayloadType;  /* This 5-bit field indicates the type of the payload data carried by the associated PLP */
    bool                                 ffFlag;          /* This flag is set to '1' if a PLP of type 1 in a TFS system occurs on the same RF channel in each
                                                                                                          T2-frame. This flag is set to '0' if inter-frame TFS is applied as described in annex E */
    uint8_t                              firstRfIdx;      /* This 3-bit field indicates on which RF channel a type 1 data PLP occurs in the first frame of
                                                                                                          a super-frame in a TFS system. */
    uint8_t                              firstFrameIdx;   /* This 8-bit field indicates the IDX of the first frame of the super-frame in which the current PLP occurs. */
    NEXUS_FrontendOfdmPlpCodeRate        plpCodeRate;     /* This field represents the code rate used by the associated PLP. */
    NEXUS_FrontendOfdmPlpMod             plpMod;          /* This 3-bit field indicates the modulation used by the associated PLP */
    bool                                 plpRotation;     /* This flag indicates whether constellation rotation is in use or not by the associatedPLP */
    NEXUS_FrontendOfdmFecType            plpFecType;      /* This 2-bit field indicates the FEC type used by the associated PLP */
    uint16_t                             plpNumBlocksMax; /* This 10-bit field indicates the maximum value of plpNumBlocksMax */
    uint8_t                              frameInterval;   /* This 8-bit field indicates the T2-frame interval within the super-frame for the associated PLP */
    uint8_t                              timeIlLength;
    bool                                 timeIlType;      /* This 1-bit field indicates the type of time-interleaving */
    bool                                 inBandFlag;      /* This 1-bit field indicates whether the current PLP carries in-band type A signalling information */
}NEXUS_FrontendOfdmL1PostCfgPlp;

/***************************************************************************
Summary:
    This structure is returned when BTC2_GetAsyncStatus() is called.  This
    structure contains the complete L1 Post Configurable status. 
    Applicable only to DVBT2 channel according to ETSI EN 302 755 standard.
****************************************************************************/
typedef struct NEXUS_FrontendOfdmL1PostConfigurableStatus
{
    uint16_t                       subSlicesPerFrame; /* This 15-bit field indicates Nsubslices_total, the total number of sub-slices for the type 2
                                                                                                   data PLPs across all RF channels in one T2-frame */
    uint8_t                        numPlp;            /* This 8-bit field indicates the number of PLPs carried within the current super-frame */ 
    uint8_t                        numAux;            /* This 4-bit field indicates the number of auxiliary streams */
    uint8_t                        auxConfigRFu;
    uint8_t                        fefType;           /* This 4-bit field shall indicate the type of the associated FEF part */
    uint8_t                        rfIdx;             /* This 3-bit field indicates the index of each FREQUENCY listed within this loop */
    uint8_t                        fefInterval;       /* This 8-bit field indicates the number of T2-frames between two FEF parts */
    uint32_t                       frequency;         /* This 32-bit field indicates the centre frequency in Hz of the RF channel whose index is rfIdx */
    uint32_t                       fefLength;         /* This 22-bit field indicates the length of the associated FEF part as the number of elementary
                                                                                                   periods T, from the start of the P1 symbol of the FEF part to the start of the P1 symbol of the ext T2-frame*/
    NEXUS_FrontendOfdmL1PostCfgPlp plpA;              /* PLP loop A */
    NEXUS_FrontendOfdmL1PostCfgPlp plpB;              /* PLP loop B */
    uint8_t                        auxStreamType;     /* This 4-bit field indicates the type of the current auxiliary stream */
    uint32_t                       auxPrivateConf;    /* This 28-bit field is for future use for signalling auxiliary streams */
} NEXUS_FrontendOfdmL1PostConfigurableStatus;

/***************************************************************************
Summary:
    This structure represents the TC2 L1 Post Dynamic PLP structure.

Description:
    This structure is returned when BTC2_GetAsyncStatus() is called.  This
    structure contains the complete L1 Post Dynamic PLP parameters of a TC2 channel.

See Also:
    BTC2_GetAsyncStatus()

****************************************************************************/
typedef struct NEXUS_FrontendOfdmL1PostDynamicPlp
{
    uint8_t      plpId;
    uint16_t     plpNumBlocks; /* This 10-bit field indicates the number of FEC blocks contained in the current
                                                           Interleaving Frame for the current PLP */
    uint32_t     plpStart;     /* This 22-bit field indicates the start position of the associated PLP within the current T2-frame
                                                           using the cell addressing scheme */
} NEXUS_FrontendOfdmL1PostDynamicPlp;

/***************************************************************************
Summary:
    This structure is returned when BTC2_GetAsyncStatus() is called.  This
    structure contains the complete L1 Post Dynamic status. 
    Applicable only to DVBT2 channel according to ETSI EN 302 755 standard.
****************************************************************************/
typedef struct NEXUS_FrontendOfdmL1PostDynamicStatus
{
    uint8_t                             frameIdx;            /* This 8-bit field is the index of the current T2-frame within a super-frame */
    uint8_t                             l1ChanlgeCounter;    /* This 8-bit field indicates the number of super-frames ahead where the configuration will change */
    uint8_t                             startRfIdx;          /* This 3-bit field indicates the ID of the starting frequency of the TFS scheduled frame, for the next T2-frame */
    uint32_t                            subSliceInterval;    /* This 22-bit field indicates the number of OFDM cells from the start of one sub-slice of one
                                                                                                                PLP to the start of the next sub-slice of the same PLP on the same RF channel for the current T2-frame */
    uint32_t                            type2Start;          /* This 22-bit field indicates the start position of the first of the type 2 PLPs using the cell addressing scheme */
    NEXUS_FrontendOfdmL1PostDynamicPlp  plpA;                /* plp loop A */
    NEXUS_FrontendOfdmL1PostDynamicPlp  plpB;                /* plp loop B */
    uint32_t                            auxPrivateDyn_47_32; /* This 48-bit field is reserved for future use for signalling auxiliary streams. */
    uint32_t                            auxPrivateDyn_31_0;  /* This 48-bit field is reserved for future use for signalling auxiliary streams. */
} NEXUS_FrontendOfdmL1PostDynamicStatus;

/***************************************************************************
Summary:
    This structure is returned when BTC2_GetAsyncStatus()  is called.  This
    structure contains the PLP informationl. 
    Applicable only to DVBT2 channel according to ETSI EN 302 755 standard.
****************************************************************************/
typedef struct NEXUS_FrontendOfdmPlp
{
    uint8_t                           plpId;
    uint8_t                           plpGroupId;
    NEXUS_FrontendOfdmPlpPayloadType  plpPayloadType;
    NEXUS_FrontendOfdmPlpType         plpType;
} NEXUS_FrontendOfdmPlp;

/***************************************************************************
Summary:
    This structure is returned when BTC2_GetAsyncStatus()  is called.  This
    structure contains the complete L1 Post Configurable status. 
    Applicable only to DVBT2 channel according to ETSI EN 302 755 standard.
****************************************************************************/
typedef struct NEXUS_FrontendOfdmL1PlpStatus
{
    uint8_t                 numPlp;   /* number of PLPs found in the RF signal */
    NEXUS_FrontendOfdmPlp   plp[255];
} NEXUS_FrontendOfdmL1PlpStatus;

/***************************************************************************
Summary:
    This structure contains DVBT2 FEC Statistics. 
    Applicable only to DVBT2 channel according to ETSI EN 302 755 standard.
****************************************************************************/
typedef struct NEXUS_FrontendOfdmFecStatistics
{
    bool           lock;        /* This field indicates the lock status for the given PLP*/
    int32_t        snrData;     /* This signed number represents the Signal-to-Noise Ratio (SNR) computed from slicing the data carriers in units of dB. */
    uint32_t       ldpcAvgIter; /* This unsigned number represents the average number of iterations by the LDPC decoder. */
    uint32_t       ldpcTotIter; /* This unsigned number represents the accumulated number of  iterations by the LDPC decoder */   
    uint32_t       ldpcTotFrm;  /* This unsigned number represents the accumulated number of  total frames processed by the LDPC decoder. */
    uint32_t       ldpcUncFrm;  /* This unsigned number represents the accumulated number of  uncorrectable frames processed by the LDPC decoder */
    uint32_t       ldpcBER;     /* This unsigned number represents the LDPC BER as a fraction of 2^32.  The actual LDPC BER is therefore computed by dividing this integer value by 2^32 */
    uint32_t       bchCorBit;   /* This unsigned number represents the accumulated number of  bits corrected by the BCH decoder */
    uint32_t       bchTotBlk;   /* This unsigned number represents the accumulated number of  total blocks processed by the BCH decoder */
    uint32_t       bchClnBlk;   /* This unsigned number represents the accumulated number of  clean blocks processed by the BCH decoder */
    uint32_t       bchCorBlk;   /* This unsigned number represents the accumulated number of  corrected blocks processed by the BCH decoder */  
    uint32_t       bchUncBlk;   /* This unsigned number represents the accumulated number of  uncorrectable blocks processed by the BCH decoder */
} NEXUS_FrontendOfdmFecStatistics;

/***************************************************************************
Summary:
    OFDM tuning status
****************************************************************************/
typedef struct NEXUS_FrontendOfdmStatus
{
    NEXUS_FrontendOfdmSettings settings; /* Settings provided at last call to NEXUS_Frontend_TuneOfdm */

    bool                                 fecLock;                /* Indicates whether the FEC is locked */
    bool                                 receiverLock;           /* Indicates whether the receiver is locked */
    bool                                 noSignalDetected;       /* True if the demodulator detects no signal for channel scan purposes.
                                                                    This should only be used for channel scanning. */
    bool                                 spectrumInverted;       /* If true, the spectrum is inverted. */
    bool                                 ews;                    /* If true, the ISDB-T EWS (Emergency Warning System) Indicator is present */
    bool                                 partialReception;       /* If true, the ISDB-T signal is partially received */
    NEXUS_FrontendOfdmModulation         modulation;             /* Current modulation type */
    NEXUS_FrontendOfdmTransmissionMode   transmissionMode;       /* Detected trasmission mode */
    NEXUS_FrontendOfdmGuardInterval      guardInterval;          /* Detected guard interval */
    NEXUS_FrontendOfdmCodeRate           codeRate;               /* Code rate of the input signal */
    NEXUS_FrontendOfdmHierarchy          hierarchy;              /* Hierarchy of the input signal */
    unsigned                             cellId;                 /* Indicates Cell Id obtained from the TPS parameters */
    int                                  carrierOffset;          /* Offset of carrier loop in Hz */
    int                                  timingOffset;           /* Offset of timing loop in Hz */
    int                                  snr;                    /* SNR value of receiver in 1/100 dB */

    unsigned                             fecCorrectedBlocks;     /* FEC corrected block count, accumulated since tune or NEXUS_Frontent_ResetStatus */
    unsigned                             fecUncorrectedBlocks;   /* FEC uncorrected block count, accumulated since tune or NEXUS_Frontent_ResetStatus */
    unsigned                             fecCleanBlocks;         /* FEC clean block count, accumulated since tune or NEXUS_Frontent_ResetStatus */
    unsigned                             reacquireCount;         /* number of reacquisitions performed */
    int                                  signalStrength;         /* signal strength in units of 1/10 dBmV */
    unsigned                             signalLevelPercent;     /* Signal Level in percent */
    unsigned                             signalQualityPercent;   /* Signal Quality in percent */

    unsigned                             viterbiUncorrectedBits; /* uncorrected error bits output from Viterbi, accumulated since tune or NEXUS_Frontent_ResetStatus */
    unsigned                             viterbiTotalBits;       /* total number of bits output from Viterbi, accumulated since tune or NEXUS_Frontent_ResetStatus */
    uint32_t                             viterbiErrorRate;       /* viterbi bit error rate (BER) in 1/2147483648 th units.
                                                                    Convert to floating point by dividing by 2147483648.0 */

    uint32_t                             preViterbiErrorRate;    /* This indicates the pre-viterbi bit error rate in 1/2147483648 th unit.
                                                                    Convert to floating point by dividing by 2147483648.0 */

    unsigned                             ifAgcLevel;             /* IF AGC level in units of 1/10 percent */
    unsigned                             rfAgcLevel;             /* tuner AGC level in units of 1/10 percent */

    uint32_t                             rsPerDelta;             /* RS PER delta in 1/2147483648 th units. Valid for a GetStatus call no more than 5 seconds after a previous GetStatus call. */
    uint32_t                             vitBerDelta;            /* Viterbi BER delta in 1/2147483648 th units. Valid for a GetStatus call no more than 5 seconds after a previous GetStatus call. */

    NEXUS_FrontendOfdmModulation       modulationA;             /* ISDB-T Layer A modulation type. */
    NEXUS_FrontendOfdmCodeRate         codeRateA;               /* ISDB-T Layer A code rate. */
    uint32_t                           isdbtAPreRS;             /* ISDB-T Layer A uncorrected error by Viterbi before ReedSolomon */
    NEXUS_FrontendOfdmTimeInterleaving timeInterleavingA;       /* ISDB-T Layer A time interleaving.*/
    unsigned                           numSegmentsA;            /* ISDB-T Layer A number of segments. */
    unsigned                           fecCorrectedBlocksA;     /* ISDB-T Layer A FEC corrected block count, accumulated since tune or NEXUS_Frontent_ResetStatus */
    unsigned                           fecUncorrectedBlocksA;   /* ISDB-T Layer A FEC uncorrected block count, accumulated since tune or NEXUS_Frontent_ResetStatus */
    unsigned                           fecCleanBlocksA;         /* ISDB-T Layer A FEC clean block count, accumulated since tune or NEXUS_Frontent_ResetStatus */
    unsigned                           signalLevelPercentA;     /* ISDB-T Layer A Signal Level in percent */
    unsigned                           signalQualityPercentA;   /* ISDB-T Layer A Signal Quality in percent */

    NEXUS_FrontendOfdmModulation       modulationB;             /* ISDB-T Layer B modulation type. */
    NEXUS_FrontendOfdmCodeRate         codeRateB;               /* ISDB-T Layer B code rate. */
    uint32_t                           isdbtBPreRS;             /* ISDB-T Layer B uncorrected error by Viterbi before ReedSolomon */
    NEXUS_FrontendOfdmTimeInterleaving timeInterleavingB;       /* ISDB-T Layer B time interleaving.*/
    unsigned                           numSegmentsB;            /* ISDB-T Layer B number of segments. */
    unsigned                           fecCorrectedBlocksB;     /* ISDB-T Layer B FEC corrected block count, accumulated since tune or NEXUS_Frontent_ResetStatus */
    unsigned                           fecUncorrectedBlocksB;   /* ISDB-T Layer B FEC uncorrected block count, accumulated since tune or NEXUS_Frontent_ResetStatus */
    unsigned                           fecCleanBlocksB;         /* ISDB-T Layer B FEC clean block count, accumulated since tune or NEXUS_Frontent_ResetStatus */
    unsigned                           signalLevelPercentB;     /* ISDB-T Layer B Signal Level in percent */
    unsigned                           signalQualityPercentB;   /* ISDB-T Layer B Signal Quality in percent */

    NEXUS_FrontendOfdmModulation       modulationC;             /* ISDB-T Layer C modulation type. */
    NEXUS_FrontendOfdmCodeRate         codeRateC;               /* ISDB-T Layer C code rate. */
    uint32_t                           isdbtCPreRS;             /* ISDB-T Layer C uncorrected error by Viterbi before ReedSolomon */
    NEXUS_FrontendOfdmTimeInterleaving timeInterleavingC;       /* ISDB-T Layer C time interleaving.*/
    unsigned                           numSegmentsC;            /* ISDB-T Layer C number of segments. */
    unsigned                           fecCorrectedBlocksC;     /* ISDB-T Layer C FEC corrected block count, accumulated since tune or NEXUS_Frontent_ResetStatus */
    unsigned                           fecUncorrectedBlocksC;   /* ISDB-T Layer C FEC uncorrected block count, accumulated since tune or NEXUS_Frontent_ResetStatus */
    unsigned                           fecCleanBlocksC;         /* ISDB-T Layer C FEC clean block count, accumulated since tune or NEXUS_Frontent_ResetStatus */
    unsigned                           signalLevelPercentC;     /* ISDB-T Layer C Signal Level in percent */
    unsigned                           signalQualityPercentC;   /* ISDB-T Layer C Signal Quality in percent */
    struct
    {
       int32_t                                      gainOffset;
       NEXUS_FrontendOfdmFecStatistics              l1PreStatistics;     /* This Structure contains the lock status, SNR and FEC Statistics for L1 Pre */
       NEXUS_FrontendOfdmFecStatistics              l1PostStatistics;    /* This Structure contains the lock status, SNR and FEC Statistics for L1 Post */
       NEXUS_FrontendOfdmFecStatistics              plpAStatistics;      /* This Structure contains the lock status, SNR and FEC Statistics for PLPA */
       NEXUS_FrontendOfdmFecStatistics              plpBStatistics;      /* This Structure contains the lock status, SNR and FEC Statistics for PLPB */
       NEXUS_FrontendOfdmL1PreStatus                l1PreStatus;         /* This Structure contains the decoded L1 Pre parameters */
       NEXUS_FrontendOfdmL1PostConfigurableStatus   l1PostCfgStatus;     /* This Structure contains the decoded L1 Post Configurable parameters */
       NEXUS_FrontendOfdmL1PostDynamicStatus        l1PostDynamicStatus; /* This Structure contains the decoded L1 Post Dynamic parameters */
       NEXUS_FrontendOfdmL1PlpStatus                l1PlpStatus;         /* This Structure contains a list of the PLPs found on the RF signal */
    } dvbt2Status;
} NEXUS_FrontendOfdmStatus;

/***************************************************************************
Summary:
    Get the status of a OFDM tuner
 ***************************************************************************/
NEXUS_Error NEXUS_Frontend_GetOfdmStatus(
    NEXUS_FrontendHandle handle,
    NEXUS_FrontendOfdmStatus *pStatus /* [out] */
    );

/***************************************************************************
Summary:
    Request the asynchronous status of a Ofdm tuner
***************************************************************************/
NEXUS_Error NEXUS_Frontend_RequestOfdmAsyncStatus(
    NEXUS_FrontendHandle handle
    );

/***************************************************************************
Summary:
    Get the asynchronous status of a Ofdm tuner
***************************************************************************/
NEXUS_Error NEXUS_Frontend_GetOfdmAsyncStatus(
    NEXUS_FrontendHandle handle,
    NEXUS_FrontendOfdmStatus *pStatus /* [out] */
    );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef NEXUS_FRONTEND_OFDM_H__ */

