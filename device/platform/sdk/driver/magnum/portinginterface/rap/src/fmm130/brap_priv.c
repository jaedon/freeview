/***************************************************************************
*     Copyright (c) 2004-2011, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brap_priv.c $
* $brcm_Revision: Hydra_Software_Devel/224 $
* $brcm_Date: 8/16/11 3:27p $
*
* Module Description:
*	This file contains the common APIs used internally by different 
*	modules of the Raptor Audio PI. 
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/rap/base/brap_priv.c $
* 
* Hydra_Software_Devel/224   8/16/11 3:27p srajapur
* SW7400-3010 : [7400] Added 7.1 channelmap support for DDP StreamInfo
* 
* Hydra_Software_Devel/223   5/10/10 5:38p srajapur
* SW7400-2688 : [7400] Added Support for BRAP_DSPCHN_AudioType_ePcmWav
* algorithm
* 
* Hydra_Software_Devel/222   12/31/09 2:41p srajapur
* SW7400-2602 : [7400] Support DSOLA PB80 / PB120 on BCM7400
* 
* Hydra_Software_Devel/221   12/15/09 9:28a srajapur
* SW7400-2602 : [7400] Support DSOLA PB80 / PB120 on BCM7400
* 
* Hydra_Software_Devel/220   11/21/09 6:59a srajapur
* SW7400-2534 : [7400] DDP test streams fail to play after initial
* playback. HDMI audio has severe static.  --> Changed the programming
* of REP_PERIOD as per the standard for DDP.
* 
* Hydra_Software_Devel/219   7/29/09 5:29p srajapur
* PR53240 : [7403] DRA decoder development on 7403 chip
* 
* Hydra_Software_Devel/218   7/21/09 5:28p srajapur
* PR53240 : [7403] DRA decoder development on 7403 chip
* 
* Hydra_Software_Devel/217   6/12/09 6:10p srajapur
* PR53489: [7401,7400]  Added support for SRS True Volume certification
* and added PCM decode algorithm
* 
* Hydra_Software_Devel/216   4/28/09 5:11p srajapur
* PR53814 : [7400] Dual-Display: VC1_ASF, and VC1_SM_ASF - Seg-Fault on
* playback stop.
* --> Increase the Interstage o/p memory for WMAPR0 algorithm
* 
* PR53672 : [7400] System crash after stopping WMA-PRO playback, after
* performing any brutus operation on live stream, in dual-decode mode.
* --> Increase the Interstage o/p memory for WMAPR0 algorithm
* 
* Hydra_Software_Devel/215   4/6/09 6:45p srajapur
* PR 43833 : [7400]  Updated the userconfig parameters as per firmware
* requirements for AAC-HE v2
* 
* Hydra_Software_Devel/214   3/18/09 9:08p srajapur
* PR 52051 : [7400]  Add a config parameter to the decoder for dialog
* normalization enable / disable
* 
* Hydra_Software_Devel/213   3/18/09 8:39p srajapur
* PR53363 : [7400] Adding DTS passthrough support
* 
* Hydra_Software_Devel/212   3/18/09 7:57p srajapur
* PR43833 : [7400]  BCM7400 support codec for AAC HE V2
* 
* Hydra_Software_Devel/211   3/11/09 11:39a srajapur
* PR 50778 : [7400,7403] Add mono downmix support for all decoders
* --->Updated memory requirement for  mono downmix support feature
* addition.
* 
* Hydra_Software_Devel/210   2/25/09 6:19p srajapur
* PR52480: [7400] Name changing SRS VIQ to SRS TrueVolume
* 
* Hydra_Software_Devel/209   2/20/09 3:13p srajapur
* PR52285 : [7400] PCM playback no audio or only play once
* ---> Programming STEP size for PCM playback.
* 
* PR52195 : [7400] no audio from 2nd PCM playback to DAC1.
* ---> Programming STEP size for PCM playback.
* 
* Hydra_Software_Devel/208   2/17/09 12:04p srajapur
* PR 50778 : [7400,7403] Add mono downmix support for all 7400 decoders
* --->Updated memory requirement for WMA mono downmix support feature
* addition.
* 
* Hydra_Software_Devel/207   2/16/09 12:22p srajapur
* PR 50778 : [7400,7403] Add mono downmix support for all 7400 decoders
* --->Updated memory requirement for WMA and AAC mono downmix support
* feature addition.
* 
* Hydra_Software_Devel/206   2/11/09 12:16p srajapur
* PR 51734: [7400] SRS VIQ certification
* --> Added two more user config params
* 
* Hydra_Software_Devel/205   1/13/09 12:11p srajapur
* PR50613 :[7400] Added SRS VIQ post processing algorithm for 7400
* --> Modified the memory requirement for SRS post processing algorithm.
* 
* Hydra_Software_Devel/204   1/12/09 11:00p srajapur
* PR50613 :[7400] Added SRS VIQ post processing algorithm for 7400
* 
* Hydra_Software_Devel/204   1/12/09 11:00p srajapur
* PR50613 :[7400] Added SRS VIQ post processing algorithm for 7400
* 
* Hydra_Software_Devel/203   1/8/09 11:36a srajapur
* PR 50778 : [7400,7403] Add mono downmix support for all 7400 decoders
* ---> Added mono downmix support for DVD LPCM algorithm
* 
* Hydra_Software_Devel/202   12/12/08 12:16p srajapur
* PR40958 : [7400] Water mark hardware interrupt doesn't work in Audio
* PCM Playback --> Added watermark hardware interrupt support
* 
* Hydra_Software_Devel/201   12/10/08 3:37p srajapur
* PR 48052 : [7401]  Coverity Defect ID:12273 OVERRUN_STATIC brap_priv.c
* Product=97401linux
* 
* Hydra_Software_Devel/200   11/26/08 8:59p srajapur
* PR 48054 : [7401] Coverity Defect ID:12271 OVERRUN_STATIC brap_priv.c
* Product=97401linux
* 
* Hydra_Software_Devel/199   11/26/08 8:35p srajapur
* PR 48052 : [7401]  Coverity Defect ID:12273 OVERRUN_STATIC brap_priv.c
* Product=97401linux
* 
* PR 48053 : [7401]  Coverity Defect ID:12272 OVERRUN_STATIC brap_priv.c
* Product=97401linux
* 
* Hydra_Software_Devel/198   11/26/08 8:20p srajapur
* PR 48051 : [7401] Coverity Defect ID:12274 OVERRUN_STATIC brap_priv.c
* Product=97401linux
* PR 48053 : [7401]  Coverity Defect ID:12272 OVERRUN_STATIC brap_priv.c
* Product=97401linux
* 
* ----> Issue fixed. checked the conditional for Output Audio channel
* type.
* 
* Hydra_Software_Devel/197   11/12/08 7:30p srajapur
* PR48893 : 74XX merged release for 7400,7401,7403 and 7118 [For more
* info see Description]
* PR45669: [7403] Adding support for DTS Certification.
* PR43467 : [7403] Implementation of DRA on 7403
* PR46472 : [7400] Implement Dolby Volume Leveler and Modeler per
* Motorola's Requirements
* PR42285 : [7401] Added Audio descriptor support
* PR44899 :[7401] Independent delay on spdif causes left and right
* channel to be out of phase
* PR44614 : [7400] [7401C1][NDS-CDI] Few seconds of noise on SPDIF/HDMI
* during PCM to Compressed & vice-a-versa transition
* PR42342: [7401] Added Downmix Method parameter in the AACHE user config
* structure for Sumitomo
* 
* Hydra_Software_Devel/196   7/4/08 5:20p sgadara
* PR 41579: [7403] Increasing Code+Table size for WMA_STD as firmware
* files have been updated for WMA Cetification.
* 
* Hydra_Software_Devel/195   5/19/08 9:23p srajapur
* PR42140 : [7400] Implemented  PPM adjustments as in 7401
* 
* Hydra_Software_Devel/194   5/6/08 8:03p srajapur
* PR42131: [7401,7403] Added support for AACHEV2
* 
* Hydra_Software_Devel/193   4/28/08 2:29p srajapur
* PR42131 : [7401] Added support for AACHEV2
* 
* Hydra_Software_Devel/192   4/15/08 5:14p srajapur
* PR38305 : [7401] Audio locks up on channel change test with independent
* delay configured
* - As this is a HW bug.To fixe this issue we have added a delay between
* stopping of OP and stopping source fifo during a channel change
* 
* Hydra_Software_Devel/191   4/10/08 12:29p srajapur
* PR39875 : [7401] Added support for PCM playback of sample 8k,11.025k
* and 12k.
* 
* Hydra_Software_Devel/190   2/26/08 3:54p srajapur
* PR34858: [7400,7118,7403,7401] Added API support to set the 41 bits of
* channel status in SPDIF
* 
* Hydra_Software_Devel/189   2/15/08 1:09p srajapur
* PR34858: [7400,7118,7403,7401] Added API support to set the 41 bits of
* channel status in SPDIF
* 
* Hydra_Software_Devel/188   2/6/08 7:39p srajapur
* PR39186 : [7400,7401,7403,7118] segmentation fault inf BRAP_P_SetCbit()
* - Initialized the  hDirChannels  array to NULL.
* 
* Hydra_Software_Devel/187   1/14/08 4:56p srajapur
* PR 25571 : [7401,7400,7403,7118] Changed the size of Interstage I/P and
* Interstage O/P buffers.
* 
* Hydra_Software_Devel/186   10/18/07 6:24p srajapur
* PR 36053 : [7401,7403,7118,7400] Modified the PI code to download AAC-
* HE F/W within the PI to support AAC algorithm.
* 
* Hydra_Software_Devel/185   10/15/07 12:31p srajapur
* PR36093  : [7401,7403,7118,7400] Changed Inter-frame and Dram Tables
* size  for WMA-Pro.
* 
* Hydra_Software_Devel/184   10/12/07 4:02p srajapur
* PR 36053 : [7401,7403,7118,7400] Modified the PI code to download AAC-
* HE F/W within the PI to support AAC algorithm.
* 
* Hydra_Software_Devel/183   10/11/07 4:02p bselva
* PR 35803:[7401,7118,7403,7400] The handling of Rbufsize given during
* Playback start is corrected in PI so that bLoopAroundEn flag is
* honoured properly.
* 
* Hydra_Software_Devel/182   10/10/07 7:32p srajapur
* PR 34858 : [7401,7403,7118,7400] Added API support to set the bit-32 of
* channel status in SPDIF.
* 
* Hydra_Software_Devel/181   10/10/07 5:55p gautamk
* PR23960: [7401] correcting the download/Alloc  function which called
* for open time download
* 
* Hydra_Software_Devel/180   9/27/07 4:38p bselva
* PR 34050:[7401,7118,7403,7400] Increasing the code + table size for AAC
* HE algo as per the request from F/W team.
* 
* Hydra_Software_Devel/179   8/31/07 3:19p bselva
* PR 34050:[7401,7403,7118,7400] Modified the pass through executable
* size to 4096 Bytes as per the request from F/W team
* 
* Hydra_Software_Devel/178   7/26/07 3:27p rjain
* PR 30026: [7401] Adding flag bMonoToAll to let app send downmixed mono
* audio to all output ports. We pass this to FW as an array in the user
* config parameters.
* 
* Hydra_Software_Devel/177   7/19/07 6:26p bselva
* PR 33176:[7400]Checkin the PI changes for phase 4.0 release
* 
* Hydra_Software_Devel/176   7/17/07 3:47p bselva
* PR 28274:[7403]Adding the support for DVD LPCM
* 
* Hydra_Software_Devel/175   7/12/07 10:23a gautamk
* PR32256: [7401] Adding Support for passthru in WMA PRO
* 
* Hydra_Software_Devel/174   7/11/07 12:23p bselva
* PR 32900: [7400]Fixing the compilation warning for 7400
* 
* Hydra_Software_Devel/173   7/11/07 12:13p rjain
* PR 32750: Handling Mai in BRAP_P_SetCbit ()
* 
* Hydra_Software_Devel/172   7/9/07 2:38p rjain
* PR 27546: [7401,7403,7118,7400] Make ADAPT_RATE_THRESHOLD default to 5
* 
* Hydra_Software_Devel/171   7/5/07 4:20p rjain
* PR 27546:  Always prorgam the enable bit for Adaptive rate control
* 
* Hydra_Software_Devel/170   7/5/07 4:06p gautamk
* PR32256: [7401] Adding new algo WMA PRO for 7401
* 
* Hydra_Software_Devel/169   6/28/07 3:00p rjain
* PR 30616: Fixing compilation on 7411
* 
* Hydra_Software_Devel/168   6/26/07 7:13p rjain
* PR 30616: Making user programmable channel status parameters
* programmable on-the-fly
* 
* Hydra_Software_Devel/167   6/21/07 3:24p rjain
* PR 28524: Fixing compile warnings
* 
* Hydra_Software_Devel/166   6/18/07 11:16a rjain
* PR 30576: correcting BDBG_ERR to BDBG_MSG
* 
* Hydra_Software_Devel/165   6/15/07 3:34p rjain
* PR 30576: Added debug messages.
* 
* Hydra_Software_Devel/164   5/31/07 6:06p bselva
* PR 26501: [7401 Family]Checking in the changes for AAC-HE multichannel
* support
* 
* Hydra_Software_Devel/163   5/30/07 7:18p gautamk
* PR27106: Fixing number of warnings in brap module
* 
* Hydra_Software_Devel/162   5/22/07 12:29p rjain
* PR 28422:[7401 family]  second round of changes for FSPB
* 
* Hydra_Software_Devel/161   5/15/07 4:07p rjain
* PR 28422:[7401 family] Checking in PI changes for FSPB and DSOLA [needs
* new 7401 MIT]
* 
* Hydra_Software_Devel/160   5/4/07 7:52p rjain
* PR 30346: [7401 family] new mit changes..
* 
* Hydra_Software_Devel/159   5/4/07 4:39p rjain
* PR 30436: [7401 family] First round of check ins for new mit. All
* changes for new MIT are within the macro BRAP_DSP_P_7401_NEWMIT. This
* macro is defined as 0 at present. It will be enabled once new MIT FW
* is also checked in.
* 
* Hydra_Software_Devel/158   4/24/07 1:59p rjain
* PR 28524: increasing SRC downloadable size
* 
* Hydra_Software_Devel/157   4/18/07 12:08p gautamk
* PR 23011: [All chips except 7411] Increasing Mpeg Table size to 24250
* for QSF support
* 
* Hydra_Software_Devel/156   4/16/07 5:16p gautamk
* PR28472: [7401,7400] Use #if BRAP_SECURE_HEAP for the changes to
* BRAP_P_AllocAligned(); BRAP_P_Free() and other securtiy related
* changes
* 
* Hydra_Software_Devel/155   4/12/07 2:12p bselva
* PR 27768:[7400 b0]Disabling the programing of Adaptive rate control for
* 7400 b0 as this is not supported in F/W for this chip
* 
* Hydra_Software_Devel/154   4/5/07 3:33p rjain
* PR 28524 : [All] The stop sequence had been changed for PR 23121.
* Updating the comments to reflect that.
* 
* Hydra_Software_Devel/153   3/13/07 3:55p rjain
* PR 23011: [All chips] Increasing MPEG interframe buffer size to 11172
* bytes
* 
* Hydra_Software_Devel/152   3/8/07 10:50a rjain
* PR 28524:[7401, 7118, 7403] code clean up: replacing (BCHP_CHIP ==
* 7401) || (BCHP_CHIP == 7118) || (BCHP_CHIP == 7403) with
* (BRAP_7401_FAMILY == 1) where appropriate
* 
* Hydra_Software_Devel/151   3/7/07 2:44p gautamk
* PR 28303: Removed build error  in brap_p_free()  for the chips , in
* which  BRAP_SECURE_HEAP is not defined.
* 
* Hydra_Software_Devel/150   3/6/07 3:47p rjain
* PR 28303: BRAP_P_Free() should not assert if its passed a null pointer,
* but should return cleanly. Refer PR 26454. Also cleaned up
* BRAP_P_Free().
* 
* Hydra_Software_Devel/149   2/28/07 9:07a rjain
* PR 24247: Porting LSF (with SRC) for 7118 and 7403
* 
* Hydra_Software_Devel/148   2/8/07 4:28p rjain
* PR 24247 : [7401] adding support for SRC as a post processing moidule
* for all algos
* 
* Hydra_Software_Devel/147   2/8/07 2:45p gautamk
* PR23960 : RAP ZSP FW must reside in a single block of read-only memory
* to support NDS SVP -- Removing Compilation error for other platforms
* not supporting NDS/SVP
* 
* Hydra_Software_Devel/146   2/7/07 3:14p gautamk
* PR23960 : RAP ZSP FW must reside in a single block of read-only memory
* to support NDS SVP
* 
* Hydra_Software_Devel/145   1/23/07 12:35p bselva
* PR 27078: Changes the F/W sizes and buffer sizes for 7400 Ac3
* executables as we are using version 2.0 code for this release.
* 
* Hydra_Software_Devel/144   1/2/07 4:30p rjain
* PR 26084: [7401/7400/7118] Changing the memory requirements for DDP
* v2.0  codec
* 
* Hydra_Software_Devel/143   12/20/06 12:10p gautamk
* PR26454 :BRAP_DSP_P_FreeMem should check for whether memory allocation
* was done or not before calling BRAP_P_Free
* 
* Hydra_Software_Devel/142   12/19/06 1:26p rjain
* PR 25670: add 7403 support to RAP
* 
* Hydra_Software_Devel/141   12/13/06 6:41p gautamk
* PR26454 :BRAP_DSP_P_FreeMem should check for whether memory allocation
* was done or not before calling BRAP_P_Free
* 
* Hydra_Software_Devel/140   12/13/06 3:51p rjain
* PR 25135: increasing size for DDP inter-stage interface buffer as
* requested
* 
* Hydra_Software_Devel/139   11/3/06 4:39p rjain
* PR 25397: adding support for C1
* 
* Hydra_Software_Devel/138   11/3/06 2:39p gautamk
* PR24342: resolving compiler warning for DEBUG=n build
* 
* Hydra_Software_Devel/137   10/31/06 2:44p rjain
* PR 22635: turning on PI support for new TSM  scheme
* 
* Hydra_Software_Devel/136   10/30/06 2:25p rjain
* PR 22635: Program adaptive rate control for the new TSM scheme
* 
* Hydra_Software_Devel/135   10/3/06 10:51a bselva
* PR 24468: Checking in the changes required for phase 3 features
* 
* Hydra_Software_Devel/134   9/28/06 3:12p rjain
* PR24347: 7118 adopts MEMC_0_ naming convention
* 
* Hydra_Software_Devel/133   8/28/06 11:51a bselva
* PR 18604: Fixing the compilation warning with DEBUG=n
* 
* Hydra_Software_Devel/132   8/24/06 11:27a bselva
* PR 22486: Checking in the changes for buffer sizes for latest F/W
* files.
* 
* Hydra_Software_Devel/131   8/23/06 12:22p bselva
* PR 23121: OP is disabled after DP in order to solve the switching from
* stereo to multichannel.
* 
* Hydra_Software_Devel/130   8/13/06 7:24p nitinb
* PR 23192: Need to remove all the compilation warnings in different chip
* builds with different build options
* 
* Hydra_Software_Devel/129   8/13/06 6:31p nitinb
* PR 21105: Corrected buffer sizes for WMA_STD
* 
* Hydra_Software_Devel/128   8/3/06 1:15p bselva
* PR 22486: Fixing the compilation error for 7411
* 
* Hydra_Software_Devel/127   8/2/06 5:59p bselva
* PR 22486: Merged the changes for multichannel and concurrent stereo
* downmix support for 7400 to main branch
* 
* Hydra_Software_Devel/126   7/24/06 6:42p rjain
* PR 22663: Updating memory usage table sAlgoMemRequirementArray[] for
* 7411 and 7401 for the algorithms MPEG1, AC3, AAC, AAC+, DDP.
* 
* Hydra_Software_Devel/125   7/3/06 8:18p nitinb
* PR 22443: Replace AC3 decoder with DDP decoder in AC3 Lossless codec
* 
* Hydra_Software_Devel/124   6/30/06 10:53a rjain
* PR22283: adding debug messages to BRAP_P_AllocAligned() and
* BRAP_P_Free()
* 
* Hydra_Software_Devel/123   6/26/06 12:50p kagrawal
* PR 22297: Fixed segmentation fault and corrected buffer sizes for LPCM
* 
* Hydra_Software_Devel/122   6/25/06 4:39p kagrawal
* PR 22297: Added support for stereo downmix path concurrent with
* multichannel path
* 
* Hydra_Software_Devel/121   6/21/06 8:19p rjain
* PR 22179: uOpSettings gets saved in hOp only on a start or on-the-fly
* change. So make sure no one accesses hOp->uOpSettings before a start.
* 
* Hydra_Software_Devel/120   6/19/06 4:51p rjain
* PR 21942: fixing compilation
* 
* Hydra_Software_Devel/119   6/19/06 4:36p rjain
* PR 21942: 7118 doesn't have bchp_memc_0.h, it has bchp_memc.h
* 
* Hydra_Software_Devel/118   6/16/06 3:31p bhanus
* PR 22118 : Removed compilation warning
* 
* Hydra_Software_Devel/117   6/16/06 10:55a bhanus
* PR 22118 : Added a check to make sure that channel is running when
* checked for compressed mixed with PCM.
* 
* Hydra_Software_Devel/116   6/13/06 6:44p bhanus
* PR 22118 : Added a check to make sure that channel is running when
* checked for compressed mixed with PCM.
* 
* Hydra_Software_Devel/115   6/13/06 2:54p nitinb
* PR 22083: Add support for MLP algorithm in 7411D0 RAP PI
* 
* Hydra_Software_Devel/114   6/7/06 5:17p nitinb
* PR 19615: Replacing BCHP_REV macros with BCHP_VER macro
* 
* Hydra_Software_Devel/113   6/2/06 1:08a nitinb
* PR 19615: Correcting buffer sizes for multichannel mode
* 
* Hydra_Software_Devel/112   5/31/06 11:13p nitinb
* PR 20117: Corrected Decoder config buffer sizes for LPCM algorithms
* 
* Hydra_Software_Devel/111   5/25/06 7:44p nitinb
* PR 21794: Implement audio SRC for 7411D0
* 
* Hydra_Software_Devel/110   5/25/06 4:50p nitinb
* PR 19615: For multichannel mode, codecs require non-zero size of
* interstage buffers
* 
* Hydra_Software_Devel/109   5/24/06 4:13p nitinb
* PR 19615: Added function to stop I2S2 and corresponding mixer
* 
* Hydra_Software_Devel/108   5/16/06 3:34p bselva
* PR 21388: Changed the code size for MPEG algo and Interframe and
* interstage buffer size for AC3 and DDP.
* 
* Hydra_Software_Devel/107   5/9/06 11:22a rjain
* PR 21481: adding support for 7118 to RAP PI
* 
* Hydra_Software_Devel/106   5/4/06 11:35a nitinb
* PR 21400: Add RAP support for DD in lossless for 7411D0
* 
* Hydra_Software_Devel/105   5/3/06 11:06p nitinb
* PR 21105: 7411D0 - Add support WMA Raptor Porting Interface
* 
* Hydra_Software_Devel/104   5/3/06 6:33p nitinb
* PR 19615: Support upto 8 channel audio using two I2S ports on 7411D0
* 
* Hydra_Software_Devel/103   4/28/06 3:24p rjain
* PR 20318: Adding Open count and start count to hSpdifFm so that if
* multiple channels use the same hSpdfiFm, it is handled correctly.
* 
* Hydra_Software_Devel/102   4/24/06 12:05p kagrawal
* PR 21159: Added a default case in BRAP_P_ConvertSR()
* 
* Hydra_Software_Devel/101   4/24/06 11:59a rjain
* PR 20123: Fixing handling of Flex as Mai Mux Select in raptor PI
* 
* Hydra_Software_Devel/100   4/24/06 11:49a kagrawal
* PR 21159: Replaced uiGetSamplingRate[] by BRAP_P_ConvertSR() to support
* all the BAVC_AudioSamplingRate enums
* 
* Hydra_Software_Devel/99   4/20/06 7:22p kagrawal
* PR 19361: Reverted back AM review comment implementation for SPDIFFM
* open
* 
* Hydra_Software_Devel/98   4/12/06 1:38p bselva
* PR 20318: Implemented the Audio Manager review coments
* 
* Hydra_Software_Devel/97   4/11/06 1:05p nitinb
* PR 20318: PR for Audio Manager code review related changes
* 
* Hydra_Software_Devel/96   4/6/06 5:56p rjain
* PR 20318: Implementing code review comments for AM
* 
* Hydra_Software_Devel/95   4/3/06 6:32p rjain
* PR 20318: Implementing code review comments for AM
* 
* Hydra_Software_Devel/94   3/30/06 6:59p kagrawal
* PR 20318: Implemented review comments for AM and Sampling Rate Change
* 
* Hydra_Software_Devel/93   3/30/06 5:11p bhanus
* PR 19360 : Removed BDBG_ERR statements.
* 
* Hydra_Software_Devel/92   3/30/06 4:40p bselva
* PR 19878: Checking in the changes required for 7400 phase 0.5 release
* 
* Hydra_Software_Devel/91   3/30/06 3:19p bhanus
* PR 19360 : Added a check to return error if PCM data is mixed with
* compressed data
* 
* Hydra_Software_Devel/90   3/30/06 2:26p kagrawal
* PR 20318: Implemented reveiw comments on Audio Manager
* 
* Hydra_Software_Devel/89   3/28/06 12:14p rnayak
* PR20439: Restore Bangalore Raptor files after magnum vob corruption on
* 17th March 2006
* 
* Hydra_Software_Devel/89   3/20/06 8:10p nitinb
* PR 20287: Implement DVD LPCM algorithm for 7411 D0
* 
* Hydra_Software_Devel/88   3/16/06 6:56p nitinb
* PR 20117: Implement dolby digital bass management algorithm
* 
* Hydra_Software_Devel/87   3/16/06 5:24p nitinb
* PR 20117: Implement dolby digital bass management algorithm
* 
* Hydra_Software_Devel/86   3/2/06 7:38p nitinb
* PR 19231: Replaced function BRAP_P_MuteChannelOutput_isr by pair of
* functions BRAP_P_MuteChannelOutputOnSr_isr and
* BRAP_P_UnMuteChannelOutputOnSr_isr
* 
* Hydra_Software_Devel/85   3/1/06 6:22p rjain
* PR 19946 :
* - Fixed some issues related to handling of Mai
* - Fixed some issues related to sampling rate changes for Mai, for
* cloned ports and for decode channels
* 
* Hydra_Software_Devel/84   2/28/06 7:13p nitinb
* PR 19923: Correcting AC3 passthru firmware download buffer size
* 
* Hydra_Software_Devel/83   2/27/06 4:38p rjain
* PR 19593: removing unsupported algo types
* BRAP_DSPCHN_AudioType_ePcmPlayback and BRAP_DSPCHN_AudioType_eDv25
* from enum  BRAP_DSPCHN_AudioType
* 
* Hydra_Software_Devel/82   2/27/06 2:13p rjain
* PR 19593: removing unsupported algo types
* BRAP_DSPCHN_AudioType_ePcmPlayback and BRAP_DSPCHN_AudioType_eDv25
* from enum  BRAP_DSPCHN_AudioType.
* 
* Hydra_Software_Devel/81   2/17/06 2:42p bselva
* PR 19168: Modified to treat Ac3 decode as DDP for 7400
* 
* Hydra_Software_Devel/80   2/17/06 10:08a sushmit
* PR18604: Merging Encoder to latest.
* 
* Hydra_Software_Devel/80   2/17/06 9:56a sushmit
* PR18604: Merging Encoder to latest.
* 
* Hydra_Software_Devel/79   2/13/06 11:37a bselva
* PR 19168: Increased the interframe buffer size for AC3 dual decode
* support
* 
* Hydra_Software_Devel/78   2/10/06 5:29p kagrawal
* PR 18264: Added BRAP_MemRead32_isr
* 
* Hydra_Software_Devel/77   2/8/06 10:14a bselva
* PR 19168: Checking in the changes for 7400 test release
* 
* Hydra_Software_Devel/76   2/8/06 5:23p kagrawal
* PR 19194: Added support for DTS-HD
* 
* Hydra_Software_Devel/75   2/7/06 10:12a bselva
* PR 19168: Checking in the changes done during 7400 Bringup
* 
* Hydra_Software_Devel/74   2/1/06 3:13p rjain
* PR 19275: passing dither etc correctly in addOutputPort. Also fixed
* some misc things.
* 
* Hydra_Software_Devel/73   1/30/06 2:27p nitinb
* PR 17882: Changed FW buffer sizes for DDP algorithm.
* 
* Hydra_Software_Devel/72   1/23/06 4:28p kagrawal
* PR 19023: 1. Setting write pointer to the ring buffer start address
* during rbuf start irrespective of the channel type. This will ensure
* that starting of pcm playback 2nd time, there is no leftover from
* previous playback
* 2. Removed eChannelType passed to BRAP_RBUF_P_Start
* 
* Hydra_Software_Devel/71   1/20/06 5:30p rjain
* PR 19190: adding PI changes for HD DVD LPCM
* 
* Hydra_Software_Devel/70   1/19/06 7:23p bselva
* PR 19168: Checking in the changes required for 7400 bringup
* 
* Hydra_Software_Devel/69   1/19/06 2:08p kagrawal
* PR 18264: 1. Rearchitected SRC to accomodate capture channel
* 2. Implemented review comments
* 
* Hydra_Software_Devel/68   1/16/06 8:39p nitinb
* PR 19098: 7411 RAP PI takes long time to download firmware binaries
* 
* Hydra_Software_Devel/67   12/26/05 5:02p kagrawal
* PR 18264: Added bCapDirect2SrcFIFO in sSrcChParams
* 
* Hydra_Software_Devel/66   12/23/05 11:05a nitinb
* PR 17882: EC-3 error handling test failed
* 
* Hydra_Software_Devel/65   12/14/05 9:43p nitinb
* PR 18312: Implement 7411 RAP PI phase-5 deliverables
* 
* Hydra_Software_Devel/64   12/14/05 9:18p nitinb
* PR 18312: Implement 7411 RAP PI phase-5 deliverables
* 
* Hydra_Software_Devel/63   12/14/05 7:08p nitinb
* PR 18061: Raptor memory allocation must be less than 0.5M for MPEG/AC-3
* only
* 
* Hydra_Software_Devel/62   12/13/05 4:39p nitinb
* PR 18163: RAP BMEM leak on system uninit; calls to BMEM_AllocAligned
* but no calls to BMEM_Free
* 
* Hydra_Software_Devel/61   12/13/05 1:58p kagrawal
* PR 18578: Added support for dither and burst mode programming for
* SPDIFFM
* 
* Hydra_Software_Devel/60   12/9/05 4:41p rjain
* PR 18264: Capture PI Development
* 
* Hydra_Software_Devel/59   12/6/05 6:32p kagrawal
* PR 18312: Renamed BRAP_P_GetChannelPairUsingOpPort() to
* BRAP_P_GetChannelPairUsingOpPort_isr()
* 
* Hydra_Software_Devel/58   12/5/05 10:36a nitinb
* PR 18061: Added memory sizes for DTS.
* 
* Hydra_Software_Devel/57   12/4/05 8:14p kagrawal
* PR 18312: Added support to write PCM data in interrupt driven mode
* 
* Hydra_Software_Devel/56   12/3/05 4:54p kagrawal
* PR 18312: Removed unused ePcmOutputMode from source channel start
* interface
* 
* Hydra_Software_Devel/55   12/3/05 4:12p kagrawal
* PR 18312: Removed compile time warning
* 
* Hydra_Software_Devel/54   12/2/05 3:32p nitinb
* PR 18061: Raptor memory allocation must be less than 0.5M for MPEG/AC-3
* only
* 
* Hydra_Software_Devel/53   12/1/05 8:21p kagrawal
* PR 18278: Watchdog support for PCM playback and mixing
* 
* Hydra_Software_Devel/52   11/30/05 5:59p kagrawal
* PR 18279: Fixed compilation error for 7411
* 
* Hydra_Software_Devel/51   11/30/05 4:17p kagrawal
* PR 18279: Rearchitected output sampling rate change for PCM channel
* 
* Hydra_Software_Devel/50   11/25/05 2:47p bselva
* PR 17590: Fix for BigEndian system
* 
* Hydra_Software_Devel/49   11/16/05 9:13p rjain
* PR 18129: Making the changes reqd for PI-Fw interface to add an output
* port on the fly for a DEC channel.
* 
* Hydra_Software_Devel/48   11/16/05 1:30p rjain
* PR 18129: Added code for 1.Watchdog recovery. 2. Adding a cloned port
* before a channel start
* 
* Hydra_Software_Devel/47   11/14/05 10:34a rjain
* PR 17862 : removing warning
* 
* Hydra_Software_Devel/46   11/14/05 9:53a rjain
* PR 18129: Adding support for routing to multiple output ports
* 
* Hydra_Software_Devel/45   11/10/05 11:53a kagrawal
* PR 17590: Merging PCM Playback and Mixing changes to
* Hydra_Software_Devel
* 
* Hydra_Software_Devel/Kapil_PCM_Playback/3   11/7/05 7:24p kagrawal
* PR 17590: Enable PCM playback on SPDIF output port
* 
* Hydra_Software_Devel/Kapil_PCM_Playback/2   11/1/05 9:43a kagrawal
* Added eChannelType in BRAP_RBUF_P_Open
* 
* Hydra_Software_Devel/Kapil_PCM_Playback/1   10/24/05 5:05p kagrawal
* PR 17590: Initial check in for PCM Playback and Mixing
* 
* Hydra_Software_Devel/44   10/28/05 1:12p rjain
* PR16888: earlier check in for this pR got undone. re-doing it.
* 
* Hydra_Software_Devel/43   10/28/05 1:05p rjain
* PR 17862 : Added code to clear all memory allocated by
* BRAP_AllocAligned.
* 
* Hydra_Software_Devel/41   10/18/05 4:56p rjain
* PR 17651: Changing checks for BCHP_CHIP ==/!= 7401 to check for
* BCHP_7411_VER instead
* 
* Hydra_Software_Devel/40   10/17/05 2:16p rjain
* PR 17651: Making changes for 7411 D0
* 
* Hydra_Software_Devel/39   10/14/05 3:41p nitinb
* PR 17420: Added private function BRAP_P_GetInternalCallFlag
* 
* Hydra_Software_Devel/38   10/13/05 2:39p bselva
* PR 16148: Fixed the compilation warning for the change in rdb wrt to
* MEMC registers.
* 
* Hydra_Software_Devel/37   10/5/05 7:30p bselva
* PR 16148: Added interrupt support for 7401
* 
* Hydra_Software_Devel/36   9/22/05 1:09p nitinb
* PR 16899: Making changes in the code for watchdog feature
* 
* Hydra_Software_Devel/35   9/21/05 5:43p bselva
* PR 16148: fixing the warnings for 7401 compilation
* 
* Hydra_Software_Devel/34   9/19/05 6:35p rjain
* PR 16899: FMM changes for the Watchdog recovery
* 
* Hydra_Software_Devel/33   9/19/05 4:35p rjain
* PR16899: making changes to FMM modules for Watchdog recovery
* 
* Hydra_Software_Devel/32   9/15/05 8:31p nitinb
* PR 16899: Added private function to get watchdog recovery flag status.
* 
* Hydra_Software_Devel/31   9/14/05 7:49p nitinb
* PR 17042: Raptor does not support sample rate change in the middle of a
* stream
* 
* Hydra_Software_Devel/30   9/13/05 7:43p bmishra
* PR 16148: Merged 7401 related changes
* 
* Hydra_Software_Devel/BLR_RAP_7401_Bringup/5   8/19/05 12:03p bmishra
* PR 16148: Merged latest changes from the main branch
* 
* Hydra_Software_Devel/29   8/5/05 1:39p rjain
* PR 16514:
*
* Hydra_Software_Devel/28   8/1/05 4:53p rjain
* PR 16420: Audio mute ramp down should only apply when decoder is
* locked. Also reduced timeout period from 100 to 20.
* 
* Hydra_Software_Devel/27   7/21/05 6:54p rjain
* PR16215: Start and Stop sequences of Raptor modules
* 
* Hydra_Software_Devel/BLR_RAP_7401_Bringup/2   7/15/05 3:29p bmishra
* PR 16148: Added code for MSA Read/Write and using temporarily for
* Raptor device memory access for bringup since CPU is not able to
* access Raptor device memory.
* 
* Hydra_Software_Devel/BLR_RAP_7401_Bringup/1   7/12/05 8:15p bmishra
* PR 16148: Modified the file for 7401 compatibility
* 
* Hydra_Software_Devel/25   7/11/05 11:17a nitinb
* PR 16148: Adding wrapper functions to convert device memory physical
* address to virtual and vice versa
* 
* Hydra_Software_Devel/24   7/8/05 6:10p nitinb
* PR 16148: Modifying memory read/write functions for 7401
* 
* Hydra_Software_Devel/23   7/8/05 2:04p rjain
* PR 16148: Modifiying for 7401
* 
* Hydra_Software_Devel/22   7/6/05 10:41a bselva
* PR15611: Checking in RDB that has absolute physical register addresses
* 
* Hydra_Software_Devel/21   6/24/05 3:32p bmishra
* PR 14143: Implemented code review comments
* 
* Hydra_Software_Devel/20   6/23/05 6:43p nitinb
* PR 15976: Added function BRAP_P_DownloadDspFwInMem that is required for
* new MITimplementation.
* 
* Hydra_Software_Devel/19   6/18/05 3:31p rjain
* PR 15925: PI changes required for routing decoded data to I2S and SPDIF
* ports simultanesouly
* 
* Hydra_Software_Devel/18   6/2/05 10:47a rjain
* PR 14143: Implemented FMM Code review comments.
* 
* Hydra_Software_Devel/17   5/31/05 10:21a nitinb
* PR 15272: Implemented register read/write functions that select
* appropriate read/write functions depending on isr or task context
* 
* Hydra_Software_Devel/16   5/30/05 6:50p rjain
* PR 15272: Updating interrupt infrastructure
* 
* Hydra_Software_Devel/15   5/4/05 11:09a rjain
* PR 14507:- Added proper States, open/start counts for modules.
* - Added appropriate checks to handle multiple calls to _Open(),
* _Start(), _Stop(), _Close() for modules.
* - Corrected SPDIFFM handle stored in hFmm / channel handle to be
* SPDIFFM Stream handle.
* - Some clean up, mroe comments/debug messages.
* - Added BRAP_RM_P_GetSpdifFmId() to RM
* 
* Hydra_Software_Devel/14   4/27/05 6:14p bmishra
* PR 14507: pOpPathResrc->uiSpdiffmId should be used as index in stead of
* pOpPathResrc->uiMixerId while opening SpdifFm handle
* 
* Hydra_Software_Devel/13   4/22/05 2:44p rjain
* PR 14507: Modifying so that  _StopXXX and _CloseXXX  do not return
* errors from in between.
* 
* Hydra_Software_Devel/12   4/22/05 10:04a rjain
* PR 14507: Adding changes for simul mode
* 
* Hydra_Software_Devel/11   4/15/05 2:58p bmishra
* PR 14507: Added Simultaneous mode related changes
* 
* Hydra_Software_Devel/10   4/11/05 3:24p nitinb
* PR 13082: Corrected debug module name
* 
* Hydra_Software_Devel/9   4/4/05 10:31p nitinb
* PR 13082: correcting compilation warning under linux
* 
* Hydra_Software_Devel/8   3/15/05 6:49p rjain
* PR 13082: correcting compilation errors under Metrowerks
* 
* Hydra_Software_Devel/6   2/8/05 7:00p rjain
* PR13082: Some bug fixes and clean up
* 
* Hydra_Software_Devel/5   2/3/05 2:46p rjain
* PR13082: Fixing compilation warnings
* 
* Hydra_Software_Devel/4   2/2/05 7:16p nitinb
* PR 13082: Fixing compilation warnings
* 
* Hydra_Software_Devel/3   2/2/05 5:34p rjain
* PR13082:Converted BREG_Read32 and BRAP_Read32 and BREG_Write32 to
* BRAP_Write32
* 
* Hydra_Software_Devel/2   1/27/05 6:10p bmishra
* PR 13827: Implemented Memory read and write functions using BARC
* interface
*
***************************************************************************/
#include "brap.h"
#include "brap_priv.h"

BDBG_MODULE(rap_priv);

#define BRAP_P_DEC_PT_MODE_COUNT		(2) /* Decode mode and pass thru mode - 2 in count */

BRAP_OP_P_SpdifParams	sSpdifParams;
BRAP_OP_P_I2sParams		sI2sParams;
BRAP_OP_P_MaiParams		sMaiParams;
BRAP_OP_P_FlexParams	sFlexParams;
BRAP_OP_P_DacParams	sDacParams;

/* Static array sAlgoMemRequirementArray contains buffer size requirements for all the algorithms. 
 * Application selects the algorithms to be supported during BRAP_Open. While allocating these
 * buffers, PI finds out worst case size requirement for each of the buffer for selected algorithms
 * using this array and makes buffer allocation accordingly.
 */
 
/*
For 7401: 
Copied from 
http://twiki-01.broadcom.com/bin/viewfile/Raptor/WebHome?rev=1;filename=
7401MemoryUsage.xls
Dated: 20th July 06
Decoder specific memory estimates		
Algo        Code size	Tables	scratch 	interframe 	Total
                                space       buffer

MPEG1	    59392	    23432	8	        10916	    93748	
AC3 5.1	    28672	    7560	110	        31588	    67930	
AAC 5.1	    28672	    29612	123368	    14336	    195988	
AAC HE stereo	57344	44084	90600	    52180	    244208	
Dolby plus 5.1	45056	20620	79060	    31562	    176298	
*/

/*
For 7411: 
Copied from 
 http://twiki-01.broadcom.com/bin/viewfile/Bseavsw/ReferencePhases97398?rev=1.
11;filename=7411MemoryUsage.xls
Dated: 20th July 06
Decoder specific memory estimates		
Algo        Code size	Tables	scratch 	interframe 	Total
                                space       buffer

MPEG1	    86016	    23432	8	        10916	120372
AC3 5.1	    28672	    7560	110	        3376	39718
AAC 5.1	    28672	    29612	123368	    14336	195988
AAC HE stereo	57344	44084	90600	    52180	244208
Dolby plus 5.1	45056	20620	79060	    31512	176248
DTS core	19770	    86092	67668	    13488	187018

*/


#if (BRAP_DSP_P_7401_NEWMIT==1)
static const 
BRAP_P_Dec_AlgoMemRequirement sAlgoMemRequirementArray[BRAP_MAX_AUDIO_TYPES] = {
		{	/* MPEG */
			{
				BRAP_P_ALIGN((59392 + 24250),2), /* Code + Table */
				BRAP_P_ALIGN(11172,2),  /* Interframe buf */
				BRAP_P_ALIGN(0,2),      /* Interstage I/P */
				BRAP_P_ALIGN(0,2),      /* Interstate IF I/P */
				BRAP_P_ALIGN(147456,2),      /* Interstage O/P */
				BRAP_P_ALIGN(0,2),      /* Interstate IF O/P */
				BRAP_P_ALIGN(0,2),      /* DEC scratch buf */
				BRAP_P_ALIGN(84,2),     /* DEC config */
			},
			BRAP_P_ALIGN(4096,2), /* Pass Thru Exec Size */
			BRAP_P_ALIGN(24576,2)       /* Ring Buffer */
		},	
#if ( BCHP_CHIP == 7400 )
		{	/* AAC */
			{
				BRAP_P_ALIGN((149504 +71680),2), /* Code + Table */
				BRAP_P_ALIGN(187392,2),  /* Interframe buf */
				BRAP_P_ALIGN(73728,2),  /* Interstage I/P */
				BRAP_P_ALIGN(41984 ,2),   /* Interstate IF I/P */
				BRAP_P_ALIGN(147728,2),  /* Interstage O/P */
				BRAP_P_ALIGN(41984 ,2),   /* Interstate IF O/P */
				BRAP_P_ALIGN(81920,2),  /* DEC scratch buf */
				BRAP_P_ALIGN(1024,2),    /* DEC config */
			},
			BRAP_P_ALIGN(4096,2), /* Pass Thru Exec Size */
			BRAP_P_ALIGN(24576,2)       /* Ring Buffer */
		},	
		{	/* AAC_SBR */
			{
				BRAP_P_ALIGN((149504 +71680),2), /* Code + Table */
				BRAP_P_ALIGN(187392,2),  /* Interframe buf */
				BRAP_P_ALIGN(73728,2),  /* Interstage I/P */
				BRAP_P_ALIGN(41984 ,2),   /* Interstate IF I/P */
				BRAP_P_ALIGN(147728,2),  /* Interstage O/P */
				BRAP_P_ALIGN(41984 ,2),   /* Interstate IF O/P */
				BRAP_P_ALIGN(81920,2),  /* DEC scratch buf */
				BRAP_P_ALIGN(1024,2),    /* DEC config */
			},
			BRAP_P_ALIGN(4096,2), /* Pass Thru Exec Size */
			BRAP_P_ALIGN(24576,2)       /* Ring Buffer */
		},	
#else
		{	/* AAC */
			{
				BRAP_P_ALIGN((155000 +76000),2), /* Code + Table */
				BRAP_P_ALIGN(333000,2),  /* Interframe buf */
				BRAP_P_ALIGN(56320,2),  /* Interstage I/P */
				BRAP_P_ALIGN(3000 ,2),   /* Interstate IF I/P */
				BRAP_P_ALIGN(131072,2),  /* Interstage O/P */
				BRAP_P_ALIGN(3000 ,2),   /* Interstate IF O/P */
				BRAP_P_ALIGN(74752,2),  /* DEC scratch buf */
				BRAP_P_ALIGN(1024,2),    /* DEC config */
			},
			BRAP_P_ALIGN(4096,2), /* Pass Thru Exec Size */
			BRAP_P_ALIGN(24576,2)       /* Ring Buffer */
		},	
		{	/* AAC_SBR */
			{
				BRAP_P_ALIGN((155000 + 76000),2), /* Code + Table */
				BRAP_P_ALIGN(333000,2),  /* Interframe buf */
				BRAP_P_ALIGN(56320,2),  /* Interstage I/P */
				BRAP_P_ALIGN(3000 ,2),   /* Interstate IF I/P */
				BRAP_P_ALIGN(131072,2),  /* Interstage O/P */
				BRAP_P_ALIGN(3000 ,2),   /* Interstate IF O/P */
				BRAP_P_ALIGN(74752,2),  /* DEC scratch buf */
				BRAP_P_ALIGN(1024,2),    /* DEC config */
			},
			BRAP_P_ALIGN(4096,2), /* Pass Thru Exec Size */
			BRAP_P_ALIGN(24576,2)       /* Ring Buffer */
		},	
#endif		
#if (BRAP_7401_FAMILY == 1)		
		{	/* AC3 */
			{
				BRAP_P_ALIGN((28672 + 7560),2), /* Code + Table */ 
				BRAP_P_ALIGN(31588,2),  /* Interframe buf */
				BRAP_P_ALIGN(0,2),      /* Interstage I/P */ 
				BRAP_P_ALIGN(0,2),      /* Interstate IF I/P */ 
				BRAP_P_ALIGN(49152,2),      /* Interstage O/P */ 
				BRAP_P_ALIGN(0,2),      /* Interstate IF O/P */ 
				BRAP_P_ALIGN(0,2),      /* DEC scratch buf */ 
				BRAP_P_ALIGN(130,2),    /* DEC config */
			},
			BRAP_P_ALIGN(4200,2), /* Pass Thru Exec Size */
			BRAP_P_ALIGN(24576,2)       /* Ring Buffer */
		},	
#else
		/* We use DDP executables for AC3 decoding */
		{	/* AC3 */
			{
				BRAP_P_ALIGN((47000+21740),2), /* Code + Table */ 
				BRAP_P_ALIGN(33804,2),  /* Interframe buf */
				BRAP_P_ALIGN(0,2),      /* Interstage I/P */ 
				BRAP_P_ALIGN(78688,2),  /* Interstate IF I/P */ 
				BRAP_P_ALIGN(49152,2),      /* Interstage O/P */ 
				BRAP_P_ALIGN(78688,2),  /* Interstate IF O/P */ 
				BRAP_P_ALIGN(3584,2),   /* DEC scratch buf */ 
				BRAP_P_ALIGN(520,2),    /* DEC config */
			},
			BRAP_P_ALIGN((28672 + 0),2), /* Pass Thru Exec Size */
			BRAP_P_ALIGN(24576,2)  /* Ring Buffer */
		},
#endif
		{	/* AC3 Plus  aka DDP  */
			{
				BRAP_P_ALIGN((47000+21740),2), /* Code + Table */ 
				BRAP_P_ALIGN(33804,2),  /* Interframe buf */
				BRAP_P_ALIGN(0,2),      /* Interstage I/P */ 
				BRAP_P_ALIGN(78688,2),  /* Interstate IF I/P */ 
				BRAP_P_ALIGN(49152,2),      /* Interstage O/P */ 
				BRAP_P_ALIGN(78688,2),  /* Interstate IF O/P */ 
				BRAP_P_ALIGN(3584,2),   /* DEC scratch buf */ 
				BRAP_P_ALIGN(520,2),    /* DEC config */
			},
			BRAP_P_ALIGN(28672,2),/* Pass Thru Exec Size */
			BRAP_P_ALIGN(24576,2)       /* Ring Buffer */
		},	
#if ((BRAP_DTS_SUPPORTED == 1) || (BRAP_DTS_PASSTHRU_SUPPORTED==1))		
		{	/* DTS */
			{
				BRAP_P_ALIGN((30720 + 102400),2), /* Code + Table */ 
				BRAP_P_ALIGN(15000,2),  /* Interframe buf */
				BRAP_P_ALIGN(33000,2),      /* Interstage I/P */ 
				BRAP_P_ALIGN(19000,2),  /* Interstate IF I/P */ 
				BRAP_P_ALIGN(65536,2),      /* Interstage O/P */ 
				BRAP_P_ALIGN(0,2),  /* Interstate IF O/P */ 
				BRAP_P_ALIGN(50000,2),      /* DEC scratch buf */ 
				BRAP_P_ALIGN(250,2)     /* DEC config */

			},
			BRAP_P_ALIGN(5000,2),          /* Pass Thru Exec Size */
			BRAP_P_ALIGN(24576,2)       /* Ring Buffer */
		},	
#else		
		{	/* DTS */
			{
				BRAP_P_ALIGN((28672 + 87090),2), /* Code + Table */ 
				BRAP_P_ALIGN(13488,2),  /* Interframe buf */
				BRAP_P_ALIGN(8,2),      /* Interstage I/P */ 
				BRAP_P_ALIGN(67488,2),  /* Interstate IF I/P */ 
				BRAP_P_ALIGN(65536,2),      /* Interstage O/P */ 
				BRAP_P_ALIGN(67488,2),  /* Interstate IF O/P */ 
				BRAP_P_ALIGN(8,2),      /* DEC scratch buf */ 
				BRAP_P_ALIGN(164,2)     /* DEC config */

			},
			BRAP_P_ALIGN(0,2),          /* Pass Thru Exec Size */
			BRAP_P_ALIGN(24576,2)       /* Ring Buffer */
		},	
#endif		
		{	/* LPCM BD */
			{
				BRAP_P_ALIGN((28672 + 23432),2), /* Code + Table */ 
				BRAP_P_ALIGN(964,2),    /* Interframe buf */
				BRAP_P_ALIGN(30720,2),  /* Interstage I/P */ 
				BRAP_P_ALIGN(28,2),     /* Interstate IF I/P */
				BRAP_P_ALIGN(30720,2),  /* Interstage O/P */ 
				BRAP_P_ALIGN(28,2),     /* Interstate IF O/P */
				BRAP_P_ALIGN(0,2),      /* DEC scratch buf */ 
				BRAP_P_ALIGN(540,2),    /* DEC config */
			},
			BRAP_P_ALIGN(0,2),          /* Pass Thru Exec Size */
			BRAP_P_ALIGN(24576,2)       /* Ring Buffer */
		},	
		{	/* LPCM HD-DVD */
			{
				BRAP_P_ALIGN((28672 + 23432),2), /* Code + Table */ 
				BRAP_P_ALIGN(968,2),    /* Interframe buf */
				BRAP_P_ALIGN(32128,2),  /* Interstage I/P */ 
				BRAP_P_ALIGN(28,2),     /* Interstate IF I/P */ 
				BRAP_P_ALIGN(32128,2),  /* Interstage O/P */ 
				BRAP_P_ALIGN(28,2),     /* Interstate IF O/P */
				BRAP_P_ALIGN(0,2),      /* DEC scratch buf */ 
				BRAP_P_ALIGN(540,2),    /* DEC config */
			},
			BRAP_P_ALIGN(0,2),          /* Pass Thru Exec Size */
			BRAP_P_ALIGN(24576,2)       /* Ring Buffer */
		},	
            {	/* DTS HD */
			{
				BRAP_P_ALIGN((28672 + 87090),2), /* Code + Table */ 
				BRAP_P_ALIGN(13488,2),  /* Interframe buf */
				BRAP_P_ALIGN(8,2),      /* Interstage I/P */ 
				BRAP_P_ALIGN(67488,2),  /* Interstate IF I/P */ 
				BRAP_P_ALIGN(8,2),      /* Interstage O/P */ 
				BRAP_P_ALIGN(67488,2),  /* Interstate IF O/P */ 
				BRAP_P_ALIGN(8,2),      /* DEC scratch buf */ 
				BRAP_P_ALIGN(164,2),    /* DEC config */
			},
			BRAP_P_ALIGN(0,2),          /* Pass Thru Exec Size */
			BRAP_P_ALIGN(24576,2)       /* Ring Buffer */
            },
		{	/* LPCM DVD */
			{
				BRAP_P_ALIGN((28672 + 23432),2), /* Code + Table */ 
				BRAP_P_ALIGN(964,2),    /* Interframe buf */
				BRAP_P_ALIGN(32128,2),  /* Interstage I/P */ 
				BRAP_P_ALIGN(28,2),     /* Interstate IF I/P */ 
				BRAP_P_ALIGN(32128,2),  /* Interstage O/P */ 
				BRAP_P_ALIGN(28,2),     /* Interstate IF O/P */ 
				BRAP_P_ALIGN(0,2),      /* DEC scratch buf */ 
				BRAP_P_ALIGN(612,2),    /* DEC config */
			},
			BRAP_P_ALIGN(0,2),          /* Pass Thru Exec Size */
			BRAP_P_ALIGN(24576,2)       /* Ring Buffer */
		},	
		{	/* WMA_STD */
			{
				BRAP_P_ALIGN((40960 + 36000),2), /* Code + Table */ 
				BRAP_P_ALIGN(32840,2),  /* Interframe buf */
				BRAP_P_ALIGN(0,2),      /* Interstage I/P */ 
				BRAP_P_ALIGN(0,2),      /* Interstate IF I/P */ 
				BRAP_P_ALIGN(96256,2),      /* Interstage O/P */ 
				BRAP_P_ALIGN(0,2),      /* Interstate IF O/P */ 
				BRAP_P_ALIGN(22908,2),   /* DEC scratch buf */ 
				BRAP_P_ALIGN(60,2)       /* DEC config */
			},
			BRAP_P_ALIGN(0,2),          /* Pass Thru Exec Size */
			BRAP_P_ALIGN(24576,2)       /* Ring Buffer */
		},
		{	/* AC3_LOSSLESS */
			{
				BRAP_P_ALIGN((47000 + 20620),2), /* Code + Table */ 
				BRAP_P_ALIGN(33280,2),  /* Interframe buf */
				BRAP_P_ALIGN(49152,2),  /* Interstage I/P */ 
				BRAP_P_ALIGN(75292,2),  /* Interstate IF I/P */ 
				BRAP_P_ALIGN(49152,2),  /* Interstage O/P */ 
				BRAP_P_ALIGN(75292,2),  /* Interstate IF O/P */ 
				BRAP_P_ALIGN(3584,2),   /* DEC scratch buf */ 
				BRAP_P_ALIGN(412,2)     /* DEC config */
			},
			BRAP_P_ALIGN(28672,2),      /* Pass Thru Exec Size */
			BRAP_P_ALIGN(24576,2)       /* Ring Buffer */
		},
		{	/* MLP */
			{
				BRAP_P_ALIGN((14336 + 4608),2), /* Code + Table */ 
				BRAP_P_ALIGN(1024,2),   /* Interframe buf */
				BRAP_P_ALIGN(0,2),      /* Interstage I/P */ 
				BRAP_P_ALIGN(0,2),      /* Interstate IF I/P */ 
				BRAP_P_ALIGN(0,2),      /* Interstage O/P */ 
				BRAP_P_ALIGN(0,2),      /* Interstate IF O/P */ 
				BRAP_P_ALIGN(0,2),      /* DEC scratch buf */ 
				BRAP_P_ALIGN(40,2)      /* DEC config */
			},
			BRAP_P_ALIGN(0,2),          /* Pass Thru Exec Size */
			BRAP_P_ALIGN(24576,2)       /* Ring Buffer */
		},
		{	/* WMA_PRO */
			{
				BRAP_P_ALIGN((26624 +22320),2), /* Code + Table */ 
				BRAP_P_ALIGN(133500,2), /* Interframe buf */
				BRAP_P_ALIGN(152500,2), /* Interstage I/P */ 
				BRAP_P_ALIGN(128,2), /* Interstate IF I/P*/ 
				BRAP_P_ALIGN(152500,2), /* Interstage O/P */ 
				BRAP_P_ALIGN(128,2), /* Interstate IF O/P*/ 
				BRAP_P_ALIGN(174080,2), /* DEC scratch buf */ 
				BRAP_P_ALIGN(118,2)  /* DEC config */
			},
			BRAP_P_ALIGN(0,2), /* Pass Thru Exec Size */
			BRAP_P_ALIGN(24576,2), /* Ring Buffer */
		},
		{	/* PCM Wav */
			{
				BRAP_P_ALIGN((25000+0),2), /* Code + Table */ 
				BRAP_P_ALIGN(480,2), /* Interframe buf */
				BRAP_P_ALIGN(24576,2), /* Interstage I/P */ 
				BRAP_P_ALIGN(0,2), /* Interstate IF I/P*/ 
				BRAP_P_ALIGN(24576,2), /* Interstage O/P */ 
				BRAP_P_ALIGN(0,2), /* Interstate IF O/P*/ 
				BRAP_P_ALIGN(0,2), /* DEC scratch buf */ 
				BRAP_P_ALIGN(20,2) /* DEC config */
			},
			BRAP_P_ALIGN(0,2), /* Pass Thru Exec Size */
			BRAP_P_ALIGN(24576,2), /* Ring Buffer */
		}		
		,{	/* DRA */
			{
				BRAP_P_ALIGN((20000 + 40000),2), /* Code + Table */
				BRAP_P_ALIGN(35000,2),  /* Interframe buf */
				BRAP_P_ALIGN(0,2),  /* Interstage I/P */
				BRAP_P_ALIGN(0,2),   /* Interstate IF I/P */
				BRAP_P_ALIGN(0,2),  /* Interstage O/P */
				BRAP_P_ALIGN(0,2),   /* Interstate IF O/P */
				BRAP_P_ALIGN(30000,2),  /* DEC scratch buf */
				BRAP_P_ALIGN(32,2),    /* DEC config */
			},
			BRAP_P_ALIGN(4096,2), /* Pass Thru Exec Size */
			BRAP_P_ALIGN(24576,2)       /* Ring Buffer */
		}
#ifdef RAP_SRSTRUVOL_CERTIFICATION
		,{	/* PCM Passthru */
			{
				BRAP_P_ALIGN((87040 + 28672),2), /* Code + Table */
				BRAP_P_ALIGN(8,2),  /* Interframe buf */
				BRAP_P_ALIGN(8,2),  /* Interstage I/P */
				BRAP_P_ALIGN(8,2),   /* Interstate IF I/P */
				BRAP_P_ALIGN(26576,2),  /* Interstage O/P */
				BRAP_P_ALIGN(8,2),   /* Interstate IF O/P */
				BRAP_P_ALIGN(8,2),  /* DEC scratch buf */
				BRAP_P_ALIGN(8,2),    /* DEC config */
			},
			BRAP_P_ALIGN(0,2), /* Pass Thru Exec Size */
			BRAP_P_ALIGN(24576,2)       /* Ring Buffer */
		}
#endif
};
#else

static const 
BRAP_P_AlgoMemRequirement sAlgoMemRequirementArray[BRAP_MAX_AUDIO_TYPES] = {
		{	/* MPEG */
			{
#if defined ( BCHP_7411_VER ) 
				BRAP_P_ALIGN((86016 /* 28672 */ + 23432),2), /* Code + Table */
				BRAP_P_ALIGN(10916,2), /* Interframe buf */
				BRAP_P_ALIGN(0,2), /* Interstage */
				BRAP_P_ALIGN(0,2), /* Interstate IF */
				BRAP_P_ALIGN(0,2), /* DEC scratch buf */
				BRAP_P_ALIGN(8,2)  /* DEC config */
#else
				BRAP_P_ALIGN((59392 /* 28672 */ + 24250),2), /* Code + Table */
				BRAP_P_ALIGN(11172,2), /* Interframe buf */
				BRAP_P_ALIGN(0,2), /* Interstage */
				BRAP_P_ALIGN(0,2), /* Interstate IF */
				BRAP_P_ALIGN(0,2), /* DEC scratch buf */
				BRAP_P_ALIGN(20,2)  /* DEC config */
#endif
			},
			BRAP_P_ALIGN((2048 + 0),2), /* Pass Thru Exec Size */
			BRAP_P_ALIGN(24576,2),  /* Ring Buffer */
			BRAP_P_ALIGN(12000,2) /*Frame Sync Size*/
		},	
		{	/* AAC */
			{
				BRAP_P_ALIGN((28672 + 29612 + 10240),2), /* Code + Table + Downmix code */
				BRAP_P_ALIGN(14336,2), /* Interframe buf */
				BRAP_P_ALIGN(49152,2), /* Interstage */
				BRAP_P_ALIGN(1500,2), /* Intersate IF */
				BRAP_P_ALIGN(73088,2), /* DEC scratch buf */
				BRAP_P_ALIGN(340,2)  /* DEC config */
			},
			BRAP_P_ALIGN((2048 + 0),2), /* Pass Thru Exec Size */
			BRAP_P_ALIGN(24576,2),  /* Ring Buffer */
			BRAP_P_ALIGN(12000,2) /*Frame Sync Size*/
		},	
		{	/* AAC_SBR */
			{
				BRAP_P_ALIGN((57344 + 44084 + 10000),2), /* Code + Table */
				BRAP_P_ALIGN(52180,2), /* Interframe buf */
				BRAP_P_ALIGN(16384,2), /* Interstage */
				BRAP_P_ALIGN(1104,2), /* Interstate IF */ 
				BRAP_P_ALIGN(73088,2), /* DEC scratch buf */ 
				BRAP_P_ALIGN(340,2)   /* DEC config */
			},
			BRAP_P_ALIGN((2048 + 0),2), /* Pass Thru Exec Size */
			BRAP_P_ALIGN(24576,2),  /* Ring Buffer */
			BRAP_P_ALIGN(12000,2) /*Frame Sync Size*/
		},	
#if defined ( BCHP_7411_VER ) 
		{	/* AC3 */
			{
				BRAP_P_ALIGN((28672 + 7560),2), /* Code + Table */ 
				BRAP_P_ALIGN(3376,2), /* Interframe buf */
				BRAP_P_ALIGN(0,2), /* Interstage */ 
				BRAP_P_ALIGN(0,2), /* Interstate IF */ 
				BRAP_P_ALIGN(0,2), /* DEC scratch buf */ 
				BRAP_P_ALIGN(110,2)   /* DEC config */
			},
			BRAP_P_ALIGN((2292 + 0),2), /* Pass Thru Exec Size */
			BRAP_P_ALIGN(24576,2),  /* Ring Buffer */
			BRAP_P_ALIGN(12000,2) /*Frame Sync Size*/
		},
#elif (BRAP_7401_FAMILY == 1)
		{	/* AC3 */
			{
				BRAP_P_ALIGN((28672 + 7560),2), /* Code + Table */ 
				BRAP_P_ALIGN(31588,2), /* Interframe buf */
				BRAP_P_ALIGN(0,2), /* Interstage */ 
				BRAP_P_ALIGN(0,2), /* Interstate IF */ 
				BRAP_P_ALIGN(0,2), /* DEC scratch buf */ 
				BRAP_P_ALIGN(110,2)   /* DEC config */
			},
			BRAP_P_ALIGN((2292 + 0),2), /* Pass Thru Exec Size */
			BRAP_P_ALIGN(24576,2),  /* Ring Buffer */
			BRAP_P_ALIGN(12000,2) /*Frame Sync Size*/
		},                
#else
		
		/* We use DDP executables for AC3 decoding */
		{	/* AC3 */
			{
				BRAP_P_ALIGN((47000+21740),2), /* Code + Table */ 
				BRAP_P_ALIGN(0x840C,2), /* Interframe buf */
				BRAP_P_ALIGN(0,2), /* Interstage */ 
				BRAP_P_ALIGN(0x13360,2), /* Interstate IF */ 
				BRAP_P_ALIGN(0xe00,2), /* DEC scratch buf */ 
				BRAP_P_ALIGN(0x19c,2)  /* DEC config */
			},
			BRAP_P_ALIGN((28672 + 0),2), /* Pass Thru Exec Size */
			BRAP_P_ALIGN(24576,2),  /* Ring Buffer */
			BRAP_P_ALIGN(12000,2) /*Frame Sync Size*/
		},	
#endif		
		{	/* AC3 Plus */
			{
#if defined (BCHP_7411_VER ) /* 7114 C0 and D0 */
				BRAP_P_ALIGN((45056/*47000*/+20620),2), /* Code + Table */ 
				BRAP_P_ALIGN(31512/*0x8200*/,2), /* Interframe buf */
				BRAP_P_ALIGN(0/*0xc000*/,2), /* Interstage */ 
				BRAP_P_ALIGN(75264/*0x1261c*/,2), /* Interstate IF */ 
				BRAP_P_ALIGN(3584/*0xe00*/,2), /* DEC scratch buf */ 
				BRAP_P_ALIGN(212/*0x19c*/,2)  /* DEC config */
#else	/* For 7400 or 7401 or 7118 */			
				BRAP_P_ALIGN((47000+21740),2), /* Code + Table */ 
				BRAP_P_ALIGN(0x840C,2), /* Interframe buf */
				BRAP_P_ALIGN(0,2), /* Interstage */ 
				BRAP_P_ALIGN(0x13360,2), /* Interstate IF */ 
				BRAP_P_ALIGN(0xe00,2), /* DEC scratch buf */ 
				BRAP_P_ALIGN(0x19c,2)  /* DEC config */
#endif				
			},
			BRAP_P_ALIGN((28672 + 0),2), /* Pass Thru Exec Size */
			BRAP_P_ALIGN(24576,2),  /* Ring Buffer */
			BRAP_P_ALIGN(12000,2) /*Frame Sync Size*/
		},	
		{	/* DTS */
			{
#if BCHP_7411_VER > BCHP_VER_C0 /* Only for 7411D0 */
				BRAP_P_ALIGN((28672 + 87242),2), /* Code + Table */ 
				BRAP_P_ALIGN(17848,2), /* Interframe buf */
				BRAP_P_ALIGN(16384,2), /* Interstage */ 
				BRAP_P_ALIGN(18336,2), /* Interstate IF */ 
				BRAP_P_ALIGN(8,2), /* DEC scratch buf */ 
				BRAP_P_ALIGN(168,2)  /* DEC config */
#else
				BRAP_P_ALIGN((28672 + 87090),2), /* Code + Table */ 
				BRAP_P_ALIGN(13488,2), /* Interframe buf */
				BRAP_P_ALIGN(8,2), /* Interstage */ 
				BRAP_P_ALIGN(67488,2), /* Interstate IF */ 
				BRAP_P_ALIGN(8,2), /* DEC scratch buf */ 
				BRAP_P_ALIGN(164,2)  /* DEC config */
#endif /* BCHP_7411_VER > BCHP_VER_C0 */								
			},
			BRAP_P_ALIGN(0,2), /* Pass Thru Exec Size */
			BRAP_P_ALIGN(24576,2), /* Ring Buffer */
			BRAP_P_ALIGN(12000,2) /*Frame Sync Size*/
		},	
		{	/* LPCM BD */
			{
				BRAP_P_ALIGN((28672 + 23432),2), /* Code + Table */ 
				BRAP_P_ALIGN(964,2), /* Interframe buf */
				BRAP_P_ALIGN(30720,2), /* Interstage */ 
				BRAP_P_ALIGN(28,2), /* Interstate IF */
				BRAP_P_ALIGN(0,2), /* DEC scratch buf */ 
				BRAP_P_ALIGN(540,2)  /* DEC config */
			},
			BRAP_P_ALIGN(0,2), /* Pass Thru Exec Size */
			BRAP_P_ALIGN(24576,2),  /* Ring Buffer */
			BRAP_P_ALIGN(12000,2) /*Frame Sync Size*/
		},	
		{	/* LPCM HD-DVD */
			{
				BRAP_P_ALIGN((28672 + 23432),2), /* Code + Table */ 
				BRAP_P_ALIGN(968,2), /* Interframe buf */
				BRAP_P_ALIGN(32128 ,2), /* Interstage */ 
				BRAP_P_ALIGN(28,2), /* Interstate IF */ 
				BRAP_P_ALIGN(0,2), /* DEC scratch buf */ 
				BRAP_P_ALIGN(540,2)  /* DEC config */
			},
			BRAP_P_ALIGN(0,2), /* Pass Thru Exec Size */
			BRAP_P_ALIGN(24576,2), /* Ring Buffer */
			BRAP_P_ALIGN(12000,2) /*Frame Sync Size*/
		},	
            {	/* DTS HD */
			{
#if BCHP_7411_VER > BCHP_VER_C0 /* Only for 7411D0 */
				BRAP_P_ALIGN((28672 + 87242),2), /* Code + Table */ 
				BRAP_P_ALIGN(17848,2), /* Interframe buf */
				BRAP_P_ALIGN(16384,2), /* Interstage */ 
				BRAP_P_ALIGN(18336,2), /* Interstate IF */ 
				BRAP_P_ALIGN(8,2), /* DEC scratch buf */ 
				BRAP_P_ALIGN(168,2)  /* DEC config */
#else
				BRAP_P_ALIGN((28672 + 87090),2), /* Code + Table */ 
				BRAP_P_ALIGN(13488,2), /* Interframe buf */
				BRAP_P_ALIGN(8,2), /* Interstage */ 
				BRAP_P_ALIGN(67488,2), /* Interstate IF */ 
				BRAP_P_ALIGN(8,2), /* DEC scratch buf */ 
				BRAP_P_ALIGN(164,2)  /* DEC config */
#endif /* BCHP_7411_VER > BCHP_VER_C0 */				
			},
			BRAP_P_ALIGN(0,2), /* Pass Thru Exec Size */
			BRAP_P_ALIGN(24576,2), /* Ring Buffer */
			BRAP_P_ALIGN(12000,2) /*Frame Sync Size*/
            },
		{	/* LPCM DVD */
			{
				BRAP_P_ALIGN((28672 + 23432),2), /* Code + Table */ 
				BRAP_P_ALIGN(964,2), /* Interframe buf */
				BRAP_P_ALIGN(32128,2), /* Interstage */ 
				BRAP_P_ALIGN(28,2), /* Interstate IF */ 
				BRAP_P_ALIGN(0,2), /* DEC scratch buf */ 
				BRAP_P_ALIGN(540,2)  /* DEC config */
			},
			BRAP_P_ALIGN(0,2), /* Pass Thru Exec Size */
			BRAP_P_ALIGN(24576,2), /* Ring Buffer */
			BRAP_P_ALIGN(12000,2) /*Frame Sync Size*/
		},
		{	/* WMA_STD */
			{
				BRAP_P_ALIGN((40960 + 36000),2), /* Code + Table */ 
				BRAP_P_ALIGN(32840,2), /* Interframe buf */
				BRAP_P_ALIGN(0,2), /* Interstage */ 
				BRAP_P_ALIGN(0,2), /* Interstate IF */ 
				BRAP_P_ALIGN(6524,2), /* DEC scratch buf */ 
				BRAP_P_ALIGN(0,2)  /* DEC config */
			},
			BRAP_P_ALIGN(0,2), /* Pass Thru Exec Size */
			BRAP_P_ALIGN(24576,2), /* Ring Buffer */
			BRAP_P_ALIGN(12000,2) /*Frame Sync Size*/
		},
		{	/* AC3_LOSSLESS */
			{
				BRAP_P_ALIGN((47000 + 20620),2), /* Code + Table */ 
				BRAP_P_ALIGN(0x8200,2), /* Interframe buf */
				BRAP_P_ALIGN(0xc000,2), /* Interstage */ 
				BRAP_P_ALIGN(0x1261c,2), /* Interstate IF */ 
				BRAP_P_ALIGN(0xe00,2), /* DEC scratch buf */ 
				BRAP_P_ALIGN(0x19c,2)   /* DEC config */
			},
			BRAP_P_ALIGN((28672 + 0),2), /* Pass Thru Exec Size */
			BRAP_P_ALIGN(24576,2),  /* Ring Buffer */
			BRAP_P_ALIGN(12000,2) /*Frame Sync Size*/
		},
		{	/* MLP */
			{
				BRAP_P_ALIGN((14336 + 4608),2), /* Code + Table */ 
				BRAP_P_ALIGN(1024,2), /* Interframe buf */
				BRAP_P_ALIGN(0,2), /* Interstage */ 
				BRAP_P_ALIGN(0,2), /* Interstate IF */ 
				BRAP_P_ALIGN(0,2), /* DEC scratch buf */ 
				BRAP_P_ALIGN(40,2)   /* DEC config */
			},
			BRAP_P_ALIGN((0 + 0),2), /* Pass Thru Exec Size */
			BRAP_P_ALIGN(24576,2),  /* Ring Buffer */
			BRAP_P_ALIGN(12000,2) /*Frame Sync Size*/
		},
		{	/* WMA_PRO */
			{
				BRAP_P_ALIGN((26624 +16384 ),2), /* Code + Table */ 
				BRAP_P_ALIGN(133000,2), /* Interframe buf */
				BRAP_P_ALIGN(152500,2), /* Interstage */ 
				BRAP_P_ALIGN(128,2), /* Interstate IF */ 
				BRAP_P_ALIGN(174080,2), /* DEC scratch buf */ 
				BRAP_P_ALIGN(48,2)  /* DEC config */
			},
			BRAP_P_ALIGN(0,2), /* Pass Thru Exec Size */
			BRAP_P_ALIGN(24576,2), /* Ring Buffer */
			BRAP_P_ALIGN(12000,2) /*Frame Sync Size*/
		},
		{	/* DRA */
			{
				BRAP_P_ALIGN((20000 + 40000),2), /* Code + Table */
				BRAP_P_ALIGN(35000,2),  /* Interframe buf */
				BRAP_P_ALIGN(0,2),  /* Interstage I/P */
				BRAP_P_ALIGN(0,2),   /* Interstate IF I/P */
				BRAP_P_ALIGN(9000,2),  /* Interstage O/P */
				BRAP_P_ALIGN(0,2),   /* Interstate IF O/P */
				BRAP_P_ALIGN(30000,2),  /* DEC scratch buf */
				BRAP_P_ALIGN(32,2),    /* DEC config */
			},
			BRAP_P_ALIGN(4096,2), /* Pass Thru Exec Size */
			BRAP_P_ALIGN(24576,2)       /* Ring Buffer */
		}
};
#endif

static const
uint32_t	ui32MultiChannelInterstageBufReqArray[BRAP_DSPCHN_AudioType_eMax] = {
	BRAP_P_ALIGN(0, 2),			/* MPEG */
	BRAP_P_ALIGN(49152, 2),		/* AAC */
	BRAP_P_ALIGN(16384,2),		/* AAC-SBR */
	BRAP_P_ALIGN(0x9000,2),		/* AC3 */
	BRAP_P_ALIGN(0xc000 /*0x9000*/,2),		/* AC3 Plus */
	BRAP_P_ALIGN(16384,2),		/* DTS */
	BRAP_P_ALIGN(30720,2),		/* LPCM-BD */
	BRAP_P_ALIGN(32128,2),		/* LPCM HD-DVD */
	BRAP_P_ALIGN(16384,2),		/* DTS-HD */
	BRAP_P_ALIGN(32128,2),		/* LPCM-DVD */
	BRAP_P_ALIGN(0,2),			/* WMA-STD */
	BRAP_P_ALIGN(0xc000,2),		/* AC3 LOSSLESS */
	BRAP_P_ALIGN(0,2),           /* MLP */
	BRAP_P_ALIGN(51200,2)           /* WMA PRO */
};


#if (BRAP_DSP_P_7401_NEWMIT==1)
static const
BRAP_P_Pp_AlgoMemRequirement sPpAlgoMemRequirementArray[BRAP_DSPCHN_PP_Algo_eMax] = {
		{	/* BRAP_DSPCHN_PP_Algo_eDdbm */ /*note DDBM is not used for 7401 */
			{
			BRAP_P_ALIGN((8192 + 8192), 2), /* FW Code + Table size */ 
				BRAP_P_ALIGN(0,2),			/* Interframe buf size */  
				BRAP_P_ALIGN(0,2), 			/* Interstage Input Buf size */ 
				BRAP_P_ALIGN(0,2),			/* Interstate IF Input */ 
				BRAP_P_ALIGN(0,2), 			/* Interstage Output Buf size */ 
				BRAP_P_ALIGN(0,2),			/* Interstate IF Output */ 
				BRAP_P_ALIGN(0,2),          /* PP scratch buf */
				BRAP_P_ALIGN(0,2)           /* PP config */ 
			}
		},	
		{	/* BRAP_DSPCHN_PP_Algo_eSrc */
			{
    			BRAP_P_ALIGN((4600 + 700), 2), /* FW Code + Table size */ 
				BRAP_P_ALIGN(4352,2),	    /* Interframe buf size */ 
				BRAP_P_ALIGN(0,2), 			/* Interstage Input Buf size */  
				BRAP_P_ALIGN(0,2),			/* Interstate IF Input */ 
				BRAP_P_ALIGN(0,2), 			/* Interstage Output Buf size */ 
				BRAP_P_ALIGN(0,2),			/* Interstate IF Output */ 
				BRAP_P_ALIGN(0,2),          /* PP scratch buf */
				BRAP_P_ALIGN(0,2)           /* PP config */  
			}
		},
		{	/* BRAP_DSPCHN_PP_Algo_eAacDownmix */
			{
    			BRAP_P_ALIGN(10240, 2), /* FW Code + Table size */ 
				BRAP_P_ALIGN(0,2),	    /* Interframe buf size */ 
				BRAP_P_ALIGN(0,2), 			/* Interstage Input Buf size */  
				BRAP_P_ALIGN(0,2),			/* Interstate IF Input */ 
				BRAP_P_ALIGN(0,2), 			/* Interstage Output Buf size */ 
				BRAP_P_ALIGN(0,2),			/* Interstate IF Output */ 
				BRAP_P_ALIGN(0,2),          /* PP scratch buf */
				BRAP_P_ALIGN(0,2)           /* PP config */  
			}
        },
        {   /* BRAP_DSPCHN_PP_Algo_eDsola */
            {   /* FIXME: get correct sizes */
                BRAP_P_ALIGN(6800+1500, 2),     /* FW Code + Table size */ 
                BRAP_P_ALIGN(15000,2),      /* Interframe buf size */ 
                BRAP_P_ALIGN(0,2),     /* Interstage Input Buf size */  
                BRAP_P_ALIGN(0,2),         /* Interstate IF Input */ 
                BRAP_P_ALIGN(0,2),     /* Interstage Output Buf size */ 
                BRAP_P_ALIGN(0,2),         /* Interstate IF Output */ 
                BRAP_P_ALIGN(60000,2),         /* PP scratch buf */
                BRAP_P_ALIGN(0,2)          /* PP config */  
            }
		}		
#if (BRAP_AD_SUPPORTED == 1)
		,
		{	/* BRAP_DSPCHN_PP_Algo_eAD_FadeCtrl */
			{
    			BRAP_P_ALIGN((4600 + 2048), 2), /* FW Code + Table size */ 
				BRAP_P_ALIGN(4352,2),	    /* Interframe buf size */ 
				BRAP_P_ALIGN(0,2), 			/* Interstage Input Buf size */  
				BRAP_P_ALIGN(0,2),			/* Interstate IF Input */ 
				BRAP_P_ALIGN(0,2), 			/* Interstage Output Buf size */ 
				BRAP_P_ALIGN(0,2),			/* Interstate IF Output */ 
				BRAP_P_ALIGN(0,2),          /* PP scratch buf */
				BRAP_P_ALIGN(0,2)           /* PP config */  
			}
		},
		{	/* BRAP_DSPCHN_PP_Algo_eAD_PanCtrl */
			{
    			BRAP_P_ALIGN((6000 + 2048), 2), /* FW Code + Table size */ 
				BRAP_P_ALIGN(4352,2),	    /* Interframe buf size */ 
				BRAP_P_ALIGN(0,2), 			/* Interstage Input Buf size */  
				BRAP_P_ALIGN(0,2),			/* Interstate IF Input */ 
				BRAP_P_ALIGN(0,2), 			/* Interstage Output Buf size */ 
				BRAP_P_ALIGN(0,2),			/* Interstate IF Output */ 
				BRAP_P_ALIGN(0,2),          /* PP scratch buf */
				BRAP_P_ALIGN(0,2)           /* PP config */  
			}
		}		
#endif
#if (BRAP_DOLBYVOLUME_SUPPORTED == 1)
		,{	/* BRAP_DSPCHN_PP_Algo_eDolbyVolume */
			{
    			BRAP_P_ALIGN((44000 + 200000), 2), /* FW Code + Table size */ 
				BRAP_P_ALIGN(18000,2),	    /* Interframe buf size */ 
				BRAP_P_ALIGN(0,2), 			/* Interstage Input Buf size */  
				BRAP_P_ALIGN(0,2),			/* Interstate IF Input */ 
				BRAP_P_ALIGN(0,2), 			/* Interstage Output Buf size */ 
				BRAP_P_ALIGN(0,2),			/* Interstate IF Output */ 
				BRAP_P_ALIGN(50000,2),          /* PP scratch buf */
				BRAP_P_ALIGN(110,2)           /* PP config */  
			}
		}
#endif
#if (BRAP_SRS_TRUVOL_SUPPORTED == 1)
		,{	/* BRAP_DSPCHN_PP_Algo_eSRS_TruVol */
			{
    			BRAP_P_ALIGN((20000 + 8000), 2), /* FW Code + Table size */ 
				BRAP_P_ALIGN(18000,2),	    /* Interframe buf size */ 
				BRAP_P_ALIGN(0,2), 			/* Interstage Input Buf size */  
				BRAP_P_ALIGN(0,2),			/* Interstate IF Input */ 
				BRAP_P_ALIGN(0,2), 			/* Interstage Output Buf size */ 
				BRAP_P_ALIGN(0,2),			/* Interstate IF Output */ 
				BRAP_P_ALIGN(8,2),          /* PP scratch buf */
				BRAP_P_ALIGN(112,2)           /* PP config */  
			}
		}
#endif
};
#else
static const
BRAP_P_DecodeModeMemRequirement sPpAlgoMemRequirementArray[BRAP_DSPCHN_PP_Algo_eMax] = {
		{ /* BRAP_DSPCHN_PP_Algo_eDdbm */
			BRAP_P_ALIGN((8192 + 8192), 2), 		/* FW Code + Table size */ 
				BRAP_P_ALIGN(0,2),			/* Interframe buf size */
				BRAP_P_ALIGN(0,2), 			/* Interstage Buf size */ 
				BRAP_P_ALIGN(0,2),			/* Interstate IF */ 
				BRAP_P_ALIGN(0,2), 			/* DEC scratch buf */ 
				BRAP_P_ALIGN(0, 2)			/* DEC config */
		},
		{ /* BRAP_DSPCHN_PP_Algo_eSrc */
			BRAP_P_ALIGN((4600 + 700), 2), 		/* FW Code + Table size */ 
				BRAP_P_ALIGN(0,2),			/* Interframe buf size */
				BRAP_P_ALIGN(0,2), 			/* Interstage Buf size */ 
				BRAP_P_ALIGN(0,2),			/* Interstate IF */ 
				BRAP_P_ALIGN(0,2), 			/* DEC scratch buf */ 
				BRAP_P_ALIGN(0, 2)			/* DEC config */
		}
};
#endif

/* This function gives the absolute value corresponding to the enum 
   BAVC_AudioSamplingRate */
BERR_Code BRAP_P_ConvertSR (
    BAVC_AudioSamplingRate   eSR,   /* [in] samping rate enum */
    unsigned int            *uiSR   /* [out] sampling rate unsigned value */
)
{
    BERR_Code err = BERR_SUCCESS;
    BDBG_ASSERT(uiSR);

    switch(eSR)
    {
    	case BAVC_AudioSamplingRate_e32k:   *uiSR = 32000; break;
        case BAVC_AudioSamplingRate_e44_1k: *uiSR = 44100; break;
        case BAVC_AudioSamplingRate_e48k:   *uiSR = 48000; break;
        case BAVC_AudioSamplingRate_e96k:   *uiSR = 96000; break;
        case BAVC_AudioSamplingRate_e16k:   *uiSR = 16000; break;
        case BAVC_AudioSamplingRate_e22_05k:*uiSR = 22050; break;
        case BAVC_AudioSamplingRate_e24k:   *uiSR = 24000; break;
        case BAVC_AudioSamplingRate_e64k:   *uiSR = 64000; break;
        case BAVC_AudioSamplingRate_e88_2k: *uiSR = 88200; break;
        case BAVC_AudioSamplingRate_e128k:  *uiSR = 128000; break;
        case BAVC_AudioSamplingRate_e176_4k:*uiSR = 176400; break;
        case BAVC_AudioSamplingRate_e192k:  *uiSR = 192000; break;
        case BAVC_AudioSamplingRate_e12k:  *uiSR = 12000; break;
        case BAVC_AudioSamplingRate_e8k:  *uiSR = 8000; break;
        case BAVC_AudioSamplingRate_e11_025k:  *uiSR = 11025; break;
        case BAVC_AudioSamplingRate_eUnknown: 
        default:            
            *uiSR = 0; err = BERR_INVALID_PARAMETER; break;
    }
    return err;        
}

/* BRAP_P_AlignAddress : Aligns the address to the specified bit position
 */
uint32_t BRAP_P_AlignAddress(
		uint32_t	ui32Address,	/* [in] size in bytes of block to allocate */
		uint32_t	uiAlignBits		/* [in] alignment for the block */
		)
{
	return (ui32Address+((1<<uiAlignBits)-1)) & ~((1<<uiAlignBits)-1) ;
}

/* BRAP_P_GetAlgoMemRequirement: This function gets total memory required for an
 * algorithm. This memory requirement is not considering following buffers
 * CDB/ITB
 * Common Post Processing buffers (For downloading FW image)
 * Firmware Resident code
 * Master Index Table structure */

BERR_Code BRAP_P_GetAlgoMemRequirement (
				BRAP_Handle hRap, 	/* [in] RAP Handle */
				BRAP_DSPCHN_AudioType eAudioType,	/* [in] Audio Type */
#if (BRAP_DSP_P_7401_NEWMIT==1)
				BRAP_P_Dec_AlgoMemRequirement *psAlgoMemReq /* [out] pointer to struct to be
														   initialized with memory requirement
														   for the given algorithm */
#else
				BRAP_P_AlgoMemRequirement *psAlgoMemReq /* [out] pointer to struct to be
														   initialized with memory requirement
														   for the given algorithm */
#endif
)
{
	BERR_Code err = BERR_SUCCESS;
	
	BDBG_ASSERT(hRap);
	BDBG_ASSERT(psAlgoMemReq);
	BSTD_UNUSED(hRap);

	*psAlgoMemReq = sAlgoMemRequirementArray[eAudioType];

	return err;
}

/* BRAP_P_GetPpAlgoMemRequirement: This function gets memory requirement for a
 * post processing algorithm. This function essentially gets the split-up of scratch buffer
 * as required by post processing algorithm.
 */


BERR_Code BRAP_P_GetPpAlgoMemRequirement(
				BRAP_Handle hRap, 							/* [in] RAP Handle */
				BRAP_DSPCHN_PP_Algo	ePpAlgo,			/* [in] Post processing algorithm */
#if (BRAP_DSP_P_7401_NEWMIT==1)				
				BRAP_P_Pp_AlgoMemRequirement * psPpAlgoMemReq	/* [out] pointer to struct to be
														   initialized with memory requirement
														   for the given post processing algorithm */

#else
				BRAP_P_AlgoMemRequirement * psPpAlgoMemReq	/* [out] pointer to struct to be
														   initialized with memory requirement
														   for the given post processing algorithm */
#endif														   
)
{
	BERR_Code err = BERR_SUCCESS;
	
	BDBG_ASSERT(hRap);
	BDBG_ASSERT(psPpAlgoMemReq);
	BSTD_UNUSED(hRap);

#if (BRAP_DSP_P_7401_NEWMIT==1)
	BKNI_Memset(psPpAlgoMemReq, 0x0, sizeof(BRAP_P_Pp_AlgoMemRequirement));
	*psPpAlgoMemReq = sPpAlgoMemRequirementArray[ePpAlgo];

    BDBG_MSG(("For PP algo %d:", ePpAlgo));
    BDBG_MSG(("ui32PpFirmwareExecutablesSize :0x%x", sPpAlgoMemRequirementArray[ePpAlgo].sPpMemReq.ui32PpFirmwareExecutablesSize));
    BDBG_MSG(("ui32PpInterframeBufSize :0x%x", sPpAlgoMemRequirementArray[ePpAlgo].sPpMemReq.ui32PpInterframeBufSize));
    BDBG_MSG(("ui32PpInterstageInputBufSize :0x%x", sPpAlgoMemRequirementArray[ePpAlgo].sPpMemReq.ui32PpInterstageInputBufSize));
    BDBG_MSG(("ui32PpInterstageInputInterfaceBufSize :0x%x", sPpAlgoMemRequirementArray[ePpAlgo].sPpMemReq.ui32PpInterstageInputInterfaceBufSize));
    BDBG_MSG(("ui32PpInterstageOutputBufSize :0x%x", sPpAlgoMemRequirementArray[ePpAlgo].sPpMemReq.ui32PpInterstageOutputBufSize));
    BDBG_MSG(("ui32PpInterstageOutputInterfaceBufSize :0x%x", sPpAlgoMemRequirementArray[ePpAlgo].sPpMemReq.ui32PpInterstageOutputInterfaceBufSize));
    BDBG_MSG(("ui32PpScratchBufSize :0x%x", sPpAlgoMemRequirementArray[ePpAlgo].sPpMemReq.ui32PpScratchBufSize));
    BDBG_MSG(("ui32PpConfigParamStructSize :0x%x", sPpAlgoMemRequirementArray[ePpAlgo].sPpMemReq.ui32PpConfigParamStructSize));  
#else
	BKNI_Memset(psPpAlgoMemReq, 0x0, sizeof(BRAP_P_AlgoMemRequirement));
	psPpAlgoMemReq->sDecodeMemReq = sPpAlgoMemRequirementArray[ePpAlgo];
#endif


    
    

	return err;
}

/* BRAP_P_GetSelectedAlgos: This function returns audio types selected by application
 * during BRAP_Open call */
BERR_Code BRAP_P_GetSelectedAlgos(
			BRAP_Handle	hRap,		/* [in] RAP Handle */
			bool *pbSelectedAlgos	/* [out] pointer to bSupportAlgos[BRAP_MAX_AUDIO_TYPES] array */
)
{
	unsigned int i;
	BERR_Code err = BERR_SUCCESS;
	
	BDBG_ASSERT(hRap);
	BDBG_ASSERT(pbSelectedAlgos);

	for (i = 0; i < BRAP_MAX_AUDIO_TYPES; i++) {
		*(pbSelectedAlgos + i) = hRap->sSettings.bSupportAlgos[i];
	}

	return err;
}

#if (BCHP_7411_VER) || (BRAP_7401_FAMILY == 1) || ( BCHP_CHIP == 7400 )
/* BRAP_P_GetSelectedPpAlgos: This function returns post processing algorithms selected
  * by application during BRAP_Open call */
 BERR_Code BRAP_P_GetSelectedPpAlgos(
			BRAP_Handle	hRap,		/* [in] RAP Handle */
			bool *pbSelectedPpAlgos	/* [out] pointer to bSupportPpAlgos[BRAP_MAX_PP_ALGOS] array */
)
{
	unsigned int i;
	BERR_Code err = BERR_SUCCESS;
	
	BDBG_ASSERT(hRap);
	BDBG_ASSERT(pbSelectedPpAlgos);

	for (i = 0; i < BRAP_MAX_PP_ALGOS; i++) {
		*(pbSelectedPpAlgos + i) = hRap->sSettings.bSupportPpAlgos[i];
	}

	return err;

}
#endif
 
/* BRAP_P_GetDecodeBufferSizes : This function finds out worst case sizes of various buffers
    required for combination of algorithms selected by application for a RAP decode
    channel.
    */

#if (BRAP_DSP_P_7401_NEWMIT==1)
BERR_Code BRAP_P_GetDecChannelBufferSizes(
						bool *pbSupportAlgos,	/* [in] pointer to bSupportAlgos[BRAP_MAX_AUDIO_TYPES] array */
						BRAP_DSPCHN_DecodeMode eDecodeMode, /* [in] Decode mode */
						BRAP_DEC_P_ChannelMemReq	*psChMemReq, /* [out] Dec channel memory requirement */
            			bool bFwAuthEnable	 /*Firmware authentication Enable bit */
			
						)
{
	int i = 0;
	unsigned int maxInterstageBuf = 0, maxInterstageInterfaceBuf = 0;
	BRAP_P_Dec_AlgoMemRequirement	sAlgoBufSizes;
	BERR_Code err = BERR_SUCCESS;
	uint32_t	ui32FrameSyncSize = 0;

	BDBG_ASSERT(psChMemReq);
	
	BKNI_Memset(&sAlgoBufSizes, 0, sizeof(BRAP_P_Dec_AlgoMemRequirement));
	BKNI_Memset(psChMemReq, 0, sizeof(BRAP_DEC_P_ChannelMemReq));
	
	if (eDecodeMode==BRAP_DSPCHN_DecodeMode_eSimulMode) {
		BDBG_ERR(("This function finds buffer sizes only for decode and passthru modes"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	for (i = 0; i < BRAP_MAX_AUDIO_TYPES; i++) {
		/* If algorithm is not selected by application, go to next algorithm */
		if (*(pbSupportAlgos + i)==false)
			continue;
		
		if (eDecodeMode==BRAP_DSPCHN_DecodeMode_eDecode) {	
			
	
			if(bFwAuthEnable==false)
			{
			
			if (sAlgoMemRequirementArray[i].sDecodeMemReq.ui32FirmwareExecutablesSize > 
				sAlgoBufSizes.sDecodeMemReq.ui32FirmwareExecutablesSize)
				sAlgoBufSizes.sDecodeMemReq.ui32FirmwareExecutablesSize = 
					sAlgoMemRequirementArray[i].sDecodeMemReq.ui32FirmwareExecutablesSize;
								
			}
			else
			{
				sAlgoBufSizes.sDecodeMemReq.ui32FirmwareExecutablesSize+=sAlgoMemRequirementArray[i].sDecodeMemReq.ui32FirmwareExecutablesSize;		
				ui32FrameSyncSize += BRAP_DSP_P_CTXT_FS_EXE_SIZE;
			}
		
			if (sAlgoMemRequirementArray[i].sDecodeMemReq.ui32InterstageOutputBufSize > maxInterstageBuf)
				maxInterstageBuf = sAlgoMemRequirementArray[i].sDecodeMemReq.ui32InterstageOutputBufSize;

			if (sAlgoMemRequirementArray[i].sDecodeMemReq.ui32InterstageInputInterfaceBufSize > maxInterstageInterfaceBuf)
				maxInterstageInterfaceBuf = sAlgoMemRequirementArray[i].sDecodeMemReq.ui32InterstageInputInterfaceBufSize;

			if (sAlgoMemRequirementArray[i].sDecodeMemReq.ui32InterstageOutputInterfaceBufSize > maxInterstageInterfaceBuf)
				maxInterstageInterfaceBuf = sAlgoMemRequirementArray[i].sDecodeMemReq.ui32InterstageOutputInterfaceBufSize;
			
			if (sAlgoMemRequirementArray[i].sDecodeMemReq.ui32DecScratchBufSize > 
				sAlgoBufSizes.sDecodeMemReq.ui32DecScratchBufSize)
				sAlgoBufSizes.sDecodeMemReq.ui32DecScratchBufSize =
					sAlgoMemRequirementArray[i].sDecodeMemReq.ui32DecScratchBufSize;
	

			if (sAlgoMemRequirementArray[i].sDecodeMemReq.ui32InterframeBufSize > 
				sAlgoBufSizes.sDecodeMemReq.ui32InterframeBufSize)
				sAlgoBufSizes.sDecodeMemReq.ui32InterframeBufSize = 
					sAlgoMemRequirementArray[i].sDecodeMemReq.ui32InterframeBufSize;

			if (sAlgoMemRequirementArray[i].sDecodeMemReq.ui32DecoderConfigParamStructSize >
				sAlgoBufSizes.sDecodeMemReq.ui32DecoderConfigParamStructSize)
				sAlgoBufSizes.sDecodeMemReq.ui32DecoderConfigParamStructSize = 
					sAlgoMemRequirementArray[i].sDecodeMemReq.ui32DecoderConfigParamStructSize;
		
		}
		else if (eDecodeMode==BRAP_DSPCHN_DecodeMode_ePassThru) {
						
			if(bFwAuthEnable==false)
			{
			
    			if (sAlgoMemRequirementArray[i].ui32PassThruExecSize > 
    				sAlgoBufSizes.ui32PassThruExecSize)
    				sAlgoBufSizes.ui32PassThruExecSize = 
    				sAlgoMemRequirementArray[i].ui32PassThruExecSize;			
			}
			else
			{	
				sAlgoBufSizes.ui32PassThruExecSize += sAlgoMemRequirementArray[i].ui32PassThruExecSize;
			}
			
		}
		

		if (sAlgoMemRequirementArray[i].ui32RingBufSize > 
			sAlgoBufSizes.ui32RingBufSize)
			sAlgoBufSizes.ui32RingBufSize = 
			sAlgoMemRequirementArray[i].ui32RingBufSize;

	}

	if (eDecodeMode==BRAP_DSPCHN_DecodeMode_eDecode)
	{
		psChMemReq->ui32FwCodeSize = sAlgoBufSizes.sDecodeMemReq.ui32FirmwareExecutablesSize;
		psChMemReq->ui32FsSize=ui32FrameSyncSize;				
	}
	else
		psChMemReq->ui32FwCodeSize = sAlgoBufSizes.ui32PassThruExecSize;
    
	psChMemReq->ui32ScratchBufSize = sAlgoBufSizes.sDecodeMemReq.ui32DecScratchBufSize;
	psChMemReq->ui32InterframeBufSize = sAlgoBufSizes.sDecodeMemReq.ui32InterframeBufSize;
	psChMemReq->uiInterstageBufSize = maxInterstageBuf;
	psChMemReq->uiInterstageInterfaceBufSize = maxInterstageInterfaceBuf;
	psChMemReq->ui32DecConfigParamBufSize = sAlgoBufSizes.sDecodeMemReq.ui32DecoderConfigParamStructSize;
	psChMemReq->ui32RingBufSize = sAlgoBufSizes.ui32RingBufSize;


	BDBG_MSG(("Buffer sizes requirement for decode mode %d are\n"
		"Firmware image download buffer size = %d, \n"
		"Scratch Buffer Size = %d, \n"
		"Interframe Buffer Size = %d, \n"
		"Decoder Config Params Buffer Size = %d, \n"
		"Ring Buffer Size = %d\n"
		"Frame sync Size = %d\n"	
		"Interstage Buffer Size = %d,\n"
		"Interstage Interface Buffer Size = %d,\n"
		"ui32PpFwCodeSize = %d\n"
		"ui32PpScratchBufSize = %d\n"
		"ui32PpInterframeBufSize = %d\n"
		"ui32PpConfigParamBufSize = %d\n",
		eDecodeMode,
		psChMemReq->ui32FwCodeSize, 
		psChMemReq->ui32ScratchBufSize, 
		psChMemReq->ui32InterframeBufSize,
		psChMemReq->ui32DecConfigParamBufSize, 
		psChMemReq->ui32RingBufSize,
		psChMemReq->ui32FsSize,
		psChMemReq->uiInterstageBufSize,
		psChMemReq->uiInterstageInterfaceBufSize,
		psChMemReq->ui32PpFwCodeSize,
		psChMemReq->ui32PpScratchBufSize,
		psChMemReq->ui32PpInterframeBufSize,
		psChMemReq->ui32PpConfigParamBufSize));
	
	return (err);

}

#else
BERR_Code BRAP_P_GetDecChannelBufferSizes(
						bool *pbSupportAlgos,	/* [in] pointer to bSupportAlgos[BRAP_MAX_AUDIO_TYPES] array */
						BRAP_DSPCHN_DecodeMode eDecodeMode, /* [in] Decode mode */
#ifdef BCHP_7411_VER
						BRAP_AudioMemAllocation	eAudioMemAllocation, /* [in] Audio memory allocation type */
#endif
						BRAP_DEC_P_ChannelMemReq	*psChMemReq /* [out] Dec channel memory requirement */
#ifndef BCHP_7411_VER			
			, 
			bool bFwAuthEnable	 /*Firmware authentication Enable bit */
#endif			
						)
{
	int i = 0;
	unsigned int temp = 0, maxScratchBuf = 0;
	BRAP_P_AlgoMemRequirement	sAlgoBufSizes;
	BERR_Code err = BERR_SUCCESS;
	unsigned int uiInterstageBufSize = 0;

	BDBG_ASSERT(psChMemReq);
	
	BKNI_Memset(&sAlgoBufSizes, 0, sizeof(BRAP_P_AlgoMemRequirement));
	
	if (eDecodeMode==BRAP_DSPCHN_DecodeMode_eSimulMode) {
		BDBG_ERR(("This function finds buffer sizes only for decode and passthru modes"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	for (i = 0; i < BRAP_MAX_AUDIO_TYPES; i++) {
		/* If algorithm is not selected by application, go to next algorithm */
		if (*(pbSupportAlgos + i)==false)
			continue;
		
		if (eDecodeMode==BRAP_DSPCHN_DecodeMode_eDecode) {	
			
#ifndef BCHP_7411_VER		
			if(bFwAuthEnable==false)
			{
#endif			
			if (sAlgoMemRequirementArray[i].sDecodeMemReq.ui32FirmwareExecutablesSize > 
				sAlgoBufSizes.sDecodeMemReq.ui32FirmwareExecutablesSize)
				sAlgoBufSizes.sDecodeMemReq.ui32FirmwareExecutablesSize = 
					sAlgoMemRequirementArray[i].sDecodeMemReq.ui32FirmwareExecutablesSize;

					sAlgoBufSizes.ui32FrameSyncSize=0;
#ifndef BCHP_7411_VER								
			}
			else
			{
				sAlgoBufSizes.sDecodeMemReq.ui32FirmwareExecutablesSize+=sAlgoMemRequirementArray[i].sDecodeMemReq.ui32FirmwareExecutablesSize;
				sAlgoBufSizes.ui32FrameSyncSize+=sAlgoMemRequirementArray[i].ui32FrameSyncSize;
		
			}
#endif			

#ifdef BCHP_7411_VER
			if ((eAudioMemAllocation==BRAP_AudioMemAllocation_eMultiChannel) 
				|| (eAudioMemAllocation==BRAP_AudioMemAllocation_eMultiChannelSrc)) {
				uiInterstageBufSize = ui32MultiChannelInterstageBufReqArray[i];
			}
			else {
				uiInterstageBufSize = sAlgoMemRequirementArray[i].sDecodeMemReq.ui32InterstageBufSize;
			}
#else
			uiInterstageBufSize = sAlgoMemRequirementArray[i].sDecodeMemReq.ui32InterstageBufSize;
#endif

			temp = uiInterstageBufSize
				+ sAlgoMemRequirementArray[i].sDecodeMemReq.ui32InterstageInterfaceBufSize
				+ sAlgoMemRequirementArray[i].sDecodeMemReq.ui32DecScratchBufSize;

			if (temp > maxScratchBuf)
				maxScratchBuf = temp;

			if (sAlgoMemRequirementArray[i].sDecodeMemReq.ui32InterframeBufSize > 
				sAlgoBufSizes.sDecodeMemReq.ui32InterframeBufSize)
				sAlgoBufSizes.sDecodeMemReq.ui32InterframeBufSize = 
					sAlgoMemRequirementArray[i].sDecodeMemReq.ui32InterframeBufSize;

			if (sAlgoMemRequirementArray[i].sDecodeMemReq.ui32DecoderConfigParamStructSize >
				sAlgoBufSizes.sDecodeMemReq.ui32DecoderConfigParamStructSize)
				sAlgoBufSizes.sDecodeMemReq.ui32DecoderConfigParamStructSize = 
					sAlgoMemRequirementArray[i].sDecodeMemReq.ui32DecoderConfigParamStructSize;
		
			}
		else if (eDecodeMode==BRAP_DSPCHN_DecodeMode_ePassThru) {
#ifndef BCHP_7411_VER						
			if(bFwAuthEnable==false)
			{
#endif			
				if (sAlgoMemRequirementArray[i].ui32PassThruExecSize > sAlgoBufSizes.ui32PassThruExecSize)
				sAlgoBufSizes.ui32PassThruExecSize = sAlgoMemRequirementArray[i].ui32PassThruExecSize;
#ifndef BCHP_7411_VER							
			}
			else
			{
				sAlgoBufSizes.ui32PassThruExecSize += sAlgoMemRequirementArray[i].ui32PassThruExecSize;
			}
#endif			
		}
		

		if (sAlgoMemRequirementArray[i].ui32RingBufSize > 
			sAlgoBufSizes.ui32RingBufSize)
			sAlgoBufSizes.ui32RingBufSize = 
			sAlgoMemRequirementArray[i].ui32RingBufSize;

	}

	if (eDecodeMode==BRAP_DSPCHN_DecodeMode_eDecode)
		psChMemReq->ui32FwCodeSize = sAlgoBufSizes.sDecodeMemReq.ui32FirmwareExecutablesSize;
	else
		psChMemReq->ui32FwCodeSize = sAlgoBufSizes.ui32PassThruExecSize;
	psChMemReq->ui32ScratchBufSize = maxScratchBuf;
	psChMemReq->ui32InterframeBufSize = sAlgoBufSizes.sDecodeMemReq.ui32InterframeBufSize;
	psChMemReq->ui32DecConfigParamBufSize = sAlgoBufSizes.sDecodeMemReq.ui32DecoderConfigParamStructSize;
	psChMemReq->ui32RingBufSize = sAlgoBufSizes.ui32RingBufSize;
	psChMemReq->ui32FsSize=sAlgoBufSizes.ui32FrameSyncSize;

	BDBG_MSG(("Buffer sizes requirement for decode mode %d are\n"
		"Firmware image download buffer size = %d, "
		"Scratch Buffer Size = %d, "
		"Interframe Buffer Size = %d, "
		"Decoder Config Params Buffer Size = %d, "
		"Ring Buffer Size = %d",
		eDecodeMode,
		psChMemReq->ui32FwCodeSize, psChMemReq->ui32ScratchBufSize, 
		psChMemReq->ui32InterframeBufSize, psChMemReq->ui32DecConfigParamBufSize, psChMemReq->ui32RingBufSize));
	
	return (err);

}
#endif /* end of BRAP_DSP_P_7401_NEWMIT*/

#if (BCHP_7411_VER) || (BRAP_7401_FAMILY == 1) || ( BCHP_CHIP == 7400 )
/* BRAP_P_GetPpBufferSizes : This function finds out total memory
    required for combination of post processing algorithms selected by 
    application.
    */
#if (BRAP_DSP_P_7401_NEWMIT==1)
void  BRAP_P_GetPpBufferSizes(
						bool *pbSupportPpAlgos,	/* [in] pointer to bSupportPpAlgos[BRAP_MAX_PP_ALGOS] array */
						BRAP_DEC_P_ChannelMemReq	*psPpMemReq /* [out] Post processing memory requirement */
						)
{
	int i = 0;
	unsigned int maxInterstageBuf = 0, maxInterstageInterfaceBuf = 0;
	BRAP_P_Pp_AlgoMemRequirement	sAlgoBufSizes;
	
	BDBG_ASSERT(pbSupportPpAlgos);
	BSTD_UNUSED(psPpMemReq);

	BKNI_Memset(&sAlgoBufSizes, 0x0, sizeof(BRAP_P_Pp_AlgoMemRequirement));
	BKNI_Memset(psPpMemReq, 0, sizeof(BRAP_DEC_P_ChannelMemReq));
    
	for (i = 0; i < BRAP_MAX_PP_ALGOS; i++) {
		/* If algorithm is not selected by application, go to next algorithm */
		if (*(pbSupportPpAlgos + i)==false)
			continue;
		
		sAlgoBufSizes.sPpMemReq.ui32PpFirmwareExecutablesSize += 
			sPpAlgoMemRequirementArray[i].sPpMemReq.ui32PpFirmwareExecutablesSize;

	if (sPpAlgoMemRequirementArray[i].sPpMemReq.ui32PpInterframeBufSize > 
		sAlgoBufSizes.sPpMemReq.ui32PpInterframeBufSize)
		sAlgoBufSizes.sPpMemReq.ui32PpInterframeBufSize =
			sPpAlgoMemRequirementArray[i].sPpMemReq.ui32PpInterframeBufSize;
	
	if (sPpAlgoMemRequirementArray[i].sPpMemReq.ui32PpInterstageInputBufSize > maxInterstageBuf)
		maxInterstageBuf = sPpAlgoMemRequirementArray[i].sPpMemReq.ui32PpInterstageInputBufSize;
	
	if (sPpAlgoMemRequirementArray[i].sPpMemReq.ui32PpInterstageInputInterfaceBufSize > maxInterstageInterfaceBuf)
		maxInterstageInterfaceBuf = sPpAlgoMemRequirementArray[i].sPpMemReq.ui32PpInterstageInputInterfaceBufSize;
	
	if (sPpAlgoMemRequirementArray[i].sPpMemReq.ui32PpInterstageOutputBufSize > maxInterstageBuf )
		maxInterstageBuf = sPpAlgoMemRequirementArray[i].sPpMemReq.ui32PpInterstageOutputBufSize;
	
	if (sPpAlgoMemRequirementArray[i].sPpMemReq.ui32PpInterstageOutputInterfaceBufSize > maxInterstageInterfaceBuf)
		maxInterstageInterfaceBuf = sPpAlgoMemRequirementArray[i].sPpMemReq.ui32PpInterstageOutputInterfaceBufSize;
	
	if (sPpAlgoMemRequirementArray[i].sPpMemReq.ui32PpScratchBufSize > 
		sAlgoBufSizes.sPpMemReq.ui32PpScratchBufSize)
		sAlgoBufSizes.sPpMemReq.ui32PpScratchBufSize =
			sPpAlgoMemRequirementArray[i].sPpMemReq.ui32PpScratchBufSize;

	sAlgoBufSizes.sPpMemReq.ui32PpConfigParamStructSize += 
		sPpAlgoMemRequirementArray[i].sPpMemReq.ui32PpConfigParamStructSize;

	}

	psPpMemReq->ui32PpFwCodeSize = sAlgoBufSizes.sPpMemReq.ui32PpFirmwareExecutablesSize;
	psPpMemReq->ui32PpScratchBufSize = sAlgoBufSizes.sPpMemReq.ui32PpScratchBufSize;
	psPpMemReq->ui32PpInterframeBufSize = sAlgoBufSizes.sPpMemReq.ui32PpInterframeBufSize;
	psPpMemReq->uiInterstageBufSize = maxInterstageBuf;
	psPpMemReq->uiInterstageInterfaceBufSize = maxInterstageInterfaceBuf;
	psPpMemReq->ui32PpConfigParamBufSize = sAlgoBufSizes.sPpMemReq.ui32PpConfigParamStructSize;

	BDBG_MSG(("Buffer requirements for post processing algorithms \n"
		"Firmware image download buffer size = %d, \n"
		"Scratch Buffer Size = %d, \n"
		"Interframe Buffer Size = %d, \n"
		"Decoder Config Params Buffer Size = %d, \n"
		"Ring Buffer Size = %d\n"
		"Frame sync Size = %d\n"	
		"Interstage Buffer Size = %d,\n"
		"Interstage Interface Buffer Size = %d,\n"
		"ui32PpFwCodeSize = %d\n"
		"ui32PpScratchBufSize = %d\n"
		"ui32PpInterframeBufSize = %d\n"
		"ui32PpConfigParamBufSize = %d\n",
		psPpMemReq->ui32FwCodeSize, 
		psPpMemReq->ui32ScratchBufSize, 
		psPpMemReq->ui32InterframeBufSize,
		psPpMemReq->ui32DecConfigParamBufSize, 
		psPpMemReq->ui32RingBufSize,
		psPpMemReq->ui32FsSize,
		psPpMemReq->uiInterstageBufSize,
		psPpMemReq->uiInterstageInterfaceBufSize,
		psPpMemReq->ui32PpFwCodeSize,
		psPpMemReq->ui32PpScratchBufSize,
		psPpMemReq->ui32PpInterframeBufSize,
		psPpMemReq->ui32PpConfigParamBufSize));
	
	
}
#else
void  BRAP_P_GetPpBufferSizes(
						bool *pbSupportPpAlgos,	/* [in] pointer to bSupportPpAlgos[BRAP_MAX_PP_ALGOS] array */
						BRAP_DEC_P_ChannelMemReq	*psPpMemReq /* [out] Post processing memory requirement */
						)
{
	int i = 0;
	
	BDBG_ASSERT(pbSupportPpAlgos);
	BSTD_UNUSED(pbSupportPpAlgos);

	BKNI_Memset(psPpMemReq, 0x0, sizeof(BRAP_DEC_P_ChannelMemReq));
	for (i = 0; i < BRAP_MAX_PP_ALGOS; i++) {
		psPpMemReq->ui32FwCodeSize += sPpAlgoMemRequirementArray[i].ui32FirmwareExecutablesSize;
	}
}
#endif /* New MIT */




#endif

BERR_Code BRAP_P_OpenOpPathFmmModules
(
	BRAP_ChannelHandle 		hRapCh,			/* [in] The RAP Channel handle */
	BRAP_RBUF_P_Settings	*pRbufSettings,	/* [in] Pointer to an array of 2 
													ring buffer settings */
	BRAP_SRCCH_P_Settings	*pSrcChSettings,/* [in] Source channel settings */
	BRAP_MIXER_P_Settings	*pMixerSettings,/* [in] Mixer settings */
	BRAP_SPDIFFM_P_Settings	*pSpdfFmSettings,/* [in] Spdif Formater settings */
	void					*pOpSettings,	/* [in] Output settings */
	BRAP_RM_P_OpPathResrc	*pOpPathResrc,	/* [in] The FMM Module ids to be opened */
	BRAP_OutputChannelPair	eOpChannel,		/* [in] Output Audio channel type */
	BRAP_P_ObjectHandles	*pModuleHandles /* [out] Internal Module Handles */
)
{
	BERR_Code				ret = BERR_SUCCESS;
	BERR_Code				rc = BERR_SUCCESS;
	int						i;

#ifndef BCHP_7411_VER
	BRAP_OutputChannelPair	eMaxOpChannelPair = BRAP_OutputChannelPair_eDownMixedLR;
#else
	BRAP_OutputChannelPair	eMaxOpChannelPair = BRAP_OutputChannelPair_eCentreLF;
#endif

	BDBG_ENTER(BRAP_P_OpenOpPathFmmModules);

	BDBG_ASSERT(hRapCh);

    if (BRAP_P_GetWatchdogRecoveryFlag (hRapCh->hRap) == false)
    {   
    	/* If not in WatchDog recovery */  
		BDBG_ASSERT(pRbufSettings);
    	BDBG_ASSERT(pSrcChSettings);
    	BDBG_ASSERT(pSpdfFmSettings);
	    BDBG_ASSERT(pOpSettings); 
    } 
	
    BDBG_ASSERT(pMixerSettings);      

	BDBG_ASSERT(pOpPathResrc);
	BDBG_ASSERT(pModuleHandles);


#if (BRAP_7401_FAMILY ==1)	
    if (eOpChannel >= eMaxOpChannelPair)
    {
        BDBG_ERR(("BRAP_P_OpenOpPathFmmModules: Invalid Output Audio channel type!!! "));
        return BERR_TRACE(BERR_INVALID_PARAMETER); 
    }
#else 
    if (eOpChannel > eMaxOpChannelPair)
    {
        BDBG_ERR(("BRAP_P_OpenOpPathFmmModules: Invalid Output Audio channel type!!! "));
        return BERR_TRACE(BERR_INVALID_PARAMETER); 
    }
#endif

	for(i=0; i < BRAP_RM_P_MAX_RBUFS_PER_PORT; i++)
	{
		if(pOpPathResrc->uiRbufId[i] != (unsigned int)BRAP_RM_P_INVALID_INDEX)
		{
			/* Instantiate the ring buffer */
			ret = BRAP_RBUF_P_Open (
					pModuleHandles->hFmm,
					&(pModuleHandles->hRBuf[eOpChannel * BRAP_RM_P_MAX_RBUFS_PER_PORT + i]),
					pOpPathResrc->uiRbufId[i],
					&pRbufSettings[i]);
			if(ret != BERR_SUCCESS){goto end_fmm_open;}
			BDBG_MSG(("Ring buffer %d opened", pOpPathResrc->uiRbufId[i]));
		}
		else
		{
			/* RBuf is not needed for current channel */
			pModuleHandles->hRBuf[eOpChannel * BRAP_RM_P_MAX_RBUFS_PER_PORT + i] = NULL;
		}
	}


	if(pOpPathResrc->uiSrcChId != (unsigned int)BRAP_RM_P_INVALID_INDEX)
	{
		/* Instantiate the Source Channel */
		ret = BERR_TRACE(BRAP_SRCCH_P_Open (
				pModuleHandles->hFmm,
				&(pModuleHandles->hSrcCh[eOpChannel]),
				pOpPathResrc->uiSrcChId,
				pSrcChSettings));
		if(ret != BERR_SUCCESS) {goto close_rbuf;}
		BDBG_MSG(("Source channel %d opened", pOpPathResrc->uiSrcChId));
	}
	else
		pModuleHandles->hSrcCh[eOpChannel] = NULL;


    /* Fill up the mixer input id */
	pModuleHandles->uiMixerInputIndex[eOpChannel] = pOpPathResrc->uiMixerInputId;
	
	if(pOpPathResrc->uiMixerId != (unsigned int)BRAP_RM_P_INVALID_INDEX)
	{
		/* Instantiate the Mixer */
		ret = BERR_TRACE(BRAP_MIXER_P_Open (
				pModuleHandles->hFmm,
				&(pModuleHandles->hMixer[eOpChannel]),
				pOpPathResrc->uiMixerId,
				pMixerSettings));
		if(ret != BERR_SUCCESS){goto close_srcch;}
		BDBG_MSG(("Mixer %d - inpur %d opened", 
				  pOpPathResrc->uiMixerId, 
				  pOpPathResrc->uiMixerInputId));
	}
	else
		pModuleHandles->hMixer[eOpChannel] = NULL;

    if(pOpPathResrc->uiSpdiffmStreamId != (unsigned int)BRAP_RM_P_INVALID_INDEX)
	{
		/* SPDIF Formater is used */

		/* Instantiate the Micro-sequencer */
		/* Instantiate the Micro-sequencer */
		ret = BERR_TRACE(BRAP_SPDIFFM_P_Open (
				pModuleHandles->hFmm,
				&(pModuleHandles->hSpdifFm[eOpChannel]),
				pOpPathResrc->uiSpdiffmStreamId,
				pSpdfFmSettings));
		if(ret != BERR_SUCCESS){goto close_mixer;}
		BDBG_MSG(("Spdiffm %d - stream %d opened", 
				  pOpPathResrc->uiSpdiffmId, 
				  pOpPathResrc->uiSpdiffmStreamId));
	}
	else
	{
		pModuleHandles->hSpdifFm[eOpChannel] = NULL;
	}

	if(pOpPathResrc->eOutputPortType != (unsigned int)BRAP_RM_P_INVALID_INDEX)
	{
			/* Instantiate the output port */
			ret = BERR_TRACE(BRAP_OP_P_Open(pModuleHandles->hFmm,
								 &(pModuleHandles->hOp[eOpChannel]),
								 pOpPathResrc->eOutputPortType,
								 pOpSettings));
			if(ret != BERR_SUCCESS){goto close_spdiffm;}
			BDBG_MSG(("Output port type %d opened", 
					  pOpPathResrc->eOutputPortType));


	}
	else
	{
		pModuleHandles->hOp[eOpChannel] = NULL;
	}

	goto end_fmm_open;

close_spdiffm:
	/* Close the SPDIF Formator objects */
	if(pModuleHandles->hSpdifFm[eOpChannel] != NULL)
	{
		rc = BERR_TRACE(BRAP_SPDIFFM_P_Close(pModuleHandles->hSpdifFm[eOpChannel]));
	        if (rc != BERR_SUCCESS)
	        {
	            BDBG_ERR (("BRAP_P_OpenOpPathFmmModules: call to BRAP_SPDIFFM_P_Close() failed. Ignoring error!!!!!"));
	        }           
		BDBG_MSG(("Spdiffm %d - stream %d closed", 
				  pModuleHandles->hSpdifFm[eOpChannel]->uiIndex, 
				  pModuleHandles->hSpdifFm[eOpChannel]->uiStreamIndex));
	}
	
close_mixer:
	/* Close the Mixer objects */
	if(pModuleHandles->hMixer[eOpChannel] != NULL)
	{
		rc = BERR_TRACE(BRAP_MIXER_P_Close(pModuleHandles->hMixer[eOpChannel], 
								pOpPathResrc->uiMixerInputId));
        if (rc != BERR_SUCCESS)
        {
            BDBG_ERR (("BRAP_P_OpenOpPathFmmModules: call to BRAP_MIXER_P_Close() failed. Ignoring error!!!!!"));
        }     
		BDBG_MSG(("Mixer %d - input %d closed", 
				  pModuleHandles->hMixer[eOpChannel]->uiMixerIndex, 
				  pOpPathResrc->uiMixerInputId));
	}
	
close_srcch:
	/* Close the Source Channel objects */
	if(pModuleHandles->hSrcCh[eOpChannel] != NULL)
	{
		rc = BERR_TRACE(BRAP_SRCCH_P_Close(pModuleHandles->hSrcCh[eOpChannel]));
        if (rc != BERR_SUCCESS)
        {
            BDBG_ERR (("BRAP_P_OpenOpPathFmmModules: call to BRAP_SRCCH_P_Close() failed. Ignoring error!!!!!"));
        }     
		BDBG_MSG(("Source channel %d closed", 
				  pModuleHandles->hSrcCh[eOpChannel]->uiIndex));
	}
	
close_rbuf:
	/* Close the Ring buffer objects */
	for(i=0; i < BRAP_RM_P_MAX_RBUFS_PER_PORT; i++)
	{
		if(pModuleHandles->hRBuf[eOpChannel * BRAP_RM_P_MAX_RBUFS_PER_PORT + i] != NULL)
		{
			rc = BERR_TRACE(BRAP_RBUF_P_Close(pModuleHandles->hRBuf[eOpChannel * BRAP_RM_P_MAX_RBUFS_PER_PORT + i]));
            if (rc != BERR_SUCCESS)
            {
                BDBG_ERR (("BRAP_P_OpenOpPathFmmModules: call to BRAP_RBUF_P_Close() failed. Ignoring error!!!!!"));
            }  
			BDBG_MSG(("Ring buffer %d closed", 
					  pModuleHandles->hRBuf[eOpChannel * BRAP_RM_P_MAX_RBUFS_PER_PORT + i]->uiIndex));
		}
	}
	
end_fmm_open:
	BDBG_LEAVE(BRAP_P_OpenOpPathFmmModules);
	
	return (ret);
}

BERR_Code BRAP_P_CloseOpPathFmmModules
(
	BRAP_OutputChannelPair	eOpChannel,		/* [in] Output Audio channel type */
	BRAP_P_ObjectHandles	*pModuleHandles /* [in] Internal Module Handles */
)
{
	BERR_Code ret = BERR_SUCCESS;
	unsigned int i=0;

#ifndef BCHP_7411_VER
	BRAP_OutputChannelPair	eMaxOpChannelPair = BRAP_OutputChannelPair_eDownMixedLR;
#else
	BRAP_OutputChannelPair	eMaxOpChannelPair = BRAP_OutputChannelPair_eCentreLF;
#endif
	

	BDBG_ENTER(BRAP_P_CloseOpPathFmmModules);

	BDBG_ASSERT(pModuleHandles);
#if (BRAP_7401_FAMILY ==1)	
    if (eOpChannel >= eMaxOpChannelPair)
    {
        BDBG_ERR(("BRAP_P_CloseOpPathFmmModules: Invalid Output Audio channel type!!! "));
        return BERR_TRACE(BERR_INVALID_PARAMETER); 
    }
#else 
    if (eOpChannel > eMaxOpChannelPair)
    {
        BDBG_ERR(("BRAP_P_CloseOpPathFmmModules: Invalid Output Audio channel type!!! "));
        return BERR_TRACE(BERR_INVALID_PARAMETER); 
    }
#endif
	
	/* Close the outputs */
	if(pModuleHandles->hOp[eOpChannel] != NULL)
	{
		ret = BERR_TRACE(BRAP_OP_P_Close(pModuleHandles->hOp[eOpChannel]));
		if(ret != BERR_SUCCESS)
        {
            BDBG_ERR(("BRAP_P_CloseOpPathFmmModules: call to BRAP_OP_P_Close() failed!!! Ignoring error!!! "));
        }
		BDBG_MSG(("Output port type %d closed", 
				  pModuleHandles->hOp[eOpChannel]->eOutputPort));
		pModuleHandles->hOp[eOpChannel] = NULL;
	}

	/* Close the SPDIF Formaters */
	if(pModuleHandles->hSpdifFm[eOpChannel] != NULL)
	{
		ret = BERR_TRACE(BRAP_SPDIFFM_P_Close(pModuleHandles->hSpdifFm[eOpChannel]));
		if(ret != BERR_SUCCESS)
        {
            BDBG_ERR(("BRAP_P_CloseOpPathFmmModules: call to BRAP_SPDIFFM_P_Close() failed!!! Ignoring error!!! "));
        }
		BDBG_MSG(("Spdiffm %d - stream %d closed", 
				  pModuleHandles->hSpdifFm[eOpChannel]->uiIndex, 
				  pModuleHandles->hSpdifFm[eOpChannel]->uiStreamIndex));
		pModuleHandles->hSpdifFm[eOpChannel] = NULL;
	}

	/* Close the Mixers */
	if(pModuleHandles->hMixer[eOpChannel] != NULL)
	{
		ret = BERR_TRACE(BRAP_MIXER_P_Close(pModuleHandles->hMixer[eOpChannel], 
								 pModuleHandles->uiMixerInputIndex[eOpChannel]));
		if(ret != BERR_SUCCESS)
        {
            BDBG_ERR(("BRAP_P_CloseOpPathFmmModules: call to BRAP_MIXER_P_Close() failed!!! Ignoring error!!! "));
        }
		BDBG_MSG(("Mixer %d - input %d closed", 
				  pModuleHandles->hMixer[eOpChannel]->uiMixerIndex, 
				  pModuleHandles->uiMixerInputIndex[eOpChannel]));
		pModuleHandles->hMixer[eOpChannel] = NULL;
	}

	/* Close the source channels */
	if(pModuleHandles->hSrcCh[eOpChannel] != NULL)
	{
		ret = BERR_TRACE(BRAP_SRCCH_P_Close(pModuleHandles->hSrcCh[eOpChannel]));
		if(ret != BERR_SUCCESS)
        {
            BDBG_ERR(("BRAP_P_CloseOpPathFmmModules: call to BRAP_SRCCH_P_Close() failed!!! Ignoring error!!! "));
        }
		BDBG_MSG(("Source channel %d closed", 
				  pModuleHandles->hSrcCh[eOpChannel]->uiIndex));
		pModuleHandles->hSrcCh[eOpChannel] = NULL;
	}

	/* Close the ring buffers */
	for(i=0; i < BRAP_RM_P_MAX_RBUFS_PER_PORT; i++)    
	{
		if(pModuleHandles->hRBuf[eOpChannel * BRAP_RM_P_MAX_RBUFS_PER_PORT + i] != NULL)
		{
			ret = BERR_TRACE(BRAP_RBUF_P_Close(pModuleHandles->hRBuf[eOpChannel * BRAP_RM_P_MAX_RBUFS_PER_PORT + i]));
    		if(ret != BERR_SUCCESS)
            {
                BDBG_ERR(("BRAP_P_CloseOpPathFmmModules: call to BRAP_RBUF_P_Close() failed!!! Ignoring error!!! "));
            }
			BDBG_MSG(("Ring buffer %d closed", 
					  pModuleHandles->hRBuf[eOpChannel * BRAP_RM_P_MAX_RBUFS_PER_PORT + i]->uiIndex));
			pModuleHandles->hRBuf[eOpChannel * BRAP_RM_P_MAX_RBUFS_PER_PORT + i] = NULL;
		}
	}
	BDBG_LEAVE(BRAP_P_CloseOpPathFmmModules);
	
	return (ret);
}

/*
 The entire DEC_Start sequence is:

1. Mute associate channel in DP (was Muted in _Stop()) 

2. Start the associated Output operation

3. Start the associated Microsequencer operation 

4. Start the associated Mixer operation

5. Unmute the channel (taken care of in BRAP_MIXER_P_Start() which starts with SCALE_MUTE_ENA & VOLUME_MUTE_ENA both unMuted)

6. Start the associated Ring buffers' operation

7. Start the associated Source FIFOs' operation

8. Start the associated DSP Context operation (Program audio PLL, open the gate)


*/    

BERR_Code BRAP_P_StartOpPathFmmModules
(
	BRAP_ChannelHandle 		hRapCh,			/* [in] The RAP Channel handle */
	BRAP_RBUF_P_Params		*pRBufParams,	/* [in] Pointer to an array of 2 
													ring buffer settings */
	BRAP_SRCCH_P_Params		*pSrcChParams,	/* [in] Source channel start params */
	BRAP_MIXER_P_Params		*pMixerParams,	/* [in] Mixer start params */
	BRAP_SPDIFFM_P_Params	*pSpdifFmParams,/* [in] Spdif Formater start params */
	void					*pOpParams,		/* [in] Output params */
	BRAP_OutputChannelPair	eOpChannel,		/* [in] Output Audio channel type */
	BRAP_P_ObjectHandles	*pModuleHandles /* [in] Internal Module Handles */
)
{
	BERR_Code				ret = BERR_SUCCESS;
	int						i;
#ifndef BCHP_7411_VER
	BRAP_OutputChannelPair	eMaxOpChannelPair = BRAP_OutputChannelPair_eDownMixedLR;
#else
	BRAP_OutputChannelPair	eMaxOpChannelPair = BRAP_OutputChannelPair_eCentreLF;
#endif
	
	BDBG_ENTER(BRAP_P_StartOpPathFmmModules);

	BDBG_ASSERT(hRapCh);
	BDBG_ASSERT(pModuleHandles);
    
    if (BRAP_P_GetWatchdogRecoveryFlag (hRapCh->hRap) == false)
    {   /* If not in WatchDog recovery */  
	    BDBG_ASSERT(pRBufParams);
    	BDBG_ASSERT(pSrcChParams);
	    BDBG_ASSERT(pMixerParams);
    	BDBG_ASSERT(pSpdifFmParams);
	    BDBG_ASSERT(pOpParams);
    }    

    if (eOpChannel > eMaxOpChannelPair)
    {
        BDBG_ERR(("BRAP_P_StartOpPathFmmModules: Invalid Output Audio channel type!!! "));
        return BERR_TRACE(BERR_INVALID_PARAMETER); 
    }

	/* Start the associated output block */
	if(pModuleHandles->hOp[eOpChannel] != NULL)
	{
		ret = BERR_TRACE(BRAP_OP_P_Start(pModuleHandles->hOp[eOpChannel], 
							  pOpParams));
		if(ret != BERR_SUCCESS){goto end_fmm_start;}
		BDBG_MSG(("Output port type %d started", 
				  pModuleHandles->hOp[eOpChannel]->eOutputPort));
	}
	
	/* TODO: Do Output alignment if required. It is not needed for stereo. */
	
	/* Start the associated SPDIF Formaters */
	if(pModuleHandles->hSpdifFm[eOpChannel] != NULL)
	{
	    pSpdifFmParams->eSamplingRate = pModuleHandles->hOp[eOpChannel]->eSamplingRate;
		/* Start the SPDIF Formater */
		ret = BERR_TRACE(BRAP_SPDIFFM_P_Start(	hRapCh, 
									pModuleHandles->hSpdifFm[eOpChannel], 
									pSpdifFmParams));
		if(ret != BERR_SUCCESS){goto stop_op;}
		BDBG_MSG(("Spdiffm %d - stream %d started", 
				  pModuleHandles->hSpdifFm[eOpChannel]->uiIndex, 
				  pModuleHandles->hSpdifFm[eOpChannel]->uiStreamIndex));
	}

	if(hRapCh->eChannelType != BRAP_P_ChannelType_eDecode)
	{
		int ui32RegVal=0x0;

	    ui32RegVal = BRAP_Read32 (hRapCh->hRegister,BCHP_AUD_FMM_DP_CTRL0_FMM_SCALE_VOL_STEP);
		
	    ui32RegVal &= ~(BCHP_MASK (    
	                    AUD_FMM_DP_CTRL0_FMM_SCALE_VOL_STEP, 
	                    SCALE_RAMP_STEP_SIZE));
	    
	    ui32RegVal |= (BCHP_FIELD_DATA (    
	                        AUD_FMM_DP_CTRL0_FMM_SCALE_VOL_STEP, 
	                        SCALE_RAMP_STEP_SIZE, 
	                        0x20));

	    BRAP_Write32 (hRapCh->hRegister, BCHP_AUD_FMM_DP_CTRL0_FMM_SCALE_VOL_STEP, ui32RegVal);
#if ( BCHP_CHIP == 7400 )
	    BRAP_Write32 (hRapCh->hRegister, BCHP_AUD_FMM_DP_CTRL1_FMM_SCALE_VOL_STEP, ui32RegVal);
#endif		
	}
		
	/* Start the associated mixer */
	if(pModuleHandles->hMixer[eOpChannel] != NULL)
	{
		ret = BERR_TRACE(BRAP_MIXER_P_Start(pModuleHandles->hMixer[eOpChannel], 
								 pModuleHandles->uiMixerInputIndex[eOpChannel], 
								 pMixerParams));
		if(ret != BERR_SUCCESS){goto stop_spdiffm;}
		BDBG_MSG(("Mixer %d - input %d started", 
				  pModuleHandles->hMixer[eOpChannel]->uiMixerIndex, 
				  pModuleHandles->uiMixerInputIndex[eOpChannel]));
	}
    /* Start the associated ring buffers */
    
    for(i=0; i < BRAP_RM_P_MAX_RBUFS_PER_PORT; i++)
    {
        if(pModuleHandles->hRBuf[eOpChannel * BRAP_RM_P_MAX_RBUFS_PER_PORT + i] != NULL)
        {
    
            /* Start the ring buffer */
            ret = BERR_TRACE(BRAP_RBUF_P_Start(pModuleHandles->hRBuf[eOpChannel * BRAP_RM_P_MAX_RBUFS_PER_PORT + i], 
                                    &pRBufParams[i]));
            if(ret != BERR_SUCCESS){goto stop_mixer;}
            BDBG_MSG(("Ring buffer %d started", 
                      pModuleHandles->hRBuf[eOpChannel * BRAP_RM_P_MAX_RBUFS_PER_PORT + i]->uiIndex));

	    /* If it is a playback channel then we need to program the end address properly based
	    on the size given during start time */
	    if ( hRapCh->eChannelType == BRAP_P_ChannelType_ePcmPlayback )
	    {
	    	ret = BERR_TRACE(BRAP_RBUF_P_ProgramEndAddress(
				pModuleHandles->hRBuf[eOpChannel * BRAP_RM_P_MAX_RBUFS_PER_PORT + i]));
		if(ret != BERR_SUCCESS){goto stop_rbuf;}
		BDBG_MSG(("Ring buffer %d End address programmed", 
		          pModuleHandles->hRBuf[eOpChannel * BRAP_RM_P_MAX_RBUFS_PER_PORT + i]->uiIndex));

	    }
        }
    }
#ifndef RAP_SRSTRUVOL_CERTIFICATION 	
	/* Start the associated source channels */
	if(pModuleHandles->hSrcCh[eOpChannel] != NULL)
	{
		ret = BERR_TRACE(BRAP_SRCCH_P_Start(pModuleHandles->hSrcCh[eOpChannel],
								 pSrcChParams));
		if(ret != BERR_SUCCESS){goto stop_rbuf;}
		BDBG_MSG(("Source channel %d started", 
				  pModuleHandles->hSrcCh[eOpChannel]->uiIndex));
	}
#endif	

	goto end_fmm_start;
	
stop_rbuf:
	/* Stop the Ring buffer objects */
	for(i=0; i < BRAP_RM_P_MAX_OP_CHANNELS; i++)
	{
		if(pModuleHandles->hRBuf[eOpChannel * BRAP_RM_P_MAX_RBUFS_PER_PORT + i] != NULL)
		{
			ret = BERR_TRACE(BRAP_RBUF_P_Stop(pModuleHandles->hRBuf[eOpChannel * BRAP_RM_P_MAX_RBUFS_PER_PORT + i]));
		    if(ret != BERR_SUCCESS)
            {
                BDBG_ERR(("BRAP_P_StartOpPathFmmModules: call to BRAP_RBUF_P_Stop() failed!!! Ignoring error!!! "));
            }
			BDBG_MSG(("Ring buffer %d stopped", 
					  pModuleHandles->hRBuf[eOpChannel * BRAP_RM_P_MAX_RBUFS_PER_PORT + i]->uiIndex));
		}
	}
	
stop_mixer:
	/* Stop the Mixer objects */
	if(pModuleHandles->hMixer[eOpChannel] != NULL)
	{
		ret = BERR_TRACE(BRAP_MIXER_P_Stop(pModuleHandles->hMixer[eOpChannel], 
								pModuleHandles->uiMixerInputIndex[eOpChannel]));
	    if(ret != BERR_SUCCESS)
        {
            BDBG_ERR(("BRAP_P_StartOpPathFmmModules: call to BRAP_MIXER_P_Stop() failed!!! Ignoring error!!! "));
        }
		BDBG_MSG(("Mixer %d - input %d stopped", 
				  pModuleHandles->hMixer[eOpChannel]->uiMixerIndex, 
				  pModuleHandles->uiMixerInputIndex[eOpChannel]));
	}
	
stop_spdiffm:
	if(pModuleHandles->hSpdifFm[eOpChannel] != NULL)
	{
		/* Stop the Spdif Formater objects */
		ret = BERR_TRACE(BRAP_SPDIFFM_P_Stop(pModuleHandles->hSpdifFm[eOpChannel]));
	    if(ret != BERR_SUCCESS)
        {
            BDBG_ERR(("BRAP_P_StartOpPathFmmModules: call to BRAP_SPDIFFM_P_Stop() failed!!! Ignoring error!!! "));
        }
		BDBG_MSG(("Spdiffm %d - stream %d stopped", 
				  pModuleHandles->hSpdifFm[eOpChannel]->uiIndex, 
				  pModuleHandles->hSpdifFm[eOpChannel]->uiStreamIndex));
	}
	
stop_op:
	if(pModuleHandles->hOp[eOpChannel] != NULL)
	{
		/* Stop the Output objects */
		ret = BERR_TRACE(BRAP_OP_P_Stop(pModuleHandles->hOp[eOpChannel]));
	    if(ret != BERR_SUCCESS)
        {
            BDBG_ERR(("BRAP_P_StartOpPathFmmModules: call to BRAP_OP_P_Stop() failed!!! Ignoring error!!! "));
        }
		BDBG_MSG(("Output port type %d stopped", 
				  pModuleHandles->hOp[eOpChannel]->eOutputPort));
	}
	
end_fmm_start:
	
	BDBG_LEAVE(BRAP_P_StartOpPathFmmModules);
	
	return (ret);
}
/*

    The entire DEC_Stop sequence is
    
1a. (PCM mode)Stop the associated DSP Context operation(stop decoding but ring buffer still playing)

1b. (Compress SPDIF mode)Stop the associated DSP Context operation, insert one pause/null burst in ring buffer

2a. (PCM mode)Mute associated PCM output stream in DP and wait for DP volume ramp down done interrupt  

2b. (Compressed SPDIF mode)Play until ring buffer empty 

4. *Stop the associated Microsequencer operation

5. Stop the associated Mixer operation

3. *Stop the associated Output operation.

5. Stop the associated Source FIFOs' operation(close gate)

6. Stop the associated Ring buffers' operation

At step 2 ring buffer should have more than 1 frame of data left.

*Never shut down SPDIF output.

For solving the PR 23121 we need to disable the DP first because during the next start up of multichannel 
	  with alignment between pairs can no longer be guaranteed due to the leftover sample in DP. It looks like the
	 only way to flush data out of DP is to leave the corresponding OP stream enabled for at least one additional sample after
	 DP is disabled. If this happens, then on the next restart, all streams will be waiting for the correct first sample
	 like they are the first time.


*/
BERR_Code BRAP_P_StopOpPathFmmModules
(
	BRAP_OutputChannelPair	eOpChannel,		/* [in] Output Audio channel type */
	BRAP_P_ObjectHandles	*pModuleHandles /* [in] Internal Module Handles */
)
{
	BERR_Code	ret = BERR_SUCCESS;
	int			i;
#ifndef BCHP_7411_VER
	BRAP_OutputChannelPair	eMaxOpChannelPair = BRAP_OutputChannelPair_eDownMixedLR;
#else
	BRAP_OutputChannelPair	eMaxOpChannelPair = BRAP_OutputChannelPair_eCentreLF;
#endif

	BDBG_ENTER(BRAP_P_StopOpPathFmmModules);

	BDBG_ASSERT(pModuleHandles);

#if (BRAP_7401_FAMILY ==1)	
    if (eOpChannel >= eMaxOpChannelPair)
    {
        BDBG_ERR(("BRAP_P_StopOpPathFmmModules: Invalid Output Audio channel type!!! "));
        return BERR_TRACE(BERR_INVALID_PARAMETER); 
    }
#else 
    if (eOpChannel > eMaxOpChannelPair)
    {
        BDBG_ERR(("BRAP_P_StopOpPathFmmModules: Invalid Output Audio channel type!!! "));
        return BERR_TRACE(BERR_INVALID_PARAMETER); 
    }
#endif


#if 1 
	if((pModuleHandles->hMixer[eOpChannel] != NULL)
            && (pModuleHandles->hSrcCh[eOpChannel] !=NULL) 
            && (pModuleHandles->hDspCh !=NULL))
    {
        /* We can mute/unmute only for uncompressed data */    
        if (pModuleHandles->hSrcCh[eOpChannel]->sParams.bCompress == false)        
        { 
    	    /* Mute the channel */ 
	    	ret = BRAP_SRCCH_P_SetMute(pModuleHandles->hSrcCh[eOpChannel], 
									   pModuleHandles->hMixer[eOpChannel], 
									   true);
		    if(ret != BERR_SUCCESS)
    		{
	    		return BERR_TRACE(ret);
		    }

            if (pModuleHandles->hDspCh->bDecLocked ==true)
            {
    			/* Wait for ramp done only if decoder is locked.*/
            ret = BERR_TRACE(BRAP_SRCCH_P_WaitForRampDown (pModuleHandles->hSrcCh[eOpChannel], 
										pModuleHandles->hMixer[eOpChannel]));
            if (ret != BERR_SUCCESS)
            {
                BDBG_ERR (("BRAP_P_StopOpPathFmmModules: call to BRAP_SRCCH_P_WaitForRampDown() failed. Ignoring error!!!!!"));
            }            
            }
           
        }
    }
#endif
    
	/* Stop the SPDIF Formater */
	if(pModuleHandles->hSpdifFm[eOpChannel] != NULL)
	{
		ret = BERR_TRACE(BRAP_SPDIFFM_P_Stop(pModuleHandles->hSpdifFm[eOpChannel]));
	    if(ret != BERR_SUCCESS)
        {
            BDBG_ERR(("BRAP_P_StopOpPathFmmModules: call to BRAP_SPDIFFM_P_Stop() failed!!! Ignoring error!!! "));
        }
		BDBG_MSG(("Spdiffm %d - stream %d stopped", 
				  pModuleHandles->hSpdifFm[eOpChannel]->uiIndex, 
				  pModuleHandles->hSpdifFm[eOpChannel]->uiStreamIndex));
	}

	/* Stop the Mixer */
	if(pModuleHandles->hMixer[eOpChannel] != NULL)
	{
		ret = BERR_TRACE(BRAP_MIXER_P_Stop(pModuleHandles->hMixer[eOpChannel],	
								pModuleHandles->uiMixerInputIndex[eOpChannel]));
	    if(ret != BERR_SUCCESS)
        {
            BDBG_ERR(("BRAP_P_StopOpPathFmmModules: call to BRAP_MIXER_P_Stop() failed!!! Ignoring error!!! "));
        }
		BDBG_MSG(("Mixer %d - input %d stopped", 
				  pModuleHandles->hMixer[eOpChannel]->uiMixerIndex, 
				  pModuleHandles->uiMixerInputIndex[eOpChannel]));
	}

	/* For solving the PR 23121 we need to disable the DP first because during the next start up of multichannel 
	* with alignment between pairs can no longer be guaranteed due to the leftover sample in DP. It looks like the
	* only way to flush data out of DP is to leave the corresponding OP stream enabled for at least one additional sample after
	* DP is disabled. If this happens, then on the next restart, all streams will be waiting for the correct first sample
	* like they are the first time.
	*/
	/* Stop the output */
	if(pModuleHandles->hOp[eOpChannel] != NULL)
	{
		ret = BERR_TRACE(BRAP_OP_P_Stop(pModuleHandles->hOp[eOpChannel]));
	    if(ret != BERR_SUCCESS)
        {
            BDBG_ERR(("BRAP_P_StopOpPathFmmModules: call to BRAP_OP_P_Stop() failed!!! Ignoring error!!! "));
        }
		BDBG_MSG(("Output port type %d stopped", 
				  pModuleHandles->hOp[eOpChannel]->eOutputPort));
	}

	BKNI_Delay(3000);  /*Added 3msec delay - PR38305*/

#ifndef RAP_SRSTRUVOL_CERTIFICATION 
	/* Stop the source channel */
	if(pModuleHandles->hSrcCh[eOpChannel] != NULL)
	{
		ret = BERR_TRACE(BRAP_SRCCH_P_Stop(pModuleHandles->hSrcCh[eOpChannel]));
	    if(ret != BERR_SUCCESS)
        {
            BDBG_ERR(("BRAP_P_StopOpPathFmmModules: call to BRAP_SRCCH_P_Stop() failed!!! Ignoring error!!! "));
        }
		BDBG_MSG(("Source channel %d stopped", 
				  pModuleHandles->hSrcCh[eOpChannel]->uiIndex));
	}
#endif

	/* Stop the ring buffers */
	for(i=0; i < BRAP_RM_P_MAX_RBUFS_PER_PORT; i++)
	{
		if(pModuleHandles->hRBuf[eOpChannel * BRAP_RM_P_MAX_RBUFS_PER_PORT + i] != NULL)
		{
			ret = BERR_TRACE(BRAP_RBUF_P_Stop(pModuleHandles->hRBuf[eOpChannel * BRAP_RM_P_MAX_RBUFS_PER_PORT + i]));
	        if(ret != BERR_SUCCESS)
            {
                BDBG_ERR(("BRAP_P_StopOpPathFmmModules: call to BRAP_RBUF_P_Stop() failed!!! Ignoring error!!! "));
            }
			BDBG_MSG(("Ring buffer %d stopped", 
					  pModuleHandles->hRBuf[eOpChannel * BRAP_RM_P_MAX_RBUFS_PER_PORT + i]->uiIndex));
		}
	}
	

	
	BDBG_LEAVE(BRAP_P_StopOpPathFmmModules);
	
	return (ret);
}




BERR_Code BRAP_P_FormOpParams
(
	BRAP_ChannelHandle     hRapCh,		/* [in] The RAP decode channel handle */ 
	const BRAP_OutputPort	    eOutputPort,/* [in] Output port */
	BAVC_Timebase                   eTimebase, /* [in] Timebase */
	unsigned int uiOutputBitsPerSample, /* [in] Bits per sample */
	void * * 			pParamAddr		/* [out] pointer of type void * to 
                                            return a pointer to the output parameter struct */	
)
{
	BERR_Code				ret = BERR_SUCCESS;

    BDBG_MSG(("hRapCh=0x%x", hRapCh));
    BDBG_MSG(("eOutputPort=0x%x", eOutputPort));
    BDBG_MSG(("eTimebase=0x%x", eTimebase));
    BDBG_MSG(("uiOutputBitsPerSample=0x%x", uiOutputBitsPerSample));
    BDBG_MSG(("pParamAddr=0x%x", pParamAddr));

		/* Output block params */
		switch(eOutputPort)
		{
			case BRAP_OutputPort_eSpdif:
                            BRAP_OP_P_GetDefaultParams (BRAP_OutputPort_eSpdif, (void *)&sSpdifParams);
				sSpdifParams.eTimebase = eTimebase;
				*pParamAddr = &sSpdifParams;
				break;
			case BRAP_OutputPort_eI2s0:
			case BRAP_OutputPort_eI2s1:
			case BRAP_OutputPort_eI2s2:
                            BRAP_OP_P_GetDefaultParams (eOutputPort, (void *)&sI2sParams);               
				sI2sParams.eTimebase = eTimebase;
				sI2sParams.uiBitsPerSample 
					= uiOutputBitsPerSample;
				*pParamAddr = &sI2sParams;
				break;
			case BRAP_OutputPort_eMai:
                            BRAP_OP_P_GetDefaultParams (BRAP_OutputPort_eMai, (void *)&sMaiParams);               
				sMaiParams.uiSampleWidth 
					= uiOutputBitsPerSample;
                            sMaiParams.eTimebase = eTimebase;
				*pParamAddr = &sMaiParams;
				/* Populate the SPDIF settings properly if MAI input is SPDIF */
				if (hRapCh->hRap->sOutputSettings[BRAP_OutputPort_eMai].uOutputPortSettings.sMaiSettings.eMaiMuxSelector 
                        == BRAP_OutputPort_eSpdif )
				{
					hRapCh->sModuleHandles.hFmm->hOp[BRAP_OutputPort_eSpdif]->
						uOpParams.sSpdif.eTimebase = eTimebase;
				}
				else if ( hRapCh->hRap->sOutputSettings[BRAP_OutputPort_eMai].uOutputPortSettings.sMaiSettings.eMaiMuxSelector 
                            == BRAP_OutputPort_eFlex)
				{
					hRapCh->sModuleHandles.hFmm->hOp[BRAP_OutputPort_eFlex]->
						uOpParams.sFlex.eTimebase = eTimebase;
				}
				else
				{
					BDBG_ERR(("Invalid MAI mux selector port "));
				}
				break;
			case BRAP_OutputPort_eFlex:
				sFlexParams.eTimebase = eTimebase;			
				*pParamAddr = &sFlexParams;	
				break;
			case BRAP_OutputPort_eDac0:
			case BRAP_OutputPort_eDac1:
                            BRAP_OP_P_GetDefaultParams (eOutputPort, (void *)&sDacParams); 
				sDacParams.eTimebase = eTimebase;
				*pParamAddr = &sDacParams;		
				break;
			default:
				BDBG_ERR(("BRAP_P_FormOpParams: Output port type %d not supported", 
						  eOutputPort));
				ret = BERR_TRACE(BRAP_ERR_OUPUT_PORT_NOT_SUPPORTED);
		}

	BDBG_LEAVE (BRAP_P_FormOpParams);
	return (ret);

}



/* BRAP_P_MuteChannelOutputOnSr_isr: Mutes the output ports associated with
 * a decode channel at output port level on sample rate change. Returns the
 * previous mute status of output ports in psOpPortPrevMuteStatus.
 */
BERR_Code 
BRAP_P_MuteChannelOutputOnSr_isr
(
	BRAP_ChannelHandle hRapCh,		/* [in] The RAP decode channel handle */ 
	BRAP_P_OpPortPrevMuteStatus *psOpPortPrevMuteStatus /* [out] Returns previous mute states of output ports */
)
{
	unsigned int count;
	BERR_Code rc = BERR_SUCCESS;
#ifndef 	BCHP_7411_VER
	unsigned int i;
#endif

	BDBG_ASSERT(hRapCh);
	BDBG_ASSERT(psOpPortPrevMuteStatus);

	BKNI_Memset_isr(psOpPortPrevMuteStatus, 0x0, sizeof(BRAP_P_OpPortPrevMuteStatus));

	for (count = 0; count < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; count++) {
		if (hRapCh->sModuleHandles.hOp[count]) {
			rc = BRAP_OP_GetMute(hRapCh->hRap, hRapCh->sModuleHandles.hOp[count]->eOutputPort, 
					&psOpPortPrevMuteStatus->bDecMuteStatus[count]);
			if (rc != BERR_SUCCESS)
				return BERR_TRACE(rc);
			rc = BRAP_OP_P_SetMute_isr(hRapCh->hRap, hRapCh->sModuleHandles.hOp[count]->eOutputPort, true);
			if (rc != BERR_SUCCESS)
				return BERR_TRACE(rc);
			BDBG_MSG(("Muted outputport %d, mute status = %d", hRapCh->sModuleHandles.hOp[count]->eOutputPort, 
				psOpPortPrevMuteStatus->bDecMuteStatus[count]));
		}
#ifdef BCHP_7411_VER  /* only for 7411 */    	
		if (hRapCh->eClone!=BRAP_P_CloneState_eInvalid) {
#endif     
			if (hRapCh->sSimulPtModuleHandles.hOp[count]) {
				rc = BRAP_OP_GetMute(hRapCh->hRap, hRapCh->sSimulPtModuleHandles.hOp[count]->eOutputPort, 
						&psOpPortPrevMuteStatus->bSimulPtMuteStatus[count]);
				if (rc != BERR_SUCCESS)
					return BERR_TRACE(rc);
				rc = BRAP_OP_P_SetMute_isr(hRapCh->hRap, hRapCh->sSimulPtModuleHandles.hOp[count]->eOutputPort, true);
				if (rc != BERR_SUCCESS)
					return BERR_TRACE(rc);
				BDBG_MSG(("Muted simulpt outputport %d, mute status = %d", hRapCh->sSimulPtModuleHandles.hOp[count]->eOutputPort, 
				psOpPortPrevMuteStatus->bSimulPtMuteStatus[count]));
			}
#ifdef BCHP_7411_VER  /* only for 7411 */                  
		}	
#endif       
#ifndef 	BCHP_7411_VER /* Other than 7411 chips */
		for (i = 0; i < BRAP_RM_P_MAX_OUTPUTS; i++) {
			if (hRapCh->sCloneOpPathHandles[count][i].hOp) {
				rc = BRAP_OP_GetMute(hRapCh->hRap, hRapCh->sCloneOpPathHandles[count][i].hOp->eOutputPort, 
						&psOpPortPrevMuteStatus->bCloneMuteStates[count][i]);
				if (rc != BERR_SUCCESS)
					return BERR_TRACE(rc);
				rc = BRAP_OP_P_SetMute_isr(hRapCh->hRap, hRapCh->sCloneOpPathHandles[count][i].hOp->eOutputPort, true);
				if (rc != BERR_SUCCESS)
					return BERR_TRACE(rc);
				BDBG_MSG(("Muted cloned outputport %d, mute status = %d", hRapCh->sCloneOpPathHandles[count][i].hOp->eOutputPort, 
					psOpPortPrevMuteStatus->bCloneMuteStates[count][i]));
			}
		}
#endif
	}
	return rc;
}

/* BRAP_P_UnMuteChannelOutputOnSr_isr: Unmutes the output ports associated with
 * a decode channel at output port level on sample rate change. Unmutes only those
 * output ports for which previous mute status was unmute. Previous mute states of
 * output ports are passed in parameter psOpPortPrevMuteStatus.
 */
BERR_Code 
BRAP_P_UnMuteChannelOutputOnSr_isr
(
	BRAP_ChannelHandle hRapCh, 		/* [in] The RAP decode channel handle */ 
	BRAP_P_OpPortPrevMuteStatus *psOpPortPrevMuteStatus /* [in] Previous mute states of output ports */
)
{
	unsigned int count;
	BERR_Code rc = BERR_SUCCESS;
#ifndef 	BCHP_7411_VER
	unsigned int i;
#endif

	BDBG_ASSERT(hRapCh);
	BDBG_ASSERT(psOpPortPrevMuteStatus);
	for (count = 0; count < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; count++) {
		if (hRapCh->sModuleHandles.hOp[count]) {
			rc = BRAP_OP_P_SetMute_isr(hRapCh->hRap, hRapCh->sModuleHandles.hOp[count]->eOutputPort, 
					psOpPortPrevMuteStatus->bDecMuteStatus[count]);
			if (rc != BERR_SUCCESS)
				return BERR_TRACE(rc);
			BDBG_MSG(("Unmuted outputport %d, mute status = %d", hRapCh->sModuleHandles.hOp[count]->eOutputPort, 
				psOpPortPrevMuteStatus->bDecMuteStatus[count]));
		}
#ifdef BCHP_7411_VER  /* only for 7411 */    	
		if (hRapCh->eClone!=BRAP_P_CloneState_eInvalid) {
#endif     
			if (hRapCh->sSimulPtModuleHandles.hOp[count]) {
				rc = BRAP_OP_P_SetMute_isr(hRapCh->hRap, hRapCh->sSimulPtModuleHandles.hOp[count]->eOutputPort, 
						psOpPortPrevMuteStatus->bSimulPtMuteStatus[count]);
				if (rc != BERR_SUCCESS)
					return BERR_TRACE(rc);
			BDBG_MSG(("Unmuted simulpt outputport %d, mute status = %d", hRapCh->sSimulPtModuleHandles.hOp[count]->eOutputPort, 
				psOpPortPrevMuteStatus->bSimulPtMuteStatus[count]));
			}
#ifdef BCHP_7411_VER  /* only for 7411 */                  
		}	
#endif       
#ifndef 	BCHP_7411_VER /* Other than 7411 chips */
		for (i = 0; i < BRAP_RM_P_MAX_OUTPUTS; i++) {
			if (hRapCh->sCloneOpPathHandles[count][i].hOp) {
				rc = BRAP_OP_P_SetMute_isr(hRapCh->hRap, hRapCh->sCloneOpPathHandles[count][i].hOp->eOutputPort, 
						psOpPortPrevMuteStatus->bCloneMuteStates[count][i]);
				if (rc != BERR_SUCCESS)
					return BERR_TRACE(rc);
				BDBG_MSG(("Unmuted cloned outputport %d, mute status = %d", hRapCh->sCloneOpPathHandles[count][i].hOp->eOutputPort, 
					psOpPortPrevMuteStatus->bCloneMuteStates[count][i]));
			}
		}
#endif
	}
	return rc;
}

/* BRAP_P_GetWatchdogRecoveryFlag: Returns watchdog recovery flag */
bool BRAP_P_GetWatchdogRecoveryFlag(BRAP_Handle hRap)
{
	return hRap->bWatchdogRecoveryOn;
}

/* BRAP_P_GetInternalCallFlag: Returns internal call flag. This flag is set when a RAP API
 * gets called from within PI. The called API checks for this flag and depending on its status
 * executes/skips updation of internal state variables. This function is used in decoder flush
 * operaton. */
bool BRAP_P_GetInternalCallFlag (BRAP_ChannelHandle hRapCh)
{
	return hRapCh->bInternalCallFromRap;
}

/* BRAP_P_AllocAligned : A wrapper around the BMEM module to 
 * include static memory allocation 
 */
void *BRAP_P_AllocAligned(
		BRAP_Handle  hRap,        /* [in] The RAP device handle */
		size_t       ulSize,      /* [in] size in bytes of block to allocate */
		unsigned int uiAlignBits, /* [in] alignment for the block */
		unsigned int uiBoundary   /* [in] boundary restricting allocated value */
#if (BRAP_SECURE_HEAP==1)
		,bool bAllocSecureMem		/*[in ] Enabled if the memory is allocated from secure memory*/
#endif		
		)
{
	uint32_t ui32_AllocMem ;
	uint32_t ui32_adjSize;
	void *ptr;

#ifndef BCHP_7411_VER /* For chips other than 7411 */
	uint32_t ui32PhyAddr ;
	BSTD_UNUSED(ui32_AllocMem);
	BSTD_UNUSED(ui32_adjSize);
#if (BRAP_SECURE_HEAP==1)
	if(hRap->sSettings.hSecureMemory!=NULL && bAllocSecureMem==true)
	{
		ptr = BMEM_AllocAligned(
				hRap->sSettings.hSecureMemory, 
				ulSize, 
				uiAlignBits, 
				uiBoundary) ;
	}
	else
	{
			ptr = BMEM_AllocAligned(
			hRap->hHeap, 
			ulSize, 
			uiAlignBits, 
			uiBoundary);

	}
#else
			ptr = BMEM_AllocAligned(
			hRap->hHeap, 
			ulSize, 
			uiAlignBits, 
			uiBoundary);
#endif

		BRAP_P_ConvertAddressToOffset(hRap->hHeap, ptr, &ui32PhyAddr);

		BDBG_MSG(("Allocated Memory : 0x%08lx (physical addr=0x%08lx), size 0x%08lx", ptr, ui32PhyAddr, ulSize));

		hRap->uiTotalMemUsed+=ulSize;
		return (ptr == 0)? (void *)0xFFFFFFFF : ptr ;
#else /* other than 7411 */
	BSTD_UNUSED(uiBoundary);
	BSTD_UNUSED(ptr);
		ui32_adjSize = ulSize + (1<<uiAlignBits)-1 ;

		/* The simple static memory allocator works as follows :
		 * The running pointer always points to the starting address
		 * of the remaining and free portion of the static memory block.
		 */
		ui32_AllocMem = hRap->sMemAllocInfo.currentStaticMemoryPointer ;

		BDBG_MSG(("Current Mem Pointer = 0x%08x, Base = 0x%08x, Size = 0x%08x",
		hRap->sMemAllocInfo.currentStaticMemoryPointer,
		hRap->sMemAllocInfo.currentStaticMemoryBase,
		hRap->sMemAllocInfo.staticMemorySize));

		/* Point the running pointer to the end+1 of the current buffer *
		 * but first check if the static memory block is not exhausted */
		if( (hRap->sMemAllocInfo.currentStaticMemoryPointer+ulSize) > 
			(hRap->sMemAllocInfo.currentStaticMemoryBase +
			hRap->sMemAllocInfo.staticMemorySize) )
		{
			BDBG_ERR(("BRAP_P_AllocAligned : No more Static Memory available"));
			return (void *) 0xFFFFFFFF ;
		}

		/* Align the address to the specified Bit position */
		ui32_AllocMem = BRAP_P_AlignAddress(ui32_AllocMem, uiAlignBits) ;

		hRap->sMemAllocInfo.currentStaticMemoryPointer += ui32_adjSize;

		/* Set the memory that is allocate to zero, this is asure allocated memory is a known value */
		{
			uint32_t offset;
			
			for(offset = 0; offset <= ui32_adjSize; offset += 4)
			{
				/* may over the allocated buffer by upto 3 bytes, but this should be okay 
				   since the override will occur to unallocated memory, due to the simple
				   memory allocator that currently exist */
				BRAP_P_DRAMWRITE((BARC_Handle)hRap->hRegister, (ui32_AllocMem + offset), 0x00);
			}
		}


		/* TODO : Add Boundary checks */

		hRap->uiTotalMemUsed+=ulSize;
		BDBG_MSG(("Allocated Memory : %08lx, size %08lx, prealigned = %08lx", ui32_AllocMem, ulSize,hRap->sMemAllocInfo.currentStaticMemoryPointer ));


		return (void *)ui32_AllocMem ;	
#endif
}

/* BRAP_P_Free : A wrapper around the BMEM module to  free device memory.
 * Device memory is actually freed only for non-7411 chips. For 7411 chips, this
 * function just returns.
 */
BERR_Code BRAP_P_Free(
        BRAP_Handle  hRap,      /* [in] The RAP device handle */
        void *pMemPtr           /* [in] Pointer to device memory to be freed */
#if (BRAP_SECURE_HEAP==1)    
        , bool bAllocSecureMem  /*[in ] Enabled if the memory is allocated from secure memory*/
#endif      
        )           
{
    BERR_Code err = BERR_SUCCESS;
#ifndef BCHP_7411_VER
    uint32_t ui32PhyAddr=0;
#endif
    BDBG_ASSERT(hRap);

#ifdef BCHP_7411_VER
    BSTD_UNUSED(hRap);
    BSTD_UNUSED(pMemPtr);
    BSTD_UNUSED(err);
    return BERR_SUCCESS;
#else
    /* For non-7411 chips */

    if((pMemPtr !=NULL) && (pMemPtr!=(void *)BRAP_P_INVALID_DRAM_ADDRESS))
    {
#if (BRAP_SECURE_HEAP==1)
        if(hRap->sSettings.hSecureMemory!=NULL && bAllocSecureMem==true)
        {
            BDBG_MSG(("Freeing memory [0x%x] from Secure region", pMemPtr));        
            err = BMEM_Free(hRap->sSettings.hSecureMemory, pMemPtr);    
        }
        else
        {
#endif  
		BRAP_P_ConvertAddressToOffset(hRap->hHeap, pMemPtr, &ui32PhyAddr);


            BDBG_MSG(("Freeing memory 0x%x (physical addr=0x%x)", pMemPtr, ui32PhyAddr));
            err = BMEM_Free(hRap->hHeap, pMemPtr);

#if (BRAP_SECURE_HEAP==1)
        }
#endif
    }
    
    return err;
#endif /* For non-7411 chips */
}


/* BRAP_P_ConvertAddressToOffset: Wrapper on BMEM function BMEM_ConvertAddressToOffset.
 * Since for 7411 chips, RAP PI doesn't get virtual pointer to device memory and access
 * device memory using physical address only, this function returns device offset equal
 * to virtual pointer to device memory for these chips. For all other chips it calls 
 * BMEM function BMEM_ConvertAddressToOffset for this conversion.
 */

BERR_Code BRAP_P_ConvertAddressToOffset(
	BMEM_Handle  Heap,    /* Heap that contains the memory block. */
	void        *pvAddress, /* Address of the memory block */
	uint32_t    *pulOffset)   /* [out] Returned device offset. */
{
	BERR_Code err = BERR_SUCCESS;
	
#ifndef BCHP_7411_VER /* For chips other than 7411 */
	err = BMEM_ConvertAddressToOffset(Heap, pvAddress, pulOffset);
#else
	BSTD_UNUSED(Heap);
	*pulOffset = (uint32_t) pvAddress;
#endif
	return err;
}

/* BRAP_P_ConvertAddressToOffset_isr: Isr version of BRAP_P_ConvertAddressToOffset */

BERR_Code BRAP_P_ConvertAddressToOffset_isr(
	BMEM_Handle  Heap,    /* Heap that contains the memory block. */
	void        *pvAddress, /* Address of the memory block */
	uint32_t    *pulOffset)   /* [out] Returned device offset. */
{
	BERR_Code err = BERR_SUCCESS;
	
#ifndef BCHP_7411_VER /* For chips other than 7411 */
	err = BMEM_ConvertAddressToOffset_isr(Heap, pvAddress, pulOffset);
#else
	BSTD_UNUSED(Heap);
	*pulOffset = (uint32_t) pvAddress;
#endif
	return err;
}

/* BRAP_P_ConvertOffsetToAddress: Wrapper on BMEM function BMEM_ConvertOffsetToAddress.
 * Since for 7411 chips, RAP PI doesn't get virtual pointer to device memory and access
 * device memory using physical address only, this function returns device offset equal
 * to virtual pointer to device memory for these chips. For all other chips it calls 
 * BMEM function BMEM_ConvertOffsetToAddress for this conversion.
 */
BERR_Code BRAP_ConvertOffsetToAddress
(
	BMEM_Handle   hHeap,    /* Heap that contains the memory block */
	uint32_t      ulOffset,  /* Device offset within the heap. */
	void        **ppvAddress)  /* [out] Returned address. */
{
	BERR_Code err = BERR_SUCCESS;
	
#ifndef BCHP_7411_VER /* For chips other than 7411 */
	err = BMEM_ConvertOffsetToAddress(hHeap, ulOffset, ppvAddress);
#else
	BSTD_UNUSED(hHeap);
	*ppvAddress = (void *)ulOffset;
#endif
	return err;
}

void BRAP_P_DownloadDspFwInMem(BARC_Handle hArc, uint32_t *data,
									uint32_t memAdr,
									uint32_t size)
{
	uint32_t index;

	if (data==NULL) {
		/* Initialize this buffer with zeros */
		for (index = 0; index < size/4; index++) {
			BRAP_P_DRAMWRITE(hArc, memAdr,0x0);
			memAdr += 4;
		}
	}
	else {
#ifndef BCHP_7411_VER /* For chips other than 7411 */	
		for (index = 0; index < size/4; index++) {
			BRAP_P_DRAMWRITE(hArc, memAdr,*(data + index));
			memAdr += 4;
		}
#else
              /* For 7411, we can use a BARC_Mem_Write to do a bulk write and speed thigns up */
		BARC_Mem_Write(hArc, memAdr, (void *) data,  size);
#endif
	}

	BRAP_P_DummyMemRead32(hArc, memAdr);
}

#ifndef BCHP_7411_VER /* For chips other than 7411 */	
/* We can use this function to change the endian-ness of the contents
  * and download to DRAM.This Function is added for debug pupose
  * for swapping the F/W contents and not used by any one
  */
void BRAP_P_DownloadDspFwInMemSwapped(BARC_Handle hArc, 
									uint32_t *data,
									uint32_t memAdr,
									uint32_t size)
{
	uint32_t index;
	uint32_t tempValue;

	if (data==NULL) {
		/* Initialize this buffer with zeros */
		for (index = 0; index < size/4; index++) {
			BRAP_P_DRAMWRITE(hArc, memAdr,0x0);
			memAdr += 4;
		}
	}
	else {
		for (index = 0; index < size/4; index++) {
			tempValue = *(data + index );
			tempValue = ( ( tempValue & 0x000000FF ) << 24 ) |
					      ( ( tempValue & 0x0000FF00 ) << 8 ) |
					      ( ( tempValue & 0x00FF0000 ) >> 8 ) |
					      ( ( tempValue & 0xFF000000 ) >> 24 );
			BRAP_P_DRAMWRITE(hArc, memAdr,tempValue);
			memAdr += 4;
		}
	}

	BRAP_P_DummyMemRead32(hArc, memAdr);
}
#endif

#ifndef EMULATION
void BRAP_MemWrite32(BARC_Handle hArc,   /* ARC Handle */
					uint32_t	offset, /* Memory offset to write */
					uint32_t	data   /* Data to write */
					)
{
#if BRAP_P_USE_BARC ==0
	BSTD_UNUSED(hArc);
	*((volatile uint32_t *)offset) = data;
#else
	BARC_Mem_Write(hArc, offset, &data, 4);
#endif
}

void BRAP_MemWrite32_isr(BARC_Handle hArc,   /* ARC Handle */
					uint32_t	offset, /* Memory offset to write */
					uint32_t	data   /* Data to write */
					)
{
#if BRAP_P_USE_BARC ==0
	BSTD_UNUSED(hArc);
	*((volatile uint32_t *)offset) = data;
#else
	BARC_Mem_Write_isr(hArc, offset, &data, 4);
#endif
}

void BRAP_MemWrite8(BARC_Handle hArc,   /* ARC Handle */
					uint32_t	offset, /* Memory offset to write */
					uint8_t	data   /* Data to write */
					)
{
#if BRAP_P_USE_BARC ==0
	BSTD_UNUSED(hArc);
	*((volatile uint8_t *)offset) = data;
#else
	BARC_Mem_Write(hArc, offset, &data, 1);
#endif
}

uint32_t BRAP_MemRead32(BARC_Handle hArc,      /* ARC Handle */
						uint32_t	offset /* Memory offset to write */
					   )
{
	uint32_t ui32ValRead;

#if BRAP_P_USE_BARC ==0
	BSTD_UNUSED(hArc);
	ui32ValRead = *((volatile uint32_t *)offset);
#else
	BARC_Mem_Read(hArc, offset, &ui32ValRead, 4);
#endif

	return ui32ValRead;
}

uint32_t BRAP_MemRead32_isr(BARC_Handle hArc,      /* ARC Handle */
						uint32_t	offset /* Memory offset to write */
					   )
{
	uint32_t ui32ValRead;

#if BRAP_P_USE_BARC ==0
	BSTD_UNUSED(hArc);
	ui32ValRead = *((volatile uint32_t *)offset);
#else
	BARC_Mem_Read_isr(hArc, offset, &ui32ValRead, 4);
#endif

	return ui32ValRead;
}

uint8_t BRAP_MemRead8(	BARC_Handle hArc,      /* ARC Handle */
						uint32_t	offset /* Memory offset to write */
					   )
{
	uint8_t ui8ValRead;

#if BRAP_P_USE_BARC ==0
	BSTD_UNUSED(hArc);
	ui8ValRead = *((volatile uint8_t *)offset);
#else
	BARC_Mem_Read(hArc, offset, &ui8ValRead, 1);
#endif

	return ui8ValRead;

}

void BRAP_DSP_Write32(BARC_Handle	hArc,
					uint32_t		offset,
					uint32_t		data,
					bool			bIsrCxt)
{
	if (bIsrCxt)
		BRAP_Write32_isr(hArc, offset, data);
	else
		BRAP_Write32(hArc, offset, data);
}

uint32_t BRAP_DSP_Read32(BARC_Handle hArc,
					uint32_t		offset,
					bool			bIsrCxt)
{
	uint32_t regVal;

	if(bIsrCxt)
		regVal = BRAP_Read32_isr(hArc, offset);
	else
		regVal = BRAP_Read32(hArc, offset);

	return regVal;
}

#endif /* EMULATION */


#ifndef BCHP_7411_VER /* For chips other than 7411 */

/* Read Physical Memory through MSA Client.
   It reads 8 32-bit words at a time.
   So uiPhyAddr should be a OWORD address */
/* This PI is not actively used. It is meant to be used only for debug purposes */
void BRAP_P_ReadPhyMem( BREG_Handle hReg,
						uint32_t	ui32PhyAddr,
						uint32_t*	pData)
{
	uint32_t ui32RegVal = 0;
	int iLoopCnt = 0;

	BDBG_ASSERT(hReg);
	BDBG_ASSERT(pData);

	/* write the command */
	BRAP_Write32(hReg, BCHP_MEMC_0_MSA_CMD_TYPE, 1);

	/* write the physical address */
	BRAP_Write32(hReg, BCHP_MEMC_0_MSA_CMD_ADDR, ui32PhyAddr);
	
	/* Wait till MSA is busy */
	do{
		ui32RegVal = BRAP_Read32(hReg, BCHP_MEMC_0_MSA_STATUS);
		iLoopCnt++;
		if(iLoopCnt > 1000)
		{
			BDBG_ERR(("\nMSA Read: Timeout!!!\n"));
			break;
		}
	}while(BCHP_GET_FIELD_DATA(ui32RegVal, MEMC_0_MSA_STATUS, BUSY) != 0);

	/* read back the data */
	*pData = BRAP_Read32(hReg, BCHP_MEMC_0_MSA_RD_DATA7);
	*(pData+1) = BRAP_Read32(hReg, BCHP_MEMC_0_MSA_RD_DATA6);
	*(pData+2) = BRAP_Read32(hReg, BCHP_MEMC_0_MSA_RD_DATA5);
	*(pData+3) = BRAP_Read32(hReg, BCHP_MEMC_0_MSA_RD_DATA4);
	*(pData+4) = BRAP_Read32(hReg, BCHP_MEMC_0_MSA_RD_DATA3);
	*(pData+5) = BRAP_Read32(hReg, BCHP_MEMC_0_MSA_RD_DATA2);
	*(pData+6) = BRAP_Read32(hReg, BCHP_MEMC_0_MSA_RD_DATA1);
	*(pData+7) = BRAP_Read32(hReg, BCHP_MEMC_0_MSA_RD_DATA0);
	
	return;
}

/* Write Physical Memory through MSA Client.
   It writess 8 32-bit words at a time.
   So uiPhyAddr should be a OWORD address */
/* This PI is not actively used. It is meant to be used only for debug purposes */   
void BRAP_P_WritePhyMem( BREG_Handle hReg,
						 uint32_t	ui32PhyAddr,
						 uint32_t*	pData)
{
	uint32_t ui32RegVal = 0;
	int iLoopCnt = 0;

	BDBG_ASSERT(hReg);
	BDBG_ASSERT(pData);

	/* read back the data */
	BRAP_Write32(hReg, BCHP_MEMC_0_MSA_WR_DATA7, *pData);
	BRAP_Write32(hReg, BCHP_MEMC_0_MSA_WR_DATA6, *(pData+1));
	BRAP_Write32(hReg, BCHP_MEMC_0_MSA_WR_DATA5, *(pData+2));
	BRAP_Write32(hReg, BCHP_MEMC_0_MSA_WR_DATA4, *(pData+3));
	BRAP_Write32(hReg, BCHP_MEMC_0_MSA_WR_DATA3, *(pData+4));
	BRAP_Write32(hReg, BCHP_MEMC_0_MSA_WR_DATA2, *(pData+5));
	BRAP_Write32(hReg, BCHP_MEMC_0_MSA_WR_DATA1, *(pData+6));
	BRAP_Write32(hReg, BCHP_MEMC_0_MSA_WR_DATA0, *(pData+7));

	/* write the command */
	BRAP_Write32(hReg, BCHP_MEMC_0_MSA_CMD_TYPE, 0x11);

	/* write the physical address */
	BRAP_Write32(hReg, BCHP_MEMC_0_MSA_CMD_ADDR, ui32PhyAddr);
	
	/* Wait till MSA is busy */
	do{
		ui32RegVal = BRAP_Read32(hReg, BCHP_MEMC_0_MSA_STATUS);
		iLoopCnt++;
		if(iLoopCnt > 1000)
		{
			BDBG_ERR(("\nMSA Write: Timeout!!!\n"));
			break;
		}
	}while(BCHP_GET_FIELD_DATA(ui32RegVal, MEMC_0_MSA_STATUS, BUSY) != 0);
		
	return;
}
#endif 



/* This is a static routine that checks if an output port is used 
   in a RAP channel. This routine checks sModuleHandles, 
   sSimulPtModuleHandles and sCloneOpPathHandles to decide if an 
   output port is in use by this channel or not.
   If yes, it returns output params that indicate 
   i) channel pair 
   ii) whether cloned port
   iii) whether used for simulPt mode
   If neither from sClone or sSimulPt handles, then it is from a
   normal module handle.

   Note: To get eChanPair, the caller of this API should also pass 
   valid pClone and pSimulPt.*/
static BERR_Code
BRAP_P_GetInfoForOpPortUsedbyChannel(
	const BRAP_ChannelHandle 	hRapCh,     /* [in] The RAP Channel handle */
	const BRAP_OutputPort	    eOutputPort,/* [in] Output port */
    bool                        *pFlag,     /* [out] TRUE: output port eOutputPort 
                                                    is used by the channel hRapCh 
                                                FALSE: is not used by this channel*/
    BRAP_OutputChannelPair      *pChanPair, /* [out] Channel pair if output port is used */
    bool                        *pClone,    /* [out] TRUE: if output port is cloned port
                                                     FALSE: otherwise.
                                               This output param should be used by the 
                                               caller only if pFlag is TRUE.
                                               This is an optional output parameter. */
    bool                        *pSimulPt   /* [out] TRUE: if output port is used for 
                                               simulPt mode
                                                     FALSE: otherwise.
                                               This is an optional output parameter. */
)
{
    unsigned int i;
#ifndef BCHP_7411_VER
    unsigned int j;
#endif
    BERR_Code ret = BERR_SUCCESS;
    
    BDBG_ENTER(BRAP_P_GetInfoForOpPortUsedbyChannel);
	
    /* Validate input params */
    BDBG_ASSERT(hRapCh);
    BDBG_ASSERT(pFlag);

    /* Pre-init out params */
    *pFlag = false;
    if(pClone != NULL)
    {   
        *pClone = false;
    }
    if(pSimulPt != NULL)
    {
        *pSimulPt = false;
    }

    /* If user is interested in pChanPair, he should also
       pass a valid pClone and pSimulPt */
    if((pChanPair != NULL) && 
       ((pClone == NULL) || (pSimulPt == NULL)))
    {
        BDBG_ERR(("BRAP_P_GetInfoForOpPortUsedbyChannel: All pChanPair, pClone and pSimulPt be valid"));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
        
    for(i = 0; i < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; i++)
    {
        /* Check if this port is the main port for this channel pair */
        if (hRapCh->sModuleHandles.hOp[i] != NULL)
        {
            if (hRapCh->sModuleHandles.hOp[i]->eOutputPort == eOutputPort)
            {
                *pFlag = true;
                if(pChanPair != NULL)
                {
                    *pChanPair = (BRAP_OutputChannelPair)i;
                }

                BDBG_MSG(("Output port %d is used as the main port for channel pair %d in this channel.", eOutputPort, i));
                return ret;
            }
            /* If this port is the Flex or SPDIF, then it may not show up in the stored handles if it is used in conjunction with the Mai. 
            So check the Mai Mux Select */            
            else if ((hRapCh->sModuleHandles.hOp[i]->eOutputPort == BRAP_OutputPort_eMai)
                        && (hRapCh->hRap->sOutputSettings[BRAP_OutputPort_eMai].uOutputPortSettings.sMaiSettings.eMaiMuxSelector == eOutputPort))
            {
                *pFlag = true;
                if(pChanPair != NULL)
                {
                    *pChanPair = (BRAP_OutputChannelPair)i;
                }

                BDBG_MSG(("Output port %d is used in conjunction with Mai as the main port for channel pair %d in this channel.", eOutputPort, i));
                return ret;
            }
        } /* end of main ports */

        /* Check if this port is the simulPt port for this channel pair */        
        if (hRapCh->sSimulPtModuleHandles.hOp[i] != NULL)
        {
            if (hRapCh->sSimulPtModuleHandles.hOp[i]->eOutputPort == eOutputPort)
            {
                *pFlag = true;
                if(pChanPair != NULL)
                {
                    *pChanPair = (BRAP_OutputChannelPair)i;
                }
                if(pSimulPt != NULL)
                {
                    *pSimulPt = true;
                }

                BDBG_MSG(("Output port %d is used as the simul mode (pass thru context) port for channel pair %d in this channel.", eOutputPort, i));
                return ret;
            }
            /* If this port is the Flex or SPDIF, then it may not show up in the stored handles if it is used in conjunction with the Mai. 
            So check the Mai Mux Select */            
            else if ((hRapCh->sSimulPtModuleHandles.hOp[i]->eOutputPort == BRAP_OutputPort_eMai)
                        && (hRapCh->hRap->sOutputSettings[BRAP_OutputPort_eMai].uOutputPortSettings.sMaiSettings.eMaiMuxSelector == eOutputPort))
            {
                *pFlag = true;
                if(pChanPair != NULL)
                {
                    *pChanPair = (BRAP_OutputChannelPair)i;
                }
                if(pSimulPt != NULL)
                {
                    *pSimulPt = true;
                }

                BDBG_MSG(("Output port %d is used in conjunction with Mai as the simul port for channel pair %d in this channel.", eOutputPort, i));
                return ret;
            }            
        } /* end of simulPt ports */

#ifndef BCHP_7411_VER /* For chips other than 7411 */
        /* Check if this port is cloned for this channel pair */
        for(j = 0; j < BRAP_RM_P_MAX_OUTPUTS; j++)
        {
            if (hRapCh->sCloneOpPathHandles[i][j].hOp != NULL)
            {
                if (hRapCh->sCloneOpPathHandles[i][j].hOp->eOutputPort == eOutputPort)
                {
                    *pFlag = true;
                    if(pClone != NULL)
                    {   
                        *pClone = true;
                    }
                    if(pChanPair != NULL)
                    {
                        *pChanPair = (BRAP_OutputChannelPair)i;
                    }

                    BDBG_MSG(("Output port %d is used as the cloned port for channel pair %d in this channel.", eOutputPort, i));
                    return ret;
                } 
                /* If this port is the Flex or SPDIF, then it may not show up in the stored handles if it is used in conjunction with the Mai. 
                So check the Mai Mux Select */            
                else if ((hRapCh->sCloneOpPathHandles[i][j].hOp->eOutputPort == BRAP_OutputPort_eMai)
                            && (hRapCh->hRap->sOutputSettings[BRAP_OutputPort_eMai].uOutputPortSettings.sMaiSettings.eMaiMuxSelector == eOutputPort))
                {
                    *pFlag = true;
                    if(pClone != NULL)
                    {   
                        *pClone = true;
                    }
                    if(pChanPair != NULL)
                    {
                        *pChanPair = (BRAP_OutputChannelPair)i;
                    }

                    BDBG_MSG(("Output port %d is used in conjunction with Mai as the cloned port for channel pair %d in this channel.", eOutputPort, i));
                    return ret;
                }                         
             }
        }/* end of cloned ports */
#endif /* Not for 7411 */
    }

    BDBG_LEAVE(BRAP_P_GetInfoForOpPortUsedbyChannel);
    return ret;
}

/* This routine checks if an output port is used in a RAP channel.
   This routine checks sModuleHandles, sSimulPtModuleHandles and
   sCloneOpPathHandles to decide if an output port is in use by 
   this channel or not.
   If an output port is used by a channel, this routine also informs
   the caller if the output port is a cloned port or not. This is an
   optional output parameter. */    
BERR_Code
BRAP_P_IsOpPortUsedbyChannel(
	const BRAP_ChannelHandle 	hRapCh,     /* [in] The RAP Channel handle */
	const BRAP_OutputPort	    eOutputPort,/* [in] Output port */
    bool                        *pFlag,     /* [out] TRUE: output port eOutputPort 
                                                    is used by the channel hRapCh 
                                                FALSE: is not used by this channel*/
    bool                        *pClone     /* [out] TRUE: if output port is cloned port
                                                     FALSE: otherwise.
                                               This output param should be used by the 
                                               caller only if pFlag is TRUE.
                                               This is an optional output parameter. */
)
{
    BERR_Code ret = BERR_SUCCESS;
    
    BDBG_ENTER(BRAP_P_IsOpPortUsedbyChannel);
	
    /* Validate input params */
    BDBG_ASSERT(hRapCh);
    BDBG_ASSERT(pFlag);

    *pFlag = false;
    if(pClone != NULL)
    {   
        *pClone = false;
    }

    ret = BRAP_P_GetInfoForOpPortUsedbyChannel(
                        hRapCh, 
                        eOutputPort, 
                        pFlag, 
                        NULL /*pChanPair*/, 
                        pClone, 
                        NULL /*pSimulPt*/
                        );
    BDBG_LEAVE(BRAP_P_IsOpPortUsedbyChannel);
    return BERR_TRACE(ret);    
}

/* Returns the channel pair along with used for whether clone/SimulPt info if 
   the output is used by the channel */
BERR_Code
BRAP_P_GetChannelPairUsingOpPort_isr(
	const BRAP_ChannelHandle hRapCh,        /* [in] The RAP Channel handle */
	const BRAP_OutputPort	 eOutputPort, 	/* [in] Output port */
    unsigned int             *pChnPair,     /* [out] channel pair if eOutputPort 
                                                is used by the channel hRapCh */ 
    bool                     *pClone,       /* [out]TRUE: chan pair corresponds to 
                                               sCloneHandles */
    bool                     *pSimulPt      /* [out]TRUE: chan pair corresponds to 
                                               sSimulPtHandles */
                                            /* Note: if valid chan pair and both 
                                               pClone and pSimulPt are false, 
                                               then this is from sModuleHandle */
)
{
    BERR_Code   ret = BERR_SUCCESS;
    bool        bFlag = false;
    
    BDBG_ENTER(BRAP_P_GetChannelPairUsingOpPort_isr);
	
    /* Validate input params */
    BDBG_ASSERT(hRapCh);
    BDBG_ASSERT(pChnPair);
    BDBG_ASSERT(pClone);
    BDBG_ASSERT(pSimulPt);
    
    *pClone = false;
    *pSimulPt = false;
    *pChnPair = BRAP_INVALID_VALUE;

    ret = BRAP_P_GetInfoForOpPortUsedbyChannel(
                        hRapCh, 
                        eOutputPort, 
                        &bFlag, 
                        (BRAP_OutputChannelPair *)pChnPair,
                        pClone, 
                        pSimulPt);
    if(ret == BERR_SUCCESS)
    {
        if(bFlag == false)
        {
            ret = BERR_NOT_INITIALIZED;
        }
    }

    BDBG_LEAVE(BRAP_P_GetChannelPairUsingOpPort_isr);
    return BERR_TRACE(ret);    
}

BERR_Code
BRAP_P_GetPllForOp(
	const BRAP_Handle 	hRap, /* [in] The RAP handle */
	const BRAP_OutputPort	eOutputPort,	/* [in] Output port */
       BRAP_OP_Pll  * pPll  /* [out] the Pll associated with this output port */
)
{
    BERR_Code ret = BERR_SUCCESS;
    
    BDBG_ENTER(BRAP_P_GetPllForOp);
    BDBG_ASSERT(hRap);
    BDBG_ASSERT(pPll);
    
    /* Make sure  port is configured */
    if(hRap->bOpSettingsValid[eOutputPort] == false)    
    {
        BDBG_ERR(("BRAP_P_GetPllForOp: Output port %d is not configured. Please configure before calling this PI.", eOutputPort));
        return BERR_TRACE(BRAP_ERR_OUTPUT_NOT_CONFIGURED);
    }       
        switch(eOutputPort)
        {
            case BRAP_OutputPort_eSpdif:
                *pPll = hRap->sOutputSettings[eOutputPort].uOutputPortSettings.sSpdifSettings.ePll;                            
                break;
            case BRAP_OutputPort_eI2s0:
            case BRAP_OutputPort_eI2s1:
            case BRAP_OutputPort_eI2s2:              
                *pPll = hRap->sOutputSettings[eOutputPort].uOutputPortSettings.sI2sSettings.ePll;                             
                break;
            case BRAP_OutputPort_eMai:
                    if (hRap->sOutputSettings[eOutputPort].uOutputPortSettings.sMaiSettings.eMaiMuxSelector == BRAP_OutputPort_eSpdif )
                    {
                        *pPll =  hRap->sOutputSettings[BRAP_OutputPort_eSpdif].uOutputPortSettings.sSpdifSettings.ePll; 
                     }
                    else if (hRap->sOutputSettings[eOutputPort].uOutputPortSettings.sMaiSettings.eMaiMuxSelector == BRAP_OutputPort_eFlex )
                    {
                        *pPll = hRap->sOutputSettings[BRAP_OutputPort_eFlex].uOutputPortSettings.sFlexSettings.ePll;                   
                    }                               
                break;
            case BRAP_OutputPort_eFlex:
                *pPll = hRap->sOutputSettings[eOutputPort].uOutputPortSettings.sFlexSettings.ePll; 
                break;
            case BRAP_OutputPort_eDac0:
            case BRAP_OutputPort_eDac1:
                *pPll = BRAP_RM_P_INVALID_INDEX;
                break;
            default:
                    BDBG_ERR(("BRAP_P_GetPllForOp:  Output port type %d not supported",  eOutputPort ));
                    return BERR_TRACE(BRAP_ERR_OUPUT_PORT_NOT_SUPPORTED);   
        }

    BDBG_MSG(("BRAP_P_GetPllForOp: Pll %d is associated with ouput port %d", *pPll, eOutputPort));

    BDBG_LEAVE (BRAP_P_GetPllForOp);        
    return ret;
}

BERR_Code BRAP_P_ConvertDelayToStartWRPoint(
	unsigned int 			uiDelay /* micro sec */,
	BRAP_BufDataMode 		eBufDataMode,
	BAVC_AudioSamplingRate 	eSamplingRate,
	unsigned int			uiIpBitsPerSample,
    unsigned int	        *pStartWRPoint
	)
{
	unsigned int uiDelayInBytes = 0;
	unsigned int uiSR = 0;
    BERR_Code err = BERR_SUCCESS;
    
	BDBG_ENTER(BRAP_P_ConvertDelayToStartWRPoint);
    BDBG_ASSERT(pStartWRPoint);

	err = BERR_TRACE(BRAP_P_ConvertSR(eSamplingRate, &uiSR));
    if(err != BERR_SUCCESS)
    {
        return err;
    }
    
	if(eBufDataMode == BRAP_BufDataMode_eStereoInterleaved)
	{
	 	/* For Stereo Interleaved case, 
 			delayInBytes = 2 * Delay * Sampling rate * Bits per Sample / 8 */
 		uiDelayInBytes = (((2 * uiSR * uiIpBitsPerSample)/1000) * uiDelay)/1000;
		uiDelayInBytes >>= 3; /* Divide by 8: bits to byte conversion */
	}
	else
	{
		/* For Mono or Stereo Non-interleaved case,
 			delayInBytes = Delay * Sampling rate * Bits per Sample / 8; */
 		uiDelayInBytes = (((uiSR * uiIpBitsPerSample)/1000) * uiDelay)/1000;
		uiDelayInBytes >>= 3; /* Divide by 8: bits to byte conversion */
	}

    *pStartWRPoint = uiDelayInBytes;
        
	BDBG_LEAVE(BRAP_P_ConvertDelayToStartWRPoint);
	return err;
}

/* 
    Recommended Burst Repetition Periods for various algo types are as follows:
    AC3 and DTS = PER_3
    MPEG-2 Layers 1, 2, and 3 < 32kHz = PER_64
    MPEG-1, MPEG-2/AAC, and any other MPEG-2 formats = PER_32
*/
BERR_Code BRAP_P_GetBurstRepetitionPeriodForAlgo(
    BAVC_StreamType                 eStreamType,
                                        /* [in] audio stream type*/
    BRAP_DSPCHN_AudioType           eAudioType, 
                                        /* [in] audio type for which 
                                           burst repetition period is 
                                           sought */
    BRAP_SPDIFFM_P_BurstRepPeriod   *pBurstRepPeriod
                                        /* [out] corresponding burst 
                                           repetition period for the 
                                           algo type passed */
    )
{
    BERR_Code ret = BERR_SUCCESS;
    BDBG_ENTER(BRAP_P_GetBurstRepetitionPeriodForAlgo);

    /* validate input */
    BDBG_ASSERT(pBurstRepPeriod);

    switch(eAudioType)
    {
        /* Here we return PER_32 for both MPEG-1 and MPEG-2. For
           MPEG-2 < 32KHz, PER_64 is set from SamplingRateChange_isr */
        case BRAP_DSPCHN_AudioType_eMpeg:
            if((eStreamType == BAVC_StreamType_ePS) ||
               (eStreamType == BAVC_StreamType_eTsMpeg)) 
                *pBurstRepPeriod = BRAP_SPDIFFM_P_BurstRepPeriod_ePer32;
            else
                *pBurstRepPeriod = BRAP_SPDIFFM_P_BurstRepPeriod_eNone;
            break;

    	case BRAP_DSPCHN_AudioType_eAc3:
    	case BRAP_DSPCHN_AudioType_eDra:				
    	case BRAP_DSPCHN_AudioType_eAc3Lossless:
       	case BRAP_DSPCHN_AudioType_eDts:
       	case BRAP_DSPCHN_AudioType_eDtshd:
            *pBurstRepPeriod = BRAP_SPDIFFM_P_BurstRepPeriod_ePer3;
            break;
            
        /*Note: AC3+ simul mode uses SPDIFFM */
        case BRAP_DSPCHN_AudioType_eAc3Plus:
            *pBurstRepPeriod = BRAP_SPDIFFM_P_BurstRepPeriod_ePer4;
        break;
        
       	case BRAP_DSPCHN_AudioType_eAac:
       	case BRAP_DSPCHN_AudioType_eAacSbr:
            *pBurstRepPeriod = BRAP_SPDIFFM_P_BurstRepPeriod_ePer32;
            break;

        /* Not supported / un-compressed algo types */
       	case BRAP_DSPCHN_AudioType_eLpcmBd:
       	case BRAP_DSPCHN_AudioType_eLpcmHdDvd:            
       	case BRAP_DSPCHN_AudioType_eLpcmDvd:
		case BRAP_DSPCHN_AudioType_eWmaStd:
		case BRAP_DSPCHN_AudioType_eWmaPro:
		case BRAP_DSPCHN_AudioType_ePcmWav:            
		case BRAP_DSPCHN_AudioType_eMlp:
#ifdef RAP_SRSTRUVOL_CERTIFICATION  
		case BRAP_DSPCHN_AudioType_ePCM:          
#endif			
            *pBurstRepPeriod = BRAP_SPDIFFM_P_BurstRepPeriod_eNone;
            break;
            
        case BRAP_DSPCHN_AudioType_eInvalid:
        default: 
            return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    BDBG_LEAVE(BRAP_P_GetBurstRepetitionPeriodForAlgo);
    return ret;
}
    
BERR_Code BRAP_P_GetCurrentMemoryUsage(
	BRAP_Handle 			hRap,		/* [in] The RAP device handle */
	unsigned int			*puiTotalMemUsed	/* [out] Total device memory used by RAP PI */
	)
{
	BERR_Code err = BERR_SUCCESS;

    BDBG_ENTER(BRAP_P_GetCurrentMemoryUsage);
    
	BDBG_ASSERT(hRap);
	BDBG_ASSERT(puiTotalMemUsed);

	*puiTotalMemUsed = hRap->uiTotalMemUsed;

    BDBG_LEAVE(BRAP_P_GetCurrentMemoryUsage);
	return err;	
}


/***************************************************************************
Summary:
    This private routine is used to check if a new channel is compatible with
    existing channels.
Description:
    This private routine is used to check if a new channel is compatible with
    existing channels. This routine should be called at the very beginning of a 
    channel open(DEC/PB/CAP).

    The non support combinations of channels are as follows:
    1) Two DEC channels on the same output port i.e. 2 direct DEC channels
    2) Two DEC channels on different output ports where one is captured and
       mixed to another i.e. one direct DEC channel and another indirect DEC
       channel associated to an output port through a CAP channel.
    3) A CAP channel getting captured again by another CAP channel i.e. 
       one direct CAP channel and another indirect CAP channel

Returns:
	BERR_SUCCESS if compatible else error
See Also:
    BRAP_OP_P_GetAssociatedChannels
****************************************************************************/
BERR_Code BRAP_P_IsNewChanCompatible(
	BRAP_Handle 			hRap,		/* [in] The RAP device handle */
    BRAP_P_ChannelType      eNewChannelType,/*[in] The new RAP Channel type */
    BRAP_OutputPort         eOutputPort     /*[in] output port for the new channel */  
    )
{
	BERR_Code ret = BERR_SUCCESS;
    BRAP_ChannelHandle hDirChannels[BRAP_RM_P_MAX_MIXER_INPUTS];
    BRAP_ChannelHandle hIndirChannels[BRAP_RM_P_MAX_MIXER_INPUTS];
    int i;

    BDBG_ENTER(BRAP_P_IsNewChanCompatible);
    BDBG_ASSERT(hRap);

    ret = BRAP_OP_P_GetAssociatedChannels(hRap, &hDirChannels[0], &hIndirChannels[0], 
            eOutputPort, false /* bCalledFromISR */);
    if(ret != BERR_SUCCESS)
    {
        BDBG_ERR(("BRAP_OP_P_SetSamplingRate: Can't get associated channel handles"));
        return BERR_TRACE (ret);
    }

    if(eNewChannelType == BRAP_P_ChannelType_eDecode)
    {
        /* Invalid case#1: New chan = DEC and an already DEC chan in direct path */
        for(i=0; i<BRAP_RM_P_MAX_MIXER_INPUTS && hDirChannels[i]!=NULL; i++)
        {
            if(hDirChannels[i]->eChannelType == BRAP_P_ChannelType_eDecode)
            {
                BDBG_ERR(("BRAP_P_IsNewChanCompatible: Two direct DEC channels are not supported"));
                return BERR_TRACE(BERR_NOT_SUPPORTED);
            }
        }

        /* Invalid case#2: New chan = DEC and an already DEC chan in indirect path */
        for(i=0; i<BRAP_RM_P_MAX_MIXER_INPUTS && hIndirChannels[i]!=NULL; i++)
        {
            if(hIndirChannels[i]->eChannelType == BRAP_P_ChannelType_eDecode)
            {
                BDBG_ERR(("BRAP_P_IsNewChanCompatible: One direct DEC channels captured and"
                          "linked to another indirect DEC channel is not supported"));
                return BERR_TRACE(BERR_NOT_SUPPORTED);
            }
        }
    }

    /* Invalid case#3: A CAP channel getting captured again by another CAP channel */
    if(eNewChannelType == BRAP_P_ChannelType_eCapture)
    {
        for(i=0; i<BRAP_RM_P_MAX_MIXER_INPUTS && hIndirChannels[i]!=NULL; i++)
        {
            if(hIndirChannels[i]->eChannelType == BRAP_P_ChannelType_eCapture)
            {
                BDBG_ERR(("BRAP_P_IsNewChanCompatible: A CAP channel getting captured again"
                          "by another CAP channel is not supported"));
                return BERR_TRACE(BERR_NOT_SUPPORTED);
            }
        }    
    }        

    BDBG_LEAVE(BRAP_P_IsNewChanCompatible);
    return ret;
}


/***************************************************************************
Summary:
	Checks if Compressed channel being Mixed with PCM channel
Description:
 	This functions returns true of false depending on if compressed 
 	data on a channel is being mixed with PCm dat on other channel.
Returns:
        BERR_SUCCESS on success, else error.
        bIsCompressedMixedWithPCM would be true or false.
See Also:
 	None
**************************************************************************/
BERR_Code BRAP_P_CheckIfCompressedMixedWithPCM(
	const BRAP_Handle hRap,			/*[in] The Rap Handle*/
	const BRAP_OutputPort eOutputPort, /*[in] The output port for the current channel*/
	bool bNewChannelCompressed,	/*[in] Current channel compressed or not*/
	bool *bIsCompressedMixedWithPCM	/*[out] TRUE: if compressed and PCM are being mixed 
										     FALSE: If not*/
	)
{
	BERR_Code 			ret = BERR_SUCCESS;
	unsigned int			uiMixerId;
	BRAP_ChannelHandle	phChannels[BRAP_RM_P_MAX_MIXER_INPUTS];
	BRAP_ChannelHandle 	hRapCh = NULL;
	unsigned int 			i,j;

	BDBG_ENTER(BRAP_P_CheckIfCompressedMixedWithPCM);

	/*Validate the Input Parameters*/
	BDBG_ASSERT(hRap);

	/*Initialize the output value to false*/
	*bIsCompressedMixedWithPCM = false;

	/*Intialize the Array to Null*/
	for(i=0; i<BRAP_RM_P_MAX_MIXER_INPUTS; i++)
		phChannels[i]=NULL;

	/*Get the list of channels on the given Output Port*/
	ret = BRAP_P_GetChannelsUsingOpPort(hRap, eOutputPort, phChannels);
	if(BERR_SUCCESS == ret)
	{
		for(i=0; i<BRAP_RM_P_MAX_MIXER_INPUTS && phChannels[i];i++)
		{
			hRapCh = phChannels[i];
			
			/*If current channel has compressed data*/
			if(bNewChannelCompressed)
			{
				/* Since the compressed data can come only on decode channel and bNewChannelCompressed 
				     is true that means the current channel is Decode and since two decode channels can not
				     go on one Mixer input , so checking only for PlayBack and Capture Channel.*/
				if((hRapCh->bStarted == true)&&
                   ((hRapCh->eChannelType ==BRAP_P_ChannelType_eCapture) || 
				   (hRapCh->eChannelType ==BRAP_P_ChannelType_ePcmPlayback)))
			   	{
					*bIsCompressedMixedWithPCM = true;
					BDBG_ERR(("BRAP_P_CheckIfCompressedMixedWithPCM(): Channel cant Be mixed"));
					goto end;
				}
			}
			else	
			{
				/*Check for a Decode Channel on which Compressed Data can be present*/
				if(hRapCh->eChannelType ==BRAP_P_ChannelType_eDecode)
				{
					/*Get the Mixer being used by getting the Mixer Id*/
					ret = BRAP_RM_P_GetMixerForOpPort(hRap->hRm,eOutputPort, &uiMixerId);
					if (ret != BERR_SUCCESS)
					{
					        BDBG_ERR(("BRAP_P_CheckIfCompressedMixedWithPCM(): Cant get Mixer ID"));
					        return BERR_TRACE(ret);
	    				}

					for(j=0; j<BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; j++)
					{
						/*Check the Mixer Handle*/
						if((hRapCh->bStarted == true)&&
                           (hRapCh->sModuleHandles.hMixer[j] != NULL) && 
						   (hRapCh->sModuleHandles.hMixer[j]->uiMixerIndex == uiMixerId))
						{
							/*Check if Decode Channel has Compressed Data or not*/
							if(hRapCh->sModuleHandles.hMixer[j]->bCompress == true)
							{
								*bIsCompressedMixedWithPCM = true;
								BDBG_ERR(("BRAP_P_CheckIfCompressedMixedWithPCM(): Channel cant Be mixed"));
								goto end;
							}
						}

						/*Check the Mixer Handle for Simul Mode*/
						if((hRapCh->bStarted == true)&&
                           (hRapCh->sSimulPtModuleHandles.hMixer[j] != NULL )&& 
						   (hRapCh->sSimulPtModuleHandles.hMixer[j]->uiMixerIndex == uiMixerId))
						{
							/*Check if Decode Channel has Compressed Data or not*/
							if(hRapCh->sSimulPtModuleHandles.hMixer[j]->bCompress == true)
							{
								*bIsCompressedMixedWithPCM = true;
								BDBG_ERR(("BRAP_P_CheckIfCompressedMixedWithPCM(): Channel cant Be mixed"));
								goto end;
							}
						}
						#ifndef BCHP_7411_VER
						{
							unsigned int k;

							/*Check the Mixer Handle for Clonned outputs*/
							for(k=0;k<BRAP_RM_P_MAX_OUTPUTS;k++)
							{
								if((hRapCh->bStarted == true)&&
                                   (hRapCh->sCloneOpPathHandles[j][k].hMixer != NULL)&&
                                   (hRapCh->sCloneOpPathHandles[j][k].hMixer->uiMixerIndex == uiMixerId))
								{
									/*Check if Decode Channel has Compressed Data or not*/
									if(hRapCh->sCloneOpPathHandles[j][k].hMixer->bCompress == true)
									{
										*bIsCompressedMixedWithPCM = true;
										BDBG_ERR(("BRAP_P_CheckIfCompressedMixedWithPCM(): Channel cant Be mixed"));
										goto end;
									}
								}
							}
						}
						#endif
					}
				}
			}
		}
	}
		
end:	
	BDBG_LEAVE(BRAP_P_CheckIfCompressedMixedWithPCM);
	return ret;
}


#ifndef BCHP_7411_VER /* For chips other than 7411 */

/***************************************************************************
Summary:
    This API should be called for setting the Channel status params on-the-fly for SPDIF and HDMI.
    It should be called only after atleast one channel using this output port 
    is already running since it assumes that the sample rate info /dsp channel handle is 
    already available
    
    If there is any decode channel channel using this port, this PI will write the 
    new values to DRAM and the Fw will then program the CBIT registers.
    If there is no currently running decode channel using tihs port, the PI will 
    directly write to the CBIT registers.

    Note: if a channel using this port is started/restarted AFTER calling this PI, then the Channel status 
    params passed in BRAP_DEC_Start() [or corresponding PB/CAP start] will take effect.

See Also:
	BRAP_SetOutputConfig, BRAP_GetOutputDefaultConfig, BRAP_GetOutputConfig
****************************************************************************/
BERR_Code BRAP_P_SetCbit( 
	BRAP_Handle 				hRap,				/* [in] The Raptor Audio device handle*/
	const BRAP_OutputSettings	*pOutputSettings	/* [in] Output port settings*/ 
	)
{
    BERR_Code ret = BERR_SUCCESS;
    int i;
    BRAP_ChannelHandle hDirChannels[BRAP_RM_P_MAX_MIXER_INPUTS];
    bool bDone=false;
    unsigned int uiSpdiffmId, uiSpdiffmStreamId;

    BDBG_ENTER (BRAP_P_SetCbit);
    BDBG_ASSERT (hRap);   

    BDBG_MSG(("Entering BRAP_P_SetCbit"));        

    BDBG_MSG(("bProfessionalMode=%d", pOutputSettings->sSpdifChanStatusParams.bProfessionalMode)); 
    BDBG_MSG(("bLinear_PCM=%d", pOutputSettings->sSpdifChanStatusParams.bLinear_PCM));     
    BDBG_MSG(("bSwCopyRight=%d", pOutputSettings->sSpdifChanStatusParams.bSwCopyRight)); 
    BDBG_MSG(("uiPre_Emphasis=%d", pOutputSettings->sSpdifChanStatusParams.uiPre_Emphasis)); 
    BDBG_MSG(("uiChannel_Status_Mode=%d", pOutputSettings->sSpdifChanStatusParams.uiChannel_Status_Mode)); 
    BDBG_MSG(("ui16CategoryCode=%d", pOutputSettings->sSpdifChanStatusParams.ui16CategoryCode)); 
    BDBG_MSG(("uiSource_Number=%d", pOutputSettings->sSpdifChanStatusParams.uiSource_Number)); 
    BDBG_MSG(("bSeparateLRChanNum=%d", pOutputSettings->sSpdifChanStatusParams.bSeparateLRChanNum)); 
    
    BDBG_MSG(("ui16ClockAccuracy=%d", pOutputSettings->sSpdifChanStatusParams.ui16ClockAccuracy));
    BDBG_MSG(("bSampleWorldLength=%d", pOutputSettings->sSpdifChanStatusParams.bSampleWorldLength)); 
    BDBG_MSG(("uiSample_Word_Length=%d", pOutputSettings->sSpdifChanStatusParams.uiSample_Word_Length)); 
    BDBG_MSG(("uiOriginal_Sampling_Frequency=%d", pOutputSettings->sSpdifChanStatusParams.uiOriginal_Sampling_Frequency));
    BDBG_MSG(("uiCGMS_A =%d", pOutputSettings->sSpdifChanStatusParams.uiCGMS_A));	

    /* Intializing the hDirChannels to NULL*/
    for(i = 0; i < BRAP_RM_P_MAX_MIXER_INPUTS; i++)
    {
        hDirChannels[i] = NULL;
    }

    /* Find the relevant SPDIFFM stream and save the new channel status params*/
    switch (pOutputSettings->eOutput)
    {
         case BRAP_OutputPort_eSpdif:
         case BRAP_OutputPort_eFlex:
    BRAP_RM_P_GetSpdifFmForOpPort (hRap->hRm, 
                                   pOutputSettings->eOutput, 
                                   &uiSpdiffmId, 
                                   &uiSpdiffmStreamId);  
            break;
        case BRAP_OutputPort_eMai:
            BRAP_RM_P_GetSpdifFmForOpPort (hRap->hRm, 
                                       pOutputSettings->uOutputPortSettings.sMaiSettings.eMaiMuxSelector, 
                                       &uiSpdiffmId, 
                                       &uiSpdiffmStreamId); 
            break;
        default:
            BDBG_ERR(("Channel status bits can not be programmed for output port %d", pOutputSettings->eOutput));
            BDBG_ERR(("The only valid ports are SPDIF, Flex and Mai"));         
            return BERR_TRACE (BERR_INVALID_PARAMETER);
    }
    BDBG_MSG(("uiSpdiffmId=%d, uiSpdiffmStreamId=%d", uiSpdiffmId, uiSpdiffmStreamId));    
    hRap->hFmm[0]->hSpdifFm[uiSpdiffmStreamId]->sParams.sChanStatusParams = pOutputSettings->sSpdifChanStatusParams;
    
    /* Get channel handles using this output port. there can be upto 3 channels coz a mixer can take upto 3 inputs */
    ret = BRAP_P_GetChannelsUsingOpPort(hRap, pOutputSettings->eOutput, &hDirChannels[0]);
    if(ret != BERR_SUCCESS)
    {
        BDBG_ERR(("BRAP_P_SetCbit: No channels are connected to this output port. Please connect channels and then try again."));
        return BERR_TRACE (ret);
    }    

    /* If there's a decode channel running, write the new CBIT values to DRAM.
    If not => there's only a PB/CAP channel using this port (since this function 
    is called only if the port is already running), directly write to the CBIT registers.
    */

    /* Programming the CBIT for one channel is sufficent even if multiple channels use the same port*/

    for(i=0; i<BRAP_RM_P_MAX_MIXER_INPUTS; i++)
    {
        if((hDirChannels[i] != NULL)  
           && (hDirChannels[i]->eChannelType == BRAP_P_ChannelType_eDecode)
           && (hDirChannels[i]->bStarted == true))
        {
            /* If this is a decode channel and it is running, write the new CBITs to DRAM */
            BRAP_DSPCHN_P_InitSpdifChanStatusParams (
                hDirChannels[i]->sModuleHandles.hDspCh,
                pOutputSettings->sSpdifChanStatusParams);       
            BDBG_MSG(("Decode channel 0x%x is using this port. Wrote new CBIT params to the DRAM.", hDirChannels[i])); 
            /* There can be just one decode channel for this port*/
            bDone=true;
            break;
        }
    }    
    if (bDone == false)
    {
        /* There is no decode channel currently running. Directly write to the CBIT registers */
        for(i=0; i<BRAP_RM_P_MAX_MIXER_INPUTS; i++)
        {
            if((hDirChannels[i] != NULL)  
               && (hDirChannels[i]->eChannelType != BRAP_P_ChannelType_eDecode)
               && (hDirChannels[i]->bStarted == true))
            {
  

                /* BRAP_SPDIFFM_P_ProgramChanStatusBits() reads the new params from the hSpdifFm->sParams*/
                BRAP_SPDIFFM_P_ProgramChanStatusBits(hRap->hFmm[0]->hSpdifFm[uiSpdiffmStreamId]);
                BDBG_MSG(("PB/CAP channel 0x%x is using this port. Programmed CBIT registers for SPDIFFM Stream Id %d", 
                    hDirChannels[i],uiSpdiffmStreamId)); 
                bDone=true;
                break;
            }
        }         
    }

    BDBG_MSG(("Leaving BRAP_P_SetCbit"));   
    BDBG_LEAVE (BRAP_P_SetCbit);
    return ret;        

}
#endif

#if (BCHP_CHIP == 7400)
#if (BRAP_DSPCHN_P_HAS_NEW_TSM_SCHEME==1)
BERR_Code BRAP_P_ProgramAdaptRateCtrl (
		BRAP_ChannelHandle		hRapCh
		)
{

    BERR_Code ret = BERR_SUCCESS;
    uint32_t  ui32RegVal = 0,ui32RegVal1=0, ui32Offset=0,ui32Offset1=0,Index=0;   
    uint32_t i=0, uiCount=0,k=0;

    BDBG_ENTER (BRAP_P_ProgramAdaptRateCtrl);
    BDBG_ASSERT (hRapCh);    

    /* If this channel has a SrcCh which carries PCM data, program the 
    AUD_FMM_BF_CTRL_ADAPTRATE_X accordingly */
    ui32Offset = BCHP_AUD_FMM_BF_CTRL_ADAPTRATE_1_CFG - 
    BCHP_AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG;

    
    ui32Offset1=BCHP_AUD_DSP_CFG0_PPM_CONFIG_REG_CXT1 - BCHP_AUD_DSP_CFG0_PPM_CONFIG_REG_CXT0;
    
    Index =hRapCh->sRsrcGrnt.uiDspContextId;

    BRAP_Write32(hRapCh->hRegister, 
          BCHP_AUD_DSP_CFG0_PPM_CONFIG_REG_CXT0+(Index*ui32Offset1), 0xFFFFFFFF); 
    
    
    for(i=0; i < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; i++)
    {
    	if (hRapCh->sModuleHandles.hSrcCh[i] != NULL) 
    	{
            /* Make sure the SrcCh the SrcCh is carrying PCM data.
            Note: whether the srcch is carrying PCM or Comp is determined only 
            at start time. So make sure the Srcch has been started.*/
            if ((hRapCh->sModuleHandles.hSrcCh[i]->eState == BRAP_SRCCH_P_State_eRunning) 
                && (hRapCh->sModuleHandles.hSrcCh[i]->sParams.bCompress == false))
            {
                 uiCount = hRapCh->sRsrcGrnt.sOpResrcId[i].uiPpmId;  
                k++;

                 ui32RegVal1 = BRAP_Read32(hRapCh->hRegister, 
                            BCHP_AUD_DSP_CFG0_PPM_CONFIG_REG_CXT0 + (Index*ui32Offset1));
                 
                 switch(i)
                {
                    case 0:
                        ui32RegVal1 &= ~(BCHP_MASK (AUD_DSP_CFG0_PPM_CONFIG_REG_CXT0,PPM_MODULE0));                
                        ui32RegVal1 |= (BCHP_FIELD_DATA (AUD_DSP_CFG0_PPM_CONFIG_REG_CXT0, 
                                        PPM_MODULE0,  hRapCh->sRsrcGrnt.sOpResrcId[i].uiPpmId));
                        break;
                    case 1:
                        ui32RegVal1 &= ~(BCHP_MASK (AUD_DSP_CFG0_PPM_CONFIG_REG_CXT0,PPM_MODULE1));                
                        ui32RegVal1 |= (BCHP_FIELD_DATA (AUD_DSP_CFG0_PPM_CONFIG_REG_CXT0, 
                                        PPM_MODULE1,  hRapCh->sRsrcGrnt.sOpResrcId[i].uiPpmId));
                        break;                        
                    case 2:
                        ui32RegVal1 &= ~(BCHP_MASK (AUD_DSP_CFG0_PPM_CONFIG_REG_CXT0,PPM_MODULE2));                
                        ui32RegVal1 |= (BCHP_FIELD_DATA (AUD_DSP_CFG0_PPM_CONFIG_REG_CXT0, 
                                        PPM_MODULE2,  hRapCh->sRsrcGrnt.sOpResrcId[i].uiPpmId));
                        break;                        
                    case 3:
                        ui32RegVal1 &= ~(BCHP_MASK (AUD_DSP_CFG0_PPM_CONFIG_REG_CXT0,PPM_MODULE3));                
                        ui32RegVal1 |= (BCHP_FIELD_DATA (AUD_DSP_CFG0_PPM_CONFIG_REG_CXT0, 
                                        PPM_MODULE3,  hRapCh->sRsrcGrnt.sOpResrcId[i].uiPpmId));
                        
                } 

                BRAP_Write32(hRapCh->hRegister, 
                   BCHP_AUD_DSP_CFG0_PPM_CONFIG_REG_CXT0+(Index*ui32Offset1), ui32RegVal1);                

                ui32RegVal = BRAP_Read32(hRapCh->hRegister, 
                BCHP_AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG + (uiCount*ui32Offset));

                ui32RegVal &= ~(BCHP_MASK (AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, ADAPTIVE_SFIFO_SEL));                
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, 
                                ADAPTIVE_SFIFO_SEL, 
                                hRapCh->sModuleHandles.hSrcCh[i]->uiIndex));            

#if (BCHP_CHIP == 7401 && BCHP_VER >= BCHP_VER_C0) || (BCHP_CHIP == 7403) ||(BCHP_CHIP == 7400)
                ui32RegVal &= ~(BCHP_MASK (AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, 
                                TRIWINDOW_WIDTH_SEL));
                /* TODO: hardcoding window width field to 8 ie actual window 
                width of 256. This has to be changed later to be taken from 
                application */
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, 
                                TRIWINDOW_WIDTH_SEL, 8));
#else
                ui32RegVal &= ~(BCHP_MASK (AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, 
                                    TRIWINDOW_WIDTH_SEL));
                /* TODO: hardcoding window width field to 8 ie actual window 
                width of 256. This has to be changed later to be taken from 
                application */
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, 
                                TRIWINDOW_WIDTH_SEL, 8));
#endif
                /* Earlier, for a decode channel FW was setting the enable flag. 
                Now we're doing AdaptRate control for PCM channels also. So let the 
                PI set this flag always */
                ui32RegVal &= ~(BCHP_MASK (AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, ADAPTIVE_RATE_ENABLE));                
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, 
                                ADAPTIVE_RATE_ENABLE, 1));  


                /* Program ADAPTIVE_RATE_THRESHOLD required */
                ui32RegVal &= ~(BCHP_MASK (AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, ADAPTIVE_RATE_THRESHOLD));                
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, 
                                ADAPTIVE_RATE_THRESHOLD, 5));  



                BDBG_MSG(("For AdaptRateControl: channel pair %d, SrcFifo %d"
                    ", AdaptRateThreshold=5 and WindowWidth = 256 ", i, 
                    hRapCh->sModuleHandles.hSrcCh[i]->uiIndex));
                
                BRAP_Write32(hRapCh->hRegister, 
                            BCHP_AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG + (uiCount*ui32Offset), 
                            ui32RegVal);    
                uiCount ++;
                if (uiCount == 4)
                { 
                    BDBG_ERR(("uiCount cant exceed 4. There are only 4 SrcCh on 7401!")); 
                    return BERR_TRACE(BERR_INVALID_PARAMETER);
                }
            }
    	}
    }  

    /* also program it for all the cloned srcCh */
    for(i=0; i < BRAP_RM_P_MAX_OUTPUTS; i++)
    {
    	if (hRapCh->sCloneOpPathHandles[BRAP_OutputChannelPair_eLR][i].hSrcCh!= NULL) 
    	{  
            
    	  
            /* Make sure the SrcCh the SrcCh is carrying PCM data.
            Note: whether the srcch is carrying PCM or Comp is determined only 
            at start time. So make sure the Srcch has been started.*/
            if ((hRapCh->sCloneOpPathHandles[BRAP_OutputChannelPair_eLR][i].hSrcCh->eState == BRAP_SRCCH_P_State_eRunning) 
                && (hRapCh->sCloneOpPathHandles[BRAP_OutputChannelPair_eLR][i].hSrcCh->sParams.bCompress == false))
            {    
                uiCount = hRapCh->sRsrcGrnt.sOpResrcId[k].uiPpmId;

                 ui32RegVal1 = BRAP_Read32(hRapCh->hRegister, 
                            BCHP_AUD_DSP_CFG0_PPM_CONFIG_REG_CXT0 + (Index*ui32Offset1));
                 switch(k)
                {
                    case 0:
                        ui32RegVal1 &= ~(BCHP_MASK (AUD_DSP_CFG0_PPM_CONFIG_REG_CXT0,PPM_MODULE0));                
                        ui32RegVal1 |= (BCHP_FIELD_DATA (AUD_DSP_CFG0_PPM_CONFIG_REG_CXT0, 
                                        PPM_MODULE0,  hRapCh->sRsrcGrnt.sOpResrcId[k].uiPpmId));
                        break;
                    case 1:
                        ui32RegVal1 &= ~(BCHP_MASK (AUD_DSP_CFG0_PPM_CONFIG_REG_CXT0,PPM_MODULE1));                
                        ui32RegVal1 |= (BCHP_FIELD_DATA (AUD_DSP_CFG0_PPM_CONFIG_REG_CXT0, 
                                        PPM_MODULE1,  hRapCh->sRsrcGrnt.sOpResrcId[k].uiPpmId));
                        break;                        
                    case 2:
                        ui32RegVal1 &= ~(BCHP_MASK (AUD_DSP_CFG0_PPM_CONFIG_REG_CXT0,PPM_MODULE2));                
                        ui32RegVal1 |= (BCHP_FIELD_DATA (AUD_DSP_CFG0_PPM_CONFIG_REG_CXT0, 
                                        PPM_MODULE2,  hRapCh->sRsrcGrnt.sOpResrcId[k].uiPpmId));
                        break;                        
                    case 3:
                        ui32RegVal1 &= ~(BCHP_MASK (AUD_DSP_CFG0_PPM_CONFIG_REG_CXT0,PPM_MODULE3));                
                        ui32RegVal1 |= (BCHP_FIELD_DATA (AUD_DSP_CFG0_PPM_CONFIG_REG_CXT0, 
                                        PPM_MODULE3,  hRapCh->sRsrcGrnt.sOpResrcId[k].uiPpmId));
                        
                } 

                BRAP_Write32(hRapCh->hRegister, 
                   BCHP_AUD_DSP_CFG0_PPM_CONFIG_REG_CXT0+(Index*ui32Offset1), ui32RegVal1); 

                ui32RegVal = BRAP_Read32(hRapCh->hRegister, 
                BCHP_AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG + (uiCount*ui32Offset));

                ui32RegVal &= ~(BCHP_MASK (AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, ADAPTIVE_SFIFO_SEL));                
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, 
                                ADAPTIVE_SFIFO_SEL, 
                                hRapCh->sCloneOpPathHandles[BRAP_OutputChannelPair_eLR][i].hSrcCh->uiIndex));            

#if (BCHP_CHIP == 7401 && BCHP_VER >= BCHP_VER_C0) || (BCHP_CHIP == 7403) || (BCHP_CHIP == 7400 )
                ui32RegVal &= ~(BCHP_MASK (AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, 
                                TRIWINDOW_WIDTH_SEL));
                /* TODO: hardcoding window width field to 8 ie actual window 
                width of 256. This has to be changed later to be taken from 
                application */
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, 
                                TRIWINDOW_WIDTH_SEL, 8));
#else
                ui32RegVal &= ~(BCHP_MASK (AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, 
                                    TRIWINDOW_WIDTH_SEL));
                /* TODO: hardcoding window width field to 8 ie actual window 
                width of 256. This has to be changed later to be taken from 
                application */
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, 
                                TRIWINDOW_WIDTH_SEL, 8));
#endif

                /* Earlier, for a decode channel FW was setting the enable flag. 
                Now we're doing AdaptRate control for PCM channels also. So let the 
                PI set this flag always */
                ui32RegVal &= ~(BCHP_MASK (AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, ADAPTIVE_RATE_ENABLE));                
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, 
                                ADAPTIVE_RATE_ENABLE, 1));  


                /* Program ADAPTIVE_RATE_THRESHOLD if required */
                ui32RegVal &= ~(BCHP_MASK (AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, ADAPTIVE_RATE_THRESHOLD));                
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, 
                                ADAPTIVE_RATE_THRESHOLD, 5));  


                BDBG_MSG(("For AdaptRateControl: channel pair %d, cloned SrcFifo %d"
                    ", AdaptRateThreshold=5 and WindowWidth=256 ", i, 
                    hRapCh->sCloneOpPathHandles[BRAP_OutputChannelPair_eLR][i].hSrcCh->uiIndex));
                
                BRAP_Write32(hRapCh->hRegister, 
                            BCHP_AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG + (uiCount*ui32Offset), 
                            ui32RegVal);                
            }
    	}
    }  
    
    BDBG_LEAVE (BRAP_P_ProgramAdaptRateCtrl);
    return ret;    
}
#endif

#else /* For 7401,7403 and 7118*/

#if (BRAP_DSPCHN_P_HAS_NEW_TSM_SCHEME==1)
BERR_Code BRAP_P_ProgramAdaptRateCtrl (
		BRAP_ChannelHandle		hRapCh
		)
{

    BERR_Code ret = BERR_SUCCESS;
    uint32_t  ui32RegVal = 0, ui32Offset=0;   
    int i, count=0;

    BDBG_ENTER (BRAP_P_ProgramAdaptRateCtrl);
    BDBG_ASSERT (hRapCh);    

    /* If this channel has a SrcCh which carries PCM data, program the 
    AUD_FMM_BF_CTRL_ADAPTRATE_X accordingly */
    ui32Offset = BCHP_AUD_FMM_BF_CTRL_ADAPTRATE_1_CFG - 
    BCHP_AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG;

    for(i=0; i < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; i++)
    {
    	if (hRapCh->sModuleHandles.hSrcCh[i] != NULL) 
    	{
            /* Make sure the SrcCh the SrcCh is carrying PCM data.
            Note: whether the srcch is carrying PCM or Comp is determined only 
            at start time. So make sure the Srcch has been started.*/
            if ((hRapCh->sModuleHandles.hSrcCh[i]->eState == BRAP_SRCCH_P_State_eRunning) 
                && (hRapCh->sModuleHandles.hSrcCh[i]->sParams.bCompress == false))
            {
                ui32RegVal = BRAP_Read32(hRapCh->hRegister, 
                BCHP_AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG + (count*ui32Offset));

                ui32RegVal &= ~(BCHP_MASK (AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, ADAPTIVE_SFIFO_SEL));                
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, 
                                ADAPTIVE_SFIFO_SEL, 
                                hRapCh->sModuleHandles.hSrcCh[i]->uiIndex));            

#if (BCHP_CHIP == 7401 && BCHP_VER >= BCHP_VER_C0) || (BCHP_CHIP == 7403)
                ui32RegVal &= ~(BCHP_MASK (AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, 
                                TRIWINDOW_WIDTH_SEL));
                /* TODO: hardcoding window width field to 8 ie actual window 
                width of 256. This has to be changed later to be taken from 
                application */
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, 
                                TRIWINDOW_WIDTH_SEL, 8));
#else
                ui32RegVal &= ~(BCHP_MASK (AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, 
                                    TRIWINDOW_WDITH_SEL));
                /* TODO: hardcoding window width field to 8 ie actual window 
                width of 256. This has to be changed later to be taken from 
                application */
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, 
                                TRIWINDOW_WDITH_SEL, 8));
#endif
                /* Earlier, for a decode channel FW was setting the enable flag. 
                Now we're doing AdaptRate control for PCM channels also. So let the 
                PI set this flag always */
                ui32RegVal &= ~(BCHP_MASK (AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, ADAPTIVE_RATE_ENABLE));                
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, 
                                ADAPTIVE_RATE_ENABLE, 1));  


                /* Program ADAPTIVE_RATE_THRESHOLD required */
                ui32RegVal &= ~(BCHP_MASK (AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, ADAPTIVE_RATE_THRESHOLD));                
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, 
                                ADAPTIVE_RATE_THRESHOLD, 5));  



                BDBG_MSG(("For AdaptRateControl: channel pair %d, SrcFifo %d"
                    ", AdaptRateThreshold=5 and WindowWidth = 256 ", i, 
                    hRapCh->sModuleHandles.hSrcCh[i]->uiIndex));
                
                BRAP_Write32(hRapCh->hRegister, 
                            BCHP_AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG + (count*ui32Offset), 
                            ui32RegVal);                
                count ++;
                if (count == 4)
                { 
                    BDBG_ERR(("Count cant exceed 4. There are only 4 SrcCh on 7401!")); 
                    return BERR_TRACE(BERR_INVALID_PARAMETER);
                }
            }
    	}
    }  

    /* also program it for all the cloned srcCh */
    for(i=0; i < BRAP_RM_P_MAX_OUTPUTS; i++)
    {
    	if (hRapCh->sCloneOpPathHandles[BRAP_OutputChannelPair_eLR][i].hSrcCh!= NULL) 
    	{
            /* Make sure the SrcCh the SrcCh is carrying PCM data.
            Note: whether the srcch is carrying PCM or Comp is determined only 
            at start time. So make sure the Srcch has been started.*/
            if ((hRapCh->sCloneOpPathHandles[BRAP_OutputChannelPair_eLR][i].hSrcCh->eState == BRAP_SRCCH_P_State_eRunning) 
                && (hRapCh->sCloneOpPathHandles[BRAP_OutputChannelPair_eLR][i].hSrcCh->sParams.bCompress == false))
            {
                ui32RegVal = BRAP_Read32(hRapCh->hRegister, 
                BCHP_AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG + (count*ui32Offset));

                ui32RegVal &= ~(BCHP_MASK (AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, ADAPTIVE_SFIFO_SEL));                
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, 
                                ADAPTIVE_SFIFO_SEL, 
                                hRapCh->sCloneOpPathHandles[BRAP_OutputChannelPair_eLR][i].hSrcCh->uiIndex));            

#if (BCHP_CHIP == 7401 && BCHP_VER >= BCHP_VER_C0) || (BCHP_CHIP == 7403)
                ui32RegVal &= ~(BCHP_MASK (AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, 
                                TRIWINDOW_WIDTH_SEL));
                /* TODO: hardcoding window width field to 8 ie actual window 
                width of 256. This has to be changed later to be taken from 
                application */
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, 
                                TRIWINDOW_WIDTH_SEL, 8));
#else
                ui32RegVal &= ~(BCHP_MASK (AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, 
                                    TRIWINDOW_WDITH_SEL));
                /* TODO: hardcoding window width field to 8 ie actual window 
                width of 256. This has to be changed later to be taken from 
                application */
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, 
                                TRIWINDOW_WDITH_SEL, 8));
#endif

                /* Earlier, for a decode channel FW was setting the enable flag. 
                Now we're doing AdaptRate control for PCM channels also. So let the 
                PI set this flag always */
                ui32RegVal &= ~(BCHP_MASK (AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, ADAPTIVE_RATE_ENABLE));                
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, 
                                ADAPTIVE_RATE_ENABLE, 1));  


                /* Program ADAPTIVE_RATE_THRESHOLD if required */
                ui32RegVal &= ~(BCHP_MASK (AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, ADAPTIVE_RATE_THRESHOLD));                
                ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, 
                                ADAPTIVE_RATE_THRESHOLD, 5));  


                BDBG_MSG(("For AdaptRateControl: channel pair %d, cloned SrcFifo %d"
                    ", AdaptRateThreshold=5 and WindowWidth=256 ", i, 
                    hRapCh->sCloneOpPathHandles[BRAP_OutputChannelPair_eLR][i].hSrcCh->uiIndex));
                
                BRAP_Write32(hRapCh->hRegister, 
                            BCHP_AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG + (count*ui32Offset), 
                            ui32RegVal);                
            }
    	}
    }  
    
    BDBG_LEAVE (BRAP_P_ProgramAdaptRateCtrl);
    return ret;    
}
#endif

#endif

#if BCHP_7411_VER > BCHP_VER_C0
#define BRAP_P_I2S2_CFG_OFFSET	(0x8)
BERR_Code BRAP_P_I2sAlignmentWorkaround (
		BRAP_ChannelHandle		hRapCh
		)
{
	unsigned int regVal = 0;
	
	/* For I2S alignment to work, all three I2S ports should be enabled and drawing data. For
	 * 7411D multichannel mode, we get two I2S ports from application. Start third I2S port 
	 * internally. We need to start corresponding mixer also.
	 * The steps are
	 * Enable I2S2 port
	 * Enable Mixer 1 of DP1
	 * Share source channel corresponding to I2S1 with I2S2
	 */
	 BDBG_WRN(("For I2S alignment to work, all three I2S ports needs to be enabled"
	 			" Enabling I2S 2 internally."));

	/* Enable I2S 2 port */
	regVal = BRAP_Read32(hRapCh->hRegister, BCHP_AUD_FMM_OP_CTRL_I2S_CFG_i_ARRAY_BASE + BRAP_P_I2S2_CFG_OFFSET);
	
	regVal = (regVal & ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2S_CFG_i, CLOCK_ENABLE)))
			| BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_I2S_CFG_i, CLOCK_ENABLE, Enable);

	regVal = (regVal & ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2S_CFG_i, DATA_ENABLE)))
			| BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_I2S_CFG_i, DATA_ENABLE, Enable);

	regVal = (regVal & ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2S_CFG_i, MCLK_RATE)))
			| BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_I2S_CFG_i, MCLK_RATE, MCLK_128fs_SCLK_64fs);

	regVal = (regVal & ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2S_CFG_i, BITS_PER_SAMPLE)))
			| BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_I2S_CFG_i, BITS_PER_SAMPLE, Bitwidth24);
	
	regVal = (regVal & ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2S_CFG_i, BITS_PER_SAMPLE)))
			| BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_I2S_CFG_i, BITS_PER_SAMPLE, Bitwidth24);

	BDBG_ERR(("BCHP_AUD_FMM_OP_CTRL_I2S_CFG_i_ARRAY_BASE = 0x%08x", regVal));
	BRAP_Write32(hRapCh->hRegister, BCHP_AUD_FMM_OP_CTRL_I2S_CFG_i_ARRAY_BASE + BRAP_P_I2S2_CFG_OFFSET, regVal);

	BRAP_Write32(hRapCh->hRegister, BCHP_AUD_FMM_OP_CTRL_ENABLE_SET, 0x10);
	
	/* Enable Mixer1 of DP1 */
	regVal = BRAP_Read32(hRapCh->hRegister, BCHP_AUD_FMM_DP_CTRL1_MS_CLIENT1_MIX1_CONFIG);

	regVal = (regVal & ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MS_CLIENT0_MIX1_CONFIG, MIX_PB_ID)))
			| BCHP_FIELD_DATA(AUD_FMM_DP_CTRL0_MS_CLIENT0_MIX1_CONFIG, MIX_PB_ID, 0x0);

	regVal = (regVal & ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MS_CLIENT0_MIX1_CONFIG, MIX_ENABLE)))
			| BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MS_CLIENT0_MIX1_CONFIG, MIX_ENABLE, Enable);

	BDBG_ERR(("BCHP_AUD_FMM_DP_CTRL1_MS_CLIENT1_MIX1_CONFIG = 0x%08x", regVal));
	BRAP_Write32(hRapCh->hRegister, BCHP_AUD_FMM_DP_CTRL1_MS_CLIENT1_MIX1_CONFIG, regVal);

	regVal = BRAP_Read32(hRapCh->hRegister, BCHP_AUD_FMM_DP_CTRL1_STRM_FORMAT);

	regVal = (regVal & ~(BCHP_MASK(AUD_FMM_DP_CTRL0_STRM_FORMAT, STREAM0_BIT_RESOLUTION)))
			| BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_STRM_FORMAT, STREAM0_BIT_RESOLUTION, RES_24);

	BRAP_Write32(hRapCh->hRegister, BCHP_AUD_FMM_DP_CTRL1_STRM_FORMAT, regVal);

	regVal = BRAP_Read32(hRapCh->hRegister, BCHP_AUD_FMM_DP_CTRL1_STRM_ENA);

	regVal = (regVal & ~(BCHP_MASK(AUD_FMM_DP_CTRL0_STRM_ENA, STREAM1_ENA)))
			| BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_STRM_ENA, STREAM1_ENA, Enable);

	BDBG_ERR(("BCHP_AUD_FMM_DP_CTRL1_STRM_ENA = 0x%08x", regVal));
	BRAP_Write32(hRapCh->hRegister, BCHP_AUD_FMM_DP_CTRL1_STRM_ENA, regVal);

	return BERR_SUCCESS;
}

BERR_Code
BRAP_P_I2sAlignmentWorkaroundStop	( BRAP_ChannelHandle		hRapCh
		)
{
	unsigned int regVal = 0;

	regVal = BRAP_Read32(hRapCh->hRegister, BCHP_AUD_FMM_DP_CTRL1_STRM_ENA);

	regVal = (regVal & ~(BCHP_MASK(AUD_FMM_DP_CTRL0_STRM_ENA, STREAM1_ENA)))
			| BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_STRM_ENA, STREAM1_ENA, Disable);

	BDBG_ERR(("BCHP_AUD_FMM_DP_CTRL1_STRM_ENA = 0x%08x", regVal));
	BRAP_Write32(hRapCh->hRegister, BCHP_AUD_FMM_DP_CTRL1_STRM_ENA, regVal);

	regVal = BRAP_Read32(hRapCh->hRegister, BCHP_AUD_FMM_DP_CTRL1_MS_CLIENT1_MIX1_CONFIG);

	regVal = (regVal & ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MS_CLIENT0_MIX1_CONFIG, MIX_PB_ID)))
			| BCHP_FIELD_DATA(AUD_FMM_DP_CTRL0_MS_CLIENT0_MIX1_CONFIG, MIX_PB_ID, 0xF);
#if 0 
	regVal = (regVal & ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MS_CLIENT0_MIX1_CONFIG, MIX_ENABLE)))
			| BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MS_CLIENT0_MIX1_CONFIG, MIX_ENABLE, Disable);
#endif

	BDBG_ERR(("BCHP_AUD_FMM_DP_CTRL1_MS_CLIENT1_MIX1_CONFIG = 0x%08x", regVal));
	BRAP_Write32(hRapCh->hRegister, BCHP_AUD_FMM_DP_CTRL1_MS_CLIENT1_MIX1_CONFIG, regVal);
#if 0
	regVal = BRAP_Read32(hRapCh->hRegister, BCHP_AUD_FMM_OP_CTRL_I2S_CFG_i_ARRAY_BASE + BRAP_P_I2S2_CFG_OFFSET);
	
	regVal = (regVal & ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2S_CFG_i, CLOCK_ENABLE)))
			| BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_I2S_CFG_i, CLOCK_ENABLE, Enable);

	regVal = (regVal & ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2S_CFG_i, DATA_ENABLE)))
			| BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_I2S_CFG_i, DATA_ENABLE, Disable);

	regVal = (regVal & ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2S_CFG_i, MCLK_RATE)))
			| BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_I2S_CFG_i, MCLK_RATE, MCLK_128fs_SCLK_64fs);

	regVal = (regVal & ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2S_CFG_i, BITS_PER_SAMPLE)))
			| BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_I2S_CFG_i, BITS_PER_SAMPLE, Bitwidth24);
	
	regVal = (regVal & ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2S_CFG_i, BITS_PER_SAMPLE)))
			| BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_I2S_CFG_i, BITS_PER_SAMPLE, Bitwidth24);

	BDBG_ERR(("BCHP_AUD_FMM_OP_CTRL_I2S_CFG_i_ARRAY_BASE = 0x%08x", regVal));
	BRAP_Write32(hRapCh->hRegister, BCHP_AUD_FMM_OP_CTRL_I2S_CFG_i_ARRAY_BASE + BRAP_P_I2S2_CFG_OFFSET, regVal);


	regVal = BRAP_Read32(hRapCh->hRegister, BCHP_AUD_FMM_OP_CTRL_I2S_CFG_i_ARRAY_BASE + BRAP_P_I2S2_CFG_OFFSET);

	regVal = (regVal & ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2S_CFG_i, DATA_ENABLE)))
	| BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_I2S_CFG_i, DATA_ENABLE, Disable);


	BDBG_ERR(("BCHP_AUD_FMM_OP_CTRL_I2S_CFG_i_ARRAY_BASE = 0x%08x", regVal));
	BRAP_Write32(hRapCh->hRegister, BCHP_AUD_FMM_OP_CTRL_I2S_CFG_i_ARRAY_BASE + BRAP_P_I2S2_CFG_OFFSET, regVal);


#endif


	regVal = 0x80010; /* To disable the Alignment at stop */
	BRAP_Write32(hRapCh->hRegister, BCHP_AUD_FMM_OP_CTRL_ENABLE_CLEAR, regVal );


	return BERR_SUCCESS;

}

BRAP_AudioMemAllocation BRAP_P_GetMemoryAllocationType (
							BRAP_Handle			hRap		/* [in] RAP Handle */
							)
{
	return hRap->sSettings.eMemAllocation;
}
#endif

#if ( BCHP_CHIP == 7400 )
/**************************************************************************
Summary:
    Private function that Resets the CHP_AUD_FMM_BF_CTRL_REARM_FREEFULL_MARK
    bit for the SrcCh which is unmasked to recieve the interrupt.
    Resetting this bit is a part of hardware workaround. For resetting the 
    bit, First set it to 0, then set the bit to 1 again
**************************************************************************/

void BRAP_P_ReArmSrcChInterrupt(
	BRAP_ChannelHandle	hRapch
	)
{
	BDBG_ENTER(BRAP_P_ReArmSrcChInterrupt);
	BDBG_ASSERT(hRapch);

    BKNI_EnterCriticalSection();
    BRAP_P_ReArmSrcChInterrupt_isr(hRapch);
    BKNI_LeaveCriticalSection();

    BDBG_LEAVE(BRAP_P_ReArmSrcChInterrupt);
    return;
}

/**************************************************************************
Summary:
	Isr version of BRAP_P_ReArmSrcChInterrupt.
**************************************************************************/

void BRAP_P_ReArmSrcChInterrupt_isr(
	BRAP_ChannelHandle	hRapch
	)
{
	uint32_t 	ui32RegVal =0x0,ui32ReArmBit=0x0;
	
	BDBG_ENTER(BRAP_P_ReArmSrcChInterrupt_isr);
	BDBG_ASSERT(hRapch);
	
	ui32ReArmBit |= (1 << hRapch->sRsrcGrnt.sOpResrcId[0].uiSrcChId);

	BDBG_MSG(("ui32ReArmBit = %d",ui32ReArmBit));
	/* First set 0 to Re-Arm bit of source channel, then set it to 1 */
	
	ui32RegVal = BRAP_Read32_isr(hRapch->hRegister,BCHP_AUD_FMM_BF_CTRL_REARM_FREEFULL_MARK );
	ui32RegVal = (~ui32ReArmBit) & ui32RegVal;
	BRAP_Write32_isr (hRapch->hRegister, BCHP_AUD_FMM_BF_CTRL_REARM_FREEFULL_MARK,ui32RegVal);
	
	ui32RegVal = BRAP_Read32_isr(hRapch->hRegister,BCHP_AUD_FMM_BF_CTRL_REARM_FREEFULL_MARK );
	ui32RegVal |= BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_REARM_FREEFULL_MARK, 
                        REARM_FREEMARK, ui32ReArmBit);
	BRAP_Write32_isr (hRapch->hRegister,  
              BCHP_AUD_FMM_BF_CTRL_REARM_FREEFULL_MARK,ui32RegVal);
	
	BDBG_LEAVE(BRAP_P_ReArmSrcChInterrupt_isr);
	return;
}
#endif

/* End Of File */

