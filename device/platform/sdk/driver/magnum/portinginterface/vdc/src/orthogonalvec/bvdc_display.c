/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_display.c $
 * $brcm_Revision: Hydra_Software_Devel/134 $
 * $brcm_Date: 10/4/12 3:43p $
 *
 * Module Description:
 *  Main module for BVDC_Display_ functions.
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/orthogonalvec/bvdc_display.c $
 * 
 * Hydra_Software_Devel/134   10/4/12 3:43p vanessah
 * SW7435-386: update the aspect ratio when new fmt applies
 * 
 * Hydra_Software_Devel/133   9/24/12 6:19p syang
 * SW7425-3977: add aspect-ratio handling for 3D video src and display
 * 
 * Hydra_Software_Devel/132   8/20/12 8:40p tdo
 * SW7429-238: Cannot RemoveOutput and then AddOutput for RFM
 *
 * Hydra_Software_Devel/131   8/16/12 2:42p yuxiaz
 * SW7425-3619: Add const to read only settings.
 *
 * Hydra_Software_Devel/130   8/10/12 3:29p pntruong
 * SW7435-276: Updated NTSC/480p to 720x480i/720x480p respectively.
 * Added support 482i/483p with new enums.
 *
 * Hydra_Software_Devel/129   7/3/12 7:26p albertl
 * SW7125-1297: Changed Svideo Chroma VF adjustments to adjust Ch1 and Ch2
 * and Composite adjustments to affect all channels.
 *
 * Hydra_Software_Devel/128   6/20/12 4:43p pntruong
 * SW7405-5488: Added support for querying display capabilties of vdc
 * supportable format.
 *
 * Hydra_Software_Devel/127   6/12/12 6:50p darnstein
 * SW7425-3226: fix Coverity errors involving suspicious tests for NULL
 * pointer.
 *
 * Hydra_Software_Devel/126   3/27/12 11:43a yuxiaz
 * SW7425-2753: Treat display orientation change as format change to flush
 * all the buffers in video windows to avoid BVN errors.
 *
 * Hydra_Software_Devel/125   3/21/12 1:39p syang
 * SW7425-2699: put mpaa under the control of vdc resource manager; handle
 * correctly both order of enabling/disabling mpaa and turning on/off
 * hdmi/component output
 *
 * Hydra_Software_Devel/124   3/2/12 4:02p pntruong
 * SW7231-584: Added artificial display vsync generation to support dsp
 * transcoding.
 *
 * Hydra_Software_Devel/122   2/23/12 10:18a vanessah
 * SW7425-2431: fix custom fmt stress test
 *
 * Hydra_Software_Devel/121   1/26/12 1:51p yuxiaz
 * SW7425-1875, SW7125-1222: Only print out error message when
 * BVDC_3dSourceBufferSelect setting is not
 * BVDC_3dSourceBufferSelect_eNormal on chips having no 3d support in
 * MFD.
 *
 * Hydra_Software_Devel/120   1/20/12 6:29p vanessah
 * SW7435-9: add STG2/3 support
 *
 * Hydra_Software_Devel/SW7125-1124/1   1/11/12 2:58p darnstein
 * SW7125-1124: Modify definition of BVDC_P_SUPPORT_IT_VER.
 *
 * Hydra_Software_Devel/118   12/13/11 5:01p yuxiaz
 * SW7425-1875: Added support for source buffer selection in 3d mode.
 *
 * Hydra_Software_Devel/117   11/29/11 11:26a vanessah
 * SW7425-1783: add user data support in VDC PI
 *
 * Hydra_Software_Devel/116   11/3/11 3:57p tdo
 * SW7344-135: Video lost in Vesa format on HDMI/Component outputs when
 * switching output formats during a playback.  IT stand-alone is not
 * working for 7344B0/7231B0
 *
 * Hydra_Software_Devel/115   10/27/11 4:30p syang
 * SW7425-1429: move per vsync call back from window to display; clean up
 * display callback description/implementation
 *
 * Hydra_Software_Devel/114   9/30/11 5:14p darnstein
 * SW7400-2665: merge from JIRA branch.
 *
 * Hydra_Software_Devel/SW7400-2665/2   9/30/11 4:57p darnstein
 * SW7400-2665: merge from main branch.
 *
 * Hydra_Software_Devel/SW7400-2665/1   9/28/11 7:26p darnstein
 * SW7400-2665: Implement double rate 576P analog output for orthogonal
 * VEC chipsets.
 *
 * Hydra_Software_Devel/113   9/29/11 4:50p tdo
 * SW7425-980: Insure HDMI tx is always in slave mode
 *
 * Hydra_Software_Devel/112   9/22/11 11:01a tdo
 * SW7425-98: Insure HDMI tx is always in slave mode
 *
 * Hydra_Software_Devel/111   9/8/11 10:19a syang
 * SW7425-1265: honor back to back new setting
 *
 * Hydra_Software_Devel/110   8/31/11 5:39p pntruong
 * SW7425-973:  Implement bandwidth restriction for madr.
 *
 * Hydra_Software_Devel/109   8/30/11 3:08p syang
 * SW7425-1170: clean up
 *
 * Hydra_Software_Devel/107   8/26/11 5:40p syang
 * SW7425-1170: merge from branch SW7572-1170
 *
 * Hydra_Software_Devel/SW7425-1170/2   8/26/11 4:28p syang
 * SW7425-1170: refactor all pixel aspect ratio related code
 *
 * Hydra_Software_Devel/SW7425-1170/1   8/24/11 6:57p vanessah
 * SW7425-1170: ASP code clean
 *
 * Hydra_Software_Devel/106   8/25/11 3:55p pntruong
 * SW7425-1191: Rollback.  The acquire/release is more efficience to be
 * handle in nexus, where the done event is already known.
 *
 * Hydra_Software_Devel/105   8/25/11 9:21a vanessah
 * SW7425-1191:sg pwr management
 *
 * Hydra_Software_Devel/104   8/22/11 12:17p tdo
 * SW7231-309: handling vec resource when analog is in slave mode.
 *
 * Hydra_Software_Devel/103   8/21/11 5:34p hongtaoz
 * SW7425-592: removed order dependent code in display aspect ratio Set
 * APIs;
 *
 * Hydra_Software_Devel/102   8/15/11 2:04p vanessah
 * SW7425-1083: get rid of bogus setting
 *
 * Hydra_Software_Devel/101   8/15/11 12:48p vanessah
 * SW7425-1083: correct indent
 *
 * Hydra_Software_Devel/100   8/15/11 12:26p vanessah
 * SW7425-1083: further code optimization
 *
 * Hydra_Software_Devel/99   8/15/11 11:43a vanessah
 * SW7425-1083:enable stg on create display
 *
 * Hydra_Software_Devel/98   8/12/11 2:55p vanessah
 * SW7425-1083: enable STG in master mode
 *
 * Hydra_Software_Devel/97   8/5/11 8:25p albertl
 * SW7425-745: Added blanking to compositor color for all outputs and all
 * colorspaces.
 *
 * Hydra_Software_Devel/96   7/18/11 5:41p albertl
 * SW7408-291: Relaxed VF filter requirements to be able to take any
 * register array size greater than platform minimum.
 *
 * Hydra_Software_Devel/95   7/18/11 11:24a vanessah
 * SW7425-835: SW7425-923: fix ViCE2 channel id bug + B0 STG
 *
 * Hydra_Software_Devel/94   7/11/11 5:28p albertl
 * SW7408-291: Fixed missing call to get analog channel for error
 * checking.
 *
 * Hydra_Software_Devel/93   7/11/11 2:18p tdo
 * SW7420-1971: Video Pause seen when VEC alignment is going on.  Add flag
 * to keep BVN connected while doing alignment.
 *
 * Hydra_Software_Devel/92   7/11/11 1:19p albertl
 * SW7408-291: Fixed warning.
 *
 * Hydra_Software_Devel/91   7/8/11 6:01p albertl
 * SW7408-291: Updated VF filter API to use Dac enums for channel
 * selection.
 *
 * Hydra_Software_Devel/90   7/8/11 3:12p yuxiaz
 * SW7425-592: Added back display sample aspect ratio support lost due to
 * merge.
 *
 * Hydra_Software_Devel/89   7/6/11 6:07p albertl
 * SW7408-291, SW7425-745: Changed VF filter API to also take a selected
 * display output as parameter.  Added selective muting of display
 * outputs using CSC.
 *
 * Hydra_Software_Devel/88   6/22/11 5:58p albertl
 * SW7408-291: Updated interface of get vf filter API and renamed
 * variables for clarity.
 *
 * Hydra_Software_Devel/86   6/17/11 2:33p albertl
 * SW7408-291:  Added interface to set and get VF filter settings.
 *
 * Hydra_Software_Devel/85   6/9/11 1:17p tdo
 * SW7405-5339: BVDC_Display_Create doesn't recover from bad parameters.
 * Fix various bugs in error handling cases.
 *
 * Hydra_Software_Devel/84   4/13/11 11:40a pntruong
 * SW7231-122: Remove usage assumption as to which display can be hd.
 *
 * Hydra_Software_Devel/83   4/11/11 11:05a vanessah
 * SW7425-310: change the alignment back
 *
 * Hydra_Software_Devel/82	 4/8/11 5:26p vanessah
 * SW7425-310: add checksum for continous custom format changes
 *
 * Hydra_Software_Devel/81   3/21/11 1:42p tdo
 * SW7420-1638: DACs do not get powered down properly with certain call
 * sequences
 *
 * Hydra_Software_Devel/80   1/7/11 5:13p tdo
 * SW7125-792: Properly handle HDMI sync only
 *
 * Hydra_Software_Devel/79   12/30/10 10:03a pntruong
 * SW3548-1660: Turn BVDC_Test_Display_GetInterruptName into official api
 * BVDC_Display_GetInterruptName.
 *
 * Hydra_Software_Devel/78   12/20/10 3:29p pntruong
 * SW7550-577: Returned approriate vbipath enum with invalid analog it
 * channel.
 *
 * Hydra_Software_Devel/77   11/18/10 12:32p tdo
 * SW7422-92: Add function to set display orientation.  Add new dirty bit
 * to handle 3D setting
 *
 * Hydra_Software_Devel/76   11/18/10 12:05a hongtaoz
 * SW7422-13, SW7425-29: bring up dtg master mode on/off toggle; make
 * display switch mode more robust; defer DTG reset from disable to
 * enable to avoid timeout; bring up 656out;
 *
 * Hydra_Software_Devel/75   11/16/10 6:52p tdo
 * SW7422-92: Need display 3D orientation override function for VDC
 *
 * Hydra_Software_Devel/74   11/16/10 4:30p hongtaoz
 * SW7425-33: allow HD format on cmp3;
 *
 * Hydra_Software_Devel/73   11/15/10 10:26a vanessah
 * SW7550-577:  SCART RGB(YPbPr) and CVBS don't output at the same time.
 *
 * Hydra_Software_Devel/72   10/12/10 11:00a vanessah
 * SW7550-598:  Rolling back to old version
 *
 * Hydra_Software_Devel/71   10/12/10 10:29a vanessah
 * SW7550-598:  Add BVDC_Display_GetDisplayMasterTg
 *
 * Hydra_Software_Devel/70   10/4/10 5:17p hongtaoz
 * SW7425-33: added the 4th compositor support;
 *
 * Hydra_Software_Devel/69   10/2/10 1:31a hongtaoz
 * SW7425-29: adding STG support;
 *
 * Hydra_Software_Devel/68   10/1/10 11:14a hongtaoz
 * SW7425-29: added explicit enable argument to the STG set API;
 *
 * Hydra_Software_Devel/67   9/30/10 1:24p tdo
 * SW7550-588: Changing video format causes DACs to power down.
 *
 * Hydra_Software_Devel/66   9/23/10 10:32a vanessah
 * SW7425-29:  STG support for transcoding
 *
 * Hydra_Software_Devel/65   9/17/10 1:39p tdo
 * SW7420-1096: undo previous check in to fix problem of no RFM output
 * when Svideo is enabled
 *
 * Hydra_Software_Devel/64   9/16/10 9:51p tdo
 * SW7420-1096: No RFM output when composite output is enabled
 *
 * Hydra_Software_Devel/63   9/9/10 3:15p rpan
 * SW7550-561: Improved VEC resource management error messages.
 *
 * Hydra_Software_Devel/62   8/24/10 3:53p tdo
 * SW7420-978: BVB_SAV_REMOVE is set to 3 with 704 microcode.  Clean up
 * duplicated logic.
 *
 * Hydra_Software_Devel/61   6/7/10 4:45p tdo
 * SW7208-57, SW7125-248: VEC resource management fine tuning
 *
 * Hydra_Software_Devel/60   5/13/10 1:45p rpan
 * SW7420-738: API BVDC_Display_SetHdmiConfiguration() enables HDMI only
 * if it is in disabled state. Otherwise it simply adjusts CSC settings.
 *
 * Hydra_Software_Devel/59   5/12/10 12:13p rpan
 * SW7420-738: Enable/disable VEC alignment as long as user request it.
 *
 * Hydra_Software_Devel/58   5/10/10 2:41p albertl
 * SW7125-364: Fixed build error for 7468.
 *
 * Hydra_Software_Devel/57   5/7/10 7:22p albertl
 * SW7125-364: Changed dirty bits to use union structure to avoid type-pun
 * warnings
 *
 * Hydra_Software_Devel/56   4/19/10 10:26p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic. Move
 * BDBG_OBJECT_ID_DECLARE private header files instead of .c.
 *
 * Hydra_Software_Devel/55   4/7/10 5:07p darnstein
 * SW7420-543: fix previous check in.
 *
 * Hydra_Software_Devel/54   4/7/10 4:08p darnstein
 * SW7420-543: Analog 480P output is always sampled at 54Mhz.
 *
 * Hydra_Software_Devel/53   4/7/10 11:40a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/52   4/5/10 4:19p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/51   2/9/10 2:09p rpan
 * SW7400-2642: Added SECAM output for orthogonal VEC.
 *
 * Hydra_Software_Devel/50   1/6/10 3:41p rpan
 * SW7468-64: Consolidate VEC modulo count trigger implementation.
 *
 * Hydra_Software_Devel/49   12/18/09 8:59a rpan
 * SW3556-935: New interface for setting spread spectrum.
 *
 * Hydra_Software_Devel/48   11/30/09 10:34a rpan
 * SW7468-30: Initial OSCL support.
 *
 * Hydra_Software_Devel/47   11/2/09 4:35p tdo
 * SW7340-71: Flash occurs when Same Format is Set in VDC (toggling 16x9
 * or Auto-Format.)
 *
 * Hydra_Software_Devel/46   10/14/09 10:39a tdo
 * SW7405-3004: Fix coverity error
 *
 * Hydra_Software_Devel/45   8/24/09 3:56p rpan
 * SW7342-27: Make analog channel 1 data structure and programming model
 * consistent with channel 0. Enhance channel 1 handling state machine.
 *
 * Hydra_Software_Devel/44   8/21/09 2:34p tdo
 * PR57734: Add capability for display to handle DACs re-assignment
 *
 * Hydra_Software_Devel/43   8/20/09 3:59p rpan
 * PR57873: Disable analog path channel 1 on all chips with orthogonal VEC
 * for now.
 *
 * Hydra_Software_Devel/42   8/19/09 4:17p tdo
 * PR57734: Temp fix for 7342
 *
 * Hydra_Software_Devel/41   6/25/09 12:12p rpan
 * PR56137, PR56138, PR56139, PR56166, PR56167, PR56168: Support for
 * various orthogonal VEC configurations.
 *
 * Hydra_Software_Devel/40   6/9/09 3:41p rpan
 * PR55613, PR55643: 1) Each user display settings now generates dirty
 * bit(s). This simplifies how invalid user settings can be cleaned. 2)
 * Moved HDMI and ITU-656 validation to per display basis. 3) Fixed the
 * ITU-656 not enabled in slave mode issue.
 *
 * Hydra_Software_Devel/39   6/1/09 2:14p darnstein
 * PR48884: prevent more crashes when user specifies bad video
 * configuration on VEC. These changes specifically support 7420.
 *
 * Hydra_Software_Devel/38   4/8/09 1:59p tdo
 * PR53373: Add HDMI deep color support
 *
 * Hydra_Software_Devel/37   4/7/09 9:57a tdo
 * PR53373: Add HDMI deep color support
 *
 * Hydra_Software_Devel/36   3/16/09 10:55p tdo
 * PR45785, PR45789: Merge from MCVP branch
 *
 * Hydra_Software_Devel/7420_mcvp/4   2/13/09 6:08p tdo
 * PR 45785, PR 45789: merge from main branch on 2/13/09
 *
 * Hydra_Software_Devel/35   2/13/09 9:39a rpan
 * PR52100: Got the user registered callback mechanism working.
 *
 * Hydra_Software_Devel/7420_mcvp/3   2/11/09 11:36a tdo
 * PR 45785, PR 45789: merge from main branch on 2/11/09
 *
 * Hydra_Software_Devel/34   2/10/09 7:12p pntruong
 * PR51976: [GDVR-BR] Frame Skip is happened at Analog RF.  Also used
 * vertical frequency as another condition for detecting new custom ucode
 * loaded.
 *
 * Hydra_Software_Devel/7420_mcvp/2   2/9/09 7:56p darnstein
 * PR45819: I changed the operation of BVDC_Display_GetVbiPath() to suit
 * 7420. It now returns the identity of the IT block in use.
 *
 * Hydra_Software_Devel/7420_mcvp/1   2/9/09 4:33p tdo
 * PR 45785, PR 45789: merge from main branch on 2/9/09
 *
 * Hydra_Software_Devel/33   2/9/09 3:34p rpan
 * PR45804: Some clean-up work.
 *
 * Hydra_Software_Devel/32   2/9/09 9:01a rpan
 * PR45804: Removed the BVDC_Display_GetItNumber() API.
 *
 * Hydra_Software_Devel/31   2/6/09 2:52p rpan
 * PR45804: Added HDMI sync-only support.
 *
 * Hydra_Software_Devel/30   2/5/09 1:18p rpan
 * PR45804: Added API for query which IT is assigned to the display.
 *
 * Hydra_Software_Devel/29   2/5/09 11:12a rpan
 * PR45804: More work for seamless attachment of a slave path.
 *
 * Hydra_Software_Devel/28   2/5/09 8:50a rpan
 * PR45804: Added the initial support for Hdmi XvYcc.
 *
 * Hydra_Software_Devel/27   2/4/09 3:43p rpan
 * PR45804: Added HDMI drop-line support.
 *
 * Hydra_Software_Devel/26   2/3/09 9:38a rpan
 * PR45804: Implemented another scheme of attaching DVI slave path to
 * analog master path, which resets master path.
 *
 * Hydra_Software_Devel/25   1/29/09 4:49p rpan
 * PR45804: Added support for alignment.
 *
 * Hydra_Software_Devel/24   1/26/09 4:21p rpan
 * PR45804: Initial support for ITU656 output.
 *
 * Hydra_Software_Devel/23   1/26/09 11:28a rpan
 * PR45804: Store user specified DVI output csc info.
 *
 * Hydra_Software_Devel/22   1/21/09 10:14a rpan
 * PR45804: More HDMI output work.
 *
 * Hydra_Software_Devel/21   1/20/09 9:11a rpan
 * PR45804: Initial HDMI support  work.
 *
 * Hydra_Software_Devel/20   1/12/09 4:07p rpan
 * PR45804: Added initial RFM output support.
 *
 * Hydra_Software_Devel/19   1/9/09 7:16p darnstein
 * PR45819: Restore BVDC_Display_GetVbiPath(). This will need to be
 * refined.
 *
 * Hydra_Software_Devel/18   1/9/09 2:17p rpan
 * PR45804: Added component path MPAA support.
 *
 * Hydra_Software_Devel/17   12/24/08 10:22a rpan
 * PR50391: Assign triggers according to the IT acquired. We used to hard
 * code that.
 *
 * Hydra_Software_Devel/16   12/23/08 2:15p rpan
 * PR45804: Implemented force frame drop handlers.
 *
 * Hydra_Software_Devel/15   12/23/08 11:43a rpan
 * PR50391: Moved VF, CSC, SRC and SM programming to DAC event handler.
 *
 * Hydra_Software_Devel/14   12/23/08 10:00a rpan
 * PR45804: Complete handlers for stream source related events.
 *
 * Hydra_Software_Devel/13   12/22/08 10:30p tdo
 * PR50391: Force Timing dirty when Dac dirty
 *
 * Hydra_Software_Devel/12   12/22/08 3:19p rpan
 * PR45804: Added support for display callback.
 *
 * Hydra_Software_Devel/11   12/22/08 12:00p rpan
 * PR50391: Convert those not yet implemented APIs to return success for
 * bring-up.
 *
 * Hydra_Software_Devel/10   12/19/08 4:34p rpan
 * PR50391: Implemented set timebase support.
 *
 * Hydra_Software_Devel/9   12/19/08 3:27p rpan
 * PR50391: Replaced assertion by error message for not yet implemented
 * APIs.
 *
 * Hydra_Software_Devel/8   12/19/08 2:00p rpan
 * PR50391: Cleaned up some comments.
 *
 * Hydra_Software_Devel/7   12/19/08 12:11p rpan
 * PR50391: Diable analog chan 1. Also not use Secam_0.
 *
 * Hydra_Software_Devel/6   12/17/08 7:52p tdo
 * PR50391: Add support for Analog Chan 1
 *
 * Hydra_Software_Devel/5   12/9/08 4:45p rpan
 * PR45804: More orthogonal VEC support.
 *
 * Hydra_Software_Devel/4   12/8/08 5:44p rpan
 * PR45804: Switched to VDC generic resource management lib.
 *
 * Hydra_Software_Devel/3   12/3/08 10:10p rpan
 * PR45804: More orthogonal VEC support.
 *
 * Hydra_Software_Devel/2   12/3/08 4:39p rpan
 * PR45804: Initial effort for the orthogonal VEC support.
 *
 * Hydra_Software_Devel/1   11/24/08 4:38p rpan
 * PR45804: Add a 7420 specific display implementation.
 *
 *
 ***************************************************************************/
#include "bstd.h"          /* standard types */
#include "bkni.h"          /* For malloc */

#include "bvdc.h"
#include "bvdc_priv.h"
#include "bvdc_display_priv.h"
#include "bvdc_displayfmt_priv.h"
#include "bvdc_compositor_priv.h"
#include "bvdc_feeder_priv.h"

BDBG_MODULE(BVDC_DISP);

/*************************************************************************
 *	BVDC_Display_GetDefaultSettings
 *
 *************************************************************************/
BERR_Code BVDC_Display_GetDefaultSettings
	( BVDC_DisplayId                   eDisplayId,
	  BVDC_Display_Settings           *pDefSettings )
{
	/* Clear out first */
	BKNI_Memset(pDefSettings, 0, sizeof(BVDC_Display_Settings));

	if(BVDC_DisplayId_eAuto == eDisplayId)
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* default to analog master */
	pDefSettings->eMasterTg = (BVDC_DisplayTg)(eDisplayId % BVDC_DisplayTg_eTertIt);

	/* Heap NULL heap means using default main-heap.  If user would like
	 * to use a separate heap then this field should set accordingly */
	pDefSettings->hHeap = NULL;

	return BERR_SUCCESS;
}

/*************************************************************************
 *	BVDC_Display_Create
 *
 *	Creates a display handle. Assumes that it has not been created yet.
 *	Unlike the compositor handle, we do not know which display it is
 *	associated with until the user selects the video format.
 *
 *************************************************************************/
BERR_Code BVDC_Display_Create
	( BVDC_Compositor_Handle           hCompositor,
	  BVDC_Display_Handle             *phDisplay,
	  BVDC_DisplayId                   eDisplayId,
	  const BVDC_Display_Settings     *pDefSettings )
{
	BVDC_Display_Handle hDisplay;
	BVDC_DisplayId eId;
	uint32_t ulSrcId;
	BERR_Code err;


	BDBG_ENTER(BVDC_Display_Create);
	BDBG_ASSERT(phDisplay);
	BSTD_UNUSED(pDefSettings);

	/* Check internally if we created. */
	BDBG_OBJECT_ASSERT(hCompositor, BVDC_CMP);
	BDBG_OBJECT_ASSERT(hCompositor->hVdc, BVDC_VDC);

	/* determine display ID */
	if(BVDC_DisplayId_eAuto == eDisplayId)
	{
		eId = (BVDC_DisplayId)hCompositor->eId;
	}
	else
	{
		eId = eDisplayId;
	}

	hDisplay = hCompositor->hVdc->ahDisplay[eId];
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	/* Check if display is created or not. */
	if(BVDC_P_STATE_IS_ACTIVE(hDisplay) ||
	   BVDC_P_STATE_IS_CREATE(hDisplay) ||
	   BVDC_P_STATE_IS_DESTROY(hDisplay))
	{
		return BERR_TRACE(BVDC_ERR_DISPLAY_ALREADY_CREATED);
	}

	BDBG_MSG(("Creating display%d", hDisplay->eId));
	BDBG_ASSERT(BVDC_P_STATE_IS_INACTIVE(hDisplay));

#if (BVDC_P_SUPPORT_STG)
	BVDC_P_ResetStgChanInfo(&hDisplay->stStgChan);
#endif
	BVDC_P_ResetDviChanInfo(&hDisplay->stDviChan);
	BVDC_P_Reset656ChanInfo(&hDisplay->st656Chan);
	BVDC_P_ResetAnalogChanInfo(&hDisplay->stAnlgChan_0);
	BVDC_P_ResetAnalogChanInfo(&hDisplay->stAnlgChan_1);
	hDisplay->bAnlgEnable = false;
	hDisplay->stDviChan.bEnable = false;

	/* determine display channel and the master timing generator */
	if(pDefSettings)
	{
		/* 7420 has no restriction on which can be the master. Any of
		 * IT_0, IT_2, IT3_3, DVI_DTG, and 656_DTG can be the master.
		 */
		BDBG_ASSERT(pDefSettings->eMasterTg <= BVDC_DisplayTg_eStg3);

#if !BVDC_P_SUPPORT_VEC_MUXING
		if(!hDisplay->bIsBypass &&
		   ((BVDC_DisplayTg)eId != pDefSettings->eMasterTg))
		{
			return BERR_TRACE(BERR_INVALID_PARAMETER);
		}
#endif
		hDisplay->eMasterTg = pDefSettings->eMasterTg;

		/* Select specific flavor of 480P output */
		hDisplay->bArib480p = pDefSettings->bArib480p;
	}
	else
	{
		/* In case of auto, use one of the 3 analog master trigger */
		hDisplay->eMasterTg = (BVDC_DisplayTg)(eId % BVDC_DisplayTg_eTertIt);
		hDisplay->bArib480p = false;
	}

#if (BVDC_P_SUPPORT_IT_VER >= 3)
	/* HDMI always in slave mode */
	if(hDisplay->eMasterTg == BVDC_DisplayTg_eDviDtg)
	{
		BDBG_WRN(("HDMI can no longer run in master mode.  Use Analog master"));
		hDisplay->eMasterTg = (BVDC_DisplayTg)(eId % BVDC_DisplayTg_eTertIt);
		hDisplay->stDviChan.bEnable= true;
	}
#endif

	switch(hDisplay->eMasterTg)
	{
		case BVDC_DisplayTg_eDviDtg:
			hDisplay->stDviChan.bEnable= true;
			break;

#if BVDC_P_SUPPORT_ITU656_OUT
		case BVDC_DisplayTg_e656Dtg:
			hDisplay->st656Chan.bEnable = true;
			break;
#endif

		case BVDC_DisplayTg_ePrimIt:
		case BVDC_DisplayTg_eSecIt:
		case BVDC_DisplayTg_eTertIt:
			hDisplay->bAnlgEnable = true;
			break;

#if (BVDC_P_SUPPORT_STG)
		case BVDC_DisplayTg_eStg0:
		case BVDC_DisplayTg_eStg1:
		case BVDC_DisplayTg_eStg2:
		case BVDC_DisplayTg_eStg3:

		if((hDisplay->eMasterTg - BVDC_DisplayTg_eStg0) > BVDC_P_NUM_SHARED_STG)
		{
			BDBG_ERR(("System not supported STG_%d, %d available",
				hDisplay->eMasterTg - BVDC_DisplayTg_eStg0, BVDC_P_NUM_SHARED_STG));
			return BERR_TRACE(BERR_INVALID_PARAMETER);
		}

			hDisplay->stStgChan.bEnable = true;
			break;
#endif
		default:
			break;
	}

	/* Allocate modules for each enabled channel */

	/* Prepare source (compositor) ID */
	ulSrcId = (uint32_t) hCompositor->eId;

	/* determine bHdCap and bSecamCap flag here */
	hDisplay->bHdCap = (ulSrcId == BVDC_CompositorId_eCompositor0) ? true : false;
	hDisplay->bSecamCap = ((BVDC_P_NUM_SHARED_SECAM == 0) || (BVDC_P_NUM_SHARED_SECAM < BVDC_P_MAX_DISPLAY_COUNT && hDisplay->bHdCap)) ? false: true;

	BKNI_EnterCriticalSection();
	if (hDisplay->stDviChan.bEnable)
	{
		BDBG_MSG(("BVDC_Display_Create Display %d allocates resource for DVI", hDisplay->eId));
		err = BVDC_P_AllocDviChanResources(hCompositor->hVdc->hResource, eId, &hDisplay->stDviChan, ulSrcId);

		if (err)
		{
			BKNI_LeaveCriticalSection();
			BDBG_ERR(("Failed to create display %d for HDMI output. Short of DVI block. Check hardware capability.", eId));
			return err;
		}
	}

#if BVDC_P_SUPPORT_ITU656_OUT
	if (hDisplay->st656Chan.bEnable)
	{
		BDBG_MSG(("BVDC_Display_Create Display %d allocates resource for 656", hDisplay->eId));
		err = BVDC_P_Alloc656ChanResources(hCompositor->hVdc->hResource, eId, &hDisplay->st656Chan, ulSrcId);

		if (err)
		{
			BKNI_LeaveCriticalSection();
			BDBG_ERR(("Failed to create display %d for 656 output. Short of 656 block. Check hardware capability.", eId));
			return err;
		}
	}
#endif

#if (BVDC_P_SUPPORT_STG)
	if(hDisplay->stStgChan.bEnable)
	{
		BDBG_MSG(("BVDC_Display_Create Display %d allocates resource for STG", hDisplay->eId));
		err = BVDC_P_AllocStgChanResources(hCompositor->hVdc->hResource, eId, &hDisplay->stStgChan);

		if (err)
		{
			BKNI_LeaveCriticalSection();
			BDBG_ERR(("Failed to create display %d for %s output. Short of VEC hardware block. Check hardware capability.",
				      eId, hDisplay->bHdCap ? "component/VGA" : (hDisplay->bSecamCap ? "CVBS/S-Video/RF/SECAM" : "CVBS/S-Video/RF")));
			return err;
		}
	}
#endif

	if(hDisplay->bAnlgEnable)
	{
		BDBG_MSG(("BVDC_Display_Create Display %d allocates resource for AnlgChan_0", hDisplay->eId));
		err = BVDC_P_AllocITResources(hCompositor->hVdc->hResource, eId * 2, &hDisplay->stAnlgChan_0, BVDC_P_HW_ID_INVALID);

		if(err)
		{
			BKNI_LeaveCriticalSection();
			BDBG_ERR(("Failed to create display %d for %s output. Short of VEC hardware block. Check hardware capability.",
				      eId, hDisplay->bHdCap ? "component/VGA" : (hDisplay->bSecamCap ? "CVBS/S-Video/RF/SECAM" : "CVBS/S-Video/RF")));
			return err;
		}
	}

	BKNI_LeaveCriticalSection();

	/* Which heap to use? */
	hDisplay->hHeap = ((pDefSettings) && (pDefSettings->hHeap))
		? pDefSettings->hHeap : hCompositor->hVdc->hBufferHeap;

	/* link compsoitor and display */
	hDisplay->hCompositor       = hCompositor;
	hCompositor->hDisplay       = (BVDC_Display_Handle)hDisplay;

	/* Reinitialize context.  But not make it active until apply. */
	BVDC_P_Display_Init(hDisplay);

	/* Mark as create, awating for apply. */
	hDisplay->eState = BVDC_P_State_eCreate;
	*phDisplay = hDisplay;

	BDBG_LEAVE(BVDC_Display_Create);
	return BERR_SUCCESS;
}

/*************************************************************************
 *	BVDC_Display_Destroy
 *************************************************************************/
BERR_Code BVDC_Display_Destroy
	( BVDC_Display_Handle              hDisplay )
{

	BDBG_ENTER(BVDC_Display_Destroy);

	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if(BVDC_P_STATE_IS_DESTROY(hDisplay) ||
	   BVDC_P_STATE_IS_INACTIVE(hDisplay))
	{
		goto done;
	}

	if (BVDC_P_HW_ID_INVALID != hDisplay->stMpaaComp.ulHwId ||
		BVDC_P_HW_ID_INVALID != hDisplay->stMpaaHdmi.ulHwId)
	{
		BDBG_ERR(("Mpaa is still enabled with display ID %d", hDisplay->eId));
		return BERR_TRACE(BERR_LEAKED_RESOURCE);
	}

	if(BVDC_P_STATE_IS_ACTIVE(hDisplay))
	{
		hDisplay->eState = BVDC_P_State_eDestroy;
#if BVDC_P_SUPPORT_STG
		if(BVDC_P_DISPLAY_USED_STG(hDisplay->eMasterTg))
			hDisplay->stNewInfo.bEnableStg = false;
#endif
	}

	if(BVDC_P_STATE_IS_CREATE(hDisplay))
	{
		hDisplay->eState = BVDC_P_State_eInactive;
	}

done:
	BDBG_LEAVE(BVDC_Display_Destroy);
	return BERR_SUCCESS;
}

/*************************************************************************
 *
 */
BERR_Code BVDC_Display_GetDisplayId
	( const BVDC_Display_Handle        hDisplay,
	  BVDC_DisplayId                  *peDisplayId )
{
	BDBG_ENTER(BVDC_Display_GetDisplayId);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if(peDisplayId)
	{
		*peDisplayId = hDisplay->eId;
	}

	BDBG_LEAVE(BVDC_Display_GetDisplayId);
	return BERR_SUCCESS;
}


/*************************************************************************
 *	BVDC_Display_GetVbiPath
 *************************************************************************/
BERR_Code BVDC_Display_GetVbiPath
/* TODO: implement BVDC settings template logic */
	( const BVDC_Display_Handle        hDisplay,
	  BAVC_VbiPath                    *peVbiPath )
{
	BDBG_ENTER(BVDC_Display_GetVbiPath);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if(peVbiPath)
	{
		*peVbiPath = (BVDC_P_HW_ID_INVALID != hDisplay->stAnlgChan_0.ulIt)
			? hDisplay->stAnlgChan_0.ulIt : BAVC_VbiPath_eUnknown;
	}

	BDBG_LEAVE(BVDC_Display_GetVbiPath);
	return BERR_SUCCESS;
}

/*************************************************************************
 *	BVDC_Display_GetAnalogVbiPath
 *************************************************************************/
BERR_Code BVDC_Display_GetAnalogVbiPath
	( const BVDC_Display_Handle        hDisplay,
	  BAVC_VbiPath                    *peVbiPath )
{
	BDBG_ENTER(BVDC_Display_GetAnalogVbiPath);

	BDBG_LEAVE(BVDC_Display_GetAnalogVbiPath);

	BSTD_UNUSED(hDisplay);
	BSTD_UNUSED(peVbiPath);
	BDBG_ERR((" !!! Not supported feature!"));
	return BERR_SUCCESS;
}

/*************************************************************************
 *	BVDC_Display_SetCustomVideoFormat
 *
 *	Sets video format
 *************************************************************************/
BERR_Code BVDC_Display_SetCustomVideoFormat
	( BVDC_Display_Handle              hDisplay,
	  const BFMT_VideoInfo            *pFmtInfo )
{
	BERR_Code eStatus = BERR_SUCCESS;
	bool      bFmtChange = false;
	uint32_t  ulCheckSum = 0;

	BDBG_ENTER(BVDC_Display_SetCustomVideoFormat);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	/* Valid parameters */
	if((NULL == pFmtInfo) ||
	   (pFmtInfo->eVideoFmt >= BFMT_VideoFmt_eMaxCount))
	{
		BDBG_ERR(("Invalid parameter"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	if(pFmtInfo->eVideoFmt == BFMT_VideoFmt_eDVI_1280x768p_Red)
	{
		BDBG_ERR(("Format Not Supported"));
		return BERR_TRACE(BERR_NOT_SUPPORTED);
	}

	if(!BVDC_P_IsVidfmtSupported(pFmtInfo->eVideoFmt))
	{
		BDBG_ERR(("Format not supported: %s", pFmtInfo->pchFormatStr));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Update new video format */
	hDisplay->stNewInfo.pFmtInfo = pFmtInfo;
	if(BVDC_P_IS_CUSTOMFMT(pFmtInfo->eVideoFmt))
	{
		BVDC_P_SET_CUSTOMFMT(hDisplay->stNewInfo.pFmtInfo,
			&hDisplay->stNewInfo.stCustomFormatInfo, pFmtInfo);
	}

	hDisplay->stNewInfo.ulCheckSum = 0;
	bFmtChange = (hDisplay->stCurInfo.pFmtInfo != pFmtInfo);

	/* specificlly for custom format
	 * a simple check sum calculation*/
	if(!bFmtChange)
	{
		ulCheckSum  = pFmtInfo->ulWidth |
			pFmtInfo->ulDigitalWidth |
			pFmtInfo->ulHeight       |
			pFmtInfo->ulDigitalHeight|
			pFmtInfo->ulVertFreq     |
			pFmtInfo->eAspectRatio   |
			pFmtInfo->bInterlaced;

		bFmtChange = (hDisplay->stCurInfo.ulCheckSum != ulCheckSum);
		hDisplay->stCurInfo.ulCheckSum = ulCheckSum;
	}


#if BVDC_P_SUPPORT_OSCL
	/* Certain chipsets rely on OSCL (output sclaer) within compositor to
	 * to achieve 1080p output. When in this mode, VEC is running at 1080p, but
	 * the whole BVN is configured as if the output format is 1080i.
	 * We use OSCL to convert the interlaced picture to frame then render it
	 * to VEC.
	 */
	if ((BFMT_VideoFmt_e1080p == pFmtInfo->eVideoFmt) ||
		(BFMT_VideoFmt_e1080p_50Hz== pFmtInfo->eVideoFmt))
	{
		hDisplay->hCompositor->stNewInfo.pFmtInfo =
			BFMT_GetVideoFormatInfoPtr((BFMT_VideoFmt_e1080p == pFmtInfo->eVideoFmt) ?
				BFMT_VideoFmt_e1080i : BFMT_VideoFmt_e1080i_50Hz);
		hDisplay->hCompositor->stNewInfo.bEnableOScl = true;
		if((bFmtChange) || (hDisplay->stNewInfo.bErrorLastSetting))
		{
			hDisplay->hCompositor->stNewInfo.stDirty.stBits.bOScl = BVDC_P_DIRTY;
		}
		hDisplay->stNewInfo.ulTriggerModuloCnt = 2;
	}
	else
	{
		hDisplay->hCompositor->stNewInfo.pFmtInfo = pFmtInfo;
		hDisplay->hCompositor->stNewInfo.bEnableOScl = false;
		if((bFmtChange) || (hDisplay->stNewInfo.bErrorLastSetting))
		{
			hDisplay->hCompositor->stNewInfo.stDirty.stBits.bOScl = BVDC_P_DIRTY;
		}
		hDisplay->stNewInfo.ulTriggerModuloCnt = 1;
	}
#else
	hDisplay->hCompositor->stNewInfo.pFmtInfo = pFmtInfo;
#endif
	hDisplay->stNewInfo.ulVertFreq = pFmtInfo->ulVertFreq;
	hDisplay->stNewInfo.eAspectRatio = pFmtInfo->eAspectRatio;
	if (BVDC_P_DISPLAY_USED_DIGTRIG(hDisplay->eMasterTg))
		hDisplay->stNewInfo.ulHeight = pFmtInfo->ulDigitalHeight >> pFmtInfo->bInterlaced;
	else
		hDisplay->stNewInfo.ulHeight = pFmtInfo->ulHeight >> pFmtInfo->bInterlaced;

	if((bFmtChange) || (hDisplay->stNewInfo.bErrorLastSetting))
	{
		/* When setting a new video format, most stuffs like time base,
		 * DAC, etc need to be programmed.
		 */
		hDisplay->stNewInfo.stDirty.stBits.bTiming = BVDC_P_DIRTY;
		hDisplay->stNewInfo.stDirty.stBits.bTimeBase = BVDC_P_DIRTY;
		hDisplay->stNewInfo.stDirty.stBits.bDacSetting = BVDC_P_DIRTY;
		hDisplay->stNewInfo.stDirty.stBits.b3DSetting = BVDC_P_DIRTY;
		hDisplay->stNewInfo.stDirty.stBits.bAlignment = BVDC_P_DIRTY;
		hDisplay->stNewInfo.stDirty.stBits.bAspRatio = BVDC_P_DIRTY;
		if (hDisplay->bAnlgEnable)
		{
			hDisplay->stNewInfo.stDirty.stBits.bMpaaComp = BVDC_P_DIRTY;
		}
		if (hDisplay->stDviChan.bEnable)
		{
			hDisplay->stNewInfo.stDirty.stBits.bMpaaHdmi = BVDC_P_DIRTY;
		}
	}

	BDBG_LEAVE(BVDC_Display_SetCustomVideoFormat);
	return eStatus;
}

/*************************************************************************
 *	BVDC_Display_GetCustomVideoFormat
 *************************************************************************/
const BFMT_VideoInfo* BVDC_Display_GetCustomVideoFormat
	( const BVDC_Display_Handle        hDisplay )
{
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);
	return hDisplay->stCurInfo.pFmtInfo;
}

/*************************************************************************
 *	BVDC_Display_SetVideoFormat
 *
 *	Sets video format
 *************************************************************************/
BERR_Code BVDC_Display_SetVideoFormat
	( BVDC_Display_Handle              hDisplay,
	  BFMT_VideoFmt                    eVideoFormat )
{
	return BVDC_Display_SetCustomVideoFormat(hDisplay,
		BFMT_GetVideoFormatInfoPtr(eVideoFormat));
}

/*************************************************************************
 *	BVDC_Display_GetVideoFormat
 *************************************************************************/
BERR_Code BVDC_Display_GetVideoFormat
	( const BVDC_Display_Handle        hDisplay,
	  BFMT_VideoFmt                   *peVideoFormat )
{
	BDBG_ENTER(BVDC_Display_GetVideoFormat);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if(peVideoFormat)
	{
		*peVideoFormat = hDisplay->stCurInfo.pFmtInfo->eVideoFmt;
	}

	BDBG_LEAVE(BVDC_Display_GetVideoFormat);
	return BERR_SUCCESS;
}

/*************************************************************************
 * BVDC_Display_SetDacConfiguration
 *
 * Enable Dac(s) to specific output, or disable Dac(s). Use the following
 *  defines to identify specific Dacs:
 * BVDC_Dac_0
 * BVDC_Dac_1
 * BVDC_Dac_2
 * BVDC_Dac_3
 * BVDC_Dac_4
 * BVDC_Dac_5
 *
 *  A DAC can only be used by one display at a time. A display is not using
 *  a specifc DAC if its corresponding state is BVDC_DacOutput_eUnused.
 *  A DAC that is unused by all displays is disabled. Initially, all DACs
 *  are unused for all displays.
 *************************************************************************/
BERR_Code  BVDC_Display_SetDacConfiguration
	( BVDC_Display_Handle              hDisplay,
	  uint32_t                         ulDacs,
	  BVDC_DacOutput                   eDacOutput )
{
	BVDC_P_DisplayInfo *pNewInfo;
	BVDC_P_DisplayInfo *pCurInfo;
	uint32_t            i;

	BDBG_ENTER(BVDC_Display_SetDacConfiguration);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if(ulDacs > BVDC_Dac_0+BVDC_Dac_1+BVDC_Dac_2+BVDC_Dac_3+BVDC_Dac_4+BVDC_Dac_5+BVDC_Dac_6)
	{
		BDBG_ERR(("Invalid parameter"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	pNewInfo = &hDisplay->stNewInfo;
	pCurInfo = &hDisplay->stCurInfo;


	/* Other than VDEC pass-through, bypass path does not support Dac outputs */
	if(hDisplay->bIsBypass && ((eDacOutput != BVDC_DacOutput_eVdec0) && (eDacOutput != BVDC_DacOutput_eUnused)))
	{
		return BVDC_ERR_INVALID_DAC_SETTINGS;
	}


	/* Update new outputs for each Dac */
	for(i=0; i < BVDC_P_MAX_DACS; i++)
	{
		if((ulDacs >> i) & 1)
		{
			pNewInfo->aDacOutput[i] = eDacOutput;
			if(eDacOutput != pCurInfo->aDacOutput[i] || pNewInfo->bErrorLastSetting)
			{
				pNewInfo->stDirty.stBits.bDacSetting = BVDC_P_DIRTY;
			}
		}
	}

	if(!hDisplay->bAnlgEnable)
	{
		if (pNewInfo->stDirty.stBits.bDacSetting == BVDC_P_DIRTY)
		{
			/* when adding a slave analog path */
			pNewInfo->stDirty.stBits.bTiming = BVDC_P_DIRTY;
			pNewInfo->stDirty.stBits.bTimeBase = BVDC_P_DIRTY;
			pNewInfo->stDirty.stBits.b3DSetting = BVDC_P_DIRTY;
			pNewInfo->stDirty.stBits.bAlignment = BVDC_P_DIRTY;
		}
	}

	if (pNewInfo->stDirty.stBits.bDacSetting == BVDC_P_DIRTY &&
		pCurInfo->aulEnableMpaaDeci[BVDC_MpaaDeciIf_eComponent] &&
		(eDacOutput == BVDC_DacOutput_eRed   || eDacOutput == BVDC_DacOutput_eY  ||
		 eDacOutput == BVDC_DacOutput_eGreen || eDacOutput == BVDC_DacOutput_ePr ||
		 eDacOutput == BVDC_DacOutput_eBlue  || eDacOutput == BVDC_DacOutput_ePb ||
		 (eDacOutput == BVDC_DacOutput_eUnused &&
		  (hDisplay->stCurInfo.bRgb || hDisplay->stCurInfo.bYPrPb))))
	{
		pNewInfo->stDirty.stBits.bMpaaComp = BVDC_P_DIRTY;
	}

	BDBG_LEAVE(BVDC_Display_SetDacConfiguration);
	return BERR_SUCCESS;
}

/*************************************************************************
 *	BVDC_Display_GetDacConfiguration
 *
 *	Returns a specific Dac output setting.
 *************************************************************************/
BERR_Code BVDC_Display_GetDacConfiguration
	( const BVDC_Display_Handle        hDisplay,
	  uint32_t                         ulDac,
	  BVDC_DacOutput                  *peDacOutput )
{
	uint32_t     i;

	BDBG_ENTER(BVDC_Display_GetDacConfiguration);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if((!peDacOutput) || (ulDac > (BVDC_Dac_6 << 1)))
	{
		BDBG_ERR(("Invalid parameter"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Get index to Dac table */
	for (i=0; i < BVDC_P_MAX_DACS; i++)
	{
		if((ulDac >> i) & 1)
		{
			*peDacOutput = hDisplay->stCurInfo.aDacOutput[i];
			break;
		}
	}

	BDBG_LEAVE(BVDC_Display_GetDacConfiguration);
	return BERR_SUCCESS;
}


/*************************************************************************
 *	BVDC_Display_SetHdmiConfiguration
 *************************************************************************/
BERR_Code BVDC_Display_SetHdmiConfiguration
	( BVDC_Display_Handle              hDisplay,
	  uint32_t                         ulHdmi,
	  BAVC_MatrixCoefficients          eHdmiOutput )
{
	bool  bDirty;

	BDBG_ENTER(BVDC_Display_SetHdmiConfiguration);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	/* Check for valid parameters */
	if(ulHdmi != BVDC_Hdmi_0)
	{
		BDBG_ERR(("Display handle invalid"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	bDirty = ((hDisplay->stCurInfo.eHdmiOutput != eHdmiOutput) ||
			  (hDisplay->stNewInfo.bErrorLastSetting));
	switch(eHdmiOutput)
	{
	case BAVC_MatrixCoefficients_eUnknown:
		hDisplay->stNewInfo.bEnableHdmi = false;
		hDisplay->stNewInfo.eHdmiOutput = eHdmiOutput;
		if (bDirty)
		{
			hDisplay->stNewInfo.stDirty.stBits.bHdmiEnable = BVDC_P_DIRTY;
			if (hDisplay->stCurInfo.aulEnableMpaaDeci[BVDC_MpaaDeciIf_eHdmi])
			{
				hDisplay->stNewInfo.stDirty.stBits.bMpaaHdmi = BVDC_P_DIRTY;
			}
		}
		BDBG_MSG(("Display %d disables HDMI", hDisplay->eId));
		break;

	default:
		if (!hDisplay->stCurInfo.bEnableHdmi)
		{
			/* HDMI is not yet enabled */
			hDisplay->stNewInfo.bEnableHdmi = true;
			if (bDirty)
			{
				hDisplay->stNewInfo.stDirty.stBits.bHdmiEnable = BVDC_P_DIRTY;
			}
			hDisplay->stNewInfo.eHdmiOutput = eHdmiOutput;

#if (!BVDC_P_SUPPORT_SEAMLESS_ATTACH)
			/* Reset and reprogram master path as well */
			if (bDirty)
			{
				hDisplay->stNewInfo.stDirty.stBits.bTiming = BVDC_P_DIRTY;
				hDisplay->stNewInfo.stDirty.stBits.bTimeBase = BVDC_P_DIRTY;
				hDisplay->stNewInfo.stDirty.stBits.bDacSetting = BVDC_P_DIRTY;
				hDisplay->stNewInfo.stDirty.stBits.bMpaaHdmi = BVDC_P_DIRTY;
				if (hDisplay->stCurInfo.bRgb || hDisplay->stCurInfo.bYPrPb)
				{
					hDisplay->stNewInfo.stDirty.stBits.bMpaaComp = BVDC_P_DIRTY;
				}
			}
#else
			if (bDirty)
			{
				hDisplay->stNewInfo.stDirty.stBits.bMpaaHdmi = BVDC_P_DIRTY;
			}
#endif
			BDBG_MSG(("Display %d enables HDMI", hDisplay->eId));
		}

		hDisplay->stNewInfo.eHdmiOutput = eHdmiOutput;
		if (bDirty)
		{
			hDisplay->stNewInfo.stDirty.stBits.bHdmiCsc = BVDC_P_DIRTY;
		}

		break;
	}

	BDBG_LEAVE(BVDC_Display_SetHdmiConfiguration);
	return BERR_SUCCESS;
}

/*************************************************************************
 *	BVDC_Display_GetHdmiConfiguration
 *
 *	Query the HDMI output for a specific HDMI
 *************************************************************************/
BERR_Code BVDC_Display_GetHdmiConfiguration
	( const BVDC_Display_Handle        hDisplay,
	  uint32_t                         ulHdmi,
	  BAVC_MatrixCoefficients         *peHdmiOutput )
{
	BDBG_ENTER(BVDC_Display_GetHdmiConfiguration);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if(ulHdmi != BVDC_Hdmi_0)
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	if(peHdmiOutput)
	{
		*peHdmiOutput = hDisplay->stCurInfo.eHdmiOutput;
	}

	BDBG_LEAVE(BVDC_Display_GetHdmiConfiguration);
	return BERR_SUCCESS;
}

/*************************************************************************
 *	BVDC_Display_SetHdmiDropLines
 *
 *	Set the number of compositor video lines that Hdmi drops for a
 *  given format.
 *************************************************************************/
BERR_Code BVDC_Display_SetHdmiDropLines
	( const BVDC_Display_Handle        hDisplay,
	  uint32_t                         ulHdmi,
	  BFMT_VideoFmt                    eVideoFormat,
	  uint32_t                         ulHdmiDropLines )
{
	BDBG_ENTER(BVDC_Display_SetHdmiDropLines);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if(ulHdmi != BVDC_Hdmi_0)
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	if(!VIDEO_FORMAT_SUPPORTS_DROP_LINE(eVideoFormat))
	{
		BDBG_ERR(("Dropping compositor lines is supported for this format!"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	switch(eVideoFormat)
	{
	case BFMT_VideoFmt_e720x482_NTSC:
	case BFMT_VideoFmt_e720x482_NTSC_J:
		if (ulHdmiDropLines > BVDC_P_480i_DROP_LINES_MAX)
		{
		BDBG_ERR(("Exceeds number of dropped lines supported by NTSC, NTSCJ."));
			return BERR_TRACE(BERR_INVALID_PARAMETER);
		}

		if ((ulHdmiDropLines != 0) && (ulHdmiDropLines != 2))
		{
			BDBG_ERR(("NTSC, NTSCJ and PAL_M only support dropping of 0 or 2 lines."));
			return BERR_TRACE(BERR_INVALID_PARAMETER);
		}

		break;

	case BFMT_VideoFmt_e720x483p:
		if (ulHdmiDropLines > BVDC_P_480p_DROP_LINES_MAX)
		{
			BDBG_ERR(("Exceeds number of dropped lines supported by 480p."));
			return BERR_TRACE(BERR_INVALID_PARAMETER);
		}
		break;

	default:
		/* Assert if format isn't handled yet. */
		BDBG_ASSERT(false);
		break;
	}

	hDisplay->stNewInfo.aulHdmiDropLines[eVideoFormat] = ulHdmiDropLines;

	if((hDisplay->stCurInfo.aulHdmiDropLines[eVideoFormat] != ulHdmiDropLines) ||
	   (hDisplay->stNewInfo.bErrorLastSetting))
	{
		hDisplay->stNewInfo.stDirty.stBits.bHdmiDroplines = BVDC_P_DIRTY;

		/* DTRAM instructions will have to be reloaded.
		 */
		hDisplay->stNewInfo.stDirty.stBits.bTiming = BVDC_P_DIRTY;
		hDisplay->stNewInfo.stDirty.stBits.bTimeBase = BVDC_P_DIRTY;
		hDisplay->stNewInfo.stDirty.stBits.bDacSetting = BVDC_P_DIRTY;

		if (hDisplay->bAnlgEnable)
		{
			hDisplay->stNewInfo.stDirty.stBits.bMpaaComp = BVDC_P_DIRTY;
		}

		hDisplay->stNewInfo.stDirty.stBits.bMpaaHdmi = BVDC_P_DIRTY;
	}

	BDBG_LEAVE(BVDC_Display_SetHdmiDropLines);
	return BERR_SUCCESS;
}

/*************************************************************************
 *	BVDC_Display_GetHdmiDropLines
 *
 *	Get the number of compositor video lines that Hdmi drops for a given
 *  format.
 *************************************************************************/
BERR_Code BVDC_Display_GetHdmiDropLines
	( const BVDC_Display_Handle        hDisplay,
	  uint32_t                         ulHdmi,
	  BFMT_VideoFmt                    eVideoFormat,
	  uint32_t                        *pulHdmiDropLines )
{
	BDBG_ENTER(BVDC_Display_SetHdmiDropLines);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if(ulHdmi != BVDC_Hdmi_0)
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	if (pulHdmiDropLines)
	{
		*pulHdmiDropLines = hDisplay->stCurInfo.aulHdmiDropLines[eVideoFormat];
	}

	BDBG_LEAVE(BVDC_Display_SetHdmiDropLines);
	return BERR_SUCCESS;
}

/*************************************************************************
 *
 *************************************************************************/
BERR_Code BVDC_Display_SetHdmiSyncOnly
	( const BVDC_Display_Handle        hDisplay,
	  bool                             bSyncOnly )
{
	bool bCurSyncOnly;

	BVDC_Display_GetHdmiSyncOnly(hDisplay, &bCurSyncOnly);

	BDBG_ENTER(BVDC_Display_SetHdmiSyncOnly);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	/* only take effect if hdmi is enable */
	if(hDisplay->stNewInfo.bEnableHdmi == true)
	{
		BVDC_Display_SetMuteMode(hDisplay, BVDC_DisplayOutput_eDvo,
			                     bSyncOnly ? BVDC_MuteMode_eConst : BVDC_MuteMode_eDisable);
		if ((bCurSyncOnly != bSyncOnly) ||
			(hDisplay->stNewInfo.bErrorLastSetting))
		{
			hDisplay->stNewInfo.stDirty.stBits.bHdmiSyncOnly = BVDC_P_DIRTY;
		}
	}

	BDBG_LEAVE(BVDC_Display_SetHdmiSyncOnly);
	return BERR_SUCCESS;
}

/*************************************************************************
 *
 *************************************************************************/
BERR_Code BVDC_Display_GetHdmiSyncOnly
	( const BVDC_Display_Handle        hDisplay,
	  bool                            *pbSyncOnly )
{
	BVDC_MuteMode eMuteMode;

	BDBG_ENTER(BVDC_Display_GetHdmiSyncOnly);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if(pbSyncOnly)
	{
		BVDC_Display_GetMuteMode(hDisplay, BVDC_DisplayOutput_eDvo, &eMuteMode);
		*pbSyncOnly = (eMuteMode == BVDC_MuteMode_eConst) ? true : false;
	}

	BDBG_LEAVE(BVDC_Display_GetHdmiSyncOnly);
	return BERR_SUCCESS;
}

/*************************************************************************
 *
 *************************************************************************/
BERR_Code BVDC_Display_SetHdmiXvYcc
	( const BVDC_Display_Handle        hDisplay,
	  bool                             bXvYcc )
{
	BDBG_ENTER(BVDC_Display_SetHdmiXvYcc);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	hDisplay->stNewInfo.bXvYcc = bXvYcc;
	if((hDisplay->stCurInfo.bXvYcc != bXvYcc) ||
	   (hDisplay->stNewInfo.bErrorLastSetting))
	{
		hDisplay->stNewInfo.stDirty.stBits.bHdmiXvYcc = BVDC_P_DIRTY;
	}

	BDBG_LEAVE(BVDC_Display_SetHdmiXvYcc);
	return BERR_SUCCESS;
}

/*************************************************************************
 *
 *************************************************************************/
BERR_Code BVDC_Display_GetHdmiXvYcc
	( const BVDC_Display_Handle        hDisplay,
	  bool                            *pbXvYcc )
{
	BDBG_ENTER(BVDC_Display_GetHdmiXvYcc);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if(pbXvYcc)
	{
		*pbXvYcc = hDisplay->stCurInfo.bXvYcc;
	}

	BDBG_LEAVE(BVDC_Display_GetHdmiXvYcc);
	return BERR_SUCCESS;
}

/*************************************************************************
 *
 *************************************************************************/
BERR_Code BVDC_Display_SetHdmiColorDepth
	( const BVDC_Display_Handle        hDisplay,
	  BAVC_HDMI_BitsPerPixel           eColorDepth )
{
	BDBG_ENTER(BVDC_Display_SetHdmiColorDepth);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	/* Current HW only supports up to 36-bit color depth */
	if(eColorDepth > BAVC_HDMI_BitsPerPixel_e36bit)
	{
		return BERR_TRACE(BVDC_ERR_INVALID_HDMI_MODE);
	}

	hDisplay->stNewInfo.eHdmiColorDepth = eColorDepth;
	if((hDisplay->stCurInfo.eHdmiColorDepth != eColorDepth) ||
	   (hDisplay->stNewInfo.bErrorLastSetting))
	{
		hDisplay->stNewInfo.stDirty.stBits.bHdmiRmChanged = BVDC_P_DIRTY;
	}

	BDBG_LEAVE(BVDC_Display_SetHdmiColorDepth);
	return BERR_SUCCESS;
}

/*************************************************************************
 *
 *************************************************************************/
BERR_Code BVDC_Display_GetHdmiColorDepth
	( const BVDC_Display_Handle        hDisplay,
	  BAVC_HDMI_BitsPerPixel          *eColorDepth )
{
	BDBG_ENTER(BVDC_Display_GetHdmiColorDepth);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if(eColorDepth)
	{
		*eColorDepth = hDisplay->stCurInfo.eHdmiColorDepth;
	}

	BDBG_LEAVE(BVDC_Display_GetHdmiColorDepth);
	return BERR_SUCCESS;
}

/*************************************************************************
 *
 *************************************************************************/
BERR_Code BVDC_Display_SetHdmiPixelRepetition
	( const BVDC_Display_Handle        hDisplay,
	  BAVC_HDMI_PixelRepetition        ePixelRepetition )
{
	BDBG_ENTER(BVDC_Display_SetHdmiPixelRepetition);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	/* Current HW only supports none, 2x and 4x pixel repetition */
	if(ePixelRepetition != BAVC_HDMI_PixelRepetition_eNone &&
	   ePixelRepetition != BAVC_HDMI_PixelRepetition_e2x &&
	   ePixelRepetition != BAVC_HDMI_PixelRepetition_e4x)
	{
		return BERR_TRACE(BVDC_ERR_INVALID_HDMI_MODE);
	}

	hDisplay->stNewInfo.eHdmiPixelRepetition = ePixelRepetition;
	if((hDisplay->stCurInfo.eHdmiPixelRepetition != ePixelRepetition) ||
	   (hDisplay->stNewInfo.bErrorLastSetting))
	{
		hDisplay->stNewInfo.stDirty.stBits.bHdmiRmChanged = BVDC_P_DIRTY;
		if (VIDEO_FORMAT_IS_ED(hDisplay->stNewInfo.pFmtInfo->eVideoFmt))
		{
			/* DTRAM microcode may have to be reloaded */
			hDisplay->stNewInfo.stDirty.stBits.bTiming = BVDC_P_DIRTY;
			hDisplay->stNewInfo.stDirty.stBits.bTimeBase = BVDC_P_DIRTY;
			hDisplay->stNewInfo.stDirty.stBits.bDacSetting = BVDC_P_DIRTY;
		}
	}

	BDBG_LEAVE(BVDC_Display_SetHdmiPixelRepetition);
	return BERR_SUCCESS;
}

/*************************************************************************
 *
 *************************************************************************/
BERR_Code BVDC_Display_GetHdmiPixelRepetition
	( const BVDC_Display_Handle        hDisplay,
	  BAVC_HDMI_PixelRepetition       *ePixelRepetition )
{
	BDBG_ENTER(BVDC_Display_GetHdmiPixelRepetition);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if(ePixelRepetition)
	{
		*ePixelRepetition = hDisplay->stCurInfo.eHdmiPixelRepetition;
	}

	BDBG_LEAVE(BVDC_Display_GetHdmiPixelRepetition);
	return BERR_SUCCESS;
}

/*************************************************************************
 *	BVDC_Display_Set656Configuration
 *************************************************************************/
BERR_Code BVDC_Display_Set656Configuration
	( BVDC_Display_Handle              hDisplay,
	  uint32_t                         ul656Output,
	  bool                             bEnable )
{
	BDBG_ENTER(BVDC_Display_Set656Configuration);

#if (BVDC_P_SUPPORT_ITU656_OUT == 0)
	BSTD_UNUSED(hDisplay);
	BSTD_UNUSED(ul656Output);
	BSTD_UNUSED(bEnable);

	BDBG_ERR(("This chip does not support 656 output."));

	BDBG_LEAVE(BVDC_Display_Set656Configuration);

	return BERR_NOT_SUPPORTED;
#else

	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	/* Check for valid parameters */
	if(ul656Output != BVDC_Itur656Output_0)
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	hDisplay->stNewInfo.bEnable656 = bEnable;
	if((hDisplay->stCurInfo.bEnable656 != bEnable) ||
	   (hDisplay->stNewInfo.bErrorLastSetting))
	{
		hDisplay->stNewInfo.stDirty.stBits.b656Enable = BVDC_P_DIRTY;

#if (!BVDC_P_SUPPORT_SEAMLESS_ATTACH)
		if (hDisplay->stNewInfo.bEnable656)
		{
			/* Reset and reprogram master path as well */
			hDisplay->stNewInfo.stDirty.stBits.bTiming = BVDC_P_DIRTY;
			hDisplay->stNewInfo.stDirty.stBits.bTimeBase = BVDC_P_DIRTY;
			hDisplay->stNewInfo.stDirty.stBits.bDacSetting = BVDC_P_DIRTY;

			if (hDisplay->bAnlgEnable)
			{
				hDisplay->stNewInfo.stDirty.stBits.bMpaaComp = BVDC_P_DIRTY;
			}

			if (hDisplay->stDviChan.bEnable)
			{
				hDisplay->stNewInfo.stDirty.stBits.bMpaaHdmi = BVDC_P_DIRTY;
			}
		}
#endif
	}

	BDBG_LEAVE(BVDC_Display_Set656Configuration);

	return BERR_SUCCESS;
#endif
}

/*************************************************************************
 *	BVDC_Display_Get656Configuration
 *************************************************************************/
BERR_Code BVDC_Display_Get656Configuration
	( const BVDC_Display_Handle        hDisplay,
	  uint32_t                         ul656Output,
	  bool                            *pbEnable656Output )
{
	BDBG_ENTER(BVDC_Display_Get656Configuration);

#if (BVDC_P_SUPPORT_ITU656_OUT == 0)
	BSTD_UNUSED(hDisplay);
	BSTD_UNUSED(ul656Output);
	BSTD_UNUSED(pbEnable656Output);

	BDBG_ERR(("This chip does not support 656 output."));

	BDBG_LEAVE(BVDC_Display_Get656Configuration);
	return BERR_NOT_SUPPORTED;

#else
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if(ul656Output != BVDC_Itur656Output_0)
	{
		BDBG_ERR(("Invalid parameter"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	if(pbEnable656Output)
	{
		*pbEnable656Output = hDisplay->stCurInfo.bEnable656;
	}

	BDBG_LEAVE(BVDC_Display_Get656Configuration);
	return BERR_SUCCESS;
#endif
}

/*************************************************************************
 *	BVDC_Display_SetStgConfiguration
 *************************************************************************/
BERR_Code BVDC_Display_SetStgConfiguration
	( BVDC_Display_Handle              hDisplay,
	  bool                             bEnable,
	  const BVDC_Display_StgSettings  *pStgSettings )
{
	BDBG_ENTER(BVDC_Display_SetStgConfiguration);

#if (BVDC_P_SUPPORT_STG == 0)
	BSTD_UNUSED(hDisplay);
	BSTD_UNUSED(bEnable);
	BSTD_UNUSED(pStgSettings);

	BDBG_ERR(("This chip does not support STG"));

	BDBG_LEAVE(BVDC_Display_SetStgConfiguration);

	return BERR_NOT_SUPPORTED;
#else
	/* Check for valid parameters */
	if(hDisplay ==NULL)
	{
		BDBG_LEAVE(BVDC_Display_SetStgConfiguration);
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* @@@ About to remove condition for 7425 b0 slave mode removal*/
	if(BVDC_P_DISPLAY_USED_STG(hDisplay->eMasterTg))
	{
		if(false == bEnable)
		{
			BDBG_ERR(("STG Master Mode cannot turn off trigger!!!"));
			BDBG_LEAVE(BVDC_Display_SetStgConfiguration);
			return BERR_TRACE(BERR_INVALID_PARAMETER);
		}
	}

	hDisplay->stNewInfo.bEnableStg =
		(bEnable || BVDC_P_DISPLAY_USED_STG(hDisplay->eMasterTg));

	if(pStgSettings)
	{
		if(pStgSettings->bNonRealTime &&
			(!BVDC_P_DISPLAY_USED_STG(hDisplay->eMasterTg)))
		{
			BDBG_ERR(("Display %d eMasterTg=%d is not STG, cannot be non-realtime!",
				hDisplay->eId, hDisplay->eMasterTg));
			BDBG_LEAVE(BVDC_Display_SetStgConfiguration);
			return BERR_TRACE(BERR_INVALID_PARAMETER);
		}

		hDisplay->stNewInfo.bStgNonRealTime = pStgSettings->bNonRealTime;

		if((bEnable != hDisplay->stCurInfo.bEnableStg) ||
		   (pStgSettings->bNonRealTime != hDisplay->stCurInfo.bStgNonRealTime) ||
		   (hDisplay->stNewInfo.bErrorLastSetting))
		{
			hDisplay->stNewInfo.stDirty.stBits.bStgEnable = BVDC_P_DIRTY;

			if (hDisplay->stNewInfo.bEnableStg)
			{
				/* Reset and reprogram master path as well */
				hDisplay->stNewInfo.stDirty.stBits.bTiming = BVDC_P_DIRTY;
				hDisplay->stNewInfo.stDirty.stBits.bTimeBase = BVDC_P_DIRTY;
				hDisplay->stNewInfo.stDirty.stBits.bDacSetting = BVDC_P_DIRTY;
			}
		}
	}

	BDBG_LEAVE(BVDC_Display_SetStgConfiguration);

	return BERR_SUCCESS;
#endif
}

/*************************************************************************
 *	BVDC_Display_GetStgConfiguration
 *************************************************************************/
BERR_Code BVDC_Display_GetStgConfiguration
	( const BVDC_Display_Handle        hDisplay,
	  bool                            *pbEnable,
	  BVDC_Display_StgSettings        *pStgSettings )
{
	BDBG_ENTER(BVDC_Display_GetStgConfiguration);

#if (BVDC_P_SUPPORT_STG == 0)
	BSTD_UNUSED(hDisplay);
	BSTD_UNUSED(pbEnable);
	BSTD_UNUSED(pStgSettings);

	BDBG_LEAVE(BVDC_Display_GetStgConfiguration);
	return BERR_NOT_SUPPORTED;

#else

	if(hDisplay == NULL)
	{
		BDBG_ERR(("Invalid parameter"));
		BDBG_LEAVE(BVDC_Display_GetStgConfiguration);
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	if(pbEnable)
	{
		*pbEnable = hDisplay->stCurInfo.bEnableStg;
	}

	if(pStgSettings)
	{
		pStgSettings->bNonRealTime = hDisplay->stCurInfo.bStgNonRealTime;
	}

	BDBG_LEAVE(BVDC_Display_GetStgConfiguration);
	return BERR_SUCCESS;
#endif
}


/*************************************************************************
 *	BVDC_Display_GetInterruptName
 *************************************************************************/
BERR_Code BVDC_Display_GetInterruptName
	( BVDC_Display_Handle             hDisplay,
	  const BAVC_Polarity             eFieldId,
	  BINT_Id                        *pInterruptName )
{
	BDBG_ENTER(BVDC_Display_GetInterruptName);

	if (!hDisplay)
	{
		BDBG_ERR(("Invalid parameter"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	BDBG_OBJECT_ASSERT(hDisplay->hCompositor, BVDC_CMP);

	if(pInterruptName)
	{
		*pInterruptName = BRDC_Slot_GetIntId(
			hDisplay->hCompositor->ahSlot[eFieldId == BAVC_Polarity_eBotField]);
	}

	BDBG_LEAVE(BVDC_Display_GetInterruptName);
	return BERR_SUCCESS;
}

/*************************************************************************
 *	BVDC_Display_SetRfmConfiguration
 *************************************************************************/
BERR_Code BVDC_Display_SetRfmConfiguration
	( BVDC_Display_Handle              hDisplay,
	  uint32_t                         ulRfmOutput,
	  BVDC_RfmOutput                   eRfmOutput,
	  uint32_t                         ulConstantValue )
{
	BDBG_ENTER(BVDC_Display_SetRfmConfiguration);

#if (BVDC_P_SUPPORT_RFM_OUTPUT == 0)
	BSTD_UNUSED(hDisplay);
	BSTD_UNUSED(ulRfmOutput);
	BSTD_UNUSED(eRfmOutput);
	BSTD_UNUSED(ulConstantValue);

	BDBG_ERR(("This chip does not support RF output."));
	BDBG_LEAVE(BVDC_Display_SetRfmConfiguration);
	return BERR_NOT_SUPPORTED;
#else
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if(ulRfmOutput != BVDC_Rfm_0)
	{
		BDBG_ERR(("Invalid parameter"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Bypass path does not support Rfm */
	if(hDisplay->bIsBypass)
	{
		return BERR_TRACE(BVDC_ERR_INVALID_RFM_PATH);
	}

	hDisplay->stNewInfo.eRfmOutput = eRfmOutput;
	hDisplay->stNewInfo.ulRfmConst =
		(eRfmOutput == BVDC_RfmOutput_eUnused) ? 0 : ulConstantValue;
	if((hDisplay->stCurInfo.eRfmOutput != eRfmOutput) ||
	   (hDisplay->stCurInfo.ulRfmConst != ulConstantValue) ||
	   (hDisplay->stNewInfo.bErrorLastSetting))
	{
		hDisplay->stNewInfo.stDirty.stBits.bRfm = BVDC_P_DIRTY;
		hDisplay->stNewInfo.stDirty.stBits.bDacSetting = BVDC_P_DIRTY;
	}

	BDBG_LEAVE(BVDC_Display_SetRfmConfiguration);
	return BERR_SUCCESS;
#endif
}

/*************************************************************************
 *	BVDC_Display_GetRfmConfiguration
 *************************************************************************/
BERR_Code BVDC_Display_GetRfmConfiguration
	( const BVDC_Display_Handle        hDisplay,
	  uint32_t                         ulRfmOutput,
	  BVDC_RfmOutput                  *peRfmOutput,
	  uint32_t                        *pulConstantValue )
{
	BDBG_ENTER(BVDC_Display_GetRfmConfiguration);

#if (BVDC_P_SUPPORT_RFM_OUTPUT == 0)
	BSTD_UNUSED(hDisplay);
	BSTD_UNUSED(ulRfmOutput);
	BSTD_UNUSED(peRfmOutput);
	BSTD_UNUSED(pulConstantValue);

	BDBG_ERR(("This chip does not support RF output."));

	BDBG_LEAVE(BVDC_Display_GetRfmConfiguration);
	return BERR_NOT_SUPPORTED;
#else
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if(ulRfmOutput != BVDC_Rfm_0)
	{
		BDBG_ERR(("Invalid parameter"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	if(peRfmOutput)
	{
		*peRfmOutput = hDisplay->stCurInfo.eRfmOutput;
	}

	if(pulConstantValue)
	{
		*pulConstantValue = hDisplay->stCurInfo.ulRfmConst;
	}

	BDBG_LEAVE(BVDC_Display_GetRfmConfiguration);
	return BERR_SUCCESS;
#endif
}

/*************************************************************************
 *	BVDC_Display_SetMpaa
 *************************************************************************/
BERR_Code BVDC_Display_SetMpaaDecimation
	( BVDC_Display_Handle              hDisplay,
	  BVDC_MpaaDeciIf                  eMpaaDeciIf,
	  uint32_t                         ulOutPorts,
	  bool                             bEnable )
{
	BDBG_ENTER(BVDC_Display_SetMpaaDecimation);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	/* Bypass path does not support MPAA */
	if(hDisplay->bIsBypass)
	{
		return BERR_TRACE(BVDC_ERR_INVALID_MPAA_PATH);
	}

	if ((BVDC_MpaaDeciIf_eUnused <= eMpaaDeciIf) ||
		((~hDisplay->aulMpaaDeciIfPortMask[eMpaaDeciIf]) & ulOutPorts))
	{
		BDBG_ERR(("Invalid parameter"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	if (bEnable)
	{
		hDisplay->stNewInfo.aulEnableMpaaDeci[eMpaaDeciIf] |= ulOutPorts;
	}
	else
	{
		hDisplay->stNewInfo.aulEnableMpaaDeci[eMpaaDeciIf] &= ~ulOutPorts;
	}

	if((hDisplay->stCurInfo.aulEnableMpaaDeci[eMpaaDeciIf] != hDisplay->stNewInfo.aulEnableMpaaDeci[eMpaaDeciIf]) ||
	   (hDisplay->stNewInfo.bErrorLastSetting))
	{
		if (BVDC_MpaaDeciIf_eComponent == eMpaaDeciIf)
			hDisplay->stNewInfo.stDirty.stBits.bMpaaComp = BVDC_P_DIRTY;
		else /* BVDC_MpaaDeciIf_eHdmi == eMpaaDeciIf */
			hDisplay->stNewInfo.stDirty.stBits.bMpaaHdmi = BVDC_P_DIRTY;
	}

	BDBG_LEAVE(BVDC_Display_SetMpaaDecimation);
	return BERR_SUCCESS;
}

/*************************************************************************
 *	BVDC_Display_GetMpaaDecimation
 *************************************************************************/
BERR_Code BVDC_Display_GetMpaaDecimation
	( const BVDC_Display_Handle        hDisplay,
	  BVDC_MpaaDeciIf                  eMpaaDeciIf,
	  uint32_t                         ulOutPort,
	  bool                            *pbEnable )
{
	BDBG_ENTER(BVDC_Display_GetMpaaDecimation);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if ((BVDC_MpaaDeciIf_eUnused <= eMpaaDeciIf) ||
		((~hDisplay->aulMpaaDeciIfPortMask[eMpaaDeciIf]) & ulOutPort) ||
		(NULL == pbEnable))
	{
		BDBG_ERR(("Invalid parameter"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	*pbEnable = hDisplay->stCurInfo.aulEnableMpaaDeci[eMpaaDeciIf] & ulOutPort;

	BDBG_LEAVE(BVDC_Display_GetMpaaDecimation);
	return BERR_SUCCESS;
}

/*************************************************************************
 *	BVDC_Display_SetAspectRatio
 *************************************************************************/
BERR_Code BVDC_Display_SetAspectRatio
	( BVDC_Display_Handle              hDisplay,
	  BFMT_AspectRatio                 eAspectRatio )
{
	BDBG_ENTER(BVDC_Display_SetAspectRatio);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);
	BDBG_OBJECT_ASSERT(hDisplay->hCompositor, BVDC_CMP);

	if(BVDC_P_IS_UNKNOWN_ASPR(eAspectRatio, 0, 0))
	{
		BDBG_WRN(("Invalid aspect ratio: %u", eAspectRatio));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	hDisplay->stNewInfo.eAspectRatio = eAspectRatio;
	if((hDisplay->stCurInfo.eAspectRatio != eAspectRatio) ||
	   (hDisplay->stNewInfo.bErrorLastSetting))
	{
		hDisplay->stNewInfo.stDirty.stBits.bAspRatio = BVDC_P_DIRTY;
	}

	BDBG_LEAVE(BVDC_Display_SetAspectRatio);
	return BERR_SUCCESS;
}

/*************************************************************************
 *	BVDC_Display_SetSampleAspectRatio
 *************************************************************************/
BERR_Code BVDC_Display_GetSampleAspectRatio
	( BVDC_Display_Handle              hDisplay,
	  uint16_t                        *puiSampleAspectRatioX,
	  uint16_t                        *puiSampleAspectRatioY )
{
	BDBG_ENTER(BVDC_Display_GetSampleAspectRatio);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);
	BDBG_OBJECT_ASSERT(hDisplay->hCompositor, BVDC_CMP);

	if(puiSampleAspectRatioX)
	{
		*puiSampleAspectRatioX = hDisplay->stCurInfo.uiSampleAspectRatioX;
	}

	if(puiSampleAspectRatioY)
	{
		*puiSampleAspectRatioY = hDisplay->stCurInfo.uiSampleAspectRatioY;
	}

	BDBG_LEAVE(BVDC_Display_GetSampleAspectRatio);
	return BERR_SUCCESS;
}

/*************************************************************************
 *	BVDC_Display_SetSampleAspectRatio
 *************************************************************************/
BERR_Code BVDC_Display_SetSampleAspectRatio
	( BVDC_Display_Handle              hDisplay,
	  uint16_t                         uiSampleAspectRatioX,
	  uint16_t                         uiSampleAspectRatioY )
{
	BDBG_ENTER(BVDC_Display_SetSampleAspectRatio);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);
	BDBG_OBJECT_ASSERT(hDisplay->hCompositor, BVDC_CMP);

	if(BVDC_P_IS_UNKNOWN_ASPR(BFMT_AspectRatio_eSAR,
	   uiSampleAspectRatioX, uiSampleAspectRatioY))
	{
		BDBG_WRN(("Invalid sample aspect ratio: %u %u",
			uiSampleAspectRatioX, uiSampleAspectRatioY));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	hDisplay->stNewInfo.eAspectRatio = BFMT_AspectRatio_eSAR;
	hDisplay->stNewInfo.uiSampleAspectRatioX = uiSampleAspectRatioX;
	hDisplay->stNewInfo.uiSampleAspectRatioY = uiSampleAspectRatioY;
	if ((hDisplay->stCurInfo.eAspectRatio != BFMT_AspectRatio_eSAR) ||
	    (hDisplay->stCurInfo.uiSampleAspectRatioX != uiSampleAspectRatioX) ||
	    (hDisplay->stCurInfo.uiSampleAspectRatioY != uiSampleAspectRatioY) ||
	    (hDisplay->stNewInfo.bErrorLastSetting))
	{
		hDisplay->stNewInfo.stDirty.stBits.bAspRatio = BVDC_P_DIRTY;
	}

	BDBG_LEAVE(BVDC_Display_SetSampleAspectRatio);
	return BERR_SUCCESS;
}


/*************************************************************************
 *	BVDC_Display_SetAspectRatioRectangle
 *************************************************************************/
BERR_Code BVDC_Display_SetAspectRatioCanvasClip
	( BVDC_Display_Handle              hDisplay,
	  uint32_t                         ulLeft,
	  uint32_t                         ulRight,
	  uint32_t                         ulTop,
	  uint32_t                         ulBottom )
{
	BDBG_ENTER(BVDC_Display_SetAspectRatioCanvasClip);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);
	BDBG_OBJECT_ASSERT(hDisplay->hCompositor, BVDC_CMP);

	hDisplay->stNewInfo.stAspRatRectClip.ulLeft   = ulLeft;
	hDisplay->stNewInfo.stAspRatRectClip.ulRight  = ulRight;
	hDisplay->stNewInfo.stAspRatRectClip.ulTop    = ulTop;
	hDisplay->stNewInfo.stAspRatRectClip.ulBottom = ulBottom;
	if ((hDisplay->stCurInfo.stAspRatRectClip.ulLeft   != ulLeft) ||
	    (hDisplay->stCurInfo.stAspRatRectClip.ulRight  != ulRight) ||
	    (hDisplay->stCurInfo.stAspRatRectClip.ulTop    != ulTop) ||
	    (hDisplay->stCurInfo.stAspRatRectClip.ulBottom != ulBottom) ||
	    (hDisplay->stNewInfo.bErrorLastSetting))
	{
		hDisplay->stNewInfo.stDirty.stBits.bAspRatio = BVDC_P_DIRTY;
	}

	BDBG_LEAVE(BVDC_Display_SetAspectRatioCanvasClip);
	return BERR_SUCCESS;
}

/*************************************************************************
 *	BVDC_Display_GetAspectRatio
 *************************************************************************/
BERR_Code BVDC_Display_GetAspectRatio
	( const BVDC_Display_Handle        hDisplay,
	  BFMT_AspectRatio                *peAspectRatio )
{
	BDBG_ENTER(BVDC_Display_GetAspectRatio);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);
	BDBG_OBJECT_ASSERT(hDisplay->hCompositor, BVDC_CMP);

	if(peAspectRatio)
	{
		*peAspectRatio = hDisplay->stCurInfo.eAspectRatio;
	}

	BDBG_LEAVE(BVDC_Display_GetAspectRatio);
	return BERR_SUCCESS;
}

/*************************************************************************
 *	BVDC_Display_SetForceFrameDrop
 *************************************************************************/
BERR_Code BVDC_Display_SetForceFrameDrop
	( BVDC_Display_Handle              hDisplay,
	  bool                             bForceFrameDrop )
{
	BDBG_ENTER(BVDC_Display_SetForceFrameDrop);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	hDisplay->stNewInfo.bForceFrameDrop = bForceFrameDrop;
	if((hDisplay->stCurInfo.bForceFrameDrop != bForceFrameDrop) ||
	   (hDisplay->stNewInfo.bErrorLastSetting))
	{
		hDisplay->stNewInfo.stDirty.stBits.bFrameDrop = BVDC_P_DIRTY;
	}

	BDBG_LEAVE(BVDC_Display_SetForceFrameDrop);
	return BERR_SUCCESS;
}

/*************************************************************************
 *	BVDC_Display_GetForceFrameDrop
 *************************************************************************/
BERR_Code BVDC_Display_GetForceFrameDrop
	( BVDC_Display_Handle              hDisplay,
	  bool                            *pbForceFrameDrop )
{
	BDBG_ENTER(BVDC_Display_GetForceFrameDrop);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if(pbForceFrameDrop)
	{
		*pbForceFrameDrop = hDisplay->stCurInfo.bForceFrameDrop;
	}

	BDBG_LEAVE(BVDC_Display_GetForceFrameDrop);
	return BERR_SUCCESS;
}

/*************************************************************************
 *	BVDC_Display_SetTimebase
 *************************************************************************/
BERR_Code BVDC_Display_SetTimebase
	( BVDC_Display_Handle              hDisplay,
	  BAVC_Timebase                    eTimeBase )
{
	BDBG_ENTER(BVDC_Display_SetTimebase);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	/* Note: which timebase the bypass video 656 output is locked to is
	 * out of control of VDC PI; it's up to the upper level software to
	 * determine what timebase to use for vec 656 output clock by
	 * programming CLK_MISC register and VCXO_x_RM block; */
	if(hDisplay->bIsBypass)
	{
		BDBG_WRN(("Application needs to program the timebase for bypass video"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	if(eTimeBase > BAVC_Timebase_e3)
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	hDisplay->stNewInfo.eTimeBase = eTimeBase;
	if((hDisplay->stCurInfo.eTimeBase != eTimeBase) ||
	   (hDisplay->stNewInfo.bErrorLastSetting))
	{
		hDisplay->stNewInfo.stDirty.stBits.bTimeBase = BVDC_P_DIRTY;
	}

	BDBG_LEAVE(BVDC_Display_SetTimebase);
	return BERR_SUCCESS;
}

/*************************************************************************
 *	BVDC_Display_GetTimebase
 *************************************************************************/
BERR_Code BVDC_Display_GetTimebase
	( const BVDC_Display_Handle        hDisplay,
	  BAVC_Timebase                   *peTimeBase )
{
	BDBG_ENTER(BVDC_Display_GetTimebase);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	/* Note: which timebase the bypass video 656 output is locked to is
	 * out of control of VDC PI; it's up to the upper level software to
	 * determine what timebase to use for vec 656 output clock by
	 * programming CLK_MISC register and VCXO_x_RM block; */
	if(hDisplay->bIsBypass)
	{
		BDBG_ERR(("Application needs to program the timebase for bypass video"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	if(peTimeBase)
	{
		*peTimeBase = hDisplay->stCurInfo.eTimeBase;
	}

	BDBG_LEAVE(BVDC_Display_GetTimebase);
	return BERR_SUCCESS;
}

/*************************************************************************
 *	BVDC_Display_SetCallbackSettings
 *************************************************************************/
BERR_Code BVDC_Display_SetCallbackSettings
	( BVDC_Display_Handle                  hDisplay,
	  const BVDC_Display_CallbackSettings *pSettings )
{
	BDBG_ENTER(BVDC_Display_SetCallbackSettings);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if(!pSettings)
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	hDisplay->stNewInfo.stCallbackSettings.stMask = pSettings->stMask;
	if((hDisplay->stCurInfo.stCallbackSettings.stMask.bCrc        != pSettings->stMask.bCrc) ||
	   (hDisplay->stCurInfo.stCallbackSettings.stMask.bRateChange != pSettings->stMask.bRateChange) ||
	   (hDisplay->stCurInfo.stCallbackSettings.stMask.bPerVsync   != pSettings->stMask.bPerVsync) ||
	   (hDisplay->stCurInfo.stCallbackSettings.stMask.bStgPictureId!= pSettings->stMask.bStgPictureId) ||
	   (hDisplay->stNewInfo.bErrorLastSetting))
	{
		hDisplay->stNewInfo.stDirty.stBits.bCallback = BVDC_P_DIRTY;
	}

	BDBG_LEAVE(BVDC_Display_SetCallbackSettings);
	return BERR_SUCCESS;
}

/*************************************************************************
 *	BVDC_Display_GetCallbackSettings
 *************************************************************************/
BERR_Code BVDC_Display_GetCallbackSettings
	( BVDC_Display_Handle                  hDisplay,
	  BVDC_Display_CallbackSettings       *pSettings )
{
	BDBG_ENTER(BVDC_Display_GetCallbackSettings);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if (pSettings)
	{
		*pSettings = hDisplay->stCurInfo.stCallbackSettings;
	}

	BDBG_LEAVE(BVDC_Display_GetCallbackSettings);
	return BERR_SUCCESS;
}

/*************************************************************************
 * BVDC_Display_InstallCallback
 *************************************************************************/
BERR_Code BVDC_Display_InstallCallback
	( BVDC_Display_Handle              hDisplay,
	  const BVDC_CallbackFunc_isr      pfCallback,
	  void                            *pvParm1,
	  int                              iParm2 )
{
	BDBG_ENTER(BVDC_Display_InstallCallback);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	/* Store the new infos */
	hDisplay->stNewInfo.pfGenericCallback = pfCallback;
	hDisplay->stNewInfo.pvGenericParm1    = pvParm1;
	hDisplay->stNewInfo.iGenericParm2     = iParm2;
	if((hDisplay->stCurInfo.pfGenericCallback != pfCallback) ||
	   (hDisplay->stCurInfo.pvGenericParm1    != pvParm1)    ||
	   (hDisplay->stCurInfo.iGenericParm2     != iParm2)     ||
	   (hDisplay->stNewInfo.bErrorLastSetting))
	{
		hDisplay->stNewInfo.stDirty.stBits.bCallbackFunc = BVDC_P_DIRTY;
	}

	BDBG_LEAVE(BVDC_Display_InstallCallback);
	return BERR_SUCCESS;
}

/*************************************************************************
 * BVDC_Display_EnableColorCorrection
 *************************************************************************/
BERR_Code BVDC_Display_EnableColorCorrection
	( BVDC_Display_Handle              hDisplay,
	  bool                             bEnable )
{
	BDBG_ENTER(BVDC_Display_EnableColorCorrection);

	BDBG_LEAVE(BVDC_Display_EnableColorCorrection);

	BSTD_UNUSED(hDisplay);
	BSTD_UNUSED(bEnable);

	BDBG_ERR((" !!! Not supported feature!"));

	return BERR_SUCCESS;
}

/*************************************************************************
 * BVDC_Display_SetColorCorrectionTable
 *************************************************************************/
BERR_Code BVDC_Display_SetColorCorrectionTable
	( BVDC_Display_Handle              hDisplay,
	  uint32_t                         ulGammaTableId,
	  uint32_t                         ulColorTempId )
{
	BDBG_ENTER(BVDC_Display_SetColorCorrectionTable);

	BDBG_LEAVE(BVDC_Display_SetColorCorrectionTable);


	BSTD_UNUSED(hDisplay);
	BSTD_UNUSED(ulGammaTableId);
	BSTD_UNUSED(ulColorTempId);

	BDBG_ERR((" !!! Not supported feature!"));

	return BERR_SUCCESS;
}

/*************************************************************************
 * BVDC_Display_GetColorCorrectionTable
 *************************************************************************/
BERR_Code BVDC_Display_GetColorCorrectionTable
	( const BVDC_Display_Handle        hDisplay,
	  bool                            *pbEnable,
	  uint32_t                        *pulGammaTableId,
	  uint32_t                        *pulColorTempId )
{

	BDBG_ENTER(BVDC_Display_GetColorCorrectionTable);

	BDBG_LEAVE(BVDC_Display_GetColorCorrectionTable);

	BSTD_UNUSED(hDisplay);
	BSTD_UNUSED(pbEnable);
	BSTD_UNUSED(pulGammaTableId);
	BSTD_UNUSED(pulColorTempId);

	BDBG_ERR((" !!! Not supported feature!"));

	return BERR_SUCCESS;
}

/*************************************************************************
 * BVDC_Display_LoadColorCorrectionTable
 *************************************************************************/
BERR_Code BVDC_Display_LoadColorCorrectionTable
	( BVDC_Display_Handle              hDisplay,
	  const uint32_t                  *pulCcbTable )
{
	BDBG_ENTER(BVDC_Display_LoadColorCorrectionTable);
	BDBG_LEAVE(BVDC_Display_LoadColorCorrectionTable);

	BSTD_UNUSED(hDisplay);
	BSTD_UNUSED(pulCcbTable);

	BDBG_ERR((" !!! Not supported feature!"));

	return BERR_SUCCESS;
}

/*************************************************************************
 *
 *************************************************************************/
BERR_Code BVDC_Display_SetDvoConfiguration
	( BVDC_Display_Handle              hDisplay,
	  const BVDC_Display_DvoSettings  *pDvoSettings )
{
	BVDC_P_DisplayInfo *pNewInfo;
	const BVDC_Display_DvoSettings *pCurDvoSettings;

	BDBG_ENTER(BVDC_Display_SetDvoConfiguration);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);
	pNewInfo = &hDisplay->stNewInfo;
	pCurDvoSettings = &hDisplay->stCurInfo.stDvoCfg;

	pNewInfo->stDvoCfg = *pDvoSettings;
	if((hDisplay->stCurInfo.stDvoCfg.eDePolarity    != pDvoSettings->eDePolarity   ) ||
	   (hDisplay->stCurInfo.stDvoCfg.eHsPolarity    != pDvoSettings->eHsPolarity   ) ||
	   (hDisplay->stCurInfo.stDvoCfg.eVsPolarity    != pDvoSettings->eVsPolarity   ) ||
	   (hDisplay->stCurInfo.stDvoCfg.bMudEnable     != pDvoSettings->bMudEnable    ) ||
	   (hDisplay->stCurInfo.stDvoCfg.eMudFilterMode != pDvoSettings->eMudFilterMode) ||
	   (hDisplay->stCurInfo.stDvoCfg.b8BitPanel     != pDvoSettings->b8BitPanel    ) ||
	   (hDisplay->stCurInfo.stDvoCfg.stSpreadSpectrum.bEnable != pDvoSettings->stSpreadSpectrum.bEnable) ||
	   (hDisplay->stCurInfo.stDvoCfg.stSpreadSpectrum.ulFrequency != pDvoSettings->stSpreadSpectrum.ulFrequency) ||
	   (hDisplay->stCurInfo.stDvoCfg.stSpreadSpectrum.ulDelta != pDvoSettings->stSpreadSpectrum.ulDelta) ||
	   (hDisplay->stNewInfo.bErrorLastSetting))
	{
		/* Changes these require the DVO timing block to reconfig */
		hDisplay->stNewInfo.stDirty.stBits.bHdmiEnable = BVDC_P_DIRTY;
		hDisplay->stNewInfo.stDirty.stBits.bHdmiCsc= BVDC_P_DIRTY;
	}

	BDBG_LEAVE(BVDC_Display_SetDvoConfiguration);
	return BERR_SUCCESS;

}

/*************************************************************************
 *
 *************************************************************************/
BERR_Code BVDC_Display_GetDvoConfiguration
	( const BVDC_Display_Handle        hDisplay,
	  BVDC_Display_DvoSettings        *pDvoSettings )
{
	BDBG_ENTER(BVDC_Display_GetDvoConfiguration);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if(pDvoSettings)
	{
		*pDvoSettings = hDisplay->stCurInfo.stDvoCfg;
	}

	BDBG_LEAVE(BVDC_Display_GetDvoConfiguration);
	return BERR_SUCCESS;
}

/*************************************************************************
 *
 *************************************************************************/
BERR_Code BVDC_Display_SetDvoAttenuationRGB
	( BVDC_Display_Handle                hDisplay,
	  int32_t                            nAttentuationR,
	  int32_t                            nAttentuationG,
	  int32_t                            nAttentuationB,
	  int32_t                            nOffsetR,
	  int32_t                            nOffsetG,
	  int32_t                            nOffsetB )
{
	int32_t ulShiftBits;

	BDBG_ENTER(BVDC_Display_SetDvoAttenuationRGB);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	ulShiftBits = BVDC_P_CSC_DVO_CX_F_BITS - 11;

	hDisplay->stNewInfo.lDvoAttenuationR = nAttentuationR << ulShiftBits;
	hDisplay->stNewInfo.lDvoAttenuationG = nAttentuationG << ulShiftBits;
	hDisplay->stNewInfo.lDvoAttenuationB = nAttentuationB << ulShiftBits;
	hDisplay->stNewInfo.lDvoOffsetR      = nOffsetR       << ulShiftBits;
	hDisplay->stNewInfo.lDvoOffsetG      = nOffsetG       << ulShiftBits;
	hDisplay->stNewInfo.lDvoOffsetB      = nOffsetB       << ulShiftBits;
	if((hDisplay->stCurInfo.lDvoAttenuationR != nAttentuationR << ulShiftBits) ||
	   (hDisplay->stCurInfo.lDvoAttenuationG != nAttentuationG << ulShiftBits) ||
	   (hDisplay->stCurInfo.lDvoAttenuationB != nAttentuationB << ulShiftBits) ||
	   (hDisplay->stCurInfo.lDvoOffsetR      != nOffsetR       << ulShiftBits) ||
	   (hDisplay->stCurInfo.lDvoOffsetG      != nOffsetG       << ulShiftBits) ||
	   (hDisplay->stCurInfo.lDvoOffsetB      != nOffsetB       << ulShiftBits) ||
	   (hDisplay->stNewInfo.bErrorLastSetting))
	{
		hDisplay->stNewInfo.stDirty.stBits.bHdmiCsc= BVDC_P_DIRTY;
	}

	BDBG_LEAVE(BVDC_Display_SetDvoAttenuationRGB);
	return BERR_SUCCESS;
}

/*************************************************************************
 *
 *************************************************************************/
BERR_Code BVDC_Display_GetDvoAttenuationRGB
	( BVDC_Display_Handle                hDisplay,
	  int32_t                           *plAttenuationR,
	  int32_t                           *plAttenuationG,
	  int32_t                           *plAttenuationB,
	  int32_t                           *plOffsetR,
	  int32_t                           *plOffsetG,
	  int32_t                           *plOffsetB )
{
	int32_t ulShiftBits;

	BDBG_ENTER(BVDC_Display_GetDvoAttenuationRGB);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	ulShiftBits = BVDC_P_CSC_DVO_CX_F_BITS - 11;

	(*plAttenuationR) = hDisplay->stNewInfo.lDvoAttenuationR >> ulShiftBits;
	(*plAttenuationG) = hDisplay->stNewInfo.lDvoAttenuationG >> ulShiftBits;
	(*plAttenuationB) = hDisplay->stNewInfo.lDvoAttenuationB >> ulShiftBits;
	(*plOffsetR)      = hDisplay->stNewInfo.lDvoOffsetR      >> ulShiftBits;
	(*plOffsetG)      = hDisplay->stNewInfo.lDvoOffsetG      >> ulShiftBits;
	(*plOffsetB)      = hDisplay->stNewInfo.lDvoOffsetB      >> ulShiftBits;

	BDBG_LEAVE(BVDC_Display_GetDvoAttenuationRGB);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Display_SetDvoColorMatrix
	( BVDC_Display_Handle              hDisplay,
	  bool                             bOverride,
	  const int32_t                    pl32_Matrix[BVDC_CSC_COEFF_COUNT],
	  uint32_t                         ulShift )
{
	BDBG_ENTER(BVDC_Display_SetDvoColorMatrix);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	/* set new values */
	hDisplay->stNewInfo.bUserCsc = bOverride;
	if(bOverride)
	{
		uint32_t ulIndex;
		hDisplay->stNewInfo.ulUserShift = ulShift;
		for(ulIndex = 0; ulIndex < BVDC_CSC_COEFF_COUNT; ulIndex++)
		{
			hDisplay->stNewInfo.pl32_Matrix[ulIndex] = pl32_Matrix[ulIndex];
		}
	}

	/* Set display dirty bit */
	hDisplay->stNewInfo.stDirty.stBits.bHdmiCsc = BVDC_P_DIRTY;

	BDBG_LEAVE(BVDC_Display_SetDvoColorMatrix);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Display_GetDvoColorMatrix
	( BVDC_Display_Handle              hDisplay,
	  bool                            *pbOverride,
	  int32_t                          pl32_Matrix[BVDC_CSC_COEFF_COUNT],
	  uint32_t                        *pulShift )
{
	BDBG_ENTER(BVDC_Display_GetDvoColorMatrix);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if(pbOverride)
	{
		*pbOverride = hDisplay->stCurInfo.bUserCsc;
	}

	if(hDisplay->stCurInfo.bUserCsc)
	{
		uint32_t ulIndex;
		for(ulIndex = 0; ulIndex < BVDC_CSC_COEFF_COUNT; ulIndex++)
		{
			pl32_Matrix[ulIndex] = hDisplay->stCurInfo.pl32_Matrix[ulIndex];
		}

		if(pulShift)
		{
			*pulShift = hDisplay->stCurInfo.ulUserShift;
		}
	}
	else
	{
		BKNI_EnterCriticalSection();
		BVDC_P_Csc_ToMatrix_isr(pl32_Matrix, &hDisplay->stDvoCscMatrix.stCscCoeffs,
			BVDC_P_FIX_POINT_SHIFT);
		BKNI_LeaveCriticalSection();

		if(pulShift)
		{
			*pulShift = BVDC_P_FIX_POINT_SHIFT;
		}
	}

	BDBG_LEAVE(BVDC_Display_GetDvoColorMatrix);
	return BERR_SUCCESS;

}

/***************************************************************************
 *
 */
BERR_Code BVDC_Display_SetAlignment
	( BVDC_Display_Handle                           hDisplay,
	  BVDC_Display_Handle                           hTargetDisplay,
	  const BVDC_Display_AlignmentSettings         *pAlignSettings )
{
	BDBG_ENTER(BVDC_Display_SetAlignment);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);


	/* VEC alignment is per user request. As long as hTargetDisplay and
	 * pAlignSettings are not NULL we will start alignment process.
	 */

	/* set new values */
	if(hTargetDisplay && pAlignSettings)
	{
		hDisplay->stNewInfo.hTargetDisplay = hTargetDisplay;
		hDisplay->stNewInfo.stAlignCfg     = *pAlignSettings;
	}
	else
	{
		hDisplay->stNewInfo.hTargetDisplay = NULL;
	}

	/* Set display dirty bit */
	hDisplay->stNewInfo.stDirty.stBits.bAlignment = BVDC_P_DIRTY;

	BDBG_LEAVE(BVDC_Display_SetAlignment);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Display_GetAlignment
	( const BVDC_Display_Handle                  hDisplay,
	  BVDC_Display_Handle                       *phTargetDisplay,
	  BVDC_Display_AlignmentSettings            *pAlignSettings )
{
	BDBG_ENTER(BVDC_Display_GetAlignment);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	/* set new values */
	if(pAlignSettings)
	{
		*pAlignSettings = hDisplay->stCurInfo.stAlignCfg;
	}

	if(phTargetDisplay)
	{
		*phTargetDisplay = hDisplay->stNewInfo.hTargetDisplay;
	}

	BDBG_LEAVE(BVDC_Display_GetAlignment);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Display_SetOrientation
	( const BVDC_Display_Handle        hDisplay,
	  BFMT_Orientation                 eOrientation )
{
	BDBG_ENTER(BVDC_Display_SetOrientation);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	hDisplay->stNewInfo.eOrientation = eOrientation;
	hDisplay->hCompositor->stNewInfo.eOrientation = eOrientation;

	if((hDisplay->stCurInfo.eOrientation != eOrientation) ||
	   (hDisplay->stNewInfo.bErrorLastSetting))
	{
		hDisplay->stNewInfo.stDirty.stBits.b3DSetting = BVDC_P_DIRTY;
		hDisplay->stNewInfo.stDirty.stBits.bAspRatio = BVDC_P_DIRTY;

#if BVDC_P_SUPPORT_3D_VIDEO
		/* CMP_0_CMP_CTRL.BVB_VIDEO is shared by main, pip and graphics
		 * window on the CMP. Graphics has no delay, main and pip video
		 * window have different delay. So we treat display orientation
		 * change as format change to flush all the buffers in video
		 * windows to avoid BVN errors.
		 */
		hDisplay->stNewInfo.stDirty.stBits.bTiming = BVDC_P_DIRTY;
#endif

	}

	BDBG_LEAVE(BVDC_Display_SetOrientation);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Display_GetOrientation
	( const BVDC_Display_Handle        hDisplay,
	  BFMT_Orientation                *peOrientation )
{
	BDBG_ENTER(BVDC_Display_GetOrientation);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if(peOrientation)
	{
		*peOrientation = hDisplay->stCurInfo.eOrientation;
	}

	BDBG_LEAVE(BVDC_Display_GetOrientation);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Display_Set3dSourceBufferSelect
	( const BVDC_Display_Handle        hDisplay,
	  BVDC_3dSourceBufferSelect        e3dSrcBufSel )
{
	BDBG_ENTER(BVDC_Display_Set3dSourceBufferSelect);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

#if (!BVDC_P_MFD_SUPPORT_3D_VIDEO)
	if(e3dSrcBufSel != BVDC_3dSourceBufferSelect_eNormal)
	{
		BDBG_ERR(("3D video is not supported!"));
	}
#endif

	hDisplay->stNewInfo.e3dSrcBufSel = e3dSrcBufSel;
	if((hDisplay->stCurInfo.e3dSrcBufSel != e3dSrcBufSel) ||
	   (hDisplay->stNewInfo.bErrorLastSetting))
	{
		hDisplay->stNewInfo.stDirty.stBits.b3DSetting = BVDC_P_DIRTY;
	}

	BDBG_LEAVE(BVDC_Display_Set3dSourceBufferSelect);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Display_Get3dSourceBufferSelect
	( const BVDC_Display_Handle        hDisplay,
	  BVDC_3dSourceBufferSelect       *pe3dSrcBufSel )
{
	BDBG_ENTER(BVDC_Display_Get3dSourceBufferSelect);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if(pe3dSrcBufSel)
	{
		*pe3dSrcBufSel = hDisplay->stCurInfo.e3dSrcBufSel;
	}

	BDBG_LEAVE(BVDC_Display_Get3dSourceBufferSelect);
	return BERR_SUCCESS;
}

bool aaDacOutput_ChVfUsage_Tbl[][3] =
{
/*   Ch0,   Ch1,   Ch2 */
	{true,  false, false}, /* BVDC_DacOutput_eSVideo_Luma */
	{false, true,  true }, /* BVDC_DacOutput_eSVideo_Chroma */
	{true,  true,  true }, /* BVDC_DacOutput_eComposite */
	{false, true,  false}, /* BVDC_DacOutput_eRed */
	{true,  false, false}, /* BVDC_DacOutput_eGreen */
	{false, false, true }, /* BVDC_DacOutput_eBlue */
	{true,  false, false}, /* BVDC_DacOutput_eY */
	{false, true,  false}, /* BVDC_DacOutput_ePr */
	{false, false, true }, /* BVDC_DacOutput_ePb */
	{true,  false, false}, /* BVDC_DacOutput_eHsync */
	{true,  false, false}, /* BVDC_DacOutput_eGreen_NoSync */
	{false, false, false}, /* BVDC_DacOutput_eVdec0 */
	{false, false, false}, /* BVDC_DacOutput_eIfdm0 */
	{false, false, false}, /* BVDC_DacOutput_eIfdm1 */
	{false, false, false}, /* BVDC_DacOutput_eFilteredCvbs */
	{false, false, false}  /* BVDC_DacOutput_eUnused */
};

/***************************************************************************
 *
 */
BERR_Code BVDC_Display_SetVfFilter
	( BVDC_Display_Handle              hDisplay,
	  BVDC_DisplayOutput               eDisplayOutput,
	  BVDC_DacOutput                   eDacOutput,
	  bool                             bOverride,
	  const uint32_t                  *paulFilterRegs,
	  uint32_t                         ulNumFilterRegs )
{
	bool *pabUserVfFilter = NULL;
	int32_t bUseCh0, bUseCh1, bUseCh2;

	BDBG_ENTER(BVDC_Display_SetVfFilter);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if((eDisplayOutput != BVDC_DisplayOutput_eComponent) &&
	   (eDisplayOutput != BVDC_DisplayOutput_eComposite) &&
	   (eDisplayOutput != BVDC_DisplayOutput_eSVideo))
	{
		BDBG_ERR(("VF filter can only be set on Component, Cvbs, or Svideo outputs."));
		return BERR_INVALID_PARAMETER;
	}

	if (bOverride && (ulNumFilterRegs < BVDC_P_CHROMA_TABLE_SIZE))
	{
		BDBG_ERR(("Incorrect VF filter table size %d.  Table size should be %d or greater for this platform. ", ulNumFilterRegs, BVDC_P_CHROMA_TABLE_SIZE));
		return BERR_INVALID_PARAMETER;
	}

	if (!BVDC_P_DISP_IS_VALID_DISPOUTPUT_AND_DAC(eDispOutput, eDacOutput))
	{
		BDBG_ERR(("Invalid display output and dac output combination."));
		return BERR_INVALID_PARAMETER;
	}

	bUseCh0 = aaDacOutput_ChVfUsage_Tbl[eDacOutput][0];
	bUseCh1 = aaDacOutput_ChVfUsage_Tbl[eDacOutput][1];
	bUseCh2 = aaDacOutput_ChVfUsage_Tbl[eDacOutput][2];

	if(!bUseCh0 && !bUseCh1 && !bUseCh2)
	{
		BDBG_ERR(("Invalid DAC Output."));
		return BERR_INVALID_PARAMETER;
	}

	if (eDisplayOutput == BVDC_DisplayOutput_eComponent)
	{
		pabUserVfFilter = hDisplay->stNewInfo.abUserVfFilterCo;
	}
	else
	{
		pabUserVfFilter = hDisplay->stNewInfo.abUserVfFilterCvbs;
	}

	/* Program CH0 VF */
	if (bUseCh0)
	{
		pabUserVfFilter[0] = bOverride;
		if(bOverride)
		{
			if (eDisplayOutput == BVDC_DisplayOutput_eComponent)
				BKNI_Memcpy(hDisplay->stNewInfo.aaulUserVfFilterCo[0], paulFilterRegs, BVDC_P_CHROMA_TABLE_SIZE * sizeof(uint32_t));
			else
				BKNI_Memcpy(hDisplay->stNewInfo.aaulUserVfFilterCvbs[0], paulFilterRegs, BVDC_P_CHROMA_TABLE_SIZE * sizeof(uint32_t));
		}
	}

	/* Program CH1 VF */
	if (bUseCh1)
	{
		pabUserVfFilter[1] = bOverride;
		if(bOverride)
		{
			if (eDisplayOutput == BVDC_DisplayOutput_eComponent)
				BKNI_Memcpy(hDisplay->stNewInfo.aaulUserVfFilterCo[1], paulFilterRegs, BVDC_P_CHROMA_TABLE_SIZE * sizeof(uint32_t));
			else
				BKNI_Memcpy(hDisplay->stNewInfo.aaulUserVfFilterCvbs[1], paulFilterRegs, BVDC_P_CHROMA_TABLE_SIZE * sizeof(uint32_t));
		}
	}

	/* Program CH2 VF */
	if (bUseCh2)
	{
		pabUserVfFilter[2] = bOverride;
		if(bOverride)
		{
			if (eDisplayOutput == BVDC_DisplayOutput_eComponent)
				BKNI_Memcpy(hDisplay->stNewInfo.aaulUserVfFilterCo[2], paulFilterRegs, BVDC_P_CHROMA_TABLE_SIZE * sizeof(uint32_t));
			else
				BKNI_Memcpy(hDisplay->stNewInfo.aaulUserVfFilterCvbs[2], paulFilterRegs, BVDC_P_CHROMA_TABLE_SIZE * sizeof(uint32_t));
		}
	}

	/* Set display dirty bit */
	hDisplay->stNewInfo.stDirty.stBits.bVfFilter = BVDC_P_DIRTY;

	BDBG_LEAVE(BVDC_Display_SetVfFilter);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Display_GetVfFilter
	( BVDC_Display_Handle              hDisplay,
	  BVDC_DisplayOutput               eDisplayOutput,
	  BVDC_DacOutput                   eDacOutput,
	  bool                            *pbOverride,
	  uint32_t                        *paulFilterRegs,
	  uint32_t                         ulNumFilterRegs )
{
	BVDC_P_DisplayAnlgChan *pAnlgChan = NULL;
	uint32_t *paulUserVfFilter = NULL;
	int32_t lChannel;
	bool bOverride = false;

	BDBG_ENTER(BVDC_Display_GetVfFilter);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if((eDisplayOutput != BVDC_DisplayOutput_eComponent) &&
	   (eDisplayOutput != BVDC_DisplayOutput_eComposite) &&
	   (eDisplayOutput != BVDC_DisplayOutput_eSVideo))
	{
		BDBG_ERR(("VF filter can only be retrieved from Component, Cvbs, or Svideo outputs."));
		return BERR_INVALID_PARAMETER;
	}

	if (ulNumFilterRegs < BVDC_P_CHROMA_TABLE_SIZE)
	{
		BDBG_ERR(("Incorrect VF filter table size %d.  Table size should be %d or greater for this platform. ", ulNumFilterRegs, BVDC_P_CHROMA_TABLE_SIZE));
		return BERR_INVALID_PARAMETER;
	}

	if (!BVDC_P_DISP_IS_VALID_DISPOUTPUT_AND_DAC(eDispOutput, eDacOutput))
	{
		BDBG_ERR(("Invalid display output and dac output combination."));
		return BERR_INVALID_PARAMETER;
	}

	lChannel = aaDacOutput_ChVfUsage_Tbl[eDacOutput][0] ? 0 :
	           (aaDacOutput_ChVfUsage_Tbl[eDacOutput][1] ? 1 :
	            (aaDacOutput_ChVfUsage_Tbl[eDacOutput][2] ? 2 : BVDC_P_DISP_INVALID_VF_CH));

	if (lChannel == BVDC_P_DISP_INVALID_VF_CH)
	{
		BDBG_ERR(("Invalid DAC Output."));
		return BERR_INVALID_PARAMETER;
	}

	if (eDisplayOutput == BVDC_DisplayOutput_eComponent)
	{
		bOverride = hDisplay->stCurInfo.abUserVfFilterCo[lChannel];
		paulUserVfFilter = (uint32_t *)hDisplay->stCurInfo.aaulUserVfFilterCo[lChannel];
	}
	else
	{
		bOverride = hDisplay->stCurInfo.abUserVfFilterCvbs[lChannel];
		paulUserVfFilter = (uint32_t *)hDisplay->stCurInfo.aaulUserVfFilterCvbs[lChannel];
	}

	if(pbOverride)
	{
		*pbOverride = bOverride;
	}

	if(bOverride)
	{
		/* TODO: Is this necessary or can be merged with below? */
		BKNI_Memcpy(paulFilterRegs, paulUserVfFilter, BVDC_P_CHROMA_TABLE_SIZE * sizeof(uint32_t));
	}
	else
	{
		BVDC_P_Display_GetAnlgChanByOutput_isr(hDisplay, &hDisplay->stNewInfo, eDisplayOutput, &pAnlgChan);

		if(!pAnlgChan)
		{
			BDBG_ERR(("Display output does not exist.  Cannot get internal VF filter settings."));
			return BERR_INVALID_PARAMETER;
		}
		BKNI_EnterCriticalSection();
		BKNI_Memcpy(paulFilterRegs, pAnlgChan->apVfFilter[lChannel], BVDC_P_CHROMA_TABLE_SIZE * sizeof(uint32_t));
		BKNI_LeaveCriticalSection();
	}

	BDBG_LEAVE(BVDC_Display_GetDvoColorMatrix);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Display_SetMuteMode
	( BVDC_Display_Handle              hDisplay,
	  BVDC_DisplayOutput               eDisplayOutput,
	  BVDC_MuteMode                    eMuteMode )
{
	bool bMute;

	BDBG_ENTER(BVDC_Display_SetMuteMode);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if (eMuteMode == BVDC_MuteMode_eConst)
	{
		bMute = true;
	}
	else if (eMuteMode == BVDC_MuteMode_eDisable)
	{
		bMute = false;
	}
	else
	{
		BDBG_ERR(("Selected mute mode not supported for output displays"));
		return BERR_INVALID_PARAMETER;
	}

	/* set new values */
	hDisplay->stNewInfo.abOutputMute[eDisplayOutput] = bMute;

	/* tie Cvbs and Svideo together */
	if ((eDisplayOutput == BVDC_DisplayOutput_eComposite) || (eDisplayOutput == BVDC_DisplayOutput_eSVideo))
	{
		hDisplay->stNewInfo.abOutputMute[BVDC_DisplayOutput_eSVideo] = bMute;
		hDisplay->stNewInfo.abOutputMute[BVDC_DisplayOutput_eComposite] = bMute;
	}

	/* Set display dirty bit */
	hDisplay->stNewInfo.stDirty.stBits.bOutputMute = BVDC_P_DIRTY;

	BDBG_LEAVE(BVDC_Display_SetMuteMode);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Display_GetMuteMode
	( BVDC_Display_Handle              hDisplay,
	  BVDC_DisplayOutput               eDisplayOutput,
	  BVDC_MuteMode                   *peMuteMode )
{
	bool bMute;

	BDBG_ENTER(BVDC_Display_GetMuteMode);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if(peMuteMode)
	{
		bMute = hDisplay->stCurInfo.abOutputMute[eDisplayOutput];
		*peMuteMode = bMute ? BVDC_MuteMode_eConst : BVDC_MuteMode_eDisable;
	}

	BDBG_LEAVE(BVDC_Display_GetMuteMode);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Display_GetCapabilities
	( BVDC_Display_Handle              hDisplay,
	  BVDC_Display_Capabilities       *pCapabilities )
{
	BSTD_UNUSED(hDisplay);

	if(pCapabilities)
	{
		/* To make sure thing get initialize */
		BKNI_Memset(pCapabilities, 0, sizeof(*pCapabilities));
		pCapabilities->pfIsVidfmtSupported = BVDC_P_IsVidfmtSupported;
	}

	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Display_SetArtificticalVsync
	( BVDC_Display_Handle              hDisplay,
	  bool                             bEnable,
	  uint32_t                         ulVsyncRegAddr,
	  uint32_t                         ulVsyncMask )
{
	BDBG_ENTER(BVDC_Display_SetArtificticalVsync);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	/* set new values */
	hDisplay->stNewInfo.bArtificialVsync   = bEnable;
	hDisplay->stNewInfo.ulArtificialVsyncRegAddr = ulVsyncRegAddr;
	hDisplay->stNewInfo.ulArtificialVsyncMask = ulVsyncMask;

	/* Set display dirty bit */
	hDisplay->stNewInfo.stDirty.stBits.bMiscCtrl = BVDC_P_DIRTY;

	BDBG_LEAVE(BVDC_Display_SetArtificticalVsync);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Display_GetArtificticalVsync
	( const BVDC_Display_Handle        hDisplay,
	  bool                            *pbEnable,
	  uint32_t                        *pulVsyncRegAddr,
	  uint32_t                        *pulVsyncMask )
{
	BDBG_ENTER(BVDC_Display_GetArtificticalVsync);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if(pbEnable)
	{
		*pbEnable = hDisplay->stCurInfo.bArtificialVsync;
	}

	if(pulVsyncRegAddr)
	{
		*pulVsyncRegAddr = hDisplay->stCurInfo.ulArtificialVsyncRegAddr;
	}

	if(pulVsyncMask)
	{
		*pulVsyncMask = hDisplay->stCurInfo.ulArtificialVsyncMask;
	}

	BDBG_LEAVE(BVDC_Display_GetArtificticalVsync);
	return BERR_SUCCESS;
}

/* End of File */
