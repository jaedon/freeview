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
 * $brcm_Workfile: bchp_aud_fmm_src_esr0.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 9/14/09 3:57p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Fri Sep 11 19:43:05 2009
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
 * $brcm_Log: /magnum/basemodules/chp/7468/rdb/a0/bchp_aud_fmm_src_esr0.h $
 * 
 * Hydra_Software_Devel/1   9/14/09 3:57p albertl
 * SW7468-3: Initial revision.
 *
 ***************************************************************************/

#ifndef BCHP_AUD_FMM_SRC_ESR0_H__
#define BCHP_AUD_FMM_SRC_ESR0_H__

/***************************************************************************
 *AUD_FMM_SRC_ESR0 - FMM-SRC Error Status 0
 ***************************************************************************/
#define BCHP_AUD_FMM_SRC_ESR0_STATUS             0x00393000 /* Error Status Register */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_SET         0x00393004 /* Error Set Register */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_CLEAR       0x00393008 /* Error Clear Register */
#define BCHP_AUD_FMM_SRC_ESR0_MASK               0x0039300c /* Mask Status Register */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_SET           0x00393010 /* Mask Set Register */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_CLEAR         0x00393014 /* Mask Clear Register */

/***************************************************************************
 *STATUS - Error Status Register
 ***************************************************************************/
/* AUD_FMM_SRC_ESR0 :: STATUS :: reserved0 [31:12] */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_reserved0_MASK                0xfffff000
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_reserved0_SHIFT               12

/* AUD_FMM_SRC_ESR0 :: STATUS :: UNDERFLOW11 [11:11] */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_UNDERFLOW11_MASK              0x00000800
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_UNDERFLOW11_SHIFT             11

/* AUD_FMM_SRC_ESR0 :: STATUS :: UNDERFLOW10 [10:10] */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_UNDERFLOW10_MASK              0x00000400
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_UNDERFLOW10_SHIFT             10

/* AUD_FMM_SRC_ESR0 :: STATUS :: UNDERFLOW9 [09:09] */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_UNDERFLOW9_MASK               0x00000200
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_UNDERFLOW9_SHIFT              9

/* AUD_FMM_SRC_ESR0 :: STATUS :: UNDERFLOW8 [08:08] */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_UNDERFLOW8_MASK               0x00000100
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_UNDERFLOW8_SHIFT              8

/* AUD_FMM_SRC_ESR0 :: STATUS :: UNDERFLOW7 [07:07] */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_UNDERFLOW7_MASK               0x00000080
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_UNDERFLOW7_SHIFT              7

/* AUD_FMM_SRC_ESR0 :: STATUS :: UNDERFLOW6 [06:06] */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_UNDERFLOW6_MASK               0x00000040
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_UNDERFLOW6_SHIFT              6

/* AUD_FMM_SRC_ESR0 :: STATUS :: UNDERFLOW5 [05:05] */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_UNDERFLOW5_MASK               0x00000020
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_UNDERFLOW5_SHIFT              5

/* AUD_FMM_SRC_ESR0 :: STATUS :: UNDERFLOW4 [04:04] */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_UNDERFLOW4_MASK               0x00000010
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_UNDERFLOW4_SHIFT              4

/* AUD_FMM_SRC_ESR0 :: STATUS :: UNDERFLOW3 [03:03] */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_UNDERFLOW3_MASK               0x00000008
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_UNDERFLOW3_SHIFT              3

/* AUD_FMM_SRC_ESR0 :: STATUS :: UNDERFLOW2 [02:02] */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_UNDERFLOW2_MASK               0x00000004
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_UNDERFLOW2_SHIFT              2

/* AUD_FMM_SRC_ESR0 :: STATUS :: UNDERFLOW1 [01:01] */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_UNDERFLOW1_MASK               0x00000002
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_UNDERFLOW1_SHIFT              1

/* AUD_FMM_SRC_ESR0 :: STATUS :: UNDERFLOW0 [00:00] */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_UNDERFLOW0_MASK               0x00000001
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_UNDERFLOW0_SHIFT              0

/***************************************************************************
 *STATUS_SET - Error Set Register
 ***************************************************************************/
/* AUD_FMM_SRC_ESR0 :: STATUS_SET :: reserved0 [31:12] */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_SET_reserved0_MASK            0xfffff000
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_SET_reserved0_SHIFT           12

/* AUD_FMM_SRC_ESR0 :: STATUS_SET :: UNDERFLOW11 [11:11] */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_SET_UNDERFLOW11_MASK          0x00000800
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_SET_UNDERFLOW11_SHIFT         11

/* AUD_FMM_SRC_ESR0 :: STATUS_SET :: UNDERFLOW10 [10:10] */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_SET_UNDERFLOW10_MASK          0x00000400
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_SET_UNDERFLOW10_SHIFT         10

/* AUD_FMM_SRC_ESR0 :: STATUS_SET :: UNDERFLOW9 [09:09] */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_SET_UNDERFLOW9_MASK           0x00000200
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_SET_UNDERFLOW9_SHIFT          9

/* AUD_FMM_SRC_ESR0 :: STATUS_SET :: UNDERFLOW8 [08:08] */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_SET_UNDERFLOW8_MASK           0x00000100
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_SET_UNDERFLOW8_SHIFT          8

/* AUD_FMM_SRC_ESR0 :: STATUS_SET :: UNDERFLOW7 [07:07] */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_SET_UNDERFLOW7_MASK           0x00000080
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_SET_UNDERFLOW7_SHIFT          7

/* AUD_FMM_SRC_ESR0 :: STATUS_SET :: UNDERFLOW6 [06:06] */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_SET_UNDERFLOW6_MASK           0x00000040
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_SET_UNDERFLOW6_SHIFT          6

/* AUD_FMM_SRC_ESR0 :: STATUS_SET :: UNDERFLOW5 [05:05] */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_SET_UNDERFLOW5_MASK           0x00000020
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_SET_UNDERFLOW5_SHIFT          5

/* AUD_FMM_SRC_ESR0 :: STATUS_SET :: UNDERFLOW4 [04:04] */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_SET_UNDERFLOW4_MASK           0x00000010
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_SET_UNDERFLOW4_SHIFT          4

/* AUD_FMM_SRC_ESR0 :: STATUS_SET :: UNDERFLOW3 [03:03] */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_SET_UNDERFLOW3_MASK           0x00000008
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_SET_UNDERFLOW3_SHIFT          3

/* AUD_FMM_SRC_ESR0 :: STATUS_SET :: UNDERFLOW2 [02:02] */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_SET_UNDERFLOW2_MASK           0x00000004
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_SET_UNDERFLOW2_SHIFT          2

/* AUD_FMM_SRC_ESR0 :: STATUS_SET :: UNDERFLOW1 [01:01] */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_SET_UNDERFLOW1_MASK           0x00000002
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_SET_UNDERFLOW1_SHIFT          1

/* AUD_FMM_SRC_ESR0 :: STATUS_SET :: UNDERFLOW0 [00:00] */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_SET_UNDERFLOW0_MASK           0x00000001
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_SET_UNDERFLOW0_SHIFT          0

/***************************************************************************
 *STATUS_CLEAR - Error Clear Register
 ***************************************************************************/
/* AUD_FMM_SRC_ESR0 :: STATUS_CLEAR :: reserved0 [31:12] */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_CLEAR_reserved0_MASK          0xfffff000
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_CLEAR_reserved0_SHIFT         12

/* AUD_FMM_SRC_ESR0 :: STATUS_CLEAR :: UNDERFLOW11 [11:11] */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_CLEAR_UNDERFLOW11_MASK        0x00000800
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_CLEAR_UNDERFLOW11_SHIFT       11

/* AUD_FMM_SRC_ESR0 :: STATUS_CLEAR :: UNDERFLOW10 [10:10] */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_CLEAR_UNDERFLOW10_MASK        0x00000400
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_CLEAR_UNDERFLOW10_SHIFT       10

/* AUD_FMM_SRC_ESR0 :: STATUS_CLEAR :: UNDERFLOW9 [09:09] */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_CLEAR_UNDERFLOW9_MASK         0x00000200
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_CLEAR_UNDERFLOW9_SHIFT        9

/* AUD_FMM_SRC_ESR0 :: STATUS_CLEAR :: UNDERFLOW8 [08:08] */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_CLEAR_UNDERFLOW8_MASK         0x00000100
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_CLEAR_UNDERFLOW8_SHIFT        8

/* AUD_FMM_SRC_ESR0 :: STATUS_CLEAR :: UNDERFLOW7 [07:07] */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_CLEAR_UNDERFLOW7_MASK         0x00000080
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_CLEAR_UNDERFLOW7_SHIFT        7

/* AUD_FMM_SRC_ESR0 :: STATUS_CLEAR :: UNDERFLOW6 [06:06] */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_CLEAR_UNDERFLOW6_MASK         0x00000040
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_CLEAR_UNDERFLOW6_SHIFT        6

/* AUD_FMM_SRC_ESR0 :: STATUS_CLEAR :: UNDERFLOW5 [05:05] */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_CLEAR_UNDERFLOW5_MASK         0x00000020
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_CLEAR_UNDERFLOW5_SHIFT        5

/* AUD_FMM_SRC_ESR0 :: STATUS_CLEAR :: UNDERFLOW4 [04:04] */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_CLEAR_UNDERFLOW4_MASK         0x00000010
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_CLEAR_UNDERFLOW4_SHIFT        4

/* AUD_FMM_SRC_ESR0 :: STATUS_CLEAR :: UNDERFLOW3 [03:03] */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_CLEAR_UNDERFLOW3_MASK         0x00000008
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_CLEAR_UNDERFLOW3_SHIFT        3

/* AUD_FMM_SRC_ESR0 :: STATUS_CLEAR :: UNDERFLOW2 [02:02] */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_CLEAR_UNDERFLOW2_MASK         0x00000004
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_CLEAR_UNDERFLOW2_SHIFT        2

/* AUD_FMM_SRC_ESR0 :: STATUS_CLEAR :: UNDERFLOW1 [01:01] */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_CLEAR_UNDERFLOW1_MASK         0x00000002
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_CLEAR_UNDERFLOW1_SHIFT        1

/* AUD_FMM_SRC_ESR0 :: STATUS_CLEAR :: UNDERFLOW0 [00:00] */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_CLEAR_UNDERFLOW0_MASK         0x00000001
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_CLEAR_UNDERFLOW0_SHIFT        0

/***************************************************************************
 *MASK - Mask Status Register
 ***************************************************************************/
/* AUD_FMM_SRC_ESR0 :: MASK :: reserved0 [31:12] */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_reserved0_MASK                  0xfffff000
#define BCHP_AUD_FMM_SRC_ESR0_MASK_reserved0_SHIFT                 12

/* AUD_FMM_SRC_ESR0 :: MASK :: UNDERFLOW11 [11:11] */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_UNDERFLOW11_MASK                0x00000800
#define BCHP_AUD_FMM_SRC_ESR0_MASK_UNDERFLOW11_SHIFT               11

/* AUD_FMM_SRC_ESR0 :: MASK :: UNDERFLOW10 [10:10] */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_UNDERFLOW10_MASK                0x00000400
#define BCHP_AUD_FMM_SRC_ESR0_MASK_UNDERFLOW10_SHIFT               10

/* AUD_FMM_SRC_ESR0 :: MASK :: UNDERFLOW9 [09:09] */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_UNDERFLOW9_MASK                 0x00000200
#define BCHP_AUD_FMM_SRC_ESR0_MASK_UNDERFLOW9_SHIFT                9

/* AUD_FMM_SRC_ESR0 :: MASK :: UNDERFLOW8 [08:08] */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_UNDERFLOW8_MASK                 0x00000100
#define BCHP_AUD_FMM_SRC_ESR0_MASK_UNDERFLOW8_SHIFT                8

/* AUD_FMM_SRC_ESR0 :: MASK :: UNDERFLOW7 [07:07] */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_UNDERFLOW7_MASK                 0x00000080
#define BCHP_AUD_FMM_SRC_ESR0_MASK_UNDERFLOW7_SHIFT                7

/* AUD_FMM_SRC_ESR0 :: MASK :: UNDERFLOW6 [06:06] */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_UNDERFLOW6_MASK                 0x00000040
#define BCHP_AUD_FMM_SRC_ESR0_MASK_UNDERFLOW6_SHIFT                6

/* AUD_FMM_SRC_ESR0 :: MASK :: UNDERFLOW5 [05:05] */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_UNDERFLOW5_MASK                 0x00000020
#define BCHP_AUD_FMM_SRC_ESR0_MASK_UNDERFLOW5_SHIFT                5

/* AUD_FMM_SRC_ESR0 :: MASK :: UNDERFLOW4 [04:04] */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_UNDERFLOW4_MASK                 0x00000010
#define BCHP_AUD_FMM_SRC_ESR0_MASK_UNDERFLOW4_SHIFT                4

/* AUD_FMM_SRC_ESR0 :: MASK :: UNDERFLOW3 [03:03] */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_UNDERFLOW3_MASK                 0x00000008
#define BCHP_AUD_FMM_SRC_ESR0_MASK_UNDERFLOW3_SHIFT                3

/* AUD_FMM_SRC_ESR0 :: MASK :: UNDERFLOW2 [02:02] */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_UNDERFLOW2_MASK                 0x00000004
#define BCHP_AUD_FMM_SRC_ESR0_MASK_UNDERFLOW2_SHIFT                2

/* AUD_FMM_SRC_ESR0 :: MASK :: UNDERFLOW1 [01:01] */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_UNDERFLOW1_MASK                 0x00000002
#define BCHP_AUD_FMM_SRC_ESR0_MASK_UNDERFLOW1_SHIFT                1

/* AUD_FMM_SRC_ESR0 :: MASK :: UNDERFLOW0 [00:00] */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_UNDERFLOW0_MASK                 0x00000001
#define BCHP_AUD_FMM_SRC_ESR0_MASK_UNDERFLOW0_SHIFT                0

/***************************************************************************
 *MASK_SET - Mask Set Register
 ***************************************************************************/
/* AUD_FMM_SRC_ESR0 :: MASK_SET :: reserved0 [31:12] */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_SET_reserved0_MASK              0xfffff000
#define BCHP_AUD_FMM_SRC_ESR0_MASK_SET_reserved0_SHIFT             12

/* AUD_FMM_SRC_ESR0 :: MASK_SET :: UNDERFLOW11 [11:11] */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_SET_UNDERFLOW11_MASK            0x00000800
#define BCHP_AUD_FMM_SRC_ESR0_MASK_SET_UNDERFLOW11_SHIFT           11

/* AUD_FMM_SRC_ESR0 :: MASK_SET :: UNDERFLOW10 [10:10] */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_SET_UNDERFLOW10_MASK            0x00000400
#define BCHP_AUD_FMM_SRC_ESR0_MASK_SET_UNDERFLOW10_SHIFT           10

/* AUD_FMM_SRC_ESR0 :: MASK_SET :: UNDERFLOW9 [09:09] */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_SET_UNDERFLOW9_MASK             0x00000200
#define BCHP_AUD_FMM_SRC_ESR0_MASK_SET_UNDERFLOW9_SHIFT            9

/* AUD_FMM_SRC_ESR0 :: MASK_SET :: UNDERFLOW8 [08:08] */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_SET_UNDERFLOW8_MASK             0x00000100
#define BCHP_AUD_FMM_SRC_ESR0_MASK_SET_UNDERFLOW8_SHIFT            8

/* AUD_FMM_SRC_ESR0 :: MASK_SET :: UNDERFLOW7 [07:07] */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_SET_UNDERFLOW7_MASK             0x00000080
#define BCHP_AUD_FMM_SRC_ESR0_MASK_SET_UNDERFLOW7_SHIFT            7

/* AUD_FMM_SRC_ESR0 :: MASK_SET :: UNDERFLOW6 [06:06] */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_SET_UNDERFLOW6_MASK             0x00000040
#define BCHP_AUD_FMM_SRC_ESR0_MASK_SET_UNDERFLOW6_SHIFT            6

/* AUD_FMM_SRC_ESR0 :: MASK_SET :: UNDERFLOW5 [05:05] */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_SET_UNDERFLOW5_MASK             0x00000020
#define BCHP_AUD_FMM_SRC_ESR0_MASK_SET_UNDERFLOW5_SHIFT            5

/* AUD_FMM_SRC_ESR0 :: MASK_SET :: UNDERFLOW4 [04:04] */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_SET_UNDERFLOW4_MASK             0x00000010
#define BCHP_AUD_FMM_SRC_ESR0_MASK_SET_UNDERFLOW4_SHIFT            4

/* AUD_FMM_SRC_ESR0 :: MASK_SET :: UNDERFLOW3 [03:03] */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_SET_UNDERFLOW3_MASK             0x00000008
#define BCHP_AUD_FMM_SRC_ESR0_MASK_SET_UNDERFLOW3_SHIFT            3

/* AUD_FMM_SRC_ESR0 :: MASK_SET :: UNDERFLOW2 [02:02] */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_SET_UNDERFLOW2_MASK             0x00000004
#define BCHP_AUD_FMM_SRC_ESR0_MASK_SET_UNDERFLOW2_SHIFT            2

/* AUD_FMM_SRC_ESR0 :: MASK_SET :: UNDERFLOW1 [01:01] */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_SET_UNDERFLOW1_MASK             0x00000002
#define BCHP_AUD_FMM_SRC_ESR0_MASK_SET_UNDERFLOW1_SHIFT            1

/* AUD_FMM_SRC_ESR0 :: MASK_SET :: UNDERFLOW0 [00:00] */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_SET_UNDERFLOW0_MASK             0x00000001
#define BCHP_AUD_FMM_SRC_ESR0_MASK_SET_UNDERFLOW0_SHIFT            0

/***************************************************************************
 *MASK_CLEAR - Mask Clear Register
 ***************************************************************************/
/* AUD_FMM_SRC_ESR0 :: MASK_CLEAR :: reserved0 [31:12] */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_CLEAR_reserved0_MASK            0xfffff000
#define BCHP_AUD_FMM_SRC_ESR0_MASK_CLEAR_reserved0_SHIFT           12

/* AUD_FMM_SRC_ESR0 :: MASK_CLEAR :: UNDERFLOW11 [11:11] */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_CLEAR_UNDERFLOW11_MASK          0x00000800
#define BCHP_AUD_FMM_SRC_ESR0_MASK_CLEAR_UNDERFLOW11_SHIFT         11

/* AUD_FMM_SRC_ESR0 :: MASK_CLEAR :: UNDERFLOW10 [10:10] */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_CLEAR_UNDERFLOW10_MASK          0x00000400
#define BCHP_AUD_FMM_SRC_ESR0_MASK_CLEAR_UNDERFLOW10_SHIFT         10

/* AUD_FMM_SRC_ESR0 :: MASK_CLEAR :: UNDERFLOW9 [09:09] */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_CLEAR_UNDERFLOW9_MASK           0x00000200
#define BCHP_AUD_FMM_SRC_ESR0_MASK_CLEAR_UNDERFLOW9_SHIFT          9

/* AUD_FMM_SRC_ESR0 :: MASK_CLEAR :: UNDERFLOW8 [08:08] */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_CLEAR_UNDERFLOW8_MASK           0x00000100
#define BCHP_AUD_FMM_SRC_ESR0_MASK_CLEAR_UNDERFLOW8_SHIFT          8

/* AUD_FMM_SRC_ESR0 :: MASK_CLEAR :: UNDERFLOW7 [07:07] */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_CLEAR_UNDERFLOW7_MASK           0x00000080
#define BCHP_AUD_FMM_SRC_ESR0_MASK_CLEAR_UNDERFLOW7_SHIFT          7

/* AUD_FMM_SRC_ESR0 :: MASK_CLEAR :: UNDERFLOW6 [06:06] */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_CLEAR_UNDERFLOW6_MASK           0x00000040
#define BCHP_AUD_FMM_SRC_ESR0_MASK_CLEAR_UNDERFLOW6_SHIFT          6

/* AUD_FMM_SRC_ESR0 :: MASK_CLEAR :: UNDERFLOW5 [05:05] */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_CLEAR_UNDERFLOW5_MASK           0x00000020
#define BCHP_AUD_FMM_SRC_ESR0_MASK_CLEAR_UNDERFLOW5_SHIFT          5

/* AUD_FMM_SRC_ESR0 :: MASK_CLEAR :: UNDERFLOW4 [04:04] */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_CLEAR_UNDERFLOW4_MASK           0x00000010
#define BCHP_AUD_FMM_SRC_ESR0_MASK_CLEAR_UNDERFLOW4_SHIFT          4

/* AUD_FMM_SRC_ESR0 :: MASK_CLEAR :: UNDERFLOW3 [03:03] */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_CLEAR_UNDERFLOW3_MASK           0x00000008
#define BCHP_AUD_FMM_SRC_ESR0_MASK_CLEAR_UNDERFLOW3_SHIFT          3

/* AUD_FMM_SRC_ESR0 :: MASK_CLEAR :: UNDERFLOW2 [02:02] */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_CLEAR_UNDERFLOW2_MASK           0x00000004
#define BCHP_AUD_FMM_SRC_ESR0_MASK_CLEAR_UNDERFLOW2_SHIFT          2

/* AUD_FMM_SRC_ESR0 :: MASK_CLEAR :: UNDERFLOW1 [01:01] */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_CLEAR_UNDERFLOW1_MASK           0x00000002
#define BCHP_AUD_FMM_SRC_ESR0_MASK_CLEAR_UNDERFLOW1_SHIFT          1

/* AUD_FMM_SRC_ESR0 :: MASK_CLEAR :: UNDERFLOW0 [00:00] */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_CLEAR_UNDERFLOW0_MASK           0x00000001
#define BCHP_AUD_FMM_SRC_ESR0_MASK_CLEAR_UNDERFLOW0_SHIFT          0

#endif /* #ifndef BCHP_AUD_FMM_SRC_ESR0_H__ */

/* End of File */