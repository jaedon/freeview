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
 * $brcm_Date: 6/14/12 10:48a $
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
 * $brcm_Log: /magnum/basemodules/chp/7429/rdb/b0/bchp_bld_decode_main_0.h $
 * 
 * Hydra_Software_Devel/1   6/14/12 10:48a pntruong
 * SW7429-185: Initial version.
 *
 ***************************************************************************/

#ifndef BCHP_BLD_DECODE_MAIN_0_H__
#define BCHP_BLD_DECODE_MAIN_0_H__

/***************************************************************************
 *BLD_DECODE_MAIN_0
 ***************************************************************************/
#define BCHP_BLD_DECODE_MAIN_0_REG_MAINCTL       0x00100100 /* Decoder Control */
#define BCHP_BLD_DECODE_MAIN_0_REG_FRAMESIZE     0x00100104 /* Size of the picture being decoded */
#define BCHP_BLD_DECODE_MAIN_0_REG_DEC_VERSION   0x00100108 /* Version of the decoder core */
#define BCHP_BLD_DECODE_MAIN_0_REG_STATUS        0x00100110 /* Provides back-end decoder processing status */
#define BCHP_BLD_DECODE_MAIN_0_REG_PMONCTL       0x00100120 /* Performance Monitoring */
#define BCHP_BLD_DECODE_MAIN_0_REG_PMONCNT0      0x00100124 /* REG_PMONCNT0 */
#define BCHP_BLD_DECODE_MAIN_0_REG_PMONCNT1      0x00100128 /* REG_PMONCNT1 */
#define BCHP_BLD_DECODE_MAIN_0_REG_PMON_MBCTL    0x0010012c /* REG_PMON_MBCTL */
#define BCHP_BLD_DECODE_MAIN_0_DBLK_BUFF_CONTROL 0x00100130 /* DBLK_BUFF_CONTROL */
#define BCHP_BLD_DECODE_MAIN_0_CRC_CONTROL       0x00100134 /* DBLK CRC CONTROL register */
#define BCHP_BLD_DECODE_MAIN_0_CRC_SEED          0x00100138 /* DBLK CRC SEED register */
#define BCHP_BLD_DECODE_MAIN_0_CRC_CHKSUM_Y      0x0010013c /* DBLK Luma CRC/Checksum result register */
#define BCHP_BLD_DECODE_MAIN_0_CRC_CHKSUM_CB     0x00100140 /* DBLK Chroma (Cb) CRC/Checksum result register */
#define BCHP_BLD_DECODE_MAIN_0_CRC_CHKSUM_CR     0x00100144 /* DBLK Chroma (Cr) CRC/Checksum result register */
#define BCHP_BLD_DECODE_MAIN_0_REG_MAIN_END      0x001001fc /* REG_MAIN_END */

#endif /* #ifndef BCHP_BLD_DECODE_MAIN_0_H__ */

/* End of File */