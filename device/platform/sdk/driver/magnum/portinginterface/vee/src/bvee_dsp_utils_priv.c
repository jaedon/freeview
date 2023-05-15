/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvee_dsp_utils_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 10/19/11 8:30p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vee/7346/bvee_dsp_utils_priv.c $
 * 
 * Hydra_Software_Devel/1   10/19/11 8:30p venkatr
 * SW7346-515 : [7346] VEE PI code initial check in
 * 
 *
 ***************************************************************************/
 #include "bvee_dsp_utils_priv.h"

 BDBG_MODULE(bvee_dsp_utils);
 
 static const BVEE_CodecAttributes g_codecAttributes[] =
 {
 /*   AVC Codec                              DSP Type                        Name            */
     {BAVC_VideoCompressionStd_eH264,         BDSP_VideoEncode_eH264,            "H264",          },
     
     /* This entry must be last */
     {BAVC_VideoCompressionStd_eMax,         BDSP_VideoType_eMax,            "Unknown",      }
 };

 const BVEE_CodecAttributes *BVEE_P_GetCodecAttributes(
    BAVC_VideoCompressionStd codec
    )
 {
     unsigned i, tableSize;
 
     tableSize = sizeof(g_codecAttributes)/sizeof(BVEE_CodecAttributes);
 
     for ( i = 0; i < tableSize; i++ )
     {
         if ( codec == g_codecAttributes[i].codec )
         {
             return &g_codecAttributes[i];
         }
     }
 
     return &g_codecAttributes[tableSize-1];
 }
 
 void BVEE_DSP_P_InitBranch(
    BDSP_CIT_P_FwBranchInfo *pBranch
    )
 {
     BDBG_ASSERT(NULL != pBranch);
     BKNI_Memset(pBranch, 0, sizeof(BDSP_CIT_P_FwBranchInfo));
 }
