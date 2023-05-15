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
 * $brcm_Workfile: bchp_decode_cpuregs2_0.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 10/11/11 5:13p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Tue Oct 11 11:36:33 2011
 *                 MD5 Checksum         12f5dd38e0ec050efd2afb0b1c62e9d9
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7435/rdb/a0/bchp_decode_cpuregs2_0.h $
 * 
 * Hydra_Software_Devel/1   10/11/11 5:13p pntruong
 * SW7435-3: Synced up with central rdb.
 *
 ***************************************************************************/

#ifndef BCHP_DECODE_CPUREGS2_0_H__
#define BCHP_DECODE_CPUREGS2_0_H__

/***************************************************************************
 *DECODE_CPUREGS2_0 - Outer Loop CPU Registers 0
 ***************************************************************************/
#define BCHP_DECODE_CPUREGS2_0_REG_HST2CPU_MBX   0x00a00f80 /* Host 2 CPU mailbox register */
#define BCHP_DECODE_CPUREGS2_0_REG_CPU2HST_MBX   0x00a00f84 /* CPU to Host mailbox register */
#define BCHP_DECODE_CPUREGS2_0_REG_MBX_STAT      0x00a00f88 /* Mailbox status flags */
#define BCHP_DECODE_CPUREGS2_0_REG_INST_BASE     0x00a00f8c /* Instruction base address register */
#define BCHP_DECODE_CPUREGS2_0_REG_CPU_INT_ENA   0x00a00f90 /* CPU interrupt enable */
#define BCHP_DECODE_CPUREGS2_0_REG_CPU_INT_STAT  0x00a00f94 /* CPU interrupt status */
#define BCHP_DECODE_CPUREGS2_0_REG_HST2CPU_STAT  0x00a00f98 /* Host to CPU status register */
#define BCHP_DECODE_CPUREGS2_0_REG_CPU2HST_STAT  0x00a00f9c /* CPU to Host status register */
#define BCHP_DECODE_CPUREGS2_0_REG_CPU_INTGEN_SET 0x00a00fa0 /* CPU interrupt set register */
#define BCHP_DECODE_CPUREGS2_0_REG_CPU_INTGEN_CLR 0x00a00fa4 /* CPU interrupt clear register */
#define BCHP_DECODE_CPUREGS2_0_REG_CPU_ICACHE_MISS 0x00a00fa8 /* Instruction cache miss counter */
#define BCHP_DECODE_CPUREGS2_0_REG_CPU_INTGEN_MASK 0x00a00fac /* CPU interrupt mask register */
#define BCHP_DECODE_CPUREGS2_0_REG_DRAM_RD_CNTR  0x00a00fb0 /* CPU DRAM Read access Counter */
#define BCHP_DECODE_CPUREGS2_0_REG_END_OF_CODE   0x00a00fb4 /* End of code register */
#define BCHP_DECODE_CPUREGS2_0_REG_GLOBAL_IO_BASE 0x00a00fb8 /* Global IO base register */
#define BCHP_DECODE_CPUREGS2_0_REG_DEBUG_TRACE_FIFO_WR 0x00a00fbc /* CPU debug trace fifo write */
#define BCHP_DECODE_CPUREGS2_0_REG_DEBUG_TRACE_FIFO_RD 0x00a00fc0 /* CPU debug trace fifo read */
#define BCHP_DECODE_CPUREGS2_0_REG_DEBUG_TRACE_FIFO_CTL 0x00a00fc4 /* CPU debug trace fifo control */
#define BCHP_DECODE_CPUREGS2_0_REG_DRAM_WR_CNTR  0x00a00fc8 /* CPU DRAM Write access Counter */
#define BCHP_DECODE_CPUREGS2_0_REG_WATCHDOG_TMR  0x00a00fcc /* Watchdog timer register */
#define BCHP_DECODE_CPUREGS2_0_REG_SDRAM_STATUS  0x00a00fd0 /* SDRAM Status register */
#define BCHP_DECODE_CPUREGS2_0_REG_CMD_REG0      0x00a00fe0 /* Command register 0 */
#define BCHP_DECODE_CPUREGS2_0_REG_CMD_REG1      0x00a00fe4 /* Command register 1 */
#define BCHP_DECODE_CPUREGS2_0_REG_CMD_REG2      0x00a00fe8 /* Command register 2 */
#define BCHP_DECODE_CPUREGS2_0_REG_CMD_REG3      0x00a00fec /* Command register 3 */
#define BCHP_DECODE_CPUREGS2_0_REG_CPUREGS_END   0x00a00ffc /* Dummy end */

#endif /* #ifndef BCHP_DECODE_CPUREGS2_0_H__ */

/* End of File */