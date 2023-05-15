/***************************************************************************
 *     Copyright (c) 2006-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdsp_raaga_img.h $
 * $brcm_Revision: Hydra_Software_Devel/20 $
 * $brcm_Date: 1/10/13 11:21a $
 *
 * Module Description: Host DSP Task Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/dsp/src/raaga/firmware/fp2011/bdsp_raaga_img.h $
 * 
 * Hydra_Software_Devel/20   1/10/13 11:21a skalapur
 * FWRAA-474:iLBC code merging to main latest
 * 
 * Hydra_Software_Devel/19   1/9/13 11:48p ananthan
 * FWRAA-535: Removing duplicate entries for SCM task in image interface
 * structure.
 * 
 * Hydra_Software_Devel/18   11/27/12 7:23p ananthan
 * SW7425-4242: Add SCM3 interface and merge SCM to mainline.
 * 
 * Hydra_Software_Devel/17   8/23/12 12:17a rshijith
 * FWRAA-424: Code modification to support VP8 encoder in raaga.
 * 
 * Hydra_Software_Devel/16   7/17/12 2:14p dlimaye
 * SW7425-2805: Add AMR-NB and AMR-WB codecs support Added new codecs: AMR
 * and AMRWB decoder/encoder
 * 
 * Hydra_Software_Devel/15   6/6/12 1:01p ananthan
 * FWRAA-443: Add MAT Encoder support in Raaga
 * 
 * Hydra_Software_Devel/14   5/11/12 3:32p dlimaye
 * FWRAA-433: Adding G722 encoder to raaga Adding bdsp changes and
 * sys_top_raaga changes
 * 
 * Hydra_Software_Devel/13   3/30/12 12:32a gprasad
 * SW7425-2786: BDSP changes for adding MLP Passthru support
 * 
 * Hydra_Software_Devel/12   3/16/12 5:09p ramanatm
 * SW7425-2378:[7425] Add support for Monkey Audio Codec (MAC)
 * 
 * Hydra_Software_Devel/11   2/6/12 10:26p ashoky
 * FWRAA-377: Merging raaga soft video encoder to mainline.
 * 
 * Hydra_Software_Devel/10   2/3/12 12:25p ramanatm
 * SW7425-1752:[7425] Add support for FLAC media file format
 * 
 * Hydra_Software_Devel/9   1/13/12 9:52p gprasad
 * FWRAA-342: Adding support for SPEEX AEC
 * 
 * Hydra_Software_Devel/8   12/29/11 5:55p gprasad
 * FWRAA-370: G723.1 codec development on Raaga platform
 * 
 * Hydra_Software_Devel/7   12/5/11 1:57p ashoky
 * FWRAA-330: Adding WMA encoder support.
 * 
 * Hydra_Software_Devel/6   11/8/11 1:40p ramanatm
 * FWRAA-348:[7425] Vorbis bringup
 * 
 * Hydra_Software_Devel/5   8/16/11 9:11a gprasad
 * FWRAA-327: Adding g.7XX fw binaries for B0
 * 
 * Hydra_Software_Devel/4   7/13/11 3:00p gprasad
 * SW7425-858: Runtime errors with BDSP_3RDPARTY_ALGO_SUPPORT=n - code
 * cleanup
 * 
 * Hydra_Software_Devel/3   6/16/11 7:51p gprasad
 * SW7422-373: Add GetInfo routines for AudioType, AudioProcessing, and
 * AudioEncode in BDSP
 * 
 * Hydra_Software_Devel/2   5/19/11 12:02p gprasad
 * SW7425-572: [7425] Adding VP6 support to basemodules
 * 
 * Hydra_Software_Devel/1   4/14/11 11:25a gprasad
 * FWRAA-280: Adding support for fp-2011
 * 
 * Hydra_Software_Devel/1   4/5/11 4:08p gprasad
 * SW7425-286: [7425] Checkin raaga binaries to magnum into basemodules
 * folder
 * 
 * Hydra_Software_Devel/5   3/28/11 6:00p srajapur
 * SW7422-247 : [7422] Changing BRAP to BDSP
 * 
 * Hydra_Software_Devel/4   3/5/11 5:05a srajapur
 * SW7422-247 : [7422] Adding GenCdbItb and AACHE encode audio processing
 * support.  SW7422-314 : [7422] Adding RDB Independence Feature in BASE
 * DSP Codebase
 * 
 * Hydra_Software_Devel/3   2/10/11 7:03a srajapur
 * SW7422-229:[7422]Added support for following audio codec algorithms
 * ========== WMA - tested WMAPRO - tested DRA - tested DTSHD - tested
 * DTSBROADCAST - tested LPCMDVD - tested PCMWAV PCM REALAUDIOLBR ADPCM
 * ==========
 * 
 * Hydra_Software_Devel/2   1/13/11 5:50a gautamk
 * SW7422-191:[7422]Adding DSP support for Audio decode in Basemodule  It
 * includes - DSP Booting - Open time Download :  - Start Time Audio
 * Decode algorithm download  - Message buffer interaction with DSP:  -
 * Synchronized Interrupt:  - Async interrupts:  - Config Commands for
 * Decode/TSM/IDS: - Status Buffer for Decode/TSM/IDS:  - Start AC3 Audio
 * Decode:  - Pause/Resume Command:  - Frame Advance command:  - Device
 * level interrupt
 * 
 * Hydra_Software_Devel/1   12/15/10 6:50p jgarrett
 * SW7422-146: Initial compileable prototype
 * 
 ***************************************************************************/

#ifndef BDSP_RAAGA_IMG_H_
#define BDSP_RAAGA_IMG_H_

#include "bimg.h"

typedef enum BDSP_Img_Id
{
        BDSP_Img_Id_eSystemCode,
        BDSP_Img_Id_eSystemData,
        BDSP_Img_Id_eSystemRdbvars,        
        BDSP_Img_Id_eSyslibCode,
        BDSP_Img_Id_eAlgolibCode,
        BDSP_Img_Id_eCommon_Ids_Code,
   		BDSP_Img_Id_eVidCom_Encode_Ids,
		BDSP_Img_Id_eVidCom_Encode_Ids_Interframe,
		BDSP_Img_Id_eScm_Task_Code,
		BDSP_Img_Id_eScm1_Digest,
		BDSP_Img_Id_eScm2_Digest,
        BDSP_Img_Id_ePassthruCode,
        BDSP_Img_Id_ePassthruTable,
        BDSP_Img_Id_ePassthruInterFrame,
        
        BDSP_Img_Id_eAc3_Ids,
        BDSP_Img_Id_eAc3_Ids_Interframe,
        
        BDSP_Img_Id_eMpeg1_Ids,
        BDSP_Img_Id_eMpeg1_Ids_Interframe,
        
        BDSP_IMG_Id_eAache_Adts_Ids,
        BDSP_IMG_Id_eAache_Loas_Ids,
        BDSP_IMG_Id_eAache_Adts_Ids_Interframe,
        BDSP_IMG_Id_eAache_Loas_Ids_Interframe,
     
        BDSP_Img_Id_eDdp_Ids,
        BDSP_Img_Id_eDdp_Ids_Interframe,
        
        BDSP_Img_Id_eDtsBroadcast_Ids,
        BDSP_Img_Id_eDtsBroadcast_Ids_Interframe,
        
        BDSP_Img_Id_eDtsHd_Ids,
        BDSP_Img_Id_eDtsHd_Ids_Interframe,
        
        BDSP_Img_Id_eDRA_Ids,
        BDSP_Img_Id_eDRA_Ids_Interframe,
        
        BDSP_Img_Id_eRealAudioLbr_Ids,
        BDSP_Img_Id_eRealAudioLbr_Ids_Interframe,
        
        BDSP_Img_Id_eMpeg1_Decode,
        BDSP_Img_Id_eMpeg1_Decode_Table,
        BDSP_Img_Id_eMpeg1_Interframe,
        
#ifdef BDSP_MS10_SUPPORT
        BDSP_IMG_Id_eDolbyPulse_Decode,
        BDSP_IMG_Id_eDolbyPulse_Decode_Table,
        BDSP_IMG_Id_eDolbyPulse_Interframe,
#else
        BDSP_IMG_Id_eAache_Decode,
        BDSP_IMG_Id_eAache_Decode_Table,
        BDSP_IMG_Id_eAache_Interframe,
#endif

#ifdef BDSP_MS10_SUPPORT
        BDSP_Img_Id_eMsDdp_Decode,
        BDSP_Img_Id_eMsDdp_Decode_Tables,
        BDSP_Img_Id_eMsDdp_Interframe,
#elif  defined BDSP_DDP_SUPPORT
        BDSP_Img_Id_eDdp_Decode,
        BDSP_Img_Id_eDdp_Decode_Table,
        BDSP_Img_Id_eDdp_Interframe,
#else
        BDSP_Img_Id_eAc3_Decode,
        BDSP_Img_Id_eAc3_Decode_Table,
        BDSP_Img_Id_eAc3_Interframe,
#endif

        BDSP_Img_Id_eDts_Ids,
        BDSP_Img_Id_eDts_Ids_Interframe,
        BDSP_Img_Id_eDtsBroadcast_Decode_Stg,
        BDSP_Img_Id_eDtsBroadcast_Decode_Table_Stg,
        BDSP_Img_Id_eDtsBroadcast_Decode_Interframe_Stg,
        
        BDSP_Img_Id_eLpcmBd_Ids,
        BDSP_Img_Id_eLpcmBd_Decode,
        BDSP_Img_Id_eLpcmBd_Decode_Table,
        BDSP_Img_Id_eLpcmBd_Interframe,
        BDSP_Img_Id_eLpcmBd_Ids_Interframe,
        
        BDSP_Img_Id_eLpcmHdDvd_Ids,
        BDSP_Img_Id_eLpcmHdDvd_Decode,
        BDSP_Img_Id_eLpcmHdDvd_Decode_Table,
        BDSP_Img_Id_eLpcmHdDvd_Interframe,
        BDSP_Img_Id_eLpcmHdDvd_Ids_Interframe,
        
        BDSP_Img_Id_eLpcmDvd_Ids,
        BDSP_Img_Id_eLpcmDvd_Decode,
        BDSP_Img_Id_eLpcmDvd_Decode_Table,
        BDSP_Img_Id_eLpcmDvd_Interframe,
        BDSP_Img_Id_eLpcmDvd_Ids_Interframe,

        BDSP_Img_Id_eWma_Ids,
        BDSP_Img_Id_eWma_Decode,
        BDSP_Img_Id_eWma_Decode_Table,
        BDSP_Img_Id_eWma_Interframe,
        BDSP_Img_Id_eWma_Ids_Interframe,

        BDSP_Img_Id_eTrueHD_Decode,
        BDSP_Img_Id_eTrueHD_Decode_Table,
        BDSP_Img_Id_eTrueHD_Interframe,

        BDSP_Img_Id_eMlp_Ids,
        BDSP_Img_Id_eMlp_Ids_Interframe,

        BDSP_Img_Id_ePcm_Ids,
        BDSP_Img_Id_ePcm_Interframe,

        BDSP_Img_Id_eDtsLbr_Ids,
        BDSP_Img_Id_eDtsLbr_Decode,
        BDSP_Img_Id_eDtsLbr_Decode_Table,
        BDSP_Img_Id_eDtsLbr_Interframe,
        BDSP_Img_Id_eDtsLbr_Ids_Interframe,

        BDSP_Img_Id_eDdp71_Ids,
        BDSP_Img_Id_eDdp71_Decode,
        BDSP_Img_Id_eDdp71_Decode_Table,
        BDSP_Img_Id_eDdp71_Interframe,
        BDSP_Img_Id_eDdp71_Ids_Interframe,

        BDSP_Img_Id_eMpegMc_Ids,
        BDSP_Img_Id_eMpegMc_Decode,
        BDSP_Img_Id_eMpegMc_Decode_Table,
        BDSP_Img_Id_eMpegMc_Interframe,
        BDSP_Img_Id_eMpegMc_Ids_Interframe,

        BDSP_Img_Id_eWmaPro_Ids,
        BDSP_Img_Id_eWmaPro_Decode,
        BDSP_Img_Id_eWmaPro_Decode_Table,
        BDSP_Img_Id_eWmaPro_Ids_Interframe,
        BDSP_Img_Id_eWmaPro_Interframe,

        BDSP_Img_Id_eDtsNeo_Code,
        BDSP_Img_Id_eDtsNeo_Table,
        BDSP_Img_Id_eDtsNeo_Interframe,

        BDSP_Img_Id_eAvl_Code,
        BDSP_Img_Id_eAvl_Table,
        BDSP_Img_Id_eAvl_Interframe,

        BDSP_Img_Id_ePL2_Code,
        BDSP_Img_Id_ePL2_Table,
        BDSP_Img_Id_ePL2_Interframe,

        BDSP_Img_Id_eSrsXt_Code,
        BDSP_Img_Id_eSrsXt_Table,
        BDSP_Img_Id_eSrsXt_Interframe,

        BDSP_Img_Id_eSrsHd_Code,
        BDSP_Img_Id_eSrsHd_Table,
        BDSP_Img_Id_eSrsHd_Interframe,

        BDSP_Img_Id_eSrsTruVolume_Code,
        BDSP_Img_Id_eSrsTruVolume_Table,
        BDSP_Img_Id_eSrsTruVolume_Interframe,

        BDSP_Img_Id_eBbe_Code,
        BDSP_Img_Id_eBbe_Table,
        BDSP_Img_Id_eBbe_Interframe,

        BDSP_Img_Id_eAacDownmix_Code,
        BDSP_Img_Id_eAacDownmix_Table,
        BDSP_Img_Id_eAacDownmix_Interframe,

        BDSP_Img_Id_eDts_Encode_Code,
        BDSP_Img_Id_eDts_Encode_Table,
        BDSP_Img_Id_eDts_Encode_Interframe,

#ifdef BDSP_MS10_SUPPORT
        BDSP_Img_Id_eDolbyTranscode_Code,
        BDSP_Img_Id_eDolbyTranscode_Table,
        BDSP_Img_Id_eDolbyTranscode_Interframe,
#else
        BDSP_Img_Id_eAc3_Encode_Code,
        BDSP_Img_Id_eAc3_Encode_Table,
        BDSP_Img_Id_eAc3_Encode_Interframe,
#endif

        BDSP_Img_Id_eCustomVoice_Code,
        BDSP_Img_Id_eCustomVoice_Tables,
        BDSP_Img_Id_eCustomVoice_Interframe,

        BDSP_Img_Id_eSrc_Code,
        BDSP_Img_Id_eSrc_Tables,
        BDSP_Img_Id_eSrc_Interframe,

        BDSP_Img_Id_eRfAudio_Btsc,
        BDSP_Img_Id_eRfAudio_Eiaj,
        BDSP_Img_Id_eRfAudio_KoreaA2,
        BDSP_Img_Id_eRfAudio_Nicam,
        BDSP_Img_Id_eRfAudio_PalA2,
        BDSP_Img_Id_eRfAudio_SecamL,
        BDSP_Img_Id_eRfAudio_India,

        BDSP_Img_Id_eDsola_Code,
        BDSP_Img_Id_eDsola_Tables,
        BDSP_Img_Id_eDsola_Interframe,

        BDSP_Img_Id_eDolbyVol_Code,
        BDSP_Img_Id_eDolbyVol_Tables,
        BDSP_Img_Id_eDolbyVol_Interframe,

        BDSP_Img_Id_ePcmWav_Code,
        BDSP_Img_Id_ePcmWav_Code_Interframe,
        BDSP_Img_Id_ePcmWav_Ids,
        BDSP_Img_Id_ePcmWav_Ids_Interframe,

        BDSP_Img_Id_eMp3Enc_Code,
        BDSP_Img_Id_eMp3Enc_Code_Tables,
        BDSP_Img_Id_eMp3Enc_Code_Interframe,

        BDSP_Img_Id_eAMR_Ids,
        BDSP_Img_Id_eAMR_Decode,
        BDSP_Img_Id_eAMR_Decode_Table,
        BDSP_Img_Id_eAMR_Interframe,
        BDSP_Img_Id_eAMR_Ids_Interframe,

        BDSP_Img_Id_eDRA_Decode,
        BDSP_Img_Id_eDRA_Decode_Table,
        BDSP_Img_Id_eDRA_Interframe,

        BDSP_Img_Id_eSbc_Encode_Code,
        BDSP_Img_Id_eSbc_Encode_Table,
        BDSP_Img_Id_eSbc_Encode_Interframe,

        BDSP_Img_Id_eRealAudioLbr_Decode_Code,
        BDSP_Img_Id_eRealAudioLbr_Decode_Table,
        BDSP_Img_Id_eRealAudioLbr_Decode_Interframe,

        BDSP_Img_Id_eBrcm3DSurround_Code,
        BDSP_Img_Id_eBrcm3DSurround_Table,
        BDSP_Img_Id_eBrcm3DSurround_Interframe,

        BDSP_Img_Id_eMixer_Ids,
        BDSP_Img_Id_eFwMixer_Code,
        BDSP_Img_Id_eFwMixer_Table,
        BDSP_Img_Id_eMixer_Ids_Interframe,
        BDSP_Img_Id_eFwMixer_Inter_frame,    

        BDSP_Img_Id_eAvd_Code,
        BDSP_Img_Id_eAvd_Table,
        BDSP_Img_Id_eAvd_Interframe, 

        BDSP_Img_Id_eAbx_Code,
        BDSP_Img_Id_eAbx_Table,
        BDSP_Img_Id_eAbx_Interframe, 

        BDSP_Img_Id_eSrsCsTd_Code,
        BDSP_Img_Id_eSrsCsTd_Table,
        BDSP_Img_Id_eSrsCsTd_Interframe,

        BDSP_Img_Id_eSrsEqHl_Code,
        BDSP_Img_Id_eSrsEqHl_Table,
        BDSP_Img_Id_eSrsEqHl_Interframe,

        BDSP_Img_Id_eCustomDbe_Code,
        BDSP_Img_Id_eCustomDbe_Table,
        BDSP_Img_Id_eCustomDbe_Interframe,

        BDSP_Img_Id_eCustomAvlp_Code,
        BDSP_Img_Id_eCustomAvlp_Table,
        BDSP_Img_Id_eCustomAvlp_Interframe,

        BDSP_Img_Id_eCustomAcf_Code,
        BDSP_Img_Id_eCustomAcf_Table,
        BDSP_Img_Id_eCustomAcf_Interframe,

        BDSP_Img_Id_eAdpcm_Ids,
        BDSP_Img_Id_eAdpcm_Decode,
        BDSP_Img_Id_eAdpcm_Decode_Table,
        BDSP_Img_Id_eAdpcm_Ids_Interframe,
        BDSP_Img_Id_eAdpcm_Interframe,

        BDSP_Img_Id_eGenItbCdb_Code,
        BDSP_Img_Id_eGenItbCdb_Table,
        BDSP_Img_Id_eGenItbCdb_Interframe,

        BDSP_Img_Id_eBtscEncoder_Code,
        BDSP_Img_Id_eBtscEncoder_Table,
        BDSP_Img_Id_eBtscEncoder_Interframe,

        BDSP_Img_Id_eAacHe_Encode_Code,
        BDSP_Img_Id_eAacHe_Encode_Table,
        BDSP_Img_Id_eAacHe_Encode_Interframe,

        BDSP_Img_Id_eDV258_Code,
        BDSP_Img_Id_eDV258_Table,
        BDSP_Img_Id_eDV258_Interframe,

        BDSP_Img_Id_eDDRE_Code,
        BDSP_Img_Id_eDDRE_Table,
        BDSP_Img_Id_eDDRE_Interframe,

        BDSP_Img_Id_eVP6_Ids,
        BDSP_Img_Id_eVP6_Decode,
        BDSP_Img_Id_eVP6_Decode_Table,
        BDSP_Img_Id_eVP6_Ids_Interframe,
        BDSP_Img_Id_eVP6_Decode_Interframe,
        
        BDSP_Img_Id_eG711G726_Ids,
        BDSP_Img_Id_eG711G726_Decode,
        BDSP_Img_Id_eG711G726_Decode_Table,
        BDSP_Img_Id_eG711G726_Ids_Interframe,
        BDSP_Img_Id_eG711G726_Interframe,

        BDSP_Img_Id_eG729_Ids,
        BDSP_Img_Id_eG729_Decode,
        BDSP_Img_Id_eG729_Decode_Table,
        BDSP_Img_Id_eG729_Ids_Interframe,
        BDSP_Img_Id_eG729_Interframe,

        BDSP_Img_Id_eVorbis_Ids,
        BDSP_Img_Id_eVorbis_Decode,
        BDSP_Img_Id_eVorbis_Decode_Table,
        BDSP_Img_Id_eVorbis_Ids_Interframe,
        BDSP_Img_Id_eVorbis_Interframe,

        BDSP_Img_Id_eFlac_Ids,
        BDSP_Img_Id_eFlac_Decode,
        BDSP_Img_Id_eFlac_Decode_Table,
        BDSP_Img_Id_eFlac_Ids_Interframe,
        BDSP_Img_Id_eFlac_Interframe,

        BDSP_Img_Id_eMac_Ids,
        BDSP_Img_Id_eMac_Decode,
        BDSP_Img_Id_eMac_Decode_Table,
        BDSP_Img_Id_eMac_Ids_Interframe,
        BDSP_Img_Id_eMac_Interframe,

        BDSP_Img_Id_eAMRWB_Ids,
        BDSP_Img_Id_eAMRWB_Decode,
        BDSP_Img_Id_eAMRWB_Decode_Table,
        BDSP_Img_Id_eAMRWB_Interframe,
        BDSP_Img_Id_eAMRWB_Ids_Interframe,

        BDSP_Img_Id_eiLBC_Ids,
        BDSP_Img_Id_eiLBC_Decode,
        BDSP_Img_Id_eiLBC_Decode_Table,
        BDSP_Img_Id_eiLBC_Interframe,
        BDSP_Img_Id_eiLBC_Ids_Interframe,
			
		BDSP_Img_Id_eiSAC_Ids,
        BDSP_Img_Id_eiSAC_Decode,
        BDSP_Img_Id_eiSAC_Decode_Table,
        BDSP_Img_Id_eiSAC_Interframe,
        BDSP_Img_Id_eiSAC_Ids_Interframe,


        BDSP_Img_Id_eG711G726_Encode_Code,
        BDSP_Img_Id_eG711G726_Encode_Table,
        BDSP_Img_Id_eG711G726_Encode_Interframe,

        BDSP_Img_Id_eG729_Encode_Code,
        BDSP_Img_Id_eG729_Encode_Table,
        BDSP_Img_Id_eG729_Encode_Interframe,

        BDSP_Img_Id_eWma_Encode_Code,
        BDSP_Img_Id_eWma_Encode_Table,
        BDSP_Img_Id_eWma_Encode_Interframe,

        BDSP_Img_Id_eG723_1_Ids,
        BDSP_Img_Id_eG723_1_Decode,
        BDSP_Img_Id_eG723_1_Decode_Table,
        BDSP_Img_Id_eG723_1_Ids_Interframe,
        BDSP_Img_Id_eG723_1_Interframe,
        
        BDSP_Img_Id_eG723_1_Encode_Code,
        BDSP_Img_Id_eG723_1_Encode_Table,
        BDSP_Img_Id_eG723_1_Encode_Interframe,
        
        BDSP_Img_Id_eSpeexAec_Code,
        BDSP_Img_Id_eSpeexAec_Table,
        BDSP_Img_Id_eSpeexAec_Interframe,

        BDSP_Img_Id_eH264Encode,
        BDSP_Img_Id_eH264Encode_Table,
        BDSP_Img_Id_eH264Encode_Interframe,
		
		BDSP_Img_Id_eVP8Encode,
		BDSP_Img_Id_eVP8Encode_Table,
		BDSP_Img_Id_eVP8Encode_Interframe,

		BDSP_Img_Id_eMLP_Passthrough_Code,
        BDSP_Img_Id_eMLP_Passthrough_Table,
        BDSP_Img_Id_eMLP_Passthrough_Interframe,

        BDSP_Img_Id_eG722_Encode_Code,
        BDSP_Img_Id_eG722_Encode_Table,
        BDSP_Img_Id_eG722_Encode_Interframe,
        
        BDSP_Img_Id_eAmr_Encode_Code,
        BDSP_Img_Id_eAmr_Encode_Table,
        BDSP_Img_Id_eAmr_Encode_Interframe,

        BDSP_Img_Id_eAmrwb_Encode_Code,
        BDSP_Img_Id_eAmrwb_Encode_Table,
        BDSP_Img_Id_eAmrwb_Encode_Interframe,

		BDSP_Img_Id_eScm1_Code,
		BDSP_Img_Id_eScm1_Table,
		BDSP_Img_Id_eScm1_Interframe,
		
		BDSP_Img_Id_eScm2_Code,
		BDSP_Img_Id_eScm2_Table,
		BDSP_Img_Id_eScm2_Interframe,
		
		BDSP_Img_Id_eScm3_Code,
		BDSP_Img_Id_eScm3_Table,
		BDSP_Img_Id_eScm3_Interframe,

		BDSP_Img_Id_eiLBC_Encode_Code,
        BDSP_Img_Id_eiLBC_Encode_Table,
        BDSP_Img_Id_eiLBC_Encode_Interframe,
		
		BDSP_Img_Id_eiSAC_Encode_Code,
        BDSP_Img_Id_eiSAC_Encode_Table,
        BDSP_Img_Id_eiSAC_Encode_Interframe,
		
        BDSP_Img_Id_eInvalid
}BDSP_Img_Id;



/* This chunk size will be used when the firmware binary is actually present in 
    multiple chunks. The BDSP_IMG_CHUNK_SIZE will then give the size of each 
    such chunk
*/
#define BDSP_IMG_CHUNK_SIZE	65532


extern void *BDSP_IMG_Context;
extern const BIMG_Interface BDSP_IMG_Interface;

#endif
