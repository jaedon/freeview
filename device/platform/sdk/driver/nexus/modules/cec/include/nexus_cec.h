/***************************************************************************
*	   (c)2007-2012 Broadcom Corporation
*
* This program is the proprietary software of Broadcom Corporation and/or its licensors,
* and may only be used, duplicated, modified or distributed pursuant to the terms and
* conditions of a separate, written license agreement executed between you and Broadcom
* (an "Authorized License").	Except as set forth in an Authorized License, Broadcom grants
* no license (express or implied), right to use, or waiver of any kind with respect to the
* Software, and Broadcom expressly reserves all rights in and to the Software and all
* intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
* HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
* NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
*
* Except as expressly set forth in the Authorized License,
*
* 1.	   This program, including its structure, sequence and organization, constitutes the valuable trade
* secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
* and to use this information only in connection with your use of Broadcom integrated circuit products.
*
*	2.	   TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
* AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
* WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
* THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
* OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
* LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
* OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
* USE OR PERFORMANCE OF THE SOFTWARE.
*
* 3.	   TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
* LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
* EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
* USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
* ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
* LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
* ANY LIMITED REMEDY.
*
* $brcm_Workfile: nexus_cec.h $
* $brcm_Revision: 3 $
* $brcm_Date: 9/11/12 5:06p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /nexus/modules/cec/include/nexus_cec.h $
* 
* 3   9/11/12 5:06p erickson
* SW7425-3776: merge
* 
* cng_unittest/3   8/23/12 3:38p cng
* SW7425-3776: Edit OpCode comments and renamed cecVersion in Settings
* 
* cng_unittest/2   8/22/12 1:13p cng
* SW7425-3776: Revert previous changes in settings deviceType and added
*  default deviceType in getDefaultSettings()
* 
* cng_unittest/1   8/21/12 10:24a cng
* SW7425-3776: Add CEC version support in CecSettings. Corrected value of
*  device type.
*
* SW7405-4781/1   12/21/11 12:12p vle
* SW7405-4781: allow CEC device type to be configurable
*
* 1   12/15/11 4:27p vsilyaev
* SW7425-1140: Merge NEXUS_Cec module support into main-line
*
* SW7425-1140/1   12/9/11 4:40p vle
* SW7425-1140: Add NEXUS_Cec module support
*
***************************************************************************/

#ifndef NEXUS_CEC_H__
#define NEXUS_CEC_H__


#ifdef __cplusplus
extern "C" {
#endif
/* CEC OpCodes */

#define NEXUS_CEC_OpFeatureAbort     0x00
#define NEXUS_CEC_OpImageViewOn    0x04
#define NEXUS_CEC_OpTunerStepIncrement    0x05
#define NEXUS_CEC_OpTunerStepDecrement    0x06
#define NEXUS_CEC_OpTunerDeviceStatus    0x07
#define NEXUS_CEC_OpDiveTunerDeviceStatus    0x08
#define NEXUS_CEC_OpRecordOn    0x09
#define NEXUS_CEC_OpRecordStatus    0x0A
#define NEXUS_CEC_OpRecordOff    0x0B
#define NEXUS_CEC_OpTextViewOn    0x0D
#define NEXUS_CEC_OpRecordTVScreen    0x0F
#define NEXUS_CEC_OpGiveDeckStatus    0x1A
#define NEXUS_CEC_OpDecStatus    0x1B
#define NEXUS_CEC_OpSetMenuLanguage    0x32
#define NEXUS_CEC_OpClearAnalogueTimer    0x33
#define NEXUS_CEC_OpSetAnalogueTimer   0x34
#define NEXUS_CEC_OpTimerStatus    0x35
#define NEXUS_CEC_OpStandby    0x36
#define NEXUS_CEC_OpPlay    0x41
#define NEXUS_CEC_OpDeckControl    0x42
#define NEXUS_CEC_OpTimerClearedStatus    0x43
#define NEXUS_CEC_OpUserControlPressed    0x44
#define NEXUS_CEC_OpUserControlReleased    0x45
#define NEXUS_CEC_OpGiveOSDName    0x46
#define NEXUS_CEC_OpSetOSDName    0x47
#define NEXUS_CEC_OpSetOSDString    0x64
#define NEXUS_CEC_OpSetTimerProgramTitle    0x67
#define NEXUS_CEC_OpSystemAudioModeRequest    0x70
#define NEXUS_CEC_OpGiveAudioStatus    0x71
#define NEXUS_CEC_OpSetSystemAudioMode    0x72
#define NEXUS_CEC_OpReportAudioStatus    0x7A
#define NEXUS_CEC_OpGiveSystemAudioModeStatus    0x7D
#define NEXUS_CEC_OpSystemAudioModeStatus    0x7E
#define NEXUS_CEC_OpRoutingChange    0x80
#define NEXUS_CEC_OpRoutingInformation    0x81
#define NEXUS_CEC_OpActiveSource    0x82
#define NEXUS_CEC_OpGivePhysicalAddress    0x83
#define NEXUS_CEC_OpReportPhysicalAddress    0x84
#define NEXUS_CEC_OpRequestActiveSource   0x85
#define NEXUS_CEC_OpSetStreamPath    0x86
#define NEXUS_CEC_OpDeviceVendorID    0x87
#define NEXUS_CEC_OpVendorCommand    0x89
#define NEXUS_CEC_OpVendorRemoteButtonDown    0x8A
#define NEXUS_CEC_OpVendorRemoteButtonUp    0x8A
#define NEXUS_CEC_OpGiveDeviceVendorID    0x8C
#define NEXUS_CEC_OpMenuRequest    0x8D
#define NEXUS_CEC_OpMenuStatus    0x8E
#define NEXUS_CEC_OpGiveDevicePowerStatus    0x8F
#define NEXUS_CEC_OpReportPowerStatus    0x90
#define NEXUS_CEC_OpGetMenuLanguage    0x91
#define NEXUS_CEC_OpSelectAnalogueService    0x92
#define NEXUS_CEC_OpSelectDigitalService    0x93
#define NEXUS_CEC_OpSetDigitalTimer    0x97
#define NEXUS_CEC_OpClearDigitalTimer    0x99
#define NEXUS_CEC_OpSetAudioRate   0x9A
#define NEXUS_CEC_OpInActiveSource   0x9D
#define NEXUS_CEC_OpCECVersion    0x9E
#define NEXUS_CEC_OpGetCECVersion    0x9F
#define NEXUS_CEC_OpVendorCommandWithID    0xA0
#define NEXUS_CEC_OpClearExternalTimer    0xA1
#define NEXUS_CEC_OpSetExternalTimer    0xA2
#define NEXUS_CEC_OpAbort    0xFF


#define NEXUS_CEC_MESSAGE_DATA_SIZE 16

/**
Summary:
Handle for the CEC interface.
**/
typedef struct NEXUS_Cec *NEXUS_CecHandle;


/**
Summary:
Enumeration indicate the device type
**/
typedef enum NEXUS_CecDeviceType
{
	NEXUS_CecDeviceType_eTv = 0,
	NEXUS_CecDeviceType_eRecordingDevice,
	NEXUS_CecDeviceType_eReserved,
	NEXUS_CecDeviceType_eTuner,
	NEXUS_CecDeviceType_ePlaybackDevice,
	NEXUS_CecDeviceType_eAudioSystem,
	NEXUS_CecDeviceType_ePureCecSwitch,
	NEXUS_CecDeviceType_eVideoProcessor,
	NEXUS_CecDeviceType_eMax
} NEXUS_CecDeviceType;


/**
Summary:
CEC settings
**/
typedef struct NEXUS_CecSettings
{
	bool enabled; /* enable the CEC core */
	NEXUS_CallbackDesc messageTransmittedCallback;
	NEXUS_CallbackDesc messageReceivedCallback;
	NEXUS_CallbackDesc logicalAddressAcquiredCallback;
	uint8_t physicalAddress[2];		/* device physical address: read from attached EDID */
	NEXUS_CecDeviceType deviceType;
} NEXUS_CecSettings;


/**
Summary:
Get default CEC settings
**/
void NEXUS_Cec_GetDefaultSettings(
	NEXUS_CecSettings *pSettings /* [out] */
	);


/**
Summary:
Open a CEC interface
**/
NEXUS_CecHandle NEXUS_Cec_Open( /* attr{destructor=NEXUS_Cec_Close} */
	unsigned index,
	const NEXUS_CecSettings *pSettings
	);

/**
Summary:
Close the Cec interface
**/
void NEXUS_Cec_Close(
	NEXUS_CecHandle handle
	);


/**
Summary:
Get current CEC settings
**/
void NEXUS_Cec_GetSettings(
	NEXUS_CecHandle handle,
	NEXUS_CecSettings *pSettings /* [out] */
	);


/**
Summary:
Set new CEC settings

Description:
This function is expected to call this with a new physicalAddress after a hotplug occurred on the HDMI interface.
CEC interface will then re-initialize after a hotplug.

Consult the HDMI spec for more details.
**/
NEXUS_Error NEXUS_Cec_SetSettings(
	NEXUS_CecHandle handle,
	const NEXUS_CecSettings *pSettings
	);


/**
Summary:
Status returned by NEXUS_Cec_GetStatus
**/
typedef struct NEXUS_CecStatus
{
	bool ready; /* device is ready */

	uint8_t physicalAddress[2];
	uint8_t logicalAddress;  /* Until logical address is obtained by Nexus, the CEC Send/Receive functions cannot be used.
								0xFF means no address. */
	NEXUS_CecDeviceType deviceType;

	bool messageReceived;		 /* If true, call NEXUS_Cec_ReceiveMessage to receive a message. */
	bool messageTransmitPending; /* If true, you must wait before calling NEXUS_Cec_TransmitMessage again. */

	bool transmitMessageAcknowledged; /* status from last transmitted message */
	
	unsigned cecVersion; /* Version of cec that the platform is running */

} NEXUS_CecStatus;


/**
Summary:
Get CEC status
**/
NEXUS_Error NEXUS_Cec_GetStatus(
	NEXUS_CecHandle handle,
	NEXUS_CecStatus *pStatus /* [out] */
	);


/**
Summary:
Holds data of CEC message to be transmitted or received message
**/
typedef struct NEXUS_CecMessage
{
	uint8_t initiatorAddr;
	uint8_t destinationAddr;
	uint8_t buffer[NEXUS_CEC_MESSAGE_DATA_SIZE];
	unsigned length;
} NEXUS_CecMessage;


typedef struct NEXUS_CecReceivedMessage
{
	struct {
		bool receivedMessageAcknowledged;	/* HW has acknowledged received message */
		bool endOfMessage;
	} receivedStatus;

	NEXUS_CecMessage data;
} NEXUS_CecReceivedMessage;


/**
Summary:
Get a CEC message that was received

Description:
You can only have one message pending at a time.
Fails if no message was received.

Consult the HDMI spec for the contents of the CEC message.

**/
NEXUS_Error NEXUS_Cec_ReceiveMessage(
	NEXUS_CecHandle handle,
	NEXUS_CecReceivedMessage *pReceivedMessage /* [out] */
	);


/**
Summary:
Get defaults for the structure before populating it for transmission
**/
void NEXUS_Cec_GetDefaultMessageData(
	NEXUS_CecMessage *pMessage /* [out] */
	);


/**
Summary:
Transmit CEC message

Description:
This function will fail if a previous NEXUS_Cec_TransmitMessage has not completed.
Wait for messageTransmitted callback and/or check NEXUS_CecStatus.messageTransmitPending.

Consult the HDMI spec for the contents of the CEC message.
**/
NEXUS_Error NEXUS_Cec_TransmitMessage(
	NEXUS_CecHandle handle,
	const NEXUS_CecMessage *pMessage
	);


#ifdef __cplusplus
}
#endif

#endif



