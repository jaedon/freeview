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
 * $brcm_Workfile: nexus_hdmi_input.c $
 * $brcm_Revision: 131 $
 * $brcm_Date: 10/12/12 3:15p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/hdmi_input/src/nexus_hdmi_input.c $
 * 
 * 131   10/12/12 3:15p vsilyaev
 * SW7435-329: Converted NEXUS_VideoInput to NEXUS_VideoInputHandle
 * 
 * 130   10/12/12 3:12p erickson
 * SW7435-419: change proc to BDBG_LOG to that they are still available
 *  with B_REFSW_DEBUG_LEVEL=err
 * 
 * 129   9/27/12 1:13p erickson
 * SW7435-362: lock module during init and uninit
 * 
 * 128   6/19/12 12:42p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 127   5/31/12 7:21p rgreen
 * SW7425-3152: Reduce console debug messages
 * 
 * 125   4/18/12 1:24p rgreen
 * SW7425-1988: Schedule hdcpRxChanged task during isr time rather than
 *  setting/waiting for an event during task time
 * 
 * 124   3/23/12 3:19p rgreen
 * SW7425-2515: Remove CEC code.  Replace code  that toggle hotplug with
 *  actual NEXUS_HdmiInput_ToggleHotPlug API;
 * 
 * 123   11/30/11 4:50p rgreen
 * SW3548-3121: Report device Connection status; even if input is invalid
 * 
 * 122   11/14/11 2:30p erickson
 * SW7425-1448: set timebase framerate using atomic function, not
 *  Get/SetSettings
 * 
 * 121   10/20/11 4:03p rgreen
 * SW7425-1386:  Fix coverity deadcode  warning
 * 
 * 120   10/4/11 2:50p rgreen
 * SW7425-1386:  Fix coverity deadcode  warning
 * 
 * 119   8/26/11 3:04p rgreen
 * SW7425-1200,SWDTV-7644:  Add dynamic power management support; Remove
 *  depracated power management 1.0 scheme
 * 
 * 118   8/18/11 10:00a rgreen
 * SW7425-991,SWDTV-8288: Add hdmi hardware passthroug support; place phy
 *  specific code into private files
 * 
 * 117   8/5/11 9:11a gmohile
 * SW7425-692 : Fix compile error for non-power management build
 * 
 * 116   8/3/11 3:58p rgreen
 * SW7425-692:  Enable standby only if NEXUS_POWER_MANAGEMENT is defined
 * 
 * 115   8/3/11 3:50p rgreen
 * SWDTV-7092: Reduce debug message from BDBG_WRN to BDBG__MSG
 * 
 * 114   8/3/11 3:28p gmohile
 * SW7425-692 : Add hdmi input standby
 * 
 * 113   7/13/11 4:09p rgreen
 * SWDTV-6898: Update HdmiInput_Close to close/destroy all nexus handles
 *  after magnum HDMI Rx is closed
 * 
 * 112   7/11/11 8:14p rgreen
 * SWDTV-6898: Obtain hdmiInput default settings from HDR PI (phy and
 *  frontend);  Organize Nexus_HdmiInput_P_OpenHdmiRx
 * 
 * 111   7/6/11 1:16p rgreen
 * SWDTV-6771: Merge changes
 * 
 * SWDTV-6771/1   6/22/11 2:44p rgreen
 * SWDTV-6771: Add support for HDMI ISRC packets
 * 
 * 110   6/29/11 3:19p rgreen
 * SWDTV-6898,SW7425-136: Updage uninstall info for HDMI Rx PacketChange
 *  Callback
 * 
 * 109   4/20/11 6:14p jtna
 * SW7425-365: fix build warnings for 2.6.37 kernel
 * 
 * 108   4/18/11 3:45p rgreen
 * SW7422-186: Update hdcpRxChange callback name
 * 
 * SW7422-186/6   4/18/11 1:28p rgreen
 * SW7422-186: Remove depracated keySetStatusUpdate; replaced by
 *  hdcpRxChanged callback
 * 
 * SW7422-186/5   4/13/11 1:55p rgreen
 * SW7422-186: Replace RequestKsvFifoEvent with generic hdcpEvent
 * 
 * SW7422-186/4   4/11/11 2:55p rgreen
 * SW7422-186: Add NEXUS_HdmiInput_ToggleHotPlug function to support/force
 *  upstream HDCP reauthentication
 * 
 * SW7422-186/3   4/7/11 2:38p rgreen
 * SW7422-186:  Add HDCP Repeater Support
 * 
 * SW7422-186/2   3/31/11 2:38p rgreen
 * SW7422-186: Add HDCP Repeater support
 * 
 * SW7422-186/1   3/28/11 5:38p rgreen
 * SW7422-186: Add HDCP Repeater Support
 * 
 * 105   3/14/11 2:46p rgreen
 * SW7425-136: Support NULL settings for NEXUS_HdmiInput_Open
 * 
 * 104   3/7/11 5:20p rgreen
 * SWDTV-5815: Merge changes from 35230 branch
 * 
 * 103   3/3/11 1:25p rgreen
 * SWDTV-4664: Update HDCP Key Set verification and HDCP processing for
 *  OTP ROM based chips
 * 
 * 102   2/16/11 7:06p rgreen
 * SW7422-129: Merge changes
 * 
 * SW7422-129/2   2/10/11 3:48p rgreen
 * SW7422-129: Remove hdcpSettings from hdmiInputSettings; create separate
 *  hdcpSettings;  Remove hdcp Key processing from module open.  Hdcp Key
 *  processing must be done after open
 * 
 * SW7422-129/1   2/2/11 6:26p rgreen
 * SW7422-129: Add HDCP Key Loading support for OTP RAM
 * 
 * 101   12/2/10 11:19a hongtaoz
 * SW7425-21,SW7422-69: fixed compile error;
 * 
 * 100   10/29/10 6:46p rgreen
 * SW7425-21,SW7422-69:Temporarily disable CEC for 742x platforms
 * 
 * 99   10/19/10 6:40p rgreen
 * SW7422-69,SW7425-21: Fix compilation for 742x.  Remove CEC support
 * 
 * 98   8/30/10 10:36a rgreen
 * SW3548-2013: Treat VideoID 1 (VGA) as IT format when selecting
 *  colorimetry
 * 
 * 97   8/27/10 10:33a erickson
 * SWDEPRECATED-3811: add simpler g_NEXUS_hdmiInput.initInProgress instead
 *  of port[]
 *
 * 96   8/26/10 3:25p erickson
 * SWDEPRECATED-3811: clear g_NEXUS_hdmiInput.handle[] on close. add more
 *  NEXUS_HdmiInputModule_Print info.
 *
 * 95   8/26/10 2:22p erickson
 * SWDEPRECATED-3811: add NEXUS_HdmiInputModule_Print
 *
 * 94   7/8/10 5:03p rgreen
 * SW3548-2795: Update detection/configuration of 3D
 *
 * 93   6/16/10 1:32p rgreen
 * SW3548-2952: reset HDMI Format to NONE (2D) if VSI packets stop
 *
 * 92   5/12/10 4:28p rgreen
 * SW3548-2795,SW3548-2642:Merge Changes
 *
 * SW3548-2642/7   5/7/10 11:17a rgreen
 * SW3548-2642: Update 3D Master/Slave configuration
 *
 * SW3548-2642/6   5/3/10 11:49a rgreen
 * SW3548-2795: Add NEXUS_HdmiInput_SetMaster function to configure the
 *  master/slave core; Remove masterHdmiInput from hdmiInputSettings
 *
 * SW3548-2642/5   4/29/10 4:50p rgreen
 * SW3548-2795: Apply same picture settings to slaved hdmiInput for 3D
 *  mode
 *
 * SW3548-2642/4   4/27/10 10:14p rgreen
 * SW3548-2795: Update to handle dynamic configuration of hdmiInput ports
 *  for 3D
 *
 * SW3548-2642/3   4/20/10 4:11p rgreen
 * SW3548-2795: dynamically re-configure hdmiInput cores and front ends as
 *  needed for 3D configurations
 *
 * SW3548-2642/2   3/19/10 3:02p rgreen
 * SW3548-2843, SW3548-2795: Add support for 3D panel settings
 *
 * SW3548-2642/1   3/8/10 5:26p rgreen
 * SW3548-2642: For 3D mode, update slaveHdmiInput to use same settiings
 *  as master; Update debug messages to indicate which  input
 *
 * 91   2/19/10 2:45p rgreen
 * SW3548-2642: For 3D mode, allow the configuration of both cores to one
 *  frontend
  * BCM35230/20   3/3/11 7:33p rgreen
 * SWDTV-4664: Update HDCP Key Set verification and HDCP processing for
 *  OTP ROM based chips
 * 
 * BCM35230/19   2/24/11 5:30p rgreen
 * SW7422-129,SW35230-2374: Fix compilation for 35230; Update HDCP Key
 *  Processing/Status; distinguish between OTP RAM and OTP ROM
 * 
 * BCM35230/18   1/19/11 1:23p rgreen
 * SW35125-12: Fix compilation for 35125
 * 
 * BCM35230/17   1/4/11 1:45p rgreen
 * SW35230-2374: Update  NEXUS_HdmiInputHdcpKeySetOtpState_e declaration
 *  names
 * 
 * BCM35230/SW35230-2374/2   12/17/10 5:54p rgreen
 * SW35230-2374: Create separate file for HDCP related functions
 * 
 * BCM35230/SW35230-2374/1   12/3/10 9:09p rgreen
 * SW35230-2374: Add support for OTP HDCP Key verfication
 * 
 * BCM35230/15   12/1/10 5:24p jhaberf
 * SW35125-1: Added missing semicolon
 * 
 * BCM35230/14   11/8/10 2:29p rgreen
 * SW35230-2116:  Force a failure/stop if application has not properly
 *  closed audio prior to closing hdmiInput
 * 
 * BCM35230/13   11/5/10 10:20a rgreen
 * SW35230-2116: On NEXUS_HdmiInput_Close, do not return if AudioInput
 *  associated with hdmiInput has not been shutdown; continue freeing
 *  allocated resources
 * 
 * BCM35230/12   10/26/10 1:58p rgreen
 * SW35230-706: Remove extra comments from failed checkin
 * 
 * BCM35230/11   10/26/10 1:55p rgreen
 * SW35230-706: Set Transfer Characteristics when setting colorspace
 *  MatrixCoefficients
 * 
 * BCM35230/10   8/30/10 10:12a rgreen
 * SW3548-2013: Treat VideoID 1 (VGA) as IT format when selecting
 *  colorimetry
 * 
 * BCM35230/9   7/14/10 4:00p rgreen
 * SW35230-558:  Enable HDMI based on the port being selected
 *  (videoConnected), not on whether a device is attached or not.   Get
 *  HDMI Rx Status after port has been selected (videoConnected)
 * 
 * BCM35230/8   7/13/10 9:02p rgreen
 * SW35230-517: Update naming convention
 * 
 * BCM35230/7   7/13/10 6:28p rgreen
 * SW35230-517:merge changes
 * 
 * BCM35230/SW35230-517/4   7/13/10 7:22p adtran
 * SW35230-517: Add code review feedback.  Create and use
 *  HotPlugEnable and HotPlugDisable
 * 
 * BCM35230/SW35230-517/3   7/5/10 5:10p adtran
 * SW35230-517: Fixed deadlock between NEXUS_HdmiInput_SetHotPlug and
 *  rxStatus.DeviceAttached
 * 
 * BCM35230/SW35230-517/2   7/2/10 6:28p rgreen
 * SW35230-517: Remove commented out code;
 * 
 * BCM35230/SW35230-517/1   6/30/10 11:20a adtran
 * SW35230-517: add support for hotplug and edid controls
 * 
 * BCM35230/6   5/14/10 2:38p rgreen
 * SW35230-15: Merge support for 35230; Remove CEC support Remove 3D
 *  master/slave references
 * 
 * 92   5/12/10 4:28p rgreen
 * SW3548-2795,SW3548-2642:Merge Changes
 * 
 * SW3548-2642/7   5/7/10 11:17a rgreen
 * SW3548-2642: Update 3D Master/Slave configuration
 * 
 * SW3548-2642/6   5/3/10 11:49a rgreen
 * SW3548-2795: Add NEXUS_HdmiInput_SetMaster function to configure the
 *  master/slave core; Remove masterHdmiInput from hdmiInputSettings
 *
 * SW3548-2642/5   4/29/10 4:50p rgreen
 * SW3548-2795: Apply same picture settings to slaved hdmiInput for 3D
 *  mode
 *
 * SW3548-2642/4   4/27/10 10:14p rgreen
 * SW3548-2795: Update to handle dynamic configuration of hdmiInput ports
 *  for 3D
 *
 * SW3548-2642/3   4/20/10 4:11p rgreen
 * SW3548-2795: dynamically re-configure hdmiInput cores and front ends as
 *  needed for 3D configurations
 *
 * SW3548-2642/2   3/19/10 3:02p rgreen
 * SW3548-2843, SW3548-2795: Add support for 3D panel settings
 *
 * SW3548-2642/1   3/8/10 5:26p rgreen
 * SW3548-2642: For 3D mode, update slaveHdmiInput to use same settiings
 *  as master; Update debug messages to indicate which  input
 *
 * 91   2/19/10 2:45p rgreen
 * SW3548-2642: For 3D mode, allow the configuration of both cores to one
 *  frontend
 *
 * 90   2/3/10 3:03p erickson
 * SW3548-2727: merge 3DTV support
 *
 * BCM3D/4   1/21/10 11:27a erickson
 * SW3548-2727: move HD-DVI settings from NEXUS_HdmiInputModuleSettings to
 *  NEXUS_VideoHdmiInputSettings. this allows us to change settings at
 *  runtime per HD-DVI input.
 *
 * 89   2/1/10 3:29p rgreen
 * SW3556-1025: Copy bHpdDisconnected  setting from open settings
 * Power down frontend, before closing/freeing frontend channel
 *
 * 88   1/29/10 7:10p rgreen
 * SW3556-1022: Remove debug statement
 *
 * 87   1/29/10 6:55p rgreen
 * SW3556-1022,SW3548-2745: Update nexus shutdown order; remove unused
 *  event and handler
 *
 * 86   1/26/10 5:51p rgreen
 * SW3556-1025: Reorder nexus hdmiInput shutdown to prevent using a
 *  previously closed hdr handle
 *
 * 85   1/25/10 4:29p erickson
 * SW3556-1021: must cancel timer in NEXUS_HdmiInput_Close to prevent
 *  NEXUS_HdmiInput_P_ReleaseHotPlug from being called after close. also
 *  added some missing asserts.
 *
 * 84   1/21/10 11:39a erickson
 * SW3548-2727: merge non-3D changes to mainline now
 *
 * 83   1/19/10 6:26p rgreen
 * SW3548-2013: Demote warning messages for colorspace  debugging to
 *  regular messages
 *
 * 82   12/31/09 12:50p rgreen
 * SW3548-2013: Always update 'HDCP Ri Updating' status regardless of
 *  bhdr_config setting
 *
 * 81   12/11/09 8:19p rgreen
 * SW3548-2665: Fix AttachHdmiRx core to be called only when port is
 *  opened for use; not at module initialization
 *
 * 80   12/11/09 6:31p rgreen
 * SW3548-2665: Fix order of freed pointers in hdmiInput close.  Do not
 *  enable interrupts when initializing the module.
 *
 * 77   10/30/09 5:50p rgreen
 * SW3548-2013: Reduce/eliminate un-needed debug messages; Report muted
 *  video status only when AvMute is still asserted
 *
 * 76   10/16/09 5:16p rgreen
 * SW3548-2013: Update debug messages to indicate AvMute Status; report
 *  only changes in hdmiInput GetStatus
 *
 * 75   10/15/09 2:55p erickson
 * SW3548-2498: Fix compilation issue when NEXUS_POWER_MANAGEMENT is
 *  undefined
 *
 * 74   10/8/09 4:45p rgreen
 * SW3548-2498: Fix compilation issue when NEXUS_POWER_MANAGEMENT is
 *  undefined
 *
 * 73   10/7/09 2:04p erickson
 * SW3548-2534: fix warnings
 *
 * 72   10/2/09 1:46p erickson
 * SW3548-2498: added memset(0) to NEXUS_HdmiInputModule_Uninit so that
 *  full init happens next time around
 *
 * 71   10/1/09 5:12p rgreen
 * SW3548-2498: Power down all HDMI Ports at module initialization time;
 *  remove use of powerDownUnusedPort setting.  Subsequent
 *  Nexus_HdmiInput_Open calls will enable ports.
 *
 * 70   8/31/09 2:50p rgreen
 * SW3548-2013: Update debug messages to report changes in status rather
 *  than continuous debug messages.  Add debug message for Input Status
 *  (attached, valid, signal,pll, sybmolLoss,and mode)
 *
 * 69   8/19/09 11:27a rgreen
 * PR57561: Change message to warning to always advise of
 *  ConfigureAfterHotPlugStatus
 *
 * 68   8/17/09 2:25p rgreen
 * PR57565: Return NEXUS_SUCCESS on NEXUS_HdmiInput_GetStatus call.
 *  Caller to use vdcStatus.validHdmiStatus to check validadity of
 *  returned data
 *
 * 67   8/13/09 1:26p rgreen
 * PR57565:Return NEXUS_UNKNOWN error if BHDR_GetHdmiRxStatus returns an
 *  error
 *
 * 66   7/9/09 1:38p rgreen
 * PR56697: Update MaxtrixCoefficient setting based on Colorspace
 *  converter
 *
 * 65   6/18/09 1:02p rgreen
 * PR56097:  Add flag to indicate validity of channel status information
 *  returned by BHDR_GetHdmiRxStatus()
 *
 * 64   6/10/09 11:27a rgreen
 * PR55586,PR52879: Restore Hot Plug toggle when hdminput device is
 *  selected (videoConnected)
 *
 * 63   6/8/09 7:24p rgreen
 * PR55586,PR52879: Disable Hot Plug toggle when hdminput device is
 *  selected (videoConnected); Originally added to EDID RAM support
 * Call ConfigureAfterHotPlug regardless of whether or not  the port is
 *  active
 *
 * 62   6/5/09 6:56p rgreen
 * PR55586: Add debug message to indicate
 *  Nexus_HdmiInput_ConfigureAfterHotPlug
 * has been called when using an external switch
 *
 * 61   5/27/09 4:33p rgreen
 * PR51331: Add support to to explicitly power down hdmi frontend/core
 *  when not used and/or Power Management disabled
 *
 * 60   5/4/09 5:17p rgreen
 * PR54810: Remove references to unused Gamut Meta Data Packet
 *
 * 59   4/9/09 4:01p rgreen
 * PR52879: Add support for  on-chip EDID RAM
 *
 * 58   4/8/09 9:46a rgreen
 * PR43960: Fix warning. Default aspect ratio to 16:9 when defaulting to
 *  DVI mode
 *
 * 57   4/7/09 7:58p rgreen
 * PR53960: Update HDDVI Picture structure with Pixel Repition and
 *  DVI/HDMI mode info
 *
 * 56   3/26/09 4:31p rgreen
 * PR52524: Update CSC Selection when new CSC is to be loaded.
 * Fix nexus naming convention for eCscMode to cscMode
 *
 * 55   3/26/09 10:55a rgreen
 * PR52524: set index to indicate what CSC table to load based on
 *  AviInfoframe
 *
 * PR52879/1   3/23/09 2:00p rgreen
 * PR52879: Add EDID RAM Support
 *
 * 54   3/9/09 2:21p rgreen
 * PR52524: Pass RgbQuantization info from AviInfoFrame to hddvi for
 *  proper colorimetry loading
 *
 * 53   3/6/09 11:08a rgreen
 * PR51307: Add HDMI HDCP Repeater Support
 *
 * 52   3/4/09 1:03p jgarrett
 * PR 52618: Adding forcePcFormat option to HdmiInput
 *
 * 51   2/19/09 1:46p rgreen
 * PR51752: add validHdmiStatus to NEXUS_HdmiInputStatus structure to
 *  indicate validity of returned structure
 *
 * 50   2/2/09 4:47p rgreen
 * PR51798: use combination of timer and AvMute signal to unMute hddvi
 *  picture callback
 *
 * 49   1/26/09 5:31p rgreen
 * PR51601: Add Hot Plug isr callback
 *
 * 48   1/26/09 12:06p erickson
 * PR51468: global variable naming convention
 *
 * 47   1/26/09 11:30a erickson
 * PR51468: global variable naming convention
 *
 * 46   1/21/09 8:04p rgreen
 * PR51418: set mute setting in picture callback to value specified in
 *  AvMute callback.
 *
 * 45   1/5/09 2:28p rgreen
 * PR50686: Open HDMI RX Front End prior to open HDMI RX
 *
 * PR47447/1   1/5/09 2:27p rgreen
 * PR50686: Open HDMI RX Front End prior to open HDMI RX
 *
 * 44   12/16/08 3:02p rgreen
 * PR49943: Add support to enable/disable I2c pull up at initialization
 *
 * 43   12/10/08 4:18p vsilyaev
 * PR 48908: Promoted nexus_power_management.h to the public API
 *
 * 42   12/5/08 2:46p rgreen
 * PR49943: Add support to enable/disable I2c pull up at initialization
 *
 * PR49943/1   12/5/08 1:46p rgreen
 * PR49943: Add support to enable/disable I2c pull up at initialization
 *
 * 41   12/4/08 10:34a rgreen
 * PR49897: nexus hdmi input close should power down HDMI module
 *
 * PR49897/2   12/4/08 1:39p honglang
 * PR49897: nexus hdmi input close should power down HDMI module
 *
 * PR49897/1   12/3/08 5:43p honglang
 * PR49897: nexus hdmi input close should power down HDMI module
 *
 * 39   12/1/08 11:03a erickson
 * PR49226: update comments on HdmiInputStatus
 *
 * PR49226/1   11/27/08 6:14p honglang
 * PR49226, PR49228: add comment for HDMI input status
 *
 * 38   11/26/08 2:32p erickson
 * PR49228: merge
 *
 * PR49228/1   11/25/08 6:47p honglang
 * PR49226, PR49228: add the real format information to HDMI input status
 *
 * 37   11/13/08 3:36p rgreen
 * PR48741: Add setting for BHDR_FE_Open to indicate use of external
 *  switch
 *
 * 36   11/7/08 11:23a rgreen
 * PR46176: for dynamic power management; power down HDMI core after open
 *
 * 35   11/5/08 12:20p rgreen
 * PR48692,PR46176,PR48585 :return NEXUS_UNKNOWN if hdmiInput is not
 *  active
 *
 * 34   11/4/08 3:41p rgreen
 * PR46176:Remove 2nd CreateCallback for CEC
 *
 * 33   11/3/08 3:53p rgreen
 * PR48594,PR48621: Adjust HdmiInput Close sequence for power management
 *
 * 32   10/31/08 2:57p erickson
 * PR48594: allow NULL for default settings in NEXUS_HdmiInput_Open
 *
 * 31   10/31/08 12:05a erickson
 * PR40799: #include fix
 *
 * 30   10/30/08 8:38p rgreen
 * PR40799: Enable dynamic power mangement for HDMI Rx
 *
 * 29   10/30/08 1:21p erickson
 * PR40799: change #define to NEXUS_POWER_MANAGEMENT
 *
 * 28   10/28/08 2:56p erickson
 * PR46437: merge
 *
 * PR46437/1   10/28/08 6:01p honglang
 * PR46437: support power standby passive in kylin
 *
 * 27   10/25/08 2:36p rgreen
 * PR46176: Add Create/Destroy CEC in the hdmiInput object; install/set
 *  CEC  handler
 *
 * 26   10/17/08 10:00a rgreen
 * PR47970: Enable Equalization by default
 *
 * 25   10/16/08 9:08a erickson
 * PR43214: fix warning
 *
 * 24   10/14/08 3:05p rgreen
 * PR43214:Fix compilation warning when enabling debug eye diagram
 *
 * 23   10/9/08 4:40p rgreen
 * PR43214: Merge changes to support debug eye diagram
 *
 * 22   10/9/08 11:22a rgreen
 * PR46176: Update power settings only when changed
 *
 * 21   10/8/08 2:07a rgreen
 * PR46176: Implement Power Management; move frontend inside of the
 *  hdmiInput object
 *
 * 20   10/6/08 12:08a erickson
 * PR46176: added HDR dynamic power management
 *
 * 19   9/22/08 2:53p rgreen
 * PR45761: PR47081: Add option to HDMI Rx Frontend Open Channel to
 *  enable/disable the equalizer.  Equalizer is enabled by default
 * PR45761: provide GCP and audio clock regeneration packet to the high
 *  level application
 *
 * PR45761/2   9/22/08 2:48p rgreen
 * PR47081: Add option to HDMI Rx Frontend Open Channel to enable/disable
 *  the equalizer.  Equalizer is enabled by default
 *
 * PR45761/1   9/17/08 5:26p honglang
 * PR45761: provide GCP and audio clock regeneration packet to the high
 *  level application
 *
 * 18   9/22/08 10:20a erickson
 * PR47180: change default
 *
 * 17   9/16/08 9:10a erickson
 * PR46801: report HDMI receiver's color space to NEXUS_HdmiInputStatus.
 *  also, rename internal variable to reduce confusion.
 *
 * 16   9/11/08 4:12p rgreen
 * PR45358,PR44257: Remove BHDR_CEC_SUPPORT defines/macros.  CEC enabled
 *  by default
 *
 * 15   9/11/08 2:26p rgreen
 * PR44257: Install CEC handler when compilation flag is set
 *
 * 14   9/5/08 1:59p erickson
 * PR44257: HDMI CEC updates
 *
 * 13   9/4/08 6:32p rgreen
 * PR44257: Remove private declaration of NEXUS_HdmiInputFrontend
 * Add Nexus_HmdiInputFrontend_Get/SetCecSettings
 * fix parameter for BHDR_FE_InstallCecCallback to use hdmiInputFrontend
 *
 * PR43214/2   10/7/08 1:49p ditian
 * PR43214: Need to HDMI status screens to support debugging, changing
 *  parameter
 *
 * PR43214/1   9/4/08 4:29p ditian
 * PR43214: add eye diagram in status menu for HDMI input
 *
 * 12   8/7/08 8:29a erickson
 * PR45358: intial checkin
 *
 * 11   7/30/08 3:51p rgreen
 * PR44523: Add support for Vendor Specific Infoframe
 *
 * PR44523/1   7/8/08 4:37p rgreen
 * PR44523: Add support for Vendor Specific Infoframe
 *
 * 10   6/26/08 3:04p rgreen
 * PR43666:Rename variables as channel vs fe for actual channel variables;
 *  modify debug messages
 *
 * 9   6/18/08 1:30p erickson
 * PR43730: fix warning
 *
 * 8   6/13/08 3:39p erickson
 * PR39453: fix VideoFormatChange callback
 *
 * 7   6/13/08 9:54a erickson
 * PR39453: allow NULL NEXUS_HdmiInputFrontendSettings
 *
 * 6   5/19/08 1:57p erickson
 * PR39453: add more events
 *
 * 5   5/19/08 1:38p erickson
 * PR39453: HdmiInput updates
 *
 * 4   5/16/08 4:38p erickson
 * PR39453: update
 *
 * 3   5/12/08 11:07a erickson
 * PR39453: HdmiInputFrontend impl
 *
 * 2   5/12/08 10:10a erickson
 * PR39453: fix 3548 compile
 *
 * 1   5/6/08 4:45p erickson
 * PR39453: add separate impl for 3548/3556
 *
 **************************************************************************/
#include "nexus_hdmi_input_module.h"
#include "priv/nexus_hdmi_input_priv.h"
#include "priv/nexus_hdmi_input_standby_priv.h"
#include "nexus_hdmi_input_hdcp_priv.h"

#include "nexus_power_management.h"

BDBG_MODULE(nexus_hdmi_input);

#if !NEXUS_NUM_HDMI_INPUTS
#error Platform must define NEXUS_NUM_HDMI_INPUTS
#endif

NEXUS_ModuleHandle g_NEXUS_hdmiInputModule;
NEXUS_HdmiInputModuleSettings g_NEXUS_hdmiInputModuleSettings;

/* HdmiInput global state */
static struct {
    NEXUS_HdmiInputModuleSettings settings;
    BHDR_FE_Handle fe;
    bool initInProgress;
    NEXUS_HdmiInputHandle handle[NEXUS_NUM_HDMI_INPUTS] ;
    NEXUS_HdmiVendorSpecificInfoFrame_HDMIVideoFormat hdmiVideoFormat ;
} g_NEXUS_hdmiInput;


static void NEXUS_HdmiInput_P_VideoFormatChange_isr(void *context, int param2, void *data);
static void NEXUS_HdmiInput_P_PacketChange_isr(void *context, int param2, void *data);

static void NEXUS_HdmiInput_P_AvMuteNotify_isr(void *context, int param2, void *data);
static void NEXUS_HdmiInput_P_SetFrameRate(void *data);

static void NEXUS_HdmiInput_P_HotPlug_isr(void *context, int param, void *data);
static void NEXUS_HdmiInput_P_SetPower( NEXUS_HdmiInputHandle hdmiInput, bool callingHdr );
static void NEXUS_HdmiInput_P_ReleaseHotPlug(void *context) ;



/****************************************
* Module functions
***************/

void NEXUS_HdmiInputModule_Print(void)
{
    unsigned i;
    BDBG_LOG(("HdmiInputModule:"));
    for (i=0;i<NEXUS_NUM_HDMI_INPUTS;i++) {
        NEXUS_HdmiInputHandle hdmiInput = g_NEXUS_hdmiInput.handle[i];
        if (!hdmiInput) continue;
        BDBG_LOG(("hdmi_input %d: handle=%p, videoInput=%p, audioInput=%p",
            i, hdmiInput, &hdmiInput->videoInput, &hdmiInput->audioInput));
        BDBG_LOG(("    videoConnected=%d, %dx%d%c, noSignal=%d, originalFormat=%d, master=%p",
            hdmiInput->videoConnected, hdmiInput->vdcStatus.avWidth, hdmiInput->vdcStatus.avHeight, hdmiInput->vdcStatus.interlaced?'i':'p',
            hdmiInput->vdcStatus.noSignal, hdmiInput->vdcStatus.originalFormat, hdmiInput->masterHdmiInput));
    }
}

void NEXUS_HdmiInputModule_GetDefaultSettings(NEXUS_HdmiInputModuleSettings *pSettings)
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
}

NEXUS_ModuleHandle NEXUS_HdmiInputModule_Init(const NEXUS_HdmiInputModuleSettings *pSettings)
{
    NEXUS_ModuleSettings moduleSettings;
    NEXUS_HdmiInputModuleSettings hdmiInputModuleSettings;
    NEXUS_Error errCode ;

    BERR_Code rc;
    BHDR_FE_Settings feSettings;

    uint8_t index ;
    NEXUS_HdmiInputHandle hdmiInput;
    NEXUS_HdmiInputSettings hdmiInputSettings ;

    BDBG_ASSERT(NULL == g_NEXUS_hdmiInputModule);
	
#if NEXUS_HAS_SECURITY
#if BHDR_CONFIG_HDCP_KEY_OTP_RAM && BHSM_SECURE_PI_SUPPORT_KEYLADDER
    if (!pSettings || !pSettings->modules.security) 
    {
        BDBG_WRN(("security module handle required"));
        BDBG_ASSERT(false) ;
        return NULL;
    }
#endif	
#else
    if (!pSettings) {
        NEXUS_HdmiInputModule_GetDefaultSettings(&g_NEXUS_hdmiInputModuleSettings);
        pSettings = &g_NEXUS_hdmiInputModuleSettings;
    }
#endif


    if (g_NEXUS_hdmiInputModule) {
        BDBG_ERR(("Module already initialized"));
        return NULL;
    }

    if (!pSettings) {
        NEXUS_HdmiInputModule_GetDefaultSettings(&hdmiInputModuleSettings);
        pSettings = &hdmiInputModuleSettings;
    }

    /* init global module handle */
    NEXUS_Module_GetDefaultSettings(&moduleSettings);
    moduleSettings.priority = NEXUS_ModulePriority_eLow;
    moduleSettings.dbgPrint = NEXUS_HdmiInputModule_Print;
    moduleSettings.dbgModules = "nexus_hdmi_input_module";
    g_NEXUS_hdmiInputModule = NEXUS_Module_Create("hdmi_input", &moduleSettings);
    if ( NULL == g_NEXUS_hdmiInputModule )
    {
        errCode = BERR_TRACE(BERR_OS_ERROR);
        return NULL;
    }
    NEXUS_LockModule();

    g_NEXUS_hdmiInputModuleSettings = *pSettings;
    
    rc = BHDR_FE_GetDefaultSettings(g_pCoreHandles->chp, &feSettings);
    if (rc) {rc = BERR_TRACE(rc); goto error;}


    rc = BHDR_FE_Open(&g_NEXUS_hdmiInput.fe, g_pCoreHandles->chp, g_pCoreHandles->reg, g_pCoreHandles->bint, &feSettings);
    if (rc) {rc = BERR_TRACE(rc); goto error;}

    g_NEXUS_hdmiInput.initInProgress = true;
    for (index = 0 ; index < NEXUS_NUM_HDMI_INPUTS; index++)
    {
        NEXUS_HdmiInput_GetDefaultSettings(&hdmiInputSettings) ;
        hdmiInput = NEXUS_HdmiInput_Open(index, &hdmiInputSettings) ;

        if (hdmiInput) {
            NEXUS_HdmiInput_Close(hdmiInput) ;
        }
    }

    g_NEXUS_hdmiInput.initInProgress = false;    

    g_NEXUS_hdmiInput.settings = *pSettings;
    
    BDBG_MSG(("hdmiInput Module Init Complete...")) ;

    NEXUS_UnlockModule();
    return g_NEXUS_hdmiInputModule;

error:
    NEXUS_UnlockModule();
    NEXUS_Module_Destroy(g_NEXUS_hdmiInputModule);
    g_NEXUS_hdmiInputModule = NULL;
    return NULL;
}

void NEXUS_HdmiInputModule_Uninit()
{
    NEXUS_LockModule();
    BHDR_FE_Close(g_NEXUS_hdmiInput.fe);
    NEXUS_UnlockModule();
    NEXUS_Module_Destroy(g_NEXUS_hdmiInputModule);
    g_NEXUS_hdmiInputModule = NULL;
    BKNI_Memset(&g_NEXUS_hdmiInput, 0, sizeof(g_NEXUS_hdmiInput));
}

NEXUS_Error NEXUS_HdmiInputModule_Standby_priv(bool enabled, const NEXUS_StandbySettings *pSettings)
{
#if NEXUS_POWER_MANAGEMENT
    BHDR_StandbySettings standbySettings;   
    BERR_Code  rc;

    BSTD_UNUSED(pSettings);
    
    if(!g_NEXUS_hdmiInput.fe) {return NEXUS_SUCCESS;}
    
    if(enabled) {
	BHDR_GetDefaultStandbySettings(&standbySettings);
	rc = BHDR_Standby(g_NEXUS_hdmiInput.fe, &standbySettings);	    
    } else {
	rc = BHDR_Resume(g_NEXUS_hdmiInput.fe);
    }
    if (rc) return BERR_TRACE(rc);
       
#else
    BSTD_UNUSED(enabled);
    BSTD_UNUSED(pSettings);    
#endif

    return NEXUS_SUCCESS ;
}
    

/****************************************
* API functions
***************/

void NEXUS_HdmiInput_GetDefaultSettings(NEXUS_HdmiInputSettings *pHdmiInputDefaultSettings)
{
    BHDR_FE_ChannelSettings defaultHdmiFeChannelSettings ;
    BHDR_Settings defaultHdmiRxSettings ;

    BKNI_Memset(pHdmiInputDefaultSettings, 0, sizeof(*pHdmiInputDefaultSettings));

    pHdmiInputDefaultSettings->timebase = NEXUS_Timebase_e0;
    pHdmiInputDefaultSettings->autoColorSpace = true;
    pHdmiInputDefaultSettings->colorSpace = NEXUS_ColorSpace_eRgb;

    /* get default Front End Channel Settings */
    BHDR_FE_GetDefaultChannelSettings(g_NEXUS_hdmiInput.fe, &defaultHdmiFeChannelSettings) ;
    pHdmiInputDefaultSettings->frontend.equalizationEnabled = defaultHdmiFeChannelSettings.bEnableEqualizer ;
    pHdmiInputDefaultSettings->frontend.externalSwitch = defaultHdmiFeChannelSettings.bExternalSwitch ;
    pHdmiInputDefaultSettings->frontend.hpdDisconnected = defaultHdmiFeChannelSettings.bHpdDisconnected ;
    
    /* get default HDMI Rx Phy Settings */
    BHDR_GetDefaultSettings(&defaultHdmiRxSettings) ;
    pHdmiInputDefaultSettings->hdr.parseAviInfoframe = 
	defaultHdmiRxSettings.bParseAVI ;
    
    pHdmiInputDefaultSettings->hdr.disableI2cSclPullUp = 
	defaultHdmiRxSettings.bDisableI2cPadSclPullup ;
    
    pHdmiInputDefaultSettings->hdr.disableI2cSdaPullUp = 
	defaultHdmiRxSettings.bDisableI2cPadSdaPullup ;
    
    pHdmiInputDefaultSettings->hdr.enableHdmiHardwarePassthrough = 
	defaultHdmiRxSettings.bHdmiHardwarePassthrough ;

    return;
}


NEXUS_Error NEXUS_HdmiInput_P_OpenHdmiFe(NEXUS_HdmiInputHandle hdmiInput, const NEXUS_HdmiInputSettings *pHdmiInputSettings)
{
    BERR_Code rc;
    BHDR_FE_ChannelSettings frontendSettings;

    rc = BHDR_FE_GetDefaultChannelSettings(g_NEXUS_hdmiInput.fe, &frontendSettings);
    if (rc) {rc = BERR_TRACE(rc); goto error;}

    /* override frontend default settings w/ user specified settings */
    if (pHdmiInputSettings)
    {
        frontendSettings.bEnableEqualizer = pHdmiInputSettings->frontend.equalizationEnabled;
        frontendSettings.bExternalSwitch  = pHdmiInputSettings->frontend.externalSwitch;
        frontendSettings.bHpdDisconnected = pHdmiInputSettings->frontend.hpdDisconnected ;
    }
    frontendSettings.uiChannel = hdmiInput->index;

    rc = BHDR_FE_OpenChannel(g_NEXUS_hdmiInput .fe, &hdmiInput->frontend, &frontendSettings);
    if (rc) {rc = BERR_TRACE(rc); goto error;}


    hdmiInput->hotPlugCallback = NEXUS_IsrCallback_Create(hdmiInput->frontend, NULL);

    
    rc = BHDR_FE_InstallHotPlugCallback(hdmiInput->frontend,
        NEXUS_HdmiInput_P_HotPlug_isr, hdmiInput, 0);
    if (rc) {rc = BERR_TRACE(rc); goto error;}
    
#if (HDMI_RX_GEN == 35230)
    NEXUS_HdmiInputTvm_P_HotPlugEnable();
 #endif
    
    return NEXUS_SUCCESS ;


error:
    BDBG_ERR(("Error Opening HDMI_RX Frontend")) ;
    return rc ;

}


NEXUS_Error  NEXUS_HdmiInput_P_OpenHdmiRx(NEXUS_HdmiInputHandle  hdmiInput, 
	const NEXUS_HdmiInputSettings *pHdmiInputSettings)
{
    NEXUS_HdmiInputSettings defaultHdmiInputSettings;
    BHDR_Settings hdrSettings;
    BERR_Code rc;
  
    hdmiInput->avMuteCallback        = NEXUS_IsrCallback_Create(hdmiInput, NULL);
    hdmiInput->aviInfoFrameChanged   = NEXUS_IsrCallback_Create(hdmiInput, NULL);
    hdmiInput->audioInfoFrameChanged = NEXUS_IsrCallback_Create(hdmiInput, NULL);
    hdmiInput->spdInfoFrameChanged   = NEXUS_IsrCallback_Create(hdmiInput, NULL);
    hdmiInput->vendorSpecificInfoFrameChanged = NEXUS_IsrCallback_Create(hdmiInput, NULL);
    hdmiInput->audioContentProtectionChanged  = NEXUS_IsrCallback_Create(hdmiInput, NULL);
    hdmiInput->hdcpRxChanged = NEXUS_IsrCallback_Create(hdmiInput, NULL);

    hdmiInput->sourceChangedCallback = NEXUS_TaskCallback_Create(hdmiInput, NULL);

    hdmiInput->bSentResetHdDviBegin = false;
    hdmiInput->stFieldData.eMatrixCoefficients = BAVC_MatrixCoefficients_eItu_R_BT_709;
    hdmiInput->stFieldData.eTransferCharacteristics = BAVC_TransferCharacteristics_eItu_R_BT_709 ; 
    hdmiInput->stFieldData.eColorSpace = BAVC_Colorspace_eRGB;
    hdmiInput->stFieldData.eAspectRatio = BFMT_AspectRatio_eUnknown;
    hdmiInput->manualColorSpace = BAVC_Colorspace_eRGB;
    hdmiInput->reportedColorSpace = BAVC_Colorspace_eRGB;

    BHDR_GetDefaultSettings(&hdrSettings);

    if (!pHdmiInputSettings)
    {
        NEXUS_HdmiInput_GetDefaultSettings(&defaultHdmiInputSettings);
        pHdmiInputSettings = &defaultHdmiInputSettings;
    }
    else
    {
        /* overwrite magnum defaults with selected settings from app */
        hdrSettings.bDisableI2cPadSclPullup = pHdmiInputSettings->hdr.disableI2cSclPullUp ;
        hdrSettings.bDisableI2cPadSdaPullup = pHdmiInputSettings->hdr.disableI2cSdaPullUp ;
        hdrSettings.bParseAVI = pHdmiInputSettings->hdr.parseAviInfoframe ;
        hdrSettings.bHdmiHardwarePassthrough = pHdmiInputSettings->hdr.enableHdmiHardwarePassthrough ;
    }
    hdrSettings.eCoreId = hdmiInput->index;
    hdrSettings.hTmr = g_pCoreHandles->tmr;


    rc = BHDR_Open(&hdmiInput->hdr, g_pCoreHandles->chp,
        g_pCoreHandles->reg, g_pCoreHandles->bint, &hdrSettings) ;
    if (rc) {BERR_TRACE(rc); goto error;}


    rc = BHDR_InstallVideoFormatChangeCallback(hdmiInput->hdr,
        NEXUS_HdmiInput_P_VideoFormatChange_isr, hdmiInput, 0);
    if (rc) {BERR_TRACE(rc); goto error;}

    rc = BHDR_InstallAvMuteNotifyCallback(hdmiInput->hdr,
        NEXUS_HdmiInput_P_AvMuteNotify_isr, hdmiInput, 0);
    if (rc) {BERR_TRACE(rc); goto error;}

    rc = BHDR_InstallPacketChangeCallback(hdmiInput->hdr,
        NEXUS_HdmiInput_P_PacketChange_isr, hdmiInput, 0);
    if (rc) {BERR_TRACE(rc); goto error;}

    BKNI_CreateEvent(&hdmiInput->frameRateEvent);
    hdmiInput->frameRateHandler = NEXUS_RegisterEvent(hdmiInput->frameRateEvent,
        NEXUS_HdmiInput_P_SetFrameRate, hdmiInput);

    NEXUS_VIDEO_INPUT_INIT(&hdmiInput->videoInput, NEXUS_VideoInputType_eHdmi, hdmiInput);
#if NEXUS_HAS_AUDIO
    NEXUS_AUDIO_INPUT_INIT(&hdmiInput->audioInput, NEXUS_AudioInputType_eHdmi, hdmiInput);
#endif

    /* do not attach the core (i.e. enable interrupts) at module init ; wait until core is opened for use   */
    if (!g_NEXUS_hdmiInput.initInProgress)
    {
        rc = BHDR_FE_AttachHdmiRxCore(hdmiInput->frontend, hdmiInput->hdr) ;
        if (rc) {rc = BERR_TRACE(rc); goto error;}
    }


    NEXUS_HdmiInput_SetSettings(hdmiInput, pHdmiInputSettings);

     return NEXUS_SUCCESS ;

error:
    BDBG_WRN(("Error Opening HdmiRx Phy")) ;
    return rc ;
}


void NEXUS_HdmiInput_ToggleHotPlug(NEXUS_HdmiInputHandle hdmiInput)
{
    NEXUS_HdmiInput_SetHotPlug(hdmiInput, true) ;
    if (!hdmiInput->releaseHotPlugTimer)
    {
        hdmiInput->releaseHotPlugTimer =
            NEXUS_ScheduleTimer(150, NEXUS_HdmiInput_P_ReleaseHotPlug, hdmiInput) ;
    }
}


NEXUS_HdmiInputHandle NEXUS_HdmiInput_Open(unsigned index, const NEXUS_HdmiInputSettings *pSettings)
{
    NEXUS_HdmiInputHandle hdmiInput;
    BERR_Code rc;
    NEXUS_HdmiInputSettings defaultSettings;


    if (index >= NEXUS_NUM_HDMI_INPUTS) {
        BDBG_ERR(("invalid HdmiInput[%d]", index));
        return NULL;
    }

    if (!pSettings) {
        NEXUS_HdmiInput_GetDefaultSettings(&defaultSettings);
        pSettings = &defaultSettings;
    }

    hdmiInput = BKNI_Malloc(sizeof(*hdmiInput));
    if (!hdmiInput) {
        BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        return NULL;
    }
    NEXUS_OBJECT_INIT(NEXUS_HdmiInput, hdmiInput);

    hdmiInput->index = index ;

    /* Open HDMI Rx Frontend first */    
    rc = NEXUS_HdmiInput_P_OpenHdmiFe(hdmiInput, pSettings) ;
    if (rc) {rc = BERR_TRACE(rc); goto error;}


    /* Open HDMI Rx core second */	
    rc = NEXUS_HdmiInput_P_OpenHdmiRx(hdmiInput, pSettings) ;

    if (!pSettings->useInternalEdid)  /* toggle HP now if using separate PROM - DEFAULT */
    {
        if (!g_NEXUS_hdmiInput.initInProgress) /* only toggle hot plug if module has been initialized and device is connected */
        {
            BHDR_Status rxStatus ;

            rc = BHDR_GetHdmiRxStatus(hdmiInput->hdr, &rxStatus);
            if (rc) {rc = BERR_TRACE(rc); }

            if (rxStatus.DeviceAttached)
                NEXUS_HdmiInput_ToggleHotPlug(hdmiInput) ;		   	
            else
            {
                BKNI_EnterCriticalSection() ;
                BHDR_ConfigureAfterHotPlug_isr(hdmiInput->hdr, false) ;
                BKNI_LeaveCriticalSection() ;
            }
        }
    }

	
    g_NEXUS_hdmiInput.handle[index] = hdmiInput ;   
    NEXUS_OBJECT_REGISTER(NEXUS_VideoInput, &hdmiInput->videoInput, Open);

    return hdmiInput;
error:
    NEXUS_HdmiInput_Close(hdmiInput);
    return NULL;
}


NEXUS_HdmiInputHandle NEXUS_HdmiInput_OpenWithEdid(
    unsigned index,
    const NEXUS_HdmiInputSettings *pHdmiInputSettings,
    const uint8_t *edidData,
    uint16_t edidDataSize )
{
    NEXUS_Error errCode ;
    NEXUS_HdmiInputHandle hdmiInput ;

    hdmiInput = NEXUS_HdmiInput_Open(index, pHdmiInputSettings) ;
    if (!hdmiInput)
        return NULL ;


    BDBG_MSG(("Using BCM%d on-chip EDID RAM...", BCHP_CHIP)) ;

    if (edidDataSize % 128)
    {
        BDBG_ERR(("EDID Block(s) size is not a multiple of 128"));
        BERR_TRACE(BERR_INVALID_PARAMETER);
        goto error ;
    }

    errCode  = NEXUS_HdmiInput_LoadEdidData(hdmiInput, edidData, edidDataSize) ;
    if (errCode) {errCode = BERR_TRACE(errCode); goto error;}

    /* always toggle the HPD line when an EDID is loaded */
    NEXUS_HdmiInput_ToggleHotPlug(hdmiInput) ;

    return hdmiInput ;

error:
    NEXUS_HdmiInput_Close(hdmiInput);
    return NULL;
}

static void NEXUS_HdmiInput_P_Finalizer(NEXUS_HdmiInputHandle hdmiInput)
{
    NEXUS_OBJECT_ASSERT(NEXUS_HdmiInput, hdmiInput);
    if (hdmiInput->releaseHotPlugTimer) {
        NEXUS_CancelTimer(hdmiInput->releaseHotPlugTimer);
        hdmiInput->releaseHotPlugTimer = NULL;
    }

#if NEXUS_HAS_AUDIO
    if ( NULL != hdmiInput->audioInput.pMixerData )
    {
        NEXUS_Error errCode;
        BDBG_ERR(("Audio connector is still active.  Please call NEXUS_AudioInput_Shutdown()."));
        errCode = BERR_TRACE(BERR_INVALID_PARAMETER);
        return;
    }
#endif

    /* now uninstall the HDMI Rx Frontend call backs */
    if (hdmiInput->frontend) {
        BHDR_FE_UnInstallHotPlugCallback(hdmiInput->frontend, NEXUS_HdmiInput_P_HotPlug_isr);
    }


    /* now, uninstall the HDMI Rx Core callbacks ; close the core */
    if (hdmiInput->hdr)
    {
        BHDR_UnInstallVideoFormatChangeCallback(
            hdmiInput->hdr, NEXUS_HdmiInput_P_VideoFormatChange_isr) ;

        BHDR_UnInstallAvMuteNotifyCallback(hdmiInput->hdr,
            NEXUS_HdmiInput_P_AvMuteNotify_isr);

        BHDR_UnInstallPacketChangeCallback(hdmiInput->hdr,
            NEXUS_HdmiInput_P_PacketChange_isr) ;

#if BHDR_CONFIG_HDCP_KEY_OTP_ROM
        BHDR_HDCP_UnInstallHdcpStatusChangeCallback(
            hdmiInput->hdr, NEXUS_HdmiInput_P_HdcpStateChange_isr) ;
#endif
        BHDR_Close(hdmiInput->hdr);
    }

    /* close the frontend channel */
    if (hdmiInput->frontend)
    {
        BHDR_FE_CloseChannel(hdmiInput->frontend);
        hdmiInput->frontend = NULL;
    }

#if (HDMI_RX_GEN == 35230)
    NEXUS_HdmiInputTvm_P_HotPlugDisable();
#endif

    /* now clean up the nexus callbacks */
    if (hdmiInput->frameRateHandler) {
        NEXUS_UnregisterEvent(hdmiInput->frameRateHandler);
    }
    BKNI_DestroyEvent(hdmiInput->frameRateEvent);

    NEXUS_IsrCallback_Destroy(hdmiInput->aviInfoFrameChanged);
    NEXUS_IsrCallback_Destroy(hdmiInput->audioInfoFrameChanged);
    NEXUS_IsrCallback_Destroy(hdmiInput->spdInfoFrameChanged);
    NEXUS_IsrCallback_Destroy(hdmiInput->vendorSpecificInfoFrameChanged);
    NEXUS_IsrCallback_Destroy(hdmiInput->audioContentProtectionChanged);
    NEXUS_IsrCallback_Destroy(hdmiInput->avMuteCallback);
    NEXUS_IsrCallback_Destroy(hdmiInput->hdcpRxChanged);
	
    NEXUS_TaskCallback_Destroy(hdmiInput->sourceChangedCallback) ;

    if (hdmiInput->hotPlugCallback) {
        NEXUS_IsrCallback_Destroy(hdmiInput->hotPlugCallback);
    }

    NEXUS_HdmiInput_UnloadEdidData(hdmiInput) ;

    g_NEXUS_hdmiInput.handle[hdmiInput->index] = NULL;

    BDBG_OBJECT_DESTROY(hdmiInput, NEXUS_HdmiInput);
    BKNI_Free(hdmiInput);
}

static void NEXUS_HdmiInput_P_Release(NEXUS_HdmiInputHandle hdmiInput)
{
    NEXUS_OBJECT_ASSERT(NEXUS_HdmiInput, hdmiInput);
    NEXUS_OBJECT_UNREGISTER(NEXUS_VideoInput, &hdmiInput->videoInput, Close);
}

NEXUS_OBJECT_CLASS_MAKE_WITH_RELEASE(NEXUS_HdmiInput, NEXUS_HdmiInput_Close);

void NEXUS_HdmiInput_GetSettings(NEXUS_HdmiInputHandle hdmiInput, NEXUS_HdmiInputSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(hdmiInput, NEXUS_HdmiInput);
    *pSettings = hdmiInput->settings;
}

NEXUS_Error NEXUS_HdmiInput_SetSettings(NEXUS_HdmiInputHandle hdmiInput, const NEXUS_HdmiInputSettings *pSettings)
{
    BAVC_Colorspace manualColorSpace;
    BHDR_Settings hdrSettings ;
	
    BDBG_OBJECT_ASSERT(hdmiInput, NEXUS_HdmiInput);
    NEXUS_IsrCallback_Set(hdmiInput->avMuteCallback, &pSettings->avMuteChanged);
    NEXUS_TaskCallback_Set(hdmiInput->sourceChangedCallback, &pSettings->sourceChanged);
    NEXUS_IsrCallback_Set(hdmiInput->aviInfoFrameChanged, &pSettings->aviInfoFrameChanged);
    NEXUS_IsrCallback_Set(hdmiInput->audioInfoFrameChanged, &pSettings->audioInfoFrameChanged);
    NEXUS_IsrCallback_Set(hdmiInput->spdInfoFrameChanged, &pSettings->spdInfoFrameChanged);
    NEXUS_IsrCallback_Set(hdmiInput->vendorSpecificInfoFrameChanged, &pSettings->vendorSpecificInfoFrameChanged);
    NEXUS_IsrCallback_Set(hdmiInput->audioContentProtectionChanged, &pSettings->audioContentProtectionChanged);
    NEXUS_IsrCallback_Set(hdmiInput->hotPlugCallback, &pSettings->frontend.hotPlugCallback) ;

    NEXUS_IsrCallback_Set(hdmiInput->hotPlugCallback, &pSettings->frontend.hotPlugCallback) ;

    manualColorSpace = NEXUS_P_ColorSpace_ToMagnum(pSettings->colorSpace);

    BKNI_EnterCriticalSection();
        if ( hdmiInput->settings.forcePcFormat != pSettings->forcePcFormat )
        {
            if ( hdmiInput->pPcFormatCallback_isr )
            {
                BDBG_MSG(("Force PC format option has changed.  Notifying display."));
                hdmiInput->pPcFormatCallback_isr(hdmiInput->pPcFormatCallbackParam);
            }
        }
        hdmiInput->settings = *pSettings;
        hdmiInput->manualColorSpace = manualColorSpace;
    
    BKNI_LeaveCriticalSection();

    BHDR_GetSettings(hdmiInput->hdr, &hdrSettings) ;

        hdrSettings.bParseAVI = pSettings->hdr.parseAviInfoframe  ;       
        hdrSettings.bDisableI2cPadSclPullup = pSettings->hdr.disableI2cSclPullUp ;
        hdrSettings.bDisableI2cPadSdaPullup = pSettings->hdr.disableI2cSdaPullUp  ;
        hdrSettings.bHdmiHardwarePassthrough = pSettings->hdr.enableHdmiHardwarePassthrough ;

    BHDR_SetSettings(hdmiInput->hdr, &hdrSettings ) ;

    return NEXUS_SUCCESS;
}

NEXUS_VideoInput NEXUS_HdmiInput_GetVideoConnector(NEXUS_HdmiInputHandle hdmiInput)
{
    BDBG_OBJECT_ASSERT(hdmiInput, NEXUS_HdmiInput);
    return &hdmiInput->videoInput;
}

NEXUS_AudioInput NEXUS_HdmiInput_GetAudioConnector(NEXUS_HdmiInputHandle hdmiInput)
{
    BDBG_OBJECT_ASSERT(hdmiInput, NEXUS_HdmiInput);
#if NEXUS_HAS_AUDIO
    return &hdmiInput->audioInput;
#else
    return NULL;
#endif
}

/**
Apply master's stFieldData to its slave.
This function is called by both masters, slaves, and non-masters.
So we must first determine if this is a master. Only if we are, then set the slave.
**/
static void NEXUS_HdmiInput_P_SetSlaveFieldData_isr(NEXUS_HdmiInputHandle hdmiInput)
{
#if NEXUS_NUM_HDMI_INPUTS > 1
    if (hdmiInput->masterHdmiInput == NULL) {
        /* we might be a master */
        NEXUS_HdmiInputHandle slaveHdmiInput = g_NEXUS_hdmiInput.handle[1 - hdmiInput->index];
        if (slaveHdmiInput && slaveHdmiInput->masterHdmiInput == hdmiInput) {
            /* is a master, so memcpy */
            BKNI_Memcpy(&slaveHdmiInput->stFieldData, &hdmiInput->stFieldData, sizeof(BAVC_VDC_HdDvi_Picture)) ;
        }
    }
#else
    BSTD_UNUSED(hdmiInput) ;
#endif
}

void NEXUS_HdmiInput_PictureCallback_isr(NEXUS_HdmiInputHandle hdmiInput, BAVC_VDC_HdDvi_Picture **ppPicture)
{
    BDBG_OBJECT_ASSERT(hdmiInput, NEXUS_HdmiInput);

    if (hdmiInput->uiFormatChangeMuteCount)
    {

        /* only report messages if AvMute is still set */
        if (hdmiInput->avMute)
        {
            BDBG_WRN(("Mute video for %d more frames after format change; AvMute: %d",
	            hdmiInput->uiFormatChangeMuteCount, hdmiInput->avMute)) ;

            if (hdmiInput->uiFormatChangeMuteCount == 1)
            {
                BDBG_WRN(("Ready to un-mute video after format change")) ;
            }
        }
        hdmiInput->uiFormatChangeMuteCount-- ;
    }


    hdmiInput->stFieldData.bMute =
        hdmiInput->avMute || (hdmiInput->uiFormatChangeMuteCount);


    if (hdmiInput->bSentResetHdDviBegin)
    {
        hdmiInput->stFieldData.bMute            =  hdmiInput->uiFormatChangeMuteCount ;
        hdmiInput->stFieldData.bResetHdDviBegin = false;
        hdmiInput->bSentResetHdDviBegin         = false;
    }

    /* bResetHdDviBegin requested */
    if (hdmiInput->stFieldData.bResetHdDviBegin)
    {
        hdmiInput->bSentResetHdDviBegin = true;
    }

    NEXUS_HdmiInput_P_SetSlaveFieldData_isr(hdmiInput);

    /* Return the current state we have. */
    *ppPicture = &hdmiInput->stFieldData;
}


static void NEXUS_HdmiInput_P_AvMuteNotify_isr(void *context, int param2, void *data)
{
    NEXUS_HdmiInputHandle hdmiInput = (NEXUS_HdmiInputHandle)context;
    BSTD_UNUSED(param2);

    BDBG_OBJECT_ASSERT(hdmiInput, NEXUS_HdmiInput);
    hdmiInput->avMute = *((bool *) data) ? true : false;
    BDBG_MSG(("hdmiInput%d AvMuteNotify_isr %d",
        hdmiInput->index, hdmiInput->avMute)) ;

    if (hdmiInput->avMute)
        hdmiInput->stFieldData.bMute = true;
    else
        hdmiInput->stFieldData.bMute = hdmiInput->uiFormatChangeMuteCount ;

    hdmiInput->stFieldData.bResetHdDviBegin = true;

    NEXUS_HdmiInput_P_SetSlaveFieldData_isr(hdmiInput);

    NEXUS_IsrCallback_Fire_isr(hdmiInput->avMuteCallback);
}

static void NEXUS_HdmiInput_P_VideoFormatChange_isr(void *context, int param2, void *data)
{
    NEXUS_HdmiInputHandle hdmiInput = (NEXUS_HdmiInputHandle)context;
    BAVC_HDMI_AviInfoFrame *pAviInfoFrame = (BAVC_HDMI_AviInfoFrame*)data;
    BAVC_Colorspace colorSpace;
    BAVC_CscMode     cscMode ;

    BAVC_HDMI_AviInfoFrame_VideoFormat eAviInfoFrameFormat ;
    BFMT_AspectRatio aspectRatio;
    bool limitedRange ;

    BSTD_UNUSED(param2);
    BDBG_OBJECT_ASSERT(hdmiInput, NEXUS_HdmiInput);
    BDBG_CASSERT(BAVC_Colorspace_eYCbCr444 == (BAVC_Colorspace)BAVC_HDMI_AviInfoFrame_Colorspace_eYCbCr444);

    BDBG_MSG(("hdmiInput%d VideoFormatChange_isr", hdmiInput->index));

    hdmiInput->stFieldData.bHdmiMode = true ;

     /* set the default Colorspace Converter   */
    cscMode = BAVC_CscMode_e709RgbFullRange ;


    if (pAviInfoFrame->ePacketStatus == BAVC_HDMI_PacketStatus_eStopped)
    {
        /* override to DVI RGB mode */
        hdmiInput->stFieldData.bHdmiMode = false ;
        colorSpace = BAVC_Colorspace_eRGB ;

        /* assume aspect ratio 16:9 */
        aspectRatio = BFMT_AspectRatio_e16_9;

        goto NEXUS_HdmiInput_P_VideoFormatChange_isr_SETUP ;
    }


    hdmiInput->reportedColorSpace = pAviInfoFrame->ePixelEncoding;

    switch (pAviInfoFrame->ePictureAspectRatio) {
    case BAVC_HDMI_AviInfoFrame_PictureAspectRatio_eNoData:
        aspectRatio = BFMT_AspectRatio_eUnknown;
        break;
    case BAVC_HDMI_AviInfoFrame_PictureAspectRatio_e4_3:
        aspectRatio = BFMT_AspectRatio_e4_3;
        break;
    default:
    case BAVC_HDMI_AviInfoFrame_PictureAspectRatio_e16_9:
        aspectRatio = BFMT_AspectRatio_e16_9;
        break;
    }

    /* set the pixel repitition value */
    hdmiInput->stFieldData.ulPixelRepitionFactor = pAviInfoFrame->PixelRepeat ;

    if (!hdmiInput->settings.autoColorSpace)
    {
        colorSpace = hdmiInput->manualColorSpace; /* overwrite color space */
     goto NEXUS_HdmiInput_P_VideoFormatChange_isr_SETUP ;
    }

    /* use the reported Color Space specified in the Avi Infoframe */
    colorSpace = hdmiInput->reportedColorSpace;

    /* set the color matrix */
    if ((pAviInfoFrame->VideoIdCode == 0 )   /* Unknown, usually VESA format */
    || (pAviInfoFrame->VideoIdCode == 1 )) /* VGA */
        /* IT FORMAT */
        eAviInfoFrameFormat = BAVC_HDMI_AviInfoFrame_VideoFormat_eIT ;

    else if (((pAviInfoFrame->VideoIdCode >=  2) && (pAviInfoFrame->VideoIdCode <=  3))
    || ((pAviInfoFrame->VideoIdCode >=  6) && (pAviInfoFrame->VideoIdCode <= 15))
    || ((pAviInfoFrame->VideoIdCode >= 17) && (pAviInfoFrame->VideoIdCode <= 18))
    || ((pAviInfoFrame->VideoIdCode >= 21) && (pAviInfoFrame->VideoIdCode <= 30))
    || ((pAviInfoFrame->VideoIdCode >= 35) && (pAviInfoFrame->VideoIdCode <= 38))
    || ((pAviInfoFrame->VideoIdCode >= 42) && (pAviInfoFrame->VideoIdCode <= 45))
    || ((pAviInfoFrame->VideoIdCode >= 48) && (pAviInfoFrame->VideoIdCode <= 50)))
            /* SD FORMAT */
            eAviInfoFrameFormat = BAVC_HDMI_AviInfoFrame_VideoFormat_eSD ;

    else if (((pAviInfoFrame->VideoIdCode >=  4) && (pAviInfoFrame->VideoIdCode <=  5))
    || ((pAviInfoFrame->VideoIdCode ==  16))
    || ((pAviInfoFrame->VideoIdCode >=  19) && (pAviInfoFrame->VideoIdCode <= 20))
    || ((pAviInfoFrame->VideoIdCode >=  31) && (pAviInfoFrame->VideoIdCode <= 34))
    || ((pAviInfoFrame->VideoIdCode >=  39) && (pAviInfoFrame->VideoIdCode <= 41))
    || ((pAviInfoFrame->VideoIdCode >=  46) && (pAviInfoFrame->VideoIdCode <= 47)))
            /* HD FORMAT */
            eAviInfoFrameFormat = BAVC_HDMI_AviInfoFrame_VideoFormat_eHD ;

    else
    {
        /* New/Unknown Format */
        BDBG_WRN(("Unknown/Unsupported VIDEO ID Code %d; Assuming HD format",
            pAviInfoFrame->VideoIdCode)) ;
            eAviInfoFrameFormat = BAVC_HDMI_AviInfoFrame_VideoFormat_eHD ;
    }


    /* YCbCr */
    if ((pAviInfoFrame->ePixelEncoding == BAVC_HDMI_AviInfoFrame_Colorspace_eYCbCr422)
    || (pAviInfoFrame->ePixelEncoding == BAVC_HDMI_AviInfoFrame_Colorspace_eYCbCr444))
    {
        /* Unspecified colorimetry, but SD Format */
        if (((pAviInfoFrame->eColorimetry == BAVC_HDMI_AviInfoFrame_Colorimetry_eNoData)
        && (eAviInfoFrameFormat == BAVC_HDMI_AviInfoFrame_VideoFormat_eSD))

        /* OR Colorimetry is specified as BT.601 */
        || (pAviInfoFrame->eColorimetry == BAVC_HDMI_AviInfoFrame_Colorimetry_eSmpte170)

        /* OR Colorimetry is specified as  xvYCC601 */
        || ((pAviInfoFrame->eColorimetry == BAVC_HDMI_AviInfoFrame_Colorimetry_eExtended)
        && (pAviInfoFrame->eExtendedColorimetry == BAVC_HDMI_AviInfoFrame_ExtendedColorimetry_exvYCC601)))
        {
            /* Set the BT.601 YCbCr flag */
         cscMode = BAVC_CscMode_e601YCbCr ;
        }
        else
        {
            /* Set the BT.709 YCbCr flag */
            cscMode = BAVC_CscMode_e709YCbCr ;
          }
    }
    /****** RGB ******/
    else if (pAviInfoFrame->ePixelEncoding == BAVC_HDMI_AviInfoFrame_Colorspace_eRGB)
    {
        /* determine whether color range limited vs full */

        /* if xv.Color (always scaled as limited range) */
        /* if ((C=3) Or (Q=1) Or  ((Q=0) And (FormatIsSD Or FormatIsHD)))        */

        if ((pAviInfoFrame->eColorimetry == BAVC_HDMI_AviInfoFrame_Colorimetry_eExtended)

        /* OR limited range is expressly specified */
        ||  (pAviInfoFrame->eRGBQuantizationRange == BAVC_HDMI_AviInfoFrame_RGBQuantizationRange_eLimitedRange)

        /* OR video format is used to select */
        ||  ((pAviInfoFrame->eRGBQuantizationRange == BAVC_HDMI_AviInfoFrame_RGBQuantizationRange_eDefault)
        &&    ((eAviInfoFrameFormat == BAVC_HDMI_AviInfoFrame_VideoFormat_eSD)
        || (eAviInfoFrameFormat == BAVC_HDMI_AviInfoFrame_VideoFormat_eHD))))
        {
            limitedRange = true ;
#if 0
           if (rgbQuantization == BAVC_HDMI_AviInfoFrame_RGBQuantizationRange_eDefault)
               limitedRange = false ;
#endif
        }
        else
        {
            limitedRange = false ;
        }


        /* NOW determine which colorimetry */

        /* Unspecified Colorimetry, but SD Format */
        if (((pAviInfoFrame->eColorimetry == BAVC_HDMI_AviInfoFrame_Colorimetry_eNoData)
        && (eAviInfoFrameFormat == BAVC_HDMI_AviInfoFrame_VideoFormat_eSD))

        /* OR Colorimetry is specified as BT.601 */
        || (pAviInfoFrame->eColorimetry == BAVC_HDMI_AviInfoFrame_Colorimetry_eSmpte170)

        /* OR Colorimetry is specified as  xvYCC601 */
        || ((pAviInfoFrame->eColorimetry == BAVC_HDMI_AviInfoFrame_Colorimetry_eExtended)
        && (pAviInfoFrame->eExtendedColorimetry == BAVC_HDMI_AviInfoFrame_ExtendedColorimetry_exvYCC601)))
        {
            if (limitedRange)
                /****** BT.601 LIMITED ******/
                cscMode = BAVC_CscMode_e601RgbLimitedRange ;
            else

                /****** BT.601 FULL  ******/
                cscMode = BAVC_CscMode_e601RgbFullRange ;
        }
        else
        {
            if (limitedRange)
                /****** BT.709 LIMITED ******/
                cscMode = BAVC_CscMode_e709RgbLimitedRange ;
            else
                /****** BT.709 FULL  ******/
                cscMode = BAVC_CscMode_e709RgbFullRange ;
            }
        }

NEXUS_HdmiInput_P_VideoFormatChange_isr_SETUP:

   /* set the Matrix based on the CSC */

    switch(cscMode)
    {
    case BAVC_CscMode_e709RgbFullRange :
    case BAVC_CscMode_e709RgbLimitedRange :
    case BAVC_CscMode_e709YCbCr :
        hdmiInput->stFieldData.eMatrixCoefficients = BAVC_MatrixCoefficients_eItu_R_BT_709 ;
        hdmiInput->stFieldData.eTransferCharacteristics = BAVC_TransferCharacteristics_eItu_R_BT_709 ; 
        break ;

    case BAVC_CscMode_e601RgbFullRange :
    case BAVC_CscMode_e601RgbLimitedRange :
    case BAVC_CscMode_e601YCbCr :
        hdmiInput->stFieldData.eMatrixCoefficients = BAVC_MatrixCoefficients_eSmpte_170M ;
        hdmiInput->stFieldData.eTransferCharacteristics = BAVC_TransferCharacteristics_eSmpte_170M ; 
        break ;

	/* coverity[dead_error_begin: FALSE] */
    default :
        BDBG_ERR(("Unknown Csc Mode: %d; default to 709", cscMode))  ;
        hdmiInput->stFieldData.eMatrixCoefficients = BAVC_MatrixCoefficients_eItu_R_BT_709 ;
        hdmiInput->stFieldData.eTransferCharacteristics = BAVC_TransferCharacteristics_eItu_R_BT_709 ; 
        break ;
    }

     /* report all changes */
    if (hdmiInput->stFieldData.bHdmiMode)
    {
         BDBG_MSG(("hdmiInput%d Receiving HDMI format %s (VIC: %d) %s; Current ColorSpace: %s",
            hdmiInput->index,
            BAVC_HDMI_AviInfoFrame_VideoIdCodeToStr(pAviInfoFrame->VideoIdCode),
            pAviInfoFrame->VideoIdCode,
            BAVC_HDMI_AviInfoFrame_ColorspaceToStr(colorSpace),
            BAVC_HDMI_AviInfoFrame_ColorspaceToStr(
                hdmiInput->stFieldData.eColorSpace))) ;
     }
    else /* DVI MODE if (hdmiInput->vdcStatus.hdmiMode) */
    {
         BDBG_MSG(("hdmiInput%d Receiving DVI  format %s",
            hdmiInput->index,
            BAVC_HDMI_AviInfoFrame_ColorspaceToStr(colorSpace))) ;
     }

    /* Set bMute for significant changes */
    if  (colorSpace != hdmiInput->stFieldData.eColorSpace
    ||  aspectRatio != hdmiInput->stFieldData.eAspectRatio
    ||  cscMode != hdmiInput->stFieldData.eCscMode)
    {
        BDBG_MSG(("hdmiInput%d Color Space change from '%s' to '%s'",
            hdmiInput->index,
            BAVC_HDMI_AviInfoFrame_ColorspaceToStr(
            hdmiInput->stFieldData.eColorSpace),
            BAVC_HDMI_AviInfoFrame_ColorspaceToStr(colorSpace))) ;
        hdmiInput->stFieldData.eAspectRatio = aspectRatio;
        hdmiInput->stFieldData.eColorSpace = colorSpace;
        hdmiInput->stFieldData.eCscMode = cscMode ;
        hdmiInput->stFieldData.bResetHdDviBegin = true;

        hdmiInput->stFieldData.bMute = true;
        hdmiInput->uiFormatChangeMuteCount = 10 ;
    }
    else
    {
        BDBG_MSG(("No Changes detected %s",
            BAVC_HDMI_AviInfoFrame_ColorspaceToStr(hdmiInput->stFieldData.eColorSpace))) ;
    }
}


static void NEXUS_HdmiInput_P_SetHdmiFormat_isr(NEXUS_HdmiInputHandle hdmiInput,
    NEXUS_HdmiVendorSpecificInfoFrame *pVSInfoFrame)
{
    BDBG_OBJECT_ASSERT(hdmiInput, NEXUS_HdmiInput);

     /* override Video Format to Normal Mode */
    if (pVSInfoFrame->packetStatus == (NEXUS_HdmiPacketStatus)BAVC_HDMI_PacketStatus_eStopped)
    {

        BDBG_WRN(("hdmiInput%d VSI packets stopped; Overriding HDMI Video Format from <%s> to <%s>",
            hdmiInput->index,
            BAVC_HDMI_VsInfoFrame_HdmiVideoFormatToStr(pVSInfoFrame->hdmiVideoFormat),
            BAVC_HDMI_VsInfoFrame_HdmiVideoFormatToStr(BAVC_HDMI_VSInfoFrame_HDMIVideoFormat_eNone))) ;

        pVSInfoFrame->hdmiVideoFormat = BAVC_HDMI_VSInfoFrame_HDMIVideoFormat_eNone ;
    }
    else
    {
         BDBG_MSG(("hdmiInput%d Receiving HDMI Video Format <%s>",
        hdmiInput->index,
        BAVC_HDMI_VsInfoFrame_HdmiVideoFormatToStr(pVSInfoFrame->hdmiVideoFormat))) ;
    }

    switch (pVSInfoFrame->hdmiVideoFormat)
    {
    case BAVC_HDMI_VSInfoFrame_HDMIVideoFormat_e3DFormat :
        /* app handles configuring windows for 3D mode */
        break ;

    case BAVC_HDMI_VSInfoFrame_HDMIVideoFormat_eExtendedResolution :
        BDBG_WRN(("hdmiInput%d HDMI Video Format <%s> is not supported",
            hdmiInput->index,
            BAVC_HDMI_VsInfoFrame_HdmiVideoFormatToStr(pVSInfoFrame->hdmiVideoFormat))) ;

        /* DO NOT BREAK; UNSUPPORTED MODE fall through and set mode to normal */

    default :
    case BAVC_HDMI_VSInfoFrame_HDMIVideoFormat_eNone :

        /* set to NORMAL mode */

        break ;
    }

    /* save current HDMI Video Format */
    g_NEXUS_hdmiInput.hdmiVideoFormat = pVSInfoFrame->hdmiVideoFormat ;
}


static void NEXUS_HdmiInput_P_PacketChange_isr(void *context, int param2, void *data)
{
    NEXUS_HdmiInputHandle hdmiInput = (NEXUS_HdmiInputHandle)context;

    BSTD_UNUSED(data);

    BDBG_OBJECT_ASSERT(hdmiInput, NEXUS_HdmiInput);
    BDBG_MSG(("hdmiInput%d PacketChange_isr %s (%#x)",
        hdmiInput->index, BAVC_HDMI_PacketTypeToStr(param2), param2)) ;

    switch ((BAVC_HDMI_PacketType)param2)
    {
    case BAVC_HDMI_PacketType_eAviInfoFrame:
        NEXUS_IsrCallback_Fire_isr(hdmiInput->aviInfoFrameChanged);
        break;
    case BAVC_HDMI_PacketType_eAudioInfoFrame:
        NEXUS_IsrCallback_Fire_isr(hdmiInput->audioInfoFrameChanged);
        break;
    case BAVC_HDMI_PacketType_eSpdInfoFrame:
        NEXUS_IsrCallback_Fire_isr(hdmiInput->spdInfoFrameChanged);
        break;
    case BAVC_HDMI_PacketType_eVendorSpecificInfoframe:
        NEXUS_HdmiInput_P_SetHdmiFormat_isr(hdmiInput, data) ;
        NEXUS_IsrCallback_Fire_isr(hdmiInput->vendorSpecificInfoFrameChanged);
        break;
    case BAVC_HDMI_PacketType_eAudioContentProtection:
        NEXUS_IsrCallback_Fire_isr(hdmiInput->audioContentProtectionChanged);
        break;
    default:
        BDBG_WRN(("HDMI Packet Type %x Change not supported", param2));
        break;
    }
}


void NEXUS_HdmiInput_GetIndex_priv(NEXUS_HdmiInputHandle hdmiInput, unsigned *pIndex)
{
    BDBG_OBJECT_ASSERT(hdmiInput, NEXUS_HdmiInput);
    NEXUS_ASSERT_MODULE();
    *pIndex = hdmiInput->index;
}

void NEXUS_HdmiInput_GetSourceId_priv(NEXUS_HdmiInputHandle hdmiInput, BAVC_SourceId *id)
{
    BDBG_OBJECT_ASSERT(hdmiInput, NEXUS_HdmiInput);
    NEXUS_ASSERT_MODULE();
    *id = BAVC_SourceId_eHdDvi0 + hdmiInput->index;
}

static void NEXUS_HdmiInput_P_SetFrameRate(void *data)
{
    NEXUS_HdmiInputHandle hdmiInput = (NEXUS_HdmiInputHandle)data;
    BDBG_OBJECT_ASSERT(hdmiInput, NEXUS_HdmiInput);
    /* I'm typecasting from Magnum to Nexus. The assert will check if this assumption fails in the future. */
    (void)NEXUS_Timebase_SetHdDviFrameRate(hdmiInput->settings.timebase, hdmiInput->frameRate);
}

void NEXUS_HdmiInput_SetFrameRate_isr(NEXUS_HdmiInputHandle hdmiInput, BAVC_FrameRateCode frameRate)
{
    BDBG_OBJECT_ASSERT(hdmiInput, NEXUS_HdmiInput);

#if 0
    BDBG_MSG(("frameRate %d", frameRate));
#endif
    hdmiInput->stFieldData.eFrameRateCode = frameRate;
    BDBG_CASSERT((NEXUS_VideoFrameRate)BAVC_FrameRateCode_e60 == NEXUS_VideoFrameRate_e60);
    hdmiInput->frameRate = (NEXUS_VideoFrameRate)frameRate;

    NEXUS_HdmiInput_P_SetSlaveFieldData_isr(hdmiInput);

    /* There is no _isr interface for programming PCR in XPT PI, therefore we must convert to task. */
    BKNI_SetEvent(hdmiInput->frameRateEvent);
}

NEXUS_Error NEXUS_HdmiInput_GetStatus(NEXUS_HdmiInputHandle hdmiInput, NEXUS_HdmiInputStatus *pStatus)
{
    BHDR_Status rxstatus;
    BAVC_HDMI_VideoFormat hdrFmtStatus;
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(hdmiInput, NEXUS_HdmiInput);
    BKNI_Memset(pStatus, 0, sizeof(*pStatus));
    BKNI_Memset(&rxstatus, 0, sizeof(rxstatus)) ;

    /* set default for noSignal and symbolLoss */
    pStatus->symbolLoss = 1 ;
    pStatus->noSignal = 1 ;

    if (!hdmiInput->videoConnected)
    {
        /* 
        -- although the hdmiInput may not be selected; 
        -- update its status to indicate if device is actually connected 
        -- NOTE: in the case of a switch, HPD may always be connected
        */
        rc = BHDR_GetHdmiRxStatus(hdmiInput->hdr, &rxstatus);
        if (rc)
            pStatus->deviceAttached = rxstatus.DeviceAttached ;

        BDBG_WRN(("hdmiInput%d device connected: %d", 
            pStatus->deviceAttached)) ;

        if (hdmiInput->vdcStatus.validHdmiStatus)
        {
            BDBG_WRN(("Unable to get status: hdmiInput%d is not selected/active",
                hdmiInput->index)) ;
            hdmiInput->vdcStatus.validHdmiStatus = false ;
        }
        goto done  ;
    }

    rc = BHDR_GetHdmiRxStatus(hdmiInput->hdr, &rxstatus);
    if ((rc) || (!rxstatus.bValidStatus))
    {
        /* report no device connected only once */
        if ((!rxstatus.DeviceAttached) && (hdmiInput->vdcStatus.deviceAttached))
        {
            BDBG_MSG(("Unable to get status: hdmiInput%d has no device connected. rc: %d ; rxStatus: %d",
                hdmiInput->index, rc, rxstatus.bValidStatus)) ;

            hdmiInput->vdcStatus.validHdmiStatus = false ;
        }
        goto done  ;
    }

    rc = BHDR_GetHdmiRxDetectedVideoFormat(hdmiInput->hdr, &hdrFmtStatus);
    if (rc) {BERR_TRACE(rc); return rc;}

    pStatus->hdmiMode = rxstatus.HdmiMode;
    pStatus->deviceAttached = rxstatus.DeviceAttached;
    pStatus->pllLocked = rxstatus.PllLocked;
    pStatus->packetErrors = rxstatus.bPacketErrors;
    pStatus->avMute = rxstatus.bAvmute;
    pStatus->hdcpRiUpdating = rxstatus.bHdcpRiUpdating;
    pStatus->symbolLoss = rxstatus.bSymbolLoss;

    pStatus->colorSpace = NEXUS_P_ColorSpace_FromMagnum(hdmiInput->reportedColorSpace);
    pStatus->aspectRatio = NEXUS_P_AspectRatio_FromMagnum(hdmiInput->stFieldData.eAspectRatio);
    pStatus->pixelRepeat = rxstatus.PixelRepeat;

    /* audio related information */
    pStatus->audio.packets = rxstatus.uiAudioPackets;
    pStatus->audio.validSpdifInfo = rxstatus.bValidChannelStatus ;
    pStatus->audio.streamType = rxstatus.stChannelStatus.eStreamType;
    pStatus->audio.wordLength = rxstatus.stChannelStatus.eWordLength;
    pStatus->audio.sampleFreq = rxstatus.stChannelStatus.eSamplingFrequency;


#if 0
    pStatus->audio.sampleRateHz = rxstatus.uiAudioSampleRateHz ;
#endif

    /* copy these from vdcStatus. make sure the code in NEXUS_VideoInput_P_SetHdmiInputStatus is setting these fields. */
    pStatus->interlaced = hdmiInput->vdcStatus.interlaced;
    pStatus->noSignal = hdmiInput->vdcStatus.noSignal;
    pStatus->originalFormat = hdmiInput->vdcStatus.originalFormat;
    pStatus->avHeight = hdmiInput->vdcStatus.avHeight;
    pStatus->avWidth = hdmiInput->vdcStatus.avWidth;
    pStatus->height = hdrFmtStatus.uActivelinesField1;
    pStatus->width = hdrFmtStatus.uHorizontalActivePixels;
    pStatus->vertFreq = hdmiInput->vdcStatus.vertFreq;
    pStatus->vBlank = hdmiInput->vdcStatus.vBlank;
    pStatus->hBlank = hdmiInput->vdcStatus.hBlank;
    pStatus->vPolarity = hdmiInput->vdcStatus.vPolarity;
    pStatus->hPolarity = hdmiInput->vdcStatus.hPolarity;

    rc = BHDR_GetPixelClockEstimate(hdmiInput->hdr, &pStatus->lineClock);

    pStatus->validHdmiStatus = true ;

done:
#if BDBG_DEBUG_BUILD
    /* for debugging, report changes in status */
    if ((pStatus->deviceAttached != hdmiInput->previousVdcStatus.deviceAttached)
    || (pStatus->validHdmiStatus != hdmiInput->previousVdcStatus.validHdmiStatus)
    || (pStatus->noSignal != hdmiInput->previousVdcStatus.noSignal)
    || (pStatus->pllLocked != hdmiInput->previousVdcStatus.pllLocked)
    || (pStatus->symbolLoss != hdmiInput->previousVdcStatus.symbolLoss)
    || (pStatus->hdmiMode != hdmiInput->previousVdcStatus.hdmiMode))
    {
        BDBG_MSG(("hdmiInput%d devAttached(%d) validStatus(%d) noSignal(%d) pllLock(%d) symbolLoss(%d) %s Mode",
            hdmiInput->index,
            pStatus->deviceAttached, pStatus->validHdmiStatus,
            pStatus->noSignal,  pStatus->pllLocked, pStatus->symbolLoss,
            pStatus->hdmiMode ? "HDMI" : "DVI")) ;
    }
    hdmiInput->previousVdcStatus =
#endif
    hdmiInput->vdcStatus = *pStatus ;

    return NEXUS_SUCCESS ;
}

void NEXUS_HdmiInput_GetModuleSettings_priv(NEXUS_HdmiInputHandle hdmiInput, NEXUS_HdmiInputModuleSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(hdmiInput, NEXUS_HdmiInput);
    NEXUS_ASSERT_MODULE();
    *pSettings = g_NEXUS_hdmiInput.settings;
}

static void NEXUS_HdmiInput_P_SetPower( NEXUS_HdmiInputHandle hdmiInput, bool callingHdr )
{
    BERR_Code rc ;
    BHDR_FE_ChannelPowerSettings powerSettings;      

    BDBG_OBJECT_ASSERT(hdmiInput, NEXUS_HdmiInput);

    BHDR_FE_GetPowerState(hdmiInput->frontend, &powerSettings);
    powerSettings.bHdmiRxPowered  =  callingHdr || hdmiInput->videoConnected ;

    BDBG_MSG(("Configure HDMI Rx Port%d : %s",
	      hdmiInput->index, powerSettings.bHdmiRxPowered ? "ON" : "OFF")) ;    
        
    rc = BHDR_FE_SetPowerState(hdmiInput->frontend, &powerSettings);
    if (rc) return ;
}


NEXUS_Error NEXUS_HdmiInput_SetMaster(
    NEXUS_HdmiInputHandle slaveHdmiInput, NEXUS_HdmiInputHandle masterHdmiInput)
{
    BERR_Code rc = NEXUS_SUCCESS ;

#if BHDR_CONFIG_MASTER_SLAVE_3D_SUPPORT
    BDBG_OBJECT_ASSERT(masterHdmiInput, NEXUS_HdmiInput);
    BDBG_OBJECT_ASSERT(slaveHdmiInput, NEXUS_HdmiInput);

    BDBG_WRN(("NEXUS_HdmiInput_SetMaster slave=%d(%p), master=%d(%p)",
        slaveHdmiInput->index, slaveHdmiInput,
        masterHdmiInput?masterHdmiInput->index:0, masterHdmiInput));

    if (slaveHdmiInput->masterHdmiInput != masterHdmiInput) {
        if (!masterHdmiInput && slaveHdmiInput->masterHdmiInput) {
            /* Clear Master/Slave configuration of HDR cores */
            rc = BHDR_UsageMasterSlaveClear(slaveHdmiInput->masterHdmiInput->hdr, slaveHdmiInput->hdr) ;
            if (rc) return BERR_TRACE(rc);

            /* default slaved core back to original config */
            rc = BHDR_FE_AttachHdmiRxCore(
                    slaveHdmiInput->frontend, slaveHdmiInput->hdr) ;
            if (rc) return BERR_TRACE(rc);
        }
        else if (masterHdmiInput) {
            /* must configure slave */
            rc = BHDR_UsageMasterSlaveSet(masterHdmiInput->hdr, slaveHdmiInput->hdr) ;
            if (rc) return BERR_TRACE(rc);

            rc = BHDR_FE_AttachHdmiRxCore(
                masterHdmiInput->frontend, slaveHdmiInput->hdr) ;
            if (rc) return BERR_TRACE(rc);

            BKNI_EnterCriticalSection();
            NEXUS_HdmiInput_P_SetSlaveFieldData_isr(masterHdmiInput);
            BKNI_LeaveCriticalSection();
        }
        slaveHdmiInput->masterHdmiInput = masterHdmiInput;
    }
#else
    BSTD_UNUSED(masterHdmiInput);
    BSTD_UNUSED(slaveHdmiInput);
#endif

    return rc;
}


void NEXUS_HdmiInput_VideoConnected_priv( NEXUS_HdmiInputHandle hdmiInput, bool videoConnected )
{
    BERR_Code rc ;

    BDBG_OBJECT_ASSERT(hdmiInput, NEXUS_HdmiInput);
    NEXUS_ASSERT_MODULE();

    hdmiInput->videoConnected = videoConnected;

    if (!hdmiInput->frontend) {
        return;
    }

    if (hdmiInput->masterHdmiInput) {
        /* I am the slave. Ignore this connected call. No HDR setup required on slave path. Master will set things up. */
        BDBG_WRN(("hdmiInput%d is slaved. Ignore this connected call. No HDR setup required on slave path. Master will set things up.",
            hdmiInput->index)) ;
        return;
    }

    if (videoConnected)
    {
        BHDR_Status rxStatus ;

        /* power up the HDMI core  */
        NEXUS_HdmiInput_P_SetPower(hdmiInput, videoConnected);

        /* port is ENABLED;  enable interrupts  */
        BKNI_EnterCriticalSection() ;
        BHDR_ConfigureAfterHotPlug_isr(hdmiInput->hdr, videoConnected) ;
        BKNI_LeaveCriticalSection() ;

        rc = BHDR_GetHdmiRxStatus(hdmiInput->hdr, &rxStatus);
        if (rc) {rc = BERR_TRACE(rc); }
		
        if (rxStatus.DeviceAttached)
        {
            /* toggle the hot plug; so attached device can re-initialize */
            NEXUS_HdmiInput_SetHotPlug(hdmiInput, true) ;
            if (!hdmiInput->releaseHotPlugTimer)
            {
                hdmiInput->releaseHotPlugTimer =
                    NEXUS_ScheduleTimer(115, NEXUS_HdmiInput_P_ReleaseHotPlug, hdmiInput) ;
            }
        }
    }
    else
    {
        /* port is DISABLED;  disable interrupts  */
        BKNI_EnterCriticalSection() ;

            BHDR_ConfigureAfterHotPlug_isr(hdmiInput->hdr, false) ;

        BKNI_LeaveCriticalSection() ;
	
        /* power down the HDMI core  */
        NEXUS_HdmiInput_P_SetPower(hdmiInput, videoConnected);
		
    }
}

NEXUS_Error NEXUS_HdmiInput_SetHotPlug(NEXUS_HdmiInputHandle hdmiInput, bool status)
{
    BERR_Code rc = BERR_SUCCESS;
    BHDR_HotPlugSignal eHotPlugSignal ;

    BDBG_OBJECT_ASSERT(hdmiInput, NEXUS_HdmiInput);

    if (!hdmiInput->videoConnected)
        return NEXUS_UNKNOWN ; 

    if (status) {
        eHotPlugSignal = BHDR_HotPlugSignal_eLow;
    }
    else {
        eHotPlugSignal = BHDR_HotPlugSignal_eHigh;
    }

    rc = BHDR_SetHotPlug(hdmiInput->hdr, eHotPlugSignal) ;

    return rc;
}

NEXUS_Error NEXUS_HdmiInput_ConfigureAfterHotPlug(NEXUS_HdmiInputHandle hdmiInput, bool status)
{
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(hdmiInput, NEXUS_HdmiInput);

    BDBG_WRN(("CH%d Manual HdmiInput_ConfgureAfterHotPlug: %d, VideoConnected: %d",
        hdmiInput->index, status, hdmiInput->videoConnected)) ;

#if 0
    if (!hdmiInput->videoConnected)
        return NEXUS_UNKNOWN ;
#endif

    rc = BHDR_ConfigureAfterHotPlug(hdmiInput->hdr, status) ;
    return rc;
}

NEXUS_Error NEXUS_HdmiInput_GetRawPacketData( NEXUS_HdmiInputHandle hdmiInput, NEXUS_HdmiInputPacketType packetType, NEXUS_HdmiPacket *pPacket )
{
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(hdmiInput, NEXUS_HdmiInput);
    BDBG_CASSERT(NEXUS_HdmiInputPacketType_eUnused == (NEXUS_HdmiInputPacketType)BAVC_HDMI_PacketType_eUnused);
    BDBG_CASSERT(sizeof(NEXUS_HdmiPacket) == sizeof(BAVC_HDMI_Packet));

    if (!hdmiInput->videoConnected)
        return NEXUS_UNKNOWN ;

    rc = BHDR_GetRawPacketData(hdmiInput->hdr, (BAVC_HDMI_PacketType)packetType, (BAVC_HDMI_Packet*)pPacket);

    return rc;
}

void NEXUS_HdmiInput_SetStatus_priv( NEXUS_HdmiInputHandle hdmiInput, const NEXUS_HdmiInputStatus *pStatus )
{
    BDBG_OBJECT_ASSERT(hdmiInput, NEXUS_HdmiInput);
    NEXUS_ASSERT_MODULE();

    /* Status Update coming from HDDVI format detection block */
    /* Inform of changes from HDDVI block only  */

    if ((hdmiInput->vdcStatus.interlaced ==   pStatus->interlaced)
    && (hdmiInput->vdcStatus.originalFormat ==  pStatus->originalFormat) 
    && (hdmiInput->vdcStatus.noSignal ==   pStatus->noSignal) 
    && (hdmiInput->vdcStatus.avWidth ==   pStatus->avWidth) 
    && (hdmiInput->vdcStatus.avHeight ==   pStatus->avHeight) 
    && (hdmiInput->vdcStatus.vertFreq ==   pStatus->vertFreq) 
    && (hdmiInput->vdcStatus.hPolarity ==   pStatus->hPolarity) 
    && (hdmiInput->vdcStatus.vPolarity ==   pStatus->vPolarity) 
    && (hdmiInput->vdcStatus.vBlank ==   pStatus->vBlank) 
    && (hdmiInput->vdcStatus.hBlank ==   pStatus->hBlank)
    && (hdmiInput->vdcStatus.pixelRepeat ==   pStatus->pixelRepeat))
        return ;

    hdmiInput->vdcStatus = *pStatus;

    /* set initial HdmiStatus for reporting purposes */
    hdmiInput->vdcStatus.validHdmiStatus = true ;
    hdmiInput->vdcStatus.deviceAttached = true ;

    NEXUS_TaskCallback_Fire(hdmiInput->sourceChangedCallback);
}

static void NEXUS_HdmiInput_P_HotPlug_isr(void *context, int param, void *data)
{
    NEXUS_HdmiInputHandle hdmiInput = context;
    bool deviceAttached =  * (bool *) data ;

    BSTD_UNUSED(param);
    BDBG_OBJECT_ASSERT(hdmiInput, NEXUS_HdmiInput);

    BHDR_ConfigureAfterHotPlug_isr(hdmiInput->hdr, deviceAttached) ;
    NEXUS_IsrCallback_Fire_isr(hdmiInput->hotPlugCallback) ;
}

NEXUS_Error NEXUS_HdmiInput_GetAviInfoFrameData( NEXUS_HdmiInputHandle hdmiInput, NEXUS_HdmiAviInfoFrame *pAviInfoFrame )
{
    BDBG_OBJECT_ASSERT(hdmiInput, NEXUS_HdmiInput);
    BDBG_CASSERT(sizeof(*pAviInfoFrame) == sizeof(BAVC_HDMI_AviInfoFrame));

    if (!hdmiInput->videoConnected)
        return NEXUS_UNKNOWN ;

    return BHDR_GetAviInfoFrameData(hdmiInput->hdr, (BAVC_HDMI_AviInfoFrame*)pAviInfoFrame);
}

NEXUS_Error NEXUS_HdmiInput_GetAudioInfoFrameData( NEXUS_HdmiInputHandle hdmiInput, NEXUS_HdmiAudioInfoFrame *pAudioInfoFrame )
{
    BDBG_OBJECT_ASSERT(hdmiInput, NEXUS_HdmiInput);
    BDBG_CASSERT(sizeof(*pAudioInfoFrame) == sizeof(BAVC_HDMI_AudioInfoFrame));

    if (!hdmiInput->videoConnected)
        return NEXUS_UNKNOWN ;

    return BHDR_GetAudioInfoFrameData(hdmiInput->hdr, (BAVC_HDMI_AudioInfoFrame*)pAudioInfoFrame);
}

NEXUS_Error NEXUS_HdmiInput_GetSpdInfoFrameData( NEXUS_HdmiInputHandle hdmiInput, NEXUS_HdmiSpdInfoFrame *pSpdInfoFrame )
{
    BDBG_OBJECT_ASSERT(hdmiInput, NEXUS_HdmiInput);
    BDBG_CASSERT(sizeof(*pSpdInfoFrame) == sizeof(BAVC_HDMI_SPDInfoFrame));

    if (!hdmiInput->videoConnected)
        return NEXUS_UNKNOWN ;

    return BHDR_GetSPDInfoFrameData(hdmiInput->hdr, (BAVC_HDMI_SPDInfoFrame*)pSpdInfoFrame);
}

NEXUS_Error NEXUS_HdmiInput_GetVendorSpecificInfoFrameData( NEXUS_HdmiInputHandle
    hdmiInput, NEXUS_HdmiVendorSpecificInfoFrame *pVendorSpecificInfoFrame )
{
    NEXUS_Error rc ;
    BDBG_OBJECT_ASSERT(hdmiInput, NEXUS_HdmiInput);
    BDBG_CASSERT(sizeof(*pVendorSpecificInfoFrame) == sizeof(BAVC_HDMI_VendorSpecificInfoFrame));

    rc = BHDR_GetVendorSpecificInfoFrameData(   hdmiInput->hdr, (BAVC_HDMI_VendorSpecificInfoFrame*)pVendorSpecificInfoFrame);
    if (rc) {rc = BERR_TRACE(rc); return rc;}

#if BDBG_DEBUG_BUILD
    BDBG_CASSERT(NEXUS_HdmiVendorSpecificInfoFrame_HDMIVideoFormat_eMax == (NEXUS_HdmiVendorSpecificInfoFrame_HDMIVideoFormat)BAVC_HDMI_VSInfoFrame_HDMIVideoFormat_eMax);
    BDBG_MSG(("hdmiInput%d HDMI Video Format: %s (%d)",
        hdmiInput->index,
        BAVC_HDMI_VsInfoFrame_HdmiVideoFormatToStr(pVendorSpecificInfoFrame->hdmiVideoFormat),
        pVendorSpecificInfoFrame->hdmiVideoFormat)) ;

    if (pVendorSpecificInfoFrame->hdmiVideoFormat == NEXUS_HdmiVendorSpecificInfoFrame_HDMIVideoFormat_e3DFormat)
    {
        BDBG_MSG(("hdmiInput%d VSI HDMI 3D Structure: '%s' (%d)",
            hdmiInput->index,
            BAVC_HDMI_VsInfoFrame_3DStructureToStr(pVendorSpecificInfoFrame->hdmi3DStructure),
            pVendorSpecificInfoFrame->hdmi3DStructure)) ;
    }
    else
    {
        BDBG_MSG(("hdmiInput%d VSI HDMI Format '%s' not supported",
            hdmiInput->index,
            BAVC_HDMI_VsInfoFrame_HdmiVideoFormatToStr(pVendorSpecificInfoFrame->hdmiVideoFormat))) ;
    }
#endif

    return rc ;
}

NEXUS_Error NEXUS_HdmiInput_GetAudioContentProtection( NEXUS_HdmiInputHandle hdmiInput, NEXUS_HdmiAudioContentProtection *pAcp )
{
    BDBG_OBJECT_ASSERT(hdmiInput, NEXUS_HdmiInput);
    BDBG_CASSERT(sizeof(*pAcp) == sizeof(BAVC_HDMI_ACP));

    if (!hdmiInput->videoConnected)
        return NEXUS_UNKNOWN ;

    return BHDR_GetAudioContentProtection(hdmiInput->hdr, (BAVC_HDMI_ACP*)pAcp);
}

NEXUS_Error NEXUS_HdmiInput_GetAudioClockRegeneration( NEXUS_HdmiInputHandle hdmiInput, NEXUS_HdmiAudioClockRegeneration *pAudioClockRegeneration )
{
    BDBG_OBJECT_ASSERT(hdmiInput, NEXUS_HdmiInput);
    BDBG_CASSERT(sizeof(*pAudioClockRegeneration) == sizeof(BAVC_HDMI_AudioClockRegenerationPacket));

    if (!hdmiInput->videoConnected)
        return NEXUS_UNKNOWN ;

    return BHDR_GetAudioClockRegenerationData(hdmiInput->hdr, (BAVC_HDMI_AudioClockRegenerationPacket*)pAudioClockRegeneration);
}

NEXUS_Error NEXUS_HdmiInput_GetGeneralControlPacket( NEXUS_HdmiInputHandle hdmiInput, NEXUS_HdmiGeneralControlPacket *pGeneralControlPacket )
{
    BDBG_OBJECT_ASSERT(hdmiInput, NEXUS_HdmiInput);
    BDBG_CASSERT(sizeof(*pGeneralControlPacket) == sizeof(BAVC_HDMI_GcpData));

    if (!hdmiInput->videoConnected)
        return NEXUS_UNKNOWN ;

    return BHDR_GetGeneralControlPacketData(hdmiInput->hdr, (BAVC_HDMI_GcpData*)pGeneralControlPacket);
}


NEXUS_Error NEXUS_HdmiInput_GetISRCData( NEXUS_HdmiInputHandle hdmiInput, NEXUS_HdmiISRC *pISRC )
{
    BDBG_OBJECT_ASSERT(hdmiInput, NEXUS_HdmiInput);
    BDBG_CASSERT(sizeof(*pISRC) == sizeof(BAVC_HDMI_ISRC));

    if (!hdmiInput->videoConnected)
        return NEXUS_UNKNOWN ;

    return BHDR_GetISRCData(hdmiInput->hdr, (BAVC_HDMI_ISRC *)pISRC);
}



NEXUS_Error NEXUS_HdmiInput_DebugGetEyeDiagramData(NEXUS_HdmiInputHandle hdmiInput, uint8_t *padc_data)
{
#if BHDR_CONFIG_DEBUG_EYE_DIAGRAM

    BDBG_OBJECT_ASSERT(hdmiInput, NEXUS_HdmiInput);
    return BHDR_FE_DEBUG_GetEyeDiagramData(hdmiInput->hdr, padc_data);
#else
    BSTD_UNUSED(hdmiInput) ;
    BSTD_UNUSED(padc_data) ;
    return BERR_SUCCESS ;
#endif
}

void NEXUS_HdmiInput_SetFormatChangeCb_priv(
    NEXUS_HdmiInputHandle hdmiInput,
    NEXUS_HdmiInputFormatChangeCb pFunction_isr,
    void *pFuncParam
    )
{
    NEXUS_ASSERT_MODULE();
    BDBG_OBJECT_ASSERT(hdmiInput, NEXUS_HdmiInput);
    BKNI_EnterCriticalSection();
    hdmiInput->pPcFormatCallback_isr = pFunction_isr;
    hdmiInput->pPcFormatCallbackParam = pFuncParam;
    BKNI_LeaveCriticalSection();
}

static void NEXUS_HdmiInput_P_ReleaseHotPlug(void *context)
{
    NEXUS_HdmiInputHandle hdmiInput = context ;

    BDBG_OBJECT_ASSERT(hdmiInput, NEXUS_HdmiInput);
    hdmiInput->releaseHotPlugTimer = NULL;
    NEXUS_HdmiInput_SetHotPlug(hdmiInput, false) ;
}





