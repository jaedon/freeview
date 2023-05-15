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
 * $brcm_Date: 6/18/12 11:17a $                                                           
 *                                                                          
 * Porting interface code for the data transport core.                      
 *                                                                          
 * Revision History:                                                        
 *                                                                          
 * $brcm_Log: /magnum/portinginterface/xpt/src/rave/cat1a/bxpt_rave_ihex.h $                                                            
 * 
 * Hydra_Software_Devel/1   6/18/12 11:17a gmullen
 * SW7425-2914: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7425-2914/1   6/11/12 11:00a gmullen
 * SW7425-2914: Moved and updated files to new home
 * 
 * 4   3/27/12 10:31a bryanl
 * xpt_rave_fw_2.2.s    - This firmware is generated based on CAT1A Rave
 * Firmware version 2.1    - Added FWXPT_23_DROP_DATA_ON_PUSI_ERR feature
 * support.
 *     
 * 3   8/10/11 2:45p shilpag
 * xpt_rave_fw_2.1.s (10th August 2011) 
 *  - This firmware is generated based on CAT1A RAVE Firmware version 2.0
 *  - Modified FWXPT_15_TS_PACKET_ITB_SUPPORT record feature to support 13
 *    Byte start code extraction along with the non-13 byte start code
 *    extraction.
 * 
 * 2   6/27/11 4:32p teocw
 * xpt_rave_fw_2.0.s  - Upgraded to version 2.0
 * 
 * 1   6/27/11 10:31a bryanl
 * xpt_rave_fw_1.0.s
 *  - This is the baseline version for Category based 1A RAVE FW code.
 *  - This is based on BCM7335B0 ver1.5 RAVE FW code
 *                                                                          
 * RAVE FW Category  : CAT1A                                             
 * RAVE FW Version   : 2.2                                             
 * RAVE FW CRC       : 44f9d1                                                 
 * Chips supported   : 7325A, 7325B, 7335A
 *                   : 7335B
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
