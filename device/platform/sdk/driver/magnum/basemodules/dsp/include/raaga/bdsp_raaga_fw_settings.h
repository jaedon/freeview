/***************************************************************************
 *     Copyright (c) 2004-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdsp_raaga_fw_settings.h $
 * $brcm_Revision: Hydra_Software_Devel/SW7425-4771/1 $
 * $brcm_Date: 5/3/13 6:05p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/dsp/include/raaga/bdsp_raaga_fw_settings.h $
 * 
 * Hydra_Software_Devel/SW7425-4771/1   5/3/13 6:05p raghurat
 * SW7425-4771: patches for following JIRAs ( SW7425-4399, SW7425-4386,
 * SW7425-4405, FWRAA-633, FWRAA-643  ) on
 * RAAGA_BDSP_RELEASE_7p6_20130211 for HUMAX
 * 
 * Hydra_Software_Devel/71   1/29/13 12:09p sriniva
 * SW7425-4405: Adding user config to AACHE encoder to select channel
 * while encoding dual mono streams and encode it as mono
 * 
 * Hydra_Software_Devel/70   1/21/13 10:25a dlimaye
 * FWRAA-543: Add 7.1 support to PCMWAV decoder  Checking in src, ids and
 * host interface files
 * 
 * Hydra_Software_Devel/69   1/10/13 2:07p shivak
 * FWRAA-513 : added comments
 * 
 * Hydra_Software_Devel/68   1/10/13 12:34p skalapur
 * FWRAA-474:iLBC code merging to main latest
 * 
 * Hydra_Software_Devel/67   1/10/13 11:35a shivak
 * FWRAA-513 : Adding support CC in h264
 * 
 * Hydra_Software_Devel/66   1/10/13 10:45a skalapur
 * FWRAA-474:iLBC code merging to main latest
 * 
 * Hydra_Software_Devel/MAGNUM_ILBC_ISAC_integration/1   12/20/12 11:17a skalapur
 * FWRAA-474:iLBC and iSAC integration
 * 
 * Hydra_Software_Devel/65   9/18/12 6:34p ananthan
 * SW7346-914: Adding support for MPEG Ancillary data parsing.
 * 
 * Hydra_Software_Devel/64   8/24/12 3:03p shivak
 * FWRAA-424: Correcting interface file missmatch bw raaga and magnum
 * 
 * Hydra_Software_Devel/63   8/23/12 12:15a rshijith
 * FWRAA-424: Code modification to support VP8 encoder in raaga.
 * 
 * Hydra_Software_Devel/62   8/21/12 11:32a ananthan
 * FWRAA-464: iSAC initial code check in.Added user configuration
 * structure
 * 
 * Hydra_Software_Devel/61   8/9/12 2:38p dlimaye
 * SW7425-2805: Add AMR-NB and AMR-WB codecs support Adding userconfig for
 * AMRWB decoder
 * 
 * Hydra_Software_Devel/60   8/9/12 2:30p shivak
 * FWRAA-305 : added I4x4 mode support as configurable
 * 
 * Hydra_Software_Devel/59   7/20/12 11:58a ramanatm
 * SW7425-3204:[7425] BTSC Soft-Encoder Fails Stereo Separation
 * Measurement
 * 
 * Hydra_Software_Devel/58   7/20/12 1:28a ananthan
 * SW7425-3490: Adding initial frame preservation for MS-11 certification
 * 
 * Hydra_Software_Devel/57   7/16/12 4:05p dlimaye
 * SW7425-2805: Add AMR-NB and AMR-WB codecs support Added new codecs: AMR
 * and AMRWB decoder/encoder
 * 
 * Hydra_Software_Devel/56   7/4/12 7:45p ramanatm
 * FWRAA-454: [7425] user config update for ARIB style downmix
 * 
 * Hydra_Software_Devel/55   7/3/12 11:39a dlimaye
 * FWRAA-453: Modify the LPCM userconfig to reduce it's size Removed the
 * user defined downmix tables from userconfig
 * 
 * Hydra_Software_Devel/54   6/28/12 3:50p ramanatm
 * SW7425-3116: [7425] Adding src support from 48 khz to 32khz in aache
 * encoder
 * 
 * Hydra_Software_Devel/53   6/19/12 3:16a gprasad
 * SW7425-2952: DSP directory structure flattening
 * 
 * Hydra_Software_Devel/52   6/1/12 12:02p dlimaye
 * FWRAA-442: Add support for BD-LPCM decoder Added support in IDS,
 * decoder and host interface
 * 
 * Hydra_Software_Devel/51   5/28/12 4:45p ramanatm
 * FWRAA-434: [7425] BTSC merge the Mono channel changes.
 * 
 * Hydra_Software_Devel/50   5/23/12 1:58p ramanatm
 * SW7425-2889:[7425] DTS-CD Interface change for IDS
 * 
 * Hydra_Software_Devel/49   5/11/12 3:37p dlimaye
 * FWRAA-433: Adding G722 encoder to raaga Adding bdsp changes and
 * sys_top_raaga changes
 * 
 * Hydra_Software_Devel/48   5/8/12 3:11p dlimaye
 * FWRAA-433: Adding G722 encoder to raaga
 * 
 * Hydra_Software_Devel/47   5/7/12 11:05p ramanatm
 * FWRAA-434:[7425] Raaga BTSC Encode to bit exactness and sync with
 * latest raptor code
 * 
 * Hydra_Software_Devel/46   5/4/12 5:01p shivak
 * SW7429-143 :  change the comment to C style
 * 
 * Hydra_Software_Devel/45   5/4/12 10:04a rshijith
 * SW7429-143: Added userconfig enum eVarModeDecsOptEnable and
 * SubPelEnable to enable this feature.
 * 
 * Hydra_Software_Devel/44   4/12/12 12:28p gprasad
 * SW7425-2700: Adding Per frame STC snapshot support - currently disabled
 * 
 * Hydra_Software_Devel/43   4/11/12 8:03p gprasad
 * SW7425-2700: Adding Per frame STC snapshot support - currently disabled
 * 
 * Hydra_Software_Devel/42   4/11/12 5:50p ramanatm
 * FWRAA-420: [7425] Adding downmix scale index
 * 
 * Hydra_Software_Devel/41   3/30/12 5:25p gprasad
 * SW7435-71: Modifying the default user config for SRS TVOL
 * 
 * Hydra_Software_Devel/40   3/30/12 12:59p gprasad
 * FWRAA-418: Add prefilter stage to AVL for loudness level equalization
 * Userconfig value i32DTFPCNT should be mentioned in m-sec and not in
 * seconds
 * 
 * Hydra_Software_Devel/39   3/30/12 12:34a gprasad
 * SW7425-2786: BDSP changes for adding MLP Passthru support
 * 
 * Hydra_Software_Devel/38   3/28/12 10:48a dlimaye
 * SW7425-2736: Upgrade 3DSurround from stereo widening algorithm to full
 * 3DSurround Adding comments explaining the virtual speaker positions
 * each of the 3D surround mode represents
 * 
 * Hydra_Software_Devel/37   3/22/12 7:01p gprasad
 * FWRAA-382: Broadcom 3d surround changes
 * 
 * Hydra_Software_Devel/36   3/22/12 6:50p gprasad
 * FWRAA-399: DTS LBR SUPPORT
 * 
 * Hydra_Software_Devel/35   3/16/12 2:38p ramanatm
 * SW7425-2378:[7425] Add support for Monkey Audio Codec (MAC)
 * 
 * Hydra_Software_Devel/34   2/23/12 5:33p gprasad
 * FWRAA-397: removing joint stereo control from mp3 encoder usercfg
 * structure
 * 
 * Hydra_Software_Devel/33   2/3/12 12:23p ramanatm
 * SW7425-1752:[7425] Add support for FLAC media file format
 * 
 * Hydra_Software_Devel/32   1/31/12 2:48p gprasad
 * SW7425-500: Updating the user config defaults for G.7xx decoders
 * 
 * Hydra_Software_Devel/31   1/27/12 1:32p ashoky
 * FWRAA-377: Merging h264 soft dsp video encoder to mainline.
 * 
 * Hydra_Software_Devel/30   1/20/12 3:25p gprasad
 * FWRAA-377 : Merging h264 video encoder to mainline
 * 
 * Hydra_Software_Devel/29   1/13/12 9:51p gprasad
 * FWRAA-342: Adding support for SPEEX AEC
 * 
 * Hydra_Software_Devel/28   1/6/12 5:09p gprasad
 * FWRAA-366:[7425] A85 Support
 * 
 * Hydra_Software_Devel/27   1/6/12 11:34a ashoky
 * SW7231-524: Adding changes for 6 channels PCMWAV
 * 
 * Hydra_Software_Devel/26   12/29/11 5:56p gprasad
 * FWRAA-370: G723.1 codec development on Raaga platform
 * 
 * Hydra_Software_Devel/25   12/15/11 5:57p ashoky
 * SW7231-500: Changes to support IEEE1394LPCM. Making union of
 * userconfig.
 * 
 * Hydra_Software_Devel/24   12/14/11 10:57p ananthan
 * FWRAA-366: Adding support in MPEG decoder to support A85
 * 
 * Hydra_Software_Devel/23   12/6/11 5:08p gprasad
 * FWRAA-330: Adding wma encoder support.
 * 
 * Hydra_Software_Devel/22   12/5/11 6:11p ramanatm
 * FWRAA-366:[7425] A85 Support
 * 
 * Hydra_Software_Devel/21   12/5/11 12:40p ashoky
 * FWRAA-330: WMA Encoder Development on Raaga.
 * 
 * Hydra_Software_Devel/20   11/8/11 1:43p ramanatm
 * FWRAA-348:[7425] Vorbis bringup
 * 
 * Hydra_Software_Devel/19   8/19/11 3:47p gprasad
 * FWRAA-327: Updates to g.729 decoder user config
 * 
 * Hydra_Software_Devel/18   8/17/11 12:51p gprasad
 * FWRAA-280: Syncing basemodules and Raaga host interface files
 * 
 * Hydra_Software_Devel/17   8/5/11 5:46p ananthan
 * SW7425-724: [7425] Add BTSC audio encoder support
 * 
 * Hydra_Software_Devel/16   8/1/11 3:12p gprasad
 * FWRAA-327: Add support for G.711/G.726/G.729 codecs
 * 
 * Hydra_Software_Devel/15   7/26/11 2:34p gprasad
 * SWDTV-6761: Add StudioSound support
 * 
 * Hydra_Software_Devel/14   7/12/11 5:32p gprasad
 * SW7422-372: Add macros in BDSP to help simplify the FW Settings/Status
 * interfaces
 * 
 * Hydra_Software_Devel/13   6/24/11 10:41p ashoky
 * SW7425-499: [7425] Adding to mainline
 * 
 * Hydra_Software_Devel/12   6/22/11 11:28a gprasad
 * SW7425-756: Audio descriptor support in mpeg decoder
 * 
 * Hydra_Software_Devel/11   6/15/11 12:53a gprasad
 * SW7422-357: Need to add new FW mixer support for APE/DSP - Adding mix
 * balance element ot mixer's user cfg
 * 
 * Hydra_Software_Devel/10   6/14/11 9:45p ashoky
 * SW7425-462: [7425]Adding metadata entries and algo info entries
 * 
 * Hydra_Software_Devel/9   6/7/11 6:51p gprasad
 * SW7422-357: Need to add new FW mixer support for APE/DSP
 * 
 * Hydra_Software_Devel/8   6/7/11 6:50p gprasad
 * SW7422-357: Need to add new FW mixer support for APE/DSP
 * 
 * Hydra_Software_Devel/7   5/27/11 4:45p gprasad
 * SW7422-389: [7422] Add Dolby MS11 support
 * 
 * Hydra_Software_Devel/6   5/18/11 5:45p gprasad
 * SW7422-389: [7422] Add Dolby MS11 support
 * 
 * Hydra_Software_Devel/5   5/17/11 3:07p ashoky
 * SW7425-477: [7425] Adding user config struct for MP3encoder.
 * 
 * Hydra_Software_Devel/4   5/12/11 5:31p gprasad
 * SW7422-389: [7422] Add Dolby MS11 support
 * 
 * Hydra_Software_Devel/MS11_DEVEL_BRANCH/1   5/11/11 11:13a gprasad
 * SW7422-389: [7422] Add Dolby MS11 support
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
 * Hydra_Software_Devel/4   3/5/11 4:42a srajapur
 * SW7422-316 : [7422] Merging Phase 1.0 release firmware code base to
 * BASE DSP code base
 * 
 * Hydra_Software_Devel/3   1/18/11 4:54a gautamk
 * SW7422-191: Implementing code review comments.
 * 
 * Hydra_Software_Devel/2   1/18/11 4:23a gautamk
 * SW7125-766: Added code for review comments.
 * 
 * Hydra_Software_Devel/1   1/13/11 5:37a gautamk
 * SW7422-191:[7422]Adding DSP support for Audio decode in Basemodule  It
 * includes - DSP Booting - Open time Download :  - Start Time Audio
 * Decode algorithm download  - Message buffer interaction with DSP:  -
 * Synchronized Interrupt:  - Async interrupts:  - Config Commands for
 * Decode/TSM/IDS: - Status Buffer for Decode/TSM/IDS:  - Start AC3 Audio
 * Decode:  - Pause/Resume Command:  - Frame Advance command:  - Device
 * level interrupt
 * 
 * Hydra_Software_Devel/12   11/24/10 6:32p ananthan
 * FWRAA-182 : [35230] Compliance to new reference custom voice code.
 * 
 * Hydra_Software_Devel/11   10/26/10 12:20p ananthan
 * FWRAA-199, SW35230-432: [35230] Changing user config for Dolby Pulse to
 * meet EU requirements. Changing default value for
 * i32TruSurrndHDByPassGain  for SRS Tru Surround HD
 * 
 * Hydra_Software_Devel/10   10/20/10 5:03p ananthan
 * FWRAA-191, SW35230-432, FWRAA-189, FWRAA-194: [35230] Adding userconfig
 * and status buffer structures for SRS Studio, ADPCM and G726
 * 
 * Hydra_Software_Devel/9   10/12/10 5:50p ananthan
 * FWRAA-168, FWRAA-169, FWRAA-170: [35230] Adding support for custom DBE,
 * custom AVLP and custom ACF
 * 
 * Hydra_Software_Devel/8   9/28/10 3:04p ashoky
 * FWRAA-182: [35230] New change request from LG for clearvoice.
 * 
 * Hydra_Software_Devel/7   8/18/10 5:26p gprasad
 * FWRAA-140: [35230] DD convert support
 * 
 * Hydra_Software_Devel/6   8/12/10 7:27p psmitha
 * FWRAA-140: [35230] MS10 Support. Adding metadata log enable flag.
 * 
 * Hydra_Software_Devel/5   8/11/10 1:50p ashoky
 * SW35230-775 : [35230] Adding converter and DTS encode support.
 * 
 * Hydra_Software_Devel/4   8/6/10 11:35a ashoky
 * FWRAA-141: [35230] changing the user config parameter according to new
 * set.
 * 
 * Hydra_Software_Devel/3   8/3/10 7:19p gprasad
 * FWRAA-133: [35230] Merging all the host interface changes from phase
 * 6p0 to phase 7p5 take 2 into main line
 * 
 * Hydra_Software_Devel/Raaga_Phase7p5tk2_Branch/4   8/2/10 8:58p ashoky
 * FWRAA-141: [35230]adding new user config for CV3.
 * 
 * Hydra_Software_Devel/Raaga_Phase7p5tk2_Branch/3   7/27/10 11:49a gprasad
 * FWRAA-133: [35230] Merging all the host interface changes from phase
 * 6p0 to phase 7p5 take 2 into Raaga code base
 * 
 * Hydra_Software_Devel/Raaga_Phase7p5tk2_Branch/2   7/26/10 8:33p gprasad
 * FWRAA-133: [35230] Merging all the host interface changes from phase
 * 6p0 to phase 7p5 take 2 into Raaga code base
 * 
 * Hydra_Software_Devel/Raaga_Phase7p5tk2_Branch/1   7/20/10 2:28p gprasad
 * FWRAA-133: [35230] Merging all the host interface changes from phase
 * 6p0 to phase 7p5 take 2 into Raaga code base
 * 
 * Hydra_Software_Devel/2   5/13/10 11:53a dlimaye
 * FW35230-31: [35230] Add support for TSM in CIT Combined the interframe
 * and userconfig structures of datasync and TSM
 * 
 * Hydra_Software_Devel/1   4/30/10 1:11p dlimaye
 * FW35230-30: [35230] Checkin CIT files in base4 Move the CIT files from
 * label RAAGA_FW35230-29_20100419 in base3 folder to the base4 folder
 * 
 * Hydra_Software_Devel/RAAGA_EMULATION/4   3/30/10 11:18a dlimaye
 * FW35230-28: [35230] Checkin CIT files for BM
 * 
 * Hydra_Software_Devel/RAAGA_EMULATION/3   3/23/10 2:10p dlimaye
 * FWRAA-63: [35230] Modify CIT files: Add endofalgos in the algoid enum
 * and add the file brap_fwuserconfig_priv.h
 * 
 * Hydra_Software_Devel/RAAGA_EMULATION/2   3/12/10 11:01a dlimaye
 * FWRAA-54: [35230] Unify CIT files between magnum and Raaga vobs
 * 
 * Hydra_Software_Devel/RAAGA_EMULATION/1   9/11/09 1:46p dlimaye
 * FW35230-2:[3513]Check-in the initial Raaga CIT files + FW binaries to
 * magnum
 * Checking in CIT files in the branch RAAGA_EMULATION
 * 
 ***************************************************************************/

#ifndef BRAAGA_FW_USER_CONFIG_H
#define BRAAGA_FW_USER_CONFIG_H

#include "bdsp_raaga_fw.h"

#define  DDP_DEC_GBL_MAXPCMCHANS					6
#define  DTSHD_DEC_MAXCHANS							8

/* BDSP_AudioType */
#define BDSP_Raaga_MpegSettings 					BDSP_Raaga_Audio_MpegConfigParams

#ifdef BDSP_MS10_SUPPORT
	#define BDSP_Raaga_AacSettings 					BDSP_Raaga_Audio_DolbyPulseUserConfig
	#define BDSP_Raaga_AacAdtsSettings 				BDSP_Raaga_Audio_DolbyPulseUserConfig
	#define BDSP_Raaga_AacLoasSettings 				BDSP_Raaga_Audio_DolbyPulseUserConfig
	#define BDSP_Raaga_AacSbrAdtsSettings 			BDSP_Raaga_Audio_DolbyPulseUserConfig
	#define BDSP_Raaga_AacSbrSettings 				BDSP_Raaga_Audio_DolbyPulseUserConfig
	#define BDSP_Raaga_AacSbrLoasSettings 			BDSP_Raaga_Audio_DolbyPulseUserConfig
#else
	#define BDSP_Raaga_AacSettings 					BDSP_Raaga_Audio_AacheConfigParams
	#define BDSP_Raaga_AacAdtsSettings 				BDSP_Raaga_Audio_AacheConfigParams
	#define BDSP_Raaga_AacLoasSettings 				BDSP_Raaga_Audio_AacheConfigParams
	#define BDSP_Raaga_AacSbrAdtsSettings 			BDSP_Raaga_Audio_AacheConfigParams
	#define BDSP_Raaga_AacSbrSettings 				BDSP_Raaga_Audio_AacheConfigParams
	#define BDSP_Raaga_AacSbrLoasSettings 			BDSP_Raaga_Audio_AacheConfigParams
#endif

#define BDSP_Raaga_Ac3Settings 						BDSP_Raaga_Audio_DDPMultiStreamConfigParams
#define BDSP_Raaga_Ac3PlusSettings 					BDSP_Raaga_Audio_DDPMultiStreamConfigParams
#define BDSP_Raaga_LpcmBdSettings 					BDSP_Raaga_Audio_LpcmUserConfig
#define BDSP_Raaga_LpcmHdDvdSettings 				BDSP_Raaga_Audio_LpcmUserConfig
#define BDSP_Raaga_DtshdSettings 					BDSP_Raaga_Audio_DtsHdConfigParams
#define BDSP_Raaga_LpcmDvdSettings 					BDSP_Raaga_Audio_LpcmUserConfig
#define BDSP_Raaga_WmaStdSettings 					BDSP_Raaga_Audio_WmaConfigParams
#define BDSP_Raaga_MlpSettings 						BDSP_Raaga_Audio_MlpConfigParams
#define BDSP_Raaga_PcmSettings 						BDSP_Raaga_Audio_PassthruConfigParams
#define BDSP_Raaga_DtsLbrSettings 					BDSP_Raaga_Audio_DtslbrConfigParams
#define BDSP_Raaga_Ddp7_1Settings 					BDSP_Raaga_Audio_DecodeConfigParams
#define BDSP_Raaga_MpegMcSettings 					BDSP_Raaga_Audio_DecodeConfigParams
#define BDSP_Raaga_WmaProSettings 					BDSP_Raaga_Audio_WmaProConfigParams
#define BDSP_Raaga_LpcmDvdASettings 				BDSP_Raaga_Audio_DecodeConfigParams
#define BDSP_Raaga_DtsBroadcastSettings 			BDSP_Raaga_Audio_DtsHdConfigParams
#define BDSP_Raaga_PcmWavSettings 					BDSP_Raaga_Audio_PcmWavConfigParams
#define BDSP_Raaga_AmrSettings 						BDSP_Raaga_Audio_AmrConfigParams
#define BDSP_Raaga_DraSettings 						BDSP_Raaga_Audio_DraConfigParams
#define BDSP_Raaga_RealAudioLbrSettings 			BDSP_Raaga_Audio_RalbrConfigParams
#define BDSP_Raaga_AdpcmSettings 					BDSP_Raaga_Audio_AdpcmConfigParams
#define BDSP_Raaga_G711G726Settings		            BDSP_Raaga_Audio_G726ConfigParams
#define BDSP_Raaga_G729Settings 				    BDSP_Raaga_Audio_G729DecConfigParams
#define BDSP_Raaga_VorbisSettings 				    BDSP_Raaga_Audio_VorbisDecConfigParams
#define BDSP_Raaga_G723_1Settings                   BDSP_Raaga_Audio_G723_1DEC_ConfigParams
#define BDSP_Raaga_FlacSettings 				    BDSP_Raaga_Audio_FlacDecConfigParams
#define BDSP_Raaga_MacSettings 				        BDSP_Raaga_Audio_MacDecConfigParams
#define BDSP_Raaga_AmrwbSettings 					BDSP_Raaga_Audio_AmrwbdecConfigParams
#define BDSP_Raaga_iLBCSettings 					BDSP_Raaga_Audio_iLBCdecConfigParams
#define BDSP_Raaga_iSACSettings 					BDSP_Raaga_Audio_iSACdecConfigParams

/* BDSP_AudioProcessing */
#define BDSP_Raaga_DtsNeoSettings 					BDSP_Raaga_Audio_ProcessingConfigParams
#define BDSP_Raaga_AVLSettings 						BDSP_Raaga_Audio_AVLConfigParams
#define BDSP_Raaga_PLllSettings 					BDSP_Raaga_Audio_PL2ConfigParams
#define BDSP_Raaga_SrsXtSettings 					BDSP_Raaga_Audio_TruSurrndXTConfigParams
#define BDSP_Raaga_BbeSettings 						BDSP_Raaga_Audio_ProcessingConfigParams
#define BDSP_Raaga_SrcSettings 						BDSP_Raaga_Audio_SRCUserConfigParams
#define BDSP_Raaga_CustomVoiceSettings 				BDSP_Raaga_Audio_CustomVoiceConfigParams
#define BDSP_Raaga_AacDownmixSettings 				BDSP_Raaga_Audio_ProcessingConfigParams
#define BDSP_Raaga_DsolaSettings 					BDSP_Raaga_Audio_DsolaConfigParams
#define BDSP_Raaga_SrsHdSettings 					BDSP_Raaga_Audio_TruSurrndHDConfigParams
#define BDSP_Raaga_GenericPassThruSettings 			BDSP_Raaga_Audio_PassthruConfigParams
#define BDSP_Raaga_SrsTruVolumeSettings 			BDSP_Raaga_Audio_TruVolumeUserConfig
#define BDSP_Raaga_DolbyVolumeSettings 				BDSP_Raaga_Audio_DolbyVolumeUserConfig
#define BDSP_Raaga_AudysseyVolumeSettings 			BDSP_Raaga_Audio_ProcessingConfigParams
#define BDSP_Raaga_Brcm3DSurroundSettings 			BDSP_Raaga_Audio_Brcm3DSurroundConfigParams
#define BDSP_Raaga_FWMixerSettings 					BDSP_Raaga_Audio_ProcessingConfigParams
#define BDSP_Raaga_AudysseyABXSettings 				BDSP_Raaga_Audio_ProcessingConfigParams
#define BDSP_Raaga_DdreSettings 					BDSP_Raaga_Audio_DDReencodeConfigParams
#define BDSP_Raaga_Dv258Settings 					BDSP_Raaga_Audio_DV258ConfigParams
#define BDSP_Raaga_SrsCsdTdSettings 				BDSP_Raaga_Audio_SRS_CSDecTruDialogConfigParams
#define BDSP_Raaga_SrsEqHlSettings                  BDSP_Raaga_Audio_SRS_EqualizerHardLimiterConfigParams
#define BDSP_Raaga_CustomDbeSettings 				BDSP_Raaga_Audio_ProcessingConfigParams
#define BDSP_Raaga_CustomAcfSettings 				BDSP_Raaga_Audio_ProcessingConfigParams
#define BDSP_Raaga_CustomAvlpSettings 				BDSP_Raaga_Audio_AvlPConfigParams
#define BDSP_Raaga_GenCdbItbSettings 				BDSP_Raaga_Audio_GenCdbItbConfigParams
#define BDSP_Raaga_BtscEncoderSettings 				BDSP_Raaga_Audio_BtscEncoderConfigParams
#define BDSP_Raaga_Broadcom3DSurroundSettings       BDSP_Raaga_Audio_Broadcom3DSurroundConfigParams
#define	BDSP_Raaga_SpeexAecSettings			        BDSP_Raaga_Audio_SpeexAECConfigParams

/* BDSP_AudioEncode */
#define BDSP_Raaga_Mpeg1Layer3Settings  			BDSP_Raaga_Audio_Mpeg1L3EncConfigParams
#define BDSP_Raaga_Mpeg1Layer2Settings  			BDSP_Raaga_Audio_Mpeg1L2EncConfigParams
#define BDSP_Raaga_DTSEncodeSettings 				BDSP_Raaga_Audio_DtsBroadcastEncConfigParams
#define BDSP_Raaga_AacLcSettings 					BDSP_Raaga_Audio_AaclcEncConfigParams
#define BDSP_Raaga_AacHeSettings 					BDSP_Raaga_Audio_AacheEncConfigParams
#define BDSP_Raaga_Ac3EncodeSettings 				BDSP_Raaga_Audio_DDTranscodeConfigParams
#define BDSP_Raaga_SbcSettings 						BDSP_Raaga_Audio_SbcEncoderUserConfig
#define BDSP_Raaga_G711G726EncodeSettings		    BDSP_Raaga_Audio_G711_G726EncConfigParams
#define BDSP_Raaga_G729EncodeSettings 			    BDSP_Raaga_Audio_G729EncoderUserConfig
#define BDSP_Raaga_WmaEncodeSettings				BDSP_Raaga_Audio_WMAEncoderConfigParams
#define BDSP_Raaga_G723_1EncodeSettings             BDSP_Raaga_Audio_G723EncoderUserConfig
#define BDSP_Raaga_G722EncodeSettings 			      BDSP_Raaga_Audio_G722EncConfigParams
#define BDSP_Raaga_AmrEncodeSettings               BDSP_Raaga_Audio_AmrEncoderUserConfig
#define BDSP_Raaga_AmrwbEncodeSettings              BDSP_Raaga_Audio_AMRWBConfigParams
#define BDSP_Raaga_ILBCEncodeSettings               BDSP_Raaga_Audio_ILBCConfigParams
#define BDSP_Raaga_ISACEncodeSettings               BDSP_Raaga_Audio_ISACConfigParams

/* BDSP_VideoType */
#define BDSP_Raaga_RealVideo9Settings 				BDSP_Raaga_Video_DecodeConfigParams
#define BDSP_Raaga_VP6Settings 						BDSP_Raaga_Video_DecodeConfigParams
#define BDSP_Raaga_H264EncodeSettings 				BDSP_Raaga_Audio_sBH264_USER_CONFIG
#define BDSP_Raaga_VP8EncodeSettings              BDSP_Raaga_Audio_sBVP8_USER_CONFIG
/*
	Common Enum for Acmode
*/

typedef enum BDSP_Raaga_Audio_AcMode
{
    BDSP_Raaga_Audio_AcMode_eMode10 = 1, /*C*/
    BDSP_Raaga_Audio_AcMode_eMode20 = 2, /*L,R*/
    BDSP_Raaga_Audio_AcMode_eMode30 = 3, /*L,C,R*/
    BDSP_Raaga_Audio_AcMode_eMode21 = 4, /*L,R,S*/
    BDSP_Raaga_Audio_AcMode_eMode31 = 5, /*L,C,R,S*/
    BDSP_Raaga_Audio_AcMode_eMode22 = 6, /*L,R,LS,RS*/
    BDSP_Raaga_Audio_AcMode_eMode32 = 7, /*L,C,R,LS,RS*/
    BDSP_Raaga_Audio_AcMode_eMode33 = 8, /*L,C,R,LS,RS,CS*/
    BDSP_Raaga_Audio_AcMode_eMode32_BSDigital = 9,/*L,C,R,LS,RS*/
    BDSP_Raaga_Audio_AcMode_eModeLtRt=0x20,/*L,R*/
    BDSP_Raaga_Audio_AcMode_eModePLII_Movie = 0x21,/*L,C,R,LS,RS*/
    BDSP_Raaga_Audio_AcMode_eModePLII_Music = 0x22,/*L,C,R,LS,RS*/
    BDSP_Raaga_Audio_AcMode_eModePassiveMatrix = 0x23,/*L,R*/
    BDSP_Raaga_Audio_AcMode_eModeCSII = 0x24,/*L,C,R,Ls,Rs,Cs*/
    BDSP_Raaga_Audio_AcMode_eLAST,
    BDSP_Raaga_Audio_AcMode_eINVALID=0x7fffffff

}BDSP_Raaga_Audio_AcMode;

/*
     Default ConfigParams structure for decoder types.
*/
typedef struct BDSP_Raaga_Audio_DecodeConfigParams
{
    /* Number of output ports    */
    uint32_t 	   			ui32NumOutPorts;
    /* Output mode    */
    BDSP_Raaga_Audio_AcMode    eOutMode;
    /* This is channel matrix of size 8 where each index can take any values from 0 to 7
       Depending upon  the channel routing done      */
    uint32_t    ui32OutputChannelMatrix[BDSP_AF_P_MAX_CHANNELS];
}BDSP_Raaga_Audio_DecodeConfigParams;

/*
     Default ConfigParams structure for audio processing types.
*/
typedef struct BDSP_Raaga_Audio_ProcessingConfigParams
{
    /* Number of input/output channels     */
    uint32_t                    ui32NumChans;
    /* Default: 1;  1 -> Process, 0 ->  Bypass */
    uint32_t                    ui32Enable;
}BDSP_Raaga_Audio_ProcessingConfigParams;


/*
     Default ConfigParams structure for Video decoder types.
*/
typedef struct BDSP_Raaga_Video_DecodeConfigParams
{
    /* Dummy variable     */
    uint32_t                    ui32Dummy;
}BDSP_Raaga_Video_DecodeConfigParams;

/*
   This data structure defines AC-3/DDP decoder user configuration parameters 
*/

typedef struct  BDSP_Raaga_Audio_Ac3UserOutput
{
	 /* This is LFE ON/OFF flag and can take  two values 0 or 1 
	  default value for this field is 0 */
	 int32_t i32OutLfe;                         
	 /* i32OutMode =7 default value;*/
	 int32_t i32OutMode;
	 /* preferred stereo mode  i32StereoMode = 0 is default value */
	 int32_t i32StereoMode;  
	 /* dual mono downmix mode i32DualMode = 0 is default value */
	 int32_t i32DualMode;
	 /* karaoke capable mode i32Kmode = 3 */
	 int32_t i32Kmode;	
	 /* This i32ExtDnmixEnabled flag which can take  two values 0 or 1 based on disable/enable option
	  default value for this i32ExtDnmixEnabled field is 0 */
	 int32_t i32ExtDnmixEnabled;				
	 int32_t i32ExtDnmixTab[DDP_DEC_GBL_MAXPCMCHANS][DDP_DEC_GBL_MAXPCMCHANS]; 
	 /* This i32ExtKaraokeEnabled flag which can take  two values 0 or 1 based on disable/enable option
	  default value for this i32ExtKaraokeEnabled field is 0 */
	 int32_t i32ExtKaraokeEnabled;				
	 int32_t i32Extv1Level;						
	 int32_t i32Extv1Pan;						
	 int32_t i32Extv2Level;						
	 int32_t i32Extv2Pan;						
	 int32_t i32ExtGmLevel;					    
	 int32_t i32ExtGmPan;						
 	 /*This is channel matrix of size 8 where each index can take any values from 0 to 7
	   Depending upon  the channel routing done
	 */
	 uint32_t	ui32OutputChannelMatrix[BDSP_AF_P_MAX_CHANNELS];
}BDSP_Raaga_Audio_Ac3UserOutput;

typedef struct  BDSP_Raaga_Audio_Ac3ConfigParams
{
	/* p_user_cfg->i32CompMode = 2;*/
	 int32_t					 i32CompMode;  
	 int32_t					 i32PcmScale;  
	/* p_user_cfg->i3DynScaleHigh = 0x7FFFFFFF;	 0x0 to  0x7FFFFFFF in Q1.31 format */
	 int32_t					 i32DynScaleHigh;    
	/* p_user_cfg->i32DynScaleLow = 0x7FFFFFFF;	 0x0 to  0x7FFFFFFF in Q1.31 format */
	 int32_t					 i32DynScaleLow;    
	/*  This value "i32NumOutPorts" can be set to 1 or 2 based on output ports */
	 int32_t					 i32NumOutPorts;     
    /* This value is used to enable/disable stream dialog normalization value.  0 is for Disable and 1 is for Enable
	   Default is Enable
	 */
	 int32_t					 i32StreamDialNormEnable;
	/*	This value indicates how far the average dialogue level is below digital 100 percent. Valid
		values are 1-31. The value of 0 is reserved and it means dialog normalization is turned off. The values of 1 to 31 are interpreted as -1 dB to -31dB 
		with respect to digital 100 percent. If the reserved value of 0 is received, the decoder will not do any dialog normalization, 
		Default value is 0 
	*/
	int32_t						i32UserDialNormVal;
	 /* These are user config parameters required from user  */
	 BDSP_Raaga_Audio_Ac3UserOutput  sUserOutputCfg[2];
	 

}BDSP_Raaga_Audio_Ac3ConfigParams;



/*
   This data structure defines MPEG-1 L1/L2/L3 decoder user configuration parameters 
*/

typedef struct BDSP_Raaga_Audio_MpegConfigParams
{
	BDSP_AF_P_DecoderType    	eDecoderType;    /* Decoder Type indicates whether the context is primary (default) or secondary */
	uint32_t	ui32OutMode;					 /* outputmode */
	uint32_t	ui32LeftChannelGain;			 /* In 1Q31 format - not supported */
	uint32_t	ui32RightChannelGain;			 /* In 1Q31 format - not supported */
	uint32_t	ui32DualMonoMode;	
	int32_t     i32InputVolLevel;               /* Used in the case of A85. Please set input = output for non-A85 cases. Value is in dB. For EBU Set input = -23, For ATSC, set input = -14*/
	int32_t     i32OutputVolLevel;              /* Used in the case of A85. Please set input = output for non-A85 cases. Value is in dB. For EBU Set output= -23  For ATSC, set output= -20*/
	uint32_t 	ui32AncDataParseEnable;			/* Flag to enable MPEG Ancillary Data Parsing: Default 0 (Disable)*/
	uint32_t	ui32AncDataDramCircAddr;		/* Structure : BDSP_AF_P_sDRAM_CIRCULAR_BUFFER Dram address to get the base, end, read, write and wrap address for DRAM Circular Buffer to write MPEG Ancillary Data: Default 0 */
	uint32_t	ui32OutputChannelMatrix[BDSP_AF_P_MAX_CHANNELS];
}BDSP_Raaga_Audio_MpegConfigParams;	


/*
   This data structure defines WMA-9 decoder user configuration parameters 
*/

typedef struct  BDSP_Raaga_Audio_WmaConfigParams
{
	uint32_t	ui32OutputMode;

	/*These 2 variables are used to resolve problems in comparision in RTL platform for Certification */
	uint32_t	decodeOnlyPatternFlag;

	uint32_t	decodePattern;

	uint32_t	ui32OutputChannelMatrix[BDSP_AF_P_MAX_CHANNELS]; /* Default Channel Matrix = {0, 1, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF } */
}BDSP_Raaga_Audio_WmaConfigParams;

/* 1)	D_DRC_HIGH: No scaling is applied.  Content is played in original form.
2)	D_DRC_MED: drc_frame_scale_factor is applied along with a scale factor to make 
			   the output rms of the content rms_amplitude_target_dB, 
			   and the peak of the content be peak_amplitude_target_med_dB.
3)	D_DRC_LOW: drc_frame_scale_factor is applied along with a scale factor to make 
				the output rms of the content rms_amplitude_target_dB, and the peak 
				of the content be peak_amplitude_target_low_dB.
*/
typedef enum BDSP_Raaga_Audio_eDrcSetting
{ 
	BDSP_Raaga_Audio_eDrcSetting_High=0,
    BDSP_Raaga_Audio_eDrcSetting_Med, 
    BDSP_Raaga_Audio_eDrcSetting_Low,
	BDSP_Raaga_Audio_eDrcSetting_eLAST,
    BDSP_Raaga_Audio_eDrcSetting_eINVALID=0x7fffffff

}BDSP_Raaga_Audio_eDrcSetting; 

/*
   This data structure defines WMA-PRO decoder user configuration parameters 
*/
typedef struct BDSP_Raaga_Audio_WmaProUserConfig
{
	uint32_t 					ui32DRCEnable;			/* Default 0 , range 0 and 1*/
	BDSP_Raaga_Audio_eDrcSetting		eDRCSetting;			/* Default 0 */
	int32_t						i32RmsAmplitudeRef;  
	int32_t						i32PeakAmplitudeRef;
	int32_t						i32DesiredRms;			/* desired rmsDb for normalization */
	int32_t						i32DesiredPeak;			/* desired peakDb for normalization */
	uint32_t	ui32OutMode;	/* Output channel configuration */
	uint32_t	ui32OutLfe;		/* Output LFE on/off */
	uint32_t	ui32Stereomode;	/* Stereo mode - Auto=0/ LtRt=1 /LoRo=2 */
	uint32_t	ui32OutputChannelMatrix[BDSP_AF_P_MAX_CHANNELS]; /* Default Channel Matrix = {0, 1, 2, 3, 4, 5, 0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF } */

}BDSP_Raaga_Audio_WmaProUserConfig;

typedef struct BDSP_Raaga_Audio_WmaProConfigParams
{
	uint32_t 						ui32NumOutports;	/* 1 -> Only Multichannel out [number of outmodes to be supported] ,  2 -> Stereo out + Multichannel */
	BDSP_Raaga_Audio_WmaProUserConfig	sOutputCfg[2];		/* User Config for Multichanel or Stereo Output */
	uint32_t						ui32UsageMode;		/* Decode =0, Passthru =1,SimulMode =2*/

}BDSP_Raaga_Audio_WmaProConfigParams;


/*
   This data structure defines AACHE multi-channel decoder user configuration parameters 
*/

typedef struct  BDSP_Raaga_Audio_AacheUserConfig
{
	int32_t 	i32OutLfe;											/* Default = 0, Output LFE channel present */
	int32_t 	i32OutMode;											/* Default = 2, Output channel configuration */
	int32_t 	i32DualMode;										/* Default = 0, Dual mono reproduction mode */
	int32_t 	i32ExtDnmixEnabled;									/* Default = 0, Enable external downmix */
	int32_t 	i32ExtDnmixTab[6][6];								/* Default = 0, External downmix coefficient table */
	uint32_t	ui32OutputChannelMatrix[BDSP_AF_P_MAX_CHANNELS];

} BDSP_Raaga_Audio_AacheUserConfig;

typedef struct  BDSP_Raaga_Audio_AacheConfigParams
{
	int32_t							i32NumOutPorts;		   			/* Default = 1, 1 -> Multichannel out, 2 -> Multichannel + Stereo out */
	int32_t							i32DownmixType;					/* Default = 0, 0 -> BRCM downmix, 1 -> ARIB downmix */
	int32_t							i32AribMatrixMixdownIndex;		/* Default = 0, ARIB matrix mixdown index which can take integer values from 0 to 3 */
	uint32_t						ui32DrcGainControlCompress;		/* Default = 0x40000000, Constant used in the DRC calculation, typically between 0 and 1, in Q2.30 format */
	uint32_t						ui32DrcGainControlBoost;		/* Default = 0x40000000, Constant used in the DRC calculation, typically between 0 and 1, in Q2.30 format */

	/* This parameter ui32DrcTargetLevel is no longer in use, use i32OutputVolLevel instead */
	uint32_t			    		ui32DrcTargetLevel;				/* Default = 127, Target level desired by the host */
	BDSP_Raaga_Audio_AacheUserConfig		sUserOutputCfg[2];

																	/* Boost of 6dB, specific to Buffalo */
	int32_t							i32PcmBoost6dB;					/* Default = 0, 0 -> FALSE, 1 -> TRUE */

																	/* Attenuation of 4.75dB, specific to DDCO certification */
    int32_t                         i32PcmBoostMinus4p75dB;			/* Default = TRUE for LG branch and Default = FALSE for others, 0 -> FALSE, 1 -> TRUE */

	uint32_t						ui32FreqExtensionEnable;		/* Default = 0, 0->disable freq extension for high sample rate, 1->Enable Frequency extension for high sample rates */

	uint32_t						ui32SbrUserFlag;				/* default = 0, 0-> disable SBR processing, 1-> Enable SBR processing/Upsample, This flag is used currently for DVD only */ 

	int32_t                         i32InputVolLevel;               /* in dB */
    int32_t                         i32OutputVolLevel;              /* in dB */
    uint32_t                        ui32DownmixCoefScaleIndex;      /* Default = 50,  0 -> 0dB, 1 -> -0.5dB, 2 -> -1dB, ... , 23 -> -11.5dB, 24 -> -12dB; all values beyond 24 map to 50 */
                                                                    /* 50 -> Decoder default settings (existing normalization) */
} BDSP_Raaga_Audio_AacheConfigParams;


typedef struct BDSP_Raaga_Audio_SpeexAECConfigParams
{
    /* Gain resolution to be used in the preprocessor 0 = Bark Mode (Faster), 1 = Linear mode (Better but much slower). Default = 0 */
    uint32_t ui32GainResolution;

} BDSP_Raaga_Audio_SpeexAECConfigParams;


/* 
	MPEG1-Layer-2 encoder user configuration structure 
*/

typedef struct BDSP_Raaga_Audio_Mpeg1L2EncConfigParams
{
	uint32_t	ui32bitRateIdx;
	/* This differs from layer-3
	bitRateIdx		bit-rate
	[0]				0;
	[1]				32;
	[2]				48;
	[3]				56;
	[4]				64;
	[5]				80;
	[6]				96;
	[7]				112;
	[8]				128;
	[9]				160;
	[10]			192;
	[11]			224;
	[12]			256;
	[13]			320;
	[14]			384;
	*/

	uint32_t ui32AddCRCProtect;/* not supported */
	/*
	addCRCProtect	Implication
		0			CRC protect off
		1			CRC protect on
	*/

	uint32_t ui32Ext; /* Controls private bit setting in the header */
	/*				
	ext		Implication
	0		=> not Set 
	1		=> Set
	*/

	uint32_t ui32jStereoControl; /* Joint-Stereo Control */
	/*
	Layer-2 does not have provision for m/s stereo. The user can set either 0,1 or 3
	jStereoControl	Implication	
	0				No joint stereo (ie: no intensity or m/s stereo )
	1				Allow intensity stereo
	2				Allow m/s stereo
	3				Allow intensity and m/s stereo
	4				Reserved
	5				Reserved
	6				Reserved
	7				Reserved
	*/

	uint32_t ui32Copyright;	/* Copyright bit setting in the header */
	
	uint32_t ui32Original;	/* Original bit setting in the header */

	uint32_t ui32BitsPerSampleIdx;	/* number of bits per PCM sample */
			/* The no of bits per sample is treated as 16 + bitsPerSampleIdx */
	
	uint32_t ui32SampleRateIdx;
	/*
	sampleRateIdx	Sample-Rate
	[0]				44100;
	[1]				48000;
	[2]				32000;
	*/

	uint32_t	ui32Emphasis;	 /* not supported */
	/*
	emphasis	Implication
	0			none
	1			50/15 microseconds
	2			reserved
	3			CCITT J.17
	*/

	uint32_t ui32InputMode;
	/*
	0 stereo
	1 reserved
	2 dual channel
	3 mono
	*/

}BDSP_Raaga_Audio_Mpeg1L2EncConfigParams;

/*
   This data structure defines DTS-5.1 encoder user configuration parameters 
*/


typedef struct  BDSP_Raaga_Audio_DtsEncConfigParams
{
	uint32_t ui32Amode;				/* Audio Coding Mode 2 channel, 5 channel etc. */
	uint32_t ui32Sfreq;				/* Sampling Frequency */
	uint32_t ui32Dynf;				/* Embedded Dynamic Range Flag */
	uint32_t ui32Rate;				/* Bit Rate of the Encoder output */
	uint32_t ui32Mix;				/* Downmix to stereo enable Flag */
	uint32_t ui32Timef;				/* Embedded Time Stamp Flag */
	uint32_t ui32Auxf;				/* Auxiliary Data Flag */
	uint32_t ui32Hdcd;				/* Source in HDCD Format */
	uint32_t ui32Sumf;				/* Front Sum Difference Encode Flag */
	uint32_t ui32Sums;				/* Surround Sum Difference Encode Flag */
	uint32_t ui32Lfe;				/* Low Frequency Effect Flag */
	uint32_t ui32Dtses;				/* DTS Extended Surround Enable Format */
	uint32_t ui32Pcmr;				/* Source Input PCM Resolution */
	uint32_t ui32ExtAudio;			/* Extension Audio Present Flag */
	uint32_t ui32ExtAudioId;		/* Type of the Extension Audio like XXCH, XBR, X96 etc */
	uint32_t ui32Vernum;			/* Indicates the revision status of encoder software */
	uint32_t ui32DialNorm;			/* Dialogue Normalization */
	uint32_t ui32Joinx;				/* Joint Intensity Coding Enable Flag */
	uint32_t ui32Cpf;				/* Bit Stream CRC Enable Flag */
	uint32_t ui32Aspf;				/* Indicates the frequency with which DSYNC occurs in the stream */
	uint32_t ui32Ftype;				/* Frame type whether normal or termination frame */
	uint32_t ui32Chist;				/* Copy History.  History of the source material */
	uint32_t ui32Range;				/* Dynamic Range Coefficient */
	uint32_t ui32Addmf;				/* Auxiliary Dynamic Downmix Flag */
}BDSP_Raaga_Audio_DtsEncConfigParams;




/*
   This data structure defines stereo AAC-LC encoder user configuration parameters 
*/

typedef struct  BDSP_Raaga_Audio_AaclcEncConfigParams
{
	uint32_t ui32Acmod;				/* Audio Coding Mode 2 channel, 5 channel etc.*/
	uint32_t ui32SampFreq;			/* Sampling Frequency */
	uint32_t ui32BitRate;			/* Bit Rate of the encoder */
/* This is flag which is used by AAC-LC encoder to decide whether PCM  data is coming directly or it is decimated by SBR module */
	uint32_t ui32SBR_PresentFlag;	
}BDSP_Raaga_Audio_AaclcEncConfigParams;


/*
	Bit rates supported in AAC encoder.
*/
typedef	enum	BDSP_Raaga_Audio_AacEncodeBitRate
{
	BDSP_Raaga_Audio_AacEncodeBitRate_e16kbps = 16000,
	BDSP_Raaga_Audio_AacEncodeBitRate_e20kbps = 20000,
	BDSP_Raaga_Audio_AacEncodeBitRate_e32kbps = 32000,
	BDSP_Raaga_Audio_AacEncodeBitRate_e40kbps = 40000,
	BDSP_Raaga_Audio_AacEncodeBitRate_e44kbps = 44100,
	BDSP_Raaga_Audio_AacEncodeBitRate_e48kbps = 48000,
	/* Below bit rates are supported only when SBR is off */
	BDSP_Raaga_Audio_AacEncodeBitRate_e50kbps = 50000,
	BDSP_Raaga_Audio_AacEncodeBitRate_e52kbps = 52000,
	BDSP_Raaga_Audio_AacEncodeBitRate_e64kbps = 64000,
	BDSP_Raaga_Audio_AacEncodeBitRate_e100kbps = 100000,
	BDSP_Raaga_Audio_AacEncodeBitRate_e128kbps = 128000,
	BDSP_Raaga_Audio_AacEncodeBitRate_e200kbps = 200000,
	BDSP_Raaga_Audio_AacEncodeBitRate_e224kbps = 224000,
	BDSP_Raaga_Audio_AacEncodeBitRate_e256kbps = 256000,
	BDSP_Raaga_Audio_AacEncodeBitRate_e320kbps = 320000,
	BDSP_Raaga_Audio_AacEncodeBitRate_eLast,
	BDSP_Raaga_Audio_AacEncodeBitRate_eInvalid = 0x7fffffff
}BDSP_Raaga_Audio_AacEncodeBitRate;

typedef enum
{
  BDSP_Raaga_Audio_AacEncodeComplexity_Low = 0,
  BDSP_Raaga_Audio_AacEncodeComplexity_Medium_1,
  BDSP_Raaga_Audio_AacEncodeComplexity_Medium_2,
  BDSP_Raaga_Audio_AacEncodeComplexity_High,
  BDSP_Raaga_Audio_AacEncodeComplexity_eInvalid = 0x7fffffff
}BDSP_Raaga_Audio_AacEncodeComplexity;


typedef enum
{
    BDSP_Raaga_Audio_AacEncodeMono_Left      = 0,
    BDSP_Raaga_Audio_AacEncodeMono_Right     = 1,
    BDSP_Raaga_Audio_AacEncodeMono_Mix       = 2,
    BDSP_Raaga_Audio_AacEncodeMono_DualMono  = 3
}BDSP_Raaga_Audio_AacEncodeMonoChannelSelect;

typedef enum
{
    BDSP_Raaga_Audio_AacEncodeAdtsMpeg4 = 0,
    BDSP_Raaga_Audio_AacEncodeAdtsMpeg2 = 1
}BDSP_Raaga_Audio_AacEncodeAdtsMpegType;

/*
   This data structure defines stereo AAC-HE encoder user configuration parameters 
*/

typedef struct  BDSP_Raaga_Audio_AacheEncConfigParams
{
    /* Output Bitrate of the Encoder */
    BDSP_Raaga_Audio_AacEncodeBitRate        eAacEncodeBitRate;
    /* Flag which directs the Encoder to encode in Mono or Stereo mode */
    uint32_t        ui32bEncodeMono;
    /* Disable the Spectral Band Replication module if this is reset , by default it is set */
    uint32_t        ui32EnableSBR;
    /* Selects the output Bitstream encode format
       0  RAW AUDIO
       1  ADTS
       2  LOAS
    */
    uint32_t        ui3248SRC32Enable;
      /* Enables/Disables SRC from 48khz to 32khz before encoding.
       1 SRC Enable
       0 No SRC
       */
    uint32_t        ui32EncodeFormatType;
	int32_t                         i32InputVolLevel;               /* in dB */
    int32_t                         i32OutputVolLevel;              /* in dB */
    BDSP_Raaga_Audio_AacEncodeComplexity    eAacEncodeComplexity;
    BDSP_Raaga_Audio_AacEncodeMonoChannelSelect   eAacEncodeMonoChSelcect;
    BDSP_Raaga_Audio_AacEncodeAdtsMpegType eAacEncodeAdtsMpegType;        
}BDSP_Raaga_Audio_AacheEncConfigParams;

typedef enum BDSP_Raaga_eTsmBool
{
	BDSP_Raaga_eTsmBool_False,
	BDSP_Raaga_eTsmBool_True,
	BDSP_Raaga_eTsmBool_Last,
	BDSP_Raaga_eTsmBool_Invalid = 0x7FFFFFFF
}BDSP_Raaga_eTsmBool;


typedef struct BDSP_AudioTaskTsmSettings
{
    int32_t                 i32TSMSmoothThreshold;  
    int32_t                 i32TSMSyncLimitThreshold;
    int32_t                 i32TSMGrossThreshold;
    int32_t                 i32TSMDiscardThreshold;
    int32_t                 i32TsmTransitionThreshold;     /* Transition threshold is required for the  
                                                                                DVD case not required right now*/
    uint32_t                ui32STCAddr;
    uint32_t                ui32AVOffset;           
	/* SwSTCOffset. This earlier was ui32PVROffset */
    uint32_t                ui32SwSTCOffset;
    uint32_t                ui32AudioOffset;
    
    /* For TSM error recovery*/
    BDSP_Raaga_eTsmBool            eEnableTSMErrorRecovery; /* Whether to go for error recovery 
                                                                                            when there are continuous TSM_FAIL */
    BDSP_Raaga_eTsmBool            eSTCValid;        /* If the STC in valid or not. In NRT case case, this is StcOffsetValid */
    BDSP_Raaga_eTsmBool            ePlayBackOn;  /* If the play back in on of off */
    BDSP_Raaga_eTsmBool              eTsmEnable;   /* if the tsm is enable or not*/
    BDSP_Raaga_eTsmBool              eTsmLogEnable;  /*if the tsm log is enable or not */
    BDSP_Raaga_eTsmBool             eAstmEnable;  /* if the  Adaptive System Time Management(ASTM) enable or not */

}BDSP_AudioTaskTsmSettings;	


/*
   This data structure defines Passthrough Node user configuration parameters 
*/

typedef enum BDSP_Raaga_ePassthruType
{
	BDSP_Raaga_ePassthruType_SPDIF,
	BDSP_Raaga_ePassthruType_RAW,
	BDSP_Raaga_ePassthruType_PCM,
	BDSP_Raaga_ePassthruType_Simul,
	BDSP_Raaga_ePassthruType_LAST,
	BDSP_Raaga_ePassthruType_INVALID =0x7FFFFFFF
}BDSP_Raaga_ePassthruType;

/*
 * AAC header type selection. If input stream is ADTS, supported values are BDSP_Raaga_eAacHeaderType_Raw
 * and BDSP_Raaga_eAacHeaderType_Adts. If input is LOAS, supported values are BDSP_Raaga_eAacHeaderType_Raw
 * and BDSP_Raaga_eAacHeaderType_Loas
 */
typedef enum BDSP_Raaga_eAacHeaderType
{
    /* If chosen ADTS/LOAS header will be stripped off and RAW data will be sent */
    BDSP_Raaga_eAacHeaderType_Raw,
    /* RAW data with ADTS header will be sent */
    BDSP_Raaga_eAacHeaderType_Adts,
    /* RAW data with LOAS header will be sent */
    BDSP_Raaga_eAacHeaderType_Loas,
    BDSP_Raaga_eAacHeaderType_Last,
    BDSP_Raaga_eAacHeaderType_Invalid    =   0x7fffffff

}BDSP_Raaga_eAacHeaderType;

typedef struct  BDSP_Raaga_Audio_PassthruConfigParams
{
	 BDSP_Raaga_ePassthruType ui32PassthruType;
	 BDSP_Raaga_eAacHeaderType	eAacHeaderType;

}BDSP_Raaga_Audio_PassthruConfigParams;

/* User Configuration custom voice Post Processing tool*/
typedef struct  BDSP_Raaga_Audio_CustomVoiceConfigParams
{
    int32_t     param000;     /*FUNC ENABLE*/
	int32_t    OSDVolume;
	int32_t    param011;
	uint32_t  param017;
	int32_t   param018;
	uint32_t  param019;
	uint32_t  param020;
	int32_t   param021;
	uint32_t  param022;
	int32_t   param023;
	int32_t   param024;
	int32_t   param025;
	uint32_t  param026;
	uint32_t  param027;
	uint32_t  param028;
	uint32_t  param029;
	uint32_t  param030;
	uint32_t  param031;
	uint32_t  param032;
	uint32_t  param033;
	uint32_t  param034;
	uint32_t  param035;
	uint32_t  param036;
	int32_t   param037;
	int32_t   param038;
	int32_t   param039;
	int32_t   param040;
	uint32_t  param041;
	uint32_t  param042;
	uint32_t  param043;
	uint32_t  param044;
	uint32_t  param045;
	uint32_t  param046;
	uint32_t  param047;
	uint32_t  param048;
	int32_t   param049;
	uint32_t  param050;
	uint32_t  param051;
	uint32_t  param052;
	uint32_t  param053;
	uint32_t  param054;
	int32_t   param055;
	int32_t   param056;
	int32_t   param057;
	int32_t   param058[20];
	int32_t   param059[20];
	int32_t   param060;
	int32_t   param061;
	int32_t   param062;
	int32_t fn17_tab02[11][2];
	int32_t fn17_tab03[11][3];
	int32_t fn17_tab04[11][2];
	int32_t fn17_tab05[11][3];
	int32_t   param067[3];
	int32_t   param068[2];
	int32_t   param069;
	int32_t   param070;
	int32_t   param071;
	int32_t   param072;
	int32_t   param073;
	int32_t   param074;
	int32_t   param075;
	int32_t   param076;
	int32_t   param077;
	int32_t   param078;
	int32_t   param079;
	int32_t   param080;
	int32_t   param081;
	int32_t   param082;
	int32_t   param083;
	int32_t   param084;
	int32_t   param085;
	int32_t   param086;
	int32_t   param087;
	int32_t   param088;
	int32_t   param089;
	int32_t   param090;
	int32_t   param091;
	uint32_t  param092;
	int32_t   param093;
	int32_t   param094;
	int32_t   param095;
	int32_t   param096;
	int32_t   param097;
	int32_t   param098;
	int32_t   param099;

	/*NewParams*/
	int32_t paramn093[2];
	int32_t paramn094[3];
	int32_t paramn095[2];
	int32_t paramn096[3];
	int32_t paramn097;
	int32_t paramn098;
	int32_t paramn099;
	int32_t paramn100;
	int32_t paramn101;
	int32_t paramn102;
	int32_t paramn103;
	int32_t paramn104;
	int32_t paramn105[3];
    int32_t paramn106[2];
    int32_t DSPfxdu_C68;


}BDSP_Raaga_Audio_CustomVoiceConfigParams;

/*
   This data structure defines DTS-5.1 Broadcast encoder user configuration parameters 
*/

typedef struct  BDSP_Raaga_Audio_DtsBroadcastEncConfigParams
{
	uint32_t			ui32SpdifHeaderEnable;			/* Default value = Disable:  Enable =1 Disable =0 */
 
	/*Certification Related */
	uint32_t			ui32CertificationEnableFlag;	/*Default 0 Set to 1 while doing certification*/
	uint32_t			ui32LFEEnableFlag;				/*Used only when Certification is set to 1. Enable =1 Disable =0*/
	BDSP_Raaga_Audio_AcMode	eInputDataAcMode;				/*Used only when Certification is set to 1*/
	uint32_t            ui32IsIIRLFEInterpolation;                           /*Default 0, range [0, 1]*/
                                                                             /*0 -> FIR interpolation for LFE */
                                                                             /*1 -> IIR interpolation for LFE */
}BDSP_Raaga_Audio_DtsBroadcastEncConfigParams;

/*
   This data structure defines AC3 encoder user configuration parameters 
*/

typedef struct  BDSP_Raaga_Audio_Ac3EncConfigParams
{
	int32_t  i32NumChans;
	 
	/* Audio Coding mode, Default: 7(3/2)*/
	int32_t  i32AudCodMode;
	 
	/* Data Rate is dependent on Acmod, 18 for 3/2, 14 for 2/0 and 1/1 */
	int32_t  i32DataRate;
	 
	/*  1 is LFE enabeld */
	int32_t  i32LoFreqEffOn;
	 
	/* Always Set to 48 kHz */
	int32_t  i32SampRateCod;	 
	 
	/* LFE filter is not being used, always set to 0 */
	int32_t  i32LfeFiltInUse;
	/* compression characteristic 0 = off , always off*/
	int32_t  i32CompChar;
	 
	/* compression characteristic for 1+1 R chan, 0 = off, always off */
	int32_t  i32CompChar2;
	/* Always switched off in release code, can be enabled for certification testing */
	int32_t  i32SurDelayArg;
	 
	/* This is used to enable/disable SPDIF header.. Default value = Enable:  Enable =1 Disable =0 */
	BDSP_AF_P_EnableDisable eSpdifHeaderEnable;
	 
	/* This flag is set to enable when the Encoder is being used in transcode mode */
	BDSP_AF_P_EnableDisable   eTranscodeEnable;
	 
}BDSP_Raaga_Audio_Ac3EncConfigParams;


/*
   This data structure defines SRC user configuration parameters 
*/
typedef struct  BDSP_Raaga_Audio_SRCUserConfigParams
{
	uint32_t    ui32SrcDummyUserConfig;/*Default value is 0 */

}BDSP_Raaga_Audio_SRCUserConfigParams;


/* This data structure defines for AUDIO DESCRIPTOR */

typedef struct
{
	uint32_t   ui32FadeValue; /*Default - 0x7FFFFFFF (no fade) */

}BDSP_Raaga_Audio_sPanFadeIf; 

typedef enum BDSP_Raaga_Audio_FadeConfigType
{
    BDSP_Raaga_Audio_FadeConfigType_ePrimaryChOnly = 0, 
    BDSP_Raaga_Audio_FadeConfigType_eFadedChOnly,
    BDSP_Raaga_Audio_FadeConfigType_ePrimaryAndFadedCh,
    BDSP_Raaga_Audio_FadeConfigType_eLast,
	BDSP_Raaga_Audio_FadeConfigType_eInvalid = 0x7FFFFFFF 

} BDSP_Raaga_Audio_FadeConfigType;

/*
   This data structure defines Audio Descriptor Fade user configuration parameters 
*/
typedef struct  BDSP_Raaga_Audio_AudioDescFadeConfigParams
{
	BDSP_Raaga_Audio_FadeConfigType   eFadeConfig;				/* Default Value: BDSP_Raaga_Audio_FadeConfigType_eFadedChOnly */

    uint32_t					 ui32PanFadeInterfaceAddr;	/* This is a Dram Structure having the type 'BDSP_Raaga_Audio_sPanFadeIf'*/

	uint32_t					 ui32PanFadeInterfaceValidFlag;	/*Default value will be zero and set to 1 when the InterFaceAddr is filled*/

}BDSP_Raaga_Audio_AudioDescFadeConfigParams;

/*
   This data structure defines Audio Descriptor Fade user configuration parameters 
*/
typedef enum
{
    BDSP_Raaga_Audio_ADChannelConfig_eADChannelOff = 0,
    BDSP_Raaga_Audio_ADChannelConfig_eADChannelOn,
    BDSP_Raaga_Audio_ADChannelConfig_eLast,
    BDSP_Raaga_Audio_ADChannelConfig_eInvalid = 0x7FFFFFFF
}BDSP_Raaga_Audio_ADChannelConfig;


typedef struct  BDSP_Raaga_Audio_AudioDescPanConfigParams
{	
	/* Range 0 (mute)-0x7ffffffff(0Db) */

    int32_t    i32UserVolumeLevel;		/*Default - 0x7fffffff */

    uint32_t   ui32PanFadeInterfaceAddr;/* This is a Dram Structure having the type 'BDSP_Raaga_Audio_sPanFadeIf'*/	

    BDSP_Raaga_Audio_ADChannelConfig eADChannelConfig; /*This will give the AD channel is 
                                                    enable or disable. Default of this
                                                    is Enable*/
	uint32_t ui32PanFadeInterfaceValidFlag;      /*Default value will be zero and set to 1 when the InterFaceAddr is filled*/
	
	uint32_t ui32AudioRampTimeInMs;					/*Default value should be 1000. Range 0 to 4Sec  */

}BDSP_Raaga_Audio_AudioDescPanConfigParams;


/*
   This data structure defines Frame sync configuration parameters 
*/
typedef enum BDSP_Raaga_Audio_CapInputPort
{
    BDSP_Raaga_Audio_CapInputPort_eIntCapPort0 = 0, /* Internal Capture port 0 */
    BDSP_Raaga_Audio_CapInputPort_eIntCapPort1,     /* Internal Capture port 1 */
    BDSP_Raaga_Audio_CapInputPort_eIntCapPort2,     /* Internal Capture port 2 */
    BDSP_Raaga_Audio_CapInputPort_eIntCapPort3,     /* Internal Capture port 3 */
    BDSP_Raaga_Audio_CapInputPort_eExtI2s0,         /* External I2S Capture port */
    BDSP_Raaga_Audio_CapInputPort_eRfI2s,         /* BTSC Capture port	*/
    BDSP_Raaga_Audio_CapInputPort_eSpdif,           /* SPDIF Capture port	*/
    BDSP_Raaga_Audio_CapInputPort_eHdmi,            /* HDMI */
	BDSP_Raaga_Audio_CapInputPort_eAdc,
    BDSP_Raaga_Audio_CapInputPort_eIntCapPort4,     /* Internal Capture port 4 */
    BDSP_Raaga_Audio_CapInputPort_eIntCapPort5,     /* Internal Capture port 5 */
    BDSP_Raaga_Audio_CapInputPort_eIntCapPort6,     /* Internal Capture port 6 */
    BDSP_Raaga_Audio_CapInputPort_eIntCapPort7,     /* Internal Capture port 7 */
	BDSP_Raaga_Audio_CapInputPort_eDummy,		   /* This enum is used for File based input.... Mainly applicable to Certification purposes*/	
    BDSP_Raaga_Audio_CapInputPort_eMax,			   /* Invalid/last Entry */
	BDSP_Raaga_Audio_CapInputPort_eInvalid = 0x7FFFFFFF
 
} BDSP_Raaga_Audio_CapInputPort;


typedef enum BDSP_Raaga_Audio_WMAIpType
{
 BDSP_Raaga_Audio_WMAIpType_eASF = 0,
 BDSP_Raaga_Audio_WMAIpType_eTS,
 BDSP_Raaga_Audio_WMAIpType_eMax,
 BDSP_Raaga_Audio_WMAIpType_eInvalid = 0x7FFFFFFF
}BDSP_Raaga_Audio_WMAIpType;

typedef enum BDSP_Raaga_Audio_ASFPTSType
{
	 BDSP_Raaga_Audio_ASFPTSType_eInterpolated = 0,
	 BDSP_Raaga_Audio_ASFPTSType_eCoded,
	 BDSP_Raaga_Audio_ASFPTSType_eMax,
	 BDSP_Raaga_Audio_ASFPTSType_eInvalid = 0x7FFFFFFF
}BDSP_Raaga_Audio_ASFPTSType;

typedef enum BDSP_Raaga_Audio_LpcmAlgoType
{
    BDSP_Raaga_Audio_LpcmAlgoType_eDvd,
    BDSP_Raaga_Audio_LpcmAlgoType_eIeee1394,
    BDSP_Raaga_Audio_LpcmAlgoType_eBd,
    BDSP_Raaga_Audio_LpcmAlgoType_eMax,
    BDSP_Raaga_Audio_LpcmAlgoType_eInvalid = 0x7FFFFFFF
}BDSP_Raaga_Audio_LpcmAlgoType;

typedef enum BDSP_Raaga_Audio_DtsEndianType
{
    BDSP_Raaga_Audio_DtsEndianType_eBIG_ENDIAN = 0,
    BDSP_Raaga_Audio_DtsEndianType_eLITTLE_ENDIAN,
    BDSP_Raaga_Audio_DtsEndianType_eINVALID = 0x7FFFFFFF
}BDSP_Raaga_Audio_DtsEndianType;

typedef enum BDSP_Raaga_Audio_AudioInputSource
{
    BDSP_Raaga_Audio_AudioInputSource_eExtI2s0 = 0,		/* External I2S Capture port */
    BDSP_Raaga_Audio_AudioInputSource_eCapPortRfI2s,   /* BTSC Capture port */
    BDSP_Raaga_Audio_AudioInputSource_eCapPortSpdif,     /* SPDIF Capture port      */
    BDSP_Raaga_Audio_AudioInputSource_eCapPortHdmi,      /* HDMI */
    BDSP_Raaga_Audio_AudioInputSource_eCapPortAdc,
    BDSP_Raaga_Audio_AudioInputSource_eRingbuffer,		/* This is for Certification needs where PCM Samples are loaded from file to Ring buffers*/
    BDSP_Raaga_Audio_AudioInputSource_eMax,              /* Invalid/last Entry */
    BDSP_Raaga_Audio_AudioInputSource_eInvalid = 0x7FFFFFFF

} BDSP_Raaga_Audio_AudioInputSource;



typedef struct BDSP_AudioTaskDatasyncSettings 
{
    BDSP_AF_P_EnableDisable         eEnablePESBasedFrameSync;    /* Default = Disabled */
    BDSP_Raaga_Audio_AudioInputSource    eAudioIpSourceType;           /* Capture port Type    */

    union
    {
        uint32_t ui32SamplingFrequency;					/* Will be used if IpPortType is I2S*/
        uint32_t ui32RfI2sCtrlStatusRegAddr;			/* For RfI2s i.e. BTSC */
        uint32_t ui32SpdifCtrlStatusRegAddr;			/* For SPDIF */ 
        uint32_t ui32MaiCtrlStatusRegAddr;				/* For HDMI */

    }uAudioIpSourceDetail; 

	BDSP_AF_P_EnableDisable               eEnableTargetSync;   /* Default = Enabled */

    union
    {
	    struct
	    {
	        BDSP_Raaga_Audio_ASFPTSType eAsfPtsType;            /* Default = 0 (Use Interpolation always). 1 = Use Coded always. */
	        BDSP_Raaga_Audio_WMAIpType eWMAIpType;              /* Default = 0 (Type ASF). Set to TS only when WMATS is enabled */
	    }sWmaConfig;
	    struct
	    {
	        BDSP_Raaga_Audio_LpcmAlgoType               eLpcmType;
	    }sLpcmConfig;
	    struct
	    {
	        BDSP_Raaga_Audio_DtsEndianType              eDtsEndianType; /* Added for DTS-CD Little Endian Support */
	    }sDtsConfig;
    }uAlgoSpecConfigStruct;                                     /* The algo specific structures for configuration */

    BDSP_AF_P_EnableDisable               eForceCompleteFirstFrame;   /* If enabled, the first frame will always be entirely rendered to the 
                                                                         output and not partially truncated for TSM computations.  This should 
                                                                         be disabled for normal operation, but may be required for some bit-exact 
                                                                         certification testing that requires all data to be rendered even with TSM 
                                                                         enabled. */

} BDSP_AudioTaskDatasyncSettings ;

/*
	This data structure defines IDS, DS and TSM configuration parameters 
*/

typedef struct BDSP_Raaga_Audio_FrameSyncTsmConfigParams 
{
	/*
	* Data sync configuration parameters
	*/
	BDSP_AudioTaskDatasyncSettings	sFrameSyncConfigParams;

	/*
	* TSM configuration parameters
	*/
	BDSP_AudioTaskTsmSettings			sTsmConfigParams;

} BDSP_Raaga_Audio_FrameSyncTsmConfigParams;

/*
	This data structure defines AVL configuration parameters 
*/

typedef struct BDSP_Raaga_Audio_AVLConfigParams
{   
   uint32_t  ui32AVLEnableFlag;		/* Default value = Enable:  Enable =1 Disable =0 */
   int32_t   i32Target;				/* -20*32768 Target level of output signal in terms of dBFs Q15   */
   int32_t   i32LowerBound;			/* -31*32768 Lower bound for primary range of volume control Q15   */
   int32_t   i32FixedBoost;			/* 11*32768   Amount of fixed boost (in dB) if level < LOWERBOUND Q15  */
   int32_t   i32RefLevel;			/* 2959245	(20*log10(32768)) Reference logarithmic gain value of 0 dBFs Q15 */
   int32_t   i32Alpha;				/* 32736	(1023/1024)		Attenuation factor for Level Maximum Q15    */
   int32_t   i32Beta;				/* 32256	(63/64)			Attenuation factor for Level Mean Q15      */
   int32_t   i32ActiveThreshold;    /* 9830		(0.3)			Threshold for detecting active portion of signal Q15   */
   int32_t   i32DTFPCNT;			/* 750		(750)   		Decay Time for DTF Percent delay (in miliseconds) Q0    */
   int32_t   i32Alpha2;				/* 32767	(32767/32768)	Attenuation factor for Level Minimum Q15    */
   int32_t   i32NSFGR_SEC;			/* 16384	(0.5)			Fast Gain Ramp in milliseconds Q15       */
   int32_t   i32DTF;				/* 9830		(0.3)			Decay Time Fraction Q15          */
   int32_t   i32LoudnessLevelEq;    /* Default value = 1:       Enable =1 Disable =0 */

}BDSP_Raaga_Audio_AVLConfigParams;


/*
	This data structure defines SRS Tru Surround configuration parameters 
*/

typedef enum BDSP_Raaga_Audio_SpeakerSize
{
	BDSP_Raaga_Audio_SpeakerSize_eLFResponse40Hz  = 0,
	BDSP_Raaga_Audio_SpeakerSize_eLFResponse60Hz  = 1,
	BDSP_Raaga_Audio_SpeakerSize_eLFResponse100Hz = 2,
	BDSP_Raaga_Audio_SpeakerSize_eLFResponse150Hz = 3,
	BDSP_Raaga_Audio_SpeakerSize_eLFResponse200Hz = 4,
	BDSP_Raaga_Audio_SpeakerSize_eLFResponse250Hz = 5,
	BDSP_Raaga_Audio_SpeakerSize_eLFResponse300Hz = 6,
	BDSP_Raaga_Audio_SpeakerSize_eLFResponse400Hz = 7,
	BDSP_Raaga_Audio_SpeakerSize_eLAST,
	BDSP_Raaga_Audio_SpeakerSize_eINVALID = 0x7fffffff

} BDSP_Raaga_Audio_SpeakerSize;

 
/*
	This data structure defines Tru Surrnd-XT configuration parameters 
*/
typedef struct  BDSP_Raaga_Audio_TruSurrndXTConfigParams
{ 
	/* TruSurround options  */
    uint32_t   ui32TSEnable;			/* Default value = Enable:  Enable =1 Disable =0 */
    uint32_t   ui32TSHeadphone;			/* Default value = Disable:  Enable =1 Disable =0 */
    int32_t    i32TruSurroundInputGain; /* Default value 0x40000000 corresponding to 1. Fixed point range 0 to 0x7fffffff. Floating point range 0 to 2*/

	/*DialogClarity options*/
    uint32_t   ui32DialogClarityEnable; /* Default value = Enable:  Enable =1 Disable =0 */
    uint32_t   ui32DialogClarityLevel;	 /*Default value 0x40000000 corresponding to 0.5. Fixed point range 0 to 0x7fffffff. Floating point range 0 to 1*/

    /* TruBass options */
    uint32_t   ui32TBEnable;				/* Default value = Enable:  Enable =1 Disable =0 */
    int32_t    i32TBLevel;					 /* Default value 4CCCCCCD corresponding to 0.6. Fixed point range 0 to 0x7fffffff. Floating point range 0 to 1.0 */
    BDSP_Raaga_Audio_SpeakerSize eTBSpeakerSize; /* Default value BDSP_Raaga_Audio_SpeakerSize_eLFResponse60Hz */    

#if 0
    /* Definition options */
    uint32_t ui32DefinitionEnableFlag;      /*Enable = 1*/
    uint32_t ui32DefinitionControlScale;    /*Default value 0x33333333 corresponding to 0.4. Fixed point range 0 to 0x7fffffff. Floating point range 0 to 1.0*/
    uint32_t ui32DefinitionInputGain;       /*Default value 0x40000000 corresponding to 0.5. Fixed point range 0 to 0x7fffffff. Floating point range 0 to 1.0*/
    uint32_t ui32DefinitionOutputGain;      /*Default value 0x7fffffff corresponding to 1.0. Fixed point range 0 to 0x7fffffff. Floating point range 0 to 1.0*/
    uint32_t ui32DefintionByPassGain;       /*Default value 0x5A7EF9DB corresponding to 0.707. Fixed point range 0 to 0x7fffffff. Floating point range 0 to 1.0*/
#endif

    /* Certification related options */
    uint32_t  ui32CertificationEnableFlag;   /*Default value is 0.  Set to 1 while doing certification*/
    BDSP_Raaga_Audio_AcMode            eAcMode; /*Varies depending on the input mode to be certified*/

}BDSP_Raaga_Audio_TruSurrndXTConfigParams;


/*
	This data structure defines Dolby Prologic II configuration parameters 
*/

typedef struct BDSP_Raaga_Audio_PL2ConfigParams
{
    uint32_t  ui32enable;		/* Default value = Enable:	 Enable =1 Disable =0 */
    uint32_t  ui32abalflg;		/* Default value = Disable:  Enable =1 Disable =0 */
    uint32_t  ui32decmode;		/* Default = 3*/                 
    uint32_t  ui32chancfg;		/* Default = 7*/                 
    uint32_t  ui32cwidthcfg;	/* Default = 3*/                 
    uint32_t  ui32dimcfg;		/* Default = 3*/                 
    uint32_t  ui32panoramaflg;	/* Default value = Disable:  Enable =1 Disable =0 */
    uint32_t  ui32sfiltflg;		/* Default value = Disable:  Enable =1 Disable =0 */
    uint32_t  ui32rspolinvflg;	/* Default value = Disable:  Enable =1 Disable =0 */
    uint32_t  ui32pcmscl;		/* Default = 100 */                 
    uint32_t  ui32debugOn;		/* Default value = Off:  On =1 Off =0 */
    uint32_t  ui32outputch;		/* Default = 0*/                 

} BDSP_Raaga_Audio_PL2ConfigParams;

/*
	This data structure defines DVD LPCM configuration parameters 
*/

typedef struct BDSP_Raaga_Audio_LpcmOutputConfig
{
	uint32_t    ui32OutMode;					/* Output channel assignment mode */
												
	uint32_t    ui32LfeOnFlag;					/* Flag for LFE 
												   Default value = Disable:  Enable =1 Disable =0 
												*/
	uint32_t    ui32DualMonoModeFlag ;			/* Mode for source selection in dual-mono LPCM-VR streams
												   Default value = Disable:  Enable =1 Disable =0 
												*/
	uint32_t    ui32OutputChannelMatrix[BDSP_AF_P_MAX_CHANNELS];

}BDSP_Raaga_Audio_LpcmOutputConfig ;				/* Output configuration structure for an output port */
 
typedef struct BDSP_Raaga_Audio_LpcmUserConfig
{
	uint32_t						ui32NumOutputPorts;    /* Number of output ports */
	BDSP_Raaga_Audio_LpcmOutputConfig	sOutputConfig[2];      /* Array of output configuration structures, one for each output port  */
	/*
	 * Currently removing the provision for user defined downmix tables (FWRAA-453)
	 */
	/*uint32_t    ui32UseUserDownmixCoeffsFlag;*/	/* Flag to use user downmix coefficients.
												   Default value = Disable:  Enable =1 Disable =0 
												*/	    
	/*int32_t                         i32UserDownmixTables[16][8][8];*/  /* 16x8x8 User DownMix tables - decoder uses only 1 such 8x8 table for a given frame  */

}BDSP_Raaga_Audio_LpcmUserConfig;


/*
	This data structure defines dsola configuration parameters 
*/

typedef struct BDSP_Raaga_Audio_DsolaConfigParams
{ 
     uint32_t ui32InputPcmFrameSize; /* Default value 0x266 */

}BDSP_Raaga_Audio_DsolaConfigParams;

/*
	This data structure defines Custom bass configuration parameters 
*/

typedef struct BDSP_Raaga_Audio_CustomBassUserConfig
{                                    

    uint32_t ui32enable;				/* Enable =1 Disable =0 Default=Enable */
    uint32_t ui32operation;				/* 0 (AGC and Harmonics) */
    uint32_t ui32agcType;				/* 0 (low and high) */
    uint32_t ui32harmonicsType;			/* 0 (half wave rectifier) */
    uint32_t ui32lpfFc;					/* 0xE (200 Hz) */
    uint32_t ui32hpfFc;					/* 0x12 (315 Hz) */
    uint32_t ui32agcGainMax;			/* 0x12 (6 dB) */
    uint32_t ui32agcGainMin;			/* 0xC (0 dB) */
    uint32_t ui32agcAttackTime;			/* 0x2 (5 ms)  */
    uint32_t ui32agcRelTime;			/* 0x2 (0.5 s)  */
    uint32_t ui32agcThreshold;			/* 0x16 (-22 dB) */
    uint32_t ui32agcHpfFc;				/* 0x8 (100 Hz) */
    uint32_t ui32harLevel;				/* 0 (0 dB) */
    uint32_t ui32harLpfFc;				/* 0x11 (280 Hz) */
    uint32_t ui32harHpfFc;				/* 0x8 (100 Hz) */

} BDSP_Raaga_Audio_CustomBassUserConfig;


/*
	This data structure defines Custom Surround configuration parameters 
*/
typedef struct BDSP_Raaga_Audio_CustomSurroundUserConfig
{                             
    uint32_t ui32enable;		/* Enable =1 Disable =0 Default=Enable */
    uint32_t ui32combOn;		/* FALSE;	   TURE, FALSE*/
    uint32_t ui32delay;			/* 0;          0x0 ~ 0x1F samples*/
    uint32_t ui32volume1;		/* 0(0dB);     0x0 ~ 0xC (0dB ~ 12dB), 0xF (infinite)*/
    uint32_t ui32volume2;		/* 0(0dB);     0x0 ~ 0xC (0dB ~ 12dB), 0xF (infinite)*/
    uint32_t ui32volume3;		/* 0(0dB);     0x0 ~ 0xC (0dB ~ 12dB), 0xF (infinite)*/
    uint32_t ui32volume4;		/* 0x1E(-3dB); 0x0 ~ 0x40 (12dB ~ -20dB with half dB step)*/
    uint32_t ui32volume5;		/* 0x1E(-3dB); 0x0 ~ 0x40 (12dB ~ -20dB with half dB step)*/
    uint32_t ui32lpfFc;			/* 0x74(8kHz); 0x70 ~ 0x76 (5kHz ~ 10kHz)*/
    uint32_t ui32lpfQ;			/* 0x44(1.0);  0x40 ~ 0x4F (0.33 ~ 8.20)*/
    uint32_t ui32lpfGain;		/* 0x56(-6dB); 0x40 ~ 0x4C (0dB ~ 12dB), 0x51 ~ 0x5C (-1dB ~ -12dB) */

    /* The funcVol and inputTrim are independent of SHARP Surround */
    uint32_t ui32funcVol;		/* 0x40        (0 dB)*/
    uint32_t ui32inputTrim;		/* 0x14        (-20 dB)*/

} BDSP_Raaga_Audio_CustomSurroundUserConfig;


/*
	This data structure defines DTS core configuration parameters 
*/
typedef struct  BDSP_Raaga_Audio_DtsCoreUserConfig
{
	int32_t		i32OutputMode;									/* Default: 2 */
	int32_t		i32OutLfe;										/* Default: 0 */	
	uint32_t	ui32OutputChannelMatrix[BDSP_AF_P_MAX_CHANNELS];/* Default Channel Matrix = {0, 1, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,0xFFFFFFFF} */

}BDSP_Raaga_Audio_DtsCoreUserConfig;

typedef struct  BDSP_Raaga_Audio_DtsCoreConfigParams
{
	int32_t							i32NumOutPorts;		/* Default: 1 */  
	BDSP_Raaga_Audio_DtsCoreUserConfig   sUserOutputCfg[2];	/* These are user config parameters required from user  */

}BDSP_Raaga_Audio_DtsCoreConfigParams; 

/*
	This data structure defines Dolby Volume configuration parameters 
*/

typedef struct BDSP_Raaga_Audio_DolbyVolumeUserConfig
{
	/*General Settings*/

	int32_t		i32DolbyVolumeEnable;			/*Range of values -  0 or 1 
												
												  As per the new guyidelines from Dolby, i32VolumeIqEnable =1(True) for 
												  both when DV is enabled and disabled.
													
												  The controlling factor is now i32LvlEnable 

												  Default value is 1 (True)
												*/

	int32_t		i32BlockSize;					/*Size of processing block in samples */
												/*Range of values - 256, 512 */
												/*Default value    : 512 */
	int32_t		i32nBands;						/*Number of critical bands to use */
												/*Possible values 20 or 40, default 20*/


	int32_t		i32nChans;						/*Number of input/output channels */
												/*Default val 2*/

	/* System Settings*/
	int32_t		i32InputReferenceLevel;			/*Input reference level in dBSPL */
												/*Range of values(in dB )- (0 to 100 in 8.24 format)*/
												/*Default value :0x4A000000 (74 dB)*/

	int32_t		i32OutputReferenceLevel;		/* Output reference level in dBSPL */
												/*Range of values(in dB) - (0 to 100 in 8.24 format)*/
												/*Default value :0x4A000000 (74 dB)*/

	int32_t		i32Calibration;					/*Calibration offset in dB */
												/*Range of values(in dB)- (-100 to 30 in 8.24 format)*/
												/*Default value    : 0x00000000 (0 dB)*/
	int32_t		i32VlmEnable;					/*Volume modeler on/off */
												/*Possible value 0 and 1, default 1*/
	int32_t		i32ResetNowFlag;				/*User-input forced reset flag */
												/*Range [0,1], default 0*/


	/*Volume Modeler Settings*/
	int32_t		i32DigitalVolumeLevel;			/*Volume level gain in dB -- applied by dolby volume */
												/*Range of values(in dB) - (-100 to 30 in 8.24 format)*/
												/*Default value    : 0x00000000 (0 dB)*/

	int32_t		i32AnalogVolumeLevel;			/*Volume level gain in dB -- applied after dolby volume */
												/*Range of values(in dB) - (-100 to 30 in 8.24 format)*/
												/*Default value    :  0x00000000 (0 dB)*/

	/*Volume Leveler Settings */
	int32_t		i32LvlAmount;					/*Range of values - [0 to 10]*/
												/*Default value		: 9*/
	
	int32_t		i32LvlEnable;					/*Range of values -  0=FALSE , 1=TRUE

												 i32LvlEnable = 1 When Dolby Volume is Enabled
												 i32LvlEnable = 0 When Dolby Volume is Disabled

												 Default value : 1 (TRUE - volume leveler enabled)
												*/
	
	int32_t		i32EnableMidsideProc;			/*Enable midside processing */
												/*Range [0,1], default ->0-> disable*/

	int32_t     i32HalfmodeFlag;				/*Flag to operate Dolby Volume in half mode*/
												/*Range [0,1], default ->0-> disable*/
	int32_t     i32LimiterEnable;				/*Enable Limter*/ 
												/*When DV is enabled limiter_enable = TRUE
												  When DV is disabled limiter_enable = FALSE*/

}BDSP_Raaga_Audio_DolbyVolumeUserConfig;




/*
	This data structure defines DTS broadcast decoder configuration parameters 
*/
typedef struct BDSP_Raaga_Audio_DtshdUserOutput
{
	int32_t i32UserDRCFlag; 	/*  Default =0  	Range : 0,1 (0-> DRC enabled; 1-> DRC disabled) 
									This Flag is used to turn on/off dynamic range compression 
								*/
	
	int32_t i32DynScaleHigh;
								/*  Default = 0x7fffffff
									Range 0 to 0x7fffffff (0 to 100%)
									Dynamic range compression cut scale factor. Its in Q1.31 format.*/
	
	int32_t i32DynScaleLow;

								/*  Default = 0x7fffffff
									Range 0 to 0x7fffffff (0 to 100%)
									Dynamic range compression boost scale factor. Its in Q1.31 format.
								*/

	uint32_t ui32OutMode;		/*Default =7;
								Output channel configuration */
								/* This field indicates Ouput channel Configuration
									0 = Two_mono_channels_1_1_ch1_ch2
									1 = One_centre_channel_1_0_C
									2 = Two_channels_2_0_L__R
									3 = Three_channels_3_0_L_C_R
									4 = Three_chanels_2_1_L_R_S
									5 = Four_channels_3_1_L_C_R_S
									6 = Four_channels_2_2_L_R_SL_SR
									7 = Five_channels_3_2_L_C_R_SL_SR */

	uint32_t ui32OutLfe;
								/* Default = 1 
								   Range : 0,1 (0-> LFE disabled;1-> LFE enabled)
								   Flag used to enable/disable LFE channel output */
	
	uint32_t ui32DualMode;
								/* Default =2
								   Range : 0-3 (0-> DUALLEFT_MONO; 1->DUALRIGHT_MONO;2->STEREO;3->DUAL MIX MONO)
								   Configure decoder output for dual mode */

	 uint32_t ui32StereoMode;

								/*  Default=0
									Range : 0,1 (1->Lt/Rt downmix;0->Normal output)
									Perform stereo downmix of decoder output */

	uint32_t ui32AppSampleRate;
								/* Default : 48000
								   Range : 192,000, 176400, 96000, 88200, 48000, 44100 , 320000
								   This is the sampling rate set by application layer. The decoder will limit decoding additional components in the stream (e.g, X96) to adjust the sampling rate of the decoded output, to that set by the application
								 */
	
	uint32_t ui32OutputChannelMatrix[BDSP_AF_P_MAX_CHANNELS];
								/*Here BDSP_AF_P_MAX_CHANNELS = 8 -> see the detail description in bottom */

	 uint32_t ui32ExtdnmixEnabled;
								/* Default=0
									Range : 0,1 (0->Disable external downmix;1->Enable external downmix) */

	int32_t i32ExtDnmixTab[BDSP_AF_P_MAX_CHANNELS][BDSP_AF_P_MAX_CHANNELS];
								/* Here BDSP_AF_P_MAX_CHANNELS = 8*/
								/*Default all zeros; User specified downmix coefficients can be given through this matrix*/

}BDSP_Raaga_Audio_DtshdUserOutput;

/*
   This data structure defines DTS-HD decoder user configuration parameters 
*/
typedef struct BDSP_Raaga_Audio_DtsHdConfigParams	 			
{				
	uint32_t ui32DecodeCoreOnly;
								/*Default=1; Range : 0,1 (1-> Decode only core, Disable all extension components)
								 This flag has precedence over other decode flags like  ui32DecodeDtsOnly,  ui32DecodeXLL etc) */
	uint32_t ui32DecodeDtsOnly;
								/*Default=0; Range : 0,1 (1-> decode components present in core substream only)*/
	uint32_t ui32DecodeXLL;			
								/*Default=1; Range : 0,1 (0-> does not decode XLL component)*/
	uint32_t ui32DecodeX96;
								/*Default=1; Range : 0,1 (0-> does not decode X96 component)*/			
	uint32_t ui32DecodeXCH;			
								/*Default=1; Range : 0,1 (0-> does not decode XCH component)*/
	uint32_t ui32DecodeXXCH;			
								/*Default=1; Range : 0,1 (0-> does not decode XXCH component)*/
	uint32_t ui32DecodeXBR;			
								/*Default=1; Range : 0,1 (0-> does not decode XBR component)*/
	uint32_t ui32EnableSpkrRemapping;			
								/*Default=0; Range : 0,1 (1->Enable Speaker remapping; 0->Disable speaker remapping)*/
	uint32_t ui32SpkrOut;			
								/*Default=2123*/
											/*Specifies the speaker out configuration. Instructs the decoder to perform downmixing or speaker remapping to yield the desired output speaker configuration. 
												SpkrOut - Output Speaker Configuration 
												0  - Use Encoded Native (default) 
												1  - C  
												9  - C LFE1 
												2  - L R  
												10  - L R LFE1 
												262144  Lt Rt (downmix) 
												3  - C L R  
												11  - C L R LFE1 
												18  - L R Cs 
												26  - L R Cs LFE1 
												19  - C L R Cs 
												27  - C L R Cs LFE1 
												6  - L R Ls Rs 
												14  - L R Ls Rs LFE1 
												15  - C L R Ls Rs LFE1 
												8207  - C L R Ls Rs LFE1 Lhs Rhs 
												2123  - C L R LFE1 Lsr Rsr Lss Rss 
												47  - C L R Ls Rs LFE1 Lh Rh 
												79  - C L R Ls Rs LFE1 Lsr Rsr 
												159  - C L R Ls Rs LFE1 Cs Ch 
												287  - C L R Ls Rs LFE1 Cs Oh 
												1039  - C L R Ls Rs LFE1 Lw Rw 
												31- C L R Ls Rs LFE1 Cs	*/

	uint32_t ui32MixLFE2Primary;			
												/*Default =0; Range : 0,1 (1-> Mix LFE to primary while downmixing, when Lfe output is disabled)*/
	uint32_t ui32ChReplacementSet;
												/*Default =0*/	
												/*It can take all possible values from 0 to 7. But it is not really required in core decoder. But, it will be needed in DTS-HD decoder going forward.*/
	uint32_t i32NumOutPorts;		
												/* Default =2; Range : 1,2 (2-> enables PCM output and concurrent stereo)*/
    uint32_t  ui32EnableMetadataProcessing;     /* Default =0; Range : 0,1 (0->Disable metadata processing, 1->Enable metadata processing*/

	BDSP_Raaga_Audio_DtshdUserOutput sUserOutputCfg[2];					

}BDSP_Raaga_Audio_DtsHdConfigParams;


/*
   This data structure defines DTS LBR decoder configuration parameters 
*/
typedef struct BDSP_Raaga_Audio_DtsLbrUserOutput
{
    uint32_t           ui32bEnableDialNorm;                                   /*  Default : 1     Range : [0,1] (0-> Dialnorm disabled; 1-> Dialnorm enabled) 
                                                This Flag is used to turn on/off Dialnorm */
    
    uint32_t           ui32EnableDRC;                                                                                /*  Default =0    Range : [0,1] (0-> DRC disabled; 1-> DRC enabled) 
                                                This Flag is used to turn on/off dynamic range compression */
                                                
    uint32_t           ui32DRCPercent;                                                                              /*  Default = 0x7fffffff (100 %)
                                                Range 0 to 0x7fffffff (0 to 100%) */
    
    uint32_t           ui32OutMode;                                                                  /*Default =7;
                                                Output channel configuration, this is according to BCOM_eACMOD */

    uint32_t           ui32OutLfe;                                                                                        /* Default = 1 
                                                   Range : 0,1 (0-> LFE disabled;1-> LFE enabled)
                                                   Flag used to enable/disable LFE channel output */
                                                
    uint32_t           ui32DualMode;                                                                                 /* Default =2
                                                   Range : 0-3 (0-> DUALLEFT_MONO; 1->DUALRIGHT_MONO;2->STEREO;3->DUAL MIX MONO)
                                                   Configure decoder output for dual mode */
                                                
     uint32_t          ui32StereoMode;                                                                            /*  Default=0
                                                    Range : 0,1 (1->Lt/Rt downmix;0->Normal output)
                                                    Perform stereo downmix of decoder output */

    uint32_t           ui32MixLFE2Primary;                                                      /*Default =0; Range : 0,1 (1-> Mix LFE to primary while downmixing, when Lfe output is disabled)*/

    uint32_t           ui32bEnableSpkrRemapping;                      /*Default =0; Range : 0,1 (1-> enable speaker remapping , 1-> disable speaker remapping )*/
    uint32_t           ui32SpkrOut;                                                                      /*[required output speaker configurations, default 15 , can take value from the following values*/
                                                /*
                                                ui32SpkrOut = 
                                                0       - Use Encoded Native, no speaker remapping
                                                15      - C L R         Ls                            Rs                           LFE1
                                                8207    - C L R       Ls                            Rs                           LFE1       Lhs                         Rhs
                                                2123    - C L R       LFE1       Lsr                          Rsr                          Lss                          Rss
                                                47      - C L R         Ls                            Rs                           LFE1       Lh                           Rh
                                                79      - C L R         Ls                            Rs                           LFE1       Lsr                          Rsr
                                                78      - L R Ls        Rs                           LFE1       Lsr                          Rsr                          
                                                159     - C L R        Ls                            Rs                           LFE1       Cs                           Ch
                                                287     - C L R        Ls                            Rs                           LFE1       Cs                           Oh
                                                1039    - C L R       Ls                            Rs                           LFE1       Lw                          Rw
                                                31      - C L R         Ls                            Rs                           LFE1       Cs                           
                                                30      - L R Ls        Rs                           LFE1       Cs*/


    uint32_t           ui32OutputChannelMatrix[BDSP_AF_P_MAX_CHANNELS];
                                                /*Here BDSP_AF_P_MAX_CHANNELS = 8 */
                                                /*This matrix decides the output order of channels
                                                
                                                    For Multi-Channel (5.1) ports:
                                                
                                                    ui32OutputChannelMatrix[0] = 0;
                                                    ui32OutputChannelMatrix[1] = 1;
                                                    ui32OutputChannelMatrix[2] = 2;
                                                    ui32OutputChannelMatrix[3] = 3;
                                                    ui32OutputChannelMatrix[4] = 4;
                                                    ui32OutputChannelMatrix[5] = 5;
                                                    ui32OutputChannelMatrix[6] = 0xffffffff; this value shows invalid 
                                                    ui32OutputChannelMatrix[7] = 0xffffffff; this value shows invalid 
                                                
                                                    For Stereo Output Port:

                                                    ui32OutputChannelMatrix[0] = 0;
                                                    ui32OutputChannelMatrix[1] = 1;
                                                    for(i=2; i<8; i++)
                                                        ui32OutputChannelMatrix[i] = 0xffffffff; this value shows invalid 
                                                    */                                                                                                                                       

}BDSP_Raaga_Audio_DtsLbrUserOutput;


/*
   This data structure defines DTS-LBR decoder user configuration parameters 
*/
typedef struct BDSP_Raaga_Audio_DtslbrConfigParams
{
    uint32_t         i32bIsPrimaryDecode;                                                    /*Default: 1, Range [0,1] , 0 -> primary, 1-> secondary*/
    uint32_t         i32bDecodeEmbeddedStereo;                                   /*Default: 0, Range [0,1] , 0 -> disabled, 1-> decode embedded stereo if present*/
    uint32_t         i32auxDynamicDownmixFlag;                                     /*default: 0, range: [0,1], [0, 1] = disableing/enabling Embedded downmix*/
    uint32_t    	 ui32DramMixerDataBufferAddress;                               /*Dram address for communicating Mixer metadata between Primary, Secondary and Mixer*/

    uint32_t         i32NumOutPorts;                                                                                             /* Default =1; Range : 1,2 (2-> enables PCM output and concurrent stereo)*/
                
    uint32_t         ui32IsMixingEnabled  ;              /* Default =0; 1-> If Mixing is enabled */                       /* Default =0; 1-> DVD */
    uint32_t    	 ui32PrimaryOutmode;                 /* When the decoder is configured for primary decoder or if there is no active primary decoder, the value of this variable will be invalid i.e. 0xFFFFFFFF ; If it is called in secondary mode then this variable will carry outmode of primary */

    BDSP_Raaga_Audio_DtsLbrUserOutput sUserOutputCfg[2];   

}BDSP_Raaga_Audio_DtslbrConfigParams;


/*
	This data structure defines PCMWAV/PCM_AVI configuration parameters 
*/
typedef struct BDSP_FWIF_P_PcmWavUserOutput
{   /*
    enum    {
            BCOM_eACMOD_MODE11, //CH1,CH2
            BCOM_eACMOD_MODE10, //C
            BCOM_eACMOD_MODE20, //L,R -->To be used for BufferOutputMode only.
            BCOM_eACMOD_MODE30, //L,C,R
            BCOM_eACMOD_MODE21, //L,R,S
            BCOM_eACMOD_MODE31, //L,C,R,S
            BCOM_eACMOD_MODE22, //L,R,LS,RS
            BCOM_eACMOD_MODE32, //L,C,R,LS,RS
            BCOM_eACMOD_INVALID=0xffffffff
    };
    default - 2*/
    int32_t i32OutMode;
    /*	preferred stereo mode if i32OutMode = STEREO
		enum { GBL_STEREOMODE_LORO = 0, GBL_STEREOMODE_LTRT };
		i32StereoMode = 0 is default value ;
	*/
    int32_t i32StereoMode;  
    /* dual mono downmix mode
	   enum { GBL_DUAL_STEREO=0, GBL_DUAL_LEFTMONO, GBL_DUAL_RGHTMONO, GBL_DUAL_MIXMONO };
       i32DualMode = 0 is default value;
	*/
    int32_t i32DualMode;
    /*
     * Downmix coefficients for each channel
     */
    int32_t i32DownmixCoeffs[BDSP_AF_P_MAX_CHANNELS];
              
    /*This is channel matrix of size 8 where each index can take any values from 0 to 7
	  Depending upon  the channel routing done    
	*/
    uint32_t   ui32OutputChannelMatrix[BDSP_AF_P_MAX_CHANNELS];

}BDSP_FWIF_P_PcmWavUserOutput;

typedef struct BDSP_Raaga_Audio_PcmWavConfigParams
{
	
	uint32_t	ui32NumOutputPorts;
    BDSP_FWIF_P_PcmWavUserOutput sUserOutputCfg[2];

}BDSP_Raaga_Audio_PcmWavConfigParams;	

/*
	Bit rates supported in MP3 encoder
*/
typedef	enum	BDSP_Raaga_Audio_Mp3EncodeBitRate
{
	BDSP_Raaga_Audio_Mp3EncodeBitRate_e32kbps = 32,
	BDSP_Raaga_Audio_Mp3EncodeBitRate_e40kbps = 40,
	BDSP_Raaga_Audio_Mp3EncodeBitRate_e48kbps = 48,
	BDSP_Raaga_Audio_Mp3EncodeBitRate_e56kbps = 56,
	BDSP_Raaga_Audio_Mp3EncodeBitRate_e64kbps = 64,
	BDSP_Raaga_Audio_Mp3EncodeBitRate_e80kbps = 80,
	BDSP_Raaga_Audio_Mp3EncodeBitRate_e96kbps = 96,
	BDSP_Raaga_Audio_Mp3EncodeBitRate_e112kbps = 112,
	BDSP_Raaga_Audio_Mp3EncodeBitRate_e128kbps = 128,
	BDSP_Raaga_Audio_Mp3EncodeBitRate_e160kbps = 160,
	BDSP_Raaga_Audio_Mp3EncodeBitRate_e192kbps = 192,
	BDSP_Raaga_Audio_Mp3EncodeBitRate_e224kbps = 224,
	BDSP_Raaga_Audio_Mp3EncodeBitRate_e256kbps = 256,
	BDSP_Raaga_Audio_Mp3EncodeBitRate_e320kbps = 320,
	BDSP_Raaga_Audio_Mp3EncodeBitRate_eLast,
	BDSP_Raaga_Audio_Mp3EncodeBitRate_eInvalid = 0x7fffffff
}BDSP_Raaga_Audio_Mp3EncodeBitRate;

/*
	Emphasis types in mp3 encoder
*/
typedef	enum	BDSP_Raaga_Audio_Mp3EncodeEmphasisType
{
	BDSP_Raaga_Audio_Mp3EncodeEmphasisType_eNone,
	BDSP_Raaga_Audio_Mp3EncodeEmphasisType_e50_15uSeconds,
	BDSP_Raaga_Audio_Mp3EncodeEmphasisType_eReserved,
	BDSP_Raaga_Audio_Mp3EncodeEmphasisType_eCCITTJ_17,
	BDSP_Raaga_Audio_Mp3EncodeEmphasisType_eLast,
	BDSP_Raaga_Audio_Mp3EncodeEmphasisType_eInvalid = 0x7fffffff
}BDSP_Raaga_Audio_Mp3EncodeEmphasisType;

typedef enum
{
    BDSP_Raaga_Audio_Mp3EncodeMono_Left      = 0,
    BDSP_Raaga_Audio_Mp3EncodeMono_Right     = 1,
    BDSP_Raaga_Audio_Mp3EncodeMono_Mix       = 2,
    BDSP_Raaga_Audio_Mp3EncodeMono_DualMono  = 3
}BDSP_Raaga_Audio_Mp3EncodeMonoChannelSelect;
/* 
	MPEG1-Layer-3 encoder user configuration structure 
*/
typedef struct BDSP_Raaga_Audio_Mpeg1L3EncConfigParams
{
	/* Default Value = BDSP_Raaga_Audio_Mp3EncodeBitRate_e128kbps; */
	BDSP_Raaga_Audio_Mp3EncodeBitRate eMp3EncodeBitRate;

	/* CRC Not Supported */
	/* Default Value = 0; */
	uint32_t ui32AddCRCProtect;
				         
	/* Controls private bit setting in the header */
	/* Bit for private use, Default Value = BDSP_AF_P_eDisable */
	BDSP_AF_P_EnableDisable ePrivateBit; 

	/* Copyright bit setting in the header */
	/* Default Value = BDSP_AF_P_eDisable; */
	BDSP_AF_P_EnableDisable eCopyright;	

	/* Original bit setting in the header */
	/* Default Value = BDSP_AF_P_eDisable */
	BDSP_AF_P_EnableDisable	eOriginal;

	/* Indicates type of de-emphasis that shall be used */
	/* Default Value = BDSP_Raaga_Audio_Mp3EncodeEmphasisType_eNone; */ 
	BDSP_Raaga_Audio_Mp3EncodeEmphasisType eEmphasisType;
	
	/* A85 Compliant*/
	int32_t                         i32InputVolLevel;               /* in dB */
    int32_t                         i32OutputVolLevel;              /* in dB */

    /* Flag that indicates to encode in stereo or mono Default=0 (stereo)*/
    uint32_t                        ui32bEncodeMono;
    BDSP_Raaga_Audio_Mp3EncodeMonoChannelSelect eMp3EncodeMonoChannelSelect;

}BDSP_Raaga_Audio_Mpeg1L3EncConfigParams;


/*
   This data structure defines SBC Encoder user configuration parameters 
*/

typedef struct BDSP_Raaga_Audio_SbcEncoderUserConfig
{
   	uint32_t		NumBlocks; 		/* Default = 8; Possible Values: 4, 8, 12 and 16 */
	uint32_t		NumSubbands;	/* Default = 8; Possible Values: 4 and 8 */
	uint32_t		JointStereo;	/* Default = 1; 0 -> JointStereo OFF, 1 -> JointStereo ON */
	uint32_t		BitAllocation;	/* Default = 0; 0 -> Loudness, 1 -> SNR */
	uint32_t		BitPool;		/* Default = 45; Range of Values: 2 to (16 * Number of Channels * Number of Subbands) */

} BDSP_Raaga_Audio_SbcEncoderUserConfig;


/*
   This data structure defines AMR Decoder user configuration parameters 
*/

typedef struct  BDSP_Raaga_Audio_AmrUsrCfg
{
	uint32_t 	ui32OutMode;	/* Default = 1; Output channel configuration */
	uint32_t	ui32ScaleOp;	/* Default = 0; 0 -> FALSE, 1 -> TRUE */
	uint32_t	ui32ScaleIdx;	/* Default = 0; 0 -> +3dB, 1 -> +6dB, 2 -> +9dB, 3 -> +12dB */
	uint32_t	ui32OutputChannelMatrix[BDSP_AF_P_MAX_CHANNELS];

		/*
		For Multichannel (5.1) Output Port:
					ui32OutputChannelMatrix[0] = 0;
					ui32OutputChannelMatrix[1] = 1;
					
					for(i=2; i<BDSP_AF_P_MAX_CHANNELS; i++)
							ui32OutputChannelMatrix[i] = 0xFFFFFFFF; 
						
		For Stereo Output Port:
					for(i=0; i<BDSP_AF_P_MAX_CHANNELS; i++)
							ui32OutputChannelMatrix[i] = 0xFFFFFFFF;  this value shows invalid 
		*/


} BDSP_Raaga_Audio_AmrUsrCfg;

typedef struct  BDSP_Raaga_Audio_AmrConfigParams
{
	uint32_t				ui32NumOutPorts;	/* Default = 1; 1 -> Multichannel out, 2 -> Multichannel + Stereo out */

	BDSP_Raaga_Audio_AmrUsrCfg	sUsrOutputCfg[2];

} BDSP_Raaga_Audio_AmrConfigParams;

/*
   This data structure defines AMRWB Decoder user configuration parameters 
*/

typedef struct  BDSP_Raaga_Audio_AmrwbdecUsrCfg
{
	uint32_t 	ui32OutMode;	/* Default = 1; Output channel configuration */
	uint32_t	ui32ScaleOp;	/* Default = 0; 0 -> FALSE, 1 -> TRUE */
	uint32_t	ui32ScaleIdx;	/* Default = 0; 0 -> +3dB, 1 -> +6dB, 2 -> +9dB, 3 -> +12dB */
	uint32_t	ui32OutputChannelMatrix[BDSP_AF_P_MAX_CHANNELS];

		/*
		For Multichannel (5.1) Output Port:
					ui32OutputChannelMatrix[0] = 0;
					ui32OutputChannelMatrix[1] = 1;
					
					for(i=2; i<BDSP_AF_P_MAX_CHANNELS; i++)
							ui32OutputChannelMatrix[i] = 0xFFFFFFFF; 
						
		For Stereo Output Port:
					for(i=0; i<BDSP_AF_P_MAX_CHANNELS; i++)
							ui32OutputChannelMatrix[i] = 0xFFFFFFFF;  this value shows invalid 
		*/


} BDSP_Raaga_Audio_AmrwbdecUsrCfg;

typedef struct  BDSP_Raaga_Audio_AmrwbdecConfigParams
{
	uint32_t				ui32NumOutPorts;	/* Default = 1; 1 -> Multichannel out, 2 -> Multichannel + Stereo out */

	BDSP_Raaga_Audio_AmrwbdecUsrCfg	sUsrOutputCfg[2];

} BDSP_Raaga_Audio_AmrwbdecConfigParams;


typedef struct  BDSP_Raaga_Audio_iLBCdecUsrCfg
{
	uint32_t	mode;			/* Default = 20; 20->frame length 20msec, 30->frame length 30msec*/
	uint32_t	plc;			/* Default = 0; 0 -> no pakel loss, 1 -> packet loss*/
	uint32_t 	ui32OutMode;	/* Default = 1; Output channel configuration */
	uint32_t	ui32ScaleOp;	/* Default = 0; 0 -> FALSE, 1 -> TRUE */
	uint32_t	ui32ScaleIdx;	/* Default = 0; 0 -> +3dB, 1 -> +6dB, 2 -> +9dB, 3 -> +12dB */
	uint32_t	ui32OutputChannelMatrix[BDSP_AF_P_MAX_CHANNELS];

		/*
		For Multichannel (5.1) Output Port:
					ui32OutputChannelMatrix[0] = 0;
					ui32OutputChannelMatrix[1] = 1;
					
					for(i=2; i<BDSP_AF_P_MAX_CHANNELS; i++)
							ui32OutputChannelMatrix[i] = 0xFFFFFFFF; 
						
		For Stereo Output Port:
					for(i=0; i<BDSP_AF_P_MAX_CHANNELS; i++)
							ui32OutputChannelMatrix[i] = 0xFFFFFFFF;  this value shows invalid 
		*/


} BDSP_Raaga_Audio_iLBCdecUsrCfg;

typedef struct  BDSP_Raaga_Audio_iLBCdecConfigParams
{
	uint32_t				ui32NumOutPorts;	/* Default = 1; 1 -> Multichannel out, 2 -> Multichannel + Stereo out */

	BDSP_Raaga_Audio_iLBCdecUsrCfg	sUsrOutputCfg[2];

} BDSP_Raaga_Audio_iLBCdecConfigParams;



typedef struct  BDSP_Raaga_Audio_isacdecUsrCfg
{
    uint32_t    ui32BandMode;   /* Default = 0; 0 -> Narrowband, 1 -> Wideband  */
    uint32_t    plc;            /* Default = 0; 0 -> no pakel loss, 1 -> packet loss*/
    uint32_t    ui32OutMode;    /* Default = 1; Output channel configuration */
    uint32_t    ui32ScaleOp;    /* Default = 0; 0 -> FALSE, 1 -> TRUE */
    uint32_t    ui32ScaleIdx;   /* Default = 0; 0 -> +3dB, 1 -> +6dB, 2 -> +9dB, 3 -> +12dB */
    uint32_t    ui32OutputChannelMatrix[BDSP_AF_P_MAX_CHANNELS];
		/*
		For Multichannel (5.1) Output Port:
					ui32OutputChannelMatrix[0] = 0;
					ui32OutputChannelMatrix[1] = 1;
					
					for(i=2; i<BDSP_AF_P_MAX_CHANNELS; i++)
							ui32OutputChannelMatrix[i] = 0xFFFFFFFF; 
						
		For Stereo Output Port:
					for(i=0; i<BDSP_AF_P_MAX_CHANNELS; i++)
							ui32OutputChannelMatrix[i] = 0xFFFFFFFF;  this value shows invalid 
		*/

} BDSP_Raaga_Audio_isacdecUsrCfg;

typedef struct BDSP_Raaga_Audio_iSACdecConfigParams
{
    uint32_t    ui32NumOutPorts;      /*  Default = 1; 1 -> Multichannel out, 2 -> Multichannel + Stereo out */

    BDSP_Raaga_Audio_isacdecUsrCfg sUsrOutputCfg[2];

}BDSP_Raaga_Audio_iSACdecConfigParams;


typedef struct BDSP_Raaga_Audio_AmrEncoderUserConfig
{
    /*Enables or Disable DTX */
    uint32_t     ui32DtxEnable; /* ui32DtxEnable =1 => DTX Enable
                                   ui32DtxDisable=0 => DTX Disable
                                   Default => ui32DtxEnable=0 (Disabled)*/
    /*Set required Bitrate */
    uint32_t     ui32Bitrate;   /*ui32Bitrate = 0x0000 >> Mode Name => MR475 BitRate => 4.75 kbit/s
                                  ui32Bitrate = 0x0001 >> Mode Name => MR515   BitRate => 5.15 kbit/s
                                  ui32Bitrate = 0x0002 >> Mode Name => MR59    BitRate => 5.90 kbit/s
                                  ui32Bitrate = 0x0003 >> Mode Name => MR67    BitRate => 6.70 kbit/s
                                  ui32Bitrate = 0x0004 >> Mode Name => MR74    BitRate => 7.40 kbit/s
                                  ui32Bitrate = 0x0005 >> Mode Name => MR795   BitRate => 7.95 kbit/s
                                  ui32Bitrate = 0x0006 >> Mode Name => MR102  BitRate => 10.20 kbit/s
                                  ui32Bitrate = 0x0007 >> Mode Name => MR122  BitRate => 12.20 kbit/s
                                  Default => ui32Bitrate = 0x0000 >> Mode Name => MR475 BitRate => 4.75 kbit/s*/
} BDSP_Raaga_Audio_AmrEncoderUserConfig;

typedef struct  BDSP_Raaga_Audio_AmrwbplusUsrCfg
{

  int32_t fs;
  int32_t mono_dec_stereo;
  int32_t limiter_on ;
  int16_t FileFormat;
  int16_t fer_sim;                /* frame errasures simulation */

  uint32_t  ui32OutMode;    /* Default = 1; Output channel configuration */
  uint32_t  ui32ScaleOp;    /* Default = 0; 0 -> FALSE, 1 -> TRUE */
  uint32_t  ui32ScaleIdx;   /* Default = 0; 0 -> +3dB, 1 -> +6dB, 2 -> +9dB, 3 -> +12dB */
  uint32_t  ui32OutputChannelMatrix[BDSP_AF_P_MAX_CHANNELS];

        /*
        For Multichannel (5.1) Output Port:
                    ui32OutputChannelMatrix[0] = 4;
                    ui32OutputChannelMatrix[1] = 4;

                    for(i=2; i<BDSP_AF_P_MAX_CHANNELS; i++)
                            ui32OutputChannelMatrix[i] = 0xFFFFFFFF;

        For Stereo Output Port:
                    for(i=0; i<BDSP_AF_P_MAX_CHANNELS; i++)
                            ui32OutputChannelMatrix[i] = 0xFFFFFFFF;  this value shows invalid
        */


} BDSP_Raaga_Audio_AmrwbplusUsrCfg;

typedef struct  BDSP_Raaga_Audio_AmrwbplusConfigParams
{
    uint32_t                ui32NumOutPorts;    /* Default = 1; 1 -> Multichannel out, 2 -> Multichannel + Stereo out */

    BDSP_Raaga_Audio_AmrwbplusUsrCfg sUsrOutputCfg[2];

} BDSP_Raaga_Audio_AmrwbplusConfigParams;



/*
   This data structure defines DRA Decoder user configuration parameters 
*/


typedef struct BDSP_Raaga_Audio_DraUserOutput
{
	uint32_t ui32OutMode;	/*
							Default = 2
							Range - 1, 2, 7
							1 - Downmix to mono
							2 - Downmix to stereo
							7 - Downmix to 5.1
							The decoded output is downmixed to output configuration based on ui32OutMode value*/
	uint32_t ui32OutLfe;
							/*
							Default=1
							Range - 0, 1
							0 - LFE disabled at output
							1 - LFE enabled at output*/
	uint32_t ui32StereoMode;
							/*
							Default = 0
							Range - 0, 1
							0 - LoRo downmixed output
							1 - LtRt downmixed output
							This value is relevant only when ui32OutMode is 2.*/

	uint32_t ui32OutputChannelMatrix[BDSP_AF_P_MAX_CHANNELS];
						/*This matrix decides the output order of channels

							For Multi-Channel (5.1) ports:

							ui32OutputChannelMatrix[0] = 0;
							ui32OutputChannelMatrix[1] = 1;
							ui32OutputChannelMatrix[2] = 2;
							ui32OutputChannelMatrix[3] = 3;
							ui32OutputChannelMatrix[4] = 4;
							ui32OutputChannelMatrix[5] = 5;
							ui32OutputChannelMatrix[6] = 0xffffffff; this value shows invalid 
							ui32OutputChannelMatrix[7] = 0xffffffff; this value shows invalid 

							For Stereo Output Port:

									ui32OutputChannelMatrix[0] = 0;
									ui32OutputChannelMatrix[1] = 1;
									for(i=2; i<8; i++)
										ui32OutputChannelMatrix[i] = 0xffffffff; this value shows invalid 
						*/									
}BDSP_Raaga_Audio_DraUserOutput;


typedef struct BDSP_Raaga_Audio_DraConfigParams
{
	uint32_t					ui32NumOutPorts; 	/*Default = 2; 	5.1 PCM out and concurrent Stereo */		
	BDSP_Raaga_Audio_DraUserOutput	sUserOutputCfg[2];	

}BDSP_Raaga_Audio_DraConfigParams;


/*
   This data structure defines Real Audio LBR Decoder user configuration parameters 
*/

typedef struct BDSP_Raaga_Audio_ralbr_UserOutput
{
	uint32_t			ui32OutMode;
					/*	Default = 2
						Value according to BCOM_eACMOD 
					*/

	uint32_t			ui32OutputChannelMatrix[BDSP_AF_P_MAX_CHANNELS];
					/*	This matrix decides the output order of channels

						For Multi-Channel (5.1) ports:

						ui32OutputChannelMatrix[0] = 0;
						ui32OutputChannelMatrix[1] = 1;
						ui32OutputChannelMatrix[2] = 2;
						ui32OutputChannelMatrix[3] = 3;
						ui32OutputChannelMatrix[4] = 4;
						ui32OutputChannelMatrix[5] = 5;
						ui32OutputChannelMatrix[6] = 0xffffffff; 
						this value shows invalid 
						ui32OutputChannelMatrix[7] = 0xffffffff; this value shows invalid 

					For Stereo Output Port:

						ui32OutputChannelMatrix[0] = 0;
						ui32OutputChannelMatrix[1] = 1;

						for(i=2; i<8; i++)
							ui32OutputChannelMatrix[i] = 0xffffffff; 

						this value shows invalid 
					*/			
	
}BDSP_Raaga_Audio_RalbrUserOutput;

typedef struct BDSP_Raaga_Audio_RalbrConfigParams
{
	uint32_t			ui32NumOutPorts; 

						/*Default = 2; 	5.1 PCM out and concurrent Stereo */		

	BDSP_Raaga_Audio_RalbrUserOutput	sUserOutputCfg[2];	

}BDSP_Raaga_Audio_RalbrConfigParams;


/*
   This data structure defines Dolby Pulse Decoder user configuration parameters 
*/
typedef struct BDSP_Raaga_Audio_DolbyPulsePortConfig
{
	uint32_t 	ui32OutMode;	/* Default = 2, Output channel configuration */

	uint32_t 	ui32OutLfe;		/* Default = 0, Output LFE channel present */

	uint32_t 	ui32DualMode;	/* Default = 0, Dual mono reproduction mode */

	uint32_t	ui32LoRoDownmix;/* Default = 0, 0 -> Lt/Rt Downmix, 1 -> Lo/Ro Downmix, 2 -> ARIB Downmix
								   Dolby Pulse decoder can be configured for Lt/Rt downmixing only in the single decode usage case.
								   If dual decode with mixing is enabled then both the primary and the secondary decode have to be set
								   for Lo/Ro downmixing always and Lt/Rt downmixing should never be allowed. */

	uint32_t	ui32RfMode;		/* Default = 0, 0 -> Line Mode, 1 -> RF Mode */

	uint32_t	ui32DrcCut;		/* Default = 100, Dynamic range compression cut scale factor in percent, 0 to 100 */

	uint32_t	ui32DrcBoost;	/* Default = 100, Dynamic range compression boost scale factor in percent, 0 to 100 */

	uint32_t	ui32OutputChannelMatrix[BDSP_AF_P_MAX_CHANNELS];

	/*
	For Multichannel (5.1) Output Port:
		for(i=0; i<6; i++)
			ui32OutputChannelMatrix[i] = i;

		for(i=6; i<BDSP_AF_P_MAX_CHANNELS; i++)
				ui32OutputChannelMatrix[i] = 0xFFFFFFFF; This represents INVALID


	For Stereo Output Port:
		for(i=0; i<2; i++)
			ui32OutputChannelMatrix[i] = i;

		for(i=2; i<BDSP_AF_P_MAX_CHANNELS; i++)
			ui32OutputChannelMatrix[i] = 0xFFFFFFFF; This represents INVALID
	*/

    uint32_t    ui32ApplyGain;      /* Default = 0, 0 -> FALSE, 1 -> TRUE */

    int32_t     i32GainFactor;      /* Default = 0x00B504F3, corresponding to -3dB in Q8.24 format; valid only if ui32ApplyGain = 1 */

} BDSP_Raaga_Audio_DolbyPulsePortConfig;

typedef struct BDSP_Raaga_Audio_DolbyPulseUserConfig
{
    /* This enum lets the mixer know if the decoder type is primary or secondary or sound effects. */    
    BDSP_AF_P_DecoderType 	eDecoderType; /* Default : BDSP_AF_P_DecoderType_ePrimary */

	/* BDSP_AF_P_DolbyMsUsageMode_eMpegConformanceMode Should only be enabled for 
	MPEG Conformance testing and in this only	one 5.1 Multichannel output port should be enabled.   */
	BDSP_AF_P_DolbyMsUsageMode 		eDolbyPulseUsageMode; /* Default : BDSP_AF_P_DolbyMsUsageMode_eSingleDecodeMode */

	uint32_t	ui32NumOutPorts;	/* Default = 1, 1 -> Multichannel, 2 -> Multichannel + Stereo */

    uint32_t    ui32RefDialnormLevel;   /* Default = 124, corresponding to -31dB LKFS (31*4) */

    uint32_t    ui32DefDialnormLevel;   /* Default = 108, corresponding to -27dB LKFS (27*4); Maximum = 127 */

	BDSP_Raaga_Audio_DolbyPulsePortConfig	sOutPortCfg[2];

} BDSP_Raaga_Audio_DolbyPulseUserConfig;

/*
   This data structure defines Audessey Volume Decoder user configuration parameters 
*/

typedef struct BDSP_Raaga_Audio_AudysseyVolUserConfig
{
		
    uint32_t	 ui32NumChannels;		/*	No of channels */
    uint32_t     ui32AudysseyVolBypass;	/*	Default : 0, range [0,1]	*/
										/*	0 -> Processing enable, 1-> total disable of the processing*/

    int32_t      i32ChannelMask;		/* Specifying the channels where gain is to be applied*/
										/* Default:    0xFF	All channels */
										/* Meaning of bits

											0 -> L, 1 -> R, 2 -> C, 3 -> LFE
											4 -> Ls, 5-> Rs, 6 -> Lb, 7 -> Rb
										*/

    int32_t      i32AudysseyVolInit;       
										/* Default : 0, range [0,1]*/
										/* 0 -> Do nothing, 1-> Reset(re-initialize) ADV*/
            
    int32_t      i32HeadroomOffset;		/* In q22 format. headroom_offset in dB */
										/* Default : 0, range waiting response from Audyssey Labs*/

    int32_t      i32SwAudysseyVol;		/* ADV On/Off */
										/* Default : 1, range [0,1]*/
										/* 0 -> off, 1-> on*/

    int32_t      i32SwDynEQ;			/* Dynamic EQ On/Off/Ramp off  3 modes 0, 1 and 2 */
										/* Default : 0, range [0,1,2]*/
										/* 0 -> off, 1-> on, 2 -> Ramp off */

    int32_t      i32SwDynSurrGain;		/* Switch for surround channel */
										/* Default : 1, range [0,1]*/
										/* 0 -> off, 1-> on*/
    int32_t      i32SwHiLoCmpress;		/* Switch for having boost in calc cntr gain    */
										/* Default : 1, range [0,1]*/
										/* 0 -> off, 1-> on */

    int32_t      i32dBVolSetting;		/* In q22 format for gain function and Dynamic EQ */
										/* Default : 0, float range [0, -50] */
    int32_t      i32GCF;				/* In q22 format	Gain Compression Factor */
										/* Default : 0x733333 (1.8 in float), float range [1, 2.4] */

    int32_t      i32chCalbGain[8];		/* q22 chCAlbGain in dB in floating format. Default 0 */

    int32_t      i32chCalcLevel[8];		/* q22 chCalcLevel in dB in floating format Default 0 */
    
} BDSP_Raaga_Audio_AudysseyVolUserConfig;


/*
   This data structure defines Broadcom 3D surround user configuration parameters
*/

typedef enum BDSP_Raaga_Audio_eSurroundMode
{
    BDSP_Raaga_Audio_eSurroundMode_NoSurround=0,
    BDSP_Raaga_Audio_eSurroundMode_StereoWidening,
    BDSP_Raaga_Audio_eSurroundMode_Broadcom3DSurround,
    BDSP_Raaga_Audio_eSurroundMode_LAST,
    BDSP_Raaga_Audio_eSurroundMode_INVALID=0x7fffffff

} BDSP_Raaga_Audio_eSurroundMode;

/* Different 3D surround modes correspond to different
surround speaker locations (theta) and height (phi) */
/* BDSP_Raaga_Audio_eBroadcom3DSurroundMode_Centre
Speaker position      = 0
theta                 = 30
phi                   = 15
Speaker position      = 1
theta                 = 330
phi                   = 15
Speaker position      = 2
theta                 = 90
phi                   = 0
Speaker position      = 3
theta                 = 270
phi                   = 0
Speaker position      = 4
theta                 = 0
phi                   = 15
Speaker position      = 5
theta                 = 180
phi                   = 15
*/
/* BDSP_Raaga_Audio_eBroadcom3DSurroundMode_Wide
Speaker position      = 0
theta                 = 30
phi                   = 15
Speaker position      = 1
theta                 = 330
phi                   = 15
Speaker position      = 2
theta                 = 105
phi                   = 30
Speaker position      = 3
theta                 = 255
phi                   = 30
Speaker position      = 4
theta                 = 0
phi                   = 15
Speaker position      = 5
theta                 = 180
phi                   = 15
*/
/* BDSP_Raaga_Audio_eBroadcom3DSurroundMode_Extrawide
Speaker position      = 0
theta                 = 60
phi                   = 30
Speaker position      = 1
theta                 = 300
phi                   = 30
Speaker position      = 2
theta                 = 120
phi                   = 45
Speaker position      = 3
theta                 = 240
phi                   = 45
Speaker position      = 4
theta                 = 0
phi                   = 30
Speaker position      = 5
theta                 = 180
phi                   = 30
*/
typedef enum BDSP_Raaga_Audio_eBroadcom3DSurroundMode
{
    BDSP_Raaga_Audio_eBroadcom3DSurroundMode_Centre=0,
    BDSP_Raaga_Audio_eBroadcom3DSurroundMode_Wide,
    BDSP_Raaga_Audio_eBroadcom3DSurroundMode_Extrawide,
    BDSP_Raaga_Audio_eBroadcom3DSurroundMode_LAST,
    BDSP_Raaga_Audio_eBroadcom3DSurroundMode_INVALID=0x7fffffff
} BDSP_Raaga_Audio_eBroadcom3DSurroundMode;

typedef struct BDSP_Raaga_Audio_Broadcom3DSurroundConfigParams
{
    int32_t                                  i32BRCM3DSurroundEnableFlag;    /*Default 1 Enable =1 Disable = 0*/

    BDSP_Raaga_Audio_eBroadcom3DSurroundMode eBroadcom3DSurroundMode;        /*Default BDSP_Raaga_Audio_eBroadcom3DSurroundMode_Wide*/

}BDSP_Raaga_Audio_Broadcom3DSurroundConfigParams;

#define BDSP_Raaga_Audio_Brcm3DSurroundConfigParams BDSP_Raaga_Audio_Broadcom3DSurroundConfigParams

/*
   This data structure defines Mixer user configuration parameters 
*/
typedef struct BDSP_Raaga_Audio_MixerConfigParams
{
	uint32_t        ui32NumInput;           /* The number of input ports to the mixer */   
	uint32_t        MixingCoeffs[BDSP_AF_P_MAX_IP_FORKS][BDSP_AF_P_MAX_CHANNELS];
                                                              /* Q4.28 foremat : This field is only for sound effect mixing.
																 Separate mixing coefficients can be provided 
																 per channel per input basis.
															  */
	int32_t         i32UserMixBalance;
}BDSP_Raaga_Audio_MixerConfigParams;

/*
   This data structure defines MS10 DD transcode  user configuration parameters 
*/
typedef struct  BDSP_Raaga_Audio_DDTranscodeConfigParams
{
	/* Audio Coding mode, Possible Values 0(1+1),2(2/0),7(3/2), Default: 7(3/2)*/
	int32_t  i32AudCodMode;
 
	/*  1 is LFE enabeld: 0 is LFE disabled, Default Value: 1*/
	int32_t  i32LoFreqEffOn;
 
	/* This is used to enable/disable SPDIF header.  Enable =1 Disable =0. Default value = Enable.*/
	BDSP_AF_P_EnableDisable eSpdifHeaderEnable;
 
	/*This flag is set to enable when the Encoder is used in transcode mode Enable =1 Disable =0. Default value = Enable. */
	BDSP_AF_P_EnableDisable   eTranscodeEnable;

}BDSP_Raaga_Audio_DDTranscodeConfigParams;

/*
   This data structure defines Mono Downmix user configuration parameters 
*/
typedef struct BDSP_Raaga_Audio_MonoDownMixConfigParams
{
	int32_t i32MonoDownMixEnableFlag; /*Default=1 Enable =1 Disable =0 */

}BDSP_Raaga_Audio_MonoDownMixConfigParams;


/*

   This data structure defines Ms10 DDP decoder user configuration parameters 
*/
 
typedef struct  BDSP_Raaga_Audio_DDPMs10UserOutput
{
    /* This is LFE ON/OFF flag and can take  two values 0 or 1 
     default value for this field is 0 
	*/
    int32_t i32OutLfe;                         
    /*	enum { GBL_MODE11=0, GBL_MODE_RSVD=0, GBL_MODE10, GBL_MODE20, 
		GBL_MODE30, GBL_MODE21, GBL_MODE31, GBL_MODE22, GBL_MODE32 };
		i32OutMode =7 default value;
	*/
    int32_t i32OutMode;
    /*	preferred stereo mode
		enum { GBL_STEREOMODE_AUTO=0, GBL_STEREOMODE_SRND, GBL_STEREOMODE_STEREO };
		i32StereoMode = 0 is default value ;
	*/
    int32_t i32StereoMode;  
    /* dual mono downmix mode
	   enum { GBL_DUAL_STEREO=0, GBL_DUAL_LEFTMONO, GBL_DUAL_RGHTMONO, GBL_DUAL_MIXMONO };
       i32DualMode = 0 is default value;
	*/
    int32_t i32DualMode;
    /* karaoke capable mode 
       enum { NO_VOCALS=0, GBL_VOCAL1, GBL_VOCAL2, GBL_BOTH_VOCALS };
       i32Kmode = 3;
    */
    int32_t i32Kmode;   
    /* This i32ExtDnmixEnabled flag which can take  two values 0 or 1 based on disable/enable option
       default value for this i32ExtDnmixEnabled field is 0
	*/
    int32_t i32ExtDnmixEnabled;            
    int32_t i32ExtDnmixTab[DDP_DEC_GBL_MAXPCMCHANS][DDP_DEC_GBL_MAXPCMCHANS]; 
    /* This i32ExtKaraokeEnabled flag which can take  two values 0 or 1 based on disable/enable option
       default value for this i32ExtKaraokeEnabled field is 0
    */
    int32_t i32ExtKaraokeEnabled;            
    int32_t i32Extv1Level;                  
    int32_t i32Extv1Pan;                  
    int32_t i32Extv2Level;                  
    int32_t i32Extv2Pan;                  
    int32_t i32ExtGmLevel;                   
    int32_t   i32ExtGmPan;                  
    /*This is channel matrix of size 8 where each index can take any values from 0 to 7
	  Depending upon  the channel routing done    
	*/
    uint32_t   ui32OutputChannelMatrix[BDSP_AF_P_MAX_CHANNELS];
}BDSP_Raaga_Audio_DDPMs10UserOutput;
 
typedef struct  BDSP_Raaga_Audio_DDPMs10ConfigParams
{
   /* p_user_cfg->i32CompMode = 2;*/
    int32_t                i32CompMode;  
    int32_t                i32PcmScale;  
   /* p_user_cfg->i3DynScaleHigh = 0x7FFFFFFF;    0x0 to  0x7FFFFFFF in Q1.31 format */
    int32_t                i32DynScaleHigh;    
   /* p_user_cfg->i32DynScaleLow = 0x7FFFFFFF;    0x0 to  0x7FFFFFFF in Q1.31 format */
    int32_t                i32DynScaleLow;    
   /*  This value "i32NumOutPorts" can be set to 1 or 2 based on output ports */
    int32_t                i32NumOutPorts;     


	/* This value is used to enable/disable stream dialog normalization value.  0 is for Disable and 1 is for Enable
	   Default is Enable
	 */
	 int32_t				i32StreamDialNormEnable;

	/*	This value indicates how far the average dialogue level is below digital 100 percent. Valid
		values are 1-31. The value of 0 is reserved and it means dialog normalization is turned off. The values of 1 to 31 are interpreted as -1 dB to -31dB 
		with respect to digital 100 percent. If the reserved value of 0 is received, the decoder will not do any dialog normalization, 
		Default value is 0 
	*/

	int32_t					i32UserDialNormVal;

    /* These are user config parameters required from user  */
    BDSP_Raaga_Audio_DDPMs10UserOutput  sUserOutputCfg[2];
    uint32_t                ui32SubstreamIDToDecode; /*Default value =0 ; Range 0-3 for MS10*/
 
} BDSP_Raaga_Audio_DDPMs10ConfigParams;


/*
   This data structure defines Audyssey user configuration parameters 
*/

typedef struct BDSP_Raaga_Audio_ABXConfigParams
{	
    uint32_t	ui32OperationMode;	/* (Defualt 0), 1->Run ABX Processing, 0->Bypass ABX Processing*/

	uint32_t	ui32FilterSet;		/* (Default 1), 1->is for systems that roll-off lower (42/46 inch TV), 2->is for systems that roll-off higher(22 inch TV) */

    uint32_t	ui32HarmonicGain;	/* default is 6(3 dB), rangaes from 0 to 30, every step increament is equivalent to 0.5 dB step*/

    uint32_t	ui32DryGain;		/* (default is 0) typically always 0, every step increament is equivalent to 0.5 dB step */

} BDSP_Raaga_Audio_ABXConfigParams;


/* Substucture for General node configuration */
typedef struct BDSP_Raaga_Audio_TopLevelUserConfigCsTd
{

	int32_t                         i32mEnable;			/*SRS StudioSound processing enable*/
														/*Range [0, 1], Default : 1*/
														/*1:Enable SRS StudioSound processing, 0:Bypass SRS StudioSound*/  

	int32_t                         i32mBypassGain;		/*SRS StudioSound Bypass Gain*/
														/*Range: Floating point: (0.0 - 1.0), Default: 1.0*/
														/*Fixed point: (0x0 - 0x7fffffff), default 0x7fffffff, QFormat - 1.31*/



    int32_t                         i32mInputGain;		/*	SRS StudioSound Input Gain */ 
														/*	Range: Floating point: (0.0 - 1.0), Default: 1.0 */
														/*	Fixed point: (0x0 - 0x7fffffff), default 0x7fffffff, QFormat - 1.31 */

	int32_t                         i32mHeadroomGain;	/*  SRS StudioSound Headroom Gain */
														/*	Range: Floating point: (0.0 - 1.0), Default: 1.0 */
														/*	Fixed point: (0x0 - 0x7fffffff), default 0x7fffffff, QFormat - 1.31 */

    int32_t                         i32mInputMode;		/*	Channel configuration present in input*/
														/*	Range: [0, 1, 2, 3], Default: 3*/
														/*	0 : 1_0_0
															1 : 2_0_0
															2 : 3_2_1
															3 : LtRt
														*/
}BDSP_Raaga_Audio_TopLevelUserConfigCsTd;



typedef struct BDSP_Raaga_Audio_TopLevelUserConfigEquHl
{
    int32_t                         i32mInputGain;		/*SRS StudioSound Input Gain      */ 
														/*Range: Floating point: (0.0 - 1.0), Default: 1.0*/
														/*Fixed point: (0x0 - 0x7fffffff), default 0x7fffffff, QFormat - 1.31    */

	int32_t                         i32mHeadroomGain;	/*SRS StudioSound Headroom Gain*/
														/*Range: Floating point: (0.0 - 1.0), Default: 1.0*/
														/*Fixed point: (0x0 - 0x7fffffff), default 0x7fffffff, QFormat - 1.31*/

    int32_t                         i32mInputMode;		/*Channel configuration present in input*/
														/*Range: [0, 1, 2, 3], Default: 3*/
														/* 0 : 1_0_0
															1 : 2_0_0
														    2 : 3_2_1
															3 : LtRt
														*/
                                                    
    int32_t                         i32mOutputGain;		/* SRS StudioSound Output Gain */
														/* Range: Floating point: (0.0 - 1.0), Default: 1.0 */
														/* Fixed point: (0x0 - 0x7fffffff), default 0x7fffffff, QFormat - 1.31*/ 
 

}BDSP_Raaga_Audio_TopLevelUserConfigEquHl;

/*
   This data structure defines DDRE user configuration parameters 
*/

typedef struct  BDSP_Raaga_DDReencodeUserConfig
{
	
    uint32_t        ui32CompMode;   /*  Compression mode setting,Possible values: 1(RF mode) and 0(Line mode).
Default: 0(Line mode). Multichannel output can only configured for
Line mode in the  current reference implementation*/

    uint32_t        ui32DrcCutFac;   /* DRC Cut Scale factor in 1.31 Format. Possble Values: 0 to 0x7FFFFFFF.
Default value: 0x7FFFFFFF */

    uint32_t        ui32DrcBoostFac; /* DRC Boost Scale factor in 1.31 Format. Possble Values: 0 to 0x7FFFFFFF.
Default value: 0x7FFFFFFF */

	uint32_t 		ui32OutMode;	/*	Output channel configuration.Possible Values: 2(Stereo) or 7(3/2 Multichannel), 
										Default = 2(Stereo?). None of the other possible ACMODS are allowed. */

	uint32_t 		ui32OutLfe;		/*	Output LFE channel present .Possible Values: 0: LFE disabled, 1: LFE enabled, 
										Default = 0(Disabled). Can only enabled with 3/2 Multichannel audio*/

	uint32_t		ui32StereoMode; /*	Flag indicating the Stereo downmixed mode, is only effective if Outputmode is Stereo. 
										Possible Values: 0(LtRt), 1(LoRo) and 2(Arib). Default value: 0(LtRt) */ 

	uint32_t 		ui32DualMode;	/*	Flag indicating the dualmono mode which is to be used if the incoming stream is dual mono. 
										Possible values: 0(Stereo), 1(Left_Only) and 2(Right_Only). Default value: 0(Stereo). */


	uint32_t		ui32OutputChannelMatrix[BDSP_AF_P_MAX_CHANNELS]; /*This is channel matrix of size 8 where each index can take any values from 0 to 7 depending upon  the channel routing done*/

}BDSP_Raaga_DDReencodeUserConfig;


typedef struct BDSP_Raaga_Audio_DDReencodeConfigParams
{
	/* Flag indicating that external Pcm mode is active. This flag is to be set active if input is not from the Dolby MS11 decoders(DDP and Dolby Pulse */
    uint32_t        ui32ExternalPcmEnabled;

	/* Compression profiles applicable. The Settings are only if the input is from a External PCM source.
	    Possible values:
	0 : "No Compression"
	1 : "Film Standard Compression"
	2 : "Film Light Compression"
    3 : "Music Standard Compression"
	4 : "Music Light Compression"
    5 : "Speech Compression"	
	    Default Value: 1: "Film Standard Compression"*/
    uint32_t        ui32CompProfile;

	/*Index for center level in the downmixing case. Possible Values: 0-3. Default: 0. Used only if External PCM Mode is enabled or PCM Soundeffect Mixing is the only context enabled*/
    uint32_t   		ui32CmixLev;

	/*Index for surround level in the downmixing case. Possible Values: 0-3. Default: 0. Used only if External PCM Mode is enabled or PCM Soundeffect Mixing is the only context enabled */
    uint32_t   		ui32SurmixLev;

	/* Dolby Surround Encoding Mode.
	Possible Values:
		0: NOT INDICATED
		1: NOT SURROUND ENCODED
		2: SURROUND ENCODED
		Default: 0(NOT INDICATED).
	Used only if External PCM Mode is enabled or PCM Soundeffect Mixing is the only context enabled */
    uint32_t   		ui32DsurMod;

	/* Dialog level of incoming PCM in dB. Possible values: 0-31. Default: 31 Used only if External PCM Mode is enabled or PCM Soundeffect Mixing is the only context enabled */
    uint32_t     	ui32DialNorm;
	

	/* Total number of output ports enabled. If only stereo output port is set to enabled then the value should be 1, if Multichannel 5.1 PCM output is enabled the value should be 2. Only 1 output port with Multichannel 5.1 PCM output port is NOT allowed */
    uint32_t        ui32NumOutPorts;

    /* Per Output port configuration for DDRE */
    BDSP_Raaga_DDReencodeUserConfig        sUserOutputCfg[2];

}BDSP_Raaga_Audio_DDReencodeConfigParams;

/*
   This data structure defines DV258 user configuration parameters 
*/
typedef struct BDSP_Raaga_Audio_DV258ConfigParams
{
	/*General Settings*/

	/*	Range of values -  0 or 1. As per the new guyidelines from Dolby, i32DolbyVolumeEnable =1(True) for both when DV is enabled and disabled. 
		The controlling factor is now i32LvlEnable. Default value is 1 (True)*/
	int32_t		i32DolbyVolumeEnable;			
	
	/*	Volume modeler on/off. Possible values are 0(off) and 1(on). 
		Default 0(off). Always set to off for DV258 
	*/
	int32_t		i32VlmMdlEnable;

	/*	Flag to operate Dolby Volume in half mode. Flag is always set to off. Range [0,1]. Default ->0 (disable)*/
	int32_t     	i32HalfmodeFlag;				

	/*	Enable midside processing. Range [0,1]. Default ->1(enable).
		Flag is always set to enable for Dv258.*/
	int32_t		i32EnableMidsideProc;			

	/*	Volume Leveler Settings */	

	/*	Range of values -  0=FALSE , 1=TRUE. 
		i32LvlEnable = 1 When Dolby Volume is Enabled. 
		i32LvlEnable = 0 When Dolby Volume is Disabled. 
		Default value : 1 (TRUE - volume leveler enabled)*/
	int32_t		i32LvlEnable;					

	/*Range of values - [0 to 10]. Default value: 9 */
	int32_t		i32LvlAmount;					


	/*	System Settings*/

	/*	Input reference level in dB. Range of values(in dB )- (0 to 130 in steps of 0.0625 dB). Possible values are 0 to 2080. Default value :1360(85 dB)*/
	int32_t		i32InputReferenceLevel;			

	/*	The gain, if any, applied to the signal prior to entering DV258 in dB.
		Range of values(in dB )- (-30 to 30 in steps of 0.0625 dB). Possible values are -480 to 480. Default value : 0 (0 dB)*/
	int32_t		i32Pregain;						

	/*	Output reference level in dB. Range of values(in dB )- (0 to 130 in steps of 0.0625 dB). 
		Possible values are 0 to 2080. Default value :1360(85 dB)*/
	int32_t		i32OutputReferenceLevel;

	/*	Calibration offset in dB. Range of values(in dB)- (-30 to 30 in steps of 0.0625 dB). Possible values are -480 to 480. Default value    : 0( 0 dB)*/
	int32_t		i32CalibrationOffset;	

	/*	Volume Modeler Settings*/
	
	/*	Volume level gain in dB -- applied after dolby volume.
		Range of values(in dB) - (-96 to 30 in steps of 0.0625 dB). Possible values are -1536 to 480.
		Default value    : 0 (0 dB)*/
	int32_t		i32AnalogVolumeLevel;			
	
	/*	Volume level gain in dB -- applied by dolby volume.
		Range of values(in dB) - (-96 to 30 in steps of 0.0625 dB). Possible values are -1536 to 480. 
		Default value    : 0 (0 dB)*/

	int32_t		i32DigitalVolumeLevel;			

	/*User-input forced reset flag. Range [0,1], default 0*/
	int32_t		i32ResetNowFlag;				

	/* Limiter Settings */

	/*	Enable Limter. When DV is enabled limiter_enable = TRUE. 
		When DV is disabled limiter_enable = FALSE*/
	int32_t     	i32LimiterEnable;				

}BDSP_Raaga_Audio_DV258ConfigParams;



/*

   This data structure defines Dolby MS DDP decoder user configuration parameters 
*/
 
typedef struct  BDSP_Raaga_Audio_DDPMultiStreamUserOutput
{
   /* p_user_cfg->i32CompMode = 2;*/
	/*Custom_0=0, Custom_1 = 1, Line = 2, RF =3 , DRC_OFF = 4 */
    int32_t                i32CompMode;  
	/* p_user_cfg->i32PcmScale = 100;    The value is in percentage, 0 to 100. */
    int32_t                i32PcmScale;  
   /* p_user_cfg->i3DynScaleHigh = 100;    The value is in percentage, 0 to 100. */
    int32_t                i32DynScaleHigh;    
   /* p_user_cfg->i32DynScaleLow = 100;    The value is in percentage, 0 to 100. */
    int32_t                i32DynScaleLow;    
    /* This is LFE ON/OFF flag and can take  two values 0 or 1 
     default value for this field is 0 
	*/
    int32_t i32OutLfe;                         
    /*	enum { GBL_MODE11=0, GBL_MODE_RSVD=0, GBL_MODE10, GBL_MODE20, 
		GBL_MODE30, GBL_MODE21, GBL_MODE31, GBL_MODE22, GBL_MODE32 };
		i32OutMode =7 default value;
	*/
    int32_t i32OutMode;
    /*	preferred stereo mode
		enum { GBL_STEREOMODE_AUTO=0, GBL_STEREOMODE_SRND, GBL_STEREOMODE_STEREO };
		i32StereoMode = 0 is default value ;
	*/
    int32_t i32StereoMode;  
    /* dual mono downmix mode
	   enum { GBL_DUAL_STEREO=0, GBL_DUAL_LEFTMONO, GBL_DUAL_RGHTMONO, GBL_DUAL_MIXMONO };
       i32DualMode = 0 is default value;
	*/
    int32_t i32DualMode;
    /* karaoke capable mode 
       enum { NO_VOCALS=0, GBL_VOCAL1, GBL_VOCAL2, GBL_BOTH_VOCALS };
       i32Kmode = 3;
    */
    int32_t i32Kmode;   
    /* This i32ExtDnmixEnabled flag which can take  two values 0 or 1 based on disable/enable option
       default value for this i32ExtDnmixEnabled field is 0
	*/
    int32_t i32ExtDnmixEnabled;            
    int32_t i32ExtDnmixTab[DDP_DEC_GBL_MAXPCMCHANS][DDP_DEC_GBL_MAXPCMCHANS]; 
    /* This i32ExtKaraokeEnabled flag which can take  two values 0 or 1 based on disable/enable option
       default value for this i32ExtKaraokeEnabled field is 0
    */
    int32_t i32ExtKaraokeEnabled;            
    int32_t i32Extv1Level;                  
    int32_t i32Extv1Pan;                  
    int32_t i32Extv2Level;                  
    int32_t i32Extv2Pan;                  
    int32_t i32ExtGmLevel;                   
    int32_t   i32ExtGmPan;                  
    /*This is channel matrix of size 8 where each index can take any values from 0 to 7
	  Depending upon  the channel routing done    
	*/
    uint32_t   ui32OutputChannelMatrix[BDSP_AF_P_MAX_CHANNELS];

} BDSP_Raaga_Audio_DDPMultiStreamUserOutput;
 

typedef enum BDSP_Raaga_Audio_eDDPUsageMode
{ 
    BDSP_Raaga_Audio_eSingleDecodeMode=0,
    BDSP_Raaga_Audio_eMS10DecodeMode, 
    BDSP_Raaga_Audio_eMS11DecodeMode,
    BDSP_Raaga_Audio_eDDPUsageMode_eLAST,
    BDSP_Raaga_Audio_eDDPUsageMode_eINVALID=0x7fffffff

} BDSP_Raaga_Audio_eDDPUsageMode;


typedef struct  BDSP_Raaga_Audio_DDPMultiStreamConfigParams
{
	/* Possible Values: 0(Single Decode), 1(MS10 Mode) and 2(MS11 Mode)	*/
	BDSP_Raaga_Audio_eDDPUsageMode	eDdpUsageMode;
	
    /* This enum lets the mixer know if the decoder type is primary or secondary or sound effects. */    
    BDSP_AF_P_DecoderType 	eDecoderType;

	uint32_t                ui32SubstreamIDToDecode; /*Default value =0 ; Range 0-3 for MS10*/
	
	/*  This value "i32NumOutPorts" can be set to 1 or 2 based on output ports */
    int32_t                i32NumOutPorts;     

	/* This value is used to enable/disable stream dialog normalization value.  0 is for Disable and 1 is for Enable
	   Default is Enable
	 */
	 int32_t				i32StreamDialNormEnable;

	/*	This value indicates how far the average dialogue level is below digital 100 percent. Valid
		values are 1-31. The value of 0 is reserved and it means dialog normalization is turned off. The values of 1 to 31 are interpreted as -1 dB to -31dB 
		with respect to digital 100 percent. If the reserved value of 0 is received, the decoder will not do any dialog normalization, 
		Default value is 0 
	*/
	int32_t					i32UserDialNormVal;

	/* This value indicates if Meta data log needs to be enabled. It should be enabled only for certification.
       Default value is Disable.
     */
    BDSP_AF_P_EnableDisable eMetaDataLogsEnable;

    /* These are user config parameters required from user  */
    BDSP_Raaga_Audio_DDPMultiStreamUserOutput  sUserOutputCfg[2];

} BDSP_Raaga_Audio_DDPMultiStreamConfigParams;

typedef struct BDSP_Raaga_Audio_BiquadCoefs
{
  int32_t i32A0Coeff; /* Q30 Format. Default: 0x40000000. */
  int32_t i32A1Coeff; /* Q30 Format. Default: 0x00000000. */
  int32_t i32A2Coeff; /* Q30 Format. Default: 0x00000000. */
  int32_t i32B1Coeff; /* Q30 Format. Default: 0x00000000. */
  int32_t i32B2Coeff; /* Q30 Format. Default: 0x00000000. */
} BDSP_Raaga_Audio_BiquadCoefs;

typedef struct BDSP_Raaga_Audio_DbeChannelConfig
{
    /* 0 : don't use the HP filter,1 : use the HP filter . Default: 1 */
   int32_t          i32UseHighPass;
   /* Headroom creation which the DBE module should do on its own. Range: 0-6. Default: 0 */
   int32_t          i32HeadRoom;
   /* Target level for the output signal in Q31 Format. Default: 0x72148300(0.89125094f) */
   int32_t          i32TargetLevel;
   /* Max Gain in Q27 Format. Default: 0x24FCC016(4.62341325f) */
   int32_t          i32MaxGain;
   /* Min Gain in Q27 Format. Default: 0x17B6F7(0.01157945f) */
   int32_t          i32MinGain;
   /* Gain Increment in Q30 Format. Default: 0x40015D80(1.00008334f) */
   int32_t          i32GainIncrement;
   /* Gain Decrement in Q30 Format. Default: 0x3fdde700(0.99791884f)*/
   int32_t          i32GainDecrement;


   /* Biquad Coefficients in Q30 format for the High Pass Filter
   i32A0Coeff = 0x3FB45998
   i32A1Coeff = 0x80974CCF
   i32A2Coeff = 0x3FB45998
   i32B1Coeff = 0x8097A63B
   i32B2Coeff = 0x3F690C9C
   */
   BDSP_Raaga_Audio_BiquadCoefs sDbeHighPassCoeffs;

   /* Biquad Coefficients in Q30 format for the Band Pass Filter
   i32A0Coeff = 0x001AC3A7
   i32A1Coeff = 0x00000000
   i32A2Coeff = 0xFFE53C59
   i32B1Coeff = 0x80363AB0
   i32B2Coeff = 0x3FCA78B1
   */
   BDSP_Raaga_Audio_BiquadCoefs	sDbeBandPassCoeffs;
}BDSP_Raaga_Audio_DbeChannelConfig;

typedef struct  BDSP_Raaga_Audio_DbeConfigParams
{
    /* Enable is 1, Disable is 0. Default: 1 */
    int32_t                             i32DbeEnable;
    /* Structure for per channel DBE configuration */
    BDSP_Raaga_Audio_DbeChannelConfig        sDbeChannelConfig[2];
} BDSP_Raaga_Audio_DbeConfigParams;


typedef struct BDSP_Raaga_Audio_ACF_ChannelConfig
{
/* Headroom creation which the ACF module should do on its own. Range: 0-8.        Default: 0 */
    int32_t                    i32HeadRoom;

  /* Biquad Coefficients in Q30 format for the Acoustic Compensation Filters
   * Default for all filters are following:
     i32A0Coeff = 0x40000000
     i32A1Coeff = 0x00000000
     i32A2Coeff = 0x00000000
     i32B1Coeff = 0x00000000
     i32B2Coeff = 0x00000000
    */
    BDSP_Raaga_Audio_BiquadCoefs    sAcfFilterCoeffs[14];
}BDSP_Raaga_Audio_ACF_ChannelConfig;

typedef struct  BDSP_Raaga_Audio_AcfConfigParams
{
    BDSP_Raaga_Audio_ACF_ChannelConfig   sAcfChannelConfig[2];
} BDSP_Raaga_Audio_AcfConfigParams;



typedef struct BDSP_Raaga_Audio_AvlpFilterConfig
{

    /* Headroom creation which the AVLP module should do on its own. Range 0-6.Default:0 */

    int32_t		i32HeadRoom;
    /* Biquad Coefficients in Q30 format for the AVL Plus Biquad Filters
     * Default for all filters are following:
	     i32A0Coeff = 0x40000000
	     i32A1Coeff = 0x00000000
	     i32A2Coeff = 0x00000000
	     i32B1Coeff = 0x00000000
	     i32B2Coeff = 0x00000000
     */
    BDSP_Raaga_Audio_BiquadCoefs	sAvlpFilterCoeffs[3];

}BDSP_Raaga_Audio_AvlpFilterConfig;


typedef struct
{
	
/* This value is used to enable/disable avlp. 0 is for Disable and 1 is for   Enable. Default is Enable
	 */

	int32_t		i32AvlOn;					
	
	/* This is used to mute avlp. Default is Enable. AFTER muting, algorithm 
      should restart (nn = 1)*/

	int32_t		i32AvlMute;						
	
/* This is used to Describes how much of the peak value of current slot
   will propagate towards next slot. Take any values from 0 to 2048,
   default value is 2007 */

	int32_t		i32PeakReductionPerSlot;
	
	/* This is used to Describes minimal peakvalue when sudden peak increase. 
	   Take any values from 0 to 4096, default value is 3277 */

	int32_t		i32PeakThreshold;     

/* This is used When sample peakvalue is above peak value /
   PEAK_THRESHOLD, peak value is multiplied with PEAK_INCREMENT. 
	   Take any values from 0 to 16384, default value is 205 */

	int32_t		i32PeakIncrement;	 
	
	/* This is used to Describes how much louder parts may sound louder at
 the end. Take any values from 0 to 1024, default value is 205 */

	int32_t		i32Corrfactorlowampl;		

	/* This is used to Describes how long the peak value needs to be
 remembered. Take any values from 0 to 64, default value is 48 */

	int32_t		i32Sp; 	   
	
	/* This is used to Describes how long the rms value needs to be
 remembered.Take any values from 0 to 64, default value is 32 */

	int32_t		i32Sr;	  
	
	/* This is used to Describes how much the gain may change per second in 
	the zone close to the wanted target. 
	   Take any values from 0 to 32, default value is 4 */

	int32_t		i32Ap2bp; 	

	/* This is used to Weight of contribution of lower rms is overall rms. 
	   Take any values from 0 to 64, default value is 21 */

	
	int32_t		i32NewRmsWeight;		
	
	/* If more then L0 dB under target rms, no change. Take any values from 0
 to 256, default value is 32 */

	
	int32_t		i32L0;							
	
	/* If more then L1 dB under target rms, 2*AP2BP change. Take any values
 from 0 to 256, default value is 54 */
	
	int32_t		i32L1;							
	
	/* If more then L2 dB under target rms, L20*AP2BP change. Take any values
 from 0 to 256, default value is 80 */
	
	int32_t		i32L2;							
	
	/* If ever more then L2 under target, take higher speed until L3 under
 target. Take any values from 0 to 256, default value is 48 */

	int32_t		i32L3;							
	
	/* If more then L4 dB under target rms, L41*AP2BP change. Take any values
 from 0 to 256, default value is 99 */

	int32_t		i32L4;								
	
	/* See L2 explanation.Take any values from 0 to 16,default value is 6 */
	
	int32_t		i32L20;							
	
	/* This is used to Weight of contribution towards fast correction range. 
	   Take any values from 0 to 16, default value is 6 */
	
	int32_t		i32L30;							
   
	/* See L4 explanation.Take any values from 0 to 16,default value is 12 */
	
	
	int32_t		i32L41;						
	
/* This is used to Peak values fast reduced to target AP=BP+SLOW_REGION_P, softly reduced to BP, so Peak + gain = BP. Take any values from 0 to 64, default value is 14 */

	int32_t		i32Bp;							
	
	/* This is used to RMS values fast reduced to target AR=BR+SLOW_REGION_R
 softly reduced to BR so RMS + gain = BR. Take any values from 0 to 64,
 default value is 40 */

	
	int32_t		i32Br;							
	
      /* Area under AR1 where gain is reduced with AP2BP dB/s. Take any values
 from 0 to 32, default value is 4 */

	int32_t		i32SlowRegionR;				
	
    /* Area under AP where gain is reduced with AP2BP dB/s. Take any values
 from 0 to 32, default value is 16 */

	
	int32_t		i32SlowRegionP;				
	
    /* Maximum allowed amplification value. Take any values from 0 to 32,
 default value is 15 */


	int32_t		i32Gmax;						
	
	/* If rms suddenly above NEW_RMS_ABOVE_PREV_RMS_THRESH*previous rms, the
 new rms-value is rms/NEW_RMS_ABOVE_PREV_RMS_THRESH. Take any values from
 0 to 64, default value is 45 */

	int32_t		i32NewRmsAbovePrevRmsThresh; 	
	
	/* Higher RMS (close to current rms) count more then lower RMS. Take any
 values from 0 to 32, default value is 12 */

	
	int32_t		i32WeightForhigherRms;			
	
	/* NOT USED. Take any values from 0 to 2448, default value is 1331 */
	
	int32_t		i32StrongRmsCorThresh;		
	
	/* If slot rms > 'peak' rms*PEAK_BEAT_SLOPE_inv and quite higher then
 previous rms, then higher weight is used. Take any values from 0 to 32,
 default value is 29 */

	int32_t		i32PeakBeatSlopeInv;

     
/* Structure for AVLP Biquad filter configuration */

	BDSP_Raaga_Audio_AvlpFilterConfig	sAvlpFilterConfig;

    /* Volume Offset which should be applied on PCM in the event AVL has been disabled
     Gain in Q27 Format. Default: 0x8000000 (1.0) */
    int32_t     i32VolumeOffset;

    /* The amount of headroom that should be created in the output.
     * Possible Value: 0-31. Default =0
     */
    int32_t     i32HeadRoomCreationParam;


} BDSP_Raaga_Audio_AvlPConfigParams;

/*
   -------------------------------------------------------------------------
   This data structure defines SRS StudioSound user configuration parameters
   -------------------------------------------------------------------------
*/

/*
    Common Substucture for General node configuration and top level information for all 4 nodes of SRS STUDIO
    ---------------------------------------------------------------------------------------------------------
*/
/*
    i32IsStudioSound, i32StudioSoundMode and i32mEnable have to be used by all 4 nodes
    i32mInputGain, i32mHeadroomGain will be used by First node(Node0), currently SRS TruVolume
    i32mInputMode will be used by First node(Node0) currently SRS TruVolume and Node 1(Circle Surround Decoder/Tru-Dialog)
    i32mOutputGain and i32mBypassGain will be used by Last node(Node 3), curretly Equalizer and Hard limiter
*/
typedef struct BDSP_Raaga_Audio_StudioSoundTopLevelUserConfig
{
    int32_t                         i32IsStudioSound;   /*To identify whether the postprocess is part of StudioSound */
                                                        /*or standalone(TVOL and/or TSHD) configuration*/
                                                        /*This field can change only at stop start*/
                                                        /*Default : 0, Range [0, 1], others invalid, will be treated as 0*/
                                                        /* 0 -> Stand Alone configuration */
                                                        /* 1 -> Part of StudioSound */

    int32_t                         i32StudioSoundMode; /* if i32IsStudioSound == 1, this field and  will signify the mode to run. Following is the description. */
                                                        /* if i32IsStudioSound == 0, This field and all other field in  BDSP_Raaga_Audio_StudioSoundTopLevelUserConfig will be ignored */

                                                        /*Default : 1, Range [1, 2], others invalid, will be treated as 1*/


/*
	In STAND ALONE MODE TVOL and TSHD : 
	-----------------------------------
		(i32IsStudioSound = 0), all other fields in  BDSP_Raaga_Audio_StudioSoundTopLevelUserConfig will be ignored by the FW node.
*/
/*	In STUDIO SURROUND MODE
		(i32IsStudioSound = 1), Based on mode (i32StudioSoundMode), a few of the modules will be disabled by the FW internally.
								In case of conflict, i32StudioSoundMode will have  Highest priority.
		
		Different Studio Sound Modes are given Below: 

			 i32StudioSoundMode = (1) -> GEq-Off. User configures all other modules
			 i32StudioSoundMode = (2) -> TVOL Off, CSDecoder Off, TruDialog Off, TSHD Off. User configures all other modules

  User cannot switch ON any module that is specified as OFF for a particular mode. For example, while running in StudioSound mode with i32StudioSoundMode = 1, GEQ cannot be switched ON by the user.
  User can chose to turn ON or OFF rest of the modules.

  Also, CSDecoder can only be switched ON when TSHD is ON, though TSHD can be switched ON with or without CS Decoder. 
 */

    int32_t                         i32mEnable;         /*SRS StudioSound processing enable*/
                                                        /*Range [0, 1], Default : 1*/
                                                        /*  1:Enable SRS StudioSound processing,
                                                            0:Bypass SRS StudioSound,
                                                            All modules will be disabled with bypassgain 1.0
                                                        */

    int32_t                         i32mInputGain;      /*SRS StudioSound Input Gain*/
                                                        /*Range: Floating point: (0.0 - 1.0), Default: 1.0*/
                                                        /*Fixed point: (0x0 - 0x7fffffff), default 0x7fffffff, QFormat - 1.31    */

    int32_t                         i32mHeadroomGain;   /*SRS StudioSound Headroom Gain*/
                                                        /*Range: Floating point: (0.0 - 1.0), Default: 1.0*/
                                                        /*Fixed point: (0x0 - 0x7fffffff), default 0x7fffffff, QFormat - 1.31*/

    int32_t                         i32mInputMode;      /*Channel configuration present in input*/
                                                        /*Range: [0, 1, 2, 3], Default: 3*/
                                                        /* 0 : 1_0_0
                                                            1 : 2_0_0
                                                            2 : 3_2_1
                                                            3 : LtRt
                                                        */

    int32_t                         i32mOutputGain;     /* SRS StudioSound Output Gain */
                                                        /* Range: Floating point: (0.0 - 1.0), Default: 1.0 */
                                                        /* Fixed point: (0x0 - 0x7fffffff), default 0x7fffffff, QFormat - 1.31*/

    int32_t                         i32mBypassGain;     /*SRS StudioSound Bypass Gain*/
                                                        /*Range: Floating point: (0.0 - 1.0), Default: 1.0*/
                                                        /*Fixed point: (0x0 - 0x7fffffff), default 0x7fffffff, QFormat - 1.31*/


}BDSP_Raaga_Audio_StudioSoundTopLevelUserConfig;


/*--------------------------*/
/*HighPass Filter UserConfig*/
/*--------------------------*/
typedef struct BDSP_Raaga_FilterSpecHpf
{
	uint32_t  ui32CutoffFrequency;		/*Range: 20 to 1000 hz, default: 180 Hz*/
	uint32_t  ui32Order;				/*Range: [0, 2, 4, 6], default : 4*/
	
}BDSP_Raaga_FilterSpecHpf;

/*Coefficients should be generated by SRS_Apollo_TruEQ_HPF_Coefficients_Generator.exe*/
typedef struct BDSP_Raaga_FilterCoefHpfTdf2
{
	int32_t				i32Scale;				/*specified q-format of the coefficients. (1+i32Scale).(31-i32Scale) format 
												Default - 1. i32Scale=1 implies a q-format of 2.30*/
	int32_t				i32FilterCoefficientB0;
	int32_t				i32FilterCoefficientB1;
	int32_t				i32FilterCoefficientB2;

	int32_t				i32FilterCoefficientA1;
	int32_t				i32FilterCoefficientA2;
	
}BDSP_Raaga_FilterCoefHpfTdf2;

typedef struct BDSP_Raaga_FilterCoefHpf
{
	uint32_t			ui32Order;/*HighPass Filter order. Range: [2, 4, 6] */

	BDSP_Raaga_FilterCoefHpfTdf2	sFilterCoefHpfTdf2[3];	/*number of active biquads filter decided by i32Order*/
	
}BDSP_Raaga_FilterCoefHpf;


typedef struct BDSP_Raaga_Audio_HighPassFilterUserConfig
{
    uint32_t     ui32mEnable;        /*Highpass filter enable, Range: [0, 1], Default - 1 */
                                     /*0 - Disable HPF Processing, 1 - Enable HPF Processing*/

    uint32_t     ui32CoefGenMode;     /*Range - [0, 1], Default -> 0
										0 - > use the coefficients provided by user, in field sFilterCoefHpf[]
										1 - > use filter setting provided by user in BDSP_Raaga_FilterSpecHpf[] to generate coefficients,
										will overwrite the coefficients in sFilterCoefHpf[][]*/

	BDSP_Raaga_FilterCoefHpf 	sFilterCoefHpf[3];			/* Array size 3, representing supported sampling rates 32, 44.1 and 48kHz, will be used if i32CoefGenMode==0*/
	BDSP_Raaga_FilterSpecHpf	sFilterSpecHpf;			/* Filter specification for generating filter coeffcients, will be used if i32CoefGenMode==1 */

	
}BDSP_Raaga_Audio_HighPassFilterUserConfig;

/*Default values of HighPass filter coefficients*/

/* i32mEnable=1*/

/*Sampling frequency - 32 kHz*/
/*---------------------------*/
/*
sFilterCoefHpf[0].i32Order = 0x00000004
sFilterCoefHpf[0].sFilterCoefHpfTdf2[0].i32Scale = 0x00000001
sFilterCoefHpf[0].sFilterCoefHpfTdf2[0].i32FilterCoefficientB0 = 0x0fc81f80
sFilterCoefHpf[0].sFilterCoefHpfTdf2[0].i32FilterCoefficientB1 = 0xe06fc100
sFilterCoefHpf[0].sFilterCoefHpfTdf2[0].i32FilterCoefficientB2 = 0x0fc81f80
sFilterCoefHpf[0].sFilterCoefHpfTdf2[0].i32FilterCoefficientA1 = 0x7e36e680
sFilterCoefHpf[0].sFilterCoefHpfTdf2[0].i32FilterCoefficientA2 = 0xc1b4eec0

sFilterCoefHpf[0].sFilterCoefHpfTdf2[1].i32Scale = 0x00000001
sFilterCoefHpf[0].sFilterCoefHpfTdf2[1].i32FilterCoefficientB0 = 0x3df563c0
sFilterCoefHpf[0].sFilterCoefHpfTdf2[1].i32FilterCoefficientB1 = 0x84153880
sFilterCoefHpf[0].sFilterCoefHpfTdf2[1].i32FilterCoefficientB2 = 0x3df563c0
sFilterCoefHpf[0].sFilterCoefHpfTdf2[1].i32FilterCoefficientA1 = 0x7be0e200
sFilterCoefHpf[0].sFilterCoefHpfTdf2[1].i32FilterCoefficientA2 = 0xc40b5300
*/
/*Sampling frequency - 44.1 kHz*/
/*-----------------------------*/
/*
sFilterCoefHpf[1].i32Order = 0x00000004
sFilterCoefHpf[1].sFilterCoefHpfTdf2[0].i32Scale = 0x00000001
sFilterCoefHpf[1].sFilterCoefHpfTdf2[0].i32FilterCoefficientB0 = 0x0fd78db0
sFilterCoefHpf[1].sFilterCoefHpfTdf2[0].i32FilterCoefficientB1 = 0xe050e4a0
sFilterCoefHpf[1].sFilterCoefHpfTdf2[0].i32FilterCoefficientB2 = 0x0fd78db0
sFilterCoefHpf[1].sFilterCoefHpfTdf2[0].i32FilterCoefficientA1 = 0x7eb71980
sFilterCoefHpf[1].sFilterCoefHpfTdf2[0].i32FilterCoefficientA2 = 0xc13e3e40

sFilterCoefHpf[1].sFilterCoefHpfTdf2[1].i32Scale = 0x00000001
sFilterCoefHpf[1].sFilterCoefHpfTdf2[1].i32FilterCoefficientB0 = 0x3e826c40
sFilterCoefHpf[1].sFilterCoefHpfTdf2[1].i32FilterCoefficientB1 = 0x82fb2780
sFilterCoefHpf[1].sFilterCoefHpfTdf2[1].i32FilterCoefficientB2 = 0x3e826c40
sFilterCoefHpf[1].sFilterCoefHpfTdf2[1].i32FilterCoefficientA1 = 0x7cff9680
sFilterCoefHpf[1].sFilterCoefHpfTdf2[1].i32FilterCoefficientA2 = 0xc2f5e600
*/

/*Sampling frequency - 48 kHz*/
/*---------------------------*/
/*
sFilterCoefHpf[2].i32Order = 0x00000004
sFilterCoefHpf[2].sFilterCoefHpfTdf2[0].i32Scale = 0x00000001
sFilterCoefHpf[2].sFilterCoefHpfTdf2[0].i32FilterCoefficientB0 = 0x0fdadc10
sFilterCoefHpf[2].sFilterCoefHpfTdf2[0].i32FilterCoefficientB1 = 0xe04a47e0
sFilterCoefHpf[2].sFilterCoefHpfTdf2[0].i32FilterCoefficientB2 = 0x0fdadc10
sFilterCoefHpf[2].sFilterCoefHpfTdf2[0].i32FilterCoefficientA1 = 0x7ed26000
sFilterCoefHpf[2].sFilterCoefHpfTdf2[0].i32FilterCoefficientA2 = 0xc1249f40

sFilterCoefHpf[2].sFilterCoefHpfTdf2[1].i32Scale = 0x00000001
sFilterCoefHpf[2].sFilterCoefHpfTdf2[1].i32FilterCoefficientB0 = 0x3ea0f4c0
sFilterCoefHpf[2].sFilterCoefHpfTdf2[1].i32FilterCoefficientB1 = 0x82be1680
sFilterCoefHpf[2].sFilterCoefHpfTdf2[1].i32FilterCoefficientB2 = 0x3ea0f4c0
sFilterCoefHpf[2].sFilterCoefHpfTdf2[1].i32FilterCoefficientA1 = 0x7d3d7780
sFilterCoefHpf[2].sFilterCoefHpfTdf2[1].i32FilterCoefficientA2 = 0xc2b9a440
*/

/*3rd biquad need not be initialized as filter order is 4*/

/*Default HighPass filter specification - 
	i32CutoffFrequency=180 Hz;
	i32Order=4*/


/*
    This data structure defines TruVolume configuration parameters.
    ---------------------------------------------------------------

    TVOL can be a standalone Post processing module or it can be one of the nodes in SRS studio
    The selection of this mode is determined by the i32IsStudioSound field.
*/

typedef struct BDSP_Raaga_Audio_TruVolumeUserConfig
{
    int32_t                     i32TruVolume_enable;        /* Top level disable, no processing will happen at all when 0*/
                                                            /* Deafault : 1, range [0,1]*/
                                                            /* 1 -> processing enable, 0-> total disable of the processing*/

    int32_t                     i32nchans;                  /* Number of input/output channels     */
                                                            /* Default val 2*/

    int32_t                     i32blockSize;               /* Default: 256, range [256, 512, 768, 1024]*/

    int32_t                     i32mEnable;                 /* Default: 1;  1 -> Process, 0 ->  Bypass */

    /* This value needs to be programmed in  Q21 format */
    int32_t                     i32mInputGain;              /* Default: 0x00200000 range[0, 0x03ffffff]   Floating val: default 1: range [0, 32] */

    /* This value needs to be programmed in  Q21 format */
    int32_t                     i32mOutputGain;             /* Default: 0x00200000  range[0, 0x007fffff]   Floating val: default 1: range [0, 4] */

    int32_t                     i32mBypassGain;             /* Bypass gain */
                                                            /* default: 0x007fffff range[0, 0x007fffff]
                                                               Floating val: default 1: range [0, 1] */

    int32_t                     i32mReferenceLevel;         /* Reference Level */
                                                            /* Default: 0x00400000 range[0x00000109, 0x007fffff]
                                                               Floating val: default 0.5: range [3.1623 * 10^-5, 1] */

    int32_t                     i32EnableDCNotchFilter;     /* Enable DC Notch filter*/
                                                            /* Default: 0 range[0 -> disble, 1 -> enable]*/

    int32_t                     i32mMode;                   /* Default: 1 range[0,1] 1-> kSrsLight, 0-> kSrsNormal*/
                
    int32_t                     i32mSize;                   /* Default: 0 range[0,1,2,3,4, 5]*/
                                                            /*
                                                                0   ->  VIQ_kSrs20Hz,
                                                                1   ->  kSrs40Hz,
                                                                2   ->  kSrs110Hz,
                                                                3   ->  kSrs200Hz,
                                                                4   ->  kSrs315Hz,
                                                                5   ->  kSrs410Hz*/

    int32_t                     i32mMaxGain;                 /* Max Gain Control */
                                                             /* Default: 0x00080000 rnage[0x00002000, 0x007fffff]*/
                                                             /* Float default: 16.0 rnage[.25, 256.0], conversion formula: (f_val/256)*2^23 */


    int32_t                     i32mNoiseManager;           /* Default: 1; 1 -> Noise manager enable, 0 -> disable */
    int32_t                     i32mNoiseManagerThreshold;  /* Default: 0x000ccccd, range [0x0000a3d7, 0x007fffff] */
                                                            /* Float Default: 0.1 range[0.005, 1.0], conversion formula: f_val*2^23 */

    int32_t                     i32mCalibrate;              /* Default: 0x8000, range [0x00000148, 0x007fffff] */
                                                            /* Float Default: 1.0 range[0.01, 256.0]*/

    int32_t                     i32mNormalizerEnable;       /* Default: 1;  1 -> Normalizer Enable, 0   ->  Normalizer Disable */


    BDSP_Raaga_Audio_StudioSoundTopLevelUserConfig   sTopLevelConfig;
    BDSP_Raaga_Audio_HighPassFilterUserConfig        sHighPassFilterConfig;

}BDSP_Raaga_Audio_TruVolumeUserConfig;


/*
    Node1 of SRS STUDIO: This is a combination of SRS-Tru-Dialog and SRS Circle Surround.
    -------------------------------------------------------------------------------------

    The user configuration of Node1 is BDSP_Raaga_Audio_SRS_CSDecTruDialogConfigParams

    User CFG structure for Circle Surround Decoder/Tru-Dialog are separately defined as below
*/

/*TruDialog user config*/
typedef struct BDSP_Raaga_Audio_TruDialogUserConfig
{
    int32_t         i32mEnable;                     /*Enable TruDialog, Range [0, 1], Default : 1*/
                                                    /*1:Enable TruDialog, 0:Bypass TruDialog*/


    int32_t         i32mTruDialogProcessGain;       /*Amount of final calculated gain applied to final  output signal*/
                                                    /*Range: Floating point: (0.5 - 3.0), Default: 1.5*/
                                                    /*Fixed point: (0x04000000 - 0x18000000), default: 0x0c000000,  QFormat - 5.27*/


    int32_t         i32mTruDialogInputGain;         /*TruDialog Input Gain*/
                                                    /*Range: Floating point: (0.0 - 1.0), Default: 1.0*/
                                                    /*Fixed point: (0x0 - 0x7fffffff), default: 0x7fffffff, QFormat - 1.31*/

    int32_t         i32mTruDialogOutputGain;        /*TruDialog Output Gain*/
                                                    /*Range: Floating point: (0.0 - 1.0), Default: 1.0*/
                                                    /*Fixed point: (0x0 - 0x7fffffff), default: 0x7fffffff, QFormat - 1.31*/

    int32_t         i32mTruDialogDialogClarityGain; /*Amount of Dialog Clarity enhancement applied to input signal*/
                                                    /*Range: Floating point: (0.0 - 1.0), Default: 1.0*/
                                                    /*Fixed point: (0x0 - 0x7fffffff), default: 0x7fffffff, QFormat - 1.31*/

    int32_t         i32mTruDialogBypassGain;        /*TruDialog Bypass Gain*/
                                                    /*Range: Floating point: (0.0 - 1.0), Default: 1.0*/
                                                    /*Fixed point: (0x0 - 0x7fffffff), default: 0x7fffffff, QFormat - 1.31*/
}BDSP_Raaga_Audio_TruDialogUserConfig;

/*CSDecoder user config*/
typedef struct BDSP_Raaga_Audio_CSDecoderUserConfig
{

    int32_t         i32mEnable;                     /*Enable CS Decoding*/
                                                    /*Range [0, 1], Default : 1*/
                                                    /*1:Enable CS Decoding, 0:Bypass CS Decoding*/

    int32_t         i32mInputGain;                  /*CS Decoder Input Gain */
                                                    /*Range: Floating point: (0.12589 - 1.0), Default: 1.0*/
                                                    /*In dB       : (-18dB - 0dB) , Default: 0 dB*/
                                                    /*Fixed point : (0x1020c49c - 0x7fffffff), default: 0x7fffffff, QFormat - 1.31*/

    int32_t         i32mMode;                       /*CS Decoder processing mode*/
                                                    /*Range [1, 4], Default : 1, [1:Cinema 4:Music]*/

    int32_t         i32mOutputMode;                 /*Output mode Range: [0, 1]. Only supported mode is 0 Default : 0*/
                                                    /*0 - Stereo output mode, 1 - Multi channel output mode*/

    int32_t         i32mCSDecOutputGainLR;          /*CS Decoder output gain for LR channels*/
                                                    /*Range: Floating point (0.089 to 4.0), Default: 1.0*/
                                                    /*Range: Fixed point (0x016c8b44 to 0x40000000), default: 0x10000000, QFormat: 4.28*/
                                                    /*Range: in decibels (-21dB to 12dB), default: 0dB*/

    int32_t         i32mCSDecOutputGainLsRs;        /*CS Decoder output gain for LsRs channels*/
                                                    /*Range: Floating point (0.089 to 4.0), Default: 1.0*/
                                                    /*Range: Fixed point (0x016c8b44 to 0x40000000), default: 0x10000000, QFormat: 4.28*/
                                                    /*Range: in decibels (-21dB to 12dB), default: 0dB*/

    int32_t         i32mCSDecOutputGainC;           /*CS Decoder output gain for Center channel*/
                                                    /*Range: Floating point (0.089 to 4.0), Default: 1.0*/
                                                    /*Range: Fixed point (0x016c8b44 to 0x40000000), default: 0x10000000, QFormat: 4.28*/
                                                    /*Range: in decibels (-21dB to 12dB), default: 0dB*/

    int32_t         i32mCSDecOutputGainSub;         /*CS Decoder output gain for Subwoofer channel*/
                                                    /*Range: Floating point (0.089 to 4.0), Default: 1.0*/
                                                    /*Range: Fixed point (0x016c8b44 to 0x40000000), default: 0x10000000, QFormat: 4.28*/
                                                    /*Range: in decibels (-21dB to 12dB), default: 0dB*/
}BDSP_Raaga_Audio_CSDecoderUserConfig;

/* Top level User Cfg structure for Node1 of SRS studio*/

typedef struct BDSP_Raaga_Audio_SRS_CSDecTruDialogConfigParams
{
    BDSP_Raaga_Audio_StudioSoundTopLevelUserConfig   sTopLevelConfig;
    BDSP_Raaga_Audio_TruDialogUserConfig             sTruDialogConfig;
    BDSP_Raaga_Audio_CSDecoderUserConfig             sCSDecoderConfig;

}BDSP_Raaga_Audio_SRS_CSDecTruDialogConfigParams;


/*
    Node2 of SRS STUDIO: SRS TS-HD.
    -------------------------------------------------------------------------------------

    The user configuration of Node2 is BDSP_Raaga_Audio_TruSurrndHDConfigParams

    SRS TS-HD can be a standalone Post processing module or it can be one of the nodes in SRS studio
    The selection of this mode is determined by the i32IsStudioSound field.
*/

typedef enum BDSP_Raaga_Audio_SRSTruBassProcessMode
{
    BDSP_Raaga_Audio_SRSTruBassProcessMode_eMono,
    BDSP_Raaga_Audio_SRSTruBassProcessMode_eStereo,
    BDSP_Raaga_Audio_SRSTruBassProcessMode_eLast,
    BDSP_Raaga_Audio_SRSTruBassProcessMode_eInvalid = 0x7fffffff

} BDSP_Raaga_Audio_SRSTruBassProcessMode;

typedef enum BDSP_Raaga_Audio_SRS3DMode
{
    BDSP_Raaga_Audio_SRS3DMode_eMono,
    BDSP_Raaga_Audio_SRS3DMode_eSingleSpeaker,
    BDSP_Raaga_Audio_SRS3DMode_eStereo,
    BDSP_Raaga_Audio_SRS3DMode_eExtreme,
    BDSP_Raaga_Audio_SRS3DMode_eLast,
    BDSP_Raaga_Audio_SRS3DMode_eInvalid = 0x7fffffff

} BDSP_Raaga_Audio_SRS3DMode;

typedef enum
{
    BDSP_Raaga_Audio_SRSTSHDCrossoverFrequency_e80Hz = 0,
    BDSP_Raaga_Audio_SRSTSHDCrossoverFrequency_e120Hz,
    BDSP_Raaga_Audio_SRSTSHDCrossoverFrequency_e160Hz,
    BDSP_Raaga_Audio_SRSTSHDCrossoverFrequency_e200Hz,
    BDSP_Raaga_Audio_SRSTSHDCrossoverFrequency_eMax,
    BDSP_Raaga_Audio_SRSTSHDCrossoverFrequency_eInvalid =0x7fffffff

} BDSP_Raaga_Audio_SRSTSHDCrossoverFrequency;

typedef struct BDSP_Raaga_Audio_TruSurrndHDConfigParams
{
    int32_t     i32TruSurrndHDEnableFlag;   /*Default 1 Enable*/
    int32_t     i32HeadPhoneEnableFlag;     /*Default 0 disable*/
    int32_t     i32TruSurrndHDInputGain;    /*Range 0 to 1.0 in Q31 format
                                            Default 0x40000000*/
    int32_t     i32TruSurrndHDOutputGain;   /*Range 0 to 1.0 in Q31 format
                                            Default 0x7fffffff*/
    int32_t     i32TruSurrndHDByPassGain;   /*Range 0 to 1.0 in Q31 format
                                            Default 0x7FFFFFFF*/
    BDSP_Raaga_Audio_SRSTSHDCrossoverFrequency eSubCrossOverFreq; /*Used only for non Apollo mode
                                            Default BDSP_Raaga_Audio_SRSTSHDCrossoverFrequency_e200Hz*/

    int32_t     i32TruBassFrontEnableFlag;  /*Default 1 Enable*/
    int32_t     i32TruBassFrontCtrl;        /*Range 0 to 1.0 in Q31 format
                                            Default 0x26666666*/

    BDSP_Raaga_Audio_SpeakerSize eTruBassFrontSpeakerSize; /*Default BDSP_Raaga_Audio_SpeakerSize_eLFResponse150Hz
                                            with a value of 3*/
    BDSP_Raaga_Audio_SRSTruBassProcessMode   eSRSTruBassProcessMode; /*Default to stereo mode*/

    int32_t     i32DefinitionFrontEnableFlag;   /*Default 1 Enable*/
    int32_t     i32DefinitionFrontCtrl;     /*Range 0 to 1.0 in Q31 format
                                            Default 0x26666666*/


    int32_t     i32DialogClarityEnableFlag; /*Default 1 Enable*/
    int32_t     i32DialogClarityCtrl;       /*Range 0 to 1.0 in Q31 format
                                            Default 0x40000000*/

    int32_t     i32SurroundLevel;           /*Range 0 to 1.0 in Q31 format
                                            Default 4CCCCCCD*/

    int32_t     i32WowHDSRS3DEnableFlag;    /*Default 1 Enable*/
    int32_t     i32SRS3DHighBitRateFlag;    /*Default 1 and sets to High Bit Rate*/
    BDSP_Raaga_Audio_SRS3DMode   eWowHDSRS3DMode;/*Default BDSP_Raaga_Audio_SRS3DMode_eStereo with value 2*/
    int32_t     i32WowHDSRS3DSpaceCtrl;     /*Range 0 to 1.0 in Q31 format
                                            Default 0x66666666*/
    int32_t     i32WowHDSRS3DCenterCtrl;    /*Range 0 to 1.0
                                            Default 0x40000000*/
    int32_t     i32WowHDFocusEnableFlag;    /*Default 1 Enable*/
    int32_t     i32WowHDFocusCtrl;          /*Range 0 to 1.0 in Q31 format
                                            Default 0x33333333*/

    int32_t     i32Mono2StereoEnableFlag;


    BDSP_Raaga_Audio_AcMode  eOutputAcMode;      /*Default SRSTruSurroundHDOutputMode_e2_0_0*/
    int32_t     i32OuputLFEEnableFlag;      /*Used only for TSHD4.  Not used for Studio Sound*/

    int32_t     i32CertificationEnableFlag; /*Default 0 Disable*/
    BDSP_Raaga_Audio_AcMode  eInputAcMode;       /*Used only during certification*/
    int32_t     i32InputLFEPresentFlag;     /*Used only during certification*/

    BDSP_Raaga_Audio_StudioSoundTopLevelUserConfig           sTopLevelConfig;

}BDSP_Raaga_Audio_TruSurrndHDConfigParams;



/*
    Node3 of SRS STUDIO: This is a combination of ParamtericEQ, Graphic equalizer and Hard Limiter.
    -----------------------------------------------------------------------------------------------

    The user configuration of Node3 is BDSP_Raaga_Audio_SRS_EqualizerHardLimiterConfigParams

    User CFG structure for ParamtericEQ, Graphic equalizer and Hard Limiter are separately defined as below
*/

/*------------------------*/
/*ParamtericEQ user config*/
/*------------------------*/
typedef struct  BDSP_Raaga_FilterSpecPeq
{
	int32_t i32BandGain;		/*Range in dB: -12 to 12 db
										Format - q24
										Range in Fixed point: 0x00404DE6 to 0x03FB2783
										Default: 0dB
										Default in fixed point: 0x01000000*/

	uint32_t ui32BandFrequency;	/*Range: 20 to 20000 Hz
								  Default: 1000 Hz*/

	int32_t i32QFactor;			/*Range in floating point: 0.25 to 16.0 
								  Format: q24
								  Range, fixed point: 0x00400000 to 0x10000000
								  Default in floating point: 2.0
								  Default in fixed point: 0x02000000

								*/
}BDSP_Raaga_FilterSpecPeq;

/*Coefficients should be generated by SRS_Apollo_TruEQ_HPF_Coefficients_Generator.exe*/
typedef struct
{
	int32_t				i32Order;	/*ParametricEQ filter order*/
	int32_t				i32Scale;	/*specified q-format of the coefficients. (1+i32Scale).(31-i32Scale) format 
										Default - 1. i32Scale=1 implies a q-format of 2.30*/
	int32_t				i32FilterCoefficientB0;
	int32_t				i32FilterCoefficientA1;
	int32_t				i32FilterCoefficientB1;
	int32_t				i32FilterCoefficientA2;
	int32_t				i32FilterCoefficientB2;
	
}BDSP_Raaga_FilterCoefPeq;


typedef struct BDSP_Raaga_Audio_ParametricEqUserConfig
{
    int32_t i32mEnable[2];                          /*Left/Right channel ParametricEQ Processing Enable*/
                                                    /*Range: [0,1], Default: i32mEnable[0] = 1, i32mEnable[1] = 1*/
                                                    /*0 - Disable PEQ processing, 1 - Enable PEQ processing*/

    int32_t i32mBandEnable[2][8];                   /*Turn ON/OFF a specific band on one or both channels.*/
                                                    /*Range: [0,1], Default: 1 for all bands*/
                                                    /*0 - Disable band, 1 - Enable band*/

    int32_t i32mInputGain;                          /*Input gain*/
                                                    /*Range: Floating point: [0.0 to 10.0], Default: 1.0*/
                                                    /*Fixed point   : [0x0 to 0x50000000], Default: 0x08000000, QFormat - 5.27*/

    int32_t i32mOutputGain;                         /*Output gain*/
                                                    /*Range: Floating point: [0.0 to 10.0], Default: 1.0*/
                                                    /*Fixed point   : [0x0 to 0x50000000], Default: 0x08000000, QFormat - 5.27*/

    int32_t i32mBypassGain;                         /*Bypass gain*/
                                                    /*Range: Floating point: [0.0 to 10.0], Default: 1.0*/
                                                    /*Fixed point   : [0x0 to 0x50000000], Default: 0x08000000, QFormat - 5.27*/

    int32_t     i32CoefGenMode;                     /*Range - [0, 1], Default -> 0
								                                                      0 - > use the coefficients provided by user, in field sFilterCoefPeq[]
								                                                      1 - > use filter setting provided by user in sFilterSpecPeq[] to generate coefficients,
													    will overwrite the coefficients in sFilterCoefPeq[][]*/


	BDSP_Raaga_FilterCoefPeq	sFilterCoefPeq[3][8];	/*array size 3x8 implying 3 sampling frequencies with 8 bands each, will be used if i32CoefGenMode==0*/
	BDSP_Raaga_FilterSpecPeq	sFilterSpecPeq[8];		/* Filter specification for generating filter coeffcients, will be used if i32CoefGenMode==1*/			

}BDSP_Raaga_Audio_ParametricEqUserConfig;

/*Default filter specification - Gain: 0dB, Frequency: 1000Hz, Q: 2 for all bands*/

/*Default ParametricEQ Filter coefficients*/

/*Sampling Rate - 32 kHz*/
/*
//band 0
sFilterCoefPeq[0][0].i32Order = 0x00000002
sFilterCoefPeq[0][0].i32Scale = 0x00000001
sFilterCoefPeq[0][0].i32FilterCoefficientB0 = 0x40000000
sFilterCoefPeq[0][0].i32FilterCoefficientA1 = 0x798f9f00
sFilterCoefPeq[0][0].i32FilterCoefficientB1 = 0x86706100
sFilterCoefPeq[0][0].i32FilterCoefficientA2 = 0xc40eb580
sFilterCoefPeq[0][0].i32FilterCoefficientB2 = 0x3bf14a80


//band 1
sFilterCoefPeq[0][1].i32Order = 0x00000002
sFilterCoefPeq[0][1].i32Scale = 0x00000001
sFilterCoefPeq[0][1].i32FilterCoefficientB0 = 0x40000000
sFilterCoefPeq[0][1].i32FilterCoefficientA1 = 0x798f9f00
sFilterCoefPeq[0][1].i32FilterCoefficientB1 = 0x86706100
sFilterCoefPeq[0][1].i32FilterCoefficientA2 = 0xc40eb580
sFilterCoefPeq[0][1].i32FilterCoefficientB2 = 0x3bf14a80


//band 2
sFilterCoefPeq[0][2].i32Order = 0x00000002
sFilterCoefPeq[0][2].i32Scale = 0x00000001
sFilterCoefPeq[0][2].i32FilterCoefficientB0 = 0x40000000
sFilterCoefPeq[0][2].i32FilterCoefficientA1 = 0x798f9f00
sFilterCoefPeq[0][2].i32FilterCoefficientB1 = 0x86706100
sFilterCoefPeq[0][2].i32FilterCoefficientA2 = 0xc40eb580
sFilterCoefPeq[0][2].i32FilterCoefficientB2 = 0x3bf14a80


//band 3
sFilterCoefPeq[0][3].i32Order = 0x00000002
sFilterCoefPeq[0][3].i32Scale = 0x00000001
sFilterCoefPeq[0][3].i32FilterCoefficientB0 = 0x40000000
sFilterCoefPeq[0][3].i32FilterCoefficientA1 = 0x798f9f00
sFilterCoefPeq[0][3].i32FilterCoefficientB1 = 0x86706100
sFilterCoefPeq[0][3].i32FilterCoefficientA2 = 0xc40eb580
sFilterCoefPeq[0][3].i32FilterCoefficientB2 = 0x3bf14a80


//band 4
sFilterCoefPeq[0][4].i32Order = 0x00000002
sFilterCoefPeq[0][4].i32Scale = 0x00000001
sFilterCoefPeq[0][4].i32FilterCoefficientB0 = 0x40000000
sFilterCoefPeq[0][4].i32FilterCoefficientA1 = 0x798f9f00
sFilterCoefPeq[0][4].i32FilterCoefficientB1 = 0x86706100
sFilterCoefPeq[0][4].i32FilterCoefficientA2 = 0xc40eb580
sFilterCoefPeq[0][4].i32FilterCoefficientB2 = 0x3bf14a80

//band 5
sFilterCoefPeq[0][5].i32Order = 0x00000002
sFilterCoefPeq[0][5].i32Scale = 0x00000001
sFilterCoefPeq[0][5].i32FilterCoefficientB0 = 0x40000000
sFilterCoefPeq[0][5].i32FilterCoefficientA1 = 0x798f9f00
sFilterCoefPeq[0][5].i32FilterCoefficientB1 = 0x86706100
sFilterCoefPeq[0][5].i32FilterCoefficientA2 = 0xc40eb580
sFilterCoefPeq[0][5].i32FilterCoefficientB2 = 0x3bf14a80

//band 6
sFilterCoefPeq[0][6].i32Order = 0x00000002
sFilterCoefPeq[0][6].i32Scale = 0x00000001
sFilterCoefPeq[0][6].i32FilterCoefficientB0 = 0x40000000
sFilterCoefPeq[0][6].i32FilterCoefficientA1 = 0x798f9f00
sFilterCoefPeq[0][6].i32FilterCoefficientB1 = 0x86706100
sFilterCoefPeq[0][6].i32FilterCoefficientA2 = 0xc40eb580
sFilterCoefPeq[0][6].i32FilterCoefficientB2 = 0x3bf14a80

//band 7
sFilterCoefPeq[0][7].i32Order = 0x00000002
sFilterCoefPeq[0][7].i32Scale = 0x00000001
sFilterCoefPeq[0][7].i32FilterCoefficientB0 = 0x40000000
sFilterCoefPeq[0][7].i32FilterCoefficientA1 = 0x798f9f00
sFilterCoefPeq[0][7].i32FilterCoefficientB1 = 0x86706100
sFilterCoefPeq[0][7].i32FilterCoefficientA2 = 0xc40eb580
sFilterCoefPeq[0][7].i32FilterCoefficientB2 = 0x3bf14a80
*/

/*Sampling Rate - 44.1 kHz*/
/*
//band 0
sFilterCoefPeq[1][0].i32Order = 0x00000002
sFilterCoefPeq[1][0].i32Scale = 0x00000001
sFilterCoefPeq[1][0].i32FilterCoefficientB0 = 0x40000000
sFilterCoefPeq[1][0].i32FilterCoefficientA1 = 0x7bc37b00
sFilterCoefPeq[1][0].i32FilterCoefficientB1 = 0x843c8500
sFilterCoefPeq[1][0].i32FilterCoefficientA2 = 0xc2f83300
sFilterCoefPeq[1][0].i32FilterCoefficientB2 = 0x3d07cd00

//band 1
sFilterCoefPeq[1][1].i32Order = 0x00000002
sFilterCoefPeq[1][1].i32Scale = 0x00000001
sFilterCoefPeq[1][1].i32FilterCoefficientB0 = 0x40000000
sFilterCoefPeq[1][1].i32FilterCoefficientA1 = 0x7bc37b00
sFilterCoefPeq[1][1].i32FilterCoefficientB1 = 0x843c8500
sFilterCoefPeq[1][1].i32FilterCoefficientA2 = 0xc2f83300
sFilterCoefPeq[1][1].i32FilterCoefficientB2 = 0x3d07cd00

//band 2
sFilterCoefPeq[1][2].i32Order = 0x00000002
sFilterCoefPeq[1][2].i32Scale = 0x00000001
sFilterCoefPeq[1][2].i32FilterCoefficientB0 = 0x40000000
sFilterCoefPeq[1][2].i32FilterCoefficientA1 = 0x7bc37b00
sFilterCoefPeq[1][2].i32FilterCoefficientB1 = 0x843c8500
sFilterCoefPeq[1][2].i32FilterCoefficientA2 = 0xc2f83300
sFilterCoefPeq[1][2].i32FilterCoefficientB2 = 0x3d07cd00

//band 3
sFilterCoefPeq[1][3].i32Order = 0x00000002
sFilterCoefPeq[1][3].i32Scale = 0x00000001
sFilterCoefPeq[1][3].i32FilterCoefficientB0 = 0x40000000
sFilterCoefPeq[1][3].i32FilterCoefficientA1 = 0x7bc37b00
sFilterCoefPeq[1][3].i32FilterCoefficientB1 = 0x843c8500
sFilterCoefPeq[1][3].i32FilterCoefficientA2 = 0xc2f83300
sFilterCoefPeq[1][3].i32FilterCoefficientB2 = 0x3d07cd00

//band 4
sFilterCoefPeq[1][4].i32Order = 0x00000002
sFilterCoefPeq[1][4].i32Scale = 0x00000001
sFilterCoefPeq[1][4].i32FilterCoefficientB0 = 0x40000000
sFilterCoefPeq[1][4].i32FilterCoefficientA1 = 0x7bc37b00
sFilterCoefPeq[1][4].i32FilterCoefficientB1 = 0x843c8500
sFilterCoefPeq[1][4].i32FilterCoefficientA2 = 0xc2f83300
sFilterCoefPeq[1][4].i32FilterCoefficientB2 = 0x3d07cd00

//band 5
sFilterCoefPeq[1][5].i32Order = 0x00000002
sFilterCoefPeq[1][5].i32Scale = 0x00000001
sFilterCoefPeq[1][5].i32FilterCoefficientB0 = 0x40000000
sFilterCoefPeq[1][5].i32FilterCoefficientA1 = 0x7bc37b00
sFilterCoefPeq[1][5].i32FilterCoefficientB1 = 0x843c8500
sFilterCoefPeq[1][5].i32FilterCoefficientA2 = 0xc2f83300
sFilterCoefPeq[1][5].i32FilterCoefficientB2 = 0x3d07cd00

//band 6
sFilterCoefPeq[1][6].i32Order = 0x00000002
sFilterCoefPeq[1][6].i32Scale = 0x00000001
sFilterCoefPeq[1][6].i32FilterCoefficientB0 = 0x40000000
sFilterCoefPeq[1][6].i32FilterCoefficientA1 = 0x7bc37b00
sFilterCoefPeq[1][6].i32FilterCoefficientB1 = 0x843c8500
sFilterCoefPeq[1][6].i32FilterCoefficientA2 = 0xc2f83300
sFilterCoefPeq[1][6].i32FilterCoefficientB2 = 0x3d07cd00

//band 7
sFilterCoefPeq[1][7].i32Order = 0x00000002
sFilterCoefPeq[1][7].i32Scale = 0x00000001
sFilterCoefPeq[1][7].i32FilterCoefficientB0 = 0x40000000
sFilterCoefPeq[1][7].i32FilterCoefficientA1 = 0x7bc37b00
sFilterCoefPeq[1][7].i32FilterCoefficientB1 = 0x843c8500
sFilterCoefPeq[1][7].i32FilterCoefficientA2 = 0xc2f83300
sFilterCoefPeq[1][7].i32FilterCoefficientB2 = 0x3d07cd00
*/

/*Sampling Rate - 48 kHz*/
/*
//band 0
sFilterCoefPeq[2][0].i32Order = 0x00000002
sFilterCoefPeq[2][0].i32Scale = 0x00000001
sFilterCoefPeq[2][0].i32FilterCoefficientB0 = 0x40000000
sFilterCoefPeq[2][0].i32FilterCoefficientA1 = 0x7c31eb80
sFilterCoefPeq[2][0].i32FilterCoefficientB1 = 0x83ce1480
sFilterCoefPeq[2][0].i32FilterCoefficientA2 = 0xc2bbbb80
sFilterCoefPeq[2][0].i32FilterCoefficientB2 = 0x3d444480

//band 1
sFilterCoefPeq[2][1].i32Order = 0x00000002
sFilterCoefPeq[2][1].i32Scale = 0x00000001
sFilterCoefPeq[2][1].i32FilterCoefficientB0 = 0x40000000
sFilterCoefPeq[2][1].i32FilterCoefficientA1 = 0x7c31eb80
sFilterCoefPeq[2][1].i32FilterCoefficientB1 = 0x83ce1480
sFilterCoefPeq[2][1].i32FilterCoefficientA2 = 0xc2bbbb80
sFilterCoefPeq[2][1].i32FilterCoefficientB2 = 0x3d444480

//band 2
sFilterCoefPeq[2][2].i32Order = 0x00000002
sFilterCoefPeq[2][2].i32Scale = 0x00000001
sFilterCoefPeq[2][2].i32FilterCoefficientB0 = 0x40000000
sFilterCoefPeq[2][2].i32FilterCoefficientA1 = 0x7c31eb80
sFilterCoefPeq[2][2].i32FilterCoefficientB1 = 0x83ce1480
sFilterCoefPeq[2][2].i32FilterCoefficientA2 = 0xc2bbbb80
sFilterCoefPeq[2][2].i32FilterCoefficientB2 = 0x3d444480

//band 3
sFilterCoefPeq[2][3].i32Order = 0x00000002
sFilterCoefPeq[2][3].i32Scale = 0x00000001
sFilterCoefPeq[2][3].i32FilterCoefficientB0 = 0x40000000
sFilterCoefPeq[2][3].i32FilterCoefficientA1 = 0x7c31eb80
sFilterCoefPeq[2][3].i32FilterCoefficientB1 = 0x83ce1480
sFilterCoefPeq[2][3].i32FilterCoefficientA2 = 0xc2bbbb80
sFilterCoefPeq[2][3].i32FilterCoefficientB2 = 0x3d444480

//band 4
sFilterCoefPeq[2][4].i32Order = 0x00000002
sFilterCoefPeq[2][4].i32Scale = 0x00000001
sFilterCoefPeq[2][4].i32FilterCoefficientB0 = 0x40000000
sFilterCoefPeq[2][4].i32FilterCoefficientA1 = 0x7c31eb80
sFilterCoefPeq[2][4].i32FilterCoefficientB1 = 0x83ce1480
sFilterCoefPeq[2][4].i32FilterCoefficientA2 = 0xc2bbbb80
sFilterCoefPeq[2][4].i32FilterCoefficientB2 = 0x3d444480

//band 5
sFilterCoefPeq[2][5].i32Order = 0x00000002
sFilterCoefPeq[2][5].i32Scale = 0x00000001
sFilterCoefPeq[2][5].i32FilterCoefficientB0 = 0x40000000
sFilterCoefPeq[2][5].i32FilterCoefficientA1 = 0x7c31eb80
sFilterCoefPeq[2][5].i32FilterCoefficientB1 = 0x83ce1480
sFilterCoefPeq[2][5].i32FilterCoefficientA2 = 0xc2bbbb80
sFilterCoefPeq[2][5].i32FilterCoefficientB2 = 0x3d444480

//band 6
sFilterCoefPeq[2][6].i32Order = 0x00000002
sFilterCoefPeq[2][6].i32Scale = 0x00000001
sFilterCoefPeq[2][6].i32FilterCoefficientB0 = 0x40000000
sFilterCoefPeq[2][6].i32FilterCoefficientA1 = 0x7c31eb80
sFilterCoefPeq[2][6].i32FilterCoefficientB1 = 0x83ce1480
sFilterCoefPeq[2][6].i32FilterCoefficientA2 = 0xc2bbbb80
sFilterCoefPeq[2][6].i32FilterCoefficientB2 = 0x3d444480

//band 7
sFilterCoefPeq[2][7].i32Order = 0x00000002
sFilterCoefPeq[2][7].i32Scale = 0x00000001
sFilterCoefPeq[2][7].i32FilterCoefficientB0 = 0x40000000
sFilterCoefPeq[2][7].i32FilterCoefficientA1 = 0x7c31eb80
sFilterCoefPeq[2][7].i32FilterCoefficientB1 = 0x83ce1480
sFilterCoefPeq[2][7].i32FilterCoefficientA2 = 0xc2bbbb80
sFilterCoefPeq[2][7].i32FilterCoefficientB2 = 0x3d444480
*/



/*GraphicEQ user config*/
typedef struct BDSP_Raaga_Audio_GraphicEqUserConfig
{

    int32_t i32mEnable[2];                          /*Left/Right channel GraphicEQ Processing Enable */
                                                    /*Range: [0,1], Default: i32mEnable[0] = 1, i32mEnable[1] = 1*/
                                                    /*0 - Disable GEQ processing, 1 - Enable GEQ processing*/

    int32_t i32mBandGain[2][10];                    /*Band gain for a specific band for one or both channels.*/
                                                    /*MAX_GEQ_BANDS=10*/

                                                    /*Range:*/
                                                        /*For 10 band design*/
                                                        /*Floating point: (0.251 to 4.0), Default: 1.0*/
                                                        /*In dB         : [-12dB to +12dB], Default: 0dB*/
                                                        /*Fixed point   : [0x08083127 to 0x7fffffff), Default: 0x20000000, QFormat : 3.29*/

                                                        /*For 5 band design*/
                                                        /*Floating point: (0.316 to 3.16), Default: 1.0*/
                                                        /*In dB         : [-10dB to +10dB], Default: 0dB*/
                                                        /*Fixed point   : [0x0a1cac08 to 0x651eb852), Default: 0x20000000, QFormat : 3.29*/


    int32_t i32mInputGain;                          /*Input gain applied to both channels prior to GEQ processing*/
                                                    /*Range:
                                                        For 10 band design
                                                        Floating point: (0 to 1.0), Default: 0.251
                                                        In dB         : [-ve infinity to 0dB], Default: -12dB
                                                        Fixed point   : [ 0 to 0x7fffffff), Default: 0x2020c49c
                                                        QFormat : 1.31


                                                        For 5 band design
                                                        Floating point: : (0 to 1.0), Default: 0.316
                                                        In dB         : [-ve infinity to 0dB],, Default: -10dB
                                                        Fixed point   :[ 0 to 0x7fffffff) Default: 0x2872b021, QFormat : 1.31*/


    int32_t i32mOutputGain;                         /*Output Gain*/
                                                    /*Range:
                                                        Floating point: (0.0 - 1.0), Default: 1.0
                                                        Fixed point: (0x0 - 0x7fffffff), default: 0x7fffffff, QFormat - 1.31*/

    int32_t i32mBypassGain;                         /*Bypass Gain
                                                      Range:
                                                        Floating point: (0.0 - 1.0), Default: 1.0
                                                        Fixed point: (0x0 - 0x7fffffff), default: 0x7fffffff, QFormat - 1.31*/

    int32_t  i32mFilterMode;                        /*GEQ filter configuration                                  Range: [0, 1, 2], Default: 0
                                                        0: five band config, 1: 10 band config, 2: Speaker compensation config
                                                    */

}BDSP_Raaga_Audio_GraphicEqUserConfig;

/*Hard limiter user config*/
typedef struct BDSP_Raaga_Audio_HardLimiterUserConfig
{
    int32_t             i32LimiterEnable;           /* Limiter enable separate control */
                                                    /* Default: 1;  1 -> Process, 0 ->  Disable of processing */

    int32_t             i32nchans;                  /* Number of input/output channels     */
                                                    /* Default val 2*/

    int32_t             i32blockSize;               /* Default: 256, range [256, 512, 768, 1024]*/

    int32_t             i32InputGain;               /* Input gain */
                                                    /* Default: 0x20000000 range[0, 0x7fffffff]
                                                       Floating val: default 1: range [0, 4] */

    int32_t             i32OutputGain;              /* Output gain */
                                                    /* Default: 0x20000000 range[0, 0x7fffffff]
                                                       Floating val: default 1: range [0, 4] */

    int32_t             i32BypassGain;              /* Bypass gain */
                                                    /* Default: 0x7fffffff range[0, 0x7fffffff]
                                                       Floating val: default 1: range [0, 1] */

    int32_t             i32LimiterBoost;            /* HL gain boost */
                                                    /* Default: 0x0168f5c3 range[0x000020c5, 0x7fffffff]
                                                       Floating val: default 2.82: range [0.001, 256] */

    int32_t             i32HardLimit;               /* HardLimit */
                                                    /* Default: 0x7fffffff range[0x00010945, 0x7fffffff]
                                                       Floating val: default 1: range [3.16227766e-5, 1] */

    int32_t             i32DelayLength;             /* DelayLength */
                                                    /* Default: 22 range[6, 22]
                                                       Floating val: default 22: range [6, 22] */
}BDSP_Raaga_Audio_HardLimiterUserConfig;

/*
Top Level Data structure for Node 3 of SRS Studio
*/
typedef struct BDSP_Raaga_Audio_SRS_EqualizerHardLimiterConfigParams
{
    BDSP_Raaga_Audio_StudioSoundTopLevelUserConfig       sTopLevelConfig;
    BDSP_Raaga_Audio_ParametricEqUserConfig              sParametricEqConfig;
    BDSP_Raaga_Audio_HighPassFilterUserConfig            sHighPassFilterConfig;
    BDSP_Raaga_Audio_GraphicEqUserConfig                 sGraphicEqConfig;
    BDSP_Raaga_Audio_HardLimiterUserConfig               sHardLimiterConfig;

}BDSP_Raaga_Audio_SRS_EqualizerHardLimiterConfigParams;


/*These structure are defined here till cit support gets checked in*/
/*
   This data structure defines G729 decoder user configuration parameters
*/
typedef struct  BDSP_Raaga_Audio_G729DecUsrCfg
{
	uint32_t 	ui32OutMode;
			/* Default = 2; Output channel configuration */
	uint32_t	ui32ScaleOp;
			/*/ Default = 1; 0 -> FALSE, 1 -> TRUE */
	uint32_t	ui32ScaleIdx;
			/* Default = 0; 0 -> -3dB, 1 -> -6dB, 2 -> -9dB, 3 -> -12dB */
	uint32_t	ui32OutputChannelMatrix[BDSP_AF_P_MAX_CHANNELS];
    /*
    * For Output Port 0:
    * ui32OutputChannelMatrix[0] = 0;
    * ui32OutputChannelMatrix[1] = 1;
    *
    * for(i=2; i<BDSP_AF_P_MAX_CHANNELS; i++)
    * ui32OutputChannelMatrix[i] = 0xFFFFFFFF;
    *
    * For Output Port 1:
    * for(i=0; i<BDSP_AF_P_MAX_CHANNELS; i++)
    * ui32OutputChannelMatrix[i] = 0xFFFFFFFF; this value shows invalid
    */
}BDSP_Raaga_Audio_G729DecUsrCfg;


typedef struct  BDSP_Raaga_Audio_G729DecConfigParams
{
	uint32_t	ui32NumOutPorts;
/* Default = 1;*/
/*1 -> Stereo out, 2 -> Mono + Stereo out*/
	BDSP_Raaga_Audio_G729DecUsrCfg	sUsrOutputCfg[2];

}BDSP_Raaga_Audio_G729DecConfigParams;


typedef struct  BDSP_Raaga_Audio_VorbisUsrCfg
{
    uint32_t    ui32OutMode;            /* Default = 2; Output channel configuration */
    uint32_t    ui32OutLfe;             /* Default = 0; Output channel configuration */
    uint32_t    ui32OutputChannelMatrix[BDSP_AF_P_MAX_CHANNELS];

} BDSP_Raaga_Audio_VorbisUsrCfg;

typedef struct BDSP_Raaga_Audio_MlpConfigParams
{
    /* The Profile Select parameter has been reamed to outmode for commonality with other decoders */
    uint32_t    ui32OutMode;                /* {2 = STEREO, 7 = 5.1, 21 = 7.1} */
    uint32_t    DrcMode;                    /* {Disabled = 0, Follow = 1, Enabled = 2} */
    uint32_t    DrcBoost;
    uint32_t    DrcCut;
    uint32_t    DialogueEnable;
    int32_t     DialogueLevel;              /* in dB */
    uint32_t    EnableSpeakerRemapping;
    uint32_t    SpeakerOutLayout;
    uint32_t    ui32OutputChannelMatrix[BDSP_AF_P_MAX_CHANNELS]; /* Default Channel Matrix = {0, 1, 2, 3, 4, 5, 0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF } */
}BDSP_Raaga_Audio_MlpConfigParams;

typedef struct  BDSP_Raaga_Audio_VorbisDecConfigParams
{
    uint32_t        ui32NumOutPorts;        /* Default = 1;                 = 1 - Multichannel / Stereo out,
                                                                                                                                                                  = 2 - Multichannel + Stereo out  */
    BDSP_Raaga_Audio_VorbisUsrCfg    sUsrOutputCfg[2];

} BDSP_Raaga_Audio_VorbisDecConfigParams;


typedef struct  BDSP_Raaga_Audio_FlacUsrCfg
{
    uint32_t    ui32OutMode;            /* Default = 2; Output channel configuration */
    uint32_t    ui32OutLfe;             /* Default = 0; Output channel configuration */
    uint32_t    ui32OutputChannelMatrix[BDSP_AF_P_MAX_CHANNELS];

} BDSP_Raaga_Audio_FlacUsrCfg;

typedef struct  BDSP_Raaga_Audio_FlacDecConfigParams
{
    uint32_t        ui32NumOutPorts;        /* Default = 1;                 = 1 - Multichannel / Stereo out,
                                                                                                                                                                  = 2 - Multichannel + Stereo out  */
    BDSP_Raaga_Audio_FlacUsrCfg    sUsrOutputCfg[2];

} BDSP_Raaga_Audio_FlacDecConfigParams;

typedef struct  BDSP_Raaga_Audio_MacUsrCfg
{
	uint32_t    ui32OutMode;
	/*
	Default = 2(2/0)
	Range -> 0-9
	0 - Dual to mono
	1 - Mono          - 1/0 format
	2 - Stereo        - 2/0 format
	*/

	uint32_t    ui32DualMode;
	/*
	Default = 2(Stereo)
	Range : 0-3
	0 -> Normal Stereo - Left & Right
	1 -> Dual Left Mono
	2 -> Dual Right Mono
	3 -> Dual Mix Mono
	1. Mix left to left and right to right
	2. Mix right to left
	3. Mix left to right
	*/

	uint32_t    ui32StereoMode;
	/*
	Default = 0
	Range - 0, 1
	0 - LoRo downmixed output
	1 - LtRt downmixed output
	This value is relevant only when ui32OutMode is 2.
	*/

	uint32_t    ui32ScaleEnable;
	    /*
	        Default = 0
	        0 -> FALSE
	        1 -> TRUE
	    */
    uint32_t    ui32OutputChannelMatrix[BDSP_AF_P_MAX_CHANNELS];

} BDSP_Raaga_Audio_MacUsrCfg;

typedef struct  BDSP_Raaga_Audio_MacDecConfigParams
{
    uint32_t        ui32NumOutPorts;        /* Default = 1;                 = 1 - Multichannel / Stereo out,
                                                                                                                                                                  = 2 - Multichannel + Stereo out  */
    BDSP_Raaga_Audio_MacUsrCfg    sUsrOutputCfg[2];

} BDSP_Raaga_Audio_MacDecConfigParams;

typedef struct BDSP_Raaga_Audio_G729EncoderUserConfig
{
    /*Enables or Disable DTX */
    uint32_t     ui32DtxEnable; /* ui32DtxEnable =1 => DTX Enable
								   ui32DtxDisable=0 => DTX Disable
								   Default => ui32DtxEnable=0 (Disabled)*/
	/*Set required Bitrate */
	uint32_t     ui32Bitrate;   /*  ui32Bitrate = 0 => 6.4kbps
								    ui32Bitrate = 1 => 8.0kbps
									ui32Bitrate = 2 =>11.8kbps*/
} BDSP_Raaga_Audio_G729EncoderUserConfig;

/*
   This data structure defines G711/G726 encoder user configuration parameters
*/
/*
1)  BDSP_Raaga_Audio_eCompressionType_aLaw_G726: A law compression to be applied followed by G.726.
2)  BDSP_Raaga_Audio_eCompressionType_uLaw_G726: U law compression to be applied followed by G.726.
3)  BDSP_Raaga_Audio_eCompressionType_aLaw_ext: A law compression has been externally applied. G.726 to be applied.
4)  BDSP_Raaga_Audio_eCompressionType_uLaw_ext: U law compression has been externally applied. G.726 to be applied.
5)  BDSP_Raaga_Audio_eCompressionType_aLaw_disableG726: G.711 A-law to be applied. G.726 compression is disabled
6)  BDSP_Raaga_Audio_eCompressionType_uLaw_disableG726: G.711 U-law to be applied. G.726 compression is disabled
*/

typedef enum BDSP_Raaga_Audio_eG711G726EncCompressionType
{
    BDSP_Raaga_Audio_eCompressionType_aLaw_G726=0,
    BDSP_Raaga_Audio_eCompressionType_uLaw_G726,
    BDSP_Raaga_Audio_eCompressionType_aLaw_ext,
    BDSP_Raaga_Audio_eCompressionType_uLaw_ext,
    BDSP_Raaga_Audio_eCompressionType_aLaw_disableG726,
    BDSP_Raaga_Audio_eCompressionType_uLaw_disableG726,
    BDSP_Raaga_Audio_eCompressionType_eINVALID=0x7fffffff

}BDSP_Raaga_Audio_eG711G726EncCompressionType;

/*
Bit rates supported
*/

typedef enum BDSP_Raaga_Audio_eG711G726EncBitRate
{
    BDSP_Raaga_Audio_eBitRate_16kbps=0,
    BDSP_Raaga_Audio_eBitRate_24kbps=1,
    BDSP_Raaga_Audio_eBitRate_32kbps=2,
    BDSP_Raaga_Audio_eBitRate_40kbps=3,
    /* Bit rates of 16, 24, 32, 40 kbps to be used along with G.726 */
    BDSP_Raaga_Audio_eBitRate_64kbps=4,
    /* Bit rate of 64 kbps to be used along with G.711 only*/
    BDSP_Raaga_Audio_eBitRate_eINVALID=0x7fffffff

}BDSP_Raaga_Audio_eG711G726EncBitRate;

typedef struct BDSP_Raaga_Audio_G711_G726EncConfigParams
{
    BDSP_Raaga_Audio_eG711G726EncCompressionType    eCompressionType;
    /* Default = BDSP_Raaga_Audio_eCompressionType_uLaw_disableG726 */
    BDSP_Raaga_Audio_eG711G726EncBitRate            eBitRate;
    /* Default = BDSP_Raaga_Audio_eBitRate_64kbps */
}BDSP_Raaga_Audio_G711_G726EncConfigParams;


typedef struct BDSP_Raaga_Audio_G726UsrCfg
{
    uint32_t ui32OutMode;
    /* Default = 2; Output channel configuration */
    uint32_t ui32ApplyGain;
    /* Default = 1, 0 -> FALSE, 1 -> TRUE */
    int32_t i32GainFactor;
    /* Default = 0x5A827999, corresponding to -3dB in Q1.31 format; valid
    only if ui32ApplyGain = 1 */
    uint32_t ui32OutputChannelMatrix[BDSP_AF_P_MAX_CHANNELS];
    /*
    * For Output Port 0:
    * ui32OutputChannelMatrix[0] = 0;
    * ui32OutputChannelMatrix[1] = 1;
    *
    * for(i=2; i<BDSP_AF_P_MAX_CHANNELS; i++)
    * ui32OutputChannelMatrix[i] = 0xFFFFFFFF;
    *
    * For Output Port 1:
    * for(i=0; i<BDSP_AF_P_MAX_CHANNELS; i++)
    * ui32OutputChannelMatrix[i] = 0xFFFFFFFF; this value shows invalid
    */
} BDSP_Raaga_Audio_G726UsrCfg;

typedef struct BDSP_Raaga_Audio_G726ConfigParams
{
    uint32_t ui32NumOutPorts;
    /* Default = 1; 1 -> Stereo out, 2 -> Mono + Stereo out */
    uint32_t ui32OutputType;
    /* Default = 0; 0 -> PCM samples, 1 -> 8-bit ADPCM u/A-law packed data */
    BDSP_Raaga_Audio_G726UsrCfg sUsrOutputCfg[2];
} BDSP_Raaga_Audio_G726ConfigParams;

/*
   This data structure defines G722 encoder user configuration parameters
*/

typedef struct BDSP_Raaga_Audio_G722EncConfigParams
{
    int32_t dummy;
}BDSP_Raaga_Audio_G722EncConfigParams;

typedef struct BDSP_Raaga_Audio_AdpcmUsrCfg
{
    uint32_t ui32OutMode;
    /* Default = 1; Output channel configuration */
    uint32_t ui32ApplyGain;
    /* Default = 0, 0 -> FALSE, 1 -> TRUE */
    int32_t i32GainFactor;
    /* Default = 0x016A09E6, corresponding to +3dB in Q8.24 format; valid
    only if ui32ApplyGain = 1 */
    uint32_t ui32OutputChannelMatrix[BDSP_AF_P_MAX_CHANNELS];
    /*
    * For Port 0:
    * ui32OutputChannelMatrix[0] = 0;
    * ui32OutputChannelMatrix[1] = 1;
    *
    * for(i=2; i<BDSP_AF_P_MAX_CHANNELS; i++)
    * ui32OutputChannelMatrix[i] = 0xFFFFFFFF;
    *
    * For Port 1:
    * for(i=0; i<BDSP_AF_P_MAX_CHANNELS; i++)
    * ui32OutputChannelMatrix[i] = 0xFFFFFFFF; this value shows invalid
    */
} BDSP_Raaga_Audio_AdpcmUsrCfg;
typedef struct BDSP_Raaga_Audio_AdpcmConfigParams
{
    uint32_t ui32NumOutPorts;
    /* Default = 1; 1 -> Stereo out, 2 -> Multichannel + Stereo out */
    BDSP_Raaga_Audio_AdpcmUsrCfg sUsrOutputCfg[2];
} BDSP_Raaga_Audio_AdpcmConfigParams;
typedef struct BDSP_Raaga_Audio_AMRWBConfigParams
{
    uint32_t    ui32allow_dtx;		 	/* Default = 1; 0-false 1-true */
    uint32_t    ui32bitstreamformat;		/* Default = 2; 1-itu    2-mime */
    uint32_t    ui32mode;
	/*
	     Default 0
	     mode = 0        BitRate = 6600
	     mode = 1        BitRate = 8850
	     mode = 2        BitRate = 12650
	     mode = 3        BitRate = 14250
	     mode = 4        BitRate = 15850
	     mode = 5        BitRate = 18250
	     mode = 6        BitRate = 19850
	     mode = 7        BitRate = 23050
	     mode = 8        BitRate = 23850
	*/
}BDSP_Raaga_Audio_AMRWBConfigParams;

typedef struct BDSP_Raaga_Audio_ILBCConfigParams
{
	uint32_t   mode;		/* Default = 20;20->frame length 20msec ,30->frame length 30msec */	

}BDSP_Raaga_Audio_ILBCConfigParams;


typedef struct  BDSP_Raaga_Audio_GenCdbItbConfigParams
{
    /* Switch to turn off encoder's mux output on the fly */
    BDSP_AF_P_EnableDisable     eEnableEncode;
	
	/* If snapshot of STC HW register is required for ESCR/PTS etc */
	BDSP_AF_P_EnableDisable 	eSnapshotRequired;
	/* The RDB register to be read for encoder's ESCR clock */
	uint32_t                    ui32EncSTCAddr;
	/* Arrival to presentation time in Msecs, described in section 3.2.1 of transcode system arch 1.4 */
	uint32_t                    ui32A2PInMilliSeconds;

	/* Enable (1) / Disable (0) : Default - 0 (false)*/
	BDSP_AF_P_EnableDisable     eStcItbEntryEnable;

    /* The RDB register address to read for encoder's STC clock (Upper)*/
    uint32_t                    ui32EncSTCAddrUpper;

} BDSP_Raaga_Audio_GenCdbItbConfigParams;

typedef struct BDSP_Raaga_Audio_SpectrumAnalyserConfigParams
{

     /* This value indicates whether downmixeing enabled or disable. set to "1"
        to ensure that the input channels and downmixed and then the downmixed
        data is used for generating spectral lines. If set to zero, then data
        is generated for all input channels.

        Range: 0 to 1.
               Set  to 0 for disable downmix, set  to 1 for enable downmix

       */

    int32_t ui32UseDownmixedDataForWindowFFT ;

    /* This value indicates Frame rate of Host device. If set to -1, then all
         input data is passed through FFT and is pushed to output buffer.
       If set to a valid sampling frequency like 25/30, only that many FFT
         blocks are computed.

        Range: -1 to 93.

                Set to -1 for all input data is passed through FFT
                Set to  a valid sampling frequency like 25/30.


        */

    int32_t i32HostRequiredFrameRate ;



} BDSP_Raaga_Audio_SpectrumAnalyserConfigParams;


/* BTSC encoder user config params

    This data structure defines BTSC configuration parameters
*/

typedef struct BDSP_Raaga_Audio_BtscEncoderConfigParams
{
    BDSP_AF_P_EnableDisable eBTSCEnableFlag;                    /*Default Enable*/
    BDSP_AF_P_EnableDisable eUseDeEmphasizedSourceSignals;      /*Default Enable*/
    BDSP_AF_P_EnableDisable eMainChannelPreEmphasisOn;          /*Default Enable*/
    BDSP_AF_P_EnableDisable eEquivalenMode75MicroSec;           /*Default Disable*/
    BDSP_AF_P_EnableDisable eClipDiffChannelData;               /*Default Enable*/
    BDSP_AF_P_EnableDisable eDiffChannelPreEmphasisOn;          /*Default Enable*/
    BDSP_AF_P_EnableDisable eUseLowerThresholdSpGain;           /*Default Enable*/
    uint32_t ui32AttenuationFactor;                             /*Q-format 3.29 format: Maximum is 0x7fffffff and default is : 0x20000000  */
    uint32_t ui32SRDCalibrationFactor;              			/*Q-format 4.28 format: Maximum value is 0x7fffffff corresponding to 8.0:
                                                    			Default :12BE76C9 for next gen 0xE666666 for 7425 Ref board*/
    BDSP_AF_P_EnableDisable eSumChanFreqDevCtrl;        	/*Default Enable Used to Control FM Deviation of Sum Channel to 25Khz*/
    BDSP_AF_P_EnableDisable eDiffChanFreqDevCtrl;       	/*Default Enable Used to Control FM Deviation of Diff Channel to 50Khz*/

    BDSP_AF_P_EnableDisable eOpFreqDevCtrl;         		/*Default Enable Used to Control FM Deviation of Final Output to 50Khz*/
    BDSP_AF_P_EnableDisable eFreqGeneration;            	/*Default Disable. Used to Generate Test Tones*/
    int32_t   i32Frequency;     							/*Default 0 generates frequency sweep.  Programming to a fixed value generates tone*/

    uint32_t i32SumChannelLevelCtrl;				/*Q-format 3.29 format: Maximum is 0x7fffffff and default is : 0x20000000  */
    uint32_t i32DiffChannelLevelCtrl; 				/*Q-format 3.29 format: Maximum is 0x7fffffff and default is : 0x20000000  */

	BDSP_AF_P_EnableDisable eBTSCMonoModeOn;  /* Default Disable */
	BDSP_AF_P_EnableDisable eMuteRightChannel; /*Default Disbale - Mute the Right for Stereo Separation Tests*/

}BDSP_Raaga_Audio_BtscEncoderConfigParams;

typedef struct  BDSP_Raaga_Audio_G723_1_UsrCfg
{
    uint32_t    ui32OutMode;    /* Default = 2; Output channel configuration */
    uint32_t    ui32ScaleOp;    /* Default = 1; 0 -> FALSE, 1 -> TRUE */
    uint32_t    ui32ScaleIdx;   /* Default = 0; 0 -> -3dB, 1 -> -6dB, 2 -> -9dB, 3 -> -12dB */
    uint32_t    ui32OutputChannelMatrix[BDSP_AF_P_MAX_CHANNELS];
    /*
    * For Output Port 0:
    * ui32OutputChannelMatrix[0] = 0;
    * ui32OutputChannelMatrix[1] = 1;
    *
    * for(i=2; i<BDSP_AF_P_MAX_CHANNELS; i++)
    * ui32OutputChannelMatrix[i] = 0xFFFFFFFF;
    *
    * For Output Port 1:
    * for(i=0; i<BDSP_AF_P_MAX_CHANNELS; i++)
    * ui32OutputChannelMatrix[i] = 0xFFFFFFFF; this value shows invalid
    */

} BDSP_Raaga_Audio_G723_1DEC_UsrCfg;

typedef struct  BDSP_Raaga_Audio_G723_1_ConfigParams
{
    uint32_t                ui32NumOutPorts;    /* Default = 1; 1 -> Stereo out, 2 -> Mono + Stereo out */

    BDSP_Raaga_Audio_G723_1DEC_UsrCfg    sUsrOutputCfg[2];

} BDSP_Raaga_Audio_G723_1DEC_ConfigParams;

/* WMA Encoder user config structure */
typedef struct BDSP_Raaga_Audio_WMAEncoderConfigParams
{
	/* Desired bit rate of the encoder. Default = 192000 */
	/* WARN: For lower bitrates, DSP processing is very HIGH!! */
    uint32_t ui32Bitrate;
	
	/* Set to 1 if mono down mix is required. 
	Default = 0, Mono Downmix = 1, Stereo encoding = 0 */
    uint32_t ui32DownMixToMono;       

} BDSP_Raaga_Audio_WMAEncoderConfigParams;

typedef struct BDSP_Raaga_Audio_ISACConfigParams
{
    uint16_t    ui16frameLen ;              /* Default = 30;range: 30 msec or 60 msec */

    uint16_t    ui16CodingMode;             /* Default = 0;range: 0 - 1, 0 - channel-adaptive, 1 - channel-independent  */

    uint16_t    ui16nbTest;                 /* Default = 1;range: 0 - 1, 0 - wide band, 1 - narrow band */

    uint16_t    ui16fixedFL;                /* Default = 1;range: 0 - 1, 0 - Variable frame length, 1-Fixed frame length*/

    uint16_t    ui16rateBPS;                /* Default = 32000;range: 10000 - 32000 */

    uint16_t    ui16bottleneck;             /* Default = 32000;range: 10000 - 32000 */

    uint16_t    ui16payloadSize;            /* Default = 200;range: 100 - 400     */

    uint16_t    ui16payloadRate;            /* Default = 32000;range: 32000 - 53400 */
}BDSP_Raaga_Audio_ISACConfigParams;


typedef struct
{
    uint32_t WrkRate;    /* Bitrate: 0 - 6.3 Kbps 1- 5.3 Kbps Default: 0 */
    uint32_t UseHp;      /* High Pass Filter 1 - Enabled 0 - Disabled :  Default 1 */
    uint32_t UseVx;      /* VAD Status 1 - Enabled 0 - Disabled : Default Disable */
  
}BDSP_Raaga_Audio_G723EncoderUserConfig;
/* Video specific definitions */
/****************************************************************************/
/****************************************************************************/
/************************* VIDEO Algo  **************************************/
/****************************************************************************/
/****************************************************************************/
/* Video Encoder UD */
/* Encoder IDS userconfig */
typedef struct BDSP_VideoEncodeTaskDatasyncSettings 
{
    BDSP_AF_P_EnableDisable eEnableStc;				/* Will be used if IpPortType is I2S*/
    uint32_t ui32StcAddress;						/* For RfI2s i.e. BTSC */

} BDSP_VideoEncodeTaskDatasyncSettings ;

/* H.264 */
/***************************************************************************
Summary: 
	Defines the Slice type

Description:

See Also:
	None.
****************************************************************************/
typedef enum
{
    BDSP_Raaga_VideoSlice_eI	            = 0x0,      
    BDSP_Raaga_VideoSlice_eP,
    BDSP_Raaga_VideoSlice_eB,
	BDSP_Raaga_VideoSlice_eMax,
	BDSP_Raaga_VideoSlice_eInvalid   = 0x7FFFFFFF
}BDSP_Raaga_VideoSliceType;

/***************************************************************************
Summary: 
	Defines the encode mode

Description:

See Also:
	None.
****************************************************************************/
typedef enum
{
    BDSP_Raaga_VideoEncodeMode_eHighDelay   				    = 0x0,      
    BDSP_Raaga_VideoEncodeMode_eLowDelay    				    = 0x1,      
    BDSP_Raaga_VideoEncodeMode_eAfap         				    = 0x2,
	BDSP_Raaga_VideoEncodeMode_eInvalid		                = 0x7FFFFFFF
}BDSP_Raaga_VideoEncodeMode;

/***************************************************************************
Summary: 
	Deblock to be disabled or enabled

Description:

See Also:
	None.
****************************************************************************/
typedef enum
{
	BDSP_Raaga_Video_FALSE									    = 0x0,
	BDSP_Raaga_Video_TRUE									    = 0x1
}BDSP_Raaga_Video_eBOOLEAN;

/***************************************************************************
Summary: 
	GOP structure to be used for encoding

Description:

See Also:
	None.
****************************************************************************/
typedef enum
{
    BDSP_Raaga_VideoGopStruct_eIOnly		       			    = 0x0,      
    BDSP_Raaga_VideoGopStruct_eIP			       			    = 0x1,      
	BDSP_Raaga_VideoGopStruct_eIPB   							= 0x2,
    BDSP_Raaga_VideoGopStruct_eIPBB			       			    = 0x3,
    BDSP_Raaga_VideoGopStruct_eMax,
	BDSP_Raaga_VideoGopStruct_eInvalid					        = 0x7FFFFFFF
}BDSP_Raaga_VideoGopStruct;

/* H264 Encoder User config */
typedef enum
{
	BDSP_Raaga_VideoH264Profile_eBaseline		                            = 66,
	BDSP_Raaga_VideoH264Profile_eMain									    = 77,
	BDSP_Raaga_VideoH264Profile_eExtended								    = 88,
	BDSP_Raaga_VideoH264Profile_eFRExtProfileHigh						    = 100,							
	BDSP_Raaga_VideoH264Profile_eFRExtProfileHigh10							= 110,	
	BDSP_Raaga_VideoH264Profile_eFRExtProfileHigh422					    = 122,	
	BDSP_Raaga_VideoH264Profile_eFRExtProfileHigh444					    = 244,	
	BDSP_Raaga_VideoH264Profile_eFRExtProfileHighCavlc444					= 44	
}BDSP_Raaga_VideoH264ProfileIdc;
/***********************************
*
* Following Config Parameters can be set 
* as one time parameters
*
***********************************/
typedef struct BDSP_Raaga_VideoBH264RCConfigStruct
{
	uint32_t   basicunit; /* Size of Basic Unit in MBs. '0' impliese FrameLevel RC */
    /* Min/ Max Qp for every slice I/P/B */
	uint32_t			RCMinQP[BDSP_Raaga_VideoSlice_eMax];
    uint32_t			RCMaxQP[BDSP_Raaga_VideoSlice_eMax];

    uint32_t    RCMaxQPChange;/*Max Qp Change allowed between consequetive BUs in BU-level RC
                              * At frame-level, Max Qp change allowed is sett to 2 for smooth quality*/
	uint32_t	SeinitialQp; /* Initial Qp: Initial config parameter.*/

}BDSP_Raaga_VideoBH264RCConfigStruct;


typedef struct BDSP_Raaga_VideoBH264UserConfig
{	
	BDSP_Raaga_VideoH264ProfileIdc					eProfileIDC;
	uint32_t										ui32LevelIdc;			/* ranges from 9 to 51. For SD it is 30 */
	BDSP_Raaga_VideoEncodeMode	 					eMode;					/* (High Delay, Low Delay, AFAP)	 */
	uint32_t 										ui32TargetBitRate;			/* Number of bits per sec.	*/
	uint32_t 										ui32EncodPicWidth;
	uint32_t 										ui32EncodPicHeight;
	uint32_t										ui32IntraPeriod;
	uint32_t										ui32IDRPeriod;
	BDSP_Raaga_VideoGopStruct					    eGopStruct;
	BDSP_Raaga_Video_eBOOLEAN						eDblkEnable;
	BDSP_Raaga_Video_eBOOLEAN						eRateControlEnable;    
	uint32_t										ui32End2EndDelay;	
	/* Rate control */
    BDSP_Raaga_VideoBH264RCConfigStruct				sRCConfig;
    uint32_t										ui32FrameRate;
	uint32_t										ui32AspectRatioIdc;
	uint32_t										ui32SARWidth;
	uint32_t										ui32SARHeight;
	BDSP_Raaga_Video_eBOOLEAN						eSendAud;

	/* 0 - Disable Variance based intra/inter mod decsion; 1 - Enable Variance based intra/inter mod decsion */
	BDSP_Raaga_Video_eBOOLEAN                       eVarModeDecsOptEnable;
	/* 0 - Disable Subpel; 1 - Enable Subpel interpolation filter */
	BDSP_Raaga_Video_eBOOLEAN                       eSubPelEnable;
    /* 0 - Disable I4x4 mode  in I frm; 1 - Enable I4x4 in I frm */
    BDSP_Raaga_Video_eBOOLEAN                       eI4x4Enable;
    /* 0 - Disable CC data ; 1 - Enable CC data */
    BDSP_Raaga_Video_eBOOLEAN                       eSendCC;
}BDSP_Raaga_VideoBH264UserConfig;

typedef struct BDSP_Raaga_VideoBVP8UserConfig
{
    BDSP_Raaga_VideoH264ProfileIdc                  eProfileIDC;
    uint32_t                                        ui32LevelIdc;           /* ranges from 9 to 51. For SD it is 30 */
    BDSP_Raaga_VideoEncodeMode                      eMode;                  /* (High Delay, Low Delay, AFAP)     */
    uint32_t                                        ui32TargetBitRate;          /* Number of bits per sec.  */
    uint32_t                                        ui32EncodPicWidth;
    uint32_t                                        ui32EncodPicHeight;
    uint32_t                                        ui32IntraPeriod;
    uint32_t                                        ui32IDRPeriod;
    BDSP_Raaga_VideoGopStruct                       eGopStruct;
    BDSP_Raaga_Video_eBOOLEAN                       eDblkEnable;
    BDSP_Raaga_Video_eBOOLEAN                       eRateControlEnable;
    uint32_t                                        ui32End2EndDelay;
    /* Rate control */
    BDSP_Raaga_VideoBH264RCConfigStruct             sRCConfig;
    uint32_t                                        ui32FrameRate;
    uint32_t                                        ui32AspectRatioIdc;
    uint32_t                                        ui32SARWidth;
    uint32_t                                        ui32SARHeight;
    BDSP_Raaga_Video_eBOOLEAN                       eSendAud;
    /* 0 - Disable Variance based intra/inter mod decsion; 1 - Enable Variance based intra/inter mod decsion */
    BDSP_Raaga_Video_eBOOLEAN                       eVarModeDecsOptEnable;
    /* 0 - Disable Subpel; 1 - Enable Subpel interpolation filter */
    BDSP_Raaga_Video_eBOOLEAN                       eSubPelEnable;
    /* 0 - Disable I4x4 mode  in I frm; 1 - Enable I4x4 in I frm */
    BDSP_Raaga_Video_eBOOLEAN                       eI4x4Enable;
}BDSP_Raaga_VideoBVP8UserConfig;

extern const BDSP_Raaga_Audio_MpegConfigParams  BDSP_sMpegDefaultUserConfig;
extern const BDSP_Raaga_Audio_AacheConfigParams BDSP_sAacheDefaultUserConfig;
extern const BDSP_Raaga_Audio_DolbyPulseUserConfig BDSP_sDolbyPulseDefaultUserConfig;
extern const BDSP_Raaga_Audio_DDPMultiStreamConfigParams  BDSP_sDDPDefaultUserConfig;
extern const BDSP_Raaga_Audio_DtsHdConfigParams  BDSP_sDtsHdDefaultUserConfig;
extern const BDSP_Raaga_Audio_DtslbrConfigParams  BDSP_sDtsLbrDefaultUserConfig;
extern const BDSP_Raaga_Audio_LpcmUserConfig  BDSP_sLcpmDvdDefaultUserConfig;
extern const BDSP_Raaga_Audio_PassthruConfigParams  BDSP_sPcmDefaultUserConfig;
extern const BDSP_Raaga_Audio_PcmWavConfigParams  BDSP_sPcmWavDefaultUserConfig;
extern const BDSP_Raaga_Audio_AmrConfigParams BDSP_sAmrDefaultUserConfig;
extern const BDSP_Raaga_Audio_WmaConfigParams  BDSP_sWmaDefaultUserConfig;
extern const BDSP_Raaga_Audio_WmaProConfigParams  BDSP_sWmaProDefaultUserConfig;
extern const BDSP_Raaga_Audio_DraConfigParams  BDSP_sDraDefaultUserConfig;
extern const BDSP_Raaga_Audio_RalbrConfigParams  BDSP_sRalbrDefaultUserConfig;
extern const BDSP_Raaga_Audio_AdpcmConfigParams  BDSP_sAdpcmDefaultUserConfig;
extern const BDSP_Raaga_Audio_G726ConfigParams BDSP_sG711G726DecUserConfig;
extern const BDSP_Raaga_Audio_G729DecConfigParams BDSP_sG729DecUserConfig;
extern const BDSP_Raaga_Audio_VorbisDecConfigParams BDSP_sVorbisDecUserConfig;
extern const BDSP_Raaga_Audio_FlacDecConfigParams BDSP_sFlacDecUserConfig;
extern const BDSP_Raaga_Audio_MacDecConfigParams BDSP_sMacDecUserConfig;
extern const BDSP_Raaga_Audio_AmrwbdecConfigParams BDSP_sAmrwbdecDefaultUserConfig;
extern const BDSP_Raaga_Audio_iLBCdecConfigParams BDSP_siLBCdecDefaultUserConfig;
extern const BDSP_Raaga_Audio_iSACdecConfigParams BDSP_siSACdecDefaultUserConfig;
extern const BDSP_Raaga_Audio_MlpConfigParams BDSP_sMlpUserConfig;
extern const BDSP_Raaga_Audio_PassthruConfigParams   BDSP_sDefaultPassthruSettings;
extern const BDSP_Raaga_Audio_AVLConfigParams BDSP_sDefAVLConfigSettings;
extern const BDSP_Raaga_Audio_TruVolumeUserConfig BDSP_sDefSrsTruVolumeUserConfig;
extern const BDSP_Raaga_Audio_DDReencodeConfigParams BDSP_sDefDDReencodeUserConfig; 
extern const BDSP_Raaga_Audio_DV258ConfigParams BDSP_sDefDV258UserConfig; 
extern const BDSP_Raaga_Audio_DolbyVolumeUserConfig BDSP_sDefDolbyVolUserConfig; 	
extern const BDSP_Raaga_Audio_PL2ConfigParams BDSP_sDefProLogicIIConfigSettings;  
extern const BDSP_Raaga_Audio_TruSurrndXTConfigParams BDSP_sDefTruSurroundXtSettings; 	
extern const BDSP_Raaga_Audio_SRCUserConfigParams   BDSP_sDefaultSrcSettings;
extern const BDSP_Raaga_Audio_CustomSurroundUserConfig BDSP_sDefCustomSurroundConfigSettings; 	
extern const BDSP_Raaga_Audio_CustomBassUserConfig BDSP_sDefaultCustomBassSettings;	
extern const BDSP_Raaga_Audio_CustomVoiceConfigParams BDSP_sDefCustomVoiceConfigSettings;	
extern const BDSP_Raaga_Audio_GenCdbItbConfigParams BDSP_sDefGenCdbItbConfigSettings; 
extern const BDSP_Raaga_Audio_BtscEncoderConfigParams BDSP_sDefBtscEncoderConfigSettings; 
extern const BDSP_Raaga_Audio_AudioDescPanConfigParams BDSP_sDefAdPanConfigSettings;	
extern const BDSP_Raaga_Audio_DtsBroadcastEncConfigParams BDSP_sDefDTSENCConfigSettings; 
extern const BDSP_Raaga_Audio_AacheEncConfigParams BDSP_sDefAacHeENCConfigSettings; 
extern const BDSP_Raaga_Audio_Ac3EncConfigParams BDSP_sDefAc3ENCConfigSettings; 
extern const BDSP_Raaga_Audio_DDTranscodeConfigParams BDSP_sDefDDTranscodeConfigSettings; 
extern const BDSP_Raaga_Audio_G711_G726EncConfigParams BDSP_sDefG711G726EncConfigSettings;
extern const BDSP_Raaga_Audio_G729EncoderUserConfig BDSP_sDefG729EncConfigSettings;
extern const BDSP_Raaga_Audio_Mpeg1L3EncConfigParams   BDSP_sDefMpeg1L3EncConfigSettings; 
extern const BDSP_Raaga_Audio_SbcEncoderUserConfig   BDSP_sDefSbcEncConfigSettings; 
extern const BDSP_Raaga_Audio_Brcm3DSurroundConfigParams   BDSP_sDefBrcm3DSurroundConfigSettings; 
extern const BDSP_Raaga_Audio_MonoDownMixConfigParams   BDSP_sDefMonoDownmixConfigSettings; 
extern const BDSP_Raaga_Audio_MixerConfigParams  BDSP_sDefFwMixerConfigSettings; 
extern const BDSP_Raaga_Audio_AudysseyVolUserConfig BDSP_sDefAudysseyVolConfigSettings; 
extern const BDSP_Raaga_Audio_ABXConfigParams BDSP_sDefAudysseyABXConfigSettings; 
extern const BDSP_Raaga_Audio_SRS_CSDecTruDialogConfigParams BDSP_sDefSrsCsdecTDConfigSettings; 
extern const BDSP_Raaga_Audio_SRS_EqualizerHardLimiterConfigParams BDSP_sDefSrsEqHlConfigSettings; 
extern const BDSP_Raaga_Audio_DbeConfigParams BDSP_sDefCustomDbeConfigSettings; 
extern const BDSP_Raaga_Audio_AvlPConfigParams BDSP_sDefCustomAvlplusConfigSettings; 
extern const BDSP_Raaga_Audio_DsolaConfigParams BDSP_sDefDsolaConfigSettings;
extern const BDSP_Raaga_Audio_TruSurrndHDConfigParams BDSP_sDefTruSurrndHDConfigSettings;
extern const BDSP_Raaga_Audio_G723_1DEC_ConfigParams BDSP_sG723_1_Configsettings;
extern const BDSP_Raaga_Audio_WMAEncoderConfigParams BDSP_sDefWmaEncodeConfigSettings;
extern const BDSP_Raaga_Audio_G723EncoderUserConfig BDSP_sDefG723_1EncodeConfigSettings;
extern const BDSP_Raaga_VideoBH264UserConfig BDSP_sBH264EncodeUserConfigSettings;
extern const BDSP_Raaga_VideoBVP8UserConfig BDSP_sBVP8EncodeUserConfigSettings;
extern const BDSP_Raaga_Audio_Broadcom3DSurroundConfigParams BDSP_sBroadcom3DSurroundConfigSettings;
extern const BDSP_Raaga_Audio_SpeexAECConfigParams	BDSP_sDefSpeexAECConfigParams;
extern const BDSP_Raaga_Audio_G722EncConfigParams BDSP_sDefG722EncConfigSettings;
extern const BDSP_Raaga_Audio_AmrEncoderUserConfig BDSP_sDefAmrEncConfigSettings;
extern const BDSP_Raaga_Audio_AMRWBConfigParams BDSP_sDefAmrwbEncConfigSettings;
extern const BDSP_Raaga_Audio_ILBCConfigParams BDSP_sDefiLBCEncConfigSettings;
extern const BDSP_Raaga_Audio_ISACConfigParams  BDSP_sDefiSACEncConfigSettings;
#endif
