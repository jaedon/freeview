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
 * $brcm_Workfile: bchp_kbd2.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 4/13/11 6:17p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Wed Apr 13 13:21:56 2011
 *                 MD5 Checksum         5014fc6b805cdf8eed48fe0da9f96997
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7231/rdb/b0/bchp_kbd2.h $
 * 
 * Hydra_Software_Devel/1   4/13/11 6:17p albertl
 * SW7231-123: Initial revision.
 *
 ***************************************************************************/

#ifndef BCHP_KBD2_H__
#define BCHP_KBD2_H__

/***************************************************************************
 *KBD2 - IR Keyboard/Remote Receiver 2
 ***************************************************************************/
#define BCHP_KBD2_STATUS                         0x00408900 /* KEYBOARD/REMOTE RECEIVER STATUS REGISTER */
#define BCHP_KBD2_DIVISOR                        0x00408904 /* KEYBOARD/REMOTE RECEIVER DIVISOR */
#define BCHP_KBD2_FILTER1                        0x00408908 /* KEYBOARD FILTER1 */
#define BCHP_KBD2_DATA1                          0x0040890c /* KEYBOARD/REMOTE RECEIVER DATA WORD 1 (MS Word) */
#define BCHP_KBD2_DATA0                          0x00408910 /* KEYBOARD/REMOTE RECEIVER DATA WORD 0 (LS Word) */
#define BCHP_KBD2_CMD                            0x00408914 /* KEYBOARD/REMOTE RECEIVER COMMAND REGISTER */
#define BCHP_KBD2_CIR_ADDR                       0x00408918 /* CONSUMER IR DECODER ADDRESS REGISTER */
#define BCHP_KBD2_CIR_DATA                       0x0040891c /* CONSUMER IR DECODER DATA (LSB) */
#define BCHP_KBD2_KBD_MASK0                      0x00408920 /* KEBOARD INPUT MASK 0 */
#define BCHP_KBD2_KBD_MASK1                      0x00408924 /* KEBOARD INPUT MASK 1 */
#define BCHP_KBD2_KBD_MASK2                      0x00408928 /* KEBOARD INPUT MASK 2 */
#define BCHP_KBD2_KBD_PAT0                       0x0040892c /* KEBOARD INPUT PATTERN MATCH 0 */
#define BCHP_KBD2_KBD_PAT1                       0x00408930 /* KEBOARD INPUT PATTERN MATCH 1 */
#define BCHP_KBD2_KBD_PAT2                       0x00408934 /* KEBOARD INPUT PATTERN MATCH 2 */
#define BCHP_KBD2_STATUS2                        0x0040893c /* KEYBOARD/REMOTE RECEIVER STATUS2 REGISTER */

#endif /* #ifndef BCHP_KBD2_H__ */

/* End of File */