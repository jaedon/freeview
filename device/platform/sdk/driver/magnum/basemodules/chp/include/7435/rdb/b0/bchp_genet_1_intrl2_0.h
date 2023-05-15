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
 * $brcm_Workfile: bchp_genet_1_intrl2_0.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 2/28/12 4:32p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Tue Feb 28 11:03:20 2012
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
 * $brcm_Log: /magnum/basemodules/chp/7435/rdb/b0/bchp_genet_1_intrl2_0.h $
 * 
 * Hydra_Software_Devel/1   2/28/12 4:32p tdo
 * SW7435-40: Need 7435B0 public/central RDB (magnum) headers checked into
 * clearcase
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