/***************************************************************************
 *     (c)2007-2012 Broadcom Corporation
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
 * $brcm_Workfile: nexus_hdmi_output_hdcp.h $
 * $brcm_Revision: 11 $
 * $brcm_Date: 7/18/12 1:04p $
 *
 * Module Description:
 *                      HdmiOutput: Specific interfaces for an HDMI/DVI output.
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/hdmi_output/include/nexus_hdmi_output_hdcp.h $
 * 
 * 11   7/18/12 1:04p erickson
 * SW7425-3060: clarify use of successCallback and failureCallback and
 *  possible race conditions
 * 
 * 10   4/30/12 11:16a erickson
 * SW7425-2666, SW7405-5150: merge
 * 
 * SW7425-2666_FifoTest/2   4/27/12 3:02p vle
 * SW7425-2666, SW7405-5150: Add additional state waitForValidVideo to
 *  HDCP state machine and add configurable delay for reading KVS List in
 *  addition to the required 5seconds time out. - Take 2
 * 
 * SW7425-2666_FifoTest/1   4/25/12 5:17p vle
 * SW7425-2666, SW7405-5150: Add additional state waitForValidVideo to
 *  HDCP state machine and add configurable delay for reading KVS List in
 *  addition to the required 5seconds time out.
 * 
 * 9   4/18/11 12:22p erickson
 * SW7422-186: merge
 * 
 * SW7422-186/2   4/15/11 2:56p rgreen
 * SW7422-186: Add API comments
 * 
 * SW7422-186/1   4/11/11 2:59p rgreen
 * SW7422-186:  Add HDCP Repeater Support
 * 
 * 8   3/3/10 9:37a erickson
 * SW7405-3987: merge
 * 
 * SW7405-3987/1   3/2/10 5:39p vle
 * SW7405-3987: Report specific HDCP error status to high level
 *  application.
 * 
 * 7   1/27/10 1:12p erickson
 * SW7420-489: merge
 *
 * SW7420-489/1   1/26/10 3:39p vle
 * SW7420-489: propagate max_device_count and max_depth to application
 *
 * 6   11/26/08 4:51p erickson
 * PR49583: merge
 *
 * PR49583/1   11/24/08 9:27p vle
 * PR49583: Fix HDCP certification issue.
 *
 * 5   4/2/08 6:46p jgarrett
 * PR 41233: Moving PJ checking to HDCP
 *
 * 4   3/14/08 8:31p ptimariu
 * PR39017: add extern "C" guards
 *
 * wince_7400/1   3/14/08 8:27p ptimariu
 * add extern "C" guards
 *
 * 3   2/29/08 1:57p erickson
 * PR38682: fix 7405 kernel mode build
 *
 * 2   2/29/08 10:12a jgarrett
 * PR 39017: Adding HDCP failed and success callbacks
 *
 * 1   2/21/08 5:59p jgarrett
 * PR 39017: Adding HDCP
 *
 **************************************************************************/
#ifndef NEXUS_HDMI_OUTPUT_HDCP_H__
#define NEXUS_HDMI_OUTPUT_HDCP_H__

#include "nexus_hdmi_output.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
Summary:
Enumeration for HDCP An Values

Description:
The DVI/HDMI transmitter is capable of generating a psuedo-random number (An)
which is used as a initial seed for the HDCP calculations.  This Enumerated
Type specifies the type of An value which can be generated.
**/
typedef enum NEXUS_HdmiOutputHdcpAnValue
{
    NEXUS_HdmiOutputHdcpAnValue_eTestA1B1, /* generate fixed A1/B1 HDCP Spec An value */
    NEXUS_HdmiOutputHdcpAnValue_eTestA1B2, /* generate fixed A1/B2 HDCP Spec An value */
    NEXUS_HdmiOutputHdcpAnValue_eTestA2B1, /* generate fixed A2/B1 HDCP Spec An value */
    NEXUS_HdmiOutputHdcpAnValue_eTestA2B2, /* generate fixed A2/B2 HDCP Spec An value */
    NEXUS_HdmiOutputHdcpAnValue_eRandom    /* generate random An value */
} NEXUS_HdmiOutputHdcpAnValue;

/**
Summary:
Structure for an HDCP KSV key
**/
#define NEXUS_HDMI_OUTPUT_HDCP_KSV_LENGTH 5
typedef struct NEXUS_HdmiOutputHdcpKsv
{
    uint8_t data[NEXUS_HDMI_OUTPUT_HDCP_KSV_LENGTH];
} NEXUS_HdmiOutputHdcpKsv;

/**
Summary:
Structure for an encrypted HDCP key
**/
#define NEXUS_HDMI_OUTPUT_HDCP_NUM_KEYS 40
typedef struct NEXUS_HdmiOutputHdcpKey
{
    uint8_t  alg;
    uint8_t  custKeyVarL;
    uint8_t  custKeyVarH;
    uint8_t  custKeySel;
    uint32_t caDataLo;
    uint32_t caDataHi;
    uint32_t tCaDataLo;
    uint32_t tCaDataHi;
    uint32_t hdcpKeyLo;
    uint32_t hdcpKeyHi;
} NEXUS_HdmiOutputHdcpKey;

/**
Summary:
HDCP Settings

Description:
By default, this structure will contain a test key set.  The test key set
will not authenticate with production devices.  For production devices, you
must obtain a production key set that can be loaded via this routine.
Please contact your FAE for more details on this process.
**/
typedef struct NEXUS_HdmiOutputHdcpSettings
{
    NEXUS_HdmiOutputHdcpAnValue anValue;
    NEXUS_HdmiOutputHdcpKsv aksv;
    NEXUS_HdmiOutputHdcpKey encryptedKeySet[NEXUS_HDMI_OUTPUT_HDCP_NUM_KEYS];

    bool transmitEncrypted;                     /* If true, transmission will be encrypted once authenticated */
    bool pjCheckEnabled;                        /* If true, the HDCP Pj key will be checked */

    NEXUS_CallbackDesc successCallback;         /* Called when authentication succeeds. It is recommended that NEXUS_HdmiOutputHdcpStatus.hdcpState be tested in the callback
                                                   to prevent race conditions; however the HDCP protocol should ensure that state changes not occur faster than 2 seconds. */
    NEXUS_CallbackDesc failureCallback;         /* Called if authentication fails. It is recommended that NEXUS_HdmiOutputHdcpStatus.hdcpState be tested in the callback
                                                   to prevent race conditions; however the HDCP protocol should ensure that state changes not occur faster than 2 seconds. */
    NEXUS_CallbackDesc stateChangedCallback;    /* Called on all state transitions */
  
    unsigned waitForValidVideo;                 /* Wait time for valid video before starting authentication. units in milliseconds. */
    unsigned waitForRxR0Margin;                 /* Additional wait time to read receiver R0' (in addition to the required 100ms). units in milliseconds. */
    unsigned waitForKsvFifoMargin;              /* Additional wait time for KSV List ready (in addition to the required 5secs). units in milliseconds. */
    
    unsigned maxDeviceCountSupported;            /* Max downstream device count supported */
    unsigned maxDepthSupported;                  /* Max depth supported */

} NEXUS_HdmiOutputHdcpSettings;

/**
Summary:
Get HDCP Settings
**/
void NEXUS_HdmiOutput_GetHdcpSettings(
    NEXUS_HdmiOutputHandle handle,
    NEXUS_HdmiOutputHdcpSettings *pSettings /* [out] */
    );

/**
Summary:
Set HDCP Settings
**/
NEXUS_Error NEXUS_HdmiOutput_SetHdcpSettings(
    NEXUS_HdmiOutputHandle handle,
    const NEXUS_HdmiOutputHdcpSettings *pSettings
    );

/**
Summary:
Establish list of Revoked KSV's
**/
NEXUS_Error NEXUS_HdmiOutput_SetHdcpRevokedKsvs(
    NEXUS_HdmiOutputHandle handle,
    const NEXUS_HdmiOutputHdcpKsv *pRevokedKsvs,    /* attr{nelem=numKsvs;nelem_convert=NEXUS_P_HDMI_OUTPUT_HDCP_KSV_SIZE} array of revoked ksv's */
    uint16_t numKsvs                                /* Number of ksvs in the array provided */
    );

/**
Summary:
Initiate HDCP authentication

Description:
Calls to NEXUS_HdmiOutput_SetHdcpSettings() and NEXUS_HdmiOutput_SetHdcpRevokedKsvs()
should be made prior to starting authentication.

See Also:
NEXUS_HdmiOutput_SetHdcpSettings
NEXUS_HdmiOutput_SetHdcpRevokedKsvs
**/
NEXUS_Error NEXUS_HdmiOutput_StartHdcpAuthentication(
    NEXUS_HdmiOutputHandle handle
    );

/**
Summary:
Terminate HDCP authentication
**/
NEXUS_Error NEXUS_HdmiOutput_DisableHdcpAuthentication(
    NEXUS_HdmiOutputHandle handle
    );

/**
Summary:
Enumeration for HDCP states
**/
typedef enum NEXUS_HdmiOutputHdcpState
{
    NEXUS_HdmiOutputHdcpState_eUnpowered,
    NEXUS_HdmiOutputHdcpState_eUnauthenticated,
    NEXUS_HdmiOutputHdcpState_eWaitForValidVideo,
    NEXUS_HdmiOutputHdcpState_eInitializedAuthentication,
    NEXUS_HdmiOutputHdcpState_eWaitForReceiverAuthentication,
    NEXUS_HdmiOutputHdcpState_eReceiverR0Ready,
    NEXUS_HdmiOutputHdcpState_eR0LinkFailure,
    NEXUS_HdmiOutputHdcpState_eReceiverAuthenticated,
    NEXUS_HdmiOutputHdcpState_eWaitForRepeaterReady,
    NEXUS_HdmiOutputHdcpState_eCheckForRepeaterReady,
    NEXUS_HdmiOutputHdcpState_eRepeaterReady,
    NEXUS_HdmiOutputHdcpState_eLinkAuthenticated,      /* Includes down stream devices */
    NEXUS_HdmiOutputHdcpState_eEncryptionEnabled,
    NEXUS_HdmiOutputHdcpState_eRepeaterAuthenticationFailure,
    NEXUS_HdmiOutputHdcpState_eRiLinkIntegrityFailure,
    NEXUS_HdmiOutputHdcpState_ePjLinkIntegrityFailure,
    NEXUS_HdmiOutputHdcpState_eMax
} NEXUS_HdmiOutputHdcpState;


/**
Summary:
Enumeration for HDCP errors
**/
typedef enum NEXUS_HdmiOutputHdcpError
{
    NEXUS_HdmiOutputHdcpError_eSuccess,
    NEXUS_HdmiOutputHdcpError_eRxBksvError,
    NEXUS_HdmiOutputHdcpError_eRxBksvRevoked,
    NEXUS_HdmiOutputHdcpError_eRxBksvI2cReadError,
    NEXUS_HdmiOutputHdcpError_eTxAksvError,
    NEXUS_HdmiOutputHdcpError_eTxAksvI2cWriteError,
    NEXUS_HdmiOutputHdcpError_eReceiverAuthenticationError,
    NEXUS_HdmiOutputHdcpError_eRepeaterAuthenticationError,
    NEXUS_HdmiOutputHdcpError_eRxDevicesExceeded,
    NEXUS_HdmiOutputHdcpError_eRepeaterDepthExceeded,
    NEXUS_HdmiOutputHdcpError_eRepeaterFifoNotReady,
    NEXUS_HdmiOutputHdcpError_eRepeaterDeviceCount0,
    NEXUS_HdmiOutputHdcpError_eRepeaterLinkFailure,     /* Repeater Error */    
    NEXUS_HdmiOutputHdcpError_eLinkRiFailure,
    NEXUS_HdmiOutputHdcpError_eLinkPjFailure,
    NEXUS_HdmiOutputHdcpError_eFifoUnderflow,
    NEXUS_HdmiOutputHdcpError_eFifoOverflow,
    NEXUS_HdmiOutputHdcpError_eMultipleAnRequest,
    NEXUS_HdmiOutputHdcpError_eMax
} NEXUS_HdmiOutputHdcpError;


/**
Summary:
HDCP Status
**/
typedef struct NEXUS_HdmiOutputHdcpStatus
{
    NEXUS_HdmiOutputHdcpState hdcpState;    /* Current HDCP State */
    NEXUS_HdmiOutputHdcpError hdcpError;    /* Last HDCP error */   

    bool linkReadyForEncryption;    /* Link is ready for encryption */
    bool transmittingEncrypted;     /* Encryption is enabled */

    bool isHdcpRepeater;            /* Receiver is a repeater */
    bool ksvFifoReady;              /* Receiver ksv FIFO is ready */
    bool i2c400Support;             /* Receiver supports 400kHz I2C accesses */
    bool hdcp1_1Features;           /* Receiver supports HDCP 1.1 features {EESS, Advance Cipher, Enhanced Link Verification} */
    bool fastReauthentication;      /* Receiver can receive un-encrypted video during re-authentication */

    NEXUS_HdmiOutputHdcpKsv bksv;   /* Receiver's Bksv value */
} NEXUS_HdmiOutputHdcpStatus;

/**
Summary:
Get HDCP Status
**/
NEXUS_Error NEXUS_HdmiOutput_GetHdcpStatus(
    NEXUS_HdmiOutputHandle handle,
    NEXUS_HdmiOutputHdcpStatus *pStatus /* [out] */
    );

/**
Summary:
Get HDCP info for Downstream Devices connected to the hdmi transmitter
**/
NEXUS_Error NEXUS_HdmiOuput_HdcpGetDownstreamInfo(
    NEXUS_HdmiOutputHandle handle,
    NEXUS_HdmiHdcpDownStreamInfo *pDownstream  /* [out] */
    );

/**
Summary:
Get the HDCP KSVs of the down stream devices connected to the hdmi transmitter
**/
NEXUS_Error NEXUS_HdmiOuput_HdcpGetDownstreamKsvs(
    NEXUS_HdmiOutputHandle handle,
    NEXUS_HdmiHdcpKsv *pKsvs, /* attr{nelem=numDevices;nelem_out=pNumRead} */
    unsigned numDevices,
    unsigned *pNumRead /* [out] */
    );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef NEXUS_HDMI_OUTPUT_HDCP_H__ */

