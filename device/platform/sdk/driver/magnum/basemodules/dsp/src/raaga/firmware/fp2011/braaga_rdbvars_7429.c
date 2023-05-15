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
	0x10821080, /* DSP Subsystem UART Status */
	0x10821084, /* DSP Subsystem UART Receive Data */
	0x10821088, /* DSP Subsystem UART Transmit Data */
	0x1082108c, /* DSP Subsystem UART Control */
	0x10821000, /* TSM Timer Control Register */
	0x10821004, /* Time of the TSM Timer */
	0x1082304c, /* Decode Control Register 0 - Context 0 */
	0x10823050, /* Decode Control Register 1 - Context 0 */
	0x10821140, /* SW SET CLEAR Status Register (Register 0) */
	0x10821144, /* SW SET CLEAR Set Register (Register 0) */
	0x10821148, /* SW SET CLEAR Clear Register (Register 0) */
	0x1082114c, /* SW SET CLEAR Status Register (Register 1) */
	0x10821150, /* SW SET CLEAR Set Register (Register 1) */
	0x10821154, /* SW SET CLEAR Clear Register (Register 1) */
	0x10822404, /* Host Interrupt Set Register */
	0x10821054, /* Watchdog Timer Control Register */
	0x10821058, /* Time of the Watchdog Timer */
	0x10834000, /* MDMEM RAM */
	0x10880020, /* AUD_FMM_MISC_STC_UPPER0 */
	0x108234f0, /* Base Address for FIFO 17 */
	0x108234f4, /* End Address for FIFO 17 */
	0x108234f8, /* Write Pointer for FIFO 17 */
	0x108234fc, /* Read Pointer for FIFO 17 */
	0x10838000, /* VOM RAM */
	0x10820000, /* Audio DSP System Revision Register */
	0x10821400, /* Priority of DMA Queues */
	0x10821120, /* Mailbox Register 0 */
	0x10821124, /* Mailbox Register 1 */
	0x10821128, /* Mailbox Register 2 */
	0x1082112c, /* Mailbox Register 3 */
	0x10821130, /* Mailbox Register 4 */
	0x10821134, /* Mailbox Register 5 */
	0x10821138, /* Mailbox Register 6 */
	0x1082113c, /* Mailbox Register 7 */
	0x10823500, /* Reserved Register 0 */
	0x000000F0, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_FS0_MCLK_RATE_MASK */
	0x00000000, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_FS0_MCLK_RATE_ALIGN */
	0x00000004, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_FS0_MCLK_RATE_BITS */
	0x00000004,	/* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_FS0_MCLK_RATE_SHIFT */
	0x00000004, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_FS0_MCLK_RATE_MCLK_is_512fs */
	0x00000003, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_FS0_MCLK_RATE_MCLK_is_384fs */
	0x00000002, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_FS0_MCLK_RATE_MCLK_is_256fs */
	0x00000001, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_FS0_MCLK_RATE_MCLK_is_128fs */
	0x0000000F, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_MASK */
	0x00000000, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_ALIGN */
	0x00000004, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_BITS */
	0x00000000, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_SHIFT */
	0x00000000, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_PLL0_ch1 */
	0x00000001, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_PLL0_ch2 */
	0x00000002, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_PLL0_ch3 */
	0x00000003, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_PLL1_ch1 */
	0x00000004, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_PLL1_ch2 */
	0x00000005, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_PLL1_ch3 */
	0x00000006, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_Hifidac0 */
	0x00000007, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_Hifidac1 */
	0x00000008, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_PLL2_ch1 */
	0x00000009, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_PLL2_ch2 */
	0x0000000A, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_PLL2_ch3 */
	0x0000000B, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_HiFiDacDebug_0 */
	0x0000000C, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_HiFiDacDebug_1 */
	0x0000000D, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_clk_I2S_In0 */
	0x0000000E, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_Mclk_gen0 */
	0x0000000F, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_Mclk_gen1 */
	0x000000F0, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_MCLK_RATE_MASK */
	0x00000000, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_MCLK_RATE_ALIGN */
	0x00000004, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_MCLK_RATE_BITS */
	0x00000004, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_MCLK_RATE_SHIFT */
	0x00000004, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_MCLK_RATE_MCLK_is_512fs */
	0x00000003, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_MCLK_RATE_MCLK_is_384fs */
	0x00000002, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_MCLK_RATE_MCLK_is_256fs */
	0x00000001, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_MCLK_RATE_MCLK_is_128fs */
	0x0000000F, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_MASK */
	0x00000000, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_ALIGN */
	0x00000004, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_BITS */
	0x00000000, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_SHIFT */
	0x00000000, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_PLL0_ch1 */
	0x00000001, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_PLL0_ch2 */
	0x00000002, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_PLL0_ch3 */
	0x00000003, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_PLL1_ch1 */
	0x00000004, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_PLL1_ch2 */
	0x00000005, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_PLL1_ch3 */
	0x00000006, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_Hifidac0 */
	0x00000007, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_Hifidac1 */
	0x00000008, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_PLL2_ch1 */
	0x00000009, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_PLL2_ch2 */
	0x0000000A, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_PLL2_ch3 */
	0x0000000B, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_HiFiDacDebug_0 */
	0x0000000C, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_HiFiDacDebug_1 */
	0x0000000D, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_clk_I2S_In0 */
	0x0000000E, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_Mclk_gen0 */
	0x0000000F, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_Mclk_gen1	*/
	0x000000F0, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0_MCLK_RATE_MASK */
	0x00000000, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0_MCLK_RATE_ALIGN */
	0x00000004, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0_MCLK_RATE_BITS */
	0x00000004, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0_MCLK_RATE_SHIFT */
	0x0000000F, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0_PLLCLKSEL_MASK */
	0x00000000, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0_PLLCLKSEL_ALIGN */
	0x00000004, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0_PLLCLKSEL_BITS */
	0x00000000, /* !!! NOT USED !!! AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0_PLLCLKSEL_SHIFT */
	0x00000007, /* !!! NOT USED !!! AUD_FMM_PLL0_MACRO_MACRO_SELECT_User */
	0x10821448, /* DMA Source Address Register for DMA Command Queue-0 */
	0x10821488, /* DMA Source Address Register for DMA command Queue-1 */
	0x1082144c, /* DMA destination address register for DMA Command Queue-0 */
	0x10821444, /* DMA SCB command type for Video Block Access and Video Raster access commands. */
	0x10821450, /* DMA transfer enable register for DMA-Queue-0 */
	0x10822214, /* Host Interrupt Mask Clear Register */
	0x10822210, /* Host Interrupt Mask Set Register */
	0x10822204, /* Host Interrupt Set Register */
	0x1082145c, /* DMA-Token-ID clear for DMA-Queue-0 */
	0x108233e0, /* Base Address for FIFO 0 */
	0x108233e4, /* End Address for FIFO 0 */
	0x108233e8, /* Write Pointer for FIFO 0 */
	0x108233ec, /* Read Pointer for FIFO 0 */
	0x108233f0, /* Base Address for FIFO 1 */
	0x108215a8, /* DMA Source Address Register for Video Queue-4 for non-Pixel patch operations */
	0x108215ac, /* DMA destination address register for Queue-4 for non-Pixel patch operations */
	0x108215b0, /* DMA transfer enable register for DMA-Video Queue-4 */
	0x108215b4, /* DMA transfer progress register for DMA-Queue-0 */
	0x108215bc, /* DMA-Token-ID clear for DMA-VQ4 */
	0x108215a0, /* DMA maximum SCB command burst size for DMA-Video Queue-4 */
	0x108215a4, /* DMA SCB command type for SCB_VIDEO_ACCESS command for Video Queue # 4. */
	0x108215c4, /* DMA Address1 Register for Video Queue-4 */
	0x108215c8, /* DMA Address2 Register for Video Queue-4 */
	0x108215cc, /* Video Patch offset register for Video Queue 4 */
	0x108215d0, /* Video Patch operation parameters */
	0x108215f8, /* DMA Source Address Register for Video Queue-5 for non-Pixel Patch operations */
	0x10821564, /* NMBY for DRAM Video Base Address 0 */
	0x10821544, /* DRAM Video Base Address 0 */
	0x10821540, /* Configuration register SCB0/1 MS bits programming and stripe width selection */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
	0x00000000, /* Not Used */
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
