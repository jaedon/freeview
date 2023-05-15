/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhdm.h $
 * $brcm_Revision: Hydra_Software_Devel/120 $
 * $brcm_Date: 7/12/12 1:23p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hdm/include/bhdm.h $
 * 
 * Hydra_Software_Devel/120   7/12/12 1:23p rgreen
 * SW7425-2989: Merge changes
 * 
 * Hydra_Software_Devel/SW7425-2989/1   7/6/12 2:16p rgreen
 * SW7425-2989: Add debug monitor to report format changes to HDMI Tx core
 * 
 * Hydra_Software_Devel/119   6/18/12 3:37p vle
 * SW7425-2951: Update HDM PI directory structure
 * 
 * Hydra_Software_Devel/118   4/30/12 4:22p vle
 * SW7425-2666: Merge to mainline
 * 
 * Hydra_Software_Devel/SW7425-2666_FifoTest/1   4/19/12 10:24a rgreen
 * SW7425-2666:  Update FIFO recenter algorithm
 * 
 * Hydra_Software_Devel/117   4/16/12 2:48p vle
 * SW7346-737: Merge to mainline
 * 
 * Hydra_Software_Devel/SW7346-737/1   4/12/12 4:21p vle
 * SW7346-737: Initial attempt adding CRC support in HDM
 * 
 * Hydra_Software_Devel/116   3/19/12 11:33a rgreen
 * SW7425-2650: Move BHDM_SupportedVideoInfo to
 * BHDM_EDID_GetSupportedVideoInfo
 * 
 * Hydra_Software_Devel/115   3/1/12 1:20p rgreen
 * SW7425-2515: Remove unused CEC code which is now located in CEC pi;
 * remove CEC EventHandle
 * 
 * Hydra_Software_Devel/114   2/23/12 10:49a rgreen
 * SW7125-1146,SW7408-317: Merge changes
 * 
 * Hydra_Software_Devel/SW7408-317/1   2/21/12 6:33p rgreen
 * SW7125-1146,SW7408-317: Treat RxSense and HP events separately.  Update
 * processing of both events
 * 
 * Hydra_Software_Devel/113   1/4/12 3:31p rgreen
 * SW7405-5004: Use BAVC_HDMI_xxx vs BHDM_xxx macros
 * 
 * Hydra_Software_Devel/112   11/30/11 11:14a rgreen
 * SW7405-5004: Replace BKNI_Memcmp for structure with field by field
 * comparison
 * 
 * Hydra_Software_Devel/111   4/29/11 4:48p vle
 * SW7125-798: Revised
 * 
 * Hydra_Software_Devel/110   4/22/11 11:48a vle
 * SW7125-798: Add API to allow customer to control pre-emphasis settings
 * 
 * Hydra_Software_Devel/SW7125-798/2   4/21/11 10:57a vle
 * SW7125-798: Update pre-emphasis configuration structure for both 65nm
 * and 40nm platforms.
 * 
 * Hydra_Software_Devel/SW7125-798/1   2/3/11 11:26a vle
 * SW7125-798: Add NEXUS API to allow customer to control pre-emphasis
 * settings
 * 
 * Hydra_Software_Devel/109   2/17/11 7:38p jtna
 * SW7420-1141: rework HDMI power management. break public API to allow
 * for StandbySettings
 * 
 * Hydra_Software_Devel/108   10/20/10 6:48p jtna
 * SW7420-972: added standby/resume functions. removed references to
 * 108MHz BCHP_PWR node.
 * 
 * Hydra_Software_Devel/107   9/28/10 7:18p vle
 * SW7422-23: Refactor HDMI code to isolate platform dependent code
 * furthermore. Add support for 7422 and other 40nm platforms.
 * 
 * Hydra_Software_Devel/106   9/22/10 7:10p vle
 * SW7405-4782: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7405-4782/1   9/10/10 8:43p vle
 * SW7405-4782: Changing aspect ratio does not require HDCP re-
 * authentication.
 * 
 * Hydra_Software_Devel/105   6/28/10 6:59p vle
 * SW7208-59: Add API to control the HDMI data transfer mode, master or
 * slave
 * 
 * Hydra_Software_Devel/104   4/23/10 10:43a vle
 * SW7420-676: merge to main branch
 * 
 * Hydra_Software_Devel/SW7420-676/1   4/21/10 2:27p vle
 * SW7420-676: Add API to return supported video info
 * 
 * Hydra_Software_Devel/103   4/16/10 6:50p vle
 * SW7420-543: BHDM_InputPixelClock enum should be private
 * 
 * Hydra_Software_Devel/102   3/26/10 4:33p vle
 * SW7601-172: Merge xvYCC support with Gamut Metadata Packet transmission
 * from bdvd branch
 * 
 * Hydra_Software_Devel/101   2/26/10 5:17p vle
 * SW7405-3016: Remove software i2c settings from HDM PI. This mode is
 * configure in I2C module
 * 
 * Hydra_Software_Devel/100   2/23/10 12:48a vle
 * SW7420-579: Refactor HDMI PI.
 * 
 * Hydra_Software_Devel/16   1/18/10 6:47p vle
 * SW7342-129: Add BHDM_GetPreEmphasisConfiguration and
 * BHDM_SetPreEmphasisConfiguration implementations.
 * 
 * Hydra_Software_Devel/15   1/8/10 5:19p vle
 * SW7405-3740: Port changes to 7420, 7468, and other platforms.
 * Add isr callback to provide immediate notifcation of HP changes vs
 * waiting for event processing
 * 
 * Hydra_Software_Devel/14   9/21/09 6:14p vle
 * SW7601-164: Merge changes in bdvd_v3.0 branch
 * 
 * Hydra_Software_Devel/bdvd_v3.0/1   9/21/09 3:01p rbshah
 * PR16772[DVD]:[ see HiDef-DVD bug tracking system for more info ].  The
 * ability to have the app. get/set the N value for HW-CTS was
 * added as precautionary measure. Appears it's not needed anymore
 * so taking it out.
 * 
 * Hydra_Software_Devel/13   8/26/09 3:41p vle
 * SW7405-2670:
 * Add implementation of interrupt based receiver sense
 * 
 * Hydra_Software_Devel/12   7/22/09 7:35p vle
 * PR56776: Prevent HDCP An Timeout
 * 
 * Hydra_Software_Devel/11   6/25/09 3:04p vle
 * PR56132, PR48151:
 * Add support for Get HDMI Vendor Specific Info Frame
 * Correct channel assignment for PCM 5.1
 * 
 * Hydra_Software_Devel/10   3/11/09 6:29p vle
 * PR52873: Merge changes from bdvd_v2.0 branch to Hydra branch
 * Ensure we don't miss a hot-plug pulse whenever we're waiting.
 * Ensure HDMI input/drift FIFO is initialized and re-centered, disable
 * GCP packet generation when connect to DVI monitors.
 * Updated all the Audio CRP N-CTS values (both SW and HW CTS modes)
 * 
 * Hydra_Software_Devel/bdvd_v2.0/11   3/9/09 1:58p rbshah
 * PR_10346 [ see HiDef-DVD bug tracking system for more info ].  Updated
 * all the Audio CRP N-CTS tables with those generated by the HW
 * folks (Vincent Y. and Chris P.). We now have "official" values for both
 * HW and SW CTS modes.
 * 
 * Also modified the Display/HDMI API so application can only set N values
 * related to HW CTS. In other words, prior API for updating SW CTS values
 * was removed.
 * 
 * Tested all audio sample rates (except 176.4KHz) at all color depths and
 * all relevant pixel clock rates using both HW and CW CTS with the Astro.
 * 
 * Hydra_Software_Devel/bdvd_v2.0/10   2/24/09 3:43p rbshah
 * PR_13243 [ see HiDef-DVD bug tracking system for more info ].  Enhance
 * Display/HDMI API to allow the app. precise control over
 * N and CTS values used for Audio Clock Regen (ACR) packets. This
 * required a little re-design of the N/CTS tables in BHDM.
 * 
 * Hydra_Software_Devel/bdvd_v2.0/9   2/18/09 9:44a rbshah
 * PR_13266 [ see HiDef-DVD bug tracking system for more info ].  Added
 * Bluray and Super Audio CD types as requested.
 * 
 * Hydra_Software_Devel/bdvd_v2.0/8   1/29/09 3:41p rbshah
 * PR_12826 [ see HiDef-DVD bug tracking system for more info ].  Ensure
 * we don't miss a hot-plug pulse whenever we're waiting (in
 * the PI as well as BDVD). Also removed an unnecessary wait before
 * starting HDCP.
 * 
 * Hydra_Software_Devel/9   3/9/09 3:20p vle
 * PR50570, PR50918, PR49277, PR49652, PR52873:
 * Add API to mute/unmute audio, update pixel repitition support, add
 * SetPixelDataOverride API for transmission of black video. Merge
 * changes/updates from bdvd_v2.0 to main branch.
 * 
 * Hydra_Software_Devel/8   3/3/09 8:23p vle
 * PR50569: Add HW Ri/Pj checking feature. Merged from bdvd branch after
 * Rajul's testing effort.
 * 
 * Hydra_Software_Devel/bdvd_v2.0/6   1/23/09 10:14a rbshah
 * PR_10346 [ see HiDef-DVD bug tracking system for more info ].  Checkin
 * code drop from Anthony Le for Auto Ri,Pj feature in the
 * 7601B0 (Digital Video PR50569).
 * 
 * This is disabled by default and will be turned on once it has
 * been tested and soaked.
 * 
 * Did verify the A0 build!
 * 
 * Hydra_Software_Devel/7   1/12/09 7:34p vle
 * PR49277: Merge to main branch.
 * Add pixel repetition support for 480p
 * 
 * Hydra_Software_Devel/PR49277/2   1/12/09 7:18p vle
 * PR49277: Update to add pixel repetition support.
 * 
 * Hydra_Software_Devel/bdvd_v2.0/7   1/27/09 4:18p rbshah
 * PR_12826 [ see HiDef-DVD bug tracking system for more info ].  Add
 * logic to wait for stable video to arrive from VEC before we
 * start the HDCP authentication process.
 * 
 * This checkin may not solve the Simplay failure at hand, but it's
 * something that needs to be done anyway.
 * 
 * Hydra_Software_Devel/bdvd_v2.0/6   1/23/09 10:14a rbshah
 * PR_10346 [ see HiDef-DVD bug tracking system for more info ].  Checkin
 * code drop from Anthony Le for Auto Ri,Pj feature in the
 * 7601B0 (Digital Video PR50569).
 * 
 * This is disabled by default and will be turned on once it has
 * been tested and soaked.
 * 
 * Did verify the A0 build!
 * 
 * Hydra_Software_Devel/bdvd_v2.0/5   1/21/09 11:56a rbshah
 * PR_10346 [ see HiDef-DVD bug tracking system for more info ].  Enhance
 * Display/HDMI API to allow application to mute/unmute just
 * audio (Digital Video PR50570).
 * 
 * This feature is only available on the 7601B0.
 * 
 * Hydra_Software_Devel/bdvd_v2.0/4   1/20/09 5:06p rbshah
 * PR_10346 [ see HiDef-DVD bug tracking system for more info ].  Take 2:
 * need to shift the 8-bit color component values to 12-bits.
 * 
 * Hydra_Software_Devel/bdvd_v2.0/3   1/20/09 4:02p rbshah
 * PR_10346 [ see HiDef-DVD bug tracking system for more info ].  Use new
 * B0 feature to transmit black video correctly in YCC color
 * space [Digital Video PR49652].
 * 
 * Hydra_Software_Devel/bdvd_v2.0/2   1/12/09 5:18p rbshah
 * PR_12066 [ Enable pixel repetition to allow HBR audio at lower video
 * resolutions ].  Wrap up pixel repetition support at bdvd api. Quick
 * verification of
 * 480p and 576p 2x, 4x and 480i 4x at all color depths.
 * 
 * Hydra_Software_Devel/bdvd_v2.0/1   1/9/09 5:57p rbshah
 * PR_12066 [ Enable pixel repetition to allow HBR audio at lower video
 * resolutions ].  Re-work VDC HDMI RM table and lookup. Also absorb
 * pixel repetition
 * support in BHDM from Anthony Le.
 * 
 * Still work in progress.
 * 
 * Hydra_Software_Devel/PR49277/1   1/8/09 4:30p vle
 * PR49277: Add pixel repetition support for 480p
 * 
 * Hydra_Software_Devel/6   12/23/08 7:27p vle
 * PR50081: Merge to main branch.
 * Add deep color support.
 * 
 * Hydra_Software_Devel/PR50081/2   12/11/08 2:19p vle
 * PR50081: Add deep color support for 7601.
 * 
 * Hydra_Software_Devel/PR50081/1   12/8/08 5:30p vle
 * PR50081: add deep color support.
 * 
 * Hydra_Software_Devel/5   12/5/08 2:28p vle
 * PR 48987: Update variable name for better description
 * 
 * Hydra_Software_Devel/4   12/2/08 8:02p vle
 * PR48987: Add setting to enable I2C bit bang mode.
 * 
 * Hydra_Software_Devel/3   12/2/08 11:20a vle
 * PR44535: Add settings to allow user to always force enable display even
 * with the same HDMI settings.
 * 
 * Hydra_Software_Devel/2   10/28/08 10:35p vle
 * PR44535: Update CTS programming using Vincent latest calculated values.
 * Merge changes from branch bdvd v2.0
 * 
 * Hydra_Software_Devel/1   10/9/08 4:37p vle
 * PR44535: Merge to main Hydra dev. branch
 * 
 * Hydra_Software_Devel/PR44535/2   9/12/08 2:54p vle
 * PR44535: Update CTS settings. Add 1080p support.
 * 
 * Hydra_Software_Devel/PR44535/bdvd_v2.0/1   9/5/08 4:57p rbshah
 * PR_10374 [ Adding support for 7601 ].  Initial checkin for HDMI
 * support. Not functional yet, but app_hdmi now
 * compiles and runs. EDID is operational. Video support requires a few
 * VDC tweaks that will come shortly (next day or two).
 * 
 * Some of the BHDM changes provided by Anthony Le.
 * 
 * Hydra_Software_Devel/PR44535/1   8/6/08 7:47p vle
 * PR44535: Initial version
 * 
 ***************************************************************************/

 
/*= Module Overview *********************************************************
<verbatim>

Overview
The HDMI (High-Definition Multimedia Interface) API is a library used to
provide HDMI/DVI (Digital Visual Interface) functionality between Broadcom
cores and the connected HDMI/DVI receivers.

The API includes support for associated HDMI functionality such as parsing
of the EDID (Extended Display Identification Data) contained in the HDMI/DVI
monitors as well as support for HDCP (High-bandwidth Data Content Protection)

Additional support for reading CEC (Consumer Electronic Control) will also be
provided.


Sample Code 
//
// ********************************************
// Delclare / Initialize Device Handles for...
// ********************************************

BHDM_Handle     MyHDMI
BCHP_Handle     MyChip;
BREG_Handle     MyRegister;
BINT_Handle     MyInterupt;
BREG_I2C_Handle MyI2C;


// ********************************************
// Video Display Handle Must be open prior to 
// opening HDMI device
// ********************************************
BVDC_Open(&hDisplay,...)

// ********************************************
// Get/Modify Default Settings for HDMI
// ********************************************
BHDM_GetDefaultSettings(&BHDM_Settings)

// ********************************************
// open HDMI Device Handle 
// ********************************************
BHDM_Open(..., BHDM_Settings)


// ********************************************
// Set VDC DAC Configuration
// See VDC Porting Interface
// ********************************************


BVDC_ApplyChanges(...)

// ********************************************
// Enable the VEC output to the HDMI device
// ********************************************

BVDC_Display_SetHdmiConfiguration(
	hDisplay, BVDC_Hdmi_0, BVDC_HdmiOutput_eHDRGB) ;

BVDC_ApplyChanges(...)


// ********************************************
// check the attached receiver for HDMI support.
// HDMI support is determined by the existence of
// the Vendor Specific Data Block (VSDB) in the EDID 
// ********************************************

rc = BHDM_EDID_IsRxDeviceHdmi(hHDMI, &MyRxVSDB, &RxHasHdmiSupport) ;
if (rc != BERR_SUCCESS)
	process error - return ;

// ********************************************


// ********************************************
// get/modify current HDMI settings for display 
// default display mode is DVI
// use the current video format used by the VDC
// GetHdmiSettings can be used if in a different
// area of the code
// ********************************************

BVDC_Display_GetVideoFormat(hDisplay, &MyVDCFormat) ;
MyHdmiSettings.eInputVideoFmt   = MyVDCFormat ;


// ********************************************
// Set the Pixel Clock Rate based on TBD (headend
// or stream)
// ********************************************
MyHdmiSettings.eInputPixelClock   = BHDM_PixelClock_eXXX ;


// ********************************************
// if neccessary, set parameters for HDMI Audio 
// ********************************************
if (RxHasHdmiSupport)
{
	MyHdmiSettings.eOutputFormat      = BHDM_OutputFormat_eHDMIMode ;
	MyHdmiSettings.eAudioSamplingRate =	BAVC_AudioSamplingRate_eXXX ;
}
	
		 
// ********************************************
// Enable/Turn on the HDMI Output
// ********************************************
if ((rc = BHDM_EnableDisplay(hHDMI, &MyHdmiSettings)) == BERR_SUCCESS)
	printf("DisplayOutput Enabled") ;
else
{
	// check Return Code for possible errors 
	printf("*** ERROR Enabling DVI/HDMI Output to the Display") ;
}
.
.
.
</verbatim>
****************************************************************************/

#ifndef BHDM_H__
#define BHDM_H__

#include "bchp.h"       /* Chip Info */
#include "breg_mem.h"   /* Chip register access. */
#include "bkni.h"       /* Kernel Interface */
#include "bint.h"       /* Interrupt */
#include "breg_i2c.h"   /* I2C */
#include "btmr.h"   /* Timer Handle  */

#include "bavc.h"       /* A/V constants */
#include "bavc_hdmi.h"  /* HDMI A/V constants */
#include "berr_ids.h"   /* Error codes */
#include "bfmt.h"       /* Video Formats */
#include "bdbg.h"       /* Debug Support */


/******************************************************************************
Summary:
HDMI Context Handle
*******************************************************************************/
typedef struct BHDM_P_Handle *BHDM_Handle ;

#include "bhdm_config.h"
#if BHDM_HAS_HDMI_1_3_FEATURES
#include "bhdm_1_3_features.h"
#endif
 

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************
Summary:
HDMI Basic Return Codes
*******************************************************************************/
#define BHDM_BASE_ERRS                0

#define BHDM_NOT_IMPLEMENTED			BERR_MAKE_CODE(BERR_HDM_ID, BHDM_BASE_ERRS + 1)
#define BHDM_RX_FEATURE_NOT_SUPPORTED	BERR_MAKE_CODE(BERR_HDM_ID, BHDM_BASE_ERRS + 2)
#define BHDM_NO_RX_DEVICE				BERR_MAKE_CODE(BERR_HDM_ID, BHDM_BASE_ERRS + 3)
#define BHDM_UNSUPPORTED_VIDEO_FORMAT	BERR_MAKE_CODE(BERR_HDM_ID, BHDM_BASE_ERRS + 4)
#define BHDM_HARDWARE_ERROR				BERR_MAKE_CODE(BERR_HDM_ID, BHDM_BASE_ERRS + 5)


#define BHDM_HDCP_ERRS 100     /* HDCP error codes */
#define BHDM_EDID_ERRS 200 	   /* EDID error codes */
#define BHDM_CEC_ERRS  300	   /* CEC error codes */


/******************************************************************************
Summary:
HDMI Packet Size in Bytes
*******************************************************************************/
#define BHDM_NUM_PACKET_BYTES  28

#define BHDM_INFOFRAME_PACKET_TYPE 0x80


/******************************************************************************
Summary:
Enumerated Type of different events available from the HDMI code

Description:
The HDMI core supports for five sets of interrupts (HDMI, HDCP, CEC, Hot Plug,
and RAM).  Applications can request notification of events associated with these
interrupts.  This enumeration can be passed to the BHDM_GetEvent function

See Also:
	o BHDM_GetEvent

*******************************************************************************/
typedef enum
{
	BHDM_EventHDMI,
	BHDM_EventHDCP,
	BHDM_EventHDCPRiValue,
	BHDM_EventHDCPPjValue,
	BHDM_EventHDCPRepeater,
	BHDM_EventHotPlug,
	BHDM_EventFIFO, /* debugging event */
	BHDM_EventRAM,   /* debugging event */
	BHDM_EventRxSense	
} BHDM_EventType ;


/******************************************************************************
Summary:
Enumerated Type of the output destinations for the HDMI data.
This enumeration is used in the Default Settings used by BHDM_Open

Description:
The HDMI core must know where to place the output DVI/HDMI data.
This table enumerates those types

   BHDM_OutputPort_eDVO
   BHDM_OutputPort_eHDMI


See Also:
	o BHDM_Open

*******************************************************************************/
typedef enum
{
	BHDM_OutputPort_eDVI,
	BHDM_OutputPort_eDVO = BHDM_OutputPort_eDVI,
	BHDM_OutputPort_eHDMI
} BHDM_OutputPort ;



/******************************************************************************
Summary:
Enumerated Type of the format of the data from the HDMI Core
This enumeration is used in the Default Settings used by BHDM_Open

Description:
The HDMI core must know which type (DVI / HDMI) of data to send to the Output
Port

   BHDM_OutputFormat_eDVO
   BHDM_OutputFormat_eHDMI
   BHDM_OutputFormat_e12BitDVOMode
   BHDM_OutputFormat_e24BitDVOMode
   BHDM_OutputFormat_e768pPanel
   BHDM_OutputFormat_e1080pPanel
   BHDM_OutputFormat_eSingleChannel1_DVOMode,
   BHDM_OutputFormat_eSingleChannel2_DVOMOde,
   BHDM_OutputFormat_eDualChannel1_DVOMode,
   BHDM_OutputFormat_eDualChannel2_DVOMode

See Also:
	o BHDM_Open

*******************************************************************************/
typedef enum
{	
	BHDM_OutputFormat_eDVIMode, 
	BHDM_OutputFormat_eHDMIMode,
	BHDM_OutputFormat_e12BitDVOMode, 
	BHDM_OutputFormat_e24BitDVOMode, 
	BHDM_OutputFormat_e768pPanel
} BHDM_OutputFormat ;


/******************************************************************************
Summary:
Enumerated Type of the type of frames to be sent to the HDMI Rx

Description:
The enumeration can be used to configure packets for sending to the HDMI Rx

*******************************************************************************/
typedef enum
{
	BHDM_PACKET_eGCP_ID = 0,
	BHDM_PACKET_eAVI_ID,
	BHDM_PACKET_eAudioFrame_ID, 
	BHDM_PACKET_eSPD_ID,
	BHDM_Packet_eVendorSpecific_ID,
	BHDM_PACKET_eGamutMetadata_ID,
	BHDM_Packet_eUser1,
	BHDM_Packet_eUser2,
	BHDM_Packet_eUser3,
	BHDM_Packet_eUser4,
	BHDM_Packet_eUser5,
	BHDM_Packet_eUser6,
	BHDM_Packet_eUser7,
	BHDM_Packet_eUser8
} BHDM_Packet ;


/* Moved BHDM_AVI_Packet_XXXX enumerations to 
   magnum/commonutils/bavc/bavc_hdmi.h */

/******************************************************************************
Summary:
Enumerated Type to describe changes to the Audio or Video Rates input to 
the HDMI core 

Description:
The enumeration can be used to set up callbacks to the HDMI 


See Also:
*******************************************************************************/
typedef enum
{
	BHDM_Callback_Type_eUnknown = 0,
	BHDM_Callback_Type_eAudioChange,
	BHDM_Callback_Type_eVideoChange,
	BHDM_Callback_Type_eManualAudioChange	
} BHDM_Callback_Type ;



/******************************************************************************
Summary:
Enumerated Type for Pre-Emphasis Control 

Description:
The enumeration can be used to set up values for Pre-Emphasis Control 


See Also:
	o BHDM_EnablePreEmphasis
*******************************************************************************/
typedef enum 
{
	BHDM_PreEmphasis_eOFF =  0,
	BHDM_PreEmphasis_eDeepColorMode = 4,
	BHDM_PreEmphasis_eLOW =  5,
	BHDM_PreEmphasis_eMED = 10,
	BHDM_PreEmphasis_eMAX = 15
} BHDM_PreEmphasisSetting ;


/******************************************************************************
Summary:
Pre-emphasic configurations settings

Description:
This structure can be used to set up values for Pre-Emphasis Control 


See Also:
	o BHDM_GetPreEmphasisConfiguration
	o BHDM_SetPreEmphasisConfiguration
*******************************************************************************/
typedef struct _BHDM_PREEMPHASIS_CONFIGURATION_
{
	uint16_t uiPreEmphasis_CK;
	uint16_t uiPreEmphasis_Ch0;
	uint16_t uiPreEmphasis_Ch1;
	uint16_t uiPreEmphasis_Ch2; 

	/* The following fields only applicable to 65nm platforms. */
	uint8_t uiDriverAmp;
	uint8_t uiRefCntl;
	uint8_t uiTermR;
	
	/* The following fields only applicable to 40nm platforms */
	uint8_t  uiHfEn;
	uint16_t uiCurrentRatioSel;
	uint8_t  uiKP;
	uint8_t  uiKI;
	uint8_t  uiKA;
} BHDM_PreEmphasis_Configuration;


/******************************************************************************
Summary:
Enumerated Type for Source Product Description Frame

Description:
The enumeration can be used to set up values for he HDMI Source Product 
Description Frame  


See Also:
	o BHDM_SetSPDInfoFramePacket
*******************************************************************************/
typedef enum 
{
	BHDM_SPD_SourceDevice_eUnknown,
	BHDM_SPD_SourceDevice_eDigitalSTB,   
	BHDM_SPD_SourceDevice_eDVD,
	BHDM_SPD_SourceDevice_eDVHS,         
	BHDM_SPD_SourceDevice_eHDDVideo,     
	BHDM_SPD_SourceDevice_eDVC,    
	BHDM_SPD_SourceDevice_eDSC,           
	BHDM_SPD_SourceDevice_eVideoCD,      
	BHDM_SPD_SourceDevice_eGame,          
	BHDM_SPD_SourceDevice_ePC,     
	BHDM_SPD_SourceDevice_eBD, 
	BHDM_SPD_SourceDevice_eSuperAudioCD
} BHDM_SPD_SourceDevice ;



/******************************************************************************
Summary:
Enumerated Type for HDMI Audio Channels 

Description:
The enumeration can be used to set up the Audio Channels used in the Audio Infoframe Packets
and MAI bus.


See Also:
*******************************************************************************/
typedef enum 
{

	/* BHDM_ChannelAllocation_e_Front_Rear_Subwoofer */
	/* See CEA-861-x Audio Info frame */
	 BHDM_ChannelAllocation_e_xx__xx__xx__xx__xx___xx_FR__FL =0,
	 	BHDM_ChannelAllocation_eStereo = 0,
	 	BHDM_ChannelAllocation_e2_0_0 = 0,
	 	
	 BHDM_ChannelAllocation_e_xx__xx__xx__xx__xx__LFE_FR__FL = 1,
	 	BHDM_ChannelAllocation_e2_0_1 = 1, 
	 
	 BHDM_ChannelAllocation_e_xx__xx__xx__xx__FC___xx_FR__FL = 2,
	 	BHDM_ChannelAllocation_e3_0_0 = 2, 
	 	
	 BHDM_ChannelAllocation_e_xx__xx__xx__xx__FC__LFE_FR__FL = 3, 
	 	BHDM_ChannelAllocation_e3_0_1 = 3, 
	 	
	 BHDM_ChannelAllocation_e_xx__xx__xx__RC__xx___xx_FR__FL = 4,
	 	BHDM_ChannelAllocation_e2_1_0 = 4, 
	 
	 BHDM_ChannelAllocation_e_xx__xx__xx__RC__xx__LFE_FR__FL = 5,
	 	BHDM_ChannelAllocation_e2_1_1 = 5, 
	 
	 BHDM_ChannelAllocation_e_xx__xx__xx__RC__FC___xx_FR__FL = 6,
	 	BHDM_ChannelAllocation_e3_1_0 = 6, 
	 	
	 BHDM_ChannelAllocation_e_xx__xx__xx__RC__FC__LFE_FR__FL = 7,
	 	BHDM_ChannelAllocation_e3_1_1 = 7, 
	 
	 BHDM_ChannelAllocation_e_xx__xx__RR__RL__xx___xx_FR__FL = 8,
	 	BHDM_ChannelAllocation_e2_2_0 = 8, 
	 	
	 BHDM_ChannelAllocation_e_xx__xx__RR__RL__xx__LFE_FR__FL = 9,
	 	BHDM_ChannelAllocation_e2_2_1 = 9, 
	 
	 BHDM_ChannelAllocation_e_xx__xx__RR__RL__FC___xx_FR__FL = 10,
	 	BHDM_ChannelAllocation_e3_2_0 = 10, 
	 
	 BHDM_ChannelAllocation_e_xx__xx__RR__RL__FC__LFE_FR__FL = 11,
		BHDM_ChannelAllocation_e5_1 = 11,		  /* 5.1 */
		BHDM_ChannelAllocation_e3_2_1 = 11, 
		
	 BHDM_ChannelAllocation_e_xx__RC__RR__RL__xx___xx_FR__FL = 12,
	 	BHDM_ChannelAllocation_e2_3_0 = 12, 
	 
	 BHDM_ChannelAllocation_e_xx__RC__RR__RL__xx__LFE_FR__FL = 13,
	 	BHDM_ChannelAllocation_e2_3_1 = 13, 
	 	
	 BHDM_ChannelAllocation_e_xx__RC__RR__RL__FC___xx_FR__FL = 14,
	 	BHDM_ChannelAllocation_e3_3_0 = 14, 
	 	
	 BHDM_ChannelAllocation_e_xx__RC__RR__RL__FC__LFE_FR__FL = 15,
	 	BHDM_ChannelAllocation_e3_3_1 = 15,

	 BHDM_ChannelAllocation_e_RRC_RLC__RR__RL__xx__xx_FR__FL = 16,
 		BHDM_ChannelAllocation_e2_4_0 = 16,

	 BHDM_ChannelAllocation_e_RRC_RLC__RR__RL__xx_LFE_FR__FL = 17,
 		BHDM_ChannelAllocation_e2_4_1 = 17,

	 BHDM_ChannelAllocation_e_RRC_RLC__RR__RL__FC__xx_FR__FL = 18,
 		BHDM_ChannelAllocation_e3_4_0 = 18,

	 BHDM_ChannelAllocation_e_RRC_RLC__RR__RL__FC_LFE_FR__FL = 19,
 		BHDM_ChannelAllocation_e3_4_1 = 19,

	 BHDM_ChannelAllocation_e_FRC_FLC__xx__xx__xx__xx_FR__FL = 20,
 		BHDM_ChannelAllocation_e4_0_0 = 20,

	 BHDM_ChannelAllocation_e_FRC_FLC__xx__xx__xx_LFE_FR__FL = 21,
 		BHDM_ChannelAllocation_e4_0_1 = 21,

	 BHDM_ChannelAllocation_e_FRC_FLC__xx__xx__FC__xx_FR__FL = 22,
 		BHDM_ChannelAllocation_e5_0_0 = 22,

	 BHDM_ChannelAllocation_e_FRC_FLC__xx__xx__FC_LFE_FR__FL = 23,
 		BHDM_ChannelAllocation_e5_0_1 = 23,

	 BHDM_ChannelAllocation_e_FRC_FLC__xx__RC__xx__xx_FR__FL = 24,
 		BHDM_ChannelAllocation_e4_1_0 = 24,

	 BHDM_ChannelAllocation_e_FRC_FLC__xx__RC__xx_LFE_FR__FL = 25,
 		BHDM_ChannelAllocation_e4_1_1 = 25,

	 BHDM_ChannelAllocation_e_FRC_FLC__xx__RC__FC__xx_FR__FL = 26,
 		BHDM_ChannelAllocation_e5_1_0 = 26,

	 BHDM_ChannelAllocation_e_FRC_FLC__xx__RC__FC_LFE_FR__FL = 27,
 		BHDM_ChannelAllocation_e5_1_1 = 27,

	 BHDM_ChannelAllocation_e_FRC_FLC__RR__RL__xx__xx_FR__FL = 28,
 		BHDM_ChannelAllocation_e4_2_0 = 28,

	 BHDM_ChannelAllocation_e_FRC_FLC__RR__RL__xx_LFE_FR__FL = 29,
 		BHDM_ChannelAllocation_e4_2_1 = 29,

	 BHDM_ChannelAllocation_e_FRC_FLC__RR__RL__FC__xx_FR__FL = 30,
 		BHDM_ChannelAllocation_e5_2_0 = 30,

	 BHDM_ChannelAllocation_e_FRC_FLC__RR__RL__FC_LFE_FR__FL = 31,
 		BHDM_ChannelAllocation_e5_2_1 = 31

} BHDM_AudioChannel  ;


/* Moved AVI info typedef to magnum/commonutils/bavc/bavc_hdmi.h */


/***************************************************************************
Summary:
	Prototype for external modules to install callback with HDMI to notify of changes

Description:

Input:
	pvParam1 - User defined data structure casted to void.
	iParam2 - Additional user defined value.
	pvData - Data to pass from HDMI to application thru callback. 

Output:
	None

See Also:
	None
**************************************************************************/
typedef void (*BHDM_CallbackFunc)
	( void							  *pvParam1,
	  int							   iParam2,
	  void							  *pvData );


/***************************************************************************
Summary:
HDMI settings used for BHDM_Open().

See Also
	o BHDM_GetDefaultSettings
****************************************************************************/
typedef struct _BHDM_Settings_
{

	/* 
	--NOTE: modifications to this BHDM_Settings structure must be 
	-- accounted for in the BHDM_P_HdmiSettingsChange function
	*/
	BFMT_VideoFmt           eInputVideoFmt ;

	BAVC_Timebase           eTimebase ;

	BHDM_OutputPort         eOutputPort ;
	BHDM_OutputFormat       eOutputFormat ;

	BAVC_MatrixCoefficients	eColorimetry ;
	BFMT_AspectRatio eAspectRatio ;
	BAVC_HDMI_PixelRepetition ePixelRepetition ;

#if BHDM_CONFIG_HDMI_1_3_SUPPORT
	BHDM_ColorDepth_Settings stColorDepth;
#endif

	/**** AVI Info Frame Structure ****/	
	BAVC_HDMI_AviInfoFrame stAviInfoFrame ;
	/**********************************/	
	
	/**** Audio Info Frame Structure ****/	
	BAVC_HDMI_AudioInfoFrame stAudioInfoFrame ;
	/**********************************/	
	BAVC_AudioSamplingRate  eAudioSamplingRate ;
	BAVC_AudioFormat eAudioFormat ;
	BAVC_AudioBits eAudioBits ;
	
	BHDM_SPD_SourceDevice eSpdSourceDevice ;
	uint8_t SpdVendorName[BAVC_HDMI_SPD_IF_VENDOR_LEN+1] ;
	uint8_t SpdDescription[BAVC_HDMI_SPD_IF_DESC_LEN+1] ;

	/**** Vendor Specific Info Frame Structure ****/	
	BAVC_HDMI_VendorSpecificInfoFrame stVendorSpecificInfoFrame ;
	/**********************************/	

	/* Currently this parameter is not being used */	
	bool CalculateCts  ;

	uint8_t uiDriverAmpDefault;
	
	bool AltDvoPath ;
	
	bool BypassEDIDChecking ;  /* debug tool */
	bool bFifoMasterMode;      /* Set true to enable master mode */

	bool bForceEnableDisplay; 	/* Set to true to always enable display even when
								using the same HDMI settings */
	bool bEnableAutoRiPjChecking;	/* Set to true to enable auto Ri, Pj checking handle by HW */									
	BTMR_Handle hTMR ;
								
} BHDM_Settings;


typedef struct _BHDM_CrcData_
{
	uint16_t crc;
} BHDM_CrcData;


/******************************************************************************
Summary:
Get the default settings for the HDMI device.

Input:
	<None>

Output:
   pBHDM_Settings  - pointer to memory to hold default settings

Returns:
	BERR_SUCCESS - Successfully opened HDMI connection.
	BERR_INVALID_PARAMETER - Invalid function parameter.

See Also:

*******************************************************************************/
BERR_Code BHDM_GetDefaultSettings(
   BHDM_Settings *pBHDM_DefaultSettings  /* [out] pointer to memory to hold default settings */
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

	hI2cRegHandle - The I2C handle that the application created earler during the
	initialization sequence.  This handle is used to access the HDMI Rx.

	pBHDM_Settings - The default settings for configuring the HDMI connection. This
	sturcture can be queried prior to BHDM_Open with BHDM_GetDefaultSettings.
	This parameter can be NULL.  In this case the default setting will be used.

Output:
	phHDMI - pointer to previously allocated HDMI structure

Returns:
	BERR_SUCCESS - Successfully opened HDMI connection.
	BERR_INVALID_PARAMETER - Invalid function parameter.

See Also:
	BHDM_Handle
	BREG_Handle
	BI2C_ChannelHandle

*******************************************************************************/
BERR_Code BHDM_Open(
   BHDM_Handle *phHDMI,       /* [out] HDMI handle */
   BCHP_Handle hChip,         /* [in] Chip handle */
   BREG_Handle hRegister,     /* [in] Register Interface to HDMI Tx Core */
   BINT_Handle hInterrupt,    /* [in] Interrupt handle */
   BREG_I2C_Handle hI2cRegHandle,      /* [in] I2C Interface to HDMI Rx */
   const BHDM_Settings  *pBHDM_Settings /* [in] default HDMI settings */
) ;



/******************************************************************************
Summary:
Close the HDMI connection to the HDMI Rx.

Description:
This function will close the HDMI connection to the HDMI Rx.  A new HDMI
connection will have to be opened to display to the HDMI Rx again.

Input:
	hHDMI - HDMI control handle that was previously opened by BHDM_Open.

Output:
	<None>

Returns:
	BERR_SUCCESS - Successfully closed the HDMI connection.
	BERR_INVALID_PARAMETER - Invalid function parameter.

See Also:
	BHDM_Handle
*******************************************************************************/
BERR_Code BHDM_Close(
   BHDM_Handle hHDMI  /* [in] HDMI handle */
) ;



/******************************************************************************
Summary:
Clear the Hot Plug Interrupt for the next event.

Input:
	hHDMI - HDMI control handle that was previously opened by BHDM_Open.

Output:
	<None>

Returns:
	BERR_SUCCESS - Successfully reset the HotPlug interrupt.
	BERR_INVALID_PARAMETER - Invalid function parameter.

See Also:
	BHDM_Handle

*******************************************************************************/
BERR_Code BHDM_ClearHotPlugInterrupt(
   BHDM_Handle hHDMI        /* [in] HDMI handle */
) ;


/******************************************************************************
Summary:
Check the Hot Plug Interrupt to see if there was a change (since last Clear).

Input:
	hHDMI - HDMI control handle that was previously opened by BHDM_Open.

Output:
	<None>

Returns:
	BERR_SUCCESS - Successfully checked the HotPlug interrupt.

See Also:
	BHDM_Handle

*******************************************************************************/
BERR_Code BHDM_CheckHotPlugInterrupt(
	BHDM_Handle hHDMI,         /* [in] HDMI handle */
	uint8_t *bHotPlugInterrupt /* [out] HPD changed or not */
) ;


/******************************************************************************
Summary:
Check for an attached Rx Device.

Description:
Check if a receiver is attached to the HDMI transmitter


Input:
	hHDMI - HDMI control handle that was previously opened by BHDM_Open.

Output:
	DeviceAttached - flag indicating if a receiver device is attached or not

Returns:
	BERR_SUCCESS           - Rx Device is Attached.
	BERR_INVALID_PARAMETER - Invalid function parameter.
	BHDM_NO_RX_DEVICE      - No HDMI/DVI Rx Device is Attached.

See Also:
	BHDM_Handle

*******************************************************************************/
BERR_Code BHDM_RxDeviceAttached(
   BHDM_Handle hHDMI,		/* [in] HDMI handle */
   uint8_t *RxDeviceAttached
) ;



/******************************************************************************
Summary:
Reset HDMI/DVI Rx device (for non-compliant receivers)

Description:
Some Silicon Image Receivers do not properly reset.  The HDCP Specification
calls for a reset of the DVI Rx when the HDCP Bksv value is written to the
Rx.  The SI Rxs require the TMDS output be disabled for 100 ms.  This causes
a temporary blanking of the screen; therefore this function should only be
used when initial HDCP Authentication fails.

Input:
	hHDMI - HDMI control handle that was previously opened by BHDM_Open.

Output:
	<None>

Returns:
	BERR_SUCCESS - Commands successfully initiated to Reset Rx Device.
	BERR_INVALID_PARAMETER - Invalid function parameter.

See Also:
	BHDM_Handle

*******************************************************************************/
BERR_Code BHDM_ResetHdmiRx(
   BHDM_Handle hHDMI	    /* [in] HDMI handle */
) ;



/******************************************************************************
Summary:
Enable (Display) TMDS Output

Input:
	hHDMI - HDMI control handle that was previously opened by BHDM_Open.
	bEnableTmdsOutput  - True=Enable TMDS   False=Disable TMDS


Output:
	<None>

Returns:
	BERR_SUCCESS - TMDS Output Enabled.
	BERR_INVALID_PARAMETER - Invalid function parameter.

See Also:
	BHDM_Handle

*******************************************************************************/
BERR_Code BHDM_EnableTmdsOutput(
   BHDM_Handle hHDMI,		/* [in] HDMI handle */
   bool bEnableTmdsOutput   /* [in] boolean to enable/disable */
) ;




/******************************************************************************
Summary:
Set the AvMute (True/False) functionality for HDMI.

Description:
	The AvMute (TRUE) signals to the HDMI Receiver that the current audio and
	video data can be considered invalid.  The HDMI Receiver can use this to
	blank the display and mute the audio to minimize problems for the end user.

	AvMute (FALSE) signals the HDMI Receiver that it can begin processing audio
	and video data valid.

	This function is useful to notify the Receiver of changes in the TMDS clock
	frequency (i.e. channel change, format change etc.)


Input:
	hHDMI - The HDMI device handle that the application created earlier
	during the system initialization sequence.

	bEnableAvMute - Disable/Enable handling audio/video data.

Output:
	<none>

Returns:
	BERR_SUCCESS - AVMute was enabled/disabled
	BERR_INVALID_PARAMETER - Invalid function parameter.

Note:
	Both the audio and video our muted at the receiver.  They are not mutually
	exclusive.

See Also:
	o BHDM_Handle
	o BHDM_Open

*******************************************************************************/
BERR_Code BHDM_SetAvMute(
   BHDM_Handle hHDMI,              /* [in] HDMI handle */
   bool bEnableAvMute              /* [in] boolean to enable/disable */
) ;


/******************************************************************************
Summary:
	Mute/Unmute Audio on the HDMI interface.

Description:
	This function may be used to mute just the audio on HDMI. This is
	independent of the AvMute functionality (done via GCP). It may
	be used in conjunction with BHDM_SetPixelDataOverride to implement 
	an A/V mute that does not depend on the receiver's handling of the
	AV_MUTE flag in the GCP.


Input:
	hHDMI - The HDMI device handle that the application created earlier
	during the system initialization sequence.

	bEnableAudioMute - Disable/Enable handling audio data.

Output:
	<none>

Returns:
	BERR_SUCCESS - AVMute was enabled/disabled
	BERR_INVALID_PARAMETER - Invalid function parameter.

Note:
	None

See Also:
	o BHDM_Handle
	o BHDM_Open

*******************************************************************************/
BERR_Code BHDM_SetAudioMute(
	BHDM_Handle hHDMI,			   /* [in] HDMI handle */
	bool bEnableAudioMute		   /* [in] boolean to enable/disable */
) ;





#if defined(HUMAX_PLATFORM_BASE)
BERR_Code BHDM_SetVideoBlank(
	BHDM_Handle hHDMI,              /* [in] HDMI handle */
	bool bEnableBlank              /* [in] boolean to enable/disable */
);
#endif



/******************************************************************************
Summary:
Initialize the Drift FIFO

Input:
	hHDMI - HDMI control handle that was previously opened by BHDM_Open.

Output:
	<None>

Returns:
	BERR_SUCCESS -
	BERR_INVALID_PARAMETER - Invalid function parameter.

See Also:
	BHDM_Handle

*******************************************************************************/
BERR_Code BHDM_InitializeDriftFIFO(
   BHDM_Handle hHDMI		/* [in] HDMI handle */
) ;



/***************************************************************************
Summary:
	Get the event handle for checking HDMI events.

Input:
	hHDMI - HDMI control handle that was previously opened by BHDM_Open.
	EventType - Type of event the requested handle is for (CEC, HDCP, etc.)

Output:
	pBHDMEvent - HDMI Event Handle

Returns:
	BERR_SUCCESS - Sucessfully returned the HDMI handle
	BERR_INVALID_PARAMETER - Invalid function parameter.

See Also:
	BHDM_EventType

****************************************************************************/
BERR_Code BHDM_GetEventHandle(
   BHDM_Handle hHDMI,           /* [in] HDMI handle */
   BHDM_EventType EventType,    /* [in] HDMI Event Type */
   BKNI_EventHandle *pBHDMEvent	/* [out] event handle */
);



/***************************************************************************
Summary:
	Initialize the HDMI Packet RAM

Input:
	hHDMI - HDMI control handle that was previously opened by BHDM_Open.

Output:
	<None>

Returns:
	BERR_SUCCESS - Sucessfully returned the HDMI handle
	BERR_INVALID_PARAMETER - Invalid function parameter.

See Also:

****************************************************************************/
BERR_Code BHDM_InitializePacketRAM(
   BHDM_Handle hHDMI          /* [in] HDMI handle */
) ;


/***************************************************************************
Summary:
	Callback used for notification of changes to the Audio or Video Rate.

Input:
	hHDMI  - HDMI control handle that was previously opened by BHDM_Open.
	CallbackType -  see BHDM_Callback_Type ;
	pvAudioOrVideoData - void pointer to structure containing information 
	describing Audio or Video rate change

Output:
	<None>

Returns:
	<None>

See Also:
	BHDM_Callback_Type
	BVDC_Display_InstallRateChangeCallback
	BAUD_InstallRateChangeCallback
****************************************************************************/
void BHDM_AudioVideoRateChangeCB_isr(
	BHDM_Handle hHDMI,
	int   CallbackType,
	void *pvAudioOrVideoData) ;


/***************************************************************************
Summary:
	Enable the HDMI/DVI display output
	

Input:
	hHDMI - HDMI control handle that was previously opened by BHDM_Open.
	
    pHdmiSettings - Settings used to enable DVI/HDMI output 
    
Output:
	<None>

Returns:
	BERR_SUCCESS - Enabled the display output
	BERR_INVALID_PARAMETER - Invalid function parameter.

See Also:

****************************************************************************/
BERR_Code BHDM_EnableDisplay(
   BHDM_Handle hHDMI,          /* [in] HDMI handle */
   BHDM_Settings *pHdmiSettings  /* [in] HDMI Settings to enable Display with */
) ;



/***************************************************************************
Summary:
	Disable HDMI/DVI display output
	

Input:
	hHDMI - HDMI control handle that was previously opened by BHDM_Open.
	
    
Output:
	<None>

Returns:
	BERR_SUCCESS - Display output disabled 
	BERR_INVALID_PARAMETER - Invalid function parameter.

See Also:
	BHDM_EnableDisplay
	BHDM_EnableTmdsOutput 

****************************************************************************/
BERR_Code BHDM_DisableDisplay(
   BHDM_Handle hHDMI          /* [in] HDMI handle */
) ;



/***************************************************************************
Summary:
	Get the current settings used for confuring the HDMI display and audio
	

Input:
	hHDMI - HDMI control handle that was previously opened by BHDM_Open.
	
Output:
    pHdmiSettings - Settings used to enable DVI/HDMI output 
    

Returns:
	BERR_SUCCESS - Settings returned in the pHdmiSettings structure
	BERR_INVALID_PARAMETER - Invalid function parameter.

See Also:

****************************************************************************/
BERR_Code BHDM_GetHdmiSettings(
   BHDM_Handle hHDMI,          /* [in] HDMI handle */
   BHDM_Settings *pHdmiSettings  /* [in] HDMI Settings to enable Display with */
) ;


/***************************************************************************
Summary:
	Set the current settings used to configure the HDMI display and audio
	

Input:
	hHDMI - HDMI control handle that was previously opened by BHDM_Open.
	pHdmiSettings - Settings used to enable DVI/HDMI output 
	

Returns:
	BERR_SUCCESS - Settings returned in the pHdmiSettings structure

See Also:
	BHDM_EnableDisplay
****************************************************************************/
BERR_Code BHDM_SetHdmiSettings(
	BHDM_Handle hHDMI, /* [in] handle to HDMI device */
	BHDM_Settings *pHdmiSettings  /* [in] pointer to memory to hold the current  HDMI settings */
);


/***************************************************************************
Summary:
	Set the Timebase for the HDMI Rate Manager to use.

Input:
	hHDMI - HDMI control handle that was previously opened by BHDM_Open.
	eTimebase - the selected Timebase to use
	
Returns:
	BERR_SUCCESS - Timebase Set for use
	BERR_INVALID_PARAMETER - Invalid function parameter.
	
Note:
	The selected Timebase should be the same Timebase used for the VEC 
	Rate Manager configured for HD

See Also:
	BHDM_GetTimebase

****************************************************************************/
BERR_Code BHDM_SetTimebase(
   BHDM_Handle hHDMI,       /* [in] HDMI handle */
   BAVC_Timebase eTimebase  /* [in] HDMI Rate Manager Timebase to be used */
) ;



/***************************************************************************
Summary:
	Get the current Timebase in use by the HDMI Rate Manager 

Input:
	hHDMI - HDMI control handle that was previously opened by BHDM_Open.
	
Output:	
	eTimebase - the current Timebase
	
Returns:
	BERR_SUCCESS - Timebase returned 
	BERR_INVALID_PARAMETER - Invalid function parameter.
	
Note:
	The returned Timebase should be the same Timebase used for the VEC 
	Rate Manager configured for HD

See Also:
	BHDM_SetTimebase
	BHDM_GetHDMISettings
	
****************************************************************************/
BERR_Code BHDM_GetTimebase(
   BHDM_Handle hHDMI,       /* [in] HDMI handle */
   BAVC_Timebase *eTimebase  /* [out] current HDMI Rate Manager Timebase */
) ;



/***************************************************************************
Summary:
	Enable the transmission of the specified packet id

Input:
	hHDMI - HDMI control handle that was previously opened by BHDM_Open.
	ePacketId - Packet ID to enable
	
	
Returns:
	BERR_SUCCESS - Timebase returned 
	BERR_INVALID_PARAMETER - Invalid function parameter.
	

See Also:
	BHDM_DisablePacketTransmission
	BHDM_SetUserDefinedPacket
	
****************************************************************************/
BERR_Code BHDM_EnablePacketTransmission(
   BHDM_Handle hHDMI, 
   BHDM_Packet ePacketId) ;



/***************************************************************************
Summary:
	Disable the transmission of the specified packet id

Input:
	hHDMI - HDMI control handle that was previously opened by BHDM_Open.
	ePacketId - Packet ID to disable
	
	
Returns:
	BERR_SUCCESS - Timebase returned 
	BERR_INVALID_PARAMETER - Invalid function parameter.
	

See Also:
	BHDM_EnablePacketTransmission
	BHDM_SetUserDefinedPacket
	
****************************************************************************/
BERR_Code BHDM_DisablePacketTransmission(
	BHDM_Handle hHDMI, 
	BHDM_Packet ePacketId) ;


	
/***************************************************************************
Summary:
	Create/Set a user defined packet

Input:
	hHDMI - HDMI control handle that was previously opened by BHDM_Open.
	ePacketId - Packet ID to store the user defined packet
	PacketType  - Type ID of the Packet to transmit
	PacketVersion - Version of the Packet to transmit
	PacketLength - length of Packet Data excluding checksum (auto calculated)
	PacketData - pointer to the user packet data
	
	
Returns:
	BERR_SUCCESS - Packet Created and transmitted 
	BERR_INVALID_PARAMETER - Invalid function parameter.
	
Note:
	The Packet Data checksum value is computed automatically and is not part 
	of the Packet Data passed to BHDM_SetUserDefinedPacket.
	
	Caller must manage available User Defined packets; User Packets can be 
	overwritten.

	
See Also:
	BHDM_EnablePacketTransmission
	BHDM_DisablePacketTransmission
****************************************************************************/
BERR_Code BHDM_SetUserDefinedPacket(BHDM_Handle hHDMI, 
	BHDM_Packet ePacketId, 
	
	uint8_t PacketType,
	uint8_t PacketVersion, 
	uint8_t PacketLength,
	
	uint8_t *PacketData
) ;



/***************************************************************************
Summary:
	Configure Pre-Emphasis Control 

Input:
	hHDMI - HDMI control handle that was previously opened by BHDM_Open.
	eValue - enumeration of the value to use on PreEmphasis Control 
	
	
Returns:
	BERR_SUCCESS - Pre-Emphasis Control enabled with the selected value 
	BERR_INVALID_PARAMETER - Invalid function parameter.

		
Note:
	Pre-Emphasis *may* be used for extended DVI/HDMI cable lengths.

	
See Also:
	BHDM_PreEmphasis 
****************************************************************************/
BERR_Code BHDM_ConfigurePreEmphasis(
	BHDM_Handle hHDMI, 
	BHDM_PreEmphasisSetting eValue
) ;


/***************************************************************************
Summary:
	Get current Pre-Emphasis Control settings

Input:
	hHDMI - HDMI control handle that was previously opened by BHDM_Open.
	stPreEmphasisConfig - structure containing all the pre-emphasis settings
	
	
Returns:
	BERR_SUCCESS - Successfully retrieve current Pre-Emphasis settings
		
Note:
	Pre-Emphasis *may* be used for extended DVI/HDMI cable lengths.

See Also:
	BHDM_SetPreEmphasisConfiguration
****************************************************************************/
BERR_Code BHDM_GetPreEmphasisConfiguration(
	BHDM_Handle hHDMI,
	BHDM_PreEmphasis_Configuration *stPreEmphasisConfig
);


/***************************************************************************
Summary:
	Set Pre-Emphasis Control with provided settings

Input:
	hHDMI - HDMI control handle that was previously opened by BHDM_Open.
	stPreEmphasisConfig - structure containing all the pre-emphasis settings to be configured
	
	
Returns:
	BERR_SUCCESS - Pre-Emphasis Control enabled with the selected value 
	BERR_INVALID_PARAMETER - Provided settings are out of range
		
Note:
	Pre-Emphasis *may* be used for extended DVI/HDMI cable lengths.

See Also:
	BHDM_GetPreEmphasisConfiguration
****************************************************************************/
BERR_Code BHDM_SetPreEmphasisConfiguration(
	BHDM_Handle hHDMI,
	BHDM_PreEmphasis_Configuration *stPreEmphasisConfig
);


/******************************************************************************
Summary:
Set/Enable the Source Product Description Info Frame packet to be sent to the HDMI Rx 

Input:
	hHDMI - HDMI control handle that was previously opened by BHDM_Open.
	
	
Returns:
	BERR_SUCCESS - Packet Created and transmitted 
	BERR_INVALID_PARAMETER - Invalid function parameter.
	
	
See Also:
*******************************************************************************/
BERR_Code BHDM_SetSPDInfoFramePacket(
   BHDM_Handle hHDMI          /* [in] HDMI handle */
) ;


	
/******************************************************************************
Summary:
Get Receiver Sense Value 

Input:
	hHDMI - HDMI control handle that was previously opened by BHDM_Open.
	
Output:
	ReceiverSense - 1 Rx Powered On; 0 Rx Powered Off
	
	
Returns:
	BERR_SUCCESS - Receiver Sense returned
	BERR_INVALID_PARAMETER - Invalid function parameter.
	BHDM_HARDWARE_ERROR    - Hardware Error with Device and/or Cable
	
	
See Also:
*******************************************************************************/
BERR_Code BHDM_GetReceiverSense(
   BHDM_Handle hHDMI,
   uint8_t *ReceiverSense          /* [in] HDMI handle */
) ;


/******************************************************************************
Summary:
Set Vendor Specific InfoFrame 

Input:
	hHDMI - HDMI control handle that was previously opened by BHDM_Open.
	stVendorSpecificInfoFrame - structure containing Vendor Specific Info
	
Returns:
	BERR_SUCCESS - Vendor Specific  Info Frame data set
	BERR_INVALID_PARAMETER - Invalid function parameter.
	BHDM_HARDWARE_ERROR    - Hardware Error with Device and/or Cable
	
	
See Also:
	BHDM_GetVendorSpecificInfoFrame
*******************************************************************************/
BERR_Code BHDM_SetVendorSpecificInfoFrame(
	BHDM_Handle hHDMI,          /* [in] HDMI handle */
	const BAVC_HDMI_VendorSpecificInfoFrame *stVendorSpecificInfoFrame
) ;


/******************************************************************************
Summary:
Set Vendor Specific  InfoFrame 

Input:
	hHDMI - HDMI control handle that was previously opened by BHDM_Open.
	pVendorSpecificInfoFrame - pointer to structure to hold the Vendor Specific Infoframe values 
	
	
Returns:
	None	
	
See Also:
	BHDM_SetVendorSpecificInfoFramePacket
*******************************************************************************/
void  BHDM_GetVendorSpecificInfoFrame(
	BHDM_Handle hHDMI,          /* [in] HDMI handle */
	BAVC_HDMI_VendorSpecificInfoFrame *pVendorSpecficInfoFrame
) ;


/******************************************************************************
Summary:
Set AVI InfoFrame 

Input:
	hHDMI - HDMI control handle that was previously opened by BHDM_Open.
	AviInfo - values to set info frame to
	
	
Returns:
	BERR_SUCCESS - Avi Info Frame Packet information set
	BERR_INVALID_PARAMETER - Invalid function parameter.
	BHDM_HARDWARE_ERROR    - Hardware Error with Device and/or Cable
	
	
See Also:
	BHDM_GetAviInfoFramePacket
*******************************************************************************/
BERR_Code BHDM_SetAVIInfoFramePacket(
	BHDM_Handle hHDMI,
	BAVC_HDMI_AviInfoFrame *stAviInfoFrame
) ;



/******************************************************************************
Summary:
Get current AVI InfoFrame Settings

Input:
	hHDMI - HDMI control handle that was previously opened by BHDM_Open.
	AviInfo - structure to copy AVI Info Frame values to 
	
	
Returns:
	BERR_SUCCESS - Avi Info Frame Packet information returned 
	BERR_INVALID_PARAMETER - Invalid function parameter.
	BHDM_HARDWARE_ERROR    - Hardware Error with Device and/or Cable
	
	
See Also:
	BHDM_GetAviInfoFramePacket
*******************************************************************************/
BERR_Code BHDM_GetAVIInfoFramePacket(
	BHDM_Handle hHDMI,
	BAVC_HDMI_AviInfoFrame *stAviInfoFrame
) ;



/******************************************************************************
Summary:
Set Audio InfoFrame 

Input:
	hHDMI - HDMI control handle that was previously opened by BHDM_Open.
	stAudioInfoFrame - values to set info frame to
	
	
Returns:
	BERR_SUCCESS - Avi Info Frame Packet information set
	BERR_INVALID_PARAMETER - Invalid function parameter.
	BHDM_HARDWARE_ERROR    - Hardware Error with Device and/or Cable
	
	
See Also:
	BHDM_GetAudioInfoFramePacket
*******************************************************************************/
BERR_Code BHDM_SetAudioInfoFramePacket(
	BHDM_Handle hHDMI,
	BAVC_HDMI_AudioInfoFrame *stAudioInfoFrame
) ;



/******************************************************************************
Summary:
Get current Audio InfoFrame Settings

Input:
	hHDMI - HDMI control handle that was previously opened by BHDM_Open.
	stAudioInfoFrae - structure to copy Audio Info Frame values to 
	
	
Returns:
	BERR_SUCCESS - Audio Info Frame Packet information returned 
	BERR_INVALID_PARAMETER - Invalid function parameter.
	BHDM_HARDWARE_ERROR    - Hardware Error with Device and/or Cable
	
	
See Also:
	BHDM_GetAudioInfoFramePacket
*******************************************************************************/
BERR_Code BHDM_GetAudioInfoFramePacket(
	BHDM_Handle hHDMI,
	BAVC_HDMI_AudioInfoFrame *stAudioInfoFrame
) ;



/******************************************************************************
Summary:
Get the preferred Colorimetry Setting based on the Output Format (DVI/HDMI)
and current Video Format (SD/HD/PC) Setting

Input:
	eOutputFormat - output (encoding) format sent to display (DVI vs HDMI)
	eVideoFmt - output display format: SD, HD, PC formats
	
Returns:
	BERR_SUCCESS - Avi Info Frame Packet information returned 
	
	
See Also:
	none
*******************************************************************************/
BERR_Code BHDM_GetColorimetry(BHDM_OutputFormat eOutputFormat, 
	BFMT_VideoFmt eVideoFmt, BAVC_MatrixCoefficients *eColorimetry) ;


/******************************************************************************
Summary:
	Set pixel repetition

Input:
	hHDMI - HDMI control handle that was previously opened by BHDM_Open.
	ePixelRepetition - pixel repetition configuration to set
	
	
Returns:
	BERR_SUCCESS - color depth mode successfully set
	BERR_INVALID_PARAMETER - Invalid function parameter.
	BERR_NOT_SUPPORTED - requested color depth is not supported
	
*******************************************************************************/
BERR_Code BHDM_SetPixelRepetition(
	BHDM_Handle hHDMI,		   /* [in] HDMI handle */
	BAVC_HDMI_PixelRepetition ePixelRepetition
) ;


/******************************************************************************
Summary:
	Get pixel repetition

Input:
	hHDMI - HDMI control handle that was previously opened by BHDM_Open.
	*ePixelRepetition - the current pixel repetition configuration 
	
	
Returns:
	BERR_SUCCESS - color depth mode successfully set
	BERR_INVALID_PARAMETER - Invalid function parameter.
	BERR_NOT_SUPPORTED - requested color depth is not supported
	
*******************************************************************************/
BERR_Code BHDM_GetPixelRepetition(
   BHDM_Handle hHDMI,		   /* [in] HDMI handle */
   BAVC_HDMI_PixelRepetition *ePixelRepetition
);


/******************************************************************************
Summary:
	Set the HDMI data transferring mode (Master or Slave)

Input:
	hHDMI - HDMI control handle that was previously opened by BHDM_Open.
	masterMode - a boolean indicating master (true) or slave (false) mode 
	
	
Returns:
	BERR_SUCCESS - data transfer mode successfully set
	
*******************************************************************************/
BERR_Code BHDM_SetHdmiDataTransferMode(
	BHDM_Handle hHDMI,
	bool masterMode
);


/******************************************************************************
Summary:
	Get the current HDMI data transferring mode (Master or Slave)

Input:
	hHDMI - HDMI control handle that was previously opened by BHDM_Open.
	masterMode - a boolean indicating master (true) or slave (false) mode 
	
	
Returns:
	BERR_SUCCESS - successfully retrieve data transfer mode
	
*******************************************************************************/
BERR_Code BHDM_GetHdmiDataTransferMode(
	BHDM_Handle hHDMI,
	bool *masterMode
);


/******************************************************************************
Summary:
	Check for valid/stable video output before starting HDCP authentication

Input:
	hHDMI - HDMI control handle that was previously opened by BHDM_Open.
		
*******************************************************************************/
BERR_Code BHDM_CheckForValidVideo(
	BHDM_Handle hHDMI) ;


/******************************************************************************
Summary:
	install Hot Plug Change Callback to notify of HP dtect changes 

Input:
	hHDMI - HDMI control handle that was previously opened by BHDM_Open.
	pfCallback_isr - callback for hotplug change
	
*******************************************************************************/
BERR_Code BHDM_InstallHotplugChangeCallback(
	BHDM_Handle hHDMI,			/* [in] HDMI Handle */
	const BHDM_CallbackFunc pfCallback_isr, /* [in] cb for informing hotplug change */
	void *pvParm1, /* [in] the first argument (void *) passed to the callback function */ 
	int iParm2) ;	/* [in] the second argument(int) passed to the callback function */ 


/******************************************************************************
Summary:
	Uninstall HotPlug Change Callback

Input:
	hHDMI - HDMI control handle that was previously opened by BHDM_Open.
	pfCallback_isr - callback for hotplug change
	
*******************************************************************************/
BERR_Code BHDM_UnInstallHotplugChangeCallback( 
	BHDM_Handle hHDMI,						 /* [in] HDMI Handle */
	const BHDM_CallbackFunc pfCallback_isr) ; /* [in] cb for hotplug change */


/******************************************************************************
Summary:
	install Rx Sense Change Callback to notify of HP dtect changes 

Input:
	hHDMI - HDMI control handle that was previously opened by BHDM_Open.
	pfCallback_isr - callback for RxSense change
	
*******************************************************************************/
BERR_Code BHDM_InstallRxSenseChangeCallback(
	BHDM_Handle hHDMI,			/* [in] HDMI Handle */
	const BHDM_CallbackFunc pfCallback_isr, /* [in] cb for informing RxSense change */
	void *pvParm1, /* [in] the first argument (void *) passed to the callback function */ 
	int iParm2) ;	/* [in] the second argument(int) passed to the callback function */ 


/******************************************************************************
Summary:
	Uninstall RxSense Change Callback

Input:
	hHDMI - HDMI control handle that was previously opened by BHDM_Open.
	pfCallback_isr - callback for RxSense change
	
*******************************************************************************/
BERR_Code BHDM_UnInstallRxSenseChangeCallback( 
	BHDM_Handle hHDMI,						 /* [in] HDMI Handle */
	const BHDM_CallbackFunc pfCallback_isr) ; /* [in] cb for RxSense change */


/***************************************************************************
Summary:
	HDMI standby settings
	
****************************************************************************/
typedef struct BHDM_StandbySettings
{
	bool bEnableWakeup; /* If true, then allows wakeup from standby using HDM. 
	                       If false, the device is powered down during standby */
} BHDM_StandbySettings;

/******************************************************************************
Summary:
	Get default HDMI standby settings
	
*******************************************************************************/
void BHDM_GetDefaultStandbySettings(
	BHDM_StandbySettings *pSettings
	);


/******************************************************************************
Summary:
	Enter standby mode
	
*******************************************************************************/
BERR_Code BHDM_Standby(
	BHDM_Handle hHDMI, /* [in] HDMI Handle */
	const BHDM_StandbySettings *pSettings /* optional */
	);

/******************************************************************************
Summary:
	Resume from standby mode
	
*******************************************************************************/
BERR_Code BHDM_Resume(
	BHDM_Handle hHDMI /* [in] HDMI Handle */
	);


BERR_Code BHDM_GetCrcValue_isr(
	BHDM_Handle hHDMI,	/* [in] HDMI Handle */
	BHDM_CrcData *stCrcData
);


#ifdef __cplusplus
}
#endif

#endif /* BHDM_H__ */

