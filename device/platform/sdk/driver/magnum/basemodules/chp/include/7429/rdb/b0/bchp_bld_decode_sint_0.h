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
 * $brcm_Workfile: bchp_bld_decode_sint_0.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 6/14/12 10:50a $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Wed Jun 13 16:39:03 2012
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
 * $brcm_Log: /magnum/basemodules/chp/7429/rdb/b0/bchp_bld_decode_sint_0.h $
 * 
 * Hydra_Software_Devel/1   6/14/12 10:50a pntruong
 * SW7429-185: Initial version.
 *
 ***************************************************************************/

#ifndef BCHP_BLD_DECODE_SINT_0_H__
#define BCHP_BLD_DECODE_SINT_0_H__

/***************************************************************************
 *BLD_DECODE_SINT_0
 ***************************************************************************/
#define BCHP_BLD_DECODE_SINT_0_REG_SINT_DMA_ADDR 0x00100c00 /* REG_SINT_DMA_ADDR */
#define BCHP_BLD_DECODE_SINT_0_REG_SINT_DMA_LEN  0x00100c04 /* REG_SINT_DMA_LEN */
#define BCHP_BLD_DECODE_SINT_0_REG_SINT_DMA_BASE 0x00100c08 /* REG_SINT_DMA_BASE */
#define BCHP_BLD_DECODE_SINT_0_REG_SINT_DMA_END  0x00100c0c /* REG_SINT_DMA_END */
#define BCHP_BLD_DECODE_SINT_0_REG_SINT_STRM_POS 0x00100c10 /* REG_SINT_STRM_POS */
#define BCHP_BLD_DECODE_SINT_0_REG_SINT_STRM_STAT 0x00100c14 /* REG_SINT_STRM_STAT */
#define BCHP_BLD_DECODE_SINT_0_REG_SINT_IENA     0x00100c18 /* REG_SINT_IENA */
#define BCHP_BLD_DECODE_SINT_0_REG_SINT_STRM_BITS 0x00100c1c /* REG_SINT_STRM_BITS */
#define BCHP_BLD_DECODE_SINT_0_REG_SINT_GET_SYMB 0x00100c20 /* REG_SINT_GET_SYMB */
#define BCHP_BLD_DECODE_SINT_0_REG_SINT_MPEG_DC  0x00100c24 /* REG_SINT_MPEG_DC */
#define BCHP_BLD_DECODE_SINT_0_REG_SINT_DO_RESID 0x00100c28 /* REG_SINT_DO_RESID */
#define BCHP_BLD_DECODE_SINT_0_REG_SINT_XNZERO   0x00100c2c /* REG_SINT_XNZERO */
#define BCHP_BLD_DECODE_SINT_0_REG_SINT_VEC_MBTYPE 0x00100c30 /* REG_SINT_VEC_MBTYPE */
#define BCHP_BLD_DECODE_SINT_0_REG_SINT_VEC_RESID 0x00100c34 /* REG_SINT_VEC_RESID */
#define BCHP_BLD_DECODE_SINT_0_REG_SINT_VEC_DMODE 0x00100c38 /* REG_SINT_VEC_DMODE */
#define BCHP_BLD_DECODE_SINT_0_REG_SINT_VEC_LDST_DMA 0x00100c3c /* REG_SINT_VEC_LDST_DMA */
#define BCHP_BLD_DECODE_SINT_0_REG_SINT_VEC_DO_CONST 0x00100c40 /* REG_SINT_VEC_DO_CONST */
#define BCHP_BLD_DECODE_SINT_0_REG_SINT_VEC_MVDIFF 0x00100c44 /* Deblocking Motion Vector Difference */
#define BCHP_BLD_DECODE_SINT_0_REG_SINT_VEC_REFIDX 0x00100c48 /* REG_SINT_VEC_REFIDX */
#define BCHP_BLD_DECODE_SINT_0_REG_SINT_VEC_TOPREF 0x00100c4c /* REG_SINT_VEC_TOPREF */
#define BCHP_BLD_DECODE_SINT_0_REG_SINT_VEC_TOPTOPREF 0x00100c50 /* REG_SINT_VEC_TOPTOPREF */
#define BCHP_BLD_DECODE_SINT_0_REG_SINT_VEC_COL_TYPE 0x00100c54 /* REG_SINT_VEC_COL_TYPE */
#define BCHP_BLD_DECODE_SINT_0_REG_SINT_VEC_COL_REFID 0x00100c58 /* REG_SINT_VEC_COL_REFID */
#define BCHP_BLD_DECODE_SINT_0_REG_SINT_VEC_TOPPIC 0x00100c5c /* REG_SINT_VEC_TOPPIC */
#define BCHP_BLD_DECODE_SINT_0_REG_SINT_VEC_VC1_INFO 0x00100c60 /* REG_SINT_VEC_VC1_INFO */
#define BCHP_BLD_DECODE_SINT_0_REG_SINT_VEC_REFPIC 0x00100c64 /* REG_SINT_VEC_REFPIC - H.264 only */
#define BCHP_BLD_DECODE_SINT_0_REG_SINT_VEC_COUNT 0x00100c68 /* REG_SINT_VEC_COUNT */
#define BCHP_BLD_DECODE_SINT_0_REG_SINT_VEC_MVD_FIFO 0x00100c6c /* REG_SINT_VEC_MVD_FIFO */
#define BCHP_BLD_DECODE_SINT_0_REG_SINT_DIVX_TABSEL 0x00100c70 /* REG_SINT_DIVX_TABSEL */
#define BCHP_BLD_DECODE_SINT_0_REG_SINT_CTL_AUX  0x00100c74 /* REG_SINT_CTL_AUX */
#define BCHP_BLD_DECODE_SINT_0_REG_SINT_VEC_MP_FLAG 0x00100c78 /* REG_SINT_VEC_MP_FLAG */
#define BCHP_BLD_DECODE_SINT_0_REG_SINT_VEC_REGEND 0x00100c7c /* REG_SINT_VEC_REGEND */
#define BCHP_BLD_DECODE_SINT_0_REG_SINT_CTL      0x00100c80 /* REG_SINT_CTL */
#define BCHP_BLD_DECODE_SINT_0_REG_SINT_VLC_TOPCTX 0x00100c84 /* REG_SINT_VLC_TOPCTX */
#define BCHP_BLD_DECODE_SINT_0_REG_SINT_SLICE_ID 0x00100c88 /* REG_SINT_SLICE_ID */
#define BCHP_BLD_DECODE_SINT_0_REG_SINT_QP       0x00100c8c /* REG_SINT_QP */
#define BCHP_BLD_DECODE_SINT_0_REG_SINT_VEC_IMG_SIZE 0x00100c90 /* REG_SINT_VEC_IMG_SIZE */
#define BCHP_BLD_DECODE_SINT_0_REG_SINT_DIRCTX_WR_ADDR 0x00100c94 /* REG_SINT_DIRCTX_WR_ADDR */
#define BCHP_BLD_DECODE_SINT_0_REG_SINT_TOPCTX_DATA 0x00100c98 /* REG_SINT_TOPCTX_DATA */
#define BCHP_BLD_DECODE_SINT_0_REG_SINT_XFER_SYMB 0x00100c9c /* REG_SINT_XFER_SYMB */
#define BCHP_BLD_DECODE_SINT_0_REG_SINT_SMODE_BASE 0x00100ca0 /* REG_SINT_SMODE_BASE */
#define BCHP_BLD_DECODE_SINT_0_REG_SINT_SMODE_LEFT 0x00100ca4 /* REG_SINT_SMODE_LEFT */
#define BCHP_BLD_DECODE_SINT_0_REG_SINT_SMODE_TOP 0x00100ca8 /* REG_SINT_SMODE_TOP */
#define BCHP_BLD_DECODE_SINT_0_REG_SINT_SMODE_END 0x00100cac /* REG_SINT_SMODE_END */
#define BCHP_BLD_DECODE_SINT_0_REG_SINT_CTX_INIT 0x00100cb0 /* REG_SINT_CTX_INIT */
#define BCHP_BLD_DECODE_SINT_0_REG_SINT_TOP_CTX  0x00100cb4 /* REG_SINT_TOP_CTX */
#define BCHP_BLD_DECODE_SINT_0_REG_SINT_VC1_TABSEL 0x00100cb8 /* REG_SINT_VC1_TABSEL */
#define BCHP_BLD_DECODE_SINT_0_REG_SINT_CNST_INTRA 0x00100cbc /* REG_SINT_CNST_INTRA */
#define BCHP_BLD_DECODE_SINT_0_REG_SINT_OPIC_MEM_BASE 0x00100cc0 /* Outpic Lookup */
#define BCHP_BLD_DECODE_SINT_0_REG_SINT_OPIC_MEM_END 0x00100cfc /* REG_SINT_OPIC_MEM_END */
#define BCHP_BLD_DECODE_SINT_0_REG_SINT_VEC_MEM_BASE 0x00100d00 /* Vector Memory */
#define BCHP_BLD_DECODE_SINT_0_REG_SINT_VEC_MEM_END 0x00100dfc /* REG_SINT_VEC_MEM_END */

#endif /* #ifndef BCHP_BLD_DECODE_SINT_0_H__ */

/* End of File */