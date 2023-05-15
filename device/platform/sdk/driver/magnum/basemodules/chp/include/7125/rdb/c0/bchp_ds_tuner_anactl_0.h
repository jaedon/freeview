/***************************************************************************
 *     Copyright (c) 1999-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bchp_ds_tuner_anactl_0.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 1/25/10 8:10p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Fri Jan 22 20:29:38 2010
 *                 MD5 Checksum         a2d1f2163f65e87d228a0fb491cb442d
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7125/rdb/c0/bchp_ds_tuner_anactl_0.h $
 * 
 * Hydra_Software_Devel/1   1/25/10 8:10p albertl
 * SW7125-177: Initial revision.
 *
 ***************************************************************************/

#ifndef BCHP_DS_TUNER_ANACTL_0_H__
#define BCHP_DS_TUNER_ANACTL_0_H__

/***************************************************************************
 *DS_TUNER_ANACTL_0 - Tuner Analog Control Registers 0
 ***************************************************************************/
#define BCHP_DS_TUNER_ANACTL_0_WDATA_00          0x04c32000 /* Analog Tuner Control Register */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_01          0x04c32004 /* Analog Tuner Control Register */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_02          0x04c32008 /* Analog Tuner Control Register */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_03          0x04c3200c /* Analog Tuner Control Register */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_04          0x04c32010 /* Analog Tuner Control Register */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_05          0x04c32014 /* Analog Tuner Control Register */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_06          0x04c32018 /* Analog Tuner Control Register */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_07          0x04c3201c /* Analog Tuner Control Register */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_14          0x04c32038 /* Analog Tuner Control Register */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_15          0x04c3203c /* Analog Tuner Control Register */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_16          0x04c32040 /* Analog Tuner Control Register */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_17          0x04c32044 /* Analog Tuner Control Register */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_18          0x04c32048 /* Analog Tuner Control Register */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_19          0x04c3204c /* Analog Tuner Control Register */

/***************************************************************************
 *WDATA_00 - Analog Tuner Control Register
 ***************************************************************************/
/* DS_TUNER_ANACTL_0 :: WDATA_00 :: SPARE [31:18] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_00_SPARE_MASK                 0xfffc0000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_00_SPARE_SHIFT                18

/* DS_TUNER_ANACTL_0 :: WDATA_00 :: DCO_PRE_Q_COMP_OUT [17:17] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_00_DCO_PRE_Q_COMP_OUT_MASK    0x00020000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_00_DCO_PRE_Q_COMP_OUT_SHIFT   17

/* DS_TUNER_ANACTL_0 :: WDATA_00 :: DCO_PRE_I_COMP_OUT [16:16] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_00_DCO_PRE_I_COMP_OUT_MASK    0x00010000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_00_DCO_PRE_I_COMP_OUT_SHIFT   16

/* DS_TUNER_ANACTL_0 :: WDATA_00 :: REVISION [15:00] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_00_REVISION_MASK              0x0000ffff
#define BCHP_DS_TUNER_ANACTL_0_WDATA_00_REVISION_SHIFT             0

/***************************************************************************
 *WDATA_01 - Analog Tuner Control Register
 ***************************************************************************/
/* DS_TUNER_ANACTL_0 :: WDATA_01 :: RFTRK1_IN [31:29] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_01_RFTRK1_IN_MASK             0xe0000000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_01_RFTRK1_IN_SHIFT            29

/* DS_TUNER_ANACTL_0 :: WDATA_01 :: RFTRK2_BIAS [28:27] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_01_RFTRK2_BIAS_MASK           0x18000000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_01_RFTRK2_BIAS_SHIFT          27

/* DS_TUNER_ANACTL_0 :: WDATA_01 :: RFTRK1_BIAS [26:23] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_01_RFTRK1_BIAS_MASK           0x07800000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_01_RFTRK1_BIAS_SHIFT          23

/* DS_TUNER_ANACTL_0 :: WDATA_01 :: RFTRK1_VDD_CNTL [22:20] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_01_RFTRK1_VDD_CNTL_MASK       0x00700000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_01_RFTRK1_VDD_CNTL_SHIFT      20

/* DS_TUNER_ANACTL_0 :: WDATA_01 :: RFTRK1_ROOF [19:16] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_01_RFTRK1_ROOF_MASK           0x000f0000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_01_RFTRK1_ROOF_SHIFT          16

/* DS_TUNER_ANACTL_0 :: WDATA_01 :: RFTRK1_BW [15:10] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_01_RFTRK1_BW_MASK             0x0000fc00
#define BCHP_DS_TUNER_ANACTL_0_WDATA_01_RFTRK1_BW_SHIFT            10

/* DS_TUNER_ANACTL_0 :: WDATA_01 :: RFFE_ATTDB [09:07] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_01_RFFE_ATTDB_MASK            0x00000380
#define BCHP_DS_TUNER_ANACTL_0_WDATA_01_RFFE_ATTDB_SHIFT           7

/* DS_TUNER_ANACTL_0 :: WDATA_01 :: FILTER_SEL [06:05] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_01_FILTER_SEL_MASK            0x00000060
#define BCHP_DS_TUNER_ANACTL_0_WDATA_01_FILTER_SEL_SHIFT           5

/* DS_TUNER_ANACTL_0 :: WDATA_01 :: DPM_RST [04:04] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_01_DPM_RST_MASK               0x00000010
#define BCHP_DS_TUNER_ANACTL_0_WDATA_01_DPM_RST_SHIFT              4

/* DS_TUNER_ANACTL_0 :: WDATA_01 :: DPM_PWRDN [03:03] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_01_DPM_PWRDN_MASK             0x00000008
#define BCHP_DS_TUNER_ANACTL_0_WDATA_01_DPM_PWRDN_SHIFT            3

/* DS_TUNER_ANACTL_0 :: WDATA_01 :: DPM_AMP [02:00] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_01_DPM_AMP_MASK               0x00000007
#define BCHP_DS_TUNER_ANACTL_0_WDATA_01_DPM_AMP_SHIFT              0

/***************************************************************************
 *WDATA_02 - Analog Tuner Control Register
 ***************************************************************************/
/* DS_TUNER_ANACTL_0 :: WDATA_02 :: MIX_OUTBIAS [31:30] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_02_MIX_OUTBIAS_MASK           0xc0000000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_02_MIX_OUTBIAS_SHIFT          30

/* DS_TUNER_ANACTL_0 :: WDATA_02 :: MIX_ATT [29:27] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_02_MIX_ATT_MASK               0x38000000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_02_MIX_ATT_SHIFT              27

/* DS_TUNER_ANACTL_0 :: WDATA_02 :: MIX_PWRDN [26:26] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_02_MIX_PWRDN_MASK             0x04000000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_02_MIX_PWRDN_SHIFT            26

/* DS_TUNER_ANACTL_0 :: WDATA_02 :: MIX_ICNTL [25:22] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_02_MIX_ICNTL_MASK             0x03c00000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_02_MIX_ICNTL_SHIFT            22

/* DS_TUNER_ANACTL_0 :: WDATA_02 :: PHOUT_LVLBKOFF [21:21] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_02_PHOUT_LVLBKOFF_MASK        0x00200000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_02_PHOUT_LVLBKOFF_SHIFT       21

/* DS_TUNER_ANACTL_0 :: WDATA_02 :: DIV_SEL [20:18] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_02_DIV_SEL_MASK               0x001c0000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_02_DIV_SEL_SHIFT              18

/* DS_TUNER_ANACTL_0 :: WDATA_02 :: RSTCMOSBB [17:17] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_02_RSTCMOSBB_MASK             0x00020000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_02_RSTCMOSBB_SHIFT            17

/* DS_TUNER_ANACTL_0 :: WDATA_02 :: HR_EN [16:16] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_02_HR_EN_MASK                 0x00010000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_02_HR_EN_SHIFT                16

/* DS_TUNER_ANACTL_0 :: WDATA_02 :: EN_DC_DPM [15:15] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_02_EN_DC_DPM_MASK             0x00008000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_02_EN_DC_DPM_SHIFT            15

/* DS_TUNER_ANACTL_0 :: WDATA_02 :: VGA_CM_EN [14:14] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_02_VGA_CM_EN_MASK             0x00004000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_02_VGA_CM_EN_SHIFT            14

/* DS_TUNER_ANACTL_0 :: WDATA_02 :: DPM_6DB_GAIN [13:13] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_02_DPM_6DB_GAIN_MASK          0x00002000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_02_DPM_6DB_GAIN_SHIFT         13

/* DS_TUNER_ANACTL_0 :: WDATA_02 :: RFT_VCM_BOOST_EN [12:12] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_02_RFT_VCM_BOOST_EN_MASK      0x00001000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_02_RFT_VCM_BOOST_EN_SHIFT     12

/* DS_TUNER_ANACTL_0 :: WDATA_02 :: PGALPF_PWRDN [11:11] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_02_PGALPF_PWRDN_MASK          0x00000800
#define BCHP_DS_TUNER_ANACTL_0_WDATA_02_PGALPF_PWRDN_SHIFT         11

/* DS_TUNER_ANACTL_0 :: WDATA_02 :: RFTRK_VDD2P1 [10:08] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_02_RFTRK_VDD2P1_MASK          0x00000700
#define BCHP_DS_TUNER_ANACTL_0_WDATA_02_RFTRK_VDD2P1_SHIFT         8

/* DS_TUNER_ANACTL_0 :: WDATA_02 :: RFTRK_CM [07:05] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_02_RFTRK_CM_MASK              0x000000e0
#define BCHP_DS_TUNER_ANACTL_0_WDATA_02_RFTRK_CM_SHIFT             5

/* DS_TUNER_ANACTL_0 :: WDATA_02 :: RFTRK2_BW_HF [04:04] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_02_RFTRK2_BW_HF_MASK          0x00000010
#define BCHP_DS_TUNER_ANACTL_0_WDATA_02_RFTRK2_BW_HF_SHIFT         4

/* DS_TUNER_ANACTL_0 :: WDATA_02 :: RFTRK2_BW [03:00] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_02_RFTRK2_BW_MASK             0x0000000f
#define BCHP_DS_TUNER_ANACTL_0_WDATA_02_RFTRK2_BW_SHIFT            0

/***************************************************************************
 *WDATA_03 - Analog Tuner Control Register
 ***************************************************************************/
/* DS_TUNER_ANACTL_0 :: WDATA_03 :: PDB_PHOUT [31:31] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_03_PDB_PHOUT_MASK             0x80000000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_03_PDB_PHOUT_SHIFT            31

/* DS_TUNER_ANACTL_0 :: WDATA_03 :: IFLPF_BIAS [30:27] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_03_IFLPF_BIAS_MASK            0x78000000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_03_IFLPF_BIAS_SHIFT           27

/* DS_TUNER_ANACTL_0 :: WDATA_03 :: IFLPF_PWROFFQ [26:26] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_03_IFLPF_PWROFFQ_MASK         0x04000000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_03_IFLPF_PWROFFQ_SHIFT        26

/* DS_TUNER_ANACTL_0 :: WDATA_03 :: IFLPF_PWROFFI [25:25] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_03_IFLPF_PWROFFI_MASK         0x02000000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_03_IFLPF_PWROFFI_SHIFT        25

/* DS_TUNER_ANACTL_0 :: WDATA_03 :: IFLPF_PWROFF [24:24] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_03_IFLPF_PWROFF_MASK          0x01000000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_03_IFLPF_PWROFF_SHIFT         24

/* DS_TUNER_ANACTL_0 :: WDATA_03 :: IFLPF_BWR [23:17] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_03_IFLPF_BWR_MASK             0x00fe0000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_03_IFLPF_BWR_SHIFT            17

/* DS_TUNER_ANACTL_0 :: WDATA_03 :: FGA_RC_CNTL [16:10] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_03_FGA_RC_CNTL_MASK           0x0001fc00
#define BCHP_DS_TUNER_ANACTL_0_WDATA_03_FGA_RC_CNTL_SHIFT          10

/* DS_TUNER_ANACTL_0 :: WDATA_03 :: PGA_PWRDN [09:09] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_03_PGA_PWRDN_MASK             0x00000200
#define BCHP_DS_TUNER_ANACTL_0_WDATA_03_PGA_PWRDN_SHIFT            9

/* DS_TUNER_ANACTL_0 :: WDATA_03 :: FGA_ICNTL [08:06] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_03_FGA_ICNTL_MASK             0x000001c0
#define BCHP_DS_TUNER_ANACTL_0_WDATA_03_FGA_ICNTL_SHIFT            6

/* DS_TUNER_ANACTL_0 :: WDATA_03 :: FGA_GAIN [05:03] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_03_FGA_GAIN_MASK              0x00000038
#define BCHP_DS_TUNER_ANACTL_0_WDATA_03_FGA_GAIN_SHIFT             3

/* DS_TUNER_ANACTL_0 :: WDATA_03 :: MIX_LOBIAS [02:00] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_03_MIX_LOBIAS_MASK            0x00000007
#define BCHP_DS_TUNER_ANACTL_0_WDATA_03_MIX_LOBIAS_SHIFT           0

/***************************************************************************
 *WDATA_04 - Analog Tuner Control Register
 ***************************************************************************/
/* DS_TUNER_ANACTL_0 :: WDATA_04 :: RFTRK1_GAIN [31:30] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_04_RFTRK1_GAIN_MASK           0xc0000000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_04_RFTRK1_GAIN_SHIFT          30

/* DS_TUNER_ANACTL_0 :: WDATA_04 :: VGAOFS_DAC_2COMP [29:29] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_04_VGAOFS_DAC_2COMP_MASK      0x20000000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_04_VGAOFS_DAC_2COMP_SHIFT     29

/* DS_TUNER_ANACTL_0 :: WDATA_04 :: VGA_QDCO_PWRDN [28:28] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_04_VGA_QDCO_PWRDN_MASK        0x10000000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_04_VGA_QDCO_PWRDN_SHIFT       28

/* DS_TUNER_ANACTL_0 :: WDATA_04 :: VGA_QDCO_LBS [27:25] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_04_VGA_QDCO_LBS_MASK          0x0e000000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_04_VGA_QDCO_LBS_SHIFT         25

/* DS_TUNER_ANACTL_0 :: WDATA_04 :: VGA_IDCO_PWRDN [24:24] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_04_VGA_IDCO_PWRDN_MASK        0x01000000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_04_VGA_IDCO_PWRDN_SHIFT       24

/* DS_TUNER_ANACTL_0 :: WDATA_04 :: VGA_IDCO_LBS [23:21] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_04_VGA_IDCO_LBS_MASK          0x00e00000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_04_VGA_IDCO_LBS_SHIFT         21

/* DS_TUNER_ANACTL_0 :: WDATA_04 :: REG1P2V_PWRDN [20:20] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_04_REG1P2V_PWRDN_MASK         0x00100000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_04_REG1P2V_PWRDN_SHIFT        20

/* DS_TUNER_ANACTL_0 :: WDATA_04 :: DCO_PRE_GM_GAIN [19:19] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_04_DCO_PRE_GM_GAIN_MASK       0x00080000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_04_DCO_PRE_GM_GAIN_SHIFT      19

/* DS_TUNER_ANACTL_0 :: WDATA_04 :: DCO_PRE_QP_I [18:16] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_04_DCO_PRE_QP_I_MASK          0x00070000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_04_DCO_PRE_QP_I_SHIFT         16

/* DS_TUNER_ANACTL_0 :: WDATA_04 :: I_DCO_PRE_IQ_PWRDN_VBAL [15:15] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_04_I_DCO_PRE_IQ_PWRDN_VBAL_MASK 0x00008000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_04_I_DCO_PRE_IQ_PWRDN_VBAL_SHIFT 15

/* DS_TUNER_ANACTL_0 :: WDATA_04 :: DCO_PRE_IQ_PWRDN [14:14] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_04_DCO_PRE_IQ_PWRDN_MASK      0x00004000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_04_DCO_PRE_IQ_PWRDN_SHIFT     14

/* DS_TUNER_ANACTL_0 :: WDATA_04 :: I_DCO_MIX_RESET [13:13] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_04_I_DCO_MIX_RESET_MASK       0x00002000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_04_I_DCO_MIX_RESET_SHIFT      13

/* DS_TUNER_ANACTL_0 :: WDATA_04 :: VGA_64M_BW [12:12] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_04_VGA_64M_BW_MASK            0x00001000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_04_VGA_64M_BW_SHIFT           12

/* DS_TUNER_ANACTL_0 :: WDATA_04 :: I_ADC_CM_EN [11:11] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_04_I_ADC_CM_EN_MASK           0x00000800
#define BCHP_DS_TUNER_ANACTL_0_WDATA_04_I_ADC_CM_EN_SHIFT          11

/* DS_TUNER_ANACTL_0 :: WDATA_04 :: OUT_PAD_EN [10:10] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_04_OUT_PAD_EN_MASK            0x00000400
#define BCHP_DS_TUNER_ANACTL_0_WDATA_04_OUT_PAD_EN_SHIFT           10

/* DS_TUNER_ANACTL_0 :: WDATA_04 :: OUTVGA_PWRSAV [09:06] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_04_OUTVGA_PWRSAV_MASK         0x000003c0
#define BCHP_DS_TUNER_ANACTL_0_WDATA_04_OUTVGA_PWRSAV_SHIFT        6

/* DS_TUNER_ANACTL_0 :: WDATA_04 :: OUTVGA_PWRDN [05:05] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_04_OUTVGA_PWRDN_MASK          0x00000020
#define BCHP_DS_TUNER_ANACTL_0_WDATA_04_OUTVGA_PWRDN_SHIFT         5

/* DS_TUNER_ANACTL_0 :: WDATA_04 :: OUTVGA_CM_SFT [04:02] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_04_OUTVGA_CM_SFT_MASK         0x0000001c
#define BCHP_DS_TUNER_ANACTL_0_WDATA_04_OUTVGA_CM_SFT_SHIFT        2

/* DS_TUNER_ANACTL_0 :: WDATA_04 :: OUTVGA_ATT [01:00] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_04_OUTVGA_ATT_MASK            0x00000003
#define BCHP_DS_TUNER_ANACTL_0_WDATA_04_OUTVGA_ATT_SHIFT           0

/***************************************************************************
 *WDATA_05 - Analog Tuner Control Register
 ***************************************************************************/
/* DS_TUNER_ANACTL_0 :: WDATA_05 :: IP_FBLVLFDOFF [31:31] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_05_IP_FBLVLFDOFF_MASK         0x80000000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_05_IP_FBLVLFDOFF_SHIFT        31

/* DS_TUNER_ANACTL_0 :: WDATA_05 :: IP_PD [30:30] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_05_IP_PD_MASK                 0x40000000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_05_IP_PD_SHIFT                30

/* DS_TUNER_ANACTL_0 :: WDATA_05 :: IP_VCREF [29:25] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_05_IP_VCREF_MASK              0x3e000000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_05_IP_VCREF_SHIFT             25

/* DS_TUNER_ANACTL_0 :: WDATA_05 :: IP_PWRUP_WD [24:24] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_05_IP_PWRUP_WD_MASK           0x01000000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_05_IP_PWRUP_WD_SHIFT          24

/* DS_TUNER_ANACTL_0 :: WDATA_05 :: IP_LD_RESET [23:23] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_05_IP_LD_RESET_MASK           0x00800000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_05_IP_LD_RESET_SHIFT          23

/* DS_TUNER_ANACTL_0 :: WDATA_05 :: IP_LD_EN [22:22] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_05_IP_LD_EN_MASK              0x00400000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_05_IP_LD_EN_SHIFT             22

/* DS_TUNER_ANACTL_0 :: WDATA_05 :: IP_VCO_SEL [21:20] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_05_IP_VCO_SEL_MASK            0x00300000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_05_IP_VCO_SEL_SHIFT           20

/* DS_TUNER_ANACTL_0 :: WDATA_05 :: IP_CAP_CNTL [19:14] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_05_IP_CAP_CNTL_MASK           0x000fc000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_05_IP_CAP_CNTL_SHIFT          14

/* DS_TUNER_ANACTL_0 :: WDATA_05 :: IP_REF_CNTL [13:08] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_05_IP_REF_CNTL_MASK           0x00003f00
#define BCHP_DS_TUNER_ANACTL_0_WDATA_05_IP_REF_CNTL_SHIFT          8

/* DS_TUNER_ANACTL_0 :: WDATA_05 :: IP_VCO_PWRDN [07:07] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_05_IP_VCO_PWRDN_MASK          0x00000080
#define BCHP_DS_TUNER_ANACTL_0_WDATA_05_IP_VCO_PWRDN_SHIFT         7

/* DS_TUNER_ANACTL_0 :: WDATA_05 :: IP_CML_DIV2_CNT [06:05] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_05_IP_CML_DIV2_CNT_MASK       0x00000060
#define BCHP_DS_TUNER_ANACTL_0_WDATA_05_IP_CML_DIV2_CNT_SHIFT      5

/* DS_TUNER_ANACTL_0 :: WDATA_05 :: IP_CMLDIVRST [04:04] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_05_IP_CMLDIVRST_MASK          0x00000010
#define BCHP_DS_TUNER_ANACTL_0_WDATA_05_IP_CMLDIVRST_SHIFT         4

/* DS_TUNER_ANACTL_0 :: WDATA_05 :: PWRDN_BG_1P2 [03:03] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_05_PWRDN_BG_1P2_MASK          0x00000008
#define BCHP_DS_TUNER_ANACTL_0_WDATA_05_PWRDN_BG_1P2_SHIFT         3

/* DS_TUNER_ANACTL_0 :: WDATA_05 :: BG_CORE_ADJ [02:00] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_05_BG_CORE_ADJ_MASK           0x00000007
#define BCHP_DS_TUNER_ANACTL_0_WDATA_05_BG_CORE_ADJ_SHIFT          0

/***************************************************************************
 *WDATA_06 - Analog Tuner Control Register
 ***************************************************************************/
/* DS_TUNER_ANACTL_0 :: WDATA_06 :: IP_LODIVBY4EN [31:31] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_06_IP_LODIVBY4EN_MASK         0x80000000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_06_IP_LODIVBY4EN_SHIFT        31

/* DS_TUNER_ANACTL_0 :: WDATA_06 :: SPARE0 [30:30] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_06_SPARE0_MASK                0x40000000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_06_SPARE0_SHIFT               30

/* DS_TUNER_ANACTL_0 :: WDATA_06 :: IMF_CM [29:27] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_06_IMF_CM_MASK                0x38000000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_06_IMF_CM_SHIFT               27

/* DS_TUNER_ANACTL_0 :: WDATA_06 :: IP_RCNTL [26:25] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_06_IP_RCNTL_MASK              0x06000000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_06_IP_RCNTL_SHIFT             25

/* DS_TUNER_ANACTL_0 :: WDATA_06 :: IP_MANRSTCLF [24:24] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_06_IP_MANRSTCLF_MASK          0x01000000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_06_IP_MANRSTCLF_SHIFT         24

/* DS_TUNER_ANACTL_0 :: WDATA_06 :: IP_PWRUP_LF [23:23] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_06_IP_PWRUP_LF_MASK           0x00800000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_06_IP_PWRUP_LF_SHIFT          23

/* DS_TUNER_ANACTL_0 :: WDATA_06 :: IP_VC_PRB_EN [22:22] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_06_IP_VC_PRB_EN_MASK          0x00400000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_06_IP_VC_PRB_EN_SHIFT         22

/* DS_TUNER_ANACTL_0 :: WDATA_06 :: IP_VZ_PRB_EN [21:21] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_06_IP_VZ_PRB_EN_MASK          0x00200000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_06_IP_VZ_PRB_EN_SHIFT         21

/* DS_TUNER_ANACTL_0 :: WDATA_06 :: IP_QP2_PD [20:20] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_06_IP_QP2_PD_MASK             0x00100000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_06_IP_QP2_PD_SHIFT            20

/* DS_TUNER_ANACTL_0 :: WDATA_06 :: IP_QPBIASCNT2 [19:15] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_06_IP_QPBIASCNT2_MASK         0x000f8000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_06_IP_QPBIASCNT2_SHIFT        15

/* DS_TUNER_ANACTL_0 :: WDATA_06 :: IP_QPMAINBIASCNT2 [14:13] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_06_IP_QPMAINBIASCNT2_MASK     0x00006000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_06_IP_QPMAINBIASCNT2_SHIFT    13

/* DS_TUNER_ANACTL_0 :: WDATA_06 :: IP_MANVBAL [12:12] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_06_IP_MANVBAL_MASK            0x00001000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_06_IP_MANVBAL_SHIFT           12

/* DS_TUNER_ANACTL_0 :: WDATA_06 :: IP_MANIBAL [11:11] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_06_IP_MANIBAL_MASK            0x00000800
#define BCHP_DS_TUNER_ANACTL_0_WDATA_06_IP_MANIBAL_SHIFT           11

/* DS_TUNER_ANACTL_0 :: WDATA_06 :: IP_QPBIASCNT [10:06] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_06_IP_QPBIASCNT_MASK          0x000007c0
#define BCHP_DS_TUNER_ANACTL_0_WDATA_06_IP_QPBIASCNT_SHIFT         6

/* DS_TUNER_ANACTL_0 :: WDATA_06 :: IP_QPMAINBIASCNT [05:04] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_06_IP_QPMAINBIASCNT_MASK      0x00000030
#define BCHP_DS_TUNER_ANACTL_0_WDATA_06_IP_QPMAINBIASCNT_SHIFT     4

/* DS_TUNER_ANACTL_0 :: WDATA_06 :: IP_QP_PD [03:03] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_06_IP_QP_PD_MASK              0x00000008
#define BCHP_DS_TUNER_ANACTL_0_WDATA_06_IP_QP_PD_SHIFT             3

/* DS_TUNER_ANACTL_0 :: WDATA_06 :: IP_FBCMIBIASCNT [02:01] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_06_IP_FBCMIBIASCNT_MASK       0x00000006
#define BCHP_DS_TUNER_ANACTL_0_WDATA_06_IP_FBCMIBIASCNT_SHIFT      1

/* DS_TUNER_ANACTL_0 :: WDATA_06 :: IP_DIV16_12B [00:00] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_06_IP_DIV16_12B_MASK          0x00000001
#define BCHP_DS_TUNER_ANACTL_0_WDATA_06_IP_DIV16_12B_SHIFT         0

/***************************************************************************
 *WDATA_07 - Analog Tuner Control Register
 ***************************************************************************/
/* DS_TUNER_ANACTL_0 :: WDATA_07 :: SPARE0 [31:31] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_07_SPARE0_MASK                0x80000000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_07_SPARE0_SHIFT               31

/* DS_TUNER_ANACTL_0 :: WDATA_07 :: SPARE1 [30:29] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_07_SPARE1_MASK                0x60000000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_07_SPARE1_SHIFT               29

/* DS_TUNER_ANACTL_0 :: WDATA_07 :: CML1200_CMLCNT [28:27] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_07_CML1200_CMLCNT_MASK        0x18000000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_07_CML1200_CMLCNT_SHIFT       27

/* DS_TUNER_ANACTL_0 :: WDATA_07 :: D2D1200_PWRDN [26:26] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_07_D2D1200_PWRDN_MASK         0x04000000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_07_D2D1200_PWRDN_SHIFT        26

/* DS_TUNER_ANACTL_0 :: WDATA_07 :: VBG_XTAL_EN [25:25] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_07_VBG_XTAL_EN_MASK           0x02000000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_07_VBG_XTAL_EN_SHIFT          25

/* DS_TUNER_ANACTL_0 :: WDATA_07 :: CML1200_PWRDN [24:24] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_07_CML1200_PWRDN_MASK         0x01000000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_07_CML1200_PWRDN_SHIFT        24

/* DS_TUNER_ANACTL_0 :: WDATA_07 :: I_DAC_CLKSEL [23:23] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_07_I_DAC_CLKSEL_MASK          0x00800000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_07_I_DAC_CLKSEL_SHIFT         23

/* DS_TUNER_ANACTL_0 :: WDATA_07 :: I_DAC_CLKDEL [22:22] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_07_I_DAC_CLKDEL_MASK          0x00400000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_07_I_DAC_CLKDEL_SHIFT         22

/* DS_TUNER_ANACTL_0 :: WDATA_07 :: DAC_ICTL [21:18] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_07_DAC_ICTL_MASK              0x003c0000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_07_DAC_ICTL_SHIFT             18

/* DS_TUNER_ANACTL_0 :: WDATA_07 :: I_DAC_PWRUP [17:17] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_07_I_DAC_PWRUP_MASK           0x00020000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_07_I_DAC_PWRUP_SHIFT          17

/* DS_TUNER_ANACTL_0 :: WDATA_07 :: DAC_GMX [16:13] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_07_DAC_GMX_MASK               0x0001e000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_07_DAC_GMX_SHIFT              13

/* DS_TUNER_ANACTL_0 :: WDATA_07 :: IMF_PWRDN [12:12] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_07_IMF_PWRDN_MASK             0x00001000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_07_IMF_PWRDN_SHIFT            12

/* DS_TUNER_ANACTL_0 :: WDATA_07 :: DAC_IF_HP [11:11] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_07_DAC_IF_HP_MASK             0x00000800
#define BCHP_DS_TUNER_ANACTL_0_WDATA_07_DAC_IF_HP_SHIFT            11

/* DS_TUNER_ANACTL_0 :: WDATA_07 :: DAC_B [10:08] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_07_DAC_B_MASK                 0x00000700
#define BCHP_DS_TUNER_ANACTL_0_WDATA_07_DAC_B_SHIFT                8

/* DS_TUNER_ANACTL_0 :: WDATA_07 :: DACIMFREG_PWRDN [07:07] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_07_DACIMFREG_PWRDN_MASK       0x00000080
#define BCHP_DS_TUNER_ANACTL_0_WDATA_07_DACIMFREG_PWRDN_SHIFT      7

/* DS_TUNER_ANACTL_0 :: WDATA_07 :: IP_PD_LO [06:06] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_07_IP_PD_LO_MASK              0x00000040
#define BCHP_DS_TUNER_ANACTL_0_WDATA_07_IP_PD_LO_SHIFT             6

/* DS_TUNER_ANACTL_0 :: WDATA_07 :: IMF_RES_CAL [05:01] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_07_IMF_RES_CAL_MASK           0x0000003e
#define BCHP_DS_TUNER_ANACTL_0_WDATA_07_IMF_RES_CAL_SHIFT          1

/* DS_TUNER_ANACTL_0 :: WDATA_07 :: IMF_RES_PD [00:00] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_07_IMF_RES_PD_MASK            0x00000001
#define BCHP_DS_TUNER_ANACTL_0_WDATA_07_IMF_RES_PD_SHIFT           0

/***************************************************************************
 *WDATA_14 - Analog Tuner Control Register
 ***************************************************************************/
/* DS_TUNER_ANACTL_0 :: WDATA_14 :: SPARE0 [31:21] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_14_SPARE0_MASK                0xffe00000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_14_SPARE0_SHIFT               21

/* DS_TUNER_ANACTL_0 :: WDATA_14 :: MXRDCO_GMICNTL [20:18] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_14_MXRDCO_GMICNTL_MASK        0x001c0000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_14_MXRDCO_GMICNTL_SHIFT       18

/* DS_TUNER_ANACTL_0 :: WDATA_14 :: MXRDCO_COMP_EN_1P2 [17:17] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_14_MXRDCO_COMP_EN_1P2_MASK    0x00020000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_14_MXRDCO_COMP_EN_1P2_SHIFT   17

/* DS_TUNER_ANACTL_0 :: WDATA_14 :: MXR_DCO_LSB_1P2 [16:14] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_14_MXR_DCO_LSB_1P2_MASK       0x0001c000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_14_MXR_DCO_LSB_1P2_SHIFT      14

/* DS_TUNER_ANACTL_0 :: WDATA_14 :: MXR_DCODAC_PWRDN_1P2 [13:13] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_14_MXR_DCODAC_PWRDN_1P2_MASK  0x00002000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_14_MXR_DCODAC_PWRDN_1P2_SHIFT 13

/* DS_TUNER_ANACTL_0 :: WDATA_14 :: MXRDCO_DATA_Q_1P2 [12:08] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_14_MXRDCO_DATA_Q_1P2_MASK     0x00001f00
#define BCHP_DS_TUNER_ANACTL_0_WDATA_14_MXRDCO_DATA_Q_1P2_SHIFT    8

/* DS_TUNER_ANACTL_0 :: WDATA_14 :: MXRDCO_DATA_I_1P2 [07:03] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_14_MXRDCO_DATA_I_1P2_MASK     0x000000f8
#define BCHP_DS_TUNER_ANACTL_0_WDATA_14_MXRDCO_DATA_I_1P2_SHIFT    3

/* DS_TUNER_ANACTL_0 :: WDATA_14 :: SPARE1 [02:02] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_14_SPARE1_MASK                0x00000004
#define BCHP_DS_TUNER_ANACTL_0_WDATA_14_SPARE1_SHIFT               2

/* DS_TUNER_ANACTL_0 :: WDATA_14 :: DCO_MIX_I_RST [01:01] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_14_DCO_MIX_I_RST_MASK         0x00000002
#define BCHP_DS_TUNER_ANACTL_0_WDATA_14_DCO_MIX_I_RST_SHIFT        1

/* DS_TUNER_ANACTL_0 :: WDATA_14 :: DCO_MIX_Q_QP_RST [00:00] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_14_DCO_MIX_Q_QP_RST_MASK      0x00000001
#define BCHP_DS_TUNER_ANACTL_0_WDATA_14_DCO_MIX_Q_QP_RST_SHIFT     0

/***************************************************************************
 *WDATA_15 - Analog Tuner Control Register
 ***************************************************************************/
/* DS_TUNER_ANACTL_0 :: WDATA_15 :: SPARE0 [31:07] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_15_SPARE0_MASK                0xffffff80
#define BCHP_DS_TUNER_ANACTL_0_WDATA_15_SPARE0_SHIFT               7

/* DS_TUNER_ANACTL_0 :: WDATA_15 :: SPLT_BIAS_CNTL [06:05] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_15_SPLT_BIAS_CNTL_MASK        0x00000060
#define BCHP_DS_TUNER_ANACTL_0_WDATA_15_SPLT_BIAS_CNTL_SHIFT       5

/* DS_TUNER_ANACTL_0 :: WDATA_15 :: MIXBIAS_EN [04:04] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_15_MIXBIAS_EN_MASK            0x00000010
#define BCHP_DS_TUNER_ANACTL_0_WDATA_15_MIXBIAS_EN_SHIFT           4

/* DS_TUNER_ANACTL_0 :: WDATA_15 :: SPLT_PD [03:03] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_15_SPLT_PD_MASK               0x00000008
#define BCHP_DS_TUNER_ANACTL_0_WDATA_15_SPLT_PD_SHIFT              3

/* DS_TUNER_ANACTL_0 :: WDATA_15 :: SPLT_VDD_CNTL [02:00] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_15_SPLT_VDD_CNTL_MASK         0x00000007
#define BCHP_DS_TUNER_ANACTL_0_WDATA_15_SPLT_VDD_CNTL_SHIFT        0

/***************************************************************************
 *WDATA_16 - Analog Tuner Control Register
 ***************************************************************************/
/* DS_TUNER_ANACTL_0 :: WDATA_16 :: USE_TUNER_MIXPLL [31:31] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_16_USE_TUNER_MIXPLL_MASK      0x80000000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_16_USE_TUNER_MIXPLL_SHIFT     31

/* DS_TUNER_ANACTL_0 :: WDATA_16 :: MIXPLL_TUNE [30:30] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_16_MIXPLL_TUNE_MASK           0x40000000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_16_MIXPLL_TUNE_SHIFT          30

/* DS_TUNER_ANACTL_0 :: WDATA_16 :: MIXPLL_PARM_SEL [29:29] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_16_MIXPLL_PARM_SEL_MASK       0x20000000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_16_MIXPLL_PARM_SEL_SHIFT      29

/* DS_TUNER_ANACTL_0 :: WDATA_16 :: reserved_for_eco0 [28:24] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_16_reserved_for_eco0_MASK     0x1f000000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_16_reserved_for_eco0_SHIFT    24

/* DS_TUNER_ANACTL_0 :: WDATA_16 :: CMOS_RESET_DLY [23:16] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_16_CMOS_RESET_DLY_MASK        0x00ff0000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_16_CMOS_RESET_DLY_SHIFT       16

/* DS_TUNER_ANACTL_0 :: WDATA_16 :: MAN_RESET_DLY [15:08] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_16_MAN_RESET_DLY_MASK         0x0000ff00
#define BCHP_DS_TUNER_ANACTL_0_WDATA_16_MAN_RESET_DLY_SHIFT        8

/* DS_TUNER_ANACTL_0 :: WDATA_16 :: CML_RESET_DLY [07:00] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_16_CML_RESET_DLY_MASK         0x000000ff
#define BCHP_DS_TUNER_ANACTL_0_WDATA_16_CML_RESET_DLY_SHIFT        0

/***************************************************************************
 *WDATA_17 - Analog Tuner Control Register
 ***************************************************************************/
/* DS_TUNER_ANACTL_0 :: WDATA_17 :: reserved_for_eco0 [31:16] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_17_reserved_for_eco0_MASK     0xffff0000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_17_reserved_for_eco0_SHIFT    16

/* DS_TUNER_ANACTL_0 :: WDATA_17 :: MIXPLL_TIMER [15:00] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_17_MIXPLL_TIMER_MASK          0x0000ffff
#define BCHP_DS_TUNER_ANACTL_0_WDATA_17_MIXPLL_TIMER_SHIFT         0

/***************************************************************************
 *WDATA_18 - Analog Tuner Control Register
 ***************************************************************************/
/* DS_TUNER_ANACTL_0 :: WDATA_18 :: reserved_for_eco0 [31:19] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_18_reserved_for_eco0_MASK     0xfff80000
#define BCHP_DS_TUNER_ANACTL_0_WDATA_18_reserved_for_eco0_SHIFT    19

/* DS_TUNER_ANACTL_0 :: WDATA_18 :: MIXPLL_TIMER2 [18:00] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_18_MIXPLL_TIMER2_MASK         0x0007ffff
#define BCHP_DS_TUNER_ANACTL_0_WDATA_18_MIXPLL_TIMER2_SHIFT        0

/***************************************************************************
 *WDATA_19 - Analog Tuner Control Register
 ***************************************************************************/
/* DS_TUNER_ANACTL_0 :: WDATA_19 :: SPARE [31:05] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_19_SPARE_MASK                 0xffffffe0
#define BCHP_DS_TUNER_ANACTL_0_WDATA_19_SPARE_SHIFT                5

/* DS_TUNER_ANACTL_0 :: WDATA_19 :: MIXPLL_TUNE_SM [04:02] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_19_MIXPLL_TUNE_SM_MASK        0x0000001c
#define BCHP_DS_TUNER_ANACTL_0_WDATA_19_MIXPLL_TUNE_SM_SHIFT       2

/* DS_TUNER_ANACTL_0 :: WDATA_19 :: MIXPLL_TUNE [01:01] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_19_MIXPLL_TUNE_MASK           0x00000002
#define BCHP_DS_TUNER_ANACTL_0_WDATA_19_MIXPLL_TUNE_SHIFT          1

/* DS_TUNER_ANACTL_0 :: WDATA_19 :: MIXPLL_TUNE_DONE [00:00] */
#define BCHP_DS_TUNER_ANACTL_0_WDATA_19_MIXPLL_TUNE_DONE_MASK      0x00000001
#define BCHP_DS_TUNER_ANACTL_0_WDATA_19_MIXPLL_TUNE_DONE_SHIFT     0

#endif /* #ifndef BCHP_DS_TUNER_ANACTL_0_H__ */

/* End of File */