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
 * $brcm_Date: 8/17/11 3:30p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Thu Aug 11 21:50:38 2011
 *                 MD5 Checksum         30eb4d41dba6899590ea7664187e6ba5
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7429/rdb/a0/bchp_sdio_1_cfg.h $
 * 
 * Hydra_Software_Devel/1   8/17/11 3:30p pntruong
 * SW7429-2: Initial version.
 *
 ***************************************************************************/

#ifndef BCHP_SDIO_1_CFG_H__
#define BCHP_SDIO_1_CFG_H__

/***************************************************************************
 *SDIO_1_CFG - SDIO (EMMC) Configuration Registers
 ***************************************************************************/
#define BCHP_SDIO_1_CFG_SDIO_EMMC_CTRL1          0x00413800 /* SDIO EMMC Control Register */
#define BCHP_SDIO_1_CFG_SDIO_EMMC_CTRL2          0x00413804 /* SDIO EMMC Control Register */
#define BCHP_SDIO_1_CFG_TP_OUT_SEL               0x00413808 /* SDIO TP_OUT Control Register */
#define BCHP_SDIO_1_CFG_CAP_REG0                 0x0041380c /* SDIO CAPABILITIES override Register */
#define BCHP_SDIO_1_CFG_CAP_REG1                 0x00413810 /* SDIO CAPABILITIES override Register */
#define BCHP_SDIO_1_CFG_PRESET1                  0x00413814 /* SDIO CAPABILITIES override Register */
#define BCHP_SDIO_1_CFG_PRESET2                  0x00413818 /* SDIO CAPABILITIES override Register */
#define BCHP_SDIO_1_CFG_SD_CLOCK_DELAY           0x0041381c /* SDIO Clock delay register */
#define BCHP_SDIO_1_CFG_SD_PAD_DRV               0x00413820 /* SDIO Clock delay register */
#define BCHP_SDIO_1_CFG_IP_DLY                   0x00413830 /* SDIO Host input delay register */
#define BCHP_SDIO_1_CFG_OP_DLY                   0x00413834 /* SDIO Host output delay register */
#define BCHP_SDIO_1_CFG_TUNING                   0x00413838 /* SDIO Host tuning configuration register */
#define BCHP_SDIO_1_CFG_VOLT_CTRL                0x0041383c /* SDIO Host 1p8V control logic select register */
#define BCHP_SDIO_1_CFG_SCRATCH                  0x004138fc /* SDIO Scratch Register */
#define BCHP_SDIO_1_CFG_VERSION                  0x004138f0 /* SDIO VERSION Register */

#endif /* #ifndef BCHP_SDIO_1_CFG_H__ */

/* End of File */