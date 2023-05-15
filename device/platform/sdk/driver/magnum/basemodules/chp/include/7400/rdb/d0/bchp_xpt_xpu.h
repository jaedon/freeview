/***************************************************************************
 *     Copyright (c) 1999-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bchp_xpt_xpu.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 12/13/07 11:27p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Thu Dec 13 17:35:31 2007
 *                 MD5 Checksum         c3f8e87caf81e56d957436286f3d2c1a
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7400/rdb/d0/bchp_xpt_xpu.h $
 * 
 * Hydra_Software_Devel/1   12/13/07 11:27p pntruong
 * PR32842: Sync header files with rdb central.
 *
 ***************************************************************************/

#ifndef BCHP_XPT_XPU_H__
#define BCHP_XPT_XPU_H__

/***************************************************************************
 *XPT_XPU - XPT XPU Control Registers
 ***************************************************************************/

/***************************************************************************
 *IMEM%i - Instruction memory address 0..2047
 ***************************************************************************/
#define BCHP_XPT_XPU_IMEMi_ARRAY_BASE                              0x00220800
#define BCHP_XPT_XPU_IMEMi_ARRAY_START                             0
#define BCHP_XPT_XPU_IMEMi_ARRAY_END                               2047
#define BCHP_XPT_XPU_IMEMi_ARRAY_ELEMENT_SIZE                      32

/***************************************************************************
 *IMEM%i - Instruction memory address 0..2047
 ***************************************************************************/
/* XPT_XPU :: IMEMi :: reserved0 [31:22] */
#define BCHP_XPT_XPU_IMEMi_reserved0_MASK                          0xffc00000
#define BCHP_XPT_XPU_IMEMi_reserved0_SHIFT                         22

/* XPT_XPU :: IMEMi :: INSTRUCTION [21:00] */
#define BCHP_XPT_XPU_IMEMi_INSTRUCTION_MASK                        0x003fffff
#define BCHP_XPT_XPU_IMEMi_INSTRUCTION_SHIFT                       0


#endif /* #ifndef BCHP_XPT_XPU_H__ */

/* End of File */