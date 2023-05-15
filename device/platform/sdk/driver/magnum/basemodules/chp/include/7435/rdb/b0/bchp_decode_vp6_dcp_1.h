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
 * $brcm_Workfile: bchp_decode_vp6_dcp_1.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 2/28/12 3:47p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Tue Feb 28 11:03:18 2012
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
 * $brcm_Log: /magnum/basemodules/chp/7435/rdb/b0/bchp_decode_vp6_dcp_1.h $
 * 
 * Hydra_Software_Devel/1   2/28/12 3:47p tdo
 * SW7435-40: Need 7435B0 public/central RDB (magnum) headers checked into
 * clearcase
 *
 ***************************************************************************/

#ifndef BCHP_DECODE_VP6_DCP_1_H__
#define BCHP_DECODE_VP6_DCP_1_H__

/***************************************************************************
 *DECODE_VP6_DCP_1
 ***************************************************************************/
#define BCHP_DECODE_VP6_DCP_1_REG_VP6_DCP        0x00000620 /* VP6 DC Prediction */
#define BCHP_DECODE_VP6_DCP_1_REG_VP6_DCP_END    0x0000062c /* REG_VP6_DCP_END */

/***************************************************************************
 *REG_VP6_DCP - VP6 DC Prediction
 ***************************************************************************/
/* DECODE_VP6_DCP_1 :: REG_VP6_DCP :: reserved0 [31:05] */
#define BCHP_DECODE_VP6_DCP_1_REG_VP6_DCP_reserved0_MASK           0xffffffe0
#define BCHP_DECODE_VP6_DCP_1_REG_VP6_DCP_reserved0_SHIFT          5

/* DECODE_VP6_DCP_1 :: REG_VP6_DCP :: startPic [04:04] */
#define BCHP_DECODE_VP6_DCP_1_REG_VP6_DCP_startPic_MASK            0x00000010
#define BCHP_DECODE_VP6_DCP_1_REG_VP6_DCP_startPic_SHIFT           4

/* DECODE_VP6_DCP_1 :: REG_VP6_DCP :: frameMode [03:02] */
#define BCHP_DECODE_VP6_DCP_1_REG_VP6_DCP_frameMode_MASK           0x0000000c
#define BCHP_DECODE_VP6_DCP_1_REG_VP6_DCP_frameMode_SHIFT          2

/* DECODE_VP6_DCP_1 :: REG_VP6_DCP :: useTopDC [01:01] */
#define BCHP_DECODE_VP6_DCP_1_REG_VP6_DCP_useTopDC_MASK            0x00000002
#define BCHP_DECODE_VP6_DCP_1_REG_VP6_DCP_useTopDC_SHIFT           1

/* DECODE_VP6_DCP_1 :: REG_VP6_DCP :: useLeftDC [00:00] */
#define BCHP_DECODE_VP6_DCP_1_REG_VP6_DCP_useLeftDC_MASK           0x00000001
#define BCHP_DECODE_VP6_DCP_1_REG_VP6_DCP_useLeftDC_SHIFT          0

/***************************************************************************
 *REG_VP6_DCP_END - REG_VP6_DCP_END
 ***************************************************************************/
/* DECODE_VP6_DCP_1 :: REG_VP6_DCP_END :: reserved0 [31:00] */
#define BCHP_DECODE_VP6_DCP_1_REG_VP6_DCP_END_reserved0_MASK       0xffffffff
#define BCHP_DECODE_VP6_DCP_1_REG_VP6_DCP_END_reserved0_SHIFT      0

#endif /* #ifndef BCHP_DECODE_VP6_DCP_1_H__ */

/* End of File */