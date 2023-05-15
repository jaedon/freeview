/***************************************************************************
 *     Copyright (c) 2004-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdsp_raaga_fw_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/49 $
 * $brcm_Date: 2/6/13 4:22p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/dsp/src/raaga/bdsp_raaga_fw_priv.c $
 * 
 * Hydra_Software_Devel/49   2/6/13 4:22p ananthan
 * FWRAA-561: Mismatch in type enumerations for Audio Processing
 * Algorithms. Matching the definitions in sAudioProcessingInfo with the
 * BDSP_AudioProcessing enumarations.
 * 
 * Hydra_Software_Devel/48   2/4/13 12:49p nihar
 * SW7425-4229: Adding support for BDSP Version - Merging changes to
 * Hydra_Software_Devel Branch
 * 
 * Hydra_Software_Devel/47   1/29/13 5:28p sriniva
 * FWRAA-559: Renaming dts lbr braaga files to dts_express
 * 
 * Hydra_Software_Devel/BDSP_VERSION_BRANCH/2   1/30/13 11:54a nihar
 * SW7425-4229: Adding support for BDSP Version - Modification after Jon's
 * feedback.
 * 
 * Hydra_Software_Devel/BDSP_VERSION_BRANCH/1   1/29/13 5:05p nihar
 * SW7425-4229: Adding support for BDSP Version
 * 
 * Hydra_Software_Devel/46   1/10/13 11:02a skalapur
 * FWRAA-474:iLBC code merging to main latest
 * 
 * Hydra_Software_Devel/MAGNUM_ILBC_ISAC_integration/1   12/20/12 11:39a skalapur
 * FWRAA-474:iLBC and iSAC integration
 * 
 * Hydra_Software_Devel/45   11/26/12 1:20p ananthan
 * SW7425-4242: Add SCM3 interface and merge SCM to mainline.
 * 
 * Hydra_Software_Devel/44   8/24/12 12:40p rshijith
 * FWRAA-424: Removing warning
 * 
 * Hydra_Software_Devel/43   8/23/12 12:16a rshijith
 * FWRAA-424: Code modification to support VP8 encoder in raaga.
 * 
 * Hydra_Software_Devel/42   8/10/12 10:37a dlimaye
 * SW7425-2805: Add AMR-NB and AMR-WB codecs support Adding userconfig for
 * AMRWB decoder.
 * 
 * Hydra_Software_Devel/41   7/17/12 5:29p ramanatm
 * SW7425-2805: Add AMR-NB and AMR-WB codecs support Added new codecs: AMR
 * and AMRWB decoder/encoder
 * 
 * Hydra_Software_Devel/40   7/16/12 4:14p dlimaye
 * SW7425-2805: Add AMR-NB and AMR-WB codecs support Added new codecs: AMR
 * and AMRWB decoder/encoder
 * 
 * Hydra_Software_Devel/39   6/27/12 6:35p ramanatm
 * SW7425-2911: [7425] adding multichannel support to FLAC
 * 
 * Hydra_Software_Devel/38   6/19/12 3:16a gprasad
 * SW7425-2952: DSP directory structure flattening
 * 
 * Hydra_Software_Devel/37   6/6/12 1:00p ananthan
 * FWRAA-443: Add MAT Encoder support in Raaga
 * 
 * Hydra_Software_Devel/36   6/1/12 12:03p dlimaye
 * FWRAA-442: Add support for BD-LPCM decoder Added support in IDS,
 * decoder and host interface
 * 
 * Hydra_Software_Devel/35   5/11/12 3:37p dlimaye
 * FWRAA-433: Adding G722 encoder to raaga Adding bdsp changes and
 * sys_top_raaga changes
 * 
 * Hydra_Software_Devel/34   4/20/12 2:14p ramamurt
 * SW7425-2519: Secondary audio should be re-sampled at stream sampling
 * rate before mixing in FwMixer. Adding ASRC module in FW Mixer. Add
 * scratch buffer for mixer
 * 
 * Hydra_Software_Devel/33   3/30/12 5:25p gprasad
 * SW7425-2786: BDSP changes for adding MLP Passthru support
 * 
 * Hydra_Software_Devel/32   3/30/12 12:33a gprasad
 * SW7425-2786: BDSP changes for adding MLP Passthru support
 * 
 * Hydra_Software_Devel/31   3/22/12 7:01p gprasad
 * FWRAA-382: Broadcom 3d surround changes
 * 
 * Hydra_Software_Devel/30   3/22/12 6:49p gprasad
 * FWRAA-399: DTS LBR SUPPORT
 * 
 * Hydra_Software_Devel/29   3/16/12 2:37p ramanatm
 * SW7425-2378:[7425] Add support for Monkey Audio Codec (MAC)
 * 
 * Hydra_Software_Devel/28   2/3/12 12:22p ramanatm
 * SW7425-1752:[7425] Add support for FLAC media file format
 * 
 * Hydra_Software_Devel/27   1/27/12 1:32p ashoky
 * FWRAA-377: Merging h264 soft dsp video encoder to mainline.
 * 
 * Hydra_Software_Devel/26   1/20/12 4:26p gprasad
 * FWRAA-377 : Merging h264 video encoder to mainline
 * 
 * Hydra_Software_Devel/25   1/13/12 9:51p gprasad
 * FWRAA-342: Adding support for SPEEX AEC
 * 
 * Hydra_Software_Devel/24   1/8/12 8:20p gprasad
 * FWRAA-370: Updating AudioType info structures for G.7231 and echo
 * canceller
 * 
 * Hydra_Software_Devel/23   1/4/12 8:40p ashoky
 * SW7231-524: Increasing memreq for multichannel PCMWav.
 * 
 * Hydra_Software_Devel/22   12/29/11 5:56p gprasad
 * FWRAA-370: G723.1 codec development on Raaga platform
 * 
 * Hydra_Software_Devel/21   12/15/11 11:32p ashoky
 * SW7425-1671: Need DDP7.1 StreamInfo support:
 * 
 * Hydra_Software_Devel/20   12/5/11 12:39p ashoky
 * FWRAA-330: WMA Encoder Development on Raaga.
 * 
 * Hydra_Software_Devel/19   11/8/11 1:42p ramanatm
 * FWRAA-348:[7425] Vorbis bringup
 * 
 * Hydra_Software_Devel/18   8/19/11 3:49p gprasad
 * FWRAA-327: Updates to g.7xx decoder/encoder buffer requirements
 * 
 * Hydra_Software_Devel/17   8/5/11 5:46p ananthan
 * SW7425-724: [7425] Add BTSC audio encoder support
 * 
 * Hydra_Software_Devel/16   8/1/11 3:12p gprasad
 * FWRAA-327: Add support for G.711/G.726/G.729 codecs
 * 
 * Hydra_Software_Devel/15   7/26/11 2:33p gprasad
 * SWDTV-6761: Add StudioSound support
 * 
 * Hydra_Software_Devel/14   7/12/11 4:44p gprasad
 * SW7422-370: Add changes for bdsp_types.h based on code review feedback
 * 
 * Hydra_Software_Devel/13   6/22/11 3:32p gprasad
 * SW7422-370: Code review changes in bdsp_tyes.h
 * 
 * Hydra_Software_Devel/12   6/17/11 6:15p gprasad
 * SW7422-373: Enabling the codec info structure initializations
 *
 * Hydra_Software_Devel/11   6/16/11 5:40p gprasad
 * SW7422-373: Add GetInfo routines for AudioType, AudioProcessing, and
 * AudioEncode in BDSP
 *
 * Hydra_Software_Devel/10   6/16/11 3:53p gprasad
 * SW7422-373: Add GetInfo routines for AudioType, AudioProcessing, and
 * AudioEncode in BDSP
 *
 * Hydra_Software_Devel/9   6/14/11 10:16a gprasad
 * SW7422-389: Add Dolby MS11 support - Merging changes to main line
 *
 * Hydra_Software_Devel/MS11_DEVEL_BRANCH/2   5/30/11 4:39p gprasad
 * FWRAA-289: Adding changes to use auto generated defines / sizeof
 * operators to get the code size table zise etc ...
 *
 * Hydra_Software_Devel/8   5/25/11 2:09p gprasad
 * SWDTV-6762: [35233] APE: Add Audyssey ADV/ABX Support
 *
 * Hydra_Software_Devel/7   5/24/11 7:37p gprasad
 * SW7422-389: [7422] Add Dolby MS11 support
 *
 * Hydra_Software_Devel/6   5/17/11 3:08p ashoky
 * SW7425-477: Adding interface details for mp3 encoder.
 *
 * Hydra_Software_Devel/5   5/16/11 12:31p gprasad
 * SW7425-572: [7425] Adding VP6 support to basemodules
 *
 * Hydra_Software_Devel/3   4/14/11 10:56a gprasad
 * FWRAA-279: Phase 9p0 release for raaga
 *
 * Hydra_Software_Devel/2   4/6/11 11:39a gprasad
 * SW7425-286: [7425] Checkin raaga binaries to magnum into basemodules
 * folder
 *
 * Hydra_Software_Devel/1   4/5/11 3:50p gprasad
 * SW7425-286: [7425] Checkin raaga binaries to magnum into basemodules
 * folder
 *
 * Hydra_Software_Devel/4   3/15/11 3:02p gskerl
 * SW7422-146: Added include of bdsp_raaga_priv.h
 *
 * Hydra_Software_Devel/3   3/5/11 4:40a srajapur
 * SW7422-316 : [7422] Merging Phase 1.0 release firmware code base to
 * BASE DSP code base
 *
 * Hydra_Software_Devel/2   1/19/11 7:13a gautamk
 * SW7422-191: Various DSP Basmodules features and review comment
 * implementation
 *
 * Hydra_Software_Devel/1   1/13/11 5:41a gautamk
 * SW7422-191:[7422]Adding DSP support for Audio decode in Basemodule  It
 * includes - DSP Booting - Open time Download :  - Start Time Audio
 * Decode algorithm download  - Message buffer interaction with DSP:  -
 * Synchronized Interrupt:  - Async interrupts:  - Config Commands for
 * Decode/TSM/IDS: - Status Buffer for Decode/TSM/IDS:  - Start AC3 Audio
 * Decode:  - Pause/Resume Command:  - Frame Advance command:  - Device
 * level interrupt
 *
 * Hydra_Software_Devel/23   11/15/10 3:40p ananthan
 * FWRAA-189: [35230] Adding support for ADPCM
 *
 * Hydra_Software_Devel/22   11/12/10 2:31p ananthan
 * FWRAA-189: [35230] Adding support for ADPCM
 *
 * Hydra_Software_Devel/21   10/22/10 12:20p ananthan
 * SW35230-432: [35230] Fix compile error
 *
 * Hydra_Software_Devel/20   10/20/10 5:03p ananthan
 * FWRAA-191, SW35230-432, FWRAA-189, FWRAA-194: [35230] Adding userconfig
 * and status buffer structures for SRS Studio, ADPCM and G726
 *
 * Hydra_Software_Devel/19   10/13/10 8:17p ananthan
 * FWRAA-168, FWRAA-169, FWRAA-170: [35230] Adding support for custom DBE,
 * custom AVLP and custom ACF
 *
 * Hydra_Software_Devel/18   8/24/10 7:05p gprasad
 * FWRAA-140: [35230] Changing the framesync buffer size to present
 *
 * Hydra_Software_Devel/17   8/20/10 11:46p ashoky
 * FWRAA-140: [35230] Increasing the status buffer size of MS10 decoder.
 *
 * Hydra_Software_Devel/16   8/18/10 11:39p ashoky
 * FWRAA-151: [35230] Adding dts encode support. Changes to use dts encode
 * instead of dts broadcast encode.
 *
 * Hydra_Software_Devel/15   8/18/10 7:24p gprasad
 * FWRAA-140: [35230] MS10 DDP support
 *
 * Hydra_Software_Devel/14   8/18/10 6:44p gprasad
 * FWRAA-140: [35230] MS10 DDP support
 *
 * Hydra_Software_Devel/13   8/16/10 12:24p ananthan
 * FWRAA-153: [35230] DRA Decode Segfault on 8/13 code drop [Updating
 * status buffer size on DRA Framesync]
 *
 * Hydra_Software_Devel/12   8/13/10 7:43p gprasad
 * FWRAA-104: [35230] MS10 decoder support
 *
 * Hydra_Software_Devel/11   8/13/10 11:23a gprasad
 * FWRAA-107: [35230] Setting DTS framesync buffer sizes
 *
 * Hydra_Software_Devel/10   8/6/10 8:11p ashoky
 * FWRAA-133: [35230] increasing passthru max buffers supported to 6.
 *
 * Hydra_Software_Devel/9   8/3/10 7:16p gprasad
 * FWRAA-133: [35230] Merging all the host interface changes from phase
 * 6p0 to phase 7p5 take 2 into main line
 *
 * Hydra_Software_Devel/Raaga_Phase7p5tk2_Branch/2   8/2/10 8:53p ashoky
 * FWRAA-141: [35230] Changes for cv3 and modification in interstage
 * sizes.
 *
 * Hydra_Software_Devel/Raaga_Phase7p5tk2_Branch/1   7/26/10 8:30p gprasad
 * FWRAA-133: [35230] Merging all the host interface changes from phase
 * 6p0 to phase 7p5 take 2 into Raaga code base
 *
 * Hydra_Software_Devel/8   7/14/10 8:38p ashoky
 * FWRAA-121, FWRAA-126: [35230] Adding support for WMAPRO and PCMWAV
 * decoders.
 *
 * Hydra_Software_Devel/6   6/16/10 9:28p ananthan
 * FWRAA-114: [35230] Increase interframe size for AACHE decode
 *
 * Hydra_Software_Devel/5   6/4/10 12:54a ananthan
 * FWRAA-100: [35230] Increase interframe size for AC3 decode.
 *
 * Hydra_Software_Devel/4   5/14/10 3:53p dlimaye
 * FWRAA-85: [35230] Raaga TSM Bring up on BM: Issues relating to removal
 * of TSM node. Marking fwstatus buffers to be present in all the data
 * sync nodes
 *
 * Hydra_Software_Devel/3   5/13/10 3:19p dlimaye
 * FWRAA-85: [35230] Raaga TSM Bring up on BM: Issues relating to removal
 * of TSM node Merging the TSM statusinfo structure with that of
 * framesync
 *
 * Hydra_Software_Devel/2   5/13/10 11:53a dlimaye
 * FW35230-31: [35230] Add support for TSM in CIT Combined the interframe
 * and userconfig structures of datasync and TSM
 *
 * Hydra_Software_Devel/1   4/30/10 1:09p dlimaye
 * FW35230-30: [35230] Checkin CIT files in base4 Move the CIT files from
 * label RAAGA_FW35230-29_20100419 in base3 folder to the base4 folder
 *
 * Hydra_Software_Devel/RAAGA_EMULATION/3   3/30/10 11:42a dlimaye
 * FW35230-28: [35230] Checkin CIT files for BM
 *
 * Hydra_Software_Devel/RAAGA_EMULATION/2   1/13/10 1:53p dlimaye
 * FW35230-16: [35230] Change the DDP memory requirement for Raaga
 * Increased the memory requirement for LUP table, scratch, status buffer
 * and interframe.
 *
 * Hydra_Software_Devel/RAAGA_EMULATION/1   9/11/09 1:46p dlimaye
 * FW35230-2:[3513]Check-in the initial Raaga CIT files + FW binaries to
 * magnum
 * Checking in CIT files in the branch RAAGA_EMULATION
 *
 ***************************************************************************/

#ifdef TESTBENCH_BUILD
	#include "data_types.h"
	#include "brap_buff_defs_7043.h"
#else

#endif

#include "bdsp_raaga_priv.h"
#include "bdsp_raaga_fw.h"

#include "bdsp_raaga_fw_cit.h"
#include "bdsp_raaga_img_sizes.h"

#include "bdsp_raaga_version.h"

#ifndef BDSP_IMG_VP8_ENCODE_SIZE
#define BDSP_IMG_VP8_ENCODE_SIZE   100
#endif

#ifndef BDSP_IMG_VP8_ENCODE_TABLES_SIZE
#define BDSP_IMG_VP8_ENCODE_TABLES_SIZE  100
#endif

#ifndef BDSP_IMG_VP8_ENCODE_INTER_FRAME_SIZE
#define BDSP_IMG_VP8_ENCODE_INTER_FRAME_SIZE  100
#endif


#define BDSP_AF_P_EXTRA_SAMPLES	(8)	/* used to put some extra buffer */

/*	The structure will be instantiated based on the max algo type id */
/*	This will be a two dimensional structure. The second dimesion
	indicates what algos need to be executed for say passthrough of the same
	algorithm.
	There will be three such structures. One for Decoder, Encoder and
	one for post process */
/*	The structure will be initialized by the firmware and provided to the
	PI for each of the algorithm type */
/*	WARNING!!! Removed static declaration due to compilation error */

/*	Init structure for decoder algorithms */
const BDSP_AF_P_sALGO_EXEC_INFO BDSP_sDecAlgoExecInfo[BDSP_AudioType_eMax][BDSP_DecodeMode_eMax] =
{
	/* BDSP_AudioType_eMpeg */
	{
		/*	BDSP_sDecAlgoExecInfo[BDSP_AudioType_eMpeg][BDSP_DecodeMode_eDecode] = */
		{
			2,		/* Number of nodes in Mpeg Decode */
			{
				BDSP_AF_P_AlgoId_eMpegFrameSync,

				BDSP_AF_P_AlgoId_eMpegDecode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},

		/*	BDSP_sDecAlgoExecInfo[BDSP_AudioType_eMpeg][BDSP_DecodeMode_ePassThru] = */
		{
			2,		/* Number of nodes in Mpeg pass through */
			{
				BDSP_AF_P_AlgoId_eMpegFrameSync,

				BDSP_AF_P_AlgoId_ePassThru,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
	},


	/* BDSP_AudioType_eAacAdts */
	{
#ifdef BDSP_MS10_SUPPORT
		/* MS10 Dolby ADTS Pulse support */
		/* BDSP_sDecAlgoExecInfo[BDSP_AudioType_eAacAdts][BDSP_DecodeMode_eDecode] = */
		{
			2,
			{
				BDSP_AF_P_AlgoId_eAdtsFrameSync,
				BDSP_AF_P_AlgoId_eDolbyPulseDecode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
		/* BDSP_sDecAlgoExecInfo[BDSP_AudioType_eAacAdts][BDSP_DecodeMode_ePassThru] = */
		{
			2,
			{
				BDSP_AF_P_AlgoId_eAdtsFrameSync,
				BDSP_AF_P_AlgoId_ePassThru,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},

#else
		/*	BDSP_sDecAlgoExecInfo[BDSP_AudioType_eAacAdts][BDSP_DecodeMode_eDecode] = */
		{
			2,		/* Number of nodes in AAC ADTS Decode */
			{
				BDSP_AF_P_AlgoId_eAdtsFrameSync,

				BDSP_AF_P_AlgoId_eAacHeLpSbrDecode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},

		/*	BDSP_sDecAlgoExecInfo[BDSP_AudioType_eAacAdts][BDSP_DecodeMode_ePassThru] = */
		{
			2,		/* Number of nodes in AAC ADTS  pass thru */
			{
				BDSP_AF_P_AlgoId_eAdtsFrameSync,

				BDSP_AF_P_AlgoId_ePassThru,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
#endif
	},

	/* BDSP_AudioType_eAacLoas */
	{
#ifdef BDSP_MS10_SUPPORT

		/* MS10 Dolby LOAS Pulse support */
		/* BDSP_sDecAlgoExecInfo[BDSP_AudioType_eAacLoas][BDSP_DecodeMode_eDecode] = */
		{
			2,
			{
				BDSP_AF_P_AlgoId_eLoasFrameSync,
				BDSP_AF_P_AlgoId_eDolbyPulseDecode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
		/* BDSP_sDecAlgoExecInfo[BDSP_AudioType_eAacLoas][BDSP_DecodeMode_ePassThru] = */
		{
			2,
			{
				BDSP_AF_P_AlgoId_eLoasFrameSync,
				BDSP_AF_P_AlgoId_ePassThru,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},

#else
		/*	BDSP_sDecAlgoExecInfo[BDSP_AudioType_eAacLoas][BDSP_DecodeMode_eDecode] = */
		{
			2,		/* Number of nodes in AAC LOAS Decode */
			{
				BDSP_AF_P_AlgoId_eLoasFrameSync,

				BDSP_AF_P_AlgoId_eAacHeLpSbrDecode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},

		/*	BDSP_sDecAlgoExecInfo[BDSP_AudioType_eAacLoas][BDSP_DecodeMode_ePassThru] = */
		{
			2,		/* Number of nodes in AAC LOAS pass thru */
			{
				BDSP_AF_P_AlgoId_eLoasFrameSync,

				BDSP_AF_P_AlgoId_ePassThru,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
#endif
	},


	/* BDSP_AudioType_eAacSbrAdts */
	{
#ifdef BDSP_MS10_SUPPORT

		/* MS10 Dolby ADTS Pulse support */

		/* BDSP_sDecAlgoExecInfo[BDSP_AudioType_eAacSbrAdts][BDSP_DecodeMode_eDecode] = */
		{
			2,
			{
				BDSP_AF_P_AlgoId_eAdtsFrameSync,
				BDSP_AF_P_AlgoId_eDolbyPulseDecode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
		/* BDSP_sDecAlgoExecInfo[BDSP_AudioType_eAacSbrAdts][BDSP_DecodeMode_ePassThru] = */
		{
			2,
			{
				BDSP_AF_P_AlgoId_eAdtsFrameSync,
				BDSP_AF_P_AlgoId_ePassThru,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
#else
		/*	BDSP_sDecAlgoExecInfo[BDSP_AudioType_eAacSbrAdts][BDSP_DecodeMode_eDecode] = */
		{
			2,		/* Number of nodes in AAC-HE ADTS Decode */
			{
				BDSP_AF_P_AlgoId_eAdtsFrameSync,

				BDSP_AF_P_AlgoId_eAacHeLpSbrDecode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},

		/*	BDSP_sDecAlgoExecInfo[BDSP_AudioType_eAacSbrAdts][BDSP_DecodeMode_ePassThru] = */
		{
			2,		/* Number of nodes in AAC-HE ADTS pass thru */
			{
				BDSP_AF_P_AlgoId_eAdtsFrameSync,

				BDSP_AF_P_AlgoId_ePassThru,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
#endif
	},

	/* BDSP_AudioType_eAacSbrLoas */
	{
#ifdef BDSP_MS10_SUPPORT
		/* MS10 Dolby LOAS Pulse support */
		/* BDSP_sDecAlgoExecInfo[BDSP_AudioType_eAacLoasSbr][BDSP_DecodeMode_eDecode] = */
		{
			2,
			{
				BDSP_AF_P_AlgoId_eLoasFrameSync,
				BDSP_AF_P_AlgoId_eDolbyPulseDecode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
		/* BDSP_sDecAlgoExecInfo[BDSP_AudioType_eAacLoasSbr][BDSP_DecodeMode_ePassThru] = */
		{
			2,
			{
				BDSP_AF_P_AlgoId_eLoasFrameSync,
				BDSP_AF_P_AlgoId_ePassThru,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
#else
		/*	BDSP_sDecAlgoExecInfo[BDSP_AudioType_eAacSbrLoas][BDSP_DecodeMode_eDecode] = */
		{
			2,		/* Number of nodes in AAC-HE LOAS Decode */
			{
				BDSP_AF_P_AlgoId_eLoasFrameSync,

				BDSP_AF_P_AlgoId_eAacHeLpSbrDecode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid

			},
		},

		/*	BDSP_sDecAlgoExecInfo[BDSP_AudioType_eAacLoasSbr][BDSP_DecodeMode_ePassThru] = */
		{
			2,		/* Number of nodes in AAC-HE  LOAS pass thru */
			{
				BDSP_AF_P_AlgoId_eLoasFrameSync,

				BDSP_AF_P_AlgoId_ePassThru,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
#endif
	},
	/* BDSP_AudioType_eAc3 */
	{
#ifdef BDSP_MS10_SUPPORT

		/*MS10 DD Decoder */

		/* BDSP_sDecAlgoExecInfo[BDSP_AudioType_eAc3][BDSP_DecodeMode_eDecode] = */
		{
			2,
			{
				BDSP_AF_P_AlgoId_eMs10DdpFrameSync,
				BDSP_AF_P_AlgoId_eMs10DdpDecode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
		/* BDSP_sDecAlgoExecInfo[BDSP_AudioType_eAc3][BDSP_DecodeMode_ePassThru] = */
		{
			2,
			{
				BDSP_AF_P_AlgoId_eMs10DdpFrameSync,
				BDSP_AF_P_AlgoId_ePassThru,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		}
#else	 /* BDSP_MS10_SUPPORT */
	#ifdef BDSP_DDP_SUPPORT
		/*	BDSP_sDecAlgoExecInfo[BDSP_AudioType_eAc3][BDSP_DecodeMode_eDecode] = */
		{
			2,		/* Number of nodes in AC3 Plus Decode */
			{
				BDSP_AF_P_AlgoId_eDdpFrameSync,

				BDSP_AF_P_AlgoId_eDdpDecode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},

		/*	BDSP_sDecAlgoExecInfo[BDSP_AudioType_eAc3][BDSP_DecodeMode_ePassThru] = */
		{
			2,		/* Number of nodes in AC3 Plus Pass thru */
			{
				BDSP_AF_P_AlgoId_eDdpFrameSync,

				BDSP_AF_P_AlgoId_ePassThru,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
	#else /* BDSP_DDP_SUPPORT */
		/*	BDSP_sDecAlgoExecInfo[BDSP_AudioType_eAc3][BDSP_DecodeMode_eDecode] = */
		{
			2,		/* Number of nodes in AC3 Decode */
			{
				BDSP_AF_P_AlgoId_eAc3FrameSync,

				BDSP_AF_P_AlgoId_eAc3Decode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid

			},
		},

		/*	BDSP_sDecAlgoExecInfo[BDSP_AudioType_eAc3][BDSP_DecodeMode_ePassThru] = */
		{
			2,		/* Number of nodes in AC3 Pass thru */
			{
				BDSP_AF_P_AlgoId_eAc3FrameSync,

				BDSP_AF_P_AlgoId_ePassThru,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
	#endif /* BDSP_DDP_SUPPORT */
#endif /* BDSP_MS10_SUPPORT */
	},

	/* BDSP_AudioType_eAc3Plus */
	{
#ifdef BDSP_MS10_SUPPORT

		/*MS10 DDP  Decoder */
		/* BDSP_sDecAlgoExecInfo[BDSP_AudioType_eAc3Plus][BDSP_DecodeMode_eDecode] = */
		{
			2,
			{
				BDSP_AF_P_AlgoId_eMs10DdpFrameSync,
				BDSP_AF_P_AlgoId_eMs10DdpDecode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
		/* BDSP_sDecAlgoExecInfo[BDSP_AudioType_eAc3Plus][BDSP_DecodeMode_ePassThru] = */
		{
			2,
			{
				BDSP_AF_P_AlgoId_eMs10DdpFrameSync,
				BDSP_AF_P_AlgoId_ePassThru,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		}
#else /* BDSP_MS10_SUPPORT */
		/*	BDSP_sDecAlgoExecInfo[BDSP_AudioType_eAc3Plus][BDSP_DecodeMode_eDecode] = */
		{
			2,		/* Number of nodes in AC3 Plus Decode */
			{
				BDSP_AF_P_AlgoId_eDdpFrameSync,

				BDSP_AF_P_AlgoId_eDdpDecode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},

		/*	BDSP_sDecAlgoExecInfo[BDSP_AudioType_eAc3Plus][BDSP_DecodeMode_ePassThru] = */
		{
			2,		/* Number of nodes in AC3 Plus Pass thru */
			{
				BDSP_AF_P_AlgoId_eDdpFrameSync,

				BDSP_AF_P_AlgoId_ePassThru,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
#endif /* BDSP_MS10_SUPPORT */
	},
	/* BDSP_AudioType_eLpcmBd */
	{
		{
			0,
			{
				BDSP_AF_P_AlgoId_eInvalid,

				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
		{
			0,
			{
				BDSP_AF_P_AlgoId_eInvalid,

				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		}
	},
	/* BDSP_AudioType_eLpcmHdDvd */
	{
		{
			0,
			{
				BDSP_AF_P_AlgoId_eInvalid,

				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
		{
			0,
			{
				BDSP_AF_P_AlgoId_eInvalid,

				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		}
	},
	/* BDSP_AudioType_eDtshd */
	{
		/* BDSP_sDecAlgoExecInfo[BDSP_AudioType_eDtshd][BDSP_DecodeMode_eDecode] = */
		{
			2,
			{
				BDSP_AF_P_AlgoId_eDtsHdFrameSync,
				BDSP_AF_P_AlgoId_eDtsHdDecode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
		/* BDSP_sDecAlgoExecInfo[BDSP_AudioType_eDtshd][BDSP_DecodeMode_ePassThru] = */
		{
			2,
			{
				BDSP_AF_P_AlgoId_eDtsHdFrameSync,
				BDSP_AF_P_AlgoId_ePassThru,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		}
	},
	/* BDSP_AudioType_eLpcmDvd */
	{
		/*BDSP_sDecAlgoExecInfo[BDSP_AudioType_eLpcmDvd][BDSP_DecodeMode_eDecode] = */
		{
			2,
			{
				BDSP_AF_P_AlgoId_eDvdLpcmFrameSync,

				BDSP_AF_P_AlgoId_eDvdLpcmDecode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
		/*BDSP_sDecAlgoExecInfo[BDSP_AudioType_eLpcmDvd][BDSP_DecodeMode_ePassThru] = */
		{
			0,
			{
				BDSP_AF_P_AlgoId_eInvalid,

				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		}
	},

	/* BDSP_AudioType_eWmaStd */
	{

		/*	BDSP_sDecAlgoExecInfo[BDSP_AudioType_eWmaStd][BDSP_DecodeMode_eDecode] = */
		{
			2,		/* Number of nodes in WMA-STD Decode */
			{
				BDSP_AF_P_AlgoId_eWmaStdFrameSync,

				BDSP_AF_P_AlgoId_eWmaStdDecode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},

		/*	BDSP_sDecAlgoExecInfo[BDSP_AudioType_eWmaStd][BDSP_DecodeMode_ePassThru] = */
		{
			2,		/* Number of nodes in WMA-STD Pass thru */
			{
				BDSP_AF_P_AlgoId_eWmaStdFrameSync,

				BDSP_AF_P_AlgoId_ePassThru,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},

	},

	/*BDSP_AudioType_eMlp*/
	{
			{
			2,
			{
				BDSP_AF_P_AlgoId_eMlpFrameSync,
				BDSP_AF_P_AlgoId_eMlpDecode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
		{
			2,
			{
				BDSP_AF_P_AlgoId_eMlpFrameSync,
				BDSP_AF_P_AlgoId_eMLPPassThru,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		}
	},

	/* BDSP_AudioType_ePcm*/
	{
		{
			/*	BDSP_sDecAlgoExecInfo[BDSP_AudioType_ePcm][BDSP_DecodeMode_eDecode] = */
			2,
			{
				BDSP_AF_P_AlgoId_ePesFrameSync,

				BDSP_AF_P_AlgoId_ePassThru,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
		{
			/*	BDSP_sDecAlgoExecInfo[BDSP_AudioType_ePcm][BDSP_DecodeMode_ePassThru] = */
			0,
			{
				BDSP_AF_P_AlgoId_eInvalid,

				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		}
	},

	{
		/* BDSP_AudioType_eDtslbr */
		{
			/* BDSP_sDecAlgoExecInfo[BDSP_AudioType_eDtslbr][BDSP_DecodeMode_eDecode] = */
			
			2,
			{
					BDSP_AF_P_AlgoId_eDtsLbrFrameSync,
					
					BDSP_AF_P_AlgoId_eDtsLbrDecode,
					BDSP_AF_P_AlgoId_eInvalid,
					BDSP_AF_P_AlgoId_eInvalid,
					BDSP_AF_P_AlgoId_eInvalid,
					BDSP_AF_P_AlgoId_eInvalid
			},
			
		},
		/* BDSP_AudioType_eDtslbrPassThrough*/
		{
			0,
			{
				BDSP_AF_P_AlgoId_eInvalid,

				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		}
	},

	{
		/* BDSP_AudioType_eDdp7_1*/
		{
			0,
			{
				BDSP_AF_P_AlgoId_eInvalid,

				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
		{
			0,
			{
				BDSP_AF_P_AlgoId_eInvalid,

				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		}
	},

	{
		/*BDSP_AudioType_eMpegMc*/
		{
			0,
			{
				BDSP_AF_P_AlgoId_eInvalid,

				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
		{
			0,
			{
				BDSP_AF_P_AlgoId_eInvalid,

				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		}
	},


	/* BDSP_AudioType_eWmaPro */
	{

		/*	BDSP_sDecAlgoExecInfo[BDSP_AudioType_eWmaPro][BDSP_DecodeMode_eDecode] = */
		{
			2,		/* Number of nodes in WMA-pro Decode */
			{
				BDSP_AF_P_AlgoId_eWmaProFrameSync,

				BDSP_AF_P_AlgoId_eWmaProStdDecode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},

		/*	BDSP_sDecAlgoExecInfo[BDSP_AudioType_eWmaPro][BDSP_DecodeMode_ePassThru] = */
		{
			2,		/* Number of nodes in WMA-pro Pass thru */
			{
				BDSP_AF_P_AlgoId_eWmaProFrameSync,

				BDSP_AF_P_AlgoId_ePassThru,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
	},

	{
		/*BDSP_AudioType_eLpcmDvdA*/
		/* BDSP_sDecAlgoExecInfo[BDSP_AudioType_eLpcmDvdA][BDSP_DecodeMode_eDecode] = */
		{
			0,
			{
				BDSP_AF_P_AlgoId_eInvalid,

				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
		/* BDSP_sDecAlgoExecInfo[BDSP_AudioType_eLpcmDvdA][BDSP_DecodeMode_ePassThru] = */
		{
			0,
			{
				BDSP_AF_P_AlgoId_eInvalid,

				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		}
	},
	{
		/*BDSP_AudioType_eDtsBroadcast*/
		/* BDSP_sDecAlgoExecInfo[BDSP_AudioType_eDtsBroadcast][BDSP_DecodeMode_eDecode] = */
		{
			2,
			{
				BDSP_AF_P_AlgoId_eDtsFrameSync,

				BDSP_AF_P_AlgoId_eDtsHdDecode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
		/* BDSP_sDecAlgoExecInfo[BDSP_AudioType_eDtsBroadcast][BDSP_DecodeMode_ePassThru] = */
		{
			2,
			{
				BDSP_AF_P_AlgoId_eDtsFrameSync,

				BDSP_AF_P_AlgoId_ePassThru,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		}
	},
	{
		/*BDSP_AudioType_ePcmWav*/
		/* BDSP_sDecAlgoExecInfo[BDSP_AudioType_ePcmWav][BDSP_DecodeMode_eDecode] = */
		{
			2,
			{
				BDSP_AF_P_AlgoId_ePcmWavFrameSync,

				BDSP_AF_P_AlgoId_ePcmWavDecode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
		/* BDSP_sDecAlgoExecInfo[BDSP_AudioType_ePcmWav][BDSP_DecodeMode_ePassThru] = */
		{
			0,
			{
				BDSP_AF_P_AlgoId_eInvalid,

				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		}
	},
	/*BDSP_AudioType_eAmr*/
	{
		/* BDSP_sDecAlgoExecInfo[BDSP_AudioType_eAmr][BDSP_DecodeMode_eDecode] = */
		{
			2,
			{
				BDSP_AF_P_AlgoId_eAmrFrameSync,
				BDSP_AF_P_AlgoId_eAmrDecode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
		/* BDSP_sDecAlgoExecInfo[BDSP_AudioType_eAmr][BDSP_DecodeMode_ePassThru] = */
		{
			2,
			{
				BDSP_AF_P_AlgoId_eAmrFrameSync,
				BDSP_AF_P_AlgoId_ePassThru,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		}
	},

	/*BDSP_AudioType_eDra*/
	{
		/* BDSP_sDecAlgoExecInfo[BDSP_AudioType_eDra][BDSP_DecodeMode_eDecode] = */
		{
			2,
			{
				BDSP_AF_P_AlgoId_eDraFrameSync,
				BDSP_AF_P_AlgoId_eDraDecode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
		/* BDSP_sDecAlgoExecInfo[BDSP_AudioType_eDra][BDSP_DecodeMode_ePassThru] = */
		{
			2,
			{
				BDSP_AF_P_AlgoId_eDraFrameSync,
				BDSP_AF_P_AlgoId_ePassThru,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		}
	},
	/*BDSP_AudioType_eRealAudioLbr*/
	{
		/* BDSP_sDecAlgoExecInfo[BDSP_AudioType_eRealAudioLbr][BDSP_DecodeMode_eDecode] = */
		{
			2,
			{
				BDSP_AF_P_AlgoId_eRealAudioLbrFrameSync,
				BDSP_AF_P_AlgoId_eRealAudioLbrDecode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
		/* BDSP_sDecAlgoExecInfo[BDSP_AudioType_eRealAudioLbr][BDSP_DecodeMode_ePassThru] = */
		{
			2,
			{
				BDSP_AF_P_AlgoId_eRealAudioLbrFrameSync,
				BDSP_AF_P_AlgoId_ePassThru,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		}
	},
	/*BDSP_AudioType_eAdpcm*/
	{
		/* BDSP_sDecAlgoExecInfo[BDSP_AudioType_eAdpcm][BDSP_DecodeMode_eDecode] = */
		{
			2,
			{
				BDSP_AF_P_AlgoId_eAdpcmFrameSync,
				BDSP_AF_P_AlgoId_eAdpcmDecode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
		/* BDSP_sDecAlgoExecInfo[BDSP_AudioType_eAdpcm][BDSP_DecodeMode_ePassThru] = */
		{
			2,
			{
				BDSP_AF_P_AlgoId_eAdpcmFrameSync,
				BDSP_AF_P_AlgoId_ePassThru,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		}
	},
	/*BDSP_AudioType_eG711G726*/
	{
		/* BDSP_sDecAlgoExecInfo[BDSP_AudioType_eG711G726][BDSP_DecodeMode_eDecode] = */
		{
			2,
			{
				BDSP_AF_P_AlgoId_eG711G726FrameSync,
				BDSP_AF_P_AlgoId_eG711G726Decode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
		/* BDSP_sDecAlgoExecInfo[BDSP_AudioType_eG711G726][BDSP_DecodeMode_ePassThru] = */
		{
			2,
			{
				BDSP_AF_P_AlgoId_eG711G726FrameSync,
				BDSP_AF_P_AlgoId_ePassThru,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		}
	},
	/*BDSP_AudioType_eG729*/
	{
		/* BDSP_sDecAlgoExecInfo[BDSP_AudioType_eG729][BDSP_DecodeMode_eDecode] = */
		{
			2,
			{
				BDSP_AF_P_AlgoId_eG729FrameSync,
				BDSP_AF_P_AlgoId_eG729Decode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
		/* BDSP_sDecAlgoExecInfo[BDSP_AudioType_eG729][BDSP_DecodeMode_ePassThru] = */
		{
			2,
			{
				BDSP_AF_P_AlgoId_eG729FrameSync,
				BDSP_AF_P_AlgoId_ePassThru,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		}
	},
	/*BDSP_AudioType_eVorbis*/
	{
		/* BDSP_sDecAlgoExecInfo[BDSP_AudioType_eVorbis][BDSP_DecodeMode_eDecode] = */
		{
			2,
			{
				BDSP_AF_P_AlgoId_eVorbisFrameSync,
				BDSP_AF_P_AlgoId_eVorbisDecode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
		/* BDSP_sDecAlgoExecInfo[BDSP_AudioType_eVorbis][BDSP_DecodeMode_ePassThru] = */
		{
			2,
			{
				BDSP_AF_P_AlgoId_eVorbisFrameSync,
				BDSP_AF_P_AlgoId_ePassThru,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		}
	},
	/*BDSP_AudioType_eG723_1*/
	{
		/* BDSP_sDecAlgoExecInfo[BDSP_AudioType_eG723_1][BDSP_DecodeMode_eDecode] = */
		{
			2,
			{
				BDSP_AF_P_AlgoId_eG723_1FrameSync,
				BDSP_AF_P_AlgoId_eG723_1Decode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
		/* BDSP_sDecAlgoExecInfo[BDSP_AudioType_eG723_1][BDSP_DecodeMode_ePassThru] = */
		{
			2,
			{
				BDSP_AF_P_AlgoId_eG723_1FrameSync,
				BDSP_AF_P_AlgoId_ePassThru,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		}
	},
	/*BDSP_AudioType_eFlac*/
	{
		/* BDSP_sDecAlgoExecInfo[BDSP_AudioType_eFlac][BDSP_DecodeMode_eDecode] = */
		{
			2,
			{
				BDSP_AF_P_AlgoId_eFlacFrameSync,
				BDSP_AF_P_AlgoId_eFlacDecode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
		/* BDSP_sDecAlgoExecInfo[BDSP_AudioType_eFlac][BDSP_DecodeMode_ePassThru] = */
		{
			2,
			{
				BDSP_AF_P_AlgoId_eFlacFrameSync,
				BDSP_AF_P_AlgoId_ePassThru,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		}
	},
	/*BDSP_AudioType_eMac*/
	{
		/* BDSP_sDecAlgoExecInfo[BDSP_AudioType_eMac][BDSP_DecodeMode_eDecode] = */
		{
			2,
			{
				BDSP_AF_P_AlgoId_eMacFrameSync,
				BDSP_AF_P_AlgoId_eMacDecode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
		/* BDSP_sDecAlgoExecInfo[BDSP_AudioType_eMac][BDSP_DecodeMode_ePassThru] = */
		{
			2,
			{
				BDSP_AF_P_AlgoId_eMacFrameSync,
				BDSP_AF_P_AlgoId_ePassThru,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		}
	},
	/*BDSP_AudioType_eAmrwb*/
	{
		/* BDSP_sDecAlgoExecInfo[BDSP_AudioType_eAmrwb][BDSP_DecodeMode_eDecode] = */
		{
			2,
			{
				BDSP_AF_P_AlgoId_eAmrWbFrameSync,
				BDSP_AF_P_AlgoId_eAmrWbDecode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
		/* BDSP_sDecAlgoExecInfo[BDSP_AudioType_eAmrwb][BDSP_DecodeMode_ePassThru] = */
		{
			2,
			{
				BDSP_AF_P_AlgoId_eAmrWbFrameSync,
				BDSP_AF_P_AlgoId_ePassThru,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		}
	},
	/*BDSP_AudioType_eiLBC*/
	{
		/* BDSP_sDecAlgoExecInfo[BDSP_AudioType_eiLBC][BDSP_DecodeMode_eDecode] = */
		{
			2,
			{
				BDSP_AF_P_AlgoId_eiLBCFrameSync,
				BDSP_AF_P_AlgoId_eiLBCDecode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
		/* BDSP_sDecAlgoExecInfo[BDSP_AudioType_eiLBC][BDSP_DecodeMode_ePassThru] = */
		{
			2,
			{
				BDSP_AF_P_AlgoId_eiLBCFrameSync,
				BDSP_AF_P_AlgoId_ePassThru,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		}
	},
	
		/*BDSP_AudioType_eiSAC*/
	{
		/* BDSP_sDecAlgoExecInfo[BDSP_AudioType_eiSAC][BDSP_DecodeMode_eDecode] = */
		{
			2,
			{
				BDSP_AF_P_AlgoId_eiSACFrameSync,
				BDSP_AF_P_AlgoId_eiSACDecode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
		/* BDSP_sDecAlgoExecInfo[BDSP_AudioType_eAmrwb][BDSP_DecodeMode_ePassThru] = */
		{
			2,
			{
				BDSP_AF_P_AlgoId_eiSACFrameSync,
				BDSP_AF_P_AlgoId_ePassThru,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		}
	},
};

const BDSP_AF_P_sALGO_EXEC_INFO BDSP_sVideoDecAlgoExecInfo[BDSP_VideoType_eMax][BDSP_DecodeMode_eMax] =
{
	/*BDSP_VideoType_eRealVideo9*/
	{
		/* BDSP_sVideoDecAlgoExecInfo[BDSP_VideoType_eRealVideo9][BDSP_DecodeMode_eDecode] = */
		{
			2,
			{
				BDSP_VF_P_AlgoId_eRealVideo9FrameSync,
				BDSP_VF_P_AlgoId_eRealVideo9Decode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
		/* BDSP_sVideoDecAlgoExecInfo[BDSP_VideoType_eRealVideo9][BDSP_DecodeMode_ePassThru] = */
		{
			0,
			{
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		}
	},
	/*BDSP_VideoType_eVP6*/
	{
		/* BDSP_sVideoDecAlgoExecInfo[BDSP_VideoType_eVP6][BDSP_DecodeMode_eDecode] = */
		{
			2,
			{
				BDSP_VF_P_AlgoId_eVP6FrameSync,
				BDSP_VF_P_AlgoId_eVP6Decode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
		/* BDSP_sVideoDecAlgoExecInfo[BDSP_VideoType_eVP6][BDSP_DecodeMode_ePassThru] = */
		{
			0,
			{
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		}
	},

};

/*	Init structure for Encoder algorithms */
const BDSP_AF_P_sALGO_EXEC_INFO BDSP_sEncAlgoExecInfo[BDSP_AudioEncode_eMax][BDSP_EncodeMode_eSimulMode] =
{
	/*BDSP_AudioEncode_eMpeg1Layer3*/
	{
		/*	BDSP_sEncAlgoExecInfo[BDSP_AudioEncode_eMpeg1Layer3][BDSP_EncodeMode_eRealtime] = */
		{
			2,	/* Number of nodes in MPEG1 L3 Encode */
			{
				BDSP_AF_P_AlgoId_eInvalid,

				BDSP_AF_P_AlgoId_eMpegL3Encode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},

		/*	BDSP_sEncAlgoExecInfo[BDSP_AudioEncode_eMpeg1Layer3][BDSP_EncodeMode_eNonRealtime] = */
		{
			2,	/* Number of nodes in MPEG1 Layer 3 Encode NRT */
			{
				BDSP_AF_P_AlgoId_eInvalid,

				BDSP_AF_P_AlgoId_eMpegL3Encode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*BDSP_AudioEncode_eMpeg1Layer2*/
	{
		/*	BDSP_sEncAlgoExecInfo[BDSP_AudioEncode_eMpeg1Layer2][BDSP_EncodeMode_eRealtime] = */
		{
			2,	/* Number of nodes in MPEG1 Layer 2 Encode */
			{
				BDSP_AF_P_AlgoId_eMpegL2EncFrameSync,

				BDSP_AF_P_AlgoId_eMpegL2Encode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},

		/*	BDSP_sEncAlgoExecInfo[BDSP_AudioEncode_eMpeg1Layer2][BDSP_EncodeMode_eNonRealtime] = */
		{
			2,	/* Number of nodes in MPEG1 Layer 2 Encode NRT */
			{
				BDSP_AF_P_AlgoId_eMpegL2EncFrameSync,

				BDSP_AF_P_AlgoId_eMpegL2Encode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid

			},
		},
	},

	/*BDSP_AudioEncode_eDTS*/
	{
		/*	BDSP_sEncAlgoExecInfo[BDSP_AudioEncode_eDTS][BDSP_EncodeMode_eRealtime] = */
		{
			2,	/* Number of nodes in DTS Encode */
			{
				BDSP_AF_P_AlgoId_eDtsEncFrameSync,

				BDSP_AF_P_AlgoId_eDtsEncode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},

		/*	BDSP_sEncAlgoExecInfo[BDSP_AudioEncode_eDTS][BDSP_EncodeMode_eNonRealtime] = */
		{
			2,	/* Number of nodes in DTS Encode */
			{
				BDSP_AF_P_AlgoId_eDtsEncFrameSync,

				BDSP_AF_P_AlgoId_eDtsEncode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
	},

	/*BDSP_AudioEncode_eAacLc*/
	{
		/*	BDSP_sEncAlgoExecInfo[BDSP_AudioEncode_eAacLc][BDSP_EncodeMode_eRealtime] = */
		{
			2,	/* Number of nodes in AAC-LC Encode */
			{
				BDSP_AF_P_AlgoId_eAacLcEncFrameSync,

				BDSP_AF_P_AlgoId_eAacLcEncode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid

			},
		},

		/*	BDSP_sEncAlgoExecInfo[BDSP_AudioEncode_eAacLc][BDSP_EncodeMode_eNonRealtime] = */
		{
			2,	/* Number of nodes in AAC-LC Encode */
			{
				BDSP_AF_P_AlgoId_eAacLcEncFrameSync,

				BDSP_AF_P_AlgoId_eAacLcEncode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid

			},
		},
	},

	/*BDSP_AudioEncode_eAacHe*/
	{
		/*	BDSP_sEncAlgoExecInfo[BDSP_AudioEncode_eAacHe][BDSP_EncodeMode_eRealtime] = */
		{
			2,	/* Number of nodes in AAC-HE Encode */
			{
				BDSP_AF_P_AlgoId_eAacHeEncFrameSync,

				BDSP_AF_P_AlgoId_eAacHeEncode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},

		/*	BDSP_sEncAlgoExecInfo[BDSP_AudioEncode_eAacHe][BDSP_EncodeMode_eNonRealtime] = */
		{
			2,	/* Number of nodes in AAC-HE Encode NRT*/
			{
				BDSP_AF_P_AlgoId_eAacHeEncFrameSync,

				BDSP_AF_P_AlgoId_eAacHeEncode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
	},

	/*BDSP_AudioEncode_eAc3*/
	{

#ifdef BDSP_MS10_SUPPORT

		/*MS10 DD Encoder */
		/*	BDSP_sEncAlgoExecInfo[BDSP_AudioEncode_eAc3][BDSP_EncodeMode_eRealtime] = */
		{
			2,	/* Number of nodes in DD Transcode */
			{
                BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eMs10DDTranscode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
		/*	BDSP_sEncAlgoExecInfo[BDSP_AudioEncode_eAc3][BDSP_EncodeMode_eNonRealtime] = */
		{
			2,	/* Number of nodes in DDTranscode NRT*/
			{
                BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eMs10DDTranscode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
#else
		/*	BDSP_sEncAlgoExecInfo[BDSP_AudioEncode_eAc3][BDSP_EncodeMode_eRealtime] = */
		{
			2,	/* Number of nodes in AC3 Encode */
			{
				BDSP_AF_P_AlgoId_eInvalid,

				BDSP_AF_P_AlgoId_eAc3Encode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},

		/*	BDSP_sEncAlgoExecInfo[BDSP_AudioEncode_eAc3][BDSP_EncodeMode_eNonRealtime] = */
		{
			2,	/* Number of nodes in AC3 Encode NRT*/
			{
				BDSP_AF_P_AlgoId_eInvalid,

				BDSP_AF_P_AlgoId_eAc3Encode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
#endif
	},

	/*BDSP_AudioEncode_eSbc*/
	{
		/*	BDSP_sEncAlgoExecInfo[BDSP_AudioEncode_eSbc][BDSP_EncodeMode_eRealtime] = */
		{
			2,	/* Number of nodes in SBC Encode */
			{
                BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eSbcEncode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
		/*	BDSP_sEncAlgoExecInfo[BDSP_AudioEncode_eSbc][BDSP_EncodeMode_eNonRealtime] = */
		{
			2,	/* Number of nodes in SBC Encode NRT*/
			{
                BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eSbcEncode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
	},

	/*BDSP_AudioEncode_eG711G726*/
	{
		/*	BDSP_sEncAlgoExecInfo[BDSP_AudioEncode_eG711G726][BDSP_EncodeMode_eRealtime] = */
		{
			2,	/* Number of nodes in G711 G726 Encode */
			{
                BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eG711G726Encode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
		/*	BDSP_sEncAlgoExecInfo[BDSP_AudioEncode_eG711G726][BDSP_EncodeMode_eNonRealtime] = */
		{
			2,	/* Number of nodes in G711 G726 Encode NRT*/
			{
                BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eG711G726Encode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
	},

	/*BDSP_AudioEncode_eG729*/
	{
		/*	BDSP_sEncAlgoExecInfo[BDSP_AudioEncode_eG729][BDSP_EncodeMode_eRealtime] = */
		{
			2,	/* Number of nodes in G729 Encode */
			{
                BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eG729Encode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
		/*	BDSP_sEncAlgoExecInfo[BDSP_AudioEncode_eG729][BDSP_EncodeMode_eNonRealtime] = */
		{
			2,	/* Number of nodes in G729 Encode NRT*/
			{
                BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eG729Encode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*	BDSP_AudioEncode_eWma */
	{
		/*	BDSP_sEncAlgoExecInfo[BDSP_AudioEncode_eWma][BDSP_EncodeMode_eRealtime] = */
		{
			2,	/* Number of nodes in WMA Encode */
			{
                BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eWmaEncode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
		/*	BDSP_sEncAlgoExecInfo[BDSP_AudioEncode_eWma][BDSP_EncodeMode_eNonRealtime] = */
		{
			2,	/* Number of nodes in WMA Encode NRT*/
			{
                BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eWmaEncode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*	BDSP_AudioEncode_eG723_1 */
	{
		/*	BDSP_sEncAlgoExecInfo[BDSP_AudioEncode_eG723_1][BDSP_EncodeMode_eRealtime] = */
		{
			2,	/* Number of nodes in WMA Encode */
			{
                BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eG723_1Encode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
		/*	BDSP_sEncAlgoExecInfo[BDSP_AudioEncode_eG723_1][BDSP_EncodeMode_eNonRealtime] = */
		{
			2,	/* Number of nodes in WMA Encode NRT*/
			{
                BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eG723_1Encode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*BDSP_AudioEncode_eG722*/
	{
		/*	BDSP_sEncAlgoExecInfo[BDSP_AudioEncode_eG722][BDSP_EncodeMode_eRealtime] = */
		{
			2,	/* Number of nodes in G722 Encode */
			{
                BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eG722Encode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
		/*	BDSP_sEncAlgoExecInfo[BDSP_AudioEncode_eG722][BDSP_EncodeMode_eNonRealtime] = */
		{
			2,	/* Number of nodes in G722 Encode NRT*/
			{
                BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eG722Encode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*BDSP_AudioEncode_eAmr*/
	{
		/*	BDSP_sEncAlgoExecInfo[BDSP_AudioEncode_eAmr][BDSP_EncodeMode_eRealtime] = */
		{
			2,	/* Number of nodes in Amr Encode */
			{
		              BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eAmrEncode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
		/*	BDSP_sEncAlgoExecInfo[BDSP_AudioEncode_eAmr][BDSP_EncodeMode_eNonRealtime] = */
		{
			2,	/* Number of nodes in AMR Encode NRT*/
			{
                		BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eAmrEncode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*BDSP_AudioEncode_eAmrwb*/
	{
		/*	BDSP_sEncAlgoExecInfo[BDSP_AudioEncode_eAmrwb][BDSP_EncodeMode_eRealtime] = */
		{
			2,	/* Number of nodes in Amrwb Encode */
			{
		              BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eAmrwbEncode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
		/*	BDSP_sEncAlgoExecInfo[BDSP_AudioEncode_eAmrwb][BDSP_EncodeMode_eNonRealtime] = */
		{
			2,	/* Number of nodes in AMRwb Encode NRT*/
			{
                		BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eAmrwbEncode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
	},

	/*BDSP_AudioEncode_eiLBC*/
	{
		/*	BDSP_sEncAlgoExecInfo[BDSP_AudioEncode_eiLBC][BDSP_EncodeMode_eRealtime] = */
		{
			2,	/* Number of nodes in ilbc Encode */
			{
		        BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eiLBCEncode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
		/*	BDSP_sEncAlgoExecInfo[BDSP_AudioEncode_eiLBC][BDSP_EncodeMode_eNonRealtime] = */
		{
			2,	/* Number of nodes in isac Encode NRT*/
			{
                BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eiLBCEncode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
	},
	
		/*BDSP_AudioEncode_eiSAC*/
	{
		/*	BDSP_sEncAlgoExecInfo[BDSP_AudioEncode_eiSAC][BDSP_EncodeMode_eRealtime] = */
		{
			2,	/* Number of nodes in isac Encode */
			{
		        BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eiSACEncode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
		/*	BDSP_sEncAlgoExecInfo[BDSP_AudioEncode_eiSAC][BDSP_EncodeMode_eNonRealtime] = */
		{
			2,	/* Number of nodes in isac Encode NRT*/
			{
                BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eiSACEncode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
	},

};

/*	Init structure for Encoder algorithms */
const BDSP_AF_P_sALGO_EXEC_INFO BDSP_sVideoEncAlgoExecInfo[BDSP_VideoEncode_eMax][BDSP_EncodeMode_eSimulMode] =
{
	/*BDSP_AF_P_AlgoId_eVideoH264Encode*/
	{
		/*	BDSP_sVideoEncAlgoExecInfo[BDSP_AF_P_AlgoId_eVideoH264Encode][BDSP_EncodeMode_eRealtime] = */
		{
			2,	/* Number of nodes in H264 Video Encode  */
			{
				BDSP_AF_P_AlgoId_eVidIDSCommonLib,

				BDSP_VF_P_AlgoId_eH264Encode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},

		/*	BDSP_sVideoEncAlgoExecInfo[BDSP_AF_P_AlgoId_eVideoH264Encode][BDSP_EncodeMode_eNonRealtime] = */
		{
			2,	/* Number of nodes in H264 Video Encode  */
			{
				BDSP_AF_P_AlgoId_eVidIDSCommonLib,

				BDSP_VF_P_AlgoId_eH264Encode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*BDSP_AF_P_AlgoId_eVideoVP8Encode*/
	{
		/*	BDSP_sVideoEncAlgoExecInfo[BDSP_AF_P_AlgoId_eVideoVP8Encode][BDSP_EncodeMode_eRealtime] = */
		{
			2,	/* Number of nodes in VP8 Video Encode  */
			{
				BDSP_AF_P_AlgoId_eVidIDSCommonLib,
	
				BDSP_VF_P_AlgoId_eVP8Encode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
	
		/*	BDSP_sVideoEncAlgoExecInfo[BDSP_AF_P_AlgoId_eVideoVP8Encode][BDSP_EncodeMode_eNonRealtime] = */
		{
			2,	/* Number of nodes in VP8 Video Encode  */
			{
				BDSP_AF_P_AlgoId_eVidIDSCommonLib,
	
				BDSP_VF_P_AlgoId_eVP8Encode,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
	},

};

/*	Init structure for Post Processing algorithms */
const BDSP_AF_P_sALGO_EXEC_INFO BDSP_sPpAlgoExecInfo[BDSP_AudioProcessing_eMax][BDSP_DSPCHN_ProcessingMode_eMax] =
{
	/*BDSP_AudioProcessing_eDtsNeo*/
	{
		/*	BDSP_sPpAlgoExecInfo[BDSP_AudioProcessing_eDtsNeo][BDSP_DSPCHN_ProcessingMode_ePP] = */
		{
			2,	/* Number of nodes in DTS Neo Post Proc */
			{
				BDSP_AF_P_AlgoId_eInvalid,

				BDSP_AF_P_AlgoId_eDtsNeoPostProc,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*BDSP_AudioProcessing_eAVL*/
	{
		/*	BDSP_sPpAlgoExecInfo[BDSP_AudioProcessing_eAVL][BDSP_DSPCHN_ProcessingMode_ePP] = */
		{
			2,	/* Number of nodes in AVL Post Proc */
			{
				BDSP_AF_P_AlgoId_eInvalid,

				BDSP_AF_P_AlgoId_eAvlPostProc,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*BDSP_AudioProcessing_ePLll,*/    /* Dolby Prologic-II. */
	{
		/*	BDSP_sPpAlgoExecInfo[BDSP_AudioProcessing_ePLll][BDSP_DSPCHN_ProcessingMode_ePP] = */
		{
			2,	/* Number of nodes in Dolby Prologic-II*/
			{
				BDSP_AF_P_AlgoId_eInvalid,

				BDSP_AF_P_AlgoId_ePl2PostProc,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
	},

	/*BDSP_AudioProcessing_eSrsXt,	*/    /* TruSurroundXT. */
	{
		/*	BDSP_sPpAlgoExecInfo[BDSP_AudioProcessing_eSrsXt][BDSP_DSPCHN_ProcessingMode_ePP] = */
		{
			2,	/* Number of nodes in Srs Xt */
			{
				BDSP_AF_P_AlgoId_eInvalid,

				BDSP_AF_P_AlgoId_eSrsTruSurroundPostProc,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*	BDSP_AudioProcessing_eBbe,	  */       /* BBE */
	{
		/*	BDSP_sPpAlgoExecInfo[BDSP_AudioProcessing_eBbe][BDSP_DSPCHN_ProcessingMode_ePP] = */
		{
			0,	/* Number of nodes in BBE Post Proc */
			{
				BDSP_AF_P_AlgoId_eInvalid,

				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*	BDSP_AudioProcessing_eSrc,	 */	    /* Sample Rate Change */
	{
		/*	BDSP_sPpAlgoExecInfo[BDSP_AudioProcessing_eSrc][BDSP_DSPCHN_ProcessingMode_ePP] = */
		{
			2,	/* Number of nodes in SRC Post Proc */
			{
				BDSP_AF_P_AlgoId_eInvalid,

				BDSP_AF_P_AlgoId_eSrcPostProc,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*BDSP_AudioProcessing_eCustomVoice, */  /* CUSTOM Voice Algorithm*/
	{
		/*	BDSP_sPpAlgoExecInfo[BDSP_AudioProcessing_eCustomVoice][BDSP_DSPCHN_ProcessingMode_ePP] = */
		{
			2,	/* Number of nodes in CUSTOM Voice Post Proc */
			{
				BDSP_AF_P_AlgoId_eInvalid,

				BDSP_AF_P_AlgoId_eCustomVoicePostProc,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*BDSP_AudioProcessing_eAacDownmix,*/
	{
		/*	BDSP_sPpAlgoExecInfo[BDSP_AudioProcessing_eAacDownmix][BDSP_DSPCHN_ProcessingMode_ePP] = */
		{
			0,	/* Number of nodes in Downmix Post Proc */
			{
				BDSP_AF_P_AlgoId_eInvalid,

				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
	},

	/*BDSP_AudioProcessing_eDsola,*/
	{
		/*	BDSP_sPpAlgoExecInfo[BDSP_AudioProcessing_eDsola][BDSP_DSPCHN_ProcessingMode_ePP] = */
		{
			2,	/* Number of nodes in DSOLA  */
			{
				BDSP_AF_P_AlgoId_eInvalid,

				BDSP_AF_P_AlgoId_eDsolaPostProc,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*BDSP_AudioProcessing_eSrsHd,*/
	{
		/*	BDSP_sPpAlgoExecInfo[BDSP_AudioProcessing_eSrsHd][BDSP_DSPCHN_ProcessingMode_ePP] = */
		{
			2,	/* Number of nodes in SrsHd  */
			{
				BDSP_AF_P_AlgoId_eInvalid,

				BDSP_AF_P_AlgoId_eSrsTruSurroundHDPostProc,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*BDSP_AudioProcessing_eGenericPassThru,*/
	{
		/*	BDSP_sPpAlgoExecInfo[BDSP_AudioProcessing_eGenericPassThru][BDSP_DSPCHN_ProcessingMode_ePP] = */
		{
			2,	/* Number of nodes in Generic Pass through  */
			{
				BDSP_AF_P_AlgoId_eInvalid,

				BDSP_AF_P_AlgoId_ePassThru,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*BDSP_AudioProcessing_eMlpPassThru,*/
	{
		/*	BDSP_sPpAlgoExecInfo[BDSP_AudioProcessing_eMlpPassThru][BDSP_DSPCHN_ProcessingMode_ePP] = */
		{
			2,	/* Number of nodes in Generic Pass through  */
			{
				BDSP_AF_P_AlgoId_eInvalid,

				BDSP_AF_P_AlgoId_eMLPPassThru,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*BDSP_AudioProcessing_eSrsTruVolume,*/
	{
		/*	BDSP_sPpAlgoExecInfo[BDSP_AudioProcessing_eSrsTruVolume][BDSP_DSPCHN_ProcessingMode_ePP] = */
		{
			2,	/* Number of nodes in Srs VolumeIq  */
			{
				BDSP_AF_P_AlgoId_eInvalid,

				BDSP_AF_P_AlgoId_eSrsTruVolumePostProc,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*BDSP_AudioProcessing_eDolbyVolume,*/
	{
		/*	BDSP_sPpAlgoExecInfo[BDSP_AudioProcessing_eDolbyVolume][BDSP_DSPCHN_ProcessingMode_ePP] = */
		{
			2,	/* Number of nodes in Dolby Volume */
			{
				BDSP_AF_P_AlgoId_eInvalid,

				BDSP_AF_P_AlgoId_eDolbyVolumePostProc,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*BDSP_AudioProcessing_eAudysseyVolume,*/
	{
		/*	BDSP_sPpAlgoExecInfo[BDSP_AudioProcessing_eAudysseyVolume][BDSP_DSPCHN_ProcessingMode_ePP] = */
		{
			2,	/* Number of nodes in AudysseyV olume */
			{
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eAudysseyVolumePostProc,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*BDSP_AudioProcessing_eBrcm3DSurround,*/
	{
		/*	BDSP_sPpAlgoExecInfo[BDSP_AudioProcessing_eBrcm3DSurround][BDSP_DSPCHN_ProcessingMode_ePP] = */
		{
			2,	/* Number of nodes in AudysseyV olume */
			{
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eBrcm3DSurroundPostProc,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*BDSP_AudioProcessing_eFWMixer,*/
	{
		/*	BDSP_sPpAlgoExecInfo[BDSP_AudioProcessing_eFWMixer][BDSP_DSPCHN_ProcessingMode_ePP] = */
		{
			2,	/* Number of nodes in FWMixer */
			{
				BDSP_AF_P_AlgoId_eMixerFrameSync,
				BDSP_AF_P_AlgoId_eFWMixerPostProc,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
	},

	/*BDSP_AudioProcessing_eAudysseyABX,*/
	{
		/*	BDSP_sPpAlgoExecInfo[BDSP_AudioProcessing_eAudysseyABX][BDSP_DSPCHN_ProcessingMode_ePP] = */
		{
			2,	/* Number of nodes in Mono Downmix */
			{
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eAudysseyABXPostProc,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*BDSP_AudioProcessing_eDdre,*/
	{
		/*	BDSP_sPpAlgoExecInfo[BDSP_AudioProcessing_eDdre][BDSP_DSPCHN_ProcessingMode_ePP] = */
		{
			2,	/* Number of nodes in DDRE */
			{
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eDdrePostProc,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*BDSP_AudioProcessing_eDv258,*/
	{
		/*	BDSP_sPpAlgoExecInfo[BDSP_AudioProcessing_eDv258][BDSP_DSPCHN_ProcessingMode_ePP] = */
		{
			2,	/* Number of nodes in DV258 */
			{
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eDv258PostProc,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*BDSP_AudioProcessing_eSrsCsdTd,*/
	{
		/*	BDSP_sPpAlgoExecInfo[BDSP_AudioProcessing_eSrsCsdTd][BDSP_DSPCHN_ProcessingMode_ePP] = */
		{
			2,	/* Number of nodes in CSD and TD */
			{
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eSrsCircleSurrPostProc,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*BDSP_AudioProcessing_eSrsGeq,*/
	{
		/*	BDSP_sPpAlgoExecInfo[BDSP_AudioProcessing_eSrsGeq][BDSP_DSPCHN_ProcessingMode_ePP] = */
		{
			2,	/* Number of nodes in SRS GEQ */
			{
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eSrsEqualizerPostProc,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*BDSP_AudioProcessing_eCustomDbe,*/
	{
		/*	BDSP_sPpAlgoExecInfo[BDSP_AudioProcessing_eCustomDbe][BDSP_DSPCHN_ProcessingMode_ePP] = */
		{
			2,	/* Number of nodes in CUSTOM DBE */
			{
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eCustomDbePostProc,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*BDSP_AudioProcessing_eCustomAcf,*/
	{
		/*	BDSP_sPpAlgoExecInfo[BDSP_AudioProcessing_eCustomAcf][BDSP_DSPCHN_ProcessingMode_ePP] = */
		{
			2,	/* Number of nodes in CUSTOM ACF */
			{
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eCustomAcfPostProc,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*BDSP_AudioProcessing_eCustomAvlp,*/
	{
		/*	BDSP_sPpAlgoExecInfo[BDSP_AudioProcessing_eCustomAvlp][BDSP_DSPCHN_ProcessingMode_ePP] = */
		{
			2,	/* Number of nodes in CUSTOM AVLP */
			{
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eCustomAvlpPostProc,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*BDSP_CIT_P_ProcessingType_eGenCdbItb,*/
	{
		/*	BDSP_sPpAlgoExecInfo[BDSP_AudioProcessing_eGenRave][BDSP_DSPCHN_ProcessingMode_ePP] = */
		{
			2,	/* Number of nodes in GEN CDB ITB */
			{
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eGenCdbItbPostProc,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*BDSP_AudioProcessing_eBtscEncoder,*/
	{
		/*	BDSP_sPpAlgoExecInfo[BDSP_AudioProcessing_eBtscEncoder][BDSP_DSPCHN_ProcessingMode_ePP] = */
		{
			2,	/* Number of nodes in BTSC Encoder */
			{
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eBtscEncoderPostProc,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*BDSP_AudioProcessing_eSpeexAec*/
	{
		/* BDSP_sPpAlgoExecInfo[BDSP_AudioProcessing_eSpeexAec][BDSP_DSPCHN_ProcessingMode_ePP] = */
		{
			2,
			{
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eSpeexAECPostProc,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
	},
};

/*	Init structure for Post Processing algorithms */
const BDSP_AF_P_sALGO_EXEC_INFO BDSP_sScmAlgoExecInfo[BDSP_ScmType_eMax] =
{
		/*	BDSP_sScmAlgoExecInfo[BDSP_ScmType_eScm1] = */
		{
			1,	/* Number of nodes in Scm1 */ /* TODO: Vijay Why 2 in other PP cases? */
			{
				BDSP_AF_P_AlgoId_eScm1,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
		/*BDSP_ScmType_eScm2*/
		{
			1,	/* Number of nodes in Scm2 */ /* TODO: Vijay Why 2 in other PP cases? */
			{
				BDSP_AF_P_AlgoId_eScm2,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
		/*BDSP_ScmType_eScm3*/
		{
			1,	/* Number of nodes in Scm3 */ /* TODO: Vijay Why 2 in other PP cases? */
			{
				BDSP_AF_P_AlgoId_eScm3,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid,
				BDSP_AF_P_AlgoId_eInvalid
			},
		},
};
const BDSP_AF_P_sNODE_INFO BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eMax] =
{
	/*	this entry will contain all the sizes (in bytes) of the Mpeg Decode algorihtm */
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eMpegDecode] =  */
	{
		BDSP_IMG_MPEG1_DECODE_SIZE,						/*	ui32CodeSize */
		BDSP_IMG_MPEG1_DECODE_TABLES_SIZE,				/*	ui32RomTableSize */
		BDSP_IMG_MPEG1_DECODE_INTER_FRAME_SIZE,			/*	ui32InterFrameBuffSize */
		(1152+BDSP_AF_P_EXTRA_SAMPLES)*4*2,				/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_MpegConfigParams),		/*	ui32UserCfgBuffSize */
		(1152+BDSP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		2,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eStatusBuffType */
		sizeof(BDSP_Raaga_Audio_MpegStreamInfo)			/*  FwStatusBuffSize */

		/*BDSP_AF_P_NodeStatusBuffType_ePresent */
	},

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eAc3Decode] =  */
	{
#ifdef BDSP_MS10_SUPPORT
		BDSP_IMG_DOLBY_MS_DDP_DECODE_SIZE,				/*	ui32CodeSize */
		BDSP_IMG_DOLBY_MS_DDP_DECODE_TABLES_SIZE,		/*	ui32RomTableSize */
		/* Interframe size with Dolby MS 10 Changes */
		BDSP_IMG_DOLBY_MS_DDP_DECODE_INTER_FRAME_SIZE,	/*	ui32InterFrameBuffSize */
#else
		BDSP_IMG_AC3_DECODE_SIZE,						/*	ui32CodeSize */
		BDSP_IMG_AC3_DECODE_TABLES_SIZE,				/*	ui32RomTableSize */
		/* Interframe size with Dolby MS 10 Changes */
		BDSP_IMG_AC3_DECODE_INTER_FRAME_SIZE,			/*	ui32InterFrameBuffSize */
#endif
		(2560+BDSP_AF_P_EXTRA_SAMPLES)*4*8,				/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		82600,											/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_DDPMultiStreamConfigParams),	/*	ui32UserCfgBuffSize		              */
		(2560+BDSP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		8,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_MultiStreamDDPStreamInfo)	/*  FwStatusBuffSize */
	},

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eAacDecode] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eAacHeLpSbrDecode] =  */
	{
		BDSP_IMG_AACHE_DECODE_SIZE,						/*	ui32CodeSize */
		BDSP_IMG_AACHE_DECODE_TABLES_SIZE,				/*	ui32RomTableSize */
		BDSP_IMG_AACHE_DECODE_INTER_FRAME_SIZE,			/*	ui32InterFrameBuffSize */
		(12300+BDSP_AF_P_EXTRA_SAMPLES)*4*6,			/*	ui32InterStageIoBuffSize */
		41984,											/*	ui32InterStageGenericBuffSize */
		81920,											/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_AacheConfigParams),		/*	ui32UserCfgBuffSize		 */
		(12300+BDSP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		6,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_AacStreamInfo)			/*  FwStatusBuffSize */
	},

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eDdpDecode] =  */
	{
#ifdef BDSP_MS10_SUPPORT
		BDSP_IMG_DOLBY_MS_DDP_DECODE_SIZE,				/*	ui32CodeSize */
		BDSP_IMG_DOLBY_MS_DDP_DECODE_TABLES_SIZE,		/*	ui32RomTableSize */
		/* Interframe size with Dolby MS 10 Changes */
		BDSP_IMG_DOLBY_MS_DDP_DECODE_INTER_FRAME_SIZE,	/*	ui32InterFrameBuffSize */
#else
		BDSP_IMG_DDP_DECODE_SIZE,						/*	ui32CodeSize */
		BDSP_IMG_DDP_DECODE_TABLES_SIZE,				/*	ui32RomTableSize */
		/* Interframe size with Dolby MS 10 Changes */
		BDSP_IMG_DDP_DECODE_INTER_FRAME_SIZE,			/*	ui32InterFrameBuffSize */
#endif
		(2560+BDSP_AF_P_EXTRA_SAMPLES)*4*8,										/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		82600,											/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_DDPMultiStreamConfigParams),	/*	ui32UserCfgBuffSize	 */
		(2560+BDSP_AF_P_EXTRA_SAMPLES)*4,											/*	ui32MaxSizePerChan */
		8,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_MultiStreamDDPStreamInfo)	/*  FwStatusBuffSize */
	},

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eDdLosslessDecode] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eLpcmCustomDecode] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eBdLpcmDecode] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eDvdLpcmDecode] =  */
	{
		BDSP_IMG_LPCM_DECODE_CODE_SIZE,					/*	ui32CodeSize */
		BDSP_IMG_LPCM_DECODE_TABLES_SIZE,				/*	ui32RomTableSize */
		BDSP_IMG_LPCM_DECODE_INTER_FRAME_SIZE,			/*	ui32InterFrameBuffSize */
		(5000+BDSP_AF_P_EXTRA_SAMPLES)*4*8,				/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		100,											/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_LpcmUserConfig),		/*	ui32UserCfgBuffSize		 */
		(5000+BDSP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		8,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_LpcmStreamInfo)			/*  FwStatusBuffSize */
	},

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eHdDvdLpcmDecode] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eMpegMcDecode] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eWmaStdDecode] =  */
	{
		BDSP_IMG_WMA_DECODE_SIZE,						/*	ui32CodeSize */
		BDSP_IMG_WMA_DECODE_TABLES_SIZE,				/*	ui32RomTableSize */
		BDSP_IMG_WMA_DECODE_INTER_FRAME_SIZE,			/*	ui32InterFrameBuffSize */
		(4096+BDSP_AF_P_EXTRA_SAMPLES)*4*2,				/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		7000,											/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_WmaConfigParams),		/*	ui32UserCfgBuffSize		 */
		(4096+BDSP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		2,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_WmaStreamInfo)			/*  FwStatusBuffSize */
	},

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eWmaProStdDecode] =  */
	 {
        BDSP_IMG_WMA_PRO_DECODE_SIZE,                  	/*  ui32CodeSize */
        BDSP_IMG_WMA_PRO_DECODE_TABLES_SIZE,            /*  ui32RomTableSize */
        BDSP_IMG_WMA_PRO_DECODE_INTER_FRAME_SIZE,       /*  ui32InterFrameBuffSize */
        (4500+BDSP_AF_P_EXTRA_SAMPLES)*4*6,                /*  ui32InterStageIoBuffSize */
        160000*2,                                          /*  ui32InterStageGenericBuffSize */
        250000,                                          /*  ui32ScratchBuffSize */
        sizeof(BDSP_Raaga_Audio_WmaProConfigParams),    /*  ui32UserCfgBuffSize */
        (4500+BDSP_AF_P_EXTRA_SAMPLES)*4,		        /*  ui32MaxSizePerChan */
        6,                                              /*  ui32MaxNumChansSupported */
        BDSP_AF_P_InterFrameBuffType_ePresent,			/*  eInterFrameBuffType */
        BDSP_AF_P_FwStatus_ePresent,                    /*  eFwStatusBuffType */
        sizeof(BDSP_Raaga_Audio_WmaProStreamInfo)		/*  FwStatusBuffSize */
     },

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eMlpDecode] =  */
	{
		BDSP_IMG_TRUEHD_DECODE_SIZE,						/*	ui32CodeSize */
		BDSP_IMG_TRUEHD_DECODE_TABLES_SIZE,				/*	ui32RomTableSize */
		BDSP_IMG_TRUEHD_DECODE_INTER_FRAME_SIZE,			/*	ui32InterFrameBuffSize */
        (4500+BDSP_AF_P_EXTRA_SAMPLES)*4*6,             /*  ui32InterStageIoBuffSize */
        20000,											/*  ui32InterStageGenericBuffSize */
        100,											/*  ui32ScratchBuffSize */
        sizeof(BDSP_Raaga_Audio_MlpConfigParams),		/*  ui32UserCfgBuffSize */
        (4500+BDSP_AF_P_EXTRA_SAMPLES)*4,               /*  ui32MaxSizePerChan */
        6,                                              /*  ui32MaxNumChansSupported */
        BDSP_AF_P_InterFrameBuffType_ePresent,			/*  eInterFrameBuffType */
        BDSP_AF_P_FwStatus_ePresent,                    /*  eFwStatusBuffType */
        sizeof(BDSP_Raaga_Audio_MlpStreamInfo)		/*  FwStatusBuffSize */
	},

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eDdp71Decode] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eDtsDecode] =  */
	{
		BDSP_IMG_DTSHD_DECODE_SIZE,						/*	ui32CodeSize */
		BDSP_IMG_DTSHD_DECODE_TABLES_SIZE,				/*	ui32RomTableSize */
		BDSP_IMG_DTSHD_DECODE_INTER_FRAME_SIZE,			/*	ui32InterFrameBuffSize */
        (4500+BDSP_AF_P_EXTRA_SAMPLES)*4*6,             /*  ui32InterStageIoBuffSize */
        20000,											/*  ui32InterStageGenericBuffSize */
        100,											/*  ui32ScratchBuffSize */
        sizeof(BDSP_Raaga_Audio_DtsHdConfigParams),		/*  ui32UserCfgBuffSize */
        (4500+BDSP_AF_P_EXTRA_SAMPLES)*4,               /*  ui32MaxSizePerChan */
        6,                                              /*  ui32MaxNumChansSupported */
        BDSP_AF_P_InterFrameBuffType_ePresent,			/*  eInterFrameBuffType */
        BDSP_AF_P_FwStatus_ePresent,                    /*  eFwStatusBuffType */
        sizeof(BDSP_Raaga_Audio_DtsHdStreamInfo)		/*  FwStatusBuffSize */
	},

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eDtsLbrDecode] =  */
	{
		BDSP_IMG_DTS_EXPRESS_DECODE_SIZE,                              /*      ui32CodeSize */
		BDSP_IMG_DTS_EXPRESS_DECODE_TABLES_SIZE,                       /*      ui32RomTableSize */
		BDSP_IMG_DTS_EXPRESS_DECODE_INTER_FRAME_SIZE,                  /*      ui32InterFrameBuffSize */
		(2048+BDSP_AF_P_EXTRA_SAMPLES)*4*8,                      /*      ui32InterStageIoBuffSize */
		54000,                                                   /*      ui32InterStageGenericBuffSize */
		716,                                                     /*      ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_DtslbrConfigParams),             /*      ui32UserCfgBuffSize */
		(2048+BDSP_AF_P_EXTRA_SAMPLES)*4,                        /*      ui32MaxSizePerChan */
		8,                                                       /*      ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,                   /*      eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,                             /*      eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_DtslbrStreamInfo)                /*      FwStatusBuffSize */
	},

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eDtsHdDecode] =  */
	{
		BDSP_IMG_DTSHD_DECODE_SIZE,						/*	ui32CodeSize */
		BDSP_IMG_DTSHD_DECODE_TABLES_SIZE,				/*	ui32RomTableSize */
		BDSP_IMG_DTSHD_DECODE_INTER_FRAME_SIZE,			/*	ui32InterFrameBuffSize */
		(2048+BDSP_AF_P_EXTRA_SAMPLES)*4*6,				/*	ui32InterStageIoBuffSize */
		54000,											/*	ui32InterStageGenericBuffSize */
		51200,											/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_DtsHdConfigParams),		/*	ui32UserCfgBuffSize */
		(2048+BDSP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		6,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_DtsHdStreamInfo)		/*  FwStatusBuffSize */
	},

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_ePcmWavDecode] =  */
	{
		BDSP_IMG_PCMWAV_DECODE_SIZE,					/*	ui32CodeSize */
		BDSP_IMG_PCMWAV_DECODE_TABLES_SIZE,				/*	ui32RomTableSize */
		BDSP_IMG_PCMWAV_DECODE_INTER_FRAME_SIZE,		/*	ui32InterFrameBuffSize */
		(4096+BDSP_AF_P_EXTRA_SAMPLES)*4*8,				/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		1024,											/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_PcmWavConfigParams),	/*	ui32UserCfgBuffSize */
		(4096+BDSP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		6,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_PcmWavStreamInfo)		/*  FwStatusBuffSize */
	},
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eAmrDecode] =  */
	{
		BDSP_IMG_AMR_DECODE_SIZE,						/*	ui32CodeSize */
		BDSP_IMG_AMR_DECODE_TABLES_SIZE,				/*	ui32RomTableSize */
		BDSP_IMG_AMR_DECODE_INTER_FRAME_SIZE,			/*	ui32InterFrameBuffSize */
		(160+BDSP_AF_P_EXTRA_SAMPLES)*4*6,				/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_AmrConfigParams),		/*	ui32UserCfgBuffSize */
		(160+BDSP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		6,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_AmrStreamInfo)			/*  FwStatusBuffSize */
	},

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eDraDecode] =  */
	{
		BDSP_IMG_DRA_DECODE_SIZE,						/*	ui32CodeSize */
		BDSP_IMG_DRA_DECODE_TABLES_SIZE,				/*	ui32RomTableSize */
		BDSP_IMG_DRA_DECODE_INTER_FRAME_SIZE,			/*	ui32InterFrameBuffSize */
		(1024+BDSP_AF_P_EXTRA_SAMPLES)*4*6,				/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		38912,											/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_DraConfigParams),		/*	ui32UserCfgBuffSize */
		(1024+BDSP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		6,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_DraStreamInfo)			/*  FwStatusBuffSize */
	},
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eRealAudioLbrDecode] =  */
	{
		BDSP_IMG_RALBR_DECODE_SIZE,						/*	ui32CodeSize */
		BDSP_IMG_RALBR_DECODE_TABLES_SIZE,				/*	ui32RomTableSize */
		BDSP_IMG_RALBR_DECODE_INTER_FRAME_SIZE,			/*	ui32InterFrameBuffSize */
		(1024+BDSP_AF_P_EXTRA_SAMPLES)*4*6,				/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		100,											/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_RalbrConfigParams),		/*	ui32UserCfgBuffSize */
		(1024+BDSP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		6,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_RalbrStreamInfo)		/*  FwStatusBuffSize */
	},
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eDolbyPulseDecode] =  */
	{
		BDSP_IMG_DOLBY_PULSE_DECODE_SIZE,				/*	ui32CodeSize */
		BDSP_IMG_DOLBY_PULSE_DECODE_TABLES_SIZE,		/*	ui32RomTableSize */
		BDSP_IMG_DOLBY_PULSE_DECODE_INTER_FRAME_SIZE,	/*	ui32InterFrameBuffSize */
		(3072+BDSP_AF_P_EXTRA_SAMPLES)*4*8,				/*	ui32InterStageIoBuffSize */
		80240,											/*	ui32InterStageGenericBuffSize */
		185088,											/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_DolbyPulseUserConfig),	/*	ui32UserCfgBuffSize */
		(3072+BDSP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		8,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_DolbyPulseStreamInfo)	/*  FwStatusBuffSize */
	},
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eMs10DdpDecode] =  */
	{
		BDSP_IMG_DOLBY_MS_DDP_DECODE_SIZE,				/*	ui32CodeSize */
		BDSP_IMG_DOLBY_MS_DDP_DECODE_TABLES_SIZE,		/*	ui32RomTableSize */
		BDSP_IMG_DOLBY_MS_DDP_DECODE_INTER_FRAME_SIZE,	/*	ui32InterFrameBuffSize */
		(2560+BDSP_AF_P_EXTRA_SAMPLES)*4*8,				/*	ui32InterStageIoBuffSize */
		87000,											/*	ui32InterStageGenericBuffSize */
		6584,											/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_DDPMultiStreamConfigParams),/*	ui32UserCfgBuffSize */
		(2560+BDSP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		8,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_MultiStreamDDPStreamInfo)	/*  FwStatusBuffSize */
	},
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eAdpcmDecode] =  */
	{
		BDSP_IMG_ADPCM_DECODE_SIZE,						/*	ui32CodeSize */
		BDSP_IMG_ADPCM_DECODE_TABLES_SIZE,				/*	ui32RomTableSize */
		BDSP_IMG_ADPCM_DECODE_INTER_FRAME_SIZE,			/*	ui32InterFrameBuffSize */
		(2048+BDSP_AF_P_EXTRA_SAMPLES)*4*2,				/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_AdpcmConfigParams),		/*	ui32UserCfgBuffSize */
		(2048+BDSP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		2,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_AdpcmStreamInfo)		/*  FwStatusBuffSize */
	},
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eG711G726Decode] =  */
	{
		BDSP_IMG_G711_G726_DECODE_SIZE,					/*	ui32CodeSize */
		BDSP_IMG_G711_G726_DECODE_TABLES_SIZE,			/*	ui32RomTableSize */
		BDSP_IMG_G711_G726_DECODE_INTER_FRAME_SIZE,		/*	ui32InterFrameBuffSize */
		(568+BDSP_AF_P_EXTRA_SAMPLES)*4*2,				/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_G726ConfigParams),		/*	ui32UserCfgBuffSize */
		(568+BDSP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		2,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_G726StreamInfo)		    /*  FwStatusBuffSize */
	},
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eG729Decode] =  */
	{
		BDSP_IMG_G729_DECODE_SIZE,					    /*	ui32CodeSize */
		BDSP_IMG_G729_DECODE_TABLES_SIZE,			    /*	ui32RomTableSize */
		BDSP_IMG_G729_DECODE_INTER_FRAME_SIZE,		    /*	ui32InterFrameBuffSize */
		(256+BDSP_AF_P_EXTRA_SAMPLES)*4*2,				/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		12604,											/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_G729DecConfigParams),	/*	ui32UserCfgBuffSize */
		(256+BDSP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		2,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_G729DecStreamInfo)		/*  FwStatusBuffSize */
	},
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eVorbisDecode] =  */
	{
		BDSP_IMG_VORBIS_DECODE_SIZE,					/*	ui32CodeSize */
		BDSP_IMG_VORBIS_DECODE_TABLES_SIZE,			    /*	ui32RomTableSize */
		BDSP_IMG_VORBIS_DECODE_INTER_FRAME_SIZE,		    /*	ui32InterFrameBuffSize */
		(2048+BDSP_AF_P_EXTRA_SAMPLES)*4*8,				/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		73728,											/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_VorbisDecConfigParams),	/*	ui32UserCfgBuffSize */
		(2048+BDSP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		8,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_VorbisDecStreamInfo)		/*  FwStatusBuffSize */
	},

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eG723_1Decode] =  */
	{
		BDSP_IMG_G723_1_DECODE_SIZE,					/*	ui32CodeSize */
		BDSP_IMG_G723_1_DECODE_TABLES_SIZE,			    /*	ui32RomTableSize */
		BDSP_IMG_G723_1_DECODE_INTER_FRAME_SIZE,		/*	ui32InterFrameBuffSize */
		(2048+BDSP_AF_P_EXTRA_SAMPLES)*4*8,				/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		73728,											/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_G723_1DEC_ConfigParams),	/*	ui32UserCfgBuffSize */
		(2048+BDSP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		8,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_G723_1_StreamInfo)	    /*  FwStatusBuffSize */
	},

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eFlacDecode] =  */
	{
		BDSP_IMG_FLAC_DECODE_SIZE,						/*	ui32CodeSize */
		BDSP_IMG_FLAC_DECODE_TABLES_SIZE,			    /*	ui32RomTableSize */
		BDSP_IMG_FLAC_DECODE_INTER_FRAME_SIZE,		    /*	ui32InterFrameBuffSize */
		(64*1024+BDSP_AF_P_EXTRA_SAMPLES)*4*8,			/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		(64*1024+BDSP_AF_P_EXTRA_SAMPLES)*4*8,			/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_FlacDecConfigParams),	/*	ui32UserCfgBuffSize */
		(64*1024+BDSP_AF_P_EXTRA_SAMPLES)*4,			/*	ui32MaxSizePerChan */
		8,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_FlacDecStreamInfo)		/*  FwStatusBuffSize */
	},

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eMacDecode] =  */
	{
		BDSP_IMG_MAC_DECODE_SIZE,						/*	ui32CodeSize */
		BDSP_IMG_MAC_DECODE_TABLES_SIZE,			    /*	ui32RomTableSize */
		BDSP_IMG_MAC_DECODE_INTER_FRAME_SIZE,		    /*	ui32InterFrameBuffSize */
		(2048+BDSP_AF_P_EXTRA_SAMPLES)*4*8,				/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		73728,											/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_MacDecConfigParams),	/*	ui32UserCfgBuffSize */
		(2048+BDSP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		8,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_MacDecStreamInfo)		/*  FwStatusBuffSize */
	},
	
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eAmrWbDecode] =  */
	{
		BDSP_IMG_AMRWB_DECODE_SIZE,						/*	ui32CodeSize */
		BDSP_IMG_AMRWB_DECODE_TABLES_SIZE,				/*	ui32RomTableSize */
		BDSP_IMG_AMRWB_DECODE_INTER_FRAME_SIZE,			/*	ui32InterFrameBuffSize */
		(160+BDSP_AF_P_EXTRA_SAMPLES)*4*6,				/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_AmrwbdecConfigParams),		/*	ui32UserCfgBuffSize */
		(160+BDSP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		6,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_AmrWbStreamInfo)			/*  FwStatusBuffSize */
	},

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eiLBCDecode] =  */
	{
		BDSP_IMG_ILBC_DECODE_SIZE,						/*	ui32CodeSize */
		BDSP_IMG_ILBC_DECODE_TABLES_SIZE,				/*	ui32RomTableSize */
		BDSP_IMG_ILBC_DECODE_INTER_FRAME_SIZE,			/*	ui32InterFrameBuffSize */
		(240+BDSP_AF_P_EXTRA_SAMPLES)*4*6,				/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_iLBCdecConfigParams),		/*	ui32UserCfgBuffSize */
		(240+BDSP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		6,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_iLBCStreamInfo)			/*  FwStatusBuffSize */
	},
			/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eiSACDecode] =  */
	{
		BDSP_IMG_ISAC_DECODE_SIZE,						/*	ui32CodeSize */
		BDSP_IMG_ISAC_DECODE_TABLES_SIZE,				/*	ui32RomTableSize */
		BDSP_IMG_ISAC_DECODE_INTER_FRAME_SIZE,			/*	ui32InterFrameBuffSize */
		(240+BDSP_AF_P_EXTRA_SAMPLES)*4*6,				/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_iSACdecConfigParams),		/*	ui32UserCfgBuffSize */
		(240+BDSP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		6,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_iSACStreamInfo)			/*  FwStatusBuffSize */
	},


	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eEndOfAudioDecodeAlgos] =  */
	{ 0,0,0,0,0,0,0,0,0,0,0,0 },
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eRealVideo9Decode] =  */
	{ 0,0,0,0,0,0,0,0,0,0,0,0 },
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eVP6Decode] =  */
	{
		BDSP_IMG_VP6_DECODE_SIZE,						/*	ui32CodeSize */
		BDSP_IMG_VP6_DECODE_TABLES_SIZE,				/*	ui32RomTableSize */
		BDSP_IMG_VP6_DECODE_INTER_FRAME_SIZE,			/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		20000,												/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		0,												/*	ui32UserCfgBuffSize */
		0,												/*	ui32MaxSizePerChan */
		0,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		sizeof(BDSP_Raaga_P_Vp6StreamInfo)				/*  FwStatusBuffSize */
	},
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eEndOfDecodeAlgos] =  */
	{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	Algo inits for the frame syncs of the decoder algorithms */
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eMpegFrameSync] =  */
	{
		BDSP_IMG_MPEG1_IDS_SIZE,						/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		BDSP_IMG_MPEG1_IDS_INTER_FRAME_SIZE,			/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_FrameSyncTsmConfigParams),	/*	ui32UserCfgBuffSize */
		0,												/*	ui32MaxSizePerChan */
		2,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_IdsTsmInfo)				/*  FwStatusBuffSize */
	},

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eMpegMcFrameSync] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eAdtsFrameSync] =  */
	{
		BDSP_IMG_AACHEADTS_IDS_SIZE,					/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		BDSP_IMG_AACHEADTS_IDS_INTER_FRAME_SIZE,		/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_FrameSyncTsmConfigParams),	/*	ui32UserCfgBuffSize */
		0,												/*	ui32MaxSizePerChan */
		2,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_IdsTsmInfo)				/*  FwStatusBuffSize */
	},

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eLoasFrameSync] =  */
	{
		BDSP_IMG_AACHELOAS_IDS_SIZE,					/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		BDSP_IMG_AACHELOAS_IDS_INTER_FRAME_SIZE,		/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_FrameSyncTsmConfigParams),	/*	ui32UserCfgBuffSize */
		0,												/*	ui32MaxSizePerChan */
		2,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_IdsTsmInfo)				/*  FwStatusBuffSize */
	},

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eWmaStdFrameSync] =  */
	{
		BDSP_IMG_WMA_IDS_SIZE,							/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		BDSP_IMG_WMA_IDS_INTER_FRAME_SIZE,				/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_FrameSyncTsmConfigParams),	/*	ui32UserCfgBuffSize */
		0,												/*	ui32MaxSizePerChan */
		2,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_IdsTsmInfo)				/*  FwStatusBuffSize */
	},

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eWmaProFrameSync] =  */
	{
		BDSP_IMG_WMA_PRO_IDS_SIZE,							/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		BDSP_IMG_WMA_PRO_IDS_INTER_FRAME_SIZE,				/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_FrameSyncTsmConfigParams),	/*	ui32UserCfgBuffSize */
		0,												/*	ui32MaxSizePerChan */
		2,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_IdsTsmInfo)				/*  FwStatusBuffSize */
	},

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eAc3FrameSync] =  */
	{
		BDSP_IMG_AC3_IDS_SIZE,							/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		BDSP_IMG_AC3_IDS_INTER_FRAME_SIZE,				/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_FrameSyncTsmConfigParams),	/*	ui32UserCfgBuffSize */
		0,												/*	ui32MaxSizePerChan */
		2,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_IdsTsmInfo)				/*  FwStatusBuffSize */
	},

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eDdpFrameSync] =  */
	{
	    BDSP_IMG_DDP_IDS_SIZE,							/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		BDSP_IMG_DDP_IDS_INTER_FRAME_SIZE,				/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_FrameSyncTsmConfigParams),	/*	ui32UserCfgBuffSize */
		0,												/*	ui32MaxSizePerChan */
		2,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_IdsTsmInfo)				/*  FwStatusBuffSize */
    },
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eDdp71FrameSync] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eDtsFrameSync] =  */
	{
	    BDSP_IMG_DTS_IDS_SIZE,							/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		BDSP_IMG_DTS_IDS_INTER_FRAME_SIZE,				/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_FrameSyncTsmConfigParams),	/*	ui32UserCfgBuffSize */
		0,												/*	ui32MaxSizePerChan */
		0,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_IdsTsmInfo)				/*  FwStatusBuffSize */
    },

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eDtsLbrFrameSync] =  */
	{ 
	      BDSP_IMG_DTS_EXPRESS_IDS_DECODE_SIZE,				/*	ui32CodeSize */
		  0,											/*	ui32RomTableSize */
		  0,											/*	ui32InterFrameBuffSize */
		  0,											/*	ui32InterStageIoBuffSize */
		  20000,										/*	ui32InterStageGenericBuffSize */
		  0,											/*	ui32ScratchBuffSize */
		  sizeof(BDSP_Raaga_Audio_FrameSyncTsmConfigParams),	/*	ui32UserCfgBuffSize */
		  0,											/*	ui32MaxSizePerChan */
		  0,											/*	ui32MaxNumChansSupported */
		  BDSP_AF_P_InterFrameBuffType_eAbsent,			/*	eInterFrameBuffType */
		  BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		  sizeof(BDSP_Raaga_Audio_IdsTsmInfo)			/*  FwStatusBuffSize */
     },

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eDtsHdFrameSync] =  */
	{
	    BDSP_IMG_DTSHD_IDS_SIZE,						/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		BDSP_IMG_DTSHD_IDS_INTER_FRAME_SIZE,			/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_FrameSyncTsmConfigParams),	/*	ui32UserCfgBuffSize */
		0,												/*	ui32MaxSizePerChan */
		0,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_IdsTsmInfo)				/*  FwStatusBuffSize */
    },

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eDtsHdFrameSync_1] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eDtsHdHdDvdFrameSync] =  */
		{
		    BDSP_IMG_DTSHD_IDS_SIZE,						/*	ui32CodeSize */
			0,												/*	ui32RomTableSize */
			BDSP_IMG_DTSHD_IDS_INTER_FRAME_SIZE ,			/*	ui32InterFrameBuffSize */
			0,												/*	ui32InterStageIoBuffSize */
			20000 ,											/*	ui32InterStageGenericBuffSize */
			0,												/*	ui32ScratchBuffSize */
			sizeof(BDSP_Raaga_Audio_FrameSyncTsmConfigParams),	/*	ui32UserCfgBuffSize */
			0,												/*	ui32MaxSizePerChan */
			0,												/*	ui32MaxNumChansSupported */
			BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
			BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
			sizeof(BDSP_Raaga_Audio_IdsTsmInfo)				/*  FwStatusBuffSize */
		},

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eDdLosslessFrameSync] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eMlpFrameSync] =  */
		{
		    BDSP_IMG_MLP_IDS_SIZE,						/*	ui32CodeSize */
			0,												/*	ui32RomTableSize */
			BDSP_IMG_MLP_IDS_INTER_FRAME_SIZE ,			/*	ui32InterFrameBuffSize */
			0,												/*	ui32InterStageIoBuffSize */
			20000 ,											/*	ui32InterStageGenericBuffSize */
			0,												/*	ui32ScratchBuffSize */
			sizeof(BDSP_Raaga_Audio_FrameSyncTsmConfigParams),	/*	ui32UserCfgBuffSize */
			0,												/*	ui32MaxSizePerChan */
			0,												/*	ui32MaxNumChansSupported */
			BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
			BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
			sizeof(BDSP_Raaga_Audio_IdsTsmInfo)				/*  FwStatusBuffSize */
		},


	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eMlpHdDvdFrameSync] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_ePesFrameSync] =  */
	{
	    BDSP_IMG_PCM_IDS_SIZE,							/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		BDSP_IMG_PCM_IDS_INTER_FRAME_SIZE,				/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_FrameSyncTsmConfigParams),	/*	ui32UserCfgBuffSize */
		0,												/*	ui32MaxSizePerChan */
		0,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_IdsTsmInfo)				/*  FwStatusBuffSize */
    },

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eBdLpcmFrameSync] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eHdDvdLpcmFrameSync] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eDvdLpcmFrameSync] =  */
	{
	    BDSP_IMG_DVDLPCM_IDS_SIZE,						/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		BDSP_IMG_DVDLPCM_IDS_INTER_FRAME_SIZE,			/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_FrameSyncTsmConfigParams),	/*	ui32UserCfgBuffSize */
		0,												/*	ui32MaxSizePerChan */
		0,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_IdsTsmInfo)				/*  FwStatusBuffSize */
    },

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eDvdLpcmFrameSync_1] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },


	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_ePcmWavFrameSync] =  */
	{
		BDSP_IMG_PCMWAV_IDS_SIZE,						/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		BDSP_IMG_PCMWAV_IDS_INTER_FRAME_SIZE,			/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_FrameSyncTsmConfigParams),	/*	ui32UserCfgBuffSize */
		0,												/*	ui32MaxSizePerChan */
		0,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_IdsTsmInfo)				/*  FwStatusBuffSize */
	},

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eAmrFrameSync] =  */
	{
		BDSP_IMG_AMR_IDS_SIZE,							/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		BDSP_IMG_AMR_IDS_INTER_FRAME_SIZE ,				/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		20000 ,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_FrameSyncTsmConfigParams),	/*	ui32UserCfgBuffSize */
		0,												/*	ui32MaxSizePerChan */
		0,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_IdsTsmInfo)				/*  FwStatusBuffSize */
	},
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eDraFrameSync] =  */
	{
		BDSP_IMG_DRA_IDS_SIZE,							/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		BDSP_IMG_DRA_IDS_INTER_FRAME_SIZE ,				/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		20000 ,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_FrameSyncTsmConfigParams),	/*	ui32UserCfgBuffSize */
		0,												/*	ui32MaxSizePerChan */
		0,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_IdsTsmInfo)				/*  FwStatusBuffSize */
	},
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eRealAudioLbrFrameSync] =  */
	{
		BDSP_IMG_RALBR_IDS_SIZE,						/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		BDSP_IMG_RALBR_IDS_INTER_FRAME_SIZE ,			/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		20000 ,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_FrameSyncTsmConfigParams),	/*	ui32UserCfgBuffSize */
		0,												/*	ui32MaxSizePerChan */
		0,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_IdsTsmInfo)				/*  FwStatusBuffSize */
	},
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eMs10DdpFrameSync] =  */
	{
		BDSP_IMG_DDP_IDS_SIZE,							/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		BDSP_IMG_DDP_IDS_INTER_FRAME_SIZE,				/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_FrameSyncTsmConfigParams),	/*	ui32UserCfgBuffSize */
		0,												/*	ui32MaxSizePerChan */
		0,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_IdsTsmInfo)				/*  FwStatusBuffSize */
	},
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eAdpcmFrameSync] =  */
	{
		BDSP_IMG_ADPCM_IDS_SIZE,						/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		BDSP_IMG_ADPCM_IDS_INTER_FRAME_SIZE ,			/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		20000 ,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_FrameSyncTsmConfigParams),	/*	ui32UserCfgBuffSize */
		0,												/*	ui32MaxSizePerChan */
		0,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_IdsTsmInfo)				/*  FwStatusBuffSize */
	},
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eG711G726FrameSync] =  */
	{
		BDSP_IMG_G711_G726_IDS_SIZE,					/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		BDSP_IMG_G711_G726_IDS_INTER_FRAME_SIZE ,		/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		20000 ,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_FrameSyncTsmConfigParams),	/*	ui32UserCfgBuffSize */
		0,												/*	ui32MaxSizePerChan */
		0,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_IdsTsmInfo)				/*  FwStatusBuffSize */
	},
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eG729FrameSync] =  */
	{
		BDSP_IMG_G729_IDS_SIZE,						    /*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		BDSP_IMG_G729_IDS_INTER_FRAME_SIZE ,			/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		20000 ,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_FrameSyncTsmConfigParams),	/*	ui32UserCfgBuffSize */
		0,												/*	ui32MaxSizePerChan */
		0,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_IdsTsmInfo)				/*  FwStatusBuffSize */
	},
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eVorbisFrameSync] =  */
	{
		BDSP_IMG_VORBIS_IDS_SIZE,						    /*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		BDSP_IMG_VORBIS_IDS_INTER_FRAME_SIZE ,			/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		20000 ,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_FrameSyncTsmConfigParams),	/*	ui32UserCfgBuffSize */
		0,												/*	ui32MaxSizePerChan */
		0,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_IdsTsmInfo)				/*  FwStatusBuffSize */
	},
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eG723_1FrameSync] =  */
	{
		BDSP_IMG_G723_1_IDS_SIZE,						/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		BDSP_IMG_G723_1_IDS_INTER_FRAME_SIZE ,			/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		20000 ,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_FrameSyncTsmConfigParams),	/*	ui32UserCfgBuffSize */
		0,												/*	ui32MaxSizePerChan */
		0,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_IdsTsmInfo)				/*  FwStatusBuffSize */
	},

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eFlacFrameSync] =  */
	{
		BDSP_IMG_FLAC_IDS_SIZE,						    /*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		BDSP_IMG_FLAC_IDS_INTER_FRAME_SIZE ,			/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		20000 ,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_FrameSyncTsmConfigParams),	/*	ui32UserCfgBuffSize */
		0,												/*	ui32MaxSizePerChan */
		0,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_IdsTsmInfo)				/*  FwStatusBuffSize */
	},

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eMacFrameSync] =  */
	{
		BDSP_IMG_MAC_IDS_SIZE,						    /*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		BDSP_IMG_MAC_IDS_INTER_FRAME_SIZE ,			/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		20000 ,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_FrameSyncTsmConfigParams),	/*	ui32UserCfgBuffSize */
		0,												/*	ui32MaxSizePerChan */
		0,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_IdsTsmInfo)				/*  FwStatusBuffSize */
	},
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eAmrWbFrameSync] =  */
	{
		BDSP_IMG_AMRWB_IDS_SIZE,							/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		BDSP_IMG_AMRWB_IDS_INTER_FRAME_SIZE ,				/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		20000 ,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_FrameSyncTsmConfigParams),	/*	ui32UserCfgBuffSize */
		0,												/*	ui32MaxSizePerChan */
		0,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_IdsTsmInfo)				/*  FwStatusBuffSize */
	},
	{
		BDSP_IMG_ILBC_IDS_SIZE,							/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		BDSP_IMG_ILBC_IDS_INTER_FRAME_SIZE ,				/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		20000 ,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_FrameSyncTsmConfigParams),	/*	ui32UserCfgBuffSize */
		0,												/*	ui32MaxSizePerChan */
		0,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_IdsTsmInfo)				/*  FwStatusBuffSize */
	},
	
		/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eiSACFrameSync] =  */
	{
		BDSP_IMG_ISAC_IDS_SIZE,							/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		BDSP_IMG_ISAC_IDS_INTER_FRAME_SIZE ,				/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		20000 ,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_FrameSyncTsmConfigParams),	/*	ui32UserCfgBuffSize */
		0,												/*	ui32MaxSizePerChan */
		0,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_IdsTsmInfo)				/*  FwStatusBuffSize */
	},

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eEndOfAudioDecFsAlgos] =  */
	{ 0,0,0,0,0,0,0,0,0,0,0,0 },
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eRealVideo9FrameSync] =  */
	{ 0,0,0,0,0,0,0,0,0,0,0,0 },
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eVP6FrameSync] =  */
	{
		BDSP_IMG_VP6_IDS_SIZE,							/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		BDSP_IMG_VP6_IDS_INTER_FRAME_SIZE ,				/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		20000 ,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_FrameSyncTsmConfigParams),	/*	ui32UserCfgBuffSize */
		0,												/*	ui32MaxSizePerChan */
		0,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_IdsTsmInfo)				/*  FwStatusBuffSize */
	},
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eEndOfDecFsAlgos] =  */
	{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eAc3Encode] =  */
	{ 	44000,											/*	ui32CodeSize */
		12000,											/*	ui32RomTableSize */
		56000,											/*	ui32InterFrameBuffSize */
		(12000 +BDSP_AF_P_EXTRA_SAMPLES)*4*2,			/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		85000,											/*	ui32ScratchBuffSize */
		100,											/*	ui32UserCfgBuffSize */
		12000*4,										/*	ui32MaxSizePerChan */
		2,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	 },

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eMpegL2Encode] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eMpegL3Encode] =  */
	{
		BDSP_IMG_MP3_ENCODE_SIZE,						/*	ui32CodeSize */
		BDSP_IMG_MP3_ENCODE_TABLES_SIZE,				/*	ui32RomTableSize */
		BDSP_IMG_MP3_ENCODE_INTER_FRAME_SIZE,			/*	ui32InterFrameBuffSize */
		(1024+BDSP_AF_P_EXTRA_SAMPLES)*4*2,				/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_Mpeg1L3EncConfigParams),/*	ui32UserCfgBuffSize */
		1024*4,											/*	ui32MaxSizePerChan */
		2,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	 },

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eAacLcEncode] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eAacHeEncode] =  */
	{
		BDSP_IMG_AACHE_ENCODE_SIZE,						/*	ui32CodeSize */
		BDSP_IMG_AACHE_ENCODE_TABLES_SIZE,				/*	ui32RomTableSize */
		BDSP_IMG_AACHE_ENCODE_INTER_FRAME_SIZE,			/*	ui32InterFrameBuffSize */
		(6144 + BDSP_AF_P_EXTRA_SAMPLES)*4*6,			/*	ui32InterStageIoBuffSize */
		16384,											/*	ui32InterStageGenericBuffSize */
		2000 ,											/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_AacheEncConfigParams),	/*	ui32UserCfgBuffSize */
		(6144+BDSP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		6,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eDtsEncode] =  */
	{
		BDSP_IMG_DTS_ENCODE_SIZE,						/* ui32CodeSize */
		BDSP_IMG_DTS_ENCODE_TABLES_SIZE,				/* ui32RomTableSize */
		BDSP_IMG_DTS_ENCODE_INTER_FRAME_SIZE,			/* ui32InterFrameBuffSize */
		(1024+BDSP_AF_P_EXTRA_SAMPLES)*4*1,				/* ui32InterStageIoBuffSize */
		20000,											/* ui32InterStageGenericBuffSize */
		0,												/* ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_DtsBroadcastEncConfigParams),/* ui32UserCfgBuffSize */
		(1024+BDSP_AF_P_EXTRA_SAMPLES)*4,				/* ui32MaxSizePerChan */
		1,												/* ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/* eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/* eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_DTSBroadcastEncoderStreamInfo)	/* FwStatusBuffSize */
	},

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eDtsBroadcastEncode] =  */
	{
		BDSP_IMG_DTS_ENCODE_SIZE,						/* ui32CodeSize */
		BDSP_IMG_DTS_ENCODE_TABLES_SIZE,				/* ui32RomTableSize */
		BDSP_IMG_DTS_ENCODE_INTER_FRAME_SIZE,			/* ui32InterFrameBuffSize */
		(1024+BDSP_AF_P_EXTRA_SAMPLES)*4*1,				/* ui32InterStageIoBuffSize */
		20000,												/* ui32InterStageGenericBuffSize */
		0,												/* ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_DtsBroadcastEncConfigParams),/* ui32UserCfgBuffSize */
		(1024+BDSP_AF_P_EXTRA_SAMPLES)*4,				/* ui32MaxSizePerChan */
		1,												/* ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/* eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/* eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_DTSBroadcastEncoderStreamInfo)	/* FwStatusBuffSize */
	},

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eSbcEncode] =  */
	{
		10240,											/* ui32CodeSize */
		1024,											/* ui32RomTableSize */
		2048,											/* ui32InterFrameBuffSize */
		(37400+BDSP_AF_P_EXTRA_SAMPLES)*1,				/* ui32InterStageIoBuffSize */
		20000,											/* ui32InterStageGenericBuffSize */
		256,											/* ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_SbcEncoderUserConfig),	/* ui32UserCfgBuffSize */
		(37400+BDSP_AF_P_EXTRA_SAMPLES),				/* ui32MaxSizePerChan */
		1,												/* ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/* eInterFrameBuffType */
		BDSP_AF_P_FwStatus_eAbsent,						/* eFwStatusBuffType */
		0												/* FwStatusBuffSize */
	},

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eMs10DDTranscode] =  */
	{
		BDSP_IMG_DD_TRANSCODE_SIZE,						/*	ui32CodeSize */
		BDSP_IMG_DD_TRANSCODE_TABLES_SIZE,				/*	ui32RomTableSize */
		BDSP_IMG_DD_TRANSCODE_INTER_FRAME_SIZE,			/*	ui32InterFrameBuffSize */
		(6144 + BDSP_AF_P_EXTRA_SAMPLES)*4*6,			/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		85000 ,											/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_DDTranscodeConfigParams),/*	ui32UserCfgBuffSize */
		(6144+BDSP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		6,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eG711G726Encode] =  */
	{
		BDSP_IMG_G711_G726_ENCODE_SIZE,					    /* ui32CodeSize */
		BDSP_IMG_G711_G726_ENCODE_TABLES_SIZE,			    /* ui32RomTableSize */
		BDSP_IMG_G711_G726_ENCODE_INTER_FRAME_SIZE,		    /* ui32InterFrameBuffSize */
		(2272+BDSP_AF_P_EXTRA_SAMPLES)*1,				    /* ui32InterStageIoBuffSize */
		20000,											    /* ui32InterStageGenericBuffSize */
		0,											        /* ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_G711_G726EncConfigParams),	/* ui32UserCfgBuffSize */
		(1136+BDSP_AF_P_EXTRA_SAMPLES),				        /* ui32MaxSizePerChan */
		1,												    /* ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			    /* eInterFrameBuffType */
		BDSP_AF_P_FwStatus_eAbsent,						    /* eFwStatusBuffType */
		0												    /* FwStatusBuffSize */
	},

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eG729Encode] =  */
	{
		BDSP_IMG_G729_ENCODE_SIZE,						/* ui32CodeSize */
		BDSP_IMG_G729_ENCODE_TABLES_SIZE,				/* ui32RomTableSize */
		BDSP_IMG_G729_ENCODE_INTER_FRAME_SIZE,			/* ui32InterFrameBuffSize */
		(2048+BDSP_AF_P_EXTRA_SAMPLES)*1,				/* ui32InterStageIoBuffSize */
		20000,											/* ui32InterStageGenericBuffSize */
		0,											    /* ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_G729EncoderUserConfig),	/* ui32UserCfgBuffSize */
		(2048+BDSP_AF_P_EXTRA_SAMPLES),				    /* ui32MaxSizePerChan */
		1,												/* ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/* eInterFrameBuffType */
		BDSP_AF_P_FwStatus_eAbsent,						/* eFwStatusBuffType */
		0												/* FwStatusBuffSize */
	},
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eWmaEncode] =	*/
	{
		BDSP_IMG_WMA_ENCODE_SIZE,						/* ui32CodeSize */
		BDSP_IMG_WMA_ENCODE_TABLES_SIZE,				/* ui32RomTableSize */
		BDSP_IMG_WMA_ENCODE_INTER_FRAME_SIZE,			/* ui32InterFrameBuffSize */
		(12*1024)*2,									/* ui32InterStageIoBuffSize */
		20000,											/* ui32InterStageGenericBuffSize */
		0,												/* ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_WMAEncoderConfigParams), 		/* ui32UserCfgBuffSize */
		(12*1024), 									/* ui32MaxSizePerChan */
		2,												/* ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/* eInterFrameBuffType */
		BDSP_AF_P_FwStatus_eAbsent, 					/* eFwStatusBuffType */
		0												/* FwStatusBuffSize */
	},
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eG723_1Encode] =	*/
	{
		BDSP_IMG_G723_1_ENCODE_SIZE,					/* ui32CodeSize */
		BDSP_IMG_G723_1_ENCODE_TABLES_SIZE,				/* ui32RomTableSize */
		BDSP_IMG_G723_1_ENCODE_INTER_FRAME_SIZE,		/* ui32InterFrameBuffSize */
		4800,									        /* ui32InterStageIoBuffSize */
		2000,											/* ui32InterStageGenericBuffSize */
		0,												/* ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_G723EncoderUserConfig), /* ui32UserCfgBuffSize */
		2400, 									        /* ui32MaxSizePerChan */
		2,												/* ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/* eInterFrameBuffType */
		BDSP_AF_P_FwStatus_eAbsent, 					/* eFwStatusBuffType */
		0												/* FwStatusBuffSize */
	},
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eG722Encode] =  */
	{
		BDSP_IMG_G722_ENCODE_SIZE,						/* ui32CodeSize */
		BDSP_IMG_G722_ENCODE_TABLES_SIZE,				/* ui32RomTableSize */
		BDSP_IMG_G722_ENCODE_INTER_FRAME_SIZE,			/* ui32InterFrameBuffSize */
		(2048+BDSP_AF_P_EXTRA_SAMPLES)*1,				/* ui32InterStageIoBuffSize */
		20000,											/* ui32InterStageGenericBuffSize */
		0,											    /* ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_G722EncConfigParams),	/* ui32UserCfgBuffSize */
		(2048+BDSP_AF_P_EXTRA_SAMPLES),				    /* ui32MaxSizePerChan */
		1,												/* ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/* eInterFrameBuffType */
		BDSP_AF_P_FwStatus_eAbsent,						/* eFwStatusBuffType */
		0												/* FwStatusBuffSize */
	},
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eAmrEncode] =  */
	{
		BDSP_IMG_AMR_ENCODE_SIZE,						/* ui32CodeSize */
		BDSP_IMG_AMR_ENCODE_TABLES_SIZE,				/* ui32RomTableSize */
		BDSP_IMG_AMR_ENCODE_INTER_FRAME_SIZE,			/* ui32InterFrameBuffSize */
		(2048+BDSP_AF_P_EXTRA_SAMPLES)*1,				/* ui32InterStageIoBuffSize */
		20000,											/* ui32InterStageGenericBuffSize */
		0,											    /* ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_AmrEncoderUserConfig),	/* ui32UserCfgBuffSize */
		(2048+BDSP_AF_P_EXTRA_SAMPLES),				    /* ui32MaxSizePerChan */
		1,												/* ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/* eInterFrameBuffType */
		BDSP_AF_P_FwStatus_eAbsent,						/* eFwStatusBuffType */
		0												/* FwStatusBuffSize */
	},
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eAmrwbEncode] =  */
	{
		BDSP_IMG_AMRWB_ENCODE_SIZE,						/* ui32CodeSize */
		BDSP_IMG_AMRWB_ENCODE_TABLES_SIZE,				/* ui32RomTableSize */
		BDSP_IMG_AMRWB_ENCODE_INTER_FRAME_SIZE,			/* ui32InterFrameBuffSize */
		(2048+BDSP_AF_P_EXTRA_SAMPLES)*1,				/* ui32InterStageIoBuffSize */
		20000,											/* ui32InterStageGenericBuffSize */
		0,											    /* ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_AMRWBConfigParams),	/* ui32UserCfgBuffSize */
		(2048+BDSP_AF_P_EXTRA_SAMPLES),				    /* ui32MaxSizePerChan */
		1,												/* ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/* eInterFrameBuffType */
		BDSP_AF_P_FwStatus_eAbsent,						/* eFwStatusBuffType */
		0												/* FwStatusBuffSize */
	},
		/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eiLBCEncode] =  */
	{
		BDSP_IMG_ILBC_ENCODE_SIZE,						/* ui32CodeSize */
		BDSP_IMG_ILBC_ENCODE_TABLES_SIZE,				/* ui32RomTableSize */
		BDSP_IMG_ILBC_ENCODE_INTER_FRAME_SIZE,			/* ui32InterFrameBuffSize */
		(2048+BDSP_AF_P_EXTRA_SAMPLES)*1,				/* ui32InterStageIoBuffSize */
		20000,											/* ui32InterStageGenericBuffSize */
		0,											    /* ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_ILBCConfigParams),	/* ui32UserCfgBuffSize */
		(2048+BDSP_AF_P_EXTRA_SAMPLES),				    /* ui32MaxSizePerChan */
		1,												/* ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/* eInterFrameBuffType */
		BDSP_AF_P_FwStatus_eAbsent,						/* eFwStatusBuffType */
		0												/* FwStatusBuffSize */
	},
			/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eiSACEncode] =  */
	{
		BDSP_IMG_ISAC_ENCODE_SIZE,						/* ui32CodeSize */
		BDSP_IMG_ISAC_ENCODE_TABLES_SIZE,				/* ui32RomTableSize */
		BDSP_IMG_ISAC_ENCODE_INTER_FRAME_SIZE,			/* ui32InterFrameBuffSize */
		(2048+BDSP_AF_P_EXTRA_SAMPLES)*1,				/* ui32InterStageIoBuffSize */
		20000,											/* ui32InterStageGenericBuffSize */
		0,											    /* ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_ISACConfigParams),	/* ui32UserCfgBuffSize */
		(2048+BDSP_AF_P_EXTRA_SAMPLES),				    /* ui32MaxSizePerChan */
		1,												/* ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/* eInterFrameBuffType */
		BDSP_AF_P_FwStatus_eAbsent,						/* eFwStatusBuffType */
		0												/* FwStatusBuffSize */
	},

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eEndOfEncodeAlgos] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BDSP_sNodeInfo[BDSP_VF_P_AlgoId_eH264Encode] =  */
	{
		BDSP_IMG_H264_ENCODE_SIZE,						/* ui32CodeSize */
		BDSP_IMG_H264_ENCODE_TABLES_SIZE,				/* ui32RomTableSize */
		BDSP_IMG_H264_ENCODE_INTER_FRAME_SIZE,			/* ui32InterFrameBuffSize */
		(2048+BDSP_AF_P_EXTRA_SAMPLES)*1,				/* ui32InterStageIoBuffSize */
		1024,											/* ui32InterStageGenericBuffSize */
		0,												/* ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_VideoBH264UserConfig), /* ui32UserCfgBuffSize */
		(2048+BDSP_AF_P_EXTRA_SAMPLES), 				/* ui32MaxSizePerChan */
		1,												/* ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/* eInterFrameBuffType */
		BDSP_AF_P_FwStatus_eAbsent, 					/* eFwStatusBuffType */
		0												/* FwStatusBuffSize */
	},
	/*	BDSP_sNodeInfo[BDSP_VF_P_AlgoId_eVP8Encode] =  */
	{
		BDSP_IMG_VP8_ENCODE_SIZE,						/* ui32CodeSize */
		BDSP_IMG_VP8_ENCODE_TABLES_SIZE,				/* ui32RomTableSize */
		BDSP_IMG_VP8_ENCODE_INTER_FRAME_SIZE,			/* ui32InterFrameBuffSize */
		(2048+BDSP_AF_P_EXTRA_SAMPLES)*1,				/* ui32InterStageIoBuffSize */
		1024,											/* ui32InterStageGenericBuffSize */
		0,												/* ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_VideoBVP8UserConfig), /* ui32UserCfgBuffSize */
		(2048+BDSP_AF_P_EXTRA_SAMPLES), 				/* ui32MaxSizePerChan */
		1,												/* ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/* eInterFrameBuffType */
		BDSP_AF_P_FwStatus_eAbsent, 					/* eFwStatusBuffType */
		0												/* FwStatusBuffSize */
	},
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eEndOfEncodeAlgos] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eAc3EncFrameSync] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eMpegL3EncFrameSync] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eMpegL2EncFrameSync] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eAacLcEncFrameSync] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eAacHeEncFrameSync] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eDtsEncFrameSync] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eWmaEncFrameSync] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eEndOfEncFsAlgos] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_ePassThru] =  */
	{
		BDSP_IMG_CDB_PASSTHRU_CODE_SIZE,				/*	ui32CodeSize */
		BDSP_IMG_CDB_PASSTHRU_TABLES_SIZE,				/*	ui32RomTableSize */
		BDSP_IMG_CDB_PASSTHRU_INTER_FRAME_SIZE,			/*	ui32InterFrameBuffSize */
		(4096+BDSP_AF_P_EXTRA_SAMPLES)*4*6,				/*	ui32InterStageIoBuffSize */
		32768,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_PassthruConfigParams),	/*	ui32UserCfgBuffSize */
		(4096+BDSP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		6,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		400												/*  FwStatusBuffSize */
	},

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eMLPPassThru] =  */
	{
		BDSP_IMG_MLP_PASSTHROUGH_CODE_SIZE,				/*	ui32CodeSize */
		BDSP_IMG_MLP_PASSTHROUGH_TABLES_SIZE,			/*	ui32RomTableSize */
		BDSP_IMG_MLP_PASSTHROUGH_INTER_FRAME_SIZE,		/*	ui32InterFrameBuffSize */
		(4096+BDSP_AF_P_EXTRA_SAMPLES)*4*6,				/*	ui32InterStageIoBuffSize */
		32768,											/*	ui32InterStageGenericBuffSize */
		1024,											/*	ui32ScratchBuffSize */
		0,												/*	ui32UserCfgBuffSize */
		(4096+BDSP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		6,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eEndOfAuxAlgos] =  */
	{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eSrsTruSurroundPostProc] =  */
	{
		16000,											/*	ui32CodeSize */
		8000,											/*	ui32RomTableSize */
		2000,											/*	ui32InterFrameBuffSize */
		(4096+BDSP_AF_P_EXTRA_SAMPLES)*4*2,				/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		100,											/*	ui32UserCfgBuffSize */
		(4096+BDSP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		2,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eSrcPostProc] =  */
	{
		BDSP_IMG_SRC_CODE_SIZE,							/*	ui32CodeSize */
		BDSP_IMG_SRC_TABLES_SIZE,						/*	ui32RomTableSize */
		BDSP_IMG_SRC_INTER_FRAME_SIZE,					/*	ui32InterFrameBuffSize */
		(64*1024+BDSP_AF_P_EXTRA_SAMPLES)*4*8,				/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		64*1024*4+1000,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_SRCUserConfigParams),	/*	ui32UserCfgBuffSize */
		(64*1024+BDSP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		8,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eDdbmPostProc] =  */
	{ 0,0,0,0,0,0,0,0,0,0,0,0 },
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eDownmixPostProc] =  */
	{ 0,0,0,0,0,0,0,0,0,0,0,0 },
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eCustomSurroundPostProc] =  */
	{
		13000,											/*	ui32CodeSize */
		8000,											/*	ui32RomTableSize */
		1500,											/*	ui32InterFrameBuffSize */
		(4096+BDSP_AF_P_EXTRA_SAMPLES)*4*2,				/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_CustomSurroundUserConfig),	/*	ui32UserCfgBuffSize	 */
		(4096+BDSP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		2,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0
	},
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eCustomBassPostProc] =  */
	{
		13000,											/*	ui32CodeSize */
		5900 ,											/*	ui32RomTableSize */
		800,											/*	ui32InterFrameBuffSize */
		(4096+BDSP_AF_P_EXTRA_SAMPLES)*4*2,				/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_CustomBassUserConfig),	/*	ui32UserCfgBuffSize	 */
		(4096+BDSP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		2,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0
	},
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eKaraokeCapablePostProc] =  */
	{ 0,0,0,0,0,0,0,0,0,0,0,0 },
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eCustomVoicePostProc] =  */
	{
		BDSP_IMG_CUSTOMVOICE_CODE_SIZE,					/*	ui32CodeSize */
		BDSP_IMG_CUSTOMVOICE_TABLES_SIZE,				/*	ui32RomTableSize */
		BDSP_IMG_CUSTOMVOICE_INTER_FRAME_SIZE,			/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_CustomVoiceConfigParams),/*	ui32UserCfgBuffSize	 */
		0,												/*	ui32MaxSizePerChan */
		0,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_CustomVoiceStatusInfo)	/*  FwStatusBuffSize */
	},
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_ePeqPostProc] =  */
	{ 0,0,0,0,0,0,0,0,0,0,0,0 },
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eAvlPostProc] =  */
	{
		BDSP_IMG_BRCM_AVL_CODE_SIZE,					/*	ui32CodeSize */
		BDSP_IMG_BRCM_AVL_TABLES_SIZE,					/*	ui32RomTableSize */
		BDSP_IMG_BRCM_AVL_INTER_FRAME_SIZE,				/*	ui32InterFrameBuffSize */
		(6144 + BDSP_AF_P_EXTRA_SAMPLES)*4*8,			/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_AVLConfigParams),		/*	ui32UserCfgBuffSize */
		(6144+BDSP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		8,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_ePl2PostProc] =  */
	{
		18000,											/*	ui32CodeSize */
		1500,											/*	ui32RomTableSize */
		3000,											/*	ui32InterFrameBuffSize */
		(1152*4+BDSP_AF_P_EXTRA_SAMPLES)*4*6,			/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_PL2ConfigParams),		/*	ui32UserCfgBuffSize */
		(1152*4+BDSP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		6,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eXenPostProc] =  */
	{ 0,0,0,0,0,0,0,0,0,0,0,0 },
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eBbePostProc] =  */
	{ 0,0,0,0,0,0,0,0,0,0,0,0 },
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eDsolaPostProc] =  */
	{
		BDSP_IMG_DSOLA_CODE_SIZE,						/*	ui32CodeSize */
		BDSP_IMG_DSOLA_TABLES_SIZE,						/*	ui32RomTableSize */
		BDSP_IMG_DSOLA_INTER_FRAME_SIZE,				/*	ui32InterFrameBuffSize */
		(6144+BDSP_AF_P_EXTRA_SAMPLES)*4*2,				/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		60000,											/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_DsolaConfigParams),		/*	ui32UserCfgBuffSize	*/
		(6144+BDSP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		2,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eDtsNeoPostProc] =  */
	{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eDDConvert] =  */
	{
		14336,											/*	ui32CodeSize */
		4096,											/*	ui32RomTableSize */
		2048,											/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		20000	,											/*	ui32InterStageGenericBuffSize */
		1024,											/*	ui32ScratchBuffSize */
		0,												/*	ui32UserCfgBuffSize	*/
		0,												/*	ui32MaxSizePerChan */
		0,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eAudioDescriptorFadePostProc] =  */
	{
		12000,											/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		0,												/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		0,												/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_AudioDescFadeConfigParams),	/*	ui32UserCfgBuffSize	*/
		0,												/*	ui32MaxSizePerChan */
		0,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_eAbsent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eAudioDescriptorPanPostProc] =  */
	{
		12000,											/*	ui32CodeSize */
		7000,											/*	ui32RomTableSize */
		1024,											/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		0,												/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_AudioDescPanConfigParams),	/*	ui32UserCfgBuffSize	*/
		0,												/*	ui32MaxSizePerChan */
		0,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_ePCMRouterPostProc] =  */
	{
		12000,											/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		0,												/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		0,												/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		0,												/*	ui32UserCfgBuffSize	*/
		0,												/*	ui32MaxSizePerChan */
		0,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_eAbsent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eWMAPassThrough] =  */
	{
		6000,											/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		20,												/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		13000,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		0,												/*	ui32UserCfgBuffSize	*/
		0,												/*	ui32MaxSizePerChan */
		0,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eSrsTruSurroundHDPostProc] =  */
	{
		BDSP_IMG_SRS_TRUSURROUNDHD_CODE_SIZE,			/*	ui32CodeSize */
		BDSP_IMG_SRS_TRUSURROUNDHD_TABLES_SIZE,			/*	ui32RomTableSize */
		BDSP_IMG_SRS_TRUSURROUNDHD_INTER_FRAME_SIZE,	/*	ui32InterFrameBuffSize */
		(4096+BDSP_AF_P_EXTRA_SAMPLES)*4*6,				/*	ui32InterStageIoBuffSize */
		20000,												/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_TruSurrndHDConfigParams),/*	ui32UserCfgBuffSize */
		(4096+BDSP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		6,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eSrsTruVolumePostProc] =  */
	{
		BDSP_IMG_SRS_TVOL_CODE_SIZE,					/*	ui32CodeSize */
		BDSP_IMG_SRS_TVOL_TABLES_SIZE,					/*	ui32RomTableSize */
		BDSP_IMG_SRS_TVOL_INTER_FRAME_SIZE,				/*	ui32InterFrameBuffSize */
		(4096+BDSP_AF_P_EXTRA_SAMPLES)*4*2,				/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_TruVolumeUserConfig),	/*	ui32UserCfgBuffSize */
		(4096+BDSP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		2,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eDolbyVolumePostProc] =  */
	{
		40000,											/*	ui32CodeSize */
		200000,											/*	ui32RomTableSize */
		20000,											/*	ui32InterFrameBuffSize */
		(6144 + BDSP_AF_P_EXTRA_SAMPLES)*4*2,			/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		85000,											/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_DolbyVolumeUserConfig),	/*	ui32UserCfgBuffSize */
		(6144+BDSP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		2,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eAudysseyVolumePostProc] =  */
	{
		BDSP_IMG_ADV_CODE_SIZE,							/*	ui32CodeSize */
		BDSP_IMG_ADV_TABLES_SIZE,						/*	ui32RomTableSize */
		BDSP_IMG_ADV_INTER_FRAME_SIZE,					/*	ui32InterFrameBuffSize */
		(6144 + BDSP_AF_P_EXTRA_SAMPLES)*4*2,			/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_AudysseyVolUserConfig),	/*	ui32UserCfgBuffSize */
		(6144+BDSP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		2,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_AudysseyVolStatusInfo)	/*  FwStatusBuffSize */
	},

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eBrcm3DSurroundPostProc] =  */
	{
		BDSP_IMG_BRCM_3DSURROUND_CODE_SIZE,				/*	ui32CodeSize */
		BDSP_IMG_BRCM_3DSURROUND_TABLES_SIZE,			/*	ui32RomTableSize */
		BDSP_IMG_BRCM_3DSURROUND_INTER_FRAME_SIZE,		/*	ui32InterFrameBuffSize */
		(6144 + BDSP_AF_P_EXTRA_SAMPLES)*4*2,			/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_Brcm3DSurroundConfigParams),/*	ui32UserCfgBuffSize */
		(6144+BDSP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		2,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eFWMixerPostProc] =  */
	{
		BDSP_IMG_FW_MIXER_CODE_SIZE,					/*	ui32CodeSize */
		BDSP_IMG_FW_MIXER_TABLES_SIZE,					/*	ui32RomTableSize */
		BDSP_IMG_FW_MIXER_INTER_FRAME_SIZE,				/*	ui32InterFrameBuffSize */
		(6144 + BDSP_AF_P_EXTRA_SAMPLES)*4*6,			/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		16384*6,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_MixerConfigParams),		/*	ui32UserCfgBuffSize */
		(6144+BDSP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		6,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eMonoDownMixPostProc] =  */
	{
		6000,											/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		0,												/*	ui32InterFrameBuffSize */
		(6144 + BDSP_AF_P_EXTRA_SAMPLES)*4*2,			/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		20,											/*	ui32UserCfgBuffSize */
		(6144+BDSP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		2,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eMs10DDConvert] =  */
	{
		22500,											/*	ui32CodeSize */
		4096,											/*	ui32RomTableSize */
		2048,											/*	ui32InterFrameBuffSize */
		(6144 + BDSP_AF_P_EXTRA_SAMPLES)*4*2,			/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		20,												/*	ui32UserCfgBuffSize */
		(6144+BDSP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		2,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eAudysseyABXPostProc] = */
	{
		BDSP_IMG_ABX_CODE_SIZE,							/*	ui32CodeSize */
		BDSP_IMG_ABX_TABLES_SIZE,						/*	ui32RomTableSize */
		BDSP_IMG_ABX_INTER_FRAME_SIZE,					/*	ui32InterFrameBuffSize */
		(6144 + BDSP_AF_P_EXTRA_SAMPLES)*4*2,			/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_ABXConfigParams),		/*	ui32UserCfgBuffSize */
		(6144+BDSP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		2,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eSrsCircleSurrPostProc] =  */
	{
		22500,											/*	ui32CodeSize */
		1024,											/*	ui32RomTableSize */
		6144,											/*	ui32InterFrameBuffSize */
		(6144 + BDSP_AF_P_EXTRA_SAMPLES)*4*7,			/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		100,											/*	ui32UserCfgBuffSize */
		(6144+BDSP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		7,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eSrsEqualizerPostProc] =  */
	{
		BDSP_IMG_SRS_EQU_HL_CODE_SIZE,					/*	ui32CodeSize */
		BDSP_IMG_SRS_EQU_HL_TABLES_SIZE,				/*	ui32RomTableSize */
		BDSP_IMG_SRS_EQU_HL_INTER_FRAME_SIZE,			/*	ui32InterFrameBuffSize */
		(6144 + BDSP_AF_P_EXTRA_SAMPLES)*4*2,			/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_SRS_EqualizerHardLimiterConfigParams),	/*	ui32UserCfgBuffSize */
		(6144+BDSP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		2,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eDdrePostProc] =  */
	{
		BDSP_IMG_DDRE_CODE_SIZE,						/*	ui32CodeSize */
		BDSP_IMG_DDRE_TABLES_SIZE,						/*	ui32RomTableSize */
		BDSP_IMG_DDRE_INTER_FRAME_SIZE,					/*	ui32InterFrameBuffSize */
		(6144 + BDSP_AF_P_EXTRA_SAMPLES)*4*6,			/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		100,											/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_DDReencodeConfigParams),/*	ui32UserCfgBuffSize */
		(6144+BDSP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		6,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eDv258PostProc] =  */
	{
		BDSP_IMG_DV258_CODE_SIZE,						/*	ui32CodeSize */
		BDSP_IMG_DV258_TABLES_SIZE,						/*	ui32RomTableSize */
		BDSP_IMG_DV258_INTER_FRAME_SIZE,				/*	ui32InterFrameBuffSize */
		(6144 + BDSP_AF_P_EXTRA_SAMPLES)*4*6,			/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		100,											/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_DV258ConfigParams),		/*	ui32UserCfgBuffSize */
		(6144+BDSP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		6,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eCustomDbePostProc] =  */
	{
		BDSP_IMG_CUSTOM_DBE_CODE_SIZE,					/*	ui32CodeSize */
		BDSP_IMG_CUSTOM_DBE_TABLES_SIZE,				/*	ui32RomTableSize */
		BDSP_IMG_CUSTOM_DBE_INTER_FRAME_SIZE,			/*	ui32InterFrameBuffSize */
		(6144 + BDSP_AF_P_EXTRA_SAMPLES)*4*6,			/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		120,											/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_DbeConfigParams),		/*	ui32UserCfgBuffSize */
		(6144+BDSP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		2,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eCustomAcfPostProc] =  */
	{
		BDSP_IMG_CUSTOM_ACF_CODE_SIZE,					/*	ui32CodeSize */
		BDSP_IMG_CUSTOM_ACF_TABLES_SIZE,				/*	ui32RomTableSize */
		BDSP_IMG_CUSTOM_ACF_INTER_FRAME_SIZE,			/*	ui32InterFrameBuffSize */
		(6144 + BDSP_AF_P_EXTRA_SAMPLES)*4*6,			/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		120,											/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_AcfConfigParams),		/*	ui32UserCfgBuffSize */
		(6144+BDSP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		2,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eCustomAvlpPostProc] =  */
	{
		BDSP_IMG_CUSTOM_AVLP_CODE_SIZE,					/*	ui32CodeSize */
		BDSP_IMG_CUSTOM_AVLP_TABLES_SIZE,				/*	ui32RomTableSize */
		BDSP_IMG_CUSTOM_AVLP_INTER_FRAME_SIZE,			/*	ui32InterFrameBuffSize */
		(6144 + BDSP_AF_P_EXTRA_SAMPLES)*4*6,			/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		100,											/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_AvlPConfigParams),		/*	ui32UserCfgBuffSize */
		(6144+BDSP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		2,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eGenCdbItbPostProc] =  */
	{
		BDSP_IMG_GEN_CDBITB_CODE_SIZE,					/*	ui32CodeSize */
		BDSP_IMG_GEN_CDBITB_TABLES_SIZE,				/*	ui32RomTableSize */
		BDSP_IMG_GEN_CDBITB_INTER_FRAME_SIZE,			/*	ui32InterFrameBuffSize */
		(6144 + BDSP_AF_P_EXTRA_SAMPLES)*4*6,			/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_GenCdbItbConfigParams),	/*	ui32UserCfgBuffSize */
		(6144+BDSP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		2,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eBtscEncoderPostProc] =  */
	{
		BDSP_IMG_BTSCENC_CODE_SIZE,					/*	ui32CodeSize */
		BDSP_IMG_BTSCENC_TABLES_SIZE,				/*	ui32RomTableSize */
		BDSP_IMG_BTSCENC_INTER_FRAME_SIZE,			/*	ui32InterFrameBuffSize */
		(6144 + BDSP_AF_P_EXTRA_SAMPLES)*4*6,			/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_BtscEncoderConfigParams),	/*	ui32UserCfgBuffSize */
		(6144+BDSP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		2,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eSpeexAECPostProc] =  */
	{
		BDSP_IMG_SPEEXAEC_CODE_SIZE,				    /*	ui32CodeSize */
		BDSP_IMG_SPEEXAEC_TABLES_SIZE,				    /*	ui32RomTableSize */
		BDSP_IMG_SPEEXAEC_INTER_FRAME_SIZE,			    /*	ui32InterFrameBuffSize */
		(6144 + BDSP_AF_P_EXTRA_SAMPLES)*4*2,		    /*	ui32InterStageIoBuffSize */
		20000,										    /*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		sizeof(BDSP_Raaga_Audio_SpeexAECConfigParams),  /*	ui32UserCfgBuffSize */
		(6144+BDSP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		2,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eEndOfPpAlgos] =  */
	{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eMixerFrameSync] =  */
	{
	    BDSP_IMG_MIXER_IDS_SIZE,						/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		BDSP_IMG_MIXER_IDS_INTER_FRAME_SIZE,			/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		20000,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		160,											/*	ui32UserCfgBuffSize */
		0,												/*	ui32MaxSizePerChan */
		2,												/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		sizeof(BDSP_Raaga_Audio_IdsTsmInfo)				/*  FwStatusBuffSize */
    },

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eEndOfPpFsAlgos] =  */
	{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eSyslib] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eAlgoLib] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eIDSCommonLib] =  */
	{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eVidIDSCommonLib] =  */
	{
		BDSP_IMG_VIDIDSCOMMON_CODE_SIZE,						/*	ui32CodeSize */
		0,														/*	ui32RomTableSize */
		BDSP_IMG_VIDIDSCOMMON_INTER_FRAME_SIZE,					/*	ui32InterFrameBuffSize */
		0,														/*	ui32InterStageIoBuffSize */
		8192,													/*	ui32InterStageGenericBuffSize */
		0,														/*	ui32ScratchBuffSize */
		sizeof(BDSP_VideoEncodeTaskDatasyncSettings),													/*	ui32UserCfgBuffSize */
		0,														/*	ui32MaxSizePerChan */
		2,														/*	ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,					/*	eInterFrameBuffType */
		BDSP_AF_P_FwStatus_ePresent,							/*	eFwStatusBuffType */
		160									 					/*	FwStatusBuffSize */

	},

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eEndOfLibAlgos] =  */
	{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/* 	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eScm1] = 	*/
	{
		BDSP_IMG_SCM1_DECODE_SIZE,						/* ui32CodeSize */
		BDSP_IMG_SCM1_DECODE_TABLES_SIZE,				/* ui32RomTableSize */
		BDSP_IMG_SCM1_DECODE_INTER_FRAME_SIZE,			/* ui32InterFrameBuffSize */
		(2048+BDSP_AF_P_EXTRA_SAMPLES)*1,		/* ui32InterStageIoBuffSize */
		1024,									/* ui32InterStageGenericBuffSize */
		0,										/* ui32ScratchBuffSize */
		0, 										/* ui32UserCfgBuffSize */
		(2048+BDSP_AF_P_EXTRA_SAMPLES), 		/* ui32MaxSizePerChan */
		1,										/* ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,	/* eInterFrameBuffType */
		BDSP_AF_P_FwStatus_eAbsent, 			/* eFwStatusBuffType */
		0										/* FwStatusBuffSize */
	},

	/* 	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eScm2] = 	*/
	{
		BDSP_IMG_SCM2_DECODE_SIZE,						/* ui32CodeSize */
		BDSP_IMG_SCM2_DECODE_TABLES_SIZE,				/* ui32RomTableSize */
		BDSP_IMG_SCM2_DECODE_INTER_FRAME_SIZE,			/* ui32InterFrameBuffSize */
		(2048+BDSP_AF_P_EXTRA_SAMPLES)*1,		/* ui32InterStageIoBuffSize */
		1024,									/* ui32InterStageGenericBuffSize */
		0,										/* ui32ScratchBuffSize */
		0, 										/* ui32UserCfgBuffSize */
		(2048+BDSP_AF_P_EXTRA_SAMPLES), 		/* ui32MaxSizePerChan */
		1,										/* ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,	/* eInterFrameBuffType */
		BDSP_AF_P_FwStatus_eAbsent, 			/* eFwStatusBuffType */
		0										/* FwStatusBuffSize */
	},
	/* 	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eScm3] = 	*/
	{
		BDSP_IMG_SCM3_DECODE_SIZE,						/* ui32CodeSize */
		BDSP_IMG_SCM3_DECODE_TABLES_SIZE,				/* ui32RomTableSize */
		BDSP_IMG_SCM3_DECODE_INTER_FRAME_SIZE,			/* ui32InterFrameBuffSize */
		(2048+BDSP_AF_P_EXTRA_SAMPLES)*1,		/* ui32InterStageIoBuffSize */
		1024,									/* ui32InterStageGenericBuffSize */
		0,										/* ui32ScratchBuffSize */
		0, 										/* ui32UserCfgBuffSize */
		(2048+BDSP_AF_P_EXTRA_SAMPLES), 		/* ui32MaxSizePerChan */
		1,										/* ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_ePresent,	/* eInterFrameBuffType */
		BDSP_AF_P_FwStatus_eAbsent, 			/* eFwStatusBuffType */
		0										/* FwStatusBuffSize */
	},	
		/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eEndOfScmAlgos] =  */
	{ 0,0,0,0,0,0,0,0,0,0,0,0 },
	
	/* 	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eScmTask] = 	*/
	{
		BDSP_IMG_SCM_TASK_CODE_SIZE,			/* ui32CodeSize */
		0,				/* ui32RomTableSize */
		0,			/* ui32InterFrameBuffSize */
		(2048+BDSP_AF_P_EXTRA_SAMPLES)*1,		/* ui32InterStageIoBuffSize */
		1024,									/* ui32InterStageGenericBuffSize */
		0,										/* ui32ScratchBuffSize */
		0, 										/* ui32UserCfgBuffSize */
		(2048+BDSP_AF_P_EXTRA_SAMPLES), 		/* ui32MaxSizePerChan */
		1,										/* ui32MaxNumChansSupported */
		BDSP_AF_P_InterFrameBuffType_eAbsent,	/* eInterFrameBuffType */
		BDSP_AF_P_FwStatus_eAbsent, 			/* eFwStatusBuffType */
		0										/* FwStatusBuffSize */
	},	
	
	
	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eEndOfTaskAlgos] =  */
	{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eEndOfAlgos] =  */
	{ 0,0,0,0,0,0,0,0,0,0,0,0 }

};


const BDSP_VF_P_sOutputBuffInfo sOutputBuffInfo[BDSP_AF_P_AlgoId_eEndOfDecodeAlgos - BDSP_VF_P_AlgoId_eVideoAlgoStartIdx] =
{
	/*	RealVideo Decoder */
	{
	  2,	/* ui32NumRefBuff */
	  2,    /* ui32MinNumRefBuff */
	  0,    /* eRefPixelDataType */
	  16,   /* ui32NumDispBuff */
	  4,    /* ui32MinNumDispBuff */
	  0,    /* eDispPixelDataType */
      32,	/* ui32PaddingX */
	  32    /* ui32PaddingY */
	},
	/* RealVideo Decoder - End */
	/*	VP6 Decoder */
	{
	  3,	/* ui32NumRefBuff */
	  3,    /* ui32MinNumRefBuff */
	  0,    /* eRefPixelDataType */
	  16,   /* ui32NumDispBuff */
	  4,    /* ui32MinNumDispBuff */
	  0,    /* eDispPixelDataType */
      48,	/* ui32PaddingX */
	  48    /* ui32PaddingY */
	}
	/* VP6 Decoder - End */

};


/*
	This structure is used in the PI to determine the input-output channel mapping for all the algorithms
	for configuring the FORK matrix configuration.

	Bypass  =  Output number of channel is the same as that of input number channel.
	eFixed  =  Output number of channel is fixed to a particular value.

*/
const BDSP_AF_P_PpOutMode BDSP_sPpOutMode[BDSP_AudioProcessing_eMax] =
{
    {BDSP_AF_P_OutModeType_eBypass,	BDSP_AF_P_DistinctOpType_eInvalid},		/*Dts Neo*/
    {BDSP_AF_P_OutModeType_eBypass,	BDSP_AF_P_DistinctOpType_eInvalid},		/*AVL*/
    {BDSP_AF_P_OutModeType_eFixed,	BDSP_AF_P_DistinctOpType_e5_1_PCM},		/*Pl2*/
    {BDSP_AF_P_OutModeType_eFixed,	BDSP_AF_P_DistinctOpType_eStereo_PCM},	/*SrsXt*/
    {BDSP_AF_P_OutModeType_eBypass,	BDSP_AF_P_DistinctOpType_eInvalid},		/*Bbe*/
    {BDSP_AF_P_OutModeType_eBypass,	BDSP_AF_P_DistinctOpType_eInvalid},		/*SRC*/
    {BDSP_AF_P_OutModeType_eFixed,	BDSP_AF_P_DistinctOpType_eStereo_PCM},  /*Custom Voice*/
    {BDSP_AF_P_OutModeType_eBypass,	BDSP_AF_P_DistinctOpType_eInvalid},		/*AacDownmix*/
    {BDSP_AF_P_OutModeType_eBypass,	BDSP_AF_P_DistinctOpType_eInvalid},		/*DSOLA*/
    {BDSP_AF_P_OutModeType_eFixed,	BDSP_AF_P_DistinctOpType_eStereo_PCM},	/*SRSHD*/
	{BDSP_AF_P_OutModeType_eBypass,	BDSP_AF_P_DistinctOpType_eInvalid},		/*Generic pass through*/
	{BDSP_AF_P_OutModeType_eBypass,	BDSP_AF_P_DistinctOpType_eInvalid},		/*SRS Volume IQ */
	{BDSP_AF_P_OutModeType_eBypass,	BDSP_AF_P_DistinctOpType_eInvalid},		/*Dolby Volume*/
	{BDSP_AF_P_OutModeType_eBypass,	BDSP_AF_P_DistinctOpType_eInvalid},		/*Audyssey Volume*/
	{BDSP_AF_P_OutModeType_eFixed,	BDSP_AF_P_DistinctOpType_eStereo_PCM},	/*3D Surround Volume*/
	{BDSP_AF_P_OutModeType_eBypass,	BDSP_AF_P_DistinctOpType_eInvalid},		/*Fw mixer */
	{BDSP_AF_P_OutModeType_eBypass,	BDSP_AF_P_DistinctOpType_eInvalid},		/*Audyssey ABX */
	{BDSP_AF_P_OutModeType_eBypass,	BDSP_AF_P_DistinctOpType_eInvalid},		/*DDRE */
	{BDSP_AF_P_OutModeType_eBypass,	BDSP_AF_P_DistinctOpType_eInvalid},		/*DV258 */
	{BDSP_AF_P_OutModeType_eBypass,	BDSP_AF_P_DistinctOpType_eInvalid},		/*SRS CSD */
	{BDSP_AF_P_OutModeType_eBypass,	BDSP_AF_P_DistinctOpType_eInvalid},		/*SRS EQ HL*/
	{BDSP_AF_P_OutModeType_eBypass,	BDSP_AF_P_DistinctOpType_eInvalid},		/*CUSTOM DBE*/
	{BDSP_AF_P_OutModeType_eBypass,	BDSP_AF_P_DistinctOpType_eInvalid},		/*CUSTOM ACF*/
	{BDSP_AF_P_OutModeType_eBypass,	BDSP_AF_P_DistinctOpType_eInvalid},		/*CUSTOM AVLP*/
	{BDSP_AF_P_OutModeType_eFixed,	BDSP_AF_P_DistinctOpType_eCdb},		    /*Generate CdbItb*/
	{BDSP_AF_P_OutModeType_eFixed,	BDSP_AF_P_DistinctOpType_eInvalid},		/*BTSC ENC */
};


const BDSP_AudioTypeInfo sAudioDecodeInfo[BDSP_AudioType_eMax] =
{
	{
#ifdef BDSP_MPEG_SUPPORT
		true,
#else
		false,
#endif
        false, "MPEG",               {false, false, false, false, false, true,  false, true,  false, false, false, false, false, false, false}}, /* BDSP_AudioType_eMpeg */
#ifndef BDSP_MS10_SUPPORT
	{
#ifdef BDSP_AACSBR_SUPPORT
		true,
#else
		false,
#endif
        true,  "AAC ADTS",           {false, false, false, false, true,  true,  false, true,  false, false, false, true , false, false, false}}, /* BDSP_AudioType_eAac */
	{
#ifdef BDSP_AACSBR_SUPPORT
		true,
#else
		false,
#endif
        true,  "AAC LOAS",           {false, false, false, false, true,  true,  false, true,  false, false, false, true , false, false, false}}, /* BDSP_AudioType_eAacLoas */
	{
#ifdef BDSP_AACSBR_SUPPORT
		true,
#else
		false,
#endif
        true,  "AAC SBR LOAS",       {false, false, false, false, true,  true,  false, true,  false, false, false, true , false, false, false}}, /* BDSP_AudioType_eAacSbrAdts */
	{
#ifdef BDSP_AACSBR_SUPPORT
        true,
#else
        false,
#endif
		true,  "AAC SBR ADTS",       {false, false, false, false, true,  true,  false, true,  false, false, false, true , false, false, false}}, /* BDSP_AudioType_eAacSbr */
#else /* BDSP_MS10_SUPPORT */
	{true,  true,  "Dolby Pulse",    {false, false, false, false, true,  true,  false, true,  false, false, false, true , false, false, false}}, /* BDSP_AudioType_eAac */
	{true,  true,  "Dolby Pulse",    {false, false, false, false, true,  true,  false, true,  false, false, false, true , false, false, false}}, /* BDSP_AudioType_eAacLoas */
	{true,  true,  "Dolby Pulse",    {false, false, false, false, true,  true,  false, true,  false, false, false, true , false, false, false}}, /* BDSP_AudioType_eAacSbrAdts */
	{true,  true,  "Dolby Pulse",    {false, false, false, false, true,  true,  false, true,  false, false, false, true , false, false, false}}, /* BDSP_AudioType_eAacSbr */
#endif /* BDSP_MS10_SUPPORT */
	{
#ifdef BDSP_AC3_SUPPORT
        true,
#else
        false,
#endif
		true,  "AC3",                {false, false, false, false, true,  true,  true,  true,  false, false, false, false, false, false, false}}, /* BDSP_AudioType_eAc3 */
	{
#ifdef BDSP_DDP_SUPPORT
        true,
#else
        false,
#endif
		true,  "AC3 Plus",           {false, false, false, false, true,  true,  true,  true,  false, false, false, false, false, false, false}}, /* BDSP_AudioType_eAc3Plus */
	{false, true,  "BD LPCM",        {false, false, false, false, true,  true,  false, false, false, false, false, false, false, false, false}}, /* BDSP_AudioType_eLpcmBd */
	{false, true,  "HD-DVD LPCM",    {false, false, false, false, true,  true,  false, false, false, false, false, false, false, false, false}}, /* BDSP_AudioType_eLpcmHdDvd */
	{
#ifdef BDSP_DTSHD_SUPPORT
            true,
#else
            false,
#endif

	true,  "DTS-HD",                 {false, false, false, false, true,  true,  false, true,  false, false, false, false, false, false, false}}, /* BDSP_AudioType_eDtshd */
	{
#ifdef BDSP_LPCMDVD_SUPPORT
        true,
#else
        false,
#endif
	true,  "DVD LPCM",               {false, false, false, true,  true,  true,  false, false, false, false, false, false, false, false, false}}, /* BDSP_AudioType_eLpcmDvd */
	{
#ifdef BDSP_WMA_SUPPORT
        true,
#else
        false,
#endif
		false, "WMA-STD",            {false, false, false, false, false, true,  false, false, false, false, false, false, false, false, false}}, /* BDSP_AudioType_eWmaStd */
{
#ifdef BDSP_MLP_SUPPORT
        true,
#else
        false,
#endif

 		true,  "Dolby TrueHD",   {false, false, false, false, true,  true,  false, false, false, false, false, false, false, false, false}}, /* BDSP_AudioType_eMlp */
	{
#ifdef BDSP_PCM_SUPPORT
		true,
#else
		false,
#endif
		true,  "PCM",                {false, false, false, false, true,  true,  false, false, false, false, false, false, false, false, false}}, /* BDSP_AudioType_ePcm */
	{
#ifdef BDSP_DTSLBR_SUPPORT
		true,
#else
		false,
#endif
		 true,  "DTS LBR",        {false, false, false, false, true,  true,  false, false, false, false, false, false, false, false, false}}, /* BDSP_AudioType_eDtsLbr */
	{false, true,  "DDP 7.1",        {false, false, false, true,  true,  true,  false, true,  false, false, false, false, false, false, false}}, /* BDSP_AudioType_eDdp7_1 */
	{false, true,  "MPEG-MC",        {false, false, false, false, true,  true,  false, false, false, false, false, false, false, false, false}}, /* BDSP_AudioType_eMpegMc */
	{
#ifdef BDSP_WMAPRO_SUPPORT
        true,
#else
        false,
#endif
		true,  "WMA PRO",            {false, false, false, false, true,  true,  true,  true,  false, false, false, false, false, false, false}}, /* BDSP_AudioType_eWmaPro */
	{false, true,  "DVD A-LPCM",     {false, false, false, false, true,  true,  false, false, false, false, false, false, false, false, false}}, /* BDSP_AudioType_eLpcmDvdA */
	{
#ifdef BDSP_DTSBROADCAST_SUPPORT
        true,
#else
        false,
#endif
		true,  "DTS Broadcast",      {false, false, false, false, true,  true,  false, true,  false, false, false, false, false, false, false}}, /* BDSP_AudioType_eDtsBroadcast */
	{
#ifdef BDSP_PCMWAV_SUPPORT
        true,
#else
        false,
#endif
		true,  "PCM-WAV",           {false, false, false, false, true,  true,  false, false, false, false, false, false, false, false, false}}, /* BDSP_AudioType_ePcmWav */
	{
#ifdef BDSP_AMR_SUPPORT
        true,
#else
        false,
#endif
		false, "AMR",                {false, false, false, false, false, true,  false, false, false, false, false, false, false, false, false}}, /* BDSP_AudioType_eAmr */
	{
#ifdef BDSP_DRA_SUPPORT
        true,
#else
        false,
#endif
		true,  "DRA",                {false, false, false, false, true,  true,  false, true,  false, false, false, false, false, false, false}}, /* BDSP_AudioType_eDra */
	{
#ifdef BDSP_REALAUDIOLBR_SUPPORT
        true,
#else
        false,
#endif
		false, "REAL AUDIO",         {false, false, false, false, false, true,  false, false, false, false, false, false, false, false, false}}, /* BDSP_AudioType_eRealAudioLbr */
	{
#ifdef BDSP_ADPCM_SUPPORT
        true,
#else
        false,
#endif
		true,  "AD-PCM",             {false, false, false, false, true,  true,  false, false, false, false, false, false, false, false, false}},  /* BDSP_AudioType_eAdpcm*/
	{
#ifdef BDSP_G711G726_SUPPORT
        true,
#else
        false,
#endif
		true,  "G.711/G.726",        {false, false, false, false, false, true,  false, false, false, false, false, false, true , false, false}},  /* BDSP_AF_P_AlgoId_eG711G726Decode*/
	{
#ifdef BDSP_G729_SUPPORT
        true,
#else
        false,
#endif
		true,  "G.729",              {false, false, false, false, false, true,  false, false, false, false, false, false, true , false, false}},  /* BDSP_AF_P_AlgoId_eG729Decode*/
	{
#ifdef BDSP_VORBIS_SUPPORT
        true,
#else
        false,
#endif
		true,  "VORBIS",              {false, false, false, false, false, true,  false, false, false, false, false, false, false, false, false}},  /* BDSP_AF_P_AlgoId_eVorbisDecode*/
	{
#ifdef BDSP_G723_1_SUPPORT
        true,
#else
        false,
#endif
		true,  "G.723.1",            {false, false, false, false, false, true,  false, false, false, false, false, false, true , false, false}},  /* BDSP_AF_P_AlgoId_eG711G726Decode*/
	{
#ifdef BDSP_FLAC_SUPPORT
        true,
#else
        false,
#endif
		true,  "FLAC",              {false, false, false, false, false, true,  false, false, false, false, false, false, false, false, false}},  /* BDSP_AF_P_AlgoId_eFlacDecode*/
	{
#ifdef BDSP_MAC_SUPPORT
        true,
#else
        false,
#endif
		true,  "MAC",              {false, false, false, false, false, true,  false, false, false, false, false, false, false, false, false}},  /* BDSP_AF_P_AlgoId_eMacDecode*/
	{
#ifdef BDSP_AMRWB_SUPPORT
        true,
#else
        false,
#endif
		false, "AMRWB",                {false, false, false, false, false, true,  false, false, false, false, false, false, false, false, false}}, /* BDSP_AudioType_eAmrwb */
		{
#ifdef BDSP_ILBC_SUPPORT
        true,
#else
        false,
#endif
		false, "ILBC",                {false, false, false, false, false, true,  false, false, false, false, false, false, false, false, false}}, /* BDSP_AudioType_eiLBC */
		{
#ifdef BDSP_ISAC_SUPPORT
        true,
#else
        false,
#endif
		false, "ISAC",                {false, false, false, false, false, true,  false, false, false, false, false, false, false, false, false}}, /* BDSP_AudioType_eiSAC */

};

const BDSP_AudioEncodeInfo sAudioEncodeInfo[BDSP_AudioEncode_eMax] =
{
	{
#ifdef BDSP_MP3ENC_SUPPORT
		true,
#else
		false,
#endif
        "MPEG1-L3",              {false, false, false, false, false, false, true,  false, false, false, false, false, false, false, false}}, /* BDSP_AudioEncode_eMpeg1Layer3 */
	{
#ifdef BDSP_MP2ENC_SUPPORT
		true,
#else
		false,
#endif
	"MPEG1-L2",                  {false, false, false, false, false, false, true,  false, false, false, false, false, false, false, false}}, /* BDSP_AudioEncode_eMpeg1Layer2 */
	{
#ifdef BDSP_DTSENC_SUPPORT
		true,
#else
		false,
#endif
        "DTS",                   {false, false, false, false, false, false, true,  false, false, false, false, false, false, false, false}}, /* BDSP_AudioEncode_eDTS */
	{
#ifdef BDSP_AACLCENC_SUPPORT
		true,
#else
		false,
#endif
        "AAC LC",                {false, false, false, false, false, false, true,  false, false, false, false, false, false, false, false}}, /* BDSP_AudioEncode_eAacLc */
	{
#ifdef BDSP_AACHEENC_SUPPORT
		true,
#else
		false,
#endif
        "AAC HE",                {false, false, false, false, false, false, true,  false, false, false, false, false, false, false, false}}, /* BDSP_AudioEncode_eAacHe */
	{
#ifdef BDSP_AC3ENC_SUPPORT
		true,
#else
		false,
#endif
        "DD Transcode",          {false, false, false, false, false, false, true,  false, false, false, false, false, false, false, false}}, /* BDSP_AudioEncode_eAc3 */
	{
#ifdef BDSP_SBCENC_SUPPORT
		true,
#else
		false,
#endif
        "SBC",                   {false, false, false, false, false, false, true,  false, false, false, false, false, false, false, false}},  /* BDSP_AudioEncode_eSbc */
	{
#ifdef BDSP_G711G726ENC_SUPPORT
		true,
#else
		false,
#endif
        "G.711/G.726 ENC",       {false, false, false, false, false, false, true,  false, false, false, false, false, true , false, false}},  /* BDSP_AudioEncode_eG711G726 */
	{
#ifdef BDSP_G729ENC_SUPPORT
		true,
#else
		false,
#endif
        "G.729 ENC",             {false, false, false, false, false, false, true,  false, false, false, false, false, true, false, false}},  /* BDSP_AudioEncode_eG729 */
	{
#ifdef BDSP_WMAENC_SUPPORT
		true,
#else
		false,
#endif
        "WMA ENC",               {false, false, false, false, false, false, true,  false, false, true, false, false, false, false, false}},  /* BDSP_AudioEncode_eWma */
	{
#ifdef BDSP_G723_1ENC_SUPPORT
		true,
#else
		false,
#endif
        "G.723.1 ENC",            {false, false, false, false, false, false, true,  false, false, true, false, false, true , false, false}},  /* BDSP_AudioEncode_eWma */
	{
#ifdef BDSP_G722ENC_SUPPORT
		true,
#else
		false,
#endif
        "G.722 ENC",             {false, false, false, false, false, false, true,  false, false, false, false, false, true, false, false}},  /* BDSP_AudioEncode_eG722 */
	{
#ifdef BDSP_AMRENC_SUPPORT
		true,
#else
		false,
#endif
        "AMR ENC",             {false, false, false, false, false, false, true,  false, false, false, false, false, true, false, false}},  /* BDSP_AudioEncode_eAmr */
	{
#ifdef BDSP_AMRWBENC_SUPPORT
		true,
#else
		false,
#endif
        "AMRWB ENC",             {false, false, false, false, false, false, true,  false, false, false, false, false, true, false, false}},  /* BDSP_AudioEncode_eAmrwb */
	{
#ifdef BDSP_ILBCENC_SUPPORT
		true,
#else
		false,
#endif
        "ILBC ENC",             {false, false, false, false, false, false, true,  false, false, false, false, false, true, false, false}},  /* BDSP_AudioEncode_eilbc */
		{
#ifdef BDSP_ISACENC_SUPPORT
		true,
#else
		false,
#endif
        "ISAC ENC",             {false, false, false, false, false, false, true,  false, false, false, false, false, true, false, false}},  /* BDSP_AudioEncode_eisac */

};

const BDSP_AudioProcessingInfo sAudioProcessingInfo[BDSP_AudioProcessing_eMax] =
{
	/* BDSP_AudioProcessing_eDtsNeo */
	{
		false,                                                                                   /* Supported */
		"DTS NEO",                                                                               /* Name */
		{false, false, false, false, true,  true,  false, false, false, false, false, false, false, false, false},    /* Valid Input Types */
		{false, false, false, false, true,  true,  false, false, false, false, false, false, false, false, false},    /* Valid Output Types */
		BDSP_AF_P_OutModeType_eBypass                                                            /* Outmode Type */
	},
	/* BDSP_AudioProcessing_eAVL */
	{
#ifdef BDSP_AVL_SUPPORT
		true,
#else
		false,
#endif                                                                                           /* Supported */
		"AVL",                                                                                   /* Name */
		{false, false, false, false, true,  true,  false, false, false, false, false, false, false, false, false},    /* Valid Input Types */
		{false, false, false, false, true,  true,  false, false, false, false, false, false, false, false, false},    /* Valid Output Types */
		BDSP_AF_P_OutModeType_eBypass                                                            /* Outmode Type */
	},
	/* BDSP_AudioProcessing_ePLll */
	{
		false,                                                                                   /* Supported */
		"PRO LOGIC II",                                                                          /* Name */
		{false, false, false, false, false, false, false, false, false, false, false, false, false, false, false},    /* Valid Input Types */
		{false, false, false, false, false, false, false, false, false, false, false, false, false, false, false},    /* Valid Output Types */
		BDSP_AF_P_OutModeType_eFixed                                                             /* Outmode Type */
	},
	/* BDSP_AudioProcessing_eSrsXt */
	{
#ifdef BDSP_SRSXT_SUPPORT
		true,
#else
		false,
#endif                                                                                           /* Supported */
		"SRS XT",                                                                                /* Name */
		{false, false, false, false, true,  true,  false, false, false, false, false, false, false, false, false},    /* Valid Input Types */
		{false, false, false, false, false, true,  false, false, false, false, false, false, false, false, false},    /* Valid Output Types */
		BDSP_AF_P_OutModeType_eFixed                                                             /* Outmode Type */
	},
	/* BDSP_AudioProcessing_eBbe */
    {
		false,                                                                                   /* Supported */
		"BBE",                                                                                   /* Name */
		{false, false, false, false, true,  true,  false, false, false, false, false, false, false, false, false},    /* Valid Input Types */
		{false, false, false, false, true,  true,  false, false, false, false, false, false, false, false, false},    /* Valid Output Types */
		BDSP_AF_P_OutModeType_eBypass                                                            /* Outmode Type */
	},
	/* BDSP_AudioProcessing_eSrc */
    {
#ifdef BDSP_SRC_SUPPORT
		true,
#else
		false,
#endif                                                                                           /* Supported */
		"SRC",                                                                                   /* Name */
		{false, false, false, false, true,  true,  false, false, false, false, false, false, false, false, false},    /* Valid Input Types */
		{false, false, false, false, true,  true,  false, false, false, false, false, false, false, false, false},    /* Valid Output Types */
		BDSP_AF_P_OutModeType_eBypass                                                            /* Outmode Type */
	},
	/* BDSP_AudioProcessing_eCustomVoice */
    {
#ifdef BDSP_CUSTOMVOICE_SUPPORT
		true,
#else
		false,
#endif                                                                                           /* Supported */
		"Custom Voice",                                                                          /* Name */
		{false, false, false, false, false, true,  false, false, false, false, false, false, false, false, false},    /* Valid Input Types */
		{false, false, false, false, false, true,  false, false, false, false, false, false, false, false, false},    /* Valid Output Types */
		BDSP_AF_P_OutModeType_eFixed                                                             /* Outmode Type */
	},
	/* BDSP_AudioProcessing_eAacDownmix */
    {
		false,                                                                                   /* Supported */
		"Downmix",                                                                               /* Name */
		{false, false, false, false, true,  true,  false, false, false, false, false, false, false, false, false},    /* Valid Input Types */
		{false, false, false, false, true,  true,  false, false, false, false, false, false, false, false, false},    /* Valid Output Types */
		BDSP_AF_P_OutModeType_eBypass                                                            /* Outmode Type */
	},
	/* BDSP_AudioProcessing_eDsola */
    {
#ifdef BDSP_DSOLA_SUPPORT
		true,
#else
		false,
#endif                                                                                           /* Supported */
		"DSOLA",                                                                                 /* Name */
		{false, false, false, false, false, true,  false, false, false, false, false, false, false, false, false},    /* Valid Input Types */
		{false, false, false, false, false, true,  false, false, false, false, false, false, false, false, false},    /* Valid Output Types */
		BDSP_AF_P_OutModeType_eBypass                                                            /* Outmode Type */
	},
	/* BDSP_AudioProcessing_eSrsHd */
    {
#ifdef BDSP_SRSHD_SUPPORT
		true,
#else
		false,
#endif                                                                                           /* Supported */
		"SRS HD",                                                                                /* Name */
		{false, false, false, false, true,  true,  false, false, false, false, false, false, false, false, false},    /* Valid Input Types */
		{false, false, false, false, false, true,  false, false, false, false, false, false, false, false, false},    /* Valid Output Types */
		BDSP_AF_P_OutModeType_eFixed                                                             /* Outmode Type */
	},
	/* BDSP_AudioProcessing_eGenericPassThru */
    {
		true,                                                                                    /* Supported */
		"Generic Passthru",                                                                      /* Name */
		{false, false, false, false, false, false, false, true,  false, false, false, false, false, false, false},    /* Valid Input Types */
		{false, false, false, false, false, false, true,  false, false, false, false, false, false, true , true },    /* Valid Output Types */
		BDSP_AF_P_OutModeType_eBypass                                                            /* Outmode Type */
	},
	/* BDSP_AudioProcessing_eMlpPassThru */
    {
#ifdef BDSP_MLP_PASSTHROUGH_SUPPORT	
		true,                      																		/* Supported */
#else
		false,
#endif
		"MLP Passthru",                                                                      /* Name */
		{false, false, false, false, false, false, false, true,  false, false, false, false, false, false, false},    /* Valid Input Types */
		{false, false, false, false, false, false, true,  false, false, false, false, false, false, true , true },    /* Valid Output Types */
		BDSP_AF_P_OutModeType_eBypass                                                            /* Outmode Type */
	},	
	/* BDSP_AudioProcessing_eSrsTruVolume */
    {
#ifdef BDSP_SRSTRUVOL_SUPPORT
		true,
#else
		false,
#endif                                                                                           /* Supported */
		"SRS TruVolume",                                                                         /* Name */
		{false, false, false, false, false, true,  false, false, false, false, false, false, false, false, false},    /* Valid Input Types */
		{false, false, false, false, false, true,  false, false, false, false, false, false, false, false, false},    /* Valid Output Types */
		BDSP_AF_P_OutModeType_eBypass                                                            /* Outmode Type */
	},
	/* BDSP_AudioProcessing_eDolbyVolume */
    {
		false,                                                                                   /* Supported */
		"Dolby Voume",                                                                           /* Name */
		{false, false, false, false, false, false, false, false, false, false, false, false, false, false, false},    /* Valid Input Types */
		{false, false, false, false, false, false, false, false, false, false, false, false, false, false, false},    /* Valid Output Types */
		BDSP_AF_P_OutModeType_eBypass                                                            /* Outmode Type */
	},
	/* BDSP_AudioProcessing_eAudysseyVolume */
    {
#ifdef BDSP_ADV_SUPPORT
		true,
#else
		false,
#endif                                                                                           /* Supported */
		"Audyssey Volume",                                                                       /* Name */
		{false, false, false, false, true,  true,  false, false, false, false, false, false, false, false, false},    /* Valid Input Types */
		{false, false, false, false, true,  true,  false, false, false, false, false, false, false, false, false},    /* Valid Output Types */
		BDSP_AF_P_OutModeType_eBypass                                                            /* Outmode Type */
	},
	/* BDSP_AudioProcessing_eBrcm3DSurround */
    {
#ifdef BDSP_BRCM3DSURROUND_SUPPORT
		true,
#else
		false,
#endif                                                                                           /* Supported */
		"3D Surround",                                                                           /* Name */
		{false, false, false, false, true,  true,  false, false, false, false, false, false, false, false, false},    /* Valid Input Types */
		{false, false, false, false, false, true,  false, false, false, false, false, false, false, false, false},    /* Valid Output Types */
		BDSP_AF_P_OutModeType_eFixed                                                             /* Outmode Type */
	},
	/* BDSP_AudioProcessing_eFWMixer */
    {
#ifdef BDSP_FWMIXER_SUPPORT
		true,
#else
		false,
#endif                                                                                           /* Supported */
		"Fw Mixer",                                                                              /* Name */
		{false, false, false, false, true,  true,  false, false, false, false, false, false, false, false, false},    /* Valid Input Types */
		{false, true,  true,  false, true,  true,  false, false, false, false, false, false, false, false, false},    /* Valid Output Types */
		BDSP_AF_P_OutModeType_eBypass                                                            /* Outmode Type */
	},
	/* BDSP_AudioProcessing_eAudysseyABX */
    {
#ifdef BDSP_ABX_SUPPORT
		true,                                                                                   
#else
		false,
#endif                                                                                           /* Supported */
		"Audyssey ABX",                                                                          /* Name */
		{false, false, false, false, false, true,  false, false, false, false, false, false, false, false, false},    /* Valid Input Types */
		{false, false, false, false, false, true,  false, false, false, false, false, false, false, false, false},    /* Valid Output Types */
		BDSP_AF_P_OutModeType_eBypass                                                            /* Outmode Type */
	},
	/* BDSP_AudioProcessing_eDdre */
    {
#ifdef BDSP_DDRE_SUPPORT
		true,
#else
		false,
#endif                                                                                           /* Supported */
		"DDRE",                                                                                  /* Name */
		{false, false, false, false, true,  true,  false, false, false, false, false, false, false, false, false},    /* Valid Input Types */
		{false, false, false, false, true,  true,  false, false, false, false, false, true , false, false, false},    /* Valid Output Types */
		BDSP_AF_P_OutModeType_eBypass                                                            /* Outmode Type */
	},
	/* BDSP_AudioProcessing_eDv258 */
    {
#ifdef BDSP_DV258_SUPPORT
		true,
#else
		false,
#endif                                                                                           /* Supported */
		"DV258",                                                                                 /* Name */
		{false, false, false, false, true,  true,  false, false, false, false, false, false, false, false, false},    /* Valid Input Types */
		{false, false, false, false, true,  true,  false, false, false, false, false, false, false, false, false},    /* Valid Output Types */
		BDSP_AF_P_OutModeType_eBypass                                                            /* Outmode Type */
	},
	/* BDSP_AudioProcessing_eSrsCsdTd */
    {
#ifdef BDSP_SRSCSTD_SUPPORT
		true,
#else
		false,
#endif                                                                                           /* Supported */
		"SRS CSDEC TD",                                                                          /* Name */
		{false, false, false, false, true,  true,  false, false, false, false, false, false, false, false, false},    /* Valid Input Types */
		{false, false, false, false, false, true,  false, false, false, false, false, false, false, false, false},    /* Valid Output Types */
		BDSP_AF_P_OutModeType_eBypass                                                            /* Outmode Type */
	},
	/* BDSP_AudioProcessing_eSrsGeq */
    {
#ifdef BDSP_SRSEQHL_SUPPORT
		true,
#else
		false,
#endif                                                                                           /* Supported */
		"SRS EQ HL",                                                                               /* Name */
		{false, false, false, false, false, true,  false, false, false, false, false, false, false, false, false},    /* Valid Input Types */
		{false, false, false, false, false, true,  false, false, false, false, false, false, false, false, false},    /* Valid Output Types */
		BDSP_AF_P_OutModeType_eBypass                                                            /* Outmode Type */
	},
	/* BDSP_AudioProcessing_eCustomDbe */
    {
#ifdef BDSP_CUSTOMDBE_SUPPORT
		true,
#else
		false,
#endif                                                                                           /* Supported */
		"Custom DBE",                                                                            /* Name */
		{false, false, false, false, false, true,  false, false, false, false, false, false, false, false, false},    /* Valid Input Types */
		{false, false, false, false, false, true,  false, false, false, false, false, false, false, false, false},    /* Valid Output Types */
		BDSP_AF_P_OutModeType_eBypass                                                            /* Outmode Type */
	},
	/* BDSP_AudioProcessing_eCustomAcf */
    {
#ifdef BDSP_CUSTOMACF_SUPPORT
		true,
#else
		false,
#endif                                                                                           /* Supported */
		"Custom ACF",                                                                            /* Name */
		{false, false, false, false, false, true,  false, false, false, false, false, false, false, false, false},    /* Valid Input Types */
		{false, false, false, false, false, true,  false, false, false, false, false, false, false, false, false},    /* Valid Output Types */
		BDSP_AF_P_OutModeType_eBypass                                                            /* Outmode Type */
	},
	/* BDSP_AudioProcessing_eCustomAvlp */
    {
#ifdef BDSP_CUSTOMAVLP_SUPPORT
		true,
#else
		false,
#endif                                                                                           /* Supported */
		"Custom AVLP",                                                                           /* Name */
		{false, false, false, false, false, true,  false, false, false, false, false, false, false, false, false},    /* Valid Input Types */
		{false, false, false, false, false, true,  false, false, false, false, false, false, false, false, false},    /* Valid Output Types */
		BDSP_AF_P_OutModeType_eBypass                                                            /* Outmode Type */
	},
	/* BDSP_AudioProcessing_eGenCdbItb */
    {
#ifdef BDSP_GENCDBITB_SUPPORT
		true,
#else
		false,
#endif                                                                                           /* Supported */
		"Gen CDB ITB",                                                                           /* Name */
		{false, false, false, false, false, false, false, false, false, false, false, false, false, false, false},    /* Valid Input Types */
		{false, false, false, false, false, false, false, false, false, true,  true,  false, false, false, false},    /* Valid Output Types */
		BDSP_AF_P_OutModeType_eFixed                                                             /* Outmode Type */
	},
	{
	/*BDSP_AudioProcessing_eBtscEncoder */
#ifdef BDSP_BTSCENC_SUPPORT
		true,
#else
		false,
#endif                                                                                           /* Supported */
		"BTSC ENC",                                                                           /* Name */
		{false, false, false, false, false, true, false, false, false, false, false, false, false, false, false},    /* Valid Input Types */
		{false, false, false, false, false, false, true, false, false, false, false, false, false, true , false},    /* Valid Output Types */
		BDSP_AF_P_OutModeType_eFixed                                                             /* Outmode Type */
	},
	{
#ifdef BDSP_SPEEXAEC_SUPPORT
        true,
#else
        false,
#endif                                                                                          /* Supported */
		"Speex AEC",                                                                            /* Name */
        {false, false, false, false, false, true,  false, false, false, false, false, false, true , false, false},   /* Valid Input Types */
        {false, false, false, false, false, true,  false, false, false, false, false, false, true , false, false},   /* Valid Output Types */
        BDSP_AF_P_OutModeType_eFixed,                                                          /* Outmode Type */
	}
};

const BDSP_Version BDSP_sRaagaVersion = {BDSP_RAAGA_MAJOR_VERSION, BDSP_RAAGA_MINOR_VERSION, BDSP_RAAGA_BRANCH_VERSION, BDSP_RAAGA_BRANCH_SUBVERSION};
