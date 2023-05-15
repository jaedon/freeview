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
 * $brcm_Workfile: bchp_genet_0_ext.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 12/12/11 2:55p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Mon Dec 12 12:00:25 2011
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
 * $brcm_Log: /magnum/basemodules/chp/7346/rdb/b0/bchp_genet_0_ext.h $
 * 
 * Hydra_Software_Devel/2   12/12/11 2:55p pntruong
 * SW7346-143: Resynced with central rdb.
 *
 ***************************************************************************/

#ifndef BCHP_GENET_0_EXT_H__
#define BCHP_GENET_0_EXT_H__

/***************************************************************************
 *GENET_0_EXT
 ***************************************************************************/
#define BCHP_GENET_0_EXT_PWR_MGNT                0x00430080 /* EPHY Power Management Register */
#define BCHP_GENET_0_EXT_EMCG_CNTRL              0x00430084 /* ENDT EMCG Control Register */
#define BCHP_GENET_0_EXT_TEST_CNTRL              0x00430088 /* Test Control Register */
#define BCHP_GENET_0_EXT_RGMII_OOB_CNTRL         0x0043008c /* RGMII OOB Control Register */
#define BCHP_GENET_0_EXT_RGMII_IB_STATUS         0x00430090 /* RGMII InBand Status Register */
#define BCHP_GENET_0_EXT_RGMII_LED_CNTRL         0x00430094 /* RGMII LED Control Register */
#define BCHP_GENET_0_EXT_CLOCK_SEL_CNTRL         0x00430098 /* GENET CORE CLOCK and GMII CLOCK SELECT Control Register */

/***************************************************************************
 *PWR_MGNT - EPHY Power Management Register
 ***************************************************************************/
/* GENET_0_EXT :: PWR_MGNT :: reserved0 [31:22] */
#define BCHP_GENET_0_EXT_PWR_MGNT_reserved0_MASK                   0xffc00000
#define BCHP_GENET_0_EXT_PWR_MGNT_reserved0_SHIFT                  22

/* GENET_0_EXT :: PWR_MGNT :: ext_super_isolate [21:21] */
#define BCHP_GENET_0_EXT_PWR_MGNT_ext_super_isolate_MASK           0x00200000
#define BCHP_GENET_0_EXT_PWR_MGNT_ext_super_isolate_SHIFT          21
#define BCHP_GENET_0_EXT_PWR_MGNT_ext_super_isolate_DEFAULT        0x00000000

/* GENET_0_EXT :: PWR_MGNT :: ext_pwr_down_phy_en [20:20] */
#define BCHP_GENET_0_EXT_PWR_MGNT_ext_pwr_down_phy_en_MASK         0x00100000
#define BCHP_GENET_0_EXT_PWR_MGNT_ext_pwr_down_phy_en_SHIFT        20
#define BCHP_GENET_0_EXT_PWR_MGNT_ext_pwr_down_phy_en_DEFAULT      0x00000000

/* GENET_0_EXT :: PWR_MGNT :: ext_pwr_down_phy_rd [19:19] */
#define BCHP_GENET_0_EXT_PWR_MGNT_ext_pwr_down_phy_rd_MASK         0x00080000
#define BCHP_GENET_0_EXT_PWR_MGNT_ext_pwr_down_phy_rd_SHIFT        19
#define BCHP_GENET_0_EXT_PWR_MGNT_ext_pwr_down_phy_rd_DEFAULT      0x00000000

/* GENET_0_EXT :: PWR_MGNT :: ext_pwr_down_phy_sd [18:18] */
#define BCHP_GENET_0_EXT_PWR_MGNT_ext_pwr_down_phy_sd_MASK         0x00040000
#define BCHP_GENET_0_EXT_PWR_MGNT_ext_pwr_down_phy_sd_SHIFT        18
#define BCHP_GENET_0_EXT_PWR_MGNT_ext_pwr_down_phy_sd_DEFAULT      0x00000000

/* GENET_0_EXT :: PWR_MGNT :: ext_pwr_down_phy_rx [17:17] */
#define BCHP_GENET_0_EXT_PWR_MGNT_ext_pwr_down_phy_rx_MASK         0x00020000
#define BCHP_GENET_0_EXT_PWR_MGNT_ext_pwr_down_phy_rx_SHIFT        17
#define BCHP_GENET_0_EXT_PWR_MGNT_ext_pwr_down_phy_rx_DEFAULT      0x00000000

/* GENET_0_EXT :: PWR_MGNT :: ext_pwr_down_phy_tx [16:16] */
#define BCHP_GENET_0_EXT_PWR_MGNT_ext_pwr_down_phy_tx_MASK         0x00010000
#define BCHP_GENET_0_EXT_PWR_MGNT_ext_pwr_down_phy_tx_SHIFT        16
#define BCHP_GENET_0_EXT_PWR_MGNT_ext_pwr_down_phy_tx_DEFAULT      0x00000000

/* GENET_0_EXT :: PWR_MGNT :: reserved1 [15:13] */
#define BCHP_GENET_0_EXT_PWR_MGNT_reserved1_MASK                   0x0000e000
#define BCHP_GENET_0_EXT_PWR_MGNT_reserved1_SHIFT                  13

/* GENET_0_EXT :: PWR_MGNT :: energy_det_mask [12:12] */
#define BCHP_GENET_0_EXT_PWR_MGNT_energy_det_mask_MASK             0x00001000
#define BCHP_GENET_0_EXT_PWR_MGNT_energy_det_mask_SHIFT            12
#define BCHP_GENET_0_EXT_PWR_MGNT_energy_det_mask_DEFAULT          0x00000000

/* GENET_0_EXT :: PWR_MGNT :: reserved2 [11:10] */
#define BCHP_GENET_0_EXT_PWR_MGNT_reserved2_MASK                   0x00000c00
#define BCHP_GENET_0_EXT_PWR_MGNT_reserved2_SHIFT                  10

/* GENET_0_EXT :: PWR_MGNT :: mdc_ex_clk_dis_ld [09:09] */
#define BCHP_GENET_0_EXT_PWR_MGNT_mdc_ex_clk_dis_ld_MASK           0x00000200
#define BCHP_GENET_0_EXT_PWR_MGNT_mdc_ex_clk_dis_ld_SHIFT          9
#define BCHP_GENET_0_EXT_PWR_MGNT_mdc_ex_clk_dis_ld_DEFAULT        0x00000001

/* GENET_0_EXT :: PWR_MGNT :: reset [08:08] */
#define BCHP_GENET_0_EXT_PWR_MGNT_reset_MASK                       0x00000100
#define BCHP_GENET_0_EXT_PWR_MGNT_reset_SHIFT                      8
#define BCHP_GENET_0_EXT_PWR_MGNT_reset_DEFAULT                    0x00000000

/* GENET_0_EXT :: PWR_MGNT :: reserved3 [07:06] */
#define BCHP_GENET_0_EXT_PWR_MGNT_reserved3_MASK                   0x000000c0
#define BCHP_GENET_0_EXT_PWR_MGNT_reserved3_SHIFT                  6

/* GENET_0_EXT :: PWR_MGNT :: iddq_from_phy [05:05] */
#define BCHP_GENET_0_EXT_PWR_MGNT_iddq_from_phy_MASK               0x00000020
#define BCHP_GENET_0_EXT_PWR_MGNT_iddq_from_phy_SHIFT              5
#define BCHP_GENET_0_EXT_PWR_MGNT_iddq_from_phy_DEFAULT            0x00000000

/* GENET_0_EXT :: PWR_MGNT :: energy_det [04:04] */
#define BCHP_GENET_0_EXT_PWR_MGNT_energy_det_MASK                  0x00000010
#define BCHP_GENET_0_EXT_PWR_MGNT_energy_det_SHIFT                 4
#define BCHP_GENET_0_EXT_PWR_MGNT_energy_det_DEFAULT               0x00000000

/* GENET_0_EXT :: PWR_MGNT :: auto_pwr_dn_en_ld [03:03] */
#define BCHP_GENET_0_EXT_PWR_MGNT_auto_pwr_dn_en_ld_MASK           0x00000008
#define BCHP_GENET_0_EXT_PWR_MGNT_auto_pwr_dn_en_ld_SHIFT          3
#define BCHP_GENET_0_EXT_PWR_MGNT_auto_pwr_dn_en_ld_DEFAULT        0x00000000

/* GENET_0_EXT :: PWR_MGNT :: ext_pwr_down_phy [02:02] */
#define BCHP_GENET_0_EXT_PWR_MGNT_ext_pwr_down_phy_MASK            0x00000004
#define BCHP_GENET_0_EXT_PWR_MGNT_ext_pwr_down_phy_SHIFT           2
#define BCHP_GENET_0_EXT_PWR_MGNT_ext_pwr_down_phy_DEFAULT         0x00000000

/* GENET_0_EXT :: PWR_MGNT :: ext_pwr_down_dll [01:01] */
#define BCHP_GENET_0_EXT_PWR_MGNT_ext_pwr_down_dll_MASK            0x00000002
#define BCHP_GENET_0_EXT_PWR_MGNT_ext_pwr_down_dll_SHIFT           1
#define BCHP_GENET_0_EXT_PWR_MGNT_ext_pwr_down_dll_DEFAULT         0x00000000

/* GENET_0_EXT :: PWR_MGNT :: ext_pwr_down_bias [00:00] */
#define BCHP_GENET_0_EXT_PWR_MGNT_ext_pwr_down_bias_MASK           0x00000001
#define BCHP_GENET_0_EXT_PWR_MGNT_ext_pwr_down_bias_SHIFT          0
#define BCHP_GENET_0_EXT_PWR_MGNT_ext_pwr_down_bias_DEFAULT        0x00000000

/***************************************************************************
 *EMCG_CNTRL - ENDT EMCG Control Register
 ***************************************************************************/
/* GENET_0_EXT :: EMCG_CNTRL :: reserved0 [31:05] */
#define BCHP_GENET_0_EXT_EMCG_CNTRL_reserved0_MASK                 0xffffffe0
#define BCHP_GENET_0_EXT_EMCG_CNTRL_reserved0_SHIFT                5

/* GENET_0_EXT :: EMCG_CNTRL :: ephy_clk_sel [04:04] */
#define BCHP_GENET_0_EXT_EMCG_CNTRL_ephy_clk_sel_MASK              0x00000010
#define BCHP_GENET_0_EXT_EMCG_CNTRL_ephy_clk_sel_SHIFT             4
#define BCHP_GENET_0_EXT_EMCG_CNTRL_ephy_clk_sel_DEFAULT           0x00000000

/* GENET_0_EXT :: EMCG_CNTRL :: reserved1 [03:01] */
#define BCHP_GENET_0_EXT_EMCG_CNTRL_reserved1_MASK                 0x0000000e
#define BCHP_GENET_0_EXT_EMCG_CNTRL_reserved1_SHIFT                1

/* GENET_0_EXT :: EMCG_CNTRL :: ext_ck25_en [00:00] */
#define BCHP_GENET_0_EXT_EMCG_CNTRL_ext_ck25_en_MASK               0x00000001
#define BCHP_GENET_0_EXT_EMCG_CNTRL_ext_ck25_en_SHIFT              0
#define BCHP_GENET_0_EXT_EMCG_CNTRL_ext_ck25_en_DEFAULT            0x00000000

/***************************************************************************
 *TEST_CNTRL - Test Control Register
 ***************************************************************************/
/* GENET_0_EXT :: TEST_CNTRL :: reserved0 [31:15] */
#define BCHP_GENET_0_EXT_TEST_CNTRL_reserved0_MASK                 0xffff8000
#define BCHP_GENET_0_EXT_TEST_CNTRL_reserved0_SHIFT                15

/* GENET_0_EXT :: TEST_CNTRL :: pll_lock [14:14] */
#define BCHP_GENET_0_EXT_TEST_CNTRL_pll_lock_MASK                  0x00004000
#define BCHP_GENET_0_EXT_TEST_CNTRL_pll_lock_SHIFT                 14
#define BCHP_GENET_0_EXT_TEST_CNTRL_pll_lock_DEFAULT               0x00000000

/* GENET_0_EXT :: TEST_CNTRL :: atest_25MHz_en [13:13] */
#define BCHP_GENET_0_EXT_TEST_CNTRL_atest_25MHz_en_MASK            0x00002000
#define BCHP_GENET_0_EXT_TEST_CNTRL_atest_25MHz_en_SHIFT           13
#define BCHP_GENET_0_EXT_TEST_CNTRL_atest_25MHz_en_DEFAULT         0x00000000

/* GENET_0_EXT :: TEST_CNTRL :: reclock_en [12:12] */
#define BCHP_GENET_0_EXT_TEST_CNTRL_reclock_en_MASK                0x00001000
#define BCHP_GENET_0_EXT_TEST_CNTRL_reclock_en_SHIFT               12
#define BCHP_GENET_0_EXT_TEST_CNTRL_reclock_en_DEFAULT             0x00000000

/* GENET_0_EXT :: TEST_CNTRL :: ephy_scan_mode_en [11:11] */
#define BCHP_GENET_0_EXT_TEST_CNTRL_ephy_scan_mode_en_MASK         0x00000800
#define BCHP_GENET_0_EXT_TEST_CNTRL_ephy_scan_mode_en_SHIFT        11
#define BCHP_GENET_0_EXT_TEST_CNTRL_ephy_scan_mode_en_DEFAULT      0x00000000

/* GENET_0_EXT :: TEST_CNTRL :: ephy_test_mode [10:10] */
#define BCHP_GENET_0_EXT_TEST_CNTRL_ephy_test_mode_MASK            0x00000400
#define BCHP_GENET_0_EXT_TEST_CNTRL_ephy_test_mode_SHIFT           10
#define BCHP_GENET_0_EXT_TEST_CNTRL_ephy_test_mode_DEFAULT         0x00000000

/* GENET_0_EXT :: TEST_CNTRL :: ephy_standalone_mode [09:09] */
#define BCHP_GENET_0_EXT_TEST_CNTRL_ephy_standalone_mode_MASK      0x00000200
#define BCHP_GENET_0_EXT_TEST_CNTRL_ephy_standalone_mode_SHIFT     9
#define BCHP_GENET_0_EXT_TEST_CNTRL_ephy_standalone_mode_DEFAULT   0x00000000

/* GENET_0_EXT :: TEST_CNTRL :: ephy_dll_byp [08:08] */
#define BCHP_GENET_0_EXT_TEST_CNTRL_ephy_dll_byp_MASK              0x00000100
#define BCHP_GENET_0_EXT_TEST_CNTRL_ephy_dll_byp_SHIFT             8
#define BCHP_GENET_0_EXT_TEST_CNTRL_ephy_dll_byp_DEFAULT           0x00000000

/* GENET_0_EXT :: TEST_CNTRL :: ephy_dummy0 [07:07] */
#define BCHP_GENET_0_EXT_TEST_CNTRL_ephy_dummy0_MASK               0x00000080
#define BCHP_GENET_0_EXT_TEST_CNTRL_ephy_dummy0_SHIFT              7
#define BCHP_GENET_0_EXT_TEST_CNTRL_ephy_dummy0_DEFAULT            0x00000000

/* GENET_0_EXT :: TEST_CNTRL :: ephy_testen [06:06] */
#define BCHP_GENET_0_EXT_TEST_CNTRL_ephy_testen_MASK               0x00000040
#define BCHP_GENET_0_EXT_TEST_CNTRL_ephy_testen_SHIFT              6
#define BCHP_GENET_0_EXT_TEST_CNTRL_ephy_testen_DEFAULT            0x00000000

/* GENET_0_EXT :: TEST_CNTRL :: ephy_ext_test_mode [05:01] */
#define BCHP_GENET_0_EXT_TEST_CNTRL_ephy_ext_test_mode_MASK        0x0000003e
#define BCHP_GENET_0_EXT_TEST_CNTRL_ephy_ext_test_mode_SHIFT       1
#define BCHP_GENET_0_EXT_TEST_CNTRL_ephy_ext_test_mode_DEFAULT     0x00000000

/* GENET_0_EXT :: TEST_CNTRL :: ephy_reset [00:00] */
#define BCHP_GENET_0_EXT_TEST_CNTRL_ephy_reset_MASK                0x00000001
#define BCHP_GENET_0_EXT_TEST_CNTRL_ephy_reset_SHIFT               0
#define BCHP_GENET_0_EXT_TEST_CNTRL_ephy_reset_DEFAULT             0x00000000

/***************************************************************************
 *RGMII_OOB_CNTRL - RGMII OOB Control Register
 ***************************************************************************/
/* GENET_0_EXT :: RGMII_OOB_CNTRL :: reserved0 [31:17] */
#define BCHP_GENET_0_EXT_RGMII_OOB_CNTRL_reserved0_MASK            0xfffe0000
#define BCHP_GENET_0_EXT_RGMII_OOB_CNTRL_reserved0_SHIFT           17

/* GENET_0_EXT :: RGMII_OOB_CNTRL :: id_mode_dis [16:16] */
#define BCHP_GENET_0_EXT_RGMII_OOB_CNTRL_id_mode_dis_MASK          0x00010000
#define BCHP_GENET_0_EXT_RGMII_OOB_CNTRL_id_mode_dis_SHIFT         16
#define BCHP_GENET_0_EXT_RGMII_OOB_CNTRL_id_mode_dis_DEFAULT       0x00000000

/* GENET_0_EXT :: RGMII_OOB_CNTRL :: txclk_dly [15:12] */
#define BCHP_GENET_0_EXT_RGMII_OOB_CNTRL_txclk_dly_MASK            0x0000f000
#define BCHP_GENET_0_EXT_RGMII_OOB_CNTRL_txclk_dly_SHIFT           12
#define BCHP_GENET_0_EXT_RGMII_OOB_CNTRL_txclk_dly_DEFAULT         0x00000008

/* GENET_0_EXT :: RGMII_OOB_CNTRL :: rxclk_dly [11:08] */
#define BCHP_GENET_0_EXT_RGMII_OOB_CNTRL_rxclk_dly_MASK            0x00000f00
#define BCHP_GENET_0_EXT_RGMII_OOB_CNTRL_rxclk_dly_SHIFT           8
#define BCHP_GENET_0_EXT_RGMII_OOB_CNTRL_rxclk_dly_DEFAULT         0x00000008

/* GENET_0_EXT :: RGMII_OOB_CNTRL :: reserved1 [07:06] */
#define BCHP_GENET_0_EXT_RGMII_OOB_CNTRL_reserved1_MASK            0x000000c0
#define BCHP_GENET_0_EXT_RGMII_OOB_CNTRL_reserved1_SHIFT           6

/* GENET_0_EXT :: RGMII_OOB_CNTRL :: oob_dis [05:05] */
#define BCHP_GENET_0_EXT_RGMII_OOB_CNTRL_oob_dis_MASK              0x00000020
#define BCHP_GENET_0_EXT_RGMII_OOB_CNTRL_oob_dis_SHIFT             5
#define BCHP_GENET_0_EXT_RGMII_OOB_CNTRL_oob_dis_DEFAULT           0x00000000

/* GENET_0_EXT :: RGMII_OOB_CNTRL :: link [04:04] */
#define BCHP_GENET_0_EXT_RGMII_OOB_CNTRL_link_MASK                 0x00000010
#define BCHP_GENET_0_EXT_RGMII_OOB_CNTRL_link_SHIFT                4
#define BCHP_GENET_0_EXT_RGMII_OOB_CNTRL_link_DEFAULT              0x00000000

/* GENET_0_EXT :: RGMII_OOB_CNTRL :: reserved2 [03:00] */
#define BCHP_GENET_0_EXT_RGMII_OOB_CNTRL_reserved2_MASK            0x0000000f
#define BCHP_GENET_0_EXT_RGMII_OOB_CNTRL_reserved2_SHIFT           0

/***************************************************************************
 *RGMII_IB_STATUS - RGMII InBand Status Register
 ***************************************************************************/
/* GENET_0_EXT :: RGMII_IB_STATUS :: reserved0 [31:04] */
#define BCHP_GENET_0_EXT_RGMII_IB_STATUS_reserved0_MASK            0xfffffff0
#define BCHP_GENET_0_EXT_RGMII_IB_STATUS_reserved0_SHIFT           4

/* GENET_0_EXT :: RGMII_IB_STATUS :: link [03:03] */
#define BCHP_GENET_0_EXT_RGMII_IB_STATUS_link_MASK                 0x00000008
#define BCHP_GENET_0_EXT_RGMII_IB_STATUS_link_SHIFT                3
#define BCHP_GENET_0_EXT_RGMII_IB_STATUS_link_DEFAULT              0x00000000

/* GENET_0_EXT :: RGMII_IB_STATUS :: duplex [02:02] */
#define BCHP_GENET_0_EXT_RGMII_IB_STATUS_duplex_MASK               0x00000004
#define BCHP_GENET_0_EXT_RGMII_IB_STATUS_duplex_SHIFT              2
#define BCHP_GENET_0_EXT_RGMII_IB_STATUS_duplex_DEFAULT            0x00000000

/* GENET_0_EXT :: RGMII_IB_STATUS :: speed [01:00] */
#define BCHP_GENET_0_EXT_RGMII_IB_STATUS_speed_MASK                0x00000003
#define BCHP_GENET_0_EXT_RGMII_IB_STATUS_speed_SHIFT               0
#define BCHP_GENET_0_EXT_RGMII_IB_STATUS_speed_DEFAULT             0x00000000

/***************************************************************************
 *RGMII_LED_CNTRL - RGMII LED Control Register
 ***************************************************************************/
/* GENET_0_EXT :: RGMII_LED_CNTRL :: rgmii_led_on_cyc_count [31:16] */
#define BCHP_GENET_0_EXT_RGMII_LED_CNTRL_rgmii_led_on_cyc_count_MASK 0xffff0000
#define BCHP_GENET_0_EXT_RGMII_LED_CNTRL_rgmii_led_on_cyc_count_SHIFT 16
#define BCHP_GENET_0_EXT_RGMII_LED_CNTRL_rgmii_led_on_cyc_count_DEFAULT 0x00000670

/* GENET_0_EXT :: RGMII_LED_CNTRL :: rgmii_led_off_cyc_count [15:00] */
#define BCHP_GENET_0_EXT_RGMII_LED_CNTRL_rgmii_led_off_cyc_count_MASK 0x0000ffff
#define BCHP_GENET_0_EXT_RGMII_LED_CNTRL_rgmii_led_off_cyc_count_SHIFT 0
#define BCHP_GENET_0_EXT_RGMII_LED_CNTRL_rgmii_led_off_cyc_count_DEFAULT 0x00000670

/***************************************************************************
 *CLOCK_SEL_CNTRL - GENET CORE CLOCK and GMII CLOCK SELECT Control Register
 ***************************************************************************/
/* GENET_0_EXT :: CLOCK_SEL_CNTRL :: reserved0 [31:03] */
#define BCHP_GENET_0_EXT_CLOCK_SEL_CNTRL_reserved0_MASK            0xfffffff8
#define BCHP_GENET_0_EXT_CLOCK_SEL_CNTRL_reserved0_SHIFT           3

/* GENET_0_EXT :: CLOCK_SEL_CNTRL :: clk_sel [02:02] */
#define BCHP_GENET_0_EXT_CLOCK_SEL_CNTRL_clk_sel_MASK              0x00000004
#define BCHP_GENET_0_EXT_CLOCK_SEL_CNTRL_clk_sel_SHIFT             2
#define BCHP_GENET_0_EXT_CLOCK_SEL_CNTRL_clk_sel_DEFAULT           0x00000000

/* GENET_0_EXT :: CLOCK_SEL_CNTRL :: core_clk_sel [01:01] */
#define BCHP_GENET_0_EXT_CLOCK_SEL_CNTRL_core_clk_sel_MASK         0x00000002
#define BCHP_GENET_0_EXT_CLOCK_SEL_CNTRL_core_clk_sel_SHIFT        1
#define BCHP_GENET_0_EXT_CLOCK_SEL_CNTRL_core_clk_sel_DEFAULT      0x00000000

/* GENET_0_EXT :: CLOCK_SEL_CNTRL :: gmii_clk_sel [00:00] */
#define BCHP_GENET_0_EXT_CLOCK_SEL_CNTRL_gmii_clk_sel_MASK         0x00000001
#define BCHP_GENET_0_EXT_CLOCK_SEL_CNTRL_gmii_clk_sel_SHIFT        0
#define BCHP_GENET_0_EXT_CLOCK_SEL_CNTRL_gmii_clk_sel_DEFAULT      0x00000000

#endif /* #ifndef BCHP_GENET_0_EXT_H__ */

/* End of File */