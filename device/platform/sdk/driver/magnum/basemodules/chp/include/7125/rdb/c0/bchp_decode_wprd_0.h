/***************************************************************************
 *     Copyright (c) 1999-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bchp_decode_wprd_0.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 1/25/10 8:01p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Fri Jan 22 20:18:25 2010
 *                 MD5 Checksum         a2d1f2163f65e87d228a0fb491cb442d
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7125/rdb/c0/bchp_decode_wprd_0.h $
 * 
 * Hydra_Software_Devel/1   1/25/10 8:01p albertl
 * SW7125-177: Initial revision.
 *
 ***************************************************************************/

#ifndef BCHP_DECODE_WPRD_0_H__
#define BCHP_DECODE_WPRD_0_H__

/***************************************************************************
 *DECODE_WPRD_0
 ***************************************************************************/
#define BCHP_DECODE_WPRD_0_REG_WPRD_CTL          0x00000340 /* Weighted Prediction Control */
#define BCHP_DECODE_WPRD_0_REG_WPRD_END          0x0000035c /* REG_WPRD_END */

/***************************************************************************
 *REG_WPRD_CTL - Weighted Prediction Control
 ***************************************************************************/
/* DECODE_WPRD_0 :: REG_WPRD_CTL :: reserved0 [31:15] */
#define BCHP_DECODE_WPRD_0_REG_WPRD_CTL_reserved0_MASK             0xffff8000
#define BCHP_DECODE_WPRD_0_REG_WPRD_CTL_reserved0_SHIFT            15

/* DECODE_WPRD_0 :: REG_WPRD_CTL :: ChromaDenom [14:12] */
#define BCHP_DECODE_WPRD_0_REG_WPRD_CTL_ChromaDenom_MASK           0x00007000
#define BCHP_DECODE_WPRD_0_REG_WPRD_CTL_ChromaDenom_SHIFT          12

/* DECODE_WPRD_0 :: REG_WPRD_CTL :: reserved1 [11:11] */
#define BCHP_DECODE_WPRD_0_REG_WPRD_CTL_reserved1_MASK             0x00000800
#define BCHP_DECODE_WPRD_0_REG_WPRD_CTL_reserved1_SHIFT            11

/* DECODE_WPRD_0 :: REG_WPRD_CTL :: LumDenom [10:08] */
#define BCHP_DECODE_WPRD_0_REG_WPRD_CTL_LumDenom_MASK              0x00000700
#define BCHP_DECODE_WPRD_0_REG_WPRD_CTL_LumDenom_SHIFT             8

/* DECODE_WPRD_0 :: REG_WPRD_CTL :: reserved2 [07:02] */
#define BCHP_DECODE_WPRD_0_REG_WPRD_CTL_reserved2_MASK             0x000000fc
#define BCHP_DECODE_WPRD_0_REG_WPRD_CTL_reserved2_SHIFT            2

/* DECODE_WPRD_0 :: REG_WPRD_CTL :: PredType [01:00] */
#define BCHP_DECODE_WPRD_0_REG_WPRD_CTL_PredType_MASK              0x00000003
#define BCHP_DECODE_WPRD_0_REG_WPRD_CTL_PredType_SHIFT             0

/***************************************************************************
 *REG_WPRD_END - REG_WPRD_END
 ***************************************************************************/
/* DECODE_WPRD_0 :: REG_WPRD_END :: reserved0 [31:00] */
#define BCHP_DECODE_WPRD_0_REG_WPRD_END_reserved0_MASK             0xffffffff
#define BCHP_DECODE_WPRD_0_REG_WPRD_END_reserved0_SHIFT            0

#endif /* #ifndef BCHP_DECODE_WPRD_0_H__ */

/* End of File */