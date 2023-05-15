/******************************************************************************
 *     (c)2010-2011 Broadcom Corporation
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
 * $brcm_Workfile: nexus_hdmi_input_hdcp_types.h $
 * $brcm_Revision: 4 $
 * $brcm_Date: 4/18/11 3:33p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/hdmi_input/3548/include/nexus_hdmi_input_hdcp_types.h $
 * 
 * 4   4/18/11 3:33p erickson
 * SW7422-186: update
 * 
 * 3   4/18/11 12:22p erickson
 * SW7422-186: merge
 * 
 * SW7422-186/7   4/15/11 6:08p rgreen
 * SW7422-186: Remove hdcpKeyset from hdmiInputSettings; Add to  hdmiInput
 *  handle
 * 
 * SW7422-186/6   4/15/11 2:56p rgreen
 * SW7422-186:  NEXUS API Review updates
 * 
 * SW7422-186/5   4/13/11 1:55p rgreen
 * SW7422-186: Replace RequestKsvFifoEvent with generic hdcpEvent
 * 
 * SW7422-186/4   4/11/11 2:55p rgreen
 * SW7422-186: Remove rundant bRepeater declaration
 * 
 * SW7422-186/3   4/7/11 4:02p rgreen
 * SW7422-186: Move shared HDMI HDCP declaration to neuxus_hdmi_types.h
 * 
 * SW7422-186/2   3/31/11 2:38p rgreen
 * SW7422-186: Add HDCP Repeater support
 * 
 * SW7422-186/1   3/28/11 5:38p rgreen
 * SW7422-186: Add HDCP Repeater Support
 * 
 * 2   3/3/11 2:41p jhaberf
 * SWDTV-4664: Update HDCP Key Set verification for OTP ROM based chips
 * 
 * 1   2/16/11 5:21p erickson
 * SW7422-129: merge
 *
 * SW7422-129/1   2/10/11 3:52p rgreen
 * SW7422-129:Add nexus hdmi hdcp types
 *
 *****************************************************************************/
#ifndef NEXUS_HDMI_INPUT_HDCP_TYPES_H__
#define NEXUS_HDMI_INPUT_HDCP_TYPES_H__

#include "nexus_hdmi_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
Summary:
Structure for an HDCP KSV key
**/
typedef struct NEXUS_HdmiInputHdcpKsv
{
    uint8_t data[NEXUS_HDMI_HDCP_KSV_LENGTH];
} NEXUS_HdmiInputHdcpKsv;

/**
Summary:
Structure for an encrypted HDCP key
**/
typedef struct NEXUS_HdmiInputHdcpKey
{
    uint32_t caDataLo;
    uint32_t caDataHi;
    uint32_t tCaDataLo;
    uint32_t tCaDataHi;
    uint32_t hdcpKeyLo;
    uint32_t hdcpKeyHi;
} NEXUS_HdmiInputHdcpKey;


/**
Summary:
HDMI HDCP Encrypted KeySet declatation 
primarily used when loading/decrypting keys from external storage 
to secure on-chip RAM
**/
typedef struct NEXUS_HdmiInputHdcpKeyset
{
    NEXUS_HdmiInputHdcpKsv rxBksv;  /* Ksv are shown in the clear */
    uint8_t  alg;
    uint8_t  custKeyVarL;
    uint8_t  custKeyVarH;
    uint8_t  custKeySel;
    NEXUS_HdmiInputHdcpKey privateKey[NEXUS_HDMI_HDCP_NUM_KEYS];
} NEXUS_HdmiInputHdcpKeyset ;


/**
Summary:
HDMI Input HDCP Settings
**/
typedef struct NEXUS_HdmiInputHdcpSettings
{
    NEXUS_CallbackDesc hdcpRxChanged ;  /* Called on all state transitions */

    bool repeater ;
    unsigned maxDeviceCountSupported;            /* Max downstream device count supported */
    unsigned maxDepthSupported;                  /* Max depth supported */

} NEXUS_HdmiInputHdcpSettings;



/**
Summary:
HDMI HDCP KeySet Storage Type
**/
typedef enum NEXUS_HdmiInputHdcpKeyStorage
{
    NEXUS_HdmiInputHdcpKeyStorage_eNone,
    NEXUS_HdmiInputHdcpKeyStorage_eOtpROM,
    NEXUS_HdmiInputHdcpKeyStorage_eOtpRAM,
    NEXUS_HdmiInputHdcpKeyStorage_eMax
} NEXUS_HdmiInputHdcpKeyStorage ;



/**
Summary:
HDMI HDCP KeySet OTP Status
**/
typedef enum NEXUS_HdmiInputHdcpKeySetOtpState
{
    NEXUS_HdmiInputHdcpKeySetOtpState_eNoOtpSupport,
    NEXUS_HdmiInputHdcpKeySetOtpState_eHwError,
    NEXUS_HdmiInputHdcpKeySetOtpState_eCalcInitialized,
    NEXUS_HdmiInputHdcpKeySetOtpState_eCalcRunning,
    NEXUS_HdmiInputHdcpKeySetOtpState_eCrcMatch,
    NEXUS_HdmiInputHdcpKeySetOtpState_eCrcMismatch,
    NEXUS_HdmiInputHdcpKeySetOtpState_eMax
} NEXUS_HdmiInputHdcpKeySetOtpState;


/**
Summary:
HDMI HDCP  Authentication State
**/
typedef enum NEXUS_HdmiInputHdcpAuthState
{
	NEXUS_HdmiInputHdcpAuthState_eKeysetInitialization,
	NEXUS_HdmiInputHdcpAuthState_eKeysetError,
	NEXUS_HdmiInputHdcpAuthState_eIdle,
	NEXUS_HdmiInputHdcpAuthState_eWaitForKeyloading,
	NEXUS_HdmiInputHdcpAuthState_eWaitForDownstreamKsvs,
	NEXUS_HdmiInputHdcpAuthState_eKsvFifoReady,
	NEXUS_HdmiInputHdcpAuthState_eCalcRunning,
	NEXUS_HdmiInputHdcpAuthState_eCrcMatch,
	NEXUS_HdmiInputHdcpAuthState_eCrcMismatch,
	NEXUS_HdmiInputHdcpAuthState_eMax 
} NEXUS_HdmiInputHdcpAuthState ;



/**
Summary:
HDMI HDCP  Status
**/
typedef struct NEXUS_HdmiInputHdcpStatus
{
    NEXUS_HdmiInputHdcpKeyStorage eKeyStorage;
    NEXUS_HdmiInputHdcpKeySetOtpState eOtpState;
    NEXUS_HdmiInputHdcpAuthState eAuthState ;
    uint32_t programmedCrc;
    uint32_t computedCrc;
    uint8_t anValue [NEXUS_HDMI_HDCP_AN_LENGTH] ;
    uint8_t aksvValue [NEXUS_HDMI_HDCP_KSV_LENGTH] ;
    uint8_t bksvValue [NEXUS_HDMI_HDCP_KSV_LENGTH] ;
} NEXUS_HdmiInputHdcpStatus;




#ifdef __cplusplus
}
#endif

#endif
