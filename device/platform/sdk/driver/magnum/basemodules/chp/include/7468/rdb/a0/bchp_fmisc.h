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
 * $brcm_Workfile: bchp_fmisc.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 9/14/09 4:22p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Fri Sep 11 19:45:13 2009
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
 * $brcm_Log: /magnum/basemodules/chp/7468/rdb/a0/bchp_fmisc.h $
 * 
 * Hydra_Software_Devel/1   9/14/09 4:22p albertl
 * SW7468-3: Initial revision.
 *
 ***************************************************************************/

#ifndef BCHP_FMISC_H__
#define BCHP_FMISC_H__

/***************************************************************************
 *FMISC - BVN Front Control Registers
 ***************************************************************************/
#define BCHP_FMISC_SOFT_RESET                    0x00404000 /* BVN Front Soft Reset */
#define BCHP_FMISC_TEST_PORT_SEL                 0x00404004 /* BVN Front Test Port Select */
#define BCHP_FMISC_TEST_PORT_DATA                0x00404008 /* BVN Front Test Port Status */
#define BCHP_FMISC_BVNF_CLOCK_CTRL               0x00404018 /* BVN Front clock control register */
#define BCHP_FMISC_SCRATCH_0                     0x00404020 /* Scratch Register */

/***************************************************************************
 *SOFT_RESET - BVN Front Soft Reset
 ***************************************************************************/
/* FMISC :: SOFT_RESET :: reserved0 [31:25] */
#define BCHP_FMISC_SOFT_RESET_reserved0_MASK                       0xfe000000
#define BCHP_FMISC_SOFT_RESET_reserved0_SHIFT                      25

/* FMISC :: SOFT_RESET :: RDC [24:24] */
#define BCHP_FMISC_SOFT_RESET_RDC_MASK                             0x01000000
#define BCHP_FMISC_SOFT_RESET_RDC_SHIFT                            24

/* FMISC :: SOFT_RESET :: reserved1 [23:06] */
#define BCHP_FMISC_SOFT_RESET_reserved1_MASK                       0x00ffffc0
#define BCHP_FMISC_SOFT_RESET_reserved1_SHIFT                      6

/* FMISC :: SOFT_RESET :: VFD_1 [05:05] */
#define BCHP_FMISC_SOFT_RESET_VFD_1_MASK                           0x00000020
#define BCHP_FMISC_SOFT_RESET_VFD_1_SHIFT                          5

/* FMISC :: SOFT_RESET :: VFD_0 [04:04] */
#define BCHP_FMISC_SOFT_RESET_VFD_0_MASK                           0x00000010
#define BCHP_FMISC_SOFT_RESET_VFD_0_SHIFT                          4

/* FMISC :: SOFT_RESET :: reserved2 [03:01] */
#define BCHP_FMISC_SOFT_RESET_reserved2_MASK                       0x0000000e
#define BCHP_FMISC_SOFT_RESET_reserved2_SHIFT                      1

/* FMISC :: SOFT_RESET :: MFD_0 [00:00] */
#define BCHP_FMISC_SOFT_RESET_MFD_0_MASK                           0x00000001
#define BCHP_FMISC_SOFT_RESET_MFD_0_SHIFT                          0

/***************************************************************************
 *TEST_PORT_SEL - BVN Front Test Port Select
 ***************************************************************************/
/* FMISC :: TEST_PORT_SEL :: reserved0 [31:01] */
#define BCHP_FMISC_TEST_PORT_SEL_reserved0_MASK                    0xfffffffe
#define BCHP_FMISC_TEST_PORT_SEL_reserved0_SHIFT                   1

/* FMISC :: TEST_PORT_SEL :: TEST_PORT_SEL [00:00] */
#define BCHP_FMISC_TEST_PORT_SEL_TEST_PORT_SEL_MASK                0x00000001
#define BCHP_FMISC_TEST_PORT_SEL_TEST_PORT_SEL_SHIFT               0
#define BCHP_FMISC_TEST_PORT_SEL_TEST_PORT_SEL_TP_OUT_1            1
#define BCHP_FMISC_TEST_PORT_SEL_TEST_PORT_SEL_TP_OUT_0            0

/***************************************************************************
 *TEST_PORT_DATA - BVN Front Test Port Status
 ***************************************************************************/
/* FMISC :: TEST_PORT_DATA :: TEST_PORT_DATA [31:00] */
#define BCHP_FMISC_TEST_PORT_DATA_TEST_PORT_DATA_MASK              0xffffffff
#define BCHP_FMISC_TEST_PORT_DATA_TEST_PORT_DATA_SHIFT             0

/***************************************************************************
 *BVNF_CLOCK_CTRL - BVN Front clock control register
 ***************************************************************************/
/* FMISC :: BVNF_CLOCK_CTRL :: reserved0 [31:01] */
#define BCHP_FMISC_BVNF_CLOCK_CTRL_reserved0_MASK                  0xfffffffe
#define BCHP_FMISC_BVNF_CLOCK_CTRL_reserved0_SHIFT                 1

/* FMISC :: BVNF_CLOCK_CTRL :: CLK_FREE_RUN_MODE [00:00] */
#define BCHP_FMISC_BVNF_CLOCK_CTRL_CLK_FREE_RUN_MODE_MASK          0x00000001
#define BCHP_FMISC_BVNF_CLOCK_CTRL_CLK_FREE_RUN_MODE_SHIFT         0

/***************************************************************************
 *SCRATCH_0 - Scratch Register
 ***************************************************************************/
/* FMISC :: SCRATCH_0 :: VALUE [31:00] */
#define BCHP_FMISC_SCRATCH_0_VALUE_MASK                            0xffffffff
#define BCHP_FMISC_SCRATCH_0_VALUE_SHIFT                           0

#endif /* #ifndef BCHP_FMISC_H__ */

/* End of File */