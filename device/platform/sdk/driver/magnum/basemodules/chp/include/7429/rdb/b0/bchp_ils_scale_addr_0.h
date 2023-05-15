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
 * $brcm_Workfile: bchp_ils_scale_addr_0.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 6/14/12 12:58p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Wed Jun 13 16:39:04 2012
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
 * $brcm_Log: /magnum/basemodules/chp/7429/rdb/b0/bchp_ils_scale_addr_0.h $
 * 
 * Hydra_Software_Devel/1   6/14/12 12:58p pntruong
 * SW7429-185: Initial version.
 *
 ***************************************************************************/

#ifndef BCHP_ILS_SCALE_ADDR_0_H__
#define BCHP_ILS_SCALE_ADDR_0_H__

/***************************************************************************
 *ILS_SCALE_ADDR_0 - Inter Layer Scaler Address Registers 0
 ***************************************************************************/
#define BCHP_ILS_SCALE_ADDR_0_ILS_ADDR_LUMA_X    0x00070100 /* Scaler Luma Starting Address X Register */
#define BCHP_ILS_SCALE_ADDR_0_ILS_ADDR_LUMA_Y    0x00070104 /* Scaler Luma Starting Address Y Register */
#define BCHP_ILS_SCALE_ADDR_0_ILS_ADDR_CHROMA_X  0x00070108 /* Scaler Chroma Starting Address X Register */
#define BCHP_ILS_SCALE_ADDR_0_ILS_ADDR_CHROMA_Y  0x0007010c /* Scaler Chroma Starting Address Y Register */

/***************************************************************************
 *ILS_ADDR_LUMA_X - Scaler Luma Starting Address X Register
 ***************************************************************************/
/* ILS_SCALE_ADDR_0 :: ILS_ADDR_LUMA_X :: XaddrLuma [31:00] */
#define BCHP_ILS_SCALE_ADDR_0_ILS_ADDR_LUMA_X_XaddrLuma_MASK       0xffffffff
#define BCHP_ILS_SCALE_ADDR_0_ILS_ADDR_LUMA_X_XaddrLuma_SHIFT      0

/***************************************************************************
 *ILS_ADDR_LUMA_Y - Scaler Luma Starting Address Y Register
 ***************************************************************************/
/* ILS_SCALE_ADDR_0 :: ILS_ADDR_LUMA_Y :: YaddrLuma [31:00] */
#define BCHP_ILS_SCALE_ADDR_0_ILS_ADDR_LUMA_Y_YaddrLuma_MASK       0xffffffff
#define BCHP_ILS_SCALE_ADDR_0_ILS_ADDR_LUMA_Y_YaddrLuma_SHIFT      0

/***************************************************************************
 *ILS_ADDR_CHROMA_X - Scaler Chroma Starting Address X Register
 ***************************************************************************/
/* ILS_SCALE_ADDR_0 :: ILS_ADDR_CHROMA_X :: XaddrChroma [31:00] */
#define BCHP_ILS_SCALE_ADDR_0_ILS_ADDR_CHROMA_X_XaddrChroma_MASK   0xffffffff
#define BCHP_ILS_SCALE_ADDR_0_ILS_ADDR_CHROMA_X_XaddrChroma_SHIFT  0

/***************************************************************************
 *ILS_ADDR_CHROMA_Y - Scaler Chroma Starting Address Y Register
 ***************************************************************************/
/* ILS_SCALE_ADDR_0 :: ILS_ADDR_CHROMA_Y :: YaddrChroma [31:00] */
#define BCHP_ILS_SCALE_ADDR_0_ILS_ADDR_CHROMA_Y_YaddrChroma_MASK   0xffffffff
#define BCHP_ILS_SCALE_ADDR_0_ILS_ADDR_CHROMA_Y_YaddrChroma_SHIFT  0

#endif /* #ifndef BCHP_ILS_SCALE_ADDR_0_H__ */

/* End of File */