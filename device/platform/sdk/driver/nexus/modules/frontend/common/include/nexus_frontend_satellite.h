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
* $brcm_Workfile: nexus_frontend_satellite.h $
* $brcm_Revision: 34 $
* $brcm_Date: 6/21/12 5:26p $
*
* API Description:
*   API name: Frontend Satellite
*    Generic APIs for satelitte tuning.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/frontend/common/include/nexus_frontend_satellite.h $
*
* 34   6/21/12 5:26p mphillip
* SW7358-304: Merge tone search to main
*
* SW7358-304/3   6/21/12 5:13p mphillip
* SW7358-304: Place tone search min ratio numerator/denominator in a
*  minRatio struct in tone search settings
*
* SW7358-304/2   6/20/12 6:32p mphillip
* SW7358-304: Remove Peakscan to BlindScan rename, update parameters and
*  results for tone search
*
* SW7358-304/1   6/13/12 6:40p mphillip
* SW7358-304: Tone search API proposal
*
* 33   5/15/12 3:35p mphillip
* SW3461-210: Switch Read/WriteSatelliteRegister to the new, general
*  interface
*
* 32   5/15/12 3:10p mphillip
* SW7358-277: Merge BERT clarifications to main
*
* SW7358-277/1   4/25/12 4:48p mphillip
* SW7358-277: Clarify BERT comments, rename berErrors to berErrorCount
*
* 31   11/11/11 3:50p mphillip
* SW7346-516: Merge to main
*
* SW7346-516/1   11/8/11 5:18p mphillip
* SW7346-516: Allow diseqc to be powered down separately
*
* 30   2/16/11 1:04p erickson
* SW7420-1491: add nelem required with nelem_out
*
* 29   2/15/11 2:13p erickson
* SW7420-1491: change to preferred nelem_out attribute
*
* 28   11/16/10 2:20p erickson
* SW7405-4968: add NEXUS_FrontendSatelliteSettings.networkSpec
*
* 27   11/9/10 3:13p erickson
* SW7405-4968: add NEXUS_FrontendSatelliteNetworkSpec, update 4506 and
*  73xx Open
*
* 26   10/7/10 9:43a erickson
* SW7325-820: fix sign of NEXUS_FrontendSatelliteStatus.agf
*
* 25   8/20/09 12:57p erickson
* PR57471: convert to NEXUS_FrontendSatelliteStatusError enum
*
* 24   8/12/09 3:07p erickson
* PR57471: added NEXUS_FrontendSatelliteStatus.statusAvailable
*
* 23   8/12/09 10:38a erickson
* PR57598: added NEXUS_FrontendDiseqcStatus.sendStatus
*
* 22   7/31/09 2:40p erickson
* PR57239: added NEXUS_Frontend_ReadSatelliteRegister and
*  NEXUS_Frontend_WriteSatelliteRegister
*
* 21   7/30/09 5:05p erickson
* PR57175: sync up NEXUS_FrontendDiseqcMessageStatus with AST PI.
*
* 20   7/21/09 3:21p erickson
* PR56834: deprecated NEXUS_FrontendSatelliteStatus.lnaStatus for all sat
*  demods except 4501
*
* 19   7/14/09 10:40a erickson
* PR56791: added NEXUS_FrontendDiseqcSettings.replyDisabled
*
* 18   7/13/09 4:47p erickson
* PR56773: added NEXUS_FrontendDiseqcStatus
*
* 17   7/13/09 3:48p erickson
* PR56509: add NEXUS_FrontendDiseqcSettings.lnbEnabled
*
* 16   6/29/09 9:38a erickson
* PR56408: added NEXUS_Frontend_ResetDiseqc
*
* 15   6/24/09 9:31a erickson
* PR53078: added NEXUS_FrontendSatelliteStatus.agf
*
* 14   6/16/09 3:54p jgarrett
* PR 55888: Adding envelope mode
*
* 13   6/9/09 9:03a erickson
* PR54416: fix bad merge
*
* 12   6/4/09 10:27a erickson
* PR54416: merge
*
* 11   5/26/09 12:16p erickson
* PR55403: added NEXUS_FrontendDiseqcToneBurst
*
* PR54416/3   6/3/09 3:56p jtna
* PR54416: allow freqRange=0 and expose peakpower in peakscan
*
* PR54416/2   5/21/09 6:03p jtna
* PR54416: improve naming
*
* PR54416/1   5/15/09 7:47p jtna
* PR54416: nexus support for satellite scanning
*
* 10   5/11/09 4:29p erickson
* PR53078: added NEXUS_FrontendSatelliteStatus.timeElapsed
*
* 9   4/2/09 2:29p erickson
* PR53078: satellite api additions
*
* 8   3/5/09 10:42a jgarrett
* PR 52817: Converting offset values to signed
*
* 7   2/9/09 5:04p jgarrett
* PR 50837: Merge to main branch
*
* PR50837/1   1/29/09 10:37a jgarrett
* PR 50837: Adding ifOffset
*
* 6   12/18/08 6:37p jgarrett
* PR 49983: Adding more status members
*
* 5   11/14/08 6:58p jgarrett
* PR 48985: Adding turbo and ldpc options
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
#ifndef NEXUS_FRONTEND_SATELLITE_H__
#define NEXUS_FRONTEND_SATELLITE_H__

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
Summary:
Satellite acquisition modes
***************************************************************************/
typedef enum NEXUS_FrontendSatelliteMode
{
    NEXUS_FrontendSatelliteMode_eDvb,
    NEXUS_FrontendSatelliteMode_eDss,
    NEXUS_FrontendSatelliteMode_eDcii,
    NEXUS_FrontendSatelliteMode_eQpskTurbo,
    NEXUS_FrontendSatelliteMode_eTurboQpsk=NEXUS_FrontendSatelliteMode_eQpskTurbo,
    NEXUS_FrontendSatelliteMode_e8pskTurbo,
    NEXUS_FrontendSatelliteMode_eTurbo8psk=NEXUS_FrontendSatelliteMode_e8pskTurbo,
    NEXUS_FrontendSatelliteMode_eTurbo,
    NEXUS_FrontendSatelliteMode_eQpskLdpc,
    NEXUS_FrontendSatelliteMode_e8pskLdpc,
    NEXUS_FrontendSatelliteMode_eLdpc,
    NEXUS_FrontendSatelliteMode_eBlindAcquisition,
    NEXUS_FrontendSatelliteMode_eMax
} NEXUS_FrontendSatelliteMode;

/***************************************************************************
Summary:
Satellite spectral inversion
***************************************************************************/
typedef enum NEXUS_FrontendSatelliteInversion
{
    NEXUS_FrontendSatelliteInversion_eScan,         /* Scan for spectral inversion. Default setting on tune, unlikely to be returned for status. */
    NEXUS_FrontendSatelliteInversion_eNormal,       /* No spectral inversion. */
    NEXUS_FrontendSatelliteInversion_eI,            /* Spectral inversion. For older frontends, may indicate I vs. Q inversion. */
    NEXUS_FrontendSatelliteInversion_eInverted = NEXUS_FrontendSatelliteInversion_eI,
    NEXUS_FrontendSatelliteInversion_eQ,            /* Spectral inversion. Never returned for 28nm (or newer) frontends. */
    NEXUS_FrontendSatelliteInversion_eMax
} NEXUS_FrontendSatelliteInversion;

/***************************************************************************
Summary:
Satellite code rate
***************************************************************************/
typedef struct NEXUS_FrontendSatelliteCodeRate
{
    unsigned numerator;         /* If 0, value is scanned */
    unsigned denominator;       /* If 0, value is scanned */
    unsigned bitsPerSymbol;     /* x1000.  If 0, uses the default.  For example, 2050 is 2.05 */
} NEXUS_FrontendSatelliteCodeRate;

/***************************************************************************
Summary:
Satellite network spec
***************************************************************************/
typedef enum NEXUS_FrontendSatelliteNetworkSpec
{
   NEXUS_FrontendSatelliteNetworkSpec_eDefault = 0,
   NEXUS_FrontendSatelliteNetworkSpec_eCustom1,
   NEXUS_FrontendSatelliteNetworkSpec_eEuro,
   NEXUS_FrontendSatelliteNetworkSpec_eMax
} NEXUS_FrontendSatelliteNetworkSpec;

/***************************************************************************
Summary:
Settings used for NEXUS_Frontend_TuneSatellite
***************************************************************************/
typedef struct NEXUS_FrontendSatelliteSettings
{
    NEXUS_FrontendSatelliteMode mode;
    NEXUS_FrontendSatelliteCodeRate codeRate;
    NEXUS_FrontendSatelliteInversion spectralInversion;
    unsigned frequency;                                 /* In Hz */
    unsigned symbolRate;                                /* In Baud */
    int      carrierOffset;                             /* In Hz */
    unsigned searchRange;                               /* In Hz */
    int32_t ifOffset;                                   /* In Hz */
    NEXUS_FrontendSatelliteNetworkSpec networkSpec;

    /* The following settings map into the demod's 32 bit acq_ctl control word */
    bool pnData;
    bool prbs15;            /* true: PRBS-15, false: PRBS-23 */
    bool nyquist20;         /* true: nyquist20, false: nyquist35 */
    bool bertEnable;        /* If set to true, this sets the frontend into BER test mode (see the BER Testing section of the satellite app note for details). */
                            /* Enabling this prevents the frontend from working with non-test signals until it is disabled. */
                            /* Once enabled, only a signal of the expected test format is valid input. */
                            /* A call to NEXUS_Frontend_TuneSatellite should follow changes to this setting. */
    bool pnDataInvert;
    bool bertResyncDisable;
    bool reacquireDisable;
    bool dciiSplit;
    bool dciiSplitQ;
    bool oQpsk;
    bool rsDisable;
    bool ldpcPilot;         /* This option applies only for LDPC modes (low density parity check) */
    bool ldpcPilotPll;      /* This option applies only for LDPC modes only if ldpcPilot is true */
    bool ldpcPilotScan;
    bool tunerTestMode;
    bool toneSearchMode;
    bool signalDetectMode;
    bool bypassTune;
    bool bypassAcquire;

    NEXUS_CallbackDesc lockCallback; /* This callback will be called when lock status changes */
} NEXUS_FrontendSatelliteSettings;

/***************************************************************************
Summary:
    Initialize a satellite settings structure to defaults
***************************************************************************/
void NEXUS_Frontend_GetDefaultSatelliteSettings(
    NEXUS_FrontendSatelliteSettings *pSettings /* [out] */
    );

/***************************************************************************
Summary:
    Initialize a satellite settings structure to defaults for a specific mode
***************************************************************************/
void NEXUS_Frontend_GetDefaultSatelliteSettingsForMode(
    NEXUS_FrontendSatelliteMode mode,
    NEXUS_FrontendSatelliteSettings *pSettings /* [out] */
    );

/***************************************************************************
Summary:
    Tune to a satellite channel
***************************************************************************/
NEXUS_Error NEXUS_Frontend_TuneSatellite(
    NEXUS_FrontendHandle handle,
    const NEXUS_FrontendSatelliteSettings *pSettings
    );

/***************************************************************************
Summary:
Type of error encountered when reading status.
***************************************************************************/
typedef enum NEXUS_FrontendSatelliteStatusError
{
    NEXUS_FrontendSatelliteStatusError_eNone,
    NEXUS_FrontendSatelliteStatusError_eI2cNoAck,
    NEXUS_FrontendSatelliteStatusError_eHabTimeout,
    NEXUS_FrontendSatelliteStatusError_eGetChannelStatusError,
    NEXUS_FrontendSatelliteStatusError_eMax
} NEXUS_FrontendSatelliteStatusError;

/***************************************************************************
Summary:
Satellite tuning status
***************************************************************************/
typedef struct NEXUS_FrontendSatelliteStatus
{
    NEXUS_FrontendSatelliteSettings settings; /* Settings provided at last call to NEXUS_Frontend_TuneSatellite. */

    NEXUS_FrontendSatelliteStatusError statusError; /* Returns a recognized failure. NEXUS_Frontend_GetSatelliteStatus will return NEXUS_SUCCESS so this statusError
                                                       can be retrieved. */

    NEXUS_FrontendSatelliteMode mode;                       /* Mode */
    NEXUS_FrontendSatelliteCodeRate codeRate;               /* Code rate detected */
    NEXUS_FrontendSatelliteInversion spectralInversion;     /* Spectral inversion status */
    unsigned frequency;         /* actual tuner frequency */

    bool tunerLocked;           /* true if the tuner is locked */
    bool demodLocked;           /* true if the demodulator is locked */
    bool bertLocked;            /* true if the BER tester is locked.  If so, see berEstimate. */
                                /* Also, see BERT notes under NEXUS_FrontendSatelliteSettings. */

    unsigned channel;           /* Channel number */
    unsigned symbolRate;        /* In baud */
    int symbolRateError;        /* In baud */

    int carrierOffset;          /* In Hz */
    int carrierError;           /* In Hz */
    unsigned sampleClock;       /* In Hz */
    unsigned outputBitRate;     /* Output bit rate in bps */

    unsigned ifAgcLevel;        /* IF AGC level in units of 1/10 percent */
    unsigned rfAgcLevel;        /* tuner AGC level in units of 1/10 percent */
    unsigned intAgcLevel;       /* Internal AGC level in units of 1/10 percent */
    unsigned snrEstimate;       /* SNR in 1/100 dB */
    unsigned berEstimate;       /* Bit error rate as log10 of 0.0-1.0 range.
                                    1.0  => 100% => 0
                                    0.1  => 10%  => -1
                                    0.01 => 1%   => -2
                                    0    => 0%   => 1 (special value for NONE)
                                    If bertLocked == false, it's set to 1. */

    unsigned fecPhase;          /* 0, 90, 180, 270 */
    unsigned fecCorrected;      /* cumulative block correctable errors */
    unsigned fecUncorrected;    /* cumulative block uncorrectable errors */
    unsigned fecClean;          /* cumulative clean block count */
    unsigned bitErrCorrected;   /* cumulative bit correctable errors */
    unsigned reacquireCount;    /* cumulative reacquisition count */
    unsigned berErrorCount;     /* BER error count - only valid when using BERT and when bertLocked is true.  Also, see BERT notes under NEXUS_FrontendSatelliteSettings. */
    unsigned preViterbiErrorCount;    /* accumulated pre-Viterbi error count */
    unsigned mpegErrors;        /* mpeg frame error count */
    unsigned mpegCount;         /* total mpeg frame count */
    unsigned ifAgc;             /* if agc value from hw unmodified */
    unsigned rfAgc;             /* rf agc value from hw unmodified */
    int agf;                    /* AGF integrator value */

    /* Deprecated. lnaStatus is only populated for BCM4501 frontends. See NEXUS_Frontend_Get73xxLnaStatus. */
    struct
    {
        unsigned intConfig;
        unsigned extConfig;
        unsigned version;
        unsigned agc0;
        unsigned agc1;
    } lnaStatus; /* deprecated */

    struct {
        uint16_t chipId;
        uint8_t chipVersion;
        uint32_t bondingOption;
        uint8_t apMicrocodeVersion;
        uint8_t hostConfigurationVersion;
    } version; /* hardware, firmware and software version information */

    unsigned timeElapsed; /* time elapsed in milliseconds since the last call to NEXUS_Frontend_ResetStatus.
                             the elapsed time is measured at the same time that the rest of the values in NEXUS_FrontendSatelliteStatus are captured. */
} NEXUS_FrontendSatelliteStatus;

/***************************************************************************
Summary:
Get the status of a satellite tuner

Description:
This function will return an error code only if it's unable to get any status because of an unknown failure.
In some cases, the type of status error is recognized as a specific type of failure, NEXUS_FrontendSatelliteStatus.statusError is
set and the function returns NEXUS_SUCCESS.
***************************************************************************/
NEXUS_Error NEXUS_Frontend_GetSatelliteStatus(
    NEXUS_FrontendHandle handle,
    NEXUS_FrontendSatelliteStatus *pStatus /* [out] */
    );

/***************************************************************************
Summary:
Power estimation status for a satellite channel

Description:
This structure contains an array of values which can be used in power
estimation.  The formula to estimate power is specific to a given frontend
chip and board configuration.

Which and how many values returned vary from satellite chip to satellite
chip.

A given value in the array should not be used if the corresponding valid
flag is set to false.  Depending on the chip, an invalid value may be
unexpectedly 0, or random data.

See Also:
    NEXUS_Frontend_GetSatelliteAgcStatus
***************************************************************************/
#define MAX_SATELLITE_AGC_VALUES 5
typedef struct NEXUS_FrontendSatelliteAgcStatus
{
    struct {
        bool valid;         /* If true, value may be used in a calculation.  If false, do not use this value. */
        unsigned value;     /* A value to be used in power estimation. */
    } agc[MAX_SATELLITE_AGC_VALUES];
} NEXUS_FrontendSatelliteAgcStatus;

/***************************************************************************
Summary:
Get the power estimation values for this specific frontend.

Returns:
    NEXUS_SUCCESS: the runtime settings in pSettings were set
    NEXUS_NOT_SUPPORTED: this function is not yet implemented for this frontend
    NEXUS_OS_ERROR: the settings could not be read

See Also:
    NEXUS_Frontend4538AgcStatus
***************************************************************************/
NEXUS_Error NEXUS_Frontend_GetSatelliteAgcStatus(
    NEXUS_FrontendHandle handle,
    NEXUS_FrontendSatelliteAgcStatus *pStatus
);

/***************************************************************************
Summary:
DiSEqC voltage
***************************************************************************/
typedef enum NEXUS_FrontendDiseqcVoltage
{
    NEXUS_FrontendDiseqcVoltage_e13v,
    NEXUS_FrontendDiseqcVoltage_e18v,
    NEXUS_FrontendDiseqcVoltage_eMax
} NEXUS_FrontendDiseqcVoltage;

/***************************************************************************
Summary:
DiSEqC tone burst
***************************************************************************/
typedef enum NEXUS_FrontendDiseqcToneBurst {
    NEXUS_FrontendDiseqcToneBurst_eNone,
    NEXUS_FrontendDiseqcToneBurst_eUnmodulated,
    NEXUS_FrontendDiseqcToneBurst_eNominal,
    NEXUS_FrontendDiseqcToneBurst_eMax
} NEXUS_FrontendDiseqcToneBurst;

/***************************************************************************
Summary:
DiSEqC tone burst
***************************************************************************/
typedef enum NEXUS_FrontendDiseqcToneMode {
    NEXUS_FrontendDiseqcToneMode_eTone,
    NEXUS_FrontendDiseqcToneMode_eEnvelope,
    NEXUS_FrontendDiseqcToneMode_eMax
} NEXUS_FrontendDiseqcToneMode;

/***************************************************************************
Summary:
DiSEqC message framing override

Description:
DiSEqC messages can embed whether or not a reply is expected. When set to eDefault,
diseqc will use the value in the command to determine whether or not a reply is expected.

When set to _eExpectReply, this is overridden, and it forces a reply to be expected.

When set to _eDontExpectReply, this is overridden, and no reply is expected.
***************************************************************************/
typedef enum NEXUS_FrontendDiseqcFraming {
    NEXUS_FrontendDiseqcFraming_eDefault,
    NEXUS_FrontendDiseqcFraming_eExpectReply,
    NEXUS_FrontendDiseqcFraming_eDontExpectReply,
    NEXUS_FrontendDiseqcFraming_eMax
} NEXUS_FrontendDiseqcFraming;

/***************************************************************************
Summary:
DiSEqC Settings for tone and voltage control
***************************************************************************/
typedef struct NEXUS_FrontendDiseqcSettings
{
    bool lnbEnabled; /* if true, LNBP voltage (low noise blocker power) is enabled, otherwise LNBP voltage is zero volt */

    NEXUS_FrontendDiseqcVoltage voltage;
    bool toneEnabled;
    NEXUS_FrontendDiseqcToneBurst toneBurst;
    NEXUS_FrontendDiseqcToneMode toneMode;

    bool replyDisabled;

    bool enabled; /* If set to false, power down the diseqc core. */

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_FRONTEND_ADD)
	unsigned char ucPreTxDelay;
	unsigned char ulBurstQuietTime;
#endif

    NEXUS_FrontendDiseqcFraming framing;
} NEXUS_FrontendDiseqcSettings;

/***************************************************************************
Summary:
    Get the current DiSEqC settings for a satellite tuner
***************************************************************************/
void NEXUS_Frontend_GetDiseqcSettings(
    NEXUS_FrontendHandle handle,
    NEXUS_FrontendDiseqcSettings *pSettings /* [out] */
    );

/***************************************************************************
Summary:
    Set the current DiSEqC settings for a satellite tuner
***************************************************************************/
NEXUS_Error NEXUS_Frontend_SetDiseqcSettings(
    NEXUS_FrontendHandle handle,
    const NEXUS_FrontendDiseqcSettings *pSettings
    );

/***************************************************************************
Summary:
DiSEqC message transmission status
***************************************************************************/
typedef enum NEXUS_FrontendDiseqcMessageStatus
{
    NEXUS_FrontendDiseqcMessageStatus_eSuccess = 0, /* no error */
    NEXUS_FrontendDiseqcMessageStatus_eRxOverflow,
    NEXUS_FrontendDiseqcMessageStatus_eRxReplyTimeout,
    NEXUS_FrontendDiseqcMessageStatus_eRxParityError,
    NEXUS_FrontendDiseqcMessageStatus_eAcwTimeout,
    NEXUS_FrontendDiseqcMessageStatus_eBusy,
    NEXUS_FrontendDiseqcMessageStatus_eMax
} NEXUS_FrontendDiseqcMessageStatus;

/***************************************************************************
Summary:
    DiSEqC status

Description:
    This retrives the state of hardware, not current software state.
***************************************************************************/
typedef struct NEXUS_FrontendDiseqcStatus
{
    bool toneEnabled; /* 22KHz continuous tone present */
    unsigned voltage; /* voltage estimation */
    NEXUS_FrontendDiseqcMessageStatus sendStatus; /* last disqec send status.
                                                     same status as NEXUS_Frontend_GetDiseqcReply's pStatus out parameter.
                                                     use this if no reply buffer is needed. */
} NEXUS_FrontendDiseqcStatus;

/***************************************************************************
Summary:
    Get the current DiSEqC status
***************************************************************************/
NEXUS_Error NEXUS_Frontend_GetDiseqcStatus(
    NEXUS_FrontendHandle handle,
    NEXUS_FrontendDiseqcStatus *pStatus /* [out] */
    );

/***************************************************************************
Summary:
    Send a DiSEqC message

Description:
    This routine will send the number of bytes specified in a DiSEqC message.
    The callback provided will be called when the transfer is complete, at
    which point the reply/status can be read.
***************************************************************************/
NEXUS_Error NEXUS_Frontend_SendDiseqcMessage(
    NEXUS_FrontendHandle handle,
    const uint8_t *pSendData,               /* attr{nelem=sendDataSize} */
    size_t sendDataSize,                    /* In Bytes */
    const NEXUS_CallbackDesc *pSendComplete /* Callback will be received when message status is ready */
    );

/***************************************************************************
Summary:
    Reset DiSEqC

Description:
    This routine will reset the diseqc.
 ***************************************************************************/
NEXUS_Error NEXUS_Frontend_ResetDiseqc(
    NEXUS_FrontendHandle handle,
    uint8_t options
    );

/***************************************************************************
Summary:
    Read a DiSEqC message reply

Description:
    This routine will read up to the number of bytes specified from a DiSEqC
    message reply.  Any remaining bytes will be discarded.
***************************************************************************/
NEXUS_Error NEXUS_Frontend_GetDiseqcReply(
    NEXUS_FrontendHandle handle,
    NEXUS_FrontendDiseqcMessageStatus *pStatus,
    uint8_t *pReplyBuffer,              /* [out] attr{nelem=replyBufferSize;nelem_out=pReplyLength} */
    size_t replyBufferSize,             /* in bytes */
    size_t *pReplyLength                /* [out] */
    );

/***************************************************************************
Summary:
    Send a DiSEqC Auto Control Word
***************************************************************************/
NEXUS_Error NEXUS_Frontend_SendDiseqcAcw(
    NEXUS_FrontendHandle handle,
    uint8_t codeWord
    );

/***************************************************************************
Summary:
    Reads from the configuration space.
***************************************************************************/
NEXUS_Error NEXUS_Frontend_ReadSatelliteConfig(
    NEXUS_FrontendHandle handle,
    unsigned id,        /* configuration parameter ID */
    void *buffer,       /* [out] attr{nelem=bufferSize} bytes to read */
    unsigned bufferSize /* size of buffer in bytes */
    );

/***************************************************************************
Summary:
    Writes to the configuration space.
***************************************************************************/
NEXUS_Error NEXUS_Frontend_WriteSatelliteConfig(
    NEXUS_FrontendHandle handle,
    unsigned id,        /* configuration parameter ID */
    const void *buffer, /* attr{nelem=bufferSize} bytes to write */
    unsigned bufferSize /* size of buffer in bytes */
    );

/***************************************************************************
Summary:
    Satellite blind scan settings

Description:
    This is used to control a blind (symbol rate) scan.
***************************************************************************/
typedef struct NEXUS_FrontendSatellitePeakscanSettings {
    unsigned frequency;                  /* tuner frequency in Hz */
    unsigned minSymbolRate;              /* minimum symbol rate in sym/sec */
    unsigned maxSymbolRate;              /* maximum symbol rate in sym/sec */
    unsigned frequencyRange;             /* search range in Hz. freq -/+freqRange will be searched */
    unsigned frequencyStep;              /* tuner step frequency in Hz */
    NEXUS_CallbackDesc peakscanCallback; /* this callback will be called when the blind scan finishes */
} NEXUS_FrontendSatellitePeakscanSettings;

/***************************************************************************
Summary:
    Get default satellite blind scan settings
***************************************************************************/
void NEXUS_Frontend_GetDefaultSatellitePeakscanSettings(
    NEXUS_FrontendSatellitePeakscanSettings *pSettings /* [out] */
    );

/***************************************************************************
Summary:
    Minimum and maximum values for NEXUS_FrontendSatellitePeakscanSettings.freq
***************************************************************************/
#define NEXUS_SATELLITE_PEAKSCAN_MIN_FREQ 250000000UL
#define NEXUS_SATELLITE_PEAKSCAN_MAX_FREQ 2150000000UL

/***************************************************************************
Summary:
    Perform satellite blind scan

Description:
    This function initiates a scan for a peak signal, and returns the matching
symbol rate.  This is only a symbol rate scan.  For tone search, call
NEXUS_Frontend_SatelliteSearchTone().
***************************************************************************/
NEXUS_Error NEXUS_Frontend_SatellitePeakscan(
    NEXUS_FrontendHandle handle,
    const NEXUS_FrontendSatellitePeakscanSettings *pSettings
    );

/***************************************************************************
Summary:
    Satellite blind scan result
***************************************************************************/
typedef struct NEXUS_FrontendSatellitePeakscanResult {
    unsigned peakFrequency;  /* peak frequency. null if no peak frequency was found for given peak scan settings */
    unsigned symbolRate;     /* symbol rate corresponding to peak frequency */
    unsigned lastFrequency;  /* last frequency scanned before scan was terminated */
    unsigned peakPower;      /* power corresponding to peakFrequency */
} NEXUS_FrontendSatellitePeakscanResult;

/***************************************************************************
Summary:
    Get results of satellite blind scan
***************************************************************************/
NEXUS_Error NEXUS_Frontend_GetSatellitePeakscanResult(
    NEXUS_FrontendHandle handle,
    NEXUS_FrontendSatellitePeakscanResult *pResult /* [out] */
    );

/***************************************************************************
Summary:
    Satellite tone search settings

Description:
    This is used to control a tone search peak scan.
***************************************************************************/
typedef struct NEXUS_FrontendSatelliteToneSearchSettings {
    unsigned frequency;                  /* tuner frequency in Hz */
    unsigned frequencyRange;             /* search range in Hz. freq -/+freqRange will be searched */
                                         /* unlike blind scan, the frequency step is dictated by the signal */
    struct {
        unsigned numerator;                  /* (numerator / denominator) defines the minimum ratio of max to min power, default is 2.5 (numerator = 5, denominator = 2) */
        unsigned denominator;
    } minRatio;
    NEXUS_CallbackDesc completionCallback; /* this callback will be called when the tone search finishes */
} NEXUS_FrontendSatelliteToneSearchSettings;

/***************************************************************************
Summary:
    Get default satellite blind scan settings
***************************************************************************/
void NEXUS_Frontend_GetDefaultSatelliteToneSearchSettings(
    NEXUS_FrontendSatelliteToneSearchSettings *pSettings /* [out] */
    );

/***************************************************************************
Summary:
    Perform satellite tone search

Description:
    This function initiates a scan for a peak signal, and returns the matching
frequencies.  This is searches for tone.  For a blind symbol rate scan, call
NEXUS_Frontend_SatellitePeakscan().
***************************************************************************/
NEXUS_Error NEXUS_Frontend_SatelliteToneSearch(
    NEXUS_FrontendHandle handle,
    const NEXUS_FrontendSatelliteToneSearchSettings *pSettings
    );

/***************************************************************************
Summary:
    Satellite tone search result
***************************************************************************/
typedef struct NEXUS_FrontendSatelliteToneSearchResult {
    unsigned peakFrequency;  /* peak frequency (in Hz). NULL if no peak frequency was found for given tone search settings */
    unsigned lastFrequency;  /* last frequency (in Hz) scanned before scan was terminated */
    unsigned peakPower;      /* power corresponding to peakFrequency */
    unsigned frequencyStep;  /* tuner step frequency in Hz as calculated for this tone search */
} NEXUS_FrontendSatelliteToneSearchResult;

/***************************************************************************
Summary:
    Get results of satellite tone search
***************************************************************************/
NEXUS_Error NEXUS_Frontend_GetSatelliteToneSearchResult(
    NEXUS_FrontendHandle handle,
    NEXUS_FrontendSatelliteToneSearchResult *pResult /* [out] */
    );

/***************************************************************************
Summary:
    Satellite signal detect status

Description:
    This structure is filled by NEXUS_Frontend_GetSatelliteSignalDetectStatus.

See also:
    NEXUS_FrontendSatelliteSettings,
    NEXUS_Frontend_TuneSatellite,
    NEXUS_Frontend_GetSatelliteSignalDetectStatus
***************************************************************************/
typedef struct NEXUS_FrontendSatelliteSignalDetectStatus {
    bool enabled;           /* true if signal detect mode was initiated by the last NEXUS_Frontend_TuneSatellite.
                               if false, the last tune was not in signal detect mode and the rest of the structure is invalid. */
    bool detectionComplete; /* true if enabled AND signal detection operation has completed */
    bool signalDetected;    /* true if completed AND a signal was detected (the timing loop locked) */
} NEXUS_FrontendSatelliteSignalDetectStatus;

/***************************************************************************
Summary:
    Get results of satellite detect status

Description:
    This function returns the status for signal detect mode.  To initiate a signal detect
tune, set signalDetectMode in the NEXUS_FrontendSatelliteSettings passed to NEXUS_Frontend_TuneSatellite.
When this is set, no lock callback will be called.  Instead, the calling application is expected to
read the status with this function.  Once detectionComplete is set in pStatus, the operation
is complete and pStatus->timingLoopLocked can be checked to see if there is a signal.

See also:
    NEXUS_FrontendSatelliteSettings,
    NEXUS_Frontend_TuneSatellite,
    NEXUS_FrontendSatelliteSignalDetectStatus
***************************************************************************/
NEXUS_Error NEXUS_Frontend_GetSatelliteSignalDetectStatus(
    NEXUS_FrontendHandle handle,
    NEXUS_FrontendSatelliteSignalDetectStatus *pStatus /* [out] */
    );

/***************************************************************************
Summary:
Capabilities of the satellite device

Description:
This structure contains information about the satellite frontend as a device.  This
information applies to the entire device, not to a specific channel.

The structure contains the number of ADCs and channels (demods) available on the device.

See Also:
    NEXUS_FrontendDevice_GetSatelliteCapabilities,
    NEXUS_FrontendSatelliteRuntimeSettings,
    NEXUS_Frontend_GetSatelliteRuntimeSettings,
    NEXUS_Frontend_SetSatelliteRuntimeSettings
***************************************************************************/
typedef struct NEXUS_FrontendSatelliteCapabilities
{
    unsigned numAdc;            /* number of ADCs available */
    unsigned numChannels;       /* number of demod channels available */
    bool externalBert;          /* Supports external bert */
} NEXUS_FrontendSatelliteCapabilities;


/***************************************************************************
Summary:
Retrieve the capabilities of this satellite device.
***************************************************************************/
NEXUS_Error NEXUS_FrontendDevice_GetSatelliteCapabilities(
    NEXUS_FrontendDeviceHandle handle,
    NEXUS_FrontendSatelliteCapabilities *pCapabilities
);

/***************************************************************************
Summary:
Run-time settings for a satellite channel

Description:
This structure contains runtime settings for a specific channel.  Specifically,
it contains the ADC to which this channel is currently mapped.  If used as a
parameter for NEXUS_Frontend_SetSatelliteRuntimeSettings, it will remap a channel's
ADC to the requested ADC.

ADCs are zero-indexed.  The first ADC is 0, the second is 1, etc.

Not all satellite frontends support configuring external bert (controlled by the
externalBert substructure).  Whether or not it is supported on a given satellite
device can be read via a call to NEXUS_FrontendDevice_GetSatelliteCapabilities.

When external bert is enabled, the demod specified by the frontend handle will
have its output additionally mapped to specific pins. When invertClock is set, the
clock will be inverted.

For 45216 family, external bert can only be enabled on one demod at a time, and
enabling it on one demod will disable it for any other demod that might have it
enabled.

See Also:
    NEXUS_FrontendSatelliteStatus,
    NEXUS_FrontendDevice_GetSatelliteStatus,
    NEXUS_FrontendSatelliteRuntimeSettings,
    NEXUS_Frontend_GetSatelliteRuntimeSettings,
    NEXUS_Frontend_SetSatelliteRuntimeSettings
***************************************************************************/
typedef struct NEXUS_FrontendSatelliteRuntimeSettings
{
    unsigned selectedAdc;     /* ADC mapping for this demod */
    struct {
        bool enabled;         /* Enable serial BERT output on this demod */
        bool invertClock;     /* Invert BERT_CLK output */
    } externalBert;
} NEXUS_FrontendSatelliteRuntimeSettings;

/***************************************************************************
Summary:
Get the satellite runtime configuration on this specific frontend.

Returns:
    NEXUS_SUCCESS: the current runtime settings were written into pSettings
    NEXUS_INVALID_PARAMETER: the handle was invalid or not a satellite frontend

See Also:
    NEXUS_FrontendSatelliteRuntimeSettings,
    NEXUS_Frontend_SetSatelliteRuntimeSettings
***************************************************************************/
NEXUS_Error NEXUS_Frontend_GetSatelliteRuntimeSettings(
    NEXUS_FrontendHandle handle,
    NEXUS_FrontendSatelliteRuntimeSettings *pSettings
);

/***************************************************************************
Summary:
Set the satellite runtime configuration on this specific frontend.

Returns:
    NEXUS_SUCCESS: the runtime settings in pSettings were set
    NEXUS_INVALID_PARAMETER: the handle was invalid or not a satellite frontend

See Also:
    NEXUS_FrontendSatelliteRuntimeSettings,
    NEXUS_Frontend_GetSatelliteRuntimeSettings
***************************************************************************/
NEXUS_Error NEXUS_Frontend_SetSatelliteRuntimeSettings(
    NEXUS_FrontendHandle handle,
    const NEXUS_FrontendSatelliteRuntimeSettings *pSettings
);


/***************************************************************************
Summary:
Read a satellite demod register
***************************************************************************/
#define NEXUS_Frontend_ReadSatelliteRegister NEXUS_Frontend_ReadRegister

/***************************************************************************
Summary:
Write a satellite demod register
***************************************************************************/
#define NEXUS_Frontend_WriteSatelliteRegister NEXUS_Frontend_WriteRegister

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_FRONTEND_ADD)
NEXUS_Error NEXUS_Frontend_SetStuffBytes(
	NEXUS_FrontendHandle handle,
    uint8_t *ucStuffbyte
);

NEXUS_Error NEXUS_Frontend_SetXportCtrl(
    NEXUS_FrontendHandle handle,
	bool bClkSup
);

NEXUS_Error NEXUS_Frontend_SetTsOutput(
       NEXUS_FrontendHandle handle,
	bool bEnableOutput
);
#endif

#ifdef __cplusplus
}
#endif

#endif /* #ifndef NEXUS_FRONTEND_SATELLITE_H__ */

