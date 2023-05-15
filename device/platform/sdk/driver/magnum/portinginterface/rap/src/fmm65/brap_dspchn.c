/***************************************************************************
*     Copyright (c) 2006-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brap_dspchn.c $
* $brcm_Revision: Hydra_Software_Devel/503 $
* $brcm_Date: 7/26/12 2:31p $
*
* Module Description:
*	This file contains high level functions related to DSP and firmware.
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/rap/base3/brap_dspchn.c $
* 
* Hydra_Software_Devel/503   7/26/12 2:31p purush
* SW7550-799: [7550] Adding PI support for AVL 2.0. Updating default AVL
* config values.
* 
* Hydra_Software_Devel/502   2/9/12 10:13p sgadara
* SW7420-2230: [7420] Fix memory leaks
* 
* Hydra_Software_Devel/501   2/8/12 2:03p sgadara
* SW7420-2230: [7420] Change the Feedback buffer from circular to linear
* 
* Hydra_Software_Devel/500   2/1/12 8:11p sgadara
* SW7420-2230-2216: [7420,7125] Update DDRE user configuraion parameters
* structure. And roll back SRS TVOL back to version 1.2.3.2
* 
* Hydra_Software_Devel/499   12/16/11 9:33p srajapur
* SW7125-1178:[7125] Fixing Unknown Acmod for wma algorithm
* 
* Hydra_Software_Devel/498   11/25/11 4:08p srajapur
* SW7420-1898 : [7420] Reverting changes done to avoid the download of
* PCM algorithm binaries during loopback in case of MS11
* 
* Hydra_Software_Devel/497   11/17/11 7:59p srajapur
* SW7420-2131:[7420] - MS11 - DDRE - expose userconfigs for external pcm
* mode
* 
* Hydra_Software_Devel/496   11/14/11 4:08p srajapur
* SW7420-2074 : [7420] Adding support for MpegConformanceMode
* 
* Hydra_Software_Devel/495   11/8/11 2:00p srajapur
* SW7125-1117 : [7125] Added a BTSC user config parameter to set the
* output mode to mono or stereo.
* 
* Hydra_Software_Devel/494   11/4/11 2:50p srajapur
* SW7335-1371 : [7335] Fixing the Kernel oops in
* BRAP_DSPCHN_P_GetDspChHandle
* 
* Hydra_Software_Devel/493   10/10/11 8:48p srajapur
* SW7125-1117 : [7125] Added separate volume controls for L+R and L-R
* data in BTSC Encoder
* 
* Hydra_Software_Devel/492   8/25/11 11:15a srajapur
* SW7420-2019: [7420] 7420 - Corrected the downmix path cut/boost values
* settings.
* 
* Hydra_Software_Devel/491   8/24/11 5:15p srajapur
* SW7405-5456 : [7405] Corrected the wmastd and wmapro enum values.
* 
* Hydra_Software_Devel/490   8/4/11 11:23a srajapur
* SW7420-1898 : [7420] Avoiding the download of PCM algorithm binaries
* during loopback in case of MS11
* 
* Hydra_Software_Devel/489   7/1/11 6:22p srajapur
* SW7550-755 : [7550] Adding flags for control the output FM deviation
* levels.
* 
* Hydra_Software_Devel/488   6/29/11 2:22p srajapur
* SW7420-1748: [7420] xxx_isr API crashes in RAP, XVD and VDC due to
* multi-threading in SMP
* 
* Hydra_Software_Devel/487   6/17/11 1:53p gautamk
* SW7405-5165: [7405] Enabling mode change interrupt in default
* 
* Hydra_Software_Devel/486   6/2/11 5:50p gautamk
* SW7405-4673: [7405] Changes for userconfig for ms11 dolby pulse.
* 
* Hydra_Software_Devel/485   5/25/11 3:14p gautamk
* SW7420-767: [7420] DDRE stage can be added on single decode. Adding
* support for it
* 
* Hydra_Software_Devel/484   5/16/11 5:31p gautamk
* SW7125-933: [7125] Fixing compile warning related to type cast.
* 
* Hydra_Software_Devel/483   5/3/11 10:58a sgadara
* SW7420-1756: [7420] Modify DDRE user config to use enums
* 
* Hydra_Software_Devel/482   4/20/11 3:42p sgadara
* SW7420-1756: [7420] Implementing code review comments --> Change the
* Pointer validity check function to a macro and name it aptly
* 
* Hydra_Software_Devel/481   4/19/11 9:25p sgadara
* SW7420-1756: [7420] Correct DDRE user config parameters
* 
* Hydra_Software_Devel/480   4/18/11 3:04p sgadara
* SW7420-1756: [7420] Implementing code review comments and correcting
* the indentations
* 
* Hydra_Software_Devel/479   4/6/11 3:20p gautamk
* SW7125-901: [7125] MAI channel Status bit should be picked from
* different structure.
* 
* Hydra_Software_Devel/478   3/29/11 7:59p gautamk
* SW7335-1205: [7335] Correcting default outmode for WMA
* 
* Hydra_Software_Devel/477   3/18/11 11:06p sgadara
* SW7420-767: [7420] Fixing non-MS build issue
* 
* Hydra_Software_Devel/476   3/18/11 9:40p sgadara
* SW7420-767: [7420] Merge MS11 development branch to mainline
* 
* Hydra_Software_Devel/475   3/7/11 6:03p pothiraj
* SW7550-648: [7550] Secure Code Module feature
* 
* Hydra_Software_Devel/474   3/4/11 6:24p sgadara
* SW3548-3136: [3548,3556] Add new user configuraiton parameter for SRS
* Studio Sound. -->Modules affected, SRS TVOL(High Pass Filter),
* EQHL(PEQ, High Pass Filter) and InputGain range of GEQ and default
* Bypass gain of TSHD changed.
* 
* Hydra_Software_Devel/473   2/18/11 12:16p gautamk
* SW7550-601: [7550] Changing the unit of Attenuation factor to Q5.23
* 
* Hydra_Software_Devel/472   2/11/11 6:56p gautamk
* SW7550-601: [7550] Changing the unit of Attenuation factor to DB
* 
* Hydra_Software_Devel/471   2/11/11 5:26p gautamk
* SW7405-4713: [7405] Modifying user config for dolby pulse
* 
* Hydra_Software_Devel/470   2/7/11 1:58p gautamk
* SW7550-631: [7550] Sample rate value and mode value in
* Getstreaminformation() is taken from payload sent by FW through
* interrupt.
* 
* Hydra_Software_Devel/469   2/1/11 5:04p gautamk
* SW7550-637: [7550] Adding PI support for multiple sampling frequency
* support.
* 
* Hydra_Software_Devel/468   1/12/11 3:57p bselva
* SW7468-346: [7468]Fixed the memory requirement calculation for video FW
* files.
* 
* Hydra_Software_Devel/468   1/12/11 3:55p bselva
* SW7468-346: [7468]Fixed the memory requirement calculation for video FW
* files.
* 
* Hydra_Software_Devel/467   11/9/10 6:01p gautamk
* SW7335-931: [7335] Fixing warning msg
* 
* Hydra_Software_Devel/466   10/25/10 6:28p sgadara
* SW3556-1206: [3548,3556] Merge customer branch to main line -->
* Removing the param from AppFormat strcuture as there is no format
* conversion before passing it on to FW
* 
* Hydra_Software_Devel/465   10/22/10 8:29p sgadara
* SW3556-1206: [3548,3556] Merge customer branch to main line
* 
* Hydra_Software_Devel/464   10/21/10 7:08p srajapur
* SW7550-601 : [7550] Adding PI Support for BTSC Encoder
* 
* Hydra_Software_Devel/463   10/21/10 4:48p sgadara
* SW3548-2923 : [3548,3556] Increasing the scaling factor for few  params
* of SRS TVOL 2.1 --> Porting to main line
* 
* Hydra_Software_Devel/462   10/18/10 4:25p gautamk
* SW7405-4713: [7405] Incrementing uint will only increment by 1. It
* should be uint *
* 
* Hydra_Software_Devel/461   10/1/10 7:28p srajapur
* SW7468-298 :[7468]Merging Real Video Code to main latest
* 
* Hydra_Software_Devel/460   10/1/10 1:38p gautamk
* SW7420-850: [7420] Adding PI support for 24bit SPDIF PCM sample
* support.
* 
* Hydra_Software_Devel/459   9/16/10 12:18a srajapur
* SW7468-298 :[7468]Merging Real Video Code to main latest
* 
* Hydra_Software_Devel/458   9/14/10 7:53p sgadara
* SW7405-3993: [3548] Adding Audyssey Volume status info --> Porting to
* main line
* 
* Hydra_Software_Devel/457   9/7/10 6:28p gautamk
* SW7405-4713:[7405] passing physical address
* 
* Hydra_Software_Devel/456   9/6/10 5:14p gautamk
* SW7550-459: [7550] Merging GFX support in mainline
* 
* Hydra_Software_Devel/455   9/6/10 3:43p gautamk
* SW7550-547: [7550] Adding support for System task downloadable code.
* Merging to mainline
* 
* Hydra_Software_Devel/454   9/3/10 5:30p gautamk
* SW7405-4713: [7405] PPm should be disabled if Ms10 usage case
* 
* Hydra_Software_Devel/453   8/26/10 5:37p gautamk
* SW7405-4739: [7405] If Fw image is already downladed update the MIT
* table
* 
* Hydra_Software_Devel/452   8/24/10 11:02p gautamk
* SW7420-976: [7420] Fixing bug in STC VALID flag for passthru context.
* 
* Hydra_Software_Devel/451   8/20/10 2:44p sgadara
* SW3548-2992: [3548,3556] Fixing a bug to check the right status buffer
* size.
* 
* Hydra_Software_Devel/450   8/18/10 5:23p gautamk
* SW7550-196: [7550] Not sending user config change command for
* StcValid/InvalidateSTc when the state is already same
* 
* Hydra_Software_Devel/449   8/13/10 5:23p gautamk
* SW7405-4713:[7405] Merging changes for RDB indepedence in Main line
* 
* Hydra_Software_Devel/448   8/13/10 5:05p gautamk
* SW7420-767: [7420] Userconfig change for Fwmixer and SRC
* 
* Hydra_Software_Devel/447   8/10/10 1:53p gautamk
* SW7420-767: [7420] Merging MS11 feature support to mainline
* 
* Hydra_Software_Devel/446   8/5/10 3:19p gautamk
* SW7405-4602: [7405] Using Temporray StageStatus Buffer from pool of
* buffers.
* 
* Hydra_Software_Devel/445   7/26/10 4:18p gautamk
* SW7405-3063: [7405]Correcting name for enum
* 
* Hydra_Software_Devel/444   7/22/10 5:47p gautamk
* SW7405-3063: [7405] Config params change for SRSHD for certification
* app.
* 
* Hydra_Software_Devel/443   7/21/10 7:26p gautamk
* SW7405-3063:[7405] Updating userconfig of AACHE
* 
* Hydra_Software_Devel/442   7/21/10 7:19p gautamk
* SW7405-3063: [7405] For ringbuffer Capture, only 2 Rbuf should be
* allocated if its stero capture.
* 
* Hydra_Software_Devel/441   7/16/10 5:41p sgadara
* SW3548-2923 : [3548] Modifying the SRS StudioSound user cfg. -->
* Updating(user config) SRS Truvolume version --> Adding StudioSound Top
* Level structure to all 4 nodes
* 
* Hydra_Software_Devel/440   7/6/10 3:27p sgadara
* SW3548-2899 : [3548] Changing the default enable flag value of ADV and
* ABX
* 
* Hydra_Software_Devel/439   6/29/10 5:04p gautamk
* SW7405-4553: [7405] returning the error code correctly.
* 
* Hydra_Software_Devel/438   6/28/10 2:02p sgadara
* SW3548-2982: [3548,3556] Removing assert(0) in GetStreamInfo and
* GetCurrentPTS_isr calls as it is exiting gracefully with the relevant
* error return.
* 
* Hydra_Software_Devel/437   6/25/10 2:56p srajapur
* SW7550-461: [7550] Unaligned access in BRAP_GetDecoderVersionInfo -----
* ---> Fixed the unaligned access in BRAP_GetDecoderVersionInfo function
* 
* Hydra_Software_Devel/436   6/22/10 6:11p gautamk
* SW7335-774: [7335] Moving the frequent dynamic Memory allocations of
* structures to BRAP_Open and store it in hRap. Using it from hRap
* whenever required.
* 
* Hydra_Software_Devel/435   6/21/10 4:19p gautamk
* SW7405-3063: [7405] Fixing bug in compressed Rbuf capture.
* 
* Hydra_Software_Devel/434   6/18/10 1:42p gautamk
* SW7405-3063: [7405] Passing eTaskType correctly.
* 
* Hydra_Software_Devel/433   6/10/10 6:51p sgadara
* SW3548-2923: [3548,3556] Make changes to new TSHD config params. -->
* Certification params will need to exposed on a branch during chip
* level certification
* 
* Hydra_Software_Devel/432   6/4/10 5:26p gautamk
* SW7405-3063: [7405] updating default user config value
* 
* Hydra_Software_Devel/431   5/28/10 4:32p sgadara
* SW3548-2923: [3548,3556] Adding Support for SRS StudioSound. Correcting
* the parameter conversion
* 
* Hydra_Software_Devel/430   5/27/10 4:53p gautamk
* SW7405-4170: [7405] Modifying user config programming for Dolby pulse +
* No SRC required for Dolby pulse
* 
* Hydra_Software_Devel/429   5/27/10 2:24p sgadara
* SW3548-2923: [3548,3556] Add Support for SRS StudioSound. Modifying
* User configuration structure of SRS TSHD to be compatible with TSHD4
* configuration.
* 
* Hydra_Software_Devel/428   5/24/10 8:26p sgadara
* SW3548-2923: [3548,3556] Adding Support for SRS StudioSound.
* StudioSound  --> CsTd+TSHD+TVOL+EqHl
* 
* Hydra_Software_Devel/427   5/19/10 2:36p gautamk
* SW7405-3063: [7405] updating user config programming for STS TVO
* 
* Hydra_Software_Devel/426   5/18/10 4:16p gautamk
* SW7405-4170: [7405] Changing BDBG_ERR to BDBG_MSG
* 
* Hydra_Software_Devel/425   5/17/10 6:03p gautamk
* SW7405-4170: [7405] Adding User config change for framesync
* 
* Hydra_Software_Devel/424   5/17/10 10:54a gautamk
* SW7405-4170: [7405] Merging MS10 changes to mainline
* 
* Hydra_Software_Devel/423   5/13/10 4:08p gautamk
* SW7125-401: [7125] Allocating large arrays/structures on Heap to
* prevent stack overflow.
* 
* Hydra_Software_Devel/422   5/13/10 12:04p gautamk
* SW7405-4170: [7405] Default user config for Dolby Pulse is LtRt
* 
* Hydra_Software_Devel/421   5/6/10 5:53p gautamk
* SW7405-4058: [7405] Adding BRAP_Interrupt_eUnlicensedAlgo interrupt
* support
* 
* Hydra_Software_Devel/420   5/5/10 6:53p sgadara
* SW7405-3993 : [3548] Modifying user config of Audyssey Volume post
* processing algorithm
* 
* Hydra_Software_Devel/419   4/26/10 5:29p srajapur
* SW7405-3993 : [3548] Changed the default user config params
* 
* Hydra_Software_Devel/418   4/26/10 1:59p srajapur
* SW3548-2899 : [3548] Added PI support for ABX post processing
* 
* Hydra_Software_Devel/417   4/22/10 8:58p srajapur
* SW7405-3993 : [3548] Adding Audyssey Volume post processing algorithm
* 
* Hydra_Software_Devel/416   4/22/10 7:56p sgadara
* SW7405-4170:[7405,3548,3556] Making chages to internally map AACHE and
* AC3 Encode to Dolby Pulse and DD Transcode in PI for MS10 license
* 
* Hydra_Software_Devel/415   4/22/10 3:13p gautamk
* SW7405-4170:[7405] updating user config for MS10 ddp
* 
* Hydra_Software_Devel/414   4/19/10 7:10p gautamk
* SW7405-4170:[7405] Adding Support for MS10 DDP/AC3
* 
* Hydra_Software_Devel/413   4/9/10 9:58p gautamk
* SW7405-4168: [7405] using MACRO instead of 100.
* 
* Hydra_Software_Devel/412   4/9/10 4:01p gautamk
* SW7405-4168: [7405] Adding support for 0.5x to 2x playback rate.
* 
* Hydra_Software_Devel/411   4/8/10 7:31p gautamk
* SW7405-4146: [7405] indepndent delay value passed in Start tak
* parameter should be of Buffer sent for deadline computation
* 
* Hydra_Software_Devel/410   4/7/10 6:43p gautamk
* SW7405-4170: [7405] Adding Support for Multi Stream Decoder. Master and
* Secodary channel will be mixed by FwMixer Processing stage.
* 
* Hydra_Software_Devel/409   4/6/10 2:37p gautamk
* SW7405-4170: [7405] Adding FWMixer processing stage.
* 
* Hydra_Software_Devel/408   4/5/10 4:27p gautamk
* SW7405-4146: [7405] For PCM5.1 on HDMI, Delay value should also get
* applied to its Muxselector.
* 
* Hydra_Software_Devel/407   3/29/10 4:31p gautamk
* SW7335-697: [7335] Adding API to invalidate STC Flag
* 
* Hydra_Software_Devel/406   3/15/10 3:06p gautamk
* SW7405-3972: [7405] Profile info is only valid for DTS
* 
* Hydra_Software_Devel/405   3/12/10 2:45p speter
* SW3548-2827:[3548] Adding Mono Downmix support as post processing
* algorithm
* 
* Hydra_Software_Devel/404   3/11/10 9:03p sgadara
* SW3548-2819:[3548] Replacing back the code that got deleted in the
* previous version(400,401) checkins
* --> clearcase bug??
* 
* Hydra_Software_Devel/403   3/11/10 5:01p speter
* SW3548-2822:[3548] Add Dolby Transcode and Broadcom 3D surround post
* processing algos
* 
* Hydra_Software_Devel/402   3/11/10 9:26a speter
* SW3548-2819:[3548] Adding Dolby Pulse Algorithm
* 
* Hydra_Software_Devel/401   3/10/10 5:05p speter
* SW3548-2819:[3548] Adding Dolby Pulse Algorithm
* 
* Hydra_Software_Devel/400   3/9/10 9:32p sgadara
* SW7405-3972: [3548,3556,7405] Adding the support for providing
* decoder/encoder related info.
* 
* Hydra_Software_Devel/399   2/23/10 2:35p gautamk
* SW7342-155: [7340] Returning Audio decoder delay in Milli sec.
* 
* Hydra_Software_Devel/398   2/19/10 5:19p gautamk
* SW7468-119: [7468] Adding RAP PI support for Real audio LBR
* 
* Hydra_Software_Devel/397   2/18/10 6:40p sgadara
* SW3548-2785: [3548,3556] Add Metadata processing User Configuration
* parameter to DTS HD and DTS Broadcast
* --> Merge to main line
* 
* Hydra_Software_Devel/396   2/12/10 1:07p sgadara
* SW3548-2747: [3548,3556] GetTsmNodeInfo can be called in an ISR context
* and it is possible that a channel stop is issued by a task in
* parallel. So need to return with a warning if DSP is in stopped state.
* -->Merge to main line
* 
* Hydra_Software_Devel/RAP_3556_LG_GP2/4   2/5/10 5:25p sgadara
* SW3548-2747: [3548,3556] GetTsmNodeInfo can be called in an ISR context
* and it is possible that a channel stop is issued by a task in
* parallel. So need to return with a warning if DSP is in stopped state.
* 
* Hydra_Software_Devel/RAP_3556_LG_GP2/3   2/3/10 10:55a sgadara
* SW3548-2728: [3548,3556] Updating the sample rate info correctly for
* DTS Broadcast and DTS HD stream info structures
* 
* Hydra_Software_Devel/395   2/3/10 11:01a sgadara
* SW3548-2728: [7405,3548,3556] Updating the sample rate info correctly
* for DTS Broadcast and DTS HD stream info structures
* 
* Hydra_Software_Devel/394   1/29/10 5:01p sgadara
* SW3556-1017: [3548,3556] Updating userconfig of Audio Descriptor Pan
* Params
* --> Merge to main line
* 
* Hydra_Software_Devel/393   1/25/10 4:20p sgadara
* SW3556-983:[3548,3556] Adding parameters to Ac3 and DDP userconfig to
* program user provided Dialog Normalization values
* 
* Hydra_Software_Devel/392   1/13/10 2:46p gautamk
* SW7405-3202: [7405] implementing code review comments
* 
* Hydra_Software_Devel/391   1/12/10 2:41p gautamk
* SW7405-2922: [7405] Updating status buffer parameter for DTS
* Broadcast/HD
* 
* Hydra_Software_Devel/390   1/6/10 4:53p gautamk
* SW7405-3063:[7405] Using Channel OutMode instead of Stream Inmode for
* determining Num Buffers
* 
* Hydra_Software_Devel/389   12/18/09 5:08p gautamk
* SW7405-3591 : [7405] Fixing coverity issue
* 
* Hydra_Software_Devel/388   12/17/09 2:16p gautamk
* SW7405-3063: [7405] adding new paramter in user config of AACHE
* 
* Hydra_Software_Devel/387   12/8/09 4:09p gautamk
* SW7405-3063:[7405] CORRECTING INDEX.
* 
* Hydra_Software_Devel/386   12/5/09 4:03p gautamk
* SW7405-3063:[7405] validating pointer before accessing it.
* 
* Hydra_Software_Devel/385   11/30/09 3:08p gautamk
* SW7405-3063:[7405] Adding Dolby volume config paramter.
* 
* Hydra_Software_Devel/384   11/19/09 7:15p sgadara
* SW3556-832 : [3548,3556] Adding the "dynamic port switching" feature
* support.
* --> Passing register handle to CIT to program the external clock. This
* clock is a source to external DAC connected to one of our chip I2S
* output.
* 
* Hydra_Software_Devel/383   11/19/09 5:25p sgadara
* SW3556-832 : [3548,3556] Adding the "dynamic port switching" feature
* support.
* --> Passing register handle to CIT to program the external clock. This
* clock is a source to external DAC connected to one of our chip I2S
* output.
* 
* Hydra_Software_Devel/382   11/17/09 6:21p sgadara
* SW3556-832 : [3548,3556] Adding the "dynamic port switching" feature
* support.
* --> Adding PI-CIT interface for FMM Port(Destination) reconfiguration
* 
* Hydra_Software_Devel/381   11/17/09 4:30p speter
* SW3548-2566:[3548] Modifying the status buffer  for Clear Voice
* 
* Hydra_Software_Devel/380   11/10/09 2:30p speter
* SW7405-2959: [3548] Adding DTS-HD support
* 
* Hydra_Software_Devel/379   11/10/09 2:14p sgadara
* SW3556-859: [3548,3556] Access the destination details only after the
* destination type matches across.
* 
* Hydra_Software_Devel/378   11/6/09 7:33p sgadara
* SW3548-2566: [3548,3556] Updating Custom Voice user config params
* 
* Hydra_Software_Devel/377   11/2/09 3:30p sgadara
* SW3548-2566: [3548,3556] Updating Custom Voice user config params
* 
* Hydra_Software_Devel/376   10/29/09 5:43p gautamk
* SW7405-3312: [7405] Fixing crash.
* 
* Hydra_Software_Devel/375   10/28/09 2:23p sgadara
* CR3556-49 : [3548,3556] Modifying the user cfg for SRS TVOL to include
* the DC Notch Filter
* 
* Hydra_Software_Devel/374   10/26/09 2:29p gautamk
* SW7405-3283: [7405] Fixing warning.
* 
* Hydra_Software_Devel/373   10/22/09 7:02p sgadara
* SW3548-2566: [3548,3556] Updating Custom Voice user config params
* 
* Hydra_Software_Devel/372   10/21/09 5:28p gautamk
* SW7405-3079: [7405] Correcting streaminformation for PCM wav.
* 
* Hydra_Software_Devel/371   10/21/09 5:01p gautamk
* SW7335-609:[7405] Setting hTask-bStopped appropriately in Isr Ctxt when
* Event is getting set. This is done to fix race condition. Porting to
* mainline
* 
* Hydra_Software_Devel/370   10/14/09 4:15p gautamk
* SW7405-3063: [7405]changing the debug MSG
* 
* Hydra_Software_Devel/369   10/14/09 4:04p gautamk
* sw7405-2740:[7405] adding PI support for WMA DRC, Merging to mainline
* 
* Hydra_Software_Devel/368   10/8/09 1:23p gautamk
* SW7405-3063: [7405] doing misc changes related to post processing
* stages and AD.
* 
* Hydra_Software_Devel/367   10/7/09 4:33p gautamk
* SW7405-3063: [7405] Adding Delay parameter for Rbuf.
* 
* Hydra_Software_Devel/366   9/29/09 2:03p gautamk
* SW7405-3063: [7405] fixing coverity issue
* 
* Hydra_Software_Devel/366   9/29/09 1:22p gautamk
* SW7405-3063: [7405] fixing coverity issue
* 
* Hydra_Software_Devel/366   9/29/09 1:21p gautamk
* SW7405-3063: [7405] fixing coverity issue
* 
* Hydra_Software_Devel/365   9/23/09 6:11p gautamk
* SW7420-357: [7420] Adding support for SBC Encode
* 
* Hydra_Software_Devel/364   9/22/09 5:51p gautamk
* SW7405-3058: [7405] Adding support for DRA decoder.
* 
* Hydra_Software_Devel/363   9/18/09 7:05p sgadara
* SW3548-2188: [3548,3556] Correcting the default user config settings
* for AMR algo.
* 
* Hydra_Software_Devel/362   9/18/09 3:04p gautamk
* SW7405-2606: [7405] Updating user config of AACHE
* 
* Hydra_Software_Devel/361   9/11/09 6:45p sgadara
* SW3548-2188: [3548,3556] Adding support for AMR decode algorithm.
* 
* Hydra_Software_Devel/360   9/8/09 2:31p gautamk
* SW7405-2986: [7405]Enabinge DTS Broadcast Pasthru without DTS
* Licensing. Also Enable Other Passthru which can be allowed without
* license
* 
* Hydra_Software_Devel/359   9/4/09 9:07p sgadara
* SW3548-2454: [3548,3556] Inserting "_P_" to the names of private
* functions.
* 
* Hydra_Software_Devel/358   9/4/09 5:34p gautamk
* SW7405-2972: [7405] Adding API BRAP_GetDecoderVersionNo.
* 
* Hydra_Software_Devel/357   9/4/09 5:32p gautamk
* SW7405-2972: [7405] Adding API BRAP_GetDecoderVersionNo.
* 
* Hydra_Software_Devel/356   9/3/09 6:25p gautamk
* SW7405-2606: [7405] Exosinguserconfig  NumChannels for Dolby VOl to APP
* 
* Hydra_Software_Devel/355   9/3/09 6:17p gautamk
* SW7405-2606: [7405] Adding user config for DTS enc certification
* 
* Hydra_Software_Devel/354   8/28/09 12:38p gautamk
* SW7405-2947: [7405] updating the default value.
* 
* Hydra_Software_Devel/353   8/11/09 3:17p gautamk
* PR55224: [7405] Fixing compilatio for 7340,7342
* 
* Hydra_Software_Devel/352   7/23/09 4:30p gautamk
* PR56006: [7405] correcting BERR_Code
* 
* Hydra_Software_Devel/351   7/15/09 11:04a gautamk
* PR56006: [7405] Adding support for mp3 encode in rap pi
* 
* Hydra_Software_Devel/350   7/7/09 12:00p speter
* PR 56011:[3548,3556] Implementing review comments (Init pointers to
* null. Allocating big structure in heap)
* 
* Hydra_Software_Devel/349   6/23/09 2:33p gautamk
* PR56006: [7405] Fixing coverity bug
* 
* Hydra_Software_Devel/348   6/17/09 5:17p gautamk
* PR56011: [7405] Implementing code review comments
* 
* Hydra_Software_Devel/347   6/17/09 4:49p gautamk
* PR56010: [7405] Implmenting Code review comment
* 
* Hydra_Software_Devel/346   6/12/09 3:43p gautamk
* PR53302: [7405] Merging cleanup code to mainline
* 
* Hydra_Software_Devel/345   6/9/09 12:34p gautamk
* PR55572:[7405] Before accessing any pointer location ccheck if it is
* 0XDA7ADEAD or 0xDEADDA7A in addition to NULL.
* 
* Hydra_Software_Devel/344   6/9/09 11:37a gautamk
* PR47949: [7405]Changing name from PCMOnAVI to PCMWAV
* 
* Hydra_Software_Devel/343   6/5/09 5:19p gautamk
* PR55572: [7405] Adding State BRAP_P_State_eStopInvoked to handle race
* condition between Stopchannel and  Isr functions like SetStcValid_isr
* etc.
* 
* Hydra_Software_Devel/342   6/5/09 4:36p gautamk
* PR55572: [7405] Existing gracefullt if Task/Channel already stopped at
* the Time of calling Isr functions like SetStcValid_isr etc.
* 
* Hydra_Software_Devel/341   6/4/09 3:04p gautamk
* PR47949: [7405] Adding PI support for PCM On Avi
* 
* Hydra_Software_Devel/340   5/28/09 7:08p sgadara
* PR 55437: [3548,3556] Adding support for SRS TSHD certification.
* --> Modifying the user config params and default values for SRSXT.
* 
* Hydra_Software_Devel/339   5/28/09 4:03p sgadara
* PR 55437: [3548,3556] Adding support for SRS TSHD certification.
* --> Modifying the user config params and default values.
* 
* Hydra_Software_Devel/338   5/27/09 4:48p sgadara
* PR 54330: [3548,3556] Framesync user confg structure has been modified
* replacing capture port type with audio source type.
* --> Changing the enum name in the defualt settings accordingly.
* Merging the changes to main line.
* 
* Hydra_Software_Devel/337   5/22/09 4:42p gautamk
* PR55135: [7405] Moving SRC programming of PB/CAP from Samplerate_isr to
* RampEnable_isr
* 
* Hydra_Software_Devel/336   5/19/09 1:33p gautamk
* PR52383: [7405] adding various counts  of frame error/info.
* 
* Hydra_Software_Devel/335   5/14/09 2:18p gautamk
* PR53560: [7405] Removing unwanted error returns.
* 
* Hydra_Software_Devel/334   5/12/09 3:54p gautamk
* PR53560: [7405] Adding Stream info avaialable interrupt support.
* 
* Hydra_Software_Devel/333   5/11/09 3:52p gautamk
* PR52383: [7405] Adding algorithm support for DTS decoder.
* 
* Hydra_Software_Devel/332   5/6/09 12:15p sgadara
* PR 54330: [3548,3556] Modifying  the default and range values of few
* user config parameters of SRS TruVolume since SRS labs has changed
* these, as informed  by Firmware. Also changing the format in which PI
* used to receive reference level param value.
* 
* Hydra_Software_Devel/331   4/28/09 1:30p gautamk
* PR52383:[7405] GetStreamInformation() should not be called for passthru
* contetxt.
* 
* Hydra_Software_Devel/330   4/24/09 3:55p sgadara
* PR 54330: [3548,3556] Add support for SRS TruVolume certification.
* --> Changing the default value of Max Gain param.
* 
* Hydra_Software_Devel/329   4/20/09 3:56p gautamk
* PR52383: [7405] Creating seperate event handles for each task.
* 
* Hydra_Software_Devel/328   4/17/09 7:48p sgadara
* PR 54330: [3548,3556] Add support for SRS TruVolume certification.
* --> Adding the interface, Ringbuffer as the audio source to the
* channel.
* 
* Hydra_Software_Devel/327   4/16/09 4:14p gautamk
* PR52383: [7405] Not waiting for response when sending SetPTsoffset
* command. When waiting COnfig change command timeout.
* 
* Hydra_Software_Devel/326   4/8/09 4:45p gautamk
* PR52383:[7405] Changing debug level of error
* 
* Hydra_Software_Devel/325   4/8/09 2:49p gautamk
* PR52383: [7405] Fixing coverity issue.
* 
* Hydra_Software_Devel/324   4/7/09 2:22p gautamk
* PR52383: [7405] Updating AAC default Config param.
* And correcting macro in brap_fwif_priv.c
* 
* Hydra_Software_Devel/323   4/3/09 4:23p gautamk
* PR52383: [7405] Making Set_PtsOffset() function non-isr  to wait on
* response from FW.
* 
* Hydra_Software_Devel/322   4/2/09 4:33p speter
* PR 52015: [3548,3556] Adding code to take CIT branch and CIT stage IDs
* for getting the processing information.
* 
* Hydra_Software_Devel/321   4/2/09 1:37p gautamk
* PR53764: [7405]
* PR53393: [7405] Memory optimization in RAP PI
* 
* Hydra_Software_Devel/320   4/1/09 4:38p gautamk
* PR53764: [7405]
* PR53393: [7405] Memory optimization in RAP PI
* 
* Hydra_Software_Devel/319   3/26/09 11:35a gautamk
* PR53283: [7405] Correcting debug messages.
* 
* Hydra_Software_Devel/318   3/24/09 5:14p gautamk
* PR53394:[7405] passing outputmode to Mpeg decoder.
* 
* Hydra_Software_Devel/317   3/23/09 2:56p gautamk
* PR53419: [7405] non-isr function hsould not be called from isr
* functions.
* 
* Hydra_Software_Devel/316   3/19/09 12:03p gautamk
* PR53365: [7405] Adding PI support for dolby volume
* 
* Hydra_Software_Devel/315   3/18/09 9:52p gautamk
* PR53365: [7405] Adding PI support for dolby volume
* 
* Hydra_Software_Devel/314   3/17/09 11:59a gautamk
* PR52383: [7405] Enabling Es type for 7405
* 
* Hydra_Software_Devel/313   3/16/09 3:51p speter
* PR 52015: [3548,3556] Adding support for status buffer in Post
* Processing algorithms. Adding to main line.
* 
* Hydra_Software_Devel/312   3/5/09 12:03p speter
* PR 52090: [3548,3556] Merging the Phase 4.2 Take 2 release to main line
* 
* Hydra_Software_Devel/311   2/27/09 5:19p gautamk
* PR52187: [7405] Updating the enum name.
* 
* Hydra_Software_Devel/310   2/13/09 1:39p speter
* PR 52090: [3548,3556] Merging the Phase 4.2 Take 2 release to main line
* 
* Hydra_Software_Devel/309   2/4/09 6:34p gautamk
* PR48850: [7405]
* 
* Hydra_Software_Devel/RAP_354856_PH_4_3/4   3/3/09 3:58p speter
* PR 50740: [3548,3556] Adding check for Rap Channel before accessing it
* 
* Hydra_Software_Devel/RAP_354856_PH_4_3/3   2/11/09 2:00p speter
* PR 52014: [3548,3556] LFE information of stream was conveyed
* incorrectly for AAC/AAC-HE streams. Fixing in main line.
* 
* Hydra_Software_Devel/RAP_354856_PH_4_3/2   2/10/09 1:48p speter
* PR 51145: [3548,3556] Adding support for AC3 Encoder Certification.
* 
* Hydra_Software_Devel/RAP_354856_PH_4_3/1   2/10/09 12:06p speter
* PR 46135: [3548,3556] Adding SRS Tru Volume Support - Chaning the name
* to TruVolume
* 
* Hydra_Software_Devel/308   2/4/09 11:49a speter
* PR 46135: [3548,3556] Adding SRS VIQ Support
* 
* Hydra_Software_Devel/307   2/3/09 5:19p gautamk
* PR 49464 : [7405]Exposing ASFPTStype to app.
* 
* Hydra_Software_Devel/306   2/2/09 4:38p gautamk
* PR50291: [7405] Adding Upperthreshold to structure BRAP_DSPCHN_PtsInfo.
* 
* Hydra_Software_Devel/305   2/2/09 4:28p gautamk
* PR51652: [7405]
* PR51618: [7405] Changing the global variables as per following rule.
* 1) make them static
* 2) if you cannot make them static because they must be shared between
* multiple .c files in your module, you must rename them to include the
* BXXX_ magnum prefix for your module.
* 
* Hydra_Software_Devel/304   2/2/09 3:42p gautamk
* PR51618: [7405] Changing the global variables as per following rule.
* 1) make them static
* 2) if you cannot make them static because they must be shared between
* multiple .c files in your module, you must rename them to include the
* BXXX_ magnum prefix for your module.
* 
* Hydra_Software_Devel/303   1/29/09 4:14p gautamk
* PR43157: [7405] fixing resource leak
* 
* Hydra_Software_Devel/302   1/29/09 12:27p speter
* PR 46135: [3548,3556] Adding SRS VIQ Support - Making the config
* parameters user friendly
* 
* Hydra_Software_Devel/301   1/28/09 1:39p speter
* PR 46135: [3548,3556] Adding SRS VIQ Support
* 
* Hydra_Software_Devel/300   1/27/09 3:59p speter
* PR 50495: [3548,3556] Merging the customer branch to main line
* 
* Hydra_Software_Devel/299   1/22/09 5:26p gautamk
* PR50455: [7405] Setting VOLUME STEP RAMP to 0x20 when DSP FW triggers
* EnableRamp interrupt.
* 
* Hydra_Software_Devel/298   1/15/09 12:27p sgadara
* PR 43157: [3548,3556] Storing the threshold and block time  values in a
* local variable when the first call to
* BRAP_CITGEN_P_CalcThresholdZeroFillTimeAudOffset() is made and use the
* these values to populate the task params later.
* 
* Hydra_Software_Devel/297   1/13/09 2:56p sgadara
* PR 43157: [3548,3556] Taking the second call of
* BRAP_CITGEN_P_CalcThresholdZeroFillTimeAudOffset() out and populating
* the threshold and block time after the first call.
* 
* Hydra_Software_Devel/296   1/13/09 9:49a gautamk
* PR48850:[7405] triggerring watchdog when stop/start task timeouts. This
* watchdog should be debugged in  FW
* 
* Hydra_Software_Devel/295   1/9/09 7:38p gautamk
* PR48850: [7405] Allocating big structure on heap
* 
* Hydra_Software_Devel/294   1/9/09 6:21p gautamk
* PR48850: [7405] Allocating big structure on HEAP
* 
* Hydra_Software_Devel/293   1/7/09 4:37p gautamk
* PR50291:[7405] Adding API GetTsmDebugInfo.
* 
* Hydra_Software_Devel/292   12/30/08 1:31p gautamk
* PR48850: [7405] Fixing bug in Memory requirement function.
* 
* Hydra_Software_Devel/291   12/30/08 12:09p gautamk
* PR48850: [7405] Merging to mainline.
* 
* Hydra_Software_Devel/290   12/26/08 3:49p gautamk
* PR48850:[7405]Merging WMATS changes to Mainline
* 
* Hydra_Software_Devel/289   12/24/08 3:47p gautamk
* PR48850: [7405] Correcting debug mesg
* 
* Hydra_Software_Devel/288   12/24/08 3:34p gautamk
* PR48850: [7405] Changes for DVD  LPCM
* 
* Hydra_Software_Devel/287   12/24/08 1:05p gautamk
* PR48850: [7405] Fixing compilation.
* 
* Hydra_Software_Devel/286   12/24/08 11:40a gautamk
* PR48850: [7405] INtializing the elements in handle
* 
* Hydra_Software_Devel/285   12/23/08 1:32p gautamk
* PR48850: [7405]  adding support for DVD LPCM. Merging to mainline
* 
* Hydra_Software_Devel/284   12/22/08 11:34p gautamk
* PR48850:[7405] Minor fix.
* 
* Hydra_Software_Devel/283   12/22/08 4:11p gautamk
* PR48850: [7405] Accessing invalid index of the array was causing
* Segmentaion fault . Fixing it.
* 
* Hydra_Software_Devel/282   12/22/08 11:30a speter
* PR 46135: [3548,3556] Adding SRS VIQ Support
* 
* Hydra_Software_Devel/281   12/16/08 2:36p gautamk
* PR48850: [7405] Moving Timebase parameter from TSM Config  to Global
* Task Config
* 
* Hydra_Software_Devel/NEXTGEN_MAGNUM_TIMEBASE/1   12/15/08 4:55p gautamk
* PR48850: [7405] Moving Timebase parameter from TSM Config  to Global
* Task Config
* 
* Hydra_Software_Devel/280   12/15/08 4:35p gautamk
* PR44109: [7405] Adding Debug logs for BRAP_GetAudioDecodeDelay()
* 
* Hydra_Software_Devel/279   12/10/08 10:58p gautamk
* PR49232:[7405] Adding support for Allocating Rbuf/SrcCh and Paths at
* OpenChannel time.
* 
* Hydra_Software_Devel/278   12/5/08 6:05p gautamk
* PR48850: [7405] Fixing Compile error.
* 
* Hydra_Software_Devel/RAP_354856_PH_4_1/5   1/19/09 3:21p sgadara
* PR 50859: [3548,3556] Adding the code to trigger watchdog whenever
* there is a timeout error.
* 
* Hydra_Software_Devel/RAP_354856_PH_4_1/4   1/14/09 12:27a sgadara
* PR 43157: [3548,3556] Storing the threshold and block time  values in a
* local variable when the first call to
* BRAP_CITGEN_P_CalcThresholdZeroFillTimeAudOffset() is made and use the
* these values to populate the task params later.
* 
* Hydra_Software_Devel/RAP_354856_PH_4_1/3   1/13/09 2:43p sgadara
* PR 43157: [3548,3556] Taking the second call of
* BRAP_CITGEN_P_CalcThresholdZeroFillTimeAudOffset() out and populating
* the threshold and block time after the first call.
* 
* Hydra_Software_Devel/RAP_354856_PH_4_1/2   1/7/09 4:12p sgadara
* PR 50878: [3548,3556] Adding the code to trigger Raptor Watchdog
* whenever there is a start/stop task ack timeout.
* 
* Hydra_Software_Devel/RAP_354856_PH_4_1/1   12/5/08 10:53a sgadara
* PR49845:[3548,3556] changing ui32PtsToStcPhase param to
* i32Pts2StcPhase since it carries signed quantity now.
* 
* Hydra_Software_Devel/RAP_DVD_LPCM_SUPPORT/2   12/5/08 11:30a gautamk
* PR48850: [7405]  adding support for DVD LPCM
* 
* Hydra_Software_Devel/RAP_DVD_LPCM_SUPPORT/1   11/25/08 6:25p gautamk
* PR48850: [7405]  adding support for DVD LPCM
* 
* Hydra_Software_Devel/275   11/25/08 11:16a speter
* PR 49564: [3548,3556] Merging the Phase 4.0 release to main line
* 
* Hydra_Software_Devel/274   11/21/08 3:35p gautamk
* PR49281:[7405] Not Programming O/P PLL0 for Playback when Decode is
* already present.
* 
* Fixing Bug in generate cit input.
* 
* Fixing bug in Download Exec function
* 
* Hydra_Software_Devel/RAP_354856_PH_4_0/4   11/7/08 8:38p sgadara
* PR 43158: [3548,3556] Adding support for minimal delay through decoder.
* 
* Hydra_Software_Devel/RAP_354856_PH_4_0/3   11/7/08 2:48p sgadara
* PR 43158: [3548,3556] Adding support for minimal delay through decoder.
* 
* Hydra_Software_Devel/RAP_354856_PH_4_0/2   11/6/08 8:14p sgadara
* PR 38950: [3548,3556] Changing BDBG_ERR messages to BDBG_MSG.
* 
* Hydra_Software_Devel/RAP_354856_PH_4_0/1   11/5/08 7:04p sgadara
* PR 40715: [3548,3556] Adding support for new AAC decoder.
* 
* Hydra_Software_Devel/271   10/21/08 2:54a speter
* PR 45260: [3548,3556,7405] Correcting the debug message
* 
* Hydra_Software_Devel/270   10/20/08 2:42p gautamk
* PR43454: [7405] If the task in the stop state OR task id is invalid,
* Handle this case appropriately.
* 
* Hydra_Software_Devel/269   10/10/08 10:19a speter
* PR 47764: [3548,3556] Merging the release branch 3.0 to main line of
* development
* 
* Hydra_Software_Devel/268   10/7/08 5:59p gautamk
* PR43454: [7405] Correcting comments.
* 
* Hydra_Software_Devel/267   9/26/08 8:35p gautamk
* PR47350: [7405] Do not set TSM config params, when Task is in stopped
* state.
* 
* Hydra_Software_Devel/266   9/23/08 7:37p sushmit
* PR 43454:[7405] Fixing watchdog recovery
* 
* Hydra_Software_Devel/RAP_354856_PH_3_0/3   10/9/08 4:31p speter
* PR 38950: [3548,3556] Adding support for SPDIF Rx - Adding other
* framesync algos user config
* 
* Hydra_Software_Devel/RAP_354856_PH_3_0/2   10/8/08 6:50p speter
* PR 47226: [3548,3556] Adding code in RAP PI to provide the audio mode
* to all post processing algorithm stages.
* 
* Hydra_Software_Devel/RAP_354856_PH_3_0/1   10/1/08 3:18p speter
* PR 46051: [3548,3556] Changing DDEnc to AC3Enc
* 
* Hydra_Software_Devel/265   9/23/08 9:31a speter
* PR 46054: [3548,3556] Fixing the common code to use Decode mode to get
* the firmware IDs.
* 
* Hydra_Software_Devel/264   9/22/08 9:10a speter
* PR 45032: [3548,3556] Adding Framesync user config params related
* changes
* 
* Hydra_Software_Devel/263   9/19/08 6:22p gautamk
* PR45739: [7405] Changing bLargeRbuf to bIndepDelayEnabled
* 
* Hydra_Software_Devel/262   9/18/08 9:03a speter
* PR 46047: [3548,3556] Adding support for Custom Bass and Custom
* Surround. Fixing default values of surround
* 
* Hydra_Software_Devel/261   9/18/08 8:58a speter
* PR 38950: [3548,3556] Adding support for SPDIF In
* 
* Hydra_Software_Devel/260   9/17/08 8:13p sushmit
* PR 43454:[7405] Reducing Cit Input Structure Size. Need to move
* allocation runtime later based on no. of actual branches.
* 
* Hydra_Software_Devel/259   9/17/08 3:23p gautamk
* PR46634: [7405] when independent delay is on, Seperate Path is created
* for Internal capture
* 
* Hydra_Software_Devel/258   9/17/08 9:46a speter
* PR 46942: [3548,3556] Fixing the compile related problems after merging
* to main line
* 
* Hydra_Software_Devel/257   9/16/08 3:48p speter
* PR 46942: [3548,3556] Merging back to main line of development to get
* the distinct output feature
* 
* Hydra_Software_Devel/256   9/16/08 2:43p sushmit
* PR 43454:[7405]Adding AC3 Encode Support.
* 
* Hydra_Software_Devel/255   9/15/08 4:08p gautamk
* PR43454: [7405] Do not create seperate path/branch for
* bDownmixedOpPort= true O/P, when outputmode is 2_0
* 
* Hydra_Software_Devel/254   9/15/08 11:40a sushmit
* PR 43454:[7405] Converting BDBG_ERR to BDBG_MSG
* 
* Hydra_Software_Devel/253   9/12/08 4:17p gautamk
* PR43454: [7405] Moving code to GenCit()
* 
* Hydra_Software_Devel/252   9/9/08 7:37p sushmit
* PR 43454: [7405]Updating RBUF Sizes.
* 
* Hydra_Software_Devel/251   9/9/08 3:11p gautamk
* PR42605: [7405] Fixing coverity issue.
* 
* Hydra_Software_Devel/250   9/8/08 9:18p gautamk
* PR45739: [7405] Adding support for indep delay for o/p
* 
* Hydra_Software_Devel/249   9/8/08 7:18p sushmit
* PR45739: [7405] Updating BRAP_CITGEN_P_CalcThresholdAndBlockTime call
* 
* Hydra_Software_Devel/248   9/8/08 5:27p gautamk
* PR45739: [7405] Adding support for indep delay for o/p
* 
* Hydra_Software_Devel/RAP_354856_20080904_BRANCH/2   9/16/08 8:55a speter
* PR 46592: [3548,3556] System hang due to invalid task ID. Adding check
* for invalid task ID.
* 
* Hydra_Software_Devel/RAP_354856_20080904_BRANCH/1   9/5/08 3:19p speter
* PR 45032: [3548,3556] Adding PES Framesync user config params related
* changes
* 
* Hydra_Software_Devel/247   9/4/08 11:02p speter
* PR 46565: [3548,3556] Adding SRS-HD support
* 
* Hydra_Software_Devel/246   9/2/08 11:05a speter
* PR 45377: [3548,3556] Changin the custom voice config params as per the
* latest firmware
* 
* Hydra_Software_Devel/245   9/1/08 3:49p speter
* PR44701: PR 44701: [3548,3556] Fixing the user config for SRS-XT
* 
* Hydra_Software_Devel/244   8/28/08 12:21p speter
* PR 46047: [3548,3556] Adding support for Custom Bass and Custom
* Surround
* 
* Hydra_Software_Devel/243   8/27/08 11:19p sushmit
* PR 43454:[7405] Adding PPM Correction to Start Task Parameters
* 
* Hydra_Software_Devel/242   8/22/08 4:07p gautamk
* PR43454: [7405] Clone stage of downmix branch , can be a FW stage.
* 
* Hydra_Software_Devel/241   8/21/08 1:56p gautamk
* PR43454: [7405] Fixing warnings.
* 
* Hydra_Software_Devel/240   8/21/08 1:44p gautamk
* PR43454: [7405] Fixing compilation break.
* 
* Hydra_Software_Devel/239   8/20/08 6:10p gautamk
* PR43454: [7405] Changes for Concurrent downmix.
* 
* Hydra_Software_Devel/238   8/19/08 4:04p sushmit
* PR 43454:[7405]Making bChSpecificStage to false
* 
* Hydra_Software_Devel/237   8/14/08 12:07p sushmit
* PR 43454:[7405]Updating DSOLA Params.
* 
* Hydra_Software_Devel/236   8/12/08 11:00p sushmit
* PR 43454:[7405]Adding DSOLA.
* 
* Hydra_Software_Devel/235   8/12/08 3:12p sushmit
* PR 43454:[7405]Bug Fixes
* 
* Hydra_Software_Devel/234   8/12/08 10:48a speter
* PR 45032: [3548,3556] Adding PES Framesync user config params related
* changes
* 
* Hydra_Software_Devel/233   8/12/08 10:22a speter
* PR 45032: [3548,3556] Adding PES Framesync user config params related
* changes
* 
* Hydra_Software_Devel/232   8/11/08 7:06p gautamk
* PR43454: [7405] For multichannel Pass the no. of ringbuffer 6
* 
* Hydra_Software_Devel/231   8/10/08 6:55p sushmit
* PR 43454:[7405] DTSENC Updates.
* 
* Hydra_Software_Devel/PR44701/1   8/22/08 1:51p speter
* PR 44701: [3548,3556] Fixing the user config for SRS-XT
* 
* Hydra_Software_Devel/230   8/5/08 3:52p speter
* PR 45407: [3548,3556] Merging the Phase 2.1 release branch to main line
* of development (Hydra_Software_Devel)
* 
* Hydra_Software_Devel/RAP_354856_PH_2_1/1   8/4/08 4:37p speter
* PR 45377: [3548,3556] Changin the custom voice config params as per the
* latest firmware
* 
* Hydra_Software_Devel/229   8/1/08 9:06a speter
* PR 44895: [3548,3556] To bring up 'capture to ringbuffer'  feature on
* 3548
* 
* Hydra_Software_Devel/228   7/30/08 3:14p gautamk
* PR43454: [7405] Some minor fixes
* 
* Hydra_Software_Devel/227   7/28/08 9:33a speter
* PR 45171: [3548,3556] Merging from Phase 2.0 release to main line of
* development (Hydra_Software_Devel)
* 
* Hydra_Software_Devel/226   7/25/08 4:39p gautamk
* PR43493: [7405] TSM configuration can be  set asynchronously for
* different tasks, So using a single buffer for ChangeCfgCommand was
* causing overwrite of the memory. Using seperate memory buffer for each
* task for updating TSm config param.
* 
* Hydra_Software_Devel/225   7/23/08 9:31p gautamk
* PR43454: [7405] Adding PI support for WMA Passthru
* 
* Hydra_Software_Devel/224   7/23/08 4:25p gautamk
* PR44143: [7405] AACHE LOAS is the default audio type when its chosen
* AACHE
* 
* Hydra_Software_Devel/223   7/23/08 3:57p gautamk
* PR43454: [7405] inserting PCMrouter satge interanally for AD Fade
* 
* Hydra_Software_Devel/222   7/23/08 11:52a gautamk
* PR43454: [7405] updating comments. and adding warning.
* 
* Hydra_Software_Devel/RAP_354856_PH_2_0/1   7/24/08 4:56p speter
* PR 43454: [3548,3556] Adding PCM router post processing algorithm
* 
* Hydra_Software_Devel/221   7/22/08 3:21p speter
* PR 45032: [3548,3556] Adding DSP support to SPDIF/HDMI In
* 
* Hydra_Software_Devel/220   7/18/08 4:17p gautamk
* PR43454: [7405] Correcting comment
* 
* Hydra_Software_Devel/219   7/16/08 10:07p gautamk
* PR43454: [7405] Implementing delta code review.
* 
* Hydra_Software_Devel/218   7/11/08 4:34p gautamk
* PR43454: [7405] adding the Macro for checking validity of
* Branch/Stage/network
* 
* Hydra_Software_Devel/217   7/10/08 6:22p gautamk
* PR41825: [7405] merging the code for multiple associatio from Branch to
* mainline.
* 
* Hydra_Software_Devel/216   7/10/08 5:02p speter
* PR 44701: [3548,3556] Adding support for post processing algorithms
* PL2, SRSXT and AVL
* 
* Hydra_Software_Devel/215   7/4/08 5:04p gautamk
* PR43454: [7405] programming transition threshold in 45KHZ ticks.
* 
* Hydra_Software_Devel/214   7/4/08 4:29p gautamk
* PR43454: [7405] Implementation for Adaptive rate ctrl (2ms lipsync)
* 
* Hydra_Software_Devel/213   7/4/08 3:14p gautamk
* PR43454: [7405] programming gross threshold in terms of 45 KHZ tics.
* 
* Hydra_Software_Devel/212   7/3/08 11:29p gautamk
* PR42605: [7405] fixing coverity issues for 7405
* 
* Hydra_Software_Devel/211   6/30/08 4:04p gautamk
* PR43454: [7405] Merging the code for distinct output of a stage.
* 
* Hydra_Software_Devel/210   6/26/08 9:25p gautamk
* PR44216: [7405] using bLfeOn parameter insteaed of bOutputLfeMode in
* the streaminformation.
* 
* Hydra_Software_Devel/209   6/26/08 9:14p gautamk
* PR43454: [7405] Adding sanity check for the network for audio
* processing stages.
* 
* Hydra_Software_Devel/208   6/20/08 5:57p gautamk
* PR43454: [7405] Adding function determine if the the brancg is cloned
* or not.
* 
* Hydra_Software_Devel/207   6/20/08 5:41a speter
* PR 38950: [3548,3556] Adding custom voice user config support
* 
* Hydra_Software_Devel/206   6/17/08 6:21p gautamk
* PR43454: [7405] Fixing compilation warning for VXworks.
* 
* Hydra_Software_Devel/205   6/16/08 5:13p gautamk
* PR43454: [7405] In case of BOpentimeDownload , don't download duplicate
* execs.
* 
* Hydra_Software_Devel/204   6/16/08 10:52a gdata
* PR41726: [7405] Adding changes for audio descriptor
* 
* Hydra_Software_Devel/203   6/14/08 7:34a speter
* PR 42110: [3548,3556] Adding ADC support
* 
* Hydra_Software_Devel/202   6/12/08 1:21p gautamk
* PR43454: [7405] correcting default config param
* 
* Hydra_Software_Devel/201   6/11/08 7:31a speter
* PR38950: [3548,3556] Changing generic use of BTSC as RF Audio
* 
* Hydra_Software_Devel/200   6/10/08 8:55p gautamk
* PR43472: [7405] Adding interrupt support for TSM fail interrupt.
* 
* Hydra_Software_Devel/199   6/10/08 12:22a gautamk
* PR41726: [3548] changes for external capture on 3548.
* 
* Hydra_Software_Devel/198   6/6/08 4:53p gautamk
* PR41726: [7405] not making tsm status buffer invalid in dspchn_start(),
* if its getting called from internal function like FlushChannel()
* 
* Hydra_Software_Devel/197   6/4/08 11:08p gautamk
* PR43314:[7405] Segmentataion fault occurs on trigerring watchdog(via
* BBS) with playback in pause state
* PR43312:[7405]
* 
* Hydra_Software_Devel/196   6/2/08 1:59p gautamk
* PR41726: [7405] making the fields of Status buffer invalid.
* 
* Hydra_Software_Devel/195   6/2/08 1:30a gautamk
* PR41726: [7405] Changing blocking time and threshold values.
* 
* Hydra_Software_Devel/194   5/30/08 11:58p gautamk
* PR41726: [7405] temporary change in threshold values
* 
* Hydra_Software_Devel/193   5/30/08 2:28p gautamk
* PR41726:[7405] Changing Threshold value,blocking time for DDP Passthru
* 
* Hydra_Software_Devel/192   5/29/08 10:08p gautamk
* PR41726: [7405] fixing compilation warning
* 
* Hydra_Software_Devel/191   5/29/08 9:47p gautamk
* PR41726: [7405] doing Memset after Malloc
* 
* Hydra_Software_Devel/190   5/29/08 3:28p gautamk
* PR41726: [7405] Adding function to insert/destroy stages internally in
* Task network.
* 
* Hydra_Software_Devel/189   5/29/08 3:13p gautamk
* PR41726: [7405]Adding function to calculate Threshold and blocking
* time.
* 
* Hydra_Software_Devel/188   5/29/08 2:17a speter
* PR 38950: [3548,3556] Adding Audio Descriptor support
* 
* Hydra_Software_Devel/187   5/28/08 11:38p gautamk
* PR41726: [7405] Modifying files for AAC/AAC-HE LOAS/ADTS
* 
* Hydra_Software_Devel/186   5/28/08 8:11p gautamk
* PR41726: [7405] Adding support for SRC postprocessing
* 
* Hydra_Software_Devel/185   5/23/08 3:40p gautamk
* PR41726:[7405] Moving the STOP TASK Command at the end of DSPCHN_STOP
* after freeing up all the resources.
* 
* Hydra_Software_Devel/184   5/23/08 1:32p gautamk
* PR42302: [7325] Fixing the race condition between MIPS and DSP for
* updating ET bit of wachdog timer.
* PR42807: [7335]
* PR42837: [7325]
* 
* Hydra_Software_Devel/183   5/21/08 3:34p sushmit
* PR41726: [7325] Making default STC valid to false for passthru
* 
* Hydra_Software_Devel/182   5/21/08 7:19p gdata
* PR41726: [7405] Implementing review comments for Interrputs
* 
* Hydra_Software_Devel/181   5/21/08 2:48p gautamk
* PR42859: [7405] Changing BDBG_ERR to BDBG_MSG
* 
* Hydra_Software_Devel/180   5/20/08 7:09p gautamk
* PR42807: [7335] Fixing the error condition incase of watchdog recovery
* 
* PR42837:[7325]
* 
* Hydra_Software_Devel/179   5/16/08 7:10p gautamk
* PR42752: [7405] Sending other command for atask only when Ack for start
* task is recieved
* 
* Hydra_Software_Devel/178   5/15/08 3:36p gautamk
* PR42664: [7405] setting hTask->bStopped to false when watchdog recovery
* is called so that no command gets called till start of channel
* 
* Hydra_Software_Devel/177   5/13/08 6:26p gautamk
* PR42504:[7405] Not sending task command if the stop command for task
* already sent.
* 
* PR42627: [7405]
* 
* Hydra_Software_Devel/176   5/10/08 5:08a sushmit
* PR 42531: [7335] Making RfMode as the default.
* 
* Hydra_Software_Devel/175   5/9/08 4:17p gautamk
* PR42302:[7405] If watchdogISR is trigerred in between when PI waiting a
* response/ack from FW DSP, Then PI shoul exit gracefully. And then
* ProcessWatchdogRecovery() function should be called
* 
* PR42504:[7405]
* 
* Hydra_Software_Devel/174   5/9/08 1:28p speter
* PR 38950: [3548,3556] Adding SetTsmThreshold support
* 
* Hydra_Software_Devel/173   5/6/08 8:25p gautamk
* PR41726: [7405] removing compilation warning
* 
* Hydra_Software_Devel/172   5/6/08 6:35p gautamk
* PR41726: [7405] Programming TSm condig parameter correctly .
* 
* Hydra_Software_Devel/171   5/5/08 9:57p gautamk
* PR41726: [7405] return error if the streaminfo is not valid.
* 
* Hydra_Software_Devel/170   5/5/08 5:26p gautamk
* PR41726: [7405]  For stream info, Streaminfo is invaid if its not equal
* to 0.
* 
* Hydra_Software_Devel/169   5/3/08 1:52a gautamk
* PR41726: [7405] Update for TSM config params
* 
* Hydra_Software_Devel/168   5/2/08 2:42p gautamk
* PR41726: [7405] Adding PI support for Fw Verification in mainline
* 
* Hydra_Software_Devel/167   4/30/08 7:42p gautamk
* PR42335: [7405] downgrade non-error BDBG_ERR's to either WRN or MSG
* 
* Hydra_Software_Devel/166   4/30/08 6:59p gautamk
* PR41726: [7405] Adding code to program user config param for processing
* algos on the fly.
* 
* Hydra_Software_Devel/165   4/28/08 11:41a gautamk
* PR41726: [7405] Changes for WMA PRO
* 
* Hydra_Software_Devel/164   4/25/08 5:19p gautamk
* PR41726: [7405] PI support for WMA PRO
* 
* Hydra_Software_Devel/163   4/24/08 2:35p gautamk
* PR41726: [7405] Modifying TSM config params
* 
* Hydra_Software_Devel/162   4/24/08 10:14a speter
* PR 38950: [3548] Adding Custom Voice support
* 
* Hydra_Software_Devel/161   4/22/08 4:57p speter
* PR 38950: [3548] Adding CustomVoice (Post Processing) Support
* 
* Hydra_Software_Devel/160   4/21/08 6:21p gautamk
* PR41726: [7405] IMplementing review comments start/stop APis
* 
* Hydra_Software_Devel/159   4/18/08 11:22a sushmit
* PR 41893: [7405] Making default DD Convert as false.
* 
* Hydra_Software_Devel/158   4/17/08 1:20p gautamk
* PR41877: [7405] Checking hDestHandle for NULL before dereferncing it.
* 
* Hydra_Software_Devel/157   4/16/08 6:01p gautamk
* PR40920: [7405] Adding PI support for ASTM on new arch code (base3).
* 
* Hydra_Software_Devel/156   4/16/08 12:06p gautamk
* PR41726: [7405] Coverity fixes.
* 
* Hydra_Software_Devel/155   4/15/08 3:29p sushmit
* PR 41726: [7405] Fixing Dst Config for internal capture.
* 
* Hydra_Software_Devel/154   4/15/08 1:02p sushmit
* PR41726:[7405] Quick fix for audiotype till streaminfo for passthru is
* added.
* 
* Hydra_Software_Devel/153   4/14/08 2:57p sushmit
* pr 34648: [7405] Fixing DesTroyProcessingStage
* 
* Hydra_Software_Devel/152   4/14/08 1:33p gautamk
* PR34648: [7405] Changiong the streaminfo parameter name for DDp.
* Also fixing warning in brap_af_priv.c
* 
* Hydra_Software_Devel/151   4/14/08 11:37a sushmit
* PR 34648: [7405] Reducing Stack Usage
* 
* Hydra_Software_Devel/150   4/13/08 11:01p sushmit
* PR 34648: [7405] Fixing Start-Stop-Start in Brutus when PP Stages are
* enabled.
* 
* Hydra_Software_Devel/149   4/11/08 6:24p sushmit
* PR 34648: [7405] Fixing Create/Destroy Processing Stage API.
* 
* Hydra_Software_Devel/148   4/11/08 5:13p sushmit
* PR 41365: [7405] Allowing only AC3 Passthru without license.
* 
* Hydra_Software_Devel/147   4/11/08 4:37p sushmit
* PR 41365: [7405] Allowing only AC3 Passthru without license.
* 
* Hydra_Software_Devel/146   4/11/08 4:16p sushmit
* PR34648: [7405] Adding frame advance time.
* 
* Hydra_Software_Devel/145   4/11/08 12:50a gautamk
* PR34648: [7405] Adding PI support for Trick modes.
* 
* Hydra_Software_Devel/144   4/10/08 6:12p sushmit
* PR 34648: [7405] DDP Passthru Bringup.
* 
* Hydra_Software_Devel/143   4/8/08 4:37p gautamk
* PR34648: [7405] Fixing compilation break
* 
* Hydra_Software_Devel/142   4/8/08 2:19p gautamk
* PR34648: [7405] Merging wdog code to mainline
* 
* Hydra_Software_Devel/RAP_NEWFWARCH_WDOG/6   4/8/08 1:55p gautamk
* PR34648: [7405] Merging wdog code to mainline
* 
* Hydra_Software_Devel/RAP_NEWFWARCH_WDOG/5   4/8/08 1:18p gautamk
* PR34648: [7405] adding code for wdog...
* 
* Hydra_Software_Devel/RAP_NEWFWARCH_WDOG/4   4/3/08 2:23p gautamk
* PR34648:[7405] Adding code for wdog
* 
* Hydra_Software_Devel/RAP_NEWFWARCH_WDOG/3   4/1/08 2:41p gautamk
* PR34648: [7405] Adding changes in WDOg branch
* 
* Hydra_Software_Devel/RAP_NEWFWARCH_WDOG/2   4/1/08 2:24p gautamk
* PR34648: [7405] Merging change to wdog branch
* 
* Hydra_Software_Devel/136   3/31/08 4:34p sushmit
* PR 34648: [7405] Modifying TSM Config Params & adding DDP Files.
* 
* Hydra_Software_Devel/135   3/31/08 12:13p nitinb
* PR 34648: [7405] Implemented review comments: Removed unused handle
* hArc from memory read/write functions. Removed function
* BRAP_P_DummyMemRead32().
* 
* Hydra_Software_Devel/134   3/27/08 4:42p sushmit
* PR 34648: [7405] Fixing DSP Context Free
* 
* Hydra_Software_Devel/133   3/27/08 12:15p sushmit
* PR 40888: [7405] Fixing restart of playback
* 
* Hydra_Software_Devel/132   3/26/08 3:44p sushmit
* PR 40888:[7405] Adding DSP Free at StopCxt. Temporary checkin only for
* playback
* 
* Hydra_Software_Devel/131   3/26/08 2:26p sushmit
* PR 40888: [7405] Adding DSP Free at StopCxt
* 
* Hydra_Software_Devel/130   3/25/08 7:45p sushmit
* PR 34648: [7405] Making PI changes for playback support
* 
* Hydra_Software_Devel/129   3/24/08 2:48p sushmit
* PR 34648: [7405] Kernel Mode Unaligned Memory Access Fix, removing
* BDBG_ERRs, fix for multiple dest.
* 
* Hydra_Software_Devel/128   3/19/08 5:04p gautamk
* PR34648: [7405] replacing BDBG_ERR with BDBG_MSG
* 
* Hydra_Software_Devel/127   3/18/08 6:59p gautamk
* PR34648: [7405] Fixing Bugs for GenCItOutput and createProcessing stage
* 
* Hydra_Software_Devel/126   3/18/08 6:39p sushmit
* PR 34648: [7405] Fixing reading of TSM Status
* 
* Hydra_Software_Devel/125   3/18/08 5:27p sushmit
* PR 34648: [7405] Fixing Status Buffer Reading
* 
* Hydra_Software_Devel/124   3/17/08 10:37p sushmit
* PR 34648: [7405] Adding default event enable mask to start task params
* & fixing HDMI passthru when both SPDIF & HDMI are configured for
* passthru
* 
* Hydra_Software_Devel/123   3/17/08 3:28p gdata
* PR34648: [7405] Adding the isr version of write and read message queue
* 
* Hydra_Software_Devel/122   3/14/08 11:52a gautamk
* PR34648: [7405] Merging the branched code to mainline
* 
* Hydra_Software_Devel/121   3/13/08 4:19p sushmit
* PR 34648: [7405] Removing extra TSMConfig Params
* 
* Hydra_Software_Devel/120   3/13/08 2:44p sushmit
* PR 34648: [7405] Checking in AACHE Support Changes
* 
* Hydra_Software_Devel/119   3/6/08 10:08p gautamk
* PR34648: [7405] Adding extra parameter in
* BRAP_FWIF_P_SetDecodeStageConfigParams to return Actual Config size.
* 
* Hydra_Software_Devel/118   3/6/08 9:46p gautamk
* PR34648: [7405] Fixing bug in setTSmConfig
* 
* Hydra_Software_Devel/117   3/6/08 9:41p gautamk
* PR34648: [7405] Fixing bug in BRAP_DSPCHN_P_SetTsmConfig
* 
* Hydra_Software_Devel/116   3/6/08 9:24p sushmit
* PR 34648: [7405] AudioType Should be AC3Plus
* 
* Hydra_Software_Devel/115   3/6/08 8:14p gautamk
* PR34648: [7405] Correcting bug in setTSMConfig
* 
* Hydra_Software_Devel/114   3/6/08 6:17p sushmit
* PR 34648: [7405] Fixing TSM Config buffer address conversion
* 
* Hydra_Software_Devel/113   3/5/08 9:31p sushmit
* PR 34648: [7405] Adding some checks for channel started
* 
* Hydra_Software_Devel/112   3/4/08 11:33p sushmit
* PR 34648: [7325] Fixing APIs BRAP_EnableTSM etc.
* 
* Hydra_Software_Devel/111   3/4/08 7:29p gautamk
* PR34648: [7405] Increasing the size of Threshold value passed at start
* task time to 3008.
* 
* Hydra_Software_Devel/110   3/4/08 6:40p gautamk
* PR34648: [7405] adding paramater in hRapch to have the userconfig
* parameters in Appformat for some fields.
* 
* Hydra_Software_Devel/109   3/4/08 6:01p sushmit
* PR 34648: [7405] Fixing some errors & adding workaround for CIT not
* populating invalid dram address for status buffers for passthru case
* 
* Hydra_Software_Devel/108   3/3/08 6:53p srajapur
* PR34648: [7405] Fixed the coverity issues.
* 
* Hydra_Software_Devel/107   3/3/08 2:49p gautamk
* PR34648: [7405] Fixing coverity defects.
* 
* Hydra_Software_Devel/106   2/29/08 8:34p gautamk
* PR40131: [7405] Ac3 DualMono mode enum between FW and PI had different
* values. Adding function convert it.
* 
* Hydra_Software_Devel/105   2/29/08 7:21p gdata
* PR40134: [7405] Correcting the event wait from different locations
* 
* Hydra_Software_Devel/104   2/29/08 3:47p gautamk
* PR34648: [7405] Removing Unwanted function
* 
* Hydra_Software_Devel/103   2/29/08 11:25a gdata
* PR34648: [7405] Correcting the error return if task ack fail
* 
* Hydra_Software_Devel/102   2/28/08 5:26p gdata
* PR34648: [7405] Changing the implementation for TSM for disabling
* response/ack
* 
* Hydra_Software_Devel/101   2/28/08 4:52p gdata
* PR34648: [7405] Implementing API
* BRAP_DSPCHN_P_GetDefaultSettings , BRAP_DSPCHN_P_GetDefaultParams,
* BRAP_DSPCHN_P_GetCurrentSettings, BRAP_DSPCHN_P_GetCurrentAudioParams,
* BRAP_GetDecoderLockStatus
* 
* Hydra_Software_Devel/100   2/26/08 7:24p gautamk
* PR34648: [7405] Changing the structure names from DDp to Ac3Plus
* 
* Hydra_Software_Devel/99   2/26/08 6:38p gautamk
* PR34648: [7405] Fixing bugs and programming RDBDsp Offset to CIT.
* 
* Hydra_Software_Devel/98   2/22/08 4:17p gautamk
* PR34648: [7405] fixing coverity defects + implementing review comments
* 
* Hydra_Software_Devel/97   2/19/08 7:58p gautamk
* PR34648: [7405] Fixing Compiler warnings.
* 
* Hydra_Software_Devel/96   2/19/08 4:06p gautamk
* PR34648: [7405] Updating function names.
* 
* Hydra_Software_Devel/95   2/19/08 3:31p gautamk
* PR34648: [7405] updating file names and fuction names
* 
* Hydra_Software_Devel/94   2/19/08 2:43p gautamk
* PR34648: [7405] updating dsp allocation at start time.
* 
* Hydra_Software_Devel/93   2/18/08 3:40p sushmit
* PR 34648: [7405] Adding Memset of zeroes
* 
* Hydra_Software_Devel/92   2/18/08 3:27p gdata
* PR34648: [7405] Correcting the logic of getdspch handle
* 
* Hydra_Software_Devel/90   2/18/08 3:03p gautamk
* PR34648: [7405] Changes for moving DSP allocation at start time
* 
* Hydra_Software_Devel/89   2/18/08 2:59p sushmit
* PR34648: [7405] PI Changes for OpPortConfig to CIT Input
* 
* Hydra_Software_Devel/88   2/15/08 10:10p gautamk
* PR34648: [7405] Fixing bug for donloading passthru exec.
* 
* Hydra_Software_Devel/87   2/15/08 9:14p gautamk
* PR34648: [7405] correcting bug while downloadding passthru exec
* 
* Hydra_Software_Devel/86   2/15/08 8:55p gautamk
* PR34648: [7405] Adding image id for passthru interframe.
* 
* Hydra_Software_Devel/85   2/15/08 8:44p gautamk
* PR34648: [7405]Adding User Config for Passthru
* Adding spdif CBit buffer.
* 
* Hydra_Software_Devel/84   2/15/08 5:26p gdata
* PR34648: [7405] Adding the changes to support the interrupt install
* before channel start
* 
* Hydra_Software_Devel/83   2/15/08 3:47p gdata
* PR34648: [7405] Enabling the asynchronous interrupt at dspchn start
* time
* 
* Hydra_Software_Devel/82   2/13/08 8:53p gautamk
* PR34648: [7405] Modifying default value of user config
* 
* Hydra_Software_Devel/81   2/13/08 8:15p gautamk
* PR34648:[7405] Adding BRAP_P_MapMpegUserConfigFwtoApp()
* 
* Hydra_Software_Devel/80   2/13/08 11:25a sushmit
* PR34648: [7405] Fixing issue with default dspchn params
* 
* Hydra_Software_Devel/79   2/12/08 2:04p gautamk
* PR34648: [7405] updating the function BRAP_DSPCHN_P_CreateFwTask(),
* and allocating DSP to task in it.
* 
* Hydra_Software_Devel/78   2/11/08 7:02p gautamk
* PR34648: [7405] Adding PI Support for DDP.
* 
* Hydra_Software_Devel/77   2/11/08 2:29p gautamk
* PR34648: [7405] Adding FreeCtxBuffer Function to free the buffer usage
* at DSPCHN_STOP.
* Adding support for Decode TSM
* 
* Hydra_Software_Devel/76   2/8/08 6:33p gdata
* PR34648: [7405] Removing the compilation break in brutus build
* 
* Hydra_Software_Devel/75   2/8/08 5:23p gdata
* PR34648: [7405]: correcting the faster interface implementation.
* 
* Hydra_Software_Devel/74   2/7/08 5:37p nitinb
* PR34648: [7405] Fixed compilation warnings.
* 
* Hydra_Software_Devel/73   2/7/08 2:40p gautamk
* PR34648: [7405] implementing getDefaultConfigParams,
* getCurrentConfigparams.
* Removing compilation warning of
* brap_fwif.h
* brap_fwif_user_config.h
* brap_fwif_proc.c
* brap_fwif_dwnldutil.c
* 
* Hydra_Software_Devel/72   2/6/08 8:09p nitinb
* PR34648: [7405] Fixed compilation warnings.
* 
* Hydra_Software_Devel/71   2/6/08 12:34p nitinb
* PR34648: [7405] Fixed reading of status buffer from dram in
* BRAP_P_GetxxxInfo functions.
* 
* Hydra_Software_Devel/70   2/4/08 8:28p gautamk
* PR34648: [7405] Coorecting config pamas for Ac3
* 
* Hydra_Software_Devel/69   2/4/08 8:17p gautamk
* PR34648: [7405] Changes for CHG_CFG command
* 
* Hydra_Software_Devel/68   2/4/08 2:16p gautamk
* PR34648: [7405] Adding function to conert to Q1.31 .
* 
* Hydra_Software_Devel/67   2/4/08 1:57p gautamk
* PR34648: [7405] Updating setconfig function.
* 
* Hydra_Software_Devel/66   2/1/08 9:11p gdata
* PR34648: [7405] Adding faster implementation changes
* 
* Hydra_Software_Devel/65   1/31/08 7:55p nitinb
* PR34648: [7405] Implemented review comments on stack usage
* 
* Hydra_Software_Devel/64   1/31/08 5:22p gautamk
* PR34648: [7405] Removing BRAP_DSPCHN_DecodeMode_eSimulMode Enum
* 
* Hydra_Software_Devel/63   1/31/08 4:41p gautamk
* PR34648: [7405] Changes for TSM user config
* 
* Hydra_Software_Devel/62   1/31/08 1:58p gautamk
* PR34648: [7405] Changes for TSM config params and Added CreateFwTask
* Function
* 
* Hydra_Software_Devel/61   1/30/08 4:36p gautamk
* PR34648: [7405] Updating design for downloading FW. Now download FW ids
* are coming from common arrary between PI/FW
* 
* Hydra_Software_Devel/60   1/30/08 3:05p gdata
* PR34648: [7405] Adding support for status buffers
* 
* Hydra_Software_Devel/59   1/30/08 1:42p gdata
* PR34648: [7405] Correcting the call to BRAP_FWIF_P_GetMsg
* 
* Hydra_Software_Devel/58   1/29/08 7:04p gautamk
* PR34648: [7405] programming CDB/ITB END_PTR
* 
* Hydra_Software_Devel/57   1/29/08 4:16p gautamk
* PR34648: [7405] Adding config params for MPEG
* 
* Hydra_Software_Devel/56   1/28/08 7:46p gautamk
* PR34648: [7405] Adding config param programming for Ac3 as per new
* design in which hRap stores config struct in the Format FW expects
* 
* Hydra_Software_Devel/55   1/28/08 3:13p gautamk
* PR34648: [7405] removing unwanted structure and includes
* 
* Hydra_Software_Devel/54   1/25/08 7:55p gdata
* PR 34648: [7405] Restting the task id in rap handle
* 
* Hydra_Software_Devel/53   1/25/08 2:49p gautamk
* PR34648: [7405] Fixing bug to form processing network and CIT input
* 
* Hydra_Software_Devel/52   1/25/08 2:40p gautamk
* PR34648: [7405] Fixing bug to form processing network and CIT input
* 
* Hydra_Software_Devel/51   1/25/08 2:11p sushmit
* PR 34648: [7405] Updating PI for Passthru Bringup
* 
* Hydra_Software_Devel/50   1/25/08 12:29p sushmit
* PR 34648: [7405] Updated passthru BRUP Code.
* 
* Hydra_Software_Devel/49   1/24/08 3:50p gdata
* PR 34648: [7405] adding support for stop dpschan
* 
* Hydra_Software_Devel/48   1/24/08 2:25p sushmit
* PR34648: [7405] Updating code for Passthru bringup
* 
* Hydra_Software_Devel/47   1/24/08 1:26p sushmit
* PR34648: [7405] Updating code for Passthru bringup
* 
* Hydra_Software_Devel/46   1/24/08 8:18a sushmit
* PR34648: [7405] Merging in updated code from Base2
* 
* Hydra_Software_Devel/45   1/22/08 1:35p sushmit
* PR 34648: [7405] AC3 Bringup Changes.
* 
* Hydra_Software_Devel/44   1/21/08 11:50a sushmit
* PR 34648: [7405] AC3 Bringup Changes.
* Change AUD_START_TASK_ID_OFFSET
* Convert MIT Addrs to physical addresses
* Fix FIFO Addr Table Generation
* Take care of AC3 IDS instead of AC3 FrameSync
* Take care of AC3 IDS Interframe requirement
* Convert CIT Virt Addr to Phys Addrs
* 
* Hydra_Software_Devel/43   1/18/08 3:53p gautamk
* PR34648: [7405] Changes for statr time download
* 
* Hydra_Software_Devel/42   1/18/08 3:24p sushmit
* PR 34648: [7405] Getting Brutus compiled.
* 
* Hydra_Software_Devel/41   1/18/08 11:56a gautamk
* PR34648: [7405] updating files for initializing interframe and user
* config params.
* 
* Hydra_Software_Devel/40   1/17/08 5:08p sushmit
* PR34648: [7405] AC3 Bringup Changes
* 
* Hydra_Software_Devel/39   1/17/08 12:00p sushmit
* PR34648: [7405] Updating AC3 Img Ids
* 
* Hydra_Software_Devel/38   1/16/08 5:04p gautamk
* PR34648: [7405] using cache to download memory into DRAm
* 
* Hydra_Software_Devel/37   1/16/08 4:49p nitinb
* PR34648: [7405] Added support for programming configuration parameters
* 
* Hydra_Software_Devel/36   1/16/08 3:21p gautamk
* PR34648: [7405] Checkin in updated code
* 
* Hydra_Software_Devel/35   1/16/08 11:57a sushmit
* PR34648: [7405] Downloading CIT to DRAM
* 
* Hydra_Software_Devel/34   1/15/08 5:02p sushmit
* PR34648: [7405] Moving CIT Output malloc to DRAM
* 
* Hydra_Software_Devel/33   1/15/08 2:30p sushmit
* PR34648: [7405] Moving CIT Output malloc to DRAM
* 
* Hydra_Software_Devel/32   1/15/08 1:37p sushmit
* PR34648: [7405] Checkin in updated code
* 
* Hydra_Software_Devel/31   1/15/08 12:01p sushmit
* PR34648: [7405] Checkin in updated code
* 
* Hydra_Software_Devel/30   1/14/08 3:09p gautamk
* PR34648: [7405] Changes for having only one passthru exec for all exec.
* And Correting the Rave Pointers sent to CIT
* 
* Hydra_Software_Devel/29   1/11/08 6:40p gautamk
* PR34648: [7405] Files for Mapping Processing network to CIT input
* 
* Hydra_Software_Devel/28   1/11/08 12:14p sushmit
* PR34648: [7405] Temporary fix for seg fault
* 
* Hydra_Software_Devel/27   1/10/08 7:09p gautamk
* PR34648: [7405] Files for Mapping Processing network to CIT input
* 
* Hydra_Software_Devel/26   1/10/08 6:44p gautamk
* PR34648: [7405] Files for Mapping Processing network to CIT input
* 
* Hydra_Software_Devel/25   1/10/08 3:18p sushmit
* PR 34648: [7405] Removing hDsp from hDspChn
* 
* Hydra_Software_Devel/24   1/10/08 2:25p sushmit
* PR 34648: [7405] Fixing hDsp population
* 
* Hydra_Software_Devel/23   1/9/08 10:36p sushmit
* PR 34648: [7405] Updating FW-PI Interface after syncup meeting.
* 
* Hydra_Software_Devel/22   1/9/08 3:49p nitinb
* PR34648: [7405] Fixed compilation error.
* 
* Hydra_Software_Devel/21   1/9/08 3:41p nitinb
* PR34648: [7405] Added support for programming configuration parameters
* 
* Hydra_Software_Devel/20   1/8/08 8:46p sushmit
* PR34648: [7405] Updating Start
* 
* Hydra_Software_Devel/19   1/7/08 4:48p sushmit
* PR34648: [7405] Updating Start
* 
* Hydra_Software_Devel/18   1/7/08 4:18p sushmit
* PR34648: [7405] Updating Start
* 
* Hydra_Software_Devel/17   1/7/08 3:54p sushmit
* PR34648: [7405] Updating Start
* 
* Hydra_Software_Devel/16   1/7/08 3:44p sushmit
* PR34648: [7405] Adding Event Creation
* 
* Hydra_Software_Devel/15   1/7/08 2:26p sushmit
* PR34648: [7405] Adding Start Task Command
* 
* Hydra_Software_Devel/14   1/4/08 5:53p bhanus
* PR34648: [7405] Adding code for new changed interface of
* BRAP_FWIF_P_GenerateCitInput().
* 
* Hydra_Software_Devel/13   1/3/08 6:52p nitinb
* PR 34648: [7405] Integration of FW-Interface module with other modules
* 
* Hydra_Software_Devel/12   1/3/08 2:21p bhanus
* PR34648: [7405] Changing unsigned int variables to uint32_t
* 
* Hydra_Software_Devel/11   12/28/07 9:53p sushmit
* PR 34648: [7405] Initial Checkins after DSP Bootup completion.
* 
* Hydra_Software_Devel/10   12/20/07 2:52p srajapur
* PR 34648: [7405] Implemented APIs for creating/destroying audio
* processing stages and other API's. Added BRAP_P_DestroyProcessingStage
* API.
* 
* Hydra_Software_Devel/9   12/14/07 10:19p gautamk
* PR34648:  [7405] Adding updated files for downloading FW module.
* 
* Hydra_Software_Devel/8   12/14/07 1:58p gautamk
* PR34648:  [7405] Adding updated files for downloading FW module.
* 
* Hydra_Software_Devel/6   11/15/07 10:46p nitinb
* PR 34648: [7405] Merged converter and encoder enums into
* AudioProcessing enum
* 
* Hydra_Software_Devel/5   10/31/07 1:51p nitinb
* PR 34648: [7405] Implemented review comments of new API interface as
* mentioned in PR progress-log
* 
* Hydra_Software_Devel/4   10/25/07 5:28p nitinb
* PR 34648: [7405] Added APIs for creating/destroying audio processing
* stages and programming their configuration parameters.
* 
***************************************************************************/

#include "brap.h"
#include "brap_priv.h"
#include "brap_img.h"
#include "bchp_xpt_rave.h"
#include "brap_af_priv.h"

BDBG_MODULE(rap_dspchn);
extern  BRAP_FWIF_P_MapProcessingEnum BRAP_sMapProcessingEnum[/*BRAP_ProcessingType_eMax*/];
extern const BRAP_AF_P_sALGO_EXEC_INFO BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_eMax][BRAP_DSPCHN_DecodeMode_eMax];
extern const BRAP_AF_P_sALGO_EXEC_INFO BRAP_sEncAlgoExecInfo[BRAP_CIT_P_EncAudioType_eMax][BRAP_DSPCHN_EncodeMode_eSimulMode];
extern const BRAP_AF_P_sALGO_EXEC_INFO BRAP_sPpAlgoExecInfo[BRAP_CIT_P_ProcessingType_eMax][BRAP_DSPCHN_ProcessingMode_eMax];

#define BRAP_DSP_P_DEFAULT_WATCHDOG_COUNT	(0x3FFFFFFF)
#define BRAP_CIT_DEBUG_PRINT 0

/* This table implements 10^(Gpk / 40) Where Gpk ranges from 0 to 120 */
static const uint16_t ui16TenPowerXBy40 [] = 
{
    10000,10057,10115,10174,10232,10292,10351,10411,10471,10531,
    10592,10653,10715,10777,10839,10901,10964,11028,11091,11155,
    11220,11284,11350,11415,11481,11547,11614,11681,11748,11816,
    11885,11953,12022,12092,12161,12232,12302,12373,12445,12516,
    12589,12661,12735,12808,12882,12956,13031,13106,13182,13258,
    13335,13412,13489,13567,13645,13724,13803,13883,13963,14044,
    14125,14206,14288,14371,14454,14537,14621,14706,14791,14876,
    14962,15048,15135,15222,15310,15399,15488,15577,15667,15757,
    15848,15940,16032,16125,16218,16311,16405,16500,16595,16691,
    16788,16884,16982,17080,17179,17278,17378,17478,17579,17680,
    17782,17885,17988,18092,18197,18302,18407,18513,18620,18728,
    18836,18945,19054,19164,19275,19386,19498,19611,19724,19838,
    19953
};

/* This table contains the subsonic filter coefficients for PEQ PP algorithm */

static const int i16SubsonicFilterCoeffs [3][19][12] = 
{
    {
        {
            99278,-198556,99278,
            00000,-198553,98559,
            99698,-199397,99698,
            00000,-199394,99400   
        },
        {
            99188,-198376,99188,
            00000,-198372,98380,
            99661,-199322,99661,
            00000,-199318,99326
        },
        {
            99098,-198197,99098,
            00000,-198192,98202,
            99623,-199246,99623,
            00000,-199241,99251
        },
        {
            98991,-197982,98991,
            00000,-197976,97988,
            99577,-199155,99577,
            00000,-199149,99161
        },        
        {
            98866,-197732,98866,
            00000,-197725,97740,
            99525,-199050,99525,
            00000,-199042,99057
        },        
        {
            98741,-197482,98741,
            00000,-197473,97492,
            99472,-198944,99472,
            00000,-198934,98953
        },        
        {
            98563,-197127,98563,
            00000,-197114,97139,
            99396,-198792,99396,
            00000,-198780,98804
        },        
        {
            98385,-196771,98385,
            00000,-196756,96787,
            99320,-198641,99320,
            00000,-198625,98656
        },        
        {
            98208,-196417,98208,
            00000,-196398,96436,
            99244,-198489,99244,
            00000,-198470,98508
        },        
        {
            98032,-196064,98032,
            00000,-196041,96087,
            99168,-198337,99168,
            00000,-198314,98360
        },        
        {
            97768,-195536,97768,
            00000,-195507,95565,
            99054,-198109,99054,
            00000,-198079,98139
        },        
        {
            97505,-195010,97505,
            00000,-194973,95047,
            98940,-197880,98940,
            00000,-197843,97918
        },        
        {
            97155,-194311,97155,
            00000,-194263,94359,
            98787,-197575,98787,
            00000,-197526,97624
        },        
        {
            96808,-193616,96808,
            00000,-193556,93677,
            98635,-197269,98635,
            00000,-197208,97331
        },        
        {
            96462,-192925,96462,
            00000,-192850,92999,
            98481,-196963,98481,
            00000,-196887,97039
        },        
        {
            96118,-192237,96118,
            00000,-192147,92326,
            98328,-196656,98328,
            00000,-196565,96748
        },        
        {
            95605,-191211,95605,
            00000,-191096,91326,
            98097,-196195,98097,
            00000,-196077,96313
        },        
        {
            95096,-190193,95096,
            00000,-190049,90337,
            97866,-195732,97866,
            00000,-195584,95880
        },        
        {
            94507,-189014,94507,
            00000,-188834,89195,
            97595,-195191,97595,
            00000,-195004,95378
        }
    },
    {
        {
            99475,-198950,99475,
            00000,-198949,98952,
            99781,-199563,99781,
            00000,-199561,99564
        },        
        {
            99410,-198820,99410,
            00000,-198818,98822,
            99754,-199508,99754,
            00000,-199506,99510
        },        
        {
            99344,-198689,99344,
            00000,-198687,98692,
            99726,-199453,99726,
            00000,-199451,99456
        },
        {
            99266,-198533,99266,
            00000,-198530,98536,
            99694,-199388,99694,
            00000,-199384,99391
        },        
        {
            99175,-198351,99175,
            00000,-198347,98355,
            99655,-199311,99655,
            00000,-199307,99315
        },        
        {
            99084,-198169,99084,
            00000,-198164,98174,
            99617,-199234,99617,
            00000,-199229,99239
        },
        {
            98954,-197909,98954,
            00000,-197903,97915,
            99562,-199124,99562,
            00000,-199118,99131
        },        
        {
            98825,-197650,98825,
            00000,-197642,97658,
            99507,-199015,99507,
            00000,-199007,99023
        },        
        {
            98695,-197391,98695,
            00000,-197381,97401,
            99452,-198905,99452,
            00000,-198895,98915
        },
        {
            98566,-197133,98566,
            00000,-197121,97145,
            99397,-198795,99397,
            00000,-198783,98807
        },        
        {
            98373,-196747,98373,
            00000,-196731,96762,
            99315,-198630,99315,
            00000,-198614,98646
        },        
        {
            98180,-196361,98180,
            00000,-196342,96381,
            99232,-198465,99232,
            00000,-198445,98485
        },
        {
            97924,-195849,97924,
            00000,-195824,95875,
            99122,-198244,99122,
            00000,-198218,98270
        },        
        {
            97669,-195339,97669,
            00000,-195307,95371,
            99011,-198023,99011,
            00000,-197991,98056
        },        
        {
            97415,-194830,97415,
            00000,-194791,94870,
            98901,-197802,98901,
            00000,-197762,97842
        },
        {
            97162,-194324,97162,
            00000,-194276,94372,
            98790,-197581,98790,
            00000,-197532,97629
        },        
        {
            96784,-193568,96784,
            00000,-193506,93629,
            98624,-197248,98624,
            00000,-197185,97311
        },        
        {
            96407,-192815,96407,
            00000,-192739,92892,
            98457,-196915,98457,
            00000,-196836,96993
        },
        {
            95971,-191943,95971,
            00000,-191846,92039,
            98262,-196525,98262,
            00000,-196426,96624
        }    
    },
    {
        {
            99517,-199035,99517,
            00000,-199034,99037,
            99799,-199598,99799,
            00000,-199597,99600
        },
        {
            99457,-198915,99457,
            00000,-198914,98917,
            99774,-199548,99774,
            00000,-199546,99550
        },
        {
            99397,-198795,99397,
            00000,-198793,98797,
            99749,-199498,99749,
            00000,-199496,99500       
        },
        {
            99325,-198651,99325,
            00000,-198649,98654,
            99718,-199437,99718,
            00000,-199435,99440
        },
        {
            99242,-198484,99242,
            00000,-198481,98487,
            99683,-199367,99683,
            00000,-199364,99370
        },
        {
            99158,-198316,99158,
            00000,-198312,98321,
            99648,-199296,99648,
            00000,-199292,99301
        },
        {
            99039,-198078,99039,
            00000,-198072,98083,
            99598,-199196,99598,
            00000,-199190,99201
        },  
        {
            98919,-197839,98919,
            00000,-197832,97846,
            99547,-199095,99547,
            00000,-199088,99102
        
        },
        {
            98800,-197601,98800,
            00000,-197593,97610,
            99497,-198994,99497,
            00000,-198986,99003
        },
        {
            98682,-197364,98682,
            00000,-197353,97374,
            99446,-198893,99446,
            00000,-198883,98904
        },
        {
            98504,-197008,98504,
            00000,-196995,97021,
            99371,-198742,99371,
            00000,-198728,98755
        },
        {
            98326,-196653,98326,
            00000,-196637,96670,
            99295,-198590,99295,
            00000,-198573,98607
        },        
        {
            98091,-196182,98091,
            00000,-196160,96203,
            99194,-198388,99194,
            00000,-198366,98409
        },
        {
            97856,-195712,97856,
            00000,-195685,95739,
            99092,-198185,99092,
            00000,-198157,98212
        },
        {
            97621,-195243,97621,
            00000,-195210,95277,
            98991,-197982,98991,
            00000,-197948,98016
        },
        {
            97388,-194777,97388,
            00000,-194736,94817,
            98889,-197779,98889,
            00000,-197738,97820
        
        },
        {
            97039,-194079,97039,
            00000,-194027,94131,
            98736,-197473,98736,
            00000,-197420,97526
        },
        {
            96692,-193385,96692,            
            00000,-193320,93450,
            98583,-197167,98583,       
            00000,-197101,97234        
        },         
        {
            96290,-192580,96290,
            00000,-192498,92662,
            98405,-196810,98405,
            00000,-196726,96894
        }        
   
    }

};

static const BRAP_DSPCHN_TruSurroundXtParams sDefTruSurroundXtSettings = 	{
			true,								/* bTruSurroundEnable */
			false,								/* bTsHeadphoneEnable */
			100,							    /* uiTruSurroundInputGain */
			true,								/* bDialogClarityEnable */
			50,									/* uiDialogClarityLevel */
			true,								/* bTruBassEnable */
			50,									/* bTbLevel */
			BRAP_DSPCHN_TbSpeakerSize_eLfr60Hz,	/* eTbSpeakerSize */
            false,                              /* bCertificationEnable */
            BRAP_DSPCHN_SRS_AcMode_eMode20          /* eAcMode */			
		};

static const BRAP_DSPCHN_TruSurroundHdParams sDefTruSurroundHdSettings = 	{
            { /* BRAP_DSPCHN_TopLevelStudioSoundParams */
                false,      /* bIsStudioSound */
                BRAP_DSPCHN_StudioSoundMode_eMode1, /* eStudioSoundMode */    
                true,       /* bEnable */
                100,        /* ui32InputGain */
                100,        /* ui32HeadroomGain */
                BRAP_DSPCHN_StudioSound_InputMode_eLtRt, /* ui32InputMode */
                100,        /* ui32OutputGain */
                100,        /* ui32BypassGain */                
            },    
			true,                                   /* bEnable */
			false,								    /* bHeadPhoneEnable */
			50,							            /* ui32InputGain */
			100,								    /* ui32OutputGain */
			100,								    /* ui32ByPassGain */
			BRAP_DSPCHN_SRSTSHDCrossOverFreq_e200Hz,/* eSubCrossOverFreq */
			true,									/* bTruBassFrontEnable */
			30,	                                    /* ui32TruBassFrontLevel */
			BRAP_DSPCHN_SpeakerSize_eLFResponse150Hz,/* eTruBassFrontSpeakerSize */
			BRAP_DSPCHN_SRSTruBassProcessMode_eStereo,/* eSRSTruBassProcessMode */
            true,                                   /* bDefinitionFrontEnable */
            30,                                     /* ui32DefinitionFrontLevel */
            true,                                   /* bDialogClarityEnable */
            50,                                     /* ui32DialogClarityLevel */
            60,                                     /* ui32SurroundLevel */
            true,                                   /* bWowHDSRS3DEnable */
            true,                                   /* bSRS3DHighBitRateEnable */
            BRAP_DSPCHN_SRS3DMode_eStereo,          /* eWowHDSRS3DMode */
            80,                                     /* ui32WowHDSRS3DSpaceLevel */
            50,                                     /* ui32WowHDSRS3DCenterLevel */
            true,                                   /* bWowHDFocusEnable */
            40,                                     /* ui32WowHDFocusLevel */
            true,                                   /* bMonoToStereoEnable */
            BRAP_DSPCHN_SRS_AcMode_eMode20,             /* eOutputAcMode */
            false,                                   /* bOutputLFEEnable */
            false,                                   /* bCertificationApp */          
            false,                                   /* bInputLfeEnable */
            BRAP_DSPCHN_SRS_AcMode_eMode20                                   /* eInputAcmod */
		};

static const BRAP_DSPCHN_SrsTruVolumeParams sDefSrsTruVolumeSettings = 	{
            { /* BRAP_DSPCHN_TopLevelStudioSoundParams */
                false,      /* bIsStudioSound */
                BRAP_DSPCHN_StudioSoundMode_eMode1, /* eStudioSoundMode */    
                true,       /* bEnable */
                100,        /* ui32InputGain */
                100,        /* ui32HeadroomGain */
                BRAP_DSPCHN_StudioSound_InputMode_eLtRt, /* ui32InputMode */
                100,        /* ui32OutputGain */
                100,        /* ui32BypassGain */                
            },    
			true,								/* bSrsTruVolumeEnable */
			256,								/* uiBlockSize */
			true,								/* bEnableNormalGain */
            25,                                 /* uiInputGain */			
            25,                                 /* uiOutputGain */
			100,							    /* uiBypassGain */
			50, 	                            /* uiReferenceLevel */
			false,								/* bEnableDCNotchFilter */			
			BRAP_DSPCHN_SrsTruVolumeMode_eLight,           /* eMode */
			BRAP_DSPCHN_SrsTruVolumeSpeakerResolution_20Hz, /* eSpeakerResolution */
                64,                               /* uiMaxGain */	
            true,                               /* bEnableNoiseManager */
                20,                                  /* uiNoiseManagerThreshold */                
                1,                                /*bEnableNormalizer*/
            100,                              /*uiCalibrate*/
            { /* BRAP_DSPCHN_HighPassFilterParams */
                true,       /* bEnable */
                BRAP_DSPCHN_CoefGenMode_eUserCoef, /* eCoefGenMode */    
                { /* uHPFSettings */
                    { /* sHPFilterCoef[3] */
                        { /* sHPFilterCoef[0] - Sampling frequency - 32 kHz */
                            BRAP_DSPCHN_EQFilterOrder_eOrder4,  /* eFilterOrder */
                            {
                                { /* sHPFilterCoefTdf2[0] */
                                    0x00000001,     /* ui32Scale */ 
                                    0x0fc81f80,     /* i32FilterCoefficientB0 */
                                    0xe06fc100,     /* i32FilterCoefficientB1 */
                                    0x0fc81f80,     /* i32FilterCoefficientB2 */
                                    0x7e36e680,     /* i32FilterCoefficientA1 */
                                    0xc1b4eec0,     /* i32FilterCoefficientA2 */
                                },                        
                                { /* sHPFilterCoefTdf2[1] */
                                    0x00000001,     /* ui32Scale */ 
                                    0x3df563c0,     /* i32FilterCoefficientB0 */
                                    0x84153880,     /* i32FilterCoefficientB1 */
                                    0x3df563c0,     /* i32FilterCoefficientB2 */
                                    0x7be0e200,     /* i32FilterCoefficientA1 */
                                    0xc40b5300      /* i32FilterCoefficientA2 */
                                },
                                { /* sHPFilterCoefTdf2[2] */
                                    0x00000001,     /* ui32Scale */ 
                                    0x0,            /* i32FilterCoefficientB0 */
                                    0x0,            /* i32FilterCoefficientB1 */
                                    0x0,            /* i32FilterCoefficientB2 */
                                    0x0,            /* i32FilterCoefficientA1 */
                                    0x0             /* i32FilterCoefficientA2 */
                                }                                
                            }
                        },
                        { /* sHPFilterCoef[1] - Sampling frequency - 44.1 kHz */
                            BRAP_DSPCHN_EQFilterOrder_eOrder4,  /* eFilterOrder */
                            {
                                { /* sHPFilterCoefTdf2[0] */
                                    0x00000001,     /* ui32Scale */ 
                                    0x0fd78db0,     /* i32FilterCoefficientB0 */
                                    0xe050e4a0,     /* i32FilterCoefficientB1 */
                                    0x0fd78db0,     /* i32FilterCoefficientB2 */
                                    0x7eb71980,     /* i32FilterCoefficientA1 */
                                    0xc13e3e40      /* i32FilterCoefficientA2 */
                                },                        
                                { /* sHPFilterCoefTdf2[1] */
                                    0x00000001,     /* ui32Scale */ 
                                    0x3e826c40,     /* i32FilterCoefficientB0 */
                                    0x82fb2780,     /* i32FilterCoefficientB1 */
                                    0x3e826c40,     /* i32FilterCoefficientB2 */
                                    0x7cff9680,     /* i32FilterCoefficientA1 */
                                    0xc2f5e600      /* i32FilterCoefficientA2 */
                                },
                                { /* sHPFilterCoefTdf2[2] */
                                    0x00000001,     /* ui32Scale */ 
                                    0x0,            /* i32FilterCoefficientB0 */
                                    0x0,            /* i32FilterCoefficientB1 */
                                    0x0,            /* i32FilterCoefficientB2 */
                                    0x0,            /* i32FilterCoefficientA1 */
                                    0x0             /* i32FilterCoefficientA2 */
                                }                                    
                            }
                        },
                        { /* sHPFilterCoef[2] - Sampling frequency - 48 kHz */
                            BRAP_DSPCHN_EQFilterOrder_eOrder4,  /* eFilterOrder */
                            {
                                { /* sHPFilterCoefTdf2[0] */
                                    0x00000001,     /* ui32Scale */ 
                                    0x0fdadc10,     /* i32FilterCoefficientB0 */
                                    0xe04a47e0,     /* i32FilterCoefficientB1 */
                                    0x0fdadc10,     /* i32FilterCoefficientB2 */
                                    0x7ed26000,     /* i32FilterCoefficientA1 */
                                    0xc1249f40      /* i32FilterCoefficientA2 */
                                },          
                                { /* sHPFilterCoefTdf2[1] */
                                    0x00000001,     /* ui32Scale */ 
                                    0x3ea0f4c0,     /* i32FilterCoefficientB0 */
                                    0x82be1680,     /* i32FilterCoefficientB1 */
                                    0x3ea0f4c0,     /* i32FilterCoefficientB2 */
                                    0x7d3d7780,     /* i32FilterCoefficientA1 */
                                    0xc2b9a440      /* i32FilterCoefficientA2 */
                                },                        
                                { /* sHPFilterCoefTdf2[2] */
                                    0x00000001,     /* ui32Scale */ 
                                    0x0,            /* i32FilterCoefficientB0 */
                                    0x0,            /* i32FilterCoefficientB1 */
                                    0x0,            /* i32FilterCoefficientB2 */
                                    0x0,            /* i32FilterCoefficientA1 */
                                    0x0             /* i32FilterCoefficientA2 */
                                }                        
                            }
                        }                    
                    }              
                    /* Default HighPass filter specification - 
                    	ui32CutoffFrequency=180 Hz;
                    	eFilterOrder=4 */                
                }
            }            
		};

static const BRAP_DSPCHN_DolbyVolumeUserConfig sDefDolbyVolUserConfig = 	{
			true,								/* bDolbyVolEna */
			512,								/* iBlockSize */
			20,								/* uiBands */
			2,                                                      /*uiNumInputOutputChannel*/
			74,							    /* uiInputReferenceLevel */
			74, 	                                    /* uiOutputReferenceLevel */
			0,                                              /* iCalibration */
			true,                                            /* bVolModelerEna */
                     false,                                             /* bResetNowFlag */	
                     0,                                             /* iDigitalVolumeLevel */
                     0,                                           /* iAnalogVolumeLevel */                
			9,                                            /* uiLvlAmount */
                     true,                                             /* bVolLvlEnable */	
                     false,                                             /* bEnableMidsideProc */
                     false,                                           /* bHalfmodeEna */                                    
                     true                                           /*bLimiterEnable*/
		};

static const BRAP_DSPCHN_ProLogicllConfigParams sDefProLogicIIConfigSettings =  {
	        true,                                       /* bEnablePLII */
			false,								        /* bEnableAutoBal */
			BRAP_DSPCHN_PLllDecMode_MovieRStandard,	    /* ePLllDecMode */
			BRAP_OutputMode_e3_2,				        /* eOutputMode */
			BRAP_DSPCHN_PLllCenterWidthCfg_36_Degree,   /* eCenterWidCfg */
			3,									        /* ui32DimSetting */
			false,								        /* bEnablePanorama */
			false,								        /* bEnableSurChnShelfFilt */
			false,								        /* bEnableRSurPolInv */
			100,									    /* ui32PcmScaleFactor */
			false,								        /* bDebugOnOff */
			BRAP_DSPCHN_PLllDebugChannelCfg_eLR	        /* eDbgChanCfg */
		};

static const BRAP_DSPCHN_AVLConfigParams sDefAVLConfigSettings = {
			false,								/* bBypass */
			-20,								/* iTarget */
			-31,								/* iLowerBound */
			11,									/* uiFixedBoost */
			2959245,							/* uiRef */
			32736,								/* uiAlpha */
			32256,								/* uiBeta */
			3,									/* uiThreshold */
			750,								/* uiDtfPcnt */
			32767,								/* uiAlpha2 */
			5,									/* uisNsfgr */
			3,									/* uiDtf */							
			1									/* Enable =1 is AVL2.0 and Disable =0 is AVL1.0*/
		};

static const BRAP_DSPCHN_XenConfigParams sDefXenConfigSettings =   {
            false,							    /* bBypass */
            true,							    /* bApply6dBHeadroom */
			-3,									/* iBoostCenter */
			0,									/* iLevelFront */
			0,									/* iLevelRear */
			0,									/* iLevelLfe */
			-1,									/* iLevelOutput */
			140,								/* uiSp2Sp */
			4,									/* uiSp2Ls */
			10,									/* uiExpandMidHigh */
			10,									/* uiExpandMidLow */
			110,								/* uiLpfFc */
			3									/* uiPfBoostGain */
    	};

static const BRAP_DSPCHN_BbeConfigParams sDefBbeConfigSettings = {
			false,								/* bBypassBbe */
			BRAP_DSPCHN_BbeType_e1,			    /* eBbeType */
			false,								/* bAcousticFlatSystem */
			0,									/* uiLoContour */
			0,									/* uiProcess */
			false								/* bApplyHeadroom */
		};

static const  BRAP_DSPCHN_CustomSurroundConfigParams sDefCustomSurroundConfigSettings = 	{
			false,								/* bBypassShSurround */
			false,			                    /* bCombOn */
			0,								    /* uiDelay */
			0,									/* uiVolume1 */
			0,									/* uiVolume2 */			
			0,									/* uiVolume3 */			
			0x1E,								/* uiVolume4 */			
			0x1E,								/* uiVolume5 */			
			0x74,								/* uiLpfFc */
			0x44,     							/* uiLpfQ */
			0x56,                               /* uiLpfGain */
			0x40,                               /* uiFuncVol */
			0x14                                /* uiTrim */
		};

static const BRAP_DSPCHN_CustomBassConfigParams sDefCustomBassConfigSettings =	{
			true,                                                   /* bBypassCustomBass */
			BRAP_DSPCHN_CustomBassOperation_eAgcOnly,               /* eCustomBassOperation */
			BRAP_DSPCHN_CustomBassAgcType_eUseOnlyLowForMonitor,    /* eCustomBassAgcType */
			BRAP_DSPCHN_CustomBassHarmonicsType_eHalfWaveRectifier, /* eCustomBassHarmonicsType */
			1,                                                      /* iAgcGainMax */
			1,                                                      /* iAgcGainMin */
			0x0,                                                    /* uiAgcAttackTime */
			0x0,                                                    /* uiAgcRelTime */
			1,                                                      /* iAgcThreshold */
			1,                                                      /* uiHarmonicsLevel */
			1,                                                      /* uiLpfFc */
			1,                                                      /* uiHpfFc */
			1,                                                      /* uiAgcHpfFc */
			1,                                                      /* uiHarmonicsLpfFc */
			1                                                       /* uiHarmonicsHpfFc */
		};

static const BRAP_DSPCHN_CustomVoiceConfigParams sDefCustomVoiceConfigSettings =	{
            1,          /* CV3Enable;   */
            0x00000001,
            0x00000002, /*param02*/
            0x00000001,
            0x406f4a00, /*param65*/
            0x04FD6880, /*param66*/
            0x04444400, /*param71*/
            0x2aaaaa00, /*param72*/
            0x00003000, /*param39*/
            0x40a5c000, /*param40*/
            0x05B7B100, /*param48*/
            0x598C8180, /*param49*/
            0x40000000, /*param100*/
            0x00000003, /*param102*/
            0x287A2680, /*param103*/
            0x00000001, /*param104*/
            0x51EB8005,
            0x00000000,
            0x00004189,
            0x00378000,
            0xd3000000, /*param52*/
            0xe7000000,
            0x40000000,
            0x40000000, /*param97*/
            0x00000000, /*param59*/
            0x007FC000, /*param55*/
            0x59f98000, /*param57*/
            0x199b7200, /*param58*/
            0x00000007, /*param87*/
            0x08a3c500,
            0x59992d00,
            0x16666d00,
            0x2666d300,
            0x0008a3c5, /*param92*/
            0x16a77d00, /*param93*/
            0x0000000d, /*param101*/
            0x4ccccd00, /*param79*/
            0x33333300, /*param80*/
            0x28000000, /*param81*/
            0x40000000, /*param99*/
            0x00000001, /*param98*/
            0x000000c8, /*param82*/
        	0x0146e700, /*param26*/
        	0xef02a300,
        	0x78000000,
        	0x78000000,
        	0x16c31080,
        	0x59f98000,
        	0x287A2680, /*param32*/
        	0x10000000, /*param23*/
        	0x1e000000, /*PARAM188*/
        	0xe2000000, /*PARAM189*/
        	0x4b000000, /*PARAM190*/
        	0xb5000000, /*PARAM191*/
        	0x0000001f, /*PARAM217*/
        	0x0000000b, /*PARAM218*/
        	0xc0000000, /*PARAM055*/
        	0xd9999a00, /*PARAM056*/
        	0x00000005, /*PARAM132*/
        	0x00000009, /*PARAM133*/
        	0x00000005, /*PARAM134*/
        	0x0000000a, /*PARAM135*/
        	0x00000028, /*PARAM144*/
        	0x00000028, /*PARAM145*/
        	0x0000012c, /*PARAM146*/
        	0x028F5C00, /*param25*/
            /* table_ram23[11] */
            {
                0xcae1e900,
                0xdccd0000,
                0xe584fc00,
                0xea63d900,
                0xed9b6b00,
                0xf0fd2400,
                0xef73fa00,
                0xf7399700,
                0xf9fcae00,
                0xfcffc000,
                0x00000000
            },
            /* sFreqDepndenti32params[3] */
            {
                {
                    0x00000019, /*param20*/
                    0x001A3680, /*param21*/
                    0x0CCCCC80, /*param22*/
                    0x00000007, /*param42*/
                    /*param46[7]*/
                    {
                        0x11a4d180,
                        0x0e0bcc80,
                        0x0c0f6880,
                        0x0a5ae580,
                        0x088fa980,
                        0x068dfc80,
                        0x0611e680
                    },
                    /*param47[7]*/
                    {
                        0x6640e000,
                        0x63a50a00,
                        0x61f02780,
                        0x6042c000,
                        0x5e352800,
                        0x5b662980,
                        0x5a9b5400
                    },
                    /*param94[20]*/
                    {
                        0x10000000,
                        0x10000000,
                        0x10000000,
                        0x10000000,
                        0x10000000,
                        0x10000000,
                        0x10000000,
                        0x10000000,
                        0x10000000,
                        0x10000000,
                        0x10000000,
                        0x10000000,
                        0x10000000,
                        0x10000000,
                        0x10000000,
                        0x10000000,
                        0x10000000,
                        0x10000000,
                        0x10000000,
                        0x10000000
                    },
                    0x01767d80, /*param109*/
                    0x04189300, /*param110*/
                    0x0006d3a0, /*param19*/
                    /*param51[8]*/
                    {
                        0x08000000,
                        0x2cfcc000,
                        0x2521f900,
                        0x1ea64500,
                        0x194c5800,
                        0x113c4800,
                        0x00000000,
                        0x00000000
                    },
                    0x00000008, /*param50*/
                    0x7ff5c200, /*param73*/
                    0x00051e00, /*param74*/
                    0x00000011, /*param64*/
                    0x00000000, /*param63*/
                    0x00000000, /*param61*/
                    0x00000014, /*param62*/
        			0x58469f00,	/*param33*/
        			0x58469f00,	/*param34*/
        			0x1611a780,	/*param35*/
        			0x57857880,	/*param36*/
        			0x4f613d80,	/*param37*/
        			0x01c39500,	/*param38*/
        			0x0369D000, /*param111*/
        			0x147ae100,
        			0x6b851f00,
        			0x147AE100,
        			0x00d70a3e,
        			0x19999980,
        			0x33333300
                },
                {
                    0x00000022, /*param20*/ 
                    0x00130580, /*param21*/
                    0x0949B900, /*param22*/
                    0x00000006, /*param42*/
                    /*param46[7]*/
                    {
                        0x118ed700,
                        0x0d822e80,
                        0x0af3d980,
                        0x08de8e80,
                        0x0730f000,
                        0x0611e680,
                        0x00000000
                    },
                    /*param47[7]*/
                    {
                        0x66326880,
                        0x63347a00,
                        0x60dfe880,
                        0x5e960180,
                        0x5c5d3700,
                        0x5a9b5400,
                        0x00000000
                    },
                    /*param94[20]*/
                    {
                        0x10000000,
                        0x10000000,
                        0x10000000,
                        0x10000000,
                        0x10000000,
                        0x10000000,
                        0x10000000,
                        0x10000000,
                        0x10000000,
                        0x10000000,
                        0x10000000,
                        0x10000000,
                        0x10000000,
                        0x10000000,
                        0x10000000,
                        0x10000000,
                        0x10000000,
                        0x10000000,
                        0x10000000,
                        0x10000000
                    },
                    0x010fbd00,
                    0x02f8df00,
                    0x0004f41e,
                    /*param51[8]*/                    
                    {
                        0x08000000,
                        0x21bc5800,
                        0x194c5800,
                        0x113c4800,
                        0x0bbe0d80,
                        0x07ffff80,
                        0x08000000,
                        0x2cfcc000
                    },
                    0x00000006, /*param50*/
                    0x7ff89200, /*param73*/
                    0x0003b700, /*param74*/
                    0x00000010, /*param64*/
                    0x00000000, /*param63*/
                    0x00000000, /*param61*/
                    0x00000014, /*param62*/
        			0x400e1780,
        			0x400e1780,
        			0x10038580,
        			0x3f81f000,
        			0x39999200,
        			0x0147ad80,
        			0x027A0F00,
        			0x0edc5b80,
        			0x7123a480,
        			0x0EDC5B80,
        			0x7123A480,
        			0x12937200,
        			0x2526e400
                },
                {
                    0x00000025, /*param20*/ 
                    0x00117980, /*param21*/ 
                    0x08888880, /*param22*/ 
                    0x00000006, /*param42*/
                    /*param46[7]*/
                    {
                        0x1152f280,
                        0x0d01e400,
                        0x0a5ae580,
                        0x083bc880,
                        0x068dfc80,
                        0x0611e680,
                        0x00000000
                    },
                    /*param47[7]*/
                    {
                        0x660aaa80,
                        0x62c7e800,
                        0x6042c000,
                        0x5dcabc00,
                        0x5b662980,
                        0x5a9b5400,
                        0x00000000
                    },
                    /*param94[20]*/
                    {
                        0x10000000,
                        0x10000000,
                        0x10000000,
                        0x10000000,
                        0x10000000,
                        0x10000000,
                        0x10000000,
                        0x10000000,
                        0x10000000,
                        0x10000000,
                        0x10000000,
                        0x10000000,
                        0x10000000,
                        0x10000000,
                        0x10000000,
                        0x10000000,
                        0x10000000,
                        0x10000000,
                        0x10000000,
                        0x10000000
                    },
                    0x00f9a900,
                    0x02bb0c80,
                    0x02468A80,
                    /*param51[8]*/                                        
                    {
                        0x08000000,
                        0x21bc5800,
                        0x194c5800,
                        0x0e39ea80,
                        0x08000000,
                        0x00000000,
                        0x00000000,
                        0x00000000
                    },
                    0x00000005,
                    0x7ff92c00,
                    0x00036900,
                    0x0000000f,
                    0x00000000,
                    0x00000000,
                    0x00000014,
        			0x3ad9bf00,
        			0x3ad9bf00,
        			0x0eb66f80,
        			0x3a58fb00,
        			0x34eb7e00,
        			0x012d0e00,
        			0x02468A80,
        			0x0da74080,
        			0x7258bf00,
        			0x0DA74080,
        			0x7258BF00,
        			0x11111100,
        			0x22222200
                }
            },
            0x00000001,  /* i32param115 */
            0x0000001e,  /* i32param116 */
            0x7fffff00,  /* i32param117 */
            /* i32table_coef33[11][2] */
            {
                {
                    0x3ebbe400, 0xe13b8200
                },
                {
                    0x3ecd9c00, 0xe12a4c00
                },
                {
                    0x3edf5a00, 0xe1190d00
                },
                {
                    0x3ef11f00, 0xe107c500
                },
                {
                    0x3f09d800, 0xe0ef8300
                },
                {
                    0x3f22a000, 0xe0d72e00
                },
                {
                    0x3f3b7700, 0xe0bec600
                },
                {
                    0x3f43ed00, 0xe0b6ea00
                },
                {
                    0x3f424600, 0xffe0b94d
                },  
                {
                    0x3f0a5d00, 0xe0f1e900
                },  
                {
                    0x3ec22200, 0xe13ac600                
                }               
            },
            /* i32table_coef34[11][3] */
            {
                {
                    0x21551b00, 0xc1441c00, 0x1d6f6400
                },
                {
                    0x2130be00, 0xc1326400, 0x1da4f600
                },                
                {
                    0x210f0600, 0xc120a600, 0x1dd7ed00
                },       
                {
                    0x20efd000, 0xc10ee100, 0x1e086b00
                },
                {
                    0x20a9bb00, 0xc0f62800, 0x1e66c300
                },                
                {
                    0x20737900, 0xc0dd6000, 0x1eb55800
                },
                {
                    0x204a3c00, 0xc0c48900, 0x1ef6fd00
                },
                {
                    0x201b0a00, 0xc0bc1300, 0x1f2e0b00
                },
                {
                    0x1ffad100, 0xc0bdba00, 0x1f4be200
                },
                {
                    0x1fda3e00, 0xc0f5a300, 0x1f33d800
                },
                {
                    0x1fb17e00, 0xc13dde00, 0x1f13bc00                
                }
            },
            /* i32table_coef35[11][2] */
            {
                {
                    0x00000000, 0xed475f00, 
                },
                {
                    0xfef5ee00, 0xec62ea00, 
                },
                {
                    0xfde27000, 0xeb764700, 
                },
                {
                    0xfcc51e00, 0xea810200,                
                },
                {
                    0xfbabf300, 0xea35ac00,                 
                },
                {
                    0xfa902000, 0xe9e98100, 
                },
                {
                    0xf971b500, 0xe99c7d00,
                },
                {
                    0xf5ca0400, 0xe913a100, 
                },                 
                {
                    0xefc74c00, 0xe8f75600,
                },
                {
                    0xeacb9600, 0xeaf94d00, 
                },
                {
                    0xe6a07e00, 0xed40fc00
                }
            },
            /* i32table_coef36[11][3] */
            {
                {
                    0x2e5b3100, 0x00000000, 0x045d7100 
                },
                {
                    0x2ca76f00, 0x010a1200, 0x06f5a600 
                },
                {
                    0x2b0d9700, 0x021d9000, 0x097c2300 
                },
                {
                    0x298cd200, 0x033ae200, 0x0bf22c00 
                },
                {
                    0x273c3100, 0x04540d00, 0x0e8e2100 
                },
                {
                    0x2551c700, 0x056fe000, 0x10c4b800 
                },
                {
                    0x23bd7900, 0x068e4b00, 0x12a60900
                },
                {
                    0x21578800, 0x0a35fc00, 0x1594d600
                },
                {
                    0x1fbfcc00, 0x1038b400, 0x1748dd00 
                },
                {
                    0x1e498d00, 0x15346a00, 0x16bd2500
                },               
                {
                    0x1cb1c400, 0x195f8200, 0x160d3f00
                },
            },
            /* i32table_coef37[2] */
            {
                0x3fb42c00, 
                0xe04b7a00
            },
            /* i32table_coef38[3] */
            {
                0x1fda2c00, 
                0xc04ba800, 
                0x1fda2c00
            }
		};


static const BRAP_DSPCHN_AdPanParams sDefAdPanConfigSettings =	{
            0,
            false,
            1000
		};

static const BRAP_DSPCHN_DTSENCParams sDefDTSENCConfigSettings = {
            true,
            false,
            false,
            BRAP_DSPCHN_DtsEnc_AcMode_eINVALID
        };

static const BRAP_DSPCHN_Ac3ENCParams sDefAc3ENCConfigSettings = {
            true,                                   /* bTranscodeEnable */    
            true,                                   /* bSpdifHeaderEnable */
            BRAP_DSPCHN_AudCodeMode_eMono,          /* eAudCodMode */
            false,                                  /* bLfe */
            BRAP_OutputChannel_eMax,                /* eInputCh */
            BRAP_DSPCHN_AC3ENC_DataRate_Invalid,    /* eDataRate */
            BAVC_AudioSamplingRate_e48k,            /* eSampleRate */
            false,                                  /* bLfeFilter */
            false,                                  /* bCmpChar */
            false,                                  /* bCmpChar2 */
            false                                   /* bSurrDelay */
        };

static const BRAP_DSPCHN_Mp3EncUserParam   sDefMp3EncConfigSettings = {
            BRAP_DSPCHN_Mp3Enc_BitRate_128,                                   /* eBitRate */    
            false,                                                                                  /* bCrcProtect */
            false,                                                                                  /* bPrivateBit */
            BRAP_DSPCHN_Mp3Enc_StereoControl_eNoJointStereo,         /* eStereoControl */
            false,                                                                                  /* bCopyright */
            false,                                                                                  /* bOriginalBitSetting */
            16,                                                                                     /*uiBitsPerSample*/
            BAVC_AudioSamplingRate_e48k,                                          /* eSampleRate */
            BRAP_DSPCHN_MpegEmphasisMode_None,                            /* eEmphasisMode */
            BRAP_DSPCHN_Mp3Enc_InputMode_eStereo,                         /* BRAP_DSPCHN_Mp3Enc_InputMode */
            false,                                                                                  /* bInterleavedPcm */
        };

static const BRAP_DSPCHN_SbcEncUserParam   sDefSbcEncConfigSettings = {
            8,  /*uiNumBlocks*/
            8,  /*uiNumSubBands*/
            true,   /*bJointStereoEnable*/
            0,  /*uiBitAllocation*/
            45  /*uiBitPool*/
        };

static const BRAP_DSPCHN_Brcm3DSurroundParams   sDefBrcm3DSurroundConfigSettings = {
            true,                                   /* b3DSurroundEnable */
            true                                    /* bSoftLimiterEnable */
        };

static const BRAP_DSPCHN_MonoDownmixParams   sDefMonoDownmixConfigSettings = {
            true                                    /* bMonoDownMixEnableFlag */
        };

static const BRAP_DSPCHN_AudysseyVolParams sDefAudysseyVolConfigSettings = {
            true,                                   /*  bAudysseyVolEnable */
            0xFF,                                   /*  ui32ChannelMask */
            false,                                  /*  bAudysseyVolInit */
            0,                                      /*  i32HeadroomOffset  */
            true,                                   /*  bSwAudysseyVol  */
            0,                                      /*  ui32SwDynEQ */
            true,                                   /*  bSwDynSurrGain  */
            true,                                   /*  bSwHiLoCmpress  */
            0,                                      /*  i32dBVolSetting    */
            180,                                    /*  i32GCF */
            {0,0,0,0,0,0,0,0},                      /*  i32chCalbGain[8]   */
            {0,0,0,0,0,0,0,0}                       /*  i32chCalcLevel[8]  */                        
        };

static const BRAP_DSPCHN_AudysseyABXParams sDefAudysseyABXConfigSettings = {
            true,                                  /*  bOperationMode - ABX Enable/Disable */
            1,                                      /*  ui32FilterSet    */
            6,                                      /*  ui32HarmonicGain */
            0                                       /*  ui32DryGain */
        };

static const    BRAP_DSPCHN_DV258UserConfig sDefDV258ConfigSettings ={
    true,  /*bDolbyVolEna*/
    false,  /*bVolModelerEna*/
    false,  /*bHalfmodeEna*/
    true,  /*bEnableMidsideProc*/
    9,  /*uiVolLvlAmount*/
    true,  /*bVolLvlEnable*/
    1360,  /*uiInputReferenceLevel*/
    1360,  /*uiOutputReferenceLevel*/
    0,  /*iPreGain*/
    0,  /*iCalibrationOffset*/
    0,  /*iDigitalVolumeLevel*/
    0,  /*iAnalogVolumeLevel*/
    false,  /*bResetNowFlag*/
    true  /*bLimiterEnable*/
};

static const    BRAP_DSPCHN_DDREUserConfig    sDefDDREConfigSettings ={
    BRAP_OutputMode_eStereo,                    /* eOutputMode */
    false,                                      /* bOutputLfeOn */
    BRAP_DSPCHN_DDRE_CompMode_eLine,            /* eCompMode */    
    100,                                        /* ui32DrcCutFac */
    100,                                        /* ui32DrcBoostFac */
    BRAP_DSPCHN_DDRE_DownmixType_eLtRt,         /* eDownmixType */
    BRAP_DSPCHN_DualMonoMode_eStereo,           /* eDualMonoMode */
    false,                                      /* bCertificationModeEnabled */
    false,                                      /* bExternalPcmModeEnabled */    
    /* sExternalPcmSettings */
    {
        BRAP_DSPCHN_DDRE_Acmode_eTwoCh_2_0_L_R, /* eInputStreamAcmod */
        false,                                  /* bInputHasLfe */
        false,                                  /* bDv258ProcessedInput */
        BRAP_DSPCHN_DDREProfile_eFilmStandardCompression,      /* eProfile */
        BRAP_DSPCHN_Ac3CMix_e3dbBelow,          /* eCentreMixLevel */
        BRAP_DSPCHN_Ac3SMix_e3dbBelow,          /* eSurroundMixLevel */
        BRAP_DSPCHN_Ac3DSur_eNotIndicated,      /* eDolbySurroundMode */
        31                                      /* ui32DialogLevel */
    }
};

static const    BRAP_DSPCHN_SRCUserConfig   sDefSRCConfigSettings ={
    false  /*bDummy*/
};

static const BRAP_DSPCHN_BtscUserParams   sDefBtscConfigSettings = {
            true,  /*bBTSCEnableFlag*/
            true,  /*bUseDeEmphasizedSourceSignals*/                
            true,  /*bMainChannelPreEmphasisOn*/    
            false, /*bEquivalenMode75MicroSec*/            
            true,  /*bClipDiffChannelData*/
            true,  /*bDiffChannelPreEmphasisOn*/                
            true,  /*bUseLowerThresholdSpGain*/    
            0x800000,                 /*uiVolumeLevel*/
            1171,   /*ui32SRDCalibrationFactor*/
            true,   /*bSumChanFreqDevCtrl*/
            true,   /*bDiffChanFreqDevCtrl*/
            true,   /*bOpFreqDevCtrl*/
            false,  /*bFreqGeneration*/
            0,      /*ui32Frequency*/
            0x800000,   /*ui32SumChannelLevelCtrl*/            
            0x800000,   /*ui32DiffChannelLevelCtrl*/
            false       /*bBTSCMonoModeOn*/            
        };

static const BRAP_DSPCHN_SrsCsTdParams sDefSrsCsTdConfigSettings = {
            { /* BRAP_DSPCHN_TopLevelStudioSoundParams */
                false,      /* bIsStudioSound */
                BRAP_DSPCHN_StudioSoundMode_eMode1, /* eStudioSoundMode */    
                true,       /* bEnable */
                100,        /* ui32InputGain */
                100,        /* ui32HeadroomGain */
                BRAP_DSPCHN_StudioSound_InputMode_eLtRt, /* ui32InputMode */
                100,        /* ui32OutputGain */
                100,        /* ui32BypassGain */                
            },
            { /* BRAP_DSPCHN_SrsCircleSurroundParams */
                true,       /* bEnable */
                1000,       /* ui32InputGain */
                1,          /* ui32Mode */
                0,          /* ui32OutputMode */
                1000,       /* ui32OutputGainLR */
                1000,       /* ui32OutputGainLsRs */
                1000,       /* ui32OutputGainC */
                1000        /* ui32OutputGainSub */
            },
            { /* BRAP_DSPCHN_SrsTruDialogParams */
                true,       /* bEnable */
                150,        /* ui32ProcessGain */
                100,        /* ui32InputGain */
                100,        /* ui32OutputGain */
                100,        /* ui32DialogClarityGain */
                100,        /* ui32BypassGain */
            }
        };

static const BRAP_DSPCHN_SrsEqHlParams sDefSrsEqHlConfigSettings = {
            { /* BRAP_DSPCHN_TopLevelStudioSoundParams */
                false,      /* bIsStudioSound */
                BRAP_DSPCHN_StudioSoundMode_eMode1, /* eStudioSoundMode */    
                true,       /* bEnable */
                100,        /* ui32InputGain */
                100,        /* ui32HeadroomGain */
                BRAP_DSPCHN_StudioSound_InputMode_eLtRt, /* ui32InputMode */
                100,        /* ui32OutputGain */
                100,        /* ui32BypassGain */                
            },
            { /* BRAP_DSPCHN_ParametricEqParams */
                {
                    true,   /* bEnable[0] */
                    true    /* bEnable[1] */
                },
                {
                    {
                        true,   /* bBandEnable[0][0] */
                        true,   /* bBandEnable[0][1] */
                        true,   /* bBandEnable[0][2] */
                        true,   /* bBandEnable[0][3] */
                        true,   /* bBandEnable[0][4] */
                        true,   /* bBandEnable[0][5] */
                        true,   /* bBandEnable[0][6] */
                        true    /* bBandEnable[0][7] */
                    },
                    {
                        true,   /* bBandEnable[1][0] */
                        true,   /* bBandEnable[1][1] */
                        true,   /* bBandEnable[1][2] */
                        true,   /* bBandEnable[1][3] */
                        true,   /* bBandEnable[1][4] */
                        true,   /* bBandEnable[1][5] */
                        true,   /* bBandEnable[1][6] */
                        true    /* bBandEnable[1][7] */
                    }
                },
                100,        /* ui32InputGain */
                100,        /* ui32OutputGain */
                100,        /* ui32BypassGain */
                BRAP_DSPCHN_CoefGenMode_eUserCoef, /* eCoefGenMode */
                { /* uPEQFilterSettings */
                    { /* sPEQFilterCoef[3][8] */
                        { /* sPEQFilterCoef[0][] - Sampling Rate - 32 kHz */
                            { /* Band 0 */
                                BRAP_DSPCHN_EQFilterOrder_eOrder2,   /* eFilterOrder */                            
                                0x00000001,   /* ui32Scale */
                                0x40000000,   /* i32FilterCoefficientB0 */
                                0x798f9f00,   /* i32FilterCoefficientA1 */
                                0x86706100,   /* i32FilterCoefficientB1 */
                                0xc40eb580,   /* i32FilterCoefficientA2 */
                                0x3bf14a80    /* i32FilterCoefficientB2 */
                            },
                            { /* Band 1 */
                                BRAP_DSPCHN_EQFilterOrder_eOrder2,   /* eFilterOrder */                            
                                0x00000001,   /* ui32Scale */
                                0x40000000,   /* i32FilterCoefficientB0 */
                                0x798f9f00,   /* i32FilterCoefficientA1 */
                                0x86706100,   /* i32FilterCoefficientB1 */
                                0xc40eb580,   /* i32FilterCoefficientA2 */
                                0x3bf14a80,   /* i32FilterCoefficientB2 */
                            },
                            { /* Band 2 */
                                BRAP_DSPCHN_EQFilterOrder_eOrder2,   /* eFilterOrder */                            
                                0x00000001,   /* ui32Scale */
                                0x40000000,   /* i32FilterCoefficientB0 */
                                0x798f9f00,   /* i32FilterCoefficientA1 */
                                0x86706100,   /* i32FilterCoefficientB1 */
                                0xc40eb580,   /* i32FilterCoefficientA2 */
                                0x3bf14a80,   /* i32FilterCoefficientB2 */
                            },
                            { /* Band 3 */
                                BRAP_DSPCHN_EQFilterOrder_eOrder2,   /* eFilterOrder */                            
                                0x00000001,   /* ui32Scale */
                                0x40000000,   /* i32FilterCoefficientB0 */
                                0x798f9f00,   /* i32FilterCoefficientA1 */
                                0x86706100,   /* i32FilterCoefficientB1 */
                                0xc40eb580,   /* i32FilterCoefficientA2 */
                                0x3bf14a80,   /* i32FilterCoefficientB2 */
                            },
                            { /* Band 4 */
                                BRAP_DSPCHN_EQFilterOrder_eOrder2,   /* eFilterOrder */                            
                                0x00000001,   /* ui32Scale */
                                0x40000000,   /* i32FilterCoefficientB0 */
                                0x798f9f00,   /* i32FilterCoefficientA1 */
                                0x86706100,   /* i32FilterCoefficientB1 */
                                0xc40eb580,   /* i32FilterCoefficientA2 */
                                0x3bf14a80,   /* i32FilterCoefficientB2 */
                            },
                            { /* Band 5 */
                                BRAP_DSPCHN_EQFilterOrder_eOrder2,   /* eFilterOrder */                            
                                0x00000001,   /* ui32Scale */
                                0x40000000,   /* i32FilterCoefficientB0 */
                                0x798f9f00,   /* i32FilterCoefficientA1 */
                                0x86706100,   /* i32FilterCoefficientB1 */
                                0xc40eb580,   /* i32FilterCoefficientA2 */
                                0x3bf14a80    /* i32FilterCoefficientB2 */
                            },
                            { /* Band 6 */
                                BRAP_DSPCHN_EQFilterOrder_eOrder2,   /* eFilterOrder */                            
                                0x00000001,   /* ui32Scale */
                                0x40000000,   /* i32FilterCoefficientB0 */
                                0x798f9f00,   /* i32FilterCoefficientA1 */
                                0x86706100,   /* i32FilterCoefficientB1 */
                                0xc40eb580,   /* i32FilterCoefficientA2 */
                                0x3bf14a80,   /* i32FilterCoefficientB2 */
                            },
                            { /* Band 7 */
                                BRAP_DSPCHN_EQFilterOrder_eOrder2,   /* eFilterOrder */                            
                                0x00000001,   /* ui32Scale */
                                0x40000000,   /* i32FilterCoefficientB0 */
                                0x798f9f00,   /* i32FilterCoefficientA1 */
                                0x86706100,   /* i32FilterCoefficientB1 */
                                0xc40eb580,   /* i32FilterCoefficientA2 */
                                0x3bf14a80,   /* i32FilterCoefficientB2 */
                            }                            
                        },
                        { /* sPEQFilterCoef[1][] - Sampling Rate - 44.1 kHz */
                            { /* Band 0 */
                                BRAP_DSPCHN_EQFilterOrder_eOrder2,   /* eFilterOrder */                            
                                0x00000001,   /* ui32Scale */
                                0x40000000,   /* i32FilterCoefficientB0 */
                                0x7bc37b00,   /* i32FilterCoefficientA1 */
                                0x843c8500,   /* i32FilterCoefficientB1 */
                                0xc2f83300,   /* i32FilterCoefficientA2 */
                                0x3d07cd00    /* i32FilterCoefficientB2 */
                            },                    
                            { /* Band 1 */
                                BRAP_DSPCHN_EQFilterOrder_eOrder2,   /* eFilterOrder */                            
                                0x00000001,   /* ui32Scale */
                                0x40000000,   /* i32FilterCoefficientB0 */
                                0x7bc37b00,   /* i32FilterCoefficientA1 */
                                0x843c8500,   /* i32FilterCoefficientB1 */
                                0xc2f83300,   /* i32FilterCoefficientA2 */
                                0x3d07cd00    /* i32FilterCoefficientB2 */
                            },                    
                            { /* Band 2 */
                                BRAP_DSPCHN_EQFilterOrder_eOrder2,   /* eFilterOrder */                            
                                0x00000001,   /* ui32Scale */
                                0x40000000,   /* i32FilterCoefficientB0 */
                                0x7bc37b00,   /* i32FilterCoefficientA1 */
                                0x843c8500,   /* i32FilterCoefficientB1 */
                                0xc2f83300,   /* i32FilterCoefficientA2 */
                                0x3d07cd00    /* i32FilterCoefficientB2 */
                            },                    
                            { /* Band 3 */
                                BRAP_DSPCHN_EQFilterOrder_eOrder2,   /* eFilterOrder */                            
                                0x00000001,   /* ui32Scale */
                                0x40000000,   /* i32FilterCoefficientB0 */
                                0x7bc37b00,   /* i32FilterCoefficientA1 */
                                0x843c8500,   /* i32FilterCoefficientB1 */
                                0xc2f83300,   /* i32FilterCoefficientA2 */
                                0x3d07cd00    /* i32FilterCoefficientB2 */
                            },                    
                            { /* Band 4 */
                                BRAP_DSPCHN_EQFilterOrder_eOrder2,   /* eFilterOrder */                            
                                0x00000001,   /* ui32Scale */
                                0x40000000,   /* i32FilterCoefficientB0 */
                                0x7bc37b00,   /* i32FilterCoefficientA1 */
                                0x843c8500,   /* i32FilterCoefficientB1 */
                                0xc2f83300,   /* i32FilterCoefficientA2 */
                                0x3d07cd00    /* i32FilterCoefficientB2 */
                            },                    
                            { /* Band 5 */
                                BRAP_DSPCHN_EQFilterOrder_eOrder2,   /* eFilterOrder */                            
                                0x00000001,   /* ui32Scale */
                                0x40000000,   /* i32FilterCoefficientB0 */
                                0x7bc37b00,   /* i32FilterCoefficientA1 */
                                0x843c8500,   /* i32FilterCoefficientB1 */
                                0xc2f83300,   /* i32FilterCoefficientA2 */
                                0x3d07cd00    /* i32FilterCoefficientB2 */
                            },                    
                            { /* Band 6 */
                                BRAP_DSPCHN_EQFilterOrder_eOrder2,   /* eFilterOrder */                            
                                0x00000001,   /* ui32Scale */
                                0x40000000,   /* i32FilterCoefficientB0 */
                                0x7bc37b00,   /* i32FilterCoefficientA1 */
                                0x843c8500,   /* i32FilterCoefficientB1 */
                                0xc2f83300,   /* i32FilterCoefficientA2 */
                                0x3d07cd00    /* i32FilterCoefficientB2 */
                            },                                            
                            { /* Band 7 */
                                BRAP_DSPCHN_EQFilterOrder_eOrder2,   /* eFilterOrder */                            
                                0x00000001,   /* ui32Scale */
                                0x40000000,   /* i32FilterCoefficientB0 */
                                0x7bc37b00,   /* i32FilterCoefficientA1 */
                                0x843c8500,   /* i32FilterCoefficientB1 */
                                0xc2f83300,   /* i32FilterCoefficientA2 */
                                0x3d07cd00    /* i32FilterCoefficientB2 */
                            }                          
                        },
                        { /* sPEQFilterCoef[2][] - Sampling Rate - 48 kHz */
                            { /* Band 0 */
                                BRAP_DSPCHN_EQFilterOrder_eOrder2,   /* eFilterOrder */                            
                                0x00000001,   /* ui32Scale */
                                0x40000000,   /* i32FilterCoefficientB0 */
                                0x7c31eb80,   /* i32FilterCoefficientA1 */
                                0x83ce1480,   /* i32FilterCoefficientB1 */
                                0xc2bbbb80,   /* i32FilterCoefficientA2 */
                                0x3d444480    /* i32FilterCoefficientB2 */
                            },   
                            { /* Band 1 */
                                BRAP_DSPCHN_EQFilterOrder_eOrder2,   /* eFilterOrder */                            
                                0x00000001,   /* ui32Scale */
                                0x40000000,   /* i32FilterCoefficientB0 */
                                0x7c31eb80,   /* i32FilterCoefficientA1 */
                                0x83ce1480,   /* i32FilterCoefficientB1 */
                                0xc2bbbb80,   /* i32FilterCoefficientA2 */
                                0x3d444480    /* i32FilterCoefficientB2 */
                            },     
                            { /* Band 2 */
                                BRAP_DSPCHN_EQFilterOrder_eOrder2,   /* eFilterOrder */                            
                                0x00000001,   /* ui32Scale */
                                0x40000000,   /* i32FilterCoefficientB0 */
                                0x7c31eb80,   /* i32FilterCoefficientA1 */
                                0x83ce1480,   /* i32FilterCoefficientB1 */
                                0xc2bbbb80,   /* i32FilterCoefficientA2 */
                                0x3d444480    /* i32FilterCoefficientB2 */
                            }, 
                            { /* Band 3 */
                                BRAP_DSPCHN_EQFilterOrder_eOrder2,   /* eFilterOrder */                            
                                0x00000001,   /* ui32Scale */
                                0x40000000,   /* i32FilterCoefficientB0 */
                                0x7c31eb80,   /* i32FilterCoefficientA1 */
                                0x83ce1480,   /* i32FilterCoefficientB1 */
                                0xc2bbbb80,   /* i32FilterCoefficientA2 */
                                0x3d444480    /* i32FilterCoefficientB2 */
                            },
                            { /* Band 4 */
                                BRAP_DSPCHN_EQFilterOrder_eOrder2,   /* eFilterOrder */                            
                                0x00000001,   /* ui32Scale */
                                0x40000000,   /* i32FilterCoefficientB0 */
                                0x7c31eb80,   /* i32FilterCoefficientA1 */
                                0x83ce1480,   /* i32FilterCoefficientB1 */
                                0xc2bbbb80,   /* i32FilterCoefficientA2 */
                                0x3d444480    /* i32FilterCoefficientB2 */
                            },
                            { /* Band 5 */
                                BRAP_DSPCHN_EQFilterOrder_eOrder2,   /* eFilterOrder */                            
                                0x00000001,   /* ui32Scale */
                                0x40000000,   /* i32FilterCoefficientB0 */
                                0x7c31eb80,   /* i32FilterCoefficientA1 */
                                0x83ce1480,   /* i32FilterCoefficientB1 */
                                0xc2bbbb80,   /* i32FilterCoefficientA2 */
                                0x3d444480    /* i32FilterCoefficientB2 */
                            },
                            { /* Band 6 */
                                BRAP_DSPCHN_EQFilterOrder_eOrder2,   /* eFilterOrder */                            
                                0x00000001,   /* ui32Scale */
                                0x40000000,   /* i32FilterCoefficientB0 */
                                0x7c31eb80,   /* i32FilterCoefficientA1 */
                                0x83ce1480,   /* i32FilterCoefficientB1 */
                                0xc2bbbb80,   /* i32FilterCoefficientA2 */
                                0x3d444480    /* i32FilterCoefficientB2 */
                            },
                            { /* Band 7 */
                                BRAP_DSPCHN_EQFilterOrder_eOrder2,   /* eFilterOrder */                            
                                0x00000001,   /* ui32Scale */
                                0x40000000,   /* i32FilterCoefficientB0 */
                                0x7c31eb80,   /* i32FilterCoefficientA1 */
                                0x83ce1480,   /* i32FilterCoefficientB1 */
                                0xc2bbbb80,   /* i32FilterCoefficientA2 */
                                0x3d444480    /* i32FilterCoefficientB2 */
                            }    
                        }
                    }
                    /*Default PEQ filter specification - 
                        i32BandGain = 0;
                    	ui32CutoffFrequency = 1000 Hz;
                    	i32QFactor = 200; */                
                }
            },
            { /* BRAP_DSPCHN_HighPassFilterParams */
                true,       /* bEnable */
                BRAP_DSPCHN_CoefGenMode_eUserCoef, /* eCoefGenMode */    
                { /* uHPFSettings */
                    { /* sHPFilterCoef[3] */
                        { /* sHPFilterCoef[0] - Sampling frequency - 32 kHz */
                            BRAP_DSPCHN_EQFilterOrder_eOrder4,  /* eFilterOrder */
                            {
                                { /* sHPFilterCoefTdf2[0] */
                                    0x00000001,     /* ui32Scale */ 
                                    0x0fc81f80,     /* i32FilterCoefficientB0 */
                                    0xe06fc100,     /* i32FilterCoefficientB1 */
                                    0x0fc81f80,     /* i32FilterCoefficientB2 */
                                    0x7e36e680,     /* i32FilterCoefficientA1 */
                                    0xc1b4eec0,     /* i32FilterCoefficientA2 */
                                },                        
                                { /* sHPFilterCoefTdf2[1] */
                                    0x00000001,     /* ui32Scale */ 
                                    0x3df563c0,     /* i32FilterCoefficientB0 */
                                    0x84153880,     /* i32FilterCoefficientB1 */
                                    0x3df563c0,     /* i32FilterCoefficientB2 */
                                    0x7be0e200,     /* i32FilterCoefficientA1 */
                                    0xc40b5300      /* i32FilterCoefficientA2 */
                                },
                                { /* sHPFilterCoefTdf2[2] */
                                    0x00000001,     /* ui32Scale */ 
                                    0x0,            /* i32FilterCoefficientB0 */
                                    0x0,            /* i32FilterCoefficientB1 */
                                    0x0,            /* i32FilterCoefficientB2 */
                                    0x0,            /* i32FilterCoefficientA1 */
                                    0x0             /* i32FilterCoefficientA2 */
                                }                                
                            }
                        },
                        { /* sHPFilterCoef[1] - Sampling frequency - 44.1 kHz */
                            BRAP_DSPCHN_EQFilterOrder_eOrder4,  /* eFilterOrder */
                            {
                                { /* sHPFilterCoefTdf2[0] */
                                    0x00000001,     /* ui32Scale */ 
                                    0x0fd78db0,     /* i32FilterCoefficientB0 */
                                    0xe050e4a0,     /* i32FilterCoefficientB1 */
                                    0x0fd78db0,     /* i32FilterCoefficientB2 */
                                    0x7eb71980,     /* i32FilterCoefficientA1 */
                                    0xc13e3e40      /* i32FilterCoefficientA2 */
                                },                        
                                { /* sHPFilterCoefTdf2[1] */
                                    0x00000001,     /* ui32Scale */ 
                                    0x3e826c40,     /* i32FilterCoefficientB0 */
                                    0x82fb2780,     /* i32FilterCoefficientB1 */
                                    0x3e826c40,     /* i32FilterCoefficientB2 */
                                    0x7cff9680,     /* i32FilterCoefficientA1 */
                                    0xc2f5e600      /* i32FilterCoefficientA2 */
                                },
                                { /* sHPFilterCoefTdf2[2] */
                                    0x00000001,     /* ui32Scale */ 
                                    0x0,            /* i32FilterCoefficientB0 */
                                    0x0,            /* i32FilterCoefficientB1 */
                                    0x0,            /* i32FilterCoefficientB2 */
                                    0x0,            /* i32FilterCoefficientA1 */
                                    0x0             /* i32FilterCoefficientA2 */
                                }                                    
                            }
                        },
                        { /* sHPFilterCoef[2] - Sampling frequency - 48 kHz */
                            BRAP_DSPCHN_EQFilterOrder_eOrder4,  /* eFilterOrder */
                            {
                                { /* sHPFilterCoefTdf2[0] */
                                    0x00000001,     /* ui32Scale */ 
                                    0x0fdadc10,     /* i32FilterCoefficientB0 */
                                    0xe04a47e0,     /* i32FilterCoefficientB1 */
                                    0x0fdadc10,     /* i32FilterCoefficientB2 */
                                    0x7ed26000,     /* i32FilterCoefficientA1 */
                                    0xc1249f40      /* i32FilterCoefficientA2 */
                                },          
                                { /* sHPFilterCoefTdf2[1] */
                                    0x00000001,     /* ui32Scale */ 
                                    0x3ea0f4c0,     /* i32FilterCoefficientB0 */
                                    0x82be1680,     /* i32FilterCoefficientB1 */
                                    0x3ea0f4c0,     /* i32FilterCoefficientB2 */
                                    0x7d3d7780,     /* i32FilterCoefficientA1 */
                                    0xc2b9a440      /* i32FilterCoefficientA2 */
                                },                        
                                { /* sHPFilterCoefTdf2[2] */
                                    0x00000001,     /* ui32Scale */ 
                                    0x0,            /* i32FilterCoefficientB0 */
                                    0x0,            /* i32FilterCoefficientB1 */
                                    0x0,            /* i32FilterCoefficientB2 */
                                    0x0,            /* i32FilterCoefficientA1 */
                                    0x0             /* i32FilterCoefficientA2 */
                                }                        
                            }
                        }                    
                    }              
                    /*Default HighPass filter specification - 
                    	ui32CutoffFrequency=180 Hz;
                    	eFilterOrder=4*/                
                },
            },
            { /* BRAP_DSPCHN_GraphicEqParams */
                {
                    false,  /* bEnable[0] */
                    false   /* bEnable[1] */
                },
                0,          /* ui32FilterMode */            
                {
                    {
                        1000,   /* ui32BandGain[0][0] */
                        1000,   /* ui32BandGain[0][1] */
                        1000,   /* ui32BandGain[0][2] */
                        1000,   /* ui32BandGain[0][3] */
                        1000,   /* ui32BandGain[0][4] */
                        1000,   /* ui32BandGain[0][5] */
                        1000,   /* ui32BandGain[0][6] */
                        1000,   /* ui32BandGain[0][7] */
                        1000,   /* ui32BandGain[0][8] */
                        1000    /* ui32BandGain[0][9] */
                    },
                    {
                        1000,   /* ui32BandGain[1][0] */
                        1000,   /* ui32BandGain[1][1] */
                        1000,   /* ui32BandGain[1][2] */
                        1000,   /* ui32BandGain[1][3] */
                        1000,   /* ui32BandGain[1][4] */
                        1000,   /* ui32BandGain[1][5] */
                        1000,   /* ui32BandGain[1][6] */
                        1000,   /* ui32BandGain[1][7] */
                        1000,   /* ui32BandGain[1][8] */
                        1000    /* ui32BandGain[1][9] */
                    }
                },
                316,        /* ui32InputGain */
                100,        /* ui32OutputGain */
                100         /* ui32BypassGain */
            },
            { /* BRAP_DSPCHN_HardLimiterParams */
                false,      /* bEnable */
                256,        /* ui32blockSize */
                100,        /* ui32InputGain */
                100,        /* ui32OutputGain*/
                100,        /* ui32BypassGain */
                2820,       /* ui32LimiterBoost */
                100000,     /* ui32HardLimit */
                22          /* ui32DelayLength */                
            }
        };


const BRAP_P_DecoderSettings  BRAP_sDefaultDecSettings = {
	{/*sUserConfigStruct*/
     		{/*sMpegConfigParam*/
			2,										/*ui32OutMode*/
			0x7FFFFFFF,										/*ui32LeftChannelGain*/
			0x7FFFFFFF,										/*ui32RightChannelGain*/
			BRAP_DSPCHN_DualMonoMode_eStereo,		/*ui32DualMonoMode*/
			{0, 1,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},					/* ui32OutputChannelMatrix */
		},
		{/*sAc3ConfigParam*/
			BRAP_DSPCHN_Ac3CompMode_eRf,				/*i32CompMode*/
			0x7FFFFFFF,				/*i32PcmScale*/
			0x7FFFFFFF,				/*i3DynScaleHigh*/
			0x7FFFFFFF,				/*i32DynScaleLow*/
			1,				        /*i32NumOutPorts*/
			1,                      /*i32StreamDialNormEnable*/
			0,                      /*i32UserDialNormVal*/
			{{/*sUserOutputCfg[0]*/
				false,			/*i32OutLfe*/
				2,			/*i32OutMode*/
				BRAP_DSPCHN_Ac3StereoMode_eAuto,			/*i32StereoMode*/
				0,			/*i32DualMode*/
				BRAP_DSPCHN_Ac3KaraokeVocalSelect_eBothVocal,			/*i32Kmode*/
				false,			/*i32ExtDnmixEnabled*/
			     {{ 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[0] */	
		            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[1] */	
		            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[2] */	
		            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[3] */	
		            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[4] */	
		            { 0, 0, 0, 0, 0, 0}},   			/* i32ExtDnmixTab[5] */		
				false,			/*i32ExtKaraokeEnabled*/
				0,			/*i32Extv1Level*/
				0,			/*i32Extv1Pan*/
				0,			/*i32Extv2Level*/
				0,			/*i32Extv2Pan*/
				0,			/*i32ExtGmLevel*/
				0,			/*i32ExtGmPan*/
            		 {0, 1,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF}	/* ui32OutputChannelMatrix */				
			},
			{/*sUserOutputCfg[1]*/
				false,			/*i32OutLfe*/
				2,			/*i32OutMode*/
				BRAP_DSPCHN_Ac3StereoMode_eAuto,			/*i32StereoMode*/
				0,			/*i32DualMode*/
				BRAP_DSPCHN_Ac3KaraokeVocalSelect_eBothVocal,			/*i32Kmode*/
				false,			/*i32ExtDnmixEnabled*/
			     {{ 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[0] */	
		            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[1] */	
		            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[2] */	
		            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[3] */	
		            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[4] */	
		            { 0, 0, 0, 0, 0, 0}},   			/* i32ExtDnmixTab[5] */		
				false,			/*i32ExtKaraokeEnabled*/
				0,			/*i32Extv1Level*/
				0,			/*i32Extv1Pan*/
				0,			/*i32Extv2Level*/
				0,			/*i32Extv2Pan*/
				0,			/*i32ExtGmLevel*/
				0,			/*i32ExtGmPan*/
                		 {0, 1,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF}	/* ui32OutputChannelMatrix */				
			}}
		},
		{/*sAc3PlusConfigParam*/
			BRAP_DSPCHN_Ac3CompMode_eRf,				/*i32CompMode*/
			0x7FFFFFFF,				/*i32PcmScale*/
			0x7FFFFFFF,				/*i3DynScaleHigh*/
			0x7FFFFFFF,				/*i32DynScaleLow*/
			1,				        /*i32NumOutPorts*/
			1,                      /*i32StreamDialNormEnable*/
			0,                      /*i32UserDialNormVal*/			
			{{/*sUserOutputCfg[0]*/
				false,			/*i32OutLfe*/
				2,			/*i32OutMode*/
				BRAP_DSPCHN_Ac3StereoMode_eAuto,			/*i32StereoMode*/
				0,			/*i32DualMode*/
				BRAP_DSPCHN_Ac3KaraokeVocalSelect_eBothVocal,			/*i32Kmode*/
				false,			/*i32ExtDnmixEnabled*/
			     {{ 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[0] */	
		            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[1] */	
		            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[2] */	
		            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[3] */	
		            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[4] */	
		            { 0, 0, 0, 0, 0, 0}},   			/* i32ExtDnmixTab[5] */		
				false,			/*i32ExtKaraokeEnabled*/
				0,			/*i32Extv1Level*/
				0,			/*i32Extv1Pan*/
				0,			/*i32Extv2Level*/
				0,			/*i32Extv2Pan*/
				0,			/*i32ExtGmLevel*/
				0,			/*i32ExtGmPan*/
            		 {0, 1,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF}	/* ui32OutputChannelMatrix */				
			},
			{/*sUserOutputCfg[1]*/
				false,			/*i32OutLfe*/
				2,			/*i32OutMode*/
				BRAP_DSPCHN_Ac3StereoMode_eAuto,			/*i32StereoMode*/
				0,			/*i32DualMode*/
				BRAP_DSPCHN_Ac3KaraokeVocalSelect_eBothVocal,			/*i32Kmode*/
				false,			/*i32ExtDnmixEnabled*/
			     {{ 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[0] */	
		            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[1] */	
		            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[2] */	
		            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[3] */	
		            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[4] */	
		            { 0, 0, 0, 0, 0, 0}},   			/* i32ExtDnmixTab[5] */		
				false,			/*i32ExtKaraokeEnabled*/
				0,			/*i32Extv1Level*/
				0,			/*i32Extv1Pan*/
				0,			/*i32Extv2Level*/
				0,			/*i32Extv2Pan*/
				0,			/*i32ExtGmLevel*/
				0,			/*i32ExtGmPan*/
            		 {0, 1,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF}	/* ui32OutputChannelMatrix */				
			}}
		},
		{/*sAacSbrConfigParam*/
			1,				/*i32NumOutPorts*/
			BRAP_DSPCHN_AACSbr_DownmixType_eBRCM,	    /* DownmixType */
			0x0,										/* i32AribMatrixMixdownIndex = 0 */
			0x40000000,									/* ui16DrcGainControlCompress = 1 */
			0x40000000,									/* ui16DrcGainControlBoost = 1 */
			127,
			{{/*sUserOutputCfg[0]*/
				false,		/*i32OutLfe*/
				2,			/*i32OutMode*/
				0,			/*i32DualMode*/
				false,			/*i32ExtDnmixEnabled*/
			     {{ 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[0] */	
		            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[1] */	
		            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[2] */	
		            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[3] */	
		            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[4] */	
		            { 0, 0, 0, 0, 0, 0}},   			/* i32ExtDnmixTab[5] */		
       		 {0, 1, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}		/* ui32OutputChannelMatrix */		            
			},
			{/*sUserOutputCfg[1]*/
				false,		/*i32OutLfe*/
				2,			/*i32OutMode*/
				0,			/*i32DualMode*/
				false,			/*i32ExtDnmixEnabled*/
			     {{ 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[0] */	
		            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[1] */	
		            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[2] */	
		            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[3] */	
		            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[4] */	
		            { 0, 0, 0, 0, 0, 0}},   			/* i32ExtDnmixTab[5] */		
       		 {0, 1, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}		/* ui32OutputChannelMatrix */		            
			}},
			0,/*i32PcmBoost6dB*/
			false,/*i32PcmBoostMinus4p75dB*/
			0,   /*ui32FreqExtensionEnable*/
			0,   /*ui32SbrUserFlag*/
			50  /*ui32DownmixCoefScaleIndex*/
		},
            {/*sWmaStdConfigParam*/
                2,              /*Outmode*/
                0,
                0,
     		 {0, 1,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF}	/* ui32OutputChannelMatrix */
            },
            {/*sWmaProConfigParam*/
                    2,  /*ui32NumOutports*/
                    {{/* sOutputCfg[0]*/
                        0,                                                       /*ui32DRCEnable*/
                        BRAP_DSPCHN_WmaPro_DrcSetting_eHigh,    /*eDRCSetting*/                            
                        0X7FFFFFFF,                                             /*i32RmsAmplitudeRef*/
                        0X7FFFFFFF,                                             /*i32PeakAmplitudeRef*/
                        0X7FFFFFFF,                                             /*i32DesiredRms*/
                        0X7FFFFFFF,                                             /*i32DesiredPeak*/
                        2,                                                      /*ui32OutMode*/
                        false,                                                 /*ui32OutLfe*/
                        BRAP_DSPCHN_Ac3StereoMode_eAuto,/*ui32StereoMode*/
       		 {0, 1,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},	/* ui32OutputChannelMatrix */                         
                    },
                    {/* sOutputCfg[1]*/
                        0,                                                       /*ui32DRCEnable*/
                        BRAP_DSPCHN_WmaPro_DrcSetting_eHigh,    /*eDRCSetting*/                            
                        0X7FFFFFFF,                                             /*i32RmsAmplitudeRef*/
                        0X7FFFFFFF,                                             /*i32PeakAmplitudeRef*/
                        0X7FFFFFFF,                                             /*i32DesiredRms*/
                        0X7FFFFFFF,                                             /*i32DesiredPeak*/
                        2,                                                      /*ui32OutMode*/
                        false,                                                 /*ui32OutLfe*/
                        BRAP_DSPCHN_Ac3StereoMode_eAuto,/*ui32StereoMode*/
       		 {0, 1,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},	/* ui32OutputChannelMatrix */                         
                    }},                    
       		 0
            },
            {/*sLpcmDvdConfigParam*/
                    1,  /*ui32NumOutports*/
                    {{/* sOutputCfg[0]*/
                        2,/*ui32OutMode*/
                        false,/*ui32LfeOnFlag*/
                        0,/*ui32DualMonoMode*/
       		 {0, 1,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},	/* ui32OutputChannelMatrix */                         
                    },
                    {/* sOutputCfg[0]*/
                        2,/*ui32OutMode*/
                        false,/*ui32LfeOnFlag*/
                        0,/*ui32DualMonoMode*/
       		 {0, 1,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},	/* ui32OutputChannelMatrix */                         
                    }},
                    false,
                    {{{ 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[0] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[1] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[2] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[3] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[4] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0}},
        	        {{ 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[0] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[1] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[2] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[3] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[4] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0}},
                     {{ 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[0] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[1] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[2] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[3] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[4] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0}},
                     {{ 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[0] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[1] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[2] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[3] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[4] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0}},
        	        {{ 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[0] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[1] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[2] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[3] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[4] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0}},
                     {{ 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[0] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[1] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[2] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[3] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[4] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0}},        	            
                    {{ 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[0] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[1] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[2] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[3] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[4] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0}},
        	        {{ 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[0] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[1] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[2] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[3] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[4] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0}},
                     {{ 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[0] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[1] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[2] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[3] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[4] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0}},
                     {{ 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[0] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[1] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[2] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[3] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[4] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0}},
        	        {{ 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[0] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[1] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[2] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[3] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[4] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0}},
                     {{ 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[0] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[1] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[2] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[3] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[4] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0}},    
                     {{ 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[0] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[1] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[2] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[3] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[4] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0}},
                     {{ 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[0] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[1] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[2] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[3] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[4] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0}},
        	        {{ 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[0] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[1] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[2] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[3] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[4] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0}},
                     {{ 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[0] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[1] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[2] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[3] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0},  				/* i32UserDownmixTables[4] */	
        	            { 0, 0, 0, 0, 0, 0, 0, 0}}}           	            
                            	            
            },
            {/*sDtsBroadcastConfigParam*/
                true,       /*ui32DecodeCoreOnly*/
                false,      /*ui32DecodeDtsOnly*/
                true,       /*ui32DecodeXLL*/
                true,       /*ui32DecodeX96*/
                true,       /*ui32DecodeXCH*/
                true,       /*ui32DecodeXXCH*/
                true,       /*ui32DecodeXBR*/
                false,      /*ui32EnableSpkrRemapping*/
                2123,       /*ui32SpkrOut*/
                false,      /*ui32MixLFE2Primary*/
                0,          /*ui32ChReplacementSet*/
                2,          /*i32NumOutPorts*/
                false,      /*ui32EnableMetadataProcessing*/                
                {{/*sUserOutputCfg[0]*/
                    false,              /*i32UserDRCFlag*/
                    0x7fffffff,    /*i32DynScaleHigh*/
                    0x7fffffff,     /*i32DynScaleLow*/
                    2,                      /*ui32OutMode*/
                    false,                  /*ui32OutLfe*/
                    2,                      /*ui32DualMode*/
                    0,                      /*ui32StereoMode*/
                    48000,                      /*ui32AppSampleRate*/
                    {0, 1,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},  /*ui32OutputChannelMatrix*/
                    0,              /*ui32ExtdnmixEnabled*/
                    {{ 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[0] */	
	            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[1] */	
	            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[2] */	
	            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[3] */	
	            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[4] */	
	            { 0, 0, 0, 0, 0, 0}}   			/* i32ExtDnmixTab[5] */	                                        
                },
                {/*sUserOutputCfg[1]*/
                    false,              /*i32UserDRCFlag*/
                    0x7fffffff,    /*i32DynScaleHigh*/
                    0x7fffffff,     /*i32DynScaleLow*/
                    2,                      /*ui32OutMode*/
                    false,                  /*ui32OutLfe*/
                    2,                      /*ui32DualMode*/
                    0,                      /*ui32StereoMode*/
                    48000,                      /*ui32AppSampleRate*/
                    {0, 1,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},  /*ui32OutputChannelMatrix*/
                    0,              /*ui32ExtdnmixEnabled*/
                    {{ 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[0] */	
	            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[1] */	
	            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[2] */	
	            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[3] */	
	            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[4] */	
	            { 0, 0, 0, 0, 0, 0}}   			/* i32ExtDnmixTab[5] */	                                        
                } }               
            },
            {/*sDtsHdConfigParam*/
                true,       /*ui32DecodeCoreOnly*/
                false,      /*ui32DecodeDtsOnly*/
                true,       /*ui32DecodeXLL*/
                true,       /*ui32DecodeX96*/
                true,       /*ui32DecodeXCH*/
                true,       /*ui32DecodeXXCH*/
                true,       /*ui32DecodeXBR*/
                false,      /*ui32EnableSpkrRemapping*/
                2123,       /*ui32SpkrOut*/
                false,      /*ui32MixLFE2Primary*/
                0,          /*ui32ChReplacementSet*/
                2,          /*i32NumOutPorts*/
                false,      /*ui32EnableMetadataProcessing*/
                {{/*sUserOutputCfg[0]*/
                    false,              /*i32UserDRCFlag*/
                    0x7fffffff,    /*i32DynScaleHigh*/
                    0x7fffffff,     /*i32DynScaleLow*/
                    2,                      /*ui32OutMode*/
                    false,                  /*ui32OutLfe*/
                    2,                      /*ui32DualMode*/
                    0,                      /*ui32StereoMode*/
                    48000,                      /*ui32AppSampleRate*/
                    {0, 1,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},  /*ui32OutputChannelMatrix*/
                    0,              /*ui32ExtdnmixEnabled*/
                    {{ 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[0] */	
	            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[1] */	
	            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[2] */	
	            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[3] */	
	            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[4] */	
	            { 0, 0, 0, 0, 0, 0}}   			/* i32ExtDnmixTab[5] */	                                        
                },
                {/*sUserOutputCfg[1]*/
                    false,              /*i32UserDRCFlag*/
                    0x7fffffff,    /*i32DynScaleHigh*/
                    0x7fffffff,     /*i32DynScaleLow*/
                    2,                      /*ui32OutMode*/
                    false,                  /*ui32OutLfe*/
                    2,                      /*ui32DualMode*/
                    0,                      /*ui32StereoMode*/
                    48000,                      /*ui32AppSampleRate*/
                    {0, 1,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},  /*ui32OutputChannelMatrix*/
                    0,              /*ui32ExtdnmixEnabled*/
                    {{ 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[0] */	
	            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[1] */	
	            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[2] */	
	            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[3] */	
	            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[4] */	
	            { 0, 0, 0, 0, 0, 0}}   			/* i32ExtDnmixTab[5] */	                                        
                } }               
            },            
            {/*sPcmWavConfigParam*/
                1,
                2,
                {0, 1,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},  /*ui32OutputChannelMatrix*/                
                BRAP_AF_P_DolbyMsUsageMode_eINVALID                
            },
            {/*sAmrConfigParam*/
                1,      /*i32NumOutPorts*/
                {{/*sUserOutputCfg[0]*/
                    1,  /* ui32OutMode */
                    0,  /* ui32ScaleOp */
                    0,  /* ui32ScaleIdx */
                    {4, 4,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},  /*ui32OutputChannelMatrix*/
                },
                {/*sUserOutputCfg[1]*/
                    1,  /* ui32OutMode */
                    0,  /* ui32ScaleOp */
                    0,  /* ui32ScaleIdx */
                    {0xFFFFFFFF, 0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},  /*ui32OutputChannelMatrix*/
                }}
            },   
            {/*DRA*/
                2,/*ui32NumOutPorts*/
                {{
                    2,/*ui32OutMode*/
                    false,/*ui32OutLfe*/
                    BRAP_DSPCHN_DraStereoMode_eLoRo,/*ui32StereoMode*/
                    {0, 1,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},  /*ui32OutputChannelMatrix*/                    
                },
                {
                    2,/*ui32OutMode*/
                    false,/*ui32OutLfe*/
                    BRAP_DSPCHN_DraStereoMode_eLoRo,/*ui32StereoMode*/
                    {0, 1,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},  /*ui32OutputChannelMatrix*/                    
                }}
            },
            {/*Real Audio LBR*/
                2,/*ui32NumOutPorts*/
                {{
                    2,/*ui32OutMode*/
                    {0, 1,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},  /*ui32OutputChannelMatrix*/                    
                },
                {
                    2,/*ui32OutMode*/
                    {0, 1,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},  /*ui32OutputChannelMatrix*/                    
                }}
            },       
            {/*Dolby Pulse*/
                BRAP_AF_P_DolbyMsUsageMode_eSingleDecodeMode,/**eDolbyPulseUsageMode*/
                BRAP_AF_P_DolbyMsDecoderType_ePrimary,  /* eDecoderType */
                1, /* ui32NumOutPorts */
                124,/*ui32RefDialnormLevel*/
                108,/*ui32DefDialnormLevel*/
                {
                    {
                        2,  /* ui32OutMode */
                        0,  /* ui32OutLfe */
                        0,  /* ui32DualMode */
                        0,  /* ui32LoRoDownmix */
                        0,  /* ui32RfMode */
                        100,/* ui32DrcCut */
                        100,/* ui32DrcBoost */
                        0,  /*ui32ApplyGain*/
                        0x00B504F3,/*i32GainFactor*/
                        {0, 1,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},  /*ui32OutputChannelMatrix*/                    
                    },
                    {
                        2,  /* ui32OutMode */
                        0,  /* ui32OutLfe */
                        0,  /* ui32DualMode */
                        0,  /* ui32LoRoDownmix */
                        0,  /* ui32RfMode */
                        100,/* ui32DrcCut */
                        100,/* ui32DrcBoost */
                        0,  /*ui32ApplyGain*/
                        0x00B504F3,/*i32GainFactor*/               
                        {0, 1,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},  /*ui32OutputChannelMatrix*/                    
                    },
                }
            },   
            {/*sMs10DDPConfigParam*/
			BRAP_DSPCHN_Ac3CompMode_eLine,				/*i32CompMode*/
			0x7FFFFFFF,				/*i32PcmScale*/
			0x7FFFFFFF,				/*i3DynScaleHigh*/
			0x7FFFFFFF,				/*i32DynScaleLow*/
			2,				        /*i32NumOutPorts*/
			1,                      /*i32StreamDialNormEnable*/
			0,                      /*i32UserDialNormVal*/
			{{/*sUserOutputCfg[0]*/
				false,			/*i32OutLfe*/
				2,			/*i32OutMode*/
				BRAP_DSPCHN_Ac3StereoMode_eAuto,			/*i32StereoMode*/
				0,			/*i32DualMode*/
				BRAP_DSPCHN_Ac3KaraokeVocalSelect_eBothVocal,			/*i32Kmode*/
				false,			/*i32ExtDnmixEnabled*/
			     {{ 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[0] */	
		            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[1] */	
		            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[2] */	
		            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[3] */	
		            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[4] */	
		            { 0, 0, 0, 0, 0, 0}},   			/* i32ExtDnmixTab[5] */		
				false,			/*i32ExtKaraokeEnabled*/
				0,			/*i32Extv1Level*/
				0,			/*i32Extv1Pan*/
				0,			/*i32Extv2Level*/
				0,			/*i32Extv2Pan*/
				0,			/*i32ExtGmLevel*/
				0,			/*i32ExtGmPan*/
            		 {0, 1,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF}	/* ui32OutputChannelMatrix */				
			},
			{/*sUserOutputCfg[1]*/
				false,			/*i32OutLfe*/
				2,			/*i32OutMode*/
				BRAP_DSPCHN_Ac3StereoMode_eAuto,			/*i32StereoMode*/
				0,			/*i32DualMode*/
				BRAP_DSPCHN_Ac3KaraokeVocalSelect_eBothVocal,			/*i32Kmode*/
				false,			/*i32ExtDnmixEnabled*/
			     {{ 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[0] */	
		            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[1] */	
		            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[2] */	
		            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[3] */	
		            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[4] */	
		            { 0, 0, 0, 0, 0, 0}},   			/* i32ExtDnmixTab[5] */		
				false,			/*i32ExtKaraokeEnabled*/
				0,			/*i32Extv1Level*/
				0,			/*i32Extv1Pan*/
				0,			/*i32Extv2Level*/
				0,			/*i32Extv2Pan*/
				0,			/*i32ExtGmLevel*/
				0,			/*i32ExtGmPan*/
                		 {0, 1,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF}	/* ui32OutputChannelMatrix */				
			}},
			0 /*ui32SubstreamIDToDecode*/
		} ,           
		{/*sMs11DDPConfigParam*/
        	        0,  /*eDdpUsageMode*/
            BRAP_AF_P_DolbyMsDecoderType_ePrimary,  /* eDecoderType */
			1,				        /*i32NumOutPorts*/
			{{/*sUserOutputCfg[0]*/
                            BRAP_DSPCHN_Ac3CompMode_eLine,				/*i32CompMode*/
                            0x7FFFFFFF,				/*i32PcmScale*/
                            0x7FFFFFFF,				/*i3DynScaleHigh*/
                            0x7FFFFFFF,				/*i32DynScaleLow*/
				false,			/*i32OutLfe*/
				2,			/*i32OutMode*/
				BRAP_DSPCHN_Ac3StereoMode_eAuto,			/*i32StereoMode*/
				0,			/*i32DualMode*/
				BRAP_DSPCHN_Ac3KaraokeVocalSelect_eBothVocal,			/*i32Kmode*/
				false,			/*i32ExtDnmixEnabled*/
			     {{ 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[0] */	
		            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[1] */	
		            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[2] */	
		            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[3] */	
		            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[4] */	
		            { 0, 0, 0, 0, 0, 0}},   			/* i32ExtDnmixTab[5] */		
				false,			/*i32ExtKaraokeEnabled*/
				0,			/*i32Extv1Level*/
				0,			/*i32Extv1Pan*/
				0,			/*i32Extv2Level*/
				0,			/*i32Extv2Pan*/
				0,			/*i32ExtGmLevel*/
				0,			/*i32ExtGmPan*/
            		 {0, 1,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF}	/* ui32OutputChannelMatrix */				
			},
			{/*sUserOutputCfg[0]*/
                            BRAP_DSPCHN_Ac3CompMode_eLine,				/*i32CompMode*/
                            0x7FFFFFFF,				/*i32PcmScale*/
                            0x7FFFFFFF,				/*i3DynScaleHigh*/
                            0x7FFFFFFF,				/*i32DynScaleLow*/
				false,			/*i32OutLfe*/
				2,			/*i32OutMode*/
				BRAP_DSPCHN_Ac3StereoMode_eAuto,			/*i32StereoMode*/
				0,			/*i32DualMode*/
				BRAP_DSPCHN_Ac3KaraokeVocalSelect_eBothVocal,			/*i32Kmode*/
				false,			/*i32ExtDnmixEnabled*/
			     {{ 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[0] */	
		            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[1] */	
		            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[2] */	
		            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[3] */	
		            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[4] */	
		            { 0, 0, 0, 0, 0, 0}},   			/* i32ExtDnmixTab[5] */		
				false,			/*i32ExtKaraokeEnabled*/
				0,			/*i32Extv1Level*/
				0,			/*i32Extv1Pan*/
				0,			/*i32Extv2Level*/
				0,			/*i32Extv2Pan*/
				0,			/*i32ExtGmLevel*/
				0,			/*i32ExtGmPan*/
            		 {0, 1,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF}	/* ui32OutputChannelMatrix */				
			}},
			0,                      /*i32StreamDialNormEnable*/
			0,                      /*i32UserDialNormVal*/			
			0 /*ui32SubstreamIDToDecode*/
        },
        {/*sMs11AC3ConfigParam*/
        	        0,  /*eDdpUsageMode*/
                    BRAP_AF_P_DolbyMsDecoderType_ePrimary,  /* eDecoderType */
			1,				        /*i32NumOutPorts*/
			{{/*sUserOutputCfg[0]*/
                            BRAP_DSPCHN_Ac3CompMode_eLine,				/*i32CompMode*/
                            0x7FFFFFFF,				/*i32PcmScale*/
                            0x7FFFFFFF,				/*i3DynScaleHigh*/
                            0x7FFFFFFF,				/*i32DynScaleLow*/
				false,			/*i32OutLfe*/
				2,			/*i32OutMode*/
				BRAP_DSPCHN_Ac3StereoMode_eAuto,			/*i32StereoMode*/
				0,			/*i32DualMode*/
				BRAP_DSPCHN_Ac3KaraokeVocalSelect_eBothVocal,			/*i32Kmode*/
				false,			/*i32ExtDnmixEnabled*/
			     {{ 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[0] */	
		            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[1] */	
		            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[2] */	
		            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[3] */	
		            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[4] */	
		            { 0, 0, 0, 0, 0, 0}},   			/* i32ExtDnmixTab[5] */		
				false,			/*i32ExtKaraokeEnabled*/
				0,			/*i32Extv1Level*/
				0,			/*i32Extv1Pan*/
				0,			/*i32Extv2Level*/
				0,			/*i32Extv2Pan*/
				0,			/*i32ExtGmLevel*/
				0,			/*i32ExtGmPan*/
            		 {0, 1,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF}	/* ui32OutputChannelMatrix */				
			},
			{/*sUserOutputCfg[0]*/
                            BRAP_DSPCHN_Ac3CompMode_eLine,				/*i32CompMode*/
                            0x7FFFFFFF,				/*i32PcmScale*/
                            0x7FFFFFFF,				/*i3DynScaleHigh*/
                            0x7FFFFFFF,				/*i32DynScaleLow*/
				false,			/*i32OutLfe*/
				2,			/*i32OutMode*/
				BRAP_DSPCHN_Ac3StereoMode_eAuto,			/*i32StereoMode*/
				0,			/*i32DualMode*/
				BRAP_DSPCHN_Ac3KaraokeVocalSelect_eBothVocal,			/*i32Kmode*/
				false,			/*i32ExtDnmixEnabled*/
			     {{ 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[0] */	
		            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[1] */	
		            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[2] */	
		            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[3] */	
		            { 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[4] */	
		            { 0, 0, 0, 0, 0, 0}},   			/* i32ExtDnmixTab[5] */		
				false,			/*i32ExtKaraokeEnabled*/
				0,			/*i32Extv1Level*/
				0,			/*i32Extv1Pan*/
				0,			/*i32Extv2Level*/
				0,			/*i32Extv2Pan*/
				0,			/*i32ExtGmLevel*/
				0,			/*i32ExtGmPan*/
            		 {0, 1,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF}	/* ui32OutputChannelMatrix */				
			}},
			0,                      /*i32StreamDialNormEnable*/
			0,                      /*i32UserDialNormVal*/			
			0 /*ui32SubstreamIDToDecode*/
            },
		{/*sDecodeTsmConfigParam*/
                    90,                           /*i32TSMSmoothThreshold*/
                    0,				/*i32TSMSyncLimitThreshold*/
                    360,				/*i32TSMGrossThreshold*/		
                    135000,				/*i32TSMDiscardThreshold*/
                    0,				/*i32TsmTransitionThreshold*/
                    0,				/*ui32STCAddr*/
                    0,				/*ui32AVOffset*/
                    0,				/*ui32PVROffset*/
                    5760,                          /*ui32AudioOffset*/
                    BRAP_FWIF_eTsmBool_True,	/*eEnableTSMErrorRecovery*/                    
                    BRAP_FWIF_eTsmBool_False,	/*eSTCValid*/                    
                    BRAP_FWIF_eTsmBool_False,	/*ePlayBackOn*/                    
                    BRAP_FWIF_eTsmBool_True,	/*eTsmEnable*/                    
                    BRAP_FWIF_eTsmBool_True,	/*eTsmLogEnable*/
                    BRAP_FWIF_eTsmBool_False,   /*eASTMEnable */
		},
              {/*sEncodeTsmConfigParam*/
                    90,                           /*i32TSMSmoothThreshold*/
                    0,				/*i32TSMSyncLimitThreshold*/
                    360,				/*i32TSMGrossThreshold*/		
                    135000,				/*i32TSMDiscardThreshold*/
                    0,				/*i32TsmTransitionThreshold*/
                    0,				/*ui32STCAddr*/
                    0,				/*ui32AVOffset*/
                    0,				/*ui32PVROffset*/
                    5760,                          /*ui32AudioOffset*/
                    BRAP_FWIF_eTsmBool_True,	/*eEnableTSMErrorRecovery*/                    
                    BRAP_FWIF_eTsmBool_False,	/*eSTCValid*/                    
                    BRAP_FWIF_eTsmBool_False,	/*ePlayBackOn*/                    
                    BRAP_FWIF_eTsmBool_True,	/*eTsmEnable*/                    
                    BRAP_FWIF_eTsmBool_True,	/*eTsmLogEnable*/                                       
                    BRAP_FWIF_eTsmBool_False,   /*eASTMEnable */                    
		},
              {/*sPassthruTsmConfigParam*/
                    90,                           /*i32TSMSmoothThreshold*/
                    0,				/*i32TSMSyncLimitThreshold*/
                    360,				/*i32TSMGrossThreshold*/		
                    135000,				/*i32TSMDiscardThreshold*/
                    0,				/*i32TsmTransitionThreshold*/
                    0,				/*ui32STCAddr*/
                    0,				/*ui32AVOffset*/
                    0,				/*ui32PVROffset*/
                    5760,                          /*ui32AudioOffset*/
                    BRAP_FWIF_eTsmBool_True,	/*eEnableTSMErrorRecovery*/                    
                    BRAP_FWIF_eTsmBool_False,	/*eSTCValid*/                    
                    BRAP_FWIF_eTsmBool_False,	/*ePlayBackOn*/                    
                    BRAP_FWIF_eTsmBool_True,	/*eTsmEnable*/                    
                    BRAP_FWIF_eTsmBool_True,	/*eTsmLogEnable*/                                        
                    BRAP_FWIF_eTsmBool_False,   /*eASTMEnable */                                     
		},
              {/*sPassthruConfigParam*/
                BRAP_FWIF_ePassthruType_SPDIF
		},
		
              {/*sFrameSyncConfigParams*/
                0,/*eEnablePESBasedFrameSync*/
                BRAP_FWIF_P_AudioInputSource_eExtI2s0,/*eAudioIpSourceType*/
                {         
                    48000/*uAudioIpSourceDetail*/
                },
                0,  /*eEnableTargetSync*/
                {
                    BRAP_FWIF_P_ASFPTSType_eInterpolated,         /*eAsfPtsType*/       
                    BRAP_FWIF_P_WMAIpType_eASF                      /*eWMAIpType*/
                }
		},

	},
        {/*sUserConfigAppFormat*/
            100,            /*i32MpegLeftChannelGain*/
            100,            /*i32MpegRightChannelGain*/
            100,            /*i32Ac3PcmScale*/
            100,            /*i32Ac3DynScaleHigh*/
            100,            /*i32Ac3DynScaleLow*/
            100,            /*i32Ms11Ac3PcmScale_DownMixPath*/
            100,            /*i32Ms11Ac3DynScaleHigh_DownMixPath*/
            100,            /*i32Ms11Ac3DynScaleLow_DownMixPath*/                        
            100,            /*i32Ms11DdpPcmScale_DownMixPath*/
            100,            /*i32Ms11DdpDynScaleHigh_DownMixPath*/
            100,            /*i32Ms11DdpDynScaleLow_DownMixPath*/            
            100,            /*i32Ac3PlusPcmScale*/
            100,            /*i32Ac3PlusDynScaleHigh*/
            100,            /*i32Ac3PlusDynScaleLow*/
            100,            /*i32AacDrcGainControlCompress*/
            100,            /*i32AacDrcGainControlBoost*/
            100,            /*i32DtsBroadcastDynScaleHigh*/
            100,            /*i32DtsBroadcastDynScaleLow*/            
            true,           /*bMsUsageMode*/
            false           /*bMpegConformanceMode*/            
	}
};

static const BRAP_DSPCHN_P_Settings sDefDspChnSettings = {
    (unsigned int)-1
};

static BERR_Code BRAP_P_GetInterframeImgId(
			BRAP_Handle hRap,
			BRAP_AF_P_AlgoId	eFwExecId,
			uint32_t	*ui32ImgId,
			bool *bDownloaded);


 static BERR_Code
BRAP_DSPCHN_P_SetTsmConfig (
	BRAP_DSPCHN_Handle	hDspCh,
	unsigned int        uiConfigBufAddr,
    BRAP_FWIF_P_FwTaskHandle hFwTask,
    BRAP_FWIF_P_TsmConfigParams     *psTsmConfigParam, 
    BRAP_FWIF_P_ResponseType        *peResponseType   
);

static BERR_Code
BRAP_DSPCHN_P_SetTsmConfig_isr (
	BRAP_DSPCHN_Handle	hDspCh,
	unsigned int        uiConfigBufAddr,
    BRAP_FWIF_P_FwTaskHandle hFwTask,
    BRAP_FWIF_P_TsmConfigParams     *psTsmConfigParam, 
    BRAP_FWIF_P_ResponseType        *peResponseType   
);

static BERR_Code
BRAP_DSPCHN_P_GetMpegInfo(
    BRAP_DSPCHN_Handle      hDspCh,
	unsigned int	        uiStatusBufAddr,
	BRAP_DSPCHN_StreamInfo *psStreamInfo
	);
#if 1
static BERR_Code
BRAP_DSPCHN_P_GetDdpMultiStreamInfo(
    BRAP_DSPCHN_Handle      hDspCh,
	unsigned int	        uiStatusBufAddr,
	BRAP_DSPCHN_StreamInfo *psStreamInfo
	);
#else
static BERR_Code
BRAP_DSPCHN_P_GetDdpInfo(
    BRAP_DSPCHN_Handle      hDspCh,
	unsigned int	        uiStatusBufAddr,
	BRAP_DSPCHN_StreamInfo *psStreamInfo
	);

static BERR_Code
BRAP_DSPCHN_P_GetAc3Info(
    BRAP_DSPCHN_Handle      hDspCh,
	unsigned int	        uiStatusBufAddr,
	BRAP_DSPCHN_StreamInfo *psStreamInfo
	);
#endif

static BERR_Code
BRAP_DSPCHN_P_GetLpcmDvdInfo(
    BRAP_DSPCHN_Handle      hDspCh,
	unsigned int	        uiStatusBufAddr,
	BRAP_DSPCHN_StreamInfo *psStreamInfo
	);
#ifdef RAP_MULTISTREAM_DECODER_SUPPORT 
static BERR_Code
BRAP_DSPCHN_P_GetDolbyPulseInfo(
    BRAP_DSPCHN_Handle      hDspCh,
	unsigned int	        uiStatusBufAddr,
	BRAP_DSPCHN_StreamInfo *psStreamInfo
	);
#else
static BERR_Code
BRAP_DSPCHN_P_GetAacheInfo(
    BRAP_DSPCHN_Handle      hDspCh,
	unsigned int	        uiStatusBufAddr,
	BRAP_DSPCHN_StreamInfo *psStreamInfo
	);
#endif

static BERR_Code
BRAP_DSPCHN_P_GetWmaStdInfo(
    BRAP_DSPCHN_Handle      hDspCh,
	unsigned int	        uiStatusBufAddr,
	BRAP_DSPCHN_StreamInfo *psStreamInfo
	);

static BERR_Code
BRAP_DSPCHN_P_GetWmaProInfo(
    BRAP_DSPCHN_Handle      hDspCh,
	unsigned int	        uiStatusBufAddr,
	BRAP_DSPCHN_StreamInfo *psStreamInfo
	);
static BERR_Code
BRAP_DSPCHN_P_GetDtsBroadcastInfo(
    BRAP_DSPCHN_Handle      hDspCh,
	unsigned int	        uiStatusBufAddr,
	BRAP_DSPCHN_StreamInfo *psStreamInfo
	);
static BERR_Code
BRAP_DSPCHN_P_GetDtsHdInfo(
    BRAP_DSPCHN_Handle      hDspCh,
	unsigned int	        uiStatusBufAddr,
	BRAP_DSPCHN_StreamInfo *psStreamInfo
	);
static BERR_Code
BRAP_DSPCHN_P_GetPcmWavInfo(
    BRAP_DSPCHN_Handle      hDspCh,
	unsigned int	        uiStatusBufAddr,
	BRAP_DSPCHN_StreamInfo *psStreamInfo
	);

static BERR_Code
BRAP_DSPCHN_P_GetAmrInfo(
    BRAP_DSPCHN_Handle      hDspCh,
	unsigned int	        uiStatusBufAddr,
	BRAP_DSPCHN_StreamInfo *psStreamInfo
	);
static BERR_Code
BRAP_DSPCHN_P_GetDraInfo(
    BRAP_DSPCHN_Handle      hDspCh,
	unsigned int	        uiStatusBufAddr,
	BRAP_DSPCHN_StreamInfo *psStreamInfo
	);

static BERR_Code
BRAP_DSPCHN_P_GetRealAudioLbrInfo(
    BRAP_DSPCHN_Handle      hDspCh,
	unsigned int	        uiStatusBufAddr,
	BRAP_DSPCHN_StreamInfo *psStreamInfo
	);


static BERR_Code
BRAP_DSPCHN_P_GetCustomVoiceInfo(
    BRAP_DSPCHN_Handle          hDspCh,
	unsigned int	            uiStatusBufAddr,
	BRAP_DSPCHN_ProcessingInfo *psProcessingInfo
	);

static BERR_Code
BRAP_DSPCHN_P_GetAudysseyVolumeInfo(
    BRAP_DSPCHN_Handle          hDspCh,
	unsigned int	            uiStatusBufAddr,
	BRAP_DSPCHN_ProcessingInfo *psProcessingInfo
	);

static BERR_Code
BRAP_DSPCHN_P_PopulateRBufIds(
	BRAP_ChannelHandle  hRapCh,
	BRAP_P_DspOutputBufferConfig	*psDspOutConfig,
    const BRAP_DSPCHN_P_AudioParams *psAudioParams,	
    BRAP_P_DstDetails               *pDstDetails
	);
/******************************************************************************
Summary:
	This function returns the memory requirement for downloading executable.
	Memory allocation should interpret the requirement in following manner.

	If(true == hRap->sSettings.bOpenTimeFwDownload) 
	{
		Then allocate consecutive memory of size pui32Size, And then Initialize
		the following pointers as well according to size in strictly following sequence.

			hRap->sDwnldMemInfo->ui32OSKernalCodePtr;
			hRap->sDwnldMemInfo->ui32TsmCodePtr;
			hRap->sDwnldMemInfo->sProcessingAlgoBufInfo[0].ui32BaseAddr;		
			hRap->sDwnldMemInfo->sFsBufInfo[0].ui32BaseAddr;
			hRap->sDwnldMemInfo->sDecCodecBufInfo[0].ui32BaseAddr;
			hRap->sDwnldMemInfo->sPtCodecBufInfo[0].ui32BaseAddr;	

		Sizes for them are stored in following parameters correspondingly

			pDwnldMemInfo->ui32OSKernalCodeSize;
			pDwnldMemInfo->ui32TsmCodeSize;
			pDwnldMemInfo->sProcessingAlgoBufInfo[0].ui32Size;		
			pDwnldMemInfo->sFsBufInfo[0].ui32Size;
			pDwnldMemInfo->sDecCodecBufInfo[0].ui32Size;
			pDwnldMemInfo->sPtCodecBufInfo[0].ui32Size;	
	}
	else
	{
		Allocate as following.
			-- Allocate pDwnldMemInfo->ui32OSKernalCodeSize	
				and store ptr at hRap->sDwnldMemInfo->ui32OSKernalCodePtr;
			-- Allocate pDwnldMemInfo->ui32TsmCodeSize	
				and store ptr at hRap->sDwnldMemInfo->ui32TsmCodePtr;
			if()
			{
				-- Allocate pDwnldMemInfo->sProcessingAlgoBufInfo[0].ui32Size
					and store ptr at hRap->sDwnldMemInfo->sProcessingAlgoBufInfo[0].ui32BaseAddr;					
			}
			else
			{
				-- Allocate BRAP_DSP_P_MAX_AUD_PROCESSING_CTX buffer of size pDwnldMemInfo->sProcessingAlgoBufInfo[0].ui32Size
					and store ptr at hRap->sDwnldMemInfo->sProcessingAlgoBufInfo[0 - (BRAP_DSP_P_MAX_AUD_PROCESSING_CTX-1)].ui32BaseAddr;					
			}
			-- Allocate BRAP_DSP_P_MAX_AUD_PROCESSING_CTX buffer of size pDwnldMemInfo->sProcessingAlgoBufInfo[0].ui32Size
				and store ptr at hRap->sDwnldMemInfo->sProcessingAlgoBufInfo[0 - (BRAP_DSP_P_MAX_AUD_PROCESSING_CTX-1)].ui32BaseAddr;

			-- Allocate BRAP_DSP_P_MAX_FS_CTXT buffer of size pDwnldMemInfo->sFsBufInfo[0].ui32Size
				and store ptr at hRap->sDwnldMemInfo->sFsBufInfo[0 - (BRAP_DSP_P_MAX_FS_CTXT - 1)].ui32BaseAddr;

			-- Allocate BRAP_DSP_P_MAX_DECODE_CTXT buffer of size pDwnldMemInfo->sDecCodecBufInfo[0].ui32Size;
				and store ptr at hRap->sDwnldMemInfo->sDecCodecBufInfo[0 - (BRAP_DSP_P_MAX_DECODE_CTXT - 1)].ui32BaseAddr;

			-- Allocate BRAP_DSP_P_MAX_PASSTHRU_CTXT buffer of size pDwnldMemInfo->sPtCodecBufInfo[0].ui32Size
				and store ptr at hRap->sDwnldMemInfo->sPtCodecBufInfo[0 - (BRAP_DSP_P_MAX_PASSTHRU_CTXT - 1)].ui32BaseAddr;
	}
*******************************************************************************/


BERR_Code BRAP_DSP_GetDownloadMemRequirement(
		BRAP_Handle	 hRap,
		BRAP_DSP_DwnldMemInfo *pDwnldMemInfo,
		uint32_t	*pui32Size)
{
    bool bOpenTimeAudProcDownload = true;
    uint32_t count = 0, i = 0;
    BRAP_Img_Id eCodeFirmwareId,eTblFirmwareId;
    uint32_t uiTempSize = 0, uiStartNodeIndex=0;
    bool bDownloaded = false;
    BRAP_AF_P_sALGO_EXEC_INFO	sExecId;
    BRAP_AF_P_AlgoId eCodeExecId = BRAP_AF_P_AlgoId_eMax, eTempExecId;       
    bool bIsExecAllocated[BRAP_AF_P_AlgoId_eMax];     

    *pui32Size = 0;

    BDBG_ENTER(BRAP_DSP_GetDownloadMemRequirement);
    /* Array to remember if the memory is not getting allocated twice for same Executable*/
    for(i=0; i < BRAP_AF_P_AlgoId_eMax; i++)
    {
        bIsExecAllocated[i] = false;
    }
	
    BDBG_ASSERT(pDwnldMemInfo);
    
    if(!(BRAP_P_IsPointerValid((void *)pDwnldMemInfo)))
    {
          return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    else
    {
        BKNI_Memset(pDwnldMemInfo,0,sizeof(BRAP_DSP_DwnldMemInfo));
    }
    
	/* Sum of sizes of resident code Buf*/
	 BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eSystemIboot, &(pDwnldMemInfo->ui32SystemIbootCodeSize));
	*pui32Size += pDwnldMemInfo->ui32SystemIbootCodeSize;
	

	 BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eSystemCode, &( pDwnldMemInfo->ui32SystemCodeSize));
	*pui32Size += pDwnldMemInfo->ui32SystemCodeSize;
	
    	 BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eSystemData, &(pDwnldMemInfo->ui32SystemDataSize));
	*pui32Size += pDwnldMemInfo->ui32SystemDataSize;
	
    	 BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eSystemRdb, &(pDwnldMemInfo->ui32SystemRdbVariablesSize));
	*pui32Size += pDwnldMemInfo->ui32SystemRdbVariablesSize;    
	
#ifdef RAP_SCM_SUPPORT
    	 BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eSystemTableOfHashes, &(pDwnldMemInfo->ui32SystemTableOfHashesSize));
	*pui32Size += pDwnldMemInfo->ui32SystemTableOfHashesSize;    
#endif	
	
	/*Fill it accordingly */

	/* Sum of sizes of Tsm Exec Buf*/

	uiTempSize += hRap->sImgIdMappingArrays.ui32CodeSize[BRAP_AF_P_AlgoId_eDecodeTsm];
#if 0
                            +hRap->sImgIdMappingArrays.ui32CodeSize[BRAP_AF_P_AlgoId_eEncodeTsm]
                            +hRap->sImgIdMappingArrays.ui32CodeSize[BRAP_AF_P_AlgoId_ePassthroughTsm];
#endif	
	pDwnldMemInfo->ui32TsmCodeSize = uiTempSize;
	*pui32Size += uiTempSize;
	uiTempSize = 0;

	/* Sum of sizes of System task code */
	uiTempSize += hRap->sImgIdMappingArrays.ui32CodeSize[BRAP_AF_P_AlgoId_eSystemDecodeTask]
                            +hRap->sImgIdMappingArrays.ui32CodeSize[BRAP_AF_P_AlgoId_eSystemGraphicTask]
#ifdef RAP_SCM_SUPPORT                            
                            +hRap->sImgIdMappingArrays.ui32CodeSize[BRAP_AF_P_AlgoId_eSystemVideoTask]
                            +hRap->sImgIdMappingArrays.ui32CodeSize[BRAP_AF_P_AlgoId_eSCMTask];
#else
                            +hRap->sImgIdMappingArrays.ui32CodeSize[BRAP_AF_P_AlgoId_eSystemVideoTask];
#endif

	pDwnldMemInfo->ui32SystemTaskCodeSize= uiTempSize;
	*pui32Size += uiTempSize;
	uiTempSize = 0;

	BRAP_FWDWNLD_P_IfOpenTimeProcDownload(&bOpenTimeAudProcDownload);

	if(true == bOpenTimeAudProcDownload)
	{		
        	/* Sum of sizes of Post Processing Exec Buf*/
		for(count = 0; count < BRAP_CIT_P_ProcessingType_eMax; count++)
		{
		/* Check if audio processing algorithm is supported */
			if(!BRAP_FWDWNLD_P_IsAudProcSupported(count))
			{
				continue;
			}
			BRAP_FWDWNLD_P_GetAudProcExecIds((BRAP_CIT_P_ProcessingType)count,&sExecId);
            if(count == BRAP_CIT_P_ProcessingType_eFWMixerPostProc)
                uiStartNodeIndex = 0;
            else
                uiStartNodeIndex = 2;
		    /* For each stage of audio processing algorithm	*/	
			for(i = uiStartNodeIndex;i<sExecId.NumNodes;i++)
			{
				/* Get size of audio processing code */	
                            eCodeExecId = sExecId.eAlgoIds[i];
        			eCodeFirmwareId = hRap->sImgIdMappingArrays.ui32CodeImgId[eCodeExecId];

				if(BRAP_Img_Id_eInvalid == eCodeFirmwareId)
				{
					continue;
				}
                            if(true == bIsExecAllocated[eCodeExecId])
                            {
                                continue;
                            }
                            else
                            {
                                bIsExecAllocated[eCodeExecId] = true;
                            }
                uiTempSize += hRap->sImgIdMappingArrays.ui32CodeSize[eCodeExecId];
                
                /* Get size of audio processing table */	
				eTblFirmwareId = hRap->sImgIdMappingArrays.ui32TableImgId[eCodeExecId];
				if(BRAP_Img_Id_eInvalid == eTblFirmwareId)
				{
					continue;
				}
				/*If the previous table is used for this stage, dont sum its size */
                            bDownloaded =false;
				BRAP_FWDWNLD_P_IsTableDownloaded(BRAP_DSP_ExecType_eProcessingAlgo,eCodeExecId,&bDownloaded,&eTempExecId);
				if(!bDownloaded)
				{
					uiTempSize += hRap->sImgIdMappingArrays.ui32TableSize[eCodeExecId];
				}
			}
		}
		
		pDwnldMemInfo->sProcessingAlgoBufInfo[0].ui32Size = uiTempSize;
		*pui32Size += uiTempSize;
		uiTempSize = 0;
	}
	else
	{
		/* BRAP_DSP_P_MAX_AUD_PROCESSING_CTX buffers of size MAX(sizeof(ProcessingAlgo Code +Table))*/
		/* Sum of sizes of Post Processing Exec Buf*/
		for(count = 0; count < BRAP_CIT_P_ProcessingType_eMax; count++)
		{
		/* Check if audio processing algorithm is supported */
			if(!BRAP_FWDWNLD_P_IsAudProcSupported(count))
			{
				continue;
			}
			uiTempSize = 0;			
			BRAP_FWDWNLD_P_GetAudProcExecIds((BRAP_ProcessingType)count,&sExecId);
		/* For each stage of audio processing algorithm	*/	
			for(i = 2;i<sExecId.NumNodes;i++)
			{
				/* Get size of audio processing code */	
                            eCodeExecId = sExecId.eAlgoIds[i];
        			eCodeFirmwareId = hRap->sImgIdMappingArrays.ui32CodeImgId[eCodeExecId];

				if(BRAP_Img_Id_eInvalid == eCodeFirmwareId)
				{
					continue;
				}
                uiTempSize += hRap->sImgIdMappingArrays.ui32CodeSize[eCodeExecId];
				/* Get size of audio processing table */	

				eTblFirmwareId = hRap->sImgIdMappingArrays.ui32TableImgId[eCodeExecId];
				if(BRAP_Img_Id_eInvalid == eTblFirmwareId)
				{
					continue;
				}
				/*If the previous table is used for this stage, dont sum its size */
                            bDownloaded = false;
				BRAP_FWDWNLD_P_IsTableDownloaded(BRAP_DSP_ExecType_eProcessingAlgo,eCodeExecId,&bDownloaded,&eTempExecId);
				if(!bDownloaded)
				{
					uiTempSize += hRap->sImgIdMappingArrays.ui32TableSize[eCodeExecId];
				}
			}
			if(pDwnldMemInfo->sProcessingAlgoBufInfo[0].ui32Size < uiTempSize)
				pDwnldMemInfo->sProcessingAlgoBufInfo[0].ui32Size = uiTempSize;
		}
	}


	BDBG_MSG(("hRap->sSettings.bOpenTimeFwDownload = %d",hRap->sSettings.bOpenTimeFwDownload));

	if(true == hRap->sSettings.bOpenTimeFwDownload)
	{
                uiTempSize =0;
            /* Sum of sizes of Encode Exec Buf*/
		for(count = 0; count < BRAP_CIT_P_EncAudioType_eMax; count++)
		{
		/* Check if Encode algorithm is supported */
			if(!BRAP_FWDWNLD_P_IsEncodeSupported(count))
			{
				continue;
			}
			BRAP_FWDWNLD_P_GetEncodeExecIds((BRAP_CIT_P_EncAudioType)count,&sExecId);

        /* Frame sync of Encode */
                    eCodeExecId = sExecId.eAlgoIds[0];
                    if(eCodeExecId != BRAP_AF_P_AlgoId_eInvalid)
                    {
                            eCodeFirmwareId = hRap->sImgIdMappingArrays.ui32CodeImgId[eCodeExecId];

                            if(BRAP_Img_Id_eInvalid != eCodeFirmwareId)
                            {	
                                    if(false == bIsExecAllocated[eCodeExecId])
                                    {
                                        bIsExecAllocated[eCodeExecId] = true;
                                        uiTempSize += hRap->sImgIdMappingArrays.ui32CodeSize[eCodeExecId];                                        
                                    }
                            }
                    }
		/* For each stage of Encode algorithm	*/	
			for(i = 2;i<sExecId.NumNodes;i++)
			{
				/* Get size of audio processing code */	
                            eCodeExecId = sExecId.eAlgoIds[i];
                            if(eCodeExecId == BRAP_AF_P_AlgoId_eInvalid)
                            {
                                continue;
                            }
        			eCodeFirmwareId = hRap->sImgIdMappingArrays.ui32CodeImgId[eCodeExecId];

				if(BRAP_Img_Id_eInvalid == eCodeFirmwareId)
				{
					break;
				}
                            if(true == bIsExecAllocated[eCodeExecId])
                            {
                                continue;
                            }
                            else
                            {
                                bIsExecAllocated[eCodeExecId] = true;
                            }
				uiTempSize += hRap->sImgIdMappingArrays.ui32CodeSize[eCodeExecId];

				/* Get size of Encode table */	
				eTblFirmwareId = hRap->sImgIdMappingArrays.ui32TableImgId[eCodeExecId];
				if(BRAP_Img_Id_eInvalid == eTblFirmwareId)
				{
					continue;
				}
				/*If the previous table is used for this stage, dont sum its size */
                            bDownloaded =false;
				BRAP_FWDWNLD_P_IsTableDownloaded(BRAP_DSP_ExecType_eEncode,eCodeExecId,&bDownloaded,&eTempExecId);
				if(!bDownloaded)
				{
					uiTempSize += hRap->sImgIdMappingArrays.ui32TableSize[eCodeExecId];
				}
			}
		    }

		pDwnldMemInfo->sEncodeCodecBufInfo[0].ui32Size = uiTempSize;
		*pui32Size += uiTempSize;
		uiTempSize = 0;
    
		for(count = 0; count < BRAP_DSPCHN_AudioType_eMax; count++)
		{
		/* Check if decode algorithm is supported */
                if(!(BRAP_P_IsPassthruSupportedWithoutLicense(count)))
                {
			if(!BRAP_FWDWNLD_P_IsAudCodecSupported(count))
			{
				continue;
			}
                }
			BRAP_FWDWNLD_P_GetExecIds(BRAP_DSPCHN_DecodeMode_eDecode,(BRAP_DSPCHN_AudioType)count,&sExecId);
		/* Sum of sizes of Framesync Exec */
			/* For each stage framesync	*/	
			
                    eCodeExecId = sExecId.eAlgoIds[0];
                    if(eCodeExecId != BRAP_AF_P_AlgoId_eInvalid)
                    {
                            eCodeFirmwareId = hRap->sImgIdMappingArrays.ui32CodeImgId[eCodeExecId];

                            if(BRAP_Img_Id_eInvalid != eCodeFirmwareId)
                            {                            
                                if(false == bIsExecAllocated[eCodeExecId])
                                {
                                    bIsExecAllocated[eCodeExecId] = true;
                                    uiTempSize += hRap->sImgIdMappingArrays.ui32CodeSize[eCodeExecId];                                    
                                }

                            }
                    }
		
		}

		pDwnldMemInfo->sFsBufInfo[0].ui32Size = uiTempSize;
		*pui32Size += uiTempSize;
		uiTempSize = 0;

		for(count = 0; count < BRAP_DSPCHN_AudioType_eMax; count++)
		{
		/* Check if decode algorithm is supported */
			if(!BRAP_FWDWNLD_P_IsAudCodecSupported(count))
			{
				continue;
			}
			BRAP_FWDWNLD_P_GetExecIds(BRAP_DSPCHN_DecodeMode_eDecode,(BRAP_DSPCHN_AudioType)count,&sExecId);			
		/* Sum of sizes of Decode code+table Buf */

			for(i = 2;i<sExecId.NumNodes;i++)
			{
				/* Get size of Decode code */	
                            eCodeExecId = sExecId.eAlgoIds[i];
                            if(eCodeExecId == BRAP_AF_P_AlgoId_eInvalid)
                            {
                                continue;
                            }          
        			eCodeFirmwareId = hRap->sImgIdMappingArrays.ui32CodeImgId[eCodeExecId];

				if(BRAP_Img_Id_eInvalid == eCodeFirmwareId)
				{
					break;
				}
                
                            if(true == bIsExecAllocated[eCodeExecId])
                            {
                                continue;
                            }
                            else
                            {
                                bIsExecAllocated[eCodeExecId] = true;
                            }
				uiTempSize += hRap->sImgIdMappingArrays.ui32CodeSize[eCodeExecId];


				/* Get size of Decode table */	

				eTblFirmwareId = hRap->sImgIdMappingArrays.ui32TableImgId[eCodeExecId];
				if(BRAP_Img_Id_eInvalid == eTblFirmwareId)
				{
					continue;
				}
				/*If the previous table is used for this stage, dont sum its size */
                            bDownloaded =false;
				BRAP_FWDWNLD_P_IsTableDownloaded(BRAP_DSP_ExecType_eDecode,eCodeExecId,&bDownloaded,&eTempExecId);
				if(!bDownloaded)
				{
					uiTempSize += hRap->sImgIdMappingArrays.ui32TableSize[eCodeExecId];
				}
			}
		}

		pDwnldMemInfo->sDecCodecBufInfo[0].ui32Size = uiTempSize;
		*pui32Size += uiTempSize;
		uiTempSize = 0;

		/* Get size of  Passthru Code */	
		uiTempSize += hRap->sImgIdMappingArrays.ui32CodeSize[BRAP_AF_P_AlgoId_ePassThru];
		/* Get size of  Passthru table */	
		uiTempSize += hRap->sImgIdMappingArrays.ui32TableSize[BRAP_AF_P_AlgoId_ePassThru];

		pDwnldMemInfo->sPtCodecBufInfo[0].ui32Size = uiTempSize;
		*pui32Size += uiTempSize;
		uiTempSize = 0;
#ifdef RAP_VIDEOONDSP_SUPPORT
            /*Calculate Real Video Exec Size*/
            for(count = 0; count < (BRAP_DSPCHN_VideoType_eMax - BRAP_VIDEO_CODEC_START_INDEX); count++)
		{		    
		/* Check if decode algorithm is supported */
			if(!BRAP_FWDWNLD_P_IsVidCodecSupported(count))
			{
				continue;
			}		    
			BRAP_FWDWNLD_P_GetVidExecIds(BRAP_DSPCHN_DecodeMode_eDecode,(BRAP_DSPCHN_VideoType)(count + BRAP_VIDEO_CODEC_START_INDEX),&sExecId);			
		/* Sum of sizes of Decode code+table Buf */       

			for(i = 0;i<sExecId.NumNodes;i++)
			{		    
				/* Get size of Decode code */	
                            eCodeExecId = sExecId.eAlgoIds[i];
                            if(eCodeExecId == BRAP_AF_P_AlgoId_eInvalid)
                            {
                                continue;
                            }          		                             
        			eCodeFirmwareId = hRap->sImgIdMappingArrays.ui32CodeImgId[eCodeExecId];

				if(BRAP_Img_Id_eInvalid == eCodeFirmwareId)
				{
					continue;
				}		                
                            if(true == bIsExecAllocated[eCodeExecId])
                            {
                                continue;
                            }
                            else
                            {
                                bIsExecAllocated[eCodeExecId] = true;
                            }		                             
				uiTempSize += hRap->sImgIdMappingArrays.ui32CodeSize[eCodeExecId];
                

				/* Get size of Decode table */	

				eTblFirmwareId = hRap->sImgIdMappingArrays.ui32TableImgId[eCodeExecId];
				if(BRAP_Img_Id_eInvalid == eTblFirmwareId)
				{
					continue;
				}                      
				/*If the previous table is used for this stage, dont sum its size */
                            bDownloaded =false;
				BRAP_FWDWNLD_P_IsTableDownloaded(BRAP_DSP_ExecType_eDecode,eCodeExecId,&bDownloaded,&eTempExecId);
				if(!bDownloaded)
				{                            				
					uiTempSize += hRap->sImgIdMappingArrays.ui32TableSize[eCodeExecId];                                  
				}
			}
		}          
		pDwnldMemInfo->sVideoDecCodecBufInfo[0].ui32Size = uiTempSize;
		*pui32Size += uiTempSize;
		uiTempSize = 0;  
#endif   

#ifdef RAP_GFX_SUPPORT
                /*Calculate GFX Size*/
                for(count = 0; count < (BRAP_DSPCHN_GfxType_eMax); count++)
            {           
            /* Check if decode algorithm is supported */
                if(!BRAP_FWDWNLD_P_IsGfxSupported(count))
                {
                    continue;
                }           
                BRAP_FWDWNLD_P_GetGfxExecIds((BRAP_DSPCHN_GfxType)(count),&sExecId);           
            /* Sum of sizes of Decode code+table Buf */       
    
                for(i = 0;i<sExecId.NumNodes;i++)
                {           
            /* Get size of Decode code */   
                        eCodeExecId = sExecId.eAlgoIds[i];
                        if(eCodeExecId == BRAP_AF_P_AlgoId_eInvalid)
                        {
                            continue;
                        }                                            
                        eCodeFirmwareId = hRap->sImgIdMappingArrays.ui32CodeImgId[eCodeExecId];
    
                        if(BRAP_Img_Id_eInvalid == eCodeFirmwareId)
                        {
                            continue;
                        }                       
                        if(true == bIsExecAllocated[eCodeExecId])
                        {
                            continue;
                        }
                        else
                        {
                            bIsExecAllocated[eCodeExecId] = true;
                        }                                    
                    uiTempSize += hRap->sImgIdMappingArrays.ui32CodeSize[eCodeExecId];                    
                    /* Get size of Decode table */  
    
                    eTblFirmwareId = hRap->sImgIdMappingArrays.ui32TableImgId[eCodeExecId];
                    if(BRAP_Img_Id_eInvalid == eTblFirmwareId)
                    {
                        continue;
                    }                      
                    /*If the previous table is used for this stage, dont sum its size */
                                bDownloaded =false;
                    BRAP_FWDWNLD_P_IsTableDownloaded(BRAP_DSP_ExecType_eDecode,eCodeExecId,&bDownloaded,&eTempExecId);
                    if(!bDownloaded)
                    {                                           
                        uiTempSize += hRap->sImgIdMappingArrays.ui32TableSize[eCodeExecId];                                  
                    }
                }
            }          
            pDwnldMemInfo->sGfxBufInfo[0].ui32Size = uiTempSize;
            *pui32Size += uiTempSize;
            uiTempSize = 0;  
#endif                  
#ifdef RAP_SCM_SUPPORT
				/*Calculate SCM Size*/
				for(count = 0; count < (BRAP_DSPCHN_ScmType_eMax); count++)
				{			
				/* Check if SCM is supported */
					if(!BRAP_FWDWNLD_P_IsScmSupported(count))
					{
						continue;
					}			
					BRAP_FWDWNLD_P_GetScmExecIds((BRAP_DSPCHN_ScmType)(count),&sExecId);		   
				/* Sum of sizes of SCM code+table Buf */ 	  
		
					for(i = 0;i<sExecId.NumNodes;i++)
					{			
				/* Get size of SCM code */	
							eCodeExecId = sExecId.eAlgoIds[i];
							if(eCodeExecId == BRAP_AF_P_AlgoId_eInvalid)
							{
								continue;
							}											 
							eCodeFirmwareId = hRap->sImgIdMappingArrays.ui32CodeImgId[eCodeExecId];
		
							if(BRAP_Img_Id_eInvalid == eCodeFirmwareId)
							{
								continue;
							}						
							if(true == bIsExecAllocated[eCodeExecId])
							{
								continue;
							}
							else
							{
								bIsExecAllocated[eCodeExecId] = true;
							}									 
						uiTempSize += hRap->sImgIdMappingArrays.ui32CodeSize[eCodeExecId];					  
						/* Get size of SCM table */	
		
						eTblFirmwareId = hRap->sImgIdMappingArrays.ui32TableImgId[eCodeExecId];
						if(BRAP_Img_Id_eInvalid == eTblFirmwareId)
						{
							continue;
						}					   
						/*If the previous table is used for this stage, dont sum its size */
									bDownloaded =false;
						BRAP_FWDWNLD_P_IsTableDownloaded(BRAP_DSP_ExecType_eScm,eCodeExecId,&bDownloaded,&eTempExecId);
						if(!bDownloaded)
						{											
							uiTempSize += hRap->sImgIdMappingArrays.ui32TableSize[eCodeExecId]; 								 
						}
					}
				}		   
				pDwnldMemInfo->sScmBufInfo[0].ui32Size = uiTempSize;
				*pui32Size += uiTempSize;
				uiTempSize = 0;  
#endif                  

	}
	else
	{
		
		/* BRAP_DSP_P_MAX_FS_CTXT buffers of size MAX(sizeof(Framesync exec))*/
		for(count = 0; count < BRAP_DSPCHN_AudioType_eMax; count++)
		{
        		/* Check if decode algorithm is supported */
                    if(!(BRAP_P_IsPassthruSupportedWithoutLicense(count)))
                    {
            			if(!BRAP_FWDWNLD_P_IsAudCodecSupported(count))
            			{
            				continue;
            			}
                    }
			BRAP_FWDWNLD_P_GetExecIds(BRAP_DSPCHN_DecodeMode_eDecode,(BRAP_DSPCHN_AudioType)count,&sExecId);
		/* Sum of sizes of Framesync Exec */
			/* For each stage framesync	*/	

                     eCodeExecId = sExecId.eAlgoIds[0];
                    if(eCodeExecId == BRAP_AF_P_AlgoId_eInvalid)
                    {
                        continue;
                    }                
    			eCodeFirmwareId = hRap->sImgIdMappingArrays.ui32CodeImgId[eCodeExecId];

			if(BRAP_Img_Id_eInvalid == eCodeFirmwareId)
			{
				continue;
			}

			uiTempSize = hRap->sImgIdMappingArrays.ui32CodeSize[eCodeExecId];
				if(pDwnldMemInfo->sFsBufInfo[0].ui32Size < uiTempSize)
					pDwnldMemInfo->sFsBufInfo[0].ui32Size = uiTempSize;
		}
		
		/* BRAP_DSP_P_MAX_DECODE_CTXT buffers of size MAX(sizeof(Decode Code+Table))*/	
		for(count = 0; count < BRAP_DSPCHN_AudioType_eMax; count++)
		{
		/* Check if decode algorithm is supported */
			if(!BRAP_FWDWNLD_P_IsAudCodecSupported(count))
			{
				continue;
			}
			
			BRAP_FWDWNLD_P_GetExecIds(BRAP_DSPCHN_DecodeMode_eDecode,(BRAP_DSPCHN_AudioType)count,&sExecId);			
			uiTempSize = 0;					
		/* Sum of sizes of Decode code+table Buf */

			for(i = 2;i<sExecId.NumNodes;i++)
			{
				/* Get size of Decode code */	
                            eCodeExecId = sExecId.eAlgoIds[i];
                            if(eCodeExecId == BRAP_AF_P_AlgoId_eInvalid)
                            {
                                continue;
                            }                     
                            eCodeFirmwareId = hRap->sImgIdMappingArrays.ui32CodeImgId[eCodeExecId];

				if(BRAP_Img_Id_eInvalid == eCodeFirmwareId)
				{
					continue;
				}

				uiTempSize += hRap->sImgIdMappingArrays.ui32CodeSize[eCodeExecId];


				/* Get size of Decode table */	

				eTblFirmwareId = hRap->sImgIdMappingArrays.ui32TableImgId[eCodeExecId];
				if(BRAP_Img_Id_eInvalid == eTblFirmwareId)
				{
					continue;
				}
				/*If the previous table is used for this stage, dont sum its size */
                            bDownloaded =false;
				BRAP_FWDWNLD_P_IsTableDownloaded(BRAP_DSP_ExecType_eDecode,eCodeExecId,&bDownloaded,&eTempExecId);
				if(!bDownloaded)
				{
					uiTempSize += hRap->sImgIdMappingArrays.ui32TableSize[eCodeExecId];
				}
			}
			if(pDwnldMemInfo->sDecCodecBufInfo[0].ui32Size < uiTempSize)
				pDwnldMemInfo->sDecCodecBufInfo[0].ui32Size = uiTempSize;
		}			

		/* BRAP_DSP_P_MAX_PASSTHRU_CTXT buffers of size MAX(sizeof(Passthru Code +Table))*/	
                uiTempSize = 0;
		/* Get size of  Passthru Code */	
		uiTempSize += hRap->sImgIdMappingArrays.ui32CodeSize[BRAP_AF_P_AlgoId_ePassThru];
		/* Get size of  Passthru table */	
		uiTempSize += hRap->sImgIdMappingArrays.ui32TableSize[BRAP_AF_P_AlgoId_ePassThru];

		pDwnldMemInfo->sPtCodecBufInfo[0].ui32Size = uiTempSize;		


		for(count = 0; count < BRAP_CIT_P_EncAudioType_eMax; count++)
		{
		/* Check if audio processing algorithm is supported */
			if(!BRAP_FWDWNLD_P_IsEncodeSupported(count))
			{
				continue;
			}
			uiTempSize = 0;			
			BRAP_FWDWNLD_P_GetEncodeExecIds((BRAP_ProcessingType)count,&sExecId);

                    eCodeExecId = sExecId.eAlgoIds[0];
                    if(eCodeExecId != BRAP_AF_P_AlgoId_eInvalid)
                    {
                        eCodeFirmwareId = hRap->sImgIdMappingArrays.ui32CodeImgId[eCodeExecId];

                        if(BRAP_Img_Id_eInvalid != eCodeFirmwareId)
                        {	
                            uiTempSize += hRap->sImgIdMappingArrays.ui32CodeSize[eCodeExecId];
                        }
                    }

		/* For each stage of audio processing algorithm	*/	
			for(i = 2;i<sExecId.NumNodes;i++)
			{
				/* Get size of audio processing code */	
				/* Get size of Decode code */	
                            eCodeExecId = sExecId.eAlgoIds[i];
                            eCodeFirmwareId = hRap->sImgIdMappingArrays.ui32CodeImgId[eCodeExecId];

				if(BRAP_Img_Id_eInvalid == eCodeFirmwareId)
				{
					continue;
				}
            uiTempSize += hRap->sImgIdMappingArrays.ui32CodeSize[eCodeExecId];
            
            /* Get size of audio processing table */	
				eTblFirmwareId = hRap->sImgIdMappingArrays.ui32TableImgId[eCodeExecId];;
				if(BRAP_Img_Id_eInvalid == eTblFirmwareId)
				{
					continue;
				}
				/*If the previous table is used for this stage, dont sum its size */
                            bDownloaded =false;
				BRAP_FWDWNLD_P_IsTableDownloaded(BRAP_DSP_ExecType_eEncode,eCodeExecId,&bDownloaded,&eTempExecId);
				if(!bDownloaded)
				{
					uiTempSize += hRap->sImgIdMappingArrays.ui32TableSize[eCodeExecId];
				}
			}
			if(pDwnldMemInfo->sEncodeCodecBufInfo[0].ui32Size < uiTempSize)
				pDwnldMemInfo->sEncodeCodecBufInfo[0].ui32Size = uiTempSize;
		}

#ifdef RAP_VIDEOONDSP_SUPPORT
            /* Real VIDEO*/
            for(count = 0; count < (BRAP_DSPCHN_VideoType_eMax - BRAP_VIDEO_CODEC_START_INDEX); count++)
		{                    		
		/* Check if decode algorithm is supported */
			if(!BRAP_FWDWNLD_P_IsVidCodecSupported(count))
			{
				continue;
			}                    					
			BRAP_FWDWNLD_P_GetVidExecIds(BRAP_DSPCHN_DecodeMode_eDecode,(BRAP_DSPCHN_VideoType)(count+BRAP_VIDEO_CODEC_START_INDEX),&sExecId);			                    		            
			uiTempSize = 0;					
		/* Sum of sizes of Decode code+table Buf */

			for(i = 0;i<sExecId.NumNodes;i++)
			{                    					
				/* Get size of Decode code */	
                            eCodeExecId = sExecId.eAlgoIds[i];
                            if(eCodeExecId == BRAP_AF_P_AlgoId_eInvalid)
                            {
                                continue;
                            }                                		                            
                            eCodeFirmwareId = hRap->sImgIdMappingArrays.ui32CodeImgId[eCodeExecId];

				if(BRAP_Img_Id_eInvalid == eCodeFirmwareId)
				{
					continue;
				}                      

				uiTempSize += hRap->sImgIdMappingArrays.ui32CodeSize[eCodeExecId];                       


				/* Get size of Decode table */	

				eTblFirmwareId = hRap->sImgIdMappingArrays.ui32TableImgId[eCodeExecId];
				if(BRAP_Img_Id_eInvalid == eTblFirmwareId)
				{
					continue;
				}                
				/*If the previous table is used for this stage, dont sum its size */
                            bDownloaded =false;
				BRAP_FWDWNLD_P_IsTableDownloaded(BRAP_DSP_ExecType_eDecode,eCodeExecId,&bDownloaded,&eTempExecId);
				if(!bDownloaded)
				{
					uiTempSize += hRap->sImgIdMappingArrays.ui32TableSize[eCodeExecId];                
				}
			}
			if(pDwnldMemInfo->sVideoDecCodecBufInfo[0].ui32Size < uiTempSize)
				pDwnldMemInfo->sVideoDecCodecBufInfo[0].ui32Size = uiTempSize;
		}
#endif

#ifdef RAP_GFX_SUPPORT
                /* GFX*/
                for(count = 0; count < (BRAP_DSPCHN_GfxType_eMax); count++)
            {                           
            /* Check if decode algorithm is supported */
                if(!BRAP_FWDWNLD_P_IsGfxSupported(count))
                {
                    continue;
                }                                       
                BRAP_FWDWNLD_P_GetGfxExecIds((BRAP_DSPCHN_GfxType)(count),&sExecId);                                                 
                uiTempSize = 0;                 
            /* Sum of sizes of Decode code+table Buf */
    
                for(i = 0;i<sExecId.NumNodes;i++)
                {                                       
                    /* Get size of Decode code */   
                    eCodeExecId = sExecId.eAlgoIds[i];
                    if(eCodeExecId == BRAP_AF_P_AlgoId_eInvalid)
                    {
                        continue;
                    }                                                                   
                    eCodeFirmwareId = hRap->sImgIdMappingArrays.ui32CodeImgId[eCodeExecId];
    
                    if(BRAP_Img_Id_eInvalid == eCodeFirmwareId)
                    {
                        continue;
                    }                      
    
                    uiTempSize += hRap->sImgIdMappingArrays.ui32CodeSize[eCodeExecId];                       
    
    
                    /* Get size of Decode table */  
    
                    eTblFirmwareId = hRap->sImgIdMappingArrays.ui32TableImgId[eCodeExecId];
                    if(BRAP_Img_Id_eInvalid == eTblFirmwareId)
                    {
                        continue;
                    }                
                    /*If the previous table is used for this stage, dont sum its size */
                                bDownloaded =false;
                    BRAP_FWDWNLD_P_IsTableDownloaded(BRAP_DSP_ExecType_eDecode,eCodeExecId,&bDownloaded,&eTempExecId);
                    if(!bDownloaded)
                    {
                        uiTempSize += hRap->sImgIdMappingArrays.ui32TableSize[eCodeExecId];                
                    }
                }
                if(pDwnldMemInfo->sGfxBufInfo[0].ui32Size < uiTempSize)
                    pDwnldMemInfo->sGfxBufInfo[0].ui32Size = uiTempSize;
            }
#endif			        
#ifdef 	RAP_SCM_SUPPORT
	/* SCM*/
	for(count = 0; count < (BRAP_DSPCHN_ScmType_eMax); count++)
	{							
	/* Check if decode algorithm is supported */
		if(!BRAP_FWDWNLD_P_IsScmSupported(count))
		{
			continue;
		}										
		BRAP_FWDWNLD_P_GetScmExecIds((BRAP_DSPCHN_ScmType)(count),&sExecId);												 
		uiTempSize = 0; 				
	/* Sum of sizes of SCM code+table Buf */
	
		for(i = 0;i<sExecId.NumNodes;i++)
		{										
			/* Get size of SCM code */	
			eCodeExecId = sExecId.eAlgoIds[i];
			if(eCodeExecId == BRAP_AF_P_AlgoId_eInvalid)
			{
				continue;
			}																	
			eCodeFirmwareId = hRap->sImgIdMappingArrays.ui32CodeImgId[eCodeExecId];
	
			if(BRAP_Img_Id_eInvalid == eCodeFirmwareId)
			{
				continue;
			}					   
	
			uiTempSize += hRap->sImgIdMappingArrays.ui32CodeSize[eCodeExecId];						 
	
	
			/* Get size of SCM table */	
	
			eTblFirmwareId = hRap->sImgIdMappingArrays.ui32TableImgId[eCodeExecId];
			if(BRAP_Img_Id_eInvalid == eTblFirmwareId)
			{
				continue;
			}				 
			/*If the previous table is used for this stage, dont sum its size */
						bDownloaded =false;
			BRAP_FWDWNLD_P_IsTableDownloaded(BRAP_DSP_ExecType_eScm,eCodeExecId,&bDownloaded,&eTempExecId);
			if(!bDownloaded)
			{
				uiTempSize += hRap->sImgIdMappingArrays.ui32TableSize[eCodeExecId]; 			   
			}
		}
		if(pDwnldMemInfo->sScmBufInfo[0].ui32Size < uiTempSize)
			pDwnldMemInfo->sScmBufInfo[0].ui32Size = uiTempSize;
	}
#endif
        
	}

	BDBG_LEAVE(BRAP_DSP_GetDownloadMemRequirement);
	return BERR_SUCCESS;
}

BERR_Code BRAP_DSPCHN_P_FreeFwCtxtBuf(
	BRAP_Handle hRap,
	uint32_t eAudType,	/* For PostProcessing it wiil be BRAP_ProcessingType. For Decode it will be audtio type*/
	BRAP_DSP_ExecType eExecType)
{
	unsigned int	i = 0;
	BRAP_DSP_P_CtxtFwBufInfo	*psTempBufInfo;
	bool	bOpenTimeAudProcDownload = false;    
	
	BDBG_ENTER(BRAP_DSPCHN_P_FreeFwCtxtBuf);

        if(true == hRap->sSettings.bOpenTimeFwDownload)
        {/* If Execs Downloaded at opentime, No need to decrement the usage count */
            goto end; 
        }
    
	if(BRAP_DSP_ExecType_eFrameSync == eExecType)
	{
		for(i = 0; i < BRAP_DSP_P_MAX_FS_CTXT;i++)
		{
			psTempBufInfo = &(hRap->sDwnldMemInfo.sFsBufInfo[i]);
			if(psTempBufInfo->uExecAlgoType.eAlgoType == (BRAP_DSPCHN_AudioType)eAudType)
			{
				if(psTempBufInfo->numUser > 0)
				{
					psTempBufInfo->numUser--;
					BDBG_MSG(("Decreasing usage count of Fs Buffer [%d] allocted" 
						"for audio type %#x, Current usage count = %#x",
						i,eAudType,psTempBufInfo->numUser));
					break;
					
				}
				else
				{
					BDBG_ERR(("Usage count of Fs Buffer [%d] is already 0."
						"This is an Error condition.",i));
					return BERR_TRACE(BERR_UNKNOWN);
				}
			}
		}
	}
	else if(BRAP_DSP_ExecType_eDecode == eExecType)
	{		
		for(i = 0; i < BRAP_DSP_P_MAX_DECODE_CTXT;i++)
		{
			psTempBufInfo = &(hRap->sDwnldMemInfo.sDecCodecBufInfo[i]);
			if(psTempBufInfo->uExecAlgoType.eAlgoType == (BRAP_DSPCHN_AudioType)eAudType)
			{
				if(psTempBufInfo->numUser > 0)
				{
					psTempBufInfo->numUser--;
					BDBG_MSG(("Decreasing usage count of Decode codec Buffer [%d] allocted" 
						"for audio type %#x, Current usage count = %#x",
						i,eAudType,psTempBufInfo->numUser));
					break;
				}
				else
				{
					BDBG_ERR(("Usage count of Decode codec Buffer [%d] is already 0."
						"This is an Error condition.",i));
					return BERR_TRACE(BERR_UNKNOWN);
				}
			}
		}
	}
	else if(BRAP_DSP_ExecType_ePassthru== eExecType)
	{	
		for(i = 0; i < BRAP_DSP_P_MAX_PASSTHRU_CTXT;i++)
		{
			psTempBufInfo = &(hRap->sDwnldMemInfo.sPtCodecBufInfo[i]);
			if(psTempBufInfo->uExecAlgoType.ePAlgoType == (BRAP_ProcessingType)eAudType)
			{
				if(psTempBufInfo->numUser > 0)
				{
					psTempBufInfo->numUser--;
					BDBG_MSG(("Decreasing usage count of Passthru codec Buffer [%d] allocted" 
						"for audio type %#x, Current usage count = %#x",
						i,eAudType,psTempBufInfo->numUser));
					break;
				}
				else
				{
					BDBG_ERR(("Usage count of Passthru codec Buffer [%d] is already 0."
						"This is an Error condition.",i));
					return BERR_TRACE(BERR_UNKNOWN);
				}
			}
		}	
	}
	else if(BRAP_DSP_ExecType_eProcessingAlgo== eExecType)
	{
            BRAP_FWDWNLD_P_IfOpenTimeProcDownload(&bOpenTimeAudProcDownload);
            if(true == bOpenTimeAudProcDownload)
            {/* If Execs Downloaded at opentime, No need to decrement the usage count */
                goto end; 
            }	    
		for(i = 0; i < BRAP_DSP_P_MAX_AUD_PROCESSING_CTX;i++)
		{
			psTempBufInfo = &(hRap->sDwnldMemInfo.sProcessingAlgoBufInfo[i]);
			if(psTempBufInfo->uExecAlgoType.ePAlgoType == (BRAP_ProcessingType)eAudType)
			{
				if(psTempBufInfo->numUser > 0)
				{
					psTempBufInfo->numUser--;
					BDBG_MSG(("Decreasing usage count of Audio Processing codec " 
						"Buffer [%d] allocted for audio type %#x, Current usage count = %#x",
						i,eAudType,psTempBufInfo->numUser));
					break;
				}
				else
				{
					BDBG_ERR(("Usage count of Audio Processing codec Buffer [%d] ."
						"is already 0. This is an Error condition.",i));
					return BERR_TRACE(BERR_UNKNOWN);
				}
			}
		}		
	}	
	else if(BRAP_DSP_ExecType_eEncode== eExecType)
	{   
		for(i = 0; i < BRAP_DSP_P_MAX_ENCODE_CTXT;i++)
		{
			psTempBufInfo = &(hRap->sDwnldMemInfo.sEncodeCodecBufInfo[i]);
			if(psTempBufInfo->uExecAlgoType.ePAlgoType == (BRAP_ProcessingType)eAudType)
			{
				if(psTempBufInfo->numUser > 0)
				{
					psTempBufInfo->numUser--;
					BDBG_MSG(("Decreasing usage count of Encode codec " 
						"Buffer [%d] allocted for audio type %#x, Current usage count = %#x",
						i,eAudType,psTempBufInfo->numUser));
					break;
				}
				else
				{
					BDBG_ERR(("Usage count of Encode codec Buffer [%d] ."
						"is already 0. This is an Error condition.",i));
					return BERR_TRACE(BERR_UNKNOWN);
				}
			}
		}		
	}	    
#ifdef RAP_VIDEOONDSP_SUPPORT
	else if(BRAP_DSP_ExecType_eVideoDecode == eExecType)
	{		
		for(i = 0; i < BRAP_DSP_P_MAX_VIDEODECODE_CTXT;i++)
		{
			psTempBufInfo = &(hRap->sDwnldMemInfo.sVideoDecCodecBufInfo[i]);
			if(psTempBufInfo->uExecAlgoType.eVideoAlgoType == (BRAP_DSPCHN_VideoType)eAudType)
			{
				if(psTempBufInfo->numUser > 0)
				{
					psTempBufInfo->numUser--;
					BDBG_MSG(("Decreasing usage count of Video Decode codec Buffer [%d] allocted" 
						"for audio type %#x, Current usage count = %#x",
						i,eAudType,psTempBufInfo->numUser));
					break;
				}
				else
				{
					BDBG_ERR(("Usage count of Video Decode codec Buffer [%d] is already 0."
						"This is an Error condition.",i));
					return BERR_TRACE(BERR_UNKNOWN);
				}
			}
		}
	}  
#endif	    
#ifdef 	RAP_GFX_SUPPORT
	else if(BRAP_DSP_ExecType_eGfx== eExecType)
	{   
		for(i = 0; i < BRAP_DSP_P_MAX_GFX_CTXT;i++)
		{
			psTempBufInfo = &(hRap->sDwnldMemInfo.sGfxBufInfo[i]);
			if(psTempBufInfo->uExecAlgoType.eGfxAlgoType == (BRAP_DSPCHN_GfxType)eAudType)
			{
				if(psTempBufInfo->numUser > 0)
				{
					psTempBufInfo->numUser--;
					BDBG_MSG(("Decreasing usage count of Gfx codec " 
						"Buffer [%d] allocted for audio type %#x, Current usage count = %#x",
						i,eAudType,psTempBufInfo->numUser));
					break;
				}
				else
				{
					BDBG_ERR(("Usage count of Gfx codec Buffer [%d] ."
						"is already 0. This is an Error condition.",i));
					return BERR_TRACE(BERR_UNKNOWN);
				}
			}
		}		
	}	        
#endif
#ifdef 	RAP_SCM_SUPPORT
	else if(BRAP_DSP_ExecType_eScm == eExecType)
	{	
		for(i = 0; i < BRAP_DSP_P_MAX_SCM_CTXT;i++)
		{
			psTempBufInfo = &(hRap->sDwnldMemInfo.sScmBufInfo[i]);
			if(psTempBufInfo->uExecAlgoType.eScmAlgoType == (BRAP_DSPCHN_ScmType)eAudType)
			{
				if(psTempBufInfo->numUser > 0)
				{
					psTempBufInfo->numUser--;
					BDBG_MSG(("Decreasing usage count of Scm codec " 
						"Buffer [%d] allocted for audio type %#x, Current usage count = %#x",
						i,eAudType,psTempBufInfo->numUser));
					break;
				}
				else
				{
					BDBG_ERR(("Usage count of Scm codec Buffer [%d] ."
						"is already 0. This is an Error condition.",i));
					return BERR_TRACE(BERR_UNKNOWN);
				}
			}
		}		
	}
#endif	

end:
	BDBG_LEAVE(BRAP_DSPCHN_P_FreeFwCtxtBuf);
	return BERR_SUCCESS;
}


BERR_Code BRAP_DSPCHN_P_AllocateFwCtxtBuf(
	BRAP_Handle hRap,
	uint32_t eAudType,	/* For PostProcessing it wiil be BRAP_ProcessingType. For Decode it will be audio type*/
	uint32_t *pui32FwCtxIndx,
	bool *bDownload,
	BRAP_DSP_ExecType eExecType,
	bool bWatchDogRecovery)
{
	unsigned int	i = 0;
	uint32_t	ui32FirstAvailIdx = RAP_INVALID_INDEX, ui32SecondAvailIdx = RAP_INVALID_INDEX;
	BRAP_DSP_P_CtxtFwBufInfo	*psTempBufInfo;
	*pui32FwCtxIndx = RAP_INVALID_INDEX;


	BDBG_ENTER(BRAP_DSPCHN_P_AllocateFwCtxtBuf);
	
	if(BRAP_DSP_ExecType_eFrameSync == eExecType)
	{
		for(i = 0; i < BRAP_DSP_P_MAX_FS_CTXT;i++)
		{
			psTempBufInfo = &(hRap->sDwnldMemInfo.sFsBufInfo[i]);
			if(psTempBufInfo->uExecAlgoType.eAlgoType == (BRAP_DSPCHN_AudioType)eAudType)
			{
				*pui32FwCtxIndx = i;
				*bDownload = false;
				
				if(false == bWatchDogRecovery)
				{
					psTempBufInfo->numUser++;
					psTempBufInfo->uExecAlgoType.eAlgoType =(BRAP_DSPCHN_AudioType)eAudType;							
				}
				break;
			}
			else if((ui32FirstAvailIdx ==RAP_INVALID_INDEX) 
				&& (psTempBufInfo->numUser == -1))
					ui32FirstAvailIdx = i;
			else if((ui32SecondAvailIdx ==RAP_INVALID_INDEX) 
				&& (psTempBufInfo->numUser == 0))
					ui32SecondAvailIdx = i;
		}

		BDBG_MSG(("pui32FwCtxIndx = %d, ui32FirstAvailIdx = %d, ui32SecondAvailIdx =%d , psTempBufInfo.numUser =%d",*pui32FwCtxIndx,ui32FirstAvailIdx,ui32SecondAvailIdx,psTempBufInfo->numUser));
		
		if(RAP_INVALID_INDEX != *pui32FwCtxIndx)
			goto exit;

		if(ui32FirstAvailIdx !=RAP_INVALID_INDEX)
		{
				*pui32FwCtxIndx = ui32FirstAvailIdx;
				*bDownload = true;
				if(false == bWatchDogRecovery)
				{
					hRap->sDwnldMemInfo.sFsBufInfo[*pui32FwCtxIndx].numUser = 1;
					hRap->sDwnldMemInfo.sFsBufInfo[*pui32FwCtxIndx].uExecAlgoType.eAlgoType = (BRAP_DSPCHN_AudioType)eAudType;					
				}
				goto exit;
		}
		else if(ui32SecondAvailIdx !=RAP_INVALID_INDEX)
		{
				*pui32FwCtxIndx = ui32SecondAvailIdx;
				*bDownload = true;
				if(false == bWatchDogRecovery)
				{
					hRap->sDwnldMemInfo.sFsBufInfo[*pui32FwCtxIndx].numUser = 1;
					hRap->sDwnldMemInfo.sFsBufInfo[*pui32FwCtxIndx].uExecAlgoType.eAlgoType = (BRAP_DSPCHN_AudioType)eAudType;					
				}
				goto exit;
		}
	}
	else if(BRAP_DSP_ExecType_eDecode == eExecType)
	{
		for(i = 0; i < BRAP_DSP_P_MAX_DECODE_CTXT;i++)
		{
			psTempBufInfo = &(hRap->sDwnldMemInfo.sDecCodecBufInfo[i]);
			if(psTempBufInfo->uExecAlgoType.eAlgoType == (BRAP_DSPCHN_AudioType)eAudType)
			{
				*pui32FwCtxIndx = i;
				*bDownload = false;
				
				if(false == bWatchDogRecovery)
				{
					psTempBufInfo->numUser++;
					psTempBufInfo->uExecAlgoType.eAlgoType = (BRAP_DSPCHN_AudioType)eAudType;		
				}
				break;
			}
			else if((ui32FirstAvailIdx ==RAP_INVALID_INDEX) 
				&& (psTempBufInfo->numUser == -1))
					ui32FirstAvailIdx = i;
			else if((ui32SecondAvailIdx ==RAP_INVALID_INDEX) 
				&& (psTempBufInfo->numUser == 0))
					ui32SecondAvailIdx = i;
		}
		BDBG_MSG(("LINE: - %d pui32FwCtxIndx = %d, ui32FirstAvailIdx = %d, ui32SecondAvailIdx =%d , psTempBufInfo.numUser =%d",__LINE__,*pui32FwCtxIndx,ui32FirstAvailIdx,ui32SecondAvailIdx,psTempBufInfo->numUser));
		if(RAP_INVALID_INDEX != *pui32FwCtxIndx)
			goto exit;
		if(ui32FirstAvailIdx !=RAP_INVALID_INDEX)
		{
				*pui32FwCtxIndx = ui32FirstAvailIdx;
				*bDownload = true;
				if(false == bWatchDogRecovery)
				{
					hRap->sDwnldMemInfo.sDecCodecBufInfo[*pui32FwCtxIndx].numUser = 1;
					hRap->sDwnldMemInfo.sDecCodecBufInfo[*pui32FwCtxIndx].uExecAlgoType.eAlgoType = (BRAP_DSPCHN_AudioType)eAudType;
				}
				goto exit;
		}
		else if(ui32SecondAvailIdx !=RAP_INVALID_INDEX)
		{
				*pui32FwCtxIndx = ui32SecondAvailIdx;
				*bDownload = true;
				if(false == bWatchDogRecovery)
				{
					hRap->sDwnldMemInfo.sDecCodecBufInfo[*pui32FwCtxIndx].numUser = 1;
					hRap->sDwnldMemInfo.sDecCodecBufInfo[*pui32FwCtxIndx].uExecAlgoType.eAlgoType = (BRAP_DSPCHN_AudioType)eAudType;
				}
				goto exit;
		}
	}
	else if(BRAP_DSP_ExecType_ePassthru == eExecType)
	{
		for(i = 0; i < BRAP_DSP_P_MAX_PASSTHRU_CTXT;i++)
		{
			psTempBufInfo = &(hRap->sDwnldMemInfo.sPtCodecBufInfo[i]);
			if(psTempBufInfo->uExecAlgoType.ePAlgoType == (BRAP_ProcessingType)eAudType)
			{
				*pui32FwCtxIndx = i;
				*bDownload = false;
				
				if(false == bWatchDogRecovery)
				{
					psTempBufInfo->numUser++;
					psTempBufInfo->uExecAlgoType.ePAlgoType = (BRAP_ProcessingType)eAudType;		
				}
				break;
			}
			else if((ui32FirstAvailIdx ==RAP_INVALID_INDEX) 
				&& (psTempBufInfo->numUser == -1))
					ui32FirstAvailIdx = i;
			else if((ui32SecondAvailIdx ==RAP_INVALID_INDEX) 
				&& (psTempBufInfo->numUser == 0))
					ui32SecondAvailIdx = i;
		}
		BDBG_MSG(("LINE: - %d pui32FwCtxIndx = %d, ui32FirstAvailIdx = %d, ui32SecondAvailIdx =%d , psTempBufInfo.numUser =%d",__LINE__,*pui32FwCtxIndx,ui32FirstAvailIdx,ui32SecondAvailIdx,psTempBufInfo->numUser));
		if(RAP_INVALID_INDEX != *pui32FwCtxIndx)
			goto exit;

		if(ui32FirstAvailIdx !=RAP_INVALID_INDEX)
		{
				*pui32FwCtxIndx = ui32FirstAvailIdx;
				*bDownload = true;
				if(false == bWatchDogRecovery)
				{
					hRap->sDwnldMemInfo.sPtCodecBufInfo[*pui32FwCtxIndx].numUser = 1;
					hRap->sDwnldMemInfo.sPtCodecBufInfo[*pui32FwCtxIndx].uExecAlgoType.ePAlgoType = (BRAP_ProcessingType)eAudType;					
				}
				goto exit;
		}
		else if(ui32SecondAvailIdx !=RAP_INVALID_INDEX)
		{
				*pui32FwCtxIndx = ui32SecondAvailIdx;
				*bDownload = true;
				if(false == bWatchDogRecovery)
				{
					hRap->sDwnldMemInfo.sPtCodecBufInfo[*pui32FwCtxIndx].numUser = 1;
					hRap->sDwnldMemInfo.sPtCodecBufInfo[*pui32FwCtxIndx].uExecAlgoType.ePAlgoType =(BRAP_ProcessingType)eAudType;					
				}
				goto exit;
		}

	}
	else if(BRAP_DSP_ExecType_eProcessingAlgo == eExecType)
	{	
		for(i = 0; i < BRAP_DSP_P_MAX_AUD_PROCESSING_CTX;i++)
		{
			psTempBufInfo = &(hRap->sDwnldMemInfo.sProcessingAlgoBufInfo[i]);
			if(psTempBufInfo->uExecAlgoType.ePAlgoType == (BRAP_ProcessingType)eAudType)
			{
				*pui32FwCtxIndx = i;
				*bDownload = false;
				
				if(false == bWatchDogRecovery)
				{
					psTempBufInfo->numUser++;
					psTempBufInfo->uExecAlgoType.ePAlgoType = (BRAP_ProcessingType)eAudType;		
				}
				break;
			}
			else if((ui32FirstAvailIdx ==RAP_INVALID_INDEX) 
				&& (psTempBufInfo->numUser == -1))
					ui32FirstAvailIdx = i;
			else if((ui32SecondAvailIdx ==RAP_INVALID_INDEX) 
				&& (psTempBufInfo->numUser == 0))
					ui32SecondAvailIdx = i;
		}
		BDBG_MSG(("LINE: - %d pui32FwCtxIndx = %d, ui32FirstAvailIdx = %d, ui32SecondAvailIdx =%d , psTempBufInfo.numUser =%d",__LINE__,*pui32FwCtxIndx,ui32FirstAvailIdx,ui32SecondAvailIdx,psTempBufInfo->numUser));
		if(RAP_INVALID_INDEX != *pui32FwCtxIndx)
			goto exit;
		if(ui32FirstAvailIdx !=RAP_INVALID_INDEX)
		{
				*pui32FwCtxIndx = ui32FirstAvailIdx;
				*bDownload = true;
				if(false == bWatchDogRecovery)
				{
					hRap->sDwnldMemInfo.sProcessingAlgoBufInfo[*pui32FwCtxIndx].numUser = 1;
					hRap->sDwnldMemInfo.sProcessingAlgoBufInfo[*pui32FwCtxIndx].uExecAlgoType.ePAlgoType = (BRAP_ProcessingType)eAudType;	
				}
				goto exit;
		}
		else if(ui32SecondAvailIdx !=RAP_INVALID_INDEX)
		{
				*pui32FwCtxIndx = ui32SecondAvailIdx;
				*bDownload = true;
				if(false == bWatchDogRecovery)
				{
					hRap->sDwnldMemInfo.sProcessingAlgoBufInfo[*pui32FwCtxIndx].numUser = 1;
					hRap->sDwnldMemInfo.sProcessingAlgoBufInfo[*pui32FwCtxIndx].uExecAlgoType.ePAlgoType = (BRAP_ProcessingType)eAudType;	
				}
				goto exit;
		}
	}
	else if(BRAP_DSP_ExecType_eEncode== eExecType)
	{	
		for(i = 0; i < BRAP_DSP_P_MAX_ENCODE_CTXT;i++)
		{
			psTempBufInfo = &(hRap->sDwnldMemInfo.sEncodeCodecBufInfo[i]);
			if(psTempBufInfo->uExecAlgoType.ePAlgoType == (BRAP_ProcessingType)eAudType)
			{
				*pui32FwCtxIndx = i;
				*bDownload = false;
				
				if(false == bWatchDogRecovery)
				{
					psTempBufInfo->numUser++;
					psTempBufInfo->uExecAlgoType.ePAlgoType = (BRAP_ProcessingType)eAudType;		
				}
				break;
			}
			else if((ui32FirstAvailIdx ==RAP_INVALID_INDEX) 
				&& (psTempBufInfo->numUser == -1))
					ui32FirstAvailIdx = i;
			else if((ui32SecondAvailIdx ==RAP_INVALID_INDEX) 
				&& (psTempBufInfo->numUser == 0))
					ui32SecondAvailIdx = i;
		}
		BDBG_MSG(("LINE: - %d pui32FwCtxIndx = %d, ui32FirstAvailIdx = %d, ui32SecondAvailIdx =%d , psTempBufInfo.numUser =%d",__LINE__,*pui32FwCtxIndx,ui32FirstAvailIdx,ui32SecondAvailIdx,psTempBufInfo->numUser));
		if(RAP_INVALID_INDEX != *pui32FwCtxIndx)
			goto exit;

		if(ui32FirstAvailIdx !=RAP_INVALID_INDEX)
		{
				*pui32FwCtxIndx = ui32FirstAvailIdx;
				*bDownload = true;
				if(false == bWatchDogRecovery)
				{
					hRap->sDwnldMemInfo.sEncodeCodecBufInfo[*pui32FwCtxIndx].numUser = 1;
					hRap->sDwnldMemInfo.sEncodeCodecBufInfo[*pui32FwCtxIndx].uExecAlgoType.ePAlgoType = (BRAP_ProcessingType)eAudType;	
				}
				goto exit;
		}
		else if(ui32SecondAvailIdx !=RAP_INVALID_INDEX)
		{
				*pui32FwCtxIndx = ui32SecondAvailIdx;
				*bDownload = true;
				if(false == bWatchDogRecovery)
				{
					hRap->sDwnldMemInfo.sEncodeCodecBufInfo[*pui32FwCtxIndx].numUser = 1;
					hRap->sDwnldMemInfo.sEncodeCodecBufInfo[*pui32FwCtxIndx].uExecAlgoType.ePAlgoType = (BRAP_ProcessingType)eAudType;	
				}
				goto exit;
		}
	}
#ifdef RAP_VIDEOONDSP_SUPPORT
	else if(BRAP_DSP_ExecType_eVideoDecode == eExecType)
	{
		for(i = 0; i < BRAP_DSP_P_MAX_VIDEODECODE_CTXT;i++)
		{
			psTempBufInfo = &(hRap->sDwnldMemInfo.sVideoDecCodecBufInfo[i]);
			if(psTempBufInfo->uExecAlgoType.eVideoAlgoType == (BRAP_DSPCHN_VideoType)eAudType)
			{
				*pui32FwCtxIndx = i;
				*bDownload = false;
				
				if(false == bWatchDogRecovery)
				{
					psTempBufInfo->numUser++;
					psTempBufInfo->uExecAlgoType.eVideoAlgoType = (BRAP_DSPCHN_VideoType)eAudType;		
				}
				break;
			}
			else if((ui32FirstAvailIdx ==RAP_INVALID_INDEX) 
				&& (psTempBufInfo->numUser == -1))
					ui32FirstAvailIdx = i;
			else if((ui32SecondAvailIdx ==RAP_INVALID_INDEX) 
				&& (psTempBufInfo->numUser == 0))
					ui32SecondAvailIdx = i;
		}
		BDBG_MSG(("LINE: - %d pui32FwCtxIndx = %d, ui32FirstAvailIdx = %d, ui32SecondAvailIdx =%d , psTempBufInfo.numUser =%d",__LINE__,*pui32FwCtxIndx,ui32FirstAvailIdx,ui32SecondAvailIdx,psTempBufInfo->numUser));
		if(RAP_INVALID_INDEX != *pui32FwCtxIndx)
			goto exit;
		if(ui32FirstAvailIdx !=RAP_INVALID_INDEX)
		{
				*pui32FwCtxIndx = ui32FirstAvailIdx;
				*bDownload = true;
				if(false == bWatchDogRecovery)
				{
					hRap->sDwnldMemInfo.sVideoDecCodecBufInfo[*pui32FwCtxIndx].numUser = 1;
					hRap->sDwnldMemInfo.sVideoDecCodecBufInfo[*pui32FwCtxIndx].uExecAlgoType.eVideoAlgoType = (BRAP_DSPCHN_VideoType)eAudType;
				}
				goto exit;
		}
		else if(ui32SecondAvailIdx !=RAP_INVALID_INDEX)
		{
				*pui32FwCtxIndx = ui32SecondAvailIdx;
				*bDownload = true;
				if(false == bWatchDogRecovery)
				{
					hRap->sDwnldMemInfo.sVideoDecCodecBufInfo[*pui32FwCtxIndx].numUser = 1;
					hRap->sDwnldMemInfo.sVideoDecCodecBufInfo[*pui32FwCtxIndx].uExecAlgoType.eVideoAlgoType = (BRAP_DSPCHN_VideoType)eAudType;
				}
				goto exit;
		}
	}
#endif
#ifdef 	RAP_GFX_SUPPORT
	else if(BRAP_DSP_ExecType_eGfx== eExecType)
	{
		for(i = 0; i < BRAP_DSP_P_MAX_GFX_CTXT;i++)
		{
			psTempBufInfo = &(hRap->sDwnldMemInfo.sGfxBufInfo[i]);
			if(psTempBufInfo->uExecAlgoType.eGfxAlgoType == (BRAP_DSPCHN_GfxType)eAudType)
			{
				*pui32FwCtxIndx = i;
				*bDownload = false;
				
				if(false == bWatchDogRecovery)
				{
					psTempBufInfo->numUser++;
					psTempBufInfo->uExecAlgoType.eGfxAlgoType = (BRAP_DSPCHN_GfxType)eAudType;		
				}
				break;
			}
			else if((ui32FirstAvailIdx ==RAP_INVALID_INDEX) 
				&& (psTempBufInfo->numUser == -1))
					ui32FirstAvailIdx = i;
			else if((ui32SecondAvailIdx ==RAP_INVALID_INDEX) 
				&& (psTempBufInfo->numUser == 0))
					ui32SecondAvailIdx = i;
		}
		BDBG_MSG(("LINE: - %d pui32FwCtxIndx = %d, ui32FirstAvailIdx = %d, ui32SecondAvailIdx =%d , psTempBufInfo.numUser =%d",__LINE__,*pui32FwCtxIndx,ui32FirstAvailIdx,ui32SecondAvailIdx,psTempBufInfo->numUser));
		if(RAP_INVALID_INDEX != *pui32FwCtxIndx)
			goto exit;
		if(ui32FirstAvailIdx !=RAP_INVALID_INDEX)
		{
				*pui32FwCtxIndx = ui32FirstAvailIdx;
				*bDownload = true;
				if(false == bWatchDogRecovery)
				{
					hRap->sDwnldMemInfo.sGfxBufInfo[*pui32FwCtxIndx].numUser = 1;
					hRap->sDwnldMemInfo.sGfxBufInfo[*pui32FwCtxIndx].uExecAlgoType.eGfxAlgoType = (BRAP_DSPCHN_GfxType)eAudType;
				}
				goto exit;
		}
		else if(ui32SecondAvailIdx !=RAP_INVALID_INDEX)
		{
				*pui32FwCtxIndx = ui32SecondAvailIdx;
				*bDownload = true;
				if(false == bWatchDogRecovery)
				{
					hRap->sDwnldMemInfo.sGfxBufInfo[*pui32FwCtxIndx].numUser = 1;
					hRap->sDwnldMemInfo.sGfxBufInfo[*pui32FwCtxIndx].uExecAlgoType.eGfxAlgoType = (BRAP_DSPCHN_GfxType)eAudType;
				}
				goto exit;
		}
	}    
#endif
#ifdef 	RAP_SCM_SUPPORT
	else if(BRAP_DSP_ExecType_eScm == eExecType)
	{
		for(i = 0; i < BRAP_DSP_P_MAX_SCM_CTXT;i++)
		{
			psTempBufInfo = &(hRap->sDwnldMemInfo.sScmBufInfo[i]);
			if(psTempBufInfo->uExecAlgoType.eScmAlgoType == (BRAP_DSPCHN_ScmType)eAudType)
			{
				*pui32FwCtxIndx = i;
				*bDownload = false;
				
				if(false == bWatchDogRecovery)
				{
					psTempBufInfo->numUser++;
					psTempBufInfo->uExecAlgoType.eScmAlgoType = (BRAP_DSPCHN_ScmType)eAudType;		
				}
				break;
			}
			else if((ui32FirstAvailIdx ==RAP_INVALID_INDEX) 
				&& (psTempBufInfo->numUser == -1))
					ui32FirstAvailIdx = i;
			else if((ui32SecondAvailIdx ==RAP_INVALID_INDEX) 
				&& (psTempBufInfo->numUser == 0))
					ui32SecondAvailIdx = i;
		}
		BDBG_MSG(("LINE: - %d pui32FwCtxIndx = %d, ui32FirstAvailIdx = %d, ui32SecondAvailIdx =%d , psTempBufInfo.numUser =%d",__LINE__,*pui32FwCtxIndx,ui32FirstAvailIdx,ui32SecondAvailIdx,psTempBufInfo->numUser));
		if(RAP_INVALID_INDEX != *pui32FwCtxIndx)
			goto exit;
		if(ui32FirstAvailIdx !=RAP_INVALID_INDEX)
		{
				*pui32FwCtxIndx = ui32FirstAvailIdx;
				*bDownload = true;
				if(false == bWatchDogRecovery)
				{
					hRap->sDwnldMemInfo.sScmBufInfo[*pui32FwCtxIndx].numUser = 1;
					hRap->sDwnldMemInfo.sScmBufInfo[*pui32FwCtxIndx].uExecAlgoType.eScmAlgoType = (BRAP_DSPCHN_ScmType)eAudType;
				}
				goto exit;
		}
		else if(ui32SecondAvailIdx !=RAP_INVALID_INDEX)
		{
				*pui32FwCtxIndx = ui32SecondAvailIdx;
				*bDownload = true;
				if(false == bWatchDogRecovery)
				{
					hRap->sDwnldMemInfo.sScmBufInfo[*pui32FwCtxIndx].numUser = 1;
					hRap->sDwnldMemInfo.sScmBufInfo[*pui32FwCtxIndx].uExecAlgoType.eScmAlgoType = (BRAP_DSPCHN_ScmType)eAudType;
				}
				goto exit;
		}
	}	 
#endif	

exit:
	
	BDBG_LEAVE(BRAP_DSPCHN_P_AllocateFwCtxtBuf);
	return BERR_SUCCESS;
}

BERR_Code BRAP_DSPCHN_P_DownloadStartTimeFWExec(
				BRAP_DSPCHN_Handle hDspCh, 
				const BRAP_DSPCHN_P_AudioParams *psAudioParams)
{
	BRAP_Handle	hRap = NULL;
	BRAP_DSPCHN_DecodeMode eDecMode = BRAP_DSPCHN_DecodeMode_eInvalid; 
	BRAP_DSPCHN_AudioType eAudType = BRAP_DSPCHN_AudioType_eMax;
	BRAP_Img_Id eCodeFirmwareId = BRAP_Img_Id_eInvalid;
	BRAP_Img_Id eTblFirmwareId = BRAP_Img_Id_eInvalid;
	BRAP_AF_P_AlgoId eTempExecId = BRAP_AF_P_AlgoId_eMax;
    	BRAP_AF_P_AlgoId eCodeExecId = BRAP_AF_P_AlgoId_eMax;
	bool	bDownload = false;
	uint32_t	ui32FwCtxIndx = RAP_INVALID_INDEX;
	uint32_t ui32CodePtr = 0,ui32CodeSize = 0,ui32TblPtr = 0,ui32TblSize = 0,ui32physAddress;
	uint32_t ui32SizeAvail = 0,i = 0,j = 0, k = 0, l = 0;
	const BIMG_Interface *iface = NULL;	
	void *pImgContext = NULL;
	bool	bOpenTimeAudProcDownload = true;

	bool bDownloaded= false, bWatchDogRecovery = false;	
 
	BRAP_DSP_P_CtxtFwBufInfo	sBufInfo;
	BRAP_AF_P_sALGO_EXEC_INFO sExecID;
    

	BRAP_ProcessingType eAudioProcessing = BRAP_ProcessingType_eNone;
	
	BDBG_ENTER(BRAP_DSPCHN_P_DownloadStartTimeFWExec);

	BDBG_ASSERT(hDspCh);
    
    if(true == hDspCh->bChSpecificDspCh)
    {
        hRap = hDspCh->uHandle.hRapCh->hRap;
    }
    else
    {
        hRap = hDspCh->uHandle.hAssociation->hRap;
    }
	BDBG_ASSERT(hRap->sSettings.pImgInterface);
	BDBG_ASSERT(hRap->sSettings.pImgContext);

	iface = hRap->sSettings.pImgInterface;	
	pImgContext = hRap->sSettings.pImgContext;
	eDecMode = psAudioParams->sExtAudioParams.eDecodeMode;
	eAudType = psAudioParams->sExtAudioParams.eType;

    if(!((BRAP_P_IsPassthruSupportedWithoutLicense(eAudType))&&
        (BRAP_DSPCHN_DecodeMode_ePassThru == eDecMode)))
    {
    	if(!BRAP_FWDWNLD_P_IsAudCodecSupported((uint32_t)eAudType))
    	{
    		BDBG_ERR(("Audio type %#x is not supported for this platform",eAudType));
    		return BERR_TRACE(BERR_NOT_SUPPORTED);
    	}
    }

	/* Download PostProcessing Algorithms if not downloaded at init time. */
	BRAP_FWDWNLD_P_IfOpenTimeProcDownload(&bOpenTimeAudProcDownload);
	
	bWatchDogRecovery = BRAP_P_GetWatchdogRecoveryFlag(hRap);
	
	BRAP_FWDWNLD_P_GetExecIds(eDecMode,eAudType,&sExecID);

	/* Download FS Exec */
	{
		/* BRAP_DSPCHN_P_GetFSImgId(psAudioParams,sExecID,&eCodeFirmwareId); */
        eCodeExecId = sExecID.eAlgoIds[0];
        if(eCodeExecId != BRAP_AF_P_AlgoId_eInvalid)
        {
            eCodeFirmwareId = hRap->sImgIdMappingArrays.ui32CodeImgId[eCodeExecId];
            if(eCodeFirmwareId != BRAP_Img_Id_eInvalid)
            {        
        		BRAP_DSPCHN_P_AllocateFwCtxtBuf(hRap, eAudType, &ui32FwCtxIndx,&bDownload,BRAP_DSP_ExecType_eFrameSync,bWatchDogRecovery);

        		/* If its Watchdog recovery, And its exec is not already in the DRAM, 
        		then this is a BAS DEVICE STATE */

        		if ((true == bWatchDogRecovery)
        			&& (true == bDownload))
        		{
        			BDBG_ERR(("In Watchdog recovery FS exec is not already in the DRAM"
        					" Corruption of DRAM "));
        			return BERR_TRACE( BRAP_ERR_BAD_DEVICE_STATE );
        		}

        		/* If no Free Buf to download FS */
        		if((RAP_INVALID_INDEX == ui32FwCtxIndx)
        			|| ui32FwCtxIndx >= BRAP_DSP_P_MAX_FS_CTXT)
        		{
        			BDBG_ERR(("ERROR!!! -- All FS context buffers are in use. ui32FwCtxIndx =%d",ui32FwCtxIndx));
        			return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
        		}

        		sBufInfo = hRap->sDwnldMemInfo.sFsBufInfo[ui32FwCtxIndx];
        		ui32CodePtr = sBufInfo.ui32BaseAddr;
        			
    			ui32CodeSize = hRap->sImgIdMappingArrays.ui32CodeSize[eCodeExecId];
    			sBufInfo = hRap->sDwnldMemInfo.sFsBufInfo[ui32FwCtxIndx];
    			
    			if(ui32CodeSize> sBufInfo.ui32Size)
    			{
    				BDBG_ERR(("Size available (%d) to download Framesync IMG ID (%#x) of"
    				"Audio Algorithm (%#x) is less than required (%d)",
    				sBufInfo.ui32Size,eCodeFirmwareId,eAudType,
    				ui32CodeSize));
    				return	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
    			}

                if(!((false == bDownload)
                    && (false == bWatchDogRecovery)))
                {
                    BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(ui32CodePtr), &ui32physAddress);	

                    BDBG_MSG(("Downloading Framesync (IMG ID = %#x) of Audio Algorithm (%#x) at address %#x (Physical = %#x), size = 0x%x",
                    eCodeFirmwareId,eAudType,ui32CodePtr,ui32physAddress,ui32CodeSize));

                    BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32CodePtr,eCodeFirmwareId,hRap->hHeap);	
                }
    			
    			BRAP_UpdateMitEntry(hRap,eCodeExecId,ui32CodePtr,ui32CodeSize,0,0);
        	}
        }
    }

	ui32FwCtxIndx = RAP_INVALID_INDEX;
	ui32CodePtr = 0;
	ui32CodeSize = 0;
	
	/* Download Decode Code+Table, Encode Code+Table (added as Processing stage) */
	{
		if((BRAP_DSPCHN_DecodeMode_eDecode == eDecMode))
		{
			BRAP_DSPCHN_P_AllocateFwCtxtBuf(hRap, eAudType, &ui32FwCtxIndx,&bDownload,BRAP_DSP_ExecType_eDecode,bWatchDogRecovery);

			/* If its Watchdog recovery, And its exec is not already in the DRAM, 
			then this is a BAS DEVICE STATE */

			if ((BRAP_P_GetWatchdogRecoveryFlag(hRap))
				&& (true == bDownload))
			{
				BDBG_ERR(("In Watchdog recovery Decode exec is not already in the DRAM"
						" Corruption of DRAM "));
				return BERR_TRACE( BRAP_ERR_BAD_DEVICE_STATE );
			}

			/* If no Free Buf to download algo code */
			if((RAP_INVALID_INDEX == ui32FwCtxIndx)
				|| ui32FwCtxIndx >= BRAP_DSP_P_MAX_DECODE_CTXT)
			{
				BDBG_ERR(("ERROR!!! -- All decode context buffers are in use."));
				return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
			}

			sBufInfo = hRap->sDwnldMemInfo.sDecCodecBufInfo[ui32FwCtxIndx];
			ui32CodePtr = sBufInfo.ui32BaseAddr;
			ui32SizeAvail = sBufInfo.ui32Size;
			
			/*Download all decode Code+Table for supported algorithm*/	
    		for(i = 2;i<sExecID.NumNodes;i++)
			{
    			/*Download decode Code */
        		eCodeExecId = sExecID.eAlgoIds[i];
                if(eCodeExecId == BRAP_AF_P_AlgoId_eInvalid)
                {
                    continue;
                }                   
        		eCodeFirmwareId = hRap->sImgIdMappingArrays.ui32CodeImgId[eCodeExecId];

				if(BRAP_Img_Id_eInvalid == eCodeFirmwareId)
				{
					BDBG_MSG(("Invalid Image ID for decode code of algorithm %#x Stage %d",eAudType,i));
					continue;
				}

				ui32CodeSize = hRap->sImgIdMappingArrays.ui32CodeSize[eCodeExecId];

				if(ui32CodeSize > ui32SizeAvail)
				{
					BDBG_ERR(("Size available (%d) to download decode code (IMG ID = %#x) of Audio Algorithm (%#x)"
							" is less than required (%d)",	
							ui32SizeAvail,eCodeFirmwareId,eAudType,ui32CodeSize
							));
					return	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
				}

                /* Code is already downloaded, just update the MIT entries */
    			if(!((false == bDownload)
    				&& (false == bWatchDogRecovery)))
    			{                    
    				BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(ui32CodePtr), &ui32physAddress);	

    				BDBG_MSG(("Downloading decode code (IMG ID = %#x) of Audio Algorithm (%#x) at address %#x (Physical = %#x), size = 0x%x",
    								eCodeFirmwareId,eAudType,ui32CodePtr,ui32physAddress,ui32CodeSize));
    				BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32CodePtr,eCodeFirmwareId,hRap->hHeap);	
    		    }
            
				ui32SizeAvail-= ui32CodeSize;

				ui32TblPtr = ui32CodePtr + ui32CodeSize;

				/*Download decode table */
            			eTblFirmwareId = hRap->sImgIdMappingArrays.ui32TableImgId[eCodeExecId];

				if(BRAP_Img_Id_eInvalid == eTblFirmwareId)
				{
					BRAP_UpdateMitEntry(hRap,eCodeExecId,ui32CodePtr,ui32CodeSize,0,0);	
            				ui32CodePtr += ui32CodeSize;	                                      
					BDBG_MSG(("Invalid Image ID for decode table of algorithm %#x Stage %d",eAudType,i));
					continue;
				}
				
        		bDownloaded = false;
        		BRAP_FWDWNLD_P_IsTableDownloaded(BRAP_DSP_ExecType_eDecode,eCodeExecId,&bDownloaded,&eTempExecId);
				
				if(!bDownloaded)
				{
					ui32TblSize = hRap->sImgIdMappingArrays.ui32TableSize[eCodeExecId];

					if( ui32TblSize > ui32SizeAvail)
					{
						BDBG_ERR(("Size available (%d) to download decode table (IMG ID = %#x) of Audio Algorithm (%#x)"
								"is less than required (%d)",
								ui32SizeAvail,eTblFirmwareId,eAudType,ui32TblSize
								));
						return	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
					}

                    /* Table is already downloaded, just update the MIT entries */
        			if(!((false == bDownload)
        				&& (false == bWatchDogRecovery)))
        			{          
						BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(ui32TblPtr), &ui32physAddress);	

						BDBG_MSG(("Downloading decode table (IMG ID = %#x) of Audio Algorithm (%#x) at address %#x (Physical = %#x), size = 0x%x",
										eTblFirmwareId,eAudType,ui32TblPtr,ui32physAddress,ui32TblSize));
						BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32TblPtr,eTblFirmwareId,hRap->hHeap);	
                    }       
					/* updating ui32CodePtr variable, used to track the used size */ 		
					ui32SizeAvail-=ui32TblSize;
				}
				else
				{
					ui32TblPtr = hRap->sMit.sAlgoDwnldDetail[eTempExecId].ui32AlgoDataPtr;
					ui32TblSize= hRap->sMit.sAlgoDwnldDetail[eTempExecId].ui32AlgoDataSize;
					BDBG_MSG(("decode Table (IMG ID = %#x) of Audio Algorithm (%#x) is already downloaded at address 0x%x, size = 0x%x",
									eTblFirmwareId,eAudType,ui32TblPtr,ui32TblSize));
				}
				BRAP_UpdateMitEntry(hRap,eCodeExecId,ui32CodePtr,ui32CodeSize,ui32TblPtr,ui32TblSize);	
				if(false == bDownloaded)
				{
					ui32CodePtr += ui32CodeSize + ui32TblSize;
				}
				else
				{
					ui32CodePtr += ui32CodeSize;				
				}
        }/*for*/



			ui32FwCtxIndx = RAP_INVALID_INDEX;
			ui32CodePtr = 0;
			ui32CodeSize = 0;
			ui32SizeAvail = 0;
			bDownload = 0;

			/* Download Encode/passthru Code + Table added as Processing stage*/
                    for(i=0;i<BRAP_FWIF_P_MAX_FW_TASK_PER_DSPCHN;i++)
			{
                        for(j=0;j<BRAP_P_MAX_DST_PER_RAPCH;j++)
                        {
                                for(k=0;k<BRAP_MAX_STAGE_PER_BRANCH_SUPPORTED;k++)
                                {
					ui32FwCtxIndx = RAP_INVALID_INDEX;
					ui32CodePtr = 0;
					ui32CodeSize = 0;
					ui32SizeAvail = 0;
					bDownload = 0;
                    /*TODO: Take hAudioProcessingStageHandle from following as
					hDspCh->sFwTaskInfo.sProcessingNw.sAudProcessingStage.hAudioProcessing */
					if(!(BRAP_P_IsPointerValid((void *)hDspCh->sFwTaskInfo[i].sProcessingNw.sAudProcessingStage[j][k].hAudioProcessing)))
					{
						continue;
					}
					eAudioProcessing = hDspCh->sFwTaskInfo[i].sProcessingNw.sAudProcessingStage[j][k].hAudioProcessing->sProcessingStageSettings.eAudioProcessing;
					
					if((BRAP_ProcessingType_ePassthru == eAudioProcessing))
					{
						BRAP_DSPCHN_P_AllocateFwCtxtBuf(hRap, BRAP_ProcessingType_ePassthru , &ui32FwCtxIndx,&bDownload,BRAP_DSP_ExecType_ePassthru,bWatchDogRecovery);

						/* If its Watchdog recovery, And its exec is not already in the DRAM, 
						then this is a BAD DEVICE STATE */

						if ((BRAP_P_GetWatchdogRecoveryFlag(hRap))
							&& (true == bDownload))
						{
							BDBG_ERR(("In Watchdog recovery Passthru exec is not already in the DRAM"
									" Corruption of DRAM "));
							return BERR_TRACE( BRAP_ERR_BAD_DEVICE_STATE );
						}

						/* If no Free Buf to download passthru exec */
						if((RAP_INVALID_INDEX == ui32FwCtxIndx)
							|| ui32FwCtxIndx >= BRAP_DSP_P_MAX_PASSTHRU_CTXT)
						{
							BDBG_ERR(("ERROR!!! -- All Passthru context buffers are in use."));
							return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
						}

						sBufInfo = hRap->sDwnldMemInfo.sPtCodecBufInfo[ui32FwCtxIndx];
						ui32CodePtr = sBufInfo.ui32BaseAddr;
						ui32SizeAvail = sBufInfo.ui32Size;
						

						{
							/*Download passthru Code */
							eCodeFirmwareId = BRAP_Img_Id_ePassthruCode;

								ui32CodeSize = hRap->sImgIdMappingArrays.ui32CodeSize[BRAP_AF_P_AlgoId_ePassThru];

								if(ui32CodeSize > ui32SizeAvail)
								{
									BDBG_ERR(("Size available (%d) to download passthru code (IMG ID = %#x) of Audio Algorithm (%#x)"
											"is less than required (%d)",	
											ui32SizeAvail,eCodeFirmwareId,eAudType,ui32CodeSize
											));
									return	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
								}
                						if(!((false == bDownload)
                							&& (false == bWatchDogRecovery)))                                
                                                        {                  
								BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(ui32CodePtr), &ui32physAddress);	
								BDBG_MSG(("Downloading passthru code (IMG ID = %#x) of Audio Algorithm (%#x) at address %#x (Physical = %#x), size = 0x%x",
												eCodeFirmwareId,eAudType,ui32CodePtr,ui32physAddress,ui32CodeSize));
								BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32CodePtr,eCodeFirmwareId,hRap->hHeap);	
                                                      }

								ui32SizeAvail-= ui32CodeSize;

								ui32TblPtr = ui32CodePtr + ui32CodeSize;

								/*Download passthru table */
							eTblFirmwareId = BRAP_Img_Id_ePassthruTable;

        							ui32TblSize = hRap->sImgIdMappingArrays.ui32TableSize[BRAP_AF_P_AlgoId_ePassThru];

									if( ui32TblSize > ui32SizeAvail)
									{
										BDBG_ERR(("Size available (%d) to download passthru table (IMG ID = %#x) of Audio Algorithm (%#x)"
												"(%#x) is less than required (%d)",
												ui32SizeAvail,eTblFirmwareId,eAudType,ui32TblSize
												));
										return	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
									}
                                                            if(!((false == bDownload)
                                                                && (false == bWatchDogRecovery)))
                                                            {
									BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(ui32TblPtr), &ui32physAddress);	

									BDBG_MSG(("Downloading passthru Table (IMG ID = %#x) of Audio Algorithm (%#x) at address %#x (Physical = %#x) size = 0x%x",
													eTblFirmwareId,eAudType,ui32TblPtr,ui32physAddress,ui32TblSize));
									BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32TblPtr,eTblFirmwareId,hRap->hHeap);	
                                                            }
									/* updating ui32CodePtr variable, used to track the used size */ 		
									ui32SizeAvail-=ui32TblSize;
                						BRAP_UpdateMitEntry(hRap,BRAP_AF_P_AlgoId_ePassThru,ui32CodePtr,ui32CodeSize,ui32TblPtr,ui32TblSize);	

						}/*if-else*/
					}
					else if((BRAP_ProcessingType_eEncodeDts == eAudioProcessing)
					|| (BRAP_ProcessingType_eEncodeAc3== eAudioProcessing)
					|| (BRAP_ProcessingType_eEncodeMp3== eAudioProcessing))
					{
					       
						BRAP_FWDWNLD_P_GetEncodeExecIds(BRAP_sMapProcessingEnum[eAudioProcessing].uEnumName.eEncodeType,&sExecID);
						
						BRAP_DSPCHN_P_AllocateFwCtxtBuf(hRap, (uint32_t)eAudioProcessing, &ui32FwCtxIndx,&bDownload,BRAP_DSP_ExecType_eEncode,bWatchDogRecovery);

						/* If its Watchdog recovery, And its exec is not already in the DRAM, 
						then this is a BAS DEVICE STATE */

						if ((BRAP_P_GetWatchdogRecoveryFlag(hRap))
							&& (true == bDownload))
						{
							BDBG_ERR(("In Watchdog recovery Encode exec is not already in the DRAM"
									" Corruption of DRAM "));
							return BERR_TRACE( BRAP_ERR_BAD_DEVICE_STATE );
						}

						/* If no Free Buf to download Processing algo */
						if((RAP_INVALID_INDEX == ui32FwCtxIndx)
							|| ui32FwCtxIndx >= BRAP_DSP_P_MAX_ENCODE_CTXT)
						{
							BDBG_ERR(("ERROR!!! -- All Encode context buffers are in use."));
							return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
						}

						sBufInfo = hRap->sDwnldMemInfo.sEncodeCodecBufInfo[ui32FwCtxIndx];
						ui32CodePtr = sBufInfo.ui32BaseAddr;
						ui32SizeAvail = sBufInfo.ui32Size;
						

						{
                        				/*Download all encode Framesync for supported algorithm*/	
                                    		eCodeExecId = sExecID.eAlgoIds[0];
                                                if(eCodeExecId != BRAP_AF_P_AlgoId_eInvalid)
                                                {
                                                    eCodeFirmwareId = hRap->sImgIdMappingArrays.ui32CodeImgId[eCodeExecId];
                                                    if(eCodeFirmwareId != BRAP_Img_Id_eInvalid)
                                                    {
                                                        		ui32CodeSize = hRap->sImgIdMappingArrays.ui32CodeSize[eCodeExecId];

                                                        		if( ui32CodeSize > ui32SizeAvail)
                                                        		{
                                                        			BDBG_ERR(("Size available (%d) to download Framesync (IMG ID = %#x) of Encode Algorithm (%#x)"
                                                        					"is less than required (%d)",ui32SizeAvail,eCodeFirmwareId,eAudioProcessing,ui32CodeSize));
                                                        			return	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                                                        		}
                            						if(!((false == bDownload)
                            							&& (false == bWatchDogRecovery)))                                                                
                                                                    {                  
        										BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(ui32CodePtr), &ui32physAddress);	
                                                        		BDBG_MSG(("Downloading Framesync(IMG ID = %#x) of Encode Algorithm (%#x) at address %#x (Physical = %#x) size = 0x%x",
                                                        						eCodeFirmwareId,eAudioProcessing,ui32CodePtr,ui32physAddress,ui32CodeSize));
                                                        		BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32CodePtr,eCodeFirmwareId,hRap->hHeap);	
                                                                      }
                                                        		BRAP_UpdateMitEntry(hRap,eCodeExecId,ui32CodePtr,ui32CodeSize,0,0);
                                                        		ui32CodePtr += ui32CodeSize;
                    							ui32SizeAvail-= ui32CodeSize;                                            
                                                    }
                                                }
							/*Download all encode Code+Table for supported algorithm*/	
							for(l = 2;l<sExecID.NumNodes;l++)
							{
							/*Download encode Code */
                        		                    eCodeExecId = sExecID.eAlgoIds[l];
                                                        if(eCodeExecId == BRAP_AF_P_AlgoId_eInvalid)
                                                        {
                                                            continue;
                                                        }                               
                                            		eCodeFirmwareId = hRap->sImgIdMappingArrays.ui32CodeImgId[eCodeExecId];

								if(BRAP_Img_Id_eInvalid == eCodeFirmwareId)
								{
									BDBG_MSG(("Invalid Image ID for  code of encode processing algorithm %#x Stage %d",eAudioProcessing,l));
									continue;
								}

								ui32CodeSize = hRap->sImgIdMappingArrays.ui32CodeSize[eCodeExecId];

								if(ui32CodeSize > ui32SizeAvail)
								{
									BDBG_ERR(("Size available (%d) to download encode code (IMG ID = %#x) of processing Algorithm (%#x)"
											"is less than required (%d)",	
											ui32SizeAvail,eCodeFirmwareId,eAudioProcessing,ui32CodeSize
											));
									return	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
								}
                    						if(!((false == bDownload)
                    							&& (false == bWatchDogRecovery)))                                                                
                                                        {                                 
								BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(ui32CodePtr), &ui32physAddress);	
								BDBG_MSG(("Downloading encode code (IMG ID = %#x) of Processing Algorithm (%#x) at address %#x (Physical = %#x), size = 0x%x",
												eCodeFirmwareId,eAudioProcessing,ui32CodePtr,ui32physAddress,ui32CodeSize));
								BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32CodePtr,eCodeFirmwareId,hRap->hHeap);	
                                                        }

								ui32SizeAvail-= ui32CodeSize;

								ui32TblPtr = ui32CodePtr + ui32CodeSize;

								/*Download encode table */
								eTblFirmwareId = hRap->sImgIdMappingArrays.ui32TableImgId[eCodeExecId];

								if(BRAP_Img_Id_eInvalid == eTblFirmwareId)
								{
									BRAP_UpdateMitEntry(hRap,eCodeExecId,ui32CodePtr,ui32CodeSize,0,0);	
                                    				ui32CodePtr += ui32CodeSize;	                                                           
									BDBG_MSG(("Invalid Image ID for encode table of algorithm %#x Stage %d",eAudioProcessing,l));
									continue;
								}
        							bDownloaded = false;
								BRAP_FWDWNLD_P_IsTableDownloaded(BRAP_DSP_ExecType_eEncode,eCodeExecId,&bDownloaded,&eTempExecId);
								
								if(!bDownloaded)
								{
									ui32TblSize = hRap->sImgIdMappingArrays.ui32TableSize[eCodeExecId];

									if( ui32TblSize > ui32SizeAvail)
									{
										BDBG_ERR(("Size available (%d) to download table (IMG ID = %#x) of encode processing Algorithm (%#x)"
												"(%#x) is less than required (%d)",
												ui32SizeAvail,eTblFirmwareId,eAudioProcessing,ui32TblSize
												));
										return	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
									}
                        						if(!((false == bDownload)
                        							&& (false == bWatchDogRecovery)))                                       
                                                             {                  
									BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(ui32TblPtr), &ui32physAddress);	

									BDBG_MSG(("Downloading  table (Image ID = %#x) of encode processing algorithm %#x at address %#x (Physical = %#x), size = 0x%x",
													eTblFirmwareId,eAudioProcessing,ui32TblPtr,ui32physAddress,ui32TblSize));
									BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32TblPtr,eTblFirmwareId,hRap->hHeap);	
                                                              }

									/* updating ui32CodePtr variable, used to track the used size */ 		
									ui32SizeAvail-=ui32TblSize;
									
								}
								else
								{
									ui32TblPtr = hRap->sMit.sAlgoDwnldDetail[eTempExecId].ui32AlgoDataPtr;
									ui32TblSize= hRap->sMit.sAlgoDwnldDetail[eTempExecId].ui32AlgoDataSize;
									BDBG_MSG((" Table (IMG ID = %#x) of Encode processing Algorithm (%#x) is already downloaded at address 0x%x, size = 0x%x",
													eTblFirmwareId,eAudioProcessing,ui32TblPtr,ui32TblSize));
								}
                                    			BRAP_UpdateMitEntry(hRap,eCodeExecId,ui32CodePtr,ui32CodeSize,ui32TblPtr,ui32TblSize);	
								if(false == bDownloaded)
								{
									ui32CodePtr += ui32CodeSize + ui32TblSize;
								}
								else
								{
									ui32CodePtr += ui32CodeSize;				
								}
							}/*if (false == download)*/
						}/*if*/
					} /*else if*/
					else if((false == bOpenTimeAudProcDownload)
						&& (eAudioProcessing >= BRAP_ProcessingType_ePostprocessingStartIndex)
						&& (eAudioProcessing < BRAP_ProcessingType_eMax)) /*Download Post Processing algorithm, If it needs to be downloaded at start time*/
					{
						BRAP_FWDWNLD_P_GetAudProcExecIds(BRAP_sMapProcessingEnum[eAudioProcessing].uEnumName.eProcessingType,&sExecID);
						BRAP_DSPCHN_P_AllocateFwCtxtBuf(hRap, (uint32_t)eAudioProcessing, &ui32FwCtxIndx,&bDownload,BRAP_DSP_ExecType_eProcessingAlgo,bWatchDogRecovery);

						/* If its Watchdog recovery, And its exec is not already in the DRAM, 
						then this is a BAD DEVICE STATE */

						if ((BRAP_P_GetWatchdogRecoveryFlag(hRap))
							&& (true == bDownload))
						{
							BDBG_ERR(("In Watchdog recovery processing exec is not already in the DRAM"
									" Corruption of DRAM "));
							return BERR_TRACE( BRAP_ERR_BAD_DEVICE_STATE );
						}

						/* If no Free Buf to download Processing Algo */
						if((RAP_INVALID_INDEX == ui32FwCtxIndx)
							|| ui32FwCtxIndx >= BRAP_DSP_P_MAX_AUD_PROCESSING_CTX)
						{
							BDBG_ERR(("ERROR!!! -- All processing context buffers are in use."));
							return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
						}

						sBufInfo = hRap->sDwnldMemInfo.sProcessingAlgoBufInfo[ui32FwCtxIndx];
						ui32CodePtr = sBufInfo.ui32BaseAddr;
						ui32SizeAvail = sBufInfo.ui32Size;
						

						{
							/*Download all Processing Code+Table for supported algorithm*/	
							for(l = 2;l<sExecID.NumNodes;l++)
							{
							/*Download Processing Code */
                                            		eCodeExecId = sExecID.eAlgoIds[l];
                                                        if(eCodeExecId == BRAP_AF_P_AlgoId_eInvalid)
                                                        {
                                                            continue;
                                                        }                               
                                            		eCodeFirmwareId = hRap->sImgIdMappingArrays.ui32CodeImgId[eCodeExecId];

								if(BRAP_Img_Id_eInvalid == eCodeFirmwareId)
								{
									BDBG_MSG(("Invalid Image ID for  code of processing algorithm %#x Stage %d",eAudioProcessing,l));
									continue;
								}

								ui32CodeSize = hRap->sImgIdMappingArrays.ui32CodeSize[eCodeExecId];

								if(ui32CodeSize > ui32SizeAvail)
								{
									BDBG_ERR(("Size available (%d) to download code (IMG ID = %#x) of processing Algorithm (%#x)"
											"is less than required (%d)",	
											ui32SizeAvail,eCodeFirmwareId,eAudioProcessing,ui32CodeSize
											));
									return	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
								}
                						if(!((false == bDownload)
                							&& (false == bWatchDogRecovery)))
                                                        {                  
								BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(ui32CodePtr), &ui32physAddress);					
								BDBG_MSG(("Downloading  code (IMG ID = %#x) of Processing Algorithm (%#x) at address %#x (Physical = %#x),, size = 0x%x",
												eCodeFirmwareId,eAudioProcessing,ui32CodePtr,ui32physAddress,ui32CodeSize));
								BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32CodePtr,eCodeFirmwareId,hRap->hHeap);	
                                                      }

								ui32SizeAvail-= ui32CodeSize;

								ui32TblPtr = ui32CodePtr + ui32CodeSize;

								/*Download processing table */
								eTblFirmwareId = hRap->sImgIdMappingArrays.ui32TableImgId[eCodeExecId];

								if(BRAP_Img_Id_eInvalid == eTblFirmwareId)
								{
									BRAP_UpdateMitEntry(hRap,eCodeExecId,ui32CodePtr,ui32CodeSize,0,0);	
                                    				ui32CodePtr += ui32CodeSize;	                                                           
									BDBG_MSG(("Invalid Image ID for table of processing algorithm %#x Stage %d",eAudioProcessing,l));
									continue;
								}
								bDownloaded = false;
								BRAP_FWDWNLD_P_IsTableDownloaded(BRAP_DSP_ExecType_eProcessingAlgo,eCodeExecId,&bDownloaded,&eTempExecId);
								
								if(!bDownloaded)
								{
									ui32TblSize = hRap->sImgIdMappingArrays.ui32TableSize[eCodeExecId];

									if( ui32TblSize > ui32SizeAvail)
									{
										BDBG_ERR(("Size available (%d) to download table (IMG ID = %#x) of processing Algorithm (%#x)"
												"(%#x) is less than required (%d)",
												ui32SizeAvail,eTblFirmwareId,eAudioProcessing,ui32TblSize
												));
										return	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
									}
                        						if(!((false == bDownload)
                        							&& (false == bWatchDogRecovery)))
                                                                {                                     
									BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(ui32TblPtr), &ui32physAddress);	

									BDBG_MSG(("Downloading  table (Image ID = %#x) of processing algorithm %#x at address %#x (Physical = %#x), size = 0x%x",
													eTblFirmwareId,eAudioProcessing,ui32TblPtr,ui32physAddress,ui32TblSize));
									BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32TblPtr,eTblFirmwareId,hRap->hHeap);	
									/* updating ui32CodePtr variable, used to track the used size */ 		
                                                              }
									ui32SizeAvail-=ui32TblSize;
									
								}
								else
								{
									ui32TblPtr = hRap->sMit.sAlgoDwnldDetail[eTempExecId].ui32AlgoDataPtr;
									ui32TblSize= hRap->sMit.sAlgoDwnldDetail[eTempExecId].ui32AlgoDataSize;
                                                        
									BDBG_MSG((" Table (IMG ID = %#x) of processing Algorithm (%#x) is already downloaded at address 0x%x, size = 0x%x",
													eTblFirmwareId,eAudioProcessing,ui32TblPtr,ui32TblSize));
								}
                                    			BRAP_UpdateMitEntry(hRap,eCodeExecId,ui32CodePtr,ui32CodeSize,ui32TblPtr,ui32TblSize);	    
								if(false == bDownloaded)
								{
									ui32CodePtr += ui32CodeSize + ui32TblSize;
								}
								else
								{
									ui32CodePtr += ui32CodeSize;				
								}
							}/*if (false == download)*/
						}/*if*/
					}
			}/* for i*/
		    }
      	    }
		}/*if*/

	}/* Download Decode Mode Execs */

	ui32FwCtxIndx = RAP_INVALID_INDEX;
	ui32CodePtr = 0;
	ui32CodeSize = 0;
	ui32SizeAvail = 0;
	bDownload = 0;
	/* Download Passthru Mode Execs */
	{
		if((BRAP_DSPCHN_DecodeMode_ePassThru== eDecMode))
		{
			BRAP_DSPCHN_P_AllocateFwCtxtBuf(hRap,BRAP_ProcessingType_ePassthru, &ui32FwCtxIndx,&bDownload,BRAP_DSP_ExecType_ePassthru,bWatchDogRecovery);

			/* If its Watchdog recovery, And its exec is not already in the DRAM, 
			then this is a BAD DEVICE STATE */
			if ((true == bWatchDogRecovery)
				&& (true == bDownload))
			{
				BDBG_ERR(("In Watchdog recovery Passthru exec is not already in the DRAM"
						" Corruption of DRAM "));
				return BERR_TRACE( BRAP_ERR_BAD_DEVICE_STATE );
			}

			/* If no Free Buf to download FS */
			if((RAP_INVALID_INDEX == ui32FwCtxIndx)
				|| ui32FwCtxIndx >= BRAP_DSP_P_MAX_PASSTHRU_CTXT)
			{
				BDBG_ERR(("ERROR!!! -- All Passthru context buffers are in use."));
				return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
			}

			sBufInfo = hRap->sDwnldMemInfo.sPtCodecBufInfo[ui32FwCtxIndx];
			ui32CodePtr = sBufInfo.ui32BaseAddr;
			ui32SizeAvail = sBufInfo.ui32Size;
			if(!((false == bDownload)
				&& (false == bWatchDogRecovery)))
			{
					eCodeFirmwareId = BRAP_Img_Id_ePassthruCode;
					
					ui32CodeSize = hRap->sImgIdMappingArrays.ui32CodeSize[BRAP_AF_P_AlgoId_ePassThru];

					if(ui32CodeSize > ui32SizeAvail)
					{
						BDBG_ERR(("Size available (%d) to download passthru code (IMG ID = %#x) of Audio Algorithm (%#x)"
								"is less than required (%d)",	
								ui32SizeAvail,eCodeFirmwareId,eAudType,ui32CodeSize
								));
						return	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
					}
					BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(ui32CodePtr), &ui32physAddress);	
					BDBG_MSG(("Downloading passthru code (IMG ID = %#x) of Audio Algorithm (%#x) at address %#x (Physical = %#x), size = 0x%x",
									eCodeFirmwareId,eAudType,ui32CodePtr,ui32physAddress,ui32CodeSize));
					BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32CodePtr,eCodeFirmwareId,hRap->hHeap);	

					ui32SizeAvail-= ui32CodeSize;

					ui32TblPtr = ui32CodePtr + ui32CodeSize;

					/*Download passthru table */
					eTblFirmwareId = BRAP_Img_Id_ePassthruTable;

					ui32TblSize = hRap->sImgIdMappingArrays.ui32TableSize[BRAP_AF_P_AlgoId_ePassThru];

						if( ui32TblSize > ui32SizeAvail)
						{
							BDBG_ERR(("Size available (%d) to download passthru table (IMG ID = %#x) of Audio Algorithm (%#x)"
									"(%#x) is less than required (%d)",
									ui32SizeAvail,eTblFirmwareId,eAudType,ui32TblSize
									));
							return	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
						}
						BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(ui32TblPtr), &ui32physAddress);	

						BDBG_MSG(("Downloading passthru Table (IMG ID = %#x) of Audio Algorithm (%#x) at address %#x (Physical = %#x), size = 0x%x",
										eTblFirmwareId,eAudType,ui32TblPtr,ui32physAddress,ui32TblSize));
						BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32TblPtr,eTblFirmwareId,hRap->hHeap);	

					BRAP_UpdateMitEntry(hRap,BRAP_AF_P_AlgoId_ePassThru,ui32CodePtr,ui32CodeSize,ui32TblPtr,ui32TblSize);	
			}/*if-else*/
		}/*if*/

	}/* Download Passthru Mode Execs */

	BDBG_LEAVE(BRAP_DSPCHN_P_DownloadStartTimeFWExec);	
	return BERR_SUCCESS;
}	


BERR_Code BRAP_DSP_P_InitInterframeBuffer(
				BRAP_FWIF_P_FwTaskHandle	hTask)
{
	BRAP_CIT_P_Output	*pCitOutput = NULL;
	BRAP_CIT_P_FwBufInfo *pOpNodeInfo = NULL;
	BERR_Code	rc = BERR_SUCCESS;
	unsigned int i,j,k;
	const BIMG_Interface *iface = NULL;	
	void *pImgContext = NULL;
	uint32_t	ui32ImgId = BRAP_MAX_IMG_ID,ui32physAddress=0;
	bool bDownloaded =false;
	BRAP_Handle	hRap = NULL;

	BDBG_ENTER(BRAP_DSP_P_InitInterframeBuffer);
	BDBG_ASSERT(hTask);

    if(true == hTask->bChSpecificTask)
    {
        hRap = hTask->uHandle.hRapCh->hRap;
    }
    else
    {
        hRap = hTask->uHandle.hAssociation->hRap;
    }
	iface = hRap->sSettings.pImgInterface;	
	pImgContext = hRap->sSettings.pImgContext;
	pCitOutput = &(hTask->sCitOutput);
	for(i = 0;i < pCitOutput->ui32NumBranches; i++)
	{
		for(j = 0; j < pCitOutput->sCitBranchInfo[i].ui32NumStages; j++)
		{
			for(k = 0; k < pCitOutput->sCitBranchInfo[i].sCitStgInfo[j].ui32NumNodes; k++)
			{
				if(pCitOutput->sCitBranchInfo[i].sCitStgInfo[j].sFwOpNodeInfo[k].eFwExecId != BRAP_AF_P_AlgoId_eMax)
				{
					pOpNodeInfo = &(pCitOutput->sCitBranchInfo[i].sCitStgInfo[j].sFwOpNodeInfo[k]);

					/* check if enough memory is allocated for interframe buffer */
					if(pOpNodeInfo->ui32InterframeBufSize < hRap->sImgIdMappingArrays.ui32InterframeSize[pOpNodeInfo->eFwExecId])
					{
						BDBG_ERR(("Allocated memory (%d) for interframe buffer is less than required (%d) for ExecId =%#x",
							pOpNodeInfo->ui32InterframeBufSize ,
							hRap->sImgIdMappingArrays.ui32InterframeSize[pOpNodeInfo->eFwExecId],
							pOpNodeInfo->eFwExecId));
					}
					else
					{
						rc = BRAP_P_GetInterframeImgId(hRap,pOpNodeInfo->eFwExecId,&ui32ImgId,&bDownloaded);
					if(BERR_SUCCESS != rc)
					{
						return BERR_TRACE(rc);
					}
						BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(pOpNodeInfo->ui32InterframeBufAdr), &ui32physAddress);	
						BDBG_MSG(("pOpNodeInfo->ui32InterframeBufAdr %#x physical %#x",pOpNodeInfo->ui32InterframeBufAdr,ui32physAddress));

					
					if(BRAP_MAX_IMG_ID == ui32ImgId)
					{
						BDBG_MSG(("No FW Image file exists for Exec Id %#x",pOpNodeInfo->eFwExecId));
					}
					else if(false == bDownloaded)
					{
				        BDBG_MSG(("pOpNodeInfo->eFwExecId > %d, ui32ImgId > %d", pOpNodeInfo->eFwExecId, ui32ImgId));
						BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,pOpNodeInfo->ui32InterframeBufAdr,ui32ImgId,hRap->hHeap);
					}
				}
				}
				else
					break;
				
			}
		}
	}

	
	BDBG_LEAVE(BRAP_DSP_P_InitInterframeBuffer);
	return rc;

}




BERR_Code BRAP_DSP_P_InitStatusBuffer(
				BRAP_FWIF_P_FwTaskHandle	hTask)
{
	BRAP_CIT_P_Output	*pCitOutput = NULL;
	BRAP_CIT_P_FwBufInfo *pOpNodeInfo = NULL;
	BERR_Code	rc = BERR_SUCCESS;
	unsigned int i,j,k;

	BDBG_ENTER(BRAP_DSP_P_InitStatusBuffer);
	BDBG_ASSERT(hTask);

	pCitOutput = &(hTask->sCitOutput);
	for(i = 0;i < pCitOutput->ui32NumBranches; i++)
	{
		for(j = 0; j < pCitOutput->sCitBranchInfo[i].ui32NumStages; j++)
		{
			for(k = 0; k < pCitOutput->sCitBranchInfo[i].sCitStgInfo[j].ui32NumNodes; k++)
			{
				if(pCitOutput->sCitBranchInfo[i].sCitStgInfo[j].sFwOpNodeInfo[k].eFwExecId != BRAP_AF_P_AlgoId_eMax)
				{
					pOpNodeInfo = &(pCitOutput->sCitBranchInfo[i].sCitStgInfo[j].sFwOpNodeInfo[k]);

					/* check if enough memory is allocated or not. If memory is allocated than 
					initalize the first four byte with valid status*/
					if(pOpNodeInfo->ui32StatusBufAdr != BRAP_P_INVALID_DRAM_ADDRESS)
					{
					    if(pOpNodeInfo->ui32StatusBufSize > 0)
                        {
                            BRAP_Write32(hTask->hDsp->hRegister,pOpNodeInfo->ui32StatusBufAdr, 
                                BRAP_FWSTATUS_STATUS_INVALID);
                        }
                        else
                        {
                            BDBG_ERR(("BRAP_DSP_P_InitStatusBuffer: Status buffer size invalid 0x%x",
                                        pOpNodeInfo->ui32StatusBufSize));
                            BDBG_ASSERT(0);
                            return BERR_TRACE(BRAP_ERR_BUFFER_INVALID);
                         }
					}
				}
				else
					break;
 			}
		}
	}
 	BDBG_LEAVE(BRAP_DSP_P_InitStatusBuffer);
	return rc;

}

static BERR_Code BRAP_P_GetInterframeImgId(
			BRAP_Handle hRap,
			BRAP_AF_P_AlgoId	eFwExecId,
			uint32_t	*ui32ImgId,
			bool *bDownloaded)
{

	BDBG_ENTER(BRAP_P_GetInterframeImgId);
	BDBG_ASSERT(hRap);

	*bDownloaded = false;
	
	if(eFwExecId < BRAP_AF_P_AlgoId_eMax)
	{
		*ui32ImgId = hRap->sImgIdMappingArrays.ui32InterframeImgId[eFwExecId];
	}
	else
	{
		BDBG_ERR(("Invalid FW exec ID"));
		return BERR_INVALID_PARAMETER;
	}
	
	
	BDBG_LEAVE(BRAP_P_GetInterframeImgId);
	return BERR_SUCCESS;
}

BERR_Code BRAP_DSPCHN_P_FreeStartTimeFWCtxt(
				BRAP_DSPCHN_Handle hDspCh)
{
    BRAP_Handle	hRap = NULL;
    BRAP_DSPCHN_AudioType eAudType = BRAP_DSPCHN_AudioType_eMax;
    bool	bOpenTimeAudProcDownload = true;
    BRAP_ProcessingType eAudioProcessing = BRAP_ProcessingType_eNone;
    BRAP_DSPCHN_DecodeMode eDecMode = BRAP_DSPCHN_DecodeMode_eInvalid; 
    int i = 0, j = 0, k = 0;

    BDBG_ASSERT(hDspCh);
    eAudType = hDspCh->sDspAudioParams.sExtAudioParams.eType;
    eDecMode = hDspCh->sDspAudioParams.sExtAudioParams.eDecodeMode;

    BRAP_FWDWNLD_P_IfOpenTimeProcDownload(&bOpenTimeAudProcDownload);
    
    if(true == hDspCh->bChSpecificDspCh)
    {
        hRap = hDspCh->uHandle.hRapCh->hRap;
    }
    else
    {
        hRap = hDspCh->uHandle.hAssociation->hRap;
    }
#ifdef RAP_VIDEOONDSP_SUPPORT
    if(true == hDspCh->bChSpecificDspCh)
    {
          if(hDspCh->uHandle.hRapCh->eChannelType == BRAP_ChannelType_eVideoDecode)
        {
              BRAP_DSPCHN_P_FreeFwCtxtBuf(hRap, eAudType, BRAP_DSP_ExecType_eVideoDecode);            
              return BERR_SUCCESS;
        }
    }
#endif    
    BRAP_DSPCHN_P_FreeFwCtxtBuf(hRap, eAudType, BRAP_DSP_ExecType_eFrameSync);

    if(BRAP_DSPCHN_DecodeMode_eDecode == eDecMode)
    {
        BRAP_DSPCHN_P_FreeFwCtxtBuf(hRap, eAudType, BRAP_DSP_ExecType_eDecode);
        for(i=0;i<BRAP_FWIF_P_MAX_FW_TASK_PER_DSPCHN;i++)
        {
            for(j=0;j<BRAP_P_MAX_DST_PER_RAPCH;j++)
            {
                    for(k=0;k<BRAP_MAX_STAGE_PER_BRANCH_SUPPORTED;k++)
                    {
    					if(!(BRAP_P_IsPointerValid((void *)hDspCh->sFwTaskInfo[i].sProcessingNw.sAudProcessingStage[j][k].hAudioProcessing)))
					{
						continue;
					}
					eAudioProcessing = hDspCh->sFwTaskInfo[i].sProcessingNw.sAudProcessingStage[j][k].hAudioProcessing->sProcessingStageSettings.eAudioProcessing;
					
					if((BRAP_ProcessingType_ePassthru == eAudioProcessing))
					{
					        BRAP_DSPCHN_P_FreeFwCtxtBuf(hRap, eAudioProcessing, BRAP_DSP_ExecType_ePassthru);
					}
     					else if((eAudioProcessing >= BRAP_ProcessingType_eEncodeStartIndex)
        					&& (eAudioProcessing < BRAP_ProcessingType_ePostprocessingStartIndex))
					{
                                            BRAP_DSPCHN_P_FreeFwCtxtBuf(hRap, eAudioProcessing, BRAP_DSP_ExecType_eEncode);
     					}
                    			else if((false == bOpenTimeAudProcDownload)
        					&& (eAudioProcessing >= BRAP_ProcessingType_ePostprocessingStartIndex)
        					&& (eAudioProcessing < BRAP_ProcessingType_eMax)) /*Download Post Processing algorithm, If it needs to be downloaded at start time*/
					{
                                            BRAP_DSPCHN_P_FreeFwCtxtBuf(hRap, eAudioProcessing, BRAP_DSP_ExecType_eProcessingAlgo);
                                   }
                    }
            }
        }

    }
    else if(BRAP_DSPCHN_DecodeMode_ePassThru == eDecMode)
    {
        BRAP_DSPCHN_P_FreeFwCtxtBuf(hRap, BRAP_ProcessingType_ePassthru, BRAP_DSP_ExecType_ePassthru);
    }       
    return BERR_SUCCESS;
}



/***************************************************************************
Summary: Enable/Disable Time Stamp Management

Description:
	This function enables/disables Time Stamp Management for a given audio
	channel. 
	Note: This API is valid only for a decode channel.
	
Returns:
	BERR_SUCCESS - If successful

See Also:
	BRAP_SetTSMDiscardThreshold
**************************************************************************/
BERR_Code 
BRAP_GetTsmNodeInfo(
    BRAP_ChannelHandle	            hRapCh,             /*[In] Rap channel handle */
	BRAP_DSPCHN_Handle              hDspCh,             /*[In] Dsp CHannel handle */
    unsigned int                    *pConfigBufAddr,    /*[Out] Config buffer address */
    BRAP_FWIF_P_FwTaskHandle        *hFwTask,       /*{out] task handle */    
    BRAP_FWIF_P_TsmConfigParams     **psTsmConfigParam /*[out] Tsm config param*/   
)
{
    unsigned int uiTaskIndex = 0, uiBranchId = 0, uiStageId = 0, uiTsmNodeIndex;
    BRAP_CIT_P_OpStgInfo *psOpStgInfo;
    bool bDecStageFound = false;
    bool bIsFwBranch =false;

    BRAP_P_ChannelAudioProcessingStage  sTempAudProcStage;
    unsigned int uiTempBranchId = 0;
    bool bBranchCloned = false;

    BDBG_ENTER(BRAP_GetTsmNodeInfo);
    BDBG_ASSERT(hRapCh);
    BDBG_ASSERT(hDspCh);

    /* Find out the task in which the decode stage is getting executed.
     Also find out the exact position of the decode stage within audio
     processing network. */
  for (uiTaskIndex = 0; uiTaskIndex < BRAP_FWIF_P_MAX_FW_TASK_PER_DSPCHN; uiTaskIndex++)
    {
        uiTempBranchId = 0;
        for (uiBranchId = 0; uiBranchId < BRAP_P_MAX_DST_PER_RAPCH; uiBranchId++)
        {
            bIsFwBranch = false;
            for (uiStageId = 0; uiStageId < BRAP_MAX_STAGE_PER_BRANCH_SUPPORTED; uiStageId++)
            {
                sTempAudProcStage = hDspCh->sFwTaskInfo[uiTaskIndex].sProcessingNw.sAudProcessingStage[uiBranchId][uiStageId];
                if (true== sTempAudProcStage.bDecoderStage)
                {
                    bDecStageFound = true;
                    break;
                }
                else if(((false == sTempAudProcStage.bCloneStage) && (NULL == sTempAudProcStage.hAudioProcessing)
                                && (true == hDspCh->sFwTaskInfo[uiTaskIndex].sProcessingNw.sAudProcessingStage[uiBranchId][uiStageId -1].bCloneStage))
                            ||((uiStageId == BRAP_MAX_STAGE_PER_BRANCH_SUPPORTED -1) && (true == sTempAudProcStage.bCloneStage)) /* Check if the last valid stage of the branch has bCloneStage == true. In that case output port cloning is happening.*/
                          ) 
                {
                    if(uiStageId == BRAP_MAX_STAGE_PER_BRANCH_SUPPORTED -1) /*If stageId is equal to MAX Stage ID*/
                    {
                        BRAP_FWIF_P_IsBranchCloned(hRapCh,&(hDspCh->sFwTaskInfo[uiTaskIndex].sProcessingNw),uiBranchId,uiStageId,&bBranchCloned,&bIsFwBranch);                
                    }
                    else
                    {
                        BRAP_FWIF_P_IsBranchCloned(hRapCh,&(hDspCh->sFwTaskInfo[uiTaskIndex].sProcessingNw),uiBranchId,uiStageId-1,&bBranchCloned,&bIsFwBranch);                
                    }                        
                    break;  /* Break from the FOR loop beacause last valid stage of the Branch */
                }
            }
            if (true==bDecStageFound) 
                break;
            
            if(true == bIsFwBranch)
            {
                uiTempBranchId++;
            }
        }
        if (true==bDecStageFound) break;
    }
    if (false==bDecStageFound)
    {
        for (uiTaskIndex = 0; uiTaskIndex < BRAP_FWIF_P_MAX_FW_TASK_PER_DSPCHN; uiTaskIndex++)
        {
            if((!(BRAP_P_IsPointerValid((void *)(hDspCh->sFwTaskInfo[uiTaskIndex].hFwTask)))) || (hDspCh->sFwTaskInfo[uiTaskIndex].hFwTask->bStopped ==true))
            {
                BDBG_WRN(("Task/DSP Channel already stopped"));
                return BERR_INVALID_PARAMETER;
            }
        }
        
        BDBG_ERR(("Could not find decoder stage in the network of audio processing stages."));
        BDBG_ASSERT(0);
        return BERR_TRACE(BRAP_ERR_BAD_DEVICE_STATE);
    }

    if(uiTempBranchId >= BRAP_P_MAX_FW_BRANCH_PER_FW_TASK)
    {
        BDBG_ERR(("Branch id (%d) is exceeding the BRAP_P_MAX_FW_BRANCH_PER_FW_TASK =%d",uiTempBranchId ,BRAP_P_MAX_FW_BRANCH_PER_FW_TASK));
        BDBG_ASSERT(0);
        return BERR_TRACE(BRAP_ERR_BAD_DEVICE_STATE);
    }

    /* Get the Status buffer params */
    *hFwTask = hDspCh->sFwTaskInfo[uiTaskIndex].hFwTask;
    
    if((!(BRAP_P_IsPointerValid((void *)(*hFwTask)))) || (*hFwTask)->bStopped ==true)
    {
        BDBG_WRN(("Task already stopped"));
        return BERR_INVALID_PARAMETER;
    }
    psOpStgInfo = &((*hFwTask)->sCitOutput.sCitBranchInfo[uiTempBranchId].sCitStgInfo[uiStageId]);
    uiTsmNodeIndex = psOpStgInfo->ui32TsmNodeIndex;
	if (sizeof(BRAP_FWIF_P_TsmConfigParams) > psOpStgInfo->sFwOpNodeInfo[uiTsmNodeIndex].ui32UserParamBufSize)
	{
		BDBG_ERR(("Buffer size allocated is less than size of configuration parameters structure., Actual =%d, Expected =%d"
          ,psOpStgInfo->sFwOpNodeInfo[uiTsmNodeIndex].ui32UserParamBufSize,sizeof(BRAP_FWIF_P_TsmConfigParams)));
		return BERR_TRACE(BRAP_ERR_BAD_DEVICE_STATE);
	}    
    uiTsmNodeIndex = psOpStgInfo->ui32TsmNodeIndex;    
    *pConfigBufAddr = psOpStgInfo->sFwOpNodeInfo[uiTsmNodeIndex].ui32UserParamBufAdr;

    switch(psOpStgInfo->sFwOpNodeInfo[uiTsmNodeIndex].eFwExecId)
    {
        case BRAP_AF_P_AlgoId_eDecodeTsm:
        {
            if(hDspCh->sDspAudioParams.sExtAudioParams.eDecodeMode == BRAP_DSPCHN_DecodeMode_eDecode)
            {
                *psTsmConfigParam = &(hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam);
            }
            else if(hDspCh->sDspAudioParams.sExtAudioParams.eDecodeMode == BRAP_DSPCHN_DecodeMode_ePassThru)
            {
                *psTsmConfigParam = &(hRapCh->sDecSettings.sUserConfigStruct.sPassthruTsmConfigParam);
            }
            else
            {
                BDBG_ERR(("Invalid decode mode %d, Probably Channel is already stopped",hDspCh->sDspAudioParams.sExtAudioParams.eDecodeMode));
                return BERR_INVALID_PARAMETER;
            }            
            break;
        }
        case BRAP_AF_P_AlgoId_eEncodeTsm:
        {
            *psTsmConfigParam = &(hRapCh->sDecSettings.sUserConfigStruct.sEncodeTsmConfigParam);
            break;
        }
/*        
        case BRAP_AF_P_AlgoId_ePassthroughTsm:
        {
            *psTsmConfigParam = &(hRapCh->sDecSettings.sUserConfigStruct.sPassthruTsmConfigParam);
            break;
        }
*/        
        default:
            BDBG_ASSERT(0);
    }        
    BDBG_LEAVE(BRAP_GetTsmNodeInfo);    
    return BERR_SUCCESS;
}


static BERR_Code
BRAP_DSPCHN_P_SetTsmConfig (
	BRAP_DSPCHN_Handle	hDspCh,
	unsigned int        uiConfigBufAddr,
    BRAP_FWIF_P_FwTaskHandle hFwTask,
    BRAP_FWIF_P_TsmConfigParams     *psTsmConfigParam, 
    BRAP_FWIF_P_ResponseType        *peResponseType   
)
{
    BERR_Code err = BERR_SUCCESS;
    unsigned int  uiTaskId = 0;
    BRAP_Fwif_P_FwBufInfo  *psFwBufInfo;
    BRAP_FWIF_P_Command sCommand;
    BRAP_FWIF_P_Response sRsp;
    BRAP_Handle hRap;
    BRAP_DSP_Handle hDsp;
    BRAP_P_MsgType      eMsgType;
    uint32_t ui32PhysAddr = 0;    
    
    BDBG_ENTER(BRAP_DSPCHN_P_SetTsmConfig);

    BDBG_ASSERT(hDspCh);
    BDBG_ASSERT(psTsmConfigParam);    
    BDBG_ASSERT(uiConfigBufAddr);    
    BDBG_ASSERT(hFwTask);    

    /* Program configuration parameters in extra buffer */
    if(hDspCh->bChSpecificDspCh == true)
    {
        hRap = hDspCh->uHandle.hRapCh->hRap;
    }
    else
    {
        hRap = hDspCh->uHandle.hAssociation->hRap;
    }
    hDsp = hFwTask->hDsp;
    uiTaskId = hFwTask->uiTaskId;
    if((hFwTask->bStopped == true)
        || (uiTaskId == BRAP_FWIF_P_INVALID_TSK_ID))
    {        
        BDBG_MSG(("BRAP_DSPCHN_P_SetTsmConfig: BRAP_FWIF_P_INVALID_TSK_ID!"));
        return BERR_SUCCESS;
    }
    psFwBufInfo = &(hRap->sMemInfo.sTsmConfigParamBufInfo[BRAP_GET_TASK_INDEX(uiTaskId)]);

	BKNI_Memcpy((void *)(volatile void*)(psFwBufInfo->ui32BaseAddr),(void *)psTsmConfigParam,sizeof(BRAP_FWIF_P_TsmConfigParams));
    /* Create Change-Config command */
    sCommand.sCommandHeader.ui32CommandID = BRAP_FWIF_P_ALGO_PARAMS_CFG_COMMAND_ID;
    sCommand.sCommandHeader.ui32CommandCounter = hRap->uiCommandCounter++;
    sCommand.sCommandHeader.ui32TaskID = uiTaskId;
    sCommand.sCommandHeader.eResponseType = *peResponseType;
    BRAP_P_ConvertAddressToOffset(hRap->hHeap,(void *)(psFwBufInfo->ui32BaseAddr),&ui32PhysAddr);
    sCommand.uCommand.sCfgChange.ui32HostConfigParamBuffAddr = ui32PhysAddr;
    BRAP_P_ConvertAddressToOffset(hRap->hHeap,(void *)uiConfigBufAddr,&ui32PhysAddr);
    
    sCommand.uCommand.sCfgChange.ui32DspConfigParamBuffAddr = ui32PhysAddr;
    sCommand.uCommand.sCfgChange.ui32SizeInBytes = sizeof(BRAP_FWIF_P_TsmConfigParams);

    if(*peResponseType == BRAP_FWIF_P_ResponseType_eResponseRequired)
    {
        hFwTask->uiLastEventType = sCommand.sCommandHeader.ui32CommandID;    
        BRAP_P_EventReset(hDsp,BRAP_GET_TASK_INDEX(hFwTask->uiTaskId));
    }

    err = BRAP_FWIF_P_SendCommand(hDsp->hCmdQueue, &sCommand,hRap,hFwTask);
    if(BERR_SUCCESS != err)
    {
        BDBG_ERR(("BRAP_DSPCHN_P_SetTsmConfig: CFG_Command failed!"));
        return BERR_TRACE(err);
    }

    if(*peResponseType == BRAP_FWIF_P_ResponseType_eResponseRequired)
    {
    /* Wait for Ack_Response_Received event w/ timeout */
    err = BRAP_P_EventWait(hDsp, BRAP_DSPCHN_P_EVENT_TIMEOUT_IN_MS,BRAP_GET_TASK_INDEX(hFwTask->uiTaskId));
    if((BERR_TIMEOUT == err)
        &&(hFwTask->bStopped == false))
    {
        if((hRap->bWatchdogTriggered == false))
        {
            /* Please note that, If the code reaches at this point then there is a potential Bug in Fw 
            code which needs to be debugged. However Watchdog is being triggered to recover the system*/            
            BDBG_WRN(("BRAP_DSPCHN_P_SetTsmConfig: CFG_Command ACK timeout! Triggering Watchdog"));
#if 0                
            BDBG_ASSERT(0);                
#else
            BRAP_Write32(hDsp->hRegister, BCHP_AUD_DSP_INTH0_R5F_SET+ hDsp->ui32Offset,0x1);
            hRap->bWatchdogTriggered  = true;
#endif            
#if 0                
            err = BERR_TRACE(err);
            goto error;
#endif                 
            err = BERR_SUCCESS;              
        }
        else
            err = BERR_SUCCESS;              
    }
        
    eMsgType = BRAP_P_MsgType_eSyn;
        err = BRAP_FWIF_P_GetMsg(hFwTask->hSyncMsgQueue, (void *)&sRsp, eMsgType);
    if(BERR_SUCCESS != err)
    {
        if((hRap->bWatchdogTriggered == false)
            &&(hFwTask->bStopped == false))
        {
            BDBG_ERR(("BRAP_DSPCHN_P_SetTsmConfig: Unable to read ACK!"));
            return BERR_TRACE(err);
        }
        else
            err = BERR_SUCCESS;         
    }
    
    if((sRsp.sCommonAckResponseHeader.eStatus != BERR_SUCCESS)||
        (sRsp.sCommonAckResponseHeader.ui32ResponseID != BRAP_FWIF_P_ALGO_PARAMS_CFG_RESPONSE_ID)||
        (sRsp.sCommonAckResponseHeader.ui32ResponseCounter!=sCommand.sCommandHeader.ui32CommandCounter)||
        (sRsp.sCommonAckResponseHeader.ui32TaskID != hFwTask->uiTaskId))
    {
        if((hRap->bWatchdogTriggered == false)
            &&(hFwTask->bStopped == false))
        {    
            BDBG_ERR(("BRAP_DSPCHN_P_SetTsmConfig: CFG_Command response not received successfully!"));
            return BERR_TRACE(sRsp.sCommonAckResponseHeader.eStatus);
        }
        else
            err = BERR_SUCCESS;            
    }
   }
        
    BDBG_LEAVE(BRAP_DSPCHN_P_SetTsmConfig);
    return err;
}	

static  BERR_Code
BRAP_DSPCHN_P_SetTsmConfig_isr (
	BRAP_DSPCHN_Handle	hDspCh,
	unsigned int        uiConfigBufAddr,
    BRAP_FWIF_P_FwTaskHandle hFwTask,
    BRAP_FWIF_P_TsmConfigParams     *psTsmConfigParam, 
    BRAP_FWIF_P_ResponseType        *peResponseType   
)
{
    BERR_Code err = BERR_SUCCESS;
    unsigned int  uiTaskId = 0;
    BRAP_Fwif_P_FwBufInfo  *psFwBufInfo;
    BRAP_FWIF_P_Command sCommand;
    BRAP_Handle hRap;
    BRAP_DSP_Handle hDsp;
    uint32_t ui32PhysAddr = 0;    
    
    BDBG_ENTER(BRAP_DSPCHN_P_SetTsmConfig_isr);

    BDBG_ASSERT(hDspCh);
    BDBG_ASSERT(psTsmConfigParam);    
    BDBG_ASSERT(uiConfigBufAddr);    
    BDBG_ASSERT(hFwTask);    

    if(*peResponseType != BRAP_FWIF_P_ResponseType_eNone)
    {
        BDBG_ERR(("BRAP_DSPCHN_P_SetTsmConfig_isr: This function"
            " should be called with ResponseType None, because this is a isr function."));
        BDBG_ASSERT(0);
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    /* Program configuration parameters in extra buffer */
    if(hDspCh->bChSpecificDspCh == true)
    {
        hRap = hDspCh->uHandle.hRapCh->hRap;
    }
    else
    {
        hRap = hDspCh->uHandle.hAssociation->hRap;
    }
    hDsp = hFwTask->hDsp;
    uiTaskId = hFwTask->uiTaskId;
    if((hFwTask->bStopped == true)
        || (uiTaskId == BRAP_FWIF_P_INVALID_TSK_ID))
    {        
        BDBG_MSG(("BRAP_DSPCHN_P_SetTsmConfig_isr: BRAP_FWIF_P_INVALID_TSK_ID!"));
        return BERR_SUCCESS;
    }
    psFwBufInfo = &(hRap->sMemInfo.sTsmConfigParamBufInfo[BRAP_GET_TASK_INDEX(uiTaskId)]);

	BKNI_Memcpy((void *)(volatile void*)(psFwBufInfo->ui32BaseAddr),(void *)psTsmConfigParam,sizeof(BRAP_FWIF_P_TsmConfigParams));
    /* Create Change-Config command */
    sCommand.sCommandHeader.ui32CommandID = BRAP_FWIF_P_ALGO_PARAMS_CFG_COMMAND_ID;
    sCommand.sCommandHeader.ui32CommandCounter = hRap->uiCommandCounter++;
    sCommand.sCommandHeader.ui32TaskID = uiTaskId;
    sCommand.sCommandHeader.eResponseType = *peResponseType;
    BRAP_P_ConvertAddressToOffset_isr(hRap->hHeap,(void *)(psFwBufInfo->ui32BaseAddr),&ui32PhysAddr);
    sCommand.uCommand.sCfgChange.ui32HostConfigParamBuffAddr = ui32PhysAddr;
    BRAP_P_ConvertAddressToOffset_isr(hRap->hHeap,(void *)uiConfigBufAddr,&ui32PhysAddr);
    
    sCommand.uCommand.sCfgChange.ui32DspConfigParamBuffAddr = ui32PhysAddr;
    sCommand.uCommand.sCfgChange.ui32SizeInBytes = sizeof(BRAP_FWIF_P_TsmConfigParams);


    err = BRAP_FWIF_P_SendCommand_isr(hDsp->hCmdQueue, &sCommand,hRap,hFwTask);
    if(BERR_SUCCESS != err)
    {
        BDBG_ERR(("BRAP_DSPCHN_P_SetTsmConfig_isr: CFG_Command failed!"));
        return BERR_TRACE(err);
    }        
    BDBG_LEAVE(BRAP_DSPCHN_P_SetTsmConfig_isr);
    return err;
}	

BERR_Code
BRAP_EnableTSM(
	BRAP_ChannelHandle	hRapCh,			/* [in] Audio channel handle */
	bool			    bEnableTSM		/* [in] true = enable TSM,
							               false = disable TSM  */
	)
{

	BERR_Code rc = BERR_SUCCESS;

	BDBG_ENTER(BRAP_EnableTSM);
	/* Assert the function arguments */
	BDBG_ASSERT(hRapCh);

	BKNI_EnterCriticalSection();
	rc = BRAP_EnableTSM_isr(hRapCh, bEnableTSM);
	BKNI_LeaveCriticalSection();

	BDBG_LEAVE(BRAP_EnableTSM);
	return rc;
}

/***************************************************************************
Summary: ISR version of BRAP_EnableTSM

Description:
	This function enables/disables Time Stamp Management for a given audio
	channel in isr context.
	Note: This API is valid only for a decode channel.	
	
Returns:
	BERR_SUCCESS - If successful

See Also:
	BRAP_SetTSMDiscardThreshold
**************************************************************************/
BERR_Code
BRAP_EnableTSM_isr(
	BRAP_ChannelHandle	hRapCh,			/* [in] Audio channel handle */
	bool			    bEnableTSM	    /* [in] true = enable TSM,
							               false = disable TSM  */
	)
{
    BERR_Code err = BERR_SUCCESS;
    unsigned int uiConfigBufAddr;
	BRAP_DSPCHN_Handle hDspCh;
    BRAP_FWIF_P_TsmConfigParams      *psTsmConfigParam = NULL;
    BRAP_FWIF_P_FwTaskHandle        hFwTask=NULL;   
    BRAP_FWIF_P_ResponseType        eResponseType=BRAP_FWIF_P_ResponseType_eInvalid;

	BDBG_ENTER(BRAP_EnableTSM_isr);
	BDBG_ASSERT(hRapCh);

    if((hRapCh->eState == BRAP_P_State_eOpened)
        ||(hRapCh->bStopinvoked == true))
    {
/*Initializing all 3 type of TSM userconfig, because at Opentime DecodeMode is not known*/
        if(bEnableTSM)
        {                
            hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.eTsmEnable = BRAP_FWIF_eTsmBool_True;
            hRapCh->sDecSettings.sUserConfigStruct.sPassthruTsmConfigParam.eTsmEnable = BRAP_FWIF_eTsmBool_True;
            hRapCh->sDecSettings.sUserConfigStruct.sEncodeTsmConfigParam.eTsmEnable = BRAP_FWIF_eTsmBool_True; 
        }
        else
        {
            hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.eTsmEnable = BRAP_FWIF_eTsmBool_False;
            hRapCh->sDecSettings.sUserConfigStruct.sPassthruTsmConfigParam.eTsmEnable = BRAP_FWIF_eTsmBool_False;
            hRapCh->sDecSettings.sUserConfigStruct.sEncodeTsmConfigParam.eTsmEnable = BRAP_FWIF_eTsmBool_False;                
        }
    }
    else if(hRapCh->eState == BRAP_P_State_eStarted)
    {
        hDspCh = BRAP_DSPCHN_P_GetDspChHandle_isr(&hRapCh, true);
        if(!(BRAP_P_IsPointerValid((void *)hDspCh)))
        {
    		BDBG_WRN(("BRAP_EnableTSM_isr : Channel already stopped, saving it in Handle"));
            hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.eAstmEnable = bEnableTSM;
            hRapCh->sDecSettings.sUserConfigStruct.sPassthruTsmConfigParam.eAstmEnable = bEnableTSM;
            hRapCh->sDecSettings.sUserConfigStruct.sEncodeTsmConfigParam.eAstmEnable = bEnableTSM;            
            return BERR_NOT_INITIALIZED;
        }
        /* Don't program the TSM for an ES stream. But, do save the user preference
           in the hDspCh->sSettings.bTsmEnable for later use with other stream types 
           during StartDspCxt */
        if(BAVC_StreamType_eEs == hDspCh->sDspAudioParams.sExtAudioParams.eStreamType)
        {
            /* Throw a warning message and return success for an ES stream type */
            BDBG_WRN(("BRAP_EnableTSM_isr: ES stream type can't have TSM! \n Saving "
                "the user preference bEnableTSM = %d for use with other stream types"
                " in future.", bEnableTSM));
            hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.eTsmEnable = bEnableTSM;
            hRapCh->sDecSettings.sUserConfigStruct.sPassthruTsmConfigParam.eTsmEnable = bEnableTSM;
            hRapCh->sDecSettings.sUserConfigStruct.sEncodeTsmConfigParam.eTsmEnable = bEnableTSM;        
            
        	return BERR_SUCCESS;
        }

        err = BRAP_GetTsmNodeInfo_isr(hRapCh,hDspCh,&uiConfigBufAddr,&hFwTask,&psTsmConfigParam);
        if(err != BERR_SUCCESS)
        {
    	     BDBG_WRN(("BRAP_EnableTSM_isr : Channel already stopped, saving it in Handle"));
            hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.eAstmEnable = bEnableTSM;
            hRapCh->sDecSettings.sUserConfigStruct.sPassthruTsmConfigParam.eAstmEnable = bEnableTSM;
            hRapCh->sDecSettings.sUserConfigStruct.sEncodeTsmConfigParam.eAstmEnable = bEnableTSM;        
            return err;
        }

        if(hFwTask->bStopped == true)
        {
            if(bEnableTSM)
            {                
                hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.eTsmEnable = BRAP_FWIF_eTsmBool_True;
                hRapCh->sDecSettings.sUserConfigStruct.sPassthruTsmConfigParam.eTsmEnable = BRAP_FWIF_eTsmBool_True;
                hRapCh->sDecSettings.sUserConfigStruct.sEncodeTsmConfigParam.eTsmEnable = BRAP_FWIF_eTsmBool_True; 
            }
            else
            {
                hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.eTsmEnable = BRAP_FWIF_eTsmBool_False;
                hRapCh->sDecSettings.sUserConfigStruct.sPassthruTsmConfigParam.eTsmEnable = BRAP_FWIF_eTsmBool_False;
                hRapCh->sDecSettings.sUserConfigStruct.sEncodeTsmConfigParam.eTsmEnable = BRAP_FWIF_eTsmBool_False;                
            }            
        }
        else
        {
            if(bEnableTSM)
            {
                psTsmConfigParam->eTsmEnable = BRAP_FWIF_eTsmBool_True;
            }
            else
            {
                psTsmConfigParam->eTsmEnable = BRAP_FWIF_eTsmBool_False;
            }
            eResponseType = BRAP_FWIF_P_ResponseType_eNone;
            err = BRAP_DSPCHN_P_SetTsmConfig_isr(hDspCh,uiConfigBufAddr,hFwTask,psTsmConfigParam,&eResponseType);
            if(BERR_SUCCESS != err)
            {
                BDBG_ERR(("BRAP_EnableTSM_isr: Set Tsm config failed!"));
                return BERR_TRACE(err);
            }
        }
    }       

    BDBG_LEAVE(BRAP_EnableTSM_isr);
	return BERR_SUCCESS;
}

BERR_Code   BRAP_P_AllocateStageStatusBufFromPool(BRAP_Handle hRap,unsigned int *uiBufId)
{
    BERR_Code   ret=BERR_SUCCESS;
    unsigned int i=0;
    for(i = 0 ; i < 2*BRAP_MAX_FW_TSK_SUPPORTED ; i ++)
    {
        if(hRap->sOpenTimeMallocs.bStageStatusBufFree[i] == true)
        {
            *uiBufId = i;
            hRap->sOpenTimeMallocs.bStageStatusBufFree[i] = false;           
            break;
        }
    }
    if(i >= 2*BRAP_MAX_FW_TSK_SUPPORTED)
    {
        ret = BERR_NOT_SUPPORTED;
    }
    return ret;
}


BERR_Code   BRAP_P_ReturnStageStatusBufToPool(BRAP_Handle hRap,unsigned int uiBufId)
{
    BERR_Code   ret=BERR_SUCCESS;
    hRap->sOpenTimeMallocs.bStageStatusBufFree[uiBufId] = true;           
    return ret;
}


/***************************************************************************
Summary: Get stream Information

Description:
	This function returns stream information of the decoded stream, if
	such information is available (i.e. DSP firmware has decoded atleast
	one audio frame and initialized stream information registers).
	Otherwise it returns error.
	Note: This API is valid only for a decode channel.
	
Returns:
	BERR_SUCCESS - If successful

See Also:
**************************************************************************/

BERR_Code
BRAP_GetProcessingInformation(
	BRAP_ProcessingStageHandle hAudioProcessingStageHandle, /* [in] Processing Stage handle */
	BRAP_DSPCHN_ProcessingInfo *psProcessingInfo	        /* [out]Processing Information */
	)
{
    BERR_Code err = BERR_SUCCESS;
    BRAP_DSPCHN_Handle hDspCh = NULL;
    unsigned int uiTaskIndex = 0, uiBranchId = 0, uiStageId = 0, uiNode = 0;
    unsigned int uiNodeIndex, uiStatusBufAddr;    
    BRAP_P_ChannelAudioProcessingStage  *psTempAudProcStage = NULL;
    BRAP_FWIF_P_FwTaskHandle hFwTask = NULL;
    BRAP_CIT_P_OpStgInfo *psOpStgInfo = NULL;
    
	BRAP_Handle hRap = NULL;
    BRAP_DSP_Handle hDsp = NULL;
    unsigned int uiCitBranchId = BRAP_INVALID_VALUE, uiCitStageId = BRAP_INVALID_VALUE;
	unsigned int uiTaskId = BRAP_INVALID_VALUE;
    BRAP_Fwif_P_FwBufInfo  *psFwBufInfo = NULL;
    
    
    BDBG_ENTER(BRAP_GetProcessingInformation);
	BDBG_ASSERT(hAudioProcessingStageHandle);
	BDBG_ASSERT(psProcessingInfo);

    /* Allocate BRAP_P_ChannelAudioProcessingStage handle */
    psTempAudProcStage = (BRAP_P_ChannelAudioProcessingStage *)BKNI_Malloc(sizeof(BRAP_P_ChannelAudioProcessingStage));
    if(psTempAudProcStage == NULL)
    {
        err = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        goto end;
    }
    BKNI_Memset( psTempAudProcStage, 0, sizeof(BRAP_P_ChannelAudioProcessingStage) );    

    if(true == hAudioProcessingStageHandle->bChSpecificStage)  
    {
        /*Post processing stage is added before mixer*/
        if((BRAP_P_IsPointerValid((void *)hAudioProcessingStageHandle->uHandle.hRapCh))
            &&(hAudioProcessingStageHandle->uHandle.hRapCh->eState ==  BRAP_P_State_eStarted))
        {
            /* The channel is started */
            hDspCh = BRAP_DSPCHN_P_GetDspChHandle(&(hAudioProcessingStageHandle->uHandle.hRapCh),
                                                    hAudioProcessingStageHandle->bChSpecificStage);
            if(!(BRAP_P_IsPointerValid((void *)hDspCh)))
            {
                BDBG_ERR(("BRAP_GetProcessingInformation: Error!!! DspCh Can't be NULL"));
                err = BERR_TRACE(BRAP_ERR_BAD_DEVICE_STATE);
                goto end;
            }

            /* Find the exact place where this Post Processing is getting executed */
            for (uiTaskIndex = 0; uiTaskIndex < BRAP_FWIF_P_MAX_FW_TASK_PER_DSPCHN; uiTaskIndex++)
            {
                for (uiBranchId = 0; uiBranchId < BRAP_P_MAX_DST_PER_RAPCH; uiBranchId++)
                {
                    for (uiStageId = 0; uiStageId < BRAP_MAX_STAGE_PER_BRANCH_SUPPORTED; uiStageId++)
                    {
                        (*psTempAudProcStage) = hDspCh->sFwTaskInfo[uiTaskIndex].sProcessingNw.sAudProcessingStage[uiBranchId][uiStageId];

                        if(BRAP_P_IsPointerValid((void *)psTempAudProcStage->hAudioProcessing))
                        {
                            if (psTempAudProcStage->hAudioProcessing == hAudioProcessingStageHandle)
                            {
                                /* If the processing handle matches, then find the processing info */

                                /* Get the Status buffer params */
								hRap = hAudioProcessingStageHandle->hRap;
	                            psFwBufInfo = &(hRap->sMemInfo.sConfigParamBufInfo);
	                            hFwTask = hDspCh->sFwTaskInfo[uiTaskIndex].hFwTask;
	                            uiTaskId = hFwTask->uiTaskId;
	                            hDsp = hFwTask->hDsp;
	                            uiCitBranchId = hDspCh->sFwTaskInfo[uiTaskIndex].sProcessingNw.sAudProcessingStage[uiBranchId][uiStageId].ui32CitBranchId;
	                            uiCitStageId = hDspCh->sFwTaskInfo[uiTaskIndex].sProcessingNw.sAudProcessingStage[uiBranchId][uiStageId].ui32CitStageId;

	                            if((BRAP_INVALID_VALUE == uiCitBranchId)
	                                ||(BRAP_INVALID_VALUE == uiCitStageId))
	                            {
	                                BDBG_ERR(("BRAP_GetProcessingInformation: Error Cit Branch/Stage Id corresponding to Processing stage can't be invalid"));
	                                err = BERR_TRACE(BERR_INVALID_PARAMETER);
	                                goto end;
	                            }

	                            psOpStgInfo = &(hFwTask->sCitOutput.sCitBranchInfo[uiCitBranchId].sCitStgInfo[uiCitStageId]);                                

                                for (uiNode = 0; uiNode < psOpStgInfo->ui32NumNodes; uiNode ++)
                                {
                                	switch (hAudioProcessingStageHandle->sProcessingStageSettings.eAudioProcessing) 
                                    {
                                        case BRAP_ProcessingType_eCustomVoice:
                                            if (BRAP_AF_P_AlgoId_eCustomVoicePostProc == psOpStgInfo->sFwOpNodeInfo[uiNode].eFwExecId)
                                            {
                                                uiNodeIndex = uiNode;
                                                uiStatusBufAddr = psOpStgInfo->sFwOpNodeInfo[uiNodeIndex].ui32StatusBufAdr;

                                                if((uiStatusBufAddr == BRAP_P_INVALID_DRAM_ADDRESS)||
                                                    (uiStatusBufAddr == BRAP_AF_P_DRAM_ADDR_INVALID))
                                                {
                                                    BDBG_ERR(("BRAP_GetProcessingInformation: Status buffer is not present for this node 0x%x",
                                                                uiStatusBufAddr));
                                                    err = BERR_SUCCESS;
                                                    goto end;
                                                }

                                                psProcessingInfo->eProcessingType = hAudioProcessingStageHandle->sProcessingStageSettings.eAudioProcessing;                                

                                                if(psOpStgInfo->sFwOpNodeInfo[uiNodeIndex].ui32StatusBufSize < 
                                                        sizeof(BRAP_FWIF_P_CustomVoiceStatusInfo))
                                                {
                                                    BDBG_ERR(("BRAP_GetProcessingInformation: Status buffer size invalid 0x%x",
                                                                sizeof(BRAP_FWIF_P_CustomVoiceStatusInfo)));
                                                    BDBG_ASSERT(0);
                                                    err = BERR_TRACE(BRAP_ERR_BUFFER_INVALID);
                                                    goto end;
                                                }

                                                err = BRAP_DSPCHN_P_GetCustomVoiceInfo (hDspCh,uiStatusBufAddr,psProcessingInfo);
                                                if(err != BERR_SUCCESS)
                                                {
                                                    BDBG_ERR (("BRAP_GetProcessingInformation: ERROR from cusotm voice info call"));
                                                    goto end;
                                                }                                
                                            }
                                            
                                            break;


                                        case BRAP_ProcessingType_eAudysseyVolume:
                                            if (BRAP_AF_P_AlgoId_eAudysseyVolumePostProc == psOpStgInfo->sFwOpNodeInfo[uiNode].eFwExecId)
                                            {
                                                uiNodeIndex = uiNode;
                                                uiStatusBufAddr = psOpStgInfo->sFwOpNodeInfo[uiNodeIndex].ui32StatusBufAdr;

                                                if((uiStatusBufAddr == BRAP_P_INVALID_DRAM_ADDRESS)||
                                                    (uiStatusBufAddr == BRAP_AF_P_DRAM_ADDR_INVALID))
                                                {
                                                    BDBG_ERR(("BRAP_GetProcessingInformation: Status buffer is not present for this node 0x%x",
                                                                uiStatusBufAddr));
                                                    err = BERR_SUCCESS;
                                                    goto end;
                                                }

                                                psProcessingInfo->eProcessingType = hAudioProcessingStageHandle->sProcessingStageSettings.eAudioProcessing;                                

                                                if(psOpStgInfo->sFwOpNodeInfo[uiNodeIndex].ui32StatusBufSize < 
                                                        sizeof(BRAP_FWIF_P_AudysseyVolStatusInfo))
                                                {
                                                    BDBG_ERR(("BRAP_GetProcessingInformation: Status buffer size invalid 0x%x",
                                                                sizeof(BRAP_FWIF_P_AudysseyVolStatusInfo)));
                                                    BDBG_ASSERT(0);
                                                    err = BERR_TRACE(BRAP_ERR_BUFFER_INVALID);
                                                    goto end;
                                                }

                                                err = BRAP_DSPCHN_P_GetAudysseyVolumeInfo (hDspCh,uiStatusBufAddr,psProcessingInfo);
                                                if(err != BERR_SUCCESS)
                                                {
                                                    BDBG_ERR (("BRAP_GetProcessingInformation: ERROR from audyssey volume info call"));
                                                    goto end;
                                                }                                
                                            }
                                            
                                            break;                                            

                                        default:
                                			BDBG_ERR(("BRAP_GetProcessingInformation: Audio type not supported"));
                                			err = BERR_TRACE(BERR_INVALID_PARAMETER);
                                            goto end;
                            	    }                        
                                }
                            }
                        }
                    }
                }
            }
        }
    }            

end:

    if (psTempAudProcStage != NULL)
    {
        BKNI_Free(psTempAudProcStage);
        psTempAudProcStage = NULL;
    }
    
    BDBG_LEAVE(BRAP_GetProcessingInformation);
    return err;
}

static BERR_Code
BRAP_DSPCHN_P_GetCustomVoiceInfo(
    BRAP_DSPCHN_Handle          hDspCh,
	unsigned int	            uiStatusBufAddr,
	BRAP_DSPCHN_ProcessingInfo *psProcessingInfo
	)
{
    BERR_Code err = BERR_SUCCESS;
    BRAP_FWIF_P_StageStatus *psStageStatus=NULL;
	uint32_t                i=0; 
	uint32_t                *bufPtr=NULL; 
    unsigned int uiBufId = BRAP_INVALID_VALUE;

    BDBG_ENTER (BRAP_DSPCHN_P_GetCustomVoiceInfo);

     err = BRAP_P_AllocateStageStatusBufFromPool(hDspCh->uHandle.hRapCh->hRap,&uiBufId);
     if(err != BERR_SUCCESS)
    {
         BDBG_ERR(("Out of Buffer for StageStausBuffer. Function: %sLINE = %d",__FUNCTION__,__LINE__));
        goto end;
    }

     psStageStatus = hDspCh->uHandle.hRapCh->hRap->sOpenTimeMallocs.psStageStatus[uiBufId];
     BKNI_Memset( psStageStatus, 0, sizeof(BRAP_FWIF_P_StageStatus));    
    
    bufPtr =(uint32_t *)psStageStatus;
    for(i=0; i<sizeof(BRAP_FWIF_P_CustomVoiceStatusInfo)/4; i++)
    {
        *(bufPtr+i) = (uint32_t)BRAP_MemRead32(uiStatusBufAddr+i*4);
    }
    if(psStageStatus->sProcessingStage.sCustomVoiceStatusInfo.ui32StatusValid != 0)
    {
        BDBG_ERR (("BRAP_DSPCHN_P_GetCustomVoiceInfo: Status buffer is not in valid status"));
        err = (BRAP_ERR_BAD_DEVICE_STATE);
        goto end;        
    }

	psProcessingInfo->eProcessingType = BRAP_ProcessingType_eCustomVoice;
	
	psProcessingInfo->uProcessingInfo.sCustomVoiceInfo.i32PVCMonitorLevel = 
        psStageStatus->sProcessingStage.sCustomVoiceStatusInfo.i32PVCMonitorLevel;


    BDBG_MSG (("Get Processing Info :: Custom Voice"));
    BDBG_MSG (("-----------------------------------"));
    BDBG_MSG (("eProcessingType = %d",psProcessingInfo->eProcessingType));
    BDBG_MSG (("i32PVCMonitorLevel = %u",psProcessingInfo->uProcessingInfo.sCustomVoiceInfo.i32PVCMonitorLevel));

    BDBG_LEAVE (BRAP_DSPCHN_P_GetCustomVoiceInfo);
end:

    if(uiBufId != BRAP_INVALID_VALUE)
        BRAP_P_ReturnStageStatusBufToPool(hDspCh->uHandle.hRapCh->hRap,uiBufId);       
    return err;
}






BERR_Code
BRAP_DSPCHN_P_GetAudysseyVolumeInfo(
    BRAP_DSPCHN_Handle          hDspCh,
	unsigned int	            uiStatusBufAddr,
	BRAP_DSPCHN_ProcessingInfo *psProcessingInfo
	)
{
    BRAP_FWIF_P_StageStatus sStateStatus;
	uint32_t                i=0; 
	uint32_t                *bufPtr=NULL; 

    BDBG_ENTER (BRAP_DSPCHN_P_GetAudysseyVolumeInfo);
    BSTD_UNUSED(hDspCh);

    bufPtr =(uint32_t *) &sStateStatus;
    for(i=0; i<sizeof(BRAP_FWIF_P_AudysseyVolStatusInfo)/4; i++)
    {
        *(bufPtr+i) = (uint32_t)BRAP_MemRead32(uiStatusBufAddr+i*4);
    }
    if(sStateStatus.sProcessingStage.sAudysseyVolStatusInfo.ui32StatusValid != 0)
    {
        BDBG_ERR (("BRAP_DSPCHN_P_GetAudysseyVolumeInfo: Status buffer is not in valid status"));
        return BRAP_ERR_BAD_DEVICE_STATE;
    }

	psProcessingInfo->eProcessingType = BRAP_ProcessingType_eAudysseyVolume;

    for (i=0; i<8; i++)
    {
    	psProcessingInfo->uProcessingInfo.sAuddseyVolumeInfo.i32chCalcLevel[i] = 
            sStateStatus.sProcessingStage.sAudysseyVolStatusInfo.i32chCalcLevel[i];

    	psProcessingInfo->uProcessingInfo.sAuddseyVolumeInfo.i32chCalbGain[i] = 
            sStateStatus.sProcessingStage.sAudysseyVolStatusInfo.i32chCalbGain[i];
        
    }

    psProcessingInfo->uProcessingInfo.sAuddseyVolumeInfo.i32Vol_Applied_dB = 
        sStateStatus.sProcessingStage.sAudysseyVolStatusInfo.i32Vol_Applied_dB;


    psProcessingInfo->uProcessingInfo.sAuddseyVolumeInfo.i32SwDynEQ = 
        sStateStatus.sProcessingStage.sAudysseyVolStatusInfo.i32SwDynEQ;

    psProcessingInfo->uProcessingInfo.sAuddseyVolumeInfo.ui32NumChannels = 
        sStateStatus.sProcessingStage.sAudysseyVolStatusInfo.ui32NumChannels;

    psProcessingInfo->uProcessingInfo.sAuddseyVolumeInfo.ui32AudysseyVolBypass = 
        sStateStatus.sProcessingStage.sAudysseyVolStatusInfo.ui32AudysseyVolBypass;

    psProcessingInfo->uProcessingInfo.sAuddseyVolumeInfo.i32ChannelMask = 
        sStateStatus.sProcessingStage.sAudysseyVolStatusInfo.i32ChannelMask;


    psProcessingInfo->uProcessingInfo.sAuddseyVolumeInfo.i32AudysseyVolInit = 
        sStateStatus.sProcessingStage.sAudysseyVolStatusInfo.i32AudysseyVolInit;


    psProcessingInfo->uProcessingInfo.sAuddseyVolumeInfo.i32HeadroomOffset = 
        sStateStatus.sProcessingStage.sAudysseyVolStatusInfo.i32HeadroomOffset;

    psProcessingInfo->uProcessingInfo.sAuddseyVolumeInfo.i32SwAudysseyVol = 
        sStateStatus.sProcessingStage.sAudysseyVolStatusInfo.i32SwAudysseyVol;


    psProcessingInfo->uProcessingInfo.sAuddseyVolumeInfo.i32SwDynSurrGain = 
        sStateStatus.sProcessingStage.sAudysseyVolStatusInfo.i32SwDynSurrGain;


    psProcessingInfo->uProcessingInfo.sAuddseyVolumeInfo.i32SwHiLoCmpress = 
        sStateStatus.sProcessingStage.sAudysseyVolStatusInfo.i32SwHiLoCmpress;


    psProcessingInfo->uProcessingInfo.sAuddseyVolumeInfo.i32dBVolSetting = 
        sStateStatus.sProcessingStage.sAudysseyVolStatusInfo.i32dBVolSetting;


    psProcessingInfo->uProcessingInfo.sAuddseyVolumeInfo.i32GCF = 
        sStateStatus.sProcessingStage.sAudysseyVolStatusInfo.i32GCF;
    
    
    BDBG_LEAVE (BRAP_DSPCHN_P_GetAudysseyVolumeInfo);
    return BERR_SUCCESS;
}

BERR_Code
BRAP_GetStreamInformation(
	BRAP_ChannelHandle	    hRapCh,		    /* [in] Audio channel handle */
	BRAP_DSPCHN_StreamInfo  *psStreamInfo	/* [out] Stream Information */
	)
{
	BRAP_DSPCHN_Handle hDspCh;
	BRAP_DSPCHN_AudioType eAudioType;
    BERR_Code err = BERR_SUCCESS;
    unsigned int uiTaskIndex = 0, uiBranchId = 0, uiStageId = 0;
    unsigned int uiStartNodeIndex, uiStatusBufAddr;
    bool bDecStageFound = false;
    BRAP_CIT_P_OpStgInfo *psOpStgInfo;
    BRAP_FWIF_P_FwTaskHandle hFwTask;

    BRAP_P_ChannelAudioProcessingStage  sTempAudProcStage;
    unsigned int uiTempBranchId = 0;
    bool bBranchCloned = false;
    bool bIsFwBranch =false;    

    BDBG_ENTER(BRAP_GetStreamInformation);
	BDBG_ASSERT(hRapCh);
	BDBG_ASSERT(psStreamInfo);

    if (BRAP_P_State_eStarted != hRapCh->eState)    
    {
        BDBG_ERR(("Can get stream info only after the channel has been started."
        "This channel [hRapCh=0x%8x] is currently stopped.", hRapCh));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    hDspCh = BRAP_DSPCHN_P_GetDspChHandle(&hRapCh, true);
    if(!(BRAP_P_IsPointerValid((void *)hDspCh)))
    {
        return BERR_TRACE(BERR_NOT_INITIALIZED);
    }


    /* Find out the task in which the decode stage is getting executed.
     Also find out the exact position of the decode stage within audio
     processing network. */
  for (uiTaskIndex = 0; uiTaskIndex < BRAP_FWIF_P_MAX_FW_TASK_PER_DSPCHN; uiTaskIndex++)
    {
        uiTempBranchId = 0;
        for (uiBranchId = 0; uiBranchId < BRAP_P_MAX_DST_PER_RAPCH; uiBranchId++)
        {
            bIsFwBranch = false;
            for (uiStageId = 0; uiStageId < BRAP_MAX_STAGE_PER_BRANCH_SUPPORTED; uiStageId++)
            {
                sTempAudProcStage = hDspCh->sFwTaskInfo[uiTaskIndex].sProcessingNw.sAudProcessingStage[uiBranchId][uiStageId];
                if (true== sTempAudProcStage.bDecoderStage)
                {
                    bDecStageFound = true;
                    break;
                }
                else if(((false == sTempAudProcStage.bCloneStage) && (NULL == sTempAudProcStage.hAudioProcessing)
                                && (true == hDspCh->sFwTaskInfo[uiTaskIndex].sProcessingNw.sAudProcessingStage[uiBranchId][uiStageId -1].bCloneStage))
                            ||((uiStageId == BRAP_MAX_STAGE_PER_BRANCH_SUPPORTED -1) && (true == sTempAudProcStage.bCloneStage)) /* Check if the last valid stage of the branch has bCloneStage == true. In that case output port cloning is happening.*/
                          ) 
                {
                    if(uiStageId == BRAP_MAX_STAGE_PER_BRANCH_SUPPORTED -1) /*If stageId is equal to MAX Stage ID*/
                    {
                        BRAP_FWIF_P_IsBranchCloned(hRapCh,&(hDspCh->sFwTaskInfo[uiTaskIndex].sProcessingNw),uiBranchId,uiStageId,&bBranchCloned,&bIsFwBranch);                
                    }
                    else
                    {
                        BRAP_FWIF_P_IsBranchCloned(hRapCh,&(hDspCh->sFwTaskInfo[uiTaskIndex].sProcessingNw),uiBranchId,uiStageId-1,&bBranchCloned,&bIsFwBranch);                
                    }                        
                    break;  /* Break from the FOR loop beacause last valid stage of the Branch */
                }
            }
            if (true==bDecStageFound) break;
            
            if(true == bIsFwBranch)
            {
                uiTempBranchId++;
            }
        }
        if (true==bDecStageFound) break;
    }
    if (false==bDecStageFound)
    {
        BDBG_ERR(("BRAP_GetStreamInformation: Could not find decoder stage in the network of audio processing stages."));
        return BERR_TRACE(BRAP_ERR_BAD_DEVICE_STATE);
    }

    if(uiTempBranchId >= BRAP_P_MAX_FW_BRANCH_PER_FW_TASK)
    {
        BDBG_ERR(("BRAP_GetStreamInformation: Branch id (%d) is exceeding the BRAP_P_MAX_FW_BRANCH_PER_FW_TASK =%d",
                    uiTempBranchId ,BRAP_P_MAX_FW_BRANCH_PER_FW_TASK));
        return BERR_TRACE(BRAP_ERR_BAD_DEVICE_STATE);
    }
    /* Get the Status buffer params */
    hFwTask = hDspCh->sFwTaskInfo[uiTaskIndex].hFwTask;
    psOpStgInfo = &(hFwTask->sCitOutput.sCitBranchInfo[uiTempBranchId].sCitStgInfo[uiStageId]);
    uiStartNodeIndex = psOpStgInfo->ui32StartNodeIndex;
    uiStatusBufAddr = psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32StatusBufAdr;
	eAudioType = hDspCh->sDspAudioParams.sExtAudioParams.eType;
    psStreamInfo->eType = eAudioType;
    psStreamInfo->sFrameDecAndErrInfo.eFrameErrorStatus = BRAP_DSPCHN_FrameErrorStatus_Sane;
    psStreamInfo->sFrameDecAndErrInfo.ui32NumSamplesDecoded = BRAP_INVALID_VALUE;    
    
    if((uiStatusBufAddr == BRAP_P_INVALID_DRAM_ADDRESS)||
        (uiStatusBufAddr == BRAP_AF_P_DRAM_ADDR_INVALID))
    {
        BDBG_ERR(("BRAP_GetStreamInformation: Status buffer is not present for this node 0x%x",
                    uiStatusBufAddr));
/*        BDBG_ASSERT(0);
        return BERR_TRACE(BRAP_ERR_BUFFER_INVALID);*/
        return BERR_SUCCESS;
    }

	switch (eAudioType) {
		case BRAP_DSPCHN_AudioType_eMpeg:
            if(psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32StatusBufSize < 
                    sizeof(BRAP_FWIF_P_MpegStreamInfo))
            {
                BDBG_ERR(("BRAP_GetStreamInformation: Status buffer size invalid 0x%x",
                            sizeof(BRAP_FWIF_P_MpegStreamInfo)));
                return BERR_TRACE(BRAP_ERR_BUFFER_INVALID);
            }
        	err = BRAP_DSPCHN_P_GetMpegInfo(hDspCh,uiStatusBufAddr, psStreamInfo);
            if(err != BERR_SUCCESS)
            {
                return err;
            }
			break;
#if 1            
		case BRAP_DSPCHN_AudioType_eAc3Plus:
		case BRAP_DSPCHN_AudioType_eAc3:          
            if(psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32StatusBufSize < 
                    sizeof(BRAP_FWIF_P_MultiStreamDDPStreamInfo))
            {
                BDBG_ERR(("BRAP_GetStreamInformation: Status buffer size invalid 0x%x",
                            sizeof(BRAP_FWIF_P_MultiStreamDDPStreamInfo)));
                return BERR_TRACE(BRAP_ERR_BUFFER_INVALID);
            }
		    err = BRAP_DSPCHN_P_GetDdpMultiStreamInfo(hDspCh,uiStatusBufAddr, psStreamInfo);
            if(err != BERR_SUCCESS)
            {
                return err;
            }
            
			break;
#else
		case BRAP_DSPCHN_AudioType_eAc3Plus:
            if(psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32StatusBufSize < 
                    sizeof(BRAP_FWIF_P_DdpStreamInfo))
            {
                BDBG_ERR(("BRAP_GetStreamInformation: Status buffer size invalid 0x%x",
                            sizeof(BRAP_FWIF_P_DdpStreamInfo)));
                return BERR_TRACE(BRAP_ERR_BUFFER_INVALID);
            }
		err  =BRAP_DSPCHN_P_GetDdpInfo(hDspCh,uiStatusBufAddr, psStreamInfo);
            if(err != BERR_SUCCESS)
            {
                return err;
            }
            
			break;
		case BRAP_DSPCHN_AudioType_eAc3:
            if(psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32StatusBufSize < 
                    sizeof(BRAP_FWIF_P_Ac3StreamInfo))
            {
                BDBG_ERR(("BRAP_GetStreamInformation: Exec Id 0x%x",
                            psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].eFwExecId));
                BDBG_ERR(("BRAP_GetStreamInformation: Status buffer size given %d 0x%x",uiStartNodeIndex,
                            psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32StatusBufSize));
                BDBG_ERR(("BRAP_GetStreamInformation: Status buffer size invalid 0x%x",
                            sizeof(BRAP_FWIF_P_Ac3StreamInfo)));
                return BERR_TRACE(BRAP_ERR_BUFFER_INVALID);
            }
		err = BRAP_DSPCHN_P_GetAc3Info(hDspCh,uiStatusBufAddr, psStreamInfo);
            if(err != BERR_SUCCESS)
            {
                return err;
            }
            
			break;
#endif            
            case BRAP_DSPCHN_AudioType_eAac:
            case BRAP_DSPCHN_AudioType_eAacLoas:                        
            case BRAP_DSPCHN_AudioType_eAacSbr:
            case BRAP_DSPCHN_AudioType_eAacSbrAdts:
#ifdef RAP_MULTISTREAM_DECODER_SUPPORT                
            if(psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32StatusBufSize < 
                    sizeof(BRAP_FWIF_P_DolbyPulseStreamInfo))
            {
                BDBG_ERR(("BRAP_GetStreamInformation: Status buffer size invalid 0x%x",
                            sizeof(BRAP_FWIF_P_DolbyPulseStreamInfo)));
                return BERR_TRACE(BRAP_ERR_BUFFER_INVALID);
            }
			err = BRAP_DSPCHN_P_GetDolbyPulseInfo(hDspCh,uiStatusBufAddr, psStreamInfo);
            if(err != BERR_SUCCESS)
            {
                return err;
            }       
#else                    
            if(psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32StatusBufSize < 
                    sizeof(BRAP_FWIF_P_AacheStreamInfo))
            {
                BDBG_ERR(("BRAP_GetStreamInformation: Status buffer size invalid 0x%x",
                            sizeof(BRAP_FWIF_P_AacheStreamInfo)));
                return BERR_TRACE(BRAP_ERR_BUFFER_INVALID);
            }
			err = BRAP_DSPCHN_P_GetAacheInfo(hDspCh,uiStatusBufAddr, psStreamInfo);
            if(err != BERR_SUCCESS)
            {
                return err;
            }            
#endif            
			break;
    		case BRAP_DSPCHN_AudioType_eWmaStd:
                    if(psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32StatusBufSize < 
                            sizeof(BRAP_FWIF_P_WmaStreamInfo))
                    {
                        BDBG_ERR(("BRAP_GetStreamInformation: Status buffer size allocated %d is less than required %d",
                                    psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32StatusBufSize,sizeof(BRAP_FWIF_P_WmaStreamInfo)));
                        return BERR_TRACE(BRAP_ERR_BUFFER_INVALID);
                    }
			err = BRAP_DSPCHN_P_GetWmaStdInfo(hDspCh,uiStatusBufAddr, psStreamInfo);
                    if(err != BERR_SUCCESS)
                    {
                        return err;
                    }
            
			break;
    		case BRAP_DSPCHN_AudioType_eWmaPro:
                    if(psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32StatusBufSize < 
                            sizeof(BRAP_FWIF_P_WmaProStreamInfo))
                    {
                        BDBG_ERR(("BRAP_GetStreamInformation: Status buffer size allocated %d is less than required %d",
                                    psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32StatusBufSize,sizeof(BRAP_FWIF_P_WmaProStreamInfo)));
                        return BERR_TRACE(BRAP_ERR_BUFFER_INVALID);
                    }
			err =BRAP_DSPCHN_P_GetWmaProInfo(hDspCh,uiStatusBufAddr, psStreamInfo);
                    if(err != BERR_SUCCESS)
                    {
                        return err;
                    }            
			break;
    		case BRAP_DSPCHN_AudioType_eLpcmDvd:
                    if(psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32StatusBufSize < 
                            sizeof(BRAP_FWIF_P_LpcmStreamInfo))
                    {
                        BDBG_ERR(("BRAP_GetStreamInformation: Status buffer size allocated %d is less than required %d",
                                    psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32StatusBufSize,sizeof(BRAP_FWIF_P_LpcmStreamInfo)));
                        return BERR_TRACE(BRAP_ERR_BUFFER_INVALID);
                    }
			err =BRAP_DSPCHN_P_GetLpcmDvdInfo(hDspCh,uiStatusBufAddr, psStreamInfo);
                    if(err != BERR_SUCCESS)
                    {
                        return err;
                    }            
			break;
    		case BRAP_DSPCHN_AudioType_eDtsBroadcast:
                    if(psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32StatusBufSize < 
                            sizeof(BRAP_FWIF_P_DtsHdStreamInfo))
                    {
                        BDBG_ERR(("BRAP_GetStreamInformation: Status buffer size allocated %d is less than required %d",
                                    psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32StatusBufSize,sizeof(BRAP_FWIF_P_DtsHdStreamInfo)));
                        return BERR_TRACE(BRAP_ERR_BUFFER_INVALID);
                    }
			err =BRAP_DSPCHN_P_GetDtsBroadcastInfo(hDspCh,uiStatusBufAddr, psStreamInfo);
                    if(err != BERR_SUCCESS)
                    {
                        return err;
                    }            
			break;
    		case BRAP_DSPCHN_AudioType_eDtshd:
                    if(psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32StatusBufSize < 
                            sizeof(BRAP_FWIF_P_DtsHdStreamInfo))
                    {
                        BDBG_ERR(("BRAP_GetStreamInformation: Status buffer size allocated %d is less than required %d",
                                    psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32StatusBufSize,sizeof(BRAP_FWIF_P_DtsHdStreamInfo)));
                        return BERR_TRACE(BRAP_ERR_BUFFER_INVALID);
                    }
			        err =BRAP_DSPCHN_P_GetDtsHdInfo(hDspCh,uiStatusBufAddr, psStreamInfo);
                    if(err != BERR_SUCCESS)
                    {
                        return err;
                    }            
			break;            
    		case BRAP_DSPCHN_AudioType_ePcmWav:
                    if(psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32StatusBufSize < 
                            sizeof(BRAP_FWIF_P_PcmWavStreamInfo))
                    {
                        BDBG_ERR(("BRAP_GetStreamInformation: Status buffer size allocated %d is less than required %d",
                                    psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32StatusBufSize,sizeof(BRAP_FWIF_P_PcmWavStreamInfo)));
                        return BERR_TRACE(BRAP_ERR_BUFFER_INVALID);
                    }
			err =BRAP_DSPCHN_P_GetPcmWavInfo(hDspCh,uiStatusBufAddr, psStreamInfo);
                    if(err != BERR_SUCCESS)
                    {
                        return err;
                    }            
			break;                
    		case BRAP_DSPCHN_AudioType_eAmr:
                    if(psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32StatusBufSize < 
                            sizeof(BRAP_FWIF_P_AmrStreamInfo))
                    {
                        BDBG_ERR(("BRAP_GetStreamInformation: Status buffer size allocated %d is less than required %d",
                                    psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32StatusBufSize,sizeof(BRAP_FWIF_P_AmrStreamInfo)));
                        return BERR_TRACE(BRAP_ERR_BUFFER_INVALID);
                    }
			        err =BRAP_DSPCHN_P_GetAmrInfo(hDspCh,uiStatusBufAddr, psStreamInfo);
                    if(err != BERR_SUCCESS)
                    {
                        return err;
                    }            
			break; 
    		case BRAP_DSPCHN_AudioType_eDra:
                    if(psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32StatusBufSize <  sizeof(BRAP_FWIF_P_DraStreamInfo))
                    {
                        BDBG_ERR(("BRAP_GetStreamInformation: Status buffer for DRA ,  size allocated %d is less than required %d",
                        psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32StatusBufSize,sizeof(BRAP_FWIF_P_DraStreamInfo)));
                        return BERR_TRACE(BRAP_ERR_BUFFER_INVALID);
                    }
                    
                    err =BRAP_DSPCHN_P_GetDraInfo(hDspCh,uiStatusBufAddr, psStreamInfo);
                    
                    if(err != BERR_SUCCESS)
                    {
                        return err;
                    }            
                    break; 
		case BRAP_DSPCHN_AudioType_eRealAudioLbr:
            if(psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32StatusBufSize <  sizeof(BRAP_FWIF_P_RalbrStreamInfo))
            {
                BDBG_ERR(("BRAP_GetStreamInformation: Status buffer for Real Audio LBR,  size allocated %d is less than required %d",
                psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32StatusBufSize,sizeof(BRAP_FWIF_P_RalbrStreamInfo)));
                return BERR_TRACE(BRAP_ERR_BUFFER_INVALID);
            }
            
            err = BRAP_DSPCHN_P_GetRealAudioLbrInfo(hDspCh,uiStatusBufAddr, psStreamInfo);
                    
            if(err != BERR_SUCCESS)
            {
                return err;
            }            
            break;     
		default: 
			BDBG_ERR(("BRAP_GetStreamInformation: Audio type not supported"));
			return BERR_TRACE(BERR_INVALID_PARAMETER);
	}
    
        psStreamInfo->eType = eAudioType;
    BDBG_LEAVE(BRAP_GetStreamInformation);
	return err;
}


static BERR_Code
BRAP_DSPCHN_P_GetMpegInfo(
    BRAP_DSPCHN_Handle      hDspCh,
	unsigned int	        uiStatusBufAddr,
	BRAP_DSPCHN_StreamInfo *psStreamInfo
	)
{
    BRAP_FWIF_P_StageStatus *psStageStatus=NULL;
    uint32_t                i=0; 
    uint32_t           *bufPtr=NULL; 
    BERR_Code   err=BERR_SUCCESS;
    unsigned   ui32Mode;

    unsigned int uiBufId = BRAP_INVALID_VALUE;
     err = BRAP_P_AllocateStageStatusBufFromPool(hDspCh->uHandle.hRapCh->hRap,&uiBufId);
     if(err != BERR_SUCCESS)
    {
         BDBG_ERR(("Out of Buffer for StageStausBuffer. Function: %sLINE = %d",__FUNCTION__,__LINE__));
        goto end;
    }

     psStageStatus = hDspCh->uHandle.hRapCh->hRap->sOpenTimeMallocs.psStageStatus[uiBufId];
     BKNI_Memset( psStageStatus, 0, sizeof(BRAP_FWIF_P_StageStatus));    

    bufPtr =(uint32_t *) psStageStatus;
    for(i=0; i<sizeof(BRAP_FWIF_P_MpegStreamInfo)/4; i++)
    {
        *(bufPtr+i) = (uint32_t)BRAP_MemRead32(uiStatusBufAddr+i*4);
    }
    if(0 != psStageStatus->sDecodeStage.sStreamInfo.sMpegStreamInfo.ui32StatusValid)
    {
        BDBG_MSG(("BRAP_DSPCHN_P_GetMpegInfo: Status buffer is not in valid status"));
        err = (BRAP_ERR_BAD_DEVICE_STATE);
        goto end;        
    }
    
	psStreamInfo->eType = BRAP_DSPCHN_AudioType_eMpeg;
	
	psStreamInfo->uStreamInfo.sMpegInfo.bOriginal = 
        psStageStatus->sDecodeStage.sStreamInfo.sMpegStreamInfo.ui32OriginalCopy;

	psStreamInfo->uStreamInfo.sMpegInfo.bCopyRight = 
        psStageStatus->sDecodeStage.sStreamInfo.sMpegStreamInfo.ui32Copyright;
	
	psStreamInfo->uStreamInfo.sMpegInfo.bCRCPresent = 
        psStageStatus->sDecodeStage.sStreamInfo.sMpegStreamInfo.ui32CrcPresent;

        if(hDspCh->uHandle.hRapCh->uiModeValue != BRAP_INVALID_VALUE)
        {
            ui32Mode = hDspCh->uHandle.hRapCh->uiModeValue;
        }
        else
        {
            ui32Mode = psStageStatus->sDecodeStage.sStreamInfo.sMpegStreamInfo.ui32Mode;
        }

    	switch(ui32Mode)
        {
            case 0:
        	psStreamInfo->uStreamInfo.sMpegInfo.eChmod  = BRAP_DSPCHN_MpegChannelMode_eStereo;
             break;
            case 1:
        	psStreamInfo->uStreamInfo.sMpegInfo.eChmod  = BRAP_DSPCHN_MpegChannelMode_eIntensityStereo;
             break;     
            case 2:
        	psStreamInfo->uStreamInfo.sMpegInfo.eChmod  = BRAP_DSPCHN_MpegChannelMode_eDualChannel;
             break;  
            case 3:
        	psStreamInfo->uStreamInfo.sMpegInfo.eChmod  = BRAP_DSPCHN_MpegChannelMode_eSingleChannel;
             break;             
            default:
        	psStreamInfo->uStreamInfo.sMpegInfo.eChmod  = BRAP_DSPCHN_MpegChannelMode_eInvalid;                
                BDBG_MSG(("Unknown eChmod for MPEG %d,",psStageStatus->sDecodeStage.sStreamInfo.sMpegStreamInfo.ui32Mode));
	    }


	switch(psStageStatus->sDecodeStage.sStreamInfo.sMpegStreamInfo.ui32MpegLayer)
        {
            case 1:
        	psStreamInfo->uStreamInfo.sMpegInfo.eLayer = BRAP_DSPCHN_MpegAudioLayer_eMpegLayer3;
             break;
            case 2:
        	psStreamInfo->uStreamInfo.sMpegInfo.eLayer = BRAP_DSPCHN_MpegAudioLayer_eMpegLayer2;
             break;            
            case 3:
        	psStreamInfo->uStreamInfo.sMpegInfo.eLayer = BRAP_DSPCHN_MpegAudioLayer_eMpegLayer1;
             break;            
            default:
        	psStreamInfo->uStreamInfo.sMpegInfo.eLayer = BRAP_DSPCHN_MpegAudioLayer_eMpegLayerInvalid;                
                BDBG_MSG(("Unknown e-Layer for MPEG %d,",psStageStatus->sDecodeStage.sStreamInfo.sMpegStreamInfo.ui32MpegLayer));
	    }

	psStreamInfo->uStreamInfo.sMpegInfo.ui32BitRate = 
        psStageStatus->sDecodeStage.sStreamInfo.sMpegStreamInfo.ui32BitRate;        

        if((psStageStatus->sDecodeStage.sStreamInfo.sMpegStreamInfo.ui32Emphasis <= 3))
        {
    	psStreamInfo->uStreamInfo.sMpegInfo.eEmphasisMode = 
            psStageStatus->sDecodeStage.sStreamInfo.sMpegStreamInfo.ui32Emphasis;        
        }
        else
        {
            psStreamInfo->uStreamInfo.sMpegInfo.eEmphasisMode =   BRAP_DSPCHN_MpegEmphasisMode_Invalid;
            BDBG_MSG(("Unknown emphasis for MPEG %d,",psStageStatus->sDecodeStage.sStreamInfo.sMpegStreamInfo.ui32Emphasis));
        }

	switch(psStageStatus->sDecodeStage.sStreamInfo.sMpegStreamInfo.ui32SamplingFreq)
    {
		case 0:		/* 44.1 KHz */
			psStreamInfo->uStreamInfo.sMpegInfo.eSamplingRate = BAVC_AudioSamplingRate_e44_1k;
			break;
		case 1:		/* 48 KHz */
			psStreamInfo->uStreamInfo.sMpegInfo.eSamplingRate = BAVC_AudioSamplingRate_e48k;
			break;
		case 2:		/* 32 KHz */
			psStreamInfo->uStreamInfo.sMpegInfo.eSamplingRate = BAVC_AudioSamplingRate_e32k;
			break;
		default:
                     psStreamInfo->uStreamInfo.sMpegInfo.eSamplingRate = BAVC_AudioSamplingRate_eUnknown;
			BDBG_MSG(("Error: Unknown MPEG sampling rate %#X",psStageStatus->sDecodeStage.sStreamInfo.sMpegStreamInfo.ui32SamplingFreq));
	}
        if(hDspCh->uHandle.hRapCh->eSamplingRate  != BAVC_AudioSamplingRate_eUnknown)
        {
            psStreamInfo->eSamplingRate = hDspCh->uHandle.hRapCh->eSamplingRate;
        }
        else
        {
            psStreamInfo->eSamplingRate = psStreamInfo->uStreamInfo.sMpegInfo.eSamplingRate;
        }
            

        psStreamInfo->sFrameDecAndErrInfo.uiTotalFramesDecoded = psStageStatus->sDecodeStage.sStreamInfo.sMpegStreamInfo.ui32TotalFramesDecoded;
        psStreamInfo->sFrameDecAndErrInfo.uiTotalFramesDummy = psStageStatus->sDecodeStage.sStreamInfo.sMpegStreamInfo.ui32TotalFramesDummy;
        psStreamInfo->sFrameDecAndErrInfo.uiTotalFramesInError = psStageStatus->sDecodeStage.sStreamInfo.sMpegStreamInfo.ui32TotalFramesInError;        

end:
    if(uiBufId != BRAP_INVALID_VALUE)
        BRAP_P_ReturnStageStatusBufToPool(hDspCh->uHandle.hRapCh->hRap,uiBufId);    

    return err;
}

#if 1
static BERR_Code
BRAP_DSPCHN_P_GetDdpMultiStreamInfo(
    BRAP_DSPCHN_Handle      hDspCh,
	unsigned int	        uiStatusBufAddr,
	BRAP_DSPCHN_StreamInfo *psStreamInfo
	)
{
    BRAP_FWIF_P_StageStatus *psStageStatus=NULL;
    uint32_t                i=0; 
    uint32_t           *bufPtr=NULL; 
    BERR_Code   err=BERR_SUCCESS;

    unsigned int uiBufId = BRAP_INVALID_VALUE;
     err = BRAP_P_AllocateStageStatusBufFromPool(hDspCh->uHandle.hRapCh->hRap,&uiBufId);
     if(err != BERR_SUCCESS)
    {
         BDBG_ERR(("Out of Buffer for StageStausBuffer. Function: %sLINE = %d",__FUNCTION__,__LINE__));
        goto end;
    }

     psStageStatus = hDspCh->uHandle.hRapCh->hRap->sOpenTimeMallocs.psStageStatus[uiBufId];
     BKNI_Memset( psStageStatus, 0, sizeof(BRAP_FWIF_P_StageStatus));    
        
    bufPtr =(uint32_t *) psStageStatus;
    for(i=0; i<sizeof(BRAP_FWIF_P_MultiStreamDDPStreamInfo)/4; i++)
    {
        *(bufPtr+i) = (uint32_t)BRAP_MemRead32(uiStatusBufAddr+i*4);
    }

    if(0 !=psStageStatus->sDecodeStage.sStreamInfo.sMSDDPStreamInfo.ui32StatusValid)
    {
        BDBG_MSG(("BRAP_DSPCHN_P_GetDdpMultiStreamInfo: Status buffer is not in valid status"));
        err = (BRAP_ERR_BAD_DEVICE_STATE);
        goto end;        
    }
    
	psStreamInfo->eType = BRAP_DSPCHN_AudioType_eAc3Plus;

	switch(psStageStatus->sDecodeStage.sStreamInfo.sMSDDPStreamInfo.ui32SamplingFrequency)
    {
		case 0:		/* 48 KHz */
			psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.eSamplingRate = BAVC_AudioSamplingRate_e48k;
			break;
		case 1:		/* 44.1 KHz */
			psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.eSamplingRate = BAVC_AudioSamplingRate_e44_1k;
			break;
		case 2:		/* 32 KHz */
			psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.eSamplingRate = BAVC_AudioSamplingRate_e32k;
			break;
		case 0xb:	/* 192 KHz */
			psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.eSamplingRate = BAVC_AudioSamplingRate_e192k;
			break;
		default:
			BDBG_ERR(("Error: Unknown DDP sampling rate"));
            psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.eSamplingRate = BAVC_AudioSamplingRate_eUnknown;
	}

    if(hDspCh->uHandle.hRapCh->eSamplingRate  != BAVC_AudioSamplingRate_eUnknown)
    {
        psStreamInfo->eSamplingRate = hDspCh->uHandle.hRapCh->eSamplingRate;
    }
    else
    {
        psStreamInfo->eSamplingRate = psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.eSamplingRate;
    }

	psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.ui8BitStreamId = 
        psStageStatus->sDecodeStage.sStreamInfo.sMSDDPStreamInfo.ui32BitStreamIdentification;

	psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.eBsmod = 
        psStageStatus->sDecodeStage.sStreamInfo.sMSDDPStreamInfo.ui32BsmodValue;

    if(hDspCh->uHandle.hRapCh->uiModeValue != BRAP_INVALID_VALUE)
    {
         psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.eAcmod = 
                         hDspCh->uHandle.hRapCh->uiModeValue;
    }
    else
    {
        psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.eAcmod = 
            psStageStatus->sDecodeStage.sStreamInfo.sMSDDPStreamInfo.ui32AcmodValue;
    }


        
	psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.eDSur = 
        psStageStatus->sDecodeStage.sStreamInfo.sMSDDPStreamInfo.ui32DsurmodValue;

	psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.bLfeOn = 
        psStageStatus->sDecodeStage.sStreamInfo.sMSDDPStreamInfo.ui32LfeOn;

	psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.bCh1LCodeExist = 
        psStageStatus->sDecodeStage.sStreamInfo.sMSDDPStreamInfo.ui32LangCodeExists;

	psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.ui8Ch1LCode = 
        psStageStatus->sDecodeStage.sStreamInfo.sMSDDPStreamInfo.ui32LangCodeValue;

	psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.eRoomType = 
        psStageStatus->sDecodeStage.sStreamInfo.sMSDDPStreamInfo.ui32RoomTypeValue;

    psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.eRoomType2 = 
        psStageStatus->sDecodeStage.sStreamInfo.sMSDDPStreamInfo.ui32RoomType2Value;

	psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.bCpyrhtPresent = 
        psStageStatus->sDecodeStage.sStreamInfo.sMSDDPStreamInfo.ui32CopyrightBit;
	
	psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.bOrigBitStream = 
        psStageStatus->sDecodeStage.sStreamInfo.sMSDDPStreamInfo.ui32OriginalBitStream;

    psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.eCentreMix=
        psStageStatus->sDecodeStage.sStreamInfo.sMSDDPStreamInfo.ui32CmixLevel;

    psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.eSurMix=    
        psStageStatus->sDecodeStage.sStreamInfo.sMSDDPStreamInfo.ui32SurmixLevel;
    
    psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.bCh2LCodeExist=    
        psStageStatus->sDecodeStage.sStreamInfo.sMSDDPStreamInfo.ui32Langcode2Exists;

    psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.ui8Ch2LCode=    
        psStageStatus->sDecodeStage.sStreamInfo.sMSDDPStreamInfo.ui32Langcode2Value;    

    psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.eTCode=    
        psStageStatus->sDecodeStage.sStreamInfo.sMSDDPStreamInfo.ui32TimeCodeExists;  
    
    psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.sFirstHalfTCode.ui32hours=    
        (psStageStatus->sDecodeStage.sStreamInfo.sMSDDPStreamInfo.ui32TimeCodeFirstHalf & 0x1F);    
    psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.sFirstHalfTCode.ui32minutes=    
        ((psStageStatus->sDecodeStage.sStreamInfo.sMSDDPStreamInfo.ui32TimeCodeFirstHalf >> 5) & 0x3F);    
    psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.sFirstHalfTCode.ui32seconds=        
        ((psStageStatus->sDecodeStage.sStreamInfo.sMSDDPStreamInfo.ui32TimeCodeFirstHalf >> 11) & 0x7);    

    psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.sSecondHalfTCode.ui32hours=    
        (psStageStatus->sDecodeStage.sStreamInfo.sMSDDPStreamInfo.ui32TimeCodeSecondHalf & 0x1F);    
    psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.sSecondHalfTCode.ui32minutes=    
        ((psStageStatus->sDecodeStage.sStreamInfo.sMSDDPStreamInfo.ui32TimeCodeSecondHalf >> 5) & 0x3F);    
    psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.sSecondHalfTCode.ui32seconds=        
        ((psStageStatus->sDecodeStage.sStreamInfo.sMSDDPStreamInfo.ui32TimeCodeSecondHalf >> 11) & 0x7);    

    psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.ui32FrameSizeCode=      
        psStageStatus->sDecodeStage.sStreamInfo.sMSDDPStreamInfo.ui32FrmSizeCod;

    psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.eDependentFrameChannelMapMode=            
        psStageStatus->sDecodeStage.sStreamInfo.sMSDDPStreamInfo.ui32DepFrameChanmapMode;

    psStreamInfo->sFrameDecAndErrInfo.uiTotalFramesDecoded = psStageStatus->sDecodeStage.sStreamInfo.sMSDDPStreamInfo.ui32TotalFramesDecoded;
    psStreamInfo->sFrameDecAndErrInfo.uiTotalFramesDummy = psStageStatus->sDecodeStage.sStreamInfo.sMSDDPStreamInfo.ui32TotalFramesDummy;
    psStreamInfo->sFrameDecAndErrInfo.uiTotalFramesInError = psStageStatus->sDecodeStage.sStreamInfo.sMSDDPStreamInfo.ui32TotalFramesInError;        

end:
    if(uiBufId != BRAP_INVALID_VALUE)
        BRAP_P_ReturnStageStatusBufToPool(hDspCh->uHandle.hRapCh->hRap,uiBufId);   


    return err;
}
#else
static BERR_Code
BRAP_DSPCHN_P_GetDdpInfo(
    BRAP_DSPCHN_Handle      hDspCh,
	unsigned int	        uiStatusBufAddr,
	BRAP_DSPCHN_StreamInfo *psStreamInfo
	)
{
    BRAP_FWIF_P_StageStatus *psStageStatus=NULL;
    uint32_t                i=0; 
    uint32_t           *bufPtr=NULL; 
    BERR_Code   err=BERR_SUCCESS;

    unsigned int uiBufId = BRAP_INVALID_VALUE;
     err = BRAP_P_AllocateStageStatusBufFromPool(hDspCh->uHandle.hRapCh->hRap,&uiBufId);
     if(err != BERR_SUCCESS)
    {
         BDBG_ERR(("Out of Buffer for StageStausBuffer. Function: %sLINE = %d",__FUNCTION__,__LINE__));
        goto end;
    }

     psStageStatus = hDspCh->uHandle.hRapCh->hRap->sOpenTimeMallocs.psStageStatus[uiBufId];
     BKNI_Memset( psStageStatus, 0, sizeof(BRAP_FWIF_P_StageStatus));    

    
    bufPtr =(uint32_t *) psStageStatus;
    for(i=0; i<sizeof(BRAP_FWIF_P_DdpStreamInfo)/4; i++)
    {
        *(bufPtr+i) = (uint32_t)BRAP_MemRead32(uiStatusBufAddr+i*4);
    }

    if(0 !=psStageStatus->sDecodeStage.sStreamInfo.sDdpStreamInfo.ui32StatusValid)
    {
        BDBG_MSG(("BRAP_DSPCHN_P_GetDdpInfo: Status buffer is not in valid status"));
        err = (BRAP_ERR_BAD_DEVICE_STATE);
        goto end;        
    }
    
	psStreamInfo->eType = BRAP_DSPCHN_AudioType_eAc3Plus;

	switch(psStageStatus->sDecodeStage.sStreamInfo.sDdpStreamInfo.ui32SamplingFrequency)
    {
		case 0:		/* 48 KHz */
			psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.eSamplingRate = BAVC_AudioSamplingRate_e48k;
			break;
		case 1:		/* 44.1 KHz */
			psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.eSamplingRate = BAVC_AudioSamplingRate_e44_1k;
			break;
		case 2:		/* 32 KHz */
			psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.eSamplingRate = BAVC_AudioSamplingRate_e32k;
			break;
		case 0xb:	/* 192 KHz */
			psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.eSamplingRate = BAVC_AudioSamplingRate_e192k;
			break;
		default:
			BDBG_ERR(("Error: Unknown DDP sampling rate"));
            psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.eSamplingRate = BAVC_AudioSamplingRate_eUnknown;
	}

    if(hDspCh->uHandle.hRapCh->eSamplingRate  != BAVC_AudioSamplingRate_eUnknown)
    {
        psStreamInfo->eSamplingRate = hDspCh->uHandle.hRapCh->eSamplingRate;
    }
    else
    {
	psStreamInfo->eSamplingRate = psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.eSamplingRate;
    }

	psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.ui8BitStreamId = 
        psStageStatus->sDecodeStage.sStreamInfo.sDdpStreamInfo.ui32BitStreamIdentification;

	psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.eBsmod = 
        psStageStatus->sDecodeStage.sStreamInfo.sDdpStreamInfo.ui32BsmodValue;


    if(hDspCh->uHandle.hRapCh->uiModeValue != BRAP_INVALID_VALUE)
    {
         psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.eAcmod = 
                         hDspCh->uHandle.hRapCh->uiModeValue;
    }
    else
    {
	psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.eAcmod = 
        psStageStatus->sDecodeStage.sStreamInfo.sDdpStreamInfo.ui32AcmodValue;;
    }


        
	psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.eDSur = 
        psStageStatus->sDecodeStage.sStreamInfo.sDdpStreamInfo.ui32DsurmodValue;

	psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.bLfeOn = 
        psStageStatus->sDecodeStage.sStreamInfo.sDdpStreamInfo.ui32LfeOn;

	psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.bCh1LCodeExist = 
        psStageStatus->sDecodeStage.sStreamInfo.sDdpStreamInfo.ui32LangCodeExists;

	psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.ui8Ch1LCode = 
        psStageStatus->sDecodeStage.sStreamInfo.sDdpStreamInfo.ui32LangCodeValue;

	psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.eRoomType = 
        psStageStatus->sDecodeStage.sStreamInfo.sDdpStreamInfo.ui32RoomTypeValue;

    psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.eRoomType2 = 
        psStageStatus->sDecodeStage.sStreamInfo.sDdpStreamInfo.ui32RoomType2Value;

	psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.bCpyrhtPresent = 
        psStageStatus->sDecodeStage.sStreamInfo.sDdpStreamInfo.ui32CopyrightBit;
	
	psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.bOrigBitStream = 
        psStageStatus->sDecodeStage.sStreamInfo.sDdpStreamInfo.ui32OriginalBitStream;

    psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.eCentreMix=
        psStageStatus->sDecodeStage.sStreamInfo.sDdpStreamInfo.ui32CmixLevel;

    psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.eSurMix=    
        psStageStatus->sDecodeStage.sStreamInfo.sDdpStreamInfo.ui32SurmixLevel;
    
    psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.bCh2LCodeExist=    
        psStageStatus->sDecodeStage.sStreamInfo.sDdpStreamInfo.ui32Langcode2Exists;

    psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.ui8Ch2LCode=    
        psStageStatus->sDecodeStage.sStreamInfo.sDdpStreamInfo.ui32Langcode2Value;    

    psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.eTCode=    
        psStageStatus->sDecodeStage.sStreamInfo.sDdpStreamInfo.ui32TimeCodeExists;  
    
    psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.sFirstHalfTCode.ui32hours=    
        (psStageStatus->sDecodeStage.sStreamInfo.sDdpStreamInfo.ui32TimeCodeFirstHalf & 0x1F);    
    psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.sFirstHalfTCode.ui32minutes=    
        ((psStageStatus->sDecodeStage.sStreamInfo.sDdpStreamInfo.ui32TimeCodeFirstHalf >> 5) & 0x3F);    
    psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.sFirstHalfTCode.ui32seconds=        
        ((psStageStatus->sDecodeStage.sStreamInfo.sDdpStreamInfo.ui32TimeCodeFirstHalf >> 11) & 0x7);    

    psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.sSecondHalfTCode.ui32hours=    
        (psStageStatus->sDecodeStage.sStreamInfo.sDdpStreamInfo.ui32TimeCodeSecondHalf & 0x1F);    
    psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.sSecondHalfTCode.ui32minutes=    
        ((psStageStatus->sDecodeStage.sStreamInfo.sDdpStreamInfo.ui32TimeCodeSecondHalf >> 5) & 0x3F);    
    psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.sSecondHalfTCode.ui32seconds=        
        ((psStageStatus->sDecodeStage.sStreamInfo.sDdpStreamInfo.ui32TimeCodeSecondHalf >> 11) & 0x7);    

    psStreamInfo->uStreamInfo.sAc3PlusInfo.sAc3Info.ui32FrameSizeCode=      
        psStageStatus->sDecodeStage.sStreamInfo.sDdpStreamInfo.ui32FrmSizeCod;

    psStreamInfo->sFrameDecAndErrInfo.uiTotalFramesDecoded = psStageStatus->sDecodeStage.sStreamInfo.sDdpStreamInfo.ui32TotalFramesDecoded;
    psStreamInfo->sFrameDecAndErrInfo.uiTotalFramesDummy = psStageStatus->sDecodeStage.sStreamInfo.sDdpStreamInfo.ui32TotalFramesDummy;
    psStreamInfo->sFrameDecAndErrInfo.uiTotalFramesInError = psStageStatus->sDecodeStage.sStreamInfo.sDdpStreamInfo.ui32TotalFramesInError;        

end:
    if(uiBufId != BRAP_INVALID_VALUE)
        BRAP_P_ReturnStageStatusBufToPool(hDspCh->uHandle.hRapCh->hRap,uiBufId);   

    return err;
}


static BERR_Code
BRAP_DSPCHN_P_GetAc3Info(
    BRAP_DSPCHN_Handle      hDspCh,
	unsigned int	        uiStatusBufAddr,
	BRAP_DSPCHN_StreamInfo *psStreamInfo
	)
{

    BRAP_FWIF_P_StageStatus *psStageStatus=NULL;
	uint32_t                i=0; 
	uint32_t           *bufPtr=NULL; 
    BERR_Code   err=BERR_SUCCESS;

    unsigned int uiBufId = BRAP_INVALID_VALUE;
     err = BRAP_P_AllocateStageStatusBufFromPool(hDspCh->uHandle.hRapCh->hRap,&uiBufId);
     if(err != BERR_SUCCESS)
    {
         BDBG_ERR(("Out of Buffer for StageStausBuffer. Function: %sLINE = %d",__FUNCTION__,__LINE__));
        goto end;
    }

     psStageStatus = hDspCh->uHandle.hRapCh->hRap->sOpenTimeMallocs.psStageStatus[uiBufId];
     BKNI_Memset( psStageStatus, 0, sizeof(BRAP_FWIF_P_StageStatus));    
    
    bufPtr =(uint32_t *) psStageStatus;
    for(i=0; i<sizeof(BRAP_FWIF_P_Ac3StreamInfo)/4; i++)
    {
        *(bufPtr+i) = (uint32_t)BRAP_MemRead32(uiStatusBufAddr+i*4);
    }
    
    if(0 != psStageStatus->sDecodeStage.sStreamInfo.sAc3StreamInfo.ui32StatusValid)
    {
        BDBG_MSG(("BRAP_DSPCHN_P_GetAc3Info: Status buffer is not in valid status"));
        err = (BRAP_ERR_BAD_DEVICE_STATE);
        goto end;  
    }
	psStreamInfo->eType = BRAP_DSPCHN_AudioType_eAc3;

	switch(psStageStatus->sDecodeStage.sStreamInfo.sAc3StreamInfo.ui32SamplingFrequency)
    {
		case 0:		/* 48 KHz */
			psStreamInfo->uStreamInfo.sAc3Info.eSamplingRate = BAVC_AudioSamplingRate_e48k;
			break;
		case 1:		/* 44.1 KHz */
			psStreamInfo->uStreamInfo.sAc3Info.eSamplingRate = BAVC_AudioSamplingRate_e44_1k;
			break;
		case 2:		/* 32 KHz */
			psStreamInfo->uStreamInfo.sAc3Info.eSamplingRate = BAVC_AudioSamplingRate_e32k;
			break;
		default:
			BDBG_ERR(("Error: Unknown AC3 sampling rate"));
            psStreamInfo->uStreamInfo.sAc3Info.eSamplingRate = BAVC_AudioSamplingRate_eUnknown;
	}

    if(hDspCh->uHandle.hRapCh->eSamplingRate  != BAVC_AudioSamplingRate_eUnknown)
    {
        psStreamInfo->eSamplingRate = hDspCh->uHandle.hRapCh->eSamplingRate;
    }
    else
    {
	psStreamInfo->eSamplingRate = psStreamInfo->uStreamInfo.sAc3Info.eSamplingRate;
    }

	psStreamInfo->uStreamInfo.sAc3Info.ui8BitStreamId = 
        psStageStatus->sDecodeStage.sStreamInfo.sAc3StreamInfo.ui32BitStreamIdentification;

	psStreamInfo->uStreamInfo.sAc3Info.eBsmod = 
        psStageStatus->sDecodeStage.sStreamInfo.sAc3StreamInfo.ui32BsmodValue;

        if(hDspCh->uHandle.hRapCh->uiModeValue != BRAP_INVALID_VALUE)
        {
            psStreamInfo->uStreamInfo.sAc3Info.eAcmod  = hDspCh->uHandle.hRapCh->uiModeValue;
        }
        else
        {
            psStreamInfo->uStreamInfo.sAc3Info.eAcmod  = psStageStatus->sDecodeStage.sStreamInfo.sAc3StreamInfo.ui32AcmodValue;
        }

	psStreamInfo->uStreamInfo.sAc3Info.eDSur = 
        psStageStatus->sDecodeStage.sStreamInfo.sAc3StreamInfo.ui32DsurmodValue;

	psStreamInfo->uStreamInfo.sAc3Info.bLfeOn = 
        psStageStatus->sDecodeStage.sStreamInfo.sAc3StreamInfo.ui32LfeOn;

	psStreamInfo->uStreamInfo.sAc3Info.bCh1LCodeExist = 
        psStageStatus->sDecodeStage.sStreamInfo.sAc3StreamInfo.ui32LangCodeExists;

	psStreamInfo->uStreamInfo.sAc3Info.ui8Ch1LCode = 
        psStageStatus->sDecodeStage.sStreamInfo.sAc3StreamInfo.ui32LangCodeValue;

	psStreamInfo->uStreamInfo.sAc3Info.eRoomType = 
        psStageStatus->sDecodeStage.sStreamInfo.sAc3StreamInfo.ui32RoomTypeValue;

    psStreamInfo->uStreamInfo.sAc3Info.eRoomType2 = 
        psStageStatus->sDecodeStage.sStreamInfo.sAc3StreamInfo.ui32RoomType2Value;

	psStreamInfo->uStreamInfo.sAc3Info.bCpyrhtPresent = 
        psStageStatus->sDecodeStage.sStreamInfo.sAc3StreamInfo.ui32CopyrightBit;
	
	psStreamInfo->uStreamInfo.sAc3Info.bOrigBitStream = 
        psStageStatus->sDecodeStage.sStreamInfo.sAc3StreamInfo.ui32OriginalBitStream;


    psStreamInfo->uStreamInfo.sAc3Info.eCentreMix=
        psStageStatus->sDecodeStage.sStreamInfo.sAc3StreamInfo.ui32CmixLevel;

    psStreamInfo->uStreamInfo.sAc3Info.eSurMix=    
        psStageStatus->sDecodeStage.sStreamInfo.sAc3StreamInfo.ui32SurmixLevel;
    
    psStreamInfo->uStreamInfo.sAc3Info.bCh2LCodeExist=    
        psStageStatus->sDecodeStage.sStreamInfo.sAc3StreamInfo.ui32Langcode2Exists;

    psStreamInfo->uStreamInfo.sAc3Info.ui8Ch2LCode=    
        psStageStatus->sDecodeStage.sStreamInfo.sAc3StreamInfo.ui32Langcode2Value;    

    psStreamInfo->uStreamInfo.sAc3Info.eTCode=    
        psStageStatus->sDecodeStage.sStreamInfo.sAc3StreamInfo.ui32TimeCodeExists;  
    
    psStreamInfo->uStreamInfo.sAc3Info.sFirstHalfTCode.ui32hours=    
        (psStageStatus->sDecodeStage.sStreamInfo.sAc3StreamInfo.ui32TimeCodeFirstHalf & 0x1F);    
    psStreamInfo->uStreamInfo.sAc3Info.sFirstHalfTCode.ui32minutes=    
        ((psStageStatus->sDecodeStage.sStreamInfo.sAc3StreamInfo.ui32TimeCodeFirstHalf >> 5) & 0x3F);    
    psStreamInfo->uStreamInfo.sAc3Info.sFirstHalfTCode.ui32seconds=        
        ((psStageStatus->sDecodeStage.sStreamInfo.sAc3StreamInfo.ui32TimeCodeFirstHalf >> 11) & 0x7);    

    psStreamInfo->uStreamInfo.sAc3Info.ui32FrameSizeCode=      
        psStageStatus->sDecodeStage.sStreamInfo.sAc3StreamInfo.ui32FrmSizeCod;

    psStreamInfo->sFrameDecAndErrInfo.uiTotalFramesDecoded = psStageStatus->sDecodeStage.sStreamInfo.sAc3StreamInfo.ui32TotalFramesDecoded;
    psStreamInfo->sFrameDecAndErrInfo.uiTotalFramesDummy = psStageStatus->sDecodeStage.sStreamInfo.sAc3StreamInfo.ui32TotalFramesDummy;
    psStreamInfo->sFrameDecAndErrInfo.uiTotalFramesInError = psStageStatus->sDecodeStage.sStreamInfo.sAc3StreamInfo.ui32TotalFramesInError;    

end:
    if(uiBufId != BRAP_INVALID_VALUE)
        BRAP_P_ReturnStageStatusBufToPool(hDspCh->uHandle.hRapCh->hRap,uiBufId);   


    return err;
}
#endif
static BERR_Code
BRAP_DSPCHN_P_GetLpcmDvdInfo(
    BRAP_DSPCHN_Handle      hDspCh,
	unsigned int	        uiStatusBufAddr,
	BRAP_DSPCHN_StreamInfo *psStreamInfo
	)
{

    BRAP_FWIF_P_StageStatus *psStageStatus=NULL;
    uint32_t                i=0; 
    uint32_t           *bufPtr=NULL; 
    BERR_Code   err=BERR_SUCCESS;

    unsigned int uiBufId = BRAP_INVALID_VALUE;
     err = BRAP_P_AllocateStageStatusBufFromPool(hDspCh->uHandle.hRapCh->hRap,&uiBufId);
     if(err != BERR_SUCCESS)
    {
         BDBG_ERR(("Out of Buffer for StageStausBuffer. Function: %sLINE = %d",__FUNCTION__,__LINE__));
        goto end;
    }

     psStageStatus = hDspCh->uHandle.hRapCh->hRap->sOpenTimeMallocs.psStageStatus[uiBufId];
     BKNI_Memset( psStageStatus, 0, sizeof(BRAP_FWIF_P_StageStatus));    

    bufPtr =(uint32_t *) psStageStatus;
    for(i=0; i<sizeof(BRAP_FWIF_P_LpcmStreamInfo)/4; i++)
    {
        *(bufPtr+i) = (uint32_t)BRAP_MemRead32(uiStatusBufAddr+i*4);
    }
    
    if(0 != psStageStatus->sDecodeStage.sStreamInfo.sLpcmStreamInfo.ui32StatusValid)
    {
        BDBG_MSG(("BRAP_DSPCHN_P_GetLpcmDvdInfo: Status buffer is not in valid status"));
        err = (BRAP_ERR_BAD_DEVICE_STATE);
        goto end;  
    }

	switch(psStageStatus->sDecodeStage.sStreamInfo.sLpcmStreamInfo.ui32SamplingFreq)
    {
		case 0:		/* 48 KHz */
			psStreamInfo->uStreamInfo.sLpcmDvdInfo.eSamplingRate = BAVC_AudioSamplingRate_e48k;
			break;
		case 1:		/* 44.1 KHz */
			psStreamInfo->uStreamInfo.sLpcmDvdInfo.eSamplingRate = BAVC_AudioSamplingRate_e44_1k;
			break;
		case 2:		/* 32 KHz */
			psStreamInfo->uStreamInfo.sLpcmDvdInfo.eSamplingRate = BAVC_AudioSamplingRate_e32k;
			break;
		default:
			BDBG_ERR(("Error: Unknown LPCM DVD sampling rate"));
            psStreamInfo->uStreamInfo.sLpcmDvdInfo.eSamplingRate = BAVC_AudioSamplingRate_eUnknown;
	}

    if(hDspCh->uHandle.hRapCh->eSamplingRate  != BAVC_AudioSamplingRate_eUnknown)
    {
        psStreamInfo->eSamplingRate = hDspCh->uHandle.hRapCh->eSamplingRate;
    }
    else
    {
	psStreamInfo->eSamplingRate = psStreamInfo->uStreamInfo.sLpcmDvdInfo.eSamplingRate;
    }

	psStreamInfo->uStreamInfo.sLpcmDvdInfo.ui32FrameSize = psStageStatus->sDecodeStage.sStreamInfo.sLpcmStreamInfo.ui32FrameSize;
	psStreamInfo->uStreamInfo.sLpcmDvdInfo.eChnAssignment = psStageStatus->sDecodeStage.sStreamInfo.sLpcmStreamInfo.ui32ChannelAssignment;
	psStreamInfo->uStreamInfo.sLpcmDvdInfo.eBitsPerSample = psStageStatus->sDecodeStage.sStreamInfo.sLpcmStreamInfo.ui32BitsPerSample;
	psStreamInfo->uStreamInfo.sLpcmDvdInfo.bStartFlag = psStageStatus->sDecodeStage.sStreamInfo.sLpcmStreamInfo.ui32StartFlag;    
	psStreamInfo->uStreamInfo.sLpcmDvdInfo.bOutputLfe = psStageStatus->sDecodeStage.sStreamInfo.sLpcmStreamInfo.ui32OutputLfeMode;   

    psStreamInfo->sFrameDecAndErrInfo.uiTotalFramesDecoded = psStageStatus->sDecodeStage.sStreamInfo.sLpcmStreamInfo.ui32TotalFramesDecoded;
    psStreamInfo->sFrameDecAndErrInfo.uiTotalFramesDummy = psStageStatus->sDecodeStage.sStreamInfo.sLpcmStreamInfo.ui32TotalFramesDummy;
    psStreamInfo->sFrameDecAndErrInfo.uiTotalFramesInError = psStageStatus->sDecodeStage.sStreamInfo.sLpcmStreamInfo.ui32TotalFramesInError;  

end:
    if(uiBufId != BRAP_INVALID_VALUE)
        BRAP_P_ReturnStageStatusBufToPool(hDspCh->uHandle.hRapCh->hRap,uiBufId);   

    
    return err;
}



#ifdef RAP_MULTISTREAM_DECODER_SUPPORT
static BERR_Code
BRAP_DSPCHN_P_GetDolbyPulseInfo(
    BRAP_DSPCHN_Handle      hDspCh,
	unsigned int	        uiStatusBufAddr,
	BRAP_DSPCHN_StreamInfo *psStreamInfo
	)
{

    BRAP_FWIF_P_StageStatus *psStageStatus=NULL;
    uint32_t                i=0; 
    uint32_t           *bufPtr=NULL; 
    BERR_Code   err=BERR_SUCCESS;

    unsigned int uiBufId = BRAP_INVALID_VALUE;
     err = BRAP_P_AllocateStageStatusBufFromPool(hDspCh->uHandle.hRapCh->hRap,&uiBufId);
     if(err != BERR_SUCCESS)
    {
         BDBG_ERR(("Out of Buffer for StageStausBuffer. Function: %sLINE = %d",__FUNCTION__,__LINE__));
        goto end;
    }

     psStageStatus = hDspCh->uHandle.hRapCh->hRap->sOpenTimeMallocs.psStageStatus[uiBufId];
     BKNI_Memset( psStageStatus, 0, sizeof(BRAP_FWIF_P_StageStatus));    
    
    bufPtr =(uint32_t *) psStageStatus;
    for(i=0; i<sizeof(BRAP_FWIF_P_DolbyPulseStreamInfo)/4; i++)
    {
        *(bufPtr+i) = (uint32_t)BRAP_MemRead32(uiStatusBufAddr+i*4);
    }

    if(0 != psStageStatus->sDecodeStage.sStreamInfo.sDolbyPulseStreamInfo.ui32StatusValid)
    {
        BDBG_MSG(("BRAP_DSPCHN_P_GetDolbyPulseInfo: Status buffer is not in valid status"));
        err = (BRAP_ERR_BAD_DEVICE_STATE);
        goto end;  
    }
	psStreamInfo->eType = BRAP_DSPCHN_AudioType_eAacSbr;

	switch(psStageStatus->sDecodeStage.sStreamInfo.sDolbyPulseStreamInfo.ui32SamplingFreq)
    {
		case 0:		/* 96 KHz */
			psStreamInfo->uStreamInfo.sDolbyPulseInfo.sAacInfo.eSamplingRate = BAVC_AudioSamplingRate_e96k;
			break;
		case 1:		/* 88.2 KHz */
			psStreamInfo->uStreamInfo.sDolbyPulseInfo.sAacInfo.eSamplingRate = BAVC_AudioSamplingRate_e88_2k;
			break;
		case 2:		/* 64 KHz */
			psStreamInfo->uStreamInfo.sDolbyPulseInfo.sAacInfo.eSamplingRate = BAVC_AudioSamplingRate_e64k;
			break;
		case 3:		/* 48 KHz */
			psStreamInfo->uStreamInfo.sDolbyPulseInfo.sAacInfo.eSamplingRate = BAVC_AudioSamplingRate_e48k;
			break;
		case 4:		/* 44.1 KHz */
			psStreamInfo->uStreamInfo.sDolbyPulseInfo.sAacInfo.eSamplingRate = BAVC_AudioSamplingRate_e44_1k;
			break;
		case 5:		/* 32 KHz */
			psStreamInfo->uStreamInfo.sDolbyPulseInfo.sAacInfo.eSamplingRate = BAVC_AudioSamplingRate_e32k;
			break;
		case 6:		/* 24 KHz */
			psStreamInfo->uStreamInfo.sDolbyPulseInfo.sAacInfo.eSamplingRate = BAVC_AudioSamplingRate_e24k;
			break;
		case 7:		/* 22.05 KHz */
			psStreamInfo->uStreamInfo.sDolbyPulseInfo.sAacInfo.eSamplingRate = BAVC_AudioSamplingRate_e22_05k;
			break;
		case 8:		/* 16 KHz */
			psStreamInfo->uStreamInfo.sDolbyPulseInfo.sAacInfo.eSamplingRate = BAVC_AudioSamplingRate_e16k;
			break;
		case 9:		/* 12 KHz */
			psStreamInfo->uStreamInfo.sDolbyPulseInfo.sAacInfo.eSamplingRate = BAVC_AudioSamplingRate_e12k;
			break;
		case 10:	/* 11.025 KHz */
			psStreamInfo->uStreamInfo.sDolbyPulseInfo.sAacInfo.eSamplingRate = BAVC_AudioSamplingRate_e11_025k;
			break;
		case 11:	/* 8 KHz */
			psStreamInfo->uStreamInfo.sDolbyPulseInfo.sAacInfo.eSamplingRate = BAVC_AudioSamplingRate_e8k;
			break;
		default:
			BDBG_MSG(("Error: Unknown AACHE sampling rate = %d",
                psStageStatus->sDecodeStage.sStreamInfo.sDolbyPulseStreamInfo.ui32SamplingFreq));
            psStreamInfo->uStreamInfo.sDolbyPulseInfo.sAacInfo.eSamplingRate = BAVC_AudioSamplingRate_eUnknown;
	}

    if(hDspCh->uHandle.hRapCh->eSamplingRate  != BAVC_AudioSamplingRate_eUnknown)
    {
        psStreamInfo->eSamplingRate = hDspCh->uHandle.hRapCh->eSamplingRate;
    }
    else
    {
	psStreamInfo->eSamplingRate = psStreamInfo->uStreamInfo.sDolbyPulseInfo.sAacInfo.eSamplingRate;
    }

    psStreamInfo->uStreamInfo.sDolbyPulseInfo.sAacInfo.ui32BitRate = 
        psStageStatus->sDecodeStage.sStreamInfo.sDolbyPulseStreamInfo.ui32BitRate;

    psStreamInfo->uStreamInfo.sDolbyPulseInfo.sAacInfo.eProfile = 
        psStageStatus->sDecodeStage.sStreamInfo.sDolbyPulseStreamInfo.ui32Profile;

    psStreamInfo->uStreamInfo.sDolbyPulseInfo.sAacInfo.ui32LfeChannels = 
        psStageStatus->sDecodeStage.sStreamInfo.sDolbyPulseStreamInfo.ui32NumLfeChannels;

    psStreamInfo->uStreamInfo.sDolbyPulseInfo.sAacInfo.ui32BackChannels = 
        psStageStatus->sDecodeStage.sStreamInfo.sDolbyPulseStreamInfo.ui32NumBackChannels;

    psStreamInfo->uStreamInfo.sDolbyPulseInfo.sAacInfo.ui32SideChannels = 
        psStageStatus->sDecodeStage.sStreamInfo.sDolbyPulseStreamInfo.ui32NumSideChannels;

    psStreamInfo->uStreamInfo.sDolbyPulseInfo.sAacInfo.ui32FrontChannels = 
        psStageStatus->sDecodeStage.sStreamInfo.sDolbyPulseStreamInfo.ui32NumFrontChannels;

    psStreamInfo->uStreamInfo.sDolbyPulseInfo.sAacInfo.bLfeOn = 
        psStageStatus->sDecodeStage.sStreamInfo.sDolbyPulseStreamInfo.ui32LfeOn;


    if(hDspCh->uHandle.hRapCh->uiModeValue != BRAP_INVALID_VALUE)
    {
         psStreamInfo->uStreamInfo.sDolbyPulseInfo.sAacInfo.eAcmod = 
                         hDspCh->uHandle.hRapCh->uiModeValue;
    }
    else
    {
        psStreamInfo->uStreamInfo.sDolbyPulseInfo.sAacInfo.eAcmod = 
            psStageStatus->sDecodeStage.sStreamInfo.sDolbyPulseStreamInfo.ui32AcmodValue;
    }

    psStreamInfo->uStreamInfo.sDolbyPulseInfo.sAacInfo.bPseudoSurroundEnable = 
        psStageStatus->sDecodeStage.sStreamInfo.sDolbyPulseStreamInfo.ui32PseudoSurroundEnable;

    psStreamInfo->uStreamInfo.sDolbyPulseInfo.sAacInfo.uiMatrixMixdownIdx = 
        psStageStatus->sDecodeStage.sStreamInfo.sDolbyPulseStreamInfo.ui32MatrixMixdownIndex;

    psStreamInfo->uStreamInfo.sDolbyPulseInfo.sAacInfo.bMatrixMixdownPresent = 
        psStageStatus->sDecodeStage.sStreamInfo.sDolbyPulseStreamInfo.ui32MatrixMixdownPresent;

    psStreamInfo->uStreamInfo.sDolbyPulseInfo.sAacInfo.bDRCPresent = 
        psStageStatus->sDecodeStage.sStreamInfo.sDolbyPulseStreamInfo.ui32DrcPresent;

    psStreamInfo->uStreamInfo.sDolbyPulseInfo.sAacInfo.uiNumPulseId = 
        psStageStatus->sDecodeStage.sStreamInfo.sDolbyPulseStreamInfo.ui32DolbyPulseIdCnt;
    

    psStreamInfo->sFrameDecAndErrInfo.uiTotalFramesDecoded = psStageStatus->sDecodeStage.sStreamInfo.sDolbyPulseStreamInfo.ui32TotalFramesDecoded;
    psStreamInfo->sFrameDecAndErrInfo.uiTotalFramesDummy = psStageStatus->sDecodeStage.sStreamInfo.sDolbyPulseStreamInfo.ui32TotalFramesDummy;
    psStreamInfo->sFrameDecAndErrInfo.uiTotalFramesInError = psStageStatus->sDecodeStage.sStreamInfo.sDolbyPulseStreamInfo.ui32TotalFramesInError;  

end:
    if(uiBufId != BRAP_INVALID_VALUE)
        BRAP_P_ReturnStageStatusBufToPool(hDspCh->uHandle.hRapCh->hRap,uiBufId);   

    return err;
}
#else
static BERR_Code
BRAP_DSPCHN_P_GetAacheInfo(
    BRAP_DSPCHN_Handle      hDspCh,
	unsigned int	        uiStatusBufAddr,
	BRAP_DSPCHN_StreamInfo *psStreamInfo
	)
{

    BRAP_FWIF_P_StageStatus *psStageStatus=NULL;
    uint32_t                i=0; 
    uint32_t           *bufPtr=NULL; 
    BERR_Code   err=BERR_SUCCESS;

    unsigned int uiBufId = BRAP_INVALID_VALUE;
     err = BRAP_P_AllocateStageStatusBufFromPool(hDspCh->uHandle.hRapCh->hRap,&uiBufId);
     if(err != BERR_SUCCESS)
    {
         BDBG_ERR(("Out of Buffer for StageStausBuffer. Function: %sLINE = %d",__FUNCTION__,__LINE__));
        goto end;
    }

     psStageStatus = hDspCh->uHandle.hRapCh->hRap->sOpenTimeMallocs.psStageStatus[uiBufId];
     BKNI_Memset( psStageStatus, 0, sizeof(BRAP_FWIF_P_StageStatus));    
    
    bufPtr =(uint32_t *) psStageStatus;
    for(i=0; i<sizeof(BRAP_FWIF_P_AacheStreamInfo)/4; i++)
    {
        *(bufPtr+i) = (uint32_t)BRAP_MemRead32(uiStatusBufAddr+i*4);
    }
    
    if(0 != psStageStatus->sDecodeStage.sStreamInfo.sAacheStreamInfo.ui32StatusValid)
    {
        BDBG_MSG(("BRAP_DSPCHN_P_GetAacheInfo: Status buffer is not in valid status"));
        err = (BRAP_ERR_BAD_DEVICE_STATE);
        goto end;  
    }
	psStreamInfo->eType = BRAP_DSPCHN_AudioType_eAacSbr;

	switch(psStageStatus->sDecodeStage.sStreamInfo.sAacheStreamInfo.ui32SamplingFreq)
    {
		case 0:		/* 96 KHz */
			psStreamInfo->uStreamInfo.sAacSbrInfo.sAacInfo.eSamplingRate = BAVC_AudioSamplingRate_e96k;
			break;
		case 1:		/* 88.2 KHz */
			psStreamInfo->uStreamInfo.sAacSbrInfo.sAacInfo.eSamplingRate = BAVC_AudioSamplingRate_e88_2k;
			break;
		case 2:		/* 64 KHz */
			psStreamInfo->uStreamInfo.sAacSbrInfo.sAacInfo.eSamplingRate = BAVC_AudioSamplingRate_e64k;
			break;
		case 3:		/* 48 KHz */
			psStreamInfo->uStreamInfo.sAacSbrInfo.sAacInfo.eSamplingRate = BAVC_AudioSamplingRate_e48k;
			break;
		case 4:		/* 44.1 KHz */
			psStreamInfo->uStreamInfo.sAacSbrInfo.sAacInfo.eSamplingRate = BAVC_AudioSamplingRate_e44_1k;
			break;
		case 5:		/* 32 KHz */
			psStreamInfo->uStreamInfo.sAacSbrInfo.sAacInfo.eSamplingRate = BAVC_AudioSamplingRate_e32k;
			break;
		case 6:		/* 24 KHz */
			psStreamInfo->uStreamInfo.sAacSbrInfo.sAacInfo.eSamplingRate = BAVC_AudioSamplingRate_e24k;
			break;
		case 7:		/* 22.05 KHz */
			psStreamInfo->uStreamInfo.sAacSbrInfo.sAacInfo.eSamplingRate = BAVC_AudioSamplingRate_e22_05k;
			break;
		case 8:		/* 16 KHz */
			psStreamInfo->uStreamInfo.sAacSbrInfo.sAacInfo.eSamplingRate = BAVC_AudioSamplingRate_e16k;
			break;
		case 9:		/* 12 KHz */
			psStreamInfo->uStreamInfo.sAacSbrInfo.sAacInfo.eSamplingRate = BAVC_AudioSamplingRate_e12k;
			break;
		case 10:	/* 11.025 KHz */
			psStreamInfo->uStreamInfo.sAacSbrInfo.sAacInfo.eSamplingRate = BAVC_AudioSamplingRate_e11_025k;
			break;
		case 11:	/* 8 KHz */
			psStreamInfo->uStreamInfo.sAacSbrInfo.sAacInfo.eSamplingRate = BAVC_AudioSamplingRate_e8k;
			break;
		default:
			BDBG_ERR(("Error: Unknown AACHE sampling rate"));
            psStreamInfo->uStreamInfo.sAacSbrInfo.sAacInfo.eSamplingRate = BAVC_AudioSamplingRate_eUnknown;
	}

    if(hDspCh->uHandle.hRapCh->eSamplingRate  != BAVC_AudioSamplingRate_eUnknown)
    {
        psStreamInfo->eSamplingRate = hDspCh->uHandle.hRapCh->eSamplingRate;
    }
    else
    {
	psStreamInfo->eSamplingRate = psStreamInfo->uStreamInfo.sAacSbrInfo.sAacInfo.eSamplingRate;
    }

    psStreamInfo->uStreamInfo.sAacSbrInfo.sAacInfo.ui32BitRate = 
        psStageStatus->sDecodeStage.sStreamInfo.sAacheStreamInfo.ui32BitRate;

    psStreamInfo->uStreamInfo.sAacSbrInfo.sAacInfo.eProfile = 
        psStageStatus->sDecodeStage.sStreamInfo.sAacheStreamInfo.ui32Profile;

    psStreamInfo->uStreamInfo.sAacSbrInfo.sAacInfo.ui32LfeChannels = 
        psStageStatus->sDecodeStage.sStreamInfo.sAacheStreamInfo.ui32NumLfeChannels;

    psStreamInfo->uStreamInfo.sAacSbrInfo.sAacInfo.ui32BackChannels = 
        psStageStatus->sDecodeStage.sStreamInfo.sAacheStreamInfo.ui32NumBackChannels;

    psStreamInfo->uStreamInfo.sAacSbrInfo.sAacInfo.ui32SideChannels = 
        psStageStatus->sDecodeStage.sStreamInfo.sAacheStreamInfo.ui32NumSideChannels;

    psStreamInfo->uStreamInfo.sAacSbrInfo.sAacInfo.ui32FrontChannels = 
        psStageStatus->sDecodeStage.sStreamInfo.sAacheStreamInfo.ui32NumFrontChannels;

    psStreamInfo->uStreamInfo.sAacSbrInfo.sAacInfo.bLfeOn = 
        psStageStatus->sDecodeStage.sStreamInfo.sAacheStreamInfo.ui32LfeOn;


    if(hDspCh->uHandle.hRapCh->uiModeValue != BRAP_INVALID_VALUE)
    {
         psStreamInfo->uStreamInfo.sAacSbrInfo.sAacInfo.eAcmod = 
                         hDspCh->uHandle.hRapCh->uiModeValue;
    }
    else
    {
        psStreamInfo->uStreamInfo.sAacSbrInfo.sAacInfo.eAcmod = 
            psStageStatus->sDecodeStage.sStreamInfo.sAacheStreamInfo.ui32AcmodValue;
    }



    psStreamInfo->uStreamInfo.sAacSbrInfo.sAacInfo.bPseudoSurroundEnable = 
        psStageStatus->sDecodeStage.sStreamInfo.sAacheStreamInfo.ui32PseudoSurroundEnable;

    psStreamInfo->uStreamInfo.sAacSbrInfo.sAacInfo.uiMatrixMixdownIdx = 
        psStageStatus->sDecodeStage.sStreamInfo.sAacheStreamInfo.ui32MatrixMixdownIndex;

    psStreamInfo->uStreamInfo.sAacSbrInfo.sAacInfo.bMatrixMixdownPresent = 
        psStageStatus->sDecodeStage.sStreamInfo.sAacheStreamInfo.ui32MatrixMixdownPresent;

    psStreamInfo->uStreamInfo.sAacSbrInfo.sAacInfo.bDRCPresent = 
        psStageStatus->sDecodeStage.sStreamInfo.sAacheStreamInfo.ui32DrcPresent;

    psStreamInfo->sFrameDecAndErrInfo.uiTotalFramesDecoded = psStageStatus->sDecodeStage.sStreamInfo.sAacheStreamInfo.ui32TotalFramesDecoded;
    psStreamInfo->sFrameDecAndErrInfo.uiTotalFramesDummy = psStageStatus->sDecodeStage.sStreamInfo.sAacheStreamInfo.ui32TotalFramesDummy;
    psStreamInfo->sFrameDecAndErrInfo.uiTotalFramesInError = psStageStatus->sDecodeStage.sStreamInfo.sAacheStreamInfo.ui32TotalFramesInError;  

end:
    if(uiBufId != BRAP_INVALID_VALUE)
        BRAP_P_ReturnStageStatusBufToPool(hDspCh->uHandle.hRapCh->hRap,uiBufId);   

    return err;
}
#endif

static BERR_Code
BRAP_DSPCHN_P_GetWmaStdInfo(
    BRAP_DSPCHN_Handle      hDspCh,
	unsigned int	        uiStatusBufAddr,
	BRAP_DSPCHN_StreamInfo *psStreamInfo
	)
{

    BRAP_FWIF_P_StageStatus *psStageStatus=NULL;
    uint32_t                i=0; 
    uint32_t           *bufPtr=NULL; 
    BERR_Code   err=BERR_SUCCESS;
 unsigned   ui32Acmod;

    unsigned int uiBufId = BRAP_INVALID_VALUE;
     err = BRAP_P_AllocateStageStatusBufFromPool(hDspCh->uHandle.hRapCh->hRap,&uiBufId);
     if(err != BERR_SUCCESS)
    {
         BDBG_ERR(("Out of Buffer for StageStausBuffer. Function: %sLINE = %d",__FUNCTION__,__LINE__));
        goto end;
    }

     psStageStatus = hDspCh->uHandle.hRapCh->hRap->sOpenTimeMallocs.psStageStatus[uiBufId];
     BKNI_Memset( psStageStatus, 0, sizeof(BRAP_FWIF_P_StageStatus));    

    bufPtr =(uint32_t *) psStageStatus;
    for(i=0; i<sizeof(BRAP_FWIF_P_WmaStreamInfo)/4; i++)
    {
        *(bufPtr+i) = (uint32_t)BRAP_MemRead32(uiStatusBufAddr+i*4);
    }
    
    if(0 != psStageStatus->sDecodeStage.sStreamInfo.sWmaStreamInfo.ui32StatusValid)
    {
        BDBG_MSG(("BRAP_DSPCHN_P_GetWmaStdInfo: Status buffer is not in valid status"));
        err = (BRAP_ERR_BAD_DEVICE_STATE);
        goto end;  
    }
	psStreamInfo->eType = BRAP_DSPCHN_AudioType_eWmaStd;

	switch(psStageStatus->sDecodeStage.sStreamInfo.sWmaStreamInfo.ui32SamplingFreq)
    {
    		case 0:		/* 44.1 KHz */
			psStreamInfo->eSamplingRate = BAVC_AudioSamplingRate_e44_1k;
			break;
		case 1:		/* 48 KHz */
			psStreamInfo->eSamplingRate = BAVC_AudioSamplingRate_e48k;
			break;
		case 2:		/* 32 KHz */
			psStreamInfo->eSamplingRate = BAVC_AudioSamplingRate_e32k;
			break;
		default:
                    BDBG_ERR(("Error: Unknown WmaStd sampling rate"));
                    psStreamInfo->eSamplingRate = BAVC_AudioSamplingRate_eUnknown;
                    break;
	}

    if(hDspCh->uHandle.hRapCh->eSamplingRate  != BAVC_AudioSamplingRate_eUnknown)
    {
        psStreamInfo->eSamplingRate = hDspCh->uHandle.hRapCh->eSamplingRate;
    }

	psStreamInfo->uStreamInfo.sWmaStdInfo.ui32OriginalCopy = 
        psStageStatus->sDecodeStage.sStreamInfo.sWmaStreamInfo.ui32OriginalCopy;

	psStreamInfo->uStreamInfo.sWmaStdInfo.ui32Copyright = 
        psStageStatus->sDecodeStage.sStreamInfo.sWmaStreamInfo.ui32Copyright;



        switch(psStageStatus->sDecodeStage.sStreamInfo.sWmaStreamInfo.ui32Mode)
        {
            case 0:
                psStreamInfo->uStreamInfo.sWmaStdInfo.eStereoMode = BRAP_DSPCHN_WmaStdStereoMode_eAuto;
                break;
            case 1:
                psStreamInfo->uStreamInfo.sWmaStdInfo.eStereoMode = BRAP_DSPCHN_WmaStdStereoMode_eLtRt;
                break;                
            case 2:
                psStreamInfo->uStreamInfo.sWmaStdInfo.eStereoMode = BRAP_DSPCHN_WmaStdStereoMode_eLoRo;
                break;
            default:              
          	BDBG_ERR(("Error: Unknown WmaStd Stereo Mode"));
                psStreamInfo->uStreamInfo.sWmaStdInfo.eStereoMode = BRAP_DSPCHN_WmaStdStereoMode_eInvalid;
                break;
        }

        if(hDspCh->uHandle.hRapCh->uiModeValue != BRAP_INVALID_VALUE)
        {
            ui32Acmod = hDspCh->uHandle.hRapCh->uiModeValue;
        }
        else
        {
            ui32Acmod = psStageStatus->sDecodeStage.sStreamInfo.sWmaStreamInfo.ui32Acmod;
        }        

        switch(ui32Acmod)
        {
            case 1:
                psStreamInfo->uStreamInfo.sWmaStdInfo.eAcmod = BRAP_DSPCHN_WmaStdAcmod_eOneCh;
                break;
            case 2:
                psStreamInfo->uStreamInfo.sWmaStdInfo.eAcmod = BRAP_DSPCHN_WmaStdAcmod_eTwoCh;
                break;
            default:
                BDBG_ERR(("Error!! Unknown Acmod for wma %d",psStageStatus->sDecodeStage.sStreamInfo.sWmaStreamInfo.ui32Acmod));
                psStreamInfo->uStreamInfo.sWmaStdInfo.eAcmod = BRAP_DSPCHN_WmaStdAcmod_NotDefined;                
        }

    	psStreamInfo->uStreamInfo.sWmaStdInfo.ui32BitRate = 
        psStageStatus->sDecodeStage.sStreamInfo.sWmaStreamInfo.ui32BitRate;

	psStreamInfo->uStreamInfo.sWmaStdInfo.bCrcPresent = 
        psStageStatus->sDecodeStage.sStreamInfo.sWmaStreamInfo.ui32CrcPresent;        

    	psStreamInfo->uStreamInfo.sWmaStdInfo.eVersion = 
        psStageStatus->sDecodeStage.sStreamInfo.sWmaStreamInfo.ui32Version;        

    	psStreamInfo->uStreamInfo.sWmaStdInfo.bLfeOn = 
        psStageStatus->sDecodeStage.sStreamInfo.sWmaStreamInfo.ui32LfeOn;             

    psStreamInfo->sFrameDecAndErrInfo.uiTotalFramesDecoded = psStageStatus->sDecodeStage.sStreamInfo.sWmaStreamInfo.ui32TotalFramesDecoded;
    psStreamInfo->sFrameDecAndErrInfo.uiTotalFramesDummy = psStageStatus->sDecodeStage.sStreamInfo.sWmaStreamInfo.ui32TotalFramesDummy;
    psStreamInfo->sFrameDecAndErrInfo.uiTotalFramesInError = psStageStatus->sDecodeStage.sStreamInfo.sWmaStreamInfo.ui32TotalFramesInError;          

    end:
        if(uiBufId != BRAP_INVALID_VALUE)
            BRAP_P_ReturnStageStatusBufToPool(hDspCh->uHandle.hRapCh->hRap,uiBufId);   

    return err;
}

static BERR_Code
BRAP_DSPCHN_P_GetWmaProInfo(
    BRAP_DSPCHN_Handle      hDspCh,
	unsigned int	        uiStatusBufAddr,
	BRAP_DSPCHN_StreamInfo *psStreamInfo
	)
{

    BRAP_FWIF_P_StageStatus *psStageStatus=NULL;
    uint32_t                i=0; 
    uint32_t           *bufPtr=NULL; 
    BERR_Code   err=BERR_SUCCESS;
    unsigned   ui32Acmod;

    unsigned int uiBufId = BRAP_INVALID_VALUE;
     err = BRAP_P_AllocateStageStatusBufFromPool(hDspCh->uHandle.hRapCh->hRap,&uiBufId);
     if(err != BERR_SUCCESS)
    {
         BDBG_ERR(("Out of Buffer for StageStausBuffer. Function: %sLINE = %d",__FUNCTION__,__LINE__));
        goto end;
    }

     psStageStatus = hDspCh->uHandle.hRapCh->hRap->sOpenTimeMallocs.psStageStatus[uiBufId];
     BKNI_Memset( psStageStatus, 0, sizeof(BRAP_FWIF_P_StageStatus));    
    
    bufPtr =(uint32_t *) psStageStatus;
    for(i=0; i<sizeof(BRAP_FWIF_P_WmaProStreamInfo)/4; i++)
    {
        *(bufPtr+i) = (uint32_t)BRAP_MemRead32(uiStatusBufAddr+i*4);
    }
    
    if(0 != psStageStatus->sDecodeStage.sStreamInfo.sWmaProStreamInfo.ui32StatusValid)
    {
        BDBG_MSG(("BRAP_DSPCHN_P_GetWmaProInfo: Status buffer is not in valid status"));
        err = (BRAP_ERR_BAD_DEVICE_STATE);
        goto end;  
    }
	psStreamInfo->eType = BRAP_DSPCHN_AudioType_eWmaPro;

	switch(psStageStatus->sDecodeStage.sStreamInfo.sWmaProStreamInfo.ui32SamplingFreq)
    {
    		case 0:		/* 44.1 KHz */
			psStreamInfo->eSamplingRate = BAVC_AudioSamplingRate_e44_1k;
			break;
		case 1:		/* 48 KHz */
			psStreamInfo->eSamplingRate = BAVC_AudioSamplingRate_e48k;
			break;
		case 2:		/* 32 KHz */
			psStreamInfo->eSamplingRate = BAVC_AudioSamplingRate_e32k;
			break;
		default:
                    BDBG_ERR(("Error: Unknown WmaPro sampling rate"));
                    psStreamInfo->eSamplingRate = BAVC_AudioSamplingRate_eUnknown;
                    break;
	}

    if(hDspCh->uHandle.hRapCh->eSamplingRate  != BAVC_AudioSamplingRate_eUnknown)
    {
        psStreamInfo->eSamplingRate = hDspCh->uHandle.hRapCh->eSamplingRate;
    }


	psStreamInfo->uStreamInfo.sWmaProInfo.ui32OriginalCopy = 
        psStageStatus->sDecodeStage.sStreamInfo.sWmaProStreamInfo.ui32OriginalCopy;

	psStreamInfo->uStreamInfo.sWmaProInfo.ui32Copyright = 
        psStageStatus->sDecodeStage.sStreamInfo.sWmaProStreamInfo.ui32Copyright;

        switch(psStageStatus->sDecodeStage.sStreamInfo.sWmaProStreamInfo.ui32Mode)
        {
            case 0:
                psStreamInfo->uStreamInfo.sWmaProInfo.eStereoMode = BRAP_DSPCHN_WmaProStereoMode_eAuto;
                break;
            case 1:
                psStreamInfo->uStreamInfo.sWmaProInfo.eStereoMode = BRAP_DSPCHN_WmaProStereoMode_eLtRt;
                break;                
            case 2:
                psStreamInfo->uStreamInfo.sWmaProInfo.eStereoMode = BRAP_DSPCHN_WmaProStereoMode_eLoRo;
                break;
            default:              
          	BDBG_ERR(("Error: Unknown WmaPro Stereo Mode"));
                psStreamInfo->uStreamInfo.sWmaProInfo.eStereoMode = BRAP_DSPCHN_WmaProStereoMode_eInvalid;
                break;
        }

        if(hDspCh->uHandle.hRapCh->uiModeValue != BRAP_INVALID_VALUE)
        {
            ui32Acmod = hDspCh->uHandle.hRapCh->uiModeValue;
        }
        else
        {
            ui32Acmod = psStageStatus->sDecodeStage.sStreamInfo.sWmaProStreamInfo.ui32Acmod;
        }        

        switch(psStageStatus->sDecodeStage.sStreamInfo.sWmaProStreamInfo.ui32Acmod)
        {
            case 1:
                psStreamInfo->uStreamInfo.sWmaProInfo.eAcmod = BRAP_DSPCHN_WmaProAcmod_eOneCentre_1_0_C;
                break;
            case 0: /* For Dual mono */                
            case 2: /* For Stereo */
                psStreamInfo->uStreamInfo.sWmaProInfo.eAcmod = BRAP_DSPCHN_WmaProAcmod_eTwoCh_2_0_L_R;
                break;
            case 3:
                psStreamInfo->uStreamInfo.sWmaProInfo.eAcmod = BRAP_DSPCHN_WmaProAcmod_eThreeCh_3_0_L_C_R;
                break;
            case 4:
                psStreamInfo->uStreamInfo.sWmaProInfo.eAcmod = BRAP_DSPCHN_WmaProAcmod_eThreeCh_2_1_L_R_S;
                break;
            case 5:
                psStreamInfo->uStreamInfo.sWmaProInfo.eAcmod = BRAP_DSPCHN_WmaProAcmod_eFourCh_3_1_L_C_R_S;
                break;                
            case 6:
                psStreamInfo->uStreamInfo.sWmaProInfo.eAcmod = BRAP_DSPCHN_WmaProAcmod_eFourCh_2_2_L_R_SL_SR;
                break;                
            case 7:
                psStreamInfo->uStreamInfo.sWmaProInfo.eAcmod = BRAP_DSPCHN_WmaProAcmod_eFiveCh_3_2_L_C_R_SL_SR;
                break;
            default:
                BDBG_ERR(("Error!! Unknown Acmod for wma pro %d",psStageStatus->sDecodeStage.sStreamInfo.sWmaProStreamInfo.ui32Acmod));
                psStreamInfo->uStreamInfo.sWmaProInfo.eAcmod = BRAP_DSPCHN_WmaProAcmod_NotDefined;                
        }

    	psStreamInfo->uStreamInfo.sWmaProInfo.ui32BitRate = 
        psStageStatus->sDecodeStage.sStreamInfo.sWmaProStreamInfo.ui32BitRate;

	psStreamInfo->uStreamInfo.sWmaProInfo.bCrcPresent = 
        psStageStatus->sDecodeStage.sStreamInfo.sWmaProStreamInfo.ui32CrcPresent;        

    	psStreamInfo->uStreamInfo.sWmaProInfo.eVersion = 
        psStageStatus->sDecodeStage.sStreamInfo.sWmaProStreamInfo.ui32Version;        

    	psStreamInfo->uStreamInfo.sWmaProInfo.bLfeOn = 
        psStageStatus->sDecodeStage.sStreamInfo.sWmaProStreamInfo.ui32LfeOn;          

    psStreamInfo->sFrameDecAndErrInfo.uiTotalFramesDecoded = psStageStatus->sDecodeStage.sStreamInfo.sWmaProStreamInfo.ui32TotalFramesDecoded;
    psStreamInfo->sFrameDecAndErrInfo.uiTotalFramesDummy = psStageStatus->sDecodeStage.sStreamInfo.sWmaProStreamInfo.ui32TotalFramesDummy;
    psStreamInfo->sFrameDecAndErrInfo.uiTotalFramesInError = psStageStatus->sDecodeStage.sStreamInfo.sWmaProStreamInfo.ui32TotalFramesInError;                 

end:
    if(uiBufId != BRAP_INVALID_VALUE)
        BRAP_P_ReturnStageStatusBufToPool(hDspCh->uHandle.hRapCh->hRap,uiBufId);   


    return err;
}


static BERR_Code
BRAP_DSPCHN_P_GetDtsBroadcastInfo(
    BRAP_DSPCHN_Handle      hDspCh,
	unsigned int	        uiStatusBufAddr,
	BRAP_DSPCHN_StreamInfo *psStreamInfo
	)
{

    BRAP_FWIF_P_StageStatus *psStageStatus=NULL;
	uint32_t                i=0; 
	uint32_t           *bufPtr=NULL; 
    BERR_Code   err=BERR_SUCCESS;

    unsigned int uiBufId = BRAP_INVALID_VALUE;
     err = BRAP_P_AllocateStageStatusBufFromPool(hDspCh->uHandle.hRapCh->hRap,&uiBufId);
     if(err != BERR_SUCCESS)
    {
         BDBG_ERR(("Out of Buffer for StageStausBuffer. Function: %sLINE = %d",__FUNCTION__,__LINE__));
        goto end;
    }

     psStageStatus = hDspCh->uHandle.hRapCh->hRap->sOpenTimeMallocs.psStageStatus[uiBufId];
     BKNI_Memset( psStageStatus, 0, sizeof(BRAP_FWIF_P_StageStatus));    

    bufPtr =(uint32_t *) psStageStatus;
    for(i=0; i<sizeof(BRAP_FWIF_P_DtsHdStreamInfo)/4; i++)
    {
        *(bufPtr+i) = (uint32_t)BRAP_MemRead32(uiStatusBufAddr+i*4);
    }
    
    if(0 != psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.ui32StatusValid)
    {
        BDBG_MSG(("BRAP_DSPCHN_P_GetDtsBroadcastInfo: Status buffer is not in valid status"));
        err = (BRAP_ERR_BAD_DEVICE_STATE);
        goto end;  
    }
    psStreamInfo->eType = BRAP_DSPCHN_AudioType_eDtsBroadcast;

    psStreamInfo->uStreamInfo.sDtsBroadcastInfo.bCrcFlag = psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.ui32CrcFlag;
    psStreamInfo->uStreamInfo.sDtsBroadcastInfo.ui32NoOfPcmBlocks = psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.ui32NBlocks;
    psStreamInfo->uStreamInfo.sDtsBroadcastInfo.ui32FrameSize = psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.ui32Fsize;

    if(hDspCh->uHandle.hRapCh->uiModeValue != BRAP_INVALID_VALUE)
    {
        psStreamInfo->uStreamInfo.sDtsBroadcastInfo.eAMode  = hDspCh->uHandle.hRapCh->uiModeValue;
    }
    else
    {
        psStreamInfo->uStreamInfo.sDtsBroadcastInfo.eAMode = psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.ui32Amode;
    }      

    psStreamInfo->uStreamInfo.sDtsBroadcastInfo.ui32TransRateIndex = psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.ui32TransRate;
    psStreamInfo->uStreamInfo.sDtsBroadcastInfo.bEmbeddedDownMixEn = psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.ui32DownMix;
    psStreamInfo->uStreamInfo.sDtsBroadcastInfo.bEmbeddedDrc = psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.ui32DynRangeCoeff;
    psStreamInfo->uStreamInfo.sDtsBroadcastInfo.bHdcdFormat = psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.ui32HdcdFormat;
    psStreamInfo->uStreamInfo.sDtsBroadcastInfo.eExtAudioId = psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.ui32ExtAudioId;
    psStreamInfo->uStreamInfo.sDtsBroadcastInfo.bExtAudio = psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.ui32ExtAudioFlag;
    psStreamInfo->uStreamInfo.sDtsBroadcastInfo.ui32VersionNumber = psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.ui32VerNum;
    psStreamInfo->uStreamInfo.sDtsBroadcastInfo.eCopyHistory = psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.ui32CopyHist;
    psStreamInfo->uStreamInfo.sDtsBroadcastInfo.eSourcePcmResolution = psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.ui32PcmResolution;
    psStreamInfo->uStreamInfo.sDtsBroadcastInfo.ui32NumChannels = psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.sDtsFrameInfo.ui32NumOfChannels;
  
    switch(psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.sDtsFrameInfo.ui32SampleRate)
    {
        case 3:		    /* 32 KHz */
            psStreamInfo->uStreamInfo.sDtsBroadcastInfo.eSamplingRate = BAVC_AudioSamplingRate_e32k;
            break;
        case 8:		    /* 44.1 KHz */
            psStreamInfo->uStreamInfo.sDtsBroadcastInfo.eSamplingRate = BAVC_AudioSamplingRate_e44_1k;
            break;
        case 13:		/* 48 KHz */
            psStreamInfo->uStreamInfo.sDtsBroadcastInfo.eSamplingRate = BAVC_AudioSamplingRate_e48k;
            break;
        case 12:        /* 24 KHz */
            psStreamInfo->uStreamInfo.sDtsBroadcastInfo.eSamplingRate = BAVC_AudioSamplingRate_e24k;
            break;
        case 11:        /* 12 KHz */
            psStreamInfo->uStreamInfo.sDtsBroadcastInfo.eSamplingRate = BAVC_AudioSamplingRate_e12k;
            break;            
        default:
            BDBG_MSG(("Error: Unknown DTS Broadcast sampling rate"));
            psStreamInfo->uStreamInfo.sDtsBroadcastInfo.eSamplingRate = BAVC_AudioSamplingRate_eUnknown;
            break;
	}
    
    psStreamInfo->uStreamInfo.sDtsBroadcastInfo.ui32ChannelAllocation = psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.sDtsFrameInfo.ui32ChannelAllocation;
    psStreamInfo->uStreamInfo.sDtsBroadcastInfo.bLfeFlag = psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.sDtsFrameInfo.ui32LFEPresent;
    psStreamInfo->uStreamInfo.sDtsBroadcastInfo.ui32BitRate = psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.sDtsFrameInfo.ui32BitsPerSample;
    psStreamInfo->uStreamInfo.sDtsBroadcastInfo.ui32ErrorStatus = psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.sDtsFrameInfo.ui32ErrorStatus;
    psStreamInfo->uStreamInfo.sDtsBroadcastInfo.bDTSNeoEnable = psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.sDtsFrameInfo.ui32DTSNeoEnable;
    psStreamInfo->uStreamInfo.sDtsBroadcastInfo.bEsFlag = psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.sDtsFrameInfo.ui32EsFlag;
    psStreamInfo->uStreamInfo.sDtsBroadcastInfo.bExtendedCodingFlag = psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.sDtsFrameInfo.ui32ExtendedCodingFlag;
    psStreamInfo->uStreamInfo.sDtsBroadcastInfo.ui32ExtensionAudioDescriptorFlag = psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.sDtsFrameInfo.ui32ExtensionAudioDescriptorFlag;
    psStreamInfo->uStreamInfo.sDtsBroadcastInfo.ui32PCMFrameSize = psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.sDtsFrameInfo.ui32PCMFrameSize;

    if(hDspCh->uHandle.hRapCh->eSamplingRate  != BAVC_AudioSamplingRate_eUnknown)
    {
        psStreamInfo->eSamplingRate = hDspCh->uHandle.hRapCh->eSamplingRate;
    }
    else
    {
        psStreamInfo->eSamplingRate = psStreamInfo->uStreamInfo.sDtsBroadcastInfo.eSamplingRate;
    }

    psStreamInfo->sFrameDecAndErrInfo.uiTotalFramesDecoded = psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.ui32TotalFramesDecoded;
    psStreamInfo->sFrameDecAndErrInfo.uiTotalFramesDummy = psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.ui32TotalFramesDummy;
    psStreamInfo->sFrameDecAndErrInfo.uiTotalFramesInError = psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.ui32TotalFramesInError;    

end:
    if(uiBufId != BRAP_INVALID_VALUE)
        BRAP_P_ReturnStageStatusBufToPool(hDspCh->uHandle.hRapCh->hRap,uiBufId);   


    return err;
}

static BERR_Code
BRAP_DSPCHN_P_GetDtsHdInfo(
    BRAP_DSPCHN_Handle      hDspCh,
	unsigned int	        uiStatusBufAddr,
	BRAP_DSPCHN_StreamInfo *psStreamInfo
	)
{

    BRAP_FWIF_P_StageStatus *psStageStatus=NULL;
	uint32_t                i=0; 
	uint32_t           *bufPtr=NULL; 
    BERR_Code   err=BERR_SUCCESS;

    unsigned int uiBufId = BRAP_INVALID_VALUE;
     err = BRAP_P_AllocateStageStatusBufFromPool(hDspCh->uHandle.hRapCh->hRap,&uiBufId);
     if(err != BERR_SUCCESS)
    {
         BDBG_ERR(("Out of Buffer for StageStausBuffer. Function: %sLINE = %d",__FUNCTION__,__LINE__));
        goto end;
    }

     psStageStatus = hDspCh->uHandle.hRapCh->hRap->sOpenTimeMallocs.psStageStatus[uiBufId];
     BKNI_Memset( psStageStatus, 0, sizeof(BRAP_FWIF_P_StageStatus));    

    bufPtr =(uint32_t *) psStageStatus;
    for(i=0; i<sizeof(BRAP_FWIF_P_DtsHdStreamInfo)/4; i++)
    {
        *(bufPtr+i) = (uint32_t)BRAP_MemRead32(uiStatusBufAddr+i*4);
    }
    
    if(0 != psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.ui32StatusValid)
    {
        BDBG_MSG(("BRAP_DSPCHN_P_GetDtsHdInfo: Status buffer is not in valid status"));
        err = (BRAP_ERR_BAD_DEVICE_STATE);
        goto end;  
    }
    psStreamInfo->eType = BRAP_DSPCHN_AudioType_eDtshd;

    psStreamInfo->uStreamInfo.sDtsHdInfo.bCrcFlag = psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.ui32CrcFlag;
    psStreamInfo->uStreamInfo.sDtsHdInfo.ui32NoOfPcmBlocks = psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.ui32NBlocks;
    psStreamInfo->uStreamInfo.sDtsHdInfo.ui32FrameSize = psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.ui32Fsize;

    if(hDspCh->uHandle.hRapCh->uiModeValue != BRAP_INVALID_VALUE)
    {
        psStreamInfo->uStreamInfo.sDtsHdInfo.eAMode  = hDspCh->uHandle.hRapCh->uiModeValue;
    }
    else
    {
        psStreamInfo->uStreamInfo.sDtsHdInfo.eAMode = psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.ui32Amode;
    }     

    psStreamInfo->uStreamInfo.sDtsHdInfo.ui32TransRateIndex = psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.ui32TransRate;
    psStreamInfo->uStreamInfo.sDtsHdInfo.bEmbeddedDownMixEn = psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.ui32DownMix;
    psStreamInfo->uStreamInfo.sDtsHdInfo.bEmbeddedDrc = psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.ui32DynRangeCoeff;
    psStreamInfo->uStreamInfo.sDtsHdInfo.bHdcdFormat = psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.ui32HdcdFormat;
    psStreamInfo->uStreamInfo.sDtsHdInfo.eExtAudioId = psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.ui32ExtAudioId;
    psStreamInfo->uStreamInfo.sDtsHdInfo.bExtAudio = psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.ui32ExtAudioFlag;
    psStreamInfo->uStreamInfo.sDtsHdInfo.ui32VersionNumber = psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.ui32VerNum;
    psStreamInfo->uStreamInfo.sDtsHdInfo.eCopyHistory = psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.ui32CopyHist;
    psStreamInfo->uStreamInfo.sDtsHdInfo.eSourcePcmResolution = psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.ui32PcmResolution;
    psStreamInfo->uStreamInfo.sDtsHdInfo.ui32NumChannels = psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.sDtsFrameInfo.ui32NumOfChannels;

    switch(psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.sDtsFrameInfo.ui32SampleRate)
    {
        case 3:		    /* 32 KHz */
            psStreamInfo->uStreamInfo.sDtsHdInfo.eSamplingRate = BAVC_AudioSamplingRate_e32k;
            break;
        case 8:		    /* 44.1 KHz */
            psStreamInfo->uStreamInfo.sDtsHdInfo.eSamplingRate = BAVC_AudioSamplingRate_e44_1k;
            break;
        case 13:		/* 48 KHz */
            psStreamInfo->uStreamInfo.sDtsHdInfo.eSamplingRate = BAVC_AudioSamplingRate_e48k;
            break;
        case 12:        /* 24 KHz */
            psStreamInfo->uStreamInfo.sDtsHdInfo.eSamplingRate = BAVC_AudioSamplingRate_e24k;
            break;
        case 11:        /* 12 KHz */
            psStreamInfo->uStreamInfo.sDtsHdInfo.eSamplingRate = BAVC_AudioSamplingRate_e12k;
            break;            
        default:
            BDBG_MSG(("Error: Unknown DTS HD sampling rate"));
            psStreamInfo->uStreamInfo.sDtsHdInfo.eSamplingRate = BAVC_AudioSamplingRate_eUnknown;
            break;
	}    
    
    psStreamInfo->uStreamInfo.sDtsHdInfo.ui32ChannelAllocation = psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.sDtsFrameInfo.ui32ChannelAllocation;
    psStreamInfo->uStreamInfo.sDtsHdInfo.bLfeFlag = psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.sDtsFrameInfo.ui32LFEPresent;
    psStreamInfo->uStreamInfo.sDtsHdInfo.ui32BitRate = psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.sDtsFrameInfo.ui32BitsPerSample;
    psStreamInfo->uStreamInfo.sDtsHdInfo.ui32ErrorStatus = psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.sDtsFrameInfo.ui32ErrorStatus;
    psStreamInfo->uStreamInfo.sDtsHdInfo.bDTSNeoEnable = psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.sDtsFrameInfo.ui32DTSNeoEnable;
    psStreamInfo->uStreamInfo.sDtsHdInfo.bEsFlag = psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.sDtsFrameInfo.ui32EsFlag;
    psStreamInfo->uStreamInfo.sDtsHdInfo.bExtendedCodingFlag = psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.sDtsFrameInfo.ui32ExtendedCodingFlag;
    psStreamInfo->uStreamInfo.sDtsHdInfo.ui32ExtensionAudioDescriptorFlag = psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.sDtsFrameInfo.ui32ExtensionAudioDescriptorFlag;
    psStreamInfo->uStreamInfo.sDtsHdInfo.ui32PCMFrameSize = psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.sDtsFrameInfo.ui32PCMFrameSize;

    if(hDspCh->uHandle.hRapCh->eSamplingRate  != BAVC_AudioSamplingRate_eUnknown)
    {
        psStreamInfo->eSamplingRate = hDspCh->uHandle.hRapCh->eSamplingRate;
    }
    else
    {
        psStreamInfo->eSamplingRate = psStreamInfo->uStreamInfo.sDtsHdInfo.eSamplingRate;
    }

    psStreamInfo->sFrameDecAndErrInfo.uiTotalFramesDecoded = psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.ui32TotalFramesDecoded;
    psStreamInfo->sFrameDecAndErrInfo.uiTotalFramesDummy = psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.ui32TotalFramesDummy;
    psStreamInfo->sFrameDecAndErrInfo.uiTotalFramesInError = psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.ui32TotalFramesInError;    
    psStreamInfo->sFrameDecAndErrInfo.eFrameErrorStatus = psStageStatus->sDecodeStage.sStreamInfo.sDtsHdStreamInfo.sDtsFrameInfo.ui32ErrorStatus;    
    
end:
    if(uiBufId != BRAP_INVALID_VALUE)
        BRAP_P_ReturnStageStatusBufToPool(hDspCh->uHandle.hRapCh->hRap,uiBufId);   


    return err;
}

static BERR_Code
BRAP_DSPCHN_P_GetPcmWavInfo(
    BRAP_DSPCHN_Handle      hDspCh,
	unsigned int	        uiStatusBufAddr,
	BRAP_DSPCHN_StreamInfo *psStreamInfo
	)
{
    BRAP_FWIF_P_StageStatus *psStageStatus=NULL;
	uint32_t                i=0; 
	uint32_t           *bufPtr=NULL; 
    BERR_Code   err=BERR_SUCCESS;

    unsigned int uiBufId = BRAP_INVALID_VALUE;
     err = BRAP_P_AllocateStageStatusBufFromPool(hDspCh->uHandle.hRapCh->hRap,&uiBufId);
     if(err != BERR_SUCCESS)
    {
        BDBG_ERR(("Out of Buffer for StageStausBuffer. Function: %sLINE = %d",__FUNCTION__,__LINE__));
        goto end;
    }

     psStageStatus = hDspCh->uHandle.hRapCh->hRap->sOpenTimeMallocs.psStageStatus[uiBufId];
     BKNI_Memset( psStageStatus, 0, sizeof(BRAP_FWIF_P_StageStatus));    
    
    bufPtr =(uint32_t *) psStageStatus;
    for(i=0; i<sizeof(BRAP_FWIF_P_PcmWavStreamInfo)/4; i++)
    {
        *(bufPtr+i) = (uint32_t)BRAP_MemRead32(uiStatusBufAddr+i*4);
    }
    if(0 != psStageStatus->sDecodeStage.sStreamInfo.sPcmWavStreamInfo.ui32StatusValid)
    {
        BDBG_MSG(("BRAP_DSPCHN_P_GetPcmWavInfo: Status buffer is not in valid status"));
        err = (BRAP_ERR_BAD_DEVICE_STATE);
        goto end;  
    }
    
    psStreamInfo->eType = BRAP_DSPCHN_AudioType_ePcmWav;

    BRAP_P_ConvertSrToEnum(psStageStatus->sDecodeStage.sStreamInfo.sPcmWavStreamInfo.ui32SamplingFreq,&(psStreamInfo->uStreamInfo.sPcmWavInfo.eSamplingRate));         

    if(hDspCh->uHandle.hRapCh->eSamplingRate  != BAVC_AudioSamplingRate_eUnknown)
    {
        psStreamInfo->eSamplingRate = hDspCh->uHandle.hRapCh->eSamplingRate;
    }
    else
    {
        psStreamInfo->eSamplingRate = psStreamInfo->uStreamInfo.sPcmWavInfo.eSamplingRate;
    }

    psStreamInfo->uStreamInfo.sPcmWavInfo.ui32NumChannels = psStageStatus->sDecodeStage.sStreamInfo.sPcmWavStreamInfo.ui32NumChannels;
    
    psStreamInfo->sFrameDecAndErrInfo.ui32NumSamplesDecoded = psStageStatus->sDecodeStage.sStreamInfo.sPcmWavStreamInfo.ui32NumSamplesDecoded;
    psStreamInfo->sFrameDecAndErrInfo.uiTotalFramesDecoded = psStageStatus->sDecodeStage.sStreamInfo.sPcmWavStreamInfo.ui32TotalFramesDecoded;
    psStreamInfo->sFrameDecAndErrInfo.uiTotalFramesDummy = psStageStatus->sDecodeStage.sStreamInfo.sPcmWavStreamInfo.ui32TotalFramesDummy;
    psStreamInfo->sFrameDecAndErrInfo.uiTotalFramesInError = psStageStatus->sDecodeStage.sStreamInfo.sPcmWavStreamInfo.ui32TotalFramesInError;        

end:
    if(uiBufId != BRAP_INVALID_VALUE)
        BRAP_P_ReturnStageStatusBufToPool(hDspCh->uHandle.hRapCh->hRap,uiBufId);   


    return err;
}

static BERR_Code
BRAP_DSPCHN_P_GetAmrInfo(
    BRAP_DSPCHN_Handle      hDspCh,
	unsigned int	        uiStatusBufAddr,
	BRAP_DSPCHN_StreamInfo *psStreamInfo
	)
{
    BRAP_FWIF_P_StageStatus *psStageStatus=NULL;
    uint32_t                i=0; 
    uint32_t           *bufPtr=NULL; 
    BERR_Code   err=BERR_SUCCESS;

    unsigned int uiBufId = BRAP_INVALID_VALUE;
     err = BRAP_P_AllocateStageStatusBufFromPool(hDspCh->uHandle.hRapCh->hRap,&uiBufId);
     if(err != BERR_SUCCESS)
    {
        BDBG_ERR(("Out of Buffer for StageStausBuffer. Function: %sLINE = %d",__FUNCTION__,__LINE__));
        goto end;
    }

     psStageStatus = hDspCh->uHandle.hRapCh->hRap->sOpenTimeMallocs.psStageStatus[uiBufId];
     BKNI_Memset( psStageStatus, 0, sizeof(BRAP_FWIF_P_StageStatus));    

    bufPtr =(uint32_t *) psStageStatus;
    for(i=0; i<sizeof(BRAP_FWIF_P_AmrStreamInfo)/4; i++)
    {
        *(bufPtr+i) = (uint32_t)BRAP_MemRead32(uiStatusBufAddr+i*4);
    }
    if(0 != psStageStatus->sDecodeStage.sStreamInfo.sAmrStreamInfo.ui32StatusValid)
    {
        BDBG_MSG(("BRAP_DSPCHN_P_GetAmrInfo: Status buffer is not in valid status"));
        err = (BRAP_ERR_BAD_DEVICE_STATE);
        goto end;  
    }
    
    psStreamInfo->eType = BRAP_DSPCHN_AudioType_eAmr;

    BRAP_P_ConvertSrToEnum(psStageStatus->sDecodeStage.sStreamInfo.sAmrStreamInfo.ui32SamplingFreq,&(psStreamInfo->uStreamInfo.sAmrInfo.eSamplingRate));

    if(hDspCh->uHandle.hRapCh->eSamplingRate  != BAVC_AudioSamplingRate_eUnknown)
    {
        psStreamInfo->eSamplingRate = hDspCh->uHandle.hRapCh->eSamplingRate;
    }
    else
    {
        psStreamInfo->eSamplingRate = psStreamInfo->uStreamInfo.sAmrInfo.eSamplingRate;
    }

    psStreamInfo->uStreamInfo.sAmrInfo.ui32BitRate = psStageStatus->sDecodeStage.sStreamInfo.sAmrStreamInfo.ui32BitRate;

    psStreamInfo->sFrameDecAndErrInfo.ui32NumSamplesDecoded = psStageStatus->sDecodeStage.sStreamInfo.sAmrStreamInfo.ui32NumSamplesDecoded;
    psStreamInfo->sFrameDecAndErrInfo.uiTotalFramesDecoded = psStageStatus->sDecodeStage.sStreamInfo.sAmrStreamInfo.ui32TotalFramesDecoded;
    psStreamInfo->sFrameDecAndErrInfo.uiTotalFramesDummy = psStageStatus->sDecodeStage.sStreamInfo.sAmrStreamInfo.ui32TotalFramesDummy;
    psStreamInfo->sFrameDecAndErrInfo.uiTotalFramesInError = psStageStatus->sDecodeStage.sStreamInfo.sAmrStreamInfo.ui32TotalFramesInError;        

end:
    if(uiBufId != BRAP_INVALID_VALUE)
        BRAP_P_ReturnStageStatusBufToPool(hDspCh->uHandle.hRapCh->hRap,uiBufId);   


    return err;
}


static BERR_Code
BRAP_DSPCHN_P_GetDraInfo(
    BRAP_DSPCHN_Handle      hDspCh,
	unsigned int	        uiStatusBufAddr,
	BRAP_DSPCHN_StreamInfo *psStreamInfo
	)
{
    BRAP_FWIF_P_StageStatus *psStageStatus=NULL;
	uint32_t                i=0; 
	uint32_t           *bufPtr=NULL; 
    BERR_Code   err=BERR_SUCCESS;

    unsigned int uiBufId = BRAP_INVALID_VALUE;
     err = BRAP_P_AllocateStageStatusBufFromPool(hDspCh->uHandle.hRapCh->hRap,&uiBufId);
     if(err != BERR_SUCCESS)
    {
        BDBG_ERR(("Out of Buffer for StageStausBuffer. Function: %sLINE = %d",__FUNCTION__,__LINE__));
        goto end;
    }

     psStageStatus = hDspCh->uHandle.hRapCh->hRap->sOpenTimeMallocs.psStageStatus[uiBufId];
     BKNI_Memset( psStageStatus, 0, sizeof(BRAP_FWIF_P_StageStatus));    
    
    bufPtr =(uint32_t *) psStageStatus;
    for(i=0; i<sizeof(BRAP_FWIF_P_DraStreamInfo)/4; i++)
    {
        *(bufPtr+i) = (uint32_t)BRAP_MemRead32(uiStatusBufAddr+i*4);
    }
    if(0 != psStageStatus->sDecodeStage.sStreamInfo.sDraStreamInfo.ui32StatusValid)
    {
        BDBG_MSG(("BRAP_DSPCHN_P_GetDraInfo: Status buffer is not in valid status"));
        err = (BRAP_ERR_BAD_DEVICE_STATE);
        goto end;  
    }
    
    psStreamInfo->eType = BRAP_DSPCHN_AudioType_eDra;


    switch(psStageStatus->sDecodeStage.sStreamInfo.sDraStreamInfo.ui32SamplingFrequency)
    {
        case    0:
            psStreamInfo->uStreamInfo.sDraInfo.eSamplingRate =   BAVC_AudioSamplingRate_e8k;      
            break;
        case    1:
            psStreamInfo->uStreamInfo.sDraInfo.eSamplingRate =   BAVC_AudioSamplingRate_e11_025k;      
            break;
        case    2:
            psStreamInfo->uStreamInfo.sDraInfo.eSamplingRate =   BAVC_AudioSamplingRate_e12k;      
            break;
        case    3:
            psStreamInfo->uStreamInfo.sDraInfo.eSamplingRate =   BAVC_AudioSamplingRate_e16k;      
            break;            
        case    4:
            psStreamInfo->uStreamInfo.sDraInfo.eSamplingRate =   BAVC_AudioSamplingRate_e22_05k;      
            break;
        case    5:
            psStreamInfo->uStreamInfo.sDraInfo.eSamplingRate =   BAVC_AudioSamplingRate_e24k;      
            break;
        case    6:
            psStreamInfo->uStreamInfo.sDraInfo.eSamplingRate =   BAVC_AudioSamplingRate_e32k;      
            break;
        case    7:
            psStreamInfo->uStreamInfo.sDraInfo.eSamplingRate =   BAVC_AudioSamplingRate_e44_1k;      
            break;
        case    8:
            psStreamInfo->uStreamInfo.sDraInfo.eSamplingRate =   BAVC_AudioSamplingRate_e48k;      
            break;
        case    9:
            psStreamInfo->uStreamInfo.sDraInfo.eSamplingRate =   BAVC_AudioSamplingRate_e88_2k;      
            break;
        case    10:
            psStreamInfo->uStreamInfo.sDraInfo.eSamplingRate =   BAVC_AudioSamplingRate_e96k;      
            break;
        case    11:
            psStreamInfo->uStreamInfo.sDraInfo.eSamplingRate =   BAVC_AudioSamplingRate_e176_4k;      
            break;            
        case    12:
            psStreamInfo->uStreamInfo.sDraInfo.eSamplingRate =   BAVC_AudioSamplingRate_e192k;      
            break;
        default:
            BDBG_MSG(("Invalid Sample rate in DRA Status Buf"));
            psStreamInfo->uStreamInfo.sDraInfo.eSamplingRate =   BAVC_AudioSamplingRate_eUnknown;                  
            break;
    }

    if(hDspCh->uHandle.hRapCh->eSamplingRate  != BAVC_AudioSamplingRate_eUnknown)
    {
        psStreamInfo->eSamplingRate = hDspCh->uHandle.hRapCh->eSamplingRate;
    }
    else
    {
        psStreamInfo->eSamplingRate = psStreamInfo->uStreamInfo.sDraInfo.eSamplingRate;
    }

    psStreamInfo->uStreamInfo.sDraInfo.uiFrameSize = psStageStatus->sDecodeStage.sStreamInfo.sDraStreamInfo.ui32FrameSize;
    psStreamInfo->uStreamInfo.sDraInfo.uiNumBlocks = psStageStatus->sDecodeStage.sStreamInfo.sDraStreamInfo.ui32NumBlocks;

    if(hDspCh->uHandle.hRapCh->uiModeValue != BRAP_INVALID_VALUE)
    {
        psStreamInfo->uStreamInfo.sDraInfo.eAcmod  = hDspCh->uHandle.hRapCh->uiModeValue;
    }
    else
    {
        psStreamInfo->uStreamInfo.sDraInfo.eAcmod = psStageStatus->sDecodeStage.sStreamInfo.sDraStreamInfo.ui32AcmodValue;    
    }    

    psStreamInfo->uStreamInfo.sDraInfo.bInputLfe = psStageStatus->sDecodeStage.sStreamInfo.sDraStreamInfo.ui32LFEOn;    
    psStreamInfo->uStreamInfo.sDraInfo.bOutputLfe = psStageStatus->sDecodeStage.sStreamInfo.sDraStreamInfo.ui32OutputLFEMode;    

    switch(psStageStatus->sDecodeStage.sStreamInfo.sDraStreamInfo.ui32OutputMode)
    {
        case 0:
            psStreamInfo->uStreamInfo.sDraInfo.eOutMode =BRAP_OutputMode_e1_0;
            psStreamInfo->uStreamInfo.sDraInfo.eStereoMode =BRAP_DSPCHN_DraStereoMode_eInvalid;            
            break;                        
        case 1:
            psStreamInfo->uStreamInfo.sDraInfo.eOutMode =BRAP_OutputMode_e2_0;
            psStreamInfo->uStreamInfo.sDraInfo.eStereoMode =BRAP_DSPCHN_DraStereoMode_eLoRo;
            break;                        
        case 2:
            psStreamInfo->uStreamInfo.sDraInfo.eOutMode =BRAP_OutputMode_e2_0;
            psStreamInfo->uStreamInfo.sDraInfo.eStereoMode =BRAP_DSPCHN_DraStereoMode_eLtRt;        
            break;                        
        case 3:
            psStreamInfo->uStreamInfo.sDraInfo.eOutMode =BRAP_OutputMode_e3_2;
            psStreamInfo->uStreamInfo.sDraInfo.eStereoMode =BRAP_DSPCHN_DraStereoMode_eInvalid;           
            break;                        
        default:
            BDBG_MSG(("Invalid Output/Stereo Mode in DRA Status Buf"));
            psStreamInfo->uStreamInfo.sDraInfo.eOutMode =BRAP_OutputMode_eLast;
            psStreamInfo->uStreamInfo.sDraInfo.eStereoMode =BRAP_DSPCHN_DraStereoMode_eInvalid;                  
            break;            
    }


    psStreamInfo->sFrameDecAndErrInfo.ui32NumSamplesDecoded = psStageStatus->sDecodeStage.sStreamInfo.sDraStreamInfo.ui32NumSamplesDecoded;
    psStreamInfo->sFrameDecAndErrInfo.uiTotalFramesDecoded = psStageStatus->sDecodeStage.sStreamInfo.sDraStreamInfo.ui32TotalFramesDecoded;
    psStreamInfo->sFrameDecAndErrInfo.uiTotalFramesDummy = psStageStatus->sDecodeStage.sStreamInfo.sDraStreamInfo.ui32TotalFramesDummy;
    psStreamInfo->sFrameDecAndErrInfo.uiTotalFramesInError = psStageStatus->sDecodeStage.sStreamInfo.sDraStreamInfo.ui32TotalFramesInError;        
    psStreamInfo->sFrameDecAndErrInfo.eFrameErrorStatus = psStageStatus->sDecodeStage.sStreamInfo.sDraStreamInfo.ui32ErrorStatus;    

end:
    if(uiBufId != BRAP_INVALID_VALUE)
        BRAP_P_ReturnStageStatusBufToPool(hDspCh->uHandle.hRapCh->hRap,uiBufId);   

    return err;
}




static BERR_Code
BRAP_DSPCHN_P_GetRealAudioLbrInfo(
    BRAP_DSPCHN_Handle      hDspCh,
	unsigned int	        uiStatusBufAddr,
	BRAP_DSPCHN_StreamInfo *psStreamInfo
	)
{
    BRAP_FWIF_P_StageStatus *psStageStatus=NULL;
	uint32_t                i=0; 
	uint32_t           *bufPtr=NULL; 
    BERR_Code       err=BERR_SUCCESS;

    unsigned int uiBufId = BRAP_INVALID_VALUE;
     err = BRAP_P_AllocateStageStatusBufFromPool(hDspCh->uHandle.hRapCh->hRap,&uiBufId);
     if(err != BERR_SUCCESS)
    {
        BDBG_ERR(("Out of Buffer for StageStausBuffer. Function: %sLINE = %d",__FUNCTION__,__LINE__));
        goto end;
    }

     psStageStatus = hDspCh->uHandle.hRapCh->hRap->sOpenTimeMallocs.psStageStatus[uiBufId];
     BKNI_Memset( psStageStatus, 0, sizeof(BRAP_FWIF_P_StageStatus));    
    
    bufPtr =(uint32_t *) psStageStatus;
    for(i=0; i<sizeof(BRAP_FWIF_P_RalbrStreamInfo)/4; i++)
    {
        *(bufPtr+i) = (uint32_t)BRAP_MemRead32(uiStatusBufAddr+i*4);
    }
    if(0 != psStageStatus->sDecodeStage.sStreamInfo.sRalbrStreamInfo.ui32StatusValid)
    {
        BDBG_MSG(("BRAP_DSPCHN_P_GetRealAudioLbrInfo: Status buffer is not in valid status"));
        err = (BRAP_ERR_BAD_DEVICE_STATE);
        goto end;  
    }
    
    psStreamInfo->eType = BRAP_DSPCHN_AudioType_eRealAudioLbr;

    BRAP_P_ConvertSrToEnum(psStageStatus->sDecodeStage.sStreamInfo.sRalbrStreamInfo.ui32SamplingFrequency,&(psStreamInfo->uStreamInfo.sRealAudioLbrInfo.eSamplingRate));
    
    if(hDspCh->uHandle.hRapCh->eSamplingRate  != BAVC_AudioSamplingRate_eUnknown)
    {
        psStreamInfo->eSamplingRate = hDspCh->uHandle.hRapCh->eSamplingRate;
    }
    else
    {
        psStreamInfo->eSamplingRate = psStreamInfo->uStreamInfo.sRealAudioLbrInfo.eSamplingRate;
    }

    psStreamInfo->uStreamInfo.sRealAudioLbrInfo.uiFrameSize = psStageStatus->sDecodeStage.sStreamInfo.sRalbrStreamInfo.ui32FrameSize;

    if(hDspCh->uHandle.hRapCh->uiModeValue != BRAP_INVALID_VALUE)
    {
        psStreamInfo->uStreamInfo.sRealAudioLbrInfo.eAcmod  = hDspCh->uHandle.hRapCh->uiModeValue;
    }
    else
    {
        psStreamInfo->uStreamInfo.sRealAudioLbrInfo.eAcmod = psStageStatus->sDecodeStage.sStreamInfo.sRalbrStreamInfo.ui32StreamAcmod;    
    }  





/*Need to update this when proper comments are recieved from FW team */
    switch(psStageStatus->sDecodeStage.sStreamInfo.sRalbrStreamInfo.ui32UserOutputMode)
    {
        case 0:
            psStreamInfo->uStreamInfo.sRealAudioLbrInfo.eOutMode =BRAP_OutputMode_e1_0;
            break;                        
        case 1:
            psStreamInfo->uStreamInfo.sRealAudioLbrInfo.eOutMode =BRAP_OutputMode_e2_0;
            break;                        
        case 2:
            psStreamInfo->uStreamInfo.sRealAudioLbrInfo.eOutMode =BRAP_OutputMode_e2_0;
            break;                        
        case 3:
            psStreamInfo->uStreamInfo.sRealAudioLbrInfo.eOutMode =BRAP_OutputMode_e3_2;
            break;                        
        default:
            BDBG_MSG(("Invalid Output/Stereo Mode in DRA Status Buf"));
            psStreamInfo->uStreamInfo.sRealAudioLbrInfo.eOutMode =BRAP_OutputMode_eLast;
            break;            
    }


    psStreamInfo->sFrameDecAndErrInfo.ui32NumSamplesDecoded = psStageStatus->sDecodeStage.sStreamInfo.sRalbrStreamInfo.ui32NumSamplesDecoded;
    psStreamInfo->sFrameDecAndErrInfo.uiTotalFramesDecoded = psStageStatus->sDecodeStage.sStreamInfo.sRalbrStreamInfo.ui32TotalFramesDecoded;
    psStreamInfo->sFrameDecAndErrInfo.uiTotalFramesDummy = psStageStatus->sDecodeStage.sStreamInfo.sRalbrStreamInfo.ui32TotalFramesDummy;
    psStreamInfo->sFrameDecAndErrInfo.uiTotalFramesInError = psStageStatus->sDecodeStage.sStreamInfo.sRalbrStreamInfo.ui32TotalFramesInError;        
    psStreamInfo->sFrameDecAndErrInfo.eFrameErrorStatus = psStageStatus->sDecodeStage.sStreamInfo.sRalbrStreamInfo.ui32ErrorStatus;    
    
end:
    if(uiBufId != BRAP_INVALID_VALUE)
        BRAP_P_ReturnStageStatusBufToPool(hDspCh->uHandle.hRapCh->hRap,uiBufId);   

    return err;
}


/***************************************************************************
Summary: Enable/Disable TSM logging 
 
Description:
	This function enables/disables TSM logging feature in firmware. If TSM
	logging is enabled, firmware logs TSM debug data into ping-pong buffers
	and interrupts RAP PI. RAP PI interrupt handler initializes structure
	BRAP_DSPCHN_TsmLogInfo and pass it on to application callback function.
	Application function evcuates data from the log buffer.
	Note: This API is valid only for a decode channel.

Returns:
	BERR_SUCCESS - If successful

See Also: 
**************************************************************************/
BERR_Code
BRAP_EnableTsmLog(
	BRAP_ChannelHandle		hRapCh,		    /* [in] Audio channel handle */
	bool					bEnable			/* [in] true = Enable TSM log
											   false = Disable TSM log */
	)
{
    BERR_Code err = BERR_SUCCESS;
    unsigned int uiConfigBufAddr;
	BRAP_DSPCHN_Handle hDspCh;
    BRAP_FWIF_P_TsmConfigParams      *psTsmConfigParam = NULL;
    BRAP_FWIF_P_FwTaskHandle        hFwTask=NULL;   
    BRAP_FWIF_P_ResponseType        eResponseType=BRAP_FWIF_P_ResponseType_eInvalid;

	BDBG_ENTER(BRAP_EnableTsmLog);
	BDBG_ASSERT(hRapCh);

    hDspCh = BRAP_DSPCHN_P_GetDspChHandle(&hRapCh, true);
    if(!(BRAP_P_IsPointerValid((void *)hDspCh)))
    {
        return BERR_TRACE(BERR_NOT_INITIALIZED);
    }
    

    if(hRapCh->eState == BRAP_P_State_eStarted)
    {
        err = BRAP_GetTsmNodeInfo(hRapCh,hDspCh,&uiConfigBufAddr,&hFwTask,&psTsmConfigParam);
        if(err != BERR_SUCCESS)
        {
    		BDBG_ERR(("BRAP_EnableTsmLog : TSM node info failed"));
    		BDBG_ASSERT(0);
        	return BERR_TRACE(err);
        }
        if(hFwTask->bStopped == true)
        {
                if(bEnable)
                {
                    hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.eTsmLogEnable = BRAP_FWIF_eTsmBool_True;
                    hRapCh->sDecSettings.sUserConfigStruct.sPassthruTsmConfigParam.eTsmLogEnable = BRAP_FWIF_eTsmBool_True;     
                    hRapCh->sDecSettings.sUserConfigStruct.sEncodeTsmConfigParam.eTsmLogEnable = BRAP_FWIF_eTsmBool_True;
                }
                else
                {
                    hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.eTsmLogEnable = BRAP_FWIF_eTsmBool_False;
                    hRapCh->sDecSettings.sUserConfigStruct.sPassthruTsmConfigParam.eTsmLogEnable = BRAP_FWIF_eTsmBool_False;  
                    hRapCh->sDecSettings.sUserConfigStruct.sEncodeTsmConfigParam.eTsmLogEnable = BRAP_FWIF_eTsmBool_False;                  
                }          
        }
        else
        {        
            if(bEnable)
            {
                psTsmConfigParam->eTsmLogEnable = BRAP_FWIF_eTsmBool_True;
            }
            else
            {
                psTsmConfigParam->eTsmLogEnable = BRAP_FWIF_eTsmBool_False;
            }
            eResponseType = BRAP_FWIF_P_ResponseType_eResponseRequired;
            err = BRAP_DSPCHN_P_SetTsmConfig(hDspCh,uiConfigBufAddr,hFwTask,psTsmConfigParam,&eResponseType);

            if(BERR_SUCCESS != err)
            {
                BDBG_ERR(("BRAP_EnableTsmLog: Set Tsm config failed!"));
                return BERR_TRACE(err);
            }
        }
    }       
    else if((hRapCh->eState == BRAP_P_State_eOpened)
        ||(hRapCh->bStopinvoked == true))
    {
/*Initializing all 3 type of TSM userconfig, because at Opentime DecodeMode is not known*/
            if(bEnable)
            {
                hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.eTsmLogEnable = BRAP_FWIF_eTsmBool_True;
                hRapCh->sDecSettings.sUserConfigStruct.sPassthruTsmConfigParam.eTsmLogEnable = BRAP_FWIF_eTsmBool_True;     
                hRapCh->sDecSettings.sUserConfigStruct.sEncodeTsmConfigParam.eTsmLogEnable = BRAP_FWIF_eTsmBool_True;
            }
            else
            {
                hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.eTsmLogEnable = BRAP_FWIF_eTsmBool_False;
                hRapCh->sDecSettings.sUserConfigStruct.sPassthruTsmConfigParam.eTsmLogEnable = BRAP_FWIF_eTsmBool_False;  
                hRapCh->sDecSettings.sUserConfigStruct.sEncodeTsmConfigParam.eTsmLogEnable = BRAP_FWIF_eTsmBool_False;                  
            }
    }

    BDBG_LEAVE(BRAP_EnableTsmLog);
	return err;
}

/***************************************************************************
Summary: Get TSM related information 

Description:
	This function returns "Time Stamp Management" related information. This
	function is introduced for debug purpose.
	Note: This API is valid only for a decode channel.

Returns:
	BERR_SUCCESS - If successful

See Also: 
**************************************************************************/
BERR_Code
BRAP_GetTsmDebugInfo(
	BRAP_ChannelHandle		    hRapCh,			/* [in] Audio channel handle */
	BRAP_DSPCHN_TsmDebugInfo	*psTsmInfo		/* [out] TSM related info */
	)
{
    BDBG_ASSERT(hRapCh);
    BDBG_ASSERT(psTsmInfo);

    psTsmInfo->bPlayback = hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.ePlayBackOn;
    psTsmInfo->i32PtsOffset = hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.ui32AVOffset;
    psTsmInfo->i32DiscardThreshold = hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.i32TSMDiscardThreshold;
    psTsmInfo->i32TsmGAThreshold = hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.i32TSMGrossThreshold;
    psTsmInfo->i32TsmSLThreshold = hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.i32TSMSyncLimitThreshold;
    psTsmInfo->bEnableASTMFlag = hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.eAstmEnable;
    psTsmInfo->i32TsmTransitionThreshold = hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.i32TsmTransitionThreshold;
    psTsmInfo->bStcValidFlag = hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.eSTCValid;
    psTsmInfo->bEnableTSMFlag = hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.eTsmEnable;    

    BRAP_GetCurrentPTS(hRapCh,&(psTsmInfo->sPtsInfo));

	return BERR_SUCCESS;
}

/***************************************************************************
Summary: Get decoder lock/unlock status 
 
Description:
	This function returns decoder locked/unlocked status. When decoder
	finds frame sync pattern in the stream, this function returns decoder
	lock status otherwise it returns decoder unlock status.
	Note: This API is valid only for a decode channel.	

Returns:
	BERR_SUCCESS - If successful

See Also: 
**************************************************************************/
BERR_Code
BRAP_GetDecoderLockStatus(
	BRAP_ChannelHandle		hRapCh,			/* [in] Audio channel handle */
	bool					*pbLock			/* [out] true = Decoder locked 
											   false = Decoder unlocked */
	)
{
	BDBG_ENTER(BRAP_GetDecoderLockStatus);
	/* Assert on the arguments */
	BDBG_ASSERT(hRapCh);
	BDBG_ASSERT(pbLock);

    if (BRAP_P_State_eStarted != hRapCh->eState)    
    {
        BDBG_ERR(("Can get lock status only after the channel has been started."
        "This channel [hRapCh=0x%8x] is currently stopped.", hRapCh));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    /* Since this is a single variable, we need not enter critical section */
	*pbLock = hRapCh->bDecLocked;
	BDBG_LEAVE(BRAP_GetDecoderLockStatus);
	return BERR_SUCCESS;
}

/***************************************************************************
Summary: ISR version of Get current PTS

Description:
	This function returns the value of current PTS in ISR context
	Note: This API is valid only for a decode channel.
	
Returns:
	BERR_SUCCESS - If successful

See Also:
**************************************************************************/
BERR_Code
BRAP_GetCurrentPTS_isr(
	BRAP_ChannelHandle	    hRapCh,			/* [in] Audio channel handle */
	BRAP_DSPCHN_PtsInfo		*psPtsInfo		/* [out] Current PTS information */ 
	)
{
    BERR_Code err = BERR_SUCCESS;
    unsigned int uiTaskIndex = 0, uiBranchId = 0, uiStageId = 0;
    unsigned int ui32TsmNodeIndex;
    uint32_t        uiStatusBufAddr=0;
    bool bDecStageFound = false;
    BRAP_CIT_P_OpStgInfo *psOpStgInfo;
    BRAP_FWIF_P_FwTaskHandle hFwTask;
    BRAP_DSPCHN_Handle hDspCh;
    BRAP_FWIF_P_TsmInfo psStageStatus;
    uint32_t                i=0; 
    uint32_t           *bufPtr=NULL; 

    BRAP_P_ChannelAudioProcessingStage  sTempAudProcStage;
    unsigned int uiTempBranchId = 0;
    bool bBranchCloned = false,bIsFwBranch =false;

    BDBG_ENTER(BRAP_GetCurrentPTS);
    BDBG_ASSERT(hRapCh);
    BDBG_ASSERT(psPtsInfo);

    if((hRapCh->eState == BRAP_P_State_eOpened)
    ||(hRapCh->bStopinvoked == true))
    {
        BDBG_MSG(("BRAP_GetCurrentPTS: Channel is in stopped condition"));
        err = (BRAP_ERR_BAD_DEVICE_STATE);
        goto end;                      
    }

    hDspCh = BRAP_DSPCHN_P_GetDspChHandle_isr(&hRapCh, true);
    if(!(BRAP_P_IsPointerValid((void *)hDspCh)))
    {
        err = BERR_TRACE(BERR_NOT_INITIALIZED);
        goto end;
    }
    
    /* Find out the task in which the decode stage is getting executed.
     Also find out the exact position of the decode stage within audio
     processing network. */

  for (uiTaskIndex = 0; uiTaskIndex < BRAP_FWIF_P_MAX_FW_TASK_PER_DSPCHN; uiTaskIndex++)
    {
        uiTempBranchId = 0;
        for (uiBranchId = 0; uiBranchId < BRAP_P_MAX_DST_PER_RAPCH; uiBranchId++)
        {
        bIsFwBranch = false;
            for (uiStageId = 0; uiStageId < BRAP_MAX_STAGE_PER_BRANCH_SUPPORTED; uiStageId++)
            {
                sTempAudProcStage = hDspCh->sFwTaskInfo[uiTaskIndex].sProcessingNw.sAudProcessingStage[uiBranchId][uiStageId];
                if (true== sTempAudProcStage.bDecoderStage)
                {
                    bDecStageFound = true;
                    break;
                }
                else if(((false == sTempAudProcStage.bCloneStage) && (NULL == sTempAudProcStage.hAudioProcessing)
                                && (true == hDspCh->sFwTaskInfo[uiTaskIndex].sProcessingNw.sAudProcessingStage[uiBranchId][uiStageId -1].bCloneStage))
                            ||((uiStageId == BRAP_MAX_STAGE_PER_BRANCH_SUPPORTED -1) && (true == sTempAudProcStage.bCloneStage)) /* Check if the last valid stage of the branch has bCloneStage == true. In that case output port cloning is happening.*/
                          ) 
                {
                    if(uiStageId == BRAP_MAX_STAGE_PER_BRANCH_SUPPORTED -1) /*If stageId is equal to MAX Stage ID*/
                    {
                        BRAP_FWIF_P_IsBranchCloned(hRapCh,&(hDspCh->sFwTaskInfo[uiTaskIndex].sProcessingNw),uiBranchId,uiStageId,&bBranchCloned,&bIsFwBranch);                
                    }
                    else
                    {
                        BRAP_FWIF_P_IsBranchCloned(hRapCh,&(hDspCh->sFwTaskInfo[uiTaskIndex].sProcessingNw),uiBranchId,uiStageId-1,&bBranchCloned,&bIsFwBranch);                
                    }                        
                    break;  /* Break from the FOR loop beacause last valid stage of the Branch */
                }
            }
            if (true==bDecStageFound) break;
            
            if(true == bIsFwBranch)
            {
                uiTempBranchId++;
            }
        }
        if (true==bDecStageFound) break;
    }
    if (false==bDecStageFound)
    {
        BDBG_ERR(("BRAP_GetCurrentPTS_isr: Could not find decoder stage in the network of audio processing stages."));
        err = (BRAP_ERR_BAD_DEVICE_STATE);
        goto end;        
    }

    if(uiTempBranchId >= BRAP_P_MAX_FW_BRANCH_PER_FW_TASK)
    {
        BDBG_ERR(("BRAP_GetCurrentPTS_isr: Branch id (%d) is exceeding the BRAP_P_MAX_FW_BRANCH_PER_FW_TASK =%d",
                    uiTempBranchId ,BRAP_P_MAX_FW_BRANCH_PER_FW_TASK));
        err = (BRAP_ERR_BAD_DEVICE_STATE);
        goto end;      
    }

    /* Get the Status buffer params */
    hFwTask = hDspCh->sFwTaskInfo[uiTaskIndex].hFwTask;
    psOpStgInfo = &(hFwTask->sCitOutput.sCitBranchInfo[uiTempBranchId].sCitStgInfo[uiStageId]);
    ui32TsmNodeIndex = psOpStgInfo->ui32TsmNodeIndex;
    uiStatusBufAddr = psOpStgInfo->sFwOpNodeInfo[ui32TsmNodeIndex].ui32StatusBufAdr;
    if(uiStatusBufAddr == BRAP_P_INVALID_DRAM_ADDRESS)
    {
        BDBG_ERR(("BRAP_GetCurrentPTS: Status buffer is invalid 0x%x",
                    uiStatusBufAddr));
        err = (BRAP_ERR_BUFFER_INVALID);
        goto end;              
    }
    if(psOpStgInfo->sFwOpNodeInfo[ui32TsmNodeIndex].ui32StatusBufSize <
            sizeof(BRAP_FWIF_P_TsmInfo))
    {
        BDBG_ERR(("BRAP_GetCurrentPTS: Status buffer size is invalid 0x%x",
                    sizeof(BRAP_FWIF_P_TsmInfo)));
        err = (BRAP_ERR_BUFFER_INVALID);
        goto end;              
    }

    bufPtr =(uint32_t *) &psStageStatus;
    for(i=0; i<sizeof(BRAP_FWIF_P_TsmInfo)/4; i++)
    {
        *(bufPtr+i) = (uint32_t)BRAP_MemRead32(uiStatusBufAddr+i*4);
    }
    
    if(0 != psStageStatus.ui32StatusValid)
    {
        BDBG_MSG(("BRAP_GetCurrentPTS: Status buffer is not in valid status"));
        err = (BRAP_ERR_BAD_DEVICE_STATE);
        goto end;                      
    }
    /*Write in the applications structure*/
    psPtsInfo->ePtsType = psStageStatus.ui32PtsType;
    psPtsInfo->i32Pts2StcPhase = psStageStatus.i32PtsToStcPhase;
    psPtsInfo->ui32RunningPts = psStageStatus.ui32RunningPts;
    psPtsInfo->i32TSMUpperThreshold = psStageStatus.i32TSMUpperThreshold;

	BDBG_LEAVE(BRAP_GetCurrentPTS_isr);

end:
	return err;
}


/***************************************************************************
Summary: Get current PTS

Description:
	This function returns the value of current PTS
	Note: This API is valid only for a decode channel.
	
Returns:
	BERR_SUCCESS - If successful

See Also:
**************************************************************************/
BERR_Code
BRAP_GetCurrentPTS(
	BRAP_ChannelHandle	    hRapCh,			/* [in] Audio channel handle */
	BRAP_DSPCHN_PtsInfo		*psPtsInfo		/* [out] Current PTS information */ 
	)
{
    BERR_Code err = BERR_SUCCESS;
    if (BRAP_P_State_eStarted != hRapCh->eState)    
    {
        BDBG_ERR(("Can get current PTS only after the channel has been started."
        "This channel [hRapCh=0x%8x] is currently stopped.", hRapCh));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    BKNI_EnterCriticalSection();
    err = BRAP_GetCurrentPTS_isr(hRapCh, psPtsInfo);
    BKNI_LeaveCriticalSection();
    
    return err;
}

/***************************************************************************
Summary: Sets start PTS and stop PTS values

Description:
	This function sets the start PTS and stop PTS values for a decode channel.
	Note: This API is valid only for a decode channel.
	
Returns:
	BERR_SUCCESS - If successful

See Also:
**************************************************************************/
BERR_Code
BRAP_SetStartStopPts (
	BRAP_ChannelHandle	hRapCh,			    /* [in] Audio channel handle */
	int					iStartPts,          /* [in] Start PTS value */
	int					iStopPts            /* [in] Stop PTS value */
	)
{
       BSTD_UNUSED(hRapCh);
       BSTD_UNUSED(iStartPts);
       BSTD_UNUSED(iStopPts);
	return BERR_SUCCESS;
}


/***************************************************************************
Summary: Set PTS offset

Description:
	This function sets the value of PTS offset. i32PtsOffset should be passed in 45Khz ticks
	i.e.  No of Milli second * 45

Returns:
	BERR_SUCCESS - If successful

See Also:
**************************************************************************/
BERR_Code
BRAP_SetPTSOffset(
	BRAP_ChannelHandle	hRapCh,			/* [in] Audio channel handle */
	int32_t				i32PtsOffset	/* [in] PTS Offset value */
	)
#if 1
{
    BERR_Code ret = BERR_SUCCESS;
	BDBG_ENTER(BRAP_SetPTSOffset);
	/* Assert the function arguments */
	BDBG_ASSERT(hRapCh);
	BKNI_EnterCriticalSection();
	ret = BRAP_SetPTSOffset_isr(hRapCh, i32PtsOffset);
	BKNI_LeaveCriticalSection();
	BDBG_LEAVE(BRAP_SetPTSOffset);
	return ret;
}
#else
{

    BERR_Code err = BERR_SUCCESS;
    unsigned int uiConfigBufAddr;
	BRAP_DSPCHN_Handle hDspCh;
    BRAP_FWIF_P_TsmConfigParams      *psTsmConfigParam = NULL;
    BRAP_FWIF_P_FwTaskHandle        hFwTask=NULL;   
    BRAP_FWIF_P_ResponseType        eResponseType=BRAP_FWIF_P_ResponseType_eInvalid;

	BDBG_ENTER(BRAP_SetPTSOffset);
	/* Assert on the arguments */
	BDBG_ASSERT(hRapCh);
 
    if(hRapCh->eState == BRAP_P_State_eOpened)
    {
/*Initializing all 3 type of TSM userconfig, because at Opentime DecodeMode is not known*/
        hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.ui32AVOffset = i32PtsOffset;
        hRapCh->sDecSettings.sUserConfigStruct.sPassthruTsmConfigParam.ui32AVOffset = i32PtsOffset; 
        hRapCh->sDecSettings.sUserConfigStruct.sEncodeTsmConfigParam.ui32AVOffset = i32PtsOffset;        
    }

    if(hRapCh->eState == BRAP_P_State_eStarted)
    {
        hDspCh = BRAP_DSPCHN_P_GetDspChHandle(hRapCh);
        if(NULL == hDspCh)
        {
            return BERR_TRACE(BERR_NOT_INITIALIZED);
        }
        err = BRAP_GetTsmNodeInfo(hRapCh,hDspCh,&uiConfigBufAddr,&hFwTask,&psTsmConfigParam);
        if(err != BERR_SUCCESS)
        {
    		BDBG_ERR(("BRAP_SetPTSOffset : TSM node info failed"));
    		BDBG_ASSERT(0);
        	return BERR_TRACE(err);
        }
        if(hFwTask->bStopped == true)
        {
            hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.ui32AVOffset = i32PtsOffset;
            hRapCh->sDecSettings.sUserConfigStruct.sPassthruTsmConfigParam.ui32AVOffset = i32PtsOffset; 
            hRapCh->sDecSettings.sUserConfigStruct.sEncodeTsmConfigParam.ui32AVOffset = i32PtsOffset;          
        }
        else
        {                
    /*        psTsmConfigParam->ui32PTSOffset = i32PtsOffset;*/
            psTsmConfigParam->ui32AVOffset = i32PtsOffset;
            eResponseType = BRAP_FWIF_P_ResponseType_eResponseRequired;
            err = BRAP_DSPCHN_P_SetTsmConfig(hDspCh,uiConfigBufAddr,hFwTask,psTsmConfigParam,&eResponseType);

            if(BERR_SUCCESS != err)
            {
                BDBG_ERR(("BRAP_SetPTSOffset: Set Tsm config failed!"));
                return BERR_TRACE(err);
            }
        }
    } 
    
	BDBG_LEAVE(BRAP_SetPTSOffset);
	return err;
}
#endif

/***************************************************************************
Summary: ISR Version of BRAP_SetPTSOffset

Description:
	This function sets the value of PTS offset. i32PtsOffset should be passed in 45Khz ticks
	i.e.  No of Milli second * 45

Returns:
	BERR_SUCCESS - If successful

See Also:
**************************************************************************/
BERR_Code
BRAP_SetPTSOffset_isr(
	BRAP_ChannelHandle	hRapCh,			/* [in] Audio channel handle */
	int32_t				i32PtsOffset	/* [in] PTS Offset value */
	)
{

    BERR_Code err = BERR_SUCCESS;
    unsigned int uiConfigBufAddr;
	BRAP_DSPCHN_Handle hDspCh;
    BRAP_FWIF_P_TsmConfigParams      *psTsmConfigParam = NULL;
    BRAP_FWIF_P_FwTaskHandle        hFwTask=NULL;   
    BRAP_FWIF_P_ResponseType        eResponseType=BRAP_FWIF_P_ResponseType_eInvalid;

	BDBG_ENTER(BRAP_SetPTSOffset_isr);
	/* Assert on the arguments */
	BDBG_ASSERT(hRapCh);
 
    if((hRapCh->eState == BRAP_P_State_eOpened)
        ||(hRapCh->bStopinvoked == true))
    {
/*Initializing all 3 type of TSM userconfig, because at Opentime DecodeMode is not known*/
        hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.ui32AVOffset = i32PtsOffset;
        hRapCh->sDecSettings.sUserConfigStruct.sPassthruTsmConfigParam.ui32AVOffset = i32PtsOffset; 
        hRapCh->sDecSettings.sUserConfigStruct.sEncodeTsmConfigParam.ui32AVOffset = i32PtsOffset;        
    }
    else if(hRapCh->eState == BRAP_P_State_eStarted)
    {
        hDspCh = BRAP_DSPCHN_P_GetDspChHandle_isr(&hRapCh, true);
        if(!(BRAP_P_IsPointerValid((void *)hDspCh)))
        {
    	     BDBG_WRN(("BRAP_SetPTSOffset_isr : Channel already stopped, saving it in Handle"));
            hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.ui32AVOffset = i32PtsOffset;
            hRapCh->sDecSettings.sUserConfigStruct.sPassthruTsmConfigParam.ui32AVOffset = i32PtsOffset; 
            hRapCh->sDecSettings.sUserConfigStruct.sEncodeTsmConfigParam.ui32AVOffset = i32PtsOffset;          
            return BERR_NOT_INITIALIZED;
        }
        err = BRAP_GetTsmNodeInfo_isr(hRapCh,hDspCh,&uiConfigBufAddr,&hFwTask,&psTsmConfigParam);
        if(err != BERR_SUCCESS)
        {
    	     BDBG_WRN(("BRAP_SetPTSOffset_isr : Channel already stopped, saving it in Handle"));
            hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.ui32AVOffset = i32PtsOffset;
            hRapCh->sDecSettings.sUserConfigStruct.sPassthruTsmConfigParam.ui32AVOffset = i32PtsOffset; 
            hRapCh->sDecSettings.sUserConfigStruct.sEncodeTsmConfigParam.ui32AVOffset = i32PtsOffset;  
        	return err;
        }
        if(hFwTask->bStopped == true)
        {
            hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.ui32AVOffset = i32PtsOffset;
            hRapCh->sDecSettings.sUserConfigStruct.sPassthruTsmConfigParam.ui32AVOffset = i32PtsOffset; 
            hRapCh->sDecSettings.sUserConfigStruct.sEncodeTsmConfigParam.ui32AVOffset = i32PtsOffset;          
        }
        else
        {                
    /*        psTsmConfigParam->ui32PTSOffset = i32PtsOffset;*/
            psTsmConfigParam->ui32AVOffset = i32PtsOffset;
            eResponseType = BRAP_FWIF_P_ResponseType_eNone;
            err = BRAP_DSPCHN_P_SetTsmConfig_isr(hDspCh,uiConfigBufAddr,hFwTask,psTsmConfigParam,&eResponseType);

            if(BERR_SUCCESS != err)
            {
                BDBG_ERR(("BRAP_SetPTSOffset_isr: Set Tsm config failed!"));
                return BERR_TRACE(err);
            }
        }
    } 
    
	BDBG_LEAVE(BRAP_SetPTSOffset_isr);
	return err;
}

/***************************************************************************
Summary: Set Playback Flag

Description:
	This function sets the Playback flag, i.e. it tells if its Live Decode or Playback.
	TRUE:  Playback
	FALSE: Live Decode

Returns:
	BERR_SUCCESS - If successful

See Also:
**************************************************************************/
BERR_Code
BRAP_SetPlaybackFlag(
	BRAP_ChannelHandle	hRapCh,			/* [in] Audio channel handle */
	bool				bPlayback	/* [in] PTS Offset value */
	)
{
    BERR_Code ret = BERR_SUCCESS;
	BDBG_ENTER(BRAP_SetPlaybackFlag);
	/* Assert the function arguments */
	BDBG_ASSERT(hRapCh);
	BKNI_EnterCriticalSection();
	ret = BRAP_SetPlaybackFlag_isr(hRapCh, bPlayback);
	BKNI_LeaveCriticalSection();
	BDBG_LEAVE(BRAP_SetPlaybackFlag);
	return ret;
}


/***************************************************************************
Summary: ISR Version of BRAP_SetPTSOffset
                Set Playback Flag

Description:
	This function sets the Playback flag, i.e. it tells if its Live Decode or Playback.
	TRUE:  Playback
	FALSE: Live Decode

Returns:
	BERR_SUCCESS - If successful

See Also:
**************************************************************************/
BERR_Code
BRAP_SetPlaybackFlag_isr(
	BRAP_ChannelHandle	hRapCh,			/* [in] Audio channel handle */
	bool				bPlayback	/* [in] PTS Offset value */
	)
{

    BERR_Code err = BERR_SUCCESS;
    unsigned int uiConfigBufAddr;
	BRAP_DSPCHN_Handle hDspCh;
    BRAP_FWIF_P_TsmConfigParams      *psTsmConfigParam = NULL;
    BRAP_FWIF_P_FwTaskHandle        hFwTask=NULL;   
    BRAP_FWIF_P_ResponseType        eResponseType=BRAP_FWIF_P_ResponseType_eInvalid;

	BDBG_ENTER(BRAP_SetPlaybackFlag_isr);
	/* Assert on the arguments */
	BDBG_ASSERT(hRapCh);
 
    if((hRapCh->eState == BRAP_P_State_eOpened)
        ||(hRapCh->bStopinvoked == true))
    {
/*Initializing all 3 type of TSM userconfig, because at Opentime DecodeMode is not known*/
        hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.ePlayBackOn = bPlayback;
        hRapCh->sDecSettings.sUserConfigStruct.sPassthruTsmConfigParam.ePlayBackOn = bPlayback; 
        hRapCh->sDecSettings.sUserConfigStruct.sEncodeTsmConfigParam.ePlayBackOn = bPlayback;        
    }
    else if(hRapCh->eState == BRAP_P_State_eStarted)
    {
        hDspCh = BRAP_DSPCHN_P_GetDspChHandle_isr(&hRapCh, true);
        if(!(BRAP_P_IsPointerValid((void *)hDspCh)))
        {
    	     BDBG_WRN(("BRAP_SetPlaybackFlag_isr : Channel already stopped, saving it in Handle"));
            hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.ePlayBackOn = bPlayback;
            hRapCh->sDecSettings.sUserConfigStruct.sPassthruTsmConfigParam.ePlayBackOn = bPlayback; 
            hRapCh->sDecSettings.sUserConfigStruct.sEncodeTsmConfigParam.ePlayBackOn = bPlayback;           
            return BERR_NOT_INITIALIZED;
        }
        err = BRAP_GetTsmNodeInfo_isr(hRapCh,hDspCh,&uiConfigBufAddr,&hFwTask,&psTsmConfigParam);
        if(err != BERR_SUCCESS)
        {
    	     BDBG_WRN(("BRAP_SetPlaybackFlag_isr : Channel already stopped, saving it in Handle"));
            hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.ePlayBackOn = bPlayback;
            hRapCh->sDecSettings.sUserConfigStruct.sPassthruTsmConfigParam.ePlayBackOn = bPlayback; 
            hRapCh->sDecSettings.sUserConfigStruct.sEncodeTsmConfigParam.ePlayBackOn = bPlayback;        
        	return err;
        }
        if(hFwTask->bStopped == true)
        {
            hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.ePlayBackOn = bPlayback;
            hRapCh->sDecSettings.sUserConfigStruct.sPassthruTsmConfigParam.ePlayBackOn = bPlayback; 
            hRapCh->sDecSettings.sUserConfigStruct.sEncodeTsmConfigParam.ePlayBackOn = bPlayback;          
        }
        else
        {                
    /*        psTsmConfigParam->ui32PTSOffset = i32PtsOffset;*/
            psTsmConfigParam->ePlayBackOn = bPlayback;
            eResponseType = BRAP_FWIF_P_ResponseType_eNone;
            err = BRAP_DSPCHN_P_SetTsmConfig_isr(hDspCh,uiConfigBufAddr,hFwTask,psTsmConfigParam,&eResponseType);

            if(BERR_SUCCESS != err)
            {
                BDBG_ERR(("BRAP_SetPlaybackFlag_isr: Set Tsm config failed!"));
                return BERR_TRACE(err);
            }
        }
    } 
    
	BDBG_LEAVE(BRAP_SetPlaybackFlag_isr);
	return err;
}

/***************************************************************************
Summary: Get audio decode delay

Description:
	This function gets the delay time for audio decode including post processing for current
	input type in milliseconds. This function is not supported for 7440.
	
Returns:
	BERR_SUCCESS - If successful

See Also:
**************************************************************************/
BERR_Code
BRAP_GetAudioDecodeDelay(
	BRAP_ChannelHandle	hRapCh,			/* [in] Audio channel handle */
	unsigned int			*uiAudDecDelay	/* [out] Audio Decode time */
	)
{
    BERR_Code err = BERR_SUCCESS;

    BDBG_ENTER(BRAP_GetAudioDecodeDelay);
    BDBG_ASSERT(hRapCh);
    BDBG_ASSERT(uiAudDecDelay);    
    
    *uiAudDecDelay = (unsigned int)((hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.ui32AudioOffset)/45); 
                                                            /*value in miili sec*/

    BDBG_LEAVE(BRAP_GetAudioDecodeDelay);
	return err;
}

/***************************************************************************
Summary: Set TSM transition threshold

Description:
	This function sets transition threshold for time stamp management. If the
	PTS-STC difference for an audio frame is more than this threshold,
	then audio decoder switches to non-TSM mode and also TSM error is
	raised.
	
Returns:
	BERR_SUCCESS - If successful

See Also:
	BRAP_EnableTSM
**************************************************************************/
BERR_Code
BRAP_SetTsmTransitionThreshold (	
	BRAP_ChannelHandle	hRapCh,				/* [in] Audio channel handle */
	int32_t				i32TsmTransitionThreshold
	)
{

    BERR_Code err = BERR_SUCCESS;
    unsigned int uiConfigBufAddr;
	BRAP_DSPCHN_Handle hDspCh;
    BRAP_FWIF_P_TsmConfigParams      *psTsmConfigParam = NULL;
    BRAP_FWIF_P_FwTaskHandle        hFwTask=NULL;   
    BRAP_FWIF_P_ResponseType        eResponseType=BRAP_FWIF_P_ResponseType_eInvalid;

	BDBG_ENTER(BRAP_SetTsmTransitionThreshold);
	/* Assert on the arguments */
	BDBG_ASSERT(hRapCh);

    if((hRapCh->eState == BRAP_P_State_eOpened)
        ||(hRapCh->bStopinvoked == true))
    {
/*Initializing all 3 type of TSM userconfig, because at Opentime DecodeMode is not known*/
/* Programming in 45KHZ tics format*/
        hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.i32TsmTransitionThreshold = i32TsmTransitionThreshold * 45;
        hRapCh->sDecSettings.sUserConfigStruct.sPassthruTsmConfigParam.i32TsmTransitionThreshold = i32TsmTransitionThreshold * 45; 
        hRapCh->sDecSettings.sUserConfigStruct.sEncodeTsmConfigParam.i32TsmTransitionThreshold = i32TsmTransitionThreshold * 45;
        
    }
    else if(hRapCh->eState == BRAP_P_State_eStarted)
    {
        hDspCh = BRAP_DSPCHN_P_GetDspChHandle(&hRapCh, true);
        if(!(BRAP_P_IsPointerValid((void *)hDspCh)))
        {
            return BERR_TRACE(BERR_NOT_INITIALIZED);
        }
        err = BRAP_GetTsmNodeInfo(hRapCh,hDspCh,&uiConfigBufAddr,&hFwTask,&psTsmConfigParam);
        if(err != BERR_SUCCESS)
        {
    	     BDBG_WRN(("BRAP_SetTsmTransitionThreshold : Channel already stopped, saving it in Handle"));
            hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.i32TsmTransitionThreshold = i32TsmTransitionThreshold * 45;
            hRapCh->sDecSettings.sUserConfigStruct.sPassthruTsmConfigParam.i32TsmTransitionThreshold = i32TsmTransitionThreshold * 45; 
            hRapCh->sDecSettings.sUserConfigStruct.sEncodeTsmConfigParam.i32TsmTransitionThreshold = i32TsmTransitionThreshold * 45;       
        	return err;
        }
        if(hFwTask->bStopped == true)
        {
            hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.i32TsmTransitionThreshold = i32TsmTransitionThreshold * 45;
            hRapCh->sDecSettings.sUserConfigStruct.sPassthruTsmConfigParam.i32TsmTransitionThreshold = i32TsmTransitionThreshold * 45; 
            hRapCh->sDecSettings.sUserConfigStruct.sEncodeTsmConfigParam.i32TsmTransitionThreshold = i32TsmTransitionThreshold * 45;      
        }
        else
        {              
    /* Programming in 45KHZ tics format*/
            psTsmConfigParam->i32TsmTransitionThreshold = i32TsmTransitionThreshold * 45;
            eResponseType = BRAP_FWIF_P_ResponseType_eResponseRequired;
            err = BRAP_DSPCHN_P_SetTsmConfig(hDspCh,uiConfigBufAddr,hFwTask,psTsmConfigParam,&eResponseType);
            if(BERR_SUCCESS != err)
            {
                BDBG_ERR(("BRAP_SetTsmTransitionThreshold: Set Tsm config failed!"));
                return BERR_TRACE(err);
            }
        }
    } 
    
    BDBG_LEAVE(BRAP_SetTsmTransitionThreshold);
    return BERR_SUCCESS;
}


/***************************************************************************
Summary: Set "STC valid" flag in firmware 

Description:
	This function sets "STC Valid" flag in firmware. In playback operation,
	application should call this function after programming STC. In playback
	firmware starts time stamp management only after this flag is set.
	
Returns:
	BERR_SUCCESS - If successful

See Also:
**************************************************************************/
BERR_Code
BRAP_SetStcValidFlag(
	BRAP_ChannelHandle	hRapCh			/* [in] Audio channel handle */
	)
{
    BERR_Code ret = BERR_SUCCESS;

	BDBG_ENTER(BRAP_SetStcValidFlag);
	/* Assert the function arguments */
	BDBG_ASSERT(hRapCh);
	BKNI_EnterCriticalSection();
	ret = BRAP_SetStcValidFlag_isr(hRapCh);
	BKNI_LeaveCriticalSection();
	BDBG_LEAVE(BRAP_SetStcValidFlag);
	return ret;
}

/***************************************************************************
Summary: Isr version of API  BRAP_SetStcValidFlag

Description:
	This function sets "STC Valid" flag in firmware. In playback operation,
	application should call this function after programming STC. In playback
	firmware starts time stamp management only after this flag is set. This
	function should be used from isr context.
	
Returns:
	BERR_SUCCESS - If successful

See Also:
**************************************************************************/
BERR_Code
BRAP_SetStcValidFlag_isr(
	BRAP_ChannelHandle	hRapCh			/* [in] Audio channel handle */
	)
{
    BERR_Code err = BERR_SUCCESS;
    unsigned int uiConfigBufAddr;
	BRAP_DSPCHN_Handle hDspCh;
    BRAP_FWIF_P_TsmConfigParams      *psTsmConfigParam = NULL;
    BRAP_FWIF_P_FwTaskHandle        hFwTask=NULL;   
    BRAP_FWIF_P_ResponseType        eResponseType=BRAP_FWIF_P_ResponseType_eInvalid;
    bool    bStopped =true;

	BDBG_ENTER(BRAP_SetStcValidFlag_isr);
	/* Assert on the arguments */
	BDBG_ASSERT(hRapCh);

    hDspCh = BRAP_DSPCHN_P_GetDspChHandle_isr(&hRapCh, true);

    if(hDspCh !=NULL)
    {
        err = BRAP_GetTsmNodeInfo_isr(hRapCh,hDspCh,&uiConfigBufAddr,&hFwTask,&psTsmConfigParam);
        if(err != BERR_SUCCESS)
        {
    		BDBG_WRN(("BRAP_SetStcValidFlag_isr : Channel already stopped, Ignoring SetStcValid Call"));
        	return err;
        }
        if(hFwTask->bStopped == true)
        {
            bStopped = true;
        }
        else
        {
            bStopped = false;
        }                    
    }

    if(bStopped == true)
    {
        BDBG_MSG(("BRAP_SetStcValidFlag_isr: Task is already stopped. Ignoring BRAP_SetStcValidFlag call"));    
        BDBG_LEAVE(BRAP_SetStcValidFlag_isr);
        return BERR_SUCCESS;     
    }
    else if(psTsmConfigParam->eSTCValid == BRAP_FWIF_eTsmBool_False)
    {
        psTsmConfigParam->eSTCValid = BRAP_FWIF_eTsmBool_True;
        eResponseType = BRAP_FWIF_P_ResponseType_eNone;
        err = BRAP_DSPCHN_P_SetTsmConfig_isr(hDspCh,uiConfigBufAddr,hFwTask,psTsmConfigParam,&eResponseType);
        if(BERR_SUCCESS != err)
        {
            BDBG_ERR(("BRAP_SetStcValidFlag_isr: Set Tsm config failed!"));
            return BERR_TRACE(err);
        }
        hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.eSTCValid = BRAP_FWIF_eTsmBool_True;
        hRapCh->sDecSettings.sUserConfigStruct.sPassthruTsmConfigParam.eSTCValid = BRAP_FWIF_eTsmBool_True;     
        hRapCh->sDecSettings.sUserConfigStruct.sEncodeTsmConfigParam.eSTCValid = BRAP_FWIF_eTsmBool_True;           
    }
    else
    {
        BDBG_MSG(("BRAP_SetStcValidFlag_isr: STC Valid is already set to true. Ignoring BRAP_SetStcValidFlag call"));
    } 
    
    BDBG_LEAVE(BRAP_SetStcValidFlag_isr);
    return BERR_SUCCESS;
}


/***************************************************************************
Summary: Invalidate  "STC " flag in firmware 

Description:
	This function Invalidate "STC " flag in firmware. 
	
Returns:
	BERR_SUCCESS - If successful

See Also:
**************************************************************************/
BERR_Code
BRAP_InvalidateStc(
	BRAP_ChannelHandle	hRapCh			/* [in] Audio channel handle */
	)
{
    BERR_Code ret = BERR_SUCCESS;

	BDBG_ENTER(BRAP_InvalidateStc);
	/* Assert the function arguments */
	BDBG_ASSERT(hRapCh);
	BKNI_EnterCriticalSection();
	ret = BRAP_InvalidateStc_isr(hRapCh);
	BKNI_LeaveCriticalSection();
	BDBG_LEAVE(BRAP_InvalidateStc);
	return ret;
}

/***************************************************************************
Summary: Isr version of API  BRAP_InvalidateStc

Description:
	This function Invalidate "STC " flag in firmware. 
	
Returns:
	BERR_SUCCESS - If successful

See Also:

**************************************************************************/
BERR_Code
BRAP_InvalidateStc_isr(
	BRAP_ChannelHandle	hRapCh			/* [in] Audio channel handle */
	)
{
    BERR_Code err = BERR_SUCCESS;
    unsigned int uiConfigBufAddr;
	BRAP_DSPCHN_Handle hDspCh;
    BRAP_FWIF_P_TsmConfigParams      *psTsmConfigParam = NULL;
    BRAP_FWIF_P_FwTaskHandle        hFwTask=NULL;   
    BRAP_FWIF_P_ResponseType        eResponseType=BRAP_FWIF_P_ResponseType_eInvalid;
    bool    bStopped =true;

	BDBG_ENTER(BRAP_InvalidateStc_isr);
	/* Assert on the arguments */
	BDBG_ASSERT(hRapCh);

    hDspCh = BRAP_DSPCHN_P_GetDspChHandle_isr(&hRapCh, true);

    if(hDspCh !=NULL)
    {
        err = BRAP_GetTsmNodeInfo_isr(hRapCh,hDspCh,&uiConfigBufAddr,&hFwTask,&psTsmConfigParam);
        if(err != BERR_SUCCESS)
        {
    		BDBG_WRN(("BRAP_InvalidateStc_isr : Channel already stopped, Ignoring InvalidateStc Call"));
        	return err;
        }
        if(hFwTask->bStopped == true)
        {
            bStopped = true;
        }
        else
        {
            bStopped = false;
        }                    
    }

    if(bStopped == true)
    {
        BDBG_MSG(("BRAP_SetStcValidFlag_isr: Task is already stopped. Ignoring BRAP_SetStcValidFlag call"));        
        BDBG_LEAVE(BRAP_InvalidateStc_isr);
        return BERR_SUCCESS;     
    }
    else if(psTsmConfigParam->eSTCValid == BRAP_FWIF_eTsmBool_True)
    {
        psTsmConfigParam->eSTCValid = BRAP_FWIF_eTsmBool_False;
        eResponseType = BRAP_FWIF_P_ResponseType_eNone;
        err = BRAP_DSPCHN_P_SetTsmConfig_isr(hDspCh,uiConfigBufAddr,hFwTask,psTsmConfigParam,&eResponseType);
        if(BERR_SUCCESS != err)
        {
            BDBG_ERR(("BRAP_InvalidateStc_isr: Set Tsm config failed!"));
            return BERR_TRACE(err);
        }
/*Initializing all 3 type of TSM userconfig, because at Opentime DecodeMode is not known*/
        hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.eSTCValid = BRAP_FWIF_eTsmBool_False;
        hRapCh->sDecSettings.sUserConfigStruct.sPassthruTsmConfigParam.eSTCValid = BRAP_FWIF_eTsmBool_False;     
        hRapCh->sDecSettings.sUserConfigStruct.sEncodeTsmConfigParam.eSTCValid = BRAP_FWIF_eTsmBool_False;            
    } 
    else
    {
        BDBG_MSG(("BRAP_InvalidateStc_isr: STC Valid is already set to true. Ignoring BRAP_InvalidateStc call"));
    } 
    
    BDBG_LEAVE(BRAP_InvalidateStc_isr);
    return BERR_SUCCESS;
}


/***************************************************************************
Summary: Gets the number of decoded samples

Description:
	This function gets the number of samples decoded of the input compressed stream.
	This function is supported only for MPEG, AAC and AAC-HE audio types. It is
	supported only for 3563.
	
Returns:
	BERR_SUCCESS - If successful
	BERR_NOT_SUPPORTED - If audio type is other than MPEG, AAC, AAC-HE. OR if
						    chip is 7440.

See Also:
**************************************************************************/
BERR_Code
BRAP_GetNumDecodedSamples(
	BRAP_ChannelHandle	hRapCh,			/* [in] Audio channel handle */
	unsigned int			*uiNumSamples	/* [out] Number of samples
										     decoded */
	)
{
       BSTD_UNUSED(hRapCh);
       BSTD_UNUSED(uiNumSamples);
	return BERR_SUCCESS;
}									     

/***************************************************************************
Summary:
    Creates audio processing stage.

Description:
    This function creates an audio processing stage.

Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    BRAP_DestroyProcessingStage
    
**************************************************************************/
BERR_Code BRAP_CreateProcessingStage(
    BRAP_Handle                 hRap,   /* [in] The Raptor Audio 
                                                           device handle*/                                                                    
    const BRAP_ProcessingStageSettings    *psProcessingStageSettings,   
                                                                        /* [in] Audio processing to be executed
                                                                             in this stage */
    BRAP_ProcessingStageHandle   *hAudioProcessingStageHandle  
                                                                        /* [out] Handle for the audio processing
                                                                             stage */
)
{
    BERR_Code rc=BERR_SUCCESS;
    BRAP_ProcessingStageHandle handle;
    unsigned int uiPpStg = 0,i=0;

    BDBG_ENTER(BRAP_CreateProcessingStage); 
    BDBG_ASSERT(psProcessingStageSettings);
    BDBG_ASSERT(hRap);         

    /* Allocate ProcessStage handle */
    handle = (BRAP_ProcessingStageHandle)BKNI_Malloc(sizeof(BRAP_P_AudioProcessingStage));
    if(handle == NULL)
    {
        rc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        return rc;
    }
    
    *hAudioProcessingStageHandle = handle;

    /* Reset the entire structure */
    BKNI_Memset( handle, 0, sizeof(BRAP_P_AudioProcessingStage));

    /* Init the data structure */
    handle->sProcessingStageSettings = *psProcessingStageSettings;
    handle->uiStagePosition = BRAP_INVALID_VALUE;
    handle->uHandle.hRapCh=NULL; 
    handle->uHandle.hAssociation=NULL;
    handle->hRap = hRap;
    handle->bChSpecificStage = false;

    /* Initialize  elements of hDestHandle array to NULL*/
    for( uiPpStg = 0; uiPpStg < BRAP_P_MAX_DEST_PER_PROCESSING_STAGE; uiPpStg++ )
    {
        handle->hDestHandle[uiPpStg]=NULL;
    }     
        
    /*Store BRAP_ProcessingStageHandle in BRAP_Handle*/
    for(i=0; i < BRAP_MAX_PP_SUPPORTED ; i++)
    {    
        if(!(BRAP_P_IsPointerValid((void *)hRap->hAudioProcessingStageHandle[i])))
        {
            hRap->hAudioProcessingStageHandle[i]=handle;
            break;
        }
    }    
    if(i==BRAP_MAX_PP_SUPPORTED)
    {
        BDBG_ERR(("Cann't Create more stages, reached Max limit = %d ",BRAP_MAX_PP_SUPPORTED));
        BKNI_Free(handle);
        return BERR_NOT_SUPPORTED;
    }
        
    BDBG_LEAVE(BRAP_CreateProcessingStage);
    return rc;

}
                                                                               
/***************************************************************************
Summary:
    Gets default audio processing stage settings.

Description:
    This function gets the default processing stage settings for a given processing algorithm.

Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    BRAP_CreateProcessingStage
    BRAP_DestroyProcessingStage
    BRAP_GetCurrentProcessingStageSettings
    BRAP_SetProcessingStageSettings
    
**************************************************************************/
BERR_Code BRAP_GetDefaultProcessingStageSettings(
    BRAP_ProcessingType                     eProcessingType,
                                                                /* [in] Audio processing
                                                                    algorithm */
    BRAP_ProcessingStageSettings       *psProcessingStageSettings 
                                                                            /* [out] Audio Processing Stage
                                                                                settings */
)
{
	BDBG_ENTER(BRAP_GetDefaultProcessingStageSettings);

    /* Copy the Processing Type */
	psProcessingStageSettings->eAudioProcessing = eProcessingType;
    
	switch(eProcessingType)
	{
		case BRAP_ProcessingType_eEncodeDts:
            psProcessingStageSettings->uConfigParams.sDTSENCParams = sDefDTSENCConfigSettings;
			break;
		case BRAP_ProcessingType_eEncodeAc3:
            psProcessingStageSettings->uConfigParams.sAc3ENCParams = sDefAc3ENCConfigSettings;
			break;
		case BRAP_ProcessingType_eAvl:
			psProcessingStageSettings->uConfigParams.sAvlParams = sDefAVLConfigSettings;
			break;
		case BRAP_ProcessingType_ePl2:
			psProcessingStageSettings->uConfigParams.sPl2Params = sDefProLogicIIConfigSettings;
			break;
		case BRAP_ProcessingType_eSrsXt:
			psProcessingStageSettings->uConfigParams.sSrsXtParams = sDefTruSurroundXtSettings;
			break;
		case BRAP_ProcessingType_eSrsHd:
			psProcessingStageSettings->uConfigParams.sSrsHdParams = sDefTruSurroundHdSettings;
			break;
		case BRAP_ProcessingType_eSrsTruVolume:
			psProcessingStageSettings->uConfigParams.sSrsTruVolumeParams = sDefSrsTruVolumeSettings;
			break;
		case BRAP_ProcessingType_eXen:
			psProcessingStageSettings->uConfigParams.sXenParams= sDefXenConfigSettings;
			break;
		case BRAP_ProcessingType_eBbe:
			psProcessingStageSettings->uConfigParams.sBbeParams = sDefBbeConfigSettings;
			break;
		case BRAP_ProcessingType_eCustomSurround:
			psProcessingStageSettings->uConfigParams.sCustomSurroundParams = sDefCustomSurroundConfigSettings;
			break;
		case BRAP_ProcessingType_eCustomBass:
			psProcessingStageSettings->uConfigParams.sCustomBassParams = sDefCustomBassConfigSettings;
			break;
		case BRAP_ProcessingType_eCustomVoice:
                psProcessingStageSettings->uConfigParams.sCustomVoiceConfigParams = sDefCustomVoiceConfigSettings;            
                break;	            
		case BRAP_ProcessingType_eAdPan:
                psProcessingStageSettings->uConfigParams.sAdPanParams.uiVol = 0x7FFFFFFF;
                psProcessingStageSettings->uConfigParams.sAdPanParams.bEnableAd = true;
                psProcessingStageSettings->uConfigParams.sAdPanParams.uiAudioRampTimeInMs = 1000;
			break;
		case BRAP_ProcessingType_eDolbyVolume:
                psProcessingStageSettings->uConfigParams.sDolbyVolConfigParams = sDefDolbyVolUserConfig;            
                break;	                        
		case  BRAP_ProcessingType_eEncodeMp3:
                psProcessingStageSettings->uConfigParams.sMp3EncConfigParams = sDefMp3EncConfigSettings;
                break;
		case  BRAP_ProcessingType_eEncodeSbc:
                psProcessingStageSettings->uConfigParams.sSbcEncConfigParams = sDefSbcEncConfigSettings;
                break;            
		case BRAP_ProcessingType_eBrcm3DSurround:
            psProcessingStageSettings->uConfigParams.sBrcm3DSurroundParams = sDefBrcm3DSurroundConfigSettings;
            break;            
        case BRAP_ProcessingType_eMonoDownmix:
            psProcessingStageSettings->uConfigParams.sMonoDownmixParams = sDefMonoDownmixConfigSettings;
            break;
        case BRAP_ProcessingType_eAudysseyVolume:
            psProcessingStageSettings->uConfigParams.sAudysseyVolParams = sDefAudysseyVolConfigSettings;
            break;
        case BRAP_ProcessingType_eAudysseyABX:
            psProcessingStageSettings->uConfigParams.sAudysseyABXParams = sDefAudysseyABXConfigSettings;
            break;            
        case BRAP_ProcessingType_eDV258:
            psProcessingStageSettings->uConfigParams.sDV258Params = sDefDV258ConfigSettings;
            break;   
        case BRAP_ProcessingType_eDDRE:
            psProcessingStageSettings->uConfigParams.sDDREParams = sDefDDREConfigSettings;
            break;
        case BRAP_ProcessingType_eBtsc:
            psProcessingStageSettings->uConfigParams.sBTSCParams = sDefBtscConfigSettings;
            break;             
        case BRAP_ProcessingType_eSrsCsTd:
            psProcessingStageSettings->uConfigParams.sSrsCsTdParams = sDefSrsCsTdConfigSettings;
            break;            
        case BRAP_ProcessingType_eSrsEqHl:
            psProcessingStageSettings->uConfigParams.sSrsEqHlParams = sDefSrsEqHlConfigSettings;
            break;                        
        case BRAP_ProcessingType_eSRC:
            psProcessingStageSettings->uConfigParams.sSRCParams = sDefSRCConfigSettings;
            break;
		case BRAP_ProcessingType_ePCMRouter:
		case BRAP_ProcessingType_eDSOLA:
		case BRAP_ProcessingType_ePassthru:
		case  BRAP_ProcessingType_eWmaProPassThru:
		case  BRAP_ProcessingType_eConvertDdpToAc3:          
		case BRAP_ProcessingType_eAdFade:
          case BRAP_ProcessingType_eGenericPassthru:
            case BRAP_ProcessingType_eFwMixer:            
            /*  Since the psProcessingStageSettings->eAudioProcessing is populated above
                nothing else to do here. */
			break;
            
		case BRAP_ProcessingType_eMax:
		case BRAP_ProcessingType_eNone:
		default:
			BDBG_MSG(("Unknown ProcessingType "));
	}

	BDBG_LEAVE(BRAP_GetDefaultProcessingStageSettings);

    return BERR_SUCCESS;
    
}

/***************************************************************************
Summary:
    Gets current audio processing stage settings.

Description:
    This function gets the current processing stage settings for a given processing stage
    handle.

Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    BRAP_CreateProcessingStage
    BRAP_DestroyProcessingStage
    BRAP_GetDefaultProcessingStageSettings
    BRAP_SetProcessingStageSettings
    
**************************************************************************/
BERR_Code BRAP_GetCurrentProcessingStageSettings(
BRAP_ProcessingStageHandle   hAudioProcessingStageHandle,  
                                                                    /* [in] Handle for the audio processing
                                                                        stage */
BRAP_ProcessingStageSettings       *psProcessingStageSettings 
                                                                        /* [out] Audio Processing Stage
                                                                            settings */
)
{
    BDBG_ENTER(BRAP_GetCurrentProcessingStageSettings); 
    
    BDBG_ASSERT(hAudioProcessingStageHandle);
    BDBG_ASSERT(psProcessingStageSettings);    
    /*Return BRAP_ProcessingStageSettings from BRAP_ProcessingStageHandle*/
    *psProcessingStageSettings=hAudioProcessingStageHandle->sProcessingStageSettings;

    BDBG_LEAVE(BRAP_GetCurrentProcessingStageSettings);

    return BERR_SUCCESS;
}


/***************************************************************************
Summary:
    Sets audio processing stage settings.

Description:
    This function sets  the processing stage settings for a given processing stage handle.

Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    BRAP_CreateProcessingStage
    BRAP_DestroyProcessingStage
    BRAP_GetDefaultProcessingStageSettings
    BRAP_GetCurrentProcessingStageSettings
    
**************************************************************************/
BERR_Code BRAP_SetProcessingStageSettings(
    BRAP_ProcessingStageHandle   hAudioProcessingStageHandle,  
                                                                        /* [in] Handle for the audio processing
                                                                            stage */
    const BRAP_ProcessingStageSettings       *psProcessingStageSettings 
                                                                            /* [in] Audio Processing Stage
                                                                                settings */
)
{
    BERR_Code err = BERR_SUCCESS;
    unsigned int i=0,k=0,j =0;
    bool bStageSetting=false;
    BRAP_DSPCHN_Handle	hDspCh = NULL;

    unsigned int uiTaskId = BRAP_INVALID_VALUE;
    unsigned int uiStartNodeIndex, uiConfigBufAddr;
    bool bPpStageFound = false;
    BRAP_Fwif_P_FwBufInfo  *psFwBufInfo;
    BRAP_FWIF_P_Command sCommand;
    BRAP_FWIF_P_Response sRsp;
    BRAP_Handle hRap;
    BRAP_DSP_Handle hDsp;
    BRAP_CIT_P_OpStgInfo *psOpStgInfo;
    BRAP_FWIF_P_FwTaskHandle hFwTask;
    BRAP_P_MsgType      eMsgType;

    unsigned int uiCitBranchId = BRAP_INVALID_VALUE, uiCitStageId = BRAP_INVALID_VALUE;

    unsigned int uiActualConfigSize = 0;
    

    BDBG_ENTER( BRAP_SetProcessingStageSettings );
    BDBG_ASSERT(hAudioProcessingStageHandle);
    BDBG_ASSERT(psProcessingStageSettings);

    BKNI_Memset((void *)&sRsp,0,sizeof(BRAP_FWIF_P_Response));
    /*check if any of the channels using this stage is in "start" condition. 
    If yes, compare new settings with old settings */
    if(true == hAudioProcessingStageHandle->bChSpecificStage)  /*Post processing stage added before mixer*/
    {
        if((hAudioProcessingStageHandle->uHandle.hRapCh) && 
           (hAudioProcessingStageHandle->uHandle.hRapCh->eState ==  BRAP_P_State_eStarted))
        {
            bStageSetting=true;
            hDspCh = BRAP_DSPCHN_P_GetDspChHandle(&(hAudioProcessingStageHandle->uHandle.hRapCh), true);
            if(!(BRAP_P_IsPointerValid((void *)hDspCh)))
            {
                BDBG_ERR(("Error!!! DspCh Can't be NULL"));
                err = BERR_TRACE(BRAP_ERR_BAD_DEVICE_STATE);
                goto end;
            }
        }
    }
    else if(false == hAudioProcessingStageHandle->bChSpecificStage) 
    {   /*Post processing stage added after mixer*/
        if(BRAP_P_IsPointerValid((void *)hAudioProcessingStageHandle->uHandle.hAssociation))
        {
            hDspCh = BRAP_DSPCHN_P_GetDspChHandle(&(hAudioProcessingStageHandle->uHandle.hAssociation), false);
            if(!(BRAP_P_IsPointerValid((void *)hDspCh)))
            {
                BDBG_MSG(("DspCh is NULL, Rap Channel is not started"));
            } 
            else
            {
                if(hDspCh->eChState == BRAP_DSPCHN_P_ChState_eStart)
                    bStageSetting=true; 
            }
        }
    }
  
    if(bStageSetting==true)
    {
        if(psProcessingStageSettings->eAudioProcessing != hAudioProcessingStageHandle->sProcessingStageSettings.eAudioProcessing)
        {   
            BDBG_ERR(("Audio processing type  cannot be changed on the fly.Channel must be in stopped state!"));
            err =  BERR_TRACE( BERR_NOT_SUPPORTED );
            goto end;
        }
    
        /*If any of the channels using this stage is in "start" condition,  call BRAP_P_SetStageSettings() function.*/
        /* TODO*/
    }

    /*Update BRAP_ProcessingStageSettings in BRAP_ProcessingStageHandle with new settings*/
    BKNI_EnterCriticalSection();
    hAudioProcessingStageHandle->sProcessingStageSettings = *psProcessingStageSettings;
    BKNI_LeaveCriticalSection();
    if(bStageSetting==true)
    {
        for(i = 0; i<BRAP_FWIF_P_MAX_FW_TASK_PER_DSPCHN; i++)
        {
            if(BRAP_P_IsPointerValid((void *)hDspCh->sFwTaskInfo[i].hFwTask))
            {
                for(j=0; j < BRAP_P_MAX_DST_PER_RAPCH; j++)
                {
                    for(k=0 ; k < BRAP_MAX_STAGE_PER_BRANCH_SUPPORTED ; k++)
                    {
                        if (hDspCh->sFwTaskInfo[i].sProcessingNw.sAudProcessingStage[j][k].hAudioProcessing == hAudioProcessingStageHandle)
                        {
                            hRap = hAudioProcessingStageHandle->hRap;
                            psFwBufInfo = &(hRap->sMemInfo.sConfigParamBufInfo);                        
                            hFwTask = hDspCh->sFwTaskInfo[i].hFwTask;
                            uiTaskId = hFwTask->uiTaskId;
                            hDsp = hFwTask->hDsp;                                                    
                            uiCitBranchId = hDspCh->sFwTaskInfo[i].sProcessingNw.sAudProcessingStage[j][k].ui32CitBranchId;
                            uiCitStageId = hDspCh->sFwTaskInfo[i].sProcessingNw.sAudProcessingStage[j][k].ui32CitStageId;

                            if((BRAP_INVALID_VALUE == uiCitBranchId)
                                ||(BRAP_INVALID_VALUE == uiCitStageId))
                            {
                                BDBG_ERR(("Error Cit Branch/Stage Id corresponding to Processing stage can't be invalid"));
                                err = BERR_TRACE(BERR_INVALID_PARAMETER);
                                goto end;
                            }
                            
                            psOpStgInfo = &(hFwTask->sCitOutput.sCitBranchInfo[uiCitBranchId].sCitStgInfo[uiCitStageId]);
                            uiStartNodeIndex = psOpStgInfo->ui32StartNodeIndex;
                            uiConfigBufAddr = psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32UserParamBufAdr;

                            err = BRAP_FWIF_P_SetProcessingStageConfigParams(hAudioProcessingStageHandle,
                                psFwBufInfo ->ui32BaseAddr, psFwBufInfo ->ui32Size,&uiActualConfigSize);
                            if( BERR_SUCCESS!=err )
                            {
                                BDBG_ERR(("Error in setting config params for Processing type %d",hAudioProcessingStageHandle->sProcessingStageSettings.eAudioProcessing));
                                err = BERR_TRACE(BERR_INVALID_PARAMETER);
                                goto end;
                            } 
                            
                        	BDBG_MSG(("uiActualConfigSize =%d , psFwBufInfo->ui32Size =%d",uiActualConfigSize,psFwBufInfo->ui32Size));
                            /* Create Change-Config command */
                            sCommand.sCommandHeader.ui32CommandID = BRAP_FWIF_P_ALGO_PARAMS_CFG_COMMAND_ID;
                            sCommand.sCommandHeader.ui32CommandCounter = hRap->uiCommandCounter++;
                            sCommand.sCommandHeader.ui32TaskID = uiTaskId;
                            sCommand.sCommandHeader.eResponseType = BRAP_FWIF_P_ResponseType_eResponseRequired;
                            BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(psFwBufInfo ->ui32BaseAddr), 
                                &(sCommand.uCommand.sCfgChange.ui32HostConfigParamBuffAddr));	
                            BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(uiConfigBufAddr), 
                                &(sCommand.uCommand.sCfgChange.ui32DspConfigParamBuffAddr));	
                            sCommand.uCommand.sCfgChange.ui32SizeInBytes = uiActualConfigSize;

                            hFwTask->uiLastEventType = sCommand.sCommandHeader.ui32CommandID;
                            BRAP_P_EventReset(hDsp,BRAP_GET_TASK_INDEX(hFwTask->uiTaskId));
                            err = BRAP_FWIF_P_SendCommand(hDsp->hCmdQueue, &sCommand,hRap,hFwTask);
                            if(BERR_SUCCESS != err)
                            {
                                if((hRap->bWatchdogTriggered == false)
                                    &&(hFwTask->bStopped == false))
                                {
                                    BDBG_ERR(("BRAP_SetProcessingStageSettings: CFG_Command failed!"));
                                    return BERR_TRACE(err);
                                }
                                else
                                    err = BERR_SUCCESS;     

                            }
                            /* Wait for Ack_Response_Received event w/ timeout */
                            err = BRAP_P_EventWait(hDsp, BRAP_DSPCHN_P_EVENT_TIMEOUT_IN_MS,BRAP_GET_TASK_INDEX(hFwTask->uiTaskId));
                            if(BERR_TIMEOUT == err)
                            {
                                if((hRap->bWatchdogTriggered == false))
                                {
                                    /* Please note that, If the code reaches at this point then there is a potential Bug in Fw 
                                    code which needs to be debugged. However Watchdog is being triggered to recover the system*/            
                                    BDBG_WRN(("BRAP_SetProcessingStageSettings: CFG_Command ACK timeout! Triggering Watchdog"));
#if 0                
                                    BDBG_ASSERT(0);                
#endif
                                    BRAP_Write32(hDsp->hRegister, BCHP_AUD_DSP_INTH0_R5F_SET+ hDsp->ui32Offset,0x1);
                                    hRap->bWatchdogTriggered  = true;
#if 0                
                                    err = BERR_TRACE(err);
                                    goto error;
#endif                 
                                    err = BERR_SUCCESS;              
                                }
                                else
                                    err = BERR_SUCCESS;              
                            }
                            
                            eMsgType = BRAP_P_MsgType_eSyn;
                            if((hRap->bWatchdogTriggered == false)
                            &&(hFwTask->bStopped == false))
                            {
                                err = BRAP_FWIF_P_GetMsg(hFwTask->hSyncMsgQueue, (void *)&sRsp, eMsgType);
                            }
                            if(BERR_SUCCESS != err)
                            {
                                if((hRap->bWatchdogTriggered == false)
                                    &&(hFwTask->bStopped == false))
                                {
                                    BDBG_ERR(("BRAP_SetProcessingStageSettings: Unable to read ACK!"));
                                    return BERR_TRACE(err);
                                }
                                else
                                    err = BERR_SUCCESS;                                
                            }
                            if((sRsp.sCommonAckResponseHeader.eStatus != BERR_SUCCESS)||
                                (sRsp.sCommonAckResponseHeader.ui32ResponseID != BRAP_FWIF_P_ALGO_PARAMS_CFG_RESPONSE_ID)||
                                (sRsp.sCommonAckResponseHeader.ui32ResponseCounter!=sCommand.sCommandHeader.ui32CommandCounter)||
                                (sRsp.sCommonAckResponseHeader.ui32TaskID != hFwTask->uiTaskId))
                            {
                                if((hRap->bWatchdogTriggered == false)
                                    &&(hFwTask->bStopped == false))
                                {
                                    BDBG_ERR(("BRAP_SetProcessingStageSettings: CFG_Command response not received successfully!"));
                                    return BERR_TRACE(sRsp.sCommonAckResponseHeader.eStatus);
                                }
                                else
                                    err = BERR_SUCCESS;                                
                            }
                            bPpStageFound = true;
                        }
                        if(true == bPpStageFound)
                            break;
                    }
                    if(true == bPpStageFound)
                        break;
                }                
            }
            if(true == bPpStageFound)
                break;            
        }
        if(false == bPpStageFound)
        {
            BDBG_ERR(("Can't find any valid stage in CIT network to set the user config params for processing algo"));
            err = BERR_TRACE(BERR_INVALID_PARAMETER);
            goto end;
        }
    }

end:    
    BDBG_LEAVE( BRAP_SetProcessingStageSettings); 

    return err;
}

/***************************************************************************
Summary:
    Destroys audio processing stage.

Description:
    This function free up the processing Handle memory.This function will not free's the memory 
    which is allocated in BRAP_CreateProcessingStage.To free the memory which is allocated in 
    create stage,we must call the BRAP_DestroyProcessingStage.


Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    BRAP_CreateProcessingStage,BRAP_DestroyProcessingStage
    
**************************************************************************/
BERR_Code BRAP_P_DestroyProcessingStage(
    BRAP_ProcessingStageHandle   hAudioProcessingStageHandle  
                                                                        /* [in] Handle for the audio processing
                                                                            stage */
)
{
    BDBG_ENTER( BRAP_P_DestroyProcessingStage );
    BDBG_ASSERT(hAudioProcessingStageHandle);

    /*Free BRAP_ProcessingStageHandle*/            
    BKNI_Free( hAudioProcessingStageHandle);
        
    BDBG_LEAVE( BRAP_P_DestroyProcessingStage);       
    return BERR_SUCCESS;
}


/***************************************************************************
Summary:
    Destroys audio processing stage.

Description:
    This function destroys an audio processing stage.Before destroying the stage, function will
    check if the stage is used by any destination or channel.And this function free's the memory with is
    allocated in BRAP_CreateProcessingStage.

Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    BRAP_CreateProcessingStage
    
**************************************************************************/
BERR_Code BRAP_DestroyProcessingStage(
    BRAP_ProcessingStageHandle   hAudioProcessingStageHandle  
                                                                        /* [in] Handle for the audio processing
                                                                            stage */
)
{
    unsigned  int i=0;

    BDBG_ENTER( BRAP_DestroyProcessingStage );
    BDBG_ASSERT(hAudioProcessingStageHandle);

    /*Check if this stage is added as a post processing stage for any destination,
     If yes, return BERR_NOT_SUPPORTED */
    for(i=0;i< BRAP_P_MAX_DEST_PER_PROCESSING_STAGE ; i++)
    {
        if (hAudioProcessingStageHandle->hDestHandle[i]!=NULL)
        {
            BDBG_ERR(("Cann't Destory : This Stage is added to the destination 0x%x as a post processing stage",
                hAudioProcessingStageHandle->hDestHandle[i]));
            return BERR_TRACE( BERR_NOT_SUPPORTED );
        }
    }
    
#if 0 /* TODO: Re-enable these after adding new API for stage removal */
    /*Check if this stage is added as a pre processing stage  for any channel ,
    If yes, return BERR_NOT_SUPPORTED*/
    /*Note : The processing stage is pre-processing stage 
                1.If Rap channel is not NULL and 
                2. For that processing stage the all the destination handles are NULL*/
    for(i=0;i<BRAP_P_MAX_DEST_PER_PROCESSING_STAGE;i++)
    {
        if (hAudioProcessingStageHandle->hRapCh!=NULL)
        {
            BDBG_ERR(("Cann't Destory : This Stage is added to the channel 0x%x as pre-processing stage",
                hAudioProcessingStageHandle->hRapCh));
            return BERR_TRACE( BERR_NOT_SUPPORTED );
        }
    }
#endif    

    /*Initialize corresponding entry in BRAP_Handle to NULL.*/
    for(i=0; i < BRAP_MAX_PP_SUPPORTED ; i++)
    {
        if(hAudioProcessingStageHandle->hRap->hAudioProcessingStageHandle[i] == hAudioProcessingStageHandle)
        {
            hAudioProcessingStageHandle->hRap->hAudioProcessingStageHandle[i] =NULL;
            break;
        }
    }
    if(BRAP_MAX_PP_SUPPORTED == i)
    {
        BDBG_ERR(("Unable to find Processing Stage Handle in RAP Handle!"));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    
    /* Free the Handle*/
    BRAP_P_DestroyProcessingStage(hAudioProcessingStageHandle);
    
    BDBG_LEAVE( BRAP_DestroyProcessingStage);       
    
    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
	Gets DSP context default settings

Description:
	The default configuration for DSP context (DSPCHN channel) are returned
	
Returns:
    BERR_SUCCESS                - if successful 

See Also:
	BRAP_DSPCHN_P_Open
**************************************************************************/
BERR_Code 
BRAP_DSPCHN_P_GetDefaultSettings( 
   BRAP_DSPCHN_P_Settings	*psDefSettings	/* [out] The DEC channel default configuration setting */
   )
{
	/* Assert on input parameters */
	BDBG_ASSERT(psDefSettings);
	*psDefSettings = sDefDspChnSettings;
	return BERR_SUCCESS;
}	

/***************************************************************************
Summary:
	Gets DSP context current settings

Description:
	The current configuration for DSP context (DSPCHN channel) are returned
	
Returns:
    BERR_SUCCESS                - if successful 

See Also:
	BRAP_DSPCHN_P_Open
**************************************************************************/
BERR_Code 
BRAP_DSPCHN_P_GetCurrentSettings(
	BRAP_DSPCHN_Handle		hDspCh,			/* [in] DSP channel handle */
   BRAP_DSPCHN_P_Settings	*psDefSettings	/* [out] The DEC channel default configuration setting */
   )
{
	BDBG_ASSERT(hDspCh);
	BDBG_ASSERT(psDefSettings);

	*psDefSettings = hDspCh->sSettings;
	return BERR_SUCCESS;
}	
/***************************************************************************
Summary:
	Gets DSP context default params 

Description:
	The default parameters for DSP context (DSPCHN channel) are returned
	
Returns:
    BERR_SUCCESS                - if successful 

See Also:
	BRAP_DSPCHN_P_Open
**************************************************************************/
BERR_Code 
BRAP_DSPCHN_P_GetDefaultParams( 
   BRAP_DSPCHN_P_AudioParams	*psDefParams	/* [out] The DEC channel default parameters */
   )
{
    unsigned i=0;
	BDBG_ASSERT(psDefParams);
	
	BKNI_Memset(psDefParams,0,sizeof(*psDefParams));
	psDefParams->sExtAudioParams.eOutputSampleRate = BAVC_AudioSamplingRate_eUnknown;
    psDefParams->sExtAudioParams.bPPMCorrection = true;
    psDefParams->sExtAudioParams.uiPBRate = BRAP_DSPCHN_PLAYBACKRATE_NORMAL;
	psDefParams->bUpdateChStatusParams = true;
    
    psDefParams->bFwToUpdateRdPtr = true;
	psDefParams->sExtAudioParams.bHbrCompressed = false;
	psDefParams->sExtAudioParams.bDtsCdCompressed = false;
    psDefParams->sExtAudioParams.bEnableTargetSync= true;

    for(i=0; i < BRAP_AF_P_MAX_NUM_SPDIF_PORTS ; i++)
    {
        BRAP_OP_P_GetDefaultSpdifChanStatusParams(&(psDefParams->sSpdifChStatusParams[i]));
    }

#ifdef RAP_VIDEOONDSP_SUPPORT    
    psDefParams->sExtVideoParams.ui32DispStripeWidth = 64;
    psDefParams->sExtVideoParams.ui32DispStripeHeight = 512;
    psDefParams->sExtVideoParams.ui32NumBuffAvl = 16;
#endif
    
	return BERR_SUCCESS;
}	
/***************************************************************************
Summary: Opens a DSP context 

Description:
	This function is responsible for opening a DSP context (DSPCHN channel). 
	When a DSP context is opened, it will create a module channel handle 
	and configure the module based on the channel settings. 
	Once a channel is opened, it must be closed before it can be opened 
	again.

Returns:
	BERR_SUCCESS - If successful

See Also:
	BRAP_DSPCHN_P_Close,
	BRAP_DSPCHN_P_GetDefaultSettings
**************************************************************************/
BERR_Code 
BRAP_DSPCHN_P_Open(
	BRAP_DSPCHN_Handle		*phDspChn,		/* [out] DSP channel handle */
	void	                *pHandle,           /* [in] Raptor Channel or Association handle */	
    bool                    bChSpecificDspCh,   /* [in] DSP channel handle is Raptor Channel 
                                                        or Association specific */	
	const BRAP_DSPCHN_P_Settings	*pDefSettings	/* [in] The DSP channel configuration setting */
	)
{
    BRAP_DSPCHN_Handle hHandle = NULL;
    bool                    bWdgRecovery;
    unsigned int i=0,j=0;
    BRAP_ChannelHandle              hRapCh = NULL;        
    BRAP_AssociatedChannelHandle    hAssociation = NULL;    
    BRAP_DSPCHN_P_FirmwareTaskInfo  *psTempTaskInfo = NULL;
    BRAP_P_ChannelAudioProcessingStage *psTempStage = NULL;
    BERR_Code err=BERR_SUCCESS;
    
    BDBG_ENTER( BRAP_DSPCHN_P_Open );
    
    /* Assert on invalid input parameters */
    BDBG_ASSERT( phDspChn );
    BDBG_ASSERT( pHandle );

    if(bChSpecificDspCh == true)
    {
        hRapCh = *(BRAP_ChannelHandle *)pHandle;
        BDBG_ASSERT(hRapCh);
    bWdgRecovery = BRAP_P_GetWatchdogRecoveryFlag(hRapCh->hRap);
    }
    else
    {
        hAssociation = *(BRAP_AssociatedChannelHandle *)pHandle;
        BDBG_ASSERT(hAssociation);        
        bWdgRecovery = BRAP_P_GetWatchdogRecoveryFlag(hAssociation->hRap);
    }
    
    if(false == bWdgRecovery)
    {
    /* Allocate memory for handle */
    hHandle = ( BRAP_DSPCHN_Handle ) BKNI_Malloc( sizeof( BRAP_DSPCHN_P_Channel ) );
    if( NULL== hHandle )
    {
        err = BERR_TRACE( BERR_OUT_OF_SYSTEM_MEMORY );
        goto exit;
    }
	BKNI_Memset( hHandle, 0, sizeof(BRAP_DSPCHN_P_Channel) );

    psTempTaskInfo = ( BRAP_DSPCHN_P_FirmwareTaskInfo * ) BKNI_Malloc( sizeof( BRAP_DSPCHN_P_FirmwareTaskInfo ) );
    if( NULL== psTempTaskInfo )
    {
        err = BERR_TRACE( BERR_OUT_OF_SYSTEM_MEMORY );
        goto error;
    }
	BKNI_Memset( psTempTaskInfo, 0, sizeof(BRAP_DSPCHN_P_FirmwareTaskInfo) );

    psTempStage = ( BRAP_P_ChannelAudioProcessingStage * ) BKNI_Malloc( sizeof( BRAP_P_ChannelAudioProcessingStage ) );
    if( NULL== psTempStage )
    {
        err = BERR_TRACE( BERR_OUT_OF_SYSTEM_MEMORY );
        goto error;
    }
	BKNI_Memset( psTempStage, 0, sizeof(BRAP_P_ChannelAudioProcessingStage) );    

    /* Initialize the handle */
    *phDspChn = hHandle;

    hHandle->sSettings = *pDefSettings;

        hHandle->bChSpecificDspCh = bChSpecificDspCh;
        
        if(bChSpecificDspCh == true)
        {
            hHandle->uHandle.hRapCh = hRapCh;    
            hHandle->hHeap = hRapCh->hHeap;
            hHandle->hRegister = hRapCh->hRegister;
            hHandle->hChip = hRapCh->hChip;
            hHandle->hInt = hRapCh->hInt;        
        }
        else
        {
            hHandle->uHandle.hAssociation = hAssociation;        
            hHandle->hHeap = hAssociation->hRap->hHeap;
            hHandle->hRegister = hAssociation->hRap->hRegister;
            hHandle->hChip = hAssociation->hRap->hChip;
            hHandle->hInt = hAssociation->hRap->hInt;        
        }
        
    /* Initialize the state variables */
    hHandle->eChState = BRAP_DSPCHN_P_ChState_eStop;

        /*Initialize NetworkInfo */
        hHandle->sNetworkInfo.eAudioType = BRAP_DSPCHN_AudioType_eInvalid;
#ifdef RAP_VIDEOONDSP_SUPPORT
        hHandle->sNetworkInfo.eVideoType = BRAP_DSPCHN_VideoType_eInvalid;        
#endif
        hHandle->sNetworkInfo.eDecodeMode = BRAP_DSPCHN_DecodeMode_eInvalid;    
        hHandle->sNetworkInfo.eNetworkInputType= BRAP_CIT_P_FwStgSrcDstType_eInvalid;        
        hHandle->sNetworkInfo.sInputBufConfig.uIoBuffer.sIoBuffer.eBufferType= BRAP_AF_P_BufferType_eInvalid;      
        hHandle->sNetworkInfo.sInputBufConfig.uIoGenericBuffer.sIoGenericBuffer.eBufferType= BRAP_AF_P_BufferType_eInvalid;         
        for(i =0 ; i < BRAP_P_MAX_DST_PER_RAPCH; i++)
        {
            hHandle->sNetworkInfo.sDspOutConfig[i].hDestHandle = NULL;             
            hHandle->sNetworkInfo.sDspOutConfig[i].sOutputBufConfig.uIoBuffer.sIoBuffer.eBufferType= BRAP_AF_P_BufferType_eInvalid;                 
            hHandle->sNetworkInfo.sDspOutConfig[i].sOutputBufConfig.uIoGenericBuffer.sIoGenericBuffer.eBufferType= BRAP_AF_P_BufferType_eInvalid;                     
            hHandle->sNetworkInfo.sDspOutConfig[i].sOutputBufConfig.ui32IndepDelay = 0;
        }
        psTempTaskInfo->hFwTask = NULL;

        psTempStage->bCloneStage = false;
        psTempStage->bDecoderStage = false;
        psTempStage->bInternalProcessingStage = false;    
        psTempStage->hAudioProcessing = NULL;
        psTempStage->hValidDestHandle = NULL;
        for(i=0;i<BRAP_P_MAX_DEST_PER_PROCESSING_STAGE;i++)    
        {
            psTempStage->hDestHandle[i] = NULL;
        }
        psTempStage->ui32MasterBranchId = BRAP_INVALID_VALUE;
        psTempStage->ui32MasterStageId = BRAP_INVALID_VALUE;
        psTempStage->ui32CitBranchId = BRAP_INVALID_VALUE;
        psTempStage->ui32CitStageId = BRAP_INVALID_VALUE;

        for (i=0;i<BRAP_P_MAX_DST_PER_RAPCH;i++)
        {
            for(j=0;j<BRAP_MAX_STAGE_PER_BRANCH_SUPPORTED;j++)
            {
                psTempTaskInfo->sProcessingNw.sAudProcessingStage[i][j] = *psTempStage;
            }
        }        

        for (i=0;i<BRAP_FWIF_P_MAX_FW_TASK_PER_DSPCHN;i++)
        {
            hHandle->sFwTaskInfo[i] = *psTempTaskInfo;
        }
    }
    else
    {
        hHandle =*phDspChn;
        /* Is this required in case of watchdog recovery */
   /*     hHandle->eChState = BRAP_DSPCHN_P_ChState_eStop;        */
    }


    goto exit;
    
    error:
        if(BRAP_P_IsPointerValid((void *)hHandle))
            BKNI_Free(hHandle);
    exit:        
        if(BRAP_P_IsPointerValid((void *)psTempStage))
            BKNI_Free(psTempStage);
        
        if(BRAP_P_IsPointerValid((void *)psTempTaskInfo))
            BKNI_Free(psTempTaskInfo);        
    
    BDBG_LEAVE( BRAP_DSPCHN_P_Open );
    
    return err;
}	
/***************************************************************************
Summary: Closes an DSP context

Description:
	This function is responsible for closing a DSP context and releasing
	all the resources associated with this context.
	
Returns:
	BERR_SUCCESS - If successful

See Also:
	BRAP_DSPCHN_P_Open
**************************************************************************/
BERR_Code 
BRAP_DSPCHN_P_Close( 
	BRAP_DSPCHN_Handle hDspChn /* [in] DSP handle */
	)
{
    BDBG_ENTER(BRAP_DSPCHN_P_Close);
    BDBG_ASSERT(hDspChn);

	/* Free DSP channel handle */
    BKNI_Free(hDspChn);
    
    BDBG_LEAVE(BRAP_DSPCHN_P_Close);
	return BERR_SUCCESS;
}	
/***************************************************************************
Summary: This is a dummy start for a DSP Context

Description:
	This function is responsible for copying the DSP Channel parameters from
	Audio params. It does not do the actual DSP channel start.
	
Returns:
	BERR_SUCCESS - If successful

See Also:
	BRAP_DSPCHN_P_Start
**************************************************************************/
BERR_Code
BRAP_DSPCHN_P_DummyStart(
	BRAP_DSPCHN_Handle	pDspChn,					/* [in] DSPCHN handle */
	const BRAP_DSPCHN_P_AudioParams *psAudioParams	/* [in] Audio decode parameters */
	)
{
	BERR_Code ret = BERR_SUCCESS;
	
	BDBG_ENTER(BRAP_DSPCHN_P_DummyStart);

	/* Assert on the arguments */
	BDBG_ASSERT(pDspChn);

    /* Copy the dspchn params to the dspchn handle from audio params */
	pDspChn->sDspAudioParams = *psAudioParams;

	BDBG_LEAVE(BRAP_DSPCHN_P_DummyStart);

	return ret;
}		


 BERR_Code 
BRAP_DSPCHN_P_CreateFwTask(
        BRAP_DSPCHN_Handle hDspCh,
        BRAP_P_AudioProcNetwork     *psAudProcNetwork
        )
{
    BRAP_DSP_Handle hDsp = NULL;
    BRAP_Handle hRap;
    BRAP_FWIF_P_FwTaskHandle  hTask =NULL;
    BRAP_RM_P_ResrcReq      *psRsrcReq = NULL;
    BRAP_RM_P_ResrcGrant    *psRsrcGrnt = NULL;
    int i=0, j=0,uiPth = 0,uiPathIndx = 0;
    BERR_Code   err = BERR_SUCCESS;
    bool bFoundPath = false;
    BRAP_ChannelHandle hTempRapCh = NULL;

    BDBG_ENTER(BRAP_DSPCHN_P_CreateFwTask);
    BDBG_ASSERT(hDspCh);
    BDBG_ASSERT(psAudProcNetwork);    

    if(true == hDspCh->bChSpecificDspCh)
    {
        if(BRAP_P_IsPointerValid((void *)hDspCh->uHandle.hRapCh))
        {
            hRap = hDspCh->uHandle.hRapCh->hRap;
        }
        else
        {
            BDBG_ERR(("ERROR !! hRapCh is NULL"));
            err = BERR_TRACE(BERR_INVALID_PARAMETER);
            goto error;
        }
    }
    else
    {
        if(BRAP_P_IsPointerValid((void *)hDspCh->uHandle.hAssociation))
        {
            hRap = hDspCh->uHandle.hAssociation->hRap;
        }
        else
        {
            BDBG_ERR(("ERROR !! hAssociation is NULL"));
            err = BERR_TRACE(BERR_INVALID_PARAMETER);
            goto error;
        }
    }

      	/* Malloc large structures */
    psRsrcReq = hRap->sOpenTimeMallocs.psResrcReq;
    if ( NULL==psRsrcReq )
    {
	err = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
       goto error;
    }
	BKNI_Memset( psRsrcReq, 0, sizeof(BRAP_RM_P_ResrcReq) );    
    psRsrcGrnt = hRap->sOpenTimeMallocs.psResrcGrant;
    if ( NULL==psRsrcGrnt )
    {
        err = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        goto error;

    }
	BKNI_Memset( psRsrcGrnt, 0, sizeof(BRAP_RM_P_ResrcGrant) );            
    
        /* Allocating DSP to the Path */
     if(true == hDspCh->bChSpecificDspCh)
    {
        for(uiPth = 0; uiPth < BRAP_P_MAX_PATHS_IN_A_CHAN; uiPth++)
        {
            if((BRAP_P_IsPointerValid((void *)hDspCh->uHandle.hRapCh->pPath[uiPth]))
                && (hDspCh->uHandle.hRapCh->pPath[uiPth]->hDspCh == hDspCh))
                {
                    uiPathIndx = uiPth;
                    bFoundPath = true;                    
                    hTempRapCh = hDspCh->uHandle.hRapCh;
                    break;
                }
        }
        if(uiPth >= BRAP_P_MAX_PATHS_IN_A_CHAN)
        {
            BDBG_ERR(("No sPath allocated for the running DspCh"));
            err = BERR_TRACE(BERR_INVALID_PARAMETER); 
            goto error;            
        }
    }
     else
    {
        for(i = 0;i < BRAP_MAX_PRI_DEC_CHAN_IN_ASSOCIATED_GRP; i++)
        {
            if(!(BRAP_P_IsPointerValid((void *)hDspCh->uHandle.hAssociation->hPriDecCh[i])))
                continue;
            
            for(uiPth = 0; uiPth < BRAP_P_MAX_PATHS_IN_A_CHAN; uiPth++)
            {
                 if((BRAP_P_IsPointerValid((void *)hDspCh->uHandle.hAssociation->hPriDecCh[i]->pPath[uiPth]))
                        && (hDspCh->uHandle.hAssociation->hPriDecCh[i]->pPath[uiPth]->hDspCh == hDspCh))
                {
                    uiPathIndx = uiPth;
                    bFoundPath = true;
                    hTempRapCh = hDspCh->uHandle.hAssociation->hPriDecCh[i];
                    break;
                }
             }
            if(true ==bFoundPath)
                break;
        }
        if(false == bFoundPath)
            {
            for(i = 0;i < BRAP_MAX_SEC_DEC_CHAN_IN_ASSOCIATED_GRP; i++)
            {
                if(!(BRAP_P_IsPointerValid((void *)hDspCh->uHandle.hAssociation->hSecDecCh[i])))
                    continue;            
                for(uiPth = 0; uiPth < BRAP_P_MAX_PATHS_IN_A_CHAN; uiPth++)
                {
                     if((BRAP_P_IsPointerValid((void *)hDspCh->uHandle.hAssociation->hSecDecCh[i]->pPath[uiPth]))
                            && (hDspCh->uHandle.hAssociation->hSecDecCh[i]->pPath[uiPth]->hDspCh == hDspCh))
                    {
                        uiPathIndx = uiPth;                
                        bFoundPath = true;              
                        hTempRapCh = hDspCh->uHandle.hAssociation->hSecDecCh[i];
                        break;
                    }
                 }
                if(true ==bFoundPath)
                    break;
            }
        }
        if(false == bFoundPath)
        {
            for(i = 0;i < BRAP_MAX_PB_CHAN_IN_ASSOCIATED_GRP; i++)
            {
                if(!(BRAP_P_IsPointerValid((void *)hDspCh->uHandle.hAssociation->hPBCh[i])))
                    continue;                        
                for(uiPth = 0; uiPth < BRAP_P_MAX_PATHS_IN_A_CHAN; uiPth++)
                {                
                     if((BRAP_P_IsPointerValid((void *)hDspCh->uHandle.hAssociation->hPBCh[i]->pPath[uiPth]))
                            && (hDspCh->uHandle.hAssociation->hPBCh[i]->pPath[uiPth]->hDspCh == hDspCh))
                    {
                        uiPathIndx = uiPth;                
                        bFoundPath = true;           
                        hTempRapCh = hDspCh->uHandle.hAssociation->hPBCh[i];                        
                        break;
                    }
                 }
                if(true ==bFoundPath)
                    break;
            }
        }
        if(false == bFoundPath)
        {
            for(i = 0;i < BRAP_MAX_CAP_CHAN_IN_ASSOCIATED_GRP; i++)
            {
                if(!(BRAP_P_IsPointerValid((void *)hDspCh->uHandle.hAssociation->hCapCh[i])))
                    continue;                        
                for(uiPth = 0; uiPth < BRAP_P_MAX_PATHS_IN_A_CHAN; uiPth++)
                {
                     if((BRAP_P_IsPointerValid((void *)hDspCh->uHandle.hAssociation->hCapCh[i]->pPath[uiPth]))
                            && (hDspCh->uHandle.hAssociation->hCapCh[i]->pPath[uiPth]->hDspCh == hDspCh))
                    {
                        uiPathIndx = uiPth;                
                        bFoundPath = true;                    
                        hTempRapCh = hDspCh->uHandle.hAssociation->hCapCh[i];                        
                        break;
                    }
                 }
                if(true ==bFoundPath)
                    break;
            }
        }

        if(false == bFoundPath)
        {
            BDBG_ERR(("No sPath allocated for the running DspCh"));
            err = BERR_TRACE(BERR_INVALID_PARAMETER); 
            goto error;            
        }            

    }

    BRAP_RM_P_InitResourceReq(psRsrcReq);

    psRsrcReq->eChType  = hTempRapCh->eChannelType;
    psRsrcReq->eChSubType = hTempRapCh->eChannelSubType;  
    psRsrcReq->ePath = hTempRapCh->pPath[uiPathIndx]->eUsgPath;
    psRsrcReq->bAllocateDSP  = true;
    
    /* Call resource manager to allocate required resources. */
    err = BRAP_RM_P_AllocateResources(hRap->hRm, psRsrcReq, psRsrcGrnt);
    if(BERR_SUCCESS != err)
    {
    	BDBG_ERR(("DSP Resource alloc failed with err = %d", err));
    	err = BERR_TRACE(err); 
        goto error;        
    }
    /* Update the resource grant structure in hRapCh */
    err = BRAP_RM_P_UpdateResrcGrant(&(hTempRapCh->pPath[uiPathIndx]->sRsrcGrnt), 
                                                               psRsrcGrnt);
    if(BERR_SUCCESS != err)
    {
    	BDBG_ERR(("UpdateResrcGrant failed %d", err));
    	err = BERR_TRACE(err); 
        goto error;        
    }

    for(uiPth = 0; uiPth < BRAP_P_MAX_PATHS_IN_A_CHAN; uiPth++)
    {
        if(BRAP_P_IsPointerValid((void *)hTempRapCh->pPath[uiPth]))
        {
            hTempRapCh->pPath[uiPth]->hDsp = hRap->hDsp[psRsrcGrnt->uiDspId];
        }
    }
    
    hDsp = hRap->hDsp[psRsrcGrnt->uiDspId];
        
    for (i = 0; i < BRAP_FWIF_P_MAX_FW_TASK_PER_DSPCHN; i++)
    {
	    /* Create Task. Note that task starts running/executing */
	    /* Find free task handle */
	    for(j=0;j<BRAP_RM_P_MAX_FW_TASK_PER_DSP;j++)
	    {
	        if(BRAP_FWIF_P_INVALID_TSK_ID == hRap->hFwTask[hDsp->uiDspIndex][j]->uiTaskId)
	            break;
	    }
	    if(BRAP_RM_P_MAX_FW_TASK_PER_DSP == j)
	    {
	        BDBG_ERR(("Unable to find free Task Handle!"));		
	        err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                goto error;            
	    }
	    else
	    {
                hTask = hRap->hFwTask[hDsp->uiDspIndex][j];
                hTask->hDsp = hDsp;        
                hTask->uiTaskId = j + BRAP_FWIF_P_TASK_ID_START_OFFSET;
                if(true == hDspCh->bChSpecificDspCh)
                {
                    hTask->bChSpecificTask =  true;
                    hTask->uHandle.hRapCh = hTempRapCh;
                }
                else
                {
                    hTask->bChSpecificTask =  false;
                    hTask->uHandle.hAssociation = hDspCh->uHandle.hAssociation;
                }
                hTask->bStopped = true;
                hTask->uiLastEventType= 0xFFFF;                 
                hTask->uiMasterTaskId= BRAP_FWIF_P_INVALID_TSK_ID;     
                hTask->bMaster= false;                       
                hDspCh->sFwTaskInfo[i].hFwTask = hTask;
                hDspCh->sFwTaskInfo[i].sProcessingNw = *psAudProcNetwork;
	    }
      }

    error:

    BDBG_LEAVE(BRAP_DSPCHN_P_CreateFwTask);
    return err;

}

void BRAP_DSPCHN_P_GetDestinationFsRate(BRAP_ChannelHandle hRapCh,
    BRAP_P_DstDetails  *pDstDetails,
    const BRAP_DSPCHN_P_AudioParams *psAudioParams,
    BRAP_AF_P_FmmDstFsRate  *eDstFsRate)
{
    unsigned j=0,k=0;
    BRAP_ProcessingStageHandle  hAudProcessing;
    BRAP_ProcessingType     eAudProc;
    bool    bBtscPresent=false;

    for(j = 0 ; j < BRAP_P_MAX_RAPCH_PER_DST ; j++) 
        /* Till the BRAP_P_MAX_RAPCH_PER_DST entry of psProcessingSettings, because next entry i.e. last entry is for Association specific network*/
    {
        if((NULL == pDstDetails->psProcessingSettings[j]) 
            ||(hRapCh != pDstDetails->psProcessingSettings[j]->hRapCh))
        {
            continue;
        }
        for(k =0; k < BRAP_MAX_PP_PER_BRANCH_SUPPORTED; k++)
        {
            hAudProcessing = pDstDetails->psProcessingSettings[j]->sExternalSettings.hAudProcessing[k];
            if(NULL == hAudProcessing)
                break;
            eAudProc = hAudProcessing->sProcessingStageSettings.eAudioProcessing;
            if(BRAP_ProcessingType_eBtsc== eAudProc)
            {
                bBtscPresent=true;
                break;
            }
            
        }
        if(true == bBtscPresent)
            break;
    }

    /*BTSC Encoder Buffer*/
    if(bBtscPresent)
    {
        *eDstFsRate = BRAP_AF_P_FmmDstFsRate_e4xBaseRate;
    }
    else if((psAudioParams->sExtAudioParams.eDecodeMode== BRAP_DSPCHN_DecodeMode_ePassThru)
                &&(psAudioParams->sExtAudioParams.eType == BRAP_DSPCHN_AudioType_eAc3Plus))
    {    
        /*DDP Passthru Buffer*/
        *eDstFsRate   = BRAP_AF_P_FmmDstFsRate_eHBRRate;
    }
#if 0    /*Enable this when this usage mode comes*/
    /*DD Convert with MS11*/
    else if()
    {

    }
#endif    
    /*Normal PCM Buffers*/
    else
    {
        *eDstFsRate = BRAP_AF_P_FmmDstFsRate_eBaseRate;
    }    
    return;
}

/***************************************************************************
Summary: Creates DSP Out Config in NetWorkInfo

Description:
	This function is responsible for creating the whole DSP Output Config.
	
Returns:
	BERR_SUCCESS - If successful

See Also:
	BRAP_DSPCHN_P_Start
**************************************************************************/
static BERR_Code
BRAP_DSPCHN_P_PopulateRBufIds(
	BRAP_ChannelHandle  hRapCh,
	BRAP_P_DspOutputBufferConfig	*psDspOutConfig,	
    const BRAP_DSPCHN_P_AudioParams *psAudioParams,
    BRAP_P_DstDetails  *pDstDetails
	)
{
    BERR_Code   ret = BERR_SUCCESS;

    unsigned int    i=0, j=0, k=0;
    unsigned int    *pui32NumBuffers;
    unsigned int    rBufIndex[BRAP_AF_P_MAX_CHANNELS];
    unsigned int    uiAdaptRateCtrlId[BRAP_AF_P_MAX_CHANNEL_PAIR];  
    bool bFoundRbufs = false,bCompress =false;
#ifdef RAP_MULTISTREAM_DECODER_SUPPORT
    bool            bFWOutputInDRAM = false;
#endif

    BDBG_ENTER(BRAP_DSPCHN_P_PopulateRBufIds);
    BDBG_ASSERT(hRapCh);
    BDBG_ASSERT(psDspOutConfig);
    BDBG_ASSERT(psAudioParams);    
    
    for(i = 0; i < BRAP_AF_P_MAX_CHANNELS;i++)
        rBufIndex[i]=BRAP_INVALID_VALUE;
    
    for(i = 0; i < BRAP_AF_P_MAX_CHANNEL_PAIR;i++)
        uiAdaptRateCtrlId[i]=BRAP_INVALID_VALUE;
    
    pui32NumBuffers = &(psDspOutConfig->sOutputBufConfig.uIoBuffer.sIoBuffId.ui32NumBuffers);
    
    if(BRAP_P_IsPointerValid((void *)pDstDetails))    
    {
            if((BRAP_DSPCHN_DecodeMode_eDecode == psAudioParams->sExtAudioParams.eDecodeMode))
            {
                if(( (pDstDetails->sExtDstDetails.eAudioDst == BRAP_AudioDst_eOutputPort) && 
                (pDstDetails->sExtDstDetails.uDstDetails.sOpDetails.eAudioMode == BRAP_OutputMode_e2_0)) ||
                ((pDstDetails->sExtDstDetails.eAudioDst == BRAP_AudioDst_eRingBuffer) && 
                    (pDstDetails->sExtDstDetails.uDstDetails.sRBufDetails.eAudioMode == BRAP_OutputMode_e2_0)))
                {
                *pui32NumBuffers = 2 ;
                }
                else
                {
                    switch(hRapCh->eChannelOutputMode)
                    {
                        case BRAP_OutputMode_e2_0:
                    *pui32NumBuffers = 2 ;
                            break;
                        case BRAP_OutputMode_e3_2:                        
                    *pui32NumBuffers = 6;
                            break;
                        default:
                            break;
                    }
                }
            }
            else if(BRAP_DSPCHN_DecodeMode_ePassThru == psAudioParams->sExtAudioParams.eDecodeMode)
            {
            *pui32NumBuffers = 1 ;
            }
    }

    for(i = 0; i < BRAP_P_MAX_PATHS_IN_A_CHAN; i++)
    {
        if(!(BRAP_P_IsPointerValid((void *)hRapCh->pPath[i])))
                    break;
                
#if (BRAP_3548_FAMILY == 1)
        if (BRAP_P_UsgPath_eCapture == hRapCh->pPath[i]->eUsgPath)
                {
                    continue;
                }
#endif
        for(j = 0; j < BRAP_P_MAX_DST_PER_RAPCH; j++)
                {
            if(!(BRAP_P_IsPointerValid((void *)hRapCh->pPath[i]->pDstDetails[j])))
            {
                /* Association network, DecodePcm/PPBranch paths don't have destination details,
                   have to populate the capture buffer details */
                if((pDstDetails == NULL) &&
                   ((BRAP_P_UsgPath_eDecodePcm == hRapCh->pPath[i]->eUsgPath) ||
                    (BRAP_P_UsgPath_ePPBranch == hRapCh->pPath[i]->eUsgPath)))
                {      
#ifdef RAP_MULTISTREAM_DECODER_SUPPORT
                    /* For MS10, MS11 the decode channels output to sepearate task(FW Mixer) through DRAM and not FMM */
                    if((BRAP_P_IsFwMixingPostLoopbackEnabled(hRapCh)) &&
                       (BRAP_ChannelType_eDecode == hRapCh->eChannelType) &&
                       (BRAP_INVALID_VALUE != hRapCh->pPath[i]->sOutPathProp[0].uiPathIdx) &&
                       (BRAP_P_IsPointerValid((void *)hRapCh->pPath[hRapCh->pPath[i]->sOutPathProp[0].uiPathIdx])) &&
                       (BRAP_P_UsgPath_eDecodePcmPostMixing == hRapCh->pPath[hRapCh->pPath[i]->sOutPathProp[0].uiPathIdx]->eUsgPath) &&
                       (BRAP_P_IsPointerValid((void *)hRapCh->pPath[hRapCh->pPath[i]->sOutPathProp[0].uiPathIdx]->pDstDetails[0])))
                    {
                        *pui32NumBuffers = hRapCh->hMultiStreamDecoder->uiNumValidIOBuffer[hRapCh->uiFWMixerIpIndex];
                        bFWOutputInDRAM = true;
                        bFoundRbufs = true;
                        break;               
                    }
#endif                
                    for(k=0; k<BRAP_RM_P_MAX_OP_CHANNELS; k++)
                    {
                        rBufIndex[k] = hRapCh->pPath[i]->sRsrcGrnt.uiRbufId[k];
                    }
                            
                    for(k=0; k<BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; k++)
                    {
                        if(hRapCh->pPath[i]->sRsrcGrnt.uiAdaptRateCtrlId[k] != BRAP_INVALID_VALUE)
                        {                        
                            uiAdaptRateCtrlId[k] = hRapCh->pPath[i]->sRsrcGrnt.uiAdaptRateCtrlId[k];
                        }
                    }  
                    bFoundRbufs = true;                    
                }
                else
                {
                    break;
                }
            }
            else
            {
                if(!(BRAP_P_IsPointerValid((void *)pDstDetails)))
                {
                    BDBG_ERR(("Destination details present in the path %d but not in the input param"));
                    ret = BERR_INVALID_PARAMETER;
                    goto exit;
                }
                if(hRapCh->pPath[i]->pDstDetails[j]->eAudioDst == pDstDetails->sExtDstDetails.eAudioDst)
                {      
                    if(BRAP_AudioDst_eOutputPort == hRapCh->pPath[i]->pDstDetails[j]->eAudioDst)
                    {
                        BDBG_MSG(("i > %d, j > %d, Dst1 > %d, Dst2 > %d", i, j,
                            hRapCh->pPath[i]->pDstDetails[j]->uDstDetails.sOpDetails.eOutput[0],
                                pDstDetails->sExtDstDetails.uDstDetails.sOpDetails.eOutput[0]));
                        if(hRapCh->pPath[i]->pDstDetails[j]->uDstDetails.sOpDetails.eOutput[0] ==
                                pDstDetails->sExtDstDetails.uDstDetails.sOpDetails.eOutput[0])
                            {
                                bCompress =false;
                            BRAP_P_IsPathOutputCompress(hRapCh,i,&bCompress);
                                if(bCompress == true)    
                                {
                                *pui32NumBuffers = 1 ;        
                                }
                            
                            bFoundRbufs = true;
                            for(k=0; k<(*pui32NumBuffers); k++)
                            {
                                rBufIndex[k] = hRapCh->pPath[i]->sRsrcGrnt.uiRbufId[k];
                            }
                                    
                            for(k=0; k<(*pui32NumBuffers)/2; k++)
                            {
                                if(hRapCh->pPath[i]->sRsrcGrnt.uiAdaptRateCtrlId[k] != BRAP_INVALID_VALUE)
                                {                        
                                    uiAdaptRateCtrlId[k] = hRapCh->pPath[i]->sRsrcGrnt.uiAdaptRateCtrlId[k];
                                }
                            }
                        }
                    }
                    else if(BRAP_AudioDst_eRingBuffer == hRapCh->pPath[i]->pDstDetails[j]->eAudioDst)
                    {
                        BDBG_MSG(("i > %d, j > %d, Dst1 > %d, Dst2 > %d", i, j,
                            hRapCh->pPath[i]->pDstDetails[j]->uDstDetails.sRBufDetails.uiRBufId[0],
                                pDstDetails->sExtDstDetails.uDstDetails.sRBufDetails.uiRBufId[0]));
                        if(hRapCh->pPath[i]->pDstDetails[j]->uDstDetails.sRBufDetails.uiRBufId[0] ==
                                pDstDetails->sExtDstDetails.uDstDetails.sRBufDetails.uiRBufId[0])
                            {
                                bCompress =false;
                            BRAP_P_IsPathOutputCompress(hRapCh,i,&bCompress);
                                if(bCompress == true)    
                                {
                                *pui32NumBuffers = 1 ;        
                                }                            
                            
                            bFoundRbufs = true;
                            for(k=0; k<(*pui32NumBuffers); k++)
                            {
                                rBufIndex[k] = hRapCh->pPath[i]->sRsrcGrnt.uiRbufId[k];
                            }
                            for(k=0; k<(*pui32NumBuffers)/2; k++)
                            {
                                if(hRapCh->pPath[i]->sRsrcGrnt.uiAdaptRateCtrlId[k] != BRAP_INVALID_VALUE)
                                {                                         
                                    uiAdaptRateCtrlId[k] = hRapCh->pPath[i]->sRsrcGrnt.uiAdaptRateCtrlId[k];                       
                                }
                            }
                        }
                    }                    
                }
            } /* if(!(BRAP_P_IsPointerValid((void *)hRapCh->pPath[i]->pDstDetails[j]) )) */
            if(true == bFoundRbufs)
                break;
        }
        if(true == bFoundRbufs)
            break;
    }

#ifdef RAP_MULTISTREAM_DECODER_SUPPORT
    /* For MS10, MS11 the decode channels output to sepearate task(FW Mixer) through DRAM and not FMM */
    if(true == bFWOutputInDRAM)
    {
        uint32_t    ui32Offset;
        
        if(hRapCh->hMultiStreamDecoder != NULL)
        {
            for(i=0; i<BRAP_P_MAX_FW_STG_INPUTS; i++)
            {
                if(hRapCh->hMultiStreamDecoder->bDRAMBuffersUsed[i] == false)
                    break;
            }
            if(i == BRAP_P_MAX_FW_STG_INPUTS)
            {
                BDBG_ERR(("No free DRAM buffer found for InterTask buffer usage"));
                ret = BRAP_ERR_BAD_DEVICE_STATE;
                goto exit;
            }

            psDspOutConfig->sOutputBufConfig.uIoBuffer.sIoBuffer.eBufferType = BRAP_AF_P_BufferType_eDRAM;
            psDspOutConfig->sOutputBufConfig.uIoBuffer.sIoBuffer.ui32NumBuffers= hRapCh->hMultiStreamDecoder->uiNumValidIOBuffer[i];
            for(j=0 ; j<psDspOutConfig->sOutputBufConfig.uIoBuffer.sIoBuffer.ui32NumBuffers ; j++)
            {	
                BRAP_P_ConvertAddressToOffset(	hRapCh->hHeap,
    						(void *)(hRapCh->hMultiStreamDecoder->ui32InterTaskIoBufferAddress[i][j]), &ui32Offset);
    			psDspOutConfig->sOutputBufConfig.uIoBuffer.sIoBuffer.sCircBuffer[j].ui32BaseAddr = ui32Offset;
                psDspOutConfig->sOutputBufConfig.uIoBuffer.sIoBuffer.sCircBuffer[j].ui32ReadAddr = ui32Offset;
                psDspOutConfig->sOutputBufConfig.uIoBuffer.sIoBuffer.sCircBuffer[j].ui32WriteAddr = ui32Offset;		

                BRAP_P_ConvertAddressToOffset(	hRapCh->hHeap, (void *)(hRapCh->hMultiStreamDecoder-> \
    						ui32InterTaskIoBufferAddress[i][j] + BRAP_AF_P_INTERTASK_IOBUFFER_SIZE), &ui32Offset);
    			psDspOutConfig->sOutputBufConfig.uIoBuffer.sIoBuffer.sCircBuffer[j].ui32EndAddr = ui32Offset;
                psDspOutConfig->sOutputBufConfig.uIoBuffer.sIoBuffer.sCircBuffer[j].ui32WrapAddr = ui32Offset;			
                
            }
            psDspOutConfig->sOutputBufConfig.uIoGenericBuffer.sIoGenericBuffer.eBufferType = BRAP_AF_P_BufferType_eDRAM;
            psDspOutConfig->sOutputBufConfig.uIoGenericBuffer.sIoGenericBuffer.ui32NumBuffers = 1;


            BRAP_P_ConvertAddressToOffset(  hRapCh->hHeap, (void *)(hRapCh->hMultiStreamDecoder-> \
                            ui32InterTaskIoGenericBufferAddress[i]), &ui32Offset);
            psDspOutConfig->sOutputBufConfig.uIoGenericBuffer.sIoGenericBuffer.sCircBuffer.ui32BaseAddr = ui32Offset;         
            psDspOutConfig->sOutputBufConfig.uIoGenericBuffer.sIoGenericBuffer.sCircBuffer.ui32ReadAddr = ui32Offset;
            psDspOutConfig->sOutputBufConfig.uIoGenericBuffer.sIoGenericBuffer.sCircBuffer.ui32WriteAddr = ui32Offset;       

            BRAP_P_ConvertAddressToOffset(  hRapCh->hHeap, (void *)(hRapCh->hMultiStreamDecoder-> \
                ui32InterTaskIoGenericBufferAddress[i] +BRAP_AF_P_INTERTASK_IOGENBUFFER_SIZE), &ui32Offset);
            psDspOutConfig->sOutputBufConfig.uIoGenericBuffer.sIoGenericBuffer.sCircBuffer.ui32EndAddr = ui32Offset;         
            psDspOutConfig->sOutputBufConfig.uIoGenericBuffer.sIoGenericBuffer.sCircBuffer.ui32WrapAddr = ui32Offset;
        }
    } 
    else
#endif        
    {
        psDspOutConfig->sOutputBufConfig.uIoBuffer.sIoBuffId.eBufferType = BRAP_AF_P_BufferType_eFMM;    
            for(j = 0; j < BRAP_AF_P_MAX_CHANNELS ; j++)
            {
                /* TODO: Note: the size of array uiBufferId and inputRBufIndex should be same */
            BDBG_MSG(("sDspOutConfig[%d].sOutputBufConfig.uIoBuffer.sIoBuffId.ui32BufferId[%d] = %d",k,j,rBufIndex[j]));
            psDspOutConfig->sOutputBufConfig.uIoBuffer.sIoBuffId.ui32BufferId[j] = rBufIndex[j];		
            }

            for(j = 0; j <  BRAP_AF_P_MAX_CHANNEL_PAIR; j++)
            {
                /* TODO: Note: the size of array uiBufferId and inputRBufIndex should be same */
                BDBG_MSG(("sDspOutConfig[%d].sOutputBufConfig.uIoBuffer.sIoBuffId.ui32AdaptRateCtrlIds[%d] = %d",k,j,uiAdaptRateCtrlId[j]));
            psDspOutConfig->sOutputBufConfig.uIoBuffer.sIoBuffId.ui32AdaptRateCtrlIds[j] = uiAdaptRateCtrlId[j];		
            }
    }

    if(BRAP_P_IsPointerValid((void *)pDstDetails))
    {
#if (BRAP_INDEP_OP_DELAY_SUPPORTED == 1)                    
            if(BRAP_AudioDst_eOutputPort == pDstDetails->sExtDstDetails.eAudioDst)
            {    
                if(true == hRapCh->bIndepDelayEnabled)
                {
                    if(((pDstDetails->sExtDstDetails.uDstDetails.sOpDetails.eOutput[0] == BRAP_OutputPort_eI2s0)
                        ||(pDstDetails->sExtDstDetails.uDstDetails.sOpDetails.eOutput[0] == BRAP_OutputPort_eMaiMulti0))
                        &&(hRapCh->hRap->sOutputSettings[BRAP_OutputPort_eMai].uOutputPortSettings.sMaiSettings.eMaiMuxSelector == pDstDetails->sExtDstDetails.uDstDetails.sOpDetails.eOutput[0]))
                    {
                psDspOutConfig->sOutputBufConfig.ui32IndepDelay = 
                            hRapCh->hRap->sOutputSettings[BRAP_OutputPort_eMai].iDelay;
                    }
                    else 
                    {
                /* To call CIT API for the delay value to offset to below IndepDelay */
                psDspOutConfig->sOutputBufConfig.ui32IndepDelay = 
                        hRapCh->hRap->sOutputSettings[pDstDetails->sExtDstDetails.uDstDetails.sOpDetails.eOutput[0]].iDelay;
                    }                   
                    BDBG_MSG(("Op > %d", pDstDetails->sExtDstDetails.uDstDetails.sOpDetails.eOutput[0]));
            BDBG_MSG(("Delay > %x", psDspOutConfig->sOutputBufConfig.ui32IndepDelay));
                }
            }
            else
            {
                if(true == hRapCh->bIndepDelayEnabled)
                {
#ifdef  RAP_AUTOMATED_UNIT_TEST                
            psDspOutConfig->sOutputBufConfig.ui32IndepDelay = pDstDetails->sExtDstDetails.uDstDetails.sRBufDetails.uiDelay;
#else
            psDspOutConfig->sOutputBufConfig.ui32IndepDelay = 0;
#endif
                }
            }
#endif                            
        BRAP_DSPCHN_P_GetDestinationFsRate(hRapCh,pDstDetails,psAudioParams,&psDspOutConfig->sOutputBufConfig.eFmmDstFsRate);
    }
    else
    {
        /* 0 delay for ouput of DecodePCM/PPBranch going into capture buffers */
        psDspOutConfig->sOutputBufConfig.ui32IndepDelay = 0;
    }

exit:
    BDBG_LEAVE(BRAP_DSPCHN_P_PopulateRBufIds);
    return ret;
}

/***************************************************************************
Summary: Creates DSP Out Config in NetWorkInfo

Description:
	This function is responsible for creating the whole DSP Output Config.
	
Returns:
	BERR_SUCCESS - If successful

See Also:
	BRAP_DSPCHN_P_Start
**************************************************************************/
BERR_Code
BRAP_DSPCHN_P_CreateDstConfig(
	BRAP_DSPCHN_Handle	hDspCh,					/* [in] DSPCHN handle */
	const BRAP_DSPCHN_P_AudioParams *psAudioParams	/* [in] Audio decode parameters */
	)
{
    BERR_Code   ret = BERR_SUCCESS;
    
    unsigned int i = 0, j = 0, k = 0;
    BRAP_P_DstDetails  *pPvtDstDetails = NULL;
    BRAP_ChannelHandle hRapCh = NULL;

    BDBG_ENTER(BRAP_DSPCHN_P_CreateDstConfig);
    BDBG_ASSERT(hDspCh);
    BDBG_ASSERT(psAudioParams);
                
	if(true == hDspCh->bChSpecificDspCh)
	{
        hRapCh = hDspCh->uHandle.hRapCh;    	
        
        for(i=0, k = 0;i < BRAP_MAX_ASSOCIATED_GROUPS;i++)
        {
            for(j = 0  ; j < BRAP_P_MAX_DST_PER_RAPCH ; j++)
            {
                if(hRapCh->uiAssociationId[i] == BRAP_INVALID_VALUE)
                    continue;
            
        		pPvtDstDetails = &(hDspCh->uHandle.hRapCh->hRap->sAssociatedCh[hDspCh->uHandle.hRapCh->uiAssociationId[i]].sDstDetails[j]);
                if(BRAP_AudioDst_eMax != pPvtDstDetails->sExtDstDetails.eAudioDst)
                {        
                    if(BRAP_P_IsAssociationNWValid(pPvtDstDetails))
                    {    
                        /* Association network, DecodePcm/PPBranch(bChSpecificDspCh is true) paths don't have destination details */
                        ret =BRAP_DSPCHN_P_PopulateRBufIds(hRapCh, 
                                &(hDspCh->sNetworkInfo.sDspOutConfig[k]), psAudioParams, NULL);
                        if(ret != BERR_SUCCESS)
                        {
                                    BDBG_ERR(("BRAP_DSPCHN_P_PopulateRBufIds returned error"));
                            goto end;
                        }
                        hDspCh->sNetworkInfo.sDspOutConfig[k].hDestHandle = NULL;
                    }
                    else
                    {        
                        ret =BRAP_DSPCHN_P_PopulateRBufIds(hRapCh, 
                                &(hDspCh->sNetworkInfo.sDspOutConfig[k]), psAudioParams, pPvtDstDetails);
                        if(ret != BERR_SUCCESS)
                        {
                            BDBG_ERR(("BRAP_DSPCHN_P_PopulateRBufIds returned error"));
                            goto end;
                        }   
                    hDspCh->sNetworkInfo.sDspOutConfig[k].hDestHandle = pPvtDstDetails;
                    }                    
                    k++;                    
                }
            }
        }
	}
	else
	{
   	    BDBG_MSG(("Association specific hDspCh."));	
        for(i=0,k=0; i < BRAP_P_MAX_DST_PER_RAPCH ; i++)
        {        	
    		pPvtDstDetails = &(hDspCh->uHandle.hAssociation->sDstDetails[i]);

            if(BRAP_P_IsAssociationNWValid(pPvtDstDetails))
            {            
                if(BRAP_AudioDst_eMax !=pPvtDstDetails->sExtDstDetails.eAudioDst)
                {
                    bool            bFound = false;
                    unsigned int    l = 0;

                    for(j = 0; j < BRAP_MAX_PRI_DEC_CHAN_IN_ASSOCIATED_GRP; j++)
                    {
                        if(BRAP_P_IsPointerValid((void *)hDspCh->uHandle.hAssociation->hPriDecCh[j]))
                        {
                            hRapCh = hDspCh->uHandle.hAssociation->hPriDecCh[j];
                            for(l=0; l < BRAP_P_MAX_PATHS_IN_A_CHAN; l++)
                            {
                                if((BRAP_P_IsPointerValid((void *)hRapCh->pPath[l])) &&
                                   (hRapCh->pPath[l]->hDspCh == hDspCh))
                                {
                                    bFound = true;
                                    break;
                                }

                            }
                        }
                        if(bFound == true)
                            break;
                    }
                    if(bFound == false)
                    {
                        for(j = 0; j < BRAP_MAX_SEC_DEC_CHAN_IN_ASSOCIATED_GRP; j++)
                        {
                            if(BRAP_P_IsPointerValid((void *)hDspCh->uHandle.hAssociation->hSecDecCh[j]))
                            {
                                hRapCh = hDspCh->uHandle.hAssociation->hSecDecCh[j];
                                for(l=0; l < BRAP_P_MAX_PATHS_IN_A_CHAN; l++)
                                {
                                    if((BRAP_P_IsPointerValid((void *)hRapCh->pPath[l])) &&
                                       (hRapCh->pPath[l]->hDspCh == hDspCh))
                                    {
                                        bFound = true;
                                        break;
                                    }
                                }
                            }
                            if(bFound == true)
                                break;
                        }
                    }
                    if(bFound == false)
                    {                    
                        for(j = 0; j < BRAP_MAX_CAP_CHAN_IN_ASSOCIATED_GRP; j++)
                        {
                            if(BRAP_P_IsPointerValid((void *)hDspCh->uHandle.hAssociation->hCapCh[j]))
                            {
                                hRapCh = hDspCh->uHandle.hAssociation->hCapCh[j];
                                for(l=0; l < BRAP_P_MAX_PATHS_IN_A_CHAN; l++)
                                {
                                    if((BRAP_P_IsPointerValid((void *)hRapCh->pPath[l])) &&
                                       (hRapCh->pPath[l]->hDspCh == hDspCh))
                                    {
                                        bFound = true;
                                        break;
                                    }
                                }
                            }
                            if(bFound == true)
                                break;
                        }
                    }
                    if(bFound == false)
                    {                    
                        for(j = 0; j < BRAP_MAX_PB_CHAN_IN_ASSOCIATED_GRP; j++)
                        {
                            if(BRAP_P_IsPointerValid((void *)hDspCh->uHandle.hAssociation->hPBCh[j]))
                            {
                                hRapCh = hDspCh->uHandle.hAssociation->hPBCh[j];
                                for(l=0; l < BRAP_P_MAX_PATHS_IN_A_CHAN; l++)
                                {
                                    if((BRAP_P_IsPointerValid((void *)hRapCh->pPath[l])) &&
                                       (hRapCh->pPath[l]->hDspCh == hDspCh))
                                    {
                                        bFound = true;
                                        break;
                                    }
                                }
                            }
                            if(bFound == true)
                                break;
                        }
                    }
                    if(bFound == false)
                    {
                        BDBG_ERR(("Destination not found in any of the Raptor channels"));
                        goto end;                    
                    }
                    
                    ret =BRAP_DSPCHN_P_PopulateRBufIds(hRapCh, &(hDspCh->sNetworkInfo.sDspOutConfig[k]),
                                                       psAudioParams, pPvtDstDetails);
                    if(ret != BERR_SUCCESS)
                    {
                        BDBG_ERR(("BRAP_DSPCHN_P_PopulateRBufIds returned error"));
                        goto end;
                    }
                    hDspCh->sNetworkInfo.sDspOutConfig[k].hDestHandle = pPvtDstDetails;
                    k++;
                }            
            }
        }
	}
    
 end:
    BDBG_LEAVE(BRAP_DSPCHN_P_CreateDstConfig);    
    return ret;
}

/***************************************************************************
Summary: Starts a DSP context

Description:
	This function is responsible for starting a DSP context for the given
	input decode mode.
	
Returns:
	BERR_SUCCESS - If successful

See Also:
	BRAP_DSPCHN_P_Stop
**************************************************************************/
BERR_Code
BRAP_DSPCHN_P_Start(
	BRAP_DSPCHN_Handle	hDspCh,					/* [in] DSPCHN handle */
	const BRAP_DSPCHN_P_AudioParams *psAudioParams	/* [in] Audio decode parameters */
	)
{
    BRAP_P_AudioProcNetwork     *psAudProcNetwork = NULL;
    BRAP_CIT_P_InputInfo        *psCitInputStruct = NULL;
    unsigned int i = 0, j = 0, k = 0, l = 0;
    unsigned int uiBranchId = 0, uiStageId = 0, uiConfigBufAddr, uiConfigBufSize, uiStartNodeIndex;
    BRAP_CIT_P_OpStgInfo *psOpStgInfo = NULL;
    BERR_Code err = BERR_SUCCESS;
    BRAP_FWIF_MsgQueueHandle hMsgQueue;
    uint32_t ui32SyncFifo = BRAP_DSP_P_FIFO_INVALID;
    uint32_t ui32AsyncFifo = BRAP_DSP_P_FIFO_INVALID;
    BRAP_FWIF_P_FwTaskHandle hFwTaskCreate = NULL;
    BRAP_FWIF_P_Command sCommand;
    BRAP_FWIF_P_Response sRsp;
    BRAP_Handle hRap = NULL;
    BRAP_CTB_Output sCtbOutput;
    uint32_t ui32PhysAddr = 0, ui32RbufOffset = 0, ui32StatusBufAdr;
    uint32_t ui32TsmNodeIndex = 0;
    BRAP_FWIF_P_TaskParamInfo *pTaskParams = NULL;
    BRAP_P_MsgType      eMsgType;
    BRAP_DSP_Handle hDsp = NULL;
    uint32_t    ui32CBitBufAddr, ui32CBitBufSize;
    unsigned int uiActualConfigSize;
    bool                        bWdgRecovery = false;
    uint32_t    ui32QueueId;
    uint32_t ui32NumBranchesValid = 0, ui32NumStagesValid = 0;
    BRAP_ProcessingStageHandle  hAudioProcessing;
    bool bPpConfigDone= false;
    BRAP_ProcessingType   eProcType = BRAP_ProcessingType_eMax;
    unsigned int uiMasterStageId=BRAP_INVALID_VALUE, uiMasterBranchId=BRAP_INVALID_VALUE;
    unsigned int    ui32Threshold = 0;
    unsigned int    ui32BlockTime = 0;        
    bool    bAdaptiveRateEnable=false, bFWSchedulingBufFound=false;
    BDBG_ENTER( BRAP_DSPCHN_P_Start );

    if(true == hDspCh->bChSpecificDspCh)
    {
        BDBG_ASSERT(hDspCh->uHandle.hRapCh);
        hRap = hDspCh->uHandle.hRapCh->hRap;
    }
    else
    {
        BDBG_ASSERT(hDspCh->uHandle.hAssociation);
        hRap = hDspCh->uHandle.hAssociation->hRap;
    }

    bWdgRecovery = BRAP_P_GetWatchdogRecoveryFlag(hRap);
    if(false == bWdgRecovery)
    {
	/* Storing psAudioParams in hDspch */
	hDspCh->sDspAudioParams = *psAudioParams;
    }
    else
    {
        psAudioParams = &(hDspCh->sDspAudioParams);
    }

    /* Allocate big structures on heap */
    psAudProcNetwork = (BRAP_P_AudioProcNetwork *) BKNI_Malloc(sizeof(BRAP_P_AudioProcNetwork));
    if (NULL==psAudProcNetwork)
    {
        BDBG_ERR(("Out of System memory"));
        err = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        goto error;
    }
    BKNI_Memset(psAudProcNetwork, 0,(sizeof(BRAP_P_AudioProcNetwork)));

    psCitInputStruct = (BRAP_CIT_P_InputInfo *)BKNI_Malloc(sizeof(BRAP_CIT_P_InputInfo));
    if (NULL==psCitInputStruct)
    {
        BDBG_ERR(("Out of System memory"));
        err = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        goto error;
    }
	BKNI_Memset( psCitInputStruct, 0, sizeof(BRAP_CIT_P_InputInfo) );

    if(true == hDspCh->bChSpecificDspCh)
    {
        if ((BRAP_AudioSource_eExtCapPort== hDspCh->uHandle.hRapCh->eAudioSource) ||
            (BRAP_AudioSource_eRingBuffer== hDspCh->uHandle.hRapCh->eAudioSource))
        {    
            hDspCh->sNetworkInfo.eNetworkInputType = BRAP_CIT_P_FwStgSrcDstType_eFMMBuf;
        }
        else if (BRAP_AudioSource_eXptInterface == hDspCh->uHandle.hRapCh->eAudioSource)
        {
            hDspCh->sNetworkInfo.eNetworkInputType = BRAP_CIT_P_FwStgSrcDstType_eRaveBuf;        
        }
    }
    else
    {
        BRAP_ChannelHandle  hRapCh;
        hRapCh = BRAP_P_GetRapChHandle(hDspCh->uHandle.hAssociation);
        if(!(BRAP_P_IsPointerValid((void *)hRapCh)))
        {
            BDBG_ERR(("No Raptor channel found in Association handle"));
            err = BERR_INVALID_PARAMETER;
            goto error;
        }
#ifdef RAP_MULTISTREAM_DECODER_SUPPORT        
        if(BRAP_P_IsFwMixingPostLoopbackEnabled(hRapCh))
        {
            if(BRAP_DSPCHN_P_SourceType_eFMMBuf == psAudioParams->sDecOrPtParams.eDecSourceType)
            {
                hDspCh->sNetworkInfo.eNetworkInputType = BRAP_CIT_P_FwStgSrcDstType_eFMMBuf;            
            }
            else if(BRAP_DSPCHN_P_SourceType_eDRAMBuf == psAudioParams->sDecOrPtParams.eDecSourceType)
            {
                hDspCh->sNetworkInfo.eNetworkInputType = BRAP_CIT_P_FwStgSrcDstType_eDRAMBuf;
            }
            else
            {
                hDspCh->sNetworkInfo.eNetworkInputType = BRAP_CIT_P_FwStgSrcDstType_eInvalid;                
            }
        }
        else
#endif            
        {
            hDspCh->sNetworkInfo.eNetworkInputType = BRAP_CIT_P_FwStgSrcDstType_eFMMBuf;        
        }
    }

    /* Populate psNetworkInfo struct */
    hDspCh->sNetworkInfo.eAudioType = psAudioParams->sExtAudioParams.eType;
    hDspCh->sNetworkInfo.eDecodeMode = psAudioParams->sExtAudioParams.eDecodeMode;
    hDspCh->sNetworkInfo.sInputBufConfig.uIoBuffer.sIoBuffer.ui32NumBuffers = 2;

    if(hDspCh->sNetworkInfo.eNetworkInputType == BRAP_CIT_P_FwStgSrcDstType_eRaveBuf)
    {
        hDspCh->sNetworkInfo.sInputBufConfig.uIoBuffer.sIoBuffer.eBufferType = BRAP_AF_P_BufferType_eRAVE;	
        hDspCh->sNetworkInfo.sInputBufConfig.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32BaseAddr = psAudioParams->sXptContextMap.CDB_Base + BCHP_PHYSICAL_OFFSET;
        hDspCh->sNetworkInfo.sInputBufConfig.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32EndAddr = psAudioParams->sXptContextMap.CDB_End + BCHP_PHYSICAL_OFFSET;
        hDspCh->sNetworkInfo.sInputBufConfig.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32ReadAddr = psAudioParams->sXptContextMap.CDB_Read + BCHP_PHYSICAL_OFFSET;
        hDspCh->sNetworkInfo.sInputBufConfig.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32WriteAddr = psAudioParams->sXptContextMap.CDB_Valid + BCHP_PHYSICAL_OFFSET;
        hDspCh->sNetworkInfo.sInputBufConfig.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32WrapAddr = psAudioParams->sXptContextMap.CDB_Wrap + BCHP_PHYSICAL_OFFSET;		


        hDspCh->sNetworkInfo.sInputBufConfig.uIoBuffer.sIoBuffer.sCircBuffer[1].ui32BaseAddr = psAudioParams->sXptContextMap.ITB_Base + BCHP_PHYSICAL_OFFSET;
        hDspCh->sNetworkInfo.sInputBufConfig.uIoBuffer.sIoBuffer.sCircBuffer[1].ui32EndAddr = psAudioParams->sXptContextMap.ITB_End + BCHP_PHYSICAL_OFFSET;
        hDspCh->sNetworkInfo.sInputBufConfig.uIoBuffer.sIoBuffer.sCircBuffer[1].ui32ReadAddr = psAudioParams->sXptContextMap.ITB_Read + BCHP_PHYSICAL_OFFSET;
        hDspCh->sNetworkInfo.sInputBufConfig.uIoBuffer.sIoBuffer.sCircBuffer[1].ui32WriteAddr = psAudioParams->sXptContextMap.ITB_Valid + BCHP_PHYSICAL_OFFSET;
        hDspCh->sNetworkInfo.sInputBufConfig.uIoBuffer.sIoBuffer.sCircBuffer[1].ui32WrapAddr = psAudioParams->sXptContextMap.ITB_Wrap + BCHP_PHYSICAL_OFFSET;		
    }
    else if(hDspCh->sNetworkInfo.eNetworkInputType == BRAP_CIT_P_FwStgSrcDstType_eFMMBuf)
    {
        hDspCh->sNetworkInfo.sInputBufConfig.uIoBuffer.sIoBuffId.ui32NumBuffers = 2;	    
        hDspCh->sNetworkInfo.sInputBufConfig.uIoBuffer.sIoBuffId.eBufferType = BRAP_AF_P_BufferType_eFMM;	
        if(true == hDspCh->bChSpecificDspCh)
        {        
        if (BRAP_AudioSource_eExtCapPort== hDspCh->uHandle.hRapCh->eAudioSource)
        {
                if (psAudioParams->sDecOrPtParams.uDecBufFmmParams.sDecPathFmmParams.inputRBufIndex[0] != (int8_t)BRAP_RM_P_INVALID_INDEX)        
            {
                    hDspCh->sNetworkInfo.sInputBufConfig.uIoBuffer.sIoBuffId.ui32BufferId[0] =  psAudioParams->sDecOrPtParams.uDecBufFmmParams.sDecPathFmmParams.inputRBufIndex[0];     
            }
                if (psAudioParams->sDecOrPtParams.uDecBufFmmParams.sDecPathFmmParams.inputRBufIndex[1] != (int8_t)BRAP_RM_P_INVALID_INDEX)        
                {
                    hDspCh->sNetworkInfo.sInputBufConfig.uIoBuffer.sIoBuffId.ui32BufferId[1] =  psAudioParams->sDecOrPtParams.uDecBufFmmParams.sDecPathFmmParams.inputRBufIndex[1];
            }
            /* [TODO] Program CapInputPort */
            
            switch (hDspCh->uHandle.hRapCh->eCapInputPort)
            {
                case BRAP_CapInputPort_eExtI2s0:
                    /* [TODO] Program Sampling frequencty */
    /*                    BRAP_P_ConvertSR(psAudioParams->sExtAudioParams.eInputSampleRate,);*/                    
                    break;                
                case BRAP_CapInputPort_eSpdif:
                    /*  [TODO] Program SPDIF reg Addr */
                    break;                
                case BRAP_CapInputPort_eHdmi:
                    /*  [TODO] Program HDMI reg Addr */                    
                    break;                
                case BRAP_CapInputPort_eRfAudio:
                    /*  [TODO] Program RF Audio reg Addr */
                    break;  
                case BRAP_CapInputPort_eAdc:
                    /*  [TODO] Program ADC Audio reg Addr */
                    break;               
                default:  
                    break;
            }
        }
        else if (BRAP_AudioSource_eRingBuffer== hDspCh->uHandle.hRapCh->eAudioSource)
        {
                if (psAudioParams->sDecOrPtParams.uDecBufFmmParams.sDecPathFmmParams.inputRBufIndex[0] != (int8_t)BRAP_RM_P_INVALID_INDEX)        
            {
                    hDspCh->sNetworkInfo.sInputBufConfig.uIoBuffer.sIoBuffId.ui32BufferId[0] =  psAudioParams->sDecOrPtParams.uDecBufFmmParams.sDecPathFmmParams.inputRBufIndex[0];     
            }
                if (psAudioParams->sDecOrPtParams.uDecBufFmmParams.sDecPathFmmParams.inputRBufIndex[1] != (int8_t)BRAP_RM_P_INVALID_INDEX)        
                {
                    hDspCh->sNetworkInfo.sInputBufConfig.uIoBuffer.sIoBuffId.ui32BufferId[1] =  psAudioParams->sDecOrPtParams.uDecBufFmmParams.sDecPathFmmParams.inputRBufIndex[1];
                }
            }
        }
        else
        {
            if (psAudioParams->sDecOrPtParams.uDecBufFmmParams.sDecPathFmmParams.inputRBufIndex[0] != (int8_t)BRAP_RM_P_INVALID_INDEX)        
            {
                hDspCh->sNetworkInfo.sInputBufConfig.uIoBuffer.sIoBuffId.ui32BufferId[0] =  psAudioParams->sDecOrPtParams.uDecBufFmmParams.sDecPathFmmParams.inputRBufIndex[0];     
            }
            if (psAudioParams->sDecOrPtParams.uDecBufFmmParams.sDecPathFmmParams.inputRBufIndex[1] != (int8_t)BRAP_RM_P_INVALID_INDEX)        
            {
                hDspCh->sNetworkInfo.sInputBufConfig.uIoBuffer.sIoBuffId.ui32BufferId[1] =  psAudioParams->sDecOrPtParams.uDecBufFmmParams.sDecPathFmmParams.inputRBufIndex[1];
            }
        }
    }
    else if(hDspCh->sNetworkInfo.eNetworkInputType == BRAP_CIT_P_FwStgSrcDstType_eDRAMBuf)
    {
        if((psAudioParams->sDecOrPtParams.uDecBufFmmParams.sDecSourceBufDetails.sDecIoBuffer.ui32NumBuffers != 0) &&
           (psAudioParams->sDecOrPtParams.uDecBufFmmParams.sDecSourceBufDetails.sDecIoBuffer.eBufferType == BRAP_AF_P_BufferType_eDRAM) &&
           (psAudioParams->sDecOrPtParams.uDecBufFmmParams.sDecSourceBufDetails.sDecIoBuffer.sCircBuffer[0].ui32BaseAddr != BRAP_INVALID_VALUE))
        {
            hDspCh->sNetworkInfo.sInputBufConfig.uIoBuffer.sIoBuffer = 
                psAudioParams->sDecOrPtParams.uDecBufFmmParams.sDecSourceBufDetails.sDecIoBuffer;
        }
        if((psAudioParams->sDecOrPtParams.uDecBufFmmParams.sDecSourceBufDetails.sDecIoGenericBuffer.ui32NumBuffers != 0) &&
           (psAudioParams->sDecOrPtParams.uDecBufFmmParams.sDecSourceBufDetails.sDecIoGenericBuffer.eBufferType == BRAP_AF_P_BufferType_eDRAM) &&
           (psAudioParams->sDecOrPtParams.uDecBufFmmParams.sDecSourceBufDetails.sDecIoGenericBuffer.sCircBuffer.ui32BaseAddr != BRAP_INVALID_VALUE))
        {
            hDspCh->sNetworkInfo.sInputBufConfig.uIoGenericBuffer.sIoGenericBuffer = 
                psAudioParams->sDecOrPtParams.uDecBufFmmParams.sDecSourceBufDetails.sDecIoGenericBuffer;
        }        
    }
	
    if(true == hDspCh->bChSpecificDspCh)
    {
    	if(BRAP_P_IsPointerValid((void *)hDspCh->uHandle.hRapCh))
    	{
            for(j=0;j<BRAP_P_MAX_DST_PER_RAPCH;j++)
            {
                for(k=0;k<BRAP_MAX_STAGE_PER_BRANCH_SUPPORTED;k++)
            	{
                    psAudProcNetwork->sAudProcessingStage[j][k]= 
                                    hDspCh->uHandle.hRapCh->sChAudProcessingStage[j][k];                    
            	}
            }
        }
    }
    else
    {
    	if(BRAP_P_IsPointerValid((void *)hDspCh->uHandle.hAssociation))
    	{    
        for(j=0;j<BRAP_P_MAX_DST_PER_RAPCH;j++)
        {
            for(k=0;k<BRAP_MAX_PP_PER_BRANCH_SUPPORTED;k++)
            	{
                    psAudProcNetwork->sAudProcessingStage[j][k]= 
                                    hDspCh->uHandle.hAssociation->sAudProcessingStage[j][k];
            	}
            }
        }
    }
    
    if(false == bWdgRecovery)
    {
        /* TODO: Split the network into multiple small networks */
        BRAP_DSPCHN_P_CreateFwTask(hDspCh,psAudProcNetwork);
    }
    
    if(false == hRap->sSettings.bOpenTimeFwDownload)
    {
        /* Download the firmware binaries required by the complete network */        

        {
            err = BRAP_DSPCHN_P_DownloadStartTimeFWExec(hDspCh, &(hDspCh->sDspAudioParams));
            if( BERR_SUCCESS!=err )
            {
                goto error;
            }
        }
    }

    for (i = 0; i < BRAP_FWIF_P_MAX_FW_TASK_PER_DSPCHN; i++)
    {
        bFWSchedulingBufFound = false;
        hFwTaskCreate = hDspCh->sFwTaskInfo[i].hFwTask;
        if(!(BRAP_P_IsPointerValid((void *)hFwTaskCreate)))
        {
            BDBG_MSG(("Task handle is NULL for Task no. %d",i+BRAP_FWIF_P_TASK_ID_START_OFFSET));
            continue;
        }
        hDsp = hFwTaskCreate->hDsp;

        BDBG_MSG(("hFwTaskCreate->uiTaskId = %d , hDsp->uiDspIndex =%d",hFwTaskCreate->uiTaskId,hDsp->uiDspIndex));

        /* Convert processing network into CIT module input */
        err = BRAP_FWIF_P_GenerateCitInput(hRap,hDspCh, &(hDspCh->sFwTaskInfo[i].sProcessingNw), &(hDspCh->sNetworkInfo), psCitInputStruct,hFwTaskCreate);
        if( BERR_SUCCESS!=err )
        {
            goto error;
        }

#ifdef BRAP_CIT_DEBUG_PRINT
    	BRAP_P_ShowCitSettings(&(hDspCh->sFwTaskInfo[i].sProcessingNw), psCitInputStruct);
#endif            

        /* Call CIT module */
        BDBG_MSG(("sCitOutput = %#x",&(hFwTaskCreate->sCitOutput)));
        BDBG_MSG(("Calling CIT Generation Module"));

        err =BRAP_P_GenCit(hRap->hHeap, hRap->hRegister, psCitInputStruct, &(hFwTaskCreate->sCitOutput));
        if(BERR_SUCCESS != err)
        {
            BDBG_ERR(("ERROR returned from Cit module %d!",err));
            err =BERR_TRACE(err);
            goto error;
        }

#ifdef RAP_MULTISTREAM_DECODER_SUPPORT        
        /*If Current task is Mixer(Master) of Multi Stream Decoder, Then store Common inter task DRAM address*/
        if(false == hDspCh->bChSpecificDspCh)
        {       
            BRAP_ChannelHandle  hRapCh;

            hRapCh = BRAP_P_GetRapChHandle(hDspCh->uHandle.hAssociation);
            if(!(BRAP_P_IsPointerValid((void *)hRapCh)))
            {
                BDBG_ERR(("No Raptor channel found in Association handle"));
                err = BERR_INVALID_PARAMETER;
                goto error;
            }            
            
            if(BRAP_P_IsFwMixingPostLoopbackEnabled(hRapCh))
            {
                for(j = 0; j < BRAP_AF_P_MAX_NODES; j++)
                {
                    if(hFwTaskCreate->sCitOutput.sCit.sNodeConfig[j].eAlgoId == BRAP_AF_P_AlgoId_eFWMixerPostProc)
                    {
                        for(k=0; k<BRAP_P_MAX_FW_STG_INPUTS; k++)
                        {
                            hRapCh->hMultiStreamDecoder->ui32InterTaskIoBufferHandle[k] = hFwTaskCreate->sCitOutput.sCit.sNodeConfig[j].ui32NodeIpBuffCfgAddr[k];
                            hRapCh->hMultiStreamDecoder->ui32InterTaskIoGenericBufferHandle[k] = hFwTaskCreate->sCitOutput.sCit.sNodeConfig[j].ui32NodeIpGenericDataBuffCfgAddr[k];
                        }
                        hFwTaskCreate->bMaster = true;
                        break;
                    }
                }
            }
        }
        else
        {
            if((BRAP_P_IsFwMixingPostLoopbackEnabled(hDspCh->uHandle.hRapCh))
            &&((hDspCh->sDspAudioParams.sExtAudioParams.eType == BRAP_DSPCHN_AudioType_eAc3Plus)
                ||(hDspCh->sDspAudioParams.sExtAudioParams.eType == BRAP_DSPCHN_AudioType_eAc3)
                ||(hDspCh->sDspAudioParams.sExtAudioParams.eType == BRAP_DSPCHN_AudioType_eAacAdts)
                ||(hDspCh->sDspAudioParams.sExtAudioParams.eType == BRAP_DSPCHN_AudioType_eAacLoas)
                ||(hDspCh->sDspAudioParams.sExtAudioParams.eType == BRAP_DSPCHN_AudioType_eAacSbrAdts)
                ||(hDspCh->sDspAudioParams.sExtAudioParams.eType == BRAP_DSPCHN_AudioType_eAacSbrLoas)))
            {
                bool    bBreakLoop =false;
                for (k = 0; k < BRAP_RM_P_MAX_DSPS; k++)
                {
                    for (j = 0; j < BRAP_RM_P_MAX_FW_TASK_PER_DSP; j++)
                    {
                        if((hRap->hFwTask[k][j]!=NULL)
                            &&(hRap->hFwTask[k][j]->bMaster == true)
                            &&(hRap->hFwTask[k][j]->uiTaskId != BRAP_FWIF_P_INVALID_TSK_ID))
                        {
                            hFwTaskCreate->uiMasterTaskId = hRap->hFwTask[k][j]->uiTaskId;
                            bBreakLoop = true;
                            break;
                        }
                    }
                    if(bBreakLoop == true)
                        break;
                }
            }
        }
#endif        

        if(true == hDspCh->bChSpecificDspCh)
        {
            BRAP_CITGEN_P_CalcThresholdZeroFillTimeAudOffset(psAudioParams->sExtAudioParams.eLowDelayEnableMode, psCitInputStruct, &sCtbOutput);
            hDspCh->uHandle.hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.ui32AudioOffset = sCtbOutput.ui32AudOffset*45;
                                                       /*Update AudOffset value in number of ticks for 45khz clock in TSM user config params*/
            ui32Threshold = sCtbOutput.ui32Threshold;
            ui32BlockTime = sCtbOutput.ui32BlockTime;                                                   
        }
        else
        {
            BRAP_CITGEN_P_CalcThresholdZeroFillTimeAudOffset(psAudioParams->sExtAudioParams.eLowDelayEnableMode, psCitInputStruct, &sCtbOutput);

            /* ToDo: Populate TsmConfig for Assoc N/W
            hDspCh->uHandle.hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.ui32AudioOffset = sCtbOutput.ui32AudOffset*45;
            */
                                                       /*Update AudOffset value in number of ticks for 45khz clock in TSM user config params*/
            ui32Threshold = sCtbOutput.ui32Threshold;
            ui32BlockTime = sCtbOutput.ui32BlockTime;                                                   
        }
        
        for(j=0; j< BRAP_P_MAX_FW_BRANCH_PER_FW_TASK; j++)
        {
            if (BRAP_P_IsPointerValid((void *)psCitInputStruct->pBranchInfo[j]))
            {
                psCitInputStruct->pBranchInfo[j] = NULL;
            }
        }

        BDBG_MSG(("GENCIT DONE"));

        /*	Analyze the CIT generated : First level Information */
        BRAP_P_AnalyseCit(hRap->hHeap, &(hFwTaskCreate->sCitOutput.sCit));
        BDBG_MSG(("ANALYSECIT DONE"));
		
        /* Download CIT structure into DRAM */
        err = BRAP_P_CopyDataToDram(
                hDsp->hHeap, 
                (uint32_t *)(&(hFwTaskCreate->sCitOutput.sCit)),      
                hRap->sMemInfo.sTskMemInfo[hDsp->uiDspIndex].sCitStruct[BRAP_GET_TASK_INDEX(hFwTaskCreate->uiTaskId)].ui32BaseAddr,
                hRap->sMemInfo.sTskMemInfo[hDsp->uiDspIndex].sCitStruct[BRAP_GET_TASK_INDEX(hFwTaskCreate->uiTaskId)].ui32Size
                );

        if(BERR_SUCCESS != err)
        {
               BDBG_ERR(("Error in Copying data to DRAM"));
               err = BERR_TRACE(err);
               goto error;
        }           

        /* Initialize interframe buffers for all the nodes */
        err = BRAP_DSP_P_InitInterframeBuffer(hFwTaskCreate);  
        if(BERR_SUCCESS != err)
        {
               BDBG_ERR(("Error in initializing Interframe buffers for Task id %d",hFwTaskCreate->uiTaskId));
               err = BERR_TRACE(err);
               goto error;
        }

        /* Initialize configuration parameters for all the nodes */

		/* TODO Change  task handle here */
		
        ui32NumBranchesValid = hFwTaskCreate->sCitOutput.ui32NumBranches;
        for(uiBranchId = 0; uiBranchId < ui32NumBranchesValid; uiBranchId++)
        {
            /* Get the configuration buffer address for this stage */
            ui32NumStagesValid = hFwTaskCreate->sCitOutput.sCitBranchInfo[uiBranchId].ui32NumStages;
            for(uiStageId = 0; uiStageId < ui32NumStagesValid; uiStageId++)
            {
                bPpConfigDone = false;
                psOpStgInfo = &(hFwTaskCreate->sCitOutput.sCitBranchInfo[uiBranchId].sCitStgInfo[uiStageId]);
                uiStartNodeIndex = psOpStgInfo->ui32StartNodeIndex;
                uiConfigBufAddr = psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32UserParamBufAdr;
                uiConfigBufSize = psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32UserParamBufSize;
                ui32TsmNodeIndex = psOpStgInfo->ui32TsmNodeIndex;
                
                /* Check if this is a decode stage */
                if(BRAP_CIT_P_AudioAlgoType_eDecode== psOpStgInfo->eAudioAlgoType    /*psAudProcNetwork->sAudProcessingStage[uiBranchId][uiStageId].bDecoderStage*/)
                {
                    /* Only Channel N/W has Decode stage */
                    if(true == hDspCh->bChSpecificDspCh)
                    {                
                    /* Decode stage. Program the decoder configuration parameters stored in
                    channel handle. */                        
                    if(!((uiConfigBufAddr == BRAP_P_INVALID_DRAM_ADDRESS) ||
                   (uiConfigBufAddr == 0) ||
                   (uiConfigBufAddr == BRAP_AF_P_DRAM_ADDR_INVALID)||
                        (0 == uiConfigBufSize)) )
                    {
                         err = BRAP_FWIF_P_SetDecodeStageConfigParams(hDspCh->uHandle.hRapCh, 
                            hDspCh->sDspAudioParams.sExtAudioParams.eType,
                            hDspCh->sDspAudioParams.sExtAudioParams.eDecodeMode,
                            uiConfigBufAddr, uiConfigBufSize,&uiActualConfigSize);
                        if( BERR_SUCCESS!=err )
                        {
                            goto error;
                        }
                    }
                    }           
                    
                    if(!((psOpStgInfo->sFwOpNodeInfo[ui32TsmNodeIndex].ui32UserParamBufAdr == BRAP_P_INVALID_DRAM_ADDRESS) ||
                            (psOpStgInfo->sFwOpNodeInfo[ui32TsmNodeIndex].ui32UserParamBufAdr == 0) ||
                            (psOpStgInfo->sFwOpNodeInfo[ui32TsmNodeIndex].ui32UserParamBufAdr == BRAP_AF_P_DRAM_ADDR_INVALID)||
                            (0 ==  psOpStgInfo->sFwOpNodeInfo[ui32TsmNodeIndex].ui32UserParamBufSize)) )
                    {
                        err = BRAP_FWIF_P_SetTsmStageConfigParams(hDspCh, 
                            psOpStgInfo->sFwOpNodeInfo[ui32TsmNodeIndex].eFwExecId,
                            psOpStgInfo->sFwOpNodeInfo[ui32TsmNodeIndex].ui32UserParamBufAdr, 
                            psOpStgInfo->sFwOpNodeInfo[ui32TsmNodeIndex].ui32UserParamBufSize);
                        if( BERR_SUCCESS!=err )
                        {
                            goto error;
                        }     
                    }                          
           
                    if(!((psOpStgInfo->sFwOpNodeInfo[0].ui32UserParamBufAdr== BRAP_P_INVALID_DRAM_ADDRESS) ||
                            (psOpStgInfo->sFwOpNodeInfo[0].ui32UserParamBufAdr == 0) ||
                            (psOpStgInfo->sFwOpNodeInfo[0].ui32UserParamBufAdr == BRAP_AF_P_DRAM_ADDR_INVALID)||
                            (0 ==  psOpStgInfo->sFwOpNodeInfo[0].ui32UserParamBufSize)) )
                            {
                                BDBG_MSG (("Set Frame Sync Config Params"));
                                err = BRAP_FWIF_P_SetFrameSyncStageConfigParams (hDspCh, 
                                                            psOpStgInfo->sFwOpNodeInfo[0].ui32UserParamBufAdr, 
                                                            psOpStgInfo->sFwOpNodeInfo[0].ui32UserParamBufSize);
                                
                                if( BERR_SUCCESS!=err )
                                {
                                    goto error;
                            }
                        }
                }
                else if((BRAP_CIT_P_AudioAlgoType_eEncode == psOpStgInfo->eAudioAlgoType))
                {
                    /* Audio processing stage. Program configuration parameters stored in
                        audio processing stage handle. */
                        for(j = BRAP_ProcessingType_eEncodeStartIndex ; j <  BRAP_ProcessingType_ePostprocessingStartIndex;j++)
                        {
                            if(BRAP_sMapProcessingEnum[j].uEnumName.eEncodeType == psOpStgInfo->uAlgorithm.eEncAudioAlgo)
                            {
                                eProcType = j;
                                break;
                            }
                        }
                        if(j == BRAP_ProcessingType_ePostprocessingStartIndex)
                        {
                            BDBG_ERR(("Invalid encode type returned by CIT Module eEncAudioAlgo =%d",psOpStgInfo->uAlgorithm.eEncAudioAlgo));
                            err = BERR_INVALID_PARAMETER;
                            goto error;
                        }

                        
                        for(j =0; j<BRAP_P_MAX_DST_PER_RAPCH; j++)
                        {
                            if(!(BRAP_P_BRANCH_VALID(hDspCh->sFwTaskInfo[i].sProcessingNw.sAudProcessingStage,j)))                        
                            {/* No more valid branch */
                                break;
                            }
                            for(k =0; k< BRAP_MAX_STAGE_PER_BRANCH_SUPPORTED;k++)
                            {
                                hAudioProcessing = hDspCh->sFwTaskInfo[i].sProcessingNw.sAudProcessingStage[j][k].hAudioProcessing;
                                
                                if(!(BRAP_P_STAGE_VALID(hDspCh->sFwTaskInfo[i].sProcessingNw.sAudProcessingStage,j,k)))                                                        
                                {/* No more valid stages */
                                    break;
                                }
                                
                                if((BRAP_P_IsPointerValid((void *)hAudioProcessing))
                                    &&(eProcType == hAudioProcessing->sProcessingStageSettings.eAudioProcessing)
                                    &&(uiBranchId== hDspCh->sFwTaskInfo[i].sProcessingNw.sAudProcessingStage[j][k].ui32CitBranchId)
                                    &&(uiStageId== hDspCh->sFwTaskInfo[i].sProcessingNw.sAudProcessingStage[j][k].ui32CitStageId)                                    
                                    )
                                {
                                     BDBG_MSG(("uiBranchId > %d, uiStageId > %d", uiBranchId, uiStageId));
                                    err = BRAP_FWIF_P_SetProcessingStageConfigParams(hAudioProcessing,
                                        uiConfigBufAddr, uiConfigBufSize,&uiActualConfigSize);
                                    if( BERR_SUCCESS!=err )
                                    {
                                        BDBG_ERR(("Error in setting config params for Processing type %d",hAudioProcessing->sProcessingStageSettings.eAudioProcessing));
                                        err = BERR_TRACE(BERR_INVALID_PARAMETER);
                                        goto error;
                                    } 
                                    bPpConfigDone =true;
                                    break;
                                }
                            }
                            if(true == bPpConfigDone)
                            {
                                break;
                            }
                        }
                        if(false == bPpConfigDone)
                        {
                            BDBG_ERR(("Could not find the hAudioProcessing, corresponding Encode Algo type %d passed by CIT ",psOpStgInfo->uAlgorithm.eEncAudioAlgo));
                            err = BERR_TRACE(BERR_INVALID_PARAMETER);
                            goto error;
                        }
                }
                else if((BRAP_CIT_P_AudioAlgoType_ePostProc== psOpStgInfo->eAudioAlgoType))
                {
                        /* Audio processing stage. Program configuration parameters stored in
                            audio processing stage handle. */
                        for(j = BRAP_ProcessingType_ePostprocessingStartIndex ; j <  BRAP_ProcessingType_eNone;j++)
                        {
                            if(BRAP_sMapProcessingEnum[j].uEnumName.eProcessingType == psOpStgInfo->uAlgorithm.eProcAudioAlgo)
                            {
                                eProcType = j;
                                break;
                            }
                        }
                        if(j == BRAP_ProcessingType_eNone)
                        {
                            BDBG_ERR(("Invalid processing type returned by CIT Module eEncAudioAlgo =%d",psOpStgInfo->uAlgorithm.eEncAudioAlgo));
                            err = BERR_INVALID_PARAMETER;
                            goto error;
                        }
                        
                        for(j =0; j<BRAP_P_MAX_DST_PER_RAPCH; j++)
                        {
                            if(!(BRAP_P_BRANCH_VALID(hDspCh->sFwTaskInfo[i].sProcessingNw.sAudProcessingStage,j)))                                
                            {/* No more valid branch */
                                break;
                            }
                            for(k =0; k< BRAP_MAX_STAGE_PER_BRANCH_SUPPORTED;k++)
                            {
                                hAudioProcessing = hDspCh->sFwTaskInfo[i].sProcessingNw.sAudProcessingStage[j][k].hAudioProcessing;
                                
                                if(!(BRAP_P_STAGE_VALID(hDspCh->sFwTaskInfo[i].sProcessingNw.sAudProcessingStage,j,k)))                                                                                        
                                {/* No more valid stages */
                                    break;
                                }
                                if(NULL == hAudioProcessing) /*It might be possible that it is a Clone stage of an Processing Stage. And it is added in Fw branch. e.g. PP on Stereo Downmix branch*/
                                {
                                    if((hDspCh->sFwTaskInfo[i].sProcessingNw.sAudProcessingStage[j][k].ui32CitBranchId !=BRAP_INVALID_VALUE)
                                        &&(hDspCh->sFwTaskInfo[i].sProcessingNw.sAudProcessingStage[j][k].ui32CitStageId !=BRAP_INVALID_VALUE))
                                    {
                                        uiMasterStageId = hDspCh->sFwTaskInfo[i].sProcessingNw.sAudProcessingStage[j][k].ui32MasterStageId;
                                        uiMasterBranchId = hDspCh->sFwTaskInfo[i].sProcessingNw.sAudProcessingStage[j][k].ui32MasterBranchId;                                        
                                        hAudioProcessing = hDspCh->sFwTaskInfo[i].sProcessingNw.sAudProcessingStage[uiMasterBranchId][uiMasterStageId].hAudioProcessing;
                                    }
                                }
                                if((BRAP_P_IsPointerValid((void *)hAudioProcessing))
                                    &&(eProcType == hAudioProcessing->sProcessingStageSettings.eAudioProcessing)
                                    &&(uiBranchId== hDspCh->sFwTaskInfo[i].sProcessingNw.sAudProcessingStage[j][k].ui32CitBranchId)
                                    &&(uiStageId== hDspCh->sFwTaskInfo[i].sProcessingNw.sAudProcessingStage[j][k].ui32CitStageId)                                    
                                    )
                                {
                                     BDBG_MSG(("uiBranchId > %d, uiStageId > %d", uiBranchId, uiStageId));
                                    err = BRAP_FWIF_P_SetProcessingStageConfigParams(hAudioProcessing,
                                        uiConfigBufAddr, uiConfigBufSize,&uiActualConfigSize);
                                    if( BERR_SUCCESS!=err )
                                    {
                                        BDBG_ERR(("Error in setting config params for Processing type %d",hAudioProcessing->sProcessingStageSettings.eAudioProcessing));
                                        err = BERR_TRACE(BERR_INVALID_PARAMETER);
                                        goto error;
                                    } 
                                    bPpConfigDone =true;
                                    break;
                                }
                            }
                            if(true == bPpConfigDone)
                            {
                                break;
                            }
                        }
                        if(false == bPpConfigDone)
                        {
                            BDBG_ERR(("Could not find the hAudioProcessing, corresponding Processing Algo type %d passed by CIT ",psOpStgInfo->uAlgorithm.eProcAudioAlgo));
                            err = BERR_TRACE(BERR_INVALID_PARAMETER);
                            goto error;
                }
            }

/* Status Buffer */
            BDBG_MSG(("psOpStgInfo > %x", psOpStgInfo));
            BDBG_MSG(("uiStartNodeIndex > %x", uiStartNodeIndex));
            BDBG_MSG(("psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32StatusBufAdr > %x", 
                psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32StatusBufAdr));
            BDBG_MSG(("psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].eFwExecId > %x", 
                psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].eFwExecId));
            ui32StatusBufAdr = psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32StatusBufAdr; 
            if((ui32StatusBufAdr != BRAP_P_INVALID_DRAM_ADDRESS) &&
               (ui32StatusBufAdr != 0) &&
               (ui32StatusBufAdr != BRAP_AF_P_DRAM_ADDR_INVALID)&&
               (0 != psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32StatusBufSize)) 
            {
/*                (*((volatile uint32_t *)ui32StatusBufAdr)) = 0;*/
/*                BRAP_MemWrite32(ui32StatusBufAdr, 0);*/
                BKNI_Memset((void *)ui32StatusBufAdr,1,psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32StatusBufSize);
            }

            if(true == hDspCh->bChSpecificDspCh)
            {                
            if(BRAP_CIT_P_AudioAlgoType_eDecode== psOpStgInfo->eAudioAlgoType)
            {
                if(hDspCh->uHandle.hRapCh->bInternalCallFromRap == false)
                {
                    /* Initialize TSM status Buffer */
                    ui32StatusBufAdr = psOpStgInfo->sFwOpNodeInfo[ui32TsmNodeIndex].ui32StatusBufAdr; 
                    if((ui32StatusBufAdr != BRAP_P_INVALID_DRAM_ADDRESS) &&
                       (ui32StatusBufAdr != 0) &&
                       (ui32StatusBufAdr != BRAP_AF_P_DRAM_ADDR_INVALID)&&
                       (0 != psOpStgInfo->sFwOpNodeInfo[ui32TsmNodeIndex].ui32StatusBufSize)) 
                    {
        /*                (*((volatile uint32_t *)ui32StatusBufAdr)) = 0;*/
/*                       BRAP_MemWrite32(ui32StatusBufAdr, 0);*/
                        BKNI_Memset((void *)ui32StatusBufAdr,1,psOpStgInfo->sFwOpNodeInfo[ui32TsmNodeIndex].ui32StatusBufSize);
                    }
                }
            }
            }  
            else
            {
                /* Currently TSM is dummy for Assoc N/W. Handle accordingly if needed in future */
            }
        }        
    }

        for(j = 0; j < BRAP_AF_P_MAX_NUM_SPDIF_PORTS; j++)
        {
            /*C-Bit Buffer */
            ui32CBitBufAddr = hFwTaskCreate->sCitOutput.sSpdifUserConfigAddr[j].ui32DramBufferAddress;
            ui32CBitBufSize = hFwTaskCreate->sCitOutput.sSpdifUserConfigAddr[j].ui32BufferSizeInBytes;

            if((ui32CBitBufAddr != BRAP_P_INVALID_DRAM_ADDRESS) &&
               (ui32CBitBufAddr != 0) &&
               (ui32CBitBufAddr != BRAP_AF_P_DRAM_ADDR_INVALID) ) 
            {            
                BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(ui32CBitBufAddr), &(ui32PhysAddr));	

                BDBG_MSG(("ui32CBitBufAddr = %#x, ui32CBitBufSize =%d",ui32PhysAddr,ui32CBitBufSize));

                BRAP_FWIF_P_InitSpdifChanStatusParams(( BRAP_OP_SpdifChanStatusParams   *)&(psAudioParams->sSpdifChStatusParams[j]),ui32CBitBufAddr,ui32CBitBufSize);
            }
        }

        if(false == bWdgRecovery)
        {
        /* Find free Fifo Ids */
        for(j=0;j<BRAP_DSP_P_NUM_FIFOS;j++)
        {
            if(false == hDsp->bFifoUsed[j])
                break;
        }
        if(BRAP_DSP_P_NUM_FIFOS == j)
        {
            BDBG_ERR(("Unable to find free fifo for SYNC MSG!"));
            err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            goto error;
        }
        else
        {
            hDsp->bFifoUsed[j] = true;
            ui32SyncFifo = j;
        }
        for(j=0;j<BRAP_DSP_P_NUM_FIFOS;j++)
        {
            if(false == hDsp->bFifoUsed[j])
                break;
        }
        if(BRAP_DSP_P_NUM_FIFOS == j)
        {
            BDBG_ERR(("Unable to find free fifo for ASYNC MSG!"));
            err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            goto error;
        }
        else
        {
            hDsp->bFifoUsed[j] = true;
            ui32AsyncFifo = j;
        }
        
        for(j=0;j<BRAP_MAX_FW_TSK_SUPPORTED;j++)
        {
                if(BRAP_FWIF_P_INVALID_TSK_ID == hRap->sMemInfo.sTskMemInfo[hDsp->uiDspIndex].sTskQueue[j].ui32TaskId)
                break;
        }
        if(BRAP_MAX_FW_TSK_SUPPORTED == j)
        {
            BDBG_ERR(("Unable to find free queues for this task!"));
            err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            goto error;
        }
        else
        {
                ui32QueueId = j;
            }
            hRap->sMemInfo.sTskMemInfo[hDsp->uiDspIndex].sTskQueue[ui32QueueId].ui32TaskId = hFwTaskCreate->uiTaskId;
            hRap->sMemInfo.sTskMemInfo[hDsp->uiDspIndex].sTskQueue[ui32QueueId].sTskSyncQueue.ui32FifoId
                = ui32SyncFifo;
            hRap->sMemInfo.sTskMemInfo[hDsp->uiDspIndex].sTskQueue[ui32QueueId].sTskAsyncQueue.ui32FifoId
                = ui32AsyncFifo;
        }
        else
        {
            for(j=0;j<BRAP_MAX_FW_TSK_SUPPORTED;j++)
            {
                if(hFwTaskCreate->uiTaskId == hRap->sMemInfo.sTskMemInfo[hDsp->uiDspIndex].sTskQueue[j].ui32TaskId)
                    break;
            }
            if(BRAP_MAX_FW_TSK_SUPPORTED == j)
            {
                BDBG_ERR(("Unable to find matching queues for this task in the handle for watchdog recovery!"));
                err = BERR_TRACE(BRAP_ERR_BAD_DEVICE_STATE);
                goto error;
            }
            else
            {
                ui32QueueId = j;
            }
        }

            if(true == bWdgRecovery)
            {
                   hMsgQueue = hFwTaskCreate->hSyncMsgQueue;
            }
            
            err = BRAP_FWIF_P_CreateMsgQueue(&(hRap->sMemInfo.sTskMemInfo[hDsp->uiDspIndex].sTskQueue[ui32QueueId].sTskSyncQueue), 
                hDspCh->hHeap, hDspCh->hRegister, hDsp->ui32Offset, &hMsgQueue,bWdgRecovery);
            if(BERR_SUCCESS != err)
            {
                BDBG_ERR(("BRAP_DSPCHN_P_Start: SYNC RSP queue creation failed!"));
                err = BERR_TRACE(err);
                goto error;
            }
           if(false == bWdgRecovery)
            {
                /* Fill up task handle message queue */
                hFwTaskCreate->hSyncMsgQueue = hMsgQueue;
            }

            if(true == bWdgRecovery)
            {
                   hMsgQueue = hFwTaskCreate->hAsyncMsgQueue;
            }
            err = BRAP_FWIF_P_CreateMsgQueue(&(hRap->sMemInfo.sTskMemInfo[hDsp->uiDspIndex].sTskQueue[j].sTskAsyncQueue), 
                    hDspCh->hHeap, hDspCh->hRegister, hDsp->ui32Offset, &hMsgQueue,bWdgRecovery);
            if(BERR_SUCCESS != err)
            {
                BDBG_ERR(("BRAP_DSPCHN_P_Start: ASYNC RSP queue creation failed!"));
                err = BERR_TRACE(err);
                goto error;
            }
           if(false == bWdgRecovery)
            {
                /* Fill up task handle message queue */
                hFwTaskCreate->hAsyncMsgQueue = hMsgQueue;
            }        
           if(false == bWdgRecovery)
            {
               hFwTaskCreate->pAsyncMsgMemory = 
                (void *)(hRap->sMemInfo.sTskMemInfo[hDsp->uiDspIndex].sTskQueue[j].sAsyncMsgBufmem.ui32BaseAddr);
            }        


        /* Unmask interrupt bits corresponding to synchronous and asynchronous msg queues */
        err = BRAP_P_UnmaskTask(hFwTaskCreate);
        if(BERR_SUCCESS != err)
        {
            BDBG_ERR(("BRAP_DSPCHN_P_Start: Unable to unmask task id interrupt!"));
            err = BERR_TRACE(err);
            goto error;
        }

        /* Send Start Task Command */
        sCommand.sCommandHeader.ui32CommandID = BRAP_FWIF_P_START_TASK_COMMAND_ID;
        sCommand.sCommandHeader.ui32CommandCounter = 0;
        sCommand.sCommandHeader.ui32TaskID = hFwTaskCreate->uiTaskId;
        sCommand.sCommandHeader.eResponseType = BRAP_FWIF_P_ResponseType_eResponseRequired;
        switch(psAudioParams->sExtAudioParams.eDecodeMode)
        {
            case BRAP_DSPCHN_DecodeMode_eDecode:
                sCommand.uCommand.sStartTask.eTaskAlgoType = BRAP_FWIF_P_AlgoType_eDecode; 
                sCommand.uCommand.sStartTask.eDeadlineComputationFuncType = 
                    BRAP_FWIF_P_DeadLineComputeFuncType_eRealtimeDecode;
                if(true == hDspCh->bChSpecificDspCh)
                {                
                    BRAP_P_EnableAdaptiveRate(hDspCh->uHandle.hRapCh,&bAdaptiveRateEnable,hDspCh->sDspAudioParams.sExtAudioParams.bPPMCorrection);
                }
                else
                {
                    bAdaptiveRateEnable = false;
                }
                sCommand.uCommand.sStartTask.ePPMCorrEnable = bAdaptiveRateEnable;
                break;
            case BRAP_DSPCHN_DecodeMode_ePassThru:
                /* BRUP Change - Till FW adds proper programming */
/*                sCommand.uCommand.sStartTask.eTaskAlgoType = BRAP_FWIF_P_AlgoType_ePassThru; */
/*                sCommand.uCommand.sStartTask.eDeadlineComputationFuncType = 
                    BRAP_FWIF_P_DeadLineComputeFuncType_ePassthrough;*/
                sCommand.uCommand.sStartTask.eTaskAlgoType = BRAP_FWIF_P_AlgoType_eDecode; 
                sCommand.uCommand.sStartTask.eDeadlineComputationFuncType = 
                    BRAP_FWIF_P_DeadLineComputeFuncType_eRealtimeDecode;
                sCommand.uCommand.sStartTask.ePPMCorrEnable = BRAP_AF_P_eDisable;
                break;
            case BRAP_DSPCHN_DecodeMode_eInvalid:
            default:
                BDBG_ERR(("INVALID DECODE MODE"));
                err = BERR_TRACE(BERR_NOT_SUPPORTED);
                goto error;
                break;
        }
        BDBG_MSG(("PPMCorrection > %d", sCommand.uCommand.sStartTask.ePPMCorrEnable));
        if(hFwTaskCreate->uiMasterTaskId != BRAP_FWIF_P_INVALID_TSK_ID)
        {
            sCommand.uCommand.sStartTask.eTaskType =BRAP_FWIF_P_TaskType_eSlave;
            sCommand.uCommand.sStartTask.ui32MasterTaskId =hFwTaskCreate->uiMasterTaskId;            
        }
        else
        {
        sCommand.uCommand.sStartTask.eTaskType = BRAP_FWIF_P_TaskType_eRealtime;
            sCommand.uCommand.sStartTask.ui32MasterTaskId =BRAP_FWIF_P_INVALID_TSK_ID;                        
        }
        BRAP_P_ConvertAddressToOffset(hRap->hHeap, 
            (void *)(hRap->sMemInfo.sTskMemInfo[hDsp->uiDspIndex].sTskInfo[BRAP_GET_TASK_INDEX(hFwTaskCreate->uiTaskId)].ui32BaseAddr),
            &ui32PhysAddr);
        sCommand.uCommand.sStartTask.ui32DramDeadlineConfigStructAddr = ui32PhysAddr;

        BRAP_P_ConvertAddressToOffset(hRap->hHeap, 
            (void *)(hRap->sMemInfo.sTskMemInfo[hDsp->uiDspIndex].sCitStruct[BRAP_GET_TASK_INDEX(hFwTaskCreate->uiTaskId)].ui32BaseAddr),
            &ui32PhysAddr);
        sCommand.uCommand.sStartTask.ui32DramTaskConfigAddr = ui32PhysAddr;
        sCommand.uCommand.sStartTask.ui32SyncQueueId = hFwTaskCreate->hSyncMsgQueue->ui32FifoId;
        sCommand.uCommand.sStartTask.ui32AsyncQueueId = hFwTaskCreate->hAsyncMsgQueue->ui32FifoId;

        BRAP_P_ConvertAddressToOffset(hRap->hHeap, 
            (void *)(hFwTaskCreate->sCitOutput.sStackSwapBuff.ui32DramBufferAddress),
            &ui32PhysAddr);
        sCommand.uCommand.sStartTask.sDramStackBuffer.ui32DramBufferAddress = ui32PhysAddr;
        sCommand.uCommand.sStartTask.sDramStackBuffer.ui32BufferSizeInBytes = hFwTaskCreate->sCitOutput.sStackSwapBuff.ui32BufferSizeInBytes;
        
        BRAP_P_ConvertAddressToOffset(hRap->hHeap, 
            (void *)(hRap->sMit.sAlgoDwnldDetail[BRAP_AF_P_AlgoId_eSystemDecodeTask].ui32AlgoCodePtr),
            &ui32PhysAddr);

        sCommand.uCommand.sStartTask.sDramTaskCodeBuffer.ui32DramBufferAddress = ui32PhysAddr;
        sCommand.uCommand.sStartTask.sDramTaskCodeBuffer.ui32BufferSizeInBytes= hRap->sMit.sAlgoDwnldDetail[BRAP_AF_P_AlgoId_eSystemDecodeTask].ui32AlgoCodeSize;

        
        /* Fill up start task parameters */
        pTaskParams = (BRAP_FWIF_P_TaskParamInfo *)(hRap->sMemInfo.sTskMemInfo[hDsp->uiDspIndex].sTskInfo[BRAP_GET_TASK_INDEX(hFwTaskCreate->uiTaskId)].ui32BaseAddr);
/*        err = BRAP_P_ConvertSR(psAudioParams->sExtAudioParams.eOutputSampleRate, &(pTaskParams->ui32SamplingFrequency));*/
        err = BRAP_P_ConvertSR(BAVC_AudioSamplingRate_e48k, (unsigned int *)&(pTaskParams->ui32SamplingFrequency));
        BDBG_MSG(("pTaskParams->ui32SamplingFrequency > %x", pTaskParams->ui32SamplingFrequency));
        if(BERR_SUCCESS != err)
        {
            BDBG_ERR(("BRAP_DSPCHN_P_Start: Convert SR Failed!"));
            err = BERR_TRACE(err);
            goto error;
        }

        pTaskParams->ui32Threshold = ui32Threshold;
        pTaskParams->ui32BlockTime =ui32BlockTime; 

        /*TODO : ui32FrameSize is not required anymore by FW. remove this parameter */
        pTaskParams->ui32FrameSize = 1536;
        /* Provide the Output buffer addresses (or) the handle to DRAM that contains so */
#ifdef RAP_MULTISTREAM_DECODER_SUPPORT                
        if(true == hDspCh->bChSpecificDspCh)
        {    
            if(BRAP_P_IsFwMixingPostLoopbackEnabled(hDspCh->uHandle.hRapCh))
            {
                k = hDspCh->uHandle.hRapCh->uiFWMixerIpIndex;
                pTaskParams->eBufferType = BRAP_AF_P_BufferType_eDRAM;
                pTaskParams->sDspSchedulingBuffInfo.ui32DramSchedulingBuffCfgAddr = 
                    hDspCh->uHandle.hRapCh->hMultiStreamDecoder->ui32InterTaskIoBufferHandle[k];
                bFWSchedulingBufFound = true;
                
                BRAP_P_ConvertAddressToOffset(  hDspCh->hHeap,
                    (void *)(hDspCh->uHandle.hRapCh->hMultiStreamDecoder->ui32FeedbackBufferAddress),
                    &(pTaskParams->ui32MasterTaskFeedbackBuffCfgAddr));
        
                pTaskParams->ui32MasterTaskFeedbackBuffValid = 1;
                hDspCh->uHandle.hRapCh->hMultiStreamDecoder->bDRAMBuffersUsed[k] = true;                
            }
            /* else: Non-MS10/11. The Task outputs to FMM Buffer */
        }       
#endif        
        if(false == bFWSchedulingBufFound)
        {            
            pTaskParams->eBufferType = BRAP_AF_P_BufferType_eFMM;

            if(hFwTaskCreate->uiMasterTaskId == BRAP_FWIF_P_INVALID_TSK_ID)
            {
                if(BRAP_AF_P_BufferType_eFMM !=
                    hDspCh->sNetworkInfo.sDspOutConfig[0].sOutputBufConfig.uIoBuffer.sIoBuffId.eBufferType)
                {
                    BDBG_ERR(("BRAP_DSPCHN_P_Start: DSP output buffer type is invalid"));
                    err = BERR_INVALID_PARAMETER;
                    goto error;                
                }
                ui32RbufOffset = (BCHP_AUD_FMM_BF_CTRL_RINGBUF_1_BASEADDR - BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_BASEADDR) *
                    hDspCh->sNetworkInfo.sDspOutConfig[0].sOutputBufConfig.uIoBuffer.sIoBuffId.ui32BufferId[0] + BCHP_PHYSICAL_OFFSET;
                BDBG_MSG(("hDspCh->sNetworkInfo.sDspOutConfig[0].sOutputBufConfig.uIoBuffer.sIoBuffId.ui32BufferId[0] > %x", hDspCh->sNetworkInfo.sDspOutConfig[0].sOutputBufConfig.uIoBuffer.sIoBuffId.ui32BufferId[0]));

                pTaskParams->sDspSchedulingBuffInfo.sRdbBasedSchedulingBuffer.ui32BaseAddr = BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_BASEADDR + ui32RbufOffset;
                BDBG_MSG(("psAudioParams->sDecOrPtParams.uDecBufFmmParams.sDecPathFmmParams.rBufIndex[0] > %x", psAudioParams->sDecOrPtParams.uDecBufFmmParams.sDecPathFmmParams.rBufIndex[0]));
                BDBG_MSG(("pTaskParams->sDspSchedulingBuffInfo.sRdbBasedSchedulingBuffer.ui32BaseAddr > %x", pTaskParams->sDspSchedulingBuffInfo.sRdbBasedSchedulingBuffer.ui32BaseAddr));
                pTaskParams->sDspSchedulingBuffInfo.sRdbBasedSchedulingBuffer.ui32EndAddr = BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_ENDADDR + ui32RbufOffset;
                pTaskParams->sDspSchedulingBuffInfo.sRdbBasedSchedulingBuffer.ui32ReadAddr = BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR + ui32RbufOffset;
                pTaskParams->sDspSchedulingBuffInfo.sRdbBasedSchedulingBuffer.ui32WriteAddr = BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_WRADDR + ui32RbufOffset;
                pTaskParams->sDspSchedulingBuffInfo.sRdbBasedSchedulingBuffer.ui32WrapAddr = BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_ENDADDR + ui32RbufOffset;
            }     

#ifdef RAP_MULTISTREAM_DECODER_SUPPORT                
            if(hDspCh->bChSpecificDspCh == false)
            {
                BRAP_ChannelHandle  hRapCh;
                
                hRapCh = BRAP_P_GetRapChHandle(hDspCh->uHandle.hAssociation);
                if(!(BRAP_P_IsPointerValid((void *)hRapCh)))
                {
                    BDBG_ERR(("No Raptor channel found in Association handle"));
                    err = BERR_INVALID_PARAMETER;
                    goto error;
                }       
                if(BRAP_P_IsFwMixingPostLoopbackEnabled(hRapCh))
                {
                    BRAP_P_ConvertAddressToOffset(  hDspCh->hHeap,
                        (void *)(hRapCh->hMultiStreamDecoder->ui32FeedbackBufferAddress),
                        &(pTaskParams->ui32MasterTaskFeedbackBuffCfgAddr));
                    
                    pTaskParams->ui32MasterTaskFeedbackBuffValid = 1;
                }
            }
#endif
            /*else
            {
                pTaskParams->sCircBuffer = hDspCh->sNetworkInfo.sDspOutConfig[0].sOutputBufConfig.uIoBuffer.sIoBuffer.sCircBuffer[0];
                BDBG_MSG(("ui32BaseAddr = 0x%x",pTaskParams->sCircBuffer.ui32BaseAddr));     
            }*/
        }
        pTaskParams->eFmmDstFsRate = hDspCh->sNetworkInfo.sDspOutConfig[0].sOutputBufConfig.eFmmDstFsRate;

        if(true == hDspCh->bChSpecificDspCh)
        {
            BRAP_ChannelHandle hRapCh = NULL;
            hRapCh = hDspCh->uHandle.hRapCh;
#if (BRAP_INDEP_OP_DELAY_SUPPORTED == 1)            
            if(hRapCh->bIndepDelayEnabled == true)
            {            
                if(!(BRAP_P_IsPointerValid((void *)hDspCh->sNetworkInfo.sDspOutConfig[0].hDestHandle)))
                {
                    /* No Independent Delay for Channel N/W output feeding to Association network */                
                    pTaskParams->ui32MaxIndepDelay = 0;
                    pTaskParams->ui32IndepDelay = 0;
                }
                else
                {            
                for(l=0;l<BRAP_P_MAX_DST_PER_RAPCH;l++)
                {
                        if(!(BRAP_P_IsPointerValid((void *)hRapCh->pDstDetails[l])))
                        continue;

                    pTaskParams->ui32MaxIndepDelay = BRAP_AF_P_MAX_INDEPENDENT_DELAY;
                    if(BRAP_AudioDst_eOutputPort != hRapCh->pDstDetails[l]->eAudioDst)
                        continue;
                        pTaskParams->ui32IndepDelay = 
                            hDspCh->sNetworkInfo.sDspOutConfig[0].sOutputBufConfig.ui32IndepDelay;                
                    break;
                }
            }
            }
            else
#endif                
            {
                pTaskParams->ui32IndepDelay = 0;
                pTaskParams->ui32MaxIndepDelay = 0;                     
            }

        }
        else
        {
            BRAP_ChannelHandle  hRapCh;
            hRapCh = BRAP_P_GetRapChHandle(hDspCh->uHandle.hAssociation);
            if(!(BRAP_P_IsPointerValid((void *)hRapCh)))
            {
                BDBG_ERR(("No Raptor channel found in Association handle"));
                err = BERR_INVALID_PARAMETER;
                goto error;
            }
#if (BRAP_INDEP_OP_DELAY_SUPPORTED == 1)            
            if(hRapCh->bIndepDelayEnabled == true)
            {                
                pTaskParams->ui32IndepDelay = 0;            
                pTaskParams->ui32MaxIndepDelay = BRAP_AF_P_MAX_INDEPENDENT_DELAY;
            }
            else
#endif          
            {
                pTaskParams->ui32IndepDelay = 0;
                pTaskParams->ui32MaxIndepDelay = 0;            
            }
        }

        /* Enable required asynchronous event notification */        
        hFwTaskCreate->hDsp->ui32EventIdMatrix[BRAP_GET_TASK_INDEX(hFwTaskCreate->uiTaskId)] |= BRAP_FWIF_P_DEFAULT_EVENTS_ENABLE_MASK;
        if(hDspCh->bChSpecificDspCh == true)
        {
            hFwTaskCreate->hDsp->ui32EventIdMatrix[BRAP_GET_TASK_INDEX(hFwTaskCreate->uiTaskId)] |= hDspCh->uHandle.hRapCh->ui32AsynIntMask;
        }
        hFwTaskCreate->hDsp->ui32EventIdMatrix[BRAP_GET_TASK_INDEX(hFwTaskCreate->uiTaskId)] |= (
                                       BRAP_FWIF_P_EventIdMask_eSampleRateChange   
                                     |BRAP_FWIF_P_EventIdMask_eStreamInfoAvail                                       
                                     |BRAP_FWIF_P_EventIdMask_eUnlicensedAlgo                        
                                     |BRAP_FWIF_P_EventIdMask_eFirstPTS_Received 
                                     |BRAP_FWIF_P_EventIdMask_ePTS_error
                                     |BRAP_FWIF_P_EventIdMask_eTSM_Lock
                                     |BRAP_FWIF_P_EventIdMask_eFrameSyncLock 
                                     |BRAP_FWIF_P_EventIdMask_eFrameSyncLockLost
                                     |BRAP_FWIF_P_EventIdMask_eStartOnPTS
                                     |BRAP_FWIF_P_EventIdMask_eStopOnPTS
                                     |BRAP_FWIF_P_EventIdMask_eAstmTsmPass
                                    |BRAP_FWIF_P_EventIdMask_eAudioModeChange
                                     |BRAP_FWIF_P_EventIdMask_eTsmFail);        
        if(psAudioParams->sExtAudioParams.eDecodeMode == BRAP_DSPCHN_DecodeMode_eDecode)
        {
            hFwTaskCreate->hDsp->ui32EventIdMatrix[BRAP_GET_TASK_INDEX(hFwTaskCreate->uiTaskId)] |= BRAP_FWIF_P_EventIdMask_eRampEnable;
        }
        sCommand.uCommand.sStartTask.ui32EventEnableMask = hFwTaskCreate->hDsp->ui32EventIdMatrix[BRAP_GET_TASK_INDEX(hFwTaskCreate->uiTaskId)];
        BDBG_MSG(("hFwTaskCreate->hDsp->ui32EventIdMatrix > %x", hFwTaskCreate->hDsp->ui32EventIdMatrix[BRAP_GET_TASK_INDEX(hFwTaskCreate->uiTaskId )]));

        hFwTaskCreate->uiLastEventType = sCommand.sCommandHeader.ui32CommandID;     
        BRAP_P_EventReset(hDsp,BRAP_GET_TASK_INDEX(hFwTaskCreate->uiTaskId));
        /* End Fill up start task parameters */

        err = BRAP_FWIF_P_SendCommand(hDsp->hCmdQueue, &sCommand,hRap,hFwTaskCreate);

        /*Accept the other Commands , After posting Start task Command */

        
        if(BERR_SUCCESS != err)
        {
            if(hRap->bWatchdogTriggered == false)
            {        
                BDBG_ERR(("BRAP_DSPCHN_P_Start: START_TASK creation failed!"));
                err = BERR_TRACE(err);
                BDBG_ASSERT(0);
                goto error;
            }
            else
                err = BERR_SUCCESS;              
        }
        /* Wait for Ack_Response_Received event w/ timeout */
        err = BRAP_P_EventWait(hDsp, BRAP_DSPCHN_P_START_STOP_EVENT_TIMEOUT_IN_MS,BRAP_GET_TASK_INDEX(hFwTaskCreate->uiTaskId));
        if(BERR_TIMEOUT == err)
        {
            if((hRap->bWatchdogTriggered == false))
            {
                /* Please note that, If the code reaches at this point then there is a potential Bug in Fw 
                code which needs to be debugged. However Watchdog is being triggered to recover the system*/            
                BDBG_WRN(("BRAP_DSPCHN_P_Start: START_TASK ACK timeout! Triggering Watchdog"));
#if 0                
                BDBG_ASSERT(0);                
#endif
                BRAP_Write32(hDsp->hRegister, BCHP_AUD_DSP_INTH0_R5F_SET+ hDsp->ui32Offset,0x1);
                hRap->bWatchdogTriggered  = true;
#if 0                
            err = BERR_TRACE(err);
            goto error;
#endif                 
                err = BERR_SUCCESS;              
            }
            else
                err = BERR_SUCCESS;              
        }

/* Send command for the task , Only if the ack for the Start of the task is recieved */
        hFwTaskCreate->bStopped = false;            

        eMsgType = BRAP_P_MsgType_eSyn;
        
        if(hRap->bWatchdogTriggered == false)
        {
            err = BRAP_FWIF_P_GetMsg(hFwTaskCreate->hSyncMsgQueue, (void *)&sRsp,eMsgType);
        }

        if(BERR_SUCCESS != err)
        {
            if(hRap->bWatchdogTriggered == false)
            {
                BDBG_ERR(("BRAP_DSPCHN_P_Start: Unable to read ACK!"));
                err = BERR_TRACE(err);
                goto error;
            }
            else
                err = BERR_SUCCESS;              
        }

        if((sRsp.sCommonAckResponseHeader.eStatus != BERR_SUCCESS)||
            (sRsp.sCommonAckResponseHeader.ui32ResponseID != BRAP_FWIF_P_START_TASK_RESPONSE_ID)||
            (sRsp.sCommonAckResponseHeader.ui32TaskID != hFwTaskCreate->uiTaskId))
        {
            if((hRap->bWatchdogTriggered == false))
            {
               BDBG_ERR(("BRAP_DSPCHN_P_Start: START_TASK ACK not received successfully!eStatus = %d , ui32ResponseID = %d , ui32TaskID %d ",
                    sRsp.sCommonAckResponseHeader.eStatus,sRsp.sCommonAckResponseHeader.ui32ResponseID,sRsp.sCommonAckResponseHeader.ui32TaskID));
                err = BERR_TRACE(sRsp.sCommonAckResponseHeader.eStatus);
                goto error;
            }
            else
                err = BERR_SUCCESS;              
        }      
       
        }
error:     
    if(false == bWdgRecovery)
    {
        hDspCh->eChState = BRAP_DSPCHN_P_ChState_eStart;     
    }
        
   
    if(psCitInputStruct)
    BKNI_Free(psCitInputStruct);
    if(psAudProcNetwork)
    BKNI_Free(psAudProcNetwork);
    
    BDBG_LEAVE( BRAP_DSPCHN_P_Start );
    return err;    
}	


/***************************************************************************
Summary: Stops a DSP context

Description:
	This function stops a currently running context
	
Returns:
	BERR_SUCCESS - If successful

See Also:
	BRAP_DSPCHN_P_Start
**************************************************************************/
BERR_Code
BRAP_DSPCHN_P_Stop(
	BRAP_DSPCHN_Handle	pDspChn					/* [in] DSPCHN handle */
	)
{
  	BERR_Code err = BERR_SUCCESS;
    BRAP_FWIF_P_FwTaskHandle hFwTask = NULL;
    BRAP_DSP_Handle hDsp; 
    BRAP_FWIF_P_Command sCommand;
    BRAP_Handle         hRap;
    unsigned int         i=0,j=0;
    BRAP_P_MsgType  eMsgType;
    BRAP_FWIF_P_Response sRsp;

    BDBG_ENTER(BRAP_DSPCHN_P_Stop);

    /* Assert on the arguments */
    BDBG_ASSERT(pDspChn);

    if(pDspChn->eChState == BRAP_DSPCHN_P_ChState_eStop)
    {
        BDBG_MSG(("BRAP_DSPCHN_P_Stop: hDspChn 0x%x is already stoped", pDspChn));
        return err;
    }
       
    BKNI_Memset((void *)&sRsp,0,sizeof(BRAP_FWIF_P_Response));
       
    if(true == pDspChn->bChSpecificDspCh)
    {
        hRap = pDspChn->uHandle.hRapCh->hRap;
    }
    else
    {
        hRap = pDspChn->uHandle.hAssociation->hRap;
    }
    
    BRAP_DSPCHN_P_FreeStartTimeFWCtxt(pDspChn);

/*    if((true == pDspChn->bChSpecificDspCh)&&
        (true == pDspChn->sDspAudioParams.bPlayback))*/
    if(true == pDspChn->bChSpecificDspCh)
    {
        unsigned int uiPth = 0, uiPathIndx = 0;
        bool bFoundPath = false;
        BRAP_RM_P_ResrcGrant *psRsrcGrnt = NULL;
        for(uiPth = 0; uiPth < BRAP_P_MAX_PATHS_IN_A_CHAN; uiPth++)
        {
            if((BRAP_P_IsPointerValid((void *)pDspChn->uHandle.hRapCh->pPath[uiPth]))
                && (pDspChn->uHandle.hRapCh->pPath[uiPth]->hDspCh == pDspChn))
                {
                    uiPathIndx = uiPth;
                    bFoundPath = true;                    
                    break;
                }
        }
        if(uiPth >= BRAP_P_MAX_PATHS_IN_A_CHAN)
        {
            BDBG_ERR(("No sPath allocated for the running DspCh"));
    		return BERR_TRACE(err); 
        }
    	psRsrcGrnt = hRap->sOpenTimeMallocs.psResrcGrant;
    	if ( NULL==psRsrcGrnt )
    	{
    			return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    	}
	BKNI_Memset( psRsrcGrnt, 0, sizeof(BRAP_RM_P_ResrcGrant) ); 
    
        BRAP_RM_P_InitResourceGrant(psRsrcGrnt,true);
/*        BKNI_Memcpy((void *)(volatile void*)psRsrcGrnt,
            (void *)(&(pDspChn->uHandle.hRapCh->pPath[uiPathIndx]->sRsrcGrnt)),
            sizeof(BRAP_RM_P_ResrcGrant));*/
        psRsrcGrnt->uiDspId = pDspChn->uHandle.hRapCh->pPath[uiPathIndx]->sRsrcGrnt.uiDspId;
        psRsrcGrnt->uiDspContextId = pDspChn->uHandle.hRapCh->pPath[uiPathIndx]->sRsrcGrnt.uiDspContextId;
        err = BRAP_RM_P_FreeResources(hRap->hRm, psRsrcGrnt,true);
        if(BERR_SUCCESS != err)
        {
            BDBG_ERR(("Unable to free allocated DSP"));
    		return BERR_TRACE(err); 
        }
        pDspChn->uHandle.hRapCh->pPath[uiPathIndx]->sRsrcGrnt.uiDspId = BRAP_RM_P_INVALID_INDEX;
        pDspChn->uHandle.hRapCh->pPath[uiPathIndx]->sRsrcGrnt.uiDspContextId = BRAP_RM_P_INVALID_INDEX;
    }


    for(i=0; i<BRAP_FWIF_P_MAX_FW_TASK_PER_DSPCHN; i++)
    {
        hFwTask = pDspChn->sFwTaskInfo[i].hFwTask;

        if(!(BRAP_P_IsPointerValid((void *)hFwTask)))
            continue;
        if(hFwTask->bStopped == true)
        {
            BDBG_MSG(("Task Already stopped"));
            continue;
        }
        hDsp = hFwTask->hDsp;

        /*Prepare command to stop the task */
        sCommand.sCommandHeader.ui32CommandID = BRAP_FWIF_P_STOP_TASK_COMMAND_ID;
        sCommand.sCommandHeader.ui32CommandCounter = 0;
        sCommand.sCommandHeader.ui32TaskID = hFwTask->uiTaskId;
        sCommand.sCommandHeader.eResponseType = BRAP_FWIF_P_ResponseType_eResponseRequired;

        if(hFwTask->uiMasterTaskId != BRAP_FWIF_P_INVALID_TSK_ID)
        {
            sCommand.uCommand.sStopTask.eTaskType =BRAP_FWIF_P_TaskType_eSlave;
            sCommand.uCommand.sStopTask.ui32MasterTaskId =hFwTask->uiMasterTaskId;            
        }        
        else
        {
            sCommand.uCommand.sStopTask.eTaskType =BRAP_FWIF_P_TaskType_eRealtime;
            sCommand.uCommand.sStopTask.ui32MasterTaskId =BRAP_FWIF_P_INVALID_TSK_ID;                
        }

        hFwTask->uiLastEventType = sCommand.sCommandHeader.ui32CommandID;
        BRAP_P_EventReset(hDsp,BRAP_GET_TASK_INDEX(hFwTask->uiTaskId));
        err = BRAP_FWIF_P_SendCommand(hDsp->hCmdQueue, &sCommand,hRap,hFwTask);            
        if(BERR_SUCCESS != err)
        {
            if(hRap->bWatchdogTriggered == false)
            {        
                BDBG_ERR(("BRAP_DSPCHN_P_Stop: STOP_TASK creation failed!"));

                err = BERR_TRACE(err);            
                goto freetask;    
                   
             }
            else
                err = BERR_SUCCESS;   
        }

        /* Wait for Ack_Response_Received event w/ timeout */
        err = BRAP_P_EventWait(hDsp, BRAP_DSPCHN_P_START_STOP_EVENT_TIMEOUT_IN_MS,BRAP_GET_TASK_INDEX(hFwTask->uiTaskId));
        if(BERR_TIMEOUT == err)
        {
            if(hRap->bWatchdogTriggered == false)
            {        
                /* Please note that, If the code reaches at this point then there is a potential Bug in Fw 
                code which needs to be debugged. However Watchdog is being triggered to recover the system*/
                BDBG_WRN(("BRAP_DSPCHN_P_Stop: STOP_TASK Timeout! Triggering watchdog"));
#if 0                
                BDBG_ASSERT(0);                
#endif
                BRAP_Write32(hDsp->hRegister, BCHP_AUD_DSP_INTH0_R5F_SET+ hDsp->ui32Offset,0x1);
                hRap->bWatchdogTriggered  = true;
#if 0                
                err = BERR_TRACE(err);            
                goto freetask;    
#endif                 
                err = BERR_SUCCESS;                     
             }
            else
                err = BERR_SUCCESS;             
        }
        
        eMsgType = BRAP_P_MsgType_eSyn;
        if(hRap->bWatchdogTriggered == false)
        {
            err = BRAP_FWIF_P_GetMsg(hFwTask->hSyncMsgQueue, (void *)&sRsp,eMsgType);
        }
        if(BERR_SUCCESS != err)
        {
            if(hRap->bWatchdogTriggered == false)
            {
                BDBG_ERR(("BRAP_DSPCHN_P_Stop: Unable to read ACK!"));
                err = BERR_TRACE(err);            
                goto freetask;                     
            }
            else
                err = BERR_SUCCESS;             
        }
        if((sRsp.sCommonAckResponseHeader.eStatus != BERR_SUCCESS)||
            (sRsp.sCommonAckResponseHeader.ui32ResponseID != BRAP_FWIF_P_STOP_TASK_RESPONSE_ID)||
            (sRsp.sCommonAckResponseHeader.ui32TaskID != hFwTask->uiTaskId))
        {
            if(hRap->bWatchdogTriggered == false)
            {
                BDBG_ERR(("BRAP_DSPCHN_P_Stop: STOP_TASK ACK not received successfully!"));
                err = BERR_TRACE(err);            
                goto freetask;                     
    
            }
            else
                err = BERR_SUCCESS;             
        }
        
        /*Mask the task */
        err = BRAP_P_MaskTask(hFwTask);
        if(BERR_SUCCESS != err)
        {
            BDBG_ERR(("BRAP_DSPCHN_P_Stop: Unable to mask task id interrupt!"));
            err = BERR_TRACE(err);            
            goto freetask;
        }
        /* Destroy synchronous and asynchronous queue of Task */
        hDsp->bFifoUsed[hFwTask->hSyncMsgQueue->ui32FifoId] = false;
        err = BRAP_FWIF_P_DestroyMsgQueue(hFwTask->hSyncMsgQueue,hDsp);
        if(BERR_SUCCESS != err)
        {
            BDBG_ERR(("BRAP_DSPCHN_P_Stop: SYNC queue destroy failed!"));
            err = BERR_TRACE(err);            
            goto freetask;            
        }

        hDsp->bFifoUsed[hFwTask->hAsyncMsgQueue->ui32FifoId] = false;
        err = BRAP_FWIF_P_DestroyMsgQueue(hFwTask->hAsyncMsgQueue,hDsp);
        if(BERR_SUCCESS != err)
        {
            BDBG_ERR(("BRAP_DSPCHN_P_Stop: ASYNC queue destroy failed!"));
            err = BERR_TRACE(err);
            goto freetask;            
        }

#ifdef RAP_MULTISTREAM_DECODER_SUPPORT        
        /*If Current task is Mixer(Master) of Multi Stream Decoder, then invalidate inter task DRAM address*/
        if(false == pDspChn->bChSpecificDspCh)
        {
            BRAP_ChannelHandle  hRapCh;

            hRapCh = BRAP_P_GetRapChHandle(pDspChn->uHandle.hAssociation);
            if(!(BRAP_P_IsPointerValid((void *)hRapCh)))
            {
                BDBG_ERR(("No Raptor channel found in Association handle"));
                err = BERR_INVALID_PARAMETER;
                goto freetask;
            }            
            
            if(BRAP_P_IsFwMixingPostLoopbackEnabled(hRapCh))
            {
                unsigned int    uiStgIp = 0;            
                for(j = 0; j < BRAP_AF_P_MAX_NODES; j++)
                {
                    if(hFwTask->sCitOutput.sCit.sNodeConfig[j].eAlgoId == BRAP_AF_P_AlgoId_eFWMixerPostProc)
                    {
                        for(uiStgIp=0; uiStgIp<BRAP_P_MAX_FW_STG_INPUTS; uiStgIp++)
                        {
                            hRapCh->hMultiStreamDecoder->ui32InterTaskIoBufferHandle[uiStgIp] = (uint32_t)NULL;
                            hRapCh->hMultiStreamDecoder->ui32InterTaskIoGenericBufferHandle[uiStgIp] = (uint32_t)NULL;
                        }
                        break;
                    }
                }
            }
        }
#endif

freetask:
        for(j=0;j<BRAP_MAX_FW_TSK_SUPPORTED;j++)
        {
            if(hRap->sMemInfo.sTskMemInfo[hDsp->uiDspIndex].sTskQueue[j].ui32TaskId == hFwTask->uiTaskId)
            {
                hRap->sMemInfo.sTskMemInfo[hDsp->uiDspIndex].sTskQueue[j].ui32TaskId = BRAP_FWIF_P_INVALID_TSK_ID;
                hFwTask->pAsyncMsgMemory = NULL;
                hFwTask->uiTaskId = BRAP_FWIF_P_INVALID_TSK_ID;
                hFwTask->bStopped = true;
                hFwTask->uiMasterTaskId= BRAP_FWIF_P_INVALID_TSK_ID;     
                hFwTask->bMaster= false;                       
           }
        }
        if(BERR_SUCCESS != err)
        {
            return err;
        }
    }

    
    pDspChn->eChState = BRAP_DSPCHN_P_ChState_eStop; 
    
    BDBG_LEAVE(BRAP_DSPCHN_P_Stop);
    return err;
}	

#ifdef RAP_MULTISTREAM_DECODER_SUPPORT
BERR_Code
BRAP_DSPCHN_P_AddRemoveInputToTask(
	BRAP_ChannelHandle 	    hRapCh,
    BRAP_DSPCHN_Handle      hDspCh,
    bool                    bAddInput,
    BRAP_P_IOBufferDetails  *psIOBuffer
)
{
    BERR_Code               ret = BERR_SUCCESS;
    unsigned int            i=0;
    BRAP_Handle             hRap = NULL;
    BRAP_DSP_Handle         hDsp = NULL;    
    BRAP_SIPS_OutputInfo	sSipsOp;
	BRAP_SIPS_InputInfo		*psSipsIp;    
    BRAP_FWIF_P_FwTaskHandle hFwTask = NULL;
    BRAP_FWIF_P_Command     sCommand;
    BRAP_P_MsgType          eMsgType;    
    BRAP_FWIF_P_Response    sRsp;
    
    BDBG_ASSERT(hRapCh);
    BDBG_ASSERT(hDspCh);    
    BDBG_ENTER(BRAP_DSPCHN_P_AddRemoveInputToTask);
    
    psSipsIp = (BRAP_SIPS_InputInfo *)BKNI_Malloc(sizeof(BRAP_SIPS_InputInfo));
    if (NULL==psSipsIp)
    {
        BDBG_ERR(("Out of System memory"));
        ret = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        goto exit;
    }    
    if(hDspCh->bChSpecificDspCh != false)
    {
        BDBG_ERR(("Dynamic reconfiguration of FW Task supported only for"
            " Association Network"));
        ret = BERR_NOT_SUPPORTED;
        goto exit;
    }

    for (i = 0; i < BRAP_FWIF_P_MAX_FW_TASK_PER_DSPCHN; i++)
    {
        hFwTask = hDspCh->sFwTaskInfo[i].hFwTask;
        if((BRAP_P_IsPointerValid((void *)hFwTask)) &&
           (hFwTask->bMaster == true))
           break;
    }
    if(i == BRAP_FWIF_P_MAX_FW_TASK_PER_DSPCHN)
    {
        BDBG_ERR(("BRAP_DSPCHN_P_AddRemoveInputToTask: No Master Task found"
                    " in the DSP Channel handle"));
        ret = BERR_INVALID_PARAMETER;
        goto exit;            
    }

    hDsp = hFwTask->hDsp;
    hRap = hDsp->hRap;

    psSipsIp->ui32CitDataStructureDramAddr = 
        hRap->sMemInfo.sTskMemInfo[hDsp->uiDspIndex].sCitStruct[BRAP_GET_TASK_INDEX(hFwTask->uiTaskId)].ui32BaseAddr;
    
    if(hRapCh->hMultiStreamDecoder != NULL)
    {
        psSipsIp->ui32WorkingCitDataStructureDramAddr = 
            hRapCh->hMultiStreamDecoder->ui32ReconfiguredCITAddress;
    } 
        
    if(bAddInput == true)
    {    
        BDBG_ASSERT(psIOBuffer);
        psSipsIp->eAddRemoveIpPort = BRAP_SIPS_P_AddRemoveIpPort_eAddPort;
        psSipsIp->sRdbCfg.ui32RdbBaseOffset = 0;
        psSipsIp->sRdbCfg.ui32RdbDspOffset = BCHP_PHYSICAL_OFFSET;
        psSipsIp->sInputSrcDetails.eType = BRAP_CIT_P_FwStgSrcDstType_eDRAMBuf;

        if(psIOBuffer->eBufferType == BRAP_P_IOBufferType_eFMM)
        {
            psSipsIp->sInputSrcDetails.eType = BRAP_CIT_P_FwStgSrcDstType_eFMMBuf;
            psSipsIp->sInputSrcDetails.uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32NumBuffers = 
                psIOBuffer->uBufferDetails.sIOBufId.uiNumBuffers;
            psSipsIp->sInputSrcDetails.uDetails.sIoBuf.uIoBuffer.sIoBuffId.eBufferType = BRAP_AF_P_BufferType_eFMM;
            for(i=0; i<psIOBuffer->uBufferDetails.sIOBufId.uiNumBuffers; i++)
            {
                psSipsIp->sInputSrcDetails.uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32BufferId[i] = 
                    psIOBuffer->uBufferDetails.sIOBufId.ui32RbufId[i];
            }
            psSipsIp->sInputSrcDetails.uDetails.sIoBuf.uIoGenericBuffer.sIoGenericBuffId.ui32NumBuffers = 0;
            psSipsIp->sInputSrcDetails.uDetails.sIoBuf.uIoGenericBuffer.sIoGenericBuffId.eBufferType = BRAP_AF_P_BufferType_eInvalid;
        }
        else if(psIOBuffer->eBufferType == BRAP_P_IOBufferType_eDRAM)
        {
            psSipsIp->sInputSrcDetails.eType = BRAP_CIT_P_FwStgSrcDstType_eDRAMBuf;
            psSipsIp->sInputSrcDetails.uDetails.sIoBuf.uIoBuffer.sIoBuffer.ui32NumBuffers = 
                psIOBuffer->uBufferDetails.sIOBuffer.uiNumBuffers;
            psSipsIp->sInputSrcDetails.uDetails.sIoBuf.uIoBuffer.sIoBuffer.eBufferType = BRAP_AF_P_BufferType_eDRAM;
            for(i=0; i<psIOBuffer->uBufferDetails.sIOBuffer.uiNumBuffers; i++)
            {
                psSipsIp->sInputSrcDetails.uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[i].ui32BaseAddr = 
                    psIOBuffer->uBufferDetails.sIOBuffer.sIOCircularBuf[i].ui32BaseAddr;
                psSipsIp->sInputSrcDetails.uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[i].ui32ReadAddr = 
                    psIOBuffer->uBufferDetails.sIOBuffer.sIOCircularBuf[i].ui32ReadAddr;
                psSipsIp->sInputSrcDetails.uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[i].ui32WriteAddr = 
                    psIOBuffer->uBufferDetails.sIOBuffer.sIOCircularBuf[i].ui32WriteAddr;
                psSipsIp->sInputSrcDetails.uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[i].ui32EndAddr = 
                    psIOBuffer->uBufferDetails.sIOBuffer.sIOCircularBuf[i].ui32EndAddr;
                psSipsIp->sInputSrcDetails.uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[i].ui32WrapAddr = 
                    psIOBuffer->uBufferDetails.sIOBuffer.sIOCircularBuf[i].ui32WrapAddr;
            } 
            
            psSipsIp->sInputSrcDetails.uDetails.sIoBuf.uIoGenericBuffer.sIoGenericBuffer.ui32NumBuffers = 1;
            psSipsIp->sInputSrcDetails.uDetails.sIoBuf.uIoGenericBuffer.sIoGenericBuffer.eBufferType = BRAP_AF_P_BufferType_eDRAM;
            psSipsIp->sInputSrcDetails.uDetails.sIoBuf.uIoGenericBuffer.sIoGenericBuffer.sCircBuffer.ui32BaseAddr = 
                psIOBuffer->uBufferDetails.sIOBuffer.sIOGenericCircularBuf.ui32BaseAddr;
            psSipsIp->sInputSrcDetails.uDetails.sIoBuf.uIoGenericBuffer.sIoGenericBuffer.sCircBuffer.ui32ReadAddr = 
                psIOBuffer->uBufferDetails.sIOBuffer.sIOGenericCircularBuf.ui32ReadAddr;
            psSipsIp->sInputSrcDetails.uDetails.sIoBuf.uIoGenericBuffer.sIoGenericBuffer.sCircBuffer.ui32WriteAddr = 
                psIOBuffer->uBufferDetails.sIOBuffer.sIOGenericCircularBuf.ui32WriteAddr;
            psSipsIp->sInputSrcDetails.uDetails.sIoBuf.uIoGenericBuffer.sIoGenericBuffer.sCircBuffer.ui32EndAddr = 
                psIOBuffer->uBufferDetails.sIOBuffer.sIOGenericCircularBuf.ui32EndAddr;
            psSipsIp->sInputSrcDetails.uDetails.sIoBuf.uIoGenericBuffer.sIoGenericBuffer.sCircBuffer.ui32WrapAddr = 
                psIOBuffer->uBufferDetails.sIOBuffer.sIOGenericCircularBuf.ui32WrapAddr;            
        }
    }
    else
    {
        psSipsIp->ui32InputPortIndexToRemove = hRapCh->uiFWMixerIpIndex;
        psSipsIp->eAddRemoveIpPort = BRAP_SIPS_P_AddRemoveIpPort_eRemovePort;        
    }         
    
    ret = BRAP_SIPS_P_SeamlessIpPortSwitchAPI(hRap->hHeap, psSipsIp, &sSipsOp);
    if(BERR_SUCCESS != ret)
    {
        BDBG_ERR(("BRAP_SIPS_P_SeamlessIpPortSwitchAPI returned error!"));
        ret =BERR_TRACE(ret);
        goto exit;
    }
    if(bAddInput == true)
        hRapCh->uiFWMixerIpIndex = sSipsOp.ui32InputPortIndexAdded;
        
    /*	Analyze the Reconfigured CIT : First level Information */
    BRAP_P_AnalyseCit(hRap->hHeap, (BRAP_AF_P_sTASK_CONFIG *)psSipsIp->ui32WorkingCitDataStructureDramAddr);

    /*Prepare command to stop the task */
    sCommand.sCommandHeader.ui32CommandID = BRAP_FWIF_P_CIT_RECONFIGURATION_COMMAND_ID;
    sCommand.sCommandHeader.ui32CommandCounter = 0;
    sCommand.sCommandHeader.ui32TaskID = hFwTask->uiTaskId;
    sCommand.sCommandHeader.eResponseType = BRAP_FWIF_P_ResponseType_eResponseRequired;
    ret = BRAP_P_ConvertAddressToOffset(hRap->hHeap, 
                (void *)(psSipsIp->ui32WorkingCitDataStructureDramAddr),
                &(sCommand.uCommand.sCitReconfigCommand.ui32ModifiedCitAddr));    
    if(BERR_SUCCESS != ret)
    {
        BDBG_ERR(("BRAP_P_ConvertAddressToOffset returned error"));
        ret = BERR_INVALID_PARAMETER;
        goto exit;
    }
    
    ret = BRAP_P_ConvertAddressToOffset(hRap->hHeap, 
            (void *)(hRap->sMemInfo.sTskMemInfo[hDsp->uiDspIndex].sCitStruct[BRAP_GET_TASK_INDEX(hFwTask->uiTaskId)].ui32BaseAddr),
            &(sCommand.uCommand.sCitReconfigCommand.ui32RunningTaskCitAddr));    
    if(BERR_SUCCESS != ret)
    {
        BDBG_ERR(("BRAP_P_ConvertAddressToOffset returned error"));
        ret = BERR_INVALID_PARAMETER;
        goto exit;
    }        
    sCommand.uCommand.sCitReconfigCommand.ui32SizeInBytes = 
        hRap->sMemInfo.sTskMemInfo[hDsp->uiDspIndex].sCitStruct[BRAP_GET_TASK_INDEX(hFwTask->uiTaskId)].ui32Size;
    
    hFwTask->uiLastEventType = sCommand.sCommandHeader.ui32CommandID;
    BRAP_P_EventReset(hDsp,BRAP_GET_TASK_INDEX(hFwTask->uiTaskId));
    ret = BRAP_FWIF_P_SendCommand(hDsp->hCmdQueue, &sCommand,hRap,hFwTask);        
    if(BERR_SUCCESS != ret)
    {
        if(hRap->bWatchdogTriggered == false)
        {        
            BDBG_ERR(("BRAP_DSPCHN_P_AddRemoveInputToTask: CIT_RECONFIGURATION failed!"));
            ret = BERR_TRACE(ret);
            BDBG_ASSERT(0);
            goto exit;
        }
        else
            ret = BERR_SUCCESS;              
    }
    /* Wait for Ack_Response_Received event w/ timeout */
    ret = BRAP_P_EventWait(hDsp, BRAP_DSPCHN_P_START_STOP_EVENT_TIMEOUT_IN_MS,
                            BRAP_GET_TASK_INDEX(hFwTask->uiTaskId));
    if(BERR_TIMEOUT == ret)
    {
        if((hRap->bWatchdogTriggered == false))
        {
            /* Please note that, If the code reaches at this point then 
            there is a potential Bug in Fw code which needs to be debugged. 
            However Watchdog is being triggered to recover the system */            
            BDBG_WRN(("BRAP_DSPCHN_P_AddRemoveInputToTask: CIT_RECONFIGURATION ACK timeout!"
                        " Triggering Watchdog"));
#if 0                
            BDBG_ASSERT(0);                
#endif
            BRAP_Write32(hDsp->hRegister, BCHP_AUD_DSP_INTH0_R5F_SET+ hDsp->ui32Offset,0x1);
            hRap->bWatchdogTriggered  = true;
#if 0                
            ret = BERR_TRACE(ret);
            goto exit;
#endif                 
            ret = BERR_SUCCESS;              
        }
        else
            ret = BERR_SUCCESS;              
    }

/* Send command for the task , Only if the ack for the Start of the task is recieved */
    eMsgType = BRAP_P_MsgType_eSyn;
    if(hRap->bWatchdogTriggered == false)
    {
        ret = BRAP_FWIF_P_GetMsg(hFwTask->hSyncMsgQueue, (void *)&sRsp,eMsgType);
    }
    if(BERR_SUCCESS != ret)
    {
        if(hRap->bWatchdogTriggered == false)
        {
            BDBG_ERR(("BRAP_DSPCHN_P_AddRemoveInputToTask: Unable to read ACK!"));
            ret = BERR_TRACE(ret);
            goto exit;
        }
        else
            ret = BERR_SUCCESS;              
    }

    if((sRsp.sCommonAckResponseHeader.eStatus != BERR_SUCCESS)||
       (sRsp.sCommonAckResponseHeader.ui32ResponseID != BRAP_FWIF_P_CIT_RECONFIGURATION_RESPONSE_ID)||
       (sRsp.sCommonAckResponseHeader.ui32TaskID != hFwTask->uiTaskId))
    {
        if((hRap->bWatchdogTriggered == false))
        {
           BDBG_ERR(("BRAP_DSPCHN_P_AddRemoveInputToTask: CIT_RECONFIGURATION ACK"
            " not received successfully!eStatus = %d , ui32ResponseID = %d , ui32TaskID %d ",
                sRsp.sCommonAckResponseHeader.eStatus,sRsp.sCommonAckResponseHeader.ui32ResponseID,sRsp.sCommonAckResponseHeader.ui32TaskID));
            ret = BERR_TRACE(sRsp.sCommonAckResponseHeader.eStatus);
            goto exit;
        }
        else
            ret = BERR_SUCCESS;              
    } 

exit:    
    if(psSipsIp)
        BKNI_Free(psSipsIp);
    BDBG_LEAVE(BRAP_DSPCHN_P_AddRemoveInputToTask);
    return ret;
}
#endif

void
BRAP_DSPCHN_P_TsmLog_isr(
	BRAP_DSPCHN_Handle	hDspCh,			/* [in] DSP channel handle */
	BRAP_DSPCHN_TsmLogInfo	*psTsmLogInfo)	/* [out] Information about logged TSM data */
{
       BSTD_UNUSED(hDspCh);
       BSTD_UNUSED(psTsmLogInfo);
	return ;
}	

BERR_Code
BRAP_DSPCHN_P_EnablePause(
						BRAP_DSPCHN_Handle      hDspCh,	/* [in] DSPCHN handle */
						bool					bOnOff		/* [In] TRUE = Pause video
																FALSE = Resume video */
						)
{
    BRAP_FWIF_P_FwTaskHandle    hFwTask;
    BRAP_P_MsgType      eMsgType;
    BRAP_FWIF_P_Command sCommand;
    BRAP_FWIF_P_Response sRsp;
    BRAP_Handle hRap = NULL;   
    BERR_Code ret = BERR_SUCCESS;    
    unsigned int i;
    bool bDecoderTaskPresent=false;

    BDBG_ENTER(BRAP_DSPCHN_P_EnablePause);
    /* Assert on input parameters */
    BDBG_ASSERT(hDspCh);

    BKNI_Memset((void *)&sRsp,0,sizeof(BRAP_FWIF_P_Response));

    if(true == hDspCh->bChSpecificDspCh)
    {
        hRap = hDspCh->uHandle.hRapCh->hRap;
    }
    else
    {
        hRap = hDspCh->uHandle.hAssociation->hRap;
    }
    for(i = 0; i< BRAP_FWIF_P_MAX_FW_TASK_PER_DSPCHN ; i++)
    {
        hFwTask = hDspCh->sFwTaskInfo[i].hFwTask;
        if(BRAP_P_IsPointerValid((void *)hFwTask))
        {
            if(true == hDspCh->sFwTaskInfo[i].sProcessingNw.sAudProcessingStage[0][0].bDecoderStage) /* Send pause command to the task having decode stage*/
            {
                bDecoderTaskPresent = true;
                if (bOnOff)
                { /* Enable pause */
                    /* Create Pause command */
                    sCommand.sCommandHeader.ui32CommandID = BRAP_FWIF_P_PAUSE_COMMAND_ID;
                    sCommand.sCommandHeader.ui32CommandCounter =  hRap->uiCommandCounter++;
                    sCommand.sCommandHeader.ui32TaskID =  hFwTask->uiTaskId;
                    sCommand.sCommandHeader.eResponseType = BRAP_FWIF_P_ResponseType_eAckRequired;   

                    hFwTask->uiLastEventType = sCommand.sCommandHeader.ui32CommandID;                    
                    BRAP_P_EventReset(hFwTask->hDsp,BRAP_GET_TASK_INDEX(hFwTask->uiTaskId));
                    ret = BRAP_FWIF_P_SendCommand(hFwTask->hDsp->hCmdQueue, &sCommand,hRap,hFwTask);
                    if(BERR_SUCCESS != ret)
                    {
                        if((hRap->bWatchdogTriggered == false)
                        &&(hFwTask->bStopped == false))
                        {
                            BDBG_ERR(("BRAP_DSPCHN_P_EnablePause: Pause Command failed!"));
                            ret = BERR_TRACE(ret);
                            goto exit;
                        }
                        else
                            ret = BERR_SUCCESS;    
                    }
                    /* Wait for Ack_Response_Received event w/ timeout */
                    ret = BRAP_P_EventWait(hFwTask->hDsp, BRAP_DSPCHN_P_EVENT_TIMEOUT_IN_MS,BRAP_GET_TASK_INDEX(hFwTask->uiTaskId));
                    if(BERR_TIMEOUT == ret)
                    {
                        if((hRap->bWatchdogTriggered == false))
                        {
                            /* Please note that, If the code reaches at this point then there is a potential Bug in Fw 
                            code which needs to be debugged. However Watchdog is being triggered to recover the system*/            
                            BDBG_WRN(("BRAP_DSPCHN_P_EnablePause: Pause Command ACK timeout! Triggering Watchdog"));
#if 0                
                            BDBG_ASSERT(0);                
#endif
                            BRAP_Write32(hFwTask->hDsp->hRegister, BCHP_AUD_DSP_INTH0_R5F_SET+ hFwTask->hDsp->ui32Offset,0x1);
                            hRap->bWatchdogTriggered  = true;
#if 0                
                            ret = BERR_TRACE(err);
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
                            BDBG_ERR(("BRAP_DSPCHN_P_EnablePause: Unable to read ACK!"));
                            ret = BERR_TRACE(ret);
                            goto exit;
                        }
                        else
                            ret = BERR_SUCCESS;                     
                    }

                    if((sRsp.sCommonAckResponseHeader.eStatus != BERR_SUCCESS)||
                    (sRsp.sCommonAckResponseHeader.ui32ResponseID != BRAP_FWIF_P_PAUSE_ACK_ID)||
                    (sRsp.sCommonAckResponseHeader.ui32TaskID != hFwTask->uiTaskId))
                    {
                        if((hRap->bWatchdogTriggered == false)
                        &&(hFwTask->bStopped == false))
                        {
                            BDBG_ERR(("sRsp.sCommonAckResponseHeader.eStatus =%d",sRsp.sCommonAckResponseHeader.eStatus));
                            BDBG_ERR(("BRAP_DSPCHN_P_EnablePause: Pause Command response not received successfully!"));
                            ret = BERR_TRACE(sRsp.sCommonAckResponseHeader.eStatus);
                            goto exit;                    
                        }
                        else
                            ret = BERR_SUCCESS;                                         
                    }
                }
                else
                {
                    /* Create Resume command */                
                    sCommand.sCommandHeader.ui32CommandID = BRAP_FWIF_P_RESUME_COMMAND_ID;
                    sCommand.sCommandHeader.ui32CommandCounter =  hRap->uiCommandCounter++;
                    sCommand.sCommandHeader.ui32TaskID =  hFwTask->uiTaskId;
                    sCommand.sCommandHeader.eResponseType = BRAP_FWIF_P_ResponseType_eAckRequired;   

                    hFwTask->uiLastEventType = sCommand.sCommandHeader.ui32CommandID;                    
                    BRAP_P_EventReset(hFwTask->hDsp,BRAP_GET_TASK_INDEX(hFwTask->uiTaskId));
                    ret = BRAP_FWIF_P_SendCommand(hFwTask->hDsp->hCmdQueue, &sCommand,hRap,hFwTask);
                    if(BERR_SUCCESS != ret)
                    {
                        if((hRap->bWatchdogTriggered == false)
                        &&(hFwTask->bStopped == false))
                        {
                            BDBG_ERR(("BRAP_DSPCHN_P_EnablePause: Resume Command failed!"));
                            ret = BERR_TRACE(ret);
                            goto exit;
                        }
                        else
                            ret = BERR_SUCCESS;                       
                    }
                    /* Wait for Ack_Response_Received event w/ timeout */
                    ret = BRAP_P_EventWait(hFwTask->hDsp, BRAP_DSPCHN_P_EVENT_TIMEOUT_IN_MS,BRAP_GET_TASK_INDEX(hFwTask->uiTaskId));
                    if(BERR_TIMEOUT == ret)
                    {
                        if((hRap->bWatchdogTriggered == false))
                        {
                            /* Please note that, If the code reaches at this point then there is a potential Bug in Fw 
                            code which needs to be debugged. However Watchdog is being triggered to recover the system*/            
                            BDBG_WRN(("BRAP_DSPCHN_P_EnablePause: Resume Command ACK timeout! Triggering Watchdog"));
#if 0                
                            BDBG_ASSERT(0);                
#endif
                            BRAP_Write32(hFwTask->hDsp->hRegister, BCHP_AUD_DSP_INTH0_R5F_SET+ hFwTask->hDsp->ui32Offset,0x1);
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
                            BDBG_ERR(("BRAP_DSPCHN_P_EnablePause: Unable to read ACK!"));
                            ret = BERR_TRACE(ret);
                            goto exit;
                        }
                        else
                            ret = BERR_SUCCESS;                                             
                    }

                    if((sRsp.sCommonAckResponseHeader.eStatus != BERR_SUCCESS)||
                    (sRsp.sCommonAckResponseHeader.ui32ResponseID != BRAP_FWIF_P_RESUME_ACK_ID)||
                    (sRsp.sCommonAckResponseHeader.ui32ResponseCounter!=sCommand.sCommandHeader.ui32CommandCounter)||
                    (sRsp.sCommonAckResponseHeader.ui32TaskID != hFwTask->uiTaskId))
                    {
                        if((hRap->bWatchdogTriggered == false)
                            &&(hFwTask->bStopped == false))
                        {
                            BDBG_ERR(("sRsp.sCommonAckResponseHeader.eStatus =%d",sRsp.sCommonAckResponseHeader.eStatus));
                            BDBG_ERR(("BRAP_DSPCHN_P_EnablePause: Resume Command response not received successfully!"));
                            ret =  BERR_TRACE(sRsp.sCommonAckResponseHeader.eStatus);
                            goto exit;
                        }
                        else
                            ret = BERR_SUCCESS;                                                                 
                    }
                }
            }
        }
    }
    if(false == bDecoderTaskPresent)
    {
        BDBG_ERR(("Error!!! No task of the DspCh has decoder stage in it"));
        ret = BERR_TRACE(BERR_NOT_SUPPORTED);
    }

exit:    
    BDBG_ENTER(BRAP_DSPCHN_P_EnablePause);    
    return ret;
}	


BERR_Code
BRAP_DSPCHN_P_GetOneAudioFrameTime(
						BRAP_DSPCHN_Handle      hDspChn,			/* [in] DSPCHN handle */
						unsigned int			*puiOneFrameTime	/* [out] One Audio Frame Time */
						)
{
       BSTD_UNUSED(hDspChn);
       BSTD_UNUSED(puiOneFrameTime);
	return BERR_SUCCESS;
}	


BERR_Code
BRAP_DSPCHN_P_FrameAdvance(
						BRAP_DSPCHN_Handle      hDspCh,			/* [in] DSPCHN handle */
						unsigned int			uiFrameAdvTime				/* [In] Frame advance time in msec */
						)
{
{
    BRAP_FWIF_P_FwTaskHandle    hFwTask;
    BRAP_P_MsgType      eMsgType;
    BRAP_FWIF_P_Command sCommand;
    BRAP_FWIF_P_Response sRsp;
    BRAP_Handle hRap = NULL;   
    BERR_Code ret = BERR_SUCCESS;    
    unsigned int i;
    bool bDecoderTaskPresent = false;

    BDBG_ENTER(BRAP_DSPCHN_P_FrameAdvance);
    /* Assert on input parameters */
    BDBG_ASSERT(hDspCh);

    BKNI_Memset((void *)&sRsp,0,sizeof(BRAP_FWIF_P_Response));

    if(true == hDspCh->bChSpecificDspCh)
    {
        hRap = hDspCh->uHandle.hRapCh->hRap;
    }
    else
    {
        hRap = hDspCh->uHandle.hAssociation->hRap;
    }
    for(i = 0; i< BRAP_FWIF_P_MAX_FW_TASK_PER_DSPCHN ; i++)
    {
        hFwTask = hDspCh->sFwTaskInfo[i].hFwTask;
        if(BRAP_P_IsPointerValid((void *)hFwTask))
        {
            if(true == hDspCh->sFwTaskInfo[i].sProcessingNw.sAudProcessingStage[0][0].bDecoderStage) /* Send pause command to the task having decode stage*/
            {
                    bDecoderTaskPresent = true;
                    /* Create Frame Advance command */
                    sCommand.sCommandHeader.ui32CommandID = BRAP_FWIF_P_FRAME_ADVANCE_COMMAND_ID;
                    sCommand.sCommandHeader.ui32CommandCounter =  hRap->uiCommandCounter++;
                    sCommand.sCommandHeader.ui32TaskID =  hFwTask->uiTaskId;
                    sCommand.sCommandHeader.eResponseType = BRAP_FWIF_P_ResponseType_eAckRequired;   
                    sCommand.uCommand.sFrameAdvance.ui32DurationOfFrameAdv = uiFrameAdvTime;

                    hFwTask->uiLastEventType = sCommand.sCommandHeader.ui32CommandID;                    
                    BRAP_P_EventReset(hFwTask->hDsp,BRAP_GET_TASK_INDEX(hFwTask->uiTaskId));
                    ret = BRAP_FWIF_P_SendCommand(hFwTask->hDsp->hCmdQueue, &sCommand,hRap,hFwTask);
                    if(BERR_SUCCESS != ret)
                    {
                        if((hRap->bWatchdogTriggered == false)
                        &&(hFwTask->bStopped == false))
                        {
                            BDBG_ERR(("BRAP_DSPCHN_P_FrameAdvance: Frame advance Command failed!"));
                            ret = BERR_TRACE(ret);
                            goto exit;
                        }
                        else
                            ret = BERR_SUCCESS;   
                    }
                    /* Wait for Ack_Response_Received event w/ timeout */
                    ret = BRAP_P_EventWait(hFwTask->hDsp, BRAP_DSPCHN_P_EVENT_TIMEOUT_IN_MS,BRAP_GET_TASK_INDEX(hFwTask->uiTaskId));
                    if(BERR_TIMEOUT == ret)
                    {
                        if((hRap->bWatchdogTriggered == false))
                        {
                            /* Please note that, If the code reaches at this point then there is a potential Bug in Fw 
                            code which needs to be debugged. However Watchdog is being triggered to recover the system*/            
                            BDBG_WRN(("BRAP_DSPCHN_P_FrameAdvance: Frame advance Command ACK timeout! Triggering Watchdog"));
#if 0                
                            BDBG_ASSERT(0);                
#endif
                            BRAP_Write32(hFwTask->hDsp->hRegister, BCHP_AUD_DSP_INTH0_R5F_SET+ hFwTask->hDsp->ui32Offset,0x1);
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
                            BDBG_ERR(("BRAP_DSPCHN_P_FrameAdvance: Unable to read response for frame advance!"));
                            ret = BERR_TRACE(ret);
                            goto exit;
                        }
                        else
                            ret = BERR_SUCCESS;                                             
                    }

                    if((sRsp.sCommonAckResponseHeader.eStatus != BERR_SUCCESS)||
                    (sRsp.sCommonAckResponseHeader.ui32ResponseID != BRAP_FWIF_P_FRAME_ADVANCE_ACK_ID)||
                    (sRsp.sCommonAckResponseHeader.ui32TaskID != hFwTask->uiTaskId))
                    {
                        if((hRap->bWatchdogTriggered == false)
                            &&(hFwTask->bStopped == false))
                        {
                            BDBG_ERR(("sRsp.sCommonAckResponseHeader.eStatus =%d",sRsp.sCommonAckResponseHeader.eStatus));
                            BDBG_ERR(("BRAP_DSPCHN_P_FrameAdvance: Frame advance Command response not received successfully!"));
                            ret = BERR_TRACE(sRsp.sCommonAckResponseHeader.eStatus);
                            goto exit;                    
                        }
                        else
                            ret = BERR_SUCCESS;                              
                    }
            }
        }
    }
    if(false == bDecoderTaskPresent)
    {
        BDBG_ERR(("Error!!! No task of the DspCh has decoder stage in it"));
        ret = BERR_TRACE(BERR_NOT_SUPPORTED);
    }
exit:    
    BDBG_ENTER(BRAP_DSPCHN_P_FrameAdvance);    
    return ret;
}
}	


void BRAP_DSP_P_GetFwVersionInfo(
		BRAP_DSP_Handle hDsp,
		BRAP_DSPVersionInfo	*psDSPVersion
		)
{
       BSTD_UNUSED(hDsp);
       BSTD_UNUSED(psDSPVersion);
	return;
}	

BERR_Code BRAP_DSPCHN_P_GetCurrentAudioParams (
						BRAP_DSPCHN_Handle	hDspCh,
						BRAP_DSPCHN_P_AudioParams	*psDspChAudioParams
						)
{
	BDBG_ASSERT(hDspCh);
	BDBG_ASSERT(psDspChAudioParams);

	*psDspChAudioParams = hDspCh->sDspAudioParams;
	return BERR_SUCCESS;
}	


bool BRAP_DSPCHN_P_GetDecoderPauseState (
						BRAP_DSPCHN_Handle	hDspCh
						)
{
       BSTD_UNUSED(hDspCh);
	return true;
}	

BERR_Code
BRAP_DSPCHN_P_SetConfig (
	BRAP_DSPCHN_Handle	hDspCh,
	BRAP_DEC_DownmixPath	eDownmixPath,
	BRAP_DSPCHN_AudioType   eType
	)
{
    BERR_Code err = BERR_SUCCESS;
    unsigned int uiTaskIndex = 0, uiBranchId = 0, uiStageId = 0, uiTaskId;
    unsigned int uiStartNodeIndex, uiConfigBufAddr;
    bool bDecStageFound = false;
    BRAP_Fwif_P_FwBufInfo  *psFwBufInfo;
    BRAP_FWIF_P_Command sCommand;
    BRAP_FWIF_P_Response sRsp;
    BRAP_Handle hRap;
    BRAP_DSP_Handle hDsp;
    BRAP_CIT_P_OpStgInfo *psOpStgInfo;
    BRAP_FWIF_P_FwTaskHandle hFwTask;
    BRAP_P_MsgType      eMsgType;
    BRAP_P_ChannelAudioProcessingStage  sTempAudProcStage;
    unsigned int uiTempBranchId = 0;
    bool bBranchCloned = false,bIsFwBranch =false;
    unsigned int uiActualConfigSize = 0;
    
    BDBG_ENTER(BRAP_DSPCHN_P_SetConfig);

    BSTD_UNUSED(eDownmixPath);
    BDBG_ASSERT(hDspCh);

    BKNI_Memset((void *)&sRsp,0,sizeof(BRAP_FWIF_P_Response));

        /* Program configuration parameters in extra buffer */
    if(hDspCh->bChSpecificDspCh == true)
    {
        hRap = hDspCh->uHandle.hRapCh->hRap;
    }
    else
    {
        hRap = hDspCh->uHandle.hAssociation->hRap;
        BDBG_ERR(("This function is not supported till now for association specific DspCh"));
        BDBG_ASSERT(0);
    }

    /* Find out the task in which the decode stage is getting executed.
         Also find out the exact position of the decode stage within audio
         processing network. */
    for (uiTaskIndex = 0; uiTaskIndex < BRAP_FWIF_P_MAX_FW_TASK_PER_DSPCHN; uiTaskIndex++)
    {
        uiTempBranchId = 0;
        for (uiBranchId = 0; uiBranchId < BRAP_P_MAX_DST_PER_RAPCH; uiBranchId++)
        {
            bIsFwBranch = false;
            for (uiStageId = 0; uiStageId < BRAP_MAX_STAGE_PER_BRANCH_SUPPORTED; uiStageId++)
            {
                sTempAudProcStage = hDspCh->sFwTaskInfo[uiTaskIndex].sProcessingNw.sAudProcessingStage[uiBranchId][uiStageId];
                if (true== sTempAudProcStage.bDecoderStage)
                {
                    bDecStageFound = true;
                    break;
                }
                else if(((false == sTempAudProcStage.bCloneStage) && (NULL == sTempAudProcStage.hAudioProcessing)
                                && (true == hDspCh->sFwTaskInfo[uiTaskIndex].sProcessingNw.sAudProcessingStage[uiBranchId][uiStageId -1].bCloneStage))
                            ||((uiStageId == BRAP_MAX_STAGE_PER_BRANCH_SUPPORTED -1) && (true == sTempAudProcStage.bCloneStage)) /* Check if the last valid stage of the branch has bCloneStage == true. In that case output port cloning is happening.*/
                          ) 
                {
                    if(uiStageId == BRAP_MAX_STAGE_PER_BRANCH_SUPPORTED -1) /*If stageId is equal to MAX Stage ID*/
                    {
                        BRAP_FWIF_P_IsBranchCloned(hDspCh->uHandle.hRapCh,&(hDspCh->sFwTaskInfo[uiTaskIndex].sProcessingNw),uiBranchId,uiStageId,&bBranchCloned,&bIsFwBranch);                
                    }
                    else
                    {
                        BRAP_FWIF_P_IsBranchCloned(hDspCh->uHandle.hRapCh, &(hDspCh->sFwTaskInfo[uiTaskIndex].sProcessingNw),uiBranchId,uiStageId-1,&bBranchCloned,&bIsFwBranch);                
                    }                        
                    break;  /* Break from the FOR loop beacause last valid stage of the Branch */
                }
            }
            if (true==bDecStageFound) break;
            
            if(true == bIsFwBranch)
            {
                uiTempBranchId++;
            }
        }
        if (true==bDecStageFound) break;
    }

    if (false==bDecStageFound)
    {
        BDBG_ERR(("Could not find decoder stage in the network of audio processing stages."));
        BDBG_ASSERT(0);
        return BERR_TRACE(BRAP_ERR_BAD_DEVICE_STATE);
    }

    if(uiTempBranchId >= BRAP_P_MAX_FW_BRANCH_PER_FW_TASK)
    {
        BDBG_ERR(("Branch id (%d) is exceeding the BRAP_P_MAX_FW_BRANCH_PER_FW_TASK =%d",uiTempBranchId ,BRAP_P_MAX_FW_BRANCH_PER_FW_TASK));
        BDBG_ASSERT(0);
        return BERR_TRACE(BRAP_ERR_BAD_DEVICE_STATE);
    }
    /* Program configuration parameters in extra buffer */


    
    hDsp = hDspCh->sFwTaskInfo[uiTaskIndex].hFwTask->hDsp;
    psFwBufInfo = &(hRap->sMemInfo.sConfigParamBufInfo);
    hFwTask = hDspCh->sFwTaskInfo[uiTaskIndex].hFwTask;
    uiTaskId = hFwTask->uiTaskId;

    psOpStgInfo = &(hFwTask->sCitOutput.sCitBranchInfo[uiTempBranchId].sCitStgInfo[uiStageId]);
    uiStartNodeIndex = psOpStgInfo->ui32StartNodeIndex;
    uiConfigBufAddr = psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32UserParamBufAdr;


  err = BRAP_FWIF_P_SetDecodeStageConfigParams(
        hDspCh->uHandle.hRapCh,
        eType,
        hDspCh->sDspAudioParams.sExtAudioParams.eDecodeMode,
        psFwBufInfo ->ui32BaseAddr,
        psFwBufInfo ->ui32Size,&uiActualConfigSize);
     if (BERR_SUCCESS!=err)
    {
        return BERR_TRACE(err);
    }

	BDBG_MSG(("uiActualConfigSize =%d , psFwBufInfo->ui32Size =%d",uiActualConfigSize,psFwBufInfo->ui32Size));
    /* Create Change-Config command */
    sCommand.sCommandHeader.ui32CommandID = BRAP_FWIF_P_ALGO_PARAMS_CFG_COMMAND_ID;
    sCommand.sCommandHeader.ui32CommandCounter = hRap->uiCommandCounter++;
    sCommand.sCommandHeader.ui32TaskID = uiTaskId;
    sCommand.sCommandHeader.eResponseType = BRAP_FWIF_P_ResponseType_eResponseRequired;
    BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(psFwBufInfo ->ui32BaseAddr), 
        &(sCommand.uCommand.sCfgChange.ui32HostConfigParamBuffAddr));	
    BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(uiConfigBufAddr), 
        &(sCommand.uCommand.sCfgChange.ui32DspConfigParamBuffAddr));	
    sCommand.uCommand.sCfgChange.ui32SizeInBytes = uiActualConfigSize;

    hFwTask->uiLastEventType = sCommand.sCommandHeader.ui32CommandID;
    BRAP_P_EventReset(hDsp,BRAP_GET_TASK_INDEX(hFwTask->uiTaskId));
    err = BRAP_FWIF_P_SendCommand(hDsp->hCmdQueue, &sCommand,hRap,hFwTask);
    if(BERR_SUCCESS != err)
    {
        if((hRap->bWatchdogTriggered == false)
        &&(hFwTask->bStopped == false))
        {
            BDBG_ERR(("BRAP_DSPCHN_P_SetConfig: CFG_Command failed!"));
            return BERR_TRACE(err);
        }
        else
            err = BERR_SUCCESS;     
    }
    /* Wait for Ack_Response_Received event w/ timeout */
    err = BRAP_P_EventWait(hDsp, BRAP_DSPCHN_P_EVENT_TIMEOUT_IN_MS,BRAP_GET_TASK_INDEX(hFwTask->uiTaskId));
    if(BERR_TIMEOUT == err)
    {
        if((hRap->bWatchdogTriggered == false))
        {
            /* Please note that, If the code reaches at this point then there is a potential Bug in Fw 
            code which needs to be debugged. However Watchdog is being triggered to recover the system*/            
            BDBG_WRN(("BRAP_DSPCHN_P_SetConfig: CFG_Command ACK timeout! Triggering Watchdog"));
#if 0                
            BDBG_ASSERT(0);                
#endif
            BRAP_Write32(hDsp->hRegister, BCHP_AUD_DSP_INTH0_R5F_SET+ hDsp->ui32Offset,0x1);
            hRap->bWatchdogTriggered  = true;
#if 0                
            err = BERR_TRACE(err);
            goto error;
#endif                 
            err = BERR_SUCCESS;              
        }
        else
            err = BERR_SUCCESS;              
    }
        
    eMsgType = BRAP_P_MsgType_eSyn;
    if((hRap->bWatchdogTriggered == false)
    &&(hFwTask->bStopped == false))
    {
        err = BRAP_FWIF_P_GetMsg(hFwTask->hSyncMsgQueue, (void *)&sRsp, eMsgType);
    }
    if(BERR_SUCCESS != err)
    {
        if((hRap->bWatchdogTriggered == false)
            &&(hFwTask->bStopped == false))
        {
            BDBG_ERR(("BRAP_DSPCHN_P_SetConfig: Unable to read ACK!"));
            return BERR_TRACE(err);
        }
        else
            err = BERR_SUCCESS;      
    }
    if((sRsp.sCommonAckResponseHeader.eStatus != BERR_SUCCESS)||
        (sRsp.sCommonAckResponseHeader.ui32ResponseID != BRAP_FWIF_P_ALGO_PARAMS_CFG_RESPONSE_ID)||
        (sRsp.sCommonAckResponseHeader.ui32ResponseCounter!=sCommand.sCommandHeader.ui32CommandCounter)||
        (sRsp.sCommonAckResponseHeader.ui32TaskID != hFwTask->uiTaskId))
    {
        if((hRap->bWatchdogTriggered == false)
            &&(hFwTask->bStopped == false))
        {
            BDBG_ERR(("BRAP_DSPCHN_P_SetConfig: CFG_Command response not received successfully!"));
            return BERR_TRACE(sRsp.sCommonAckResponseHeader.eStatus);
        }
        else
            err = BERR_SUCCESS;     
    }
    BDBG_LEAVE(BRAP_DSPCHN_P_SetConfig);
    return err;
}	


BERR_Code BRAP_DSP_P_EnableDspWatchdogTimer (
						BRAP_DSP_Handle		hDsp,
						bool				bEnable
						)
{
	BERR_Code err = BERR_SUCCESS;
	uint32_t regVal = 0;
	BDBG_ASSERT(hDsp);

	if (bEnable) {
		/* Disable auto reload of count */
		regVal = regVal & ~(BCHP_MASK(AUD_DSP_CTRL0_WATCHDOG_TIMER, CM));
		/* Enable timer bit */
		regVal = regVal | BCHP_MASK(AUD_DSP_CTRL0_WATCHDOG_TIMER, ET);
		/* Program default watchdog count */
		regVal = (regVal & ~(BCHP_MASK(AUD_DSP_CTRL0_WATCHDOG_TIMER, COUNT)))
				| BCHP_FIELD_DATA(AUD_DSP_CTRL0_WATCHDOG_TIMER, COUNT, BRAP_DSP_P_DEFAULT_WATCHDOG_COUNT);
	}
	else {
		/* Disable timer bit */
		regVal = regVal & ~(BCHP_MASK(AUD_DSP_CTRL0_WATCHDOG_TIMER, ET));
	}
	
	BRAP_Write32(hDsp->hRegister, BCHP_AUD_DSP_CTRL0_WATCHDOG_TIMER + hDsp->ui32Offset, regVal);
	return err;
}	

void BRAP_DSP_P_WatchDogResetHardware(BRAP_DSP_Handle hDsp)
{
	uint32_t regVal = 0;

	/* Reset DSP processor and its peripherals */
	regVal = BRAP_Read32(hDsp->hRegister,BCHP_AUD_DSP_MISC_SOFT_RESET + hDsp->ui32Offset);
    
	regVal = (regVal & ~(BCHP_MASK(AUD_DSP_MISC_SOFT_RESET,RESET_SCBDMA0_B)))|
				(BCHP_FIELD_DATA(AUD_DSP_MISC_SOFT_RESET,RESET_SCBDMA0_B,0));
	regVal = (regVal & ~(BCHP_MASK(AUD_DSP_MISC_SOFT_RESET,RESET_PROC0_B)))|
				(BCHP_FIELD_DATA(AUD_DSP_MISC_SOFT_RESET,RESET_PROC0_B,0));
	regVal = (regVal & ~(BCHP_MASK(AUD_DSP_MISC_SOFT_RESET,RESET0_B)))|
				(BCHP_FIELD_DATA(AUD_DSP_MISC_SOFT_RESET,RESET0_B,0));
	BRAP_Write32(hDsp->hRegister,BCHP_AUD_DSP_MISC_SOFT_RESET + hDsp->ui32Offset, regVal);

	/* Withdraw only RESET0_B reset */
	regVal = (regVal & ~(BCHP_MASK(AUD_DSP_MISC_SOFT_RESET,RESET0_B)))|
				(BCHP_FIELD_DATA(AUD_DSP_MISC_SOFT_RESET,RESET0_B,1));
	BRAP_Write32(hDsp->hRegister,BCHP_AUD_DSP_MISC_SOFT_RESET + hDsp->ui32Offset, regVal);
	BDBG_MSG(("\n**** Soft reset: reset0_b withdrawn = 0x%x",regVal));
}



/***************************************************************************
Summary:
    This private routine returns the DSPCH handle associated with the
    decode channel handle.
**************************************************************************/
BRAP_DSPCHN_Handle 
BRAP_DSPCHN_P_GetDspChHandle(
	void	                *pHandle,       /* [in] Raptor Channel or Association handle */
	bool                    bChSpecificDspCh
	)
{
    unsigned int uiPth = 0;
    BRAP_ChannelHandle      hRapCh;    
        
    if(bChSpecificDspCh == true)
    {
        hRapCh = *(BRAP_ChannelHandle *)pHandle;
        BDBG_ASSERT(hRapCh);
        
        for(uiPth=0; uiPth < BRAP_P_MAX_PATHS_IN_A_CHAN; uiPth++)
        {
            if((BRAP_P_IsPointerValid((void *)hRapCh->pPath[uiPth])) &&
           ((hRapCh->pPath[uiPth]->eUsgPath == BRAP_P_UsgPath_eDecodePcm)||
            (hRapCh->pPath[uiPth]->eUsgPath == BRAP_P_UsgPath_eDecodeCompress)
#ifdef RAP_GFX_SUPPORT            
            ||(hRapCh->pPath[uiPth]->eUsgPath == BRAP_P_UsgPath_eGfx)
#endif            
#ifdef RAP_SCM_SUPPORT            
    			||(hRapCh->pPath[uiPth]->eUsgPath == BRAP_P_UsgPath_eScm)
#endif   
            )&&           
               ((BRAP_P_IsPointerValid((void *)hRapCh->pPath[uiPth]->hDspCh)))
#if (BRAP_3548_FAMILY != 1) /* Due to SPDIF/HDMI In having DSP only after ProcessDigital Change occurs */           
               && (BRAP_P_IsPointerValid((void *)hRapCh->pPath[uiPth]->hDsp))
#endif 
           )
        {

            return hRapCh->pPath[uiPth]->hDspCh;
        }            
    }
    }
    else
    {
        unsigned int                    i = 0;
        BRAP_AssociatedChannelHandle    hAssociatedCh;    
        hAssociatedCh = *(BRAP_AssociatedChannelHandle *)pHandle;
        BDBG_ASSERT(hAssociatedCh);        

        for(i=0;i<BRAP_MAX_PRI_DEC_CHAN_IN_ASSOCIATED_GRP;i++)
        {
            hRapCh = hAssociatedCh->hPriDecCh[i];
            if(!(BRAP_P_IsPointerValid((void *)hRapCh)))
                break;
            for(uiPth=0; uiPth < BRAP_P_MAX_PATHS_IN_A_CHAN; uiPth++)
            {
                if(!(BRAP_P_IsPointerValid((void *)hRapCh->pPath[uiPth])))
                    break;
                if(((hRapCh->pPath[uiPth]->eUsgPath == BRAP_P_UsgPath_eDecodePcmPostMixing)||
                    (hRapCh->pPath[uiPth]->eUsgPath == BRAP_P_UsgPath_eDecodeCompressPostMixing))&&           
                   ((BRAP_P_IsPointerValid((void *)hRapCh->pPath[uiPth]->hDspCh)))
#if (BRAP_3548_FAMILY != 1) /* Due to SPDIF/HDMI In having DSP only after ProcessDigital Change occurs */           
                   && (BRAP_P_IsPointerValid((void *)hRapCh->pPath[uiPth]->hDsp))
#endif 
                   )
                {
                    return hRapCh->pPath[uiPth]->hDspCh;
                }            
            }
        }
        for(i=0;i<BRAP_MAX_SEC_DEC_CHAN_IN_ASSOCIATED_GRP;i++)
        {
            hRapCh = hAssociatedCh->hSecDecCh[i];
            if(!(BRAP_P_IsPointerValid((void *)hRapCh)))
                break;
            for(uiPth=0; uiPth < BRAP_P_MAX_PATHS_IN_A_CHAN; uiPth++)
            {
                if(!(BRAP_P_IsPointerValid((void *)hRapCh->pPath[uiPth])))
                    break;
                
                if(((hRapCh->pPath[uiPth]->eUsgPath == BRAP_P_UsgPath_eDecodePcmPostMixing)||
                    (hRapCh->pPath[uiPth]->eUsgPath == BRAP_P_UsgPath_eDecodeCompressPostMixing))&&           
                   ((BRAP_P_IsPointerValid((void *)hRapCh->pPath[uiPth]->hDspCh)))
#if (BRAP_3548_FAMILY != 1) /* Due to SPDIF/HDMI In having DSP only after ProcessDigital Change occurs */           
                   && (BRAP_P_IsPointerValid((void *)hRapCh->pPath[uiPth]->hDsp))
#endif 
                  )
                {
                    return hRapCh->pPath[uiPth]->hDspCh;
                }            
            }
        }
        for(i=0;i<BRAP_MAX_PB_CHAN_IN_ASSOCIATED_GRP;i++)
        {
            hRapCh = hAssociatedCh->hPBCh[i];
            if(!(BRAP_P_IsPointerValid((void *)hRapCh)))
                break;
            for(uiPth=0; uiPth < BRAP_P_MAX_PATHS_IN_A_CHAN; uiPth++)
            {
                if(!(BRAP_P_IsPointerValid((void *)hRapCh->pPath[uiPth])))
                    break;                    
                if(((hRapCh->pPath[uiPth]->eUsgPath == BRAP_P_UsgPath_eDecodePcmPostMixing)||
                    (hRapCh->pPath[uiPth]->eUsgPath == BRAP_P_UsgPath_eDecodeCompressPostMixing))&&           
                   (BRAP_P_IsPointerValid((void *)hRapCh->pPath[uiPth]->hDspCh)) &&
                   (BRAP_P_IsPointerValid((void *)hRapCh->pPath[uiPth]->hDsp))
                  )
                {
                    return hRapCh->pPath[uiPth]->hDspCh;
                }            
            }
        }
        for(i=0;i<BRAP_MAX_CAP_CHAN_IN_ASSOCIATED_GRP;i++)
        {
            hRapCh = hAssociatedCh->hCapCh[i];
            if(!(BRAP_P_IsPointerValid((void *)hRapCh)))
                break;
            for(uiPth=0; uiPth < BRAP_P_MAX_PATHS_IN_A_CHAN; uiPth++)
            {
                if(!(BRAP_P_IsPointerValid((void *)hRapCh->pPath[uiPth])))
                    break;
                if(((hRapCh->pPath[uiPth]->eUsgPath == BRAP_P_UsgPath_eDecodePcmPostMixing)||
                    (hRapCh->pPath[uiPth]->eUsgPath == BRAP_P_UsgPath_eDecodeCompressPostMixing))&&           
                   (BRAP_P_IsPointerValid((void *)hRapCh->pPath[uiPth]->hDspCh)) &&
                   (BRAP_P_IsPointerValid((void *)hRapCh->pPath[uiPth]->hDsp))
                  )
                {
                    return hRapCh->pPath[uiPth]->hDspCh;
                }            
            }
        }  
    }

    return NULL;
}


    
void BRAP_DSPCHN_P_AddWordToFWBuffer(
					unsigned char *bufPtr,
					uint32_t data,
					uint32_t nBytes)
{
       BSTD_UNUSED(bufPtr);
       BSTD_UNUSED(data);
       BSTD_UNUSED(nBytes);
	return;
}

#if ( (BRAP_7405_FAMILY == 1) || (BRAP_3548_FAMILY == 1) )
/***************************************************************************
Summary: Set TSM discard threshold. The Parameter i32DiscardThreshold should be 
programmed in ms (milli second)

Description:
	This function set discard threshold for time stamp management. If the
	PTS-STC difference for an audio frame is more than this threshold,
	then that frame is dropped.
	
Returns:
	BERR_SUCCESS - If successful

See Also:
	BRAP_EnableTSM
**************************************************************************/
BERR_Code
BRAP_SetTSMDiscardThreshold(
	BRAP_ChannelHandle	hRapCh,				/* [in] Audio channel handle */
	int32_t				i32DiscardThreshold/* [in] Discard threshold value in ms*/
	)
{
    BERR_Code err = BERR_SUCCESS;
    unsigned int uiConfigBufAddr;
	BRAP_DSPCHN_Handle hDspCh;
    BRAP_FWIF_P_TsmConfigParams      *psTsmConfigParam = NULL;
    BRAP_FWIF_P_FwTaskHandle        hFwTask=NULL;   
    BRAP_FWIF_P_ResponseType        eResponseType=BRAP_FWIF_P_ResponseType_eInvalid;

	BDBG_ENTER(BRAP_SetTSMDiscardThreshold);
	/* Assert on the arguments */
	BDBG_ASSERT(hRapCh);

    if(i32DiscardThreshold > 60000)
    {
        BDBG_WRN(("The Discard threshold value seems to be very high. Please make sure that"
            " it is being set in mili second unit"));
    }

    if((hRapCh->eState == BRAP_P_State_eOpened)
        ||(hRapCh->bStopinvoked == true))
    {
/*Initializing all 3 type of TSM userconfig, because at Opentime DecodeMode is not known*/
/* Programming in 45KHZ tics format*/
        hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.i32TSMDiscardThreshold = i32DiscardThreshold * 45;
        hRapCh->sDecSettings.sUserConfigStruct.sPassthruTsmConfigParam.i32TSMDiscardThreshold = i32DiscardThreshold * 45;  
        hRapCh->sDecSettings.sUserConfigStruct.sEncodeTsmConfigParam.i32TSMDiscardThreshold = i32DiscardThreshold * 45;        
    }
    else if(hRapCh->eState == BRAP_P_State_eStarted)
    {
        hDspCh = BRAP_DSPCHN_P_GetDspChHandle(&hRapCh, true);
        if(!(BRAP_P_IsPointerValid((void *)hDspCh)))
        {
            return BERR_TRACE(BERR_NOT_INITIALIZED);
        }
        
        err = BRAP_GetTsmNodeInfo(hRapCh,hDspCh,&uiConfigBufAddr,&hFwTask,&psTsmConfigParam);
        if(err != BERR_SUCCESS)
        {
    	     BDBG_WRN(("BRAP_SetTsmTransitionThreshold : Channel already stopped, saving it in Handle"));
            hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.i32TSMDiscardThreshold = i32DiscardThreshold * 45;
            hRapCh->sDecSettings.sUserConfigStruct.sPassthruTsmConfigParam.i32TSMDiscardThreshold = i32DiscardThreshold * 45;  
            hRapCh->sDecSettings.sUserConfigStruct.sEncodeTsmConfigParam.i32TSMDiscardThreshold = i32DiscardThreshold * 45;     
        	return err;
        }
        if(hFwTask->bStopped == true)
        {
            hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.i32TSMDiscardThreshold = i32DiscardThreshold * 45;
            hRapCh->sDecSettings.sUserConfigStruct.sPassthruTsmConfigParam.i32TSMDiscardThreshold = i32DiscardThreshold * 45;  
            hRapCh->sDecSettings.sUserConfigStruct.sEncodeTsmConfigParam.i32TSMDiscardThreshold = i32DiscardThreshold * 45;            
        }
        else
        {                      
            
    /* Programming in 45KHZ tics format*/
            psTsmConfigParam->i32TSMDiscardThreshold = i32DiscardThreshold * 45;
            eResponseType = BRAP_FWIF_P_ResponseType_eResponseRequired;
            err = BRAP_DSPCHN_P_SetTsmConfig(hDspCh,uiConfigBufAddr,hFwTask,psTsmConfigParam,&eResponseType);
            if(BERR_SUCCESS != err)
            {
                BDBG_ERR(("BRAP_SetTSMDiscardThreshold: Set Tsm config failed!"));
                return BERR_TRACE(err);
            }
        }
    } 
    BDBG_LEAVE(BRAP_SetTSMDiscardThreshold);
    return BERR_SUCCESS;
}

/***************************************************************************
Summary: Set TSM Gross Adjustment threshold. The Parameter i32DiscardThreshold should be 
programmed in ms (milli second)
	
Returns:
	BERR_SUCCESS - If successful

See Also:
	BRAP_EnableTSM
**************************************************************************/
BERR_Code
BRAP_SetTsmGAThreshold (	
	BRAP_ChannelHandle	hRapCh,				/* [in] Audio channel handle */
	int32_t				i32TsmGAThreshold   /* [in] The new GA threshold in ms*/
	)
{
    BERR_Code err = BERR_SUCCESS;
    unsigned int uiConfigBufAddr;
	BRAP_DSPCHN_Handle hDspCh;
    BRAP_FWIF_P_TsmConfigParams      *psTsmConfigParam = NULL;
    BRAP_FWIF_P_FwTaskHandle        hFwTask=NULL;   
    BRAP_FWIF_P_ResponseType        eResponseType=BRAP_FWIF_P_ResponseType_eInvalid;

	BDBG_ENTER(BRAP_SetTsmGAThreshold);
	/* Assert on the arguments */
	BDBG_ASSERT(hRapCh);
   
    if((hRapCh->eState == BRAP_P_State_eOpened)
        ||(hRapCh->bStopinvoked == true))
    {
/*Initializing all 3 type of TSM userconfig, because at Opentime DecodeMode is not known*/
/* Programming in 45KHZ tics format*/
        hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.i32TSMGrossThreshold= i32TsmGAThreshold * 45;
        hRapCh->sDecSettings.sUserConfigStruct.sPassthruTsmConfigParam.i32TSMGrossThreshold= i32TsmGAThreshold * 45;         
        hRapCh->sDecSettings.sUserConfigStruct.sEncodeTsmConfigParam.i32TSMGrossThreshold= i32TsmGAThreshold * 45;            
    }
    else if(hRapCh->eState == BRAP_P_State_eStarted)
    {
        hDspCh = BRAP_DSPCHN_P_GetDspChHandle(&hRapCh, true);
        if(!(BRAP_P_IsPointerValid((void *)hDspCh)))
        {
            return BERR_TRACE(BERR_NOT_INITIALIZED);
        }
        err = BRAP_GetTsmNodeInfo(hRapCh,hDspCh,&uiConfigBufAddr,&hFwTask,&psTsmConfigParam);
        if(err != BERR_SUCCESS)
        {
    	     BDBG_WRN(("BRAP_SetTsmTransitionThreshold : Channel already stopped, saving it in Handle"));
            hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.i32TSMGrossThreshold= i32TsmGAThreshold * 45;
            hRapCh->sDecSettings.sUserConfigStruct.sPassthruTsmConfigParam.i32TSMGrossThreshold= i32TsmGAThreshold * 45;         
            hRapCh->sDecSettings.sUserConfigStruct.sEncodeTsmConfigParam.i32TSMGrossThreshold= i32TsmGAThreshold * 45;        
        	return err;
        }
        if(hFwTask->bStopped == true)
        {
            hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.i32TSMGrossThreshold= i32TsmGAThreshold * 45;
            hRapCh->sDecSettings.sUserConfigStruct.sPassthruTsmConfigParam.i32TSMGrossThreshold= i32TsmGAThreshold * 45;         
            hRapCh->sDecSettings.sUserConfigStruct.sEncodeTsmConfigParam.i32TSMGrossThreshold= i32TsmGAThreshold * 45;      
        }
        else
        {                      
            
    /* Programming in 45KHZ tics format*/
            psTsmConfigParam->i32TSMGrossThreshold = i32TsmGAThreshold * 45;
            eResponseType = BRAP_FWIF_P_ResponseType_eResponseRequired;
            err = BRAP_DSPCHN_P_SetTsmConfig(hDspCh,uiConfigBufAddr,hFwTask,psTsmConfigParam,&eResponseType);
            if(BERR_SUCCESS != err)
            {
                BDBG_ERR(("BRAP_SetTsmGAThreshold: Set Tsm config failed!"));
                return BERR_TRACE(err);
            }
        }
    } 
    BDBG_LEAVE(BRAP_SetTsmGAThreshold);
    return BERR_SUCCESS;
}
#endif

/***************************************************************************
Summary: Set TSM sync limit threshold. The Parameter i32DiscardThreshold should be 
programmed in ms (milli second)

Description:
        This function sets synclimit threshold for audio master mode. The value of 
        i32TsmSLThreshold is in milliseconds. When the value is set to zero audio master mode
        is disabled.
        
Returns:
        BERR_SUCCESS - If successful

See Also:
        BRAP_EnableTSM
**************************************************************************/
BERR_Code
BRAP_SetTsmSLThreshold ( 
        BRAP_ChannelHandle      hRapCh,                         /* [in] Audio channel handle */
        int32_t                         i32TsmSLThreshold   /* [in] The new SL threshold  in ms*/
        )
{
    BERR_Code err = BERR_SUCCESS;
    unsigned int uiConfigBufAddr;
    BRAP_DSPCHN_Handle hDspCh;
    BRAP_FWIF_P_TsmConfigParams      *psTsmConfigParam = NULL;
    BRAP_FWIF_P_FwTaskHandle        hFwTask=NULL;   
    BRAP_FWIF_P_ResponseType        eResponseType=BRAP_FWIF_P_ResponseType_eInvalid;

    BDBG_ENTER(BRAP_SetTsmSLThreshold);
    /* Assert on the arguments */
    BDBG_ASSERT(hRapCh);
    if((hRapCh->eState == BRAP_P_State_eOpened)
        ||(hRapCh->bStopinvoked == true))
    {
/*Initializing all 3 type of TSM userconfig, because at Opentime DecodeMode is not known*/
/* Programming in 45KHZ tics format*/
        hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.i32TSMSyncLimitThreshold= i32TsmSLThreshold * 45;
        hRapCh->sDecSettings.sUserConfigStruct.sPassthruTsmConfigParam.i32TSMSyncLimitThreshold= i32TsmSLThreshold * 45;
        hRapCh->sDecSettings.sUserConfigStruct.sEncodeTsmConfigParam.i32TSMSyncLimitThreshold= i32TsmSLThreshold * 45;          
    }
    else if(hRapCh->eState == BRAP_P_State_eStarted)
    {
        hDspCh = BRAP_DSPCHN_P_GetDspChHandle(&hRapCh, true);
        if(!(BRAP_P_IsPointerValid((void *)hDspCh)))
        {
            return BERR_TRACE(BERR_NOT_INITIALIZED);
        }
        err = BRAP_GetTsmNodeInfo(hRapCh,hDspCh,&uiConfigBufAddr,&hFwTask,&psTsmConfigParam);
        if(err != BERR_SUCCESS)
        {
    	     BDBG_WRN(("BRAP_SetTsmTransitionThreshold : Channel already stopped, saving it in Handle"));
            hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.i32TSMSyncLimitThreshold= i32TsmSLThreshold * 45;
            hRapCh->sDecSettings.sUserConfigStruct.sPassthruTsmConfigParam.i32TSMSyncLimitThreshold= i32TsmSLThreshold * 45;
            hRapCh->sDecSettings.sUserConfigStruct.sEncodeTsmConfigParam.i32TSMSyncLimitThreshold= i32TsmSLThreshold * 45;   
        	return err;
        }
        if(hFwTask->bStopped == true)
        {
            hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.i32TSMSyncLimitThreshold= i32TsmSLThreshold * 45;
            hRapCh->sDecSettings.sUserConfigStruct.sPassthruTsmConfigParam.i32TSMSyncLimitThreshold= i32TsmSLThreshold * 45;
            hRapCh->sDecSettings.sUserConfigStruct.sEncodeTsmConfigParam.i32TSMSyncLimitThreshold= i32TsmSLThreshold * 45;           
        }
        else
        {                             
    /* Programming in 45KHZ tics format*/
            psTsmConfigParam->i32TSMSyncLimitThreshold = i32TsmSLThreshold * 45;
            eResponseType = BRAP_FWIF_P_ResponseType_eResponseRequired;
            err = BRAP_DSPCHN_P_SetTsmConfig(hDspCh,uiConfigBufAddr,hFwTask,psTsmConfigParam,&eResponseType);
            if(BERR_SUCCESS != err)
            {
                BDBG_ERR(("BRAP_SetTsmSLThreshold: Set Tsm config failed!"));
                return BERR_TRACE(err);
            }
        }
    } 
    BDBG_LEAVE(BRAP_SetTsmSLThreshold);
    return BERR_SUCCESS;
}

/***************************************************************************
Summary: Enable/Disable Adaptive System Time Management

Description:
        This function enables/disables Adaptive System Time Management(ASTM) for a given audio
        channel.
        
Returns:
        BERR_SUCCESS - If successful

See Also:
        BRAP_EnableASTM_isr
**************************************************************************/
BERR_Code
BRAP_EnableASTM(
        BRAP_ChannelHandle      hRapCh,                 /* [in] Audio channel handle */
        bool                    bEnableASTM             /* [in] true = enable ASTM,
                                                                false = disable ASTM  */
        )
{

	BERR_Code rc = BERR_SUCCESS;

	BDBG_ENTER(BRAP_EnableASTM);
	/* Assert the function arguments */
	BDBG_ASSERT(hRapCh);

	BKNI_EnterCriticalSection();
	rc = BRAP_EnableASTM_isr(hRapCh, bEnableASTM);
	BKNI_LeaveCriticalSection();

	BDBG_LEAVE(BRAP_EnableASTM);
	return rc;

}

/***************************************************************************
Summary: ISR version of BRAP_DSPCHN_EnableASTM

Description:
        This function enables/disables Adaptive System Time Management(ASTM) for a given audio
        channel in isr context.
        
Returns:
        BERR_SUCCESS - If successful

See Also:
        BRAP_DSPCHN_EnableASTM
**************************************************************************/
BERR_Code
BRAP_EnableASTM_isr(
        BRAP_ChannelHandle      hRapCh,                 /* [in] Audio channel handle */
        bool                    bEnableASTM             /* [in] true = enable ASTM,
                                                                false = disable ASTM  */
        )
{
    BERR_Code err = BERR_SUCCESS;
    unsigned int uiConfigBufAddr;
    BRAP_DSPCHN_Handle hDspCh;
    BRAP_FWIF_P_TsmConfigParams      *psTsmConfigParam = NULL;
    BRAP_FWIF_P_FwTaskHandle        hFwTask=NULL;   
    BRAP_FWIF_P_ResponseType        eResponseType=BRAP_FWIF_P_ResponseType_eInvalid;

    BDBG_ENTER(BRAP_EnableASTM_isr);
    BDBG_ASSERT(hRapCh);
    
    if((hRapCh->eState == BRAP_P_State_eOpened)
        ||(hRapCh->bStopinvoked == true))
    {
/*Initializing all 3 type of TSM userconfig, because at Opentime DecodeMode is not known*/
        if(bEnableASTM)
        {
            hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.eAstmEnable = BRAP_FWIF_eTsmBool_True;
            hRapCh->sDecSettings.sUserConfigStruct.sPassthruTsmConfigParam.eAstmEnable = BRAP_FWIF_eTsmBool_True;
            hRapCh->sDecSettings.sUserConfigStruct.sEncodeTsmConfigParam.eAstmEnable = BRAP_FWIF_eTsmBool_True;            
        }
        else
        {
            hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.eAstmEnable = BRAP_FWIF_eTsmBool_False;
            hRapCh->sDecSettings.sUserConfigStruct.sPassthruTsmConfigParam.eAstmEnable = BRAP_FWIF_eTsmBool_False;
            hRapCh->sDecSettings.sUserConfigStruct.sEncodeTsmConfigParam.eAstmEnable = BRAP_FWIF_eTsmBool_False;            
        }
    }
    else if(hRapCh->eState == BRAP_P_State_eStarted)
    {
        hDspCh = BRAP_DSPCHN_P_GetDspChHandle_isr(&hRapCh, true);
        if(!(BRAP_P_IsPointerValid((void *)hDspCh)))
        {
    		BDBG_WRN(("BRAP_EnableASTM_isr : Channel already stopped, saving it in Handle"));
            hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.eAstmEnable = bEnableASTM;
            hRapCh->sDecSettings.sUserConfigStruct.sPassthruTsmConfigParam.eAstmEnable = bEnableASTM;
            hRapCh->sDecSettings.sUserConfigStruct.sEncodeTsmConfigParam.eAstmEnable = bEnableASTM;             
            return BERR_NOT_INITIALIZED;
        }
        /* Don't program the ASTM for an ES stream. But, do save the user preference
           in the hDspCh->sSettings.eAstmEnable for later use with other stream types 
           during StartDspCxt */
        if(BAVC_StreamType_eEs == hDspCh->sDspAudioParams.sExtAudioParams.eStreamType)
        {
            /* Throw a warning message and return success for an ES stream type */
            BDBG_WRN(("BRAP_EnableASTM_isr: ES stream type can't have ASTM! \n Saving "
                "the user preference eAstmEnable = %d for use with other stream types"
                " in future.", bEnableASTM));
            hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.eAstmEnable = bEnableASTM;
            hRapCh->sDecSettings.sUserConfigStruct.sPassthruTsmConfigParam.eAstmEnable = bEnableASTM;
            hRapCh->sDecSettings.sUserConfigStruct.sEncodeTsmConfigParam.eAstmEnable = bEnableASTM;                   
        	return BERR_SUCCESS;
        }

        err = BRAP_GetTsmNodeInfo_isr(hRapCh,hDspCh,&uiConfigBufAddr,&hFwTask,&psTsmConfigParam);
        if(err != BERR_SUCCESS)
        {
    		BDBG_WRN(("BRAP_EnableASTM_isr : Channel already stopped, saving it in Handle"));
            hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.eAstmEnable = bEnableASTM;
            hRapCh->sDecSettings.sUserConfigStruct.sPassthruTsmConfigParam.eAstmEnable = bEnableASTM;
            hRapCh->sDecSettings.sUserConfigStruct.sEncodeTsmConfigParam.eAstmEnable = bEnableASTM;               
        	return err;
        }
        if(hFwTask->bStopped == true)
        {
            if(bEnableASTM)
            {
                hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.eAstmEnable = BRAP_FWIF_eTsmBool_True;
                hRapCh->sDecSettings.sUserConfigStruct.sPassthruTsmConfigParam.eAstmEnable = BRAP_FWIF_eTsmBool_True;
                hRapCh->sDecSettings.sUserConfigStruct.sEncodeTsmConfigParam.eAstmEnable = BRAP_FWIF_eTsmBool_True;            
            }
            else
            {
                hRapCh->sDecSettings.sUserConfigStruct.sDecodeTsmConfigParam.eAstmEnable = BRAP_FWIF_eTsmBool_False;
                hRapCh->sDecSettings.sUserConfigStruct.sPassthruTsmConfigParam.eAstmEnable = BRAP_FWIF_eTsmBool_False;
                hRapCh->sDecSettings.sUserConfigStruct.sEncodeTsmConfigParam.eAstmEnable = BRAP_FWIF_eTsmBool_False;            
            }      
        }
        else
        {                             
            if(bEnableASTM)
            {
                psTsmConfigParam->eAstmEnable = BRAP_FWIF_eTsmBool_True;
            }
            else
            {
                psTsmConfigParam->eAstmEnable = BRAP_FWIF_eTsmBool_False;
            }
            eResponseType = BRAP_FWIF_P_ResponseType_eNone;
            err = BRAP_DSPCHN_P_SetTsmConfig_isr(hDspCh,uiConfigBufAddr,hFwTask,psTsmConfigParam,&eResponseType);
            if(BERR_SUCCESS != err)
            {
                BDBG_ERR(("BRAP_EnableASTM_isr: Set Tsm config failed!"));
                return BERR_TRACE(err);
            }
        }
    }       

    BDBG_LEAVE(BRAP_EnableASTM_isr);
	return BERR_SUCCESS;
}

/***************************************************************************
Summary: It returns 
-   the Version Information of Decoder 
-   Profile Id in case of DTS Decoder.
-   Internal version of FW.

Format of display should be
ui8AlgoProfileId.ui8AlgoVersionId[3].ui8AlgoVersionId[2].ui8AlgoVersionId[1]. \
ui8AlgoVersionId[0].ui8FWVersionId

**************************************************************************/
BERR_Code   BRAP_GetDecoderVersionInfo(
    BRAP_ChannelHandle              hRapCh,              /* [in] Raptor Channel Handle */
    BRAP_DSPCHN_DecoderVersionInfo  *psDecoderVersionInfo /* [out] Decoder Version Info*/
    )
{
    BERR_Code   ret=BERR_SUCCESS;
    uint32_t    *uiCodePtr=NULL;
    uint32_t    uiTemp=0,uiFwId=BRAP_INVALID_VALUE;
    BRAP_DSPCHN_Handle  hDspCh=NULL;
    
    BDBG_ASSERT(hRapCh);

    if(hRapCh->eState == BRAP_P_State_eStarted)
    {
        hDspCh = BRAP_DSPCHN_P_GetDspChHandle(&hRapCh, true);

        if(!(BRAP_P_IsPointerValid((void *)hDspCh)))
        {
            BDBG_ERR(("BRAP_GetDecoderVersionInfo: Error!!! DspCh Can't be NULL"));
            ret = BERR_TRACE(BRAP_ERR_BAD_DEVICE_STATE);
            goto end;
        }        
#if 0    
        if(!((hDspCh->sDspAudioParams.sExtAudioParams.eType == BRAP_DSPCHN_AudioType_eDtshd) ||
             (hDspCh->sDspAudioParams.sExtAudioParams.eType == BRAP_DSPCHN_AudioType_eDtsBroadcast)))
        {
            BDBG_ERR(("BRAP_GetDecoderVersionInfo: Unsupported Algo Type %d",
                hDspCh->sDspAudioParams.sExtAudioParams.eType));
            ret = BERR_TRACE(BERR_NOT_SUPPORTED);                        
            goto end;            
        }
#endif        
        uiFwId = BRAP_sDecAlgoExecInfo[hDspCh->sDspAudioParams.sExtAudioParams.eType][0].eAlgoIds[2];

        uiCodePtr = (uint32_t  *)hRapCh->hRap->sMit.sAlgoDwnldDetail[uiFwId].ui32AlgoCodePtr;
        if(!(BRAP_P_IsPointerValid((void *)uiCodePtr)))
        {
            BDBG_ERR(("BRAP_GetDecoderVersionInfo: Version Id of Algorithm Id %d is not available",
                hDspCh->sDspAudioParams.sExtAudioParams.eType));
            ret = BERR_TRACE(BERR_INVALID_PARAMETER);
            
        }
        else
        {
            if((hDspCh->sDspAudioParams.sExtAudioParams.eType == BRAP_DSPCHN_AudioType_eDtshd) ||
                 (hDspCh->sDspAudioParams.sExtAudioParams.eType == BRAP_DSPCHN_AudioType_eDtsBroadcast))
            {        
                BRAP_P_GetAlgorithmProfileId(&(psDecoderVersionInfo->ui8AlgoProfileId));
            }
            
            uiTemp = BRAP_MemRead32((uint32_t)uiCodePtr);            
            psDecoderVersionInfo->ui8AlgoVersionId[3] = (uiTemp & 0xFF000000) >> 24;
            psDecoderVersionInfo->ui8AlgoVersionId[2] = (uiTemp & 0x00FF0000) >>16;
            psDecoderVersionInfo->ui8AlgoVersionId[1] = (uiTemp & 0x0000FF00) >>8;
            psDecoderVersionInfo->ui8AlgoVersionId[0] = (uiTemp & 0x000000FF);  
            uiCodePtr++;
            
            uiTemp = BRAP_MemRead32((uint32_t)uiCodePtr);            
            psDecoderVersionInfo->ui8FWVersionId = (uiTemp & 0xFF000000) >> 24;            

            BDBG_MSG(("BRAP_GetDecoderVersionInfo: Version Info is %d.%d.%d.%d.%d.%d",
                psDecoderVersionInfo->ui8AlgoProfileId,psDecoderVersionInfo->ui8AlgoVersionId[3],
                psDecoderVersionInfo->ui8AlgoVersionId[2],psDecoderVersionInfo->ui8AlgoVersionId[1],
                psDecoderVersionInfo->ui8AlgoVersionId[0],psDecoderVersionInfo->ui8FWVersionId));
        }
    }
    else
    {
        BDBG_ERR(("BRAP_GetDecoderVersionInfo: Channel must be started before calling this API"));
        ret = BERR_TRACE(BERR_INVALID_PARAMETER);            
    }
end:
    return ret;
}

/***************************************************************************
Summary: It returns the Version Information of Encoder .

Format of display should be
ui8AlgoProfileId.ui8AlgoVersionId[3].ui8AlgoVersionId[2].ui8AlgoVersionId[1]. \
ui8AlgoVersionId[0].ui8FWVersionId

**************************************************************************/
BERR_Code   BRAP_GetEncoderVersionInfo(
    BRAP_ChannelHandle              hRapCh,              /* [in] Raptor Channel Handle */
    BRAP_DSPCHN_EncoderVersionInfo  *psEncoderVersionInfo /* [out] Encoder Version Info*/
    )
{
    BERR_Code           ret=BERR_SUCCESS;
    uint32_t                *uiCodePtr=NULL;
    unsigned int    uiDst=0, uiStage=0;
    unsigned int        uiTemp=0,uiFwId=BRAP_INVALID_VALUE;
    bool                bDTSEncFound = false;
    
    BDBG_ASSERT(hRapCh);

    if(hRapCh->eState == BRAP_P_State_eStarted)
    {           
        for(uiDst = 0; uiDst < BRAP_P_MAX_DST_PER_RAPCH; uiDst++)
        {
            for(uiStage = 0; uiStage < BRAP_MAX_STAGE_PER_BRANCH_SUPPORTED; uiStage++)            
            {
                if((NULL != hRapCh->sChAudProcessingStage[uiDst][uiStage].hAudioProcessing) &&
                   (BRAP_ProcessingType_eEncodeDts == hRapCh->sChAudProcessingStage[uiDst][uiStage].
                            hAudioProcessing->sProcessingStageSettings.eAudioProcessing))
                {
                    bDTSEncFound = true;                
                    break;
                }
            }
            if(bDTSEncFound == true)
                break;
        }

        if(bDTSEncFound != true)
        {
            BDBG_ERR(("BRAP_GetEncoderVersionInfo: DTS Encoder stage not found"
                " in the channel handle 0x%x", hRapCh));
            ret = BERR_TRACE(BERR_NOT_SUPPORTED);                        
            goto end;            
        }
        
        uiFwId = BRAP_sEncAlgoExecInfo[BRAP_CIT_P_EncAudioType_eDTSBroadcast][0].eAlgoIds[2];

        uiCodePtr = (uint32_t *)hRapCh->hRap->sMit.sAlgoDwnldDetail[uiFwId].ui32AlgoCodePtr;
        if(!(BRAP_P_IsPointerValid((void *)uiCodePtr)))
        {
            BDBG_ERR(("BRAP_GetEncoderVersionInfo: Version Id of DTS Encoder is not available"));
            ret = BERR_TRACE(BERR_INVALID_PARAMETER);
            
        }
        else
        {
            BRAP_P_GetAlgorithmProfileId(&(psEncoderVersionInfo->ui8AlgoProfileId));
           uiTemp = BRAP_MemRead32((uint32_t)uiCodePtr);
            psEncoderVersionInfo->ui8AlgoVersionId[3] = (uiTemp & 0xFF000000) >> 24;
            psEncoderVersionInfo->ui8AlgoVersionId[2] = (uiTemp & 0x00FF0000) >>16;
            psEncoderVersionInfo->ui8AlgoVersionId[1] = (uiTemp & 0x0000FF00) >>8;
            psEncoderVersionInfo->ui8AlgoVersionId[0] = (uiTemp & 0x000000FF);       
            uiCodePtr++;
            
            uiTemp = BRAP_MemRead32((uint32_t)uiCodePtr);            
            psEncoderVersionInfo->ui8FWVersionId = (uiTemp & 0xFF000000) >> 24;      

            BDBG_MSG(("BRAP_GetEncoderVersionInfo: Version Info is %d.%d.%d.%d.%d.%d",
                psEncoderVersionInfo->ui8AlgoProfileId,psEncoderVersionInfo->ui8AlgoVersionId[3],
                psEncoderVersionInfo->ui8AlgoVersionId[2],psEncoderVersionInfo->ui8AlgoVersionId[1],
                psEncoderVersionInfo->ui8AlgoVersionId[0],psEncoderVersionInfo->ui8FWVersionId));
        }
    }
    else
    {
        BDBG_ERR(("BRAP_GetEncoderVersionInfo: Channel must be started before calling this API"));
            ret = BERR_TRACE(BERR_INVALID_PARAMETER);            
    }
end:
    return ret;
}

BERR_Code
BRAP_DSPCHN_P_FMMPortReconfig (
    BRAP_Handle             hRap,
	BRAP_ChannelHandle      hRapCh,
    BRAP_DstDetails         *psExtDstDetails,
	bool                    bPortEnable
	)
{
    BERR_Code ret = BERR_SUCCESS;
    unsigned int            uiFwTaskId;
    BRAP_FWIF_P_Command     sCommand;
    BRAP_FWIF_P_Response    sRsp;    
    BRAP_SPS_InputInfo      sSPSInputInfo;
    BRAP_SPS_InputPortCfg   sSPSInputPortCfg;
    BRAP_FWIF_P_FwTaskHandle hFwTask = NULL;
    BRAP_P_MsgType      eMsgType;    
    
    BDBG_ENTER(BRAP_DSPCHN_P_FMMPortReconfig);

    BDBG_ASSERT(hRap);
    BDBG_ASSERT(hRapCh);
    BDBG_ASSERT(psExtDstDetails);

    for(uiFwTaskId = 0; uiFwTaskId < BRAP_RM_P_MAX_FW_TASK_PER_DSP; uiFwTaskId++)
    {
        if((hRap->hFwTask[0][uiFwTaskId] != NULL) &&
            (hRap->hFwTask[0][uiFwTaskId]->uHandle.hRapCh == hRapCh))
        {
            hFwTask = hRap->hFwTask[0][uiFwTaskId];
            if(hFwTask->bStopped == false)
            {
                sSPSInputInfo.ui32TaskId = hFwTask->uiTaskId;
                sSPSInputInfo.ui32CitTaskFmmDestCfgMemoryAddr = hFwTask->sCitOutput.sCit.sGlobalTaskConfig.ui32FmmDestCfgAddr;
                sSPSInputInfo.sOperatingTaskFmmDestCfgBuff.ui32DramBufferAddress = hRap->sMemInfo.sConfigParamBufInfo.ui32BaseAddr;
                sSPSInputInfo.sOperatingTaskFmmDestCfgBuff.ui32BufferSizeInBytes = sizeof(BRAP_AF_P_sFMM_DEST_CFG);            
                break;            
            }
        }
    }
    if(uiFwTaskId >= BRAP_RM_P_MAX_FW_TASK_PER_DSP)
    {
        BDBG_ERR(("No Task in running state for this Channel %x", hRapCh));
        ret = BERR_TRACE(BERR_INVALID_PARAMETER);
        goto exit;
    }

    sSPSInputPortCfg.ui32NumOutputs = 1;
    sSPSInputPortCfg.sPortCfg[0].ePortEnableDisableFlag = bPortEnable;
    
    switch(psExtDstDetails->eAudioDst)
    {
        case BRAP_AudioDst_eOutputPort:
            sSPSInputPortCfg.sPortCfg[0].sFmmDstDetails.eFmmDstSelect = BRAP_CIT_P_FmmDstSelect_eOpPort;
            sSPSInputPortCfg.sPortCfg[0].sFmmDstDetails.uFmmDstPortType.eOpType = 
                psExtDstDetails->uDstDetails.sOpDetails.eOutput[0];
            BDBG_MSG(("PortEnable=%d, OpPort=%d", sSPSInputPortCfg.sPortCfg[0].ePortEnableDisableFlag,
                sSPSInputPortCfg.sPortCfg[0].sFmmDstDetails.uFmmDstPortType.eOpType ));
            break;
        case BRAP_AudioDst_eRingBuffer:
            sSPSInputPortCfg.sPortCfg[0].sFmmDstDetails.eFmmDstSelect = BRAP_CIT_P_FmmDstSelect_eCapturePort;
            sSPSInputPortCfg.sPortCfg[0].sFmmDstDetails.uFmmDstPortType.eCapPortType = 
                psExtDstDetails->uDstDetails.sRBufDetails.eCapPort[0];
            BDBG_MSG(("PortEnable=%d, CapPort=%d", sSPSInputPortCfg.sPortCfg[0].ePortEnableDisableFlag,
                sSPSInputPortCfg.sPortCfg[0].sFmmDstDetails.uFmmDstPortType.eCapPortType ));            
            break;
        default:
            BDBG_ERR(("Invalid Destination Type %d", psExtDstDetails->eAudioDst));
            ret = BERR_INVALID_PARAMETER;
            goto exit;
    }
    
    BDBG_MSG(("TaskId=%d, FmmCfgAddr=%x, NewFmmCfgAddr=%x, size=%d", sSPSInputInfo.ui32TaskId, sSPSInputInfo.ui32CitTaskFmmDestCfgMemoryAddr,
        sSPSInputInfo.sOperatingTaskFmmDestCfgBuff.ui32DramBufferAddress, sSPSInputInfo.sOperatingTaskFmmDestCfgBuff.ui32BufferSizeInBytes));
    
    ret = BRAP_SPS_P_SeamlessPortSwitch(hRap->hHeap, hRap->hRegister, &sSPSInputInfo, &sSPSInputPortCfg);
    if(BERR_SUCCESS != ret)
    {
        BDBG_ERR(("BRAP_DSPCHN_P_FMMPortReconfig: BRAP_SPS_P_SeamlessPortSwitch returned Error"));
		ret = BERR_TRACE(ret);                
        goto exit;
    }

    /* Create FMMPORT-Reconfig command */
    sCommand.sCommandHeader.ui32CommandID = BRAP_FWIF_P_FMMPORT_RECFG_COMMAND_ID;
    sCommand.sCommandHeader.ui32CommandCounter = hRap->uiCommandCounter++;
    sCommand.sCommandHeader.ui32TaskID = sSPSInputInfo.ui32TaskId;
    sCommand.sCommandHeader.eResponseType = BRAP_FWIF_P_ResponseType_eResponseRequired;
    sCommand.uCommand.sFmmPortReconfig.ui32TaskFMMPortCfgBuffAddr = sSPSInputInfo.ui32CitTaskFmmDestCfgMemoryAddr;
    sCommand.uCommand.sFmmPortReconfig.ui32SizeInBytes = sSPSInputInfo.sOperatingTaskFmmDestCfgBuff.ui32BufferSizeInBytes;

    ret = BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(sSPSInputInfo.sOperatingTaskFmmDestCfgBuff.ui32DramBufferAddress), 
            &(sCommand.uCommand.sFmmPortReconfig.ui32NewFMMPortBuffAdd));	
    if(BERR_SUCCESS != ret)
    {
        BDBG_ERR(("BRAP_P_ConvertAddressToOffset returned error"));
        ret = BERR_INVALID_PARAMETER;
        goto exit;
    }

    hFwTask->uiLastEventType = sCommand.sCommandHeader.ui32CommandID;
    BRAP_P_EventReset(hFwTask->hDsp,BRAP_GET_TASK_INDEX(hFwTask->uiTaskId));

    ret = BRAP_FWIF_P_SendCommand(hFwTask->hDsp->hCmdQueue, &sCommand,hRap,hFwTask);
    if(BERR_SUCCESS != ret)
    {
        BDBG_ERR(("BRAP_DSPCHN_P_SetConfig: FMMPORT_RECFG_Command failed!"));
        return BERR_TRACE(ret);
    }

    /* Wait for Ack_Response_Received event w/ timeout */
    ret = BRAP_P_EventWait(hFwTask->hDsp, BRAP_DSPCHN_P_EVENT_TIMEOUT_IN_MS,BRAP_GET_TASK_INDEX(hFwTask->uiTaskId));
    if(BERR_TIMEOUT == ret)
    {
        BDBG_ERR(("Event Timed Out"));
        BDBG_ASSERT(0);
        ret = BERR_TRACE(ret);
        goto exit;
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
            BDBG_ERR(("BRAP_DSPCHN_P_SetConfig: Unable to read ACK!"));
            return BERR_TRACE(ret);
        }
        else
            ret = BERR_SUCCESS;      
    }
    
    if((sRsp.sCommonAckResponseHeader.eStatus != BERR_SUCCESS)||
        (sRsp.sCommonAckResponseHeader.ui32ResponseID != BRAP_FWIF_P_FMMPORT_RECFG_RESPONSE_ID)||
        (sRsp.sCommonAckResponseHeader.ui32TaskID != hFwTask->uiTaskId))
    {
        if((hRap->bWatchdogTriggered == false)
            &&(hFwTask->bStopped == false))
        {
            BDBG_ERR(("BRAP_DSPCHN_P_SetConfig: FMMPORT_RECFG_Command response not received successfully!"));
            return BERR_TRACE(sRsp.sCommonAckResponseHeader.eStatus);
        }
        else
            ret = BERR_SUCCESS;
    }

exit:
    BDBG_LEAVE(BRAP_DSPCHN_P_FMMPortReconfig);
    return ret;
}
#ifdef RAP_VIDEOONDSP_SUPPORT
/***************************************************************************
Summary: Starts a DSP context

Description:
	This function is responsible for starting a DSP context for the given
	input decode mode.
	
Returns:
	BERR_SUCCESS - If successful

See Also:
	BRAP_DSPCHN_P_Stop
**************************************************************************/
BERR_Code
BRAP_DSPCHN_P_StartVideo(
	BRAP_DSPCHN_Handle	hDspCh,					/* [in] DSPCHN handle */
	const BRAP_DSPCHN_P_AudioParams *psAudioParams	/* [in] Audio decode parameters */
	)

{
    BRAP_P_AudioProcNetwork     *psAudProcNetwork = NULL;
    BRAP_CIT_P_InputInfo        *psCitInputStruct = NULL;
    unsigned int i = 0, j = 0, k = 0;
    unsigned int uiBranchId = 0, uiStageId = 0;
    BERR_Code err = BERR_SUCCESS;
    BRAP_FWIF_MsgQueueHandle hMsgQueue;
    uint32_t ui32SyncFifo = BRAP_DSP_P_FIFO_INVALID;
    uint32_t ui32AsyncFifo = BRAP_DSP_P_FIFO_INVALID;
    BRAP_FWIF_P_FwTaskHandle hFwTaskCreate = NULL;
    BRAP_FWIF_P_Command sCommand;
    BRAP_FWIF_P_Response sRsp;
    BRAP_Handle hRap = NULL;
    uint32_t ui32PhysAddr = 0, ui32RbufOffset = 0;
    BRAP_FWIF_P_TaskParamInfo *pTaskParams = NULL;
    BRAP_P_MsgType      eMsgType;
    BRAP_DSP_Handle hDsp = NULL;
    bool                        bWdgRecovery = false;
    uint32_t    ui32QueueId;
    uint32_t ui32NumBranchesValid = 0, ui32NumStagesValid = 0;
    unsigned int    ui32Threshold = 0;
    unsigned int    ui32BlockTime = 0;            
    BRAP_FWDWNLD_P_Param	sParam;
    BRAP_VF_P_sVDecodeBuffCfg   *psVDecodeBuffCfg=NULL; 
    
    BDBG_ENTER( BRAP_DSPCHN_P_StartVideo );

    hRap = hDspCh->uHandle.hRapCh->hRap;

    bWdgRecovery = BRAP_P_GetWatchdogRecoveryFlag(hRap);
    if(false == bWdgRecovery)
    {
	/* Storing psAudioParams in hDspch */
	hDspCh->sDspAudioParams = *psAudioParams;
    }
    else
    {
        psAudioParams = &(hDspCh->sDspAudioParams);
    }

    hDspCh->bChSpecificDspCh = true;
    
    /* Allocate big structures on heap */
    psAudProcNetwork = (BRAP_P_AudioProcNetwork *) BKNI_Malloc(sizeof(BRAP_P_AudioProcNetwork));
    if (NULL==psAudProcNetwork)
    {
        BDBG_ERR(("Out of System memory"));
        err = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        goto error;
    }
    BKNI_Memset(psAudProcNetwork, 0,(sizeof(BRAP_P_AudioProcNetwork)));

    psCitInputStruct = (BRAP_CIT_P_InputInfo *)BKNI_Malloc(sizeof(BRAP_CIT_P_InputInfo));
    if (NULL==psCitInputStruct)
    {
        BDBG_ERR(("Out of System memory"));
        err = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        goto error;
    }
	BKNI_Memset( psCitInputStruct, 0, sizeof(BRAP_CIT_P_InputInfo) );

    psVDecodeBuffCfg = (BRAP_VF_P_sVDecodeBuffCfg *)BKNI_Malloc(sizeof(BRAP_VF_P_sVDecodeBuffCfg));
    if (NULL==psVDecodeBuffCfg)
    {
        BDBG_ERR(("Out of System memory"));
        err = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        goto error;
    }
	BKNI_Memset( psCitInputStruct, 0, sizeof(BRAP_CIT_P_InputInfo) );    

    if ((BRAP_AudioSource_eExtCapPort== hDspCh->uHandle.hRapCh->eAudioSource) ||
        (BRAP_AudioSource_eRingBuffer== hDspCh->uHandle.hRapCh->eAudioSource))
    {    
        hDspCh->sNetworkInfo.eNetworkInputType = BRAP_CIT_P_FwStgSrcDstType_eFMMBuf;
    }
    else if (BRAP_AudioSource_eXptInterface == hDspCh->uHandle.hRapCh->eAudioSource)
    {
        hDspCh->sNetworkInfo.eNetworkInputType = BRAP_CIT_P_FwStgSrcDstType_eRaveBuf;        
    }

/* Populate psNetworkInfo struct */
    hDspCh->sNetworkInfo.eVideoType = psAudioParams->sExtVideoParams.eType;
    hDspCh->sNetworkInfo.sInputBufConfig.uIoBuffer.sIoBuffer.ui32NumBuffers = 2;

    if(hDspCh->sNetworkInfo.eNetworkInputType == BRAP_CIT_P_FwStgSrcDstType_eRaveBuf)
    {
        hDspCh->sNetworkInfo.sInputBufConfig.uIoBuffer.sIoBuffer.eBufferType = BRAP_AF_P_BufferType_eRAVE;	
        hDspCh->sNetworkInfo.sInputBufConfig.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32BaseAddr = psAudioParams->sXptContextMap.CDB_Base + BCHP_PHYSICAL_OFFSET;
        hDspCh->sNetworkInfo.sInputBufConfig.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32EndAddr = psAudioParams->sXptContextMap.CDB_End + BCHP_PHYSICAL_OFFSET;
        hDspCh->sNetworkInfo.sInputBufConfig.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32ReadAddr = psAudioParams->sXptContextMap.CDB_Read + BCHP_PHYSICAL_OFFSET;
        hDspCh->sNetworkInfo.sInputBufConfig.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32WriteAddr = psAudioParams->sXptContextMap.CDB_Valid + BCHP_PHYSICAL_OFFSET;
        hDspCh->sNetworkInfo.sInputBufConfig.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32WrapAddr = psAudioParams->sXptContextMap.CDB_Wrap + BCHP_PHYSICAL_OFFSET;		


        hDspCh->sNetworkInfo.sInputBufConfig.uIoBuffer.sIoBuffer.sCircBuffer[1].ui32BaseAddr = psAudioParams->sXptContextMap.ITB_Base + BCHP_PHYSICAL_OFFSET;
        hDspCh->sNetworkInfo.sInputBufConfig.uIoBuffer.sIoBuffer.sCircBuffer[1].ui32EndAddr = psAudioParams->sXptContextMap.ITB_End + BCHP_PHYSICAL_OFFSET;
        hDspCh->sNetworkInfo.sInputBufConfig.uIoBuffer.sIoBuffer.sCircBuffer[1].ui32ReadAddr = psAudioParams->sXptContextMap.ITB_Read + BCHP_PHYSICAL_OFFSET;
        hDspCh->sNetworkInfo.sInputBufConfig.uIoBuffer.sIoBuffer.sCircBuffer[1].ui32WriteAddr = psAudioParams->sXptContextMap.ITB_Valid + BCHP_PHYSICAL_OFFSET;
        hDspCh->sNetworkInfo.sInputBufConfig.uIoBuffer.sIoBuffer.sCircBuffer[1].ui32WrapAddr = psAudioParams->sXptContextMap.ITB_Wrap + BCHP_PHYSICAL_OFFSET;		
    }  
	
    if(BRAP_P_IsPointerValid((void *)hDspCh->uHandle.hRapCh))
    {
        for(j=0;j<BRAP_P_MAX_DST_PER_RAPCH;j++)
        {
            for(k=0;k<BRAP_MAX_STAGE_PER_BRANCH_SUPPORTED;k++)
            {
                psAudProcNetwork->sAudProcessingStage[j][k]=  hDspCh->uHandle.hRapCh->sChAudProcessingStage[j][k];
            }
        }
    }        
    if(false == bWdgRecovery)
    {
        /* TODO: Split the network into multiple small networks */
        BRAP_DSPCHN_P_CreateFwTask(hDspCh,psAudProcNetwork);
    }

    if(false == hRap->sSettings.bOpenTimeFwDownload)
    {
        /* Download the firmware binaries required by the complete network */
        sParam.hRap = hRap;
        BRAP_FWDWNLD_P_DownloadVideoDecodeExecs(&sParam,false,psAudioParams->sExtVideoParams.eType);        
        if( BERR_SUCCESS!=err )
        {
            goto error;
        }
    }

    for (i = 0; i < BRAP_FWIF_P_MAX_FW_TASK_PER_DSPCHN; i++)
    {
        hFwTaskCreate = hDspCh->sFwTaskInfo[i].hFwTask;
        if(!(BRAP_P_IsPointerValid((void *)hFwTaskCreate)))
        {
            BDBG_MSG(("Task handle is NULL for Task no. %d",i+BRAP_FWIF_P_TASK_ID_START_OFFSET));
            continue;
        }
        hDsp = hFwTaskCreate->hDsp;

        BDBG_MSG(("hFwTaskCreate->uiTaskId = %d , hDsp->uiDspIndex =%d",hFwTaskCreate->uiTaskId,hDsp->uiDspIndex));

        /* Convert processing network into CIT module input */
        err = BRAP_FWIF_P_GenerateVideoCitInput(hRap,hDspCh, &(hDspCh->sFwTaskInfo[i].sProcessingNw), &(hDspCh->sNetworkInfo), psCitInputStruct,psVDecodeBuffCfg,hFwTaskCreate);
        if( BERR_SUCCESS!=err )
        {
            goto error;
        }     
    	BRAP_P_ShowCitSettings(&(hDspCh->sFwTaskInfo[i].sProcessingNw), psCitInputStruct);

        /* Call CIT module */
        BDBG_MSG(("sCitOutput = %#x",&(hFwTaskCreate->sCitOutput)));
        BDBG_MSG(("Calling CIT Generation Module"));

        err =BRAP_P_VideoGenCit(hRap->hHeap, hRap->hRegister, psCitInputStruct, psVDecodeBuffCfg,&(hFwTaskCreate->sVideoCitOutput));
        if(BERR_SUCCESS != err)
        {
            BDBG_ERR(("ERROR returned from Cit module %d!",err));
            err =BERR_TRACE(err);
            goto error;
        }

                                                 
        
        for(j=0; j< BRAP_P_MAX_FW_BRANCH_PER_FW_TASK; j++)
        {
            if(BRAP_P_IsPointerValid((void *)psCitInputStruct->pBranchInfo[j]))
            {
                BKNI_Free(psCitInputStruct->pBranchInfo[j]);
                psCitInputStruct->pBranchInfo[j] = NULL;
            }
        }

        BDBG_MSG(("GENCIT DONE"));

        /*	Analyze the CIT generated : First level Information */
        BRAP_P_AnalyseVideoCit(hRap->hHeap, &(hFwTaskCreate->sVideoCitOutput.sVideoCit));
        BDBG_MSG(("ANALYSECIT DONE"));
		
        /* Download CIT structure into DRAM */
        err = BRAP_P_CopyDataToDram(
                hDsp->hHeap, 
                (uint32_t *)(&(hFwTaskCreate->sVideoCitOutput.sVideoCit)),      
                hRap->sMemInfo.sTskMemInfo[hDsp->uiDspIndex].sCitStruct[BRAP_GET_TASK_INDEX(hFwTaskCreate->uiTaskId)].ui32BaseAddr,
                hRap->sMemInfo.sTskMemInfo[hDsp->uiDspIndex].sCitStruct[BRAP_GET_TASK_INDEX(hFwTaskCreate->uiTaskId)].ui32Size
                );

        if(BERR_SUCCESS != err)
        {
               BDBG_ERR(("Error in Copying data to DRAM"));
               err = BERR_TRACE(err);
               goto error;
        }           

        /* Initialize interframe buffers for all the nodes */

        err = BRAP_VID_P_InitInterframeBuffer(hFwTaskCreate);  
        if(BERR_SUCCESS != err)
        {
               BDBG_ERR(("Error in initializing Interframe buffers for Task id %d",hFwTaskCreate->uiTaskId));
               err = BERR_TRACE(err);
               goto error;
        }

        /* Initialize configuration parameters for all the nodes */

		
        ui32NumBranchesValid = hFwTaskCreate->sCitOutput.ui32NumBranches;
        for(uiBranchId = 0; uiBranchId < ui32NumBranchesValid; uiBranchId++)
        {
            /* Get the configuration buffer address for this stage */
            ui32NumStagesValid = hFwTaskCreate->sCitOutput.sCitBranchInfo[uiBranchId].ui32NumStages;
            for(uiStageId = 0; uiStageId < ui32NumStagesValid; uiStageId++)
            {
                   
            }        
        }       

        if(false == bWdgRecovery)
        {
        /* Find free Fifo Ids */
        for(j=0;j<BRAP_DSP_P_NUM_FIFOS;j++)
        {
            if(false == hDsp->bFifoUsed[j])
                break;
        }
        if(BRAP_DSP_P_NUM_FIFOS == j)
        {
            BDBG_ERR(("Unable to find free fifo for SYNC MSG!"));
            err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            goto error;
        }
        else
        {
            hDsp->bFifoUsed[j] = true;
            ui32SyncFifo = j;
        }
        for(j=0;j<BRAP_DSP_P_NUM_FIFOS;j++)
        {
            if(false == hDsp->bFifoUsed[j])
                break;
        }
        if(BRAP_DSP_P_NUM_FIFOS == j)
        {
            BDBG_ERR(("Unable to find free fifo for ASYNC MSG!"));
            err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            goto error;
        }
        else
        {
            hDsp->bFifoUsed[j] = true;
            ui32AsyncFifo = j;
        }
        
        for(j=0;j<BRAP_MAX_FW_TSK_SUPPORTED;j++)
        {
                if(BRAP_FWIF_P_INVALID_TSK_ID == hRap->sMemInfo.sTskMemInfo[hDsp->uiDspIndex].sTskQueue[j].ui32TaskId)
                break;
        }
        if(BRAP_MAX_FW_TSK_SUPPORTED == j)
        {
            BDBG_ERR(("Unable to find free queues for this task!"));
            err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            goto error;
        }
        else
        {
            ui32QueueId = j;
        }
            hRap->sMemInfo.sTskMemInfo[hDsp->uiDspIndex].sTskQueue[ui32QueueId].ui32TaskId = hFwTaskCreate->uiTaskId;
            hRap->sMemInfo.sTskMemInfo[hDsp->uiDspIndex].sTskQueue[ui32QueueId].sTskSyncQueue.ui32FifoId
                = ui32SyncFifo;
            hRap->sMemInfo.sTskMemInfo[hDsp->uiDspIndex].sTskQueue[ui32QueueId].sTskAsyncQueue.ui32FifoId
                = ui32AsyncFifo;
        }
        else
        {
            for(j=0;j<BRAP_MAX_FW_TSK_SUPPORTED;j++)
            {
                if(hFwTaskCreate->uiTaskId == hRap->sMemInfo.sTskMemInfo[hDsp->uiDspIndex].sTskQueue[j].ui32TaskId)
                    break;
            }
            if(BRAP_MAX_FW_TSK_SUPPORTED == j)
            {
                BDBG_ERR(("Unable to find matching queues for this task in the handle for watchdog recovery!"));
                err = BERR_TRACE(BRAP_ERR_BAD_DEVICE_STATE);
                goto error;
            }
            else
            {
                ui32QueueId = j;
            }
        }

        if(true == bWdgRecovery)
        {
               hMsgQueue = hFwTaskCreate->hSyncMsgQueue;
        }
            
            err = BRAP_FWIF_P_CreateMsgQueue(&(hRap->sMemInfo.sTskMemInfo[hDsp->uiDspIndex].sTskQueue[ui32QueueId].sTskSyncQueue), 
                hDspCh->hHeap, hDspCh->hRegister, hDsp->ui32Offset, &hMsgQueue,bWdgRecovery);
            if(BERR_SUCCESS != err)
            {
                BDBG_ERR(("BRAP_DSPCHN_P_Start: SYNC RSP queue creation failed!"));
                err = BERR_TRACE(err);
                goto error;
            }
           if(false == bWdgRecovery)
            {
                /* Fill up task handle message queue */
                hFwTaskCreate->hSyncMsgQueue = hMsgQueue;
            }

            if(true == bWdgRecovery)
            {
                   hMsgQueue = hFwTaskCreate->hAsyncMsgQueue;
            }
            err = BRAP_FWIF_P_CreateMsgQueue(&(hRap->sMemInfo.sTskMemInfo[hDsp->uiDspIndex].sTskQueue[j].sTskAsyncQueue), 
                    hDspCh->hHeap, hDspCh->hRegister, hDsp->ui32Offset, &hMsgQueue,bWdgRecovery);
            if(BERR_SUCCESS != err)
            {
                BDBG_ERR(("BRAP_DSPCHN_P_Start: ASYNC RSP queue creation failed!"));
                err = BERR_TRACE(err);
                goto error;
            }
           if(false == bWdgRecovery)
            {
                /* Fill up task handle message queue */
                hFwTaskCreate->hAsyncMsgQueue = hMsgQueue;
            }        
           if(false == bWdgRecovery)
            {
               hFwTaskCreate->pAsyncMsgMemory = 
                (void *)(hRap->sMemInfo.sTskMemInfo[hDsp->uiDspIndex].sTskQueue[j].sAsyncMsgBufmem.ui32BaseAddr);
            }        


        /* Unmask interrupt bits corresponding to synchronous and asynchronous msg queues */
        err = BRAP_P_UnmaskTask(hFwTaskCreate);
        if(BERR_SUCCESS != err)
        {
            BDBG_ERR(("BRAP_DSPCHN_P_Start: Unable to unmask task id interrupt!"));
            err = BERR_TRACE(err);
            goto error;
        }

        /* Send Start Task Command */
        sCommand.sCommandHeader.ui32CommandID = BRAP_FWIF_P_START_TASK_COMMAND_ID;
        sCommand.sCommandHeader.ui32CommandCounter = 0;
        sCommand.sCommandHeader.ui32TaskID = hFwTaskCreate->uiTaskId;
        sCommand.sCommandHeader.eResponseType = BRAP_FWIF_P_ResponseType_eResponseRequired;

        sCommand.uCommand.sStartTask.eTaskAlgoType = BRAP_FWIF_P_AlgoType_eVideoDecode; 
        sCommand.uCommand.sStartTask.eDeadlineComputationFuncType =  BRAP_FWIF_P_DeadLineComputeFuncType_eNonRealtimeEncode;
               
        sCommand.uCommand.sStartTask.eTaskType = BRAP_FWIF_P_TaskType_eSoftRealtime;
        
        BRAP_P_ConvertAddressToOffset(hRap->hHeap, 
            (void *)(hRap->sMemInfo.sTskMemInfo[hDsp->uiDspIndex].sTskInfo[BRAP_GET_TASK_INDEX(hFwTaskCreate->uiTaskId)].ui32BaseAddr),
            &ui32PhysAddr);
        sCommand.uCommand.sStartTask.ui32DramDeadlineConfigStructAddr = ui32PhysAddr;

        BRAP_P_ConvertAddressToOffset(hRap->hHeap, 
            (void *)(hRap->sMemInfo.sTskMemInfo[hDsp->uiDspIndex].sCitStruct[BRAP_GET_TASK_INDEX(hFwTaskCreate->uiTaskId)].ui32BaseAddr),
            &ui32PhysAddr);
        sCommand.uCommand.sStartTask.ui32DramTaskConfigAddr = ui32PhysAddr;
        sCommand.uCommand.sStartTask.ui32SyncQueueId = hFwTaskCreate->hSyncMsgQueue->ui32FifoId;
        sCommand.uCommand.sStartTask.ui32AsyncQueueId = hFwTaskCreate->hAsyncMsgQueue->ui32FifoId;

        BRAP_P_ConvertAddressToOffset(hRap->hHeap, 
            (void *)(hFwTaskCreate->sVideoCitOutput.sStackSwapBuff.ui32DramBufferAddress),
            &ui32PhysAddr);
        sCommand.uCommand.sStartTask.sDramStackBuffer.ui32DramBufferAddress = ui32PhysAddr;
        sCommand.uCommand.sStartTask.sDramStackBuffer.ui32BufferSizeInBytes = hFwTaskCreate->sVideoCitOutput.sStackSwapBuff.ui32BufferSizeInBytes;
        
        BRAP_P_ConvertAddressToOffset(hRap->hHeap, 
            (void *)(hRap->sMit.sAlgoDwnldDetail[BRAP_AF_P_AlgoId_eSystemVideoTask].ui32AlgoCodePtr),
            &ui32PhysAddr);

        sCommand.uCommand.sStartTask.sDramTaskCodeBuffer.ui32DramBufferAddress = ui32PhysAddr;
        sCommand.uCommand.sStartTask.sDramTaskCodeBuffer.ui32BufferSizeInBytes= hRap->sMit.sAlgoDwnldDetail[BRAP_AF_P_AlgoId_eSystemVideoTask].ui32AlgoCodeSize;
        
        /* Fill up start task parameters */
        pTaskParams = (BRAP_FWIF_P_TaskParamInfo *)(hRap->sMemInfo.sTskMemInfo[hDsp->uiDspIndex].sTskInfo[BRAP_GET_TASK_INDEX(hFwTaskCreate->uiTaskId)].ui32BaseAddr);
/*        err = BRAP_P_ConvertSR(psAudioParams->sExtAudioParams.eOutputSampleRate, &(pTaskParams->ui32SamplingFrequency));*/
        err = BRAP_P_ConvertSR(BAVC_AudioSamplingRate_e48k, (unsigned int *)&(pTaskParams->ui32SamplingFrequency));
        BDBG_MSG(("pTaskParams->ui32SamplingFrequency > %x", pTaskParams->ui32SamplingFrequency));
        if(BERR_SUCCESS != err)
        {
            BDBG_ERR(("BRAP_DSPCHN_P_Start: Convert SR Failed!"));
            err = BERR_TRACE(err);
            goto error;
        }

        pTaskParams->ui32Threshold = ui32Threshold;
        pTaskParams->ui32BlockTime =ui32BlockTime; 

        pTaskParams->sNonRealTimeTaskParams.ui32MaxTimeSlice = 1350;
        pTaskParams->sNonRealTimeTaskParams.ui32MinTimeThreshold = 45;                

        /*TODO : ui32FrameSize is not required anymore by FW. remove this parameter */
        pTaskParams->ui32FrameSize = 1536;
        pTaskParams->eBufferType = BRAP_AF_P_BufferType_eFMM;

        ui32RbufOffset = (BCHP_AUD_FMM_BF_CTRL_RINGBUF_1_BASEADDR - BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_BASEADDR) *
            hDspCh->sNetworkInfo.sDspOutConfig[0].sOutputBufConfig.uIoBuffer.sIoBuffId.ui32BufferId[0] + BCHP_PHYSICAL_OFFSET;
        BDBG_MSG(("hDspCh->sNetworkInfo.sDspOutConfig[0].sOutputBufConfig.uIoBuffer.sIoBuffId.ui32BufferId[0] > %x", hDspCh->sNetworkInfo.sDspOutConfig[0].sOutputBufConfig.uIoBuffer.sIoBuffId.ui32BufferId[0]));

        pTaskParams->sCircBuffer.ui32BaseAddr = BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_BASEADDR + ui32RbufOffset;
        BDBG_MSG(("psAudioParams->sDecOrPtParams.sDecPathFmmParams.rBufIndex[0] > %x", psAudioParams->sDecOrPtParams.uDecBufFmmParams.sDecPathFmmParams.rBufIndex[0]));
        BDBG_MSG(("pTaskParams->sCircBuffer.ui32BaseAddr > %x", pTaskParams->sCircBuffer.ui32BaseAddr));
        pTaskParams->sCircBuffer.ui32EndAddr = BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_ENDADDR + ui32RbufOffset;
        pTaskParams->sCircBuffer.ui32ReadAddr = BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR + ui32RbufOffset;
        pTaskParams->sCircBuffer.ui32WriteAddr = BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_WRADDR + ui32RbufOffset;
        pTaskParams->sCircBuffer.ui32WrapAddr = BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_ENDADDR + ui32RbufOffset;


        /* Enable required asynchronous event notification */        
        hFwTaskCreate->hDsp->ui32EventIdMatrix[BRAP_GET_TASK_INDEX(hFwTaskCreate->uiTaskId)] |= BRAP_FWIF_P_DEFAULT_EVENTS_ENABLE_MASK;
        if(hDspCh->bChSpecificDspCh == true)
        {
            hFwTaskCreate->hDsp->ui32EventIdMatrix[BRAP_GET_TASK_INDEX(hFwTaskCreate->uiTaskId)] |= hDspCh->uHandle.hRapCh->ui32AsynIntMask;
        }

        sCommand.uCommand.sStartTask.ui32EventEnableMask = hFwTaskCreate->hDsp->ui32EventIdMatrix[BRAP_GET_TASK_INDEX(hFwTaskCreate->uiTaskId)];
        BDBG_MSG(("hFwTaskCreate->hDsp->ui32EventIdMatrix > %x", hFwTaskCreate->hDsp->ui32EventIdMatrix[BRAP_GET_TASK_INDEX(hFwTaskCreate->uiTaskId )]));

        hFwTaskCreate->uiLastEventType = sCommand.sCommandHeader.ui32CommandID;     
#if 1
        BRAP_P_EventReset(hDsp,BRAP_GET_TASK_INDEX(hFwTaskCreate->uiTaskId));
        /* End Fill up start task parameters */

        err = BRAP_FWIF_P_SendCommand(hDsp->hCmdQueue, &sCommand,hRap,hFwTaskCreate);
        /*Accept the other Commands , After posting Start task Command */

        
        if(BERR_SUCCESS != err)
        {
            if(hRap->bWatchdogTriggered == false)
            {        
                BDBG_ERR(("BRAP_DSPCHN_P_Start: START_TASK creation failed!"));
                err = BERR_TRACE(err);
                BDBG_ASSERT(0);
                goto error;
            }
            else
                err = BERR_SUCCESS;              
        }
        /* Wait for Ack_Response_Received event w/ timeout */
        err = BRAP_P_EventWait(hDsp, BRAP_DSPCHN_P_START_STOP_EVENT_TIMEOUT_IN_MS,BRAP_GET_TASK_INDEX(hFwTaskCreate->uiTaskId));
        if(BERR_TIMEOUT == err)
        {
            if((hRap->bWatchdogTriggered == false))
            {
                /* Please note that, If the code reaches at this point then there is a potential Bug in Fw 
                code which needs to be debugged. However Watchdog is being triggered to recover the system*/            
                BDBG_WRN(("BRAP_DSPCHN_P_Start: START_TASK ACK timeout! Triggering Watchdog"));
#if 0                
                BDBG_ASSERT(0);                
#endif
                BRAP_Write32(hDsp->hRegister, BCHP_AUD_DSP_INTH0_R5F_SET+ hDsp->ui32Offset,0x1);
                hRap->bWatchdogTriggered  = true;
#if 0                
            err = BERR_TRACE(err);
            goto error;
#endif                 
                err = BERR_SUCCESS;              
            }
            else
                err = BERR_SUCCESS;              
        }
/* Send command for the task , Only if the ack for the Start of the task is recieved */
        hFwTaskCreate->bStopped = false;            


        eMsgType = BRAP_P_MsgType_eSyn;
        
        if(hRap->bWatchdogTriggered == false)
        {
            err = BRAP_FWIF_P_GetMsg(hFwTaskCreate->hSyncMsgQueue, (void *)&sRsp,eMsgType);
        }

        if(BERR_SUCCESS != err)
        {
            if(hRap->bWatchdogTriggered == false)
            {
                BDBG_ERR(("BRAP_DSPCHN_P_Start: Unable to read ACK!"));
                err = BERR_TRACE(err);
                goto error;
            }
            else
                err = BERR_SUCCESS;              
        }
        if((sRsp.sCommonAckResponseHeader.eStatus != BERR_SUCCESS)||
            (sRsp.sCommonAckResponseHeader.ui32ResponseID != BRAP_FWIF_P_START_TASK_RESPONSE_ID)||
            (sRsp.sCommonAckResponseHeader.ui32TaskID != hFwTaskCreate->uiTaskId))
        {
            if((hRap->bWatchdogTriggered == false))
            {
               BDBG_ERR(("BRAP_DSPCHN_P_Start: START_TASK ACK not received successfully!eStatus = %d , ui32ResponseID = %d , ui32TaskID %d ",
                    sRsp.sCommonAckResponseHeader.eStatus,sRsp.sCommonAckResponseHeader.ui32ResponseID,sRsp.sCommonAckResponseHeader.ui32TaskID));
                err = BERR_TRACE(sRsp.sCommonAckResponseHeader.eStatus);
                goto error;
            }
            else
                err = BERR_SUCCESS;              
        }      
#endif        
        }
error:     
    if(false == bWdgRecovery)
    {
        hDspCh->eChState = BRAP_DSPCHN_P_ChState_eStart;     
    }
        
   
    if(psCitInputStruct)
    BKNI_Free(psCitInputStruct);
    if(psVDecodeBuffCfg)
    BKNI_Free(psVDecodeBuffCfg);               
    if(psAudProcNetwork)
    BKNI_Free(psAudProcNetwork);
    
    BDBG_LEAVE( BRAP_DSPCHN_P_Start );
    return err;    
}

BERR_Code BRAP_DSPCHN_P_StopVideo(
	BRAP_DSPCHN_Handle	pDspChn					/* [in] DSPCHN handle */
	)
{
  	BERR_Code err = BERR_SUCCESS;
    BRAP_FWIF_P_FwTaskHandle hFwTask = NULL;
    BRAP_DSP_Handle hDsp; 
    BRAP_FWIF_P_Command sCommand;
    BRAP_Handle         hRap;
    int         i=0,j=0;
    BRAP_P_MsgType  eMsgType;
    BRAP_FWIF_P_Response sRsp;

    BDBG_ENTER(BRAP_DSPCHN_P_StopVideo);

    /* Assert on the arguments */
    BDBG_ASSERT(pDspChn);

    if(pDspChn->eChState == BRAP_DSPCHN_P_ChState_eStop)
    {
        BDBG_MSG(("BRAP_DSPCHN_P_Stop: hDspChn 0x%x is already stoped", pDspChn));
        return err;
    }
       
    BKNI_Memset((void *)&sRsp,0,sizeof(BRAP_FWIF_P_Response));
       
    if(true == pDspChn->bChSpecificDspCh)
    {
        hRap = pDspChn->uHandle.hRapCh->hRap;
    }
    else
    {
        hRap = pDspChn->uHandle.hAssociation->hRap;
    }
    
    BRAP_DSPCHN_P_FreeStartTimeFWCtxt(pDspChn);

/*    if((true == pDspChn->bChSpecificDspCh)&&
        (true == pDspChn->sDspAudioParams.bPlayback))*/
    if(true == pDspChn->bChSpecificDspCh)
    {
        unsigned int uiPth = 0, uiPathIndx = 0;
        bool bFoundPath = false;
        BRAP_RM_P_ResrcGrant *psRsrcGrnt = NULL;
        for(uiPth = 0; uiPth < BRAP_P_MAX_PATHS_IN_A_CHAN; uiPth++)
        {
            if((BRAP_P_IsPointerValid((void *)pDspChn->uHandle.hRapCh->pPath[uiPth]))
                && (pDspChn->uHandle.hRapCh->pPath[uiPth]->hDspCh == pDspChn))
                {
                    uiPathIndx = uiPth;
                    bFoundPath = true;                    
                    break;
                }
        }
        if(uiPth >= BRAP_P_MAX_PATHS_IN_A_CHAN)
        {
            BDBG_ERR(("No sPath allocated for the running DspCh"));
    		return BERR_TRACE(err); 
        }
    	psRsrcGrnt = ( BRAP_RM_P_ResrcGrant *) BKNI_Malloc( sizeof( BRAP_RM_P_ResrcGrant ));
    	if ( NULL==psRsrcGrnt )
    	{
    			return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    	}
	BKNI_Memset( psRsrcGrnt, 0, sizeof(BRAP_RM_P_ResrcGrant) ); 
    
        BRAP_RM_P_InitResourceGrant(psRsrcGrnt,true);
/*        BKNI_Memcpy((void *)(volatile void*)psRsrcGrnt,
            (void *)(&(pDspChn->uHandle.hRapCh->pPath[uiPathIndx]->sRsrcGrnt)),
            sizeof(BRAP_RM_P_ResrcGrant));*/
        psRsrcGrnt->uiDspId = pDspChn->uHandle.hRapCh->pPath[uiPathIndx]->sRsrcGrnt.uiDspId;
        psRsrcGrnt->uiDspContextId = pDspChn->uHandle.hRapCh->pPath[uiPathIndx]->sRsrcGrnt.uiDspContextId;
        err = BRAP_RM_P_FreeResources(hRap->hRm, psRsrcGrnt,true);
        if(BERR_SUCCESS != err)
        {
            BDBG_ERR(("Unable to free allocated DSP"));
            BKNI_Free(psRsrcGrnt);
    		return BERR_TRACE(err); 
        }
        pDspChn->uHandle.hRapCh->pPath[uiPathIndx]->sRsrcGrnt.uiDspId = BRAP_RM_P_INVALID_INDEX;
        pDspChn->uHandle.hRapCh->pPath[uiPathIndx]->sRsrcGrnt.uiDspContextId = BRAP_RM_P_INVALID_INDEX;
        BKNI_Free(psRsrcGrnt);
    }


    for(i=0; i<BRAP_FWIF_P_MAX_FW_TASK_PER_DSPCHN; i++)
    {
        hFwTask = pDspChn->sFwTaskInfo[i].hFwTask;

        if(!(BRAP_P_IsPointerValid((void *)hFwTask)))
            continue;
        if(hFwTask->bStopped == true)
        {
            BDBG_MSG(("Task Already stopped"));
            continue;
        }
        hDsp = hFwTask->hDsp;

        /*Prepare command to stop the task */
        sCommand.sCommandHeader.ui32CommandID = BRAP_FWIF_P_STOP_TASK_COMMAND_ID;
        sCommand.sCommandHeader.ui32CommandCounter = 0;
        sCommand.sCommandHeader.ui32TaskID = hFwTask->uiTaskId;
        sCommand.sCommandHeader.eResponseType = BRAP_FWIF_P_ResponseType_eResponseRequired;

        hFwTask->uiLastEventType = sCommand.sCommandHeader.ui32CommandID;
        BRAP_P_EventReset(hDsp,BRAP_GET_TASK_INDEX(hFwTask->uiTaskId));
        err = BRAP_FWIF_P_SendCommand(hDsp->hCmdQueue, &sCommand,hRap,hFwTask);            
        if(BERR_SUCCESS != err)
        {
            if(hRap->bWatchdogTriggered == false)
            {        
                BDBG_ERR(("BRAP_DSPCHN_P_Stop: STOP_TASK creation failed!"));

                err = BERR_TRACE(err);            
                goto freetask;    
                   
             }
            else
                err = BERR_SUCCESS;   
        }

        /* Wait for Ack_Response_Received event w/ timeout */
        err = BRAP_P_EventWait(hDsp, BRAP_DSPCHN_P_START_STOP_EVENT_TIMEOUT_IN_MS,BRAP_GET_TASK_INDEX(hFwTask->uiTaskId));
        if(BERR_TIMEOUT == err)
        {
            if(hRap->bWatchdogTriggered == false)
            {        
                /* Please note that, If the code reaches at this point then there is a potential Bug in Fw 
                code which needs to be debugged. However Watchdog is being triggered to recover the system*/
                BDBG_WRN(("BRAP_DSPCHN_P_Stop: STOP_TASK Timeout! Triggering watchdog"));
#if 0                
                BDBG_ASSERT(0);                
#endif
                BRAP_Write32(hDsp->hRegister, BCHP_AUD_DSP_INTH0_R5F_SET+ hDsp->ui32Offset,0x1);
                hRap->bWatchdogTriggered  = true;
#if 0                
                err = BERR_TRACE(err);            
                goto freetask;    
#endif                 
                err = BERR_SUCCESS;                     
             }
            else
                err = BERR_SUCCESS;             
        }
        
        eMsgType = BRAP_P_MsgType_eSyn;
        if(hRap->bWatchdogTriggered == false)
        {
            err = BRAP_FWIF_P_GetMsg(hFwTask->hSyncMsgQueue, (void *)&sRsp,eMsgType);
        }
        if(BERR_SUCCESS != err)
        {
            if(hRap->bWatchdogTriggered == false)
            {
                BDBG_ERR(("BRAP_DSPCHN_P_Stop: Unable to read ACK!"));
                err = BERR_TRACE(err);            
                goto freetask;                     
            }
            else
                err = BERR_SUCCESS;             
        }
        if((sRsp.sCommonAckResponseHeader.eStatus != BERR_SUCCESS)||
            (sRsp.sCommonAckResponseHeader.ui32ResponseID != BRAP_FWIF_P_STOP_TASK_RESPONSE_ID)||
            (sRsp.sCommonAckResponseHeader.ui32TaskID != hFwTask->uiTaskId))
        {
            if(hRap->bWatchdogTriggered == false)
            {
                BDBG_ERR(("BRAP_DSPCHN_P_Stop: STOP_TASK ACK not received successfully!"));
                err = BERR_TRACE(err);            
                goto freetask;                     
    
            }
            else
                err = BERR_SUCCESS;             
        }
        
        /*Mask the task */
        err = BRAP_P_MaskTask(hFwTask);
        if(BERR_SUCCESS != err)
        {
            BDBG_ERR(("BRAP_DSPCHN_P_Stop: Unable to mask task id interrupt!"));
            err = BERR_TRACE(err);            
            goto freetask;
        }
        /* Destroy synchronous and asynchronous queue of Task */
        hDsp->bFifoUsed[hFwTask->hSyncMsgQueue->ui32FifoId] = false;
        err = BRAP_FWIF_P_DestroyMsgQueue(hFwTask->hSyncMsgQueue,hDsp);
        if(BERR_SUCCESS != err)
        {
            BDBG_ERR(("BRAP_DSPCHN_P_Stop: SYNC queue destroy failed!"));
            err = BERR_TRACE(err);            
            goto freetask;            
        }

        hDsp->bFifoUsed[hFwTask->hAsyncMsgQueue->ui32FifoId] = false;
        err = BRAP_FWIF_P_DestroyMsgQueue(hFwTask->hAsyncMsgQueue,hDsp);
        if(BERR_SUCCESS != err)
        {
            BDBG_ERR(("BRAP_DSPCHN_P_Stop: ASYNC queue destroy failed!"));
            err = BERR_TRACE(err);
            goto freetask;            
        }
freetask:
        for(j=0;j<BRAP_MAX_FW_TSK_SUPPORTED;j++)
        {
            if(hRap->sMemInfo.sTskMemInfo[hDsp->uiDspIndex].sTskQueue[j].ui32TaskId == hFwTask->uiTaskId)
            {
                hRap->sMemInfo.sTskMemInfo[hDsp->uiDspIndex].sTskQueue[j].ui32TaskId = BRAP_FWIF_P_INVALID_TSK_ID;
                hFwTask->pAsyncMsgMemory = NULL;
                hFwTask->uiTaskId = BRAP_FWIF_P_INVALID_TSK_ID;
                hFwTask->bStopped = true;
           }
        }
        if(BERR_SUCCESS != err)
        {
            return err;
        }
    }

    
    pDspChn->eChState = BRAP_DSPCHN_P_ChState_eStop; 
    
    BDBG_LEAVE(BRAP_DSPCHN_P_StopVideo);
    return err;
}	
#endif
