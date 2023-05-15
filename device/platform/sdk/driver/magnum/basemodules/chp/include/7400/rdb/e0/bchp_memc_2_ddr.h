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
 * $brcm_Workfile: bchp_memc_2_ddr.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 1/9/08 11:12a $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Wed Jan  9 09:03:43 2008
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
 * $brcm_Log: /magnum/basemodules/chp/7400/rdb/e0/bchp_memc_2_ddr.h $
 * 
 * Hydra_Software_Devel/1   1/9/08 11:12a rpan
 * PR38572: Initial revision.
 *
 ***************************************************************************/

#ifndef BCHP_MEMC_2_DDR_H__
#define BCHP_MEMC_2_DDR_H__

/***************************************************************************
 *MEMC_2_DDR - Memory Controller 2 DDR IOBUF Registers
 ***************************************************************************/
#define BCHP_MEMC_2_DDR_IOBUF_REV_ID             0x00c07000 /* Memory Controller IOBUF Revision ID Register. */
#define BCHP_MEMC_2_DDR_PAD_SSTL_DDR2_MODE       0x00c07004 /* Pad Mode Control Register */
#define BCHP_MEMC_2_DDR_BYTE_SLICE_DLL_RESET     0x00c07008 /* Byteslice DLL reset register */
#define BCHP_MEMC_2_DDR_READ_DQS_GATE_CNTRL      0x00c0700c /* Read DQS gating control register */
#define BCHP_MEMC_2_DDR_BYTESLICE_CNTRL          0x00c07010 /* Deskew DLL Reset register */
#define BCHP_MEMC_2_DDR_CNTRLR_CONFIG            0x00c07014 /* Memory Controller Mode-Configuration Register. */
#define BCHP_MEMC_2_DDR_DRAM_MODE                0x00c07018 /* DDR Mode Register. */
#define BCHP_MEMC_2_DDR_DRAM_TIMING              0x00c0701c /* DDR-SDRAM Timing Register. */
#define BCHP_MEMC_2_DDR_POWER_DOWN_MODE          0x00c07020 /* Memory Controller Auto-Refresh Power Down Control Register. */
#define BCHP_MEMC_2_DDR_DLY_ARB                  0x00c07024 /* Delayed Arbitration Control register. */
#define BCHP_MEMC_2_DDR_CNTRLR_START_SEQ         0x00c07028 /* Memory Controller Sequencer Enable */
#define BCHP_MEMC_2_DDR_CNTRLR_SM_TIMEOUT        0x00c0702c /* Memory Controller , state machine timeout register. */
#define BCHP_MEMC_2_DDR_POWER_DOWN_STATUS        0x00c07030 /* Memory Controller , status register. */
#define BCHP_MEMC_2_DDR_BYTE0_VCDL_PHASE_CNTL    0x00c07034 /* VCDL Phase Control Register for bytelane 0 */
#define BCHP_MEMC_2_DDR_BYTE1_VCDL_PHASE_CNTL    0x00c07038 /* VCDL Phase Control Register for bytelane 1 */
#define BCHP_MEMC_2_DDR_DESKEW_DLL_RESET         0x00c0703c /* Deskew DLL Reset register */
#define BCHP_MEMC_2_DDR_DESKEW_DLL_CNTRL         0x00c07040 /* Deskew DLL control register */
#define BCHP_MEMC_2_DDR_DESKEW_DLL_PHASE         0x00c07044 /* Deskew DLL Phase register */
#define BCHP_MEMC_2_DDR_READ_WRITE_TIMING        0x00c07048 /* Read to Write & write to read timing register */
#define BCHP_MEMC_2_DDR_PFIFO_RD_WR_PNTR         0x00c0704c /* DQS read fifo (PFIFO) read & write pointers for debug purpose */
#define BCHP_MEMC_2_DDR_SPARE1_RW                0x00c07050 /* Spare register */
#define BCHP_MEMC_2_DDR_SPARE1_RO                0x00c07054 /* Spare register */

#endif /* #ifndef BCHP_MEMC_2_DDR_H__ */

/* End of File */