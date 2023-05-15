/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation                        
 *     All Rights Reserved                                                  
 *     Confidential Property of Broadcom Corporation                        
 *                                                                          
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE  
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR  
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT. 
 *                                                                          
 * $brcm_Workfile: bxpt_rave_ihex.h $                                                       
 * $brcm_Revision: Hydra_Software_Devel/1 $                                                       
 * $brcm_Date: 6/18/12 11:14a $                                                           
 *                                                                          
 * Porting interface code for the data transport core.                      
 *                                                                          
 * Revision History:                                                        
 *                                                                          
 * $brcm_Log: /magnum/portinginterface/xpt/src/rave/cat2b/bxpt_rave_ihex.h $                                                            
 * 
 * Hydra_Software_Devel/1   6/18/12 11:14a gmullen
 * SW7425-2914: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7425-2914/1   6/11/12 11:00a gmullen
 * SW7425-2914: Moved and updated files to new home
 * 
 * 2   6/27/11 4:09p teocw
 * xpt_rave_fw_2.0.s  - Upgraded to version 2.0
 * 
 * 1   6/24/11 2:27p bryanl
 * xpt_rave_fw_1.0.s
 *  - This is the baseline version for Category based 2B RAVE FW code.
 *  - This is based on 35230A0 ver1.2 RAVE FW code
 *  - Additional features include:
 *    SW7408_80_BTP_PROCESS_DELAY_FIX_V2
 *    SW7335_785_PR47907_REC_SKIP_MPEG_NULL_PKT_CORRUPT
 *    SW35230_594_FW_FIX_NON_1_8_9_BYTES
 *    FWXPT_4_ASF_SEQ_HDR_MULTI_STARTCODE_SUPPORT
 *    FWXPT_3_CLEAR_SCD_DMEM
 *    FWXPT_5_BASE_ENTRY_FOR_EVERY_PKTS_AUDIO
 *    SW7335_1153_ITB_WRITE_PTR_OVER_ITB_END_PTR
 *    FWXPT_11_REC_OVERFLOW_STATUS_UPDATE
 *    FWXPT_14_CHECK_UPDATE_SCRAMBLING_CTRL_BEFORE_OVERFLOW
 *                                                                          
 *                                                                          
 * RAVE FW Category  : CAT2B                                             
 * RAVE FW Version   : 2.0
 * RAVE FW CRC       : 13be99                                                 
 * Chips supported   : 35125A, 35130A, 35230A
 *                   : 35230B, 35230C,7408A 
 *                                                                          
 ***************************************************************************/


#ifndef BXPT_RAVE_IHEX_H__
#define BXPT_RAVE_IHEX_H__

#ifdef __cplusplus
extern "C" {
#endif

/* RAVE microcode. */
extern const uint32_t BxptRaveInitData[];
extern const unsigned long BxptRaveInitDataSize;

#ifdef __cplusplus
}
#endif

#endif /* BXPT_RAVE_IHEX_H__ */

/* end of file */
