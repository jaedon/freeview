/***************************************************************************
*     Copyright (c) 2004-2011, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brap_rm.c $
* $brcm_Revision: Hydra_Software_Devel/23 $
* $brcm_Date: 3/18/11 9:43p $
*
* Module Description:
*	This file contains the implementations of the APIs, which are used by
*	Raptor Audio PI. 
*
* For various audio operations, Raptor audio PI makes use of following hardware 
* /firmware/memory resources:
* 1. DSP contexts (Firmware resource)
* 2. DRAM ring buffers (Memory resource)
* 3. Source/Destination channels (FMM hardware resource)
* 4. SRCs (FMM hardware resource)
* 5. Mixers (FMM hardware resource)
* 6. SPDIF-Formatter (FMM hardware resource)
* 7. Output ports (FMM hardware resource)
* 8. Input/capture rate control ports (FMM hardware resource)
*  
* Above resources are shared for following audio operations:
* 1. Live Decode/Pass Thru/SRC/Simulmode in DSP    //TODO: Is simul mode reqd?? 
* 2. PCM Playback from hard-disk/memory
* 3. PCM Capture
* 4. Encode 
*  
* Resource Manager maintains above resources and provides them to Audio Manager 
* as per the request. It allocates resources, for optimum usage. However, RM
* doesn't know what all resources are getting used for a particular instance of 
* audio operation. It is the responsibility of Audio Manager to provide a list 
* of all the resources required. It is also Audio Manager's responsibility to 
* provide a list of all the resources to be freed once an instance of audio 
* operation is closed.
*  
* Other modules interact with Resource Manager for following purposes.
* 1. When a new audio channel (instance of audio operation) is formed (or 
*    closed). Audio Manager requests Resource Manager to allocate (or free) the 
*    required resources for this operation.
* 2. Adding/Removing of the output ports.
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/rap/base3/brap_rm.c $
* 
* Hydra_Software_Devel/23   3/18/11 9:43p sgadara
* SW7420-767: [7420] Merge MS11 development branch to mainline
* 
* Hydra_Software_Devel/22   4/15/10 9:46p sgadara
* SW3548-2852: [3548,3556] Providing support to add same Equalizer to
* multiple output ports. --> Currently the multiple output ports should
* belong to same channel.
* 
* Hydra_Software_Devel/21   3/26/10 10:57a sgadara
* SW3548-2859: [3548,3556] Adding Subwoofer(Low Pass Filter) to Equalizer
* settings. --> Started using SRC1 block
* 
* Hydra_Software_Devel/20   10/16/09 4:07p gautamk
* SW7405-3063: [7340] Adding support for Internal capture
* 
* Hydra_Software_Devel/19   10/2/09 2:37a srajapur
* SW7550-52: [7550] Merging 7550 branch code to main line
* 
* Hydra_Software_Devel/18   9/29/09 12:37p gautamk
* SW7420-361: [7405] Adding support for DAC1
* 
* Hydra_Software_Devel/17   9/28/09 12:47p gautamk
* SW7420-361: [7405] Enabling DAC1 for 7420 except A0 version
* 
* Hydra_Software_Devel/16   8/12/09 10:25a gautamk
* PR55224:[7405] Fixing compile error.
* 
* Hydra_Software_Devel/15   8/12/09 9:59a gautamk
* PR55224:[7405] fixing error due to merge
* 
* Hydra_Software_Devel/14   8/12/09 9:51a gautamk
* PR55224: [7405] using 7420_Family
* 
* Hydra_Software_Devel/13   8/12/09 9:44a gautamk
* PR55224: [7405] using 7420_Family
* 
* Hydra_Software_Devel/12   7/23/09 4:50p bhanus
* PR 55499 : [7340,7342,7125]Adding 7420 Family support
* 
* Hydra_Software_Devel/11   6/18/09 2:20p gautamk
* PR56112:[7420] Adding PI support for HDMI 5.1PCm for 7420
* 
* Hydra_Software_Devel/10   6/12/09 3:38p gautamk
* PR53302: [7405] Merging cleanup code to mainline
* 
* Hydra_Software_Devel/9   2/13/09 5:00p gautamk
* PR51897: [7405] using SHaredPP Path type incase of Playback and capture
* channel for 7405
* 
* Hydra_Software_Devel/8   1/27/09 3:32p speter
* PR 50495: [3548,3556] Merging the customer branch to main line
* 
* Hydra_Software_Devel/7   1/13/09 9:17p bhanus
* PR 47757: [7420] Added warning - no audio due to hardware bug - when
* DAC1 is added on 7420
* 
* Hydra_Software_Devel/6   12/10/08 10:59p gautamk
* PR49232:[7405] Adding support for Allocating Rbuf/SrcCh and Paths at
* OpenChannel time.
* 
* Hydra_Software_Devel/RAP_354856_PH_4_1/1   1/13/09 5:16p sgadara
* PR 50859: [3548,3556] Adding the SRCBLKId check before freeing the SRCs
* 
* Hydra_Software_Devel/5   10/10/08 10:17a speter
* PR 47764: [3548,3556] Merging the release branch 3.0 to main line of
* development
* 
* Hydra_Software_Devel/4   9/30/08 11:57p gautamk
* PR46702: [7405] fixing warning.
* 
* Hydra_Software_Devel/RAP_354856_PH_3_0/2   10/7/08 1:26p speter
* PR 45444: [3548,3556] Adding code for equalizer
* 
* Hydra_Software_Devel/RAP_354856_PH_3_0/1   10/6/08 7:41p speter
* PR 45444: [3548,3556]: Adding generic code for equalizer
* 
* Hydra_Software_Devel/3   8/20/08 6:12p gautamk
* PR43454: [7405] Changes for Concurrent downmix.
* 
* Hydra_Software_Devel/2   7/28/08 9:44a speter
* PR 45171: [3548,3556] Merging from Phase 2.0 release to main line of
* development (Hydra_Software_Devel)
* 
* Hydra_Software_Devel/RAP_354856_PH_2_0/1   7/25/08 11:35a speter
* PR 45032: [3548,3556] Adding SRC and Internal Capture port to the
* capture path of ADC to enable downsampling ADC so that DRAM and DSP
* can handle the ADC input data
* 
* Hydra_Software_Devel/1   7/21/08 2:20p speter
* PR 44984: [3548,3556,7405] Adding files from base2 which have new
* Architecture specific changes and cannot be merged back in base2
* 
* Hydra_Software_Devel/Rap_NewFwArch/11   7/16/08 10:08p gautamk
* PR43454: [7405] Implementing delta code review.
* 
* Hydra_Software_Devel/Rap_NewFwArch/10   7/10/08 6:19p gautamk
* PR41825: [7405] merging the code for multiple associatio from Branch to
* mainline.
* 
* Hydra_Software_Devel/Rap_NewFwArch/9   7/9/08 3:23p sushmit
* PR 40494: [7325] Moving KLOCWORK fix to NEWFWARCH
* 
* Hydra_Software_Devel/Rap_NewFwArch/8   7/4/08 4:31p gautamk
* PR43454: [7405] Implementation for Adaptive rate ctrl (2ms lipsync)
* 
* Hydra_Software_Devel/Rap_NewFwArch/7   6/10/08 9:38p speter
* PR 38950: [3548,3556] Fixing the resource manager for decode compress
* path.
* 
* Hydra_Software_Devel/Rap_NewFwArch/6   5/29/08 10:05p gautamk
* PR41726: [7405] doing Memset after Malloc
* 
* Hydra_Software_Devel/Rap_NewFwArch/5   5/14/08 4:31p speter
* PR 38950: [3548,3556] Adding code to support HIFIDAC1 and HIFIDAC2
* 
* Hydra_Software_Devel/Rap_NewFwArch/4   5/5/08 11:34a speter
* PR 38950: [3548] Making 3548_FAMILY macro and using it for both 3548
* and 3556 cases.
* 
* Hydra_Software_Devel/Rap_NewFwArch/3   4/28/08 4:37p speter
* PR 38950: [3548] Fixing compilation for 3548/3556.
* 
* Hydra_Software_Devel/Rap_NewFwArch/2   4/22/08 2:17p gautamk
* PR41994: [7405] Using BCHP_VER_ Correctly...
* 
* Hydra_Software_Devel/Rap_NewFwArch/1   4/1/08 2:48p sushmit
* PR 40820: [7405] Bringing up DD Convert
* 
* Hydra_Software_Devel/50   3/11/08 12:33p speter
* PR 36401: [3563] Fixing the problem due to ENC_DSP_ID being 0. All
* occurrences of ENC_DSP_ID should be safeguarded by MAX_DSPS > 1 marcro
* 
* Hydra_Software_Devel/49   11/19/07 3:17p sushmit
* PR 36822: [7405] Making MAI take data straight from Mixer.
* 
* Hydra_Software_Devel/48   10/25/07 1:39a gautamk
* PR36409: [3563] Fixing Coverity issue for 3563 RAP Code base
* 
* Hydra_Software_Devel/47   9/30/07 1:58p sushmit
* PR35140: [7405] Develop 7325 Raptor PI for Bringup
* 
* Hydra_Software_Devel/46   9/19/07 6:15p gdata
* PR35073: [7440] Fixing SRC cycle limitation for worst usage cases
* - PCM Playback DecPcm path gets SRC from SRC block1
* - this is in accordance with the 7440B0 usage doc
* 
* Hydra_Software_Devel/45   9/19/07 10:11a sushmit
* PR 34648: [7405] Fixing Compilation warnings.
* 
* Hydra_Software_Devel/44   8/20/07 5:19p sushmit
* PR 34014: [7405] Mallocing BRAP_RM_P_SrcMixerGrant
* 
* Hydra_Software_Devel/43   7/31/07 2:54p sushmit
* PR 27646: [7405] Allocating DP_BLK for 7405 Passthru case.
* 
* Hydra_Software_Devel/42   7/13/07 1:47p bhanus
* PR 32986: [7440 B0] Code modifed for Sequential path bring up.
* 
* Hydra_Software_Devel/41   6/26/07 11:41a bhanus
* PR 32504 :[7440 BO] Code modified for 7.1 Downmix support.
* 
* Hydra_Software_Devel/40   6/26/07 11:16a gautamk
* PR30069: Adding support for having SPDIF0 +SPDIF1 enabled together for
* Simul and XCD mode.
* 
* Hydra_Software_Devel/39   6/22/07 2:49p bhanus
* PR 30069 : [7440 B] Fixed SRC allocation and SRC SRAM allocation issue
* (HiDef DVD PR 4508)
* 
* Hydra_Software_Devel/38   6/18/07 1:30p kagrawal
* PR 30069: Fixed mixer input usage count corruption
* 
* Hydra_Software_Devel/37   6/5/07 11:01a speter
* PR 31827: [3563] Merging from Phase 2.0 release branch to main
* 
* Hydra_Software_Devel/36   6/4/07 2:07p bhanus
* PR 30069 :[7440 B0] Added Support for SPDIF1 and I2SMulti0 as Mux
* Select for HDMI
* 
* Hydra_Software_Devel/35   5/28/07 1:34p bhanus
* PR 30069 : Fixed Coverity Issues.
* 
* Hydra_Software_Devel/RAP_3563_PH_2_0/1   5/29/07 1:46p sushmit
* PR28719: Adding code for independent delay & fixing MULPP broken
* 
* Hydra_Software_Devel/34   5/24/07 6:20p kagrawal
* PR 30069: [7440 B0] Added support for stereo output on multi-I2s1
* 
* Hydra_Software_Devel/33   5/24/07 2:23p kagrawal
* PR 30069: [7440B0] Added support for SPDIF1 - Corrected the SpdifFmId
* 
* Hydra_Software_Devel/32   5/11/07 8:59p sushmit
* PR 27646: Getting 7405 PI compiled
* 
* Hydra_Software_Devel/31   5/11/07 12:29p kagrawal
* PR 30069: Added code for SPDIF1
* 
* Hydra_Software_Devel/30   5/11/07 10:31a kagrawal
* PR 30069: Added FS Timing Source as a resource in RM
* 
* Hydra_Software_Devel/29   5/9/07 12:10p bhanus
* PR 30069 : [7440 B0] Code modified for :
* - Unified SR Support
* - Copy Path to enable Mixing on Sequential Path
* 
* Hydra_Software_Devel/28   5/3/07 2:26p kagrawal
* PR 30069: Fixing compilation error due to the last checkin
* 
* Hydra_Software_Devel/27   5/3/07 1:47p bhanus
* PR 30069 : Moved the Delay Balance path related stuff under Sequential
* path define to avoid compilation breaks for 3563.
* 
* Hydra_Software_Devel/26   5/2/07 8:48p bhanus
* PR 30069 : Updated code for allocating Mixer and SRC's from DP Block 1
* and SRC Block 1 for Sequential Paths
* 
* Hydra_Software_Devel/25   4/26/07 7:27p bhanus
* PR 30069 : Code Modified for sequential path bring up
* 
* Hydra_Software_Devel/24   4/18/07 10:32a kagrawal
* PR 28561: Proper RM for CapPortAllocation
* 
* Hydra_Software_Devel/23   3/2/07 11:22a kagrawal
* PR 27912: Added PI support for Speaker Management
* - Allocate DSP1 and context 0 for SM and XCD only for 7440 A0
* 
* Hydra_Software_Devel/22   2/28/07 11:57a kagrawal
* PR 27912: Fixed mixer input allocation for multi-channel PB
* 
* Hydra_Software_Devel/21   2/16/07 12:38p bhanus
* PR 27912 : Modified for  Static Downmixing Support.
* 
* Hydra_Software_Devel/20   2/9/07 6:22p bhanus
* PR 25607 : Fixed Compilation warnings
* 
* Hydra_Software_Devel/19   2/6/07 1:16p kagrawal
* PR 25607: Misc fixes for supporting dual decode
* 
* Hydra_Software_Devel/18   2/5/07 1:12p bhanus
* PR 25607: Added support for cloning of output ports
* 
* Hydra_Software_Devel/17   1/22/07 7:27p sushmit
* PR 26864: Addition of DSP to capture channel
* 
* Hydra_Software_Devel/16   1/19/07 12:09p kagrawal
* PR 21368: [7440A0] Changing mixer input preference to secondary (0-2),
* PB (3-6) and primary (7) - Required workaround for "DP can misalign
* multichannel group"
* 
* Hydra_Software_Devel/15   1/4/07 2:05p bhanus
* PR 25607: Modified for Multichannel mixing
* 
* Hydra_Software_Devel/14   1/4/07 11:25a bhanus
* PR 25607: Added Support for Multi Channel Mixing.
* - Re- designed BRAP_P_MixerSrcRsrcLayout()
* 
* Hydra_Software_Devel/13   12/27/06 2:20p bhanus
* PR 25607 : Modified code for Full Duplex Capture.
* 
* Hydra_Software_Devel/12   12/15/06 2:20p kagrawal
* PR 25607: Added proper support for HDMI output port
* 
* Hydra_Software_Devel/11   12/11/06 3:51p sushmit
* PR 25607: Fixing DSTCH Offsets for 3563
* 
* Hydra_Software_Devel/10   11/29/06 11:13a bhanus
* PR 25607 : Modified for SRC bringup
* 
* Hydra_Software_Devel/9   11/27/06 12:13p sushmit
* PR 25607: Added code for CAP, XCD & ChSubTypeNone
* 
* Hydra_Software_Devel/8   11/17/06 5:17p bhanus
* PR 25607 : Code modified for Mixing bringup
* 
* Hydra_Software_Devel/7   11/16/06 12:23p sushmit
* PR 25607: Encode/Transcode Additions.
* 
* Hydra_Software_Devel/6   11/10/06 11:15a kagrawal
* PR 25607: Added support for multi channel output on I2s_multi.
* Corrected multi-channel SRC allocation.
* 
* Hydra_Software_Devel/5   10/6/06 5:47p kagrawal
* PR 24717: Merged 7440 devel branch to Hydra_Software_Devel
* 
* Hydra_Software_Devel/4   9/20/06 2:16p kagrawal
* PR 20654: Fixed compilation warnings
* 
* Hydra_Software_Devel/3   9/19/06 6:01p kagrawal
* PR 20654: Added channel change support - fixed bug in FreeResources()
* 
* Hydra_Software_Devel/2   9/11/06 2:59p kagrawal
* PR 20654: Removed redundant debug msg
* 
* Hydra_Software_Devel/1   8/17/06 12:07p speter
* PR 20654: Added from BLR_RAP_7440A0_Bring_Up_Branch
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/15   8/17/06 11:34a kagrawal
* PR 20654: Fixed errors found during emulation
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/14   8/10/06 4:08p kagrawal
* PR 20654: Fixed linking error
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/13   8/8/06 4:37p sushmit
* PR18604: Updating Allocate & Free API for Encoder PI for 7440.
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/12   8/8/06 12:06p kagrawal
* PR 20654: Implemented review comments
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/11   8/8/06 11:26a bhanus
* PR 20654 : Implemented review comments.
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/10   8/3/06 6:47p kagrawal
* PR 20654: Fixed compilation error
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/9   8/2/06 9:15p kagrawal
* PR 20654: Fixed compilation error
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/8   8/1/06 3:03p kagrawal
* PR 20654: Corrected resource request and grant structure for output
* port
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/7   7/26/06 10:08p kagrawal
* PR 20654: Using macro for DSP used for decode
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/6   7/18/06 3:14p bhanus
* PR 20654: Added Error conditionin BRAP_RM_P_UpdateResrcGrant
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/5   7/18/06 2:54p bhanus
* PR 20654: Added API to initialize the Resource request structure and
* Update Resource grant structure.
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/4   7/14/06 7:14p bhanus
* PR 20654: Added code for resource allocation of SRC and Mixer.
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/3   7/13/06 8:16p bhanus
* PR 20654: Added code for resource allocation of SRC and Mixer.
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/2   7/12/06 6:30p kagrawal
* PR 20654: Added Free() along with various sub alloc functions
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/1   7/11/06 6:41p kagrawal
* PR 20654: Initial version
***************************************************************************/
#include "brap.h"
#include "brap_priv.h"

BDBG_MODULE(rap_rm);	/* Register software module with debug interface */

/*{{{ Local Defines */
#define BRAP_RM_P_SIM_MODE_PT_DSP_CONTEXT 2 /* DSP Context for the Pass-thru 
											   operation of Simultaneous mode */

#define BRAP_RM_P_MAX_PRIMARY_INPUT_PER_MIXER   1
#define BRAP_RM_P_MAX_SECONDARY_INPUT_PER_MIXER 3
#if  (BRAP_3548_FAMILY == 1) || (BRAP_7405_FAMILY == 1)
#define BRAP_RM_P_MAX_PB_INPUT_PER_MIXER        8
#define BRAP_RM_P_DST_CH_OFFSET                 BRAP_RM_P_MAX_SRC_CHANNELS
#endif
/*}}}*/

/*{{{ Local Typedefs */

/*}}}*/

/*{{{ Static Variables & Function prototypes */
static BERR_Code BRAP_RM_P_AllocateDsp(
	BRAP_RM_Handle				hRm,		/* [in] Resource Manager handle */
	const BRAP_RM_P_ResrcReq	*pResrcReq,	/* [in] Required resources */
	BRAP_RM_P_ResrcGrant		*pResrcGrant/* [out] Indices of the allocated 
	                                           resources */
    );
static BERR_Code BRAP_RM_P_AllocateRBuf(
	BRAP_RM_Handle				hRm,		/* [in] Resource Manager handle */
	const BRAP_RM_P_ResrcReq	*pResrcReq,	/* [in] Required resources */
	BRAP_RM_P_ResrcGrant		*pResrcGrant/* [out] Indices of the allocated 
	                                           resources */
    );
static BERR_Code BRAP_RM_P_AllocateSrcCh(
	BRAP_RM_Handle				hRm,		/* [in] Resource Manager handle */
	const BRAP_RM_P_ResrcReq	*pResrcReq,	/* [in] Required resources */
	BRAP_RM_P_ResrcGrant		*pResrcGrant/* [out] Indices of the allocated 
	                                           resources */
    );
static BERR_Code BRAP_RM_P_AllocateDstCh(
	BRAP_RM_Handle				hRm,		/* [in] Resource Manager handle */
	const BRAP_RM_P_ResrcReq	*pResrcReq,	/* [in] Required resources */
	BRAP_RM_P_ResrcGrant		*pResrcGrant/* [out] Indices of the allocated 
	                                           resources */
    );
static BERR_Code BRAP_RM_P_AllocateAdaptRateCtrl(
	BRAP_RM_Handle				hRm,		/* [in] Resource Manager handle */
	const BRAP_RM_P_ResrcReq	*pResrcReq,	/* [in] Required resources */
	BRAP_RM_P_ResrcGrant		*pResrcGrant/* [out] Indices of the allocated 
	                                           resources */
    );
static BERR_Code BRAP_RM_P_AllocateSrc(
	BRAP_RM_Handle				hRm,		/* [in] Resource Manager handle */
	const BRAP_RM_P_ResrcReq	*pResrcReq,	/* [in] Required resources */
	BRAP_RM_P_ResrcGrant		*pResrcGrant/* [out] Indices of the allocated 
	                                           resources */
    );
static BERR_Code BRAP_RM_P_AllocateMixer(
	BRAP_RM_Handle				hRm,		/* [in] Resource Manager handle */
	const BRAP_RM_P_ResrcReq	*pResrcReq,	/* [in] Required resources */
	BRAP_RM_P_ResrcGrant		*pResrcGrant/* [out] Indices of the allocated 
	                                           resources */
    );
static BERR_Code BRAP_RM_P_AllocateSrcEq(
	BRAP_RM_Handle				hRm,		/* [in] Resource Manager handle */
	const BRAP_RM_P_ResrcReq	*pResrcReq,	/* [in] Required resources */
	BRAP_RM_P_ResrcGrant		*pResrcGrant/* [out] Indices of the allocated 
	                                           resources */
    );
static BERR_Code BRAP_RM_P_AllocateOpPort(
	BRAP_RM_Handle				hRm,		/* [in] Resource Manager handle */
	const BRAP_RM_P_ResrcReq	*pResrcReq,	/* [in] Required resources */
	BRAP_RM_P_ResrcGrant		*pResrcGrant/* [out] Indices of the allocated 
	                                           resources */
    );
#if (BRAP_7550_FAMILY != 1)
static BERR_Code BRAP_RM_P_AllocateCapPort(
	BRAP_RM_Handle				hRm,		/* [in] Resource Manager handle */
	const BRAP_RM_P_ResrcReq	*pResrcReq,	/* [in] Required resources */
	BRAP_RM_P_ResrcGrant		*pResrcGrant/* [out] Indices of the allocated 
	                                           resources */
    );
#endif

static BERR_Code BRAP_RM_P_AllocateFsTmgSource(
	BRAP_RM_Handle				hRm,		/* [in] Resource Manager handle */
	const BRAP_RM_P_FsTmgSrcReq *pFsTmgReq,	/* [in] Required resources */
	BRAP_RM_P_FsTmgSrcGrant     *pFsTmgGrant/* [out] Indices of the allocated 
	                                           resources */
    );
#ifdef BRAP_DBG_FMM
static void
BRAP_RM_P_DumpMixerUsage(
    BRAP_RM_Handle              hRm
    );    
#endif

#if (BRAP_3548_FAMILY == 1) || (BRAP_7405_FAMILY == 1)

#if (BRAP_7420_FAMILY == 1)
static const unsigned int BRAP_RM_P_SrcChPref[BRAP_RM_P_MAX_SRC_CHANNELS] = 
{
    0,  1,  2,  3,  4,  5,  6,  7, 8, 9 
};

static const unsigned int BRAP_RM_P_AdaptRateCtrlPref[BRAP_RM_P_MAX_ADAPTRATE_CTRL] = 
{
    0,  1,  2,  3, 4, 5, 6, 7
};
static const BRAP_CapInputPort BRAP_RM_P_InternalCapPortPref
    [BRAP_RM_P_MAX_INTERNAL_CAPPORTS] = 
{
     BRAP_CapInputPort_eIntCapPort0
    ,BRAP_CapInputPort_eIntCapPort1
    ,BRAP_CapInputPort_eIntCapPort2
    ,BRAP_CapInputPort_eIntCapPort3
    ,BRAP_CapInputPort_eIntCapPort4
};
#elif  (BRAP_7550_FAMILY == 1)
static const unsigned int BRAP_RM_P_SrcChPref[BRAP_RM_P_MAX_SRC_CHANNELS] = 
{
    0,  1,  2,  3,  4,  5,  6,  7, 8 
};

static const unsigned int BRAP_RM_P_AdaptRateCtrlPref[BRAP_RM_P_MAX_ADAPTRATE_CTRL] = 
{
    0,  1,  2,  3,  4,  5,  6,  7
};

#else
static const unsigned int BRAP_RM_P_SrcChPref[BRAP_RM_P_MAX_SRC_CHANNELS] = 
{
    0,  1,  2,  3,  4,  5,  6,  7 
};

static const unsigned int BRAP_RM_P_AdaptRateCtrlPref[BRAP_RM_P_MAX_ADAPTRATE_CTRL] = 
{
    0,  1,  2,  3
};
static const BRAP_CapInputPort BRAP_RM_P_InternalCapPortPref
    [BRAP_RM_P_MAX_INTERNAL_CAPPORTS] = 
{
     BRAP_CapInputPort_eIntCapPort0
    ,BRAP_CapInputPort_eIntCapPort1
    ,BRAP_CapInputPort_eIntCapPort2
    ,BRAP_CapInputPort_eIntCapPort3
};
#endif

static const unsigned int BRAP_RM_P_MixIpPb[BRAP_RM_P_MAX_PB_INPUT_PER_MIXER] =
{
    0,  1,  2,  3,  4,  5,  6,  7
};



#else
#error "Chip type NOT supported"
#endif

#if ((BRAP_3548_FAMILY == 1) || ( BRAP_7405_FAMILY == 1))
static const unsigned int BRAP_RM_P_DstChPref[BRAP_RM_P_MAX_DST_CHANNELS] = 
{
    0,  1,  2,  3
};
#else
static const unsigned int BRAP_RM_P_DstChPref[BRAP_RM_P_MAX_DST_CHANNELS] = 
{
    0,  1,  2,  3, 4, 5, 6, 7
};

#endif
static const unsigned int BRAP_RM_P_MixIpPri[BRAP_RM_P_MAX_PRIMARY_INPUT_PER_MIXER] =
{
    7
};

static const unsigned int BRAP_RM_P_MixIpSec[BRAP_RM_P_MAX_SECONDARY_INPUT_PER_MIXER] =
{
    0,  1,  2
};

static const unsigned int BRAP_RM_P_MixIpPref[BRAP_RM_P_MAX_MIXER_INPUTS] =
{
    0, 1, 2, 3, 4, 5, 6, 7
};


/***************************************************************************
Summary:
	Opens the resource manager.

Description:
	This function initializes Resource Manager data structures. This function 
	should be called before calling any other Resource Manager function.

Returns:
	BERR_SUCCESS if successful else error

See Also:
	BRAP_RM_P_Close
**************************************************************************/
BERR_Code 
BRAP_RM_P_Open( 
	BRAP_Handle		        hRap,		/* [in] RAP Device handle */
	BRAP_RM_Handle	        *phRm		/* [out]Resource Manager handle */
	)
{
	BRAP_RM_Handle	hRm = NULL;
	BERR_Code		ret = BERR_SUCCESS;
	int				i = 0, j = 0, k = 0;

	BDBG_ENTER(BRAP_RM_P_Open);

	/* Assert the function arguments*/
	BDBG_ASSERT(hRap);
	BDBG_ASSERT(phRm);

	/* Allocate the Resource Manager handle */
	hRm = (BRAP_RM_Handle)BKNI_Malloc( sizeof(BRAP_RM_P_Object));
	if(NULL == hRm)
	{
		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
	}

	/* Reset the entire structure */
	BKNI_Memset(hRm, 0, sizeof(BRAP_RM_P_Object) );

    /* Reset DSP Usage */
    for(i = 0; i < BRAP_RM_P_MAX_DSPS; i++)
	{
		for(j = 0; j < BRAP_RM_P_MAX_CXT_PER_DSP; j++)
		{
			hRm->sDspUsage[i].eContext[j] = BRAP_RM_P_ObjectState_eFree;
		}
		hRm->sDspUsage[i].uiDecCxtCount = 0;
		hRm->sDspUsage[i].uiSrcCxtCount = 0;
		hRm->sDspUsage[i].uiPtCxtCount = 0;
	}

    /* Reset Ring Buffer Usage */
	for(i = 0; i < BRAP_RM_P_MAX_RBUFS; i++)
	{
		hRm->eRBufState[i] = BRAP_RM_P_ObjectState_eFree;
	}
    
    /* Reset Source Fifo Usage */
	for(i = 0; i < BRAP_RM_P_MAX_SRC_CHANNELS; i++)
	{
		hRm->eSrcChState[i] = BRAP_RM_P_ObjectState_eFree;
	}

    /* Reset Destination Fifo Usage */
	for(i = 0; i < BRAP_RM_P_MAX_DST_CHANNELS; i++)
	{
		hRm->eDstChState[i] = BRAP_RM_P_ObjectState_eFree;
	}

    /* Reset Rate managerUsage */
	for(i = 0; i < BRAP_RM_P_MAX_ADAPTRATE_CTRL; i++)
	{
		hRm->eAdaptRateCtrlState[i] = BRAP_RM_P_ObjectState_eFree;
	}

    /* Reset SRC Usage */
    for(i = 0; i < BRAP_RM_P_MAX_SRC_BLCK; i++)
    {
        for(j = 0; j < BRAP_RM_P_MAX_SRC_PER_SRC_BLCK; j++)
        {
            hRm->sSrcUsage[i][j].uiUsageCount = 0;
            hRm->sSrcUsage[i][j].bSrcEq = false;            
        }
    }

    /* Reset Mixer Usage */
    for(i = 0; i < BRAP_RM_P_MAX_DP_BLCK; i++)
    {
        for(j = 0; j < BRAP_RM_P_MAX_MIXER_PER_DP_BLCK; j++)
        {
      		for(k = 0; k < BRAP_RM_P_MAX_MIXER_INPUTS; k++)
    		{
    			hRm->sMixerUsage[i][j].uiInputUsgCnt[k] = 0;
    		}
      		for(k = 0; k < BRAP_RM_P_MAX_MIXER_OUTPUTS; k++)
    		{
    			hRm->sMixerUsage[i][j].uiOutputUsgCnt[k] = 0;
    		}
        }
    }

    /* Reset SpdifFm Usage */
	for (i = 0; i < BRAP_RM_P_MAX_SPDIFFMS; i++)
	{
		for(j = 0; j < BRAP_RM_P_MAX_SPDIFFM_STREAMS; j++)
		{
			hRm->sSpdiffmUsage[i].uiUsageCount[j] = 0;
		}
	}

    /* Reset CapPort Usage */
	for (i = 0; i < BRAP_CapInputPort_eMax; i++)
	{
		hRm->sCapPortUsage[i].uiUsageCount = 0;
	}

    /* Reset FsTmgSrc Usage */
	for (i = 0; i < BRAP_RM_P_MAX_FS_TMG_SOURCES; i++)
    {
        hRm->sFsTmgSrcUsage[i].uiUsageCount = 0;
    }
    
	/* Store RM handle inside the RAP handle */
	hRap->hRm = hRm;
	*phRm = hRm;

	BDBG_LEAVE(BRAP_RM_P_Open);
    return ret;
}

/***************************************************************************
Summary:
	Closes the resource manager.
	
Description:
	This function shuts downs the Resource Manager and frees up the Resource 
	Manager handle. This function should be called while closing the audio 
	device.

Returns:
	BERR_SUCCESS if successful else error

See Also:
	BRAP_RM_P_Open
**************************************************************************/
BERR_Code 
BRAP_RM_P_Close( 
	BRAP_Handle		        hRap,		/* [in] Device Handle */
	BRAP_RM_Handle	        hRm			/* [in] The Resource Manager handle */
	)
{
	BDBG_ENTER(BRAP_RM_P_Close);

	/* Assert the function arguments*/
	BDBG_ASSERT(hRap);
	BDBG_ASSERT(hRm);

	/* Free up the RM handle */
	BKNI_Free(hRm);
	hRap->hRm = NULL;

	BDBG_LEAVE(BRAP_RM_P_Close);	
	return BERR_SUCCESS;
}

/***************************************************************************
Summary:
    Allocates requested resources.
    
Description:
	This function allocates all the resources required for opening a particular
	instance of audio operation.

Returns:
	BERR_SUCCESS if successful else error

See Also:
	BRAP_RM_P_FreeResources
**************************************************************************/
BERR_Code
BRAP_RM_P_AllocateResources (
	BRAP_RM_Handle				hRm,		/* [in] Resource Manager handle */
	const BRAP_RM_P_ResrcReq	*pResrcReq,	/* [in] Required resources */
	BRAP_RM_P_ResrcGrant		*pResrcGrant/* [out] Indices of the allocated 
	                                           resources */
	)
{
    BERR_Code       ret = BERR_SUCCESS;
    BRAP_RM_Handle  hRmTemp = NULL;

    BDBG_ENTER(BRAP_RM_P_AllocateResources);

	/* Assert the function arguments*/
	BDBG_ASSERT(hRm);
	BDBG_ASSERT(pResrcGrant);
  	BDBG_ASSERT(pResrcReq);

    /* Allocate a temporary RM Handle */
	hRmTemp = (BRAP_RM_Handle)BKNI_Malloc( sizeof(BRAP_RM_P_Object));
	if(hRmTemp == NULL)
	{
	    BDBG_ERR(("BRAP_RM_P_AllocateResources: Malloc failed"));
		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
	}
        BKNI_Memset(hRmTemp , 0, sizeof(BRAP_RM_P_Object)); 
	/* Copy the original RM handle content to the temporary handle */
	*hRmTemp = *hRm;

    /* Initialize pResrcGrant entries to invalid values */
    BRAP_RM_P_InitResourceGrant(pResrcGrant,true);

    /* Allocate DSP */
    ret = BRAP_RM_P_AllocateDsp(hRmTemp, pResrcReq, pResrcGrant);
    if(BERR_SUCCESS != ret)
    {
	    BDBG_ERR(("BRAP_RM_P_AllocateResources: Allocate DSP failed"));
		goto free_mem; 
    }

    /* Note: It helps to allocate corresponding SrcCh or DstCh before allocating
       any ring buffer */

    /* Allocate Source Channel */
    ret = BRAP_RM_P_AllocateSrcCh(hRmTemp, pResrcReq, pResrcGrant);
    if(BERR_SUCCESS != ret)
    {
	    BDBG_ERR(("BRAP_RM_P_AllocateResources: Allocate SrcCh failed"));
		goto free_mem; 
    }

    /* Allocate Destination Channel */
    ret = BRAP_RM_P_AllocateDstCh(hRmTemp, pResrcReq, pResrcGrant);
    if(BERR_SUCCESS != ret)
    {
	    BDBG_ERR(("BRAP_RM_P_AllocateResources: Allocate DstCh failed"));
		goto free_mem; 
    }
    
    /* Allocate Rate manager */
    ret = BRAP_RM_P_AllocateAdaptRateCtrl(hRmTemp, pResrcReq, pResrcGrant);
    if(BERR_SUCCESS != ret)
    {
	    BDBG_ERR(("BRAP_RM_P_AllocateResources: Allocate AdaptRateCtrl failed"));
		goto free_mem; 
    }
    
    /* Allocate Ring Buffer */
    ret = BRAP_RM_P_AllocateRBuf(hRmTemp, pResrcReq, pResrcGrant);
    if(BERR_SUCCESS != ret)
    {
	    BDBG_ERR(("BRAP_RM_P_AllocateResources: Allocate RBuf failed"));
		goto free_mem; 
    }

    /* Allocate SRC */
    ret = BRAP_RM_P_AllocateSrc(hRmTemp, pResrcReq, pResrcGrant);
    if(BERR_SUCCESS != ret)
    {
	    BDBG_ERR(("BRAP_RM_P_AllocateResources: Allocate SRC failed"));
		goto free_mem; 
    }

    /* Allocate Mixer */
    ret = BRAP_RM_P_AllocateMixer(hRmTemp, pResrcReq, pResrcGrant);
    if(BERR_SUCCESS != ret)
    {
	    BDBG_ERR(("BRAP_RM_P_AllocateResources: Allocate Mixer failed"));
		goto free_mem; 
    }

    /* Allocate SRC Equalizer */
    ret = BRAP_RM_P_AllocateSrcEq (hRmTemp, pResrcReq, pResrcGrant);
    if(BERR_SUCCESS != ret)
    {
	    BDBG_ERR(("BRAP_RM_P_AllocateResources: Allocate SRC failed"));
		goto free_mem; 
    }    

    /* Allocate Output Port */
    ret = BRAP_RM_P_AllocateOpPort(hRmTemp, pResrcReq, pResrcGrant);
    if(BERR_SUCCESS != ret)
    {
	    BDBG_ERR(("BRAP_RM_P_AllocateResources: Allocate Output Port failed"));
		goto free_mem; 
    }
#if (BRAP_7550_FAMILY != 1)
    /* Allocate Capture Port */
    ret = BRAP_RM_P_AllocateCapPort(hRmTemp, pResrcReq, pResrcGrant);
    if(BERR_SUCCESS != ret)
    {
	    BDBG_ERR(("BRAP_RM_P_AllocateResources: Allocate Capture Port failed"));
		goto free_mem; 
    }
#endif
    /* Allocate FS Timing Source (if any) */
    ret = BRAP_RM_P_AllocateFsTmgSource(hRmTemp, 
                                        &(pResrcReq->sFsTmgSrcReq),
                                        &(pResrcGrant->sFsTmgGrnt));
    if(BERR_SUCCESS != ret)
    {
        BDBG_ERR(("BRAP_RM_P_AllocateCapPort: Allocate Fs Timing Source failed"));
        goto free_mem;
    }

  	/* Currently we have only one FMM, so return FMM Id as 0 */
	pResrcGrant->uiFmmId = 0;

	/* Copy the temporary RM handle content to the original RM handle */
	*hRm = *hRmTemp;

    /* Even when no error, fall through ... */
free_mem:

	/* Free the temporary RM handle */
	BKNI_Free(hRmTemp);

	BDBG_LEAVE(BRAP_RM_P_AllocateResources);
    return ret;
}

/***************************************************************************
Summary:
    Private function that initializes the resource Request to invalid values.
**************************************************************************/
void 
BRAP_RM_P_InitResourceReq(
	BRAP_RM_P_ResrcReq          *pResrcReq/* [in] Resource Request structure 
                                                  to be inited to invalid
                                                  values */
)
{
    int i=0, j=0, k=0,l=0, m=0;

    BDBG_ENTER(BRAP_RM_P_InitResourceReq);
    BDBG_ASSERT(pResrcReq);

    /* Channel info */
    pResrcReq->eChType = BRAP_RM_P_INVALID_INDEX;
    pResrcReq->eChSubType = BRAP_RM_P_INVALID_INDEX;
    pResrcReq->ePath = BRAP_P_UsgPath_eMax;

    /* For DSP */
    pResrcReq->bAllocateDSP = false;

    /* For RBUF, Srcch,AdaptRateCtrl and DstCh */
    for (i=0;i<BRAP_RM_P_MAX_OP_CHANNEL_PAIRS;i++)
    {
        pResrcReq->sRbufReq[i].bAllocate = false;
        pResrcReq->sRbufReq[i].eBufDataMode = BRAP_BufDataMode_eMaxNum;
        pResrcReq->sSrcChReq[i].bAllocate = false;
        pResrcReq->sDstChReq[i].bAllocate = false;
        pResrcReq->sAdaptRateCtrlReq[i].bAllocate = false;        
    }

    /* For SRC n Mixer */
    for(i=0;i<BRAP_RM_P_MAX_MIXING_LEVELS;i++)
    {
        for(j=0;j<BRAP_RM_P_MAX_SRC_IN_CASCADE;j++)
        {
            for(k=0;k<BRAP_RM_P_MAX_OP_CHANNEL_PAIRS;k++)
            {
                for(l=0;l<BRAP_RM_P_MAX_PARALLEL_PATHS;l++)
                {
                    pResrcReq->sSrcMixerReq[i].sSrcReq[j][k][l].bAllocate = false;
                    pResrcReq->sSrcMixerReq[i].sSrcReq[j][k][l].sReallocateSrc.uiSrcBlkId
                                                      = BRAP_RM_P_INVALID_INDEX;
                    pResrcReq->sSrcMixerReq[i].sSrcReq[j][k][l].sReallocateSrc.uiSrcId
                                                      = BRAP_RM_P_INVALID_INDEX;
                    
                }
            }
        }
        for(k=0;k<BRAP_RM_P_MAX_OP_CHANNEL_PAIRS;k++)
        {
            for(l=0;l<BRAP_RM_P_MAX_PARALLEL_PATHS;l++)
            {
                pResrcReq->sSrcMixerReq[i].sMixerReq[k][l].bAllocate = false;
                pResrcReq->sSrcMixerReq[i].sMixerReq[k][l].uiNumNewInput = 
                                                    BRAP_RM_P_INVALID_INDEX;
                for(m=0;m<BRAP_RM_P_MAX_OP_CHANNEL_PAIRS;m++)
                {
                    pResrcReq->sSrcMixerReq[i].sMixerReq[k][l].bInputChPair[m]=false;
                }/* for m */
                
                pResrcReq->sSrcMixerReq[i].sMixerReq[k][l].uiNumNewOutput = 
                                                    BRAP_RM_P_INVALID_INDEX;

                for(m=0;m<BRAP_RM_P_MAX_MIXER_OUTPUTS;m++)
                {
                    pResrcReq->sSrcMixerReq[i].sMixerReq[k][l].uiMixerOutputId[m]
                                                    = BRAP_RM_P_INVALID_INDEX;
                }
                pResrcReq->sSrcMixerReq[i].sMixerReq[k][l].sReallocateMixer.uiDpId
                                                  = BRAP_RM_P_INVALID_INDEX;
                pResrcReq->sSrcMixerReq[i].sMixerReq[k][l].sReallocateMixer.uiMixerId
                                                  = BRAP_RM_P_INVALID_INDEX;
                
                for(j=0;j<BRAP_RM_P_MAX_OP_CHANNEL_PAIRS;j++)
                {
                    pResrcReq->sSrcMixerReq[i].sMixerReq[k][l].sReallocateMixer.uiMixerInputId[j]
                                                  = BRAP_RM_P_INVALID_INDEX;
                }
                pResrcReq->sSrcMixerReq[i].sMixerReq[k][l].sReallocateMixer.uiMixerOutputId[0]
                                                  = BRAP_RM_P_INVALID_INDEX;
                pResrcReq->sSrcMixerReq[i].sMixerReq[k][l].sReallocateMixer.uiMixerOutputId[1]
                                                  = BRAP_RM_P_INVALID_INDEX;
           }
        }
    }

    /* For SRC as Equalizer */
    for(j=0;j<BRAP_RM_P_MAX_SRC_IN_CASCADE;j++)
    {
        for(k=0;k<BRAP_RM_P_MAX_OP_CHANNEL_PAIRS;k++)
        {
            for(l=0;l<BRAP_RM_P_MAX_PARALLEL_PATHS;l++)
            {
                pResrcReq->sSrcEqReq[j][k][l].bAllocate = false;
                pResrcReq->sSrcEqReq[j][k][l].uiBlkId = BRAP_RM_P_INVALID_INDEX;                
                pResrcReq->sSrcEqReq[j][k][l].sReallocateSrcEq.uiSrcBlkId = BRAP_RM_P_INVALID_INDEX;
                pResrcReq->sSrcEqReq[j][k][l].sReallocateSrcEq.uiSrcId = BRAP_RM_P_INVALID_INDEX;
                
            }
        }
    }    
    
    /* For Output port */
    for(k=0;k<BRAP_RM_P_MAX_OP_CHANNEL_PAIRS;k++)
    {
        pResrcReq->sOpReq[k].bAllocate = false;
        pResrcReq->sOpReq[k].eMuxSelect = BRAP_OutputPort_eMax;
        pResrcReq->sOpReq[k].eOpPortType = BRAP_OutputPort_eMax;
    }

    /* For Cap Port */
    for (k=0;k<BRAP_RM_P_MAX_OP_CHANNEL_PAIRS;k++)
    {
        for(j=0;j<BRAP_P_MAX_OUT_PARALLEL_FMM_PATH;j++)
        {
            pResrcReq->sCapReq[k][j].bAllocate = false;
            pResrcReq->sCapReq[k][j].eCapPort = BRAP_CapInputPort_eMax;
        }
    }

    /* For FS Timing Source */
    pResrcReq->sFsTmgSrcReq.bAllocate = false;
    pResrcReq->sFsTmgSrcReq.uiFsTmgSrcId = BRAP_RM_P_INVALID_INDEX;

}

/***************************************************************************
Summary:
    Private function that initializes the resource grant to invalid values.
**************************************************************************/
void 
BRAP_RM_P_InitResourceGrant(
	BRAP_RM_P_ResrcGrant		*pResrcGrant,/* [in] Resource grant structure 
	                                                to be inited to invalid 
	                                                values */
        bool   		bFreeSrcChAndRbuf /*If this function is called for Playback Channel,
                                                                Decode PCM path And if bOpenTimeWrToRbuf = true 
                                                                then don't free Srcch and Rbuf which allocated in
                                                                ChannelOPen of Pb*/	                                                
   )
{
    int i = 0, j = 0, k = 0, l = 0;
    BRAP_RM_P_SrcMixerGrant *pSrcMixerGrnt = NULL;

	BDBG_ENTER(BRAP_RM_P_InitResourceGrant);    

	/* Malloc large structures */
	pSrcMixerGrnt = ( BRAP_RM_P_SrcMixerGrant *) BKNI_Malloc( sizeof( BRAP_RM_P_SrcMixerGrant ));
	if ( NULL==pSrcMixerGrnt )
	{
        BDBG_ERR(("BERR_OUT_OF_SYSTEM_MEMORY"));
        return;
	}
        BKNI_Memset(pSrcMixerGrnt , 0, sizeof(BRAP_RM_P_SrcMixerGrant));     
    pResrcGrant->uiDspId = BRAP_RM_P_INVALID_INDEX;
    pResrcGrant->uiDspContextId = BRAP_RM_P_INVALID_INDEX;
    pResrcGrant->uiFmmId = BRAP_RM_P_INVALID_INDEX;

if(bFreeSrcChAndRbuf == true)
{
    /* Rbuf */
    for(i = 0; i < BRAP_RM_P_MAX_OP_CHANNELS; i++)
    {
        pResrcGrant->uiRbufId[i] = BRAP_RM_P_INVALID_INDEX;
    }/* for i */
}    

    /* SrcCh,AdaptRateCtrl and DstCh*/
    for(i = 0; i < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; i++)
    {
        if(bFreeSrcChAndRbuf == true)    
        {
            pResrcGrant->uiSrcChId[i] = BRAP_RM_P_INVALID_INDEX;
        }
        pResrcGrant->uiDstChId[i] = BRAP_RM_P_INVALID_INDEX;
        pResrcGrant->uiAdaptRateCtrlId[i] = BRAP_RM_P_INVALID_INDEX;        
    }/* for i */

    /* sSrcGrant structure */
    for(j = 0; j < BRAP_RM_P_MAX_SRC_IN_CASCADE; j++)
    {
        for(k = 0; k < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; k++)
        {
            for(l = 0; l < BRAP_RM_P_MAX_PARALLEL_PATHS; l++)
            {
                pSrcMixerGrnt->sSrcGrant[j][k][l].uiSrcBlkId = 
                                            BRAP_RM_P_INVALID_INDEX;
                pSrcMixerGrnt->sSrcGrant[j][k][l].uiSrcId = 
                                            BRAP_RM_P_INVALID_INDEX;
            }/* for l */
        }/* for k */
    }/* for j */

    /* sMixerGrant structure */
    for(j = 0; j < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; j++)
    {
        for(k = 0; k < BRAP_RM_P_MAX_PARALLEL_PATHS; k++)
        {
            pSrcMixerGrnt->sMixerGrant[j][k].uiDpId = 
                                        BRAP_RM_P_INVALID_INDEX;
            pSrcMixerGrnt->sMixerGrant[j][k].uiMixerId = 
                                        BRAP_RM_P_INVALID_INDEX;
            for(l = 0; l < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; l++)
            {
                pSrcMixerGrnt->sMixerGrant[j][k].uiMixerInputId[l] = 
                                        BRAP_RM_P_INVALID_INDEX;
            }/* for l */

            for(l = 0; l < BRAP_RM_P_MAX_MIXER_OUTPUTS; l++)
            {
                pSrcMixerGrnt->sMixerGrant[j][k].uiMixerOutputId[l] = 
                                        BRAP_RM_P_INVALID_INDEX;
            }/* for l */
        }/* for k */
    }/* for j */

    /* sSrcMixerGrnt structure */
    for(i = 0; i < BRAP_RM_P_MAX_MIXING_LEVELS; i++)
    {
        pResrcGrant->sSrcMixerGrnt[i] = *pSrcMixerGrnt;
    }/* for i */

    /* sSrcEqGrant structure */
    for(j = 0; j < BRAP_RM_P_MAX_SRC_IN_CASCADE; j++)
    {
        for(k = 0; k < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; k++)
        {
            for(l = 0; l < BRAP_RM_P_MAX_PARALLEL_PATHS; l++)
            {
                pResrcGrant->sSrcEqGrant[j][k][l].uiSrcBlkId = 
                                            BRAP_RM_P_INVALID_INDEX;
                pResrcGrant->sSrcEqGrant[j][k][l].uiSrcId = 
                                            BRAP_RM_P_INVALID_INDEX;
            }/* for l */
        }/* for k */
    }/* for j */    

    /* sOpPortGrnt structure */
    for(i = 0; i < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; i++)
    {
        pResrcGrant->sOpPortGrnt[i].uiSpdiffmId = BRAP_RM_P_INVALID_INDEX;
        pResrcGrant->sOpPortGrnt[i].uiSpdiffmStreamId = BRAP_RM_P_INVALID_INDEX;
        pResrcGrant->sOpPortGrnt[i].eOutputPort = BRAP_OutputPort_eMax;
    }/* for i */

    /* sCapPortGrnt structure */
    for(i = 0; i < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; i++)
    {
        for(j=0;j<BRAP_P_MAX_OUT_PARALLEL_FMM_PATH;j++)
        {
            pResrcGrant->sCapPortGrnt[i][j].eCapPort = BRAP_CapInputPort_eMax;
        }
    }/* for i */

    /* sFsTmgGrnt structure */
    pResrcGrant->sFsTmgGrnt.uiFsTmgSrc = BRAP_RM_P_INVALID_INDEX;

    BKNI_Free(pSrcMixerGrnt);
    BDBG_LEAVE(BRAP_RM_P_InitResourceGrant);    
    return;
}

/***************************************************************************
Summary:
    Private function that allocates DSP. 

    Note: This private function will not do any malloc and pointer checks. 
    This is quite a low level helper function. Also, this function should 
    not init the pResrcGrant. It can just modify the respective allocated
    resource Id.
**************************************************************************/
static BERR_Code 
BRAP_RM_P_AllocateDsp(
	BRAP_RM_Handle				hRm,		/* [in] Resource Manager handle */
	const BRAP_RM_P_ResrcReq	*pResrcReq,	/* [in] Required resources */
	BRAP_RM_P_ResrcGrant		*pResrcGrant/* [out] Indices of the allocated 
	                                           resources */
    )
{
    BERR_Code       ret = BERR_SUCCESS;
	unsigned int	uiTotDSPContext = 0;
    unsigned int    i=0;

    BDBG_ENTER(BRAP_RM_P_AllocateDsp);

    BDBG_MSG(("bAllocateDsp = %d ePath = %d",
        pResrcReq->bAllocateDSP, pResrcReq->ePath));

	/* Allocated DSP for a decode channel */
    if((true == pResrcReq->bAllocateDSP))
    {
		/* Currently, Decode is happening in DSP0 */
		pResrcGrant->uiDspId = BRAP_RM_P_DEC_DSP_ID;

        /* Sanity checks */
        uiTotDSPContext = hRm->sDspUsage[pResrcGrant->uiDspId].uiDecCxtCount 
          + hRm->sDspUsage[pResrcGrant->uiDspId].uiPtCxtCount
		  + hRm->sDspUsage[pResrcGrant->uiDspId].uiSrcCxtCount;
  		if(uiTotDSPContext >= BRAP_RM_P_MAX_CXT_PER_DSP)
		{
			BDBG_ERR(("No DSP Contexts are free in DSP %d",
                pResrcGrant->uiDspId));
			return BERR_TRACE(BRAP_ERR_RESOURCE_EXHAUSTED);
		}

		/* Check for the free context */
		for(i=0; i < BRAP_RM_P_MAX_CXT_PER_DSP; i++)
		{
			if(BRAP_RM_P_ObjectState_eFree ==
                hRm->sDspUsage[pResrcGrant->uiDspId].eContext[i])
			{
				break;
			}
		}

        /* Check if i exceeded MAX contexts */
		if (i >= BRAP_RM_P_MAX_CXT_PER_DSP)
		{
			/* There is no free context available */
			ret = BERR_TRACE(BRAP_ERR_RESOURCE_EXHAUSTED);
			goto error; 
		}	
        else
        {
            /* Allocate the DSP context */
    		pResrcGrant->uiDspContextId = i;

       		/* Update the DSP context usage */
    		hRm->sDspUsage[pResrcGrant->uiDspId].eContext[pResrcGrant->uiDspContextId] 
    			= BRAP_RM_P_ObjectState_eBusy;
    		hRm->sDspUsage[pResrcGrant->uiDspId].uiDecCxtCount++;

            BDBG_MSG(("Resource Allocated: DspId (%d) DspContextId (%d)",
                pResrcGrant->uiDspId, pResrcGrant->uiDspContextId));
        }
    }

        ret = BERR_SUCCESS;


error:
    BDBG_LEAVE(BRAP_RM_P_AllocateDsp);
    return ret;
}

/***************************************************************************
Summary:
    Private function that allocates Ring Buffer.

    Note: This private function will not do any malloc and pointer checks. 
    This is quite a low level helper function. Also, this function should 
    not init the pResrcGrant. It can just modify the respective allocated
    resource Id.
**************************************************************************/
static BERR_Code 
BRAP_RM_P_AllocateRBuf(
	BRAP_RM_Handle				hRm,		/* [in] Resource Manager handle */
	const BRAP_RM_P_ResrcReq	*pResrcReq,	/* [in] Required resources */
	BRAP_RM_P_ResrcGrant		*pResrcGrant/* [out] Indices of the allocated 
	                                           resources */
    )
{
    BERR_Code                   ret = BERR_SUCCESS;
    BRAP_OutputChannelPair      eChP = BRAP_OutputChannelPair_eLR;
    unsigned int                i = 0;                                    
    unsigned int                uiFifoId = 0;                                    
    unsigned int                uiSrcCh = 0;                                    
    unsigned int                uiDstCh = 0;                                    
    unsigned int                uiRbufId[BRAP_RM_P_MAX_RBUFS_PER_SRCCH] = {
                                    BRAP_RM_P_INVALID_INDEX, 
                                    BRAP_RM_P_INVALID_INDEX };
    
    BDBG_ENTER(BRAP_RM_P_AllocateRBuf);

    BDBG_MSG(("AllocateRBuf: Rbuf Request "));
    for(eChP = 0; eChP < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; eChP++)
    {
        BDBG_MSG(("\t ChPair[%d]: bAllocate = %d  eBufDataMode = %d",
            eChP, pResrcReq->sRbufReq[eChP].bAllocate, 
            pResrcReq->sRbufReq[eChP].eBufDataMode));
    }
    
    for(eChP = 0; eChP < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; eChP++)
    {
        if(pResrcReq->sRbufReq[eChP].bAllocate)
        {
            if(pResrcGrant->uiSrcChId[eChP] != 
                (unsigned int)BRAP_RM_P_INVALID_INDEX)
            {
                /* If corresponding SrcCh is allocated then 
                   Rbuf0 = SrcChId * 2 and 
                   Rbuf1 = (SrcCh Index*2) + 1 */
                uiRbufId[0] = pResrcGrant->uiSrcChId[eChP] << 1;
                uiRbufId[1] = uiRbufId[0] + 1;
            }
            else if(pResrcGrant->uiDstChId[eChP] != BRAP_RM_P_INVALID_INDEX)
            {
                /*If corresponding DstCh is allocated
                For 7440
	                Rbuf0 = (24 + DstCh Index) * 2 and 
	                Rbuf1 = ((24 + DstCh Index) * 2) + 1
	            For 3563
                Rbuf0 = (8 + DstCh Index) * 2 and 
                Rbuf1 = ((8 + DstCh Index) * 2) + 1*/

                uiRbufId[0] = (pResrcGrant->uiDstChId[eChP] + BRAP_RM_P_DST_CH_OFFSET) << 1;
                uiRbufId[1] = uiRbufId[0] + 1;
            }
            else
            {
                /* If neither of the corresponding SrcCh nor DstCh is allocated,
                   allocate first free set of Rbufs for this channel pair */
                for(i = 0; i < (BRAP_RM_P_MAX_RBUFS-1); i+=2)
                {
                    if((BRAP_RM_P_ObjectState_eFree == hRm->eRBufState[i]) &&
                       (BRAP_RM_P_ObjectState_eFree == hRm->eRBufState[i+1]))
                    {
                        uiRbufId[0] = i;
                        uiRbufId[1] = i + 1;

                        /* As there is 1-to-1 mapping between Rbuf-SrcCh/DstCh,
                           SrcCh/DstCh should also be marked busy */
                        uiFifoId = uiRbufId[1] >> 1;   
                        if(uiFifoId < BRAP_RM_P_MAX_SRC_CHANNELS)
                        {
                            /* Rbuf mapped to SrcCh */
                            uiSrcCh = uiFifoId;
                            hRm->eSrcChState[uiSrcCh] = 
                                            BRAP_RM_P_ObjectState_eBusy;
                        }
                        else
                        {
                            /* Rbuf mapped to DstCh */
                            uiDstCh = uiFifoId - BRAP_RM_P_MAX_SRC_CHANNELS;
                            hRm->eDstChState[uiDstCh] = 
                                            BRAP_RM_P_ObjectState_eBusy;
                        }

                        /* Allocation done for this channel pair */
                        break;
                    }/* if rbuf free */ 
                }/* for i */
            }/* if pResrcGrant->uiSrcChId[eChP] */

            /* Rbufs should have been allocated by now */
            /* Update the usage in hRm and prepare the grant structure */
            if((uiRbufId[0] != (unsigned int)BRAP_RM_P_INVALID_INDEX) &&
               (uiRbufId[1] != (unsigned int)BRAP_RM_P_INVALID_INDEX))
            {
                hRm->eRBufState[uiRbufId[0]] = BRAP_RM_P_ObjectState_eBusy;
                hRm->eRBufState[uiRbufId[1]] = BRAP_RM_P_ObjectState_eBusy;

                /* Out of the 2 ring buffers, return 1/2 depending upon 
                   user's request */
                if(BRAP_BufDataMode_eStereoNoninterleaved == 
                                            pResrcReq->sRbufReq[eChP].eBufDataMode)
                {
                    pResrcGrant->uiRbufId[eChP*2] = uiRbufId[0];
                    pResrcGrant->uiRbufId[(eChP*2)+1] = uiRbufId[1];
                }
                else
                {
                    pResrcGrant->uiRbufId[eChP*2] = uiRbufId[0];
                }
            }
            else
            {
                BDBG_ERR(("Unable to allocate ring buffers"));
                return BERR_TRACE(BRAP_ERR_RESOURCE_EXHAUSTED);
            }            
        }/* if pResrcReq->sRbufReq[eChP].bAllocate */
    }/* for i */

    BDBG_MSG(("AllocateRBuf: Rbuf Grant "));
    for(i = 0; i < BRAP_RM_P_MAX_OP_CHANNELS; i++)
    {
        BDBG_MSG(("\t pResrcGrant->uiRbufId[%d] = %d",
            i, pResrcGrant->uiRbufId[i]));
    }

    BDBG_LEAVE(BRAP_RM_P_AllocateRBuf);
    return ret;
}

/***************************************************************************
Summary:
    Private function that allocates Source Channel. 

    Note: This private function will not do any malloc and pointer checks. 
    This is quite a low level helper function. Also, this function should 
    not init the pResrcGrant. It can just modify the respective allocated
    resource Id.
**************************************************************************/
static BERR_Code 
BRAP_RM_P_AllocateSrcCh(
	BRAP_RM_Handle				hRm,		/* [in] Resource Manager handle */
	const BRAP_RM_P_ResrcReq	*pResrcReq,	/* [in] Required resources */
	BRAP_RM_P_ResrcGrant		*pResrcGrant/* [out] Indices of the allocated 
	                                           resources */
    )
{
    BERR_Code               ret = BERR_SUCCESS;
    unsigned int            uiNumSrcCh = 0;
    unsigned int            uiIdx = 0;
    unsigned int            j = 0; 
    BRAP_OutputChannelPair  eChP = 0;
    

    BDBG_ENTER(BRAP_RM_P_AllocateSrcCh);

    BDBG_MSG(("AllocateSrcCh: SrcCh Request "));

    /* Get the number of SrcCh requested by the AM */
    for(eChP = 0; eChP < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; eChP++)
    {
        if(true == pResrcReq->sSrcChReq[eChP].bAllocate)
        {
            uiNumSrcCh++;
        }
        BDBG_MSG(("\t ChPair[%d]: bAllocate = %d", 
            eChP, pResrcReq->sSrcChReq[eChP].bAllocate));
    }/* for eChP */

    BDBG_MSG(("AllocateSrcCh: SrcCh Grant "));
    /* If none is requested, return success without allocating any */
    if(0 == uiNumSrcCh)
    {
        /* Do not allocate any, just return success */
        BDBG_MSG(("\t None"));
    }
    else
    {
        /* Find the starting index in BRAP_RM_P_SrcChPref[] that fulfills the
           required number of free & continuous SrcCh */
        for(uiIdx = 0; (uiIdx + uiNumSrcCh)<BRAP_RM_P_MAX_SRC_CHANNELS; uiIdx++)
        {
            if(BRAP_RM_P_ObjectState_eFree == 
                    hRm->eSrcChState[BRAP_RM_P_SrcChPref[uiIdx]])
            {
                /* Check if required number of continuous SrcCh are free */
                for(j = 0; j < uiNumSrcCh; j++)
                {
                    if(BRAP_RM_P_ObjectState_eBusy == 
                            hRm->eSrcChState[BRAP_RM_P_SrcChPref[uiIdx + j]]) 
                    {
                        break;
                    }
                }/* for j */
                
                if(uiNumSrcCh == j)
                {
                    /* Eureka!! */
                    break;
                }/* if */
            }/* if SrcChState == free */
        }/* for uiIdx */

        /* Allocate and update the usage for these SrcCh */
        for(eChP = 0, j = 0; 
           (eChP < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS) && (j < uiNumSrcCh);
            eChP++)
        {
            if(true == pResrcReq->sSrcChReq[eChP].bAllocate)
            {
                /* Allocate */
                pResrcGrant->uiSrcChId[eChP] = 
                        BRAP_RM_P_SrcChPref[uiIdx + j];
                BDBG_MSG(("\t pResrcGrant->uiSrcChId[%d] = %d",
                        eChP, pResrcGrant->uiSrcChId[eChP]));

                /* Update the usage info */
                hRm->eSrcChState[BRAP_RM_P_SrcChPref[uiIdx + j]] = 
                        BRAP_RM_P_ObjectState_eBusy;

                /* Increment j */
                j++;                
                
            }/* if */
        }/* for eChP */                
    }/* if - else */

    BDBG_LEAVE(BRAP_RM_P_AllocateSrcCh);
    return ret;
}

/***************************************************************************
Summary:
    Private function that allocates Destination Channel. 

    Note: This private function will not do any malloc and pointer checks. 
    This is quite a low level helper function. Also, this function should 
    not init the pResrcGrant. It can just modify the respective allocated
    resource Id.
**************************************************************************/
static BERR_Code 
BRAP_RM_P_AllocateDstCh(
	BRAP_RM_Handle				hRm,		/* [in] Resource Manager handle */
	const BRAP_RM_P_ResrcReq	*pResrcReq,	/* [in] Required resources */
	BRAP_RM_P_ResrcGrant		*pResrcGrant/* [out] Indices of the allocated 
	                                           resources */
    )
{
    BERR_Code               ret = BERR_SUCCESS;
    unsigned int            uiNumDstCh = 0;
    unsigned int            uiIdx = 0;
    unsigned int            j = 0; 
    BRAP_OutputChannelPair  eChP = 0;
    bool                    bDstChFound = false;
    

    BDBG_ENTER(BRAP_RM_P_AllocateDstCh);

    BDBG_MSG(("AllocateDstCh: DstCh Request "));

    /* Get the number of SrcCh requested by the AM */
    for(eChP = 0; eChP < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; eChP++)
    {
        if(true == pResrcReq->sDstChReq[eChP].bAllocate)
        {
            uiNumDstCh++;
        }
        BDBG_MSG(("\t ChPair[%d]: bAllocate = %d", 
            eChP, pResrcReq->sDstChReq[eChP].bAllocate));
    }/* for eChP */

    BDBG_MSG(("AllocateDstCh: DstCh Grant "));
    /* If none is requested, return success without allocating any */
    if(0 == uiNumDstCh)
    {
        /* Do not allocate any, just return success */
        BDBG_MSG(("\t None"));
    }
    else
    {
        /* Find the starting index in BRAP_RM_P_DstChPref[] that fulfills the
           required number of free & continuous SrcCh */
        for(uiIdx = 0; (uiIdx + uiNumDstCh)<BRAP_RM_P_MAX_DST_CHANNELS; uiIdx++)
        {
            if(BRAP_RM_P_ObjectState_eFree == 
                    hRm->eDstChState[BRAP_RM_P_DstChPref[uiIdx]])
            {
                /* Check if required number of continuous SrcCh are free */
                for(j = 0; j < uiNumDstCh; j++)
                {
                    if(BRAP_RM_P_ObjectState_eBusy == 
                            hRm->eDstChState[BRAP_RM_P_DstChPref[uiIdx + j]]) 
                    {
                        break;
                    }
                }/* for j */
                
                if(uiNumDstCh == j)
                {
                    /* Eureka!! */
                    bDstChFound = true;
                    break;
                }/* if */
            }/* if SrcChState == free */
        }/* for uiIdx */

        if(false == bDstChFound)
        {
            BDBG_ERR(("BRAP_RM_P_AllocateDstCh: Requsted %d continuos DstCh"
                      "not available",uiNumDstCh));
            return BERR_TRACE(BRAP_ERR_RESOURCE_EXHAUSTED);
        }
        
        /* Allocate and update the usage for these SrcCh */
        for(eChP = 0, j = 0; 
           (eChP < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS) && (j < uiNumDstCh);
            eChP++)
        {
            if(true == pResrcReq->sDstChReq[eChP].bAllocate)
            {
                /* Allocate */
                pResrcGrant->uiDstChId[eChP] = 
                        BRAP_RM_P_DstChPref[uiIdx + j];
                BDBG_MSG(("\t pResrcGrant->uiDstChId[%d] = %d",
                        eChP, pResrcGrant->uiDstChId[eChP]));

                /* Update the usage info */
                hRm->eDstChState[BRAP_RM_P_DstChPref[uiIdx + j]] = 
                        BRAP_RM_P_ObjectState_eBusy;

                /* Increment j */
                j++;                
                
            }/* if */
        }/* for eChP */                
    }/* if - else */

    BDBG_LEAVE(BRAP_RM_P_AllocateDstCh);
    return ret;
}


/***************************************************************************
Summary:
    Private function that allocates Rate Manager. 

    Note: This private function will not do any malloc and pointer checks. 
    This is quite a low level helper function. Also, this function should 
    not init the pResrcGrant. It can just modify the respective allocated
    resource Id.
**************************************************************************/
static BERR_Code 
BRAP_RM_P_AllocateAdaptRateCtrl(
	BRAP_RM_Handle				hRm,		/* [in] Resource Manager handle */
	const BRAP_RM_P_ResrcReq	*pResrcReq,	/* [in] Required resources */
	BRAP_RM_P_ResrcGrant		*pResrcGrant/* [out] Indices of the allocated 
	                                           resources */
    )
{
    BERR_Code               ret = BERR_SUCCESS;
    unsigned int            uiIdx = 0;
    BRAP_OutputChannelPair  eChP = 0;
    
    BDBG_ENTER(BRAP_RM_P_AllocateAdaptRateCtrl);

    BDBG_MSG(("BRAP_RM_P_AllocateAdaptRateCtrl: AdaptRateCtrl Request "));

    /* Get the number of AdaptRateCtrl requested by the AM */
    for(eChP = 0; eChP < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; eChP++)
    {
        if(true == pResrcReq->sAdaptRateCtrlReq[eChP].bAllocate)
        {
            for(uiIdx = 0; uiIdx <BRAP_RM_P_MAX_ADAPTRATE_CTRL; uiIdx++)
            {
                if(BRAP_RM_P_ObjectState_eFree ==hRm->eAdaptRateCtrlState[BRAP_RM_P_AdaptRateCtrlPref[uiIdx]])
                {
                    pResrcGrant->uiAdaptRateCtrlId[eChP] = BRAP_RM_P_AdaptRateCtrlPref[uiIdx];
                    
                    BDBG_MSG(("\tBRAP_RM_P_AllocateAdaptRateCtrl:  pResrcGrant->uiAdaptRateCtrlId[%d] = %d",
                            eChP, pResrcGrant->uiAdaptRateCtrlId[eChP]));

                    /* Update the usage info */
                    hRm->eAdaptRateCtrlState[BRAP_RM_P_AdaptRateCtrlPref[uiIdx]] =  BRAP_RM_P_ObjectState_eBusy;            
                    break;                    
                }/* if AdaptRateCtrlState == free */
            }/* for uiIdx */
            if(uiIdx >=BRAP_RM_P_MAX_ADAPTRATE_CTRL)
            {
                BDBG_ERR(("BRAP_RM_P_AllocateAdaptRateCtrl: No more free Rate Manager available"));
                return BERR_TRACE(BRAP_ERR_RESOURCE_EXHAUSTED);                
            }
        }
        BDBG_MSG(("\t BRAP_RM_P_AllocateAdaptRateCtrl: ChPair[%d]: bAllocate = %d", 
            eChP, pResrcReq->sAdaptRateCtrlReq[eChP].bAllocate));
    }/* for eChP */

    BDBG_LEAVE(BRAP_RM_P_AllocateAdaptRateCtrl);
    return ret;
}



/***************************************************************************
Summary:
    Private function that allocates SRC. 

    Note: This private function will not do any malloc and pointer checks. 
    This is quite a low level helper function. Also, this function should 
    not init the pResrcGrant. It can just modify the respective allocated
    resource Id.
**************************************************************************/
static BERR_Code 
BRAP_RM_P_AllocateSrc(
	BRAP_RM_Handle				hRm,		/* [in] Resource Manager handle */
	const BRAP_RM_P_ResrcReq	*pResrcReq,	/* [in] Required resources */
	BRAP_RM_P_ResrcGrant		*pResrcGrant/* [out] Indices of the allocated 
	                                           resources */
    )
{
    BERR_Code           ret = BERR_SUCCESS;
    int                 lvl=0,csc=0,chp=0,pp=0;
    int                 src_blk=0,src_id=0;
    unsigned int        i=0;
    BRAP_RM_P_SrcReq    sSrcReq;
    BRAP_RM_P_SrcGrant  sSrcGrant;
    bool                bSrcAllocated = false;
    bool                bSrcAllocateRequest =false;
    bool                bNewSrcAllocateRequest =false;
    bool                bOldSrcAllocateRequest =false;
    bool                bSrcsAvailable = false;

    unsigned int        uiNumSrc[BRAP_RM_P_MAX_MIXING_LEVELS]
                                [BRAP_RM_P_MAX_PARALLEL_PATHS]
                                [BRAP_RM_P_MAX_SRC_IN_CASCADE];

    BDBG_ENTER(BRAP_RM_P_AllocateSrc);

    /* Validate the inputs */
    BDBG_ASSERT(hRm);
    BDBG_ASSERT(pResrcReq);
    BDBG_ASSERT(pResrcGrant);

    for (lvl=0;lvl<BRAP_RM_P_MAX_MIXING_LEVELS;lvl++)
    {
        for(pp=0;pp<BRAP_RM_P_MAX_PARALLEL_PATHS;pp++)
        {
            for(csc=0; csc<BRAP_RM_P_MAX_SRC_IN_CASCADE; csc++)
            {
                uiNumSrc[lvl][pp][csc]=0;
            }
        }
    }

    /* Trace the pResrcReq for allocating the SRC. We need to first allocate all
       the SRC for all the channel pairs in one parallel path in one level and 
       then go to next parallel path. This is required because in a parallel
       path, all the SRC for different channel pairs should be contiguous */

    BDBG_MSG(("AllocateSrc: Fresh SRC Requests"));
    /* Find out the number of continuous new SRCs required for each lvl and pp */
    for (lvl=0;lvl<BRAP_RM_P_MAX_MIXING_LEVELS;lvl++)
    {
        for(pp=0;pp<BRAP_RM_P_MAX_PARALLEL_PATHS;pp++)
        {
            for(chp=0;chp<BRAP_RM_P_MAX_OP_CHANNEL_PAIRS;chp++)
            {
                for(csc=0;csc<BRAP_RM_P_MAX_SRC_IN_CASCADE;csc++)
                {                
                    sSrcReq = pResrcReq->sSrcMixerReq[lvl].sSrcReq[csc][chp][pp];
                    if ((true == sSrcReq.bAllocate)&&
                        (BRAP_RM_P_INVALID_INDEX==sSrcReq.sReallocateSrc.uiSrcBlkId) &&
                        (BRAP_RM_P_INVALID_INDEX==sSrcReq.sReallocateSrc.uiSrcId))
                    {                        
                        uiNumSrc[lvl][pp][csc]++;
                        bNewSrcAllocateRequest = true;

                        BDBG_MSG(("\t pResrcReq->sSrcMixerReq[%d].sSrcReq[%d][%d][%d]\n"
                            "\t sSrcReq: bAllocate = %d\n",
                            lvl,csc,chp,pp,
                            sSrcReq.bAllocate));
                    }
                    else if ((true == sSrcReq.bAllocate)&&
                        (BRAP_RM_P_INVALID_INDEX!=sSrcReq.sReallocateSrc.uiSrcBlkId) &&
                        (BRAP_RM_P_INVALID_INDEX!=sSrcReq.sReallocateSrc.uiSrcId))
                    {                        
                        bOldSrcAllocateRequest = true;

                        BDBG_MSG(("\t pResrcReq->sSrcMixerReq[%d].sSrcReq[%d][%d][%d]\n"
                            "\t sSrcReq: bAllocate = %d\n"
                            "\t sSrcReq.sReallocateSrc.uiSrcBlkId = %d\n"
                            "\t SrcReq.sReallocateSrc.uiSrcId =%d\n",
                            lvl,csc,chp,pp,
                            sSrcReq.bAllocate,sSrcReq.sReallocateSrc.uiSrcBlkId,
                            sSrcReq.sReallocateSrc.uiSrcId));
                    }
                }
            }
        }
    }

    if(true == bNewSrcAllocateRequest)
    {
        BDBG_MSG(("AllocateSrc: Fresh SRC Grants"));
        for (lvl=0;lvl<BRAP_RM_P_MAX_MIXING_LEVELS;lvl++)
        {
            for(pp=0;pp<BRAP_RM_P_MAX_PARALLEL_PATHS;pp++)
            {
                for(csc=0;csc<BRAP_RM_P_MAX_SRC_IN_CASCADE;csc++)
                {
                    bSrcsAvailable = false;
                    if(0 < uiNumSrc[lvl][pp][csc])
                    {
                        if((BRAP_P_UsgPath_eDecodePcm == pResrcReq->ePath)||
                           (BRAP_P_UsgPath_eMixPath == pResrcReq->ePath)||
                           (BRAP_P_UsgPath_eSharedPP== pResrcReq->ePath)||                           
                           (BRAP_P_UsgPath_eDownmixedMixPath== pResrcReq->ePath)||                               
#if (BRAP_3548_FAMILY == 1)
                           (BRAP_P_UsgPath_eCapture == pResrcReq->ePath)||
#endif
                           (BRAP_P_UsgPath_eDecodeCompress == pResrcReq->ePath)||
                           (BRAP_P_UsgPath_ePPBranch == pResrcReq->ePath)||
                           (BRAP_P_UsgPath_eDownmixedPath== pResrcReq->ePath)||
                           (BRAP_P_UsgPath_eDecodePcmPostMixing== pResrcReq->ePath)||
                           (BRAP_P_UsgPath_ePPBranchPostMixing== pResrcReq->ePath)||                           
                           (BRAP_P_UsgPath_eDecodeCompressPostMixing== pResrcReq->ePath))
                        {
                            src_blk = 0;
                        }
                        else if (((BRAP_P_UsgPath_eDecodePcm == pResrcReq->ePath) &&
                                 (BRAP_ChannelType_ePcmPlayback == pResrcReq->eChType)))         
                        {
                            src_blk = 1;
                        }
                        else
                        {
                            BDBG_ERR(("Unsupported Path %d",pResrcReq->ePath));
                            return BERR_TRACE(BERR_NOT_INITIALIZED);
                        }

                            for(src_id=0;src_id<BRAP_RM_P_MAX_SRC_PER_SRC_BLCK;src_id++)
                            {
                                BDBG_MSG(("SRC_Alloc: uiUsgCnt =%d,uiNumSrc[%d][%d][%d]=%d",
                                    hRm->sSrcUsage[src_blk][src_id].uiUsageCount,
                                    lvl,pp,csc,uiNumSrc[lvl][pp][csc]));
                                /* Find a free SRC */
                                if (0 == hRm->sSrcUsage[src_blk][src_id].uiUsageCount)
                                {
                                    bSrcsAvailable = true;
                                    BDBG_MSG((" available src_blk=%d src_id=%d",src_blk,src_id));
                                    for(i=1;i<uiNumSrc[lvl][pp][csc];i++)
                                    {
                                        if ((0 == hRm->sSrcUsage[src_blk][src_id+i].uiUsageCount)&&
                                            (BRAP_RM_P_MAX_SRC_PER_SRC_BLCK > (src_id+i)))
                                        {
                                            BDBG_MSG((" available src_blk=%d src_id=%d",src_blk,src_id+i));
                                            bSrcsAvailable = true;
                                        }
                                        else
                                        {
                                            BDBG_MSG((" NOT available src_blk=%d src_id=%d",src_blk,src_id+i));
                                            /* this src_id doesn't have required continuous SRCs free */
                                            bSrcsAvailable = false;
                                            break;
                                        }
                                    }/* for i */
                                }
                                if(true == bSrcsAvailable)
                                    break;
                            }/* for src_id */

                        if(false == bSrcsAvailable)
                        {
                            BDBG_ERR(("BRAP_RM_P_AllocateSrc: Could not Allocate SRC, no Free SRC"));
                            return BERR_TRACE(BRAP_ERR_RESOURCE_EXHAUSTED);
                        }

                for(chp = 0,i=0; chp < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; chp++,i++)
                {
                        sSrcGrant =
                            pResrcGrant->sSrcMixerGrnt[lvl].sSrcGrant[csc][chp][pp];
                        
                        /* Check if SRC is requested */
                        if (true == pResrcReq->sSrcMixerReq[lvl].sSrcReq[csc]
                                                           [chp][pp].bAllocate)
                        {
                            bSrcAllocateRequest = true;
                                
                                /* Return the ID's in grant structure */
                                sSrcGrant.uiSrcBlkId = src_blk;
                                sSrcGrant.uiSrcId = src_id + i;
                                /* Update the Usage */
                                hRm->sSrcUsage[src_blk][src_id+i].uiUsageCount++;
                                
                            bSrcAllocated = true;
                        }
                        pResrcGrant->sSrcMixerGrnt[lvl].sSrcGrant[csc][chp][pp] = 
                                                                          sSrcGrant;
                        if((BRAP_RM_P_INVALID_INDEX != sSrcGrant.uiSrcBlkId)&&
                           (BRAP_RM_P_INVALID_INDEX != sSrcGrant.uiSrcId))
                        {
                            BDBG_MSG(("\t pResrcGrant->sSrcMixerGrnt[%d].sSrcGrant[%d][%d][%d]\n"
                                "\t sSrcGrant.uiSrcBlkId = %d \n\t sSrcGrant.uiSrcId = %d\n",
                                lvl,csc,chp,pp,sSrcGrant.uiSrcBlkId,sSrcGrant.uiSrcId));
                        }
                    }/* for chp */
                    }/* if uiNumSrc */
                }/* for csc */
            }/* for pp */
        }/* for lvl */
    }/* if */

    if(true == bOldSrcAllocateRequest)
    {
        BDBG_MSG(("AllocateSrc: Reallocate SRC Requests & Grants"));
        for (lvl=0;lvl<BRAP_RM_P_MAX_MIXING_LEVELS;lvl++)
        {
            for(pp=0;pp<BRAP_RM_P_MAX_PARALLEL_PATHS;pp++)
            {
                for(csc=0;csc<BRAP_RM_P_MAX_SRC_IN_CASCADE;csc++)
                {
                    for(chp=0;chp<BRAP_RM_P_MAX_OP_CHANNEL_PAIRS;chp++)
                    {
                        sSrcReq =pResrcReq->sSrcMixerReq[lvl].sSrcReq[csc][chp][pp];
                        sSrcGrant =
                            pResrcGrant->sSrcMixerGrnt[lvl].sSrcGrant[csc][chp][pp];
                        
                        /* Check if request is to reallocate an already 
                           allocated SRC */
                        if ((true == sSrcReq.bAllocate)&&
                            (BRAP_RM_P_INVALID_INDEX != sSrcReq.sReallocateSrc.uiSrcBlkId) &&
                            (BRAP_RM_P_INVALID_INDEX != sSrcReq.sReallocateSrc.uiSrcId))
                        {
                            BDBG_MSG(("\t pResrcReq->sSrcMixerReq[%d].sSrcReq[%d][%d][%d]\n"
                                        "\t sSrcReq: bAllocate = %d\n"
                                        "\t sReallocateSrc.uiSrcBlkId = %d\n"
                                        "\t sReallocateSrc.uiSrcId = %d\n",
                                        lvl,csc,chp,pp,
                                        sSrcReq.bAllocate,
                                        sSrcReq.sReallocateSrc.uiSrcBlkId,
                                        sSrcReq.sReallocateSrc.uiSrcId));

                            bSrcAllocateRequest = true;
                            /* The request is to reallocate the already 
                               allocated SRC */
                            sSrcGrant.uiSrcBlkId = 
                                            sSrcReq.sReallocateSrc.uiSrcBlkId;
                            
                            sSrcGrant.uiSrcId = sSrcReq.sReallocateSrc.uiSrcId;

                            /* Update the usage in hRm */
                            hRm->sSrcUsage[sSrcReq.sReallocateSrc.uiSrcBlkId]
                              [sSrcReq.sReallocateSrc.uiSrcId].uiUsageCount++;

                            bSrcAllocated = true;

                        }/* if */
                        pResrcGrant->sSrcMixerGrnt[lvl].sSrcGrant[csc][chp][pp] = 
                                                                          sSrcGrant;
                        if((BRAP_RM_P_INVALID_INDEX != sSrcGrant.uiSrcBlkId)&&
                           (BRAP_RM_P_INVALID_INDEX != sSrcGrant.uiSrcId))
                        {
                            BDBG_MSG(("\t Reallocate pResrcGrant->sSrcMixerGrnt[%d].sSrcGrant[%d][%d][%d]\n"
                                "\t sSrcGrant.uiSrcBlkId = %d\n"
                                "\t sSrcGrant.uiSrcId = %d\n",
                                lvl,csc,chp,pp,sSrcGrant.uiSrcBlkId,sSrcGrant.uiSrcId));
                        }
                    }/* for chp */
                }/* for pp */
            }/* for csc */
        }/* for lvl */
    }

    if ((false == bSrcAllocated)&&(true == bSrcAllocateRequest))
    {
        BDBG_ERR(("BRAP_RM_P_AllocateSrc: Could not Allocate SRC, no Free SRC"));
        ret = BRAP_ERR_RESOURCE_EXHAUSTED;
    }
    
    BDBG_LEAVE(BRAP_RM_P_AllocateSrc);
    return ret;
}

/***************************************************************************
Summary:
    Private function that allocates Mixer for level 0. 

    Note: This private function will not do any malloc and pointer checks. 
    This is quite a low level helper function. Also, this function should 
    not init the pResrcGrant. It can just modify the respective allocated
    resource Id.
**************************************************************************/
static BERR_Code
BRAP_RM_P_AllocateMixerIpForLvl_0(
    BRAP_RM_Handle				hRm,		    /* [in] Resource Manager handle */
	const BRAP_RM_P_ResrcReq	*pResrcReq,	    /* [in] Required resources */
	BRAP_RM_P_MixerReq          *pMixReq,       /* [in] Mixer Request */
	unsigned int                uiDpBlock,      /* [in] DP Block Id */
	unsigned int                uiMixerId,      /* [in] Mixer Id */
	int                         iMixInChp,      /* [in] Channel pair for which 
	                                                    Mixer input is requested
	                                                    for allocation */
    BRAP_RM_P_MixerGrant        *pTempMixGrant /* [out] Mixer Grant */
    )
{
    BERR_Code ret=BERR_SUCCESS;
    unsigned int mix_ip=0;
    unsigned int uiNumMixIpAllocated =0;
    BRAP_OutputChannelPair eChP = BRAP_OutputChannelPair_eMax;

    BDBG_ENTER(BRAP_RM_P_AllocateMixerIpForLvl_0);
    /* Validate the inputs */
    BDBG_ASSERT(hRm);
    BDBG_ASSERT(pResrcReq);

    BDBG_MSG(("uiDpBlock = %d, uiMixerId = %d",uiDpBlock,uiMixerId));

    
    if ((BRAP_ChannelType_eDecode == pResrcReq->eChType)&&
        (BRAP_ChannelSubType_ePrimary == pResrcReq->eChSubType))
    {
        /* Since per mixer only one channel pair of primary can be a input */
        if(pMixReq->uiNumNewInput > BRAP_RM_P_MAX_PRIMARY_INPUT_PER_MIXER)
        {
            BDBG_ERR(("BRAP_RM_P_AllocateMixer:Invalid Mixer request"
                       "For primary, only one channel pair goes to mixer input"));
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }

        /* Fill the grant structure and update the usage info */
        pTempMixGrant->uiMixerInputId[iMixInChp]=BRAP_RM_P_MixIpPri[0];
        hRm->sMixerUsage[uiDpBlock][uiMixerId].uiInputUsgCnt[BRAP_RM_P_MixIpPri[0]]++;
    }
    else if ((BRAP_ChannelType_eDecode == pResrcReq->eChType)&&
              (BRAP_ChannelSubType_eSecondary == pResrcReq->eChSubType))
    {
        /* Since per mixer only three channel pairs of secondary can be a input */
        if(pMixReq->uiNumNewInput > BRAP_RM_P_MAX_SECONDARY_INPUT_PER_MIXER)
        {
            BDBG_ERR(("BRAP_RM_P_AllocateMixer:Invalid Mixer request"
                       "For Secondary, only three channel pairs are supported"));
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }

        /* Fill the grant structure and update the usage info */
        for(mix_ip=0;mix_ip<pMixReq->uiNumNewInput;mix_ip++)
        {
            pTempMixGrant->uiMixerInputId[mix_ip] = BRAP_RM_P_MixIpSec[mix_ip];
            /* Update the Usage */
            hRm->sMixerUsage[uiDpBlock][uiMixerId].uiInputUsgCnt[BRAP_RM_P_MixIpSec[mix_ip]]++;
        }
    }
    else if (((BRAP_ChannelType_eDecode == pResrcReq->eChType)&&
              (BRAP_ChannelSubType_eNone == pResrcReq->eChSubType))||
             (BRAP_ChannelType_ePcmCapture == pResrcReq->eChType))
    {
        /* Check for the maximum mixer inputs available */
        if(pMixReq->uiNumNewInput > BRAP_RM_P_MAX_MIXER_INPUTS)
        {
            BDBG_ERR(("BRAP_RM_P_AllocateMixer:Invalid Mixer request"
                       "For Secondary, only three channel pairs are supported"));
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }


        for(eChP=0; eChP < BRAP_OutputChannelPair_eMax; eChP++)
        {
            if(true == pMixReq->bInputChPair[eChP])
            {
                for(mix_ip=0;mix_ip<BRAP_RM_P_MAX_MIXER_INPUTS; mix_ip++)
                {
                    if(0 == hRm->sMixerUsage[uiDpBlock][uiMixerId].uiInputUsgCnt[BRAP_RM_P_MixIpPref[mix_ip]])
                    {
                        pTempMixGrant->uiMixerInputId[eChP] = BRAP_RM_P_MixIpPref[mix_ip];
                        /* Update the Usage */
                        hRm->sMixerUsage[uiDpBlock][uiMixerId].uiInputUsgCnt[BRAP_RM_P_MixIpPref[mix_ip]]++;
                        uiNumMixIpAllocated++;
                        break;
                    }
                }
            }
            if (uiNumMixIpAllocated == pMixReq->uiNumNewInput)
                break;
        }

    }
    else if (BRAP_ChannelType_ePcmPlayback == pResrcReq->eChType)
    {
        if(pMixReq->uiNumNewInput > BRAP_RM_P_MAX_PB_INPUT_PER_MIXER)
        {
            BDBG_ERR(("BRAP_RM_P_AllocateMixer:Invalid Mixer request"
                       "For Secondary, only three channel pairs are supported"));
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
            for(eChP=0; eChP < BRAP_OutputChannelPair_eMax; eChP++)
            {
                if(true == pMixReq->bInputChPair[eChP])
                {
                    for(mix_ip=0;mix_ip<BRAP_RM_P_MAX_PB_INPUT_PER_MIXER; mix_ip++)
                    {
                        if(0 == hRm->sMixerUsage[uiDpBlock][uiMixerId].uiInputUsgCnt[BRAP_RM_P_MixIpPb[mix_ip]])
                        {
                            pTempMixGrant->uiMixerInputId[eChP] = BRAP_RM_P_MixIpPb[mix_ip];
                            /* Update the Usage */
                            hRm->sMixerUsage[uiDpBlock][uiMixerId].uiInputUsgCnt[BRAP_RM_P_MixIpPb[mix_ip]]++;
                            uiNumMixIpAllocated++;
                            break;
                        }
                    }
                }
                if (uiNumMixIpAllocated == pMixReq->uiNumNewInput)
                    break;
            }
    }
    else
    {
        /* This condition should never occure */
        BDBG_ERR(("BRAP_RM_P_AllocateMixerForLvl_0 :Invalid channel type=%d",
                  "and Sub Channel type=%d",pResrcReq->eChType,pResrcReq->eChSubType));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
        
    }

    /* For debug */
    BDBG_MSG(("BRAP_RM_P_AllocateMixerIpForLvl_0:"));
    for(mix_ip=0; mix_ip < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; mix_ip++)
    {
        BDBG_MSG(("pTempMixGrant->uiMixerInputId[%d] = %d", 
            mix_ip, pTempMixGrant->uiMixerInputId[mix_ip]));
    }
    /* Debug ends */

    BDBG_LEAVE(BRAP_RM_P_AllocateMixerIpForLvl_0);
    return ret;
}

/***************************************************************************
Summary:
    Private function that is used while allocating Mixer. 
**************************************************************************/
static bool BRAP_RM_P_IsMixerFree(
    BRAP_RM_Handle				hRm,
    unsigned int                uiDpId,
    unsigned int                uiMixerId
    )
{
    bool bFreeMixer = true;
    unsigned int i = 0;
    
    for(i=0;i<BRAP_RM_P_MAX_MIXER_INPUTS;i++)
    {
        if(0!= hRm->sMixerUsage[uiDpId][uiMixerId].uiInputUsgCnt[i])
        {
            bFreeMixer = false;
            break;
        }
        else
        {
            bFreeMixer = true;
        }
    }
    return bFreeMixer;
}

/***************************************************************************
Summary:
    Private function that allocates Mixer. 

    Note: This private function will not do any malloc and pointer checks. 
    This is quite a low level helper function. Also, this function should 
    not init the pResrcGrant. It can just modify the respective allocated
    resource Id.
**************************************************************************/
static BERR_Code 
BRAP_RM_P_AllocateMixer(
	BRAP_RM_Handle				hRm,		/* [in] Resource Manager handle */
	const BRAP_RM_P_ResrcReq	*pResrcReq,	/* [in] Required resources */
	BRAP_RM_P_ResrcGrant		*pResrcGrant/* [out] Indices of the allocated 
	                                           resources */
    )
{
    BERR_Code               ret = BERR_SUCCESS;
    int                     lvl=0,chp=0,pp=0;
    unsigned int            dp_blk=0,mix_id=0,mix_ip=0,mix_op=0;
    unsigned int            i = 0, cnt =0;
    BRAP_RM_P_MixerReq      sMixReq;
    BRAP_RM_P_MixerGrant    sTempMixGrant;
    bool                    bMixAllocated = false;
    bool                    bNewMixAllocateRequest =false;
    bool                    bOldMixAllocateRequest =false;
    bool                    bMixersAvailable = false;
    unsigned int            uiNumIO = 0, uiNumMixIpRemain = 0;
    unsigned int            uiNumMixer[BRAP_RM_P_MAX_MIXING_LEVELS]
                                      [BRAP_RM_P_MAX_PARALLEL_PATHS];
    BRAP_OutputChannelPair  eInChp = BRAP_OutputChannelPair_eMax;

    BDBG_ENTER(BRAP_RM_P_AllocateMixer);
    /* Validate the inputs */
    BDBG_ASSERT(hRm);
    BDBG_ASSERT(pResrcReq);
    BDBG_ASSERT(pResrcGrant);

    for (lvl=0;lvl<BRAP_RM_P_MAX_MIXING_LEVELS;lvl++)
    {
        for(pp=0;pp<BRAP_RM_P_MAX_PARALLEL_PATHS;pp++)
        {
            uiNumMixer[lvl][pp]=0;
        }
    }

    /* Trace the pResrcReq for allocating the Mixer. We need to first allocate 
       the Mixers for all the channel pairs in one parallel path in one level 
       and then go to next parallel path. This is required because in a parallel
       path, all the Mixers for different channel pairs should be contiguous */

    BDBG_MSG(("AllocateMixer: New Mixer Request"));
    /* Find the number of new contiguous mixers required */
    for (lvl=0;lvl<BRAP_RM_P_MAX_MIXING_LEVELS;lvl++)
    {
        for(pp=0;pp<BRAP_RM_P_MAX_PARALLEL_PATHS;pp++)
        {
            for(chp=0;chp<BRAP_RM_P_MAX_OP_CHANNEL_PAIRS;chp++)
            {
                sMixReq = pResrcReq->sSrcMixerReq[lvl].sMixerReq[chp][pp];

                /* Check if new Mixer is requested */
                if ((true == sMixReq.bAllocate)&&
                    (BRAP_RM_P_INVALID_INDEX==sMixReq.sReallocateMixer.uiDpId)&&
                    (BRAP_RM_P_INVALID_INDEX==sMixReq.sReallocateMixer.uiMixerId))
                {
                    bNewMixAllocateRequest = true;
                    uiNumMixer[lvl][pp]++;
                    BDBG_MSG(("New Mixer Request: ResrcReq->sSrcMixerReq[%d].sMixerReq[%d][%d]\n "
                                "\t sMixReq.bAllocate= %d \n"
                                "\t sMixReq.uiNumNewInput =%d \n"
                                "\t sMixReq.uiNumNewOutput = %d ",
                                lvl,chp,pp,sMixReq.bAllocate,
                                sMixReq.uiNumNewInput,sMixReq.uiNumNewOutput));
                }
            }
        }
    }
    if (true == bNewMixAllocateRequest)
    {
        /* Allocate new mixers */
        for (lvl=0;lvl<BRAP_RM_P_MAX_MIXING_LEVELS;lvl++)
        {
            for(pp=0;pp<BRAP_RM_P_MAX_PARALLEL_PATHS;pp++)
            {
                if(0<uiNumMixer[lvl][pp])
                {
                    if((BRAP_P_UsgPath_eDecodePcm == pResrcReq->ePath)
#if ( (BRAP_3548_FAMILY == 1) || (BRAP_7405_FAMILY == 1) )
                       || (BRAP_P_UsgPath_eDecodeCompress == pResrcReq->ePath)
                       || (BRAP_P_UsgPath_ePPBranch == pResrcReq->ePath)
                       || (BRAP_P_UsgPath_eDownmixedPath== pResrcReq->ePath)                           
                       || (BRAP_P_UsgPath_eDownmixedMixPath== pResrcReq->ePath)                         
                       || (BRAP_P_UsgPath_eMixPath == pResrcReq->ePath)                       
                       || (BRAP_P_UsgPath_eSharedPP== pResrcReq->ePath)                                              
                       || (BRAP_P_UsgPath_eDecodePcmPostMixing== pResrcReq->ePath)
                       || (BRAP_P_UsgPath_ePPBranchPostMixing== pResrcReq->ePath)                       
                       || (BRAP_P_UsgPath_eDecodeCompressPostMixing== pResrcReq->ePath)
#endif                       
						)

                    {
                        dp_blk = 0;
                    }
                    else
                    {
                        BDBG_ERR(("Unsupported Path %d",pResrcReq->ePath));
                        return BERR_TRACE(BERR_NOT_INITIALIZED);
                    }
                        for(mix_id=0;mix_id<BRAP_RM_P_MAX_MIXER_PER_DP_BLCK;mix_id++)
                        {
                            /* Check if required contiguous mixers are available */
                            for(i=0;i<uiNumMixer[lvl][pp];i++)
                            {
                                if((false == BRAP_RM_P_IsMixerFree(hRm, 
                                                         dp_blk, mix_id+i))||
                                   (BRAP_RM_P_MAX_MIXER_PER_DP_BLCK <= (mix_id+i)))
                                {
                                    /* One of the contiguous mixers is not free.
                                       go to the next mixer id and start again */
                                    break;
                                }
                            }

                            /* Break if required no of mixers found */
                            if(i == uiNumMixer[lvl][pp])
                            {
                                /* All free contiguous mixers found */
                                bMixersAvailable = true;
                                break;
                            }
                        }/* For mix_id */
                    BDBG_MSG(("BRAP_RM_P_AllocateMixer: DP=%d and Mix_id=%d",
                                dp_blk,mix_id));
                    
                    /* Return error if no continuous mixers found */
                    if(false == bMixersAvailable)
                    {
                        BDBG_ERR(("BRAP_RM_P_AllocateMixer: Requested no. of Mixers not free"));
                        return BERR_TRACE(BRAP_ERR_RESOURCE_EXHAUSTED);
                    }
                    else
                    {
                        for(chp=0,i=0;
                            (chp<BRAP_RM_P_MAX_OP_CHANNEL_PAIRS)&&
                            (i<uiNumMixer[lvl][pp]);chp++)
                        {
                            sMixReq = pResrcReq->sSrcMixerReq[lvl].sMixerReq[chp][pp];
                            if(true == sMixReq.bAllocate)
                            {
                                sTempMixGrant =
                                    pResrcGrant->sSrcMixerGrnt[lvl].sMixerGrant[chp][pp];
                                /* Return the ID's in grant structure */
                                sTempMixGrant.uiDpId = dp_blk;
                                sTempMixGrant.uiMixerId = (mix_id+i);

                                /* For level 0, where first mixing happens, Due to H/W 
                                   limitation, we fix inputs 0-2 of mixer for Secondary,
                                   Input 3 for channel pair from primary and inputs 4-7 
                                   for sound effects */
                                if(0 == lvl)
                                {
                                    ret = BRAP_RM_P_AllocateMixerIpForLvl_0(hRm,
                                                                      pResrcReq,
                                                                      &sMixReq,
                                                                      dp_blk,
                                                                      mix_id+i,
                                                                      chp,
                                                                      &sTempMixGrant);
                                    if (BERR_SUCCESS != ret)
                                    {
                                        BDBG_ERR(("BRAP_RM_P_AllocateMixerIpForLvl_0"
                                                  "returned Error."));
                                        return BERR_TRACE(BERR_NOT_SUPPORTED);
                                    }
                               
                                }
                                else
                                {
                                    /* Validate the Input range */
                                    if(BRAP_RM_P_MAX_MIXER_INPUTS<sMixReq.uiNumNewInput)
                                    {
                                        BDBG_ERR(("BRAP_RM_P_AllocateMixer:No. of Mixer"
                                                   "Input = %d not valid",sMixReq.uiNumNewInput));
                                        return BERR_TRACE(BERR_NOT_SUPPORTED);
                                    }
                                    /* Get the Input ids */
                                    for(eInChp=0,mix_ip=0;eInChp<BRAP_OutputChannelPair_eMax;eInChp++)
                                    {
                                        if(false == sMixReq.bInputChPair[eInChp])
                                        {
                                            continue;
                                        }
                                        sTempMixGrant.uiMixerInputId[eInChp]= mix_ip;
                                        /* Update the Usage */
                                        hRm->sMixerUsage[dp_blk][mix_id+i].uiInputUsgCnt[mix_ip]++;
                                        /* Increment mix_ip */
                                        mix_ip++;
                                    }
                                }

                                /* Validate the Output range */
                                if(BRAP_RM_P_MAX_MIXER_OUTPUTS < sMixReq.uiNumNewOutput)
                                {
                                    BDBG_ERR(("BRAP_RM_P_AllocateMixer:No of output %d "
                                              "out of range",sMixReq.uiNumNewOutput));
                                    return BERR_TRACE(BERR_NOT_SUPPORTED);
                                }
                                
                                for (mix_op=0;mix_op<sMixReq.uiNumNewOutput;mix_op++)
                                {
                                    if(BRAP_RM_P_INVALID_INDEX == sMixReq.uiMixerOutputId[mix_op])
                                    {
                                        sTempMixGrant.uiMixerOutputId[mix_op] = mix_op;
                                        /* Update the Usage */
                                        hRm->sMixerUsage[dp_blk][mix_id+i].uiOutputUsgCnt[mix_op]++;
                                    }
                                    else
                                    {
                                        sTempMixGrant.uiMixerOutputId[mix_op] = 
                                                sMixReq.uiMixerOutputId[mix_op];
                                        /* Update the Usage */
                                        hRm->sMixerUsage[dp_blk][mix_id+i].
                                        uiOutputUsgCnt[sMixReq.uiMixerOutputId[mix_op]]++;
                                    }
                                }
                                
                                bMixAllocated = true;
                                i++;
                                /* Store the gran structure back */
                                pResrcGrant->sSrcMixerGrnt[lvl].sMixerGrant[chp][pp] =
                                                                   sTempMixGrant;
                                if((BRAP_RM_P_INVALID_INDEX != sTempMixGrant.uiDpId)&&
                                   (BRAP_RM_P_INVALID_INDEX != sTempMixGrant.uiMixerId))
                                {
                                    /* For debugging */
                                    BDBG_MSG(("pResrcGrant->sSrcMixerGrnt[%d].sMixerGrant[%d][%d]\n"
                                              "\t sTempMixGrant.uiDpId = %d \n"
                                              "\t sTempMixGrant.uiMixerId = %d",
                                              lvl,chp,pp,sTempMixGrant.uiDpId,
                                              sTempMixGrant.uiMixerId));
                                    for(cnt=0; cnt<BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; cnt++)
                                    {
                                        if(sTempMixGrant.uiMixerInputId[cnt] != BRAP_INVALID_VALUE)
                                        {
                                            BDBG_MSG(("sTempMixGrant.uiMixerInputId[%d] = %d",
                                                cnt, sTempMixGrant.uiMixerInputId[cnt]));
                                        }
                                    }
                                    for(cnt=0; cnt<BRAP_RM_P_MAX_MIXER_OUTPUTS; cnt++)
                                    {
                                        if(sTempMixGrant.uiMixerOutputId[cnt] != BRAP_INVALID_VALUE)
                                        {
                                            BDBG_MSG(("sTempMixGrant.uiMixerOutputId[%d] = %d",
                                                cnt, sTempMixGrant.uiMixerOutputId[cnt]));
                                        }
                                    }
                                    /* Debugging ends */
                                }
                            }                            
                        }/* for chp */
                    }/* else */
                }/* if */
            }/* for PP */
        }/* for lvl */
    }/* if */
    else
    {
        /* Reallocate mixers here if any */
        for (lvl=0;lvl<BRAP_RM_P_MAX_MIXING_LEVELS;lvl++)
        {
            for(pp=0;pp<BRAP_RM_P_MAX_PARALLEL_PATHS;pp++)
            {
                for(chp=0;chp<BRAP_RM_P_MAX_OP_CHANNEL_PAIRS;chp++)
                {
                    sMixReq = pResrcReq->sSrcMixerReq[lvl].sMixerReq[chp][pp];
                    sTempMixGrant =
                        pResrcGrant->sSrcMixerGrnt[lvl].sMixerGrant[chp][pp];
                    

                    /* Check if New Mixer is requested or request is to 
                       reallocate an already allocated Mixer */
                    if ((true == sMixReq.bAllocate)&&
                        (BRAP_RM_P_INVALID_INDEX!=sMixReq.sReallocateMixer.uiDpId)&&
                        (BRAP_RM_P_INVALID_INDEX!=sMixReq.sReallocateMixer.uiMixerId))
                    {
                        BDBG_MSG(("Reallocate Mixers:ResrcReq->sSrcMixerReq[%d].sMixerReq[%d][%d]\n"
                                    "\t sMixReq.bAllocate= %d \n"
                                    "\t sMixReq.uiNumNewInput =%d \n"
                                    "\t sMixReq.uiNumNewOutput = %d",
                                    lvl,chp,pp,sMixReq.bAllocate,
                                    sMixReq.uiNumNewInput,sMixReq.uiNumNewOutput));
                    
                        bOldMixAllocateRequest = true;
                        /* Validate DP Id & Mixer Id */
                        if((BRAP_RM_P_MAX_DP_BLCK<sMixReq.sReallocateMixer.uiDpId)||
                           (BRAP_RM_P_MAX_MIXER_PER_DP_BLCK < 
                                                sMixReq.sReallocateMixer.uiMixerId))
                        {
                            BDBG_ERR(("BRAP_RM_P_AllocateMixer: DP ID =%d and "
                                      "Mixer ID =%d Out of range",
                                      sMixReq.sReallocateMixer.uiDpId,
                                      sMixReq.sReallocateMixer.uiMixerId));
                            return  BERR_TRACE(BERR_NOT_SUPPORTED);
                        }
                        
                        /* The request is to reallocate the already 
                           allocated mixer */
                        /* Return the ID's in grant structure */
                        sTempMixGrant.uiDpId = sMixReq.sReallocateMixer.uiDpId;
                        sTempMixGrant.uiMixerId =sMixReq.sReallocateMixer.uiMixerId;

                        /* For Mixer Input */
                        if (BRAP_RM_P_INVALID_INDEX == sMixReq.uiNumNewInput)
                        {
                            /* If request is to reallocate old inputs of already 
                               allocated mixer */
                            for (mix_ip=0;
                                 mix_ip<BRAP_RM_P_MAX_OP_CHANNEL_PAIRS;
                                 mix_ip++)
                            {
                                /* Validate each input and allocate */
                                if(BRAP_RM_P_MAX_MIXER_INPUTS > 
                                    sMixReq.sReallocateMixer.uiMixerInputId[mix_ip])
                                {
                                    sTempMixGrant.uiMixerInputId[mix_ip]=
                                    sMixReq.sReallocateMixer.uiMixerInputId[mix_ip];

                                    /* Update the Usage */
                                    hRm->sMixerUsage[sTempMixGrant.uiDpId]
                                           [sTempMixGrant.uiMixerId].uiInputUsgCnt
                                           [sTempMixGrant.uiMixerInputId[mix_ip]]++;
                                }
                            }
                        }
                        else
                        {
                            /* If the request is to allocate new inputs of already 
                               allocated mixer */
                            if(0 == lvl)
                            {
                                ret = BRAP_RM_P_AllocateMixerIpForLvl_0(hRm,
                                                                  pResrcReq,
                                                                  &sMixReq,
                                                                  sTempMixGrant.uiDpId,
                                                                  sTempMixGrant.uiMixerId ,
                                                                  chp,
                                                                  &sTempMixGrant);
                                if (BERR_SUCCESS != ret)
                                {
                                    BDBG_ERR(("BRAP_RM_P_AllocateMixerIpForLvl_0"
                                              "returned Error."));
                                    return BERR_TRACE(BERR_NOT_SUPPORTED);
                                }
                           
                            }
                            else
                            {
                                /* Validate the Input range */
                                if(BRAP_RM_P_MAX_MIXER_INPUTS<sMixReq.uiNumNewInput)
                                {
                                    BDBG_ERR(("BRAP_RM_P_AllocateMixer:No. of Mixer"
                                               "Input = %d not valid",
                                               sMixReq.uiNumNewInput));
                                    return BERR_TRACE(BERR_NOT_SUPPORTED);
                                }
                                /* Find the number of free inputs */
                                for (mix_ip=0,uiNumIO=0;mix_ip<BRAP_RM_P_MAX_MIXER_INPUTS;mix_ip++)
                                {
                                    if (0 == hRm->sMixerUsage[sTempMixGrant.uiDpId]
                                        [sTempMixGrant.uiMixerId].uiInputUsgCnt[mix_ip])
                                    {
                                        uiNumIO++;
                                    }
                                }
                                /* This implementation is for 7.1 input max, so 
                                   sMixReq.uiNumNewInput must never cross 4 */
                                if(sMixReq.uiNumNewInput > BRAP_RM_P_MAX_OP_CHANNEL_PAIRS)
                                {
                                    BDBG_ERR(("BRAP_RM_P_AllocateMixer: No. of" 
                                                "Free Inputs invalid!!"));
                                    return BERR_TRACE(BERR_NOT_SUPPORTED);
                                }
                                /* If requested Inputs available, Allocate them and
                                   update the status */
                                if(uiNumIO >= sMixReq.uiNumNewInput)
                                {
                                    uiNumMixIpRemain = uiNumIO - sMixReq.uiNumNewInput;
                                    for (mix_ip=0;
                                         (mix_ip < BRAP_RM_P_MAX_MIXER_INPUTS) && 
                                         (uiNumIO > uiNumMixIpRemain);
                                         mix_ip++)
                                    {
                                        if (0 == hRm->sMixerUsage[sTempMixGrant.uiDpId]
                                            [sTempMixGrant.uiMixerId].uiInputUsgCnt[mix_ip])
                                        {
                                            sTempMixGrant.uiMixerInputId[mix_ip]= 
                                                                        mix_ip;
                                            
                                            hRm->sMixerUsage[sTempMixGrant.uiDpId]
                                            [sTempMixGrant.uiMixerId].uiInputUsgCnt
                                            [mix_ip]++;
                                            
                                            uiNumIO--;
                                        }
                                    }
                                }
                                else
                                {
                                    BDBG_ERR(("BRAP_RM_P_AllocateMixer:Requested "
                                              "Number of Inputs not available"));
                                    return BERR_TRACE(BRAP_ERR_RESOURCE_EXHAUSTED);
                                }
                            }
                        }

                        /* Mixer output */
                        if (BRAP_RM_P_INVALID_INDEX == sMixReq.uiNumNewOutput)
                        {
                            /* If request is to reallocate old outputs of already 
                               allocated mixer */
                            for(mix_op=0;mix_op<BRAP_RM_P_MAX_MIXER_OUTPUTS;mix_op++)
                            {
                                if(BRAP_RM_P_INVALID_INDEX != 
                                    sMixReq.sReallocateMixer.uiMixerOutputId[mix_op])
                                {
                                    sTempMixGrant.uiMixerOutputId[mix_op]=
                                        sMixReq.sReallocateMixer.uiMixerOutputId[mix_op];
                                    /* Update the Usage */
                                    hRm->sMixerUsage[sTempMixGrant.uiDpId]
                                              [sTempMixGrant.uiMixerId].uiOutputUsgCnt
                                              [sTempMixGrant.uiMixerOutputId[mix_op]]++;
                                }
                            }/* For mix_op */                            
                        }
                        else
                        {
                            
                            /* If the request is to allocate new outputs of already 
                               allocated mixer. Since currently a mixer has only 2
                               outputs and the current request is to reallocate the
                               mixer. This implies that laready atleast one output
                               of the mixer is already allocated, so the new request
                               can not be more that 1 */
                            if (1 != sMixReq.uiNumNewOutput)
                            {
                                BDBG_ERR(("BRAP_RM_P_AllocateMixer:No of output %d"
                                           " out of range",sMixReq.uiNumNewOutput));
                                return BERR_TRACE(BERR_NOT_SUPPORTED);
                            }
                            /* Find the free output and allocate */
                            uiNumIO=0;
                            for (mix_op=0;mix_op<BRAP_RM_P_MAX_MIXER_OUTPUTS;mix_op++)
                            {
                                if (0 == hRm->sMixerUsage[sTempMixGrant.uiDpId]
                                    [sTempMixGrant.uiMixerId].uiOutputUsgCnt[mix_op])
                                {
                                    uiNumIO++;
                                }
                            }
                            /* If requested Outputs available, Allocate them and
                               update the status */
                            if(uiNumIO >= sMixReq.uiNumNewOutput)
                            {
                                for (mix_op=0;
                                     (mix_op < BRAP_RM_P_MAX_MIXER_OUTPUTS) && 
                                     (uiNumIO > 0);
                                     mix_op++)
                                {
                                    if (0 == hRm->sMixerUsage[sTempMixGrant.uiDpId]
                                        [sTempMixGrant.uiMixerId].uiOutputUsgCnt[mix_op])
                                    {
                                        sTempMixGrant.uiMixerInputId[mix_op]=mix_op;
                                        
                                        hRm->sMixerUsage[sTempMixGrant.uiDpId]
                                        [sTempMixGrant.uiMixerId].uiOutputUsgCnt[mix_op]++;
                                        
                                        uiNumIO--;
                                    }
                                }
                            }
                            else
                            {
                                BDBG_ERR(("BRAP_RM_P_AllocateMixer:Requested "
                                          "Number of Inputs not available"));
                                return BERR_TRACE(BRAP_ERR_RESOURCE_EXHAUSTED);
                            }
                        }
                        bMixAllocated = true;
                    }/* else */
                    pResrcGrant->sSrcMixerGrnt[lvl].sMixerGrant[chp][pp] =
                                                                    sTempMixGrant;
                    if((BRAP_RM_P_INVALID_INDEX != sTempMixGrant.uiDpId)&&
                       (BRAP_RM_P_INVALID_INDEX != sTempMixGrant.uiMixerId))
                    {
                        BDBG_MSG(("pResrcGrant->sSrcMixerGrnt[%d].sMixerGrant[%d][%d]\n"
                                              "\t sTempMixGrant.uiDpId = %d \n"
                                              "\t sTempMixGrant.uiMixerId = %d ",
                                              lvl,chp,pp,sTempMixGrant.uiDpId,
                                              sTempMixGrant.uiMixerId));
                        for(cnt=0; cnt<BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; cnt++)
                        {
                            if(sTempMixGrant.uiMixerInputId[cnt] != BRAP_INVALID_VALUE)
                            {
                                BDBG_MSG(("sTempMixGrant.uiMixerInputId[%d] = %d",
                                    cnt, sTempMixGrant.uiMixerInputId[cnt]));
                            }
                        }
                        for(cnt=0; cnt<BRAP_RM_P_MAX_MIXER_OUTPUTS; cnt++)
                        {
                            if(sTempMixGrant.uiMixerOutputId[cnt] != BRAP_INVALID_VALUE)
                            {
                                BDBG_MSG(("sTempMixGrant.uiMixerOutputId[%d] = %d",
                                    cnt, sTempMixGrant.uiMixerOutputId[cnt]));
                            }
                        }
                    }
                }/* for chp */
            }/* for pp */
        }/* for lvl */
    }

    if (((true == bNewMixAllocateRequest)||(true == bOldMixAllocateRequest))&& 
        (false == bMixAllocated))
    {
        BDBG_ERR(("BRAP_RM_P_AllocateMixer:Could not allocate Mixers"));
        return BERR_TRACE(BRAP_ERR_RESOURCE_EXHAUSTED);
    }
        
    BDBG_LEAVE(BRAP_RM_P_AllocateMixer);
    return ret;
}

static BERR_Code BRAP_RM_P_AllocateSrcEq(
	BRAP_RM_Handle				hRm,		/* [in] Resource Manager handle */
	const BRAP_RM_P_ResrcReq	*pResrcReq,	/* [in] Required resources */
	BRAP_RM_P_ResrcGrant		*pResrcGrant/* [out] Indices of the allocated 
	                                           resources */
    )
{
    BERR_Code           ret = BERR_SUCCESS;
    int                 csc=0,chp=0,pp=0;
    int                 src_blk=0,src_id=0;
    unsigned int        i=0, j=0;
    BRAP_RM_P_SrcEqReq   sSrcEqReq;
    BRAP_RM_P_SrcEqGrant sSrcEqGrant;
    bool                bSrcEqAllocated = false;
    bool                bSrcEqAllocateRequest =false;
    bool                bNewSrcEqAllocateRequest =false;
    bool                bOldSrcEqAllocateRequest =false;
    bool                bSrcsAvailable = false;
    unsigned int        uiNumSrc[BRAP_RM_P_MAX_PARALLEL_PATHS]
                                [BRAP_RM_P_MAX_SRC_IN_CASCADE];
    
    BDBG_ENTER(BRAP_RM_P_AllocateSrcEq);

    /* Validate the inputs */
    BDBG_ASSERT(hRm);
    BDBG_ASSERT(pResrcReq);
    BDBG_ASSERT(pResrcGrant);

    for(pp=0;pp<BRAP_RM_P_MAX_PARALLEL_PATHS;pp++)
    {
        for(csc=0; csc<BRAP_RM_P_MAX_SRC_IN_CASCADE; csc++)
        {
            uiNumSrc[pp][csc]=0;
        }
    }

    /* Trace the pResrcReq for allocating the SRC. We need to first allocate all
       the SRC for all the channel pairs in one level. */

    BDBG_MSG(("AllocateSrcEq: Fresh SRC-EQ Requests"));
    /* Find out the number of continuous new SRCs required for pp */
    for(pp=0;pp<BRAP_RM_P_MAX_PARALLEL_PATHS;pp++)
    {
        for(chp=0;chp<BRAP_RM_P_MAX_OP_CHANNEL_PAIRS;chp++)
        {
            for(csc=0;csc<BRAP_RM_P_MAX_SRC_IN_CASCADE;csc++)
            {                
                sSrcEqReq = pResrcReq->sSrcEqReq[csc][chp][pp];
                if ((true == sSrcEqReq.bAllocate)&&
                    (BRAP_RM_P_INVALID_INDEX==sSrcEqReq.sReallocateSrcEq.uiSrcBlkId) &&
                    (BRAP_RM_P_INVALID_INDEX==sSrcEqReq.sReallocateSrcEq.uiSrcId))
                {                        
                    uiNumSrc[pp][csc]++;
                    bNewSrcEqAllocateRequest = true;

                    BDBG_MSG(("\t pResrcReq->sSrcEqReq[%d][%d][%d]\n"
                        "\t sSrcEqReq: bAllocate = %d\n",
                        csc,chp,pp,
                        sSrcEqReq.bAllocate));
                }
                else if ((true == sSrcEqReq.bAllocate)&&
                    (BRAP_RM_P_INVALID_INDEX!=sSrcEqReq.sReallocateSrcEq.uiSrcBlkId) &&
                    (BRAP_RM_P_INVALID_INDEX!=sSrcEqReq.sReallocateSrcEq.uiSrcId))
                {                        
                    bOldSrcEqAllocateRequest = true;

                    BDBG_MSG(("\t pResrcReq->sSrcEqReq[%d][%d][%d]\n"
                        "\t sSrcEqReq: bAllocate = %d\n"
                        "\t sSrcEqReq.sReallocateSrcEq.uiSrcBlkId = %d\n"
                        "\t SrcEqReq.sReallocateSrcEq.uiSrcId =%d\n",
                        csc,chp,pp,
                        sSrcEqReq.bAllocate,sSrcEqReq.sReallocateSrcEq.uiSrcBlkId,
                        sSrcEqReq.sReallocateSrcEq.uiSrcId));
                }
            }
        }
    }

    if(true == bNewSrcEqAllocateRequest)
    {
        BDBG_MSG(("AllocateSrcEq: Fresh SRC EQ Grants"));
        
        for(pp=0;pp<BRAP_RM_P_MAX_PARALLEL_PATHS;pp++)
        {
            for(csc=0;csc<BRAP_RM_P_MAX_SRC_IN_CASCADE;csc++)
            {
                for(chp=0,j=0; chp < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; chp++,j++)
                {
                    bSrcsAvailable = false;
                    
                    if((0 < uiNumSrc[pp][csc]) && 
                       (true == pResrcReq->sSrcEqReq[csc][chp][pp].bAllocate))
                    {
                        bSrcEqAllocateRequest = true;
                        
                        if(BRAP_RM_P_INVALID_INDEX != pResrcReq->sSrcEqReq[csc][chp][pp].uiBlkId)
                        {
                            src_blk = pResrcReq->sSrcEqReq[csc][chp][pp].uiBlkId;
                        }
                        else
                        {
                            BDBG_ERR(("BRAP_RM_P_AllocateSrcEq: Invalid SRC Block Id passed"));
                            ret = BERR_INVALID_PARAMETER;
                            goto err;            
                    }
                    
                    for(src_id=0; src_id<BRAP_RM_P_MAX_SRC_PER_SRC_BLCK; src_id++)
                    {
                        BDBG_MSG(("SRCEQ_Alloc: uiUsgCnt of SRC[%d][%d] =%d,uiNumSrc[%d][%d]=%d",
                            src_blk,src_id,hRm->sSrcUsage[src_blk][src_id].uiUsageCount,
                            pp,csc,uiNumSrc[pp][csc]));

                        /* Find a free SRC */
                        if (0 == hRm->sSrcUsage[src_blk][src_id].uiUsageCount)
                        {
                            bSrcsAvailable = true;
                            BDBG_MSG((" Available src_blk=%d src_id=%d",src_blk,src_id));
                                
                            for(i=1;i<uiNumSrc[pp][csc];i++)
                            {
                                if ((0 == hRm->sSrcUsage[src_blk][src_id+i].uiUsageCount)&&
                                    (BRAP_RM_P_MAX_SRC_PER_SRC_BLCK > (src_id+i)))
                                {
                                        BDBG_MSG(("Available subsequent cascade SRC, src_blk=%d src_id=%d",
                                                    src_blk,src_id+i));
                                    bSrcsAvailable = true;
                                }
                                else
                                {
                                    BDBG_MSG((" NOT available src_blk=%d src_id=%d",src_blk,src_id+i));
                                    /* this src_id doesn't have required continuous SRCs free */
                                    bSrcsAvailable = false;
                                    break;
                                }
                            }/* for i */
                        }
                        if(true == bSrcsAvailable)
                            break;
                    } /* for src_id */
                    
                    if(false == bSrcsAvailable)
                    {
                        BDBG_ERR(("BRAP_RM_P_AllocateSrcEq: Could not Allocate SRC, no Free SRC"));
                            ret = BERR_TRACE(BRAP_ERR_RESOURCE_EXHAUSTED);
                            goto err;                                
                        }

                        sSrcEqGrant = pResrcGrant->sSrcEqGrant[csc][chp][pp];
                                
                                /* Return the ID's in grant structure */
                                sSrcEqGrant.uiSrcBlkId = src_blk;
                        sSrcEqGrant.uiSrcId = src_id + j;
                                /* Update the Usage */
                        hRm->sSrcUsage[src_blk][src_id+j].bSrcEq = true;
                        hRm->sSrcUsage[src_blk][src_id+j].uiUsageCount++;

                        pResrcGrant->sSrcEqGrant[csc][chp][pp] = sSrcEqGrant;
                        bSrcEqAllocated = true;
                        
                        if((BRAP_RM_P_INVALID_INDEX != sSrcEqGrant.uiSrcBlkId)&&
                           (BRAP_RM_P_INVALID_INDEX != sSrcEqGrant.uiSrcId))
                        {
                            BDBG_MSG(("\t pResrcGrant->sSrcEqGrant[%d][%d][%d]\n"
                                "\t sSrcEqGrant.uiSrcBlkId = %d \n\t sSrcEqGrant.uiSrcId = %d\n",
                                csc,chp,pp,sSrcEqGrant.uiSrcBlkId,sSrcEqGrant.uiSrcId));
                        }
                    } /* if uiNumSrc */
                } /* for chp */
            }/* for csc */
        }/* for pp */
    }/* if */

    if(true == bOldSrcEqAllocateRequest)
    {
        BDBG_MSG(("AllocateSrc: Reallocate SRC Requests & Grants"));
        for(pp=0;pp<BRAP_RM_P_MAX_PARALLEL_PATHS;pp++)
        {
            for(csc=0;csc<BRAP_RM_P_MAX_SRC_IN_CASCADE;csc++)
            {
                for(chp=0;chp<BRAP_RM_P_MAX_OP_CHANNEL_PAIRS;chp++)
                {
                    sSrcEqReq = pResrcReq->sSrcEqReq[csc][chp][pp];
                    sSrcEqGrant = pResrcGrant->sSrcEqGrant[csc][chp][pp];
                    
                    /* Check if request is to reallocate an already 
                       allocated SRC */
                    if ((true == sSrcEqReq.bAllocate)&&
                        (BRAP_RM_P_INVALID_INDEX != sSrcEqReq.sReallocateSrcEq.uiSrcBlkId) &&
                        (BRAP_RM_P_INVALID_INDEX != sSrcEqReq.sReallocateSrcEq.uiSrcId))
                    {
                        BDBG_MSG(("\t pResrcReq->sSrcEqReq[%d][%d][%d]\n"
                                    "\t sSrcEqReq: bAllocate = %d\n"
                                    "\t sReallocateSrcEq.uiSrcBlkId = %d\n"
                                    "\t sReallocateSrcEq.uiSrcId = %d\n",
                                    csc,chp,pp,
                                    sSrcEqReq.bAllocate,
                                    sSrcEqReq.sReallocateSrcEq.uiSrcBlkId,
                                    sSrcEqReq.sReallocateSrcEq.uiSrcId));

                        bSrcEqAllocateRequest = true;
                        /* The request is to reallocate the already 
                           allocated SRC */
                        sSrcEqGrant.uiSrcBlkId = 
                                        sSrcEqReq.sReallocateSrcEq.uiSrcBlkId;
                        
                        sSrcEqGrant.uiSrcId = sSrcEqReq.sReallocateSrcEq.uiSrcId;

                        /* Update the usage in hRm */
                        hRm->sSrcUsage[sSrcEqReq.sReallocateSrcEq.uiSrcBlkId]
                          [sSrcEqReq.sReallocateSrcEq.uiSrcId].bSrcEq = true;
                        hRm->sSrcUsage[sSrcEqReq.sReallocateSrcEq.uiSrcBlkId]
                          [sSrcEqReq.sReallocateSrcEq.uiSrcId].uiUsageCount++;

                        bSrcEqAllocated = true;

                    }/* if */
                    pResrcGrant->sSrcEqGrant[csc][chp][pp] = sSrcEqGrant;
                    if((BRAP_RM_P_INVALID_INDEX != sSrcEqGrant.uiSrcBlkId)&&
                       (BRAP_RM_P_INVALID_INDEX != sSrcEqGrant.uiSrcId))
                    {
                        BDBG_MSG(("\t Reallocate pResrcGrant->sSrcEqGrant[%d][%d][%d]\n"
                            "\t sSrcEqGrant.uiSrcBlkId = %d\n"
                            "\t sSrcEqGrant.uiSrcId = %d\n",
                            csc,chp,pp,sSrcEqGrant.uiSrcBlkId,sSrcEqGrant.uiSrcId));
                    }
                }/* for chp */
            }/* for csc */
        }/* for pp */
    }

    if ((false == bSrcEqAllocated)&&(true == bSrcEqAllocateRequest))
    {
        BDBG_ERR(("BRAP_RM_P_AllocateSrc: Could not Allocate SRC, no Free SRC"));
        ret = BRAP_ERR_RESOURCE_EXHAUSTED;
    }
    
    BDBG_LEAVE(BRAP_RM_P_AllocateSrc);

err:
    return ret;
}

/***************************************************************************
Summary:
    Private function that allocates Output Port. 

    Note: This private function will not do any malloc and pointer checks. 
    This is quite a low level helper function. Also, this function should 
    not init the pResrcGrant. It can just modify the respective allocated
    resource Id.
**************************************************************************/
static BERR_Code 
BRAP_RM_P_AllocateOpPort(
	BRAP_RM_Handle				hRm,		/* [in] Resource Manager handle */
	const BRAP_RM_P_ResrcReq	*pResrcReq,	/* [in] Required resources */
	BRAP_RM_P_ResrcGrant		*pResrcGrant/* [out] Indices of the allocated 
	                                           resources */
    )
{
    BERR_Code               ret = BERR_SUCCESS;
    BRAP_OutputChannelPair  eChP = 0;
    BRAP_RM_P_OpPortReq     sOpReq;
    unsigned int            uiSpdifFmId = 0;
    unsigned int            uiSpdifFmChId = 0;
    
    BDBG_ENTER(BRAP_RM_P_AllocateOpPort);

    /* Start allocating for each channel pairs in each of the parallel paths */
    for(eChP = 0; eChP < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; eChP++)
    {
        sOpReq = pResrcReq->sOpReq[eChP];
        if(true == sOpReq.bAllocate)
        {
            /* Allocate Output Port with out any checks */
            pResrcGrant->sOpPortGrnt[eChP].eOutputPort = sOpReq.eOpPortType;

            /* Allocate SpdifFm Id and SpdifFm Stream Id */
            if(BRAP_OutputPort_eMai == sOpReq.eOpPortType) 
            {
                BRAP_RM_P_GetSpdifFmForOpPort(hRm,
                                              sOpReq.eOpPortType,
                                              sOpReq.eMuxSelect, 
                                              &uiSpdifFmId, 
                                              &uiSpdifFmChId);
            }
            else
            {
                BRAP_RM_P_GetSpdifFmForOpPort(hRm,
                                              sOpReq.eOpPortType,
                                              BRAP_OutputPort_eMax,
                                              &uiSpdifFmId, 
                                              &uiSpdifFmChId);
            }
            
            pResrcGrant->sOpPortGrnt[eChP].uiSpdiffmId = uiSpdifFmId;
            pResrcGrant->sOpPortGrnt[eChP].uiSpdiffmStreamId = uiSpdifFmChId;

            /* Update the usage for SpdifFm */
            if(((unsigned int)BRAP_RM_P_INVALID_INDEX != uiSpdifFmChId) &&
               ((unsigned int)BRAP_RM_P_INVALID_INDEX != uiSpdifFmId))  
            {
                hRm->sSpdiffmUsage[uiSpdifFmId].uiUsageCount[uiSpdifFmChId] += 1;        
            }
        }            
    }/* for eChP */
            
    BDBG_LEAVE(BRAP_RM_P_AllocateOpPort);
    return ret;
}
#if (BRAP_7550_FAMILY != 1)
/***************************************************************************
Summary:
    Private function that allocates Capture Port. 

    Note: This private function will not do any malloc and pointer checks. 
    This is quite a low level helper function. Also, this function should 
    not init the pResrcGrant. It can just modify the respective allocated
    resource Id.
**************************************************************************/
static BERR_Code 
BRAP_RM_P_AllocateCapPort(
	BRAP_RM_Handle				hRm,		/* [in] Resource Manager handle */
	const BRAP_RM_P_ResrcReq	*pResrcReq,	/* [in] Required resources */
	BRAP_RM_P_ResrcGrant		*pResrcGrant/* [out] Indices of the allocated 
	                                           resources */
    )
{
    BERR_Code               ret = BERR_SUCCESS;
    BRAP_OutputChannelPair  eChP = 0;
    BRAP_RM_P_CapPortReq    sCapReq;
    unsigned int            uiNumIntCapPort = 0;
    unsigned int            uiIdx = 0;
    unsigned int            i = 0, j=0;
    
    BDBG_ENTER(BRAP_RM_P_AllocateCapPort);

    /* Find out the number of continuous internal cap-ports required */
    for(i=0;i<BRAP_P_MAX_OUT_PARALLEL_FMM_PATH;i++)
    {
        for(eChP = 0; eChP < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; eChP++)
        {

            if((true == pResrcReq->sCapReq[eChP][i].bAllocate) &&
               (BRAP_CapInputPort_eMax == pResrcReq->sCapReq[eChP][i].eCapPort)) 
            {
                uiNumIntCapPort++;
            }
        }
    }

    /* If internal capPorts are requested */
    if(0 != uiNumIntCapPort)
    {
        /* Find the starting index in BRAP_RM_P_InternalCapPortPref[] that fulfils the
           required number of free & continuous internal CapPort */
        for(uiIdx = 0; (uiIdx + uiNumIntCapPort)<BRAP_RM_P_MAX_SRC_CHANNELS; uiIdx++)
        {
            if(0 == hRm->sCapPortUsage[BRAP_RM_P_InternalCapPortPref[uiIdx]].uiUsageCount)
            {
                /* Check if required number of continuous CapPorts are free */
                for(j = 0; j < uiNumIntCapPort; j++)
                {
                    if(0 != hRm->sCapPortUsage[BRAP_RM_P_InternalCapPortPref[uiIdx+j]].uiUsageCount) 
                    {
                        break;
                    }
                }/* for j */
                
                if(uiNumIntCapPort == j)
                {
                    /* Eureka!! */
                    break;
                }/* if */
            }/* if uiUsageCount == 0 */
        }/* for uiIdx */
    }/* if uiNumIntCapPort != 0 */

    BDBG_MSG(("AllocateCapPort: CapInputPort Grant "));    
    /* Start allocating for each channel pairs in each of the parallel paths */
    for(j=0, i = 0;j<BRAP_P_MAX_OUT_PARALLEL_FMM_PATH;j++)
    {
        for(eChP = 0; eChP < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; eChP++)
        {
            sCapReq = pResrcReq->sCapReq[eChP][j];
            if(true == sCapReq.bAllocate)
            {
                if(BRAP_CapInputPort_eMax == sCapReq.eCapPort)
                {
                    BDBG_ASSERT(i < uiNumIntCapPort);

                    /* Allocate the fresh internal CapPort */
                    pResrcGrant->sCapPortGrnt[eChP][j].eCapPort = 
                        BRAP_RM_P_InternalCapPortPref[uiIdx+i];

                    /* Increment the usage count */
                    hRm->sCapPortUsage[BRAP_RM_P_InternalCapPortPref[uiIdx+i]].uiUsageCount += 1;

                    BDBG_MSG(("Internal CapPort :: "
                        "pResrcGrant->sCapPortGrnt[eChP=%d][j=%d].eCapPort =%d uiUsageCount =%d",
                        eChP, j, pResrcGrant->sCapPortGrnt[eChP][j].eCapPort,
                        hRm->sCapPortUsage[BRAP_RM_P_InternalCapPortPref[uiIdx+i]].uiUsageCount));

                    /* Increment i */
                    i++;                
                }
                else if (sCapReq.eCapPort < BRAP_CapInputPort_eMax)
                {
                    /* Allocate Capture Port with out any checks */
                    pResrcGrant->sCapPortGrnt[eChP][j].eCapPort = sCapReq.eCapPort;

                    /* Increment the usage count */
                    hRm->sCapPortUsage[sCapReq.eCapPort].uiUsageCount += 1;

                    BDBG_MSG(("External CapPort :: "
                        "pResrcGrant->sCapPortGrnt[eChP=%d][j=%d].eCapPort =%d uiUsageCount =%d",
                        eChP, j,pResrcGrant->sCapPortGrnt[eChP][j].eCapPort,
                        hRm->sCapPortUsage[sCapReq.eCapPort].uiUsageCount));
                }
                else
                {
                    BDBG_ERR(("BRAP_RM_P_AllocateCapPort: Invalid CapPort(%d) request",
                        sCapReq.eCapPort));
                    return BERR_TRACE(BERR_NOT_SUPPORTED);
                }
            }/* if bAllocate */        
        }/* for eChP */
    }/* for j */

    BDBG_LEAVE(BRAP_RM_P_AllocateCapPort);
    return ret;
}
#endif

/***************************************************************************
Summary:
    Private function that allocates FS Timing Source. 

    Note: This private function will not do any malloc and pointer checks. 
    This is quite a low level helper function. Also, this function should 
    not init the pResrcGrant. It can just modify the respective allocated
    resource Id.
**************************************************************************/
static BERR_Code 
BRAP_RM_P_AllocateFsTmgSource(
	BRAP_RM_Handle				hRm,		/* [in] Resource Manager handle */
	const BRAP_RM_P_FsTmgSrcReq *pFsTmgReq,	/* [in] Required resources */
	BRAP_RM_P_FsTmgSrcGrant     *pFsTmgGrant/* [out] Indices of the allocated 
	                                           resources */
    )
{
    BERR_Code       ret = BERR_SUCCESS;
    unsigned int    i = 0;
    unsigned int    uiFsTmgSrc = BRAP_RM_P_INVALID_INDEX;
    
    BDBG_ENTER(BRAP_RM_P_AllocateFsTmgSource);

    if(true == pFsTmgReq->bAllocate)
    {
        if(BRAP_RM_P_INVALID_INDEX != pFsTmgReq->uiFsTmgSrcId)
        {
            /* Re-allocate request */
            uiFsTmgSrc = pFsTmgReq->uiFsTmgSrcId;
        }
        else
        {
            /* Fresh allocate request */
            for(i=0; i<BRAP_RM_P_MAX_FS_TMG_SOURCES; i++)
            {
                if(0 == hRm->sFsTmgSrcUsage[i].uiUsageCount)
                {
                    uiFsTmgSrc = i;
                    break;
                }
            }

            if(i >= BRAP_RM_P_MAX_FS_TMG_SOURCES)
            {
                BDBG_ERR(("BRAP_RM_P_AllocateFsTmgSource: No free FS Timing Source"));
                return BERR_TRACE(BRAP_ERR_RESOURCE_EXHAUSTED);
            }
        }
    }

    if((BERR_SUCCESS == ret) &&
       (BRAP_RM_P_INVALID_INDEX != uiFsTmgSrc))
    {
        /* Update RM Handle */
        hRm->sFsTmgSrcUsage[uiFsTmgSrc].uiUsageCount++;

        /* Populate the grant structure */
        pFsTmgGrant->uiFsTmgSrc = uiFsTmgSrc;

        BDBG_MSG(("AllocateFsTmgSource: Allocated FS Timing Source %d Usage = %d",
            uiFsTmgSrc,hRm->sFsTmgSrcUsage[uiFsTmgSrc].uiUsageCount));
    }

    BDBG_LEAVE(BRAP_RM_P_AllocateFsTmgSource);
    return ret;
}

/***************************************************************************
Summary:
    Frees allocated resources

Description:
	This function frees all the resources corresponding to a particular 
	instance of audio operation that is getting closed.

Returns:
	BERR_SUCCESS - if successful else error

See Also:
	BRAP_RM_P_AllocateResources
**************************************************************************/
BERR_Code
BRAP_RM_P_FreeResources (
	BRAP_RM_Handle	            hRm,		/* [in] Resource Manager handle */
	const BRAP_RM_P_ResrcGrant  *pResrcGrant,/* [in] Indices of the resources
											    to be freed */
        bool   		bFreeSrcChAndRbuf /*If this function is called for Playback Channel,
                                                                Decode PCM path And if bOpenTimeWrToRbuf = true 
                                                                then don't free Srcch and Rbuf which allocated in
                                                                ChannelOPen of Pb*/
	)
{
    BERR_Code               ret = BERR_SUCCESS;
    BRAP_RM_Handle          hRmTemp = NULL;
    unsigned int            uiSpdifFmId = 0;
    unsigned int            uiBlk = 0;
    unsigned int            uiId = 0;
    unsigned int            uiDp = 0;
    unsigned int            uiFifoId = 0;
    BRAP_RM_P_SrcGrant      sSrcGrant;
    BRAP_RM_P_MixerGrant    sMixerGrant;
    BRAP_RM_P_SrcEqGrant    sSrcEqGrant;    
    unsigned int            i = 0, j = 0, k = 0, l = 0; 
    unsigned int            uiSpdifFmStream = 0;
    BRAP_CapInputPort       eCapPort = BRAP_CapInputPort_eMax;
    
	BDBG_ENTER(BRAP_RM_P_FreeResources);

	/* Assert the function arguments*/
	BDBG_ASSERT(hRm);
	BDBG_ASSERT(pResrcGrant);

    /* Allocate a temporary RM Handle */
	hRmTemp = (BRAP_RM_Handle)BKNI_Malloc(sizeof(BRAP_RM_P_Object));
	if(NULL == hRmTemp)
	{
		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
	}
        BKNI_Memset(hRmTemp , 0, sizeof(BRAP_RM_P_Object)); 
	/* Copy the original RM handle content to the temporary handle */
	*hRmTemp = *hRm;

    /* Free each valid resource in pResrcGrant structure and update the usage */

    /* Free Dsp */
    if((pResrcGrant->uiDspId != (unsigned int)BRAP_RM_P_INVALID_INDEX) 
             && (hRmTemp->sDspUsage[pResrcGrant->uiDspId].uiDecCxtCount > 0))
    {   
        /* TODO: Handle other contexts later */
    	hRmTemp->sDspUsage[pResrcGrant->uiDspId].uiDecCxtCount--;
    	hRmTemp->sDspUsage[pResrcGrant->uiDspId].eContext[pResrcGrant->uiDspContextId]
    			= BRAP_RM_P_ObjectState_eFree;
    	BDBG_MSG(("\nFree DSP  ID = %d",pResrcGrant->uiDspId));			
    	BDBG_MSG(("\nFree DSP Context ID = %d",pResrcGrant->uiDspContextId));		
    }   
#if ( BRAP_RM_P_MAX_DSPS > 1 )
    if((pResrcGrant->uiDspId < BRAP_RM_P_MAX_DSPS)
		&&(pResrcGrant->uiDspId == (unsigned int)BRAP_RM_P_ENC_DSP_ID) 
             && (hRmTemp->sDspUsage[pResrcGrant->uiDspId].uiEncCxtCount > 0))
    {   
        /* TODO: Handle other contexts later */
    	hRmTemp->sDspUsage[pResrcGrant->uiDspId].uiEncCxtCount--;
    	hRmTemp->sDspUsage[pResrcGrant->uiDspId].eContext[pResrcGrant->uiDspContextId]
    			= BRAP_RM_P_ObjectState_eFree;
    	BDBG_MSG(("\nFree DSP  ID = %d",pResrcGrant->uiDspId));			
    	BDBG_MSG(("\nFree DSP Context ID = %d",pResrcGrant->uiDspContextId));		
    }   
#endif

if(bFreeSrcChAndRbuf == true)
{
    /* Free RBuf */
    for(i = 0; i < BRAP_RM_P_MAX_OP_CHANNELS; i++)
    {
        if(pResrcGrant->uiRbufId[i] != (unsigned int)BRAP_RM_P_INVALID_INDEX)
        {
            hRmTemp->eRBufState[pResrcGrant->uiRbufId[i]]
                = BRAP_RM_P_ObjectState_eFree;

            /* Free the corresponding SrcCh and DstCh as well */
            uiFifoId = pResrcGrant->uiRbufId[i]>>1;
            if(uiFifoId < BRAP_RM_P_MAX_SRC_CHANNELS)
            {
                hRmTemp->eSrcChState[uiFifoId] = BRAP_RM_P_ObjectState_eFree;            
            }
            else
            {
                uiFifoId -= BRAP_RM_P_MAX_SRC_CHANNELS;
                hRmTemp->eDstChState[uiFifoId] = BRAP_RM_P_ObjectState_eFree;            
            }
        }
    }

    /* Free SrcCh */
    for(i = 0; i < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; i++)
    {
        if(pResrcGrant->uiSrcChId[i] != (unsigned int)BRAP_RM_P_INVALID_INDEX)
        {
            hRmTemp->eSrcChState[pResrcGrant->uiSrcChId[i]] = BRAP_RM_P_ObjectState_eFree;                    
        }
    }
}
    /* Free AdaptRateCtrl */
    for(i = 0; i < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; i++)
    {
        if(pResrcGrant->uiAdaptRateCtrlId[i] != (unsigned int)BRAP_RM_P_INVALID_INDEX)
        {
            hRmTemp->eAdaptRateCtrlState[pResrcGrant->uiAdaptRateCtrlId[i]] = BRAP_RM_P_ObjectState_eFree;                    
        }
    }


    /* Free DstCh */
    for(i = 0; i < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; i++)
    {
        if(pResrcGrant->uiDstChId[i] != (unsigned int)BRAP_RM_P_INVALID_INDEX)
        {
            hRmTemp->eDstChState[pResrcGrant->uiDstChId[i]] = BRAP_RM_P_ObjectState_eFree;                    
        }
    }
    
    /* Free SRC & Mixer */
    for(l = 0; l < BRAP_RM_P_MAX_MIXING_LEVELS; l++)
    {
        /* Free SRC */
        for(k = 0; k < BRAP_RM_P_MAX_SRC_IN_CASCADE; k++)
        {
            for(i = 0; i < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; i++)
            {
                for(j = 0; j < BRAP_RM_P_MAX_PARALLEL_PATHS; j++)
                {
                    sSrcGrant = 
                        pResrcGrant->sSrcMixerGrnt[l].sSrcGrant[k][i][j];

                    if( (sSrcGrant.uiSrcId!=(unsigned int)BRAP_RM_P_INVALID_INDEX) &&
                        (sSrcGrant.uiSrcBlkId!=(unsigned int)BRAP_RM_P_INVALID_INDEX))
                    {
                        uiBlk = sSrcGrant.uiSrcBlkId;
                        uiId = sSrcGrant.uiSrcId;
                        if(hRmTemp->sSrcUsage[uiBlk][uiId].uiUsageCount != 0)
                        {
                            hRmTemp->sSrcUsage[uiBlk][uiId].uiUsageCount -= 1; 
                        }
                        else
                        {
                            ret = BERR_TRACE(BERR_NOT_INITIALIZED);
                        }
                    }
                    
                }/* for j */
            }/* for i */
        }/* for k */

        /* Free Mixer */
        for(i = 0; i < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; i++)
        {
            for(j = 0; j < BRAP_RM_P_MAX_PARALLEL_PATHS; j++)
            {
                sMixerGrant = 
                    pResrcGrant->sSrcMixerGrnt[l].sMixerGrant[i][j];
                if(sMixerGrant.uiMixerId!=(unsigned int)BRAP_RM_P_INVALID_INDEX)
                {
                    uiDp = sMixerGrant.uiDpId;
                    uiId = sMixerGrant.uiMixerId;

                    for(k = 0; k < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; k++)
                    {
                        if (sMixerGrant.uiMixerInputId[k]!=(unsigned int)BRAP_RM_P_INVALID_INDEX)
                        {
                            hRmTemp->sMixerUsage[uiDp][uiId].
                                uiInputUsgCnt[sMixerGrant.uiMixerInputId[k]]-= 1;
                        }
                    }
                    for(k = 0; k < BRAP_RM_P_MAX_MIXER_OUTPUTS; k++)
                    {
                        if (sMixerGrant.uiMixerOutputId[k] !=(unsigned int)BRAP_RM_P_INVALID_INDEX)
                        {
                            hRmTemp->sMixerUsage[uiDp][uiId].
                             uiOutputUsgCnt[sMixerGrant.uiMixerOutputId[k]]-= 1; 
                        }
                    }/* for k */
                }
            }/* for j */
        }/* for i */
           
    }/* for l */

    /* Free SRC-EQ */
    for(k = 0; k < BRAP_RM_P_MAX_SRC_IN_CASCADE; k++)
    {
        for(i = 0; i < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; i++)
        {
            for(j = 0; j < BRAP_RM_P_MAX_PARALLEL_PATHS; j++)
            {
                sSrcEqGrant = pResrcGrant->sSrcEqGrant[k][i][j];
                if( (sSrcEqGrant.uiSrcId!=(unsigned int)BRAP_RM_P_INVALID_INDEX) &&
                    (sSrcEqGrant.uiSrcBlkId!=(unsigned int)BRAP_RM_P_INVALID_INDEX))                    
                {
                    uiBlk = sSrcEqGrant.uiSrcBlkId;
                    uiId = sSrcEqGrant.uiSrcId;
                    if(hRmTemp->sSrcUsage[uiBlk][uiId].uiUsageCount != 0)
                    {
                        hRmTemp->sSrcUsage[uiBlk][uiId].uiUsageCount -= 1; 
                        if(hRmTemp->sSrcUsage[uiBlk][uiId].uiUsageCount == 0)
                            hRmTemp->sSrcUsage[uiBlk][uiId].bSrcEq = false;
                        
                        BDBG_MSG (("BRAP_RM_P_FreeResources:SRC-EQ:: UsageCount of SRC[%d][%d]=%d",
                            uiBlk,uiId,hRmTemp->sSrcUsage[uiBlk][uiId].uiUsageCount));                        
                    }
                    else
                    {
                        ret = BERR_TRACE(BERR_NOT_INITIALIZED);
                    }
                }
                
            }/* for j */
        }/* for i */
    }/* for k */    
            
    /* Free SpdifFm */
    for(i = 0; i < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; i++)
    {
        uiSpdifFmId = pResrcGrant->sOpPortGrnt[i].uiSpdiffmId;
        uiSpdifFmStream = pResrcGrant->sOpPortGrnt[i].uiSpdiffmStreamId;
        if(uiSpdifFmId != (unsigned int)BRAP_RM_P_INVALID_INDEX) 
        {
            if(hRmTemp->sSpdiffmUsage[uiSpdifFmId].uiUsageCount[uiSpdifFmStream] != 0)
            {
                hRmTemp->sSpdiffmUsage[uiSpdifFmId].uiUsageCount[uiSpdifFmStream] -= 1; 
            }
            else
            {
                ret = BERR_TRACE(BERR_NOT_INITIALIZED);
            }
        }
    }/* for i */

    /* Free CapPort */
    for(i = 0; i < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; i++)
    {
        for(j=0;j<BRAP_P_MAX_OUT_PARALLEL_FMM_PATH;j++)
        {
            eCapPort = pResrcGrant->sCapPortGrnt[i][j].eCapPort;
            if(eCapPort != BRAP_CapInputPort_eMax) 
            {
                if(hRmTemp->sCapPortUsage[eCapPort].uiUsageCount != 0)
                {
                    hRmTemp->sCapPortUsage[eCapPort].uiUsageCount -= 1; 
                }
                else
                {
                    ret = BERR_TRACE(BERR_NOT_INITIALIZED);
                }
            }
        }
    }/* for i */

    /* Free FS Timing Source */
    if(pResrcGrant->sFsTmgGrnt.uiFsTmgSrc !=(unsigned int)BRAP_RM_P_INVALID_INDEX)
    {
        if(hRmTemp->sFsTmgSrcUsage[pResrcGrant->sFsTmgGrnt.uiFsTmgSrc].uiUsageCount != 0)
        {
            hRmTemp->sFsTmgSrcUsage[pResrcGrant->sFsTmgGrnt.uiFsTmgSrc].uiUsageCount -= 1; 
        }
        else
        {
            ret = BERR_TRACE(BERR_NOT_INITIALIZED);
        }
    }

    /* Copy the temporary RM handle content to the original RM handle */
	*hRm = *hRmTemp;
	
	/* Free the temporary RM handle */
	BKNI_Free( hRmTemp );

	BDBG_MSG(("\nFreed the resource"));		

	BDBG_LEAVE(BRAP_RM_P_FreeResources);
    return ret;
}

/***************************************************************************
Summary:
    Private function that returns the preferred Spdif formater Id and stream
    Id for a particular output port.  
**************************************************************************/
BERR_Code
BRAP_RM_P_GetSpdifFmForOpPort(
    BRAP_RM_Handle      hRm,            /* [in] Resource Manager handle */
    BRAP_OutputPort     eOutputPort,    /* [in] Output port */
    BRAP_OutputPort     eMuxOpPort,     /* [in] Mux For MAI port */
    unsigned int	    *pSpdifFmId,	/* [out]Index of the SPDIF Formater
                                           used, BRAP_RM_P_INVALID_INDEX
                                           indicates SPDIF Formater not used */	
    unsigned int	    *pSpdifFmStreamId 
                                        /* [out]Index of the SPDIF Formater
                                           channel used. This field is valid
                                           if uiSpdiffmId != INVALID_INDEX */
    )
{
    BDBG_ENTER(BRAP_RM_P_GetSpdifFmForOpPort);
    
    BDBG_ASSERT(hRm);
    BDBG_ASSERT(pSpdifFmId);
    BDBG_ASSERT(pSpdifFmStreamId);
    BSTD_UNUSED(hRm);

    switch(eOutputPort)
    {
        case BRAP_OutputPort_eSpdif:
            *pSpdifFmId = 0;
            *pSpdifFmStreamId = 0;
            break;
        case BRAP_OutputPort_eSpdif1:
            *pSpdifFmId = 0; 
            *pSpdifFmStreamId = 1;
            break;
        case BRAP_OutputPort_eMai:
            switch(eMuxOpPort)
            {
                case BRAP_OutputPort_eSpdif:
                    *pSpdifFmId = 0;
                    *pSpdifFmStreamId = 0;
                    break;
                case BRAP_OutputPort_eSpdif1:
                    *pSpdifFmId = 0;
                    *pSpdifFmStreamId = 1;
                    break;                    
        	    case BRAP_OutputPort_eI2s0:
        	    case BRAP_OutputPort_eI2s1:
        	    case BRAP_OutputPort_eI2s2:
        	    case BRAP_OutputPort_eI2s3:
        	    case BRAP_OutputPort_eI2s4:
        	    case BRAP_OutputPort_eI2s5:
        	    case BRAP_OutputPort_eI2s6:
        	    case BRAP_OutputPort_eI2s7:
        	    case BRAP_OutputPort_eMaiMulti0:
        	    case BRAP_OutputPort_eMaiMulti1:
        	    case BRAP_OutputPort_eMaiMulti2:
        	    case BRAP_OutputPort_eMaiMulti3:                 
                    *pSpdifFmId = 0;
                    *pSpdifFmStreamId = 1;
                    break;
#if (BRAP_7405_FAMILY == 1)
        	    case BRAP_OutputPort_eMai:
                    *pSpdifFmId = 0;
                    *pSpdifFmStreamId = 1;
                    break;
#endif
                default: 
                    *pSpdifFmId = BRAP_RM_P_INVALID_INDEX;
                    *pSpdifFmStreamId = BRAP_RM_P_INVALID_INDEX;
                    BDBG_ERR(("eMuxOpPort %d not supported for MAI", eMuxOpPort));
                    return BERR_TRACE(BERR_NOT_SUPPORTED);
                break;
            }
            break;
	    case BRAP_OutputPort_eI2s0:
	    case BRAP_OutputPort_eI2s1:
	    case BRAP_OutputPort_eI2s2:
	    case BRAP_OutputPort_eI2s3:
            case BRAP_OutputPort_eMaiMulti0:
            case BRAP_OutputPort_eMaiMulti1:
            case BRAP_OutputPort_eMaiMulti2:
            case BRAP_OutputPort_eMaiMulti3:                 
        default: 
            *pSpdifFmId = BRAP_RM_P_INVALID_INDEX;
            *pSpdifFmStreamId = BRAP_RM_P_INVALID_INDEX;
            break;
    }/* switch */

    /* Bound checks */
    if((*pSpdifFmId != BRAP_RM_P_INVALID_INDEX) &&
       (BRAP_RM_P_MAX_SPDIFFMS <= *pSpdifFmId ))
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    if((*pSpdifFmStreamId != BRAP_RM_P_INVALID_INDEX) &&
       (BRAP_RM_P_MAX_SPDIFFM_STREAMS <= *pSpdifFmStreamId ))
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    
    BDBG_LEAVE(BRAP_RM_P_GetSpdifFmForOpPort);
    return BERR_SUCCESS;            
}

/***************************************************************************
Summary:
    Private function that returns Spdif formater Id corresponding to the 
    stream Id.  
**************************************************************************/
BERR_Code 
BRAP_RM_P_GetSpdifFmId (
	unsigned int uiStreamIndex,         /*[in] SPDIFFM Stream index */
	unsigned int * pSpdifFmIndex        /*[out] SPDIFFM corresponding to this 
	                                      SPDIFFM stream */
)
{
    BDBG_ENTER (BRAP_RM_P_GetSpdifFmId);
    BDBG_ASSERT (pSpdifFmIndex);
    if (uiStreamIndex > BRAP_RM_P_MAX_SPDIFFM_STREAMS) 
    {
    	BDBG_ERR(("BRAP_RM_P_GetSpdifFmId: SPDIFFM Stream Index should be less"
         " than %d", BRAP_RM_P_MAX_SPDIFFM_STREAMS));
    	return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    /* Currently there is only one SPDIF formatter, so we always return 0 */
    *pSpdifFmIndex = 0;

    /* Bound checks */
    if((*pSpdifFmIndex != BRAP_RM_P_INVALID_INDEX) &&
       (BRAP_RM_P_MAX_SPDIFFMS <= *pSpdifFmIndex))
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    BDBG_LEAVE (BRAP_RM_P_GetSpdifFmId);
    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
	Checks if an output port is supported. 
Description:
 	This function checks if an output port is supported or not.
Returns:
	BERR_SUCCESS - If successful
 	BERR_NOT_SUPPORTED - otherwise
See Also:
 	None
**************************************************************************/
BERR_Code BRAP_RM_P_IsOutputPortSupported (
	BRAP_OutputPort eOutputPort
)
{
	BERR_Code rc = BERR_SUCCESS;
    BDBG_ENTER(BRAP_RM_P_IsOutputPortSupported);
	
   	switch(eOutputPort)
	{
      	/* Currently supported output ports:  */
   		case BRAP_OutputPort_eI2s0:
       	case BRAP_OutputPort_eI2s1:
       	case BRAP_OutputPort_eI2s2:
       	case BRAP_OutputPort_eI2s3:
       	case BRAP_OutputPort_eI2s4:
       	case BRAP_OutputPort_eDac0:
#if (BRAP_3548_FAMILY == 1)           
        case BRAP_OutputPort_eDac1:
        case BRAP_OutputPort_eDac2:  
#endif            
       	case BRAP_OutputPort_eSpdif:
       	case BRAP_OutputPort_eMai:
        	break;
#if (BCHP_CHIP ==7420)            
              case BRAP_OutputPort_eDac1:
#if (BCHP_VER == A0)
                    BDBG_WRN(("DAC1 ON 7420 WILL NOT GET AUDIO DUE TO HARDWARE BUG."));
                    BDBG_WRN(("DAC0 NEEDS TO BE POWERED DOWN FOR THAT PURPOSE."));
                    BDBG_WRN(("THIS IS NOT SUPPORTED IN THIS VERSION OF CODE"));
#endif                    
                    break;
#endif                    
#if (BRAP_7420_FAMILY ==1)||(BRAP_7550_FAMILY ==1)                              
              case BRAP_OutputPort_eMaiMulti0:
              case BRAP_OutputPort_eMaiMulti1:
              case BRAP_OutputPort_eMaiMulti2:
              case BRAP_OutputPort_eMaiMulti3:
                break;
#endif                    
   		default:
			BDBG_ERR(("BRAP_RM_P_IsOutputPortSupported: output port %d not"
                     "supported", eOutputPort));
        	rc = BRAP_ERR_OUPUT_PORT_NOT_SUPPORTED;
   	}

	BDBG_LEAVE(BRAP_RM_P_IsOutputPortSupported);
	return rc;
}

/***************************************************************************
Summary:
    Private function that updates the old resource grant structure with the 
    new one. 
    Ex: Audio Manager has its own resource grant structure (old) with some 
    resources granted. Now, it allocates some more resources and gets a new 
    resource grant structure. These new granted structure can be updated to the 
    AM's old resource grant structure using this API. 
**************************************************************************/
BERR_Code
BRAP_RM_P_UpdateResrcGrant (
    BRAP_RM_P_ResrcGrant        *pOldResrcGrant,
    BRAP_RM_P_ResrcGrant        *pNewResrcGrant
)
{
    BERR_Code ret = BERR_SUCCESS;
    int i = 0, j = 0, k = 0, l = 0;

	BDBG_ENTER(BRAP_RM_P_UpdateResrcGrant);    
    
    if((pNewResrcGrant->uiDspId != BRAP_RM_P_INVALID_INDEX) &&
        (pOldResrcGrant->uiDspId == BRAP_RM_P_INVALID_INDEX))
    {
        pOldResrcGrant->uiDspId = pNewResrcGrant->uiDspId;
    }
    else if(((pNewResrcGrant->uiDspId != BRAP_RM_P_INVALID_INDEX) &&
             (pOldResrcGrant->uiDspId != BRAP_RM_P_INVALID_INDEX)) &&
             (pOldResrcGrant->uiDspId != pNewResrcGrant->uiDspId))
    {
        ret = BERR_TRACE(BERR_INVALID_PARAMETER);
        goto err;
    }
    
    if((pNewResrcGrant->uiDspContextId != BRAP_RM_P_INVALID_INDEX)&&
       (pOldResrcGrant->uiDspContextId == BRAP_RM_P_INVALID_INDEX))
    {
        pOldResrcGrant->uiDspContextId = pNewResrcGrant->uiDspContextId;
    }
    else if(((pNewResrcGrant->uiDspContextId != BRAP_RM_P_INVALID_INDEX) &&
             (pOldResrcGrant->uiDspContextId != BRAP_RM_P_INVALID_INDEX)) &&
             (pOldResrcGrant->uiDspContextId != pNewResrcGrant->uiDspContextId))
    {
        ret = BERR_TRACE(BERR_INVALID_PARAMETER);
        goto err;
    }
    
    if((pNewResrcGrant->uiFmmId != BRAP_RM_P_INVALID_INDEX)&&
       (pOldResrcGrant->uiFmmId == BRAP_RM_P_INVALID_INDEX))
    {
        pOldResrcGrant->uiFmmId = pNewResrcGrant->uiFmmId;
    }
    else if(((pNewResrcGrant->uiFmmId != BRAP_RM_P_INVALID_INDEX)&&
             (pOldResrcGrant->uiFmmId != BRAP_RM_P_INVALID_INDEX)) &&
            (pNewResrcGrant->uiFmmId != pOldResrcGrant->uiFmmId))
    {
        ret = BERR_TRACE(BERR_INVALID_PARAMETER);
        goto err;
    }

    /* Rbuf */
    for(i = 0; i < BRAP_RM_P_MAX_OP_CHANNELS; i++)
    {
        if ((pNewResrcGrant->uiRbufId[i] != BRAP_RM_P_INVALID_INDEX)&&
            (pOldResrcGrant->uiRbufId[i] == BRAP_RM_P_INVALID_INDEX))
        {
            pOldResrcGrant->uiRbufId[i] = pNewResrcGrant->uiRbufId[i];
        }
        else if(((pNewResrcGrant->uiRbufId[i] != BRAP_RM_P_INVALID_INDEX)&&
                 (pOldResrcGrant->uiRbufId[i] != BRAP_RM_P_INVALID_INDEX)) &&
                 (pOldResrcGrant->uiRbufId[i] != pNewResrcGrant->uiRbufId[i]))
        {
            ret = BERR_TRACE(BERR_INVALID_PARAMETER);
            goto err;
        }
    }/* for i */

    /* SrcCh */
    for(i = 0; i < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; i++)
    {
        if((pNewResrcGrant->uiSrcChId[i] != BRAP_RM_P_INVALID_INDEX)&&
           (pOldResrcGrant->uiSrcChId[i] == BRAP_RM_P_INVALID_INDEX))
        {
            pOldResrcGrant->uiSrcChId[i] = pNewResrcGrant->uiSrcChId[i];
        }
        else if(((pNewResrcGrant->uiSrcChId[i] != BRAP_RM_P_INVALID_INDEX)&&
                 (pOldResrcGrant->uiSrcChId[i] != BRAP_RM_P_INVALID_INDEX)) &&
                (pNewResrcGrant->uiSrcChId[i] != pOldResrcGrant->uiSrcChId[i]))
        {
            ret = BERR_TRACE(BERR_INVALID_PARAMETER);
            goto err;
        }
    }/* for i */

    /* DstCh */
    for(i = 0; i < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; i++)
    {
        if((pNewResrcGrant->uiDstChId[i] != BRAP_RM_P_INVALID_INDEX)&&
           (pOldResrcGrant->uiDstChId[i] == BRAP_RM_P_INVALID_INDEX))
        {
            pOldResrcGrant->uiDstChId[i] = pNewResrcGrant->uiDstChId[i];
        }
        else if(((pNewResrcGrant->uiDstChId[i] != BRAP_RM_P_INVALID_INDEX)&&
                 (pOldResrcGrant->uiDstChId[i] != BRAP_RM_P_INVALID_INDEX)) &&
                (pNewResrcGrant->uiDstChId[i] != pOldResrcGrant->uiDstChId[i]))
        {
            ret = BERR_TRACE(BERR_INVALID_PARAMETER);
            goto err;
        }
    }/* for i */

    /* AdaptRateCtrl */
    for(i = 0; i < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; i++)
    {
        if((pNewResrcGrant->uiAdaptRateCtrlId[i] != BRAP_RM_P_INVALID_INDEX)&&
           (pOldResrcGrant->uiAdaptRateCtrlId[i] == BRAP_RM_P_INVALID_INDEX))
        {
            pOldResrcGrant->uiAdaptRateCtrlId[i] = pNewResrcGrant->uiAdaptRateCtrlId[i];
        }
        else if(((pNewResrcGrant->uiAdaptRateCtrlId[i] != BRAP_RM_P_INVALID_INDEX)&&
                 (pOldResrcGrant->uiAdaptRateCtrlId[i] != BRAP_RM_P_INVALID_INDEX)) &&
                (pNewResrcGrant->uiAdaptRateCtrlId[i] != pOldResrcGrant->uiAdaptRateCtrlId[i]))
        {
            ret = BERR_TRACE(BERR_INVALID_PARAMETER);
            goto err;
        }
    }/* for i */


    /* sSrcGrant structure */
    for(i = 0; i < BRAP_RM_P_MAX_MIXING_LEVELS; i++)
    {
        for(j = 0; j < BRAP_RM_P_MAX_SRC_IN_CASCADE; j++)
        {
            for(k = 0; k < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; k++)
            {
                for(l = 0; l < BRAP_RM_P_MAX_PARALLEL_PATHS; l++)
                {
                    if((pNewResrcGrant->sSrcMixerGrnt[i].sSrcGrant[j][k][l].uiSrcBlkId 
                                                           != BRAP_RM_P_INVALID_INDEX)&&
                        (pOldResrcGrant->sSrcMixerGrnt[i].sSrcGrant[j][k][l].uiSrcBlkId 
                                                            == BRAP_RM_P_INVALID_INDEX))
                    {
                        pOldResrcGrant->sSrcMixerGrnt[i].sSrcGrant[j][k][l].uiSrcBlkId=
                            pNewResrcGrant->sSrcMixerGrnt[i].sSrcGrant[j][k][l].uiSrcBlkId;
                    }
                    else if(((pNewResrcGrant->sSrcMixerGrnt[i].sSrcGrant[j][k][l].uiSrcBlkId 
                                                           != BRAP_RM_P_INVALID_INDEX)&&
                             (pOldResrcGrant->sSrcMixerGrnt[i].sSrcGrant[j][k][l].uiSrcBlkId 
                                                            != BRAP_RM_P_INVALID_INDEX)) &&
                             (pNewResrcGrant->sSrcMixerGrnt[i].sSrcGrant[j][k][l].uiSrcBlkId != 
                              pOldResrcGrant->sSrcMixerGrnt[i].sSrcGrant[j][k][l].uiSrcBlkId))

                    {
                        ret = BERR_TRACE(BERR_INVALID_PARAMETER);
                        goto err;
                    }
                    if((pNewResrcGrant->sSrcMixerGrnt[i].sSrcGrant[j][k][l].uiSrcId  
                                                != BRAP_RM_P_INVALID_INDEX)&&
                        (pOldResrcGrant->sSrcMixerGrnt[i].sSrcGrant[j][k][l].uiSrcId  
                                                == BRAP_RM_P_INVALID_INDEX))
                    {
                        pOldResrcGrant->sSrcMixerGrnt[i].sSrcGrant[j][k][l].uiSrcId = 
                            pNewResrcGrant->sSrcMixerGrnt[i].sSrcGrant[j][k][l].uiSrcId;
                    }
                    else if(((pNewResrcGrant->sSrcMixerGrnt[i].sSrcGrant[j][k][l].uiSrcId  
                                                != BRAP_RM_P_INVALID_INDEX)&&
                             (pOldResrcGrant->sSrcMixerGrnt[i].sSrcGrant[j][k][l].uiSrcId  
                                                != BRAP_RM_P_INVALID_INDEX))&&
                            (pOldResrcGrant->sSrcMixerGrnt[i].sSrcGrant[j][k][l].uiSrcId != 
                             pNewResrcGrant->sSrcMixerGrnt[i].sSrcGrant[j][k][l].uiSrcId))

                    {
                        ret = BERR_TRACE(BERR_INVALID_PARAMETER);
                        goto err;
                    }
                }/* for l */
            }/* for k */
        }/* for j */
    }/* for i */

    /* sMixerGrant structure */
    for(i = 0; i < BRAP_RM_P_MAX_MIXING_LEVELS; i++)
    {
        for(j = 0; j < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; j++)
        {
            for(k = 0; k < BRAP_RM_P_MAX_PARALLEL_PATHS; k++)
            {
                if ((pNewResrcGrant->sSrcMixerGrnt[i].sMixerGrant[j][k].uiDpId != 
                                                        BRAP_RM_P_INVALID_INDEX)&&
                    (pOldResrcGrant->sSrcMixerGrnt[i].sMixerGrant[j][k].uiDpId == 
                                                        BRAP_RM_P_INVALID_INDEX))
                {
                    pOldResrcGrant->sSrcMixerGrnt[i].sMixerGrant[j][k].uiDpId = 
                        pNewResrcGrant->sSrcMixerGrnt[i].sMixerGrant[j][k].uiDpId;
                }
                else if(((pNewResrcGrant->sSrcMixerGrnt[i].sMixerGrant[j][k].uiDpId != 
                                                        BRAP_RM_P_INVALID_INDEX)&&
                         (pOldResrcGrant->sSrcMixerGrnt[i].sMixerGrant[j][k].uiDpId != 
                                                        BRAP_RM_P_INVALID_INDEX)) &&
                         (pNewResrcGrant->sSrcMixerGrnt[i].sMixerGrant[j][k].uiDpId != 
                          pOldResrcGrant->sSrcMixerGrnt[i].sMixerGrant[j][k].uiDpId))
                {
                    ret = BERR_TRACE(BERR_INVALID_PARAMETER);
                    goto err;
                }
                if ((pNewResrcGrant->sSrcMixerGrnt[i].sMixerGrant[j][k].uiMixerId 
                                                    != BRAP_RM_P_INVALID_INDEX)&&
                    (pOldResrcGrant->sSrcMixerGrnt[i].sMixerGrant[j][k].uiMixerId 
                                                    == BRAP_RM_P_INVALID_INDEX))
                {
                    pOldResrcGrant->sSrcMixerGrnt[i].sMixerGrant[j][k].uiMixerId =
                        pNewResrcGrant->sSrcMixerGrnt[i].sMixerGrant[j][k].uiMixerId;
                }
                else if(((pNewResrcGrant->sSrcMixerGrnt[i].sMixerGrant[j][k].uiMixerId 
                                                    != BRAP_RM_P_INVALID_INDEX)&&
                         (pOldResrcGrant->sSrcMixerGrnt[i].sMixerGrant[j][k].uiMixerId 
                                                    != BRAP_RM_P_INVALID_INDEX))&&
                        (pNewResrcGrant->sSrcMixerGrnt[i].sMixerGrant[j][k].uiMixerId != 
                         pOldResrcGrant->sSrcMixerGrnt[i].sMixerGrant[j][k].uiMixerId))
                {
                    ret = BERR_TRACE(BERR_INVALID_PARAMETER);
                    goto err;
                }
                for(l = 0; l < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; l++)
                {
                    if ((pNewResrcGrant->sSrcMixerGrnt[i].sMixerGrant[j][k].uiMixerInputId[l] != 
                                                        BRAP_RM_P_INVALID_INDEX)&&
                        (pOldResrcGrant->sSrcMixerGrnt[i].sMixerGrant[j][k].uiMixerInputId[l] == 
                                                        BRAP_RM_P_INVALID_INDEX))
                    {
                        pOldResrcGrant->sSrcMixerGrnt[i].sMixerGrant[j][k].uiMixerInputId[l] =
                            pNewResrcGrant->sSrcMixerGrnt[i].sMixerGrant[j][k].uiMixerInputId[l];
                    }
                    else if(((pNewResrcGrant->sSrcMixerGrnt[i].sMixerGrant[j][k].uiMixerInputId[l] != 
                                                        BRAP_RM_P_INVALID_INDEX)&&
                             (pOldResrcGrant->sSrcMixerGrnt[i].sMixerGrant[j][k].uiMixerInputId[l] != 
                                                        BRAP_RM_P_INVALID_INDEX))&&
                             (pOldResrcGrant->sSrcMixerGrnt[i].sMixerGrant[j][k].uiMixerInputId[l] !=
                              pNewResrcGrant->sSrcMixerGrnt[i].sMixerGrant[j][k].uiMixerInputId[l])) 
                    {
                        ret = BERR_TRACE(BERR_INVALID_PARAMETER);
                        goto err;
                    }
                }/* for l */

                for(l = 0; l < BRAP_RM_P_MAX_MIXER_OUTPUTS; l++)
                {
                    if ((pNewResrcGrant->sSrcMixerGrnt[i].sMixerGrant[j][k].uiMixerOutputId[l] != 
                                                        BRAP_RM_P_INVALID_INDEX)&&
                        (pOldResrcGrant->sSrcMixerGrnt[i].sMixerGrant[j][k].uiMixerOutputId[l] == 
                                                        BRAP_RM_P_INVALID_INDEX))
                    {
                        pOldResrcGrant->sSrcMixerGrnt[i].sMixerGrant[j][k].uiMixerOutputId[l]=
                            pNewResrcGrant->sSrcMixerGrnt[i].sMixerGrant[j][k].uiMixerOutputId[l];
                    }
                    else if(((pNewResrcGrant->sSrcMixerGrnt[i].sMixerGrant[j][k].uiMixerOutputId[l] != 
                                                        BRAP_RM_P_INVALID_INDEX)&&
                             (pOldResrcGrant->sSrcMixerGrnt[i].sMixerGrant[j][k].uiMixerOutputId[l] != 
                                                        BRAP_RM_P_INVALID_INDEX))&&
                            (pNewResrcGrant->sSrcMixerGrnt[i].sMixerGrant[j][k].uiMixerOutputId[l] != 
                             pOldResrcGrant->sSrcMixerGrnt[i].sMixerGrant[j][k].uiMixerOutputId[l]))

                    {
                        ret = BERR_TRACE(BERR_INVALID_PARAMETER);
                        goto err;
                    }
                }/* for l */
            }/* for k */
        }/* for j */
    }/* for i */


    /* sSrcEqGrant structure */
    for(j = 0; j < BRAP_RM_P_MAX_SRC_IN_CASCADE; j++)
    {
        for(k = 0; k < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; k++)
        {
            for(l = 0; l < BRAP_RM_P_MAX_PARALLEL_PATHS; l++)
            {
                if((pNewResrcGrant->sSrcEqGrant[j][k][l].uiSrcBlkId 
                                                       != BRAP_RM_P_INVALID_INDEX)&&
                    (pOldResrcGrant->sSrcEqGrant[j][k][l].uiSrcBlkId 
                                                        == BRAP_RM_P_INVALID_INDEX))
                {
                    pOldResrcGrant->sSrcEqGrant[j][k][l].uiSrcBlkId=
                        pNewResrcGrant->sSrcEqGrant[j][k][l].uiSrcBlkId;
                }
                else if(((pNewResrcGrant->sSrcEqGrant[j][k][l].uiSrcBlkId 
                                                       != BRAP_RM_P_INVALID_INDEX)&&
                         (pOldResrcGrant->sSrcEqGrant[j][k][l].uiSrcBlkId 
                                                        != BRAP_RM_P_INVALID_INDEX)) &&
                         (pNewResrcGrant->sSrcEqGrant[j][k][l].uiSrcBlkId != 
                          pOldResrcGrant->sSrcEqGrant[j][k][l].uiSrcBlkId))

                {
                    ret = BERR_TRACE(BERR_INVALID_PARAMETER);
                    goto err;
                }
                if((pNewResrcGrant->sSrcEqGrant[j][k][l].uiSrcId  
                                            != BRAP_RM_P_INVALID_INDEX)&&
                    (pOldResrcGrant->sSrcEqGrant[j][k][l].uiSrcId  
                                            == BRAP_RM_P_INVALID_INDEX))
                {
                    pOldResrcGrant->sSrcEqGrant[j][k][l].uiSrcId = 
                        pNewResrcGrant->sSrcEqGrant[j][k][l].uiSrcId;
                }
                else if(((pNewResrcGrant->sSrcEqGrant[j][k][l].uiSrcId  
                                            != BRAP_RM_P_INVALID_INDEX)&&
                         (pOldResrcGrant->sSrcEqGrant[j][k][l].uiSrcId  
                                            != BRAP_RM_P_INVALID_INDEX))&&
                        (pOldResrcGrant->sSrcEqGrant[j][k][l].uiSrcId != 
                         pNewResrcGrant->sSrcEqGrant[j][k][l].uiSrcId))

                {
                    ret = BERR_TRACE(BERR_INVALID_PARAMETER);
                    goto err;
                }
            }/* for l */
        }/* for k */
    }/* for j */
    

    /* sOpPortGrnt structure */
    for(i = 0; i < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; i++)
    {
        if ((pNewResrcGrant->sOpPortGrnt[i].uiSpdiffmId != 
                                                 BRAP_RM_P_INVALID_INDEX)&&
            (pOldResrcGrant->sOpPortGrnt[i].uiSpdiffmId == 
                                                   BRAP_RM_P_INVALID_INDEX))
        {
            pOldResrcGrant->sOpPortGrnt[i].uiSpdiffmId =
                pNewResrcGrant->sOpPortGrnt[i].uiSpdiffmId;
        }
        else if(((pNewResrcGrant->sOpPortGrnt[i].uiSpdiffmId != 
                                                 BRAP_RM_P_INVALID_INDEX)&&
                 (pOldResrcGrant->sOpPortGrnt[i].uiSpdiffmId != 
                                                   BRAP_RM_P_INVALID_INDEX)) &&
                (pOldResrcGrant->sOpPortGrnt[i].uiSpdiffmId != 
                 pNewResrcGrant->sOpPortGrnt[i].uiSpdiffmId))
        {
            ret = BERR_TRACE(BERR_INVALID_PARAMETER);
            goto err;
        }
        if((pNewResrcGrant->sOpPortGrnt[i].uiSpdiffmStreamId != 
                                                  BRAP_RM_P_INVALID_INDEX)&&
            (pOldResrcGrant->sOpPortGrnt[i].uiSpdiffmStreamId == 
                                                   BRAP_RM_P_INVALID_INDEX))
        {
            pOldResrcGrant->sOpPortGrnt[i].uiSpdiffmStreamId =
                pNewResrcGrant->sOpPortGrnt[i].uiSpdiffmStreamId;
        }
        else if(((pNewResrcGrant->sOpPortGrnt[i].uiSpdiffmStreamId != 
                                                  BRAP_RM_P_INVALID_INDEX)&&
                 (pOldResrcGrant->sOpPortGrnt[i].uiSpdiffmStreamId != 
                                                   BRAP_RM_P_INVALID_INDEX)) &&
                (pOldResrcGrant->sOpPortGrnt[i].uiSpdiffmStreamId !=
                 pNewResrcGrant->sOpPortGrnt[i].uiSpdiffmStreamId))

        {
            ret = BERR_TRACE(BERR_INVALID_PARAMETER);
            goto err;
        }
        if((pNewResrcGrant->sOpPortGrnt[i].eOutputPort != BRAP_OutputPort_eMax)&&
           (pOldResrcGrant->sOpPortGrnt[i].eOutputPort == BRAP_OutputPort_eMax))
        {
            pOldResrcGrant->sOpPortGrnt[i].eOutputPort=
                pNewResrcGrant->sOpPortGrnt[i].eOutputPort;
        }
        else if(((pNewResrcGrant->sOpPortGrnt[i].eOutputPort != BRAP_OutputPort_eMax)&&
                 (pOldResrcGrant->sOpPortGrnt[i].eOutputPort != BRAP_OutputPort_eMax))&&
                 (pOldResrcGrant->sOpPortGrnt[i].eOutputPort != 
                  pNewResrcGrant->sOpPortGrnt[i].eOutputPort))
        {
            ret = BERR_TRACE(BERR_INVALID_PARAMETER);
            goto err;
        }
    }/* for i */

    /* sCapPortGrnt structure */
    for(i = 0; i < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; i++)
    {
        for(j=0;j<BRAP_P_MAX_OUT_PARALLEL_FMM_PATH;j++)
        {
            if((pNewResrcGrant->sCapPortGrnt[i][j].eCapPort != BRAP_CapInputPort_eMax)&&
               (pOldResrcGrant->sCapPortGrnt[i][j].eCapPort == BRAP_CapInputPort_eMax))
            {
                pOldResrcGrant->sCapPortGrnt[i][j].eCapPort = 
                                            pNewResrcGrant->sCapPortGrnt[i][j].eCapPort;
            }
            else if(((pNewResrcGrant->sCapPortGrnt[i][j].eCapPort != BRAP_CapInputPort_eMax)&&
                     (pOldResrcGrant->sCapPortGrnt[i][j].eCapPort != BRAP_CapInputPort_eMax))&&
                     (pOldResrcGrant->sCapPortGrnt[i][j].eCapPort != 
                                         pNewResrcGrant->sCapPortGrnt[i][j].eCapPort ))
            {
                ret = BERR_TRACE(BERR_INVALID_PARAMETER);
                goto err;
            }
        }
    }/* for i */

    /* FS Timing Source */
    if((pNewResrcGrant->sFsTmgGrnt.uiFsTmgSrc != BRAP_RM_P_INVALID_INDEX)&&
       (pOldResrcGrant->sFsTmgGrnt.uiFsTmgSrc == BRAP_RM_P_INVALID_INDEX))
    {
        pOldResrcGrant->sFsTmgGrnt.uiFsTmgSrc = pNewResrcGrant->sFsTmgGrnt.uiFsTmgSrc;
    }
    else if(((pNewResrcGrant->sFsTmgGrnt.uiFsTmgSrc != BRAP_RM_P_INVALID_INDEX)&&
             (pOldResrcGrant->sFsTmgGrnt.uiFsTmgSrc != BRAP_RM_P_INVALID_INDEX))&&
             (pOldResrcGrant->sFsTmgGrnt.uiFsTmgSrc != pNewResrcGrant->sFsTmgGrnt.uiFsTmgSrc ))
    {
        ret = BERR_TRACE(BERR_INVALID_PARAMETER);
        goto err;
    }

    goto no_err;

err:
    BDBG_ERR(("BRAP_RM_P_UpdateResrcGrant: COuld not Update the Resource Grant"
              "COnflicting Old and New Resource Grant structure "));
no_err:

    BDBG_LEAVE(BRAP_RM_P_UpdateResrcGrant);    
    return ret;
}

#ifdef BRAP_DBG_FMM
static void
BRAP_RM_P_DumpMixerUsage(
    BRAP_RM_Handle          hRm
    )    
{
    int i, j, k;
    for(i=0; i<BRAP_RM_P_MAX_DP_BLCK; i++)
    {
        for(j=0; j<BRAP_RM_P_MAX_MIXER_PER_DP_BLCK; j++)
        {
            BDBG_ERR(("[DpId, MixerId] = [%d, %d]", i,j));
            for(k=0;k<BRAP_RM_P_MAX_MIXER_INPUTS; k++)
            {
                if(0 != hRm->sMixerUsage[i][j].uiInputUsgCnt[k])
                  BDBG_ERR(("Input[%d] = %d",k, 
                    hRm->sMixerUsage[i][j].uiInputUsgCnt[k]));
            }
            for(k=0;k<BRAP_RM_P_MAX_MIXER_OUTPUTS; k++)
            {
                BDBG_ERR(("Output[%d] = %d",k, 
                    hRm->sMixerUsage[i][j].uiOutputUsgCnt[k]));
            }
        }
    }        
    return;
}
#endif
  
/*}}}*/
/* End of File */
