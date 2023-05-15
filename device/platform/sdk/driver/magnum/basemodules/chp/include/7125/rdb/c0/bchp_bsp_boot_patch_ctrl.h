/***************************************************************************
 *     Copyright (c) 1999-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bchp_bsp_boot_patch_ctrl.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 1/25/10 7:31p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Fri Jan 22 20:22:10 2010
 *                 MD5 Checksum         a2d1f2163f65e87d228a0fb491cb442d
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7125/rdb/c0/bchp_bsp_boot_patch_ctrl.h $
 * 
 * Hydra_Software_Devel/1   1/25/10 7:31p albertl
 * SW7125-177: Initial revision.
 *
 ***************************************************************************/

#ifndef BCHP_BSP_BOOT_PATCH_CTRL_H__
#define BCHP_BSP_BOOT_PATCH_CTRL_H__

/***************************************************************************
 *BSP_BOOT_PATCH_CTRL - BSP Boot Code Patch Control Register
 ***************************************************************************/
#define BCHP_BSP_BOOT_PATCH_CTRL_ROM_HALF_SELECT 0x00328400 /* Boot Patch ROM (RAM) Half selection register */
#define BCHP_BSP_BOOT_PATCH_CTRL_PATCH_ROM_STATUS 0x00328404 /* Boot Patch ROM (RAM) Load Done Flag Register */
#define BCHP_BSP_BOOT_PATCH_CTRL_TEST_CONFIGURATON 0x00328408 /* Chip Test Configuration Register */

/***************************************************************************
 *ROM_HALF_SELECT - Boot Patch ROM (RAM) Half selection register
 ***************************************************************************/
/* BSP_BOOT_PATCH_CTRL :: ROM_HALF_SELECT :: reserved0 [31:01] */
#define BCHP_BSP_BOOT_PATCH_CTRL_ROM_HALF_SELECT_reserved0_MASK    0xfffffffe
#define BCHP_BSP_BOOT_PATCH_CTRL_ROM_HALF_SELECT_reserved0_SHIFT   1

/* BSP_BOOT_PATCH_CTRL :: ROM_HALF_SELECT :: ROM_HALF_SELECT [00:00] */
#define BCHP_BSP_BOOT_PATCH_CTRL_ROM_HALF_SELECT_ROM_HALF_SELECT_MASK 0x00000001
#define BCHP_BSP_BOOT_PATCH_CTRL_ROM_HALF_SELECT_ROM_HALF_SELECT_SHIFT 0

/***************************************************************************
 *PATCH_ROM_STATUS - Boot Patch ROM (RAM) Load Done Flag Register
 ***************************************************************************/
/* BSP_BOOT_PATCH_CTRL :: PATCH_ROM_STATUS :: reserved0 [31:01] */
#define BCHP_BSP_BOOT_PATCH_CTRL_PATCH_ROM_STATUS_reserved0_MASK   0xfffffffe
#define BCHP_BSP_BOOT_PATCH_CTRL_PATCH_ROM_STATUS_reserved0_SHIFT  1

/* BSP_BOOT_PATCH_CTRL :: PATCH_ROM_STATUS :: ROM_LOAD_DONE [00:00] */
#define BCHP_BSP_BOOT_PATCH_CTRL_PATCH_ROM_STATUS_ROM_LOAD_DONE_MASK 0x00000001
#define BCHP_BSP_BOOT_PATCH_CTRL_PATCH_ROM_STATUS_ROM_LOAD_DONE_SHIFT 0

/***************************************************************************
 *TEST_CONFIGURATON - Chip Test Configuration Register
 ***************************************************************************/
/* BSP_BOOT_PATCH_CTRL :: TEST_CONFIGURATON :: reserved0 [31:04] */
#define BCHP_BSP_BOOT_PATCH_CTRL_TEST_CONFIGURATON_reserved0_MASK  0xfffffff0
#define BCHP_BSP_BOOT_PATCH_CTRL_TEST_CONFIGURATON_reserved0_SHIFT 4

/* BSP_BOOT_PATCH_CTRL :: TEST_CONFIGURATON :: CHIP_TEST_CONFIG [03:00] */
#define BCHP_BSP_BOOT_PATCH_CTRL_TEST_CONFIGURATON_CHIP_TEST_CONFIG_MASK 0x0000000f
#define BCHP_BSP_BOOT_PATCH_CTRL_TEST_CONFIGURATON_CHIP_TEST_CONFIG_SHIFT 0

#endif /* #ifndef BCHP_BSP_BOOT_PATCH_CTRL_H__ */

/* End of File */