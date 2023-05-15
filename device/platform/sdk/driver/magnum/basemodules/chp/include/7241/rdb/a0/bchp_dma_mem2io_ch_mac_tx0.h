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
 * $brcm_Workfile: bchp_dma_mem2io_ch_mac_tx0.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 8/17/11 1:06p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Thu Aug 11 21:40:01 2011
 *                 MD5 Checksum         30eb4d41dba6899590ea7664187e6ba5
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7429/rdb/a0/bchp_dma_mem2io_ch_mac_tx0.h $
 * 
 * Hydra_Software_Devel/1   8/17/11 1:06p pntruong
 * SW7429-2: Initial version.
 *
 ***************************************************************************/

#ifndef BCHP_DMA_MEM2IO_CH_MAC_TX0_H__
#define BCHP_DMA_MEM2IO_CH_MAC_TX0_H__

/***************************************************************************
 *DMA_MEM2IO_CH_MAC_TX0 - DMA_MEM2IO_CH registers MAC TX0
 ***************************************************************************/
#define BCHP_DMA_MEM2IO_CH_MAC_TX0_LLM_CMD_ADDR  0x00bf6000 /* LLM Command Address */
#define BCHP_DMA_MEM2IO_CH_MAC_TX0_DESC_BASE_ADDR 0x00bf6004 /* The physical base address of the descriptors in memory */
#define BCHP_DMA_MEM2IO_CH_MAC_TX0_BUFF_BASE_ADDR 0x00bf6008 /* The physical base address of the buffers in  memory */
#define BCHP_DMA_MEM2IO_CH_MAC_TX0_CH_CONTROL    0x00bf600c /* Channel Control Register */
#define BCHP_DMA_MEM2IO_CH_MAC_TX0_LLM_CMD       0x00bf6010 /* "LLM Command Register.,Contains set of commands/parameters DMA should pass to LLM while issuing various LLM commands." */
#define BCHP_DMA_MEM2IO_CH_MAC_TX0_DEBUG_REG1    0x00bf6014 /* Debug Register #1 */
#define BCHP_DMA_MEM2IO_CH_MAC_TX0_DEBUG_REG2    0x00bf6018 /* Debug Register #2 */
#define BCHP_DMA_MEM2IO_CH_MAC_TX0_DEBUG_REG3    0x00bf601c /* Debug Register #3 */
#define BCHP_DMA_MEM2IO_CH_MAC_TX0_DEBUG_REG4    0x00bf6020 /* "Debug Register #4,Reflects parameters of fetched descriptor" */
#define BCHP_DMA_MEM2IO_CH_MAC_TX0_DEBUG_REG5    0x00bf6024 /* "Debug Register #5,Reflects parameters of fetched descriptor" */
#define BCHP_DMA_MEM2IO_CH_MAC_TX0_DEBUG_REG6    0x00bf6028 /* "Debug Register #6,Reflects parameters of fetched descriptor" */

#endif /* #ifndef BCHP_DMA_MEM2IO_CH_MAC_TX0_H__ */

/* End of File */