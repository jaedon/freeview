/***************************************************************************
 *     Copyright (c) 2003-2013, Broadcom Corporation                        
 *     All Rights Reserved                                                  
 *     Confidential Property of Broadcom Corporation                        
 *                                                                          
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE  
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR  
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT. 
 *                                                                          
 * $brcm_Workfile: bxpt_rave_ihex.h $                                                       
 * $brcm_Revision: Hydra_Software_Devel/11 $                                                       
 * $brcm_Date: 4/8/13 7:59p $                                                           
 *                                                                          
 * Porting interface code for the data transport core.                      
 *                                                                          
 * Revision History:                                                        
 *                                                                          
 * $brcm_Log: /magnum/portinginterface/xpt/src/rave/cat4a/bxpt_rave_ihex.h $                                                            
 * 
 * Hydra_Software_Devel/11   4/8/13 7:59p gmullen
 * FWXPT-49 : Copied fw to Hydra branch
 * 
 * 24   4/9/13 10:12a zxj
 * fix the bug of  ITB corruption in timestamp handling feature
 *                                                                          
 *                                                                          
 * RAVE FW Category  : CAT4A                                             
 * RAVE FW Version   : 2.18                                             
 * RAVE FW CRC       : 65c08f                                                 
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
