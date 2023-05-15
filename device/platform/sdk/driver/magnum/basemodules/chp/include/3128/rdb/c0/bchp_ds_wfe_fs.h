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
 * $brcm_Workfile: bchp_ds_wfe_fs.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 10/6/11 11:43a $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Thu Oct  6 10:25:02 2011
 *                 MD5 Checksum         8f37dfc69866893136bc4603c1bedf8c
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/3128/rdb/c0/bchp_ds_wfe_fs.h $
 * 
 * Hydra_Software_Devel/1   10/6/11 11:43a farshidf
 * SW3128-1: add C0 RDB files
 *
 ***************************************************************************/

#ifndef BCHP_DS_WFE_FS_H__
#define BCHP_DS_WFE_FS_H__

/***************************************************************************
 *DS_WFE_FS - WFE Sample Clock Domain Registers
 ***************************************************************************/
#define BCHP_DS_WFE_FS_DPM_ID                    0x00091000 /* DPM source channel bin ID */
#define BCHP_DS_WFE_FS_DPM                       0x00091004 /* DPM register for control signals */
#define BCHP_DS_WFE_FS_DPM_DCI                   0x00091008 /* DPM DC Canceller I Integrator Value */
#define BCHP_DS_WFE_FS_DPM_DCQ                   0x0009100c /* DPM DC Canceller Q Integrator Value */
#define BCHP_DS_WFE_FS_DPMK2                     0x00091010 /* DPM register for control signals */
#define BCHP_DS_WFE_FS_DPM_DCK2I                 0x00091014 /* DPM DC Canceller I Integrator Value */
#define BCHP_DS_WFE_FS_DPM_DCK2Q                 0x00091018 /* DPM DC Canceller Q Integrator Value */
#define BCHP_DS_WFE_FS_LFSR                      0x00091024 /* Spare register */
#define BCHP_DS_WFE_FS_PPF_CTRL                  0x0009102c /* PPF control register */
#define BCHP_DS_WFE_FS_OBADC_CTRL                0x00091030 /* OBADC Control register */
#define BCHP_DS_WFE_FS_OBADC_FCW                 0x00091034 /* OBADC FCW control register */
#define BCHP_DS_WFE_FS_OBADC_STATUS              0x00091038 /* OBADC status register */
#define BCHP_DS_WFE_FS_DAC_CTRL                  0x0009103c /* DAC control register */
#define BCHP_DS_WFE_FS_DAC_FCW0                  0x00091040 /* DAC FCW0 register */
#define BCHP_DS_WFE_FS_DAC_FCW1                  0x00091044 /* DAC FCW1 register */
#define BCHP_DS_WFE_FS_DAC_TEST                  0x00091048 /* DAC TEST register */

/***************************************************************************
 *DPM_ID - DPM source channel bin ID
 ***************************************************************************/
/* DS_WFE_FS :: DPM_ID :: reserved0 [31:04] */
#define BCHP_DS_WFE_FS_DPM_ID_reserved0_MASK                       0xfffffff0
#define BCHP_DS_WFE_FS_DPM_ID_reserved0_SHIFT                      4

/* DS_WFE_FS :: DPM_ID :: DPM_CHAN_SEL [03:00] */
#define BCHP_DS_WFE_FS_DPM_ID_DPM_CHAN_SEL_MASK                    0x0000000f
#define BCHP_DS_WFE_FS_DPM_ID_DPM_CHAN_SEL_SHIFT                   0
#define BCHP_DS_WFE_FS_DPM_ID_DPM_CHAN_SEL_DEFAULT                 0

/***************************************************************************
 *DPM - DPM register for control signals
 ***************************************************************************/
/* DS_WFE_FS :: DPM :: DPMDC_FRZ [31:31] */
#define BCHP_DS_WFE_FS_DPM_DPMDC_FRZ_MASK                          0x80000000
#define BCHP_DS_WFE_FS_DPM_DPMDC_FRZ_SHIFT                         31
#define BCHP_DS_WFE_FS_DPM_DPMDC_FRZ_DEFAULT                       1

/* DS_WFE_FS :: DPM :: reserved0 [30:30] */
#define BCHP_DS_WFE_FS_DPM_reserved0_MASK                          0x40000000
#define BCHP_DS_WFE_FS_DPM_reserved0_SHIFT                         30

/* DS_WFE_FS :: DPM :: DPMDCBYP [29:29] */
#define BCHP_DS_WFE_FS_DPM_DPMDCBYP_MASK                           0x20000000
#define BCHP_DS_WFE_FS_DPM_DPMDCBYP_SHIFT                          29
#define BCHP_DS_WFE_FS_DPM_DPMDCBYP_DEFAULT                        1

/* DS_WFE_FS :: DPM :: DPMCMULTBYP [28:28] */
#define BCHP_DS_WFE_FS_DPM_DPMCMULTBYP_MASK                        0x10000000
#define BCHP_DS_WFE_FS_DPM_DPMCMULTBYP_SHIFT                       28
#define BCHP_DS_WFE_FS_DPM_DPMCMULTBYP_DEFAULT                     0

/* DS_WFE_FS :: DPM :: DPMFCW [27:04] */
#define BCHP_DS_WFE_FS_DPM_DPMFCW_MASK                             0x0ffffff0
#define BCHP_DS_WFE_FS_DPM_DPMFCW_SHIFT                            4
#define BCHP_DS_WFE_FS_DPM_DPMFCW_DEFAULT                          0

/* DS_WFE_FS :: DPM :: DPMDCRST [03:03] */
#define BCHP_DS_WFE_FS_DPM_DPMDCRST_MASK                           0x00000008
#define BCHP_DS_WFE_FS_DPM_DPMDCRST_SHIFT                          3
#define BCHP_DS_WFE_FS_DPM_DPMDCRST_DEFAULT                        0

/* DS_WFE_FS :: DPM :: DPMNOTCHBWC [02:00] */
#define BCHP_DS_WFE_FS_DPM_DPMNOTCHBWC_MASK                        0x00000007
#define BCHP_DS_WFE_FS_DPM_DPMNOTCHBWC_SHIFT                       0
#define BCHP_DS_WFE_FS_DPM_DPMNOTCHBWC_DEFAULT                     0

/***************************************************************************
 *DPM_DCI - DPM DC Canceller I Integrator Value
 ***************************************************************************/
/* DS_WFE_FS :: DPM_DCI :: DCIVAL [31:00] */
#define BCHP_DS_WFE_FS_DPM_DCI_DCIVAL_MASK                         0xffffffff
#define BCHP_DS_WFE_FS_DPM_DCI_DCIVAL_SHIFT                        0
#define BCHP_DS_WFE_FS_DPM_DCI_DCIVAL_DEFAULT                      0

/***************************************************************************
 *DPM_DCQ - DPM DC Canceller Q Integrator Value
 ***************************************************************************/
/* DS_WFE_FS :: DPM_DCQ :: DCQVAL [31:00] */
#define BCHP_DS_WFE_FS_DPM_DCQ_DCQVAL_MASK                         0xffffffff
#define BCHP_DS_WFE_FS_DPM_DCQ_DCQVAL_SHIFT                        0
#define BCHP_DS_WFE_FS_DPM_DCQ_DCQVAL_DEFAULT                      0

/***************************************************************************
 *DPMK2 - DPM register for control signals
 ***************************************************************************/
/* DS_WFE_FS :: DPMK2 :: reserved0 [31:04] */
#define BCHP_DS_WFE_FS_DPMK2_reserved0_MASK                        0xfffffff0
#define BCHP_DS_WFE_FS_DPMK2_reserved0_SHIFT                       4

/* DS_WFE_FS :: DPMK2 :: DPMK2DCFRZ [03:03] */
#define BCHP_DS_WFE_FS_DPMK2_DPMK2DCFRZ_MASK                       0x00000008
#define BCHP_DS_WFE_FS_DPMK2_DPMK2DCFRZ_SHIFT                      3
#define BCHP_DS_WFE_FS_DPMK2_DPMK2DCFRZ_DEFAULT                    1

/* DS_WFE_FS :: DPMK2 :: DPMK2DCRST [02:02] */
#define BCHP_DS_WFE_FS_DPMK2_DPMK2DCRST_MASK                       0x00000004
#define BCHP_DS_WFE_FS_DPMK2_DPMK2DCRST_SHIFT                      2
#define BCHP_DS_WFE_FS_DPMK2_DPMK2DCRST_DEFAULT                    0

/* DS_WFE_FS :: DPMK2 :: DPMK2NOTCHBWC [01:00] */
#define BCHP_DS_WFE_FS_DPMK2_DPMK2NOTCHBWC_MASK                    0x00000003
#define BCHP_DS_WFE_FS_DPMK2_DPMK2NOTCHBWC_SHIFT                   0
#define BCHP_DS_WFE_FS_DPMK2_DPMK2NOTCHBWC_DEFAULT                 0

/***************************************************************************
 *DPM_DCK2I - DPM DC Canceller I Integrator Value
 ***************************************************************************/
/* DS_WFE_FS :: DPM_DCK2I :: DCIVAL [31:00] */
#define BCHP_DS_WFE_FS_DPM_DCK2I_DCIVAL_MASK                       0xffffffff
#define BCHP_DS_WFE_FS_DPM_DCK2I_DCIVAL_SHIFT                      0
#define BCHP_DS_WFE_FS_DPM_DCK2I_DCIVAL_DEFAULT                    0

/***************************************************************************
 *DPM_DCK2Q - DPM DC Canceller Q Integrator Value
 ***************************************************************************/
/* DS_WFE_FS :: DPM_DCK2Q :: DCQVAL [31:00] */
#define BCHP_DS_WFE_FS_DPM_DCK2Q_DCQVAL_MASK                       0xffffffff
#define BCHP_DS_WFE_FS_DPM_DCK2Q_DCQVAL_SHIFT                      0
#define BCHP_DS_WFE_FS_DPM_DCK2Q_DCQVAL_DEFAULT                    0

/***************************************************************************
 *LFSR - Spare register
 ***************************************************************************/
/* DS_WFE_FS :: LFSR :: reserved_for_eco0 [31:02] */
#define BCHP_DS_WFE_FS_LFSR_reserved_for_eco0_MASK                 0xfffffffc
#define BCHP_DS_WFE_FS_LFSR_reserved_for_eco0_SHIFT                2
#define BCHP_DS_WFE_FS_LFSR_reserved_for_eco0_DEFAULT              0

/* DS_WFE_FS :: LFSR :: LFSR_EN [01:01] */
#define BCHP_DS_WFE_FS_LFSR_LFSR_EN_MASK                           0x00000002
#define BCHP_DS_WFE_FS_LFSR_LFSR_EN_SHIFT                          1
#define BCHP_DS_WFE_FS_LFSR_LFSR_EN_DEFAULT                        0

/* DS_WFE_FS :: LFSR :: LFSR_SEL [00:00] */
#define BCHP_DS_WFE_FS_LFSR_LFSR_SEL_MASK                          0x00000001
#define BCHP_DS_WFE_FS_LFSR_LFSR_SEL_SHIFT                         0
#define BCHP_DS_WFE_FS_LFSR_LFSR_SEL_DEFAULT                       0

/***************************************************************************
 *PPF_CTRL - PPF control register
 ***************************************************************************/
/* DS_WFE_FS :: PPF_CTRL :: reserved_for_eco0 [31:01] */
#define BCHP_DS_WFE_FS_PPF_CTRL_reserved_for_eco0_MASK             0xfffffffe
#define BCHP_DS_WFE_FS_PPF_CTRL_reserved_for_eco0_SHIFT            1
#define BCHP_DS_WFE_FS_PPF_CTRL_reserved_for_eco0_DEFAULT          0

/* DS_WFE_FS :: PPF_CTRL :: RESET_PPF [00:00] */
#define BCHP_DS_WFE_FS_PPF_CTRL_RESET_PPF_MASK                     0x00000001
#define BCHP_DS_WFE_FS_PPF_CTRL_RESET_PPF_SHIFT                    0
#define BCHP_DS_WFE_FS_PPF_CTRL_RESET_PPF_DEFAULT                  0

/***************************************************************************
 *OBADC_CTRL - OBADC Control register
 ***************************************************************************/
/* DS_WFE_FS :: OBADC_CTRL :: reserved_for_eco0 [31:12] */
#define BCHP_DS_WFE_FS_OBADC_CTRL_reserved_for_eco0_MASK           0xfffff000
#define BCHP_DS_WFE_FS_OBADC_CTRL_reserved_for_eco0_SHIFT          12
#define BCHP_DS_WFE_FS_OBADC_CTRL_reserved_for_eco0_DEFAULT        0

/* DS_WFE_FS :: OBADC_CTRL :: LFSR_FIFO_ENABLE [11:11] */
#define BCHP_DS_WFE_FS_OBADC_CTRL_LFSR_FIFO_ENABLE_MASK            0x00000800
#define BCHP_DS_WFE_FS_OBADC_CTRL_LFSR_FIFO_ENABLE_SHIFT           11
#define BCHP_DS_WFE_FS_OBADC_CTRL_LFSR_FIFO_ENABLE_DEFAULT         0

/* DS_WFE_FS :: OBADC_CTRL :: LFSR_FIFO_LANE_SEL [10:10] */
#define BCHP_DS_WFE_FS_OBADC_CTRL_LFSR_FIFO_LANE_SEL_MASK          0x00000400
#define BCHP_DS_WFE_FS_OBADC_CTRL_LFSR_FIFO_LANE_SEL_SHIFT         10
#define BCHP_DS_WFE_FS_OBADC_CTRL_LFSR_FIFO_LANE_SEL_DEFAULT       0

/* DS_WFE_FS :: OBADC_CTRL :: LFSR_RCVR_ENABLE [09:09] */
#define BCHP_DS_WFE_FS_OBADC_CTRL_LFSR_RCVR_ENABLE_MASK            0x00000200
#define BCHP_DS_WFE_FS_OBADC_CTRL_LFSR_RCVR_ENABLE_SHIFT           9
#define BCHP_DS_WFE_FS_OBADC_CTRL_LFSR_RCVR_ENABLE_DEFAULT         0

/* DS_WFE_FS :: OBADC_CTRL :: LFSR_RCVR_LANE_SEL [08:08] */
#define BCHP_DS_WFE_FS_OBADC_CTRL_LFSR_RCVR_LANE_SEL_MASK          0x00000100
#define BCHP_DS_WFE_FS_OBADC_CTRL_LFSR_RCVR_LANE_SEL_SHIFT         8
#define BCHP_DS_WFE_FS_OBADC_CTRL_LFSR_RCVR_LANE_SEL_DEFAULT       0

/* DS_WFE_FS :: OBADC_CTRL :: ADCIN1_SEL [07:07] */
#define BCHP_DS_WFE_FS_OBADC_CTRL_ADCIN1_SEL_MASK                  0x00000080
#define BCHP_DS_WFE_FS_OBADC_CTRL_ADCIN1_SEL_SHIFT                 7
#define BCHP_DS_WFE_FS_OBADC_CTRL_ADCIN1_SEL_DEFAULT               1

/* DS_WFE_FS :: OBADC_CTRL :: ADCIN1_EDGE [06:06] */
#define BCHP_DS_WFE_FS_OBADC_CTRL_ADCIN1_EDGE_MASK                 0x00000040
#define BCHP_DS_WFE_FS_OBADC_CTRL_ADCIN1_EDGE_SHIFT                6
#define BCHP_DS_WFE_FS_OBADC_CTRL_ADCIN1_EDGE_DEFAULT              0

/* DS_WFE_FS :: OBADC_CTRL :: ADCIN0_SEL [05:05] */
#define BCHP_DS_WFE_FS_OBADC_CTRL_ADCIN0_SEL_MASK                  0x00000020
#define BCHP_DS_WFE_FS_OBADC_CTRL_ADCIN0_SEL_SHIFT                 5
#define BCHP_DS_WFE_FS_OBADC_CTRL_ADCIN0_SEL_DEFAULT               0

/* DS_WFE_FS :: OBADC_CTRL :: ADCIN0_EDGE [04:04] */
#define BCHP_DS_WFE_FS_OBADC_CTRL_ADCIN0_EDGE_MASK                 0x00000010
#define BCHP_DS_WFE_FS_OBADC_CTRL_ADCIN0_EDGE_SHIFT                4
#define BCHP_DS_WFE_FS_OBADC_CTRL_ADCIN0_EDGE_DEFAULT              0

/* DS_WFE_FS :: OBADC_CTRL :: reserved_for_eco1 [03:03] */
#define BCHP_DS_WFE_FS_OBADC_CTRL_reserved_for_eco1_MASK           0x00000008
#define BCHP_DS_WFE_FS_OBADC_CTRL_reserved_for_eco1_SHIFT          3
#define BCHP_DS_WFE_FS_OBADC_CTRL_reserved_for_eco1_DEFAULT        0

/* DS_WFE_FS :: OBADC_CTRL :: HB2_IN_SEL [02:01] */
#define BCHP_DS_WFE_FS_OBADC_CTRL_HB2_IN_SEL_MASK                  0x00000006
#define BCHP_DS_WFE_FS_OBADC_CTRL_HB2_IN_SEL_SHIFT                 1
#define BCHP_DS_WFE_FS_OBADC_CTRL_HB2_IN_SEL_DEFAULT               0

/* DS_WFE_FS :: OBADC_CTRL :: RESET [00:00] */
#define BCHP_DS_WFE_FS_OBADC_CTRL_RESET_MASK                       0x00000001
#define BCHP_DS_WFE_FS_OBADC_CTRL_RESET_SHIFT                      0
#define BCHP_DS_WFE_FS_OBADC_CTRL_RESET_DEFAULT                    0

/***************************************************************************
 *OBADC_FCW - OBADC FCW control register
 ***************************************************************************/
/* DS_WFE_FS :: OBADC_FCW :: reserved0 [31:24] */
#define BCHP_DS_WFE_FS_OBADC_FCW_reserved0_MASK                    0xff000000
#define BCHP_DS_WFE_FS_OBADC_FCW_reserved0_SHIFT                   24

/* DS_WFE_FS :: OBADC_FCW :: FCW [23:00] */
#define BCHP_DS_WFE_FS_OBADC_FCW_FCW_MASK                          0x00ffffff
#define BCHP_DS_WFE_FS_OBADC_FCW_FCW_SHIFT                         0
#define BCHP_DS_WFE_FS_OBADC_FCW_FCW_DEFAULT                       0

/***************************************************************************
 *OBADC_STATUS - OBADC status register
 ***************************************************************************/
/* DS_WFE_FS :: OBADC_STATUS :: reserved0 [31:02] */
#define BCHP_DS_WFE_FS_OBADC_STATUS_reserved0_MASK                 0xfffffffc
#define BCHP_DS_WFE_FS_OBADC_STATUS_reserved0_SHIFT                2

/* DS_WFE_FS :: OBADC_STATUS :: LFSR_FIFO_COMP_FAIL [01:01] */
#define BCHP_DS_WFE_FS_OBADC_STATUS_LFSR_FIFO_COMP_FAIL_MASK       0x00000002
#define BCHP_DS_WFE_FS_OBADC_STATUS_LFSR_FIFO_COMP_FAIL_SHIFT      1
#define BCHP_DS_WFE_FS_OBADC_STATUS_LFSR_FIFO_COMP_FAIL_DEFAULT    0

/* DS_WFE_FS :: OBADC_STATUS :: LFSR_RCVR_COMP_FAIL [00:00] */
#define BCHP_DS_WFE_FS_OBADC_STATUS_LFSR_RCVR_COMP_FAIL_MASK       0x00000001
#define BCHP_DS_WFE_FS_OBADC_STATUS_LFSR_RCVR_COMP_FAIL_SHIFT      0
#define BCHP_DS_WFE_FS_OBADC_STATUS_LFSR_RCVR_COMP_FAIL_DEFAULT    0

/***************************************************************************
 *DAC_CTRL - DAC control register
 ***************************************************************************/
/* DS_WFE_FS :: DAC_CTRL :: reserved_for_eco0 [31:12] */
#define BCHP_DS_WFE_FS_DAC_CTRL_reserved_for_eco0_MASK             0xfffff000
#define BCHP_DS_WFE_FS_DAC_CTRL_reserved_for_eco0_SHIFT            12
#define BCHP_DS_WFE_FS_DAC_CTRL_reserved_for_eco0_DEFAULT          0

/* DS_WFE_FS :: DAC_CTRL :: LFSR_XMTR_ENABLE1 [11:11] */
#define BCHP_DS_WFE_FS_DAC_CTRL_LFSR_XMTR_ENABLE1_MASK             0x00000800
#define BCHP_DS_WFE_FS_DAC_CTRL_LFSR_XMTR_ENABLE1_SHIFT            11
#define BCHP_DS_WFE_FS_DAC_CTRL_LFSR_XMTR_ENABLE1_DEFAULT          0

/* DS_WFE_FS :: DAC_CTRL :: LFSR_XMTR_SEL1 [10:10] */
#define BCHP_DS_WFE_FS_DAC_CTRL_LFSR_XMTR_SEL1_MASK                0x00000400
#define BCHP_DS_WFE_FS_DAC_CTRL_LFSR_XMTR_SEL1_SHIFT               10
#define BCHP_DS_WFE_FS_DAC_CTRL_LFSR_XMTR_SEL1_DEFAULT             0

/* DS_WFE_FS :: DAC_CTRL :: LFSR_XMTR_ENABLE0 [09:09] */
#define BCHP_DS_WFE_FS_DAC_CTRL_LFSR_XMTR_ENABLE0_MASK             0x00000200
#define BCHP_DS_WFE_FS_DAC_CTRL_LFSR_XMTR_ENABLE0_SHIFT            9
#define BCHP_DS_WFE_FS_DAC_CTRL_LFSR_XMTR_ENABLE0_DEFAULT          0

/* DS_WFE_FS :: DAC_CTRL :: LFSR_XMTR_SEL0 [08:08] */
#define BCHP_DS_WFE_FS_DAC_CTRL_LFSR_XMTR_SEL0_MASK                0x00000100
#define BCHP_DS_WFE_FS_DAC_CTRL_LFSR_XMTR_SEL0_SHIFT               8
#define BCHP_DS_WFE_FS_DAC_CTRL_LFSR_XMTR_SEL0_DEFAULT             0

/* DS_WFE_FS :: DAC_CTRL :: CHAN_IN_SEL [07:04] */
#define BCHP_DS_WFE_FS_DAC_CTRL_CHAN_IN_SEL_MASK                   0x000000f0
#define BCHP_DS_WFE_FS_DAC_CTRL_CHAN_IN_SEL_SHIFT                  4
#define BCHP_DS_WFE_FS_DAC_CTRL_CHAN_IN_SEL_DEFAULT                0

/* DS_WFE_FS :: DAC_CTRL :: reserved_for_eco1 [03:02] */
#define BCHP_DS_WFE_FS_DAC_CTRL_reserved_for_eco1_MASK             0x0000000c
#define BCHP_DS_WFE_FS_DAC_CTRL_reserved_for_eco1_SHIFT            2
#define BCHP_DS_WFE_FS_DAC_CTRL_reserved_for_eco1_DEFAULT          0

/* DS_WFE_FS :: DAC_CTRL :: HB_IN_SEL [01:01] */
#define BCHP_DS_WFE_FS_DAC_CTRL_HB_IN_SEL_MASK                     0x00000002
#define BCHP_DS_WFE_FS_DAC_CTRL_HB_IN_SEL_SHIFT                    1
#define BCHP_DS_WFE_FS_DAC_CTRL_HB_IN_SEL_DEFAULT                  0

/* DS_WFE_FS :: DAC_CTRL :: RESET [00:00] */
#define BCHP_DS_WFE_FS_DAC_CTRL_RESET_MASK                         0x00000001
#define BCHP_DS_WFE_FS_DAC_CTRL_RESET_SHIFT                        0
#define BCHP_DS_WFE_FS_DAC_CTRL_RESET_DEFAULT                      0

/***************************************************************************
 *DAC_FCW0 - DAC FCW0 register
 ***************************************************************************/
/* DS_WFE_FS :: DAC_FCW0 :: reserved0 [31:24] */
#define BCHP_DS_WFE_FS_DAC_FCW0_reserved0_MASK                     0xff000000
#define BCHP_DS_WFE_FS_DAC_FCW0_reserved0_SHIFT                    24

/* DS_WFE_FS :: DAC_FCW0 :: FCW [23:00] */
#define BCHP_DS_WFE_FS_DAC_FCW0_FCW_MASK                           0x00ffffff
#define BCHP_DS_WFE_FS_DAC_FCW0_FCW_SHIFT                          0
#define BCHP_DS_WFE_FS_DAC_FCW0_FCW_DEFAULT                        0

/***************************************************************************
 *DAC_FCW1 - DAC FCW1 register
 ***************************************************************************/
/* DS_WFE_FS :: DAC_FCW1 :: reserved0 [31:24] */
#define BCHP_DS_WFE_FS_DAC_FCW1_reserved0_MASK                     0xff000000
#define BCHP_DS_WFE_FS_DAC_FCW1_reserved0_SHIFT                    24

/* DS_WFE_FS :: DAC_FCW1 :: FCW [23:00] */
#define BCHP_DS_WFE_FS_DAC_FCW1_FCW_MASK                           0x00ffffff
#define BCHP_DS_WFE_FS_DAC_FCW1_FCW_SHIFT                          0
#define BCHP_DS_WFE_FS_DAC_FCW1_FCW_DEFAULT                        0

/***************************************************************************
 *DAC_TEST - DAC TEST register
 ***************************************************************************/
/* DS_WFE_FS :: DAC_TEST :: reserved0 [31:25] */
#define BCHP_DS_WFE_FS_DAC_TEST_reserved0_MASK                     0xfe000000
#define BCHP_DS_WFE_FS_DAC_TEST_reserved0_SHIFT                    25

/* DS_WFE_FS :: DAC_TEST :: DDFS_2TONE_EN [24:24] */
#define BCHP_DS_WFE_FS_DAC_TEST_DDFS_2TONE_EN_MASK                 0x01000000
#define BCHP_DS_WFE_FS_DAC_TEST_DDFS_2TONE_EN_SHIFT                24
#define BCHP_DS_WFE_FS_DAC_TEST_DDFS_2TONE_EN_DEFAULT              0

/* DS_WFE_FS :: DAC_TEST :: SCALER2 [23:12] */
#define BCHP_DS_WFE_FS_DAC_TEST_SCALER2_MASK                       0x00fff000
#define BCHP_DS_WFE_FS_DAC_TEST_SCALER2_SHIFT                      12
#define BCHP_DS_WFE_FS_DAC_TEST_SCALER2_DEFAULT                    0

/* DS_WFE_FS :: DAC_TEST :: SCALER1 [11:00] */
#define BCHP_DS_WFE_FS_DAC_TEST_SCALER1_MASK                       0x00000fff
#define BCHP_DS_WFE_FS_DAC_TEST_SCALER1_SHIFT                      0
#define BCHP_DS_WFE_FS_DAC_TEST_SCALER1_DEFAULT                    0

#endif /* #ifndef BCHP_DS_WFE_FS_H__ */

/* End of File */