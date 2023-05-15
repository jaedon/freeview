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
 * $brcm_Workfile: bchp_swb_wfe_ana_u1.h $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 3/8/11 5:41p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Tue Mar  8 15:04:52 2011
 *                 MD5 Checksum         68d5047ca356bd3c027b61b17a21a5d8
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/4528/rdb/a0/bchp_swb_wfe_ana_u1.h $
 * 
 * Hydra_Software_Devel/3   3/8/11 5:41p ronchan
 * HW4528-2: sync up with local RDB
 *
 ***************************************************************************/

#ifndef BCHP_SWB_WFE_ANA_U1_H__
#define BCHP_SWB_WFE_ANA_U1_H__

/***************************************************************************
 *SWB_WFE_ANA_U1 - ANA WBADC Register Set
 ***************************************************************************/
#define BCHP_SWB_WFE_ANA_U1_RFFE_SIGR01_IN0      0x000a0600 /* RFFE_sigR01 */
#define BCHP_SWB_WFE_ANA_U1_RFFE_WRITER01_IN0    0x000a0604 /* RFFE_writeR01 */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL0            0x000a0608 /* adc_cntl0 */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL1            0x000a060c /* adc_cntl1 */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL2            0x000a0610 /* adc_cntl2 */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL3            0x000a0614 /* adc_cntl3 */
#define BCHP_SWB_WFE_ANA_U1_WBADC_CAL_CNTL_IN0   0x000a0618 /* wbadc_cal_cntl */
#define BCHP_SWB_WFE_ANA_U1_WBADC_SYS_CNTL_IN0   0x000a061c /* wbadc_sys_cntl */
#define BCHP_SWB_WFE_ANA_U1_WBADC_TIMING_CNTL_IN0 0x000a0620 /* wbadc_timing_cntl */
#define BCHP_SWB_WFE_ANA_U1_WBADC_TOP_CNTL_IN0   0x000a0624 /* wbadc_top_cntl */
#define BCHP_SWB_WFE_ANA_U1_REG_CTRL             0x000a0628 /* register access control */
#define BCHP_SWB_WFE_ANA_U1_SW_SPARE0            0x000a062c /* sw_spare0 */
#define BCHP_SWB_WFE_ANA_U1_SW_SPARE1            0x000a0630 /* sw_spare1 */

/***************************************************************************
 *RFFE_SIGR01_IN0 - RFFE_sigR01
 ***************************************************************************/
/* SWB_WFE_ANA_U1 :: RFFE_SIGR01_IN0 :: RFFE_spare4 [31:24] */
#define BCHP_SWB_WFE_ANA_U1_RFFE_SIGR01_IN0_RFFE_spare4_MASK       0xff000000
#define BCHP_SWB_WFE_ANA_U1_RFFE_SIGR01_IN0_RFFE_spare4_SHIFT      24

/* SWB_WFE_ANA_U1 :: RFFE_SIGR01_IN0 :: LNA2_SF_BIAS [23:19] */
#define BCHP_SWB_WFE_ANA_U1_RFFE_SIGR01_IN0_LNA2_SF_BIAS_MASK      0x00f80000
#define BCHP_SWB_WFE_ANA_U1_RFFE_SIGR01_IN0_LNA2_SF_BIAS_SHIFT     19

/* SWB_WFE_ANA_U1 :: RFFE_SIGR01_IN0 :: LNA2_GAIN_BIAS [18:14] */
#define BCHP_SWB_WFE_ANA_U1_RFFE_SIGR01_IN0_LNA2_GAIN_BIAS_MASK    0x0007c000
#define BCHP_SWB_WFE_ANA_U1_RFFE_SIGR01_IN0_LNA2_GAIN_BIAS_SHIFT   14

/* SWB_WFE_ANA_U1 :: RFFE_SIGR01_IN0 :: RFPGA_RESET_DIGITAL [13:13] */
#define BCHP_SWB_WFE_ANA_U1_RFFE_SIGR01_IN0_RFPGA_RESET_DIGITAL_MASK 0x00002000
#define BCHP_SWB_WFE_ANA_U1_RFFE_SIGR01_IN0_RFPGA_RESET_DIGITAL_SHIFT 13

/* SWB_WFE_ANA_U1 :: RFFE_SIGR01_IN0 :: RFFE_spare [12:07] */
#define BCHP_SWB_WFE_ANA_U1_RFFE_SIGR01_IN0_RFFE_spare_MASK        0x00001f80
#define BCHP_SWB_WFE_ANA_U1_RFFE_SIGR01_IN0_RFFE_spare_SHIFT       7

/* SWB_WFE_ANA_U1 :: RFFE_SIGR01_IN0 :: RFPGA_BIAS_CTRL [06:02] */
#define BCHP_SWB_WFE_ANA_U1_RFFE_SIGR01_IN0_RFPGA_BIAS_CTRL_MASK   0x0000007c
#define BCHP_SWB_WFE_ANA_U1_RFFE_SIGR01_IN0_RFPGA_BIAS_CTRL_SHIFT  2

/* SWB_WFE_ANA_U1 :: RFFE_SIGR01_IN0 :: spare_sigR01_1 [01:01] */
#define BCHP_SWB_WFE_ANA_U1_RFFE_SIGR01_IN0_spare_sigR01_1_MASK    0x00000002
#define BCHP_SWB_WFE_ANA_U1_RFFE_SIGR01_IN0_spare_sigR01_1_SHIFT   1

/* SWB_WFE_ANA_U1 :: RFFE_SIGR01_IN0 :: spare_sigR01_0 [00:00] */
#define BCHP_SWB_WFE_ANA_U1_RFFE_SIGR01_IN0_spare_sigR01_0_MASK    0x00000001
#define BCHP_SWB_WFE_ANA_U1_RFFE_SIGR01_IN0_spare_sigR01_0_SHIFT   0

/***************************************************************************
 *RFFE_WRITER01_IN0 - RFFE_writeR01
 ***************************************************************************/
/* SWB_WFE_ANA_U1 :: RFFE_WRITER01_IN0 :: REG_1P3_OUT_SW_CNTL [31:31] */
#define BCHP_SWB_WFE_ANA_U1_RFFE_WRITER01_IN0_REG_1P3_OUT_SW_CNTL_MASK 0x80000000
#define BCHP_SWB_WFE_ANA_U1_RFFE_WRITER01_IN0_REG_1P3_OUT_SW_CNTL_SHIFT 31

/* SWB_WFE_ANA_U1 :: RFFE_WRITER01_IN0 :: REG_1P0_OUT_SW_CNTL [30:30] */
#define BCHP_SWB_WFE_ANA_U1_RFFE_WRITER01_IN0_REG_1P0_OUT_SW_CNTL_MASK 0x40000000
#define BCHP_SWB_WFE_ANA_U1_RFFE_WRITER01_IN0_REG_1P0_OUT_SW_CNTL_SHIFT 30

/* SWB_WFE_ANA_U1 :: RFFE_WRITER01_IN0 :: RFFE_CAL_Itail_CNTL [29:29] */
#define BCHP_SWB_WFE_ANA_U1_RFFE_WRITER01_IN0_RFFE_CAL_Itail_CNTL_MASK 0x20000000
#define BCHP_SWB_WFE_ANA_U1_RFFE_WRITER01_IN0_RFFE_CAL_Itail_CNTL_SHIFT 29

/* SWB_WFE_ANA_U1 :: RFFE_WRITER01_IN0 :: CAL_CM [28:27] */
#define BCHP_SWB_WFE_ANA_U1_RFFE_WRITER01_IN0_CAL_CM_MASK          0x18000000
#define BCHP_SWB_WFE_ANA_U1_RFFE_WRITER01_IN0_CAL_CM_SHIFT         27

/* SWB_WFE_ANA_U1 :: RFFE_WRITER01_IN0 :: spare2 [26:26] */
#define BCHP_SWB_WFE_ANA_U1_RFFE_WRITER01_IN0_spare2_MASK          0x04000000
#define BCHP_SWB_WFE_ANA_U1_RFFE_WRITER01_IN0_spare2_SHIFT         26

/* SWB_WFE_ANA_U1 :: RFFE_WRITER01_IN0 :: REG_pwrup [25:25] */
#define BCHP_SWB_WFE_ANA_U1_RFFE_WRITER01_IN0_REG_pwrup_MASK       0x02000000
#define BCHP_SWB_WFE_ANA_U1_RFFE_WRITER01_IN0_REG_pwrup_SHIFT      25

/* SWB_WFE_ANA_U1 :: RFFE_WRITER01_IN0 :: REG_1P3_CTRL [24:22] */
#define BCHP_SWB_WFE_ANA_U1_RFFE_WRITER01_IN0_REG_1P3_CTRL_MASK    0x01c00000
#define BCHP_SWB_WFE_ANA_U1_RFFE_WRITER01_IN0_REG_1P3_CTRL_SHIFT   22

/* SWB_WFE_ANA_U1 :: RFFE_WRITER01_IN0 :: REG_1P0_CTRL [21:19] */
#define BCHP_SWB_WFE_ANA_U1_RFFE_WRITER01_IN0_REG_1P0_CTRL_MASK    0x00380000
#define BCHP_SWB_WFE_ANA_U1_RFFE_WRITER01_IN0_REG_1P0_CTRL_SHIFT   19

/* SWB_WFE_ANA_U1 :: RFFE_WRITER01_IN0 :: spare3 [18:17] */
#define BCHP_SWB_WFE_ANA_U1_RFFE_WRITER01_IN0_spare3_MASK          0x00060000
#define BCHP_SWB_WFE_ANA_U1_RFFE_WRITER01_IN0_spare3_SHIFT         17

/* SWB_WFE_ANA_U1 :: RFFE_WRITER01_IN0 :: LNA2_CAP_CNTL_GAIN_PEAKING [16:15] */
#define BCHP_SWB_WFE_ANA_U1_RFFE_WRITER01_IN0_LNA2_CAP_CNTL_GAIN_PEAKING_MASK 0x00018000
#define BCHP_SWB_WFE_ANA_U1_RFFE_WRITER01_IN0_LNA2_CAP_CNTL_GAIN_PEAKING_SHIFT 15

/* SWB_WFE_ANA_U1 :: RFFE_WRITER01_IN0 :: spare_writeR_14 [14:14] */
#define BCHP_SWB_WFE_ANA_U1_RFFE_WRITER01_IN0_spare_writeR_14_MASK 0x00004000
#define BCHP_SWB_WFE_ANA_U1_RFFE_WRITER01_IN0_spare_writeR_14_SHIFT 14

/* SWB_WFE_ANA_U1 :: RFFE_WRITER01_IN0 :: VCM_OUT_SW_CNTL [13:13] */
#define BCHP_SWB_WFE_ANA_U1_RFFE_WRITER01_IN0_VCM_OUT_SW_CNTL_MASK 0x00002000
#define BCHP_SWB_WFE_ANA_U1_RFFE_WRITER01_IN0_VCM_OUT_SW_CNTL_SHIFT 13

/* SWB_WFE_ANA_U1 :: RFFE_WRITER01_IN0 :: LNA2_CM_CTRL [12:10] */
#define BCHP_SWB_WFE_ANA_U1_RFFE_WRITER01_IN0_LNA2_CM_CTRL_MASK    0x00001c00
#define BCHP_SWB_WFE_ANA_U1_RFFE_WRITER01_IN0_LNA2_CM_CTRL_SHIFT   10

/* SWB_WFE_ANA_U1 :: RFFE_WRITER01_IN0 :: LNA2_BIAS_SEL [09:09] */
#define BCHP_SWB_WFE_ANA_U1_RFFE_WRITER01_IN0_LNA2_BIAS_SEL_MASK   0x00000200
#define BCHP_SWB_WFE_ANA_U1_RFFE_WRITER01_IN0_LNA2_BIAS_SEL_SHIFT  9

/* SWB_WFE_ANA_U1 :: RFFE_WRITER01_IN0 :: spare_RFFE_8_7 [08:07] */
#define BCHP_SWB_WFE_ANA_U1_RFFE_WRITER01_IN0_spare_RFFE_8_7_MASK  0x00000180
#define BCHP_SWB_WFE_ANA_U1_RFFE_WRITER01_IN0_spare_RFFE_8_7_SHIFT 7

/* SWB_WFE_ANA_U1 :: RFFE_WRITER01_IN0 :: Bandgap_ON_sel [06:06] */
#define BCHP_SWB_WFE_ANA_U1_RFFE_WRITER01_IN0_Bandgap_ON_sel_MASK  0x00000040
#define BCHP_SWB_WFE_ANA_U1_RFFE_WRITER01_IN0_Bandgap_ON_sel_SHIFT 6

/* SWB_WFE_ANA_U1 :: RFFE_WRITER01_IN0 :: DPM_MODE_EN [05:05] */
#define BCHP_SWB_WFE_ANA_U1_RFFE_WRITER01_IN0_DPM_MODE_EN_MASK     0x00000020
#define BCHP_SWB_WFE_ANA_U1_RFFE_WRITER01_IN0_DPM_MODE_EN_SHIFT    5

/* SWB_WFE_ANA_U1 :: RFFE_WRITER01_IN0 :: RFPGA_MODE_SEL [04:04] */
#define BCHP_SWB_WFE_ANA_U1_RFFE_WRITER01_IN0_RFPGA_MODE_SEL_MASK  0x00000010
#define BCHP_SWB_WFE_ANA_U1_RFFE_WRITER01_IN0_RFPGA_MODE_SEL_SHIFT 4

/* SWB_WFE_ANA_U1 :: RFFE_WRITER01_IN0 :: RFPGA_CM_CTRL [03:00] */
#define BCHP_SWB_WFE_ANA_U1_RFFE_WRITER01_IN0_RFPGA_CM_CTRL_MASK   0x0000000f
#define BCHP_SWB_WFE_ANA_U1_RFFE_WRITER01_IN0_RFPGA_CM_CTRL_SHIFT  0

/***************************************************************************
 *ADC_CNTL0 - adc_cntl0
 ***************************************************************************/
/* SWB_WFE_ANA_U1 :: ADC_CNTL0 :: Full_Scale_Control [31:30] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL0_Full_Scale_Control_MASK      0xc0000000
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL0_Full_Scale_Control_SHIFT     30

/* SWB_WFE_ANA_U1 :: ADC_CNTL0 :: ADC_MDAC3_Cap_Calibration [29:24] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL0_ADC_MDAC3_Cap_Calibration_MASK 0x3f000000
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL0_ADC_MDAC3_Cap_Calibration_SHIFT 24

/* SWB_WFE_ANA_U1 :: ADC_CNTL0 :: Digisum1_Output_Format_MSB_bit [23:23] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL0_Digisum1_Output_Format_MSB_bit_MASK 0x00800000
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL0_Digisum1_Output_Format_MSB_bit_SHIFT 23

/* SWB_WFE_ANA_U1 :: ADC_CNTL0 :: Flash_input_timing_adjust_control [22:22] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL0_Flash_input_timing_adjust_control_MASK 0x00400000
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL0_Flash_input_timing_adjust_control_SHIFT 22

/* SWB_WFE_ANA_U1 :: ADC_CNTL0 :: MDAC3_input_timing_adjust_control [21:21] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL0_MDAC3_input_timing_adjust_control_MASK 0x00200000
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL0_MDAC3_input_timing_adjust_control_SHIFT 21

/* SWB_WFE_ANA_U1 :: ADC_CNTL0 :: MDAC2_input_timing_adjust_control [20:20] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL0_MDAC2_input_timing_adjust_control_MASK 0x00100000
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL0_MDAC2_input_timing_adjust_control_SHIFT 20

/* SWB_WFE_ANA_U1 :: ADC_CNTL0 :: MDAC1_input_timing_adjust_control [19:19] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL0_MDAC1_input_timing_adjust_control_MASK 0x00080000
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL0_MDAC1_input_timing_adjust_control_SHIFT 19

/* SWB_WFE_ANA_U1 :: ADC_CNTL0 :: Flash_input_sampling_edge_selection [18:18] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL0_Flash_input_sampling_edge_selection_MASK 0x00040000
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL0_Flash_input_sampling_edge_selection_SHIFT 18

/* SWB_WFE_ANA_U1 :: ADC_CNTL0 :: MDAC3_input_sampling_edge_selection [17:17] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL0_MDAC3_input_sampling_edge_selection_MASK 0x00020000
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL0_MDAC3_input_sampling_edge_selection_SHIFT 17

/* SWB_WFE_ANA_U1 :: ADC_CNTL0 :: MDAC2_input_sampling_edge_selection [16:16] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL0_MDAC2_input_sampling_edge_selection_MASK 0x00010000
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL0_MDAC2_input_sampling_edge_selection_SHIFT 16

/* SWB_WFE_ANA_U1 :: ADC_CNTL0 :: MDAC1_input_sampling_edge_selection [15:15] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL0_MDAC1_input_sampling_edge_selection_MASK 0x00008000
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL0_MDAC1_input_sampling_edge_selection_SHIFT 15

/* SWB_WFE_ANA_U1 :: ADC_CNTL0 :: Saturation_bypass [14:14] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL0_Saturation_bypass_MASK       0x00004000
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL0_Saturation_bypass_SHIFT      14

/* SWB_WFE_ANA_U1 :: ADC_CNTL0 :: Digisum1_Output_Format_LSB_bit [13:13] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL0_Digisum1_Output_Format_LSB_bit_MASK 0x00002000
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL0_Digisum1_Output_Format_LSB_bit_SHIFT 13

/* SWB_WFE_ANA_U1 :: ADC_CNTL0 :: Sub_sampling_ratio_control [12:11] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL0_Sub_sampling_ratio_control_MASK 0x00001800
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL0_Sub_sampling_ratio_control_SHIFT 11

/* SWB_WFE_ANA_U1 :: ADC_CNTL0 :: Invert_internal_fast_clock_phase [10:10] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL0_Invert_internal_fast_clock_phase_MASK 0x00000400
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL0_Invert_internal_fast_clock_phase_SHIFT 10

/* SWB_WFE_ANA_U1 :: ADC_CNTL0 :: Invert_dmux_output_clock_phase [09:09] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL0_Invert_dmux_output_clock_phase_MASK 0x00000200
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL0_Invert_dmux_output_clock_phase_SHIFT 9

/* SWB_WFE_ANA_U1 :: ADC_CNTL0 :: Disable_dmux_clock [08:08] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL0_Disable_dmux_clock_MASK      0x00000100
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL0_Disable_dmux_clock_SHIFT     8

/* SWB_WFE_ANA_U1 :: ADC_CNTL0 :: Reset_noise_cancellation_subsamping [07:07] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL0_Reset_noise_cancellation_subsamping_MASK 0x00000080
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL0_Reset_noise_cancellation_subsamping_SHIFT 7

/* SWB_WFE_ANA_U1 :: ADC_CNTL0 :: Noise_cancellation_control_bits [06:04] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL0_Noise_cancellation_control_bits_MASK 0x00000070
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL0_Noise_cancellation_control_bits_SHIFT 4

/* SWB_WFE_ANA_U1 :: ADC_CNTL0 :: Data_mask_control [03:02] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL0_Data_mask_control_MASK       0x0000000c
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL0_Data_mask_control_SHIFT      2

/* SWB_WFE_ANA_U1 :: ADC_CNTL0 :: Output_data_format [01:01] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL0_Output_data_format_MASK      0x00000002
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL0_Output_data_format_SHIFT     1

/* SWB_WFE_ANA_U1 :: ADC_CNTL0 :: Invert_output_clock_phase [00:00] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL0_Invert_output_clock_phase_MASK 0x00000001
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL0_Invert_output_clock_phase_SHIFT 0

/***************************************************************************
 *ADC_CNTL1 - adc_cntl1
 ***************************************************************************/
/* SWB_WFE_ANA_U1 :: ADC_CNTL1 :: AMP_cm_cntl [31:30] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL1_AMP_cm_cntl_MASK             0xc0000000
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL1_AMP_cm_cntl_SHIFT            30

/* SWB_WFE_ANA_U1 :: ADC_CNTL1 :: MDAC3_AMP_Power_Control [29:27] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL1_MDAC3_AMP_Power_Control_MASK 0x38000000
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL1_MDAC3_AMP_Power_Control_SHIFT 27

/* SWB_WFE_ANA_U1 :: ADC_CNTL1 :: Vcmi_Control [26:24] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL1_Vcmi_Control_MASK            0x07000000
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL1_Vcmi_Control_SHIFT           24

/* SWB_WFE_ANA_U1 :: ADC_CNTL1 :: MDAC2_AMP_Power [23:21] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL1_MDAC2_AMP_Power_MASK         0x00e00000
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL1_MDAC2_AMP_Power_SHIFT        21

/* SWB_WFE_ANA_U1 :: ADC_CNTL1 :: MDAC1_AMP_Power [20:18] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL1_MDAC1_AMP_Power_MASK         0x001c0000
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL1_MDAC1_AMP_Power_SHIFT        18

/* SWB_WFE_ANA_U1 :: ADC_CNTL1 :: SHA_Power [17:15] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL1_SHA_Power_MASK               0x00038000
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL1_SHA_Power_SHIFT              15

/* SWB_WFE_ANA_U1 :: ADC_CNTL1 :: BUF_Power [14:12] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL1_BUF_Power_MASK               0x00007000
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL1_BUF_Power_SHIFT              12

/* SWB_WFE_ANA_U1 :: ADC_CNTL1 :: VCOM_Buffer_Power [11:10] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL1_VCOM_Buffer_Power_MASK       0x00000c00
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL1_VCOM_Buffer_Power_SHIFT      10

/* SWB_WFE_ANA_U1 :: ADC_CNTL1 :: REFGEN_refb_block [09:08] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL1_REFGEN_refb_block_MASK       0x00000300
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL1_REFGEN_refb_block_SHIFT      8

/* SWB_WFE_ANA_U1 :: ADC_CNTL1 :: REFGEN_reft_block [07:06] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL1_REFGEN_reft_block_MASK       0x000000c0
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL1_REFGEN_reft_block_SHIFT      6

/* SWB_WFE_ANA_U1 :: ADC_CNTL1 :: Iref_for_MDAC_flash3 [05:04] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL1_Iref_for_MDAC_flash3_MASK    0x00000030
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL1_Iref_for_MDAC_flash3_SHIFT   4

/* SWB_WFE_ANA_U1 :: ADC_CNTL1 :: Iref_MDAC1_flash3 [03:02] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL1_Iref_MDAC1_flash3_MASK       0x0000000c
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL1_Iref_MDAC1_flash3_SHIFT      2

/* SWB_WFE_ANA_U1 :: ADC_CNTL1 :: Iref_for_FLASH6 [01:00] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL1_Iref_for_FLASH6_MASK         0x00000003
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL1_Iref_for_FLASH6_SHIFT        0

/***************************************************************************
 *ADC_CNTL2 - adc_cntl2
 ***************************************************************************/
/* SWB_WFE_ANA_U1 :: ADC_CNTL2 :: FLASH6_Tcntl [31:30] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL2_FLASH6_Tcntl_MASK            0xc0000000
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL2_FLASH6_Tcntl_SHIFT           30

/* SWB_WFE_ANA_U1 :: ADC_CNTL2 :: MDAC23_Tcntl [29:28] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL2_MDAC23_Tcntl_MASK            0x30000000
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL2_MDAC23_Tcntl_SHIFT           28

/* SWB_WFE_ANA_U1 :: ADC_CNTL2 :: Vcom_ext_voltage [27:26] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL2_Vcom_ext_voltage_MASK        0x0c000000
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL2_Vcom_ext_voltage_SHIFT       26

/* SWB_WFE_ANA_U1 :: ADC_CNTL2 :: Dvdd15_common_control [25:24] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL2_Dvdd15_common_control_MASK   0x03000000
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL2_Dvdd15_common_control_SHIFT  24

/* SWB_WFE_ANA_U1 :: ADC_CNTL2 :: LDO_DVDD15_DVSS_VOLTAGE [23:22] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL2_LDO_DVDD15_DVSS_VOLTAGE_MASK 0x00c00000
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL2_LDO_DVDD15_DVSS_VOLTAGE_SHIFT 22

/* SWB_WFE_ANA_U1 :: ADC_CNTL2 :: Test_port_selection [21:19] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL2_Test_port_selection_MASK     0x00380000
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL2_Test_port_selection_SHIFT    19

/* SWB_WFE_ANA_U1 :: ADC_CNTL2 :: Clock_level_shifter_bias_control [18:18] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL2_Clock_level_shifter_bias_control_MASK 0x00040000
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL2_Clock_level_shifter_bias_control_SHIFT 18

/* SWB_WFE_ANA_U1 :: ADC_CNTL2 :: MDAC1_Tcntl [17:16] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL2_MDAC1_Tcntl_MASK             0x00030000
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL2_MDAC1_Tcntl_SHIFT            16

/* SWB_WFE_ANA_U1 :: ADC_CNTL2 :: Non_overlap_timing_control [15:13] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL2_Non_overlap_timing_control_MASK 0x0000e000
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL2_Non_overlap_timing_control_SHIFT 13

/* SWB_WFE_ANA_U1 :: ADC_CNTL2 :: Increas_pcasc_voltage [12:11] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL2_Increas_pcasc_voltage_MASK   0x00001800
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL2_Increas_pcasc_voltage_SHIFT  11

/* SWB_WFE_ANA_U1 :: ADC_CNTL2 :: increase_cmi_voltage [10:09] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL2_increase_cmi_voltage_MASK    0x00000600
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL2_increase_cmi_voltage_SHIFT   9

/* SWB_WFE_ANA_U1 :: ADC_CNTL2 :: LDO_DVDD_refernce_current [08:06] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL2_LDO_DVDD_refernce_current_MASK 0x000001c0
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL2_LDO_DVDD_refernce_current_SHIFT 6

/* SWB_WFE_ANA_U1 :: ADC_CNTL2 :: AMP_BIAS_VCM_current [05:03] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL2_AMP_BIAS_VCM_current_MASK    0x00000038
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL2_AMP_BIAS_VCM_current_SHIFT   3

/* SWB_WFE_ANA_U1 :: ADC_CNTL2 :: I_Ambias_Cmi [02:00] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL2_I_Ambias_Cmi_MASK            0x00000007
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL2_I_Ambias_Cmi_SHIFT           0

/***************************************************************************
 *ADC_CNTL3 - adc_cntl3
 ***************************************************************************/
/* SWB_WFE_ANA_U1 :: ADC_CNTL3 :: ADC3_reserved [31:18] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL3_ADC3_reserved_MASK           0xfffc0000
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL3_ADC3_reserved_SHIFT          18

/* SWB_WFE_ANA_U1 :: ADC_CNTL3 :: ADC_reserved [17:17] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL3_ADC_reserved_MASK            0x00020000
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL3_ADC_reserved_SHIFT           17

/* SWB_WFE_ANA_U1 :: ADC_CNTL3 :: Clock_rate_control [16:16] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL3_Clock_rate_control_MASK      0x00010000
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL3_Clock_rate_control_SHIFT     16

/* SWB_WFE_ANA_U1 :: ADC_CNTL3 :: Clock_Presentation_Detection [15:15] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL3_Clock_Presentation_Detection_MASK 0x00008000
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL3_Clock_Presentation_Detection_SHIFT 15

/* SWB_WFE_ANA_U1 :: ADC_CNTL3 :: Gain_Calibration_MDAC3 [14:14] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL3_Gain_Calibration_MDAC3_MASK  0x00004000
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL3_Gain_Calibration_MDAC3_SHIFT 14

/* SWB_WFE_ANA_U1 :: ADC_CNTL3 :: Calibration_MDAC2 [13:13] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL3_Calibration_MDAC2_MASK       0x00002000
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL3_Calibration_MDAC2_SHIFT      13

/* SWB_WFE_ANA_U1 :: ADC_CNTL3 :: Calibration_MDAC1 [12:12] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL3_Calibration_MDAC1_MASK       0x00001000
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL3_Calibration_MDAC1_SHIFT      12

/* SWB_WFE_ANA_U1 :: ADC_CNTL3 :: MDAC2_swap_value [11:06] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL3_MDAC2_swap_value_MASK        0x00000fc0
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL3_MDAC2_swap_value_SHIFT       6

/* SWB_WFE_ANA_U1 :: ADC_CNTL3 :: MDAC1_swap_value [05:00] */
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL3_MDAC1_swap_value_MASK        0x0000003f
#define BCHP_SWB_WFE_ANA_U1_ADC_CNTL3_MDAC1_swap_value_SHIFT       0

/***************************************************************************
 *WBADC_CAL_CNTL_IN0 - wbadc_cal_cntl
 ***************************************************************************/
/* SWB_WFE_ANA_U1 :: WBADC_CAL_CNTL_IN0 :: MDAC_EQU_ADC0_ENABLE [31:31] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_CAL_CNTL_IN0_MDAC_EQU_ADC0_ENABLE_MASK 0x80000000
#define BCHP_SWB_WFE_ANA_U1_WBADC_CAL_CNTL_IN0_MDAC_EQU_ADC0_ENABLE_SHIFT 31

/* SWB_WFE_ANA_U1 :: WBADC_CAL_CNTL_IN0 :: MDAC_EQU_ADC1_ENABLE [30:30] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_CAL_CNTL_IN0_MDAC_EQU_ADC1_ENABLE_MASK 0x40000000
#define BCHP_SWB_WFE_ANA_U1_WBADC_CAL_CNTL_IN0_MDAC_EQU_ADC1_ENABLE_SHIFT 30

/* SWB_WFE_ANA_U1 :: WBADC_CAL_CNTL_IN0 :: MDAC_EQU_ADC2_ENABLE [29:29] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_CAL_CNTL_IN0_MDAC_EQU_ADC2_ENABLE_MASK 0x20000000
#define BCHP_SWB_WFE_ANA_U1_WBADC_CAL_CNTL_IN0_MDAC_EQU_ADC2_ENABLE_SHIFT 29

/* SWB_WFE_ANA_U1 :: WBADC_CAL_CNTL_IN0 :: MDAC_EQU_ADC3_ENABLE [28:28] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_CAL_CNTL_IN0_MDAC_EQU_ADC3_ENABLE_MASK 0x10000000
#define BCHP_SWB_WFE_ANA_U1_WBADC_CAL_CNTL_IN0_MDAC_EQU_ADC3_ENABLE_SHIFT 28

/* SWB_WFE_ANA_U1 :: WBADC_CAL_CNTL_IN0 :: MDAC_EQU_ADC4_ENABLE [27:27] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_CAL_CNTL_IN0_MDAC_EQU_ADC4_ENABLE_MASK 0x08000000
#define BCHP_SWB_WFE_ANA_U1_WBADC_CAL_CNTL_IN0_MDAC_EQU_ADC4_ENABLE_SHIFT 27

/* SWB_WFE_ANA_U1 :: WBADC_CAL_CNTL_IN0 :: CAL_CNTL_RESERVED [26:20] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_CAL_CNTL_IN0_CAL_CNTL_RESERVED_MASK 0x07f00000
#define BCHP_SWB_WFE_ANA_U1_WBADC_CAL_CNTL_IN0_CAL_CNTL_RESERVED_SHIFT 20

/* SWB_WFE_ANA_U1 :: WBADC_CAL_CNTL_IN0 :: BIAS_EXT_R_CNTL [19:16] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_CAL_CNTL_IN0_BIAS_EXT_R_CNTL_MASK 0x000f0000
#define BCHP_SWB_WFE_ANA_U1_WBADC_CAL_CNTL_IN0_BIAS_EXT_R_CNTL_SHIFT 16

/* SWB_WFE_ANA_U1 :: WBADC_CAL_CNTL_IN0 :: SET_SHA_OUTPUT_TO_ZERO [15:15] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_CAL_CNTL_IN0_SET_SHA_OUTPUT_TO_ZERO_MASK 0x00008000
#define BCHP_SWB_WFE_ANA_U1_WBADC_CAL_CNTL_IN0_SET_SHA_OUTPUT_TO_ZERO_SHIFT 15

/* SWB_WFE_ANA_U1 :: WBADC_CAL_CNTL_IN0 :: MDAC3_EQU_CAL_CNTL [14:10] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_CAL_CNTL_IN0_MDAC3_EQU_CAL_CNTL_MASK 0x00007c00
#define BCHP_SWB_WFE_ANA_U1_WBADC_CAL_CNTL_IN0_MDAC3_EQU_CAL_CNTL_SHIFT 10

/* SWB_WFE_ANA_U1 :: WBADC_CAL_CNTL_IN0 :: MDAC2_EQU_CAL_CNTL [09:05] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_CAL_CNTL_IN0_MDAC2_EQU_CAL_CNTL_MASK 0x000003e0
#define BCHP_SWB_WFE_ANA_U1_WBADC_CAL_CNTL_IN0_MDAC2_EQU_CAL_CNTL_SHIFT 5

/* SWB_WFE_ANA_U1 :: WBADC_CAL_CNTL_IN0 :: MDAC1_EQU_CAL_CNTL [04:00] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_CAL_CNTL_IN0_MDAC1_EQU_CAL_CNTL_MASK 0x0000001f
#define BCHP_SWB_WFE_ANA_U1_WBADC_CAL_CNTL_IN0_MDAC1_EQU_CAL_CNTL_SHIFT 0

/***************************************************************************
 *WBADC_SYS_CNTL_IN0 - wbadc_sys_cntl
 ***************************************************************************/
/* SWB_WFE_ANA_U1 :: WBADC_SYS_CNTL_IN0 :: RFFE_POWER_UP [31:31] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_SYS_CNTL_IN0_RFFE_POWER_UP_MASK  0x80000000
#define BCHP_SWB_WFE_ANA_U1_WBADC_SYS_CNTL_IN0_RFFE_POWER_UP_SHIFT 31

/* SWB_WFE_ANA_U1 :: WBADC_SYS_CNTL_IN0 :: i_wbadc_sys_cntl_reserved_29 [30:29] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_SYS_CNTL_IN0_i_wbadc_sys_cntl_reserved_29_MASK 0x60000000
#define BCHP_SWB_WFE_ANA_U1_WBADC_SYS_CNTL_IN0_i_wbadc_sys_cntl_reserved_29_SHIFT 29

/* SWB_WFE_ANA_U1 :: WBADC_SYS_CNTL_IN0 :: ADC4_Digisum_Ping_Data_Skip_1_Latency [28:28] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_SYS_CNTL_IN0_ADC4_Digisum_Ping_Data_Skip_1_Latency_MASK 0x10000000
#define BCHP_SWB_WFE_ANA_U1_WBADC_SYS_CNTL_IN0_ADC4_Digisum_Ping_Data_Skip_1_Latency_SHIFT 28

/* SWB_WFE_ANA_U1 :: WBADC_SYS_CNTL_IN0 :: ADC3_Digisum_Ping_Data_Skip_1_Latency [27:27] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_SYS_CNTL_IN0_ADC3_Digisum_Ping_Data_Skip_1_Latency_MASK 0x08000000
#define BCHP_SWB_WFE_ANA_U1_WBADC_SYS_CNTL_IN0_ADC3_Digisum_Ping_Data_Skip_1_Latency_SHIFT 27

/* SWB_WFE_ANA_U1 :: WBADC_SYS_CNTL_IN0 :: ADC2_Digisum_Ping_Data_Skip_1_Latency [26:26] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_SYS_CNTL_IN0_ADC2_Digisum_Ping_Data_Skip_1_Latency_MASK 0x04000000
#define BCHP_SWB_WFE_ANA_U1_WBADC_SYS_CNTL_IN0_ADC2_Digisum_Ping_Data_Skip_1_Latency_SHIFT 26

/* SWB_WFE_ANA_U1 :: WBADC_SYS_CNTL_IN0 :: ADC1_Digisum_Ping_Data_Skip_1_Latency [25:25] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_SYS_CNTL_IN0_ADC1_Digisum_Ping_Data_Skip_1_Latency_MASK 0x02000000
#define BCHP_SWB_WFE_ANA_U1_WBADC_SYS_CNTL_IN0_ADC1_Digisum_Ping_Data_Skip_1_Latency_SHIFT 25

/* SWB_WFE_ANA_U1 :: WBADC_SYS_CNTL_IN0 :: ADC0_Digisum_Ping_Data_Skip_1_Latency [24:24] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_SYS_CNTL_IN0_ADC0_Digisum_Ping_Data_Skip_1_Latency_MASK 0x01000000
#define BCHP_SWB_WFE_ANA_U1_WBADC_SYS_CNTL_IN0_ADC0_Digisum_Ping_Data_Skip_1_Latency_SHIFT 24

/* SWB_WFE_ANA_U1 :: WBADC_SYS_CNTL_IN0 :: ADC0_TestPort_Enable [23:23] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_SYS_CNTL_IN0_ADC0_TestPort_Enable_MASK 0x00800000
#define BCHP_SWB_WFE_ANA_U1_WBADC_SYS_CNTL_IN0_ADC0_TestPort_Enable_SHIFT 23

/* SWB_WFE_ANA_U1 :: WBADC_SYS_CNTL_IN0 :: WBADC_clock_gen_resetb [22:22] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_SYS_CNTL_IN0_WBADC_clock_gen_resetb_MASK 0x00400000
#define BCHP_SWB_WFE_ANA_U1_WBADC_SYS_CNTL_IN0_WBADC_clock_gen_resetb_SHIFT 22

/* SWB_WFE_ANA_U1 :: WBADC_SYS_CNTL_IN0 :: ADC4_Phase_Control [21:19] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_SYS_CNTL_IN0_ADC4_Phase_Control_MASK 0x00380000
#define BCHP_SWB_WFE_ANA_U1_WBADC_SYS_CNTL_IN0_ADC4_Phase_Control_SHIFT 19

/* SWB_WFE_ANA_U1 :: WBADC_SYS_CNTL_IN0 :: ADC3_Phase_Control [18:16] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_SYS_CNTL_IN0_ADC3_Phase_Control_MASK 0x00070000
#define BCHP_SWB_WFE_ANA_U1_WBADC_SYS_CNTL_IN0_ADC3_Phase_Control_SHIFT 16

/* SWB_WFE_ANA_U1 :: WBADC_SYS_CNTL_IN0 :: ADC2_Phase_Control [15:13] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_SYS_CNTL_IN0_ADC2_Phase_Control_MASK 0x0000e000
#define BCHP_SWB_WFE_ANA_U1_WBADC_SYS_CNTL_IN0_ADC2_Phase_Control_SHIFT 13

/* SWB_WFE_ANA_U1 :: WBADC_SYS_CNTL_IN0 :: ADC1_Phase_Control [12:10] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_SYS_CNTL_IN0_ADC1_Phase_Control_MASK 0x00001c00
#define BCHP_SWB_WFE_ANA_U1_WBADC_SYS_CNTL_IN0_ADC1_Phase_Control_SHIFT 10

/* SWB_WFE_ANA_U1 :: WBADC_SYS_CNTL_IN0 :: ADC0_Phase_Control [09:07] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_SYS_CNTL_IN0_ADC0_Phase_Control_MASK 0x00000380
#define BCHP_SWB_WFE_ANA_U1_WBADC_SYS_CNTL_IN0_ADC0_Phase_Control_SHIFT 7

/* SWB_WFE_ANA_U1 :: WBADC_SYS_CNTL_IN0 :: Sleep_mode_ADC4 [06:06] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_SYS_CNTL_IN0_Sleep_mode_ADC4_MASK 0x00000040
#define BCHP_SWB_WFE_ANA_U1_WBADC_SYS_CNTL_IN0_Sleep_mode_ADC4_SHIFT 6

/* SWB_WFE_ANA_U1 :: WBADC_SYS_CNTL_IN0 :: Sleep_mode_ADC3 [05:05] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_SYS_CNTL_IN0_Sleep_mode_ADC3_MASK 0x00000020
#define BCHP_SWB_WFE_ANA_U1_WBADC_SYS_CNTL_IN0_Sleep_mode_ADC3_SHIFT 5

/* SWB_WFE_ANA_U1 :: WBADC_SYS_CNTL_IN0 :: Sleep_mode_ADC2 [04:04] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_SYS_CNTL_IN0_Sleep_mode_ADC2_MASK 0x00000010
#define BCHP_SWB_WFE_ANA_U1_WBADC_SYS_CNTL_IN0_Sleep_mode_ADC2_SHIFT 4

/* SWB_WFE_ANA_U1 :: WBADC_SYS_CNTL_IN0 :: Sleep_mode_ADC1 [03:03] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_SYS_CNTL_IN0_Sleep_mode_ADC1_MASK 0x00000008
#define BCHP_SWB_WFE_ANA_U1_WBADC_SYS_CNTL_IN0_Sleep_mode_ADC1_SHIFT 3

/* SWB_WFE_ANA_U1 :: WBADC_SYS_CNTL_IN0 :: Sleep_mode_ADC0 [02:02] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_SYS_CNTL_IN0_Sleep_mode_ADC0_MASK 0x00000004
#define BCHP_SWB_WFE_ANA_U1_WBADC_SYS_CNTL_IN0_Sleep_mode_ADC0_SHIFT 2

/* SWB_WFE_ANA_U1 :: WBADC_SYS_CNTL_IN0 :: wbadc_sys_cntl_reserve_1 [01:01] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_SYS_CNTL_IN0_wbadc_sys_cntl_reserve_1_MASK 0x00000002
#define BCHP_SWB_WFE_ANA_U1_WBADC_SYS_CNTL_IN0_wbadc_sys_cntl_reserve_1_SHIFT 1

/* SWB_WFE_ANA_U1 :: WBADC_SYS_CNTL_IN0 :: ADC_Power_Up [00:00] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_SYS_CNTL_IN0_ADC_Power_Up_MASK   0x00000001
#define BCHP_SWB_WFE_ANA_U1_WBADC_SYS_CNTL_IN0_ADC_Power_Up_SHIFT  0

/***************************************************************************
 *WBADC_TIMING_CNTL_IN0 - wbadc_timing_cntl
 ***************************************************************************/
/* SWB_WFE_ANA_U1 :: WBADC_TIMING_CNTL_IN0 :: i_wbadc_timing_cntl_reserved [31:30] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_TIMING_CNTL_IN0_i_wbadc_timing_cntl_reserved_MASK 0xc0000000
#define BCHP_SWB_WFE_ANA_U1_WBADC_TIMING_CNTL_IN0_i_wbadc_timing_cntl_reserved_SHIFT 30

/* SWB_WFE_ANA_U1 :: WBADC_TIMING_CNTL_IN0 :: CLK_TIMING_CNTL_ADC4 [29:24] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_TIMING_CNTL_IN0_CLK_TIMING_CNTL_ADC4_MASK 0x3f000000
#define BCHP_SWB_WFE_ANA_U1_WBADC_TIMING_CNTL_IN0_CLK_TIMING_CNTL_ADC4_SHIFT 24

/* SWB_WFE_ANA_U1 :: WBADC_TIMING_CNTL_IN0 :: CLK_TIMING_CNTL_ADC3 [23:18] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_TIMING_CNTL_IN0_CLK_TIMING_CNTL_ADC3_MASK 0x00fc0000
#define BCHP_SWB_WFE_ANA_U1_WBADC_TIMING_CNTL_IN0_CLK_TIMING_CNTL_ADC3_SHIFT 18

/* SWB_WFE_ANA_U1 :: WBADC_TIMING_CNTL_IN0 :: CLK_TIMING_CNTL_ADC2 [17:12] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_TIMING_CNTL_IN0_CLK_TIMING_CNTL_ADC2_MASK 0x0003f000
#define BCHP_SWB_WFE_ANA_U1_WBADC_TIMING_CNTL_IN0_CLK_TIMING_CNTL_ADC2_SHIFT 12

/* SWB_WFE_ANA_U1 :: WBADC_TIMING_CNTL_IN0 :: CLK_TIMING_CNTL_ADC1 [11:06] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_TIMING_CNTL_IN0_CLK_TIMING_CNTL_ADC1_MASK 0x00000fc0
#define BCHP_SWB_WFE_ANA_U1_WBADC_TIMING_CNTL_IN0_CLK_TIMING_CNTL_ADC1_SHIFT 6

/* SWB_WFE_ANA_U1 :: WBADC_TIMING_CNTL_IN0 :: CLK_TIMING_CNTL_ADC0 [05:00] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_TIMING_CNTL_IN0_CLK_TIMING_CNTL_ADC0_MASK 0x0000003f
#define BCHP_SWB_WFE_ANA_U1_WBADC_TIMING_CNTL_IN0_CLK_TIMING_CNTL_ADC0_SHIFT 0

/***************************************************************************
 *WBADC_TOP_CNTL_IN0 - wbadc_top_cntl
 ***************************************************************************/
/* SWB_WFE_ANA_U1 :: WBADC_TOP_CNTL_IN0 :: ADC5_averaging_enable [31:31] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_TOP_CNTL_IN0_ADC5_averaging_enable_MASK 0x80000000
#define BCHP_SWB_WFE_ANA_U1_WBADC_TOP_CNTL_IN0_ADC5_averaging_enable_SHIFT 31

/* SWB_WFE_ANA_U1 :: WBADC_TOP_CNTL_IN0 :: ADC3_averaging_enable [30:30] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_TOP_CNTL_IN0_ADC3_averaging_enable_MASK 0x40000000
#define BCHP_SWB_WFE_ANA_U1_WBADC_TOP_CNTL_IN0_ADC3_averaging_enable_SHIFT 30

/* SWB_WFE_ANA_U1 :: WBADC_TOP_CNTL_IN0 :: ADC124_averaging_enable [29:29] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_TOP_CNTL_IN0_ADC124_averaging_enable_MASK 0x20000000
#define BCHP_SWB_WFE_ANA_U1_WBADC_TOP_CNTL_IN0_ADC124_averaging_enable_SHIFT 29

/* SWB_WFE_ANA_U1 :: WBADC_TOP_CNTL_IN0 :: Rterm_Control [28:28] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_TOP_CNTL_IN0_Rterm_Control_MASK  0x10000000
#define BCHP_SWB_WFE_ANA_U1_WBADC_TOP_CNTL_IN0_Rterm_Control_SHIFT 28

/* SWB_WFE_ANA_U1 :: WBADC_TOP_CNTL_IN0 :: i_wbadc_top_reserved [27:27] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_TOP_CNTL_IN0_i_wbadc_top_reserved_MASK 0x08000000
#define BCHP_SWB_WFE_ANA_U1_WBADC_TOP_CNTL_IN0_i_wbadc_top_reserved_SHIFT 27

/* SWB_WFE_ANA_U1 :: WBADC_TOP_CNTL_IN0 :: Cal_Dac_Shuffling_Clock_Freq_Cntl [26:25] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_TOP_CNTL_IN0_Cal_Dac_Shuffling_Clock_Freq_Cntl_MASK 0x06000000
#define BCHP_SWB_WFE_ANA_U1_WBADC_TOP_CNTL_IN0_Cal_Dac_Shuffling_Clock_Freq_Cntl_SHIFT 25

/* SWB_WFE_ANA_U1 :: WBADC_TOP_CNTL_IN0 :: Cal_Dac_LSB_Size_Cntl [24:22] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_TOP_CNTL_IN0_Cal_Dac_LSB_Size_Cntl_MASK 0x01c00000
#define BCHP_SWB_WFE_ANA_U1_WBADC_TOP_CNTL_IN0_Cal_Dac_LSB_Size_Cntl_SHIFT 22

/* SWB_WFE_ANA_U1 :: WBADC_TOP_CNTL_IN0 :: Cal_Dac_MSB_input [21:17] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_TOP_CNTL_IN0_Cal_Dac_MSB_input_MASK 0x003e0000
#define BCHP_SWB_WFE_ANA_U1_WBADC_TOP_CNTL_IN0_Cal_Dac_MSB_input_SHIFT 17

/* SWB_WFE_ANA_U1 :: WBADC_TOP_CNTL_IN0 :: CAL_Dac_LSB_input [16:12] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_TOP_CNTL_IN0_CAL_Dac_LSB_input_MASK 0x0001f000
#define BCHP_SWB_WFE_ANA_U1_WBADC_TOP_CNTL_IN0_CAL_Dac_LSB_input_SHIFT 12

/* SWB_WFE_ANA_U1 :: WBADC_TOP_CNTL_IN0 :: Cal_Dac_shuffling_enable [11:11] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_TOP_CNTL_IN0_Cal_Dac_shuffling_enable_MASK 0x00000800
#define BCHP_SWB_WFE_ANA_U1_WBADC_TOP_CNTL_IN0_Cal_Dac_shuffling_enable_SHIFT 11

/* SWB_WFE_ANA_U1 :: WBADC_TOP_CNTL_IN0 :: Cal_Dac_shuffle_clk_enable [10:10] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_TOP_CNTL_IN0_Cal_Dac_shuffle_clk_enable_MASK 0x00000400
#define BCHP_SWB_WFE_ANA_U1_WBADC_TOP_CNTL_IN0_Cal_Dac_shuffle_clk_enable_SHIFT 10

/* SWB_WFE_ANA_U1 :: WBADC_TOP_CNTL_IN0 :: Cal_dac_Power_Down [09:09] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_TOP_CNTL_IN0_Cal_dac_Power_Down_MASK 0x00000200
#define BCHP_SWB_WFE_ANA_U1_WBADC_TOP_CNTL_IN0_Cal_dac_Power_Down_SHIFT 9

/* SWB_WFE_ANA_U1 :: WBADC_TOP_CNTL_IN0 :: adc_top_8_reserved [08:08] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_TOP_CNTL_IN0_adc_top_8_reserved_MASK 0x00000100
#define BCHP_SWB_WFE_ANA_U1_WBADC_TOP_CNTL_IN0_adc_top_8_reserved_SHIFT 8

/* SWB_WFE_ANA_U1 :: WBADC_TOP_CNTL_IN0 :: spare_top_cntl_6_7 [07:06] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_TOP_CNTL_IN0_spare_top_cntl_6_7_MASK 0x000000c0
#define BCHP_SWB_WFE_ANA_U1_WBADC_TOP_CNTL_IN0_spare_top_cntl_6_7_SHIFT 6

/* SWB_WFE_ANA_U1 :: WBADC_TOP_CNTL_IN0 :: CLK_ROUTING_CNTL [05:05] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_TOP_CNTL_IN0_CLK_ROUTING_CNTL_MASK 0x00000020
#define BCHP_SWB_WFE_ANA_U1_WBADC_TOP_CNTL_IN0_CLK_ROUTING_CNTL_SHIFT 5

/* SWB_WFE_ANA_U1 :: WBADC_TOP_CNTL_IN0 :: CLK_load_Control [04:02] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_TOP_CNTL_IN0_CLK_load_Control_MASK 0x0000001c
#define BCHP_SWB_WFE_ANA_U1_WBADC_TOP_CNTL_IN0_CLK_load_Control_SHIFT 2

/* SWB_WFE_ANA_U1 :: WBADC_TOP_CNTL_IN0 :: CLK_CML_SEL [01:00] */
#define BCHP_SWB_WFE_ANA_U1_WBADC_TOP_CNTL_IN0_CLK_CML_SEL_MASK    0x00000003
#define BCHP_SWB_WFE_ANA_U1_WBADC_TOP_CNTL_IN0_CLK_CML_SEL_SHIFT   0

/***************************************************************************
 *REG_CTRL - register access control
 ***************************************************************************/
/* SWB_WFE_ANA_U1 :: REG_CTRL :: reserved_for_eco0 [31:15] */
#define BCHP_SWB_WFE_ANA_U1_REG_CTRL_reserved_for_eco0_MASK        0xffff8000
#define BCHP_SWB_WFE_ANA_U1_REG_CTRL_reserved_for_eco0_SHIFT       15

/* SWB_WFE_ANA_U1 :: REG_CTRL :: broadcast_ack_status [14:08] */
#define BCHP_SWB_WFE_ANA_U1_REG_CTRL_broadcast_ack_status_MASK     0x00007f00
#define BCHP_SWB_WFE_ANA_U1_REG_CTRL_broadcast_ack_status_SHIFT    8

/* SWB_WFE_ANA_U1 :: REG_CTRL :: reserved_for_eco1 [07:01] */
#define BCHP_SWB_WFE_ANA_U1_REG_CTRL_reserved_for_eco1_MASK        0x000000fe
#define BCHP_SWB_WFE_ANA_U1_REG_CTRL_reserved_for_eco1_SHIFT       1

/* SWB_WFE_ANA_U1 :: REG_CTRL :: broadcast_enable [00:00] */
#define BCHP_SWB_WFE_ANA_U1_REG_CTRL_broadcast_enable_MASK         0x00000001
#define BCHP_SWB_WFE_ANA_U1_REG_CTRL_broadcast_enable_SHIFT        0

/***************************************************************************
 *SW_SPARE0 - sw_spare0
 ***************************************************************************/
/* SWB_WFE_ANA_U1 :: SW_SPARE0 :: sw_spare0 [31:00] */
#define BCHP_SWB_WFE_ANA_U1_SW_SPARE0_sw_spare0_MASK               0xffffffff
#define BCHP_SWB_WFE_ANA_U1_SW_SPARE0_sw_spare0_SHIFT              0

/***************************************************************************
 *SW_SPARE1 - sw_spare1
 ***************************************************************************/
/* SWB_WFE_ANA_U1 :: SW_SPARE1 :: sw_spare0 [31:00] */
#define BCHP_SWB_WFE_ANA_U1_SW_SPARE1_sw_spare0_MASK               0xffffffff
#define BCHP_SWB_WFE_ANA_U1_SW_SPARE1_sw_spare0_SHIFT              0

#endif /* #ifndef BCHP_SWB_WFE_ANA_U1_H__ */

/* End of File */