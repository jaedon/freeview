/***************************************************************************
 *     Copyright (c) 2006-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdsp_raaga_img.c $
 * $brcm_Revision: Hydra_Software_Devel/30 $
 * $brcm_Date: 1/29/13 5:50p $
 *
 * Module Description: Host DSP Task Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/dsp/src/raaga/firmware/fp2008/bdsp_raaga_img.c $
 * 
 * Hydra_Software_Devel/30   1/29/13 5:50p sriniva
 * FWRAA-559: Renaming dts lbr braaga files to dts_express
 * 
 * Hydra_Software_Devel/29   1/10/13 11:27a skalapur
 * FWRAA-474:iLBC code merging to main latest
 * 
 * Hydra_Software_Devel/28   1/9/13 11:47p ananthan
 * FWRAA-535: Removing duplicate entries for SCM task in image interface
 * structure
 * 
 * Hydra_Software_Devel/27   11/27/12 7:24p ananthan
 * SW7425-4242: Add SCM3 interface and merge SCM to mainline.
 * 
 * Hydra_Software_Devel/23   8/30/12 12:50p shivak
 * FWRAA-424 : Fix for non-VP8 build
 * 
 * Hydra_Software_Devel/22   8/27/12 5:44p shivak
 * FWRAA-424 : added VP8 support
 * 
 * Hydra_Software_Devel/20   7/17/12 2:13p dlimaye
 * SW7425-2805: Add AMR-NB and AMR-WB codecs support Added new codecs: AMR
 * and AMRWB decoder/encoder
 * 
 * Hydra_Software_Devel/19   6/19/12 3:17a gprasad
 * SW7425-2952: DSP directory structure flattening
 * 
 * Hydra_Software_Devel/18   6/6/12 1:01p ananthan
 * FWRAA-443: Add MAT Encoder support in Raaga
 * 
 * Hydra_Software_Devel/17   5/11/12 3:32p dlimaye
 * FWRAA-433: Adding G722 encoder to raaga Adding bdsp changes and
 * sys_top_raaga changes
 * 
 * Hydra_Software_Devel/16   3/30/12 12:32a gprasad
 * SW7425-2786: BDSP changes for adding MLP Passthru support
 * 
 * Hydra_Software_Devel/15   3/22/12 6:50p gprasad
 * FWRAA-399: DTS LBR SUPPORT
 * 
 * Hydra_Software_Devel/14   3/16/12 5:08p ramanatm
 * SW7425-2378:[7425] Add support for Monkey Audio Codec (MAC)
 * 
 * Hydra_Software_Devel/13   2/7/12 4:27p rshijith
 * FWRAA-377: Merging soft raaga video encode to mainline.
 * 
 * Hydra_Software_Devel/12   2/6/12 10:24p ashoky
 * FWRAA-377: Merging raaga soft video encoder to mainline.
 * 
 * Hydra_Software_Devel/11   2/3/12 12:25p ramanatm
 * SW7425-1752:[7425] Add support for FLAC media file format
 * 
 * Hydra_Software_Devel/10   1/13/12 9:52p gprasad
 * FWRAA-342: Adding support for SPEEX AEC
 * 
 * Hydra_Software_Devel/9   1/12/12 6:16p gprasad
 * FWRAA-370: Fixing build errors observed when BDSP_MS10_SUPPORT=n and
 * BDSP_AC3ENC_SUPPORT=y
 * 
 * Hydra_Software_Devel/8   12/29/11 5:54p gprasad
 * FWRAA-370: G723.1 codec development on Raaga platform
 * 
 * Hydra_Software_Devel/7   12/5/11 1:57p ashoky
 * FWRAA-330: Adding WMA encoder support.
 * 
 * Hydra_Software_Devel/6   11/8/11 1:40p ramanatm
 * FWRAA-348:[7425] Vorbis bringup
 * 
 * Hydra_Software_Devel/5   8/25/11 1:50p gprasad
 * FWRAA-327: Adding g.7XX fw binaries for B0
 * 
 * Hydra_Software_Devel/4   7/13/11 3:00p gprasad
 * SW7425-858: Runtime errors with BDSP_3RDPARTY_ALGO_SUPPORT=n - code
 * cleanup
 * 
 * Hydra_Software_Devel/3   5/19/11 4:18p ashoky
 * SW7425-477: [7425]Including mp3 encoder source for build.
 * 
 * Hydra_Software_Devel/2   5/19/11 12:02p gprasad
 * SW7425-572: [7425] Adding VP6 support to basemodules
 * 
 * Hydra_Software_Devel/1   4/14/11 11:25a gprasad
 * FWRAA-280: Adding support for fp-2011
 * 
 * Hydra_Software_Devel/1   4/5/11 4:07p gprasad
 * SW7425-286: [7425] Checkin raaga binaries to magnum into basemodules
 * folder
 * 
 * Hydra_Software_Devel/5   3/5/11 5:04a srajapur
 * SW7422-247 : [7422] Adding GenCdbItb and AACHE encode audio processing
 * support.  SW7422-314 : [7422] Adding RDB Independence Feature in BASE
 * DSP Codebase
 * 
 * Hydra_Software_Devel/4   2/16/11 5:40a srajapur
 * SW7422-247 : [7422] Adding all audio post processing algorithms.
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

#include "bdsp.h"
#include "../src/bdsp_priv.h" /* FIXME: Remove the _priv dependency */
#include "bdsp_raaga_img.h"

BDBG_MODULE(bdsp_raaga_img);


extern const void * BDSP_IMG_system_data [];
extern const void * BDSP_IMG_system_code [];
extern const void * BDSP_IMG_system_rdbvars [];
extern const void * BDSP_IMG_algolib_code[];
extern const void * BDSP_IMG_syslib_code [];
extern const void * BDSP_IMG_idscommon_code [];
extern const void * BDSP_IMG_vididscommon_code[];
extern const void * BDSP_IMG_vididscommon_inter_frame[];
extern const void * BDSP_IMG_scm_task_code [];
extern const void * BDSP_IMG_scm1_digest [];
extern const void * BDSP_IMG_scm2_digest [];
extern const void *	BDSP_IMG_cdb_passthru_code[];
extern const void *	BDSP_IMG_cdb_passthru_tables[];
extern const void * BDSP_IMG_cdb_passthru_inter_frame[];

/* Passthru AC3/MPEG/DDP/DTS/AAC even without license */
#ifdef  BDSP_AC3_PASSTHRU_SUPPORT         
extern const void * BDSP_IMG_ac3_ids [];
extern const void * BDSP_IMG_ac3_ids_inter_frame [];
#endif

#ifdef  BDSP_MPEG_PASSTHRU_SUPPORT         
extern const void * BDSP_IMG_mpeg1_ids [];
extern const void * BDSP_IMG_mpeg1_ids_inter_frame [];
#endif

#ifdef  BDSP_AACSBR_PASSTHRU_SUPPORT         
extern const void * BDSP_IMG_aacheadts_ids [];
extern const void * BDSP_IMG_aacheloas_ids [];
extern const void * BDSP_IMG_aacheadts_ids_inter_frame [];
extern const void * BDSP_IMG_aacheloas_ids_inter_frame [];
#endif

#ifdef  BDSP_DDP_PASSTHRU_SUPPORT         
extern const void * BDSP_IMG_ddp_ids [];
extern const void * BDSP_IMG_ddp_ids_inter_frame [];
#endif

#ifdef  BDSP_DTSBROADCAST_PASSTHRU_SUPPORT         
extern const void * BDSP_IMG_dts_ids [];
extern const void * BDSP_IMG_dts_ids_inter_frame [];
#endif

#ifdef  BDSP_DTSHD_PASSTHRU_SUPPORT
extern const void * BDSP_IMG_dtshd_ids [];
extern const void * BDSP_IMG_dtshd_ids_inter_frame [];
#endif

#ifdef  BDSP_DRA_PASSTHRU_SUPPORT
extern const void * BDSP_IMG_dra_ids[];
extern const void * BDSP_IMG_dra_ids_inter_frame[];
#endif

#ifdef  BDSP_REALAUDIOLBR_SUPPORT
extern const void * BDSP_IMG_ralbr_ids [];
extern const void * BDSP_IMG_ralbr_ids_inter_frame [];
#endif

#ifdef BDSP_MPEG_SUPPORT
extern const void * BDSP_IMG_mpeg1_decode [];
extern const void * BDSP_IMG_mpeg1_decode_tables [];
extern const void * BDSP_IMG_mpeg1_decode_inter_frame [];
#endif

#ifdef BDSP_AAC_SUPPORT
#endif

#ifdef BDSP_MS10_SUPPORT
extern const void * BDSP_IMG_dolby_pulse_decode [];
extern const void * BDSP_IMG_dolby_pulse_decode_tables [];
extern const void * BDSP_IMG_dolby_pulse_decode_inter_frame [];
#elif  BDSP_AACSBR_SUPPORT
extern const void * BDSP_IMG_aache_decode [];
extern const void * BDSP_IMG_aache_decode_tables [];
extern const void * BDSP_IMG_aache_decode_inter_frame [];
#endif

#ifdef BDSP_MS10_SUPPORT
extern const void * BDSP_IMG_dolby_ms_ddp_decode [];
extern const void * BDSP_IMG_dolby_ms_ddp_decode_tables [];
extern const void * BDSP_IMG_dolby_ms_ddp_decode_inter_frame [];
#elif  BDSP_DDP_SUPPORT
extern const void * BDSP_IMG_ddp_decode [];
extern const void * BDSP_IMG_ddp_decode_tables [];
extern const void * BDSP_IMG_ddp_decode_inter_frame [];
#elif  BDSP_AC3_SUPPORT
extern const void * BDSP_IMG_ac3_decode [];
extern const void * BDSP_IMG_ac3_decode_tables [];
extern const void * BDSP_IMG_ac3_decode_inter_frame [];
#endif


#ifdef BDSP_DTS_SUPPORT
extern const void * BDSP_IMG_dts_ids [];
extern const void * BDSP_IMG_dts_ids_inter_frame [];
#endif

#if defined(BDSP_DTSBROADCAST_SUPPORT) || defined(BDSP_DTSHD_SUPPORT)
extern const void * BDSP_IMG_dtshd_decode [];
extern const void * BDSP_IMG_dtshd_decode_tables [];
extern const void * BDSP_IMG_dtshd_decode_inter_frame [];
#endif

#ifdef BDSP_LPCMBD_SUPPORT
#endif

#ifdef BDSP_LPCMHDDVD_SUPPORT
#endif

#ifdef BDSP_LPCMDVD_SUPPORT
extern const void *   BDSP_IMG_dvdlpcm_ids [];
extern const void *   BDSP_IMG_lpcm_decode_code [];
extern const void *   BDSP_IMG_lpcm_decode_tables [];
extern const void *   BDSP_IMG_lpcm_decode_inter_frame [];
extern const void *   BDSP_IMG_dvdlpcm_ids_inter_frame [];
#endif

#ifdef BDSP_WMA_SUPPORT
extern const void *  BDSP_IMG_wma_ids [];
extern const void *  BDSP_IMG_wma_decode [];
extern const void *  BDSP_IMG_wma_decode_tables [];
extern const void *  BDSP_IMG_wma_decode_inter_frame [];
extern const void *  BDSP_IMG_wma_ids_inter_frame [];
#endif

#ifdef BDSP_AC3LOSSLESS_SUPPORT
#endif

#if defined (BDSP_MLP_SUPPORT) || defined (BDSP_MLP_PASSTHROUGH_SUPPORT)
extern const void *  BDSP_IMG_mlp_ids [];
extern const void *  BDSP_IMG_mlp_ids_inter_frame [];
#endif
 
#ifdef BDSP_MLP_SUPPORT
extern const void *  BDSP_IMG_truehd_decode [];
extern const void *  BDSP_IMG_truehd_decode_tables [];
extern const void *  BDSP_IMG_truehd_decode_inter_frame [];
#endif

#ifdef BDSP_PCM_SUPPORT
extern const void *  BDSP_IMG_pcm_ids [];
extern const void *  BDSP_IMG_pcm_ids_inter_frame [];
#endif

#ifdef BDSP_DTSLBR_SUPPORT
extern const void *        BDSP_IMG_dts_express_ids_decode [];
extern const void *        BDSP_IMG_dts_express_decode [];
extern const void *		   BDSP_IMG_dts_express_decode_tables [];
extern const void *        BDSP_IMG_dts_express_decode_inter_frame [];
extern const void *        BDSP_IMG_dts_express_ids_inter_frame[];
#endif

#ifdef BDSP_DDP7_1_SUPPORT
#endif

#ifdef BDSP_MPEGMC_SUPPORT
#endif

#ifdef BDSP_WMAPRO_SUPPORT
extern const void *  BDSP_IMG_wma_pro_ids [];
extern const void *  BDSP_IMG_wma_pro_decode [];
extern const void *  BDSP_IMG_wma_pro_decode_tables [];
extern const void *  BDSP_IMG_wma_pro_ids_inter_frame []; 
extern const void *  BDSP_IMG_wma_pro_decode_inter_frame []; 
#endif

#ifdef BDSP_DDP_TO_AC3_SUPPORT
extern const void * BDSP_IMG_ddp_convert [];
extern const void * BDSP_IMG_ddp_convert_tables [];
extern const void * BDSP_IMG_ddp_convert_inter_frame [];
#endif

#ifdef BDSP_DDBM_SUPPORT
extern const void * BDSP_IMG_ddbm_postprocess[];
extern const void * BDSP_IMG_ddbm_postprocess_table[];
#endif

#ifdef BDSP_DTSNEO_SUPPORT
extern const void * BDSP_IMG_dts_neo_postprocess[];
extern const void * BDSP_IMG_dts_neo_postprocess_table[];
extern const void * BDSP_IMG_dts_neo_interframe_buf[];
#endif

#ifdef BDSP_AVL_SUPPORT
extern const void *BDSP_IMG_brcm_avl_code[];
extern const void *BDSP_IMG_brcm_avl_tables[];
extern const void *BDSP_IMG_brcm_avl_inter_frame[];
#endif

#ifdef BDSP_PL2_SUPPORT
extern const void *BDSP_IMG_prologic2_code[];
extern const void *BDSP_IMG_prologic2_tables[];
extern const void *BDSP_IMG_prologic2_inter_frame[];
#endif

#ifdef BDSP_SRSXT_SUPPORT
extern const void *BDSP_IMG_trusurroundxt[];
extern const void *BDSP_IMG_trusurroundxt_tables[];
extern const void *BDSP_IMG_trusurroundxt_inter_frame[];
#endif

#ifdef BDSP_SRSHD_SUPPORT
extern const void *BDSP_IMG_srs_trusurroundhd_code [];
extern const void *BDSP_IMG_srs_trusurroundhd_tables [];
extern const void *BDSP_IMG_srs_trusurroundhd_inter_frame [];
#endif

#ifdef BDSP_SRSTRUVOL_SUPPORT
extern const void *BDSP_IMG_srs_tvol_code [];
extern const void *BDSP_IMG_srs_tvol_tables [];
extern const void *BDSP_IMG_srs_tvol_inter_frame [];
#endif

#ifdef BDSP_XEN_SUPPORT
extern const void *BDSP_IMG_xen[];
extern const void *BDSP_IMG_xen_table[];
extern const void *BDSP_IMG_xen_interframe_buf[];
#endif

#ifdef BDSP_BBE_SUPPORT
extern const void *BDSP_IMG_bbe[];
extern const void *BDSP_IMG_bbe_table[];
extern const void *BDSP_IMG_bbe_interframe_buf[];
#endif

#ifdef BDSP_CUSTOMSURROUND_SUPPORT
extern const void *BDSP_IMG_customsurround_code[];
extern const void *BDSP_IMG_customsurround_tables[];
extern const void *BDSP_IMG_customsurround_inter_frame[];
#endif

#ifdef BDSP_CUSTOMBASS_SUPPORT
extern const void *BDSP_IMG_custombass_code[];
extern const void *BDSP_IMG_custombass_tables[];
extern const void *BDSP_IMG_custombass_inter_frame[];
#endif

#ifdef BDSP_AACDOWNMIX_SUPPORT
#endif

#ifdef BDSP_DTSENC_SUPPORT
extern const void * BDSP_IMG_dts_encode [];
extern const void * BDSP_IMG_dts_encode_tables [];
extern const void * BDSP_IMG_dts_encode_inter_frame [];
#endif

#ifdef BDSP_MS10_SUPPORT
extern const void * BDSP_IMG_dd_transcode[];
extern const void * BDSP_IMG_dd_transcode_tables [];
extern const void * BDSP_IMG_dd_transcode_inter_frame [];
#else
#ifdef BDSP_AC3ENC_SUPPORT
extern const void * BDSP_IMG_dd_transcode[];
extern const void * BDSP_IMG_dd_transcode_tables [];
extern const void * BDSP_IMG_dd_transcode_inter_frame [];
#endif
#endif

#ifdef BDSP_CUSTOMVOICE_SUPPORT
extern const void * BDSP_IMG_customvoice_code [];
extern const void * BDSP_IMG_customvoice_tables [];
extern const void * BDSP_IMG_customvoice_inter_frame [];
#endif

#ifdef BDSP_SRC_SUPPORT
extern const void * BDSP_IMG_src_code [];
extern const void * BDSP_IMG_src_tables [];
extern const void * BDSP_IMG_src_inter_frame [];
#endif

#ifdef BDSP_RFAUDIO_SUPPORT
extern const void * BDSP_IMG_RfAudioBtsc[];
extern const void * BDSP_IMG_RfAudioEiaj[];
extern const void * BDSP_IMG_RfAudioKoreaa2[];
extern const void * BDSP_IMG_RfAudioNicam[];
extern const void * BDSP_IMG_RfAudioPala2[];
extern const void * BDSP_IMG_RfAudioSecaml[];
extern const void * BDSP_IMG_RfAudioIndia[];
#endif

#ifdef BDSP_AUDIODESC_SUPPORT
extern const void * BDSP_IMG_fade_control[];
extern const void * BDSP_IMG_pan_control[];
extern const void * BDSP_IMG_pan_control_interframe_buf[];
extern const void * BDSP_IMG_pan_control_tables[];
#endif

#ifdef BDSP_WMAPROPASSTHRU_SUPPORT
extern const void *  BDSP_IMG_wmapro_passthru_code[];
#endif
#ifdef BDSP_PCMROUTER_SUPPORT
extern const void * BDSP_IMG_pcm_router[];
#endif
#ifdef BDSP_DSOLA_SUPPORT
extern const void * BDSP_IMG_dsola_code[];
extern const void * BDSP_IMG_dsola_tables[];
extern const void * BDSP_IMG_dsola_inter_frame[];
#endif

#ifdef BDSP_DOLBYVOL_SUPPORT
extern const void * BDSP_IMG_dolby_volume[];
extern const void * BDSP_IMG_dolby_volume_table[];
extern const void * BDSP_IMG_dolby_volume_inter_frame[];
#endif


#ifdef BDSP_PCMWAV_SUPPORT
extern const void * BDSP_IMG_pcmwav_decode[];
extern const void * BDSP_IMG_pcmwav_decode_inter_frame[];
extern const void * BDSP_IMG_pcmwav_ids[];
extern const void * BDSP_IMG_pcmwav_ids_inter_frame[];
#endif


#ifdef BDSP_MP3ENC_SUPPORT
extern const void * BDSP_IMG_mp3_encode[];
extern const void * BDSP_IMG_mp3_encode_tables[];
extern const void * BDSP_IMG_mp3_encode_inter_frame[];
#endif

#ifdef BDSP_AMR_SUPPORT
extern const void * BDSP_IMG_amr_ids[];
extern const void * BDSP_IMG_amr_decode[];
extern const void * BDSP_IMG_amr_decode_tables[];
extern const void * BDSP_IMG_amr_decode_inter_frame[];
extern const void * BDSP_IMG_amr_ids_inter_frame[];
#endif

#ifdef BDSP_AMRWB_SUPPORT
extern const void * BDSP_IMG_amrwb_ids[];
extern const void * BDSP_IMG_amrwb_decode[];
extern const void * BDSP_IMG_amrwb_decode_tables[];
extern const void * BDSP_IMG_amrwb_decode_inter_frame[];
extern const void * BDSP_IMG_amrwb_ids_inter_frame[];
#endif

#ifdef BDSP_ILBC_SUPPORT
extern const void * BDSP_IMG_ilbc_ids[];
extern const void * BDSP_IMG_ilbc_decode[];
extern const void * BDSP_IMG_ilbc_decode_tables[];
extern const void * BDSP_IMG_ilbc_decode_inter_frame[];
extern const void * BDSP_IMG_ilbc_ids_inter_frame[];
#endif

#ifdef BDSP_ISAC_SUPPORT
extern const void * BDSP_IMG_isac_ids[];
extern const void * BDSP_IMG_isac_decode[];
extern const void * BDSP_IMG_isac_decode_tables[];
extern const void * BDSP_IMG_isac_decode_inter_frame[];
extern const void * BDSP_IMG_isac_ids_inter_frame[];
#endif



#ifdef BDSP_DRA_SUPPORT
extern const void * BDSP_IMG_dra_decode[];
extern const void * BDSP_IMG_dra_decode_tables[];
extern const void * BDSP_IMG_dra_decode_inter_frame[];
#endif

#ifdef BDSP_SBCENC_SUPPORT
extern const void * BDSP_IMG_sbc_encode [];
extern const void * BDSP_IMG_sbc_encode_table [];
extern const void * BDSP_IMG_sbc_encode_inter_frame [];
#endif

#ifdef BDSP_REALAUDIOLBR_SUPPORT
extern const void * BDSP_IMG_ralbr_decode [];
extern const void * BDSP_IMG_ralbr_decode_tables [];
extern const void * BDSP_IMG_ralbr_decode_inter_frame [];
#endif

#ifdef BDSP_BRCM3DSURROUND_SUPPORT
extern const void * BDSP_IMG_brcm_3dsurround_code [];
extern const void * BDSP_IMG_brcm_3dsurround_tables [];
extern const void * BDSP_IMG_brcm_3dsurround_inter_frame [];
#endif

#ifdef BDSP_MONODOWNMIX_SUPPORT
extern const void * BDSP_IMG_monodownmix [];
#endif

#ifdef BDSP_FWMIXER_SUPPORT
extern const void * BDSP_IMG_mixer_ids[];
extern const void * BDSP_IMG_fw_mixer_code [];
extern const void * BDSP_IMG_fw_mixer_tables [];
extern const void * BDSP_IMG_fw_mixer_inter_frame [];    
extern const void * BDSP_IMG_mixer_ids_inter_frame[];
#endif

#ifdef BDSP_ADV_SUPPORT
extern const void * BDSP_IMG_adv_code [];
extern const void * BDSP_IMG_adv_tables [];
extern const void * BDSP_IMG_adv_inter_frame [];    
#endif

#ifdef BDSP_ABX_SUPPORT
extern const void * BDSP_IMG_abx_code [];
extern const void * BDSP_IMG_abx_tables [];
extern const void * BDSP_IMG_abx_inter_frame [];    
#endif

#ifdef BDSP_SRSCSTD_SUPPORT
extern const void * BDSP_IMG_srs_csdec_trudialog_code [];
extern const void * BDSP_IMG_srs_csdec_trudialog_tables [];
extern const void * BDSP_IMG_srs_csdec_trudialog_inter_frame [];    
#endif

#ifdef BDSP_SRSEQHL_SUPPORT
extern const void * BDSP_IMG_srs_equ_hl_code [];
extern const void * BDSP_IMG_srs_equ_hl_tables [];
extern const void * BDSP_IMG_srs_equ_hl_inter_frame [];    
#endif

#ifdef BDSP_CUSTOMDBE_SUPPORT
extern const void * BDSP_IMG_custom_dbe_code [];
extern const void * BDSP_IMG_custom_dbe_tables [];
extern const void * BDSP_IMG_custom_dbe_inter_frame [];    
#endif

#ifdef BDSP_CUSTOMAVLP_SUPPORT
extern const void * BDSP_IMG_custom_avlp_code [];
extern const void * BDSP_IMG_custom_avlp_tables [];
extern const void * BDSP_IMG_custom_avlp_inter_frame [];    
#endif

#ifdef BDSP_CUSTOMACF_SUPPORT
extern const void * BDSP_IMG_custom_acf_code [];
extern const void * BDSP_IMG_custom_acf_tables [];
extern const void * BDSP_IMG_custom_acf_inter_frame [];    
#endif

#ifdef  BDSP_ADPCM_SUPPORT
extern const void * BDSP_IMG_adpcm_ids[];
extern const void * BDSP_IMG_adpcm_decode [];
extern const void * BDSP_IMG_adpcm_decode_tables [];
extern const void * BDSP_IMG_adpcm_decode_inter_frame [];
extern const void * BDSP_IMG_adpcm_ids_inter_frame[];
#endif

#ifdef BDSP_GENCDBITB_SUPPORT
extern const void * BDSP_IMG_gen_cdbitb_code [];
extern const void * BDSP_IMG_gen_cdbitb_tables [];
extern const void * BDSP_IMG_gen_cdbitb_inter_frame [];    
#endif

#ifdef BDSP_BTSCENC_SUPPORT
extern const void * BDSP_IMG_btscenc_code [];
extern const void * BDSP_IMG_btscenc_tables [];
extern const void * BDSP_IMG_btscenc_inter_frame [];    
#endif


#ifdef BDSP_AACHEENC_SUPPORT
extern const void * BDSP_IMG_aache_encode [];
extern const void * BDSP_IMG_aache_encode_tables [];
extern const void * BDSP_IMG_aache_encode_inter_frame [];
#endif

#ifdef BDSP_DV258_SUPPORT
extern const void *BDSP_IMG_dv258_code [];
extern const void *BDSP_IMG_dv258_tables [];
extern const void *BDSP_IMG_dv258_inter_frame [];
#endif

#ifdef BDSP_DDRE_SUPPORT
extern const void *BDSP_IMG_ddre_code [];
extern const void *BDSP_IMG_ddre_tables [];
extern const void *BDSP_IMG_ddre_inter_frame [];
#endif

#ifdef BDSP_VP6_SUPPORT
extern const void *BDSP_IMG_vp6_ids[];
extern const void *BDSP_IMG_vp6_decode[];
extern const void *BDSP_IMG_vp6_decode_tables[];
extern const void *BDSP_IMG_vp6_ids_inter_frame[];
extern const void *BDSP_IMG_vp6_decode_inter_frame[];
#endif

#ifdef BDSP_G711G726_SUPPORT
extern const void *BDSP_IMG_g711_g726_ids[];
extern const void *BDSP_IMG_g711_g726_decode[];
extern const void *BDSP_IMG_g711_g726_decode_tables[];
extern const void *BDSP_IMG_g711_g726_ids_inter_frame[];
extern const void *BDSP_IMG_g711_g726_decode_inter_frame[];
#endif

#ifdef BDSP_G729_SUPPORT
extern const void *BDSP_IMG_g729_ids[];
extern const void *BDSP_IMG_g729_decode[];
extern const void *BDSP_IMG_g729_decode_tables[];
extern const void *BDSP_IMG_g729_ids_inter_frame[];
extern const void *BDSP_IMG_g729_decode_inter_frame[];
#endif

#ifdef BDSP_G723_1_SUPPORT
extern const void *BDSP_IMG_g723_1_ids[];
extern const void *BDSP_IMG_g723_1_decode[];
extern const void *BDSP_IMG_g723_1_decode_tables[];
extern const void *BDSP_IMG_g723_1_ids_inter_frame[];
extern const void *BDSP_IMG_g723_1_decode_inter_frame[];
#endif

#ifdef BDSP_VORBIS_SUPPORT
extern const void *BDSP_IMG_vorbis_ids[];
extern const void *BDSP_IMG_vorbis_decode[];
extern const void *BDSP_IMG_vorbis_decode_tables[];
extern const void *BDSP_IMG_vorbis_ids_inter_frame[];
extern const void *BDSP_IMG_vorbis_decode_inter_frame[];
#endif

#ifdef BDSP_FLAC_SUPPORT
extern const void *BDSP_IMG_flac_ids[];
extern const void *BDSP_IMG_flac_decode[];
extern const void *BDSP_IMG_flac_decode_tables[];
extern const void *BDSP_IMG_flac_ids_inter_frame[];
extern const void *BDSP_IMG_flac_decode_inter_frame[];
#endif

#ifdef BDSP_MAC_SUPPORT
extern const void *BDSP_IMG_mac_ids[];
extern const void *BDSP_IMG_mac_decode[];
extern const void *BDSP_IMG_mac_decode_tables[];
extern const void *BDSP_IMG_mac_ids_inter_frame[];
extern const void *BDSP_IMG_mac_decode_inter_frame[];
#endif

#ifdef BDSP_G711G726ENC_SUPPORT
extern const void *BDSP_IMG_g711_g726_encode[];
extern const void *BDSP_IMG_g711_g726_encode_tables[];
extern const void *BDSP_IMG_g711_g726_encode_inter_frame[];
#endif

#ifdef BDSP_G729ENC_SUPPORT
extern const void *BDSP_IMG_g729_encode[];
extern const void *BDSP_IMG_g729_encode_tables[];
extern const void *BDSP_IMG_g729_encode_inter_frame[];
#endif

#ifdef BDSP_G723_1ENC_SUPPORT
extern const void *BDSP_IMG_g723_1_encode[];
extern const void *BDSP_IMG_g723_1_encode_tables[];
extern const void *BDSP_IMG_g723_1_encode_inter_frame[];
#endif

#ifdef BDSP_SPEEXAEC_SUPPORT
extern const void *BDSP_IMG_speexaec_code[];
extern const void *BDSP_IMG_speexaec_tables[];
extern const void *BDSP_IMG_speexaec_inter_frame[];
#endif

#ifdef BDSP_WMAENC_SUPPORT
extern const void *BDSP_IMG_wma_encode[];
extern const void *BDSP_IMG_wma_encode_tables[];
extern const void *BDSP_IMG_wma_encode_inter_frame[];
#endif 

#ifdef BDSP_H264_ENCODE_SUPPORT
extern const void *BDSP_IMG_h264_encode[];
extern const void *BDSP_IMG_h264_encode_tables[];
extern const void *BDSP_IMG_h264_encode_inter_frame[];
#endif

#ifdef BDSP_VP8_ENCODE_SUPPORT
extern const void *BDSP_IMG_vp8_encode[];
extern const void *BDSP_IMG_vp8_encode_tables[];
extern const void *BDSP_IMG_vp8_encode_inter_frame[];
#endif

#ifdef BDSP_MLP_PASSTHROUGH_SUPPORT
extern const void *  BDSP_IMG_mlp_passthrough_code [];
extern const void *  BDSP_IMG_mlp_passthrough_tables [];
extern const void *  BDSP_IMG_mlp_passthrough_inter_frame [];
#endif

#ifdef BDSP_G722ENC_SUPPORT
extern const void *BDSP_IMG_g722_encode[];
extern const void *BDSP_IMG_g722_encode_tables[];
extern const void *BDSP_IMG_g722_encode_inter_frame[];
#endif


#ifdef BDSP_AMRENC_SUPPORT
extern const void *BDSP_IMG_amr_encode[];
extern const void *BDSP_IMG_amr_encode_tables[];
extern const void *BDSP_IMG_amr_encode_inter_frame[];
#endif

#ifdef BDSP_AMRWBENC_SUPPORT
extern const void *BDSP_IMG_amrwb_encode[];
extern const void *BDSP_IMG_amrwb_encode_tables[];
extern const void *BDSP_IMG_amrwb_encode_inter_frame[];
#endif

#ifdef BDSP_SCM1_SUPPORT
extern const void *BDSP_IMG_scm1_decode[];
extern const void *BDSP_IMG_scm1_decode_tables[];
extern const void *BDSP_IMG_scm1_decode_inter_frame[];
#endif
#ifdef BDSP_SCM2_SUPPORT
extern const void *BDSP_IMG_scm2_decode[];
extern const void *BDSP_IMG_scm2_decode_tables[];
extern const void *BDSP_IMG_scm2_decode_inter_frame[];
#endif 
#ifdef BDSP_SCM3_SUPPORT
extern const void *BDSP_IMG_scm3_decode[];
extern const void *BDSP_IMG_scm3_decode_tables[];
extern const void *BDSP_IMG_scm3_decode_inter_frame[];
#endif 

#ifdef BDSP_ILBCENC_SUPPORT
extern const void *BDSP_IMG_ilbc_encode[];
extern const void *BDSP_IMG_ilbc_encode_tables[];
extern const void *BDSP_IMG_ilbc_encode_inter_frame[];
#endif

#ifdef BDSP_ISACENC_SUPPORT
extern const void *BDSP_IMG_isac_encode[];
extern const void *BDSP_IMG_isac_encode_tables[];
extern const void *BDSP_IMG_isac_encode_inter_frame[];
#endif
/*
	This array will contain pointers to the arrays that contain that chunk 
	pointers of the firmware binary. Based on the firmware ID one particular 
	firmware Image can be accessed from this array
*/

static const void **BDSP_IMG_FirmwareImages [] = 
{
        BDSP_IMG_system_code 	
        ,BDSP_IMG_system_data
        ,BDSP_IMG_system_rdbvars        
        ,BDSP_IMG_syslib_code
        ,BDSP_IMG_algolib_code        
        ,BDSP_IMG_idscommon_code
        ,BDSP_IMG_vididscommon_code
        ,BDSP_IMG_vididscommon_inter_frame
		,BDSP_IMG_scm_task_code
		,BDSP_IMG_scm1_digest
		,BDSP_IMG_scm2_digest	
        ,BDSP_IMG_cdb_passthru_code
        ,BDSP_IMG_cdb_passthru_tables
        ,BDSP_IMG_cdb_passthru_inter_frame		
#ifdef  BDSP_AC3_PASSTHRU_SUPPORT           		
		,BDSP_IMG_ac3_ids
		,BDSP_IMG_ac3_ids_inter_frame
#else
        ,NULL
        ,NULL
#endif	

#ifdef  BDSP_MPEG_PASSTHRU_SUPPORT           		
        ,BDSP_IMG_mpeg1_ids		
        ,BDSP_IMG_mpeg1_ids_inter_frame		
#else
        ,NULL
        ,NULL
#endif            

#ifdef  BDSP_AACSBR_PASSTHRU_SUPPORT                       
        ,BDSP_IMG_aacheadts_ids        
        ,BDSP_IMG_aacheloas_ids	
        ,BDSP_IMG_aacheadts_ids_inter_frame        
        ,BDSP_IMG_aacheloas_ids_inter_frame         
#else
        ,NULL
        ,NULL
        ,NULL
        ,NULL
#endif            

#ifdef  BDSP_DDP_PASSTHRU_SUPPORT                       
        ,BDSP_IMG_ddp_ids            
        ,BDSP_IMG_ddp_ids_inter_frame   
#else
        ,NULL
        ,NULL
#endif            

#ifdef  BDSP_DTSBROADCAST_PASSTHRU_SUPPORT                       
        ,BDSP_IMG_dts_ids
        ,BDSP_IMG_dts_ids_inter_frame    
#else
        ,NULL
        ,NULL
#endif           
#ifdef  BDSP_DTSHD_PASSTHRU_SUPPORT       
        ,BDSP_IMG_dtshd_ids
        ,BDSP_IMG_dtshd_ids_inter_frame
#else
        ,NULL
        ,NULL
#endif

#ifdef  BDSP_DRA_PASSTHRU_SUPPORT            
        ,BDSP_IMG_dra_ids            
        ,BDSP_IMG_dra_ids_inter_frame                        
#else
        ,NULL
        ,NULL
#endif

#ifdef  BDSP_REALAUDIOLBR_SUPPORT            
        ,BDSP_IMG_ralbr_ids
        ,BDSP_IMG_ralbr_ids_inter_frame
#else
        ,NULL
        ,NULL
#endif

#ifdef BDSP_MPEG_SUPPORT
        ,BDSP_IMG_mpeg1_decode
        ,BDSP_IMG_mpeg1_decode_tables 
        ,BDSP_IMG_mpeg1_decode_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef  BDSP_MS10_SUPPORT
        ,BDSP_IMG_dolby_pulse_decode
        ,BDSP_IMG_dolby_pulse_decode_tables
        ,BDSP_IMG_dolby_pulse_decode_inter_frame
#else
#ifdef BDSP_AACSBR_SUPPORT
        ,BDSP_IMG_aache_decode
        ,BDSP_IMG_aache_decode_tables
        ,BDSP_IMG_aache_decode_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
#endif
#endif

#ifdef BDSP_MS10_SUPPORT
		,BDSP_IMG_dolby_ms_ddp_decode
		,BDSP_IMG_dolby_ms_ddp_decode_tables	
		,BDSP_IMG_dolby_ms_ddp_decode_inter_frame 
#elif  defined BDSP_DDP_SUPPORT
        ,BDSP_IMG_ddp_decode		
        ,BDSP_IMG_ddp_decode_tables			
        ,BDSP_IMG_ddp_decode_inter_frame
#elif  defined BDSP_AC3_SUPPORT
        ,BDSP_IMG_ac3_decode		
        ,BDSP_IMG_ac3_decode_tables			
        ,BDSP_IMG_ac3_decode_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
#endif


#ifdef BDSP_DTS_SUPPORT
        ,BDSP_IMG_dts_ids
        ,BDSP_IMG_dts_ids_inter_frame
#else
        ,NULL
        ,NULL
#endif
#if defined(BDSP_DTSBROADCAST_SUPPORT) || defined(BDSP_DTSHD_SUPPORT)
        ,BDSP_IMG_dtshd_decode
        ,BDSP_IMG_dtshd_decode_tables
        ,BDSP_IMG_dtshd_decode_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_LPCMBD_SUPPORT
        ,NULL
        ,NULL
        ,NULL
        ,NULL
        ,NULL
#else
        ,NULL
        ,NULL
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_LPCMHDDVD_SUPPORT
        ,NULL
        ,NULL
        ,NULL
        ,NULL
        ,NULL
#else
        ,NULL
        ,NULL
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_LPCMDVD_SUPPORT
        ,BDSP_IMG_dvdlpcm_ids
        ,BDSP_IMG_lpcm_decode_code
        ,BDSP_IMG_lpcm_decode_tables
        ,BDSP_IMG_lpcm_decode_inter_frame
        ,BDSP_IMG_dvdlpcm_ids_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_WMA_SUPPORT
        ,BDSP_IMG_wma_ids
        ,BDSP_IMG_wma_decode
        ,BDSP_IMG_wma_decode_tables
        ,BDSP_IMG_wma_decode_inter_frame
        ,BDSP_IMG_wma_ids_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_MLP_SUPPORT
        ,BDSP_IMG_truehd_decode
        ,BDSP_IMG_truehd_decode_tables
        ,BDSP_IMG_truehd_decode_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
#endif

#if defined (BDSP_MLP_SUPPORT) || defined (BDSP_MLP_PASSTHROUGH_SUPPORT)
        ,BDSP_IMG_mlp_ids
        ,BDSP_IMG_mlp_ids_inter_frame
#else
        ,NULL
        ,NULL
#endif

#ifdef BDSP_PCM_SUPPORT
        ,BDSP_IMG_pcm_ids
        ,BDSP_IMG_pcm_ids_inter_frame
#else
        ,NULL
        ,NULL
#endif

#ifdef BDSP_DTSLBR_SUPPORT
	   	,BDSP_IMG_dts_express_ids_decode
		,BDSP_IMG_dts_express_decode
		,BDSP_IMG_dts_express_decode_tables
		,BDSP_IMG_dts_express_decode_inter_frame
        ,BDSP_IMG_dts_express_ids_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_DDP7_1_SUPPORT
        ,NULL
        ,NULL
        ,NULL
        ,NULL
        ,NULL
#else
        ,NULL
        ,NULL
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_MPEGMC_SUPPORT
        ,NULL
        ,NULL
        ,NULL
        ,NULL
        ,NULL
#else
        ,NULL
        ,NULL
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_WMAPRO_SUPPORT
        ,BDSP_IMG_wma_pro_ids
        ,BDSP_IMG_wma_pro_decode
        ,BDSP_IMG_wma_pro_decode_tables
        ,BDSP_IMG_wma_pro_ids_inter_frame            
        ,BDSP_IMG_wma_pro_decode_inter_frame            
#else
        ,NULL
        ,NULL
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_DTSNEO_SUPPORT
        ,BDSP_IMG_dts_neo_postprocess
        ,BDSP_IMG_dts_neo_postprocess_table
        ,BDSP_IMG_dts_neo_interframe_buf
#else
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_AVL_SUPPORT
        ,BDSP_IMG_brcm_avl_code
        ,BDSP_IMG_brcm_avl_tables
        ,BDSP_IMG_brcm_avl_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_PL2_SUPPORT
        ,BDSP_IMG_prologic2_code
        ,BDSP_IMG_prologic2_tables
        ,BDSP_IMG_prologic2_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_SRSXT_SUPPORT
        ,BDSP_IMG_trusurroundxt
        ,BDSP_IMG_trusurroundxt_tables
        ,BDSP_IMG_trusurroundxt_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_SRSHD_SUPPORT
        ,BDSP_IMG_srs_trusurroundhd_code
        ,BDSP_IMG_srs_trusurroundhd_tables
        ,BDSP_IMG_srs_trusurroundhd_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_SRSTRUVOL_SUPPORT
        ,BDSP_IMG_srs_tvol_code
        ,BDSP_IMG_srs_tvol_tables
        ,BDSP_IMG_srs_tvol_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_BBE_SUPPORT
        ,BDSP_IMG_bbe
        ,BDSP_IMG_bbe_table
        ,BDSP_IMG_bbe_interframe_buf
#else
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_AACDOWNMIX_SUPPORT
        ,NULL
        ,NULL
        ,NULL
#else
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_DTSENC_SUPPORT
        ,BDSP_IMG_dts_encode
        ,BDSP_IMG_dts_encode_tables
        ,BDSP_IMG_dts_encode_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_MS10_SUPPORT
        ,BDSP_IMG_dd_transcode 
        ,BDSP_IMG_dd_transcode_tables
        ,BDSP_IMG_dd_transcode_inter_frame
#else
#ifdef BDSP_AC3ENC_SUPPORT
        ,BDSP_IMG_dd_transcode 
        ,BDSP_IMG_dd_transcode_tables
        ,BDSP_IMG_dd_transcode_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
#endif
#endif

#ifdef BDSP_CUSTOMVOICE_SUPPORT
        ,BDSP_IMG_customvoice_code
        ,BDSP_IMG_customvoice_tables
        ,BDSP_IMG_customvoice_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_SRC_SUPPORT
        ,BDSP_IMG_src_code
        ,BDSP_IMG_src_tables
        ,BDSP_IMG_src_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_RFAUDIO_SUPPORT
        ,BDSP_IMG_RfAudioBtsc
        ,BDSP_IMG_RfAudioEiaj
        ,BDSP_IMG_RfAudioKoreaa2
        ,BDSP_IMG_RfAudioNicam
        ,BDSP_IMG_RfAudioPala2
        ,BDSP_IMG_RfAudioSecaml
        ,BDSP_IMG_RfAudioIndia
#else
        ,NULL
        ,NULL
        ,NULL
        ,NULL
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_DSOLA_SUPPORT
        ,BDSP_IMG_dsola_code
        ,BDSP_IMG_dsola_tables
        ,BDSP_IMG_dsola_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_DOLBYVOL_SUPPORT
        ,BDSP_IMG_dolby_volume
        ,BDSP_IMG_dolby_volume_table
        ,BDSP_IMG_dolby_volume_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_PCMWAV_SUPPORT
        ,BDSP_IMG_pcmwav_decode
        ,BDSP_IMG_pcmwav_decode_inter_frame
        ,BDSP_IMG_pcmwav_ids
        ,BDSP_IMG_pcmwav_ids_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_MP3ENC_SUPPORT
        ,BDSP_IMG_mp3_encode
        ,BDSP_IMG_mp3_encode_tables
        ,BDSP_IMG_mp3_encode_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_AMR_SUPPORT
        ,BDSP_IMG_amr_ids
        ,BDSP_IMG_amr_decode
        ,BDSP_IMG_amr_decode_tables
        ,BDSP_IMG_amr_decode_inter_frame
        ,BDSP_IMG_amr_ids_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_DRA_SUPPORT
        ,BDSP_IMG_dra_decode
        ,BDSP_IMG_dra_decode_tables
        ,BDSP_IMG_dra_decode_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_SBCENC_SUPPORT
        ,BDSP_IMG_sbc_encode
        ,BDSP_IMG_sbc_encode_table
        ,BDSP_IMG_sbc_encode_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef  BDSP_REALAUDIOLBR_SUPPORT
        ,BDSP_IMG_ralbr_decode
        ,BDSP_IMG_ralbr_decode_tables
        ,BDSP_IMG_ralbr_decode_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_BRCM3DSURROUND_SUPPORT
		,BDSP_IMG_brcm_3dsurround_code
		,BDSP_IMG_brcm_3dsurround_tables
		,BDSP_IMG_brcm_3dsurround_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_FWMIXER_SUPPORT
        ,BDSP_IMG_mixer_ids
        ,BDSP_IMG_fw_mixer_code
        ,BDSP_IMG_fw_mixer_tables
        ,BDSP_IMG_mixer_ids_inter_frame
        ,BDSP_IMG_fw_mixer_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_ADV_SUPPORT
        ,BDSP_IMG_adv_code
        ,BDSP_IMG_adv_tables
        ,BDSP_IMG_adv_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_ABX_SUPPORT
        ,BDSP_IMG_abx_code
        ,BDSP_IMG_abx_tables
        ,BDSP_IMG_abx_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_SRSCSTD_SUPPORT
        ,BDSP_IMG_srs_csdec_trudialog_code
        ,BDSP_IMG_srs_csdec_trudialog_tables
        ,BDSP_IMG_srs_csdec_trudialog_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_SRSEQHL_SUPPORT
        ,BDSP_IMG_srs_equ_hl_code
        ,BDSP_IMG_srs_equ_hl_tables
        ,BDSP_IMG_srs_equ_hl_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_CUSTOMDBE_SUPPORT
        ,BDSP_IMG_custom_dbe_code
        ,BDSP_IMG_custom_dbe_tables
        ,BDSP_IMG_custom_dbe_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_CUSTOMAVLP_SUPPORT
        ,BDSP_IMG_custom_avlp_code
        ,BDSP_IMG_custom_avlp_tables
        ,BDSP_IMG_custom_avlp_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_CUSTOMACF_SUPPORT
        ,BDSP_IMG_custom_acf_code
        ,BDSP_IMG_custom_acf_tables
        ,BDSP_IMG_custom_acf_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_ADPCM_SUPPORT
        ,BDSP_IMG_adpcm_ids
        ,BDSP_IMG_adpcm_decode
        ,BDSP_IMG_adpcm_decode_tables
        ,BDSP_IMG_adpcm_ids_inter_frame            
        ,BDSP_IMG_adpcm_decode_inter_frame            
#else
        ,NULL
        ,NULL
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_GENCDBITB_SUPPORT
        ,BDSP_IMG_gen_cdbitb_code
        ,BDSP_IMG_gen_cdbitb_tables
        ,BDSP_IMG_gen_cdbitb_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_BTSCENC_SUPPORT
        ,BDSP_IMG_btscenc_code
        ,BDSP_IMG_btscenc_tables
        ,BDSP_IMG_btscenc_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
#endif


#ifdef BDSP_AACHEENC_SUPPORT
        ,BDSP_IMG_aache_encode
        ,BDSP_IMG_aache_encode_tables
        ,BDSP_IMG_aache_encode_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_DV258_SUPPORT
		,BDSP_IMG_dv258_code
		,BDSP_IMG_dv258_tables
		,BDSP_IMG_dv258_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_DDRE_SUPPORT
		,BDSP_IMG_ddre_code
		,BDSP_IMG_ddre_tables
		,BDSP_IMG_ddre_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_VP6_SUPPORT
        ,BDSP_IMG_vp6_ids
        ,BDSP_IMG_vp6_decode
        ,BDSP_IMG_vp6_decode_tables
        ,BDSP_IMG_vp6_ids_inter_frame
        ,BDSP_IMG_vp6_decode_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_G711G726_SUPPORT
        ,BDSP_IMG_g711_g726_ids
        ,BDSP_IMG_g711_g726_decode
        ,BDSP_IMG_g711_g726_decode_tables
        ,BDSP_IMG_g711_g726_ids_inter_frame
        ,BDSP_IMG_g711_g726_decode_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_G729_SUPPORT
        ,BDSP_IMG_g729_ids
        ,BDSP_IMG_g729_decode
        ,BDSP_IMG_g729_decode_tables
        ,BDSP_IMG_g729_ids_inter_frame
        ,BDSP_IMG_g729_decode_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_VORBIS_SUPPORT
        ,BDSP_IMG_vorbis_ids
        ,BDSP_IMG_vorbis_decode
        ,BDSP_IMG_vorbis_decode_tables
        ,BDSP_IMG_vorbis_ids_inter_frame
        ,BDSP_IMG_vorbis_decode_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_FLAC_SUPPORT
        ,BDSP_IMG_flac_ids
        ,BDSP_IMG_flac_decode
        ,BDSP_IMG_flac_decode_tables
        ,BDSP_IMG_flac_ids_inter_frame
        ,BDSP_IMG_flac_decode_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_MAC_SUPPORT
        ,BDSP_IMG_mac_ids
        ,BDSP_IMG_mac_decode
        ,BDSP_IMG_mac_decode_tables
        ,BDSP_IMG_mac_ids_inter_frame
        ,BDSP_IMG_mac_decode_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_AMRWB_SUPPORT
        ,BDSP_IMG_amrwb_ids
        ,BDSP_IMG_amrwb_decode
        ,BDSP_IMG_amrwb_decode_tables
        ,BDSP_IMG_amrwb_decode_inter_frame
        ,BDSP_IMG_amrwb_ids_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_ILBC_SUPPORT
        ,BDSP_IMG_ilbc_ids
        ,BDSP_IMG_ilbc_decode
        ,BDSP_IMG_ilbc_decode_tables
        ,BDSP_IMG_ilbc_decode_inter_frame
        ,BDSP_IMG_ilbc_ids_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
        ,NULL
        ,NULL
#endif


#ifdef BDSP_ISAC_SUPPORT
        ,BDSP_IMG_isac_ids
        ,BDSP_IMG_isac_decode
        ,BDSP_IMG_isac_decode_tables
        ,BDSP_IMG_isac_decode_inter_frame
        ,BDSP_IMG_isac_ids_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
        ,NULL
        ,NULL
#endif


#ifdef BDSP_G711G726ENC_SUPPORT
        ,BDSP_IMG_g711_g726_encode
        ,BDSP_IMG_g711_g726_encode_tables
        ,BDSP_IMG_g711_g726_encode_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_G729ENC_SUPPORT
        ,BDSP_IMG_g729_encode
        ,BDSP_IMG_g729_encode_tables
        ,BDSP_IMG_g729_encode_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
#endif


#ifdef BDSP_WMAENC_SUPPORT
        ,BDSP_IMG_wma_encode
        ,BDSP_IMG_wma_encode_tables
        ,BDSP_IMG_wma_encode_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_G723_1_SUPPORT
        ,BDSP_IMG_g723_1_ids
        ,BDSP_IMG_g723_1_decode
        ,BDSP_IMG_g723_1_decode_tables
        ,BDSP_IMG_g723_1_ids_inter_frame
        ,BDSP_IMG_g723_1_decode_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_G723_1ENC_SUPPORT
        ,BDSP_IMG_g723_1_encode
        ,BDSP_IMG_g723_1_encode_tables
        ,BDSP_IMG_g723_1_encode_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_SPEEXAEC_SUPPORT
        ,BDSP_IMG_speexaec_code
        ,BDSP_IMG_speexaec_tables
        ,BDSP_IMG_speexaec_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_H264_ENCODE_SUPPORT
        ,BDSP_IMG_h264_encode
        ,BDSP_IMG_h264_encode_tables
        ,BDSP_IMG_h264_encode_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_VP8_ENCODE_SUPPORT
        ,BDSP_IMG_vp8_encode
        ,BDSP_IMG_vp8_encode_tables
        ,BDSP_IMG_vp8_encode_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_MLP_PASSTHROUGH_SUPPORT
        ,BDSP_IMG_mlp_passthrough_code
        ,BDSP_IMG_mlp_passthrough_tables
        ,BDSP_IMG_mlp_passthrough_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_G722ENC_SUPPORT
        ,BDSP_IMG_g722_encode
        ,BDSP_IMG_g722_encode_tables
        ,BDSP_IMG_g722_encode_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_AMRENC_SUPPORT
        ,BDSP_IMG_amr_encode
        ,BDSP_IMG_amr_encode_tables
        ,BDSP_IMG_amr_encode_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
#endif
#ifdef BDSP_AMRWBENC_SUPPORT
        ,BDSP_IMG_amrwb_encode
        ,BDSP_IMG_amrwb_encode_tables
        ,BDSP_IMG_amrwb_encode_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
#endif
#ifdef BDSP_SCM1_SUPPORT
        ,BDSP_IMG_scm1_decode
        ,BDSP_IMG_scm1_decode_tables
        ,BDSP_IMG_scm1_decode_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
#endif
#ifdef BDSP_SCM2_SUPPORT
        ,BDSP_IMG_scm2_decode
        ,BDSP_IMG_scm2_decode_tables
        ,BDSP_IMG_scm2_decode_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
#endif
#ifdef BDSP_SCM3_SUPPORT
        ,BDSP_IMG_scm3_decode   
        ,BDSP_IMG_scm3_decode_tables
        ,BDSP_IMG_scm3_decode_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_ILBCENC_SUPPORT
        ,BDSP_IMG_ilbc_encode
        ,BDSP_IMG_ilbc_encode_tables
        ,BDSP_IMG_ilbc_encode_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
#endif

#ifdef BDSP_ISACENC_SUPPORT
        ,BDSP_IMG_isac_encode
        ,BDSP_IMG_isac_encode_tables
        ,BDSP_IMG_isac_encode_inter_frame
#else
        ,NULL
        ,NULL
        ,NULL
#endif


};


/*	This context is used by other modules to make use of this interface. They 
    need to supply this as a parameter to BDSP_IMG_Open */
void *BDSP_IMG_Context = (void *)BDSP_IMG_FirmwareImages;


/* This function has to be used for opening a firmware image. The pointer to the 
    firmware image array is given that contains the header and the chunks. 
    This works based on the firmware image id  that is supplied.
*/

static BERR_Code BDSP_IMG_Open(void *context, void **image, unsigned image_id)
{
	if (image_id >=  sizeof(BDSP_IMG_FirmwareImages)/sizeof(*BDSP_IMG_FirmwareImages)) 
	{
		/* If the image ID is greater than available IDs return an error */
		BDBG_ERR(("Invalid image_id %d",image_id));
		return BERR_TRACE(BERR_INVALID_PARAMETER);	
	}	

	*image = ((void **)context)[image_id];	

	if (NULL == *image)
	{
	    BDBG_ERR (("Null image pointer!!!"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);	
	}

	return BERR_SUCCESS;
}

/*  After opening the firmware image, the user of this interface will then be 
    interested in getting the chunks and the header giving information about the 
    chunks.
*/

static BERR_Code BDSP_IMG_Next(void *image, unsigned chunk, const void **data, uint16_t length)
{

	BDBG_ASSERT(data);	
	BSTD_UNUSED(length);

	if (chunk > ((uint32_t*)((const void **)image)[0])[1])
	{
		/* if the chunk number is greater than the available chunks in that 
		    image return error */
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	*data = ((const void **)image)[chunk];	
	
	if (NULL == *data) 
	{		
		return BERR_TRACE(BERR_INVALID_PARAMETER);	
	}	

	return BERR_SUCCESS;
}

/* This function is used to close the firmware image that was opened using 
    BDSP_IMG_Open */
static void BDSP_IMG_Close(void *image)
{	

	BSTD_UNUSED(image);	
	return;
}

/* The interface is actually a set of three functions open, next and close. 
    The same has been implemented here and their function pointers supplied */
const BIMG_Interface BDSP_IMG_Interface = {
    BDSP_IMG_Open, 
    BDSP_IMG_Next, 
    BDSP_IMG_Close
    };




