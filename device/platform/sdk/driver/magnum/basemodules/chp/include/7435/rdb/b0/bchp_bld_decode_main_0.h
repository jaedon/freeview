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
 * $brcm_Workfile: bchp_bld_decode_main_0.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 2/28/12 3:12p $
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
 * $brcm_Log: /magnum/basemodules/chp/7435/rdb/b0/bchp_bld_decode_main_0.h $
 * 
 * Hydra_Software_Devel/1   2/28/12 3:12p tdo
 * SW7435-40: Need 7435B0 public/central RDB (magnum) headers checked into
 * clearcase
 *
 ***************************************************************************/

#ifndef BCHP_BLD_DECODE_MAIN_0_H__
#define BCHP_BLD_DECODE_MAIN_0_H__

/***************************************************************************
 *BLD_DECODE_MAIN_0
 ***************************************************************************/
#define BCHP_BLD_DECODE_MAIN_0_REG_MAINCTL       0x00b00100 /* Decoder Control */
#define BCHP_BLD_DECODE_MAIN_0_REG_FRAMESIZE     0x00b00104 /* Size of the picture being decoded */
#define BCHP_BLD_DECODE_MAIN_0_REG_DEC_VERSION   0x00b00108 /* Version of the decoder core */
#define BCHP_BLD_DECODE_MAIN_0_REG_STATUS        0x00b00110 /* Provides back-end decoder processing status */
#define BCHP_BLD_DECODE_MAIN_0_REG_PMONCTL       0x00b00120 /* Performance Monitoring */
#define BCHP_BLD_DECODE_MAIN_0_REG_PMONCNT0      0x00b00124 /* REG_PMONCNT0 */
#define BCHP_BLD_DECODE_MAIN_0_REG_PMONCNT1      0x00b00128 /* REG_PMONCNT1 */
#define BCHP_BLD_DECODE_MAIN_0_REG_PMON_MBCTL    0x00b0012c /* REG_PMON_MBCTL */
#define BCHP_BLD_DECODE_MAIN_0_DBLK_BUFF_CONTROL 0x00b00130 /* DBLK_BUFF_CONTROL */
#define BCHP_BLD_DECODE_MAIN_0_CRC_CONTROL       0x00b00134 /* DBLK CRC CONTROL register */
#define BCHP_BLD_DECODE_MAIN_0_CRC_SEED          0x00b00138 /* DBLK CRC SEED register */
#define BCHP_BLD_DECODE_MAIN_0_CRC_CHKSUM_Y      0x00b0013c /* DBLK Luma CRC/Checksum result register */
#define BCHP_BLD_DECODE_MAIN_0_CRC_CHKSUM_CB     0x00b00140 /* DBLK Chroma (Cb) CRC/Checksum result register */
#define BCHP_BLD_DECODE_MAIN_0_CRC_CHKSUM_CR     0x00b00144 /* DBLK Chroma (Cr) CRC/Checksum result register */
#define BCHP_BLD_DECODE_MAIN_0_REG_VP6_MCOM_CONTROL 0x00b00150 /* VP6 Mocomp Control */
#define BCHP_BLD_DECODE_MAIN_0_REG_VP6_MCOM_AUTO 0x00b00154 /* VP6 Mocomp Auto Filter Selection */
#define BCHP_BLD_DECODE_MAIN_0_REG_VP6_MCOM_ALPHA 0x00b00158 /* VP6 Mocomp Alpha Filter Selection */
#define BCHP_BLD_DECODE_MAIN_0_REG_VP6_MCOM_FLIMIT 0x00b0015c /* VP6 Mocomp Flimit Control */
#define BCHP_BLD_DECODE_MAIN_0_REG_BACKEND_DEBUG 0x00b00160 /* Backend debug Select */
#define BCHP_BLD_DECODE_MAIN_0_REG_VC1_MC_DEBUG  0x00b00164 /* VC1 Mocomp Debug */
#define BCHP_BLD_DECODE_MAIN_0_REG_QPEL_FIFO_DEBUG 0x00b00168 /* Qpel FIFO Debug */
#define BCHP_BLD_DECODE_MAIN_0_REG_MAIN_END      0x00b001fc /* REG_MAIN_END */

#endif /* #ifndef BCHP_BLD_DECODE_MAIN_0_H__ */

/* End of File */