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
	0x00020000, /* Imem Size */
	0x00018000, /* Dmem Size */
	0x00000196, /* Uart baud rate div factor */
	0x165A0BC0, /* Firepath frequency in MHz */
	0x00000000, /* PLL_MACRO_SELECT_NA */
	0x00000000, /* USE_384FS_I2S_MCLK_FOR_32KHZ */
	/* RDB Addresses */
	0x80021080, /* RAAGA_DSP_PERI_DBG_CTRL_UART_STATUS 		- DSP Subsystem UART Status */
	0x80021084, /* RAAGA_DSP_PERI_DBG_CTRL_UART_RCV_DATA 	- DSP Subsystem UART Receive Data */
	0x80021088, /* RAAGA_DSP_PERI_DBG_CTRL_UART_XMIT_DATA 	- DSP Subsystem UART Transmit Data */
	0x8002108c, /* RAAGA_DSP_PERI_DBG_CTRL_UART_CTRL 		- DSP Subsystem UART Control */
	0x80021000, /* RAAGA_DSP_TIMERS_TSM_TIMER 				- TSM Timer Control Register */
	0x80021004, /* RAAGA_DSP_TIMERS_TSM_TIMER_VALUE 		- Time of the TSM Timer */
	0x8002304c, /* RAAGA_DSP_FW_CFG_CONTROL_REGISTER0_CXT0 	- Decode Control Register 0 - Context 0 */
	0x80023050, /* RAAGA_DSP_FW_CFG_CONTROL_REGISTER1_CXT0 	- Decode Control Register 1 - Context 0 */
	0x80021140, /* RAAGA_DSP_PERI_SW_MSG_BITS_STATUS_0 		- SW SET CLEAR Status Register (Register 0) */
	0x80021144, /* RAAGA_DSP_PERI_SW_MSG_BITS_SET_0 		- SW SET CLEAR Set Register (Register 0) */
	0x80021148, /* RAAGA_DSP_PERI_SW_MSG_BITS_CLEAR_0 		- SW SET CLEAR Clear Register (Register 0) */
	0x8002114c, /* RAAGA_DSP_PERI_SW_MSG_BITS_STATUS_1 		- SW SET CLEAR Status Register (Register 1) */
	0x80021150, /* RAAGA_DSP_PERI_SW_MSG_BITS_SET_1 		- SW SET CLEAR Set Register (Register 1) */
	0x80021154, /* RAAGA_DSP_PERI_SW_MSG_BITS_CLEAR_1 		- SW SET CLEAR Clear Register (Register 1) */
	0x80022404, /* RAAGA_DSP_FW_INTH_HOST_SET 				- Host Interrupt Set Register */
	0x80021054, /* RAAGA_DSP_TIMERS_WATCHDOG_TIMER 			- Watchdog Timer Control Register */
	0x80021058, /* RAAGA_DSP_TIMERS_WATCHDOG_TIMER_VALUE 	- Time of the Watchdog Timer */
	0x80034000, /* RAAGA_DSP_MEM_SUBSYSTEM_MDMEMRAM0 		- MDMEM RAM */
	0x80080480, /* AUD_FMM_MISC_STC_UPPER0 					- Upper bits of STC 0 */
	0x800234f0, /* RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_17 		- Base Address for FIFO 17 */
	0x800234f4, /* RAAGA_DSP_FW_CFG_END_ADDR_FIFO_17 		- End Address for FIFO 17 */
	0x800234f8, /* RAAGA_DSP_FW_CFG_WRITE_PTR_FIFO_17 		- Write Pointer for FIFO 17 */
	0x800234fc, /* RAAGA_DSP_FW_CFG_READ_PTR_FIFO_17 		- Read Pointer for FIFO 17 */
	0x80038000, /* RAAGA_DSP_MEM_SUBSYSTEM_VOMRAM0 			- VOM RAM */
	0x80020000, /* RAAGA_DSP_MISC_REVISION Audio 			- DSP System Revision Register */
	0x80021200, /* RAAGA_DSP_DMA_QUEUE_PRIORITY 			- Priority of DMA Queues */
	0x80021120, /* RAAGA_DSP_PERI_SW_MAILBOX0 				- Mailbox Register 0 */
	0x80021124, /* RAAGA_DSP_PERI_SW_MAILBOX1 				- Mailbox Register 1 */
	0x80021128, /* RAAGA_DSP_PERI_SW_MAILBOX2 				- Mailbox Register 2 */
	0x8002112c, /* RAAGA_DSP_PERI_SW_MAILBOX3 				- Mailbox Register 3 */
	0x80021130, /* RAAGA_DSP_PERI_SW_MAILBOX4 				- Mailbox Register 4 */
	0x80021134, /* RAAGA_DSP_PERI_SW_MAILBOX5 				- Mailbox Register 5 */
	0x80021138, /* RAAGA_DSP_PERI_SW_MAILBOX6 				- Mailbox Register 6 */
	0x8002113c, /* RAAGA_DSP_PERI_SW_MAILBOX7 				- Mailbox Register 7 */
	0x80023500, /* RAAGA_DSP_FW_CFG_SW_UNDEFINED_SPARE0 	- Sw Undefined spare 0 */
	/* MASK, SHIFT and ALIGN VALUES */
	0x000000f0, /*  AUD_FMM_OP_CTRL_MCLK_CFG_FS0_MCLK_RATE_MASK */
	0x00000000, /*  AUD_FMM_OP_CTRL_MCLK_CFG_FS0_MCLK_RATE_ALIGN */
	0x00000004, /*  AUD_FMM_OP_CTRL_MCLK_CFG_FS0_MCLK_RATE_BITS */
	0x00000004, /*  AUD_FMM_OP_CTRL_MCLK_CFG_FS0_MCLK_RATE_SHIFT */
	0x00000004, /*  AUD_FMM_OP_CTRL_MCLK_CFG_FS0_MCLK_RATE_MCLK_is_512fs */
	0x00000003, /*  AUD_FMM_OP_CTRL_MCLK_CFG_FS0_MCLK_RATE_MCLK_is_384fs */
	0x00000002, /*  AUD_FMM_OP_CTRL_MCLK_CFG_FS0_MCLK_RATE_MCLK_is_256fs */
	0x00000001, /*  AUD_FMM_OP_CTRL_MCLK_CFG_FS0_MCLK_RATE_MCLK_is_128fs */
	0x0000000f, /*  AUD_FMM_OP_CTRL_MCLK_CFG_FS0_PLLCLKSEL_MASK */
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
	0x000000f0, /*  AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_MCLK_RATE_MASK */
	0x00000000, /*  AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_MCLK_RATE_ALIGN */
	0x00000004, /*  AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_MCLK_RATE_BITS */
	0x00000004, /*  AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_MCLK_RATE_SHIFT */
	0x00000004, /*  AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_MCLK_RATE_MCLK_is_512fs */
	0x00000003, /*  AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_MCLK_RATE_MCLK_is_384fs */
	0x00000002, /*  AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_MCLK_RATE_MCLK_is_256fs */
	0x00000001, /*  AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_MCLK_RATE_MCLK_is_128fs */
	0x0000000f, /*  AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_MASK */
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
	0x000000f0, /*  AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0_MCLK_RATE_MASK  */
	0x00000000, /*  AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0_MCLK_RATE_ALIGN */
	0x00000004, /*  AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0_MCLK_RATE_BITS */
	0x00000004, /*  AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0_MCLK_RATE_SHIFT */
	0x0000000f, /*  AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0_PLLCLKSEL_MASK */
	0x00000000, /*  AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0_PLLCLKSEL_ALIGN */
	0x00000004, /*  AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0_PLLCLKSEL_BITS */
	0x00000000, /*  AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0_PLLCLKSEL_SHIFT */
	0x00000007, /*  AUD_FMM_PLL0_MACRO_MACRO_SELECT_User */
	0x80021248, /*  RAAGA_DSP_DMA_SRC_ADDR_Q0 */
	0x80021288, /*  RAAGA_DSP_DMA_SRC_ADDR_Q1 */
	0x8002124c, /*  RAAGA_DSP_DMA_DEST_ADDR_Q0 */
	0x80021244, /*  RAAGA_DSP_DMA_SCB_GEN_CMD_TYPE_Q0 */
	0x80021250, /*  RAAGA_DSP_DMA_TRANSFER_Q0 */
	0x80022214, /*  RAAGA_DSP_INTH_HOST_MASK_CLEAR */
	0x80022210, /*  RAAGA_DSP_INTH_HOST_MASK_SET */
	0x80022204, /*  RAAGA_DSP_INTH_HOST_SET */
	0x8002125c, /*  RAAGA_DSP_DMA_TOKEN_ID_CLR_Q0 */
	0x800233e0, /* RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 		- Base Address for FIFO 0 */
	0x800233e4, /* RAAGA_DSP_FW_CFG_END_ADDR_FIFO_0 		- End Address for FIFO 0 */
	0x800233e8, /* RAAGA_DSP_FW_CFG_WRITE_PTR_FIFO_0 		- Write Pointer for FIFO 0 */
	0x800233ec, /* RAAGA_DSP_FW_CFG_READ_PTR_FIFO_0 		- Read Pointer for FIFO 0 */
	0x800233f0, /* RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_1 		- Base Address for FIFO 1 */
	/* No Used */
	0x00000000, /* UNDEFINED */
	0x00000000, /* . */
	0x00000000, /* . */
	0x00000000, /* . */
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000
};
const uint32_t BDSP_IMG_system_rdbvars_header [2] = {sizeof(BDSP_IMG_system_rdbvars_array1), 1};
const void * const BDSP_IMG_system_rdbvars [2] = {BDSP_IMG_system_rdbvars_header, BDSP_IMG_system_rdbvars_array1};
/* End of File */