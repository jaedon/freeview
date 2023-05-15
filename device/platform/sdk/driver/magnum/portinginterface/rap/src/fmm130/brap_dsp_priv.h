/***************************************************************************
*     Copyright (c) 2004-2010, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brap_dsp_priv.h $
* $brcm_Revision: Hydra_Software_Devel/99 $
* $brcm_Date: 5/10/10 5:37p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/rap/base/brap_dsp_priv.h $
* 
* Hydra_Software_Devel/99   5/10/10 5:37p srajapur
* SW7400-2688 : [7400] Added Support for BRAP_DSPCHN_AudioType_ePcmWav
* algorithm
* 
* Hydra_Software_Devel/98   9/10/09 10:34a srajapur
* SW7400-2465 : [7400] allocated 12K APM memory for two decode two
* passthru feature
* 
* Hydra_Software_Devel/97   7/21/09 5:26p srajapur
* PR53240 : [7403] DRA decoder development on 7403 chip
* 
* Hydra_Software_Devel/96   4/6/09 4:49p srajapur
* PR 53700 : [7400,7401,7403,7118] Added Downloadable scheduler code for
* code memory optimization
* 
* Hydra_Software_Devel/95   2/25/09 6:17p srajapur
* PR52480: [7400] Name changing SRS VIQ to SRS TrueVolume
* 
* Hydra_Software_Devel/94   1/12/09 10:59p srajapur
* PR50613 :[7400] Added SRS VIQ post processing algorithm for 7400
* 
* Hydra_Software_Devel/93   11/25/08 3:18p srajapur
* PR48893 : [7400] 74XX merged release for 7400,7401,7403 and 7118 [For
* more info see Description]
* 
* Hydra_Software_Devel/92   11/24/08 10:03p srajapur
* PR48893 : [7400] 74XX merged release for 7400,7401,7403 and 7118 [For
* more info see Description]
* PR46472 : [7400] Implement Dolby Volume Leveler and Modeler per
* Motorola's Requirements
* PR42285 : [7401] Added Audio descriptor support
* 
* Hydra_Software_Devel/91   11/12/08 7:29p srajapur
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
* Hydra_Software_Devel/90   5/19/08 5:23p srajapur
* PR42245 : [7400] Added 7.1 Channel Pass-Thru Support for DDP on HDMI
* output
* 
* Hydra_Software_Devel/89   4/28/08 2:28p srajapur
* PR42131 : [7401] Added support for AACHEV2
* 
* Hydra_Software_Devel/88   4/11/08 6:34p srajapur
* PR41579:[7401] WMA certification - Added API to program the config
* structure for WMA.
* 
* Hydra_Software_Devel/87   2/15/08 10:03a srajapur
* PR37898 : [7401,7118,7403,7400] Added ASTM support and Added an API to
* Enble and Disable ASTM
* 
* Hydra_Software_Devel/86   11/29/07 5:34p bselva
* PR 36396:[7400]Enabled the TSM error recovery support for 7400
* 
* Hydra_Software_Devel/85   11/27/07 1:04p bselva
* PR36396:[7401,7118,7403] Added the support to enable/Disable the TSM
* error recovery mechanism
* 
* Hydra_Software_Devel/84   10/17/07 7:03p bselva
* PR 35525:[7400,7401,7403,7118] The DSP context allocation is moved to
* start time to support dual decode for 7400.
* 
* Hydra_Software_Devel/83   8/1/07 3:04p bselva
* PR32542:[7401] Added the PI change to support audio master mode for
* 7401 family of chips.
* 
* Hydra_Software_Devel/82   7/26/07 3:26p rjain
* PR 30026: [7401] Adding flag bMonoToAll to let app send downmixed mono
* audio to all output ports. We pass this to FW as an array in the user
* config parameters.
* 
* Hydra_Software_Devel/81   7/5/07 3:54p gautamk
* PR32256: [7401] Adding new algo WMA PRO for 7401
* 
* Hydra_Software_Devel/RAP_7401_WMA_PRO/1   6/18/07 12:24p gautamk
* PR32256: [7401] Adding new algo WMA PRO for 7401
* 
* Hydra_Software_Devel/80   6/5/07 2:39p rjain
* PR 28524: fix warnings
* 
* Hydra_Software_Devel/79   5/4/07 7:51p rjain
* PR 30346: [7401 family] new mit changes..
* 
* Hydra_Software_Devel/78   5/4/07 6:47p rjain
* PR 30346: [7401 family] fixing compilation
* 
* Hydra_Software_Devel/77   5/4/07 4:37p rjain
* PR 30436: [7401 family] First round of check ins for new mit. All
* changes for new MIT are within the macro BRAP_DSP_P_7401_NEWMIT. This
* macro is defined as 0 at present. It will be enabled once new MIT FW
* is also checked in.
* 
* Hydra_Software_Devel/76   4/12/07 2:09p bselva
* PR 27768: [7400 B0]Enabling the new TSm scheme for 7400 B0 version
* 
* Hydra_Software_Devel/75   3/8/07 10:46a rjain
* PR 28524:[7401, 7118, 7403] code clean up: replacing (BCHP_CHIP ==
* 7401) || (BCHP_CHIP == 7118) || (BCHP_CHIP == 7403) with
* (BRAP_7401_FAMILY == 1) where appropriate
* 
* Hydra_Software_Devel/74   2/8/07 4:30p rjain
* PR 24247 : [7401] adding support for SRC as a post processing moidule
* for all algos
* 
* Hydra_Software_Devel/73   2/7/07 2:46p gautamk
* PR23960 : RAP ZSP FW must reside in a single block of read-only memory
* to support NDS SVP
* 
* Hydra_Software_Devel/72   12/19/06 1:24p rjain
* PR 25670: add 7403 support to RAP
* 
* Hydra_Software_Devel/71   12/12/06 4:55p nitinb
* PR 25919: Created a private struct BRAP_DSPCHN_P_ConfigParams to hold
* all the removed config parameters from BRAP_DSPCHN_Settings
* 
* Hydra_Software_Devel/70   10/31/06 2:44p rjain
* PR 22635: turning on PI support for new TSM  scheme
* 
* Hydra_Software_Devel/69   10/30/06 1:26p rjain
* PR 22635: adding support for Smooth tracking threshold and
* GrossAdjustment threshold in PI. Code is under
* #if (BRAP_DSPCHN_P_HAS_NEW_TSM_SCHEME==1)
* 
* Hydra_Software_Devel/68   9/27/06 5:47p rjain
* PR 22087: PI changes for independent output delay
* 
* Hydra_Software_Devel/67   8/28/06 11:08a sushmit
* PR 18604: Renaming baud_dsphostencoderinterface.h to
* brap_dsphostencoderinterface.h
* 
* Hydra_Software_Devel/66   8/13/06 6:12p nitinb
* PR 22699: Added TSM upper threshold field to dspchn_p_settings.
* 
* Hydra_Software_Devel/65   6/25/06 6:34p kagrawal
* PR 20590: Merged Stop/Start at PTS code from RAP_7411D0_PR20590 branch
* 
* Hydra_Software_Devel/64   6/25/06 3:40p kagrawal
* PR 22297: Added support for stereo downmix path concurrent with
* multichannel path
* 
* Hydra_Software_Devel/63   6/6/06 11:18a sushmit
* PR18604: Updating Encoder PI for 7401.
* 
* Hydra_Software_Devel/62   5/17/06 11:31a sushmit
* PR18604: Updating Encoder PI for 7401.
* 
* Hydra_Software_Devel/61   4/24/06 10:52a sushmit
* PR18604: Updating Encoder MIT.
* 
* Hydra_Software_Devel/60   4/11/06 6:02p sushmit
* PR18604: Modifying
* #if (( BCHP_CHIP == 7401 )||( BCHP_CHIP == 7400 ))
* With code like
* #ifndef BCHP_7411_VER
* 
* Hydra_Software_Devel/59   4/11/06 8:51a nitinb
* PR 20670: Implement code review comments for dsp module of RAP PI
* 
* Hydra_Software_Devel/58   4/7/06 2:51p sushmit
* PR18604: Updating Encoder MIT & Watchdog.
* 
* Hydra_Software_Devel/57   4/3/06 6:05p rjain
* PR 20318: Implementing review comments for AM
* 
* Hydra_Software_Devel/56   3/30/06 4:44p bselva
* PR 19878: Checking in the changes required for 7400 phase 0.5 features
* 
* Hydra_Software_Devel/55   3/28/06 12:05p rnayak
* PR20439: Restore Bangalore Raptor files after magnum vob corruption on
* 17th March 2006
* 
* Hydra_Software_Devel/55   3/21/06 6:26p sushmit
* PR18604: Updating review comments.
* 
* Hydra_Software_Devel/54   3/17/06 2:34p nitinb
* PR 20117: Initialize DDBM table on every "start decode"
* 
* Hydra_Software_Devel/53   3/16/06 5:19p nitinb
* PR 20117: Implement dolby digital bass management algorithm
* 
* Hydra_Software_Devel/52   3/10/06 5:58p kagrawal
* PR 19615: Encorporated RDB change for multi channel I2S support
* 
* Hydra_Software_Devel/51   2/28/06 10:14p nitinb
* PR 19502: Added prototype for BRAP_DSP_P_InitInterframeBuf function.
* Updated prototype of function BRAP_DSP_AllocateInterframeBuf.
* 
* Hydra_Software_Devel/50   2/28/06 11:13a sushmit
* PR18604: Fix for 7411 compilation issues.
* 
* Hydra_Software_Devel/49   2/27/06 4:16p sushmit
* PR18604: Fixing 7411 compilation issues.
* 
* Hydra_Software_Devel/48   2/17/06 9:52a sushmit
* PR18604: Merging Encoder to latest.
* 
* Hydra_Software_Devel/47   1/30/06 2:27p nitinb
* PR 17882: Increased frame sync executable buffer size (as required for
* DDP)
* 
* Hydra_Software_Devel/46   1/24/06 2:46p bselva
* PR 19168: Checking in the changes for 7400 bringup
* 
* Hydra_Software_Devel/RAP_AUD_ENC/5   2/6/06 4:14p sushmit
* PR18604: Merging latest Decoder PI to Encoder PI branch
* 
* Hydra_Software_Devel/RAP_AUD_ENC/3   12/19/05 6:30p sushmit
* PR18604: Updating for combined decoder & encoder scheduler.
* 
* Hydra_Software_Devel/RAP_AUD_ENC/2   12/14/05 5:30p sushmit
* [PR18604]:Susmit
* Merging Encoder to latest.
* SUSMIT
* 14 DEC 2005
* 
* Hydra_Software_Devel/RAP_AUD_ENC/1   12/14/05 5:10p sushmit
* [PR18604]:Susmit
* Merging Encoder to latest.
* SUSMIT
* 14 DEC 2005
* 
* Hydra_Software_Devel/43   11/30/05 12:07p susmit
* Merging Decoder & Encoder
* 
* Hydra_Software_Devel/42   12/5/05 10:35a nitinb
* PR 18061: Making TSM log support compile time selectable
* 
* Hydra_Software_Devel/41   12/2/05 3:28p nitinb
* PR 18061: Raptor memory allocation must be less than 0.5M for MPEG/AC-3
* only
* 
* Hydra_Software_Devel/40   11/16/05 9:16p rjain
* PR 18129 : Making the changes reqd for PI-Fw interface to add an output
* port on the fly for a DEC channel.
* 
* Hydra_Software_Devel/39   11/16/05 9:11p kagrawal
* PR 17590: Endian independent changes for multi codec
* 
* Hydra_Software_Devel/38   11/2/05 3:24p bselva
* PR 17590: Added watchdog support for 7401
* 
* Hydra_Software_Devel/37   10/25/05 5:46p nitinb
* PR 17415: Correcting buffer sizes for downloading decode and passthru
* firmware images
* 
* Hydra_Software_Devel/36   10/24/05 5:14p nitinb
* PR 17415: Added decoder configuration parameters for DDP, AAC, AAC-HE
* and MPEG
* 
* Hydra_Software_Devel/35   10/23/05 8:15p nitinb
* PR 17415: Added decoder configuration parameters for DDP, AAC, AAC-HE
* and MPEG
* 
* Hydra_Software_Devel/34   10/20/05 7:30p nitinb
* PR 17651: Changing checks for BCHP_CHIP ==/!= 7401 to check for
* BCHP_7411_VER instead
* 
* Hydra_Software_Devel/33   10/18/05 6:21p nitinb
* PR 17399: PVR: wrap-around with 'slow' trickmode result in audio noise
* at wrap-around point
* 
* Hydra_Software_Devel/32   10/2/05 10:00p nitinb
* PR 17415: Add support for advance codec for Raptor PI
* 
* Hydra_Software_Devel/31   9/29/05 8:19p shyam
* PR 17367: Handling multiple codecs in single codec memory allocation
* 
* Hydra_Software_Devel/30   9/22/05 10:00p nitinb
* PR 16889: Implemented private function to enable watchdog in DSP
* 
* Hydra_Software_Devel/29   9/22/05 1:09p nitinb
* PR 16899: Making changes in the code for watchdog feature
* 
* Hydra_Software_Devel/28   9/20/05 1:11p nitinb
* PR 16899: Making changes in the code for watchdog feature
* 
* Hydra_Software_Devel/27   9/15/05 8:29p nitinb
* PR 16899: Added function to get RAP handle
* 
* Hydra_Software_Devel/26   9/14/05 7:39p nitinb
* PR 17042: Raptor does not support sample rate change in the middle of a
* stream
* 
* Hydra_Software_Devel/25   9/13/05 7:41p bmishra
* PR 16148: Merged 7401 related changes
* 
* Hydra_Software_Devel/BLR_RAP_7401_Bringup/3   8/19/05 12:02p bmishra
* PR 16148: Mergeded the latest changes in the main branch.
* 
* Hydra_Software_Devel/24   8/3/05 9:16p bselva
* PR 15766: Support for BRAP_GetRevision API added
* 
* Hydra_Software_Devel/23   7/28/05 11:21p nitinb
* PR 16092: Merging new MIT code into main branch
* 
* Hydra_Software_Devel/BLR_Raptor7411C0_NewMit_Multicodec/BLR_Raptor7411C0_NewMit_Singlecodec/7   7/22/05 4:43p nitinb
* PR 16354: Made function BRAP_DSPCHN_P_GetCurrentPTS_isr public
* 
* Hydra_Software_Devel/BLR_Raptor7411C0_NewMit_Multicodec/BLR_Raptor7411C0_NewMit_Singlecodec/6   7/20/05 10:32p nitinb
* PR 16354: Added field bPlayback to structure BRAP_DSPCHN_P_AudioParams
* 
* Hydra_Software_Devel/BLR_Raptor7411C0_NewMit_Multicodec/BLR_Raptor7411C0_NewMit_Singlecodec/5   7/19/05 6:58p nitinb
* PR 16354: Develope PVR features for RAP PI
*
* Hydra_Software_Devel/20   7/13/05 7:43p nitinb
* PR 16093: Registers must be accessed using absolute addresses
* 
* Hydra_Software_Devel/BLR_RAP_7401_Bringup/1   7/12/05 8:14p bmishra
* PR 16148: Modified the file for 7401 compatibility
* 
* Hydra_Software_Devel/BLR_Raptor7411C0_NewMit_Multicodec/BLR_Raptor7411C0_NewMit_Singlecodec/4   7/11/05 1:05p bselva
* The decode scratch buffer size is reduced to 512 bytes ( As MPEG and
* AC3 Decoder does not use this buffer )
* 
* Hydra_Software_Devel/BLR_Raptor7411C0_NewMit_Multicodec/BLR_Raptor7411C0_NewMit_Singlecodec/3   7/7/05 5:26p nitinb
* PR 15765: Added DSP channel specific offset for ESR_SO0X registers in
* DSPCHN handle
* Storing decoder lock unlock status in DSPCHN handle
* 
* Hydra_Software_Devel/BLR_Raptor7411C0_NewMit_Multicodec/BLR_Raptor7411C0_NewMit_Singlecodec/2   6/24/05 5:46p nitinb
* PR 15925: Removing function BRAP_DSPCHN_P_SPDIFConfig
* 
* Hydra_Software_Devel/BLR_Raptor7411C0_NewMit_Multicodec/BLR_Raptor7411C0_NewMit_Singlecodec/1   6/23/05 6:56p nitinb
* Merging main branch changes into newmit_singlecodec branch.
* 
* Hydra_Software_Devel/BLR_Raptor7411C0_NewMit_Multicodec/2   6/14/05 9:04p nitinb
* Correcting the buffer sizes. Allocating worst case buffer sizes as
* required by various algorithms.
* 
* Hydra_Software_Devel/BLR_Raptor7411C0_NewMit_Multicodec/1   6/13/05 2:58p nitinb
* PR 15272: Made changes in code required for new MIT format
* 
* Hydra_Software_Devel/14   6/10/05 8:18p nitinb
* PR 15837: Feature development for the Minor Release of June 2005
* (Implemented TSM log)
* 
* Hydra_Software_Devel/13   5/31/05 10:18a nitinb
* PR 15272: Making interrupt handler code interrupt safe
* 
* Hydra_Software_Devel/12   5/25/05 7:44p nitinb
* PR 15272: Changing name of enumtype BRAP_PtsType to BRAP_DSPCHN_PtsType
* and name of struct BRAP_PtsInfo to BRAP_DSPCHN_PtsInfo
* 
* Hydra_Software_Devel/11   5/25/05 3:10p nitinb
* PR 15272: Removing inclusion of bchp_aud_dsp* header files
* 
* Hydra_Software_Devel/10   5/25/05 11:46a nitinb
* PR 15272: Implementing FirstPTSReady, PTSError and StreamInfo
* interrupts
* 
* Hydra_Software_Devel/9   5/13/05 12:33p nitinb
* PR 15160: Need ability to set various fields in the SPDIF channel
* status
* 
* Hydra_Software_Devel/8   5/4/05 5:25p nitinb
* PR 14057: Added DSP channel state information in channel handle
* 
* Hydra_Software_Devel/7   4/21/05 4:07p nitinb
* PR 14507: Made changes in AudioParams structure as required by simul
* mode
* 
* Hydra_Software_Devel/6   4/14/05 11:47a nitinb
* PR 14507: Merging changes from multicodec branch to main branch
* 
* Hydra_Software_Devel/BLR_Raptor7411C0_MultiCodec/2   3/28/05 11:12a nitinb
* PR 14507: Added channel memory information field in channel handle
* 
* Hydra_Software_Devel/BLR_Raptor7411C0_MultiCodec/1   3/18/05 8:31p nitinb
* PR 13082: Added pass thru related parameters to dsp handle
* 
* Hydra_Software_Devel/5   3/10/05 2:05p nitinb
* PR 13082: Added fields BRAP_OutputMode and BAVC_Timebase to structure
* BRAP_DSPCHN_P_AudioParams
* 
* Hydra_Software_Devel/3   2/16/05 8:29p bmishra
* PR 14143: Implemented Phase 1 code review comments for the interfaces
* only
* 
* Hydra_Software_Devel/2   1/24/05 3:59p nitinb
* PR 13082: Added AAC donwmix and TSM related functions
* 
* Hydra_Software_Devel/3   11/2/04 6:55p nitinb
* PR 13082: Adding BRAP_DSPCHN_Start code
* 
* Hydra_Software_Devel/2   10/29/04 1:14p nitinb
* PR 13082: Adding DSP initialization code
* 
* Hydra_Software_Devel/1   10/26/04 8:22a nitinb
* PR13082: Initial Version
* 
***************************************************************************/

#ifndef BRAP_DSP_PRIV_H__
#define BRAP_DSP_PRIV_H__




#include "brap_types.h"
#include "brap_dsphostinterface.h"
#include "brap_dsphostcommon.h"


#ifndef BCHP_7411_VER /* For chips other than 7411 */
/* Adding Encoder */
#include "brap_dsphostencoderinterface.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if (BRAP_7401_FAMILY == 1) || (BCHP_CHIP == 7400 && BCHP_VER !=BCHP_VER_A0)
#define BRAP_DSPCHN_P_HAS_NEW_TSM_SCHEME 1
#endif

#if (BRAP_7401_FAMILY == 1) || (BCHP_CHIP == 7400)
#define BRAP_DSPCHN_P_HAS_TSM_ERROR_RECOVERY 1
#else
#define BRAP_DSPCHN_P_HAS_TSM_ERROR_RECOVERY 0
#endif


#define	BRAP_DSP_P_MAX_AUDIO_CHANNELS					3
#define	BRAP_DSP_P_APM_SCRATCH_BUF_SIZE					(1024*12)	
#define BRAP_DSP_P_INTERFRAME_BUF_SIZE					52180 /* Worst case size for AAC-HE */	
#define BRAP_DSP_P_TOTAL_INTERFRAME_BUF_SIZE			(BRAP_DSP_P_INTERFRAME_BUF_SIZE * BRAP_DSP_P_MAX_AUDIO_CHANNELS)
#define BRAP_DSP_P_INTERSTAGE_BUF_SIZE					(2048 * 4 * 6) /* Size for AAC-HE */	
#define BRAP_DSP_P_INTERSTAGE_IF_BUF_SIZE				72524 /* Worst case size for DDP */	
#define BRAP_DSP_P_COMMON_PP_BUF_SIZE 					4096 /* 4KB Worst case size of each common post processing buffer */ 
#define BRAP_DSP_P_CTXT_FS_EXE_SIZE						22000	/* Worst case frame sync buf size (for DDP) for each context */
#define BRAP_DSP_P_MAX_DECODER_MEM_SIZE					(1024 * 116) /* Worst case decoder mem required estimate */
#define BRAP_DSP_P_MAX_PP_EXE_SIZE						26824 /* Worst case downmix (PP) mem required estimate */
#define BRAP_DSP_P_TOTAL_DECODE_FW_BUF_SIZE				(BRAP_DSP_P_MAX_DECODER_MEM_SIZE + BRAP_DSP_P_MAX_PP_EXE_SIZE)

/* Note: These macros should be modified when more decode modes or audio algo types are supported.*/
#define BRAP_DSP_P_MAX_NUM_DECODE_MODE_SUPPORTED 		(BRAP_DSPCHN_DecodeMode_eSimulMode+1)
#define BRAP_DSP_P_MAX_NUM_AUDIO_TYPE_SUPPORTED 		(BRAP_DSPCHN_AudioType_eAc3Plus+1)

#if ( BCHP_CHIP == 7400 )
#define BRAP_DSP_P_MAX_NUM_FW_CTXT_IN_FW_EXE_BUF 4  /* max number of ctxt buffers */
#elif (BRAP_AD_SUPPORTED == 1)
#define BRAP_DSP_P_MAX_NUM_FW_CTXT_IN_FW_EXE_BUF 3  /* max number of ctxt buffers = max number of algorithm running simultaneously */
#else /* For 7403 , 7118 */
#define BRAP_DSP_P_MAX_NUM_FW_CTXT_IN_FW_EXE_BUF 2  /* max number of ctxt buffers = max number of algorithm running simultaneously */
#endif

#if ( BCHP_CHIP == 7400 )
#define BRAP_DSP_P_MAX_NUM_FS_CTXT_IN_FS_EXE_BUF (2+1)  /* max number of framesync ctxt buffers = max number of framesync exec needed in DRAM 
                                                           Extra one(+1) is added for downloading the frame sync file for DDP passthrough ctxt*/

#define BRAP_DSP_P_DDP_FS_CTXT_EXE_BUF 3        /* This context is used for downloading of frame sync file for DDP passthrough ctxt*/

#else
#define BRAP_DSP_P_MAX_NUM_FS_CTXT_IN_FS_EXE_BUF 2  /* max number of framesync ctxt buffers = max number of framesync exec needed in DRAM */
#endif

/* Max number of execs in the DRAM context buffers (FS context / FW context) */
#define BRAP_DSP_P_MAX_FS_ALG_IN_A_CTXT	 		1
#define BRAP_DSP_P_MAX_DEC_EXE_N_TBL_IN_A_CTXT 	5 /* max number of decode algo execs */
#define BRAP_DSP_P_MAX_PP_EXE_N_TBL_IN_A_CTXT 	2 /* max number of post processing algo/table execs */
#define BRAP_DSP_P_MAX_PT_EXE_N_TBL_IN_A_CTXT  	1 /* max number of pass thru algo/table execs */

#define BRAP_DSP_P_DECODE_CTXT_BUFFER			0
#define BRAP_DSP_P_PASSTHRU_CTXT_BUFFER			( BRAP_RM_P_MAX_DECODE_SUPPORTED )

#define SIZE_OF_BAF_HOST_BUF_INFO_ALLIGNED  BRAP_P_ALIGN(SIZE_OF_BAF_HOST_BUF_INFO,2)
#define SIZE_OF_BAF_HOST_INFO_ALLIGNED      BRAP_P_ALIGN(SIZE_OF_BAF_HOST_INFO, 2) 
#define SIZE_OF_BAF_DOWNLOAD_ALLIGNED	BRAP_P_ALIGN(SIZE_OF_BAF_DOWNLOAD, 2)


#define BRAP_DSPCHN_P_MAX_NUM_DELAY_PORTS 3 /* Maximum no. of output ports 
that can be delayed at start time */

/* Debug flags */
#define BRAP_DSP_P_DBG_ENABLED 		1
#define BRAP_DSP_P_DBG_MIT_TABLE 	0

typedef struct BRAP_DSP_P_Device			*BRAP_DSP_Handle;

typedef struct BRAP_DSPCHN_P_Channel			*BRAP_DSPCHN_Handle;	/* Opaque DSP context handle */

/***************************************************************************
Summary:
	This structure stores information about downloaded firmware executables
	and various firmware tables.
***************************************************************************/
typedef struct sMITArrays
{
	uint32_t fsAlgoSizes[BAF_HOST_MAX_DL_MODULE];
	uint32_t fsAlgoArrays[BAF_HOST_MAX_DL_MODULE];
	uint32_t decAlgoSizes[BAF_HOST_MAX_DL_MODULE];
	uint32_t decAlgoArrays[BAF_HOST_MAX_DL_MODULE];
	uint32_t decTblSizes[BAF_HOST_MAX_DL_MODULE];
	uint32_t decTblArrays[BAF_HOST_MAX_DL_MODULE];
	uint32_t ppAlgoArrays[BAF_HOST_MAX_DL_MODULE];
	uint32_t ppAlgoSizes[BAF_HOST_MAX_DL_MODULE];
	uint32_t ppTblArrays[BAF_HOST_MAX_DL_MODULE];
	uint32_t ppTblSizes[BAF_HOST_MAX_DL_MODULE];
	uint32_t ptAlgoArrays[BAF_HOST_MAX_DL_MODULE];
	uint32_t ptAlgoSizes[BAF_HOST_MAX_DL_MODULE];
	uint32_t ptTblArrays[BAF_HOST_MAX_DL_MODULE];
	uint32_t ptTblSizes[BAF_HOST_MAX_DL_MODULE];
} sMITArrays;

#ifndef BCHP_7411_VER /* For chips other than 7411 */
/* Merging Decoder & Encoder */
typedef struct sEncMITArrays
{
	uint32_t encAlgoSizes[BAF_HOST_MAX_ENCODED_DL_MODULE];
	uint32_t encAlgoArrays[BAF_HOST_MAX_ENCODED_DL_MODULE];
	uint32_t encTblSizes[BAF_HOST_MAX_ENCODED_DL_MODULE];
	uint32_t encTblArrays[BAF_HOST_MAX_ENCODED_DL_MODULE];
	uint32_t encPrePAlgoSizes[BAF_HOST_MAX_PRE_PROCESS_DL_MODULE];
	uint32_t encPrePAlgoArrays[BAF_HOST_MAX_PRE_PROCESS_DL_MODULE];
	uint32_t encPrePTblSizes[BAF_HOST_MAX_PRE_PROCESS_DL_MODULE];
	uint32_t encPrePTblArrays[BAF_HOST_MAX_PRE_PROCESS_DL_MODULE];
	uint32_t encPostPAlgoSizes[BAF_HOST_MAX_POST_PROCESS_DL_MODULE];
	uint32_t encPostPAlgoArrays[BAF_HOST_MAX_POST_PROCESS_DL_MODULE];
	uint32_t encPostPTblSizes[BAF_HOST_MAX_POST_PROCESS_DL_MODULE];
	uint32_t encPostPTblArrays[BAF_HOST_MAX_POST_PROCESS_DL_MODULE];
	uint32_t encIFrameSizes[BAF_HOST_MAX_ENCODE_BUF_COUNT];
	uint32_t encIFrameArrays[BAF_HOST_MAX_ENCODE_BUF_COUNT];
	uint32_t encIStageSizes[BAF_HOST_MAX_ENCODE_BUF_COUNT];
	uint32_t encIStageArrays[BAF_HOST_MAX_ENCODE_BUF_COUNT];
} sEncMITArrays;
#endif

/***************************************************************************
Summary:
	This structure contains DSP settings.

***************************************************************************/
typedef struct BRAP_DSP_P_Settings
{
	BRAP_DSP_Settings sDspExtSettings;
} BRAP_DSP_P_Settings;

/***************************************************************************
Summary:
	This structure contains decoder configuration parameters for various algorithms.

***************************************************************************/
typedef struct BRAP_DSPCHN_P_ConfigParams
{
	BRAP_DSPCHN_Ac3ConfigParams	sAc3ConfigParam;	
	BRAP_DSPCHN_AacConfigParams sAacConfigParam;	
	BRAP_DSPCHN_AacSbrConfigParams sAacSbrConfigParam;	
	BRAP_DSPCHN_Ac3PlusConfigParams sAc3PlusConfigParam;	
	BRAP_DSPCHN_DtsConfigParams	sDtsConfigParam; 
#if (BRAP_DTS_SUPPORTED == 1)		    
    BRAP_DSPCHN_DtsCoreConfigParams	sDtsCoreConfigParam;     
#endif
	BRAP_DSPCHN_DtshdConfigParams sDtshdConfigParam;	
	BRAP_DSPCHN_BdlpcmConfigParams	sBdlpcmConfigParam;	
	BRAP_DSPCHN_LpcmHdDvdConfigParams	sLpcmHdDvdConfigParam;    	
	BRAP_DSPCHN_LpcmDvdConfigParams	sLpcmDvdConfigParam;    	
	BRAP_DSPCHN_Ac3PlusConfigParams		sAc3LosslessConfigParam;	
	BRAP_DSPCHN_DdbmConfigParams	sDdbmConfigParams;	
	BRAP_DSPCHN_MlpConfigParams		sMlpConfigParams;	
	BRAP_DSPCHN_WmaProConfigParams		sWmaProConfigParams;	
	BRAP_DSPCHN_WmaStdConfigParams		sWmaStdConfigParams;	
    BRAP_DSPCHN_PcmWavConfigParams  sPcmWavConfigParams;
	BRAP_DSPCHN_DraConfigParams		sDraConfigParams;    
} BRAP_DSPCHN_P_ConfigParams;

/***************************************************************************
Summary:
	This structure contains DSPCHN channel settings.

***************************************************************************/
typedef struct BRAP_DSPCHN_P_Settings
{
	BRAP_OutputMode			eOutputMode[BRAP_DEC_DownmixPath_eMax];
    bool  bMonoToAll[BRAP_DEC_DownmixPath_eMax];    
	BRAP_DSPCHN_Settings	sDspchnExtSettings[BRAP_DEC_DownmixPath_eMax]; 
	BRAP_DSPCHN_P_ConfigParams	sDspchnCfgParams[BRAP_DEC_DownmixPath_eMax]; 
	bool					bTsmEnable;
	bool					bAtsmEnable;            
	bool					bTsmLogEnable;
	uint32_t				ui32TsmDiscardThresh;
	uint32_t				ui32TsmUpperThresh;
#if (BRAP_DSPCHN_P_HAS_NEW_TSM_SCHEME==1)
	uint32_t				ui32TsmSTThresh;        /* Smooth Tracking 
	threshold for TSM */
	uint32_t				ui32TsmGAThresh;        /* Gross Adjustment 
	threshold for TSM */
#if (BRAP_7401_FAMILY == 1)
	uint32_t				ui32TsmSLThresh;	/* Sync Limit threshold for TSM */
#endif
#endif

#if BCHP_7411_VER > BCHP_VER_C0
	int32_t				i32TsmTransitionThreshold;
#endif
#if (BRAP_DSPCHN_P_HAS_TSM_ERROR_RECOVERY==1)
	bool					bEnableTSMErrRecovery;
#endif
#if (BRAP_AD_SUPPORTED == 1) || (BRAP_DOLBYVOLUME_SUPPORTED == 1) || (BRAP_SRS_TRUVOL_SUPPORTED == 1)
	BRAP_PP_Settings		sPPSettings;
#endif
}BRAP_DSPCHN_P_Settings;

/***************************************************************************
Summary:
***************************************************************************/
typedef struct BRAP_DSPCHN_P_DecConfigParams
{
	BRAP_DSPCHN_AudioType    eType;   /* audio type */
	union {
		BRAP_DSPCHN_Ac3ConfigParams	sAc3ConfigParams;
		BRAP_DSPCHN_AacConfigParams	sAacConfigParams;
		BRAP_DSPCHN_AacSbrConfigParams	sAacSbrConfigParams;
		BRAP_DSPCHN_Ac3PlusConfigParams	sAc3PlusConfigParams;
	}u;
} BRAP_DSPCHN_P_DecConfigParams;

/***************************************************************************
Summary:
	This structure contains a list of IDs corresponding to a list of execs
	that need to be downloaded in the DRAM to decode a audio stream based
	on audio type, decode mode and the transID.
***************************************************************************/
typedef struct BRAP_DSP_P_ExecID{
	uint32_t 				fsID[BRAP_DSP_P_MAX_FS_ALG_IN_A_CTXT];
	uint32_t 				decID[BRAP_DSP_P_MAX_DEC_EXE_N_TBL_IN_A_CTXT];
	uint32_t 				ppID[BRAP_DSP_P_MAX_PP_EXE_N_TBL_IN_A_CTXT];
	uint32_t 				ptID[BRAP_DSP_P_MAX_PT_EXE_N_TBL_IN_A_CTXT];
	/* TODO: check the type and number of this transID -- AAC HE */
	uint32_t				transID;
}BRAP_DSP_P_ExecID;

/***************************************************************************
Summary:
	This structure contains pointers to device memory for various DSP
	FW algorithms and tables.
***************************************************************************/

#if (BRAP_DSP_P_7401_NEWMIT==1)
typedef struct BRAP_DSP_P_MemInfo
{
	uint32_t				apmScratchBufPtr;
	uint32_t				encScratchBufsBasePtr;	
	uint32_t				encScratchBufsSize;
	uint32_t				interFrameBufBasePtr[BRAP_RM_P_MAX_DECODE_SUPPORTED];
	uint32_t				interStageBufBasePtr[BAF_MAX_BRANCHES * (1 + 2 * BAF_MAX_BRANCHES)];
	uint32_t				interStageBufSize;
	uint32_t				interStageInterfaceBufBasePtr[BAF_MAX_BRANCHES * (1 + 2 * BAF_MAX_BRANCHES)];
	uint32_t				interStageInterfaceBufSize;
	uint32_t				decConfigBufPtr[BAF_MAX_CONTEXTS * BAF_MAX_BRANCHES * ( BAF_MAX_POSTPROCESS_STAGES + 1) + BAF_MAX_CONTEXTS];
	uint32_t				interFrameBufSize;
	uint32_t				decConfigBufSize;
	uint32_t				scratchSpaceBufPtr;
	uint32_t				scratchSpaceBufSize;
	uint32_t				commonPpBufPtr;
	uint32_t				commonPpBufSize;
	uint32_t				ddbmTblPtr;
	uint32_t				commonPpBufSizeUsed;
	uint32_t				iboot_ptr;			
	uint32_t				scheduler_code_ptr;
	uint32_t				encscheduler_code_ptr;
	uint32_t				scheduler_data_ptr;
	uint32_t				encscheduler_data_ptr;
	uint32_t				mitDSPtr;
	uint32_t				encmitDSPtr;
	uint32_t				totalMITSize;
	uint32_t				enctotalMITSize;
	uint32_t 				fsBufBasePtr[BRAP_DSP_P_MAX_NUM_FS_CTXT_IN_FS_EXE_BUF];
	uint32_t 				fsBufSize[BRAP_DSP_P_MAX_NUM_FS_CTXT_IN_FS_EXE_BUF];
	uint32_t				ctxtFwBufBasePtr[BRAP_DSP_P_MAX_NUM_FW_CTXT_IN_FW_EXE_BUF];
	uint32_t				ctxtFwBufSize[BRAP_DSP_P_MAX_NUM_FW_CTXT_IN_FW_EXE_BUF];
	uint32_t				allocFwDwnldSize;	/*Total size allocated to downloaded 
												firmware executables in contiguous
												memory location.The value of this
												parameter will be worst case 
												firmware download size. Valid if
												Firmware authentication is 
												Enabled */
	uint32_t				fwDwnldOffset; /*Physical memory offset of 
											downloaded firmware executables.
											Valid if Firmware authentication 
											is Enabled	*/
	uint32_t				actualFwDwnldSize; /* Actual downloaded FW size is 
												diifferent from Allocated FW 
												download size. So this parameter
												is different from allocFwDwnldSize 
												and always be less 	than 
												allocFwDwnldSize. Valid if 
												Firmware authentication is Enabled	*/

	uint32_t				encAlgoBasePtr;
	uint32_t				totalEncAlgoSize;
	uint32_t				encTblBasePtr;
	uint32_t				totalEncTblSize;
	uint32_t				encPrePAlgoBasePtr;
	uint32_t				totalEncPrePAlgoSize;
	uint32_t				encPrePTblBasePtr;
	uint32_t				totalEncPrePTblSize;
	uint32_t				encPostPAlgoBasePtr;
	uint32_t				totalEncPostPAlgoSize;
	uint32_t				encPostPTblBasePtr;
	uint32_t				totalEncPostPTblSize;
	uint32_t				encIFrameBasePtr;
	uint32_t				totalEncIFrameSize;
	uint32_t				encIStageBasePtr;
	uint32_t				totalEncIStageSize;
    uint32_t                ui32PpFwBufPtr;
    uint32_t                ui32PpFwBufSize;
    uint32_t                ui32PpFwBufSizeUsed;
    uint32_t                ui32PpInterFrameBufBasePtr[BAF_MAX_CONTEXTS * BAF_MAX_BRANCHES * BAF_MAX_POSTPROCESS_STAGES];
    uint32_t                ui32PpInterFrameBufSize;
    uint32_t                ui32PpScratchBufPtr;
    uint32_t                ui32PpScratchBufSize;
	uint32_t				arrayOfConfigBufPtr[BAF_MAX_CONTEXTS];
	uint32_t				arrayOfConfigBufSize;
    uint32_t                ui32DwnldableSchedulerPtr;
    uint32_t                ui32DwnldableSchedulerSize;	
} BRAP_DSP_P_MemInfo;
#else /* if old MIT */
typedef struct BRAP_DSP_P_MemInfo
{
	uint32_t				apmScratchBufPtr;
#ifndef BCHP_7411_VER /* For chips other than 7411 */
/* Merging Decoder & Encoder */
	uint32_t				encScratchBufsBasePtr;	
	uint32_t				encScratchBufsSize;
#endif
	uint32_t				interFrameBufBasePtr[BRAP_RM_P_MAX_DECODE_SUPPORTED];
	uint32_t				decConfigBufPtr[BRAP_RM_P_MAX_DECODE_SUPPORTED];
	uint32_t				interFrameBufSize;
	uint32_t				decConfigBufSize;
	uint32_t				scratchSpaceBufPtr;
	uint32_t				scratchSpaceBufSize;
	uint32_t				commonPpBufPtr;
	uint32_t				commonPpBufSize;
	uint32_t				ddbmTblPtr;
	uint32_t				commonPpBufSizeUsed;
	uint32_t				iboot_ptr;			
	uint32_t				scheduler_code_ptr;
#ifndef BCHP_7411_VER /* For chips other than 7411 */
	uint32_t				encscheduler_code_ptr;
#endif
	uint32_t				scheduler_data_ptr;
#ifndef BCHP_7411_VER /* For chips other than 7411 */
	uint32_t				encscheduler_data_ptr;
#endif
	uint32_t				mitDSPtr;
#ifndef BCHP_7411_VER /* For chips other than 7411 */
	uint32_t				encmitDSPtr;
#endif
	uint32_t				totalMITSize;
#ifndef BCHP_7411_VER /* For chips other than 7411 */
	uint32_t				enctotalMITSize;
#endif
	uint32_t 				fsBufBasePtr[BRAP_DSP_P_MAX_NUM_FS_CTXT_IN_FS_EXE_BUF];
	uint32_t 				fsBufSize[BRAP_DSP_P_MAX_NUM_FS_CTXT_IN_FS_EXE_BUF];
	uint32_t				ctxtFwBufBasePtr[BRAP_DSP_P_MAX_NUM_FW_CTXT_IN_FW_EXE_BUF];
	uint32_t				ctxtFwBufSize[BRAP_DSP_P_MAX_NUM_FW_CTXT_IN_FW_EXE_BUF];
	uint32_t				allocFwDwnldSize;	/*Total size allocated to downloaded 
												firmware executables in contiguous
												memory location.The value of this
												parameter will be worst case 
												firmware download size. Valid if
												Firmware authentication is 
												Enabled */
	uint32_t				fwDwnldOffset; /*Physical memory offset of 
											downloaded firmware executables.
											Valid if Firmware authentication 
											is Enabled	*/
	uint32_t				actualFwDwnldSize; /* Actual downloaded FW size is 
												diifferent from Allocated FW 
												download size. So this parameter
												is different from allocFwDwnldSize 
												and always be less 	than 
												allocFwDwnldSize. Valid if 
												Firmware authentication is Enabled	*/
#ifndef BCHP_7411_VER /* For chips other than 7411 */
/* Merging Decoder & Encoder */
	uint32_t				encAlgoBasePtr;
	uint32_t				totalEncAlgoSize;
	uint32_t				encTblBasePtr;
	uint32_t				totalEncTblSize;
	uint32_t				encPrePAlgoBasePtr;
	uint32_t				totalEncPrePAlgoSize;
	uint32_t				encPrePTblBasePtr;
	uint32_t				totalEncPrePTblSize;
	uint32_t				encPostPAlgoBasePtr;
	uint32_t				totalEncPostPAlgoSize;
	uint32_t				encPostPTblBasePtr;
	uint32_t				totalEncPostPTblSize;
	uint32_t				encIFrameBasePtr;
	uint32_t				totalEncIFrameSize;
	uint32_t				encIStageBasePtr;
	uint32_t				totalEncIStageSize;
#endif
} BRAP_DSP_P_MemInfo;
#endif /* old MIT */

/***************************************************************************
Summary:
	This structure contains the DRAM context buffer usage information. 
***************************************************************************/
typedef struct BRAP_DSP_P_CtxtFwBufUsage
{
	BRAP_DSPCHN_DecodeMode	eDecodeMode;		
	BRAP_DSPCHN_AudioType	eAlgoType;    	
	/* TODO: Classification based on the transport type can be added here */
	/* Needed for AAC later */
	uint32_t				numUser;
}  BRAP_DSP_P_CtxtFwBufUsage;	

/***************************************************************************
Summary:
	This structure contains the DRAM Frame Sync Buffer Context usage 
	information. 
***************************************************************************/
typedef struct BRAP_DSP_P_CtxtFsBufUsage{
	BRAP_DSPCHN_AudioType	eAlgoType;    	
	uint32_t 				numUser;
}  BRAP_DSP_P_CtxtFsBufUsage;	

/***************************************************************************
Summary:
	Handle structure for DSP device.
***************************************************************************/
typedef struct BRAP_DSP_P_Device
{
	uint32_t				channelIndex;			/* DSP object index */
	BRAP_DSP_P_Settings		sSettings;				/* public AUD setting */

	BCHP_Handle				hChip;					/* handle to chip interface*/
	BREG_Handle				hRegister;				/* handle to register interface */
	BMEM_Handle				hHeap;					/* handle to memory interface */
	BINT_Handle				hInt;					/* handle to interrupt interface*/

	BRAP_Handle				hRap;					/* handle to parent audio device*/
 	BRAP_DSPCHN_Handle		phAudChannelHandle[BRAP_DSP_P_MAX_AUDIO_CHANNELS];
													/* handles to DSP contexts for
													   this DSP channel */

	BRAP_DSP_P_MemInfo		memInfo;				/* Information about device memory
													   required by DSP algorithms */
	sMITArrays				*psMitArrays;			/* Pointer to struct that has
													   info about firmware download */
	BRAP_DSP_P_CtxtFwBufUsage 	ctxtFwBufUsage[BRAP_DSP_P_MAX_NUM_FW_CTXT_IN_FW_EXE_BUF];
													/* Information about DRAM context 
													   firmware buffer usage */	
	BRAP_DSP_P_CtxtFsBufUsage 	ctxtFsBufUsage[BRAP_DSP_P_MAX_NUM_FS_CTXT_IN_FS_EXE_BUF];
													/* Information about DRAM frame sync
													   context buffer usage */							   
	bool						bInterframeBufFreeStatus[BRAP_RM_P_MAX_DECODE_SUPPORTED];
													/* Interframe buffers Free/Busy status */
#if (BRAP_DSP_P_7401_NEWMIT==1)
	bool						bPpInterframeBufFreeStatus[BAF_MAX_CONTEXTS * BRAP_P_MAX_PP_SUPPORTED * BRAP_P_MAX_BRANCH_SUPPORTED];
													/* Interframe buffers Free/Busy status */
													
	bool						bInterstageBufFreeStatus[BAF_MAX_CONTEXTS * (1 + 2 * BRAP_P_MAX_BRANCH_SUPPORTED)];
#endif
#ifndef BCHP_7411_VER /* For chips other than 7411 */
/* Merging Decoder & Encoder */
	sEncMITArrays			*psEncMitArrays;		/* Pointer to struct that has
													   info about firmware download */
#endif
	bool						bDecConfigBufFreeStatus[BAF_MAX_CONTEXTS * BAF_MAX_BRANCHES * ( BAF_MAX_POSTPROCESS_STAGES ) + BAF_MAX_CONTEXTS];
													/* Decoder config buffers Free/Busy status */                                                    
	bool						bConfigBufArrayFreeStatus[BAF_MAX_CONTEXTS];
} BRAP_DSP_P_Device;

/***************************************************************************
Summary:
***************************************************************************/
typedef struct BRAP_DSPCHN_P_OpPortConfig
{
    BRAP_OutputPort eOutputPortType;
    BRAP_OP_Pll     ePll;

#if (BRAP_INDEP_OP_DELAY_SUPPORTED == 1)
    /* The following information is required for the FW to implement indep
    delay on an output port */
    int             iDelay; /* The independent delay to be applied to this 
                            output port in units of milliseconds. 
                            Note: This delay value is used only if the 
                            channel is opened with bSelectDelay as TRUE  */
    unsigned int    uiDlydRBufIndex[BRAP_RM_P_MAX_RBUFS_PER_SRCCH]; 
                            /* If this port has to be delayed at start time, 
                            list the associated RBUFs here. Else leave this
                            field as BRAP_RM_P_INVALID_INDEX */
    unsigned int    uiSrcChIndex; /* If this port has to be delayed at start 
                            time, list the associated SrcCh here. Else leave 
                            this field as BRAP_RM_P_INVALID_INDEX */    
    BRAP_OutputChannelPair eChanPair; /* This output port can be a master 
                            or cloned port. FW needs to know which channel
                            pair data it is carrying */
#endif                            
} BRAP_DSPCHN_P_OpPortConfig; 


/***************************************************************************
Summary:
***************************************************************************/
typedef struct BRAP_DSPCHN_P_OpPortParams
{
	unsigned int			uiNumOpPorts;
	BRAP_DSPCHN_P_OpPortConfig	sOpPortConfig[BRAP_RM_P_MAX_OUTPUTS];
} BRAP_DSPCHN_P_OpPortParams;

/***************************************************************************
Summary:
***************************************************************************/
typedef struct BRAP_DSPCHN_P_DecModeParams
{
    unsigned int        rBufIndex[BRAP_RM_P_MAX_OP_CHANNELS]; 
#if (BRAP_INDEP_OP_DELAY_SUPPORTED == 1)
    bool          bLargeRbuf; /* TRUE: Output ports connected to this 
                                channel require larger 
                                rbufs in order to support independent delay.
                                FALSE: Normal RBUF sixe will be used 
                                => No output port connected to this 
                                channel can have an independent delay */    
#endif
    BRAP_DSPCHN_P_OpPortParams    sOpPortParams;
} BRAP_DSPCHN_P_DecModeParams;


/***************************************************************************
Summary:
	This structure contains audio parameters required to decode
	input compressed audio data.

***************************************************************************/
typedef struct BRAP_DSPCHN_P_AudioParams
{
	BRAP_DSPCHN_AudioParams		sExtAudioParams;
	BAVC_Timebase				eTimebase;
	unsigned int				uiTransChId;	
	bool						bPlayback;
	bool                        		bMultiChanOnI2S;
	BRAP_DSPCHN_P_DecModeParams	sDecOrPtParams;
	BRAP_DSPCHN_P_DecModeParams	sSimulPtParams;
	BRAP_OP_SpdifChanStatusParams sSpdifChStatusParams;
} BRAP_DSPCHN_P_AudioParams;

/***************************************************************************
Summary:
	This structure contains information about device memory used by
	DSP channel
***************************************************************************/
typedef struct BRAP_DSPCHN_P_MemInfo
{
	unsigned int uiAc3ConfBuffer1; 
	unsigned int uiAc3ConfBuffer2;
	unsigned int uiSpdifChStatusBuff;
	unsigned int uiTsmLogBufAdr[2];
	unsigned int uiTsmLogBufSize[2];
	unsigned int uiConfBufferPingArray; 
	unsigned int uiConfBufferPongArray;
	unsigned int uiPpPingConfBuffer[BAF_MAX_BRANCHES][BAF_MAX_POSTPROCESS_STAGES];
	unsigned int uiPpPongConfBuffer[BAF_MAX_BRANCHES][BAF_MAX_POSTPROCESS_STAGES];    
#if BCHP_7411_VER > BCHP_VER_C0    
	unsigned int uiStartStopPtsBuffer;
#endif
} BRAP_DSPCHN_P_MemInfo; 

/***************************************************************************
Summary:
	This structure returns information of sample rate change in stream
***************************************************************************/
typedef struct BRAP_DSPCHN_P_SampleRateChangeInfo
{
	bool 					bFsChanged; /* true = Change in sample rate,
											false = Sample rate not changed*/
	BRAP_DSPCHN_SampleRateChangeInfo	sSampleRateChangeInfo;
										/* Sample rate change information */
} BRAP_DSPCHN_P_SampleRateChangeInfo;

/***************************************************************************
Summary:
	This structure contains Stream Information private to RAP PI.
***************************************************************************/
typedef struct BRAP_DSPCHN_P_StreamInfo
{
	BRAP_DSPCHN_P_SampleRateChangeInfo sPrivateFsChangeInfo; /* RAP private
											sample rate change structure */
	BRAP_DSPCHN_StreamInfo	sStreamInfo; /* Stream information */
} BRAP_DSPCHN_P_StreamInfo;

/***************************************************************************
Summary:
	This enumeration defines current state of the DSP channel 
***************************************************************************/
typedef enum BRAP_DSPCHN_P_ChState
{
	BRAP_DSPCHN_P_ChState_eStart,
	BRAP_DSPCHN_P_ChState_eStop
}BRAP_DSPCHN_P_ChState;

#if BCHP_7411_VER > BCHP_VER_C0
typedef struct BRAP_DSPCHN_P_PtsBasedStartStop
{
	uint32_t	ui32NumPtsPairProgrammed;
	uint32_t	ui32StartPts0;
	uint32_t 	ui32StopPts0;
	uint32_t	ui32StartPts1;
	uint32_t 	ui32StopPts1;
} BRAP_DSPCHN_P_PtsBasedStartStop;
#endif


#if (BRAP_DSP_P_7401_NEWMIT==1) 
/***************************************************************************
Summary:
	Structure for Setting the post process stages in a post processing branch
    Note: This structure is exposed through RAP PI for 3563.	
****************************************************************************/
typedef struct BRAP_PpStageSettings
{
	BRAP_DSPCHN_PP_Algo	ePpAlgo[BRAP_P_MAX_PP_SUPPORTED];
					/* Stage wise post processing 
					   algorithms. If no post
					   processing required for a
					   stage, then corresponding
 					   entry should be set to
				 	   BRAP_DSPCHN_PP_Algo_eMax */
} BRAP_PpStageSettings;

typedef struct BRAP_DSPCHN_P_PpBranchInfo
{
	unsigned int			uiParentId;	
	unsigned int			uiForkPt;	
	BRAP_PpStageSettings	sPpStgSettings;
} BRAP_DSPCHN_P_PpBranchInfo;

typedef union BRAP_DSPCHN_P_PpStgConfigParam
{
	BRAP_DSPCHN_DolbyVolumeConfigParams  sDolbyVolumeConfigParams;
	BRAP_DSPCHN_SRSTruVolConfigParams  sSRSTruVolConfigParams;
	BRAP_DSPCHN_ADFadeCtrlConfigParams  sAdFadeConfigParams;
	BRAP_DSPCHN_ADPanCtrlConfigParams   sAdPanConfigParams;
} BRAP_DSPCHN_P_PpStgConfigParam;
#endif


/***************************************************************************
Summary:
	Handle structure for DSP context.
***************************************************************************/
typedef struct BRAP_DSPCHN_P_Channel
{
	uint32_t				channelIndex;			/* DSP context number */
	BRAP_DSPCHN_P_Settings	sSettings;				/* setting that is public */

	BCHP_Handle				hChip;					/* handle to chip object */
	BREG_Handle				hRegister;				/* handle to register object */
	BMEM_Handle				hHeap;					/* handle to memory object */
	BINT_Handle				hInt;					/* handle to interrupt object */

	BRAP_DSP_Handle			hDsp;					/* Pointer to DSP handle */	

	uint32_t				chCfgRegOffset;			/* channel configuration reg offset */
	uint32_t				chEsrSiRegOffset;	/* ESR SI register offset for the channel */
	uint32_t				chEsrSoRegOffset;	/* ESR SO register offset for the channel */
	BRAP_DSPCHN_P_AudioParams		sDspAudioParams;	/* Audio parameters for the channel */
	BRAP_DSPCHN_P_MemInfo			sChMemInfo;		/* Channel memory usage information */
	BRAP_DSPCHN_P_ChState			eChState;		/* Channel state */
	bool					bDecLocked;			/* Decoder lock/unlock state for this channel */
	BAVC_AudioSamplingRate	eSamplingRate; 			/* Sampling freq value */
	unsigned int			uiInterframeBufIndex;		/* Index correspondig to interframe buffer 
													    this channel is using */
	unsigned int			uiDecConfigBufIndex;		/* Index corresponding to decoder configuration
													   buffer this channel is using */
	unsigned int			uiPpConfigBufIndex[BAF_MAX_BRANCHES][BAF_MAX_POSTPROCESS_STAGES];	
													   /* Indices corresponding to post processing
														config buffer */
                                                        
	unsigned int			uiConfigBufArrayIndex;	/* Index corresponding to configuration
													   buffer array */
	unsigned int			uiCurrentArrayOfConfBufPtrCopy[BAF_MAX_BRANCHES][BAF_MAX_POSTPROCESS_STAGES+1]; 
	bool					bUseDefaultConfig[BAF_MAX_BRANCHES][BAF_MAX_POSTPROCESS_STAGES];
#if (BRAP_AD_SUPPORTED == 1) || (BRAP_DOLBYVOLUME_SUPPORTED == 1) || (BRAP_SRS_TRUVOL_SUPPORTED == 1)
	BRAP_DSPCHN_P_PpStgConfigParam	uPpStgConfigParam[BAF_MAX_BRANCHES][BAF_MAX_POSTPROCESS_STAGES];
#endif                                                       
                                                        
#if BCHP_7411_VER > BCHP_VER_C0
	bool					bFirstPtsPairProgrammed;
	BRAP_DSPCHN_P_PtsBasedStartStop	sPtsBasedStartStop;												   
#endif    
#if (BRAP_DSP_P_7401_NEWMIT==1) 
	BRAP_DSPCHN_P_PpBranchInfo 	sPpBranchInfo[BRAP_P_MAX_BRANCH_SUPPORTED];
	unsigned int			uiDecInterstageOutputBuf;
	unsigned int			uiDecInterstageIfOutputBuf;
	unsigned int			uiPpInterframeBuf[BRAP_P_MAX_BRANCH_SUPPORTED][BRAP_P_MAX_PP_SUPPORTED];
	unsigned int			uiInterstageInputBuf[BRAP_P_MAX_BRANCH_SUPPORTED][BRAP_P_MAX_PP_SUPPORTED];
	unsigned int			uiInterstageOutputBuf[BRAP_P_MAX_BRANCH_SUPPORTED][BRAP_P_MAX_PP_SUPPORTED];
	unsigned int			uiInterstageIfInputBuf[BRAP_P_MAX_BRANCH_SUPPORTED][BRAP_P_MAX_PP_SUPPORTED];
	unsigned int			uiInterstageIfOutputBuf[BRAP_P_MAX_BRANCH_SUPPORTED][BRAP_P_MAX_PP_SUPPORTED];
#endif
}BRAP_DSPCHN_P_Channel;


/***************************************************************************
Summary:
	This enumeration defines various firmwares type that needs to be 
	downloaded.
***************************************************************************/
typedef enum BRAP_DSP_P_FwType
{
	BRAP_DSP_P_FwType_eFs,
	BRAP_DSP_P_FwType_eDec,
	BRAP_DSP_P_FwType_ePp,
	BRAP_DSP_P_FwType_ePt
}BRAP_DSP_P_FwType;
#define BRAP_DSP_P_MAX_FWTYPE 	(BRAP_DSP_P_FwType_ePt+1)
	
/***************************************************************************
Summary:
	Gets DSP default channel setting

Description:
	The default DSP channel configuration settings are returned
	
Returns:
    BERR_SUCCESS                - if successful 

See Also:
	BRAP_DSP_P_Open
**************************************************************************/
BERR_Code 
BRAP_DSP_P_GetDefaultSettings( 
   BRAP_DSP_P_Settings	*psDefSettings	/* [out] The DEC channel default configuration setting */
   );

/***************************************************************************
Summary: Opens an BRAP_DSP channel 

Description:
	This function is responsible for opening a DSP channel. 
	When a BRAP_DSP channel is opened, it will create a module channel handle 
	and configure the module based on the channel settings. 
	Once a channel is opened, it must be closed before it can be opened 
	again.

Returns:
	BERR_SUCCESS - If successful

See Also:
	BRAP_DSP_P_Close,
	BRAP_DSP_P_GetDefaultSettings
**************************************************************************/
BERR_Code 
BRAP_DSP_P_Open(
	BRAP_DSP_Handle			*phDsp,			/* [out] DSP handle */
	BRAP_Handle				hRap,			/* [in] AUD handle */
	uint32_t				dspIndex,		/* [in] Channel index starting from 0 to max number of channels */ 
	const BRAP_DSP_P_Settings	*pDefSettings	/* [in] The DSP channel configuration setting */
	);

/***************************************************************************
Summary: Closes an BRAP_DSP channel

Description:
	This function is responsible for closing a DSP channel and releasing
	all the resources associated with this channel.
	

Returns:
	BERR_SUCCESS - If successful

See Also:
	BRAP_DSP_P_Open
**************************************************************************/
BERR_Code 
BRAP_DSP_P_Close( 
	BRAP_DSP_Handle handle /* [in] DSP handle */
	);

/***************************************************************************
Summary: Initialize hardware flags and bits.

Description:
	This function is responsible for booting the DSP. It first resets  and at
	the end unresets the DSP.
	

Returns:
	BERR_SUCCESS - If successful

See Also:
	BRAP_DSP_P_Open
**************************************************************************/
BERR_Code 
BRAP_DSP_P_InitHardware(
		BRAP_DSP_Handle hDsp /* [in] DSP handle */
		);

/***************************************************************************
Summary: Gets the watchdog recovery flag 

Description:
	This function returns current status of watchdog recovery flag. This
	is required to know whether a function is getting called in watchdog
	context or from application.

Returns:
	Watchdog flag

See Also:
**************************************************************************/
bool
BRAP_DSP_P_GetWatchdogRecoveryFlag(BRAP_DSP_Handle hDsp);

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
   );

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
   );

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
   );

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
	BRAP_DSP_Handle			hDsp,			/* [in] DSP handle */
	uint32_t				channelIndex,	/* [in] Channel index starting from 0 to max number of channels */ 
	const BRAP_DSPCHN_P_Settings	*pDefSettings	/* [in] The DSP channel configuration setting */
	);

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
	);

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
	BRAP_DSPCHN_Handle	pDspChn,					/* [in] DSPCHN handle */
	const BRAP_DSPCHN_P_AudioParams *psAudioParams	/* [in] Audio decode parameters */
	);

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
	);

BERR_Code
BRAP_DSPCHN_P_GetStreamInformation_isr(
						BRAP_DSPCHN_Handle      hDspChn,	/* [in] DSPCHN handle */
						BRAP_DSPCHN_P_StreamInfo *psStreamInfo /* [out] Stream Information */
						);
void
BRAP_DSPCHN_P_TsmLog_isr(
	BRAP_DSPCHN_Handle	hDspCh,			/* [in] DSP channel handle */
	BRAP_DSPCHN_TsmLogInfo	*psTsmLogInfo);	/* [out] Information about logged TSM data */

BERR_Code
BRAP_DSPCHN_P_EnablePause(
						BRAP_DSPCHN_Handle      hDspChn,	/* [in] DSPCHN handle */
						bool					bOnOff		/* [In] TRUE = Pause video
																FALSE = Resume video */
						);
BERR_Code
BRAP_DSPCHN_P_GetOneAudioFrameTime(
						BRAP_DSPCHN_Handle      hDspChn,			/* [in] DSPCHN handle */
						unsigned int			*puiOneFrameTime	/* [out] One Audio Frame Time */
						);

BERR_Code
BRAP_DSPCHN_P_FrameAdvance(
						BRAP_DSPCHN_Handle      hDspChn,			/* [in] DSPCHN handle */
						unsigned int			uiNumFrames			/* [in] Num of frames to be 
																	   advanced */
						);

void BRAP_DSP_P_GetFwVersionInfo(
		BRAP_DSP_Handle hDsp,
		BRAP_DSPVersionInfo	*psDSPVersion
		);

BERR_Code
BRAP_DSPCHN_P_SetAc3ConfigurationParams(
	BRAP_DSPCHN_Handle		hDspCh,			/* [in] DSP channel handle */
	BRAP_DSPCHN_Ac3ConfigParams	*psAc3ConfigParams 	/* [in] AC3 decoder configuration parameters */
	);

BERR_Code BRAP_DSP_P_GetExecIds(
	BRAP_DSPCHN_DecodeMode decMode, 
	BRAP_DSPCHN_AudioType audType, 
	BRAP_DSP_P_ExecID *psExecID); /* [out] */

void  BRAP_DSP_P_GetPpExecIds(
				BRAP_DSPCHN_PP_Algo	ePpAlgo,	/* [in] Post processing algorithm */
				BRAP_DSP_P_ExecID *psExecID /* [out] */);
				
BERR_Code BRAP_DSP_P_DownloadExecFW(
	BRAP_DSP_Handle hDsp, 
	BRAP_DSPCHN_DecodeMode decMode,
	BRAP_DSPCHN_AudioType audType,
	BRAP_DSPCHN_PP_Algo	ePpAlgo,
	bool bDnldFs,
	bool bDnPp,
	uint32_t fwCtxIndx,
	BRAP_DSP_P_ExecID sExecID);


/* BRAP_DSP_AllocateInterframeBuf: This function allocates a free Interframe buffer
 * that is required for decode operation.
 */
BERR_Code BRAP_DSP_AllocateInterframeBuf(
				BRAP_DSP_Handle		hDsp,
				unsigned int 	*puiInterframeBufIndex
);



/* BRAP_DSP_FreeInterframeBuf: Free interframe buffer */
void BRAP_DSP_FreeInterframeBuf(
		BRAP_DSP_Handle	hDsp,  
		unsigned int uiInterframeBufIndex
);

/* BRAP_DSP_P_InitInterframeBuffer: This function initializes interframe buffer as per the 
 * input audio type and programs corresponding entry in MIT. */
BERR_Code BRAP_DSP_P_InitInterframeBuffer(
				BRAP_DSP_Handle		hDsp,
				unsigned int	channelIndex,
				unsigned int 	uiInterframeBufIndex,
				BRAP_DSPCHN_AudioType	eAudioType
);

#if (BRAP_DSP_P_7401_NEWMIT==1)
BERR_Code BRAP_DSP_P_AllocatePpInterframeBuf(
				BRAP_DSP_Handle		hDsp,
				BRAP_DSPCHN_P_PpBranchInfo * sPpBranchInfo,
				unsigned int	* puiPpInterframeBuf
);


void BRAP_DSP_P_FreePpInterframeBuf(
		BRAP_DSP_Handle	hDsp,  
		unsigned int uiPpInterframeBufIndex
);

/* BRAP_DSP_P_AllocateIsIsIfBuffers: Allocate interstage ( IS ), interstage interface ( ISIF ) buffers.
 * These buffers are allocated in pairs */
BERR_Code BRAP_DSP_P_AllocateIsIsIfBuffers(
		BRAP_DSP_Handle	hDsp,
		unsigned int		*puiBufferIndex
);

BERR_Code BRAP_DSP_P_FreeIsIsIfBuffers(
		BRAP_DSP_Handle	hDsp,
		unsigned int		puiBufferIndex
);

BERR_Code BRAP_DSP_P_InitPpInterframeBuffer(
				BRAP_DSP_Handle		hDsp,
				unsigned int	channelIndex,
				unsigned int	uiPpBrIndex,
				unsigned int	uiPpStgIndex,
				unsigned int 	uiInterframeBufIndex,
				BRAP_DSPCHN_PP_Algo ePpAlgo
);

/* BRAP_DSP_P_UpdateMitForIsIsIfBuffers: Update MIT entries for interstage and interstage
 * interface buffers */
void	BRAP_DSP_P_UpdateMitForIsIsIfBuffers( 
	BRAP_DSP_Handle	hDsp,
	unsigned int uiContextNumber,
	unsigned int uiDecInterstageOutputBuf,
	unsigned int * puiInterstageInputBuf,
	unsigned int * puiInterstageOutputBuf
);
#endif

BERR_Code BRAP_DSP_P_AllocateConfigBufArray(
				BRAP_DSP_Handle		hDsp,
				unsigned int			*puiConfigBufArrayIndex,
				unsigned int			*puiConfigBufArrayStartAdr
);

void BRAP_DSP_P_FreeConfigBufArray(
		BRAP_DSP_Handle	hDsp,  
		unsigned int		puiDecConfigBufIndex
);
				
/* BRAP_DSP_P_AllocateDecConfigBuf: This function allocates a free decoder config buffer
 * that is required for decode operation.
 */
BERR_Code BRAP_DSP_P_AllocateDecConfigBuf(
				BRAP_DSP_Handle		hDsp,
				unsigned int			*puiDecConfigBufIndex,
				unsigned int			*puiDecConfigBufStartAdr
);

/* BRAP_DSP_P_FreeDecConfigBuf: Free decoder configuration buffer */
void BRAP_DSP_P_FreeDecConfigBuf(
		BRAP_DSP_Handle	hDsp,  
		unsigned int			puiDecConfigBufIndex
);

BERR_Code BRAP_DSPCHN_P_GetCurrentAudioParams (
						BRAP_DSPCHN_Handle	hDspCh,
						BRAP_DSPCHN_P_AudioParams	*psDspChAudioParams
						);

bool BRAP_DSPCHN_P_GetDecoderPauseState (
						BRAP_DSPCHN_Handle	hDspCh
						);

BERR_Code
BRAP_DSPCHN_P_SetConfig (
	BRAP_DSPCHN_Handle	hDspCh,
	BRAP_DEC_DownmixPath	eDownmixPath,
	BRAP_DEC_ConfigParams	*psDecConfigParams
	);

BERR_Code
BRAP_DSPCHN_P_SetPPConfig (
	BRAP_DSPCHN_Handle	hDspCh,
	BRAP_PP_ConfigParams		*psPpConfigParsms	
	);


BERR_Code BRAP_DSP_P_EnableDspWatchdogTimer (
						BRAP_DSP_Handle		hDsp,
						bool				bEnable
						);

void BRAP_DSPCHN_P_InitSpdifChanStatusParams (
							BRAP_DSPCHN_Handle hDspCh,
							BRAP_OP_SpdifChanStatusParams sSpdifParams);

BERR_Code BRAP_DSPCHN_P_PrgmFmmDstCxt (
            BRAP_DSPCHN_Handle hDspCh,
            BRAP_DSPCHN_P_OpPortParams * pOpPortParams 
);


BERR_Code BRAP_DSPCHN_P_GetCurrentPpConfig( 
		BRAP_DSPCHN_Handle hDspCh, 
		unsigned int uiPpBranchId,
		unsigned int uiPpStageId,
		BRAP_DSPCHN_PP_Algo *ePpAlgo, 
		BRAP_DSPCHN_P_PpStgConfigParam *uPpStgConfigParam
);

#ifndef BCHP_7411_VER
void BRAP_DSP_P_WatchDogResetHardware(BRAP_DSP_Handle hDsp);
#endif

BERR_Code 
BRAP_DSPCHN_P_InitializeHandle ( 
	BRAP_DSPCHN_Handle		hDspChn, /* [In] DSP channel handle */
	uint32_t				channelIndex	/* [in] Channel index starting from 0 to max number of channels */ 
	);

#ifdef __cplusplus
}
#endif

#endif /* BRAP_DSP_PRIV_H__ */

/* End of File */
