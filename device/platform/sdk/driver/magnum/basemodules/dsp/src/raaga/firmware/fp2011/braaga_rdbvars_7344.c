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
	0x11321080, /* DSP Subsystem UART Status */
	0x11321084, /* DSP Subsystem UART Receive Data */
	0x11321088, /* DSP Subsystem UART Transmit Data */
	0x1132108C, /* DSP Subsystem UART Control */
	0x11321000, /* TSM Timer Control Register */
	0x11321004, /* Time of the TSM Timer */
	0x1132304C, /* Decode Control Register 0 - Context 0 */
	0x11323050, /* Decode Control Register 1 - Context 0 */
	0x11321140, /* SW SET CLEAR Status Register (Register 0) */
	0x11321144, /* SW SET CLEAR Set Register (Register 0) */
	0x11321148, /* SW SET CLEAR Clear Register (Register 0) */
	0x1132114C, /* SW SET CLEAR Status Register (Register 1) */
	0x11321150, /* SW SET CLEAR Set Register (Register 1) */
	0x11321154, /* SW SET CLEAR Clear Register (Register 1) */
	0x11322404, /* Host Interrupt Set Register */
	0x11321054, /* Watchdog Timer Control Register */
	0x11321058, /* Time of the Watchdog Timer */
	0x11334000, /* MDMEM RAM */
	0x113A0080, /* Upper bits of STC 0 */
	0x113234F0, /* Base Address for FIFO 17 */
	0x113234F4, /* End Address for FIFO 17 */
	0x113234F8, /* Write Pointer for FIFO 17 */
	0x113234FC, /* Read Pointer for FIFO 17 */
	0x11338000, /* VOM RAM */
	0x11320000, /* Audio DSP System Revision Register */
	0x11321400, /* Priority of DMA Queues */
	0x11321120, /* Mailbox Register 0 */
	0x11321124, /* Mailbox Register 1 */
	0x11321128, /* Mailbox Register 2 */
	0x1132112C, /* Mailbox Register 3 */
	0x11321130, /* Mailbox Register 4 */
	0x11321134, /* Mailbox Register 5 */
	0x11321138, /* Mailbox Register 6 */
	0x1132113C, /* Mailbox Register 7 */
	0x11323500, /* Reserved Register 0 */
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
	0x11321448, /* DMA Source Address Register for DMA Command Queue-0 */
	0x11321488, /* DMA Source Address Register for DMA command Queue-1 */
	0x1132144C, /* DMA destination address register for DMA Command Queue-0 */
	0x11321444, /* DMA SCB command type for Video Block Access and Video Raster access commands. */
	0x11321450, /* DMA transfer enable register for DMA-Queue-0 */
	0x11322214, /* Host Interrupt Mask Clear Register */
	0x11322210, /* Host Interrupt Mask Set Register */
	0x11322204, /* Host Interrupt Set Register */
	0x1132145C, /* DMA-Token-ID clear for DMA-Queue-0 */
	0x113233E0, /* Base Address for FIFO 0 */
	0x113233E4, /* End Address for FIFO 0 */
	0x113233E8, /* Write Pointer for FIFO 0 */
	0x113233EC, /* Read Pointer for FIFO 0 */
	0x113233F0, /* Base Address for FIFO 1 */
    0X113215A8, /* RAAGA_DSP_DMA_SRC_ADDR_VQ4 */
    0X113215AC, /* RAAGA_DSP_DMA_DEST_ADDR_VQ4 */
    0X113215B0, /* RAAGA_DSP_DMA_TRANSFER_VQ4 */
    0X113215B4, /* RAAGA_DSP_DMA_PROGRESS_VQ4 */
    0X113215BC, /* RAAGA_DSP_DMA_TOKEN_ID_CLR_VQ4 */
    0X113215A0, /* RAAGA_DSP_DMA_MAX_SCB_BURST_SIZE_VQ4 */
    0X113215A4, /* RAAGA_DSP_DMA_SCB_GEN_CMD_TYPE_VQ4 */
    0X113215C4, /* RAAGA_DSP_DMA_DMA_ADDR1_VQ4 */
    0X113215C8, /* RAAGA_DSP_DMA_DMA_ADDR2_VQ4 */
    0X113215CC, /* RAAGA_DSP_DMA_VIDEO_PATCH_PARAM1_VQ4 */
    0X113215D0, /* RAAGA_DSP_DMA_VIDEO_PATCH_PARAM2_VQ4 */
    0X113215F8, /* RAAGA_DSP_DMA_SRC_ADDR_VQ5 */
    0X11321564, /* RAAGA_DSP_DMA_DRAM_VIDEO_NMBY0 */
    0X11321544, /* RAAGA_DSP_DMA_DRAM_VIDEO_BASE_ADDR0 */
    0X11321540, /* RAAGA_DSP_DMA_SCB_IF_CONFIG */
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
