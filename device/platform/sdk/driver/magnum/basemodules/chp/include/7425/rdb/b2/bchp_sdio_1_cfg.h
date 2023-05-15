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
 * $brcm_Workfile: bchp_sdio_1_cfg.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 1/17/12 4:56p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Tue Jan 17 13:26:48 2012
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
 * $brcm_Log: /magnum/basemodules/chp/7425/rdb/b2/bchp_sdio_1_cfg.h $
 * 
 * Hydra_Software_Devel/1   1/17/12 4:56p vanessah
 * SW7425-2202: add B2 headers
 *
 ***************************************************************************/

#ifndef BCHP_SDIO_1_CFG_H__
#define BCHP_SDIO_1_CFG_H__

/***************************************************************************
 *SDIO_1_CFG - SDIO (EMMC) Configuration Registers
 ***************************************************************************/
#define BCHP_SDIO_1_CFG_SDIO_EMMC_CTRL1          0x00419300 /* SDIO EMMC Control Register */
#define BCHP_SDIO_1_CFG_SDIO_EMMC_CTRL2          0x00419304 /* SDIO EMMC Control Register */
#define BCHP_SDIO_1_CFG_TP_OUT_SEL               0x00419308 /* SDIO TP_OUT Control Register */
#define BCHP_SDIO_1_CFG_CAP_REG0                 0x0041930c /* SDIO CAPABILITIES override Register */
#define BCHP_SDIO_1_CFG_CAP_REG1                 0x00419310 /* SDIO CAPABILITIES override Register */
#define BCHP_SDIO_1_CFG_PRESET1                  0x00419314 /* SDIO CAPABILITIES override Register */
#define BCHP_SDIO_1_CFG_PRESET2                  0x00419318 /* SDIO CAPABILITIES override Register */
#define BCHP_SDIO_1_CFG_SD_CLOCK_DELAY           0x0041931c /* SDIO Clock delay register */
#define BCHP_SDIO_1_CFG_SD_PAD_DRV               0x00419320 /* SDIO Clock delay register */
#define BCHP_SDIO_1_CFG_IP_DLY                   0x00419330 /* SDIO Host input delay register */
#define BCHP_SDIO_1_CFG_OP_DLY                   0x00419334 /* SDIO Host output delay register */
#define BCHP_SDIO_1_CFG_TUNING                   0x00419338 /* SDIO Host tuning configuration register */
#define BCHP_SDIO_1_CFG_VOLT_CTRL                0x0041933c /* SDIO Host 1p8V control logic select register */
#define BCHP_SDIO_1_CFG_SCRATCH                  0x004193fc /* SDIO Scratch Register */
#define BCHP_SDIO_1_CFG_VERSION                  0x004193f0 /* SDIO VERSION Register */

#endif /* #ifndef BCHP_SDIO_1_CFG_H__ */

/* End of File */