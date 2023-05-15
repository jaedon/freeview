/***************************************************************************
 *     Copyright (c) 1999-2009, Broadcom Corporation
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
 * $brcm_Date: 9/14/09 5:13p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Fri Sep 11 19:47:07 2009
 *                 MD5 Checksum         957f01e03a68c1766fd2e8ad6484f5f9
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7468/rdb/a0/bchp_xpt_xpu.h $
 * 
 * Hydra_Software_Devel/1   9/14/09 5:13p albertl
 * SW7468-3: Initial revision.
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
#define BCHP_XPT_XPU_IMEMi_ARRAY_BASE                              0x0028c800
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