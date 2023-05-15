/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvee_dsp_utils_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 6/13/12 9:44a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vee/7346/bvee_dsp_utils_priv.h $
 * 
 * Hydra_Software_Devel/3   6/13/12 9:44a venkatr
 * SW7344-336 : [7344] Raaga Transcode support 30/29.97 and 15/14.98 frame
 * rates. Modified to code to get proper STC register offset
 * 
 * Hydra_Software_Devel/2   3/15/12 10:51a venkatr
 * SW7231-585: [7231] Add the macros to get the STC address respective to
 * stc index
 * 
 * Hydra_Software_Devel/1   10/19/11 8:30p venkatr
 * SW7346-515 : [7346] VEE PI code initial check in
 * 
 *
 ***************************************************************************/
#ifndef BVEE_DSP_UTILS_PRIV_H_
#define BVEE_DSP_UTILS_PRIV_H_

#include "bvee_priv.h"
#include "bdsp.h"
#include "bdsp_task.h"
#include "bdsp_audio_task.h"
#ifdef BCHP_AIO_MISC_REG_START
#include "bchp_aio_misc.h"
#include "bchp_aud_fmm_misc.h"
#else
#include "bchp_aud_misc.h"
#endif

 /***************************************************************************
 Summary:
 Codec Attribute Table Entry
 ***************************************************************************/
 typedef struct BVEE_CodecAttributes
 {
     BAVC_VideoCompressionStd codec;
     BDSP_VideoEncode dspVideoType;
     const char *pName;
 } BVEE_CodecAttributes;
 
 /***************************************************************************
 Summary:
 Get Codec Attributes
 ***************************************************************************/
 const BVEE_CodecAttributes *BVEE_P_GetCodecAttributes(
     BAVC_VideoCompressionStd codec
     );
 
 void BVEE_DSP_P_InitBranch(
    BDSP_CIT_P_FwBranchInfo *pBranch
    );
 
 #define BVEE_P_GetCodecName(codec) (BVEE_P_GetCodecAttributes((codec))->pName)
 #define BVEE_P_GetCodecVideoType(codec) (BVEE_P_GetCodecAttributes((codec))->dspVideoType)
 /***************************************************************************
 Summary:
 Get STC ADDRESS to be passed to DSP
 ***************************************************************************/
 #ifdef BCHP_AUD_FMM_MISC_STC_UPPERi_ARRAY_END

 /* Legacy RDB */
 #define BVEE_CHP_MAX_STCS (BCHP_AUD_FMM_MISC_STC_UPPERi_ARRAY_END)
 #ifdef BCHP_AUD_FMM_MISC_STC_LOWERi_ARRAY_BASE
 #define BVEE_CHP_GET_STC_ADDR(idx) (BCHP_PHYSICAL_OFFSET + BCHP_AUD_FMM_MISC_STC_LOWERi_ARRAY_BASE + ((BCHP_AUD_FMM_MISC_STC_LOWERi_ARRAY_ELEMENT_SIZE/8)*(idx)))
 #else
 #define BVEE_CHP_GET_STC_ADDR(idx) (BCHP_PHYSICAL_OFFSET + BCHP_AUD_FMM_MISC_STC_UPPERi_ARRAY_BASE + ((BCHP_AUD_FMM_MISC_STC_UPPERi_ARRAY_ELEMENT_SIZE/8)*(idx)))
 #endif
 #else
 
 /* 7429 style RDB */
 #define BVEE_CHP_MAX_STCS (BCHP_AUD_MISC_STC_UPPERi_ARRAY_END)
 #ifdef BCHP_AUD_MISC_STC_LOWERi_ARRAY_BASE
 #define BVEE_CHP_GET_STC_ADDR(idx) (BCHP_PHYSICAL_OFFSET + BCHP_AUD_MISC_STC_LOWERi_ARRAY_BASE + ((BCHP_AUD_MISC_STC_LOWERi_ARRAY_ELEMENT_SIZE/8)*(idx)))
 #else
 #define BVEE_CHP_GET_STC_ADDR(idx) (BCHP_PHYSICAL_OFFSET + BCHP_AUD_MISC_STC_UPPERi_ARRAY_BASE + ((BCHP_AUD_MISC_STC_UPPERi_ARRAY_ELEMENT_SIZE/8)*(idx)))
 #endif

 #endif

#endif
