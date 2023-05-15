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
 * $brcm_Workfile: bchp_dma_io2mem_ch_phy0_global.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 1/19/12 1:49p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Tue Jan 17 13:26:47 2012
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
 * $brcm_Log: /magnum/basemodules/chp/7425/rdb/b2/bchp_dma_io2mem_ch_phy0_global.h $
 * 
 * Hydra_Software_Devel/1   1/19/12 1:49p vanessah
 * SW7425-2202: add B2 left out files
 *
 ***************************************************************************/

#ifndef BCHP_DMA_IO2MEM_CH_PHY0_GLOBAL_H__
#define BCHP_DMA_IO2MEM_CH_PHY0_GLOBAL_H__

/***************************************************************************
 *DMA_IO2MEM_CH_PHY0_Global - DMA_IO2MEM_CH registers PHY0 Global
 ***************************************************************************/
#define BCHP_DMA_IO2MEM_CH_PHY0_Global_LLM_CMD_ADDR 0x00ff4800 /* LLM Command Address */
#define BCHP_DMA_IO2MEM_CH_PHY0_Global_DESC_BASE_ADDR 0x00ff4804 /* The physical base address of the DMA descriptors in memory */
#define BCHP_DMA_IO2MEM_CH_PHY0_Global_BUFF_BASE_ADDR 0x00ff4808 /* The physical base address of the buffers in  memory */
#define BCHP_DMA_IO2MEM_CH_PHY0_Global_LLM_POOL_PERIOD 0x00ff480c /* "The period for repeatedly issuing a command to LLM in case of a DMA descriptors starvation situation.,The period is in DMA clock cycle units.,Its typical value suppose to match 100us period time" */
#define BCHP_DMA_IO2MEM_CH_PHY0_Global_CH_CONTROL 0x00ff4810 /* Channel Control Register */
#define BCHP_DMA_IO2MEM_CH_PHY0_Global_DEBUG_REG 0x00ff4814 /* Debug Register */

#endif /* #ifndef BCHP_DMA_IO2MEM_CH_PHY0_GLOBAL_H__ */

/* End of File */