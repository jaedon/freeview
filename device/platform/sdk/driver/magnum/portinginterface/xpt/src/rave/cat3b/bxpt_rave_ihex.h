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
 * $brcm_Log: /magnum/portinginterface/xpt/src/rave/cat3b/bxpt_rave_ihex.h $                                                            
 * 
 * Hydra_Software_Devel/1   6/18/12 11:14a gmullen
 * SW7425-2914: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7425-2914/1   6/11/12 11:00a gmullen
 * SW7425-2914: Moved and updated files to new home
 * 
 * 4   9/7/11 11:20a shilpag
 * xpt_rave_fw_2.1.s (7th September 2011) - This firmware is generated
 * based on CAT3B Rave Firmware version 2.0 - This firmware is generated
 * based on Rave Unified Firmware version - xpt_rave_fw_unified_3.001.003
 * - Added the following features into Record Firmware, 1.
 * FWXPT_15_TS_PACKET_ITB_SUPPORT -  TS Packet ITB feature is supported
 * in Record firmware due to hardware ITB buffer limitation which support
 * maximum of  10 ITB per ITB DMA. The TS Packet ITB will be generated
 * only if the number of start code greater than 8. This  feature
 * generates following ITB,  - TPIT Entry (irrespective of
 * enable/disable, indicating the number of start code greater than 8 for
 * non-13byte  extract and 4 for 13 byte extract)  - Spanning start code
 * entries  - TS Packet ITB entries 2.
 * 
 * 
 * 2   6/27/11 3:58p teocw
 * xpt_rave_fw_2.0.s  - Upgraded to version 2.0
 * 
 * 1   6/23/11 1:00p bryanl
 * xpt_rave_fw_1.0.s
 *  - This is the baseline version for Category based 3B RAVE FW code.
 *  - This is based on 35330A ver1.0 RAVE FW code
 *  - Additional features include:
 *    SW7335_1153_ITB_WRITE_PTR_OVER_ITB_END_PTR
 *    FWXPT_11_REC_OVERFLOW_STATUS_UPDATE
 *    FWXPT_14_CHECK_UPDATE_SCRAMBLING_CTRL_BEFORE_OVERFLOW
 *    FWXPT_16_REC_ADAPTATION_FIELD_LEN_CHECK_UPDATE
 * 
 * 
 * RAVE FW Category  : CAT3B                                             
 * RAVE FW Version   : 2.1                                             
 * RAVE FW CRC       : 29762f                                                 
 * Chips supported   : 35330A
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
