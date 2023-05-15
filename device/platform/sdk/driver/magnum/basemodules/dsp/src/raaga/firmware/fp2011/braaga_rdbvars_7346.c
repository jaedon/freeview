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
 ***************************************************************************/

#include "bchp.h"
const uint32_t BDSP_IMG_system_rdbvars_array1[] = {
	0x00030000, /* IMEM_SIZE */
	0x00020000, /* DMEM_SIZE */
	0x00000256, /* UART_BAUD_RATE_DIV_FACTOR */
	0x20E6DA00, /* DSP_FREQUENCY */
	0x00000000, /* PLL_MACRO_SELECT_NA */
	0x00000000, /* USE_384FS_I2S_MCLK_FOR_32KHZ */
	/* RDB Addresses */
	0x10821080, /* DSP Subsystem UART Status */
	0x10821084, /* DSP Subsystem UART Receive Data */
	0x10821088, /* DSP Subsystem UART Transmit Data */
	0x1082108C, /* DSP Subsystem UART Control */
	0x10821000, /* TSM Timer Control Register */
	0x10821004, /* Time of the TSM Timer */
	0x1082304C, /* Decode Control Register 0 - Context 0 */
	0x10823050, /* Decode Control Register 1 - Context 0 */
	0x10821140, /* SW SET CLEAR Status Register (Register 0) */
	0x10821144, /* SW SET CLEAR Set Register (Register 0) */
	0x10821148, /* SW SET CLEAR Clear Register (Register 0) */
	0x1082114C, /* SW SET CLEAR Status Register (Register 1) */
	0x10821150, /* SW SET CLEAR Set Register (Register 1) */
	0x10821154, /* SW SET CLEAR Clear Register (Register 1) */
	0x10822404, /* Host Interrupt Set Register */
	0x10821054, /* Watchdog Timer Control Register */
	0x10821058, /* Time of the Watchdog Timer */
	0x10834000, /* MDMEM RAM */
	0x108A0080, /* Upper bits of STC 0 */
	0x108234F0, /* Base Address for FIFO 17 */
	0x108234F4, /* End Address for FIFO 17 */
	0x108234F8, /* Write Pointer for FIFO 17 */
	0x108234FC, /* Read Pointer for FIFO 17 */
	0x10838000, /* VOM RAM */
	0x10820000, /* Audio DSP System Revision Register */
	0x10821400, /* Priority of DMA Queues */
	0x10821120, /* Mailbox Register 0 */
	0x10821124, /* Mailbox Register 1 */
	0x10821128, /* Mailbox Register 2 */
	0x1082112C, /* Mailbox Register 3 */
	0x10821130, /* Mailbox Register 4 */
	0x10821134, /* Mailbox Register 5 */
	0x10821138, /* Mailbox Register 6 */
	0x1082113C, /* Mailbox Register 7 */
	0x10823500, /* Reserved Register 0 */
	0x000000F0, /* AUD_FMM_OP_CTRL_MCLK_CFG_FS0_MCLK_RATE_MASK */
	0x00000000, /* AUD_FMM_OP_CTRL_MCLK_CFG_FS0_MCLK_RATE_ALIGN */
	0x00000004, /* AUD_FMM_OP_CTRL_MCLK_CFG_FS0_MCLK_RATE_BITS */
	0x00000004, /* AUD_FMM_OP_CTRL_MCLK_CFG_FS0_MCLK_RATE_SHIFT */
	0x00000004, /* AUD_FMM_OP_CTRL_MCLK_CFG_FS0_MCLK_RATE_MCLK_is_512fs */
	0x00000003, /* AUD_FMM_OP_CTRL_MCLK_CFG_FS0_MCLK_RATE_MCLK_is_384fs */
	0x00000002, /* AUD_FMM_OP_CTRL_MCLK_CFG_FS0_MCLK_RATE_MCLK_is_256fs */
	0x00000001, /* AUD_FMM_OP_CTRL_MCLK_CFG_FS0_MCLK_RATE_MCLK_is_128fs */
	0x0000000F, /* AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_MASK */
	0x00000000, /* AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_ALIGN */
	0x00000004, /* AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_BITS */
	0x00000000, /* AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_SHIFT */
	0x00000000, /* AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_PLL0_ch1 */
	0x00000001, /* AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_PLL0_ch2 */
	0x00000002, /* AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_PLL0_ch3 */
	0x00000003, /* AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_PLL1_ch1 */
	0x00000004, /* AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_PLL1_ch2 */
	0x00000005, /* AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_PLL1_ch3 */
	0x00000006, /* AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_Hifidac0 */
	0x00000000, /* AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_Hifidac1 */
	0x00000000, /* AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_PLL2_ch1 */
	0x00000000, /* AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_PLL2_ch2 */
	0x00000000, /* AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_PLL2_ch3 */
	0x00000000, /* AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_HiFiDacDebug_0 */
	0x00000000, /* AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_HiFiDacDebug_1 */
	0x0000000D, /* AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_clk_I2S_In0 */
	0x00000000, /* AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_Mclk_gen0 */
	0x00000000, /* AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_Mclk_gen1 */
	0x000000F0, /* AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_MCLK_RATE_MASK */
	0x00000000, /* AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_MCLK_RATE_ALIGN */
	0x00000004, /* AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_MCLK_RATE_BITS */
	0x00000004, /* AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_MCLK_RATE_SHIFT */
	0x00000004, /* AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_MCLK_RATE_MCLK_is_512fs */
	0x00000003, /* AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_MCLK_RATE_MCLK_is_384fs */
	0x00000002, /* AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_MCLK_RATE_MCLK_is_256fs */
	0x00000001, /* AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_MCLK_RATE_MCLK_is_128fs */
	0x0000000F, /* AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_MASK */
	0x00000000, /* AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_ALIGN */
	0x00000004, /* AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_BITS */
	0x00000000, /* AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_SHIFT */
	0x00000000, /* AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_PLL0_ch1 */
	0x00000001, /* AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_PLL0_ch2 */
	0x00000002, /* AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_PLL0_ch3 */
	0x00000003, /* AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_PLL1_ch1 */
	0x00000004, /* AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_PLL1_ch2 */
	0x00000005, /* AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_PLL1_ch3 */
	0x00000006, /* AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_Hifidac0 */
	0x00000000, /* AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_Hifidac1 */
	0x00000000, /* AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_PLL2_ch1 */
	0x00000000, /* AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_PLL2_ch2 */
	0x00000000, /* AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_PLL2_ch3 */
	0x00000000, /* AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_HiFiDacDebug_0 */
	0x00000000, /* AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_HiFiDacDebug_1 */
	0x0000000D, /* AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_clk_I2S_In0 */
	0x00000000, /* AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_Mclk_gen0 */
	0x00000000, /* AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_Mclk_gen1 */
	0x000000F0, /* AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0_MCLK_RATE_MASK */
	0x00000000, /* AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0_MCLK_RATE_ALIGN */
	0x00000004, /* AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0_MCLK_RATE_BITS */
	0x00000004, /* AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0_MCLK_RATE_SHIFT */
	0x0000000F, /* AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0_PLLCLKSEL_MASK */
	0x00000000, /* AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0_PLLCLKSEL_ALIGN */
	0x00000004, /* AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0_PLLCLKSEL_BITS */
	0x00000000, /* AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0_PLLCLKSEL_SHIFT */
	0x00000007, /* AUD_FMM_PLL0_MACRO_MACRO_SELECT_User */
	0x10821448, /* DMA Source Address Register for DMA Command Queue-0 */
	0x10821488, /* DMA Source Address Register for DMA command Queue-1 */
	0x1082144C, /* DMA destination address register for DMA Command Queue-0 */
	0x10821444, /* DMA SCB command type for Video Block Access and Video Raster access commands. */
	0x10821450, /* DMA transfer enable register for DMA-Queue-0 */
	0x10822214, /* Host Interrupt Mask Clear Register */
	0x10822210, /* Host Interrupt Mask Set Register */
	0x10822204, /* Host Interrupt Set Register */
	0x1082145C, /* DMA-Token-ID clear for DMA-Queue-0 */
	0x108233E0, /* Base Address for FIFO 0 */
	0x108233E4, /* End Address for FIFO 0 */
	0x108233E8, /* Write Pointer for FIFO 0 */
	0x108233EC, /* Read Pointer for FIFO 0 */
	0x108233F0, /* Base Address for FIFO 1 */
    0X108215A8, /* RAAGA_DSP_DMA_SRC_ADDR_VQ4 */
    0X108215AC, /* RAAGA_DSP_DMA_DEST_ADDR_VQ4 */
    0X108215B0, /* RAAGA_DSP_DMA_TRANSFER_VQ4 */
    0X108215B4, /* RAAGA_DSP_DMA_PROGRESS_VQ4 */
    0X108215BC, /* RAAGA_DSP_DMA_TOKEN_ID_CLR_VQ4 */
    0X108215A0, /* RAAGA_DSP_DMA_MAX_SCB_BURST_SIZE_VQ4 */
    0X108215A4, /* RAAGA_DSP_DMA_SCB_GEN_CMD_TYPE_VQ4 */
    0X108215C4, /* RAAGA_DSP_DMA_DMA_ADDR1_VQ4 */
    0X108215C8, /* RAAGA_DSP_DMA_DMA_ADDR2_VQ4 */
    0X108215CC, /* RAAGA_DSP_DMA_VIDEO_PATCH_PARAM1_VQ4 */
    0X108215D0, /* RAAGA_DSP_DMA_VIDEO_PATCH_PARAM2_VQ4 */
    0X108215F8, /* RAAGA_DSP_DMA_SRC_ADDR_VQ5 */
    0X10821564, /* RAAGA_DSP_DMA_DRAM_VIDEO_NMBY0 */
    0X10821544, /* RAAGA_DSP_DMA_DRAM_VIDEO_BASE_ADDR0 */
    0X10821540, /* RAAGA_DSP_DMA_SCB_IF_CONFIG */
	/* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
};
const uint32_t BDSP_IMG_system_rdbvars_header [2] = {sizeof(BDSP_IMG_system_rdbvars_array1), 1};
const void * const BDSP_IMG_system_rdbvars [2] = {BDSP_IMG_system_rdbvars_header, BDSP_IMG_system_rdbvars_array1};
/* End of File */
