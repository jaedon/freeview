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
 * $brcm_Workfile: bchp_aud_fmm_src_esr0.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 6/15/12 5:56p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Wed Jun 13 16:33:10 2012
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
 * $brcm_Log: /magnum/basemodules/chp/7429/rdb/a0/bchp_aud_fmm_src_esr0.h $
 * 
 * Hydra_Software_Devel/2   6/15/12 5:56p pntruong
 * SW7429-185: Resynced with central rdb.
 *
 ***************************************************************************/

#ifndef BCHP_AUD_FMM_SRC_ESR0_H__
#define BCHP_AUD_FMM_SRC_ESR0_H__

/***************************************************************************
 *AUD_FMM_SRC_ESR0 - Audio SRC Error Status 0
 ***************************************************************************/
#define BCHP_AUD_FMM_SRC_ESR0_STATUS             0x008a3000 /* Error Status Register */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_SET         0x008a3004 /* Error Set Register */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_CLEAR       0x008a3008 /* Error Clear Register */
#define BCHP_AUD_FMM_SRC_ESR0_MASK               0x008a300c /* Mask Status Register */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_SET           0x008a3010 /* Mask Set Register */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_CLEAR         0x008a3014 /* Mask Clear Register */

/***************************************************************************
 *STATUS - Error Status Register
 ***************************************************************************/
/* AUD_FMM_SRC_ESR0 :: STATUS :: reserved0 [31:25] */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_reserved0_MASK                0xfe000000
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_reserved0_SHIFT               25

/* AUD_FMM_SRC_ESR0 :: STATUS :: ACCUM_SAT [24:24] */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_ACCUM_SAT_MASK                0x01000000
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_ACCUM_SAT_SHIFT               24
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_ACCUM_SAT_DEFAULT             0x00000000

/* AUD_FMM_SRC_ESR0 :: STATUS :: CF_BANK_READY [23:12] */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_CF_BANK_READY_MASK            0x00fff000
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_CF_BANK_READY_SHIFT           12
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_CF_BANK_READY_DEFAULT         0x00000000

/* AUD_FMM_SRC_ESR0 :: STATUS :: UNDERFLOW [11:00] */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_UNDERFLOW_MASK                0x00000fff
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_UNDERFLOW_SHIFT               0
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_UNDERFLOW_DEFAULT             0x00000000

/***************************************************************************
 *STATUS_SET - Error Set Register
 ***************************************************************************/
/* AUD_FMM_SRC_ESR0 :: STATUS_SET :: reserved0 [31:25] */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_SET_reserved0_MASK            0xfe000000
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_SET_reserved0_SHIFT           25

/* AUD_FMM_SRC_ESR0 :: STATUS_SET :: ACCUM_SAT [24:24] */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_SET_ACCUM_SAT_MASK            0x01000000
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_SET_ACCUM_SAT_SHIFT           24
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_SET_ACCUM_SAT_DEFAULT         0x00000000

/* AUD_FMM_SRC_ESR0 :: STATUS_SET :: CF_BANK_READY [23:12] */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_SET_CF_BANK_READY_MASK        0x00fff000
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_SET_CF_BANK_READY_SHIFT       12
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_SET_CF_BANK_READY_DEFAULT     0x00000000

/* AUD_FMM_SRC_ESR0 :: STATUS_SET :: UNDERFLOW [11:00] */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_SET_UNDERFLOW_MASK            0x00000fff
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_SET_UNDERFLOW_SHIFT           0
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_SET_UNDERFLOW_DEFAULT         0x00000000

/***************************************************************************
 *STATUS_CLEAR - Error Clear Register
 ***************************************************************************/
/* AUD_FMM_SRC_ESR0 :: STATUS_CLEAR :: reserved0 [31:25] */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_CLEAR_reserved0_MASK          0xfe000000
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_CLEAR_reserved0_SHIFT         25

/* AUD_FMM_SRC_ESR0 :: STATUS_CLEAR :: ACCUM_SAT [24:24] */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_CLEAR_ACCUM_SAT_MASK          0x01000000
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_CLEAR_ACCUM_SAT_SHIFT         24
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_CLEAR_ACCUM_SAT_DEFAULT       0x00000000

/* AUD_FMM_SRC_ESR0 :: STATUS_CLEAR :: CF_BANK_READY [23:12] */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_CLEAR_CF_BANK_READY_MASK      0x00fff000
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_CLEAR_CF_BANK_READY_SHIFT     12
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_CLEAR_CF_BANK_READY_DEFAULT   0x00000000

/* AUD_FMM_SRC_ESR0 :: STATUS_CLEAR :: UNDERFLOW [11:00] */
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_CLEAR_UNDERFLOW_MASK          0x00000fff
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_CLEAR_UNDERFLOW_SHIFT         0
#define BCHP_AUD_FMM_SRC_ESR0_STATUS_CLEAR_UNDERFLOW_DEFAULT       0x00000000

/***************************************************************************
 *MASK - Mask Status Register
 ***************************************************************************/
/* AUD_FMM_SRC_ESR0 :: MASK :: reserved0 [31:25] */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_reserved0_MASK                  0xfe000000
#define BCHP_AUD_FMM_SRC_ESR0_MASK_reserved0_SHIFT                 25

/* AUD_FMM_SRC_ESR0 :: MASK :: ACCUM_SAT [24:24] */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_ACCUM_SAT_MASK                  0x01000000
#define BCHP_AUD_FMM_SRC_ESR0_MASK_ACCUM_SAT_SHIFT                 24
#define BCHP_AUD_FMM_SRC_ESR0_MASK_ACCUM_SAT_DEFAULT               0x00000001

/* AUD_FMM_SRC_ESR0 :: MASK :: CF_BANK_READY [23:12] */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_CF_BANK_READY_MASK              0x00fff000
#define BCHP_AUD_FMM_SRC_ESR0_MASK_CF_BANK_READY_SHIFT             12
#define BCHP_AUD_FMM_SRC_ESR0_MASK_CF_BANK_READY_DEFAULT           0x00000fff

/* AUD_FMM_SRC_ESR0 :: MASK :: UNDERFLOW [11:00] */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_UNDERFLOW_MASK                  0x00000fff
#define BCHP_AUD_FMM_SRC_ESR0_MASK_UNDERFLOW_SHIFT                 0
#define BCHP_AUD_FMM_SRC_ESR0_MASK_UNDERFLOW_DEFAULT               0x00000fff

/***************************************************************************
 *MASK_SET - Mask Set Register
 ***************************************************************************/
/* AUD_FMM_SRC_ESR0 :: MASK_SET :: reserved0 [31:25] */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_SET_reserved0_MASK              0xfe000000
#define BCHP_AUD_FMM_SRC_ESR0_MASK_SET_reserved0_SHIFT             25

/* AUD_FMM_SRC_ESR0 :: MASK_SET :: ACCUM_SAT [24:24] */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_SET_ACCUM_SAT_MASK              0x01000000
#define BCHP_AUD_FMM_SRC_ESR0_MASK_SET_ACCUM_SAT_SHIFT             24
#define BCHP_AUD_FMM_SRC_ESR0_MASK_SET_ACCUM_SAT_DEFAULT           0x00000001

/* AUD_FMM_SRC_ESR0 :: MASK_SET :: CF_BANK_READY [23:12] */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_SET_CF_BANK_READY_MASK          0x00fff000
#define BCHP_AUD_FMM_SRC_ESR0_MASK_SET_CF_BANK_READY_SHIFT         12
#define BCHP_AUD_FMM_SRC_ESR0_MASK_SET_CF_BANK_READY_DEFAULT       0x00000fff

/* AUD_FMM_SRC_ESR0 :: MASK_SET :: UNDERFLOW [11:00] */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_SET_UNDERFLOW_MASK              0x00000fff
#define BCHP_AUD_FMM_SRC_ESR0_MASK_SET_UNDERFLOW_SHIFT             0
#define BCHP_AUD_FMM_SRC_ESR0_MASK_SET_UNDERFLOW_DEFAULT           0x00000fff

/***************************************************************************
 *MASK_CLEAR - Mask Clear Register
 ***************************************************************************/
/* AUD_FMM_SRC_ESR0 :: MASK_CLEAR :: reserved0 [31:25] */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_CLEAR_reserved0_MASK            0xfe000000
#define BCHP_AUD_FMM_SRC_ESR0_MASK_CLEAR_reserved0_SHIFT           25

/* AUD_FMM_SRC_ESR0 :: MASK_CLEAR :: ACCUM_SAT [24:24] */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_CLEAR_ACCUM_SAT_MASK            0x01000000
#define BCHP_AUD_FMM_SRC_ESR0_MASK_CLEAR_ACCUM_SAT_SHIFT           24
#define BCHP_AUD_FMM_SRC_ESR0_MASK_CLEAR_ACCUM_SAT_DEFAULT         0x00000001

/* AUD_FMM_SRC_ESR0 :: MASK_CLEAR :: CF_BANK_READY [23:12] */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_CLEAR_CF_BANK_READY_MASK        0x00fff000
#define BCHP_AUD_FMM_SRC_ESR0_MASK_CLEAR_CF_BANK_READY_SHIFT       12
#define BCHP_AUD_FMM_SRC_ESR0_MASK_CLEAR_CF_BANK_READY_DEFAULT     0x00000fff

/* AUD_FMM_SRC_ESR0 :: MASK_CLEAR :: UNDERFLOW [11:00] */
#define BCHP_AUD_FMM_SRC_ESR0_MASK_CLEAR_UNDERFLOW_MASK            0x00000fff
#define BCHP_AUD_FMM_SRC_ESR0_MASK_CLEAR_UNDERFLOW_SHIFT           0
#define BCHP_AUD_FMM_SRC_ESR0_MASK_CLEAR_UNDERFLOW_DEFAULT         0x00000fff

#endif /* #ifndef BCHP_AUD_FMM_SRC_ESR0_H__ */

/* End of File */