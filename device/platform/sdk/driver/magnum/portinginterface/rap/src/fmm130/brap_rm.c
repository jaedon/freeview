/***************************************************************************
*     Copyright (c) 2004-2008, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brap_rm.c $
* $brcm_Revision: Hydra_Software_Devel/70 $
* $brcm_Date: 11/12/08 7:31p $
*
* Module Description:
*	This file contains the implementations of the APIs, which are used by
*	Raptor Audio PI. 
*
*	For various audio operations, Raptor audio PI makes use of following
*	hardware/firmware/memory resources.
*		1. DSP contexts (Firmware resource)
*		2. DRAM ring buffers (Memory resource)
*		3. Source/Destination channels (FMM hardware resource)
*		4. Mixers (FMM hardware resource)
*		5. SPDIF-Formatter (FMM hardware resource)
*		6. Output ports (FMM hardware resource)
*
*	Above resources are shared for following audio operations.
*		1. Live Decode/Pass Thru/SRC/Simulmode in DSP
*		2. Playback from hard-disk/memory
*		3. Capture
*		4. Encode
*		
*	Resource Manager maintains above resources and provides them to Audio
*	Manager as per the request. Resource Manager has the knowledge of
*	mappings between various resources. It allocates resources, 
*	for optimum usage. However, Resource Manager doesn't know what all
*	resources getting used for a particular instance of audio operation. It is
*	the responsibility of Audio Manager to provide list of all the 
*	resources to be freed once an instance of audio operation is closed.
*
*	Other modules interact with Resource Manager for following purposes.
*		1. When a new audio channel (instance of audio operation) is formed
*		   (or closed). Audio Manager requests Resource Manager to allocate 
*		   (or free) all the required resources for this operation.
*		2. Add/Remove Output ports.
*		3. Obtaining mapping information.
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/rap/base/brap_rm.c $
* 
* Hydra_Software_Devel/70   11/12/08 7:31p srajapur
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
* Hydra_Software_Devel/69   8/6/08 4:44p sgadara
* PR 43053: [7401] Adding the debug message while freeing PPM resources.
* 
* Hydra_Software_Devel/68   5/19/08 10:22p srajapur
* PR42140 : [7400] Implemented  PPM adjustments as in 7401
* 
* Hydra_Software_Devel/67   5/19/08 9:24p srajapur
* PR42140 : [7400] Implemented  PPM adjustments as in 7401
* 
* Hydra_Software_Devel/66   3/11/08 11:33a srajapur
* PR 39994 : [7400,7401,7403,7118] Removed warning
* 
* Hydra_Software_Devel/65   3/7/08 2:52p srajapur
* PR 39994 : [7400,7401,7403,7118] In dual mode: can not play any VC1
* stream on second window and the sytem crashes in certain sequence of
* action
* - For passthrough mode the context 2 is used always.(Expect two
* passthrough support mode)
* 
* Hydra_Software_Devel/64   10/17/07 7:03p bselva
* PR 35525:[7400,7401,7403,7118] The DSP context allocation is moved to
* start time to support dual decode for 7400.
* 
* Hydra_Software_Devel/63   5/30/07 7:19p gautamk
* PR27106: Fixing number of warnings in brap module
* 
* Hydra_Software_Devel/62   4/25/07 4:09p bselva
* PR 27546: Added the code to allocate extra ring buffers for output path
* during the full duplex capture mode.
* 
* Hydra_Software_Devel/61   4/17/07 4:02p gautamk
* PR29826:  Fix Coverity issues for RAP 1.0 PI
* 
* Hydra_Software_Devel/60   3/8/07 10:50a rjain
* PR 28524:[7401, 7118, 7403] code clean up: replacing (BCHP_CHIP ==
* 7401) || (BCHP_CHIP == 7118) || (BCHP_CHIP == 7403) with
* (BRAP_7401_FAMILY == 1) where appropriate
* 
* Hydra_Software_Devel/59   12/19/06 1:26p rjain
* PR 25670: add 7403 support to RAP
* 
* Hydra_Software_Devel/58   8/28/06 11:51a bselva
* PR 18604: Fixing the compilation warning with DEBUG=n
* 
* Hydra_Software_Devel/57   8/28/06 11:01a sushmit
* PR 18604: Fixing compilation issues with DEBUG=n
* 
* Hydra_Software_Devel/56   8/11/06 4:02p bselva
* PR 23192: Fixed the compilation warning
* 
* Hydra_Software_Devel/55   7/12/06 4:29p bselva
* PR 22288: Checking in the usage of timer for full mark interrupt
* 
* Hydra_Software_Devel/54   5/15/06 12:35p bselva
* PR 21388: If simulmode use allways context 2 for all chips
* 
* Hydra_Software_Devel/53   5/9/06 11:23a rjain
* PR 21481: adding support for 7118 to RAP PI
* 
* Hydra_Software_Devel/52   4/27/06 1:29p bselva
* PR 21232: Added the support for MAI for 7400
* 
* Hydra_Software_Devel/51   4/24/06 10:53a sushmit
* PR18604: Updating Encoder MIT.
* 
* Hydra_Software_Devel/50   4/20/06 6:04p rjain
* PR 20039: fixing compilation for 7411
* 
* Hydra_Software_Devel/49   4/20/06 4:28p sushmit
* PR18604: Fixing Encoder MIT.
* 
* Hydra_Software_Devel/48   4/20/06 4:25p rjain
* PR 20039: fixing DSP context allocation
* 
* Hydra_Software_Devel/47   4/11/06 6:05p sushmit
* PR18604: Modifying
* #if (( BCHP_CHIP == 7401 )||( BCHP_CHIP == 7400 ))
* With code like
* #ifndef BCHP_7411_VER
* 
* Hydra_Software_Devel/46   4/7/06 2:50p sushmit
* PR18604: Updating Encoder MIT & Watchdog.
* 
* Hydra_Software_Devel/45   3/30/06 4:37p bselva
* PR 19878: Implemented the code review coments
* 
* Hydra_Software_Devel/44   3/21/06 6:28p sushmit
* PR18604: Updating review comments.
* 
* Hydra_Software_Devel/43   3/2/06 5:56p rjain
* PR 19932: Fixed. hRmTemp->sDspUsage[pResrcGrant->uiDspId].uiDecCxtCount
* was incorrectly being decremented even when pResrcGrant->uiDspId was
* invalid. This corrupted the hRm structure.
* 
* Hydra_Software_Devel/42   3/1/06 6:22p rjain
* PR 19946 :
* - Fixed some issues related to handling of Mai
* - Fixed some issues related to sampling rate changes for Mai, for
* cloned ports and for decode channels
* 
* Hydra_Software_Devel/41   2/27/06 4:17p sushmit
* PR18604: Fixing 7411 compilation issues.
* 
* Hydra_Software_Devel/40   2/20/06 10:59a bselva
* PR 19168: Removed the warnings
* 
* Hydra_Software_Devel/39   2/17/06 10:07a sushmit
* PR18604: Merging Encoder to latest.
* 
* Hydra_Software_Devel/39   2/17/06 9:53a sushmit
* PR18604: Merging Encoder to latest.
* 
* Hydra_Software_Devel/38   2/8/06 5:16p bselva
* PR 19168: Checking in the changes for I2S output on 7400
* 
* Hydra_Software_Devel/RAP_AUD_ENC/5   2/16/06 4:44p sushmit
* PR18604: Updating Audio Encoder PI.
* 
* Hydra_Software_Devel/RAP_AUD_ENC/4   2/6/06 4:15p sushmit
* PR18604: Merging latest Decoder PI to Encoder PI branch
* 
* Hydra_Software_Devel/RAP_AUD_ENC/2   1/16/06 5:37p sushmit
* PR18604: Updating Audio Encoder PI.
* 
* Hydra_Software_Devel/35   12/15/05 5:31p kagrawal
* PR 18312: Implement 7411 RAP PI phase-5 deliverables
* - Optimized source channel preference
* 
* Hydra_Software_Devel/34   12/12/05 2:03p bselva
* PR 18264: Added MAi and Flex to the supported output port for 7401
* 
* Hydra_Software_Devel/33   12/9/05 4:41p rjain
* PR 18264: Capture PI Development
* 
* Hydra_Software_Devel/32   12/8/05 1:55p kagrawal
* PR 18478: Replaced an assert by a if to check whether a decode context
* was in use in BRAP_RM_P_FreeResources()
* 
* Hydra_Software_Devel/31   12/5/05 11:24a kagrawal
* PR 18312: Changed a BDBG_ERR to BDBG_MSG in BRAP_RM_P_AllocateResources
* 
* Hydra_Software_Devel/30   11/30/05 4:18p kagrawal
* PR 18279: Rearchitected output sampling rate change for PCM channel
* 
* Hydra_Software_Devel/29   11/14/05 9:54a rjain
* PR 18129: Adding support for routing to multiple output ports
* 
* Hydra_Software_Devel/28   11/13/05 4:34p kagrawal
* PR 17590: Added a new routine BRAP_RM_P_GetChannelsUsingMixer
* 
* Hydra_Software_Devel/27   11/12/05 6:20p kagrawal
* PR 17590: Added an internal routine BRAP_RM_P_GetDecChannel
* 
* Hydra_Software_Devel/26   11/10/05 11:54a kagrawal
* PR 17590: Merging PCM Playback and Mixing changes to
* Hydra_Software_Devel
* 
* Hydra_Software_Devel/Kapil_PCM_Playback/3   11/9/05 12:46p kagrawal
* PR 17590: Supported any source channel to any ring buffer mapping for
* chips other than 7411.
* 
* Hydra_Software_Devel/Kapil_PCM_Playback/2   11/1/05 9:45a kagrawal
* Implemented review comments on PCM playback and mixing
* 
* Hydra_Software_Devel/Kapil_PCM_Playback/1   10/24/05 5:07p kagrawal
* PR 17590: Initial check in for PCM Playback and Mixing
*
* Hydra_Software_Devel/25   11/9/05 4:07p bselva
* PR 17590: Added support for HDMI output
* 
* Hydra_Software_Devel/24   10/18/05 4:58p rjain
* PR 17651: Changing checks for BCHP_CHIP ==/!= 7401 to check for
* BCHP_7411_VER instead
* 
* Hydra_Software_Devel/23   9/14/05 6:45p bmishra
* PR 16148: Fixed the 7401 related changes overwritten by the latest
* check-in
* 
* Hydra_Software_Devel/22   9/13/05 2:28p rjain
* PR 17042: Adding ISR version of BRAP_MIXER_Set/GetMute PI.
* 
* Hydra_Software_Devel/21   9/13/05 7:41p bmishra
* PR 16148: Merged 7401 related changes
* 
* Hydra_Software_Devel/20   8/22/05 3:41p nitinb
* PR 16799: Use BERR_TRACE macro while returning errors in RAP PI
*
* Hydra_Software_Devel/BLR_RAP_7401_Bringup/2   7/20/05 2:29p bmishra
* PR 16148: Modified to support HiFiDAC for 7401
* 
* Hydra_Software_Devel/BLR_RAP_7401_Bringup/1   7/12/05 8:16p bmishra
* PR 16148: Modified the file for 7401 compatibility
* 
* Hydra_Software_Devel/18   7/8/05 4:10p bselva
* PR15925: To allocate the resources properly when both simulmode and
* cloneoutput flag is set
* 
* Hydra_Software_Devel/17   7/8/05 2:05p rjain
* PR 16148: Modifiying for 7401
* 
* Hydra_Software_Devel/16   6/18/05 3:31p rjain
* PR 15925: PI changes required for routing decoded data to I2S and SPDIF
* ports simultanesouly
* 
* Hydra_Software_Devel/BLR_RAP_7411C0_RM_REDESIGN/2   6/13/05 7:40p bselva
* Implemented the code reveiw coments
* 
* Hydra_Software_Devel/14   5/4/05 11:09a rjain
* PR 14507:- Added proper States, open/start counts for modules.
* - Added appropriate checks to handle multiple calls to _Open(),
* _Start(), _Stop(), _Close() for modules.
* - Corrected SPDIFFM handle stored in hFmm / channel handle to be
* SPDIFFM Stream handle.
* - Some clean up, mroe comments/debug messages.
* - Added BRAP_RM_P_GetSpdifFmId() to RM
* 
* Hydra_Software_Devel/13   4/15/05 2:58p bmishra
* PR 14507: Simultaneous mode related changes
* 
* Hydra_Software_Devel/12   3/10/05 8:55p rjain
* PR 13082: some fixes....
* 
* Hydra_Software_Devel/11   3/10/05 3:25p rjain
* PR 13082:
* - Implemented the new Start-Stop scheme where Hosts prgms all
* registers. DSP uses Start_WRPoint and RBUF_EndAddress to start decode.
* - removed bMixingEnable from BRAP_MIXER_Params
* - removed sRBufParams from BRAP_DEC_AudioParams.
* - Added BRAP_RM_P_GetDpStreamId
* - Corrected programming of Stream Id in DP registers
* - Added BRAP_SRCCH_P_WaitForRampDown()
* - Other minor changes
* 
* Hydra_Software_Devel/9   2/16/05 8:30p bmishra
* PR13082: Changed BAUD_ to BRAP_
* 
* Hydra_Software_Devel/8   2/16/05 1:46p rjain
* PR 14143: Implementing some comments from Phase 1 Code review done till
* date
* 
* Hydra_Software_Devel/7   2/8/05 7:00p rjain
* PR13082: Some bug fixes and clean up
* 
* Hydra_Software_Devel/6   2/3/05 2:46p rjain
* PR13082: Fixing compilation warnings
* 
* Hydra_Software_Devel/5   2/2/05 5:34p rjain
* PR13082:Converted BREG_Read32 and BRAP_Read32 and BREG_Write32 to
* BRAP_Write32
* 
* Hydra_Software_Devel/4   1/25/05 9:54a rjain
* PR13082: Modifying according to settings/bugs highlighted in emulation
* so far.
* 
* Hydra_Software_Devel/3   1/12/05 5:59p rjain
* PR13082: adding BRAP_RM_P_GetDpId
* 
* Hydra_Software_Devel/2   1/11/05 11:46a rjain
* PR13082: Added functions BRAP_RM_P_GetMixerForOpPort() and
* BRAP_RM_P_GetSpdifFmForOpPort()
* 
* Hydra_Software_Devel/1   1/10/05 8:26p nitinb
* PR 13082: Initial version
* 
***************************************************************************/

#include "brap_rm_priv.h"
#include "brap_priv.h"
#include "brap.h"

BDBG_MODULE(rap_rm);		/* Register software module with debug interface */

/*{{{ Local Defines */
#define BRAP_RM_P_SIM_MODE_PT_DSP_CONTEXT	2	/* DSP Context for the Pass-thru 
												   operation of Simultaneous mode */
#ifndef BCHP_7411_VER /* For chips other than 7411 */
/* Encoder support in RM */
#define BRAP_RM_P_ENC_MODE_DSP_CONTEXT 1 /* DSP context for audio encoder */
#endif
/*}}}*/

/*{{{ Local Typedefs */

/*}}}*/


/*{{{ Static Variables & Function prototypes */
#if defined ( BCHP_7411_VER ) /* For the 7411 */ 
const unsigned int BRAP_RM_P_MixerPref[BRAP_RM_P_MAX_OUTPUTS] = 
{
	0,		/* Mixer preferences for SPDIF */
	1,		/* Mixer preferences for DAC0 */
	2,		/* Mixer preferences for I2S0 */
	3,		/* Mixer preferences for I2S1 */
	4,		/* Mixer preferences for I2S2 */
	5		/* Mixer preferences for DAC1 */
};

const BRAP_RM_P_SpdiffmMapping BRAP_RM_P_SpdiffmPref[BRAP_RM_P_MAX_OUTPUTS] = 
{
	{0, 0},	/* SPDIF Formator Stream 0 required for SPDIF */
	{0, 1},	/* SPDIF Formator Stream 1 required for DAC0 */
	{BRAP_RM_P_INVALID_INDEX, BRAP_RM_P_INVALID_INDEX}, /* No SPDIF Formator for I2S0 */
	{BRAP_RM_P_INVALID_INDEX, BRAP_RM_P_INVALID_INDEX}, /* No SPDIF Formator for I2S1 */
	{BRAP_RM_P_INVALID_INDEX, BRAP_RM_P_INVALID_INDEX}, /* No SPDIF Formator for I2S2 */
	{BRAP_RM_P_INVALID_INDEX, BRAP_RM_P_INVALID_INDEX}  /* No SPDIF Formator for DAC1 */
};

const unsigned int	BRAP_RM_P_DpPref[BRAP_RM_P_MAX_MIXERS] = 
{
	0, 0, 0,		/* Mixers 0, 1, 2 belongs to DP 0 */
	1, 1, 1		/* Mixers 3, 4, 5 belongs to DP 1 */
};

/* These are the soft mappings */
const unsigned int	BRAP_RM_P_SrcChPref[BRAP_RM_P_MAX_DPS][BRAP_RM_P_MAX_SRCCH_PER_DP] = 
{
	{1, 0, 2, 3},	/* For DP0 */
	{3, 2, 4, 5}	/* For DP1 */
};

const unsigned int	BRAP_RM_P_SharedRBufs[BRAP_RM_P_MAX_SRCCH_PER_DP] = 
{
	8, 9, 10, 11
};

const unsigned int BRAP_RM_P_RBufPref[BRAP_RM_P_MAX_SRC_CHANNELS][BRAP_RM_P_MAX_RBUFS_PER_SRCCH] = 
{
	{0, 8},			/* For Source FIFO0 */
	{1, 9},			/* For Source FIFO1 */
	{2, 10},			/* For Source FIFO2 */
	{3, 11},			/* For Source FIFO3 */
	{4, 8},			/* For Source FIFO4 */
	{5, 9}			/* For Source FIFO5 */
};

const unsigned int BRAP_RM_P_RBufPref4DstCh[BRAP_RM_P_MAX_DST_CHANNELS][BRAP_RM_P_MAX_RBUFS_PER_DSTCH] = 
{
	{6, 10},
	{7, 11}
};

/* 'playback stream X' => Stream X in this DP
        For 7411, the source fifo id mapping to DP Stream id is as follow:
        Source fifo id      DP0 Stream id
        0                   0
        1                   1
        2                   2
        3                   3
        Source fifo id      DP1 Stream id
        3                   0
        4                   1
        5                   2
        2                   3
*/
const unsigned int	BRAP_RM_P_DpStreamId[BRAP_RM_P_MAX_DPS][BRAP_RM_P_MAX_SRCCH_PER_DP] = 
{
	{0, 1, 2, 3},	/* For DP0 */
	{3, 4, 5, 2}	/* For DP1 */
};
#elif (BRAP_7401_FAMILY == 1)
const unsigned int BRAP_RM_P_MixerPref[BRAP_RM_P_MAX_OUTPUTS] = 
{
	0,		/* Mixer preferences for SPDIF */
	2,		/* Mixer preferences for DAC0 */
	3,		/* Mixer preferences for I2S0 */
	BRAP_RM_P_INVALID_INDEX,		/* Mixer preferences for I2S1 */
	BRAP_RM_P_INVALID_INDEX,		/* Mixer preferences for I2S2 */
	BRAP_RM_P_INVALID_INDEX,		/* Mixer preferences for DAC1 */
	BRAP_RM_P_INVALID_INDEX,		/* Mixer preferences for MAI */
	1		/* Mixer preferences for FLEX */
};

const BRAP_RM_P_SpdiffmMapping BRAP_RM_P_SpdiffmPref[BRAP_RM_P_MAX_OUTPUTS] = 
{
	{0, 0},	/* SPDIF Formator Stream 0 required for SPDIF */
	{BRAP_RM_P_INVALID_INDEX, BRAP_RM_P_INVALID_INDEX}, /* No SPDIF Formator for DAC0 */
	{BRAP_RM_P_INVALID_INDEX, BRAP_RM_P_INVALID_INDEX}, /* No SPDIF Formator for I2S0 */
	{BRAP_RM_P_INVALID_INDEX, BRAP_RM_P_INVALID_INDEX}, /* No SPDIF Formator for I2S1 */
	{BRAP_RM_P_INVALID_INDEX, BRAP_RM_P_INVALID_INDEX}, /* No SPDIF Formator for I2S2 */
	{BRAP_RM_P_INVALID_INDEX, BRAP_RM_P_INVALID_INDEX}, /* No SPDIF Formator for DAC1 */
	{BRAP_RM_P_INVALID_INDEX, BRAP_RM_P_INVALID_INDEX}, /* No SPDIF Formator for MAI */
	{0, 1}  /* SPDIF Formator Steam 1 required for FLEX */
};

const unsigned int	BRAP_RM_P_DpPref[BRAP_RM_P_MAX_MIXERS] = 
{
	0, 0, 0, 0		/* Mixers 0, 1, 2, 3 belongs to DP 0 */
};

const unsigned int	BRAP_RM_P_SrcChPref[BRAP_RM_P_MAX_DPS][BRAP_RM_P_MAX_SRCCH_PER_DP] = 
{
	{0, 1, 2, 3}	/* For DP0 */
};

const unsigned int BRAP_RM_P_RBufId[BRAP_RM_P_MAX_RBUFS] = {0, 1, 2, 3, 4, 5, 6, 7};

/* 'playback stream X' => Stream X in this DP
        For 7401, the source fifo id mapping to DP Stream id is as follow:
        Source fifo id      DP0 Stream id
        0                   0
        1                   1
        2                   2
        3                   3
*/
const unsigned int	BRAP_RM_P_DpStreamId[BRAP_RM_P_MAX_DPS][BRAP_RM_P_MAX_SRCCH_PER_DP] = 
{
	{0, 1, 2, 3}	/* For DP0 */
};
#elif ( BCHP_CHIP == 7400 )
const unsigned int BRAP_RM_P_MixerPref[BRAP_RM_P_MAX_OUTPUTS] = 
{
	0,		/* Mixer preferences for SPDIF */
	2,		/* Mixer preferences for DAC0 */
	3,		/* Mixer preferences for I2S0 */
	7,		/* Mixer preferences for I2S1 */
	4,		/* Mixer preferences for I2S2 */
	6,		/* Mixer preferences for DAC1 */
	BRAP_RM_P_INVALID_INDEX,		/* Mixer preferences for MAI */
	1		/* Mixer preferences for FLEX */
};

const BRAP_RM_P_SpdiffmMapping BRAP_RM_P_SpdiffmPref[BRAP_RM_P_MAX_OUTPUTS] = 
{
	{0, 0},	/* SPDIF Formator Stream 0 required for SPDIF */
	{BRAP_RM_P_INVALID_INDEX, BRAP_RM_P_INVALID_INDEX}, /* No SPDIF Formator for DAC0 */
	{BRAP_RM_P_INVALID_INDEX, BRAP_RM_P_INVALID_INDEX}, /* No SPDIF Formator for I2S0 */
	{BRAP_RM_P_INVALID_INDEX, BRAP_RM_P_INVALID_INDEX}, /* No SPDIF Formator for I2S1 */
	{BRAP_RM_P_INVALID_INDEX, BRAP_RM_P_INVALID_INDEX}, /* SPDIF Formator stream 1 required for I2S2 */
	{BRAP_RM_P_INVALID_INDEX, BRAP_RM_P_INVALID_INDEX}, /* No SPDIF Formator for DAC1 */
	{BRAP_RM_P_INVALID_INDEX, BRAP_RM_P_INVALID_INDEX}, /* No SPDIF Formator for MAI */
	{0, 1}  /* SPDIF Formator stream 1 required for FLEX */
};

const unsigned int	BRAP_RM_P_DpPref[BRAP_RM_P_MAX_MIXERS] = 
{
	0, 0, 0, 0,	/* Mixers 0, 1, 2, 3 belongs to DP 0 */
	1, 1, 1, 1		/* Mixers 4, 5, 6, 7 belongs to DP 1 */
};

const unsigned int	BRAP_RM_P_SrcChPref[BRAP_RM_P_MAX_DPS][BRAP_RM_P_MAX_SRCCH_PER_DP] = 
{
	{1, 0, 2, 3},	/* For DP0 */
	{4, 5, 6, 7}	/* For DP1 */
};

const unsigned int	BRAP_RM_P_PpmPref[BRAP_RM_P_MAX_PPM_CHANNELS] = 
{
	 1,0, 2, 3 	
};


const unsigned int BRAP_RM_P_RBufId[BRAP_RM_P_MAX_RBUFS] = {0, 1, 2, 3, 4, 5, 6, 7,
															8, 9, 10, 11, 12, 13, 14, 15};

/* 'playback stream X' => Stream X in this DP
        For 7400, the source fifo id mapping to DP Stream id is as follow:
        Source fifo id      DP0 Stream id
        0                   0
        1                   1
        2                   2
        3                   3
        Source fifo id      DP1 Stream id
        4                   0
        5                   1
        6                   2
        7                   3
*/
const unsigned int	BRAP_RM_P_DpStreamId[BRAP_RM_P_MAX_DPS][BRAP_RM_P_MAX_SRCCH_PER_DP] = 
{
	{0, 1, 2, 3},	/* For DP0 */
	{4, 5, 6, 7}	/* For DP1 */
};
#else
#error "Not supported chip type"
#endif

/*}}}*/


/***************************************************************************
Summary:
	Opens the resource manager.
Description:
	This function initializes Resource Manager data structures. This 
	function should be called before calling any other Resource Manager
	function.

Returns:
	BERR_SUCCESS - If successful
	BERR_OUT_OF_SYSTEM_MEMORY - If enough memory not available

See Also:
	BRAP_RM_P_Close
**************************************************************************/
BERR_Code 
BRAP_RM_P_Open( 
	BRAP_Handle		hRap,		/* [in] RAP Device handle */
	BRAP_RM_Handle	*phRm		/* [out]Resource Manager handle */
	)
{
	BRAP_RM_Handle	hRm;
	BERR_Code		ret = BERR_SUCCESS;
	int				i, j;

	BDBG_ENTER(BRAP_RM_P_Open);

	/* Assert the function arguments*/
	BDBG_ASSERT(hRap);
	BDBG_ASSERT(phRm);

	/* Allocate the Resource Manager handle */
	hRm = (BRAP_RM_Handle)BKNI_Malloc( sizeof(BRAP_RM_P_Object));
	if(hRm == NULL)
	{
		ret = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		return ret;
	}

	*phRm = hRm;

	/* Reset the entire structure */
	BKNI_Memset( hRm, 0, sizeof(BRAP_RM_P_Object) );
	
	for(i = 0; i < BRAP_RM_P_MAX_MIXERS; i++)
	{
		hRm->sMixerUsage[i].uiUsageCount = 0;
		
		for(j = 0; j < BRAP_RM_P_MAX_MIXER_INPUTS; j++)
		{
			hRm->sMixerUsage[i].eInputStatus[j] = BRAP_RM_P_ObjectState_eFree;
		}
	}

	for (i = 0; i < BRAP_RM_P_MAX_SPDIFFMS; i++)
	{
		for(j = 0; j < BRAP_RM_P_MAX_SPDIFFM_STREAMS; j++)
		{
			hRm->sSpdiffmUsage[i].eSpdiffmChStatus[j] = BRAP_RM_P_ObjectState_eFree;
		}
	}
	
	for(i = 0; i < BRAP_RM_P_MAX_SRC_CHANNELS; i++)
	{
		hRm->sSrcChUsage[i].uiUsageCount = 0;
	}
	
	for(i = 0; i < BRAP_RM_P_MAX_PPM_CHANNELS; i++)
	{
		hRm->sPpmChUsage[i].uiUsageCount = false;
	}
	
	for(i = 0; i < BRAP_RM_P_MAX_RBUFS; i++)
	{
		hRm->eRBufState[i] = BRAP_RM_P_ObjectState_eFree;
	}
	
	for(i = 0; i < BRAP_RM_P_MAX_DSPS; i++)
	{
		for(j = 0; j < BRAP_RM_P_MAX_CXT_PER_DSP; j++)
		{
			hRm->sDspUsage[i].eContext[j] = BRAP_RM_P_ObjectState_eFree;
		}
		hRm->sDspUsage[i].uiDecCxtCount = 0;
		hRm->sDspUsage[i].uiSrcCxtCount = 0;
		hRm->sDspUsage[i].uiPtCxtCount = 0;
#ifndef BCHP_7411_VER /* For chips other than 7411 */
/* Encoder support in RM */
		hRm->sDspUsage[i].uiEncCxtCount = 0;
#endif
	}

	for(i = 0; i < BRAP_RM_P_MAX_DST_CHANNELS; i++)
	{
		hRm->eDestChState[i] = BRAP_RM_P_ObjectState_eFree;
	}

	/* Store RM handle inside the RAP handle */
	hRap->hRm = hRm;

	BDBG_LEAVE(BRAP_RM_P_Open);
	
	return ret;
}

/***************************************************************************
Summary:
	Closes the resource manager.
Description:
	This function shuts downs the Resource Manager and frees up the 
	Resource Manager handle. This function should be called while closing 
	the audio device.

Returns:
	BERR_SUCCESS - If successful

See Also:
	BRAP_RM_P_Open
**************************************************************************/
BERR_Code 
BRAP_RM_P_Close( 
	BRAP_Handle		hRap,		/* [in] Device Handle */
	BRAP_RM_Handle	hRm			/* [in] The Resource Manager handle */
	)
{
	
	BDBG_ENTER(BRAP_RM_P_Open);

	/* Assert the function arguments*/
	BDBG_ASSERT(hRap);
	BDBG_ASSERT(hRm);

	/* Free up the RM handle */
	BKNI_Free( hRm );
	hRap->hRm = NULL;

	BDBG_LEAVE(BRAP_RM_P_Open);
	
	return BERR_SUCCESS;
}

/***************************************************************************
Summary:

Description:
	This function allocates all the resources required for opening a 
	particular instance of audio operation.

Returns:
	BERR_SUCCESS - If successful
	BERR_OUT_OF_SYSTEM_MEMORY - If enough memory not available
	BRAP_ERR_RESOURCE_EXHAUSTED - If Resources are exhausted

See Also:
	BRAP_RM_P_FreeResources
**************************************************************************/
BERR_Code
BRAP_RM_P_AllocateResources (
	BRAP_RM_Handle				hRm,		/* [in] Resource Manager handle */
	const BRAP_RM_P_ResrcReq	*pResrcReq,	/* [in] Required resources */
	BRAP_RM_P_ResrcGrant		*pResrcGrant/* [out] Indexed of the resources
											   allocated */
	)
{
	BERR_Code		ret = BERR_SUCCESS;
	unsigned int	i, j;
	BRAP_RM_Handle	hRmTemp;
	bool			bLastChMono;
	unsigned int	uiNumOpPorts;
	BRAP_OutputPort	eOpPortType;
	unsigned int	uiSrcChCnt = 0;
	unsigned int	uiDstChCnt = 0;
	unsigned int	uiMixerInputId = 0;
	unsigned int	uiSrcChId;
#if (BCHP_CHIP == 7400)	
	unsigned int 	uiPpmId=0,uiPpmChCnt=0;
#endif    
	unsigned int	uiTotDSPContext = 0;
#ifdef BCHP_7411_VER
	unsigned int	uiTempRBufId[BRAP_RM_P_MAX_RBUFS_PER_SRCCH];
#else
	unsigned int	k = 0;
	unsigned int 	uiNumRbufRequested = 0;
	unsigned int 	uiTmpRBufId = 0;
#endif

	BDBG_ENTER(BRAP_RM_P_AllocateResources);

	/* Assert the function arguments*/
	BDBG_ASSERT(hRm);
	BDBG_ASSERT(pResrcReq);
	BDBG_ASSERT(pResrcGrant);

	/* Allocate a temporary RM Handle */
	hRmTemp = (BRAP_RM_Handle)BKNI_Malloc( sizeof(BRAP_RM_P_Object));
	if(hRmTemp == NULL)
	{
		ret = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		return ret;
	}

	/* Copy the original RM handle content to the temporary handle */
	*hRmTemp = *hRm;

	/* Initialize pResrcGrant entries to invalid */
	pResrcGrant->uiNumOpPorts = 0;
	for(i = 0; i < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; i++)
	{
		pResrcGrant->sOpResrcId[i].eOutputPortType = BRAP_RM_P_INVALID_INDEX;
		for(j = 0; j < BRAP_RM_P_MAX_RBUFS_PER_PORT; j++)
			pResrcGrant->sOpResrcId[i].uiRbufId[j] = BRAP_RM_P_INVALID_INDEX;
		pResrcGrant->sOpResrcId[i].uiSrcChId = BRAP_RM_P_INVALID_INDEX;
        pResrcGrant->sOpResrcId[i].uiPpmId = BRAP_RM_P_INVALID_INDEX;
		pResrcGrant->sOpResrcId[i].uiDataPathId = BRAP_RM_P_INVALID_INDEX;
		pResrcGrant->sOpResrcId[i].uiMixerId = BRAP_RM_P_INVALID_INDEX;
		pResrcGrant->sOpResrcId[i].uiMixerInputId = BRAP_RM_P_INVALID_INDEX;
		pResrcGrant->sOpResrcId[i].uiSpdiffmId = BRAP_RM_P_INVALID_INDEX;
		pResrcGrant->sOpResrcId[i].uiSpdiffmStreamId = BRAP_RM_P_INVALID_INDEX;
	}
	pResrcGrant->sCapResrcId.eInputPortType = BRAP_RM_P_INVALID_INDEX;
	for(i = 0; i < BRAP_RM_P_MAX_RBUFS_PER_DSTCH; i++)
		pResrcGrant->sCapResrcId.uiRbufId[i] = BRAP_RM_P_INVALID_INDEX;
	pResrcGrant->sCapResrcId.uiDstChId = BRAP_RM_P_INVALID_INDEX;
	pResrcGrant->uiDspId = BRAP_RM_P_INVALID_INDEX;
	pResrcGrant->uiDspContextId = BRAP_RM_P_INVALID_INDEX;
	pResrcGrant->uiFmmId = BRAP_RM_P_INVALID_INDEX;

	uiNumOpPorts = pResrcReq->uiNumOpPorts;
	bLastChMono = pResrcReq->bLastChMono;

#ifndef BCHP_7411_VER
	uiNumRbufRequested = bLastChMono ? 1 : 2;
#endif

	pResrcGrant->uiNumOpPorts = uiNumOpPorts;

	BDBG_MSG(( "Resource Requested: \n"
     				"ChannelType = %d\n"
				"eBufDataMode =%d \n"
				"uiNumOpPorts = %d\n"
				"bSimulModePt = %d\n"
				"bAllocateDsp = %d\n"				
				"bLastChMono = %d\n"
				"bAllocateRBuf = %d\n"
				"bAllocateSrcCh = %d\n"
				"bAllocatePpm = %d\n"
				"bAllocateDstCh = %d\n",				
				pResrcReq->eChannelType, 
				pResrcReq->eBufDataMode, 				
				pResrcReq->uiNumOpPorts, 
				pResrcReq->bSimulModePt, 
				pResrcReq->bAllocateDsp, 				
				pResrcReq->bLastChMono, 
				pResrcReq->bAllocateRBuf, 
				pResrcReq->bAllocateSrcCh, 				
				pResrcReq->bAllocatePpm,	
				pResrcReq->bAllocateDstCh));

	BDBG_MSG((	"\nResource Granted:" ));

	if ( pResrcReq->bAllocateDstCh == true )
	{
		for ( uiDstChCnt=0; uiDstChCnt<BRAP_RM_P_MAX_DST_CHANNELS; uiDstChCnt++ )
		{
			if ( hRmTemp->eDestChState[uiDstChCnt] == BRAP_RM_P_ObjectState_eFree )
                            /* Got a free destination channel */
                            break;
		}

		/* Validate src channel count */
		if ( uiDstChCnt >= BRAP_RM_P_MAX_DST_CHANNELS)
		{
			/* There is no free source channel to be allocated */
			ret = BERR_TRACE(BRAP_ERR_RESOURCE_EXHAUSTED);
			goto error; 
		}

		/* update the destination channel usage */
		hRmTemp->eDestChState[uiDstChCnt] = BRAP_RM_P_ObjectState_eBusy;
		pResrcGrant->sCapResrcId.uiDstChId = uiDstChCnt;
		BDBG_MSG(("got this DstCh[%d]",uiDstChCnt));

    		/* Check if RBuf allocation is requested */
		if ( pResrcReq->bAllocateRBuf == true )
		{
#ifdef BCHP_7411_VER		
			for(j = 0; j < BRAP_RM_P_MAX_RBUFS_PER_DSTCH; j++)
			{
				uiTempRBufId[j] = BRAP_RM_P_RBufPref4DstCh[uiDstChCnt][j];
			}
			if ((hRmTemp->eRBufState[uiTempRBufId[0]] == BRAP_RM_P_ObjectState_eFree)&&
				(hRmTemp->eRBufState[uiTempRBufId[1]] == BRAP_RM_P_ObjectState_eFree))
			{
				if ( bLastChMono )
				{
					/* Requires only one Ring buffer to be allocated */
					pResrcGrant->sCapResrcId.uiRbufId[0] = uiTempRBufId[0];
					pResrcGrant->sCapResrcId.uiRbufId[1] = BRAP_RM_P_INVALID_INDEX;
				}
				else
				{
					/* Requires Buffer Pair to be allocated */
					pResrcGrant->sCapResrcId.uiRbufId[0] = uiTempRBufId[0];
					pResrcGrant->sCapResrcId.uiRbufId[1] = uiTempRBufId[1];
				}
			}
			else
			{
			    /* There is no free source channel to be allocated */
        			ret = BERR_TRACE(BRAP_ERR_RESOURCE_EXHAUSTED);
	            		goto error; 
			}
			/* Update the RingBuffer state */
			hRmTemp->eRBufState[uiTempRBufId[0]] = BRAP_RM_P_ObjectState_eBusy;
			hRmTemp->eRBufState[uiTempRBufId[1]] = BRAP_RM_P_ObjectState_eBusy;

#else
			for(j = 0, k = 0; 
                    ((j < BRAP_RM_P_MAX_RBUFS) && (k < (uiNumRbufRequested))); j++)
			{
				uiTmpRBufId = BRAP_RM_P_RBufId[j];
				if(hRmTemp->eRBufState[uiTmpRBufId] == BRAP_RM_P_ObjectState_eFree)
				{
					BDBG_MSG(("CAP got this Rbuf[%d] for Cap path j=%d, k=%d",uiTmpRBufId,j,k));
					/* The allocated single buffer id is saved in index 0 always */
					pResrcGrant->sCapResrcId.uiRbufId[k] = uiTmpRBufId;
				
					/* Update the RingBuffer state */
					hRmTemp->eRBufState[uiTmpRBufId] = BRAP_RM_P_ObjectState_eBusy;

                    /* Need to allocate a dummy rbuf for the Playback path */
                    for (i=j; i<BRAP_RM_P_MAX_RBUFS; i++)
                    {
                        uiTmpRBufId = BRAP_RM_P_RBufId[i];
        				if(hRmTemp->eRBufState[uiTmpRBufId] == BRAP_RM_P_ObjectState_eFree)
        				{
        					BDBG_MSG(("CAP got this Rbuf[%d] for PB path i=%d, k=%d",uiTmpRBufId,i,k));
        					pResrcGrant->sOpResrcId[0].uiRbufId[k] = uiTmpRBufId;
        				
        					/* Update the RingBuffer state */
        					hRmTemp->eRBufState[uiTmpRBufId] = BRAP_RM_P_ObjectState_eBusy;
        					/* Increment k to indicate rbuf allocation */
        					k++;

        					if ( k == uiNumRbufRequested )
        						break;                            
    				    }
                    }                    
					if ( k == uiNumRbufRequested )
						break;
                    
				} /* if */
			} /* for j,k */

			if ( j >= BRAP_RM_P_MAX_RBUFS )
			{
				/* There is no free source channel to be allocated */
                BDBG_ERR(("RBUFS exhausted."));
				ret = BERR_TRACE(BRAP_ERR_RESOURCE_EXHAUSTED);
				goto error; 
			}

			/* If user has requested only a single channel, return an invalid index on second */
			if(uiNumRbufRequested == 1)
				pResrcGrant->sCapResrcId.uiRbufId[1] = BRAP_RM_P_INVALID_INDEX;

#endif		
		}

	}

#if (BCHP_CHIP == 7400 )
	for(i = 0; i < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; i++)
	{	
		if(pResrcReq->sOpPortReq[i].eOpPortType == (unsigned int)BRAP_RM_P_INVALID_INDEX)
		{
			/* Resources for this output audio channel is not requested, so skip it */
			continue;
		}

    	if ( pResrcReq->bAllocatePpm==true)
    	{		
        		/* Select the Ppm Channel */
        		for ( uiPpmChCnt=0; uiPpmChCnt<BRAP_RM_P_MAX_PPM_CHANNELS; uiPpmChCnt++ )
        		{
        			uiPpmId = BRAP_RM_P_PpmPref[uiPpmChCnt];
        			/* Check if this ppm channel is unused */
        			if(hRmTemp->sPpmChUsage[uiPpmId].uiUsageCount != false)
        			{
        				/* Already in use, check if next is available */
        				BDBG_MSG(("Already in use uiPpmChId[%d] usgCnt[%d]", 
        					uiPpmId, hRmTemp->sPpmChUsage[uiPpmId].uiUsageCount));
        				continue;
        			}
        			break;
        		}

        		/* Validate Ppm channel count */
        		if ( uiPpmChCnt >= BRAP_RM_P_MAX_PPM_CHANNELS )
        		{
        			/* There is no free source channel to be allocated */
        			ret = BERR_TRACE(BRAP_ERR_RESOURCE_EXHAUSTED);
        			goto error; 
        		}		

        		hRmTemp->sPpmChUsage[uiPpmId].uiUsageCount=true;
        		pResrcGrant->sOpResrcId[i].uiPpmId = uiPpmId;
                goto end; 

                
        }		
    }
#endif
    
    

	for(i = 0; i < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; i++)
	{	
		if(pResrcReq->sOpPortReq[i].eOpPortType == (unsigned int)BRAP_RM_P_INVALID_INDEX)
		{
			/* Resources for this output audio channel is not requested, so skip it */
			continue;
		}
		eOpPortType = pResrcReq->sOpPortReq[i].eOpPortType;
		pResrcGrant->sOpResrcId[i].eOutputPortType = eOpPortType;

		if ((eOpPortType == BRAP_OutputPort_eMai) ||
			(eOpPortType == BRAP_OutputPort_eFlex))
		{
			/* For MAI and Flex, take the Mixers and SPDIF Formaters 
			   corresponding to their inputs */
			pResrcGrant->sOpResrcId[i].uiMixerId 
				= BRAP_RM_P_MixerPref[pResrcReq->sOpPortReq[i].eMuxSelect];
			pResrcGrant->sOpResrcId[i].uiSpdiffmId 
				= BRAP_RM_P_SpdiffmPref[pResrcReq->sOpPortReq[i].eMuxSelect].uiSpdiffmId;
			pResrcGrant->sOpResrcId[i].uiSpdiffmStreamId 
				= BRAP_RM_P_SpdiffmPref[pResrcReq->sOpPortReq[i].eMuxSelect].uiSpdiffmChId;
		}
		else
		{
			pResrcGrant->sOpResrcId[i].uiMixerId 
				=  BRAP_RM_P_MixerPref[eOpPortType];
			pResrcGrant->sOpResrcId[i].uiSpdiffmId 
				= BRAP_RM_P_SpdiffmPref[eOpPortType].uiSpdiffmId;
			pResrcGrant->sOpResrcId[i].uiSpdiffmStreamId 
				= BRAP_RM_P_SpdiffmPref[eOpPortType].uiSpdiffmChId;
		}

		/* Update the SPDIF usage status only if the SPDIF formated is valid */
		if ( pResrcGrant->sOpResrcId[i].uiSpdiffmId != (unsigned int)BRAP_RM_P_INVALID_INDEX )
		{
			hRmTemp->sSpdiffmUsage[pResrcGrant->sOpResrcId[i].uiSpdiffmId].
				eSpdiffmChStatus[pResrcGrant->sOpResrcId[i].uiSpdiffmStreamId] = BRAP_RM_P_ObjectState_eBusy;
		}		
		
		/* Update the mixer state */
		hRmTemp->sMixerUsage[pResrcGrant->sOpResrcId[i].uiMixerId].uiUsageCount++;
		if( hRmTemp->sMixerUsage[pResrcGrant->sOpResrcId[i].uiMixerId].uiUsageCount 
												> BRAP_RM_P_MAX_MIXER_INPUTS)
		{
			hRmTemp->sMixerUsage[pResrcGrant->sOpResrcId[i].uiMixerId].uiUsageCount--;
			ret = BERR_TRACE(BRAP_ERR_RESOURCE_EXHAUSTED);
			goto error; 
		}

		/* Set the mixer input. Search and allocate the free input */
		for ( uiMixerInputId=0; uiMixerInputId<BRAP_RM_P_MAX_MIXER_INPUTS; uiMixerInputId++ )
		{
			if (hRmTemp->sMixerUsage[pResrcGrant->sOpResrcId[i].uiMixerId].
					eInputStatus[uiMixerInputId] == BRAP_RM_P_ObjectState_eFree)
			{
				pResrcGrant->sOpResrcId[i].uiMixerInputId = uiMixerInputId;
				break;
			}
		}
		if ( uiMixerInputId >= BRAP_RM_P_MAX_MIXER_INPUTS )
		{
			/* It is a serious error in maintaing the Mixer usage count */
			ret = BERR_TRACE(BRAP_ERR_BAD_DEVICE_STATE);
			BDBG_ASSERT(0);
			goto error; 
		}
		
		/* Update the mixer input state */
		hRmTemp->sMixerUsage[pResrcGrant->sOpResrcId[i].uiMixerId].
			eInputStatus[uiMixerInputId] = BRAP_RM_P_ObjectState_eBusy;
		
		/* Select the DP */
		pResrcGrant->sOpResrcId[i].uiDataPathId 
			= BRAP_RM_P_DpPref[pResrcGrant->sOpResrcId[i].uiMixerId];

		if ( pResrcReq->bAllocateSrcCh == false )
		{
			/* Allocation is done */
			goto end; 
		}

		/* Select the Source Channel */
		for ( uiSrcChCnt=0; uiSrcChCnt<BRAP_RM_P_MAX_SRCCH_PER_DP; uiSrcChCnt++ )
		{
			uiSrcChId = BRAP_RM_P_SrcChPref[pResrcGrant->sOpResrcId[i].uiDataPathId][uiSrcChCnt];
			/* Check if this source channel is unused */
			if(hRmTemp->sSrcChUsage[uiSrcChId].uiUsageCount != 0)
			{
				/* Already in use, check if next is available */
				BDBG_MSG(("Already in use uiSrcChId[%d] usgCnt[%d]", 
					uiSrcChId, hRmTemp->sSrcChUsage[uiSrcChId].uiUsageCount));
				continue;
			}
			break;
		}

		/* Validate src channel count */
		if ( uiSrcChCnt >= BRAP_RM_P_MAX_SRCCH_PER_DP )
		{
			/* There is no free source channel to be allocated */
			ret = BERR_TRACE(BRAP_ERR_RESOURCE_EXHAUSTED);
			goto error; 
		}		

		/* Update the source channle usage */
		hRmTemp->sSrcChUsage[uiSrcChId].uiUsageCount++;
		pResrcGrant->sOpResrcId[i].uiSrcChId = uiSrcChId;

		/* We need not allocate Ring buffers, if allocation for destination
		 * channel is requested. More over the required Ring buffers are
		 * already allocated for this case */
		if (  pResrcReq->bAllocateRBuf == false ||
			pResrcReq->bAllocateDstCh == true )
		{
			/* Allocation is done */
			goto end;
		}

#ifdef BCHP_7411_VER		
		for(j = 0; j < BRAP_RM_P_MAX_RBUFS_PER_SRCCH; j++)
		{
			uiTempRBufId[j] = BRAP_RM_P_RBufPref[uiSrcChId][j];
		}
		if ((hRmTemp->eRBufState[uiTempRBufId[0]] == BRAP_RM_P_ObjectState_eFree)&&
			(hRmTemp->eRBufState[uiTempRBufId[1]] == BRAP_RM_P_ObjectState_eFree))
		{

			if ( bLastChMono )
			{
				/* Requires only one Ring buffer to be allocated */
				pResrcGrant->sOpResrcId[i].uiRbufId[0] = uiTempRBufId[0];
				pResrcGrant->sOpResrcId[i].uiRbufId[1] = BRAP_RM_P_INVALID_INDEX;
			}
			else
			{
				/* Requires Buffer Pair to be allocated */
				pResrcGrant->sOpResrcId[i].uiRbufId[0] = uiTempRBufId[0];
				pResrcGrant->sOpResrcId[i].uiRbufId[1] = uiTempRBufId[1];
			}

                     /* Update the Ring Buffer status */
                     hRmTemp->eRBufState[uiTempRBufId[0]] = BRAP_RM_P_ObjectState_eBusy;
                     hRmTemp->eRBufState[uiTempRBufId[1]] = BRAP_RM_P_ObjectState_eBusy;
		}
              else
              {
                	/* There is no free source channel to be allocated */
			ret = BERR_TRACE(BRAP_ERR_RESOURCE_EXHAUSTED);
			goto error; 
              }
#else
		for(j = 0, k = 0; j < BRAP_RM_P_MAX_RBUFS && k < uiNumRbufRequested; j++)
		{
			uiTmpRBufId = BRAP_RM_P_RBufId[j];
			if(hRmTemp->eRBufState[uiTmpRBufId] == BRAP_RM_P_ObjectState_eFree)
			{
				BDBG_MSG(("got this Rbuf[%d] j=%d, k=%d",uiTmpRBufId,j,k));
				/*pResrcGrant->sOpResrcId[i].uiSrcChId = uiSrcChId;*/
				/* The allocated single buffer id is saved in index 0 always */
				pResrcGrant->sOpResrcId[i].uiRbufId[k] = uiTmpRBufId;
					
				/* Update the RingBuffer state */
				hRmTemp->eRBufState[uiTmpRBufId] = BRAP_RM_P_ObjectState_eBusy;

				/* Increment k to indicate rbuf allocation */
				k++;

				if ( k == uiNumRbufRequested )
					break;
			} /* if */
		} /* for j,k */

		if ( j >= BRAP_RM_P_MAX_RBUFS )
		{
			/* There is no free source channel to be allocated */
			ret = BERR_TRACE(BRAP_ERR_RESOURCE_EXHAUSTED);
			goto error; 
		}

		/* If user has requested only a single channel, return an invalid index on second */
		if(uiNumRbufRequested == 1)
			pResrcGrant->sOpResrcId[i].uiRbufId[1] = BRAP_RM_P_INVALID_INDEX;
		
#endif		
	}

	/* DSP is allocated only for a decode channel */
	if ((pResrcReq->bAllocateDsp == true) 
		&& (pResrcReq->eChannelType == BRAP_P_ChannelType_eDecode))
	{
		/* Presently there is only one DSP, so assign DSP Id to 0 */
		pResrcGrant->uiDspId = 0;
	}

	/* DSP Context is allocated only for a decode channel */
	if ((pResrcReq->bAllocateDspCtx== true) 
		&& (pResrcReq->eChannelType == BRAP_P_ChannelType_eDecode))
    	{
		uiTotDSPContext = hRmTemp->sDspUsage[0].uiDecCxtCount 
						+  hRmTemp->sDspUsage[0].uiPtCxtCount
						+  hRmTemp->sDspUsage[0].uiSrcCxtCount;

#ifndef BCHP_7411_VER /* For chips other than 7411 */
		uiTotDSPContext +=  hRmTemp->sDspUsage[0].uiEncCxtCount;
#endif        
		
		if ( uiTotDSPContext >= BRAP_RM_P_MAX_CXT_PER_DSP )
		{
			BDBG_ERR(("No DSP Contexts are free in DSP"));
			ret = BERR_TRACE(BRAP_ERR_RESOURCE_EXHAUSTED);
			goto error; 
		}

		if((pResrcReq->bSimulModePt == true) || (pResrcReq->bPassthrough==true))
		{
			/* It is the 2nd context of simultaneous mode. So the DSP Id and DSP 
			Context Id for the 1st context will be used here and these two 
			parameters of pResrcGrant will be ignored by the caller. 
			But internally DSP Context 2 can't be used further. 
			Hence mark DSP context 2 as busy. */
			if (hRmTemp->sDspUsage[0].eContext[BRAP_RM_P_SIM_MODE_PT_DSP_CONTEXT] == BRAP_RM_P_ObjectState_eBusy)
			{
				#ifndef RAP_I2S_COMPRESS_SUPPORT
				BDBG_ERR(("Context %d is already in use. ", BRAP_RM_P_SIM_MODE_PT_DSP_CONTEXT)); 
				ret = BERR_TRACE(BRAP_ERR_RESOURCE_EXHAUSTED);
				goto error; 
				#else
					goto checkfreecontext;
				#endif
			}
			else
			{
				pResrcGrant->uiDspContextId = BRAP_RM_P_SIM_MODE_PT_DSP_CONTEXT;
				/* Update the DSP state */
				hRmTemp->sDspUsage[0].eContext[pResrcGrant->uiDspContextId] 
					= BRAP_RM_P_ObjectState_eBusy;
				hRmTemp->sDspUsage[0].uiDecCxtCount++;	
				BDBG_MSG(("Allocated DSP Context is %d", pResrcGrant->uiDspContextId)); 
				goto end;
			}
		}	

#ifdef RAP_I2S_COMPRESS_SUPPORT        
checkfreecontext:
#endif
		{
			/* Check for the free transport context */
			for ( i=0; i < BRAP_RM_P_MAX_CXT_PER_DSP; i++)
			{
				if ( hRmTemp->sDspUsage[0].eContext[i] ==
					BRAP_RM_P_ObjectState_eFree )
				{
						BDBG_MSG(("Allocated DSP Context is %d", i)); 
						pResrcGrant->uiDspContextId = i;
						break;
				}
			}
			if ( i >= BRAP_RM_P_MAX_CXT_PER_DSP )
			{
				/* There is no free context available */
				BDBG_ERR(("There are no free DSP Context")); 
				ret = BERR_TRACE(BRAP_ERR_RESOURCE_EXHAUSTED);
				goto error; 
			}	
		}
		/* Update the DSP state */
		hRmTemp->sDspUsage[0].eContext[pResrcGrant->uiDspContextId] 
			= BRAP_RM_P_ObjectState_eBusy;
		hRmTemp->sDspUsage[0].uiDecCxtCount++;	
	}
end:   

               BDBG_MSG(("\nInput path resources:\n"));
                BDBG_MSG(("eInputPortType= %d\n"
                                    "uiRbufId[0] = %d \n"
                                    "uiRbufId[1] = %d\n"
                                    "uiDstChId = %d\n" ,                                  
                                    pResrcGrant->sCapResrcId.eInputPortType,
                                    pResrcGrant->sCapResrcId.uiRbufId[0],
                                    pResrcGrant->sCapResrcId.uiRbufId[1],
                                    pResrcGrant->sCapResrcId.uiDstChId ));
                
	for(i = 0; i < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; i++)
	{	
        if(pResrcReq->sOpPortReq[i].eOpPortType == (unsigned int)BRAP_RM_P_INVALID_INDEX)
		{
			/* Resources for this output audio channel is not requested, so skip it */
			continue;
		}
        
		/* Print the Granted Resources */
              BDBG_MSG(("Output path resources:"));
		BDBG_MSG((	"\n channel pair id:%d"
					"\n Output Port Type = %d"
					"\n Source Channel Id = %d"
					"\n Ppm Channel Id = %x"
					"\n Data Path Id = %d"
					"\n Mixer Id = %d"
					"\n Mixer Input Id = %d"
					"\n SPDIF Formatter Id = %d"
					"\n SPDIF Formatter Stream Id = %d",
					i, pResrcGrant->sOpResrcId[i].eOutputPortType,
					pResrcGrant->sOpResrcId[i].uiSrcChId,
					pResrcGrant->sOpResrcId[i].uiPpmId,
					pResrcGrant->sOpResrcId[i].uiDataPathId,
					pResrcGrant->sOpResrcId[i].uiMixerId,
					pResrcGrant->sOpResrcId[i].uiMixerInputId,
					pResrcGrant->sOpResrcId[i].uiSpdiffmId,
					pResrcGrant->sOpResrcId[i].uiSpdiffmStreamId));

        for(j = 0; j < BRAP_RM_P_MAX_RBUFS_PER_SRCCH; j++)
		{
			BDBG_MSG(("\nRing Buffer[%d] = %d", j, pResrcGrant->sOpResrcId[i].uiRbufId[j]));
		}
                
    }
	/* Currently we have only one FMM, so return FMM Id as 0 */
	pResrcGrant->uiFmmId = 0;

    BDBG_MSG((	"\nDSP object index = %d\nDSP context index = %d"
				"\nFMM object index = %d",	pResrcGrant->uiDspId, 
				pResrcGrant->uiDspContextId, pResrcGrant->uiFmmId));


	/* Copy the temporary RM handle content to the original RM handle */
	*hRm = *hRmTemp;


	/* Free the temporary RM handle */
	BKNI_Free( hRmTemp );
	BDBG_LEAVE(BRAP_RM_P_AllocateResources);
	return ret;	
	
error:
	/* Free the temporary RM handle */
	BKNI_Free( hRmTemp );
	return ret;
}

/***************************************************************************
Summary:

Description:
	This function frees all the resources corresponding to a particular 
	instance of audio operation that is getting closed.

Returns:
	BERR_SUCCESS - If successful
	BERR_OUT_OF_SYSTEM_MEMORY - If there is not enough memory

See Also:
	BRAP_RM_P_AllocateResources
**************************************************************************/
BERR_Code
BRAP_RM_P_FreeResources (
	BRAP_RM_Handle	hRm,						/* [in] Resource Manager handle */
	const BRAP_RM_P_ResrcGrant *pResrcGrant		/* [in] Indexed of the resources
												   allocated */
	)
{
	BERR_Code		ret = BERR_SUCCESS;
	unsigned int	i, j;
	BRAP_RM_Handle	hRmTemp;
#if 0
	BRAP_OutputPort		eOpPortType;
#endif
	unsigned int uiTMixerId;
	unsigned int uiTMixerInputId;
	unsigned int uiTSrcChId;

	BDBG_ENTER(BRAP_RM_P_FreeResources);

	/* Assert the function arguments*/
	BDBG_ASSERT(hRm);
	BDBG_ASSERT(pResrcGrant);
	
	/* Allocate a temporary RM Handle */
	hRmTemp = (BRAP_RM_Handle)BKNI_Malloc( sizeof(BRAP_RM_P_Object));
	if(hRmTemp == NULL)
	{
		ret = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		return ret;
	}

	/* Copy the original RM handle content to the temporary handle */
	*hRmTemp = *hRm;

#if (BCHP_CHIP == 7400)
	for(i = 0; i < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; i++)
	{	
	
        if(pResrcGrant->sOpResrcId[i].uiPpmId!=BRAP_RM_P_INVALID_INDEX)
        {
            for(j=0;j<BRAP_RM_P_MAX_PPM_CHANNELS;j++)
            {
                if(hRmTemp->sPpmChUsage[j].uiUsageCount!=false)
                {
                    hRmTemp->sPpmChUsage[pResrcGrant->sOpResrcId[i].uiPpmId].uiUsageCount=false;
                    BDBG_MSG(("freed Ppm Id %d ", pResrcGrant->sOpResrcId[i].uiPpmId));
                }
            }
        }
        
    } 
#endif



	for(i = 0; i < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; i++)
	{	
		if(pResrcGrant->sOpResrcId[i].eOutputPortType == (unsigned int)BRAP_RM_P_INVALID_INDEX)
		{
			/* Resources for this output audio channel doesn't exist, so skip it */
			continue;
		}
#if 0		
		eOpPortType = pResrcGrant->sOpResrcId[i].eOutputPortType;
#endif

		uiTMixerId = pResrcGrant->sOpResrcId[i].uiMixerId;
		uiTMixerInputId = pResrcGrant->sOpResrcId[i].uiMixerInputId;
		uiTSrcChId = pResrcGrant->sOpResrcId[i].uiSrcChId;

		BDBG_MSG(("\nMixer index = %d\nMixer Input index = %d",
				uiTMixerId, uiTMixerInputId));

		/* Update the mixer state */
		if(uiTMixerId != (unsigned int)BRAP_RM_P_INVALID_INDEX)
		{
			BDBG_ASSERT(hRmTemp->sMixerUsage[uiTMixerId].uiUsageCount > 0);
			hRmTemp->sMixerUsage[uiTMixerId].uiUsageCount--;

			if(hRmTemp->sMixerUsage[uiTMixerId].uiUsageCount == 0)
			{
				if ( pResrcGrant->sOpResrcId[i].uiSpdiffmId != (unsigned int)BRAP_RM_P_INVALID_INDEX )
				{
					hRmTemp->sSpdiffmUsage[pResrcGrant->sOpResrcId[i].uiSpdiffmId].
						eSpdiffmChStatus[pResrcGrant->sOpResrcId[i].uiSpdiffmStreamId] = BRAP_RM_P_ObjectState_eFree;
					BDBG_MSG(("\nSPDIF Formated ID = %d\nSPDIF Stream ID = %d",
						pResrcGrant->sOpResrcId[i].uiSpdiffmId, pResrcGrant->sOpResrcId[i].uiSpdiffmStreamId));

				}
			}

			hRmTemp->sMixerUsage[uiTMixerId].eInputStatus[uiTMixerInputId] = BRAP_RM_P_ObjectState_eFree;
		}

		if ( uiTSrcChId != (unsigned int)BRAP_RM_P_INVALID_INDEX)
		{
		    /* Update the source channel state */
    		BDBG_ASSERT(hRmTemp->sSrcChUsage[uiTSrcChId].uiUsageCount > 0);
	    	hRmTemp->sSrcChUsage[uiTSrcChId].uiUsageCount--;


		    /* Only if the particular source channel is unused,
    		free the ring buffers associated with it */
	    	if ( hRmTemp->sSrcChUsage[uiTSrcChId].uiUsageCount == 0 )
		    {
			    /* Update the ring buffer state */
    			for(j = 0; j < BRAP_RM_P_MAX_RBUFS_PER_SRCCH; j++)
	    		{
	    			/* Free only if the ring buffer was in use */
	    			if(pResrcGrant->sOpResrcId[i].uiRbufId[j] != (unsigned int)BRAP_RM_P_INVALID_INDEX)
			    	{
			    		hRmTemp->eRBufState[pResrcGrant->sOpResrcId[i].uiRbufId[j]]
			        		= BRAP_RM_P_ObjectState_eFree;
	    				BDBG_MSG(("\nFree RBUF ID = %d",pResrcGrant->sOpResrcId[i].uiRbufId[j]));			
	    			}
	    		}
		    }
	    }
		else
		{
		   /* TODO currently this occurs only for clone output */
		   /* Later modify to handle it correctly */
		   goto end; 
		}
	}

	/* Update the Destination channel states */
	if ( pResrcGrant->sCapResrcId.uiDstChId != (unsigned int)BRAP_RM_P_INVALID_INDEX )
		hRmTemp->eDestChState[pResrcGrant->sCapResrcId.uiDstChId] = 
			BRAP_RM_P_ObjectState_eFree;

	/* Update the Ring buffer states */
	for(j = 0; j < BRAP_RM_P_MAX_RBUFS_PER_DSTCH; j++)
	{
		/* Free only if the ring buffer was in use */
		if(pResrcGrant->sCapResrcId.uiRbufId[j] != (unsigned int)BRAP_RM_P_INVALID_INDEX)
    		{
    			hRmTemp->eRBufState[pResrcGrant->sCapResrcId.uiRbufId[j]]
        			= BRAP_RM_P_ObjectState_eFree;
			BDBG_MSG(("\nFree RBUF ID = %d",pResrcGrant->sCapResrcId.uiRbufId[j]));			
		}
	}

    
	if((pResrcGrant->uiDspId != (unsigned int)BRAP_RM_P_INVALID_INDEX) 
             && (hRmTemp->sDspUsage[pResrcGrant->uiDspId].uiDecCxtCount > 0))
    {   
    	hRmTemp->sDspUsage[pResrcGrant->uiDspId].uiDecCxtCount--;
    	hRmTemp->sDspUsage[pResrcGrant->uiDspId].eContext[pResrcGrant->uiDspContextId]
    			= BRAP_RM_P_ObjectState_eFree;
    	BDBG_MSG(("\nFree DSP  ID = %d",pResrcGrant->uiDspId));			
    	BDBG_MSG(("\nFree DSP Context ID = %d",pResrcGrant->uiDspContextId));		
    }   
end:	
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
 
Description:
 This function returns Mixer Index associated with the output port from the array BRAP_RM_P_MixerPref.
  This PI cant be used for Mai.
Returns:
 BERR_SUCCESS - If successful
 
See Also:
 None
**************************************************************************/

BERR_Code
BRAP_RM_P_GetMixerForOpPort (
    BRAP_RM_Handle hRm,        /* [in] Resource Manager handle */
    BRAP_OutputPort   eOpPortType, /* [in] Output port */
    unsigned int *pMixerId     /* [out]Mixer index */
)
{
	BERR_Code     eStatus;
    BDBG_ENTER (BRAP_RM_P_GetMixerForOpPort);

	BKNI_EnterCriticalSection();
	eStatus = BRAP_RM_P_GetMixerForOpPort_isr (hRm, eOpPortType, pMixerId);
	BKNI_LeaveCriticalSection();
    BDBG_LEAVE (BRAP_RM_P_GetMixerForOpPort);
	return eStatus;
}

BERR_Code
BRAP_RM_P_GetMixerForOpPort_isr (
    BRAP_RM_Handle hRm,        /* [in] Resource Manager handle */
    BRAP_OutputPort   eOpPortType, /* [in] Output port */
    unsigned int *pMixerId     /* [out]Mixer index */
)
{
 
	BDBG_ENTER(BRAP_RM_P_GetMixerForOpPort_isr);

	/* Assert the function arguments*/
	BDBG_ASSERT(hRm);
	BDBG_ASSERT(pMixerId);
	BSTD_UNUSED(hRm);

	/* Get the Mixer Index */
	*pMixerId = BRAP_RM_P_MixerPref[eOpPortType];

	BDBG_LEAVE(BRAP_RM_P_GetMixerForOpPort_isr);

	return BERR_SUCCESS;
}

/***************************************************************************
Summary:
 
Description:
 This function returns SPDIF Formater index (if any) & 
 SPDIF Formater stream index (if SPDIF Formater is associated) 
 associated with the output port from the array BRAP_RM_P_SpdiffmPref

 This PI cant be used for Mai.
 
Returns:
 BERR_SUCCESS - If successful
 
See Also:
 None
**************************************************************************/
BERR_Code
BRAP_RM_P_GetSpdifFmForOpPort (
    BRAP_RM_Handle hRm,        /* [in] Resource Manager handle */
    BRAP_OutputPort   eOpPortType, /* [in] Output port */
    unsigned int *pSpdiffmId,  /* [out]SPDIF Formater index */
    unsigned int *pSpdiffmStreamId /* [out]SPDIF Formater stream index */
)
{
 
	BDBG_ENTER(BRAP_RM_P_GetSpdifFmForOpPort);

	/* Assert the function arguments*/
	BDBG_ASSERT(hRm);
	BDBG_ASSERT(pSpdiffmId);
	BDBG_ASSERT(pSpdiffmStreamId);
	BSTD_UNUSED(hRm);

	/* Get the SPDIF Formater index (if any) & SPDIF Formater
	stream index (if SPDIF Formater is associated) associated with the 
	output */
	*pSpdiffmId = BRAP_RM_P_SpdiffmPref[eOpPortType].uiSpdiffmId;
	*pSpdiffmStreamId = BRAP_RM_P_SpdiffmPref[eOpPortType].uiSpdiffmChId;

	BDBG_LEAVE(BRAP_RM_P_GetSpdifFmForOpPort);

	return BERR_SUCCESS;
}

BERR_Code BRAP_RM_P_GetDpId (
	unsigned int uiMixerIndex,  /*[in] Mixer index */
	unsigned int * pDpIndex     /*[out] DP index corresponding to
								  input mixer index */
)
{
    BDBG_ENTER (BRAP_RM_P_GetDpId);
    BDBG_ASSERT (pDpIndex);
	if (uiMixerIndex >= BRAP_RM_P_MAX_MIXERS) {
		BDBG_ERR(("BRAP_RM_P_GetDpId: Mixer Index should be less than %d",BRAP_RM_P_MAX_MIXERS));
		return BERR_TRACE(BERR_INVALID_PARAMETER);

	}

    *pDpIndex = BRAP_RM_P_DpPref[uiMixerIndex];

    BDBG_LEAVE (BRAP_RM_P_GetDpId);
    return BERR_SUCCESS;
}

/* This PI cant be used for Mai.*/
BERR_Code
BRAP_RM_P_GetDpForOpPort (
    BRAP_RM_Handle hRm,        /* [in] Resource Manager handle */
    BRAP_OutputPort   eOpPortType, /* [in] Output port */
    unsigned int *pDpId     /* [out]Dp index */
)
{
	BERR_Code     eStatus;
	unsigned int uiMixerId;
       BDBG_ENTER (BRAP_RM_P_GetDpForOpPort);

	eStatus = BRAP_RM_P_GetMixerForOpPort (hRm, eOpPortType, &uiMixerId);
	eStatus = BRAP_RM_P_GetDpId (uiMixerId, pDpId);

    	BDBG_LEAVE (BRAP_RM_P_GetDpForOpPort);
	return eStatus;
}

BERR_Code BRAP_RM_P_GetSpdifFmId (
	unsigned int uiStreamIndex,  /*[in] SPDIFFM Stream index */
	unsigned int * pSpdifFmIndex     /*[out] SPDIFFM corresponding to this SPDIFFM stream */
)
{
    BDBG_ENTER (BRAP_RM_P_GetSpdifFmId);
    BDBG_ASSERT (pSpdifFmIndex);
	if (uiStreamIndex > BRAP_RM_P_MAX_SPDIFFM_STREAMS) {
		BDBG_ERR(("BRAP_RM_P_GetSpdifFmId: SPDIFFM Stream Index should be less than %d",
                    BRAP_RM_P_MAX_SPDIFFM_STREAMS));
		return BERR_TRACE(BERR_INVALID_PARAMETER);

	}
  
    /* Currently there is only one SPDIF formatter
         So we allways return 0 */
         
    *pSpdifFmIndex = 0;
    BDBG_LEAVE (BRAP_RM_P_GetSpdifFmId);
    return BERR_SUCCESS;
}

BERR_Code BRAP_RM_P_GetDpStreamId (
	unsigned int uiSrcCh,       /*[in] Src Ch index for this stream */
	unsigned int uiDpId,        /*[in] DP for this stream */
	unsigned int * pStreamId   /*[out] Stream's index in this DP */
)
{
    int i;
    BERR_Code ret = BERR_SUCCESS;

    BDBG_ENTER (BRAP_RM_P_GetDpStreamId);
    BDBG_ASSERT (pStreamId);
    
	if (uiSrcCh > BRAP_RM_P_MAX_SRC_CHANNELS) {
		BDBG_ERR(("BRAP_RM_P_GetDpStreamId: Source Channel Index is %d. It should be less than %d",
                    uiSrcCh, BRAP_RM_P_MAX_SRC_CHANNELS));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}
	if (uiDpId > BRAP_RM_P_MAX_DPS) {
		BDBG_ERR(("BRAP_RM_P_GetDpStreamId: DP Index should be less than %d",
                    BRAP_RM_P_MAX_DPS));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

    for (i=0; i<BRAP_RM_P_MAX_SRCCH_PER_DP; i++)
    { 
        /* TODO: Here we assume BRAP_RM_P_DpStreamId is of size [BRAP_RM_P_MAX_DPS][BRAP_RM_P_MAX_SRCCH_PER_DP] */
        if ( uiSrcCh == BRAP_RM_P_DpStreamId [uiDpId][i])
        {
            break;
        }
    }
    *pStreamId = i;

    BDBG_LEAVE (BRAP_RM_P_GetDpStreamId);
    return ret;
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
	
#ifdef BCHP_7411_VER
    /* For 7411, the only permissible output ports are I2S0 and SPDIF. */
   	/* NOTE: these checks should always be updated to reflect latest supported
      	options on each platform */
   	switch (eOutputPort)
   	{
       	/* Currently supported output ports:  */
   	  	case BRAP_OutputPort_eI2s0:
   		case BRAP_OutputPort_eSpdif:
       	case BRAP_OutputPort_eI2s1:
        case BRAP_OutputPort_eI2s2:
             break;
       	/* Not supported */            
   		case BRAP_OutputPort_eDac0:
       	case BRAP_OutputPort_eDac1:
       	case BRAP_OutputPort_eMai:
       	case BRAP_OutputPort_eFlex:
       	case BRAP_OutputPort_eRfMod:
   		default:
			BDBG_ERR(("BRAP_RM_P_IsOutputPortSupported: output port %d not supported", 
					  eOutputPort));
        	rc = BRAP_ERR_OUPUT_PORT_NOT_SUPPORTED;
   	}
#elif (BRAP_7401_FAMILY == 1)
	/* For 7401 */
   	switch (eOutputPort)
	{
      	/* Currently supported output ports:  */
   		case BRAP_OutputPort_eI2s0:
       	case BRAP_OutputPort_eDac0:
       	case BRAP_OutputPort_eSpdif:
       	case BRAP_OutputPort_eMai:
       	case BRAP_OutputPort_eFlex:
            break;
       	/* Not supported */
       	case BRAP_OutputPort_eI2s1:
       	case BRAP_OutputPort_eI2s2:
       	case BRAP_OutputPort_eDac1:
       	case BRAP_OutputPort_eRfMod:
   		default:
			BDBG_ERR(("BRAP_RM_P_IsOutputPortSupported: output port %d not supported", 
					  eOutputPort));
        	rc = BRAP_ERR_OUPUT_PORT_NOT_SUPPORTED;
   	}
#elif ( BCHP_CHIP == 7400 )
	/* For 7400 */
   	switch (eOutputPort)
	{
      	/* Currently supported output ports:  */
   		case BRAP_OutputPort_eI2s0:
       	case BRAP_OutputPort_eDac0:
       	case BRAP_OutputPort_eSpdif:
       	case BRAP_OutputPort_eMai:
       	case BRAP_OutputPort_eFlex:
       	case BRAP_OutputPort_eI2s1:
       	case BRAP_OutputPort_eI2s2:
       	case BRAP_OutputPort_eDac1:
              	break;
       	/* Not supported */
       	case BRAP_OutputPort_eRfMod:
   		default:
			BDBG_ERR(("BRAP_RM_P_IsOutputPortSupported: output port %d not supported", 
					  eOutputPort));
        	rc = BRAP_ERR_OUPUT_PORT_NOT_SUPPORTED;
   	}
#endif

	BDBG_LEAVE(BRAP_RM_P_IsOutputPortSupported);
	return rc;
}


BERR_Code BRAP_RM_P_MatchingMixerDPIds(
    BRAP_ChannelHandle  hRapCh,     /* [in] The RAP Channel handle */
    unsigned int        uiMixerId,  /* [in] Mixer id */
    unsigned int        uiDpId,     /* [in] Data path id */
    bool                *bMatched   /* [out] true if both mixer and 
                                       dp id matches with ids available 
                                       with hRapCh */
    )
{
    unsigned int uiChnPair;
    
    BDBG_ENTER(BRAP_RM_P_MatchingMixerDPIds);

    BDBG_ASSERT(hRapCh);
    BDBG_ASSERT(bMatched);

    *bMatched = false;    
    /* TODO: need to handle the case  for cloned ports */
    for(uiChnPair=0; uiChnPair < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; uiChnPair++)
    {
        if((hRapCh->sRsrcGrnt.sOpResrcId[uiChnPair].uiMixerId == uiMixerId) &&
           (hRapCh->sRsrcGrnt.sOpResrcId[uiChnPair].uiDataPathId == uiDpId)) 
        {
            /* Both mixer id and DP id used by the the channel handle */
            *bMatched = true;
            return BERR_TRACE(BERR_SUCCESS);     
         }/* for uiChnPair */
    }/* for */
    
    BDBG_LEAVE(BRAP_RM_P_MatchingMixerDPIds);
    return BERR_SUCCESS;
}


#ifndef BCHP_7411_VER /* For chips other than 7411 */
/* Encoder support in RM */
/***************************************************************************
Summary:

Description:
	This function allocates all the resources required for opening a 
	particular instance of audio encoder operation.

Returns:
	BERR_SUCCESS - If successful

See Also:
	BRAP_RM_P_FreeResourcesEnc
**************************************************************************/
BERR_Code
BRAP_RM_P_AllocateResourcesEnc (
	BRAP_RM_Handle				hRm,		/* [in] Resource Manager handle */
	const BRAP_RM_P_EncResrcReq	*pEncResrcReq,	/* [in] Required resources */
	BRAP_RM_P_EncResrcGrant		*pEncResrcGrant/* [out] Indexed of the resources
											   allocated */
	)
{
	BERR_Code		ret = BERR_SUCCESS;
	unsigned int	i, j;
	BRAP_RM_Handle	hRmTemp;
	unsigned int	uiDSPCnt = 0;	
	unsigned int	uiTotDSPContext = 0;
	unsigned int	k = 0;
	unsigned int 	uiNumRbufRequested = 0;
	unsigned int 	uiTmpRBufId = 0;

	BDBG_ENTER(BRAP_RM_P_AllocateResourcesEnc);
	BSTD_UNUSED(uiDSPCnt); /* Will be needed later for multiple DSPs */

	/* Assert the function arguments*/
	BDBG_ASSERT(hRm);
	BDBG_ASSERT(pEncResrcReq);
	BDBG_ASSERT(pEncResrcGrant);

	/* Allocate a temporary RM Handle */
	hRmTemp = (BRAP_RM_Handle)BKNI_Malloc( sizeof(BRAP_RM_P_Object));
	if(hRmTemp == NULL)
	{
		ret = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		return ret;
	}

	/* Copy the original RM handle content to the temporary handle */
	*hRmTemp = *hRm;

	/* Initialize pResrcGrant entries to invalid */
	pEncResrcGrant->uiDspId = BRAP_RM_P_INVALID_INDEX;
	pEncResrcGrant->uiDspContextId = BRAP_RM_P_INVALID_INDEX;
	for(i = 0; i < BRAP_RM_P_MAX_RBUFS_PER_ENC_INPUT; i++)
		pEncResrcGrant->uiEncRbufId[i] = BRAP_RM_P_INVALID_INDEX;

		/* Check if RBuf allocation is requested */
	if(( pEncResrcReq->eEncBufDataMode == BRAP_EncBufDataMode_eMono) ||
		( pEncResrcReq->eEncBufDataMode == BRAP_EncBufDataMode_eStereoInterleaved))
		uiNumRbufRequested = 1;
	else if (pEncResrcReq->eEncBufDataMode == BRAP_EncBufDataMode_eStereoNoninterleaved)
		uiNumRbufRequested = 2;
	else if (pEncResrcReq->eEncBufDataMode == BRAP_EncBufDataMode_eFiveptoneLFENonInterLeaved)
		uiNumRbufRequested = 6;
	if ( pEncResrcReq->bAllocateRBuf == true )
	{
		for(j = 0, k = 0; j < BRAP_RM_P_MAX_RBUFS && k < uiNumRbufRequested; j++)
		{
			uiTmpRBufId = BRAP_RM_P_RBufId[j];
			if(hRmTemp->eRBufState[uiTmpRBufId] == BRAP_RM_P_ObjectState_eFree)
			{
				BDBG_ERR(("ENC got this Rbuf[%d] j=%d, k=%d",uiTmpRBufId,j,k));
				/* The allocated single buffer id is saved in index 0 always */
				pEncResrcGrant->uiEncRbufId[k] = uiTmpRBufId;
			
				/* Update the RingBuffer state */
				hRmTemp->eRBufState[uiTmpRBufId] = BRAP_RM_P_ObjectState_eBusy;

				/* Increment k to indicate rbuf allocation */
				k++;

				if ( k == uiNumRbufRequested )
					break;
			} /* if */
		} /* for j,k */

		if ( j >= BRAP_RM_P_MAX_RBUFS )
		{
			/* There is no free ring buffer to be allocated */
			ret = BERR_TRACE(BRAP_ERR_RESOURCE_EXHAUSTED);
			goto error; 
		}

	}

	/* DSP is allocated only for an encode channel */
	if(pEncResrcReq->eChannelType == BRAP_P_ChannelType_eEncode) 	
	{
		/* Presently there is only one DSP, so assign DSP Id to 0 */
		pEncResrcGrant->uiDspId = 0;

		uiTotDSPContext = hRmTemp->sDspUsage[pEncResrcGrant->uiDspId].uiDecCxtCount +
			hRmTemp->sDspUsage[pEncResrcGrant->uiDspId].uiPtCxtCount+
			hRmTemp->sDspUsage[pEncResrcGrant->uiDspId].uiEncCxtCount +
			hRmTemp->sDspUsage[pEncResrcGrant->uiDspId].uiSrcCxtCount;
		
		if ( uiTotDSPContext >= BRAP_RM_P_MAX_CXT_PER_DSP )
		{
			BDBG_ERR(("No DSP Contexts are free in DSP %d", uiDSPCnt));
			ret = BERR_TRACE(BRAP_ERR_RESOURCE_EXHAUSTED);
			goto error; 
		}

		/* TODO :- Change this later for working with any context */
			/* Check for the free DSP context */
#if 0
			for ( i=0; i <= BRAP_RM_P_MAX_CXT_PER_DSP; i++)
#else
	/* As of now, encoder can't run in CXT0 due to scheduler issues, so leaving CXT0 */
			for ( i=1; i < BRAP_RM_P_MAX_CXT_PER_DSP; i++)
#endif
			{
				if ( hRmTemp->sDspUsage[pEncResrcGrant->uiDspId].eContext[i] ==
					BRAP_RM_P_ObjectState_eFree )
				{
					pEncResrcGrant->uiDspContextId = i;
					break;
				}
			}
			if ( i >= BRAP_RM_P_MAX_CXT_PER_DSP )
			{
				/* There is no free context available */
    			BDBG_ERR(("No DSP Contexts are free in DSP %d", uiDSPCnt));                
				ret = BERR_TRACE(BRAP_ERR_RESOURCE_EXHAUSTED);
				goto error; 
			}	
	
		/* Check if the appropriate DSP Context is free or not. */
		if(hRmTemp->sDspUsage[pEncResrcGrant->uiDspId].eContext[pEncResrcGrant->uiDspContextId] 
		   != BRAP_RM_P_ObjectState_eFree)
		{
			BDBG_ERR(("DSP Context %d of DSP %d is not free", 
					  pEncResrcGrant->uiDspId, pEncResrcGrant->uiDspContextId));
			ret = BERR_TRACE(BRAP_ERR_RESOURCE_EXHAUSTED);
			goto error; 
		}
	
		/* Update the DSP state */
		hRmTemp->sDspUsage[pEncResrcGrant->uiDspId].eContext[pEncResrcGrant->uiDspContextId] 
			= BRAP_RM_P_ObjectState_eBusy;
		hRmTemp->sDspUsage[pEncResrcGrant->uiDspId].uiEncCxtCount++;	
	}/* if( pResrcReq->eChannelType == BRAP_P_ChannelType_eEncode ) */

	/* Copy the temporary RM handle content to the original RM handle */
	*hRm = *hRmTemp;

	/* Free the temporary RM handle */
	BKNI_Free( hRmTemp );
	BDBG_LEAVE(BRAP_RM_P_AllocateResourcesEnc);
	return ret;	
	
error:
	/* Free the temporary RM handle */
	BKNI_Free( hRmTemp );
	return ret;
}

/***************************************************************************
Summary:

Description:
	This function frees all the resources corresponding to a particular 
	instance of audio Encoder operation that is getting closed.

Returns:
	BERR_SUCCESS - If successful

See Also:
	BRAP_RM_P_AllocateResourcesEnc
**************************************************************************/
BERR_Code
BRAP_RM_P_FreeResourcesEnc (
	BRAP_RM_Handle	hRm,						/* [in] Resource Manager handle */
	const BRAP_RM_P_EncResrcGrant *pEncResrcGrant		/* [in] Indexed of the resources
												   allocated */
	)
{
	BERR_Code		ret = BERR_SUCCESS;
	unsigned int	 j;
	BRAP_RM_Handle	hRmTemp;

	BDBG_ENTER(BRAP_RM_P_FreeResourcesEnc);

	/* Assert the function arguments*/
	BDBG_ASSERT(hRm);
	BDBG_ASSERT(pEncResrcGrant);

	/* Allocate a temporary RM Handle */
	hRmTemp = (BRAP_RM_Handle)BKNI_Malloc( sizeof(BRAP_RM_P_Object));
	if(hRmTemp == NULL)
	{
		ret = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		return ret;
	}

	/* Copy the original RM handle content to the temporary handle */
	*hRmTemp = *hRm;

	/* Update the Ring buffer states */
	for(j = 0; j < BRAP_RM_P_MAX_RBUFS_PER_ENC_INPUT; j++)
	{
		/* Free only if the ring buffer was in use */
		if(pEncResrcGrant->uiEncRbufId[j] != (unsigned int)BRAP_RM_P_INVALID_INDEX)
    		{
    			hRmTemp->eRBufState[pEncResrcGrant->uiEncRbufId[j]]
        			= BRAP_RM_P_ObjectState_eFree;
			BDBG_MSG(("\nFree RBUF ID = %d",pEncResrcGrant->uiEncRbufId[j]));			
		}
	}

	/* Update the DSP Context state and DSP Usage */
	if(hRmTemp->sDspUsage[pEncResrcGrant->uiDspId].uiEncCxtCount > 0)
    {   
    	hRmTemp->sDspUsage[pEncResrcGrant->uiDspId].uiEncCxtCount--;
    	hRmTemp->sDspUsage[pEncResrcGrant->uiDspId].eContext[pEncResrcGrant->uiDspContextId]
    			= BRAP_RM_P_ObjectState_eFree;
    	BDBG_MSG(("\nFree DSP  ID = %d",pEncResrcGrant->uiDspId));			
    	BDBG_MSG(("\nFree DSP Context ID = %d",pEncResrcGrant->uiDspContextId));		
    }   

	/* Copy the temporary RM handle content to the original RM handle */
	*hRm = *hRmTemp;
	
	/* Free the temporary RM handle */
	BKNI_Free( hRmTemp );

	BDBG_MSG(("\nFreed the resource"));		

	BDBG_LEAVE(BRAP_RM_P_FreeResourcesEnc);
	
	return ret;
}
#endif

/* End of File */
