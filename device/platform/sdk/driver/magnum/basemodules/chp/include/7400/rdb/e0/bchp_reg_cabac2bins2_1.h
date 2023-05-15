/***************************************************************************
 *     Copyright (c) 1999-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bchp_reg_cabac2bins2_1.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 1/9/08 11:24a $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Wed Jan  9 08:56:51 2008
 *                 MD5 Checksum         847dc12a9d71c4c68a648bbf19a883e3
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7400/rdb/e0/bchp_reg_cabac2bins2_1.h $
 * 
 * Hydra_Software_Devel/1   1/9/08 11:24a rpan
 * PR38572: Initial revision.
 *
 ***************************************************************************/

#ifndef BCHP_REG_CABAC2BINS2_1_H__
#define BCHP_REG_CABAC2BINS2_1_H__

/***************************************************************************
 *REG_CABAC2BINS2_1
 ***************************************************************************/
#define BCHP_REG_CABAC2BINS2_1_REG_CABAC2BINS_RD_BUFF_ADDR 0x00e02588 /* REG_CABAC2BINS_RD_BUFF_ADDR */
#define BCHP_REG_CABAC2BINS2_1_REG_CABAC2BINS_RD_BUFF_CTL 0x00e0258c /* REG_CABAC2BINS_RD_BUFF_CTL */
#define BCHP_REG_CABAC2BINS2_1_REG_CABAC2BINS_RD_BUFF_START_ADDR 0x00e02594 /* REG_CABAC2BINS_RD_BUFF_START_ADDR */
#define BCHP_REG_CABAC2BINS2_1_REG_CABAC2BINS_RD_BUFF_END_ADDR 0x00e02598 /* REG_CABAC2BINS_RD_BUFF_END_ADDR */
#define BCHP_REG_CABAC2BINS2_1_REG_CABAC2BINS_RD_BUFF_STALL_CNT 0x00e0259c /* REG_CABAC2BINS_RD_BUFF_STALL_CNT */
#define BCHP_REG_CABAC2BINS2_1_REG_CABAC2BINS_WR_BUFF_START_ADDR 0x00e025a8 /* REG_CABAC2BINS_WR_BUFF_START_ADDR */
#define BCHP_REG_CABAC2BINS2_1_REG_CABAC2BINS_WR_BUFF_CTL 0x00e025ac /* REG_CABAC2BINS_WR_BUFF_CTL */
#define BCHP_REG_CABAC2BINS2_1_REG_CABAC2BINS_WR_BUFF_END_ADDR 0x00e025b0 /* REG_CABAC2BINS_WR_BUFF_END_ADDR */
#define BCHP_REG_CABAC2BINS2_1_REG_CABAC2BINS_WR_BUFF_MARK_ADDR 0x00e025b4 /* REG_CABAC2BINS_WR_BUFF_MARK_ADDR */
#define BCHP_REG_CABAC2BINS2_1_REG_CABAC2BINS_WR_BUFF_STALL_CNT 0x00e025b8 /* REG_CABAC2BINS_WR_BUFF_STALL_CNT */
#define BCHP_REG_CABAC2BINS2_1_REG_CABAC2BINS_WR_BUFF_ADDR 0x00e025bc /* REG_CABAC2BINS_WR_BUFF_ADDR */
#define BCHP_REG_CABAC2BINS2_1_REG_CABAC2BINS_CHANNEL_ID 0x00e025c0 /* REG_CABAC2BINS_CHANNEL_ID */
#define BCHP_REG_CABAC2BINS2_1_REG_CABAC2BINS_NOT_IDLE_CYCLES 0x00e02620 /* REG_CABAC2BINS_NOT_IDLE_CYCLES */
#define BCHP_REG_CABAC2BINS2_1_REG_CABAC2BINS_STATE0 0x00e02630 /* REG_CABAC2BINS_STATE0 */
#define BCHP_REG_CABAC2BINS2_1_REG_CABAC2BINS_STATE1 0x00e02634 /* REG_CABAC2BINS_STATE1 */
#define BCHP_REG_CABAC2BINS2_1_REG_CABAC2BINS_INIT_TBL_CTL 0x00e02640 /* REG_CABAC2BINS_INIT_TBL_CTL */
#define BCHP_REG_CABAC2BINS2_1_REG_CABAC2BINS_PICTURE_COMMAND_ADDR 0x00e02710 /* REG_CABAC2BINS_PICTURE_COMMAND_ADDR */
#define BCHP_REG_CABAC2BINS2_1_REG_CABAC2BINS_LITTLE_ENDIAN 0x00e02714 /* REG_CABAC2BINS_LITTLE_ENDIAN */
#define BCHP_REG_CABAC2BINS2_1_REG_CABAC2BINS_PICTURE_STATUS 0x00e02718 /* REG_CABAC2BINS_PICTURE_STATUS */
#define BCHP_REG_CABAC2BINS2_1_REG_CABAC2BINS_CTL 0x00e0272c /* REG_CABAC2BINS_CTL */
#define BCHP_REG_CABAC2BINS2_1_REG_CABAC2BINS_END_END_1 0x00e027fc /* REG_CABAC2BINS_END_END_1 */

/***************************************************************************
 *REG_CABAC2BINS_CHANNEL_WR_POSITION_%i - REG_CABAC2BINS_CHANNEL_WR_POSITION_0..31
 ***************************************************************************/
#define BCHP_REG_CABAC2BINS2_1_REG_CABAC2BINS_CHANNEL_WR_POSITION_i_ARRAY_BASE 0x00e02400
#define BCHP_REG_CABAC2BINS2_1_REG_CABAC2BINS_CHANNEL_WR_POSITION_i_ARRAY_START 0
#define BCHP_REG_CABAC2BINS2_1_REG_CABAC2BINS_CHANNEL_WR_POSITION_i_ARRAY_END 31
#define BCHP_REG_CABAC2BINS2_1_REG_CABAC2BINS_CHANNEL_WR_POSITION_i_ARRAY_ELEMENT_SIZE 32

/***************************************************************************
 *REG_CABAC2BINS_CHANNEL_WR_POSITION_%i - REG_CABAC2BINS_CHANNEL_WR_POSITION_0..31
 ***************************************************************************/
/* REG_CABAC2BINS2_1 :: REG_CABAC2BINS_CHANNEL_WR_POSITION_i :: Addr [31:00] */
#define BCHP_REG_CABAC2BINS2_1_REG_CABAC2BINS_CHANNEL_WR_POSITION_i_Addr_MASK 0xffffffff
#define BCHP_REG_CABAC2BINS2_1_REG_CABAC2BINS_CHANNEL_WR_POSITION_i_Addr_SHIFT 0


#endif /* #ifndef BCHP_REG_CABAC2BINS2_1_H__ */

/* End of File */