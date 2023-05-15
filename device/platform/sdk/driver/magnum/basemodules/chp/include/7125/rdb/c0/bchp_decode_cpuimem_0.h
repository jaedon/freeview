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
 * $brcm_Workfile: bchp_decode_cpuimem_0.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 1/25/10 7:55p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Fri Jan 22 20:25:11 2010
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
 * $brcm_Log: /magnum/basemodules/chp/7125/rdb/c0/bchp_decode_cpuimem_0.h $
 * 
 * Hydra_Software_Devel/1   1/25/10 7:55p albertl
 * SW7125-177: Initial revision.
 *
 ***************************************************************************/

#ifndef BCHP_DECODE_CPUIMEM_0_H__
#define BCHP_DECODE_CPUIMEM_0_H__

/***************************************************************************
 *DECODE_CPUIMEM_0
 ***************************************************************************/
#define BCHP_DECODE_CPUIMEM_0_CPUIMEM_REG        0x00046000 /* CPUIMEM_REG */
#define BCHP_DECODE_CPUIMEM_0_CPUIMEM_END        0x00047ffc /* CPUIMEM_END */

/***************************************************************************
 *CPUIMEM_REG - CPUIMEM_REG
 ***************************************************************************/
/* DECODE_CPUIMEM_0 :: CPUIMEM_REG :: Addr [31:00] */
#define BCHP_DECODE_CPUIMEM_0_CPUIMEM_REG_Addr_MASK                0xffffffff
#define BCHP_DECODE_CPUIMEM_0_CPUIMEM_REG_Addr_SHIFT               0

/***************************************************************************
 *CPUIMEM_END - CPUIMEM_END
 ***************************************************************************/
/* DECODE_CPUIMEM_0 :: CPUIMEM_END :: reserved0 [31:00] */
#define BCHP_DECODE_CPUIMEM_0_CPUIMEM_END_reserved0_MASK           0xffffffff
#define BCHP_DECODE_CPUIMEM_0_CPUIMEM_END_reserved0_SHIFT          0

#endif /* #ifndef BCHP_DECODE_CPUIMEM_0_H__ */

/* End of File */