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
 * $brcm_Workfile: bchp_decode_cpucore_0.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 8/24/10 7:49p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Tue Aug 24 17:47:32 2010
 *                 MD5 Checksum         39761c6e4b9d69cf41e7412c1f6df022
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7231/rdb/a0/bchp_decode_cpucore_0.h $
 * 
 * Hydra_Software_Devel/1   8/24/10 7:49p albertl
 * HW7231-39: Initial revision.
 *
 ***************************************************************************/

#ifndef BCHP_DECODE_CPUCORE_0_H__
#define BCHP_DECODE_CPUCORE_0_H__

/***************************************************************************
 *DECODE_CPUCORE_0
 ***************************************************************************/
#define BCHP_DECODE_CPUCORE_0_CPUCORE_REG        0x00044000 /* CPUCORE_REG */
#define BCHP_DECODE_CPUCORE_0_CPUCORE_END        0x00044ffc /* CPUCORE_END */

/***************************************************************************
 *CPUCORE_REG - CPUCORE_REG
 ***************************************************************************/
/* DECODE_CPUCORE_0 :: CPUCORE_REG :: Addr [31:00] */
#define BCHP_DECODE_CPUCORE_0_CPUCORE_REG_Addr_MASK                0xffffffff
#define BCHP_DECODE_CPUCORE_0_CPUCORE_REG_Addr_SHIFT               0

/***************************************************************************
 *CPUCORE_END - CPUCORE_END
 ***************************************************************************/
/* DECODE_CPUCORE_0 :: CPUCORE_END :: reserved0 [31:00] */
#define BCHP_DECODE_CPUCORE_0_CPUCORE_END_reserved0_MASK           0xffffffff
#define BCHP_DECODE_CPUCORE_0_CPUCORE_END_reserved0_SHIFT          0

#endif /* #ifndef BCHP_DECODE_CPUCORE_0_H__ */

/* End of File */