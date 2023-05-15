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
 * $brcm_Workfile: bchp_wbadc_ai_0.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 3/29/12 3:26p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Thu Mar 29 14:38:30 2012
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
 * $brcm_Log: /magnum/basemodules/chp/3472/rdb/a0/bchp_wbadc_ai_0.h $
 * 
 * Hydra_Software_Devel/1   3/29/12 3:26p farshidf
 * SW3461-1: add 3472 header files
 *
 ***************************************************************************/

#ifndef BCHP_WBADC_AI_0_H__
#define BCHP_WBADC_AI_0_H__

/***************************************************************************
 *WBADC_AI_0 - UFE 6-bit WDADC Analog Interface Registers for DCOs & AGCs 0
 ***************************************************************************/
#define BCHP_WBADC_AI_0_CTRL                     0x00120400 /* Control register for ADC input */
#define BCHP_WBADC_AI_0_TP                       0x00120404 /* TP control registers */
#define BCHP_WBADC_AI_0_LFSR_SEED                0x00120408 /* LFSR initial seed */
#define BCHP_WBADC_AI_0_DCOC0                    0x00120410 /* DC Canceller Control Register */
#define BCHP_WBADC_AI_0_DCI0                     0x00120414 /* DC Canceller Integrator Value */
#define BCHP_WBADC_AI_0_DCOC1                    0x00120418 /* DC Canceller Control Register */
#define BCHP_WBADC_AI_0_DCI1                     0x0012041c /* DC Canceller Integrator Value */
#define BCHP_WBADC_AI_0_DCOC2                    0x00120420 /* DC Canceller Control Register */
#define BCHP_WBADC_AI_0_DCI2                     0x00120424 /* DC Canceller Integrator Value */
#define BCHP_WBADC_AI_0_DCOC3                    0x00120428 /* DC Canceller Control Register */
#define BCHP_WBADC_AI_0_DCI3                     0x0012042c /* DC Canceller Integrator Value */
#define BCHP_WBADC_AI_0_AGC0                     0x00120440 /* AGC Control Register */
#define BCHP_WBADC_AI_0_AGCI0                    0x00120444 /* AGC Integrator Value */
#define BCHP_WBADC_AI_0_AGCL0                    0x00120448 /* AGC Leaky Integrator Value */
#define BCHP_WBADC_AI_0_AGCTHRES0                0x0012044c /* AGC Status Control Register */
#define BCHP_WBADC_AI_0_AGC1                     0x00120450 /* AGC Control Register */
#define BCHP_WBADC_AI_0_AGCI1                    0x00120454 /* AGC Integrator Value */
#define BCHP_WBADC_AI_0_AGCL1                    0x00120458 /* AGC Leaky Integrator Value */
#define BCHP_WBADC_AI_0_AGCTHRES1                0x0012045c /* AGC Status Control Register */
#define BCHP_WBADC_AI_0_AGC2                     0x00120460 /* AGC Control Register */
#define BCHP_WBADC_AI_0_AGCI2                    0x00120464 /* AGC Integrator Value */
#define BCHP_WBADC_AI_0_AGCL2                    0x00120468 /* AGC Leaky Integrator Value */
#define BCHP_WBADC_AI_0_AGCTHRES2                0x0012046c /* AGC Status Control Register */
#define BCHP_WBADC_AI_0_AGC3                     0x00120470 /* AGC Control Register */
#define BCHP_WBADC_AI_0_AGCI3                    0x00120474 /* AGC Integrator Value */
#define BCHP_WBADC_AI_0_AGCL3                    0x00120478 /* AGC Leaky Integrator Value */
#define BCHP_WBADC_AI_0_AGCTHRES3                0x0012047c /* AGC Status Control Register */

/***************************************************************************
 *CTRL - Control register for ADC input
 ***************************************************************************/
/* WBADC_AI_0 :: CTRL :: INPUT_FMT [31:31] */
#define BCHP_WBADC_AI_0_CTRL_INPUT_FMT_MASK                        0x80000000
#define BCHP_WBADC_AI_0_CTRL_INPUT_FMT_SHIFT                       31
#define BCHP_WBADC_AI_0_CTRL_INPUT_FMT_DEFAULT                     0x00000000

/* WBADC_AI_0 :: CTRL :: INPUT_EDGE [30:30] */
#define BCHP_WBADC_AI_0_CTRL_INPUT_EDGE_MASK                       0x40000000
#define BCHP_WBADC_AI_0_CTRL_INPUT_EDGE_SHIFT                      30
#define BCHP_WBADC_AI_0_CTRL_INPUT_EDGE_DEFAULT                    0x00000000

/* WBADC_AI_0 :: CTRL :: reserved0 [29:23] */
#define BCHP_WBADC_AI_0_CTRL_reserved0_MASK                        0x3f800000
#define BCHP_WBADC_AI_0_CTRL_reserved0_SHIFT                       23

/* WBADC_AI_0 :: CTRL :: INPUT270_SEL [22:20] */
#define BCHP_WBADC_AI_0_CTRL_INPUT270_SEL_MASK                     0x00700000
#define BCHP_WBADC_AI_0_CTRL_INPUT270_SEL_SHIFT                    20
#define BCHP_WBADC_AI_0_CTRL_INPUT270_SEL_DEFAULT                  0x00000000

/* WBADC_AI_0 :: CTRL :: reserved1 [19:19] */
#define BCHP_WBADC_AI_0_CTRL_reserved1_MASK                        0x00080000
#define BCHP_WBADC_AI_0_CTRL_reserved1_SHIFT                       19

/* WBADC_AI_0 :: CTRL :: INPUT180_SEL [18:16] */
#define BCHP_WBADC_AI_0_CTRL_INPUT180_SEL_MASK                     0x00070000
#define BCHP_WBADC_AI_0_CTRL_INPUT180_SEL_SHIFT                    16
#define BCHP_WBADC_AI_0_CTRL_INPUT180_SEL_DEFAULT                  0x00000000

/* WBADC_AI_0 :: CTRL :: reserved2 [15:15] */
#define BCHP_WBADC_AI_0_CTRL_reserved2_MASK                        0x00008000
#define BCHP_WBADC_AI_0_CTRL_reserved2_SHIFT                       15

/* WBADC_AI_0 :: CTRL :: INPUT90_SEL [14:12] */
#define BCHP_WBADC_AI_0_CTRL_INPUT90_SEL_MASK                      0x00007000
#define BCHP_WBADC_AI_0_CTRL_INPUT90_SEL_SHIFT                     12
#define BCHP_WBADC_AI_0_CTRL_INPUT90_SEL_DEFAULT                   0x00000000

/* WBADC_AI_0 :: CTRL :: reserved3 [11:11] */
#define BCHP_WBADC_AI_0_CTRL_reserved3_MASK                        0x00000800
#define BCHP_WBADC_AI_0_CTRL_reserved3_SHIFT                       11

/* WBADC_AI_0 :: CTRL :: INPUT0_SEL [10:08] */
#define BCHP_WBADC_AI_0_CTRL_INPUT0_SEL_MASK                       0x00000700
#define BCHP_WBADC_AI_0_CTRL_INPUT0_SEL_SHIFT                      8
#define BCHP_WBADC_AI_0_CTRL_INPUT0_SEL_DEFAULT                    0x00000000

/* WBADC_AI_0 :: CTRL :: reserved_for_eco4 [07:00] */
#define BCHP_WBADC_AI_0_CTRL_reserved_for_eco4_MASK                0x000000ff
#define BCHP_WBADC_AI_0_CTRL_reserved_for_eco4_SHIFT               0
#define BCHP_WBADC_AI_0_CTRL_reserved_for_eco4_DEFAULT             0x00000000

/***************************************************************************
 *TP - TP control registers
 ***************************************************************************/
/* WBADC_AI_0 :: TP :: reserved0 [31:31] */
#define BCHP_WBADC_AI_0_TP_reserved0_MASK                          0x80000000
#define BCHP_WBADC_AI_0_TP_reserved0_SHIFT                         31

/* WBADC_AI_0 :: TP :: TPOUT_SEL [30:29] */
#define BCHP_WBADC_AI_0_TP_TPOUT_SEL_MASK                          0x60000000
#define BCHP_WBADC_AI_0_TP_TPOUT_SEL_SHIFT                         29
#define BCHP_WBADC_AI_0_TP_TPOUT_SEL_DEFAULT                       0x00000000

/* WBADC_AI_0 :: TP :: reserved1 [28:12] */
#define BCHP_WBADC_AI_0_TP_reserved1_MASK                          0x1ffff000
#define BCHP_WBADC_AI_0_TP_reserved1_SHIFT                         12

/* WBADC_AI_0 :: TP :: DIGISUM0_INC [11:10] */
#define BCHP_WBADC_AI_0_TP_DIGISUM0_INC_MASK                       0x00000c00
#define BCHP_WBADC_AI_0_TP_DIGISUM0_INC_SHIFT                      10
#define BCHP_WBADC_AI_0_TP_DIGISUM0_INC_DEFAULT                    0x00000000

/* WBADC_AI_0 :: TP :: DIGISUM1_INC [09:08] */
#define BCHP_WBADC_AI_0_TP_DIGISUM1_INC_MASK                       0x00000300
#define BCHP_WBADC_AI_0_TP_DIGISUM1_INC_SHIFT                      8
#define BCHP_WBADC_AI_0_TP_DIGISUM1_INC_DEFAULT                    0x00000001

/* WBADC_AI_0 :: TP :: AI_CHK [07:04] */
#define BCHP_WBADC_AI_0_TP_AI_CHK_MASK                             0x000000f0
#define BCHP_WBADC_AI_0_TP_AI_CHK_SHIFT                            4
#define BCHP_WBADC_AI_0_TP_AI_CHK_DEFAULT                          0x00000000

/* WBADC_AI_0 :: TP :: reserved2 [03:02] */
#define BCHP_WBADC_AI_0_TP_reserved2_MASK                          0x0000000c
#define BCHP_WBADC_AI_0_TP_reserved2_SHIFT                         2

/* WBADC_AI_0 :: TP :: LFSR_EN [01:01] */
#define BCHP_WBADC_AI_0_TP_LFSR_EN_MASK                            0x00000002
#define BCHP_WBADC_AI_0_TP_LFSR_EN_SHIFT                           1
#define BCHP_WBADC_AI_0_TP_LFSR_EN_DEFAULT                         0x00000000

/* WBADC_AI_0 :: TP :: reserved3 [00:00] */
#define BCHP_WBADC_AI_0_TP_reserved3_MASK                          0x00000001
#define BCHP_WBADC_AI_0_TP_reserved3_SHIFT                         0

/***************************************************************************
 *LFSR_SEED - LFSR initial seed
 ***************************************************************************/
/* WBADC_AI_0 :: LFSR_SEED :: SEED [31:00] */
#define BCHP_WBADC_AI_0_LFSR_SEED_SEED_MASK                        0xffffffff
#define BCHP_WBADC_AI_0_LFSR_SEED_SEED_SHIFT                       0
#define BCHP_WBADC_AI_0_LFSR_SEED_SEED_DEFAULT                     0x00000001

/***************************************************************************
 *DCOC0 - DC Canceller Control Register
 ***************************************************************************/
/* WBADC_AI_0 :: DCOC0 :: reserved0 [31:08] */
#define BCHP_WBADC_AI_0_DCOC0_reserved0_MASK                       0xffffff00
#define BCHP_WBADC_AI_0_DCOC0_reserved0_SHIFT                      8

/* WBADC_AI_0 :: DCOC0 :: DCOC_BWC [07:04] */
#define BCHP_WBADC_AI_0_DCOC0_DCOC_BWC_MASK                        0x000000f0
#define BCHP_WBADC_AI_0_DCOC0_DCOC_BWC_SHIFT                       4
#define BCHP_WBADC_AI_0_DCOC0_DCOC_BWC_DEFAULT                     0x00000008

/* WBADC_AI_0 :: DCOC0 :: reserved1 [03:03] */
#define BCHP_WBADC_AI_0_DCOC0_reserved1_MASK                       0x00000008
#define BCHP_WBADC_AI_0_DCOC0_reserved1_SHIFT                      3

/* WBADC_AI_0 :: DCOC0 :: DCOC_BYP [02:02] */
#define BCHP_WBADC_AI_0_DCOC0_DCOC_BYP_MASK                        0x00000004
#define BCHP_WBADC_AI_0_DCOC0_DCOC_BYP_SHIFT                       2
#define BCHP_WBADC_AI_0_DCOC0_DCOC_BYP_DEFAULT                     0x00000000

/* WBADC_AI_0 :: DCOC0 :: DCOC_LPRST [01:01] */
#define BCHP_WBADC_AI_0_DCOC0_DCOC_LPRST_MASK                      0x00000002
#define BCHP_WBADC_AI_0_DCOC0_DCOC_LPRST_SHIFT                     1
#define BCHP_WBADC_AI_0_DCOC0_DCOC_LPRST_DEFAULT                   0x00000001

/* WBADC_AI_0 :: DCOC0 :: DCOC_LPFRZ [00:00] */
#define BCHP_WBADC_AI_0_DCOC0_DCOC_LPFRZ_MASK                      0x00000001
#define BCHP_WBADC_AI_0_DCOC0_DCOC_LPFRZ_SHIFT                     0
#define BCHP_WBADC_AI_0_DCOC0_DCOC_LPFRZ_DEFAULT                   0x00000001

/***************************************************************************
 *DCI0 - DC Canceller Integrator Value
 ***************************************************************************/
/* WBADC_AI_0 :: DCI0 :: DCVAL [31:00] */
#define BCHP_WBADC_AI_0_DCI0_DCVAL_MASK                            0xffffffff
#define BCHP_WBADC_AI_0_DCI0_DCVAL_SHIFT                           0
#define BCHP_WBADC_AI_0_DCI0_DCVAL_DEFAULT                         0x00000000

/***************************************************************************
 *DCOC1 - DC Canceller Control Register
 ***************************************************************************/
/* WBADC_AI_0 :: DCOC1 :: reserved0 [31:08] */
#define BCHP_WBADC_AI_0_DCOC1_reserved0_MASK                       0xffffff00
#define BCHP_WBADC_AI_0_DCOC1_reserved0_SHIFT                      8

/* WBADC_AI_0 :: DCOC1 :: DCOC_BWC [07:04] */
#define BCHP_WBADC_AI_0_DCOC1_DCOC_BWC_MASK                        0x000000f0
#define BCHP_WBADC_AI_0_DCOC1_DCOC_BWC_SHIFT                       4
#define BCHP_WBADC_AI_0_DCOC1_DCOC_BWC_DEFAULT                     0x00000008

/* WBADC_AI_0 :: DCOC1 :: reserved1 [03:03] */
#define BCHP_WBADC_AI_0_DCOC1_reserved1_MASK                       0x00000008
#define BCHP_WBADC_AI_0_DCOC1_reserved1_SHIFT                      3

/* WBADC_AI_0 :: DCOC1 :: DCOC_BYP [02:02] */
#define BCHP_WBADC_AI_0_DCOC1_DCOC_BYP_MASK                        0x00000004
#define BCHP_WBADC_AI_0_DCOC1_DCOC_BYP_SHIFT                       2
#define BCHP_WBADC_AI_0_DCOC1_DCOC_BYP_DEFAULT                     0x00000000

/* WBADC_AI_0 :: DCOC1 :: DCOC_LPRST [01:01] */
#define BCHP_WBADC_AI_0_DCOC1_DCOC_LPRST_MASK                      0x00000002
#define BCHP_WBADC_AI_0_DCOC1_DCOC_LPRST_SHIFT                     1
#define BCHP_WBADC_AI_0_DCOC1_DCOC_LPRST_DEFAULT                   0x00000001

/* WBADC_AI_0 :: DCOC1 :: DCOC_LPFRZ [00:00] */
#define BCHP_WBADC_AI_0_DCOC1_DCOC_LPFRZ_MASK                      0x00000001
#define BCHP_WBADC_AI_0_DCOC1_DCOC_LPFRZ_SHIFT                     0
#define BCHP_WBADC_AI_0_DCOC1_DCOC_LPFRZ_DEFAULT                   0x00000001

/***************************************************************************
 *DCI1 - DC Canceller Integrator Value
 ***************************************************************************/
/* WBADC_AI_0 :: DCI1 :: DCVAL [31:00] */
#define BCHP_WBADC_AI_0_DCI1_DCVAL_MASK                            0xffffffff
#define BCHP_WBADC_AI_0_DCI1_DCVAL_SHIFT                           0
#define BCHP_WBADC_AI_0_DCI1_DCVAL_DEFAULT                         0x00000000

/***************************************************************************
 *DCOC2 - DC Canceller Control Register
 ***************************************************************************/
/* WBADC_AI_0 :: DCOC2 :: reserved0 [31:08] */
#define BCHP_WBADC_AI_0_DCOC2_reserved0_MASK                       0xffffff00
#define BCHP_WBADC_AI_0_DCOC2_reserved0_SHIFT                      8

/* WBADC_AI_0 :: DCOC2 :: DCOC_BWC [07:04] */
#define BCHP_WBADC_AI_0_DCOC2_DCOC_BWC_MASK                        0x000000f0
#define BCHP_WBADC_AI_0_DCOC2_DCOC_BWC_SHIFT                       4
#define BCHP_WBADC_AI_0_DCOC2_DCOC_BWC_DEFAULT                     0x00000008

/* WBADC_AI_0 :: DCOC2 :: reserved1 [03:03] */
#define BCHP_WBADC_AI_0_DCOC2_reserved1_MASK                       0x00000008
#define BCHP_WBADC_AI_0_DCOC2_reserved1_SHIFT                      3

/* WBADC_AI_0 :: DCOC2 :: DCOC_BYP [02:02] */
#define BCHP_WBADC_AI_0_DCOC2_DCOC_BYP_MASK                        0x00000004
#define BCHP_WBADC_AI_0_DCOC2_DCOC_BYP_SHIFT                       2
#define BCHP_WBADC_AI_0_DCOC2_DCOC_BYP_DEFAULT                     0x00000000

/* WBADC_AI_0 :: DCOC2 :: DCOC_LPRST [01:01] */
#define BCHP_WBADC_AI_0_DCOC2_DCOC_LPRST_MASK                      0x00000002
#define BCHP_WBADC_AI_0_DCOC2_DCOC_LPRST_SHIFT                     1
#define BCHP_WBADC_AI_0_DCOC2_DCOC_LPRST_DEFAULT                   0x00000001

/* WBADC_AI_0 :: DCOC2 :: DCOC_LPFRZ [00:00] */
#define BCHP_WBADC_AI_0_DCOC2_DCOC_LPFRZ_MASK                      0x00000001
#define BCHP_WBADC_AI_0_DCOC2_DCOC_LPFRZ_SHIFT                     0
#define BCHP_WBADC_AI_0_DCOC2_DCOC_LPFRZ_DEFAULT                   0x00000001

/***************************************************************************
 *DCI2 - DC Canceller Integrator Value
 ***************************************************************************/
/* WBADC_AI_0 :: DCI2 :: DCVAL [31:00] */
#define BCHP_WBADC_AI_0_DCI2_DCVAL_MASK                            0xffffffff
#define BCHP_WBADC_AI_0_DCI2_DCVAL_SHIFT                           0
#define BCHP_WBADC_AI_0_DCI2_DCVAL_DEFAULT                         0x00000000

/***************************************************************************
 *DCOC3 - DC Canceller Control Register
 ***************************************************************************/
/* WBADC_AI_0 :: DCOC3 :: reserved0 [31:08] */
#define BCHP_WBADC_AI_0_DCOC3_reserved0_MASK                       0xffffff00
#define BCHP_WBADC_AI_0_DCOC3_reserved0_SHIFT                      8

/* WBADC_AI_0 :: DCOC3 :: DCOC_BWC [07:04] */
#define BCHP_WBADC_AI_0_DCOC3_DCOC_BWC_MASK                        0x000000f0
#define BCHP_WBADC_AI_0_DCOC3_DCOC_BWC_SHIFT                       4
#define BCHP_WBADC_AI_0_DCOC3_DCOC_BWC_DEFAULT                     0x00000008

/* WBADC_AI_0 :: DCOC3 :: reserved1 [03:03] */
#define BCHP_WBADC_AI_0_DCOC3_reserved1_MASK                       0x00000008
#define BCHP_WBADC_AI_0_DCOC3_reserved1_SHIFT                      3

/* WBADC_AI_0 :: DCOC3 :: DCOC_BYP [02:02] */
#define BCHP_WBADC_AI_0_DCOC3_DCOC_BYP_MASK                        0x00000004
#define BCHP_WBADC_AI_0_DCOC3_DCOC_BYP_SHIFT                       2
#define BCHP_WBADC_AI_0_DCOC3_DCOC_BYP_DEFAULT                     0x00000000

/* WBADC_AI_0 :: DCOC3 :: DCOC_LPRST [01:01] */
#define BCHP_WBADC_AI_0_DCOC3_DCOC_LPRST_MASK                      0x00000002
#define BCHP_WBADC_AI_0_DCOC3_DCOC_LPRST_SHIFT                     1
#define BCHP_WBADC_AI_0_DCOC3_DCOC_LPRST_DEFAULT                   0x00000001

/* WBADC_AI_0 :: DCOC3 :: DCOC_LPFRZ [00:00] */
#define BCHP_WBADC_AI_0_DCOC3_DCOC_LPFRZ_MASK                      0x00000001
#define BCHP_WBADC_AI_0_DCOC3_DCOC_LPFRZ_SHIFT                     0
#define BCHP_WBADC_AI_0_DCOC3_DCOC_LPFRZ_DEFAULT                   0x00000001

/***************************************************************************
 *DCI3 - DC Canceller Integrator Value
 ***************************************************************************/
/* WBADC_AI_0 :: DCI3 :: DCVAL [31:00] */
#define BCHP_WBADC_AI_0_DCI3_DCVAL_MASK                            0xffffffff
#define BCHP_WBADC_AI_0_DCI3_DCVAL_SHIFT                           0
#define BCHP_WBADC_AI_0_DCI3_DCVAL_DEFAULT                         0x00000000

/***************************************************************************
 *AGC0 - AGC Control Register
 ***************************************************************************/
/* WBADC_AI_0 :: AGC0 :: reserved0 [31:12] */
#define BCHP_WBADC_AI_0_AGC0_reserved0_MASK                        0xfffff000
#define BCHP_WBADC_AI_0_AGC0_reserved0_SHIFT                       12

/* WBADC_AI_0 :: AGC0 :: K [11:08] */
#define BCHP_WBADC_AI_0_AGC0_K_MASK                                0x00000f00
#define BCHP_WBADC_AI_0_AGC0_K_SHIFT                               8
#define BCHP_WBADC_AI_0_AGC0_K_DEFAULT                             0x0000000f

/* WBADC_AI_0 :: AGC0 :: beta [07:04] */
#define BCHP_WBADC_AI_0_AGC0_beta_MASK                             0x000000f0
#define BCHP_WBADC_AI_0_AGC0_beta_SHIFT                            4
#define BCHP_WBADC_AI_0_AGC0_beta_DEFAULT                          0x00000004

/* WBADC_AI_0 :: AGC0 :: reserved1 [03:03] */
#define BCHP_WBADC_AI_0_AGC0_reserved1_MASK                        0x00000008
#define BCHP_WBADC_AI_0_AGC0_reserved1_SHIFT                       3

/* WBADC_AI_0 :: AGC0 :: BYP [02:02] */
#define BCHP_WBADC_AI_0_AGC0_BYP_MASK                              0x00000004
#define BCHP_WBADC_AI_0_AGC0_BYP_SHIFT                             2
#define BCHP_WBADC_AI_0_AGC0_BYP_DEFAULT                           0x00000000

/* WBADC_AI_0 :: AGC0 :: AGCFRZ [01:01] */
#define BCHP_WBADC_AI_0_AGC0_AGCFRZ_MASK                           0x00000002
#define BCHP_WBADC_AI_0_AGC0_AGCFRZ_SHIFT                          1
#define BCHP_WBADC_AI_0_AGC0_AGCFRZ_DEFAULT                        0x00000001

/* WBADC_AI_0 :: AGC0 :: AGCRST [00:00] */
#define BCHP_WBADC_AI_0_AGC0_AGCRST_MASK                           0x00000001
#define BCHP_WBADC_AI_0_AGC0_AGCRST_SHIFT                          0
#define BCHP_WBADC_AI_0_AGC0_AGCRST_DEFAULT                        0x00000001

/***************************************************************************
 *AGCI0 - AGC Integrator Value
 ***************************************************************************/
/* WBADC_AI_0 :: AGCI0 :: AGCVAL [31:00] */
#define BCHP_WBADC_AI_0_AGCI0_AGCVAL_MASK                          0xffffffff
#define BCHP_WBADC_AI_0_AGCI0_AGCVAL_SHIFT                         0
#define BCHP_WBADC_AI_0_AGCI0_AGCVAL_DEFAULT                       0x40000000

/***************************************************************************
 *AGCL0 - AGC Leaky Integrator Value
 ***************************************************************************/
/* WBADC_AI_0 :: AGCL0 :: AGCLVAL [31:00] */
#define BCHP_WBADC_AI_0_AGCL0_AGCLVAL_MASK                         0xffffffff
#define BCHP_WBADC_AI_0_AGCL0_AGCLVAL_SHIFT                        0
#define BCHP_WBADC_AI_0_AGCL0_AGCLVAL_DEFAULT                      0x00000000

/***************************************************************************
 *AGCTHRES0 - AGC Status Control Register
 ***************************************************************************/
/* WBADC_AI_0 :: AGCTHRES0 :: THRES_USE [31:31] */
#define BCHP_WBADC_AI_0_AGCTHRES0_THRES_USE_MASK                   0x80000000
#define BCHP_WBADC_AI_0_AGCTHRES0_THRES_USE_SHIFT                  31
#define BCHP_WBADC_AI_0_AGCTHRES0_THRES_USE_DEFAULT                0x00000000

/* WBADC_AI_0 :: AGCTHRES0 :: reserved0 [30:15] */
#define BCHP_WBADC_AI_0_AGCTHRES0_reserved0_MASK                   0x7fff8000
#define BCHP_WBADC_AI_0_AGCTHRES0_reserved0_SHIFT                  15

/* WBADC_AI_0 :: AGCTHRES0 :: THRES [14:00] */
#define BCHP_WBADC_AI_0_AGCTHRES0_THRES_MASK                       0x00007fff
#define BCHP_WBADC_AI_0_AGCTHRES0_THRES_SHIFT                      0
#define BCHP_WBADC_AI_0_AGCTHRES0_THRES_DEFAULT                    0x00000000

/***************************************************************************
 *AGC1 - AGC Control Register
 ***************************************************************************/
/* WBADC_AI_0 :: AGC1 :: reserved0 [31:12] */
#define BCHP_WBADC_AI_0_AGC1_reserved0_MASK                        0xfffff000
#define BCHP_WBADC_AI_0_AGC1_reserved0_SHIFT                       12

/* WBADC_AI_0 :: AGC1 :: K [11:08] */
#define BCHP_WBADC_AI_0_AGC1_K_MASK                                0x00000f00
#define BCHP_WBADC_AI_0_AGC1_K_SHIFT                               8
#define BCHP_WBADC_AI_0_AGC1_K_DEFAULT                             0x0000000f

/* WBADC_AI_0 :: AGC1 :: beta [07:04] */
#define BCHP_WBADC_AI_0_AGC1_beta_MASK                             0x000000f0
#define BCHP_WBADC_AI_0_AGC1_beta_SHIFT                            4
#define BCHP_WBADC_AI_0_AGC1_beta_DEFAULT                          0x00000004

/* WBADC_AI_0 :: AGC1 :: reserved1 [03:03] */
#define BCHP_WBADC_AI_0_AGC1_reserved1_MASK                        0x00000008
#define BCHP_WBADC_AI_0_AGC1_reserved1_SHIFT                       3

/* WBADC_AI_0 :: AGC1 :: BYP [02:02] */
#define BCHP_WBADC_AI_0_AGC1_BYP_MASK                              0x00000004
#define BCHP_WBADC_AI_0_AGC1_BYP_SHIFT                             2
#define BCHP_WBADC_AI_0_AGC1_BYP_DEFAULT                           0x00000000

/* WBADC_AI_0 :: AGC1 :: AGCFRZ [01:01] */
#define BCHP_WBADC_AI_0_AGC1_AGCFRZ_MASK                           0x00000002
#define BCHP_WBADC_AI_0_AGC1_AGCFRZ_SHIFT                          1
#define BCHP_WBADC_AI_0_AGC1_AGCFRZ_DEFAULT                        0x00000001

/* WBADC_AI_0 :: AGC1 :: AGCRST [00:00] */
#define BCHP_WBADC_AI_0_AGC1_AGCRST_MASK                           0x00000001
#define BCHP_WBADC_AI_0_AGC1_AGCRST_SHIFT                          0
#define BCHP_WBADC_AI_0_AGC1_AGCRST_DEFAULT                        0x00000001

/***************************************************************************
 *AGCI1 - AGC Integrator Value
 ***************************************************************************/
/* WBADC_AI_0 :: AGCI1 :: AGCVAL [31:00] */
#define BCHP_WBADC_AI_0_AGCI1_AGCVAL_MASK                          0xffffffff
#define BCHP_WBADC_AI_0_AGCI1_AGCVAL_SHIFT                         0
#define BCHP_WBADC_AI_0_AGCI1_AGCVAL_DEFAULT                       0x40000000

/***************************************************************************
 *AGCL1 - AGC Leaky Integrator Value
 ***************************************************************************/
/* WBADC_AI_0 :: AGCL1 :: AGCLVAL [31:00] */
#define BCHP_WBADC_AI_0_AGCL1_AGCLVAL_MASK                         0xffffffff
#define BCHP_WBADC_AI_0_AGCL1_AGCLVAL_SHIFT                        0
#define BCHP_WBADC_AI_0_AGCL1_AGCLVAL_DEFAULT                      0x00000000

/***************************************************************************
 *AGCTHRES1 - AGC Status Control Register
 ***************************************************************************/
/* WBADC_AI_0 :: AGCTHRES1 :: THRES_USE [31:31] */
#define BCHP_WBADC_AI_0_AGCTHRES1_THRES_USE_MASK                   0x80000000
#define BCHP_WBADC_AI_0_AGCTHRES1_THRES_USE_SHIFT                  31
#define BCHP_WBADC_AI_0_AGCTHRES1_THRES_USE_DEFAULT                0x00000000

/* WBADC_AI_0 :: AGCTHRES1 :: reserved0 [30:15] */
#define BCHP_WBADC_AI_0_AGCTHRES1_reserved0_MASK                   0x7fff8000
#define BCHP_WBADC_AI_0_AGCTHRES1_reserved0_SHIFT                  15

/* WBADC_AI_0 :: AGCTHRES1 :: THRES [14:00] */
#define BCHP_WBADC_AI_0_AGCTHRES1_THRES_MASK                       0x00007fff
#define BCHP_WBADC_AI_0_AGCTHRES1_THRES_SHIFT                      0
#define BCHP_WBADC_AI_0_AGCTHRES1_THRES_DEFAULT                    0x00000000

/***************************************************************************
 *AGC2 - AGC Control Register
 ***************************************************************************/
/* WBADC_AI_0 :: AGC2 :: reserved0 [31:12] */
#define BCHP_WBADC_AI_0_AGC2_reserved0_MASK                        0xfffff000
#define BCHP_WBADC_AI_0_AGC2_reserved0_SHIFT                       12

/* WBADC_AI_0 :: AGC2 :: K [11:08] */
#define BCHP_WBADC_AI_0_AGC2_K_MASK                                0x00000f00
#define BCHP_WBADC_AI_0_AGC2_K_SHIFT                               8
#define BCHP_WBADC_AI_0_AGC2_K_DEFAULT                             0x0000000f

/* WBADC_AI_0 :: AGC2 :: beta [07:04] */
#define BCHP_WBADC_AI_0_AGC2_beta_MASK                             0x000000f0
#define BCHP_WBADC_AI_0_AGC2_beta_SHIFT                            4
#define BCHP_WBADC_AI_0_AGC2_beta_DEFAULT                          0x00000004

/* WBADC_AI_0 :: AGC2 :: reserved1 [03:03] */
#define BCHP_WBADC_AI_0_AGC2_reserved1_MASK                        0x00000008
#define BCHP_WBADC_AI_0_AGC2_reserved1_SHIFT                       3

/* WBADC_AI_0 :: AGC2 :: BYP [02:02] */
#define BCHP_WBADC_AI_0_AGC2_BYP_MASK                              0x00000004
#define BCHP_WBADC_AI_0_AGC2_BYP_SHIFT                             2
#define BCHP_WBADC_AI_0_AGC2_BYP_DEFAULT                           0x00000000

/* WBADC_AI_0 :: AGC2 :: AGCFRZ [01:01] */
#define BCHP_WBADC_AI_0_AGC2_AGCFRZ_MASK                           0x00000002
#define BCHP_WBADC_AI_0_AGC2_AGCFRZ_SHIFT                          1
#define BCHP_WBADC_AI_0_AGC2_AGCFRZ_DEFAULT                        0x00000001

/* WBADC_AI_0 :: AGC2 :: AGCRST [00:00] */
#define BCHP_WBADC_AI_0_AGC2_AGCRST_MASK                           0x00000001
#define BCHP_WBADC_AI_0_AGC2_AGCRST_SHIFT                          0
#define BCHP_WBADC_AI_0_AGC2_AGCRST_DEFAULT                        0x00000001

/***************************************************************************
 *AGCI2 - AGC Integrator Value
 ***************************************************************************/
/* WBADC_AI_0 :: AGCI2 :: AGCVAL [31:00] */
#define BCHP_WBADC_AI_0_AGCI2_AGCVAL_MASK                          0xffffffff
#define BCHP_WBADC_AI_0_AGCI2_AGCVAL_SHIFT                         0
#define BCHP_WBADC_AI_0_AGCI2_AGCVAL_DEFAULT                       0x40000000

/***************************************************************************
 *AGCL2 - AGC Leaky Integrator Value
 ***************************************************************************/
/* WBADC_AI_0 :: AGCL2 :: AGCLVAL [31:00] */
#define BCHP_WBADC_AI_0_AGCL2_AGCLVAL_MASK                         0xffffffff
#define BCHP_WBADC_AI_0_AGCL2_AGCLVAL_SHIFT                        0
#define BCHP_WBADC_AI_0_AGCL2_AGCLVAL_DEFAULT                      0x00000000

/***************************************************************************
 *AGCTHRES2 - AGC Status Control Register
 ***************************************************************************/
/* WBADC_AI_0 :: AGCTHRES2 :: THRES_USE [31:31] */
#define BCHP_WBADC_AI_0_AGCTHRES2_THRES_USE_MASK                   0x80000000
#define BCHP_WBADC_AI_0_AGCTHRES2_THRES_USE_SHIFT                  31
#define BCHP_WBADC_AI_0_AGCTHRES2_THRES_USE_DEFAULT                0x00000000

/* WBADC_AI_0 :: AGCTHRES2 :: reserved0 [30:15] */
#define BCHP_WBADC_AI_0_AGCTHRES2_reserved0_MASK                   0x7fff8000
#define BCHP_WBADC_AI_0_AGCTHRES2_reserved0_SHIFT                  15

/* WBADC_AI_0 :: AGCTHRES2 :: THRES [14:00] */
#define BCHP_WBADC_AI_0_AGCTHRES2_THRES_MASK                       0x00007fff
#define BCHP_WBADC_AI_0_AGCTHRES2_THRES_SHIFT                      0
#define BCHP_WBADC_AI_0_AGCTHRES2_THRES_DEFAULT                    0x00000000

/***************************************************************************
 *AGC3 - AGC Control Register
 ***************************************************************************/
/* WBADC_AI_0 :: AGC3 :: reserved0 [31:12] */
#define BCHP_WBADC_AI_0_AGC3_reserved0_MASK                        0xfffff000
#define BCHP_WBADC_AI_0_AGC3_reserved0_SHIFT                       12

/* WBADC_AI_0 :: AGC3 :: K [11:08] */
#define BCHP_WBADC_AI_0_AGC3_K_MASK                                0x00000f00
#define BCHP_WBADC_AI_0_AGC3_K_SHIFT                               8
#define BCHP_WBADC_AI_0_AGC3_K_DEFAULT                             0x0000000f

/* WBADC_AI_0 :: AGC3 :: beta [07:04] */
#define BCHP_WBADC_AI_0_AGC3_beta_MASK                             0x000000f0
#define BCHP_WBADC_AI_0_AGC3_beta_SHIFT                            4
#define BCHP_WBADC_AI_0_AGC3_beta_DEFAULT                          0x00000004

/* WBADC_AI_0 :: AGC3 :: reserved1 [03:03] */
#define BCHP_WBADC_AI_0_AGC3_reserved1_MASK                        0x00000008
#define BCHP_WBADC_AI_0_AGC3_reserved1_SHIFT                       3

/* WBADC_AI_0 :: AGC3 :: BYP [02:02] */
#define BCHP_WBADC_AI_0_AGC3_BYP_MASK                              0x00000004
#define BCHP_WBADC_AI_0_AGC3_BYP_SHIFT                             2
#define BCHP_WBADC_AI_0_AGC3_BYP_DEFAULT                           0x00000000

/* WBADC_AI_0 :: AGC3 :: AGCFRZ [01:01] */
#define BCHP_WBADC_AI_0_AGC3_AGCFRZ_MASK                           0x00000002
#define BCHP_WBADC_AI_0_AGC3_AGCFRZ_SHIFT                          1
#define BCHP_WBADC_AI_0_AGC3_AGCFRZ_DEFAULT                        0x00000001

/* WBADC_AI_0 :: AGC3 :: AGCRST [00:00] */
#define BCHP_WBADC_AI_0_AGC3_AGCRST_MASK                           0x00000001
#define BCHP_WBADC_AI_0_AGC3_AGCRST_SHIFT                          0
#define BCHP_WBADC_AI_0_AGC3_AGCRST_DEFAULT                        0x00000001

/***************************************************************************
 *AGCI3 - AGC Integrator Value
 ***************************************************************************/
/* WBADC_AI_0 :: AGCI3 :: AGCVAL [31:00] */
#define BCHP_WBADC_AI_0_AGCI3_AGCVAL_MASK                          0xffffffff
#define BCHP_WBADC_AI_0_AGCI3_AGCVAL_SHIFT                         0
#define BCHP_WBADC_AI_0_AGCI3_AGCVAL_DEFAULT                       0x40000000

/***************************************************************************
 *AGCL3 - AGC Leaky Integrator Value
 ***************************************************************************/
/* WBADC_AI_0 :: AGCL3 :: AGCLVAL [31:00] */
#define BCHP_WBADC_AI_0_AGCL3_AGCLVAL_MASK                         0xffffffff
#define BCHP_WBADC_AI_0_AGCL3_AGCLVAL_SHIFT                        0
#define BCHP_WBADC_AI_0_AGCL3_AGCLVAL_DEFAULT                      0x00000000

/***************************************************************************
 *AGCTHRES3 - AGC Status Control Register
 ***************************************************************************/
/* WBADC_AI_0 :: AGCTHRES3 :: THRES_USE [31:31] */
#define BCHP_WBADC_AI_0_AGCTHRES3_THRES_USE_MASK                   0x80000000
#define BCHP_WBADC_AI_0_AGCTHRES3_THRES_USE_SHIFT                  31
#define BCHP_WBADC_AI_0_AGCTHRES3_THRES_USE_DEFAULT                0x00000000

/* WBADC_AI_0 :: AGCTHRES3 :: reserved0 [30:15] */
#define BCHP_WBADC_AI_0_AGCTHRES3_reserved0_MASK                   0x7fff8000
#define BCHP_WBADC_AI_0_AGCTHRES3_reserved0_SHIFT                  15

/* WBADC_AI_0 :: AGCTHRES3 :: THRES [14:00] */
#define BCHP_WBADC_AI_0_AGCTHRES3_THRES_MASK                       0x00007fff
#define BCHP_WBADC_AI_0_AGCTHRES3_THRES_SHIFT                      0
#define BCHP_WBADC_AI_0_AGCTHRES3_THRES_DEFAULT                    0x00000000

#endif /* #ifndef BCHP_WBADC_AI_0_H__ */

/* End of File */