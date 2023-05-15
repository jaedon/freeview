/***************************************************************************
*     Copyright (c) 2004-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brap_op.c $
* $brcm_Revision: Hydra_Software_Devel/70 $
* $brcm_Date: 9/14/12 11:45a $
*
* Module Description:
*   Module name: OUTPUT
*   This file contains the implementation of all PIs for the Output Port
*   abstraction. 
*   
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/rap/src/fmm65/brap_op.c $
* 
* Hydra_Software_Devel/70   9/14/12 11:45a srajapur
* SW7425-3090: [7425] Added support for run-time programmable
* VCXO/timebase settings
* 
* Hydra_Software_Devel/69   8/24/12 5:02p purush
* SW7425-3090: [7425] Added support for run-time programmable
* VCXO/timebase settings
* 
* Hydra_Software_Devel/68   8/24/12 1:39p purush
* SW7420-2271: [7420] Removing the unwanted prints
* 
* Hydra_Software_Devel/67   8/24/12 11:52a purush
* SW7420-2271: [7420] Added support for run-time programmable
* VCXO/timebase settings
* 
* Hydra_Software_Devel/66   11/14/11 3:29p srajapur
* SW7125-1123 : [7125] Removing  the  PR30188 workaround for
* I2SS0_CROSSBAR
* 
* Hydra_Software_Devel/65   6/3/11 4:03p gautamk
* SW7325-852: [7325] Programming default MCLK for spdif to 128fs. In case
* of DTS Compressed to PCM transition program it to 256fs.
* 
* Hydra_Software_Devel/64   5/23/11 4:48p gautamk
* SW7405-4713:[7405] Correcting the i2s offset.
* 
* Hydra_Software_Devel/63   3/29/11 2:30p gautamk
* SW7405-4713: [7405]Do not read from regaddress of 0
* 
* Hydra_Software_Devel/62   2/11/11 7:39a srajapur
* SW7550-601:[7550]Add BTSC Support in FW PI and CIT -------->
* Programming PEAK A1,A2 and GAIN value to reset values when BTSC is
* enabled on DAC
* 
* Hydra_Software_Devel/61   12/2/10 3:13p srajapur
* SW7550-623:[7550]About the HDMI output of audio playback. --> Fixed the
* issue by programming the MCLK correctly.
* 
* Hydra_Software_Devel/60   9/30/10 1:44p gautamk
* SW7420-850: [7420] Adding PI support for 24bit SPDIF PCM sample
* support.
* 
* Hydra_Software_Devel/59   9/29/10 1:46p gautamk
* SW7420-966: [7420] Using MAI_MULTI is TMG_SRC_SELECT for MAI for all
* the cases for 7420 family.
* 
* Hydra_Software_Devel/58   9/6/10 5:54p gautamk
* SW7405-4713: [7405] Programming FCI CFG ID for IOP to 0x3ff at stop
* time
* 
* Hydra_Software_Devel/57   8/31/10 2:42p gautamk
* SW7420-994: [7420] Fixing Mute status for 5.1 PCM.
* 
* Hydra_Software_Devel/56   7/23/10 3:50p gautamk
* SW7405-4646: [7405] Restoring mute status of O/P at mixer Level in
* every Start.
* 
* Hydra_Software_Devel/55   6/16/10 2:20p gautamk
* SW7405-3063: [7405] Fixing compilation warning with DEBUG=n option
* 
* Hydra_Software_Devel/54   4/29/10 5:37p gautamk
* SW7325-547: [7405] Changing the exopsed paramter name
* 
* Hydra_Software_Devel/53   4/29/10 5:32p gautamk
* SW7325-547: [7405] Adding CGMS bit SPDIF Ch bits
* 
* Hydra_Software_Devel/52   4/14/10 6:55p srajapur
* SW7550-323 : [7550] Distorted audio is heard, while running playback of
* few WAV files using Nexus.
* 
* Hydra_Software_Devel/51   4/8/10 3:16p gautamk
* SW7125-261: [7125] Fixing coverity issue
* 
* Hydra_Software_Devel/50   4/2/10 8:51a srajapur
* SW7550-275 : [7550] DDP passthrough on HDMI is not working.
* 
* Hydra_Software_Devel/49   3/8/10 8:53p sgadara
* SW7420-611: [3548,3556] Reverting back the changes as the existing
* interface of exclusive API for programming DAC volume is made use of.
* DAC Scale register continues to get programmed using SetOutputConfig
* --> For Volume register SetOutputConfig will introduce audio drop since
* there will be mute->unmute happening
* 
* Hydra_Software_Devel/48   2/16/10 2:13p sgadara
* SW3556-1033: [3548,3556] Fixing build issue for non-DTV platforms
* 
* Hydra_Software_Devel/47   2/15/10 9:36p sgadara
* SW3556-1033: [3548,3556] Providing the interface to program the
* external master clock. The API can also be used to provide the clock
* info soon after RAP device open for those of interest.
* --> Merge to main line
* 
* Hydra_Software_Devel/46   1/21/10 2:10p gautamk
* SW7125-174:[7125]Fixing compiler warning
* 
* Hydra_Software_Devel/45   1/20/10 3:09p gautamk
* SW7125-174:[7125] Fixing compiler warning.
* 
* Hydra_Software_Devel/44   1/13/10 2:46p gautamk
* SW7405-3202: [7405] implementing code review comments
* 
* Hydra_Software_Devel/43   1/8/10 5:24p sgadara
* SW3556-940: [3548,3556] Adding the interface to program the SCALE and
* DAC_VOLUME registers in HIFIDAC_CTRL
* 
* Hydra_Software_Devel/42   1/6/10 4:42p gautamk
* SW7405-3063: [7405] Adding support for I2s Stereo 1
* 
* Hydra_Software_Devel/41   12/1/09 6:57p gautamk
* SW7405-3063: [7405] Fixing segmentation fault.
* 
* Hydra_Software_Devel/40   11/20/09 1:51p sgadara
* SW3556-832 : [3548,3556] Adding the "dynamic port switching" feature
* support.
* --> Addign PLL1 support for SPDIF for 3548 family
* 
* Hydra_Software_Devel/39   11/13/09 6:05p gautamk
* SW7420-293: [7420] Not programming MAI_MULTI_GROUPING, because on
* watchdog trigger this causes audio loss.
* 
* Hydra_Software_Devel/38   10/28/09 12:10p gautamk
* SW7420-361: [7420] Programming stream ID for DAC1 correctly.
* 
* Hydra_Software_Devel/37   10/27/09 12:42p gautamk
* SW7405-3110: [7405]Adding Destination sample rate change interrupt.
* This will need to be installed for HDMI. Whenver sample rate changes,
* this interrupt gets trigerred. So app need to install callback
* function on it to program HDMI rx sample rate. Checkin at mainline
* 
* Hydra_Software_Devel/36   10/21/09 5:05p gautamk
* SW7405-3063: [7405] For HDMI, Programming Default MCLk programming as
* 128fs
* 
* Hydra_Software_Devel/35   10/15/09 1:31p srajapur
* SW7550-52: [7550] Merging 7550 branch code to main line
* --> Adding mclk messages for spdif and maimulti port
* 
* Hydra_Software_Devel/34   10/5/09 4:00p gautamk
* SW7405-3063: [7405]Fixing mute of 5.1 HDMI
* 
* Hydra_Software_Devel/33   10/2/09 2:35a srajapur
* SW7550-52: [7550] Merging 7550 branch code to main line
* 
* Hydra_Software_Devel/32   9/18/09 3:06p gautamk
* SW7405-2606: [7405] correcting default Config of MAI
* 
* Hydra_Software_Devel/31   9/16/09 1:11p gautamk
* SW7405-2606: [7405] Not setting volume for O?p having compressed data.
* 
* Hydra_Software_Devel/30   9/8/09 8:29p sgadara
* SW7405-2984: [3548,3556] Adding function to Program MAI MCLK
* --> Fixing the compilation break for 3548 family
* 
* Hydra_Software_Devel/29   9/8/09 12:40p gautamk
* SW7405-2984: [7405] Adding function to Prgram MAI MCLK
* 
* Hydra_Software_Devel/28   9/3/09 5:44p gautamk
* SW7325-388: [7405] Storing Volume Level in both Db and 5.23 Format.
* Also both these paramter remains in Sync when calling different Volume
* APIs
* 
* Hydra_Software_Devel/27   8/17/09 5:13p gautamk
* PR57743:[7405] using Pvt functions when calling from internal RAP PI
* 
* Hydra_Software_Devel/26   8/11/09 3:17p gautamk
* PR55224: [7405] Fixing compilatio for 7340,7342
* 
* Hydra_Software_Devel/25   7/23/09 4:49p bhanus
* PR 55499 : [7340,7342,7125]Adding 7420 Family support
* 
* Hydra_Software_Devel/24   6/23/09 4:46p gautamk
* PR56006: [7405] Fixing coverity bug
* 
* Hydra_Software_Devel/23   6/18/09 2:41p gautamk
* PR56112: [7420] fixing compile error
* 
* Hydra_Software_Devel/22   6/18/09 2:19p gautamk
* PR56112:[7420] Adding PI support for HDMI 5.1PCm for 7420
* 
* Hydra_Software_Devel/21   6/17/09 4:50p gautamk
* PR56010: [7405] Implmenting Code review comment
* 
* Hydra_Software_Devel/20   6/12/09 3:40p gautamk
* PR53302: [7405] Merging cleanup code to mainline
* 
* Hydra_Software_Devel/19   5/18/09 5:10p gautamk
* PR52383:[7405] correcting debug msg.
* 
* Hydra_Software_Devel/18   4/29/09 9:54p gautamk
* PR52383: [7405] fixing warning
* 
* Hydra_Software_Devel/17   4/28/09 1:52p gautamk
* PR52383: [7405] fixing compilation error
* 
* Hydra_Software_Devel/16   4/28/09 1:24p gautamk
* PR52383: [7405] This implementaion is to fix a case where Dts XCD to
* PCm transition on
* HDMI soesn't happen on Onkyo reciever when we don't diable MAI_CFG at
* MAI_STOP
* 
* Hydra_Software_Devel/15   4/7/09 4:59p gautamk
* PR52383: [7405] Programming IOP_CTRl_FCI_CFG for 5.1PCM
* 
* Hydra_Software_Devel/14   4/2/09 11:22a speter
* PR 52729: [3548,3556] Adding bHbrEnable flag to SPDIF Output as High
* bit rate is required to be supported through SPDIF out as well.
* 
* Hydra_Software_Devel/13   3/23/09 3:55p gautamk
* PR52334: [7405] Modifying register programming for MS/OP/PLL blocks, to
* solve audio loss on spdif.
* 
* Hydra_Software_Devel/12   3/10/09 1:49p gautamk
* PR52909: [7405] On muting HDMI sending pause/null burst
* 
* Hydra_Software_Devel/11   2/25/09 7:01p gautamk
* PR52383: [7405] For PCM 5.1 HDMI disabling STREAM_ENA1 for HDMI. It
* fixes 5.1
* 
* Hydra_Software_Devel/10   2/10/09 9:42a gdata
* PR51968: [7420] Changes for HDMI mux selector
* 
* Hydra_Software_Devel/9   2/2/09 3:42p gautamk
* PR51618: [7405] Changing the global variables as per following rule.
* 1) make them static
* 2) if you cannot make them static because they must be shared between
* multiple .c files in your module, you must rename them to include the
* BXXX_ magnum prefix for your module.
* 
* Hydra_Software_Devel/8   1/20/09 2:34p gautamk
* PR48850:[7405] Changing the MAPPER_Mute Type for 65nm chips.
* 
* Hydra_Software_Devel/7   1/2/09 3:34p gautamk
* PR48681: [7405] When not disabling MAI CFG at Mai Stop, Transition from
* DTS to PCM is not happening on HDMI.
* 
* Hydra_Software_Devel/6   12/3/08 4:58p gautamk
* PR48850: [7405] Removing warning.
* 
* Hydra_Software_Devel/5   12/1/08 3:24p gautamk
* PR48681: [7405] Audio loss on Channel change on Dynex TV fixed.
* --Not disabling  MS/OP CTRL registers for SPDIF / MAI on stop/Close
* Noise at stop deoced fixed.
*  -- Setting the RAmp value ion MS_CTRL at Stop.
* 
* Hydra_Software_Devel/4   11/11/08 2:46p bhanus
* PR 47757 : [7420]Checking in 7420 Support
* 
* Hydra_Software_Devel/3   10/10/08 10:08a speter
* PR 47764: [3548,3556] Merging the release branch 3.0 to main line of
* development
* 
* Hydra_Software_Devel/2   9/26/08 6:50p gautamk
* PR47355: [7405] AUD_FMM_OP_CTRL_ENABLE.STREAM1_ENA for Mai port should
* not be enabled if it its muxselector is not MAI.
* 
* Hydra_Software_Devel/RAP_354856_PH_3_0/1   10/1/08 3:09p speter
* PR 45602: [3548,3556] Adding code to have PLL and FS control for
* Internal Capture ports to 3548 family
* 
* Hydra_Software_Devel/1   7/21/08 2:17p speter
* PR 44984: [3548,3556,7405] Adding files from base2 which have new
* Architecture specific changes and cannot be merged back in base2
* 
* Hydra_Software_Devel/Rap_NewFwArch/12   7/9/08 10:16a speter
* PR 44660:[3548,3556] Merging the Phase 1.5 release branch code to main
* line of development
* 
* Hydra_Software_Devel/Rap_NewFwArch/11   6/14/08 2:11a speter
* PR 38950: [3548,3556] Adding a fix to make I2S Stereo work
* 
* Hydra_Software_Devel/Rap_NewFwArch/10   5/21/08 2:49p gautamk
* PR42809: [7335] Audio lost on DAC when doing channel-change of MPEG
* stream.
* Programming HBR MCLK even if bHBREnable is false.
* 
* Hydra_Software_Devel/Rap_NewFwArch/9   5/14/08 4:32p speter
* PR 38950: [3548,3556] Adding code to support HIFIDAC1 and HIFIDAC2
* 
* Hydra_Software_Devel/Rap_NewFwArch/8   5/9/08 3:01p speter
* PR 38950: [3548,3556] Fixing the code for 3548 family
* 
* Hydra_Software_Devel/Rap_NewFwArch/7   5/5/08 11:31a speter
* PR 38950: [3548] Making 3548_FAMILY macro and using it for both 3548
* and 3556 cases.
* 
* Hydra_Software_Devel/Rap_NewFwArch/6   4/28/08 4:35p speter
* PR 38950: [3548] Fixing compilation for 3548/3556.
* 
* Hydra_Software_Devel/Rap_NewFwArch/5   4/22/08 2:17p gautamk
* PR41994: [7405] Using BCHP_VER_ Correctly...
* 
* Hydra_Software_Devel/Rap_NewFwArch/4   4/13/08 5:29p sushmit
* PR 34648: [7405] Fixing PCM-Compressed movement of HDMI.
* 
* Hydra_Software_Devel/Rap_NewFwArch/3   4/10/08 6:30p sushmit
* PR 34648: [7405] Adding Timebase programming for MAI.
* 
* Hydra_Software_Devel/Rap_NewFwArch/2   4/10/08 6:12p sushmit
* PR 34648: [7405] DDP Passthru Bringup.
* 
* Hydra_Software_Devel/Rap_NewFwArch/1   4/8/08 2:12p gautamk
* PR34648: [7405] Merging wdog code to mainline
* 
* Hydra_Software_Devel/Rap_NewFwArch/RAP_NEWFWARCH_WDOG/1   3/28/08 4:06p gautamk
* PR34648: [7405] adding code for watchdog recovery in a branch
* 
* Hydra_Software_Devel/89   2/22/08 2:56p gdata
* PR 39821 :[7440] Fixing the compilation error due to previous check-in
* 
* Hydra_Software_Devel/88   2/22/08 4:42a bhanus
* PR 39821 :[7440] Exposing HDMI crossbar.
* 
* Hydra_Software_Devel/87   2/19/08 2:56p sushmit
* PR 39255: [7405] Timebase error removed
* 
* Hydra_Software_Devel/86   2/7/08 3:03p speter
* PR 38783: [3563] Fixing the compilation break.
* 
* Hydra_Software_Devel/85   2/5/08 4:27p gautamk
* PR38783: [7405] Add support for I2S capture on 7405 raptor PI. Added
* support for BRAP_OP_ExtMClkConfig API
* 
* Hydra_Software_Devel/84   2/2/08 12:02a bhanus
* PR 39110 : [7440] Uncommenting the 192 kHz for HDMI as updated RDB has
* been checked in.
* 
* Hydra_Software_Devel/83   1/30/08 10:18a gdata
* PR 39110 : [7440] Correcting the compilation erroe
* 
* Hydra_Software_Devel/82   1/30/08 7:40a bhanus
* PR 39110 : [7440]Adding support for HBR modes on internal HDMI
* 
* Hydra_Software_Devel/81   11/21/07 2:55p sushmit
* PR 36823: [7405] Minor Changes
* 
* Hydra_Software_Devel/80   11/20/07 8:44p sushmit
* PR 36877: [7335] Bringup PI
* 
* Hydra_Software_Devel/79   11/19/07 5:00p sushmit
* PR 36822: [7405] Fixing issues with passthru channel for combined SPDIF
* & HDMI compressed output
* 
* Hydra_Software_Devel/78   11/19/07 3:14p sushmit
* PR 36822: [7405] Making MAI take data straight from Mixer.
* 
* Hydra_Software_Devel/RAP_7405_PR36115/1   11/15/07 1:06p sushmit
* PR 36115: [7405] Fixing MAI programming so that HDMI CBITs get sent
* out.
* 
* Hydra_Software_Devel/77   11/11/07 7:35p sushmit
* PR 36167: [7405] Fixing 7405 HDMI MuteControl
* 
* Hydra_Software_Devel/76   11/6/07 3:12p sushmit
* PR 36115: [7405] Fixing PCM-HDMI Cbit programming.
* 
* Hydra_Software_Devel/75   10/26/07 7:27p bhanus
* PR 36477 : [7440] Fixing the Bus error introduced due to wrong offset
* in the last checkin
* 
* Hydra_Software_Devel/74   10/25/07 1:40a gautamk
* PR36409: [3563] Fixing Coverity issue for 3563 RAP Code base
* 
* Hydra_Software_Devel/73   10/23/07 1:11p speter
* PR 36368: [3563] Fixing the Timebase 1 for 3563 also
* 
* Hydra_Software_Devel/72   10/22/07 12:09p speter
* PR 36083: [3563] Fixing passthru case. The default values for Left
* Input to Right and Right Input to Left mixing coefficients needs to be
* 0.
* 
* Hydra_Software_Devel/71   10/22/07 12:04p sushmit
* PR 36132: [7405] VCXO_1 was getting used for Timebase1, moving that to
* VCXO_0
* 
* Hydra_Software_Devel/70   10/18/07 6:43p speter
* PR 36083: [3563] Fixing the SPDIF Output for Master Volume
* 
* Hydra_Software_Devel/69   10/18/07 2:14p speter
* PR 36083: [3563] Checking in the new Volume APIs related changes.
* 
* Hydra_Software_Devel/68   10/11/07 1:31p sushmit
* PR 36003: [7405] Bringing up Internal Capture for 7405
* 
* Hydra_Software_Devel/67   10/11/07 11:56a sushmit
* PR 36003: [7405] Fixing Compilation Warnings
* 
* Hydra_Software_Devel/66   9/30/07 1:59p sushmit
* PR35140: [7405] Develop 7325 Raptor PI for Bringup
* 
* Hydra_Software_Devel/65   9/28/07 10:47a speter
* PR 32625: [3563] Move volume amplification states from output port
* handle to rap handle
* 
* Hydra_Software_Devel/64   9/26/07 10:02a kagrawal
* PR 35330: [7440] [HiDef-Dvd 6004] Disabling/Enabling DATA_ENABLE for
* mute/unmute for I2s_multi1 in SetMute_isr()
* 
* Hydra_Software_Devel/63   9/19/07 10:11a sushmit
* PR 34648: [7405] Fixing Compilation warnings.
* 
* Hydra_Software_Devel/62   8/16/07 11:35p sushmit
* PR 27646: [7405] Fixing compilation break of 7405
* 
* Hydra_Software_Devel/61   8/3/07 2:58p kagrawal
* PR 33767: [7440] Programming TMG_SRC source for SPDIF1
* 
* Hydra_Software_Devel/60   8/3/07 1:25p bhanus
* PR 33759 : [7440] Fixed issue of configuring FS Timing Source for 44.1
* kHz content.
* 
* Hydra_Software_Devel/59   8/1/07 1:33p kagrawal
* PR 33698: [7440] Enabling channel pair mapping by crossbar programming
* for I2s5-I2s8
* 
* Hydra_Software_Devel/58   7/27/07 11:16a kagrawal
* PR 32986: [7440] Internally using the PLL0 for internal capport when an
* output port in the system doesn't use PLL (ex. HifiDAC)
* 
* Hydra_Software_Devel/57   7/24/07 11:34a speter
* PR 33241: [3563] In C0 the RDB has been fixed and hence the workaround
* that was implemented for the wrong RDB of the Crossbar register has to
* be removed.
* 
* Hydra_Software_Devel/56   7/19/07 8:29p kagrawal
* PR 32504: [7440] Adding a missing break in I2s8 switch-case
* 
* Hydra_Software_Devel/55   6/29/07 9:45p kagrawal
* PR 32504: [7440] Enabling I2s_mutli1 DATA_ENABLE even for HBR mode
* 
* Hydra_Software_Devel/54   6/28/07 9:28p kagrawal
* PR 32504: [7440] For I2s_multi1, HBR_ENABLE should be true and
* DATA_ENABLE should be false
* 
* Hydra_Software_Devel/53   6/22/07 8:18p gautamk
* PR30069 : Fixing the bug where CLOCK_ENABLE was getting disabled in
* BRAP_OP_P_SpdifStop
* 
* Hydra_Software_Devel/52   6/20/07 1:36p kagrawal
* PR 30188: [7440 B0] Reverted back the hack for L-R audio swap on stereo
* I2s. This needs correct RDB to be used -
* 7440\rdb\b0\bchp_aud_fmm_op_ctrl.h version 2 onwards
* 
* Hydra_Software_Devel/51   6/14/07 2:06p sushmit
* PR 27646: 7405 has two PLLs
* 
* Hydra_Software_Devel/50   6/12/07 10:02p sushmit
* PR 27646: Updating 7405 PI
* 
* Hydra_Software_Devel/49   6/11/07 9:02p nitinb
* PR 29166: Moved volume states from output handle to RAP handle. This
* removes restriction of calling add destination API before calling
* volume APIs.
* 
* Hydra_Software_Devel/48   6/11/07 6:59p nitinb
* PR 29166: Moved output port mute states from output handle to RAP
* handle. This removes restriction of calling add destination API before
* calling mute API.
* 
* Hydra_Software_Devel/47   6/9/07 5:33p bhanus
* PR 30069 : [7440 B0] Fixed issues for SPDIF1 (Hidef -DVD PR 4254)
* 
* Hydra_Software_Devel/46   6/8/07 3:46p sushmit
* PR 27646: 7405 Compilation Break
* 
* Hydra_Software_Devel/45   6/8/07 3:18p bhanus
* PR 30069 : [7440 B0] Added code for HDMI HBR Compressed Support.
* 
* Hydra_Software_Devel/44   6/7/07 3:30p sushmit
* PR 27646: 7405 Compilation break.
* 
* Hydra_Software_Devel/43   6/5/07 10:44a speter
* PR 31827: [3563] Merging from Phase 2.0 release branch to main
* 
* Hydra_Software_Devel/42   6/4/07 2:06p bhanus
* PR 30069 :[7440 B0] Added Support for SPDIF1 and I2SMulti0 as Mux
* Select for HDMI
* 
* Hydra_Software_Devel/41   5/28/07 1:33p bhanus
* PR 30069 : Fixed Coverity Issues.
* 
* Hydra_Software_Devel/RAP_3563_PH_2_0/1   5/28/07 4:19p nitinb
* PR 29166: Enabled DATA_ENABLE bits of SPDIF and MAI unconditionally in
* their Start Routines. Without this, SPDIF Mute -> Channel Stop ->
* Channel Start -> SPDIF Unmute sequence doesn't work. SPDIF remains
* muted. Similarly for MAI.
* 
* Hydra_Software_Devel/40   5/24/07 6:20p kagrawal
* PR 30069: [7440 B0] Added support for stereo output on multi-I2s1
* 
* Hydra_Software_Devel/39   5/24/07 2:36p sushmit
* PR 27646: 7405 Compilation & linking
* 
* Hydra_Software_Devel/38   5/23/07 5:52p nitinb
* PR 31498: Merge RAP PI development from phase 1.7 branch to main branch
* 
* Hydra_Software_Devel/RAP_3563_PH_1_7/2   5/15/07 3:16p speter
* PR 30168: [3563] Adding I2S0 support (in multi I2S0-3)
* 
* Hydra_Software_Devel/37   5/16/07 3:09p bhanus
* PR 30069 : Removed an Assert for Watchdog Recovery
* 
* Hydra_Software_Devel/36   5/14/07 10:13a kagrawal
* PR 30069: Adding RAP_P_ConfigureFsTimingSource()
* 
* Hydra_Software_Devel/35   5/11/07 8:58p sushmit
* PR 27646: Getting 7405 PI compiled
* 
* Hydra_Software_Devel/34   5/11/07 4:16p nitinb
* PR 30180: [3563] C0: Raptor: IOP outputs stale sample after stop,
* restart
* 
* Hydra_Software_Devel/33   5/11/07 12:29p kagrawal
* PR 30069: Added code for SPDIF1
* 
* Hydra_Software_Devel/32   5/9/07 5:38p kagrawal
* PR 30069: [7440] Added support for unconventional sampling rates
* - Deprecated the eSClkRate and eMClkRate params from I2s and SPDIF
* settings
* - Supported sampling rates : (32, 64, 128), (48, 96, 192) and (44.1,
* 88.2, 176.4)
* - Added BRAP_P_ConfigureStereoI2sMCLK(), BRAP_P_ConfigureMultiI2sMCLK()
* and BRAP_P_ConfigureSpdifMCLK()
* 
* Hydra_Software_Devel/31   5/4/07 11:58a sushmit
* PR 30069: Fixing 7440 compilation due to merge of 3563
* 
* Hydra_Software_Devel/30   5/3/07 4:14p speter
* PR 30400: [3563] Merged the changes done as part of Phase 1.6 Release
* to the main line. Merge happened from branch RAP_3563_PH_1_6 to
* Hydra_Software_Devel
* 
* Hydra_Software_Devel/29   4/27/07 5:26p gautamk
* PR30069: Fixing SrcCh and DstCh stop sequence limitation . And adding
* function I2sHwConfigMulti/I2sHwConfigConfig for B0
* 
* Hydra_Software_Devel/28   4/26/07 6:55p bhanus
* PR 30069 : Code changed for fixing the compilation error introduced by
* last check in for 3563.
* 
* Hydra_Software_Devel/27   4/25/07 12:34p nitinb
* PR 28187: Added field bVolIndB to output port handle
* 
* Hydra_Software_Devel/26   4/25/07 11:46a kagrawal
* PR 30186: Adding software work-around for audio swap issue for I2s4
* (stereo I2s). Replaced all In_r by In_l while setting up the channel
* order.
* 
* Hydra_Software_Devel/25   4/16/07 5:59p kagrawal
* PR 28561: Added hack for backward compatibility for SpdifCompressed
* mode of HDMI
* 
* Hydra_Software_Devel/24   4/16/07 9:46a kagrawal
* PR 28561: Reduced channel handle size by changing sPath[] to *pPath[]
* 
* Hydra_Software_Devel/23   4/9/07 8:19p sushmit
* PR 29166:
* Volume Change: Left & Right volume level stored in hOp instead of
* hMixer. SetVolume added during Op_start.
* Mute Status Change: Mute status stored properly in hOp. SetMute added
* during Op_Start.
* Set/Get BTSC Mode: Includes open & run time BTSC mode & config changes.
* Added BtscParams in Inputconfig & CapportParams.
* Tone Control changes: Modified so that these can be changed even before
* start.
* 
* Hydra_Software_Devel/22   4/6/07 9:43p bhanus
* PR 28561 : [7440 B0] Added code to support SPDIF as Mux Select for
* HDMI.
* 
* Hydra_Software_Devel/21   4/1/07 4:31a bhanus
* PR 27641 : Ported the changes from Base.
* 
* Hydra_Software_Devel/20   3/23/07 8:34p bhanus
* PR 28955 : [7440A0/3563] Modified the code for adding on-the-fly
* configuration for I2S output ports.
* 
* Hydra_Software_Devel/19   3/5/07 10:56a speter
* PR 27999: I2S Port Related Fix for Equalizer / Tone Controller
* 
* Hydra_Software_Devel/18   3/2/07 5:00p speter
* PR 27999: Equalizer Tone Control Fix
* 
* Hydra_Software_Devel/17   2/24/07 2:10p bhanus
* PR 28040 : Modified code for 7440 B0 porting
* 
* Hydra_Software_Devel/16   2/9/07 7:31p kagrawal
* PR 25607: Changes for HDMI compressed bring-up
* 
* Hydra_Software_Devel/15   2/7/07 4:47p speter
* PR 25346: [3563] Removed warnings for 0.5 release
* 
* Hydra_Software_Devel/14   1/23/07 1:10p sushmit
* PR 26864: Fixing CC issues
* 
* Hydra_Software_Devel/13   1/22/07 7:28p sushmit
* PR 26864: I2S OP bringup
* 
* Hydra_Software_Devel/12   1/12/07 8:29p sushmit
* PR 26864:
* 3563 A1 bringup. Fix for I2S Output
* 
* Hydra_Software_Devel/11   12/19/06 5:50p bselva
* PR 25346: Checking in the changes for 3563 new features
* 
* Hydra_Software_Devel/10   12/15/06 2:27p kagrawal
* PR 25607: Added proper support for HDMI output port
* 
* Hydra_Software_Devel/9   12/7/06 1:27p sushmit
* PR 25346: Merging 3563 to Hydra
* 
* Hydra_Software_Devel/8   12/4/06 1:52p kagrawal
* PR 25607: Bug fixes for stereo PCM over HDMI output port
* 
* Hydra_Software_Devel/7   11/29/06 11:13a bhanus
* PR 25607 : Modified for SRC bringup
* 
* Hydra_Software_Devel/6   11/10/06 11:11a kagrawal
* PR 25607: Added support for multi channel output on I2s_multi
* 
* Hydra_Software_Devel/5   11/9/06 11:41a bhanus
* PR 24717 : Added Support for PCM Playback.
* - Added BRAP_OP_P_ProgramOutputClock() to program Output port clock.
* 
* Hydra_Software_Devel/4   10/6/06 5:43p kagrawal
* PR 24717: Merged 7440 devel branch to Hydra_Software_Devel
* 
* Hydra_Software_Devel/3   9/20/06 2:14p kagrawal
* PR 20654: Fixed compilation warnings
* 
* Hydra_Software_Devel/2   9/12/06 11:49a kagrawal
* PR 20654: Delinked from base and added to base2
* 
***************************************************************************/
#include "brap.h"
#include "brap_priv.h"

BDBG_MODULE (rap_output);

/*TODO:
    1. Program Stream resolution.
    2. Add check to make sure that in case of Multi channel, multiple open calls
       should not overwrite the settings.
    3. Still to implement: MAI Can take input from Mixer directly in 7440 B0.
*/

#define BRAP_OP_P_DEFAULT_I2S_BITS_PER_SAMPLE   24
#define BRAP_OP_P_DEFAULT_MAI_BITS_PER_SAMPLE   20


#define BRAP_FIELD_ENUM(x,y,z) BCHP_FIELD_ENUM(x,y,z)
#define BRAP_FIELD_DATA(x,y,z) BCHP_FIELD_DATA(x,y,z)
#define BRAP_MASK(x,y) BCHP_MASK(x,y)


static const BRAP_OP_P_I2sSettings defI2sSettings = 
{   
    { 
        BRAP_OP_ChannelOrder_eNormal, /* sExtParams.eChannelOrder */
        false,                        /* sExtParams.bLsbAtLRClk */
#if (BRAP_3548_FAMILY == 1) 
        /* The I2S Out of 3548 wants it to be delayed with LR clk */
        false,                        /* sExtParams.bAlignedToLRClk */        
#else        
        true,                         /* sExtParams.bAlignedToLRClk */
#endif        
        false,                        /* sExtParams.bLRClkPolarity */
        BRAP_OP_SClkRate_e64Fs,       /* sExtParams.eSClkRate */
        BRAP_OP_MClkRate_e256Fs,      /* sExtParams.eMClkRate */
#if (BRAP_PLL1_SUPPORTED == 1)           
        BRAP_OP_Pll_ePll1,            /* sExtParams.ePll */
#else
        BRAP_OP_Pll_ePll0,            /* sExtParams.ePll */
#endif
        BRAP_OutputChannelPair_eLR    /* sExtParams.eMappedChPair */        
    } 
};
static const BRAP_OP_P_I2sParams defI2sParams = 
{   
    BRAP_OP_P_DEFAULT_I2S_BITS_PER_SAMPLE,  /* uiBitsPerSample */
    BAVC_Timebase_e0,                       /* eTimebase */
    BRAP_INVALID_VALUE,                     /* ui32InputFciId */
    false,                                  /* bIsMulti */
    false                                   /* bHbrEnable */
};
static const BRAP_OP_P_SpdifSettings defSpdifSettings = 
{
    { 
        BRAP_OP_ChannelOrder_eNormal, /* sExtParams.eChannelOrder */
        false,                        /* sExtParams.bLimitTo16Bits */
        BRAP_OP_MClkRate_e128Fs,      /* sExtParams.eMClkRate */
#if (BRAP_PLL1_SUPPORTED == 1)           
        BRAP_OP_Pll_ePll1,            /* sExtParams.ePll */
#else
        BRAP_OP_Pll_ePll0,            /* sExtParams.ePll */
#endif    
    }         
};
static const BRAP_OP_P_SpdifParams defSpdifParams = 
{
    BAVC_Timebase_e0,                       /* eTimebase */
    BRAP_INVALID_VALUE,                     /* ui32InputFciId */
    false                                   /* bHbrEnable */    
};

static const BRAP_OP_SpdifChanStatusParams defChanStatusParams =
{
    false,              /* bProfessionalMode = Consumer Mode */
    false,              /* bSwCopyRight = Not asserted */
    0,                  /* ui16CategoryCode = 0 */
    0,                  /* bSeparateLRChanNum = 0 =>Left=right channel num=0000*/
    0,                   /* ui16ClockAccuracy = 0 (Level II) */
    0,                   /*uiCGMS_A*/
    16                  /*ui16SpdifPcmWordWidth*/
};

static const BRAP_OP_P_DacParams defDacParams = 
{
    BAVC_Timebase_e0,                       /* eTimebase */
    BRAP_INVALID_VALUE,                     /* ui32InputFciId */
    false                                   /* bBtscOnDAC */
};

static const BRAP_OP_P_DacSettings defDacSettings = 
{
  {
    BRAP_OP_ChannelOrder_eNormal,   /* sExtParams.eChannelOrder */    
    BRAP_OP_DacMuteType_Drive_aaaa, /* sExtParams.eMuteType */
    0xcccc,                         /* sExtParams.ui32RightDacOutputVal */
    0x3333,                         /* sExtParams.ui32LeftDacOutputVal */    
    0x1CB80                         /* sExtParams.ui32Scale */
  }
};

static const BRAP_OP_P_MaiSettings defMaiSettings = 
{
  {
#if  (BRAP_3548_FAMILY == 1)    
	BRAP_OutputPort_eSpdif,
	BRAP_OP_ChannelOrder_eNormal, 
	BRAP_OP_MaiAudioFormat_eSpdifLinearPcm,
#else	
	BRAP_OutputPort_eMai,
	BRAP_OP_ChannelOrder_eNormal, 
	BRAP_OP_MaiAudioFormat_eSpdif2Channel,	
#endif	
	true
#if (BRAP_7550_FAMILY == 1)
    ,BRAP_OP_MClkRate_e256Fs        /* sExtParams.eMClkRate */
#elif ( (BRAP_7405_FAMILY == 1) || (BRAP_3548_FAMILY == 1) )
    ,BRAP_OP_Pll_ePll0             /* sExtParams.ePll */
#endif
  }
};

static const BRAP_OP_P_MaiParams defMaiParams = 
{   
    BRAP_OP_P_DEFAULT_MAI_BITS_PER_SAMPLE,  /* uiSampleWidth */
    BAVC_Timebase_e0,                       /* eTimebase */
    BRAP_INVALID_VALUE,                     /* ui32InputFciId */
    false                                   /* bHbrEnable */
};

static BERR_Code
BRAP_OP_P_SpdifOpen (
    BRAP_OP_P_Handle                hOp,            /*[in] Output port handle */
    const BRAP_OP_P_SpdifSettings   *pSettings      /*[in] Start time parameters */
);
static BERR_Code
BRAP_OP_P_SpdifStart (
    BRAP_OP_P_Handle                hOp,            /*[in] Output port handle */
    const BRAP_OP_P_SpdifParams     *pParams        /*[in] Start time parameters */
);
static BERR_Code
BRAP_OP_P_SpdifStop (
    BRAP_OP_P_Handle                hOp             /*[in] Output port handle */
);
#if (BRAP_7550_FAMILY !=1)
#if (BRAP_7420_FAMILY !=1)        
static BERR_Code
BRAP_OP_P_I2sOpen_Multi(
    BRAP_OP_P_Handle                hOp,            /*[in] Output port handle */
    const BRAP_OP_P_I2sSettings     *pSettings      /*[in] Open time parameters */
);
#endif
static BERR_Code
BRAP_OP_P_I2sOpen_Stereo(
    BRAP_OP_P_Handle                hOp,            /*[in] Output port handle */
    const BRAP_OP_P_I2sSettings     *pSettings      /*[in] Open time parameters */
);

#if ((BRAP_3548_FAMILY == 1) || (BRAP_7405_FAMILY == 1))
#if (BRAP_7420_FAMILY !=1)        
static BERR_Code
BRAP_OP_P_I2sStart_Multi(
    BRAP_OP_P_Handle                hOp,            /*[in] Output port handle */
    const BRAP_OP_P_I2sParams       *pParams        /*[in] Start time parameters */
);

static BERR_Code
BRAP_OP_P_I2sStop_Multi(
    BRAP_OP_P_Handle        hOp             /*[in] Output port handle */
);
#endif
#endif
static BERR_Code
BRAP_OP_P_I2sStart_Stereo(
    BRAP_OP_P_Handle                hOp,        /*[in] Output port handle */
    const BRAP_OP_P_I2sParams       *pParams    /*[in] Start time parameters */
);

static BERR_Code
BRAP_OP_P_I2sStop_Stereo(
    BRAP_OP_P_Handle        hOp             /*[in] Output port handle */
);
#endif
static BERR_Code
BRAP_OP_P_MaiOpen (
    BRAP_OP_P_Handle                hOp,            /*[in] Output port handle */
    const BRAP_OP_P_MaiSettings     *pSettings      /*[in] Start time parameters */
);

static BERR_Code
BRAP_OP_P_MaiStart (
    BRAP_OP_P_Handle                hOp,            /*[in] Output port handle */
    const BRAP_OP_P_MaiParams       *pParams        /*[in] Start time parameter */
);

#if ((BRAP_7420_FAMILY ==1) || (BRAP_7550_FAMILY ==1))
static BERR_Code
BRAP_OP_P_MaiMultiOpen (
    BRAP_OP_P_Handle                hOp,            /*[in] Output port handle */
    const BRAP_OP_P_MaiSettings     *pSettings      /*[in] Start time parameters */
);

static BERR_Code
BRAP_OP_P_MaiMultiClose (
    BRAP_OP_P_Handle        hOp              /*[in] Output port handle */
);


static BERR_Code
BRAP_OP_P_MaiMultiStart (
    BRAP_OP_P_Handle                hOp,            /*[in] Output port handle */
    const BRAP_OP_P_MaiParams       *pParams        /*[in] Start time parameter */
);

static BERR_Code
BRAP_OP_P_MaiMultiStop (
    BRAP_OP_P_Handle        hOp    /*[in] Output port handle */
);

#endif
static BERR_Code
BRAP_OP_P_MaiStop (
    BRAP_OP_P_Handle        hOp    /*[in] Output port handle */
);


BERR_Code
BRAP_OP_P_MaiSetMute (
    BRAP_Handle         hRap,		    /* [in] Audio Device Handle */
    BRAP_OutputPort     eOpType,	    /* [in] Output Type */
    bool                bMute			/* [in] True = Mute; false = unmute*/
);

BERR_Code
BRAP_OP_P_MaiSetMute_isr (
    BRAP_Handle         hRap,		/* [in] Audio Device Handle */
    BRAP_OutputPort     eOpType,	/* [in] Output Type */
    bool                bMute		/* [in] True = Mute; false = unmute*/
);

#if (BRAP_7550_FAMILY !=1)
/*Return Pllsettings*/
BERR_Code
BRAP_OP_Pll_GetSettings (
    BRAP_Handle         hRap,		/* [in] Audio Device Handle */
    BRAP_OP_Pll     	ePll,		/* [in] PLL Index */
    BRAP_OP_PllSettings *pSettings /* [out] PLL Settings*/
)
{

	BDBG_ENTER (BRAP_OP_Pll_GetSettings);
	BDBG_ASSERT(ePll < BRAP_OP_Pll_eMax);

	pSettings->uiVcxo = hRap->sPllSettings[ePll].uiVcxo;
	
	BDBG_LEAVE (BRAP_OP_Pll_GetSettings);
	return BERR_SUCCESS;

}

/*Set Pll settings*/
BERR_Code
BRAP_OP_Pll_SetSettings (
    BRAP_Handle         hRap,		/* [in] Audio Device Handle */
    BRAP_OP_Pll     	ePll,		/* [in] PLL Index */
    BRAP_OP_PllSettings *pSettings /* [out] PLL Settings*/
)
{
    BERR_Code   ret = BERR_SUCCESS;
	uint32_t	ui32RegValue = 0;
	uint32_t	ui32Offset=0;
	uint32_t 	data=0; 
 
	BDBG_ENTER (BRAP_OP_Pll_SetSettings);
	BDBG_ASSERT(ePll < BRAP_OP_Pll_eMax); 

	BDBG_MSG (("BRAP_OP_Pll_SetSettings(): handle=0x%08x, ePll=%d, VCX0=%d",
				hRap,ePll, pSettings->uiVcxo));
	hRap->sPllSettings[ePll].uiVcxo = pSettings->uiVcxo; 


	/* VCXO source */
	switch (pSettings->uiVcxo)
	{
#ifdef BCHP_AUD_FMM_PLL0_CONTROL_REFERENCE_SELECT_VCXO_0
		case 0:
			data = BCHP_AUD_FMM_PLL0_CONTROL_REFERENCE_SELECT_VCXO_0;
			break;
#endif

#ifdef BCHP_AUD_FMM_PLL0_CONTROL_REFERENCE_SELECT_VCX0_1
		case 1:
			data = BCHP_AUD_FMM_PLL0_CONTROL_REFERENCE_SELECT_VCX0_1;
			break;
#endif 

#ifdef BCHP_AUD_FMM_PLL0_CONTROL_REFERENCE_SELECT_VCXO_2
		case 2:
			data = BCHP_AUD_FMM_PLL0_CONTROL_REFERENCE_SELECT_VCXO_2;
			break;
#endif 

		default:
			BDBG_ERR(("Invalid VCXO: %u", pSettings->uiVcxo));
			return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

		    
#ifdef BCHP_AUD_FMM_PLL1_MACRO		    
	ui32Offset = (int)(ePll) * (BCHP_AUD_FMM_PLL1_MACRO - BCHP_AUD_FMM_PLL0_MACRO); 
#else
    ui32Offset = 0;
#endif    

    BKNI_EnterCriticalSection();
	switch ( ePll)
	{    
		case BRAP_OP_Pll_ePll0:
#ifdef BCHP_AUD_FMM_PLL1_MACRO			
		case BRAP_OP_Pll_ePll1:
#endif   
			ui32RegValue &= ~(BCHP_MASK (AUD_FMM_PLL0_CONTROL, REFERENCE_SELECT)); 
			ui32RegValue |= (BCHP_FIELD_DATA (AUD_FMM_PLL0_CONTROL, REFERENCE_SELECT, data));
    		break;

		default:
		    BDBG_ERR(("Invalid PLL : %u", ePll));
			break;
	}
    
	BRAP_Write32_isr(hRap->hRegister, BCHP_AUD_FMM_PLL0_CONTROL  + ui32Offset, ui32RegValue);
	BKNI_LeaveCriticalSection();

	BDBG_LEAVE (BRAP_OP_Pll_SetSettings);
	return ret;

}

#endif 

BERR_Code 
BRAP_OP_P_GetDefaultSpdifChanStatusParams ( 
    BRAP_OP_SpdifChanStatusParams *  pDefParams   /*[out] Pointer to memory where 
                                                          default parameters should 
                                                          be written */
)
{
    BDBG_ENTER (BRAP_OP_P_GetDefaultSpdifChanStatusParams);
    BDBG_ASSERT (pDefParams);      

    *pDefParams = defChanStatusParams;
    
    BDBG_LEAVE (BRAP_OP_P_GetDefaultSettings);
    return BERR_SUCCESS;
}


BERR_Code 
BRAP_OP_P_GetDefaultSettings ( 
    BRAP_OutputPort         eOutput,        /*[in] Ouput Port type */
    void *                  pDefSettings    /*[out] Pointer to memory where default
                                                    settings should be written */
)
{
    BERR_Code ret = BERR_SUCCESS;
    BDBG_ENTER (BRAP_OP_P_GetDefaultSettings);

    BDBG_ASSERT (pDefSettings);            
    
    switch (eOutput)
    {
        case BRAP_OutputPort_eSpdif:
        case BRAP_OutputPort_eSpdif1:
            *((BRAP_OP_P_SpdifSettings *)pDefSettings) = defSpdifSettings;
            break;
        case BRAP_OutputPort_eI2s0:
        case BRAP_OutputPort_eI2s1:
        case BRAP_OutputPort_eI2s2:
	    case BRAP_OutputPort_eI2s3:
	    case BRAP_OutputPort_eI2s4:
        case BRAP_OutputPort_eI2s5:
        case BRAP_OutputPort_eI2s6:
        case BRAP_OutputPort_eI2s7:
	    case BRAP_OutputPort_eI2s8:
            *((BRAP_OP_P_I2sSettings *)pDefSettings) = defI2sSettings;

            /* Prepare default eMappedChPair for I2s5-I2s8 */
            if(BRAP_OutputPort_eI2s5 == eOutput)
                ((BRAP_OP_P_I2sSettings *)pDefSettings)->sExtSettings.eMappedChPair = BRAP_OutputChannelPair_eLR;
            else if (BRAP_OutputPort_eI2s6 == eOutput)
                ((BRAP_OP_P_I2sSettings *)pDefSettings)->sExtSettings.eMappedChPair = BRAP_OutputChannelPair_eLRSurround;
            else if (BRAP_OutputPort_eI2s7 == eOutput)
                ((BRAP_OP_P_I2sSettings *)pDefSettings)->sExtSettings.eMappedChPair = BRAP_OutputChannelPair_eCentreLF;
            else if (BRAP_OutputPort_eI2s8 == eOutput)
                ((BRAP_OP_P_I2sSettings *)pDefSettings)->sExtSettings.eMappedChPair = BRAP_OutputChannelPair_eLRRear;
            else
                ((BRAP_OP_P_I2sSettings *)pDefSettings)->sExtSettings.eMappedChPair = BRAP_OutputChannelPair_eMax;            
            break;
	    case BRAP_OutputPort_eDac0:
        case BRAP_OutputPort_eDac1:
        case BRAP_OutputPort_eDac2:         
		    *((BRAP_OP_P_DacSettings *)pDefSettings) = defDacSettings;
		    break;
	    case BRAP_OutputPort_eMai:
        case BRAP_OutputPort_eMaiMulti0:
        case BRAP_OutputPort_eMaiMulti1:            
        case BRAP_OutputPort_eMaiMulti2:            
        case BRAP_OutputPort_eMaiMulti3:          
		    *((BRAP_OP_P_MaiSettings *)pDefSettings) = defMaiSettings;
		    break;
        default:
            BDBG_ERR (("BRAP_OP_P_GetDefaultSettings(): Output port type not supprted %d ",
                        eOutput));
            return BERR_TRACE (BERR_NOT_SUPPORTED);
    }

    BDBG_LEAVE (BRAP_OP_P_GetDefaultSettings);
    return ret;
}



BERR_Code 
BRAP_OP_P_GetDefaultParams ( 
    BRAP_OutputPort         eOutput,        /*[in] Ouput Port type */
    void *                  pDefParams      /*[out] Pointer to memory where default
                                                    settings should be written */     
)
{
    BERR_Code ret = BERR_SUCCESS;
    BDBG_ENTER (BRAP_OP_P_GetDefaultParams);

    BDBG_ASSERT (pDefParams);            
    
    switch (eOutput)
    {
        case BRAP_OutputPort_eSpdif:
        case BRAP_OutputPort_eSpdif1:
            *((BRAP_OP_P_SpdifParams *)pDefParams) = defSpdifParams;
            break;
        case BRAP_OutputPort_eI2s0:
        case BRAP_OutputPort_eI2s1:
        case BRAP_OutputPort_eI2s2:
	    case BRAP_OutputPort_eI2s3:
	    case BRAP_OutputPort_eI2s4:
        case BRAP_OutputPort_eI2s5:
        case BRAP_OutputPort_eI2s6:
	    case BRAP_OutputPort_eI2s7:
	    case BRAP_OutputPort_eI2s8:
            *((BRAP_OP_P_I2sParams *)pDefParams) = defI2sParams;
            break;
        case BRAP_OutputPort_eMai:
        case BRAP_OutputPort_eMaiMulti0:
        case BRAP_OutputPort_eMaiMulti1:            
        case BRAP_OutputPort_eMaiMulti2:            
        case BRAP_OutputPort_eMaiMulti3:             
            *((BRAP_OP_P_MaiParams *)pDefParams) = defMaiParams;
            break;
	    case BRAP_OutputPort_eDac0:
        case BRAP_OutputPort_eDac1:
        case BRAP_OutputPort_eDac2:         
		    *((BRAP_OP_P_DacParams *)pDefParams) = defDacParams;
		    break;
        default:
            BDBG_ERR (("BRAP_OP_P_GetDefaultParams(): Output port type not supprted %d",
                        eOutput));
            return BERR_TRACE (BRAP_ERR_OUPUT_PORT_NOT_SUPPORTED);
    }

    BDBG_LEAVE (BRAP_OP_P_GetDefaultParams);
    return ret;
}


BERR_Code BRAP_OP_P_Open (
    BRAP_FMM_P_Handle           hFmm,           /*[in] Parent FMM Handle */
    BRAP_OP_P_Handle            *phOp,          /*[out] Ouput Port handle */
    BRAP_OutputPort             eOutput,        /*[in] Ouput Port type */
    const void                  *pSettings      /*[in] Ouput Port settings */ 

)
{
    BRAP_OP_P_Handle        hOp= NULL;
    BERR_Code               ret = BERR_SUCCESS;
    uint32_t                ui32RegVal=0;
    BAVC_AudioSamplingRate  eOutputSR = BAVC_AudioSamplingRate_eUnknown;
    BRAP_OP_P_MaiSettings   sTemp1;
#if ((BRAP_3548_FAMILY != 1))
    uint32_t ui32StreamId =0;
#endif

    BDBG_ENTER (BRAP_OP_P_Open);

    /* 1. Check all input parameters to the function. Return error if
      - FMM handle is NULL
      - Given ouput type is invalid
      - Pointer to Settings structure is NULL
      - Pointer to Ouput Port handle is NULL     */
    BDBG_ASSERT (hFmm);
    BDBG_ASSERT (phOp);

    eOutputSR = hFmm->hRap->sOutputSettings[eOutput].eOutputSR;
    if (BRAP_FMM_P_GetWatchdogRecoveryFlag (hFmm) == false)
    {   /* If not in WatchDog recovery */  
    
        BDBG_ASSERT (pSettings);

        if (hFmm->hOp[eOutput] != NULL) 
        {
            /* This Outport was previoulsy opened by another audio channel.
            So this time, just increment the open count. Dont change the 
            settings etc. */
            /* OpenCount cannot exceed the no. of Mixer inputs!! */
            hOp = hFmm->hOp[eOutput];
        
            if (hOp->uiOpenCnt >= BRAP_RM_P_MAX_MIXER_INPUTS)
            {
                BDBG_ERR (("BRAP_OP_P_Open: Output port %d already has %d open"
                            "audio channels. Cannot add another!!!",
                            eOutput, hOp->uiOpenCnt));
                return BERR_TRACE (BERR_NOT_SUPPORTED);
            }
        
            *phOp = hOp;
            hOp->uiOpenCnt ++;
            BDBG_MSG (("BRAP_OP_P_Open: Output port %d was already open." 
                        "New open count = %d", eOutput, hOp->uiOpenCnt));
            BDBG_LEAVE (BRAP_OP_P_Open);
            return ret;
        }
    
        /* Check if this output port type is supported */
        ret = BRAP_RM_P_IsOutputPortSupported(eOutput);
    	if( ret != BERR_SUCCESS)
    	{
    		BDBG_ERR(("BRAP_OP_P_Open: eOutputPort(%d) is not supported",
    			        eOutput));
    		return BERR_TRACE(ret);
    	}        

        /* 2. Allocate memory for the Output port handle. Fill in parameters */
    
        /* Allocate handle */
        hOp = (BRAP_OP_P_Handle) BKNI_Malloc (sizeof(BRAP_OP_P_Object));
        if (hOp == NULL)
        {
            return BERR_TRACE (BERR_OUT_OF_SYSTEM_MEMORY);
        }
        
        /* Clear the handle memory */
        BKNI_Memset(hOp, 0, sizeof(BRAP_OP_P_Object));

        /* Initialise known elements in the handle */
        hOp->hChip = hFmm->hChip;
        hOp->hRegister = hFmm->hRegister;
        hOp->hHeap = hFmm->hHeap;
        hOp->hInt = hFmm->hInt;
        hOp->hFmm = hFmm;
        hOp->eOutputPort = eOutput;
        hOp->ui32Offset = 0;
        hOp->uiOpenCnt = 1;
        hOp->uiStartCnt = 0;
        hOp->eSamplingRate = eOutputSR;
        hOp->bRfmodMuteState = false;
        hOp->eRfmodMuxSelect = BRAP_OutputPort_eMax;
    }   /* End: If not in WatchDog recovery */
    else
    {
        hOp = *phOp;
    }    

    /* Set WAIT_FOR_VALIDX to KeepRequesting for all. 
       For debug purposes, clear the appropriate IGNORE_FIRST_UNDERFLOWx bit in
       AUD_FMM_OP_CTRL_STREAM_CFG in order see underflows when data has never
       been valid. */

    switch (eOutput)
    {
        case BRAP_OutputPort_eSpdif:
            ret = BRAP_OP_P_SpdifOpen (hOp, (BRAP_OP_P_SpdifSettings *) pSettings);
#if ((BRAP_3548_FAMILY == 1))
	        ui32RegVal = BRAP_Read32 (hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_STREAM_CFG);
            ui32RegVal &= ~(BCHP_MASK (AUD_FMM_OP_CTRL_STREAM_CFG, IGNORE_FIRST_UNDERFLOW0));
            ui32RegVal &= ~(BCHP_MASK (AUD_FMM_OP_CTRL_STREAM_CFG, WAIT_FOR_VALID0));
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_STREAM_CFG, 
                                    IGNORE_FIRST_UNDERFLOW0, Ignore));
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_STREAM_CFG, 
                                    WAIT_FOR_VALID0, Keep_requesting));
  	        BRAP_Write32 (hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_STREAM_CFG, ui32RegVal);
#else
            ui32StreamId = 0;
#endif
            break;
#if (BRAP_7550_FAMILY != 1)
#if (BRAP_7420_FAMILY != 1)
        case BRAP_OutputPort_eI2s0:
			/*	On 3563 Ref Board, I2S_Multi 0-3 were initially not connected. Then after a request from Beijing team,
			   	only I2S(0) is connected through GPIOs. So in 3563 we will be able to support this I2S(0) of I2S(0-3) in 
			   	multi */
            ret = BRAP_OP_P_I2sOpen_Multi(hOp, (BRAP_OP_P_I2sSettings *) pSettings);
#if ((BRAP_3548_FAMILY == 1))
	        ui32RegVal = BRAP_Read32 (hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_STREAM_CFG);
            ui32RegVal &= ~(BCHP_MASK (AUD_FMM_OP_CTRL_STREAM_CFG, IGNORE_FIRST_UNDERFLOW3));
            ui32RegVal &= ~(BCHP_MASK (AUD_FMM_OP_CTRL_STREAM_CFG, WAIT_FOR_VALID3));
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_STREAM_CFG, 
                                    IGNORE_FIRST_UNDERFLOW3, Ignore));
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_STREAM_CFG, 
                                    WAIT_FOR_VALID3, Keep_requesting));
	        BRAP_Write32 (hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_STREAM_CFG, ui32RegVal);
#else
            ui32StreamId = 3;
#endif
            break;
        case BRAP_OutputPort_eI2s1:
            ret = BRAP_OP_P_I2sOpen_Multi(hOp, (BRAP_OP_P_I2sSettings *) pSettings);
#if ((BRAP_3548_FAMILY == 1))
	        ui32RegVal = BRAP_Read32 (hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_STREAM_CFG);
            ui32RegVal &= ~(BCHP_MASK (AUD_FMM_OP_CTRL_STREAM_CFG, IGNORE_FIRST_UNDERFLOW4));
            ui32RegVal &= ~(BCHP_MASK (AUD_FMM_OP_CTRL_STREAM_CFG, WAIT_FOR_VALID4));
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_STREAM_CFG, 
                                    IGNORE_FIRST_UNDERFLOW4, Ignore));
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_STREAM_CFG, 
                                    WAIT_FOR_VALID4, Keep_requesting));
  	        BRAP_Write32 (hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_STREAM_CFG, ui32RegVal);
#else
            ui32StreamId = 4;
#endif
            break;

        case BRAP_OutputPort_eI2s2:
            ret = BRAP_OP_P_I2sOpen_Multi(hOp, (BRAP_OP_P_I2sSettings *) pSettings);
#if ((BRAP_3548_FAMILY == 1))
	        ui32RegVal = BRAP_Read32 (hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_STREAM_CFG);
            ui32RegVal &= ~(BCHP_MASK (AUD_FMM_OP_CTRL_STREAM_CFG, IGNORE_FIRST_UNDERFLOW5));
            ui32RegVal &= ~(BCHP_MASK (AUD_FMM_OP_CTRL_STREAM_CFG, WAIT_FOR_VALID5));
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_STREAM_CFG, 
                                    IGNORE_FIRST_UNDERFLOW5, Ignore));
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_STREAM_CFG, 
                                    WAIT_FOR_VALID5, Keep_requesting));
  	        BRAP_Write32 (hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_STREAM_CFG, ui32RegVal);
#else
            ui32StreamId = 5;
#endif
            break;

	    case BRAP_OutputPort_eI2s3:
            ret = BRAP_OP_P_I2sOpen_Multi(hOp, (BRAP_OP_P_I2sSettings *) pSettings);
#if ((BRAP_3548_FAMILY == 1))
	        ui32RegVal = BRAP_Read32 (hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_STREAM_CFG);
            ui32RegVal &= ~(BCHP_MASK (AUD_FMM_OP_CTRL_STREAM_CFG, IGNORE_FIRST_UNDERFLOW6));
            ui32RegVal &= ~(BCHP_MASK (AUD_FMM_OP_CTRL_STREAM_CFG, WAIT_FOR_VALID6));
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_STREAM_CFG, 
                                    IGNORE_FIRST_UNDERFLOW6, Ignore));
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_STREAM_CFG, 
                                    WAIT_FOR_VALID6, Keep_requesting));
  	        BRAP_Write32 (hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_STREAM_CFG, ui32RegVal);
#else
            ui32StreamId = 6;
#endif
            break;
            
    	case BRAP_OutputPort_eI2s4:
            ret = BRAP_OP_P_I2sOpen_Stereo(hOp, (BRAP_OP_P_I2sSettings *) pSettings);
#if ((BRAP_3548_FAMILY == 1))
	        ui32RegVal = BRAP_Read32 (hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_STREAM_CFG);
            ui32RegVal &= ~(BCHP_MASK (AUD_FMM_OP_CTRL_STREAM_CFG, IGNORE_FIRST_UNDERFLOW7));
            ui32RegVal &= ~(BCHP_MASK (AUD_FMM_OP_CTRL_STREAM_CFG, WAIT_FOR_VALID7));
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_STREAM_CFG, 
                                    IGNORE_FIRST_UNDERFLOW7, Ignore));
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_STREAM_CFG, 
                                    WAIT_FOR_VALID7, Keep_requesting));
  	        BRAP_Write32 (hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_STREAM_CFG, ui32RegVal);
#else
            ui32StreamId = 7;
#endif
            break;

#else /* for BRAP_7420_FAMILY */
            case BRAP_OutputPort_eI2s0:
                ret = BRAP_OP_P_I2sOpen_Stereo(hOp, (BRAP_OP_P_I2sSettings *) pSettings);
                ui32StreamId = 7;
                break;

            case BRAP_OutputPort_eI2s1:
                ret = BRAP_OP_P_I2sOpen_Stereo(hOp, (BRAP_OP_P_I2sSettings *) pSettings);
                ui32StreamId = 8;
                break;
#endif
#endif

	    case BRAP_OutputPort_eDac0:
#if (BRAP_3548_FAMILY == 1)   
	    case BRAP_OutputPort_eDac1:    
	    case BRAP_OutputPort_eDac2:        
#elif (BRAP_7420_FAMILY ==1)      
	    case BRAP_OutputPort_eDac1:         
#endif                     
            /* TODO:Find out the DAC register offset */ 
            /* TBD later
            ret = BRAP_OP_P_DacOpen (hOp, (BRAP_OP_P_DacSettings *) pSettings);
            ui32RegVal &= ~(BCHP_MASK (AUD_FMM_OP_CTRL_STREAM_CFG, IGNORE_FIRST_UNDERFLOW1));
            ui32RegVal &= ~(BCHP_MASK (AUD_FMM_OP_CTRL_STREAM_CFG, WAIT_FOR_VALID1));
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_STREAM_CFG, 
                                    IGNORE_FIRST_UNDERFLOW1, Dont_ignore));
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_STREAM_CFG, 
                                    WAIT_FOR_VALID1, Keep_requesting));*/
            BRAP_OP_P_GetDacOffset(eOutput,&(hOp->ui32Offset));
            ret = BRAP_OP_P_DacOpen (hOp, (BRAP_OP_P_DacSettings *) pSettings);
            break;

        case BRAP_OutputPort_eMai:
            ret = BRAP_OP_P_MaiOpen (hOp, &sTemp1);
            break;
#if (BRAP_7420_FAMILY ==1)||(BRAP_7550_FAMILY ==1)                 
        case BRAP_OutputPort_eMaiMulti0:
        case BRAP_OutputPort_eMaiMulti1:            
        case BRAP_OutputPort_eMaiMulti2:            
        case BRAP_OutputPort_eMaiMulti3:                        
            ret = BRAP_OP_P_MaiMultiOpen (hOp, &sTemp1);
            break;            
#endif            
#if ((BRAP_3548_FAMILY != 1)&&(BRAP_7405_FAMILY != 1))
        case BRAP_OutputPort_eI2s5:
        case BRAP_OutputPort_eI2s6:
        case BRAP_OutputPort_eI2s7:
        case BRAP_OutputPort_eI2s8:
            ret = BRAP_OP_P_I2sOpen_Multi(hOp, (BRAP_OP_P_I2sSettings *) pSettings);
            ui32StreamId = 8;
            break;
        case BRAP_OutputPort_eSpdif1:
            hOp->ui32Offset = (BCHP_AUD_FMM_OP_CTRL_SPDIF_CFG_1 - 
                                               BCHP_AUD_FMM_OP_CTRL_SPDIF_CFG_0);
            ret = BRAP_OP_P_SpdifOpen (hOp, (BRAP_OP_P_SpdifSettings *) pSettings);
            ui32StreamId = 1;
            break;
#endif

        default:
            BDBG_ERR(("BRAP_OP_P_Open(): Incorrect Output port type %d",
                        eOutput));
            /* Free the Handle memory*/
            BKNI_Free (hOp);   
            return BERR_TRACE (BRAP_ERR_OUPUT_PORT_NOT_SUPPORTED);
    }

#if ((BRAP_3548_FAMILY != 1))
    ui32RegVal = BRAP_Read32 (hOp->hRegister, 
      (BCHP_AUD_FMM_OP_CTRL_STREAM_CFGi_ARRAY_BASE +(ui32StreamId *4)));
    ui32RegVal &= ~(BCHP_MASK (AUD_FMM_OP_CTRL_STREAM_CFGi, IGNORE_FIRST_UNDERFLOW));
    ui32RegVal &= ~(BCHP_MASK (AUD_FMM_OP_CTRL_STREAM_CFGi, WAIT_FOR_VALID));
    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_STREAM_CFGi, 
                            IGNORE_FIRST_UNDERFLOW, Ignore));
    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_STREAM_CFGi, 
                            WAIT_FOR_VALID, Keep_requesting));

    BRAP_Write32 (hOp->hRegister,
        (BCHP_AUD_FMM_OP_CTRL_STREAM_CFGi_ARRAY_BASE +(ui32StreamId *4)),
                                                            ui32RegVal);
#endif
    
    if (BRAP_FMM_P_GetWatchdogRecoveryFlag (hFmm) == false)
    {   /* If not in WatchDog recovery */  
        /* Store the Ouput object handle inside the FMM handle */
        hFmm->hOp[eOutput] = hOp;

        *phOp = hOp;
    } 
    
    BDBG_LEAVE (BRAP_OP_P_Open);
    return ret;

}
#if(BRAP_3548_FAMILY != 1)
BERR_Code BRAP_OP_P_GetStreamIdForOp(BRAP_OutputPort  eOutputPort,unsigned int *uiStreamId)
{
#if (BRAP_7420_FAMILY ==1)||(BRAP_7550_FAMILY ==1)
    switch(eOutputPort)
    {
                case BRAP_OutputPort_eDac0:
                    *uiStreamId = 2;             
                    break;
#if (BRAP_7550_FAMILY !=1)                
#ifdef BCHP_HIFIDAC_CTRL1_REVISION 
                case BRAP_OutputPort_eDac1:
                    *uiStreamId = 9;              
                    break;
#endif                
                case BRAP_OutputPort_eI2s0:
                    *uiStreamId = 7;
                    break;

                case BRAP_OutputPort_eI2s1:
                    *uiStreamId = 8;
                    break;
#endif
                case BRAP_OutputPort_eSpdif:
                    *uiStreamId = 0;                
                    break;
                case BRAP_OutputPort_eMai:
                    *uiStreamId = 1;
                    break;
                case BRAP_OutputPort_eMaiMulti0:
                    *uiStreamId = 3;          
                    break;                
                case BRAP_OutputPort_eMaiMulti1:
                    *uiStreamId = 4;                         
                    break;                
                case BRAP_OutputPort_eMaiMulti2:
                    *uiStreamId = 5;                        
                    break;                
                case BRAP_OutputPort_eMaiMulti3:
                    *uiStreamId = 6;                         
                    break;                                
                default:
                    BDBG_ERR(("BRAP_OP_P_GetStreamIdForOp(): Incorrect Output port type %d",
                                eOutputPort));
                    return BERR_TRACE (BERR_INVALID_PARAMETER);
                    break;
            }
#else
            switch(eOutputPort)
            {
                        case BRAP_OutputPort_eDac0:
                            *uiStreamId = 2;                                       
                            break;
#if (BRAP_3548_FAMILY == 1)
                        case BRAP_OutputPort_eDac1:
                            *uiStreamId = 8;                
                            break;
                        case BRAP_OutputPort_eDac2:
                            *uiStreamId = 9;                
                            break;                
#endif
                        case BRAP_OutputPort_eI2s0:
                            *uiStreamId = 3;              
                            break;
                        case BRAP_OutputPort_eI2s1:
                            *uiStreamId = 4;
                            break;
                        case BRAP_OutputPort_eI2s2:
                            *uiStreamId = 5;
                            break;
                        case BRAP_OutputPort_eI2s3:
                            *uiStreamId = 6;
                            break;
                        case BRAP_OutputPort_eI2s4:
                            *uiStreamId = 7;
                            break;
                        case BRAP_OutputPort_eSpdif:
                            *uiStreamId = 0;                
                            break;

                        case BRAP_OutputPort_eMai:
                            *uiStreamId = 1;
                            break;
                        default:
                            BDBG_ERR(("BRAP_OP_P_GetStreamIdForOp(): Incorrect Output port type %d",
                                        eOutputPort));
                            return BERR_TRACE (BERR_INVALID_PARAMETER);
                            break;
                    }
#endif
return BERR_SUCCESS;
}
#endif
BERR_Code
BRAP_OP_P_Close (
    BRAP_OP_P_Handle        hOp        /*[in] Output port handle */
)                
{
    BERR_Code ret = BERR_SUCCESS;
    BDBG_ENTER (BRAP_OP_P_Close);
    BDBG_ASSERT (hOp);
       
    if (hOp->uiOpenCnt == 0)
    { 
        /* This should never happen. If it does, it means the system
           has gone into an invalid state!!!!*/
        BDBG_ERR (("BRAP_OP_P_Open: Output port %d has NO open"
                        "audio channels!!!", hOp->eOutputPort));
        return BERR_TRACE (BERR_NOT_SUPPORTED);
    }
    hOp->uiOpenCnt --;

    switch (hOp->eOutputPort)
    {
#if (BRAP_7420_FAMILY ==1)||(BRAP_7550_FAMILY ==1)                 
        case BRAP_OutputPort_eMaiMulti0:
        case BRAP_OutputPort_eMaiMulti1:            
        case BRAP_OutputPort_eMaiMulti2:            
        case BRAP_OutputPort_eMaiMulti3:                        
            ret = BRAP_OP_P_MaiMultiClose (hOp);
        break;                 
#endif                
    default:
        break;

    }


    if (hOp->uiOpenCnt == 0)
    {   
        /* If this was the last open audio channel on this output port,
           free the handle etc */
        /* Remove referrence to this Output port from the parent FMM */ 
        hOp->hFmm->hOp[hOp->eOutputPort] = NULL;

        BDBG_MSG (("BRAP_OP_P_Close: Output port %d closed. Freeing Handle",
                                                            hOp->eOutputPort));
        /* Free the Output port Handle memory*/
        BKNI_Free (hOp); 
    }             

    BDBG_LEAVE (BRAP_OP_P_Close);
    return ret;
}


BERR_Code
BRAP_OP_P_Start (
    BRAP_OP_P_Handle        hOp,            /*[in] Output port handle */
    const void              *pParams        /*[in] Output Port start time 
                                                   parameters. Typecast it to proper
                                                   setting structure based on the 
                                                   port type */ 
)
{
    BERR_Code               ret = BERR_SUCCESS;
    BRAP_OP_P_MaiParams     *pMaiParams=NULL;
    
    BDBG_ENTER (BRAP_OP_P_Start);
    BDBG_ASSERT (hOp);

    /* Make sure new port is configured */ 
    if(hOp->hFmm->hRap->bOpSettingsValid[hOp->eOutputPort] == false) 
    {
        BDBG_ERR(("BRAP_OP_P_Start: Output port %d is not configured."
            "Please configure before calling this PI.", hOp->eOutputPort));
        return BERR_TRACE(BRAP_ERR_OUTPUT_NOT_CONFIGURED);
    } 
    

    if (BRAP_FMM_P_GetWatchdogRecoveryFlag (hOp->hFmm) == false)
    {   
        /* If not in WatchDog recovery */  
    
        BDBG_ASSERT (pParams);

        if (hOp->uiStartCnt > 0)
        {
            /* This Outport was previoulsy started by another audio channel.
            So this time, just increment the start count. Dont change the 
            settings etc. */
            /* start Count cannot exceed the open count!! */
            if (hOp->uiStartCnt >= hOp->uiOpenCnt)
            {
                BDBG_ERR (("BRAP_OP_P_Start: Cannot start Output port %d. "
                        "Start Count (%d) >= Open Count (%d)",
                        hOp->eOutputPort, hOp->uiStartCnt, hOp->uiOpenCnt));
                return BERR_TRACE (BERR_NOT_SUPPORTED);
            }
        
            hOp->uiStartCnt ++;
            BDBG_MSG (("BRAP_OP_P_Start: Output port %d was already started." 
                        "New start count = %d", 
                        hOp->eOutputPort, hOp->uiStartCnt));

            BDBG_LEAVE (BRAP_OP_P_Start);
            return ret;
        }
    }
    
    /* Start the o/p port operation */
    switch (hOp->eOutputPort)
    {
        case BRAP_OutputPort_eSpdif:
#if ((BRAP_3548_FAMILY != 1)&&(BRAP_7405_FAMILY != 1))
        case BRAP_OutputPort_eSpdif1:
#endif
            ret = BRAP_OP_P_SpdifStart (hOp, (BRAP_OP_P_SpdifParams *) pParams);
            break;
#if (BRAP_7550_FAMILY !=1)            
#if (BRAP_7420_FAMILY !=1)      
        case BRAP_OutputPort_eI2s0:
        case BRAP_OutputPort_eI2s1:
        case BRAP_OutputPort_eI2s2:
	    case BRAP_OutputPort_eI2s3:
#if ((BRAP_3548_FAMILY != 1)&&(BRAP_7405_FAMILY != 1))
        case BRAP_OutputPort_eI2s5:
        case BRAP_OutputPort_eI2s6:
        case BRAP_OutputPort_eI2s7:
        case BRAP_OutputPort_eI2s8:
#endif
            ret = BRAP_OP_P_I2sStart_Multi(hOp, (BRAP_OP_P_I2sParams *) pParams);
            break;
	    case BRAP_OutputPort_eI2s4:
            ret = BRAP_OP_P_I2sStart_Stereo(hOp, (BRAP_OP_P_I2sParams *) pParams);
            break;
#else /* For BRAP_7420_FAMILY */
        case BRAP_OutputPort_eI2s0:
        case BRAP_OutputPort_eI2s1:
            ret = BRAP_OP_P_I2sStart_Stereo(hOp, (BRAP_OP_P_I2sParams *) pParams);
            break;
#endif
#endif
        case BRAP_OutputPort_eDac0:
        case BRAP_OutputPort_eDac1:
        case BRAP_OutputPort_eDac2:            
            ret = BRAP_OP_P_DacStart (hOp, (BRAP_OP_P_DacParams *) pParams);
	        break;

        case BRAP_OutputPort_eMai:
            pMaiParams = (BRAP_OP_P_MaiParams *) pParams;
            ret = BRAP_OP_P_MaiStart (hOp, (BRAP_OP_P_MaiParams *)pParams);
            break;                                                              
#if (BRAP_7420_FAMILY ==1)||(BRAP_7550_FAMILY ==1)                 
        case BRAP_OutputPort_eMaiMulti0:
        case BRAP_OutputPort_eMaiMulti1:
        case BRAP_OutputPort_eMaiMulti2:
        case BRAP_OutputPort_eMaiMulti3:            
            pMaiParams = (BRAP_OP_P_MaiParams *) pParams;
            ret = BRAP_OP_P_MaiMultiStart (hOp, (BRAP_OP_P_MaiParams *)pParams);
            break;             
#endif            
        default:
            BDBG_ERR (("BRAP_OP_P_Start(): Output port type %d not supported",
                        hOp->eOutputPort));
            return BERR_TRACE (BERR_NOT_SUPPORTED);
    }

    if ((BRAP_FMM_P_GetWatchdogRecoveryFlag (hOp->hFmm) == false)&& (ret == BERR_SUCCESS))
    {   /* If not in WatchDog recovery */  
        hOp->uiStartCnt ++;
    }

	ret = BRAP_OP_P_SetMute(hOp->hFmm->hRap, hOp->eOutputPort, hOp->hFmm->hRap->bOutputMute[hOp->eOutputPort]);
    if(ret != BERR_SUCCESS)
    {
        BDBG_ERR (("BRAP_OP_P_Start(): BRAP_OP_P_SetMute_isr returned error %d", ret));
        return BERR_TRACE (BERR_NOT_SUPPORTED);
    }

        if(hOp->hFmm->hRap->sOutputSettings[hOp->eOutputPort].bCompressed == false)
        {
		/* Volume in units of dB */
		ret = BRAP_P_SetOutputVolumeGain(hOp->hFmm->hRap, hOp->eOutputPort, 
				hOp->hFmm->hRap->uiOutputLVolumeGain[hOp->eOutputPort], 
				hOp->hFmm->hRap->uiOutputRVolumeGain[hOp->eOutputPort]);        
	    if(ret != BERR_SUCCESS)
	    {
	        BDBG_ERR (("BRAP_OP_P_Start(): BRAP_SetOutputVolume returned error %d", ret));
	        return BERR_TRACE (BERR_NOT_SUPPORTED);
	    }
        }

    
    BDBG_LEAVE (BRAP_OP_P_Start);
    return ret;
}

BERR_Code
BRAP_OP_P_Stop (
    BRAP_OP_P_Handle        hOp        /*[in] Output port handle */
)
{
    BERR_Code               ret = BERR_SUCCESS;
#if(BRAP_3548_FAMILY != 1)
    unsigned int uiStreamId=0,ui32RegVal=0;    
#endif
    BDBG_ENTER (BRAP_OP_P_Stop);
    BDBG_ASSERT (hOp);
   
        
    if (hOp->uiStartCnt == 0)
    { 
            BDBG_WRN (("BRAP_OP_P_Stop: Output port %d has NO active"
                        "audio channels. Ignoring this call to _Stop()!",
                        hOp->eOutputPort));
            BDBG_LEAVE (BRAP_OP_P_Stop);
            return ret;
    }
    hOp->uiStartCnt --;

    BDBG_MSG (("BRAP_OP_P_Stop: Output port %d new start count =%d",
                hOp->eOutputPort, hOp->uiStartCnt));
    if (hOp->uiStartCnt != 0)
    {
            BDBG_LEAVE (BRAP_OP_P_Stop);
            return ret;
    }
    
#if(BRAP_3548_FAMILY != 1)
    ret = BRAP_OP_P_GetStreamIdForOp(hOp->eOutputPort,&uiStreamId);
    if(ret == BERR_SUCCESS)
    ui32RegVal = BRAP_Read32(hOp->hRegister , 
                (BCHP_AUD_FMM_IOP_CTRL_FCI_CFGi_ARRAY_BASE + (uiStreamId*4)));
    
    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_IOP_CTRL_FCI_CFGi, ID));
    ui32RegVal |= BCHP_FIELD_DATA(AUD_FMM_IOP_CTRL_FCI_CFGi, ID,0x3FF);
    
    BRAP_Write32(hOp->hRegister , (BCHP_AUD_FMM_IOP_CTRL_FCI_CFGi_ARRAY_BASE + 
                            (uiStreamId*4)),ui32RegVal);
    ret =BERR_SUCCESS;
#endif  

    /* Stop the o/p port operation */
    switch (hOp->eOutputPort)
    {
         case BRAP_OutputPort_eSpdif:
#if ((BRAP_3548_FAMILY != 1)&&(BRAP_7405_FAMILY != 1))
        case BRAP_OutputPort_eSpdif1:
#endif
            ret = BRAP_OP_P_SpdifStop(hOp);
            break;
#if (BRAP_7550_FAMILY !=1)            
#if (BRAP_7420_FAMILY !=1)     
        case BRAP_OutputPort_eI2s0:
        case BRAP_OutputPort_eI2s1:
        case BRAP_OutputPort_eI2s2:
    	case BRAP_OutputPort_eI2s3:
#if ((BRAP_3548_FAMILY != 1)&&(BRAP_7405_FAMILY != 1))
        case BRAP_OutputPort_eI2s5:
        case BRAP_OutputPort_eI2s6:
        case BRAP_OutputPort_eI2s7:
        case BRAP_OutputPort_eI2s8:
#endif
            ret = BRAP_OP_P_I2sStop_Multi(hOp);
            break;

	    case BRAP_OutputPort_eI2s4:
            ret = BRAP_OP_P_I2sStop_Stereo(hOp);
            break;
#else
        case BRAP_OutputPort_eI2s0:
        case BRAP_OutputPort_eI2s1:
            ret = BRAP_OP_P_I2sStop_Stereo(hOp);
            break;
#endif
#endif
        case BRAP_OutputPort_eDac0:
        case BRAP_OutputPort_eDac1:
        case BRAP_OutputPort_eDac2:            
            ret = BRAP_OP_P_DacStop (hOp);
	        break;
         
        case BRAP_OutputPort_eMai:
            ret = BRAP_OP_P_MaiStop (hOp);
            break;
#if ((BRAP_7420_FAMILY ==1)||(BRAP_7550_FAMILY ==1))
        case BRAP_OutputPort_eMaiMulti0:
        case BRAP_OutputPort_eMaiMulti1:            
        case BRAP_OutputPort_eMaiMulti2:            
        case BRAP_OutputPort_eMaiMulti3:                        
            ret = BRAP_OP_P_MaiMultiStop (hOp);
            break;
#endif            
        default:
            BDBG_ERR (("BRAP_OP_P_Stop(): Incorrect Output port type %d", hOp->eOutputPort));
            return BERR_TRACE (BERR_NOT_SUPPORTED);
    }    
    BDBG_LEAVE (BRAP_OP_P_Stop);
    return ret;
}

static BERR_Code
BRAP_OP_P_SpdifOpen (
    BRAP_OP_P_Handle        hOp,              /*[in] Output port handle */
    const BRAP_OP_P_SpdifSettings * pSettings /*[in] Start time parameters */
)
{
    uint32_t ui32RegVal = 0;
    BDBG_ENTER (BRAP_OP_P_SpdifOpen);
    BDBG_ASSERT (hOp);
    BSTD_UNUSED(pSettings);

    /* Enable the clock while opening the output port. Never disable it */
    ui32RegVal = BRAP_Read32 (hOp->hRegister, 
               BCHP_AUD_FMM_OP_CTRL_SPDIF_CFG_0 + hOp->ui32Offset); 
    
	ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_SPDIF_CFG_0,CLOCK_ENABLE));  
    ui32RegVal |= (BRAP_FIELD_ENUM(AUD_FMM_OP_CTRL_SPDIF_CFG_0,  
                                    CLOCK_ENABLE, Enable));

#if (BRAP_7405_FAMILY == 1)        
	ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_SPDIF_CFG_0,DATA_ENABLE));  
    ui32RegVal |= (BRAP_FIELD_ENUM(AUD_FMM_OP_CTRL_SPDIF_CFG_0,  
                                    DATA_ENABLE, Enable));
#endif    
    BRAP_Write32 (hOp->hRegister, 
               BCHP_AUD_FMM_OP_CTRL_SPDIF_CFG_0 + hOp->ui32Offset, 
               ui32RegVal);     
    
    BDBG_LEAVE (BRAP_OP_P_SpdifOpen);
    return BERR_SUCCESS;
}


BERR_Code BRAP_OP_P_SpdifHWConfig (
    BRAP_OP_P_Handle        hOp,              /*[in] Output port handle */
    const BRAP_OP_P_SpdifSettings * pSettings /*[in] Parameters */
)
{
    uint32_t  ui32RegVal = 0;
    BRAP_OP_P_SpdifSettings sSettings;

    BDBG_ENTER (BRAP_OP_P_SpdifHWConfig);
    BDBG_ASSERT (hOp);

    if (BRAP_FMM_P_GetWatchdogRecoveryFlag (hOp->hFmm) == false)
    {   /* If not in WatchDog recovery */  
        BDBG_ASSERT (pSettings);
        sSettings = *pSettings;
    }
    else
    {
        sSettings = hOp->uOpSettings.sSpdif;
    }

    BDBG_MSG(("BRAP_OP_P_SpdifHWConfig(): "));
    BDBG_MSG(("sSettings.sExtSettings.eChannelOrder=%d", sSettings.sExtSettings.eChannelOrder));
    BDBG_MSG(("sSettings.sExtSettings.bLimitTo16Bits=%d", sSettings.sExtSettings.bLimitTo16Bits));
    BDBG_MSG(("sSettings.sExtSettings.eMClkRate=%d", sSettings.sExtSettings.eMClkRate));
    BDBG_MSG(("sSettings.sExtSettings.ePll=%d", sSettings.sExtSettings.ePll));

    BKNI_EnterCriticalSection();
    ui32RegVal = BRAP_Read32 (hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_SPDIF_CFG_0 + hOp->ui32Offset);
	ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_SPDIF_CFG_0,LR_SELECT));    
    /* Set Channel Order */
    switch (sSettings.sExtSettings.eChannelOrder)
    {
        case BRAP_OP_ChannelOrder_eNormal:
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_SPDIF_CFG_0, 
                                            LR_SELECT, Normal));
            break;
        case BRAP_OP_ChannelOrder_eBothLeft:
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_SPDIF_CFG_0, 
                                            LR_SELECT, Both_Get_Left));
            break;
        case BRAP_OP_ChannelOrder_eBothRight:
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_SPDIF_CFG_0, 
                                            LR_SELECT, Both_Get_Right));
            break;
        case BRAP_OP_ChannelOrder_eSwap:
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_SPDIF_CFG_0, 
                                            LR_SELECT, Swap));
            break;
        default:
            BDBG_ERR (("BRAP_OP_P_SpdifHWConfig(): Incorrect Channel Order %d", 
                        sSettings.sExtSettings.eChannelOrder));
            BKNI_LeaveCriticalSection();
            return BERR_TRACE (BERR_NOT_SUPPORTED);
    }            
       
    /* Set LIMIT_TO_16_BITS field */
	ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_SPDIF_CFG_0,LIMIT_TO_16_BITS));        
    if (sSettings.sExtSettings.bLimitTo16Bits == true)
    {
        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_SPDIF_CFG_0, 
                                            LIMIT_TO_16_BITS, Enable_limit));
    } 
    else if (sSettings.sExtSettings.bLimitTo16Bits == false)
    {
        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_SPDIF_CFG_0, 
                                            LIMIT_TO_16_BITS, Disable_limit));
    } 
   
    /* Set Preamble Polarity to 'High' */ 
	ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_SPDIF_CFG_0,PREAM_POL)); 
    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_SPDIF_CFG_0, 
                                    PREAM_POL, High));
    /* Set SOFT_PARITY to 'Hard' */ 
	ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_SPDIF_CFG_0,SOFT_PARITY)); 
    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_SPDIF_CFG_0, 
                                    SOFT_PARITY, Hard));

    /* Write to the register */
    BRAP_Write32 (hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_SPDIF_CFG_0 + hOp->ui32Offset, ui32RegVal);

    BKNI_LeaveCriticalSection();
    
    if (BRAP_FMM_P_GetWatchdogRecoveryFlag (hOp->hFmm) == false)
    {  
        /* store the settings in hOp */
        hOp->uOpSettings.sSpdif = sSettings;
    }
    return BERR_SUCCESS;
}



static BERR_Code
BRAP_OP_P_SpdifStart (
    BRAP_OP_P_Handle        hOp,             /*[in] Output port handle */
    const BRAP_OP_P_SpdifParams * pParams    /*[in] Start time parameters */
)
{

/*     Algo:
    1. Cofigure AUD_FMM_OP_CTRL_MCLK_CFG_SPDIFX
    2. Cofigure Timebase ie AUD_FMM_PLLX_CONTROL
    3. Clear the CBIT array
    4. Configure the Spdif port ie program register AUD_FMM_OP_CTRL_SPDIF_CFG_X
        x with the settings saved in hRap ie
        hRap->sOutputSettings[BRAP_OutputPort_eSpdif].uOutputPortSettings.sSpdifSettings
        by calling BRAP_OP_P_SpdifHWConfig()
    5. If the port was unmuted, enable data flow.
    6. Enable upstream request.
*/    

    BERR_Code ret = BERR_SUCCESS;
    uint32_t  ui32RegVal = 0;
#if (BRAP_7405_FAMILY == 1)
    unsigned int    uiOrigReVal =0;
#endif
    BRAP_OP_P_SpdifSettings sSettings;
#if (((BRAP_3548_FAMILY != 1)))
    uint32_t ui32StreamId =0;
#endif

    BDBG_ENTER (BRAP_OP_P_SpdifStart);
    BDBG_ASSERT (hOp);

    if (BRAP_FMM_P_GetWatchdogRecoveryFlag (hOp->hFmm) == false)
    {    
        BDBG_ASSERT (pParams);
        hOp->uOpParams.sSpdif = *pParams;
    }
  
    BDBG_MSG(("BRAP_OP_P_SpdifStart: Start time parameters are: "));

    BDBG_MSG(("hOp->uOpParams.sSpdif->eTimebase=%d",hOp->uOpParams.sSpdif.eTimebase));

    if(BRAP_OutputPort_eSpdif == hOp->eOutputPort)
    {
        sSettings.sExtSettings = 
        hOp->hFmm->hRap->sOutputSettings[BRAP_OutputPort_eSpdif].uOutputPortSettings.sSpdifSettings;
    }
    else if(BRAP_OutputPort_eSpdif1 == hOp->eOutputPort)
    {
        sSettings.sExtSettings = 
        hOp->hFmm->hRap->sOutputSettings[BRAP_OutputPort_eSpdif1].uOutputPortSettings.sSpdifSettings;
    }
	else 
	{
		BDBG_ERR(("BRAP_OP_P_SpdifStart called with invalid Output port %#x",hOp->eOutputPort));
		return BERR_TRACE (BERR_INVALID_PARAMETER);
	}


    /* Program the MClk */
    ui32RegVal = 0;

#if (BRAP_7550_FAMILY != 1)
    /* Set the PLL and MClock Rate */
    /* The PLL Channels are choosen in the following way.
       If MClock rate is 128 Fs, Channel 1 should be selected,
       If MClock rate is 256 Fs, Channel 2 should be selected,
       If MClock rate is 512 Fs, Channel 3 should be selected,
       Basically Channels 1,2 and 3 correspond to 128Fs, 256Fs and 512Fs respectively.
       (MClock 384Fs can be set by user programming only).
    */
    switch (sSettings.sExtSettings.ePll)
    {
        case BRAP_OP_Pll_ePll0:
        /* Set MCLK rate and PLL Channels */
        switch (sSettings.sExtSettings.eMClkRate)
        {
            case BRAP_OP_MClkRate_e128Fs:
                ui32RegVal |= ((BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0, 
                                            PLLCLKSEL, PLL0_ch1)) |
                               (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0, 
                                            MCLK_RATE, MCLK_is_128fs)));
                break;     
            case BRAP_OP_MClkRate_e256Fs:
                ui32RegVal |= ((BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0, 
                                            PLLCLKSEL, PLL0_ch2)) |
                               (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0, 
                                            MCLK_RATE, MCLK_is_256fs)));
                break;     
            case BRAP_OP_MClkRate_e512Fs:
                ui32RegVal |= ((BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0, 
                                            PLLCLKSEL, PLL0_ch3)) |
                               (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0, 
                                            MCLK_RATE, MCLK_is_512fs)));
                break;     
            default:
                BDBG_ERR (("BRAP_OP_P_SpdifStart(): Incorrect MClk Rate %d", 
                            sSettings.sExtSettings.eMClkRate));
                return BERR_TRACE (BERR_NOT_SUPPORTED);
        }     
            
        break;
#if (BRAP_PLL1_SUPPORTED == 1)   
        case BRAP_OP_Pll_ePll1:
        
        /* Set MCLK rate and PLL Channels */
        switch (sSettings.sExtSettings.eMClkRate)
        {
            case BRAP_OP_MClkRate_e128Fs:
                ui32RegVal |= ((BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0, 
                                            PLLCLKSEL, PLL1_ch1)) |
                               (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0, 
                                            MCLK_RATE, MCLK_is_128fs)));
                break;     
            case BRAP_OP_MClkRate_e256Fs:
                ui32RegVal |= ((BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0, 
                                            PLLCLKSEL, PLL1_ch2)) |
                               (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0, 
                                            MCLK_RATE, MCLK_is_256fs)));
                break;     
            case BRAP_OP_MClkRate_e512Fs:
                ui32RegVal |= ((BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0, 
                                            PLLCLKSEL, PLL1_ch3)) |
                               (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0, 
                                            MCLK_RATE, MCLK_is_512fs)));
                break;     
            default:
                BDBG_ERR (("BRAP_OP_P_SpdifStart(): Incorrect MClk Rate %d", 
                            sSettings.sExtSettings.eMClkRate));
                return BERR_TRACE (BERR_NOT_SUPPORTED);
        } 

        break;
#endif
        default:
            BDBG_ERR (("BRAP_OP_P_SpdifStart(): Incorrect PLL chosen %d", 
                        sSettings.sExtSettings.ePll));
            return BERR_TRACE (BERR_NOT_SUPPORTED);
    }
#else /* For 7550 Famliy only*/
    /* Set MCLK rate*/
    switch (sSettings.sExtSettings.eMClkRate)
    {
        case BRAP_OP_MClkRate_e128Fs:
            ui32RegVal |= ((BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0, 
                                        PLLCLKSEL, SPDIF_MCLK)) |
                           (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0, 
                                        MCLK_RATE, MCLK_is_128fs)));
            break;     
        case BRAP_OP_MClkRate_e256Fs:
            ui32RegVal |= ((BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0, 
                                        PLLCLKSEL, SPDIF_MCLK)) |
                           (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0, 
                                        MCLK_RATE, MCLK_is_256fs)));
            break;     
        case BRAP_OP_MClkRate_e512Fs:
            ui32RegVal |= ((BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0, 
                                        PLLCLKSEL, SPDIF_MCLK)) |
                           (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0, 
                                        MCLK_RATE, MCLK_is_512fs)));
            break;     
        default:
            BDBG_ERR (("BRAP_OP_P_SpdifStart(): Incorrect MClk Rate %d", 
                        sSettings.sExtSettings.eMClkRate));
            return BERR_TRACE (BERR_NOT_SUPPORTED);
    } 
    BDBG_MSG(("SPDIF MClkRate = %d\n",sSettings.sExtSettings.eMClkRate));
#endif  

#if (BRAP_7405_FAMILY == 1)   
    uiOrigReVal = BRAP_Read32 (hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0);
    if(uiOrigReVal != ui32RegVal)        
#endif
    BRAP_Write32 (hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0 + hOp->ui32Offset, ui32RegVal);

#if ((BRAP_3548_FAMILY == 1))
    /* TODO: Check the logic */
    /* Program the FCI ID of the Input Mixer */
    ui32RegVal = BRAP_Read32(hOp->hRegister, BCHP_AUD_FMM_IOP_CTRL_MIX_IOP_IDMAP10);

    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_IOP_CTRL_MIX_IOP_IDMAP10, STREAM0));
    ui32RegVal |= BCHP_FIELD_DATA(AUD_FMM_IOP_CTRL_MIX_IOP_IDMAP10, STREAM0, 
                                          hOp->uOpParams.sSpdif.ui32InputFciId);
    BRAP_Write32(hOp->hRegister, BCHP_AUD_FMM_IOP_CTRL_MIX_IOP_IDMAP10, ui32RegVal);
	
#if (BRAP_P_RESET_IOP_SM==1)
	/* To prevent last sample from previous run getting played again, reset HW internal state
	 * machine by writing 1 to INIT_SM bit corresponding to this output port. Reset this bit immediately */
	ui32RegVal = BRAP_Read32( hOp->hRegister, BCHP_AUD_FMM_IOP_CTRL_MIX_IOP_FCI_CTRL );
	ui32RegVal &= ~( BCHP_MASK( AUD_FMM_IOP_CTRL_MIX_IOP_FCI_CTRL, INIT_SM0 ));
	ui32RegVal |= BCHP_FIELD_ENUM( AUD_FMM_IOP_CTRL_MIX_IOP_FCI_CTRL, INIT_SM0, Init );
	BRAP_Write32( hOp->hRegister, BCHP_AUD_FMM_IOP_CTRL_MIX_IOP_FCI_CTRL, ui32RegVal );
	/* Reset immediately */
	ui32RegVal &= ~( BCHP_MASK( AUD_FMM_IOP_CTRL_MIX_IOP_FCI_CTRL, INIT_SM0 ));
	ui32RegVal |= BCHP_FIELD_ENUM( AUD_FMM_IOP_CTRL_MIX_IOP_FCI_CTRL, INIT_SM0, Normal );
	BRAP_Write32( hOp->hRegister, BCHP_AUD_FMM_IOP_CTRL_MIX_IOP_FCI_CTRL, ui32RegVal );
#endif

#else 
    if(BRAP_OutputPort_eSpdif == hOp->eOutputPort)
    {
        ui32StreamId = 0;
    }
#if(BRAP_7405_FAMILY != 1)
    else if(BRAP_OutputPort_eSpdif1 == hOp->eOutputPort)
    {
        ui32StreamId = 1;

        /* Program the SPDIF1 (STRM1) TMG_SRC bit */
        ui32RegVal = BRAP_Read32(hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_STREAM_ROUTE);
    	ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_STREAM_ROUTE,STRM1_TMG_SRC_SEL));
        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_STREAM_ROUTE, STRM1_TMG_SRC_SEL, SPDIF1));
        BRAP_Write32 (hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_STREAM_ROUTE, ui32RegVal);        
    }
#endif    
    
    ui32RegVal = BRAP_Read32(hOp->hRegister , 
                (BCHP_AUD_FMM_IOP_CTRL_FCI_CFGi_ARRAY_BASE + (ui32StreamId*4)));

    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_IOP_CTRL_FCI_CFGi, ID));
    ui32RegVal |= BCHP_FIELD_DATA(AUD_FMM_IOP_CTRL_FCI_CFGi, ID,hOp->uOpParams.sSpdif.ui32InputFciId);

    BRAP_Write32(hOp->hRegister , (BCHP_AUD_FMM_IOP_CTRL_FCI_CFGi_ARRAY_BASE + 
                            (ui32StreamId*4)),ui32RegVal);
#endif

#if(BRAP_7405_FAMILY != 1)
    /* Program the Timebase */
    ret = BRAP_OP_P_ProgramTimebase (hOp, sSettings.sExtSettings.ePll,
                                     hOp->uOpParams.sSpdif.eTimebase);
    if (ret != BERR_SUCCESS)
    {
        BDBG_ERR(("BRAP_OP_P_SpdifStart(): call to BRAP_OP_P_ProgramTimebase() failed"));
        return BERR_TRACE (ret);
    }
#endif
   
    /* Configure the on-the-fly paramters for Spdif port */
    BRAP_OP_P_SpdifHWConfig (hOp, &sSettings);

    BKNI_EnterCriticalSection();
    /* Enable the Spdif port */
    ui32RegVal = BRAP_Read32 (hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_SPDIF_CFG_0 + hOp->ui32Offset);
	ui32RegVal &= ~(BCHP_MASK (
                AUD_FMM_OP_CTRL_SPDIF_CFG_0, 
                DATA_ENABLE));    
        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_SPDIF_CFG_0, 
                                    DATA_ENABLE, Enable));
    BRAP_Write32 (hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_SPDIF_CFG_0 + hOp->ui32Offset, ui32RegVal);

    BKNI_LeaveCriticalSection();

    /* Enable SPDIF upstream request */
    ret = BRAP_OP_P_EnableStream (hOp, true);
    if (ret != BERR_SUCCESS)
    {
        BDBG_ERR(("BRAP_OP_P_SpdifStart(): call to BRAP_OP_P_EnableStream() failed"));
        return BERR_TRACE (ret);
    }


    BDBG_LEAVE (BRAP_OP_P_SpdifStart);
    return ret;
}

    
static BERR_Code
BRAP_OP_P_SpdifStop (
    BRAP_OP_P_Handle        hOp    /*[in] Output port handle */
)
{
    BERR_Code ret = BERR_SUCCESS;
#if (BRAP_7405_FAMILY != 1)
    uint32_t  ui32RegVal = 0;
#endif
    BDBG_ENTER (BRAP_OP_P_SpdifStop);
    BDBG_ASSERT (hOp);

    BDBG_MSG(("BRAP_OP_P_SpdifStop: entering"));

#if (BRAP_7405_FAMILY != 1)
    /* Read current value of the SPDIF config register */
    ui32RegVal = BRAP_Read32 (hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_SPDIF_CFG_0 + hOp->ui32Offset);

    /* Disable DATA_ENABLE. Do not disable the Clock */
	ui32RegVal &= ~(BCHP_MASK (AUD_FMM_OP_CTRL_SPDIF_CFG_0, DATA_ENABLE));        
    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_SPDIF_CFG_0,DATA_ENABLE, Disable));
    
    /* Write to the register */
    BRAP_Write32 (hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_SPDIF_CFG_0 + hOp->ui32Offset, ui32RegVal);
   
    /* Disable SPDIF upstream request */
    ret = BRAP_OP_P_EnableStream (hOp, false);
#endif

#if (BRAP_7405_FAMILY == 1)
    BSTD_UNUSED(hOp);
#endif
    BDBG_LEAVE (BRAP_OP_P_SpdifStop);
    return ret;
}

#if (BRAP_7550_FAMILY != 1)
/* For 7440 A0 based systems */
#if ((BRAP_3548_FAMILY == 1))

BERR_Code
BRAP_OP_P_I2sHWConfig_Multi (
    BRAP_OP_P_Handle      hOp,               /*[in] Output port handle */
    const BRAP_OP_P_I2sSettings * pSettings  /*[in] Open time parameters */
)
{
    BERR_Code               ret = BERR_SUCCESS;
    uint32_t                ui32RegVal = 0;
    unsigned int            uiTemp = 0;
    BRAP_OP_P_I2sSettings   sSettings;
    bool                    bWdRecovery = false;

    BDBG_ENTER (BRAP_OP_P_I2sHWConfig_Multi);
    BDBG_ASSERT (hOp);

    bWdRecovery = BRAP_FMM_P_GetWatchdogRecoveryFlag (hOp->hFmm);
    
    if (false == bWdRecovery)
    {   /* If not in WatchDog recovery */  
        BDBG_ASSERT (pSettings);
        sSettings = *pSettings;
    }
    else
    {
        sSettings = hOp->uOpSettings.sI2s;
    }

    /* Program I2SM_CROSSBAR */
    ui32RegVal = BRAP_Read32(hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_I2SM_CROSSBAR);
    
    /* Set Channel Order */
    switch (hOp->eOutputPort)
    {
        case BRAP_OutputPort_eI2s0:
            ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2SM_CROSSBAR,OUT0_R));
            ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2SM_CROSSBAR,OUT0_L));

            switch (sSettings.sExtSettings.eChannelOrder)
            {
                case BRAP_OP_ChannelOrder_eSwap:
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM_CROSSBAR, 
                                                    OUT0_R, In0_l));
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM_CROSSBAR, 
                                                    OUT0_L, In0_r));
                    break;
                case BRAP_OP_ChannelOrder_eBothLeft:
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM_CROSSBAR, 
                                                    OUT0_R, In0_l));
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM_CROSSBAR, 
                                                    OUT0_L, In0_l));
                    break;
                case BRAP_OP_ChannelOrder_eBothRight:
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM_CROSSBAR, 
                                                    OUT0_R, In0_r));
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM_CROSSBAR, 
                                                    OUT0_L, In0_r));
                    break;
                case BRAP_OP_ChannelOrder_eNormal:
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM_CROSSBAR, 
                                                    OUT0_R, In0_r));
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM_CROSSBAR, 
                                                    OUT0_L, In0_l));
                    break;
                default:
                    BDBG_ERR (("BRAP_OP_P_I2sOpen_Multi(): Incorrect Channel Order %d", 
                                sSettings.sExtSettings.eChannelOrder));
                    return BERR_TRACE (BERR_NOT_SUPPORTED);
            }
            break;
        case BRAP_OutputPort_eI2s1:
            ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2SM_CROSSBAR,OUT1_R));
            ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2SM_CROSSBAR,OUT1_L));
            
            switch (sSettings.sExtSettings.eChannelOrder)
            {
                case BRAP_OP_ChannelOrder_eSwap:
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM_CROSSBAR, 
                                                    OUT1_R, In1_l));
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM_CROSSBAR, 
                                                    OUT1_L, In1_r));
                    break;
                case BRAP_OP_ChannelOrder_eBothLeft:
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM_CROSSBAR, 
                                                    OUT1_R, In1_l));
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM_CROSSBAR, 
                                                    OUT1_L, In1_l));
                    break;
                case BRAP_OP_ChannelOrder_eBothRight:
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM_CROSSBAR, 
                                                    OUT1_R, In1_r));
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM_CROSSBAR, 
                                                    OUT1_L, In1_r));
                    break;
                case BRAP_OP_ChannelOrder_eNormal:
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM_CROSSBAR, 
                                                    OUT1_R, In1_r));
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM_CROSSBAR, 
                                                    OUT1_L, In1_l));
                    break;
                default:
                    BDBG_ERR (("BRAP_OP_P_I2sOpen_Multi(): Incorrect Channel Order %d", 
                                sSettings.sExtSettings.eChannelOrder));
                    return BERR_TRACE (BERR_NOT_SUPPORTED);
            }
            break;
        case BRAP_OutputPort_eI2s2:
            ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2SM_CROSSBAR,OUT2_R));
            ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2SM_CROSSBAR,OUT2_L));
            
            switch (sSettings.sExtSettings.eChannelOrder)
            {
                case BRAP_OP_ChannelOrder_eSwap:
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM_CROSSBAR, 
                                                    OUT2_R, In2_l));
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM_CROSSBAR, 
                                                    OUT2_L, In2_r));
                    break;
                case BRAP_OP_ChannelOrder_eBothLeft:
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM_CROSSBAR, 
                                                    OUT2_R, In2_l));
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM_CROSSBAR, 
                                                    OUT2_L, In2_l));
                    break;
                case BRAP_OP_ChannelOrder_eBothRight:
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM_CROSSBAR, 
                                                    OUT2_R, In2_r));
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM_CROSSBAR, 
                                                    OUT2_L, In2_r));
                    break;
                case BRAP_OP_ChannelOrder_eNormal:
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM_CROSSBAR, 
                                                    OUT2_R, In2_r));
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM_CROSSBAR, 
                                                    OUT2_L, In2_l));
                    break;
                default:
                    BDBG_ERR (("BRAP_OP_P_I2sOpen_Multi(): Incorrect Channel Order %d", 
                                sSettings.sExtSettings.eChannelOrder));
                    return BERR_TRACE (BERR_NOT_SUPPORTED);
            }
            break;
	    case BRAP_OutputPort_eI2s3: 
            ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2SM_CROSSBAR,OUT3_R));
            ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2SM_CROSSBAR,OUT3_L));
            
            switch (sSettings.sExtSettings.eChannelOrder)
            {
                case BRAP_OP_ChannelOrder_eSwap:
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM_CROSSBAR, 
                                                    OUT3_R, In3_l));
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM_CROSSBAR, 
                                                    OUT3_L, In3_r));
                    break;
                case BRAP_OP_ChannelOrder_eBothLeft:
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM_CROSSBAR, 
                                                    OUT3_R, In3_l));
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM_CROSSBAR, 
                                                    OUT3_L, In3_l));
                    break;
                case BRAP_OP_ChannelOrder_eBothRight:
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM_CROSSBAR, 
                                                    OUT3_R, In3_r));
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM_CROSSBAR, 
                                                    OUT3_L, In3_r));
                    break;
                case BRAP_OP_ChannelOrder_eNormal:
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM_CROSSBAR, 
                                                    OUT3_R, In3_r));
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM_CROSSBAR, 
                                                    OUT3_L, In3_l));
                    break;
                default:
                    BDBG_ERR (("BRAP_OP_P_I2sOpen_Multi(): Incorrect Channel Order %d", 
                                sSettings.sExtSettings.eChannelOrder));
                    return BERR_TRACE (BERR_NOT_SUPPORTED);
            }
            break;
        default:
            BDBG_ERR (("BRAP_OP_P_I2sOpen_Multi(): Incorrect Output Port %d", 
                                                            hOp->eOutputPort));
            return BERR_TRACE (BERR_NOT_SUPPORTED);
    }
    BRAP_Write32 (hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_I2SM_CROSSBAR,ui32RegVal);

    ui32RegVal = 0;

    ui32RegVal = BRAP_Read32(hOp->hRegister,BCHP_AUD_FMM_OP_CTRL_I2S_CFG_MULTI);

    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2S_CFG_MULTI,DATA_JUSTIFICATION));
    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2S_CFG_MULTI,DATA_ALIGNMENT));
    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2S_CFG_MULTI,SCLK_POLARITY));
    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2S_CFG_MULTI,LRCK_POLARITY));
    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2S_CFG_MULTI,SCLKS_PER_1FS_DIV32));

    /* Data Justification */
    if (sSettings.sExtSettings.bLsbAtLRClk == true)
    {
        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2S_CFG_MULTI, 
                                        DATA_JUSTIFICATION, LSB));
    }
    else 
    {
        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2S_CFG_MULTI, 
                                        DATA_JUSTIFICATION, MSB));
    }

    /* Data Allignment */
    if (sSettings.sExtSettings.bAlignedToLRClk == false)
    {
        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2S_CFG_MULTI, 
                                        DATA_ALIGNMENT, Delayed));
    }
    else 
    {
        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2S_CFG_MULTI, 
                                        DATA_ALIGNMENT, Aligned));
    }

    /* SCLK & LRCLK Polarity */
    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2S_CFG_MULTI, 
                                    SCLK_POLARITY, Falling_aligned_with_sdata));

    if (sSettings.sExtSettings.bLRClkPolarity == true)
    {
        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2S_CFG_MULTI, 
                                        LRCK_POLARITY, High_for_left));
    }
    else 
    {
        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2S_CFG_MULTI, 
                                        LRCK_POLARITY, Low_for_left));
    }

    /* Set SCLKS_PER_1FS_DIV32 field. This field times 32 is the number 
       of sclk cycles in one Fs period. For example, program this register
       to 2 for a 64fs bit clock. */
    switch (sSettings.sExtSettings.eSClkRate)
    {
        case BRAP_OP_SClkRate_e64Fs: 
            uiTemp = 64;
            break;
        case BRAP_OP_SClkRate_e128Fs: 
            uiTemp = 128;
            break;
        default:
            BDBG_ERR (("BRAP_OP_P_I2sOpen_Multi(): Incorrect SClk %d", 
                        sSettings.sExtSettings.eSClkRate));
            return BERR_TRACE (BERR_NOT_SUPPORTED);
    }
    ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_I2S_CFG_MULTI, 
                                    SCLKS_PER_1FS_DIV32,(uiTemp/32)));

    BRAP_Write32 (hOp->hRegister,BCHP_AUD_FMM_OP_CTRL_I2S_CFG_MULTI,ui32RegVal);     

    /* dont store the settings until everything's correct */
    if ( false == bWdRecovery)
    {  
        hOp->uOpSettings.sI2s = *pSettings;
    }
    
    BDBG_LEAVE (BRAP_OP_P_I2sHWConfig_Multi);
    return ret;
}

BERR_Code
BRAP_OP_P_I2sHWConfig_Stereo (
    BRAP_OP_P_Handle      hOp,               /*[in] Output port handle */
    const BRAP_OP_P_I2sSettings * pSettings  /*[in] Open time parameters */
)
{
    BERR_Code               ret = BERR_SUCCESS;
    uint32_t                ui32RegVal = 0;
    unsigned int            uiTemp = 0;
    BRAP_OP_P_I2sSettings   sSettings;
    bool                    bWdRecovery = false;

    BDBG_ENTER (BRAP_OP_P_I2sHWConfig_Stereo);
    BDBG_ASSERT (hOp);

    bWdRecovery = BRAP_FMM_P_GetWatchdogRecoveryFlag (hOp->hFmm);
    
    if (false == bWdRecovery)
    {   /* If not in WatchDog recovery */  
        BDBG_ASSERT (pSettings);
        sSettings = *pSettings;
    }
    else
    {
        sSettings = hOp->uOpSettings.sI2s;
    }

    /* Set Channel Order */
    ui32RegVal = BRAP_Read32 (hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_I2SS_CROSSBAR);

    ui32RegVal &= ~(BCHP_MASK (AUD_FMM_OP_CTRL_I2SS_CROSSBAR,OUT_R));
    ui32RegVal &= ~(BCHP_MASK (AUD_FMM_OP_CTRL_I2SS_CROSSBAR,OUT_L));
    
    switch (sSettings.sExtSettings.eChannelOrder)
    {

        /* With correct RDB */
        case BRAP_OP_ChannelOrder_eSwap:
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SS_CROSSBAR, 
                                            OUT_R, In_l));
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SS_CROSSBAR, 
                                            OUT_L, In_r));
            break;
        case BRAP_OP_ChannelOrder_eBothLeft:
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SS_CROSSBAR, 
                                            OUT_R, In_l));
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SS_CROSSBAR, 
                                            OUT_L, In_l));
            break;
        case BRAP_OP_ChannelOrder_eBothRight:
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SS_CROSSBAR, 
                                            OUT_R, In_r));
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SS_CROSSBAR, 
                                            OUT_L, In_r));
            break;
        case BRAP_OP_ChannelOrder_eNormal:
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SS_CROSSBAR, 
                                            OUT_R, In_r));
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SS_CROSSBAR, 
                                            OUT_L, In_l));
            break;
        default:
            BDBG_ERR (("BRAP_OP_P_I2sOpen_Stereo(): Incorrect Channel Order %d", 
                        sSettings.sExtSettings.eChannelOrder));
            return BERR_TRACE (BERR_NOT_SUPPORTED);
    }          

    BRAP_Write32 (hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_I2SS_CROSSBAR,ui32RegVal);

    ui32RegVal = 0;

    ui32RegVal = BRAP_Read32(hOp->hRegister,BCHP_AUD_FMM_OP_CTRL_I2S_CFG_STEREO);

    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2S_CFG_STEREO,DATA_JUSTIFICATION));
    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2S_CFG_STEREO,DATA_ALIGNMENT));
    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2S_CFG_STEREO,SCLK_POLARITY));
    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2S_CFG_STEREO,LRCK_POLARITY));
    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2S_CFG_STEREO,SCLKS_PER_1FS_DIV32));

    /* DATA JUSTIFICATION */
    if (sSettings.sExtSettings.bLsbAtLRClk == true)
    {
        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2S_CFG_STEREO, 
                                        DATA_JUSTIFICATION, LSB));
    }
    else 
    {
        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2S_CFG_STEREO, 
                                        DATA_JUSTIFICATION, MSB));
    }

    /* DATA ALIGNMENT */
    if (sSettings.sExtSettings.bAlignedToLRClk == false)
    {
        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2S_CFG_STEREO, 
                                        DATA_ALIGNMENT, Delayed));
    }
    else 
    {
        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2S_CFG_STEREO, 
                                        DATA_ALIGNMENT, Aligned));
    }

    /*SCLK and LRCK POLARITY*/
    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2S_CFG_STEREO, 
                                    SCLK_POLARITY, Falling_aligned_with_sdata));

    if (sSettings.sExtSettings.bLRClkPolarity == true)
    {
        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2S_CFG_STEREO, 
                                        LRCK_POLARITY, High_for_left));
    }
    else 
    {
        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2S_CFG_STEREO, 
                                        LRCK_POLARITY, Low_for_left));
    }       
 
    /* Set SCLKS_PER_1FS_DIV32 field. This field times 32 is the number 
       of sclk cycles in one Fs period. For example, program this register
       to 2 for a 64fs bit clock. */
    switch (sSettings.sExtSettings.eSClkRate)
    {
        case BRAP_OP_SClkRate_e64Fs: 
            uiTemp = 64;
            break;
        case BRAP_OP_SClkRate_e128Fs: 
            uiTemp = 128;
            break;
        default:
            BDBG_ERR (("BRAP_OP_P_I2sOpen_Stereo(): Incorrect SClk %d", 
                        sSettings.sExtSettings.eSClkRate));
            return BERR_TRACE (BERR_NOT_SUPPORTED);
    }
    ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_I2S_CFG_STEREO, 
                                    SCLKS_PER_1FS_DIV32,(uiTemp/32)));

    BRAP_Write32 (hOp->hRegister,BCHP_AUD_FMM_OP_CTRL_I2S_CFG_STEREO,ui32RegVal);     

    /* dont store the settings until everything's correct */
    if ( false == bWdRecovery)
    {  
        hOp->uOpSettings.sI2s = *pSettings;
    }
    
    BDBG_LEAVE (BRAP_OP_P_I2sHWConfig_Stereo);
    return ret;
}

static BERR_Code
BRAP_OP_P_I2sOpen_Multi(
    BRAP_OP_P_Handle                hOp,            /*[in] Output port handle */
    const BRAP_OP_P_I2sSettings     *pSettings      /*[in] Open time parameters */
)
{
    BERR_Code               ret = BERR_SUCCESS;
    uint32_t                ui32RegVal = 0;

    BDBG_ENTER (BRAP_OP_P_I2sOpen_Multi);
    BDBG_ASSERT (hOp);
    BSTD_UNUSED (pSettings);
    
    /* Clear the corresponding bit in OP_CTRL_I2SM_GROUPING */
    ui32RegVal = BRAP_Read32(hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_I2SM_GROUPING);
    switch (hOp->eOutputPort)
    {
        case BRAP_OutputPort_eI2s0: ui32RegVal &= ~(0x1); break;
        case BRAP_OutputPort_eI2s1: ui32RegVal &= ~(0x2); break;
        case BRAP_OutputPort_eI2s2: ui32RegVal &= ~(0x4); break;
        case BRAP_OutputPort_eI2s3: ui32RegVal &= ~(0x8); break;       
        default: return BERR_TRACE (BERR_NOT_SUPPORTED);
    }/* switch */
    BRAP_Write32(hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_I2SM_GROUPING, ui32RegVal);

    /* Enable CLOCK_ENABLE */
    ui32RegVal = BRAP_Read32(hOp->hRegister,BCHP_AUD_FMM_OP_CTRL_I2S_CFG_MULTI);
    
    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2S_CFG_MULTI, 
                                    CLOCK_ENABLE, Enable));

    BRAP_Write32 (hOp->hRegister,BCHP_AUD_FMM_OP_CTRL_I2S_CFG_MULTI,ui32RegVal);     
    
    BDBG_LEAVE (BRAP_OP_P_I2sOpen_Multi);
    return ret; 
}

static BERR_Code
BRAP_OP_P_I2sOpen_Stereo(
    BRAP_OP_P_Handle                hOp,            /*[in] Output port handle */
    const BRAP_OP_P_I2sSettings     *pSettings      /*[in] Open time parameters */
)
{
    BERR_Code               ret = BERR_SUCCESS;
    uint32_t                ui32RegVal = 0;
    
    BDBG_ENTER (BRAP_OP_P_I2sOpen_Stereo);
    BDBG_ASSERT (hOp);
    BSTD_UNUSED (pSettings);
    
    /* Enable CLOCK_ENABLE */
    ui32RegVal = BRAP_Read32(hOp->hRegister,BCHP_AUD_FMM_OP_CTRL_I2S_CFG_STEREO);
    
    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2S_CFG_STEREO, 
                                    CLOCK_ENABLE, Enable));

    BRAP_Write32 (hOp->hRegister,BCHP_AUD_FMM_OP_CTRL_I2S_CFG_STEREO,ui32RegVal);     
    
    BDBG_LEAVE (BRAP_OP_P_I2sOpen_Stereo);
    return ret; 
}

static BERR_Code
BRAP_OP_P_I2sStart_Multi(
    BRAP_OP_P_Handle                hOp,            /*[in] Output port handle */
    const BRAP_OP_P_I2sParams       *pParams        /*[in] Start time parameters */
)
{
    BERR_Code               ret = BERR_SUCCESS;
    uint32_t                ui32RegVal = 0, ui32GrpBit = 0;
    BRAP_OP_P_I2sParams     sParams;
    bool                    bWdRecovery = false;
    BRAP_OP_P_I2sSettings   sSettings;
    unsigned int            uiTemp =0;


    BDBG_ENTER (BRAP_OP_P_I2sStart_Multi);
    BDBG_ASSERT (hOp);

    bWdRecovery = BRAP_FMM_P_GetWatchdogRecoveryFlag (hOp->hFmm);
    if (false == bWdRecovery)
    { 
        BDBG_ASSERT (pParams);
        hOp->uOpParams.sI2s = *pParams;
        sParams = *pParams;
    }
    else
    {
        sParams = hOp->uOpParams.sI2s; 
    }

    sSettings.sExtSettings = hOp->hFmm->hRap->sOutputSettings[hOp->eOutputPort].
                                               uOutputPortSettings.sI2sSettings;

    /* Set SCLKS_PER_1FS_DIV32 field. This field times 32 is the number 
       of sclk cycles in one Fs period. For example, program this register
       to 2 for a 64fs bit clock. */
    switch (sSettings.sExtSettings.eSClkRate)
    {
        case BRAP_OP_SClkRate_e64Fs: 
            uiTemp = 64;
            break;
        case BRAP_OP_SClkRate_e128Fs: 
            uiTemp = 128;
            break;
        default:
            BDBG_ERR (("BRAP_OP_P_I2sOpen_Multi(): Incorrect SClk %d", 
                        sSettings.sExtSettings.eSClkRate));
            return BERR_TRACE (BERR_NOT_SUPPORTED);
    }
    
    /* Set MCLK/(2*SCLK) ratio in MCLK_RATE field */
    switch (sSettings.sExtSettings.eMClkRate)
    {
        case BRAP_OP_MClkRate_e128Fs:
           uiTemp = (128 / (2 * uiTemp));
           break; 
        case BRAP_OP_MClkRate_e256Fs:
           uiTemp = (256 / (2 * uiTemp));
           break; 
        case BRAP_OP_MClkRate_e384Fs:
           uiTemp = (384 / (2 * uiTemp));
           break; 
        case BRAP_OP_MClkRate_e512Fs:
           uiTemp = (512 / (2 * uiTemp));
           break; 
        default:
            BDBG_ERR (("BRAP_OP_P_I2sOpen_Multi(): Incorrect MClk %d", 
                        sSettings.sExtSettings.eMClkRate));
            return BERR_TRACE (BERR_NOT_SUPPORTED);
    }
    
    switch (uiTemp)
    {
        case 1:
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_I2S_MULTI, 
                                            MCLK_RATE, MCLK_128fs_SCLK_64fs));
            break;     
        case 2:
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_I2S_MULTI, 
                                            MCLK_RATE, MCLK_256fs_SCLK_64fs));
            break;     
        case 3:
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_I2S_MULTI, 
                                            MCLK_RATE, MCLK_384fs_SCLK_64fs));
            break;     
        case 4:
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_I2S_MULTI, 
                                            MCLK_RATE, MCLK_512fs_SCLK_64fs));
            break;     
        default:
            BDBG_ERR (("BRAP_OP_P_I2sOpen_Multi(): Unsupported MClk/SClk Ratio %d", 
                        uiTemp));
            return BERR_TRACE (BERR_NOT_SUPPORTED);
    } 

  
    /* Set the PLL */
    /* The PLL Channels are choosen in the following way.
        
       If MClock rate is 128 Fs, Channel 1 should be selected,
       If MClock rate is 256 Fs, Channel 2 should be selected,
       If MClock rate is 512 Fs, Channel 3 should be selected,
 
       Basically Channels 1,2 and 3 correspond to 128Fs, 256Fs and 512Fs respectively.
 
       (MClock 384Fs can be set by user programming only).
    */
    switch (sSettings.sExtSettings.ePll)
    {
        case BRAP_OP_Pll_ePll0:
            
        /* Set MCLK rate and PLL Channels */
        switch (sSettings.sExtSettings.eMClkRate)
        {
            case BRAP_OP_MClkRate_e128Fs:
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_I2S_MULTI, 
                                            PLLCLKSEL, PLL0_ch1));
                break;     
            case BRAP_OP_MClkRate_e256Fs:
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_I2S_MULTI, 
                                            PLLCLKSEL, PLL0_ch2));
                break;     
            case BRAP_OP_MClkRate_e512Fs:
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_I2S_MULTI, 
                                            PLLCLKSEL, PLL0_ch3));
                break;     
            default:
                BDBG_ERR (("BRAP_OP_P_I2sOpen_Multi(): Incorrect MClk Rate %d", 
                            sSettings.sExtSettings.eMClkRate));
                return BERR_TRACE (BERR_NOT_SUPPORTED);
        }     
            
        break;
        case BRAP_OP_Pll_ePll1:
        
        /* Set MCLK rate and PLL Channels */
        switch (sSettings.sExtSettings.eMClkRate)
        {
            case BRAP_OP_MClkRate_e128Fs:
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_I2S_MULTI, 
                                            PLLCLKSEL, PLL1_ch1));
                break;     
            case BRAP_OP_MClkRate_e256Fs:
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_I2S_MULTI, 
                                            PLLCLKSEL, PLL1_ch2));
                break;     
            case BRAP_OP_MClkRate_e512Fs:
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_I2S_MULTI, 
                                            PLLCLKSEL, PLL1_ch3));
                break;     
            default:
                BDBG_ERR (("BRAP_OP_P_I2sOpen_Multi(): Incorrect MClk Rate %d", 
                            sSettings.sExtSettings.eMClkRate));
                return BERR_TRACE (BERR_NOT_SUPPORTED);
        } 

        break;
       default:
            BDBG_ERR (("BRAP_OP_P_I2sOpen_Multi(): Incorrect PLL chosen %d", 
                        sSettings.sExtSettings.ePll));
            return BERR_TRACE (BERR_NOT_SUPPORTED);
    }

    BRAP_Write32 (hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_I2S_MULTI,ui32RegVal);

    /* Configure the on-the-fly paramters for I2S port */
    BRAP_OP_P_I2sHWConfig_Multi (hOp, &sSettings);


    /* Read current value of the I2S config register */
    ui32RegVal = BRAP_Read32(hOp->hRegister,BCHP_AUD_FMM_OP_CTRL_I2S_CFG_MULTI);
    
    if ((sParams.uiBitsPerSample > 0) && (sParams.uiBitsPerSample < 32))
    {
        ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_I2S_CFG_MULTI, 
                                        BITS_PER_SAMPLE, 
                                        sParams.uiBitsPerSample));    
    }
    else if (sParams.uiBitsPerSample == 32)
    {
        ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_I2S_CFG_MULTI, 
                                        BITS_PER_SAMPLE, 
                                        0));    
    }
    else 
    {
        BDBG_ERR (("BRAP_OP_P_I2sStart_Multi: Incorrect Bits per Sample %d", 
                    sParams.uiBitsPerSample));
        return BERR_TRACE (BERR_NOT_SUPPORTED);
    }

    if (hOp->hFmm->hRap->bOutputMute[hOp->eOutputPort] == false)
    {
        /* Enable DATA_ENABLE only if ouput port is unMuted */
        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2S_CFG_MULTI, 
                                    DATA_ENABLE, Enable));
    }

    /* Write to the register */
    BRAP_Write32 (hOp->hRegister,BCHP_AUD_FMM_OP_CTRL_I2S_CFG_MULTI,ui32RegVal);

    /* Read-Modify-Write AUD_FMM_MISC_SEROUT_OE */
    ui32RegVal = BRAP_Read32 (hOp->hRegister, BCHP_AUD_FMM_MISC_SEROUT_OE);

    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE,
                                            LRCKM0_OE, Drive));
    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE,
                                            SCLKM0_OE, Drive));
    switch(hOp->eOutputPort)
    {
        case BRAP_OutputPort_eI2s0:
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE, 
                                            SDATM0_OE, Drive));
            break;
        case BRAP_OutputPort_eI2s1:
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE, 
                                            SDATM1_OE, Drive));
            break;
        case BRAP_OutputPort_eI2s2:
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE, 
                                            SDATM2_OE, Drive));
            break;
	    case BRAP_OutputPort_eI2s3:
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE, 
                                            SDATM3_OE, Drive));
            break;
        default:
            BDBG_ERR(("BRAP_OP_P_I2sStart_Multi: Unhandled OpPort %d",hOp->eOutputPort));
            BDBG_ASSERT(0);
    }       
    BRAP_Write32 (hOp->hRegister, BCHP_AUD_FMM_MISC_SEROUT_OE, ui32RegVal);

    /* Program the FCI ID of the Input Mixer */
    switch(hOp->eOutputPort)
    {
        case BRAP_OutputPort_eI2s0:
            ui32GrpBit = 0x1;
            ui32RegVal = BRAP_Read32(hOp->hRegister, BCHP_AUD_FMM_IOP_CTRL_MIX_IOP_IDMAP32);

            ui32RegVal &= ~(BCHP_MASK(AUD_FMM_IOP_CTRL_MIX_IOP_IDMAP32, STREAM3));
            ui32RegVal |= BCHP_FIELD_DATA(AUD_FMM_IOP_CTRL_MIX_IOP_IDMAP32, STREAM3, 
                                                        hOp->uOpParams.sI2s.ui32InputFciId);
            BRAP_Write32(hOp->hRegister, BCHP_AUD_FMM_IOP_CTRL_MIX_IOP_IDMAP32, ui32RegVal);
            break;

        case BRAP_OutputPort_eI2s1:
            ui32GrpBit = 0x2;
            ui32RegVal = BRAP_Read32(hOp->hRegister, BCHP_AUD_FMM_IOP_CTRL_MIX_IOP_IDMAP54);

            ui32RegVal &= ~(BCHP_MASK(AUD_FMM_IOP_CTRL_MIX_IOP_IDMAP54, STREAM4));
            ui32RegVal |= BCHP_FIELD_DATA(AUD_FMM_IOP_CTRL_MIX_IOP_IDMAP54, STREAM4, 
                                                        hOp->uOpParams.sI2s.ui32InputFciId);
            BRAP_Write32(hOp->hRegister, BCHP_AUD_FMM_IOP_CTRL_MIX_IOP_IDMAP54, ui32RegVal);
            break;

        case BRAP_OutputPort_eI2s2:
            ui32GrpBit = 0x4;
            ui32RegVal = BRAP_Read32(hOp->hRegister, BCHP_AUD_FMM_IOP_CTRL_MIX_IOP_IDMAP54);

            ui32RegVal &= ~(BCHP_MASK(AUD_FMM_IOP_CTRL_MIX_IOP_IDMAP54, STREAM5));
            ui32RegVal |= BCHP_FIELD_DATA(AUD_FMM_IOP_CTRL_MIX_IOP_IDMAP54, STREAM5, 
                                                        hOp->uOpParams.sI2s.ui32InputFciId);
            BRAP_Write32(hOp->hRegister, BCHP_AUD_FMM_IOP_CTRL_MIX_IOP_IDMAP54, ui32RegVal);
            break;

        case BRAP_OutputPort_eI2s3:
            ui32GrpBit = 0x8;
            ui32RegVal = BRAP_Read32(hOp->hRegister, BCHP_AUD_FMM_IOP_CTRL_MIX_IOP_IDMAP76);

            ui32RegVal &= ~(BCHP_MASK(AUD_FMM_IOP_CTRL_MIX_IOP_IDMAP76, STREAM6));
            ui32RegVal |= BCHP_FIELD_DATA(AUD_FMM_IOP_CTRL_MIX_IOP_IDMAP76, STREAM6, 
                                                        hOp->uOpParams.sI2s.ui32InputFciId);
            BRAP_Write32(hOp->hRegister, BCHP_AUD_FMM_IOP_CTRL_MIX_IOP_IDMAP76, ui32RegVal);
            break;
        default:
            BDBG_ERR(("BRAP_OP_P_I2sStart_Multi: Unhandled OpPort %d",
                hOp->eOutputPort));
            BDBG_ASSERT(0);
    }

    /* Program the group id */
    if(true == hOp->uOpParams.sI2s.bIsMulti)
    {
        ui32RegVal = BRAP_Read32(hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_I2SM_GROUPING);
        ui32RegVal |= ui32GrpBit;
        BRAP_Write32(hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_I2SM_GROUPING, ui32RegVal);
    }
    
#if(BRAP_7405_FAMILY != 1)
    /* Program the Timebase */
    ret = BRAP_OP_P_ProgramTimebase (hOp, hOp->uOpSettings.sI2s.sExtSettings.ePll,
                                     sParams.eTimebase);
#endif
    /* Enable I2S upstream request */
    ret = BRAP_OP_P_EnableStream (hOp, true);
    BDBG_LEAVE (BRAP_OP_P_I2sStart_Multi);
    return ret;
}

static BERR_Code
BRAP_OP_P_I2sStop_Multi(
    BRAP_OP_P_Handle        hOp             /*[in] Output port handle */
)
{
    BERR_Code       ret = BERR_SUCCESS;
    uint32_t        ui32RegVal = 0;
    uint32_t        ui32GrpBit = 0;

    BDBG_ENTER (BRAP_OP_P_I2sStop_Multi);
    BDBG_ASSERT (hOp);
   
    /* Program BCHP_AUD_FMM_OP_CTRL_I2S_CFG_MULTI as following (this is to Disable I2S)
       DATA_ENABLE = Disable 
    */

    /* Read current value of the I2S config register */
    ui32RegVal = BRAP_Read32 (hOp->hRegister,BCHP_AUD_FMM_OP_CTRL_I2S_CFG_MULTI);
    

    /* Disable DATA_ENABLE */
    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2S_CFG_MULTI,DATA_ENABLE, Disable));
    
    /* Write to the register */
    BRAP_Write32 (hOp->hRegister,BCHP_AUD_FMM_OP_CTRL_I2S_CFG_MULTI,ui32RegVal);

    /* Read-Modify-Write AUD_FMM_MISC_SEROUT_OE */
    ui32RegVal = BRAP_Read32 (hOp->hRegister, BCHP_AUD_FMM_MISC_SEROUT_OE);

    /* Read-Modify-Write AUD_FMM_MISC_SEROUT_OE */
    ui32RegVal = BRAP_Read32 (hOp->hRegister, BCHP_AUD_FMM_MISC_SEROUT_OE);

    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE,LRCKM0_OE, Tristate));            
    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE,SCLKM0_OE, Tristate));            
    switch(hOp->eOutputPort)
    {
        case BRAP_OutputPort_eI2s0:
            ui32GrpBit = 0x1;
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE, 
                                            SDATM0_OE, Tristate));
            break;
        case BRAP_OutputPort_eI2s1:
            ui32GrpBit = 0x2;
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE, 
                                            SDATM1_OE, Tristate));
            break;
        case BRAP_OutputPort_eI2s2:
            ui32GrpBit = 0x4;
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE, 
                                            SDATM2_OE, Tristate));
            break;
	    case BRAP_OutputPort_eI2s3:
            ui32GrpBit = 0x8;
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE, 
                                            SDATM3_OE, Tristate));
            break;
        default:
            BDBG_ERR(("BRAP_OP_P_I2sStop_Multi: Unhandled OpPort %d",
                hOp->eOutputPort));
            BDBG_ASSERT(0);
    }
            
    BRAP_Write32 (hOp->hRegister, BCHP_AUD_FMM_MISC_SEROUT_OE, ui32RegVal);

    /* Clear the corresponding bit in OP_CTRL_I2SM_GROUPING */
    ui32RegVal = BRAP_Read32(hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_I2SM_GROUPING);
    ui32RegVal &= ~(ui32GrpBit);
    BRAP_Write32(hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_I2SM_GROUPING, ui32RegVal);

    /* Disable I2S upstream request */
    ret = BRAP_OP_P_EnableStream (hOp, false);

    BDBG_LEAVE (BRAP_OP_P_I2sStop_Multi);
    return ret;
}

static BERR_Code
BRAP_OP_P_I2sStart_Stereo(
    BRAP_OP_P_Handle                hOp,        /*[in] Output port handle */
    const BRAP_OP_P_I2sParams       *pParams    /*[in] Start time parameters */
)
{
    BERR_Code               ret = BERR_SUCCESS;
    uint32_t                ui32RegVal = 0;
    BRAP_OP_P_I2sParams     sParams;
    bool                    bWdRecovery = false ;
    BRAP_OP_P_I2sSettings   sSettings;

    unsigned int            uiTemp =0;


    BDBG_ENTER (BRAP_OP_P_I2sStart_Stereo);
    BDBG_ASSERT (hOp);

    bWdRecovery = BRAP_FMM_P_GetWatchdogRecoveryFlag (hOp->hFmm);
    if (false == bWdRecovery)
    { 
        BDBG_ASSERT (pParams);
        hOp->uOpParams.sI2s = *pParams;
        sParams = *pParams;
    }
    else
    {
        sParams = hOp->uOpParams.sI2s; 
    }

    sSettings.sExtSettings = hOp->hFmm->hRap->sOutputSettings[hOp->eOutputPort].
                                               uOutputPortSettings.sI2sSettings;

    /* Set SCLKS_PER_1FS_DIV32 field. This field times 32 is the number 
       of sclk cycles in one Fs period. For example, program this register
       to 2 for a 64fs bit clock. */
    switch (sSettings.sExtSettings.eSClkRate)
    {
        case BRAP_OP_SClkRate_e64Fs: 
            uiTemp = 64;
            break;
        case BRAP_OP_SClkRate_e128Fs: 
            uiTemp = 128;
            break;
        default:
            BDBG_ERR (("BRAP_OP_P_I2sOpen_Stereo(): Incorrect SClk %d", 
                        sSettings.sExtSettings.eSClkRate));
            return BERR_TRACE (BERR_NOT_SUPPORTED);
    }
    
    /* Set MCLK/(2*SCLK) ratio in MCLK_RATE field */
    switch (sSettings.sExtSettings.eMClkRate)
    {
        case BRAP_OP_MClkRate_e128Fs:
           uiTemp = (128 / (2 * uiTemp));
           break; 
        case BRAP_OP_MClkRate_e256Fs:
           uiTemp = (256 / (2 * uiTemp));
           break; 
        case BRAP_OP_MClkRate_e384Fs:
           uiTemp = (384 / (2 * uiTemp));
           break; 
        case BRAP_OP_MClkRate_e512Fs:
           uiTemp = (512 / (2 * uiTemp));
           break; 
        default:
            BDBG_ERR (("BRAP_OP_P_I2sOpen_Stereo(): Incorrect MClk %d", 
                        sSettings.sExtSettings.eMClkRate));
            return BERR_TRACE (BERR_NOT_SUPPORTED);
    }
    
    switch (uiTemp)
    {
        case 1:
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO, 
                                            MCLK_RATE, MCLK_128fs_SCLK_64fs));
            break;     
        case 2:
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO, 
                                            MCLK_RATE, MCLK_256fs_SCLK_64fs));
            break;     
        case 3:
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO, 
                                            MCLK_RATE, MCLK_384fs_SCLK_64fs));
            break;     
        case 4:
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO, 
                                            MCLK_RATE, MCLK_512fs_SCLK_64fs));
            break;     
        default:
            BDBG_ERR (("BRAP_OP_P_I2sOpen_Stereo(): Unsupported MClk/SClk Ratio %d", 
                        uiTemp));
            return BERR_TRACE (BERR_NOT_SUPPORTED);
    } 

  
    /* Set the PLL */
    /* The PLL Channels are choosen in the following way.
        
       If MClock rate is 128 Fs, Channel 1 should be selected,
       If MClock rate is 256 Fs, Channel 2 should be selected,
       If MClock rate is 512 Fs, Channel 3 should be selected,
 
       Basically Channels 1,2 and 3 correspond to 128Fs, 256Fs and 512Fs respectively.
 
       (MClock 384Fs can be set by user programming only).
    */
    switch (sSettings.sExtSettings.ePll)
    {
        case BRAP_OP_Pll_ePll0:
            
        /* Set MCLK rate and PLL Channels */
        switch (sSettings.sExtSettings.eMClkRate)
        {
            case BRAP_OP_MClkRate_e128Fs:
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO, 
                                            PLLCLKSEL, PLL0_ch1));
                break;     
            case BRAP_OP_MClkRate_e256Fs:
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO, 
                                            PLLCLKSEL, PLL0_ch2));
                break;     
            case BRAP_OP_MClkRate_e512Fs:
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO, 
                                            PLLCLKSEL, PLL0_ch3));
                break;     
            default:
                BDBG_ERR (("BRAP_OP_P_I2sOpen_Stereo(): Incorrect MClk Rate %d", 
                            sSettings.sExtSettings.eMClkRate));
                return BERR_TRACE (BERR_NOT_SUPPORTED);
        }     
            
        break;
        case BRAP_OP_Pll_ePll1:
        
        /* Set MCLK rate and PLL Channels */
        switch (sSettings.sExtSettings.eMClkRate)
        {
            case BRAP_OP_MClkRate_e128Fs:
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO, 
                                            PLLCLKSEL, PLL1_ch1));
                break;     
            case BRAP_OP_MClkRate_e256Fs:
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO, 
                                            PLLCLKSEL, PLL1_ch2));
                break;     
            case BRAP_OP_MClkRate_e512Fs:
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO, 
                                            PLLCLKSEL, PLL1_ch3));
                break;     
            default:
                BDBG_ERR (("BRAP_OP_P_I2sOpen_Stereo(): Incorrect MClk Rate %d", 
                            sSettings.sExtSettings.eMClkRate));
                return BERR_TRACE (BERR_NOT_SUPPORTED);
        } 

        break;
       default:
            BDBG_ERR (("BRAP_OP_P_I2sOpen_Stereo(): Incorrect PLL chosen %d", 
                        sSettings.sExtSettings.ePll));
            return BERR_TRACE (BERR_NOT_SUPPORTED);
    }

    BRAP_Write32 (hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO,ui32RegVal);

    /* Configure the on-the-fly paramters for I2S port */
    BRAP_OP_P_I2sHWConfig_Stereo (hOp, &sSettings);

    /* Read current value of the I2S config register */
    ui32RegVal = BRAP_Read32(hOp->hRegister,BCHP_AUD_FMM_OP_CTRL_I2S_CFG_STEREO);
    
    if ((sParams.uiBitsPerSample > 0) && (sParams.uiBitsPerSample < 32))
    {
        ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_I2S_CFG_STEREO, 
                                        BITS_PER_SAMPLE, 
                                        sParams.uiBitsPerSample));    
    }
    else if (sParams.uiBitsPerSample == 32)
    {
        ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_I2S_CFG_STEREO, 
                                        BITS_PER_SAMPLE, 
                                        0));    
    }
    else 
    {
        BDBG_ERR (("BRAP_OP_P_I2sStart(): Incorrect Bits per Sample %d", 
                    sParams.uiBitsPerSample));
        return BERR_TRACE (BERR_NOT_SUPPORTED);
    }

    if (hOp->hFmm->hRap->bOutputMute[hOp->eOutputPort] == false)
    {

        /* Enable DATA_ENABLE only if ouput port is unMuted */
        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2S_CFG_STEREO, 
                                    DATA_ENABLE, Enable));
    }

    /* Write to the register */
    BRAP_Write32 (hOp->hRegister,BCHP_AUD_FMM_OP_CTRL_I2S_CFG_STEREO,ui32RegVal);

    /* Read-Modify-Write AUD_FMM_MISC_SEROUT_OE */
    ui32RegVal = BRAP_Read32 (hOp->hRegister, BCHP_AUD_FMM_MISC_SEROUT_OE);

    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE,LRCKS0_OE, Drive));            
    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE,SCLKS0_OE, Drive));            
    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE,SDATS0_OE, Drive));

 
    /* 
        Not sure why but I2S Stereo requires the Multi 0 to be enabled to work 
        for 3548. This needs to be cross checked with KK.
    */
    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE,LRCKM0_OE, Drive));            
    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE,SCLKM0_OE, Drive));            
    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE,SDATM0_OE, Drive));
            
    BRAP_Write32 (hOp->hRegister, BCHP_AUD_FMM_MISC_SEROUT_OE, ui32RegVal);

    /* Program the FCI ID of the Input Mixer */
    ui32RegVal = BRAP_Read32(hOp->hRegister, BCHP_AUD_FMM_IOP_CTRL_MIX_IOP_IDMAP76);

    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_IOP_CTRL_MIX_IOP_IDMAP76, STREAM7));
    ui32RegVal |= BCHP_FIELD_DATA(AUD_FMM_IOP_CTRL_MIX_IOP_IDMAP76, STREAM7, 
                                                hOp->uOpParams.sI2s.ui32InputFciId);
    BRAP_Write32(hOp->hRegister, BCHP_AUD_FMM_IOP_CTRL_MIX_IOP_IDMAP76, ui32RegVal);
#if (BRAP_P_RESET_IOP_SM==1)
	/* To prevent last sample from previous run getting played again, reset HW internal state
	 * machine by writing 1 to INIT_SM bit corresponding to this output port. Reset this bit immediately */
	ui32RegVal = BRAP_Read32( hOp->hRegister, BCHP_AUD_FMM_IOP_CTRL_MIX_IOP_FCI_CTRL );
	ui32RegVal &= ~( BCHP_MASK( AUD_FMM_IOP_CTRL_MIX_IOP_FCI_CTRL, INIT_SM7 ));
	ui32RegVal |= BCHP_FIELD_ENUM( AUD_FMM_IOP_CTRL_MIX_IOP_FCI_CTRL, INIT_SM7, Init );
	BRAP_Write32( hOp->hRegister, BCHP_AUD_FMM_IOP_CTRL_MIX_IOP_FCI_CTRL, ui32RegVal );
	/* Reset immediately */
	ui32RegVal &= ~( BCHP_MASK( AUD_FMM_IOP_CTRL_MIX_IOP_FCI_CTRL, INIT_SM7 ));
	ui32RegVal |= BCHP_FIELD_ENUM( AUD_FMM_IOP_CTRL_MIX_IOP_FCI_CTRL, INIT_SM7, Normal );
	BRAP_Write32( hOp->hRegister, BCHP_AUD_FMM_IOP_CTRL_MIX_IOP_FCI_CTRL, ui32RegVal );
#endif

#if(BRAP_7405_FAMILY != 1)
    /* Program the Timebase */
    ret = BRAP_OP_P_ProgramTimebase (hOp, hOp->uOpSettings.sI2s.sExtSettings.ePll,
                                     sParams.eTimebase);
#endif

    /* Enable I2S upstream request */
    ret = BRAP_OP_P_EnableStream (hOp, true);
    BDBG_LEAVE (BRAP_OP_P_I2sStart_Stereo);
    return ret;
}

static BERR_Code
BRAP_OP_P_I2sStop_Stereo(
    BRAP_OP_P_Handle        hOp             /*[in] Output port handle */
)
{
    BERR_Code       ret = BERR_SUCCESS;
    uint32_t        ui32RegVal = 0;

    BDBG_ENTER (BRAP_OP_P_I2sStop_Stereo);
    BDBG_ASSERT (hOp);
   
    /* Program BCHP_AUD_FMM_OP_CTRL_I2S_CFG_STEREO as following (this is to Disable I2S)
       DATA_ENABLE = Disable */
    /* Read current value of the I2S config register */
    ui32RegVal = BRAP_Read32 (hOp->hRegister,BCHP_AUD_FMM_OP_CTRL_I2S_CFG_STEREO);
    
    /* Disable DATA_ENABLE */
    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2S_CFG_STEREO,DATA_ENABLE, Disable));

    /* Write to the register */
    BRAP_Write32 (hOp->hRegister,BCHP_AUD_FMM_OP_CTRL_I2S_CFG_STEREO,ui32RegVal);

    /* Read-Modify-Write AUD_FMM_MISC_SEROUT_OE */
    ui32RegVal = BRAP_Read32 (hOp->hRegister, BCHP_AUD_FMM_MISC_SEROUT_OE);

    /* Read-Modify-Write AUD_FMM_MISC_SEROUT_OE */
    ui32RegVal = BRAP_Read32 (hOp->hRegister, BCHP_AUD_FMM_MISC_SEROUT_OE);

    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE,LRCKS0_OE, Tristate));            
    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE,SCLKS0_OE, Tristate));            
    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE,SDATS0_OE, Tristate));

    /* 
        Not sure why but I2S Stereo requires the Multi 0 to be enabled to work 
        for 3548. This needs to be cross checked with KK.
    */
    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE,LRCKM0_OE, Tristate));            
    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE,SCLKM0_OE, Tristate));            
    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE,SDATM0_OE, Tristate));
            
    BRAP_Write32 (hOp->hRegister, BCHP_AUD_FMM_MISC_SEROUT_OE, ui32RegVal);
    
    /* Disable I2S upstream request */
    ret = BRAP_OP_P_EnableStream (hOp, false);
    BDBG_LEAVE (BRAP_OP_P_I2sStop_Stereo);
    return ret;
}
#else

#if (BRAP_7420_FAMILY !=1)      

BERR_Code
BRAP_OP_P_I2sHWConfig_Multi (
    BRAP_OP_P_Handle      hOp,               /*[in] Output port handle */
    const BRAP_OP_P_I2sSettings * pSettings  /*[in] Open time parameters */
)
{
    BERR_Code               ret = BERR_SUCCESS;
    uint32_t                ui32RegVal = 0;
    unsigned int            uiTemp = 0,uiOffset=0;
    BRAP_OP_P_I2sSettings   sSettings;
    bool                    bWdRecovery = false,bI2sMulti0=true;

    BDBG_ENTER (BRAP_OP_P_I2sHWConfig_Multi);
    BDBG_ASSERT (hOp);

    bWdRecovery = BRAP_FMM_P_GetWatchdogRecoveryFlag (hOp->hFmm);
    
    if (false == bWdRecovery)
    {   /* If not in WatchDog recovery */  
        BDBG_ASSERT (pSettings);
        sSettings = *pSettings;
    }
    else
    {
        sSettings = hOp->uOpSettings.sI2s;
    }

	switch(hOp->eOutputPort)
    {
        case BRAP_OutputPort_eI2s0:
        case BRAP_OutputPort_eI2s1:
        case BRAP_OutputPort_eI2s2:
        case BRAP_OutputPort_eI2s3:
 		    bI2sMulti0 = true;
            uiOffset =0;
            break;
        case BRAP_OutputPort_eI2s5:
        case BRAP_OutputPort_eI2s6:
        case BRAP_OutputPort_eI2s7:
        case BRAP_OutputPort_eI2s8:
     	    bI2sMulti0 = false;
#if(BRAP_7405_FAMILY == 1)
            uiOffset = 0;
#else
            uiOffset = (BCHP_AUD_FMM_OP_CTRL_I2SM1_CFG - 
                                                BCHP_AUD_FMM_OP_CTRL_I2SM0_CFG);
#endif
            break;
        default:
            BDBG_ASSERT(0);
    }

    /* Program I2SM_CROSSBAR */
    ui32RegVal = BRAP_Read32(hOp->hRegister,
                                (BCHP_AUD_FMM_OP_CTRL_I2SM0_CROSSBAR+uiOffset));
    
    /* Set Channel Order */
    switch (hOp->eOutputPort)
    {
        case BRAP_OutputPort_eI2s0:
            ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2SM0_CROSSBAR,OUT0_R));
            ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2SM0_CROSSBAR,OUT0_L));
            switch (sSettings.sExtSettings.eChannelOrder)
            {
                case BRAP_OP_ChannelOrder_eSwap:
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM0_CROSSBAR, 
                                                    OUT0_R, In0_l));
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM0_CROSSBAR, 
                                                    OUT0_L, In0_r));
                    break;
                case BRAP_OP_ChannelOrder_eBothLeft:
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM0_CROSSBAR, 
                                                    OUT0_R, In0_l));
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM0_CROSSBAR, 
                                                    OUT0_L, In0_l));
                    break;
                case BRAP_OP_ChannelOrder_eBothRight:
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM0_CROSSBAR, 
                                                    OUT0_R, In0_r));
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM0_CROSSBAR, 
                                                    OUT0_L, In0_r));
                    break;
                case BRAP_OP_ChannelOrder_eNormal:
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM0_CROSSBAR, 
                                                    OUT0_R, In0_r));
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM0_CROSSBAR, 
                                                    OUT0_L, In0_l));
                    break;
                default:
                    BDBG_ERR (("BRAP_OP_P_I2sHWConfig_Multi(): Incorrect Channel Order %d", 
                                sSettings.sExtSettings.eChannelOrder));
                    return BERR_TRACE (BERR_NOT_SUPPORTED);
            }
            break;
        case BRAP_OutputPort_eI2s1:
            ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2SM0_CROSSBAR,OUT1_R));
            ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2SM0_CROSSBAR,OUT1_L));

            switch (sSettings.sExtSettings.eChannelOrder)
            {
                case BRAP_OP_ChannelOrder_eSwap:
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM0_CROSSBAR, 
                                                    OUT1_R, In1_l));
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM0_CROSSBAR, 
                                                    OUT1_L, In1_r));
                    break;
                case BRAP_OP_ChannelOrder_eBothLeft:
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM0_CROSSBAR, 
                                                    OUT1_R, In1_l));
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM0_CROSSBAR, 
                                                    OUT1_L, In1_l));
                    break;
                case BRAP_OP_ChannelOrder_eBothRight:
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM0_CROSSBAR, 
                                                    OUT1_R, In1_r));
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM0_CROSSBAR, 
                                                    OUT1_L, In1_r));
                    break;
                case BRAP_OP_ChannelOrder_eNormal:
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM0_CROSSBAR, 
                                                    OUT1_R, In1_r));
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM0_CROSSBAR, 
                                                    OUT1_L, In1_l));
                    break;
                default:
                    BDBG_ERR (("BRAP_OP_P_I2sHWConfig_Multi(): Incorrect Channel Order %d", 
                                sSettings.sExtSettings.eChannelOrder));
                    return BERR_TRACE (BERR_NOT_SUPPORTED);
            }
            break;
        case BRAP_OutputPort_eI2s2:
			ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2SM0_CROSSBAR,OUT2_R));
            ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2SM0_CROSSBAR,OUT2_L));
            switch (sSettings.sExtSettings.eChannelOrder)
            {
                case BRAP_OP_ChannelOrder_eSwap:
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM0_CROSSBAR, 
                                                    OUT2_R, In2_l));
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM0_CROSSBAR, 
                                                    OUT2_L, In2_r));
                    break;
                case BRAP_OP_ChannelOrder_eBothLeft:
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM0_CROSSBAR, 
                                                    OUT2_R, In2_l));
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM0_CROSSBAR, 
                                                    OUT2_L, In2_l));
                    break;
                case BRAP_OP_ChannelOrder_eBothRight:
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM0_CROSSBAR, 
                                                    OUT2_R, In2_r));
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM0_CROSSBAR, 
                                                    OUT2_L, In2_r));
                    break;
                case BRAP_OP_ChannelOrder_eNormal:
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM0_CROSSBAR, 
                                                    OUT2_R, In2_r));
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM0_CROSSBAR, 
                                                    OUT2_L, In2_l));
                    break;
                default:
                    BDBG_ERR (("BRAP_OP_P_I2sHWConfig_Multi(): Incorrect Channel Order %d", 
                                sSettings.sExtSettings.eChannelOrder));
                    return BERR_TRACE (BERR_NOT_SUPPORTED);
            }
            break;
	    case BRAP_OutputPort_eI2s3: 
			ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2SM0_CROSSBAR,OUT3_R));
            ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2SM0_CROSSBAR,OUT3_L));
            switch (sSettings.sExtSettings.eChannelOrder)
            {
                case BRAP_OP_ChannelOrder_eSwap:
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM0_CROSSBAR, 
                                                    OUT3_R, In3_l));
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM0_CROSSBAR, 
                                                    OUT3_L, In3_r));
                    break;
                case BRAP_OP_ChannelOrder_eBothLeft:
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM0_CROSSBAR, 
                                                    OUT3_R, In3_l));
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM0_CROSSBAR, 
                                                    OUT3_L, In3_l));
                    break;
                case BRAP_OP_ChannelOrder_eBothRight:
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM0_CROSSBAR, 
                                                    OUT3_R, In3_r));
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM0_CROSSBAR, 
                                                    OUT3_L, In3_r));
                    break;
                case BRAP_OP_ChannelOrder_eNormal:
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM0_CROSSBAR, 
                                                    OUT3_R, In3_r));
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM0_CROSSBAR, 
                                                    OUT3_L, In3_l));
                    break;
                default:
                    BDBG_ERR (("BRAP_OP_P_I2sHWConfig_Multi(): Incorrect Channel Order %d", 
                                sSettings.sExtSettings.eChannelOrder));
                    return BERR_TRACE (BERR_NOT_SUPPORTED);
            }
            break;
#if(BRAP_7405_FAMILY != 1)
		case BRAP_OutputPort_eI2s5:
        BDBG_MSG(("BRAP_OP_P_I2sHWConfig_Multi(): eMappedChPair %d for eOp=%d", 
                            sSettings.sExtSettings.eMappedChPair, hOp->eOutputPort));

        ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2SM1_CROSSBAR,OUT0_R));
        ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2SM1_CROSSBAR,OUT0_L));

        switch(sSettings.sExtSettings.eMappedChPair)
        {
            case BRAP_OutputChannelPair_eLR:
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM1_CROSSBAR, 
                                                OUT0_R, In0_r));
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM1_CROSSBAR, 
                                                OUT0_L, In0_l));
                break;
            case BRAP_OutputChannelPair_eLRSurround:
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM1_CROSSBAR, 
                                                OUT0_R, In1_r));
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM1_CROSSBAR, 
                                                OUT0_L, In1_l));
                break;
            case BRAP_OutputChannelPair_eCentreLF:
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM1_CROSSBAR, 
                                                OUT0_R, In2_r));
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM1_CROSSBAR, 
                                                OUT0_L, In2_l));
                break;
            case BRAP_OutputChannelPair_eLRRear:
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM1_CROSSBAR, 
                                                OUT0_R, In3_r));
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM1_CROSSBAR, 
                                                OUT0_L, In3_l));
                break;
            default:
                BDBG_ERR (("BRAP_OP_P_I2sHWConfig_Multi(): Incorrect eMappedChPair %d for eOp=%d", 
                            sSettings.sExtSettings.eMappedChPair, hOp->eOutputPort));
                return BERR_TRACE (BERR_NOT_SUPPORTED);
        }/* switch eMappedChPair */
        break;
		case BRAP_OutputPort_eI2s6:
        BDBG_MSG (("BRAP_OP_P_I2sHWConfig_Multi(): eMappedChPair %d for eOp=%d", 
                            sSettings.sExtSettings.eMappedChPair, hOp->eOutputPort));

        ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2SM1_CROSSBAR,OUT1_R));
        ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2SM1_CROSSBAR,OUT1_L));

        switch(sSettings.sExtSettings.eMappedChPair)
        {
            case BRAP_OutputChannelPair_eLR:
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM1_CROSSBAR, 
                                                OUT1_R, In0_r));
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM1_CROSSBAR, 
                                                OUT1_L, In0_l));
                break;
            case BRAP_OutputChannelPair_eLRSurround:
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM1_CROSSBAR, 
                                                OUT1_R, In1_r));
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM1_CROSSBAR, 
                                                OUT1_L, In1_l));
                break;
            case BRAP_OutputChannelPair_eCentreLF:
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM1_CROSSBAR, 
                                                OUT1_R, In2_r));
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM1_CROSSBAR, 
                                                OUT1_L, In2_l));
                break;
            case BRAP_OutputChannelPair_eLRRear:
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM1_CROSSBAR, 
                                                OUT1_R, In3_r));
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM1_CROSSBAR, 
                                                OUT1_L, In3_l));
                break;
            default:
                BDBG_ERR (("BRAP_OP_P_I2sHWConfig_Multi(): Incorrect eMappedChPair %d for eOp=%d", 
                            sSettings.sExtSettings.eMappedChPair, hOp->eOutputPort));
                return BERR_TRACE (BERR_NOT_SUPPORTED);
        }/* switch eMappedChPair */
        break;
		case BRAP_OutputPort_eI2s7:
        BDBG_MSG (("BRAP_OP_P_I2sHWConfig_Multi(): eMappedChPair %d for eOp=%d", 
                            sSettings.sExtSettings.eMappedChPair, hOp->eOutputPort));

        ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2SM1_CROSSBAR,OUT2_R));
        ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2SM1_CROSSBAR,OUT2_L));

        switch(sSettings.sExtSettings.eMappedChPair)
        {
            case BRAP_OutputChannelPair_eLR:
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM1_CROSSBAR, 
                                                OUT2_R, In0_r));
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM1_CROSSBAR, 
                                                OUT2_L, In0_l));
                break;
            case BRAP_OutputChannelPair_eLRSurround:
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM1_CROSSBAR, 
                                                OUT2_R, In1_r));
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM1_CROSSBAR, 
                                                OUT2_L, In1_l));
                break;
            case BRAP_OutputChannelPair_eCentreLF:
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM1_CROSSBAR, 
                                                OUT2_R, In2_r));
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM1_CROSSBAR, 
                                                OUT2_L, In2_l));
                break;
            case BRAP_OutputChannelPair_eLRRear:
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM1_CROSSBAR, 
                                                OUT2_R, In3_r));
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM1_CROSSBAR, 
                                                OUT2_L, In3_l));
                break;
            default:
                BDBG_ERR (("BRAP_OP_P_I2sHWConfig_Multi(): Incorrect eMappedChPair %d for eOp=%d", 
                            sSettings.sExtSettings.eMappedChPair, hOp->eOutputPort));
                return BERR_TRACE (BERR_NOT_SUPPORTED);
        }/* switch eMappedChPair */
        break;
		case BRAP_OutputPort_eI2s8:
        BDBG_MSG (("BRAP_OP_P_I2sHWConfig_Multi(): eMappedChPair %d for eOp=%d", 
                            sSettings.sExtSettings.eMappedChPair, hOp->eOutputPort));

        ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2SM1_CROSSBAR,OUT3_R));
        ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2SM1_CROSSBAR,OUT3_L));

        switch(sSettings.sExtSettings.eMappedChPair)
        {
            case BRAP_OutputChannelPair_eLR:
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM1_CROSSBAR, 
                                                OUT3_R, In0_r));
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM1_CROSSBAR, 
                                                OUT3_L, In0_l));
                break;
            case BRAP_OutputChannelPair_eLRSurround:
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM1_CROSSBAR, 
                                                OUT3_R, In1_r));
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM1_CROSSBAR, 
                                                OUT3_L, In1_l));
                break;
            case BRAP_OutputChannelPair_eCentreLF:
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM1_CROSSBAR, 
                                                OUT3_R, In2_r));
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM1_CROSSBAR, 
                                                OUT3_L, In2_l));
                break;
            case BRAP_OutputChannelPair_eLRRear:
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM1_CROSSBAR, 
                                                OUT3_R, In3_r));
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM1_CROSSBAR, 
                                                OUT3_L, In3_l));
                break;
            default:
                BDBG_ERR (("BRAP_OP_P_I2sHWConfig_Multi(): Incorrect eMappedChPair %d for eOp=%d", 
                            sSettings.sExtSettings.eMappedChPair, hOp->eOutputPort));
                return BERR_TRACE (BERR_NOT_SUPPORTED);
        }/* switch eMappedChPair */
        break;
#endif            
        default:
            BDBG_ERR (("BRAP_OP_P_I2sHWConfig_Multi(): Incorrect Output Port %d", 
                                                            hOp->eOutputPort));
            return BERR_TRACE (BERR_NOT_SUPPORTED);
    }
    BRAP_Write32(hOp->hRegister,(BCHP_AUD_FMM_OP_CTRL_I2SM0_CROSSBAR + uiOffset)
                                                                   ,ui32RegVal);


	if(true==bI2sMulti0)
	{
	    ui32RegVal = 0;
	    ui32RegVal = BRAP_Read32(hOp->hRegister,BCHP_AUD_FMM_OP_CTRL_I2SM0_CFG);

	    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2SM0_CFG,DATA_JUSTIFICATION));
	    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2SM0_CFG,DATA_ALIGNMENT));
	    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2SM0_CFG,SCLK_POLARITY));
	    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2SM0_CFG,LRCK_POLARITY));
	    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2SM0_CFG,SCLKS_PER_1FS_DIV32));

	    /* Data Justification */
	    if (sSettings.sExtSettings.bLsbAtLRClk == true)
	    {
	        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM0_CFG, 
	                                        DATA_JUSTIFICATION, LSB));
	    }
	    else 
	    {
	        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM0_CFG, 
	                                        DATA_JUSTIFICATION, MSB));
	    }

	    /* Data Allignment */
	    if (sSettings.sExtSettings.bAlignedToLRClk == false)
	    {
	        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM0_CFG, 
	                                        DATA_ALIGNMENT, Delayed));
	    }
	    else 
	    {
	        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM0_CFG, 
	                                        DATA_ALIGNMENT, Aligned));
	    }

	    /* SCLK & LRCLK Polarity */
	    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM0_CFG, 
	                                    SCLK_POLARITY, Falling_aligned_with_sdata));

	    if (sSettings.sExtSettings.bLRClkPolarity == true)
	    {
	        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM0_CFG, 
	                                        LRCK_POLARITY, High_for_left));
	    }
	    else 
	    {
	        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM0_CFG, 
	                                        LRCK_POLARITY, Low_for_left));
	    }

	    /* Set SCLKS_PER_1FS_DIV32 field. This field times 32 is the number 
	       of sclk cycles in one Fs period. For example, program this register
	       to 2 for a 64fs bit clock. */
	    switch (sSettings.sExtSettings.eSClkRate)
	    {
	        case BRAP_OP_SClkRate_e64Fs: 
	            uiTemp = 64;
	            break;
	        case BRAP_OP_SClkRate_e128Fs: 
	            uiTemp = 128;
	            break;
	        default:
	            BDBG_ERR (("BRAP_OP_P_I2sHWConfig_Multi(): Incorrect SClk %d", 
	                        sSettings.sExtSettings.eSClkRate));
	            return BERR_TRACE (BERR_NOT_SUPPORTED);
	    }
	    ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_I2SM0_CFG, 
	                                    SCLKS_PER_1FS_DIV32,(uiTemp/32)));

	    BRAP_Write32 (hOp->hRegister,BCHP_AUD_FMM_OP_CTRL_I2SM0_CFG,ui32RegVal);  
	}
	else
	{
		ui32RegVal = 0;
#if(BRAP_7405_FAMILY != 1)
	    ui32RegVal = BRAP_Read32(hOp->hRegister,BCHP_AUD_FMM_OP_CTRL_I2SM1_CFG);

	    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2SM1_CFG,DATA_JUSTIFICATION));
	    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2SM1_CFG,DATA_ALIGNMENT));
	    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2SM1_CFG,SCLK_POLARITY));
	    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2SM1_CFG,LRCK_POLARITY));
	    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2SM1_CFG,SCLKS_PER_1FS_DIV32));

	    /* Data Justification */
	    if (sSettings.sExtSettings.bLsbAtLRClk == true)
	    {
	        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM1_CFG, 
	                                        DATA_JUSTIFICATION, LSB));
	    }
	    else 
	    {
	        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM1_CFG, 
	                                        DATA_JUSTIFICATION, MSB));
	    }

	    /* Data Allignment */
	    if (sSettings.sExtSettings.bAlignedToLRClk == false)
	    {
	        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM1_CFG, 
	                                        DATA_ALIGNMENT, Delayed));
	    }
	    else 
	    {
	        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM1_CFG, 
	                                        DATA_ALIGNMENT, Aligned));
	    }

	    /* SCLK & LRCLK Polarity */
	    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM1_CFG, 
	                                    SCLK_POLARITY, Falling_aligned_with_sdata));

	    if (sSettings.sExtSettings.bLRClkPolarity == true)
	    {
	        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM1_CFG, 
	                                        LRCK_POLARITY, High_for_left));
	    }
	    else 
	    {
	        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM1_CFG, 
	                                        LRCK_POLARITY, Low_for_left));
	    }

	    /* Set SCLKS_PER_1FS_DIV32 field. This field times 32 is the number 
	       of sclk cycles in one Fs period. For example, program this register
	       to 2 for a 64fs bit clock. */
	    switch (sSettings.sExtSettings.eSClkRate)
	    {
	        case BRAP_OP_SClkRate_e64Fs: 
	            uiTemp = 64;
	            break;
	        case BRAP_OP_SClkRate_e128Fs: 
	            uiTemp = 128;
	            break;
	        default:
	            BDBG_ERR (("BRAP_OP_P_I2sHWConfig_Multi(): Incorrect SClk %d", 
	                        sSettings.sExtSettings.eSClkRate));
	            return BERR_TRACE (BERR_NOT_SUPPORTED);
	    }
	    ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_I2SM1_CFG, 
	                                    SCLKS_PER_1FS_DIV32,(uiTemp/32)));

	    BRAP_Write32 (hOp->hRegister,BCHP_AUD_FMM_OP_CTRL_I2SM1_CFG,ui32RegVal);  
#endif        
	}

    /* dont store the settings until everything's correct */
    if ( false == bWdRecovery)
    {  
        hOp->uOpSettings.sI2s = *pSettings;
    }
    
    BDBG_LEAVE (BRAP_OP_P_I2sHWConfig_Multi);
    return ret;

}
#endif
BERR_Code
BRAP_OP_P_I2sHWConfig_Stereo (
    BRAP_OP_P_Handle      hOp,               /*[in] Output port handle */
    const BRAP_OP_P_I2sSettings * pSettings  /*[in] Open time parameters */
)
{
     BERR_Code               ret = BERR_SUCCESS;
    uint32_t                ui32RegVal = 0;
    unsigned int            uiTemp = 0;
    BRAP_OP_P_I2sSettings   sSettings;
    bool                    bWdRecovery = false;
    unsigned int uiI2sStereoIndex=0;

    BDBG_ENTER (BRAP_OP_P_I2sHWConfig_Stereo);
    BDBG_ASSERT (hOp);

    bWdRecovery = BRAP_FMM_P_GetWatchdogRecoveryFlag (hOp->hFmm);
    
    if (false == bWdRecovery)
    {   /* If not in WatchDog recovery */  
        BDBG_ASSERT (pSettings);
        sSettings = *pSettings;
    }
    else
    {
        sSettings = hOp->uOpSettings.sI2s;
    }
    BRAP_OP_P_GetI2sStereoIndex(hOp->eOutputPort,&uiI2sStereoIndex);
    /* Set Channel Order */
    ui32RegVal = BRAP_Read32 (hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_I2SS0_CROSSBAR + uiI2sStereoIndex*16);

    ui32RegVal &= ~(BCHP_MASK (AUD_FMM_OP_CTRL_I2SS0_CROSSBAR,OUT_R));
    ui32RegVal &= ~(BCHP_MASK (AUD_FMM_OP_CTRL_I2SS0_CROSSBAR,OUT_L));
    
    switch (sSettings.sExtSettings.eChannelOrder)
    {           
        case BRAP_OP_ChannelOrder_eSwap:
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SS0_CROSSBAR, 
                                            OUT_R, In_l));
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SS0_CROSSBAR, 
                                            OUT_L, In_r));
            break;
        case BRAP_OP_ChannelOrder_eBothLeft:
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SS0_CROSSBAR, 
                                            OUT_R, In_l));
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SS0_CROSSBAR, 
                                            OUT_L, In_l));
            break;
        case BRAP_OP_ChannelOrder_eBothRight:
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SS0_CROSSBAR, 
                                            OUT_R, In_r));
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SS0_CROSSBAR, 
                                            OUT_L, In_r));
            break;
        case BRAP_OP_ChannelOrder_eNormal:
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SS0_CROSSBAR, 
                                            OUT_R, In_r));
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SS0_CROSSBAR, 
                                            OUT_L, In_l));
            break;
        default:
            BDBG_ERR (("BRAP_OP_P_I2sOpen_Stereo(): Incorrect Channel Order %d", 
                        sSettings.sExtSettings.eChannelOrder));
            return BERR_TRACE (BERR_NOT_SUPPORTED);
    }          

    BRAP_Write32 (hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_I2SS0_CROSSBAR + uiI2sStereoIndex * 16,ui32RegVal);

    ui32RegVal = 0;

    ui32RegVal = BRAP_Read32(hOp->hRegister,BCHP_AUD_FMM_OP_CTRL_I2SS0_CFG + uiI2sStereoIndex*16);

    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2SS0_CFG,DATA_JUSTIFICATION));
    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2SS0_CFG,DATA_ALIGNMENT));
    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2SS0_CFG,SCLK_POLARITY));
    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2SS0_CFG,LRCK_POLARITY));
    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2SS0_CFG,SCLKS_PER_1FS_DIV32));

    /* DATA JUSTIFICATION */
    if (sSettings.sExtSettings.bLsbAtLRClk == true)
    {
        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SS0_CFG, 
                                        DATA_JUSTIFICATION, LSB));
    }
    else 
    {
        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SS0_CFG, 
                                        DATA_JUSTIFICATION, MSB));
    }

    /* DATA ALIGNMENT */
    if (sSettings.sExtSettings.bAlignedToLRClk == false)
    {
        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SS0_CFG, 
                                        DATA_ALIGNMENT, Delayed));
    }
    else 
    {
        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SS0_CFG, 
                                        DATA_ALIGNMENT, Aligned));
    }

    /*SCLK and LRCK POLARITY*/
    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SS0_CFG, 
                                    SCLK_POLARITY, Falling_aligned_with_sdata));

    if (sSettings.sExtSettings.bLRClkPolarity == true)
    {
        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SS0_CFG, 
                                        LRCK_POLARITY, High_for_left));
    }
    else 
    {
        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SS0_CFG, 
                                        LRCK_POLARITY, Low_for_left));
    }       
 
    /* Set SCLKS_PER_1FS_DIV32 field. This field times 32 is the number 
       of sclk cycles in one Fs period. For example, program this register
       to 2 for a 64fs bit clock. */
    switch (sSettings.sExtSettings.eSClkRate)
    {
        case BRAP_OP_SClkRate_e64Fs: 
            uiTemp = 64;
            break;
        case BRAP_OP_SClkRate_e128Fs: 
            uiTemp = 128;
            break;
        default:
            BDBG_ERR (("BRAP_OP_P_I2sOpen_Stereo(): Incorrect SClk %d", 
                        sSettings.sExtSettings.eSClkRate));
            return BERR_TRACE (BERR_NOT_SUPPORTED);
    }
    ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_I2SS0_CFG, 
                                    SCLKS_PER_1FS_DIV32,(uiTemp/32)));

    BRAP_Write32 (hOp->hRegister,BCHP_AUD_FMM_OP_CTRL_I2SS0_CFG + uiI2sStereoIndex*16,ui32RegVal);     

    /* dont store the settings until everything's correct */
    if ( false == bWdRecovery)
    {  
        hOp->uOpSettings.sI2s = *pSettings;
    }
    
    BDBG_LEAVE (BRAP_OP_P_I2sHWConfig_Stereo);
    return ret;
}

#if (BRAP_7420_FAMILY !=1)      
static BERR_Code
BRAP_OP_P_I2sOpen_Multi(
    BRAP_OP_P_Handle                hOp,            /*[in] Output port handle */
    const BRAP_OP_P_I2sSettings     *pSettings      /*[in] Open time parameters */
)
{
        BERR_Code               ret = BERR_SUCCESS;
    uint32_t                ui32RegVal = 0;
    unsigned int            uiOffset =0;
    bool                    bI2sMulti0 = false;

    BDBG_ENTER (BRAP_OP_P_I2sOpen_Multi);
    BDBG_ASSERT (hOp);
	BSTD_UNUSED(pSettings);

    switch(hOp->eOutputPort)
    {
        case BRAP_OutputPort_eI2s0:
        case BRAP_OutputPort_eI2s1:
        case BRAP_OutputPort_eI2s2:
        case BRAP_OutputPort_eI2s3:
            uiOffset =0;
            bI2sMulti0 = true;
            break;
#if(BRAP_7405_FAMILY != 1)
        case BRAP_OutputPort_eI2s5:
        case BRAP_OutputPort_eI2s6:
        case BRAP_OutputPort_eI2s7:
        case BRAP_OutputPort_eI2s8:
            uiOffset = (BCHP_AUD_FMM_OP_CTRL_I2SM1_CFG - 
                                                BCHP_AUD_FMM_OP_CTRL_I2SM0_CFG);
            bI2sMulti0 = false;
            break;
#endif            
        default:
            BDBG_ASSERT(0);
    }

    
    if (true == bI2sMulti0)
    {
        /* Clear the corresponding bit in OP_CTRL_I2SM_GROUPING */
        ui32RegVal = BRAP_Read32(hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_I2SM0_GROUPING);
        switch (hOp->eOutputPort)
        {
            case BRAP_OutputPort_eI2s0: ui32RegVal &= ~(0x1); break;
            case BRAP_OutputPort_eI2s1: ui32RegVal &= ~(0x2); break;
            case BRAP_OutputPort_eI2s2: ui32RegVal &= ~(0x4); break;
            case BRAP_OutputPort_eI2s3: ui32RegVal &= ~(0x8); break;       
            default: return BERR_TRACE (BERR_NOT_SUPPORTED);
        }/* switch */
        BRAP_Write32(hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_I2SM0_GROUPING, ui32RegVal);
    }

    ui32RegVal = 0;
    ui32RegVal = BRAP_Read32(hOp->hRegister,BCHP_AUD_FMM_OP_CTRL_I2SM0_CFG);
       /* Enable CLOCK_ENABLE */
    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM0_CFG, 
                                    CLOCK_ENABLE, Enable));

    BRAP_Write32 (hOp->hRegister,(BCHP_AUD_FMM_OP_CTRL_I2SM0_CFG + uiOffset),
                                                                    ui32RegVal);     
    
    BDBG_LEAVE (BRAP_OP_P_I2sOpen_Multi);
    return ret; 
}
#endif
BERR_Code
BRAP_OP_P_GetI2sStereoIndex(
    BRAP_OutputPort                 eOutputPort,            /*[in] Output port  */
    unsigned int     *uiI2sStereoIndex      /*[in] Open time parameters */
)
{
    BERR_Code               ret = BERR_SUCCESS;

    BDBG_ENTER (BRAP_OP_P_GetI2sStereoIndex);
    switch(eOutputPort)
    {
#if (BRAP_7420_FAMILY !=1)      
    case    BRAP_OutputPort_eI2s0:
    case    BRAP_OutputPort_eI2s1:
    case    BRAP_OutputPort_eI2s2:
    case    BRAP_OutputPort_eI2s3:       
    case    BRAP_OutputPort_eI2s4:
        *uiI2sStereoIndex =0;
        break;
#else
case    BRAP_OutputPort_eI2s0:
    *uiI2sStereoIndex =0;
    break;    
case    BRAP_OutputPort_eI2s1:
    *uiI2sStereoIndex =1;
    break;
#endif
    default:
        BDBG_ERR(("Invalid I2s Port %d",eOutputPort));
        *uiI2sStereoIndex =0;        
        ret = BERR_INVALID_PARAMETER;
    }
    BDBG_LEAVE (BRAP_OP_P_GetI2sStereoIndex);
    return ret; 
}

static BERR_Code
BRAP_OP_P_I2sOpen_Stereo(
    BRAP_OP_P_Handle                hOp,            /*[in] Output port handle */
    const BRAP_OP_P_I2sSettings     *pSettings      /*[in] Open time parameters */
)
{
    BERR_Code               ret = BERR_SUCCESS;
    uint32_t                ui32RegVal = 0;

    BDBG_ENTER (BRAP_OP_P_I2sOpen_Stereo);
    BDBG_ASSERT (hOp);
	BSTD_UNUSED(pSettings);

	ui32RegVal = BRAP_Read32(hOp->hRegister,BCHP_AUD_FMM_OP_CTRL_I2SS0_CFG);
    /* Enable CLOCK_ENABLE */
    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SS0_CFG, 
                                    CLOCK_ENABLE, Enable));

    BRAP_Write32 (hOp->hRegister,BCHP_AUD_FMM_OP_CTRL_I2SS0_CFG,ui32RegVal);     
    
    BDBG_LEAVE (BRAP_OP_P_I2sOpen_Stereo);
    return ret; 
}

#if (BRAP_7420_FAMILY !=1)      
static BERR_Code
BRAP_OP_P_I2sStart_Multi(
    BRAP_OP_P_Handle                hOp,            /*[in] Output port handle */
    const BRAP_OP_P_I2sParams       *pParams        /*[in] Start time parameters */
)
{
    BERR_Code               ret = BERR_SUCCESS;
    uint32_t                ui32RegVal = 0, ui32GrpBit = 0, ui32DriveBit = 0;
    unsigned int            uiOffset =0,  ui32StreamId = 0;
    BRAP_OP_P_I2sParams     sParams;
    unsigned int            uiTemp= 0;

    BRAP_OP_P_I2sSettings   sSettings;
    bool                    bWdRecovery = false;
    bool                    bI2sMulti0 = false;

    BDBG_ENTER (BRAP_OP_P_I2sStart_Multi);
    BDBG_ASSERT (hOp);

    bWdRecovery = BRAP_FMM_P_GetWatchdogRecoveryFlag (hOp->hFmm);
    if (false == bWdRecovery)
    { 
        BDBG_ASSERT (pParams);
        hOp->uOpParams.sI2s = *pParams;
        sParams = *pParams;
    }
    else
    {
        sParams = hOp->uOpParams.sI2s; 
    }

	sSettings.sExtSettings = hOp->hFmm->hRap->sOutputSettings[hOp->eOutputPort].
                                               uOutputPortSettings.sI2sSettings;

    switch(hOp->eOutputPort)
    {
        case BRAP_OutputPort_eI2s0:
        case BRAP_OutputPort_eI2s1:
        case BRAP_OutputPort_eI2s2:
        case BRAP_OutputPort_eI2s3:
            uiOffset =0;
            bI2sMulti0 = true;
            break;
#if(BRAP_7405_FAMILY != 1)
        case BRAP_OutputPort_eI2s5:
        case BRAP_OutputPort_eI2s6:
        case BRAP_OutputPort_eI2s7:
        case BRAP_OutputPort_eI2s8:
            uiOffset = (BCHP_AUD_FMM_OP_CTRL_I2SM1_CFG - 
                                                BCHP_AUD_FMM_OP_CTRL_I2SM0_CFG);
            bI2sMulti0 = false;

            /* Program HBR_ENABLE bit */
            ui32RegVal = BRAP_Read32 (hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_STREAM_ROUTE);
            ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_STREAM_ROUTE,HBR_ENABLE));
            if(pParams->bHbrEnable == true)
            {
				ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_STREAM_ROUTE,
                                    HBR_ENABLE,
                                    Enable));
            }
            else
            {
				ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_STREAM_ROUTE,
                                    HBR_ENABLE,
                                    Disable));
            }
            BRAP_Write32 (hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_STREAM_ROUTE, ui32RegVal);
            break;
#endif
        default:
            BDBG_ASSERT(0);
    }

    /* Set SCLKS_PER_1FS_DIV32 field. This field times 32 is the number 
       of sclk cycles in one Fs period. For example, program this register
       to 2 for a 64fs bit clock. */
    switch (sSettings.sExtSettings.eSClkRate)
    {
        case BRAP_OP_SClkRate_e64Fs: 
            uiTemp = 64;
            break;
        case BRAP_OP_SClkRate_e128Fs: 
            uiTemp = 128;
            break;
        default:
            BDBG_ERR (("BRAP_OP_P_I2sStart_Multi(): Incorrect SClk %d", 
                        sSettings.sExtSettings.eSClkRate));
            return BERR_TRACE (BERR_NOT_SUPPORTED);
    }
    
    /* Set MCLK/(2*SCLK) ratio in MCLK_RATE field */
    switch (sSettings.sExtSettings.eMClkRate)
    {
        case BRAP_OP_MClkRate_e128Fs:
           uiTemp = (128 / (2 * uiTemp));
           break; 
        case BRAP_OP_MClkRate_e256Fs:
           uiTemp = (256 / (2 * uiTemp));
           break; 
        case BRAP_OP_MClkRate_e384Fs:
           uiTemp = (384 / (2 * uiTemp));
           break; 
        case BRAP_OP_MClkRate_e512Fs:
           uiTemp = (512 / (2 * uiTemp));
           break; 
        default:
            BDBG_ERR (("BRAP_OP_P_I2sStart_Multi(): Incorrect MClk %d", 
                        sSettings.sExtSettings.eMClkRate));
            return BERR_TRACE (BERR_NOT_SUPPORTED);
    }

    switch (uiTemp)
    {
        case 1:
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_I2S_MULTI0, 
                                            MCLK_RATE, MCLK_128fs_SCLK_64fs));
            break;     
        case 2:
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_I2S_MULTI0, 
                                            MCLK_RATE, MCLK_256fs_SCLK_64fs));
            break;     
        case 3:
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_I2S_MULTI0, 
                                            MCLK_RATE, MCLK_384fs_SCLK_64fs));
            break;     
        case 4:
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_I2S_MULTI0, 
                                            MCLK_RATE, MCLK_512fs_SCLK_64fs));
            break;     
        default:
            BDBG_ERR (("BRAP_OP_P_I2sStart_Multi(): Unsupported MClk/SClk Ratio %d", 
                        uiTemp));
            return BERR_TRACE (BERR_NOT_SUPPORTED);
    } 

  
    /* Set the PLL */
    /* The PLL Channels are choosen in the following way.
        
       If MClock rate is 128 Fs, Channel 1 should be selected,
       If MClock rate is 256 Fs, Channel 2 should be selected,
       If MClock rate is 512 Fs, Channel 3 should be selected,
 
       Basically Channels 1,2 and 3 correspond to 128Fs, 256Fs and 512Fs respectively.
 
       (MClock 384Fs can be set by user programming only).
    */
    switch (sSettings.sExtSettings.ePll)
    {
        case BRAP_OP_Pll_ePll0:
            
        /* Set MCLK rate and PLL Channels */
        switch (sSettings.sExtSettings.eMClkRate)
        {
            case BRAP_OP_MClkRate_e128Fs:
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_I2S_MULTI0, 
                                            PLLCLKSEL, PLL0_ch1));
                break;     
            case BRAP_OP_MClkRate_e256Fs:
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_I2S_MULTI0, 
                                            PLLCLKSEL, PLL0_ch2));
                break;     
            case BRAP_OP_MClkRate_e512Fs:
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_I2S_MULTI0, 
                                            PLLCLKSEL, PLL0_ch3));
                break;     
            default:
                BDBG_ERR (("BRAP_OP_P_I2sStart_Multi(): Incorrect MClk Rate %d", 
                            sSettings.sExtSettings.eMClkRate));
                return BERR_TRACE (BERR_NOT_SUPPORTED);
        }     
            
        break;
#if (BRAP_PLL1_SUPPORTED == 1)         
        case BRAP_OP_Pll_ePll1:
        
        /* Set MCLK rate and PLL Channels */
        switch (sSettings.sExtSettings.eMClkRate)
        {
            case BRAP_OP_MClkRate_e128Fs:
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_I2S_MULTI0, 
                                            PLLCLKSEL, PLL1_ch1));
                break;     
            case BRAP_OP_MClkRate_e256Fs:
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_I2S_MULTI0, 
                                            PLLCLKSEL, PLL1_ch2));
                break;     
            case BRAP_OP_MClkRate_e512Fs:
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_I2S_MULTI0, 
                                            PLLCLKSEL, PLL1_ch3));
                break;     
            default:
                BDBG_ERR (("BRAP_OP_P_I2sStart_Multi(): Incorrect MClk Rate %d", 
                            sSettings.sExtSettings.eMClkRate));
                return BERR_TRACE (BERR_NOT_SUPPORTED);
        } 

        break;
#endif        
       default:
            BDBG_ERR (("BRAP_OP_P_I2sStart_Multi(): Incorrect PLL chosen %d", 
                        sSettings.sExtSettings.ePll));
            return BERR_TRACE (BERR_NOT_SUPPORTED);
    }
    if (true == bI2sMulti0)
    {    		
    	BRAP_Write32 (hOp->hRegister,BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_I2S_MULTI0,ui32RegVal);
    }
#if(BRAP_7405_FAMILY != 1)
    else
    {
    	BRAP_Write32 (hOp->hRegister,BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_I2S_MULTI1,ui32RegVal);
    }
#endif    

	/* Configure the on-the-fly paramters for I2S port */
    BRAP_OP_P_I2sHWConfig_Multi (hOp, &sSettings);

    /* Read current value of the I2S config register */
    ui32RegVal = BRAP_Read32(hOp->hRegister,
                                   (BCHP_AUD_FMM_OP_CTRL_I2SM0_CFG + uiOffset));

    ui32RegVal &= ~((BCHP_MASK(AUD_FMM_OP_CTRL_I2SM0_CFG,BITS_PER_SAMPLE)) | 
                    (BCHP_MASK(AUD_FMM_OP_CTRL_I2SM0_CFG,DATA_ENABLE)));

    if ((sParams.uiBitsPerSample > 0) && (sParams.uiBitsPerSample < 32))
    {
        ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_I2SM0_CFG, 
                                        BITS_PER_SAMPLE, 
                                        sParams.uiBitsPerSample));    
    }
    else if (sParams.uiBitsPerSample == 32)
    {
        ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_I2SM0_CFG, 
                                        BITS_PER_SAMPLE, 
                                        0));    
    }
    else 
    {
        BDBG_ERR (("BRAP_OP_P_I2sStart_Multi: Incorrect Bits per Sample %d", 
                    sParams.uiBitsPerSample));
        return BERR_TRACE (BERR_NOT_SUPPORTED);
    }

/*    if ((hOp->hFmm->hRap->bOutputMute[hOp->eOutputPort] == false) && (false == pParams->bHbrEnable)) */
    if (hOp->hFmm->hRap->bOutputMute[hOp->eOutputPort] == false)
    {
        /* Enable DATA_ENABLE only if ouput port is unMuted */
        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM0_CFG, 
                                    DATA_ENABLE, Enable));
    }
    else
    {
        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM0_CFG, 
                                    DATA_ENABLE, Disable));
    }
    
    /* Write to the register */
    BRAP_Write32 (hOp->hRegister,(BCHP_AUD_FMM_OP_CTRL_I2SM0_CFG + uiOffset),
                                                                    ui32RegVal);

        
    /* Program the FCI ID of the Input Mixer */
    switch(hOp->eOutputPort)
    {
        case BRAP_OutputPort_eI2s0:
            ui32GrpBit = 0x1;
            ui32DriveBit = 0x1;
            ui32StreamId = 3;
            break;

        case BRAP_OutputPort_eI2s1:
            ui32GrpBit = 0x2;
            ui32DriveBit = 0x2;
            ui32StreamId = 4;
            break;

        case BRAP_OutputPort_eI2s2:
            ui32GrpBit = 0x4;
            ui32DriveBit = 0x4;
            ui32StreamId = 5;
            break;

        case BRAP_OutputPort_eI2s3:
            ui32GrpBit = 0x8;
            ui32DriveBit = 0x8;
            ui32StreamId = 6;
            break;
        case BRAP_OutputPort_eI2s5:
            ui32DriveBit = 0x1000;
            ui32StreamId = 8;
            break;
        case BRAP_OutputPort_eI2s6:
            ui32DriveBit = 0x2000;
            ui32StreamId = 8;
            break;
        case BRAP_OutputPort_eI2s7:
            ui32DriveBit = 0x4000;
            ui32StreamId = 8;
            break;
        case BRAP_OutputPort_eI2s8:
            ui32DriveBit = 0x8000;
            ui32StreamId = 8;
            break;
        default:
            BDBG_ERR(("BRAP_OP_P_I2sStart_Multi: Unhandled OpPort %d",
                hOp->eOutputPort));
            BDBG_ASSERT(0);
    }
    ui32RegVal = BRAP_Read32(hOp->hRegister , 
                (BCHP_AUD_FMM_IOP_CTRL_FCI_CFGi_ARRAY_BASE + (ui32StreamId*4)));

    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_IOP_CTRL_FCI_CFGi, ID));
    ui32RegVal |= BCHP_FIELD_DATA(AUD_FMM_IOP_CTRL_FCI_CFGi, ID,
                                            hOp->uOpParams.sI2s.ui32InputFciId);

    BRAP_Write32(hOp->hRegister , (BCHP_AUD_FMM_IOP_CTRL_FCI_CFGi_ARRAY_BASE + 
                                                  (ui32StreamId*4)),ui32RegVal);

    /* Read-Modify-Write AUD_FMM_MISC_SEROUT_OE */
    ui32RegVal = BRAP_Read32 (hOp->hRegister, BCHP_AUD_FMM_MISC_SEROUT_OE);
    
    if(true == bI2sMulti0)
    {
        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE,LRCKM0_OE, Drive));
        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE,SCLKM0_OE, Drive));
        ui32RegVal |= ui32DriveBit;
    }
#if(BRAP_7405_FAMILY != 1)
    else
    {
        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE,LRCKM1_OE, Drive));
        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE,SCLKM1_OE, Drive));
        ui32RegVal |= ui32DriveBit;
    }
#endif
    
    BRAP_Write32 (hOp->hRegister, BCHP_AUD_FMM_MISC_SEROUT_OE, ui32RegVal);

#if 0
    /* Program the group id */
    if((true == bI2sMulti0)&&(true == hOp->uOpParams.sI2s.bIsMulti))
    {
        ui32RegVal = BRAP_Read32(hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_I2SM0_GROUPING);
        ui32RegVal |= ui32GrpBit;
        BRAP_Write32(hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_I2SM0_GROUPING, ui32RegVal);
    }
#endif

#if(BRAP_7405_FAMILY != 1)
    /* Program the Timebase */
    ret = BRAP_OP_P_ProgramTimebase (hOp, hOp->uOpSettings.sI2s.sExtSettings.ePll,
                                     sParams.eTimebase);
#endif

    /* Enable I2S upstream request */
    ret = BRAP_OP_P_EnableStream (hOp, true);
    BDBG_LEAVE (BRAP_OP_P_I2sStart_Multi);
    return ret;
}

static BERR_Code
BRAP_OP_P_I2sStop_Multi(
    BRAP_OP_P_Handle        hOp             /*[in] Output port handle */
)
{
    BERR_Code       ret = BERR_SUCCESS;
    uint32_t        ui32RegVal = 0;
    uint32_t        ui32GrpBit = 0, ui32DriveBit =0;
    unsigned int    uiOffset = 0;
    bool            bI2sMulti0 = false;

    BDBG_ENTER (BRAP_OP_P_I2sStop_Multi);
    BDBG_ASSERT (hOp);
   
    /* Program BCHP_AUD_FMM_OP_CTRL_I2S_CFG_MULTI as following (this is to Disable I2S)
       DATA_ENABLE = Disable 
    */

    switch(hOp->eOutputPort)
    {
        case BRAP_OutputPort_eI2s0:
        case BRAP_OutputPort_eI2s1:
        case BRAP_OutputPort_eI2s2:
        case BRAP_OutputPort_eI2s3:
            uiOffset =0;
            bI2sMulti0 = true;
            break;
#if(BRAP_7405_FAMILY != 1)
        case BRAP_OutputPort_eI2s5:
        case BRAP_OutputPort_eI2s6:
        case BRAP_OutputPort_eI2s7:
        case BRAP_OutputPort_eI2s8:
            uiOffset = (BCHP_AUD_FMM_OP_CTRL_I2SM1_CFG - 
                                                BCHP_AUD_FMM_OP_CTRL_I2SM0_CFG);
            bI2sMulti0 = false;
            
            /* Disable the HBR_ENABLE bit */
            ui32RegVal = BRAP_Read32 (hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_STREAM_ROUTE);
            ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_STREAM_ROUTE,HBR_ENABLE));
			ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_STREAM_ROUTE,
                                HBR_ENABLE,
                                Disable));
            BRAP_Write32 (hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_STREAM_ROUTE, ui32RegVal);            
            break;
#endif
        default:
            BDBG_ASSERT(0);
    }

    /* Read current value of the I2S config register */
    ui32RegVal = BRAP_Read32 (hOp->hRegister,(BCHP_AUD_FMM_OP_CTRL_I2SM0_CFG + uiOffset));

    /* Disable DATA_ENABLE */
    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2SM0_CFG, DATA_ENABLE));
    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM0_CFG, DATA_ENABLE, Disable));
    
    /* Write to the register */
    BRAP_Write32 (hOp->hRegister,(BCHP_AUD_FMM_OP_CTRL_I2SM0_CFG + uiOffset),ui32RegVal);

    /* Read-Modify-Write AUD_FMM_MISC_SEROUT_OE */
    ui32RegVal = BRAP_Read32 (hOp->hRegister, BCHP_AUD_FMM_MISC_SEROUT_OE);

     switch(hOp->eOutputPort)
    {
        case BRAP_OutputPort_eI2s0:
            ui32GrpBit = 0x1;
            ui32DriveBit = 0x1;
            break;

        case BRAP_OutputPort_eI2s1:
            ui32GrpBit = 0x2;
            ui32DriveBit = 0x2;
            break;

        case BRAP_OutputPort_eI2s2:
            ui32GrpBit = 0x4;
            ui32DriveBit = 0x4;
            break;

        case BRAP_OutputPort_eI2s3:
            ui32GrpBit = 0x8;
            ui32DriveBit = 0x8;
            break;
        case BRAP_OutputPort_eI2s5:
            ui32DriveBit = 0x1000;
            break;
        case BRAP_OutputPort_eI2s6:
            ui32DriveBit = 0x2000;
            break;
        case BRAP_OutputPort_eI2s7:
            ui32DriveBit = 0x4000;
            break;
        case BRAP_OutputPort_eI2s8:
            ui32DriveBit = 0x8000;
            break;
        default:
            BDBG_ERR(("BRAP_OP_P_I2sStart_Multi: Unhandled OpPort %d",
                hOp->eOutputPort));
            BDBG_ASSERT(0);
    }

    if(true == bI2sMulti0)
    {
        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE,LRCKM0_OE, Tristate));
        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE,SCLKM0_OE, Tristate));
        ui32RegVal &= ~(ui32DriveBit);
    }
#if(BRAP_7405_FAMILY != 1)
    else
    {
        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE,LRCKM1_OE, Tristate));
        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE,SCLKM1_OE, Tristate));
        ui32RegVal &= ~(ui32DriveBit);
    }
#endif
                
    BRAP_Write32 (hOp->hRegister, BCHP_AUD_FMM_MISC_SEROUT_OE, ui32RegVal);

#if 0
    if(true == bI2sMulti0)
    {
        /* Clear the corresponding bit in OP_CTRL_I2SM_GROUPING */
        ui32RegVal = BRAP_Read32(hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_I2SM0_GROUPING);
        ui32RegVal &= ~(ui32GrpBit);
        BRAP_Write32(hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_I2SM0_GROUPING, ui32RegVal);
    }
#endif    

    /* Disable I2S upstream request */
    ret = BRAP_OP_P_EnableStream (hOp, false);

    BDBG_LEAVE (BRAP_OP_P_I2sStop_Multi);
    return ret;
}
#endif

static BERR_Code
BRAP_OP_P_I2sStart_Stereo(
    BRAP_OP_P_Handle                hOp,        /*[in] Output port handle */
    const BRAP_OP_P_I2sParams       *pParams    /*[in] Start time parameters */
)
{
    BERR_Code               ret = BERR_SUCCESS;
    uint32_t                ui32RegVal = 0;
    BRAP_OP_P_I2sParams     sParams;
    bool                    bWdRecovery = false ;
    unsigned int            ui32StreamId = 0;
    BRAP_OP_P_I2sSettings   sSettings;
    unsigned int uiTemp=0;
    unsigned int uiI2sStereoIndex=0;


    BDBG_ENTER (BRAP_OP_P_I2sStart_Stereo);
    BDBG_ASSERT (hOp);

    bWdRecovery = BRAP_FMM_P_GetWatchdogRecoveryFlag (hOp->hFmm);
    if (false == bWdRecovery)
    { 
        BDBG_ASSERT (pParams);
        hOp->uOpParams.sI2s = *pParams;
        sParams = *pParams;
    }
    else
    {
        sParams = hOp->uOpParams.sI2s; 
    }

    BRAP_OP_P_GetI2sStereoIndex(hOp->eOutputPort,&uiI2sStereoIndex);
     sSettings.sExtSettings = hOp->hFmm->hRap->sOutputSettings[hOp->eOutputPort].
                                               uOutputPortSettings.sI2sSettings;

    /* Set SCLKS_PER_1FS_DIV32 field. This field times 32 is the number 
       of sclk cycles in one Fs period. For example, program this register
       to 2 for a 64fs bit clock. */
    switch (sSettings.sExtSettings.eSClkRate)
    {
        case BRAP_OP_SClkRate_e64Fs: 
            uiTemp = 64;
            break;
        case BRAP_OP_SClkRate_e128Fs: 
            uiTemp = 128;
            break;
        default:
            BDBG_ERR (("BRAP_OP_P_I2sOpen_Stereo(): Incorrect SClk %d", 
                        sSettings.sExtSettings.eSClkRate));
            return BERR_TRACE (BERR_NOT_SUPPORTED);
    }
    
    /* Set MCLK/(2*SCLK) ratio in MCLK_RATE field */
    switch (sSettings.sExtSettings.eMClkRate)
    {
        case BRAP_OP_MClkRate_e128Fs:
           uiTemp = (128 / (2 * uiTemp));
           break; 
        case BRAP_OP_MClkRate_e256Fs:
           uiTemp = (256 / (2 * uiTemp));
           break; 
        case BRAP_OP_MClkRate_e384Fs:
           uiTemp = (384 / (2 * uiTemp));
           break; 
        case BRAP_OP_MClkRate_e512Fs:
           uiTemp = (512 / (2 * uiTemp));
           break; 
        default:
            BDBG_ERR (("BRAP_OP_P_I2sOpen_Stereo(): Incorrect MClk %d", 
                        sSettings.sExtSettings.eMClkRate));
            return BERR_TRACE (BERR_NOT_SUPPORTED);
    }
    
    switch (uiTemp)
    {
        case 1:
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0, 
                                            MCLK_RATE, MCLK_128fs_SCLK_64fs));
            break;     
        case 2:
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0, 
                                            MCLK_RATE, MCLK_256fs_SCLK_64fs));
            break;     
        case 3:
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0, 
                                            MCLK_RATE, MCLK_384fs_SCLK_64fs));
            break;     
        case 4:
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0, 
                                            MCLK_RATE, MCLK_512fs_SCLK_64fs));
            break;     
        default:
            BDBG_ERR (("BRAP_OP_P_I2sOpen_Stereo(): Unsupported MClk/SClk Ratio %d", 
                        uiTemp));
            return BERR_TRACE (BERR_NOT_SUPPORTED);
    } 

  
    /* Set the PLL */
    /* The PLL Channels are choosen in the following way.
        
       If MClock rate is 128 Fs, Channel 1 should be selected,
       If MClock rate is 256 Fs, Channel 2 should be selected,
       If MClock rate is 512 Fs, Channel 3 should be selected,
 
       Basically Channels 1,2 and 3 correspond to 128Fs, 256Fs and 512Fs respectively.
 
       (MClock 384Fs can be set by user programming only).
    */
    switch (sSettings.sExtSettings.ePll)
    {
        case BRAP_OP_Pll_ePll0:
            
        /* Set MCLK rate and PLL Channels */
        switch (sSettings.sExtSettings.eMClkRate)
        {
            case BRAP_OP_MClkRate_e128Fs:
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0, 
                                            PLLCLKSEL, PLL0_ch1));
                break;     
            case BRAP_OP_MClkRate_e256Fs:
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0, 
                                            PLLCLKSEL, PLL0_ch2));
                break;     
            case BRAP_OP_MClkRate_e512Fs:
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0, 
                                            PLLCLKSEL, PLL0_ch3));
                break;     
            default:
                BDBG_ERR (("BRAP_OP_P_I2sOpen_Stereo(): Incorrect MClk Rate %d", 
                            sSettings.sExtSettings.eMClkRate));
                return BERR_TRACE (BERR_NOT_SUPPORTED);
        }     
            
        break;
#if (BRAP_PLL1_SUPPORTED == 1)   
        case BRAP_OP_Pll_ePll1:
        
        /* Set MCLK rate and PLL Channels */
        switch (sSettings.sExtSettings.eMClkRate)
        {
            case BRAP_OP_MClkRate_e128Fs:
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0, 
                                            PLLCLKSEL, PLL1_ch1));
                break;     
            case BRAP_OP_MClkRate_e256Fs:
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0, 
                                            PLLCLKSEL, PLL1_ch2));
                break;     
            case BRAP_OP_MClkRate_e512Fs:
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0, 
                                            PLLCLKSEL, PLL1_ch3));
                break;     
            default:
                BDBG_ERR (("BRAP_OP_P_I2sOpen_Stereo(): Incorrect MClk Rate %d", 
                            sSettings.sExtSettings.eMClkRate));
                return BERR_TRACE (BERR_NOT_SUPPORTED);
        } 

        break;
#endif        
       default:
            BDBG_ERR (("BRAP_OP_P_I2sOpen_Stereo(): Incorrect PLL chosen %d", 
                        sSettings.sExtSettings.ePll));
            return BERR_TRACE (BERR_NOT_SUPPORTED);
    }

    BRAP_Write32 (hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0 + uiI2sStereoIndex*8,ui32RegVal);

    BRAP_OP_P_I2sHWConfig_Stereo (hOp, &sSettings);

    
    /* Read current value of the I2S config register */
    ui32RegVal = BRAP_Read32(hOp->hRegister,BCHP_AUD_FMM_OP_CTRL_I2SS0_CFG + uiI2sStereoIndex*16);
    
    if ((sParams.uiBitsPerSample > 0) && (sParams.uiBitsPerSample < 32))
    {
        ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_I2SS0_CFG, 
                                        BITS_PER_SAMPLE, 
                                        sParams.uiBitsPerSample));    
    }
    else if (sParams.uiBitsPerSample == 32)
    {
        ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_I2SS0_CFG, 
                                        BITS_PER_SAMPLE, 
                                        0));    
    }
    else 
    {
        BDBG_ERR (("BRAP_OP_P_I2sStart(): Incorrect Bits per Sample %d", 
                    sParams.uiBitsPerSample));
        return BERR_TRACE (BERR_NOT_SUPPORTED);
    }

    if (hOp->hFmm->hRap->bOutputMute[hOp->eOutputPort] == false)
    {

        /* Enable DATA_ENABLE only if ouput port is unMuted */
        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SS0_CFG, 
                                    DATA_ENABLE, Enable));
    }

    /* Write to the register */
    BRAP_Write32 (hOp->hRegister,BCHP_AUD_FMM_OP_CTRL_I2SS0_CFG + uiI2sStereoIndex*16,ui32RegVal);

    /* Read-Modify-Write AUD_FMM_MISC_SEROUT_OE */
    ui32RegVal = BRAP_Read32 (hOp->hRegister, BCHP_AUD_FMM_MISC_SEROUT_OE);

    if(uiI2sStereoIndex == 0)
    {
        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE,LRCKS0_OE, Drive));            
        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE,SCLKS0_OE, Drive));            
        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE,SDATS0_OE, Drive));
    }
#if (BRAP_7420_FAMILY ==1)          
    else  if(uiI2sStereoIndex == 1)
    {
        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE,LRCKS1_OE, Drive));            
        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE,SCLKS1_OE, Drive));            
        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE,SDATS1_OE, Drive));
    }
#endif    
            
    BRAP_Write32 (hOp->hRegister, BCHP_AUD_FMM_MISC_SEROUT_OE, ui32RegVal);

    /* Program the FCI ID of the Input Mixer */
    if(uiI2sStereoIndex == 0)
    {    
        ui32StreamId = 7;
    }
    else if(uiI2sStereoIndex == 1)
    {    
        ui32StreamId = 8;
    }    
    ui32RegVal = BRAP_Read32(hOp->hRegister , 
                (BCHP_AUD_FMM_IOP_CTRL_FCI_CFGi_ARRAY_BASE + (ui32StreamId*4)));

    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_IOP_CTRL_FCI_CFGi, ID));
    ui32RegVal |= BCHP_FIELD_DATA(AUD_FMM_IOP_CTRL_FCI_CFGi, ID,
                                            hOp->uOpParams.sI2s.ui32InputFciId);

    BRAP_Write32(hOp->hRegister , (BCHP_AUD_FMM_IOP_CTRL_FCI_CFGi_ARRAY_BASE + 
                                                  (ui32StreamId*4)),ui32RegVal);

#if(BRAP_7405_FAMILY != 1)
    /* Program the Timebase */
    ret = BRAP_OP_P_ProgramTimebase (hOp, hOp->uOpSettings.sI2s.sExtSettings.ePll,
                                     sParams.eTimebase);
#endif
    /* Enable I2S upstream request */
    ret = BRAP_OP_P_EnableStream (hOp, true);
    BDBG_LEAVE (BRAP_OP_P_I2sStart_Stereo);
    return ret;
}

static BERR_Code
BRAP_OP_P_I2sStop_Stereo(
    BRAP_OP_P_Handle        hOp             /*[in] Output port handle */
)
{
    BERR_Code       ret = BERR_SUCCESS;
    uint32_t        ui32RegVal = 0;

    BDBG_ENTER (BRAP_OP_P_I2sStop_Stereo);
    BDBG_ASSERT (hOp);
   
    /* Program BCHP_AUD_FMM_OP_CTRL_I2S_CFG_STEREO as following (this is to Disable I2S)
       DATA_ENABLE = Disable */
    /* Read current value of the I2S config register */
    ui32RegVal = BRAP_Read32 (hOp->hRegister,BCHP_AUD_FMM_OP_CTRL_I2SS0_CFG);
    
    /* Enable DATA_ENABLE */
    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SS0_CFG,DATA_ENABLE, Disable));

    /* Write to the register */
    BRAP_Write32 (hOp->hRegister,BCHP_AUD_FMM_OP_CTRL_I2SS0_CFG,ui32RegVal);

    /* Read-Modify-Write AUD_FMM_MISC_SEROUT_OE */
    ui32RegVal = BRAP_Read32 (hOp->hRegister, BCHP_AUD_FMM_MISC_SEROUT_OE);

    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE,LRCKS0_OE, Tristate));            
    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE,SCLKS0_OE, Tristate));            
    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE,SDATS0_OE, Tristate));
            
    BRAP_Write32 (hOp->hRegister, BCHP_AUD_FMM_MISC_SEROUT_OE, ui32RegVal);
    
    /* Disable I2S upstream request */
    ret = BRAP_OP_P_EnableStream (hOp, false);
    BDBG_LEAVE (BRAP_OP_P_I2sStop_Stereo);
    return ret;
}
#endif /* # if 7440 A0 or 3563 */
#endif /* # if 7550 */
/* Following function Configures the MAI formatter */
BERR_Code
BRAP_OP_P_MaiHWConfig (
    BRAP_OP_P_Handle                hOp,            /*[in] Rap handle */
    const BRAP_OP_P_MaiSettings     *pMaiSettings   /* [in]Pointer to MAI settings */
)
{
	uint32_t  ui32RegVal = 0;
	BDBG_ENTER (BRAP_OP_P_MaiHWConfig);
	BDBG_ASSERT (hOp);
    

	switch ( pMaiSettings->sExtSettings.eChannelOrder )
	{
		case BRAP_OP_ChannelOrder_eBothLeft:
			ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_CROSSBAR , 
                                                    OUT0, In0_l));
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_CROSSBAR , 
                                                    OUT1, In0_l));
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_CROSSBAR , 
                                                    OUT2, In1_l));
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_CROSSBAR , 
                                                    OUT3, In1_l));
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_CROSSBAR , 
                                                    OUT4, In2_l));
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_CROSSBAR , 
                                                    OUT5, In2_l));
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_CROSSBAR , 
                                                    OUT6, In3_l));
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_CROSSBAR , 
                                                    OUT7, In3_l));
			break;
		case BRAP_OP_ChannelOrder_eBothRight:
			ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_CROSSBAR , 
                                                    OUT0, In0_r));
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_CROSSBAR , 
                                                    OUT1, In0_r));
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_CROSSBAR , 
                                                    OUT2, In1_r));
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_CROSSBAR , 
                                                    OUT3, In1_r));
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_CROSSBAR , 
                                                    OUT4, In2_r));
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_CROSSBAR , 
                                                    OUT5, In2_r));
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_CROSSBAR , 
                                                    OUT6, In3_r));
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_CROSSBAR , 
                                                    OUT7, In3_r));
			break;
		case BRAP_OP_ChannelOrder_eSwap:
			ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_CROSSBAR , 
                                                    OUT0, In0_r));
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_CROSSBAR , 
                                                    OUT1, In0_l));
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_CROSSBAR , 
                                                    OUT2, In1_r));
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_CROSSBAR , 
                                                    OUT3, In1_l));
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_CROSSBAR , 
                                                    OUT4, In2_r));
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_CROSSBAR , 
                                                    OUT5, In2_l));
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_CROSSBAR , 
                                                    OUT6, In3_r));
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_CROSSBAR , 
                                                    OUT7, In3_l));
			break;
		case BRAP_OP_ChannelOrder_eNormal:				
			ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_CROSSBAR , 
                                                    OUT0, In0_l));
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_CROSSBAR , 
                                                    OUT1, In0_r));
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_CROSSBAR , 
                                                    OUT2, In1_l));
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_CROSSBAR , 
                                                    OUT3, In1_r));
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_CROSSBAR , 
                                                    OUT4, In2_l));
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_CROSSBAR , 
                                                    OUT5, In2_r));
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_CROSSBAR , 
                                                    OUT6, In3_l));
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_CROSSBAR , 
                                                    OUT7, In3_r));
			break;
		default:
			BDBG_ERR (("BRAP_OP_P_MaiHWConfig(): Unsupported Channel order %d", 
		         				pMaiSettings->sExtSettings.eChannelOrder));
			return BERR_TRACE (BERR_INVALID_PARAMETER);
	}
	/* Write to the MAI config register */
	BRAP_Write32 (hOp->hRegister,
	                        BCHP_AUD_FMM_OP_CTRL_MAI_CROSSBAR+ hOp->ui32Offset, 
			                                                        ui32RegVal);
   
	if (BRAP_FMM_P_GetWatchdogRecoveryFlag (hOp->hFmm) == false)
	{  
		/* store the settings in hOp */
		hOp->uOpSettings.sMai = *pMaiSettings;
	}
    
	return BERR_SUCCESS;
}

static BERR_Code
BRAP_OP_P_MaiOpen (
    BRAP_OP_P_Handle        hOp,              /*[in] Output port handle */
    const BRAP_OP_P_MaiSettings * pSettings /*[in] Open parameter */
)
{
	BDBG_ENTER (BRAP_OP_P_MaiOpen);
    BSTD_UNUSED (hOp);
    BSTD_UNUSED(pSettings);

      
	BDBG_LEAVE (BRAP_OP_P_MaiOpen);
    return BERR_SUCCESS;
}
#if ((BRAP_7420_FAMILY ==1)||(BRAP_7550_FAMILY ==1))

static BERR_Code
BRAP_OP_P_MaiMultiOpen (
    BRAP_OP_P_Handle        hOp,              /*[in] Output port handle */
    const BRAP_OP_P_MaiSettings * pSettings /*[in] Open parameter */
)
{
	BDBG_ENTER (BRAP_OP_P_MaiMultiOpen);
    BSTD_UNUSED (hOp);
    BSTD_UNUSED(pSettings);

      
	BDBG_LEAVE (BRAP_OP_P_MaiMultiOpen);
    return BERR_SUCCESS;
}

static BERR_Code
BRAP_OP_P_MaiMultiClose (
    BRAP_OP_P_Handle        hOp              /*[in] Output port handle */
)
{
    BDBG_ENTER (BRAP_OP_P_MaiMultiClose);
    BSTD_UNUSED (hOp);


    BDBG_LEAVE (BRAP_OP_P_MaiMultiClose);
    return BERR_SUCCESS;
}


static BERR_Code
BRAP_OP_P_MaiMultiStart (
    BRAP_OP_P_Handle        hOp,              /*[in] Output port handle */
    const BRAP_OP_P_MaiParams * pParams /*[in] Start time parameter */
)
{
    BERR_Code               ret = BERR_SUCCESS;
    uint32_t                ui32RegVal = 0;
    unsigned int           ui32StreamId = 0;
    BRAP_OP_P_MaiParams sParams;
    BRAP_OP_P_MaiSettings   sSettings;
    bool                    bWdRecovery = false;

    BDBG_ENTER (BRAP_OP_P_MaiMultiStart);
    BDBG_ASSERT (hOp);

    bWdRecovery = BRAP_FMM_P_GetWatchdogRecoveryFlag (hOp->hFmm);
    if (false == bWdRecovery)
    { 
        BDBG_ASSERT (pParams);
        hOp->uOpParams.sMaiMulti = *pParams;
        sParams = *pParams;
    }
    else
    {
        sParams = hOp->uOpParams.sMaiMulti;
    }

	sSettings.sExtSettings = hOp->hFmm->hRap->sOutputSettings[hOp->eOutputPort].
                                               uOutputPortSettings.sMaiMultiSettings;

        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_MAI_MULTI, 
                                        MCLK_RATE, MCLK_256fs_SCLK_64fs));

#if (BRAP_7550_FAMILY !=1)
    /* Set the PLL */
    /* The PLL Channels are choosen in the following way.
        
       If MClock rate is 128 Fs, Channel 1 should be selected,
       If MClock rate is 256 Fs, Channel 2 should be selected,
       If MClock rate is 512 Fs, Channel 3 should be selected,
 
       Basically Channels 1,2 and 3 correspond to 128Fs, 256Fs and 512Fs respectively.
 
       (MClock 384Fs can be set by user programming only).
    */
    switch (sSettings.sExtSettings.ePll)
    {
        case BRAP_OP_Pll_ePll0:
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_MAI_MULTI, 
                                        PLLCLKSEL, PLL0_ch2));
        break;
#if (BRAP_PLL1_SUPPORTED == 1)   
        case BRAP_OP_Pll_ePll1:
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_MAI_MULTI, 
                                            PLLCLKSEL, PLL1_ch2));
        break;     
#endif        
       default:
            BDBG_ERR (("BRAP_OP_P_I2sStart_Multi(): Incorrect PLL chosen %d", 
                        sSettings.sExtSettings.ePll));
            return BERR_TRACE (BERR_NOT_SUPPORTED);
    }
#else
    /* Set MCLK rate and PLL Channels */
    switch (sSettings.sExtSettings.eMClkRate)
    {
        case BRAP_OP_MClkRate_e128Fs:
            ui32RegVal |= ((BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_MAI_MULTI, 
                                        PLLCLKSEL, SPDIF_MCLK)) |
                           (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_MAI_MULTI, 
                                        MCLK_RATE, MCLK_128fs_SCLK_64fs)));
            break;     
        case BRAP_OP_MClkRate_e256Fs:
            ui32RegVal |= ((BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_MAI_MULTI, 
                                        PLLCLKSEL, SPDIF_MCLK)) |
                           (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_MAI_MULTI, 
                                        MCLK_RATE, MCLK_256fs_SCLK_64fs)));
            break;     
        case BRAP_OP_MClkRate_e512Fs:
            ui32RegVal |= ((BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_MAI_MULTI, 
                                        PLLCLKSEL, SPDIF_MCLK)) |
                           (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_MAI_MULTI, 
                                        MCLK_RATE, MCLK_512fs_SCLK_64fs)));
            break;     
        default:
            BDBG_ERR (("BRAP_OP_P_MaiMultiStart(): Incorrect MClk Rate %d", 
                        sSettings.sExtSettings.eMClkRate));
            return BERR_TRACE (BERR_NOT_SUPPORTED);
    }
    BDBG_MSG(("MaiMulti MClkRate = %d\n",sSettings.sExtSettings.eMClkRate));
#endif
    BRAP_Write32 (hOp->hRegister,BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_MAI_MULTI,ui32RegVal);

        
    /* Program the FCI ID of the Input Mixer */
    switch(hOp->eOutputPort)
    {
        case BRAP_OutputPort_eMaiMulti0:
            ui32StreamId = 3;
            break;

        case BRAP_OutputPort_eMaiMulti1:
            ui32StreamId = 4;
            break;

        case BRAP_OutputPort_eMaiMulti2:
            ui32StreamId = 5;
            break;

        case BRAP_OutputPort_eMaiMulti3:
            ui32StreamId = 6;
            break;
        default:
            BDBG_ERR(("BRAP_OP_P_MaiMultiStart: Unhandled OpPort %d",
                hOp->eOutputPort));
            BDBG_ASSERT(0);
    }
    ui32RegVal = BRAP_Read32(hOp->hRegister , 
                (BCHP_AUD_FMM_IOP_CTRL_FCI_CFGi_ARRAY_BASE + (ui32StreamId*4)));

    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_IOP_CTRL_FCI_CFGi, ID));
    ui32RegVal |= BCHP_FIELD_DATA(AUD_FMM_IOP_CTRL_FCI_CFGi, ID,
                                            hOp->uOpParams.sMaiMulti.ui32InputFciId);

    BRAP_Write32(hOp->hRegister , (BCHP_AUD_FMM_IOP_CTRL_FCI_CFGi_ARRAY_BASE + 
                                                  (ui32StreamId*4)),ui32RegVal);

#if 0
    ui32RegVal = BRAP_Read32(hOp->hRegister , BCHP_AUD_FMM_OP_CTRL_MAI_MULTI_GROUPING);

    switch(hOp->eOutputPort)
    {
        case BRAP_OutputPort_eMaiMulti0:
            ui32RegVal |= 0x1;
            break;

        case BRAP_OutputPort_eMaiMulti1:
            ui32RegVal |= 0x2;
            break;

        case BRAP_OutputPort_eMaiMulti2:
            ui32RegVal |= 0x4;
            break;

        case BRAP_OutputPort_eMaiMulti3:
            ui32RegVal |= 0x8;
            break;
        default:
            BDBG_ERR(("BRAP_OP_P_MaiMultiStart: Unhandled OpPort %d",
                hOp->eOutputPort));
            BDBG_ASSERT(0);
    }
    BRAP_Write32(hOp->hRegister ,BCHP_AUD_FMM_OP_CTRL_MAI_MULTI_GROUPING ,ui32RegVal);
#endif
    /* Enable I2S upstream request */
    ret = BRAP_OP_P_EnableStream (hOp, true);
    BDBG_LEAVE (BRAP_OP_P_MaiMultiStart);
    return ret;
}

static BERR_Code
BRAP_OP_P_MaiMultiStop (
    BRAP_OP_P_Handle        hOp    /*[in] Output port handle */
)
{
    BERR_Code       ret = BERR_SUCCESS;
    BDBG_ENTER (BRAP_OP_P_MaiMultiStop);
    BDBG_ASSERT (hOp);
   
    /* Disable I2S upstream request */
    ret = BRAP_OP_P_EnableStream (hOp, false);

    BDBG_LEAVE (BRAP_OP_P_MaiMultiStop);
    return ret;
}

#endif

#if (BRAP_7405_FAMILY == 1)  
static BERR_Code
BRAP_OP_P_ProgramMaiMclk (
    BRAP_OP_P_Handle        hOp,              /*[in] Output port handle */
    BRAP_OP_P_MaiSettings *psMaiSettings 
)
{
    uint32_t  ui32RegVal = 0;
    unsigned int uiOrigReVal=0;        
    BERR_Code ret=BERR_SUCCESS;

    BDBG_ASSERT(hOp);
    BDBG_ASSERT(psMaiSettings);    
    
    /* Set the PLL and MClock Rate */
    /* The PLL Channels are choosen in the following way.
       If MClock rate is 128 Fs, Channel 1 should be selected,
       If MClock rate is 256 Fs, Channel 2 should be selected,
       If MClock rate is 512 Fs, Channel 3 should be selected,
       Basically Channels 1,2 and 3 correspond to 128Fs, 256Fs and 512Fs respectively.
       (MClock 384Fs can be set by user programming only).
    */
#if (BRAP_7550_FAMILY != 1)    
    switch (psMaiSettings->sExtSettings.ePll)
    {
        case BRAP_OP_Pll_ePll0:
            if ( psMaiSettings->sExtSettings.eMaiMuxSelector == BRAP_OutputPort_eMai)
            {            
        /* Set MCLK rate and PLL Channels */
#if (BRAP_7420_FAMILY == 1)   
                ui32RegVal |= ((BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_MAI_MULTI, 
                                            PLLCLKSEL, PLL0_ch1)) |
                               (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_MAI_MULTI, 
                                            MCLK_RATE, MCLK_128fs_SCLK_64fs)));
#else
                ui32RegVal |= ((BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_HBR0, 
                                            PLLCLKSEL, PLL0_ch1)) |
                               (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_HBR0, 
                                            MCLK_RATE, MCLK_is_128fs)));
#endif
            }
#if (BRAP_7420_FAMILY != 1)               
            else if ( psMaiSettings->sExtSettings.eMaiMuxSelector == BRAP_OutputPort_eI2s0)
            {            
        /* Set MCLK rate and PLL Channels */
                ui32RegVal |= ((BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_I2S_MULTI0, 
                                            PLLCLKSEL, PLL0_ch1)) |
                               (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_I2S_MULTI0, 
                                            MCLK_RATE, MCLK_128fs_SCLK_64fs)));
            }
#else            
            else if ( psMaiSettings->sExtSettings.eMaiMuxSelector == BRAP_OutputPort_eMaiMulti0)
            {            
        /* Set MCLK rate and PLL Channels */
                ui32RegVal |= ((BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_MAI_MULTI, 
                                            PLLCLKSEL, PLL0_ch1)) |
                               (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_MAI_MULTI, 
                                            MCLK_RATE, MCLK_128fs_SCLK_64fs)));
            }
#endif            
            else
            {
                BDBG_ERR(("Incorrect MAI Mux Selector %d ",psMaiSettings->sExtSettings.eMaiMuxSelector));
                return BERR_TRACE (BERR_INVALID_PARAMETER);	
            }            
            
            
        break;
#if (BRAP_PLL1_SUPPORTED == 1)   
        case BRAP_OP_Pll_ePll1:        
            if ( psMaiSettings->sExtSettings.eMaiMuxSelector == BRAP_OutputPort_eMai)
            {            
        /* Set MCLK rate and PLL Channels */
#if (BRAP_7420_FAMILY == 1)   
                ui32RegVal |= ((BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_MAI_MULTI, 
                                            PLLCLKSEL, PLL1_ch2)) |
                               (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_MAI_MULTI, 
                                            MCLK_RATE, MCLK_256fs_SCLK_64fs)));
#else
                ui32RegVal |= ((BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_HBR0, 
                                            PLLCLKSEL, PLL1_ch2)) |
                               (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_HBR0, 
                                            MCLK_RATE, MCLK_is_256fs)));
#endif
            }
#if (BRAP_7420_FAMILY != 1)               
            else if ( psMaiSettings->sExtSettings.eMaiMuxSelector == BRAP_OutputPort_eI2s0)
            {            
        /* Set MCLK rate and PLL Channels */
                ui32RegVal |= ((BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_I2S_MULTI0, 
                                            PLLCLKSEL, PLL1_ch2)) |
                               (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_I2S_MULTI0, 
                                            MCLK_RATE, MCLK_256fs_SCLK_64fs)));
            }
#else            
            else if ( psMaiSettings->sExtSettings.eMaiMuxSelector == BRAP_OutputPort_eMaiMulti0)
            {            
        /* Set MCLK rate and PLL Channels */
                ui32RegVal |= ((BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_MAI_MULTI, 
                                            PLLCLKSEL, PLL1_ch2)) |
                               (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_MAI_MULTI, 
                                            MCLK_RATE, MCLK_256fs_SCLK_64fs)));
            }
#endif            
            else
            {
                BDBG_ERR(("Incorrect MAI Mux Selector %d ",psMaiSettings->sExtSettings.eMaiMuxSelector));
                return BERR_TRACE (BERR_INVALID_PARAMETER);	
            }            
        break;
#endif		
        default:
            BDBG_ERR (("BRAP_OP_P_SpdifStart(): Incorrect PLL chosen %d", 
                        psMaiSettings->sExtSettings.ePll));
            return BERR_TRACE (BERR_NOT_SUPPORTED);
    }

    if ( psMaiSettings->sExtSettings.eMaiMuxSelector == BRAP_OutputPort_eMai)
    {
#if (BRAP_7420_FAMILY == 1)   
        uiOrigReVal = BRAP_Read32 (hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_MAI_MULTI);
        if(uiOrigReVal != ui32RegVal)        
        BRAP_Write32 (hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_MAI_MULTI, ui32RegVal);
#else
        uiOrigReVal = BRAP_Read32 (hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_HBR0);
        if(uiOrigReVal != ui32RegVal)        
        BRAP_Write32 (hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_HBR0, ui32RegVal);
#endif        
    }
#if (BRAP_7420_FAMILY != 1)   
    else if( psMaiSettings->sExtSettings.eMaiMuxSelector == BRAP_OutputPort_eI2s0)
    {
        uiOrigReVal = BRAP_Read32 (hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_I2S_MULTI0);
        if(uiOrigReVal != ui32RegVal)        
        BRAP_Write32 (hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_I2S_MULTI0, ui32RegVal);
    }
#else    
    else if( psMaiSettings->sExtSettings.eMaiMuxSelector == BRAP_OutputPort_eMaiMulti0)
    {
        uiOrigReVal = BRAP_Read32 (hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_MAI_MULTI);
        if(uiOrigReVal != ui32RegVal)        
        BRAP_Write32 (hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_MAI_MULTI, ui32RegVal);
    }   
#endif    
    else
    {
        BDBG_ERR(("Incorrect MAI Mux Selector %d ",psMaiSettings->sExtSettings.eMaiMuxSelector));
        return BERR_TRACE (BERR_INVALID_PARAMETER);	
    }

#else /* For 7550 Family */

    /* Set MCLK rate and PLL Channels */
    switch (psMaiSettings->sExtSettings.eMClkRate)
    {
        case BRAP_OP_MClkRate_e128Fs:
            ui32RegVal |= ((BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_MAI_MULTI, 
                                        PLLCLKSEL, SPDIF_MCLK)) |
                           (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_MAI_MULTI, 
                                        MCLK_RATE, MCLK_128fs_SCLK_64fs)));
            break;     
        case BRAP_OP_MClkRate_e256Fs:
            ui32RegVal |= ((BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_MAI_MULTI, 
                                        PLLCLKSEL, SPDIF_MCLK)) |
                           (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_MAI_MULTI, 
                                        MCLK_RATE, MCLK_256fs_SCLK_64fs)));
            break;     
        case BRAP_OP_MClkRate_e512Fs:
            ui32RegVal |= ((BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_MAI_MULTI, 
                                        PLLCLKSEL, SPDIF_MCLK)) |
                           (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_MAI_MULTI, 
                                        MCLK_RATE, MCLK_512fs_SCLK_64fs)));
            break;     
        default:
            BDBG_ERR (("BRAP_OP_P_ProgramMaiMclk(): Incorrect MClk Rate %d", 
                        psMaiSettings->sExtSettings.eMClkRate));
            return BERR_TRACE (BERR_NOT_SUPPORTED);
    }
   
    uiOrigReVal = BRAP_Read32 (hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_MAI_MULTI);
    
    if(uiOrigReVal != ui32RegVal)   
    {
        BDBG_MSG(("Mai MClkRate = %d\n",psMaiSettings->sExtSettings.eMClkRate));   
        BRAP_Write32 (hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_MAI_MULTI, ui32RegVal);
    }
    
#endif

    return ret;
}
#endif

static BERR_Code
BRAP_OP_P_MaiStart (
    BRAP_OP_P_Handle        hOp,              /*[in] Output port handle */
    const BRAP_OP_P_MaiParams * pParams /*[in] Start time parameter */
)
{

/*     Algo:
    1. Program the start-time-only paramters in BCHP_AUD_FMM_OP_CTRL_MAI_CFG
       ie SPDIF_MODE, HONOR_FORMAT
    2. Program BCHP_AUD_FMM_OP_CTRL_STREAM_CFG based on eMaiMuxSelector 
    3. Program AUD_FMM_OP_CTRL_MAI_FORMAT based on eMaiAudioFormat 
    4. Program sample width in BCHP_AUD_FMM_OP_CTRL_MAI_FORMAT 
    5. Configure the on-the-fly paramters ie program register BCHP_AUD_FMM_OP_CTRL_MAI_CFG
        x with the settings saved in hRap ie
        hRap->sOutputSettings[BRAP_OutputPort_Mai].uOutputPortSettings.sMaiSettings
        by calling BRAP_OP_P_MaiHWConfig()
        only paramter here is eChannelOrder
    6. If the port was unmuted, enable data flow.
    7. Enable upstream request.
*/
	BERR_Code ret = BERR_SUCCESS;
	uint32_t  ui32RegVal = 0;
	BRAP_OP_P_MaiParams sParams;
    BRAP_OP_P_MaiSettings sMaiSettings;     

	BDBG_ENTER (BRAP_OP_P_MaiStart);
	BDBG_ASSERT (hOp);

	if (BRAP_FMM_P_GetWatchdogRecoveryFlag (hOp->hFmm) == false)
	{ 
		BDBG_ASSERT (pParams);
		hOp->uOpParams.sMai = *pParams;
		sParams = *pParams;
	}
	else
	{
		sParams = hOp->uOpParams.sMai; 
	}

    sMaiSettings.sExtSettings= 
        hOp->hFmm->hRap->sOutputSettings[BRAP_OutputPort_eMai].uOutputPortSettings.sMaiSettings;    
    
	BDBG_MSG(("BRAP_OP_P_MaiStart: settings/parameters are:"));
    BDBG_MSG(("sParams->eTimebase=%d", sParams.eTimebase));
    BDBG_MSG(("sParams->uiSampleWidth=%d", sParams.uiSampleWidth));    
    BDBG_MSG(("sExtSettings.bSpdifoutput=%d", sMaiSettings.sExtSettings.bSpdifoutput));  
    BDBG_MSG(("sExtSettings.eChannelOrder=%d", sMaiSettings.sExtSettings.eChannelOrder));  
    BDBG_MSG(("sExtSettings.eMaiAudioFormat=%d", sMaiSettings.sExtSettings.eMaiAudioFormat));  
    BDBG_MSG(("sExtSettings.eMaiMuxSelector=%d", sMaiSettings.sExtSettings.eMaiMuxSelector));         

#if (BRAP_7405_FAMILY == 1)
    BRAP_OP_P_ProgramMaiMclk(hOp,&sMaiSettings);
#endif
      
	/* Read current value of the MAI  config register */
	ui32RegVal = BRAP_Read32 (hOp->hRegister, 
						BCHP_AUD_FMM_OP_CTRL_MAI_CFG + hOp->ui32Offset);
	ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_MAI_CFG,SPDIF_MODE));

	if ( sMaiSettings.sExtSettings.bSpdifoutput == true )
	{
		ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_CFG, 
		                            SPDIF_MODE, 
		                            SPDIF_Format));
	}
	else
	{
		ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_CFG, 
		                            SPDIF_MODE, 
		                            PCM_Format));
	}
    
	BRAP_Write32 (hOp->hRegister, 
			BCHP_AUD_FMM_OP_CTRL_MAI_CFG + hOp->ui32Offset, 
			ui32RegVal);

#if ((BRAP_3548_FAMILY == 1))
	/* Read current value of the output stream config register, modify, and 
	update the register */
	ui32RegVal = BRAP_Read32 (hOp->hRegister, 
						BCHP_AUD_FMM_OP_CTRL_STREAM_CFG + hOp->ui32Offset);
	ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_STREAM_CFG,MAI_SEL));
	ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_STREAM_CFG,STRM1_TMG_SRC_SEL));
    
	if ( sMaiSettings.sExtSettings.eMaiMuxSelector == BRAP_OutputPort_eSpdif )
	{
		ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_STREAM_CFG, 
								MAI_SEL, 
								STRM0));
		ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_STREAM_CFG, 
								STRM1_TMG_SRC_SEL, 
								SPDIF));
	}
	else if((BRAP_OutputPort_eI2s0 == sMaiSettings.sExtSettings.eMaiMuxSelector) ||
            (BRAP_OutputPort_eI2s1 == sMaiSettings.sExtSettings.eMaiMuxSelector) ||
            (BRAP_OutputPort_eI2s2 == sMaiSettings.sExtSettings.eMaiMuxSelector) ||
            (BRAP_OutputPort_eI2s3 == sMaiSettings.sExtSettings.eMaiMuxSelector))
    {
		ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_STREAM_CFG, 
								MAI_SEL, 
								STRM1));
        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_STREAM_CFG, 
								STRM1_TMG_SRC_SEL, 
								I2S_Multi));
	}
    else
    {
        /* TODO for chips that can have independent HDMI output port */
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
        
	BRAP_Write32 (hOp->hRegister, 
				BCHP_AUD_FMM_OP_CTRL_STREAM_CFG + hOp->ui32Offset, 
				ui32RegVal);
#else
    /* TODO: Add support for other Mux Select for 7440 B0 */
    
    ui32RegVal = BRAP_Read32 (hOp->hRegister,
						BCHP_AUD_FMM_OP_CTRL_STREAM_ROUTE + hOp->ui32Offset);
	ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_STREAM_ROUTE,MAI_CSTAT_SEL));
	ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_STREAM_ROUTE,STRM1_TMG_SRC_SEL));
    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_STREAM_ROUTE,MAI_PAYLOAD_SEL));
#if ((BRAP_7420_FAMILY !=1)&&(BRAP_7550_FAMILY !=1))
    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_STREAM_ROUTE,HBR_ENABLE));
#endif

	if ( sMaiSettings.sExtSettings.eMaiMuxSelector == BRAP_OutputPort_eSpdif )
	{
		ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_STREAM_ROUTE,
								MAI_CSTAT_SEL,
								STRM0));
		ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_STREAM_ROUTE,
								STRM1_TMG_SRC_SEL,
								SPDIF0));
        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_STREAM_ROUTE,
								MAI_PAYLOAD_SEL,
								Stereo));
	}
	else
	{
		ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_STREAM_ROUTE,
								MAI_CSTAT_SEL,
								STRM1));
#if (BRAP_7550_FAMILY == 1)        
        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_STREAM_ROUTE,
            					STRM1_TMG_SRC_SEL,
            					MAI_Multi));
#endif        
        switch (sMaiSettings.sExtSettings.eMaiMuxSelector)
        {
            case BRAP_OutputPort_eSpdif1:
#if (BRAP_7405_FAMILY != 1)
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_STREAM_ROUTE,
								STRM1_TMG_SRC_SEL,
								SPDIF1));
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_STREAM_ROUTE,
								MAI_PAYLOAD_SEL,
								Stereo));
#endif                
                break;
#if (BRAP_7420_FAMILY !=1)&&(BRAP_7550_FAMILY !=1)
            case BRAP_OutputPort_eI2s0:
            case BRAP_OutputPort_eI2s1:
            case BRAP_OutputPort_eI2s2:
            case BRAP_OutputPort_eI2s3:
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_STREAM_ROUTE,
								STRM1_TMG_SRC_SEL,
								I2S_Multi0));
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_STREAM_ROUTE,
								MAI_PAYLOAD_SEL,
								I2S_Multi0));
#else
            case BRAP_OutputPort_eMaiMulti0:
            case BRAP_OutputPort_eMaiMulti1:
            case BRAP_OutputPort_eMaiMulti2:
            case BRAP_OutputPort_eMaiMulti3:                
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_STREAM_ROUTE,
								STRM1_TMG_SRC_SEL,
								MAI_Multi));
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_STREAM_ROUTE,
								MAI_PAYLOAD_SEL,
								MAI_Multi));
#endif
                break;
            case BRAP_OutputPort_eI2s5:
            case BRAP_OutputPort_eI2s6:
            case BRAP_OutputPort_eI2s7:
            case BRAP_OutputPort_eI2s8:
#if (BRAP_7405_FAMILY != 1)
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_STREAM_ROUTE,
								STRM1_TMG_SRC_SEL,
								I2S_Multi1));
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_STREAM_ROUTE,
								MAI_PAYLOAD_SEL,
								I2S_Multi1));
                if(pParams->bHbrEnable == true)
                {
    				ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_STREAM_ROUTE,
                                HBR_ENABLE,
                                Enable));
                }
                else
                {
    				ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_STREAM_ROUTE,
                                HBR_ENABLE,
                                Disable));
                }
#endif                
                break;
#if (BRAP_7405_FAMILY == 1) 

            case BRAP_OutputPort_eMai:
#if (BRAP_7550_FAMILY != 1)                
#if (BRAP_7420_FAMILY ==1)      
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_STREAM_ROUTE,
    								STRM1_TMG_SRC_SEL,
    								MAI_Multi));
#else
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_STREAM_ROUTE,
    								STRM1_TMG_SRC_SEL,
    								HBR0));
#endif
#endif
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_STREAM_ROUTE,
    								MAI_PAYLOAD_SEL,
    								Stereo));
                    break;
#endif
            default:
                BDBG_ASSERT(0);
        }
	}

	BRAP_Write32 (hOp->hRegister,
					BCHP_AUD_FMM_OP_CTRL_STREAM_ROUTE + hOp->ui32Offset,
				ui32RegVal);
#endif
	/* Read current value of the MAI  Formatter register */
	ui32RegVal = BRAP_Read32 (hOp->hRegister, 
						BCHP_AUD_FMM_OP_CTRL_MAI_FORMAT + hOp->ui32Offset);
	ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_MAI_FORMAT,AUDIO_FORMAT));
	
	switch ( sMaiSettings.sExtSettings.eMaiAudioFormat )
	{
		case BRAP_OP_MaiAudioFormat_eIdle:
			ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_FORMAT, 
		                            AUDIO_FORMAT, 
		                            Idle));
			break;
#if ((BRAP_3548_FAMILY == 1))
		case BRAP_OP_MaiAudioFormat_eSpdif2Channel:
			ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_FORMAT, 
		                            AUDIO_FORMAT, 
		                            SPDIF_2_Channel));
			break;
		case BRAP_OP_MaiAudioFormat_eSpdif6Channel:
			ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_FORMAT, 
		                            AUDIO_FORMAT, 
		                            SPDIF_6_Channel));				
			break;
		case BRAP_OP_MaiAudioFormat_eSpdif8Channel:
			ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_FORMAT, 
		                            AUDIO_FORMAT, 
		                            SPDIF_8_Channel));				
			break;
            
		case BRAP_OP_MaiAudioFormat_eSpdifCompressed:
			ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_FORMAT, 
		                            AUDIO_FORMAT, 
		                            SPDIF_Compressed));				
			break;
		case BRAP_OP_MaiAudioFormat_eSpdifLinearPcm:
			ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_FORMAT, 
		                            AUDIO_FORMAT, 
		                            SPDIF_LinearPCM));				
			break;
#else
		case BRAP_OP_MaiAudioFormat_eSpdif2Channel:
			ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_FORMAT, 
		                            AUDIO_FORMAT, 
		                            SPDIF_PCM_2_Channel));
			break;
		case BRAP_OP_MaiAudioFormat_eSpdif6Channel:
			ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_FORMAT, 
		                            AUDIO_FORMAT, 
		                            SPDIF_PCM_6_Channel));				
			break;
		case BRAP_OP_MaiAudioFormat_eSpdif8Channel:
			ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_FORMAT, 
		                            AUDIO_FORMAT, 
		                            SPDIF_PCM_8_Channel));				
			break;
        case BRAP_OP_MaiAudioFormat_eSpdifCompressed: /* Temporary backward compatibility hack. Needs to be removed. */    
        case BRAP_OP_MaiAudioFormat_eSpdif2ChCompressed:
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_FORMAT, 
		                            AUDIO_FORMAT, 
		                            SPDIF_Comp_2_Channel));				
			break;
        case BRAP_OP_MaiAudioFormat_eSpdif8ChCompressed:
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_FORMAT, 
		                            AUDIO_FORMAT, 
		                            SPDIF_Comp_8_Channel));				
			break;
#endif
		default:
			BDBG_ERR (("BRAP_OP_P_MaiHWConfig(): Invalid MAI audio format %d", 
		         				sMaiSettings.sExtSettings.eMaiAudioFormat));
			return BERR_TRACE (BERR_INVALID_PARAMETER);
	}

	/* Write to the MAI Formatter register */
	BRAP_Write32 (hOp->hRegister, 
			BCHP_AUD_FMM_OP_CTRL_MAI_FORMAT + hOp->ui32Offset, 
			ui32RegVal);

	/* Program the BIT per sample */
	ui32RegVal = BRAP_Read32 (hOp->hRegister, 
					BCHP_AUD_FMM_OP_CTRL_MAI_FORMAT + hOp->ui32Offset );
	ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_MAI_FORMAT,SAMPLE_WIDTH));
	if ((sParams.uiSampleWidth > 0) && (sParams.uiSampleWidth < 32))
	{
		ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_MAI_FORMAT, 
	                            SAMPLE_WIDTH, 
	                            sParams.uiSampleWidth));
	}
	else if (sParams.uiSampleWidth == 32 )
	{
		ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_MAI_FORMAT, 
	                            SAMPLE_WIDTH, 
	                            0));
	}
	else
	{
		BDBG_ERR (("BRAP_OP_P_MaiStart(): Incorrect Bits per Sample %d", 
				sParams.uiSampleWidth));
		return BERR_TRACE (BERR_NOT_SUPPORTED);

	}

	ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_MAI_FORMAT,SAMPLE_RATE));

        switch(hOp->eSamplingRate)
        {   /* Check enum values */ 
            case BAVC_AudioSamplingRate_e32k:
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_FORMAT, SAMPLE_RATE, Rate_32000Hz));
                break;
            case BAVC_AudioSamplingRate_e44_1k:
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_FORMAT, SAMPLE_RATE, Rate_44100Hz));
                break;
            case BAVC_AudioSamplingRate_e48k:
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_FORMAT, SAMPLE_RATE, Rate_48000Hz));
                break;
            case BAVC_AudioSamplingRate_e96k:
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_FORMAT, SAMPLE_RATE, Rate_96000Hz));
                break;
            default:
                BDBG_ERR (("BRAP_OP_P_MaiStart(): Incorrect Sampling Rate %d", hOp->eSamplingRate));
                return BERR_TRACE (BERR_INVALID_PARAMETER);
        }

	BRAP_Write32 (hOp->hRegister, 
					BCHP_AUD_FMM_OP_CTRL_MAI_FORMAT + hOp->ui32Offset, 
					ui32RegVal);

	ret = BRAP_OP_P_MaiHWConfig ( hOp, &sMaiSettings );
	if ( ret != BERR_SUCCESS )
	{
		BDBG_ERR (("BRAP_OP_P_MaiStart(): Failure in MAI H/W Config %d"));
		return BERR_TRACE (ret);	
	}

#if (BRAP_7405_FAMILY == 1) 
    {
        uint32_t ui32StreamId = 1;
        ui32RegVal = BRAP_Read32(hOp->hRegister , 
                    (BCHP_AUD_FMM_IOP_CTRL_FCI_CFGi_ARRAY_BASE + (ui32StreamId*4)));

        ui32RegVal &= ~(BCHP_MASK(AUD_FMM_IOP_CTRL_FCI_CFGi, ID));
        ui32RegVal |= BCHP_FIELD_DATA(AUD_FMM_IOP_CTRL_FCI_CFGi, ID,hOp->uOpParams.sMai.ui32InputFciId);

        BRAP_Write32(hOp->hRegister , (BCHP_AUD_FMM_IOP_CTRL_FCI_CFGi_ARRAY_BASE + 
                                (ui32StreamId*4)),ui32RegVal);
    }
#endif

	/* Enable the MAI  Formatter */
		ui32RegVal = BRAP_Read32 (hOp->hRegister, 
							BCHP_AUD_FMM_OP_CTRL_MAI_CFG + hOp->ui32Offset );
		ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_MAI_CFG,ENABLE_MAI));
		ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_CFG, 
							ENABLE_MAI, 
							Enable));
		BRAP_Write32 (hOp->hRegister, 
					BCHP_AUD_FMM_OP_CTRL_MAI_CFG + hOp->ui32Offset, 
					ui32RegVal);

#if 0/* (BRAP_7405_FAMILY == 1)*/
    /* Program the Timebase */
    ret = BRAP_OP_P_ProgramTimebase (hOp, sMaiSettings.sExtSettings.ePll,
                                     hOp->uOpParams.sMai.eTimebase);
    if (ret != BERR_SUCCESS)
    {
        BDBG_ERR(("BRAP_OP_P_SpdifStart(): call to BRAP_OP_P_ProgramTimebase() failed"));
        return BERR_TRACE (ret);
    }
#endif    

	/* Enable MAI upstream request */
	ret = BRAP_OP_P_EnableStream (hOp, true);

	BDBG_LEAVE (BRAP_OP_P_MaiStart);
	return ret;
}

static BERR_Code
BRAP_OP_P_MaiStop (
    BRAP_OP_P_Handle        hOp    /*[in] Output port handle */
)
{
	BERR_Code ret = BERR_SUCCESS;
#if (BRAP_7405_FAMILY != 1)    
	uint32_t  ui32RegVal = 0;
#endif
	BDBG_ENTER (BRAP_OP_P_MaiStop);
	BDBG_ASSERT (hOp);

	BDBG_MSG(("BRAP_OP_P_MaiStop: entering"));

#if (BRAP_7405_FAMILY != 1)


	ui32RegVal = BRAP_Read32 (hOp->hRegister, 
					BCHP_AUD_FMM_OP_CTRL_MAI_CFG + hOp->ui32Offset );
	ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_MAI_CFG,ENABLE_MAI));
	ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_CFG, 
						ENABLE_MAI, 
						Disable));
	BRAP_Write32 (hOp->hRegister, 
				BCHP_AUD_FMM_OP_CTRL_MAI_CFG + hOp->ui32Offset, 
				ui32RegVal);
	/*Disable the MAI upstream request */
	ret = BRAP_OP_P_EnableStream (hOp, false);
#endif

#if (BRAP_7405_FAMILY == 1)
        BSTD_UNUSED(hOp);
#endif

	BDBG_LEAVE (BRAP_OP_P_MaiStop);
	return ret;
}


BERR_Code 
BRAP_OP_P_ProgramTimebase (
    BRAP_OP_P_Handle       hOp, 
    BRAP_OP_Pll            ePll,
    BAVC_Timebase          eTimebase
)
{
    BERR_Code   ret = BERR_SUCCESS;
    BDBG_ENTER (BRAP_OP_P_ProgramTimebase);

    BKNI_EnterCriticalSection();
    ret = BRAP_OP_P_ProgramTimebase_isr(hOp, ePll, eTimebase);
    BKNI_LeaveCriticalSection();

    BDBG_LEAVE (BRAP_OP_P_ProgramTimebase);
    return ret;
}

BERR_Code 
BRAP_OP_P_ProgramTimebase_isr (
    BRAP_OP_P_Handle       hOp, 
    BRAP_OP_Pll            ePll,
    BAVC_Timebase          eTimebase
)
{
    BERR_Code   ret = BERR_SUCCESS;
#if (BRAP_7550_FAMILY != 1) 	    
    uint32_t    ui32RegValue = 0;
    uint32_t    ui32Offset=0;
#endif    
    BDBG_ENTER (BRAP_OP_P_ProgramTimebase_isr);
    BDBG_MSG (("BRAP_OP_P_ProgramTimebase_isr(): handle=0x%08x, port=%d, ePll=%d eTimebase=%d",
                hOp, hOp->eOutputPort, ePll, eTimebase));

#if (BRAP_3548_FAMILY == 1)
    ui32Offset = (int)(ePll) * (BCHP_VCXO_CTL_MISC_AC1_MACRO - BCHP_VCXO_CTL_MISC_AC0_MACRO);    

    /* Select the proper timebase in PLLX_CONTROL Register */
    ui32RegValue = BRAP_Read32_isr(hOp->hRegister, BCHP_VCXO_CTL_MISC_AC0_CONTROL + ui32Offset );
    ui32RegValue &= ~(BCHP_MASK (VCXO_CTL_MISC_AC0_CONTROL, REFERENCE_SELECT));

    switch(eTimebase)
    {   /* Check enum values */ 
        case BAVC_Timebase_e0:
            ui32RegValue |= (BCHP_FIELD_ENUM (VCXO_CTL_MISC_AC0_CONTROL, REFERENCE_SELECT, VCXO_0));    
            break;

        default:
            BDBG_ERR (("BRAP_OP_P_ProgramTimebase_isr(): Incorrect Time Base %d",
                        eTimebase));
            return BERR_TRACE (BERR_INVALID_PARAMETER);
    }

    BRAP_Write32_isr(hOp->hRegister, BCHP_VCXO_CTL_MISC_AC0_CONTROL + ui32Offset , ui32RegValue);    

#elif (BRAP_7550_FAMILY !=1)/* Other than 3548 family */
#if (BRAP_PLL1_SUPPORTED == 1)   
    ui32Offset = (int)(ePll) * (BCHP_AUD_FMM_PLL1_MACRO - BCHP_AUD_FMM_PLL0_MACRO);    
#else
    ui32Offset =0;
#endif


    /* AUD_FMM_PLL0_MACRO, MACRO_SELECT is presently prgmed by DSP */
    
    /* Select the proper timebase in PLLX_CONTROL Register */
    ui32RegValue = BRAP_Read32_isr(hOp->hRegister, BCHP_AUD_FMM_PLL0_CONTROL + ui32Offset );
    ui32RegValue &= ~(BCHP_MASK (AUD_FMM_PLL0_CONTROL, REFERENCE_SELECT));

#if (( BRAP_7405_FAMILY != 1))
    /* Check: Store Timebase in OP object */
    switch(eTimebase)
    {   /* Check enum values */ 
        case BAVC_Timebase_e0:
            ui32RegValue |= (BCHP_FIELD_ENUM (AUD_FMM_PLL0_CONTROL, REFERENCE_SELECT, Clk27a));
            break;
        case BAVC_Timebase_e1:
            ui32RegValue |= (BCHP_FIELD_ENUM (AUD_FMM_PLL0_CONTROL, REFERENCE_SELECT, Clk27b));
            break;
        case BAVC_Timebase_e2:
            ui32RegValue |= (BCHP_FIELD_ENUM (AUD_FMM_PLL0_CONTROL, REFERENCE_SELECT, Clk27c));
            break;
        case BAVC_Timebase_e3:
            ui32RegValue |= (BCHP_FIELD_ENUM (AUD_FMM_PLL0_CONTROL, REFERENCE_SELECT, Clk27d));
            break;
        default:
            BDBG_ERR (("BRAP_OP_P_ProgramTimebase_isr(): Incorrect Time Base %d",
                        eTimebase));
            return BERR_TRACE (BERR_INVALID_PARAMETER);
    }
#else
    switch(eTimebase)
    {   /* Check enum values */ 
        case BAVC_Timebase_e0:
#if (BRAP_7405_FAMILY == 1)
        case BAVC_Timebase_e1:
        case BAVC_Timebase_e2:
        case BAVC_Timebase_e3:
#endif            
            ui32RegValue |= (BCHP_FIELD_ENUM (AUD_FMM_PLL0_CONTROL, REFERENCE_SELECT, VCXO_0));
            break;
        default:
            BDBG_ERR (("BRAP_OP_P_ProgramTimebase_isr(): Incorrect Time Base %d",
                        eTimebase));
            return BERR_TRACE (BERR_INVALID_PARAMETER);
    }
#endif

    BRAP_Write32_isr(hOp->hRegister, BCHP_AUD_FMM_PLL0_CONTROL + ui32Offset , ui32RegValue);
#endif /* (BRAP_3548_FAMILY == 1) */

    BDBG_LEAVE (BRAP_OP_P_ProgramTimebase_isr);

    return ret;
}


/* Following function enables/disables the upstream request from the o/p port temporarily */
BERR_Code
BRAP_OP_P_EnableStream (
    BRAP_OP_P_Handle            hOp,       /*[in] Output port handle */
    bool                        bEnable    /*[in] TRUE: Enable Stream 
                                                  FALSE: Disable */
)
{
    BERR_Code   ret = BERR_SUCCESS;
    
    BDBG_ENTER (BRAP_OP_P_EnableStream);

    BKNI_EnterCriticalSection();
    ret = BRAP_OP_P_EnableStream_isr(hOp, bEnable);
    BKNI_LeaveCriticalSection();

    BDBG_LEAVE (BRAP_OP_P_EnableStream);
    return ret;   
}

BERR_Code
BRAP_OP_P_EnableStream_isr (
    BRAP_OP_P_Handle        hOp,       /*[in] Output port handle */
    bool                    bEnable    /*[in] TRUE: Enable Stream 
                                              FALSE: Disable */
)
{
    BERR_Code       ret = BERR_SUCCESS;
    uint32_t        ui32RegVal = 0;
    BRAP_OutputPort eMaiMuxSelect = BRAP_OutputPort_eMax;

    BDBG_ENTER (BRAP_OP_P_EnableStream_isr);
    
    if (hOp->eOutputPort == BRAP_OutputPort_eMai)
    {
        eMaiMuxSelect = 
            hOp->hFmm->hRap->sOutputSettings[BRAP_OutputPort_eMai].uOutputPortSettings.sMaiSettings.eMaiMuxSelector;
    }              
    
    /* Select AUD_FMM_OP_CTRL_ENABLE_SET register for enabling & 
              AUD_FMM_OP_CTRL_ENABLE_CLEAR register for disabling */
#if (BRAP_7420_FAMILY ==1)||(BRAP_7550_FAMILY ==1)
    if (bEnable == true)
    {
        switch (hOp->eOutputPort)
        {
            case BRAP_OutputPort_eDac0:
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_ENABLE_SET,
                                                STREAM2_ENA, 1));
                BDBG_MSG(("BRAP_OP_P_EnableStream_isr(): Enabling DAC"));                
                break;
#if (BRAP_7550_FAMILY !=1)                
#ifdef BCHP_HIFIDAC_CTRL1_REVISION 
            case BRAP_OutputPort_eDac1:
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_ENABLE_SET,
                                                STREAM9_ENA, 1));
                BDBG_MSG(("BRAP_OP_P_EnableStream_isr(): Enabling DAC1"));                
                break;
#endif                
            case BRAP_OutputPort_eI2s0:
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_ENABLE_SET, 
                                                STREAM7_ENA, 1));
                BDBG_MSG(("BRAP_OP_P_EnableStream_isr(): Enabling I2S0"));                
                break;

            case BRAP_OutputPort_eI2s1:
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_ENABLE_SET,STREAM8_ENA, 1));
                BDBG_MSG(("BRAP_OP_P_EnableStream_isr(): Enabling I2S1"));                                
                break;
#endif
            case BRAP_OutputPort_eSpdif:
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_ENABLE_SET,
                                                STREAM0_ENA, 1));
                BDBG_MSG(("BRAP_OP_P_EnableStream_isr(): Enabling SPDIF"));                
                break;

            case BRAP_OutputPort_eMai:
                if (eMaiMuxSelect == BRAP_OutputPort_eSpdif )
                {
                    ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_ENABLE_SET,
                                                               STREAM0_ENA, 1));
                }
                else if(eMaiMuxSelect == BRAP_OutputPort_eMai)
                {
                    ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_ENABLE_SET,
                                                               STREAM1_ENA, 1));
                }
                break;
            case BRAP_OutputPort_eMaiMulti0:
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_ENABLE_SET,
                                                STREAM3_ENA, 1));
                BDBG_MSG(("BRAP_OP_P_EnableStream_isr(): Enabling Mai Multi 0"));                
                break;                
            case BRAP_OutputPort_eMaiMulti1:
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_ENABLE_SET,
                                                STREAM4_ENA, 1));
                BDBG_MSG(("BRAP_OP_P_EnableStream_isr(): Enabling Mai Multi 1"));                
                break;                
            case BRAP_OutputPort_eMaiMulti2:
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_ENABLE_SET,
                                                STREAM5_ENA, 1));
                BDBG_MSG(("BRAP_OP_P_EnableStream_isr(): Enabling Mai Multi 2"));                
                break;                
            case BRAP_OutputPort_eMaiMulti3:
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_ENABLE_SET,
                                                STREAM6_ENA, 1));
                BDBG_MSG(("BRAP_OP_P_EnableStream_isr(): Enabling Mai Multi 3"));                
                break;                                
            default:
                BDBG_ERR(("BRAP_OP_P_EnableStream_isr(): Incorrect Output port type %d",
                            hOp->eOutputPort));
                return BERR_TRACE (BERR_INVALID_PARAMETER);
                break;
        }
        
        BRAP_Write32_isr (hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_ENABLE_SET, ui32RegVal);
    }
    else
    { 
        switch (hOp->eOutputPort)
        {
            case BRAP_OutputPort_eDac0:
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_ENABLE_CLEAR,
                                                               STREAM2_ENA, 1));
            break;
#if (BRAP_7550_FAMILY !=1)                           
#ifdef BCHP_HIFIDAC_CTRL1_REVISION 
            case BRAP_OutputPort_eDac1:
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_ENABLE_CLEAR,
                                                               STREAM9_ENA, 1));
            break;            
#endif            
            case BRAP_OutputPort_eI2s0:
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_ENABLE_CLEAR, 
                                                               STREAM7_ENA, 1));
                break;

            case BRAP_OutputPort_eI2s1:
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_ENABLE_CLEAR,
                                                               STREAM8_ENA, 1));
                break;
#endif                
            case BRAP_OutputPort_eSpdif:
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_ENABLE_CLEAR,
                                                               STREAM0_ENA, 1));
                break;                
            case BRAP_OutputPort_eMai:
                if (eMaiMuxSelect == BRAP_OutputPort_eSpdif )
                {
                    ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_ENABLE_CLEAR,
                                                                STREAM0_ENA, 1));
                }
                else if(eMaiMuxSelect == BRAP_OutputPort_eMai )
                {
                    ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_ENABLE_CLEAR,
                                                                STREAM1_ENA, 1));
                }
                break;				
            case BRAP_OutputPort_eMaiMulti0:
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_ENABLE_CLEAR,
                                                STREAM3_ENA, 1));    
                break;                
            case BRAP_OutputPort_eMaiMulti1:
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_ENABLE_CLEAR,
                                                STREAM4_ENA, 1));
                break;                
            case BRAP_OutputPort_eMaiMulti2:
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_ENABLE_CLEAR,
                                                STREAM5_ENA, 1));
                break;                
            case BRAP_OutputPort_eMaiMulti3:
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_ENABLE_CLEAR,
                                                STREAM6_ENA, 1));
                break;  
            default:
                BDBG_ERR(("BRAP_OP_P_EnableStream_isr(): Incorrect Output port type %d",
                            hOp->eOutputPort));
                return BERR_TRACE (BERR_INVALID_PARAMETER);
                break;
        }         
        BRAP_Write32_isr (hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_ENABLE_CLEAR, ui32RegVal);
    }


#else
    if (bEnable == true)
    {
        switch (hOp->eOutputPort)
        {
            case BRAP_OutputPort_eDac0:
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_ENABLE_SET,
                                                STREAM2_ENA, 1));
                BDBG_MSG(("BRAP_OP_P_EnableStream_isr(): Enabling DAC"));                
                break;
#if (BRAP_3548_FAMILY == 1)
            case BRAP_OutputPort_eDac1:
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_ENABLE_SET,
                                                STREAM8_ENA, 1));
                BDBG_MSG(("BRAP_OP_P_EnableStream_isr(): Enabling DAC1"));                
                break;

            case BRAP_OutputPort_eDac2:
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_ENABLE_SET,
                                                STREAM9_ENA, 1));
                BDBG_MSG(("BRAP_OP_P_EnableStream_isr(): Enabling DAC2"));                
                break;                
#endif
            case BRAP_OutputPort_eI2s0:
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_ENABLE_SET, 
                                                STREAM3_ENA, 1));
                BDBG_MSG(("BRAP_OP_P_EnableStream_isr(): Enabling I2S0"));                
                break;

            case BRAP_OutputPort_eI2s1:
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_ENABLE_SET,STREAM4_ENA, 1));
                break;

            case BRAP_OutputPort_eI2s2:
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_ENABLE_SET,STREAM5_ENA, 1));
                break;

            case BRAP_OutputPort_eI2s3:
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_ENABLE_SET,STREAM6_ENA, 1));
                break;

            case BRAP_OutputPort_eI2s4:
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_ENABLE_SET,STREAM7_ENA, 1));
                break;
#if ((BRAP_3548_FAMILY != 1)&&(BRAP_7405_FAMILY != 1))
            case BRAP_OutputPort_eI2s5:
            case BRAP_OutputPort_eI2s6:
            case BRAP_OutputPort_eI2s7:
            case BRAP_OutputPort_eI2s8:
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_ENABLE_SET,STREAM8_ENA, 1));
                break;
            case BRAP_OutputPort_eSpdif1:
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_ENABLE_SET,
                                                STREAM1_ENA, 1));
                BDBG_MSG(("BRAP_OP_P_EnableStream_isr(): Enabling SPDIF"));                
                break;
#endif
            case BRAP_OutputPort_eSpdif:
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_ENABLE_SET,
                                                STREAM0_ENA, 1));
                BDBG_MSG(("BRAP_OP_P_EnableStream_isr(): Enabling SPDIF"));                
                break;

            case BRAP_OutputPort_eMai:
                if (eMaiMuxSelect == BRAP_OutputPort_eSpdif )
                {
                    ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_ENABLE_SET,
                                                               STREAM0_ENA, 1));
                }
                else if(eMaiMuxSelect == BRAP_OutputPort_eMai)
                {
                    ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_ENABLE_SET,
                                                               STREAM1_ENA, 1));
                }
                break;
            default:
                BDBG_ERR(("BRAP_OP_P_EnableStream_isr(): Incorrect Output port type %d",
                            hOp->eOutputPort));
                return BERR_TRACE (BERR_INVALID_PARAMETER);
                break;
        }
        
        BRAP_Write32_isr (hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_ENABLE_SET, ui32RegVal);
    }
    else
    { 
        switch (hOp->eOutputPort)
        {
            case BRAP_OutputPort_eDac0:
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_ENABLE_CLEAR,
                                                               STREAM2_ENA, 1));
            break;
#if (BRAP_3548_FAMILY == 1)
            case BRAP_OutputPort_eDac1:
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_ENABLE_CLEAR,
                                                               STREAM8_ENA, 1));
            break;            

            case BRAP_OutputPort_eDac2:
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_ENABLE_CLEAR,
                                                               STREAM9_ENA, 1));
            break;
#endif
            case BRAP_OutputPort_eI2s0:
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_ENABLE_CLEAR, 
                                                               STREAM3_ENA, 1));
                break;

            case BRAP_OutputPort_eI2s1:
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_ENABLE_CLEAR,
                                                               STREAM4_ENA, 1));
                break;

            case BRAP_OutputPort_eI2s2:
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_ENABLE_CLEAR,
                                                               STREAM5_ENA, 1));
                break;

            case BRAP_OutputPort_eI2s3:
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_ENABLE_CLEAR,
                                                               STREAM6_ENA, 1));
                break;

            case BRAP_OutputPort_eI2s4:
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_ENABLE_CLEAR,
                                                               STREAM7_ENA, 1));
                break;
#if ((BRAP_3548_FAMILY != 1)&&(BRAP_7405_FAMILY != 1))
            case BRAP_OutputPort_eI2s5:
            case BRAP_OutputPort_eI2s6:
            case BRAP_OutputPort_eI2s7:
            case BRAP_OutputPort_eI2s8:
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_ENABLE_CLEAR,STREAM8_ENA, 1));
                break;
            case BRAP_OutputPort_eSpdif1:
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_ENABLE_CLEAR,
                                                               STREAM1_ENA, 1));
                break;
#endif
            case BRAP_OutputPort_eSpdif:
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_ENABLE_CLEAR,
                                                               STREAM0_ENA, 1));
                break;                
            case BRAP_OutputPort_eMai:
                if (eMaiMuxSelect == BRAP_OutputPort_eSpdif )
                {
                    ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_ENABLE_CLEAR,
                                                                STREAM0_ENA, 1));
                }
                else if(eMaiMuxSelect == BRAP_OutputPort_eMai )
                {
                    ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_OP_CTRL_ENABLE_CLEAR,
                                                                STREAM1_ENA, 1));
                }
                break;				

            default:
                BDBG_ERR(("BRAP_OP_P_EnableStream_isr(): Incorrect Output port type %d",
                            hOp->eOutputPort));
                return BERR_TRACE (BERR_INVALID_PARAMETER);
                break;
        }         
        BRAP_Write32_isr (hOp->hRegister, BCHP_AUD_FMM_OP_CTRL_ENABLE_CLEAR, ui32RegVal);
    }

#endif
    

    BDBG_MSG(("BRAP_OP_P_EnableStream_isr: output port %d bEnable=%d", hOp->eOutputPort, bEnable));

    BDBG_LEAVE (BRAP_OP_P_EnableStream_isr);
    return ret;   
}


BERR_Code BRAP_OP_P_SetMute ( 
    BRAP_Handle             hRap,       /* [in]Audio Device Handle */
    BRAP_OutputPort         eOpType,    /* [in]Output Type */
    bool                    bMute       /* [in]TRUE: Mute data at the output port
                                               FALSE: UnMute data at the output port */
)
{
	BERR_Code     eStatus;
	BDBG_ENTER (BRAP_OP_P_SetMute);

	BKNI_EnterCriticalSection();
	eStatus = BRAP_OP_P_SetMute_isr (hRap, eOpType, bMute);
	BKNI_LeaveCriticalSection();
	BDBG_LEAVE (BRAP_OP_P_SetMute);
	return eStatus;
}

BERR_Code BRAP_OP_P_GetMute ( 
    BRAP_Handle             hRap,       /* [in]Audio Device Handle */
    BRAP_OutputPort         eOpType,    /* [in]Output Type */
    bool *                  pMute       /* [out]Pointer to memory where the Mute 
                                                status is to be written
                                                TRUE: output port is Muted
                                                FALSE:  output port is not Muted */
)
{
    BERR_Code               ret = BERR_SUCCESS;
    
    BDBG_ENTER (BRAP_OP_P_GetMute);

    *pMute = hRap->bOutputMute[eOpType] ;
    BDBG_MSG(("BRAP_OP_P_GetMute: bMute=%d",hRap->bOutputMute[eOpType]));

    BDBG_LEAVE (BRAP_OP_P_GetMute);
    return ret;
}

BERR_Code BRAP_OP_P_MuteMixerOfOutput_isr ( 
    BRAP_Handle             hRap,           /* [in]Audio Device Handle */
    BRAP_OutputPort         eOpType,        /* [in]Output Type */
    bool                    bMute           /* [in]TRUE: Mute data at the output port
                                                   FALSE: UnMute data at the output port */
)
{
    BERR_Code               ret = BERR_SUCCESS;
    BRAP_MIXER_P_Handle     hMixer = NULL;
    unsigned int    uiMixerOutput = 0;      
    bool    bRampRequired = true;    
    

    ret = BRAP_P_GetMixerForOpPort(hRap,eOpType, &hMixer, &uiMixerOutput);				
    if(BERR_SUCCESS == ret)
    {
        /* Call the internal private function */
        ret = BRAP_MIXER_P_SetMute_isr(hMixer,bMute,uiMixerOutput,&bRampRequired);
        if(BERR_SUCCESS != ret)
        {
            BDBG_ERR(("BRAP_OP_P_MuteMixerOfOutput_isr: Could not Mute the Output"));
        }
    }
    else if(BERR_INVALID_PARAMETER == ret)
    {
        /* Channel not started yet. */
        ret = BERR_SUCCESS;
    }                    
    else
    {
        BDBG_ERR(("BRAP_OP_P_MuteMixerOfOutput_isr:BRAP_P_GetMixerForRBuf returned"
        "Error"));
        return BERR_TRACE(ret);
    }
    return ret;
}

BERR_Code BRAP_OP_P_SetMute_isr ( 
    BRAP_Handle             hRap,           /* [in]Audio Device Handle */
    BRAP_OutputPort         eOpType,        /* [in]Output Type */
    bool                    bMute           /* [in]TRUE: Mute data at the output port
                                                   FALSE: UnMute data at the output port */
)
{
    BERR_Code               ret = BERR_SUCCESS;
    BRAP_OP_P_Handle        hOp = NULL;
    uint32_t                ui32RegVal=0;
    uint32_t                ui32RegAddr=0;
    unsigned int            uiSpdiffmId=0; 
    unsigned int            uiSpdiffmStreamId=0; 
    bool bWdgRecovery = false;

    
    BDBG_ENTER (BRAP_OP_P_SetMute_isr);
    bWdgRecovery = BRAP_P_GetWatchdogRecoveryFlag(hRap);

  	hOp = hRap->hFmm[0]->hOp[eOpType]; /* TODO: dont hardcode FMM index */
	
    if (hOp == NULL)
    {
        BDBG_ERR (("BRAP_OP_P_SetMute_isr(): Output port type %d has not been opened ",
                        eOpType));
        return BERR_TRACE (BERR_INVALID_PARAMETER);            
    }

    /* Mute/unmute only if the output port is already running. If it's not 
    started yet, just store mute value, dont reprogram any registers */
    if ((hOp->uiStartCnt== 0)
        && (false == bWdgRecovery))
    {
        BDBG_MSG(("BRAP_OP_P_SetMute_isr(): output port %d hasnt been started yet."
            "Mute/unmute condition will take effect when the port is started.", hOp->eOutputPort));
        hOp->hFmm->hRap->bOutputMute[hOp->eOutputPort] = bMute;
        goto end_mute;
    }
    switch (hOp->eOutputPort)
    {
        case BRAP_OutputPort_eSpdif:
        case BRAP_OutputPort_eSpdif1:
            ui32RegAddr = BCHP_AUD_FMM_OP_CTRL_SPDIF_CFG_0 + hOp->ui32Offset;
            break;
        case BRAP_OutputPort_eMai:
            ui32RegAddr = BCHP_AUD_FMM_OP_CTRL_MAI_CFG + hOp->ui32Offset;
            break;
        case BRAP_OutputPort_eDac0:
        case BRAP_OutputPort_eDac1:
        case BRAP_OutputPort_eDac2:
            ui32RegAddr = BCHP_HIFIDAC_CTRL0_MUTECTRL_DACONLY + hOp->ui32Offset;     
            break;
#if ((BRAP_3548_FAMILY == 1))
        case BRAP_OutputPort_eI2s0:
        case BRAP_OutputPort_eI2s1:
        case BRAP_OutputPort_eI2s2:
        case BRAP_OutputPort_eI2s3:
            ui32RegAddr = BCHP_AUD_FMM_OP_CTRL_I2S_CFG_MULTI ;
            break;
        case BRAP_OutputPort_eI2s4:
            ui32RegAddr = BCHP_AUD_FMM_OP_CTRL_I2S_CFG_STEREO ;
            break;
    
#else
#if (BRAP_7550_FAMILY !=1)
#if (BRAP_7420_FAMILY !=1)
        case BRAP_OutputPort_eI2s0:
        case BRAP_OutputPort_eI2s1:
        case BRAP_OutputPort_eI2s2:
        case BRAP_OutputPort_eI2s3:
            ui32RegAddr = BCHP_AUD_FMM_OP_CTRL_I2SM0_CFG ;
            break;
        case BRAP_OutputPort_eI2s4:
            ui32RegAddr = BCHP_AUD_FMM_OP_CTRL_I2SS0_CFG;
            break;
#if ( BRAP_7405_FAMILY != 1)
        case BRAP_OutputPort_eI2s5:
        case BRAP_OutputPort_eI2s6:
        case BRAP_OutputPort_eI2s7:
        case BRAP_OutputPort_eI2s8:
            ui32RegAddr = BCHP_AUD_FMM_OP_CTRL_I2SM1_CFG;
            break;
#endif

#else /* For BRAP_7420_FAMILY */
        case BRAP_OutputPort_eI2s0:
            ui32RegAddr = BCHP_AUD_FMM_OP_CTRL_I2SS0_CFG;
            break;
        case BRAP_OutputPort_eI2s1:
            ui32RegAddr = BCHP_AUD_FMM_OP_CTRL_I2SS1_CFG;
            break;
#endif
#endif
#endif
        case BRAP_OutputPort_eMaiMulti0:
        case BRAP_OutputPort_eMaiMulti1:
        case BRAP_OutputPort_eMaiMulti2:
        case BRAP_OutputPort_eMaiMulti3:
            break;
        default:
            BDBG_ERR (("BRAP_OP_P_SetMute_isr(): Output port type %d not supported by this PI yet ",
                        hOp->eOutputPort));
            return BERR_TRACE (BERR_NOT_SUPPORTED);            
    }
    if(ui32RegAddr)
    {
    	ui32RegVal = BRAP_Read32_isr(hRap->hRegister, ui32RegAddr);
    }

    /* Mute/Unmute the output port by controlling the DISABLE_DATA field */
    /* Note: For output ports using the SPDIFFM, dont disable "DATA_EnABLE" 
    bit. Keep it enabled always. Instead, program the OVERWRITE_DATA bit so 
    that a null/pause burst or a dither signal (for compressed and PCM data 
    respectively) get inserted on a MUTE. This is being done for PR 26509 */

    if (bMute == false)
    {
        switch (hOp->eOutputPort)
        {
            case BRAP_OutputPort_eSpdif:
            case BRAP_OutputPort_eSpdif1:
                BRAP_RM_P_GetSpdifFmForOpPort (hRap->hRm, 
                                               hOp->eOutputPort,
                                               BRAP_OutputPort_eMax,
                                               &uiSpdiffmId, 
                                               &uiSpdiffmStreamId);     
                BRAP_SPDIFFM_P_InsertBurstOnMute (hRap->hFmm[0]->hSpdifFm[uiSpdiffmStreamId], bMute);
                break;
            case BRAP_OutputPort_eMai:
                BRAP_RM_P_GetSpdifFmForOpPort (hRap->hRm,
                                               BRAP_OutputPort_eMai,
                                               hOp->uOpSettings.sMai.sExtSettings.eMaiMuxSelector, 
                                               &uiSpdiffmId, 
                                               &uiSpdiffmStreamId);                
#if (BRAP_7405_FAMILY == 1)
                /* Temporary fix for mute control of HDMI on 7405 family, this needs to
                be revisited taking care of PR 26509 & once we can drive HDMI directly from
                FCI sinker instead of having a muxselector */
                if((BRAP_OutputPort_eI2s0 == hOp->uOpSettings.sMai.sExtSettings.eMaiMuxSelector))
                { /* PCM Data */
                    BRAP_OP_P_MuteMixerOfOutput_isr(hRap,BRAP_OutputPort_eI2s0,bMute);
                    hRap->bOutputMute[BRAP_OutputPort_eI2s0] = bMute;                    
                    BRAP_OP_P_MuteMixerOfOutput_isr(hRap,BRAP_OutputPort_eI2s1,bMute);
                    hRap->bOutputMute[BRAP_OutputPort_eI2s1] = bMute;                                        
                    BRAP_OP_P_MuteMixerOfOutput_isr(hRap,BRAP_OutputPort_eI2s2,bMute);
                    hRap->bOutputMute[BRAP_OutputPort_eI2s2] = bMute;                                        
                    BRAP_OP_P_MuteMixerOfOutput_isr(hRap,BRAP_OutputPort_eI2s3,bMute);                    
                    hRap->bOutputMute[BRAP_OutputPort_eI2s3] = bMute;                                        
#if 0                
                    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_MAI_CFG,ENABLE_MAI));
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_CFG,ENABLE_MAI, Enable));
#endif                    
                }
                else if((BRAP_OutputPort_eSpdif == hOp->uOpSettings.sMai.sExtSettings.eMaiMuxSelector)
                        ||(BRAP_OutputPort_eMai == hOp->uOpSettings.sMai.sExtSettings.eMaiMuxSelector))
                { /* Compressed Data */
                    BRAP_SPDIFFM_P_InsertBurstOnMute (hRap->hFmm[0]->hSpdifFm[uiSpdiffmStreamId], bMute);      
                }
                else if((BRAP_OutputPort_eMaiMulti0== hOp->uOpSettings.sMai.sExtSettings.eMaiMuxSelector))
                { /* PCM Data */
                    BRAP_OP_P_MuteMixerOfOutput_isr(hRap,BRAP_OutputPort_eMaiMulti0,bMute);
                    hRap->bOutputMute[BRAP_OutputPort_eMaiMulti0] = bMute;                                        
                    BRAP_OP_P_MuteMixerOfOutput_isr(hRap,BRAP_OutputPort_eMaiMulti1,bMute);
                    hRap->bOutputMute[BRAP_OutputPort_eMaiMulti1]= bMute;                                                            
                    BRAP_OP_P_MuteMixerOfOutput_isr(hRap,BRAP_OutputPort_eMaiMulti2,bMute);
                    hRap->bOutputMute[BRAP_OutputPort_eMaiMulti2]= bMute;                                                            
                    BRAP_OP_P_MuteMixerOfOutput_isr(hRap,BRAP_OutputPort_eMaiMulti3,bMute);                    
                    hRap->bOutputMute[BRAP_OutputPort_eMaiMulti3]= bMute;                                                            
#if 0                    
                    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_MAI_CFG,ENABLE_MAI));
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_CFG,ENABLE_MAI, Enable));
#endif                    
                }
                
#else
                BRAP_SPDIFFM_P_InsertBurstOnMute (hRap->hFmm[0]->hSpdifFm[uiSpdiffmStreamId], bMute);
#endif
                break;
            case BRAP_OutputPort_eDac0:
            case BRAP_OutputPort_eDac1:
            case BRAP_OutputPort_eDac2:
                ui32RegVal &= ~(BCHP_MASK(HIFIDAC_CTRL0_MUTECTRL_DACONLY,MUTEDAC));
                ui32RegVal |= (BCHP_FIELD_ENUM (HIFIDAC_CTRL0_MUTECTRL_DACONLY, MUTEDAC, Ramp_unmute ));    
                break;
#if ((BRAP_3548_FAMILY == 1))
            case BRAP_OutputPort_eI2s0:
            case BRAP_OutputPort_eI2s1:
            case BRAP_OutputPort_eI2s2:
            case BRAP_OutputPort_eI2s3:
                ui32RegVal &= ~(BCHP_MASK (AUD_FMM_OP_CTRL_I2S_CFG_MULTI, DATA_ENABLE));
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2S_CFG_MULTI, 
                                                            DATA_ENABLE, Enable));
                break;
            case BRAP_OutputPort_eI2s4:
                ui32RegVal &= ~(BCHP_MASK (AUD_FMM_OP_CTRL_I2S_CFG_STEREO, DATA_ENABLE));
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2S_CFG_STEREO, 
                                                            DATA_ENABLE, Enable));
                break;
#else

#if (BRAP_7420_FAMILY != 1)&&(BRAP_7550_FAMILY != 1)
            case BRAP_OutputPort_eI2s0:
            case BRAP_OutputPort_eI2s1:
            case BRAP_OutputPort_eI2s2:
            case BRAP_OutputPort_eI2s3:
                ui32RegVal &= ~(BCHP_MASK (AUD_FMM_OP_CTRL_I2SM0_CFG, DATA_ENABLE));
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM0_CFG, 
                                                            DATA_ENABLE, Enable));
                break;
            case BRAP_OutputPort_eI2s4:
                ui32RegVal &= ~(BCHP_MASK (AUD_FMM_OP_CTRL_I2SS0_CFG, DATA_ENABLE));
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SS0_CFG, 
                                                            DATA_ENABLE, Enable));
                break;
#if ( BRAP_7405_FAMILY != 1)
            case BRAP_OutputPort_eI2s5:
            case BRAP_OutputPort_eI2s6:
            case BRAP_OutputPort_eI2s7:
            case BRAP_OutputPort_eI2s8:
                    ui32RegVal &= ~(BCHP_MASK (AUD_FMM_OP_CTRL_I2SM1_CFG, DATA_ENABLE));
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM1_CFG, 
                                                            DATA_ENABLE, Enable));
                break;
#endif

#elif(BRAP_7550_FAMILY == 0)/* For BRAP_7420_FAMILY*/
            case BRAP_OutputPort_eI2s0:
                ui32RegVal &= ~(BCHP_MASK (AUD_FMM_OP_CTRL_I2SS0_CFG, DATA_ENABLE));
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SS0_CFG, 
                                                            DATA_ENABLE, Enable));
                break;
            case BRAP_OutputPort_eI2s1:
                ui32RegVal &= ~(BCHP_MASK (AUD_FMM_OP_CTRL_I2SS1_CFG, DATA_ENABLE));
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SS1_CFG, 
                                                            DATA_ENABLE, Enable));
                break;
#endif

#endif
            
            case BRAP_OutputPort_eMaiMulti0:
            case BRAP_OutputPort_eMaiMulti1:
            case BRAP_OutputPort_eMaiMulti2:
            case BRAP_OutputPort_eMaiMulti3:
                break;

            default:
                BDBG_ERR (("BRAP_OP_P_SetMute_isr(): Output port type %d not supported by this PI yet ",
                                        hOp->eOutputPort));
                return BERR_TRACE (BERR_NOT_SUPPORTED);                  
        }
    }
    else
    {
        switch (hOp->eOutputPort)
        {
             case BRAP_OutputPort_eSpdif:
             case BRAP_OutputPort_eSpdif1:
                BRAP_RM_P_GetSpdifFmForOpPort (hRap->hRm, 
                                               hOp->eOutputPort,
                                               BRAP_OutputPort_eMax,
                                               &uiSpdiffmId, 
                                               &uiSpdiffmStreamId);    
                BRAP_SPDIFFM_P_InsertBurstOnMute (hRap->hFmm[0]->hSpdifFm[uiSpdiffmStreamId], bMute); 
                break;
             case BRAP_OutputPort_eMai:
                BRAP_RM_P_GetSpdifFmForOpPort (hRap->hRm,
                                               BRAP_OutputPort_eMai,
                                               hOp->uOpSettings.sMai.sExtSettings.eMaiMuxSelector, 
                                               &uiSpdiffmId, 
                                               &uiSpdiffmStreamId);       
#if (BRAP_7405_FAMILY == 1)
                /* Temporary fix for mute control of HDMI on 7405 family, this needs to
                be revisited taking care of PR 26509 & once we can drive HDMI directly from
                FCI sinker instead of having a muxselector */
                if((BRAP_OutputPort_eI2s0 == hOp->uOpSettings.sMai.sExtSettings.eMaiMuxSelector))
                { /* PCM Data */
                    BRAP_OP_P_MuteMixerOfOutput_isr(hRap,BRAP_OutputPort_eI2s0,bMute);
                    hRap->bOutputMute[BRAP_OutputPort_eI2s0] = bMute;                                                            
                    BRAP_OP_P_MuteMixerOfOutput_isr(hRap,BRAP_OutputPort_eI2s1,bMute);
                    hRap->bOutputMute[BRAP_OutputPort_eI2s1]= bMute;                    
                    BRAP_OP_P_MuteMixerOfOutput_isr(hRap,BRAP_OutputPort_eI2s2,bMute);
                    hRap->bOutputMute[BRAP_OutputPort_eI2s2]= bMute;                    
                    BRAP_OP_P_MuteMixerOfOutput_isr(hRap,BRAP_OutputPort_eI2s3,bMute);                    
                    hRap->bOutputMute[BRAP_OutputPort_eI2s3]= bMute;                    
                    
#if 0                    
                    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_MAI_CFG,ENABLE_MAI));
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_CFG,ENABLE_MAI,Disable));
#endif                    
                }
                else if((BRAP_OutputPort_eSpdif == hOp->uOpSettings.sMai.sExtSettings.eMaiMuxSelector)
                        ||(BRAP_OutputPort_eMai == hOp->uOpSettings.sMai.sExtSettings.eMaiMuxSelector))
                { /* Compressed Data */
                    BRAP_SPDIFFM_P_InsertBurstOnMute (hRap->hFmm[0]->hSpdifFm[uiSpdiffmStreamId], bMute);
                }
                else if((BRAP_OutputPort_eMaiMulti0== hOp->uOpSettings.sMai.sExtSettings.eMaiMuxSelector))
                { /* PCM Data */
                    BRAP_OP_P_MuteMixerOfOutput_isr(hRap,BRAP_OutputPort_eMaiMulti0,bMute);
                    hRap->bOutputMute[BRAP_OutputPort_eMaiMulti0] = bMute;                                                            
                    BRAP_OP_P_MuteMixerOfOutput_isr(hRap,BRAP_OutputPort_eMaiMulti1,bMute);
                    hRap->bOutputMute[BRAP_OutputPort_eMaiMulti1]= bMute;                                                            
                    BRAP_OP_P_MuteMixerOfOutput_isr(hRap,BRAP_OutputPort_eMaiMulti2,bMute);
                    hRap->bOutputMute[BRAP_OutputPort_eMaiMulti2]= bMute;                                                            
                    BRAP_OP_P_MuteMixerOfOutput_isr(hRap,BRAP_OutputPort_eMaiMulti3,bMute);                    
                    hRap->bOutputMute[BRAP_OutputPort_eMaiMulti3]= bMute;                                                            
                    
#if 0                    
                    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_MAI_CFG,ENABLE_MAI));
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_CFG,ENABLE_MAI,Disable));
#endif                    
                }
                    
#else
                BRAP_SPDIFFM_P_InsertBurstOnMute (hRap->hFmm[0]->hSpdifFm[uiSpdiffmStreamId], bMute);
#endif
                break;
                
            case BRAP_OutputPort_eDac0:
            case BRAP_OutputPort_eDac1:
            case BRAP_OutputPort_eDac2:
                ui32RegVal &= ~(BCHP_MASK(HIFIDAC_CTRL0_MUTECTRL_DACONLY,MUTEDAC));
                ui32RegVal |= (BCHP_FIELD_ENUM (HIFIDAC_CTRL0_MUTECTRL_DACONLY,MUTEDAC, Ramp_mute ));    
                break;
                
#if ((BRAP_3548_FAMILY == 1))
            case BRAP_OutputPort_eI2s0:
            case BRAP_OutputPort_eI2s1:
            case BRAP_OutputPort_eI2s2:
            case BRAP_OutputPort_eI2s3:
                ui32RegVal &= ~(BCHP_MASK (AUD_FMM_OP_CTRL_I2S_CFG_MULTI, DATA_ENABLE));
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2S_CFG_MULTI, 
                                                            DATA_ENABLE, Disable));
                break;
            case BRAP_OutputPort_eI2s4:
                ui32RegVal &= ~(BCHP_MASK (AUD_FMM_OP_CTRL_I2S_CFG_STEREO, DATA_ENABLE));
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2S_CFG_STEREO, 
                                                            DATA_ENABLE, Disable));
                break;
#else
#if (BRAP_7550_FAMILY != 1)
#if (BRAP_7420_FAMILY != 1)
            case BRAP_OutputPort_eI2s0:
            case BRAP_OutputPort_eI2s1:
            case BRAP_OutputPort_eI2s2:
            case BRAP_OutputPort_eI2s3:
                ui32RegVal &= ~(BCHP_MASK (AUD_FMM_OP_CTRL_I2SM0_CFG, DATA_ENABLE));
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM0_CFG, 
                                                            DATA_ENABLE, Disable));
                break;
            case BRAP_OutputPort_eI2s4:
                ui32RegVal &= ~(BCHP_MASK (AUD_FMM_OP_CTRL_I2SS0_CFG, DATA_ENABLE));
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SS0_CFG, 
                                                            DATA_ENABLE, Disable));
                break;
#if ( BRAP_7405_FAMILY != 1)
            case BRAP_OutputPort_eI2s5:
            case BRAP_OutputPort_eI2s6:
            case BRAP_OutputPort_eI2s7:
            case BRAP_OutputPort_eI2s8:
                    ui32RegVal &= ~(BCHP_MASK (AUD_FMM_OP_CTRL_I2SM1_CFG, DATA_ENABLE));
                    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SM1_CFG, 
                                                            DATA_ENABLE, Disable));
                break;
#endif

#else /* For BRAP_7420_FAMILY */
            case BRAP_OutputPort_eI2s0:
                ui32RegVal &= ~(BCHP_MASK (AUD_FMM_OP_CTRL_I2SS0_CFG, DATA_ENABLE));
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SS0_CFG, 
                                                            DATA_ENABLE, Disable));
                break;
            case BRAP_OutputPort_eI2s1:
                ui32RegVal &= ~(BCHP_MASK (AUD_FMM_OP_CTRL_I2SS1_CFG, DATA_ENABLE));
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_I2SS1_CFG, 
                                                            DATA_ENABLE, Disable));
                break;
#endif
#endif


#endif
            case BRAP_OutputPort_eMaiMulti0:
            case BRAP_OutputPort_eMaiMulti1:
            case BRAP_OutputPort_eMaiMulti2:
            case BRAP_OutputPort_eMaiMulti3:
                break;
            default:
                BDBG_ERR (("BRAP_OP_P_SetMute_isr(): Output port type not supported %d by this PI yet",
                        hOp->eOutputPort));
                return BERR_TRACE (BERR_NOT_SUPPORTED);                  
        }
    } 
    if(ui32RegAddr)
    {
    	BRAP_Write32_isr (hRap->hRegister, ui32RegAddr, ui32RegVal);
    }

    if(false == bWdgRecovery)
    {
    BDBG_MSG(("BRAP_OP_P_SetMute_isr: output port =%d, bmute=%d", hOp->eOutputPort,bMute)); 
    hOp->hFmm->hRap->bOutputMute[hOp->eOutputPort] = bMute;
    }
end_mute:
    
    BDBG_LEAVE (BRAP_OP_P_SetMute_isr);
    return ret;
}


/* This function sets the sampling rate by just selecting MACRO_SELECT */
/* TODO: This function will cause a lock interrupt after 20 uS. 
         So please handle that interrupt */

#if (BRAP_7550_FAMILY != 1)
BERR_Code 
BRAP_OP_P_ProgramPll (
    BRAP_OP_P_Handle       	hOp, 
    BRAP_OP_Pll 			ePll,
    BAVC_AudioSamplingRate 	eSamplingRate
)
{
    BERR_Code ret = BERR_SUCCESS;
    
    BDBG_ENTER (BRAP_OP_P_ProgramPll);
    BKNI_EnterCriticalSection();
    ret = BRAP_OP_P_ProgramPll_isr(hOp, ePll, eSamplingRate);
    BKNI_LeaveCriticalSection();
    
	BDBG_LEAVE (BRAP_OP_P_ProgramPll);
    return ret;
}

BERR_Code 
BRAP_OP_P_ProgramPll_isr (
    BRAP_OP_P_Handle       	hOp, 
    BRAP_OP_Pll 			ePll,
    BAVC_AudioSamplingRate 	eSamplingRate
)
{
    BERR_Code   ret = BERR_SUCCESS;
    uint32_t    ui32RegValue = 0;
    uint32_t    ui32Offset=0;
#if (BRAP_7405_FAMILY == 1)    
    unsigned int uiOrigRegValue = 0;;
#endif
    
    BDBG_ENTER (BRAP_OP_P_ProgramPll_isr);
    BDBG_MSG (("BRAP_OP_P_ProgramPll_isr(): handle=0x%08x ePll=%d eSamplingRate=%d",
                hOp, ePll, eSamplingRate));

#if (BRAP_3548_FAMILY == 1)
    ui32Offset = (int)(ePll) * (BCHP_VCXO_CTL_MISC_AC1_MACRO - BCHP_VCXO_CTL_MISC_AC0_MACRO);  

    /* Program the MACRO_SELECT = The desired sampling rate */
    ui32RegValue = BRAP_Read32_isr (hOp->hRegister, BCHP_VCXO_CTL_MISC_AC0_MACRO + ui32Offset);
    ui32RegValue &= ~(BCHP_MASK(VCXO_CTL_MISC_AC0_MACRO, MACRO_SELECT));

	switch(eSamplingRate)
    {   /* Check enum values */ 
        case BAVC_AudioSamplingRate_e32k:
        case BAVC_AudioSamplingRate_e64k:
        case BAVC_AudioSamplingRate_e128k:
            /* All 32kHz family of sampling rates are achieved using 32kHz 
               as the base sampling rate. Using PLL channel select 1x, 2x or 3x 
               to achieve 32kHz, 64kHz or 128kHz */
            ui32RegValue |= (BCHP_FIELD_ENUM (VCXO_CTL_MISC_AC0_MACRO, MACRO_SELECT, Mult_of_32000));
            break;
        case BAVC_AudioSamplingRate_e44_1k:
        case BAVC_AudioSamplingRate_e88_2k:
        case BAVC_AudioSamplingRate_e176_4k:
            /* All 44.1kHz family of sampling rates are achieved using 44.1kHz 
               as the base sampling rate. Using PLL channel select 1x, 2x or 3x 
               to achieve 44.1kHz, 88.2kHz or 176.4kHz */
            ui32RegValue |= (BCHP_FIELD_ENUM (VCXO_CTL_MISC_AC0_MACRO, MACRO_SELECT, Mult_of_44100));
            break;
        case BAVC_AudioSamplingRate_e48k:
            ui32RegValue |= (BCHP_FIELD_ENUM (VCXO_CTL_MISC_AC0_MACRO, MACRO_SELECT, Mult_of_48000));
            break;
        case BAVC_AudioSamplingRate_e96k:
            ui32RegValue |= (BCHP_FIELD_ENUM (VCXO_CTL_MISC_AC0_MACRO, MACRO_SELECT, Mult_of_96000));
            break;
        case BAVC_AudioSamplingRate_e192k:
            ui32RegValue |= (BCHP_FIELD_ENUM (VCXO_CTL_MISC_AC0_MACRO, MACRO_SELECT, Mult_of_192000));
            break;
        default:
            BDBG_ERR (("BRAP_OP_P_ProgramPll_isr(): Incorrect Sampling Rate %d", eSamplingRate));
            return BERR_TRACE (BERR_INVALID_PARAMETER);
    }

    BRAP_Write32_isr(hOp->hRegister, BCHP_VCXO_CTL_MISC_AC0_MACRO + ui32Offset , ui32RegValue);    
#else /* if not 3548 family */

#if (BRAP_PLL1_SUPPORTED == 1)   
    ui32Offset = (int)(ePll) * (BCHP_AUD_FMM_PLL1_MACRO - BCHP_AUD_FMM_PLL0_MACRO);    
#else
    ui32Offset =0;
#endif


    /* Program the MACRO_SELECT = The desired sampling rate */
    ui32RegValue = BRAP_Read32_isr (hOp->hRegister, 
                                BCHP_AUD_FMM_PLL0_MACRO + ui32Offset);

#if (BRAP_7405_FAMILY == 1)    
    uiOrigRegValue = ui32RegValue;     
#endif    

    ui32RegValue &= ~(BCHP_MASK(AUD_FMM_PLL0_MACRO, MACRO_SELECT));
	switch(eSamplingRate)
    {   /* Check enum values */ 
        case BAVC_AudioSamplingRate_e32k:
        case BAVC_AudioSamplingRate_e64k:
        case BAVC_AudioSamplingRate_e128k:
            /* All 32kHz family of sampling rates are achieved using 32kHz 
               as the base sampling rate. Using PLL channel select 1x, 2x or 3x 
               to achieve 32kHz, 64kHz or 128kHz */
            ui32RegValue |= (BCHP_FIELD_ENUM (AUD_FMM_PLL0_MACRO, MACRO_SELECT, Mult_of_32000));
            break;
        case BAVC_AudioSamplingRate_e44_1k:
        case BAVC_AudioSamplingRate_e88_2k:
        case BAVC_AudioSamplingRate_e176_4k:
            /* All 44.1kHz family of sampling rates are achieved using 44.1kHz 
               as the base sampling rate. Using PLL channel select 1x, 2x or 3x 
               to achieve 44.1kHz, 88.2kHz or 176.4kHz */
            ui32RegValue |= (BCHP_FIELD_ENUM (AUD_FMM_PLL0_MACRO, MACRO_SELECT, Mult_of_44100));
            break;
        case BAVC_AudioSamplingRate_e48k:
            ui32RegValue |= (BCHP_FIELD_ENUM (AUD_FMM_PLL0_MACRO, MACRO_SELECT, Mult_of_48000));
            break;
        case BAVC_AudioSamplingRate_e96k:
            ui32RegValue |= (BCHP_FIELD_ENUM (AUD_FMM_PLL0_MACRO, MACRO_SELECT, Mult_of_96000));
            break;
        case BAVC_AudioSamplingRate_e192k:
            ui32RegValue |= (BCHP_FIELD_ENUM (AUD_FMM_PLL0_MACRO, MACRO_SELECT, Mult_of_192000));
            break;
        default:
            BDBG_ERR (("BRAP_OP_P_ProgramPll_isr(): Incorrect Sampling Rate %d", eSamplingRate));
            return BERR_TRACE (BERR_INVALID_PARAMETER);
    }

#if (BRAP_7405_FAMILY == 1)    
    if(uiOrigRegValue != ui32RegValue)
#endif
    {
        BDBG_MSG(("PROGRAMMING PLL"));    
    BRAP_Write32_isr(hOp->hRegister, BCHP_AUD_FMM_PLL0_MACRO + ui32Offset , ui32RegValue);
    }
               
#endif

	BDBG_LEAVE (BRAP_OP_P_ProgramPll_isr);
    return ret;
}
#else /* For 7550 Family only */
BERR_Code 
BRAP_OP_P_ProgramNCO (
    BRAP_OP_P_Handle       	hOp, 
    BRAP_OP_MClkRate 			eMClkRate,
    BAVC_AudioSamplingRate 	eSamplingRate
)
{
    BERR_Code ret = BERR_SUCCESS;
    
    BDBG_ENTER (BRAP_OP_P_ProgramNCO);
    BKNI_EnterCriticalSection();
    ret = BRAP_OP_P_ProgramNCO_isr(hOp, eMClkRate, eSamplingRate);
    BKNI_LeaveCriticalSection();
    
	BDBG_LEAVE (BRAP_OP_P_ProgramNCO);
    return ret;
}

BERR_Code 
BRAP_OP_P_ProgramNCO_isr (
    BRAP_OP_P_Handle       	hOp, 
    BRAP_OP_MClkRate 			eMClkRate,
    BAVC_AudioSamplingRate 	eSamplingRate
)
{

    BERR_Code   ret = BERR_SUCCESS;
    uint32_t    ui32RegValue = 0;

    BDBG_ENTER (BRAP_OP_P_ProgramNCO_isr);

    BDBG_MSG(("BRAP_OP_P_ProgramNCO_isr: eSamplingRate = %d eMClkRate = %d\n",eSamplingRate,eMClkRate));

    switch(eMClkRate)
    {
    	case BRAP_OP_MClkRate_e128Fs:
    		switch(eSamplingRate)
    		{
    			case BAVC_AudioSamplingRate_e32k:
    			    ui32RegValue = BRAP_Read32 (hOp->hRegister, BCHP_AUD_FMM_OP_SPDIF_NCO_RATE_RATIO);
                    ui32RegValue &= ~((BCHP_MASK (AUD_FMM_OP_SPDIF_NCO_RATE_RATIO, DENOMINATOR)));                    
    			    ui32RegValue |= (BCHP_FIELD_DATA (AUD_FMM_OP_SPDIF_NCO_RATE_RATIO, 
    			                                     DENOMINATOR,
    			                                      0x200));
    			    BRAP_Write32 (hOp->hRegister, BCHP_AUD_FMM_OP_SPDIF_NCO_RATE_RATIO,ui32RegValue); 

    			    ui32RegValue = BRAP_Read32 (hOp->hRegister, BCHP_AUD_FMM_OP_SPDIF_NCO_SAMPLE_INC);
                    ui32RegValue &= ~((BCHP_MASK (AUD_FMM_OP_SPDIF_NCO_SAMPLE_INC, NUMERATOR))
                                    | (BCHP_MASK (AUD_FMM_OP_SPDIF_NCO_SAMPLE_INC, SAMPLE_INC)));                    
    			    ui32RegValue |= (BCHP_FIELD_DATA ( AUD_FMM_OP_SPDIF_NCO_SAMPLE_INC, 
    			                                     NUMERATOR,
    			                                      0x12f));
    			    ui32RegValue |= (BCHP_FIELD_DATA ( AUD_FMM_OP_SPDIF_NCO_SAMPLE_INC, 
    			                                     SAMPLE_INC,
    			                                      0x6));					
    			    BRAP_Write32 (hOp->hRegister, BCHP_AUD_FMM_OP_SPDIF_NCO_SAMPLE_INC,ui32RegValue); 					

    			    ui32RegValue = BRAP_Read32 (hOp->hRegister, BCHP_AUD_FMM_OP_SPDIF_NCO_PHASE_INC);
                    ui32RegValue &= ~((BCHP_MASK (AUD_FMM_OP_SPDIF_NCO_PHASE_INC, PHASE_INC)));                    
    			    ui32RegValue |= (BCHP_FIELD_DATA ( AUD_FMM_OP_SPDIF_NCO_PHASE_INC, 
    			                                     PHASE_INC,
    			                                      0x04dac1));
    			    BRAP_Write32 (hOp->hRegister, BCHP_AUD_FMM_OP_SPDIF_NCO_PHASE_INC,ui32RegValue); 
    			
    				break;
    			case BAVC_AudioSamplingRate_e44_1k:
    			    ui32RegValue = BRAP_Read32 (hOp->hRegister, BCHP_AUD_FMM_OP_SPDIF_NCO_RATE_RATIO);
                    ui32RegValue &= ~((BCHP_MASK (AUD_FMM_OP_SPDIF_NCO_RATE_RATIO, DENOMINATOR)));                    
    			    ui32RegValue |= (BCHP_FIELD_DATA ( AUD_FMM_OP_SPDIF_NCO_RATE_RATIO, 
    			                                     DENOMINATOR,
    			                                      0x188));
    			    BRAP_Write32 (hOp->hRegister, BCHP_AUD_FMM_OP_SPDIF_NCO_RATE_RATIO,ui32RegValue); 

    			    ui32RegValue = BRAP_Read32 (hOp->hRegister, BCHP_AUD_FMM_OP_SPDIF_NCO_SAMPLE_INC);
                    ui32RegValue &= ~((BCHP_MASK (AUD_FMM_OP_SPDIF_NCO_SAMPLE_INC, NUMERATOR))
                                    | (BCHP_MASK (AUD_FMM_OP_SPDIF_NCO_SAMPLE_INC, SAMPLE_INC)));                    
    			    ui32RegValue |= (BCHP_FIELD_DATA ( AUD_FMM_OP_SPDIF_NCO_SAMPLE_INC, 
    			                                     NUMERATOR,
    			                                      0x133));
    			    ui32RegValue |= (BCHP_FIELD_DATA ( AUD_FMM_OP_SPDIF_NCO_SAMPLE_INC, 
    			                                     SAMPLE_INC,
    			                                      0x4));					
    			    BRAP_Write32 (hOp->hRegister, BCHP_AUD_FMM_OP_SPDIF_NCO_SAMPLE_INC,ui32RegValue); 					

    			    ui32RegValue = BRAP_Read32 (hOp->hRegister, BCHP_AUD_FMM_OP_SPDIF_NCO_PHASE_INC);
                    ui32RegValue &= ~((BCHP_MASK (AUD_FMM_OP_SPDIF_NCO_PHASE_INC, PHASE_INC)));                    
    			    ui32RegValue |= (BCHP_FIELD_DATA ( AUD_FMM_OP_SPDIF_NCO_PHASE_INC, 
    			                                     PHASE_INC,
    			                                      0x06b0ac));
    			    BRAP_Write32 (hOp->hRegister, BCHP_AUD_FMM_OP_SPDIF_NCO_PHASE_INC,ui32RegValue); 
    				break;
    			case BAVC_AudioSamplingRate_e48k:
    			    ui32RegValue = BRAP_Read32 (hOp->hRegister, BCHP_AUD_FMM_OP_SPDIF_NCO_RATE_RATIO);
                    ui32RegValue &= ~((BCHP_MASK (AUD_FMM_OP_SPDIF_NCO_RATE_RATIO, DENOMINATOR)));                    
    			    ui32RegValue |= (BCHP_FIELD_DATA ( AUD_FMM_OP_SPDIF_NCO_RATE_RATIO, 
    			                                     DENOMINATOR,
    			                                      0x100));
    			    BRAP_Write32 (hOp->hRegister, BCHP_AUD_FMM_OP_SPDIF_NCO_RATE_RATIO,ui32RegValue); 

    			    ui32RegValue = BRAP_Read32 (hOp->hRegister, BCHP_AUD_FMM_OP_SPDIF_NCO_SAMPLE_INC);
                    ui32RegValue &= ~((BCHP_MASK (AUD_FMM_OP_SPDIF_NCO_SAMPLE_INC, NUMERATOR))
                                    | (BCHP_MASK (AUD_FMM_OP_SPDIF_NCO_SAMPLE_INC, SAMPLE_INC)));                    
    			    ui32RegValue |= (BCHP_FIELD_DATA ( AUD_FMM_OP_SPDIF_NCO_SAMPLE_INC, 
    			                                     NUMERATOR,
    			                                      0x65));
    			    ui32RegValue |= (BCHP_FIELD_DATA ( AUD_FMM_OP_SPDIF_NCO_SAMPLE_INC, 
    			                                     SAMPLE_INC,
    			                                      0x4));					
    			    BRAP_Write32 (hOp->hRegister, BCHP_AUD_FMM_OP_SPDIF_NCO_SAMPLE_INC,ui32RegValue); 					

    			    ui32RegValue = BRAP_Read32 (hOp->hRegister, BCHP_AUD_FMM_OP_SPDIF_NCO_PHASE_INC);
                    ui32RegValue &= ~((BCHP_MASK (AUD_FMM_OP_SPDIF_NCO_PHASE_INC, PHASE_INC)));                    
    			    ui32RegValue |= (BCHP_FIELD_DATA ( AUD_FMM_OP_SPDIF_NCO_PHASE_INC, 
    			                                     PHASE_INC,
    			                                      0x074822));
    			    BRAP_Write32 (hOp->hRegister, BCHP_AUD_FMM_OP_SPDIF_NCO_PHASE_INC,ui32RegValue); 
    				break;
    			default:
    				BDBG_ERR(("Unknow Sampling rate"));
    				return BERR_TRACE (BERR_INVALID_PARAMETER);			
    				break;												
    		}
    		break;
    	case BRAP_OP_MClkRate_e256Fs:
    		switch(eSamplingRate)
    		{
    			case BAVC_AudioSamplingRate_e32k:
    			    ui32RegValue = BRAP_Read32 (hOp->hRegister, BCHP_AUD_FMM_OP_SPDIF_NCO_RATE_RATIO);
                    ui32RegValue &= ~((BCHP_MASK (AUD_FMM_OP_SPDIF_NCO_RATE_RATIO, DENOMINATOR)));
    			    ui32RegValue |= (BCHP_FIELD_DATA ( AUD_FMM_OP_SPDIF_NCO_RATE_RATIO, 
    			                                     DENOMINATOR,
    			                                      0x400));
    			    BRAP_Write32 (hOp->hRegister, BCHP_AUD_FMM_OP_SPDIF_NCO_RATE_RATIO,ui32RegValue); 

    			    ui32RegValue = BRAP_Read32 (hOp->hRegister, BCHP_AUD_FMM_OP_SPDIF_NCO_SAMPLE_INC);
                    ui32RegValue &= ~((BCHP_MASK (AUD_FMM_OP_SPDIF_NCO_SAMPLE_INC, NUMERATOR))
                                    | (BCHP_MASK (AUD_FMM_OP_SPDIF_NCO_SAMPLE_INC, SAMPLE_INC)));
    			    ui32RegValue |= (BCHP_FIELD_DATA ( AUD_FMM_OP_SPDIF_NCO_SAMPLE_INC, 
    			                                     NUMERATOR,
    			                                      0x12f));
    			    ui32RegValue |= (BCHP_FIELD_DATA ( AUD_FMM_OP_SPDIF_NCO_SAMPLE_INC, 
    			                                     SAMPLE_INC,
    			                                      0x3));					
    			    BRAP_Write32 (hOp->hRegister, BCHP_AUD_FMM_OP_SPDIF_NCO_SAMPLE_INC,ui32RegValue); 					

    			    ui32RegValue = BRAP_Read32 (hOp->hRegister, BCHP_AUD_FMM_OP_SPDIF_NCO_PHASE_INC);
                    ui32RegValue &= ~((BCHP_MASK (AUD_FMM_OP_SPDIF_NCO_PHASE_INC, PHASE_INC)));
    			    ui32RegValue |= (BCHP_FIELD_DATA ( AUD_FMM_OP_SPDIF_NCO_PHASE_INC, 
    			                                     PHASE_INC,
    			                                      0x09b583));
    			    BRAP_Write32 (hOp->hRegister, BCHP_AUD_FMM_OP_SPDIF_NCO_PHASE_INC,ui32RegValue); 
    			
    				break;
    			case BAVC_AudioSamplingRate_e44_1k:
    			    ui32RegValue = BRAP_Read32 (hOp->hRegister, BCHP_AUD_FMM_OP_SPDIF_NCO_RATE_RATIO);
                    ui32RegValue &= ~((BCHP_MASK (AUD_FMM_OP_SPDIF_NCO_RATE_RATIO, DENOMINATOR)));                    
    			    ui32RegValue |= (BCHP_FIELD_DATA ( AUD_FMM_OP_SPDIF_NCO_RATE_RATIO, 
    			                                     DENOMINATOR,
    			                                      0x310));
    			    BRAP_Write32 (hOp->hRegister, BCHP_AUD_FMM_OP_SPDIF_NCO_RATE_RATIO,ui32RegValue); 

    			    ui32RegValue = BRAP_Read32 (hOp->hRegister, BCHP_AUD_FMM_OP_SPDIF_NCO_SAMPLE_INC);
                    ui32RegValue &= ~((BCHP_MASK (AUD_FMM_OP_SPDIF_NCO_SAMPLE_INC, NUMERATOR))
                                    | (BCHP_MASK (AUD_FMM_OP_SPDIF_NCO_SAMPLE_INC, SAMPLE_INC)));
    			    ui32RegValue |= (BCHP_FIELD_DATA ( AUD_FMM_OP_SPDIF_NCO_SAMPLE_INC, 
    			                                     NUMERATOR,
    			                                      0x133));                    
    			    ui32RegValue |= (BCHP_FIELD_DATA ( AUD_FMM_OP_SPDIF_NCO_SAMPLE_INC, 
    			                                     SAMPLE_INC,
    			                                      0x2));	                   
    			    BRAP_Write32 (hOp->hRegister, BCHP_AUD_FMM_OP_SPDIF_NCO_SAMPLE_INC,ui32RegValue); 					

    			    ui32RegValue = BRAP_Read32 (hOp->hRegister, BCHP_AUD_FMM_OP_SPDIF_NCO_PHASE_INC);
                    ui32RegValue &= ~((BCHP_MASK (AUD_FMM_OP_SPDIF_NCO_PHASE_INC, PHASE_INC)));
    			    ui32RegValue |= (BCHP_FIELD_DATA ( AUD_FMM_OP_SPDIF_NCO_PHASE_INC, 
    			                                     PHASE_INC,
    			                                      0x0d6159));                
    			    BRAP_Write32 (hOp->hRegister, BCHP_AUD_FMM_OP_SPDIF_NCO_PHASE_INC,ui32RegValue);     			
    				break;
    			case BAVC_AudioSamplingRate_e48k:
    			    ui32RegValue = BRAP_Read32 (hOp->hRegister, BCHP_AUD_FMM_OP_SPDIF_NCO_RATE_RATIO);
                    ui32RegValue &= ~((BCHP_MASK (AUD_FMM_OP_SPDIF_NCO_RATE_RATIO, DENOMINATOR)));                    
    			    ui32RegValue |= (BCHP_FIELD_DATA ( AUD_FMM_OP_SPDIF_NCO_RATE_RATIO, 
    			                                     DENOMINATOR,
    			                                      0x200));
    			    BRAP_Write32 (hOp->hRegister, BCHP_AUD_FMM_OP_SPDIF_NCO_RATE_RATIO,ui32RegValue); 

    			    ui32RegValue = BRAP_Read32 (hOp->hRegister, BCHP_AUD_FMM_OP_SPDIF_NCO_SAMPLE_INC);
                    ui32RegValue &= ~((BCHP_MASK (AUD_FMM_OP_SPDIF_NCO_SAMPLE_INC, NUMERATOR))
                                    | (BCHP_MASK (AUD_FMM_OP_SPDIF_NCO_SAMPLE_INC, SAMPLE_INC)));                    
    			    ui32RegValue |= (BCHP_FIELD_DATA ( AUD_FMM_OP_SPDIF_NCO_SAMPLE_INC, 
    			                                     NUMERATOR,
    			                                      0x65));
    			    ui32RegValue |= (BCHP_FIELD_DATA ( AUD_FMM_OP_SPDIF_NCO_SAMPLE_INC, 
    			                                     SAMPLE_INC,
    			                                      0x2));					
    			    BRAP_Write32 (hOp->hRegister, BCHP_AUD_FMM_OP_SPDIF_NCO_SAMPLE_INC,ui32RegValue); 					

    			    ui32RegValue = BRAP_Read32 (hOp->hRegister, BCHP_AUD_FMM_OP_SPDIF_NCO_PHASE_INC);
                    ui32RegValue &= ~((BCHP_MASK (AUD_FMM_OP_SPDIF_NCO_PHASE_INC, PHASE_INC)));                    
    			    ui32RegValue |= (BCHP_FIELD_DATA ( AUD_FMM_OP_SPDIF_NCO_PHASE_INC, 
    			                                     PHASE_INC,
    			                                      0x0e9045));
    			    BRAP_Write32 (hOp->hRegister, BCHP_AUD_FMM_OP_SPDIF_NCO_PHASE_INC,ui32RegValue); 				
    				break;
    			default:
    				BDBG_ERR(("Unknow Sampling rate"));
    				return BERR_TRACE (BERR_INVALID_PARAMETER);			
    				break;												
    		}		
    		break;
    	default:
    		BDBG_ERR(("Unknow MClkRate = %d",eMClkRate));
    		return BERR_TRACE (BERR_INVALID_PARAMETER);			
    		break;									
    }

    ui32RegValue = BRAP_Read32 (hOp->hRegister, BCHP_AUD_FMM_OP_SPDIF_NCO_RATE_RATIO);
    BDBG_MSG(("BCHP_AUD_FMM_OP_SPDIF_NCO_RATE_RATIO = 0x%x\n",ui32RegValue));
    ui32RegValue = BRAP_Read32 (hOp->hRegister, BCHP_AUD_FMM_OP_SPDIF_NCO_SAMPLE_INC);
    BDBG_MSG(("BCHP_AUD_FMM_OP_SPDIF_NCO_SAMPLE_INC= 0x%x\n",ui32RegValue));
    ui32RegValue = BRAP_Read32 (hOp->hRegister, BCHP_AUD_FMM_OP_SPDIF_NCO_PHASE_INC);
    BDBG_MSG(("BCHP_AUD_FMM_OP_SPDIF_NCO_PHASE_INC= 0x%x\n",ui32RegValue));

    BDBG_LEAVE (BRAP_OP_P_ProgramNCO_isr);
    return ret;
}
#endif

#if 0

/***************************************************************************
Summary:
	Gets the sampling rate and time base associated with the output port.
Description:
	This API is used to get the sampling rate and time base associated with 
	a particular output port.
Returns:
	BERR_SUCCESS 
See Also:
	BRAP_OP_SetSamplingRate
****************************************************************************/
BERR_Code BRAP_OP_GetSamplingRate ( 
	const BRAP_Handle 		hRap, 			/* [in] The RAP Channel handle */
	const BRAP_OutputPort	eOutputPort, 	/* [in] Output port */
	BAVC_AudioSamplingRate  *pSamplingRate, /* [out] Output Sampling rate */
	BAVC_Timebase			*pTimebase 		/* [out] Timebase to use for the output port */
	)
{
	BERR_Code ret = BERR_SUCCESS;
	BRAP_OP_P_Handle       	hOp = NULL;
	
	BDBG_ENTER(BRAP_OP_GetSamplingRate);	

	/* 1. Validate input parameters. */
	BDBG_ASSERT(hRap);
	BDBG_ASSERT(pSamplingRate);
	BDBG_ASSERT(pTimebase);

	BDBG_MSG(("BRAP_OP_GetSamplingRate: hRap = %p"
		"\n\t eOutputPort = %d, pSamplingRate = %p, pTimebase = %p",
		hRap, eOutputPort, pSamplingRate, pTimebase));
	
	if(BRAP_RM_P_IsOutputPortSupported(eOutputPort) != BERR_SUCCESS)
	{
		BDBG_ERR(("BRAP_OP_GetSamplingRate: eOutputPort(%d) is not supported", eOutputPort));
		return BERR_TRACE(BERR_NOT_SUPPORTED);
	}

	/* Find the associated hOp */
       hOp = hRap->hFmm[0]->hOp[eOutputPort]; /* TODO: dont hardcode FMM index */

	if(hOp == NULL)
	{
		BDBG_ERR(("BRAP_OP_GetSamplingRate: hOp[%d] is NULL", eOutputPort));
		return BERR_TRACE(BERR_NOT_SUPPORTED);
	}

	*pSamplingRate = hOp->eSamplingRate;
	switch(hOp->eOutputPort)
	{
		case BRAP_OutputPort_eSpdif:
			*pTimebase = hOp->uOpParams.sSpdif.eTimebase;
			break;
				
		case BRAP_OutputPort_eDac0:
        case BRAP_OutputPort_eDac1:
        case BRAP_OutputPort_eDac2:
			*pTimebase = hOp->uOpParams.sDac.eTimebase;
			break;
			
		case BRAP_OutputPort_eI2s0:
		case BRAP_OutputPort_eI2s1:
		case BRAP_OutputPort_eI2s2:
		case BRAP_OutputPort_eI2s3:
		case BRAP_OutputPort_eI2s4:
			*pTimebase = hOp->uOpParams.sI2s.eTimebase;
			break;

		case BRAP_OutputPort_eMai:
		    if (BRAP_OutputPort_eMai == BRAP_OutputPort_eSpdif)
				*pTimebase = hOp->hFmm->hOp[BRAP_OutputPort_eSpdif]->uOpParams.sSpdif.eTimebase;
		    else if (BRAP_OutputPort_eMai == BRAP_OutputPort_eFlex)
				*pTimebase = hOp->hFmm->hOp[BRAP_OutputPort_eFlex]->uOpParams.sFlex.eTimebase;	
        	    break;			
            case BRAP_OutputPort_eMaiMulti0:
            case BRAP_OutputPort_eMaiMulti1:
            case BRAP_OutputPort_eMaiMulti2:
            case BRAP_OutputPort_eMaiMulti3:
                break;
		default:	
			BDBG_ERR(("BRAP_OP_GetSamplingRate: Unknown output port(%d)",hOp->eOutputPort));
			BDBG_ASSERT(0);
	}/* switch */	
	

	BDBG_MSG(("BRAP_OP_GetSamplingRate outparams:"
		"\n\t SamplingRate = %d, Timebase = %d", *pSamplingRate, *pTimebase));
	
	BDBG_LEAVE(BRAP_OP_GetSamplingRate);	
	return ret;
}
#endif

/**************************************************************************
Summary:
	This function programs the PLL for the output port.
Description:
    This function programs the output sampling rate of an output by programming
    the PLL or Rate Manager.
Returns:
    BERR_SUCCESS on success else error
    
See Also:
 	None
**************************************************************************/

BERR_Code BRAP_OP_P_ProgramOutputClock(
    BRAP_ChannelHandle      hRapCh,
    BRAP_OutputPort         eOp, 
	BAVC_AudioSamplingRate  eOutputSR
    )
{
    BERR_Code               ret = BERR_SUCCESS;
    BRAP_OP_Pll             ePll = BRAP_OP_Pll_eMax;
#if (BRAP_7550_FAMILY == 1)
    BRAP_OP_MClkRate        eMClkRate=BRAP_OP_MClKRate_eMax;
#endif
    bool                    bValidPll = false;
    unsigned int            uiSR = 0;
    BRAP_OP_P_Handle        hOp = NULL, hOpTmp = NULL;
    BRAP_SPDIFFM_P_Handle   hSpdifFm = NULL;
    BRAP_OutputPort         eMaiMuxSelect = BRAP_OutputPort_eMax;      
    BRAP_OutputChannelPair  eChanPair = BRAP_OutputChannelPair_eLR;
    unsigned int            uiPath = 0,uiPp=0;  
#if (BRAP_7550_FAMILY != 1)	
    unsigned    int i=0,j=0,k=0;
    bool    bFound=false;
#endif    
    


    BDBG_ENTER(BRAP_OP_P_ProgramOutputClock);

    /* Validate SR */	
    ret = BRAP_P_ConvertSR(eOutputSR, &uiSR);
    if((ret != BERR_SUCCESS) || (uiSR == 0))
    {
        return BERR_TRACE(ret);
    }

    BDBG_MSG(("hRapCh->eChannelType=%d eOutputPort=%d eSamplingRate=%d uiSR=%d",
                hRapCh->eChannelType, eOp,eOutputSR,uiSR));

    if(BRAP_RM_P_IsOutputPortSupported(eOp) != BERR_SUCCESS)
	{
		BDBG_ERR(("BRAP_OP_P_ProgramOutputClock: eOutputPort(%d) is not supported", eOp));
		return BERR_TRACE(BERR_NOT_SUPPORTED);
	}

    /* Get hOp and hSpdifFm(if any) */
    for(uiPath = 0; uiPath < BRAP_P_MAX_PATHS_IN_A_CHAN; uiPath++)
	{
	    if(hRapCh->pPath[uiPath] == NULL)
        {
            continue;
        }
        
        for(eChanPair=0;eChanPair<BRAP_OutputChannelPair_eMax;eChanPair++)
        {
            for(uiPp=0;uiPp<BRAP_RM_P_MAX_PARALLEL_PATHS;uiPp++)
            {
        		if(hRapCh->pPath[uiPath]->sOp[eChanPair][uiPp].hOp != NULL)
        		{
        		    hOpTmp = hRapCh->pPath[uiPath]->sOp[eChanPair][uiPp].hOp;
        			if(hOpTmp->eOutputPort == eOp)
        			{
        			    hOp = hOpTmp;
        			    hSpdifFm = hRapCh->pPath[uiPath]->sOp[eChanPair][uiPp].hSpdifFm;
#if (BRAP_7405_FAMILY == 1)
/* Get hSPDIFFM if PCM HDMI is enabled & muxselector is I2S */
                        if((NULL != hRapCh->pPath[uiPath]->sOp[eChanPair][uiPp].hMaiOp)&&
                            (NULL == hSpdifFm))
                        {
                            unsigned int uiSpdiffmId = 0;
                            unsigned int uiSpdiffmStreamId = 0;
                            ret = BRAP_RM_P_GetSpdifFmForOpPort(
                                    hRapCh->hRap->hRm,
                                    BRAP_OutputPort_eMai,
                                    eOp,
                                    &(uiSpdiffmId),
                                    &(uiSpdiffmStreamId));
                            hSpdifFm = hRapCh->hRap->hFmm[0]->hSpdifFm[uiSpdiffmStreamId];
                        }
#endif
                        break;
        			}
        		}
            }
            if(NULL != hSpdifFm)
                break;
        }
        if(NULL != hSpdifFm)
            break;
	}

    if(hOp == NULL)
    {
        BDBG_ERR(("BRAP_OP_P_ProgramOutputClock: Could not get hOp"));
        return BERR_TRACE(BERR_NOT_INITIALIZED);
    }

    /* Disable output stream */
    BRAP_OP_P_EnableStream(hOp, false);

    switch(eOp)
	{
		case BRAP_OutputPort_eSpdif:
		case BRAP_OutputPort_eSpdif1:
#if (BRAP_7550_FAMILY != 1)
            ePll = 
    			hOp->hFmm->hRap->sOutputSettings[eOp].uOutputPortSettings.sSpdifSettings.ePll;
#else /* For 7550 Family only */
            eMClkRate = 
    			hOp->hFmm->hRap->sOutputSettings[eOp].uOutputPortSettings.sSpdifSettings.eMClkRate;
#endif
            bValidPll = true;
			break;

		case BRAP_OutputPort_eI2s0:
		case BRAP_OutputPort_eI2s1:
		case BRAP_OutputPort_eI2s2:
		case BRAP_OutputPort_eI2s3:
		case BRAP_OutputPort_eI2s4:
		case BRAP_OutputPort_eI2s5:
		case BRAP_OutputPort_eI2s6:
		case BRAP_OutputPort_eI2s7:
		case BRAP_OutputPort_eI2s8:
			ePll = 
    			hOp->hFmm->hRap->sOutputSettings[eOp].uOutputPortSettings.sI2sSettings.ePll;
            bValidPll = true;
			break;

		case BRAP_OutputPort_eDac0:
        case BRAP_OutputPort_eDac1:
        case BRAP_OutputPort_eDac2:
            ret = BRAP_OP_P_DacSetSampleRate(hRapCh->hRap, eOp, eOutputSR);	
			if(ret != BERR_SUCCESS)
			{
				BDBG_ERR(("BRAP_OP_P_ProgramOutputClock: BRAP_OP_P_DacSetSampleRate() returned error (%d)",ret));	
			}
			bValidPll = false;
			break;
			
		case BRAP_OutputPort_eMai: 
            eMaiMuxSelect = hRapCh->hRap->sOutputSettings[BRAP_OutputPort_eMai].
                               uOutputPortSettings.sMaiSettings.eMaiMuxSelector;

		    if((BRAP_OutputPort_eSpdif == eMaiMuxSelect )||((BRAP_OutputPort_eSpdif1 == eMaiMuxSelect )))
			{
#if (BRAP_7550_FAMILY != 1)			
			    ePll = hOp->hFmm->hRap->sOutputSettings[eMaiMuxSelect].uOutputPortSettings.sSpdifSettings.ePll;
#else
			    eMClkRate = hOp->hFmm->hRap->sOutputSettings[eMaiMuxSelect].uOutputPortSettings.sSpdifSettings.eMClkRate;
#endif
    			bValidPll = true;
            }
#if (BRAP_7405_FAMILY == 1)
            else if(BRAP_OutputPort_eMai == eMaiMuxSelect)
			{
#if (BRAP_7550_FAMILY != 1)						
			    ePll = hOp->hFmm->hRap->sOutputSettings[eMaiMuxSelect].uOutputPortSettings.sMaiSettings.ePll;
#else
			    eMClkRate = hOp->hFmm->hRap->sOutputSettings[eMaiMuxSelect].uOutputPortSettings.sMaiSettings.eMClkRate;
#endif
    			bValidPll = true;
            }
#endif
            else
            {
                BDBG_ERR(("BRAP_OP_P_ProgramOutputClock: Invalid Mux Select"));
                return BERR_TRACE(BERR_NOT_SUPPORTED);
            }
			break;			
            case BRAP_OutputPort_eMaiMulti0:
            case BRAP_OutputPort_eMaiMulti1:
            case BRAP_OutputPort_eMaiMulti2:
            case BRAP_OutputPort_eMaiMulti3:
#if (BRAP_7550_FAMILY != 1)						                
                ePll = hOp->hFmm->hRap->sOutputSettings[eOp].uOutputPortSettings.sMaiMultiSettings.ePll;
#else
    		    eMClkRate = hOp->hFmm->hRap->sOutputSettings[eOp].uOutputPortSettings.sMaiSettings.eMClkRate;
#endif
                bValidPll = true;                
                break;
		default:	
			BDBG_ERR(("BRAP_OP_P_ProgramOutputClock: Unknown output port(%d)",hOp->eOutputPort));
	        return BERR_TRACE(BERR_INVALID_PARAMETER);
	} /* switch */

	if(bValidPll)
	{	        
		/* Program output sampling rate */
#if (BRAP_7550_FAMILY != 1)						                        
     	ret = BRAP_OP_P_ProgramPll(hOp, ePll, eOutputSR);
#else
     	ret = BRAP_OP_P_ProgramNCO(hOp, eMClkRate, eOutputSR);
#endif
        
		if(ret != BERR_SUCCESS)
		{
			BDBG_ERR(("BRAP_OP_P_UpdateSamplingRate: ProgramPll() returned error (%d)",ret));	
		}
        /* Update Channel status bits for SPDIFFM */
        if((eOp == BRAP_OutputPort_eSpdif)||(eOp == BRAP_OutputPort_eSpdif1) || (eOp == BRAP_OutputPort_eMai))
        {
            /*TODO: pass eSR as a separate param in the below function*/
            ret = BRAP_SPDIFFM_P_ProgramChanStatusBits(hSpdifFm,eOutputSR);
       		if(ret != BERR_SUCCESS)
    		{
    			BDBG_ERR(("BRAP_OP_P_UpdateSamplingRate: ProgramChanStatusBits() returned err(%d)",ret));	
    		}
        }
#if (BRAP_7405_FAMILY == 1)
/* Program PCM HDMI Cbits for PCM playback case */
        if(((eOp == BRAP_OutputPort_eI2s0)
            ||(eOp == BRAP_OutputPort_eMaiMulti0))&&(NULL!=hSpdifFm))
        {
            /*TODO: pass eSR as a separate param in the below function*/
            ret = BRAP_SPDIFFM_P_ProgramChanStatusBits(hSpdifFm,eOutputSR);
       		if(ret != BERR_SUCCESS)
    		{
    			BDBG_ERR(("BRAP_OP_P_UpdateSamplingRate: ProgramChanStatusBits() returned err(%d)",ret));	
    		}
        }
#endif
    }

#if (BRAP_7550_FAMILY != 1)	
        bFound=false;
        for(i= 0; i<BRAP_P_MAX_DST_PER_RAPCH;i++)
        {
            if((hRapCh->pDstDetails[i] != NULL) &&
                (hRapCh->pDstDetails[i]->eAudioDst == BRAP_AudioDst_eOutputPort)
                &&(hRapCh->pDstDetails[i]->uDstDetails.sOpDetails.eOutput[0] == eOp))
            {
                for(j=0; j<BRAP_MAX_ASSOCIATED_GROUPS; j++)
                {
                    if(hRapCh->uiAssociationId[j] == BRAP_INVALID_VALUE)
                        continue;

                    for(k=0; k<BRAP_P_MAX_DST_PER_RAPCH; k++)
                    {
                        if(hRapCh->pDstDetails[i] == &(hRapCh->hRap->sAssociatedCh[hRapCh->uiAssociationId[j]].sDstDetails[k].sExtDstDetails))
                        {
                            bFound = true;
                            if(hRapCh->hRap->sAssociatedCh[hRapCh->uiAssociationId[j]].sDstDetails[k].bSampleRateChangeCallbackEnabled == true)
                            {
                                BKNI_EnterCriticalSection();
                                BRAP_P_DestinationSampleRateChange_isr(
                                    (void *)(&(hRapCh->hRap->sAssociatedCh[hRapCh->uiAssociationId[j]].sDstDetails[k]))
                                    ,(unsigned int)eOutputSR);
                                BKNI_LeaveCriticalSection();
                            }
                            break;
                        }
                    }
                    if(bFound == true)
                        break;
                }
            }
            if(bFound == true)
                break;
        }
#endif

    /* Enable stream */
    BRAP_OP_P_EnableStream(hOp, true);
    
    BDBG_LEAVE(BRAP_OP_P_ProgramOutputClock);
    return ret;
}


/***************************************************************************
Summary:
	Configures the External Master Clocks
	
Description:
	The clock info required for the external(off-chip) modules is communicated 
	through the External Master Clock pins of our chip. Depending on which external
	mclk(s) is/are selected by GPIO pins that line is let to drive the clock info out.
	This API configures the external master clock based on the specified pll and
	MClock rate, and enables the corresponding clock line.
	
Returns:
	BERR_SUCCESS 
	
See Also:

****************************************************************************/
BERR_Code BRAP_OP_ExtMClkConfig ( 
	const BRAP_Handle 		hRap, 				/* [in] The RAP Device handle */
	const BRAP_OP_ExtMClk	eExtMClk,			/* [in] External MClk to be programmed */
	BRAP_OP_ExtMClkSettings	*pExtMClkSettings	/* [in] External Master clock settings */
	)
{
	BERR_Code ret = BERR_SUCCESS;
	uint32_t  ui32RegVal = 0;

	BDBG_ENTER (BRAP_OP_ExtMClkConfig);
	BDBG_ASSERT (hRap);
	BDBG_ASSERT (pExtMClkSettings);

	if ( eExtMClk > BRAP_OP_ExtMClk_eExternal1 )
	{
    	BDBG_ERR (("BRAP_OP_ExtMClkConfig(): Invalid ExtMClock [%d]", eExtMClk));
		return BERR_TRACE (BERR_INVALID_PARAMETER);            
	}

	/* Read current value of the register */
	ui32RegVal = BRAP_Read32 (hRap->hRegister, 
	                BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_EXTi_ARRAY_BASE + ( eExtMClk * 4 ));
	ui32RegVal &= ~(BCHP_MASK (AUD_FMM_OP_CTRL_MCLK_CFG_EXTi, PLLCLKSEL)); 

#if (BRAP_7550_FAMILY != 1)
	switch ( pExtMClkSettings->ePll )
	{
		case BRAP_OP_Pll_ePll0:
			/* Select the PLL Channel based on the MClock rate */
			switch (pExtMClkSettings->eMClockRate)
			{
				case BRAP_OP_MClkRate_e128Fs:
					ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_EXTi, 
											PLLCLKSEL, PLL0_ch1));
					break;     
				case BRAP_OP_MClkRate_e256Fs:
					ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_EXTi, 
											PLLCLKSEL, PLL0_ch2));
					break;     
				case BRAP_OP_MClkRate_e512Fs:
					ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_EXTi, 
											PLLCLKSEL, PLL0_ch3));
					break;     
				default:
					BDBG_ERR (("BRAP_OP_ExtMClkConfig(): Incorrect MClk Rate %d", 
								pExtMClkSettings->eMClockRate));
					return BERR_TRACE (BERR_NOT_SUPPORTED);
			}     
			break;
#if (BRAP_PLL1_SUPPORTED == 1)   
		case BRAP_OP_Pll_ePll1:			
			/* Select the PLL Channel based on the MClock rate */
			switch (pExtMClkSettings->eMClockRate)
			{
				case BRAP_OP_MClkRate_e128Fs:
					ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_EXTi, 
											PLLCLKSEL, PLL1_ch1));
					break;     
				case BRAP_OP_MClkRate_e256Fs:
					ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_EXTi, 
											PLLCLKSEL, PLL1_ch2));
					break;     
				case BRAP_OP_MClkRate_e512Fs:
					ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_EXTi, 
											PLLCLKSEL, PLL1_ch3));
					break;     
				default:
					BDBG_ERR (("BRAP_OP_ExtMClkConfig(): Incorrect MClk Rate %d", 
								pExtMClkSettings->eMClockRate));
					return BERR_TRACE (BERR_NOT_SUPPORTED);
			}     
			break;
#endif
		default:
    		BDBG_ERR (("BRAP_OP_ExtMClkConfig(): Invalid PLL [%d]",
                    			pExtMClkSettings->ePll));
			return BERR_TRACE (BERR_NOT_SUPPORTED); 
	}
#else
    switch ( pExtMClkSettings->ePll )
    {
    		case BRAP_OP_Pll_ePll0:
    		case BRAP_OP_Pll_ePll1:			
    			/* Set MCLK rate*/
				ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_EXTi, 
								PLLCLKSEL, SPDIF_MCLK));
                break;     
    		case BRAP_OP_Pll_eHifiDac0:	
    			ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_EXTi, 
    							PLLCLKSEL, Hifidac0));
    			break;
    		default:
            		BDBG_ERR (("BRAP_OP_ExtMClkConfig(): Invalid PLL [%d]",
                            			pExtMClkSettings->ePll));
    			return BERR_TRACE (BERR_NOT_SUPPORTED); 
	}
#endif
	BRAP_Write32 (hRap->hRegister, 
			BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_EXTi_ARRAY_BASE + ( eExtMClk * 4 ), 
			ui32RegVal);

#if (BRAP_3548_FAMILY == 1)
    /* Enable the requested EXTMCLK field in AUD_FMM_MISC_SEROUT_OE */
    ui32RegVal = BRAP_Read32 (hRap->hRegister, BCHP_AUD_FMM_MISC_SEROUT_OE);
    if(BRAP_OP_ExtMClk_eExternal0 == eExtMClk)
    {
    	ui32RegVal &= ~(BCHP_MASK (AUD_FMM_MISC_SEROUT_OE, EXTMCLK0_OE)); 
        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE,	EXTMCLK0_OE, Drive));    
    }
    else
    {
    	ui32RegVal &= ~(BCHP_MASK (AUD_FMM_MISC_SEROUT_OE, EXTMCLK1_OE)); 
        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE,	EXTMCLK1_OE, Drive));    
    
    }
	BRAP_Write32 (hRap->hRegister, BCHP_AUD_FMM_MISC_SEROUT_OE, ui32RegVal);
#endif

	/* Store the settings in FMM handle for future use */
	hRap->hFmm[0]->sExtMClkSettings[eExtMClk].eMClockRate = pExtMClkSettings->eMClockRate;
	hRap->hFmm[0]->sExtMClkSettings[eExtMClk].ePll = pExtMClkSettings->ePll;

	BDBG_LEAVE (BRAP_OP_ExtMClkConfig);
	return ret;
}  

#if (BRAP_7550_FAMILY != 1)    
#if ((BRAP_7405_FAMILY == 1)||(BRAP_3548_FAMILY == 1))
/***************************************************************************
Summary:
    Generic helper routine that configures the MCLK rate and PLLCLKSEL registers
    for FS Timing Source.
    
    Note: Currently, FS timing source is used by CapPort and FCI sinkers.
****************************************************************************/
BERR_Code
BRAP_P_ConfigureFsTimingSource(
    BREG_Handle             hRegister,      /* [in] Register Handle */
    unsigned int            uiFsTmgSrcId,   /* [in] FS Timing Source Id */
    BRAP_OP_Pll 			ePll,           /* [in] PLL to be associated */
    BAVC_AudioSamplingRate 	eSamplingRate   /* [in] Sampling rate of the data at 
                                               CapPort or FCI sinker driven by 
                                               this FS timing source */
    )
{
    uint32_t ui32RegVal = 0;
    uint32_t ui32FsOffset = 0;
    uint32_t ui32Offset = 0;

    BDBG_ENTER(BRAP_P_ConfigureFsTimingSource);

    BDBG_MSG (("The uiFsTmgSrcId=%d,ePll=%d,eSamplingRate=%d",uiFsTmgSrcId,ePll,eSamplingRate));

    /* TODO: remove this and decide on PLL internally for all the output ports
       - failure case when only HiFiDAC is fed using sequential path.. there is 
       no PLL for internal cap-port */
    if(ePll == BRAP_OP_Pll_eMax)
    {
        ePll = BRAP_OP_Pll_ePll0;
#if ((BRAP_7405_FAMILY == 1))
        /* Program the MACRO_SELECT = The desired sampling rate */
#if (BRAP_PLL1_SUPPORTED == 1)   
        ui32Offset = (int)(ePll) * (BCHP_AUD_FMM_PLL1_MACRO - BCHP_AUD_FMM_PLL0_MACRO);    
#else
        ui32Offset =0;
#endif
        ui32RegVal = BRAP_Read32(hRegister, BCHP_AUD_FMM_PLL0_MACRO + ui32Offset);
        ui32RegVal &= ~(BCHP_MASK(AUD_FMM_PLL0_MACRO, MACRO_SELECT));
#else
        /* Program the MACRO_SELECT = The desired sampling rate */
        ui32Offset = (int)(ePll) * (BCHP_VCXO_CTL_MISC_AC1_MACRO - BCHP_VCXO_CTL_MISC_AC0_MACRO);    
        ui32RegVal = BRAP_Read32(hRegister, BCHP_VCXO_CTL_MISC_AC0_MACRO + ui32Offset);
        ui32RegVal &= ~(BCHP_MASK(VCXO_CTL_MISC_AC0_MACRO, MACRO_SELECT));

#endif
    	switch(eSamplingRate)
        {   /* Check enum values */ 
            case BAVC_AudioSamplingRate_e32k:
            case BAVC_AudioSamplingRate_e64k:
            case BAVC_AudioSamplingRate_e128k:
                /* All 32kHz family of sampling rates are achieved using 32kHz 
                   as the base sampling rate. Using PLL channel select 1x, 2x or 3x 
                   to achieve 32kHz, 64kHz or 128kHz */
#if ((BRAP_7405_FAMILY == 1))                   
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_PLL0_MACRO, MACRO_SELECT, Mult_of_32000));
#else
                ui32RegVal |= (BCHP_FIELD_ENUM (VCXO_CTL_MISC_AC0_MACRO, MACRO_SELECT, Mult_of_32000));
#endif
                break;
            case BAVC_AudioSamplingRate_e44_1k:
            case BAVC_AudioSamplingRate_e88_2k:
            case BAVC_AudioSamplingRate_e176_4k:
                /* All 44.1kHz family of sampling rates are achieved using 44.1kHz 
                   as the base sampling rate. Using PLL channel select 1x, 2x or 3x 
                   to achieve 44.1kHz, 88.2kHz or 176.4kHz */
#if ((BRAP_7405_FAMILY == 1))                                      
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_PLL0_MACRO, MACRO_SELECT, Mult_of_44100));
#else
                ui32RegVal |= (BCHP_FIELD_ENUM (VCXO_CTL_MISC_AC0_MACRO, MACRO_SELECT, Mult_of_44100));
#endif
                break;
            case BAVC_AudioSamplingRate_e48k:
            case BAVC_AudioSamplingRate_e96k:
            case BAVC_AudioSamplingRate_e192k:
                /* All 48kHz family of sampling rates are achieved using 48kHz as 
                   the base sampling rate. Using PLL channel select 1x, 2x or 3x to 
                   achieve 48kHz, 96kHz or 192kHz */
#if ((BRAP_7405_FAMILY == 1))                   
                ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_PLL0_MACRO, MACRO_SELECT, Mult_of_48000));
#else
                ui32RegVal |= (BCHP_FIELD_ENUM (VCXO_CTL_MISC_AC0_MACRO, MACRO_SELECT, Mult_of_48000));
#endif
                break;
            default:
                BDBG_ERR (("BRAP_OP_P_ProgramPll_isr(): Incorrect Sampling Rate %d", eSamplingRate));
                return BERR_TRACE (BERR_INVALID_PARAMETER);
        }
#if ((BRAP_7405_FAMILY == 1))                           
        BRAP_Write32(hRegister, BCHP_AUD_FMM_PLL0_MACRO + ui32Offset , ui32RegVal);
#else
        BRAP_Write32(hRegister, BCHP_VCXO_CTL_MISC_AC0_MACRO + ui32Offset , ui32RegVal);
#endif
    }

    if((uiFsTmgSrcId >= BRAP_RM_P_MAX_FS_TMG_SOURCES) ||
       (eSamplingRate == BAVC_AudioSamplingRate_eUnknown) ||
       (ePll == BRAP_OP_Pll_eMax))
    {
        BDBG_ERR(("ConfigureFsTimingSrc: uiFsTmgSrcId=%d eSamplingRate=%d ePll=%d",
            uiFsTmgSrcId,eSamplingRate,ePll));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    /* Calculate FS register offset wrt to AUD_FMM_OP_CTRL_MCLK_CFG_FSi[0] */
    ui32FsOffset = 4 * uiFsTmgSrcId;
        
    /* Fix MCLK rate to 1 (= MCLK_is_128fs) */
    ui32RegVal = 0;
    ui32RegVal |= BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_FSi, 
                                        MCLK_RATE, MCLK_is_128fs);

    /* Program PLLCLKSEL as per below guidelines: 

        -----------------------------------
        Output SR - Base SR - PLLCLKSEL
        ---------   --------  -------------
            32    -    32   -   ch1 (or 1x)
            64    -    32   -   ch2 (or 2x)
            128   -    32   -   ch3 (or 4x)

            48    -    48   -   ch1 (or 1x)
            96    -    48   -   ch2 (or 2x)
            192   -    48   -   ch3 (or 4x)
            
            44.1  -    44.1 -   ch1 (or 1x)
            88.2  -    44.1 -   ch2 (or 2x)
            176.4 -    44.1 -   ch3 (or 4x)

        ----------------------------------- */
#if (BRAP_7550_FAMILY != 1)
    switch(eSamplingRate)
    {
        case BAVC_AudioSamplingRate_e32k:
        case BAVC_AudioSamplingRate_e48k:
        case BAVC_AudioSamplingRate_e44_1k:
        switch (ePll)
        {
            case BRAP_OP_Pll_ePll0:            
                ui32RegVal |= BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_FSi, 
                            PLLCLKSEL, PLL0_ch1);
                break;
#if (BRAP_PLL1_SUPPORTED == 1)   
            case BRAP_OP_Pll_ePll1:            
                ui32RegVal |= BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_FSi, 
                            PLLCLKSEL, PLL1_ch1);
                break;
#endif                
            default:
                BDBG_ERR (("BRAP_P_ConfigureFsTimingSource(): Incorrect PLL %d", ePll));
                return BERR_TRACE (BERR_NOT_SUPPORTED);
        } 
        break;            
        
        case BAVC_AudioSamplingRate_e64k:
        case BAVC_AudioSamplingRate_e96k:
        case BAVC_AudioSamplingRate_e88_2k:
        switch (ePll)
        {
            case BRAP_OP_Pll_ePll0:            
                ui32RegVal |= BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_FSi, 
                            PLLCLKSEL, PLL0_ch2);
                break;
#if (BRAP_PLL1_SUPPORTED == 1)   
            case BRAP_OP_Pll_ePll1:            
                ui32RegVal |= BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_FSi, 
                            PLLCLKSEL, PLL1_ch2);
                break;
#endif                
            default:
                BDBG_ERR (("BRAP_P_ConfigureFsTimingSource(): Incorrect PLL %d",ePll));
                return BERR_TRACE (BERR_NOT_SUPPORTED);
        } 
        break;            
            
        case BAVC_AudioSamplingRate_e128k:
        case BAVC_AudioSamplingRate_e192k:
        case BAVC_AudioSamplingRate_e176_4k:
        switch (ePll)
        {
            case BRAP_OP_Pll_ePll0:            
                ui32RegVal |= BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_FSi, 
                            PLLCLKSEL, PLL0_ch3);
                break;
#if (BRAP_PLL1_SUPPORTED == 1)   
            case BRAP_OP_Pll_ePll1:            
                ui32RegVal |= BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_FSi, 
                            PLLCLKSEL, PLL1_ch3);
                break;
#endif                
            default:
                BDBG_ERR (("BRAP_P_ConfigureFsTimingSource(): Incorrect PLL %d",ePll));
                return BERR_TRACE (BERR_NOT_SUPPORTED);
        } 
        break;

        default:
            BDBG_ERR (("BRAP_P_ConfigureFsTimingSource(): Unsupported SR %d",eSamplingRate));
            BDBG_ASSERT(0);
    }
#else /* For 7550 Family only */
        ui32RegVal |= BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MCLK_CFG_FSi, 
                    PLLCLKSEL, SPDIF_MCLK);
#endif
    BRAP_Write32 (hRegister, 
                  BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_FSi_ARRAY_BASE + ui32FsOffset, 
                  ui32RegVal);

    BDBG_LEAVE(BRAP_P_ConfigureFsTimingSource);
    return BERR_SUCCESS;
}

#endif
#endif    


#if 0
/***************************************************************************
Summary:
	Gets the sampling rate and time base associated with the output port.
Description:
	This API is used to get the sampling rate and time base associated with 
	a particular output port.
Returns:
	BERR_SUCCESS 
See Also:
****************************************************************************/
BERR_Code BRAP_GetSamplingRate ( 
	const BRAP_Handle 		hRap, 			/* [in] The RAP Channel handle */
	const BRAP_OutputPort	eOutputPort, 	/* [in] Output port */
	BAVC_AudioSamplingRate  *pSamplingRate, /* [out] Output Sampling rate */
	BAVC_Timebase			*pTimebase 		/* [out] Timebase to use for the output port */
	)
{
	BERR_Code ret = BERR_SUCCESS;

    BDBG_ENTER(BRAP_GetSamplingRate);
    BDBG_ASSERT(hRap);
    ret = BRAP_OP_GetSamplingRate(hRap, eOutputPort, pSamplingRate, pTimebase);
    BDBG_LEAVE(BRAP_GetSamplingRate);
    return ret;
}
#endif
/* End of File */
