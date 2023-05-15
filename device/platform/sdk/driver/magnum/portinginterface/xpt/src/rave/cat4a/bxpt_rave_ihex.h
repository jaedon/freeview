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
 * $brcm_Revision: Hydra_Software_Devel/4 $                                                       
 * $brcm_Date: 8/23/12 2:32p $                                                           
 *                                                                          
 * Porting interface code for the data transport core.                      
 *                                                                          
 * Revision History:                                                        
 *                                                                          
 * $brcm_Log: /magnum/portinginterface/xpt/src/rave/cat4a/bxpt_rave_ihex.h $                                                            
 * 
 * Hydra_Software_Devel/4   8/23/12 2:32p gmullen
 * FWXPT-31 : Copied fw to Hydra branch
 * 
 * 17   8/23/12 2:54p bryanl
 * xpt_rave_fw_2.12.s     - This firmware is generated based on CAT4A RAVE
 * FW version 2.11     - It includes the following changes:          -
 * FWXPT_31_BPP_BTP_HEADEND_TAILEND_FIX. This FW checks Process Command =
 * 0x3          - FWXPT_38_SPLICE_FIX_FOR_7425B_ONWARDS. This is a
 * generic handling for increasing number of PIDCHs.
 *             
 * 16   7/23/12 3:01p shilpag
 * xpt_rave_fw_2.11.s
 *  - This firmware is generated based on CAT4A Rave Firmware version 2.10
 *  - It includes the following changes:
 *     - Added FWXPT_35_REC_ITB_WRAPAROUND_FOR_CDB_OVERFLOW 
 *     - Rave Firmware Fix for ITB Wrapraound issue during CDB Overflow condition
 * 
 * 15   7/4/12 5:40p shilpag
 * xpt_rave_fw_2.10.s 
 *  - This firmware is generated based on CAT4A Rave Firmware version 2.9
 *  - It includes the fixes:
 *    - Added P1 related changes for split packet feature
 *    - Added FWXPT-32 fix for (SW7425-3146) Invalid SCT produced in 16-byte extraction mode
 *    - Added ATS Offset related fix in ATS Offset feature
 *   
 * 14   4/20/12 11:39a bryanl
 * - This firmware is generated based on CAT4A RAVE Firmware 2.8  - Added
 * 1 feature:        * FWXPT_28_REC_BTP_LAST. This feature drops BTP
 * packet when LAST command is detected
 * 
 * 13   4/12/12 4:22p bryanl
 * xpt_rave_fw_unified_3.004.007      
 *      - Modified RAVE FW unified firmware to update support for 1 feature:
 *      FWXPT_28_REC_BTP_LAST. RAVE FW drops the BTP packet when LAST command is detected.
 *    
 * 12   11/25/11 1:45p bryanl
 * xpt_rave_fw_2.8.s    
 *      - This firmware is generated based on CAT4A RAVE Firmware 2.8
 *      - Removed FWXPT_24_BTP_HOLD_FW_BASED. Revert back to using RAVE HW hold mechanism with fixes added.
 * 
 * 11   11/22/11 12:28p bryanl
 * xpt_rave_fw_2.7.s    
 *     - This firmware is generated based on CAT4A RAVE Firmware 2.7
 *     - Modified RAVE FW for mode1 and mode2 operations in Transport Security
 *       feature for RAVE BTP in FWXPT_20_BTP_XPT_SECURITY
 * 
 * 10   11/16/11 4:19p bryanl
 * xpt_rave_fw_2.6.s    
 *     - This firmware is generated based on CAT4A RAVE Firmware 2.5
 *     - Added 1 fix:  
 *       * FWXPT_24_BTP_HOLD_FW_BASED.
 *         This is a FW fix for resolving PB freeze issue in trick mode.
 * 
 * 9   10/14/11 4:52p bryanl
 * xpt_rave_fw_2.5.s    
 *    - This firmware is generated based on CAT4A RAVE Firmware 2.4
 *    - Added 1 fix:       
 *        * FWXPT_22_BTP_REC_FLOW_CHN. This is a minor fix for RECORD BTP mode.
 *          Currently no requirement for RECORD BTP mode
 * 
 * 8   10/6/11 11:10a bryanl
 * xpt_rave_fw_2.4.s    
 *    - This firmware is generated based on CAT4A RAVE Firmware version 2.3
 *    - Added 1 feature:
 * 	  * FWXPT_20_BTP_XPT_SECURITY. This feature is required by XPT Security
 *        and the BTP command for it is 0x1A.
 *
 * 7   8/15/11 2:32p shilpag
 * xpt_rave_fw_2.3.s 
 *    - This firmware is generated based on CAT4A Rave Firmware version 2.2 
 *    - Added the following features into Record Firmware, 
 *      1. FWXPT_15_TS_PACKET_ITB_SUPPORT -  TS Packet ITB feature is supported
 *         in Record firmware due to hardware ITB buffer limitation which support
 *         maximum of 10 ITB per ITB DMA. The TS  Packet ITB will be generated
 *         only if the number of start code greater than 8. This feature
 *         generates following ITB,  - TPIT Entry (irrespective of
 *         enable/disable, indicating the number of start code greater than 8 for
 *         non-13byte extract and 4 for 13 byte extract)  - Spanning start code
 *         entries  - TS Packet ITB entries 2.
 * 
 * 6   8/3/11 10:31a bryanl
 * xpt_rave_fw_2.2.s    
 *    - This is Category 4A RAVE FW code    
 *    - This FW code is based on version 2.1 RAVE FW code    
 *    - Modification to the RAVE FW Code includes:        
 *      * Removal of 4 dead code lines from ASF feature.
 *       
 * 5   8/2/11 2:00p bryanl
 * Upgraded to version 2.1
 * 
 * 4   7/4/11 12:00p bryanl
 * xpt_rave_fw_1.1.s    
 *   - This is Category 4A RAVE FW code.    
 *   - This FW code is based on version 1.0 RAVE FW code
 *   - Additional feature/fix added:
 *     *FWXPT_18_HIGH_SCDNUM_ISSUE
 *     *FWXPT_10_REC_SCD_PTS_MODE_DISABLE_CHECK
 *        
 * 1   6/17/11 4:42p bryanl
 * xpt_rave_fw_1.0.s    
 *   - This is the baseline version for Category based 4A RAVE FW code.
 *                                                                          
 * RAVE FW Category  : CAT4A                                             
 * RAVE FW Version   : 2.12                                             
 * RAVE FW CRC       : 576ffe                                                 
 * Chips supported   : 7425B, 7346B, 7231B, 7584
 *                   : 7429, 7356A, 7435A
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
