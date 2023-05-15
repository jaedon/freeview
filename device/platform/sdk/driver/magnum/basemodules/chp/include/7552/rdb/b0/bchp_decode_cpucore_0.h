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
 * $brcm_Workfile: bchp_decode_cpucore_0.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 8/5/11 9:35p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Thu Aug  4 13:42:18 2011
 *                 MD5 Checksum         83f408cc25eb2d099cc58e22e4e239e9
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7552/rdb/b0/bchp_decode_cpucore_0.h $
 * 
 * Hydra_Software_Devel/1   8/5/11 9:35p albertl
 * SW7552-89: Initial revision.
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