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
* $brcm_Workfile: nexus_smartcard.h $
* $brcm_Revision: 22 $
* $brcm_Date: 6/4/12 4:03p $
*
* API Description:
*   API name: Smartcard
*    Specific APIs related to SMARTCARD Control.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/smartcard/7400/include/nexus_smartcard.h $
* 
* 22   6/4/12 4:03p erickson
* SW7425-2978: Convert conditional compiler settings to runtime
*  configurable setting
* 
* 21   6/4/12 4:01p erickson
* SW7425-3109: add NEXUS_SmartcardStandard_eNordig
* 
* 20   5/24/12 11:55a mphillip
* SW7358-217, SW7425-3060: Collapse insert/remove callbacks into a single
*  smartcard callback
* 
* 19   1/24/12 2:35p erickson
* SW7405-5594: remove NEXUS_SmartcardStatus.err which was unused
* 
* 18   5/26/11 3:10p erickson
* SW7346-228: add NEXUS_SmartcardSettings.resetCycles
* 
* 17   4/11/11 4:38p erickson
* SW7346-138: add NEXUS_SmartcardSettings.atrReceiveTime
* 
* 16   4/8/11 12:39p erickson
* SW7405-5210: add NEXUS_SmartcardStatus.iccClockFrequency
*
* 15   2/16/11 1:04p erickson
* SW7420-1491: add nelem required with nelem_out
*
* 14   2/15/11 2:12p erickson
* SW7420-1491: change to nelem and nelem_out attributes
*
* 13   2/7/11 11:13a erickson
* SW7346-25: added NEXUS_SmartcardSettings.sourceClockFreq
*
* 12   2/7/11 9:51a erickson
* SW7400-2841: add NEXUS_SmartcardVcc to fix nexus/magnum mismatch
*
* 11   9/28/10 2:02p erickson
* SW7420-1123: fix attr hints for variable size param
*
* 10   8/24/10 3:37p mphillip
* SW7125-598: Add IFSD as a parameter to the settings structure
*
* 9   8/16/10 4:41p mphillip
* SW7405-4023: Add NEXUS_Smartcard_Deactivate
*
* 8   8/2/10 8:56p bandrews
* SW7125-478: Currently the ATR(answer to reset) is passed through to the
*  application. Its the resposibility of the application to decode the
*  atr. But, once decoded, the application can use
*  NEXUS_Smartcard_SetSettings() to set the desired parameters. So,
*  deleting the api NEXUS_Smartcard_SetParamsFromAtr() as it is
*  redundant. Please refer to the smartcard.c and smartcard_t1.c in
*  nexus/examples/io for further details as to how to decode the ATR.
*
* 7   1/27/10 3:24p mphillip
* SW7405-3841: Expose connect directly PI variable in Nexus
*
* 6   1/26/10 1:23p mphillip
* SW7400-2551: Add pinmux and vcc settings
*
* 5   9/30/09 2:01p erickson
* SW7400-2551: add NEXUS_Smartcard_PowerIcc
*
* 4   4/29/09 12:29p erickson
* PR54618: added NEXUS_SmartcardSettings.baudDiv
*
* 3   8/19/08 2:19p erickson
* PR45615: add smartcard settings
*
* 2   5/28/08 11:01a erickson
* PR40034: fix attr hint for kernel mode
*
* 1   5/19/08 12:02p vishk
* PR 40034: Develop Nexus Smartcard Interface/Module.
*
***************************************************************************/
#ifndef NEXUS_SMARTCARD_H__
#define NEXUS_SMARTCARD_H__

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
Summary:
Handle for the Smartcard Interface.
***************************************************************************/
typedef struct NEXUS_Smartcard *NEXUS_SmartcardHandle;

/***************************************************************************
Summary:
Smartcard state enum.

Description:
This represents the current state of the given slot and card.
***************************************************************************/
typedef enum NEXUS_SmartcardState
{
    NEXUS_SmartcardState_eUnknown = 0, /* Unknown state (perhaps not yet initialized).  Persistent. */
    NEXUS_SmartcardState_eColdResetting, /* A cold reset has been requested but is not yet complete.  Transient. */
    NEXUS_SmartcardState_eWarmResetting, /* A warm reset has been requested but is not yet complete.  Transient. */
    NEXUS_SmartcardState_eResetDone, /* The slot/card reset has completed.  Persistent. */
    NEXUS_SmartcardState_eActivating, /* The slot/card is currently being activated, but activation is not yet complete.  Transient. */
    NEXUS_SmartcardState_eReceiveDecodeAtr, /* The ATR is being received or decoded.  Transient. */
    NEXUS_SmartcardState_eReady, /* The slot/card is initialized and is awaiting sends/receives.  Persistent. */
    NEXUS_SmartcardState_eTransmitting, /* The slot/card is currently transmitting. Transient. */
    NEXUS_SmartcardState_eTransmitted, /* The slot/card has completed its transmission. Persistent. */
    NEXUS_SmartcardState_eReceiving, /* The slot/card is currently receiving.  Transient. */
    NEXUS_SmartcardState_eIgnore, /* The slot/card is ignoring events/commands. Persistent. */
    NEXUS_SmartcardState_eInitialized, /* The slot/card has been initialized, but the ATR has not yet been received.  Persistent. */
    NEXUS_SmartcardState_eMax /* A value indicating the total number of possible states.  The state returned from NEXUS_SmartCard_GetStatus should never exceed this value. */
} NEXUS_SmartcardState;

/***************************************************************************
Summary:
Status information returned by NEXUS_Smartcard_GetStatus.
***************************************************************************/
typedef struct NEXUS_SmartcardStatus
{
    bool cardPresent;
    NEXUS_SmartcardState state;
    unsigned iccClockFrequency; /* units of Hz */
} NEXUS_SmartcardStatus;

/***************************************************************************
Summary:
Smartcard protocol enum definition.
***************************************************************************/
typedef enum NEXUS_SmartcardProtocol
{
    NEXUS_SmartcardProtocol_eUnknown,
    NEXUS_SmartcardProtocol_eT0,
    NEXUS_SmartcardProtocol_eT1,
    NEXUS_SmartcardProtocol_eT14,
    NEXUS_SmartcardProtocol_eMax
} NEXUS_SmartcardProtocol;

/***************************************************************************
Summary:
Smartcard standard enum definition.

Description:
Used in NEXUS_SmartcardSettings
***************************************************************************/
typedef enum NEXUS_SmartcardStandard
{
    NEXUS_SmartcardStandard_eUnknown,
    NEXUS_SmartcardStandard_eNds,
    NEXUS_SmartcardStandard_eIso,
    NEXUS_SmartcardStandard_eEmv1996,
    NEXUS_SmartcardStandard_eEmv2000,
    NEXUS_SmartcardStandard_eIrdeto,
    NEXUS_SmartcardStandard_eArib,
    NEXUS_SmartcardStandard_eMt,
    NEXUS_SmartcardStandard_eConax,
    NEXUS_SmartcardStandard_eEs,
    NEXUS_SmartcardStandard_eNdsNoFlowControl,
    NEXUS_SmartcardStandard_eNordig,
    NEXUS_SmartcardStandard_eMax
} NEXUS_SmartcardStandard;

/***************************************************************************
Summary:
This enum is to identify the unit of timer value.
****************************************************************************/
typedef enum NEXUS_TimerUnit {
    NEXUS_TimerUnit_eEtu,       /* in Elementary Time Units */
    NEXUS_TimerUnit_eClk,       /* in raw clock cycles that smart card receives */
    NEXUS_TimerUnit_eMilliSec,  /* in milliseconds */
    NEXUS_TimerUnit_eMax
} NEXUS_TimerUnit;

/***************************************************************************
Summary:
The timer value that application set to or get from the smartcard.
****************************************************************************/
typedef struct NEXUS_TimerValue
{
    uint32_t        value;    /* timer value */
    NEXUS_TimerUnit unit;   /* units */
} NEXUS_TimerValue;

/***************************************************************************
Summary:
This enum is to identify error detect code (EDC) encoding.

Description:
This enumeration defines the supported error detect code (EDC) encoding .
****************************************************************************/
typedef enum NEXUS_EdcEncode {
    NEXUS_EdcEncode_eLrd,
    NEXUS_EdcEncode_eCrc,
    NEXUS_EdcEncode_eMax
} NEXUS_EdcEncode;

/***************************************************************************
Summary:
The configuration of EDC setting for T=1 protocol only.
****************************************************************************/
typedef struct NEXUS_EdcSetting
{
    NEXUS_EdcEncode edcEncode;   /* EDC encoding */
    bool    isEnabled;
} NEXUS_EdcSetting;

/***************************************************************************
Summary:
This enum is to identify read or write a specific register.
****************************************************************************/
typedef enum NEXUS_ScPresMode {
    NEXUS_ScPresMode_eDebounce = 0,
    NEXUS_ScPresMode_eMask,
    NEXUS_ScPresMode_eMax
}  NEXUS_ScPresMode;

/***************************************************************************
Summary:
The configuration of Smartcard Pres Debounce.
****************************************************************************/
typedef struct NEXUS_ScPresDbInfo
{
    NEXUS_ScPresMode    scPresMode;
    bool                isEnabled;
    uint8_t             dbWidth;
} NEXUS_ScPresDbInfo;

/***************************************************************************
Summary:
This enum is to identify action for NEXUS_Smartcard_ResetCard function.

Description:
This enumeration defines the supported action for NEXUS_Smartcard_ResetCard function.

See Also:
NEXUS_Smartcard_ResetCard
****************************************************************************/
typedef enum NEXUS_ResetCardAction
{
    NEXUS_ResetCardAction_eNoAction = 0,
    NEXUS_ResetCardAction_eReceiveAndDecode,
    NEXUS_ResetCardAction_eMax
} NEXUS_ResetCardAction;

/***************************************************************************
Summary:
This enum is to identify action for NEXUS_Smartcard_PowerIcc function.
****************************************************************************/
typedef enum NEXUS_SmartcardPowerIcc {
    NEXUS_SmartcardPowerIcc_ePowerDown = 0,   /* power down the ICC and request deactivation of the contact */
    NEXUS_SmartcardPowerIcc_ePowerUp = 1,    /* power up the ICC and request activation of the contact */
    NEXUS_SmartcardPowerIcc_eMax
} NEXUS_SmartcardPowerIcc;

/***************************************************************************
Summary:
Voltage level for the smartcard interface
****************************************************************************/
typedef enum NEXUS_SmartcardVcc {
    NEXUS_SmartcardVcc_e5V = 0,   /* 5v is default value */
    NEXUS_SmartcardVcc_e3V = 1,   /* 3v */
    NEXUS_SmartcardVcc_eMax
} NEXUS_SmartcardVcc;

/***************************************************************************
Summary:
Smartcard settings structure.

Description:
Smartcard settings structure, used by NEXUS_Smartcard_Open.
This allows protocol (T=0, T=1, T=14) and standard (NDS, ISO, EMV2000, IRDETO) to be selected.
***************************************************************************/
typedef struct NEXUS_SmartcardSettings
{
    NEXUS_SmartcardProtocol protocol;
    NEXUS_SmartcardStandard standard;
    uint8_t                 fFactor;
    uint8_t                 dFactor;
    uint8_t                 extClockDivisor;
    uint8_t                 txRetries;
    uint8_t                 rxRetries;
    uint8_t                 baudDiv;
    NEXUS_TimerValue        workWaitTime;
    NEXUS_TimerValue        blockWaitTime;
    NEXUS_TimerValue        extraGuardTime;
    NEXUS_TimerValue        blockGuardTime;
    uint32_t                characterWaitTime;
    NEXUS_EdcSetting        edcSetting;
    NEXUS_TimerValue        timeOut;
    bool                    autoDeactiveReq;
    bool                    nullFilter;
    NEXUS_ScPresDbInfo      scPresDbInfo;
    NEXUS_ResetCardAction   resetCardAction;
    NEXUS_TimerValue        blockWaitTimeExt;
    bool                    isPresHigh;
    NEXUS_CallbackDesc      cardCallback; /* Called when a card is either inserted or removed. This replaces insertCard and removeCard. */
    bool                    setPinmux; /* Only applies during NEXUS_Smartcard_Open. */
    bool                    connectDirectly; /* If true, smartcard is directly connected to the backend chip.
                                                If false, smartcard connected to the coupling chip TDA8024. */
    uint32_t                currentIFSD;  /* This attribute indicates the current IFSD */
    unsigned                sourceClockFreq; /* Configures the Smartcard CLK freq for each smartcard channel (slot). In units of Hz. */
    NEXUS_TimerValue        atrReceiveTime; /* Defines the time to wait for the first byte of ATR. */
    unsigned                resetCycles; /* Defines the reset cycles for the coupling chips. */
    bool                    setVcc; /* If true, Sets Vcc to vcc volts. This setting is only for TDA8024 chip. */
    NEXUS_SmartcardVcc      vcc; /* This setting applie only for TDA8024 chip. */
    struct 
    {
        bool                vccInverted;   /* If true, then the direct power supply logic is inverted. That is,sc_vcc=1 will set smartcard VCC pin to low. Default is false. */
        bool                resetInverted; /* If true, then the smartcard reset is inverted. That is,sc_rst=1 will set smartcard reset pin to low. Default is false. */
    } directPowerSupply;                   /* This applies only to power supply connected directly to the smartcard and NOT using TDA8024. */
} NEXUS_SmartcardSettings;

/***************************************************************************
Summary:
Get default settings for the structure.

Description:
This is required in order to make application code resilient to the addition of new strucutre members in the future.

See Also:
NEXUS_Smartcard_Open
***************************************************************************/
void NEXUS_Smartcard_GetDefaultSettings(
    NEXUS_SmartcardSettings *pSettings /* [out] */
    );

/***************************************************************************
Summary:
Open a Smartcard handle.

Description:

See Also:
NEXUS_Smartcard_Close
***************************************************************************/
NEXUS_SmartcardHandle NEXUS_Smartcard_Open( /* attr{destructor=NEXUS_Smartcard_Close}  */
    unsigned index,
    const NEXUS_SmartcardSettings *pSettings
    );

/***************************************************************************
Summary:
Close a Smartcard handle.

Description:

See Also:
NEXUS_Smartcard_Open
***************************************************************************/
void NEXUS_Smartcard_Close(
    NEXUS_SmartcardHandle handle
    );

/***************************************************************************
Summary:
Get the settings for a SMARTCARD
***************************************************************************/
void NEXUS_Smartcard_GetSettings(
    NEXUS_SmartcardHandle handle,
    NEXUS_SmartcardSettings *pSettings   /* [out] */
    );

/***************************************************************************
Summary:
Set the settings for a SMARTCARD
***************************************************************************/
NEXUS_Error NEXUS_Smartcard_SetSettings(
    NEXUS_SmartcardHandle handle,
    const NEXUS_SmartcardSettings *pSettings
    );

/***************************************************************************
Summary:
Read data from the smartcard.

Description:
If read fails, you should call NEXUS_Smartcard_GetStatus in order to determine
the state of the smartcard interface.

See Also:
NEXUS_Smartcard_Write
***************************************************************************/
NEXUS_Error NEXUS_Smartcard_Read(
    NEXUS_SmartcardHandle smartcard,    /* handle returned by NEXUS_SmartCard_Open */
    void *pdata,            /* [out] attr{nelem=numBytes;nelem_out=pBytesRead;reserved=64} memory to read into */
    size_t numBytes,        /* maximum number of bytes to read */
    size_t *pBytesRead      /* [out] amount of data read into memory */
    );

/***************************************************************************
Summary:
Write data to a smartcard.

Description:
If write fails, you should call NEXUS_Smartcard_GetStatus in order to determine
the state of the smartcard interface.

See Also:
NEXUS_Smartcard_Read
***************************************************************************/
NEXUS_Error NEXUS_Smartcard_Write(
    NEXUS_SmartcardHandle smartcard,
    const void *pData, /* attr{nelem=numBytes;reserved=64} */
    size_t numBytes,
    size_t *pBytesWritten /* [out] */
    );

/***************************************************************************
Summary:
Get the current status of a smartcard.
***************************************************************************/
NEXUS_Error NEXUS_Smartcard_GetStatus(
    NEXUS_SmartcardHandle smartcard,
    NEXUS_SmartcardStatus *pStatus   /* [out] */
    );

/***************************************************************************
Summary:
Reset a smartcard.

Description:
Reset the smartcard itself.

See Also:
NEXUS_Smartcard_Read
***************************************************************************/
NEXUS_Error NEXUS_Smartcard_ResetCard(
    NEXUS_SmartcardHandle smartcard, /* handle returned by NEXUS_SmartCard_Open */
    void *pdata,            /* [out] attr{nelem=numBytes;nelem_out=pBytesRead;reserved=64} pointer to memory that can be read into */
    size_t numBytes,        /* maximum number of bytes pointed to by data */
    size_t *pBytesRead      /* [out] length of data read into the data field. */
    );

/***************************************************************************
Summary:
Reset the smartcard interface.

Description:
Reprogram all the Broadcom smartcard interface, not the card.
If you want to reset the card, use NEXUS_Smartcard_ResetCard.

The interface must be reset whenever a card is inserted.

See Also:
NEXUS_Smartcard_Read
***************************************************************************/
NEXUS_Error NEXUS_Smartcard_Reset(
    NEXUS_SmartcardHandle smartcard, /* handle returned by NEXUS_SmartCard_Open */
    bool warmReset /* true for a warm reset, false for a cold reset */
    );

/***************************************************************************
Summary:
Detect the card insertion.

Description:
The function will be blocked until the card is inserted.

See Also:
NEXUS_Smartcard_Read
***************************************************************************/
NEXUS_Error NEXUS_Smartcard_DetectCard(
    NEXUS_SmartcardHandle smartcard
    );

/***************************************************************************
Summary:
Power ICC

Description:
If iccAction is NEXUS_SmartcardPowerIcc_ePowerUp, this function shall set SC_VCC
high.  System should call this function so that the next NEXUS_Smartcard_GetStatus
will show the correct presence of the card or next NEXUS_Smartcard_DetectCard
will response correctly after TDA8004 emergency deactivation.

If iccAction is NEXUS_SmartcardPowerIcc_ePowerDown, this function shall set SC_VCC
low.  The next NEXUS_Smartcard_GetStatus may not show the correct presence of the
card or next NEXUS_Smartcard_DetectCard may not response correctly after
TDA8004 emergency deactivation.
**************************************************************************/
NEXUS_Error NEXUS_Smartcard_PowerIcc(
    NEXUS_SmartcardHandle      smartcard,
    NEXUS_SmartcardPowerIcc    iccAction
    );

/***************************************************************************
Summary:
Deactivate the smartcard.

Description:
This function is used to deactivate the smartcard. After deactivation, the smartcard
needs to be reset in order to communicate with it again.

See Also:
NEXUS_Smartcard_Read
***************************************************************************/
NEXUS_Error NEXUS_Smartcard_Deactivate (
    NEXUS_SmartcardHandle smartcard
    );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef NEXUS_SMARTCARD_H__ */

