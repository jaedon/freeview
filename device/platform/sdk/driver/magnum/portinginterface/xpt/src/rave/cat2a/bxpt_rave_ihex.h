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
 * $brcm_Revision: Hydra_Software_Devel/3 $                                                       
 * $brcm_Date: 7/26/12 11:01a $                                                           
 *                                                                          
 * Porting interface code for the data transport core.                      
 *                                                                          
 * Revision History:                                                        
 *                                                                          
 * $brcm_Log: /magnum/portinginterface/xpt/src/rave/cat2a/bxpt_rave_ihex.h $                                                            
 * 
 * Hydra_Software_Devel/3   7/26/12 11:01a gmullen
 * FWXPT-35 : Copied fw to Hydra branch
 * 
 * 5   7/26/12 2:31p bryanl
 * xpt_rave_fw_2.3.s    - This version is based on version 2.2    -
 * Additional feature include:         -
 * FWXPT_35_REC_ITB_WRAPAROUND_FOR_CDB_OVERFLOW. This is for issue of ITB
 * has wrap around for 24 bytes after CDB overflows        -
 * FWXPT_31_BPP_BTP_HEADEND_TAILEND_FIX. This checks for BTP Command = 3
 * for PROCESS CMD
 *      
 * 4   7/23/12 3:57p bryanl
 * xpt_rave_fw_2.2.s    
 *   - This version is based on version 2.1
 *   - Additional feature include:
 *     * FWXPT_23_DROP_DATA_ON_PUSI_ERR. This is for Irdeto's Watermarking feature
 *   
 * 3   8/26/11 11:32a shilpag
 * xpt_rave_fw_2.1.s (26th August 2011) - This firmware is generated based
 * on CAT2A Rave Firmware version 2.0 - This firmware is generated based
 * on Rave Unified Firmware version - xpt_rave_fw_unified_3.001.003 -
 * Modified FWXPT_15_TS_PACKET_ITB_SUPPORT record feature to support 13
 * Byte start code extraction along with the  non-13 byte start code
 * extraction.
 * 
 * 
 * 2   6/27/11 4:13p teocw
 * xpt_rave_fw_2.0.s  - Upgraded to version 2.0
 * 
 * 1   6/24/11 9:40a bryanl
 * xpt_rave_fw_1.0.s
 *  - This is the baseline version for Category based 2A RAVE FW code.
 *  - This is based on 7550B ver1.0 RAVE FW code
 *  - Additional features include:
 *    FWXPT_11_REC_OVERFLOW_STATUS_UPDATE
 *    FWXPT_14_CHECK_UPDATE_SCRAMBLING_CTRL_BEFORE_OVERFLOW
 *    FWXPT_16_REC_ADAPTATION_FIELD_LEN_CHECK_UPDATE
 *                                                                          
 * RAVE FW Category  : CAT2A                                             
 * RAVE FW Version   : 2.3                                             
 * RAVE FW CRC       : 015877                                                 
 * Chips supported   : 7125A, 7340A, 7342A
 *                   : 7420A, 7420B, 7420C
 *                   : 7468A, 7468B, 7550A
 *                   : 7550B
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
