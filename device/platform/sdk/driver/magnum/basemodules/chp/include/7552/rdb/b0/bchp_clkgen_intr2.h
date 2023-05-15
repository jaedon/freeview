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
 * $brcm_Workfile: bchp_clkgen_intr2.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 2/7/12 1:22p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Tue Feb  7 10:59:54 2012
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
 * $brcm_Log: /magnum/basemodules/chp/7552/rdb/b0/bchp_clkgen_intr2.h $
 * 
 * Hydra_Software_Devel/2   2/7/12 1:22p pntruong
 * SW7552-89: Synced up with central rdb.
 *
 ***************************************************************************/

#ifndef BCHP_CLKGEN_INTR2_H__
#define BCHP_CLKGEN_INTR2_H__

/***************************************************************************
 *CLKGEN_INTR2 - Avs Interrupt Level2 Controller
 ***************************************************************************/
#define BCHP_CLKGEN_INTR2_CPU_STATUS             0x00424000 /* CPU Interrupt Status Register */
#define BCHP_CLKGEN_INTR2_CPU_SET                0x00424004 /* CPU interrupt Set Register */
#define BCHP_CLKGEN_INTR2_CPU_CLEAR              0x00424008 /* CPU interrupt Clear Register */
#define BCHP_CLKGEN_INTR2_CPU_MASK_STATUS        0x0042400c /* CPU interrupt Mask Status Register */
#define BCHP_CLKGEN_INTR2_CPU_MASK_SET           0x00424010 /* CPU interrupt Mask Set Register */
#define BCHP_CLKGEN_INTR2_CPU_MASK_CLEAR         0x00424014 /* CPU interrupt Mask Clear Register */
#define BCHP_CLKGEN_INTR2_PCI_STATUS             0x00424018 /* PCI interrupt Status Register */
#define BCHP_CLKGEN_INTR2_PCI_SET                0x0042401c /* PCI interrupt Set Register */
#define BCHP_CLKGEN_INTR2_PCI_CLEAR              0x00424020 /* PCI interrupt Clear Register */
#define BCHP_CLKGEN_INTR2_PCI_MASK_STATUS        0x00424024 /* PCI interrupt Mask Status Register */
#define BCHP_CLKGEN_INTR2_PCI_MASK_SET           0x00424028 /* PCI interrupt Mask Set Register */
#define BCHP_CLKGEN_INTR2_PCI_MASK_CLEAR         0x0042402c /* PCI interrupt Mask Clear Register */

/***************************************************************************
 *CPU_STATUS - CPU Interrupt Status Register
 ***************************************************************************/
/* CLKGEN_INTR2 :: CPU_STATUS :: reserved0 [31:05] */
#define BCHP_CLKGEN_INTR2_CPU_STATUS_reserved0_MASK                0xffffffe0
#define BCHP_CLKGEN_INTR2_CPU_STATUS_reserved0_SHIFT               5

/* CLKGEN_INTR2 :: CPU_STATUS :: GISB_INTR [04:04] */
#define BCHP_CLKGEN_INTR2_CPU_STATUS_GISB_INTR_MASK                0x00000010
#define BCHP_CLKGEN_INTR2_CPU_STATUS_GISB_INTR_SHIFT               4
#define BCHP_CLKGEN_INTR2_CPU_STATUS_GISB_INTR_DEFAULT             0x00000000

/* CLKGEN_INTR2 :: CPU_STATUS :: AVS_INTR_SW_MEAS_DONE [03:03] */
#define BCHP_CLKGEN_INTR2_CPU_STATUS_AVS_INTR_SW_MEAS_DONE_MASK    0x00000008
#define BCHP_CLKGEN_INTR2_CPU_STATUS_AVS_INTR_SW_MEAS_DONE_SHIFT   3
#define BCHP_CLKGEN_INTR2_CPU_STATUS_AVS_INTR_SW_MEAS_DONE_DEFAULT 0x00000000

/* CLKGEN_INTR2 :: CPU_STATUS :: AVS_INTR_PWD [02:02] */
#define BCHP_CLKGEN_INTR2_CPU_STATUS_AVS_INTR_PWD_MASK             0x00000004
#define BCHP_CLKGEN_INTR2_CPU_STATUS_AVS_INTR_PWD_SHIFT            2
#define BCHP_CLKGEN_INTR2_CPU_STATUS_AVS_INTR_PWD_DEFAULT          0x00000000

/* CLKGEN_INTR2 :: CPU_STATUS :: AVS_INTR_THRESHOLD2 [01:01] */
#define BCHP_CLKGEN_INTR2_CPU_STATUS_AVS_INTR_THRESHOLD2_MASK      0x00000002
#define BCHP_CLKGEN_INTR2_CPU_STATUS_AVS_INTR_THRESHOLD2_SHIFT     1
#define BCHP_CLKGEN_INTR2_CPU_STATUS_AVS_INTR_THRESHOLD2_DEFAULT   0x00000000

/* CLKGEN_INTR2 :: CPU_STATUS :: AVS_INTR_THRESHOLD1 [00:00] */
#define BCHP_CLKGEN_INTR2_CPU_STATUS_AVS_INTR_THRESHOLD1_MASK      0x00000001
#define BCHP_CLKGEN_INTR2_CPU_STATUS_AVS_INTR_THRESHOLD1_SHIFT     0
#define BCHP_CLKGEN_INTR2_CPU_STATUS_AVS_INTR_THRESHOLD1_DEFAULT   0x00000000

/***************************************************************************
 *CPU_SET - CPU interrupt Set Register
 ***************************************************************************/
/* CLKGEN_INTR2 :: CPU_SET :: reserved0 [31:05] */
#define BCHP_CLKGEN_INTR2_CPU_SET_reserved0_MASK                   0xffffffe0
#define BCHP_CLKGEN_INTR2_CPU_SET_reserved0_SHIFT                  5

/* CLKGEN_INTR2 :: CPU_SET :: GISB_INTR [04:04] */
#define BCHP_CLKGEN_INTR2_CPU_SET_GISB_INTR_MASK                   0x00000010
#define BCHP_CLKGEN_INTR2_CPU_SET_GISB_INTR_SHIFT                  4
#define BCHP_CLKGEN_INTR2_CPU_SET_GISB_INTR_DEFAULT                0x00000000

/* CLKGEN_INTR2 :: CPU_SET :: AVS_INTR_SW_MEAS_DONE [03:03] */
#define BCHP_CLKGEN_INTR2_CPU_SET_AVS_INTR_SW_MEAS_DONE_MASK       0x00000008
#define BCHP_CLKGEN_INTR2_CPU_SET_AVS_INTR_SW_MEAS_DONE_SHIFT      3
#define BCHP_CLKGEN_INTR2_CPU_SET_AVS_INTR_SW_MEAS_DONE_DEFAULT    0x00000000

/* CLKGEN_INTR2 :: CPU_SET :: AVS_INTR_PWD [02:02] */
#define BCHP_CLKGEN_INTR2_CPU_SET_AVS_INTR_PWD_MASK                0x00000004
#define BCHP_CLKGEN_INTR2_CPU_SET_AVS_INTR_PWD_SHIFT               2
#define BCHP_CLKGEN_INTR2_CPU_SET_AVS_INTR_PWD_DEFAULT             0x00000000

/* CLKGEN_INTR2 :: CPU_SET :: AVS_INTR_THRESHOLD2 [01:01] */
#define BCHP_CLKGEN_INTR2_CPU_SET_AVS_INTR_THRESHOLD2_MASK         0x00000002
#define BCHP_CLKGEN_INTR2_CPU_SET_AVS_INTR_THRESHOLD2_SHIFT        1
#define BCHP_CLKGEN_INTR2_CPU_SET_AVS_INTR_THRESHOLD2_DEFAULT      0x00000000

/* CLKGEN_INTR2 :: CPU_SET :: AVS_INTR_THRESHOLD1 [00:00] */
#define BCHP_CLKGEN_INTR2_CPU_SET_AVS_INTR_THRESHOLD1_MASK         0x00000001
#define BCHP_CLKGEN_INTR2_CPU_SET_AVS_INTR_THRESHOLD1_SHIFT        0
#define BCHP_CLKGEN_INTR2_CPU_SET_AVS_INTR_THRESHOLD1_DEFAULT      0x00000000

/***************************************************************************
 *CPU_CLEAR - CPU interrupt Clear Register
 ***************************************************************************/
/* CLKGEN_INTR2 :: CPU_CLEAR :: reserved0 [31:05] */
#define BCHP_CLKGEN_INTR2_CPU_CLEAR_reserved0_MASK                 0xffffffe0
#define BCHP_CLKGEN_INTR2_CPU_CLEAR_reserved0_SHIFT                5

/* CLKGEN_INTR2 :: CPU_CLEAR :: GISB_INTR [04:04] */
#define BCHP_CLKGEN_INTR2_CPU_CLEAR_GISB_INTR_MASK                 0x00000010
#define BCHP_CLKGEN_INTR2_CPU_CLEAR_GISB_INTR_SHIFT                4
#define BCHP_CLKGEN_INTR2_CPU_CLEAR_GISB_INTR_DEFAULT              0x00000000

/* CLKGEN_INTR2 :: CPU_CLEAR :: AVS_INTR_SW_MEAS_DONE [03:03] */
#define BCHP_CLKGEN_INTR2_CPU_CLEAR_AVS_INTR_SW_MEAS_DONE_MASK     0x00000008
#define BCHP_CLKGEN_INTR2_CPU_CLEAR_AVS_INTR_SW_MEAS_DONE_SHIFT    3
#define BCHP_CLKGEN_INTR2_CPU_CLEAR_AVS_INTR_SW_MEAS_DONE_DEFAULT  0x00000000

/* CLKGEN_INTR2 :: CPU_CLEAR :: AVS_INTR_PWD [02:02] */
#define BCHP_CLKGEN_INTR2_CPU_CLEAR_AVS_INTR_PWD_MASK              0x00000004
#define BCHP_CLKGEN_INTR2_CPU_CLEAR_AVS_INTR_PWD_SHIFT             2
#define BCHP_CLKGEN_INTR2_CPU_CLEAR_AVS_INTR_PWD_DEFAULT           0x00000000

/* CLKGEN_INTR2 :: CPU_CLEAR :: AVS_INTR_THRESHOLD2 [01:01] */
#define BCHP_CLKGEN_INTR2_CPU_CLEAR_AVS_INTR_THRESHOLD2_MASK       0x00000002
#define BCHP_CLKGEN_INTR2_CPU_CLEAR_AVS_INTR_THRESHOLD2_SHIFT      1
#define BCHP_CLKGEN_INTR2_CPU_CLEAR_AVS_INTR_THRESHOLD2_DEFAULT    0x00000000

/* CLKGEN_INTR2 :: CPU_CLEAR :: AVS_INTR_THRESHOLD1 [00:00] */
#define BCHP_CLKGEN_INTR2_CPU_CLEAR_AVS_INTR_THRESHOLD1_MASK       0x00000001
#define BCHP_CLKGEN_INTR2_CPU_CLEAR_AVS_INTR_THRESHOLD1_SHIFT      0
#define BCHP_CLKGEN_INTR2_CPU_CLEAR_AVS_INTR_THRESHOLD1_DEFAULT    0x00000000

/***************************************************************************
 *CPU_MASK_STATUS - CPU interrupt Mask Status Register
 ***************************************************************************/
/* CLKGEN_INTR2 :: CPU_MASK_STATUS :: reserved0 [31:05] */
#define BCHP_CLKGEN_INTR2_CPU_MASK_STATUS_reserved0_MASK           0xffffffe0
#define BCHP_CLKGEN_INTR2_CPU_MASK_STATUS_reserved0_SHIFT          5

/* CLKGEN_INTR2 :: CPU_MASK_STATUS :: GISB_INTR [04:04] */
#define BCHP_CLKGEN_INTR2_CPU_MASK_STATUS_GISB_INTR_MASK           0x00000010
#define BCHP_CLKGEN_INTR2_CPU_MASK_STATUS_GISB_INTR_SHIFT          4
#define BCHP_CLKGEN_INTR2_CPU_MASK_STATUS_GISB_INTR_DEFAULT        0x00000001

/* CLKGEN_INTR2 :: CPU_MASK_STATUS :: AVS_INTR_SW_MEAS_DONE [03:03] */
#define BCHP_CLKGEN_INTR2_CPU_MASK_STATUS_AVS_INTR_SW_MEAS_DONE_MASK 0x00000008
#define BCHP_CLKGEN_INTR2_CPU_MASK_STATUS_AVS_INTR_SW_MEAS_DONE_SHIFT 3
#define BCHP_CLKGEN_INTR2_CPU_MASK_STATUS_AVS_INTR_SW_MEAS_DONE_DEFAULT 0x00000001

/* CLKGEN_INTR2 :: CPU_MASK_STATUS :: AVS_INTR_PWD [02:02] */
#define BCHP_CLKGEN_INTR2_CPU_MASK_STATUS_AVS_INTR_PWD_MASK        0x00000004
#define BCHP_CLKGEN_INTR2_CPU_MASK_STATUS_AVS_INTR_PWD_SHIFT       2
#define BCHP_CLKGEN_INTR2_CPU_MASK_STATUS_AVS_INTR_PWD_DEFAULT     0x00000001

/* CLKGEN_INTR2 :: CPU_MASK_STATUS :: AVS_INTR_THRESHOLD2 [01:01] */
#define BCHP_CLKGEN_INTR2_CPU_MASK_STATUS_AVS_INTR_THRESHOLD2_MASK 0x00000002
#define BCHP_CLKGEN_INTR2_CPU_MASK_STATUS_AVS_INTR_THRESHOLD2_SHIFT 1
#define BCHP_CLKGEN_INTR2_CPU_MASK_STATUS_AVS_INTR_THRESHOLD2_DEFAULT 0x00000001

/* CLKGEN_INTR2 :: CPU_MASK_STATUS :: AVS_INTR_THRESHOLD1 [00:00] */
#define BCHP_CLKGEN_INTR2_CPU_MASK_STATUS_AVS_INTR_THRESHOLD1_MASK 0x00000001
#define BCHP_CLKGEN_INTR2_CPU_MASK_STATUS_AVS_INTR_THRESHOLD1_SHIFT 0
#define BCHP_CLKGEN_INTR2_CPU_MASK_STATUS_AVS_INTR_THRESHOLD1_DEFAULT 0x00000001

/***************************************************************************
 *CPU_MASK_SET - CPU interrupt Mask Set Register
 ***************************************************************************/
/* CLKGEN_INTR2 :: CPU_MASK_SET :: reserved0 [31:05] */
#define BCHP_CLKGEN_INTR2_CPU_MASK_SET_reserved0_MASK              0xffffffe0
#define BCHP_CLKGEN_INTR2_CPU_MASK_SET_reserved0_SHIFT             5

/* CLKGEN_INTR2 :: CPU_MASK_SET :: GISB_INTR [04:04] */
#define BCHP_CLKGEN_INTR2_CPU_MASK_SET_GISB_INTR_MASK              0x00000010
#define BCHP_CLKGEN_INTR2_CPU_MASK_SET_GISB_INTR_SHIFT             4
#define BCHP_CLKGEN_INTR2_CPU_MASK_SET_GISB_INTR_DEFAULT           0x00000001

/* CLKGEN_INTR2 :: CPU_MASK_SET :: AVS_INTR_SW_MEAS_DONE [03:03] */
#define BCHP_CLKGEN_INTR2_CPU_MASK_SET_AVS_INTR_SW_MEAS_DONE_MASK  0x00000008
#define BCHP_CLKGEN_INTR2_CPU_MASK_SET_AVS_INTR_SW_MEAS_DONE_SHIFT 3
#define BCHP_CLKGEN_INTR2_CPU_MASK_SET_AVS_INTR_SW_MEAS_DONE_DEFAULT 0x00000001

/* CLKGEN_INTR2 :: CPU_MASK_SET :: AVS_INTR_PWD [02:02] */
#define BCHP_CLKGEN_INTR2_CPU_MASK_SET_AVS_INTR_PWD_MASK           0x00000004
#define BCHP_CLKGEN_INTR2_CPU_MASK_SET_AVS_INTR_PWD_SHIFT          2
#define BCHP_CLKGEN_INTR2_CPU_MASK_SET_AVS_INTR_PWD_DEFAULT        0x00000001

/* CLKGEN_INTR2 :: CPU_MASK_SET :: AVS_INTR_THRESHOLD2 [01:01] */
#define BCHP_CLKGEN_INTR2_CPU_MASK_SET_AVS_INTR_THRESHOLD2_MASK    0x00000002
#define BCHP_CLKGEN_INTR2_CPU_MASK_SET_AVS_INTR_THRESHOLD2_SHIFT   1
#define BCHP_CLKGEN_INTR2_CPU_MASK_SET_AVS_INTR_THRESHOLD2_DEFAULT 0x00000001

/* CLKGEN_INTR2 :: CPU_MASK_SET :: AVS_INTR_THRESHOLD1 [00:00] */
#define BCHP_CLKGEN_INTR2_CPU_MASK_SET_AVS_INTR_THRESHOLD1_MASK    0x00000001
#define BCHP_CLKGEN_INTR2_CPU_MASK_SET_AVS_INTR_THRESHOLD1_SHIFT   0
#define BCHP_CLKGEN_INTR2_CPU_MASK_SET_AVS_INTR_THRESHOLD1_DEFAULT 0x00000001

/***************************************************************************
 *CPU_MASK_CLEAR - CPU interrupt Mask Clear Register
 ***************************************************************************/
/* CLKGEN_INTR2 :: CPU_MASK_CLEAR :: reserved0 [31:05] */
#define BCHP_CLKGEN_INTR2_CPU_MASK_CLEAR_reserved0_MASK            0xffffffe0
#define BCHP_CLKGEN_INTR2_CPU_MASK_CLEAR_reserved0_SHIFT           5

/* CLKGEN_INTR2 :: CPU_MASK_CLEAR :: GISB_INTR [04:04] */
#define BCHP_CLKGEN_INTR2_CPU_MASK_CLEAR_GISB_INTR_MASK            0x00000010
#define BCHP_CLKGEN_INTR2_CPU_MASK_CLEAR_GISB_INTR_SHIFT           4
#define BCHP_CLKGEN_INTR2_CPU_MASK_CLEAR_GISB_INTR_DEFAULT         0x00000001

/* CLKGEN_INTR2 :: CPU_MASK_CLEAR :: AVS_INTR_SW_MEAS_DONE [03:03] */
#define BCHP_CLKGEN_INTR2_CPU_MASK_CLEAR_AVS_INTR_SW_MEAS_DONE_MASK 0x00000008
#define BCHP_CLKGEN_INTR2_CPU_MASK_CLEAR_AVS_INTR_SW_MEAS_DONE_SHIFT 3
#define BCHP_CLKGEN_INTR2_CPU_MASK_CLEAR_AVS_INTR_SW_MEAS_DONE_DEFAULT 0x00000001

/* CLKGEN_INTR2 :: CPU_MASK_CLEAR :: AVS_INTR_PWD [02:02] */
#define BCHP_CLKGEN_INTR2_CPU_MASK_CLEAR_AVS_INTR_PWD_MASK         0x00000004
#define BCHP_CLKGEN_INTR2_CPU_MASK_CLEAR_AVS_INTR_PWD_SHIFT        2
#define BCHP_CLKGEN_INTR2_CPU_MASK_CLEAR_AVS_INTR_PWD_DEFAULT      0x00000001

/* CLKGEN_INTR2 :: CPU_MASK_CLEAR :: AVS_INTR_THRESHOLD2 [01:01] */
#define BCHP_CLKGEN_INTR2_CPU_MASK_CLEAR_AVS_INTR_THRESHOLD2_MASK  0x00000002
#define BCHP_CLKGEN_INTR2_CPU_MASK_CLEAR_AVS_INTR_THRESHOLD2_SHIFT 1
#define BCHP_CLKGEN_INTR2_CPU_MASK_CLEAR_AVS_INTR_THRESHOLD2_DEFAULT 0x00000001

/* CLKGEN_INTR2 :: CPU_MASK_CLEAR :: AVS_INTR_THRESHOLD1 [00:00] */
#define BCHP_CLKGEN_INTR2_CPU_MASK_CLEAR_AVS_INTR_THRESHOLD1_MASK  0x00000001
#define BCHP_CLKGEN_INTR2_CPU_MASK_CLEAR_AVS_INTR_THRESHOLD1_SHIFT 0
#define BCHP_CLKGEN_INTR2_CPU_MASK_CLEAR_AVS_INTR_THRESHOLD1_DEFAULT 0x00000001

/***************************************************************************
 *PCI_STATUS - PCI interrupt Status Register
 ***************************************************************************/
/* CLKGEN_INTR2 :: PCI_STATUS :: reserved0 [31:05] */
#define BCHP_CLKGEN_INTR2_PCI_STATUS_reserved0_MASK                0xffffffe0
#define BCHP_CLKGEN_INTR2_PCI_STATUS_reserved0_SHIFT               5

/* CLKGEN_INTR2 :: PCI_STATUS :: GISB_INTR [04:04] */
#define BCHP_CLKGEN_INTR2_PCI_STATUS_GISB_INTR_MASK                0x00000010
#define BCHP_CLKGEN_INTR2_PCI_STATUS_GISB_INTR_SHIFT               4
#define BCHP_CLKGEN_INTR2_PCI_STATUS_GISB_INTR_DEFAULT             0x00000000

/* CLKGEN_INTR2 :: PCI_STATUS :: AVS_INTR_SW_MEAS_DONE [03:03] */
#define BCHP_CLKGEN_INTR2_PCI_STATUS_AVS_INTR_SW_MEAS_DONE_MASK    0x00000008
#define BCHP_CLKGEN_INTR2_PCI_STATUS_AVS_INTR_SW_MEAS_DONE_SHIFT   3
#define BCHP_CLKGEN_INTR2_PCI_STATUS_AVS_INTR_SW_MEAS_DONE_DEFAULT 0x00000000

/* CLKGEN_INTR2 :: PCI_STATUS :: AVS_INTR_PWD [02:02] */
#define BCHP_CLKGEN_INTR2_PCI_STATUS_AVS_INTR_PWD_MASK             0x00000004
#define BCHP_CLKGEN_INTR2_PCI_STATUS_AVS_INTR_PWD_SHIFT            2
#define BCHP_CLKGEN_INTR2_PCI_STATUS_AVS_INTR_PWD_DEFAULT          0x00000000

/* CLKGEN_INTR2 :: PCI_STATUS :: AVS_INTR_THRESHOLD2 [01:01] */
#define BCHP_CLKGEN_INTR2_PCI_STATUS_AVS_INTR_THRESHOLD2_MASK      0x00000002
#define BCHP_CLKGEN_INTR2_PCI_STATUS_AVS_INTR_THRESHOLD2_SHIFT     1
#define BCHP_CLKGEN_INTR2_PCI_STATUS_AVS_INTR_THRESHOLD2_DEFAULT   0x00000000

/* CLKGEN_INTR2 :: PCI_STATUS :: AVS_INTR_THRESHOLD1 [00:00] */
#define BCHP_CLKGEN_INTR2_PCI_STATUS_AVS_INTR_THRESHOLD1_MASK      0x00000001
#define BCHP_CLKGEN_INTR2_PCI_STATUS_AVS_INTR_THRESHOLD1_SHIFT     0
#define BCHP_CLKGEN_INTR2_PCI_STATUS_AVS_INTR_THRESHOLD1_DEFAULT   0x00000000

/***************************************************************************
 *PCI_SET - PCI interrupt Set Register
 ***************************************************************************/
/* CLKGEN_INTR2 :: PCI_SET :: reserved0 [31:05] */
#define BCHP_CLKGEN_INTR2_PCI_SET_reserved0_MASK                   0xffffffe0
#define BCHP_CLKGEN_INTR2_PCI_SET_reserved0_SHIFT                  5

/* CLKGEN_INTR2 :: PCI_SET :: GISB_INTR [04:04] */
#define BCHP_CLKGEN_INTR2_PCI_SET_GISB_INTR_MASK                   0x00000010
#define BCHP_CLKGEN_INTR2_PCI_SET_GISB_INTR_SHIFT                  4
#define BCHP_CLKGEN_INTR2_PCI_SET_GISB_INTR_DEFAULT                0x00000000

/* CLKGEN_INTR2 :: PCI_SET :: AVS_INTR_SW_MEAS_DONE [03:03] */
#define BCHP_CLKGEN_INTR2_PCI_SET_AVS_INTR_SW_MEAS_DONE_MASK       0x00000008
#define BCHP_CLKGEN_INTR2_PCI_SET_AVS_INTR_SW_MEAS_DONE_SHIFT      3
#define BCHP_CLKGEN_INTR2_PCI_SET_AVS_INTR_SW_MEAS_DONE_DEFAULT    0x00000000

/* CLKGEN_INTR2 :: PCI_SET :: AVS_INTR_PWD [02:02] */
#define BCHP_CLKGEN_INTR2_PCI_SET_AVS_INTR_PWD_MASK                0x00000004
#define BCHP_CLKGEN_INTR2_PCI_SET_AVS_INTR_PWD_SHIFT               2
#define BCHP_CLKGEN_INTR2_PCI_SET_AVS_INTR_PWD_DEFAULT             0x00000000

/* CLKGEN_INTR2 :: PCI_SET :: AVS_INTR_THRESHOLD2 [01:01] */
#define BCHP_CLKGEN_INTR2_PCI_SET_AVS_INTR_THRESHOLD2_MASK         0x00000002
#define BCHP_CLKGEN_INTR2_PCI_SET_AVS_INTR_THRESHOLD2_SHIFT        1
#define BCHP_CLKGEN_INTR2_PCI_SET_AVS_INTR_THRESHOLD2_DEFAULT      0x00000000

/* CLKGEN_INTR2 :: PCI_SET :: AVS_INTR_THRESHOLD1 [00:00] */
#define BCHP_CLKGEN_INTR2_PCI_SET_AVS_INTR_THRESHOLD1_MASK         0x00000001
#define BCHP_CLKGEN_INTR2_PCI_SET_AVS_INTR_THRESHOLD1_SHIFT        0
#define BCHP_CLKGEN_INTR2_PCI_SET_AVS_INTR_THRESHOLD1_DEFAULT      0x00000000

/***************************************************************************
 *PCI_CLEAR - PCI interrupt Clear Register
 ***************************************************************************/
/* CLKGEN_INTR2 :: PCI_CLEAR :: reserved0 [31:05] */
#define BCHP_CLKGEN_INTR2_PCI_CLEAR_reserved0_MASK                 0xffffffe0
#define BCHP_CLKGEN_INTR2_PCI_CLEAR_reserved0_SHIFT                5

/* CLKGEN_INTR2 :: PCI_CLEAR :: GISB_INTR [04:04] */
#define BCHP_CLKGEN_INTR2_PCI_CLEAR_GISB_INTR_MASK                 0x00000010
#define BCHP_CLKGEN_INTR2_PCI_CLEAR_GISB_INTR_SHIFT                4
#define BCHP_CLKGEN_INTR2_PCI_CLEAR_GISB_INTR_DEFAULT              0x00000000

/* CLKGEN_INTR2 :: PCI_CLEAR :: AVS_INTR_SW_MEAS_DONE [03:03] */
#define BCHP_CLKGEN_INTR2_PCI_CLEAR_AVS_INTR_SW_MEAS_DONE_MASK     0x00000008
#define BCHP_CLKGEN_INTR2_PCI_CLEAR_AVS_INTR_SW_MEAS_DONE_SHIFT    3
#define BCHP_CLKGEN_INTR2_PCI_CLEAR_AVS_INTR_SW_MEAS_DONE_DEFAULT  0x00000000

/* CLKGEN_INTR2 :: PCI_CLEAR :: AVS_INTR_PWD [02:02] */
#define BCHP_CLKGEN_INTR2_PCI_CLEAR_AVS_INTR_PWD_MASK              0x00000004
#define BCHP_CLKGEN_INTR2_PCI_CLEAR_AVS_INTR_PWD_SHIFT             2
#define BCHP_CLKGEN_INTR2_PCI_CLEAR_AVS_INTR_PWD_DEFAULT           0x00000000

/* CLKGEN_INTR2 :: PCI_CLEAR :: AVS_INTR_THRESHOLD2 [01:01] */
#define BCHP_CLKGEN_INTR2_PCI_CLEAR_AVS_INTR_THRESHOLD2_MASK       0x00000002
#define BCHP_CLKGEN_INTR2_PCI_CLEAR_AVS_INTR_THRESHOLD2_SHIFT      1
#define BCHP_CLKGEN_INTR2_PCI_CLEAR_AVS_INTR_THRESHOLD2_DEFAULT    0x00000000

/* CLKGEN_INTR2 :: PCI_CLEAR :: AVS_INTR_THRESHOLD1 [00:00] */
#define BCHP_CLKGEN_INTR2_PCI_CLEAR_AVS_INTR_THRESHOLD1_MASK       0x00000001
#define BCHP_CLKGEN_INTR2_PCI_CLEAR_AVS_INTR_THRESHOLD1_SHIFT      0
#define BCHP_CLKGEN_INTR2_PCI_CLEAR_AVS_INTR_THRESHOLD1_DEFAULT    0x00000000

/***************************************************************************
 *PCI_MASK_STATUS - PCI interrupt Mask Status Register
 ***************************************************************************/
/* CLKGEN_INTR2 :: PCI_MASK_STATUS :: reserved0 [31:05] */
#define BCHP_CLKGEN_INTR2_PCI_MASK_STATUS_reserved0_MASK           0xffffffe0
#define BCHP_CLKGEN_INTR2_PCI_MASK_STATUS_reserved0_SHIFT          5

/* CLKGEN_INTR2 :: PCI_MASK_STATUS :: GISB_INTR [04:04] */
#define BCHP_CLKGEN_INTR2_PCI_MASK_STATUS_GISB_INTR_MASK           0x00000010
#define BCHP_CLKGEN_INTR2_PCI_MASK_STATUS_GISB_INTR_SHIFT          4
#define BCHP_CLKGEN_INTR2_PCI_MASK_STATUS_GISB_INTR_DEFAULT        0x00000001

/* CLKGEN_INTR2 :: PCI_MASK_STATUS :: AVS_INTR_SW_MEAS_DONE [03:03] */
#define BCHP_CLKGEN_INTR2_PCI_MASK_STATUS_AVS_INTR_SW_MEAS_DONE_MASK 0x00000008
#define BCHP_CLKGEN_INTR2_PCI_MASK_STATUS_AVS_INTR_SW_MEAS_DONE_SHIFT 3
#define BCHP_CLKGEN_INTR2_PCI_MASK_STATUS_AVS_INTR_SW_MEAS_DONE_DEFAULT 0x00000001

/* CLKGEN_INTR2 :: PCI_MASK_STATUS :: AVS_INTR_PWD [02:02] */
#define BCHP_CLKGEN_INTR2_PCI_MASK_STATUS_AVS_INTR_PWD_MASK        0x00000004
#define BCHP_CLKGEN_INTR2_PCI_MASK_STATUS_AVS_INTR_PWD_SHIFT       2
#define BCHP_CLKGEN_INTR2_PCI_MASK_STATUS_AVS_INTR_PWD_DEFAULT     0x00000001

/* CLKGEN_INTR2 :: PCI_MASK_STATUS :: AVS_INTR_THRESHOLD2 [01:01] */
#define BCHP_CLKGEN_INTR2_PCI_MASK_STATUS_AVS_INTR_THRESHOLD2_MASK 0x00000002
#define BCHP_CLKGEN_INTR2_PCI_MASK_STATUS_AVS_INTR_THRESHOLD2_SHIFT 1
#define BCHP_CLKGEN_INTR2_PCI_MASK_STATUS_AVS_INTR_THRESHOLD2_DEFAULT 0x00000001

/* CLKGEN_INTR2 :: PCI_MASK_STATUS :: AVS_INTR_THRESHOLD1 [00:00] */
#define BCHP_CLKGEN_INTR2_PCI_MASK_STATUS_AVS_INTR_THRESHOLD1_MASK 0x00000001
#define BCHP_CLKGEN_INTR2_PCI_MASK_STATUS_AVS_INTR_THRESHOLD1_SHIFT 0
#define BCHP_CLKGEN_INTR2_PCI_MASK_STATUS_AVS_INTR_THRESHOLD1_DEFAULT 0x00000001

/***************************************************************************
 *PCI_MASK_SET - PCI interrupt Mask Set Register
 ***************************************************************************/
/* CLKGEN_INTR2 :: PCI_MASK_SET :: reserved0 [31:05] */
#define BCHP_CLKGEN_INTR2_PCI_MASK_SET_reserved0_MASK              0xffffffe0
#define BCHP_CLKGEN_INTR2_PCI_MASK_SET_reserved0_SHIFT             5

/* CLKGEN_INTR2 :: PCI_MASK_SET :: GISB_INTR [04:04] */
#define BCHP_CLKGEN_INTR2_PCI_MASK_SET_GISB_INTR_MASK              0x00000010
#define BCHP_CLKGEN_INTR2_PCI_MASK_SET_GISB_INTR_SHIFT             4
#define BCHP_CLKGEN_INTR2_PCI_MASK_SET_GISB_INTR_DEFAULT           0x00000001

/* CLKGEN_INTR2 :: PCI_MASK_SET :: AVS_INTR_SW_MEAS_DONE [03:03] */
#define BCHP_CLKGEN_INTR2_PCI_MASK_SET_AVS_INTR_SW_MEAS_DONE_MASK  0x00000008
#define BCHP_CLKGEN_INTR2_PCI_MASK_SET_AVS_INTR_SW_MEAS_DONE_SHIFT 3
#define BCHP_CLKGEN_INTR2_PCI_MASK_SET_AVS_INTR_SW_MEAS_DONE_DEFAULT 0x00000001

/* CLKGEN_INTR2 :: PCI_MASK_SET :: AVS_INTR_PWD [02:02] */
#define BCHP_CLKGEN_INTR2_PCI_MASK_SET_AVS_INTR_PWD_MASK           0x00000004
#define BCHP_CLKGEN_INTR2_PCI_MASK_SET_AVS_INTR_PWD_SHIFT          2
#define BCHP_CLKGEN_INTR2_PCI_MASK_SET_AVS_INTR_PWD_DEFAULT        0x00000001

/* CLKGEN_INTR2 :: PCI_MASK_SET :: AVS_INTR_THRESHOLD2 [01:01] */
#define BCHP_CLKGEN_INTR2_PCI_MASK_SET_AVS_INTR_THRESHOLD2_MASK    0x00000002
#define BCHP_CLKGEN_INTR2_PCI_MASK_SET_AVS_INTR_THRESHOLD2_SHIFT   1
#define BCHP_CLKGEN_INTR2_PCI_MASK_SET_AVS_INTR_THRESHOLD2_DEFAULT 0x00000001

/* CLKGEN_INTR2 :: PCI_MASK_SET :: AVS_INTR_THRESHOLD1 [00:00] */
#define BCHP_CLKGEN_INTR2_PCI_MASK_SET_AVS_INTR_THRESHOLD1_MASK    0x00000001
#define BCHP_CLKGEN_INTR2_PCI_MASK_SET_AVS_INTR_THRESHOLD1_SHIFT   0
#define BCHP_CLKGEN_INTR2_PCI_MASK_SET_AVS_INTR_THRESHOLD1_DEFAULT 0x00000001

/***************************************************************************
 *PCI_MASK_CLEAR - PCI interrupt Mask Clear Register
 ***************************************************************************/
/* CLKGEN_INTR2 :: PCI_MASK_CLEAR :: reserved0 [31:05] */
#define BCHP_CLKGEN_INTR2_PCI_MASK_CLEAR_reserved0_MASK            0xffffffe0
#define BCHP_CLKGEN_INTR2_PCI_MASK_CLEAR_reserved0_SHIFT           5

/* CLKGEN_INTR2 :: PCI_MASK_CLEAR :: GISB_INTR [04:04] */
#define BCHP_CLKGEN_INTR2_PCI_MASK_CLEAR_GISB_INTR_MASK            0x00000010
#define BCHP_CLKGEN_INTR2_PCI_MASK_CLEAR_GISB_INTR_SHIFT           4
#define BCHP_CLKGEN_INTR2_PCI_MASK_CLEAR_GISB_INTR_DEFAULT         0x00000001

/* CLKGEN_INTR2 :: PCI_MASK_CLEAR :: AVS_INTR_SW_MEAS_DONE [03:03] */
#define BCHP_CLKGEN_INTR2_PCI_MASK_CLEAR_AVS_INTR_SW_MEAS_DONE_MASK 0x00000008
#define BCHP_CLKGEN_INTR2_PCI_MASK_CLEAR_AVS_INTR_SW_MEAS_DONE_SHIFT 3
#define BCHP_CLKGEN_INTR2_PCI_MASK_CLEAR_AVS_INTR_SW_MEAS_DONE_DEFAULT 0x00000001

/* CLKGEN_INTR2 :: PCI_MASK_CLEAR :: AVS_INTR_PWD [02:02] */
#define BCHP_CLKGEN_INTR2_PCI_MASK_CLEAR_AVS_INTR_PWD_MASK         0x00000004
#define BCHP_CLKGEN_INTR2_PCI_MASK_CLEAR_AVS_INTR_PWD_SHIFT        2
#define BCHP_CLKGEN_INTR2_PCI_MASK_CLEAR_AVS_INTR_PWD_DEFAULT      0x00000001

/* CLKGEN_INTR2 :: PCI_MASK_CLEAR :: AVS_INTR_THRESHOLD2 [01:01] */
#define BCHP_CLKGEN_INTR2_PCI_MASK_CLEAR_AVS_INTR_THRESHOLD2_MASK  0x00000002
#define BCHP_CLKGEN_INTR2_PCI_MASK_CLEAR_AVS_INTR_THRESHOLD2_SHIFT 1
#define BCHP_CLKGEN_INTR2_PCI_MASK_CLEAR_AVS_INTR_THRESHOLD2_DEFAULT 0x00000001

/* CLKGEN_INTR2 :: PCI_MASK_CLEAR :: AVS_INTR_THRESHOLD1 [00:00] */
#define BCHP_CLKGEN_INTR2_PCI_MASK_CLEAR_AVS_INTR_THRESHOLD1_MASK  0x00000001
#define BCHP_CLKGEN_INTR2_PCI_MASK_CLEAR_AVS_INTR_THRESHOLD1_SHIFT 0
#define BCHP_CLKGEN_INTR2_PCI_MASK_CLEAR_AVS_INTR_THRESHOLD1_DEFAULT 0x00000001

#endif /* #ifndef BCHP_CLKGEN_INTR2_H__ */

/* End of File */