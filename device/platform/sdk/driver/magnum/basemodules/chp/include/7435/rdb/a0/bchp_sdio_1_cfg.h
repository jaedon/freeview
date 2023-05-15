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
 * $brcm_Workfile: bchp_sdio_1_cfg.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 10/11/11 8:53p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Tue Oct 11 11:07:10 2011
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
 * $brcm_Log: /magnum/basemodules/chp/7435/rdb/a0/bchp_sdio_1_cfg.h $
 * 
 * Hydra_Software_Devel/1   10/11/11 8:53p pntruong
 * SW7435-3: Synced up with central rdb.
 *
 ***************************************************************************/

#ifndef BCHP_SDIO_1_CFG_H__
#define BCHP_SDIO_1_CFG_H__

/***************************************************************************
 *SDIO_1_CFG - SDIO (EMMC) Configuration Registers
 ***************************************************************************/
#define BCHP_SDIO_1_CFG_SDIO_EMMC_CTRL1          0x0041a300 /* SDIO EMMC Control Register */
#define BCHP_SDIO_1_CFG_SDIO_EMMC_CTRL2          0x0041a304 /* SDIO EMMC Control Register */
#define BCHP_SDIO_1_CFG_TP_OUT_SEL               0x0041a308 /* SDIO TP_OUT Control Register */
#define BCHP_SDIO_1_CFG_CAP_REG0                 0x0041a30c /* SDIO CAPABILITIES override Register */
#define BCHP_SDIO_1_CFG_CAP_REG1                 0x0041a310 /* SDIO CAPABILITIES override Register */
#define BCHP_SDIO_1_CFG_PRESET1                  0x0041a314 /* SDIO CAPABILITIES override Register */
#define BCHP_SDIO_1_CFG_PRESET2                  0x0041a318 /* SDIO CAPABILITIES override Register */
#define BCHP_SDIO_1_CFG_SD_CLOCK_DELAY           0x0041a31c /* SDIO Clock delay register */
#define BCHP_SDIO_1_CFG_SD_PAD_DRV               0x0041a320 /* SDIO Clock delay register */
#define BCHP_SDIO_1_CFG_IP_DLY                   0x0041a330 /* SDIO Host input delay register */
#define BCHP_SDIO_1_CFG_OP_DLY                   0x0041a334 /* SDIO Host output delay register */
#define BCHP_SDIO_1_CFG_TUNING                   0x0041a338 /* SDIO Host tuning configuration register */
#define BCHP_SDIO_1_CFG_VOLT_CTRL                0x0041a33c /* SDIO Host 1p8V control logic select register */
#define BCHP_SDIO_1_CFG_SCRATCH                  0x0041a3fc /* SDIO Scratch Register */
#define BCHP_SDIO_1_CFG_VERSION                  0x0041a3f0 /* SDIO VERSION Register */

#endif /* #ifndef BCHP_SDIO_1_CFG_H__ */

/* End of File */