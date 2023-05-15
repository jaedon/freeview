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
 * $brcm_Workfile: bchp_genet_1_intrl2_0.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 10/27/11 10:29p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Thu Oct 27 17:22:57 2011
 *                 MD5 Checksum         d0bb3b528cbe25f62f7a44e82cd25af7
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7425/rdb/b1/bchp_genet_1_intrl2_0.h $
 * 
 * Hydra_Software_Devel/1   10/27/11 10:29p vanessah
 * SW7425-1620: add 7425 B0 rdb header file
 *
 ***************************************************************************/

#ifndef BCHP_GENET_1_INTRL2_0_H__
#define BCHP_GENET_1_INTRL2_0_H__

/***************************************************************************
 *GENET_1_INTRL2_0
 ***************************************************************************/
#define BCHP_GENET_1_INTRL2_0_CPU_STATUS         0x00ba0200 /* CPU interrupt Status Register */
#define BCHP_GENET_1_INTRL2_0_CPU_SET            0x00ba0204 /* CPU interrupt Set Register */
#define BCHP_GENET_1_INTRL2_0_CPU_CLEAR          0x00ba0208 /* CPU interrupt Clear Register */
#define BCHP_GENET_1_INTRL2_0_CPU_MASK_STATUS    0x00ba020c /* CPU interrupt Mask Status Register */
#define BCHP_GENET_1_INTRL2_0_CPU_MASK_SET       0x00ba0210 /* CPU interrupt Mask Set Register */
#define BCHP_GENET_1_INTRL2_0_CPU_MASK_CLEAR     0x00ba0214 /* CPU interrupt Mask Clear Register */
#define BCHP_GENET_1_INTRL2_0_PCI_STATUS         0x00ba0218 /* PCI interrupt Status Register */
#define BCHP_GENET_1_INTRL2_0_PCI_SET            0x00ba021c /* PCI interrupt Set Register */
#define BCHP_GENET_1_INTRL2_0_PCI_CLEAR          0x00ba0220 /* PCI interrupt Clear Register */
#define BCHP_GENET_1_INTRL2_0_PCI_MASK_STATUS    0x00ba0224 /* PCI interrupt Mask Status Register */
#define BCHP_GENET_1_INTRL2_0_PCI_MASK_SET       0x00ba0228 /* PCI interrupt Mask Set Register */
#define BCHP_GENET_1_INTRL2_0_PCI_MASK_CLEAR     0x00ba022c /* PCI interrupt Mask Clear Register */

#endif /* #ifndef BCHP_GENET_1_INTRL2_0_H__ */

/* End of File */