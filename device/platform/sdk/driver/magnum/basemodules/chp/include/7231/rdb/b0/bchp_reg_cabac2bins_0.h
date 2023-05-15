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
 * $brcm_Workfile: bchp_reg_cabac2bins_0.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 4/13/11 6:46p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Wed Apr 13 13:19:58 2011
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
 * $brcm_Log: /magnum/basemodules/chp/7231/rdb/b0/bchp_reg_cabac2bins_0.h $
 * 
 * Hydra_Software_Devel/1   4/13/11 6:46p albertl
 * SW7231-123: Initial revision.
 *
 ***************************************************************************/

#ifndef BCHP_REG_CABAC2BINS_0_H__
#define BCHP_REG_CABAC2BINS_0_H__

/***************************************************************************
 *REG_CABAC2BINS_0
 ***************************************************************************/
#define BCHP_REG_CABAC2BINS_0_REG_CABAC2BINS_IMG__CTX_LAST 0x00000bbc /* REG_CABAC2BINS_IMG__CTX_LAST */
#define BCHP_REG_CABAC2BINS_0_REG_CABAC2BINS_CONTEXT_TABLE_BASE_ADDR 0x00000bd0 /* REG_CABAC2BINS_CONTEXT_TABLE_BASE_ADDR */
#define BCHP_REG_CABAC2BINS_0_REG_CABAC2BINS_END_END 0x00000bfc /* REG_CABAC2BINS_END_END */

/***************************************************************************
 *REG_CABAC2BINS_IMG__CTX_LAST - REG_CABAC2BINS_IMG__CTX_LAST
 ***************************************************************************/
/* REG_CABAC2BINS_0 :: REG_CABAC2BINS_IMG__CTX_LAST :: reserved0 [31:09] */
#define BCHP_REG_CABAC2BINS_0_REG_CABAC2BINS_IMG__CTX_LAST_reserved0_MASK 0xfffffe00
#define BCHP_REG_CABAC2BINS_0_REG_CABAC2BINS_IMG__CTX_LAST_reserved0_SHIFT 9

/* REG_CABAC2BINS_0 :: REG_CABAC2BINS_IMG__CTX_LAST :: CtxLast [08:00] */
#define BCHP_REG_CABAC2BINS_0_REG_CABAC2BINS_IMG__CTX_LAST_CtxLast_MASK 0x000001ff
#define BCHP_REG_CABAC2BINS_0_REG_CABAC2BINS_IMG__CTX_LAST_CtxLast_SHIFT 0

/***************************************************************************
 *REG_CABAC2BINS_CONTEXT_TABLE_BASE_ADDR - REG_CABAC2BINS_CONTEXT_TABLE_BASE_ADDR
 ***************************************************************************/
/* REG_CABAC2BINS_0 :: REG_CABAC2BINS_CONTEXT_TABLE_BASE_ADDR :: Addr [31:04] */
#define BCHP_REG_CABAC2BINS_0_REG_CABAC2BINS_CONTEXT_TABLE_BASE_ADDR_Addr_MASK 0xfffffff0
#define BCHP_REG_CABAC2BINS_0_REG_CABAC2BINS_CONTEXT_TABLE_BASE_ADDR_Addr_SHIFT 4

/* REG_CABAC2BINS_0 :: REG_CABAC2BINS_CONTEXT_TABLE_BASE_ADDR :: reserved0 [03:00] */
#define BCHP_REG_CABAC2BINS_0_REG_CABAC2BINS_CONTEXT_TABLE_BASE_ADDR_reserved0_MASK 0x0000000f
#define BCHP_REG_CABAC2BINS_0_REG_CABAC2BINS_CONTEXT_TABLE_BASE_ADDR_reserved0_SHIFT 0

/***************************************************************************
 *REG_CABAC2BINS_END_END - REG_CABAC2BINS_END_END
 ***************************************************************************/
/* REG_CABAC2BINS_0 :: REG_CABAC2BINS_END_END :: reserved0 [31:00] */
#define BCHP_REG_CABAC2BINS_0_REG_CABAC2BINS_END_END_reserved0_MASK 0xffffffff
#define BCHP_REG_CABAC2BINS_0_REG_CABAC2BINS_END_END_reserved0_SHIFT 0

#endif /* #ifndef BCHP_REG_CABAC2BINS_0_H__ */

/* End of File */