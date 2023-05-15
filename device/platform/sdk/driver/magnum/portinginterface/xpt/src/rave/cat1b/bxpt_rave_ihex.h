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
 * $brcm_Log: /magnum/portinginterface/xpt/src/rave/cat1b/bxpt_rave_ihex.h $                                                            
 * 
 * Hydra_Software_Devel/1   6/18/12 11:14a gmullen
 * SW7425-2914: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7425-2914/1   6/11/12 11:00a gmullen
 * SW7425-2914: Moved and updated files to new home
 * 
 * 3   8/10/11 2:48p shilpag
 * xpt_rave_fw_2.1.s (10th August 2011) - This firmware is generated based
 * on CAT1B Rave Firmware version 2.0 - This firmware is generated based
 * on Rave Unified Firmware version - xpt_rave_fw_unified_3.001.003 -
 * Modified FWXPT_15_TS_PACKET_ITB_SUPPORT record feature to support 13
 * Byte start code extraction along with the non-13 byte start code
 * extraction.
 * 
 * 2   6/27/11 4:30p teocw
 * xpt_rave_fw_2.0.s  - Upgraded to version 2.0
 * 
 * 1   6/27/11 2:37p bryanl
 * xpt_rave_fw_1.0.s (24/6/2011)
 *  - This is the baseline version for Category based 1B RAVE FW code.
 *  - This is based on BCM7413B0 ver1.5 RAVE FW code
 *  - This firmware is generated from UNIFIED RAVE FW
 *    version xpt_rave_fw_unified_3.000.000
 * 
 *                                                                          
 * RAVE FW Category  : CAT1B                                             
 * RAVE FW Version   : 2.1                                             
 * RAVE FW CRC       : 43b709                                                 
 * Chips supported   : 7336A, 7405B, 7413A
 *                   : 7413B
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
