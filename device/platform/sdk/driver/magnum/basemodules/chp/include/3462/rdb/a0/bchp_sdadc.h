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
 * $brcm_Workfile: bchp_sdadc.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 11/21/11 3:59p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Mon Nov 21 15:38:40 2011
 *                 MD5 Checksum         e7ea6c23a967c588a901532a5078ba2a
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/3462/rdb/a0/bchp_sdadc.h $
 * 
 * Hydra_Software_Devel/1   11/21/11 3:59p farshidf
 * SW3461-98: add the 3462 A0 Header files
 *
 ***************************************************************************/

#ifndef BCHP_SDADC_H__
#define BCHP_SDADC_H__

/***************************************************************************
 *SDADC - UFE SDADC Register Set
 ***************************************************************************/
#define BCHP_SDADC_CTRL_SYS0                     0x000f0200 /* Misc. top level controls */
#define BCHP_SDADC_CTRL_SYS1                     0x000f0204 /* Misc. top level controls */
#define BCHP_SDADC_CTRL_PWRUP                    0x000f0208 /* Power up controls */
#define BCHP_SDADC_CTRL_RESET                    0x000f020c /* Reset controls */
#define BCHP_SDADC_CTRL_ICH                      0x000f0210 /* I-channel ADC controls */
#define BCHP_SDADC_CTRL_QCH                      0x000f0214 /* Q-channel ADC controls */
#define BCHP_SDADC_STATUS_ICH                    0x000f0218 /* I-channel ADC status */
#define BCHP_SDADC_STATUS_QCH                    0x000f021c /* Q-channel ADC status */
#define BCHP_SDADC_STATUS_ICH_CAL_DATA           0x000f0220 /* I-channel ADC calibration data */
#define BCHP_SDADC_STATUS_QCH_CAL_DATA           0x000f0224 /* Q-channel ADC calibration data */
#define BCHP_SDADC_CTRL_ICH_CAL_DATA             0x000f0228 /* I-channel ADC calibration controls */
#define BCHP_SDADC_CTRL_QCH_CAL_DATA             0x000f022c /* Q-channel ADC calibration controls */

/***************************************************************************
 *CTRL_SYS0 - Misc. top level controls
 ***************************************************************************/
/* SDADC :: CTRL_SYS0 :: i_adcclk_reset [31:31] */
#define BCHP_SDADC_CTRL_SYS0_i_adcclk_reset_MASK                   0x80000000
#define BCHP_SDADC_CTRL_SYS0_i_adcclk_reset_SHIFT                  31
#define BCHP_SDADC_CTRL_SYS0_i_adcclk_reset_DEFAULT                0

/* SDADC :: CTRL_SYS0 :: i_adcclk_inv [30:30] */
#define BCHP_SDADC_CTRL_SYS0_i_adcclk_inv_MASK                     0x40000000
#define BCHP_SDADC_CTRL_SYS0_i_adcclk_inv_SHIFT                    30
#define BCHP_SDADC_CTRL_SYS0_i_adcclk_inv_DEFAULT                  0

/* SDADC :: CTRL_SYS0 :: i_ctl_adc_bias [29:10] */
#define BCHP_SDADC_CTRL_SYS0_i_ctl_adc_bias_MASK                   0x3ffffc00
#define BCHP_SDADC_CTRL_SYS0_i_ctl_adc_bias_SHIFT                  10
#define BCHP_SDADC_CTRL_SYS0_i_ctl_adc_bias_DEFAULT                0

/* SDADC :: CTRL_SYS0 :: i_ctl_vcm [09:08] */
#define BCHP_SDADC_CTRL_SYS0_i_ctl_vcm_MASK                        0x00000300
#define BCHP_SDADC_CTRL_SYS0_i_ctl_vcm_SHIFT                       8
#define BCHP_SDADC_CTRL_SYS0_i_ctl_vcm_DEFAULT                     0

/* SDADC :: CTRL_SYS0 :: i_ctl_rc [07:05] */
#define BCHP_SDADC_CTRL_SYS0_i_ctl_rc_MASK                         0x000000e0
#define BCHP_SDADC_CTRL_SYS0_i_ctl_rc_SHIFT                        5
#define BCHP_SDADC_CTRL_SYS0_i_ctl_rc_DEFAULT                      0

/* SDADC :: CTRL_SYS0 :: i_ctl_flash_fullscale [04:03] */
#define BCHP_SDADC_CTRL_SYS0_i_ctl_flash_fullscale_MASK            0x00000018
#define BCHP_SDADC_CTRL_SYS0_i_ctl_flash_fullscale_SHIFT           3
#define BCHP_SDADC_CTRL_SYS0_i_ctl_flash_fullscale_DEFAULT         0

/* SDADC :: CTRL_SYS0 :: i_ctl_adc_gain [02:00] */
#define BCHP_SDADC_CTRL_SYS0_i_ctl_adc_gain_MASK                   0x00000007
#define BCHP_SDADC_CTRL_SYS0_i_ctl_adc_gain_SHIFT                  0
#define BCHP_SDADC_CTRL_SYS0_i_ctl_adc_gain_DEFAULT                0

/***************************************************************************
 *CTRL_SYS1 - Misc. top level controls
 ***************************************************************************/
/* SDADC :: CTRL_SYS1 :: i_ctl_adc_dly [31:26] */
#define BCHP_SDADC_CTRL_SYS1_i_ctl_adc_dly_MASK                    0xfc000000
#define BCHP_SDADC_CTRL_SYS1_i_ctl_adc_dly_SHIFT                   26
#define BCHP_SDADC_CTRL_SYS1_i_ctl_adc_dly_DEFAULT                 0

/* SDADC :: CTRL_SYS1 :: i_ctl_adc_spare [25:16] */
#define BCHP_SDADC_CTRL_SYS1_i_ctl_adc_spare_MASK                  0x03ff0000
#define BCHP_SDADC_CTRL_SYS1_i_ctl_adc_spare_SHIFT                 16
#define BCHP_SDADC_CTRL_SYS1_i_ctl_adc_spare_DEFAULT               0

/* SDADC :: CTRL_SYS1 :: i_clk_sel [15:15] */
#define BCHP_SDADC_CTRL_SYS1_i_clk_sel_MASK                        0x00008000
#define BCHP_SDADC_CTRL_SYS1_i_clk_sel_SHIFT                       15
#define BCHP_SDADC_CTRL_SYS1_i_clk_sel_DEFAULT                     0

/* SDADC :: CTRL_SYS1 :: i_rst_n [14:14] */
#define BCHP_SDADC_CTRL_SYS1_i_rst_n_MASK                          0x00004000
#define BCHP_SDADC_CTRL_SYS1_i_rst_n_SHIFT                         14
#define BCHP_SDADC_CTRL_SYS1_i_rst_n_DEFAULT                       1

/* SDADC :: CTRL_SYS1 :: i_cntr_enable [13:13] */
#define BCHP_SDADC_CTRL_SYS1_i_cntr_enable_MASK                    0x00002000
#define BCHP_SDADC_CTRL_SYS1_i_cntr_enable_SHIFT                   13
#define BCHP_SDADC_CTRL_SYS1_i_cntr_enable_DEFAULT                 0

/* SDADC :: CTRL_SYS1 :: i_cntr_select [12:12] */
#define BCHP_SDADC_CTRL_SYS1_i_cntr_select_MASK                    0x00001000
#define BCHP_SDADC_CTRL_SYS1_i_cntr_select_SHIFT                   12
#define BCHP_SDADC_CTRL_SYS1_i_cntr_select_DEFAULT                 0

/* SDADC :: CTRL_SYS1 :: reserved0 [11:00] */
#define BCHP_SDADC_CTRL_SYS1_reserved0_MASK                        0x00000fff
#define BCHP_SDADC_CTRL_SYS1_reserved0_SHIFT                       0

/***************************************************************************
 *CTRL_PWRUP - Power up controls
 ***************************************************************************/
/* SDADC :: CTRL_PWRUP :: reserved0 [31:02] */
#define BCHP_SDADC_CTRL_PWRUP_reserved0_MASK                       0xfffffffc
#define BCHP_SDADC_CTRL_PWRUP_reserved0_SHIFT                      2

/* SDADC :: CTRL_PWRUP :: i_Ich_pwrup [01:01] */
#define BCHP_SDADC_CTRL_PWRUP_i_Ich_pwrup_MASK                     0x00000002
#define BCHP_SDADC_CTRL_PWRUP_i_Ich_pwrup_SHIFT                    1
#define BCHP_SDADC_CTRL_PWRUP_i_Ich_pwrup_DEFAULT                  0

/* SDADC :: CTRL_PWRUP :: i_Qch_pwrup [00:00] */
#define BCHP_SDADC_CTRL_PWRUP_i_Qch_pwrup_MASK                     0x00000001
#define BCHP_SDADC_CTRL_PWRUP_i_Qch_pwrup_SHIFT                    0
#define BCHP_SDADC_CTRL_PWRUP_i_Qch_pwrup_DEFAULT                  0

/***************************************************************************
 *CTRL_RESET - Reset controls
 ***************************************************************************/
/* SDADC :: CTRL_RESET :: reserved0 [31:02] */
#define BCHP_SDADC_CTRL_RESET_reserved0_MASK                       0xfffffffc
#define BCHP_SDADC_CTRL_RESET_reserved0_SHIFT                      2

/* SDADC :: CTRL_RESET :: i_Ich_reset [01:01] */
#define BCHP_SDADC_CTRL_RESET_i_Ich_reset_MASK                     0x00000002
#define BCHP_SDADC_CTRL_RESET_i_Ich_reset_SHIFT                    1
#define BCHP_SDADC_CTRL_RESET_i_Ich_reset_DEFAULT                  1

/* SDADC :: CTRL_RESET :: i_Qch_reset [00:00] */
#define BCHP_SDADC_CTRL_RESET_i_Qch_reset_MASK                     0x00000001
#define BCHP_SDADC_CTRL_RESET_i_Qch_reset_SHIFT                    0
#define BCHP_SDADC_CTRL_RESET_i_Qch_reset_DEFAULT                  1

/***************************************************************************
 *CTRL_ICH - I-channel ADC controls
 ***************************************************************************/
/* SDADC :: CTRL_ICH :: reserved0 [31:06] */
#define BCHP_SDADC_CTRL_ICH_reserved0_MASK                         0xffffffc0
#define BCHP_SDADC_CTRL_ICH_reserved0_SHIFT                        6

/* SDADC :: CTRL_ICH :: i_Ich_scram_off [05:05] */
#define BCHP_SDADC_CTRL_ICH_i_Ich_scram_off_MASK                   0x00000020
#define BCHP_SDADC_CTRL_ICH_i_Ich_scram_off_SHIFT                  5
#define BCHP_SDADC_CTRL_ICH_i_Ich_scram_off_DEFAULT                0

/* SDADC :: CTRL_ICH :: i_Ich_flash_calsel [04:04] */
#define BCHP_SDADC_CTRL_ICH_i_Ich_flash_calsel_MASK                0x00000010
#define BCHP_SDADC_CTRL_ICH_i_Ich_flash_calsel_SHIFT               4
#define BCHP_SDADC_CTRL_ICH_i_Ich_flash_calsel_DEFAULT             0

/* SDADC :: CTRL_ICH :: i_Ich_flash_resetCal [03:03] */
#define BCHP_SDADC_CTRL_ICH_i_Ich_flash_resetCal_MASK              0x00000008
#define BCHP_SDADC_CTRL_ICH_i_Ich_flash_resetCal_SHIFT             3
#define BCHP_SDADC_CTRL_ICH_i_Ich_flash_resetCal_DEFAULT           0

/* SDADC :: CTRL_ICH :: i_Ich_flash_cal_on [02:02] */
#define BCHP_SDADC_CTRL_ICH_i_Ich_flash_cal_on_MASK                0x00000004
#define BCHP_SDADC_CTRL_ICH_i_Ich_flash_cal_on_SHIFT               2
#define BCHP_SDADC_CTRL_ICH_i_Ich_flash_cal_on_DEFAULT             0

/* SDADC :: CTRL_ICH :: reserved1 [01:00] */
#define BCHP_SDADC_CTRL_ICH_reserved1_MASK                         0x00000003
#define BCHP_SDADC_CTRL_ICH_reserved1_SHIFT                        0

/***************************************************************************
 *CTRL_QCH - Q-channel ADC controls
 ***************************************************************************/
/* SDADC :: CTRL_QCH :: reserved0 [31:06] */
#define BCHP_SDADC_CTRL_QCH_reserved0_MASK                         0xffffffc0
#define BCHP_SDADC_CTRL_QCH_reserved0_SHIFT                        6

/* SDADC :: CTRL_QCH :: i_Qch_scram_off [05:05] */
#define BCHP_SDADC_CTRL_QCH_i_Qch_scram_off_MASK                   0x00000020
#define BCHP_SDADC_CTRL_QCH_i_Qch_scram_off_SHIFT                  5
#define BCHP_SDADC_CTRL_QCH_i_Qch_scram_off_DEFAULT                0

/* SDADC :: CTRL_QCH :: i_Qch_flash_calsel [04:04] */
#define BCHP_SDADC_CTRL_QCH_i_Qch_flash_calsel_MASK                0x00000010
#define BCHP_SDADC_CTRL_QCH_i_Qch_flash_calsel_SHIFT               4
#define BCHP_SDADC_CTRL_QCH_i_Qch_flash_calsel_DEFAULT             0

/* SDADC :: CTRL_QCH :: i_Qch_flash_resetCal [03:03] */
#define BCHP_SDADC_CTRL_QCH_i_Qch_flash_resetCal_MASK              0x00000008
#define BCHP_SDADC_CTRL_QCH_i_Qch_flash_resetCal_SHIFT             3
#define BCHP_SDADC_CTRL_QCH_i_Qch_flash_resetCal_DEFAULT           0

/* SDADC :: CTRL_QCH :: i_Qch_flash_cal_on [02:02] */
#define BCHP_SDADC_CTRL_QCH_i_Qch_flash_cal_on_MASK                0x00000004
#define BCHP_SDADC_CTRL_QCH_i_Qch_flash_cal_on_SHIFT               2
#define BCHP_SDADC_CTRL_QCH_i_Qch_flash_cal_on_DEFAULT             0

/* SDADC :: CTRL_QCH :: reserved1 [01:00] */
#define BCHP_SDADC_CTRL_QCH_reserved1_MASK                         0x00000003
#define BCHP_SDADC_CTRL_QCH_reserved1_SHIFT                        0

/***************************************************************************
 *STATUS_ICH - I-channel ADC status
 ***************************************************************************/
/* SDADC :: STATUS_ICH :: reserved0 [31:09] */
#define BCHP_SDADC_STATUS_ICH_reserved0_MASK                       0xfffffe00
#define BCHP_SDADC_STATUS_ICH_reserved0_SHIFT                      9

/* SDADC :: STATUS_ICH :: o_ch_master_done [08:08] */
#define BCHP_SDADC_STATUS_ICH_o_ch_master_done_MASK                0x00000100
#define BCHP_SDADC_STATUS_ICH_o_ch_master_done_SHIFT               8

/* SDADC :: STATUS_ICH :: o_ch_flash_cal_done [07:00] */
#define BCHP_SDADC_STATUS_ICH_o_ch_flash_cal_done_MASK             0x000000ff
#define BCHP_SDADC_STATUS_ICH_o_ch_flash_cal_done_SHIFT            0

/***************************************************************************
 *STATUS_QCH - Q-channel ADC status
 ***************************************************************************/
/* SDADC :: STATUS_QCH :: reserved0 [31:09] */
#define BCHP_SDADC_STATUS_QCH_reserved0_MASK                       0xfffffe00
#define BCHP_SDADC_STATUS_QCH_reserved0_SHIFT                      9

/* SDADC :: STATUS_QCH :: o_ch_master_done [08:08] */
#define BCHP_SDADC_STATUS_QCH_o_ch_master_done_MASK                0x00000100
#define BCHP_SDADC_STATUS_QCH_o_ch_master_done_SHIFT               8

/* SDADC :: STATUS_QCH :: o_ch_flash_cal_done [07:00] */
#define BCHP_SDADC_STATUS_QCH_o_ch_flash_cal_done_MASK             0x000000ff
#define BCHP_SDADC_STATUS_QCH_o_ch_flash_cal_done_SHIFT            0

/***************************************************************************
 *STATUS_ICH_CAL_DATA - I-channel ADC calibration data
 ***************************************************************************/
/* SDADC :: STATUS_ICH_CAL_DATA :: reserved0 [31:24] */
#define BCHP_SDADC_STATUS_ICH_CAL_DATA_reserved0_MASK              0xff000000
#define BCHP_SDADC_STATUS_ICH_CAL_DATA_reserved0_SHIFT             24

/* SDADC :: STATUS_ICH_CAL_DATA :: ch_flash_caldata [23:00] */
#define BCHP_SDADC_STATUS_ICH_CAL_DATA_ch_flash_caldata_MASK       0x00ffffff
#define BCHP_SDADC_STATUS_ICH_CAL_DATA_ch_flash_caldata_SHIFT      0
#define BCHP_SDADC_STATUS_ICH_CAL_DATA_ch_flash_caldata_DEFAULT    0

/***************************************************************************
 *STATUS_QCH_CAL_DATA - Q-channel ADC calibration data
 ***************************************************************************/
/* SDADC :: STATUS_QCH_CAL_DATA :: reserved0 [31:24] */
#define BCHP_SDADC_STATUS_QCH_CAL_DATA_reserved0_MASK              0xff000000
#define BCHP_SDADC_STATUS_QCH_CAL_DATA_reserved0_SHIFT             24

/* SDADC :: STATUS_QCH_CAL_DATA :: ch_flash_caldata [23:00] */
#define BCHP_SDADC_STATUS_QCH_CAL_DATA_ch_flash_caldata_MASK       0x00ffffff
#define BCHP_SDADC_STATUS_QCH_CAL_DATA_ch_flash_caldata_SHIFT      0
#define BCHP_SDADC_STATUS_QCH_CAL_DATA_ch_flash_caldata_DEFAULT    0

/***************************************************************************
 *CTRL_ICH_CAL_DATA - I-channel ADC calibration controls
 ***************************************************************************/
/* SDADC :: CTRL_ICH_CAL_DATA :: reserved0 [31:24] */
#define BCHP_SDADC_CTRL_ICH_CAL_DATA_reserved0_MASK                0xff000000
#define BCHP_SDADC_CTRL_ICH_CAL_DATA_reserved0_SHIFT               24

/* SDADC :: CTRL_ICH_CAL_DATA :: i_ctl_ch_flash_offset [23:00] */
#define BCHP_SDADC_CTRL_ICH_CAL_DATA_i_ctl_ch_flash_offset_MASK    0x00ffffff
#define BCHP_SDADC_CTRL_ICH_CAL_DATA_i_ctl_ch_flash_offset_SHIFT   0
#define BCHP_SDADC_CTRL_ICH_CAL_DATA_i_ctl_ch_flash_offset_DEFAULT 4793490

/***************************************************************************
 *CTRL_QCH_CAL_DATA - Q-channel ADC calibration controls
 ***************************************************************************/
/* SDADC :: CTRL_QCH_CAL_DATA :: reserved0 [31:24] */
#define BCHP_SDADC_CTRL_QCH_CAL_DATA_reserved0_MASK                0xff000000
#define BCHP_SDADC_CTRL_QCH_CAL_DATA_reserved0_SHIFT               24

/* SDADC :: CTRL_QCH_CAL_DATA :: i_ctl_ch_flash_offset [23:00] */
#define BCHP_SDADC_CTRL_QCH_CAL_DATA_i_ctl_ch_flash_offset_MASK    0x00ffffff
#define BCHP_SDADC_CTRL_QCH_CAL_DATA_i_ctl_ch_flash_offset_SHIFT   0
#define BCHP_SDADC_CTRL_QCH_CAL_DATA_i_ctl_ch_flash_offset_DEFAULT 4793490

#endif /* #ifndef BCHP_SDADC_H__ */

/* End of File */