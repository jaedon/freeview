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
 * $brcm_Log: /magnum/portinginterface/xpt/src/rave/cat7400e/bxpt_rave_ihex.h $     
 * 
 * Hydra_Software_Devel/1   6/18/12 11:14a gmullen
 * SW7425-2914: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7425-2914/1   6/11/12 11:00a gmullen
 * SW7425-2914: Moved and updated files to new home
 * 
 * 11   1/29/10 2:56p shilpag
 * xpt_rave_fw_1.10.s (29th January 2010)
 * - This firmware is based on firmware version 1.9.
 * - This firmware is generated using Rave Firmware Unified base 1.53.
 * - Added the fix and feature for,
 *     - OPTIMIZE_REC_GEN_ITB
 *     - GENERAL_OPT
 *     - PR57627_SCRAMBLE_CTRL_DETECT_MODE
 *     - PR57314_REPORT_CDB_OVERFLOW_FOR_ALL_OVERFLOWS
 *     - REC_AUTO_UPDATE_RD_PTR
 * 
 * 10   12/3/08 1:49p shilpag
 * xpt_rave_fw_1.9.s
 * - Added PR44875 RAVE FW optimized splicing fix.
 * 
 * 9   11/21/08 3:15p shilpag
 * xpt_rave_fw_1.8.s
 * - This version is based on 1.7
 * - Added a Record Fix for PR41133: To fix ITB overflow when signal is
 * disconnected and reconnected
 * - Added a Record Fix for PR47907: NULL packets not recorded properly in
 * allpass recording mode
 * 
 * 8   9/17/08 11:11a shilpag
 * xpt_rave_fw_1.7.s
 * - Added PR44875 RAVE FW splicing fix.
 * 
 * 
 * 7   2/21/08 2:20p bryanl
 * xpt_rave_fw_1.6.s
 *    - This version is based on 1.5
 *    - Added RAVE FW fix for TEI handling
 *    - Added RAVE FW fix for comparator num bits extract handling for
 * H.263
 *
 *
 * 6   2/15/08 3:06p teocw
 * xpt_rave_fw_1.5.s
 *  - This version is based on 1.4
 *  - Added fw fix for PR37892 to set overflow interrupt bit for every
 * overflow packet.
 *
 * 5   12/19/07 2:12a bryanl
 * xpt_rave_fw_1.4.s
 *    - This version is based on 1.3
 *    - Added PR37131 FW fix
 *
 * 4   11/30/07 4:59p teocw
 * xpt_rave_fw_1.3.s
 *  - This version is based on 1.2
 *  - Added SVP_OTF Feature to the code base
 *
 * 3   11/30/07 4:55p teocw
 * xpt_rave_fw_1.2.s
 *  - This version is based on 1.1
 *  - Added PR33018 RAVE FW feature which enables or disables the showing
 * of
 * PCR/RTS ITB entries.
 *  - Added PR34934 RAVE FW fix for duplicate CC check.
 *
 * 2   11/30/07 4:48p teocw
 * xpt_rave_fw_1.1.s
 *  - This version is based on 1.0
 *  - Added TPIT Scrambling bit (SC) change for all bands in RECORD FW.
 *  - Minor modifcation to IP Checksum computation.
 *  - PR28670 RAVE FW fix
 *
 * 1   11/30/07 4:40p teocw
 * xpt_rave_fw_1.0.s (Baseline)
 *  - Initial baseline version
 *  - This version is based on 7400B UNIFIED ITB FORMAT ver1.2
 *                                                                          
 *                                                                          
 * RAVE FW Version:    1.10                                             
 * RAVE FW CRC:        1226fb                                                 
 *                                                                          
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
