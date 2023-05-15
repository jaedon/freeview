/***************************************************************************
*     Copyright (c) 2004-2009, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brap_rm_priv.h $
* $brcm_Revision: Hydra_Software_Devel/49 $
* $brcm_Date: 9/18/09 8:05p $
*
* Module Description: Refer Module Overview given below
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/rap/base/brap_rm_priv.h $
* 
* Hydra_Software_Devel/49   9/18/09 8:05p srajapur
* SW7400-2519 : [7400] Resource exhausted with dual decode / dual
* passthru
* 
* Hydra_Software_Devel/48   6/25/09 2:09p srajapur
* PR54189 : [7400] Coverity Defect ID:15720 OVERRUN_STATIC brap.c
* Product=97400linux
* 
* Hydra_Software_Devel/47   11/26/08 9:26p srajapur
* PR 48071 : [7401] Coverity Defect ID:12275 OVERRUN_STATIC
* bsettop_base_rap_740x.c Product=97401linux
* 
* Hydra_Software_Devel/46   11/12/08 7:36p srajapur
* PR48893 :[7400] 74XX merged release for 7400,7401,7403 and 7118 [For
* more info see Description]
* PR45669: [7403] Adding support for DTS Certification.
* PR43467 : [7403] Implementation of DRA on 7403
* PR46472 : [7400] Implement Dolby Volume Leveler and Modeler per
* Motorola's Requirements
* PR42285 : [7401] Added Audio descriptor support
* PR44899 :[7401] Independent delay on spdif causes left and right
* channel to be out of phase
* PR44614 : [7400] [7401] Few seconds of noise on SPDIF/HDMI during PCM
* to Compressed & vice-a-versa transition
* PR42342: [7401] Added Downmix Method parameter in the AACHE user config
* structure for Sumitomo
* 
* Hydra_Software_Devel/46   11/12/08 7:35p srajapur
* PR48893: [7400] 74XX merged release for 7400,7401,7403 and 7118 [For
* more info see Description]
* 
* Hydra_Software_Devel/46   11/12/08 7:34p srajapur
* PR48893 : [7400] 74XX merged release for 7400,7401,7403 and 7118 [For
* more info see Description]
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
* Hydra_Software_Devel/46   11/12/08 7:33p srajapur
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
* 
* PR42342: [7401] Added Downmix Method parameter in the AACHE user config
* structure for Sumitomo
* 
* Hydra_Software_Devel/46   11/12/08 7:33p srajapur
* 
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
* Hydra_Software_Devel/46   11/12/08 7:33p srajapur
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
* Hydra_Software_Devel/46   11/12/08 7:32p srajapur
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
* Hydra_Software_Devel/46   11/12/08 7:31p srajapur
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
* Hydra_Software_Devel/45   8/21/08 1:25p srajapur
* PR42140 : [7400] Implemented  PPM adjustments as in 7401
* 
* Hydra_Software_Devel/44   5/19/08 10:22p srajapur
* PR42140 : [7400] Implemented  PPM adjustments as in 7401
* 
* Hydra_Software_Devel/43   5/19/08 9:24p srajapur
* PR42140 : [7400] Implemented  PPM adjustments as in 7401
* 
* Hydra_Software_Devel/42   3/7/08 2:52p srajapur
* PR 39994 : [7400,7401,7403,7118] In dual mode: can not play any VC1
* stream on second window and the sytem crashes in certain sequence of
* action
* - For passthrough mode the context 2 is used always.(Expect two
* passthrough support mode)
* 
* Hydra_Software_Devel/41   2/28/08 8:38p srajapur
* PR37419 : [7400] Added I2S Pass-through of two compressed ouput.
* 
* Hydra_Software_Devel/40   10/17/07 7:03p bselva
* PR 35525:[7400,7401,7403,7118] The DSP context allocation is moved to
* start time to support dual decode for 7400.
* 
* Hydra_Software_Devel/39   5/30/07 7:34p gautamk
* PR27106:Fixing number of warnings in brap module
* 
* Hydra_Software_Devel/38   3/8/07 10:51a rjain
* PR 28524: Replacing (BCHP_CHIP == 7401) || (BCHP_CHIP == 7118) ||
* (BCHP_CHIP == 7403) with (BRAP_7401_FAMILY == 1)
* 
* Hydra_Software_Devel/37   12/19/06 1:26p rjain
* PR 25670: add 7403 support to RAP
* 
* Hydra_Software_Devel/36   8/2/06 5:59p bselva
* PR 22486: Merged the changes for multichannel and concurrent stereo
* downmix support for 7400 to main branch
* 
* Hydra_Software_Devel/35   5/26/06 3:04p bselva
* PR 21823: Increased the number for decode channels. Becuase for 7400 we
* can have 2 decode + 1 pass through channel.
* 
* Hydra_Software_Devel/34   5/9/06 11:23a rjain
* PR 21481: adding support for 7118 to RAP PI
* 
* Hydra_Software_Devel/33   4/20/06 4:25p rjain
* PR 20039: fixing DSP context allocation
* 
* Hydra_Software_Devel/32   4/11/06 6:05p sushmit
* PR18604: Modifying
* #if (( BCHP_CHIP == 7401 )||( BCHP_CHIP == 7400 ))
* With code like
* #ifndef BCHP_7411_VER
* 
* Hydra_Software_Devel/31   4/3/06 6:06p rjain
* PR 20318: Implementing review comments for AM
* 
* Hydra_Software_Devel/30   3/30/06 4:38p bselva
* PR 19878:Implemented the code review coments
* 
* Hydra_Software_Devel/29   3/28/06 12:19p rnayak
* PR20439: Restore Bangalore Raptor files after magnum vob corruption on
* 17th March 2006
* 
* Hydra_Software_Devel/29   3/21/06 6:27p sushmit
* PR18604: Updating review comments.
* 
* Hydra_Software_Devel/28   2/27/06 4:16p sushmit
* PR18604: Fixing 7411 compilation issues.
* 
* Hydra_Software_Devel/27   2/17/06 10:36a sushmit
* PR18604: Merging Encoder to latest.
* 
* Hydra_Software_Devel/26   2/17/06 9:52a sushmit
* PR18604: Merging Encoder to latest.
* 
* Hydra_Software_Devel/25   2/8/06 3:35p kagrawal
* PR 18264: 1. Changed BRAP_RM_P_MAX_PCM_CHANNELS to 4 for 7401
* 2. Removed redundant BRAP_RM_P_MAX_CAPPB_CHANNELS
* 
* Hydra_Software_Devel/24   2/7/06 10:14a bselva
* PR 19168: Checking in the changes done during 7400 bringup
* 
* Hydra_Software_Devel/RAP_AUD_ENC/4   2/6/06 4:15p sushmit
* PR18604: Merging latest Decoder PI to Encoder PI branch
* 
* Hydra_Software_Devel/RAP_AUD_ENC/2   1/16/06 5:37p sushmit
* PR18604: Updating Audio Encoder PI.
* 
* Hydra_Software_Devel/21   12/9/05 4:41p rjain
* PR 18264: Capture PI Development
* 
* Hydra_Software_Devel/20   11/14/05 11:31a rjain
* PR 18129: Remove bCloneOutput from RM. Other compilation related
* changes for 7411
* 
* Hydra_Software_Devel/19   11/14/05 9:54a rjain
* PR 18129: Adding support for routing to multiple output ports
* 
* Hydra_Software_Devel/18   11/13/05 4:34p kagrawal
* PR 17590: Added a new routine BRAP_RM_P_GetChannelsUsingMixer
* 
* Hydra_Software_Devel/17   11/12/05 6:20p kagrawal
* PR 17590: Added an internal routine BRAP_RM_P_GetDecChannel
* 
* Hydra_Software_Devel/16   11/10/05 11:54a kagrawal
* PR 17590: Merging PCM Playback and Mixing changes to
* Hydra_Software_Devel
* 
* Hydra_Software_Devel/Kapil_PCM_Playback/2   11/1/05 9:45a kagrawal
* Implemented review comments on PCM playback and mixing
* 
* Hydra_Software_Devel/Kapil_PCM_Playback/1   10/24/05 5:04p kagrawal
* PR 17590: Initial check in for PCM Playback and Mixing
* 
* Hydra_Software_Devel/15   11/9/05 4:07p bselva
* PR 17590: Added support for HDMI output
* 
* Hydra_Software_Devel/14   10/18/05 4:58p rjain
* PR 17651: Changing checks for BCHP_CHIP ==/!= 7401 to check for
* BCHP_7411_VER instead
* 
* Hydra_Software_Devel/13   9/13/05 2:28p rjain
* PR 17042: Adding ISR version of BRAP_MIXER_Set/GetMute PI.
* 
* Hydra_Software_Devel/12   9/13/05 7:41p bmishra
* PR 16148: Merged 7401 related changes
* 
* Hydra_Software_Devel/BLR_RAP_7401_Bringup/1   7/20/05 2:30p bmishra
* PR 16148: Modified to support HiFiDAC for 7401
* 
* Hydra_Software_Devel/11   6/18/05 3:32p rjain
* PR 15925: PI changes required for routing decoded data to I2S and SPDIF
* ports simultanesouly
* 
* Hydra_Software_Devel/BLR_RAP_7411C0_RM_REDESIGN/2   6/13/05 7:40p bselva
* Implemented the code reveiw coments
* 
* Hydra_Software_Devel/8   5/4/05 11:09a rjain
* PR 14507:- Added proper States, open/start counts for modules.
* - Added appropriate checks to handle multiple calls to _Open(),
* _Start(), _Stop(), _Close() for modules.
* - Corrected SPDIFFM handle stored in hFmm / channel handle to be
* SPDIFFM Stream handle.
* - Some clean up, mroe comments/debug messages.
* - Added BRAP_RM_P_GetSpdifFmId() to RM
* 
* Hydra_Software_Devel/7   4/15/05 2:57p bmishra
* PR 14507: Simultaneous mode related changes
* 
* Hydra_Software_Devel/6   3/10/05 3:25p rjain
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
* Hydra_Software_Devel/5   2/16/05 1:46p rjain
* PR 14143: Implementing some comments from Phase 1 Code review done till
* date
* 
* Hydra_Software_Devel/4   2/8/05 7:01p rjain
* PR13082: Some bug fixes and clean up
* 
* Hydra_Software_Devel/3   1/12/05 5:59p rjain
* PR13082: adding BRAP_RM_P_GetDpId
* 
* Hydra_Software_Devel/2   1/11/05 11:47a rjain
* PR13082: Added functions BRAP_RM_P_GetMixerForOpPort() and
* BRAP_RM_P_GetSpdifFmForOpPort()
* 
* Hydra_Software_Devel/1   1/10/05 8:24p nitinb
* PR 13082: Moved files to new directory structure
* 
***************************************************************************/


/*=************************ Module Overview ********************************
Resource Manager is an internal module of Raptor audio PI.

For various audio operations, Raptor audio PI makes use of following
hardware/firmware/memory resources.
1. DSP contexts (Firmware resource)
2. DRAM ring buffers (Memory resource)
3. Source/Destination channels (FMM hardware resource)
4. Mixers (FMM hardware resource)
5. SPDIF-Formatter (FMM hardware resource)
6. Output ports (FMM hardware resource)

Above resources are shared for following audio operations.
1. Live Decode/Pass Thru/SRC/Simulmode in DSP
2. Playback from hard-disk/memory
3. Capture

Resource Manager maintains above resources and provides them to Audio
Manager as per the request. Resource Manager has the knowledge of
mappings between various resources. It allocates resources, 
for optimum usage. However, Resource Manager doesn't know what all
resources getting used for a particular instance of audio operation. It is
the responsibility of Audio Manager to provide list of all the 
resources to be freed once an instance of audio operation is closed.

Other modules interact with Resource Manager for following purposes.
1. When a new audio channel (instance of audio operation) is formed
   (or closed). Audio Manager requests Resource Manager to allocate 
   (or free) all the required resources for this operation.
2. Swapping of the outputs.
3. Copying one output on other one.
4. For mixing operation.
5. Obtaining mapping information.
***************************************************************************/
#ifndef _BRAP_RM_PRIV_H__
#define _BRAP_RM_PRIV_H__

#include "brap_types.h"
#include "brap_types_priv.h"
#include "brap_pcm.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BRAP_RM_P_Object			*BRAP_RM_Handle;





/* Macros for maximum number of supported decode and passthru operations.
 * Decode part of simul mode is counted in BRAP_P_MAX_DECODE_OPERATIONS
 * and pass thru part of simul mode is count in BRAP_P_MAX_PASSTHRU_OPERATIONS
 */
#ifdef BCHP_7411_VER /* For 7411 C0 and D0 */
#define BRAP_RM_P_MAX_DECODE_SUPPORTED			1	
#define BRAP_RM_P_MAX_PASSTHRU_SUPPORTED		1	
#else /* Non 7411 chips */
#if (BRAP_7401_FAMILY == 1)

#if (BRAP_AD_SUPPORTED == 1)
#define BRAP_RM_P_MAX_DECODE_SUPPORTED			2	
#else
#define BRAP_RM_P_MAX_DECODE_SUPPORTED			1	
#endif

#define BRAP_RM_P_MAX_PASSTHRU_SUPPORTED		1	
#define BRAP_RM_P_MAX_ENCODE_SUPPORTED			1
#elif (BCHP_CHIP == 7400)
/* TODO: Change this number once dual decode support is added */
#define BRAP_RM_P_MAX_DECODE_SUPPORTED			2
#ifdef RAP_I2S_COMPRESS_SUPPORT
#define BRAP_RM_P_MAX_PASSTHRU_SUPPORTED		2
#else
#define BRAP_RM_P_MAX_PASSTHRU_SUPPORTED		1	
#endif
#define BRAP_RM_P_MAX_ENCODE_SUPPORTED			1
#endif
/* TODO: Define these macros for other future chips here */
#endif



/***************************************************************************
Summary:
	Used in array of resource indexes. If the array size is more than
	number of resource index entries, remaining entries are initialized
	to this macro value.
***************************************************************************/
#define BRAP_RM_P_INVALID_INDEX				((unsigned int)(-1))


/***************************************************************************
Summary:
	Macros defining maximum number of resources available.
***************************************************************************/
#if defined ( BCHP_7411_VER ) /* For the 7411 */ 
#define BRAP_RM_P_MAX_DSPS						1	/* Total Number of DSP */	
#define BRAP_RM_P_MAX_CXT_PER_DSP				3	/* Maximum context per DSP */
#define BRAP_RM_P_MAX_FMMS						1	/* Total number of FMMs */
#define BRAP_RM_P_MAX_RBUFS						12	/* Total Ring buffers */
#define BRAP_RM_P_MAX_SRC_CHANNELS				6	/* Total Source Channels */
#define BRAP_RM_P_MAX_PPM_CHANNELS				4	/* Total PPM channels */
#define BRAP_RM_P_MAX_DST_CHANNELS				2	/* Total Destination Channels */
#define BRAP_RM_P_MAX_SRC_DST_CHANNELS			(BRAP_RM_P_MAX_SRC_CHANNELS+BRAP_RM_P_MAX_DST_CHANNELS)
														/* Total Source channels + Destination channels */
#define BRAP_RM_P_MAX_FIFOS_PER_SRC_CHANNEL	2	/* Number of FIFOs pers source channel */
#define BRAP_RM_P_MAX_MIXERS						6	/* Total number of mixers */
#define BRAP_RM_P_MAX_MIXER_INPUTS				3	/* Maximum number of inputs to a mixer */
#define BRAP_RM_P_MAX_SPDIFFMS					1	/* Total number of SPDIF formaters */
#define BRAP_RM_P_MAX_SPDIFFM_STREAMS			2	/* Total no. of streams across ALL SPDIFFM*/
#define BRAP_RM_P_MAX_OUTPUTS					6	/* Total number of outputs */
														/* This value is derived from the Maximum
														 * value of the supported eOutputPort enum
														 */
#define BRAP_RM_P_MAX_DPS							2	/* Total number of Data Paths */

#define BRAP_RM_P_MAX_SRCCH_PER_DP				4	/* Maximum source channel per data path */
#define BRAP_RM_P_MAX_RBUFS_PER_SRCCH			2	/* Maximum ring buffers per source channel */
#define BRAP_RM_P_MAX_RBUFS_PER_DSTCH			2	/* Maximum ring buffers per Destination channel */
#define BRAP_RM_P_MAX_RBUFS_PER_PORT			(BRAP_RM_P_MAX_RBUFS_PER_SRCCH)
														/* Maximum ring buffers per output port */
#define BRAP_RM_P_MAX_MIXER_PER_DP				4 	/* only 3 of the 4 mixers per DP are functional */

#define BRAP_RM_P_MAX_DEC_CHANNELS				2	/* Max decode channels */
#define BRAP_RM_P_MAX_PCM_CHANNELS				4	/* Max PCM playback channels */
#define BRAP_RM_P_MAX_CAP_CHANNELS				2	/* Max capture channels */

#define BRAP_RM_P_MAX_DOWNMIXED_CHANNELS		0	/* Max Downmixed channel pairs */
#define BRAP_RM_P_MAX_OP_CHANNELS				( 6 + BRAP_RM_P_MAX_DOWNMIXED_CHANNELS )	/* Max o/p audio channels */
#define BRAP_RM_P_MAX_OP_CHANNEL_PAIRS			(BRAP_RM_P_MAX_OP_CHANNELS >> 1)	
														/* Max o/p audio channels pairs */
#elif (BRAP_7401_FAMILY == 1)
#define BRAP_RM_P_MAX_DSPS						1	/* Total Number of DSP */	
#define BRAP_RM_P_MAX_CXT_PER_DSP				3	/* Maximum context per DSP */
#define BRAP_RM_P_MAX_FMMS						1	/* Total number of FMMs */
#define BRAP_RM_P_MAX_RBUFS						8	/* Total Ring buffers */
#define BRAP_RM_P_MAX_PPM_CHANNELS				4	/* Total PPM channels */
#define BRAP_RM_P_MAX_SRC_CHANNELS				4	/* Total Source Channels */
#define BRAP_RM_P_MAX_DST_CHANNELS				1	/* Total Destination Channels */
#define BRAP_RM_P_MAX_SRC_DST_CHANNELS			(BRAP_RM_P_MAX_SRC_CHANNELS+BRAP_RM_P_MAX_DST_CHANNELS)
														/* Total Source channels + Destination channels */
#define BRAP_RM_P_MAX_FIFOS_PER_SRC_CHANNEL	2	/* Number of FIFOs pers source channel */
#define BRAP_RM_P_MAX_MIXERS						4	/* Total number of mixers */
#define BRAP_RM_P_MAX_MIXER_INPUTS				3	/* Maximum number of inputs to a mixer */
#define BRAP_RM_P_MAX_SPDIFFMS					1	/* Total number of SPDIF formaters */
#define BRAP_RM_P_MAX_SPDIFFM_STREAMS			2	/* Total no. of streams across ALL SPDIFFM*/
#define BRAP_RM_P_MAX_OUTPUTS					BRAP_OutputPort_eMax	/* Total number of outputs */
														/* This value is derived from the Maximum
														 * value of the supported eOutputPort enum
														 */
#define BRAP_RM_P_MAX_DPS							1	/* Total number of Data Paths */

#define BRAP_RM_P_MAX_SRCCH_PER_DP				4	/* Maximum source channel per data path */
#define BRAP_RM_P_MAX_RBUFS_PER_SRCCH			2	/* Maximum ring buffers per source channel */
#define BRAP_RM_P_MAX_RBUFS_PER_DSTCH			2	/* Maximum ring buffers per Destination channel */
#define BRAP_RM_P_MAX_RBUFS_PER_PORT			(BRAP_RM_P_MAX_RBUFS_PER_SRCCH)
														/* Maximum ring buffers per output port */
#define BRAP_RM_P_MAX_MIXER_PER_DP				4 	/* only 3 of the 4 mixers per DP are functional */

#define BRAP_RM_P_MAX_DEC_CHANNELS				2	/* Max decode channels */
/* Encoder support in RM */
#define BRAP_RM_P_MAX_ENC_CHANNELS				1	/* Max encode channels */
#define BRAP_RM_P_MAX_RBUFS_PER_ENC_INPUT		6	/* Maximum ring buffers per encoder input channel */

#define BRAP_RM_P_MAX_PCM_CHANNELS				4	/* Max PCM playback channels */
#define BRAP_RM_P_MAX_CAP_CHANNELS				1	/* Max capture channels */

#if (BRAP_AD_SUPPORTED == 1)
#define BRAP_RM_P_MAX_DOWNMIXED_CHANNELS		2	/* Max Downmixed channels */
#else
#define BRAP_RM_P_MAX_DOWNMIXED_CHANNELS		0	/* Max Downmixed channels */
#endif

#define BRAP_RM_P_MAX_OP_CHANNELS				( 6 + BRAP_RM_P_MAX_DOWNMIXED_CHANNELS )	/* Max o/p audio channels */
#define BRAP_RM_P_MAX_OP_CHANNEL_PAIRS			(BRAP_RM_P_MAX_OP_CHANNELS >> 1)	
														/* Max o/p audio channels pairs */
#elif ( BCHP_CHIP == 7400 )
#define BRAP_RM_P_MAX_DSPS						1	/* Total Number of DSP */	
#define BRAP_RM_P_MAX_CXT_PER_DSP				6	/* Maximum context per DSP */
#define BRAP_RM_P_MAX_FMMS						1	/* Total number of FMMs */
#define BRAP_RM_P_MAX_RBUFS						16	/* Total Ring buffers */
#define BRAP_RM_P_MAX_SRC_CHANNELS				8	/* Total Source Channels */
#define BRAP_RM_P_MAX_PPM_CHANNELS				4	/* Total PPM channels */
#define BRAP_RM_P_MAX_DST_CHANNELS				2	/* Total Destination Channels */
#define BRAP_RM_P_MAX_SRC_DST_CHANNELS			(BRAP_RM_P_MAX_SRC_CHANNELS+BRAP_RM_P_MAX_DST_CHANNELS)
														/* Total Source channels + Destination channels */
#define BRAP_RM_P_MAX_FIFOS_PER_SRC_CHANNEL	2	/* Number of FIFOs pers source channel */
#define BRAP_RM_P_MAX_MIXERS						8	/* Total number of mixers */
#define BRAP_RM_P_MAX_MIXER_INPUTS				3	/* Maximum number of inputs to a mixer */
#define BRAP_RM_P_MAX_SPDIFFMS					1	/* Total number of SPDIF formaters */
#define BRAP_RM_P_MAX_SPDIFFM_STREAMS			2	/* Total no. of streams across ALL SPDIFFM*/
#define BRAP_RM_P_MAX_OUTPUTS					BRAP_OutputPort_eMax	/* Total number of outputs */
														/* This value is derived from the Maximum
														 * value of the supported eOutputPort enum
														 */
#define BRAP_RM_P_MAX_DPS							2	/* Total number of Data Paths */

#define BRAP_RM_P_MAX_SRCCH_PER_DP				4	/* Maximum source channel per data path */
#define BRAP_RM_P_MAX_RBUFS_PER_SRCCH			2	/* Maximum ring buffers per source channel */
#define BRAP_RM_P_MAX_RBUFS_PER_DSTCH			2	/* Maximum ring buffers per Destination channel */
#define BRAP_RM_P_MAX_RBUFS_PER_PORT			(BRAP_RM_P_MAX_RBUFS_PER_SRCCH)
														/* Maximum ring buffers per output port */
#define BRAP_RM_P_MAX_MIXER_PER_DP				5 	/* 4 mixers per DP are functional */

#define BRAP_RM_P_MAX_DEC_CHANNELS				4	/* Max decode channels */
/* Encoder support in RM */
#define BRAP_RM_P_MAX_ENC_CHANNELS				1	/* Max encode channels */
#define BRAP_RM_P_MAX_RBUFS_PER_ENC_INPUT		6	/* Maximum ring buffers per encoder input channel */

#define BRAP_RM_P_MAX_PCM_CHANNELS				4	/* Max PCM playback channels */
#define BRAP_RM_P_MAX_CAP_CHANNELS				1	/* Max capture channels */

#define BRAP_RM_P_MAX_DOWNMIXED_CHANNELS		2	/* Max Downmixed channels */
#define BRAP_RM_P_MAX_OP_CHANNELS				( 6 + BRAP_RM_P_MAX_DOWNMIXED_CHANNELS )	/* Max o/p audio channels */
#define BRAP_RM_P_MAX_OP_CHANNEL_PAIRS			(BRAP_RM_P_MAX_OP_CHANNELS >> 1)	
														/* Max o/p audio channels pairs */
#else
#error "Not support chip type"
#endif

/***************************************************************************
Summary:
	Enumerations for states of an object
***************************************************************************/
typedef enum BRAP_RM_P_ObjectState
{
	BRAP_RM_P_ObjectState_eFree,	/* Free state */
	BRAP_RM_P_ObjectState_eBusy	/* Busy state */
} BRAP_RM_P_ObjectState;

/***************************************************************************
Summary:
	Structure for SPDIF Formater usage
***************************************************************************/
typedef struct BRAP_RM_P_SpdiffmUsage
{
	BRAP_RM_P_ObjectState	eSpdiffmChStatus[BRAP_RM_P_MAX_SPDIFFM_STREAMS];	
								/* Array to maintain the states of the SPDIF Formater channel */
} BRAP_RM_P_SpdiffmUsage;

/***************************************************************************
Summary:	
	Structure for SPDIF Formater mappings
***************************************************************************/
typedef struct BRAP_RM_P_SpdiffmMapping
{	
	unsigned int	uiSpdiffmId;		/* Index of the SPDIF Formater used,	
								BRAP_RM_P_INVALID_INDEX indicates SPDIF Formater not used */
	unsigned int	uiSpdiffmChId;	/* Index of the SPDIF Formater channel used. 
								This field is valid if uiSpdiffmId != BRAP_RM_P_INVALID_INDEX */
} BRAP_RM_P_SpdiffmMapping;


/***************************************************************************
Summary:
	Structure for Source channel usage
***************************************************************************/
typedef struct BRAP_RM_P_SrcChUsage
{
	unsigned int	uiUsageCount;
					/* Total number of Clients using this Source Channel.
					Zero indicates this is free */
} BRAP_RM_P_SrcChUsage;

/***************************************************************************
Summary:
	Structure for Source channel usage
***************************************************************************/
typedef struct BRAP_RM_P_PPmChUsage
{
    bool uiUsageCount;
					/* Total number of Clients using this PPm Channel.
					Zero indicates this is free */
} BRAP_RM_P_PpmChUsage;

/***************************************************************************
Summary:
	Structure for mixer usage
***************************************************************************/
typedef struct BRAP_RM_P_MixerUsage
{
	unsigned int				uiUsageCount;	
								/* Total number of Clients using this mixer.
								Zero indicates this is free */
	BRAP_RM_P_ObjectState	eInputStatus[BRAP_RM_P_MAX_MIXER_INPUTS];
								/* Array to maintain the states of the Mixer Inputs */
} BRAP_RM_P_MixerUsage;


/***************************************************************************
Summary:
	Structure for DSP object and DSP context usage
***************************************************************************/
typedef struct BRAP_RM_P_DspUsage
{
	BRAP_RM_P_ObjectState	eContext[BRAP_RM_P_MAX_CXT_PER_DSP];
								/* Array to maintain the states of the DSP contexts */
	unsigned int			uiDecCxtCount;	/* Number of decoder contexts
											   currently opened in DSP */
	unsigned int			uiSrcCxtCount;	/* Number of SRC contexts
											   currently opened in DSP */
	unsigned int			uiPtCxtCount;	/* Number of pass-thru contexts
											   currently opened in DSP */
#ifndef BCHP_7411_VER /* For chips other than 7411 */
/* Encoder support in RM */
	unsigned int			uiEncCxtCount;	/* Number of encoder contexts
											   currently opened in DSP */ 
#endif
} BRAP_RM_P_DspUsage;

/***************************************************************************
Summary:
	Handle structure for Resource Manager.
***************************************************************************/
typedef struct BRAP_RM_P_Object
{
	BRAP_RM_P_MixerUsage	sMixerUsage[BRAP_RM_P_MAX_MIXERS];
								/* Array of Mixer usage usage strucrure */
	BRAP_RM_P_SpdiffmUsage	sSpdiffmUsage[BRAP_RM_P_MAX_SPDIFFMS];
								/* Array of SPDIF formater usage strucrure */
	BRAP_RM_P_SrcChUsage	sSrcChUsage[BRAP_RM_P_MAX_SRC_CHANNELS];
								/* Array of source channel usage strucrure */
	BRAP_RM_P_PpmChUsage	sPpmChUsage[BRAP_RM_P_MAX_PPM_CHANNELS];
								/* Array of Ppm usage strucrure */								
	BRAP_RM_P_DspUsage		sDspUsage[BRAP_RM_P_MAX_DSPS];
								/* Array of DSP usage strucrure */
	BRAP_RM_P_ObjectState	eRBufState[BRAP_RM_P_MAX_RBUFS];
								/* Array to maintain the states of the Ring buffers */
	BRAP_RM_P_ObjectState	eDestChState[BRAP_RM_P_MAX_DST_CHANNELS];
								/* Array to maintain the states of the Ring buffers */
} BRAP_RM_P_Object;

/***************************************************************************
Summary:
	Structure for output port resource request details
***************************************************************************/
typedef struct BRAP_RM_P_OpPortReq
{
	BRAP_OutputPort		eOpPortType;	/* Output port type */
	BRAP_OutputPort		eMuxSelect;		/* If output port type is MAI or
									   Flex, then which input to use. */
} BRAP_RM_P_OpPortReq;

/***************************************************************************
Summary:
	Structure for requesting required resources
***************************************************************************/
typedef struct BRAP_RM_P_ResrcReq
{
	BRAP_P_ChannelType	eChannelType;	/* Type of audio channel */
    BRAP_BufDataMode 	eBufDataMode; 	/* Used only when eChannelType is 
    									   PCM playback or capture. This will 
    									   decide the number of ring buffers 
    									   required - mono and stereo-
    									   interleaved require 1 buffer while 
    									   stereo-non-interleaved requires 2 */									   
	unsigned int	uiNumOpPorts;	/* Number of output ports required 
									if channelType is Decode or
									playback */
    bool            bAllocateDsp;  /* TRUE: allocate a DSP context */									
	bool			bSimulModePt;	/* TRUE: If current set of resources 
									are required for the Pass-thru 
									operation of Simultaneous mode.
										   
									FALSE: Otherwise.

									If this is true, then 
									uiTransContextId, bLastChMono, 
									bCloneOutput, uiNumOpPorts are 
									ignored and uiNumOpPorts is 
									considered always to be 1 and 
									the output port for it will 
									be sOpPortReq[0]. */
       bool  bPassthrough; /* for Passthrough mode */									
	BRAP_RM_P_OpPortReq sOpPortReq[BRAP_RM_P_MAX_OP_CHANNEL_PAIRS];
										/* Output port information */
	bool			bLastChMono;		/* Set to true if number of Input 
									channels is odd. For example,
									if number of input channels
									= 5, then numOPPorts = 3
									and bLastChMono = true. If
									number of input channels = 6,
									then numOPPorts = 3 and
									bLastChMono = false */
						
	bool			bAllocateRBuf;		/* TRUE: If you require a RBUF to be allocated */
	bool			bAllocateSrcCh;		/* TRUE: If Source channel is required to be allocated */
	bool			bAllocatePpm;		/* TRUE: If PPM is required to be allocated */	
	bool			bAllocateDstCh;		/* TRUE: If Dest channel is required to be allocated */
	bool			bAllocateDspCtx;		/* TRUE: If DSP Context is required to be allocated */
} BRAP_RM_P_ResrcReq;

#ifndef BCHP_7411_VER /* For chips other than 7411 */
/* Encoder support in RM */
/***************************************************************************
Summary:
	Structure for requesting required resources for audio encoder
***************************************************************************/
typedef struct BRAP_RM_P_EncResrcReq
{
	BRAP_P_ChannelType	eChannelType;	/* Type of audio channel */
       BRAP_EncBufDataMode 	eEncBufDataMode; 	/* Used to determine the number of RBUFs required */
	bool			bAllocateRBuf;		/* TRUE: If you require a RBUF to be allocated */
} BRAP_RM_P_EncResrcReq;
#endif

/***************************************************************************
Summary:
	Structure for allotted resources associated with output port
***************************************************************************/
typedef struct BRAP_RM_P_OpPathResrc
{
	BRAP_OutputPort		eOutputPortType;
	unsigned int	uiRbufId[BRAP_RM_P_MAX_RBUFS_PER_PORT];	
									/* Indexes of the allotted ring
									   buffers. Unused entries of this
									   array will be initialized to
									   BRAP_RM_P_DONT_CARE */
	unsigned int	uiSrcChId;		/* Source channel index */
	unsigned int	uiPpmId;		/* Adaptive rate control index */									
	unsigned int	uiDataPathId;	/* Alloted DP index */
	unsigned int	uiMixerId;		/* Mixer Index */
	unsigned int	uiMixerInputId;	/* Mixer input index */
	unsigned int	uiSpdiffmId;	/* Alloted SPDIF Formater index */
	unsigned int	uiSpdiffmStreamId;/* Alloted SPDIF Formater stream
									   index */
} BRAP_RM_P_OpPathResrc;

/***************************************************************************
Summary:
	Structure for allotted resources associated with input port
***************************************************************************/
typedef struct BRAP_RM_P_CapturePathResrc
{
	BRAP_Input		eInputPortType;
	unsigned int	uiRbufId[BRAP_RM_P_MAX_RBUFS_PER_DSTCH];	
									/* Indexes of the allotted ring
									   buffers. Unused entries of this
									   array will be initialized to
									   BRAP_RM_P_DONT_CARE */
	unsigned int	uiDstChId;				
} BRAP_RM_P_CapturePathResrc;

/***************************************************************************
Summary:
	This structure is initialized by Resource Manager with indexes of
	allotted resources.
***************************************************************************/
typedef struct BRAP_RM_P_ResrcGrant
{
	unsigned int	uiNumOpPorts;		/* Number of output ports granted 
										   if channelType is Decode or
										   playback */
	BRAP_RM_P_OpPathResrc		sOpResrcId[BRAP_RM_P_MAX_OP_CHANNEL_PAIRS];
	BRAP_RM_P_CapturePathResrc	sCapResrcId;
	unsigned int	uiDspId;			/* Allotted DSP index */
	unsigned int	uiDspContextId;	/* Allotted DSP context index */
	unsigned int	uiFmmId;		/* Allotted FMM index */
	
} BRAP_RM_P_ResrcGrant;

#ifndef BCHP_7411_VER /* For chips other than 7411 */
/***************************************************************************
Summary:
	This structure is initialized by Resource Manager with indexes of
	allotted resources for audio encoder.
***************************************************************************/
typedef struct BRAP_RM_P_EncResrcGrant
{
	unsigned int	uiDspId;			/* Allotted DSP index */
	unsigned int	uiDspContextId;	/* Allotted DSP context index */
	unsigned int    uiEncRbufId[BRAP_RM_P_MAX_RBUFS_PER_ENC_INPUT]; 
} BRAP_RM_P_EncResrcGrant;
#endif

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
	);

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
	);

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
	);

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
	);

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
	BRAP_RM_P_EncFreeResources
**************************************************************************/
BERR_Code
BRAP_RM_P_AllocateResourcesEnc (
	BRAP_RM_Handle				hRm,		/* [in] Resource Manager handle */
	const BRAP_RM_P_EncResrcReq	*pEncResrcReq,	/* [in] Required resources */
	BRAP_RM_P_EncResrcGrant		*pEncResrcGrant/* [out] Indexed of the resources
											   allocated */
	);

/***************************************************************************
Summary:

Description:
	This function frees all the resources corresponding to a particular 
	instance of audio encoder operation that is getting closed.

Returns:
	BERR_SUCCESS - If successful

See Also:
	BRAP_RM_P_EncAllocateResources
**************************************************************************/
BERR_Code
BRAP_RM_P_FreeResourcesEnc (
	BRAP_RM_Handle	hRm,						/* [in] Resource Manager handle */
	const BRAP_RM_P_EncResrcGrant *pEncResrcGrant		/* [in] Indexed of the resources
												   allocated */
	);
#endif

/***************************************************************************
Summary:
 
Description:
 This function returns Mixer Index associated with the output port.
 
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
);


BERR_Code
BRAP_RM_P_GetMixerForOpPort_isr (
    BRAP_RM_Handle hRm,        /* [in] Resource Manager handle */
    BRAP_OutputPort   eOpPortType, /* [in] Output port */
    unsigned int *pMixerId     /* [out]Mixer index */
);

/***************************************************************************
Summary:
 
Description:
 This function returns SPDIF Formater index (if any) & 
 SPDIF Formater stream index (if SPDIF Formater is associated) 
 associated with the output port.
 
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
);

/***************************************************************************
Summary:
	This function returns DP index correspondig to Ouput port

Returns:
	BERR_SUCCESS - If successful

See Also:
**************************************************************************/
BERR_Code
BRAP_RM_P_GetDpForOpPort (
    BRAP_RM_Handle hRm,        /* [in] Resource Manager handle */
    BRAP_OutputPort   eOpPortType, /* [in] Output port */
    unsigned int *pDpId     /* [out]Dp index */
);

/***************************************************************************
Summary:
	This function returns DP index correspondig to input mixer index.

Returns:
	BERR_SUCCESS - If successful

See Also:
**************************************************************************/

BERR_Code BRAP_RM_P_GetDpId(
	unsigned int uiMixerIndex,  /*[in] Mixer index*/
	unsigned int * pDpIndex     /*[out] DP index corresponding to
								  input mixer index */
	);

/***************************************************************************
Summary:

Description:
	This function returns the Srouce channel index in this DP

Returns:
	BERR_SUCCESS - If successful

See Also:
**************************************************************************/
BERR_Code BRAP_RM_P_GetDpStreamId (
	unsigned int uiSrcCh,       /*[in] Src Ch index for this stream */
	unsigned int uiDpId,        /*[in] DP for this stream */
	unsigned int * pStreamId   /*[out] Stream's index in this DP */
);

/***************************************************************************
Summary:
	This function returns the SPDIFFM index for a given SPDIFFM Stream Index

Returns:
	BERR_SUCCESS - If successful

See Also:
**************************************************************************/
BERR_Code BRAP_RM_P_GetSpdifFmId (
	unsigned int uiStreamIndex,  /*[in] SPDIFFM Stream index */
	unsigned int * pSpdifFmIndex     /*[out] SPDIFFM corresponding to this SPDIFFM stream */
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

BERR_Code BRAP_RM_P_MatchingMixerDPIds(
    BRAP_ChannelHandle  hRapCh,     /* [in] The RAP Channel handle */
    unsigned int        uiMixerId,  /* [in] Mixer id */
    unsigned int        uiDpId,     /* [in] Data path id */
    bool                *bMatched   /* [out] true if both mixer and 
                                       dp id matches with ids available 
                                       with hRapCh */
);

#ifdef __cplusplus
}
#endif

#endif /* _BRAP_RM_PRIV_H__ */
/* End of File */

