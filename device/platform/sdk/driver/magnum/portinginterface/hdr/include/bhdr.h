/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhdr.h $
 * $brcm_Revision: Hydra_Software_Devel/35 $
 * $brcm_Date: 3/28/12 1:55p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hdr/3548/bhdr.h $
 * 
 * Hydra_Software_Devel/35   3/28/12 1:55p rgreen
 * SW7425-2515: Remove CEC support.  See CEC pi for CEC support
 * 
 * Hydra_Software_Devel/34   11/2/11 4:37p rgreen
 * SW7425-1351:  Verify/Warn if app tries to load HDCP Test Rx Keyset
 * (BHDR_HDCP_KSV_ERROR);
 * 
 * Hydra_Software_Devel/33   10/31/11 6:14p rgreen
 * SW7425-1351:  Update key loading references to refer to Key RAM vs OTP
 * ROM.
 * 
 * Hydra_Software_Devel/32   8/17/11 3:38p rgreen
 * SWDTV-8288,SW7425-991:   Add BHDR_Get/SetSettings; Move 3548 phy
 * specific code to private files; Add hw passthrough support for 7425
 * 
 * Hydra_Software_Devel/31   6/22/11 11:13a rgreen
 * SWDTV-6771: Add compile time option support for ISRC packets
 * 
 * Hydra_Software_Devel/30   4/18/11 11:42a rgreen
 * SW7422-186: Merge changes for HDCP Repeater Support
 * 
 * Hydra_Software_Devel/SW7422-186/2   4/11/11 2:54p rgreen
 * SW7422-186: Add HDCP Repeater Support
 * 
 * Hydra_Software_Devel/SW7422-186/1   3/31/11 2:36p rgreen
 * SW7422-186: Add HDCP Repeater support
 * 
 * Hydra_Software_Devel/29   1/31/11 7:28p rgreen
 * SW7422-129: Remove unused structures
 * 
 * Hydra_Software_Devel/28   12/7/10 7:46p rgreen
 * SW7422-129: Add support for HDCP Rx Key Loading
 * 
 * Hydra_Software_Devel/27   12/2/10 2:43p rgreen
 * SW35230-2374:Initial support for HDCP Key OTP check
 * 
 * Hydra_Software_Devel/26   2/19/10 2:45p rgreen
 * SW3548-2642: add HDCP consistency check when both cores are used for
 * 3D.  Disable interrupts on the core slaved for 3D operation
 * 
 * Hydra_Software_Devel/25   12/31/09 12:44p rgreen
 * SW3548-2013: Always update 'HDCP Ri Updating' status regardless of
 * bhdr_config setting
 * 
 * Hydra_Software_Devel/24   6/18/09 10:45a rgreen
 * PR56097:  Add flag to indicate validity of channel status information
 * returned by BHDR_GetHdmiRxStatus()
 * 
 * Hydra_Software_Devel/23   4/8/09 4:33p rgreen
 * PR52879: Add support for on-chip EDID RAM
 * 
 * Hydra_Software_Devel/22   3/23/09 1:20p rgreen
 * PR52879: Additional support for EDID RAM
 * 
 * Hydra_Software_Devel/21   2/26/09 7:56p rgreen
 * PR51307: Create API to load Repeater KSV FIFO; Add compile time option
 * for repeater support
 * 
 * Hydra_Software_Devel/20   2/9/09 3:53p rgreen
 * PR51752:
 * Add bValidStatus flag to indicate validity of HDMI Rx Status
 * 
 * Hydra_Software_Devel/19   1/26/09 5:26p rgreen
 * PR51601: Add Hot Plug isr callback
 * 
 * Hydra_Software_Devel/PR51307/1   1/20/09 7:39p honglang
 * PR51307: Create API to load Repeater KSV FIFO
 * 
 * Hydra_Software_Devel/18   12/16/08 2:23p rgreen
 * PR49443: Add setting top BHDR_Open to enable/disable I2C pullup
 * resistor.
 * 
 * Hydra_Software_Devel/17   12/5/08 6:56p rgreen
 * PR49943: Add support for i2c pullup configuration
 * 
 * Hydra_Software_Devel/16   9/17/08 9:36p rgreen
 * PR45761: provide GCP and audio clock regeneration packet to the high
 * level application
 * 
 * Hydra_Software_Devel/PR45761/1   9/17/08 5:19p honglang
 * PR45761: provide GCP and audio clock regeneration packet to the high
 * level application
 * 
 * Hydra_Software_Devel/15   9/11/08 5:46p rgreen
 * PR45482: PR45358,PR44257: Merge Changes
 * 
 * Hydra_Software_Devel/PR44257/PR45482/2   9/11/08 5:45p rgreen
 * PR45358,PR44257: Remove BHDR_CEC_SUPPORT defines/macros.  CEC enabled
 * by default
 * 
 * Hydra_Software_Devel/PR44257/PR45482/1   8/7/08 1:59p honglang
 * PR44257,PR45482: support CEC on 3549/56 HDMI input
 * 
 * Hydra_Software_Devel/PR44257/2   7/29/08 3:16p vle
 * PR44257: Fix to avoid compiler warning
 * 
 * Hydra_Software_Devel/PR44257/1   7/21/08 6:20p vle
 * PR44257: Add CEC support for 3549/3556
 * 
 * Hydra_Software_Devel/14   7/10/08 11:56a rgreen
 * PR44523: Add support for Vendor Specific Infoframe
 * 
 * Hydra_Software_Devel/PR44523/1   7/8/08 4:48p rgreen
 * PR44523: Add support for Vendor Specific Infoframe
 * 
 * Hydra_Software_Devel/13   6/24/08 3:57p rgreen
 * PR42977:
 * Remove unused references; Manual SyncToVideo control no longer needed
 * 
 * Hydra_Software_Devel/12   6/16/08 7:31p rgreen
 * PR38947:Remove Install/Uninstall HDCP Key loader function.  Using OTP
 * instead
 * 
 * Hydra_Software_Devel/11   6/13/08 6:27p rgreen
 * PR38947,PR39463: Remove BHDR_ProcessReceivedPackets... all packets
 * processed at Isr time
 * 
 * Hydra_Software_Devel/10   5/16/08 4:30p rgreen
 * PR38947,PR39463: Add declarations for Install/Unintall
 * PacketChangeChange Callbacks
 * 
 * Hydra_Software_Devel/9   5/15/08 5:45p rgreen
 * PR38947,PR39463:
 * Remove Packet stopped / update events
 * Remove audio Error event
 * Remove HDMI 1.3 macro (all 35xx support 1.3)
 * 
 * Hydra_Software_Devel/8   5/9/08 4:20p rgreen
 * PR38957,PR39463:Fix sample code; fix structure declaration for hTmr
 * 
 * Hydra_Software_Devel/7   5/7/08 1:51p rgreen
 * PR38957,PR39463:
 * Replace BHDR_FE_GetEventHandle with Install/Uninstall Callbacks for
 * Front End Events (Hotplug and CEC)
 * Move HP Initialization for bhdr_fe file
 * Update BHDR_ConfigureAfterHotPlug to update core after HP Event
 * 
 * Hydra_Software_Devel/6   5/6/08 3:54p rgreen
 * PR38957,PR39463: Fix declaration naming conventions; remove AudioError
 * callbacks/events
 * 
 * Hydra_Software_Devel/5   5/6/08 2:02p rgreen
 * PR38947,PR39463:
 * Remove channel handle from settings
 * 
 * Hydra_Software_Devel/4   5/2/08 5:12p rgreen
 * PR38947, PR39463: Incorporate API Review changes.
 * 
 * Hydra_Software_Devel/3   4/29/08 1:28p rgreen
 * PR38957: Fix Documentation; Remove unused API
 * 
 * Hydra_Software_Devel/2   4/22/08 8:37p rgreen
 * PR40780: Add Pixel Repeat value to HDMI Rx Status
 * 
 * Hydra_Software_Devel/1   4/7/08 11:16p rgreen
 * PR38947:Remove linkt to 356x
 * 
 ***************************************************************************/


 
/*= Module Overview *********************************************************
<verbatim>

Overview
The HDMI RX (High-Definition Multimedia Interface) API is a library used to
control/configure the HDMI Rx core. 


Sample Initialization Code 
//
// ********************************************
// Delclare / Initialize Device Handles for...
// ********************************************
BHDR_Handle     hHDR0, hHDR1 ;
BCHP_Handle     hChip ;
BREG_Handle     hRegister;
BINT_Handle     hInterrupt;


// ********************************************
// Open the Front End device which contains the  Front End Channels (ports)  
// A channel/port is the physical connection to the HDMI Core.
// ********************************************
BHDR_FE_GetDefaultSettings(hChip, &FrontEndSettings) ;
BHDR_FE_Open(&hFrontEnd, hChip, hRegister, hInterrupt, FrontEndSettings) ;

BHDR_FE_GetTotalChannels(hFrontEnd, &NumFeChannels) ;


// ********************************************
// An associated Front End Channel (port) must be opened for use with HDMI Rx device.  
// Open all available FE channels 
// ********************************************

for (i = 0 ; i < NumFeChannels; i++)
{
	BHDR_FE_GetDefaultChannelSettings(hFrontEnd, &FeChannelSettings) ;
	FeChannelSettings. uiChannel = i ;
	BHDR_FE_OpenChannel(hFrontEnd, &hFrontEndChannel[i], FeChannelSettings) ;
}



// ********************************************
// Get/Modify Default Settings for HDMI Rx; assign FE channel
// ********************************************
BHDR_GetDefaultSettings(&BHDR_Settings) ;

// timer handle for AvMute clear
BHDR_Settings.hTmr = hTmr ;

// ********************************************
// open HDMI RX Device Handle 
// ********************************************
BHDR_Open(&hHDR0, hChip, hRegister, hInterrupt, BHDR_Settings) ;
BHDR_Open(&hHDR1, hChip, hRegister, hInterrupt, BHDR_Settings) ;


// ********************************************
// Attach HDMI Rx Core to Front End Channel 
// ********************************************
rc = BHDR_FE_AttachHdmiRxCore(hFrontEndChannel[0], hHDR0 ) ;
rc = BHDR_FE_AttachHdmiRxCore(hFrontEndChannel[1], hHDR1 ) ;
.
.
.

// Sample code for  Packet Change Notification callback installation 

// changed and stopped packets will trigger the installed callback
rc = BHDR_InstallPacketChangeCallback(hHDR0, pfPacketChangeNotification_isr, 
	pvParm1, 
	0)  ;  // iParm2 will indicate Packet Type on Callback

</verbatim>
****************************************************************************/


#ifndef BHDR_H__
#define BHDR_H__

#include "bhdr_config.h"

#include "bavc.h"
#include "bavc_hdmi.h"  /* A/V constants */
#include "bacm_spdif.h"  /* Audio Common SPDIF  */

#include "bchp.h"       /* Chip Info */
#include "breg_mem.h"   /* Chip register access. */
#include "bkni.h"       /* Kernel Interface */
#include "bint.h"       /* Interrupt */

#include "btmr.h"
#include "berr_ids.h"   /* Error codes */
#include "bdbg.h"       /* Debug Support */

#include "bhdr_datatypes.h"


#ifdef __cplusplus
extern "C" {
#endif



/******************************************************************************
Summary:
HDMI Basic Return Codes
*******************************************************************************/
#define BHDR_BASE_ERRS                0

#define BHDR_NOT_IMPLEMENTED \
	BERR_MAKE_CODE(BERR_HDR_ID, BHDR_BASE_ERRS + 1)
	
#define BHDR_RX_FEATURE_NOT_SUPPORTED \
	BERR_MAKE_CODE(BERR_HDR_ID, BHDR_BASE_ERRS + 2)
	
#define BHDR_NO_RX_DEVICE \
	BERR_MAKE_CODE(BERR_HDR_ID, BHDR_BASE_ERRS + 3)
	
#define BHDR_UNSUPPORTED_VIDEO_FORMAT \
	BERR_MAKE_CODE(BERR_HDM_ID, BHDR_BASE_ERRS + 4)

#define BHDR_INVALID_RESOURCE \
	BERR_MAKE_CODE(BERR_HDR_ID, BHDR_BASE_ERRS + 5)

#define BHDR_CHANNEL_NOT_ASSIGNED  \
	BERR_MAKE_CODE(BERR_HDR_ID, BHDR_BASE_ERRS + 6)

#define BHDR_EDID_RAM_ERROR  \
	BERR_MAKE_CODE(BERR_HDR_ID, BHDR_BASE_ERRS + 7)

#define BHDR_HDCP_KSV_ERROR  \
	BERR_MAKE_CODE(BERR_HDR_ID, BHDR_BASE_ERRS + 8)

#define BHDR_HARDWARE_NOT_INITIALIZED \
	BERR_MAKE_CODE(BERR_HDR_ID, BHDR_BASE_ERRS + 9)

#define BHDR_HARDWARE_FAILURE \
	BERR_MAKE_CODE(BERR_HDR_ID, BHDR_BASE_ERRS + 10)


/******************************************************************************
Summary:
Enumerated Type of different events available from the HDMI Rx core

Description:
The HDMI core supports different  interrupts (HDMI mode, DVI mode, HDCP,  Audio updates etc.).  
Applications can request notification of events associated with these
interrupts.  This enumeration can be passed to the BHDR_GetEvent function

See Also:
	o BHDR_GetEvent

*******************************************************************************/
typedef enum
{
	BHDR_Event_DviToHdmi,
	BHDR_Event_HdmiToDvi,
	
	BHDR_Event_AudioChannelStatusUpdate,
		
	BHDR_Event_VBlankEnd

} BHDR_EventType ;




/******************************************************************************
Summary:
Enumerated Type of the level of hot plug pin. This enumeration is used to set the output of the hot 
plug pin high (attached device) or low (detached device) 

Description:
The HIGH Hot Plug signal  indicates a device is attached to transmitter.  5V was looped back to the 
transmitter through the receiver.

The LOW Hot Plug signal  indicates no device is attached to the transmitter.  The 5V was not looped 
back to teh transmitter through the receiver

   BHDR_HotPlugSignal_eLow
   BHDR_HotPlugSignal_eHigh


See Also:
	BHDR_SetHotPlug

*******************************************************************************/
typedef enum
{
	BHDR_HotPlugSignal_eLow,
	BHDR_HotPlugSignal_eHigh
} BHDR_HotPlugSignal ;



typedef enum
{
	BHDR_Usage_eNormal,
	BHDR_Usage_eMaster,
	BHDR_Usage_eSlave
} BHDR_Mode ;
		



/***************************************************************************
Summary:
	Prototype for external modules to install callback with HDMI Rx to notify of changes

Description:

Input:
	pvParam1 - User defined data structure casted to void.
	iParam2 - Additional user defined value.
	pvData - Data to pass from HDMI to application thru callback. 

Output:
	None

See Also:
	BHDR_InstallVideoFormatChangeCallback,
	BHDR_UnInstallVideoFormatChangeCallback,
	BHDR_InstallAudioChangeCallback,
	BHDR_UnInstallAudioChangeCallback
**************************************************************************/
typedef void (*BHDR_CallbackFunc)
	( void                            *pvParam1,
	  int                              iParam2,
	  void                            *pvData );


	  
/***************************************************************************
Summary:
	Prototype for external modules to use when loading HDCP Keys 

Description:

Input:
	pvParam1 - User defined data structure casted to void.
	iParam2 - Additional user defined value.
	pvData - Data to pass from HDMI to application thru callback. 

Output:
	None

See Also:
***************************************************************************/
typedef void (*BHDR_KeyLoader_CallbackFunc)
	(void *pvParam1,
	int   iParam2,
	void *pvData );

	  

/***************************************************************************
Summary:
HDMI settings used for BHDR_Open().

See Also
	o BHDR_GetDefaultSettings
****************************************************************************/
typedef struct _BHDR_Settings_
{
	uint8_t eCoreId ;	
	
	BTMR_Handle hTmr ; 
	BHDR_Handle hHDRMaster ;
	
	bool bParseAVI ;	
	bool bDisableI2cPadSclPullup ;
	bool bDisableI2cPadSdaPullup  ;
	bool bHdmiHardwarePassthrough ;

} BHDR_Settings;


typedef struct BHDR_Status
{
	bool bValidStatus ;
	uint8_t HdmiMode ;
	uint8_t DeviceAttached ;
	uint8_t PllLocked ;
	bool bPacketErrors ;
	uint16_t uiAudioPackets ;

	bool bValidChannelStatus ;
	BACM_SPDIF_ChannelStatus stChannelStatus	;
	uint32_t uiAudioSampleRateHz ; /* In Hz */

	bool bAvmute ;
	
	bool bHdcpRiUpdating ;
	bool bSymbolLoss ;
	
	bool bFormatStable ;

	uint32_t  VSyncCounter ;
	
	uint8_t PixelRepeat ;
	
 } BHDR_Status ;


/******************************************************************************
Summary:
Get the default settings for the HDMI device.

Input:
	<None>

Output:
   pBHDR_Settings  - pointer to memory to hold default settings

Returns:
	BERR_SUCCESS - Successfully opened HDMI connection.
	BERR_INVALID_PARAMETER - Invalid function parameter.

See Also:

*******************************************************************************/
BERR_Code BHDR_GetDefaultSettings(
   BHDR_Settings *pBHDR_DefaultSettings  /* [out] pointer to memory to hold default settings */
) ;




/******************************************************************************
Summary:
Open a HDMI connection to a HDMI Rx.

Input:
	hChip - The chip handle that application created ealier during chip
	initialization sequence.  This handle is use for get chip
	information, chip revision, and miscellaneous chip configurations.

	hRegister - The register handle that application created earlier during
	the chip initialization sequence.  This handle is use to access chip
	registers (HDMI registers).

	hInterrupt - The interrupt handle that the application created earler during the
	initialization sequence.  This handle is used to process interrupts.

	pBHDR_Settings - The default settings for configuring the HDMI connection. This
	sturcture can be queried prior to BHDR_Open with BHDR_GetDefaultSettings.
	This parameter can be NULL.  In this case the default setting will be used.

Output:
	phHDR - pointer to previously allocated HDMI structure

Returns:
	BERR_SUCCESS - Successfully opened HDMI connection.
	BERR_INVALID_PARAMETER - Invalid function parameter.

See Also:
	BHDR_Handle
	BREG_Handle
	BI2C_ChannelHandle

*******************************************************************************/
BERR_Code BHDR_Open(
   BHDR_Handle *phHDR,       /* [out] HDMI handle */
   BCHP_Handle hChip,         /* [in] Chip handle */
   BREG_Handle hRegister,     /* [in] Register Interface to HDMI Tx Core */
   BINT_Handle hInterrupt,    /* [in] Interrupt handle */
   const BHDR_Settings  *pBHDR_Settings /* [in] default HDMI settings */
) ;


/******************************************************************************
Summary:
Close the HDMI connection to the HDMI Rx.

Description:
This function will close the HDMI connection to the HDMI Rx.  A new HDMI
connection will have to be opened to display to the HDMI Rx again.

Input:
	hHDR - HDMI control handle that was previously opened by BHDR_Open.

Output:
	<None>

Returns:
	BERR_SUCCESS - Successfully closed the HDMI connection.
	BERR_INVALID_PARAMETER - Invalid function parameter.

See Also:
	BHDR_Handle
*******************************************************************************/
BERR_Code BHDR_Close(
   BHDR_Handle hHDR  /* [in] HDMI handle */
) ;



/***************************************************************************
Summary:
	Get the current device settings for the  HDMI Rx events

Input:
	hHDR - HDMI control handle that was previously opened by BHDR_Open.

Output:
	pHdrSettings - HDMI Rx device settings

Returns:
	BERR_SUCCESS - Sucessfully returned the HDMI handle
	BERR_INVALID_PARAMETER - Invalid function parameter.

See Also:

****************************************************************************/
BERR_Code BHDR_GetSettings(
	BHDR_Handle hHDR,            /* [in] HDMI Rx Handle */
	BHDR_Settings *pHdrSettings  /* [in] pointer to memory to hold current settings */
)  ;


/***************************************************************************
Summary:
	Set the current device settings for the  HDMI Rx events

Input:
	hHDR - HDMI control handle that was previously opened by BHDR_Open.
	pHdrSettings - HDMI Rx device settings

Output:
	none

Returns:
	BERR_SUCCESS - Sucessfully returned the HDMI handle
	BERR_INVALID_PARAMETER - Invalid function parameter.

See Also:

****************************************************************************/
BERR_Code BHDR_SetSettings(
	BHDR_Handle hHDR,            /* [in] HDMI Rx Handle */
	BHDR_Settings *pHdrSettings  /* [in] pointer to memory to hold current settings */
)  ;


/***************************************************************************
Summary:
	Get the event handle for checking HDMI Rx events.

Input:
	hHDR - HDMI control handle that was previously opened by BHDR_Open.
	EventType - Type of event the requested handle is for (HDCP, etc.)

Output:
	pBHDREvent - HDMI Event Handle

Returns:
	BERR_SUCCESS - Sucessfully returned the HDMI handle
	BERR_INVALID_PARAMETER - Invalid function parameter.

See Also:
	BHDR_EventType

****************************************************************************/
BERR_Code BHDR_GetEventHandle(
   BHDR_Handle hHDR,           /* [in] HDMI handle */
   BHDR_EventType EventType,    /* [in] HDMI Event Type */
   BKNI_EventHandle *pBHDREvent	/* [out] event handle */
);



/***************************************************************************
Summary:
	Initialize the HDMI Packet RAM

Input:
	hHDR - HDMI control handle that was previously opened by BHDR_Open.

Output:
	<None>

Returns:
	BERR_SUCCESS - Sucessfully returned the HDMI handle
	BERR_INVALID_PARAMETER - Invalid function parameter.

See Also:

****************************************************************************/
BERR_Code BHDR_InitializePacketRAM(
   BHDR_Handle hHDR          /* [in] HDMI handle */
) ;



/***************************************************************************
Summary:
	Write Data to Rx I2c Space 
	

Input:
	hHDR - HDMI control handle that was previously opened by BHDR_Open.
	offset - I2C Offset where data is to start
	pData - pointer to the data to be written
	Length - number of bytes to be written
	
    
Output:
	<None>

Returns:
	BERR_SUCCESS - Data Written  
	BERR_TIMEOUT - Timeout waiting to write/load I2c Data.

See Also:
	BHDR_WriteRxI2cRegisterSpace	

****************************************************************************/
BERR_Code BHDR_WriteRxI2cRegisterSpace(BHDR_Handle hHDR, 
	uint8_t offset, uint8_t *pData, uint8_t Length) ;

	
/***************************************************************************
Summary:
	Read Data from the Rx I2c Space 
	

Input:
	hHDR - HDMI control handle that was previously opened by BHDR_Open.
	offset - I2C Offset where data is to start
	pData - pointer to the data to be written
	Length - number of bytes to be written
	
    
Output:
	<None>

Returns:
	BERR_SUCCESS - Data Written  
	BERR_TIMEOUT - Timeout waiting to write/load I2c Data.

See Also:
	BHDR_ReadRxI2cRegisterSpace	

****************************************************************************/
BERR_Code BHDR_ReadRxI2cRegisterSpace(BHDR_Handle hHDR, 
	uint8_t offset, uint8_t *pData, uint8_t Length) ;
		
	
/***************************************************************************
Summary:
	set hot plug signal high or low
	

Input:
	hHDR - HDMI control handle that was previously opened by BHDR_Open.
	disable or enable
    
Output:
	<None>

Returns:
	BERR_SUCCESS 

See Also:
	BHDR_Open	

***************************************************************************/
BERR_Code BHDR_SetHotPlug(BHDR_Handle hHDR, 
	BHDR_HotPlugSignal eHotPlugSignal) ;


/***************************************************************************
Summary:
	Generate a Hot Plug Pulse  
	

Input:
	hHDR - HDMI control handle that was previously opened by BHDR_Open.
	
    
Output:
	<None>

Returns:
	BERR_SUCCESS 

See Also:
	BHDR_Open	

***************************************************************************/
BERR_Code BHDR_PulseHotPlug(BHDR_Handle hHDR) ; 



/***************************************************************************
Summary:
	Estimate the Pixel Clock Rate based on the number of received Pixels
		

Input:
	hHDR - HDMI control handle that was previously opened by BHDR_Open.
	
    
Output:
	EstimatedPixelClockRate - estimated Pixel Clock Rate from 3560 circuit

Returns:
	BERR_SUCCESS 

See Also:
	BHDR_Open	

***************************************************************************/
BERR_Code BHDR_GetPixelClockEstimate(BHDR_Handle hHDR,		
	uint32_t *EstimatedPixelClockRate 
) ;


/***************************************************************************
Summary:
	Clear the specified locked RAM packet 
		

Input:
	hHDR - HDMI control handle that was previously opened by BHDR_Open.
	PacketNum - Id of the packet to clear
	
    
Output:
	None

Returns:
	BERR_SUCCESS 

See Also:
	BHDR_Open	

***************************************************************************/
BERR_Code BHDR_ClearLockedPacket(
   BHDR_Handle hHDR,		/* [in] HDMI handle */
   uint8_t PacketNum        /* [in] Locked Packet to Clear */
) ;


/***************************************************************************
Summary:
	Get the HDMI Packet Structure 
		

Input:
	hHDR - HDMI control handle that was previously opened by BHDR_Open.
	PacketNum - Packet Number to retrieve
	NumPacketBufferBytes - number of bytes in Packet Buffer
	
    
Output:
	PacketBuffer - Contents of Packet

Returns:
	BERR_SUCCESS 

See Also:
	BHDR_Open	

***************************************************************************/
BERR_Code BHDR_GetPacketData(BHDR_Handle hHDR,
	uint8_t PacketNum,
	BAVC_HDMI_Packet *Packet 
) ;



/***************************************************************************
Summary:
	Get a copy of the original Packet Bytes received by the HDMI Rx
		

Input:
	hHDR - HDMI control handle that was previously opened by BHDR_Open.
	ePacketType - Packet Type to Copy
	
    
Output:
	pPacketBytes - adress to write packet bytes to.  pPacketBytes must point to an allocated
	space of BAVC_HDMI_PB_LENGTH

Returns:
	BERR_SUCCESS 
	BERR_NOT_SUPPORTED

See Also:
	BHDR_Open	
	BHDR_GetPacketData

***************************************************************************/
BERR_Code BHDR_GetRawPacketData(BHDR_Handle hHDR, 
	BAVC_HDMI_PacketType ePacketType,
	BAVC_HDMI_Packet *pPacketBytes) ;



/***************************************************************************
Summary:
	Get the AVI InfoFrame Data received/parsed from the HDMI Packets  
		

Input:
	hHDR - HDMI control handle that was previously opened by BHDR_Open.
	
    
Output:
	AviInfoFrame - AVI Info Frame data structure.

Returns:
	BERR_SUCCESS 

See Also:
	BHDR_Open	

***************************************************************************/
BERR_Code BHDR_GetAviInfoFrameData(BHDR_Handle hHDR,
	BAVC_HDMI_AviInfoFrame *AviInfoFrame ) ;


/***************************************************************************
Summary:
	Get the Audio InfoFrame Data received/parsed from the HDMI Packets  

Input:
	hHDR - HDMI control handle that was previously opened by BHDR_Open.
	
    
Output:
	AudioInfoFrame - Audio Info Frame data structure.

Returns:
	BERR_SUCCESS 

See Also:
	BHDR_Open	

***************************************************************************/
BERR_Code BHDR_GetAudioInfoFrameData(BHDR_Handle hHDR,
	BAVC_HDMI_AudioInfoFrame *AudioInfoFrame ) ;


/***************************************************************************
Summary:
	Get the SPD (Source Product Description) InfoFrame Data received/parsed from the 
	HDMI Packets  
		

Input:
	hHDR - HDMI control handle that was previously opened by BHDR_Open.
	
    
Output:
	SPDInfoFrame - SPD Info Frame data structure.

Returns:
	BERR_SUCCESS 

See Also:
	BHDR_Open	

***************************************************************************/
BERR_Code BHDR_GetSPDInfoFrameData(BHDR_Handle hHDR,
	BAVC_HDMI_SPDInfoFrame *SPDInfoFrame );



/***************************************************************************
Summary:
	Get the Vendor Specific InfoFrame Data received/parsed from the 
	HDMI Packets  
		

Input:
	hHDR - HDMI control handle that was previously opened by BHDR_Open.
	
    
Output:
	VendorSpecificInfoFrame - Vendor Specific Info Frame data structure.

Returns:
	BERR_SUCCESS 

See Also:
	BHDR_Open	

***************************************************************************/
BERR_Code BHDR_GetVendorSpecificInfoFrameData(BHDR_Handle hHDR,
	BAVC_HDMI_VendorSpecificInfoFrame *VendorSpecificInfoFrame) ;



/***************************************************************************
Summary:
	Get the Audio Content Protection Data received by HDMI Rx
		

Input:
	hHDR - HDMI control handle that was previously opened by BHDR_Open.
	
    
Output:
	ACP - Audio Content Protection data structure.

Returns:
	BERR_SUCCESS 

See Also:
	BHDR_Open	

***************************************************************************/
BERR_Code BHDR_GetAudioContentProtection(BHDR_Handle hHDR,
	BAVC_HDMI_ACP *ACP );

/***************************************************************************
Summary:
	Get the Audio Clock Regeneration Data received by HDMI Rx
		

Input:
	hHDR - HDMI control handle that was previously opened by BHDR_Open.
	
    
Output:
	AudioClockRegeneraionPacket - Audio Clock Regeneration data structure.

Returns:
	BERR_SUCCESS 

See Also:
	BHDR_Open	

***************************************************************************/
BERR_Code BHDR_GetAudioClockRegenerationData(BHDR_Handle hHDR,
	BAVC_HDMI_AudioClockRegenerationPacket * AudioClockRegeneraionPacket);

/***************************************************************************
Summary:
	Get the General Control Packet Data received by HDMI Rx
		

Input:
	hHDR - HDMI control handle that was previously opened by BHDR_Open.
	
    
Output:
	Gcpdata - General Control Packet Data structure.

Returns:
	BERR_SUCCESS 

See Also:
	BHDR_Open	

***************************************************************************/
BERR_Code BHDR_GetGeneralControlPacketData(BHDR_Handle hHDR,
	BAVC_HDMI_GcpData * GcpData);


/***************************************************************************
Summary:
	Get the ISRC Packet Data received by HDMI Rx
		

Input:
	hHDR - HDMI control handle that was previously opened by BHDR_Open.
	
    
Output:
	ISRC - ISRC Packet Data structure.  Contains combination of both ISRC1 and ISRC2 (if exists)

Returns:
	BERR_SUCCESS 

See Also:
	BHDR_Open	

***************************************************************************/
BERR_Code BHDR_GetISRCData(BHDR_Handle hHDR, 
	BAVC_HDMI_ISRC * pISRC) ;


/******************************************************************************
Summary:
*******************************************************************************/
BERR_Code BHDR_GetAudioSampleFreq(BHDR_Handle hHDR,
	uint8_t *AudioSampleFreq );
	

	
/**************************************************************************
Summary:
	Register a callback function to be called after a HDMI packet changes or stops being
	received

Description:
	This function is used to enable a callback function that will
	be called any time an incoming HDMI Packet changes or stops coming in

Input:
	hHDR - HDMI Rx Handle
	pfCallback_isr - pointer to callback function to be called at format change
	pvParam1 - User defined data structure casted to void.
	iParam2 - Additional user defined value... should be 0 as callback will be passed packet 
	type in this argument
	
Returns:
	BERR_SUCCESS - Callback Installation Successful

See Also:
		BHDR_UnInstallVideoFormatChangeCallback

**************************************************************************/
BERR_Code BHDR_InstallPacketChangeCallback(
	BHDR_Handle hHDR,			/* [in] HDMI Rx Handle */
	const BHDR_CallbackFunc pfCallback_isr, /* [in] cb for packet changes */
	void *pvParm1, /* [in] the first argument (void *) passed to the callback function */ 
	int iParm2) ;   /* [in] the second argument(int) passed to the callback function */ 



/**************************************************************************
Summary:
	Remove a previously registered callback function for Packet Change

Description:
	This function is used to remove a callback function that is called when there is a HDMI
	Packet change or stopped Packet reception

Input:
	hHDR - HDMI Rx Handle
	pfCallback - pointer to callback function to be removed
	
Returns:
	BERR_SUCCESS - Callback Removal Successful

See Also:
		BHDR_UnInstallVideoFormatChangeCallback

**************************************************************************/
BERR_Code BHDR_UnInstallPacketChangeCallback( 
	BHDR_Handle hHDR,                       /* [in] HDMI Rx Handle */
	const BHDR_CallbackFunc pfCallback_isr) ; /* [in] cb for format changes */


/**************************************************************************
Summary:
	Register a callback function to be called after a video format change
	change.

Description:
	This function is used to enable a callback function that will
	be called any time the incoming video format on the HDMI port
	changes.  

Input:
	hHDR - HDMI Rx Handle
	pfCallback - pointer to callback function to be called at format change
	pvParam1 - User defined data structure casted to void.
	iParam2 - Additional user defined value.
	
Returns:
	BERR_SUCCESS - Callback Installation Successful

See Also:
		BHDR_UnInstallVideoFormatChangeCallback

**************************************************************************/
BERR_Code BHDR_InstallVideoFormatChangeCallback( 
	BHDR_Handle hHDR,			/* [in] HDMI Rx Handle */
	const BHDR_CallbackFunc pfCallback_isr, /* [in] cb for format changes */
	void *pvParm1,  /* [in] the first argument (void *) passed to the callback function */ 
	int iParm2      /* [in] the second argument(int) passed to the callback function */ 
) ;


/**************************************************************************
Summary:
	Remove a previously registered callback function for video format changes

Description:
	This function is used to remove a callback function from being called
	any time the incoming video format on the HDMI port	changes.  

Input:
	hHDR - HDMI Rx Handle
	pfCallback - pointer to callback function to be removed
	
Returns:
	BERR_SUCCESS - Callback Removal Successful

See Also:
		BHDR_UnInstallVideoFormatChangeCallback

**************************************************************************/
BERR_Code BHDR_UnInstallVideoFormatChangeCallback( 
	BHDR_Handle hHDR,                       /* [in] HDMI Rx Handle */
	const BHDR_CallbackFunc pfCallback_isr /* [in] cb for format changes */
) ;


/**************************************************************************
Summary:
	Register a callback function to be called after a audio format change

Description:
	This function is used to enable a callback function that will
	be called any time the incoming audio format on the HDMI port
	changes.  

Input:
	hHDR - HDMI Rx Handle
	pfCallback - pointer to callback function to be called at format change
	pvParam1 - User defined data structure casted to void.
	iParam2 - Additional user defined value.
	
Returns:
	BERR_SUCCESS - Callback Installation Successful

See Also:
		BHDR_UnInstallAudioFormatChangeCallback

**************************************************************************/
BERR_Code BHDR_InstallAudioFormatChangeCallback( 
	BHDR_Handle hHDR,			/* [in] HDMI Rx Handle */
	const BHDR_CallbackFunc pfCallback_isr, /* [in] cb for format changes */
	void *pvParm1,  /* [in] the first argument (void *) passed to the callback function */ 
	int iParm2      /* [in] the second argument(int) passed to the callback function */ 
) ;


/**************************************************************************
Summary:
	Remove a previously registered callback function for audio format changes

Description:
	This function is used to remove a callback function from being called
	any time the incoming audio format on the HDMI port changes.  

Input:
	hHDR - HDMI Rx Handle
	pfCallback - pointer to callback function to be removed
	
Returns:
	BERR_SUCCESS - Callback Removal Successful

See Also:
		BHDR_UnInstallAudioFormatChangeCallback

**************************************************************************/
BERR_Code BHDR_UnInstallAudioFormatChangeCallback( 
	BHDR_Handle hHDR,                       /* [in] HDMI Rx Handle */
	const BHDR_CallbackFunc pfCallback_isr /* [in] cb for format changes */
) ;




/**************************************************************************
Summary:
	Register a callback function to be called after a Av Mute change.

Description:
	This function is used to enable a callback function that will
	be called any time the HDMI AV MUTE status changes.  The AV MUTE
	signals to the Receiever or enable/disable the incoming video 

Input:
	hHDR - HDMI Rx Handle
	pfCallback - pointer to callback function to be called at format change
	pvParam1 - User defined data structure casted to void.
	iParam2 - Additional user defined value.
	
Returns:
	BERR_SUCCESS - Callback Installation Successful

See Also:
		BHDR_UnInstallAvMuteNotifyCallback

**************************************************************************/
BERR_Code BHDR_InstallAvMuteNotifyCallback( 
	BHDR_Handle hHDR,			/* [in] HDMI Rx Handle */
	const BHDR_CallbackFunc pfCallback_isr, /* [in] cb for format changes */
	void *pvParm1,  /* [in] the first argument (void *) passed to the callback function */ 
	int iParm2      /* [in] the second argument(int) passed to the callback function */ 
) ;


/**************************************************************************
Summary:
	Remove a previously registered callback function for HDMI AV MUTE changes

Description:
	This function is used to remove a callback function from being called
	any time the incoming AV MUTE value on the HDMI port changes.  

Input:
	hHDR - HDMI Rx Handle
	pfCallback - pointer to callback function to be removed
	
Returns:
	BERR_SUCCESS - Callback Removal Successful

See Also:
		BHDR_UnInstallAvMuteNotifyCallback

**************************************************************************/
BERR_Code BHDR_UnInstallAvMuteNotifyCallback( 
	BHDR_Handle hHDR,                       /* [in] HDMI Rx Handle */
	const BHDR_CallbackFunc pfCallback_isr /* [in] cb for format changes */
) ;



/**************************************************************************
Summary:
	Register a callback function to be called when there is a change in the color Gamut Data

Description:
	This function is used to enable a callback function that will 	be called any time the Rx 
	detects changes in the Gamut Data

Input:
	hHDR - HDMI Rx Handle
	pfCallback - pointer to callback function to be called 
	pvParam1 - User defined data structure casted to void.
	iParam2 - Additional user defined value.

Note:
	The callback function is called during Isr time and should do the minimum processing 
	retrieve the gamut data
	
	
Returns:
	BERR_SUCCESS - Callback Installation Successful

See Also:
		BHDR_UnInstallGamutChangeCallback

**************************************************************************/
BERR_Code BHDR_InstallGamutChangeCallback( 
	BHDR_Handle hHDR,			/* [in] HDMI Rx Handle */
	const BHDR_CallbackFunc pfCallback_isr, /* [in] cb to load HDCP Keys  */
	void *pvParm1,  /* [in] the first argument (void *) passed to the callback function */ 
	int iParm2      /* [in] the second argument(int) passed to the callback function */ 
) ;



/**************************************************************************
Summary:
	Remove a previously registered callback function for Hdcp Key Loading

Description:
	This function is used to remove the installed callback function used when there is a 
	change in the Gamut Data

Input:
	hHDR - HDMI Rx Handle
	pfCallback - pointer to callback function to be removed
	
Returns:
	BERR_SUCCESS - Callback Removal Successful

See Also:
		BHDR_InstallGamutChangeCallback

**************************************************************************/
BERR_Code BHDR_UnInstallGamutChangeCallback( 
	BHDR_Handle hHDR,                       /* [in] HDMI Rx Handle */
	const BHDR_CallbackFunc pfCallback_isr /* [in] cb for format changes */
) ;


/**************************************************************************
Summary:
	Return current Status of HDMI Rx 
	
Description:
	This function is used to retrieve status of the HDMI Rxc (e.g Attached Device, mode, etc

Input:
	hHDR - HDMI Rx Handle
	pHdmiStatus - pointer to HDMI Status structure 
	
Returns:
	BERR_SUCCESS 

See Also:
	None
**************************************************************************/
BERR_Code BHDR_GetHdmiRxStatus(
	BHDR_Handle hHDR,           /* [in] HDMI Rx Handle */
	BHDR_Status *pHdmiStatus   /* struct Status Info */
) ;


/**************************************************************************
Summary:
	Return current format detect Status of HDMI Rx 
	
Description:
	This function is used to retrieve format detection status of the HDMI Rxc (e.g Attached Device, mode, etc

Input:
	hHDR - HDMI Rx Handle
	pHdmiFmt - pointer to detected HDMI Status structure 
	
Returns:
	BERR_SUCCESS 

See Also:
	None
**************************************************************************/ 
BERR_Code BHDR_GetHdmiRxDetectedVideoFormat(BHDR_Handle hHDR, BAVC_HDMI_VideoFormat *pHdmiFmt);


/**************************************************************************
Summary:
	Initialization after changing HDMI cable connection status 
	
Description:
	This function is mainly for the board which doesn't have hotplug signal.  This function should be
	called by the app/upper layer detects change in the Hot Plug status.

Input:
	hHDR - HDMI Rx Handle
	bHotplugStatus - current port status  1 : connected  0: unconnected
	
Returns:
	BERR_SUCCESS 

See Also:
	None
**************************************************************************/ 
BERR_Code BHDR_ConfigureAfterHotPlug(BHDR_Handle hHDR, bool bHotplugStatus);


/**************************************************************************
Summary:
	Initialization after changing HDMI cable connection status 
	
Description:
	This function workd primarily with boards that have the Hot Plug signal connected to the core 

Input:
	hHDR - HDMI Rx Handle
	bHotplugStatus - current port status  1 : connected  0: unconnected
	
Returns:
	BERR_SUCCESS 

See Also:
	None
**************************************************************************/ 
BERR_Code BHDR_ConfigureAfterHotPlug_isr(BHDR_Handle hHDR, bool bHotplugStatus) ;


#if BHDR_CONFIG_EDID_RAM

typedef struct 
{
	uint8_t uiNumBlocks ;
       uint8_t *pDataBytes ;
} BHDR_EDID_Info ;


void BHDR_EDID_RAM_Enable(BHDR_Handle hHDR) ;
void BHDR_EDID_RAM_Disable(BHDR_Handle hHDR) ;

void BHDR_EDID_RAM_GetDefaultBlocks(BHDR_EDID_Info *stEdid ) ;

BERR_Code BHDR_EDID_RAM_LoadData(BHDR_Handle hHDR, const BHDR_EDID_Info *stEdid ) ;
#endif

#if BHDR_CONFIG_MASTER_SLAVE_3D_SUPPORT
BERR_Code BHDR_UsageMasterSlaveSet(
   BHDR_Handle hHDRMaster,  BHDR_Handle hHDRSlave) ;

BERR_Code BHDR_UsageMasterSlaveClear(
   BHDR_Handle hHDRMaster,  BHDR_Handle hHDRSlave) ;
#endif

#ifdef __cplusplus
}
#endif

#endif /* BHDR_H__ */

