/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhdr_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/75 $
 * $brcm_Date: 9/24/12 5:53p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hdr/src/common/bhdr_priv.h $
 * 
 * Hydra_Software_Devel/75   9/24/12 5:53p rgreen
 * SW7445-24:  Add 28nm support
 * 
 * Hydra_Software_Devel/74   7/18/12 9:30a rgreen
 * SW7435-73: Merge Changes
 * 
 * Hydra_Software_Devel/SW7435-73/1   7/13/12 2:57p rgreen
 * SW7425-73: Filter DVI/HDMI mode changes
 * 
 * Hydra_Software_Devel/73   4/24/12 2:32p rgreen
 * SW7425-2075: Merge changes
 * 
 * Hydra_Software_Devel/SW7425-2075/1   4/12/12 11:58a jgarrett
 * SW7425-2075: Removing MAI Channel Reordering in HBR mode
 * 
 * Hydra_Software_Devel/72   1/30/12 4:32p rgreen
 * SW7429-54,SW7435-34: Add workaround to update MAI bus configuration on
 * sample rate changes detected at vsync.  Update BHDR_GetAudioSampleFreq
 * to an iisr declaration
 * 
 * Hydra_Software_Devel/71   8/11/11 11:18a rgreen
 * SWDTV-5441:  Free TMR resources on exit; include timer code for
 * required platforms only; add static declarations
 * 
 * Hydra_Software_Devel/70   8/2/11 6:15p rgreen
 * SW7425-692:  Update power management support for standby/resume
 * 
 * Hydra_Software_Devel/69   7/7/11 5:03p rgreen
 * SW7425-833: Merge changes to use BDBG_OBJECT_ASSERT
 * 
 * Hydra_Software_Devel/SW7425-833/1   7/1/11 1:54p rgreen
 * SW7425-833:  Add edbug support for BDBG_OBJECT_ASSERT
 * 
 * Hydra_Software_Devel/68   6/22/11 11:14a rgreen
 * SWDTV-6771: Add compile time option support for ISRC packets
 * 
 * Hydra_Software_Devel/67   6/8/11 5:10p rgreen
 * SW7425-692: Initial power management support
 * 
 * Hydra_Software_Devel/66   5/12/11 2:32p rgreen
 * SWDTV-6771: Add magnum ISRCn packet parsing support
 * 
 * Hydra_Software_Devel/64   4/18/11 12:11p rgreen
 * SW7422-186:  Merge HDMI HDCP Repeater Support
 * 
 * Hydra_Software_Devel/SW7422-186/3   4/13/11 1:55p rgreen
 * SW7422-186: Replace ksvFifoRequest callback with hdcpStatus change
 * callback
 * 
 * Hydra_Software_Devel/SW7422-186/2   4/11/11 2:54p rgreen
 * SW7422-186: Add HDCP Repeater Support
 * 
 * Hydra_Software_Devel/SW7422-186/1   3/31/11 2:37p rgreen
 * SW7422-186: Add HDCP Repeater support
 * 
 * Hydra_Software_Devel/63   3/25/11 4:13p rgreen
 * SW7422-186: Remove duplicate declaration of HDCP Repeater members.
 * 
 * Hydra_Software_Devel/62   3/4/11 3:53p rgreen
 * SWDTV-4664: Fix compilation for HDCP value/status debugging
 * 
 * Hydra_Software_Devel/61   3/3/11 11:48a rgreen
 * SWDTV-4664: Update HDCP Key Set verification and HDCP processing for
 * OTP ROM based chips
 * 
 * Hydra_Software_Devel/60   2/3/11 8:33p rgreen
 * SW35230-2374, SW7422-129: Use HdcpSettings and HdcpStatus across all
 * HDR platforms.  Move KeySetStatus information to HdcpStatus
 * 
 * Hydra_Software_Devel/59   2/3/11 11:19a rgreen
 * SW35230-2882: Restore support for parsing Gamut Meta Data packet;
 * Support is disabled by default (see bhdr_config.h to enable)   Fix
 * naming convention for isr functions
 * 
 * Hydra_Software_Devel/58   2/1/11 5:04p rgreen
 * SW7422-129: Fix 35230,35125 compilation; group HDCP declarations
 * together
 * 
 * Hydra_Software_Devel/57   1/31/11 7:35p rgreen
 * SW7422-129: Implement HDCP Rx Key Loading to Secure OTP RAM
 * 
 * Hydra_Software_Devel/56   1/25/11 4:59p rgreen
 * SWBLURAY-23819: Add support for 7640
 * 
 * Hydra_Software_Devel/55   1/18/11 3:58p rgreen
 * SW7425-21: Use HDMI_RX_GEN macro to determine interrupt table
 * 
 * Hydra_Software_Devel/54   12/28/10 3:46p rgreen
 * SW7422-110: Update HDMI_SYS_CLK_RATE use to calculate Audio Sample Rate
 * 
 * Hydra_Software_Devel/53   12/20/10 3:58p rgreen
 * SW35230-2374: Use state machine to describe status of HDCP KeySet
 * verification from OTP
 * 
 * Hydra_Software_Devel/52   12/7/10 4:59p rgreen
 * SW35230-2416: Add timer support to delay reset of front end;
 * 
 * Hydra_Software_Devel/51   12/2/10 2:43p rgreen
 * SW35230-2374: Add initial support for HDCP Key OTP check
 * 
 * Hydra_Software_Devel/50   11/5/10 3:44p rgreen
 * SW35230-1723:  Remove depracated defines for Color Depth
 * 
 * Hydra_Software_Devel/49   10/12/10 5:42p rgreen
 * SW35230-1598: Add Hdcp Reset function.  Update audio Enable/Disable
 * calls for AvMute
 * 
 * Hydra_Software_Devel/48   10/5/10 10:14a rgreen
 * SW35230-15: Set correct interrupt ID for SET_AV_MUTE
 * 
 * Hydra_Software_Devel/47   9/28/10 2:28p rgreen
 * SW35230-1502: Mute audio path immediately on Symbol Loss Cleanup/Remove
 * unused legacy code
 * 
 * Hydra_Software_Devel/46   9/15/10 5:22p rgreen
 * SW35230-1363,SW7425-21: Move chip/phy specific code to private file
 * 
 * Hydra_Software_Devel/45   9/10/10 2:45p rgreen
 * SW35230-1193: Continually clear packets/interrupts until front end is
 * locked/stable.
 * 
 * Hydra_Software_Devel/43   7/21/10 7:15p rgreen
 * SW35230-568: Improve front end auto frequency range detection Remove
 * unused variables
 * 
 * Hydra_Software_Devel/42   7/14/10 3:58p rgreen
 * SW35230-558: disable unused LAYOUT_UPDATE interrupt  update calculaton
 * for Sample Rate
 * 
 * Hydra_Software_Devel/41   6/7/10 5:17p rgreen
 * SW35230-128: Separate phy specific code to separate file
 * 
 * Hydra_Software_Devel/40   5/28/10 11:13a rgreen
 * SW3548-2951: For debugging, always display current color mode
 * 
 * Hydra_Software_Devel/39   2/19/10 2:45p rgreen
 * SW3548-2642: add support for 3D configuration
 * 
 * Hydra_Software_Devel/38   1/13/10 4:47p rgreen
 * SW3548-2708: Extend support for Vendor Specific InfoFrame
 * 
 * Hydra_Software_Devel/37   1/8/10 4:18p rgreen
 * SW3548-2642: Move packet processing to separate bhdr_packet_priv.c/h
 * file
 * 
 * Hydra_Software_Devel/36   12/31/09 12:44p rgreen
 * SW3548-2013: Always update 'HDCP Ri Updating' status regardless of
 * bhdr_config setting
 * 
 * Hydra_Software_Devel/35   12/1/09 3:06p rgreen
 * SW35230-15: Add temporary workaround for different interrupt IDs
 * 
 * Hydra_Software_Devel/34   11/25/09 1:14p rgreen
 * SW35230-15: Add support for 35230
 * 
 * Hydra_Software_Devel/33   9/30/09 10:34p rgreen
 * SW3548-2013:Eliminate repetitive Hdmi Rx Status messages
 * 
 * Hydra_Software_Devel/32   8/20/09 6:08p rgreen
 * PR57890:
 * Add Symbol Loss SM for debugging
 * Remove unused variables
 * 
 * Hydra_Software_Devel/31   6/5/09 4:17p rgreen
 * PR55586: Disable HDMI Frontend and Receiver interrupts when input is
 * not selected
 * 
 * Hydra_Software_Devel/PR46645/1   5/29/09 5:10p rgreen
 * PR46645,PR55586: Disable HDMI Frontend and Receiver interrupts when
 * input is not selected
 * 
 * Hydra_Software_Devel/54   12/28/10 3:46p rgreen
 * SW7422-110: Update HDMI_SYS_CLK_RATE use to calculate Audio Sample Rate
 * 
 * Hydra_Software_Devel/53   12/20/10 3:58p rgreen
 * SW35230-2374: Use state machine to describe status of HDCP KeySet
 * verification from OTP
 * 
 * Hydra_Software_Devel/52   12/7/10 4:59p rgreen
 * SW35230-2416: Add timer support to delay reset of front end;
 * 
 * Hydra_Software_Devel/51   12/2/10 2:43p rgreen
 * SW35230-2374: Add initial support for HDCP Key OTP check
 * 
 * Hydra_Software_Devel/50   11/5/10 3:44p rgreen
 * SW35230-1723:  Remove depracated defines for Color Depth
 * 
 * Hydra_Software_Devel/49   10/12/10 5:42p rgreen
 * SW35230-1598: Add Hdcp Reset function.  Update audio Enable/Disable
 * calls for AvMute
 * 
 * Hydra_Software_Devel/48   10/5/10 10:14a rgreen
 * SW35230-15: Set correct interrupt ID for SET_AV_MUTE
 * 
 * Hydra_Software_Devel/47   9/28/10 2:28p rgreen
 * SW35230-1502: Mute audio path immediately on Symbol Loss Cleanup/Remove
 * unused legacy code
 * 
 * Hydra_Software_Devel/46   9/15/10 5:22p rgreen
 * SW35230-1363,SW7425-21: Move chip/phy specific code to private file
 * 
 * Hydra_Software_Devel/45   9/10/10 2:45p rgreen
 * SW35230-1193: Continually clear packets/interrupts until front end is
 * locked/stable.
 * 
 * Hydra_Software_Devel/43   7/21/10 7:15p rgreen
 * SW35230-568: Improve front end auto frequency range detection Remove
 * unused variables
 * 
 * Hydra_Software_Devel/42   7/14/10 3:58p rgreen
 * SW35230-558: disable unused LAYOUT_UPDATE interrupt  update calculaton
 * for Sample Rate
 * 
 * Hydra_Software_Devel/41   6/7/10 5:17p rgreen
 * SW35230-128: Separate phy specific code to separate file
 * 
 * Hydra_Software_Devel/40   5/28/10 11:13a rgreen
 * SW3548-2951: For debugging, always display current color mode
 * 
 * Hydra_Software_Devel/39   2/19/10 2:45p rgreen
 * SW3548-2642: add support for 3D configuration
 * 
 * Hydra_Software_Devel/38   1/13/10 4:47p rgreen
 * SW3548-2708: Extend support for Vendor Specific InfoFrame
 * 
 * Hydra_Software_Devel/37   1/8/10 4:18p rgreen
 * SW3548-2642: Move packet processing to separate bhdr_packet_priv.c/h
 * file
 * 
 * Hydra_Software_Devel/36   12/31/09 12:44p rgreen
 * SW3548-2013: Always update 'HDCP Ri Updating' status regardless of
 * bhdr_config setting
 * 
 * Hydra_Software_Devel/35   12/1/09 3:06p rgreen
 * SW35230-15: Add temporary workaround for different interrupt IDs
 * 
 * Hydra_Software_Devel/34   11/25/09 1:14p rgreen
 * SW35230-15: Add support for 35230
 * 
 * Hydra_Software_Devel/33   9/30/09 10:34p rgreen
 * SW3548-2013:Eliminate repetitive Hdmi Rx Status messages
 * 
 * Hydra_Software_Devel/32   8/20/09 6:08p rgreen
 * PR57890:
 * Add Symbol Loss SM for debugging
 * Remove unused variables
 * 
 * Hydra_Software_Devel/31   6/5/09 4:17p rgreen
 * PR55586: Disable HDMI Frontend and Receiver interrupts when input is
 * not selected
 * 
 * Hydra_Software_Devel/30   5/8/09 10:21a rgreen
 * PR54924: Change HDMI_Detected to bHdmiMode to match RDB name
 * 
 * Hydra_Software_Devel/29   5/4/09 6:03p rgreen
 * PR54810: Remove references to unused Gamut Meta Data Packet
 * 
 * Hydra_Software_Devel/28   5/1/09 4:40p rgreen
 * PR54000: Move debug functions to separate bhdr_debug.c file.
 * Add options to dump Infoframes, monitor format changes, monitor values
 * etc
 * 
 * Hydra_Software_Devel/27   2/26/09 7:57p rgreen
 * PR51307: Add compile time option (BHDR_HDCP_REPEATER_SUPPORT) for HDCP
 * Repeater
 * 
 * Hydra_Software_Devel/26   2/25/09 10:46a rgreen
 * PR44505,PR52050: Update AvMute auto clear functionality
 * Use SysClock to calculate Sample Rate instead of N/CTS values, update
 * Mai Format register accordingly
 * Disable SR interrupt, use SR calulations instead
 * 
 * Hydra_Software_Devel/25   2/9/09 3:56p rgreen
 * PR51752,PR51898:
 * Read status registers (GetHdmiRxStatus, GetPixelClockEstimate,
 * GetHdmiRxDetectedVideoFormat, etc)
 * at interrupt time vs task time
 * Add private isr function for retrieving video format
 * 
 * Hydra_Software_Devel/24   1/26/09 5:29p rgreen
 * PR50940: Mute audio until N, CTS values are stable
 * 
 * Hydra_Software_Devel/23   12/19/08 10:13a rgreen
 * PR48775:Add changes for Sample Rate calculation
 * 
 * Hydra_Software_Devel/22   12/17/08 4:12p rgreen
 * PR48775: Use BTMR for calculating time between audio packet counts
 * 
 * Hydra_Software_Devel/21   12/5/08 6:36p rgreen
 * PR48826:Increment size of frame counter
 * 
 * Hydra_Software_Devel/20   11/6/08 11:24a rgreen
 * PR48585: Fix GCP Stop detection for Deep Color
 * 
 * Hydra_Software_Devel/19   10/31/08 2:06p rgreen
 * PR48444,PR46509: Update RDB setting for Channel Status config
 * Fix DCP Compliance test issue by switching back to DVI mode on loss of
 * HDMI stream
 * Add B0 debug interrupts
 * 
 * Hydra_Software_Devel/18   10/21/08 8:39p rgreen
 * PR48206: Fix MAI FORMAT register to match raptor
 * 
 * Hydra_Software_Devel/17   9/17/08 6:17p rgreen
 * PR45761: provide GCP and audio clock regeneration packet to the high
 * level application; Use N, CTS value in Clock Regeneration Packet
 * structure
 * 
 * Hydra_Software_Devel/PR45761/2   9/17/08 6:15p rgreen
 * PR45761: provide GCP and audio clock regeneration packet to the high
 * level application; Use N, CTS value in Clock Regeneration Packet
 * structure
 * 
 * Hydra_Software_Devel/PR45761/1   9/17/08 5:23p honglang
 * PR45761: provide GCP and audio clock regeneration packet to the high
 * level application
 * 
 * Hydra_Software_Devel/16   8/14/08 11:19p rgreen
 * PR45106: Monitor Audio Change and Layout Updates
 * Fix GCP Deep Color debug support
 * 
 * Hydra_Software_Devel/15   7/29/08 5:26p rgreen
 * PR44697:Use PLL Lock status to determine if a vald Pixel Clock can be
 * calculated
 * 
 * Hydra_Software_Devel/14   7/10/08 11:55a rgreen
 * PR44523: Add support for Vendor Specific Infoframe
 * 
 * Hydra_Software_Devel/PR44523/2   7/9/08 11:45p rgreen
 * PR44697:Use PLL Lock status to determine if a vald Pixel Clock can be
 * calculated
 * 
 * Hydra_Software_Devel/PR44523/1   7/8/08 4:36p rgreen
 * PR44523: Add support for Vendor Specific Infoframe
 * 
 * Hydra_Software_Devel/13   6/25/08 4:24p rgreen
 * PR42977: Remove references to RcvdPicture... no longer used when
 * parsing AviInfoframe;
 * Rename GcpData to GeneralControlPacket
 * 
 * Hydra_Software_Devel/12   6/24/08 8:04p rgreen
 * PR42977: Remove unused references
 * 
 * Hydra_Software_Devel/11   6/24/08 3:57p rgreen
 * PR42977:
 * Remove unused references
 * 
 * Hydra_Software_Devel/10   6/24/08 10:45a rgreen
 * PR39463: Remove references to bhdr_hdcp.c; OTP used to load keys
 * 
 * Hydra_Software_Devel/9   6/13/08 6:31p rgreen
 * PR38947:Remove unused interrupts;
 * 
 * Hydra_Software_Devel/8   6/13/08 11:55a rgreen
 * PR38947: Adjust interrupt mappint to include entries for reserved
 * 
 * Hydra_Software_Devel/7   5/20/08 2:29p rgreen
 * PR42489:Remove multiple callback installations
 * 
 * Hydra_Software_Devel/6   5/15/08 5:47p rgreen
 * PR38947,PR39463:
 * Replace Packet Update Stopped events with new private functions
 * (BHDR_P_ProcessReceivedPackets_isr and BHDR_P_ProcessStoppedPackets)
 * Remove BAVC_HDMI_PacketnnnnRaw packets from HDR handle... raw packet
 * data now stored with  packet types BAVC_HDMI_xxxx
 * 
 * Hydra_Software_Devel/5   5/7/08 1:52p rgreen
 * PR38957,PR39463:
 * Replace BHDR_FE_GetEventHandle with Install/Uninstall Callbacks for
 * Front End Events (Hotplug and CEC)
 * Move HP Initialization for bhdr_fe file
 * Update BHDR_ConfigureAfterHotPlug to update core after HP Event
 * 
 * Hydra_Software_Devel/4   5/6/08 3:54p rgreen
 * PR38957,PR39463: Fix declaration naming conventions; remove AudioError
 * callbacks/events
 * 
 * Hydra_Software_Devel/3   5/6/08 2:02p rgreen
 * PR38947,PR39463:
 * Remove Get/SetAssigned Channel APIs;Use Attach/Detach
 * Change name from hChannel to hFeChannel
 * 
 * Hydra_Software_Devel/2   4/8/08 12:06a rgreen
 * PR38947: Add HDR 3548/56 Support
 * 
 ***************************************************************************/

#ifndef BHDR_PRIV_H__
#define BHDR_PRIV_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "bchp_sun_top_ctrl.h"
#include "bchp_common.h"


#include "bchp_hdmi_rx_0.h"
#include "bchp_hdmi_rx_intr2_0.h"
#include "bchp_int_id_hdmi_rx_intr2_0.h"

#if BHDR_HAS_MULTIPLE_PORTS 
#include "bchp_hdmi_rx_1.h"
#include "bchp_hdmi_rx_intr2_1.h"
#include "bchp_int_id_hdmi_rx_intr2_1.h"
#endif 

#include "bchp_dvp_hr.h"

#include "bavc_hdmi.h"

#include "bhdr_fe.h"
#include "bhdr_packet_priv.h"
#include "bhdr_hdcp.h"



#define BHDR_P_AUDIO_ERROR_FREE_FRAMES 20

/* number of errors in a field to consider the stream unstable */
#define BHDR_P_PACKET_ERROR_THRESHOLD 5

/* number of frames to be free of errors to consider stream OK */
#define BHDR_P_MAX_ERROR_FREE_PACKET_FRAMES 5

#define BHDR_P_MICROSECONDS_PER_SECOND 1000000

/* time units for BTMR which returns microseconds */
#define BHDR_P_MILLISECOND 1000 
#define BHDR_P_SECOND 1000000


#define BHDR_P_MAX_SYMBOL_LOSS_COUNT      (3)

#if (HDMI_RX_GEN == 7445) 
#define BHDR_P_HDMI_SYS_CLK_RATE 324000000
#elif ((HDMI_RX_GEN == 7422) || (HDMI_RX_GEN == 3548))
#define BHDR_P_HDMI_SYS_CLK_RATE 259200000
#elif (HDMI_RX_GEN == 35230)
#define BHDR_P_HDMI_SYS_CLK_RATE 256515493
#else
#error "Unknown Chip"
#endif

#define	MAKE_INTR_ENUM(IntName)	BHDR_INTR_e##IntName
#define	MAKE_INTR_NAME(IntName)	"BHDR_" #IntName

#define BHDR_PACKET_HEADER_LEN 3

#define BHDR_P_NUM_PACKETS 28
#define BHDR_P_PACKET_BYTES  28
#define BHDR_P_PACKET_WORDS 9

typedef enum
{
	BHDR_P_TIMER_eOtpCrcCalculation,
	BHDR_P_TIMER_eHdmiFeReset,
	BHDR_P_TIMER_eDviHdmiModeChange
} BHDR_P_TIMER ;


#if (HDMI_RX_GEN == 3548)
/* no translation needed */
#elif (HDMI_RX_GEN == 35230) || (HDMI_RX_GEN == 7422) || (HDMI_RX_GEN == 7445)
#define BCHP_INT_ID_HDMI_RX_INTR2_0_SET_AV_MUTE                       BCHP_INT_ID_SET_AV_MUTE
#define BCHP_INT_ID_HDMI_RX_INTR2_0_AV_MUTE_UPDATE                    BCHP_INT_ID_AV_MUTE_UPDATE               
#define BCHP_INT_ID_HDMI_RX_INTR2_0_DVI_TO_HDMI                       BCHP_INT_ID_DVI_TO_HDMI                  
#define BCHP_INT_ID_HDMI_RX_INTR2_0_HDMI_TO_DVI                       BCHP_INT_ID_HDMI_TO_DVI                   
#define BCHP_INT_ID_HDMI_RX_INTR2_0_AKSV_UPDATE                       BCHP_INT_ID_AKSV_UPDATE                  
#define BCHP_INT_ID_HDMI_RX_INTR2_0_REQUEST_KEYS                      BCHP_INT_ID_REQUEST_KEYS                 
#define BCHP_INT_ID_HDMI_RX_INTR2_0_REQUEST_KSVS                      BCHP_INT_ID_REQUEST_KSVS                 
#define BCHP_INT_ID_HDMI_RX_INTR2_0_I2C_TRANSACTION_COMPLETE          BCHP_INT_ID_I2C_TRANSACTION_COMPLETE     
#define BCHP_INT_ID_HDMI_RX_INTR2_0_PJ_UPDATE                         BCHP_INT_ID_PJ_UPDATE                    
#define BCHP_INT_ID_HDMI_RX_INTR2_0_SYMBOL_LOSS                       BCHP_INT_ID_SYMBOL_LOSS                  
#define BCHP_INT_ID_HDMI_RX_INTR2_0_INVALID_DATA_ISLAND_LENGTH        BCHP_INT_ID_INVALID_DATA_ISLAND_LENGTH   
#define BCHP_INT_ID_HDMI_RX_INTR2_0_CHANNEL_STATUS_UPDATE             BCHP_INT_ID_CHANNEL_STATUS_UPDATE        
#define BCHP_INT_ID_HDMI_RX_INTR2_0_AUDIO_VALIDITY_BIT_UPDATE         BCHP_INT_ID_AUDIO_VALIDITY_BIT_UPDATE    
#define BCHP_INT_ID_HDMI_RX_INTR2_0_RAM_PACKET_UPDATE                 BCHP_INT_ID_RAM_PACKET_UPDATE            
#define BCHP_INT_ID_HDMI_RX_INTR2_0_RAM_PACKET_STOP                   BCHP_INT_ID_RAM_PACKET_STOP              
#define BCHP_INT_ID_HDMI_RX_INTR2_0_PACKET_SYNC_ERROR                 BCHP_INT_ID_PACKET_SYNC_ERROR            
#define BCHP_INT_ID_HDMI_RX_INTR2_0_LAYOUT_UPDATE                     BCHP_INT_ID_LAYOUT_UPDATE                
#define BCHP_INT_ID_HDMI_RX_INTR2_0_AUDIO_TYPE_CHANGE                 BCHP_INT_ID_AUDIO_TYPE_CHANGE            
#define BCHP_INT_ID_HDMI_RX_INTR2_0_VSYNC_LEAD_EDGE                   BCHP_INT_ID_VSYNC_LEAD_EDGE              
#define BCHP_INT_ID_HDMI_RX_INTR2_0_VERTICAL_BLANK_END                BCHP_INT_ID_VERTICAL_BLANK_END           
#define BCHP_INT_ID_HDMI_RX_INTR2_0_EXCESSIVE_PACKET_ERRORS           BCHP_INT_ID_EXCESSIVE_PACKET_ERRORS      
#define BCHP_INT_ID_HDMI_RX_INTR2_0_FORMAT_DETECT_COUNT_SATURATED     BCHP_INT_ID_FORMAT_DETECT_COUNT_SATURATED
#define BCHP_INT_ID_HDMI_RX_INTR2_0_ERROR_INTERRUPT                   BCHP_INT_ID_ERROR_INTERRUPT              
#endif
							  
/******************************************************************************
Summary:
Enumeration of BHDR_Interrupts 
*******************************************************************************/
typedef enum
{
	/* 00 */ MAKE_INTR_ENUM(SET_AV_MUTE),
		
	/* 01 */ MAKE_INTR_ENUM(AV_MUTE_UPDATE),

	/* 02 */ MAKE_INTR_ENUM(DVI_TO_HDMI),
	/* 03 */ MAKE_INTR_ENUM(HDMI_TO_DVI),
	
	/* 04 */ MAKE_INTR_ENUM(AKSV_UPDATE),
	/* 05 */ MAKE_INTR_ENUM(REQUEST_KEYS),
	/* 06 */ MAKE_INTR_ENUM(REQUEST_KSVS),
	
	/* 07 */ MAKE_INTR_ENUM(I2C_TRANSACTION_COMPLETE),

	/* 08 */ MAKE_INTR_ENUM( PJ_UPDATE ),

	
	/* 09 */ MAKE_INTR_ENUM(SYMBOL_LOSS),
	/* 10 */ MAKE_INTR_ENUM(INVALID_DATA_ISLAND_LENGTH),
	/* 11 */ MAKE_INTR_ENUM(CHANNEL_STATUS_UPDATE),     
	/* 12 */ MAKE_INTR_ENUM(AUDIO_VALIDITY_BIT_UPDATE), 

	/* 13 */ MAKE_INTR_ENUM(RAM_PACKET_UPDATE), 
	/* 14 */ MAKE_INTR_ENUM(RAM_PACKET_STOP), 
	/* 15 */ MAKE_INTR_ENUM(PACKET_SYNC_ERROR), 
	/* 16 */ MAKE_INTR_ENUM(LAYOUT_UPDATE), 
	/* 17 */ MAKE_INTR_ENUM(AUDIO_TYPE_CHANGE), 

#if (HDMI_RX_GEN == 3548) && (BCHP_VER >= BCHP_VER_B0)
	/* 18 */ MAKE_INTR_ENUM(RGB_UNDER_RANGE),
	
#if 0
	/* 19 */ MAKE_INTR_ENUM(Reserved19),
#endif	
	/* 20 */ MAKE_INTR_ENUM(AUDIO_FIFO_OVER_FLOW),
	/* 21 */ MAKE_INTR_ENUM(AUDIO_FIFO_UNDER_FLOW),
	
#if 0
	/* 22 */ MAKE_INTR_ENUM(Reserved22),
	/* 23 */ MAKE_INTR_ENUM(Reserved23),
	/* 24 */ MAKE_INTR_ENUM(Reserved24),
	/* 25 */ MAKE_INTR_ENUM(Reserved25),
	/* 26 */ MAKE_INTR_ENUM(Reserved26),
#endif
#endif

	/* 27 */ MAKE_INTR_ENUM(VSYNC_LEAD_EDGE), 
	/* 28 */ MAKE_INTR_ENUM(VERTICAL_BLANK_END),  
	
	/* 29 */ MAKE_INTR_ENUM(EXCESSIVE_PACKET_ERRORS),
	/* 30 */ MAKE_INTR_ENUM(FORMAT_DETECT_COUNT_SATURATED),  

	/* 31 */ MAKE_INTR_ENUM(ERROR_INTERRUPT),  


	/* 32 */ MAKE_INTR_ENUM(LAST)
	
} BHDR_P_InterruptMask ;


typedef struct BHDR_P_CallbackFunc 
{
	BLST_S_ENTRY(BHDR_P_CallbackFunc) link ;
	BHDR_CallbackFunc pfCallback_isr; /* function to call when the video format changes */
	void * pvParm1; /* optional argument pointer  */
	int    iParm2 ; /* optional integer argument */
} BHDR_P_CallbackFunc;


/* Get the offset of two groups of register. */
#define BHDR_P_GET_REG_OFFSET(eHdrId, ulPriReg, ulSecReg) \
	((BHDR_P_eHdrCoreId0 == (eCoreId)) ? 0 : ((ulSecReg) - (ulPriReg)))

typedef enum BHDR_P_HdrCoreId
{
	BHDR_P_eHdrCoreId0 = 0,
	BHDR_P_eHdrCoreId1,
	BHDR_P_eHdrCoreIdMax,
	BHDR_P_eHdrCoreIdNotAttached = BHDR_P_eHdrCoreIdMax

} BHDR_P_HdrCoreId ;

#if BHDR_CONFIG_SYMBOL_LOSS_SM

typedef enum
{
/* 0 */	BHDR_P_SYMBOL_LOSS_SM_eWaitAfterHotPlug,
/* 1 */	BHDR_P_SYMBOL_LOSS_SM_eLocked,
/* 2 */	BHDR_P_SYMBOL_LOSS_SM_eClearAuth,
/* 3 */	BHDR_P_SYMBOL_LOSS_SM_eResetPll,	
/* 4 */	BHDR_P_SYMBOL_LOSS_SM_eFinal,
/* 5 */	BHDR_P_SYMBOL_LOSS_SM_eClrDCThreshold,
/* 6 */	BHDR_P_SYMBOL_LOSS_SM_eSetDCThreshold,
	BHDR_P_SYMBOL_LOSS_SM_eCount

} BHDR_P_SYMBOL_LOSS_SM ;

#endif


/*******************************************************************************
Private HDMIRx Handle Declaration 
*******************************************************************************/
BDBG_OBJECT_ID_DECLARE(BHDR_P_Handle);

typedef struct BHDR_P_Handle
{
	BDBG_OBJECT(BHDR_P_Handle)
	BHDR_P_HdrCoreId eCoreId ;
	uint32_t             ulOffset ;

	BHDR_FE_ChannelHandle hFeChannel ;
	

	BCHP_Handle   hChip ;
	BREG_Handle   hRegister ;
	BINT_Handle   hInterrupt ;
	BINT_CallbackHandle hCallback[MAKE_INTR_ENUM(LAST)] ;

	BHDR_Mode eUsage ;

	BTMR_Handle hTimerDevice ;
	BTMR_TimerHandle timerGeneric ;

#if BHDR_CONFIG_RESET_FE_ON_SYMBOL_LOSS
	BTMR_TimerHandle timerHdmiFeReset ; 
#endif


	BTMR_TimerHandle timerDviHdmiModeChange ; 


#if BHDR_CONFIG_CLEAR_AVMUTE_AFTER_N_S
	uint32_t AvMuteTimeStamp ;
#endif

	BKNI_EventHandle BHDR_Event_DviToHdmi ; 
	BKNI_EventHandle BHDR_Event_HdmiToDvi ;
	BKNI_EventHandle BHDR_Event_LoadHdcpKeys ;
	
	BKNI_EventHandle BHDR_Event_AudioChannelStatusUpdate ;
	BKNI_EventHandle BHDR_Event_VBlankEnd ;
	
	BAVC_HDMI_Packet RamPacket ;
		
	BHDR_Settings DeviceSettings ;
	
	BAVC_HDMI_AviInfoFrame AviInfoFrame ;
	BAVC_HDMI_AudioInfoFrame AudioInfoFrame ;
	BAVC_HDMI_SPDInfoFrame SPDInfoFrame;
	BAVC_HDMI_VendorSpecificInfoFrame VSInfoFrame ;
	BAVC_HDMI_ACP AudioContentProtection ;	
	BAVC_HDMI_GcpData GeneralControlPacket ;
	BAVC_HDMI_AudioClockRegenerationPacket AudioClockRegenerationPacket ;

#if BHDR_CONFIG_GAMUT_PACKET_SUPPORT
	BAVC_HDMI_GamutPacket GamutPacket ;	
	BHDR_CallbackFunc pfGamutChangeCallback ;
	void *pvGamutChangeParm1 ;
	int iGamutChangeParm2 ;
#endif

#if BHDR_CONFIG_ISRC_PACKET_SUPPORT 
	BAVC_HDMI_ISRC ISRC ;	
#endif

	BAVC_Audio_Info AudioData ;
	
	uint8_t AvMute ;
	uint8_t bHdmiMode ;

	uint32_t AudioSampleRateHz ; /* In Hz */
	
	BACM_SPDIF_ChannelStatus stChannelStatus ;
	
	uint32_t SymbolLossIntrCount ;
	uint32_t SymbolLossIntrTimeStamp ;
	
	
	/* Handle Audio */
	uint8_t uiPreviousMaiSampleRate ;
	uint8_t uiPreviousMaiAudioFormat ;
	uint8_t uiPreviousMaiSampleWidth ;
	uint8_t uiConsecutiveSampleRateCalculations ;

	/******************/
	/* HDCP variables */
	/******************/

	uint16_t 
		HDCP_TxRi, 
		HDCP_RxRi ;
		
	uint8_t
		HDCP_TxPj,
		HDCP_RxPj ;

	uint32_t  FrameCount ;
	uint8_t  
		FrameCountEncrypted,
		PreviousEncryptedFrame ;

	bool 
		bHdcpRiUpdating  ;

	BHDR_HDCP_Settings stHdcpSettings ;
	BHDR_HDCP_Status stHdcpStatus ;

	BHDR_CallbackFunc pfHdcpStatusChangeCallback ;
	void *pvHdcpStatusChangeParm1  ;
	int iHdcpStatusChangeParm2  ;
	
#if BHDR_CONFIG_HDCP_KEY_OTP_ROM
	BTMR_TimerHandle timerOtpCrcCalc ;
#endif


#if BHDR_CONFIG_DEBUG_TIMER_S
	uint32_t DebugTimeStamp ;
	uint32_t ulVoCooef_C01_C00 ;
	uint8_t ulAvMute ;
	uint8_t ulUseRgbToRcbCr ;

#endif
    
	uint8_t HdcpKeyLoadOffset ;
	
	BHDR_CallbackFunc pfAvMuteNotifyCallback ;
	void *pvAvMuteNotifyParm1 ;
	int iAvMuteNotifyParm2 ;	
	
	BHDR_CallbackFunc pfVideoFormatChangeCallback ;
	void *pvVideoFormatChangeParm1 ;
	int iVideoFormatChangeParm2 ;
	
	BHDR_CallbackFunc pfAudioFormatChangeCallback ;
	void *pvAudioFormatChangeParm1 ;
	int iAudioFormatChangeParm2 ;
	
	BHDR_CallbackFunc pfAudioContentProtectionChangeCallback ;
	void *pvAudioContentProtectionChangeParm1 ;
	int iAudioContentProtectionChangeParm2 ;
	
	BHDR_CallbackFunc pfISRCChangeCallback ;
	void *pvISRCChangeParm1 ;
	int iISRCChangeParm2 ;
	
	BHDR_CallbackFunc pfPacketChangeCallback ;
	void *pvPacketChangeParm1 ;
	int iPacketChangeParm2 ;
	
	BHDR_CallbackFunc pfPacketErrorCallback ;
	void *pvPacketErrorParm1 ;
	int iPacketErrorParm2 ;


	bool bPacketErrors ;
	uint8_t ErrorFreePacketFrames ;
	
	bool bProcessPackets ;
	uint32_t VSyncCounter;
	uint8_t PllLock ;
	uint16_t HdmiAudioPackets ;

	uint32_t uiChannelStatus30 ;
	uint32_t uiChannelStatus74 ;
	
	bool bDeepColorMode ;
	uint32_t uiColorDepthFrameCounter ;
	BAVC_HDMI_VideoFormat stHdmiVideoFormat ;

#if BHDR_CONFIG_SYMBOL_LOSS_SM
	
	BHDR_P_SYMBOL_LOSS_SM eSymbolLossState ;
	BHDR_P_SYMBOL_LOSS_SM ePreviousSymbolLossState ;

	bool bClockStopped ;

	uint32_t AuthReqCnt ;
	bool bFirstPj ;
	uint8_t R0Equal0Count ;

	uint8_t RequestKeysIntrCount ;
	uint32_t RequestKeysTimeStamp ;	

	uint32_t ClockStoppedTimer	;
#endif
	bool bPreviousStatus ;

	BHDR_Handle hHDRSlave ;

#if BHDR_CONFIG_DVP_HR_TMR
	bool bWaitForTimer ;
#endif	

#if BHDR_CONFIG_UPDATE_MAI_ON_VSYNC
	uint8_t uiChannelStatusSampleFreq ;
#endif

} BHDR_P_Handle ;






/******************************************************************************
Summary:
Handle interrupts from the HDMIRx core.

Description:
Interrupts received from the HDMIRx core must be handled.  The following 
is a list of possible interrupts.


Input:
	pParameter - pointer to interrupt specific information BHDR_Open.

Output:
	<None>
	
Returns:
	<None>

See Also:

*******************************************************************************/
void BHDR_P_HandleInterrupt_isr
(
	void *pParam1,						/* Device channel handle */
	int parm2							/* not used */
) ;
								

void BHDR_P_HotPlugConnect_isr(BHDR_Handle hHDR) ;
void BHDR_P_HotPlugRemove_isr(BHDR_Handle hHDR) ;

BERR_Code BHDR_P_ConfigureAudioMaiBus_isr(BHDR_Handle hHDR);

#if BHDR_CONFIG_HDCP_REPEATER
BERR_Code  BHDR_HDCP_P_EnableRepeater(BHDR_Handle hHDR)  ;

BERR_Code BHDR_ReadRxI2cRegisterSpace(BHDR_Handle hHDR, 
	uint8_t offset, uint8_t *pData, uint8_t Length) ;

#endif

void BHDR_P_EnableInterrupts_isr(BHDR_Handle hHDR, bool enable) ;

void BHDR_P_ResetHdcp_isr(BHDR_Handle hHDR) ;

#if BHDR_CONFIG_HDCP_KEY_OTP_ROM
void BHDR_HDCP_P_VerifyOtpCalculation_isr(BHDR_Handle hHDR)  ;
#endif

#ifdef __cplusplus
}
#endif
 
#endif
/* end bhdr_priv.h */
