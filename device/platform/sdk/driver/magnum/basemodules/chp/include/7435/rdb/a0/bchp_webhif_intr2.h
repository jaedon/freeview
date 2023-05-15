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
 * $brcm_Workfile: bchp_webhif_intr2.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 12/7/11 5:01p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Tue Dec  6 18:45:39 2011
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
 * $brcm_Log: /magnum/basemodules/chp/7435/rdb/a0/bchp_webhif_intr2.h $
 * 
 * Hydra_Software_Devel/2   12/7/11 5:01p mward
 * SW7435-3: Synced up with central rdb.
 *
 ***************************************************************************/

#ifndef BCHP_WEBHIF_INTR2_H__
#define BCHP_WEBHIF_INTR2_H__

/***************************************************************************
 *WEBHIF_INTR2 - WEBHIF Level 2 Interrupt Controller Registers
 ***************************************************************************/
#define BCHP_WEBHIF_INTR2_CPU_STATUS             0x00420100 /* CPU interrupt Status Register */
#define BCHP_WEBHIF_INTR2_CPU_SET                0x00420104 /* CPU interrupt Set Register */
#define BCHP_WEBHIF_INTR2_CPU_CLEAR              0x00420108 /* CPU interrupt Clear Register */
#define BCHP_WEBHIF_INTR2_CPU_MASK_STATUS        0x0042010c /* CPU interrupt Mask Status Register */
#define BCHP_WEBHIF_INTR2_CPU_MASK_SET           0x00420110 /* CPU interrupt Mask Set Register */
#define BCHP_WEBHIF_INTR2_CPU_MASK_CLEAR         0x00420114 /* CPU interrupt Mask Clear Register */
#define BCHP_WEBHIF_INTR2_PCI_STATUS             0x00420118 /* PCI interrupt Status Register */
#define BCHP_WEBHIF_INTR2_PCI_SET                0x0042011c /* PCI interrupt Set Register */
#define BCHP_WEBHIF_INTR2_PCI_CLEAR              0x00420120 /* PCI interrupt Clear Register */
#define BCHP_WEBHIF_INTR2_PCI_MASK_STATUS        0x00420124 /* PCI interrupt Mask Status Register */
#define BCHP_WEBHIF_INTR2_PCI_MASK_SET           0x00420128 /* PCI interrupt Mask Set Register */
#define BCHP_WEBHIF_INTR2_PCI_MASK_CLEAR         0x0042012c /* PCI interrupt Mask Clear Register */

/***************************************************************************
 *CPU_STATUS - CPU interrupt Status Register
 ***************************************************************************/
/* WEBHIF_INTR2 :: CPU_STATUS :: reserved0 [31:01] */
#define BCHP_WEBHIF_INTR2_CPU_STATUS_reserved0_MASK                0xfffffffe
#define BCHP_WEBHIF_INTR2_CPU_STATUS_reserved0_SHIFT               1

/* WEBHIF_INTR2 :: CPU_STATUS :: WEBHIF_RGR1_BRIDGE_INTR [00:00] */
#define BCHP_WEBHIF_INTR2_CPU_STATUS_WEBHIF_RGR1_BRIDGE_INTR_MASK  0x00000001
#define BCHP_WEBHIF_INTR2_CPU_STATUS_WEBHIF_RGR1_BRIDGE_INTR_SHIFT 0
#define BCHP_WEBHIF_INTR2_CPU_STATUS_WEBHIF_RGR1_BRIDGE_INTR_DEFAULT 0x00000000

/***************************************************************************
 *CPU_SET - CPU interrupt Set Register
 ***************************************************************************/
/* WEBHIF_INTR2 :: CPU_SET :: reserved0 [31:01] */
#define BCHP_WEBHIF_INTR2_CPU_SET_reserved0_MASK                   0xfffffffe
#define BCHP_WEBHIF_INTR2_CPU_SET_reserved0_SHIFT                  1

/* WEBHIF_INTR2 :: CPU_SET :: WEBHIF_RGR1_BRIDGE_INTR [00:00] */
#define BCHP_WEBHIF_INTR2_CPU_SET_WEBHIF_RGR1_BRIDGE_INTR_MASK     0x00000001
#define BCHP_WEBHIF_INTR2_CPU_SET_WEBHIF_RGR1_BRIDGE_INTR_SHIFT    0
#define BCHP_WEBHIF_INTR2_CPU_SET_WEBHIF_RGR1_BRIDGE_INTR_DEFAULT  0x00000000

/***************************************************************************
 *CPU_CLEAR - CPU interrupt Clear Register
 ***************************************************************************/
/* WEBHIF_INTR2 :: CPU_CLEAR :: reserved0 [31:01] */
#define BCHP_WEBHIF_INTR2_CPU_CLEAR_reserved0_MASK                 0xfffffffe
#define BCHP_WEBHIF_INTR2_CPU_CLEAR_reserved0_SHIFT                1

/* WEBHIF_INTR2 :: CPU_CLEAR :: WEBHIF_RGR1_BRIDGE_INTR [00:00] */
#define BCHP_WEBHIF_INTR2_CPU_CLEAR_WEBHIF_RGR1_BRIDGE_INTR_MASK   0x00000001
#define BCHP_WEBHIF_INTR2_CPU_CLEAR_WEBHIF_RGR1_BRIDGE_INTR_SHIFT  0
#define BCHP_WEBHIF_INTR2_CPU_CLEAR_WEBHIF_RGR1_BRIDGE_INTR_DEFAULT 0x00000000

/***************************************************************************
 *CPU_MASK_STATUS - CPU interrupt Mask Status Register
 ***************************************************************************/
/* WEBHIF_INTR2 :: CPU_MASK_STATUS :: reserved0 [31:01] */
#define BCHP_WEBHIF_INTR2_CPU_MASK_STATUS_reserved0_MASK           0xfffffffe
#define BCHP_WEBHIF_INTR2_CPU_MASK_STATUS_reserved0_SHIFT          1

/* WEBHIF_INTR2 :: CPU_MASK_STATUS :: WEBHIF_RGR_BRIDGE_INTR [00:00] */
#define BCHP_WEBHIF_INTR2_CPU_MASK_STATUS_WEBHIF_RGR_BRIDGE_INTR_MASK 0x00000001
#define BCHP_WEBHIF_INTR2_CPU_MASK_STATUS_WEBHIF_RGR_BRIDGE_INTR_SHIFT 0
#define BCHP_WEBHIF_INTR2_CPU_MASK_STATUS_WEBHIF_RGR_BRIDGE_INTR_DEFAULT 0x00000001

/***************************************************************************
 *CPU_MASK_SET - CPU interrupt Mask Set Register
 ***************************************************************************/
/* WEBHIF_INTR2 :: CPU_MASK_SET :: reserved0 [31:01] */
#define BCHP_WEBHIF_INTR2_CPU_MASK_SET_reserved0_MASK              0xfffffffe
#define BCHP_WEBHIF_INTR2_CPU_MASK_SET_reserved0_SHIFT             1

/* WEBHIF_INTR2 :: CPU_MASK_SET :: WEBHIF_RGR_BRIDGE_INTR [00:00] */
#define BCHP_WEBHIF_INTR2_CPU_MASK_SET_WEBHIF_RGR_BRIDGE_INTR_MASK 0x00000001
#define BCHP_WEBHIF_INTR2_CPU_MASK_SET_WEBHIF_RGR_BRIDGE_INTR_SHIFT 0
#define BCHP_WEBHIF_INTR2_CPU_MASK_SET_WEBHIF_RGR_BRIDGE_INTR_DEFAULT 0x00000001

/***************************************************************************
 *CPU_MASK_CLEAR - CPU interrupt Mask Clear Register
 ***************************************************************************/
/* WEBHIF_INTR2 :: CPU_MASK_CLEAR :: reserved0 [31:01] */
#define BCHP_WEBHIF_INTR2_CPU_MASK_CLEAR_reserved0_MASK            0xfffffffe
#define BCHP_WEBHIF_INTR2_CPU_MASK_CLEAR_reserved0_SHIFT           1

/* WEBHIF_INTR2 :: CPU_MASK_CLEAR :: WEBHIF_RGR_BRIDGE_INTR [00:00] */
#define BCHP_WEBHIF_INTR2_CPU_MASK_CLEAR_WEBHIF_RGR_BRIDGE_INTR_MASK 0x00000001
#define BCHP_WEBHIF_INTR2_CPU_MASK_CLEAR_WEBHIF_RGR_BRIDGE_INTR_SHIFT 0
#define BCHP_WEBHIF_INTR2_CPU_MASK_CLEAR_WEBHIF_RGR_BRIDGE_INTR_DEFAULT 0x00000001

/***************************************************************************
 *PCI_STATUS - PCI interrupt Status Register
 ***************************************************************************/
/* WEBHIF_INTR2 :: PCI_STATUS :: reserved0 [31:01] */
#define BCHP_WEBHIF_INTR2_PCI_STATUS_reserved0_MASK                0xfffffffe
#define BCHP_WEBHIF_INTR2_PCI_STATUS_reserved0_SHIFT               1

/* WEBHIF_INTR2 :: PCI_STATUS :: WEBHIF_RGR1_BRIDGE_INTR [00:00] */
#define BCHP_WEBHIF_INTR2_PCI_STATUS_WEBHIF_RGR1_BRIDGE_INTR_MASK  0x00000001
#define BCHP_WEBHIF_INTR2_PCI_STATUS_WEBHIF_RGR1_BRIDGE_INTR_SHIFT 0
#define BCHP_WEBHIF_INTR2_PCI_STATUS_WEBHIF_RGR1_BRIDGE_INTR_DEFAULT 0x00000000

/***************************************************************************
 *PCI_SET - PCI interrupt Set Register
 ***************************************************************************/
/* WEBHIF_INTR2 :: PCI_SET :: reserved0 [31:01] */
#define BCHP_WEBHIF_INTR2_PCI_SET_reserved0_MASK                   0xfffffffe
#define BCHP_WEBHIF_INTR2_PCI_SET_reserved0_SHIFT                  1

/* WEBHIF_INTR2 :: PCI_SET :: WEBHIF_RGR1_BRIDGE_INTR [00:00] */
#define BCHP_WEBHIF_INTR2_PCI_SET_WEBHIF_RGR1_BRIDGE_INTR_MASK     0x00000001
#define BCHP_WEBHIF_INTR2_PCI_SET_WEBHIF_RGR1_BRIDGE_INTR_SHIFT    0
#define BCHP_WEBHIF_INTR2_PCI_SET_WEBHIF_RGR1_BRIDGE_INTR_DEFAULT  0x00000000

/***************************************************************************
 *PCI_CLEAR - PCI interrupt Clear Register
 ***************************************************************************/
/* WEBHIF_INTR2 :: PCI_CLEAR :: reserved0 [31:01] */
#define BCHP_WEBHIF_INTR2_PCI_CLEAR_reserved0_MASK                 0xfffffffe
#define BCHP_WEBHIF_INTR2_PCI_CLEAR_reserved0_SHIFT                1

/* WEBHIF_INTR2 :: PCI_CLEAR :: WEBHIF_RGR1_BRIDGE_INTR [00:00] */
#define BCHP_WEBHIF_INTR2_PCI_CLEAR_WEBHIF_RGR1_BRIDGE_INTR_MASK   0x00000001
#define BCHP_WEBHIF_INTR2_PCI_CLEAR_WEBHIF_RGR1_BRIDGE_INTR_SHIFT  0
#define BCHP_WEBHIF_INTR2_PCI_CLEAR_WEBHIF_RGR1_BRIDGE_INTR_DEFAULT 0x00000000

/***************************************************************************
 *PCI_MASK_STATUS - PCI interrupt Mask Status Register
 ***************************************************************************/
/* WEBHIF_INTR2 :: PCI_MASK_STATUS :: reserved0 [31:01] */
#define BCHP_WEBHIF_INTR2_PCI_MASK_STATUS_reserved0_MASK           0xfffffffe
#define BCHP_WEBHIF_INTR2_PCI_MASK_STATUS_reserved0_SHIFT          1

/* WEBHIF_INTR2 :: PCI_MASK_STATUS :: WEBHIF_RGR_BRIDGE_INTR [00:00] */
#define BCHP_WEBHIF_INTR2_PCI_MASK_STATUS_WEBHIF_RGR_BRIDGE_INTR_MASK 0x00000001
#define BCHP_WEBHIF_INTR2_PCI_MASK_STATUS_WEBHIF_RGR_BRIDGE_INTR_SHIFT 0
#define BCHP_WEBHIF_INTR2_PCI_MASK_STATUS_WEBHIF_RGR_BRIDGE_INTR_DEFAULT 0x00000001

/***************************************************************************
 *PCI_MASK_SET - PCI interrupt Mask Set Register
 ***************************************************************************/
/* WEBHIF_INTR2 :: PCI_MASK_SET :: reserved0 [31:01] */
#define BCHP_WEBHIF_INTR2_PCI_MASK_SET_reserved0_MASK              0xfffffffe
#define BCHP_WEBHIF_INTR2_PCI_MASK_SET_reserved0_SHIFT             1

/* WEBHIF_INTR2 :: PCI_MASK_SET :: WEBHIF_RGR_BRIDGE_INTR [00:00] */
#define BCHP_WEBHIF_INTR2_PCI_MASK_SET_WEBHIF_RGR_BRIDGE_INTR_MASK 0x00000001
#define BCHP_WEBHIF_INTR2_PCI_MASK_SET_WEBHIF_RGR_BRIDGE_INTR_SHIFT 0
#define BCHP_WEBHIF_INTR2_PCI_MASK_SET_WEBHIF_RGR_BRIDGE_INTR_DEFAULT 0x00000001

/***************************************************************************
 *PCI_MASK_CLEAR - PCI interrupt Mask Clear Register
 ***************************************************************************/
/* WEBHIF_INTR2 :: PCI_MASK_CLEAR :: reserved0 [31:01] */
#define BCHP_WEBHIF_INTR2_PCI_MASK_CLEAR_reserved0_MASK            0xfffffffe
#define BCHP_WEBHIF_INTR2_PCI_MASK_CLEAR_reserved0_SHIFT           1

/* WEBHIF_INTR2 :: PCI_MASK_CLEAR :: WEBHIF_RGR_BRIDGE_INTR [00:00] */
#define BCHP_WEBHIF_INTR2_PCI_MASK_CLEAR_WEBHIF_RGR_BRIDGE_INTR_MASK 0x00000001
#define BCHP_WEBHIF_INTR2_PCI_MASK_CLEAR_WEBHIF_RGR_BRIDGE_INTR_SHIFT 0
#define BCHP_WEBHIF_INTR2_PCI_MASK_CLEAR_WEBHIF_RGR_BRIDGE_INTR_DEFAULT 0x00000001

#endif /* #ifndef BCHP_WEBHIF_INTR2_H__ */

/* End of File */