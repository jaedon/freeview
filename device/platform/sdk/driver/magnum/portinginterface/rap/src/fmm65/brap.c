/***************************************************************************
*     Copyright (c) 2006-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brap.c $
* $brcm_Revision: Hydra_Software_Devel/300 $
* $brcm_Date: 8/24/12 5:02p $
*
* Module Description:
*	This file contains the implementations of the APIs, which are exposed 
*	to the upper layer by the Raptor Audio PI. This file is part of Audio 
*   Manager Module. Audio Manager module is the top level module of the 
*   RAP PI, which interfaces with the caller and manages the internal 
*   channel objects to get RAP PI produce the desired result.
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/rap/base3/brap.c $
* 
* Hydra_Software_Devel/300   8/24/12 5:02p purush
* SW7425-3090: [7425] Added support for run-time programmable
* VCXO/timebase settings
* 
* Hydra_Software_Devel/299   8/24/12 11:52a purush
* SW7420-2271: [7420] Added support for run-time programmable
* VCXO/timebase settings
* 
* Hydra_Software_Devel/298   5/24/12 8:22p sgadara
* SW7435-174: [7550] Support Destination sample rate info for 7550.
* Fixing build issue as well
* 
* Hydra_Software_Devel/297   5/21/12 6:35p sgadara
* SW7435-174: [7125] Destination interrupts are now supported for output
* ports as well
* 
* Hydra_Software_Devel/296   4/11/12 1:21p purush
* SW7340-297: [7340]Moved path structure memory allocation from start
* channel to open channel. Porting the changes to Mainline.
* 
* Hydra_Software_Devel/295   2/8/12 2:03p sgadara
* SW7420-2230: [7420] Change the Feedback buffer from circular to linear
* 
* Hydra_Software_Devel/294   1/20/12 4:35p purush
* SW7420-1898: [7420] Forking at hardware SRC is causing audio
* discontinuity. Removing default to true setting of bIndOpVolCtrl in
* "BRAP_Settings".
* 
* Hydra_Software_Devel/293   9/8/11 11:39a srajapur
* SW7125-1051: [7125] Coverity Defect ID:33737 CHECKED_RETURN brap.c
* Product=97125 SW7125-1056: [7125] Coverity Defect ID:32287
* CHECKED_RETURN brap.c Product=97125
* 
* Hydra_Software_Devel/292   8/10/11 11:03a srajapur
* SW7420-696:[7420]Fixing the compilation error.
* 
* Hydra_Software_Devel/291   7/25/11 12:15p gourabm
* SW7420-696:[97420]Fix Coverity detected issues - 7420
* 
* Hydra_Software_Devel/290   6/3/11 4:01p gautamk
* SW7325-852: [7325] Programming default MCLK for spdif to 128fs. In case
* of DTS Compressed to PCM transition program it to 256fs.
* 
* Hydra_Software_Devel/289   5/25/11 3:14p gautamk
* SW7420-767: [7420] DDRE stage can be added on single decode. Adding
* support for it
* 
* Hydra_Software_Devel/288   5/16/11 3:59p gautamk
* SW7405-4713:[7405] reducing debug level.
* 
* Hydra_Software_Devel/287   5/3/11 10:57a sgadara
* SW7420-1756: [7420] Fixing an issue and better handling on unsupported
* configuration
* 
* Hydra_Software_Devel/286   4/25/11 7:50p sgadara
* SW7420-1756: [7420] Convert error prints to messages
* 
* Hydra_Software_Devel/285   4/21/11 10:35p sgadara
* SW7420-1756: [7420] Support BRAP_GetCurrentPostProcessingStages for
* Association PP
* 
* Hydra_Software_Devel/284   4/20/11 3:41p sgadara
* SW7420-1756: [7420] Implementing code review comments --> Change the
* Pointer validity check function to a macro and name it aptly
* 
* Hydra_Software_Devel/283   4/19/11 5:24p gautamk
* SW7125-901: [7125] Fixing bug in spdif cbit updation
* 
* Hydra_Software_Devel/282   4/18/11 3:03p sgadara
* SW7420-1756: [7420] Implementing code review comments and correcting
* the indentations
* 
* Hydra_Software_Devel/281   4/6/11 3:20p gautamk
* SW7125-901: [7125] MAI channel Status bit should be picked from
* different structure.
* 
* Hydra_Software_Devel/280   3/29/11 6:34p gautamk
* SW7405-5192:[7405] Adding API to change the audio play rate on the fly.
* 
* Hydra_Software_Devel/279   3/21/11 7:06p sgadara
* SW7420-767: [7420] Fixing the issue in removal case of PP
* 
* Hydra_Software_Devel/278   3/18/11 11:06p sgadara
* SW7420-767: [7420] Fixing non-MS build issue
* 
* Hydra_Software_Devel/277   3/18/11 8:48p sgadara
* SW7420-767: [7420] Merging MS11 development branch to mainline
* 
* Hydra_Software_Devel/276   3/15/11 8:51p sgadara
* SWDEPRECATED-3659: [7405] Adding API BRAP_GetCapabilities --> Fixing
* warning
* 
* Hydra_Software_Devel/275   3/15/11 4:34p gautamk
* SWDEPRECATED-3659: [7405] Adding API BRAP_GetCapabilities
* 
* Hydra_Software_Devel/274   3/7/11 6:05p pothiraj
* SW7550-648: [7550] Secure Code Module feature
* 
* Hydra_Software_Devel/273   2/7/11 1:58p gautamk
* SW7550-631: [7550] Sample rate value and mode value in
* Getstreaminformation() is taken from payload sent by FW through
* interrupt.
* 
* Hydra_Software_Devel/272   2/1/11 6:44p gautamk
* SW7420-1202: [7420]Merging StandBy/resume code to mainline
* 
* Hydra_Software_Devel/271   12/15/10 5:12a gautamk
* SW7420-1202: [7420] Merging PM changes for RAP
* 
* Hydra_Software_Devel/SW7420-1202_t2/2   11/19/10 3:32p jtna
* SW7420-1202: [7420] disable/destroy all callbacks on Close
* 
* Hydra_Software_Devel/SW7420-1202_t2/1   11/19/10 11:10a jtna
* SW7420-1202: [7420] StartChannel/StopChannel-based dynamic PM
* 
* Hydra_Software_Devel/270   10/26/10 3:49p gautamk
* SW7420-1202: [7420] merging PM related changes
* 
* Hydra_Software_Devel/269   10/22/10 7:41p sgadara
* SW3556-1206: [3548,3556] Merge customer branch to main line
* 
* Hydra_Software_Devel/268   10/21/10 7:07p srajapur
* SW7550-601 : [7550] Adding PI Support for BTSC Encoder
* 
* Hydra_Software_Devel/267   10/20/10 6:02p gautamk
* SW7405-4713:[7405] Adding debug message
* 
* Hydra_Software_Devel/266   10/13/10 5:46p sgadara
* SW7405-4066: [3548] Implementing code review comments --> Returning
* error for AMR passthru
* 
* Hydra_Software_Devel/265   10/7/10 5:04p gautamk
* SW7340-212: [7335] muting at mixer level as well when calling
* BRAP_SetOutputmute
* 
* Hydra_Software_Devel/264   10/6/10 4:10p srajapur
* SW7468-298 :[7468]Fixed the warning.
* 
* Hydra_Software_Devel/263   9/24/10 8:14p sgadara
* SW7468-298 :[3548] Fixing the build caused by merging Real Video Code
* to main latest
* 
* Hydra_Software_Devel/262   9/22/10 3:08p gautamk
* SW7405-4713: [7405] Fixing runtime warning.
* 
* Hydra_Software_Devel/261   9/16/10 12:17a srajapur
* SW7468-298 :[7468]Merging Real Video Code to main latest
* 
* Hydra_Software_Devel/260   9/6/10 5:14p gautamk
* SW7550-459: [7550] Merging GFX support in mainline
* 
* Hydra_Software_Devel/259   8/27/10 3:34p gautamk
* SW7405-4821: [7405] Removing handle of hMasterChannel from hPriDecCh
* and hSecDecCh appropriately
* 
* Hydra_Software_Devel/258   8/17/10 3:15p gautamk
* SW7405-4713: [7405] Correction in user config
* 
* Hydra_Software_Devel/257   8/16/10 7:36p gautamk
* SW7405-4713: [7405] correcting the order to free the memory
* 
* Hydra_Software_Devel/256   8/10/10 1:52p gautamk
* SW7420-767: [7420] Merging MS11 feature support to mainline
* 
* Hydra_Software_Devel/255   8/5/10 3:18p gautamk
* SW7405-4602: [7405] Using Temporray StageStatus Buffer from pool of
* buffers.
* 
* Hydra_Software_Devel/254   7/23/10 3:50p gautamk
* SW7405-4646: [7405] Restoring mute status of O/P at mixer Level in
* every Start.
* 
* Hydra_Software_Devel/253   7/22/10 5:57p gautamk
* SW7400-2848: [7405] Adding support for on the fly input sample rate
* programming for PB channel.
* 
* Hydra_Software_Devel/252   7/16/10 4:52p gautamk
* SW7405-713:[7405] Fixing coverity issue.
* 
* Hydra_Software_Devel/251   6/30/10 5:28p gautamk
* SW7405-3063: [7405] Removing obselote parameters.
* 
* Hydra_Software_Devel/250   6/24/10 6:06p gautamk
* SW7405-4539: [7405] setting hTimer to NULL after Destroying it.
* 
* Hydra_Software_Devel/249   6/22/10 6:11p gautamk
* SW7335-774: [7335] Moving the frequent dynamic Memory allocations of
* structures to BRAP_Open and store it in hRap. Using it from hRap
* whenever required.
* 
* Hydra_Software_Devel/248   6/21/10 4:19p gautamk
* SW7405-3063: [7405] Fixing bug in compressed Rbuf capture.
* 
* Hydra_Software_Devel/247   6/16/10 2:19p gautamk
* SW7405-3063: [7405] Fixing compilation warning with DEBUG=n option
* 
* Hydra_Software_Devel/246   6/9/10 1:19p gautamk
* SW7405-4453: [7405] DSOLA sanity check only applicable for decode
* channel
* 
* Hydra_Software_Devel/245   5/31/10 2:48p gautamk
* SW7335-730: [7335] Fixing coverity bugs in mainline
* 
* Hydra_Software_Devel/244   5/28/10 6:36p sgadara
* SW3556-1009: [3548,3556] Adding required debug messages  for Equalizer
* module.
* 
* Hydra_Software_Devel/243   5/27/10 2:40p gautamk
* SW7405-4387: [7405] Fixing coverity issue
* 
* Hydra_Software_Devel/242   5/17/10 5:38p gautamk
* SW7405-4321: [7405] Adding the support to change the timebase on the
* fly
* 
* Hydra_Software_Devel/241   5/17/10 10:53a gautamk
* SW7405-4170: [7405] Merging MS10 changes to mainline
* 
* Hydra_Software_Devel/240   5/4/10 8:58p sgadara
* SW3548-2852: [3548,3556] Providing support to add same Equalizer to
* multiple output ports. --> Currently the multiple output ports should
* belong to same channel.
* 
* Hydra_Software_Devel/239   4/29/10 8:57p sgadara
* SW3548-2910: [3548,3556] Addding support for Custom Equalizer mode
* 
* Hydra_Software_Devel/238   4/22/10 5:11p sgadara
* SW7405-4170:[7405,3548,3556] Making chages to internally map AACHE and
* AC3 Encode to Dolby Pulse and DD Transcode in PI for MS10 license
* 
* Hydra_Software_Devel/237   4/19/10 4:19p sgadara
* SW3556-1009: [3548,3556,7405] Adding valid debug messages and prints --
* > Porting the changes to main line
* 
* Hydra_Software_Devel/236   4/15/10 6:00p sgadara
* SW3548-2852: [3548,3556] Improving the error handling mechanism.
* 
* Hydra_Software_Devel/235   4/13/10 5:14p gautamk
* SW7405-4164:[7405] bLastRunHadDtsTranscodeOnHdmi will be set when MAI
* is getting Compressed.
* 
* Hydra_Software_Devel/234   4/9/10 9:58p gautamk
* SW7405-4168: [7405] using MACRO instead of 100.
* 
* Hydra_Software_Devel/233   4/9/10 4:46p gautamk
* SW7405-4107: [7405] Fixing the watchdog recoveru when decoder in paused
* state.
* 
* Hydra_Software_Devel/232   4/9/10 4:00p gautamk
* SW7405-4168: [7405] Adding support for 0.5x to 2x playback rate.
* 
* Hydra_Software_Devel/231   4/8/10 3:25p gautamk
* SW7125-269: [7125] Fixing Coverity issue
* 
* Hydra_Software_Devel/230   4/7/10 6:43p gautamk
* SW7405-4170: [7405] Adding Support for Multi Stream Decoder. Master and
* Secodary channel will be mixed by FwMixer Processing stage.
* 
* Hydra_Software_Devel/229   3/26/10 10:56a sgadara
* SW3548-2859: [3548,3556] Adding Subwoofer(Low Pass Filter) to Equalizer
* settings. --> Started using SRC1 block
* 
* Hydra_Software_Devel/228   3/15/10 2:53p gautamk
* SW7420-599: [7405] For HDMI 5.1, Volume change needs to be stored for
* its muxed output as well. So that in case of watchdog it can be
* recovered.
* 
* Hydra_Software_Devel/227   3/11/10 5:09p speter
* SW3548-2822:[3548] Add Dolby Transcode and Broadcom 3D surround post
* processing algos
* 
* Hydra_Software_Devel/226   3/3/10 5:28p sgadara
* SW3548-2686: [3548,3556] Updating the default value of WindowStep size
* of PEQ
* 
* Hydra_Software_Devel/225   3/2/10 4:04p sgadara
* SW3556-965: [3548,3556] Changing the default ramp step size from 0x200
* to 0xA00 to reduce the ramp time
* 
* Hydra_Software_Devel/224   2/12/10 5:10p gautamk
* SW7325-591: [7335] Adding feature to keep output sample rate to 48khz,
* irrespective of stream sample rate. Porting to mainline
* 
* Hydra_Software_Devel/223   2/10/10 5:27p sgadara
* SW3548-2739-2686: [3548,3556] Fixing build issue for non-DTV platforms
* 
* Hydra_Software_Devel/222   2/8/10 6:09p sgadara
* SW3548-2739-2686: [3548,3556] Modifying the Equalizer implementation
* design to support PEQ, and GEQ+Tone Control.
* --> After Mixer 2 SRCs will be connected back to back if Eq is added
* 
* Hydra_Software_Devel/221   2/4/10 3:01p gautamk
* SW7405-3851:[7405] Programming only for 7405 family.
* 
* Hydra_Software_Devel/220   2/1/10 2:42p gautamk
* SW7405-3851: [7405] moving the initialization of MS/OP reg for SPDIF
* and HDMI from fmm init to SetOutputConfig. This implementation will
* not initialize corresponding regs if Bord doesn't support anyonr of
* these o/p.
* 
* Hydra_Software_Devel/219   1/27/10 6:02p srajapur
* SW7550-221 : [7550] Fixing the coverity issues.
* 
* Hydra_Software_Devel/218   1/19/10 11:05a sgadara
* SW3556-1001: [3548,3556] Writing the Default Value for Fade module
* --> Merge to main line
* 
* Hydra_Software_Devel/217   1/13/10 2:46p gautamk
* SW7405-3202: [7405] implementing code review comments
* 
* Hydra_Software_Devel/216   1/11/10 7:09p sgadara
* SW3556-905: [3548,3556] Adding "Capture of Mixed Audio" feature
* 
* Hydra_Software_Devel/215   1/6/10 9:12p sgadara
* SW3556-905: [3548,3556] Adding "Capture of Mixed Audio" feature
* 
* Hydra_Software_Devel/214   12/18/09 5:04p gautamk
* SW7405-3063:[7405] Correcting typo error
* 
* Hydra_Software_Devel/213   12/15/09 11:44a gautamk
* SW7405-3063: [7405] Checking Malloc against NULL, Because 0xDEADDATA
* and 0xDATADEAD can be valid address.
* 
* Hydra_Software_Devel/212   12/2/09 2:21p gautamk
* SW7405-3063:[7405] Making bGateOpened = false at stopchannel
* 
* Hydra_Software_Devel/211   12/1/09 12:01p gautamk
* SW7325-625: [7405] Scaling coefficients are preserved in Mixing
* coefficients array.
* 
* Hydra_Software_Devel/210   11/23/09 3:22p gautamk
* SW7405-3063: [7405] adding break statements
* 
* Hydra_Software_Devel/209   11/19/09 11:12a gautamk
* SW7405-3063: [7405] adding break statement
* 
* Hydra_Software_Devel/208   11/15/09 12:21p speter
* SW3556-832: [3548] Adding the control to either pause or continue when
* the ring buffer destination is full
* 
* Hydra_Software_Devel/207   11/12/09 2:35p gautamk
* SW7420-448 : [7420] Adding new API to Program RFMOD source
* 
* Hydra_Software_Devel/206   11/10/09 2:27p speter
* SW7405-2959: [3548] Adding DTS-HD support
* 
* Hydra_Software_Devel/205   11/3/09 5:16p gautamk
* SW7405-3337:[7405] Updating SPDIF Ch Status Bit on the fly for both
* PCM/Compressed output.
* 
* Hydra_Software_Devel/204   10/29/09 5:43p gautamk
* SW7405-3312: [7405] Fixing crash.
* 
* Hydra_Software_Devel/203   10/27/09 12:41p gautamk
* SW7405-3110: [7405]Adding Destination sample rate change interrupt.
* This will need to be installed for HDMI. Whenver sample rate changes,
* this interrupt gets trigerred. So app need to install callback
* function on it to program HDMI rx sample rate. Checkin at mainline
* 
* Hydra_Software_Devel/202   10/21/09 5:01p gautamk
* SW7335-609:[7405] Setting hTask-bStopped appropriately in Isr Ctxt when
* Event is getting set. This is done to fix race condition. Porting to
* mainline
* 
* Hydra_Software_Devel/201   10/15/09 2:42p gautamk
* SW7405-3063: [7405] Misc changes.
* - Programming SRC for Bypass case.
* 
* Hydra_Software_Devel/200   10/8/09 1:20p gautamk
* SW7405-3063: [7405] doing misc changes related to post processing
* stages and AD.
* 
* Hydra_Software_Devel/199   10/7/09 4:32p gautamk
* SW7405-3063: [7405] Adding Delay parameter for Rbuf.
* 
* Hydra_Software_Devel/198   10/6/09 6:35p srajapur
* SW7550-52 : [7550] Merging branch 7550 code to main line
* 
* Hydra_Software_Devel/197   10/5/09 12:22p sgadara
* SW3548-379: [3548,3556] Fixing the coverity issue.
* 
* Hydra_Software_Devel/196   10/2/09 2:33a srajapur
* SW7550-52: [7550] Merging 7550 branch code to main line
* 
* Hydra_Software_Devel/195   9/28/09 1:10p gautamk
* SW7405-3070: [7405] exposing RAP_INDEP_DELAY_DISABLED, to disable INDEP
* delay at compile time.
* 
* Hydra_Software_Devel/194   9/28/09 12:26p gautamk
* SW7405-3074 : [7405] restructuring the error checks.
* 
* Hydra_Software_Devel/193   9/25/09 3:32p gautamk
* sw7405-3081:[7405] Do not program PLL used by started O/P when calling
* SetOutputconfig for some other non-started O/P
* 
* Hydra_Software_Devel/191   9/14/09 3:36p gautamk
* SW7405-2534:[7405]  Programming PLLs and Hifidac Rate MGr at open time
* also.
* 
* Hydra_Software_Devel/190   9/8/09 2:31p gautamk
* SW7405-2986: [7405]Enabinge DTS Broadcast Pasthru without DTS
* Licensing. Also Enable Other Passthru which can be allowed without
* license
* 
* Hydra_Software_Devel/189   9/3/09 6:09p gautamk
* SW7325-388: [7405] fixing compilation error
* 
* Hydra_Software_Devel/188   9/3/09 5:43p gautamk
* SW7325-388: [7405] Storing Volume Level in both Db and 5.23 Format.
* Also both these paramter remains in Sync when calling different Volume
* APIs
* 
* Hydra_Software_Devel/187   8/25/09 12:07p speter
* SW3548-2411: [3548] Merging the phase 5.0 release branch to mainline
* 
* Hydra_Software_Devel/RAP_354856_PH_5_0/1   8/1/09 9:25a sgadara
* PR 56830: [3548,3556] Making the changes as to remove the earlier PP
* branch before the new branch settings(with same/different stage
* settings) are applied by the application
* 
* Hydra_Software_Devel/186   8/18/09 5:11p gautamk
* PR56006: [7405] correcting comment
* 
* Hydra_Software_Devel/185   8/17/09 5:13p gautamk
* PR57743:[7405] using Pvt functions when calling from internal RAP PI
* 
* Hydra_Software_Devel/184   8/14/09 1:42p gautamk
* PR56006: [7405] CORRECTING COMMENT
* 
* Hydra_Software_Devel/183   8/14/09 1:21p gautamk
* PR56006: [7405] modifying flush Api for PB.
* 
* Hydra_Software_Devel/182   8/12/09 3:40p gautamk
* PR56006: [7405] Adding flush Api for PB.
* 
* Hydra_Software_Devel/181   7/23/09 4:48p bhanus
* PR 55499 : [7340,7342,7125]Adding 7420 Family support
* 
* Hydra_Software_Devel/180   7/9/09 5:53p sgadara
* PR 56061: [3548,3556] Adding support for Subsonic(High Pass) filter to
* main line.
* 
* Hydra_Software_Devel/179   6/30/09 11:56a gautamk
* PR56006: [7405] Using the settings done through Brap_SetInputConfig to
* program capport
* 
* Hydra_Software_Devel/178   6/23/09 5:00p gautamk
* PR56006:[7405] removing unwanted code.
* 
* Hydra_Software_Devel/177   6/23/09 4:12p gautamk
* PR56006: [7405] fixing compilation
* 
* Hydra_Software_Devel/176   6/23/09 2:30p gautamk
* PR56006: [7405]  Adding fix for  a case where Dts passthru to PCm
* transition on
* HDMI soesn't happen on Onkyo.
* 
* Hydra_Software_Devel/175   6/18/09 2:41p gautamk
* PR56112: [7420] fixing compile error
* 
* Hydra_Software_Devel/174   6/18/09 2:18p gautamk
* PR56112:[7420] Adding PI support for HDMI 5.1PCm for 7420
* 
* Hydra_Software_Devel/173   6/17/09 3:43p gautamk
* PR56040: [7405] fixing compilation warning
* 
* Hydra_Software_Devel/172   6/17/09 3:07p gautamk
* PR56040:[7405] For 5.1PCM on HDMI , when changing volume at mixer
* level, change mixer volume for its mux selector as well
* 
* Hydra_Software_Devel/171   6/12/09 3:45p gautamk
* PR53302: [7405] Merging cleanup code to mainline
* 
* Hydra_Software_Devel/170   6/9/09 12:32p gautamk
* PR55572:[7405] Before accessing any pointer location ccheck if it is
* 0XDA7ADEAD or 0xDEADDA7A in addition to NULL.
* 
* Hydra_Software_Devel/169   6/9/09 12:03p gautamk
* PR55135: [7405] If sample rate changes on the fly, SRC of PB and CAP
* should also get configured accordingly
* 
* Hydra_Software_Devel/168   6/5/09 5:19p gautamk
* PR55572: [7405] Adding State BRAP_P_State_eStopInvoked to handle race
* condition between Stopchannel and  Isr functions like SetStcValid_isr
* etc.
* 
* Hydra_Software_Devel/167   5/28/09 3:44p gautamk
* PR50455: [7405] renaming parameter name appropriately.
* 
* Hydra_Software_Devel/166   5/28/09 3:28p gautamk
* PR50455: [7405]
* - During DestinationMute, Muting at both Mixer and O/P level to achieve
* ramping.
* - Exposing API to set the ramp step of Pin/pong coefficents, so that
* ramping could happen at input side of mixer.
* 
* Hydra_Software_Devel/165   5/27/09 4:03p gautamk
* PR52383: [7405]If SetPostProcessing for a destination is getting called
* twice for same hRapCh, Current code will create 2 branch corresponding
* to it, which is wrong. i.e. there can be 1 branch per Rapch on
* destination . Fixing it.
* 
* Hydra_Software_Devel/164   5/22/09 4:42p gautamk
* PR55135: [7405] Moving SRC programming of PB/CAP from Samplerate_isr to
* RampEnable_isr
* 
* Hydra_Software_Devel/163   5/15/09 3:13p gautamk
* PR52383: [7405] Adding test API to capture RingBuffer.
* 
* Hydra_Software_Devel/162   5/14/09 4:23p gautamk
* PR50455: [7405]Adding APIs BRAP_GetMixerRampStepSize()
* BRAP_SetMixerRampStepSize()
* BRAP_GetSrcRampStepSize()
* BRAP_SetSrcRampStepSize()
* 
* Hydra_Software_Devel/161   5/12/09 3:36p gautamk
* PR54608:[7405] Adding API BRAP_GetTotalErrorCount(), which returns sum
* of following errors
*        PTS_Error + Crc_Error + Dec_unlock +TSM_Fail
* 
* Hydra_Software_Devel/160   4/28/09 1:51p gautamk
* PR52383: [7405] fixing compilation error
* 
* Hydra_Software_Devel/159   4/28/09 1:25p gautamk
* PR52383: [7405] This implementaion is to fix a case where Dts XCD to
* PCm transition on
* HDMI soesn't happen on Onkyo reciever when we don't diable MAI_CFG at
* MAI_STOP
* 
* Hydra_Software_Devel/158   4/20/09 3:55p gautamk
* PR52383: [7405] Creating seperate event handles for each task.
* 
* Hydra_Software_Devel/157   4/8/09 2:54p gautamk
* PR52383:[7405] fixing compilation warning,
* 
* Hydra_Software_Devel/156   4/8/09 2:43p gautamk
* PR52383: [7405] Fixing coverity issue.
* 
* Hydra_Software_Devel/155   4/6/09 12:06p gautamk
* PR53647: [7405] Moving StartWRPtrLocation to StartChannel from
* OpenChannel
* 
* Hydra_Software_Devel/154   4/2/09 1:44p gautamk
* PR53647: [7405] exposing  StartWr pointer for Plaback channel Rbufs.
* 
* Hydra_Software_Devel/153   4/2/09 11:18a speter
* PR 52729: [3548,3556] Adding bHbrEnable flag to SPDIF Output as High
* bit rate is required to be supported through SPDIF out as well.
* 
* Hydra_Software_Devel/152   3/25/09 11:25a gautamk
* PR53546:[7405]  correcting memset sequence
* 
* Hydra_Software_Devel/151   3/23/09 3:54p gautamk
* PR52334: [7405] Modifying register programming for MS/OP/PLL blocks, to
* solve audio loss on spdif.
* 
* Hydra_Software_Devel/150   3/20/09 1:23p speter
* PR 53300: [3548,3556] Adding SPDIF Rx Open during watchdog recovery
* 
* Hydra_Software_Devel/149   3/16/09 3:12p speter
* PR 52729: [3548,3556] Adding DDP Passthru support at 192kHz through
* SPDIF Out
* 
* Hydra_Software_Devel/148   3/12/09 3:30p speter
* PR 52729: [3548,3556] Adding DDP Passthru support at 192kHz through
* SPDIF Out
* 
* Hydra_Software_Devel/147   2/27/09 12:26p speter
* PR 52178: [3548,3556] During watchdog recovery top level interrupts
* (AIO_INTH) have been cleared and masked but are not restored the same
* mask status, the earlier unamsked interrupts remain masked after
* watchdog recovery.
* --->Adding the code to unmask.
* 
* Hydra_Software_Devel/146   2/23/09 11:52a gautamk
* PR52383: [7405] Correcting the call sequence of Enable/Disabel Callback
* of Int module as expected by this module
* 
* Hydra_Software_Devel/145   2/23/09 11:44a gautamk
* PR52069: [7325] Programming O/P clock for PCM playabck/Capture channel
* when, Decode channel is started but didn't started decoding. This can
* happen when scrabled data is feeded to decoder. Checkin in mainline.
* 
* Hydra_Software_Devel/144   2/13/09 4:59p gautamk
* PR51897: [7405] using SHaredPP Path type incase of Playback and capture
* channel for 7405
* 
* Hydra_Software_Devel/143   2/2/09 3:40p gautamk
* PR51618: [7405] Changing the global variables as per following rule.
* 1) make them static
* 2) if you cannot make them static because they must be shared between
* multiple .c files in your module, you must rename them to include the
* BXXX_ magnum prefix for your module.
* 
* Hydra_Software_Devel/142   1/27/09 3:51p speter
* PR 50495: [3548,3556] Merging the customer branch to main line
* 
* Hydra_Software_Devel/141   1/22/09 5:25p gautamk
* PR50455: [7405] Setting VOLUME STEP RAMP to 0x20 when DSP FW triggers
* EnableRamp interrupt.
* 
* Hydra_Software_Devel/140   1/21/09 4:00p gautamk
* PR50415: [7405] Storing the coefiicients in hRapch handle from
* BRAP_P_ProgramCoefficients
* 
* Hydra_Software_Devel/139   1/13/09 1:54p gautamk
* PR48850: [7405] Adding debug function to read RBUF.
* 
* Hydra_Software_Devel/138   12/10/08 10:57p gautamk
* PR49232:[7405] Adding support for Allocating Rbuf/SrcCh and Paths at
* OpenChannel time.
* 
* Hydra_Software_Devel/RAP_354856_PH_4_1/4   1/27/09 1:58p speter
* PR 50740: [3548,3556] Adding simul mode support for HDMI/SPDIF In
* through generic passthru stage
* 
* Hydra_Software_Devel/RAP_354856_PH_4_1/3   1/19/09 3:21p sgadara
* PR 50859: [3548,3556] Adding the code to trigger watchdog whenever
* there is a timeout error.
* 
* Hydra_Software_Devel/RAP_354856_PH_4_1/2   1/2/09 6:53p sgadara
* PR 50786: [3548,3556] Changing the checking of AudioSource in from
* channel handle to audio params.
* 
* Hydra_Software_Devel/RAP_354856_PH_4_1/1   12/16/08 2:33p sgadara
* PR 49305: [3548,3556] Fixing the compiler warning.
* 
* Hydra_Software_Devel/137   12/2/08 9:33p sgadara
* PR 49716: [7325] Updating the mute status so as to take care for both
* cases of calling BRAP_SetDestinationMute() and BRAP_SetOutputMute() by
* application.
* 
* Hydra_Software_Devel/136   12/2/08 1:27p sgadara
* PR 48894: [3548,3556] Clearing channel associations for all channel
* types on BRAP_DestroyAssociation.
* 
* Hydra_Software_Devel/135   11/25/08 6:09p bhanus
* PR 47757 : [7420]Checking in 7420 Support
* 
* Hydra_Software_Devel/134   11/25/08 11:27a speter
* PR 49564: [3548,3556] Merging the Phase 4.0 release to main line
* 
* Hydra_Software_Devel/132   11/11/08 4:48p gautamk
* PR48791:[7405] Removing the instance of uiAssociatioId from hRapCh when
* Association is destroyed.
* Also order of creating the Path is changed in getchannel path.
* 
* Hydra_Software_Devel/RAP_354856_PH_4_0/5   11/8/08 1:54p sgadara
* PR 42025: [3548,3556] Add volume control, with cascaded mixers.
* Enabling independent volume control only for custom miser case.
* 
* Hydra_Software_Devel/RAP_354856_PH_4_0/4   11/8/08 12:22a sgadara
* PR 42025: [3548,3556] Add volume control, with cascaded mixers.
* 
* Hydra_Software_Devel/RAP_354856_PH_4_0/3   11/7/08 2:48p sgadara
* PR 43158: [3548,3556] Adding support for minimal delay through decoder.
* 
* Hydra_Software_Devel/RAP_354856_PH_4_0/2   11/6/08 4:19p sgadara
* PR 48564: [3548,3556] Switching to PCM or PES during channel start for
* HDMI/SPDIF.
* 
* Hydra_Software_Devel/RAP_354856_PH_4_0/1   10/31/08 6:09p sgadara
* PR 42025: [3548,3556] Add volume control, with cascaded mixers. --
* >Provide separate mixers for each output port.
* 
* Hydra_Software_Devel/131   10/29/08 3:24a speter
* PR 44771: [3548,3556] Adding interrupt for Ring Buffer Overflow and
* cleaning up base3 code to remove 3563 and 7440 specific code
* 
* Hydra_Software_Devel/130   10/29/08 1:27a speter
* PR 48399: [3548,3556] Added the code in RemoveDevice level callback for
* SPDIFRx.
* 
* Hydra_Software_Devel/129   10/22/08 4:17p gautamk
* PR42605: [7405] Fixing coverity issue.
* 
* Hydra_Software_Devel/128   10/20/08 2:26p gautamk
* PR47961: [7405] Fixing warning and compilation for 7405 platform due to
* changes of this PR.
* 
* Hydra_Software_Devel/127   10/18/08 8:19a speter
* PR 44768: [3548,3556,7405] Moving bIndOpVolCtrl from channel params to
* BRAP_Settings.
* 
* Hydra_Software_Devel/126   10/18/08 3:27a speter
* PR 47360: [3548,3556]: Changing the design of Spdif Rx. SPDIF Rx is now
* a device level interrupt and handling of digital input format change
* is moved from RAP PI to higher layer.
* 
* Hydra_Software_Devel/125   10/18/08 3:22a speter
* PR 47360: [3548,3556]: Changing the design of Spdif Rx. SPDIF Rx is now
* a device level interrupt and handling of digital input format change
* is moved from RAP PI to higher layer.
* 
* Hydra_Software_Devel/124   10/18/08 2:40a speter
* PR 47961: [3548,3556] Allowing Setinputconfig when the channel is
* running for RFAudio
* 
* Hydra_Software_Devel/123   10/17/08 6:02a speter
* PR 47360: [3548,3556]: Changing the design of Spdif Rx. SPDIF Rx is now
* a device level interrupt and handling of digital input format change
* is moved from RAP PI to higher layer.
* 
* Hydra_Software_Devel/122   10/17/08 2:29a speter
* PR 47360: [3548,3556]: Changing the design of Spdif Rx. SPDIF Rx is now
* a device level interrupt and handling of digital input format change
* is moved from RAP PI to higher layer.
* 
* Hydra_Software_Devel/121   10/12/08 11:50p gautamk
* PR43454: [7405] fixing compilation and warning.
* 
* Hydra_Software_Devel/120   10/10/08 10:28a speter
* PR 47764: [3548,3556] Merging the release branch 3.0 to main line of
* development
* 
* Hydra_Software_Devel/RAP_354856_PH_3_0/11   10/9/08 11:13a speter
* PR 45444: [3548,3556] Adding the equalizer code for Sampling rate
* 
* Hydra_Software_Devel/RAP_354856_PH_3_0/10   10/7/08 11:41a sgadara
* PR 47576: [3548,3556] Programming the application provided volume
* levels for destination at start time.
* 
* Hydra_Software_Devel/RAP_354856_PH_3_0/9   10/6/08 8:18p speter
* PR 45444: [3548,3556] Adding equalizer related code
* 
* Hydra_Software_Devel/RAP_354856_PH_3_0/8   10/6/08 3:39p sgadara
* PR 47576: [3548,3556] Extending destination volume control on ouput
* ports.
* 
* Hydra_Software_Devel/RAP_354856_PH_3_0/7   10/3/08 2:54p speter
* Pr 46611: [3548,3556] Fixing the issue in rbuf capture related to
* watermark
* 
* Hydra_Software_Devel/RAP_354856_PH_3_0/6   10/3/08 2:42p speter
* PR 44835: [3548,3556] Changing the APIs to remove hRap as it can be
* derived internally
* 
* Hydra_Software_Devel/RAP_354856_PH_3_0/5   10/3/08 2:10p speter
* PR 44835: [3548,3556] Changing the APIs to remove hRap as it can be
* derived internally
* 
* Hydra_Software_Devel/RAP_354856_PH_3_0/4   10/2/08 10:41a speter
* PR 44835: [3548,3556] Adding Mute feature for Destination
* 
* Hydra_Software_Devel/RAP_354856_PH_3_0/3   9/26/08 10:46p speter
* PR 47311: [3548,3556] Adding defensive code in PI to stop failures from
* upper layers.
* 
* Hydra_Software_Devel/RAP_354856_PH_3_0/2   9/24/08 6:32p speter
* PR 38950: [3548,3556] Adding "disable TSM" if the SPDIF Rx input is
* PCM.
* 
* Hydra_Software_Devel/RAP_354856_PH_3_0/1   9/24/08 3:51p speter
* PR 45444: [3548,3556] Adding equalizer code. Changing the API to add
* Destination and not output port
* 
* Hydra_Software_Devel/119   9/22/08 2:31p speter
* PR 45095: [3548,3556] Requests for unsupported post-processing should
* fail gracefully. Fixing encode post processing algorithm check
* 
* Hydra_Software_Devel/118   9/22/08 10:38a speter
* PR 45095: [3548,3556] Requests for unsupported post-processing should
* fail gracefully
* 
* Hydra_Software_Devel/117   9/19/08 8:45p sgadara
* PR 44770: [3548,3556] Implementing the Destination based Interrupts for
* 'capture to ringbuffer' feature.
* 
* Hydra_Software_Devel/116   9/19/08 6:22p gautamk
* PR45739: [7405] Changing bLargeRbuf to bIndepDelayEnabled
* 
* Hydra_Software_Devel/115   9/16/08 4:02p speter
* PR 46942: [3548,3556] Merging back to main line of development to get
* the distinct output feature
* 
* Hydra_Software_Devel/114   9/16/08 2:43p sushmit
* PR 43454:[7405]Adding AC3 Encode Support.
* 
* Hydra_Software_Devel/113   9/12/08 1:23p gautamk
* PR45739: [7405] removing unwanted function call.
* 
* Hydra_Software_Devel/112   9/9/08 6:50p sushmit
* PR 46211: [7405] Adding Data swapping support for PCM Playback
* 
* Hydra_Software_Devel/111   9/9/08 3:10p gautamk
* PR42605: [7405] Fixing coverity issue.
* 
* Hydra_Software_Devel/110   9/8/08 5:27p gautamk
* PR45739: [7405] Adding support for indep delay for o/p
* 
* Hydra_Software_Devel/109   9/7/08 5:50p sushmit
* PR 36583:[7405] Added two new APIs.
* 
* Hydra_Software_Devel/108   9/5/08 9:02p sushmit
* PR 45972:[7405] Adding default image interface & context to
* DefaultSettings
* 
* Hydra_Software_Devel/107   9/5/08 11:55a sushmit
* PR 45923:[7405]Removing AACXptFormat
* 
* Hydra_Software_Devel/RAP_354856_20080904_BRANCH/3   9/5/08 9:11p sgadara
* PR 44835: [3548,3556] Implementing destination-based volume APIs for
* volume control on capture data.
* 
* Hydra_Software_Devel/RAP_354856_20080904_BRANCH/2   9/5/08 2:44p speter
* PR 38950: [3548,3556] Adding support for SPDIF In
* 
* Hydra_Software_Devel/RAP_354856_20080904_BRANCH/1   9/5/08 11:15a sgadara
* PR 44140: [3556] Fixing Coverity issues.
* 
* Hydra_Software_Devel/106   8/25/08 2:12p speter
* PR 45444: [3548,3556] Implementing the hardware equalizer. Adding extra
* cascaded SRCs for equalizer upsampling, IIR and downsampling
* 
* Hydra_Software_Devel/105   8/21/08 4:37p speter
* PR 45444: [3548,3556] Adding the hardware equalizer support.
* Implementing the algorithm provided by the architecture team. This is
* the initial version and not complete
* 
* Hydra_Software_Devel/104   8/20/08 6:10p gautamk
* PR43454: [7405] Changes for Concurrent downmix.
* 
* Hydra_Software_Devel/103   8/14/08 12:07p sushmit
* PR 43454:[7405]Updating DSOLA Params.
* 
* Hydra_Software_Devel/102   8/13/08 1:34a gautamk
* PR43454: [7405] If any destination is getting removed, then remove the
* instances of hDestination handle from the hAudprocessing handle
* 
* Hydra_Software_Devel/101   8/11/08 8:46p gautamk
* PR45379: [7405] MIT table should not get initialized when watch is
* triggered.
* 
* Hydra_Software_Devel/100   8/10/08 11:55a sushmit
* PR 43454:[7405] DTSENC Updates.
* 
* Hydra_Software_Devel/99   8/6/08 2:52p speter
* PR 45444: [3548,3556] Adding the hardware equalizer support for 5 band
* equalization and Tone control
* 
* Hydra_Software_Devel/98   8/5/08 3:57p speter
* PR 45407: [3548,3556] Merging the Phase 2.1 release branch to main line
* of development (Hydra_Software_Devel)
* 
* Hydra_Software_Devel/97   8/4/08 4:43p gautamk
* PR43454: [7405] fixing a minor bug.
* 
* Hydra_Software_Devel/RAP_354856_PH_2_1/2   8/5/08 12:01p speter
* PR 45374: [3548,3556] Adding autotrap support to 3548 and 3556
* 
* Hydra_Software_Devel/RAP_354856_PH_2_1/1   8/4/08 10:56a speter
* PR 45061: [3548,3556] Changing the audio descriptor API from
* ConfigureAudioDescriptorTable to Get/Set pair
* 
* Hydra_Software_Devel/96   7/28/08 9:24a speter
* PR 45171: [3548,3556] Merging from Phase 2.0 release to main line of
* development (Hydra_Software_Devel)
* 
* Hydra_Software_Devel/95   7/23/08 9:23p gautamk
* PR43454: [7405] Adding PI support for WMA Passthru
* 
* Hydra_Software_Devel/RAP_354856_PH_2_0/1   7/24/08 10:18a speter
* PR 45032: [3548,3556] Add DSP support to capture channel (I2S In and
* ADC In)
* 
* Hydra_Software_Devel/94   7/22/08 1:29p speter
* PR 45032: [3548,3556] Adding DSP support to SPDIF/HDMI In
* 
* Hydra_Software_Devel/93   7/18/08 11:16a speter
* PR 44780: [3548,3556] Support for HDMI In
* 
* Hydra_Software_Devel/92   7/17/08 4:17p sgadara
* PR 44895: [3548] To bring up 'capture to ringbuffer'  feature on 3548.
* 
* Hydra_Software_Devel/91   7/11/08 4:34p gautamk
* PR43454: [7405] adding the Macro for checking validity of
* Branch/Stage/network
* 
* Hydra_Software_Devel/90   7/11/08 1:14p gautamk
* PR43454: [7405] fixing compilation error for autotrap.
* 
* Hydra_Software_Devel/89   7/10/08 6:19p gautamk
* PR41825: [7405] merging the code for multiple associatio from Branch to
* mainline.
* 
* Hydra_Software_Devel/88   7/9/08 10:24a speter
* PR 44660:[3548,3556] Merging the Phase 1.5 release branch code to main
* line of development
* 
* Hydra_Software_Devel/87   7/8/08 7:18p gautamk
* PR43454: [7405] written function to determine if the path outputs
* compress data.
* 
* Hydra_Software_Devel/86   7/3/08 11:29p gautamk
* PR42605: [7405] fixing coverity issues for 7405
* 
* Hydra_Software_Devel/85   7/2/08 8:12p gautamk
* PR43454: [7405] correcting the compiler warning.
* 
* Hydra_Software_Devel/84   6/26/08 9:12p gautamk
* PR43454: [7405] Adding sanity check for the network for audio
* processing stages.
* 
* Hydra_Software_Devel/83   6/24/08 3:44p sushmit
* PR 44097: [7325] Fixing KLOCKWORK issue.
* 
* Hydra_Software_Devel/82   6/24/08 7:04a speter
* PR 38950: [3548,3556] Adding support for SPDIF In without DSP.
* 
* Hydra_Software_Devel/81   6/18/08 9:14p speter
* PR 38950: [3548,3556] Fixing the NICAM module
* 
* Hydra_Software_Devel/80   6/16/08 10:52a gdata
* PR41726: [7405] Adding changes for audio descriptor
* 
* Hydra_Software_Devel/79   6/15/08 11:54p speter
* PR 38950: [3548,3556] Removing the brap_btsc_priv.c in compile and
* adding file brap_rfaudio_priv.c
* 
* Hydra_Software_Devel/78   6/15/08 4:05a speter
* PR 38950: [3548,3556] Adding support for hardware equalizer. Adding the
* interface
* 
* Hydra_Software_Devel/77   6/14/08 7:36a speter
* PR 42110: [3548,3556] Adding ADC support
* 
* Hydra_Software_Devel/76   6/11/08 7:29a speter
* PR38950: [3548,3556] Changing generic use of BTSC as RF Audio
* 
* Hydra_Software_Devel/75   6/6/08 4:52p gautamk
* PR41726: [7405] not making tsm status buffer invalid in dspchn_start(),
* if its getting called from internal function like FlushChannel()
* 
* Hydra_Software_Devel/74   6/5/08 4:29p gautamk
* PR41726: [7405]  If Fw verification is enabled, then don't download the
* Executable again in watchdog recovery
* 
* Hydra_Software_Devel/73   6/4/08 11:09p gautamk
* PR43314:[7405] Segmentataion fault occurs on trigerring watchdog(via
* BBS) with playback in pause state
* PR43312:[7405]
* 
* Hydra_Software_Devel/73   6/4/08 11:08p gautamk
* PR43314:[7405] Segmentataion fault occurs on trigerring watchdog(via
* BBS) with playback in pause state
* PR43312:[7405]
* 
* Hydra_Software_Devel/72   6/2/08 3:31p sushmit
* PR 41726: [7405] Fixing early triggering of Interrupt for PCM Playback.
* 
* Hydra_Software_Devel/71   6/1/08 2:52a gautamk
* PR41726: [7405] Resetting trickmode states only when Stop channel in
* called from external function
* 
* Hydra_Software_Devel/70   5/30/08 8:47p speter
* PR 38950: [3548,3556] Adding hRap as parameter to AudioDesc Object
* 
* Hydra_Software_Devel/69   5/30/08 2:20p gautamk
* PR41726: [7405] changes for audio descriptor
* 
* Hydra_Software_Devel/68   5/30/08 4:05a speter
* PR 38950: [3548,3556] Adding the firmware interface address parameter.
* 
* Hydra_Software_Devel/67   5/29/08 9:46p gautamk
* PR41726: [7405] doing Memset after Malloc
* 
* Hydra_Software_Devel/66   5/29/08 3:27p gautamk
* PR41726: [7405] Adding function to insert/destroy stages internally in
* Task network.
* 
* Hydra_Software_Devel/65   5/29/08 4:21a speter
* PR 38950: [3548,3556] Adding the audio descriptor support
* 
* Hydra_Software_Devel/64   5/29/08 4:05a speter
* PR 38950: [3548,3556] Adding the audio descriptor support
* 
* Hydra_Software_Devel/63   5/28/08 5:09a speter
* PR 39442: [3548,3556] Moving the countrycode parameter of BTSC from
* open to Inputconfig
* 
* Hydra_Software_Devel/62   5/28/08 2:40a speter
* PR 39442: [3548,3556] Adding the Nicam and Pal A2 support
* 
* Hydra_Software_Devel/61   5/23/08 1:28p gautamk
* PR42302: [7325] Fixing the race condition between MIPS and DSP for
* updating ET bit of wachdog timer.
* PR42807: [7335]
* PR42837: [7325]
*
* Hydra_Software_Devel/60   5/22/08 2:51p sushmit
* PR 42964: [7405] Fixing wrong instruction
* 
* Hydra_Software_Devel/59   5/20/08 4:19p speter
* PR 42110: [3548,3556] ADC code update
* 
* Hydra_Software_Devel/58   5/20/08 1:55p speter
* PR 42110: [3548,3556] Adding ADC support
* 
* Hydra_Software_Devel/57   5/15/08 3:37p gautamk
* PR42664: [7405] setting hTask->bStopped to false when watchdog recovery
* is called so that no command gets called till start of channel
* 
* Hydra_Software_Devel/56   5/14/08 4:30p speter
* PR 38950: [3548,3556] Adding code to support HIFIDAC1 and HIFIDAC2
* 
* Hydra_Software_Devel/55   5/13/08 6:27p gautamk
* PR42504:[7405] Not sending task command if the stop command for task
* already sent.
* 
* Hydra_Software_Devel/54   5/9/08 4:16p gautamk
* PR42302:[7405] If watchdogISR is trigerred in between when PI waiting a
* response/ack from FW DSP, Then PI shoul exit gracefully. And then
* ProcessWatchdogRecovery() function should be called
* 
* PR42504:[7405]
* 
* Hydra_Software_Devel/53   5/9/08 2:58p speter
* PR 38950: [3548,3556] Fixing the code for 3548 family
* 
* Hydra_Software_Devel/52   5/9/08 1:27p speter
* PR 38950: [3548,3556] Adding SpdifRx support
* 
* Hydra_Software_Devel/51   5/9/08 1:17p gautamk
* PR42495: [7405] Freeing the task handle memory correctly.
* 
* Hydra_Software_Devel/50   5/5/08 11:36a speter
* PR 38950: [3548] Making 3548_FAMILY macro and using it for both 3548
* and 3556 cases.
* 
* Hydra_Software_Devel/49   5/2/08 9:41p sushmit
* PR 41726: [7405] Small fix
* 
* Hydra_Software_Devel/48   5/2/08 2:38p gautamk
* PR41726: [7405] Adding PI support for Fw Verification in mainline
* 
* Hydra_Software_Devel/47   4/30/08 7:40p gautamk
* PR42335: [7405] downgrade non-error BDBG_ERR's to either WRN or MSG
* 
* Hydra_Software_Devel/46   4/28/08 4:26p speter
* PR 38950: [3548] Fixing compilation for 3548/3556.
* 
* Hydra_Software_Devel/45   4/24/08 11:42a gautamk
* PR41949: [7405] during DestroyAssociation, it checks if all the
* destination attched to it is removed or not. If not, returns an error.
* 
* Hydra_Software_Devel/44   4/22/08 1:58p gautamk
* PR41994: [7405] Using BCHP_VER_ Correctly...
* 
* Hydra_Software_Devel/43   4/18/08 7:13p gautamk
* PR41726: [7405] setting  hDestHandle->psProcessingSettings[i] = NULL;
* after freeing up psProcessingSettings[i] , to get rid of dangling
* pointer.
* 
* Hydra_Software_Devel/42   4/18/08 3:11p gautamk
* PR41726: [7405] freeing hDestHandle->psProcessingSettings in
* removeDestination.
* 
* Hydra_Software_Devel/41   4/17/08 8:39p gautamk
* PR41726: [7405] Reusing hDestHandle->psProcessingSettings[] after
* removing/adding post processing stage on destination handle. i.e.
* hDestHandle->psProcessingSettings[]->hRapCh is null then
* psProcessingSettings  can be used.
* 
* Hydra_Software_Devel/40   4/16/08 6:04p gautamk
* PR41726: [7405] moving the variables from heap to stack
* 
* Hydra_Software_Devel/39   4/16/08 12:04p gautamk
* PR41726: [7405] Coverity fixes.
* 
* Hydra_Software_Devel/38   4/14/08 2:57p sushmit
* pr 34648: [7405] Fixing DesTroyProcessingStage
* 
* Hydra_Software_Devel/37   4/14/08 12:07p sushmit
* PR 41016: [7405] Fixing PCM destination getting added to passthru
* channel
* 
* Hydra_Software_Devel/36   4/14/08 11:38a sushmit
* PR 34648: [7405] Reducing Stack Usage
* 
* Hydra_Software_Devel/35   4/13/08 11:01p sushmit
* PR 34648: [7405] Fixing Start-Stop-Start in Brutus when PP Stages are
* enabled.
* 
* Hydra_Software_Devel/34   4/11/08 6:47p sushmit
* PR 34648: [7405] Reducing Stack Usage
* 
* Hydra_Software_Devel/33   4/11/08 5:52p sushmit
* PR 34648: [7405] Fixing BRAP_GetCurrentPostProcessingStages
* 
* Hydra_Software_Devel/32   4/11/08 4:37p sushmit
* PR 41365: [7405] Allowing only AC3 Passthru without license.
* 
* Hydra_Software_Devel/31   4/11/08 3:52p sushmit
* PR 34648: [7405] Removing deprecated bSupportDec/Enc/PPAlgos
* 
* Hydra_Software_Devel/30   4/11/08 12:50a gautamk
* PR34648: [7405] Adding PI support for Trick modes.
* 
* Hydra_Software_Devel/29   4/8/08 2:16p gautamk
* PR34648: [7405] Merging wdog code to mainline
* 
* Hydra_Software_Devel/28   4/8/08 1:28p sushmit
* PR 41365: [7405] Adding AC3 passthru without requiring AC3 license.
* 
* Hydra_Software_Devel/28   4/8/08 1:26p sushmit
* PR 41365: [7405] Adding AC3 passthru without requiring AC3 license.
* 
* Hydra_Software_Devel/27   4/8/08 10:11a nitinb
* PR 39989: [7405] Added support for DDP pass through
* 
* Hydra_Software_Devel/26   4/1/08 2:48p sushmit
* PR 40820: [7405] Bringing up DD Convert
* 
* Hydra_Software_Devel/RAP_NEWFWARCH_WDOG/3   4/8/08 1:18p gautamk
* PR34648: [7405] adding code for wdog...
* 
* Hydra_Software_Devel/RAP_NEWFWARCH_WDOG/2   4/1/08 2:41p gautamk
* PR34648: [7405] Adding changes in WDOg branch
* 
* Hydra_Software_Devel/RAP_NEWFWARCH_WDOG/1   3/28/08 4:01p gautamk
* PR34648: [7405] adding code for watchdog recovery in a branch
* 
* Hydra_Software_Devel/25   3/24/08 2:46p sushmit
* PR 34648: [7405] Kernel Mode Unaligned Memory Access Fix, removing
* BDBG_ERRs, fix for multiple dest.
* 
* Hydra_Software_Devel/24   3/18/08 6:59p gautamk
* PR34648: [7405] Fixing Bugs for GenCItOutput and createProcessing stage
* 
* Hydra_Software_Devel/23   3/13/08 9:23p sushmit
* PR 40511: [7405] Increasing Rave Thresholds
* 
* Hydra_Software_Devel/22   3/7/08 1:43p sushmit
* PR 34648: [7325] Moving some KLOCWORK fixes to base3 & also fixing a
* memory leak.
* 
* Hydra_Software_Devel/21   3/4/08 6:40p gautamk
* PR34648: [7405] adding paramater in hRapch to have the userconfig
* parameters in Appformat for some fields.
* 
* Hydra_Software_Devel/20   3/3/08 6:53p srajapur
* PR34648: [7405] Fixed the coverity issues.
* 
* Hydra_Software_Devel/19   3/3/08 6:38p gautamk
* PR34648: [7405] Implementing review comments.
* 
* Hydra_Software_Devel/18   3/3/08 5:26p sushmit
* PR34648: [7405] Updating SR code & BRAP_FlushChannel
* 
* Hydra_Software_Devel/17   3/3/08 3:49p sushmit
* PR 39255: [7405] Moving output port timebase related changes to base3
* 
* Hydra_Software_Devel/16   3/3/08 2:49p gautamk
* PR34648: [7405] Fixing coverity defects.
* 
* Hydra_Software_Devel/15   2/29/08 3:46p gautamk
* PR34648: [7405] Removing Unwanted function
* 
* Hydra_Software_Devel/14   2/29/08 3:41p gautamk
* PR34648: [7405] Adding on the fly change for spdifstatuparam
* 
* Hydra_Software_Devel/13   2/28/08 10:01a gdata
* PR34648: [7405] Changes of review comment
* 
* Hydra_Software_Devel/12   2/26/08 6:37p gautamk
* PR34648: [7405] Fixing bugs and programming RDBDsp Offset to CIT.
* 
* Hydra_Software_Devel/11   2/22/08 4:17p gautamk
* PR34648: [7405] fixing coverity defects + implementing review comments
* 
* Hydra_Software_Devel/10   2/19/08 7:57p gautamk
* PR34648: [7405] Fixing Compiler warnings.
* 
* Hydra_Software_Devel/9   2/19/08 4:06p gautamk
* PR34648: [7405] Updating function names.
* 
* Hydra_Software_Devel/8   2/19/08 3:31p gautamk
* PR34648: [7405] updating file names and fuction names
* 
* Hydra_Software_Devel/7   2/18/08 6:07p gautamk
* PR34648: [7405] updating default value of bOpentimeFwdownlaod
* 
* Hydra_Software_Devel/6   2/13/08 7:44p gautamk
* PR34648 : [7405]Moving Samity check for audio processing algorithm from
* setProcessingstage() to StartChannel()
* 
* Hydra_Software_Devel/5   1/31/08 7:44p nitinb
* PR34648: [7405] Implemented review comments on stack usage
* 
* Hydra_Software_Devel/4   1/29/08 7:08p gdata
* PR34648: [7405] Adding support for close calls
* 
* Hydra_Software_Devel/3   1/24/08 1:26p sushmit
* PR34648: [7405] Updating code for Passthru bringup
* 
* Hydra_Software_Devel/2   1/24/08 8:18a sushmit
* PR34648: [7405] Merging in updated code from Base2
* 
* Hydra_Software_Devel/222   1/9/08 8:16p bhanus
* PR38296: [7440] Changes done during DTS-HD SUB Audio bring up.
* 
* Hydra_Software_Devel/221   1/9/08 10:56a speter
* PR 37182: [3563] Fixing the Set PP Custom Config code for SPDIF/HDMI In
* 
* Hydra_Software_Devel/220   1/7/08 9:23p sushmit
* PR 38527: [7405] Fixing Flush
* 
* Hydra_Software_Devel/219   1/4/08 1:55p speter
* PR 38001: [3563] Fixing the SPDIF In Passthru issue. This was happening
* as we were not providing proper ring buffer for firware to do the
* swap.
* 
* Hydra_Software_Devel/218   12/22/07 10:32a sgadara
* PR 38424: [7405] To get Autotrap up on 7405
* 
* Hydra_Software_Devel/218   12/21/07 8:55p sgadara
* PR 38424: [7405] To get Autotrap up on 7405
* 
* Hydra_Software_Devel/217   12/21/07 2:23p sushmit
* PR 36084: [7405] Fixing issue
* 
* Hydra_Software_Devel/216   12/20/07 3:14p sushmit
* PR 36084: [7405] Proper storage for scaling coeffs created
* 
* Hydra_Software_Devel/215   12/20/07 2:03p sushmit
* PR 36084: [7405] Making Balance coeffs as zero in BRAP_SetScaling
* 
* Hydra_Software_Devel/214   12/19/07 9:48p sushmit
* PR 36084: [7405] Fixing Mixing case for independent output volume
* control
* 
* Hydra_Software_Devel/213   12/18/07 10:42a gdata
* PR32768: [7440] Adding the condition for the call to secondary metadata
* Correcting the position of calling mask interrupt for ACK of stop
* decode
* 
* Hydra_Software_Devel/212   12/17/07 1:30p sushmit
* PR 36084: [7405] Making bIndOpVolCtrl start time parameter common for
* all platforms.
* 
* Hydra_Software_Devel/211   12/17/07 12:27p sushmit
* PR 38063: [7405] Adding bLoopAroundEn flag for PCM Playback
* 
* Hydra_Software_Devel/210   12/16/07 7:14p sushmit
* PR 36084: [7405] Adding support for adding separate mixer per output
* port.
* 
* Hydra_Software_Devel/209   12/12/07 1:43p sushmit
* PR 36167: [7405] Fixing HDMI Volume Control when muxselector is Mai
* itself
* 
* Hydra_Software_Devel/208   12/11/07 6:13p gdata
* PR37996: [7440] Moving the BRAP_P_UnmaskInterrupt call at correct place
* 
* Hydra_Software_Devel/207   12/11/07 12:07p gdata
* PR37996: [7440] Moving the BRAP_P_UnmaskInterrupt call for
* BRAP_Interrupt_eFwAck from openchannel time to start channel time.
* 
* Hydra_Software_Devel/206   12/7/07 11:14a gdata
* PR37996: [7440] Adding changes to make firmware ack as interrupt driven
* 
* Hydra_Software_Devel/205   12/6/07 5:15p gdata
* PR37952 : [7440] Fixing the bug of incrementing associated channel
* during flush
* 
* Hydra_Software_Devel/204   12/4/07 8:25p gdata
* PR37880: [7440] Merging the Massproduction branch to main branch.
* 
* Hydra_Software_Devel/203   11/27/07 10:17a speter
* PR 37577: [3563] Merging branch Phase2.9 to main line
* 
* Hydra_Software_Devel/202   11/19/07 3:14p sushmit
* PR 36822: [7405] Making MAI take data straight from Mixer.
* 
* Hydra_Software_Devel/201   10/26/07 10:16p sushmit
* PR 36466: [7405] Disable & enable callback
* 
* Hydra_Software_Devel/RAP_3563_PH_2_9/8   11/23/07 11:12a speter
* PR 36948: [3563] Fixing the Capture Channel independent delay feature
* 
* Hydra_Software_Devel/RAP_3563_PH_2_9/7   11/21/07 2:13p speter
* PR 37278: [3563] Removing unused parameter.
* 
* Hydra_Software_Devel/RAP_3563_PH_2_9/6   11/21/07 9:26a speter
* PR 37112: [3563] Fixing the Custom Set PP for capture channel
* 
* Hydra_Software_Devel/RAP_3563_PH_2_9/5   11/19/07 2:47p speter
* PR 37278: [3563] Implementing multiple post processing branches so that
* the effects of one PP does not affect the other output ports
* 
* Hydra_Software_Devel/RAP_3563_PH_2_9/4   11/17/07 1:01p speter
* PR 37278: [3563] Implementing multiple post processing branches so that
* the effects of one PP does not affect the other output ports
* 
* Hydra_Software_Devel/RAP_3563_PH_2_9/3   11/16/07 10:04a speter
* PR 37112: [3563] Custom PP Set Config workaround code.
* 
* Hydra_Software_Devel/RAP_3563_PH_2_9/2   11/5/07 6:29p speter
* PR 34996: [3563] Fixing the comments for custom equalizer
* 
* Hydra_Software_Devel/RAP_3563_PH_2_9/1   11/5/07 5:31p speter
* PR 36083: [3563] Moving the Custom Mixer requirement code in a branch
* 
* Hydra_Software_Devel/200   10/25/07 1:40a gautamk
* PR36409: [3563] Fixing Coverity issue for 3563 RAP Code base
* 
* Hydra_Software_Devel/199   10/24/07 2:35p speter
* PR 36397: [3563] Fixing GetAudioParams API in SPDIF IN context
* 
* Hydra_Software_Devel/198   10/22/07 2:37p sushmit
* PR 36003: [7405] Fixing compilation errors due to recent 7440/3563
* checkins.
* 
* Hydra_Software_Devel/197   10/20/07 12:03p speter
* PR 36081: [3563] Adding new API to control the bit length for differnt
* outputs. Currently only SPDIF is supported
* 
* Hydra_Software_Devel/196   10/18/07 8:30p speter
* PR 36083:[3563] Fixing the Balance coefficient assignements
* 
* Hydra_Software_Devel/195   10/18/07 5:21p speter
* PR 36083: [3563] Fixing the API SetGain and Amplification to use the
* Mixer level in BRAP_P_GetMixerForOpPort. This also requied isolating
* BRAP_P_GetMixerForOpPort for 3563.
* 
* Hydra_Software_Devel/194   10/18/07 4:26p speter
* PR 36083: [3563] Fixing the 3 Mixer custom Requirement
* 
* Hydra_Software_Devel/193   10/18/07 2:14p speter
* PR 36083: [3563] Checking in the new Volume APIs related changes.
* 
* Hydra_Software_Devel/192   10/15/07 3:56p speter
* PR 36100: [3563] Merge the 2.8 Release branch to main line
* 
* Hydra_Software_Devel/RAP_3563_PH_2_8/2   10/15/07 2:25p speter
* PR 28188: [3563] Fixing the capture path zero (The capture path with ID
* zero is meant only for the capture part of SPDIF / HDMI / BTSC and
* I2S)
* 
* Hydra_Software_Devel/RAP_3563_PH_2_8/1   10/12/07 2:41p speter
* PR 33800: [3563] Issue because of simultaneous writes to SPDIF Rx
* Registers. Fixed by making them critical sections.
* 
* Hydra_Software_Devel/191   10/11/07 4:50p speter
* PR 28188: [3563] Support for Capturing to file requires API changes in
* Add and Remove destinations
* 
* Hydra_Software_Devel/190   10/11/07 4:30p sushmit
* PR 36003: [7405] Fixing Memory Leaks
* 
* Hydra_Software_Devel/189   10/11/07 11:56a sushmit
* PR 36003: [7405] Fixing Compilation Warnings
* 
* Hydra_Software_Devel/188   10/11/07 11:43a sushmit
* PR 34135: [7405] Adding APIs BRAP_GetTotalChannels &
* BRAP_GetSamplingRate
* 
* Hydra_Software_Devel/187   10/11/07 10:27a speter
* PR 34642: [3563] Support AAC+ in SPDIF In
* 
* Hydra_Software_Devel/186   10/10/07 4:17p speter
* PR 34642: [3563] Support AAC+ in SPDIF In
* 
* Hydra_Software_Devel/185   10/9/07 6:02p gdata
* PR 35751 : [7440] Exposing the control of enable/disable soft limit
* cofficients to application.
* 
* Hydra_Software_Devel/184   10/4/07 2:54p bhanus
* PR 35690:[7440] Removed code that returns error for on the fly change
* of channel status bits.
* 
* Hydra_Software_Devel/183   10/4/07 2:41p gautamk
* PR35688: [7405] Implementing FW authentication support (NDS support)
* for 7405 code base.
* 
* Hydra_Software_Devel/182   10/4/07 2:13p sushmit
* PR 35464: [7405] BRAP_DSP_P_Close memory leak fixed & some other
* changes for memory leak debugging
* 
* Hydra_Software_Devel/182   10/4/07 2:11p sushmit
* PR 35464: [7405] BRAP_DSP_P_Close memory leak fixed & some other
* changes for memory leak debugging
* 
* Hydra_Software_Devel/181   9/30/07 1:54p sushmit
* PR35140: [7405] Develop 7325 Raptor PI for Bringup
* 
* Hydra_Software_Devel/180   9/28/07 10:44a speter
* PR 32625: [3563] Move volume amplification states from output port
* handle to rap handle
* 
* Hydra_Software_Devel/179   9/24/07 4:22p gdata
* PR 32768: [7440] Correcting the default value for eInputSR
* 
* Hydra_Software_Devel/178   9/19/07 4:22p sushmit
* PR 34648: [7405] Temporary check so that BRAP_MIXER_P_SetOutputVolume
* doesn't get called for compressed data
* 
* Hydra_Software_Devel/177   9/19/07 3:31p speter
* PR 35074: [3563] Providing RAP PI support for PEQ post processing
* algorithm
* 
* Hydra_Software_Devel/176   9/19/07 2:14p sushmit
* PR 34648: [7405] Bypassing setting volume in Mixer for Mai output.
* 
* Hydra_Software_Devel/175   9/18/07 5:02p speter
* PR 34996: [3563] Adding support for Custom Mode Equalizer
* 
* Hydra_Software_Devel/174   9/18/07 3:09p sushmit
* PR 34860: [7405][3563] Fixing issue when pAudioParams->eInputSR is set
* to BAVC_AudioSamplingRate_eUnknown for STB/DTV App.
* 
* Hydra_Software_Devel/173   9/14/07 12:41p gdata
* PR 34823: [7440] Fixing BKNI memory leak
* 
* Hydra_Software_Devel/172   9/14/07 12:05p gdata
* PR 32768: [7440] Code modifications for Secondary Metadata
* 
* Hydra_Software_Devel/171   9/13/07 10:38p sushmit
* PR 34484 : [7405][7440][3563] Adding sCdbItbCfg for BE mode
* 
* Hydra_Software_Devel/170   9/7/07 3:43p speter
* PR 34673: [3563] Merging the Phase 2.6 branch to mainline
* 
* Hydra_Software_Devel/RAP_3563_PH_2_6/1   9/6/07 1:09p speter
* PR 34460: [3563] Changing code to use 'Custom' for specific Post
* procesing algorithms
* 
* Hydra_Software_Devel/1   1/22/08 4:46p sushmit
* PR34648: [7405] Merging in updated code from Base2
* 
* Hydra_Software_Devel/Rap_7405_NewFwIf/14   1/18/08 3:24p sushmit
* PR 34648: [7405] Getting Brutus compiled.
* 
* Hydra_Software_Devel/Rap_7405_NewFwIf/13   1/17/08 9:04p sushmit
* PR34648: [7405] AC3 Bringup Changes
* 
* Hydra_Software_Devel/Rap_7405_NewFwIf/12   1/17/08 5:08p gautamk
* PR34648: [7405] updating comment
* 
* Hydra_Software_Devel/Rap_7405_NewFwIf/11   1/17/08 2:51p sushmit
* PR 34648: [7405] More memory for CIT
* 
* Hydra_Software_Devel/Rap_7405_NewFwIf/10   1/16/08 4:48p nitinb
* PR34648: [7405] Added support for programming configuration parameters
* 
* Hydra_Software_Devel/Rap_7405_NewFwIf/9   1/11/08 12:14p sushmit
* PR34648: [7405] Adding hRap in hAssociation
* 
* Hydra_Software_Devel/Rap_7405_NewFwIf/8   1/10/08 6:46p gautamk
* PR34648: [7405] Files for Mapping Processing network to CIT input
* 
* Hydra_Software_Devel/Rap_7405_NewFwIf/7   1/7/08 4:48p sushmit
* PR34648: [7405] Updating Start
* 
* Hydra_Software_Devel/Rap_7405_NewFwIf/6   1/4/08 1:51p bhanus
* PR34648: [7405] Adding changes for Common Audio Processing network for
* CIT module.
* 
* Hydra_Software_Devel/Rap_7405_NewFwIf/5   1/3/08 6:33p nitinb
* PR 34648: [7405] Integration of FW-Interface module with other modules
* 
* Hydra_Software_Devel/Rap_7405_NewFwIf/4   12/20/07 2:32p srajapur
* PR 34648: [7405] Implemented APIs for  Processing stages.
* Following are the API's
* 1.BRAP_SetPostProcessingStages
* 2.BRAP_GetCurrentPostProcessingStages
* 3.BRAP_GetDefaultPostProcessingStages
* 
* Hydra_Software_Devel/Rap_7405_NewFwIf/3   10/31/07 1:42p nitinb
* PR 34648: [7405] Implemented review comments of new API interface as
* mentioned in PR progress-log (dated 10/31/2007)
* 
* Hydra_Software_Devel/Rap_7405_NewFwIf/2   10/25/07 5:18p nitinb
* PR 34648: [7405] Removed API BRAP_SetAudioProcessingStages and
* BRAP_GetAudioProcessingStages
* 
* Hydra_Software_Devel/Rap_7405_NewFwIf/1   10/3/07 4:48p gautamk
* PR34648: [7405] checkin updated files for new PI/FW arch
* 
* Hydra_Software_Devel/169   8/30/07 12:06p speter
* PR 34460: [3563] Correcting the check-in comments
* 
* Hydra_Software_Devel/168   8/27/07 3:10p speter
* PR 34376: [3563] RAP PI support for Custom Voice
* 
* Hydra_Software_Devel/167   8/27/07 1:32p sushmit
* PR 34135: [7405] Fixing break due to CpToggleRate Support
* 
* Hydra_Software_Devel/166   8/27/07 12:17p sushmit
* PR 34135: [7405] Updating AACHE support & merging in stack reduction
* fixes.
* 
* Hydra_Software_Devel/165   8/21/07 2:06p speter
* PR 34202: [3563] Provide Custom Bass post processing algorithm support
* 
* Hydra_Software_Devel/164   8/21/07 10:18a speter
* PR 34201: [3563] Merge the Phase 2.5 Release code to Hydra Software
* Devel (main line)
* 
* Hydra_Software_Devel/163   8/20/07 2:51p kagrawal
* PR 34154: [7440] Fixed compilation breaks
* 
* Hydra_Software_Devel/162   8/20/07 10:43a speter
* PR 34154: [3563] Fixing compilation break
* 
* Hydra_Software_Devel/161   8/19/07 8:34p kagrawal
* PR 34154: [7440] Added CP_TOGGLE_RATE programming support
* 
* Hydra_Software_Devel/REFSW_RAP_7405_PR34312/1   8/24/07 5:17p sushmit
* PR 34312: [7405] Reducing Stack Size
* 
* Hydra_Software_Devel/160   8/16/07 10:32p sushmit
* PR 34014: [7405] Reducing stack size
* 
* Hydra_Software_Devel/RAP_3563_PH_2_5/1   8/17/07 11:44a speter
* PR 30552: [3563] Modifying the check for LinkId in API DestroyLink
* 
* Hydra_Software_Devel/159   8/10/07 11:49a sushmit
* PR 30552: [3563] Fixing issues with Compressed->PCM->Compressed input
* swiching in SPDIF SIMUL mode.
* 
* Hydra_Software_Devel/158   8/9/07 3:03p sushmit
* PR 30552: [3563] Added Channel Link Create/Destroy APIs. Modified
* BRAP_ProcessDigitalInputFormatChange to take care of Simul mode for
* compressed input.
* 
* Hydra_Software_Devel/157   7/31/07 8:40p gautamk
* PR33580 : [7440] With Dual  decode , switiching to French Commentry in
* MI3 DVD, causes CORE Dump on DVD platform. using SpdifFm handle only if
* it is not NULL
* 
* Hydra_Software_Devel/156   7/30/07 5:40p speter
* PR 33608: [3563] Providing support for Custom Surround post processing
* algorithm
* 
* Hydra_Software_Devel/155   7/30/07 12:12p kagrawal
* PR 33477: [3563] Fixed compilation break due to last checkin
* 
* Hydra_Software_Devel/154   7/26/07 4:34p speter
* PR 33477: [3563] Merge the Phase 2.3 branch to main
* 
* Hydra_Software_Devel/152   7/16/07 6:30p bhanus
* PR 32986 : [7440 B0] Fixed the crash coming because of not copying the
* Encode channel handle for multiple channels in the association.
* 
* Hydra_Software_Devel/151   7/16/07 5:53p speter
* PR 33016: [3563] Adding check for proper hanlding of Algo Type Invalid
* in firmware download and SPDIF In.
* 
* Hydra_Software_Devel/150   7/13/07 2:46p bhanus
* PR 32986: [7440 B0] Fixed compilation warning
* 
* Hydra_Software_Devel/149   7/13/07 1:46p bhanus
* PR 32986: [7440 B0] Code modifed for Sequential path bring up.
* 
* Hydra_Software_Devel/148   7/12/07 7:12p gdata
* PR22490 : Merging the CRC code on main branch & removing the
* compilation error
* 
* Hydra_Software_Devel/147   7/12/07 6:46p gdata
* PR22490 : Merging the CRC code on main branch
* 
* Hydra_Software_Devel/146   7/11/07 12:12p kagrawal
* PR 32901: Fixing broken flush on decode channel
* 
* Hydra_Software_Devel/145   7/9/07 4:21p bhanus
* PR 32828 : [7440 B] Code modified for Bitstream HBR bring up.
* - Added condition to throw error if App tries to add any compressed
* output port along with HBR compressed.
* 
* Hydra_Software_Devel/144   7/9/07 2:13p sushmit
* PR 31453: Watchdog support for 3563
* 
* Hydra_Software_Devel/RAP_3563_PH_2_3/2   7/11/07 3:13p sushmit
* PR 28714: Initialize uiPPBranchId to zero on Opening of Destination
* 
* Hydra_Software_Devel/RAP_3563_PH_2_3/1   7/10/07 8:51p nitinb
* PR 32797: Memory Leak during creation of an Association of channels
* 
* Hydra_Software_Devel/143   7/6/07 4:30p sushmit
* PR 32735: Updation of CBIT programming by HOST/DSP
* 
* Hydra_Software_Devel/142   7/6/07 2:40p speter
* PR 32797: [3563] Memory leak in Create Association is fixed
* 
* Hydra_Software_Devel/141   7/5/07 4:10p sushmit
* PR 32735: Support on the fly change of SPDIF channel status bits from
* host through DSP.
* 
* Hydra_Software_Devel/140   7/3/07 2:35p bhanus
* PR 32731 : Added a check to return error if XCD and SM are added
* simultaneously.
* 
* Hydra_Software_Devel/139   6/27/07 8:34p gautamk
* PR32504: Providing  SPDIF0/1 C-bit  params to DSP1 for XCD.
* 
* Hydra_Software_Devel/138   6/27/07 7:40p kagrawal
* PR 32504: [7440] Setting iRightDelay to 0 default
* 
* Hydra_Software_Devel/137   6/22/07 8:16p gautamk
* PR30069 : Adding support for Transcode on SPDIF1
* 
* Hydra_Software_Devel/136   6/21/07 12:19p speter
* PR 27646: [7405] Avoiding Unified SR unknown check for 7405 (Similar to
* 3563)
* 
* Hydra_Software_Devel/135   6/19/07 10:02a kagrawal
* PR 30069: Reverted back volume init changes commited in the last check-
* in
* 
* Hydra_Software_Devel/134   6/18/07 5:27p kagrawal
* PR 30069: Setting unity volume gain as default for all output ports
* 
* Hydra_Software_Devel/133   6/18/07 3:05p sushmit
* PR 28188: Modified BRAP_P_CloseCapPath so that RBUFs & DSTCHs don't get
* closed if not allocated in OpenCapPath
* 
* Hydra_Software_Devel/132   6/18/07 1:44p sushmit
* PR 28188: Adding RBUF support to BRAP_RemoveDestination
* 
* Hydra_Software_Devel/131   6/15/07 5:06p kagrawal
* PR 30069: Fixing compilation breaks in 3563
* 
* Hydra_Software_Devel/130   6/15/07 2:25p kagrawal
* PR 30069: [7440] Merging Phase2.0 branch to Hydra_Software_Devel
* 
* Hydra_Software_Devel/RAP_7440_Phase2.0_Branch/4   6/14/07 1:28p bhanus
* PR 30069 : [7440 B0] Added code to support:
* - HBR path
* - HBR configuration of I2sMulti_1 and Mai
* - Passing HBR flag to DSP1
* 
* Hydra_Software_Devel/RAP_7440_Phase2.0_Branch/3   6/12/07 5:51p kagrawal
* PR 30069: [7440] Added support for independent channel delay for DSP1
* audio processing
* 
* Hydra_Software_Devel/RAP_7440_Phase2.0_Branch/2   6/9/07 5:30p kagrawal
* PR 30069: [7440] Added support for DTS-Neo PostProcessing
* 
* Hydra_Software_Devel/RAP_7440_Phase2.0_Branch/1   6/7/07 2:21p kagrawal
* PR 30069: Added support for branches and stages for DSP1 audio post
* processing
*
* Hydra_Software_Devel/129   6/14/07 7:26p gautamk
* PR30069: Changes for new algorithm Mpeg MC
* 
* Hydra_Software_Devel/128   6/14/07 4:49p gautamk
* PR30069: Adding Algorithm DDP 7.1
* 
* Hydra_Software_Devel/127   6/14/07 3:54p bhanus
* PR 30069 : [7440 B0] Added support for playing unsigned content for PCM
* PB channel.
* 
* Hydra_Software_Devel/126   6/13/07 2:45p nitinb
* PR 30789: Support MPEG2 LSF audio decoding
* 
* Hydra_Software_Devel/125   6/12/07 12:15p sushmit
* PR 27646: Minor Checkins for 7405
* 
* Hydra_Software_Devel/124   6/12/07 11:46a sushmit
* PR 27646: Fixing Compilation Break for 7405 after 3563 Volume/Mute
* State checkin
* 
* Hydra_Software_Devel/123   6/11/07 9:01p nitinb
* PR 29166: Moved volume states from output handle to RAP handle. This
* removes restriction of calling add destination API before calling
* volume APIs.
* 
* Hydra_Software_Devel/122   6/11/07 6:58p nitinb
* PR 29166: Moved output port mute states from output handle to RAP
* handle. This removes restriction of calling add destination API before
* calling mute API.
* 
* Hydra_Software_Devel/121   6/8/07 3:18p bhanus
* PR 30069 : [7440 B0] Added code for HDMI HBR Compressed Support.
* 
* Hydra_Software_Devel/120   6/7/07 7:25p bhanus
* PR 30069 : [7440B0] Added code for Secondary Metadata Processing.
* 
* Hydra_Software_Devel/119   6/5/07 10:30a speter
* PR 31827: [3563] Merging from Phase 2.0 release branch to main
* 
* Hydra_Software_Devel/RAP_3563_PH_2_0/2   5/29/07 4:56p nitinb
* PR 30625: [3563] Added check to return error if post processing is
* enabled for passthru
* 
* Hydra_Software_Devel/RAP_3563_PH_2_0/1   5/29/07 1:45p sushmit
* PR28719: Adding code for independent delay & fixing MULPP broken
* 
* Hydra_Software_Devel/118   6/4/07 2:05p bhanus
* PR 30069 :[7440 B0] Added Support for SPDIF1 and I2SMulti0 as Mux
* Select for HDMI
* 
* Hydra_Software_Devel/117   6/1/07 5:10p sushmit
* PR 30069: XCD Flush.
* 
* Hydra_Software_Devel/116   6/1/07 2:56p sushmit
* PR 30069: XCD Watchdog Recovery Fixed.
* 
* Hydra_Software_Devel/115   5/31/07 3:46p sushmit
* PR 30069: XCD DEC & PB Start-Stop-Start
* 
* Hydra_Software_Devel/114   5/30/07 3:45p sushmit
* PR 30069: Fixing memory leak.
* 
* Hydra_Software_Devel/113   5/28/07 1:34p bhanus
* PR 30069 : Fixed Coverity Issues.
* 
* Hydra_Software_Devel/112   5/26/07 8:58p sushmit
* PR 30069: Fixing DVD PR 3573
* 
* Hydra_Software_Devel/111   5/25/07 6:22p kagrawal
* PR 30069: Added first-cut PB watchdog support
* 
* Hydra_Software_Devel/110   5/24/07 6:20p kagrawal
* PR 30069: [7440 B0] Added support for stereo output on multi-I2s1
* 
* Hydra_Software_Devel/109   5/23/07 5:50p nitinb
* PR 31498: Merge RAP PI development from phase 1.7 branch to main branch
* 
* Hydra_Software_Devel/RAP_3563_PH_1_7/3   5/15/07 3:14p speter
* PR 30168: [3563] Added Support for I2S0 of multi I2S(0-3)
* 
* Hydra_Software_Devel/RAP_3563_PH_1_7/2   5/15/07 3:52p nitinb
* PR 30748: Support user config parameters for capture channel
* 
* Hydra_Software_Devel/RAP_3563_PH_1_7/1   5/15/07 3:24p nitinb
* PR 30747: Return errors from RAP APIs that are not supported when
* channel is not started
* 
* Hydra_Software_Devel/108   5/11/07 8:57p sushmit
* PR 27646: Getting 7405 PI compiled
* 
* Hydra_Software_Devel/107   5/11/07 12:30p kagrawal
* PR 30069: Added code for SPDIF1
* 
* Hydra_Software_Devel/106   5/10/07 2:43p gautamk
* PR30069: Changes for Any to Any resource linking
* 
* Hydra_Software_Devel/105   5/8/07 4:13p bhanus
* PR 30069: Added a check to return error if application tries to remove
* destination while at least one channel is running in the Association.
* 
* Hydra_Software_Devel/104   5/7/07 4:31p nitinb
* PR 28862: [3563] Added support for post processing algorithm BBE
* 
* Hydra_Software_Devel/103   5/7/07 12:05p bhanus
* PR 30069 : Fixed the compilation break for 3563 introduced due to last
* check-in
* 
* Hydra_Software_Devel/102   5/6/07 6:42p bhanus
* PR 30069 : [7440 B0] Added code for :
* - Unified Sampling Rate
* - Copy path for Sequential Path
* - Enabling PCM playback on Sequential Path
* 
* Hydra_Software_Devel/101   5/3/07 5:04p gautamk
* PR26996: Exposing RAP PI interface to access toSet/Get RAMP Step size
* for mixer vol control.
* 
* Hydra_Software_Devel/100   5/3/07 4:57p speter
* PR 28716: [3563] SpdifRx, Input Spdif from Comp1->Comp2 switching
* support added. Major code revamp done in SpdifRx Module.
* 
* Hydra_Software_Devel/99   5/3/07 4:42p speter
* PR 30400: [3563] Merge happened from branch RAP_3563_PH_1_6 to
* Hydra_Software_Devel. After that it was found that 7440 Pvr check-ins
* have broken the build. Fixing the same.
* 
* Hydra_Software_Devel/98   5/3/07 4:23p speter
* PR 30400: [3563] Merged the changes done as part of Phase 1.6 Release
* to the main line. Merge happened from branch RAP_3563_PH_1_6 to
* Hydra_Software_Devel
* 
* Hydra_Software_Devel/97   5/3/07 2:56p kagrawal
* PR 30069: Added support to open output ports and spdif FM during
* watchdog recovery
* 
* Hydra_Software_Devel/96   5/2/07 12:17p kagrawal
* PR 30069: Adding initial support for Watchdog Recovery
* 
* Hydra_Software_Devel/95   4/28/07 6:19p sushmit
* PR 28561: Fixing Start-Stop-Start of XCD
* 
* Hydra_Software_Devel/94   4/26/07 7:23p bhanus
* PR 30069 : Code Modified for sequential path bring up
* 
* Hydra_Software_Devel/93   4/25/07 7:09p nitinb
* PR 28177: Corrected API BRAP_GetDefaultOutputConfig to return correct
* values of bUseSpdifPackedChanStatusBits and sSpdifPackedChanStatusBits
* fields
* 
* Hydra_Software_Devel/92   4/25/07 12:29p nitinb
* PR 28187: Added new API BRAP_SetOutputVolumeGain
* 
* Hydra_Software_Devel/91   4/24/07 5:29p nitinb
* PR 28177: Second argument of BRAP_SPDIFFM_P_ProgramCBITBuffer_isr()
* expects C-bit values in lower 16 bits of array of uint32_t elements
* 
* Hydra_Software_Devel/90   4/19/07 12:03p kagrawal
* PR 28561: [3563] Fixing compilation break due to the last check-in
* 
* Hydra_Software_Devel/89   4/16/07 9:47a kagrawal
* PR 28561: Reduced channel handle size by changing sPath[] to *pPath[]
* 
* Hydra_Software_Devel/88   4/13/07 2:33p sushmit
* PR 28561: Fixing AddDestination for HDMI for transcoded output
* 
* Hydra_Software_Devel/87   4/10/07 11:45a nitinb
* PR 29381: Malloc/Free large size structures instead of allocating on
* stack to prevent stack overflow
* 
* Hydra_Software_Devel/86   4/9/07 8:27p sushmit
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
* Hydra_Software_Devel/85   4/9/07 2:27p speter
* PR 28710: Modifying the comments for SetEqualizerParams as the step
* size has been increased to 200
* 
* Hydra_Software_Devel/84   4/5/07 4:34p speter
* PR 28931: [3563] Adding DTS Passthru support in SPDIF/HDMI In
* 
* Hydra_Software_Devel/83   4/3/07 1:46p sushmit
* PR 28714: Multiple PP branch support
* 
* Hydra_Software_Devel/82   3/30/07 6:14p kagrawal
* PR 28561: [7440] Enabled BRAP_SetAudioProcessingConfig() and
* BRAP_GetAudioProcessingConfig() APIs
* 
* Hydra_Software_Devel/81   3/28/07 5:06p nitinb
* PR 28177: [3563/7440] Modified API BRAP_SetOutputConfig() to enable on-
* the-fly setting of SPDIF channel status bits packed into two 32-bit
* words
* 
* Hydra_Software_Devel/80   3/27/07 11:58a speter
* PR 28718: Adding Mute API for SPDIF Rx through SetInputMute
* 
* Hydra_Software_Devel/79   3/23/07 8:34p bhanus
* PR 28955 : [7440A0/3563] Modified the code for adding on-the-fly
* configuration for I2S output ports.
* 
* Hydra_Software_Devel/78   3/22/07 10:49a kagrawal
* PR 28953: Calling BRAP_P_ReturnRBufToPool() in BRAP_RemoveDestination()
* only in case of ring buffer as a destination
* 
* Hydra_Software_Devel/77   3/16/07 5:05p gautamk
* PR27912 : Implementaion of Open time memory allocation for Ring Buffers
* 
* Hydra_Software_Devel/76   3/14/07 3:49p kagrawal
* PR 28561: Moved channel handle malloc from BRAP_XYZ_P_OpenChannel() to
* the common BRAP_OpenChannel()
* 
* Hydra_Software_Devel/75   3/14/07 1:17p kagrawal
* PR 28561: [3563] Fixed compilation break
* 
* Hydra_Software_Devel/74   3/14/07 12:26p kagrawal
* PR 28561: [7440, 3563] Restoring 7440. Changes in config params s per
* the new MIT. [Checking in on Nitin's behalf]
* 
* Hydra_Software_Devel/73   3/13/07 11:49a sushmit
* PR 28561: Fixed I2S IN Noise issue. Changed default params of alignment
* to LRCLK to be delayed.
* 
* Hydra_Software_Devel/72   3/12/07 3:34p speter
* PR 27999: Adding code to fix the spurious interrupts in SPDIF Rx.
* Adding a separate function that will read the status after the stream
* stabilizes
* 
* Hydra_Software_Devel/71   3/10/07 12:15p speter
* PR27999: Adding code for SPDIF Compressed In (Ac3) handling
* 
* Hydra_Software_Devel/70   3/8/07 6:09p nitinb
* PR 26864: Changes to support configuration parameters for post
* processing algorithms
* 
* Hydra_Software_Devel/69   3/8/07 1:58p speter
* PR 27999: Fixing Equalizer / Tone Control I2S issue. Using
* BRAP_OutputPort_eI2s4 instead of BRAP_OutputPort_eI2s0
* 
* Hydra_Software_Devel/68   3/7/07 11:14a kagrawal
* PR 27912: Checking in SetGain/GetGain code [on behalf of Bhanu]
* 
* Hydra_Software_Devel/67   3/6/07 7:08p gautamk
* PR27912 : Adding Interface for Open time Memory allocation for RBuf
* 
* Hydra_Software_Devel/66   3/6/07 12:46p sushmit
* PR 27912: Updating AudioProcessingConfig based XCD
* 
* Hydra_Software_Devel/65   3/2/07 11:31a kagrawal
* PR 27912: Added PI support for Speaker Management
* 
* Hydra_Software_Devel/64   3/1/07 10:07a kagrawal
* PR 27912: New MIT changes for 7440
* 
* Hydra_Software_Devel/63   2/27/07 9:32p kagrawal
* PR 27912: Fixed compilation error for 3563
* 
* Hydra_Software_Devel/62   2/27/07 9:01p kagrawal
* PR 27912: Modifications to take Audio Processing config params
* 
* Hydra_Software_Devel/61   2/27/07 6:27p nitinb
* PR 26864: Support post processing algorithms like SRS, AVL and PL-II in
* 3563 RAP PI
* 
* Hydra_Software_Devel/60   2/22/07 12:17p bhanus
* PR 28040 : Modified code for 7440 B0 porting
* 
* Hydra_Software_Devel/59   2/21/07 2:35p speter
* PR 27999: Uncommenting the ungetpaths function so that stop and
* subsequent start channel works
* 
* Hydra_Software_Devel/58   2/20/07 12:56p speter
* PR 27999: Fixed the SPDIF Channel Status read by enabling HOLD while
* reading
* 
* Hydra_Software_Devel/57   2/9/07 6:19p bhanus
* PR 25607 : Fixed Compilation warnings
* 
* Hydra_Software_Devel/56   2/9/07 4:42p kagrawal
* PR 25607: Removed deprecated eAudioSource from BRAP_ChannelSettings
* structure
* 
* Hydra_Software_Devel/55   2/8/07 11:08a bhanus
* PR 25607 : Added code for programming user provided mixing
* coefficients.
* 
* Hydra_Software_Devel/54   2/6/07 1:15p kagrawal
* PR 25607: Misc fixes for supporting dual decode
* 
* Hydra_Software_Devel/53   2/5/07 1:08p bhanus
* PR 25607: Modified code to call BRAP_P_CloseCapPath() and
* BRAP_P_StopCapPath()
* 
* Hydra_Software_Devel/52   1/30/07 2:28p bhanus
* PR 25607: Added a hack to modify hRap->sSettings.bSupportDecAlgos for
* DTS/DTS-HD
* 
* Hydra_Software_Devel/51   1/22/07 7:26p sushmit
* PR 26864: SPDIF RX bringup, fix in input format change handler.
* Addition of cap port params etc.
* 
* Hydra_Software_Devel/50   1/13/07 10:34a sushmit
* PR 26864: Fixing ext cap input port checking error
* 
* Hydra_Software_Devel/49   1/12/07 8:27p sushmit
* PR 26864:
* PP Related changes for 3563
* 
* Hydra_Software_Devel/48   1/12/07 10:32a kagrawal
* PR 25607: Added BRAP_P_IsDstAlreadyPresent() to check if an output port
* is already added to an association
* 
* Hydra_Software_Devel/47   1/11/07 11:55a kagrawal
* PR 27043: - Fixed remove destination when an association holds 2 or
* more channels
* - Code clean up
* 
* Hydra_Software_Devel/46   1/10/07 2:24p speter
* PR 26864: [7440] Fix compilations for PP related changes.
* 
* Hydra_Software_Devel/45   1/9/07 6:20p bselva
* PR 26864:modified BRAP_Settings sDefSettings to add the default PPs
* supported
* 
* Hydra_Software_Devel/44   1/5/07 10:48a kagrawal
* PR 25607: Distributing the destinations to each of the sPaths - added
* BRAP_P_DistributeDest()
* 
* Hydra_Software_Devel/43   1/3/07 4:24p speter
* PR 25346: [7440/3563] - Removing the bCompressed field and instead
* using ePcm as algo ID
* 
* Hydra_Software_Devel/42   12/29/06 1:48a nitinb
* PR 26766: Fixed compilation errors
* 
* Hydra_Software_Devel/41   12/29/06 1:08a nitinb
* PR 26766: 1. Updated function BRAP_SetInputConfig for 3563
* 2. Initializing BTSC device in function BRAP_Open
* 3. Updated functions BRAP_InstallDeviceLevelAppInterruptCb
*   and BRAP_RemoveDeviceLevelAppInterruptCb to support BTSC interrupt
* 
* Hydra_Software_Devel/40   12/28/06 2:27p speter
* PR 25346: [3563/7440] SetInputConfig condition modified to chack
* Externa I2S capture only allowed for 7440 and External I2S, SPDIF,
* HDMI, BTSC for 3563
* 
* Hydra_Software_Devel/39   12/28/06 1:33p speter
* PR 25346: [3563] SPDIF/HDMI In Feature - DSP source related changes
* 
* Hydra_Software_Devel/38   12/28/06 12:36p kagrawal
* PR 25607: Added Flush API
* 
* Hydra_Software_Devel/37   12/27/06 11:04a speter
* PR 25346: [3563] SPDIF/HDMI In Feature - Adding check for capture
* channel not to take SPDIF/HDMI as input
* 
* Hydra_Software_Devel/36   12/26/06 4:40p kagrawal
* PR 26804: Fix for adding-removing HDMI (or any outport) a number of
* times : Closing SpdifFm associated to HDMI
* 
* Hydra_Software_Devel/35   12/21/06 10:30a kagrawal
* PR 25607: - Fixed segment faults due to pDstDetails
* - Added support for adding and removing destinations in any sequence
* - Added support for using any valid destination after a series of calls
* to add-remove destinations
* 
* Hydra_Software_Devel/34   12/19/06 6:24p bselva
* PR 25346: Checking in the changes for 3563
* 
* Hydra_Software_Devel/33   12/19/06 5:49p bselva
* PR 25346: Checking in the changes for 3563 new features
* 
* Hydra_Software_Devel/32   12/18/06 10:48a sushmit
* PR 25607: Adding XCDParams
* 
* Hydra_Software_Devel/31   12/15/06 2:24p kagrawal
* PR 25607: Added proper support for HDMI output port
* 
* Hydra_Software_Devel/30   12/13/06 4:13p speter
* PR 25346: [3563] SPDIF/HDMI In Feature
* 
* Hydra_Software_Devel/29   12/12/06 5:58p sushmit
* PR 25607: Fixing issues with pDstDetails Implementation
* 
* Hydra_Software_Devel/28   12/12/06 2:48p sushmit
* PR 25607: Finishing XCD TODOs
* 
* Hydra_Software_Devel/27   12/11/06 2:17p kagrawal
* PR 25607: Added sDstDetails to the associated channel handle. Removed
* it from hRapCh.
* 
* Hydra_Software_Devel/26   12/7/06 5:07p kagrawal
* PR 25607: Adding eAudioSource to BRAP_ChannelParams. This will be
* depricated from BRAP_ChannelSettings in the next release.
* 
* Hydra_Software_Devel/25   12/7/06 1:27p sushmit
* PR 25346: Merging 3563 to Hydra
* 
* Hydra_Software_Devel/24   12/7/06 10:58a sushmit
* PR 25607: Updated Enc AddDestination
* 
* Hydra_Software_Devel/23   12/4/06 1:51p kagrawal
* PR 25607: Bug fixes for stereo PCM over HDMI output port
* 
* Hydra_Software_Devel/22   12/1/06 7:16p sushmit
* PR 25607: Updated Encoder Params
* 
* Hydra_Software_Devel/21   11/27/06 2:00p sushmit
* PR 25607: Added code for CAP, XCD & ChSubTypeNone
* 
* Hydra_Software_Devel/20   11/27/06 12:12p sushmit
* PR 25607: Added code for CAP, XCD & ChSubTypeNone
* 
* Hydra_Software_Devel/19   11/22/06 7:20p kagrawal
* PR 25607: Added eInputAudMode and bInputLfePresent in the start param
* for decode channel
* 
* Hydra_Software_Devel/18   11/17/06 5:17p bhanus
* PR 25607 : Code modified for Mixing bringup
* 
* Hydra_Software_Devel/17   11/16/06 12:22p sushmit
* PR 25607: Encode/Transcode Additions.
* 
* Hydra_Software_Devel/16   11/15/06 11:59a bhanus
* PR 24717: Modified channel params structure
* - Removed sPbChParams
* -Added sBufferParams.
* 
* Hydra_Software_Devel/15   11/9/06 6:41p bhanus
* PR 24717 : Replaced BTMR_TimerHandle with BTMR_Handle to fix
* compilation warning.
* 
* Hydra_Software_Devel/14   11/9/06 11:36a bhanus
* PR 24717 : Added Support for PCM Playback.
* - Added Remove destination code.
* 
* Hydra_Software_Devel/13   10/31/06 3:04p kagrawal
* PR 24717: Code clean-up and added some checks
* 
* Hydra_Software_Devel/12   10/31/06 10:50a kagrawal
* PR 24717: Setting eChType in BRAP_GetCurrentChannelConfig
* 
* Hydra_Software_Devel/11   10/16/06 8:49p sushmit
* PR 24717: Updating Transcode related PI
* 
* Hydra_Software_Devel/11   10/16/06 3:01p sushmit
* PR 24717: Updating Transcode related PI
* 
* Hydra_Software_Devel/10   10/6/06 5:51p kagrawal
* PR 24717: Merged 7440 devel branch to Hydra_Software_Devel
* 
* Hydra_Software_Devel/9   9/21/06 3:14p kagrawal
* PR 20654: Magnum compliant nomenclature. Replaced
* - BRAP_ChannelOpen by BRAP_OpenChannel
* - BRAP_ChannelClose by BRAP_CloseChannel
* - BRAP_ChannelStart by BRAP_StartChannel
* - BRAP_ChannelStop by BRAP_StopChannel
* 
* Hydra_Software_Devel/8   9/19/06 6:07p kagrawal
* PR 20654: Added channel change support.
* Added BRAP_ChannelStop() function and setting channel state correctly.
* 
* Hydra_Software_Devel/7   9/18/06 3:48p kagrawal
* PR 20654: Fixed compilation warnings
* 
* Hydra_Software_Devel/6   9/18/06 12:59p kagrawal
* PR 20654: Added interrupt support
* 
* Hydra_Software_Devel/5   9/13/06 4:43p kagrawal
* PR 20654: Added BRAP_GetBufferConfig() API to get CDB/ITB buffer
* configuration
* 
* Hydra_Software_Devel/4   9/11/06 3:00p kagrawal
* PR 20654: Removed redundant debug msg
* 
* Hydra_Software_Devel/3   9/7/06 4:35p kagrawal
* PR 20654: Modifications for bring up
* 
* Hydra_Software_Devel/2   8/29/06 4:30p kagrawal
* PR 20654: Modified for bringup
* 
* Hydra_Software_Devel/1   8/17/06 12:05p speter
* PR 20654: Added from BLR_RAP_7440A0_Bring_Up_Branch
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/22   8/17/06 11:40a kagrawal
* PR 20654: Fixed errors found during emulation
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/21   8/10/06 3:50p kagrawal
* PR 20654: Resolved linking error
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/20   8/8/06 11:41a kagrawal
* PR 20654: Implemented interface review comments
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/19   8/7/06 1:36p kagrawal
* PR 20654: Added support for decode passthru and simul mode
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/18   8/4/06 7:11p bhanus
* PR 20654: Added BRAP_GetGain() and BRAP_GetPan()
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/17   8/2/06 9:15p kagrawal
* PR 20654: Fixed compilation error
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/16   8/1/06 3:00p kagrawal
* PR 20654: Added misc codes:
* - Linking Output Port with Mixers
* - Redesigned AddDestination
* - Corrected RM resource grant and request structure for output port
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/15   7/28/06 10:51p kagrawal
* PR 20654: Added and modifiled StartFmmModules()
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/14   7/28/06 3:31p bhanus
* PR 20654 : Modified BRAP_AddDestination() code.
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/13   7/28/06 10:37a bhanus
* PR 20654 : Added code for BRAP_SetGain() and BRAP_SetPan()
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/12   7/26/06 10:01p kagrawal
* PR 20654: Added BRAP_ChannelStart() and BRAP_XXXChannelConfig() APIs
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/11   7/21/06 11:34a bhanus
* PR 20654: Modified code for  BRAP_SetOutputVolume and
* BRAP_GetOutputVolume.
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/10   7/19/06 5:48p bhanus
* PR 20654: Added interfaces for Volume and Mute control.
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/9   7/19/06 5:05a kagrawal
* PR 20654: Added BRAP_ChannelClose()
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/8   7/19/06 4:35a kagrawal
* PR 20654: Added BRAP_DEC_P_ChannelOpen()  and GetDefaultChannelParam()
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/7   7/19/06 1:01a kagrawal
* PR 20654: Added code for SetOutputConfig, etc.
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/6   7/18/06 2:51p bhanus
* PR 20654: Added code for Add_Destination
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/5   7/7/06 6:37p bhanus
* PR 20654 : Added code for AddOutputPort.
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/4   6/30/06 1:10p speter
* For common interface initial coding
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/3   6/19/06 1:55p kagrawal
* PR 20654: Added more APIs
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/2   6/15/06 9:30a kagrawal
* PR 20654: Modified BRAP_DissociateChannels()
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/1   5/19/06 11:50a kagrawal
* PR 20654: Initial version
* 
***************************************************************************/
#include "brap.h"		/* API function and data structures declarations */

/* Internal module includes */
#include "brap_priv.h"	
#include "brap_cit_priv.h"
#ifdef RAP_GFX_SUPPORT
#include "brap_gfx_priv.h"
#endif


#define DEBUG_RBUF_READ 0
#if (DEBUG_RBUF_READ == 1)
#include <stdio.h>          
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#endif

#if BCHP_PWR_SUPPORT
#include "bchp_pwr.h"
#endif
#ifdef RAP_SCM_SUPPORT
#include "brap_scm.h"
#include "brap_scm_priv.h"
#include "brap_fwdwnld_priv.h"
#endif

BDBG_MODULE(rap);		/* Register software module with debug interface */

/*{{{ Local Defines */

#define BRAP_P_DEF_DEV_MEM_SIZE (600 * 1024) /* Defdev memory size 600 KB */

/*}}}*/

/*{{{ Local Typedefs */

/*}}}*/


/*{{{ Static Variables & Function prototypes */

/* Default BRAP settings definition */
static const BRAP_Settings sDefSettings = 
{
    false,                      /* bFwAuthEnable*/
	false,						/* bOpenTimeFwDownload */
	true,						/* External device memory allocation */ 	
	0x0,						/* Device mem addr base */
	BRAP_P_DEF_DEV_MEM_SIZE,	/* 600 KB device memory */	
    {/* sDeviceRBufPool */
        0,                      /* uiNumPostMixingInputBuffers */
        0,                      /* uiNumPostMixingOutputBuffers */
        {0, 0, 0, 0},           /* iMaxDelay[] */
        {0, 0, 0, 0},           /* uiMaxNumRBuf[] */
        {                       /* sDstBufSettings[] */
            {NULL, NULL, 0, 0},
            {NULL, NULL, 0, 0},
            {NULL, NULL, 0, 0},
            {NULL, NULL, 0, 0}            
        },
        false,
    },
	NULL, 						        /* The Image Interface function pointers */
	NULL,						        /* The Image Interface Context */
    false,                              /* bSoftLimit */
    false                               /* bIndOpVolCtrl */
#ifdef RAP_SCM_SUPPORT    
	,{/* sScmSettings */
		false,							/* bSCMEnabled */
		0,								/* ui32CaSystemId */
		{0, 0, 0, 0, 0},				/* ui32RefHashDigest */
		NULL							/* pfCallbackOtpProgramming */
	 }

#endif
  
};

/* Default BRAP_ChannelSettings definition */
static const BRAP_ChannelSettings sDefChannelSettings = 
{
	BRAP_ChannelType_eDecode,		/* Decode Channel type */
    BRAP_ChannelSubType_eNone,      /* None channel sub type */
    {                               /* sChnRBufPool */
        {1, 0},               /* uiMaxNumOutChPairs[] */
        {0, 0},                     /* uiMaxNumInChPairs[] */
        0,                          /* uiMaxNumRBuf */
        {NULL, NULL, 0, 0}          /* sBufSettings */
    }
    ,false
    ,false
#ifdef RAP_REALVIDEO_SUPPORT                                                                    
    ,{
        BRAP_INVALID_VALUE  /*uiTbd*/
    }
#endif
};

#if BRAP_P_EQUALIZER
static const BRAP_EqualizerSettings sDefEqualizerSettings = 
{
    false,
    {0,0},
    true,
    {0,0,0,0,0},
    false,
    {
        7,
        {1000,1000,1000,1000,1000,1000,1000},
        {0,0,0,0,0,0,0},
        {100,100,100,100,100,100,100}
    },
    false,
    {
        8,
        {0x4000000,0x4000000,0x4000000,0x4000000,
         0x4000000,0x4000000,0x4000000,0x4000000},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0}
    },
    false,
    0,
    false,
    315   
};
static const BRAP_EqualizerToneControlSettings sDefEqualizerToneControlSettings = 
{
	0,
    0
};
static const BRAP_Equalizer5BandSettings sDefEqualizer5BandSettings = 
{
	0,
	0,
	0,
	0,
	0
};
static const BRAP_Equalizer7BandSettings sDefEqualizer7BandSettings = 
{
    4,
    {1000,1000,1000,1000,1000,1000,1000},
    {0,0,0,0,0,0,0},
    {100,100,100,100,100,100,100}
};
static const BRAP_EqualizerCustomSettings sDefEqualizerCustomSettings = 
{
    8,
    {0x4000000,0x4000000,0x4000000,0x4000000,
     0x4000000,0x4000000,0x4000000,0x4000000},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0}
};
#endif

#if (BRAP_3548_FAMILY ==1)
static const BRAP_SpdifRx_InputDetectionSettings sDefSpdifRxInputDetectionSettings =
{
    BRAP_CapInputPort_eSpdif
};
#endif

#if (BRAP_7550_FAMILY !=1)
static const BRAP_CAPPORT_I2sInParams defI2sCapPortParams =
{
    false,                      /* sInputI2sParams.bLsbAtLRClk */
    false,                       /* sInputI2sParams.bAlignedToLRClk */
    false                       /* sInputI2sParams.bLRClkPolarity */
};

static const BRAP_SpdifRxCapParams defSpdifCapPortParams =
{
    BAUD_SPDIF_RX_InsertMode_eNoInsert,
    true,
    true,
    true
};

static const BRAP_HdmiRxCapParams defHdmiCapPortParams =
{
    BAUD_SPDIF_RX_eHdmi0,
    BAUD_SPDIF_RX_InsertMode_eNoInsert,
    true,
    true,
    true
};

static const BRAP_RfAudioCapParams defRfAudioCapPortParams = 
{
    BRAP_RfAudioBtscOutputMode_eMono,
    BRAP_RfAudioEiajOutputMode_eMain,
    BRAP_RfAudioKoreaA2OutputMode_eMain,
    BRAP_RfAudioNicamOutputMode_eFmAmMono,
    BRAP_RfAudioPalA2OutputMode_eMain,
    {
        false,
        false,
        0x1000,
        0x1300,
    },
    {
        false,
        0x16b5,
        0x2000,
    },
    false,
    BRAP_RfAudioStandard_eBtsc
};

static const BRAP_AdcCapParams defAdcCapPortParams =
{
    0                           /* sAdcParams.uiInputSelect */
};
#endif

extern BRAP_FWIF_P_MapProcessingEnum BRAP_sMapProcessingEnum [];

/* Static array defining CDB and ITB sizes for various 
   Buffer Configuration Modes */
#if (BSTD_CPU_ENDIAN==BSTD_ENDIAN_BIG)
static const BAVC_CdbItbConfig sCdbItbCfg[BRAP_BufferCfgMode_eMaxMode] =
{
	/* BRAP_BufferCfgMode_eStdAudioMode */
	{ { 128 * 1024, 4, false }, { 64 * 1024, 4, false }, false },	
	/* BRAP_BufferCfgMode_eAdvAudioMode */
	{ { 256 * 1024, 4, false }, { 128 * 1024, 4, false }, false },	
	/* BRAP_BufferCfgMode_eIpStbMode */
	{ { 256 * 1024, 4, false }, { 256 * 1024, 4, false } , false},  
	/* BRAP_BufferCfgMode_eHdDvdMode */
	{ { 32 * 1024, 4, false }, { 32 * 1024, 4, false } , false},	
	/* BRAP_BufferCfgMode_eBlueRayMode */
	{ { 32 * 1024, 4, false }, { 32 * 1024, 4, false } , false}	    
	/* TODO: Pupulate this array as more new modes gets added */
};
#else
static const BAVC_CdbItbConfig sCdbItbCfg[BRAP_BufferCfgMode_eMaxMode] =
{
	/* BRAP_BufferCfgMode_eStdAudioMode */
	{ { 128 * 1024, 4, true }, { 64 * 1024, 4, false }, false },	
	/* BRAP_BufferCfgMode_eAdvAudioMode */
	{ { 256 * 1024, 4, true }, { 128 * 1024, 4, false }, false },	
	/* BRAP_BufferCfgMode_eIpStbMode */
	{ { 256 * 1024, 4, true }, { 256 * 1024, 4, false } , false},  
	/* BRAP_BufferCfgMode_eHdDvdMode */
	{ { 32 * 1024, 4, true }, { 32 * 1024, 4, false } , false},	
	/* BRAP_BufferCfgMode_eBlueRayMode */
	{ { 32 * 1024, 4, true }, { 32 * 1024, 4, false } , false}	    
	/* TODO: Pupulate this array as more new modes gets added */
};
#endif

/***************************************************************************
Summary:
    This private routine forms the output resource request, allocates them 
    and open the allocated outputs and spdiffm (if any).
****************************************************************************/
static BERR_Code 
BRAP_P_AllocateAndOpenOpPort(
    BRAP_ChannelHandle      hRapCh,         /* [in] Channel handle */
    BRAP_DstDetails         *pDstDetails    /* [in] Details of the destination 
                                              to be added to the channel group*/ 
    );

/***************************************************************************
Summary:
    This private routine Closes the requested outputs and spdiffm (if any)
and de-allocates them.
****************************************************************************/
static BERR_Code 
BRAP_P_CloseAndFreeOpPort(
    BRAP_ChannelHandle      hRapCh,         /* [in] Channel handle */
    BRAP_RemoveDstDetails   *pDstDetails    /* [in] Details of the destination 
                                              to be removed */ 
    );

#if (BRAP_7550_FAMILY !=1)
/***************************************************************************
Summary:
    This private routine forms the RBUF resource request, allocates them 
    and open the allocated RBUFs.
****************************************************************************/
/* ToDo: Destinations (Output Port and Ringbuffer) are properties of an Association
         so allocate and open of these need to be handled per assoc not per channel.
         Pending for Output Port */
static BERR_Code 
BRAP_P_AllocateAndOpenRbuf(
    BRAP_AssociatedChannelHandle    hAssociatedCh,  /* [in] Association Handle */
    BRAP_DstDetails                 *pDstDetails    /* [in] Details of the destination 
                                                      to be added to the channel group */     
    );

/***************************************************************************
Summary:
    This private routine Closes the requested RBUFs and de-allocates them.
TODO: Update with 7440 RemoveDst, as of now hardcoded for two RBUFs
****************************************************************************/
static BERR_Code 
BRAP_P_CloseAndFreeRbuf(
    BRAP_AssociatedChannelHandle    hAssociatedCh,  /* [in] Association Handle */
    BRAP_RemoveDstDetails           *pDstDetails    /* [in] Details of the destination 
                                                            to be removed */     
    );
#endif

static BERR_Code BRAP_P_UpdateSpdifChanStatusParams(
	BRAP_Handle		            hRap,		    /* [in] RAP device handle */    
    	BRAP_DSPCHN_Handle                  hDspCh,		    /* [in] Dsp Channel handle */
	const BRAP_OutputPortConfig	*pOpConfig	    /* [in] Output port  configuration */
);

static BERR_Code BRAP_P_SendSpdifChanStatusParamsCommand(
    	BRAP_DSPCHN_Handle                  hDspCh,		    /* [in] DSPCHN handle */
        BRAP_Handle		                    hRap,		    /* [in] RAP device handle */    
        BRAP_FWIF_P_FwTaskHandle     hFwTask,   /* [in] Task handle */
        const BRAP_OutputPortConfig     *pOpConfig,	    /* [in] Output port  configuration */
        uint32_t                                  ui32SpdifPortIndex       /*Spdif0 : 0 , Spdif1: 1 */
    );

static BERR_Code
BRAP_P_AllocateAndInvalidateFwTaskHandles(
    BRAP_Handle     hRap
    );

#if (BRAP_7550_FAMILY !=1)
BERR_Code BRAP_P_GetDefaultInputSettings (
	BRAP_Handle	            hRap,	        /* [in] RAP device handle */
	BRAP_CapInputPort       eCapPort,       /* [in] Input port */
	BRAP_InputPortConfig	*pInConfig      /* [out] Default configuration for
	                                           the Input port */
       );

bool    BRAP_P_IsCapPortInternal(BRAP_CapInputPort   eCapPort);
#endif

/*}}}*/

/***************************************************************************
Summary:
	Gets the default settings of the Raptor Audio device.
Description:
	This API returns the default settings of the Raptor Audio device.
Returns:
	BERR_SUCCESS 
See Also:
	BRAP_Open
***************************************************************************/
BERR_Code BRAP_GetDefaultSettings(
	BRAP_Settings       *pDefaultSettings /* [out] Default device settings */
	)
{
	BDBG_ENTER(BRAP_GetDefaultSettings);
	
	/* Assert the function argument(s) */
	BDBG_ASSERT(pDefaultSettings);
	
	*pDefaultSettings = sDefSettings;
    pDefaultSettings->pImgContext = BRAP_IMG_Context;
    pDefaultSettings->pImgInterface = &BRAP_IMG_Interface;	
	BDBG_LEAVE(BRAP_GetDefaultSettings);

	return BERR_SUCCESS;
}

bool    BRAP_P_IsCapPortInternal(BRAP_CapInputPort   eCapPort)
{
    if((eCapPort == BRAP_CapInputPort_eIntCapPort0)
        ||(eCapPort == BRAP_CapInputPort_eIntCapPort1)
        ||(eCapPort == BRAP_CapInputPort_eIntCapPort2)
        ||(eCapPort == BRAP_CapInputPort_eIntCapPort3)
        ||(eCapPort == BRAP_CapInputPort_eIntCapPort4)            
        ||(eCapPort == BRAP_CapInputPort_eIntCapPort5)
        ||(eCapPort == BRAP_CapInputPort_eIntCapPort6)
        ||(eCapPort == BRAP_CapInputPort_eIntCapPort7)
    )
    {
        return true;
    }
    else
    {
        return false;
    }
}
/***************************************************************************
Summary:
	Initializes the Raptor Audio Device.

Description:
	This function should be called to initialize the Raptor Audio Device. 
	This API must be called prior to any other Raptor PI call.
	As a result a handle to the Raptor Audio Device is returned by this API, 
	which represents the instance of this device. This Device handle needs 
	to be passed to APIs operating at device level.
	
Returns:
	BERR_SUCCESS - If open is successful else error	

See Also:
	BRAP_Close, BRAP_GetDefaultSettings.
***************************************************************************/
BERR_Code BRAP_Open(
	BRAP_Handle 			*phRap,			/* [out] The RAP handle returned*/
	BCHP_Handle 			hChip,			/* [in] Chip handle */
	BREG_Handle				hRegister,		/* [in] Register handle */
	BMEM_Handle 			hMemory,		/* [in] Memory heap handle */
	BINT_Handle				hInt,			/* [in] Interrupt module handle */
	BTMR_Handle             hTimer,         /* [in] Timer handle */
	const BRAP_Settings		*pSettings		/* [in] Device settings */
	)
{
	BRAP_Handle			hRap = NULL;
	BERR_Code			ret = BERR_SUCCESS;
	BRAP_RM_Handle		hRm = NULL;
    unsigned int        i=0, j=0, k=0;
#if BRAP_P_EQUALIZER
    unsigned int        uiPp = 0, uiCsc = 0, uiPth = 0;
#endif
#if RAP_SCM_SUPPORT
	BRAP_DSPCHN_ScmType		eScmAlgoType;
#endif

	BDBG_ENTER(BRAP_Open);
	
	/* Assert the function arguments*/
	BDBG_ASSERT(phRap);
    BDBG_ASSERT(hChip);
    BDBG_ASSERT(hRegister);
    BDBG_ASSERT(hMemory);
    BDBG_ASSERT(hInt);
	BDBG_ASSERT(pSettings);	
	BDBG_ASSERT(pSettings->pImgInterface);

    BDBG_ASSERT(hTimer);
    
	/* Allocate the Audio Device handle */
	hRap = (BRAP_Handle)BKNI_Malloc(sizeof(BRAP_P_Device));
	if(hRap == NULL)
	{
		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
	}

	/* Reset the entire structure */
	BKNI_Memset(hRap, 0, sizeof(BRAP_P_Device));

#ifdef BCHP_PWR_RESOURCE_RAP_OPEN
    /* power up */
    BCHP_PWR_AcquireResource(hChip, BCHP_PWR_RESOURCE_RAP_OPEN);
#endif

#if (BRAP_7405_FAMILY == 1)
/* This implementaion is to fix a case where Dts XCD to PCm transition on 
HDMI soesn't happen on Onkyo reciever when we don't diable MAI_CFG at MAI_STOP*/
    hRap->bLastRunHadDtsTranscodeOnHdmi = false;
    hRap->bLastRunHadDtsTranscodeOnSpdif= false;
#endif
    /* Intialize the Down Mix Structure */
    for (i = 0; i < BRAP_P_MAX_DOWN_MIX_COEFF ; i++)
    {
        hRap->sDnMixCoeff[i].eInputMode = BRAP_OutputMode_eLast;
        hRap->sDnMixCoeff[i].eOutputMode = BRAP_OutputMode_eLast;
        hRap->sDnMixCoeff[i].bInputLfePresent = false;
        hRap->sDnMixCoeff[i].bOutputLfePresent = false;
    }
    
    /* Initialize Audio processing Handles to NULL*/
    for (i = 0; i < BRAP_MAX_PP_SUPPORTED; i++)
    {
        hRap->hAudioProcessingStageHandle[i]=NULL;
    }


	/* Store the device settings */
	hRap->sSettings = *pSettings;

    if(hRap->sSettings.bFwAuthEnable == true)
    {
        hRap->sSettings.bOpenTimeFwDownload = true;
    }
    
	/* Copy the base-module handles and the settings to the device handle */
    hRap->hChip = hChip;
    hRap->hRegister = hRegister;
    hRap->hHeap = hMemory;
    hRap->hInt = hInt;
    hRap->hTmr = hTimer;
    hRap->uiMixerRampStepSize = BRAP_P_DEFAULT_RAMP_STEP_SIZE;
    hRap->uiSrcRampStepSize = BRAP_P_DEFAULT_RAMP_STEP_SIZE;
    hRap->uiScalingCoefRampStepSize = 0x00010000;
    
    hRap->bWatchdogTriggered = false;
    hRap->bWatchdogRecoveryOn= false;    

#ifdef BCHP_AUD_FMM_MISC_HIFIOUT_SEL 
#if (BRAP_MAX_RFMOD_OUT > 0)
    for(i = 0; i < BRAP_MAX_RFMOD_OUT ; i++)
    {
        switch(i)
        {
            case 0:
                hRap->eRfModSource[i]= BRAP_OutputPort_eDac0;
                break;
            case 1:
                hRap->eRfModSource[i]= BRAP_OutputPort_eDac1;
                break;
            default:
                BDBG_ERR(("Please define the default source of RFMOD here"));
        }
    }
#endif    
#endif

#if (BRAP_7550_FAMILY !=1)
    for(i=0;i < BRAP_CapInputPort_eMax;i++)
    {
        if(BRAP_P_IsCapPortInternal((BRAP_CapInputPort)i))
            continue;
        BRAP_P_GetDefaultInputSettings(hRap,(BRAP_CapInputPort)i,&(hRap->sInputSettings[i]));
    }        
#endif

    /* Set output volume levels in units of dB */
    for (i = 0; i < BRAP_RM_P_MAX_OUTPUTS; i++)
    {
       hRap->bOutputMute[i] = false;   
       hRap->uiOutputLVolume[i] = BRAP_MIXER_P_DEFAULT_VOLUME;
       hRap->uiOutputRVolume[i] = BRAP_MIXER_P_DEFAULT_VOLUME;       
       hRap->uiOutputLVolumeGain[i] = BRAP_MIXER_P_DEFAULT_VOLUME_IN_5_23;
       hRap->uiOutputRVolumeGain[i] = BRAP_MIXER_P_DEFAULT_VOLUME_IN_5_23;              
    }    

    /* Initalize the Audio Descriptor Objects */

    for (i=0; i<BRAP_P_MAX_AUDIO_DESC_OBJECTS; i++)
    {
        hRap->sAudioDescObjs[i].bAvailable = true;
        hRap->sAudioDescObjs[i].sExtAudioDescDetails.hPrimaryChannel = NULL;
        hRap->sAudioDescObjs[i].sExtAudioDescDetails.hDescriptorChannel = NULL;
        hRap->sAudioDescObjs[i].hRap = NULL;
        hRap->sAudioDescObjs[i].ui32PanFadeInterfaceAddr = BRAP_RM_P_INVALID_INDEX;
    }

    /* Initalize the Multi Stream Decoder Objects */

    for (i=0; i<BRAP_P_MAX_MULTI_STREAM_DECODER_OBJECTS; i++)
    {
        hRap->sMultiStreamDecoderObjs[i].bAvailable = true;
        hRap->sMultiStreamDecoderObjs[i].sExtMultiStreamDecoderDetails.hPrimaryChannel = NULL;
        hRap->sMultiStreamDecoderObjs[i].sExtMultiStreamDecoderDetails.uiNumSecondaryChannel= 0;        
        for (j=0; j<BRAP_MAX_SEC_CHANNEL_FOR_MS_DECODER; j++)
        {        
            hRap->sMultiStreamDecoderObjs[i].sExtMultiStreamDecoderDetails.hSecondaryChannel[j]= NULL;
        }
        hRap->sMultiStreamDecoderObjs[i].hRap = NULL;
        hRap->sMultiStreamDecoderObjs[i].ui32FeedbackBufferAddress = (uint32_t)NULL;
        hRap->sMultiStreamDecoderObjs[i].ui32ReconfiguredCITAddress = (uint32_t)NULL;
        for (j=0; j<BRAP_P_MAX_FW_STG_INPUTS; j++)
        {        
            hRap->sMultiStreamDecoderObjs[i].uiNumValidIOBuffer[j]= 0;                
            for (k=0; k<BRAP_AF_P_MAX_CHANNELS; k++)
            {             
                hRap->sMultiStreamDecoderObjs[i].ui32InterTaskIoBufferAddress[j][k] = (uint32_t)NULL;
            }
            hRap->sMultiStreamDecoderObjs[i].ui32InterTaskIoGenericBufferAddress[j] = (uint32_t)NULL;
            hRap->sMultiStreamDecoderObjs[i].ui32InterTaskIoBufferHandle[j] = (uint32_t)NULL;
            hRap->sMultiStreamDecoderObjs[i].ui32InterTaskIoGenericBufferHandle[j] = (uint32_t)NULL;
            hRap->sMultiStreamDecoderObjs[i].bDRAMBuffersUsed[j] = false;
        }
    }    

#if BRAP_P_EQUALIZER
    for (i=0; i<BRAP_P_MAX_EQUALIZER_OBJECTS; i++)
    {
        hRap->sEqualizerObjs[i].bAvailable = true;
        hRap->sEqualizerObjs[i].hRap = NULL;
        hRap->sEqualizerObjs[i].sExtEqualizerDetails.bToneControlEqualizer = false;
        hRap->sEqualizerObjs[i].sExtEqualizerDetails.bFiveBandEqualizer = false;
        hRap->sEqualizerObjs[i].sExtEqualizerDetails.bSevenBandEqualizer = false;
        hRap->sEqualizerObjs[i].sExtEqualizerDetails.bCustomMode = false;
        hRap->sEqualizerObjs[i].sExtEqualizerDetails.bSubsonicEnable = false;        

        for(uiPth=0; uiPth<BRAP_P_MAX_PATHS_IN_A_CHAN; uiPth++)
        {
            for(uiPp=0; uiPp<BRAP_RM_P_MAX_PARALLEL_PATHS; uiPp++)
            {
                for(j=0; j<BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; j++)
                {
                    for(uiCsc = 0; uiCsc < BRAP_RM_P_MAX_SRC_IN_CASCADE; uiCsc++)
                    {
                        hRap->sEqualizerObjs[i].hSrcEq[uiCsc][j][uiPp][uiPth] = NULL;                
                    }
                }
            }
        }
    }
#endif

#if((BRAP_3548_FAMILY == 1) )
    hRap->hRfAudioCallback = NULL;     
    hRap->hSpdifRxCallback = NULL;     
    hRap->eSpdifRxState = BRAP_P_SPDIFRX_InputState_eStreamNone;    
    hRap->ui32SpdifRxCompState = BRAP_P_SPDIFRX_INVALID_PREAMBLE_C;
    hRap->eCapInputPort = BRAP_CapInputPort_eMax;
#endif    


	if (pSettings->bExtDeviceMem) 
    {
		hRap->sMemAllocInfo.currentStaticMemoryPointer = pSettings->ui32MemBase;          
		hRap->sMemAllocInfo.currentStaticMemoryBase = pSettings->ui32MemBase;
		hRap->sMemAllocInfo.staticMemorySize = pSettings->ui32MemSize;
		BDBG_MSG(("\nBRAP_Open: Static MemSize = 0x%x",pSettings->ui32MemSize));
	}

/*Allocate OpenTime Mallocs*/

    hRap->sOpenTimeMallocs.psResrcGrant = (BRAP_RM_P_ResrcGrant *)BKNI_Malloc(sizeof(BRAP_RM_P_ResrcGrant));
    if(hRap->sOpenTimeMallocs.psResrcGrant == NULL)
    {
        ret = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        goto free;        
    }

    hRap->sOpenTimeMallocs.psResrcReq = (BRAP_RM_P_ResrcReq *)BKNI_Malloc(sizeof(BRAP_RM_P_ResrcReq));
    if(hRap->sOpenTimeMallocs.psResrcReq == NULL)
    {
        ret = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        goto free;        
    }

    hRap->sOpenTimeMallocs.pDspChParams = (BRAP_DSPCHN_P_AudioParams *)BKNI_Malloc(sizeof(BRAP_DSPCHN_P_AudioParams));
    if(hRap->sOpenTimeMallocs.pDspChParams == NULL)
    {
        ret = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        goto free;        
    }

    hRap->sOpenTimeMallocs.pPvtDstDetails = (BRAP_P_DstDetails *)BKNI_Malloc(sizeof(BRAP_P_DstDetails));
    if(hRap->sOpenTimeMallocs.pPvtDstDetails == NULL)
    {
        ret = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        goto free;        
    }

    for(i = 0 ; i < BRAP_P_MAX_FW_BRANCH_PER_FW_TASK ; i ++)
    {
        hRap->sOpenTimeMallocs.psBranchInfo[i] = (BRAP_CIT_P_FwBranchInfo *)BKNI_Malloc(sizeof(BRAP_CIT_P_FwBranchInfo));
        if(hRap->sOpenTimeMallocs.psBranchInfo[i] == NULL)
        {
            ret = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
            goto free;        
        }
    }

    hRap->sOpenTimeMallocs.psStageStatus[0] = (BRAP_FWIF_P_StageStatus *)BKNI_Malloc(sizeof(BRAP_FWIF_P_StageStatus)*2*BRAP_MAX_FW_TSK_SUPPORTED);
    if(hRap->sOpenTimeMallocs.psStageStatus[0] == NULL)
    {
        ret = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        goto free;        
    }    

    hRap->sOpenTimeMallocs.bStageStatusBufFree[0] = true;
    for(i = 1 ; i < 2*BRAP_MAX_FW_TSK_SUPPORTED ; i ++)
    {
        hRap->sOpenTimeMallocs.bStageStatusBufFree[i] = true;
        hRap->sOpenTimeMallocs.psStageStatus[i] = (BRAP_FWIF_P_StageStatus *)((char *)(hRap->sOpenTimeMallocs.psStageStatus[0]) +  i*sizeof(BRAP_FWIF_P_StageStatus));
    }
#if BCHP_AUD_FMM_PLL0_CONTROL_REFERENCE_SELECT_VCXO_0
	/* Initialize PLL_x <-> VCXO_0 as defaults. */
    for(i=0; i < BRAP_OP_Pll_eMax; i++)
    {
        hRap->sPllSettings[i].uiVcxo = BCHP_AUD_FMM_PLL0_CONTROL_REFERENCE_SELECT_VCXO_0; 
    }
#endif 
    
    ret = BRAP_P_AllocateAndInvalidateFwTaskHandles( hRap );
    if(BERR_SUCCESS != ret)
    {
    	ret = BERR_TRACE(ret);
        goto free;
    }    
    
#if RAP_SCM_SUPPORT
	/*Set the SCM Algorithm ID*/
	eScmAlgoType = BRAP_SCM_P_GetSCMAlgorithmID(hRap->sSettings.sScmSettings.ui32CaSystemId);
	if ( eScmAlgoType >= BRAP_DSPCHN_ScmType_eMax )
	{
    	ret = BERR_TRACE(BERR_INVALID_PARAMETER);
        goto free;
	}
	BRAP_SCM_P_SetSCMAlgorithmID(eScmAlgoType);
#endif	

	ret = BRAP_P_Open(hRap);
	if(BERR_SUCCESS != ret)
	{
    	ret = BERR_TRACE(ret);
        goto free;
	}    

	/* Initialize common interrupt callbacks */
	ret = BRAP_P_DeviceLevelInterruptInstall(hRap);
	if(ret != BERR_SUCCESS)
	{
    	ret = BERR_TRACE(ret);
        goto free;
	}

	/* Instantiate the resource manager */
	ret = BRAP_RM_P_Open(hRap, &hRm);
	if(ret != BERR_SUCCESS)
	{
    	ret = BERR_TRACE(ret);
        goto free;
	}
#if (BRAP_OPEN_TIME_RBUF_ALLOCATION==1)
    /* Pre-allocate device level rbufs */
    ret = BRAP_P_AllocateDeviceRBufPool(hRap);
	if(ret != BERR_SUCCESS)
	{
    	ret = BERR_TRACE(ret);
        goto free;
	}
#endif	
	*phRap = hRap;
#if RAP_SCM_SUPPORT
	/*Start the SCM */
	BRAP_SCM_DSPCHN_P_LockAndStartScm(hRap, eScmAlgoType);
#endif	
    goto end;

free:
	/* Free the memory allocated for Audio Device handle */
    if(hRap->sOpenTimeMallocs.psResrcGrant != NULL)
        BKNI_Free(hRap->sOpenTimeMallocs.psResrcGrant);
    if(hRap->sOpenTimeMallocs.psResrcReq != NULL)    
        BKNI_Free(hRap->sOpenTimeMallocs.psResrcReq);
    if(hRap->sOpenTimeMallocs.pDspChParams != NULL)    
        BKNI_Free(hRap->sOpenTimeMallocs.pDspChParams);
    if(hRap->sOpenTimeMallocs.pPvtDstDetails != NULL)    
        BKNI_Free(hRap->sOpenTimeMallocs.pPvtDstDetails);
    for(i = 0 ; i < BRAP_P_MAX_FW_BRANCH_PER_FW_TASK ; i ++)
    {
        if(hRap->sOpenTimeMallocs.psBranchInfo[i] != NULL)    
            BKNI_Free(hRap->sOpenTimeMallocs.psBranchInfo[i]);          
    }  
    if(hRap->sOpenTimeMallocs.psStageStatus[0] != NULL)    
        BKNI_Free(hRap->sOpenTimeMallocs.psStageStatus[0]);       
    if(hRap != NULL)    
        BKNI_Free(hRap);    
#ifdef BCHP_PWR_RESOURCE_RAP_OPEN
    /* a failed Open powers down */
    BCHP_PWR_ReleaseResource(hChip, BCHP_PWR_RESOURCE_RAP_OPEN);
#endif
    
end:

	BDBG_LEAVE(BRAP_Open);	
	return ret;
}

/***************************************************************************
Summary:
	Initializes each internal modules of the Raptor Audio Device.
***************************************************************************/
BERR_Code BRAP_P_Open(
	BRAP_Handle 			hRap	/* [in] The Raptor Audio device handle*/
)
{
    BERR_Code			ret = BERR_SUCCESS;
    BRAP_DSP_Handle		hDsp = NULL;
    BRAP_DSP_P_Settings	sDspSettings;
    BRAP_FMM_P_Handle	hFmm = NULL;
    BRAP_FMM_P_Settings	sFmmSettings;
    unsigned int		uiDspIndex = 0;
    unsigned int		uiFmmIndex = 0;
    bool                bWdgRecovery = BRAP_P_GetWatchdogRecoveryFlag(hRap);

    BDBG_ENTER(BRAP_P_Open);

    if(false == bWdgRecovery)
    {
        BRAP_InitializeMit(hRap);

        BRAP_MapImgIdWithExecId(hRap,&(hRap->sImgIdMappingArrays));
        
        ret =BRAP_DSP_AllocMem(hRap);
        if(ret != BERR_SUCCESS)
        {        
            ret = BERR_TRACE(ret);        
            goto end;
        }        
    }

    /* If Fw verification is enabled, then don't download the Executable again in watchdog recovery */
    if(!((hRap->sSettings.bFwAuthEnable == true)
        && (true == bWdgRecovery)))
    {
        ret = BRAP_FWDWNLD_P_DownloadFwExec((void *)hRap);
        if(ret != BERR_SUCCESS)
        {        
            ret = BERR_TRACE(ret);        
            goto end;
        }        
        BRAP_PrintMit(hRap);		
    }
    /* Instantiate the DSP devices */
    for(uiDspIndex = 0; uiDspIndex < BRAP_RM_P_MAX_DSPS; uiDspIndex++)
    {
        /* If this is a watchdog recovery, we already have DSP handles */
        if(true == bWdgRecovery)
        {
            hDsp = hRap->hDsp[uiDspIndex];
        }

        /* Currently BRAP_DSP_P_Settings doesn't have any members, so no need to 
        initialize sDspSettings structure. Once this structure has some 
        member, initialize it as per the requirement */
        ret = BRAP_DSP_P_Open (&hDsp, hRap, uiDspIndex, &sDspSettings);
        if(BERR_SUCCESS != ret)
        {
            BDBG_ERR(("BRAP_P_Open: call to BRAP_DSP_P_Open failed."));
            ret = BERR_TRACE(ret);            
            goto end;
        }

        /* If not watchdog recovery, Store the DSP handle inside the Audio 
        Device Handle  */
        if(false == bWdgRecovery)
        {
            hRap->hDsp[uiDspIndex] = hDsp;
        }
    }

    /* Instantiate the FMM devices */
    for(uiFmmIndex = 0; uiFmmIndex < BRAP_RM_P_MAX_FMMS; uiFmmIndex++)
    {
        /* If this is a watchdog recovery, we already have FMM handles */
        if (true == bWdgRecovery)
        {
           hFmm = hRap->hFmm[uiFmmIndex]; 
        }

        /* Currently BRAP_FMM_P_Settings doesn't have any members, so no need to 
        initialize sFmmSettings structure. Once this structure has some 
        member, initialize it as per the requirement */
        ret = BRAP_FMM_P_Open (hRap, &hFmm, uiFmmIndex, &sFmmSettings);
        if(BERR_SUCCESS != ret)
        {
            BDBG_ERR(("BRAP_P_Open: call to BRAP_FMM_P_Open failed."));	
            ret = BERR_TRACE(ret);            
            goto end;
        }

        /* If not watchdog recovery, Store the FMM handle inside the Audio 
        Device Handle */
        if(false == bWdgRecovery)
            hRap->hFmm[uiFmmIndex] = hFmm;
    }

#ifdef RAP_RFAUDIO_SUPPORT
    /* Instantiate the RF Audio device */
    ret = BRAP_RFAUDIO_P_InitDecoder(hRap);
    if(BERR_SUCCESS != ret)
    {
        BDBG_ERR(("BRAP_P_Open: call to BRAP_RFAUDIO_P_InitDecoder failed."));	
        ret = BERR_TRACE(ret);        
        goto end;
    }
#endif

    /* Clear all previous raptor interrupts and mask them all */
    /* This has to be called only after BRAP_FMM_P_Open as the 
    FMM module has to be released from reset state to access the registers */
    ret = BRAP_P_ClearInterrupts(hRap->hRegister);
    if(ret != BERR_SUCCESS)
    {        
        ret = BERR_TRACE(ret);    
        goto end;
    }
    for(uiDspIndex = 0; uiDspIndex < BRAP_RM_P_MAX_DSPS; uiDspIndex++)
    {
        /*This will install the interrupt callback for DSP ping command*/
        ret = BRAP_P_AckInstall(hRap->hDsp[uiDspIndex]);
        if(BERR_SUCCESS != ret)
        { 	
            BDBG_ERR(("BRAP_P_Open: call to BRAP_P_AckInstall failed."));	
            ret = BERR_TRACE(ret);            
            goto end;
        }
    }

    BDBG_LEAVE(BRAP_P_Open);

end:
    if(ret != BERR_SUCCESS)
    {
        if(BRAP_P_IsPointerValid((void *)hDsp))
                BKNI_Free(hDsp);           
    }
    return ret;
}

/***************************************************************************
Summary:
	Closes the Raptor Audio Device.

Description:
	This function should be called when the application is exiting or the 
	application doesn't intend to use Raptor Audio PI further. This call 
	destroys the instance of the Raptor Audio Device handle. 

Returns:
	BERR_SUCCESS - If close is successful else error

See Also:
	BRAP_Open, BRAP_GetDefaultSettings.
***************************************************************************/
BERR_Code BRAP_Close(
	BRAP_Handle 			hRap	/* [in] The Raptor Audio device handle*/
	
	)
{
	BERR_Code			ret = BERR_SUCCESS;
	unsigned int		uiDspIndex = 0;
	unsigned int		uiFmmIndex = 0,i=0;

	BDBG_ENTER(BRAP_Close);
	
	/* Assert the function arguments*/
	BDBG_ASSERT(hRap);

#if (BRAP_OPEN_TIME_RBUF_ALLOCATION==1)
    /* Free device Rbuf pool */
    BRAP_P_FreeDeviceRBufPool(hRap);
#endif
    
	/* Close the DSP devices */
	for(uiDspIndex = 0; uiDspIndex < BRAP_RM_P_MAX_DSPS; uiDspIndex++)
	{
		ret = BRAP_DSP_P_Close (hRap->hDsp[uiDspIndex]);
		if(BERR_SUCCESS != ret)
		{
			ret = BERR_TRACE(ret);
			goto error;
		}
        hRap->hDsp[uiDspIndex] = NULL;
	}
    BRAP_DSP_FreeMem(hRap);
	/* Close the FMM devices */
	for(uiFmmIndex = 0; uiFmmIndex < BRAP_RM_P_MAX_FMMS; uiFmmIndex++)
	{
		ret = BRAP_FMM_P_Close (hRap->hFmm[uiFmmIndex]);
		if(BERR_SUCCESS != ret)
		{
			ret = BERR_TRACE(ret);
			goto error;
		}
	}

    /* Close the resource manager */
	ret = BRAP_RM_P_Close(hRap, hRap->hRm);
	if(BERR_SUCCESS != ret)
	{
		ret = BERR_TRACE(ret);
		goto error;
	}

	/* Uninitialize common interrupt callbacks */
	(void) BRAP_P_DeviceLevelInterruptUnInstall(hRap);

    /*This function free up the processing Handle memory.Search for the processing Handle 
    which are not freed by the application and free the memory*/
    for(i=0;i<BRAP_MAX_PP_SUPPORTED;i++)
    {
        if(hRap->hAudioProcessingStageHandle[i]!=NULL)
        {
            BRAP_P_DestroyProcessingStage(hRap->hAudioProcessingStageHandle[i]);
        }
    }

    /* Free FW Task Handles */
    if(BRAP_P_IsPointerValid((void *)hRap->hFwTask[0][0]))
    {
        BKNI_Free(hRap->hFwTask[0][0]);    
    }
    
    if(hRap->sOpenTimeMallocs.psResrcGrant != NULL)
        BKNI_Free(hRap->sOpenTimeMallocs.psResrcGrant);
    if(hRap->sOpenTimeMallocs.psResrcReq != NULL)    
        BKNI_Free(hRap->sOpenTimeMallocs.psResrcReq);
    if(hRap->sOpenTimeMallocs.pDspChParams != NULL)    
        BKNI_Free(hRap->sOpenTimeMallocs.pDspChParams);
    if(hRap->sOpenTimeMallocs.pPvtDstDetails != NULL)    
        BKNI_Free(hRap->sOpenTimeMallocs.pPvtDstDetails);
    for(i = 0 ; i < BRAP_P_MAX_FW_BRANCH_PER_FW_TASK ; i ++)
    {
        if(hRap->sOpenTimeMallocs.psBranchInfo[i] != NULL)    
            BKNI_Free(hRap->sOpenTimeMallocs.psBranchInfo[i]);          
    } 
    if(hRap->sOpenTimeMallocs.psStageStatus[0] != NULL)    
        BKNI_Free(hRap->sOpenTimeMallocs.psStageStatus[0]);     
    
error:

#ifdef BCHP_PWR_RESOURCE_RAP_OPEN
    /* power down */
    BCHP_PWR_ReleaseResource(hRap->hChip, BCHP_PWR_RESOURCE_RAP_OPEN);
#endif
    
	/* Free up the Audio Device handle */
	BKNI_Free(hRap);

	BDBG_LEAVE(BRAP_Close);
	return ret;
}

/***************************************************************************
Summary:
	Enter standby mode with the Raptor Audio Device.

Description:
	This function puts the Raptor Audio Device into standby mode, 
	if supported. All Raptor channels must be in a stopped state 
	in order to successfully enter standby mode.
	If standby mode is not supported, calling this function has no effect.

	When in standby mode, the device clocks are turned off,
	resulting in a minimal power state.

	No BRAP_* calls should be made until standby mode exitted by calling
	BRAP_Resume().

Returns:
	BERR_SUCCESS - If standby is successful, otherwise error

See Also:
	BRAP_Resume
***************************************************************************/
BERR_Code BRAP_Standby(
	BRAP_Handle 			hRap, 		/* [in] RAP device handle */
	BRAP_StandbySettings 	*pSettings 	/* [in] standby settings */
)
{
    BDBG_ENTER(BRAP_Standby);
    BDBG_ASSERT(hRap);
    BSTD_UNUSED(pSettings);

#ifdef BCHP_PWR_RESOURCE_RAP_OPEN
    {
        unsigned i;
        /* check that all channels have been stopped */
        for (i=0; i<BRAP_RM_P_MAX_DEC_CHANNELS; i++) {
            if (hRap->hRapDecCh[i] && hRap->hRapDecCh[i]->eState == BRAP_P_State_eStarted) {
                BDBG_ERR(("One or more decode channels are not stopped"));
                return BERR_UNKNOWN;
            }
        }

        for (i=0; i<BRAP_RM_P_MAX_PCM_CHANNELS; i++) {
            if (hRap->hRapPbCh[i] && hRap->hRapPbCh[i]->eState == BRAP_P_State_eStarted) {
                BDBG_ERR(("One or more playback channels are not stopped"));
                return BERR_UNKNOWN;
            }
        }

        for (i=0; i<BRAP_RM_P_MAX_CAP_CHANNELS; i++) {
            if (hRap->hRapCapCh[i] && hRap->hRapCapCh[i]->eState == BRAP_P_State_eStarted) {
                BDBG_ERR(("One or more capture channels are not stopped"));
                return BERR_UNKNOWN;
            }
        }
#ifdef RAP_REALVIDEO_SUPPORT
        for (i=0; i<BRAP_RM_P_MAX_VIDEO_DEC_CHANNELS; i++) { 
            if (hRap->hRapVideoDecCh[i] && hRap->hRapVideoDecCh[i]->eState == BRAP_P_State_eStarted) {
                BDBG_ERR(("One or more video decode channels are not stopped"));
                return BERR_UNKNOWN;
            }
        }
#endif

        /* if we reach here, then no channels are active. we can power down */
        if (!hRap->bStandby) {
            hRap->bStandby = true;
            BCHP_PWR_ReleaseResource(hRap->hChip, BCHP_PWR_RESOURCE_RAP_OPEN);
            BDBG_MSG(("BRAP_Standby"));
        }
    }
#endif
    return BERR_SUCCESS;
    
}

/***************************************************************************
Summary:
	Exit standby mode with the Raptor Audio Device.

Description:
	This function exits the Raptor Audio Device from standby mode. 
	After exitting standby mode, upper-level SW is free to call
	BRAP_* functions.

Returns:
	BERR_SUCCESS - If resume is successful, otherwise error

See Also:
	BRAP_Standby
***************************************************************************/
BERR_Code BRAP_Resume(
	BRAP_Handle 			hRap 		/* [in] RAP device handle */
)
{
    BDBG_ENTER(BRAP_Resume);
    BDBG_ASSERT(hRap);

#ifdef BCHP_PWR_RESOURCE_RAP_OPEN
    if (hRap->bStandby) {
        hRap->bStandby = false;
        BCHP_PWR_AcquireResource(hRap->hChip, BCHP_PWR_RESOURCE_RAP_OPEN);
        BDBG_MSG(("BRAP_Resume"));
    }
    else {
        BDBG_MSG(("Not in standby mode"));
        return BERR_INVALID_PARAMETER;
    }
#endif
    return BERR_SUCCESS;

}

/***************************************************************************
Summary:
	API returns revision information.

Description:
	Gets the revision information of all the firmwares executing on Raptor 
	DSP and FMM hardware as well as the revision information of the Raptor 
	Audio PI.
Returns:
	BERR_SUCCESS 

See Also:
	None	
****************************************************************************/
BERR_Code BRAP_GetRevision ( 	
	BRAP_Handle 			hRap,		/* [in] The RAP device handle */
	BRAP_RevisionInfo		*pRevInfo	/* [out]Version infomation of Raptor 
												Audio PI, the microcode 
												running on Raptor DSP and 
												Raptor FMM */ 
	)
{
	BERR_Code ret = BERR_SUCCESS;
	unsigned int uiCount = 0;
	
	BDBG_ENTER (BRAP_GetRevision);	
	BDBG_ASSERT(hRap);
	BDBG_ASSERT(pRevInfo);

	/* Initialize the structure */
	pRevInfo->ui32ChipId = 0;
	for ( uiCount = 0; uiCount < BRAP_MAX_DSP_DECODE_ALGO; uiCount++ )
	{
		pRevInfo->sDSPVersion.sDecAlgoVersion[uiCount].ui32FwVersionField1 = 0;
		pRevInfo->sDSPVersion.sDecAlgoVersion[uiCount].ui32FwVersionField2 = 0;
		pRevInfo->sDSPVersion.sDecAlgoVersion[uiCount].ui32CustomerID = 0;
		pRevInfo->sDSPVersion.sDecAlgoVersion[uiCount].ui32Date = 0;
		pRevInfo->sDSPVersion.sDecAlgoVersion[uiCount].ui32MajorVersion = 0;
		pRevInfo->sDSPVersion.sDecAlgoVersion[uiCount].ui32MinorVersion = 0;
		pRevInfo->sDSPVersion.sDecAlgoVersion[uiCount].ui32Year= 0;
	}
	for ( uiCount = 0; uiCount < BRAP_MAX_DSP_FS_ALGO; uiCount++ )
	{
		pRevInfo->sDSPVersion.sDecFsVersion[uiCount].ui32FwVersionField1 = 0;
		pRevInfo->sDSPVersion.sDecFsVersion[uiCount].ui32FwVersionField2 = 0;
		pRevInfo->sDSPVersion.sDecFsVersion[uiCount].ui32CustomerID = 0;
		pRevInfo->sDSPVersion.sDecFsVersion[uiCount].ui32Date = 0;
		pRevInfo->sDSPVersion.sDecFsVersion[uiCount].ui32MajorVersion = 0;
		pRevInfo->sDSPVersion.sDecFsVersion[uiCount].ui32MinorVersion = 0;
		pRevInfo->sDSPVersion.sDecFsVersion[uiCount].ui32Year= 0;
	}
	for ( uiCount = 0; uiCount < BRAP_MAX_DSP_PP_ALGO; uiCount++ )
	{
		pRevInfo->sDSPVersion.sPPAlgoVersion[uiCount].ui32FwVersionField1 = 0;
		pRevInfo->sDSPVersion.sPPAlgoVersion[uiCount].ui32FwVersionField2 = 0;
		pRevInfo->sDSPVersion.sPPAlgoVersion[uiCount].ui32CustomerID = 0;
		pRevInfo->sDSPVersion.sPPAlgoVersion[uiCount].ui32Date = 0;
		pRevInfo->sDSPVersion.sPPAlgoVersion[uiCount].ui32MajorVersion = 0;
		pRevInfo->sDSPVersion.sPPAlgoVersion[uiCount].ui32MinorVersion = 0;
		pRevInfo->sDSPVersion.sPPAlgoVersion[uiCount].ui32Year= 0;
	}

	BRAP_DSP_P_GetFwVersionInfo (hRap->hDsp[0], &pRevInfo->sDSPVersion);

	/* Assign PI version info */
	pRevInfo->sPIVersion.ui32Date = \
		BRAP_PRIV_PI_VERSION_DATE; /* Includes month and date */
	pRevInfo->sPIVersion.ui32MajorVersion = BRAP_PRIV_PI_VERSION_MAJOR;
	pRevInfo->sPIVersion.ui32MinorVersion = BRAP_PRIV_PI_VERSION_MINOR;
	pRevInfo->sPIVersion.ui32Year = BRAP_PRIV_PI_VERSION_YEAR;

	pRevInfo->ui32ChipId = BRAP_PRIV_PI_CHIP_ID;
	pRevInfo->sPIVersion.pCustomerID = BRAP_PRIV_PI_VERSION_CUSTOMER_ID;

	BDBG_MSG(("Chip ID=%x",pRevInfo->ui32ChipId));
	BDBG_MSG(("Customer ID=%s",pRevInfo->sPIVersion.pCustomerID));
	BDBG_MSG(("PI MajorVersion=%x",pRevInfo->sPIVersion.ui32MajorVersion));
	BDBG_MSG(("PI MinorVersion=%x",pRevInfo->sPIVersion.ui32MinorVersion));
	BDBG_MSG(("PI Month and Date=%x",pRevInfo->sPIVersion.ui32Date));
	BDBG_MSG(("PI Year=%x",pRevInfo->sPIVersion.ui32Year));
	
	BDBG_LEAVE (BRAP_GetRevision);
	return ret;
}

/***************************************************************************
Summary:
	Gets the default channel settings for an audio channel type.
	
Description:
	This API returns the default channel settings of a channel type.
	The default channel settings is -
	"Decode channel with eNone channel sub-type and CDB/ITB (transport) as the 
	audio data source"
	
Returns:
	BERR_SUCCESS on success else error
	
See Also:
	BRAP_OpenChannel
***************************************************************************/
BERR_Code BRAP_GetDefaultChannelSettings(
	BRAP_Handle 			hRap,			/* [in] The RAP device handle */
	BRAP_ChannelSettings    *pDefSettings	/* [out] Default channel settings */
	)
{
	BDBG_ENTER(BRAP_GetDefaultChannelSettings);
	
	/* Assert the function argument(s) */
	BDBG_ASSERT(hRap);
	BDBG_ASSERT(pDefSettings);
    BSTD_UNUSED(hRap);
	
	*pDefSettings = sDefChannelSettings;
	
	BDBG_LEAVE(BRAP_GetDefaultChannelSettings);

	return BERR_SUCCESS;
}


/***************************************************************************
Summary:
	API used to open a channel.
	
Description:
	It is used to instantiate a channel. It allocates channel handle 
	and resource required for the channel if any.
	
	Note: This should be called only after the device has been opened with 
	BRAP_Open().
	
Returns:
	BERR_SUCCESS on success else error
	
See Also:
	BRAP_CloseChannel, BRAP_GetDefaultChannelSettings
****************************************************************************/
BERR_Code BRAP_OpenChannel(
	BRAP_Handle 			    hRap,		    /* [in] The Raptor Audio 
	                                               device handle*/
	BRAP_ChannelHandle 		    *phRapCh,		/* [out] The RAP Channel 
	                                               handle */
	const BRAP_ChannelSettings	*pChnSettings   /* [in] Channel settings*/ 
	)
{
    BERR_Code ret = BERR_SUCCESS;
    BRAP_ChannelHandle hRapCh = NULL;
    bool bWdgRecovery = false;
    
	BDBG_ENTER(BRAP_OpenChannel);
	
	/* Assert the function argument(s) */
	BDBG_ASSERT(hRap);
	BDBG_ASSERT(phRapCh); 
	BDBG_ASSERT(pChnSettings);

    BDBG_MSG(("============================"));
    BDBG_MSG(("BRAP_OpenChannel:"
              "\n\t Settings:"        
              "\n\t eChType = %d" 
              "\n\t eChSubType = %d",
              pChnSettings->eChType, pChnSettings->eChSubType));
    
#if (BRAP_INDEP_OP_DELAY_SUPPORTED == 1)
    if(pChnSettings->bEnaIndDly == true)
    {
        BDBG_MSG(("Independent Delay is Enabled for the channel"));
    }
#endif
    BDBG_MSG(("============================"));

    bWdgRecovery = BRAP_P_GetWatchdogRecoveryFlag(hRap);
    
    if(bWdgRecovery == false)
    {
    	/* Allocate an Audio Decode Channel handle */
    	hRapCh = (BRAP_ChannelHandle)BKNI_Malloc(sizeof(BRAP_P_Channel));
    	if(NULL == hRapCh)
    	{
    		BDBG_ERR(("Memory allocation for channel handle failed"));
    		ret = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    		goto exit;
    	}
       BKNI_Memset(hRapCh,0,sizeof(BRAP_P_Channel));
    	/* Initialize the Channel Handle */
    	ret = BRAP_P_InitRapChannelHandle(hRapCh,hRap);
    	if( ret!=BERR_SUCCESS )
    	{
    		ret = BERR_TRACE( ret );
    		goto error;
    	}

    	/* Populate hRapCh using hRap, pChnSettings etc. */
    	hRapCh->hChip                   = hRap->hChip ;
    	hRapCh->hRegister               = hRap->hRegister;
    	hRapCh->hHeap                   = hRap->hHeap ;
    	hRapCh->hInt                    = hRap->hInt ;
    	hRapCh->hRap                    = hRap;
        hRapCh->hTimer                  = NULL;
                
    	/* Invalid state as not yet opened successfully */
        hRapCh->eState                  = BRAP_P_State_eInvalid; 
    	hRapCh->eChannelType            = pChnSettings->eChType; 
    	hRapCh->bInternalCallFromRap    = false;
    	hRapCh->ui32FmmBFIntMask        = 0;
    	hRapCh->eChannelSubType         = pChnSettings->eChSubType;
       hRapCh->bOpenTimeWrToRbuf = pChnSettings->bOpenTimeWrToRbuf;
       hRapCh->eSamplingRate = BAVC_AudioSamplingRate_eUnknown;       
#if (BRAP_INDEP_OP_DELAY_SUPPORTED == 1)
        hRapCh->bIndepDelayEnabled = pChnSettings->bEnaIndDly;
#endif
#if ((BRAP_3548_FAMILY == 1)||(BRAP_7405_FAMILY == 1))
        hRapCh->hAdFade                 = NULL;
#endif
        hRapCh->hMultiStreamDecoder =NULL;

#ifdef RAP_VIDEOONDSP_SUPPORT
        if(pChnSettings->eChType != BRAP_ChannelType_eVideoDecode)
#endif
        {
#if (BRAP_OPEN_TIME_RBUF_ALLOCATION==1)		
		hRapCh->sChanRBufPool.sExtRBufPoolSettings = pChnSettings->sChnRBufPool;
        /* Allocate channel specific rbuf pool as per user request */
		BDBG_MSG(("Open Time Allocation"));
        ret = BRAP_P_AllocateChannelRBufPool(hRapCh);
    	if(BERR_SUCCESS != ret)
    	{
    		BDBG_ERR(("Channel Rbuf Pool alloc failed with err = %d", ret));
    		ret = BERR_TRACE(ret); 
			goto error;
    	}
#endif		
    }    
    }    
   	else
	{ 
		/* In watchdog recovery */
        BDBG_MSG(("BRAP_OpenChannel: Watchdog Recovery!!!"));
		hRapCh = *phRapCh;
	} /* bWdgRecovery */
    
	switch(pChnSettings->eChType)
	{
		case BRAP_ChannelType_eDecode:
			ret = BRAP_DEC_P_ChannelOpen(hRap, hRapCh, pChnSettings);
			break;
		case BRAP_ChannelType_ePcmPlayback:
            ret = BRAP_PB_P_ChannelOpen(hRap, hRapCh, pChnSettings);
            break;
#if (BRAP_7550_FAMILY !=1)
		case BRAP_ChannelType_ePcmCapture:   
			ret = BRAP_CAP_P_ChannelOpen(hRap, hRapCh, pChnSettings); 
			break;
#endif      
#ifdef RAP_REALVIDEO_SUPPORT
            case BRAP_ChannelType_eVideoDecode:
                ret = BRAP_VID_P_ChannelOpen(hRap, hRapCh, pChnSettings);
                break;
#endif
		default:
			/* eChannelType field in RAP channel handle corrupted */
            BDBG_ERR(("BRAP_OpenChannel: Unknown eChType %d", 
                pChnSettings->eChType));
			ret = BERR_TRACE(BRAP_ERR_BAD_DEVICE_STATE);
            break;
	}

    if(false == bWdgRecovery)
    {
        if(BERR_SUCCESS == ret)
        {
        	*phRapCh = hRapCh;
            goto exit;
        }
        else
        {
            *phRapCh = NULL;
    		goto error;
        }
    }
    else
    {
       if(BERR_SUCCESS == ret)
        {
            goto exit;
        }
        else
        {
    		goto error;
        }
    }
	
error:
	/* Free up the channel handle */
	BKNI_Free(hRapCh);
    
exit:

    if(BERR_SUCCESS == ret)
    {
    BDBG_MSG(("BRAP_OpenChannel: *phRapCh = 0x%x", *phRapCh));
    }
	BDBG_LEAVE(BRAP_OpenChannel);
	return ret; 
}

/***************************************************************************
Summary:
	API used to close a Raptor Audio Channel.

Description:
	It closes the instance of a raptor channel operation. It frees the channel
	handle and resources associated with it if any.

Returns:
	BERR_SUCCESS on success else error

See Also:
	BRAP_OpenChannel
****************************************************************************/
BERR_Code BRAP_CloseChannel( 
	BRAP_ChannelHandle 	hRapCh	                /* [in]The RAP Channel handle */
	)
{
    BERR_Code ret = BERR_SUCCESS;
    uint32_t i=0;

	BDBG_ENTER(BRAP_CloseChannel);

	/* Assert the function argument */
	BDBG_ASSERT(hRapCh);
    BDBG_MSG(("============================"));    
    BDBG_MSG(("BRAP_CloseChannel: hRapCh = 0x%x" 
              "\n\t Channel Type = %d"
              "\n\t Channel Subtype = %d",
                hRapCh, hRapCh->eChannelType, hRapCh->eChannelSubType));
    BDBG_MSG(("============================"));
    
    switch(hRapCh->eChannelType)
	{
		case BRAP_ChannelType_eDecode:
			ret = BRAP_DEC_P_ChannelClose(hRapCh);
			break;
		case BRAP_ChannelType_ePcmPlayback:
            ret = BRAP_PB_P_ChannelClose(hRapCh);
            break;
#if (BRAP_7550_FAMILY !=1)
		case BRAP_ChannelType_ePcmCapture:
            ret = BRAP_CAP_P_ChannelClose(hRapCh);
            break;
#endif            
#ifdef RAP_VIDEOONDSP_SUPPORT
            case BRAP_ChannelType_eVideoDecode:
                ret = BRAP_VID_P_ChannelClose(hRapCh);
                break;
#endif
		default:
			/* eChannelType field in RAP channel handle corrupted */
            BDBG_ERR(("BRAP_CloseChannel: Unknown eChType %d", 
                hRapCh->eChannelType));
			ret = BERR_TRACE(BRAP_ERR_BAD_DEVICE_STATE);
            break;
	}
#if (BRAP_OPEN_TIME_RBUF_ALLOCATION==1)
	BRAP_P_FreeChannelRBufPool(hRapCh);

#endif	

#ifdef BRAP_OPEN_TIME_PATH_ALLOCATION        
    for(i = 0; i < BRAP_P_MAX_PATHS_IN_A_CHAN; i++)
    {
        BKNI_Free( hRapCh->pMemPath[i]);        
    }
#endif

	BKNI_Free( hRapCh );

	BDBG_LEAVE(BRAP_CloseChannel);
    return ret;
}


/***************************************************************************
Summary:
	Associates/Groups a set of audio channels together.
	
Description:
	This API associates/groups a set of audio channel handles together. All 
	those channels that are required to be mixed together to produce an 
	audio, should be associated/grouped together using this API.

	E.g. Primary decode, secondary decode and PCM playback (sound effect) 
	channels are mixed together to produce the desired audio output. So,
	these channels are required to be associated together.

	Note: 
	1. In case, just one channels is sufficient to produce the desired
	   audio then just a single channel can be added to the association. 
	2. This operation is permissible only when all the channels are in 
	   the stop state.
	
Returns:
	BERR_SUCCESS for a successful grouping, else returns an error.
		
See Also:
	BRAP_DestroyAssociation.
***************************************************************************/
BERR_Code BRAP_CreateAssociation(
    BRAP_AssociatedChannelHandle    *phAssociatedCh,     
                                                /* [out] Handle to the group
                                                   of associated audio 
                                                   channels */
    BRAP_AssociatedChanSettings     *pAssociatedChSettings        
                                                /* [in] Pointer to a stucture
                                                   that holds the details of 
                                                   channels to be grouped 
                                                   together */
	)
{
    BERR_Code           err = BERR_SUCCESS;
    BRAP_Handle 	    hRap = NULL;
    BRAP_ChannelHandle  hRapCh = NULL;
    int                 i=0;
    bool                bGrpFree = true;
    unsigned int        uiGrpId = 0, uiAssociationId=0;
    unsigned int        uiChanIdx = 0;
    BRAP_P_AssociatedChanGrp *psAssociatedChanGroup=NULL;
    BRAP_P_ChannelAudioProcessingStage sTempStage;
    int                 j = 0;
    BRAP_MultiStreamDecoderHandle   hMultiStreamDecoder=NULL;
    BRAP_P_MultiStreamDecoderparams sParams;    
    BDBG_ENTER(BRAP_CreateAssociation);

    /* Validate input params */
    BDBG_ASSERT(phAssociatedCh);
    BDBG_ASSERT(pAssociatedChSettings);

    sParams.uiNumSecondaryChannel = 0;
    sParams.hPrimaryChannel = NULL;
    for(i=0; i< BRAP_MAX_SEC_CHANNEL_FOR_MS_DECODER; i++)
    {
        sParams.hSecondaryChannel[i] = NULL;
    }    
    
    *phAssociatedCh = NULL;
    psAssociatedChanGroup = ( BRAP_P_AssociatedChanGrp *) 
                                BKNI_Malloc( sizeof( BRAP_P_AssociatedChanGrp ));
    if ( NULL==psAssociatedChanGroup )
    {
    		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }
    
    BKNI_Memset(psAssociatedChanGroup, 0, sizeof(BRAP_P_AssociatedChanGrp));

    for(i=0; i< BRAP_P_MAX_DST_PER_RAPCH; i++)
    {
        BRAP_P_InitDestination(&(psAssociatedChanGroup->sDstDetails[i]));
    }
    psAssociatedChanGroup->hMasterChannel = NULL;


    /* Invalid Init */
    sTempStage.bDecoderStage = false;
    sTempStage.bCloneStage = false;
    sTempStage.bInternalProcessingStage = false;
    sTempStage.hAudioProcessing = NULL;
    sTempStage.hValidDestHandle = NULL;
    for(i=0;i<BRAP_P_MAX_DEST_PER_PROCESSING_STAGE;i++)    
    {
        sTempStage.hDestHandle[i] = NULL;
    }
    sTempStage.ui32MasterBranchId = BRAP_INVALID_VALUE;
    sTempStage.ui32MasterStageId = BRAP_INVALID_VALUE;
    sTempStage.ui32CitBranchId = BRAP_INVALID_VALUE;
    sTempStage.ui32CitStageId = BRAP_INVALID_VALUE;
    
    for(i=0;i<BRAP_P_MAX_DST_PER_RAPCH;i++)
    {
        for(j=0;j<BRAP_MAX_PP_PER_BRANCH_SUPPORTED;j++)
        {
            psAssociatedChanGroup->sAudProcessingStage[i][j] = sTempStage;
        }
    }
    
    /* Check if all channels belong to the same RAP device */
    if(BRAP_P_IsPointerValid((void *)pAssociatedChSettings->sChDetail[0].hRapCh))
    {
        hRap = pAssociatedChSettings->sChDetail[0].hRapCh->hRap;
        for(i=0; (i<BRAP_MAX_ASSOCIATED_CHANNELS_IN_GRP) && 
		(pAssociatedChSettings->sChDetail[i].hRapCh); i++)
        {
            if (hRap != pAssociatedChSettings->sChDetail[i].hRapCh->hRap)
            {
                BDBG_ERR(("BRAP_CreateAssociation: hRapCh belongs to different"
                    " RAP devices"));
                err = BERR_TRACE(BERR_NOT_SUPPORTED);
                goto error;		  
            }
        }
    }
    else
    {
        err = BERR_TRACE(BERR_NOT_INITIALIZED);
        goto error;
    }

    /* Find the free group */
    for(uiGrpId=0; uiGrpId < BRAP_MAX_ASSOCIATED_GROUPS; uiGrpId++)
    {
    	bGrpFree = true;
        for(i=0; i<BRAP_MAX_PRI_DEC_CHAN_IN_ASSOCIATED_GRP; i++)
        {   
            if(BRAP_P_IsPointerValid((void *)hRap->sAssociatedCh[uiGrpId].hPriDecCh[i]))
            {
                /* This group is in use check the next group */
                bGrpFree = false;
                break;
            }
        }
        if(false == bGrpFree)
        {
            continue;
        }

        for(i=0; i<BRAP_MAX_SEC_DEC_CHAN_IN_ASSOCIATED_GRP; i++)
        {   
            if(BRAP_P_IsPointerValid((void *)hRap->sAssociatedCh[uiGrpId].hSecDecCh[i]))
            {
                /* This group is in use check the next group */
                bGrpFree = false;
                break;
            }
        }
        if(false == bGrpFree)
        {
            continue;
        }

        for(i=0; i<BRAP_MAX_PB_CHAN_IN_ASSOCIATED_GRP; i++)
        {   
            if(BRAP_P_IsPointerValid((void *)hRap->sAssociatedCh[uiGrpId].hPBCh[i]))
            {
                /* This group is in use check the next group */
                bGrpFree = false;
                break;
            }
        }
        if(false == bGrpFree)
        {
            continue;
        }

        for(i=0; i<BRAP_MAX_CAP_CHAN_IN_ASSOCIATED_GRP; i++)
        {   
            if(BRAP_P_IsPointerValid((void *)hRap->sAssociatedCh[uiGrpId].hCapCh[i]))
            {
                /* This group is in use check the next group */
                bGrpFree = false;
                break;
            }
        }
        if(false == bGrpFree)
        {
            continue;
        }
        if(true == bGrpFree)
        {
            break;
        }
     }

    BDBG_MSG(("BRAP_CreateAssociation: uiGrpId = %d", uiGrpId));
    if(uiGrpId >= BRAP_MAX_ASSOCIATED_GROUPS)
    {
        BDBG_ERR(("BRAP_CreateAssociation: Max associated groups reached"));
        err = BERR_TRACE(BERR_NOT_SUPPORTED);
        goto error;		
    }
    uiAssociationId = uiGrpId;
    /* Group each channel in the associated group handle */
    for(i=0; i<BRAP_MAX_ASSOCIATED_CHANNELS_IN_GRP; i++)
    {   
        BDBG_MSG(("BRAP_CreateAssociation: i = %d",i));
        hRapCh = pAssociatedChSettings->sChDetail[i].hRapCh;
        if(!(BRAP_P_IsPointerValid((void *)hRapCh)))
            continue;
        hRapCh->uiAssociationId[uiAssociationId] = uiGrpId;
        switch(hRapCh->eChannelType)
        {
            case BRAP_ChannelType_eDecode:
                if((BRAP_ChannelSubType_ePrimary == hRapCh->eChannelSubType)||
			(BRAP_ChannelSubType_eNone == hRapCh->eChannelSubType))
                {
                    /* Primary decode channel */
                    for(
                       uiChanIdx=0;
                       uiChanIdx<BRAP_MAX_PRI_DEC_CHAN_IN_ASSOCIATED_GRP ; 
                       uiChanIdx++)
                    {
                        BDBG_MSG(("BRAP_CreateAssociation: PriDec uiChanIdx=%d",
                            uiChanIdx));    
                        if(!(BRAP_P_IsPointerValid((void *)psAssociatedChanGroup->hPriDecCh[uiChanIdx])))
                        {
                            /* Got a placeholder, so break */
                            break;
                        }
                    }
                    if(uiChanIdx >= BRAP_MAX_PRI_DEC_CHAN_IN_ASSOCIATED_GRP)
                    {
                        BDBG_ERR(("BRAP_CreateAssociation: More than %d primary"
                            " dec in a grp", uiChanIdx));
                        err = BERR_TRACE(BERR_NOT_SUPPORTED);
                        goto error;						
                    }

                    psAssociatedChanGroup->hPriDecCh[uiChanIdx] = hRapCh;
                }
                else if(BRAP_ChannelSubType_eSecondary == hRapCh->eChannelSubType)
                {
                    /* Secondary decode channel */
                    for(uiChanIdx=0;
                        uiChanIdx < BRAP_MAX_SEC_DEC_CHAN_IN_ASSOCIATED_GRP;
				        uiChanIdx++)
                    {
                        BDBG_MSG(("BRAP_CreateAssociation: SecDec uiChanIdx=%d",
                            uiChanIdx));    
                        if(!(BRAP_P_IsPointerValid((void *)psAssociatedChanGroup->hSecDecCh[uiChanIdx])))
                        {
                            /* Got a placeholder, so break */
                            break;
                        }
                    }
                       
                    if(uiChanIdx >= BRAP_MAX_SEC_DEC_CHAN_IN_ASSOCIATED_GRP)
                    {
                        BDBG_ERR(("BRAP_CreateAssociation: More than %d"
                            " secondary dec in a grp", uiChanIdx));
                        err = BERR_TRACE(BERR_NOT_SUPPORTED);
                        goto error;						
                    }

                    psAssociatedChanGroup->hSecDecCh[uiChanIdx] = hRapCh;
                }
				else
					BDBG_ASSERT(0);

                break;

            case BRAP_ChannelType_ePcmPlayback:        
                for(uiChanIdx=0; 
                    uiChanIdx<BRAP_MAX_PB_CHAN_IN_ASSOCIATED_GRP;
                    uiChanIdx++)
                {
                    BDBG_MSG(("BRAP_CreateAssociation: PB uiChanIdx=%d",
                        uiChanIdx));    
                    if(!(BRAP_P_IsPointerValid((void *)psAssociatedChanGroup->hPBCh[uiChanIdx])))
                    {
                        /* Got a placeholder, so break */
                        break;
                    }
                }
                if(uiChanIdx >= BRAP_MAX_PB_CHAN_IN_ASSOCIATED_GRP)
                {
                    BDBG_ERR(("BRAP_CreateAssociation: More than %d PCM"
                        " playback channels in a grp", uiChanIdx));
                    err = BERR_TRACE(BERR_NOT_SUPPORTED);
                    goto error;					
                }

                psAssociatedChanGroup->hPBCh[uiChanIdx] = hRapCh;
                break;
				
            case BRAP_ChannelType_ePcmCapture:
                for(uiChanIdx=0; 
                    uiChanIdx<BRAP_MAX_CAP_CHAN_IN_ASSOCIATED_GRP;
                    uiChanIdx++)
                {
                    BDBG_MSG(("BRAP_CreateAssociation: CAP uiChanIdx=%d",
                        uiChanIdx));    
                    if(!(BRAP_P_IsPointerValid((void *)psAssociatedChanGroup->hCapCh[uiChanIdx])))
                    {
                        /* Got a placeholder, so break */
                        break;
                    }
                }
                if(uiChanIdx >= BRAP_MAX_CAP_CHAN_IN_ASSOCIATED_GRP)
                {
                    BDBG_ERR(("BRAP_CreateAssociation: More than %d CAP"
                        " playback channels in a grp", uiChanIdx));
                    err = BERR_TRACE(BERR_NOT_SUPPORTED);
                    goto error;					
                }

                psAssociatedChanGroup->hCapCh[uiChanIdx] = hRapCh;
                break;
            default:
                BDBG_ERR(("Can't associate ChannelType %d",
                    hRapCh->eChannelType));
                BDBG_ASSERT(0);
                break;
        }/* switch */
    }/* for i */

    if(pAssociatedChSettings->hMasterChannel !=NULL)
    {      
        sParams.hPrimaryChannel = pAssociatedChSettings->hMasterChannel;
        sParams.uiNumSecondaryChannel= 0;

        for(i =0 ; i< BRAP_MAX_PRI_DEC_CHAN_IN_ASSOCIATED_GRP ; i++)
        {
            if((psAssociatedChanGroup->hPriDecCh[i] != NULL)
                &&(psAssociatedChanGroup->hPriDecCh[i]  != pAssociatedChSettings->hMasterChannel))
            {
                sParams.hSecondaryChannel[sParams.uiNumSecondaryChannel] = psAssociatedChanGroup->hPriDecCh[i];
                sParams.uiNumSecondaryChannel++;
            }
        }

        for(i =0 ; i< BRAP_MAX_SEC_DEC_CHAN_IN_ASSOCIATED_GRP ; i++)
        {
            if((psAssociatedChanGroup->hSecDecCh[i] != NULL)
                &&(psAssociatedChanGroup->hSecDecCh[i]  != pAssociatedChSettings->hMasterChannel))
            {
                sParams.hSecondaryChannel[sParams.uiNumSecondaryChannel] = psAssociatedChanGroup->hSecDecCh[i];
                sParams.uiNumSecondaryChannel++;
            }
        }        
        
        for(i =0 ; i< BRAP_MAX_PB_CHAN_IN_ASSOCIATED_GRP ; i++)
        {
            if((psAssociatedChanGroup->hPBCh[i] != NULL)
                &&(psAssociatedChanGroup->hPBCh[i]  != pAssociatedChSettings->hMasterChannel))
            {
                sParams.hSecondaryChannel[sParams.uiNumSecondaryChannel] = psAssociatedChanGroup->hPBCh[i];
                sParams.uiNumSecondaryChannel++;
            }
        }   
        
        BRAP_P_CreateMultiStreamDecoderObject(hRap,&hMultiStreamDecoder);
        BRAP_P_SetMultiStreamDecoderObject(hMultiStreamDecoder,&sParams);

        for(i =0 ; i< BRAP_MAX_PRI_DEC_CHAN_IN_ASSOCIATED_GRP ; i++)
        {
            if((psAssociatedChanGroup->hPriDecCh[i] != NULL))
            {
                psAssociatedChanGroup->hPriDecCh[i]->hMultiStreamDecoder = hMultiStreamDecoder;
            }
        }

        for(i =0 ; i< BRAP_MAX_SEC_DEC_CHAN_IN_ASSOCIATED_GRP ; i++)
        {
            if((psAssociatedChanGroup->hSecDecCh[i] != NULL))
            {
                psAssociatedChanGroup->hSecDecCh[i]->hMultiStreamDecoder = hMultiStreamDecoder;
            }
        }          
            
        for(i =0 ; i< BRAP_MAX_PB_CHAN_IN_ASSOCIATED_GRP ; i++)
        {
            if((psAssociatedChanGroup->hPBCh[i] != NULL))
            {
                psAssociatedChanGroup->hPBCh[i]->hMultiStreamDecoder = hMultiStreamDecoder;
            }
        }        
            
        psAssociatedChanGroup->hMasterChannel = pAssociatedChSettings->hMasterChannel;
        psAssociatedChanGroup->hMultiStreamDecoder = hMultiStreamDecoder;
        
    }

    psAssociatedChanGroup->hRap = hRap;
    hRap->sAssociatedCh[uiGrpId] = *psAssociatedChanGroup;
    *phAssociatedCh = &(hRap->sAssociatedCh[uiGrpId]);
    
error:
    if(BRAP_P_IsPointerValid((void *)psAssociatedChanGroup))
    {
        /* Free the allocated memory */
        BKNI_Free (psAssociatedChanGroup);
    }
    
    BDBG_LEAVE(BRAP_CreateAssociation);
    return err;
}

/***************************************************************************
Summary:
	Ungroups/dissociates a group of audio channels.
	
Description:
	This API ungroups/dissociates a group of audio channels. Note: All the 
	channel handles will still be valid after the execution of this API.
	
Returns:
	BERR_SUCCESS for a successful ungrouping else returns an error. Also,
	the phAssociatedChan is made NULL in this API.
	
See Also:
	BRAP_CreateAssociation.
***************************************************************************/
BERR_Code BRAP_DestroyAssociation(
    BRAP_AssociatedChannelHandle    hAssociatedCh     
                                                /* [in] Handle to the 
                                                   group of audio channels */
	)
{
    int i = 0,j =0;
    BERR_Code ret  = BERR_SUCCESS;

    BDBG_ENTER(BRAP_DestroyAssociation);

    /* Validate input params */
    BDBG_ASSERT(hAssociatedCh);

/* Sanity Check : Check if all the destination added is removed before destroying the association */
    for(j =0;j<BRAP_P_MAX_DST_PER_RAPCH;j++)
    {
        if((BRAP_AudioDst_eMax != hAssociatedCh->sDstDetails[j].sExtDstDetails.eAudioDst)
            ||(BRAP_P_IsPointerValid((void *)hAssociatedCh->sDstDetails[j].hAssociation)))
        {
            BDBG_ERR(("ERROR!!! All the destination of the association must be removed before"
                " destroying the association. Destination %d is not removed "
                ,hAssociatedCh->sDstDetails[j].sExtDstDetails.uDstDetails.sOpDetails.eOutput[0]));
            BDBG_ASSERT(0);
            ret = BERR_TRACE(BERR_INVALID_PARAMETER);
            goto end;
        }
    }

    if(hAssociatedCh->hMasterChannel != NULL)
    {    
        BRAP_P_DestroyMultiStreamDecoderObject(hAssociatedCh->hMultiStreamDecoder);
        for(i =0 ; i< BRAP_MAX_PRI_DEC_CHAN_IN_ASSOCIATED_GRP ; i++)
        {
            if((hAssociatedCh->hPriDecCh[i] != NULL))
            {
                hAssociatedCh->hPriDecCh[i]->hMultiStreamDecoder = NULL;
            }
        }

        for(i =0 ; i< BRAP_MAX_SEC_DEC_CHAN_IN_ASSOCIATED_GRP ; i++)
        {
            if((hAssociatedCh->hSecDecCh[i] != NULL))
            {
                hAssociatedCh->hSecDecCh[i]->hMultiStreamDecoder = NULL;
            }
        }         
    }    

    for(i=0; i<BRAP_MAX_PRI_DEC_CHAN_IN_ASSOCIATED_GRP; i++)
    {
        if(BRAP_P_IsPointerValid((void *)hAssociatedCh->hPriDecCh[i] ))
        {
            for(j=0;j<BRAP_MAX_ASSOCIATED_GROUPS;j++)
            {
                if(hAssociatedCh->hPriDecCh[i]->uiAssociationId[j] == BRAP_INVALID_VALUE)
                    continue;
                if(hAssociatedCh == &(hAssociatedCh->hPriDecCh[i]->hRap->sAssociatedCh[hAssociatedCh->hPriDecCh[i]->uiAssociationId[j]]))
                {
                    hAssociatedCh->hPriDecCh[i]->uiAssociationId[j] = BRAP_INVALID_VALUE;
                    break;
                }        
            }
            hAssociatedCh->hPriDecCh[i]=NULL;
        }
    }

    for(i=0; i<BRAP_MAX_SEC_DEC_CHAN_IN_ASSOCIATED_GRP; i++)
    {
        if(BRAP_P_IsPointerValid((void *)hAssociatedCh->hSecDecCh[i]))
        {
            for(j=0;j<BRAP_MAX_ASSOCIATED_GROUPS;j++)
            {
                if(hAssociatedCh->hSecDecCh[i]->uiAssociationId[j] == BRAP_INVALID_VALUE)
                    continue;
                if(hAssociatedCh == &(hAssociatedCh->hSecDecCh[i]->hRap->sAssociatedCh[hAssociatedCh->hSecDecCh[i]->uiAssociationId[j]]))
                {
                    hAssociatedCh->hSecDecCh[i]->uiAssociationId[j] = BRAP_INVALID_VALUE;
                    break;
                }        
            }
            hAssociatedCh->hSecDecCh[i]=NULL;
        }    
    }

    for(i=0; i<BRAP_MAX_PB_CHAN_IN_ASSOCIATED_GRP; i++)
    {
        if(BRAP_P_IsPointerValid((void *)hAssociatedCh->hPBCh[i]))
        {
            for(j=0;j<BRAP_MAX_ASSOCIATED_GROUPS;j++)
            {
                if(hAssociatedCh->hPBCh[i]->uiAssociationId[j] == BRAP_INVALID_VALUE)
                    continue;
                if(hAssociatedCh == &(hAssociatedCh->hPBCh[i]->hRap->sAssociatedCh[hAssociatedCh->hPBCh[i]->uiAssociationId[j]]))
                {
                    hAssociatedCh->hPBCh[i]->uiAssociationId[j] = BRAP_INVALID_VALUE;
                    break;
                }        
            }
            hAssociatedCh->hPBCh[i]=NULL;
        }    
    }

    for(i=0; i<BRAP_MAX_CAP_CHAN_IN_ASSOCIATED_GRP; i++)
    {
        if(BRAP_P_IsPointerValid((void *)hAssociatedCh->hCapCh[i]))
        {
            for(j=0;j<BRAP_MAX_ASSOCIATED_GROUPS;j++)
            {
                if(hAssociatedCh->hCapCh[i]->uiAssociationId[j] == BRAP_INVALID_VALUE)
                    continue;
                if(hAssociatedCh == &(hAssociatedCh->hCapCh[i]->hRap->sAssociatedCh[hAssociatedCh->hCapCh[i]->uiAssociationId[j]]))
                {
                    hAssociatedCh->hCapCh[i]->uiAssociationId[j] = BRAP_INVALID_VALUE;
                    break;
                }        
            }
            hAssociatedCh->hCapCh[i]=NULL;
        }    
    }
    
    hAssociatedCh = NULL;
    
end:    
    BDBG_LEAVE(BRAP_DestroyAssociation);
    return ret;
}


/***************************************************************************
Summary:
    Creates audio descriptor fade table object.
Description:
    This function creates audio descriptor fade table object. For every audio 
    descriptor channel pair, there needs to be one audio descriptor fade table.
Returns:
    BERR_SUCCESS on success
    Error code on failure
See Also:
    BRAP_DestroyAudioDescriptorFadeTable
    BRAP_ConfigureAudioDescriptorFadeTable
**************************************************************************/
BERR_Code BRAP_CreateAudioDescriptorFadeTable(
    BRAP_Handle hRap, 
    BRAP_AudioDescriptorFadeHandle *phAdFade
    )
{
    BERR_Code ret  = BERR_SUCCESS;
    int i = 0;

    BDBG_ENTER(BRAP_CreateAudioDescriptorFadeTable);

    /* Validate input params */
    BDBG_ASSERT (hRap);
    BDBG_ASSERT (phAdFade);    

    *phAdFade = NULL;
    for (i=0; i<BRAP_P_MAX_AUDIO_DESC_OBJECTS; i++)
    {
        if (true == hRap->sAudioDescObjs[i].bAvailable)
        {
            break;
        }
    }

    if (BRAP_P_MAX_AUDIO_DESC_OBJECTS == i)
    {
        BDBG_ERR(("BRAP_CreateAudioDescriptorFadeTable: Max audio desciptor fade tables reached"));
        ret = BERR_TRACE(BERR_NOT_SUPPORTED);
        goto exit;		
    }

    hRap->sAudioDescObjs[i].ui32PanFadeInterfaceAddr = (uint32_t)BRAP_P_AllocAligned(hRap, sizeof(uint32_t),2, 0);	/* 32 bit aligned*/
    if((void *) BRAP_P_INVALID_DRAM_ADDRESS == (void *)hRap->sAudioDescObjs[i].ui32PanFadeInterfaceAddr)
    {
        BDBG_ERR(("BRAP_CreateAudioDescriptorFadeTable: Unable to Allocate memory for firmware interface address!"));
        ret = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
        goto exit;
    }
    BKNI_Memset((void *)(hRap->sAudioDescObjs[i].ui32PanFadeInterfaceAddr), 0, sizeof(uint32_t));

    hRap->sAudioDescObjs[i].bAvailable = false;
    hRap->sAudioDescObjs[i].hRap = hRap;
    hRap->sAudioDescObjs[i].sExtAudioDescDetails.hPrimaryChannel = NULL;
    hRap->sAudioDescObjs[i].sExtAudioDescDetails.hDescriptorChannel = NULL;
    *phAdFade = &(hRap->sAudioDescObjs[i]);

exit:
    BDBG_LEAVE(BRAP_CreateAudioDescriptorFadeTable);
    return ret;
}



/***************************************************************************
Summary:
    Destroys audio descriptor fade table object.
Description:
    This function destroys audio descriptor fade table object. 
Returns:
    BERR_SUCCESS on success
    Error code on failure
See Also:
    BRAP_CreateAudioDescriptorFadeTable
    BRAP_ConfigureAudioDescriptorFadeTable
**************************************************************************/
BERR_Code BRAP_DestroyAudioDescriptorFadeTable(
    BRAP_AudioDescriptorFadeHandle hAdFade
    )
{
    BERR_Code ret  = BERR_SUCCESS;

    BDBG_ENTER(BRAP_DestroyAudioDescriptorFadeTable);

    /* Validate input params */
    BDBG_ASSERT (hAdFade);

    hAdFade->sExtAudioDescDetails.hPrimaryChannel = NULL;
    hAdFade->sExtAudioDescDetails.hDescriptorChannel = NULL;
    BRAP_P_Free (hAdFade->hRap, (void*)hAdFade->ui32PanFadeInterfaceAddr);
    hAdFade->ui32PanFadeInterfaceAddr = BRAP_RM_P_INVALID_INDEX;
    hAdFade->hRap = NULL;
    hAdFade->bAvailable = true;
   
    BDBG_LEAVE(BRAP_DestroyAudioDescriptorFadeTable);
    return ret;
}

/***************************************************************************
Summary:
    Sets the audio descriptor fade table object.
Description:
    This function sets the audio descriptor fade table object. For a given audio
    descriptor feature, this function identifies which RAP channel is a primary channel
    and which RAP channel is audio descriptor channel.
Returns:
    BERR_SUCCESS on success
    Error code on failure
See Also:
    BRAP_GetAudioDescriptorFadeTable
    BRAP_CreateAudioDescriptorFadeTable
    BRAP_DestroyAudioDescriptorFadeTable
**************************************************************************/
BERR_Code BRAP_SetAudioDescriptorFadeTable(
    BRAP_AudioDescriptorFadeHandle hAdFade,     /* [in] Audio Desc handle */ 
    BRAP_AudioDescriptorFadeTableParams *psParams /* [in] Audio Desc Params */
    )
{
    BERR_Code ret  = BERR_SUCCESS;

    BDBG_ENTER(BRAP_SetAudioDescriptorFadeTable);

    /* Validate input params */
    BDBG_ASSERT (hAdFade);
    BDBG_ASSERT (psParams);

    hAdFade->sExtAudioDescDetails = (*psParams);
  
    BDBG_LEAVE(BRAP_SetAudioDescriptorFadeTable);
    return ret;
}


/***************************************************************************
Summary:
    Gets the audio descriptor fade table object.
Description:
    This function gets the audio descriptor fade table object. For a given audio
    descriptor feature, this function provides which RAP channel is a primary channel
    and which RAP channel is audio descriptor channel. Get returns null if no Set
    has been performed earlier
Returns:
    BERR_SUCCESS on success
    Error code on failure
See Also:
    BRAP_SetAudioDescriptorFadeTable
    BRAP_CreateAudioDescriptorFadeTable
    BRAP_DestroyAudioDescriptorFadeTable
**************************************************************************/
BERR_Code BRAP_GetAudioDescriptorFadeTable(
    BRAP_AudioDescriptorFadeHandle hAdFade,     /* [in] Audio Desc handle */ 
    BRAP_AudioDescriptorFadeTableParams *psParams /* [out] Audio Desc Params */
    )
{
    BERR_Code ret  = BERR_SUCCESS;

    BDBG_ENTER(BRAP_GetAudioDescriptorFadeTable);

    /* Validate input params */
    BDBG_ASSERT (hAdFade);
    BDBG_ASSERT (psParams);

    if (false == hAdFade->bAvailable)
    {
        /* This hAdFade has been allocated and in use */
        (*psParams) = hAdFade->sExtAudioDescDetails;
    }
    else
    {
        /* This hAdFade has been already destroyed */
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
  
    BDBG_LEAVE(BRAP_GetAudioDescriptorFadeTable);
    return ret;
}

/***************************************************************************
Summary:
	API used to configure an output port. This API needs to be called 
	seperately for each output port that is in use by the system.    

Description:
    This API needs to be called atleast once before any channel using this 
    output port can be started.

    Note that an output port may be in use by mutliple channels.
    => an output port is 'stopped' when ALL the channels using it are stopped.
    => an output port is 'running' when one or more channels using it have been 
    started.

    Configuration parameters fall into 2 types:

    1. 'start-time only parameters can be reprogrammed every time an output 
    port is stopped and restarted. These can be modified only when the output 
    port is stopped and will take effect the next time the output port is 
    started.

    2. 'on-the-fly parameters can be reprogrammed any time ie even while the 
    output port is running, without stopping and restarting the channel.

    The configuration Parameters will take effect for ALL channels that are 
    currently using this output port.

    Refer to the description of the output configuration Parameters for details 
    on which of them are start-time Parameters and which can be changed 
    on-the-fly.  

Returns:
	BERR_SUCCESS on success
	BRAP_ERR_OUPUT_PORT_NOT_SUPPORTED if the output port is not supported
	BERR_NOT_SUPPORTED if there is an attempt to change the start-time 
	Parameters while the output port is running - ie if the output port is in 
	use by ANY running channel.

See Also:
	BRAP_GetDefaultOutputConfig
	BRAP_GetCurrentOutputConfig
****************************************************************************/
BERR_Code BRAP_SetOutputConfig (
	BRAP_Handle		            hRap,		    /* [in] RAP device handle */
	const BRAP_OutputPortConfig	*pOpConfig	    /* [in] Output port 
	                                               configuration */
	)
{
    /* Algorithm:
    If ((the Output port hasnt been opened) 
         ||(output port is not running at present))
    {
        Only save the settings in hRap->sOutputSettings.
        On a channel start, the settings will be read from 
        hRap->sOutputSettings, programmed to the appropriate registers, and 
        saved in hOp->sSettings.
    }
    else if (if Output port has already been started)
    {
        if (application is trying to change any of the start-time parameters)
            return error;
        else
            1. If not already muted, Mute the output port
            2. Call the Output port's BRAP_OP_P_XXXHWConfig() function to 
            program the on-the-fly changeable registers and save the new 
            settings in hOp->sSettings.
            3. If we had muted the output port in 1. above, unmute it.
    }
    */

	BERR_Code ret = BERR_SUCCESS;	
    BRAP_OP_P_SpdifSettings sSpdifSettings;  
    BRAP_OP_P_MaiSettings sMaiSettings;      
    BRAP_OP_P_DacSettings sDacSettings;      
    BRAP_OutputPortConfig sOpSettings;
#if (BRAP_7550_FAMILY !=1)
    BRAP_OP_P_I2sSettings sI2sSettings;
#endif
    bool bMute = false;
    unsigned int uiSpdifFmId, uiSpdifFmStreamId, uiCnt;
    uint32_t ui32SpdifChStatusBits[BRAP_SPDIFFM_CBIT_BUFFER_SIZE];
    BRAP_SPDIFFM_P_Handle hSpdifFm; 
    BRAP_ChannelHandle hRapCh = NULL;
    BRAP_DSPCHN_Handle hDspCh = NULL;
    BRAP_OP_P_Handle       	hOp=NULL;
    bool    bValidPll        =false;            
    BRAP_OP_Pll             ePll = BRAP_OP_Pll_eMax,eTempPll = BRAP_OP_Pll_eMax;
    BRAP_OutputPort         eMaiMuxSelect = BRAP_OutputPort_eMax;
#if (BRAP_7550_FAMILY == 1) 
    BRAP_OP_MClkRate    eMClkRate = BRAP_OP_MClKRate_eMax,eTempMClkRate =BRAP_OP_MClKRate_eMax;
#endif
    unsigned int i=0;
    bool    bPllOfStartedOp=false; 
#if ((BRAP_7405_FAMILY == 1) || (BRAP_7550_FAMILY == 1))
    unsigned int ui32RegValue=0;
#endif
	BDBG_ENTER(BRAP_SetOutputConfig);
	/* Assert the function argument(s) */
	BDBG_ASSERT(hRap);
	BDBG_ASSERT(pOpConfig);

    /* Check requested o/p port with the currently supported output ports */
    ret = BRAP_RM_P_IsOutputPortSupported(pOpConfig->eOutputPort);
	if(BERR_SUCCESS != ret)
	{
		BDBG_ERR(("BRAP_SetOutputConfig: eOutputPort(%d) is not supported",
			pOpConfig->eOutputPort));
		return BERR_TRACE(ret);
	}

    /* Check if iDelay is within the acceptable range :: TODO */

    /* Some checks for Mai */
    if(BRAP_OutputPort_eMai == pOpConfig->eOutputPort)
    {
        if((BRAP_OutputPort_eSpdif == 
            pOpConfig->uOutputPortSettings.sMaiSettings.eMaiMuxSelector)||
           (BRAP_OutputPort_eSpdif1 == 
            pOpConfig->uOutputPortSettings.sMaiSettings.eMaiMuxSelector))
        {
            if((BRAP_OP_MaiAudioFormat_eSpdifCompressed != 
                pOpConfig->uOutputPortSettings.sMaiSettings.eMaiAudioFormat) &&
               (BRAP_OP_MaiAudioFormat_eSpdif2Channel != 
                pOpConfig->uOutputPortSettings.sMaiSettings.eMaiAudioFormat))
            {
                BDBG_ERR(("BRAP_SetOutputConfig: For SPDIF as MaiMuxSelector,"
                    " supported MaiAudioFormat are (%d, %d)"
                    " Passed eMaiAudioFormat is %d", 
                    BRAP_OP_MaiAudioFormat_eSpdif2Channel,
                    BRAP_OP_MaiAudioFormat_eSpdifCompressed,
                    pOpConfig->uOutputPortSettings.sMaiSettings.eMaiAudioFormat));
                return BERR_TRACE(BERR_NOT_SUPPORTED);
            }            
        }
#if ((BRAP_7420_FAMILY !=1)&&(BRAP_7550_FAMILY !=1))
        else if((BRAP_OutputPort_eI2s0 == pOpConfig->uOutputPortSettings.sMaiSettings.eMaiMuxSelector) ||
            (BRAP_OutputPort_eI2s1 == pOpConfig->uOutputPortSettings.sMaiSettings.eMaiMuxSelector) ||
            (BRAP_OutputPort_eI2s2 == pOpConfig->uOutputPortSettings.sMaiSettings.eMaiMuxSelector) ||
            (BRAP_OutputPort_eI2s3 == pOpConfig->uOutputPortSettings.sMaiSettings.eMaiMuxSelector))
        {
            if((BRAP_OP_MaiAudioFormat_eSpdif6Channel != 
                pOpConfig->uOutputPortSettings.sMaiSettings.eMaiAudioFormat) &&
               (BRAP_OP_MaiAudioFormat_eSpdif8Channel != 
                pOpConfig->uOutputPortSettings.sMaiSettings.eMaiAudioFormat))
            {
                BDBG_ERR(("BRAP_SetOutputConfig: For I2s_Multi as MaiMuxSelector"
                    "(%d), supported MaiAudioFormat are (%d, %d)"
                    " Passed eMaiAudioFormat is %d", 
                    pOpConfig->uOutputPortSettings.sMaiSettings.eMaiMuxSelector,
                    BRAP_OP_MaiAudioFormat_eSpdif6Channel,
                    BRAP_OP_MaiAudioFormat_eSpdif8Channel,
                    pOpConfig->uOutputPortSettings.sMaiSettings.eMaiAudioFormat));
                return BERR_TRACE(BERR_NOT_SUPPORTED);
            }
        }
#else      
        else if((BRAP_OutputPort_eMaiMulti0== pOpConfig->uOutputPortSettings.sMaiSettings.eMaiMuxSelector) ||
            (BRAP_OutputPort_eMaiMulti1 == pOpConfig->uOutputPortSettings.sMaiSettings.eMaiMuxSelector) ||
            (BRAP_OutputPort_eMaiMulti2 == pOpConfig->uOutputPortSettings.sMaiSettings.eMaiMuxSelector) ||
            (BRAP_OutputPort_eMaiMulti3 == pOpConfig->uOutputPortSettings.sMaiSettings.eMaiMuxSelector))
        {
            if((BRAP_OP_MaiAudioFormat_eSpdif6Channel != 
                pOpConfig->uOutputPortSettings.sMaiSettings.eMaiAudioFormat) &&
               (BRAP_OP_MaiAudioFormat_eSpdif8Channel != 
                pOpConfig->uOutputPortSettings.sMaiSettings.eMaiAudioFormat))
            {
                BDBG_ERR(("BRAP_SetOutputConfig: For Mai_Multi as MaiMuxSelector"
                    "(%d), supported MaiAudioFormat are (%d, %d)"
                    " Passed eMaiAudioFormat is %d", 
                    pOpConfig->uOutputPortSettings.sMaiSettings.eMaiMuxSelector,
                    BRAP_OP_MaiAudioFormat_eSpdif6Channel,
                    BRAP_OP_MaiAudioFormat_eSpdif8Channel,
                    pOpConfig->uOutputPortSettings.sMaiSettings.eMaiAudioFormat));
                return BERR_TRACE(BERR_NOT_SUPPORTED);
            }
        }
#endif        
        else if((BRAP_OutputPort_eI2s5 == pOpConfig->uOutputPortSettings.sMaiSettings.eMaiMuxSelector) ||
            (BRAP_OutputPort_eI2s6 == pOpConfig->uOutputPortSettings.sMaiSettings.eMaiMuxSelector) ||
            (BRAP_OutputPort_eI2s7 == pOpConfig->uOutputPortSettings.sMaiSettings.eMaiMuxSelector) ||
            (BRAP_OutputPort_eI2s8 == pOpConfig->uOutputPortSettings.sMaiSettings.eMaiMuxSelector))
        {
            if((BRAP_OP_MaiAudioFormat_eSpdif6Channel != 
                pOpConfig->uOutputPortSettings.sMaiSettings.eMaiAudioFormat) &&
               (BRAP_OP_MaiAudioFormat_eSpdif8Channel != 
                pOpConfig->uOutputPortSettings.sMaiSettings.eMaiAudioFormat) &&
               (BRAP_OP_MaiAudioFormat_eSpdif8ChCompressed != 
                pOpConfig->uOutputPortSettings.sMaiSettings.eMaiAudioFormat))
            {
                BDBG_ERR(("BRAP_SetOutputConfig: For I2s_Multi1 as MaiMuxSelector"
                    "(%d), supported MaiAudioFormat are (%d, %d, %d)"
                    " Passed eMaiAudioFormat is %d", 
                    pOpConfig->uOutputPortSettings.sMaiSettings.eMaiMuxSelector,
                    BRAP_OP_MaiAudioFormat_eSpdif6Channel,
                    BRAP_OP_MaiAudioFormat_eSpdif8Channel,
                    BRAP_OP_MaiAudioFormat_eSpdif8ChCompressed,
                    pOpConfig->uOutputPortSettings.sMaiSettings.eMaiAudioFormat));
                return BERR_TRACE(BERR_NOT_SUPPORTED);
            }
            if((false == pOpConfig->bHbrEnable)||
               (false == hRap->sOutputSettings
               [pOpConfig->uOutputPortSettings.sMaiSettings.eMaiMuxSelector].bHbrEnable))
            {
                BDBG_ERR(("BRAP_SetOutputConfig: For I2s_Multi1 as MaiMuxSelector"
                    "MAI and its MuxSelect should br running at HBR"));
                return BERR_TRACE(BERR_NOT_SUPPORTED);
            }
                
        }
        else if(BRAP_OutputPort_eMai == pOpConfig->uOutputPortSettings.sMaiSettings.eMaiMuxSelector)
        {
            BDBG_MSG(("MAI Taking data straight from Mixer"));
        }
        else
        {
            BDBG_ERR(("BRAP_SetOutputConfig: Not supported MuxSelector(%d) for Mai",
                pOpConfig->uOutputPortSettings.sMaiSettings.eMaiMuxSelector));
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }

    /* Check for supported bit resolutions.
       For I2S and Flex output, it can be between 16 to 32 bits/sample,
       For other outputs, it can be between 16 to 24 bits/sample */
    if((pOpConfig->uiOutputBitsPerSample > BRAP_P_I2S_MAX_BITS_PER_SAMPLE) ||
       (pOpConfig->uiOutputBitsPerSample < BRAP_P_MIN_BITS_PER_SAMPLE))
    {
        BDBG_ERR(("Output bit resolution %d should be between 16 to 32", 
            pOpConfig->uiOutputBitsPerSample));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    else if(pOpConfig->uiOutputBitsPerSample > BRAP_P_MIXER_MAX_BITS_PER_SAMPLE)
    {
        if((pOpConfig->eOutputPort != BRAP_OutputPort_eI2s0) &&
           (pOpConfig->eOutputPort != BRAP_OutputPort_eI2s1) &&
           (pOpConfig->eOutputPort != BRAP_OutputPort_eI2s2) &&
           (pOpConfig->eOutputPort != BRAP_OutputPort_eI2s3) &&
           (pOpConfig->eOutputPort != BRAP_OutputPort_eI2s4) &&
           (pOpConfig->eOutputPort != BRAP_OutputPort_eI2s5) &&
           (pOpConfig->eOutputPort != BRAP_OutputPort_eI2s6) &&
           (pOpConfig->eOutputPort != BRAP_OutputPort_eI2s7) &&
           (pOpConfig->eOutputPort != BRAP_OutputPort_eI2s8) &&
           (pOpConfig->eOutputPort != BRAP_OutputPort_eFlex))
        {
            /* Only I2S and Flex can have more than 24 bits/sample output.
               So for other outputs return error */
            BDBG_ERR(("Output bit resolution %d should be between 16 to 24 for"
                  " Output Port %d", 
                  pOpConfig->uiOutputBitsPerSample,pOpConfig->eOutputPort));
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }

        /* Also, for compressed mode, on 16 bits/sample is supported */
        if((true == pOpConfig->bCompressed) &&
           (pOpConfig->uiOutputBitsPerSample != 
                BRAP_P_BITS_PER_SAMPLE_FOR_COMPRESSED_DATA)) 
        {
            BDBG_ERR(("Output bit resolution %d should be %d bits/sample"
                  " for Output Port %d carrying compressed data", 
                  pOpConfig->uiOutputBitsPerSample, 
                  BRAP_P_BITS_PER_SAMPLE_FOR_COMPRESSED_DATA,
                  pOpConfig->eOutputPort));
            return BERR_TRACE(BERR_NOT_SUPPORTED);        
        }
    }

    /* Make sure that the application doesnt try to send compressed data on a 
       port that doesn't support it */
    if((true == pOpConfig->bCompressed) &&
       (pOpConfig->eOutputPort != BRAP_OutputPort_eMai) &&
       (pOpConfig->eOutputPort != BRAP_OutputPort_eSpdif) &&
       (pOpConfig->eOutputPort != BRAP_OutputPort_eSpdif1)&&
       (pOpConfig->eOutputPort != BRAP_OutputPort_eI2s5) &&
       (pOpConfig->eOutputPort != BRAP_OutputPort_eI2s6) &&
       (pOpConfig->eOutputPort != BRAP_OutputPort_eI2s7) &&
       (pOpConfig->eOutputPort != BRAP_OutputPort_eI2s8))
    {
        BDBG_ERR(("Output port %d can't carry compressed data", 
            pOpConfig->eOutputPort));
        return BERR_TRACE(BERR_NOT_SUPPORTED);        
    }

    /* Make sure that the application doesnt try to send HBR data on a 
       port that doesn't support it */
    if((true == pOpConfig->bHbrEnable) &&
       (pOpConfig->eOutputPort != BRAP_OutputPort_eMai) &&
       (pOpConfig->eOutputPort != BRAP_OutputPort_eSpdif) &&       
       (pOpConfig->eOutputPort != BRAP_OutputPort_eI2s5) &&
       (pOpConfig->eOutputPort != BRAP_OutputPort_eI2s6) &&
       (pOpConfig->eOutputPort != BRAP_OutputPort_eI2s7) &&
       (pOpConfig->eOutputPort != BRAP_OutputPort_eI2s8))
    {
        BDBG_ERR(("Output port %d can't carry HBR data", 
            pOpConfig->eOutputPort));
        return BERR_TRACE(BERR_NOT_SUPPORTED);        
    }

    /*If Output is in started  condition*/
    if ((BRAP_P_IsPointerValid((void *)hRap->hFmm[0]->hOp[pOpConfig->eOutputPort]))
        && (hRap->hFmm[0]->hOp[pOpConfig->eOutputPort]->uiStartCnt > 0))
    {
        /* Return an error if the application is attempting to change any 
        start-time parameters. 
        Note: In order to keep the Mute time minimum, do not club this check
        with the switch case for programming on-the-fly registers in HW  */
#if 0
        if (pOpConfig->eOutputTimebase 
                != hRap->sOutputSettings[pOpConfig->eOutputPort].eOutputTimebase)
        {
            BDBG_ERR(("BRAP_SetOutputConfig: cannot change Timebase "
            "since the output port is currently running."));
            return BERR_TRACE(BERR_NOT_SUPPORTED);    
        }
#endif        
        sOpSettings = hRap->sOutputSettings[pOpConfig->eOutputPort];
        switch(pOpConfig->eOutputPort)
        {
            case BRAP_OutputPort_eSpdif:
            case BRAP_OutputPort_eSpdif1:
                if((pOpConfig->uOutputPortSettings.sSpdifSettings.eMClkRate !=
                    sOpSettings.uOutputPortSettings.sSpdifSettings.eMClkRate)||
                   (pOpConfig->uOutputPortSettings.sSpdifSettings.ePll !=
                    sOpSettings.uOutputPortSettings.sSpdifSettings.ePll))
                {
                    BDBG_ERR(("BRAP_SetOutputConfig: cannot change Spdif "
                    "MClkRate/Pll since the output port is running"));
                    return BERR_TRACE(BERR_NOT_SUPPORTED);    
                }
                break;              
            case BRAP_OutputPort_eMai:
                if ((pOpConfig->uOutputPortSettings.sMaiSettings.eMaiMuxSelector 
                     != sOpSettings.uOutputPortSettings.sMaiSettings.eMaiMuxSelector)
                    ||(pOpConfig->uOutputPortSettings.sMaiSettings.eMaiAudioFormat 
                     != sOpSettings.uOutputPortSettings.sMaiSettings.eMaiAudioFormat)                      
                    ||(pOpConfig->uOutputPortSettings.sMaiSettings.bSpdifoutput 
                     != sOpSettings.uOutputPortSettings.sMaiSettings.bSpdifoutput))
                {
                    BDBG_ERR(("BRAP_SetOutputConfig: cannot change Mai "
                    "eMaiMuxSelector/eMaiAudioFormat/bSpdifoutput since "
                    "the output port is currently running."));
                    return BERR_TRACE(BERR_NOT_SUPPORTED);    
                }
                break;  
            case BRAP_OutputPort_eDac0:
            case BRAP_OutputPort_eDac1:
            case BRAP_OutputPort_eDac2:                
                /* no start-time only params for Dac. So do nothing here*/              
                break;
            default:
                break; /* do nothing */
        }              

        /* Output bits/sample, bCompressed can't be changed on the fly */
        if((pOpConfig->uiOutputBitsPerSample != sOpSettings.uiOutputBitsPerSample) 
            ||(pOpConfig->bCompressed != sOpSettings.bCompressed))
        {
            BDBG_ERR(("BRAP_SetOutputConfig: cannot change OutputBits/Sample "
                "and/or bCompressed since the output port is currently "
                "running."));
            return BERR_TRACE(BERR_NOT_SUPPORTED);    
        }

	if( pOpConfig->bUseSpdifPackedChanStatusBits
		!= sOpSettings.bUseSpdifPackedChanStatusBits )
	{
		BDBG_ERR(("BRAP_SetOutputConfig: cannot change bUseSpdifPackedChanStatusBits"
			"field since the output port is currently running."));
		return BERR_TRACE(BERR_NOT_SUPPORTED);    
	}
        
        /* Mute the output port if it wasnt already muted */
        /* Note: Can't use SRCCH_Mute 'coz that will affect the other output
                 ports too. Can't use MIXER_Mute 'coz that wont work for 
                 compressed data. */
        BRAP_OP_P_GetMute(hRap, pOpConfig->eOutputPort, &bMute);
        if(false == bMute)
        {
            BRAP_OP_P_SetMute(hRap, pOpConfig->eOutputPort, true);
        }

        /* Program the relevant registers for on-the-fly parameters */
        switch(pOpConfig->eOutputPort)
        {
            case BRAP_OutputPort_eSpdif:
            case BRAP_OutputPort_eSpdif1:
                sSpdifSettings.sExtSettings = 
                    pOpConfig->uOutputPortSettings.sSpdifSettings;
                BRAP_OP_P_SpdifHWConfig(hRap->hFmm[0]->hOp[pOpConfig->eOutputPort], 
                    &sSpdifSettings);
                break;        
            case BRAP_OutputPort_eMai:
                sMaiSettings.sExtSettings = 
                    pOpConfig->uOutputPortSettings.sMaiSettings;
                BRAP_OP_P_MaiHWConfig(hRap->hFmm[0]->hOp[pOpConfig->eOutputPort], 
                    &sMaiSettings);
            break;
            case BRAP_OutputPort_eDac0:
            case BRAP_OutputPort_eDac1:
            case BRAP_OutputPort_eDac2:                
                sDacSettings.sExtSettings = 
                    pOpConfig->uOutputPortSettings.sDacSettings;
                
                if(sDacSettings.sExtSettings.ui32Scale < 0xFFFF)
                    BDBG_WRN(("BRAP_SetOutputConfig: DAC Scale = 0x%x need to be greater than 0xFFFF for better performance",
                        sDacSettings.sExtSettings.ui32Scale));
                
                BRAP_OP_P_DacHWConfig(hRap->hFmm[0]->hOp[pOpConfig->eOutputPort], 
                    &sDacSettings);
                break;
#if(BRAP_7550_FAMILY != 1)                
#if(BRAP_7420_FAMILY != 1) 
            case BRAP_OutputPort_eI2s0:
            case BRAP_OutputPort_eI2s1:
            case BRAP_OutputPort_eI2s2:
            case BRAP_OutputPort_eI2s3:
            case BRAP_OutputPort_eI2s5:
            case BRAP_OutputPort_eI2s6:
            case BRAP_OutputPort_eI2s7:
            case BRAP_OutputPort_eI2s8:
                sI2sSettings.sExtSettings =
                    pOpConfig->uOutputPortSettings.sI2sSettings;
                BRAP_OP_P_I2sHWConfig_Multi(hRap->hFmm[0]->hOp[pOpConfig->eOutputPort],
                                                                &sI2sSettings);
                break;
            case BRAP_OutputPort_eI2s4:
                sI2sSettings.sExtSettings =
                    pOpConfig->uOutputPortSettings.sI2sSettings;
                BRAP_OP_P_I2sHWConfig_Stereo(hRap->hFmm[0]->hOp[pOpConfig->eOutputPort],
                                                                &sI2sSettings);                
                break;
#else
            case BRAP_OutputPort_eI2s0:
            case BRAP_OutputPort_eI2s1:
                sI2sSettings.sExtSettings =
                    pOpConfig->uOutputPortSettings.sI2sSettings;
                BRAP_OP_P_I2sHWConfig_Stereo(hRap->hFmm[0]->hOp[pOpConfig->eOutputPort],
                                                                &sI2sSettings);                
                break;
#endif
#endif
            default:
                break; /* do nothing */            
        }
    	if( true==pOpConfig->bUseSpdifPackedChanStatusBits )
    	{
                if(BRAP_OutputPort_eMai == pOpConfig->eOutputPort)
                {
            		ret = BRAP_RM_P_GetSpdifFmForOpPort( hRap->hRm, 
            			pOpConfig->eOutputPort,
            			hRap->sOutputSettings[pOpConfig->eOutputPort].uOutputPortSettings.sMaiSettings.eMaiMuxSelector,
            			&uiSpdifFmId, &uiSpdifFmStreamId);
                }
                else
                {
        		ret = BRAP_RM_P_GetSpdifFmForOpPort( hRap->hRm, 
        			pOpConfig->eOutputPort, BRAP_OutputPort_eMax, &uiSpdifFmId, &uiSpdifFmStreamId);
                }

    		/* Get the corresponding SPDIFFM handle */
    		hSpdifFm = hRap->hFmm[0]->hSpdifFm[uiSpdifFmId];
    		if(ret != BERR_SUCCESS)
    		{
    			BDBG_ERR(("BRAP_SPDIFFM_P_HWConfig(): GetSpdifFmForOpPort returned err(%d)",ret));
    			return BERR_TRACE(ret);
    		}
    		
    		for(uiCnt = 0; uiCnt < BRAP_SPDIFFM_CBIT_BUFFER_SIZE; uiCnt++)
    		{
    			ui32SpdifChStatusBits[uiCnt] = 0;
    		}
    		/* Second argument to function BRAP_SPDIFFM_P_ProgramCBITBuffer_isr() should have
    		 * C-Bit values initialized in lower 16-bits of the array elements. */
    		ui32SpdifChStatusBits[0] = pOpConfig->sSpdifPackedChanStatusBits.ui32ChStatusBits[0] & 0xFFFF;
    		ui32SpdifChStatusBits[1] = ( pOpConfig->sSpdifPackedChanStatusBits.ui32ChStatusBits[0] & 0xFFFF0000 ) >> 16;
    		ui32SpdifChStatusBits[2] = pOpConfig->sSpdifPackedChanStatusBits.ui32ChStatusBits[1] & 0xFFFF;
    		
        		BKNI_EnterCriticalSection();
    		ret = BRAP_SPDIFFM_P_ProgramCBITBuffer_isr(hSpdifFm, &(ui32SpdifChStatusBits[0] )); 
        		BKNI_LeaveCriticalSection();

    		if(ret != BERR_SUCCESS)
    		{
    			BDBG_ERR(("BRAP_SPDIFFM_P_HWConfig(): ProgramCBITBuffer_isr returned err(%d)",ret));
    			return BERR_TRACE(ret);
    		}
    	}
        else
        {
    		ret = BRAP_P_GetChannelForOpPort(hRap, pOpConfig->eOutputPort, &(hRapCh), &(hDspCh)); 
    		if(ret != BERR_SUCCESS)
    		{
    			BDBG_ERR(("BRAP_SetOutputConfig(): BRAP_P_GetChannelForOpPort returned err(%d)",ret));
    			return BERR_TRACE(ret);
    		}
            if((BRAP_P_IsPointerValid((void *)hDspCh))
                &&(hDspCh->eChState == BRAP_DSPCHN_P_ChState_eStart))
            {
                if((pOpConfig->eOutputPort == BRAP_OutputPort_eSpdif)
                    ||(pOpConfig->eOutputPort == BRAP_OutputPort_eSpdif1)
                   || (pOpConfig->eOutputPort == BRAP_OutputPort_eMai))
                {
                    /* CBIT data for decode/passthru must be programmed by DSP, till MS supports it */                    
                    ret = BRAP_P_UpdateSpdifChanStatusParams(hRap,hDspCh,pOpConfig);
                    if(ret != BERR_SUCCESS)
                    {
                        BDBG_ERR(("BRAP_SetOutputConfig(): BRAP_P_UpdateSpdifChanStatusParams returned err(%d)",ret));
                        return BERR_TRACE(ret);
                    }
                }
            }
            else
            {
                    if(BRAP_OutputPort_eMai == pOpConfig->eOutputPort)
                    {
                		ret = BRAP_RM_P_GetSpdifFmForOpPort( hRap->hRm, 
                			pOpConfig->eOutputPort,
                			hRap->sOutputSettings[pOpConfig->eOutputPort].uOutputPortSettings.sMaiSettings.eMaiMuxSelector,
                			&uiSpdifFmId, &uiSpdifFmStreamId);
                    }
                    else
                    {
                		ret = BRAP_RM_P_GetSpdifFmForOpPort( hRap->hRm, 
                			pOpConfig->eOutputPort, BRAP_OutputPort_eMax, &uiSpdifFmId, &uiSpdifFmStreamId);
                    }

        		/* Get the corresponding SPDIFFM handle */
        		hSpdifFm = hRap->hFmm[0]->hSpdifFm[uiSpdifFmId];
        		if(ret != BERR_SUCCESS)
        		{
        			BDBG_ERR(("BRAP_SetOutputConfig(): GetSpdifFmForOpPort returned err(%d)",ret));
        			return BERR_TRACE(ret);
        		}
			if(BRAP_P_IsPointerValid((void *)hSpdifFm))
			{
                        hSpdifFm->sParams.sChanStatusParams = pOpConfig->sSpdifChanStatusParams;
                        hSpdifFm->sParams.uiCpToggleRate = pOpConfig->uiCpToggleRate;
                        ret = BRAP_SPDIFFM_P_ProgramChanStatusBits(hSpdifFm, pOpConfig->eOutputSR); 
                        if(ret != BERR_SUCCESS)
                        {
                            BDBG_ERR(("BRAP_SetOutputConfig(): BRAP_SPDIFFM_P_ProgramChanStatusBits returned err(%d)",ret));
                            return BERR_TRACE(ret);
                        }
        		}
            }
        }

        /* Unmute the OP port only if we'd muted it earlier in this function */
        if(false == bMute)
        {
            BRAP_OP_P_SetMute(hRap, pOpConfig->eOutputPort, false);
        }
    }
    else /*If Op is in stop condition*/
    {    
        hOp = (BRAP_OP_P_Object *)BKNI_Malloc(sizeof(BRAP_OP_P_Object));
        if ( NULL==hOp)
        {
            ret = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
            goto end;
        }        
        hOp->hRegister = hRap->hRegister;
        hOp->eOutputPort = pOpConfig->eOutputPort;
        {
            switch(pOpConfig->eOutputPort)
            {
#if (BRAP_7550_FAMILY != 1)            
                case BRAP_OutputPort_eSpdif:
                case BRAP_OutputPort_eSpdif1:
                    ePll = pOpConfig->uOutputPortSettings.sSpdifSettings.ePll;
                    bValidPll = true;
                    /*If OutputConfig is being called first time for SPDIF then program MS init Programming*/
#if (BRAP_7405_FAMILY == 1)                
                    if(hRap->bOpSettingsValid[pOpConfig->eOutputPort] == false)
                    {
                        /*Initializing SPDIF cfg*/
                        BRAP_Write32 (hRap->hRegister,BCHP_AUD_FMM_OP_CTRL_SPDIF_CFG_0, 0xb);
                            /*Initializing OP CTRL*/
                        BRAP_Write32 (hRap->hRegister, BCHP_AUD_FMM_OP_CTRL_ENABLE_SET,0x1);         
                    /*Initializing MS Enable*/               
                        ui32RegValue = BRAP_Read32 (hRap->hRegister, 
                                BCHP_AUD_FMM_MS_CTRL_STRM_ENA);
                        ui32RegValue &= ~( BCHP_MASK (AUD_FMM_MS_CTRL_STRM_ENA, STREAM0_ENA) );    
                        ui32RegValue |= (BCHP_FIELD_ENUM (AUD_FMM_MS_CTRL_STRM_ENA, 
                                                          STREAM0_ENA, Enable));                        
                        BRAP_Write32 (hRap->hRegister, BCHP_AUD_FMM_MS_CTRL_STRM_ENA, ui32RegValue);                                                    
                    }                    
#endif                    
                    break;
#else
                case BRAP_OutputPort_eSpdif:
                    eMClkRate = pOpConfig->uOutputPortSettings.sSpdifSettings.eMClkRate;
                    bValidPll = true;
                    /*If OutputConfig is being called first time for SPDIF then program MS init Programming*/
                    if(hRap->bOpSettingsValid[pOpConfig->eOutputPort] == false)
                    {
                        /*Initializing SPDIF cfg*/
                        BRAP_Write32 (hRap->hRegister,BCHP_AUD_FMM_OP_CTRL_SPDIF_CFG_0, 0xb);
                            /*Initializing OP CTRL*/
                        BRAP_Write32 (hRap->hRegister, BCHP_AUD_FMM_OP_CTRL_ENABLE_SET,0x1);    
                    /*Initializing MS Enable*/               
                        ui32RegValue = BRAP_Read32 (hRap->hRegister, 
                                BCHP_AUD_FMM_MS_CTRL_STRM_ENA);
                        ui32RegValue &= ~( BCHP_MASK (AUD_FMM_MS_CTRL_STRM_ENA, STREAM0_ENA) );    
                        ui32RegValue |= (BCHP_FIELD_ENUM (AUD_FMM_MS_CTRL_STRM_ENA, 
                                                          STREAM0_ENA, Enable));                        
                        BRAP_Write32 (hRap->hRegister, BCHP_AUD_FMM_MS_CTRL_STRM_ENA, ui32RegValue);                                                       
                    }                    
                    break;
#endif
                case BRAP_OutputPort_eI2s0:
                case BRAP_OutputPort_eI2s1:
                case BRAP_OutputPort_eI2s2:
                case BRAP_OutputPort_eI2s3:
                case BRAP_OutputPort_eI2s4:
                case BRAP_OutputPort_eI2s5:
                case BRAP_OutputPort_eI2s6:
                case BRAP_OutputPort_eI2s7:
                case BRAP_OutputPort_eI2s8:
                    ePll = pOpConfig->uOutputPortSettings.sI2sSettings.ePll;
                    bValidPll = true;
                    break;

                case BRAP_OutputPort_eDac0:
                case BRAP_OutputPort_eDac1:
                case BRAP_OutputPort_eDac2:
                    ret = BRAP_OP_P_DacSetSampleRate(hRap, pOpConfig->eOutputPort, pOpConfig->eOutputSR);	
                    if(ret != BERR_SUCCESS)
                    {
                        BDBG_ERR(("BRAP_OP_P_ProgramOutputClock: BRAP_OP_P_DacSetSampleRate() returned error (%d)",ret));	
                        goto end;
                        
                    }
                    bValidPll = false;
                    break;
#if (BRAP_7550_FAMILY != 1)
                case BRAP_OutputPort_eMai: 
#if (BRAP_7405_FAMILY == 1)                                   
                    /*If OutputConfig is being called first time for SPDIF then program MS init Programming*/
                    if(hRap->bOpSettingsValid[pOpConfig->eOutputPort] == false)
                    {
                        /*Initializing MAI cfg*/
                        BRAP_Write32 (hRap->hRegister,BCHP_AUD_FMM_OP_CTRL_MAI_CFG, 0x3);
                            /*Initializing OP CTRL*/
                        BRAP_Write32 (hRap->hRegister, BCHP_AUD_FMM_OP_CTRL_ENABLE_SET,0x2);       
                    /*Initializing MS Enable*/               
                        ui32RegValue = BRAP_Read32 (hRap->hRegister, 
                                BCHP_AUD_FMM_MS_CTRL_STRM_ENA);
                        ui32RegValue &= ~( BCHP_MASK (AUD_FMM_MS_CTRL_STRM_ENA, STREAM1_ENA) );    
                        ui32RegValue |= (BCHP_FIELD_ENUM (AUD_FMM_MS_CTRL_STRM_ENA, 
                                                          STREAM1_ENA, Enable));                        
                        BRAP_Write32 (hRap->hRegister, BCHP_AUD_FMM_MS_CTRL_STRM_ENA, ui32RegValue);                         
                    }
#endif                    
                    eMaiMuxSelect = pOpConfig->uOutputPortSettings.sMaiSettings.eMaiMuxSelector;
                    if((BRAP_OutputPort_eSpdif == eMaiMuxSelect )||((BRAP_OutputPort_eSpdif1 == eMaiMuxSelect )))
                    {
                        ePll = pOpConfig->uOutputPortSettings.sSpdifSettings.ePll;
                        bValidPll = true;
                    }
#if (BRAP_7405_FAMILY == 1)
                    else if(BRAP_OutputPort_eMai == eMaiMuxSelect)
                    {
                        ePll = pOpConfig->uOutputPortSettings.sMaiSettings.ePll;
                        bValidPll = true;
                    }
#if (BRAP_7420_FAMILY != 1)                              
                    else if(BRAP_OutputPort_eI2s0 == eMaiMuxSelect)
                    {
                        ePll = pOpConfig->uOutputPortSettings.sI2sSettings.ePll;
                        bValidPll = true;
                    }    
#else          
                    else if(BRAP_OutputPort_eMaiMulti0 == eMaiMuxSelect)
                    {
                        ePll = pOpConfig->uOutputPortSettings.sMaiMultiSettings.ePll;
                        bValidPll = true;
                    }                       
#endif                    
#endif
                    else
                    {
                        BDBG_ERR(("BRAP_OP_P_ProgramOutputClock: Invalid Mux Select"));
                        ret = BERR_TRACE(BERR_NOT_SUPPORTED);
                        goto end;
                        
                    }
                break;			
#if (BRAP_7420_FAMILY == 1)                                        
            case BRAP_OutputPort_eMaiMulti0:
            case BRAP_OutputPort_eMaiMulti1:
            case BRAP_OutputPort_eMaiMulti2:
            case BRAP_OutputPort_eMaiMulti3:                
                ePll = pOpConfig->uOutputPortSettings.sMaiMultiSettings.ePll;
                bValidPll = true;
                break;
#endif                
                    
#else
                case BRAP_OutputPort_eMai: 
                    /*If OutputConfig is being called first time for SPDIF then program MS init Programming*/
                    if(hRap->bOpSettingsValid[pOpConfig->eOutputPort] == false)
                    {
                        /*Initializing MAI cfg*/
                        BRAP_Write32 (hRap->hRegister,BCHP_AUD_FMM_OP_CTRL_MAI_CFG, 0x3);
                            /*Initializing OP CTRL*/
                        BRAP_Write32 (hRap->hRegister, BCHP_AUD_FMM_OP_CTRL_ENABLE_SET,0x2);            
                        /*Initializing MS Enable*/               
                        ui32RegValue = BRAP_Read32 (hRap->hRegister, 
                                BCHP_AUD_FMM_MS_CTRL_STRM_ENA);
                        ui32RegValue &= ~( BCHP_MASK (AUD_FMM_MS_CTRL_STRM_ENA, STREAM1_ENA) );    
                        ui32RegValue |= (BCHP_FIELD_ENUM (AUD_FMM_MS_CTRL_STRM_ENA, 
                                                          STREAM1_ENA, Enable));                        
                        BRAP_Write32 (hRap->hRegister, BCHP_AUD_FMM_MS_CTRL_STRM_ENA, ui32RegValue);                                                      
                    }                    
                    eMaiMuxSelect = pOpConfig->uOutputPortSettings.sMaiSettings.eMaiMuxSelector;
                    if(BRAP_OutputPort_eSpdif == eMaiMuxSelect )
                    {
                        eMClkRate = pOpConfig->uOutputPortSettings.sSpdifSettings.eMClkRate;
                        bValidPll = true;
                    }
                    else if(BRAP_OutputPort_eMai == eMaiMuxSelect)
                    {
                        eMClkRate = pOpConfig->uOutputPortSettings.sMaiSettings.eMClkRate;
                        bValidPll = true;
                    }
                    else if(BRAP_OutputPort_eMaiMulti0 == eMaiMuxSelect)
                    {
                        eMClkRate = pOpConfig->uOutputPortSettings.sMaiMultiSettings.eMClkRate;
                        bValidPll = true;
                    }                       
                    else
                    {
                        BDBG_ERR(("BRAP_OP_P_ProgramOutputClock: Invalid Mux Select"));
                        ret = BERR_TRACE(BERR_NOT_SUPPORTED);
                        goto end;

                    }
                    break;
            case BRAP_OutputPort_eMaiMulti0:
            case BRAP_OutputPort_eMaiMulti1:
            case BRAP_OutputPort_eMaiMulti2:
            case BRAP_OutputPort_eMaiMulti3:                
                eMClkRate = pOpConfig->uOutputPortSettings.sMaiMultiSettings.eMClkRate;
                bValidPll = true;
                break;                    
#endif
            default:	
            BDBG_ERR(("BRAP_OP_P_ProgramOutputClock: Unknown output port(%d)",hOp->eOutputPort));
            ret = BERR_TRACE(BERR_INVALID_PARAMETER);
            goto end;            
            }           
            
            if(bValidPll == true)
            {
                /*Check If any started O/P is using the PLL, If yes then don't program it*/
                for(i = 0; i < BRAP_RM_P_MAX_OUTPUTS; i++)
                {
                    if ((BRAP_P_IsPointerValid((void *)hRap->hFmm[0]->hOp[i]))
                        && (hRap->hFmm[0]->hOp[i]->uiStartCnt > 0))
                    {
                        switch(i)
                        {
                            case BRAP_OutputPort_eSpdif:
                            case BRAP_OutputPort_eSpdif1:
#if (BRAP_7550_FAMILY != 1)
                                eTempPll = hRap->sOutputSettings[i].uOutputPortSettings.sSpdifSettings.ePll;
#else
                                eTempMClkRate = hRap->sOutputSettings[i].uOutputPortSettings.sSpdifSettings.eMClkRate;
#endif
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
                                eTempPll = hRap->sOutputSettings[i].uOutputPortSettings.sI2sSettings.ePll;
                                break;
                                
                            case BRAP_OutputPort_eMai: 
                                eMaiMuxSelect = hRap->sOutputSettings[i].uOutputPortSettings.sMaiSettings.eMaiMuxSelector;
                                if((BRAP_OutputPort_eSpdif == eMaiMuxSelect )||((BRAP_OutputPort_eSpdif1 == eMaiMuxSelect )))
                                {
#if (BRAP_7550_FAMILY != 1)                                
                                    eTempPll = hRap->sOutputSettings[i].uOutputPortSettings.sSpdifSettings.ePll;
#else
                                    eTempMClkRate = hRap->sOutputSettings[i].uOutputPortSettings.sSpdifSettings.eMClkRate;
#endif
                                }
#if (BRAP_7405_FAMILY == 1)
                                else if(BRAP_OutputPort_eMai == eMaiMuxSelect)
                                {
#if (BRAP_7550_FAMILY != 1)                                                                
                                    eTempPll = hRap->sOutputSettings[i].uOutputPortSettings.sMaiSettings.ePll;                                
#else
                                    eTempMClkRate = hRap->sOutputSettings[i].uOutputPortSettings.sMaiSettings.eMClkRate;                                
#endif
                                }
#if (BRAP_7420_FAMILY != 1)&& (BRAP_7550_FAMILY != 1)                            
                                else if(BRAP_OutputPort_eI2s0 == eMaiMuxSelect)
                                {
                                    eTempPll = hRap->sOutputSettings[i].uOutputPortSettings.sI2sSettings.ePll;                                                                
                                }    
#else          
                                else if(BRAP_OutputPort_eMaiMulti0 == eMaiMuxSelect)
                                {
#if (BRAP_7550_FAMILY != 1)                                                                                                
                                    eTempPll = hRap->sOutputSettings[i].uOutputPortSettings.sMaiMultiSettings.ePll;                                                                                                
#else
                                    eTempMClkRate = hRap->sOutputSettings[i].uOutputPortSettings.sMaiMultiSettings.eMClkRate;                                                                                                
#endif
                                }                       
#endif                    
#endif
                                else
                                {
                                    BDBG_ERR(("BRAP_SetOutputConfig: Invalid Mux Select"));
                                    ret = BERR_TRACE(BERR_NOT_SUPPORTED);
                                    goto end;                                      
                                }
                            break;	    
                            
#if (BRAP_7420_FAMILY == 1)                                        
                        case BRAP_OutputPort_eMaiMulti0:
                        case BRAP_OutputPort_eMaiMulti1:
                        case BRAP_OutputPort_eMaiMulti2:
                        case BRAP_OutputPort_eMaiMulti3:     
                            eTempPll = hRap->sOutputSettings[i].uOutputPortSettings.sMaiMultiSettings.ePll;                                                  
                            break;
#endif 
#if (BRAP_7550_FAMILY == 1)                                        
                        case BRAP_OutputPort_eMaiMulti0:
                        case BRAP_OutputPort_eMaiMulti1:
                        case BRAP_OutputPort_eMaiMulti2:
                        case BRAP_OutputPort_eMaiMulti3:     
                            eTempMClkRate = hRap->sOutputSettings[i].uOutputPortSettings.sMaiMultiSettings.eMClkRate;                                                  
                            break;
#endif                             

                        default:
                            break;
                                
                        }
#if (BRAP_7550_FAMILY != 1)                                                                
                        if((ePll==eTempPll)
                            &&(ePll != BRAP_OP_Pll_eMax))
#else
                        if((eMClkRate==eTempMClkRate)
                            &&(eMClkRate != BRAP_OP_MClkRate_e256Fs))
#endif
                        {
                            bPllOfStartedOp = true;
                        }
                    }
                }
                
                if(bPllOfStartedOp == false)
                {
#if (BRAP_7550_FAMILY != 1)                                                  
                    BRAP_OP_P_ProgramPll (hOp,ePll,pOpConfig->eOutputSR);
#else
                    BRAP_OP_P_ProgramNCO (hOp,eMClkRate,pOpConfig->eOutputSR);
#endif
                }
            }                           
        }          
    }
    /* Copy the output port settings to the Audio Device Handle */
	hRap->sOutputSettings[pOpConfig->eOutputPort] = *pOpConfig;

	/* Mark the corresponding output valid flag to TRUE */
	hRap->bOpSettingsValid[pOpConfig->eOutputPort] = true;
end:

    BDBG_LEAVE(BRAP_SetOutputConfig);
    if(hOp != NULL)            
        BKNI_Free(hOp);
	return ret;
}


static BERR_Code BRAP_P_UpdateSpdifChanStatusParams(
	BRAP_Handle		            hRap,		    /* [in] RAP device handle */    
    	BRAP_DSPCHN_Handle                  hDspCh,		    /* [in] DSPCHN handle */
	const BRAP_OutputPortConfig	*pOpConfig	    /* [in] Output port 
	                                               configuration */
)
{
    BRAP_P_ChannelAudioProcessingStage sAudProcessingStage;
    int i =0,j=0,k=0,l=0,m =0;
    BRAP_FWIF_P_FwTaskHandle hFwTask;
    BERR_Code ret = BERR_SUCCESS;	

    BDBG_ENTER(BRAP_P_UpdateSpdifChanStatusParams);
    BDBG_ASSERT(hRap);
    BDBG_ASSERT(hDspCh);    
    BDBG_ASSERT(pOpConfig);    

    /* Find the Tasks using SPDIF Ports. Searching the network associated with Task for
    the Destination having SPDIF0/1 Output port*/
    for(i = 0; i< BRAP_FWIF_P_MAX_FW_TASK_PER_DSPCHN ; i++)
    {
        if(BRAP_P_IsPointerValid((void *)hDspCh->sFwTaskInfo[i].hFwTask))
        {
            for(j = 0; j< BRAP_P_MAX_DST_PER_RAPCH; j++)
            {
                if(!(BRAP_P_BRANCH_VALID(hDspCh->sFwTaskInfo[i].sProcessingNw.sAudProcessingStage,j)))
                {
                    /* No More Valid Branch left */
                    break;
                }
                
                for(k = 0; k< BRAP_MAX_STAGE_PER_BRANCH_SUPPORTED ;k++)
                {
                    sAudProcessingStage = hDspCh->sFwTaskInfo[i].sProcessingNw.sAudProcessingStage[j][k];
                    if(BRAP_P_IsPointerValid((void *)sAudProcessingStage.hAudioProcessing))/*If stage of the network is audio processing */
                    {
                        for(l =0; l < BRAP_P_MAX_DEST_PER_PROCESSING_STAGE ;l++)
                        {
                            if(BRAP_P_IsPointerValid((void *)sAudProcessingStage.hAudioProcessing->hDestHandle[l])) 
                            {
                                if (BRAP_AudioDst_eOutputPort == sAudProcessingStage.hAudioProcessing->hDestHandle[l]->sExtDstDetails.eAudioDst)
                                {
                                    for(m =0; m <BRAP_OutputChannelPair_eMax; m++)
                                    {
                                        if((BRAP_OutputPort_eSpdif == sAudProcessingStage.hAudioProcessing->hDestHandle[l]->sExtDstDetails.uDstDetails.sOpDetails.eOutput[m])
                                            &&(pOpConfig->eOutputPort== sAudProcessingStage.hAudioProcessing->hDestHandle[l]->sExtDstDetails.uDstDetails.sOpDetails.eOutput[m]))
                                        {
                                            hFwTask = hDspCh->sFwTaskInfo[i].hFwTask;
                                            ret = BRAP_P_SendSpdifChanStatusParamsCommand(hDspCh,hRap,hFwTask,pOpConfig,0); 
                                            if(BERR_SUCCESS != ret)
                                            {
                                                BDBG_ERR((" BRAP_P_UpdateSpdifChanStatusParams(): BRAP_P_SendSpdifChanStatusParamsCommand returned err(%d)",ret));
                                                return BERR_TRACE(ret);
                                            }
                                        }
                                        else if(((BRAP_OutputPort_eSpdif1 == sAudProcessingStage.hAudioProcessing->hDestHandle[l]->sExtDstDetails.uDstDetails.sOpDetails.eOutput[m])
                                                    ||(BRAP_OutputPort_eMai== sAudProcessingStage.hAudioProcessing->hDestHandle[l]->sExtDstDetails.uDstDetails.sOpDetails.eOutput[m]))
                                                    && (pOpConfig->eOutputPort== sAudProcessingStage.hAudioProcessing->hDestHandle[l]->sExtDstDetails.uDstDetails.sOpDetails.eOutput[m]))
                                        {
                                            hFwTask = hDspCh->sFwTaskInfo[i].hFwTask;
                                            ret = BRAP_P_SendSpdifChanStatusParamsCommand(hDspCh,hRap,hFwTask,pOpConfig,1);
                                            if(BERR_SUCCESS != ret)                                        
                                            {
                                                BDBG_ERR((" BRAP_P_UpdateSpdifChanStatusParams(): BRAP_P_SendSpdifChanStatusParamsCommand returned err(%d)",ret));
                                                return BERR_TRACE(ret);
                                            }
                                            
                                        }
                                    }
                                }
                            }
                            else/* No more valid destination handle associated with hAudioProcessing */
                            {
                                break;
                            }
                        }
                    }
                    else if(true == sAudProcessingStage.bDecoderStage) /*If stage of the network is Decoder */
                    {
                        for(l =0; l < BRAP_P_MAX_DEST_PER_PROCESSING_STAGE ;l++)
                        {
                            if(BRAP_P_IsPointerValid((void *)sAudProcessingStage.hDestHandle[l])) 
                            {
                                if(BRAP_AudioDst_eOutputPort == sAudProcessingStage.hDestHandle[l]->sExtDstDetails.eAudioDst)
                                {
                                    for(m =0; m <BRAP_OutputChannelPair_eMax; m++)
                                    {
                                        if(BRAP_OutputPort_eSpdif == sAudProcessingStage.hDestHandle[l]->sExtDstDetails.uDstDetails.sOpDetails.eOutput[m])
                                        {
                                            hFwTask = hDspCh->sFwTaskInfo[i].hFwTask;
                                            ret = BRAP_P_SendSpdifChanStatusParamsCommand(hDspCh,hRap,hFwTask,pOpConfig,0);   
                                            if(BERR_SUCCESS != ret)                                        
                                            {
                                                BDBG_ERR((" BRAP_P_UpdateSpdifChanStatusParams(): BRAP_P_SendSpdifChanStatusParamsCommand returned err(%d)",ret));
                                                return BERR_TRACE(ret);
                                            }                                        
                                        }
                                        else if((BRAP_OutputPort_eSpdif1 == sAudProcessingStage.hDestHandle[l]->sExtDstDetails.uDstDetails.sOpDetails.eOutput[m])
                                                    ||(BRAP_OutputPort_eMai == sAudProcessingStage.hDestHandle[l]->sExtDstDetails.uDstDetails.sOpDetails.eOutput[m]))
                                        {
                                            hFwTask = hDspCh->sFwTaskInfo[i].hFwTask;
                                            ret = BRAP_P_SendSpdifChanStatusParamsCommand(hDspCh,hRap,hFwTask,pOpConfig,1); 
                                            if(BERR_SUCCESS != ret)                                        
                                            {
                                                BDBG_ERR((" BRAP_P_UpdateSpdifChanStatusParams(): BRAP_P_SendSpdifChanStatusParamsCommand returned err(%d)",ret));
                                                return BERR_TRACE(ret);
                                            }                                        
                                        }
                                    }
                                }
                            }
                            else /* No more valid destination handle associated with Decoder stage */
                            {
                                break;
                            }
                        }
                    }
                    else if(false == sAudProcessingStage.bCloneStage) /* If at this point stage is neither Processing stage, nor DecodeStage , nor Clone Stage then Break the loop running on stages */
                    {
                        break;
                    }
                }
            }
        }
    }

    BDBG_LEAVE(BRAP_P_UpdateSpdifChanStatusParams);
    return ret;
}


static BERR_Code BRAP_P_SendSpdifChanStatusParamsCommand(
    	BRAP_DSPCHN_Handle                  hDspCh,		    /* [in] DSPCHN handle */
        BRAP_Handle		                    hRap,		    /* [in] RAP device handle */    
        BRAP_FWIF_P_FwTaskHandle     hFwTask,   /* [in] Task handle */
        const BRAP_OutputPortConfig     *pOpConfig,	    /* [in] Output port  configuration */
        uint32_t                                  ui32SpdifPortIndex       /*Spdif0 : 0 , Spdif1: 1 */
    )
{
    BRAP_Fwif_P_FwBufInfo  *psFwBufInfo;
    BRAP_DSP_Handle hDsp;
    BRAP_P_MsgType      eMsgType;
    BRAP_FWIF_P_Command sCommand;
    BRAP_FWIF_P_Response sRsp;
    unsigned int uiConfigBufAddr;
    BERR_Code ret = BERR_SUCCESS;

    BDBG_ENTER(BRAP_P_SendSpdifChanStatusParamsCommand);
    BDBG_ASSERT(hRap);
    BDBG_ASSERT(pOpConfig);    

    BKNI_Memset((void *)&sRsp,0,sizeof(BRAP_FWIF_P_Response));
    if(hFwTask->bStopped == true)
    {
        if(pOpConfig->eOutputPort == BRAP_OutputPort_eSpdif)
            hDspCh->sDspAudioParams.sSpdifChStatusParams[0] = pOpConfig->sSpdifChanStatusParams;
        else if(pOpConfig->eOutputPort == BRAP_OutputPort_eMai)
            hDspCh->sDspAudioParams.sSpdifChStatusParams[1] = pOpConfig->sSpdifChanStatusParams;
        
        BDBG_MSG(("DspChn is already stopped, Storing the Spdif Channel status params in handle"));
        goto end;
    }
    
    psFwBufInfo = &(hRap->sMemInfo.sSpdifStatusBitBufInfo);
    hDsp = hFwTask->hDsp;
    uiConfigBufAddr = hFwTask->sCitOutput.sSpdifUserConfigAddr[ui32SpdifPortIndex].ui32DramBufferAddress;
    
    BRAP_FWIF_P_InitSpdifChanStatusParams(( BRAP_OP_SpdifChanStatusParams   *)&(pOpConfig->sSpdifChanStatusParams),
                                                                            psFwBufInfo->ui32BaseAddr,psFwBufInfo->ui32Size);

    /* Create Change-Config command */
    sCommand.sCommandHeader.ui32CommandID = BRAP_FWIF_P_ALGO_PARAMS_CFG_COMMAND_ID;
    sCommand.sCommandHeader.ui32CommandCounter = hRap->uiCommandCounter++;
    sCommand.sCommandHeader.ui32TaskID =  hFwTask->uiTaskId;
    sCommand.sCommandHeader.eResponseType = BRAP_FWIF_P_ResponseType_eResponseRequired;
    
    BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(psFwBufInfo ->ui32BaseAddr), 
        &(sCommand.uCommand.sCfgChange.ui32HostConfigParamBuffAddr));	
    
    BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(uiConfigBufAddr), 
        &(sCommand.uCommand.sCfgChange.ui32DspConfigParamBuffAddr));	
    sCommand.uCommand.sCfgChange.ui32SizeInBytes = psFwBufInfo->ui32Size;

    hFwTask->uiLastEventType = sCommand.sCommandHeader.ui32CommandID;
    BRAP_P_EventReset(hDsp,BRAP_GET_TASK_INDEX(hFwTask->uiTaskId));
    ret = BRAP_FWIF_P_SendCommand(hDsp->hCmdQueue, &sCommand,hRap,hFwTask);
    if(BERR_SUCCESS != ret)
    {
        if((hRap->bWatchdogTriggered == false)
        &&(hFwTask->bStopped == false))
        {
        BDBG_ERR(("BRAP_P_SendSpdifChanStatusParamsCommand: CFG_Command failed!"));
        return BERR_TRACE(ret);
    }
        else
            ret = BERR_SUCCESS;    
    }


    /* Wait for Ack_Response_Received event w/ timeout */
    ret = BRAP_P_EventWait(hDsp, BRAP_DSPCHN_P_EVENT_TIMEOUT_IN_MS,BRAP_GET_TASK_INDEX(hFwTask->uiTaskId));
    if(BERR_TIMEOUT == ret)
    {
        if((hRap->bWatchdogTriggered == false))
        {
            /* Please note that, If the code reaches at this point then there is a potential Bug in Fw 
            code which needs to be debugged. However Watchdog is being triggered to recover the system*/            
            BDBG_WRN(("BRAP_P_SendSpdifChanStatusParamsCommand: CFG_Command ACK timeout! Triggering Watchdog"));
#if 0                
            BDBG_ASSERT(0);                
#endif
            BRAP_Write32(hDsp->hRegister, BCHP_AUD_DSP_INTH0_R5F_SET+ hDsp->ui32Offset,0x1);
            hRap->bWatchdogTriggered  = true;
#if 0                
            err = BERR_TRACE(err);
            goto error;
#endif                 
            ret = BERR_SUCCESS;              
        }
        else
            ret = BERR_SUCCESS;              
    }
        
    eMsgType = BRAP_P_MsgType_eSyn;
    if((hRap->bWatchdogTriggered == false)
    &&(hFwTask->bStopped == false))
    {
    ret = BRAP_FWIF_P_GetMsg(hFwTask->hSyncMsgQueue, (void *)&sRsp, eMsgType);
    }
    if(BERR_SUCCESS != ret)
    {
        if((hRap->bWatchdogTriggered == false)
            &&(hFwTask->bStopped == false))
        {
        BDBG_ERR(("BRAP_P_SendSpdifChanStatusParamsCommand: Unable to read ACK!"));
        return BERR_TRACE(ret);
    }
        else
            ret = BERR_SUCCESS;
    }

    if((sRsp.sCommonAckResponseHeader.eStatus != BERR_SUCCESS)||
        (sRsp.sCommonAckResponseHeader.ui32ResponseID != BRAP_FWIF_P_ALGO_PARAMS_CFG_RESPONSE_ID)||
        (sRsp.sCommonAckResponseHeader.ui32ResponseCounter!=sCommand.sCommandHeader.ui32CommandCounter)||
        (sRsp.sCommonAckResponseHeader.ui32TaskID != hFwTask->uiTaskId))
    {
        if((hRap->bWatchdogTriggered == false)
            &&(hFwTask->bStopped == false))
        {
        BDBG_ERR(("sRsp.sCommonAckResponseHeader.eStatus =%d",sRsp.sCommonAckResponseHeader.eStatus));
        BDBG_ERR(("BRAP_P_SendSpdifChanStatusParamsCommand: CFG_Command response not received successfully!"));
        return BERR_TRACE(sRsp.sCommonAckResponseHeader.eStatus);
    }
        else
            ret = BERR_SUCCESS;        
    }
end:
    BDBG_LEAVE(BRAP_P_SendSpdifChanStatusParamsCommand);
    return ret;
}
/***************************************************************************
Summary:
	Get default output port configuration

Description:
	This function gets the default output port configuration 
	
Returns:
	BERR_SUCCESS if successful else error value

See Also:
	BRAP_SetOutputConfig
	BRAP_GetCurrentOutputConfig
****************************************************************************/
BERR_Code BRAP_GetDefaultOutputConfig (
	BRAP_Handle	            hRap,	        /* [in] RAP device handle */
	BRAP_OutputPort         eOutput,        /* [in] Output port */
	BRAP_OutputPortConfig	*pOpConfig      /* [out] Default configuration for
	                                           the output port */
	)
{
	BERR_Code				ret = BERR_SUCCESS;
	BRAP_OP_P_MaiSettings	sMaiSettings;
	BRAP_OP_P_SpdifSettings	sSpdifSettings;
	BRAP_OP_P_I2sSettings	sI2sSettings;
	BRAP_OP_P_DacSettings	sDacSettings;
    BRAP_SPDIFFM_P_Settings sSpdifFmSettings;
    BRAP_SPDIFFM_P_Params   sSpdifFmParams;

	BDBG_ENTER(BRAP_GetDefaultOutputConfig);

	/* Validate input parameters. */
	BDBG_ASSERT(hRap);
	BDBG_ASSERT(pOpConfig);
    BSTD_UNUSED(hRap);

    BDBG_MSG(("BRAP_GetDefaultOutputConfig: output port = %d ", eOutput));

    /* Output port type */
    pOpConfig->eOutputPort = eOutput;

    /* Output sampling rate */
    pOpConfig->eOutputSR = BAVC_AudioSamplingRate_e48k;     

    /* Output bits per sample */
    pOpConfig->uiOutputBitsPerSample = BRAP_MIXER_P_DEFAULT_BITS_PER_SAMPLE;

    /* bCompressed */
    pOpConfig->bCompressed = false;
#if(BRAP_7405_FAMILY == 1)
/* Output port Timebase */
    pOpConfig->eOutputTimebase = BAVC_Timebase_e0;     
#endif

    /* Spdiffm Settings */        
    ret = BRAP_SPDIFFM_P_GetDefaultSettings(&sSpdifFmSettings);
    if(BERR_SUCCESS != ret)
    {
        return BERR_TRACE(ret);
    }
    pOpConfig->sSpdiffmSettings = sSpdifFmSettings.sExtSettings;

    /* Spdiffm Channel Status Params */
    ret = BRAP_SPDIFFM_P_GetDefaultParams(&sSpdifFmParams);
    if(BERR_SUCCESS != ret)
    {
        return BERR_TRACE(ret);
    }
    pOpConfig->sSpdifChanStatusParams = sSpdifFmParams.sChanStatusParams;
    pOpConfig->bUseSpdifPackedChanStatusBits = sSpdifFmParams.bUseSpdifPackedChanStatusBits;
    pOpConfig->sSpdifPackedChanStatusBits = sSpdifFmParams.sSpdifPackedChanStatusBits;
    pOpConfig->uiCpToggleRate = sSpdifFmParams.uiCpToggleRate;

    /* Get the output port default settings and return them */
	switch(eOutput)
	{
		case BRAP_OutputPort_eSpdif:
		case BRAP_OutputPort_eSpdif1:
			ret = BRAP_OP_P_GetDefaultSettings(eOutput, &sSpdifSettings);
            if(BERR_SUCCESS != ret)
            {
                return BERR_TRACE(ret);
            }
            pOpConfig->uOutputPortSettings.sSpdifSettings = 
                        sSpdifSettings.sExtSettings;
            /* SPDIF needs SPDIFFM. Use the default SPDIFFM settings */
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
			ret = BRAP_OP_P_GetDefaultSettings(eOutput, &sI2sSettings);
            if(BERR_SUCCESS != ret)
            {
                return BERR_TRACE(ret);
            }
			pOpConfig->uOutputPortSettings.sI2sSettings =
			            				sI2sSettings.sExtSettings;
            /* SPDIFFM is not required. So change the default SPDIFFM settings*/
            pOpConfig->sSpdiffmSettings.bEnableDither = false;
            pOpConfig->sSpdiffmSettings.eBurstType = 
                                BRAP_SPDIFFM_BurstType_eNone;
			break;
            
		case BRAP_OutputPort_eMai:          
			ret = BRAP_OP_P_GetDefaultSettings(eOutput, &sMaiSettings);
            if(BERR_SUCCESS != ret)
            {
                return BERR_TRACE(ret);
            }
			pOpConfig->uOutputPortSettings.sMaiSettings =
							            sMaiSettings.sExtSettings;
            /* Mai needs SPDIFFM. Use the default SPDIFFM settings */
			break;
            
		case BRAP_OutputPort_eDac0:
		case BRAP_OutputPort_eDac1:
        case BRAP_OutputPort_eDac2:
			ret = BRAP_OP_P_GetDefaultSettings(eOutput, &sDacSettings);
            if(BERR_SUCCESS != ret)
            {
                return BERR_TRACE(ret);
            }
			pOpConfig->uOutputPortSettings.sDacSettings =
            							sDacSettings.sExtSettings;
            /* SPDIFFM is not required. So change the default SPDIFFM settings*/
            pOpConfig->sSpdiffmSettings.bEnableDither = false;
            pOpConfig->sSpdiffmSettings.eBurstType = 
                                BRAP_SPDIFFM_BurstType_eNone;
            break;
	case BRAP_OutputPort_eMaiMulti0:    
	case BRAP_OutputPort_eMaiMulti1:    
	case BRAP_OutputPort_eMaiMulti2:                     
	case BRAP_OutputPort_eMaiMulti3:   
        	ret = BRAP_OP_P_GetDefaultSettings(eOutput, &sMaiSettings);    
			pOpConfig->uOutputPortSettings.sMaiMultiSettings =
							            sMaiSettings.sExtSettings;            
            break;
        default:
            BDBG_ERR(("BRAP_GetDefaultOutputConfig: Invalid Op %d", eOutput));
            return BERR_TRACE(BERR_INVALID_PARAMETER);
    }/* switch */

    BDBG_MSG(("BRAP_GetDefaultOutputConfig: bDither(%d) eBurstType(%d)",
        pOpConfig->sSpdiffmSettings.bEnableDither,
        pOpConfig->sSpdiffmSettings.eBurstType));    
	
#if (BRAP_INDEP_OP_DELAY_SUPPORTED == 1)
/*    pOpConfig->bIndpDlyRqd = false;*/
    pOpConfig->iDelay = 0;
#endif

    /* bHbrEnable */
    pOpConfig->bHbrEnable = false;
    
	BDBG_LEAVE(BRAP_GetDefaultOutputConfig);
    return ret;
}

/***************************************************************************
Summary:
	Get current output port configuration

Description:
	This function gets the current output port configuration. If the caller has 
	not configured the output port before, this API will return error. 
 	
Returns:
	BERR_SUCCESS if successful else error value

See Also:
	BRAP_SetOutputConfig
	BRAP_GetDefaultOutputConfig
****************************************************************************/
BERR_Code BRAP_GetCurrentOutputConfig (
	BRAP_Handle	            hRap,	        /* [in] RAP device handle */
	BRAP_OutputPort         eOutput,        /* [in] Output port */
	BRAP_OutputPortConfig	*pOpConfig      /* [out] Current configuration for
	                                           the output port */
    )
{
    BERR_Code ret = BERR_SUCCESS;
    BDBG_ENTER(BRAP_GetCurrentOutputConfig);

    /* Assert the function argument(s) */
    BDBG_ASSERT(hRap);
    BDBG_ASSERT(pOpConfig);

    ret = BRAP_RM_P_IsOutputPortSupported(eOutput);
    if(BERR_SUCCESS != ret)
    {
    	BDBG_ERR(("BRAP_GetCurrentOutputConfig: OutputPort(%d) is not supported",
    		eOutput));
    	return BERR_TRACE(ret);
    }
	
	/* Return the output configuration, if it exists, otherwise return error */
	if(true == hRap->bOpSettingsValid[eOutput])
	{
		*pOpConfig = hRap->sOutputSettings[eOutput];
	}
	else
	{
		return BERR_TRACE(BRAP_ERR_OUTPUT_NOT_CONFIGURED);
	}

	BDBG_LEAVE(BRAP_GetCurrentOutputConfig);
	return ret;
}
#if (BRAP_7550_FAMILY !=1)

/***************************************************************************
Summary:
    This private routine forms the RBUF resource request, allocates them 
    and open the allocated RBUFs.
****************************************************************************/
/* ToDo: Destinations (Output Port and Ringbuffer) are properties of an Association
         so allocate and open of these need to be handled per assoc not per channel.
        Pending for Output Port */

static BERR_Code BRAP_P_AllocateAndOpenRbuf(
    BRAP_AssociatedChannelHandle    hAssociatedCh,  /* [in] Association Handle */
    BRAP_DstDetails                 *pDstDetails    /* [in] Details of the destination 
                                                       to be added to the channel group */ 
    )
{
    BERR_Code                   ret = BERR_SUCCESS;
    unsigned int                i = 0, uiAssocChId = 0, uiDst = 0;
    BRAP_RM_P_ResrcReq          *psResrcReq = NULL;
    BRAP_RM_P_ResrcGrant        *psResrcGrant = NULL;
    BRAP_Handle                 hRap = hAssociatedCh->hRap;
    BRAP_RBUF_P_Settings        sRbufSettings;
    BRAP_DSTCH_P_Settings       sDstChSettings;
    BRAP_ChannelHandle          hAssocRapCh[BRAP_MAX_ASSOCIATED_CHANNELS_IN_GRP];

    BDBG_ENTER(BRAP_P_AllocateAndOpenRbuf);
    
    /* Malloc large structures */
    psResrcReq = hRap->sOpenTimeMallocs.psResrcReq;
    if( NULL==psResrcReq)
    {
		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }
    BKNI_Memset(psResrcReq,0,sizeof(BRAP_RM_P_ResrcReq));
    
    psResrcGrant = hRap->sOpenTimeMallocs.psResrcGrant;
    if( NULL==psResrcGrant)
    {
    	return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }
    BKNI_Memset(psResrcGrant,0,sizeof(BRAP_RM_P_ResrcGrant));    
    
    /* Initialize resource Request structure with Invalid values */
    BRAP_RM_P_InitResourceReq(psResrcReq);

    /* Initialize the Array of Channel Handles */
    for(uiAssocChId = 0; uiAssocChId < BRAP_MAX_ASSOCIATED_CHANNELS_IN_GRP; uiAssocChId++)
    {
        hAssocRapCh[uiAssocChId] = NULL;
    }
            
    /* Check if input is from SPDIF Compressed Input. If so then only one 
       channel pair should be valid */
    if(true == pDstDetails->uDstDetails.sRBufDetails.bCompress)
    {
        /* Prepare the resource request */
        if(pDstDetails->uDstDetails.sRBufDetails.eBufDataMode[BRAP_OutputChannelPair_eLR] 
                                                    != BRAP_BufDataMode_eMaxNum)
        {
            psResrcReq->sDstChReq[BRAP_OutputChannelPair_eLR].bAllocate = true;
            psResrcReq->sRbufReq[BRAP_OutputChannelPair_eLR].bAllocate = true;
            psResrcReq->sRbufReq[BRAP_OutputChannelPair_eLR].eBufDataMode = 
                          pDstDetails->uDstDetails.sRBufDetails.eBufDataMode[i];
        }
    }
    else
    {
        /* Prepare the resource request */
        for(i=0;i<BRAP_RM_P_MAX_OP_CHANNEL_PAIRS;i++)
        {
            if(pDstDetails->uDstDetails.sRBufDetails.eBufDataMode[i] 
                                                        != BRAP_BufDataMode_eMaxNum)
            {
                psResrcReq->sDstChReq[i].bAllocate = true;
                psResrcReq->sRbufReq[i].bAllocate = true;
                psResrcReq->sRbufReq[i].eBufDataMode = 
                        pDstDetails->uDstDetails.sRBufDetails.eBufDataMode[i];
            }
        }
    }

    /* Call resource manager to allocate required resources. */
    ret = BRAP_RM_P_AllocateResources(hRap->hRm, psResrcReq, psResrcGrant);
    if(ret != BERR_SUCCESS) 
    {
        BDBG_ERR(("BRAP_P_AllocateAndOpenRbuf: Resource Manager could not "
                  "allocate requested resources."));
        ret = BERR_TRACE(ret);
	    goto error;
    }

    for (i=0;i<BRAP_RM_P_MAX_OP_CHANNEL_PAIRS;i++)
    {
        /* Open Destination Channel */
        if(BRAP_RM_P_INVALID_INDEX != psResrcGrant->uiDstChId[i])
        {
            ret = BRAP_DSTCH_P_Open(hRap->hFmm[0],
                                    &(hRap->hFmm[0]->hDstCh[psResrcGrant->uiDstChId[i]]),
                                    psResrcGrant->uiDstChId[i], 
                                    &sDstChSettings);
            if(BERR_SUCCESS != ret)
            {
            		ret = BERR_TRACE(ret);
        			goto error;					
            }
            BDBG_MSG(("Destination Channel %d opened",psResrcGrant->uiDstChId[i]));
        }
    }

    /* Collate all the valid channel handles in the association into one array, hAssocRapCh[] */
    for(i=0; i<BRAP_MAX_PRI_DEC_CHAN_IN_ASSOCIATED_GRP; i++)
    {
        if(BRAP_P_IsPointerValid((void *)hAssociatedCh->hPriDecCh[i]))
        {
            for(uiAssocChId=0; uiAssocChId<BRAP_MAX_ASSOCIATED_CHANNELS_IN_GRP; uiAssocChId++)
            {
                if(!(BRAP_P_IsPointerValid((void *)hAssocRapCh[uiAssocChId])))
                {
                    hAssocRapCh[uiAssocChId] = hAssociatedCh->hPriDecCh[i];
                    break;
                }
            }
            if(uiAssocChId >= BRAP_MAX_ASSOCIATED_CHANNELS_IN_GRP)
            {
                BDBG_ERR(("BRAP_P_AllocateAndOpenRbuf: No free Index in hAssocRapCh=%x"
                    "to store the Primary Decode Channel Handle", hAssocRapCh));
                ret = BERR_TRACE(BERR_NOT_SUPPORTED);
        	    goto error;
            }
        }
    }

    for(i=0; i<BRAP_MAX_SEC_DEC_CHAN_IN_ASSOCIATED_GRP; i++)
    {
        if(BRAP_P_IsPointerValid((void *)hAssociatedCh->hSecDecCh[i]))
        {
            for(uiAssocChId=0; uiAssocChId<BRAP_MAX_ASSOCIATED_CHANNELS_IN_GRP; uiAssocChId++)
            {
                if(!(BRAP_P_IsPointerValid((void *)hAssocRapCh[uiAssocChId])))
                {
                    hAssocRapCh[uiAssocChId] = hAssociatedCh->hSecDecCh[i];
                    break;
                }
            }
            if(uiAssocChId >= BRAP_MAX_ASSOCIATED_CHANNELS_IN_GRP)
            {
                BDBG_ERR(("BRAP_P_AllocateAndOpenRbuf: No free Index in hAssocRapCh=%x"
                    "to store the Secondary Decode Channel Handle", hAssocRapCh));
                ret = BERR_TRACE(BERR_NOT_SUPPORTED);
        	    goto error;
            }            
        }
    }

    for(i=0; i<BRAP_MAX_PB_CHAN_IN_ASSOCIATED_GRP; i++)
    {
        if(BRAP_P_IsPointerValid((void *)hAssociatedCh->hPBCh[i]))
        {
            for(uiAssocChId=0; uiAssocChId<BRAP_MAX_ASSOCIATED_CHANNELS_IN_GRP; uiAssocChId++)
            {
                if(!(BRAP_P_IsPointerValid((void *)hAssocRapCh[uiAssocChId])))
                {
                    hAssocRapCh[uiAssocChId] = hAssociatedCh->hPBCh[i];
                    break;
                }
            }
            if(uiAssocChId >= BRAP_MAX_ASSOCIATED_CHANNELS_IN_GRP)
            {
                BDBG_ERR(("BRAP_P_AllocateAndOpenRbuf: No free Index in hAssocRapCh=%x"
                    "to store the Playback Channel Handle", hAssocRapCh));
                ret = BERR_TRACE(BERR_NOT_SUPPORTED);
        	    goto error;
            }            
        }
    }

    for(i=0; i<BRAP_MAX_CAP_CHAN_IN_ASSOCIATED_GRP; i++)
    {
        if(BRAP_P_IsPointerValid((void *)hAssociatedCh->hCapCh[i]))
        {
            for(uiAssocChId=0; uiAssocChId<BRAP_MAX_ASSOCIATED_CHANNELS_IN_GRP; uiAssocChId++)
            {
                if(!(BRAP_P_IsPointerValid((void *)hAssocRapCh[uiAssocChId])))
                {
                    hAssocRapCh[uiAssocChId] = hAssociatedCh->hCapCh[i];
                    break;
                }
            }
            if(uiAssocChId >= BRAP_MAX_ASSOCIATED_CHANNELS_IN_GRP)
            {
                BDBG_ERR(("BRAP_P_AllocateAndOpenRbuf: No free Index in hAssocRapCh=%x"
                    "to store the Capture Channel Handle", hAssocRapCh));
                ret = BERR_TRACE(BERR_NOT_SUPPORTED);
        	    goto error;
            }            
        }
    }

    if(!(BRAP_P_IsPointerValid((void *)hAssocRapCh[0])))
    {
        BDBG_ERR(("BRAP_P_AllocateAndOpenRbuf: No valid channel handle in the Association Handle %x", 
                    hAssociatedCh));
        ret = BERR_TRACE(BERR_NOT_SUPPORTED);
	    goto error;
    }
    
    /* open Rbufs */
    for(i = 0; i < BRAP_RM_P_MAX_OP_CHANNELS; i++)
    {    
        if(psResrcGrant->uiRbufId[i] != BRAP_RM_P_INVALID_INDEX)
        {
            /* Get default settings */
            ret = BRAP_RBUF_P_GetDefaultSettings(&sRbufSettings);
            if(BERR_SUCCESS != ret)
            {   
        		ret = BERR_TRACE(ret);
			    goto error;					
	        }

            /* Modify settings (if any) */
            sRbufSettings.bProgRdWrRBufAddr = false;
            sRbufSettings.bRbufOfClonedPort = false;
#if (BRAP_OPEN_TIME_RBUF_ALLOCATION==1)
  			BRAP_P_GetRbufFromPool(hAssocRapCh[0],NULL,NULL,true,0,0,&sRbufSettings,BRAP_INVALID_VALUE);

            if(0 == (i%2))
            {
                sRbufSettings.sExtSettings.uiWaterMark = 
                    pDstDetails->uDstDetails.sRBufDetails.sLtOrSnglRbufSettings[i/2].uiWaterMark;
            }
            else
            {
                sRbufSettings.sExtSettings.uiWaterMark = 
                    pDstDetails->uDstDetails.sRBufDetails.sRtRbufSettings[i/2].uiWaterMark;
            }

#else
            if(0 == (i%2))
            {
                sRbufSettings.sExtSettings = 
                pDstDetails->uDstDetails.sRBufDetails.sLtOrSnglRbufSettings[i/2];
            }
            else
            {
                sRbufSettings.sExtSettings = 
                pDstDetails->uDstDetails.sRBufDetails.sRtRbufSettings[i/2];
            }
#endif
            
            /* Open Rbuf handle */
            ret = BRAP_RBUF_P_Open( hRap->hFmm[0],
                                    &(hRap->hFmm[0]->hRBuf[psResrcGrant->uiRbufId[i]]),
                                    psResrcGrant->uiRbufId[i],
                                    &sRbufSettings);
            if(BERR_SUCCESS != ret)
            {   
        		ret = BERR_TRACE(ret);
			    goto error;
	        }
            
            BDBG_MSG(("BRAP_P_AllocateAndOpenRbuf: Ring buffer %d opened",psResrcGrant->uiRbufId[i]));
        }
        
    } /* for i */

	/* Storing the granted resources */
    if(BRAP_AudioDst_eRingBuffer == pDstDetails->eAudioDst) 
    {
	    for(i=0; i<BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; i++)
	    {
		    pDstDetails->uDstDetails.sRBufDetails.uiRBufId[i*2] = psResrcGrant->uiRbufId[i*2];
		    pDstDetails->uDstDetails.sRBufDetails.uiRBufId[i*2+1] = psResrcGrant->uiRbufId[i*2+1];
		    pDstDetails->uDstDetails.sRBufDetails.uiDstChId[i] = psResrcGrant->uiDstChId[i];
	    }
    }
        
    /* Save pDstDetails in all the channel handles of the association */
    for(uiAssocChId=0; uiAssocChId<BRAP_MAX_ASSOCIATED_CHANNELS_IN_GRP; uiAssocChId++)
    {    
        if(BRAP_P_IsPointerValid((void *)hAssocRapCh[uiAssocChId]))
        {
            for(uiDst=0; uiDst<BRAP_P_MAX_DST_PER_RAPCH; uiDst++)
            {
                if(!(BRAP_P_IsPointerValid((void *)hAssocRapCh[uiAssocChId]->pDstDetails[uiDst])))
                {
                    hAssocRapCh[uiAssocChId]->pDstDetails[uiDst] = pDstDetails;
                    break;
                }     
            }        
            if(uiDst >= BRAP_P_MAX_DST_PER_RAPCH)
            {
                ret = BERR_TRACE(BERR_NOT_SUPPORTED);
        	    goto error;
            }            
        }
    }

error:
    BDBG_LEAVE(BRAP_P_AllocateAndOpenRbuf);	
    return ret;        
}

#endif

/***************************************************************************
Summary:
	API to add destination to a group of raptor channel(s). 

Description:
o	This PI is used to add destination to a channel group.

o   This PI should be called only after the channel has been stopped. It 
    should not be called while the channel is still running.

o	Usage Example1: A group of channels sending data to 
	a) I2s0-3 7.1 mode 
	b) DAC 2.0 mode
	c) I2s4 2.0 mode
	Call sequence:
	After forming the associated channel handle(hAsctdChan), 
	a) BRAP_AddDestination(hAsctdChan, settings for I2s0-3);
	b) BRAP_AddDestination(hAsctdChan, settings for DAC);
	c) BRAP_AddDestination(hAsctdChan, settings for I2s4);

o	Hardware Limitation: Can't group other than I2s0-3 or one of its subset.
	
	TODO: Add more limitations and usage cases.
	
Returns:
	BERR_SUCCESS on success else error
	
See Also:
    BRAP_RemoveDestination
****************************************************************************/
BERR_Code BRAP_AddDestination(
    BRAP_AssociatedChannelHandle    hAssociatedCh,     
                                            /* [in] Handle to the group of 
                                               associated audio channels */
    BRAP_DstDetails                 *pDstDetails,
                                            /* [in] Details of the destination 
                                               to be added to the channel group 
                                            */
    BRAP_DestinationHandle          *hDstHandle
                                            /* [out] Destination handle that
                                               represents this distination */                                            
    )
{
    BERR_Code                   ret = BERR_SUCCESS;
    BRAP_Handle                 hRap = NULL;
    BRAP_ChannelHandle          hRapCh = NULL;
    BRAP_DstDetails             *pAssoDstDetails = NULL;
    int                         i = 0;
    unsigned int                uiDst = 0;
    BRAP_OutputPort             eOp = BRAP_OutputPort_eMax;
    BRAP_P_OpAudModProp         sOpAudModeProp;
    bool                        bPresent = false;

    BDBG_ENTER (BRAP_AddDestination);

    /* Validate the Input parameters */
    BDBG_ASSERT (hAssociatedCh);
    BDBG_ASSERT (pDstDetails);
    BDBG_ASSERT (hDstHandle);

    
    /* Get hRap */
    if(hAssociatedCh->hPriDecCh[0])
    {
        hRap = hAssociatedCh->hPriDecCh[0]->hRap;
    }
    else if(hAssociatedCh->hSecDecCh[0])
    {
        hRap = hAssociatedCh->hSecDecCh[0]->hRap;
    }
    else if(hAssociatedCh->hPBCh[0])
    {
        hRap = hAssociatedCh->hPBCh[0]->hRap;
    }
    else if(hAssociatedCh->hCapCh[0])
    {
        hRap = hAssociatedCh->hCapCh[0]->hRap;
    }
    else
    {
        BDBG_ERR(("BRAP_AddDestination: No valid channel in hAssociatedHandle"));
        return BERR_TRACE(BERR_NOT_INITIALIZED);
    }

    /* If output port is MAI, check if its mux-selector is already added and is
       compatible with MAI settings */
    ret = BRAP_P_IsMaiAndMuxSelectorCompatible(hRap, hAssociatedCh, pDstDetails);       
    if(ret != BERR_SUCCESS)
    {
        BDBG_ERR(("BRAP_AddDestination: HDMI and its mux-selector are not compatible"));
        return BERR_TRACE(ret);
    }

    /* Return an error if the new Output port is already in use by any 
       channel in the group */
    bPresent = BRAP_P_IsDstAlreadyPresent(hRap, hAssociatedCh, pDstDetails);
    if(true == bPresent)
    {
        BDBG_ERR(("BRAP_AddDestination: Destination already present in the association"));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    /* Debug Prints */
    BDBG_MSG(("---------------------------------------"));
    if(pDstDetails->eAudioDst == BRAP_AudioDst_eOutputPort)
    {
        for(i=0; i<BRAP_OutputChannelPair_eMax; i++)
        {
            if(pDstDetails->uDstDetails.sOpDetails.eOutput[i] != BRAP_OutputPort_eMax)
            {
                BDBG_MSG(("BRAP_AddDestination: Destination is Output port = %d for channel pair = %d"
                    " and hAssociatedCh = 0x%x", 
                    pDstDetails->uDstDetails.sOpDetails.eOutput[i], i,hAssociatedCh));            
            }
        }
    }
    else if(pDstDetails->eAudioDst == BRAP_AudioDst_eRingBuffer)
    {
        BDBG_MSG(("BRAP_AddDestination: Destination is Ringbuffer for hAssociatedCh = 0x%x", 
                    hAssociatedCh));
    }
    BDBG_MSG(("---------------------------------------"));
                
    /* Save the destination details in the associated channel handle */
    for(uiDst=0; uiDst<BRAP_P_MAX_DST_PER_RAPCH; uiDst++)
    {
        if(BRAP_AudioDst_eMax == hAssociatedCh->sDstDetails[uiDst].sExtDstDetails.eAudioDst) 
            break;
    }

    if(uiDst >= BRAP_P_MAX_DST_PER_RAPCH)
    {
        BDBG_ERR(("BRAP_AddDestination: All %d destinations are full",
            BRAP_P_MAX_DST_PER_RAPCH));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    else
    {
        hAssociatedCh->sDstDetails[uiDst].sExtDstDetails = *pDstDetails;
        pAssoDstDetails = &(hAssociatedCh->sDstDetails[uiDst].sExtDstDetails);
        hAssociatedCh->sDstDetails[uiDst].hAssociation = hAssociatedCh;
        hAssociatedCh->sDstDetails[uiDst].eAudioProcessing = BRAP_ProcessingType_eNone;
        hAssociatedCh->sDstDetails[uiDst].bSampleRateChangeCallbackEnabled  = false;
        

        hAssociatedCh->uiNumDstGrp++;
    }
    /* Note: In case of any error, before exiting from this function, the 
       sDstDetails populated above should be cleared */

#if (BRAP_INDEP_OP_DELAY_SUPPORTED == 1)
    if((hAssociatedCh->hPriDecCh[0])&&
        (BRAP_AudioDst_eOutputPort == pDstDetails->eAudioDst))
    {
        if((false == hAssociatedCh->hPriDecCh[0]->bIndepDelayEnabled)&&
            (hRap->sOutputSettings[pDstDetails->uDstDetails.sOpDetails.eOutput[BRAP_OutputChannelPair_eLR]].iDelay > 0))
        {
            BDBG_ERR(("Delay for the O/P port is  greater than 0. And pChnSettings->bEnaIndDly == false. Channel has to be opened with pChnSettings->bEnaIndDly=true ",
                "for this port to have an indepedent delay. Please close and reopen the channel with pChnSettings->bEnaIndDly=true"));
            return BERR_TRACE(BERR_INVALID_PARAMETER);            
        }
    }

    if((hAssociatedCh->hCapCh[0])&&
        (BRAP_AudioDst_eOutputPort == pDstDetails->eAudioDst))
    {
        if((false == hAssociatedCh->hCapCh[0]->bIndepDelayEnabled)&&
            (hRap->sOutputSettings[pDstDetails->uDstDetails.sOpDetails.eOutput[BRAP_OutputChannelPair_eLR]].iDelay > 0))
        {
            BDBG_ERR(("Delay for the O/P port is  greater than 0. And pChnSettings->bEnaIndDly == false. Channel has to be opened with pChnSettings->bEnaIndDly=true ",
                "for this port to have an indepedent delay. Please close and reopen the channel with pChnSettings->bEnaIndDly=true"));
            return BERR_TRACE(BERR_INVALID_PARAMETER);            
        }
    }    
#endif

    switch (pAssoDstDetails->eAudioDst)
    {
        case BRAP_AudioDst_eOutputPort:
            /* Validate if required number of Output ports are requested to add */
            ret = BRAP_P_GetAudOpModeProp(pAssoDstDetails->uDstDetails.sOpDetails.eAudioMode,
                                          pAssoDstDetails->uDstDetails.sOpDetails.bLfeOn,
                                          &sOpAudModeProp);
            if(BERR_SUCCESS != ret)
            {
                BDBG_ERR(("BRAP_AddDestination: BRAP_P_GetAudOpModeProp"
                    " returned %d",ret));
                return BERR_TRACE(ret);
            }
            /* Validate params */

            eOp = pAssoDstDetails->uDstDetails.sOpDetails.eOutput[BRAP_OutputChannelPair_eCompressed];
        	if(((true == hRap->sOutputSettings[eOp].bCompressed)&&
                ((BRAP_OutputPort_eSpdif == eOp) ||
                 (BRAP_OutputPort_eSpdif1 == eOp)))||
               (BRAP_OutputPort_eMai == eOp))

        	{
        	
                if(pAssoDstDetails->uDstDetails.sOpDetails.eOutput[BRAP_OutputChannelPair_eCompressed] == BRAP_OutputPort_eMax)
                {
                    BDBG_ERR(("BRAP_AddDestination:No destination request "
                        "existing channel pair %d",i));
                    return BERR_TRACE(BERR_NOT_INITIALIZED);
                }
                
                eOp = pAssoDstDetails->uDstDetails.sOpDetails.eOutput[BRAP_OutputChannelPair_eCompressed];
                /* Check if the output port(s) Selected are supported */
                ret = BRAP_RM_P_IsOutputPortSupported(eOp);
                if( ret != BERR_SUCCESS)
                {
                    return BERR_TRACE(ret);
                }

                /* Make sure all new ports are configured */ 
                if(hRap->bOpSettingsValid[eOp] == false) 
                {
                    BDBG_ERR(("BRAP_AddDestination: New Output port %d is not configured."
                        "Please configure before calling this PI.", eOp));
                    return BERR_TRACE(BRAP_ERR_OUTPUT_NOT_CONFIGURED);
                }
        	}
        	else
        	{
                for (i=0;i<BRAP_OutputChannelPair_eMax; i++)
                {
                    if((sOpAudModeProp.bChnExists[i*2] == true) &&
                       (sOpAudModeProp.bChnExists[(i*2)+1] == true))
            		{
                        if(pAssoDstDetails->uDstDetails.sOpDetails.eOutput[i] == BRAP_OutputPort_eMax)
                        {
                                BDBG_ERR(("BRAP_AddDestination:No destination request "
                                    "existing channel pair %d",i));
                                return BERR_TRACE(BERR_NOT_INITIALIZED);
                        }
                        
                        eOp = pAssoDstDetails->uDstDetails.sOpDetails.eOutput[i];
                        /* Check if the output port(s) Selected are supported */
                        ret = BRAP_RM_P_IsOutputPortSupported(eOp);
                        if( ret != BERR_SUCCESS)
                        {
                            return BERR_TRACE(ret);
                        }

                        /* Make sure all new ports are configured */ 
                        if(hRap->bOpSettingsValid[eOp] == false) 
                        {
                            BDBG_ERR(("BRAP_AddDestination: New Output port %d is not configured."
                                "Please configure before calling this PI.", eOp));
                            return BERR_TRACE(BRAP_ERR_OUTPUT_NOT_CONFIGURED);
                        }
                    }/* if */
                }/* for i */
	        }/* if - else */

            for(i=0;i<BRAP_MAX_PRI_DEC_CHAN_IN_ASSOCIATED_GRP;i++)
            {
                hRapCh = hAssociatedCh->hPriDecCh[i];
                if(BRAP_P_IsPointerValid((void *)hRapCh))
                {
                    ret = BRAP_P_AllocateAndOpenOpPort(hRapCh, pAssoDstDetails);
                    if(BERR_SUCCESS != ret)
                    {
                        BDBG_ERR(("BRAP_AddDestination: BRAP_P_AllocateAndOpenOpPort "
                            " returned %d for PrimaryDec[%d]", ret, i));
                        goto close_primary;
                    }
                }
            }

            for(i=0;i<BRAP_MAX_SEC_DEC_CHAN_IN_ASSOCIATED_GRP;i++)
            {
                hRapCh = hAssociatedCh->hSecDecCh[i];
                if(BRAP_P_IsPointerValid((void *)hRapCh))
                {
                    ret = BRAP_P_AllocateAndOpenOpPort(hRapCh, pAssoDstDetails);
                    if(BERR_SUCCESS != ret)
                    {
                        BDBG_ERR(("BRAP_AddDestination: BRAP_P_AllocateAndOpenOpPort "
                            " returned %d for SecondaryDec[%d]", ret, i));
                        goto close_secondary;
                    }
                }
            }

            for(i=0;i<BRAP_MAX_PB_CHAN_IN_ASSOCIATED_GRP;i++)
            {
                hRapCh = hAssociatedCh->hPBCh[i];
                if(BRAP_P_IsPointerValid((void *)hRapCh))
                {
                    ret = BRAP_P_AllocateAndOpenOpPort(hRapCh, pAssoDstDetails);
                    if(BERR_SUCCESS != ret)
                    {
                        BDBG_ERR(("BRAP_AddDestination: BRAP_P_AllocateAndOpenOpPort "
                            " returned %d for PCMPlayback[%d]", ret, i));
                        goto close_PB;
                    }
                }
            }

            for(i=0;i<BRAP_MAX_CAP_CHAN_IN_ASSOCIATED_GRP;i++)
            {
                hRapCh = hAssociatedCh->hCapCh[i];
                if(BRAP_P_IsPointerValid((void *)hRapCh))
                {
                    ret = BRAP_P_AllocateAndOpenOpPort(hRapCh, pAssoDstDetails);
                    if(BERR_SUCCESS != ret)
                    {
                        BDBG_ERR(("BRAP_AddDestination: BRAP_P_AllocateAndOpenOpPort "
                            " returned %d for PCMCapture[%d]", ret, i));
                        goto close_cap;
                    }
                }
            }
            break;
#if (BRAP_7550_FAMILY !=1)
        case BRAP_AudioDst_eRingBuffer:
#if ( (BRAP_3548_FAMILY == 1) )
		if ( pDstDetails->uDstDetails.sRBufDetails.eAudioMode > BRAP_OutputMode_e2_0 )
		{
			return BERR_TRACE(BERR_INVALID_PARAMETER);
		}
#endif		
            ret = BRAP_P_AllocateAndOpenRbuf(hAssociatedCh, pAssoDstDetails);
            if(BERR_SUCCESS != ret)
            {
                BDBG_ERR(("BRAP_AddDestination: BRAP_P_AllocateAndOpenRbuf "
                    " returned %d for Association Handle %x", ret, hAssociatedCh));
                goto error;
            }
            
            /* As of now Destination level interrupts are handled only for ringbuffers, not for ouput ports.*/
            /* Intialise Raptor interrupt handling */
            ret = BRAP_P_DestinationLevelInterruptInstall (&(hAssociatedCh->sDstDetails[uiDst]));
            if(BERR_SUCCESS != ret)
            {
            	ret = BERR_TRACE(ret);
            	BDBG_ERR(("InstallInterrupt()failed for Destination handle 0x%x", 
                            &(hAssociatedCh->sDstDetails[uiDst])));
                goto error;                        
            }   
            break;
#endif
        case BRAP_AudioDst_eMax:
        default:
            BDBG_ERR(("BRAP_AddDestination: eAudioDst = %d not supported",
            pAssoDstDetails->eAudioDst));
            return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    
    if(BERR_SUCCESS == ret)
    {
        /* Exit */
        goto end;    
    }
#if (BRAP_7550_FAMILY !=1)
error:
#endif
/* TODO */    
close_cap:

close_PB:
/* TODO */
close_secondary:
/* TODO */
close_primary:
/* TODO */

    /* Some error, so remove the Dst details added above */
    hAssociatedCh->uiNumDstGrp--;
    hAssociatedCh->sDstDetails[uiDst].sExtDstDetails.eAudioDst = BRAP_AudioDst_eMax;
    hAssociatedCh->sDstDetails[uiDst].hAssociation = NULL;
     
    hAssociatedCh->sDstDetails[uiDst].hParentDestHandle = NULL;
    hAssociatedCh->sDstDetails[uiDst].uiForkingStage = BRAP_INVALID_VALUE;
    hAssociatedCh->sDstDetails[uiDst].eAudioProcessing = BRAP_ProcessingType_eMax;
    for(i=0; i<BRAP_MAX_PP_PER_BRANCH_SUPPORTED; i++)
    {
        hAssociatedCh->sDstDetails[uiDst].eAudProcessing[i] = BRAP_ProcessingType_eMax;
    }


end:    
    if(BERR_SUCCESS == ret)
    {
        *hDstHandle = &(hAssociatedCh->sDstDetails[uiDst]);
    }

    BDBG_LEAVE (BRAP_AddDestination);
    return ret;
}

/***************************************************************************
Summary:
    This private routine forms the output resource request, allocates them 
    and open the allocated outputs and spdiffm (if any).
****************************************************************************/
static BERR_Code 
BRAP_P_AllocateAndOpenOpPort(
    BRAP_ChannelHandle      hRapCh,         /* [in] Channel handle */
    BRAP_DstDetails         *pDstDetails    /* [in] Details of the destination 
                                              to be added to the channel group*/ 
    )
{
    BERR_Code                   ret = BERR_SUCCESS;
    unsigned int                i = 0;
    BRAP_RM_P_ResrcReq          *psResrcReq =NULL;
    BRAP_RM_P_ResrcGrant        *psResrcGrant = NULL;
    BRAP_OutputPort             eOp = BRAP_OutputPort_eMax;
    BRAP_SPDIFFM_P_Settings     sSpdifFmSettings;
    BRAP_OP_P_SpdifSettings     sSpdifSettings;    
    BRAP_OP_P_MaiSettings       sMaiSettings;
    BRAP_OP_P_I2sSettings       sI2sSettings;
    BRAP_OP_P_DacSettings       sDacSettings;
    void                        *pNewOpSettings = NULL;
    unsigned int                uiSpdiffmStreamId = 0;
    BRAP_Handle                 hRap = hRapCh->hRap;    
    
    /* Malloc large structures */
    psResrcReq = hRap->sOpenTimeMallocs.psResrcReq;
    if ( NULL==psResrcReq)
    {
    		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }
   BKNI_Memset(psResrcReq,0,sizeof(BRAP_RM_P_ResrcReq));        
    psResrcGrant = hRap->sOpenTimeMallocs.psResrcGrant;
    if ( NULL==psResrcGrant)
    {
    		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }
   BKNI_Memset(psResrcGrant,0,sizeof(BRAP_RM_P_ResrcGrant));    	
    /* Initialize resource Request structure with Invalid values */
    BRAP_RM_P_InitResourceReq(psResrcReq);
    
    /* Prepare the resource request */
    for(i=0;i<BRAP_RM_P_MAX_OP_CHANNEL_PAIRS;i++)
    {
        if(pDstDetails->uDstDetails.sOpDetails.eOutput[i] != BRAP_OutputPort_eMax)
        {
            psResrcReq->sOpReq[i].bAllocate = true;
            psResrcReq->sOpReq[i].eOpPortType = 
                    pDstDetails->uDstDetails.sOpDetails.eOutput[i];
            if(BRAP_OutputPort_eMai == pDstDetails->uDstDetails.sOpDetails.eOutput[i]) 
            {
                psResrcReq->sOpReq[i].eMuxSelect = 
                    hRap->sOutputSettings[BRAP_OutputPort_eMai].uOutputPortSettings.
                        sMaiSettings.eMaiMuxSelector;
            }
        }
    }

    /* Call resource manager to allocate required resources. */
    ret = BRAP_RM_P_AllocateResources(hRap->hRm, psResrcReq, psResrcGrant);
    if(ret != BERR_SUCCESS) 
    {
        BDBG_ERR(("BRAP_P_AllocateAndOpenOpPort: Resource Manager could not "
                  "allocate requested resources."));
        ret = BERR_TRACE(ret);
	goto error;
    }

    for (i=0;i<BRAP_RM_P_MAX_OP_CHANNEL_PAIRS;i++)
    {
        eOp = psResrcGrant->sOpPortGrnt[i].eOutputPort;
        if(BRAP_OutputPort_eMax == eOp)
            continue;

        /* Get the Settings of SPDIF FM and output port */
        BRAP_SPDIFFM_P_GetDefaultSettings(&sSpdifFmSettings);
        sSpdifFmSettings.sExtSettings = 
                    hRap->sOutputSettings[eOp].sSpdiffmSettings;

        /* Get New Output port settings: these should have been set by the app 
        earlier by calling BRAP_SetOutputConfig() */
        switch(eOp)
        {
            case BRAP_OutputPort_eSpdif:
            case BRAP_OutputPort_eSpdif1:
                sSpdifSettings.sExtSettings = 
                  hRap->sOutputSettings[eOp].uOutputPortSettings.sSpdifSettings;
                pNewOpSettings = &sSpdifSettings;
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
                sI2sSettings.sExtSettings = 
                   hRap->sOutputSettings[eOp].uOutputPortSettings.sI2sSettings;
                pNewOpSettings = &sI2sSettings;
                break;
            case BRAP_OutputPort_eMai:
                sMaiSettings.sExtSettings = 
                    hRap->sOutputSettings[eOp].uOutputPortSettings.sMaiSettings;
                pNewOpSettings = &sMaiSettings;
                break;
            case BRAP_OutputPort_eDac0:
            case BRAP_OutputPort_eDac1:
            case BRAP_OutputPort_eDac2:                
                sDacSettings.sExtSettings = 
                    hRap->sOutputSettings[eOp].uOutputPortSettings.sDacSettings;
                pNewOpSettings = &sDacSettings;
                break;
            case BRAP_OutputPort_eMaiMulti0:
            case BRAP_OutputPort_eMaiMulti1:
            case BRAP_OutputPort_eMaiMulti2:
            case BRAP_OutputPort_eMaiMulti3:
                sMaiSettings.sExtSettings = 
                    hRap->sOutputSettings[eOp].uOutputPortSettings.sMaiMultiSettings;
                pNewOpSettings = &sMaiSettings;                
                break;
                
            default:
                BDBG_ERR(("BRAP_P_AllocateAndOpenOpPort: Output port type %d"
                    " not supported", eOp));
                ret = BERR_TRACE(BRAP_ERR_OUPUT_PORT_NOT_SUPPORTED);   
		  goto error;
        }
        
        /* Open SpdiffRm if required */
        if((BRAP_RM_P_INVALID_INDEX != psResrcGrant->sOpPortGrnt[i].uiSpdiffmId)&&
           (BRAP_RM_P_INVALID_INDEX != psResrcGrant->sOpPortGrnt[i].uiSpdiffmStreamId))
        {
            uiSpdiffmStreamId = psResrcGrant->sOpPortGrnt[i].uiSpdiffmStreamId;
            
            ret = BRAP_SPDIFFM_P_Open (hRap->hFmm[0],
			                           &(hRap->hFmm[0]->hSpdifFm[uiSpdiffmStreamId]),
			                           uiSpdiffmStreamId,
			                           &sSpdifFmSettings);
	        if(ret != BERR_SUCCESS)
    		{
    			ret = BERR_TRACE(ret);
    			goto error;
    		}

            BDBG_MSG(("Spdiffm %d - stream %d opened", 
    				  psResrcGrant->sOpPortGrnt[i].uiSpdiffmId, 
    				  uiSpdiffmStreamId));
        }
        
        /* Open the Output port */
        ret = BRAP_OP_P_Open(hRap->hFmm[0],
							 &(hRap->hFmm[0]->hOp[eOp]),
							 eOp,
							 pNewOpSettings);
        
		if(ret != BERR_SUCCESS)
		{
			ret = BERR_TRACE(ret);
			goto error;
		}
		BDBG_MSG(("Output port type %d opened",eOp));
    }/* for i */

    /* Save pDstDetails in the channel handle */
    for(i=0; i<BRAP_P_MAX_DST_PER_RAPCH; i++)
    {
        if(!(BRAP_P_IsPointerValid((void *)hRapCh->pDstDetails[i])))
            break;
    }

    if(i >= BRAP_P_MAX_DST_PER_RAPCH)
    {
        ret = BERR_TRACE(BERR_NOT_SUPPORTED);
	goto error;
    }
    else
    {
        hRapCh->pDstDetails[i] = pDstDetails;
    }

error:
	
    return ret;        
}
#if (BRAP_7550_FAMILY !=1)

/***************************************************************************
Summary:
    This private routine Closes the requested RBUFs and de-allocates them.
****************************************************************************/
/* Destinations(Output Port and Ringbuffer) are properties of an Association
   so allocate and open of these need to be handled per Assoc not per channel. 
   ToDo: For Output Port */

static BERR_Code 
BRAP_P_CloseAndFreeRbuf (
    BRAP_AssociatedChannelHandle    hAssociatedCh,  /* [in] Association handle */
    BRAP_RemoveDstDetails           *pDstDetails    /* [in] Details of the destination 
                                                            to be removed */ 
    )
{
    BERR_Code                   ret = BERR_SUCCESS;
    BRAP_Handle                 hRap = hAssociatedCh->hRap;
    BRAP_RM_P_ResrcGrant        *psResrcGrant;
    unsigned int                i = 0,uiAssocChId = 0, uiDst = 0,j=0;
    BRAP_ChannelHandle          hAssocRapCh[BRAP_MAX_ASSOCIATED_CHANNELS_IN_GRP];    

    BDBG_ENTER(BRAP_P_CloseAndFreeRbuf);
    BSTD_UNUSED(pDstDetails);
    
    psResrcGrant = hRap->sOpenTimeMallocs.psResrcGrant;
    if ( NULL==psResrcGrant)
    {
		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }
    BKNI_Memset(psResrcGrant,0,sizeof(BRAP_RM_P_ResrcGrant));    	
    /* Initialize resource Grant structure with Invalid values */
    BRAP_RM_P_InitResourceGrant(psResrcGrant,true);

    /* Initialize the Array of Channel Handles */
    for(uiAssocChId = 0; uiAssocChId < BRAP_MAX_ASSOCIATED_CHANNELS_IN_GRP; uiAssocChId++)
    {
        hAssocRapCh[uiAssocChId] = NULL;
    }
    
    /* Collate all the valid channel handles in the association into one array, hAssocRapCh[] */
    for(i=0; i<BRAP_MAX_PRI_DEC_CHAN_IN_ASSOCIATED_GRP; i++)
    {
        if(BRAP_P_IsPointerValid((void *)hAssociatedCh->hPriDecCh[i]))
        {
            for(uiAssocChId=0; uiAssocChId<BRAP_MAX_ASSOCIATED_CHANNELS_IN_GRP; uiAssocChId++)
            {
                if(!(BRAP_P_IsPointerValid((void *)hAssocRapCh[uiAssocChId])))
                {
                    hAssocRapCh[uiAssocChId] = hAssociatedCh->hPriDecCh[i];
                    break;
                }
            }
            if(uiAssocChId >= BRAP_MAX_ASSOCIATED_CHANNELS_IN_GRP)
            {
                BDBG_ERR(("BRAP_P_CloseAndFreeRbuf: No free Index in hAssocRapCh=%x"
                    "to store the Primary Decode Channel Handle", hAssocRapCh));
                ret = BERR_TRACE(BERR_NOT_SUPPORTED);
        	    goto error;
            }
        }
    }

    for(i=0; i<BRAP_MAX_SEC_DEC_CHAN_IN_ASSOCIATED_GRP; i++)
    {
        if(BRAP_P_IsPointerValid((void *)hAssociatedCh->hSecDecCh[i]))
        {
            for(uiAssocChId=0; uiAssocChId<BRAP_MAX_ASSOCIATED_CHANNELS_IN_GRP; uiAssocChId++)
            {
                if(!(BRAP_P_IsPointerValid((void *)hAssocRapCh[uiAssocChId])))
                {
                    hAssocRapCh[uiAssocChId] = hAssociatedCh->hSecDecCh[i];
                    break;
                }
            }
            if(uiAssocChId >= BRAP_MAX_ASSOCIATED_CHANNELS_IN_GRP)
            {
                BDBG_ERR(("BRAP_P_CloseAndFreeRbuf: No free Index in hAssocRapCh=%x"
                    "to store the Secondary Decode Channel Handle", hAssocRapCh));
                ret = BERR_TRACE(BERR_NOT_SUPPORTED);
        	    goto error;
            }            
        }
    }

    for(i=0; i<BRAP_MAX_PB_CHAN_IN_ASSOCIATED_GRP; i++)
    {
        if(BRAP_P_IsPointerValid((void *)hAssociatedCh->hPBCh[i]))
        {
            for(uiAssocChId=0; uiAssocChId<BRAP_MAX_ASSOCIATED_CHANNELS_IN_GRP; uiAssocChId++)
            {
                if(!(BRAP_P_IsPointerValid((void *)hAssocRapCh[uiAssocChId])))
                {
                    hAssocRapCh[uiAssocChId] = hAssociatedCh->hPBCh[i];
                    break;
                }
            }
            if(uiAssocChId >= BRAP_MAX_ASSOCIATED_CHANNELS_IN_GRP)
            {
                BDBG_ERR(("BRAP_P_CloseAndFreeRbuf: No free Index in hAssocRapCh=%x"
                    "to store the Playback Channel Handle", hAssocRapCh));
                ret = BERR_TRACE(BERR_NOT_SUPPORTED);
        	    goto error;
            }            
        }
    }

    for(i=0; i<BRAP_MAX_CAP_CHAN_IN_ASSOCIATED_GRP; i++)
    {
        if(BRAP_P_IsPointerValid((void *)hAssociatedCh->hCapCh[i]))
        {
            for(uiAssocChId=0; uiAssocChId<BRAP_MAX_ASSOCIATED_CHANNELS_IN_GRP; uiAssocChId++)
            {
                if(!(BRAP_P_IsPointerValid((void *)hAssocRapCh[uiAssocChId])))
                {
                    hAssocRapCh[uiAssocChId] = hAssociatedCh->hCapCh[i];
                    break;
                }
            }
            if(uiAssocChId >= BRAP_MAX_ASSOCIATED_CHANNELS_IN_GRP)
            {
                BDBG_ERR(("BRAP_P_CloseAndFreeRbuf: No free Index in hAssocRapCh=%x"
                    "to store the Capture Channel Handle", hAssocRapCh));
                ret = BERR_TRACE(BERR_NOT_SUPPORTED);
        	    goto error;
            }            
        }
    }

    if(!(BRAP_P_IsPointerValid((void *)hAssocRapCh[0])))
    {
        BDBG_ERR(("BRAP_P_CloseAndFreeRbuf: No valid channel handle in the Association Handle %x",
                    hAssociatedCh));
        ret = BERR_TRACE(BERR_NOT_SUPPORTED);
	    goto error;
    }
    
    for(i=0; i<BRAP_P_MAX_DST_PER_RAPCH; i++)
    {
        if(BRAP_AudioDst_eRingBuffer == hAssociatedCh->sDstDetails[i].sExtDstDetails.eAudioDst)
        {
            if(hAssociatedCh->sDstDetails[i].sExtDstDetails.uDstDetails.sRBufDetails.eAudioMode == BRAP_OutputMode_e2_0)
            {
                psResrcGrant->uiRbufId[0] = hAssociatedCh->sDstDetails[i].sExtDstDetails.uDstDetails.sRBufDetails.uiRBufId[0];
                if(hAssociatedCh->sDstDetails[i].sExtDstDetails.uDstDetails.sRBufDetails.eBufDataMode[0] == BRAP_BufDataMode_eStereoNoninterleaved)
                {
                    psResrcGrant->uiRbufId[1] = hAssociatedCh->sDstDetails[i].sExtDstDetails.uDstDetails.sRBufDetails.uiRBufId[1];
                }
                else
                {
                    psResrcGrant->uiRbufId[1] = BRAP_RM_P_INVALID_INDEX;                
                }
                psResrcGrant->uiDstChId[0] = hAssociatedCh->sDstDetails[i].sExtDstDetails.uDstDetails.sRBufDetails.uiDstChId[0];
            }
            else if(hAssociatedCh->sDstDetails[i].sExtDstDetails.uDstDetails.sRBufDetails.eAudioMode == BRAP_OutputMode_e3_2)
            {
                for(j = 0 ; j < 6 ; j++)
                {
                    psResrcGrant->uiRbufId[j] = hAssociatedCh->sDstDetails[i].sExtDstDetails.uDstDetails.sRBufDetails.uiRBufId[j];                    
                }
                for(j = 0 ; j < 3 ; j++)
                {
                    psResrcGrant->uiDstChId[j] = hAssociatedCh->sDstDetails[i].sExtDstDetails.uDstDetails.sRBufDetails.uiDstChId[j];
                }                
            }
            else
            {
                BDBG_ERR(("UNSUPPORTED Output Mode for Rbuf."));
                ret = BERR_TRACE(BERR_INVALID_PARAMETER);
                goto error;
            }
#if (BRAP_OPEN_TIME_RBUF_ALLOCATION==1)
			BRAP_P_ReturnRBufToPool(hAssocRapCh[0],NULL,true,0,0);
#endif
            break;
        }
    }
    if(BRAP_P_MAX_DST_PER_RAPCH == i)
    {
        BDBG_ERR(("BRAP_P_CloseAndFreeRbuf: Couldn't find RBUF Destination."));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    for(j = 0 ; j < BRAP_RM_P_MAX_OP_CHANNELS ; j++)
    {    
        if(psResrcGrant->uiRbufId[j] != BRAP_RM_P_INVALID_INDEX)
        {
            ret = BRAP_RBUF_P_Close(hRap->hFmm[0]->hRBuf[psResrcGrant->uiRbufId[j]]);
            if(BERR_SUCCESS != ret)
            {   
                BDBG_ERR(("BRAP_P_CloseAndFreeRbuf: BRAP_RBUF_P_Close returned error"));
        		ret = BERR_TRACE(ret);
        	    goto error;
            }
        }
    }
    for(j = 0 ; j < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS ; j++)
    {        
        if(psResrcGrant->uiDstChId[j] != BRAP_RM_P_INVALID_INDEX)
        {    
            ret = BRAP_DSTCH_P_Close(hRap->hFmm[0]->hDstCh[psResrcGrant->uiDstChId[j]]);
    if(BERR_SUCCESS != ret)
    {   
        BDBG_ERR(("BRAP_P_CloseAndFreeRbuf: BRAP_DSTCH_P_Close returned error"));
		ret = BERR_TRACE(ret);
	    goto error;
    }    
        }
    }
    
    /* Free the resources */
    ret = BRAP_RM_P_FreeResources(hRap->hRm, psResrcGrant,true);
    if(ret != BERR_SUCCESS) 
    {
        BDBG_ERR(("BRAP_P_CloseAndFreeRbuf: Resource Manager could not "
                  "de-allocate requested resources."));
		ret = BERR_TRACE(ret);
	    goto error;
    }

    /* Remove pDstDetails from all the channel handles of the association */
    for(uiAssocChId=0; uiAssocChId<BRAP_MAX_ASSOCIATED_CHANNELS_IN_GRP; uiAssocChId++)
    {    
        if(BRAP_P_IsPointerValid((void *)hAssocRapCh[uiAssocChId]))
        {
            for(uiDst=0; uiDst<BRAP_P_MAX_DST_PER_RAPCH; uiDst++)
            {
                if((BRAP_P_IsPointerValid((void *)hAssocRapCh[uiAssocChId]->pDstDetails[uiDst])) &&
                   (hAssocRapCh[uiAssocChId]->pDstDetails[uiDst]->eAudioDst == BRAP_AudioDst_eRingBuffer))
                {
                    hAssocRapCh[uiAssocChId]->pDstDetails[uiDst] = NULL;
                    break;
                }     
            }        
            if(uiDst >= BRAP_P_MAX_DST_PER_RAPCH)
            {
                BDBG_ERR(("Channel handle %x doesn't have the destination details populated", 
                            hAssocRapCh[uiAssocChId]));
                ret = BERR_TRACE(BERR_NOT_SUPPORTED);
        	    goto error;
            }            
        }
    }    

error:
    BDBG_LEAVE(BRAP_P_CloseAndFreeRbuf);
    return ret;
}

#endif
/***************************************************************************
Summary:
    This private routine Closes the requested outputs and spdiffm (if any)
and de-allocates them.
****************************************************************************/
static BERR_Code 
BRAP_P_CloseAndFreeOpPort (
    BRAP_ChannelHandle      hRapCh,         /* [in] Channel handle */
    BRAP_RemoveDstDetails   *pDstDetails    /* [in] Details of the destination 
                                                    to be removed */ 
    )
{
    BERR_Code                   ret = BERR_SUCCESS;
    BRAP_Handle                 hRap = hRapCh->hRap;
    BRAP_OutputPort             eOp = BRAP_OutputPort_eMax;
    BRAP_RM_P_ResrcGrant        *psResrcGrant;
    unsigned int                uiSpdifFmId = 0;
    unsigned int                uiSpdifFmChId = 0;
    unsigned int                i = 0, j = 0;
    bool                        bGotDst = false;
    BRAP_OutputPort             eMuxSelect = BRAP_OutputPort_eMax;

    BDBG_ENTER(BRAP_P_CloseAndFreeOpPort);
    
    psResrcGrant = hRap->sOpenTimeMallocs.psResrcGrant;
    if ( NULL==psResrcGrant)
    {
    		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }
   BKNI_Memset(psResrcGrant,0,sizeof(BRAP_RM_P_ResrcGrant));    	
    /* Initialize resource Grant structure with Invalid values */
    BRAP_RM_P_InitResourceGrant(psResrcGrant,true);

    for (i=0;i<BRAP_RM_P_MAX_OP_CHANNEL_PAIRS;i++)
    {
        eOp = pDstDetails->uDstDetails.eOutput[i];
        uiSpdifFmId = 0;
        uiSpdifFmChId = 0;
        
        if(BRAP_OutputPort_eMax == eOp)
            continue;

        /* Get the SpdifFm information for thr output port */
        if(BRAP_OutputPort_eMai == eOp) 
        {
            eMuxSelect = hRap->sOutputSettings[eOp].uOutputPortSettings.sMaiSettings.eMaiMuxSelector;
            BRAP_RM_P_GetSpdifFmForOpPort(hRap->hRm,
                                          eOp,
                                          eMuxSelect,
                                          &uiSpdifFmId, 
                                          &uiSpdifFmChId);
        }
        else
        {
            BRAP_RM_P_GetSpdifFmForOpPort(hRap->hRm,
                                          eOp,
                                          BRAP_OutputPort_eMax,
                                          &uiSpdifFmId, 
                                          &uiSpdifFmChId);
        }

        /* Close SpdifFm if required */
        if((BRAP_RM_P_INVALID_INDEX != uiSpdifFmId)&&
           (BRAP_RM_P_INVALID_INDEX != uiSpdifFmChId)&&
           (BRAP_P_IsPointerValid((void *)hRap->hFmm[0]->hSpdifFm[uiSpdifFmChId])))
        {
            ret = BRAP_SPDIFFM_P_Close(hRap->hFmm[0]->hSpdifFm[uiSpdifFmChId]);
            if(ret != BERR_SUCCESS)
            {
                return BERR_TRACE(ret);
            }
            
            BDBG_MSG(("Spdiffm %d - stream %d Closed",uiSpdifFmChId,uiSpdifFmChId));
        }

        /* Close the Output port */
        ret = BRAP_OP_P_Close(hRap->hFmm[0]->hOp[eOp]);
        if(ret != BERR_SUCCESS)
        {
            return BERR_TRACE(ret);
        }
        BDBG_MSG(("Output port type %d Closed",eOp));

        psResrcGrant->sOpPortGrnt[i].eOutputPort=eOp;
        psResrcGrant->sOpPortGrnt[i].uiSpdiffmId = uiSpdifFmId;
        psResrcGrant->sOpPortGrnt[i].uiSpdiffmStreamId = uiSpdifFmChId;
    }

    /* Free the resources */
    ret = BRAP_RM_P_FreeResources(hRap->hRm, psResrcGrant,true);
    if(ret != BERR_SUCCESS) 
    {
        BDBG_ERR(("BRAP_RemoveDestination: Resource Manager could not "
                  "de-allocate requested resources."));
        return BERR_TRACE(ret);
    }

    /* Clear pDstDetails in the channel handle */
    for(i=0; i<BRAP_P_MAX_DST_PER_RAPCH; i++)
    {
        if(!(BRAP_P_IsPointerValid((void *)hRapCh->pDstDetails[i])))
        {
            continue;
        }
    
        for(j=0; j<BRAP_RM_P_MAX_OP_CHANNEL_PAIRS;j++)
        {
            if((BRAP_AudioDst_eOutputPort == hRapCh->pDstDetails[i]->eAudioDst)&&
               (pDstDetails->uDstDetails.eOutput[j] == 
                      hRapCh->pDstDetails[i]->uDstDetails.sOpDetails.eOutput[j]))
            {
                bGotDst = true;
            }
            else
            {
                bGotDst = false;
                break;
            }
        }
        if((BRAP_RM_P_MAX_OP_CHANNEL_PAIRS == j) && (true == bGotDst))
        {
            break;
        }
    }

    if(i>=BRAP_P_MAX_DST_PER_RAPCH)
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    else
    {
       hRapCh->pDstDetails[i] = NULL;
    }

    BDBG_LEAVE(BRAP_P_CloseAndFreeOpPort);
    return ret;
}

static BERR_Code
BRAP_P_AllocateAndInvalidateFwTaskHandles(
    BRAP_Handle     hRap
    )
{
    unsigned int i,j;
    BERR_Code ret = BERR_SUCCESS;
            /* Allocate memory for FW Task Handle */
    hRap->hFwTask[0][0] = (BRAP_FWIF_P_FwTaskHandle) BKNI_Malloc((sizeof(BRAP_FWIF_P_FwTask))*BRAP_RM_P_MAX_DSPS*BRAP_RM_P_MAX_FW_TASK_PER_DSP);
    if( NULL==hRap->hFwTask[0][0])
    {
        ret = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        goto free;
    }
   BKNI_Memset(hRap->hFwTask[0][0],0,(sizeof(BRAP_FWIF_P_FwTask))*BRAP_RM_P_MAX_DSPS*BRAP_RM_P_MAX_FW_TASK_PER_DSP);    	                
    for (i = 0; i < BRAP_RM_P_MAX_DSPS; i++)
    {
        for (j = 0; j < BRAP_RM_P_MAX_FW_TASK_PER_DSP; j++)
        {
            hRap->hFwTask[i][j] = hRap->hFwTask[0][0] + ((i*BRAP_RM_P_MAX_FW_TASK_PER_DSP) + j);
            BDBG_MSG(("hRap->hFwTask[%d][%d] = %#X, sizeof(BRAP_FWIF_P_FwTask) =%#X",i,j,hRap->hFwTask[i][j],sizeof(BRAP_FWIF_P_FwTask)));
            /* Invalidate the fields of FW Task Handle */
            hRap->hFwTask[i][j]->uiTaskId = BRAP_FWIF_P_INVALID_TSK_ID;
            hRap->hFwTask[i][j]->bChSpecificTask = false;
            hRap->hFwTask[i][j]->uHandle.hAssociation = NULL;
            hRap->hFwTask[i][j]->hDsp = NULL;
            hRap->hFwTask[i][j]->hAsyncMsgQueue = NULL;
            hRap->hFwTask[i][j]->hSyncMsgQueue = NULL;
            hRap->hFwTask[i][j]->pAsyncMsgMemory = NULL;
            hRap->hFwTask[i][j]->bStopped= true;            
            hRap->hFwTask[i][j]->uiLastEventType= 0xFFFF;                                  
            hRap->hFwTask[i][j]->uiMasterTaskId= BRAP_FWIF_P_INVALID_TSK_ID;     
            hRap->hFwTask[i][j]->bMaster= false;                 
            hRap->hFwTask[i][j]->uiCommandCounter= 0;                          
        }
    }

free:
    
    return ret;
}

/***************************************************************************
Summary:
    Removes/de-links a destination from a group of associated channels
   
Description:
o   This PI can be used to remove any of the destinations associated with a 
    channel group.

o   This PI should be called only after the channel has been stopped. It 
    should not be called while the channel is still running.

o   It will close and free all the resources opened during the corresponding
    call to the BRAP_AddDestination().

Returns:
	BERR_SUCCESS if successful else error value.            
****************************************************************************/
BERR_Code BRAP_RemoveDestination(
    BRAP_AssociatedChannelHandle    hAssociatedChan,     
                                            /* [in] Handle to the group
                                               of audio channels */

	BRAP_DestinationHandle          hDestHandle
	                                        /* [in] Destination handle to be 
	                                           removed from this group of 
	                                           channels */
    )
{
    BERR_Code                   ret = BERR_SUCCESS;
    unsigned int                i=0,j =0, k=0, uiDst=0;
    BRAP_OutputPort             eOp = BRAP_OutputPort_eMax;
    BRAP_ChannelHandle          hRapCh = NULL;
    bool                        bDstFound = false;
    BRAP_RemoveDstDetails           sDstDetails; 
    BRAP_RemoveDstDetails           *pDstDetails = &sDstDetails; 

	BDBG_ENTER(BRAP_RemoveDestination);

	/* Assert the function argument(s) */
	BDBG_ASSERT(hAssociatedChan);
	BDBG_ASSERT(pDstDetails);
	BDBG_ASSERT(hDestHandle);
    sDstDetails.eAudioDst = hDestHandle->sExtDstDetails.eAudioDst;
    for(i=0; i<BRAP_OutputChannelPair_eMax; i++)
    {
        sDstDetails.uDstDetails.eOutput[i] = 
            hDestHandle->sExtDstDetails.uDstDetails.sOpDetails.eOutput[i];
    }

    /* Debug Prints */
    if(sDstDetails.eAudioDst == BRAP_AudioDst_eOutputPort)
    {
        for(i=0; i<BRAP_OutputChannelPair_eMax; i++)
        {
            if(sDstDetails.uDstDetails.eOutput[i] != BRAP_OutputPort_eMax)
            {
                BDBG_MSG(("BRAP_RemoveDestination: Destination is Output port = %d for channel pair = %d"
                    "and hAssociatedCh = 0x%x",
                    sDstDetails.uDstDetails.eOutput[i], i, hAssociatedChan));            
            }
        }
    }
    else if(pDstDetails->eAudioDst == BRAP_AudioDst_eRingBuffer)
    {
        BDBG_MSG(("BRAP_RemoveDestination: Destination is Ringbuffer for hAssociatedCh = 0x%x",
                    hAssociatedChan));
    }
    
    /* Check if destinations are present in the channel association */
    for(uiDst=0;uiDst<BRAP_P_MAX_DST_PER_RAPCH;uiDst++)
    {
        if(hAssociatedChan->sDstDetails[uiDst].sExtDstDetails.eAudioDst == pDstDetails->eAudioDst)
        {
            /* If Output port, verify the output port types. Note: Ring buffer 
               doesn't need this check. */
            if(BRAP_AudioDst_eOutputPort == pDstDetails->eAudioDst)
            {
                for(k=0;k<BRAP_OutputChannelPair_eMax;k++)
                {
                    if (hAssociatedChan->sDstDetails[uiDst].sExtDstDetails.uDstDetails.sOpDetails.
                        eOutput[k] == pDstDetails->uDstDetails.eOutput[k])
                    {
                        bDstFound = true;
                    }
                    else
                    {
                        bDstFound = false;
                        break;
                    }
                }/* for k */

                if((BRAP_OutputChannelPair_eMax == k) && (true == bDstFound))
                {
                    /* Found the dest group so break from uiDst-for loop */
                    break;
                }
            }
            else if(BRAP_AudioDst_eRingBuffer == pDstDetails->eAudioDst)
            {
                /* We need not check the ring buffer type */
                bDstFound = true;
                break;
            }
        }/* if eAudDst */
    }/* for uiDst */
    
    if((false == bDstFound) || (uiDst >= BRAP_P_MAX_DST_PER_RAPCH))
    {
        BDBG_ERR(("BRAP_RemoveDestination: The destination requested to "
                  "remove is not added."));
        return BERR_TRACE(BERR_NOT_INITIALIZED);
    }

    /* Do not remove the destination if any channel is in running state */
    for (i=0;i<BRAP_MAX_PRI_DEC_CHAN_IN_ASSOCIATED_GRP;i++)
    {
        if ((BRAP_P_IsPointerValid((void *)hAssociatedChan->hPriDecCh[i]))&&
            (BRAP_P_State_eStarted == hAssociatedChan->hPriDecCh[i]->eState))
        {
            BDBG_ERR(("BRAP_RemoveDestination: CAN NOT remove the destination as"
                      "it is already used by a running Primary Decode channel"));
            return (BERR_SUCCESS);
        }
    }
    for (i=0;i<BRAP_MAX_SEC_DEC_CHAN_IN_ASSOCIATED_GRP;i++)
    {
        if((BRAP_P_IsPointerValid((void *)hAssociatedChan->hSecDecCh[i]))&&
           (BRAP_P_State_eStarted == hAssociatedChan->hSecDecCh[i]->eState))
        {
            BDBG_ERR(("BRAP_RemoveDestination: CAN NOT remove the destination as"
                      "it is already used by a running Secondary Decode channel"));
            return (BERR_SUCCESS);
        }
    }
    for (i=0;i<BRAP_MAX_PB_CHAN_IN_ASSOCIATED_GRP;i++)
    {
        if ((BRAP_P_IsPointerValid((void *)hAssociatedChan->hPBCh[i]))&&
            (BRAP_P_State_eStarted == hAssociatedChan->hPBCh[i]->eState))
        {
            BDBG_ERR(("BRAP_RemoveDestination: CAN NOT remove the destination as"
                      "it is already used by a running PCM PB channel"));
            return (BERR_SUCCESS);
        }
    }
    for (i=0;i<BRAP_MAX_CAP_CHAN_IN_ASSOCIATED_GRP;i++)
    {
        if ((BRAP_P_IsPointerValid((void *)hAssociatedChan->hCapCh[i]))&&
            (BRAP_P_State_eStarted == hAssociatedChan->hCapCh[i]->eState))
        {
            BDBG_ERR(("BRAP_RemoveDestination: CAN NOT remove the destination as"
                      "it is already used by a running Capture channel"));
            return (BERR_SUCCESS);
        }
    }

    switch(pDstDetails->eAudioDst)
    {
        case BRAP_AudioDst_eOutputPort:

            for (i=0;i<BRAP_OutputChannelPair_eMax; i++)
            {
                /* Validate the Input Parameters */
                if(pDstDetails->uDstDetails.eOutput[i] == BRAP_OutputPort_eMax)
                {
                    continue;
                }
                
                eOp = pDstDetails->uDstDetails.eOutput[i];
                /* Check if the output port(s) Selected are supported */
                ret = BRAP_RM_P_IsOutputPortSupported(eOp);
                if( ret != BERR_SUCCESS)
                {
                    BDBG_ERR(("BRAP_RemoveDestination: Invalid output Port = %d"
                              "for channel Pair = %d",eOp,i));
                    return BERR_TRACE(ret);
                }
            }

            /* Remove destination from all the channels in the association */
            for(i=0;i<BRAP_MAX_PRI_DEC_CHAN_IN_ASSOCIATED_GRP;i++)
            {
                hRapCh = hAssociatedChan->hPriDecCh[i];
                if(BRAP_P_IsPointerValid((void *)hRapCh))
                {
                    ret = BRAP_P_CloseAndFreeOpPort(hRapCh, pDstDetails);
                    if(BERR_SUCCESS != ret)
                    {
                        BDBG_ERR(("BRAP_RemoveDestination: BRAP_P_CloseAndFreeOpPort "
                            " returned %d for PrimaryDec[%d]", ret, i));
                        goto exit;
                    }
                }
            }

            for(i=0;i<BRAP_MAX_SEC_DEC_CHAN_IN_ASSOCIATED_GRP;i++)
            {
                hRapCh = hAssociatedChan->hSecDecCh[i];
                if(BRAP_P_IsPointerValid((void *)hRapCh))
                {
                    ret = BRAP_P_CloseAndFreeOpPort(hRapCh, pDstDetails);
                    if(BERR_SUCCESS != ret)
                    {
                        BDBG_ERR(("BRAP_RemoveDestination: BRAP_P_CloseAndFreeOpPort "
                            " returned %d for SecondaryDec[%d]", ret, i));
                        goto exit;
                    }
                }
            }

            for(i=0;i<BRAP_MAX_PB_CHAN_IN_ASSOCIATED_GRP;i++)
            {
                hRapCh = hAssociatedChan->hPBCh[i];
                if(BRAP_P_IsPointerValid((void *)hRapCh))
                {
                    ret = BRAP_P_CloseAndFreeOpPort(hRapCh, pDstDetails);
                    if(BERR_SUCCESS != ret)
                    {
                        BDBG_ERR(("BRAP_RemoveDestination: BRAP_P_CloseAndFreeOpPort "
                            " returned %d for PCMPlayback[%d]", ret, i));
                        goto exit;
                    }
                }
            }

            for(i=0;i<BRAP_MAX_CAP_CHAN_IN_ASSOCIATED_GRP;i++)
            {
                hRapCh = hAssociatedChan->hCapCh[i];
                if(BRAP_P_IsPointerValid((void *)hRapCh))
                {
                    ret = BRAP_P_CloseAndFreeOpPort(hRapCh, pDstDetails);
                    if(BERR_SUCCESS != ret)
                    {
                        BDBG_ERR(("BRAP_RemoveDestination: BRAP_P_CloseAndFreeOpPort "
                            " returned %d for PCMCapture[%d]", ret, i));
                        goto exit;
                    }
                }
            }    

            /* Clear the entries in hAssociatedChan as well */
            hAssociatedChan->uiNumDstGrp--;
            for(i =0 ;i < BRAP_P_MAX_RAPCH_PER_DST+1;i++)
            {
            /* Note that psProcessingSettings is allocated in the function BRAP_SetPostProcessingStages,
            but since its without removiing the destination stage can be added and removed, so the
            parameter psProcessingSettings is used on each addition . so rather than freeing it each 
            time on removal of stage, its freed in Removedestiantion*/
                if(!(BRAP_P_IsPointerValid((void *)hDestHandle)))
                    break;

                if(BRAP_P_IsPointerValid((void *)hDestHandle->psProcessingSettings[i]))
                {
                    for(j =0 ;j <BRAP_MAX_PP_PER_BRANCH_SUPPORTED;j++)
                    {                
                        if(BRAP_P_IsPointerValid((void *)hDestHandle->psProcessingSettings[i]->sExternalSettings.hAudProcessing[j] ))
                        {
                            for(k =0 ;k <BRAP_MAX_PP_PER_BRANCH_SUPPORTED;k++)
                            {                                            
                                if(hDestHandle->psProcessingSettings[i]->sExternalSettings.hAudProcessing[j]->hDestHandle[k] == hDestHandle)
                                {
                                    hDestHandle->psProcessingSettings[i]->sExternalSettings.hAudProcessing[j]->hDestHandle[k] = NULL;
                                    break;
                                }
                            }
                            hDestHandle->psProcessingSettings[i]->sExternalSettings.hAudProcessing[j]->uHandle.hRapCh = NULL;                            
                            hDestHandle->psProcessingSettings[i]->sExternalSettings.hAudProcessing[j]->uHandle.hAssociation = NULL;                                                        
                        }
                    }
                    BKNI_Free(hDestHandle->psProcessingSettings[i]);
                    hDestHandle->psProcessingSettings[i] = NULL;
                }
            }
            hDestHandle = NULL;            
            BRAP_P_InitDestination(&(hAssociatedChan->sDstDetails[uiDst]));
            break;

#if (BRAP_7550_FAMILY !=1)
        case BRAP_AudioDst_eRingBuffer:
            
            /* Uninstall interrupts */
        	ret = BRAP_P_DestinationLevelInterruptUnInstall(hDestHandle);
            if(BERR_SUCCESS != ret)
            {
                BDBG_ERR(("BRAP_RemoveDestination: BRAP_P_CloseAndFreeOpPort "
                    " returned %d for PrimaryDec[%d]", ret, i));
                goto exit;
            }

            ret = BRAP_P_CloseAndFreeRbuf(hAssociatedChan, pDstDetails);
            if(BERR_SUCCESS != ret)
            {
                BDBG_ERR(("BRAP_RemoveDestination: BRAP_P_CloseAndFreeRbuf "
                    " returned %d for Association Handle %x", ret, hAssociatedChan));
                goto exit;
            }
            
            /* Clear the entries in hAssociatedChan as well */
            hAssociatedChan->uiNumDstGrp--;

            /*
            Remove all the audio processing stages added on this destination.
            Update handles of all the audio processing stages getting removed.
            */

            for(i =0 ;i < BRAP_P_MAX_RAPCH_PER_DST+1;i++)
            {
            /* Note that psProcessingSettings is allocated in the function BRAP_SetPostProcessingStages,
            but since its without removiing the destination stage can be added and removed, so the
            parameter psProcessingSettings is used on each addition . so rather than freeing it each 
            time on removal of stage, its freed in Removedestiantion*/
                if(!(BRAP_P_IsPointerValid((void *)hDestHandle)))
                    break;
                if(BRAP_P_IsPointerValid((void *)hDestHandle->psProcessingSettings[i]))
                {
                    for(j =0 ;j <BRAP_MAX_PP_PER_BRANCH_SUPPORTED;j++)
                    {                
                        if(BRAP_P_IsPointerValid((void *)hDestHandle->psProcessingSettings[i]->sExternalSettings.hAudProcessing[j] ))
                        {
                            for(k =0 ;k <BRAP_MAX_PP_PER_BRANCH_SUPPORTED;k++)
                            {                                            
                                if(hDestHandle->psProcessingSettings[i]->sExternalSettings.hAudProcessing[j]->hDestHandle[k] == hDestHandle)
                                {
                                    hDestHandle->psProcessingSettings[i]->sExternalSettings.hAudProcessing[j]->hDestHandle[k] = NULL;
                                    break;
                                }
                            }
                            hDestHandle->psProcessingSettings[i]->sExternalSettings.hAudProcessing[j]->uHandle.hRapCh = NULL;                            
                            hDestHandle->psProcessingSettings[i]->sExternalSettings.hAudProcessing[j]->uHandle.hAssociation = NULL;                                                        
                        }
                    }
                    BKNI_Free(hDestHandle->psProcessingSettings[i]);
                    hDestHandle->psProcessingSettings[i] = NULL;                    
                }
            }
            hDestHandle = NULL;
            BRAP_P_InitDestination(&(hAssociatedChan->sDstDetails[uiDst]));            
            break;
#endif            
        case BRAP_AudioDst_eMax:
        default:
            BDBG_ERR(("BRAP_RemoveDestination: eAudioDst = %d not supported",
            pDstDetails->eAudioDst));
            return BERR_TRACE(BERR_NOT_SUPPORTED);
    }/* switch eAudioDst */

exit:
	BDBG_LEAVE(BRAP_RemoveDestination);
	return ret;
}


/***************************************************************************
Summary:
	Gets the default params for a channel type.
	
Description:
	This API returns the default parameters for an audio channel type. 
	
Returns:
	BERR_SUCCESS 
	
See Also:
	BRAP_StartChannel
***************************************************************************/
BERR_Code BRAP_GetDefaultChannelParams(
	BRAP_ChannelHandle      hRapCh,		/* [in] The RAP channel handle */
	BRAP_ChannelParams      *pDefParams	/* [out] Default channel parameters */
	)
{
    BERR_Code                   ret = BERR_SUCCESS;
    BRAP_DSPCHN_P_AudioParams   *pDspChParams = NULL;
    unsigned int                i=0,j=0;

	BDBG_ENTER(BRAP_GetDefaultChannelParams);
	
	/* Assert the function argument(s) */
	BDBG_ASSERT(hRapCh);
	BDBG_ASSERT(pDefParams);

    /* Timebase */
    pDefParams->eTimebase = BAVC_Timebase_e0;

    /* eAudioSource */
    switch(hRapCh->eChannelType)
    {
        case BRAP_ChannelType_eDecode:
            pDefParams->eAudioSource = BRAP_AudioSource_eXptInterface;
            break;
        case BRAP_ChannelType_ePcmPlayback:
            pDefParams->eAudioSource = BRAP_AudioSource_eRingBuffer;
            break;
        case BRAP_ChannelType_ePcmCapture:
            pDefParams->eAudioSource = BRAP_AudioSource_eExtCapPort;
            break;
#ifdef RAP_VIDEOONDSP_SUPPORT            
        case BRAP_ChannelType_eVideoDecode:
            break;           
#endif            
        default:
            BDBG_ERR(("BRAP_GetDefaultChannelParams: Unsupported eChType = %d",
                hRapCh->eChannelType));        
            return BERR_TRACE(ret); 
    }/* switch */
    
	/* Malloc large local structures */
	pDspChParams = hRapCh->hRap->sOpenTimeMallocs.pDspChParams;
	if( NULL==pDspChParams)
	{
		return BERR_TRACE( BERR_OUT_OF_SYSTEM_MEMORY );
	}
       BKNI_Memset(pDspChParams,0,sizeof( BRAP_DSPCHN_P_AudioParams ));    	                
    /* bPlayback */
    pDefParams->bPlayback = false;
    pDefParams->eInputAudMode = BRAP_OutputMode_e2_0;
    pDefParams->bInputLfePresent = false;

    /* Input Sampling Rate */
    pDefParams->eInputSR = BAVC_AudioSamplingRate_eUnknown;

    /* Fill up sXptContextMap with invalid values, since PI can't provide these
       values */
    BKNI_Memset(&(pDefParams->sXptContextMap),
                BRAP_INVALID_VALUE,
                sizeof(BAVC_XptContextMap));

    /* Get DSP Channel default parameters */
    ret = BRAP_DSPCHN_P_GetDefaultParams(pDspChParams);
    if(BERR_SUCCESS != ret)
	{
        return BERR_TRACE(ret); 
    }
    else
    {
        pDefParams->sDspChParams = pDspChParams->sExtAudioParams;
#ifdef RAP_VIDEOONDSP_SUPPORT
        pDefParams->sVideoParams.ui32DispStripeHeight = pDspChParams->sExtVideoParams.ui32DispStripeHeight;
        pDefParams->sVideoParams.ui32DispStripeWidth = pDspChParams->sExtVideoParams.ui32DispStripeWidth;
        pDefParams->sVideoParams.ui32NumBuffAvl = pDspChParams->sExtVideoParams.ui32NumBuffAvl;
#endif        

#if (BRAP_3548_FAMILY == 1)
        if (BRAP_ChannelType_ePcmCapture == hRapCh->eChannelType)
        {
            pDefParams->sDspChParams.eDecodeMode = BRAP_DSPCHN_DecodeMode_eDecode;
            pDefParams->sDspChParams.eType = BRAP_DSPCHN_AudioType_ePcm;
        }
#endif        
    }

    pDefParams->eCapMode = BRAP_CaptureMode_eMaxCaptureMode;

    pDefParams->eCapInputPort = BRAP_CapInputPort_eMax;

    pDefParams->eBufDataMode = BRAP_BufDataMode_eStereoNoninterleaved;
    pDefParams->eInputBitsPerSample = BRAP_InputBitsPerSample_e32;
    pDefParams->bLoopAroundEn = false;

    pDefParams->sBufParams.pLeftBufferStart = NULL; 
    pDefParams->sBufParams.pRightBufferStart = NULL; 
	pDefParams->sBufParams.uiSize = BRAP_RBUF_P_DEFAULT_SIZE;
	pDefParams->sBufParams.uiWaterMark = BRAP_PB_P_RBUF_DEFAULT_FREE_BYTE_MARK;

    pDefParams->uiStartWRPtrLocation = BRAP_INVALID_VALUE;

    /* Fill up sMixingCoeff with invalid values, since PI can't provide these
       values */
    for(i=0;i<BRAP_OutputChannel_eMax;i++)
    {
        for(j=0;j<BRAP_OutputChannel_eMax;j++)
        {
            if(i == j)
            {
                pDefParams->sMixingCoeff.ui32Coef[i][j]=BRAP_MIXER_P_DEFAULT_SCALING_COEFF;
            }
            else
            {
                pDefParams->sMixingCoeff.ui32Coef[i][j]=0;
            }
        }
    }

    pDefParams->bIsPcmSigned = true;
 
/* If you enable this, please goto brap_priv.h, & modify BRAP_P_MixingLevelProp
   so that we have the element like BRAP_DstDetails *pDstDetails[BRAP_RM_P_MAX_PARALLEL_PATHS];*/

#if ((BRAP_3548_FAMILY == 1)||(BRAP_7405_FAMILY == 1))
    pDefParams->hAdFade = NULL;
#endif 
    pDefParams->eDataSwap = BRAP_DataEndian_eSame;
    pDefParams->eLowDelayEnableMode = BRAP_CIT_P_LowDelayMode_eDisabled;
    pDefParams->bEnableFixedSampleRateOutput = false;

	BDBG_LEAVE(BRAP_GetDefaultChannelParams);
	return BERR_SUCCESS;
}


/***************************************************************************
Summary:
	Starts an audio channel.
	
Description:
	This API is required to start the selected channel.

ALGORITHM
    0. Validate input parameters
    1. Identify which all paths are required
    2. Open each of these paths
        - Allocate resources required 
        - Open these resources
    3. Start each of these paths    
        - Start each of these resources
	
Returns:
	BERR_SUCCESS 
	
See Also:
	BRAP_DEC_Stop
****************************************************************************/
BERR_Code BRAP_StartChannel ( 
	BRAP_ChannelHandle 			hRapCh,		    /* [in] RAP Channel handle */
	const BRAP_ChannelParams	*pAudioParams	/* [in] Audio parameters for 
	                                               starting this channel */
	)
{
    BERR_Code ret = BERR_SUCCESS;
    bool bWdgRecovery = false;
    unsigned int   i=0, l = 0, k = 0;

    int  uiPth = 0;
    unsigned int uiNumCapPath = 0;
#if (BRAP_7405_FAMILY == 1)
    BTMR_Settings sTimerSettings;
    uint32_t ui32RegVal=0;
    BRAP_OutputPortConfig	*pOpConfig=NULL;
#endif


    bool bDstAdded = false;
    bool bCompressedDstAdded = false;
    BRAP_ProcessingStageSettings * psProcessingStageSettings=    NULL;

	BDBG_ENTER(BRAP_StartChannel);

	/* Validate parameters */
	BDBG_ASSERT(hRapCh);
	BDBG_ASSERT(pAudioParams);
    
    BDBG_MSG(("============================"));
    BDBG_MSG(("BRAP_StartChannel: hRapCh = 0x%x" 
              "\n\t Channel Type = %d"
              "\n\t Channel Subtype = %d",
                hRapCh, hRapCh->eChannelType, hRapCh->eChannelSubType));
    BDBG_MSG(("BRAP_StartChannel:"
        "\n\t Parameters:"
        "\n\t eTimebase = %d"
        "\n\t eAudioSource = 0x%x"
        "\n\t eInputSR = 0x%x"
        "\n\t bInputLfePresent = 0x%x"
        "\n\t eInputAudMode = 0x%x"
        "\n\t eInputBitsPerSample = 0x%x"
        "\n\t sXptContextMap.ContextIdx = 0x%x"
        "\n\t bPlayback = 0x%x"
        "\n\t sDspChParams.eDecodeMode = 0x%x"
        "\n\t sDspChParams.eType = 0x%x"
        "\n\t sDspChParams.eStreamType = 0x%x"
	    "\n\t ePBrate=0x%x"
        "\n\t sDspChParams.i32AVOffset = 0x%x"
        "\n\t eCapMode = 0x%x"
        "\n\t eCapInputPort = 0x%x"
        "\n\t eBufDataMode = 0x%x"
        "\n\t eInputChPair = 0x%x",
        pAudioParams->eTimebase, pAudioParams->eAudioSource, 
        pAudioParams->eInputSR, pAudioParams->bInputLfePresent, 
        pAudioParams->eInputAudMode, pAudioParams->eInputBitsPerSample,
        pAudioParams->sXptContextMap.ContextIdx,
        pAudioParams->bPlayback,pAudioParams->sDspChParams.eDecodeMode,
        pAudioParams->sDspChParams.eType, pAudioParams->sDspChParams.eStreamType,
        pAudioParams->sDspChParams.uiPBRate,
        pAudioParams->sDspChParams.i32AVOffset, pAudioParams->eCapMode, 
        pAudioParams->eCapInputPort, pAudioParams->eBufDataMode,
        pAudioParams->eInputChPair));
    BDBG_MSG(("============================"));

#ifdef RAP_VIDEOONDSP_SUPPORT
        if(hRapCh->eChannelType == BRAP_ChannelType_eVideoDecode)
        {
            return  BRAP_P_StartVideoChannel(hRapCh,pAudioParams);
        }
#endif

    /* Check if this is a watchdog recovery. */
	bWdgRecovery = BRAP_P_GetWatchdogRecoveryFlag(hRapCh->hRap);

#ifdef BCHP_PWR_RESOURCE_RAP_START
    /* the first call to BRAP_StartChannel will power up the rest of the block */
    if (!bWdgRecovery && hRapCh->eState != BRAP_P_State_eStarted) {
        BCHP_PWR_AcquireResource(hRapCh->hChip, BCHP_PWR_RESOURCE_RAP_START);
    }
#endif
  
/*Making Error Count to 0*/
    hRapCh->uiTotalErrorCount = 0;

    hRapCh->eSamplingRate = BAVC_AudioSamplingRate_eUnknown;    
    hRapCh->bGateOpened= false;
    hRapCh->uiModeValue = BRAP_INVALID_VALUE;


    if(BRAP_ChannelType_eDecode == hRapCh->eChannelType)
    {
#if (BRAP_3548_FAMILY == 1)    
        if (BRAP_AudioSource_eXptInterface == pAudioParams->eAudioSource)
#endif            
        {
        if(!((BRAP_P_IsPassthruSupportedWithoutLicense(pAudioParams->sDspChParams.eType))&&
            (BRAP_DSPCHN_DecodeMode_ePassThru == pAudioParams->sDspChParams.eDecodeMode)))
        {
            if(false == BRAP_FWDWNLD_P_IsAudCodecSupported(pAudioParams->sDspChParams.eType))
            {
                BDBG_ERR(("Please export RAP_XXXX_SUPPORT=y at the compile time for Algo Id = %d, where XXXX is Algo Name e.g AC3,MPEG,DDP etc"
                    ,pAudioParams->sDspChParams.eType));
                ret = BERR_TRACE(BERR_NOT_SUPPORTED);
                goto end;
            }
        }
    }
    }

    if((BRAP_ChannelType_eDecode == hRapCh->eChannelType)
        &&((pAudioParams->sDspChParams.uiPBRate < (BRAP_DSPCHN_PLAYBACKRATE_NORMAL/2))
        ||(pAudioParams->sDspChParams.uiPBRate > BRAP_DSPCHN_PLAYBACKRATE_NORMAL*2)))
    {
        BDBG_ERR(("Plaback rate can vary between 0.5x to 2x, So its value should between 50 to 200. Programmed value = %d is not in range."
            ,pAudioParams->sDspChParams.uiPBRate));
            ret = BERR_TRACE(BERR_INVALID_PARAMETER);                
            goto end;
    }

        	/* Simulmode and PassThru are not supported for following algorithms.
    	 * AAC-SBR, LPCM-BD, LPCM-DVD, LPCM HD-DVD, WMA, MLP, DtsLbr
    	 * Return error if decode mode is simulmode or passthru for these algorithms.
    	 */
        if ((BRAP_DSPCHN_AudioType_eLpcmBd == pAudioParams->sDspChParams.eType) ||
    	    (BRAP_DSPCHN_AudioType_eLpcmHdDvd == pAudioParams->sDspChParams.eType) ||
    	    (BRAP_DSPCHN_AudioType_eLpcmDvd == pAudioParams->sDspChParams.eType) ||
    	    (BRAP_DSPCHN_AudioType_eWmaStd == pAudioParams->sDspChParams.eType) ||
    	    (BRAP_DSPCHN_AudioType_eWmaPro== pAudioParams->sDspChParams.eType) ||            	    
    	    (BRAP_DSPCHN_AudioType_eDtsLbr == pAudioParams->sDspChParams.eType)||
            (BRAP_DSPCHN_AudioType_ePcm== pAudioParams->sDspChParams.eType)||    	    
    	    (BRAP_DSPCHN_AudioType_ePcmWav== pAudioParams->sDspChParams.eType)||
    	    (BRAP_DSPCHN_AudioType_eAmr == pAudioParams->sDspChParams.eType))
            {
            if ((pAudioParams->sDspChParams.eDecodeMode == BRAP_DSPCHN_DecodeMode_ePassThru))
                {
                BDBG_ERR(("Compressed data format not supported for algorithm %d",
                	pAudioParams->sDspChParams.eType));
                ret = BERR_TRACE(BRAP_ERR_COMP_SPDIF_NOT_ALLOWED);
                goto end;                
            }
        }
        
    /* Check if any destination is added to this channel else return error */
    for(i=0;i<BRAP_P_MAX_DST_PER_RAPCH;i++)
    {
        if(!(BRAP_P_IsPointerValid((void *)hRapCh->pDstDetails[i])))
        {
            continue;
        }
        
        if(BRAP_AudioDst_eMax == hRapCh->pDstDetails[i]->eAudioDst)
        {
            bDstAdded = false;
        }
        else
        {
            bDstAdded = true;
            break;
        }
    }

    if((false == bDstAdded))
    {
        BDBG_ERR(("BRAP_StartChannel: No destination added to the channel."
                  "Please call BRAP_AddDestination() before channel start"));
        ret = BERR_TRACE(BERR_NOT_INITIALIZED);
        goto end;        
    }

    /* Check for unsupported destinations of passthru channels */
    if(BRAP_DSPCHN_DecodeMode_ePassThru == pAudioParams->sDspChParams.eDecodeMode)
    {
        bCompressedDstAdded = true;
        for(i=0;i<BRAP_P_MAX_DST_PER_RAPCH;i++)
        {
            if(!(BRAP_P_IsPointerValid((void *)hRapCh->pDstDetails[i])))
            {
                continue;
            }
            
            if((BRAP_AudioDst_eMax == hRapCh->pDstDetails[i]->eAudioDst)||
                (BRAP_AudioDst_eRingBuffer == hRapCh->pDstDetails[i]->eAudioDst))
            {
                continue;
            }

            if((BRAP_OutputPort_eSpdif != hRapCh->pDstDetails[i]->uDstDetails.sOpDetails.eOutput[0])&&
                (BRAP_OutputPort_eSpdif1 != hRapCh->pDstDetails[i]->uDstDetails.sOpDetails.eOutput[0])&&
                (BRAP_OutputPort_eMai != hRapCh->pDstDetails[i]->uDstDetails.sOpDetails.eOutput[0]))
            {
                bCompressedDstAdded = false;
                break;
            }
        }
        if(false == bCompressedDstAdded)
        {
            BDBG_ERR(("BRAP_StartChannel: PCM destination added to the channel."
                      "Please add compressed destinations before channel start"));
            ret = BERR_TRACE(BERR_NOT_INITIALIZED);
            goto end;            
        }
    }


    if(pAudioParams->bEnableFixedSampleRateOutput ==true)
    {
        for(i=0;i<BRAP_P_MAX_DST_PER_RAPCH;i++)
        {
            if(!(BRAP_P_IsPointerValid((void *)hRapCh->pDstDetails[i])))
            {
                continue;
            }
            if(hRapCh->hRap->sOutputSettings[hRapCh->pDstDetails[i]->uDstDetails.sOpDetails.eOutput[0]].bCompressed==true)
            {
                BDBG_ERR(("BRAP_StartChannel: Compressed output is found. "
                          "Please note that if bEnableFixedSampleRateOutput=true, then compressed output is not supported"));
                ret = BERR_TRACE(BERR_INVALID_PARAMETER);        
                goto end;                
            }
        }
    }

    

	if ( hRapCh->eChannelType == BRAP_ChannelType_ePcmCapture )
	{
            if ( (BRAP_AudioSource_eExtCapPort == pAudioParams->eAudioSource)  &&
                    ((BRAP_CapInputPort_eSpdif == pAudioParams->eCapInputPort) ||
                     (BRAP_CapInputPort_eHdmi  == pAudioParams->eCapInputPort)
                    ) 
               )
        {
			ret = BERR_INVALID_PARAMETER;
			BDBG_ERR(("BRAP_StartChannel: Capture Channel cannot have Spdif/hdmi receiver as input"));
			ret = BERR_TRACE(ret);
                    goto end;                        
        }
	}

    if ((BRAP_ChannelType_eDecode==hRapCh->eChannelType)
        &&(BRAP_DSPCHN_DecodeMode_ePassThru==pAudioParams->sDspChParams.eDecodeMode)
        &&(BRAP_DSPCHN_AudioType_eAc3Plus==pAudioParams->sDspChParams.eType))
    {
#if (BRAP_3548_FAMILY != 1) /* DDP Passthru on 3548 does not go through Mai */    
        BRAP_P_DstDetails *pTempDstDetails = NULL;
        unsigned int j;
        bool bMai = false;
        
        /* DDP Pass through is supported only on HDMI. Check if application is sending
            pass through data on HDMI. Also check if HBR mode was selected during ring buffer
            allocation */
        for (i = 0; i < BRAP_P_MAX_DST_PER_RAPCH; i++)
        {
            for(k=0;k<BRAP_MAX_ASSOCIATED_GROUPS;k++)
            {
                if(hRapCh->uiAssociationId[k] == BRAP_INVALID_VALUE)
                    continue;
                pTempDstDetails = &(hRapCh->hRap->sAssociatedCh[hRapCh->uiAssociationId[k]].sDstDetails[i]);
            if (BRAP_AudioDst_eOutputPort==pTempDstDetails->sExtDstDetails.eAudioDst)
            {
                for (j = 0; j < BRAP_OutputChannelPair_eMax; j++)
                {
                    if (BRAP_OutputPort_eMai==pTempDstDetails->sExtDstDetails.uDstDetails.sOpDetails.eOutput[j])
                    {
                        bMai = true;
                        break;
                    }
                }
            }
            if (true==bMai)
                break;
        }
        }
        if (false==bMai)
        {
            BDBG_ERR(("DDP Pass through is supported only on MAI port. This channel doesn't"
                "feed data to HDMI port"));
            ret = BERR_TRACE(BERR_NOT_SUPPORTED);
            goto end;            
        }
#endif /* BRAP_3548_FAMILY != 1 */        
        /* check if HBR mode was selected during ring buffer allocation */
        if ((BRAP_P_IsPointerValid((void *)hRapCh->hRap->sDeviceRBufPool.pExtRBufPoolSettings))
            &&(false==hRapCh->hRap->sDeviceRBufPool.pExtRBufPoolSettings->bHbrMode))
        {
            BDBG_ERR(("DDP pass through needs large ring buffer. Please select bHbrMode"
                "in open time device ring buffer allocation. Set BRAP_Settings.sDeviceRBufPool.bHbrMode = true"));
            ret = BERR_TRACE(BERR_NOT_SUPPORTED);
            goto end;
        }
    }

    #if (BRAP_7405_FAMILY == 1)
    /* This implementaion is to fix a case where Dts XCD to PCm transition on 
    HDMI soesn't happen on Onkyo reciever when we don't diable MAI_CFG at MAI_STOP*/
        if((hRapCh->eChannelType == BRAP_ChannelType_eDecode)
            &&(pAudioParams->sDspChParams.eDecodeMode == BRAP_DSPCHN_DecodeMode_eDecode)
            &&(hRapCh->hRap->bLastRunHadDtsTranscodeOnHdmi == true)
            &&(hRapCh->hRap->sOutputSettings[BRAP_OutputPort_eMai].bCompressed == false)
            &&(hRapCh->hRap->sOutputSettings[BRAP_OutputPort_eMai].uOutputPortSettings.sMaiSettings.eMaiMuxSelector== BRAP_OutputPort_eMai))
        {
            BDBG_MSG(("DISABLING MAI CFG"));    
            hRapCh->hRap->bLastRunHadDtsTranscodeOnHdmi = false;
            ui32RegVal = BRAP_Read32 (hRapCh->hRegister, 
            BCHP_AUD_FMM_OP_CTRL_MAI_CFG);
            ui32RegVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_MAI_CFG,ENABLE_MAI));
            ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_OP_CTRL_MAI_CFG, 
            ENABLE_MAI, 
            Disable));
            BRAP_Write32 (hRapCh->hRegister, 
            BCHP_AUD_FMM_OP_CTRL_MAI_CFG, 
            ui32RegVal);        
        }    
        else if(hRapCh->hRap->bLastRunHadDtsTranscodeOnHdmi == true)
        {
            hRapCh->hRap->bLastRunHadDtsTranscodeOnHdmi = false;        
        }
    
        if((hRapCh->eChannelType == BRAP_ChannelType_eDecode)
            &&(pAudioParams->sDspChParams.eDecodeMode == BRAP_DSPCHN_DecodeMode_ePassThru)
            &&(hRapCh->hRap->sOutputSettings[BRAP_OutputPort_eMai].bCompressed == true)
            &&( (pAudioParams->sDspChParams.eType== BRAP_DSPCHN_AudioType_eDtsBroadcast) ||
                (pAudioParams->sDspChParams.eType== BRAP_DSPCHN_AudioType_eDtshd)))
        {
            hRapCh->hRap->bLastRunHadDtsTranscodeOnHdmi = true;        
        }   

    pOpConfig=(BRAP_OutputPortConfig    *)BKNI_Malloc(sizeof(BRAP_OutputPortConfig));
    BKNI_Memset(pOpConfig,0,sizeof(BRAP_OutputPortConfig));
    BRAP_GetCurrentOutputConfig(hRapCh->hRap,BRAP_OutputPort_eSpdif,pOpConfig);
    if(pOpConfig->uOutputPortSettings.sSpdifSettings.eMClkRate != BRAP_OP_MClkRate_e128Fs)
    {    
        pOpConfig->uOutputPortSettings.sSpdifSettings.eMClkRate = BRAP_OP_MClkRate_e128Fs;
		ret = BRAP_SetOutputConfig(hRapCh->hRap,pOpConfig);        
		if(ret != BERR_SUCCESS)
    	{
      		BDBG_ERR(("SPDIF Port MCLK configuration failed"));
      		return BERR_TRACE(ret);
    	}       
    }
    /* This implementaion is to fix a case where Dts XCD to PCm transition on 
    SPDIF soesn't happen on Onkyo reciever. For this MCLK needs to be disturbed. Since FW 
    programs 128Fs. PI should program 256fsin this case*/
        if((hRapCh->eChannelType == BRAP_ChannelType_eDecode)
            &&(pAudioParams->sDspChParams.eDecodeMode == BRAP_DSPCHN_DecodeMode_eDecode)
            &&(hRapCh->hRap->bLastRunHadDtsTranscodeOnSpdif == true)
            &&(hRapCh->hRap->sOutputSettings[BRAP_OutputPort_eSpdif].bCompressed == false))
        {
            hRapCh->hRap->bLastRunHadDtsTranscodeOnSpdif = false;
            BKNI_Memset(pOpConfig,0,sizeof(BRAP_OutputPortConfig));
            BRAP_GetCurrentOutputConfig(hRapCh->hRap,BRAP_OutputPort_eSpdif,pOpConfig);
            pOpConfig->uOutputPortSettings.sSpdifSettings.eMClkRate = BRAP_OP_MClkRate_e256Fs;

			ret = BRAP_SetOutputConfig(hRapCh->hRap,pOpConfig);  
            if(ret != BERR_SUCCESS)
    		{
      			BDBG_ERR(("SPDIF Port MCLK(256Fs)configuration for ONKYO receiver failed"));
      			return BERR_TRACE(ret);
    		}       
        }    
        else if(hRapCh->hRap->bLastRunHadDtsTranscodeOnSpdif == true)
        {
            hRapCh->hRap->bLastRunHadDtsTranscodeOnSpdif= false;            
        }        
        
        BKNI_Free(pOpConfig);
    
        if((hRapCh->eChannelType == BRAP_ChannelType_eDecode)
            &&(pAudioParams->sDspChParams.eDecodeMode == BRAP_DSPCHN_DecodeMode_ePassThru)
            &&(hRapCh->hRap->sOutputSettings[BRAP_OutputPort_eSpdif].bCompressed == true)
            &&( (pAudioParams->sDspChParams.eType== BRAP_DSPCHN_AudioType_eDtsBroadcast) ||
                (pAudioParams->sDspChParams.eType== BRAP_DSPCHN_AudioType_eDtshd)))
        {
            hRapCh->hRap->bLastRunHadDtsTranscodeOnSpdif = true;        
        }           
#endif    

    if(false == bWdgRecovery) 
    {
        if(false == hRapCh->bInternalCallFromRap)
        {
            if(pAudioParams->eInputSR != BAVC_AudioSamplingRate_eUnknown)
            {
             BRAP_P_ConvertSR(pAudioParams->eInputSR,&(hRapCh->uiInputSamplingRate));
            }
            else
            {
                hRapCh->uiInputSamplingRate =48000;
            }
            hRapCh->eAudioSource = pAudioParams->eAudioSource;
        	hRapCh->eCapInputPort = pAudioParams->eCapInputPort;
            hRapCh->eCapMode = pAudioParams->eCapMode;

            hRapCh->hAdFade = pAudioParams->hAdFade;
             hRapCh->uiPBRate = pAudioParams->sDspChParams.uiPBRate;
            hRapCh->bEnableFixedSampleRateOutput = pAudioParams->bEnableFixedSampleRateOutput;            

        }

        /* If channel is already started return success */
        if(BRAP_P_State_eStarted == hRapCh->eState) 
        {
            BDBG_WRN(("Channel already started hRapCh->eState = %d", hRapCh->eState));
            ret = BERR_TRACE(BERR_SUCCESS);
            goto end;            
        }

        for(l=0;l<BRAP_RM_P_MAX_MIXER_PER_DP_BLCK * BRAP_RM_P_MAX_DP_BLCK;l++)
        {
            for(k=0;k<BRAP_P_MAX_DST_PER_RAPCH;k++)
            {
                hRapCh->sScalingInfo.eOp[l][k] = BRAP_OutputPort_eMax;
                for(i=0;i<BRAP_OutputChannelPair_eMax;i++)
                {
                    hRapCh->sScalingInfo.sScalingInfo[l][k].ui32LeftGain[i] = BRAP_MIXER_P_DEFAULT_SCALING_COEFF;
                    hRapCh->sScalingInfo.sScalingInfo[l][k].ui32RightGain[i]= BRAP_MIXER_P_DEFAULT_SCALING_COEFF;
                }
            }
        }
        
#if (BRAP_7405_FAMILY == 1)
                if((hRapCh->eChannelType == BRAP_ChannelType_eDecode)
                    &&(pAudioParams->sDspChParams.eDecodeMode == BRAP_DSPCHN_DecodeMode_eDecode))
                {
                        BTMR_GetDefaultTimerSettings(&sTimerSettings);
                        sTimerSettings.type = BTMR_Type_eCountDown;
                        sTimerSettings.cb_isr = (BTMR_CallbackFunc) BRAP_P_RampEnableTimer_isr;
                        sTimerSettings.pParm1 = hRapCh;
                        sTimerSettings.parm2 = 0;
                        sTimerSettings.exclusive = false;
                        BTMR_CreateTimer (hRapCh->hRap->hTmr, &hRapCh->hTimer, &sTimerSettings);
                }
#endif

        
	}

    

    if(false == bWdgRecovery)
    {
    if ( (BRAP_ChannelType_eDecode == hRapCh->eChannelType)
#if (BRAP_3548_FAMILY == 1)
         || (BRAP_ChannelType_ePcmCapture== hRapCh->eChannelType)        
#endif         
       )
    {
                BRAP_FWIF_P_FormProcessingNetworks(hRapCh,true,pAudioParams->sDspChParams.eDecodeMode,pAudioParams->sDspChParams.eType);
    }
    else if (  (BRAP_ChannelType_ePcmPlayback== hRapCh->eChannelType)
#if (BRAP_3548_FAMILY != 1)        
               || (BRAP_ChannelType_ePcmCapture== hRapCh->eChannelType)
#endif               
             )
    {
                BRAP_FWIF_P_FormProcessingNetworks(hRapCh,false,pAudioParams->sDspChParams.eDecodeMode,pAudioParams->sDspChParams.eType);
    }
    }
    if(hRapCh->bInternalCallFromRap == false)
    {    
        /* Get all the channel paths required for this channel */
        /* This function should also populate the inlink, outlink, self link
           for all the paths required in this channel */
        ret = BRAP_P_GetChannelPaths(hRapCh, pAudioParams,&uiNumCapPath);
        if(ret != BERR_SUCCESS) 
        {
            BDBG_ERR(("BRAP_StartChannel: GetChannelPaths returned %d",ret));
            ret = BERR_TRACE(ret);
            goto end;            
        }
        BDBG_MSG(("uiNumCapPath > %d", uiNumCapPath));

        /* Open all the paths in this channel */
        for(uiPth=0; uiPth<BRAP_P_MAX_PATHS_IN_A_CHAN; uiPth++)
        {
            if(!(BRAP_P_IsPointerValid((void *)hRapCh->pPath[uiPth])))
            {
                continue;
            }
            if(false == bWdgRecovery)
            {
                hRapCh->pPath[uiPth]->hFmm = hRapCh->hRap->hFmm[0];
            }
            switch(hRapCh->pPath[uiPth]->eUsgPath)
            {
                case BRAP_P_UsgPath_eDecodePcm:
                case BRAP_P_UsgPath_eDecodeCompress:
                case BRAP_P_UsgPath_ePPBranch:
                case BRAP_P_UsgPath_eMixPath:
                case BRAP_P_UsgPath_eSharedPP:
                case BRAP_P_UsgPath_eDownmixedPath:                    
                case BRAP_P_UsgPath_eDownmixedMixPath:                       
                case BRAP_P_UsgPath_eDecodePcmPostMixing:
                case BRAP_P_UsgPath_ePPBranchPostMixing:        
                case BRAP_P_UsgPath_eDecodeCompressPostMixing:
    		        BDBG_MSG(("OpenDecPath"));
                    ret = BRAP_P_OpenDecPath(hRapCh, pAudioParams, uiPth, uiNumCapPath);
                    if(ret != BERR_SUCCESS) 
                    {
                        BDBG_ERR(("BRAP_StartChannel: OpenDecPath returned %d",ret));
                        ret = BERR_TRACE(ret);
                        goto end;                        
                    }
                    break;
#if (BRAP_7550_FAMILY != 1)
                case BRAP_P_UsgPath_eCapture:
    		        BDBG_MSG(("OpenCapPath"));
                    ret = BRAP_P_OpenCapPath(hRapCh, pAudioParams, uiPth);
                    if(ret != BERR_SUCCESS) 
                    {
                        BDBG_ERR(("BRAP_ChannelStart: OpenCapPath returned %d",ret));
                        ret = BERR_TRACE(ret);
                        goto end;                        
                    }
                    break;
#endif                    
                default:
                    BDBG_MSG(("BRAP_StartChannel: Path Open# Unsupported pPath[%d].eUsgPath = %d",
                        uiPth, hRapCh->pPath[uiPth]->eUsgPath));
                    /* Do nothing */
                    break;
            }
        }/* uiPth */
    }/* if bInternalCallFromRap == false */

    /* Start all the paths in this channel */
    for(uiPth=BRAP_P_MAX_PATHS_IN_A_CHAN-1; uiPth>=0; uiPth--)
    {
        if(!(BRAP_P_IsPointerValid((void *)hRapCh->pPath[uiPth])))
        {
            continue;
        }
        switch(hRapCh->pPath[uiPth]->eUsgPath)
        {
            case BRAP_P_UsgPath_eDecodePcm:
            case BRAP_P_UsgPath_eDecodeCompress:
            case BRAP_P_UsgPath_ePPBranch:
            case BRAP_P_UsgPath_eMixPath:
            case BRAP_P_UsgPath_eSharedPP:                
            case BRAP_P_UsgPath_eDownmixedPath:      
            case BRAP_P_UsgPath_eDownmixedMixPath:                                       
            case BRAP_P_UsgPath_eDecodePcmPostMixing:
            case BRAP_P_UsgPath_ePPBranchPostMixing:                
            case BRAP_P_UsgPath_eDecodeCompressPostMixing:
			    BDBG_MSG(("\n StartDecPath uiPth=%d \n", uiPth));
                ret = BRAP_P_StartDecPath(hRapCh, pAudioParams, uiPth);
                if(ret != BERR_SUCCESS) 
                {
                    BDBG_ERR(("BRAP_StartChannel: StartDecPath returned %d",ret));
                    ret = BERR_TRACE(ret);
                    goto end;                    
                }
                break;  
#if (BRAP_7550_FAMILY != 1)                
            case BRAP_P_UsgPath_eCapture:
			    BDBG_MSG(("\n StartCapPath uiPth=%d \n", uiPth));
                ret = BRAP_P_StartCapPath(hRapCh, pAudioParams, uiPth);
                if(ret != BERR_SUCCESS) 
                {
                    BDBG_ERR(("BRAP_StartChannel: StartCapPath returned %d",ret));
                    ret = BERR_TRACE(ret);
                    goto end;                    
                }
                break; 
#endif                
            default:
                BDBG_MSG(("BRAP_StartChannel: Path Start# Unsupported pPath[%d].eUsgPath = %d",
                    uiPth, hRapCh->pPath[uiPth]->eUsgPath));
                /* Do nothing */
                break;
        }
    }/* uiPth */

    /* Update the channel state */
    if((false == bWdgRecovery)
        && (BERR_SUCCESS == ret))
    {
        hRapCh->eState = BRAP_P_State_eStarted;
    }

    if(BRAP_P_IsFwMixingPostLoopbackEnabled(hRapCh))
    {
        for(i =0 ; i < BRAP_MAX_PP_SUPPORTED; i++)
        {
            if((hRapCh->hRap->hAudioProcessingStageHandle[i] != NULL) &&
               (hRapCh->hRap->hAudioProcessingStageHandle[i]->sProcessingStageSettings.eAudioProcessing == BRAP_ProcessingType_eFwMixer))
            {                                   
                psProcessingStageSettings=(BRAP_ProcessingStageSettings *)BKNI_Malloc(sizeof(BRAP_ProcessingStageSettings));
                if( NULL==psProcessingStageSettings)
                {
                    ret = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                    goto end;        
                }                        
                ret = BRAP_GetDefaultProcessingStageSettings(BRAP_ProcessingType_eFwMixer,psProcessingStageSettings );
                if(ret != BERR_SUCCESS)
                {
                    BDBG_ERR(("BRAP_StartChannel: BRAP_GetDefaultProcessingStageSettings() returned err(%d)",ret));	
                    goto end;
                }                    
                ret = BRAP_SetProcessingStageSettings(hRapCh->hRap->hAudioProcessingStageHandle[i],psProcessingStageSettings);
                if(ret != BERR_SUCCESS)
                {
                    BDBG_ERR(("BRAP_StartChannel: BRAP_SetProcessingStageSettings() returned err(%d)",ret));	
                    goto end;                        
                }                       

                break;
            }
        }
    }
    
#if (BRAP_P_WATERMARK_WORKAROUND == 0)
    if((false == bWdgRecovery)&&(BRAP_ChannelType_ePcmPlayback == hRapCh->eChannelType))
    {
	    /* Unmask the Free Mark interrupt */
	    ret = BRAP_P_UnmaskInterrupt(hRapCh, BRAP_Interrupt_eFmmRbufFreeByte);
	    if(ret != BERR_SUCCESS)
	    {
	        BDBG_ERR(("BRAP_P_StartDecPath: BRAP_P_UnmaskInterrupt() returned err(%d)",ret));	
	    }
    }
#if (BRAP_P_EDGE_TRIG_INTRPT == 1)           
	BRAP_P_ReArmFreeMarkInterrupt(hRapCh);
#endif
#endif 
end:
    if(psProcessingStageSettings)
        BKNI_Free(psProcessingStageSettings);

#ifdef BCHP_PWR_RESOURCE_RAP_START
    if (ret != BERR_SUCCESS) {
        /* a failed StartChannel releases the resource */
        BCHP_PWR_ReleaseResource(hRapCh->hChip, BCHP_PWR_RESOURCE_RAP_START);
    }
#endif

	BDBG_LEAVE(BRAP_StartChannel);
	return ret;
}

/***************************************************************************
Summary:
	Stops a channel

Description:
	This API is required to stop a channel.	

Returns:
	BERR_SUCCESS if successful else error value.            

See Also:
	BRAP_StartChannel
****************************************************************************/
BERR_Code BRAP_StopChannel ( 
	BRAP_ChannelHandle 	        hRapCh		    /* [in] RAP channel handle */
	)
{
    BERR_Code ret = BERR_SUCCESS;
    bool bWdgRecovery = false;
    unsigned int uiPth = 0, i=0;
    BRAP_ProcessingStageSettings * psProcessingStageSettings = NULL;    
	BDBG_ENTER(BRAP_StopChannel);

	/* Validate parameters */
	BDBG_ASSERT(hRapCh);

    /* If channel is already stopped return success */
    if(BRAP_P_State_eStarted != hRapCh->eState) 
    {
        BDBG_WRN(("Channel already stopped hRapCh->eState = %d",hRapCh->eState));
        return BERR_TRACE(BERR_SUCCESS);
    }
    
    BDBG_MSG(("============================"));    
    BDBG_MSG(("BRAP_StopChannel: hRapCh = 0x%x" 
              "\n\t Channel Type = %d"
              "\n\t Channel Subtype = %d",
                hRapCh, hRapCh->eChannelType, hRapCh->eChannelSubType));
    BDBG_MSG(("============================"));
    
    /* Check if this is a watchdog recovery. */
	bWdgRecovery = BRAP_P_GetWatchdogRecoveryFlag(hRapCh->hRap);
    
#ifdef RAP_VIDEOONDSP_SUPPORT
    if(hRapCh->eChannelType == BRAP_ChannelType_eVideoDecode)
    {
        return  BRAP_P_StopVideoChannel(hRapCh);
    }
#endif    

    if((false == bWdgRecovery) && (false == hRapCh->bInternalCallFromRap)) 
    {
        hRapCh->uiInputSamplingRate= BRAP_INVALID_VALUE;

    }
        hRapCh->eSamplingRate  = BAVC_AudioSamplingRate_eUnknown;    
        hRapCh->bGateOpened = false;            
        hRapCh->uiModeValue = BRAP_INVALID_VALUE;   
        
        hRapCh->bStopinvoked= true;
    


    /* Stop all the paths in this channel */
    for(uiPth=0; uiPth<BRAP_P_MAX_PATHS_IN_A_CHAN; uiPth++)
    {
        if(!(BRAP_P_IsPointerValid((void *)hRapCh->pPath[uiPth])))
        {
            continue;
        }
        switch(hRapCh->pPath[uiPth]->eUsgPath)
        {
            case BRAP_P_UsgPath_eDecodePcm:
            case BRAP_P_UsgPath_eDecodeCompress:
            case BRAP_P_UsgPath_ePPBranch:
            case BRAP_P_UsgPath_eMixPath:                
            case BRAP_P_UsgPath_eSharedPP:                        
            case BRAP_P_UsgPath_eDownmixedPath:   
            case BRAP_P_UsgPath_eDownmixedMixPath:                                       
            case BRAP_P_UsgPath_eDecodePcmPostMixing:
            case BRAP_P_UsgPath_ePPBranchPostMixing:                
            case BRAP_P_UsgPath_eDecodeCompressPostMixing:                
                ret = BRAP_P_StopDecPath(hRapCh, uiPth);
                if(ret != BERR_SUCCESS) 
                {
                    BDBG_ERR(("BRAP_StopChannel: StopDecPath returned %d",ret));
                    return BERR_TRACE(ret);
                }
                break;  
#if (BRAP_7550_FAMILY != 1)                
            case BRAP_P_UsgPath_eCapture:
                ret = BRAP_P_StopCapPath(hRapCh, uiPth);
                if(ret != BERR_SUCCESS) 
                {
                    BDBG_ERR(("BRAP_StopChannel: StopCapPath returned %d",ret));
                    return BERR_TRACE(ret);
                }
                break;
#endif                
            default:
                BDBG_MSG(("BRAP_StopChannel: Path Stop# Unsupported pPath[%d].eUsgPath = %d",
                    uiPth, hRapCh->pPath[uiPth]->eUsgPath));
                /* Do nothing */
                break;
        }
    }/* uiPth */

    if(hRapCh->bInternalCallFromRap == false)
    {
        /* Close all the paths in this channel */
        for(uiPth=0; uiPth<BRAP_P_MAX_PATHS_IN_A_CHAN; uiPth++)
        {   
            if(!(BRAP_P_IsPointerValid((void *)hRapCh->pPath[uiPth])))
            {
                continue;
            }
            hRapCh->pPath[uiPth]->hFmm = hRapCh->hRap->hFmm[0];
            switch(hRapCh->pPath[uiPth]->eUsgPath)
            {
                case BRAP_P_UsgPath_eDecodePcm:
                case BRAP_P_UsgPath_eDecodeCompress:
                case BRAP_P_UsgPath_ePPBranch:                    
                case BRAP_P_UsgPath_eMixPath:                    
                case BRAP_P_UsgPath_eSharedPP:                     
                case BRAP_P_UsgPath_eDownmixedPath:      
                case BRAP_P_UsgPath_eDownmixedMixPath:                                           
                case BRAP_P_UsgPath_eDecodePcmPostMixing:
                case BRAP_P_UsgPath_ePPBranchPostMixing:                    
                case BRAP_P_UsgPath_eDecodeCompressPostMixing:
                    BDBG_MSG(("Calling BRAP_P_CloseDecPath for uiPth = %d eUsgPath = %d", 
                        uiPth, hRapCh->pPath[uiPth]->eUsgPath));
                    ret = BRAP_P_CloseDecPath(hRapCh, uiPth);
                    if(ret != BERR_SUCCESS) 
                    {
                        BDBG_ERR(("BRAP_StopChannel: CloseDecPath returned %d",ret));
                        return BERR_TRACE(ret);
                    }
                    break;
#if (BRAP_7550_FAMILY != 1)                    
                case BRAP_P_UsgPath_eCapture:
                    BDBG_MSG(("Calling BRAP_P_CloseCapPath for uiPth = %d eUsgPath = %d", 
                        uiPth, hRapCh->pPath[uiPth]->eUsgPath));
                    ret = BRAP_P_CloseCapPath(hRapCh, uiPth);
                    if(ret != BERR_SUCCESS) 
                    {
                        BDBG_ERR(("BRAP_StopChannel: CloseCapPath returned %d",ret));
                        return BERR_TRACE(ret);
                    }
                    break;
#endif                    
                default:
                    BDBG_MSG(("BRAP_StopChannel: Path Close# Unsupported pPath[%d].eUsgPath = %d",
                        uiPth, hRapCh->pPath[uiPth]->eUsgPath));
                    /* Do nothing */
                    break;
            } 
        }/* uiPth */
    }/* if bInternalCallFromRap == false */

    if(false == hRapCh->bInternalCallFromRap)
    {    
        ret = BRAP_P_UngetChannelPaths(hRapCh);
        if(ret != BERR_SUCCESS) 
        {
            BDBG_ERR(("BRAP_StopChannel: UngetChannelPaths returned %d",ret));
            return BERR_TRACE(ret);
        }
    }

/* Destroy internal stage added to the network */
    ret = BRAP_FWIF_P_DestroyInternalStages(hRapCh);
    if(ret != BERR_SUCCESS) 
    {
        BDBG_ERR(("BRAP_StopChannel: Destroying internal stage function returned %d",ret));
        return BERR_TRACE(ret);
    }    

    if(hRapCh->bInternalCallFromRap == false)
    {
        /*    Reset trick mode states */
        hRapCh->sTrickModeState.bAudioPaused = false;
        hRapCh->sTrickModeState.uiFrameAdvResidualTime = 0;
        /* Reset Audio Descriptor handle */
        hRapCh->hAdFade = NULL;
     }

    
#if (BRAP_7405_FAMILY == 1)
    if((hRapCh->eChannelType == BRAP_ChannelType_eDecode))
    {
        if(BRAP_P_IsPointerValid((void *)hRapCh->hTimer))
        {
            BTMR_DestroyTimer(hRapCh->hTimer);
            hRapCh->hTimer = NULL;
        }
    }
#endif

#ifdef RAP_MULTISTREAM_DECODER_SUPPORT
    hRapCh->uiFWMixerIpIndex = BRAP_INVALID_VALUE;
#endif

    /* Update the channel state */
    if(BERR_SUCCESS == ret)
    {
        hRapCh->eState = BRAP_P_State_eOpened;
        hRapCh->bStopinvoked= false;        
    }
    
    if(BRAP_P_IsFwMixingPostLoopbackEnabled(hRapCh))
    {
        for(i =0 ; i < BRAP_MAX_PP_SUPPORTED; i++)
        {
            if((hRapCh->hRap->hAudioProcessingStageHandle[i] != NULL) &&
               (hRapCh->hRap->hAudioProcessingStageHandle[i]->sProcessingStageSettings.eAudioProcessing == BRAP_ProcessingType_eFwMixer))
            {                                   
                psProcessingStageSettings=(BRAP_ProcessingStageSettings *)BKNI_Malloc(sizeof(BRAP_ProcessingStageSettings));
                if( NULL==psProcessingStageSettings)
                {
                    ret = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                    goto end;        
                }                        
                ret = BRAP_GetCurrentProcessingStageSettings(hRapCh->hRap->hAudioProcessingStageHandle[i], psProcessingStageSettings);
                if(ret != BERR_SUCCESS)
                {
                    BDBG_ERR(("BRAP_StartChannel: BRAP_GetDefaultProcessingStageSettings() returned err(%d)",ret));	
                    goto end;
                }                    
                ret = BRAP_SetProcessingStageSettings(hRapCh->hRap->hAudioProcessingStageHandle[i],psProcessingStageSettings);
                if(ret != BERR_SUCCESS)
                {
                    BDBG_ERR(("BRAP_StartChannel: BRAP_SetProcessingStageSettings() returned err(%d)",ret));	
                    goto end;                        
                }                       
                break;
            }
        }
    }

end:
    if(psProcessingStageSettings)
        BKNI_Free(psProcessingStageSettings);
                
    if(BERR_SUCCESS == ret)
    {    
#ifdef BCHP_PWR_RESOURCE_RAP_START
        /* a successful StopChannel releases the resource */
        BCHP_PWR_ReleaseResource(hRapCh->hChip, BCHP_PWR_RESOURCE_RAP_START);
#endif
    }

	BDBG_LEAVE(BRAP_StopChannel);
	return ret;
}

/***************************************************************************
Summary:
    Sets the output volume for left and right channels.

Description:
    This function sets the output left and right channel volume. In hardware,
    the output volume control is done at the mixer feeding data to an output 
    port. The mixer output volume control is linear in hardware, ranging from 
    0 min to 8000 max volume. This API has done a mapping from this linear range
    to 1/100 DB. This function gets values in 1/100 DB from 0 (max) to 9000 
    1/100 DB (min), and for all values above 9000 the volume will be set to 0 
    linear or 90 DB. 

    Note: This API can be used only for an output port carrying uncompressed 
          data.
    
Returns:
	BERR_SUCCESS if successful else error value.            
	
See Also:
    BRAP_GetOutputVolume().
**************************************************************************/
BERR_Code BRAP_SetOutputVolume ( 
    BRAP_Handle     hRap,       /* [in] Audio Device Handle */
    BRAP_OutputPort eOpType,    /* [in] Output Port */
    unsigned int    uiLVolume,  /* [in] Left channel volume in 1/100 DB */
    unsigned int    uiRVolume   /* [in] Right channel volume in 1/100 DB*/
)
{
    BERR_Code               ret = BERR_SUCCESS;
    BRAP_MIXER_P_Handle     hMixer = NULL;
    unsigned int            uiMixerOutput = 0;
    bool bWdgRecovery = false;
    uint32_t        ui32AttenDb = 0, ui32Delta=0, ui32Temp=0;



	BDBG_ENTER(BRAP_SetOutputVolume);
	BDBG_ASSERT(hRap);

       bWdgRecovery = BRAP_P_GetWatchdogRecoveryFlag(hRap);    

	BDBG_MSG(("BRAP_SetOutputVolume: eOpType = %d,uiLVolume =%d,uiRVolume =%d",
		eOpType,uiLVolume,uiRVolume));

    if( eOpType == BRAP_OutputPort_eDac0)
    {
        ret = BRAP_P_IsVolumeControlSupported(hRap,eOpType,BRAP_ProcessingType_eBtsc);
        if(ret != BERR_SUCCESS)
        {     
			BDBG_ERR(("BRAP_SetOutputVolume:BRAP_P_IsVolumeControlSupported returned "
				  "Error"));                
			return ret;                
        }        
    }
    
        if(false == bWdgRecovery)
        {
        	/* Store the volume parameters */	
        	hRap->uiOutputLVolume[eOpType] = uiLVolume;
        	hRap->uiOutputRVolume[eOpType] = uiRVolume;

    /* Calculate  value for  Channel volume in 5.23*/ 
	/* Since we take the volume level from application in multiple of 100 i.e. if application
       wants set volume level tp 80, it needs to provide 8000 as volume. This is done to provide
       more granularity in volume control. Now if application passes 8550 DB as volume. But we 
       do not have hex value corresponding to 8550/100 = 85.50, so we calculte it by finding the
       fraction part and calculating the hex value accordingly. SO the calculation for this ex is 
                 uiLvolume   = 8550
                 ui32AttenDb = 8550/100 = 85 (So we loose .50 here)
                 ui32temp    = BRAP_VOL_DB[85]
                 ui32Delta   = ui32temp - BRAP_VOL_DB[86]
                 ui32Delta   = ui32Delta * ((uiLVolume%100)/100)
                             = ui32Delta * ((8550%100)/100)
                             = 0.5 * ui32Delta
                 uiTemp     -= ui32delta (So we accomodate the extra .5DB entered by Applicaion)*/ 
                 
            ui32AttenDb = uiLVolume/BRAP_MIXER_P_FRACTION_IN_DB;    
            if (ui32AttenDb < BRAP_MIXER_P_MAX_DB_VOLUME)
            {

                ui32Temp = BRAP_MIXER_P_ConvertVolInDbTo5_23Format(ui32AttenDb);
                ui32Delta = ui32Temp - BRAP_MIXER_P_ConvertVolInDbTo5_23Format(ui32AttenDb+1);
                ui32Delta = (ui32Delta * (uiLVolume%BRAP_MIXER_P_FRACTION_IN_DB))
                                /(BRAP_MIXER_P_FRACTION_IN_DB);
                ui32Temp -= ui32Delta;
            }       
            else
            {
                ui32Temp = 0;
            }                               
            
            hRap->uiOutputLVolumeGain[eOpType] = ui32Temp;


            ui32AttenDb = uiRVolume/BRAP_MIXER_P_FRACTION_IN_DB;    
            if (ui32AttenDb < BRAP_MIXER_P_MAX_DB_VOLUME)
            {

                ui32Temp = BRAP_MIXER_P_ConvertVolInDbTo5_23Format(ui32AttenDb);
                ui32Delta = ui32Temp - BRAP_MIXER_P_ConvertVolInDbTo5_23Format(ui32AttenDb+1);
                ui32Delta = (ui32Delta * (uiRVolume%BRAP_MIXER_P_FRACTION_IN_DB))
                                /(BRAP_MIXER_P_FRACTION_IN_DB);
                ui32Temp -= ui32Delta;
            }       
            else
            {
                ui32Temp = 0;
            }    
            
            hRap->uiOutputRVolumeGain[eOpType] = ui32Temp;

           BDBG_MSG(("hRap->uiOutputLVolume[i] =%d",hRap->uiOutputLVolume[eOpType]));
           BDBG_MSG(("hRap->uiOutputLVolumeGain[i] =%d",hRap->uiOutputLVolumeGain[eOpType]));            
            
        }

	if(BRAP_P_IsPointerValid((void *)hRap->hFmm[0]->hOp[eOpType]))
	{
#if (BRAP_7405_FAMILY == 1)
        if((BRAP_OutputPort_eMai == eOpType)&&
            (BRAP_OutputPort_eMai != hRap->hFmm[0]->hOp[eOpType]->uOpSettings.sMai.sExtSettings.eMaiMuxSelector))
        {
                if((hRap->hFmm[0]->hOp[eOpType]->uOpSettings.sMai.sExtSettings.eMaiMuxSelector == BRAP_OutputPort_eI2s0))
                {
                   BRAP_SetOutputVolume(hRap,BRAP_OutputPort_eI2s0,uiLVolume,uiRVolume); 
                   BRAP_SetOutputVolume(hRap,BRAP_OutputPort_eI2s1,uiLVolume,uiRVolume); 
                   BRAP_SetOutputVolume(hRap,BRAP_OutputPort_eI2s2,uiLVolume,uiRVolume);                    
                }
                else  if((hRap->hFmm[0]->hOp[eOpType]->uOpSettings.sMai.sExtSettings.eMaiMuxSelector == BRAP_OutputPort_eMaiMulti0))
                {
                   BRAP_SetOutputVolume(hRap,BRAP_OutputPort_eMaiMulti0,uiLVolume,uiRVolume); 
                   BRAP_SetOutputVolume(hRap,BRAP_OutputPort_eMaiMulti1,uiLVolume,uiRVolume); 
                   BRAP_SetOutputVolume(hRap,BRAP_OutputPort_eMaiMulti2,uiLVolume,uiRVolume);                    
                }
                BDBG_LEAVE(BRAP_SetOutputVolume);
        	return ret;
        }
#endif        
		/* Get the Mixer handle for mixer linked to this output port */
		ret = BRAP_P_GetMixerForOpPort(hRap,eOpType,&hMixer,&uiMixerOutput);
		if(BERR_INVALID_PARAMETER == ret)
		{
			/* Channel not started yet. */
			ret = BERR_SUCCESS;
		}
		else if((BERR_SUCCESS == ret) && (!(BRAP_P_IsPointerValid((void *)hMixer))))
		{
			BDBG_MSG(("BRAP_SetOutputVolume: eOpType = %d is not linked with any mixer."
				"Its linked with a SrcCh",eOpType));
			return ret;
		}
		else if(BERR_SUCCESS == ret)
		{
#if ( (BRAP_7405_FAMILY == 1) || (BRAP_3548_FAMILY == 1) )
            /* This is a temporary patch for 7405 watchdog recovery of Dual-Cxt mode. Issue is that
            during normal operation, while doing OP_P_Start, we call BRAP_MIXER_P_SetOutputVolume 
            & as by that time Mixer hasn't started, mixer params like hMixer->bCompress aren't 
            updated yet. So, we don't face any errors. But during watchdog recovery, as the mixer
            was started earlier, bCompress is set properly & we face an error. We should put a logical
            check at BRAP_SetOutputVolume level so that volume setting API for any compressed output
            port is returned with an error. */
            if(false == hMixer->bCompress)
            {
    			ret = BRAP_MIXER_P_SetOutputVolume(hMixer,uiMixerOutput,uiLVolume,uiRVolume);
    			if(BERR_SUCCESS != ret)
    			{
    				BDBG_ERR(("BRAP_SetOutputVolume:BRAP_MIXER_P_SetOutputVolume returned"
    					"Error"));
    				return ret;
    			}
            }
#else
			/* Call the Mixer volume control API */
			ret = BRAP_MIXER_P_SetOutputVolume(hMixer,uiMixerOutput,uiLVolume,uiRVolume);
			if(BERR_SUCCESS != ret)
			{
				BDBG_ERR(("BRAP_SetOutputVolume:BRAP_MIXER_P_SetOutputVolume returned"
					"Error"));
				return ret;
			}
#endif            
		}
		else
		{
			BDBG_ERR(("BRAP_SetOutputVolume:BRAP_P_GetMixerForOpPort returned"
				"Error"));
			return ret;
		}
	}
    
	BDBG_LEAVE(BRAP_SetOutputVolume);
	return ret;
}

/***************************************************************************
Summary:
    Gets the output volume for left and right channels

Description:
    This function returns left and right volume channel settings for an output.
    Currently, volume control is done at the mixer feeding data to an output
    port, so the associated mixer's output volume level is returned by this 
    API. Values are returned in 1/100 DB. 

    Note: This API can be used only for an output port carrying uncompressed 
          data.

Returns:
   	BERR_SUCCESS if successful else error value.            

See Also:
    BRAP_SetOutputVolume().
**************************************************************************/
BERR_Code BRAP_GetOutputVolume( 
    BRAP_Handle     hRap,       /* [in] Audio Device Handle */
    BRAP_OutputPort eOpType,    /* [in] Output Type */
    unsigned int *  pLVolume,   /* [out] Pointer to memory where left channel 
                                   volume should be stored. Units: 1/100 DB */
    unsigned int *  pRVolume    /* [out] Pointer to memory where right channel 
                                   volume should be stored. Units: 1/100 DB */
)
{
    BERR_Code               ret = BERR_SUCCESS;

    BDBG_ENTER(BRAP_GetOutputVolume);
    BDBG_ASSERT(hRap);

    BDBG_MSG(("BRAP_GetOutputVolume: eOpType = %d , hRap->uiOutputLVolume[eOpType] =%d",eOpType,hRap->uiOutputLVolume[eOpType]));

    *pLVolume = hRap->uiOutputLVolume[eOpType];
    *pRVolume = hRap->uiOutputRVolume[eOpType];

    BDBG_LEAVE(BRAP_GetOutputVolume);
    return ret;
}

/***************************************************************************
Summary:
	Sets the Destination volume level for left and right channels.

Description:
    This API has the volume control on the Destination.
    The default value written will be 0x800000
    
Note: This API can be used  for both output port and ring buffers carrying PCM data.
    
Returns:
	BERR_SUCCESS if successful else error value.            
	
See Also:
    BRAP_GetDestinationVolume()

**************************************************************************/

BERR_Code BRAP_SetDestinationVolume ( 
    BRAP_DestinationHandle      hDstHandle, /* [in] Destination Handle */
    unsigned int    			uiLVolume,  /* [in] Left channel volume in 0.23 format */
    unsigned int    			uiRVolume   /* [in] Right channel volume in 0.23 format */
    )

{
    BERR_Code               ret = BERR_SUCCESS;
	BRAP_MIXER_P_Handle     hMixer = NULL;
   	unsigned int            uiMixerOutput=0, uiMixerlevel=0,i=0;
    BRAP_OutputPort         eOpType = BRAP_OutputPort_eMax;
	
	BDBG_ENTER(BRAP_SetDestinationVolume);

	BDBG_ASSERT(hDstHandle);	

	BDBG_MSG(("BRAP_SetDestinationVolume: hDstHandle =%x,uiLVolume =%x,uiRVolume =%x",
               hDstHandle,uiLVolume,uiRVolume));

    if (hDstHandle->sExtDstDetails.eAudioDst == BRAP_AudioDst_eMax)
    {
        BDBG_ERR (("BRAP_SetDestinationInputScaling(): No proper Audio destination in destination handle %x",hDstHandle));
	    return BERR_TRACE (BERR_NOT_SUPPORTED);    
    }
    
	switch (hDstHandle->sExtDstDetails.eAudioDst)
	{
		case BRAP_AudioDst_eOutputPort:
            eOpType = hDstHandle->sExtDstDetails.uDstDetails.sOpDetails.eOutput[BRAP_OutputChannelPair_eLR];
            
            if(eOpType == BRAP_OutputPort_eDac0)
            {
                ret = BRAP_P_IsVolumeControlSupported(hDstHandle->hAssociation->hRap,eOpType,BRAP_ProcessingType_eBtsc);
                if(ret != BERR_SUCCESS)
                {     
					BDBG_ERR(("BRAP_SetDestinationVolume:BRAP_P_IsVolumeControlSupported returned "
						  "Error"));                
    				return ret;                
                }
            }            
            
        	/* Store the volume parameters */	
            for(i =0; i<BRAP_OutputChannelPair_eMax;i++)
            {
                if(hDstHandle->sExtDstDetails.uDstDetails.sOpDetails.eOutput[i] != BRAP_OutputPort_eMax)
                {
                    hDstHandle->hAssociation->hRap->uiOutputLVolumeGain[hDstHandle->sExtDstDetails.uDstDetails.sOpDetails.eOutput[i]] = uiLVolume;
                    hDstHandle->hAssociation->hRap->uiOutputRVolumeGain[hDstHandle->sExtDstDetails.uDstDetails.sOpDetails.eOutput[i]] = uiRVolume;            
                    /* Store in 1/100 Db Format*/
                    hDstHandle->hAssociation->hRap->uiOutputLVolume[hDstHandle->sExtDstDetails.uDstDetails.sOpDetails.eOutput[i]] = BRAP_MIXER_P_ConvertVolIn5_23ToDbFormat(uiLVolume);
                    hDstHandle->hAssociation->hRap->uiOutputRVolume[hDstHandle->sExtDstDetails.uDstDetails.sOpDetails.eOutput[i]] = BRAP_MIXER_P_ConvertVolIn5_23ToDbFormat(uiRVolume);
                    BDBG_MSG(("BRAP_SetDestinationVolume: hDstHandle->hAssociation->hRap->uiOutputLVolume[i] =%d",hDstHandle->hAssociation->hRap->uiOutputLVolume[hDstHandle->sExtDstDetails.uDstDetails.sOpDetails.eOutput[i]]));
                    BDBG_MSG(("BRAP_SetDestinationVolume :hDstHandle->hAssociation->hRap->uiOutputLVolumeGain[i] =%d",hDstHandle->hAssociation->hRap->uiOutputLVolumeGain[hDstHandle->sExtDstDetails.uDstDetails.sOpDetails.eOutput[i]]));
                    BRAP_P_SetOutputVolumeGain(hDstHandle->hAssociation->hRap,hDstHandle->sExtDstDetails.uDstDetails.sOpDetails.eOutput[i],uiLVolume,uiRVolume);                     
                }
            }

            if((eOpType == BRAP_OutputPort_eMai))
            {
                if((hDstHandle->hAssociation->hRap->sOutputSettings[BRAP_OutputPort_eMai].uOutputPortSettings.sMaiSettings.eMaiMuxSelector == BRAP_OutputPort_eI2s0))
                {
                   BRAP_P_SetOutputVolumeGain(hDstHandle->hAssociation->hRap,BRAP_OutputPort_eI2s0,uiLVolume,uiRVolume); 
                   BRAP_P_SetOutputVolumeGain(hDstHandle->hAssociation->hRap,BRAP_OutputPort_eI2s1,uiLVolume,uiRVolume); 
                   BRAP_P_SetOutputVolumeGain(hDstHandle->hAssociation->hRap,BRAP_OutputPort_eI2s2,uiLVolume,uiRVolume); 
                   
                   hDstHandle->hAssociation->hRap->uiOutputLVolumeGain[BRAP_OutputPort_eI2s0] = uiLVolume;
                   hDstHandle->hAssociation->hRap->uiOutputRVolumeGain[BRAP_OutputPort_eI2s0] = uiRVolume;            
                   /* Store in 1/100 Db Format*/
                   hDstHandle->hAssociation->hRap->uiOutputLVolume[BRAP_OutputPort_eI2s0] = BRAP_MIXER_P_ConvertVolIn5_23ToDbFormat(uiLVolume);
                   hDstHandle->hAssociation->hRap->uiOutputRVolume[BRAP_OutputPort_eI2s0] = BRAP_MIXER_P_ConvertVolIn5_23ToDbFormat(uiRVolume);

                   hDstHandle->hAssociation->hRap->uiOutputLVolumeGain[BRAP_OutputPort_eI2s1] = uiLVolume;
                   hDstHandle->hAssociation->hRap->uiOutputRVolumeGain[BRAP_OutputPort_eI2s1] = uiRVolume;            
                   /* Store in 1/100 Db Format*/
                   hDstHandle->hAssociation->hRap->uiOutputLVolume[BRAP_OutputPort_eI2s1] = BRAP_MIXER_P_ConvertVolIn5_23ToDbFormat(uiLVolume);
                   hDstHandle->hAssociation->hRap->uiOutputRVolume[BRAP_OutputPort_eI2s1] = BRAP_MIXER_P_ConvertVolIn5_23ToDbFormat(uiRVolume);

                   hDstHandle->hAssociation->hRap->uiOutputLVolumeGain[BRAP_OutputPort_eI2s2] = uiLVolume;
                   hDstHandle->hAssociation->hRap->uiOutputRVolumeGain[BRAP_OutputPort_eI2s2] = uiRVolume;            
                   /* Store in 1/100 Db Format*/
                   hDstHandle->hAssociation->hRap->uiOutputLVolume[BRAP_OutputPort_eI2s2] = BRAP_MIXER_P_ConvertVolIn5_23ToDbFormat(uiLVolume);
                   hDstHandle->hAssociation->hRap->uiOutputRVolume[BRAP_OutputPort_eI2s2] = BRAP_MIXER_P_ConvertVolIn5_23ToDbFormat(uiRVolume);                   

                   
                }
                else  if((hDstHandle->hAssociation->hRap->sOutputSettings[BRAP_OutputPort_eMai].uOutputPortSettings.sMaiSettings.eMaiMuxSelector == BRAP_OutputPort_eMaiMulti0))
                {
                   BRAP_P_SetOutputVolumeGain(hDstHandle->hAssociation->hRap,BRAP_OutputPort_eMaiMulti0,uiLVolume,uiRVolume); 
                   BRAP_P_SetOutputVolumeGain(hDstHandle->hAssociation->hRap,BRAP_OutputPort_eMaiMulti1,uiLVolume,uiRVolume); 
                   BRAP_P_SetOutputVolumeGain(hDstHandle->hAssociation->hRap,BRAP_OutputPort_eMaiMulti2,uiLVolume,uiRVolume);       

                   hDstHandle->hAssociation->hRap->uiOutputLVolumeGain[BRAP_OutputPort_eMaiMulti0] = uiLVolume;
                   hDstHandle->hAssociation->hRap->uiOutputRVolumeGain[BRAP_OutputPort_eMaiMulti0] = uiRVolume;            
                   /* Store in 1/100 Db Format*/
                   hDstHandle->hAssociation->hRap->uiOutputLVolume[BRAP_OutputPort_eMaiMulti0] = BRAP_MIXER_P_ConvertVolIn5_23ToDbFormat(uiLVolume);
                   hDstHandle->hAssociation->hRap->uiOutputRVolume[BRAP_OutputPort_eMaiMulti0] = BRAP_MIXER_P_ConvertVolIn5_23ToDbFormat(uiRVolume);       

                   hDstHandle->hAssociation->hRap->uiOutputLVolumeGain[BRAP_OutputPort_eMaiMulti1] = uiLVolume;
                   hDstHandle->hAssociation->hRap->uiOutputRVolumeGain[BRAP_OutputPort_eMaiMulti1] = uiRVolume;            
                   /* Store in 1/100 Db Format*/
                   hDstHandle->hAssociation->hRap->uiOutputLVolume[BRAP_OutputPort_eMaiMulti1] = BRAP_MIXER_P_ConvertVolIn5_23ToDbFormat(uiLVolume);
                   hDstHandle->hAssociation->hRap->uiOutputRVolume[BRAP_OutputPort_eMaiMulti1] = BRAP_MIXER_P_ConvertVolIn5_23ToDbFormat(uiRVolume);  

                   hDstHandle->hAssociation->hRap->uiOutputLVolumeGain[BRAP_OutputPort_eMaiMulti2] = uiLVolume;
                   hDstHandle->hAssociation->hRap->uiOutputRVolumeGain[BRAP_OutputPort_eMaiMulti2] = uiRVolume;            
                   /* Store in 1/100 Db Format*/
                   hDstHandle->hAssociation->hRap->uiOutputLVolume[BRAP_OutputPort_eMaiMulti2] = BRAP_MIXER_P_ConvertVolIn5_23ToDbFormat(uiLVolume);
                   hDstHandle->hAssociation->hRap->uiOutputRVolume[BRAP_OutputPort_eMaiMulti2] = BRAP_MIXER_P_ConvertVolIn5_23ToDbFormat(uiRVolume);                     
                }
            }

#if 0
    		/* Get the Mixer handle for mixer linked to this output port */
    		ret = BRAP_P_GetMixerForOpPort(hDstHandle->hAssociation->hRap,eOpType,&hMixer,&uiMixerOutput);
            
    		if(BERR_INVALID_PARAMETER == ret)
    		{
    			/* Channel not started yet */
    			ret = BERR_SUCCESS;
    		}
    		else if(BERR_SUCCESS == ret)
    		{
    			/* Call the Mixer volume control API */
    			ret = BRAP_MIXER_P_SetOutputVolumeGain(hMixer,uiMixerOutput,uiLVolume,uiRVolume);
    			if(BERR_SUCCESS != ret)
    			{
    				BDBG_ERR(("BRAP_SetDestinationVolume:BRAP_MIXER_P_SetOutputVolumeGain returned"
    					"Error"));
    				return ret;
    			}
    		}
    		else
    		{
    			BDBG_ERR(("BRAP_SetDestinationVolume:BRAP_P_GetMixerForOpPort returned"
    				"Error"));
    			return BERR_TRACE(ret);
    		}
#endif
		    break;

		case BRAP_AudioDst_eRingBuffer:
			/* Store the volume details */
			hDstHandle->uiLtVolume[BRAP_OutputChannelPair_eLR][uiMixerlevel] = uiLVolume;
			hDstHandle->uiRtVolume[BRAP_OutputChannelPair_eLR][uiMixerlevel] = uiRVolume;

			/* Get the corresponding mixer for the ring buffer */
			ret = BRAP_P_GetMixerForRBuf(hDstHandle->hAssociation->hRap, hDstHandle->sExtDstDetails.uDstDetails.sRBufDetails.uiRBufId[BRAP_OutputChannelPair_eLR], &hMixer, &uiMixerOutput);
				
			if(BERR_INVALID_PARAMETER == ret)
			{
				/* Channel not started yet. */
				ret = BERR_SUCCESS;
			}
			else if(BERR_SUCCESS == ret)
			{
				/* Call the Mixer volume control API */
				ret = BRAP_MIXER_P_SetOutputVolumeGain(hMixer,uiMixerOutput,uiLVolume,uiRVolume);
				if(BERR_SUCCESS != ret)
				{
					BDBG_ERR(("BRAP_SetOutputVolume:BRAP_MIXER_P_SetOutputVolume returned"
						  "Error"));
					return ret;
				}
			}
			else
			{
				BDBG_ERR(("BRAP_SetDestinationVolume:BRAP_P_GetMixerForRBuf returned"
						   "Error"));
				return BERR_TRACE(ret);
			}
			break;

		default: 	
			return BERR_TRACE( BERR_INVALID_PARAMETER );
	}

	BDBG_LEAVE(BRAP_SetDestinationVolume);
	return ret;
	
}


/***************************************************************************
Summary:
	Gets the Destination volume level for left and right channels.

Description:
    This API gets the present Destination volume level. 
    
Note: This API can be used  for both output port and ring buffers carrying PCM data.
    
Returns:
	BERR_SUCCESS if successful else error value.            
	
See Also:
    BRAP_SetDestinationVolumeGain()

**************************************************************************/
BERR_Code BRAP_GetDestinationVolume ( 
    BRAP_DestinationHandle	hDstHandle, /* [in] Destination Handle */
    unsigned int *  		pLVolume,  	/* [out] Left channel volume in 0.23 format */
    unsigned int *  		pRVolume    /* [out] Right channel volume in 0.23 format */
    )
{
    BERR_Code               ret = BERR_SUCCESS;
  	unsigned int            uiMixerlevel=0;
	
	BDBG_ENTER(BRAP_GetDestinationVolume);

	BDBG_ASSERT(hDstHandle);	

    BDBG_MSG(("BRAP_GetDestinationVolume: hDstHandle = %x",hDstHandle));	

    if (hDstHandle->sExtDstDetails.eAudioDst == BRAP_AudioDst_eMax)
    {
        BDBG_ERR (("BRAP_SetDestinationInputScaling(): No proper Audio destination in destination handle %x",hDstHandle));
	    return BERR_TRACE (BERR_NOT_SUPPORTED);    
    }
	switch (hDstHandle->sExtDstDetails.eAudioDst)
	{
		case BRAP_AudioDst_eOutputPort:
            	*pLVolume = hDstHandle->hAssociation->hRap->uiOutputLVolumeGain[hDstHandle->sExtDstDetails.uDstDetails.sOpDetails.eOutput[BRAP_OutputChannelPair_eLR]];
            	*pRVolume = hDstHandle->hAssociation->hRap->uiOutputRVolumeGain[hDstHandle->sExtDstDetails.uDstDetails.sOpDetails.eOutput[BRAP_OutputChannelPair_eLR]];
                BDBG_ERR(("BRAP_GetDestinationVolume pLVolume =%d",*pLVolume));
                break;
		case BRAP_AudioDst_eRingBuffer:                
            	*pLVolume = hDstHandle->uiLtVolume[BRAP_OutputChannelPair_eLR][uiMixerlevel];
            	*pRVolume = hDstHandle->uiRtVolume[BRAP_OutputChannelPair_eLR][uiMixerlevel];
                break;
		default: 	
			return BERR_TRACE( BERR_INVALID_PARAMETER );                
	    }
          

	BDBG_LEAVE(BRAP_GetDestinationVolume);	

	return ret;
}

/***************************************************************************
Summary:
    Mute/unmute an output port for uncompressed data

Description:
    This function mutes or unmutes output an output port. The mute control is 
    available at the mixer feeding data to an output port, so the corresponding
    mixer is muted/unmuted in this API.
    
    Note: 
    1. This API can be used only for an output port carrying uncompressed data.
    2. These mutes/umutes also ramp up/down the volume in hardware, so there is 
    no need to bring the volume down when using these mute controls.  

Returns:
	BERR_SUCCESS if successful else error value.            
	
See Also:
    BRAP_GetOutputMute().
**************************************************************************/
BERR_Code BRAP_SetDestinationMute ( 
    BRAP_DestinationHandle	hDstHandle, /* [in] Destination Handle */
    bool                    bMute       /* [in] TRUE: Mute output
                                                FALSE: UnMute output */
)
{
    BERR_Code   ret = BERR_SUCCESS;

	BRAP_MIXER_P_Handle     hMixer = NULL;
   	unsigned int            i=0,uiMixerOutput = 0;
    bool bWdgRecovery = false;    
    BDBG_ENTER(BRAP_SetDestinationMute);
    
    BDBG_ASSERT(hDstHandle);    

    bWdgRecovery = BRAP_P_GetWatchdogRecoveryFlag(hDstHandle->hAssociation->hRap);


    if(hDstHandle->sExtDstDetails.eAudioDst == BRAP_AudioDst_eOutputPort)
    {
        BDBG_MSG(("BRAP_SetDestinationMute: hDstHandle = %d , bMute = %d",hDstHandle->sExtDstDetails.uDstDetails.sOpDetails.eOutput[0],bMute));	
    }
    else
    {
        BDBG_MSG(("BRAP_SetDestinationMute: hDstHandle = %d , bMute = %d",hDstHandle,bMute));	
    }
    
    if (hDstHandle->sExtDstDetails.eAudioDst == BRAP_AudioDst_eMax)
    {
        BDBG_ERR (("BRAP_SetDestinationInputScaling(): No proper Audio destination in destination handle %x",hDstHandle));
	    return BERR_TRACE (BERR_NOT_SUPPORTED);    
    }
    
	switch (hDstHandle->sExtDstDetails.eAudioDst)
	{
		case BRAP_AudioDst_eOutputPort:
            for(i=0; i<BRAP_OutputChannelPair_eMax; i++) 
            {              
                if(hDstHandle->sExtDstDetails.uDstDetails.sOpDetails.eOutput[i] != BRAP_OutputPort_eMax)
                {
                        { /*Muting Both at mixer + O/P Level. When Mute=true, First mute mixer then O/P to 
                        get the effect of ramping. When Mute=false, do it in reverse way*/                           
                            if(bMute == false)     
                            {
                                ret = BRAP_OP_P_SetMute (
                                        hDstHandle->hAssociation->hRap, 
                                        hDstHandle->sExtDstDetails.uDstDetails.sOpDetails.eOutput[i],
                                        bMute
                                        );
                                if(BERR_SUCCESS != ret)
                                {
                                    BDBG_ERR(("BRAP_SetDestinationMute: Could not Mute the Output"));
                                    return BERR_TRACE(ret);                                  
                                }    
                            }
                            /* Get the corresponding mixer for the ring buffer */
                            ret = BRAP_P_GetMixerForOpPort(hDstHandle->hAssociation->hRap, hDstHandle->sExtDstDetails.uDstDetails.sOpDetails.eOutput[i], &hMixer, &uiMixerOutput);				
                            if(BERR_SUCCESS == ret)
                            {
                                /* Call the internal private function */
                                ret = BRAP_MIXER_P_SetMute(hMixer,bMute,uiMixerOutput);
                                if(BERR_SUCCESS != ret)
                                {
                                    BDBG_ERR(("BRAP_SetDestinationMute: Could not Mute the Output"));
                                }
                            }
                            else if(BERR_INVALID_PARAMETER == ret)
                            {
                                /* Channel not started yet. */
                                ret = BERR_SUCCESS;
                            }                    
                            else
                            {
                                BDBG_ERR(("BRAP_SetDestinationMute:BRAP_P_GetMixerForRBuf returned"
                                "Error"));
                                return BERR_TRACE(ret);
                            }
                            if(bMute == true)     
                            {                            
                                ret = BRAP_OP_P_SetMute (
                                        hDstHandle->hAssociation->hRap, 
                                        hDstHandle->sExtDstDetails.uDstDetails.sOpDetails.eOutput[i],
                                        bMute
                                        );
                                if(BERR_SUCCESS != ret)
                                {
                                    BDBG_ERR(("BRAP_SetDestinationMute: Could not Mute the Output"));
                                    return BERR_TRACE(ret);                                  
                                }                                
                            }
                        }
                        if(false == bWdgRecovery)
                        {
                            hDstHandle->hAssociation->hRap->bOutputMute[hDstHandle->sExtDstDetails.uDstDetails.sOpDetails.eOutput[i]] = bMute;
                    hDstHandle->bMute[i] = bMute;                
                        }                          
                    }
                }
		break;
            case BRAP_AudioDst_eRingBuffer:
                /* Get the corresponding mixer for the ring buffer */
                ret = BRAP_P_GetMixerForRBuf(hDstHandle->hAssociation->hRap, hDstHandle->sExtDstDetails.uDstDetails.sRBufDetails.uiRBufId[BRAP_OutputChannelPair_eLR], &hMixer, &uiMixerOutput);

                if(BERR_INVALID_PARAMETER == ret)
                {
                    /* Channel not started yet. */
                    ret = BERR_SUCCESS;
                }
                else if(BERR_SUCCESS == ret)
                {
                    /* Call the internal private function */
                    ret = BRAP_MIXER_P_SetMute(hMixer,bMute,uiMixerOutput);
                    if(BERR_SUCCESS != ret)
                    {
                        BDBG_ERR(("BRAP_SetDestinationMute: Could not Mute the Output"));
                    }
                }
                else
                {
                    BDBG_ERR(("BRAP_SetDestinationMute:BRAP_P_GetMixerForRBuf returned"
                    "Error"));
                    return BERR_TRACE(ret);
                }
                if(false == bWdgRecovery)
                {
            hDstHandle->bMute[BRAP_OutputChannelPair_eLR] = bMute;
                }            
                break;
		default: 	
			return BERR_TRACE( BERR_INVALID_PARAMETER );
	}
    

    BDBG_LEAVE(BRAP_SetDestinationMute);
    return ret;
}

/***************************************************************************
Summary:
    Gets whether an output is muted or not.

Description:
    It can be used only for an output port carrying uncompressed data.

Returns:
	BERR_SUCCESS if successful else error value.            
	
See Also:
    BRAP_SetOutputMute().
    
**************************************************************************/
BERR_Code BRAP_GetDestinationMute ( 
    BRAP_DestinationHandle	hDstHandle, /* [in] Destination Handle */
    bool                    *pMute      /* [out] Pointer to memory where the Mute 
                                                    status is to be written */            
)
{
    BERR_Code           ret = BERR_SUCCESS;

    BDBG_ENTER(BRAP_GetDestinationMute);

	BDBG_ASSERT(hDstHandle);	

    BDBG_MSG(("BRAP_GetDestinationMute: hDstHandle = %x",hDstHandle));	

    if (hDstHandle->sExtDstDetails.eAudioDst == BRAP_AudioDst_eMax)
    {
        BDBG_ERR (("BRAP_SetDestinationInputScaling(): No proper Audio destination in destination handle %x",hDstHandle));
	    return BERR_TRACE (BERR_NOT_SUPPORTED);    
    }
    
	switch (hDstHandle->sExtDstDetails.eAudioDst)
	{
		case BRAP_AudioDst_eOutputPort:
            ret = BRAP_OP_P_GetMute (
                        hDstHandle->hAssociation->hRap, 
                        hDstHandle->sExtDstDetails.uDstDetails.sOpDetails.eOutput[BRAP_OutputChannelPair_eLR], 
                        pMute
                        );                        
			break;	

		case BRAP_AudioDst_eRingBuffer:
			*pMute = hDstHandle->bMute[BRAP_OutputChannelPair_eLR];
			break;

		default:
			return BERR_TRACE( BERR_INVALID_PARAMETER );			
	}

    BDBG_LEAVE(BRAP_GetDestinationMute);

	return ret;    
}

/***************************************************************************
Summary:
    Sets the volume amplification in gain.

Description:
    This API sets the left and right channel volume amplification in units of Gain ( not in dB). 
    The range of valid gain that can be programmed into hardware is 0 to 16. This
    function requires gain values to be in format as required by hardware ( 5.23
    format ). 

    Desired Function	Input Value to function
    Mute				0
    Normal Volume       0x800000
    Low Volume 	        0x400000
    Increased Volume    0x1800000 to 0x7800000

    Note: This API can be used only for an output port carrying PCM data.
	
Returns:
	BERR_SUCCESS if successful else error value.            
	
See Also:
    BRAP_GetDestinationInputScaling().    
**************************************************************************/
BERR_Code BRAP_SetDestinationInputScaling(
    BRAP_DestinationHandle  hDstHandle,     /* [in] Destination Handle */
    unsigned int            uiLevel,        /* [in] The Mixer level where the volume level needs to be set */
    unsigned int            uiLToLScaling,  /* [in] Left to Left channel scaling in 5.23 format */
    unsigned int            uiRToRScaling,  /* [in] Right to Right channel scaling in 5.23 format */
    unsigned int            uiLToRScaling,  /* [in] Left to Right channel scaling  in 5.23 format */
    unsigned int            uiRToLScaling   /* [in] Right to Left channel scaling in 5.23 format */
    )
{
    BERR_Code               ret = BERR_SUCCESS;
    
    BRAP_MIXER_P_Handle     hMixer = NULL;
    unsigned int            i=0,uiMixerOutput = 0; 
    bool                    bWdgRecovery ;

    BDBG_ENTER( BRAP_SetDestinationInputScaling );
    BDBG_ASSERT( hDstHandle );

    bWdgRecovery = BRAP_P_GetWatchdogRecoveryFlag(hDstHandle->hAssociation->hRap);
    
    BDBG_MSG(("BRAP_SetDestinationInputScaling: hDstHandle = %x,uiLevel=%d,uiLToLScaling =%x,uiRToRScaling =%x,uiLToRScaling=%x,uiRToLScaling=%x",
               hDstHandle,uiLevel,uiLToLScaling,uiRToRScaling,uiLToRScaling,uiRToLScaling));

    if (hDstHandle->sExtDstDetails.eAudioDst == BRAP_AudioDst_eMax)
    {
        BDBG_ERR (("BRAP_SetDestinationInputScaling(): No proper Audio destination in destination handle %x",hDstHandle));
	    return BERR_TRACE (BERR_NOT_SUPPORTED);    
    }

	switch (hDstHandle->sExtDstDetails.eAudioDst)
	{
		case BRAP_AudioDst_eOutputPort:
            if(false == bWdgRecovery)
            {          
            	/* Store the volume parameters */	
    			hDstHandle->uiLtVolume[BRAP_OutputChannelPair_eLR][uiLevel] = uiLToLScaling;
    			hDstHandle->uiRtVolume[BRAP_OutputChannelPair_eLR][uiLevel] = uiRToRScaling;
    			hDstHandle->uiLtBalance[BRAP_OutputChannelPair_eLR][uiLevel] = uiLToRScaling;
    			hDstHandle->uiRtBalance[BRAP_OutputChannelPair_eLR][uiLevel] = uiRToLScaling;
            }
        
    		/* Get the Mixer handle for mixer linked to this output port */
    		ret = BRAP_P_GetCustomMixerForOpPort(hDstHandle->hAssociation->hRap,
    		                                     hDstHandle->sExtDstDetails.uDstDetails.sOpDetails.eOutput[BRAP_OutputChannelPair_eLR],
    		                                     uiLevel,&hMixer,&uiMixerOutput);
            
    		if(BERR_INVALID_PARAMETER == ret)
    		{
    			/* Channel not started yet */
                BDBG_MSG(("Channel not started yet, just storing the values passed."));                
    			ret = BERR_SUCCESS;
    		}
    		else if(BERR_SUCCESS == ret)
    		{
    			/* Call the Mixer volume control API */
                for (i=0;i<BRAP_RM_P_MAX_MIXER_INPUTS;i++)
                {
        	        ret = BRAP_MIXER_P_SetGainCoeff (hMixer, i, uiLToLScaling, uiRToRScaling, uiLToRScaling, uiRToLScaling);
                }
    			if(BERR_SUCCESS != ret)
    			{
    				BDBG_ERR(("BRAP_SetDestinationInputScaling:BRAP_MIXER_P_SetGainCoeff returned"
    					"Error"));
    				return ret;
    			}
    		}
    		else
    		{
    			BDBG_ERR(("BRAP_SetDestinationInputScaling:BRAP_P_GetMixerForOpPort returned"
    				"Error"));
    			return BERR_TRACE(ret);
    		}

		    break;

		case BRAP_AudioDst_eRingBuffer:
            if(false == bWdgRecovery)
            {          
            	/* Store the volume parameters */	
    			hDstHandle->uiLtVolume[BRAP_OutputChannelPair_eLR][uiLevel] = uiLToLScaling;
    			hDstHandle->uiRtVolume[BRAP_OutputChannelPair_eLR][uiLevel] = uiRToRScaling;
    			hDstHandle->uiLtBalance[BRAP_OutputChannelPair_eLR][uiLevel] = uiLToRScaling;
    			hDstHandle->uiRtBalance[BRAP_OutputChannelPair_eLR][uiLevel] = uiRToLScaling;            
            }


			/* Get the corresponding mixer for the ring buffer */
			ret = BRAP_P_GetMixerForRBuf(hDstHandle->hAssociation->hRap, 
			                             hDstHandle->sExtDstDetails.uDstDetails.sRBufDetails.uiRBufId[BRAP_OutputChannelPair_eLR], 
			                             &hMixer, &uiMixerOutput);
				
			if(BERR_INVALID_PARAMETER == ret)
			{
				/* Channel not started yet. */
                BDBG_MSG(("Channel not started yet, just storing the values passed."));                                
				ret = BERR_SUCCESS;
			}
			else if(BERR_SUCCESS == ret)
			{
				/* Call the Mixer volume control API */
                for (i=0;i<BRAP_RM_P_MAX_MIXER_INPUTS;i++)
                {
        	        ret = BRAP_MIXER_P_SetGainCoeff (hMixer, i, uiLToLScaling, uiRToRScaling, uiLToRScaling, uiRToLScaling);
                }
    			if(BERR_SUCCESS != ret)
    			{
    				BDBG_ERR(("BRAP_SetDestinationInputScaling:BRAP_MIXER_P_SetGainCoeff returned"
    					"Error"));
    				return ret;
    			}
			}
			else
			{
				BDBG_ERR(("BRAP_SetDestinationInputScaling:BRAP_P_GetMixerForRBuf returned"
						   "Error"));
				return BERR_TRACE(ret);
			}
			break;

		default: 	
			return BERR_TRACE( BERR_INVALID_PARAMETER );
	} /*end switch (hDstHandle->sExtDstDetails.eAudioDst)*/
        
    BDBG_LEAVE(BRAP_SetDestinationInputScaling);
    return ret;
}

/***************************************************************************
Summary:
    Gets the coefficients for controlling the Scaling of the input audio stream.

Description:
    This function gets the amplification gain value.

Returns:
	BERR_SUCCESS if successful else error value.            
	
See Also:
    BRAP_SetDestinationInputScaling().
**************************************************************************/
BERR_Code BRAP_GetDestinationInputScaling( 
    BRAP_DestinationHandle  hDstHandle,     /* [in] Destination Handle */
    unsigned int            uiLevel,        /* [in] The Mixer level where the volume level needs to be set */
    unsigned int            uiLToLScaling,  /* [out] Left to Left channel scaling in 5.23 format */
    unsigned int            uiRToRScaling,  /* [out] Right to Right channel scaling in 5.23 format */
    unsigned int            uiLToRScaling,  /* [out] Left to Right channel scaling  in 5.23 format */
    unsigned int            uiRToLScaling   /* [out] Right to Left channel scaling in 5.23 format */
    )
{
    BERR_Code               ret = BERR_SUCCESS;

    BDBG_ENTER(BRAP_GetDestinationInputScaling);
    BDBG_ASSERT(hDstHandle); 
	
    BDBG_MSG(("BRAP_GetDestinationInputScaling: hDstHandle = %x",hDstHandle));

    if (hDstHandle->sExtDstDetails.eAudioDst == BRAP_AudioDst_eMax)
    {
        BDBG_ERR (("BRAP_SetDestinationInputScaling(): No proper Audio destination in destination handle %x",hDstHandle));
	    return BERR_TRACE (BERR_NOT_SUPPORTED);    
    }
    
	uiLToLScaling = hDstHandle->uiLtVolume[BRAP_OutputChannelPair_eLR][uiLevel];
	uiRToRScaling = hDstHandle->uiRtVolume[BRAP_OutputChannelPair_eLR][uiLevel];
	uiLToRScaling = hDstHandle->uiLtBalance[BRAP_OutputChannelPair_eLR][uiLevel];
	uiRToLScaling = hDstHandle->uiRtBalance[BRAP_OutputChannelPair_eLR][uiLevel];

    BDBG_LEAVE(BRAP_GetDestinationInputScaling);
    return ret;
}

/***************************************************************************
Summary:
	Sets the Destination volume level for left and right channels. The concept of custom mixers(cascaded mixers)
	is used and the volume level mentioned is at a particular mixer level.

Description:
    This API has the volume control on the Destination.
    The default value written will be 0x800000
    
Note: This API can be used  for both output port and ring buffers carrying PCM data.
    
Returns:
	BERR_SUCCESS if successful else error value.            
	
See Also:
    BRAP_GetDestinationOutputScaling()

**************************************************************************/
BERR_Code BRAP_SetDestinationOutputScaling ( 
    BRAP_DestinationHandle  hDstHandle,     /* [in] Destination Handle */
    unsigned int            uiLevel,        /* [in] The Mixer level where the volume level needs to be set */
    unsigned int    		uiLeftScaling,  /* [in] Left channel volume scaling in 0.23 format */
    unsigned int    		uiRightScaling  /* [in] Right channel volume scaling in 0.23 format */
    )

{
    BERR_Code               ret = BERR_SUCCESS;
	BRAP_MIXER_P_Handle     hMixer = NULL;
   	unsigned int            uiMixerOutput = 0;
	
	BDBG_ENTER(BRAP_SetDestinationOutputScaling);

	BDBG_ASSERT(hDstHandle);	

	BDBG_MSG(("BRAP_SetDestinationOutputScaling: hDstHandle =%x,uiLevel=%d,uiLeftScaling =%x,uiRightScaling =%x",
               hDstHandle,uiLevel,uiLeftScaling,uiRightScaling));

    if (hDstHandle->sExtDstDetails.eAudioDst == BRAP_AudioDst_eMax)
    {
        BDBG_ERR (("BRAP_SetDestinationInputScaling(): No proper Audio destination in destination handle %x",hDstHandle));
	    return BERR_TRACE (BERR_NOT_SUPPORTED);    
    }
    
	switch (hDstHandle->sExtDstDetails.eAudioDst)
	{
		case BRAP_AudioDst_eOutputPort:            
        	/* Store the volume parameters */	
			hDstHandle->uiLtVolume[BRAP_OutputChannelPair_eLR][uiLevel] = uiLeftScaling;
			hDstHandle->uiRtVolume[BRAP_OutputChannelPair_eLR][uiLevel] = uiRightScaling;

    		/* Get the Mixer handle for mixer linked to this output port */
    		ret = BRAP_P_GetCustomMixerForOpPort(hDstHandle->hAssociation->hRap,
    		                                     hDstHandle->sExtDstDetails.uDstDetails.sOpDetails.eOutput[BRAP_OutputChannelPair_eLR],
    		                                     uiLevel,&hMixer,&uiMixerOutput);
            
    		if(BERR_INVALID_PARAMETER == ret)
    		{
    			/* Channel not started yet */
    			ret = BERR_SUCCESS;
    		}
    		else if(BERR_SUCCESS == ret)
    		{
    			/* Call the Mixer volume control API */
    			ret = BRAP_MIXER_P_SetOutputVolumeGain(hMixer,uiMixerOutput,uiLeftScaling,uiRightScaling);
    			if(BERR_SUCCESS != ret)
    			{
    				BDBG_ERR(("BRAP_SetDestinationOutputScaling:BRAP_MIXER_P_SetOutputVolumeGain returned"
    					"Error"));
    				return ret;
    			}
    		}
    		else
    		{
    			BDBG_ERR(("BRAP_SetDestinationOutputScaling:BRAP_P_GetMixerForOpPort returned"
    				"Error"));
    			return BERR_TRACE(ret);
    		}

		    break;

		case BRAP_AudioDst_eRingBuffer:
			/* Store the volume details */
			hDstHandle->uiLtVolume[BRAP_OutputChannelPair_eLR][uiLevel] = uiLeftScaling;
			hDstHandle->uiRtVolume[BRAP_OutputChannelPair_eLR][uiLevel] = uiRightScaling;

			/* Get the corresponding mixer for the ring buffer */
			ret = BRAP_P_GetMixerForRBuf(hDstHandle->hAssociation->hRap, 
			                             hDstHandle->sExtDstDetails.uDstDetails.sRBufDetails.uiRBufId[BRAP_OutputChannelPair_eLR], 
			                             &hMixer, &uiMixerOutput);
				
			if(BERR_INVALID_PARAMETER == ret)
			{
				/* Channel not started yet. */
				ret = BERR_SUCCESS;
			}
			else if(BERR_SUCCESS == ret)
			{
				/* Call the Mixer volume control API */
				ret = BRAP_MIXER_P_SetOutputVolumeGain(hMixer,uiMixerOutput,uiLeftScaling,uiRightScaling);
				if(BERR_SUCCESS != ret)
				{
					BDBG_ERR(("BRAP_SetOutputVolume:BRAP_MIXER_P_SetOutputVolume returned"
						  "Error"));
					return ret;
				}
			}
			else
			{
				BDBG_ERR(("BRAP_SetDestinationOutputScaling:BRAP_P_GetMixerForRBuf returned"
						   "Error"));
				return BERR_TRACE(ret);
			}
			break;

		default: 	
			return BERR_TRACE( BERR_INVALID_PARAMETER );
	}

	BDBG_LEAVE(BRAP_SetDestinationOutputScaling);
	return ret;
	
}


/***************************************************************************
Summary:
	Gets the Destination volume level for left and right channels.

Description:
    This API gets the present Destination volume level. 
    
Note: This API can be used  for both output port and ring buffers carrying PCM data.
    
Returns:
	BERR_SUCCESS if successful else error value.            
	
See Also:
    BRAP_SetDestinationOutputScaling()

**************************************************************************/
BERR_Code BRAP_GetDestinationOutputScaling ( 
    BRAP_DestinationHandle	hDstHandle,         /* [in] Destination Handle */
    unsigned int            uiLevel,            /* [in] The Mixer level where the volume level needs to be set */
    unsigned int *  		puiLeftScaling,  	/* [out] Left channel volume scaling in 0.23 format */
    unsigned int *  		puiRightScaling     /* [out] Right channel volume scaling in 0.23 format */
    )
{
    BERR_Code               ret = BERR_SUCCESS;
	
	BDBG_ENTER(BRAP_GetDestinationOutputScaling);

	BDBG_ASSERT(hDstHandle);	

    BDBG_MSG(("BRAP_GetDestinationOutputScaling: hDstHandle = %x,uiLevel=%d",hDstHandle,uiLevel));	

    if (hDstHandle->sExtDstDetails.eAudioDst == BRAP_AudioDst_eMax)
    {
        BDBG_ERR (("BRAP_SetDestinationInputScaling(): No proper Audio destination in destination handle %x",hDstHandle));
	    return BERR_TRACE (BERR_NOT_SUPPORTED);    
    }
    
	*puiLeftScaling = hDstHandle->uiLtVolume[BRAP_OutputChannelPair_eLR][uiLevel];
	*puiRightScaling = hDstHandle->uiRtVolume[BRAP_OutputChannelPair_eLR][uiLevel];

	BDBG_LEAVE(BRAP_GetDestinationOutputScaling);	

	return ret;
}


/***************************************************************************
Summary:
    Mute/unmute an output port for uncompressed data

Description:
    This function mutes or unmutes output an output port. The mute control is 
    available at the mixer feeding data to an output port, so the corresponding
    mixer is muted/unmuted in this API.
    
    Note: 
    1. This API can be used only for an output port carrying uncompressed data.
    2. These mutes/umutes also ramp up/down the volume in hardware, so there is 
    no need to bring the volume down when using these mute controls.  

Returns:
	BERR_SUCCESS if successful else error value.            
	
See Also:
    BRAP_GetOutputMute().
**************************************************************************/
BERR_Code BRAP_SetOutputMute ( 
    BRAP_Handle     hRap,    /* [in] Audio Device Handle */
    BRAP_OutputPort eOpType, /* [in] Output Type */
    bool            bMute    /* [in] TRUE: Mute output
                                     FALSE: UnMute output */
)
{
    BERR_Code   ret = BERR_SUCCESS;
    BRAP_MIXER_P_Handle     hMixer = NULL;
    unsigned int            uiMixerOutput = 0;

    BDBG_ENTER(BRAP_SetOutputMute);
    BDBG_ASSERT(hRap);

 /*Muting Both at mixer + O/P Level. When Mute=true, First mute mixer then O/P to 
get the effect of ramping. When Mute=false, do it in reverse way*/                           
    if(bMute == false)     
    {
        ret = BRAP_OP_P_SetMute (
                hRap, 
                eOpType,
                bMute
                );
        if(BERR_SUCCESS != ret)
        {
            BDBG_ERR(("BRAP_SetOutputMute: Could not Mute the Output"));
            return BERR_TRACE(ret);                                  
        }    
    }
    /* Get the corresponding mixer for the ring buffer */
    ret = BRAP_P_GetMixerForOpPort(hRap,eOpType, &hMixer, &uiMixerOutput);              
    if(BERR_SUCCESS == ret)
    {
        /* Call the internal private function */
        ret = BRAP_MIXER_P_SetMute(hMixer,bMute,uiMixerOutput);
        if(BERR_SUCCESS != ret)
        {
            BDBG_ERR(("BRAP_SetOutputMute: Could not Mute the Output"));
        }
    }
    else if(BERR_INVALID_PARAMETER == ret)
    {
        /* Channel not started yet. */
        ret = BERR_SUCCESS;
    }                    
    else
    {
        BDBG_ERR(("BRAP_SetOutputMute:BRAP_P_GetMixerForRBuf returned"
        "Error"));
        return BERR_TRACE(ret);
    }
    if(bMute == true)     
    {                            
        ret = BRAP_OP_P_SetMute (
                hRap, 
                eOpType,
                bMute
                );
    if(BERR_SUCCESS != ret)
    {
        BDBG_ERR(("BRAP_SetOutputMute: Could not Mute the Output"));
            return BERR_TRACE(ret);                                  
        }                                
    }

    BDBG_LEAVE(BRAP_SetOutputMute);
    return ret;
}


/***************************************************************************
Summary:
    Gets whether an output is muted or not.

Description:
    It can be used only for an output port carrying uncompressed data.

Returns:
	BERR_SUCCESS if successful else error value.            
	
See Also:
    BRAP_SetOutputMute().
    
**************************************************************************/
BERR_Code BRAP_GetOutputMute ( 
    BRAP_Handle     hRap,    /* [in] Audio Device Handle */
    BRAP_OutputPort eOpType, /* [in] Output Type */
    bool            *pMute   /* [out] Pointer to memory where the Mute 
                                status is to be written */            
)
{
    BERR_Code           ret = BERR_SUCCESS;

    BDBG_ENTER(BRAP_GetOutputMute);
    BDBG_ASSERT(hRap);

    BRAP_OP_P_GetMute(hRap,eOpType,pMute);

    BDBG_LEAVE(BRAP_GetOutputMute);
    return ret;
}

/***************************************************************************
Summary:
	Change channel configuration parameters on the fly. 

Description:
	This function sets an audio channel configuration parameters that can be 
	set	on the fly.
	Currently, this is applicable only for a decode channel, for all others
	it will return an error. 

	Change in configuration parameters depend on the timing of calling
	this API. There are following three cases.
	1. If currently decode is not happening on this channel then all 
	configuration parameters are stored in PI. When next decode starts, common 
	parameters are applied	irrespective of audio type of next decode, but 
	audio-type specific parameters are applied only on next decode with same 
	audio type.
	2. If currently channel is decoding other audio type,then only common 
	parameters are applied immediately and audio-type specific parameters are 
	stored in PI. Audio-type specific parameters are applied on next decode 
	start with same audio type.
	3. If decode is running with the same audio type, all the configuration 
	parameters are applied immediately.
    
Returns:
	BERR_SUCCESS if successful else error value.            

See Also:
	BRAP_GetDefaultChannelConfig
	BRAP_GetCurrentChannelConfig
****************************************************************************/
BERR_Code BRAP_SetChannelConfig (
	BRAP_ChannelHandle		    hRapCh,		        /* [in] RAP channel handle */
	BRAP_ChannelConfigParams	*psChConfigParams	/* [in] Channel configuration
											           parameters */
	)
{
	BERR_Code ret = BERR_SUCCESS;
    BRAP_MixingCoef         *psTempMixingCoeff =NULL;
		
	BDBG_ENTER(BRAP_SetChannelConfig);

    /* Assert the function argument(s) */
	BDBG_ASSERT(hRapCh);
	BDBG_ASSERT(psChConfigParams);

    if(hRapCh->eChannelType != psChConfigParams->eChType)
    {
        BDBG_ERR(("hRapCh->eChannelType = %d different from "
            "psChConfigParams->eChType = %d", 
            hRapCh->eChannelType,psChConfigParams->eChType));
        ret = BERR_TRACE(BERR_INVALID_PARAMETER);
        goto end;
    }

    psTempMixingCoeff =(BRAP_MixingCoef *)BKNI_Malloc(sizeof(BRAP_MixingCoef));
    if( NULL==psTempMixingCoeff)
    {
        ret = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
        goto end;        
    }


	switch(psChConfigParams->eChType)
	{
		case BRAP_ChannelType_eDecode:
            ret = BRAP_DEC_P_SetConfig(hRapCh, 
                    &(psChConfigParams->uChConfigParams.sDecConfigParams));
            if(BERR_SUCCESS != ret)
            {
                BDBG_ERR(("BRAP_DEC_P_SetConfig: returned %d",ret));
                ret =  BERR_TRACE(ret);
                goto end;                
            }

 
            /* Program the mixing Coefficients for the channel */
            *psTempMixingCoeff = hRapCh->sMixingCoeff;
            hRapCh->sMixingCoeff = psChConfigParams->sMixingCoef;            
            ret = BRAP_P_ProgramCoefficients(hRapCh, &(psChConfigParams->sMixingCoef));
            if(BERR_SUCCESS != ret)
            {
                /* If programming is unsuccessful, restore the old coeff in the Rap Channel Handle */            
                hRapCh->sMixingCoeff = *psTempMixingCoeff;
            	BDBG_ERR(("BRAP_SetChannelConfig: BRAP_P_ProgramCoefficients failed %d",
                    ret));
            	ret = BERR_TRACE(ret); 
                goto end;                    
            }
            
            break;
            
		case BRAP_ChannelType_ePcmPlayback:
          
            /* Program the mixing Coefficients for the channel */
            *psTempMixingCoeff = hRapCh->sMixingCoeff;
            hRapCh->sMixingCoeff = psChConfigParams->sMixingCoef;             
            ret = BRAP_P_ProgramCoefficients(hRapCh, &(psChConfigParams->sMixingCoef));
            if(BERR_SUCCESS != ret)
            {
                /* If programming is unsuccessful, restore the old coeff in the Rap Channel Handle */            
                hRapCh->sMixingCoeff = *psTempMixingCoeff;            
            	BDBG_ERR(("BRAP_SetChannelConfig: BRAP_P_ProgramCoefficients failed %d",
                    ret));
            	ret =  BERR_TRACE(ret); 
                goto end;                
            }
            
            break;
		case BRAP_ChannelType_ePcmCapture:
			/* Program the mixing Coefficients for the channel */
                    *psTempMixingCoeff = hRapCh->sMixingCoeff;
                    hRapCh->sMixingCoeff = psChConfigParams->sMixingCoef;             
			ret  = BRAP_P_ProgramCoefficients(hRapCh, &(psChConfigParams->sMixingCoef));            
			if(BERR_SUCCESS != ret)
			{            	
                        /* If programming is unsuccessful, restore the old coeff in the Rap Channel Handle */            
                        hRapCh->sMixingCoeff = *psTempMixingCoeff;			
        			BDBG_ERR(("BRAP_StartChannel: BRAP_P_ProgramCoefficients failed %d",                   
        				ret));            	
        			ret = BERR_TRACE(ret);            
                        goto end;                   
			}            
			break;
#ifdef RAP_VIDEOONDSP_SUPPORT
        case BRAP_ChannelType_eVideoDecode:
            break;          
#endif 
        default:
            BDBG_ERR(("Unsupported channel type %d",psChConfigParams->eChType));
            ret = BERR_TRACE(BERR_NOT_SUPPORTED);
            break;
    }
end:    
    if(psTempMixingCoeff)
        BKNI_Free(psTempMixingCoeff);
    BDBG_LEAVE(BRAP_SetChannelConfig);
    return ret;
}

/***************************************************************************
Summary:
	Get default channel configuration parameters

Description:
	This function gets the default configuration parameters for given 
	channel type. 

Returns:
	BERR_SUCCESS if successful else error value.            

See Also:
	BRAP_SetChannelConfig
	BRAP_GetCurrentChannelConfig
****************************************************************************/
BERR_Code BRAP_GetDefaultChannelConfig (
	BRAP_ChannelHandle	        hRapCh,	/* [in] RAP channel handle */
	BRAP_DSPCHN_AudioType	    eType,	/* [in] Audio type for which to get
									       default configuration parameters.
									       This field is valid only for a decode 
									       channel. For all others this is 
									       ignored */
	BRAP_ChannelConfigParams	*psChConfigParams	
	                                    /* [out] Channel configuration
									       parameters */
	)
{
    BERR_Code   ret = BERR_SUCCESS;
    int         i=0,j=0;
#if(BRAP_7405_FAMILY == 1) || (BRAP_3548_FAMILY == 1)
	BRAP_ProcessingType ePpAlgo = BRAP_ProcessingType_eMax;
#endif

    BDBG_ENTER(BRAP_GetDefaultChannelConfig);
    BDBG_ASSERT(hRapCh);
    BDBG_ASSERT(psChConfigParams);
                    
	switch(hRapCh->eChannelType)
	{
		case BRAP_ChannelType_eDecode:
            ret = BRAP_DEC_P_GetDefaultConfig(hRapCh, 
                        eType,
                        ePpAlgo,
                        &(psChConfigParams->uChConfigParams.sDecConfigParams)
                        );
            if(BERR_SUCCESS != ret)
            {
                BDBG_ERR(("BRAP_DEC_P_GetDefaultConfig: returned %d",ret));
                BDBG_ASSERT(0);
                return BERR_TRACE(ret);
            }

            /* Fill up sMixingCoeff with invalid values, since PI can't provide these
               values */
            for(i=0;i<BRAP_OutputChannel_eMax;i++)
            {
                for(j=0;j<BRAP_OutputChannel_eMax;j++)
                {
                    if(i == j)
                    {
                        psChConfigParams->sMixingCoef.ui32Coef[i][j]=BRAP_MIXER_P_DEFAULT_SCALING_COEFF;
                    }
                    else
                    {
                        psChConfigParams->sMixingCoef.ui32Coef[i][j]=0;
                    }
                }
            }
            break;
        case BRAP_ChannelType_ePcmPlayback:
            /* Fill up sMixingCoeff with invalid values, since PI can't provide these
               values */
            for(i=0;i<BRAP_OutputChannel_eMax;i++)
            {
                for(j=0;j<BRAP_OutputChannel_eMax;j++)
                {
                    if(i == j)
                    {
                        psChConfigParams->sMixingCoef.ui32Coef[i][j]=BRAP_MIXER_P_DEFAULT_SCALING_COEFF;
                    }
                    else
                    {
                        psChConfigParams->sMixingCoef.ui32Coef[i][j]=0;
                    }
                }
            }
            break;
#ifdef RAP_VIDEOONDSP_SUPPORT
        case BRAP_ChannelType_eVideoDecode:
            break;          
#endif
        default:
            BDBG_ERR(("Unsupported channel type %d", hRapCh->eChannelType));
            ret = BERR_TRACE(BERR_NOT_SUPPORTED);
            break;
    }

    /* Also, populate the channel type */
    psChConfigParams->eChType = hRapCh->eChannelType;
    
    BDBG_LEAVE(BRAP_GetDefaultChannelConfig);
    return ret;
}

									   
/***************************************************************************
Summary:
	Get current channel configuration parameters

Description:
	This function gets the current configuration parameters for given channel 
	type.

Returns:
	BERR_SUCCESS if successful else error value.            

See Also:
	BRAP_SetChannelConfig
	BRAP_GetDefaultChannelConfig
****************************************************************************/
BERR_Code BRAP_GetCurrentChannelConfig (
	BRAP_ChannelHandle	        hRapCh,	/* [in] RAP channel handle */
	BRAP_DSPCHN_AudioType	    eType,	/* [in] Audio type for which to get
									       default configuration parameters.
									       This field is valid only for a decode 
									       channel. For all others this is 
									       ignored. If current configuration is
									       required only for post processing 
									       stage and not for decode stage, then
									       this field should be initialized to
									       BRAP_DSPCHN_AudioType_eMax */
	BRAP_ChannelConfigParams    *psChConfigParams	
	                                    /* [out] Channel configuration
									       parameters */
	)
{
    BERR_Code ret = BERR_SUCCESS;
#if (BRAP_7405_FAMILY == 1) || (BRAP_3548_FAMILY == 1)
	/* Setting uiPpBranchId and uiPpStageId to max values makes sure that
	 * low level function doesn't return error if there is no post processing
	 * algorithm set for that stage */
	unsigned int uiPpBranchId = BRAP_MAX_PP_BRANCH_SUPPORTED;
	unsigned int uiPpStageId = BRAP_MAX_PP_PER_BRANCH_SUPPORTED;
#endif

    BDBG_ENTER(BRAP_GetCurrentChannelConfig);
    BDBG_ASSERT(hRapCh);
    BDBG_ASSERT(psChConfigParams);

    
	switch(hRapCh->eChannelType)
 	{
		case BRAP_ChannelType_eDecode:
            ret = BRAP_DEC_P_GetCurrentConfig(hRapCh, 
                        eType,
                        uiPpBranchId,
                        uiPpStageId,
                        &(psChConfigParams->uChConfigParams.sDecConfigParams));
            if(BERR_SUCCESS != ret)
            {
                BDBG_ERR(("BRAP_DEC_P_GetCurrentConfig: returned %d",ret));
                BDBG_ASSERT(0);
                return BERR_TRACE(ret);
            }

            psChConfigParams->sMixingCoef = hRapCh->sMixingCoeff;
            break;
   		case BRAP_ChannelType_ePcmPlayback:
            psChConfigParams->sMixingCoef = hRapCh->sMixingCoeff;
            break;
         
		case BRAP_ChannelType_ePcmCapture:
			psChConfigParams->sMixingCoef = hRapCh->sMixingCoeff;            
			break;
#ifdef RAP_VIDEOONDSP_SUPPORT
        case BRAP_ChannelType_eVideoDecode:
            break;          
#endif
        default:
            BDBG_ERR(("Unsupported channel type %d",psChConfigParams->eChType));
            ret = BERR_TRACE(BERR_NOT_SUPPORTED);
            break;
    }

    /* Also, populate the channel type */
    psChConfigParams->eChType = hRapCh->eChannelType;  
        
    BDBG_LEAVE(BRAP_GetCurrentChannelConfig);
    return ret;
}
/***************************************************************************
Summary:
    Sets the coefficients for controlling the Scaling of the Input audio stream.

Description:
	
Returns:
	BERR_SUCCESS if successful else error value.            
	
See Also:
    BRAP_GetGain().
**************************************************************************/
BERR_Code BRAP_SetGain(
    BRAP_ChannelHandle  hRapCh,     /* [in] Rap channel Handle */
    BRAP_GainControl    *psGain     /* [in] Gain coefficients Structure */
)
{
    BERR_Code                       ret = BERR_SUCCESS;
    unsigned int                    path = 0, chPair = 0;
    unsigned int                    uiLvl = 0, uiPp = 0;
    unsigned int                    uiLvl1 = 0, uiPp1 = 0;
    BRAP_OutputChannelPair          eChP = BRAP_OutputChannelPair_eMax;
    BRAP_OutputChannelPair          eChP1 = BRAP_OutputChannelPair_eMax;
    BRAP_MIXER_P_Handle             hMixer = NULL;
    BRAP_RM_P_MixerGrant            sTempMixerGrnt;
    BRAP_RM_P_ResrcGrant            *psTempGrant =NULL;
    unsigned int                    i=0, uiMixerIp =0;
    
    BDBG_ENTER(BRAP_SetGain);

    BDBG_ASSERT(hRapCh);
    BDBG_ASSERT(psGain);

    /* Get the first mixer where this input stream is going */

    /* Trace all the paths to find first mixer where these coefficents can be 
       applied */
    for (path=0;path<BRAP_P_MAX_PATHS_IN_A_CHAN;path++)
    {
        if(!(BRAP_P_IsPointerValid((void *)hRapCh->pPath[path])))
        {
            continue;
        }
        for(chPair =0 ; chPair <  BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; chPair++)
        {
            /* If channel pair present */
            if (true == hRapCh->pPath[path]->sSrcCh[chPair].bValid)
            {
                BDBG_MSG(("BRAP_P_ProgramCoefficients: Path_id = %d",path));
                /* Go to the SRC next to the srcch */
                uiLvl =hRapCh->pPath[path]->sSrcCh[chPair].sSrcChOutLink.uiLevel;
                uiPp =hRapCh->pPath[path]->sSrcCh[chPair].sSrcChOutLink.uiPrlPth;
                eChP =hRapCh->pPath[path]->sSrcCh[chPair].sSrcChOutLink.eChnPair;


                BDBG_MSG(("BRAP_P_ProgramCoefficients: Src Lvl = %d"
                          "Prll_Path = %d, Chan_Pair = %d ",
                          uiLvl,uiPp,eChP));
                
                /* for every Mixer connected to a SRC in Parallel */
                for(i=0;i<BRAP_RM_P_MAX_RSRCS_CONNECTED_TO_SAME_SRC;i++)
                {
                    /* If valid resource and its a mixer */
                    if(BRAP_P_Rsrc_eMixer == 
                        hRapCh->pPath[path]->sSrc[uiLvl][eChP][uiPp].
                                                      sSrcOutLink[i].eRsrcType)
                    {
                        /* Get the level, pp and channel pair for the mixer */
                        uiLvl1 = 
                            hRapCh->pPath[path]->sSrc[uiLvl][eChP][uiPp].
                                                        sSrcOutLink[i].uiLevel;
                        uiPp1 = 
                            hRapCh->pPath[path]->sSrc[uiLvl][eChP][uiPp].
                                                        sSrcOutLink[i].uiPrlPth;
                        eChP1 =
                            hRapCh->pPath[path]->sSrc[uiLvl][eChP][uiPp].
                                                        sSrcOutLink[i].eChnPair;
                        BDBG_MSG(("BRAP_P_ProgramCoefficients: Mixer Lvl = %d"
                                  "Prll_Path = %d, Chan_Pair = %d ",
                                  uiLvl1,uiPp1,eChP1));
                        
                        /* Store the handle */
                        hMixer = 
                        hRapCh->pPath[path]->sMixer[uiLvl1][eChP1][uiPp1].hMixer;                           

                        /* Find the Mixer Input for this channel pair */
                        psTempGrant = &(hRapCh->pPath[path]->sRsrcGrnt);

                        sTempMixerGrnt = 
                            psTempGrant->sSrcMixerGrnt[uiLvl1].sMixerGrant
                                                                 [eChP1][uiPp1];

                        uiMixerIp = sTempMixerGrnt.uiMixerInputId[chPair];

                        ret = BRAP_MIXER_P_SetGainCoeff(hMixer,uiMixerIp,
                                                   psGain->ui32LeftGain[chPair],
                                                   psGain->ui32RightGain[chPair],
                                                   0x800000,
                                                   0x800000);
                        if(BERR_SUCCESS != ret)
                        {
                            BDBG_ERR(("Could not program the gain:"
                                " BRAP_MIXER_P_SetGainCoeff failed"));
                            return BERR_TRACE(ret);
                        }
                    }
                }
            }
        }
    }

    /* If everything goes well, update gain coeff in hRapCh */
    if (BERR_SUCCESS == ret)
    {
        hRapCh->sGainInfo = *psGain;
    }

    BDBG_MSG(("BRAP_SetGain: Gain Coeff updated successfully "));
    BDBG_LEAVE(BRAP_SetGain);
    return ret;
}
/***************************************************************************
Summary:
    Gets the coefficients for controlling the Scaling of the Input audio stream.

Description:
    This function gets the mixer coefficients value.
    Note: This API returns only the gain coefficients programmed by the 
    application. If metadata is enabled in secondary, the value returned by this 
    API would be the value programmed by the Application.

Returns:
	BERR_SUCCESS if successful else error value.            
	
See Also:
    BRAP_SetGain().
**************************************************************************/
BERR_Code BRAP_GetGain(
    BRAP_ChannelHandle  hRapCh,     /* [in] Rap channel Handle */
    BRAP_GainControl    *psGain     /* [out] Gain coefficients Structure */
)
{
    BERR_Code   ret = BERR_SUCCESS;
    
    BDBG_ENTER(BRAP_GetGain);

    BDBG_ASSERT(hRapCh);
    BDBG_ASSERT(psGain);

    *psGain = hRapCh->sGainInfo;

    BDBG_LEAVE(BRAP_GetGain);
    return ret;
}

/***************************************************************************
Summary:
	API returns the buffer requirements for CDB and ITB.

Description:
	This API returns the size required for the CDB and ITB buffers
	based upon the algorithm type to be used.
	
Returns:
	BERR_SUCCESS 

See Also:
	None
	
****************************************************************************/
BERR_Code BRAP_GetBufferConfig ( 	
	BRAP_Handle 		hRap,				/* [in] The RAP device handle */
	BRAP_BufferCfgMode	eCfgMode,			/* [in] CDB/ITB Configuration Mode */
	BAVC_CdbItbConfig	*pCdbItbConfigInfo /* [out] Buffer information of CDB and ITB*/ 
	)
{
	BERR_Code ret = BERR_SUCCESS;

	BDBG_ENTER(BRAP_GetBufferConfig);

    	/* Check input parameters */
	BDBG_ASSERT(hRap);
    BSTD_UNUSED(hRap);
	BDBG_ASSERT(pCdbItbConfigInfo);
	BDBG_ASSERT(eCfgMode < BRAP_BufferCfgMode_eMaxMode);

        if(eCfgMode >= BRAP_BufferCfgMode_eMaxMode)
        {
            BDBG_ERR(("eCfgMode = %d  can't be greater  or equal to than BRAP_BufferCfgMode_eMaxMode = %d",eCfgMode,BRAP_BufferCfgMode_eMaxMode));
            BDBG_ASSERT(0);
            return BERR_TRACE(BERR_INVALID_PARAMETER);
        }

	/* Get the CDB & ITB info corresponding to the buffer config mode */
	*pCdbItbConfigInfo = sCdbItbCfg[eCfgMode];

	BDBG_LEAVE(BRAP_GetBufferConfig);
   	return ret;
}

#if (BRAP_7550_FAMILY !=1)
/***************************************************************************
Summary:
	Sets the config for input ports e.g. I2s In.
	
Description:
    
Returns:

See Also:
	BRAP_GetDefaultInputConfig
	BRAP_GetCurrentInputConfig
****************************************************************************/
BERR_Code BRAP_SetInputConfig (
	BRAP_Handle		            hRap,		    /* [in] RAP device handle */
	const BRAP_InputPortConfig	*pInConfig	    /* [in] Input port 
	                                               configuration */
	)
{
    BERR_Code   ret = BERR_SUCCESS;

    BDBG_ENTER(BRAP_SetInputConfig);

    /* Validate the Input Parameters */
    BDBG_ASSERT(hRap);
    BDBG_ASSERT(pInConfig);

    /* Check for the Input config params */
    if((pInConfig->eCapPort < BRAP_CapInputPort_eExtI2s0)&&
       ((pInConfig->eCapPort >= BRAP_CapInputPort_eIntCapPort4) &&
       (pInConfig->eCapPort <= BRAP_CapInputPort_eIntCapPort7)))
    {
        BDBG_ERR((" Input configuration supported only for external capture ports "));
        ret = BERR_TRACE(BERR_INVALID_PARAMETER);
        goto end;                

    }
    if(pInConfig->eCapPort >= BRAP_CapInputPort_eMax)
    {
        BDBG_ERR((" Wrong Capture Port Index "));
        ret = BERR_TRACE(BERR_INVALID_PARAMETER);
        goto end;                
    }

    /* Check if capture port is supported for this platform */
#if ((BRAP_7405_FAMILY == 1))
    if((pInConfig->eCapPort == BRAP_CapInputPort_eRfAudio)
        ||(pInConfig->eCapPort == BRAP_CapInputPort_eSpdif)
        ||(pInConfig->eCapPort == BRAP_CapInputPort_eHdmi)
        ||(pInConfig->eCapPort == BRAP_CapInputPort_eAdc))
    {
        BDBG_ERR((" RF Audio, ADC, SPDIF and HDMI input ports are not supported in 7405 "));
        ret = BERR_TRACE(BERR_NOT_SUPPORTED);
        goto end;        
        
    }
#endif

    if(pInConfig->eBufDataMode > BRAP_BufDataMode_eMaxNum)
    {
        BDBG_ERR(("BRAP_SetInputConfig: Not a Supported BufDataMode = %d",
                    pInConfig->eBufDataMode));
        ret = BERR_TRACE(BERR_INVALID_PARAMETER);
        goto end;        

    }

    /* Check if this port is opened or not */
    if ( (BRAP_P_IsPointerValid((void *)hRap->hFmm[0]->hCapPort[pInConfig->eCapPort])) &&
         (pInConfig->eCapPort != BRAP_CapInputPort_eRfAudio)
       )
    {
        BDBG_ERR(("BRAP_SetInputConfig: This port is already Opened,"
                   "Cannot configure "));
        ret = BERR_TRACE(BERR_INVALID_PARAMETER);
        goto end;        
    }

    /* Save the settings in the device handle */
    hRap->sInputSettings[pInConfig->eCapPort] = *pInConfig;

    if ( (pInConfig->eCapPort == BRAP_CapInputPort_eAdc) && 
         (pInConfig->eInputBitsPerSample != BRAP_InputBitsPerSample_e16)
       )  
    {
        /* ADC operates at 16 bits per sample only */
        hRap->sInputSettings[pInConfig->eCapPort].eInputBitsPerSample = BRAP_InputBitsPerSample_e16;
        BDBG_MSG (("BRAP_SetInputConfig: ADC always has 16 bits per sample"));
    }
    

    /* Set the corresponding flag */
    hRap->bInPortSettingValid[pInConfig->eCapPort] = true;

#ifdef RAP_RFAUDIO_SUPPORT
    /* For BTSC the decoder has to be started again */
    if (BRAP_CapInputPort_eRfAudio == pInConfig->eCapPort)
    {
        if (BRAP_P_IsPointerValid((void *)hRap->hFmm[0]->hCapPort[BRAP_CapInputPort_eRfAudio] ))
        {
            ret = BRAP_RFAUDIO_P_SetSettings (hRap->hFmm[0]->hCapPort[BRAP_CapInputPort_eRfAudio]);

            if (ret != BERR_SUCCESS)
            {
                BDBG_ERR(("BRAP_SetInputConfig: RF Audio Set Settings failed,"
                   "Could not configure "));
                ret = BERR_TRACE (ret);
                goto end;
            }
        }
        else
        {
            ret = BRAP_RFAUDIO_P_InitDecoder(hRap);

            if (ret != BERR_SUCCESS)
            {
                BDBG_ERR(("BRAP_SetInputConfig: RF Audio InitDecoder failed,"
                   "Could not configure "));
                ret = BERR_TRACE (ret);  
                goto end;                
            }            
        }
    }
#endif    
end:    
    BDBG_LEAVE(BRAP_SetInputConfig);
    return ret;
}

/***************************************************************************
Summary:
	Get default Input port configuration

Description:
	This function gets the default Input port configuration 
	
Returns:
	BERR_SUCCESS if successful else error value

See Also:
	BRAP_SetInputConfig
	BRAP_GetCurrentInputConfig
****************************************************************************/
BERR_Code BRAP_GetDefaultInputConfig (
	BRAP_Handle	            hRap,	        /* [in] RAP device handle */
	BRAP_CapInputPort       eCapPort,       /* [in] Input port */
	BRAP_InputPortConfig	*pInConfig      /* [out] Default configuration for
	                                           the Input port */
	)
{
    BERR_Code   ret = BERR_SUCCESS;

    BDBG_ENTER(BRAP_GetDefaultInputConfig);
    
    /* Check for the Input cap port */
    if(BRAP_P_IsInternalCapPort(eCapPort))
    {
        BDBG_ERR((" No default input config for internal ports"));
        BDBG_ASSERT(0);
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    *pInConfig = hRap->sInputSettings[eCapPort];

    BDBG_LEAVE(BRAP_GetDefaultInputConfig);
    return ret;    

}

BERR_Code BRAP_P_GetDefaultInputSettings (
	BRAP_Handle	            hRap,	        /* [in] RAP device handle */
	BRAP_CapInputPort       eCapPort,       /* [in] Input port */
	BRAP_InputPortConfig	*pInConfig      /* [out] Default configuration for
	                                           the Input port */
       )
{
    BERR_Code   ret = BERR_SUCCESS;

    BDBG_ENTER(BRAP_P_GetDefaultInputSettings);

    /* Validate the Input Parameters */
    BDBG_ASSERT(hRap);
    BDBG_ASSERT(pInConfig);
    BSTD_UNUSED(hRap);

    /* Check for the Input cap port */
    if(BRAP_P_IsInternalCapPort(eCapPort))
    {
        BDBG_ERR((" No default input config for internal ports"));
        BDBG_ASSERT(0);
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    /* Prepare the settings */
    pInConfig->eCapPort = eCapPort;

    pInConfig->eBufDataMode = BRAP_BufDataMode_eStereoNoninterleaved;
    pInConfig->eInputBitsPerSample = BRAP_InputBitsPerSample_e32;
    pInConfig->eSampleRate = BAVC_AudioSamplingRate_e48k;                
    
    switch(eCapPort)
        {
            case BRAP_CapInputPort_eExtI2s0:
                pInConfig->sCapPortParams.uCapPortParams.sInputI2sParams = defI2sCapPortParams;
                pInConfig->eInputBitsPerSample = BRAP_InputBitsPerSample_e16;
                break;
                
#if ( (BRAP_3548_FAMILY == 1) )
            case BRAP_CapInputPort_eRfAudio:
                pInConfig->sCapPortParams.uCapPortParams.sRfAudioParams = defRfAudioCapPortParams;
                break;
                
            case BRAP_CapInputPort_eSpdif:
                pInConfig->sCapPortParams.uCapPortParams.sSpdifRxParams = defSpdifCapPortParams;
                break;
                
            case BRAP_CapInputPort_eHdmi:
                pInConfig->sCapPortParams.uCapPortParams.sHdmiRxParams = defHdmiCapPortParams;
                break;
                
            case BRAP_CapInputPort_eAdc:
                pInConfig->sCapPortParams.uCapPortParams.sAdcParams = defAdcCapPortParams;
                /* For ADC input bits per sample is always 16 */
                pInConfig->eInputBitsPerSample = BRAP_InputBitsPerSample_e16;
                pInConfig->eSampleRate = BAVC_AudioSamplingRate_e48k;
                break;
#endif
            default:
            /*Do nothing*/
                break;
        }

    BDBG_LEAVE(BRAP_P_GetDefaultInputSettings);
    return ret;
}
/***************************************************************************
Summary:
	Get current Input port configuration

Description:
	This function gets the current Input port configuration. If the caller has 
	not configured the output port before, this API will return error. 
 	
Returns:
	BERR_SUCCESS if successful else error value

See Also:
	BRAP_SetInputConfig
	BRAP_GetDefaultinputConfig
****************************************************************************/
BERR_Code BRAP_GetCurrentInputConfig (
	BRAP_Handle	            hRap,	        /* [in] RAP device handle */
	BRAP_CapInputPort       eCapPort,       /* [in] Input port */
	BRAP_InputPortConfig	*pInConfig      /* [out] Current configuration for
	                                           the input port */
    )
{
    BERR_Code   ret = BERR_SUCCESS;

    BDBG_ENTER(BRAP_GetCurrentInputConfig);

    /* Validate the Input Parameters */
    BDBG_ASSERT(hRap);
    BDBG_ASSERT(pInConfig);

    if(hRap->bInPortSettingValid[eCapPort] == true)
    {
        *pInConfig = hRap->sInputSettings[eCapPort];
    }
    else
		return BERR_TRACE(BRAP_ERR_OUTPUT_NOT_CONFIGURED);

    BDBG_LEAVE(BRAP_GetCurrentInputConfig);
    return ret;
}
#endif
/*****************************************************************************
Summary:
    Used to install an application callback.

Description:
    This PI should be used by the application to install an application 
    specified callback in addition to the RAptor PI interrupt handler for that 
    interrupt. The speficied Application callback gets called from the interrupt
    handler that the Raptor PI already has installed. 
    This can be done only for the interrupts that are handled by Raptor PI and
    listed in BRAP_Interrupt. For all other interrupts, application has to use 
    the standard INT interface.
    
Returns:
    BERR_SUCCESS on success
    Error code on failure
******************************************************************************/
BERR_Code BRAP_InstallAppInterruptCb (
	BRAP_ChannelHandle hRapCh,	   /* [in] The RAP channel handle */
    BRAP_Interrupt     eInterrupt, /* [in] The interrupt that needs to 
                                      be activated */
    BRAP_CallbackFunc  pfAppInterruptCb, /* [in] The RAP callback function 
                                      to be registered for eInterrupt*/ 
    void               *pParm1,    /* [in] The application specified parameter 
                                      that needs to be passed unchanged to the 
                                      callback */
    int                iParm2      /* [in] The application specified parameter 
                                      that needs to be passed unchanged to the 
                                      callback */
)
{
    BERR_Code ret = BERR_SUCCESS;
    uint32_t ui32IntMask = 0;

    BDBG_ENTER (BRAP_InstallAppInterruptCb);

    /* Check input parameters */
    BDBG_ASSERT(hRapCh);
    BDBG_ASSERT(pfAppInterruptCb);

#ifndef BRAP_SUPPORT_TSM_LOG
	if (eInterrupt==BRAP_Interrupt_eTsmLog) {
		BDBG_ERR(("To support TSM Log, enable it at compile time by defining \
			flag BRAP_SUPPORT_TSM_LOG"));
		return BERR_TRACE(BERR_NOT_SUPPORTED);
	}
#endif

	hRapCh->sAppIntCbInfo[eInterrupt].pfAppCb = pfAppInterruptCb;
	hRapCh->sAppIntCbInfo[eInterrupt].pParm1  = pParm1;
	hRapCh->sAppIntCbInfo[eInterrupt].iParm2  = iParm2;

    BDBG_MSG(("BRAP_InstallAppInterruptCb: pfAppCb=%d", \
		hRapCh->sAppIntCbInfo[eInterrupt].pfAppCb));
    BDBG_MSG(("BRAP_InstallAppInterruptCb: pParm1=%d", \
		hRapCh->sAppIntCbInfo[eInterrupt].pParm1));
    BDBG_MSG(("BRAP_InstallAppInterruptCb: iParm2=%d", \
		hRapCh->sAppIntCbInfo[eInterrupt].iParm2));

#ifdef RAP_GFX_SUPPORT
        if(eInterrupt != BRAP_Interrupt_eGfxOperationComplete)
#endif
#ifdef RAP_SCM_SUPPORT
#ifdef RAP_GFX_SUPPORT
		|| if(eInterrupt != BRAP_Interrupt_eScmResponse )	
#else
		if(eInterrupt != BRAP_Interrupt_eScmResponse )  

#endif
#endif
    {
    ret = BRAP_P_UnmaskInterrupt(hRapCh, eInterrupt);
    }

    if (BRAP_Interrupt_eFmmRbufFreeByte == eInterrupt)
    {
        ui32IntMask = (BCHP_FIELD_DATA (AIO_INTH_R5F_MASK_SET, FMM_BF2, 1));
        hRapCh->ui32FmmIntMask |= ui32IntMask;
    }

    BDBG_LEAVE (BRAP_InstallAppInterruptCb);
    return ret;
}

/*****************************************************************************
Summary:
    Used to remove the application callback.

Description:
    This PI should be used by the application to remove an application specified 
    callback that has been install in addition to the RAptor PI interrupt handler 
    for that interrupt. 

Returns:
    BERR_SUCCESS on success
    Error code on failure
******************************************************************************/
BERR_Code BRAP_RemoveAppInterruptCb (
	BRAP_ChannelHandle hRapCh,	/* [in] The RAP channel handle */
    BRAP_Interrupt  eInterrupt  /* [in] The interrupt that needs to be deactivated */
)
{
    BERR_Code ret = BERR_SUCCESS;
    uint32_t ui32IntMask = 0;

    BDBG_ENTER (BRAP_RemoveAppInterruptCb);

    /* Check input parameters */
    BDBG_ASSERT (hRapCh);     

	hRapCh->sAppIntCbInfo[eInterrupt].pfAppCb = NULL;
	hRapCh->sAppIntCbInfo[eInterrupt].pParm1  = NULL;
	hRapCh->sAppIntCbInfo[eInterrupt].iParm2  = 0;

    BDBG_MSG(("BRAP_RemoveAppInterruptCb: pfAppCb=%d", \
		hRapCh->sAppIntCbInfo[eInterrupt].pfAppCb));
    BDBG_MSG(("BRAP_RemoveAppInterruptCb: pParm1=%d", \
		hRapCh->sAppIntCbInfo[eInterrupt].pParm1));
    BDBG_MSG(("BRAP_RemoveAppInterruptCb: iParm2=%d", \
		hRapCh->sAppIntCbInfo[eInterrupt].iParm2));

#ifdef RAP_GFX_SUPPORT
    if(eInterrupt != BRAP_Interrupt_eGfxOperationComplete)
#endif        
#ifdef RAP_SCM_SUPPORT
#ifdef RAP_GFX_SUPPORT
		|| if(eInterrupt != BRAP_Interrupt_eScmResponse )	
#else
		 if(eInterrupt != BRAP_Interrupt_eScmResponse )  

#endif
#endif        
    {    
    ret = BRAP_P_MaskInterrupt(hRapCh, eInterrupt);
    }


    if (BRAP_Interrupt_eFmmRbufFreeByte == eInterrupt)
    {
        ui32IntMask = ~(BCHP_FIELD_DATA (AIO_INTH_R5F_MASK_CLEAR, FMM_BF2, 1));
        hRapCh->ui32FmmIntMask &= ui32IntMask;
    }

    BDBG_LEAVE (BRAP_RemoveAppInterruptCb);
    return ret;
}

/*****************************************************************************
Summary:
   Install application callback function for destination level interrupts
   
Description:
   This API is used to install application callback function for destination
   level Raptor interrupts. Application callback function is called when
   these interrupts are triggered.

Returns:
    BERR_SUCCESS on success
     Error code on failure
******************************************************************************/
BERR_Code BRAP_InstallDestinationLevelAppInterruptCb (
	BRAP_DestinationHandle        hDstHandle,	   /* [in] The RAP destination handle */
    BRAP_DestinationInterrupt     eInterrupt, /* [in] The interrupt that needs to 
                                      be activated */
    BRAP_CallbackFunc             pfAppInterruptCb, /* [in] The RAP callback function 
                                      to be registered for eInterrupt*/ 
    void                         *pParm1,    /* [in] The application specified parameter 
                                      that needs to be passed unchanged to the 
                                      callback */
    int                           iParm2      /* [in] The application specified parameter 
                                      that needs to be passed unchanged to the 
                                      callback */
)
{
    BERR_Code ret = BERR_SUCCESS;
    uint32_t ui32IntMask = 0;

    BDBG_ENTER (BRAP_InstallDestinationLevelAppInterruptCb);

    /* Check input parameters */
    BDBG_ASSERT(hDstHandle);
    BDBG_ASSERT(pfAppInterruptCb);

    
	hDstHandle->sAppIntCbInfo[eInterrupt].pfAppCb = pfAppInterruptCb;
	hDstHandle->sAppIntCbInfo[eInterrupt].pParm1  = pParm1;
	hDstHandle->sAppIntCbInfo[eInterrupt].iParm2  = iParm2;

    BDBG_MSG(("BRAP_InstallDestinationLevelAppInterruptCb: pfAppCb=%d", \
		hDstHandle->sAppIntCbInfo[eInterrupt].pfAppCb));
    BDBG_MSG(("BRAP_InstallDestinationLevelAppInterruptCb: pParm1=%d", \
		hDstHandle->sAppIntCbInfo[eInterrupt].pParm1));
    BDBG_MSG(("BRAP_InstallDestinationLevelAppInterruptCb: iParm2=%d", \
		hDstHandle->sAppIntCbInfo[eInterrupt].iParm2));

    ret = BRAP_P_UnmaskDestinationInterrupt(hDstHandle, eInterrupt);

#if (BRAP_7550_FAMILY !=1)   
    ui32IntMask = (BCHP_FIELD_DATA (AIO_INTH_R5F_MASK_SET, FMM_BF1, 1));
    hDstHandle->ui32FmmIntMask |= ui32IntMask;
    ui32IntMask = (BCHP_FIELD_DATA (AIO_INTH_R5F_MASK_SET, FMM_BF2, 1));
    hDstHandle->ui32FmmIntMask |= ui32IntMask;
#else
    BSTD_UNUSED(ui32IntMask);
#endif

    BDBG_LEAVE (BRAP_InstallDestinationLevelAppInterruptCb);
    return ret;
}

/*****************************************************************************
Summary:
   Remove application callback function for destination interrupts

Description:
   This API is used to remove application callback function of destination interrupts. 

Returns:
    BERR_SUCCESS on success
     Error code on failure
******************************************************************************/
BERR_Code BRAP_RemoveDestinationLevelAppInterruptCb (
   BRAP_DestinationHandle          hDstHandle, /* [in] Destination Handle */
   BRAP_DestinationInterrupt       eInterrupt  /* [in] The destination level interrupt 
            that needs to be removed */
    )
{
    BERR_Code ret = BERR_SUCCESS;
    uint32_t ui32IntMask = 0;

    BDBG_ENTER (BRAP_RemoveDestinationLevelAppInterruptCb);

    /* Check input parameters */
    BDBG_ASSERT (hDstHandle);
    
	hDstHandle->sAppIntCbInfo[eInterrupt].pfAppCb = NULL;
	hDstHandle->sAppIntCbInfo[eInterrupt].pParm1  = NULL;
	hDstHandle->sAppIntCbInfo[eInterrupt].iParm2  = 0;

    BDBG_MSG(("BRAP_RemoveAppInterruptCb: pfAppCb=%d", \
		hDstHandle->sAppIntCbInfo[eInterrupt].pfAppCb));
    BDBG_MSG(("BRAP_RemoveAppInterruptCb: pParm1=%d", \
		hDstHandle->sAppIntCbInfo[eInterrupt].pParm1));
    BDBG_MSG(("BRAP_RemoveAppInterruptCb: iParm2=%d", \
		hDstHandle->sAppIntCbInfo[eInterrupt].iParm2));

    ret = BRAP_P_MaskDestinationInterrupt(hDstHandle, eInterrupt);

#if (BRAP_7550_FAMILY !=1)       
    BDBG_MSG(("Remove App Interrupt Cb: ui32IntMask = 0x%x, FmmIntMask = 0x%x", \
    ui32IntMask, hDstHandle->ui32FmmIntMask));

    ui32IntMask = ~(BCHP_FIELD_DATA (AIO_INTH_R5F_MASK_CLEAR, FMM_BF1, 1));
    hDstHandle->ui32FmmIntMask &= ui32IntMask;
    ui32IntMask = ~(BCHP_FIELD_DATA (AIO_INTH_R5F_MASK_CLEAR, FMM_BF2, 1));
    hDstHandle->ui32FmmIntMask &= ui32IntMask;
#else
    BSTD_UNUSED(ui32IntMask);
#endif
    
    BDBG_LEAVE (BRAP_RemoveDestinationLevelAppInterruptCb);
    return ret;
}

#if ( (BRAP_3548_FAMILY == 1) )

/**********************************************************************
Summary:
   Get the default input detection settings

Description:
   This function returns the default input settins that can be used for the enable
   spdifrx input detection fucntion.

Returns:
   BERR_SUCCESS - If successful

See Also:
    BRAP_EnableSpdifRxInputDetection
**********************************************************************/
BERR_Code BRAP_GetDefaultSpdifRxInputDetectionSettings(
    BRAP_SpdifRx_InputDetectionSettings *pDetectionSettings     /* [out] Default Settings */
    )
{
	BERR_Code ret = BERR_SUCCESS;

	BDBG_ENTER(BRAP_GetDefaultSpdifRxInputDetectionSettings);

	BDBG_ASSERT(pDetectionSettings);

    *pDetectionSettings = sDefSpdifRxInputDetectionSettings;

    BDBG_LEAVE(BRAP_GetDefaultSpdifRxInputDetectionSettings);
	return ret; 
}


/**********************************************************************
Summary:
   Opens SPDIF Rx for Input Detection

Description:
   This function opens  the SPDIF Receiver for the input detection based on the 
   capture input port (spdif/hdmi).

Returns:
   BERR_SUCCESS - If successful

See Also:
    BRAP_GetDefaultSpdifRxInputDetectionSettings
    BRAP_DisableSpdifRxInputDetection
**********************************************************************/
BERR_Code BRAP_EnableSpdifRxInputDetection (
    BRAP_Handle                                 hRap,     /* [in] The RAP handle */
    const BRAP_SpdifRx_InputDetectionSettings   *pDetectionSettings    /* [in] Detection Setttings */
)
{
	BERR_Code ret = BERR_SUCCESS;

	BDBG_ENTER(BRAP_EnableSpdifRxInputDetection);

	BDBG_ASSERT(hRap);
	BDBG_ASSERT(pDetectionSettings);

    /* Open the SPDIF RX */
    BRAP_SPDIFRX_P_Open (hRap, pDetectionSettings->eInputPort);

    hRap->eCapInputPort = pDetectionSettings->eInputPort;   
    
    BDBG_LEAVE(BRAP_EnableSpdifRxInputDetection);
	return ret;    
}

/**********************************************************************
Summary:
   Closes SPDIF Rx for Input Detection

Description:
   This function closes  the SPDIF Receiver for the input detection

Returns:
   BERR_SUCCESS - If successful

See Also:
    BRAP_EnableSpdifRxInputDetection
**********************************************************************/
BERR_Code BRAP_DisableSpdifRxInputDetection (
    BRAP_Handle                                 hRap    /* [in] The RAP handle */
)
{
	BERR_Code ret = BERR_SUCCESS;

	BDBG_ENTER(BRAP_DisableSpdifRxInputDetection);

	BDBG_ASSERT(hRap);

    /* Close the SPDIF RX */
    BRAP_SPDIFRX_P_Close (hRap);

    hRap->eCapInputPort = BRAP_CapInputPort_eMax;   
    
    BDBG_LEAVE(BRAP_DisableSpdifRxInputDetection);
	return ret;    
}

/**********************************************************************
Summary:
   Gives the current status of SPDIF Rx

Description:
   This function returns the current status of the SPDIF RX block with 
   respect to the given raptor channel.

Returns:
   BERR_SUCCESS - If successful

See Also:
**********************************************************************/
BERR_Code BRAP_GetSpdifRxStatus (
    BRAP_Handle                 hRap,       /* [in] The RAP handle */
    BRAP_SpdifRx_Status         *pStatus    /* [out] Current status of 
                                                 SPDIF input channel */
)
{
	BERR_Code ret = BERR_SUCCESS;

	BDBG_ENTER(BRAP_GetSpdifRxStatus);

	BDBG_ASSERT(hRap);
	BDBG_ASSERT(pStatus);

    /* Get the SPDIF Receiver Status */
    ret = BRAP_SPDIFRX_P_GetRxStatus (hRap, pStatus);

    BDBG_LEAVE(BRAP_GetSpdifRxStatus);
	return ret;    
}

/**********************************************************************
Summary:
   Gives the channel status bits of the input stream.

Description:
   This function returns the current status bits available
in the input stream. Gives both the left and right channel status bits.

Returns:
   BERR_SUCCESS - If successful

See Also:
**********************************************************************/
BERR_Code BRAP_GetSpdifRxChannelStatus ( 
    BRAP_Handle                 hRap,           /* [in] The RAP handle */
    BRAP_SpdifRx_ChannelStatus  *pChannelStatus /* [out] Current channel status */
)
{
    BERR_Code ret = BERR_SUCCESS;

	BDBG_ENTER(BRAP_GetSpdifRxChannelStatus);
	BDBG_ASSERT(hRap);
	BDBG_ASSERT(pChannelStatus);

  
    ret = BRAP_SPDIFRX_P_GetChannelStatus (hRap, pChannelStatus);

 	BDBG_LEAVE(BRAP_GetSpdifRxChannelStatus);
	return ret;  
}

/***************************************************************************
Summary:
	Mute / UnMute an external capture port
	
Description:
    This function mutes or unmutes the input port.
    
Returns:
    BERR_SUCCESS if successful else error value.
See Also:
    BRAP_SetOutputMute
****************************************************************************/
BERR_Code BRAP_SetInputMute (
	BRAP_Handle		            hRap,		    /* [in] RAP device handle */
	BRAP_CapInputPort           eCapPort,       /* [in] Input port */
	bool                        bMute           /* [in] TRUE: Mute input FALSE: UnMute */
	)
{
    BERR_Code ret = BERR_SUCCESS;
    
 	BDBG_ENTER(BRAP_SetInputMute);
	BDBG_ASSERT(hRap);
    BKNI_EnterCriticalSection();
    ret = BRAP_SetInputMute_isr (hRap, eCapPort, bMute);
    BKNI_LeaveCriticalSection();
 	BDBG_LEAVE(BRAP_SetInputMute);
    
    return ret;
}


/***************************************************************************
Summary:
	Mute / UnMute an external capture port (isr version)
	
Description:
    This function mutes or unmutes the input port.
    
Returns:
    BERR_SUCCESS if successful else error value.
See Also:
    BRAP_SetOutputMute
****************************************************************************/
BERR_Code BRAP_SetInputMute_isr (
	BRAP_Handle		            hRap,		    /* [in] RAP device handle */
	BRAP_CapInputPort           eCapPort,       /* [in] Input port */
	bool                        bMute           /* [in] TRUE: Mute input FALSE: UnMute */
	)
{
    BERR_Code ret = BERR_SUCCESS;
    
 	BDBG_ENTER(BRAP_SetInputMute_isr);
	BDBG_ASSERT(hRap);   

    switch (eCapPort)
    {
        case BRAP_CapInputPort_eSpdif:
        case BRAP_CapInputPort_eHdmi:
            if (bMute)
            {
                ret = BRAP_SPDIFRX_P_Stop (hRap);
            }
            else
            {
                ret = BRAP_SPDIFRX_P_Start (hRap);
            }
            break;

        /* Other input capture ports might have to be handled here */            

        default:
            BDBG_ERR (("BRAP_SetInputMute_isr(): Capture Input type %d not supported by this PI yet ",
                        eCapPort));
            return BERR_TRACE (BERR_NOT_SUPPORTED);            
    }

 	BDBG_LEAVE(BRAP_SetInputMute_isr);

    return ret;
}

#endif

/*****************************************************************************
Summary:
	Install application callback function for device level interrupts
	
Description:
	This API is used to install application callback function for device
	level Raptor interrupts. Application callback function is called when
	these interrupts are triggered.

Returns:
    BERR_SUCCESS on success
     Error code on failure
******************************************************************************/
BERR_Code BRAP_InstallDeviceLevelAppInterruptCb (
	BRAP_Handle hRap,	   /* [in] The RAP handle */
    BRAP_DeviceLevelInterrupt     eInterrupt, /* [in] The device level interrupt
										that needs to be activated */
    BRAP_CallbackFunc  pfAppInterruptCb, /* [in] The RAP callback function 
                                      to be registered for eInterrupt*/ 
    void               *pParm1,    /* [in] The application specified parameter 
                                      that needs to be passed unchanged to the 
                                      callback */
    int                iParm2      /* [in] The application specified parameter 
                                      that needs to be passed unchanged to the 
                                      callback */
    )
{
	BERR_Code ret = BERR_SUCCESS;
    unsigned int i = 0;

	BDBG_ENTER(BRAP_InstallDeviceLevelAppInterruptCb);

	/* Assert on bad input parameters */
	BDBG_ASSERT(hRap);
    BDBG_ASSERT(pfAppInterruptCb);

	switch (eInterrupt) 
    {
		case BRAP_DeviceLevelInterrupt_eWatchdog:
            for(i = 0; i < BRAP_RM_P_MAX_DSPS; i++)
            {                
                if(!(BRAP_P_IsPointerValid((void *)hRap->hDsp[i])))
                    continue;

    			ret = BRAP_DSP_P_EnableDspWatchdogTimer(hRap->hDsp[i],true);
    			if (ret!=BERR_SUCCESS)
    				return BERR_TRACE(ret);
            }
            
			ret = BINT_EnableCallback (hRap->hCallback);
			if(ret!=BERR_SUCCESS)
                return BERR_TRACE(ret);

			break;
#if (BRAP_3548_FAMILY == 1)
        case BRAP_DeviceLevelInterrupt_eRfAudio:
            break;

        case BRAP_DeviceLevelInterrupt_eSpdifRx:
            break;
#endif       

		default:
			BDBG_ERR(("Interrupt type %d is not a device level interrupt",eInterrupt));
			return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	hRap->sAppIntCbInfo[eInterrupt].pfAppCb = pfAppInterruptCb;
	hRap->sAppIntCbInfo[eInterrupt].pParm1  = pParm1;
	hRap->sAppIntCbInfo[eInterrupt].iParm2  = iParm2;
    
	BDBG_MSG(("BRAP_InstallDeviceLevelAppInterruptCb: pfAppCb=%d",
		hRap->sAppIntCbInfo[eInterrupt].pfAppCb));
    BDBG_MSG(("BRAP_InstallDeviceLevelAppInterruptCb: pParm1=%d",
		hRap->sAppIntCbInfo[eInterrupt].pParm1));
    BDBG_MSG(("BRAP_InstallDeviceLevelAppInterruptCb: iParm2=%d",
		hRap->sAppIntCbInfo[eInterrupt].iParm2));

	BDBG_LEAVE(BRAP_InstallDeviceLevelAppInterruptCb);
	return ret;
}

/*****************************************************************************
Summary:
	Remove application callback function for device level interrupts

Description:
	This API is used to remove application callback function for device
	level Raptor interrupts. 

Returns:
    BERR_SUCCESS on success
     Error code on failure
******************************************************************************/
BERR_Code BRAP_RemoveDeviceLevelAppInterruptCb (
	BRAP_Handle hRap,	/* [in] The RAP channel handle */
    BRAP_DeviceLevelInterrupt  eInterrupt  /* [in] The device level interrupt 
								that needs to be removed */
    )
{
	BERR_Code ret = BERR_SUCCESS;
    unsigned int i = 0;

	BDBG_ENTER(BRAP_RemoveDeviceLevelAppInterruptCb);

	/* Assert on bad input parameters */
	BDBG_ASSERT(hRap);

	switch (eInterrupt) 
    {
		case BRAP_DeviceLevelInterrupt_eWatchdog:
            for(i = 0; i < BRAP_RM_P_MAX_DSPS; i++)
            {                
                if(!(BRAP_P_IsPointerValid((void *)hRap->hDsp[i])))
                    continue;
          
    			ret = BRAP_DSP_P_EnableDspWatchdogTimer(hRap->hDsp[i],false);
    			if (ret!=BERR_SUCCESS)
    				return BERR_TRACE(ret);
            }/* for i */
            
			ret = BINT_DisableCallback (hRap->hCallback);
			if (ret!=BERR_SUCCESS)
				return BERR_TRACE(ret);

			break;
        
#if (BRAP_3548_FAMILY == 1)
        case BRAP_DeviceLevelInterrupt_eRfAudio:
            break;

        case BRAP_DeviceLevelInterrupt_eSpdifRx:
            break;
#endif       


		default:
			BDBG_ERR(("Interrupt type %d not supported",eInterrupt));
			return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	hRap->sAppIntCbInfo[eInterrupt].pfAppCb = NULL;
	hRap->sAppIntCbInfo[eInterrupt].pParm1  = NULL;
	hRap->sAppIntCbInfo[eInterrupt].iParm2  = 0;

    BDBG_MSG(("BRAP_RemoveAppInterruptCb: pfAppCb=%d", 
		hRap->sAppIntCbInfo[eInterrupt].pfAppCb));
    BDBG_MSG(("BRAP_RemoveAppInterruptCb: pParm1=%d", 
		hRap->sAppIntCbInfo[eInterrupt].pParm1));
    BDBG_MSG(("BRAP_RemoveAppInterruptCb: iParm2=%d", 
		hRap->sAppIntCbInfo[eInterrupt].iParm2));    

	BDBG_LEAVE(BRAP_RemoveDeviceLevelAppInterruptCb);
	return ret;
}

/***************************************************************************
Summary:
	This API flushes CDB/ITB buffers associated with a decode channel for 7411 only.
	And for other chips this fucntion clears the itnernal buffers owned by PI and FW.

Description:
	Along with CDB/ITB buffers(7411), various buffers present in the decode pipeline also get flushed.
	BRAP_DisableForFlush() API should be called before calling BRAP_FlushChannel() API.

Returns:
	BERR_SUCCESS on success
	Error value on failure

See Also:
    BRAP_DisableForFlush
****************************************************************************/
BERR_Code BRAP_FlushChannel (
	BRAP_ChannelHandle	        hRapCh		/* [in] The Decode Channel handle */
	)
{
	BERR_Code           err = BERR_SUCCESS;
    BRAP_ChannelParams  *psAudioParams = NULL;

	BDBG_ENTER(BRAP_FlushChannel);

	/* Assert on invalid input parameters */
	BDBG_ASSERT(hRapCh);

    if(hRapCh->eChannelType != BRAP_ChannelType_eDecode)
    {
        BDBG_ERR(("BRAP_FlushChannel: ChType (%d) can't be flushed. "
            "Only decode ChType(%d) can be flushed",
            hRapCh->eChannelType, BRAP_ChannelType_eDecode));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    psAudioParams = (BRAP_ChannelParams *)BKNI_Malloc(sizeof(BRAP_ChannelParams));
    if(NULL == psAudioParams)
    {
        err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
        goto end;        
    }        

    BKNI_Memset(psAudioParams, 0, sizeof(BRAP_ChannelParams));

	/* Set "Internal Call" flag in the channel handle. Some of the APIs
	 * behave differently for internal Vs external calls. For example,
	 * in BRAP_StartChannel, trick mode status is reset only on external
	 * calls. Also, FMM resources are not opened during channel start if 
	 * called internally.
	 */
	BKNI_EnterCriticalSection();
	hRapCh->bInternalCallFromRap = true;
	BKNI_LeaveCriticalSection();

	/* Get current audio parameters for this channel */
    err = BRAP_DEC_P_GetCurrentAudioParams(hRapCh, psAudioParams);
	if (err != BERR_SUCCESS)
    {
        err = BERR_TRACE(err);
        goto end;        
    }


	/* Start DEC channel. Since this is an internal call, all the modules
	 * need to use audioparams stored in their respective handles 
	 */
	if(BRAP_P_State_eStarted != hRapCh->eState)
    {   
        err = BRAP_StartChannel(hRapCh, psAudioParams);
        if (err != BERR_SUCCESS)
        {
            err = BERR_TRACE(err);
            goto end;        
        }        
    }
    else
    {
	 	BDBG_ERR(("Please call BRAP_DisableForFlush function before calling BRAP_FlushChannel function"));
    }

	/* Restore the trick modes */
    if (hRapCh->sTrickModeState.bAudioPaused==true) 
    {
        err = BRAP_EnablePvrPause(hRapCh, true);
        if (err != BERR_SUCCESS)
        {
            err = BERR_TRACE(err);
            goto end;        
        }
        /* Reset the frame advance residual time */
        hRapCh->sTrickModeState.uiFrameAdvResidualTime = 0;
    }


	/* Reset "Internal Call" flag in the channel handle */
	BKNI_EnterCriticalSection();
	hRapCh->bInternalCallFromRap = false;
	BKNI_LeaveCriticalSection();

    end:
    if(psAudioParams)
    BKNI_Free(psAudioParams);
	BDBG_LEAVE(BRAP_FlushChannel);
	return err;
}

/***************************************************************************
Summary:
	This API is used to inform the decoder to stop reading data from RAVE in
	preparation for flush. This API should be called before calling 
	BRAP_FlushChannel().
	
Description:
	This API just stops the decoder channel from reading data from CDB/ITB.
	This API should be called before calling BRAP_FlushChannel() PI. This API is
	valid only for a decode channel.

Returns:
	BERR_SUCCESS on success
	Error value on failure

See Also:
    BRAP_FlushChannel
****************************************************************************/
BERR_Code  BRAP_DisableForFlush ( 
	BRAP_ChannelHandle	hRapCh		/* [in] The RAP Decode Channel handle */
	)
{
	BERR_Code err = BERR_SUCCESS;

	BDBG_ENTER(BRAP_DisableForFlush);

	/* Assert on invalid input parameters */
	BDBG_ASSERT(hRapCh);

    if(hRapCh->eChannelType != BRAP_ChannelType_eDecode)
    {
        BDBG_ERR(("BRAP_DisableForFlush: Not supported for ChType(%d)"
            "Only decode ChType(%d) can be flushed",
            hRapCh->eChannelType, BRAP_ChannelType_eDecode));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

	/* Set "Internal Call" flag in the channel handle. Some of the APIs
	 * behave differently for internal Vs external calls. For example,
	 * in BRAP_DEC_Start, trick mode status is reset only on external
	 * calls. 
	 */
	BKNI_EnterCriticalSection();
	hRapCh->bInternalCallFromRap = true;
	BKNI_LeaveCriticalSection();

	/* Stop DEC channel */
    err = BRAP_StopChannel(hRapCh);
    if (err != BERR_SUCCESS)
    {
        return BERR_TRACE(err);
    }

	/* Reset "Internal Call" flag in the channel handle */
	BKNI_EnterCriticalSection();
	hRapCh->bInternalCallFromRap = false;
	BKNI_LeaveCriticalSection();

	BDBG_LEAVE(BRAP_DisableForFlush);
	return err;
}

/***************************************************************************
Summary:
	Sets the Static Downmixing coefficients to be used in the system.
	
Description:
    
Returns:

See Also:
    BRAP_DownMixingCoef
****************************************************************************/
BERR_Code BRAP_SetDownMixCoef(
    BRAP_Handle            		hRap,
    BRAP_DownMixingCoef        	*pDnMixingCoeff
)
{
    BERR_Code   ret = BERR_SUCCESS;
    int         i = 0;
    unsigned int        uiGrpId = 0, uiPth=0;
    bool                bGrpActive = true;
    BRAP_ChannelHandle  hRapCh = NULL;

    BDBG_ENTER(BRAP_SetDownMixCoef);

    BDBG_ASSERT(hRap);
    BDBG_ASSERT(pDnMixingCoeff);

    for(i =0 ; i<BRAP_P_MAX_DOWN_MIX_COEFF; i++)
    {
        if((hRap->sDnMixCoeff[i].eInputMode == pDnMixingCoeff->eInputMode)&&
           (hRap->sDnMixCoeff[i].eOutputMode == pDnMixingCoeff->eOutputMode)&&
           (hRap->sDnMixCoeff[i].bInputLfePresent == pDnMixingCoeff->bInputLfePresent)&&
           (hRap->sDnMixCoeff[i].bOutputLfePresent== pDnMixingCoeff->bOutputLfePresent))
        {
            hRap->sDnMixCoeff[i] = *pDnMixingCoeff;
            break;
        }
        else if(BRAP_OutputMode_eLast == hRap->sDnMixCoeff[i].eInputMode)
        {
            hRap->sDnMixCoeff[i] = *pDnMixingCoeff;
            break;
        }
    }

    if( BRAP_P_MAX_DOWN_MIX_COEFF == i)
    {
        BDBG_ERR(("BRAP_SetDownMixCoef: Could not store the DownMix Coeff"
                  "No free slot to store the coeff"));
        ret = BERR_NOT_SUPPORTED;
    }
    
    /* Find the groups that are active */
    for(uiGrpId=0; uiGrpId < BRAP_MAX_ASSOCIATED_GROUPS; uiGrpId++)
    {
        bGrpActive = false;
        for(i=0; i<BRAP_MAX_PRI_DEC_CHAN_IN_ASSOCIATED_GRP; i++)
        {   
            if((BRAP_P_IsPointerValid((void *)hRap->sAssociatedCh[uiGrpId].hPriDecCh[i]))&&
               (BRAP_P_State_eStarted == hRap->sAssociatedCh[uiGrpId].hPriDecCh[i]->eState))
            {
                /* This group is in use check the next group */
                bGrpActive = true;
                hRapCh = hRap->sAssociatedCh[uiGrpId].hPriDecCh[i];
                break;
            }
        }
        if(true == bGrpActive)
        {
            break;
        }

        for(i=0; i<BRAP_MAX_SEC_DEC_CHAN_IN_ASSOCIATED_GRP; i++)
        {   
            if((BRAP_P_IsPointerValid((void *)hRap->sAssociatedCh[uiGrpId].hSecDecCh[i] ))&&
               (BRAP_P_State_eStarted == hRap->sAssociatedCh[uiGrpId].hSecDecCh[i]->eState))
            {
                /* This group is in use check the next group */
                bGrpActive = true;
                hRapCh = hRap->sAssociatedCh[uiGrpId].hSecDecCh[i];
                break;
            }
        }
        if(true == bGrpActive)
        {
            break;
        }

        for(i=0; i<BRAP_MAX_PB_CHAN_IN_ASSOCIATED_GRP; i++)
        {   
            if((BRAP_P_IsPointerValid((void *)hRap->sAssociatedCh[uiGrpId].hPBCh[i] ))&&
               (BRAP_P_State_eStarted == hRap->sAssociatedCh[uiGrpId].hPBCh[i]->eState))
            {
                /* This group is in use check the next group */
                bGrpActive = true;
                hRapCh = hRap->sAssociatedCh[uiGrpId].hPBCh[i];
                break;
            }
        }
        if(true == bGrpActive)
        {
            break;
        }

        for(i=0; i<BRAP_MAX_CAP_CHAN_IN_ASSOCIATED_GRP; i++)
        {   
            if((BRAP_P_IsPointerValid((void *)hRap->sAssociatedCh[uiGrpId].hCapCh[i] ))&&
               (BRAP_P_State_eStarted == hRap->sAssociatedCh[uiGrpId].hCapCh[i]->eState))
            {
                /* This group is in use check the next group */
                bGrpActive = true;
                hRapCh = hRap->sAssociatedCh[uiGrpId].hCapCh[i];
                break;
            }
        }
        if(true == bGrpActive)
        {
            break;
        }
    }

    /* If a channel is active, Find the mixing level and Path where downmixing 
       is happening and apply the new coeff */
    if( (BRAP_P_IsPointerValid((void *)hRapCh)) && ((BRAP_ChannelType_eDecode == hRapCh->eChannelType)||
		 (BRAP_ChannelType_ePcmPlayback == hRapCh->eChannelType))&&
       (true == bGrpActive))
    {
        for(uiPth=0;uiPth<BRAP_P_MAX_PATHS_IN_A_CHAN;uiPth++)
        {
            if(!(BRAP_P_IsPointerValid((void *)hRapCh->pPath[uiPth])))
            {
                continue;
            }
            
            ret = BRAP_P_ProgramDownMixCoefficients(hRapCh,uiPth);
            if(BERR_SUCCESS != ret)
            {
            	BDBG_ERR(("BRAP_SetDownMixCoef: BRAP_P_ProgramDownMixCoefficients failed %d",
                    ret));
            	return BERR_TRACE(ret); 
            }
        }
    }
    
    BDBG_LEAVE(BRAP_SetDownMixCoef);
    return ret;
}

/***************************************************************************
Summary:
	Process Raptor watchdog interrupt

Description:
	This function processes Raptor watchdog interrupt. This is not _isr safe
	function and should be called only from task context. Call to this function
	should be treated as critical section code.

Returns:
	BERR_SUCCESS 

See Also:
	None
****************************************************************************/
BERR_Code BRAP_ProcessWatchdogInterrupt(
     BRAP_Handle 			hRap		/* [in] The RAP device handle */
    )
{
	unsigned int	uiDecChIndex = 0;
    BRAP_OutputPort eOp = BRAP_OutputPort_eMax;
    unsigned int    uiStrmIdx = 0;
  	unsigned int	uiPCMPbIndex = 0;
  	unsigned int	uiPCMCapIndex = 0;
#ifdef 	RAP_GFX_SUPPORT
	unsigned int	uiGfxIndex=0;
#endif
        unsigned int i =0,j=0;

	BERR_Code       ret = BERR_SUCCESS;
#if BRAP_P_USE_BRAP_TRANS ==1
	BRAP_TRANS_ChannelHandle	hRapTransCh;
	bool bTransChStarted = false;
#endif

    BRAP_ChannelSettings    *psChanSettings = NULL;
    BRAP_ChannelParams     *psChanParams = NULL;
#ifdef 	RAP_GFX_SUPPORT
    BRAP_GFX_OpenSettings    sGfxChanSettings;
#endif                 
	BDBG_ENTER(BRAP_ProcessWatchdogInterrupt);
	
    BKNI_EnterCriticalSection();
    /* BRAP_ProcessWatchdogInterrupt() should not be called from within 
    Critical sections. To ensure this, added these few lines of code. 
    Do nothing here. If BRAP_ProcessWatchdogInterrupt is called incorrectly from 
    a critical section, this piece of code will cause it to deadlock and exit */
    BKNI_LeaveCriticalSection();
    
	/* Assert the function arguments*/
	BDBG_ASSERT(hRap);

	/* Set the watchdog recovery flag */
	hRap->bWatchdogRecoveryOn = true;
    for (i = 0; i < BRAP_RM_P_MAX_DSPS; i++)
    {
        for (j = 0; j < BRAP_RM_P_MAX_FW_TASK_PER_DSP; j++)
        {
            hRap->hFwTask[i][j]->bStopped= true;           
            hRap->hFwTask[i][j]->uiLastEventType= 0xFFFF;                                  
        }
    }
       hRap->bWatchdogTriggered = false;        
    psChanSettings = (BRAP_ChannelSettings *)BKNI_Malloc(sizeof(BRAP_ChannelSettings));
    if( NULL==psChanSettings)
    {
        ret = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
        goto error;        
    }
       BKNI_Memset(psChanSettings,0,sizeof( BRAP_ChannelSettings ));    	            
    psChanParams = (BRAP_ChannelParams *)BKNI_Malloc(sizeof(BRAP_ChannelParams));
    if( NULL==psChanParams)
    {
        ret = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
        goto error;
    }
       BKNI_Memset(psChanParams,0,sizeof( BRAP_ChannelParams ));    	
	/* Reopen DSP and FMM devices */
	ret = BRAP_P_Open(hRap);
	if (ret!=BERR_SUCCESS)
	{
        ret = BERR_TRACE(ret);
        goto error;
    }

    /* Reeinitialize common interrupt callbacks */
    ret = BRAP_P_DeviceLevelInterruptInstall(hRap);
    if(ret != BERR_SUCCESS)
    {
        ret = BERR_TRACE(ret);
        goto error;
    }

#if (BRAP_3548_FAMILY == 1)
	/* Reopen SPDIF Rx */
	ret = BRAP_SPDIFRX_P_Open (hRap, hRap->eCapInputPort);
	if (ret!=BERR_SUCCESS)
	{
        ret = BERR_TRACE(ret);
        goto error;
    }
#endif    

    /* Open all the already opened SPDIFFM */
    for(uiStrmIdx=0; uiStrmIdx<BRAP_RM_P_MAX_SPDIFFM_STREAMS; uiStrmIdx++)
    {        
        if(BRAP_P_IsPointerValid((void *)hRap->hFmm[0]->hSpdifFm[uiStrmIdx]))
        {
            BDBG_MSG(("BRAP_ProcessWatchdogInterrupt: Opening SpdifFm Strm = %d"
                ,uiStrmIdx));
            
            ret = BRAP_SPDIFFM_P_Open(hRap->hFmm[0],
            		 &(hRap->hFmm[0]->hSpdifFm[uiStrmIdx]),
            		 uiStrmIdx,
            		 NULL);
            if(ret != BERR_SUCCESS)
            {
                ret =  BERR_TRACE(ret);
                goto error;
            }    
        }
    }/* for uiStrmIdx */

    /* Open all the already opened output ports */
    for(eOp=0; eOp<BRAP_OutputPort_eMax; eOp++)
    {        
        if(BRAP_P_IsPointerValid((void *)hRap->hFmm[0]->hOp[eOp]))
        {
            BDBG_MSG(("BRAP_ProcessWatchdogInterrupt: Opening eOp = %d", eOp));

            ret = BRAP_OP_P_Open(hRap->hFmm[0],
            		 &(hRap->hFmm[0]->hOp[eOp]),
            		 eOp,
            		 NULL);
            if(ret != BERR_SUCCESS)
            {
                ret = BERR_TRACE(ret);
                goto error;
            }    
        }
    }/* for eOp */

   for(uiDecChIndex = 0; uiDecChIndex < BRAP_RM_P_MAX_DEC_CHANNELS; uiDecChIndex++) 
    {
        if(hRap->hRapDecCh[uiDecChIndex])             
        { 
            if((BRAP_P_State_eOpened == hRap->hRapDecCh[uiDecChIndex]->eState)
                || (BRAP_P_State_eStarted== hRap->hRapDecCh[uiDecChIndex]->eState))
            {
                BRAP_P_InterruptUnInstall(hRap->hRapDecCh[uiDecChIndex]);
            }
        }
    }

    for(uiPCMPbIndex = 0; uiPCMPbIndex < BRAP_RM_P_MAX_PCM_CHANNELS; uiPCMPbIndex++) 
    {
        if(hRap->hRapPbCh[uiPCMPbIndex]) 
        { 
            if((BRAP_P_State_eOpened == hRap->hRapPbCh[uiPCMPbIndex]->eState)
                || (BRAP_P_State_eStarted== hRap->hRapPbCh[uiPCMPbIndex]->eState))
            {
                BRAP_P_InterruptUnInstall(hRap->hRapPbCh[uiPCMPbIndex]);
            }
        }
    }
    
    for(uiPCMCapIndex= 0; uiPCMCapIndex < BRAP_RM_P_MAX_CAP_CHANNELS; uiPCMCapIndex++) 
    {
        if(hRap->hRapCapCh[uiPCMCapIndex]) 
        { 
            if((BRAP_P_State_eOpened == hRap->hRapCapCh[uiPCMCapIndex]->eState)
                || (BRAP_P_State_eStarted== hRap->hRapCapCh[uiPCMCapIndex]->eState))
            {        
                BRAP_P_InterruptUnInstall(hRap->hRapCapCh[uiPCMCapIndex]);
            }
        }
    }    
#ifdef 	RAP_GFX_SUPPORT
    for(uiGfxIndex= 0; uiGfxIndex < BRAP_RM_P_MAX_GFX_CHANNELS; uiGfxIndex++) 
    {
        if(hRap->hRapGfxCh[uiGfxIndex]) 
        { 
            if((BRAP_P_State_eOpened == hRap->hRapGfxCh[uiGfxIndex]->eState)
                || (BRAP_P_State_eStarted== hRap->hRapGfxCh[uiGfxIndex]->eState))
            {        
                BRAP_P_InterruptUnInstall(hRap->hRapGfxCh[uiGfxIndex]);
            }
        }
    }      
#endif

#ifdef 	RAP_GFX_SUPPORT
        /* Open and start all the affected DSP and FMM modules */
        for(uiGfxIndex = 0; uiGfxIndex < BRAP_RM_P_MAX_GFX_CHANNELS; uiGfxIndex++) 
            {
                BDBG_MSG(("uiDecChIndex = %d",uiGfxIndex));
                /* Reopen active channels */    
                if(hRap->hRapGfxCh[uiGfxIndex]) 
                { 
                    /* Get current settings for this decode channel */
                    ret = BRAP_GFX_P_GetCurrentChannelSettings(hRap->hRapGfxCh[uiGfxIndex],
                                                                                                    &sGfxChanSettings);
                    if(ret!=BERR_SUCCESS)
                    {
                        ret = BERR_TRACE(ret);
                        goto error;
                    }
    
                    if((BRAP_P_State_eStarted== hRap->hRapGfxCh[uiGfxIndex]->eState))
                    {
                        /* Open this decode channel */            
                        ret = BRAP_GFX_Open(hRap, &hRap->hRapGfxCh[uiGfxIndex], &sGfxChanSettings);
                        if(ret!=BERR_SUCCESS)
                        {
                            ret = BERR_TRACE(ret);
                            goto error;
                        }
                        BDBG_MSG(("hRap->hRapDecCh[uiDecChIndex=%d]->eState = %d",
                                        uiGfxIndex, hRap->hRapGfxCh[uiGfxIndex]->eState));
                }
            }
        }
#endif

	/* Open and start all the affected DSP and FMM modules */
	for(uiDecChIndex = 0; uiDecChIndex < BRAP_RM_P_MAX_DEC_CHANNELS; uiDecChIndex++) 
    {
        BDBG_MSG(("uiDecChIndex = %d",uiDecChIndex));
		/* Reopen active channels */	
		if(hRap->hRapDecCh[uiDecChIndex]) 
        { 
            /* Get current settings for this decode channel */
            ret = BRAP_P_GetCurrentChannelSettings(hRap->hRapDecCh[uiDecChIndex],
            psChanSettings);
			if(ret!=BERR_SUCCESS)
            {
                ret = BERR_TRACE(ret);
                goto error;
            }
            
            if((BRAP_P_State_eOpened == hRap->hRapDecCh[uiDecChIndex]->eState)
                || (BRAP_P_State_eStarted== hRap->hRapDecCh[uiDecChIndex]->eState))
            {
            /* Open this decode channel */            
                ret = BRAP_OpenChannel(hRap, &hRap->hRapDecCh[uiDecChIndex], psChanSettings);
			if(ret!=BERR_SUCCESS)
                {
                    ret = BERR_TRACE(ret);
                    goto error;
                }
            BDBG_MSG(("hRap->hRapDecCh[uiDecChIndex=%d]->eState = %d",
                uiDecChIndex, hRap->hRapDecCh[uiDecChIndex]->eState));
            }

			if (BRAP_P_State_eStarted == hRap->hRapDecCh[uiDecChIndex]->eState)
		    { 
		        /* re-start the channel ONLY if it was started earlier */
                ret = BRAP_DEC_P_GetCurrentAudioParams(hRap->hRapDecCh[uiDecChIndex],
                                                                                        psChanParams);
    			if(ret!=BERR_SUCCESS)
                {
                    ret = BERR_TRACE(ret);
                    goto error;
                }
                
                ret = BRAP_StartChannel(hRap->hRapDecCh[uiDecChIndex], psChanParams);
                if (ret!=BERR_SUCCESS)
                {
                    ret = BERR_TRACE(ret);
                    goto error;
                }
            }
            /* Discuss this situation */
            /* TODO: Restore back PVR trick mode states */
            BKNI_EnterCriticalSection();
            hRap->hRapDecCh[uiDecChIndex]->bInternalCallFromRap = true;
            BKNI_LeaveCriticalSection();            
            if(true == hRap->hRapDecCh[uiDecChIndex]->sTrickModeState.bAudioPaused)
            {
                ret =BRAP_EnablePvrPause(hRap->hRapDecCh[uiDecChIndex],true);
                if (ret != BERR_SUCCESS)
                {
                    ret = BERR_TRACE(ret);
                    goto error;
                }
                
            }
            BKNI_EnterCriticalSection();
            hRap->hRapDecCh[uiDecChIndex]->bInternalCallFromRap = false;
            BKNI_LeaveCriticalSection();            
            
        }
	}

	/* Open and start all PCM playback channel */
	for(uiPCMPbIndex = 0; uiPCMPbIndex < BRAP_RM_P_MAX_PCM_CHANNELS; uiPCMPbIndex++) 
    {
        BDBG_MSG(("uiPCMPbIndex = %d",uiPCMPbIndex));
		/* Reopen active channels */	
		if(hRap->hRapPbCh[uiPCMPbIndex]) 
        { 
            /* Get current settings for this decode channel */
            ret = BRAP_P_GetCurrentChannelSettings(hRap->hRapPbCh[uiPCMPbIndex],
                                                                            psChanSettings);
			if(ret!=BERR_SUCCESS)
            {
                ret = BERR_TRACE(ret);
                goto error;
            }
            
            if((BRAP_P_State_eOpened == hRap->hRapPbCh[uiPCMPbIndex]->eState)
                || (BRAP_P_State_eStarted== hRap->hRapPbCh[uiPCMPbIndex]->eState))
            {
            /* Open this decode channel */            
                ret = BRAP_OpenChannel(hRap, &hRap->hRapPbCh[uiPCMPbIndex], psChanSettings);
			if(ret!=BERR_SUCCESS)
                {
                    ret = BERR_TRACE(ret);
                    goto error;
                }
            BDBG_MSG(("hRap->hRapPbCh[uiPCMPbIndex=%d]->eState = %d",
                uiPCMPbIndex, hRap->hRapPbCh[uiPCMPbIndex]->eState));
            }

			if (BRAP_P_State_eStarted == hRap->hRapPbCh[uiPCMPbIndex]->eState)
		    { 
		        /* re-start the channel ONLY if it was started earlier */
                ret = BRAP_PB_P_GetCurrentAudioParams(hRap->hRapPbCh[uiPCMPbIndex],
                                                                                    psChanParams);
    			if(ret!=BERR_SUCCESS)
                {
                    ret = BERR_TRACE(ret);
                    goto error;
                }

                ret = BRAP_StartChannel(hRap->hRapPbCh[uiPCMPbIndex], psChanParams);
                if (ret!=BERR_SUCCESS)
                {
                    ret = BERR_TRACE(ret);
                    goto error;
                }
		    }
		}
		/* TODO: Restore back PVR trick mode states */
	}

#if (BRAP_7550_FAMILY != 1)
    /* Open and start all the affected PCM Capture channels */
	for (uiPCMCapIndex = 0; uiPCMCapIndex < BRAP_RM_P_MAX_CAP_CHANNELS; uiPCMCapIndex++) 
    {
        BDBG_MSG(("uiPCMCapIndex = %d",uiPCMCapIndex));
       	/* Reopen active channels */	
		if (hRap->hRapCapCh[uiPCMCapIndex]) 
        { 
            /* Get current settings for this decode channel */
            ret = BRAP_P_GetCurrentChannelSettings(hRap->hRapCapCh[uiPCMCapIndex],
                                                                               psChanSettings);
			if(ret!=BERR_SUCCESS)
            {
                ret = BERR_TRACE(ret);
                goto error;
            }
            
            if((BRAP_P_State_eOpened == hRap->hRapCapCh[uiPCMCapIndex]->eState)
                || (BRAP_P_State_eStarted== hRap->hRapCapCh[uiPCMCapIndex]->eState))
            {
            /* Open this capture channel */            
                ret = BRAP_OpenChannel(hRap, &hRap->hRapCapCh[uiPCMCapIndex], psChanSettings);
			if(ret!=BERR_SUCCESS)
                {
                    ret = BERR_TRACE(ret);
                    goto error;
                }
            BDBG_MSG(("hRap->hRapCapCh[uiPCMCapIndex=%d]->eState = %d",
                uiPCMCapIndex, hRap->hRapCapCh[uiPCMCapIndex]->eState));

        BDBG_MSG(("hRap->hRapCapCh[uiPCMCapIndex] > %x", hRap->hRapCapCh[uiPCMCapIndex]));
            }
            
			if (BRAP_P_State_eStarted == hRap->hRapCapCh[uiPCMCapIndex]->eState)
		    { 
		        /* re-start the channel ONLY if it was started earlier */
                ret = BRAP_CAP_P_GetCurrentAudioParams(hRap->hRapCapCh[uiPCMCapIndex],
                                                                                        psChanParams);
    			if(ret!=BERR_SUCCESS)
                {
                    ret = BERR_TRACE(ret);
                    goto error;
                }

                ret = BRAP_StartChannel(hRap->hRapCapCh[uiPCMCapIndex], psChanParams);
                if (ret!=BERR_SUCCESS)
                {
                    ret = BERR_TRACE(ret);
                    goto error;
                }
            }
        }
    }
#endif

	ret = BINT_DisableCallback (hRap->hCallback);
	ret = BINT_EnableCallback (hRap->hCallback);

    error:
        BKNI_Free(psChanSettings);
        BKNI_Free(psChanParams);
	/* Reset the watchdog recovery flag */
	hRap->bWatchdogRecoveryOn = false;

	BDBG_LEAVE(BRAP_ProcessWatchdogInterrupt);

	return ret;
}

/**********************************************************************
Summary:
    Sets various audio post processing stages

Description:
    This function sets the desired audio post processing stages on a given destination. 

    A post processing stage is one that operates on output of a decoder or encoder.
    For PCM playback and capture channels, post processing stages operate on
    PCM samples played from memory or captured from hardware input.
    
    A destination can take audio data from a channel or mixed output of multiple
    channels. If a destination is receiving data from multiple channels after mixing,
    audio post processing stages can be added to specific channel or to the mixed output
    of multiple channels. If audio processing stages are getting added to a specifc
    channel, then this function should be called only when that channel is in "stop"
    state. If audio processing stages are getting added on the mixed output of the
    channels then this function should be called only when all the channels are in
    "stop" stage.

    Usage Examples: 
    Example 1: Consider following two data flows in a single RAP decode channel
    ----------

    Decode -> PL-II -> SRS-XT -> AVL -> DAC
                              -> BBE -> I2S
                
    Here PL-II and SRS-XT post processing stages are common for both DAC
    and I2S destinations. Hence for DAC we need to add post processing stages
    PL-II, SRS-XT and AVL. For I2S we need to add post processing stages
    PL-II, SRS-XT and BBE.
    
    BRAP_ProcessingSettings        sPostProcessingSettings;
    BRAP_ProcessingStageHandle     hPl2, hSrsXt, hAvl, hBbe;
    BRAP_ProcessingStageSettings   sProcessingStageSettings;
    BRAP_ProcessingAlgorithm       sProcessingAlgorithm;
    BERR_Code err;

    sProcessingAlgorithm.eAudioProcessing = BRAP_ProcessingType_ePl2;
    err = BRAP_GetDefaultProcessingStageSettings( &sProcessingAlgorithm,
            &sProcessingStageSettings );
    err = BRAP_CreateProcessingStage( &sProcessingStageSettings,
            &hPl2 );

    sProcessingAlgorithm.eAudioProcessing = BRAP_ProcessingType_eSrsXt;
    err = BRAP_GetDefaultProcessingStageSettings( &sProcessingAlgorithm,
            &sProcessingStageSettings );
    err = BRAP_CreateProcessingStage( &sProcessingStageSettings,
            &hSrsXt );
            
    sProcessingAlgorithm.eAudioProcessing = BRAP_ProcessingType_eAvl;
    err = BRAP_GetDefaultProcessingStageSettings( &sProcessingAlgorithm,
            &sProcessingStageSettings );
    err = BRAP_CreateProcessingStage( &sProcessingStageSettings,
            &hAvl );

    sProcessingAlgorithm.eAudioProcessing = BRAP_ProcessingType_eBbe;
    err = BRAP_GetDefaultProcessingStageSettings( &sProcessingAlgorithm,
            &sProcessingStageSettings );
    err = BRAP_CreateProcessingStage( &sProcessingStageSettings,
            &hBbe );
 
    err = BRAP_GetDefaultPostProcessingStages( &sPostProcessingSettings );
    sPostProcessingSettings.hAudProcessing[0] = hPl2;
    sPostProcessingSettings.hAudProcessing[1] = hSrsXt;
    sPostProcessingSettings.hAudProcessing[2] = hAvl;

    err = BRAP_SetPostProcessingStages( hDac, hDecCh, &sPostProcessingSettings );

    err = BRAP_GetDefaultPostProcessingStages( &sPostProcessingSettings );
    sPostProcessingSettings.hAudProcessing[0] = hPl2;
    sPostProcessingSettings.hAudProcessing[1] = hSrsXt;
    sPostProcessingSettings.hAudProcessing[2] = hBbe;

    err = BRAP_SetPostProcessingStages( hI2s, hDecCh, &sPostProcessingSettings );

    Example 2: Seperate post processing stages for two channels getting mixed.
    ----------
    Consider following data flows for two channels

    Decode -> PL-II -> SRS-XT -> DAC
    Capture -> XEN -> BBE -> DAC

    Decode and capture channels are getting mixed and going to DAC output port.
    Before mixing, decode channel has PL-II, SRS-XT post processing stages and
    capture channel has XEN, BBE processing stages.
     
    BRAP_ProcessingSettings        sPostProcessingSettings;
    BRAP_ProcessingStageHandle     hPl2, hSrsXt, hXen, hBbe;
    BRAP_ProcessingStageSettings   sProcessingStageSettings;
    BRAP_ProcessingAlgorithm       sProcessingAlgorithm;
    BERR_Code err;

    sProcessingAlgorithm.eAudioProcessing = BRAP_ProcessingType_ePl2;
    err = BRAP_GetDefaultProcessingStageSettings( &sProcessingAlgorithm,
            &sProcessingStageSettings );
    err = BRAP_CreateProcessingStage( &sProcessingStageSettings,
            &hPl2 );
            
    sProcessingAlgorithm.eAudioProcessing = BRAP_ProcessingType_eSrsXt;
    err = BRAP_GetDefaultProcessingStageSettings( &sProcessingAlgorithm,
            &sProcessingStageSettings );
    err = BRAP_CreateProcessingStage( &sProcessingStageSettings,
            &hSrsXt );
            
    sProcessingAlgorithm.eAudioProcessing = BRAP_ProcessingType_eXen;
    err = BRAP_GetDefaultProcessingStageSettings( &sProcessingAlgorithm,
            &sProcessingStageSettings );
    err = BRAP_CreateProcessingStage( &sProcessingStageSettings,
            &hXen );
               
    sProcessingAlgorithm.eAudioProcessing = BRAP_ProcessingType_eBbe;
    err = BRAP_GetDefaultProcessingStageSettings( &sProcessingAlgorithm,
            &sProcessingStageSettings );
    err = BRAP_CreateProcessingStage( &sProcessingStageSettings,
            &hBbe );

    err = BRAP_GetDefaultPostProcessingStages( &sPostProcessingSettings );
    sPostProcessingSettings.hAudProcessing[0] = hPl2;
    sPostProcessingSettings.hAudProcessing[1] = hSrsXt;

    err = BRAP_SetPostProcessingStages( hDac, hDecCh, &sPostProcessingSettings );

    err = BRAP_GetDefaultPostProcessingStages( &sPostProcessingSettings );
    sPostProcessingSettings.hAudProcessing[0] = hXen;
    sPostProcessingSettings.hAudProcessing[2] = hBbe;

    err = BRAP_SetPostProcessingStages( hI2s, hCapCh, &sPostProcessingSettings );

    Example 3: Common post processing stages for two channels getting mixed
    ----------

    (Decode + Capture ) -> PL-II -> SRS-XT -> DAC 

    BRAP_ProcessingSettings            sPostProcessingSettings;
    BRAP_ProcessingStageHandle         hPl2, hSrsXt;
    BRAP_ProcessingStageSettings       sProcessingStageSettings;
    BRAP_ProcessingAlgorithm           sProcessingAlgorithm;
    BERR_Code err;

    sProcessingAlgorithm.eAudioProcessing = BRAP_ProcessingType_ePl2;
    err = BRAP_GetDefaultProcessingStageSettings( &sProcessingAlgorithm,
            &sProcessingStageSettings );
    err = BRAP_CreateProcessingStage( &sProcessingStageSettings,
            &hPl2 );
            
    sProcessingAlgorithm.eAudioProcessing = BRAP_ProcessingType_eSrsXt;
    err = BRAP_GetDefaultProcessingStageSettings( &sProcessingAlgorithm,
            &sProcessingStageSettings );
    err = BRAP_CreateProcessingStage( &sProcessingStageSettings,
            &hSrsXt );

    err = BRAP_GetDefaultPostProcessingStages( &sPostProcessingSettings );
    sPostProcessingSettings.hAudProcessing[0] = hPl2;
    sPostProcessingSettings.hAudProcessing[1] = hSrsXt;

    err = BRAP_SetPostProcessingStages( hDac, NULL, &sPostProcessingSettings );

    Example 4: Decode with post processing and passthru in same decode channel
    ----------

    Decode -> PL-II -> SRS-XT -> DAC
           -> Convert -> SPDIF

    BRAP_ProcessingSettings            sPostProcessingSettings;
    BRAP_ProcessingStageHandle         hPl2, hSrsXt, hConvert;
    BRAP_ProcessingStageSettings       sProcessingStageSettings;
    BRAP_ProcessingAlgorithm           sProcessingAlgorithm;
    BERR_Code err;
    
    sProcessingAlgorithm.eAudioProcessing = BRAP_ProcessingType_ePl2;
    err = BRAP_GetDefaultProcessingStageSettings( &sProcessingAlgorithm,
            &sProcessingStageSettings );
    err = BRAP_CreateProcessingStage( &sProcessingStageSettings,
            &hPl2 );
            
    sProcessingAlgorithm.eAudioProcessing = BRAP_ProcessingType_eSrsXt;
    err = BRAP_GetDefaultProcessingStageSettings( &sProcessingAlgorithm,
            &sProcessingStageSettings );
    err = BRAP_CreateProcessingStage( &sProcessingStageSettings,
            &hSrsXt );

    sProcessingAlgorithm.eAudioProcessing = BRAP_ProcessingType_eConvert;
    sProcessingAlgorithm.uAlgorithm.eConverterType = BRAP_ConverterType_ePassthru;
    err = BRAP_GetDefaultProcessingStageSettings( &sProcessingAlgorithm,
            &sProcessingStageSettings );
    err = BRAP_CreateProcessingStage( &sProcessingStageSettings,
            &hConvert );


    err = BRAP_GetDefaultPostProcessingStages( &sPostProcessingSettings );
    sPostProcessingSettings.hAudProcessing[0] = hPl2;
    sPostProcessingSettings.hAudProcessing[1] = hSrsXt;

    err = BRAP_SetPostProcessingStages( hDac, hDecCh, &sPostProcessingSettings );
    
    err = BRAP_GetDefaultPostProcessingStages( &sPostProcessingSettings );
    sPostProcessingSettings.hAudProcessing[0] = hConvert;

    err = BRAP_SetPostProcessingStages( hSpdif, hDecCh, &sPostProcessingSettings );

    Example 5: Generic Transcode case
    ----------
    Decode -> DAC
           -> encode -> SPDIF
    Decoded PCM samples go to DAC whereas transcoded data goes to SPDIF
    
    BRAP_ProcessingSettings            sPostProcessingSettings;
    BRAP_ProcessingStageHandle         hEncode;
    BRAP_ProcessingStageSettings       sProcessingStageSettings;
    BRAP_ProcessingAlgorithm           sProcessingAlgorithm;
    BERR_Code err;
    
    sProcessingAlgorithm.eAudioProcessing = BRAP_ProcessingType_eEncode;
    sProcessingAlgorithm.uAlgorithm.eEncodeType = BRAP_EncodeType_eDts;
    err = BRAP_GetDefaultProcessingStageSettings( &sProcessingAlgorithm,
            &sProcessingStageSettings );
    err = BRAP_CreateProcessingStage( &sProcessingStageSettings,
            &hEncode );

    err = BRAP_GetDefaultPostProcessingStages( &sPostProcessingSettings );
    sPostProcessingSettings.hAudProcessing[0] = hEncode;

    err = BRAP_SetPostProcessingStages( hSpdif, hDecCh, &sPostProcessingSettings );
    
Returns:
    BERR_SUCCESS - If function is successful.
    BERR_INVALID_PARAMETER - On bad input parameters.
    BERR_NOT_SUPPORTED - If this function is called when channel is in
	"start" state.

See Also:
    BRAP_AddDestination
    BRAP_GetAudioProcessingStages
***********************************************************************/
BERR_Code BRAP_SetPostProcessingStages (
    BRAP_DestinationHandle          hDestHandle,            /* [in] Destination handle */
    BRAP_ChannelHandle              hRapCh,                 /* [in] Channel handle */
    const BRAP_ProcessingSettings    *psStgSettings  
                                                                                  /* [in] Settings for all the stages in 
                                                                                       the branch */
)                                                                                       
{
    BERR_Code ret = BERR_SUCCESS;
    unsigned int i = 0,j = 0,x=0,y=0,z=0,k = 0;
    BRAP_DestinationHandle hDestFound[BRAP_P_MAX_DEST_PER_PROCESSING_STAGE];
    bool bFinalStage = false,bDestPresent=false;   
    unsigned int found=BRAP_INVALID_VALUE;
    BRAP_P_DstDetails *psDstOldDetails;
    bool bRemovalCase = false, bflag = false;
    bool	bChSpecificStages = true;
    BRAP_OutputPort     eOutputPort = BRAP_OutputPort_eMax;
    

    BDBG_ENTER(BRAP_SetPostProcessingStages);
    BDBG_ASSERT(hDestHandle);
    BDBG_ASSERT(psStgSettings);
    BSTD_UNUSED(hDestFound);

    /* Allocate big structures on heap */
    psDstOldDetails = hDestHandle->hAssociation->hRap->sOpenTimeMallocs.pPvtDstDetails;
    if( NULL==psDstOldDetails)
    {
        ret = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        goto error;
    }
       BKNI_Memset(psDstOldDetails,0,sizeof( BRAP_P_DstDetails ));    
    *psDstOldDetails = *hDestHandle;

    if(hDestHandle->sExtDstDetails.eAudioDst == BRAP_AudioDst_eOutputPort)
        eOutputPort = hDestHandle->sExtDstDetails.uDstDetails.sOpDetails.eOutput[0];
        
    if(hRapCh == NULL)
    {
        bChSpecificStages = false;
        
        if(eOutputPort != BRAP_OutputPort_eMax)
            BDBG_MSG(("\n Configure Post-processing branch for Output port %d"
            " on Association Network",eOutputPort));
        else
            BDBG_MSG(("\n Configure Post-processing branch for Ringbuffer destination"
            " on Association Network"));
    }
    else /* If hRapCh==NULL i.e. Association specific */
    {
        bChSpecificStages = true;
        
        if(eOutputPort != BRAP_OutputPort_eMax)
            BDBG_MSG(("\n Configure Post-processing branch for Output port %d"
            " on Channel Network",eOutputPort));
        else
            BDBG_MSG(("\n Configure Post-processing branch for Ringbuffer destination"
            " on Channel Network"));        
    }

    if(true == bChSpecificStages)
    {
        /*check if channel feeds its output to the destination. 
        If not, return BERR_BAD_PARAMETER error.*/
        for(i=0;i<BRAP_MAX_PRI_DEC_CHAN_IN_ASSOCIATED_GRP;i++)
        {
            if(hDestHandle->hAssociation->hPriDecCh[i] == hRapCh)
            {   
                found=i;
                break;
            }
        }
        
        if (BRAP_INVALID_VALUE==found)
        {
            for(i=0;i<BRAP_MAX_SEC_DEC_CHAN_IN_ASSOCIATED_GRP;i++)
            {
                if(hDestHandle->hAssociation->hSecDecCh[i] == hRapCh)
                {   
                    found=i;
                    break;
                }
            }
        }
        if (BRAP_INVALID_VALUE==found)
        {
            for(i=0;i<BRAP_MAX_PB_CHAN_IN_ASSOCIATED_GRP;i++)
            {
                if(hDestHandle->hAssociation->hPBCh[i] == hRapCh)
                {
                    found=i;
                    break;
                }
            }
        }

        if (BRAP_INVALID_VALUE==found)
        {
            for(i=0;i<BRAP_MAX_CAP_CHAN_IN_ASSOCIATED_GRP;i++)
            {
                if(hDestHandle->hAssociation->hCapCh[i] == hRapCh)
                {   
                    found=i;
                    break;
                }
            }
        }

        if(BRAP_INVALID_VALUE==found)
        {
            BDBG_ERR(("RAP channel handle is not found in the Destination channel Handle!"));
            ret = BERR_TRACE(BERR_INVALID_PARAMETER);
            goto end;
        }        
                        
        /* check if channel is in "stop" condition.
        If not return BERR_NOT_SUPPORTED error.*/
        if(BRAP_P_State_eStarted == hRapCh->eState)
        {
            BDBG_ERR(("This API should only be called in channel stopped state!"));
            ret = BERR_TRACE( BERR_NOT_SUPPORTED );
            goto end;
        }
                        
        /*If all the audio processing stage handles are NULL, 
        then remove entry corresponding to hRapCh from hDestHandle-> psProcessingSettings*/     
        for(i=0 ; i<BRAP_MAX_PP_PER_BRANCH_SUPPORTED ; i++)
        {
            if(!(BRAP_P_IsPointerValid((void *)psStgSettings->hAudProcessing[i])))
            {
                continue;
            }
            else
            {
                break;
            }
        }
        if(i==BRAP_MAX_PP_PER_BRANCH_SUPPORTED)
        {
            bRemovalCase = true;
            for(i=0;i<BRAP_P_MAX_RAPCH_PER_DST;i++)
            {
                if(!(BRAP_P_IsPointerValid((void *)hDestHandle->psProcessingSettings[i])))
                    continue;
                if(hDestHandle->psProcessingSettings[i]->hRapCh ==hRapCh)
                {
                    hDestHandle->psProcessingSettings[i]->hRapCh =NULL;
                    for(j=0;j<BRAP_MAX_PP_PER_BRANCH_SUPPORTED;j++)
                    {
                        if(!(BRAP_P_IsPointerValid((void *)hDestHandle->psProcessingSettings[i]->sExternalSettings.hAudProcessing[j])))
                            continue;
                        for(k=0;k<BRAP_MAX_PP_SUPPORTED;k++)
                        {
                            if(!(BRAP_P_IsPointerValid((void *)hRapCh->hRap->hAudioProcessingStageHandle[k])))
                                continue;
                            if(hRapCh->hRap->hAudioProcessingStageHandle[k] == 
                                hDestHandle->psProcessingSettings[i]->sExternalSettings.hAudProcessing[j])
                            {
                                for(x=0;x<BRAP_P_MAX_DEST_PER_PROCESSING_STAGE;x++)
                                {
                                    if(hDestHandle == 
                                        hRapCh->hRap->hAudioProcessingStageHandle[k]->hDestHandle[x])
                                    {
                                        hRapCh->hRap->hAudioProcessingStageHandle[k]->hDestHandle[x] = NULL;
                                        break;
                                    }
                                }
                            }
                        }
                    }

                    hDestHandle->psProcessingSettings[i]->sExternalSettings = *psStgSettings;
                    break;
                }
            }
        }

        for(i=0;i<BRAP_MAX_PP_PER_BRANCH_SUPPORTED;i++)    
        {	
            if(BRAP_P_IsPointerValid((void *)psStgSettings->hAudProcessing[i]))
            {                 
                for(k=0;k<BRAP_P_MAX_DEST_PER_PROCESSING_STAGE;k++)
                {
                    if((BRAP_P_IsPointerValid((void *)psStgSettings->hAudProcessing[i]->hDestHandle[k]))
                        && (psStgSettings->hAudProcessing[i]->hDestHandle[k] == hDestHandle))
                    {
                        psStgSettings->hAudProcessing[i]->hDestHandle[k]=NULL;
                        break;
                    }
                }
            }
            else
            {
                break;
            }    
        }

        if(true == bRemovalCase)
        {                
            goto end;
        }             
    
    }
    else /* bChSpecificStages== false */
    {
        for(i = 0;i < BRAP_MAX_PRI_DEC_CHAN_IN_ASSOCIATED_GRP;i++)
        {
            if(BRAP_P_IsPointerValid((void *)hDestHandle->hAssociation->hPriDecCh[i]))
            {
                if(BRAP_P_State_eStarted == hDestHandle->hAssociation->hPriDecCh[i]->eState)
                {
                    BDBG_ERR(("This API should only be called in channel stopped state!"));
                    ret = BERR_TRACE( BERR_NOT_SUPPORTED );
                    goto end;
                }
            }
        }   
        
        for(i = 0;i < BRAP_MAX_SEC_DEC_CHAN_IN_ASSOCIATED_GRP;i++)
        {
            if(BRAP_P_IsPointerValid((void *)hDestHandle->hAssociation->hSecDecCh[i]))
            {
                if(BRAP_P_State_eStarted == hDestHandle->hAssociation->hSecDecCh[i]->eState)
                {
                    BDBG_ERR(("This API should only be called in channel stopped state!"));
                    ret = BERR_TRACE( BERR_NOT_SUPPORTED );
                    goto end;
                }
            }
        }            

        for(i = 0;i < BRAP_MAX_PB_CHAN_IN_ASSOCIATED_GRP;i++)
        {
            if(BRAP_P_IsPointerValid((void *)hDestHandle->hAssociation->hPBCh[i]))
            {
                if(BRAP_P_State_eStarted == hDestHandle->hAssociation->hPBCh[i]->eState)
                {
                    BDBG_ERR(("This API should only be called in channel stopped state!"));
                    ret = BERR_TRACE( BERR_NOT_SUPPORTED );
                    goto end;
                }
            }
        }            

        for(i = 0;i < BRAP_MAX_CAP_CHAN_IN_ASSOCIATED_GRP;i++)
        {
            if(BRAP_P_IsPointerValid((void *)hDestHandle->hAssociation->hCapCh[i]))
            {
                if(BRAP_P_State_eStarted == hDestHandle->hAssociation->hCapCh[i]->eState)
                {
                        BDBG_ERR(("This API should only be called in channel stopped state!"));
                        ret = BERR_TRACE( BERR_NOT_SUPPORTED );
                        goto end;
                }
            }
        }                   

        /*If all the audio processing stage handles are NULL, 
        then remove entry corresponding to hRapCh from hDestHandle-> psProcessingSettings*/     
        for(i=0 ; i<BRAP_MAX_PP_PER_BRANCH_SUPPORTED ; i++)
        {
            if(!(BRAP_P_IsPointerValid((void *)psStgSettings->hAudProcessing[i])))
            {
                continue;
            }
            else
            {
                break;
            }
        }
        if(i==BRAP_MAX_PP_PER_BRANCH_SUPPORTED)
        {
            bRemovalCase = true;
            
            if(BRAP_P_IsPointerValid((void *)hDestHandle->psProcessingSettings[BRAP_P_MAX_RAPCH_PER_DST]))
            {
                for(j=0;j<BRAP_MAX_PP_PER_BRANCH_SUPPORTED;j++)
                {
                    if(!(BRAP_P_IsPointerValid((void *)hDestHandle->psProcessingSettings[BRAP_P_MAX_RAPCH_PER_DST]-> \
                                    sExternalSettings.hAudProcessing[j])))
                        continue;
                    for(k=0;k<BRAP_MAX_PP_SUPPORTED;k++)
                    {
                        if(!(BRAP_P_IsPointerValid((void *)hDestHandle->hAssociation->hRap->hAudioProcessingStageHandle[k])))
                            continue;
                        if(hDestHandle->hAssociation->hRap->hAudioProcessingStageHandle[k] == 
                            hDestHandle->psProcessingSettings[BRAP_P_MAX_RAPCH_PER_DST]->sExternalSettings.hAudProcessing[j])
                        {
                            for(x=0;x<BRAP_P_MAX_DEST_PER_PROCESSING_STAGE;x++)
                            {
                                if(hDestHandle == 
                                    hDestHandle->hAssociation->hRap->hAudioProcessingStageHandle[k]->hDestHandle[x])
                                {
                                    hDestHandle->hAssociation->hRap->hAudioProcessingStageHandle[k]->hDestHandle[x] = NULL;
                                    break;
                                }
                            }
                        }
                    }
                }

                hDestHandle->psProcessingSettings[BRAP_P_MAX_RAPCH_PER_DST]->sExternalSettings = *psStgSettings;
            }
        }

        for(i=0;i<BRAP_MAX_PP_PER_BRANCH_SUPPORTED;i++)    
        {	
            if(BRAP_P_IsPointerValid((void *)psStgSettings->hAudProcessing[i]))
            {                 
                for(k=0;k<BRAP_P_MAX_DEST_PER_PROCESSING_STAGE;k++)
                {
                    if((BRAP_P_IsPointerValid((void *)psStgSettings->hAudProcessing[i]->hDestHandle[k]))
                        && (psStgSettings->hAudProcessing[i]->hDestHandle[k] == hDestHandle))
                    {
                        psStgSettings->hAudProcessing[i]->hDestHandle[k]=NULL;
                        break;
                    }
                }
            }
            else
            {
                break;
            }    
        }

        if(true == bRemovalCase)
        {    
            goto end;
        }         
    }       

    /*TODO: Check if any pre-processing algorithm is getting set as post processing.
           If yes, return error  later*/
    /*TIPS: Declare one static array,the array will have all the supported pre-processing algos.
                 Check any of the pre-processing algo is added as post processing in the psStgSettings*/

    /*Check following
        -Converter stage should not exist with other processing stages. (as per current usage modes)
        -Encoder stage should always be a final stage (as per current usage modes) */


    for(i=0;i < BRAP_MAX_PP_PER_BRANCH_SUPPORTED ; i++)
    {
    	if(!(BRAP_P_IsPointerValid((void *)psStgSettings->hAudProcessing[i])))
		{
            break;
        }		
        if(psStgSettings->hAudProcessing[i]->sProcessingStageSettings.eAudioProcessing == BRAP_ProcessingType_eNone)
        {
        	BDBG_MSG (("The stage %d is NONE. Ignoring it !!",i));
            continue;
        }
        if((BRAP_ProcessingType_eFwMixer == psStgSettings->hAudProcessing[i]->sProcessingStageSettings.eAudioProcessing) &&
            (bChSpecificStages == true))
        {
            BDBG_ERR(("Error!! FW Mixer  modules cannot be added at Pre-Mixing level."
                "\n They need to be added on Association(Post-Mixing)"));
            ret =  BERR_NOT_SUPPORTED;
            goto end;            
        }
            
        BDBG_MSG(("\n Post-processing module to be added is %d",
            psStgSettings->hAudProcessing[i]->sProcessingStageSettings.eAudioProcessing));
        /* Logic to check if the Post processing is supported in compile */
        if ( (psStgSettings->hAudProcessing[i]->sProcessingStageSettings.eAudioProcessing >= BRAP_ProcessingType_eEncodeStartIndex) &&
             (psStgSettings->hAudProcessing[i]->sProcessingStageSettings.eAudioProcessing < BRAP_ProcessingType_ePostprocessingStartIndex)
           )
        {
            /* Encode algorithm */
            if(false == BRAP_FWDWNLD_P_IsEncodeSupported(BRAP_sMapProcessingEnum[psStgSettings->hAudProcessing[i]->sProcessingStageSettings.eAudioProcessing].uEnumName.eEncodeType))
            {
                BDBG_ERR(("Please export RAP_XXXX_SUPPORT=y at the compile time for Processing Id = %d, where XXXX is Post Processing Name e.g AVL etc"
                    ,psStgSettings->hAudProcessing[i]->sProcessingStageSettings.eAudioProcessing));
                ret =  BERR_TRACE(BERR_INVALID_PARAMETER);
                goto end;
            }
            
        }
        else if ( (psStgSettings->hAudProcessing[i]->sProcessingStageSettings.eAudioProcessing >= BRAP_ProcessingType_ePostprocessingStartIndex) &&
                  (psStgSettings->hAudProcessing[i]->sProcessingStageSettings.eAudioProcessing < BRAP_ProcessingType_eMax)
                )
        {
            /* Normal Post Processing algorithm */
            if(false == BRAP_FWDWNLD_P_IsAudProcSupported(BRAP_sMapProcessingEnum[psStgSettings->hAudProcessing[i]->sProcessingStageSettings.eAudioProcessing].uEnumName.eProcessingType))
            {
                BDBG_ERR(("Please export RAP_XXXX_SUPPORT=y at the compile time for Processing Id = %d, where XXXX is Post Processing Name e.g AVL etc"
                    ,psStgSettings->hAudProcessing[i]->sProcessingStageSettings.eAudioProcessing));
                ret =  BERR_TRACE(BERR_INVALID_PARAMETER);
                goto end;
            }
        }
        
        if(bChSpecificStages == true)
        {
            if((BRAP_P_IsPointerValid((void *)psStgSettings->hAudProcessing[i]->uHandle.hRapCh ))
                &&(psStgSettings->hAudProcessing[i]->uHandle.hRapCh != hRapCh))
            {
                BDBG_ERR(("The Processing handle should not be the part of other RapCh "));
                ret =  BERR_TRACE(BERR_INVALID_PARAMETER);
                goto end;
            }
        }
        else
        {
            if((BRAP_P_IsPointerValid((void *)psStgSettings->hAudProcessing[i]->uHandle.hAssociation))
                &&(psStgSettings->hAudProcessing[i]->uHandle.hAssociation != hDestHandle->hAssociation))
            {
                BDBG_ERR(("The Processing handle should not be the part of other  Association"));
                ret =  BERR_TRACE(BERR_INVALID_PARAMETER);
                goto end;
            }
        }
        
        if(true == bFinalStage) /* Trying to add a valid stage after final stage */
        { 
                BDBG_ERR(("Can't add another stage after stage 0x%x with audio processing %d", 
                    psStgSettings->hAudProcessing[i-1], 
                    psStgSettings->hAudProcessing[i-1]->sProcessingStageSettings.eAudioProcessing));
                ret = BERR_TRACE( BERR_NOT_SUPPORTED );                    
                goto end;
        }
        
        if ((psStgSettings->hAudProcessing[i]->sProcessingStageSettings.eAudioProcessing == BRAP_ProcessingType_eConvertDdpToAc3)
            ||(psStgSettings->hAudProcessing[i]->sProcessingStageSettings.eAudioProcessing == BRAP_ProcessingType_eWmaProPassThru)
            ||(psStgSettings->hAudProcessing[i]->sProcessingStageSettings.eAudioProcessing == BRAP_ProcessingType_ePassthru) )
        {
            if(0==i)
            {
                bFinalStage =true;
            }
            else
            {
                    BDBG_ERR(("Stage 0x%x with audio processing %d can not be added with other "
                        "processing stages!",  psStgSettings->hAudProcessing[i],
                        psStgSettings->hAudProcessing[i]->sProcessingStageSettings.eAudioProcessing));
                    ret = BERR_TRACE( BERR_NOT_SUPPORTED );            
                    goto end;
            }
        }
        else if((psStgSettings->hAudProcessing[i]->sProcessingStageSettings.eAudioProcessing == BRAP_ProcessingType_eEncodeDts)
            ||(psStgSettings->hAudProcessing[i]->sProcessingStageSettings.eAudioProcessing == BRAP_ProcessingType_eEncodeAc3)
            ||(psStgSettings->hAudProcessing[i]->sProcessingStageSettings.eAudioProcessing == BRAP_ProcessingType_eGenericPassthru)
            ||(psStgSettings->hAudProcessing[i]->sProcessingStageSettings.eAudioProcessing == BRAP_ProcessingType_eEncodeMp3))
        {
            bFinalStage = true;
        }
    }
    
#if  0/* Need to put this code in the start channel function call */
    /* Find out all the destinations using these processing stages. 
    hDestFound[] array  is initialized to NULL and countFound = 0. */

    for(j = 0; j < BRAP_P_MAX_DEST_PER_PROCESSING_STAGE; j++)
    {
        hDestFound[j]=NULL;
    }
    
    
    for(i = 0; i < BRAP_MAX_PP_PER_BRANCH_SUPPORTED; i++)
    {
        if(psStgSettings->hAudProcessing[i]==NULL)
            break;

        if(psStgSettings->hAudProcessing[i]->uiStagePosition== BRAP_INVALID_VALUE)
        { /* New stage. No existing destinations using it */
            continue;
        }
        else
        { /* Stage is getting used by other destinations */

            /* Stage can appear only at one place. */
            if (psStgSettings->hAudProcessing[i]->hRapCh!=hRapCh)
                return BERR_INVALID_PARAMETER;

            for(j = 0; j < BRAP_P_MAX_DEST_PER_PROCESSING_STAGE; j++)
            {
                for(count = 0; count < countFound; count++)
                {
                    if(hDestFound[count]==psStgSettings->hAudProcessing[i]->hDestHandle[j])
                {
                        break;                  
                }
                    else
                    {
                        hDestFound[countFound++] = psStgSettings->hAudProcessing[i]->hDestHandle[j];
                    }
                }
            }
        }
    }

    /* Check if stages getting added are in same order on all the destinations 
    that are  already using them. */
    for(i = 0;  i <countFound; i++)
    {
        for (j = 0; j < BRAP_P_MAX_RAPCH_PER_DST; j++)
        {
            if (hDestFound[i]->psProcessingSettings[j]->hRapCh==hRapCh)
            {
            /* Find out point of mis-match. After point of mis-match, stages should not match */
            for(k = 0; k < BRAP_MAX_PP_PER_BRANCH_SUPPORTED; k++)
            {
                if(hDestFound[i]->psProcessingSettings[j]->sExternalSettings.hAudProcessing[k]
                    !=psStgSettings->hAudProcessing[k])
                            {
                                misMatchPt = k;
                        break; /* mis-match point found */
                    }
                }
                for (;k < BRAP_MAX_PP_PER_BRANCH_SUPPORTED; k++)
                  {
                    for (l = misMatchPt;l < BRAP_MAX_PP_PER_BRANCH_SUPPORTED; l++)
                      {
                            if (hDestFound[i]->psProcessingSettings[j]->sExternalSettings.hAudProcessing[k]
                                ==psStgSettings->hAudProcessing[l])
                    {
                                    return BERR_INVALID_PARAMETER;
                        }
                            }
            }
        }
       }
    }
#endif    

    /* Store hRapCh and psStgSettings in destination handle */
    if(true == bChSpecificStages) /*Added post processing branch before mixer*/
    {
        /* First BRAP_P_MAX_RAPCH_PER_DST entries in 
            hDestHandle->psProcessingSettings[BRAP_P_MAX_RAPCH_PER_DST+1] are for
            processing branches before mixing i.e. hRapCh!=NULL. Find free entry and store
            the current branch. */

        for (j = 0; j < BRAP_P_MAX_RAPCH_PER_DST; j++)
        {
            if((BRAP_P_IsPointerValid((void *)hDestHandle->psProcessingSettings[j]))
                &&(hDestHandle->psProcessingSettings[j] ->hRapCh == hRapCh))
            {
                hDestHandle->psProcessingSettings[j]->sExternalSettings = *psStgSettings;
                bflag = true;    
                break;
            }
        }
        if(bflag == false)
        {
            for (j = 0; j < BRAP_P_MAX_RAPCH_PER_DST; j++)
            {                    
                if((BRAP_P_IsPointerValid((void *)hDestHandle->psProcessingSettings[j]))
                    &&(!(BRAP_P_IsPointerValid((void *)hDestHandle->psProcessingSettings[j]->hRapCh))))
                {
                    hDestHandle->psProcessingSettings[j]->hRapCh = hRapCh;
                    hDestHandle->psProcessingSettings[j]->sExternalSettings = *psStgSettings;
                    bflag = true;                       
                    break;
                }               
            }
        }

        if(bflag == false)
        {
            for (j = 0; j < BRAP_P_MAX_RAPCH_PER_DST; j++)
            {
                if(!(BRAP_P_IsPointerValid((void *)hDestHandle->psProcessingSettings[j])))
    			{ 					
                    /* Note that psProcessingSettings is allocated in the function BRAP_SetPostProcessingStages,
                    but since its without removiing the destination stage can be added and removed, so the
                    parameter psProcessingSettings is used on each addition . so rather than freeing it each 
                    time on removal of stage, its freed in Removedestiantion*/
    				hDestHandle->psProcessingSettings[j] = (BRAP_P_ProcessingSettings *) BKNI_Malloc(sizeof(BRAP_P_ProcessingSettings));
    		        if( NULL==hDestHandle->psProcessingSettings[j])					
            		{
                        ret = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);					
                        goto end;
			}					
                    BKNI_Memset(hDestHandle->psProcessingSettings[j] ,0,sizeof( BRAP_P_ProcessingSettings ));    
                    hDestHandle->psProcessingSettings[j]->hRapCh = hRapCh;
                    hDestHandle->psProcessingSettings[j]->sExternalSettings = *psStgSettings;
                    break;
                }                
            }        
        }
    } /* if(true == bChSpecificStages) */

    else /*Added post processing branch after mixer*/
    {
        /* Last entry in hDestHandle->psProcessingSettings[BRAP_P_MAX_RAPCH_PER_DST+1]
           is reserved for processing branch after mixing. Since this branch is after mixing,
           store it in the last entry */
        if(!(BRAP_P_IsPointerValid((void *)hDestHandle->psProcessingSettings[BRAP_P_MAX_RAPCH_PER_DST] )))
        {
		    hDestHandle->psProcessingSettings[BRAP_P_MAX_RAPCH_PER_DST] = 
                        (BRAP_P_ProcessingSettings *) BKNI_Malloc(sizeof(BRAP_P_ProcessingSettings));
		
    		if( NULL==hDestHandle->psProcessingSettings[BRAP_P_MAX_RAPCH_PER_DST])
    		{
    			BDBG_ERR(("Unable allocate memory for psProcessingSettings "));						
    			ret = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);					
                goto end;            
    		}
            BKNI_Memset(hDestHandle->psProcessingSettings[BRAP_P_MAX_RAPCH_PER_DST] ,0,sizeof( BRAP_P_ProcessingSettings ));            
        }
        hDestHandle->psProcessingSettings[BRAP_P_MAX_RAPCH_PER_DST]->hRapCh=NULL;
        hDestHandle->psProcessingSettings[BRAP_P_MAX_RAPCH_PER_DST]->sExternalSettings = *psStgSettings;
    }

    if(bRemovalCase == false)
    {
        /* Update all the stage handles in psStgSettings-> hAudProcessing   */
        for(i=0;i<BRAP_MAX_PP_PER_BRANCH_SUPPORTED;i++)
        {		
            if(BRAP_P_IsPointerValid((void *)psStgSettings->hAudProcessing[i]))
            {   
                if(true == bChSpecificStages)
                {
                    psStgSettings->hAudProcessing[i]->uHandle.hRapCh=hRapCh;                
                }
                else
                {
                    psStgSettings->hAudProcessing[i]->uHandle.hAssociation = hDestHandle->hAssociation;                
                }
                psStgSettings->hAudProcessing[i]->bChSpecificStage = bChSpecificStages;
                psStgSettings->hAudProcessing[i]->uiStagePosition=i;
                
                for(k=0;k<BRAP_P_MAX_DEST_PER_PROCESSING_STAGE;k++)
                {
                    if(!(BRAP_P_IsPointerValid((void *)psStgSettings->hAudProcessing[i]->hDestHandle[k])))
                    {
                        psStgSettings->hAudProcessing[i]->hDestHandle[k]=hDestHandle;
                        break;
                    }
                }
            }
            else
            {
                break;
            }    
        }
    }

end:
    if(true == bRemovalCase)
    {          
        if(eOutputPort != BRAP_OutputPort_eMax)
            BDBG_MSG(("\n Removing Post-processing branch from Output port %d",eOutputPort));
        else
            BDBG_MSG(("\n Removing Post-processing branch from Ringbuffer destination"));
    }
        
    /*****************************/
    /* Update states of  all the earlier stage handles on this destination handle that are getting removed*/
    if(true == bChSpecificStages)
    {
        for(x=0;x<BRAP_P_MAX_RAPCH_PER_DST;x++)
        {
        	if (!(BRAP_P_IsPointerValid((void *)psDstOldDetails->psProcessingSettings[x])))
    		{
    			continue;
    		}
            if(psDstOldDetails->psProcessingSettings[x]->hRapCh==hRapCh)
            {
                for(y=0;y<BRAP_MAX_PP_PER_BRANCH_SUPPORTED;y++)
                {
                    if(!(BRAP_P_IsPointerValid((void *)psDstOldDetails->psProcessingSettings[x]->sExternalSettings.hAudProcessing[y])))
                    {
                       break;
                    }                                    
                    for(z=0;z<BRAP_MAX_PP_PER_BRANCH_SUPPORTED;z++)
                    {
                        if(!(BRAP_P_IsPointerValid((void *)psStgSettings->hAudProcessing[z])))
                        {
                           continue;
                        }                    
                        
                        if(psDstOldDetails->psProcessingSettings[x]->sExternalSettings.hAudProcessing[y]==psStgSettings->hAudProcessing[z])
                        {
                            break;
                        }
                    }
                    if(z==BRAP_MAX_PP_PER_BRANCH_SUPPORTED)
                    {
                        /* Stage getting removed from this destination */
                        for(i=0;i<BRAP_P_MAX_DEST_PER_PROCESSING_STAGE;i++)
                        {
                            if(psDstOldDetails->psProcessingSettings[x]->sExternalSettings.hAudProcessing[y]->hDestHandle[i]==hDestHandle)
                            {
                                psDstOldDetails->psProcessingSettings[x]->sExternalSettings.hAudProcessing[y]->hDestHandle[i] = NULL;
                                /* If all destination handles are NULL, then set the channel handle to NULL */
                                break;
                            }
                            if(psDstOldDetails->psProcessingSettings[x]->sExternalSettings.hAudProcessing[y]->hDestHandle[i]!=NULL)
                                bDestPresent = true;
                        }
                        if (false==bDestPresent)
                        {
                            for(;i<BRAP_P_MAX_DEST_PER_PROCESSING_STAGE;i++)
                            {
                                if(psDstOldDetails->psProcessingSettings[x]->sExternalSettings.hAudProcessing[y]->hDestHandle[i]!=NULL)
                                    break;
                            }
                            if(i==BRAP_P_MAX_DEST_PER_PROCESSING_STAGE)
                            {
                                psDstOldDetails->psProcessingSettings[x]->sExternalSettings.hAudProcessing[y]->uHandle.hRapCh=NULL;
                            }
                        }
                    }
                }
            }
        }
    }
    else  
    {
        for(j=0;j<BRAP_MAX_PP_PER_BRANCH_SUPPORTED;j++)
        {
            if ((!(BRAP_P_IsPointerValid((void *)psDstOldDetails->psProcessingSettings[BRAP_P_MAX_RAPCH_PER_DST])))||
                (!(BRAP_P_IsPointerValid((void *)psDstOldDetails->psProcessingSettings[BRAP_P_MAX_RAPCH_PER_DST]->sExternalSettings.hAudProcessing[j]))))
            {
               break;
            }           
            for(x=0;x<BRAP_MAX_PP_PER_BRANCH_SUPPORTED;x++)
            {
                if(!(BRAP_P_IsPointerValid((void *)psStgSettings->hAudProcessing[x])))
				{
                   break;
				}
				
                if(!(BRAP_P_IsPointerValid((void *)psDstOldDetails->psProcessingSettings[BRAP_P_MAX_RAPCH_PER_DST])))
				  { 
                    BDBG_MSG(("\n psProcessingSettings[BRAP_P_MAX_RAPCH_PER_DST]==NULL"));
					break;
				  }
				
                if(psDstOldDetails->psProcessingSettings[BRAP_P_MAX_RAPCH_PER_DST]->sExternalSettings.hAudProcessing[j]==psStgSettings->hAudProcessing[x])
                {
                    break;
                }
            }
            if(x==BRAP_MAX_PP_PER_BRANCH_SUPPORTED)
            {
                /* Stage getting removed from this destination */
                for(i=0;i<BRAP_P_MAX_DEST_PER_PROCESSING_STAGE;i++)
                {
                    if(psDstOldDetails->psProcessingSettings[BRAP_P_MAX_RAPCH_PER_DST]->sExternalSettings.hAudProcessing[j]->hDestHandle[i]==hDestHandle)
                    {
                        psDstOldDetails->psProcessingSettings[BRAP_P_MAX_RAPCH_PER_DST]->sExternalSettings.hAudProcessing[j]->hDestHandle[i] = NULL;
                        break;
                    }
                   if(psDstOldDetails->psProcessingSettings[BRAP_P_MAX_RAPCH_PER_DST]->sExternalSettings.hAudProcessing[j]->hDestHandle[i]!=NULL)
                        bDestPresent = true;
                }
                if (false==bDestPresent)
                {
                    for(;i<BRAP_P_MAX_DEST_PER_PROCESSING_STAGE;i++)
                    {
                        if(psDstOldDetails->psProcessingSettings[BRAP_P_MAX_RAPCH_PER_DST]->sExternalSettings.hAudProcessing[j]->hDestHandle[i]!=NULL)
                            break;
                    }
                    if(i==BRAP_P_MAX_DEST_PER_PROCESSING_STAGE)
                    {
                        psDstOldDetails->psProcessingSettings[BRAP_P_MAX_RAPCH_PER_DST]->sExternalSettings.hAudProcessing[j]->uHandle.hAssociation=NULL;
                    }
                }
            }
        }
    }

error:
    
    BDBG_LEAVE(BRAP_SetPostProcessingStages);
    return ret;
}

/**********************************************************************
Summary:
    Gets audio post processing stages added for a destination

Description:
    This function returns the post processing stages added for a destination by API
    BRAP_SetPostProcessingStages().
    
Returns:
    BERR_SUCCESS - If function is successful.
    BERR_INVALID_PARAMETER - On bad input parameters.
See Also:
    BRAP_SetPostProcessingStages
***********************************************************************/
BERR_Code BRAP_GetCurrentPostProcessingStages (
    BRAP_DestinationHandle          hDestHandle,
                                                                /* [in] Destination handle */    
    BRAP_ChannelHandle              hRapCh,                 /* [in] Channel handle */
    BRAP_ProcessingSettings    *psStgSettings 
                                                           /* [out] Settings for all the stages in 
                                                                the branch */
)
{
    unsigned int i = 0;

    BDBG_ENTER(BRAP_GetCurrentPostProcessingStages);    
    BDBG_ASSERT(hDestHandle);

    if(BRAP_P_IsPointerValid(hRapCh))
    {
        /*Find entry in psProcessingSettings array in destination handle corresponding to channel handle passed to the function */
        for(i=0;i<BRAP_P_MAX_RAPCH_PER_DST;i++)
        {
            if(!(BRAP_P_IsPointerValid((void *)hDestHandle->psProcessingSettings[i])))
                continue;
            if(hDestHandle->psProcessingSettings[i]->hRapCh == hRapCh)
            {
                *psStgSettings = hDestHandle->psProcessingSettings[i]->sExternalSettings;
                break;
            }
        }
        
        if(i == BRAP_P_MAX_RAPCH_PER_DST)
        {
            return BERR_INVALID_PARAMETER;         
        }
    }
    else
    {
        /* Association PP */
        if(BRAP_P_IsPointerValid(hDestHandle->psProcessingSettings[BRAP_P_MAX_RAPCH_PER_DST]))
        {
            *psStgSettings = hDestHandle->psProcessingSettings[BRAP_P_MAX_RAPCH_PER_DST]->sExternalSettings;
        }    
    }
    
    return BERR_SUCCESS;
}

/**********************************************************************
Summary:
    Gets default audio post processing stages added for a destination
    
Description:
    This function initializes fields of BRAP_ProcessingSettings to NULL values. Application
    should populate this structure properly before passing on to API
    BRAP_SetPostProcessingStages().
    
Returns:
    BERR_SUCCESS - If function is successful.
    BERR_INVALID_PARAMETER - On bad input parameters.
See Also:
    BRAP_SetPostProcessingStages
***********************************************************************/
BERR_Code BRAP_GetDefaultPostProcessingStages (
    BRAP_ProcessingSettings    *psStgSettings 
                                                           /* [out] Settings for all the stages in 
                                                                the branch */
)
{
    unsigned int i = 0;

    BDBG_ENTER(BRAP_GetDefaultPostProcessingStages);    
    BDBG_ASSERT(psStgSettings);

        /*Initialize all fields of struct psStgSettings to NULL.*/
    for(i=0;i<BRAP_MAX_PP_PER_BRANCH_SUPPORTED;i++)
    {
        psStgSettings->hAudProcessing[i]=NULL;
    }

    BDBG_LEAVE(BRAP_GetDefaultPostProcessingStages);
    return BERR_SUCCESS;
}

/**********************************************************************
Summary:
    Sets audio pre-processing stages for a given decode/encode channel

Description:
    This function sets the pre-processing stages for a given channel. A pre-processing
    stage is one that preceeds encoder or decoder. This function is supported only for
    encode and decode channels. This function should be called only when channel is
    in "stop" condition.
    
Returns:
    BERR_SUCCESS - If function is successful.
    BERR_INVALID_PARAMETER - On bad input parameters.
    BERR_NOT_SUPPORTED - If channel is not in "stop" condition or if channel is
        different than encode/decode.

see Also:
    BRAP_GetPreProcessingStages
***********************************************************************/
BERR_Code BRAP_SetPreProcessingStages (
    BRAP_ChannelHandle          hRapCh,
                                                                /* [in] Destination handle */    
    const BRAP_ProcessingSettings    *psStgSettings 
	                                                        /* [in] Settings for all the stages in 
	                                                             the branch */
)
{
    BSTD_UNUSED( hRapCh );
    BSTD_UNUSED( psStgSettings );

    return BERR_SUCCESS;
}


/**********************************************************************
Summary:
    Gets current audio pre-processing stages

Description:
    This function returns the pre-processing stages added by API
    BRAP_SetPreProcessingStages().
    
Returns:
    BERR_SUCCESS - If function is successful.
    BERR_INVALID_PARAMETER - On bad input parameters.
See Also:
    BRAP_SetPrePostProcessingStages
***********************************************************************/
BERR_Code BRAP_GetCurrentPreProcessingStages (
    BRAP_ChannelHandle          hRapCh,
                                                            /* [in] Destination handle */    
    BRAP_ProcessingSettings    *psStgSettings 
	                                                   /* [out] Settings for all the stages in 
	                                                       the branch */
)
{
    BSTD_UNUSED( hRapCh );
    BSTD_UNUSED( psStgSettings );

    return BERR_SUCCESS;
}


/**********************************************************************
Summary:
    Gets default audio pre-processing stages

Description:
    This function initializes fields of BRAP_ProcessingSettings to NULL values. Application
    should populate this structure properly before passing on to API
    BRAP_SetPreProcessingStages().
    
Returns:
    BERR_SUCCESS - If function is successful.
    BERR_INVALID_PARAMETER - On bad input parameters.
See Also:
    BRAP_SetPrePostProcessingStages
***********************************************************************/
BERR_Code BRAP_GetDefaultPreProcessingStages (
    BRAP_ProcessingSettings    *psStgSettings 
	                                                   /* [out] Settings for all the stages in 
	                                                       the branch */
)
{
    BSTD_UNUSED( psStgSettings );

    return BERR_SUCCESS;
}

/**********************************************************************
Summary:
	This function will configure the CRC registers

Description:
	This function will configure the CRC registers and give the crc configuration
	to install the inerrupt for respective output port. 
	This api must be called before installing BRAP_Interrupt_eFmmCrc interrupt.

Returns:
    BERR_SUCCESS - If function is successful.
    BERR_NOT_SUPPORTED - On bad input parameters.
***********************************************************************/

BERR_Code BRAP_ConfigCrc
(
	BRAP_ChannelHandle	hRapCh,	/* [in] The RAP Channel handle */
	uint32_t				uiSamplePeriod, /* [in] The sample period of Crc */
	uint32_t				*pCrcCfg  /* [out] the crc configuration*/
)
{
#ifdef CRC_ENABLE
#if ( (BRAP_7405_FAMILY == 1) || (BRAP_3548_FAMILY == 1) )
	uint32_t				reg=0;
	BERR_Code			rc=BERR_SUCCESS;
	uint32_t				uicrc_config;
	uint32_t				uiBitsPerSample;
	BRAP_CrcPolarity		ePolarity=BRAP_CrcPolarity_0;
	BRAP_OutputPort		eOp = BRAP_OutputPort_eMax;
    unsigned int        i=0;

	BDBG_ENTER (BRAP_ConfigCrc);
	BDBG_ASSERT(hRapCh);

    for(i=0;i<BRAP_MAX_ASSOCIATED_GROUPS;i++)
    {
        if(hRapCh->uiAssociationId[i] == BRAP_INVALID_VALUE)
            continue;
    	if(hRapCh->hRap->sAssociatedCh[hRapCh->uiAssociationId[i]].sDstDetails[0].sExtDstDetails.eAudioDst != BRAP_AudioDst_eOutputPort)
	{
		BDBG_ERR(("BRAP_ConfigCrc: Output destination not added"));
		return BERR_TRACE(BERR_NOT_SUPPORTED);
	}

    	eOp = hRapCh->hRap->sAssociatedCh[hRapCh->uiAssociationId[i]].sDstDetails[0].sExtDstDetails.uDstDetails.sOpDetails.eOutput[0];
	uiBitsPerSample=hRapCh->hRap->sOutputSettings[eOp].uiOutputBitsPerSample;
    	switch(hRapCh->hRap->sAssociatedCh[hRapCh->uiAssociationId[i]].sDstDetails[0].sExtDstDetails.uDstDetails.sOpDetails.eOutput[0])
	{
		case BRAP_OutputPort_eSpdif: 	uicrc_config=0; 	break;
		case BRAP_OutputPort_eMai:  	uicrc_config=1; 	break;
		case BRAP_OutputPort_eDac0:	uicrc_config=2;	break;
		case BRAP_OutputPort_eI2s0:	uicrc_config=3;	break;
		case BRAP_OutputPort_eI2s1:	uicrc_config=4; 	break;
		case BRAP_OutputPort_eI2s2:	uicrc_config=5;	break;
		case BRAP_OutputPort_eI2s3:	uicrc_config=6;	break;
		case BRAP_OutputPort_eI2s4:	uicrc_config=7;	break;
		default :
			BDBG_ERR(("BRAP_ConfigCrc: CRC is not supported on eOp = %d",eOp));
			return BERR_TRACE(BERR_NOT_SUPPORTED);
		break;
	}
	*pCrcCfg=uicrc_config;

	switch(uiBitsPerSample)
	{
		case 8:
			BDBG_ERR(("BRAP_ConfigCrc: 8 bits/sample CRC not supported"));
			return BERR_TRACE(BERR_NOT_SUPPORTED);
			break;
		case 24:
			reg= reg | (BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_CRC_CFGi,CRC_DAT_WIDTH,sample_24_bit));			
			break;
		case 20:
			reg= reg | (BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_CRC_CFGi,CRC_DAT_WIDTH,sample_20_bit));			
			break;			
		case 16:
			reg= reg | (BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_CRC_CFGi,CRC_DAT_WIDTH,sample_16_bit));			
			break;
		default:
			reg= reg | (BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_CRC_CFGi,CRC_DAT_WIDTH,spdif_status));
			break;		
	}
	switch(ePolarity)	
	{
		case BRAP_CrcPolarity_0:
			reg= reg | (BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_CRC_CFGi,CRC_INIT_POL,all_zeroes));
			break;
		case BRAP_CrcPolarity_1:
			reg= reg | (BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_CRC_CFGi,CRC_INIT_POL,all_ones));
			break;
		default:
			BDBG_ERR(("BRAP_ConfigCrc: Unsupported polarity=%d",ePolarity));
			return BERR_TRACE(BERR_NOT_SUPPORTED);
			break;			
	}

	reg |=0x1; /* To enable the crc HardWare */
	BRAP_Write32 (hRapCh->hRegister, 
	                   ( BCHP_AUD_FMM_OP_CTRL_CRC_CFGi_ARRAY_BASE 
	                   + uicrc_config*4 ), reg); 
	/* Write the sample period of crc */
	BRAP_Write32 (hRapCh->hRegister, 
	                   ( BCHP_AUD_FMM_OP_CTRL_CRC_PERIODi_ARRAY_BASE 
	                   + uicrc_config*4 ), uiSamplePeriod); 
	
    }
	BDBG_LEAVE(BRAP_ConfigCrc);
	return rc;	
#else
	BSTD_UNUSED(hRapCh);
	BSTD_UNUSED(uiSamplePeriod);
	BSTD_UNUSED(pCrcCfg);
	return BERR_NOT_SUPPORTED;
#endif
#else
	BSTD_UNUSED(hRapCh);
	BSTD_UNUSED(uiSamplePeriod);
	BSTD_UNUSED(pCrcCfg);
	return BERR_NOT_SUPPORTED;
#endif
	
}

#if ( (BRAP_3548_FAMILY == 1) )
/**********************************************************************
Summary:
    Creates link information between two channels for simul mode

Description:
    This function creates the linkage between multiple channels where one 
    is the master & other is slave. 
    
Returns:
    BERR_SUCCESS - If function is successful.
    BERR_INVALID_PARAMETER - On bad input parameters.
    BERR_NOT_SUPPORTED - On exceeding supported linkages.
See Also:
    BRAP_DestroyChannelLink
***********************************************************************/
BERR_Code BRAP_CreateChannelLink (
	BRAP_ChannelHandle hRapMaster,   /* [in] The Master RAP channel handle */
	BRAP_ChannelHandle hRapSlave,    /* [in] The Slave RAP channel handle */
	unsigned int *puiLinkId	         /* [out] Linakge index. */
)
{
    BRAP_Handle 	    hRap = NULL;
    int                 i = 0;

    BDBG_ENTER(BRAP_CreateChannelLink);
    BDBG_ASSERT(hRapMaster);
    BDBG_ASSERT(hRapSlave);
    BDBG_ASSERT(puiLinkId);
    hRap = hRapMaster->hRap;
    if(hRap != hRapSlave->hRap)
    {
        BDBG_ERR(("BRAP_CreateChannelLink: hRapMaster & hRapSlave belongs "
            "to different RAP devices"));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    if(hRapMaster->eChannelType != hRapSlave->eChannelType)
    {
        BDBG_ERR(("BRAP_CreateChannelLink: Only same type of channels can be linked "));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    for(i=0;i<BRAP_RM_P_MAX_LINKAGE_SUPPORTED;i++)
    {
        if(false == hRap->sChLinkInfo[i].bUsedLinkage)
            break; /* Found free linkage location */
    }
    if(BRAP_RM_P_MAX_LINKAGE_SUPPORTED == i)
    {
        BDBG_ERR(("BRAP_CreateChannelLink: Exceeded Max Linkages supported "));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    hRap->sChLinkInfo[i].bUsedLinkage = true;
    hRap->sChLinkInfo[i].hRapMasterCh = hRapMaster;
    hRap->sChLinkInfo[i].hRapSlaveCh = hRapSlave;
    *puiLinkId = i;
    BDBG_LEAVE(BRAP_CreateChannelLink);
    return BERR_SUCCESS;
}

/**********************************************************************
Summary:
    Destroys link information between two channels for simul mode

Description:
    This function destroys the linkage between multiple channels where one 
    is the master & other is slave. 
    
Returns:
    BERR_SUCCESS - If function is successful.
    BERR_INVALID_PARAMETER - On bad input parameters.
See Also:
    BRAP_CreateChannelLink
***********************************************************************/
BERR_Code BRAP_DestroyChannelLink (
	BRAP_Handle hRap,               /* [in] The RAP device handle */
	unsigned int uiLinkId	        /* [in] Linakge index. */
)
{
    BDBG_ENTER(BRAP_DestroyChannelLink);
    BDBG_ASSERT(hRap);
    BDBG_ASSERT(uiLinkId < BRAP_RM_P_MAX_LINKAGE_SUPPORTED);
    if(false == hRap->sChLinkInfo[uiLinkId].bUsedLinkage)
    {
        BDBG_ERR(("BRAP_DestroyChannelLink: Invalid Link id "));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    hRap->sChLinkInfo[uiLinkId].bUsedLinkage = false;
    hRap->sChLinkInfo[uiLinkId].hRapMasterCh = NULL;
    hRap->sChLinkInfo[uiLinkId].hRapSlaveCh = NULL;
    BDBG_LEAVE(BRAP_DestroyChannelLink);
    return BERR_SUCCESS;
}

#endif
/***************************************************************************
Summary:
	This function takes out DSP processor from reset.

Description:
	When firmware authentication is enabled, BRAP_Open keeps DSP processor in 
	reset condition for security software module to authenticate the Raptor 
	firmware. After firmware authentication is done, application needs to call
	this function to take out DSP processor from reset. After BRAP_Open this 
	function should be called first before calling any other RAP PI functions.

Returns:
	BERR_SUCCESS - If unresetting DSP is successful.

See Also:
***************************************************************************/

BERR_Code BRAP_StartDsp(BRAP_Handle hRap)
{

    BRAP_DSP_Handle hDsp;
	BERR_Code rc = BERR_SUCCESS ;
    unsigned int uiDspIndex = 0;

	BDBG_ENTER(BRAP_StartDSP);
    /* Assert the function arguments*/
    BDBG_ASSERT(hRap);

    if(hRap->sSettings.bFwAuthEnable==false) /*If Firmware Firmware authentication 
    							is Disabled*/
    {
        BDBG_ERR(("BRAP_StartDsp should be called only if bFwAuthEnable is true"));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    for(uiDspIndex = 0; uiDspIndex < BRAP_RM_P_MAX_DSPS; uiDspIndex++)
    {
        hDsp=hRap->hDsp[uiDspIndex];
        if(!(BRAP_P_IsPointerValid((void *)hDsp)))
            continue;

	/* Initializing the Hardware */
        rc = BRAP_DSP_P_InitHardware(hDsp,hRap->sSettings.bFwAuthEnable);
        if (rc!=BERR_SUCCESS) 
        {
		BDBG_MSG(("BRAP_DSP_P_InitHardware::Error returned %x!",rc));
		return BERR_TRACE(rc);
        }
	}
	BDBG_LEAVE(BRAP_StartDSP);
	return rc;
}

/***************************************************************************
Summary:
	Returns physical memory offset and size of firmware executables.
	
Description:
	Returns the physical memory offset where firmware executables of all the 
	supported audio codecs are downloaded in contiguous memory locations. It 
	also returns the total size of the downloaded firmware executables. This 
	function is supported only when firmware authentication is enabled.

Returns:
	BERR_SUCCESS - If valid offset is set.
	BERR_NOT_SUPPORTED - If Firmware authentication is Disabled.

See Also:	
***************************************************************************/

BERR_Code BRAP_GetFwDwnldInfo( 
	BRAP_Handle 		hRap,			/* [in] The Raptor Audio device handle*/
	uint32_t		*pui32PhysicalAddr,	/* [out] Physical memory offset of 
											downloaded firmware executables  */
	uint32_t		*pui32LogicalAddr,	/* [out] Virtual memory offset of
											downloaded firmware executables  */											
	uint32_t		*pui32FwDwnldSize	/* [out] Total size of downloaded 
											firmware executables */ 
	)
{
	
	BDBG_ENTER(BRAP_GetFwDwnldInfo);
	/* Assert the function arguments*/
	BDBG_ASSERT(hRap);
	BDBG_ASSERT(pui32PhysicalAddr);
	BDBG_ASSERT(pui32LogicalAddr);
	BDBG_ASSERT(pui32FwDwnldSize);

	
	if(hRap->sSettings.bFwAuthEnable==false) /*If Firmware Firmware authentication 
											is Disabled*/
	{
	    BDBG_ERR(("BRAP_GetFwDwnldInfo should be called only if bFwAuthEnable is true"));
		return BERR_TRACE(BERR_NOT_SUPPORTED);
	}

	*pui32LogicalAddr = hRap->sMemInfo.sOpenTimeMemInfo.ui32BaseAddr;	/*Logical address*/
	BRAP_P_ConvertAddressToOffset(hRap->hHeap,(void *)(*pui32LogicalAddr),pui32PhysicalAddr);/*Physical Address */
	*pui32FwDwnldSize=hRap->sMemInfo.sOpenTimeMemInfo.ui32Size; /*Size of the executable download */
	
	BDBG_LEAVE(BRAP_GetFwDwnldInfo);
	return BERR_SUCCESS;
}

/***************************************************************************
Summary:
	API to retrieve maximum number of channels supported for a channel type.

Description:
	This API can be used to retrieve the maximum number of channels supported 
	for a particular channel type by the Raptor Audio PI.
	
Returns:
	BERR_SUCCESS if successful else error value.            
	
See Also:
	
****************************************************************************/
BERR_Code BRAP_GetTotalChannels( 
	BRAP_Handle 	    hRap,			/* [in] Raptor Audio device handle */
	BRAP_ChannelType    eChanType,      /* [in] Channel type for which the max
	                                       number of channels are sought */
	unsigned int	    *pTotalChannels	/* [out] Total decode Audio channels 
											supported */
	)
{
    BDBG_ENTER(BRAP_GetTotalChannels);
    BDBG_ASSERT(hRap);
    BDBG_ASSERT(pTotalChannels);
    BSTD_UNUSED(hRap);
    switch(eChanType)
    {
        case BRAP_ChannelType_eDecode:
            *pTotalChannels = BRAP_RM_P_MAX_DEC_CHANNELS;
            break;
        case BRAP_ChannelType_ePcmCapture:
            *pTotalChannels = BRAP_RM_P_MAX_CAP_CHANNELS;
            break;
        case BRAP_ChannelType_ePcmPlayback:
            *pTotalChannels = BRAP_RM_P_MAX_PCM_CHANNELS;
            break;
        default:
            BDBG_ERR(("UNSUPPORTED CHANNELTYPE!"));
            return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    BDBG_LEAVE(BRAP_GetTotalChannels);
    return BERR_SUCCESS;
}
/***************************************************************************
Summary:
    Sets the coefficients for controlling the Scaling of the Input audio stream.
    This is set per channel per output port.

Description:
	
Returns:
	BERR_SUCCESS if successful else error value.            
	
See Also:
    BRAP_GetScaling().
**************************************************************************/
BERR_Code BRAP_SetScaling(
    BRAP_ChannelHandle  hRapCh,     /* [in] Rap channel Handle */
    BRAP_GainControl    *psGain,    /* [in] Gain coefficients Structure */
    BRAP_OutputPort     eOpType     /* [in] Output Port */
)
{
	BERR_Code               ret = BERR_SUCCESS;
	BRAP_MIXER_P_Handle     hMixer = NULL;
	unsigned int            uiMixerOutput = 0;

    BRAP_Handle             hRap = NULL;
    unsigned int            path = 0, chPair = 0;
    unsigned int            uiLvl = 0, uiPp = 0;
    unsigned int            uiLvl1 = 0, uiPp1 = 0;
    BRAP_OutputChannelPair  eChP = BRAP_OutputChannelPair_eMax;
    BRAP_OutputChannelPair  eChP1 = BRAP_OutputChannelPair_eMax;
    BRAP_RM_P_MixerGrant    sTempMixerGrnt;
    unsigned int            i=0, uiMixerIp =0, j = 0;
    bool                    bGainProgrammed = false;

	BDBG_ENTER(BRAP_SetScaling);
	BDBG_ASSERT(hRapCh);
    BDBG_ASSERT(psGain);

	BDBG_MSG(("BRAP_SetScaling: eOpType = %d", eOpType));
    hRap = hRapCh->hRap;

	if(BRAP_P_IsPointerValid((void *)hRap->hFmm[0]->hOp[eOpType]))
	{
#if (BRAP_7405_FAMILY == 1)
        if((BRAP_OutputPort_eMai == eOpType)&&
            (BRAP_OutputPort_eMai != hRap->hFmm[0]->hOp[eOpType]->uOpSettings.sMai.sExtSettings.eMaiMuxSelector))
        {
            /* Mixer is used for Mai only if Mai is selected as muxselector */
        	BDBG_LEAVE(BRAP_SetScaling);
        	return ret;
        }
#endif        
		/* Get the Mixer handle for mixer linked to this output port */
		ret = BRAP_P_GetMixerForOpPort(hRap,eOpType,&hMixer,&uiMixerOutput);

		if(BERR_INVALID_PARAMETER == ret)
		{
			/* Channel not started yet. */
            BDBG_MSG(("Channel Not Started yet"));
			ret = BERR_SUCCESS;
		}
		else if((BERR_SUCCESS == ret) && (!(BRAP_P_IsPointerValid((void *)hMixer))))
		{
			BDBG_MSG(("BRAP_SetScaling: eOpType = %d is not linked with any mixer."
				"Its linked with a SrcCh",eOpType));
			return ret;
		}
		else if(BERR_SUCCESS == ret)
		{
            if(false == hMixer->bCompress)
            {
                for (path=0;path<BRAP_P_MAX_PATHS_IN_A_CHAN;path++)
                {
                    if(!(BRAP_P_IsPointerValid((void *)hRapCh->pPath[path])))
                    {
                        continue;
                    }
                    for(chPair =0 ; chPair < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; chPair++)
                    {
                        /*Storing the coefficeints in Mixing Coefficients*/
                        hRapCh->sMixingCoeff.ui32Coef[chPair*2][chPair*2] = psGain->ui32LeftGain[chPair];
                        hRapCh->sMixingCoeff.ui32Coef[chPair*2 + 1][chPair*2 + 1] = psGain->ui32RightGain[chPair];                           
                    
                        /* If channel pair present */
                        if (true == hRapCh->pPath[path]->sSrcCh[chPair].bValid)
                        {
                            BDBG_MSG(("BRAP_SetScaling: Path_id = %d",path));
                            /* Go to the SRC next to the srcch */
                            uiLvl =hRapCh->pPath[path]->sSrcCh[chPair].sSrcChOutLink.uiLevel;
                            uiPp =hRapCh->pPath[path]->sSrcCh[chPair].sSrcChOutLink.uiPrlPth;
                            eChP =hRapCh->pPath[path]->sSrcCh[chPair].sSrcChOutLink.eChnPair;


                            BDBG_MSG(("BRAP_SetScaling: Src Lvl = %d"
                                      "Prll_Path = %d, Chan_Pair = %d ",
                                      uiLvl,uiPp,eChP));
                            
                            /* for every Mixer connected to a SRC in Parallel */
                            for(i=0;i<BRAP_RM_P_MAX_RSRCS_CONNECTED_TO_SAME_SRC;i++)
                            {
                                /* If valid resource and its a mixer */
                                if(BRAP_P_Rsrc_eMixer == 
                                    hRapCh->pPath[path]->sSrc[uiLvl][eChP][uiPp].
                                                                  sSrcOutLink[i].eRsrcType)
                                {
                                    /* Get the level, pp and channel pair for the mixer */
                                    uiLvl1 = 
                                        hRapCh->pPath[path]->sSrc[uiLvl][eChP][uiPp].
                                                                    sSrcOutLink[i].uiLevel;
                                    uiPp1 = 
                                        hRapCh->pPath[path]->sSrc[uiLvl][eChP][uiPp].
                                                                    sSrcOutLink[i].uiPrlPth;
                                    eChP1 =
                                        hRapCh->pPath[path]->sSrc[uiLvl][eChP][uiPp].
                                                                    sSrcOutLink[i].eChnPair;
                                    BDBG_MSG(("BRAP_SetScaling: Mixer Lvl = %d"
                                              "Prll_Path = %d, Chan_Pair = %d ",
                                              uiLvl1,uiPp1,eChP1));
                                    
                                    if(hMixer != hRapCh->pPath[path]->sMixer[uiLvl1][eChP1][uiPp1].hMixer)
                                    {
                                        /* Not the same mixer */
                                        continue;
                                    }

                                    sTempMixerGrnt = hRapCh->pPath[path]->sRsrcGrnt.
                                        sSrcMixerGrnt[uiLvl1].sMixerGrant[eChP1][uiPp1];

                                    uiMixerIp = sTempMixerGrnt.uiMixerInputId[chPair];

                                    ret = BRAP_MIXER_P_SetGainCoeff(hMixer,uiMixerIp,
                                                               psGain->ui32LeftGain[chPair],
                                                               psGain->ui32RightGain[chPair],
                                                               0,
                                                               0);
                                    if(BERR_SUCCESS != ret)
                                    {
                                        BDBG_ERR(("Could not program the gain:"
                                            " BRAP_MIXER_P_SetGainCoeff failed"));
                                        return BERR_TRACE(ret);
                                    }
                                    /* Done programming */
                                    /* Find free location to keep coeff */
                                    for(j=0;j<BRAP_P_MAX_DST_PER_RAPCH;j++)
                                    {
                                        if((hRapCh->sScalingInfo.eOp
                                        [hMixer->uiDpIndex*hMixer->uiMixerIndex][j]
                                        == BRAP_OutputPort_eMax)||
                                        (hRapCh->sScalingInfo.eOp
                                        [hMixer->uiDpIndex*hMixer->uiMixerIndex][j]
                                        == eOpType))
                                            break;
                                    }
                                    if(BRAP_P_MAX_DST_PER_RAPCH == j)
                                    {
                                        BDBG_ERR(("No space to keep Gain coeff!!"));
                                        return BERR_INVALID_PARAMETER;
                                    }
                                    hRapCh->sScalingInfo.sScalingInfo
                                       [hMixer->uiDpIndex*hMixer->uiMixerIndex][j]
                                       = *psGain;
                                    hRapCh->sScalingInfo.eOp
                                        [hMixer->uiDpIndex*hMixer->uiMixerIndex][j]
                                        = eOpType;
                                    bGainProgrammed = true;
                                    break;
                                }
                            }
                        }                        
                        if(true == bGainProgrammed)
                            break;
                    }
                    if(true == bGainProgrammed)
                        break;
                }
            }
		}
		else
		{
			BDBG_ERR(("BRAP_SetScaling:BRAP_P_GetMixerForOpPort returned"
				"Error"));
			return ret;
		}
	}
    
	BDBG_LEAVE(BRAP_SetScaling);
	return ret;
}

/***************************************************************************
Summary:
    Gets the coefficients for controlling the Scaling of the Input audio stream.

Description:
    This function gets the mixer coefficients value.
    Note: This API returns only the gain coefficients programmed by the 
    application. 

Returns:
	BERR_SUCCESS if successful else error value.            
	
See Also:
    BRAP_SetScaling().
**************************************************************************/
BERR_Code BRAP_GetScaling(
    BRAP_ChannelHandle  hRapCh,     /* [in] Rap channel Handle */
    BRAP_GainControl    *psGain,    /* [out] Gain coefficients Structure */
    BRAP_OutputPort     eOpType     /* [in] Output Port */
)
{
    BERR_Code   ret = BERR_SUCCESS;
    unsigned int k = 0, l = 0;
    bool bFound = false;
    
    BDBG_ENTER(BRAP_GetScaling);

    BDBG_ASSERT(hRapCh);
    BDBG_ASSERT(psGain);
    BSTD_UNUSED(eOpType);

    for(l=0;l<BRAP_RM_P_MAX_MIXER_PER_DP_BLCK * BRAP_RM_P_MAX_DP_BLCK;l++)
    {
        for(k=0;k<BRAP_P_MAX_DST_PER_RAPCH;k++)
        {
            if(eOpType == hRapCh->sScalingInfo.eOp[l][k])
            {
                *psGain = hRapCh->sScalingInfo.sScalingInfo[l][k];
                bFound = true;
                break;
            }
        }
        if(true == bFound)
            break;
    }
    if(false == bFound)
    {
        BDBG_ERR(("Couldn't find gain coeff for specific output port!!"));
        ret = BERR_INVALID_PARAMETER;
    }
    else
    {
        for(k=0 ; k< BRAP_OutputChannelPair_eMax; k++)
        {
            BDBG_MSG(("psGain->ui32LeftGain[%d] = %x",k,psGain->ui32LeftGain[k]));
            BDBG_MSG(("psGain->ui32RightGain[%d] = %x",k,psGain->ui32RightGain[k]));            
        }
    }

    BDBG_LEAVE(BRAP_GetScaling);
    return ret;
}

#if BRAP_P_EQUALIZER
/***************************************************************************
Summary:
    Creates Equalizer object.
Description:
    This function creates equalizer object. This can later to tied to an output
    port to get the necessary equalizer functionality.
Returns:
    BERR_SUCCESS on success
    Error code on failure
See Also:
    BRAP_DestroyEqualizer
    BRAP_AddEqualizerToOuput
**************************************************************************/
BERR_Code BRAP_CreateEqualizer(
    BRAP_Handle hRap,                   /* [in] Device handle */
    BRAP_EqualizerHandle *phEqualizer   /* [out] Equalizer handle */
    )
{
    BERR_Code   ret = BERR_SUCCESS;
    int i;
    
    BDBG_ENTER(BRAP_CreateEqualizer);

    BDBG_ASSERT (hRap);
    BDBG_ASSERT (phEqualizer);

    *phEqualizer = NULL;
    for (i=0; i<BRAP_P_MAX_EQUALIZER_OBJECTS; i++)
    {
        if (true == hRap->sEqualizerObjs[i].bAvailable)
        {
            break;
        }
    }

    if (BRAP_P_MAX_EQUALIZER_OBJECTS == i)
    {
        BDBG_ERR(("BRAP_CreateEqualizer: Max equalizers reached"));
        ret = BERR_TRACE(BERR_NOT_SUPPORTED);
        goto exit;		
    }

    hRap->sEqualizerObjs[i].bAvailable = false;
    hRap->sEqualizerObjs[i].hRap = hRap;
    hRap->sEqualizerObjs[i].sExtEqualizerDetails = sDefEqualizerSettings;
    hRap->sEqualizerObjs[i].ui32SamplingRate = 48000;

    *phEqualizer = &(hRap->sEqualizerObjs[i]);

    BDBG_MSG(("BRAP_CreateEqualizer: Equalizer 0x%x created", *phEqualizer));
exit:    
    
    BDBG_LEAVE(BRAP_CreateEqualizer);

    return ret;
}

/***************************************************************************
Summary:
    Destroys Equalizer object.
Description:
    This function destroys equalizer object.
Returns:
    BERR_SUCCESS on success
    Error code on failure
See Also:
    BRAP_CreateEqualizer
    BRAP_AddEqualizerToOuput
**************************************************************************/
BERR_Code BRAP_DestroyEqualizer(
    BRAP_EqualizerHandle hEqualizer     /* [in] Equalizer handle */
    )
{
    BERR_Code   ret = BERR_SUCCESS;
    
    BDBG_ENTER(BRAP_DestroyEqualizer);

    BDBG_ASSERT (hEqualizer);

    BDBG_MSG(("BRAP_DestroyEqualizer: Equalizer 0x%x to be destroyed", hEqualizer));
    
    hEqualizer->sExtEqualizerDetails = sDefEqualizerSettings;
    hEqualizer->sExtEqualizerDetails.bFiveBandEqualizer = false;
    hEqualizer->hRap = NULL;
    hEqualizer->bAvailable = true;
    hEqualizer->ui32SamplingRate = 48000;
    
    BDBG_LEAVE(BRAP_DestroyEqualizer);

    return ret;
}    

/***************************************************************************
Summary:
    Add/Remove an Equalizer object to a destination
Description:
    Associate an equalizer with a destination.  
    Pass NULL for hEqualizer to remove the equalizer from this destination 
Returns:
    BERR_SUCCESS on success
    Error code on failure
See Also:
    BRAP_CreateEqualizer
    BRAP_DestroyEqualizer
**************************************************************************/
BERR_Code BRAP_SetEqualizer(
    BRAP_EqualizerHandle hEqualizer,    /* [in] Equalizer handle */
    BRAP_DestinationHandle hDstHandle   /* [in] Destination that the equalizer is to be tied with */     
    )

{
    BERR_Code   ret = BERR_SUCCESS;
    
    BDBG_ENTER(BRAP_SetEqualizer);

    BDBG_ASSERT (hEqualizer);

    /* Copy the Equalizer handle to the Destination */
    hDstHandle->hEqualizer = hEqualizer;
    
    /* Currently Equalizer is supported only on output ports */
    BDBG_MSG(("BRAP_SetEqualizer: Added Equalizer 0x%x to output port %d", 
        hEqualizer, hDstHandle->sExtDstDetails.uDstDetails.sOpDetails.eOutput[0]));
    
    BDBG_LEAVE(BRAP_SetEqualizer);

    return ret;
}    

/**********************************************************************
Summary:
    Gets the default settings of an equalizer for a given mode
    
Description:
    This function returns default settings of an equalizer for a given mode. 
    
Returns:
    BERR_SUCCESS - If function is successful.
    BERR_INVALID_PARAMETER - On bad input parameters.

See Also:
    BRAP_GetCurrentEqualizerSettings
    BRAP_SetEqualizerSettings
***********************************************************************/
BERR_Code BRAP_GetDefaultEqualizerSettings(
    BRAP_EqualizerSettings *pSettings   /* [out] Equalizer settings corresponding to eMode */
    )
{
    BERR_Code   ret = BERR_SUCCESS;
    
    BDBG_ENTER(BRAP_GetDefaultEqualizerSettings);

    BDBG_ASSERT (pSettings);


    *pSettings = sDefEqualizerSettings;
    
    BDBG_LEAVE(BRAP_GetDefaultEqualizerSettings);

    return ret;
}    
 
/**********************************************************************
Summary:
    Gets the current settings of an equalizer.
    
Description:
    This function returns current settings of an equalizer.
    
Returns:
    BERR_SUCCESS - If function is successful.
    BERR_INVALID_PARAMETER - On bad input parameters.

See Also:
    BRAP_GetDefaultEqualizerSettings
    BRAP_SetEqualizerSettings
***********************************************************************/
BERR_Code BRAP_GetCurrentEqualizerSettings(
    BRAP_EqualizerHandle hEqualizer,    /* [in] Rap handle */
    BRAP_EqualizerSettings *pSettings   /* [out] Current settings of the equalizer */
    )
{
    BERR_Code   ret = BERR_SUCCESS;
    
    BDBG_ENTER(BRAP_GetCurrentEqualizerSettings);

    BDBG_ASSERT (hEqualizer);
    BDBG_ASSERT (pSettings);

    if (false == hEqualizer->bAvailable)
    {
        *pSettings = hEqualizer->sExtEqualizerDetails;
    }
    else
    {
        /* This equalizer object is free and not allocated (or already destroyed) */
        ret = BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    
    BDBG_LEAVE(BRAP_GetCurrentEqualizerSettings);

    return ret;
}    
 
/**********************************************************************
Summary:
    Sets the equalizer settings for the corresponding equalizer
    
Description:
    This function programs current settings of an equalizer. 
    Note that the equalizer mode can be changed only when output port is in "stop" 
    state i.e. when all the channels using this output port are in "stop" condition.
    
Returns:
    BERR_SUCCESS - If function is successful.
    BERR_INVALID_PARAMETER - On bad input parameters.

See Also:
    BRAP_GetDefaultEqualizerSettings
    BRAP_GetCurrentEqualizerSettings
***********************************************************************/
BERR_Code BRAP_SetEqualizerSettings(
    BRAP_EqualizerHandle hEqualizer,        /* [in] Equalizer handle */
    const BRAP_EqualizerSettings *pSettings /* [in] Desired settings of the equalizer */
    )
{
    BERR_Code   ret = BERR_SUCCESS;
    
    BDBG_ENTER(BRAP_SetEqualizerSettings);

    BDBG_ASSERT (hEqualizer);
    BDBG_ASSERT (pSettings);

    if (false == hEqualizer->bAvailable)
    {
        hEqualizer->sExtEqualizerDetails = *pSettings;
        ret = BRAP_P_ApplyCoefficients (hEqualizer);
        if(ret != BERR_SUCCESS)
        {
            BDBG_ERR(("BRAP_SetEqualizerSettings: BRAP_P_ApplyCoefficients returned error"));
            ret = BERR_TRACE(ret);
        }
    }
    else
    {
        /* This equalizer object is free and not allocated (or already destroyed) */
        ret = BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    
    BDBG_LEAVE(BRAP_SetEqualizerSettings);

    return ret;
}    
#endif

/***************************************************************************
Summary:
	Gets default association settings.
	
Description:
	
Returns:
	BERR_SUCCESS for a successful fillup, else returns an error.
		
See Also:
	BRAP_CreateAssociation.
***************************************************************************/
BERR_Code BRAP_GetDefaultAssociationSettings(
    BRAP_Handle    hRap,                        /* [in] Raptor Device Handle */
    BRAP_AssociatedChanSettings     *pAssociatedChSettings        
                                                /* [out] Pointer to a stucture
                                                   that holds the details of 
                                                   channels to be grouped 
                                                   together */
	)
{
    BERR_Code err = BERR_SUCCESS;
    unsigned int i = 0;
    
    BDBG_ENTER(BRAP_GetDefaultAssociationSettings);
    BDBG_ASSERT(hRap);
    BDBG_ASSERT(pAssociatedChSettings);
    BSTD_UNUSED(hRap);

    for(i=0;i<BRAP_MAX_ASSOCIATED_CHANNELS_IN_GRP;i++)
    {
        pAssociatedChSettings->sChDetail[i].hRapCh = NULL;
    }

    pAssociatedChSettings->hMasterChannel = NULL;
    
    BDBG_LEAVE(BRAP_GetDefaultAssociationSettings);
    return err;
}

/***************************************************************************
Summary:
	Gets default destination details.
	
Description:
	
Returns:
	BERR_SUCCESS for a successful fillup, else returns an error.
		
See Also:
	BRAP_AddDestination.
***************************************************************************/
BERR_Code BRAP_GetDefaultDstDetails(
    BRAP_Handle    hRap,                    /* [in] Raptor Device Handle */
    BRAP_AudioDst   eAudioDst,              /* [in] Destination Type */
    BRAP_DstDetails *pDstDetails
                                            /* [out] Details of the destination 
                                               to be added to the channel group 
                                            */
	)
{
    BERR_Code err = BERR_SUCCESS;
    unsigned int i = 0;
    
    BDBG_ENTER(BRAP_GetDefaultDstDetails);
    BDBG_ASSERT(hRap);
    BDBG_ASSERT(pDstDetails);
    BSTD_UNUSED(hRap);    

    if(BRAP_AudioDst_eOutputPort == eAudioDst)
    {
        pDstDetails->eAudioDst = BRAP_AudioDst_eOutputPort;
        for(i=0;i<BRAP_OutputChannelPair_eMax;i++)
        {
            pDstDetails->uDstDetails.sOpDetails.eOutput[i] = BRAP_OutputPort_eMax;
        }
        pDstDetails->uDstDetails.sOpDetails.eAudioMode = BRAP_OutputMode_e2_0;
        pDstDetails->uDstDetails.sOpDetails.bLfeOn = false;
        pDstDetails->uDstDetails.sOpDetails.bDownmixedOpPort = false;
    }
    else if(BRAP_AudioDst_eRingBuffer == eAudioDst)
    {
        pDstDetails->eAudioDst = BRAP_AudioDst_eRingBuffer;
        pDstDetails->uDstDetails.sRBufDetails.eAudioMode = BRAP_OutputMode_e2_0;
        pDstDetails->uDstDetails.sRBufDetails.bLfeOn = false;
        pDstDetails->uDstDetails.sRBufDetails.bCompress = false;
        pDstDetails->uDstDetails.sRBufDetails.eSampleRate = BAVC_AudioSamplingRate_eUnknown;

        /* Rbuf will stop for application to consume the data */
        pDstDetails->uDstDetails.sRBufDetails.bDontPauseWhenFull = false;
        
#ifdef  RAP_AUTOMATED_UNIT_TEST        
        pDstDetails->uDstDetails.sRBufDetails.uiDelay = 0;
#endif
        for(i=0;i<BRAP_OutputChannelPair_eMax;i++)
        {
            pDstDetails->uDstDetails.sRBufDetails.uiRBufId[2*i] = BRAP_RM_P_INVALID_INDEX;
            pDstDetails->uDstDetails.sRBufDetails.uiRBufId[2*i + 1] = BRAP_RM_P_INVALID_INDEX;
            pDstDetails->uDstDetails.sRBufDetails.uiDstChId[i] = BRAP_RM_P_INVALID_INDEX;
            pDstDetails->uDstDetails.sRBufDetails.eCapPort[i] = BRAP_CapInputPort_eMax;
            pDstDetails->uDstDetails.sRBufDetails.eBufDataMode[i] = BRAP_BufDataMode_eStereoNoninterleaved;
            pDstDetails->uDstDetails.sRBufDetails.sLtOrSnglRbufSettings[i].pBufferStart = NULL;
            pDstDetails->uDstDetails.sRBufDetails.sLtOrSnglRbufSettings[i].uiSize = BRAP_RBUF_P_DEFAULT_SIZE;
            pDstDetails->uDstDetails.sRBufDetails.sLtOrSnglRbufSettings[i].uiWaterMark = BRAP_RBUF_P_DEFAULT_FREE_BYTE_MARK;
            pDstDetails->uDstDetails.sRBufDetails.sRtRbufSettings[i].pBufferStart = NULL;
            pDstDetails->uDstDetails.sRBufDetails.sRtRbufSettings[i].uiSize = BRAP_RBUF_P_DEFAULT_SIZE;
            pDstDetails->uDstDetails.sRBufDetails.sRtRbufSettings[i].uiWaterMark = BRAP_RBUF_P_DEFAULT_FREE_BYTE_MARK;
        }
    }
    else
    {
        BDBG_ERR(("BRAP_GetDefaultDstDetails: Must Specify either output port of ring buffer!"));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    
    BDBG_LEAVE(BRAP_GetDefaultDstDetails);
    return err;
}

#if (DEBUG_RBUF_READ == 1)
/**********************************
API to capture Ringbuffer. 
before using  this Srcch should be disabled
*********************************/
void BRAP_ReadRbuf(BRAP_Handle hRap)
{
    unsigned int uiOffset=0,data =0;
    unsigned int i = 0,j =0;
    unsigned int ui32Base=0, ui32End =0,uiSize =0,ui32RegVal=0,ui32MaskWr,ui32MaskRd,ui32Write,ui32Read,ui32DataAvail;
    bool    bWrWrap,bRdWrap;
    FILE *fp;    
    void  *pui32LogicalAddr=NULL;
    const char FileName[16][80] =
    {
        {"Rbuf0.txt"},
        {"Rbuf1.txt"},
        {"Rbuf2.txt"},
        {"Rbuf3.txt"},
        {"Rbuf4.txt"},
        {"Rbuf5.txt"},
        {"Rbuf6.txt"},
        {"Rbuf7.txt"},
        {"Rbuf8.txt"},
        {"Rbuf9.txt"},
        {"Rbuf10.txt"},
        {"Rbuf11.txt"},
        {"Rbuf12.txt"},
        {"Rbuf13.txt"},
        {"Rbuf14.txt"},
        {"Rbuf15.txt"}            
    };

    while(1)
    {
        for(i =0 ; i < 5 ; i++)
        {
            ui32Base=0;
            ui32End =0;
            uiSize =0;    
            uiOffset = 24*i;
            ui32Base = BRAP_Read32(hRap->hRegister,BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_BASEADDR + uiOffset);
            if(ui32Base != 0)
            {
                BDBG_MSG(("ui32Base%d =%d",i,ui32Base));
                fp=NULL;
                fp = fopen(FileName[i],"a");                
                ui32End =  BRAP_Read32(hRap->hRegister,BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_ENDADDR + uiOffset);
                uiSize = ui32End - ui32Base;

                ui32Write =  BRAP_Read32(hRap->hRegister,BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_WRADDR + uiOffset);    
                bWrWrap = (ui32Write&0x80000000) >>31;
                ui32Read =  BRAP_Read32(hRap->hRegister,BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR + uiOffset);   
                bRdWrap = (ui32Read&0x80000000) >>31;       

                ui32MaskWr = ui32Write&0x7FFFFFFF;
                ui32MaskRd = ui32Read&0x7FFFFFFF;    

                if(bWrWrap ==bRdWrap)
                {
                    ui32DataAvail= ui32MaskWr -ui32MaskRd;
                }
                else
                {
                    ui32DataAvail= uiSize - (ui32MaskRd -ui32MaskWr);
                }
                BRAP_ConvertOffsetToAddress(hRap->hHeap,ui32MaskRd,(void *)(&pui32LogicalAddr));/*Physical Address */    
                ui32RegVal = ui32Read;
                if(ui32DataAvail >= 1024*4)
                {
                    for(j =0 ; j< 1024 ;j++)
                    {
                        data = BRAP_P_DRAMREAD((unsigned int)pui32LogicalAddr + 4*j);
                        fwrite (&data, 4, 1, fp);
                        /*                         fputc("\n",fp);*/
                        if(ui32MaskRd+4 > ui32End)
                        {
                            ui32RegVal = ui32Base | ((bRdWrap<<31)^ 0x80000000);
                        }
                        else
                        {
                            ui32RegVal+=4;  
                        }                    
                    }
                    BRAP_Write32(hRap->hRegister,BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR + uiOffset,ui32RegVal);
                    BDBG_MSG(("Writing %#x to Ring Buffer",ui32RegVal,i));

                }

                fclose(fp);            
            }
        }    
    }
}
#endif

/************************************************************************
Summary: Returns the sum of following errors
        PTS_Error + Crc_Error + Dec_unlock +TSM_Fail
************************************************************************/

BERR_Code   BRAP_GetTotalErrorCount(BRAP_ChannelHandle  hRapCh,unsigned int *uiTotalCount)
{
    BDBG_ASSERT(hRapCh);
    *uiTotalCount = hRapCh->uiTotalErrorCount;
    return  BERR_SUCCESS;
}

/***************************************************************************
Summary:
    Sets the ramping step size for volume change for all output ports.

Description:
The mixer output volume value is changed by this step size every fs during ramping. 
The 23 LSB of this field is the fractional part and the [26:23] MSB is the integer part. 
This step range from 16 to 0.  bit[27] should be set to zero.
Its range is 0  to 0x7FFFFFF

To have a smooth ramping minimum of 1024 samples are required, 
for 48khz sample rate this accounts to 21.33ms ramp time so any ramp step size 
below 0x2000 will provide smooth ramping.

Default value is 0xA00 (Ramp time ~68ms)
    
Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    BRAP_GetMixerRampStepSize().
    
**************************************************************************/
BERR_Code BRAP_SetMixerRampStepSize ( 
    BRAP_Handle     hRap,       /* [in] Audio Device Handle */
    unsigned int    uiRampStep  /* [in] Ramp Step size  */
)
{
    uint32_t ui32RegVal = 0;


    BDBG_ASSERT (hRap);
    BDBG_ENTER (BRAP_SetMixerRampStepSize);

    ui32RegVal |= (BCHP_FIELD_DATA (    
                        AUD_FMM_DP_CTRL0_VOLUME_RAMP_STEP, 
                        VOLUME_RAMP_STEP, 
                        uiRampStep));     

    BRAP_Write32 (hRap->hRegister, BCHP_AUD_FMM_DP_CTRL0_VOLUME_RAMP_STEP, ui32RegVal);
    
    BDBG_MSG (("Ramp step size written to Reg =0x%x",
                ui32RegVal));           

    hRap->uiMixerRampStepSize = uiRampStep;
    
    BDBG_LEAVE (BRAP_SetMixerRampStepSize);
    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
    Gets the ramping step size for volume change 

Description:
    This function returns ramping step size for scale and volume change.
     Its range is 0  to 0x7FFFFFF
    
Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    BRAP_SetMixerRampStepSize().
    
**************************************************************************/
BERR_Code BRAP_GetMixerRampStepSize( 
    BRAP_Handle     hRap,       /* [in] Audio Device Handle */
    unsigned int *  pRampStepSize /* [out] Pointer to memory where ramp size should be stored. 
                                         */
)
{  
    BDBG_ENTER (BRAP_GetMixerRampStepSize);
    BDBG_ASSERT (hRap);
    BDBG_ASSERT(pRampStepSize);    
         
    *pRampStepSize = hRap->uiMixerRampStepSize;

    BDBG_LEAVE (BRAP_GetMixerRampStepSize);
    return BERR_SUCCESS;
}
#if (BRAP_7550_FAMILY != 1)
/***************************************************************************
Summary:
    Sets the ramping step size for scale change for all output ports.

Description:
The scaling value is changed by this step size every fs during ramping. 
The 15 LSB of this field is the is fractional part and MSB is integer part.  
The maximum step size is 0x8000 (1).
Its range is 0  to 0x8000

To have a smooth ramping minimum of 1024 samples are required, 
for 48khz sample rate this accounts to 21.33ms ramp time so any ramp step size 
below 0x2000 will provide smooth ramping.

Default value is 0xA00 (Ramp time ~68ms)
    
Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    BRAP_GetSrcRampStepSize().
    
**************************************************************************/
BERR_Code BRAP_SetSrcRampStepSize ( 
    BRAP_Handle     hRap,       /* [in] Audio Device Handle */
    unsigned int    uiRampStep  /* [in] Ramp Step size  */
)
{
    uint32_t ui32RegVal = 0;


    BDBG_ASSERT (hRap);
    BDBG_ENTER (BRAP_SetSrcRampStepSize);

    ui32RegVal |= (BCHP_FIELD_DATA (    
                        AUD_FMM_SRC_CTRL0_RAMP_STEP, 
                        STEP_SIZE, 
                        uiRampStep));     

    BRAP_Write32 (hRap->hRegister, BCHP_AUD_FMM_SRC_CTRL0_RAMP_STEP, ui32RegVal);
    
    BDBG_MSG (("Ramp step size written to Reg =0x%x",
                ui32RegVal));           

    hRap->uiSrcRampStepSize= uiRampStep;
    
    BDBG_LEAVE (BRAP_SetSrcRampStepSize);
    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
    Gets the ramping step size for scale change 

Description:
    This function returns ramping step size for scale and volume change.
Its range is 0  to 0x8000
    
Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    BRAP_SetSrcRampStepSize().
    
**************************************************************************/
BERR_Code BRAP_GetSrcRampStepSize( 
    BRAP_Handle     hRap,       /* [in] Audio Device Handle */
    unsigned int *  pRampStepSize /* [out] Pointer to memory where ramp size should be stored. 
                                          */
)
{  
    BDBG_ENTER (BRAP_GetSrcRampStepSize);
    BDBG_ASSERT (hRap);
    BDBG_ASSERT(pRampStepSize);      
         
    *pRampStepSize = hRap->uiSrcRampStepSize;

    BDBG_LEAVE (BRAP_GetSrcRampStepSize);
    return BERR_SUCCESS;
}

#endif
/***************************************************************************
Summary:
    Sets the ramping step size for Ping Pong coefficients for all mixers.

Description:
The Ping coefficients value is changed by this step size every fs during ramping. 
The 23 LSB of this field is the fractional part and the bit[26:23] MSB is the integer part. T
his step ranges from 16 to 0.  Bit [27] should always set to zero.
0x80_0000 = 1
0x40_0000 = 0.5

default is 0x00100000

Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    BRAP_GetSrcRampStepSize().
    
**************************************************************************/
BERR_Code BRAP_SetScalingCoefRampStepSize ( 
    BRAP_Handle     hRap,       /* [in] Audio Device Handle */
    unsigned int    uiRampStep  /* [in] Ramp Step size  */
)
{
    unsigned int    i=0;

    BDBG_ASSERT (hRap);
    BDBG_ENTER (BRAP_SetScalingCoefRampStepSize);

    for(i=0; i<=BCHP_AUD_FMM_DP_CTRL0_PING_COEFF_RAMP_STEPi_ARRAY_END; i++)
    {
        BRAP_Write32 (hRap->hRegister,
            BCHP_AUD_FMM_DP_CTRL0_PING_COEFF_RAMP_STEPi_ARRAY_BASE + (4 * i),
            uiRampStep);
        BRAP_Write32 (hRap->hRegister,
            BCHP_AUD_FMM_DP_CTRL0_PONG_COEFF_RAMP_STEPi_ARRAY_BASE + (4 * i),
            uiRampStep);
    }
           
    hRap->uiScalingCoefRampStepSize= uiRampStep;
    
    BDBG_LEAVE (BRAP_SetScalingCoefRampStepSize);
    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
    Gets the ramping step size for scale change 

Description:
    This function returns ramping step size for ping/pong coefficents
Its range is 0  to 0x8000
    
Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    BRAP_SetSrcRampStepSize().
    
**************************************************************************/
BERR_Code BRAP_GetScalingCoefRampStepSize( 
    BRAP_Handle     hRap,       /* [in] Audio Device Handle */
    unsigned int *  pRampStepSize /* [out] Pointer to memory where ramp size should be stored. */
)
{  
    BDBG_ENTER (BRAP_GetScalingCoefRampStepSize);
    BDBG_ASSERT (hRap);
    BDBG_ASSERT(pRampStepSize);
         
    *pRampStepSize = hRap->uiScalingCoefRampStepSize;

    BDBG_LEAVE (BRAP_GetScalingCoefRampStepSize);
    return BERR_SUCCESS;
}
BERR_Code BRAP_FlushPbChannel (
	BRAP_ChannelHandle	        hRapCh		/* [in] The PB Channel handle */
	)
{
    BERR_Code   ret=BERR_SUCCESS;
    unsigned int i=0,j=0;
    uint32_t    ui32RegVal=0,ui32BaseAddr=0,uiTimeOutCtr=0;
    BDBG_ASSERT(hRapCh);
    BDBG_ENTER(BRAP_FlushPbChannel);
    if(hRapCh->eChannelType != BRAP_ChannelType_ePcmPlayback)
    {
        BDBG_ERR(("This API is only supported for playback channel"));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    /*Disable all the SrcCh associated with this Channel*/
    for(i = 0 ; i < BRAP_P_MAX_PATHS_IN_A_CHAN ; i++)
    {
        if(hRapCh->pPath[i] ==NULL)
            continue;
        for(j =0 ; j < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS ; j++)
        {
            if((hRapCh->pPath[i]->sSrcCh[j].bValid == true)
                &&(hRapCh->pPath[i]->sSrcCh[j].hSrcCh != NULL))
                {
                        BKNI_EnterCriticalSection();                
                    /*Set PLAY_RUN=0, and optionally wait for GROUP_FLOWON=0 to confirm.*/
                        ui32RegVal = 0;
                        ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_BF_CTRL_SOURCECH_CTRLi, PLAY_RUN, 0));
                        BRAP_Write32_isr(hRapCh->hRegister, BCHP_AUD_FMM_BF_CTRL_SOURCECH_CTRLi_ARRAY_BASE + hRapCh->pPath[i]->sSrcCh[j].hSrcCh->ui32Offset, ui32RegVal);
                        BKNI_LeaveCriticalSection();    
                	uiTimeOutCtr = 100 /*BRAP_P_STOP_TIMEOUT_COUNTER*/;
                	do
                	{
                		if(100 != uiTimeOutCtr)
                		{
                			BKNI_Sleep(1);
                		}
                		ui32RegVal = BRAP_Read32(hRapCh->hRegister,BCHP_AUD_FMM_BF_CTRL_SOURCECH_GROUP_FLOWON);
                                BDBG_MSG(("FLOW ON=%d, hSrcCh->uiIndex =%d",ui32RegVal,hRapCh->pPath[i]->sSrcCh[j].hSrcCh->uiIndex));
                		ui32RegVal = BCHP_GET_FIELD_DATA(ui32RegVal, AUD_FMM_BF_CTRL_SOURCECH_GROUP_FLOWON, STATUS);
                		ui32RegVal &= (1<<hRapCh->pPath[i]->sSrcCh[j].hSrcCh->uiIndex);
                		uiTimeOutCtr--;		
                	}while((0!=ui32RegVal) && (0!=uiTimeOutCtr));

                	if(0!=ui32RegVal)
                	{
                		BDBG_ERR(("AUD_FMM_BF_CTRL_SOURCECH_GROUP_FLOWON=%#x is not disabled. [Time out]",ui32RegVal));
                	}

                    BKNI_EnterCriticalSection();                
                    ui32RegVal = BRAP_Read32_isr (hRapCh->hRegister, BCHP_AUD_FMM_BF_CTRL_SOURCECH_CFGi_ARRAY_BASE + hRapCh->pPath[i]->sSrcCh[j].hSrcCh->ui32Offset);
                    ui32RegVal &= ~(BCHP_MASK (AUD_FMM_BF_CTRL_SOURCECH_CFGi, SOURCEFIFO_ENABLE));
                    ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_BF_CTRL_SOURCECH_CFGi, SOURCEFIFO_ENABLE, 0));
                    BRAP_Write32_isr (hRapCh->hRegister, BCHP_AUD_FMM_BF_CTRL_SOURCECH_CFGi_ARRAY_BASE + hRapCh->pPath[i]->sSrcCh[j].hSrcCh->ui32Offset, ui32RegVal);
                    BKNI_LeaveCriticalSection();                        
                }
        }
    }

    BKNI_EnterCriticalSection();                
    /*Reset Rd/WR Ptr of Rbufs*/
    for(i = 0 ; i < BRAP_P_MAX_PATHS_IN_A_CHAN ; i++)
    {
        if(hRapCh->pPath[i] ==NULL)
            continue;
        for(j =0 ; j < BRAP_RM_P_MAX_OP_CHANNELS ; j++)
        {
            if(hRapCh->pPath[i]->hRBuf[j] != NULL)
            {
                ui32BaseAddr = BRAP_Read32_isr (hRapCh->hRegister,
                                                                BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_BASEADDR + 
                                                                hRapCh->pPath[i]->hRBuf[j]->ui32Offset);

                BRAP_Write32_isr (hRapCh->hRegister,  
                                        BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_WRADDR + hRapCh->pPath[i]->hRBuf[j]->ui32Offset, 
                                        ui32BaseAddr);
                BRAP_Write32_isr (hRapCh->hRegister,  
                                        BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR + hRapCh->pPath[i]->hRBuf[j]->ui32Offset, 
                                        ui32BaseAddr);    
            }
        }
    }

    /*Enable all the SrcCh associated with this Channel*/
    for(i = 0 ; i < BRAP_P_MAX_PATHS_IN_A_CHAN ; i++)
    {
        if(hRapCh->pPath[i] ==NULL)
            continue;
        for(j =0 ; j < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS ; j++)
        {
            if((hRapCh->pPath[i]->sSrcCh[j].bValid == true)
                &&(hRapCh->pPath[i]->sSrcCh[j].hSrcCh != NULL))
                {
                    ui32RegVal = BRAP_Read32_isr (hRapCh->hRegister, BCHP_AUD_FMM_BF_CTRL_SOURCECH_CFGi_ARRAY_BASE + hRapCh->pPath[i]->sSrcCh[j].hSrcCh->ui32Offset);
                    ui32RegVal &= ~(BCHP_MASK (AUD_FMM_BF_CTRL_SOURCECH_CFGi, SOURCEFIFO_ENABLE));
                    ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_BF_CTRL_SOURCECH_CFGi, SOURCEFIFO_ENABLE, 1));
                    BRAP_Write32_isr (hRapCh->hRegister, BCHP_AUD_FMM_BF_CTRL_SOURCECH_CFGi_ARRAY_BASE + hRapCh->pPath[i]->sSrcCh[j].hSrcCh->ui32Offset, ui32RegVal);

                    /*Set PLAY_RUN=1, */
                        ui32RegVal = 0;
                        ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_BF_CTRL_SOURCECH_CTRLi, PLAY_RUN, 1));
                        BRAP_Write32_isr(hRapCh->hRegister, BCHP_AUD_FMM_BF_CTRL_SOURCECH_CTRLi_ARRAY_BASE + hRapCh->pPath[i]->sSrcCh[j].hSrcCh->ui32Offset, ui32RegVal);
                    
                }
        }
    }    
BKNI_LeaveCriticalSection();               
    BDBG_LEAVE(BRAP_FlushPbChannel);    
    return ret;    
}
#ifdef BCHP_AUD_FMM_MISC_HIFIOUT_SEL 
#if (BRAP_MAX_RFMOD_OUT > 0)
BERR_Code BRAP_GetCurrentRfModSource(BRAP_Handle hRap,unsigned int uiRfModIndex, BRAP_OutputPort    *eSource)
{
    BDBG_ENTER(BRAP_GetCurrentRfModSource);
    BDBG_ASSERT(hRap);
    
    if(uiRfModIndex >= BRAP_MAX_RFMOD_OUT)
    {
        BDBG_ERR(("Invalid  RFMOD  Index %d",uiRfModIndex));
        return BERR_INVALID_PARAMETER;         
    }
    
    *eSource = hRap->eRfModSource[uiRfModIndex];
    
    BDBG_LEAVE(BRAP_GetCurrentRfModSource);    
    return  BERR_SUCCESS;
}

BERR_Code BRAP_SetRfModSource(BRAP_Handle hRap,unsigned int uiRfModIndex, BRAP_OutputPort    eSource)
{
    uint32_t     ui32RegVal;
    unsigned    int uiTemp;
    
    BDBG_ENTER(BRAP_SetRfModSource);
    BDBG_ASSERT(hRap);
    

    if(uiRfModIndex >= BRAP_MAX_RFMOD_OUT)
    {
        BDBG_ERR(("Invalid  RFMOD  Index %d",uiRfModIndex));
        return BERR_INVALID_PARAMETER;         
    }

    switch(eSource)
    {
        case BRAP_OutputPort_eDac0:
            uiTemp=0;
            break;
        case BRAP_OutputPort_eDac1:
            uiTemp=1;
            break;            
        default:
            BDBG_ERR(("Invalid Source of RFMOD %d",eSource));
            return BERR_INVALID_PARAMETER;
    }

    ui32RegVal = BRAP_Read32(hRap->hRegister,BCHP_AUD_FMM_MISC_HIFIOUT_SEL);
    
    switch(uiRfModIndex)        
    {
        case 0:
            ui32RegVal &= ~(BCHP_MASK (AUD_FMM_MISC_HIFIOUT_SEL, RFMOD0_SEL));
            ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_MISC_HIFIOUT_SEL, RFMOD0_SEL,uiTemp));
            break;
        case 1:
            ui32RegVal &= ~(BCHP_MASK (AUD_FMM_MISC_HIFIOUT_SEL, RFMOD1_SEL));
            ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_MISC_HIFIOUT_SEL, RFMOD1_SEL,uiTemp));            
            break;            
        default:
            BDBG_ERR(("Invalid  RFMOD  Index %d",uiRfModIndex));
            return BERR_INVALID_PARAMETER;            
    }
    hRap->eRfModSource[uiRfModIndex]= eSource;
    
    BRAP_Write32(hRap->hRegister, BCHP_AUD_FMM_MISC_HIFIOUT_SEL, ui32RegVal);
    
    BDBG_LEAVE(BRAP_SetRfModSource);    
    return  BERR_SUCCESS;
}
#else
BERR_Code BRAP_GetCurrentRfModSource(BRAP_Handle hRap,unsigned int uiRfModIndex, BRAP_OutputPort    *eSource)
{
    BSTD_UNUSED(hRap);
    BSTD_UNUSED(uiRfModIndex);
    BSTD_UNUSED(eSource);    
    BDBG_WRN(("Initialize the Value of BRAP_MAX_RFMOD_OUT in RAP PI "));
    return  BERR_SUCCESS;
}
BERR_Code BRAP_SetRfModSource(BRAP_Handle hRap,unsigned int uiRfModIndex, BRAP_OutputPort    eSource)
{
    BSTD_UNUSED(hRap);
    BSTD_UNUSED(uiRfModIndex);
    BSTD_UNUSED(eSource);    
    BDBG_WRN(("Initialize the Value of BRAP_MAX_RFMOD_OUT in RAP PI "));    
    return  BERR_SUCCESS;
}
#endif
#else/*Do nothing If Register is not defined*/
BERR_Code BRAP_GetCurrentRfModSource(BRAP_Handle hRap,unsigned int uiRfModIndex, BRAP_OutputPort    *eSource)
{
    BSTD_UNUSED(hRap);
    BSTD_UNUSED(uiRfModIndex);
    BSTD_UNUSED(eSource);    
    return  BERR_SUCCESS;
}
BERR_Code BRAP_SetRfModSource(BRAP_Handle hRap,unsigned int uiRfModIndex, BRAP_OutputPort    eSource)
{
    BSTD_UNUSED(hRap);
    BSTD_UNUSED(uiRfModIndex);
    BSTD_UNUSED(eSource);    
    return  BERR_SUCCESS;
}
#endif



void BRAP_GetCapabilities(
    BRAP_Handle hRap, 
    BRAP_Capabilities *pCapabilities  /* [out] */ 
    )
{
    unsigned i=0;
    BSTD_UNUSED(hRap);
    for(i=0;i<BRAP_DSPCHN_AudioType_eMax;i++)
    {
        pCapabilities->bDecodeSupported[i] = BRAP_FWDWNLD_P_IsAudCodecSupported(i);
    }

    for(i=0;i<BRAP_ProcessingType_eMax;i++)
    {
        if(BRAP_sMapProcessingEnum[i].eEnumType == BRAP_CIT_P_AudioAlgoType_ePostProc)            
            pCapabilities->bProcessingSupported[i] = BRAP_FWDWNLD_P_IsAudProcSupported(BRAP_sMapProcessingEnum[i].uEnumName.eProcessingType);
        else if(BRAP_sMapProcessingEnum[i].eEnumType == BRAP_CIT_P_AudioAlgoType_eEncode)            
            pCapabilities->bProcessingSupported[i] = BRAP_FWDWNLD_P_IsEncodeSupported(BRAP_sMapProcessingEnum[i].uEnumName.eEncodeType);        
        else
            pCapabilities->bProcessingSupported[i] = false;
    }    
    return;
}

/***************************************************************************
Summary:
	Returns current audio rate of the Raptor Channel.

Description:
	This function returns current audio rate of the Raptor Channel.
	Valid only for decode channel

Returns:
	BERR_SUCCESS - if passed parameters are correct.

See Also:
	BRAP_SetAudioRate
***************************************************************************/
BERR_Code BRAP_GetAudioPlayRate(
	BRAP_ChannelHandle		    hRapCh,		        /* [in] RAP channel handle */
	unsigned int                            *uiPBRate 	/* [out] Current audio rate*/
	)
{
    BDBG_ASSERT(hRapCh);
    *uiPBRate = hRapCh->uiPBRate;
    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
	Sets  audio rate of the Raptor Channel on the fly.

Description:
	This function sets  audio rate of the Raptor Channel on the fly. This API should be called only 
	1) when decode Channel is in started case, 
	2) the decode channel has been started with non-1x rate.
	Valid only for decode channel

Returns:
	BERR_SUCCESS - if passed parameters are correct.

See Also:
	BRAP_GetAudioRate
***************************************************************************/

BERR_Code BRAP_SetAudioPlayRate(
	BRAP_ChannelHandle		    hRapCh,		        /* [in] RAP channel handle */
	unsigned int                            uiPBRate 	/* [in]  audio rate*/
	)
{
	BERR_Code               err = BERR_SUCCESS;
    unsigned int i=0,j=0;
    bool    bFound = false;
    BRAP_ProcessingStageSettings    *psProcessingStageSettings=NULL;
    BDBG_ENTER(BRAP_SetAudioPlayRate);
    BDBG_ASSERT(hRapCh);
    if(hRapCh->eState != BRAP_P_State_eStarted)
    {
        BDBG_ERR(("BRAP_SetAudioRate should be called after Channel Start"));
        return BERR_INVALID_PARAMETER;
    }
    for(i = 0 ;i < BRAP_P_MAX_DST_PER_RAPCH;i++)
    {
        for(j = 0;j < BRAP_MAX_STAGE_PER_BRANCH_SUPPORTED; j++)
        {    
            if((hRapCh->sChAudProcessingStage[i][j].hAudioProcessing !=NULL )
                &&(hRapCh->sChAudProcessingStage[i][j].hAudioProcessing->sProcessingStageSettings.eAudioProcessing == BRAP_ProcessingType_eDSOLA))
            {
                bFound = true;
                break;
            }
        }
        if(bFound == true)
            break;
    }

    if(bFound == false)
    {
        BDBG_ERR(("BRAP_SetAudioRate: DSOLA is not enabled while channel Start, So playback rate can not be changed on the fly"));
        return BERR_INVALID_PARAMETER;        
    }
    psProcessingStageSettings = (BRAP_ProcessingStageSettings    *)BKNI_Malloc(sizeof(BRAP_ProcessingStageSettings));
    if( NULL==psProcessingStageSettings)
    {
        BDBG_ERR(("Unable allocate memory for psProcessingSettings "));                     
        return BERR_OUT_OF_SYSTEM_MEMORY;                             
    }
    BDBG_ERR(("uiPBRate = %d",uiPBRate));
    hRapCh->uiPBRate = uiPBRate;    
    err = BRAP_GetCurrentProcessingStageSettings(hRapCh->sChAudProcessingStage[i][j].hAudioProcessing,
                                                        psProcessingStageSettings);  

	if(err != BERR_SUCCESS)
    {
    	BDBG_ERR(("BRAP_SetAudioPlayRate: BRAP_GetCurrentProcessingStageSettings() returned err(%d)",err));	
    	goto exit;
    }            
    err = BRAP_SetProcessingStageSettings(hRapCh->sChAudProcessingStage[i][j].hAudioProcessing,
                                                        psProcessingStageSettings);
	if(err != BERR_SUCCESS)
    {
        BDBG_ERR(("BRAP_SetAudioPlayRate: BRAP_SetProcessingStageSettings() returned err(%d)",err));	
        goto exit;
    } 

	exit:
	if(psProcessingStageSettings)
        BKNI_Free(psProcessingStageSettings);

	BDBG_LEAVE(BRAP_SetAudioPlayRate);    
    return err;
}



