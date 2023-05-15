/***************************************************************************
 *     Copyright (c) 1999-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bchp_kbd3.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 10/27/11 11:31p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Thu Oct 27 17:22:37 2011
 *                 MD5 Checksum         d0bb3b528cbe25f62f7a44e82cd25af7
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7425/rdb/b1/bchp_kbd3.h $
 * 
 * Hydra_Software_Devel/1   10/27/11 11:31p vanessah
 * SW7425-1620: add 7425 B0 rdb header file
 *
 ***************************************************************************/

#ifndef BCHP_KBD3_H__
#define BCHP_KBD3_H__

/***************************************************************************
 *KBD3 - IR Keyboard/Remote Receiver 3
 ***************************************************************************/
#define BCHP_KBD3_STATUS                         0x00409140 /* KEYBOARD/REMOTE RECEIVER STATUS REGISTER */
#define BCHP_KBD3_DIVISOR                        0x00409144 /* KEYBOARD/REMOTE RECEIVER DIVISOR */
#define BCHP_KBD3_FILTER1                        0x00409148 /* KEYBOARD FILTER1 */
#define BCHP_KBD3_DATA1                          0x0040914c /* KEYBOARD/REMOTE RECEIVER DATA WORD 1 (MS Word) */
#define BCHP_KBD3_DATA0                          0x00409150 /* KEYBOARD/REMOTE RECEIVER DATA WORD 0 (LS Word) */
#define BCHP_KBD3_CMD                            0x00409154 /* KEYBOARD/REMOTE RECEIVER COMMAND REGISTER */
#define BCHP_KBD3_CIR_ADDR                       0x00409158 /* CONSUMER IR DECODER ADDRESS REGISTER */
#define BCHP_KBD3_CIR_DATA                       0x0040915c /* CONSUMER IR DECODER DATA (LSB) */
#define BCHP_KBD3_KBD_MASK0                      0x00409160 /* KEBOARD INPUT MASK 0 */
#define BCHP_KBD3_KBD_MASK1                      0x00409164 /* KEBOARD INPUT MASK 1 */
#define BCHP_KBD3_KBD_MASK2                      0x00409168 /* KEBOARD INPUT MASK 2 */
#define BCHP_KBD3_KBD_PAT0                       0x0040916c /* KEBOARD INPUT PATTERN MATCH 0 */
#define BCHP_KBD3_KBD_PAT1                       0x00409170 /* KEBOARD INPUT PATTERN MATCH 1 */
#define BCHP_KBD3_KBD_PAT2                       0x00409174 /* KEBOARD INPUT PATTERN MATCH 2 */
#define BCHP_KBD3_STATUS2                        0x0040917c /* KEYBOARD/REMOTE RECEIVER STATUS2 REGISTER */

#endif /* #ifndef BCHP_KBD3_H__ */

/* End of File */