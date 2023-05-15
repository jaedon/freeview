/***************************************************************************
 *     Copyright (c) 1999-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bchp_sid_huff_symb.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 2/28/12 6:12p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Tue Feb 28 11:03:19 2012
 *                 MD5 Checksum         d41d8cd98f00b204e9800998ecf8427e
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7435/rdb/b0/bchp_sid_huff_symb.h $
 * 
 * Hydra_Software_Devel/1   2/28/12 6:12p tdo
 * SW7435-40: Need 7435B0 public/central RDB (magnum) headers checked into
 * clearcase
 *
 ***************************************************************************/

#ifndef BCHP_SID_HUFF_SYMB_H__
#define BCHP_SID_HUFF_SYMB_H__

/***************************************************************************
 *SID_HUFF_SYMB
 ***************************************************************************/
#define BCHP_SID_HUFF_SYMB_START_HS              0x00bc3000 /* HuffmanSymbRAM */
#define BCHP_SID_HUFF_SYMB_END_HS                0x00bc37fc /* HuffmanSymbRAM */

/***************************************************************************
 *START_HS - HuffmanSymbRAM
 ***************************************************************************/
/* SID_HUFF_SYMB :: START_HS :: reserved0 [31:09] */
#define BCHP_SID_HUFF_SYMB_START_HS_reserved0_MASK                 0xfffffe00
#define BCHP_SID_HUFF_SYMB_START_HS_reserved0_SHIFT                9

/* SID_HUFF_SYMB :: START_HS :: CODE [08:00] */
#define BCHP_SID_HUFF_SYMB_START_HS_CODE_MASK                      0x000001ff
#define BCHP_SID_HUFF_SYMB_START_HS_CODE_SHIFT                     0

/***************************************************************************
 *END_HS - HuffmanSymbRAM
 ***************************************************************************/
/* SID_HUFF_SYMB :: END_HS :: reserved0 [31:09] */
#define BCHP_SID_HUFF_SYMB_END_HS_reserved0_MASK                   0xfffffe00
#define BCHP_SID_HUFF_SYMB_END_HS_reserved0_SHIFT                  9

/* SID_HUFF_SYMB :: END_HS :: CODE [08:00] */
#define BCHP_SID_HUFF_SYMB_END_HS_CODE_MASK                        0x000001ff
#define BCHP_SID_HUFF_SYMB_END_HS_CODE_SHIFT                       0

#endif /* #ifndef BCHP_SID_HUFF_SYMB_H__ */

/* End of File */