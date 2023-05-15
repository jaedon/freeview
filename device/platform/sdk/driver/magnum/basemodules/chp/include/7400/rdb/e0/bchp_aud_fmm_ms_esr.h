/***************************************************************************
 *     Copyright (c) 1999-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bchp_aud_fmm_ms_esr.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 1/9/08 9:53a $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Wed Jan  9 09:02:15 2008
 *                 MD5 Checksum         847dc12a9d71c4c68a648bbf19a883e3
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7400/rdb/e0/bchp_aud_fmm_ms_esr.h $
 * 
 * Hydra_Software_Devel/1   1/9/08 9:53a rpan
 * PR38572: Initial revision.
 *
 ***************************************************************************/

#ifndef BCHP_AUD_FMM_MS_ESR_H__
#define BCHP_AUD_FMM_MS_ESR_H__

/***************************************************************************
 *AUD_FMM_MS_ESR - FMM-MS Error Status
 ***************************************************************************/
#define BCHP_AUD_FMM_MS_ESR_STATUS               0x0063a000 /* Error Status Register */
#define BCHP_AUD_FMM_MS_ESR_STATUS_SET           0x0063a004 /* Error Set Register */
#define BCHP_AUD_FMM_MS_ESR_STATUS_CLEAR         0x0063a008 /* Error Clear Register */
#define BCHP_AUD_FMM_MS_ESR_MASK                 0x0063a00c /* Mask Status Register */
#define BCHP_AUD_FMM_MS_ESR_MASK_SET             0x0063a010 /* Mask Set Register */
#define BCHP_AUD_FMM_MS_ESR_MASK_CLEAR           0x0063a014 /* Mask Clear Register */

/***************************************************************************
 *STATUS - Error Status Register
 ***************************************************************************/
/* AUD_FMM_MS_ESR :: STATUS :: reserved0 [31:15] */
#define BCHP_AUD_FMM_MS_ESR_STATUS_reserved0_MASK                  0xffff8000
#define BCHP_AUD_FMM_MS_ESR_STATUS_reserved0_SHIFT                 15

/* AUD_FMM_MS_ESR :: STATUS :: SNOOP1_OFLOW [14:14] */
#define BCHP_AUD_FMM_MS_ESR_STATUS_SNOOP1_OFLOW_MASK               0x00004000
#define BCHP_AUD_FMM_MS_ESR_STATUS_SNOOP1_OFLOW_SHIFT              14

/* AUD_FMM_MS_ESR :: STATUS :: SNOOP1_UFLOW [13:13] */
#define BCHP_AUD_FMM_MS_ESR_STATUS_SNOOP1_UFLOW_MASK               0x00002000
#define BCHP_AUD_FMM_MS_ESR_STATUS_SNOOP1_UFLOW_SHIFT              13

/* AUD_FMM_MS_ESR :: STATUS :: SNOOP0_OFLOW [12:12] */
#define BCHP_AUD_FMM_MS_ESR_STATUS_SNOOP0_OFLOW_MASK               0x00001000
#define BCHP_AUD_FMM_MS_ESR_STATUS_SNOOP0_OFLOW_SHIFT              12

/* AUD_FMM_MS_ESR :: STATUS :: SNOOP0_UFLOW [11:11] */
#define BCHP_AUD_FMM_MS_ESR_STATUS_SNOOP0_UFLOW_MASK               0x00000800
#define BCHP_AUD_FMM_MS_ESR_STATUS_SNOOP0_UFLOW_SHIFT              11

/* AUD_FMM_MS_ESR :: STATUS :: USEQ_SLEEP [10:10] */
#define BCHP_AUD_FMM_MS_ESR_STATUS_USEQ_SLEEP_MASK                 0x00000400
#define BCHP_AUD_FMM_MS_ESR_STATUS_USEQ_SLEEP_SHIFT                10

/* AUD_FMM_MS_ESR :: STATUS :: USEQ_ERROR1 [09:09] */
#define BCHP_AUD_FMM_MS_ESR_STATUS_USEQ_ERROR1_MASK                0x00000200
#define BCHP_AUD_FMM_MS_ESR_STATUS_USEQ_ERROR1_SHIFT               9

/* AUD_FMM_MS_ESR :: STATUS :: CBIT_PING_PONG1 [08:08] */
#define BCHP_AUD_FMM_MS_ESR_STATUS_CBIT_PING_PONG1_MASK            0x00000100
#define BCHP_AUD_FMM_MS_ESR_STATUS_CBIT_PING_PONG1_SHIFT           8

/* AUD_FMM_MS_ESR :: STATUS :: RAMP_DONE1 [07:07] */
#define BCHP_AUD_FMM_MS_ESR_STATUS_RAMP_DONE1_MASK                 0x00000080
#define BCHP_AUD_FMM_MS_ESR_STATUS_RAMP_DONE1_SHIFT                7

/* AUD_FMM_MS_ESR :: STATUS :: FLOWON1 [06:06] */
#define BCHP_AUD_FMM_MS_ESR_STATUS_FLOWON1_MASK                    0x00000040
#define BCHP_AUD_FMM_MS_ESR_STATUS_FLOWON1_SHIFT                   6

/* AUD_FMM_MS_ESR :: STATUS :: UNDERFLOW1 [05:05] */
#define BCHP_AUD_FMM_MS_ESR_STATUS_UNDERFLOW1_MASK                 0x00000020
#define BCHP_AUD_FMM_MS_ESR_STATUS_UNDERFLOW1_SHIFT                5

/* AUD_FMM_MS_ESR :: STATUS :: USEQ_ERROR0 [04:04] */
#define BCHP_AUD_FMM_MS_ESR_STATUS_USEQ_ERROR0_MASK                0x00000010
#define BCHP_AUD_FMM_MS_ESR_STATUS_USEQ_ERROR0_SHIFT               4

/* AUD_FMM_MS_ESR :: STATUS :: CBIT_PING_PONG0 [03:03] */
#define BCHP_AUD_FMM_MS_ESR_STATUS_CBIT_PING_PONG0_MASK            0x00000008
#define BCHP_AUD_FMM_MS_ESR_STATUS_CBIT_PING_PONG0_SHIFT           3

/* AUD_FMM_MS_ESR :: STATUS :: RAMP_DONE0 [02:02] */
#define BCHP_AUD_FMM_MS_ESR_STATUS_RAMP_DONE0_MASK                 0x00000004
#define BCHP_AUD_FMM_MS_ESR_STATUS_RAMP_DONE0_SHIFT                2

/* AUD_FMM_MS_ESR :: STATUS :: FLOWON0 [01:01] */
#define BCHP_AUD_FMM_MS_ESR_STATUS_FLOWON0_MASK                    0x00000002
#define BCHP_AUD_FMM_MS_ESR_STATUS_FLOWON0_SHIFT                   1

/* AUD_FMM_MS_ESR :: STATUS :: UNDERFLOW0 [00:00] */
#define BCHP_AUD_FMM_MS_ESR_STATUS_UNDERFLOW0_MASK                 0x00000001
#define BCHP_AUD_FMM_MS_ESR_STATUS_UNDERFLOW0_SHIFT                0

/***************************************************************************
 *STATUS_SET - Error Set Register
 ***************************************************************************/
/* AUD_FMM_MS_ESR :: STATUS_SET :: reserved0 [31:15] */
#define BCHP_AUD_FMM_MS_ESR_STATUS_SET_reserved0_MASK              0xffff8000
#define BCHP_AUD_FMM_MS_ESR_STATUS_SET_reserved0_SHIFT             15

/* AUD_FMM_MS_ESR :: STATUS_SET :: SNOOP1_OFLOW [14:14] */
#define BCHP_AUD_FMM_MS_ESR_STATUS_SET_SNOOP1_OFLOW_MASK           0x00004000
#define BCHP_AUD_FMM_MS_ESR_STATUS_SET_SNOOP1_OFLOW_SHIFT          14

/* AUD_FMM_MS_ESR :: STATUS_SET :: SNOOP1_UFLOW [13:13] */
#define BCHP_AUD_FMM_MS_ESR_STATUS_SET_SNOOP1_UFLOW_MASK           0x00002000
#define BCHP_AUD_FMM_MS_ESR_STATUS_SET_SNOOP1_UFLOW_SHIFT          13

/* AUD_FMM_MS_ESR :: STATUS_SET :: SNOOP0_OFLOW [12:12] */
#define BCHP_AUD_FMM_MS_ESR_STATUS_SET_SNOOP0_OFLOW_MASK           0x00001000
#define BCHP_AUD_FMM_MS_ESR_STATUS_SET_SNOOP0_OFLOW_SHIFT          12

/* AUD_FMM_MS_ESR :: STATUS_SET :: SNOOP0_UFLOW [11:11] */
#define BCHP_AUD_FMM_MS_ESR_STATUS_SET_SNOOP0_UFLOW_MASK           0x00000800
#define BCHP_AUD_FMM_MS_ESR_STATUS_SET_SNOOP0_UFLOW_SHIFT          11

/* AUD_FMM_MS_ESR :: STATUS_SET :: USEQ_SLEEP [10:10] */
#define BCHP_AUD_FMM_MS_ESR_STATUS_SET_USEQ_SLEEP_MASK             0x00000400
#define BCHP_AUD_FMM_MS_ESR_STATUS_SET_USEQ_SLEEP_SHIFT            10

/* AUD_FMM_MS_ESR :: STATUS_SET :: USEQ_ERROR1 [09:09] */
#define BCHP_AUD_FMM_MS_ESR_STATUS_SET_USEQ_ERROR1_MASK            0x00000200
#define BCHP_AUD_FMM_MS_ESR_STATUS_SET_USEQ_ERROR1_SHIFT           9

/* AUD_FMM_MS_ESR :: STATUS_SET :: CBIT_PING_PONG1 [08:08] */
#define BCHP_AUD_FMM_MS_ESR_STATUS_SET_CBIT_PING_PONG1_MASK        0x00000100
#define BCHP_AUD_FMM_MS_ESR_STATUS_SET_CBIT_PING_PONG1_SHIFT       8

/* AUD_FMM_MS_ESR :: STATUS_SET :: RAMP_DONE1 [07:07] */
#define BCHP_AUD_FMM_MS_ESR_STATUS_SET_RAMP_DONE1_MASK             0x00000080
#define BCHP_AUD_FMM_MS_ESR_STATUS_SET_RAMP_DONE1_SHIFT            7

/* AUD_FMM_MS_ESR :: STATUS_SET :: FLOWON1 [06:06] */
#define BCHP_AUD_FMM_MS_ESR_STATUS_SET_FLOWON1_MASK                0x00000040
#define BCHP_AUD_FMM_MS_ESR_STATUS_SET_FLOWON1_SHIFT               6

/* AUD_FMM_MS_ESR :: STATUS_SET :: UNDERFLOW1 [05:05] */
#define BCHP_AUD_FMM_MS_ESR_STATUS_SET_UNDERFLOW1_MASK             0x00000020
#define BCHP_AUD_FMM_MS_ESR_STATUS_SET_UNDERFLOW1_SHIFT            5

/* AUD_FMM_MS_ESR :: STATUS_SET :: USEQ_ERROR0 [04:04] */
#define BCHP_AUD_FMM_MS_ESR_STATUS_SET_USEQ_ERROR0_MASK            0x00000010
#define BCHP_AUD_FMM_MS_ESR_STATUS_SET_USEQ_ERROR0_SHIFT           4

/* AUD_FMM_MS_ESR :: STATUS_SET :: CBIT_PING_PONG0 [03:03] */
#define BCHP_AUD_FMM_MS_ESR_STATUS_SET_CBIT_PING_PONG0_MASK        0x00000008
#define BCHP_AUD_FMM_MS_ESR_STATUS_SET_CBIT_PING_PONG0_SHIFT       3

/* AUD_FMM_MS_ESR :: STATUS_SET :: RAMP_DONE0 [02:02] */
#define BCHP_AUD_FMM_MS_ESR_STATUS_SET_RAMP_DONE0_MASK             0x00000004
#define BCHP_AUD_FMM_MS_ESR_STATUS_SET_RAMP_DONE0_SHIFT            2

/* AUD_FMM_MS_ESR :: STATUS_SET :: FLOWON0 [01:01] */
#define BCHP_AUD_FMM_MS_ESR_STATUS_SET_FLOWON0_MASK                0x00000002
#define BCHP_AUD_FMM_MS_ESR_STATUS_SET_FLOWON0_SHIFT               1

/* AUD_FMM_MS_ESR :: STATUS_SET :: UNDERFLOW0 [00:00] */
#define BCHP_AUD_FMM_MS_ESR_STATUS_SET_UNDERFLOW0_MASK             0x00000001
#define BCHP_AUD_FMM_MS_ESR_STATUS_SET_UNDERFLOW0_SHIFT            0

/***************************************************************************
 *STATUS_CLEAR - Error Clear Register
 ***************************************************************************/
/* AUD_FMM_MS_ESR :: STATUS_CLEAR :: reserved0 [31:15] */
#define BCHP_AUD_FMM_MS_ESR_STATUS_CLEAR_reserved0_MASK            0xffff8000
#define BCHP_AUD_FMM_MS_ESR_STATUS_CLEAR_reserved0_SHIFT           15

/* AUD_FMM_MS_ESR :: STATUS_CLEAR :: SNOOP1_OFLOW [14:14] */
#define BCHP_AUD_FMM_MS_ESR_STATUS_CLEAR_SNOOP1_OFLOW_MASK         0x00004000
#define BCHP_AUD_FMM_MS_ESR_STATUS_CLEAR_SNOOP1_OFLOW_SHIFT        14

/* AUD_FMM_MS_ESR :: STATUS_CLEAR :: SNOOP1_UFLOW [13:13] */
#define BCHP_AUD_FMM_MS_ESR_STATUS_CLEAR_SNOOP1_UFLOW_MASK         0x00002000
#define BCHP_AUD_FMM_MS_ESR_STATUS_CLEAR_SNOOP1_UFLOW_SHIFT        13

/* AUD_FMM_MS_ESR :: STATUS_CLEAR :: SNOOP0_OFLOW [12:12] */
#define BCHP_AUD_FMM_MS_ESR_STATUS_CLEAR_SNOOP0_OFLOW_MASK         0x00001000
#define BCHP_AUD_FMM_MS_ESR_STATUS_CLEAR_SNOOP0_OFLOW_SHIFT        12

/* AUD_FMM_MS_ESR :: STATUS_CLEAR :: SNOOP0_UFLOW [11:11] */
#define BCHP_AUD_FMM_MS_ESR_STATUS_CLEAR_SNOOP0_UFLOW_MASK         0x00000800
#define BCHP_AUD_FMM_MS_ESR_STATUS_CLEAR_SNOOP0_UFLOW_SHIFT        11

/* AUD_FMM_MS_ESR :: STATUS_CLEAR :: USEQ_SLEEP [10:10] */
#define BCHP_AUD_FMM_MS_ESR_STATUS_CLEAR_USEQ_SLEEP_MASK           0x00000400
#define BCHP_AUD_FMM_MS_ESR_STATUS_CLEAR_USEQ_SLEEP_SHIFT          10

/* AUD_FMM_MS_ESR :: STATUS_CLEAR :: USEQ_ERROR1 [09:09] */
#define BCHP_AUD_FMM_MS_ESR_STATUS_CLEAR_USEQ_ERROR1_MASK          0x00000200
#define BCHP_AUD_FMM_MS_ESR_STATUS_CLEAR_USEQ_ERROR1_SHIFT         9

/* AUD_FMM_MS_ESR :: STATUS_CLEAR :: CBIT_PING_PONG1 [08:08] */
#define BCHP_AUD_FMM_MS_ESR_STATUS_CLEAR_CBIT_PING_PONG1_MASK      0x00000100
#define BCHP_AUD_FMM_MS_ESR_STATUS_CLEAR_CBIT_PING_PONG1_SHIFT     8

/* AUD_FMM_MS_ESR :: STATUS_CLEAR :: RAMP_DONE1 [07:07] */
#define BCHP_AUD_FMM_MS_ESR_STATUS_CLEAR_RAMP_DONE1_MASK           0x00000080
#define BCHP_AUD_FMM_MS_ESR_STATUS_CLEAR_RAMP_DONE1_SHIFT          7

/* AUD_FMM_MS_ESR :: STATUS_CLEAR :: FLOWON1 [06:06] */
#define BCHP_AUD_FMM_MS_ESR_STATUS_CLEAR_FLOWON1_MASK              0x00000040
#define BCHP_AUD_FMM_MS_ESR_STATUS_CLEAR_FLOWON1_SHIFT             6

/* AUD_FMM_MS_ESR :: STATUS_CLEAR :: UNDERFLOW1 [05:05] */
#define BCHP_AUD_FMM_MS_ESR_STATUS_CLEAR_UNDERFLOW1_MASK           0x00000020
#define BCHP_AUD_FMM_MS_ESR_STATUS_CLEAR_UNDERFLOW1_SHIFT          5

/* AUD_FMM_MS_ESR :: STATUS_CLEAR :: USEQ_ERROR0 [04:04] */
#define BCHP_AUD_FMM_MS_ESR_STATUS_CLEAR_USEQ_ERROR0_MASK          0x00000010
#define BCHP_AUD_FMM_MS_ESR_STATUS_CLEAR_USEQ_ERROR0_SHIFT         4

/* AUD_FMM_MS_ESR :: STATUS_CLEAR :: CBIT_PING_PONG0 [03:03] */
#define BCHP_AUD_FMM_MS_ESR_STATUS_CLEAR_CBIT_PING_PONG0_MASK      0x00000008
#define BCHP_AUD_FMM_MS_ESR_STATUS_CLEAR_CBIT_PING_PONG0_SHIFT     3

/* AUD_FMM_MS_ESR :: STATUS_CLEAR :: RAMP_DONE0 [02:02] */
#define BCHP_AUD_FMM_MS_ESR_STATUS_CLEAR_RAMP_DONE0_MASK           0x00000004
#define BCHP_AUD_FMM_MS_ESR_STATUS_CLEAR_RAMP_DONE0_SHIFT          2

/* AUD_FMM_MS_ESR :: STATUS_CLEAR :: FLOWON0 [01:01] */
#define BCHP_AUD_FMM_MS_ESR_STATUS_CLEAR_FLOWON0_MASK              0x00000002
#define BCHP_AUD_FMM_MS_ESR_STATUS_CLEAR_FLOWON0_SHIFT             1

/* AUD_FMM_MS_ESR :: STATUS_CLEAR :: UNDERFLOW0 [00:00] */
#define BCHP_AUD_FMM_MS_ESR_STATUS_CLEAR_UNDERFLOW0_MASK           0x00000001
#define BCHP_AUD_FMM_MS_ESR_STATUS_CLEAR_UNDERFLOW0_SHIFT          0

/***************************************************************************
 *MASK - Mask Status Register
 ***************************************************************************/
/* AUD_FMM_MS_ESR :: MASK :: reserved0 [31:15] */
#define BCHP_AUD_FMM_MS_ESR_MASK_reserved0_MASK                    0xffff8000
#define BCHP_AUD_FMM_MS_ESR_MASK_reserved0_SHIFT                   15

/* AUD_FMM_MS_ESR :: MASK :: SNOOP1_OFLOW [14:14] */
#define BCHP_AUD_FMM_MS_ESR_MASK_SNOOP1_OFLOW_MASK                 0x00004000
#define BCHP_AUD_FMM_MS_ESR_MASK_SNOOP1_OFLOW_SHIFT                14

/* AUD_FMM_MS_ESR :: MASK :: SNOOP1_UFLOW [13:13] */
#define BCHP_AUD_FMM_MS_ESR_MASK_SNOOP1_UFLOW_MASK                 0x00002000
#define BCHP_AUD_FMM_MS_ESR_MASK_SNOOP1_UFLOW_SHIFT                13

/* AUD_FMM_MS_ESR :: MASK :: SNOOP0_OFLOW [12:12] */
#define BCHP_AUD_FMM_MS_ESR_MASK_SNOOP0_OFLOW_MASK                 0x00001000
#define BCHP_AUD_FMM_MS_ESR_MASK_SNOOP0_OFLOW_SHIFT                12

/* AUD_FMM_MS_ESR :: MASK :: SNOOP0_UFLOW [11:11] */
#define BCHP_AUD_FMM_MS_ESR_MASK_SNOOP0_UFLOW_MASK                 0x00000800
#define BCHP_AUD_FMM_MS_ESR_MASK_SNOOP0_UFLOW_SHIFT                11

/* AUD_FMM_MS_ESR :: MASK :: USEQ_SLEEP [10:10] */
#define BCHP_AUD_FMM_MS_ESR_MASK_USEQ_SLEEP_MASK                   0x00000400
#define BCHP_AUD_FMM_MS_ESR_MASK_USEQ_SLEEP_SHIFT                  10

/* AUD_FMM_MS_ESR :: MASK :: USEQ_ERROR1 [09:09] */
#define BCHP_AUD_FMM_MS_ESR_MASK_USEQ_ERROR1_MASK                  0x00000200
#define BCHP_AUD_FMM_MS_ESR_MASK_USEQ_ERROR1_SHIFT                 9

/* AUD_FMM_MS_ESR :: MASK :: CBIT_PING_PONG1 [08:08] */
#define BCHP_AUD_FMM_MS_ESR_MASK_CBIT_PING_PONG1_MASK              0x00000100
#define BCHP_AUD_FMM_MS_ESR_MASK_CBIT_PING_PONG1_SHIFT             8

/* AUD_FMM_MS_ESR :: MASK :: RAMP_DONE1 [07:07] */
#define BCHP_AUD_FMM_MS_ESR_MASK_RAMP_DONE1_MASK                   0x00000080
#define BCHP_AUD_FMM_MS_ESR_MASK_RAMP_DONE1_SHIFT                  7

/* AUD_FMM_MS_ESR :: MASK :: FLOWON1 [06:06] */
#define BCHP_AUD_FMM_MS_ESR_MASK_FLOWON1_MASK                      0x00000040
#define BCHP_AUD_FMM_MS_ESR_MASK_FLOWON1_SHIFT                     6

/* AUD_FMM_MS_ESR :: MASK :: UNDERFLOW1 [05:05] */
#define BCHP_AUD_FMM_MS_ESR_MASK_UNDERFLOW1_MASK                   0x00000020
#define BCHP_AUD_FMM_MS_ESR_MASK_UNDERFLOW1_SHIFT                  5

/* AUD_FMM_MS_ESR :: MASK :: USEQ_ERROR0 [04:04] */
#define BCHP_AUD_FMM_MS_ESR_MASK_USEQ_ERROR0_MASK                  0x00000010
#define BCHP_AUD_FMM_MS_ESR_MASK_USEQ_ERROR0_SHIFT                 4

/* AUD_FMM_MS_ESR :: MASK :: CBIT_PING_PONG0 [03:03] */
#define BCHP_AUD_FMM_MS_ESR_MASK_CBIT_PING_PONG0_MASK              0x00000008
#define BCHP_AUD_FMM_MS_ESR_MASK_CBIT_PING_PONG0_SHIFT             3

/* AUD_FMM_MS_ESR :: MASK :: RAMP_DONE0 [02:02] */
#define BCHP_AUD_FMM_MS_ESR_MASK_RAMP_DONE0_MASK                   0x00000004
#define BCHP_AUD_FMM_MS_ESR_MASK_RAMP_DONE0_SHIFT                  2

/* AUD_FMM_MS_ESR :: MASK :: FLOWON0 [01:01] */
#define BCHP_AUD_FMM_MS_ESR_MASK_FLOWON0_MASK                      0x00000002
#define BCHP_AUD_FMM_MS_ESR_MASK_FLOWON0_SHIFT                     1

/* AUD_FMM_MS_ESR :: MASK :: UNDERFLOW0 [00:00] */
#define BCHP_AUD_FMM_MS_ESR_MASK_UNDERFLOW0_MASK                   0x00000001
#define BCHP_AUD_FMM_MS_ESR_MASK_UNDERFLOW0_SHIFT                  0

/***************************************************************************
 *MASK_SET - Mask Set Register
 ***************************************************************************/
/* AUD_FMM_MS_ESR :: MASK_SET :: reserved0 [31:15] */
#define BCHP_AUD_FMM_MS_ESR_MASK_SET_reserved0_MASK                0xffff8000
#define BCHP_AUD_FMM_MS_ESR_MASK_SET_reserved0_SHIFT               15

/* AUD_FMM_MS_ESR :: MASK_SET :: SNOOP1_OFLOW [14:14] */
#define BCHP_AUD_FMM_MS_ESR_MASK_SET_SNOOP1_OFLOW_MASK             0x00004000
#define BCHP_AUD_FMM_MS_ESR_MASK_SET_SNOOP1_OFLOW_SHIFT            14

/* AUD_FMM_MS_ESR :: MASK_SET :: SNOOP1_UFLOW [13:13] */
#define BCHP_AUD_FMM_MS_ESR_MASK_SET_SNOOP1_UFLOW_MASK             0x00002000
#define BCHP_AUD_FMM_MS_ESR_MASK_SET_SNOOP1_UFLOW_SHIFT            13

/* AUD_FMM_MS_ESR :: MASK_SET :: SNOOP0_OFLOW [12:12] */
#define BCHP_AUD_FMM_MS_ESR_MASK_SET_SNOOP0_OFLOW_MASK             0x00001000
#define BCHP_AUD_FMM_MS_ESR_MASK_SET_SNOOP0_OFLOW_SHIFT            12

/* AUD_FMM_MS_ESR :: MASK_SET :: SNOOP0_UFLOW [11:11] */
#define BCHP_AUD_FMM_MS_ESR_MASK_SET_SNOOP0_UFLOW_MASK             0x00000800
#define BCHP_AUD_FMM_MS_ESR_MASK_SET_SNOOP0_UFLOW_SHIFT            11

/* AUD_FMM_MS_ESR :: MASK_SET :: USEQ_SLEEP [10:10] */
#define BCHP_AUD_FMM_MS_ESR_MASK_SET_USEQ_SLEEP_MASK               0x00000400
#define BCHP_AUD_FMM_MS_ESR_MASK_SET_USEQ_SLEEP_SHIFT              10

/* AUD_FMM_MS_ESR :: MASK_SET :: USEQ_ERROR1 [09:09] */
#define BCHP_AUD_FMM_MS_ESR_MASK_SET_USEQ_ERROR1_MASK              0x00000200
#define BCHP_AUD_FMM_MS_ESR_MASK_SET_USEQ_ERROR1_SHIFT             9

/* AUD_FMM_MS_ESR :: MASK_SET :: CBIT_PING_PONG1 [08:08] */
#define BCHP_AUD_FMM_MS_ESR_MASK_SET_CBIT_PING_PONG1_MASK          0x00000100
#define BCHP_AUD_FMM_MS_ESR_MASK_SET_CBIT_PING_PONG1_SHIFT         8

/* AUD_FMM_MS_ESR :: MASK_SET :: RAMP_DONE1 [07:07] */
#define BCHP_AUD_FMM_MS_ESR_MASK_SET_RAMP_DONE1_MASK               0x00000080
#define BCHP_AUD_FMM_MS_ESR_MASK_SET_RAMP_DONE1_SHIFT              7

/* AUD_FMM_MS_ESR :: MASK_SET :: FLOWON1 [06:06] */
#define BCHP_AUD_FMM_MS_ESR_MASK_SET_FLOWON1_MASK                  0x00000040
#define BCHP_AUD_FMM_MS_ESR_MASK_SET_FLOWON1_SHIFT                 6

/* AUD_FMM_MS_ESR :: MASK_SET :: UNDERFLOW1 [05:05] */
#define BCHP_AUD_FMM_MS_ESR_MASK_SET_UNDERFLOW1_MASK               0x00000020
#define BCHP_AUD_FMM_MS_ESR_MASK_SET_UNDERFLOW1_SHIFT              5

/* AUD_FMM_MS_ESR :: MASK_SET :: USEQ_ERROR0 [04:04] */
#define BCHP_AUD_FMM_MS_ESR_MASK_SET_USEQ_ERROR0_MASK              0x00000010
#define BCHP_AUD_FMM_MS_ESR_MASK_SET_USEQ_ERROR0_SHIFT             4

/* AUD_FMM_MS_ESR :: MASK_SET :: CBIT_PING_PONG0 [03:03] */
#define BCHP_AUD_FMM_MS_ESR_MASK_SET_CBIT_PING_PONG0_MASK          0x00000008
#define BCHP_AUD_FMM_MS_ESR_MASK_SET_CBIT_PING_PONG0_SHIFT         3

/* AUD_FMM_MS_ESR :: MASK_SET :: RAMP_DONE0 [02:02] */
#define BCHP_AUD_FMM_MS_ESR_MASK_SET_RAMP_DONE0_MASK               0x00000004
#define BCHP_AUD_FMM_MS_ESR_MASK_SET_RAMP_DONE0_SHIFT              2

/* AUD_FMM_MS_ESR :: MASK_SET :: FLOWON0 [01:01] */
#define BCHP_AUD_FMM_MS_ESR_MASK_SET_FLOWON0_MASK                  0x00000002
#define BCHP_AUD_FMM_MS_ESR_MASK_SET_FLOWON0_SHIFT                 1

/* AUD_FMM_MS_ESR :: MASK_SET :: UNDERFLOW0 [00:00] */
#define BCHP_AUD_FMM_MS_ESR_MASK_SET_UNDERFLOW0_MASK               0x00000001
#define BCHP_AUD_FMM_MS_ESR_MASK_SET_UNDERFLOW0_SHIFT              0

/***************************************************************************
 *MASK_CLEAR - Mask Clear Register
 ***************************************************************************/
/* AUD_FMM_MS_ESR :: MASK_CLEAR :: reserved0 [31:15] */
#define BCHP_AUD_FMM_MS_ESR_MASK_CLEAR_reserved0_MASK              0xffff8000
#define BCHP_AUD_FMM_MS_ESR_MASK_CLEAR_reserved0_SHIFT             15

/* AUD_FMM_MS_ESR :: MASK_CLEAR :: SNOOP1_OFLOW [14:14] */
#define BCHP_AUD_FMM_MS_ESR_MASK_CLEAR_SNOOP1_OFLOW_MASK           0x00004000
#define BCHP_AUD_FMM_MS_ESR_MASK_CLEAR_SNOOP1_OFLOW_SHIFT          14

/* AUD_FMM_MS_ESR :: MASK_CLEAR :: SNOOP1_UFLOW [13:13] */
#define BCHP_AUD_FMM_MS_ESR_MASK_CLEAR_SNOOP1_UFLOW_MASK           0x00002000
#define BCHP_AUD_FMM_MS_ESR_MASK_CLEAR_SNOOP1_UFLOW_SHIFT          13

/* AUD_FMM_MS_ESR :: MASK_CLEAR :: SNOOP0_OFLOW [12:12] */
#define BCHP_AUD_FMM_MS_ESR_MASK_CLEAR_SNOOP0_OFLOW_MASK           0x00001000
#define BCHP_AUD_FMM_MS_ESR_MASK_CLEAR_SNOOP0_OFLOW_SHIFT          12

/* AUD_FMM_MS_ESR :: MASK_CLEAR :: SNOOP0_UFLOW [11:11] */
#define BCHP_AUD_FMM_MS_ESR_MASK_CLEAR_SNOOP0_UFLOW_MASK           0x00000800
#define BCHP_AUD_FMM_MS_ESR_MASK_CLEAR_SNOOP0_UFLOW_SHIFT          11

/* AUD_FMM_MS_ESR :: MASK_CLEAR :: USEQ_SLEEP [10:10] */
#define BCHP_AUD_FMM_MS_ESR_MASK_CLEAR_USEQ_SLEEP_MASK             0x00000400
#define BCHP_AUD_FMM_MS_ESR_MASK_CLEAR_USEQ_SLEEP_SHIFT            10

/* AUD_FMM_MS_ESR :: MASK_CLEAR :: USEQ_ERROR1 [09:09] */
#define BCHP_AUD_FMM_MS_ESR_MASK_CLEAR_USEQ_ERROR1_MASK            0x00000200
#define BCHP_AUD_FMM_MS_ESR_MASK_CLEAR_USEQ_ERROR1_SHIFT           9

/* AUD_FMM_MS_ESR :: MASK_CLEAR :: CBIT_PING_PONG1 [08:08] */
#define BCHP_AUD_FMM_MS_ESR_MASK_CLEAR_CBIT_PING_PONG1_MASK        0x00000100
#define BCHP_AUD_FMM_MS_ESR_MASK_CLEAR_CBIT_PING_PONG1_SHIFT       8

/* AUD_FMM_MS_ESR :: MASK_CLEAR :: RAMP_DONE1 [07:07] */
#define BCHP_AUD_FMM_MS_ESR_MASK_CLEAR_RAMP_DONE1_MASK             0x00000080
#define BCHP_AUD_FMM_MS_ESR_MASK_CLEAR_RAMP_DONE1_SHIFT            7

/* AUD_FMM_MS_ESR :: MASK_CLEAR :: FLOWON1 [06:06] */
#define BCHP_AUD_FMM_MS_ESR_MASK_CLEAR_FLOWON1_MASK                0x00000040
#define BCHP_AUD_FMM_MS_ESR_MASK_CLEAR_FLOWON1_SHIFT               6

/* AUD_FMM_MS_ESR :: MASK_CLEAR :: UNDERFLOW1 [05:05] */
#define BCHP_AUD_FMM_MS_ESR_MASK_CLEAR_UNDERFLOW1_MASK             0x00000020
#define BCHP_AUD_FMM_MS_ESR_MASK_CLEAR_UNDERFLOW1_SHIFT            5

/* AUD_FMM_MS_ESR :: MASK_CLEAR :: USEQ_ERROR0 [04:04] */
#define BCHP_AUD_FMM_MS_ESR_MASK_CLEAR_USEQ_ERROR0_MASK            0x00000010
#define BCHP_AUD_FMM_MS_ESR_MASK_CLEAR_USEQ_ERROR0_SHIFT           4

/* AUD_FMM_MS_ESR :: MASK_CLEAR :: CBIT_PING_PONG0 [03:03] */
#define BCHP_AUD_FMM_MS_ESR_MASK_CLEAR_CBIT_PING_PONG0_MASK        0x00000008
#define BCHP_AUD_FMM_MS_ESR_MASK_CLEAR_CBIT_PING_PONG0_SHIFT       3

/* AUD_FMM_MS_ESR :: MASK_CLEAR :: RAMP_DONE0 [02:02] */
#define BCHP_AUD_FMM_MS_ESR_MASK_CLEAR_RAMP_DONE0_MASK             0x00000004
#define BCHP_AUD_FMM_MS_ESR_MASK_CLEAR_RAMP_DONE0_SHIFT            2

/* AUD_FMM_MS_ESR :: MASK_CLEAR :: FLOWON0 [01:01] */
#define BCHP_AUD_FMM_MS_ESR_MASK_CLEAR_FLOWON0_MASK                0x00000002
#define BCHP_AUD_FMM_MS_ESR_MASK_CLEAR_FLOWON0_SHIFT               1

/* AUD_FMM_MS_ESR :: MASK_CLEAR :: UNDERFLOW0 [00:00] */
#define BCHP_AUD_FMM_MS_ESR_MASK_CLEAR_UNDERFLOW0_MASK             0x00000001
#define BCHP_AUD_FMM_MS_ESR_MASK_CLEAR_UNDERFLOW0_SHIFT            0

#endif /* #ifndef BCHP_AUD_FMM_MS_ESR_H__ */

/* End of File */