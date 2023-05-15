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
	0x00030000, /* Imem Size */
	0x00020000, /* Dmem Size */
	0x00000256, /* Uart baud rate div factor */
	0x20E6DA00, /* Firepath frequency in Hz */
	0x00000000, /* PLL_MACRO_SELECT_NA */
	0x00000000, /* USE_384FS_I2S_MCLK_FOR_32KHZ */
	/* RDB Addresses */
	0x10C21080, /* DSP Subsystem UART Status */
	0x10C21084, /* DSP Subsystem UART Receive Data */
	0x10C21088, /* DSP Subsystem UART Transmit Data */
	0x10C2108C, /* DSP Subsystem UART Control */
	0x10C21000, /* TSM Timer Control Register */
	0x10C21004, /* Time of the TSM Timer */
	0x10C2304C, /* Decode Control Register 0 - Context 0 */
	0x10C23050, /* Decode Control Register 1 - Context 0 */
	0x10C21140, /* SW SET CLEAR Status Register (Register 0) */
	0x10C21144, /* SW SET CLEAR Set Register (Register 0) */
	0x10C21148, /* SW SET CLEAR Clear Register (Register 0) */
	0x10C2114C, /* SW SET CLEAR Status Register (Register 1) */
	0x10C21150, /* SW SET CLEAR Set Register (Register 1) */
	0x10C21154, /* SW SET CLEAR Clear Register (Register 1) */
	0x10C22404, /* Host Interrupt Set Register */
	0x10C21054, /* Watchdog Timer Control Register */
	0x10C21058, /* Time of the Watchdog Timer */
	0x10C34000, /* MDMEM RAM */
	0x10C80484, /* Upper 10 bits of STC 0 */
	0x10C234F0, /* Base Address for FIFO 17 */
	0x10C234F4, /* End Address for FIFO 17 */
	0x10C234F8, /* Write Pointer for FIFO 17 */
	0x10C234FC, /* Read Pointer for FIFO 17 */
	0x10C38000, /* VOM RAM */
	0x10C20000, /* Audio DSP System Revision Register */
	0x10C21400, /* Priority of DMA Queues */
	0x10C21120, /* Mailbox Register 0 */
	0x10C21124, /* Mailbox Register 1 */
	0x10C21128, /* Mailbox Register 2 */
	0x10C2112C, /* Mailbox Register 3 */
	0x10C21130, /* Mailbox Register 4 */
	0x10C21134, /* Mailbox Register 5 */
	0x10C21138, /* Mailbox Register 6 */
	0x10C2113C, /* Mailbox Register 7 */
	0x10C23500, /* Reserved Register 0 */
	0x000000F0, /*  AUD_FMM_OP_CTRL_MCLK_CFG_FS0_MCLK_RATE_MASK */
	0x00000000, /*  AUD_FMM_OP_CTRL_MCLK_CFG_FS0_MCLK_RATE_ALIGN */
	0x00000004, /*  AUD_FMM_OP_CTRL_MCLK_CFG_FS0_MCLK_RATE_BITS */
	0x00000004,	/*  AUD_FMM_OP_CTRL_MCLK_CFG_FS0_MCLK_RATE_SHIFT */
	0x00000004, /*  AUD_FMM_OP_CTRL_MCLK_CFG_FS0_MCLK_RATE_MCLK_is_512fs */
	0x00000003, /*  AUD_FMM_OP_CTRL_MCLK_CFG_FS0_MCLK_RATE_MCLK_is_384fs */
	0x00000002, /*  AUD_FMM_OP_CTRL_MCLK_CFG_FS0_MCLK_RATE_MCLK_is_256fs */
	0x00000001, /*  AUD_FMM_OP_CTRL_MCLK_CFG_FS0_MCLK_RATE_MCLK_is_128fs */
	0x0000000F, /*  AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_MASK */
	0x00000000, /*  AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_ALIGN */
	0x00000004, /*  AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_BITS */
	0x00000000, /*  AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_SHIFT */
	0x00000000, /*  AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_PLL0_ch1 */
	0x00000001, /*  AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_PLL0_ch2 */
	0x00000002, /*  AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_PLL0_ch3 */
	0x00000003, /*  AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_PLL1_ch1 */
	0x00000004, /*  AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_PLL1_ch2 */
	0x00000005, /*  AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_PLL1_ch3 */
	0x00000006, /*  AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_Hifidac0 */
	0x00000007, /*  AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_Hifidac1 */
	0x00000008, /*  AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_PLL2_ch1 */
	0x00000009, /*  AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_PLL2_ch2 */
	0x0000000A, /*  AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_PLL2_ch3 */
	0x0000000B, /*  AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_HiFiDacDebug_0 */
	0x0000000C, /*  AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_HiFiDacDebug_1 */
	0x0000000D, /*  AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_clk_I2S_In0 */
	0x0000000E, /*  AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_Mclk_gen0 */
	0x0000000F, /*  AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_Mclk_gen1 */
	0x000000F0, /*  AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_MCLK_RATE_MASK */
	0x00000000, /*  AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_MCLK_RATE_ALIGN */
	0x00000004, /*  AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_MCLK_RATE_BITS */
	0x00000004, /*  AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_MCLK_RATE_SHIFT */
	0x00000004, /*  AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_MCLK_RATE_MCLK_is_512fs */
	0x00000003, /*  AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_MCLK_RATE_MCLK_is_384fs */
	0x00000002, /*  AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_MCLK_RATE_MCLK_is_256fs */
	0x00000001, /*  AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_MCLK_RATE_MCLK_is_128fs */
	0x0000000F, /*  AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_MASK */
	0x00000000, /*  AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_ALIGN */
	0x00000004, /*  AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_BITS */
	0x00000000, /*  AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_SHIFT */
	0x00000000, /*  AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_PLL0_ch1 */
	0x00000001, /*  AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_PLL0_ch2 */
	0x00000002, /*  AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_PLL0_ch3 */
	0x00000003, /*  AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_PLL1_ch1 */
	0x00000004, /*  AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_PLL1_ch2 */
	0x00000005, /*  AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_PLL1_ch3 */
	0x00000006, /*  AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_Hifidac0 */
	0x00000007, /*  AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_Hifidac1 */
	0x00000008, /*  AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_PLL2_ch1 */
	0x00000009, /*  AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_PLL2_ch2 */
	0x0000000A, /*  AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_PLL2_ch3 */
	0x0000000B, /*  AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_HiFiDacDebug_0 */
	0x0000000C, /*  AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_HiFiDacDebug_1 */
	0x0000000D, /*  AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_clk_I2S_In0 */
	0x0000000E, /*  AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_Mclk_gen0 */
	0x0000000F, /*  AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_Mclk_gen1	*/
	0x000000F0, /* AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0_MCLK_RATE_MASK */
	0x00000000, /* AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0_MCLK_RATE_ALIGN */
	0x00000004, /* AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0_MCLK_RATE_BITS */
	0x00000004, /* AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0_MCLK_RATE_SHIFT */
	0x0000000F, /* AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0_PLLCLKSEL_MASK */
	0x00000000, /* AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0_PLLCLKSEL_ALIGN */
	0x00000004, /* AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0_PLLCLKSEL_BITS */
	0x00000000, /* AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0_PLLCLKSEL_SHIFT */
	0x00000007, /* AUD_FMM_PLL0_MACRO_MACRO_SELECT_User */
	0x10C21448, /* DMA Source Address Register for DMA Command Queue-0 */
	0x10C21488, /* DMA Source Address Register for DMA command Queue-1 */
	0x10C2144C, /* DMA destination address register for DMA Command Queue-0 */
	0x10C21444, /* DMA SCB command type for Video Block Access and Video Raster access commands. */
	0x10C21450, /* DMA transfer enable register for DMA-Queue-0 */
	0x10C22214, /* Host Interrupt Mask Clear Register */
	0x10C22210, /* Host Interrupt Mask Set Register */
	0x10C22204, /* Host Interrupt Set Register */
	0x10C2145C, /* DMA-Token-ID clear for DMA-Queue-0 */
	0x10C233E0, /* Base Address for FIFO 0 */
	0x10C233E4, /* End Address for FIFO 0 */
	0x10C233E8, /* Write Pointer for FIFO 0 */
	0x10C233EC, /* Read Pointer for FIFO 0 */
	0x10C233F0, /* Base Address for FIFO 1 */
    0X10C215A8, /* RAAGA_DSP_DMA_SRC_ADDR_VQ4 */
    0X10C215AC, /* RAAGA_DSP_DMA_DEST_ADDR_VQ4 */
    0X10C215B0, /* RAAGA_DSP_DMA_TRANSFER_VQ4 */
    0X10C215B4, /* RAAGA_DSP_DMA_PROGRESS_VQ4 */
    0X10C215BC, /* RAAGA_DSP_DMA_TOKEN_ID_CLR_VQ4 */
    0X10C215A0, /* RAAGA_DSP_DMA_MAX_SCB_BURST_SIZE_VQ4 */
    0X10C215A4, /* RAAGA_DSP_DMA_SCB_GEN_CMD_TYPE_VQ4 */
    0X10C215C4, /* RAAGA_DSP_DMA_DMA_ADDR1_VQ4 */
    0X10C215C8, /* RAAGA_DSP_DMA_DMA_ADDR2_VQ4 */
    0X10C215CC, /* RAAGA_DSP_DMA_VIDEO_PATCH_PARAM1_VQ4 */
    0X10C215D0, /* RAAGA_DSP_DMA_VIDEO_PATCH_PARAM2_VQ4 */
    0X10C215F8, /* RAAGA_DSP_DMA_SRC_ADDR_VQ5 */
    0X10C21564, /* RAAGA_DSP_DMA_DRAM_VIDEO_NMBY0 */
    0X10C21544, /* RAAGA_DSP_DMA_DRAM_VIDEO_BASE_ADDR0 */
    0X10C21540, /* RAAGA_DSP_DMA_SCB_IF_CONFIG */
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
