/***************************************************************************
*     Copyright (c) 2004-2011, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brap_rm_priv.h $
* $brcm_Revision: Hydra_Software_Devel/36 $
* $brcm_Date: 3/7/11 6:06p $
*
* Module Description: Refer Module Overview given below
*
* Revision History:
* 
* $brcm_Log: /magnum/portinginterface/rap/base3/brap_rm_priv.h $
* 
* Hydra_Software_Devel/36   3/7/11 6:06p pothiraj
* SW7550-648: [7550] Secure Code Module feature
* 
* Hydra_Software_Devel/7550_SCM_DEVEL/1   1/6/11 3:40p pothiraj
* SW7550-648: [7550] Secure Code Module feature
* 
* Hydra_Software_Devel/35   9/16/10 12:21a srajapur
* SW7468-298 :[7468]Merging Real Video Code to main latest
* 
* Hydra_Software_Devel/34   9/6/10 5:18p gautamk
* SW7550-459: [7550] Merging GFX support in mainline
* 
* Hydra_Software_Devel/33   9/6/10 3:41p gautamk
* SW7405-4713: [7405] Programming FCI CFG ID for IOP to 0x3ff at open
* time
* 
* Hydra_Software_Devel/RAP_7550_TASK_DOWNLOAD_BRANCH/1   9/3/10 3:38p gautamk
* SW7405-4713: [7405] Programming FCI CFG ID for IOP to 0x3ff at open
* time
* 
* Hydra_Software_Devel/32   4/15/10 9:46p sgadara
* SW3548-2852: [3548,3556] Providing support to add same Equalizer to
* multiple output ports. --> Currently the multiple output ports should
* belong to same channel.
* 
* Hydra_Software_Devel/31   3/26/10 10:57a sgadara
* SW3548-2859: [3548,3556] Adding Subwoofer(Low Pass Filter) to Equalizer
* settings. --> Started using SRC1 block
* 
* Hydra_Software_Devel/30   12/15/09 2:25p gautamk
* SW7405-740:[7405] increasing count of no. of Paths.
* 
* Hydra_Software_Devel/29   10/6/09 6:36p srajapur
* SW7550-52 : [7550] Merging branch 7550 code to main line
* 
* Hydra_Software_Devel/28   10/2/09 2:37a srajapur
* SW7550-52: [7550] Merging 7550 branch code to main line
* 
* Hydra_Software_Devel/27   7/23/09 4:49p bhanus
* PR 55499 : [7340,7342,7125]Adding 7420 Family support
* 
* Hydra_Software_Devel/26   6/12/09 3:38p gautamk
* PR53302: [7405] Merging cleanup code to mainline
* 
* Hydra_Software_Devel/25   2/13/09 5:00p gautamk
* PR51897: [7405] using SHaredPP Path type incase of Playback and capture
* channel for 7405
* 
* Hydra_Software_Devel/24   12/18/08 9:44a speter
* PR 50495: [3548,3556] Merging Release Phase 4.2 to main line of
* development
* 
* Hydra_Software_Devel/23   12/10/08 11:00p gautamk
* PR49232:[7405] Adding support for Allocating Rbuf/SrcCh and Paths at
* OpenChannel time.
* 
* Hydra_Software_Devel/RAP_354856_PH_4_1/1   12/16/08 2:37p sgadara
* PR 50324: [3548,3556] Properly linking the successive paths in case of
* a capture path. Also increasing the maximum number of paths in a
* channel, supported.
* 
* Hydra_Software_Devel/22   11/25/08 10:23a speter
* PR 49564: [3548,3556] Merging the Phase 4.0 release to main line
* 
* Hydra_Software_Devel/20   11/11/08 2:47p bhanus
* PR 47757 : [7420]Checking in 7420 Support
* 
* Hydra_Software_Devel/RAP_354856_PH_4_0/2   11/8/08 1:55p sgadara
* PR 42025: [3548,3556] Add volume control, with cascaded mixers.
* Enabling SRC to feed six mixers only for custom miser case.
* 
* Hydra_Software_Devel/RAP_354856_PH_4_0/1   11/8/08 12:24a sgadara
* PR 42025: [3548,3556] Add volume control, with cascaded mixers.
* 
* Hydra_Software_Devel/19   10/22/08 4:48a speter
* PR 46937: [3548,3556] Adding support for 6 simultaneous destinations
* support and some code cleanup
* 
* Hydra_Software_Devel/18   10/10/08 10:20a speter
* PR 47764: [3548,3556] Merging the release branch 3.0 to main line of
* development
* 
* Hydra_Software_Devel/RAP_354856_PH_3_0/3   10/6/08 7:41p speter
* PR 45444: [3548,3556]: Adding generic code for equalizer
* 
* Hydra_Software_Devel/RAP_354856_PH_3_0/2   9/29/08 3:32p speter
* PR 46655: [3548,3556] Increasing the maximum destination channels to 6
* for chip version B0 and above.
* 
* Hydra_Software_Devel/RAP_354856_PH_3_0/1   9/24/08 6:02p speter
* PR 45907: [3548,3556] Support for 4 contexts. Audio descriptor decode
* (2) and passthru (1) with capture channel (1)
* 
* Hydra_Software_Devel/17   9/16/08 3:46p speter
* PR 46942: [3548,3556] Merging back to main line of development to get
* the distinct output feature
* 
* Hydra_Software_Devel/RAP_354856_20080904_BRANCH/1   9/5/08 2:51p speter
* PR 38950: [3548,3556] Adding Audio Descriptor support - Adding 3 decode
* channels to support AD decode + Passthru
* 
* Hydra_Software_Devel/16   8/25/08 2:15p speter
* PR 45444: [3548,3556] Implementing the hardware equalizer. Adding extra
* cascaded SRCs for equalizer upsampling, IIR and downsampling
* 
* Hydra_Software_Devel/15   8/22/08 6:56p sushmit
* PR 43454:[7405]Incrementing max no. of dec channels for AD
* 
* Hydra_Software_Devel/14   8/22/08 6:43p sushmit
* PR 43454:[7405]Incrementing max no. of dec channels for AD
* 
* Hydra_Software_Devel/13   8/22/08 6:38p sushmit
* PR 43454:[7405]Incrementing max no. of dec channels for AD
* 
* Hydra_Software_Devel/12   8/20/08 6:12p gautamk
* PR43454: [7405] Changes for Concurrent downmix.
* 
* Hydra_Software_Devel/11   8/5/08 3:55p speter
* PR 45407: [3548,3556] Merging the Phase 2.1 release branch to main line
* of development (Hydra_Software_Devel)
* 
* Hydra_Software_Devel/RAP_354856_PH_2_1/1   8/4/08 1:28p speter
* PR 45094: [3548,3556] Added support for 3 simultaneous decode channels.
* This usage case is required when we have Audio Desc (2) + passthru(1)
* 
* Hydra_Software_Devel/10   7/16/08 10:08p gautamk
* PR43454: [7405] Implementing delta code review.
* 
* Hydra_Software_Devel/9   7/10/08 6:23p gautamk
* PR41825: [7405] merging the code for multiple associatio from Branch to
* mainline.
* 
* Hydra_Software_Devel/8   7/4/08 4:30p gautamk
* PR43454: [7405] Implementation for Adaptive rate ctrl (2ms lipsync)
* 
* Hydra_Software_Devel/7   5/9/08 4:58p speter
* PR38950: [3548,3556] Fixing the compilation errors.
* 
* Hydra_Software_Devel/6   5/5/08 11:44a speter
* PR 38950: [3548] Making 3548_FAMILY macro and using it for both 3548
* and 3556 cases.
* 
* Hydra_Software_Devel/5   4/28/08 4:31p speter
* PR 38950: [3548] Fixing compilation for 3548/3556.
* 
* Hydra_Software_Devel/4   4/22/08 1:59p gautamk
* PR41994: [7405] Using BCHP_VER_ Correctly...
* 
* Hydra_Software_Devel/3   2/22/08 4:20p gautamk
* PR34648: [7405] fixing coverity defects + implementing review comments
* 
* Hydra_Software_Devel/2   1/24/08 12:05a sushmit
* PR34648: [7405] Merging in updated code from Base2
* 
* Hydra_Software_Devel/33   9/30/07 1:58p sushmit
* PR35140: [7405] Develop 7325 Raptor PI for Bringup
* 
* Hydra_Software_Devel/1   1/22/08 4:48p sushmit
* PR34648: [7405] Merging in updated code from Base2
* 
* Hydra_Software_Devel/Rap_7405_NewFwIf/2   1/16/08 5:03p gdata
* PR34648: [7405]  Adding changes for getchannelpath and distribute
* destination
* 
* Hydra_Software_Devel/Rap_7405_NewFwIf/1   12/31/07 3:03p nitinb
* PR34648: [7405] Added FW task related data structures
* 
* Hydra_Software_Devel/32   8/27/07 10:57a speter
* PR 33362: [3563] Including enum BRAP_P_UsgPath_eDlyBalance to be part
* of 3563 though it is used only for 7440. Otherwise there are so many
* places where #ifdef Sequential path is required.
* 
* Hydra_Software_Devel/31   8/20/07 6:57p kagrawal
* PR 34154: [7440] [Hi-Def DVD PR 5507] Increasing
* BRAP_RM_P_MAX_PARALLEL_PATHS from 6 to 7
* 
* Hydra_Software_Devel/30   8/9/07 3:07p sushmit
* PR 30552: [3563] Added Channel Link Create/Destroy APIs. Modified
* BRAP_ProcessDigitalInputFormatChange to take care of Simul mode for
* compressed input.
* 
* Hydra_Software_Devel/29   7/26/07 11:54a kagrawal
* PR 32986: [7440] Fixing mixer allocation for even number of cloned
* output ports at 2 or more levels
* 
* Hydra_Software_Devel/28   7/13/07 1:47p bhanus
* PR 32986: [7440 B0] Code modifed for Sequential path bring up.
* 
* Hydra_Software_Devel/27   6/27/07 10:07p kagrawal
* PR 32504: Added bound check for ParallelPath to prevent hOp corruption.
* Increased BRAP_RM_P_MAX_PARALLEL_PATHS to 5 for 7440.
* 
* Hydra_Software_Devel/26   6/15/07 2:26p kagrawal
* PR 30069: [7440] Merging Phase2.0 branch to Hydra_Software_Devel
* 
* Hydra_Software_Devel/RAP_7440_Phase2.0_Branch/1   6/14/07 1:30p bhanus
* PR 30069 : [7440 B0] Added code to support:
* - HBR path
* - HBR configuration of I2sMulti_1 and Mai
* - Passing HBR flag to DSP1
*
* Hydra_Software_Devel/25   6/11/07 11:46a bhanus
* PR 30069 : [7440 B] Modified BRAP_RM_P_MAX_MIXING_LEVELS to 5 for
* fixing the HiDef-DVD PR 4254.
* 
* Hydra_Software_Devel/24   6/5/07 11:06a speter
* PR 31827: [3563] Merging from Phase 2.0 release branch to main
* 
* Hydra_Software_Devel/23   5/26/07 8:07p speter
* PR 30168: [3563] When 4 output ports were added for cloning (after
* support of I2S0 in stereo mode), the MixerSrcRsrcLayout had problem
* and leading to segmentation fault. I have increased the mixing levels
* to handle this.
* 
* Hydra_Software_Devel/RAP_3563_PH_2_0/1   5/29/07 10:57a speter
* PR 30168: [3563] When 4 output ports were added for cloning (after
* support of I2S0 in stereo mode), the MixerSrcRsrcLayout had problem
* and leading to segmentation fault. I have increased the mixing levels
* to handle this.
* 
* Hydra_Software_Devel/22   5/12/07 8:40a sushmit
* PR 27646: Getting 7405 PI compiled
* 
* Hydra_Software_Devel/21   5/11/07 9:00p sushmit
* PR 27646: Getting 7405 PI compiled
* 
* Hydra_Software_Devel/20   5/11/07 10:31a kagrawal
* PR 30069: Added FS Timing Source as a resource in RM
* 
* Hydra_Software_Devel/19   4/26/07 7:23p bhanus
* PR 30069 : Code Modified for sequential path bring up
* 
* Hydra_Software_Devel/18   4/18/07 10:32a kagrawal
* PR 28561: Proper RM for CapPortAllocation
* 
* Hydra_Software_Devel/17   4/10/07 12:01p nitinb
* PR 29381: Corrected value of macro BRAP_P_MAX_PATHS_IN_A_CHAN for 3563
* 
* Hydra_Software_Devel/16   4/3/07 1:50p sushmit
* PR 28714: Multiple PP branch support
* 
* Hydra_Software_Devel/15   3/9/07 3:20p sushmit
* PR 26864: Increased BRAP_RM_P_MAX_DEC_CHANNELS to 2 to support one
* decode & one passthrough channel
* 
* Hydra_Software_Devel/14   3/2/07 11:21a kagrawal
* PR 27912: Added PI support for Speaker Management
* - Added ePath in resource request structure
* 
* Hydra_Software_Devel/13   2/22/07 12:26p bhanus
* PR 28040 : Modified code for 7440 B0 porting
* 
* Hydra_Software_Devel/12   2/16/07 1:10p bhanus
* PR 27912 : Modified for  Static Downmixing Support.
* 
* Hydra_Software_Devel/11   2/6/07 1:17p kagrawal
* PR 25607: Misc fixes for supporting dual decode
* 
* Hydra_Software_Devel/10   1/31/07 4:13p kagrawal
* PR 21368: Added software workaround for DP can mis-align bug
* 
* Hydra_Software_Devel/9   1/4/07 11:34a sushmit
* PR 25607: Temporary setting of ENC DSP ID to 1 till DSP PI gets fixed.
* 
* Hydra_Software_Devel/8   1/4/07 2:05p bhanus
* PR 25607: Modified for Multichannel mixing
* 
* Hydra_Software_Devel/7   12/11/06 3:51p sushmit
* PR 25607: Fixing DSTCH Offsets for 3563
* 
* Hydra_Software_Devel/5   12/4/06 1:45p sushmit
* PR 26079: Removed compiler warnings for 3563
* 
* Hydra_Software_Devel/RAP_3563_Merge/1   12/4/06 1:44p sushmit
* PR 26079: Remove warnings in RAP module
* 
* Hydra_Software_Devel/4   11/22/06 2:21p kagrawal
* PR 26079: Removed compilation warnings due to C++ type comments
* 
* Hydra_Software_Devel/3   10/6/06 5:47p kagrawal
* PR 24717: Merged 7440 devel branch to Hydra_Software_Devel
* 
* Hydra_Software_Devel/2   9/18/06 3:46p kagrawal
* PR 20654: Fixed compilation warnings
* 
* Hydra_Software_Devel/1   8/17/06 12:07p speter
* PR 20654: Added from BLR_RAP_7440A0_Bring_Up_Branch
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/11   8/10/06 3:47p kagrawal
* PR 20654: Resolved linking error
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/10   8/2/06 9:16p kagrawal
* PR 20654: Fixed compilation error
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/9   8/1/06 3:03p kagrawal
* PR 20654: Corrected resource request and grant structure for output
* port
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/8   7/26/06 10:08p kagrawal
* PR 20654: Added BRAP_RM_P_DEC_DSP_ID   and BRAP_RM_P_ENC_DSP_ID
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/7   7/19/06 4:32a kagrawal
* PR 20654: Added bAllocDSP to the ResourceRequest structure
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/6   7/18/06 3:16p bhanus
* PR 20654: Added Error conditionin BRAP_RM_P_UpdateResrcGrant
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/5   7/18/06 2:54p bhanus
* PR 20654: Added API to initialize the Resource request structure and
* Update Resource grant structure.
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/4   7/13/06 8:16p bhanus
* PR 20654: Added code for resource allocation of SRC and Mixer.
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/3   7/12/06 6:30p kagrawal
* PR 20654: Added Free() along with various sub alloc functions
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/2   7/11/06 12:08p kagrawal
* PR 20654: Added Clearcase Log for the initial version
***************************************************************************/

/*=************************ Module Overview ********************************
Resource Manager is an internal module of Raptor audio PI.

For various audio operations, Raptor audio PI makes use of following hardware 
/firmware/memory resources:
1. DSP contexts (Firmware resource)
2. DRAM ring buffers (Memory resource)
3. Source/Destination channels (FMM hardware resource)
4. SRCs (FMM hardware resource)
4. Mixers (FMM hardware resource)
5. SPDIF-Formatter (FMM hardware resource)
6. Output ports (FMM hardware resource)
7. Input/capture rate control ports (FMM hardware resource)

Above resources are shared for following audio operations:
1. Live Decode/Pass Thru/SRC/Simulmode in DSP       *TODO: Is simul mode reqd?? 
2. PCM Playback from hard-disk/memory
3. PCM Capture
4. Encode 

Resource Manager maintains above resources and provides them to Audio Manager 
as per the request. It allocates resources, for optimum usage. However, Resource
Manager doesn't know what all resources are getting used for a particular 
instance of audio operation. It is the responsibility of Audio Manager to 
provide a list of all the resources required. It is also Audio Manager's 
responsibility to provide a list of all the resources to be freed once an 
instance of audio operation is closed.

Other modules interact with Resource Manager for following purposes.
1. When a new audio channel (instance of audio operation) is formed (or closed).
   Audio Manager requests Resource Manager to allocate (or free) the required 
   resources for this operation.
2. Adding/Removing of the output ports.
3. For mixing operation.

***************************************************************************/
#ifndef _BRAP_RM_PRIV_H__
#define _BRAP_RM_PRIV_H__

#include "brap_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
Summary:
	Resource Manager module handle.
***************************************************************************/
typedef struct BRAP_RM_P_Object			*BRAP_RM_Handle;

/***************************************************************************
Summary:
	Used in array of resource indexes. If the array size is more than
	number of resource index entries, remaining entries are initialized
	to this macro value.
***************************************************************************/
#define BRAP_RM_P_INVALID_INDEX		    ((unsigned int)(-1))

/***************************************************************************
Summary:
	Macros defining maximum number of resources available.
***************************************************************************/
#if (BRAP_3548_FAMILY == 1)
#define BRAP_RM_P_MAX_DSPS				1	/* Total Number of DSP */	
#define BRAP_RM_P_MAX_CXT_PER_DSP		6	/* Maximum context per DSP*/ 
#define BRAP_RM_P_DEC_DSP_ID			0	/* DSP on which decode happens */
#define BRAP_RM_P_ENC_DSP_ID			1	/* DSP on which encode/transcode
											   happens */
#define BRAP_RM_P_MAX_FW_TASK_PER_DSP   4   /* Maximum number of FW tasks that											   
                                               can exist together in a DSP */
#define BRAP_RM_P_MAX_FMMS				1	/* Total number of FMMs */

/* Buffer block */
#define BRAP_RM_P_MAX_RBUFS				24	/* Total Ring buffers */
#define BRAP_RM_P_MAX_SRC_CHANNELS		8	/* Total Source Channel Fifos*/
#if (BCHP_VER==BCHP_VER_A0)
#define BRAP_RM_P_MAX_DST_CHANNELS		4	/* Total Dest Channel Fifos */
#else /* for B0 */
#define BRAP_RM_P_MAX_DST_CHANNELS		6	/* Total Dest Channel Fifos */
#endif
#define BRAP_RM_P_MAX_INTERNAL_CAPPORTS 4	/* Total Internal CapPorts */

#define BRAP_RM_P_TOTAL_SRCCH_DSTCH		(BRAP_RM_P_MAX_SRC_CHANNELS + \
						                    BRAP_RM_P_MAX_DST_CHANNELS)
												/* Total Src + Dest Fifos */
#define BRAP_RM_P_MAX_RBUFS_PER_SRCCH	2	/* Maximum rbuf per src channel */
#define BRAP_RM_P_MAX_RBUFS_PER_DSTCH	2	/* Maximum rbuf per dest channel */

/* SRC block */
#define BRAP_RM_P_MAX_SRC_BLCK			2	/* Num of SRC blocks in FMM */
#define BRAP_RM_P_MAX_SRC_PER_SRC_BLCK	12	/* Num of SRCs in each SRC block */
#define BRAP_RM_P_MAX_SRC_IN_CASCADE	3	/* Max SRCs that can be cascaded 
												back to back e.g. 12 -> 196 */

/* DP block */
#define BRAP_RM_P_MAX_DP_BLCK				1	/* Number of DP blocks in FMM */
#define BRAP_RM_P_MAX_MIXER_PER_DP_BLCK	8	/* Number of mixers in each DP 
												block */
#define BRAP_RM_P_MAX_MIXER_INPUTS		8	/* Number of inputs to a mixer */
#define BRAP_RM_P_MAX_MIXER_OUTPUTS		2	/* Maximum number of outputs to from
												a mixer */
/* IOP block */
#define BRAP_RM_P_MAX_FS_TMG_SOURCES    4   /* Number of FS Timing Sources ex:
                                               FS0, FS1, FS2 and FS3 */
#define BRAP_RM_P_MAX_SPDIFFMS			1	/* Max SPDIF formaters */
#define BRAP_RM_P_MAX_SPDIFFM_STREAMS	2	/* Max streams across all SPDIFFM */ 
#define BRAP_RM_P_MAX_OUTPUTS			BRAP_OutputPort_eMax 
											/* Max number of output ports */
#define BRAP_RM_P_MAX_RBUFS_PER_PORT	BRAP_RM_P_MAX_RBUFS_PER_SRCCH
											/* Maximum ring buffers per output 
											   port. HDMI in an exception. */
#define BRAP_RM_P_MAX_OP_CHANNELS		8	/* Max o/p audio channels */
#define BRAP_RM_P_MAX_OP_CHANNEL_PAIRS	(BRAP_RM_P_MAX_OP_CHANNELS >> 1)	
										   	/* Max o/p audio channels pairs */
/* Others related to FMM */
#define BRAP_RM_P_MAX_PARALLEL_PATHS 	6	/* Max parallel paths supported in
												the system */
#define BRAP_RM_P_MAX_MIXING_LEVELS		7	/* Max SRC-Mixing levels in the 
												system */

#if BRAP_P_CUSTOM_MIXER_REQUIREMENT												
#define BRAP_RM_P_MAX_RSRCS_CONNECTED_TO_SAME_SRC 6
												/* Max resources that can be fed
												by a single SRC */
#else
#define BRAP_RM_P_MAX_RSRCS_CONNECTED_TO_SAME_SRC 4
												/* Max resources that can be fed
												by a single SRC */
#endif

#define BRAP_P_MAX_PATHS_IN_A_CHAN		8

/* Max channel types */
#define BRAP_RM_P_MAX_DEC_CHANNELS		3	/* Max decode channels */ 
#define BRAP_RM_P_MAX_PCM_CHANNELS		4	/* Max PCM playback channels */ 
#define BRAP_RM_P_MAX_CAP_CHANNELS		4	/* Max capture channels */ 

/* Encoder support in RM */ 
/* Make BRAP_RM_P_MAX_ENC_CHANNELS zero later */
#define BRAP_RM_P_MAX_ENC_CHANNELS			1	/* Max encode channels */
#define BRAP_RM_P_MAX_RBUFS_PER_ENC_INPUT	6	/* Maximum ring buffers per 
													encoder input channel */

/* Macros for maximum number of supported decode and passthru operations.
 * Decode part of simul mode is counted in BRAP_P_MAX_DECODE_OPERATIONS
 * and pass thru part of simul mode is count in BRAP_P_MAX_PASSTHRU_OPERATIONS
 */
#define BRAP_RM_P_MAX_DECODE_SUPPORTED		1	
#define BRAP_RM_P_MAX_PASSTHRU_SUPPORTED	1	
#define BRAP_RM_P_MAX_ENCODE_SUPPORTED		0

/* Internal Destination (Internal Capture port)*/
#define BRAP_RM_P_MAX_INTERNAL_DST			4

/* Simul mode linkage */
#define BRAP_RM_P_MAX_LINKAGE_SUPPORTED     1

#define BRAP_RM_P_MAX_MIXER_PB_FCI_ID       32

/* AdaptRateCtrl block */
#define BRAP_RM_P_MAX_ADAPTRATE_CTRL			4


#elif (BRAP_7405_FAMILY == 1)
#define BRAP_RM_P_MAX_DSPS				1	/* Total Number of DSP */	
#define BRAP_RM_P_MAX_CXT_PER_DSP		6	/* Maximum context per DSP*/ 
                                            						/* TODO: check with firmware team */
#define BRAP_RM_P_DEC_DSP_ID				0	/* DSP on which decode happens */
#define BRAP_RM_P_ENC_DSP_ID				0	/* DSP on which encode/transcode
												happens */
#define BRAP_RM_P_MAX_FW_TASK_PER_DSP   3       /* Maximum number of FW tasks that
                                                                                          can exist together in a DSP */
#define BRAP_RM_P_MAX_FMMS				1	/* Total number of FMMs */

/* Buffer block */
#if (BRAP_7420_FAMILY == 1)
#define BRAP_RM_P_MAX_RBUFS				28	/* Total Ring buffers */
#define BRAP_RM_P_MAX_SRC_CHANNELS		10	/* Total Source Channel Fifos*/
#define BRAP_RM_P_MAX_INTERNAL_CAPPORTS 5	/* Total Internal CapPorts */
/* AdaptRateCtrl block */
#define BRAP_RM_P_MAX_ADAPTRATE_CTRL	8
#define BRAP_RM_P_MAX_IOP_FCI_CFG    18   /* Number of IOP FCI CFG*/	
#elif (BRAP_7550_FAMILY == 1)
#define BRAP_RM_P_MAX_RBUFS				18	/* Total Ring buffers */
#define BRAP_RM_P_MAX_SRC_CHANNELS		9	/* Total Source Channel Fifos*/
#define BRAP_RM_P_MAX_ADAPTRATE_CTRL    8   /* AdaptRateCtrl block */
#define BRAP_RM_P_MAX_IOP_FCI_CFG    11   /* Number of IOP FCI CFG*/	
#else
#define BRAP_RM_P_MAX_RBUFS				24	/* Total Ring buffers */
#define BRAP_RM_P_MAX_SRC_CHANNELS		8	/* Total Source Channel Fifos*/
#define BRAP_RM_P_MAX_INTERNAL_CAPPORTS 4	/* Total Internal CapPorts */
/* AdaptRateCtrl block */
#define BRAP_RM_P_MAX_ADAPTRATE_CTRL	4
#define BRAP_RM_P_MAX_IOP_FCI_CFG    17   /* Number of IOP FCI CFG*/	
#endif
#define BRAP_RM_P_MAX_DST_CHANNELS		4	/* Total Dest Channel Fifos */

#define BRAP_RM_P_TOTAL_SRCCH_DSTCH		(BRAP_RM_P_MAX_SRC_CHANNELS + \
						BRAP_RM_P_MAX_DST_CHANNELS)
												/* Total Src + Dest Fifos */
#define BRAP_RM_P_MAX_RBUFS_PER_SRCCH	2	/* Maximum rbuf per src channel */
#define BRAP_RM_P_MAX_RBUFS_PER_DSTCH	2	/* Maximum rbuf per dest channel */

/* SRC block */
#define BRAP_RM_P_MAX_SRC_BLCK			1	/* Num of SRC blocks in FMM */
#define BRAP_RM_P_MAX_SRC_PER_SRC_BLCK	12	/* Num of SRCs in each SRC block */
#define BRAP_RM_P_MAX_SRC_IN_CASCADE	2	/* Max SRCs that can be cascaded 
												back to back e.g. 12 -> 196 */

/* DP block */
#define BRAP_RM_P_MAX_DP_BLCK				1	/* Number of DP blocks in FMM */
#define BRAP_RM_P_MAX_MIXER_PER_DP_BLCK	8	/* Number of mixers in each DP 
												block */
#define BRAP_RM_P_MAX_MIXER_INPUTS		8	/* Number of inputs to a mixer */
#define BRAP_RM_P_MAX_MIXER_OUTPUTS		2	/* Maximum number of outputs to from
												a mixer */
#define BRAP_RM_P_MAX_MIXER_PB_FCI_ID   32
/* IOP block */
#define BRAP_RM_P_MAX_FS_TMG_SOURCES    4   /* Number of FS Timing Sources ex:
                                               FS0, FS1, FS2 and FS3 */
#define BRAP_RM_P_MAX_SPDIFFMS			1	/* Max SPDIF formaters */
#define BRAP_RM_P_MAX_SPDIFFM_STREAMS	2	/* Max streams across all SPDIFFM */ 
#define BRAP_RM_P_MAX_OUTPUTS			BRAP_OutputPort_eMax 
											/* Max number of output ports */
#define BRAP_RM_P_MAX_RBUFS_PER_PORT	BRAP_RM_P_MAX_RBUFS_PER_SRCCH
											/* Maximum ring buffers per output 
											   port. HDMI in an exception. */
#define BRAP_RM_P_MAX_OP_CHANNELS		8	/* Max o/p audio channels */
#define BRAP_RM_P_MAX_OP_CHANNEL_PAIRS	(BRAP_RM_P_MAX_OP_CHANNELS >> 1)	
										   	/* Max o/p audio channels pairs */

/* Others related to FMM */
#define BRAP_RM_P_MAX_PARALLEL_PATHS 	4	/* Max parallel paths supported in
												the system */
#define BRAP_RM_P_MAX_MIXING_LEVELS		4	/* Max SRC-Mixing levels in the 
												system */
#if (BRAP_7550_FAMILY == 1)
#define BRAP_RM_P_MAX_RSRCS_CONNECTED_TO_SAME_SRC 1
												/* Max resources that can be fed
												by a single SRCCH, not SRC.
												Same Macro is used to minimize
												the code changes*/
#else
#define BRAP_RM_P_MAX_RSRCS_CONNECTED_TO_SAME_SRC 4
												/* Max resources that can be fed
												by a single SRC */
#endif

#define BRAP_P_MAX_PATHS_IN_A_CHAN		BRAP_P_MAX_DST_PER_RAPCH

/* Max channel types */
#define BRAP_RM_P_MAX_DEC_CHANNELS		3	/* Max decode channels */ 
#define BRAP_RM_P_MAX_PCM_CHANNELS		4	/* Max PCM playback channels */ 
#define BRAP_RM_P_MAX_CAP_CHANNELS		4	/* Max capture channels */ 
#ifdef RAP_REALVIDEO_SUPPORT
#define BRAP_RM_P_MAX_VIDEO_DEC_CHANNELS	1	/* Max decode channels */ 
#endif
#ifdef RAP_GFX_SUPPORT
#define BRAP_RM_P_MAX_GFX_CHANNELS		2	/* Max capture channels */ 
#endif
#ifdef RAP_SCM_SUPPORT 
#define BRAP_RM_P_MAX_SCM_CHANNELS		1	/* Max SCM channels */ 
#endif


/* Encoder support in RM */ 
/* Make BRAP_RM_P_MAX_ENC_CHANNELS zero later */
#define BRAP_RM_P_MAX_ENC_CHANNELS			1	/* Max encode channels */
#define BRAP_RM_P_MAX_RBUFS_PER_ENC_INPUT	6	/* Maximum ring buffers per 
													encoder input channel */

/* Macros for maximum number of supported decode and passthru operations.
 * Decode part of simul mode is counted in BRAP_P_MAX_DECODE_OPERATIONS
 * and pass thru part of simul mode is count in BRAP_P_MAX_PASSTHRU_OPERATIONS
 */
#define BRAP_RM_P_MAX_DECODE_SUPPORTED		1	
#define BRAP_RM_P_MAX_PASSTHRU_SUPPORTED	1	
#define BRAP_RM_P_MAX_ENCODE_SUPPORTED		0

/* Internal Destination (Internal Capture port)*/
#define BRAP_RM_P_MAX_INTERNAL_DST			4

#else
#error "Not support chip type"
#endif

/***************************************************************************
Summary:
    This enumeration lists the various usage paths that can exist in a 
    channel.

    Note: This declaration has been moved here from brap_priv.h as this is
    required to be used in RM data structure and brap_priv.h gets included 
    after rm_priv.h.
***************************************************************************/ 

typedef enum BRAP_P_UsgPath
{   
    BRAP_P_UsgPath_eDecodePcm,      /* Starts at DSP0 and ends at OP/CAP */
    BRAP_P_UsgPath_eDecodePcmPostMixing,
    BRAP_P_UsgPath_eDecodeCompressPostMixing,
    BRAP_P_UsgPath_eDecodeCompress, /* Starts at DSP0 and ends at OP */
    BRAP_P_UsgPath_eCapture,        /* Starts at CAP and ends at RBuf/OP */
    BRAP_P_UsgPath_ePPBranchPostMixing,                                       
    BRAP_P_UsgPath_ePPBranch,       /* Starts after Decode & ends at OP.*/                                       
    BRAP_P_UsgPath_eMixPath,    /* This Path is created if Similar branch in network goes 
                                                            to 2 different association */                                                                
    BRAP_P_UsgPath_eDownmixedPath,/*This Path is created if Destination to the branch is
                                                                    taking Stereo Downmixed data*/
    BRAP_P_UsgPath_eDownmixedMixPath,/*This Path is created if if Similar branch in 
                                                                    network goes to 2 different association and 
                                                                    Destination to the branch is taking Stereo 
                                                                    Downmixed data*/
    BRAP_P_UsgPath_eSharedPP,    /* This Path is created if for the O/P ports having indep delay =0 for PCM playback/Capture. In this path is shared till SRC*/                                                                    
#ifdef RAP_REALVIDEO_SUPPORT
    BRAP_P_UsgPath_eVideoDecode,    /* Video Decode*/          
#endif    
#ifdef RAP_GFX_SUPPORT
    BRAP_P_UsgPath_eGfx,    /* Gfx Path*/                                                                        
#endif    
#ifdef RAP_SCM_SUPPORT
	BRAP_P_UsgPath_eScm,    /* SCM Path*/                                                                        
#endif	
    BRAP_P_UsgPath_eMax             /* Invalid or max usage path */                           
}BRAP_P_UsgPath;

/***************************************************************************
Summary:
	Enumerations for states of an object
***************************************************************************/
typedef enum BRAP_RM_P_ObjectState
{
	BRAP_RM_P_ObjectState_eFree,	/* Free state */
	BRAP_RM_P_ObjectState_eBusy	    /* Busy state */
} BRAP_RM_P_ObjectState;

/***************************************************************************
Summary:
	SRC resource grant structure. This is a part of the main resource grant 
	structure.
***************************************************************************/
typedef struct BRAP_RM_P_SrcGrant
{
	unsigned int	uiSrcBlkId;     /* Alloted SRC block index */
    unsigned int    uiSrcId;        /* Alloted SRC index */
}BRAP_RM_P_SrcGrant;

/***************************************************************************
Summary:
	Mixer resource grant structure. This is a part of the main resource 
	grant structure.
***************************************************************************/
typedef struct BRAP_RM_P_MixerGrant
{
    unsigned int	uiDpId;         /* Alloted DP index */
    unsigned int    uiMixerId;      /* Alloted mixer index */
    unsigned int    uiMixerInputId[BRAP_RM_P_MAX_OP_CHANNEL_PAIRS];
                                    /* Alloted Mixer input indices */
    unsigned int    uiMixerOutputId[BRAP_RM_P_MAX_MIXER_OUTPUTS];
                                    /* Alloted mixer output indices */
}BRAP_RM_P_MixerGrant;

/***************************************************************************
Summary:
	Combined resource grant structure for SRC and mixer. This is a sub-
	structure in the main resource grant structure.
***************************************************************************/
typedef struct BRAP_RM_P_SrcMixerGrant
{
    BRAP_RM_P_SrcGrant      sSrcGrant[BRAP_RM_P_MAX_SRC_IN_CASCADE]
                                     [BRAP_RM_P_MAX_OP_CHANNEL_PAIRS]
                                     [BRAP_RM_P_MAX_PARALLEL_PATHS];
                                    /* Structure containing allotted SRC Ids */ 
    BRAP_RM_P_MixerGrant    sMixerGrant[BRAP_RM_P_MAX_OP_CHANNEL_PAIRS]
                                       [BRAP_RM_P_MAX_PARALLEL_PATHS];
                                    /* Structure containing allotted Mixer Ids*/
}BRAP_RM_P_SrcMixerGrant;

/***************************************************************************
Summary:
	Ouput port and corresponding SPDIFFM resource grant structure. This is a 
	sub-structure in the main resource grant structure.
***************************************************************************/
typedef struct BRAP_RM_P_SrcEqGrant
{
	unsigned int	uiSrcBlkId;     /* Alloted SRC block index */
    unsigned int    uiSrcId;        /* Alloted SRC index */
}BRAP_RM_P_SrcEqGrant;

/***************************************************************************
Summary:
	Ouput port and corresponding SPDIFFM resource grant structure. This is a 
	sub-structure in the main resource grant structure.
***************************************************************************/
typedef struct BRAP_RM_P_OpPortGrant
{
    unsigned int        uiSpdiffmId;        /* Alloted SPDIF Formater index */
    unsigned int        uiSpdiffmStreamId;  /* Alloted SPDIF Formater stream 
                                              index */
    BRAP_OutputPort     eOutputPort;        /* Alloted output port */
}BRAP_RM_P_OpPortGrant;

/***************************************************************************
Summary:
	FS Timing Source grant structure. This is one of the sub-structures in 
	the  main resource grant structure.
***************************************************************************/
typedef struct BRAP_RM_P_FsTmgSrcGrant
{
    unsigned int        uiFsTmgSrc;     /* Alloted FS Timing Source.  */
}BRAP_RM_P_FsTmgSrcGrant;

/***************************************************************************
Summary:
	Capture port resource grant structure. This is a sub-structure in the 
	main resource grant structure.
***************************************************************************/
typedef struct BRAP_RM_P_CapPortGrant
{
    BRAP_CapInputPort           eCapPort;       /* Alloted Capture port */ 
}BRAP_RM_P_CapPortGrant;

/***************************************************************************
Summary:
	Resource grant structure. 

	Note: 
	1. Every time there is a resource request from the Audio Manager, 
	corresponding resources will be allocated and returned to the AM. It is AM's
	duty to add the newly allocated resources into it's already allocated 
	resource list.

	2. All unused entries will be initialized to BRAP_RM_P_INVALID_INDEX. 	
***************************************************************************/
typedef struct BRAP_RM_P_ResrcGrant
{
	unsigned int	        uiDspId;		/* Allotted DSP index */
	unsigned int	        uiDspContextId;	/* Allotted DSP context index */
	unsigned int	        uiFmmId;		/* Allotted FMM index */
    unsigned int	        uiRbufId[BRAP_RM_P_MAX_OP_CHANNELS]; /* Allotted RBuf Ids */
    unsigned int	        uiSrcChId[BRAP_RM_P_MAX_OP_CHANNEL_PAIRS];/* Allotted Source Fifo Ids */                                            
    unsigned int	        uiDstChId[BRAP_RM_P_MAX_OP_CHANNEL_PAIRS];/* Allotted Destination Fifo Ids */
    unsigned int	        uiAdaptRateCtrlId[BRAP_RM_P_MAX_OP_CHANNEL_PAIRS];/* Allotted Rate manger block Ids */    
    BRAP_RM_P_SrcMixerGrant sSrcMixerGrnt[BRAP_RM_P_MAX_MIXING_LEVELS];
                                            /* Structure containing allotted 
                                               SRC and Mixer Ids */    
    BRAP_RM_P_SrcEqGrant    sSrcEqGrant[BRAP_RM_P_MAX_SRC_IN_CASCADE]
                                       [BRAP_RM_P_MAX_OP_CHANNEL_PAIRS]
                                       [BRAP_RM_P_MAX_PARALLEL_PATHS];
                                            /* Structure containing allotted 
                                               Equalizer SRCs */
    BRAP_RM_P_OpPortGrant   sOpPortGrnt[BRAP_RM_P_MAX_OP_CHANNEL_PAIRS];
                                            /* Stucture containing allotted 
                                               Output Ports and SPDIFFM Ids */
    BRAP_RM_P_CapPortGrant  sCapPortGrnt[BRAP_RM_P_MAX_OP_CHANNEL_PAIRS]
                                        [BRAP_P_MAX_OUT_PARALLEL_FMM_PATH];
                                            /* Stucture containing allotted 
                                               Capture Ports */
    BRAP_RM_P_FsTmgSrcGrant sFsTmgGrnt;     /* Alloted FS Timing Source */
} BRAP_RM_P_ResrcGrant;

/***************************************************************************
Summary:
	Ring buffer resource request structure. This is a sub-structure in the
	main resource request structure.
***************************************************************************/
typedef struct BRAP_RM_P_RbufReq
{
    bool                bAllocate;      /* TRUE indicates a valid request */
    BRAP_BufDataMode    eBufDataMode;   /* Buffer data mode */
}BRAP_RM_P_RbufReq;

/***************************************************************************
Summary:
	Source Fifo (or Source Channel) resource request structure. This is a 
	sub-structure in the main resource request structure.
***************************************************************************/
typedef struct BRAP_RM_P_SrcChReq
{
    bool                bAllocate;      /* TRUE indicates a valid request */
}BRAP_RM_P_SrcChReq;

/***************************************************************************
Summary:
	Rate manger block resource request structure. This is a 
	sub-structure in the main resource request structure.
***************************************************************************/
typedef struct BRAP_RM_P_AdaptRateCtrlReq
{
    bool                bAllocate;      /* TRUE indicates a valid request */
}BRAP_RM_P_AdaptRateCtrlReq;

/***************************************************************************
Summary:
	SRC resource request structure. This is a part of the main resource 
	request structure.
***************************************************************************/
typedef struct BRAP_RM_P_SrcReq
{
    bool                bAllocate;      /* TRUE indicates a valid request */
    BRAP_RM_P_SrcGrant  sReallocateSrc; /* AM can indicate to reallocate an
                                           SRC using this. Else, fill it up 
                                           with BRAP_RM_P_INVALID_INDEX */
}BRAP_RM_P_SrcReq;

/***************************************************************************
Summary:
	SRC Equalizer resource request structure. This is a part of the main resource 
	request structure.
***************************************************************************/
typedef struct BRAP_RM_P_SrcEqReq
{
    bool                    bAllocate;          /* TRUE indicates a valid request */
    unsigned int            uiBlkId;            /* Indicates SRC Block from where
                                                   an SRC need to be allocated */
    BRAP_RM_P_SrcEqGrant    sReallocateSrcEq;   /* AM can indicate to reallocate this SRCEq */
}BRAP_RM_P_SrcEqReq;

/***************************************************************************
Summary:
	Mixer resource request structure. This is a part of the main resource 
	request structure.
***************************************************************************/
typedef struct BRAP_RM_P_MixerReq
{
    bool                    bAllocate;          /* TRUE indicates a valid 
                                                   request */
    unsigned int            uiNumNewInput;      /* Number of mixer input */
    unsigned int            uiNumNewOutput;     /* Number of mixer output */
    bool                    bInputChPair[BRAP_RM_P_MAX_OP_CHANNEL_PAIRS];
    BRAP_RM_P_MixerGrant    sReallocateMixer;   /* AM can indicate to reallocate
                                                   a mixer along with its inputs
                                                   and outputs using this. AM 
                                                   can also request to 
                                                   reallocate a mixer but with 
                                                   fresh inputs and/or outputs.
                                                   If AM is interested in having
                                                   an entirely fresh mixer, it 
                                                   has to fill up this strucure
                                                   with BRAP_RM_P_INVALID_INDEX 
                                                   */
    unsigned int            uiMixerOutputId[BRAP_RM_P_MAX_MIXER_OUTPUTS];
                                                /* Alloted mixer output indices */

}BRAP_RM_P_MixerReq;

/***************************************************************************
Summary:
	Common resource request structure for SRC and Mixer. This is a 
	sub-structure in the main resource request structure.
***************************************************************************/
typedef struct BRAP_RM_P_SrcMixerReq
{
    BRAP_RM_P_SrcReq    sSrcReq[BRAP_RM_P_MAX_SRC_IN_CASCADE]
                               [BRAP_RM_P_MAX_OP_CHANNEL_PAIRS]
                               [BRAP_RM_P_MAX_PARALLEL_PATHS];
                                        /* SRC resource request */
    BRAP_RM_P_MixerReq  sMixerReq[BRAP_RM_P_MAX_OP_CHANNEL_PAIRS]
                                 [BRAP_RM_P_MAX_PARALLEL_PATHS];   
                                        /* Mixer resource request */
}BRAP_RM_P_SrcMixerReq;

/***************************************************************************
Summary:
	Output port resource request structure. This is a sub-structure in the
	main resource request structure. There is no explicit resource request
	for SPDIFFM. It is decided based on the output port type and MuxSelect 
	provided using this structure.
***************************************************************************/
typedef struct BRAP_RM_P_OpPortReq
{
    bool                bAllocate;      /* TRUE indicates a valid request */    
	BRAP_OutputPort		eOpPortType;	/* Output port type */
	BRAP_OutputPort		eMuxSelect;		/* If output port type is MAI then 
	                                       which input to use. */
}BRAP_RM_P_OpPortReq;

/***************************************************************************
Summary:
	Structure for resource request associated with capture port
***************************************************************************/
typedef struct BRAP_RM_P_CapPortReq
{
    bool                    bAllocate;  /* TRUE indicates a valid request */    
    BRAP_CapInputPort       eCapPort;   /* BRAP_CapInputPort_eMax indicates 
                                           request for fresh internal cap-ports.
                                           Else explicit request for a 
                                           particular cap-port */
} BRAP_RM_P_CapPortReq;

/***************************************************************************
Summary:
	Destination Fifo (or Destination Channel) resource request structure. 
	This is a sub-structure in the main resource request structure.
***************************************************************************/
typedef struct BRAP_RM_P_DstChReq
{
    bool                bAllocate;      /* TRUE indicates a valid request */
}BRAP_RM_P_DstChReq;

/***************************************************************************
Summary:
	FS Timing Source resource request structure. This is used to provide FS 
	Timing Clock to Dummy Sinks and Internal Capture Ports.
	
	This is a sub-structure in the main resource request structure.
***************************************************************************/
typedef struct BRAP_RM_P_FsTmgSrcReq
{
    bool                bAllocate;      /* TRUE indicates a valid request */
    unsigned int        uiFsTmgSrcId;   /* If bAllocate is TRUE
                                           then BRAP_RM_P_INVALID_INDEX 
                                           indicates a fresh FS Timing Src 
                                           request while a valid uiFsTmgSrcId 
                                           indicates a re-allocation request */
}BRAP_RM_P_FsTmgSrcReq;
    
/***************************************************************************
Summary:
	Structure for requesting required resources. 
***************************************************************************/
typedef struct BRAP_RM_P_ResrcReq
{
    BRAP_ChannelType	    eChType;	/* Channel type */
    BRAP_ChannelSubType     eChSubType; /* Sub-channel type */
    BRAP_P_UsgPath          ePath;      /* Path type for which resource request
                                           has been sent */
    bool                    bAllocateDSP;
                                        /* true to allocate DSP, else false */
    BRAP_RM_P_RbufReq       sRbufReq[BRAP_RM_P_MAX_OP_CHANNEL_PAIRS];
                                        /* Rbuf resource request */
    BRAP_RM_P_SrcChReq      sSrcChReq[BRAP_RM_P_MAX_OP_CHANNEL_PAIRS];
                                        /* SrcCh resource request */    
    BRAP_RM_P_AdaptRateCtrlReq   sAdaptRateCtrlReq[BRAP_RM_P_MAX_OP_CHANNEL_PAIRS];
                                        /* Rate manager resource request */                                            
    BRAP_RM_P_SrcMixerReq   sSrcMixerReq[BRAP_RM_P_MAX_MIXING_LEVELS];
                                        /* SRC and Mixer resource request */

    BRAP_RM_P_SrcEqReq      sSrcEqReq[BRAP_RM_P_MAX_SRC_IN_CASCADE]
                                     [BRAP_RM_P_MAX_OP_CHANNEL_PAIRS]
                                     [BRAP_RM_P_MAX_PARALLEL_PATHS];
                                            
    BRAP_RM_P_OpPortReq     sOpReq[BRAP_RM_P_MAX_OP_CHANNEL_PAIRS];
                                        /* Output port resource request */
    BRAP_RM_P_CapPortReq    sCapReq[BRAP_RM_P_MAX_OP_CHANNEL_PAIRS]
                                   [BRAP_P_MAX_OUT_PARALLEL_FMM_PATH];
                                        /* Capture Port request */
    BRAP_RM_P_DstChReq      sDstChReq[BRAP_RM_P_MAX_OP_CHANNEL_PAIRS];
                                        /* Destination Channel Request */
    BRAP_RM_P_FsTmgSrcReq   sFsTmgSrcReq;
                                        /* FS Timing Source resource request */
}BRAP_RM_P_ResrcReq;    


/***************************************************************************
Summary:
	Structure for SPDIF Formater usage
***************************************************************************/
typedef struct BRAP_RM_P_SpdiffmUsage
{ 
    unsigned int			uiUsageCount[BRAP_RM_P_MAX_SPDIFFM_STREAMS];	
								/* Total number of Clients using this SPDIFFM
								   channel. Zero indicates this is free */
} BRAP_RM_P_SpdiffmUsage;

/***************************************************************************
Summary:
	Structure for CapPort usage
***************************************************************************/
typedef struct BRAP_RM_P_CapPortUsage
{ 
    unsigned int			uiUsageCount; 
                                /* Total number of Clients using this CapPort.
								   Zero indicates this is free */
}BRAP_RM_P_CapPortUsage;


/***************************************************************************
Summary:
	Structure for mixer usage
***************************************************************************/
typedef struct BRAP_RM_P_MixerUsage
{
	unsigned int	        uiInputUsgCnt[BRAP_RM_P_MAX_MIXER_INPUTS];
								/* Total number of Clients using this mixer 
								   input. Zero indicates this is free */
	unsigned int	        uiOutputUsgCnt[BRAP_RM_P_MAX_MIXER_OUTPUTS];
								/* Total number of Clients using this mixer 
								   output. Zero indicates this is free */
} BRAP_RM_P_MixerUsage;

/***************************************************************************
Summary:
	Structure for SRC usage
***************************************************************************/
typedef struct BRAP_RM_P_SrcUsage
{
	unsigned int			uiUsageCount;	
								/* Total number of Clients using this SRC.
								   Zero indicates this is free */
    bool                    bSrcEq;	
                                /* If the allocated SRC is used as an 
                                   Equalizer SRC(SRC type - IIR) */
}BRAP_RM_P_SrcUsage;

/***************************************************************************
Summary:
	Structure for FS Timing Source usage
***************************************************************************/
typedef struct BRAP_RM_P_FsTmgSrcUsage
{
	unsigned int			uiUsageCount;	
								/* Total number of Clients using this FS Timing
								   Source. Zero indicates this is free */
}BRAP_RM_P_FsTmgSrcUsage;

/***************************************************************************
Summary:
	Structure for DSP object and DSP context usage
***************************************************************************/
typedef struct BRAP_RM_P_DspUsage
{
	BRAP_RM_P_ObjectState	eContext[BRAP_RM_P_MAX_CXT_PER_DSP];
								            /* Array to maintain the states 
								               of the DSP contexts */
	unsigned int			uiDecCxtCount;	/* Number of decoder contexts
											   currently opened in DSP */
	unsigned int			uiSrcCxtCount;	/* Number of SRC contexts 
											   currently opened in DSP */ 
                                            /* TODO: Is this reqd?? */
	unsigned int			uiPtCxtCount;	/* Number of pass-thru contexts
											   currently opened in DSP */ 
} BRAP_RM_P_DspUsage;

/***************************************************************************
Summary:
	Handle structure for Resource Manager.
***************************************************************************/
typedef struct BRAP_RM_P_Object
{
	BRAP_RM_P_DspUsage		sDspUsage[BRAP_RM_P_MAX_DSPS];
								            /* Array of DSP usage strucrure */
	BRAP_RM_P_ObjectState	eRBufState[BRAP_RM_P_MAX_RBUFS];
								            /* Array to maintain the states of 
								               the Ring buffers */
	BRAP_RM_P_ObjectState	eSrcChState[BRAP_RM_P_MAX_SRC_CHANNELS];
								            /* Array of source channel usage 
								               strucrure */
	BRAP_RM_P_ObjectState	eDstChState[BRAP_RM_P_MAX_DST_CHANNELS];
								            /* Array to maintain the states of 
								               the Ring buffers */
    BRAP_RM_P_ObjectState	eAdaptRateCtrlState[BRAP_RM_P_MAX_ADAPTRATE_CTRL];
    							            /* Array of source channel usage 
    							               strucrure */								               
    BRAP_RM_P_SrcUsage      sSrcUsage[BRAP_RM_P_MAX_SRC_BLCK]
                                     [BRAP_RM_P_MAX_SRC_PER_SRC_BLCK];
								            /* Array to maintain the states of 
								               the SRCs */
	BRAP_RM_P_MixerUsage	sMixerUsage[BRAP_RM_P_MAX_DP_BLCK]
	                                   [BRAP_RM_P_MAX_MIXER_PER_DP_BLCK];
								            /* Array of Mixer usage usage 
								               strucrure */
	BRAP_RM_P_SpdiffmUsage	sSpdiffmUsage[BRAP_RM_P_MAX_SPDIFFMS];
								            /* Array of SPDIF formater usage 
								               strucrure */
    BRAP_RM_P_CapPortUsage  sCapPortUsage[BRAP_CapInputPort_eMax];
                                            /* Array of CapPort usage 
                                                strucrure */
    BRAP_RM_P_FsTmgSrcUsage sFsTmgSrcUsage[BRAP_RM_P_MAX_FS_TMG_SOURCES];
                                            /* Array of FS Timing Source usage 
                                               strucrure */
} BRAP_RM_P_Object;

/***************************************************************************
Summary:	
	Structure for SPDIF Formater mappings
***************************************************************************/
typedef struct BRAP_RM_P_SpdiffmMapping
{	
	unsigned int	uiSpdiffmId;	/* Index of the SPDIF Formater used,	
								       BRAP_RM_P_INVALID_INDEX indicates 
								       SPDIF Formater not used */
	unsigned int	uiSpdiffmChId;	/* Index of the SPDIF Formater channel 
	                                   used. This field is valid if 
	                                   uiSpdiffmId != INVALID_INDEX */
} BRAP_RM_P_SpdiffmMapping;


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
	);

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
	);

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
	);

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
	);

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
    );

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
    );

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
    );

/***************************************************************************
Summary:
    Private function that initializes the resource Request to invalid values.
**************************************************************************/
void 
BRAP_RM_P_InitResourceReq(
	BRAP_RM_P_ResrcReq          *pResrcReq/* [in] Resource Request structure 
                                                  to be inited to invalid
                                                  values */
    );

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
   );

/***************************************************************************
Summary:
    Private function that updates the old resource grant structure with the 
    new one.  
**************************************************************************/
BERR_Code
BRAP_RM_P_UpdateResrcGrant (
    BRAP_RM_P_ResrcGrant        *pOldResrcGrant,
    BRAP_RM_P_ResrcGrant        *pNewResrcGrant
);

#ifdef __cplusplus
}
#endif

#endif /* _BRAP_RM_PRIV_H__ */
/* End of File */

