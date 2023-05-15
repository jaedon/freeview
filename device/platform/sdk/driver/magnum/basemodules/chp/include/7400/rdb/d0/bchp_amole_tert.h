/***************************************************************************
 *     Copyright (c) 1999-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bchp_amole_tert.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 7/23/07 1:11p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Mon Jul 23 12:02:45 2007
 *                 MD5 Checksum         61f9c4d8dcdcd06017506dddbf23f434
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008004
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7400/rdb/d0/bchp_amole_tert.h $
 * 
 * Hydra_Software_Devel/1   7/23/07 1:11p maivu
 * PR 32842: Initial revision
 *
 ***************************************************************************/

#ifndef BCHP_AMOLE_TERT_H__
#define BCHP_AMOLE_TERT_H__

/***************************************************************************
 *AMOLE_TERT - TERT AMOL Encoder
 ***************************************************************************/
#define BCHP_AMOLE_TERT_REVID                    0x00188500 /* AMOL Encoder Revision ID Register */
#define BCHP_AMOLE_TERT_CONTROL                  0x00188508 /* Control Register */
#define BCHP_AMOLE_TERT_ACT_LINE_BASE_TOP        0x0018850c /* Top Field Active Line and Base Register */
#define BCHP_AMOLE_TERT_ACT_LINE_BASE_BOT        0x00188510 /* Bottom Field Active Line and Base Register */
#define BCHP_AMOLE_TERT_GAIN_PED_TOP             0x00188514 /* Top Field Gain and Pedestal Enable Register */
#define BCHP_AMOLE_TERT_GAIN_PED_BOT             0x00188518 /* Bottom Field Gain and Pedestal Enable Register */
#define BCHP_AMOLE_TERT_DATA_WORD0_BANK0         0x0018851c /* Bank 0 Word 0 Data Register */
#define BCHP_AMOLE_TERT_DATA_WORD1_BANK0         0x00188520 /* Bank 0 Word 1 Data Register */
#define BCHP_AMOLE_TERT_DATA_WORD2_BANK0         0x00188524 /* Bank 0 Word 2 Data Register */
#define BCHP_AMOLE_TERT_DATA_WORD3_BANK0         0x00188528 /* Bank 0 Word 3 Data Register */
#define BCHP_AMOLE_TERT_DATA_WORD4_BANK0         0x0018852c /* Bank 0 Word 4 Data Register */
#define BCHP_AMOLE_TERT_DATA_WORD5_BANK0         0x00188530 /* Bank 0 Word 5 Data Register */
#define BCHP_AMOLE_TERT_DATA_WORD0_BANK1         0x00188534 /* Bank 1 Word 0 Data Register */
#define BCHP_AMOLE_TERT_DATA_WORD1_BANK1         0x00188538 /* Bank 1 Word 1 Data Register */
#define BCHP_AMOLE_TERT_DATA_WORD2_BANK1         0x0018853c /* Bank 1 Word 2 Data Register */
#define BCHP_AMOLE_TERT_DATA_WORD3_BANK1         0x00188540 /* Bank 1 Word 3 Data Register */
#define BCHP_AMOLE_TERT_DATA_WORD4_BANK1         0x00188544 /* Bank 1 Word 4 Data Register */
#define BCHP_AMOLE_TERT_DATA_WORD5_BANK1         0x00188548 /* Bank 1 Word 5 Data Register */
#define BCHP_AMOLE_TERT_DATA_WORD0_BANK2         0x0018854c /* Bank 2 Word 0 Data Register */
#define BCHP_AMOLE_TERT_DATA_WORD1_BANK2         0x00188550 /* Bank 2 Word 1 Data Register */
#define BCHP_AMOLE_TERT_DATA_WORD2_BANK2         0x00188554 /* Bank 2 Word 2 Data Register */
#define BCHP_AMOLE_TERT_DATA_WORD3_BANK2         0x00188558 /* Bank 2 Word 3 Data Register */
#define BCHP_AMOLE_TERT_DATA_WORD4_BANK2         0x0018855c /* Bank 2 Word 4 Data Register */
#define BCHP_AMOLE_TERT_DATA_WORD5_BANK2         0x00188560 /* Bank 2 Word 5 Data Register */
#define BCHP_AMOLE_TERT_DATA_WORD0_BANK3         0x00188564 /* Bank 3 Word 0 Data Register */
#define BCHP_AMOLE_TERT_DATA_WORD1_BANK3         0x00188568 /* Bank 3 Word 1 Data Register */
#define BCHP_AMOLE_TERT_DATA_WORD2_BANK3         0x0018856c /* Bank 3 Word 2 Data Register */
#define BCHP_AMOLE_TERT_DATA_WORD3_BANK3         0x00188570 /* Bank 3 Word 3 Data Register */
#define BCHP_AMOLE_TERT_DATA_WORD4_BANK3         0x00188574 /* Bank 3 Word 4 Data Register */
#define BCHP_AMOLE_TERT_DATA_WORD5_BANK3         0x00188578 /* Bank 3 Word 5 Data Register */
#define BCHP_AMOLE_TERT_BYTES_PER_LINE           0x0018857c /* Bytes Per Line */
#define BCHP_AMOLE_TERT_NULL                     0x00188580 /* NULL Register */
#define BCHP_AMOLE_TERT_WRPTR                    0x00188584 /* Write Pointer Register */
#define BCHP_AMOLE_TERT_RDPTR                    0x00188588 /* Read Pointer Register */
#define BCHP_AMOLE_TERT_STATUS                   0x0018858c /* Status Register */

#endif /* #ifndef BCHP_AMOLE_TERT_H__ */

/* End of File */