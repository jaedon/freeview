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
 * $brcm_Workfile: bchp_dtram_0.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 6/14/12 12:21p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Wed Jun 13 16:39:04 2012
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
 * $brcm_Log: /magnum/basemodules/chp/7429/rdb/b0/bchp_dtram_0.h $
 * 
 * Hydra_Software_Devel/1   6/14/12 12:21p pntruong
 * SW7429-185: Initial version.
 *
 ***************************************************************************/

#ifndef BCHP_DTRAM_0_H__
#define BCHP_DTRAM_0_H__

/***************************************************************************
 *DTRAM_0 - DVI and 656 Micro-Program Store
 ***************************************************************************/
#define BCHP_DTRAM_0_DTRAM_REV_ID                0x00684000 /* Revision ID register for DTRAM block */
#define BCHP_DTRAM_0_DTRAM_CONFIG                0x00684008 /* DTRAM Configuration Register */
#define BCHP_DTRAM_0_DTRAM_656_ARBITER_LATENCY   0x0068400c /* DTRAM ITU656 ARBITER LATENCY */

/***************************************************************************
 *DTRAM_REV_ID - Revision ID register for DTRAM block
 ***************************************************************************/
/* DTRAM_0 :: DTRAM_REV_ID :: reserved0 [31:16] */
#define BCHP_DTRAM_0_DTRAM_REV_ID_reserved0_MASK                   0xffff0000
#define BCHP_DTRAM_0_DTRAM_REV_ID_reserved0_SHIFT                  16

/* DTRAM_0 :: DTRAM_REV_ID :: REVISION_ID [15:00] */
#define BCHP_DTRAM_0_DTRAM_REV_ID_REVISION_ID_MASK                 0x0000ffff
#define BCHP_DTRAM_0_DTRAM_REV_ID_REVISION_ID_SHIFT                0
#define BCHP_DTRAM_0_DTRAM_REV_ID_REVISION_ID_DEFAULT              0x00003000

/***************************************************************************
 *DTRAM_CONFIG - DTRAM Configuration Register
 ***************************************************************************/
/* DTRAM_0 :: DTRAM_CONFIG :: reserved0 [31:07] */
#define BCHP_DTRAM_0_DTRAM_CONFIG_reserved0_MASK                   0xffffff80
#define BCHP_DTRAM_0_DTRAM_CONFIG_reserved0_SHIFT                  7

/* DTRAM_0 :: DTRAM_CONFIG :: reserved_for_eco1 [06:05] */
#define BCHP_DTRAM_0_DTRAM_CONFIG_reserved_for_eco1_MASK           0x00000060
#define BCHP_DTRAM_0_DTRAM_CONFIG_reserved_for_eco1_SHIFT          5
#define BCHP_DTRAM_0_DTRAM_CONFIG_reserved_for_eco1_DEFAULT        0x00000000

/* DTRAM_0 :: DTRAM_CONFIG :: ARBITER_LATENCY [04:00] */
#define BCHP_DTRAM_0_DTRAM_CONFIG_ARBITER_LATENCY_MASK             0x0000001f
#define BCHP_DTRAM_0_DTRAM_CONFIG_ARBITER_LATENCY_SHIFT            0
#define BCHP_DTRAM_0_DTRAM_CONFIG_ARBITER_LATENCY_DEFAULT          0x00000004

/***************************************************************************
 *DTRAM_656_ARBITER_LATENCY - DTRAM ITU656 ARBITER LATENCY
 ***************************************************************************/
/* DTRAM_0 :: DTRAM_656_ARBITER_LATENCY :: reserved0 [31:05] */
#define BCHP_DTRAM_0_DTRAM_656_ARBITER_LATENCY_reserved0_MASK      0xffffffe0
#define BCHP_DTRAM_0_DTRAM_656_ARBITER_LATENCY_reserved0_SHIFT     5

/* DTRAM_0 :: DTRAM_656_ARBITER_LATENCY :: ARBITER_LATENCY [04:00] */
#define BCHP_DTRAM_0_DTRAM_656_ARBITER_LATENCY_ARBITER_LATENCY_MASK 0x0000001f
#define BCHP_DTRAM_0_DTRAM_656_ARBITER_LATENCY_ARBITER_LATENCY_SHIFT 0
#define BCHP_DTRAM_0_DTRAM_656_ARBITER_LATENCY_ARBITER_LATENCY_DEFAULT 0x0000000b

/***************************************************************************
 *DMC_INSTRUCTION%i - DTRAM Register at location 0..255
 ***************************************************************************/
#define BCHP_DTRAM_0_DMC_INSTRUCTIONi_ARRAY_BASE                   0x00684080
#define BCHP_DTRAM_0_DMC_INSTRUCTIONi_ARRAY_START                  0
#define BCHP_DTRAM_0_DMC_INSTRUCTIONi_ARRAY_END                    255
#define BCHP_DTRAM_0_DMC_INSTRUCTIONi_ARRAY_ELEMENT_SIZE           32

/***************************************************************************
 *DMC_INSTRUCTION%i - DTRAM Register at location 0..255
 ***************************************************************************/
/* DTRAM_0 :: DMC_INSTRUCTIONi :: reserved0 [31:24] */
#define BCHP_DTRAM_0_DMC_INSTRUCTIONi_reserved0_MASK               0xff000000
#define BCHP_DTRAM_0_DMC_INSTRUCTIONi_reserved0_SHIFT              24

/* DTRAM_0 :: DMC_INSTRUCTIONi :: OPCODE [23:21] */
#define BCHP_DTRAM_0_DMC_INSTRUCTIONi_OPCODE_MASK                  0x00e00000
#define BCHP_DTRAM_0_DMC_INSTRUCTIONi_OPCODE_SHIFT                 21
#define BCHP_DTRAM_0_DMC_INSTRUCTIONi_OPCODE_NOP                   0
#define BCHP_DTRAM_0_DMC_INSTRUCTIONi_OPCODE_SCOUNT                1
#define BCHP_DTRAM_0_DMC_INSTRUCTIONi_OPCODE_SCOUNTI               2
#define BCHP_DTRAM_0_DMC_INSTRUCTIONi_OPCODE_CALL                  3
#define BCHP_DTRAM_0_DMC_INSTRUCTIONi_OPCODE_JUMP                  4
#define BCHP_DTRAM_0_DMC_INSTRUCTIONi_OPCODE_RELOAD                5
#define BCHP_DTRAM_0_DMC_INSTRUCTIONi_OPCODE_LOAD                  6
#define BCHP_DTRAM_0_DMC_INSTRUCTIONi_OPCODE_CALLI                 7

/* DTRAM_0 :: DMC_INSTRUCTIONi :: RETURN_FLAG [20:20] */
#define BCHP_DTRAM_0_DMC_INSTRUCTIONi_RETURN_FLAG_MASK             0x00100000
#define BCHP_DTRAM_0_DMC_INSTRUCTIONi_RETURN_FLAG_SHIFT            20

/* DTRAM_0 :: DMC_INSTRUCTIONi :: FLAGS_OR_ADDR [19:12] */
#define BCHP_DTRAM_0_DMC_INSTRUCTIONi_FLAGS_OR_ADDR_MASK           0x000ff000
#define BCHP_DTRAM_0_DMC_INSTRUCTIONi_FLAGS_OR_ADDR_SHIFT          12

/* union - case SCOUNT [11:00] */
/* DTRAM_0 :: DMC_INSTRUCTIONi :: SCOUNT :: COUNT [11:00] */
#define BCHP_DTRAM_0_DMC_INSTRUCTIONi_SCOUNT_COUNT_MASK            0x00000fff
#define BCHP_DTRAM_0_DMC_INSTRUCTIONi_SCOUNT_COUNT_SHIFT           0

/* union - case CALL [11:00] */
/* DTRAM_0 :: DMC_INSTRUCTIONi :: CALL :: COUNT [11:00] */
#define BCHP_DTRAM_0_DMC_INSTRUCTIONi_CALL_COUNT_MASK              0x00000fff
#define BCHP_DTRAM_0_DMC_INSTRUCTIONi_CALL_COUNT_SHIFT             0

/* union - case JUMP [11:00] */
/* DTRAM_0 :: DMC_INSTRUCTIONi :: JUMP :: reserved0 [11:00] */
#define BCHP_DTRAM_0_DMC_INSTRUCTIONi_JUMP_reserved0_MASK          0x00000fff
#define BCHP_DTRAM_0_DMC_INSTRUCTIONi_JUMP_reserved0_SHIFT         0

/* union - case RELOAD [11:00] */
/* DTRAM_0 :: DMC_INSTRUCTIONi :: RELOAD :: reserved0 [11:00] */
#define BCHP_DTRAM_0_DMC_INSTRUCTIONi_RELOAD_reserved0_MASK        0x00000fff
#define BCHP_DTRAM_0_DMC_INSTRUCTIONi_RELOAD_reserved0_SHIFT       0

/* union - case LOAD [11:00] */
/* DTRAM_0 :: DMC_INSTRUCTIONi :: LOAD :: reserved0 [11:00] */
#define BCHP_DTRAM_0_DMC_INSTRUCTIONi_LOAD_reserved0_MASK          0x00000fff
#define BCHP_DTRAM_0_DMC_INSTRUCTIONi_LOAD_reserved0_SHIFT         0

/* union - case CALLI [11:00] */
/* DTRAM_0 :: DMC_INSTRUCTIONi :: CALLI :: reserved0 [11:00] */
#define BCHP_DTRAM_0_DMC_INSTRUCTIONi_CALLI_reserved0_MASK         0x00000fff
#define BCHP_DTRAM_0_DMC_INSTRUCTIONi_CALLI_reserved0_SHIFT        0

/* union - case SCOUNTI [11:00] */
/* DTRAM_0 :: DMC_INSTRUCTIONi :: SCOUNTI :: COUNT [11:00] */
#define BCHP_DTRAM_0_DMC_INSTRUCTIONi_SCOUNTI_COUNT_MASK           0x00000fff
#define BCHP_DTRAM_0_DMC_INSTRUCTIONi_SCOUNTI_COUNT_SHIFT          0


#endif /* #ifndef BCHP_DTRAM_0_H__ */

/* End of File */
