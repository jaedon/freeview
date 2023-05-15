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
 * $brcm_Workfile: bchp_dma_io2mem_subch_mac_rx_subch9.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 6/14/12 12:15p $
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
 * $brcm_Log: /magnum/basemodules/chp/7429/rdb/b0/bchp_dma_io2mem_subch_mac_rx_subch9.h $
 * 
 * Hydra_Software_Devel/1   6/14/12 12:15p pntruong
 * SW7429-185: Initial version.
 *
 ***************************************************************************/

#ifndef BCHP_DMA_IO2MEM_SUBCH_MAC_RX_SUBCH9_H__
#define BCHP_DMA_IO2MEM_SUBCH_MAC_RX_SUBCH9_H__

/***************************************************************************
 *DMA_IO2MEM_SUBCH_MAC_RX_SUBCH9 - DMA_IO2MEM_SUBCH registers MAC RX SUBCH9
 ***************************************************************************/
#define BCHP_DMA_IO2MEM_SUBCH_MAC_RX_SUBCH9_SUBCH_CONTROL 0x00bf4280 /* Subchannel Control Register */
#define BCHP_DMA_IO2MEM_SUBCH_MAC_RX_SUBCH9_LLM_CMD 0x00bf4284 /* "LLM Command Register.,Contains set of commands/parameters DMA should pass to LLM while issuing various LLM commands." */
#define BCHP_DMA_IO2MEM_SUBCH_MAC_RX_SUBCH9_FREE_DESC_REQ_NUM 0x00bf4288 /* Free Descriptors Information Register */
#define BCHP_DMA_IO2MEM_SUBCH_MAC_RX_SUBCH9_DESC_THRESHOLD 0x00bf428c /* Descriptor Threshold Register. */
#define BCHP_DMA_IO2MEM_SUBCH_MAC_RX_SUBCH9_FC_THRESHOLD 0x00bf4290 /* Flow Control threshold Register */
#define BCHP_DMA_IO2MEM_SUBCH_MAC_RX_SUBCH9_BUFF_SIZE 0x00bf4294 /* "Buffer Size.,Indicates a buffer size multiplicator accociated with specific sub channel. Given in a units of 64 Bytes." */
#define BCHP_DMA_IO2MEM_SUBCH_MAC_RX_SUBCH9_DROP_PACKET_STAT 0x00bf4298 /* Dropped Packets Statistics Status Register */
#define BCHP_DMA_IO2MEM_SUBCH_MAC_RX_SUBCH9_DROP_PACKET_STAT_RESET 0x00bf429c /* Reset Dropped Packets Statistics Register */
#define BCHP_DMA_IO2MEM_SUBCH_MAC_RX_SUBCH9_DEBUG_REG1 0x00bf42a0 /* Debug Register #1 */
#define BCHP_DMA_IO2MEM_SUBCH_MAC_RX_SUBCH9_DEBUG_REG2 0x00bf42a4 /* Debug Register #2 */
#define BCHP_DMA_IO2MEM_SUBCH_MAC_RX_SUBCH9_DEBUG_REG3 0x00bf42a8 /* Debug Register #3 */
#define BCHP_DMA_IO2MEM_SUBCH_MAC_RX_SUBCH9_DEBUG_REG4 0x00bf42ac /* Debug Register #4 */

#endif /* #ifndef BCHP_DMA_IO2MEM_SUBCH_MAC_RX_SUBCH9_H__ */

/* End of File */