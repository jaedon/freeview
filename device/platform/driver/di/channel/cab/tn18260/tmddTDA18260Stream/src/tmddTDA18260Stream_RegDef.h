/*
  Copyright (C) 2006-2009 NXP B.V., All Rights Reserved.
  This source code and any compilation or derivative thereof is the proprietary
  information of NXP B.V. and is confidential in nature. Under no circumstances
  is this software to be  exposed to or placed under an Open Source License of
  any type without the expressed written permission of NXP B.V.
 *
 * \file          tmddTDA18260Stream_RegDef.h
 *
 *                CFR_FEAP#4
 *
 * \date          %modify_time%
 *
 * \brief         Describe briefly the purpose of this file.
 *
 * REFERENCE DOCUMENTS :
 *                TDA18260_Driver_User_Guide.pdf
 *
 * Detailed description may be added here.
 *
 * \section info Change Information
 *
*/

/* File generated automatically from register description file */


#ifndef _TMDD_TDA18260STREAM_REGDEF_H
#define _TMDD_TDA18260STREAM_REGDEF_H

#ifdef __cplusplus
extern "C"
{
#endif

#pragma GCC system_header

/* #define _TMDD_TDA18260STREAM_REGMAP_DEFINED */

/*============================================================================*/
/* Registers definitions:                                                     */
/*============================================================================*/
/* TDA18260 Register power_ctrl 0x00 */
typedef union
{
    UInt8 power_ctrl;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 unused_d7_1 : 7;
    UInt8 softreset : 1;
#else
    UInt8 softreset : 1;
    UInt8 unused_d7_1 : 7;
#endif
    }bF;
}TDA18260_Reg_power_ctrl;
#define TDA18260_REG_ADD_power_ctrl (0x00)
#define TDA18260_REG_SZ_power_ctrl  (0x01)

/* TDA18260 Register msm 0x01 */
typedef union
{
    UInt8 msm;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 unused_d7_2 : 6;
    UInt8 rccal_launch : 1;
    UInt8 rssi_launch : 1;
#else
    UInt8 rssi_launch : 1;
    UInt8 rccal_launch : 1;
    UInt8 unused_d7_2 : 6;
#endif
    }bF;
}TDA18260_Reg_msm;
#define TDA18260_REG_ADD_msm (0x01)
#define TDA18260_REG_SZ_msm  (0x01)

/* TDA18260 Register power_down_1 0x02 */
typedef union
{
    UInt8 power_down_1;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 unused_d7_6 : 2;
    UInt8 pd_buf1b : 1;
    UInt8 pd_reg2v8 : 1;
    UInt8 pd_lo_synthe : 1;
    UInt8 hp_bypass : 1;
    UInt8 pd_rfagc_ifout : 1;
    UInt8 pd_agc1bis : 1;
#else
    UInt8 pd_agc1bis : 1;
    UInt8 pd_rfagc_ifout : 1;
    UInt8 hp_bypass : 1;
    UInt8 pd_lo_synthe : 1;
    UInt8 pd_reg2v8 : 1;
    UInt8 pd_buf1b : 1;
    UInt8 unused_d7_6 : 2;
#endif
    }bF;
}TDA18260_Reg_power_down_1;
#define TDA18260_REG_ADD_power_down_1 (0x02)
#define TDA18260_REG_SZ_power_down_1  (0x01)

/* TDA18260 Register power_down_2 0x03 */
typedef union
{
    UInt8 power_down_2;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 psm : 1;
    UInt8 sm : 1;
    UInt8 pd_notch : 1;
    UInt8 pd_agc2_detect4 : 1;
    UInt8 pd_agc2_detect3 : 1;
    UInt8 pd_agc2_detect2 : 1;
    UInt8 pd_agc2 : 1;
    UInt8 agc2_digck_on : 1;
#else
    UInt8 agc2_digck_on : 1;
    UInt8 pd_agc2 : 1;
    UInt8 pd_agc2_detect2 : 1;
    UInt8 pd_agc2_detect3 : 1;
    UInt8 pd_agc2_detect4 : 1;
    UInt8 pd_notch : 1;
    UInt8 sm : 1;
    UInt8 psm : 1;
#endif
    }bF;
}TDA18260_Reg_power_down_2;
#define TDA18260_REG_ADD_power_down_2 (0x03)
#define TDA18260_REG_SZ_power_down_2  (0x01)

/* TDA18260 Register agc2_ctrl 0x04 */
typedef union
{
    UInt8 agc2_ctrl;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 unused_d7_2 : 6;
    UInt8 agc2_bypass : 1;
    UInt8 agc2_gain_force : 1;
#else
    UInt8 agc2_gain_force : 1;
    UInt8 agc2_bypass : 1;
    UInt8 unused_d7_2 : 6;
#endif
    }bF;
}TDA18260_Reg_agc2_ctrl;
#define TDA18260_REG_ADD_agc2_ctrl (0x04)
#define TDA18260_REG_SZ_agc2_ctrl  (0x01)

/* TDA18260 Register agc2_gain 0x05 */
typedef union
{
    UInt8 agc2_gain;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 agc2_gain : 8;
#else
    UInt8 agc2_gain : 8;
#endif
    }bF;
}TDA18260_Reg_agc2_gain;
#define TDA18260_REG_ADD_agc2_gain (0x05)
#define TDA18260_REG_SZ_agc2_gain  (0x01)

/* TDA18260 Register agc2_step 0x06 */
typedef union
{
    UInt8 agc2_step;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 unused_d7_4 : 4;
    UInt8 agc2_down_step : 2;
    UInt8 agc2_up_step : 2;
#else
    UInt8 agc2_up_step : 2;
    UInt8 agc2_down_step : 2;
    UInt8 unused_d7_4 : 4;
#endif
    }bF;
}TDA18260_Reg_agc2_step;
#define TDA18260_REG_ADD_agc2_step (0x06)
#define TDA18260_REG_SZ_agc2_step  (0x01)

/* TDA18260 Register agc2_g_ud 0x07 */
typedef union
{
    UInt8 agc2_g_ud;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 unused_d7_5 : 3;
    UInt8 agc2_g_up_down : 5;
#else
    UInt8 agc2_g_up_down : 5;
    UInt8 unused_d7_5 : 3;
#endif
    }bF;
}TDA18260_Reg_agc2_g_ud;
#define TDA18260_REG_ADD_agc2_g_ud (0x07)
#define TDA18260_REG_SZ_agc2_g_ud  (0x01)

/* TDA18260 Register buf1b_agc1bis_da 0x08 */
typedef union
{
    UInt8 buf1b_agc1bis_da;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 unused_d7_3 : 5;
    UInt8 agc1bis_gain : 3;
#else
    UInt8 agc1bis_gain : 3;
    UInt8 unused_d7_3 : 5;
#endif
    }bF;
}TDA18260_Reg_buf1b_agc1bis_da;
#define TDA18260_REG_ADD_buf1b_agc1bis_da (0x08)
#define TDA18260_REG_SZ_buf1b_agc1bis_da  (0x01)

/* TDA18260 Register rf_filter1_smooth_t_cst 0x09 */
typedef union
{
    UInt8 rf_filter1_smooth_t_cst;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 unused_d7_3 : 5;
    UInt8 rfsw_t_cst : 3;
#else
    UInt8 rfsw_t_cst : 3;
    UInt8 unused_d7_3 : 5;
#endif
    }bF;
}TDA18260_Reg_rf_filter1_smooth_t_cst;
#define TDA18260_REG_ADD_rf_filter1_smooth_t_cst (0x09)
#define TDA18260_REG_SZ_rf_filter1_smooth_t_cst  (0x01)

/* TDA18260 Register rf_filter_da_0 0x0A */
typedef union
{
    UInt8 rf_filter_da_0;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 unused_d7_2 : 6;
    UInt8 rf_filter1_mode_3 : 1;
    UInt8 rf_filter1_mode_2 : 1;
#else
    UInt8 rf_filter1_mode_2 : 1;
    UInt8 rf_filter1_mode_3 : 1;
    UInt8 unused_d7_2 : 6;
#endif
    }bF;
}TDA18260_Reg_rf_filter_da_0;
#define TDA18260_REG_ADD_rf_filter_da_0 (0x0A)
#define TDA18260_REG_SZ_rf_filter_da_0  (0x01)

/* TDA18260 Register rf_filter1_smooth_move 0x0B */
typedef union
{
    UInt8 rf_filter1_smooth_move;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 unused_d7_2 : 6;
    UInt8 rf_filter1_move_3 : 1;
    UInt8 rf_filter1_move_2 : 1;
#else
    UInt8 rf_filter1_move_2 : 1;
    UInt8 rf_filter1_move_3 : 1;
    UInt8 unused_d7_2 : 6;
#endif
    }bF;
}TDA18260_Reg_rf_filter1_smooth_move;
#define TDA18260_REG_ADD_rf_filter1_smooth_move (0x0B)
#define TDA18260_REG_SZ_rf_filter1_smooth_move  (0x01)

/* TDA18260 Register rf_filter_da_1 0x0C */
typedef union
{
    UInt8 rf_filter_da_1;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 unused_d7_2 : 6;
    UInt8 rf_filter1_mode_1 : 1;
    UInt8 rf_filter1_mode_0 : 1;
#else
    UInt8 rf_filter1_mode_0 : 1;
    UInt8 rf_filter1_mode_1 : 1;
    UInt8 unused_d7_2 : 6;
#endif
    }bF;
}TDA18260_Reg_rf_filter_da_1;
#define TDA18260_REG_ADD_rf_filter_da_1 (0x0C)
#define TDA18260_REG_SZ_rf_filter_da_1  (0x01)

/* TDA18260 Register rf_filter_da_2 0x0D */
typedef union
{
    UInt8 rf_filter_da_2;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 unused_d7_6 : 2;
    UInt8 bp_ind2 : 2;
    UInt8 bp_ind1 : 3;
    UInt8 ind_div2 : 1;
#else
    UInt8 ind_div2 : 1;
    UInt8 bp_ind1 : 3;
    UInt8 bp_ind2 : 2;
    UInt8 unused_d7_6 : 2;
#endif
    }bF;
}TDA18260_Reg_rf_filter_da_2;
#define TDA18260_REG_ADD_rf_filter_da_2 (0x0D)
#define TDA18260_REG_SZ_rf_filter_da_2  (0x01)

/* TDA18260 Register rf_filter_da_3 0x0E */
typedef union
{
    UInt8 rf_filter_da_3;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 unused_d7_4 : 4;
    UInt8 cap_l6n : 2;
    UInt8 cap_l10n : 2;
#else
    UInt8 cap_l10n : 2;
    UInt8 cap_l6n : 2;
    UInt8 unused_d7_4 : 4;
#endif
    }bF;
}TDA18260_Reg_rf_filter_da_3;
#define TDA18260_REG_ADD_rf_filter_da_3 (0x0E)
#define TDA18260_REG_SZ_rf_filter_da_3  (0x01)

/* TDA18260 Register rf_filter_da_4 0x0F */
typedef union
{
    UInt8 rf_filter_da_4;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 unused_d7_6 : 2;
    UInt8 cap_hp_left : 2;
    UInt8 cap_hp_mid : 2;
    UInt8 cap_hp_right : 2;
#else
    UInt8 cap_hp_right : 2;
    UInt8 cap_hp_mid : 2;
    UInt8 cap_hp_left : 2;
    UInt8 unused_d7_6 : 2;
#endif
    }bF;
}TDA18260_Reg_rf_filter_da_4;
#define TDA18260_REG_ADD_rf_filter_da_4 (0x0F)
#define TDA18260_REG_SZ_rf_filter_da_4  (0x01)

/* TDA18260 Register rf_filter2_da 0x10 */
typedef union
{
    UInt8 rf_filter2_da;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 unused_d7 : 1;
    UInt8 cfi_shift : 2;
    UInt8 agc2_dc : 1;
    UInt8 unused_d3 : 1;
    UInt8 rf_filter2_mode : 3;
#else
    UInt8 rf_filter2_mode : 3;
    UInt8 unused_d3 : 1;
    UInt8 agc2_dc : 1;
    UInt8 cfi_shift : 2;
    UInt8 unused_d7 : 1;
#endif
    }bF;
}TDA18260_Reg_rf_filter2_da;
#define TDA18260_REG_ADD_rf_filter2_da (0x10)
#define TDA18260_REG_SZ_rf_filter2_da  (0x01)

/* TDA18260 Register stob_da 0x11 */
typedef union
{
    UInt8 stob_da;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 unused_d7_5 : 3;
    UInt8 stob_gain : 1;
    UInt8 psm_stob : 2;
    UInt8 stob_dc : 2;
#else
    UInt8 stob_dc : 2;
    UInt8 psm_stob : 2;
    UInt8 stob_gain : 1;
    UInt8 unused_d7_5 : 3;
#endif
    }bF;
}TDA18260_Reg_stob_da;
#define TDA18260_REG_ADD_stob_da (0x11)
#define TDA18260_REG_SZ_stob_da  (0x01)

/* TDA18260 Register if_hp_combi_da 0x12 */
typedef union
{
    UInt8 if_hp_combi_da;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 hp_freq : 2;
    UInt8 combi_gain_3db : 1;
    UInt8 shift_lf_rccal : 1;
    UInt8 lf_freq : 4;
#else
    UInt8 lf_freq : 4;
    UInt8 shift_lf_rccal : 1;
    UInt8 combi_gain_3db : 1;
    UInt8 hp_freq : 2;
#endif
    }bF;
}TDA18260_Reg_if_hp_combi_da;
#define TDA18260_REG_ADD_if_hp_combi_da (0x12)
#define TDA18260_REG_SZ_if_hp_combi_da  (0x01)

/* TDA18260 Register if_lp_da 0x13 */
typedef union
{
    UInt8 if_lp_da;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 agc_low_bw : 1;
    UInt8 if_level : 3;
    UInt8 If_notch_freq : 2;
    UInt8 lp_fc : 2;
#else
    UInt8 lp_fc : 2;
    UInt8 If_notch_freq : 2;
    UInt8 if_level : 3;
    UInt8 agc_low_bw : 1;
#endif
    }bF;
}TDA18260_Reg_if_lp_da;
#define TDA18260_REG_ADD_if_lp_da (0x13)
#define TDA18260_REG_SZ_if_lp_da  (0x01)

/* TDA18260 Register ir_freq_value 0x14 */
typedef union
{
    UInt8 ir_freq_value;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 unused_d7_2 : 6;
    UInt8 ir_freq : 2;
#else
    UInt8 ir_freq : 2;
    UInt8 unused_d7_2 : 6;
#endif
    }bF;
}TDA18260_Reg_ir_freq_value;
#define TDA18260_REG_ADD_ir_freq_value (0x14)
#define TDA18260_REG_SZ_ir_freq_value  (0x01)

/* TDA18260 Register rssi_da 0x15 */
typedef union
{
    UInt8 rssi_da;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 unused_d7_2 : 6;
    UInt8 rssi_cap_reset_en : 1;
    UInt8 rssi_cap_val : 1;
#else
    UInt8 rssi_cap_val : 1;
    UInt8 rssi_cap_reset_en : 1;
    UInt8 unused_d7_2 : 6;
#endif
    }bF;
}TDA18260_Reg_rssi_da;
#define TDA18260_REG_ADD_rssi_da (0x15)
#define TDA18260_REG_SZ_rssi_da  (0x01)

/* TDA18260 Register rssi_config 0x16 */
typedef union
{
    UInt8 rssi_config;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 unused_d7_4 : 4;
    UInt8 rssi_av : 1;
    UInt8 rssi_sinus_qam_not : 1;
    UInt8 rssi_ck_speed : 1;
    UInt8 rssi_dicho_not : 1;
#else
    UInt8 rssi_dicho_not : 1;
    UInt8 rssi_ck_speed : 1;
    UInt8 rssi_sinus_qam_not : 1;
    UInt8 rssi_av : 1;
    UInt8 unused_d7_4 : 4;
#endif
    }bF;
}TDA18260_Reg_rssi_config;
#define TDA18260_REG_ADD_rssi_config (0x16)
#define TDA18260_REG_SZ_rssi_config  (0x01)

/* TDA18260 Register pll_delta 0x17 */
typedef union
{
    UInt8 pll_delta;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 ircal_delta : 8;
#else
    UInt8 ircal_delta : 8;
#endif
    }bF;
}TDA18260_Reg_pll_delta;
#define TDA18260_REG_ADD_pll_delta (0x17)
#define TDA18260_REG_SZ_pll_delta  (0x01)

/* TDA18260 Register pll_frac_0_value 0x18 */
typedef union
{
    UInt8 pll_frac_0_value;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 pll_frac_0 : 8;
#else
    UInt8 pll_frac_0 : 8;
#endif
    }bF;
}TDA18260_Reg_pll_frac_0_value;
#define TDA18260_REG_ADD_pll_frac_0_value (0x18)
#define TDA18260_REG_SZ_pll_frac_0_value  (0x01)

/* TDA18260 Register pll_frac_1_value 0x19 */
typedef union
{
    UInt8 pll_frac_1_value;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 pll_frac_1 : 8;
#else
    UInt8 pll_frac_1 : 8;
#endif
    }bF;
}TDA18260_Reg_pll_frac_1_value;
#define TDA18260_REG_ADD_pll_frac_1_value (0x19)
#define TDA18260_REG_SZ_pll_frac_1_value  (0x01)

/* TDA18260 Register pll_frac_2_value 0x1A */
typedef union
{
    UInt8 pll_frac_2_value;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 pll_frac_2 : 8;
#else
    UInt8 pll_frac_2 : 8;
#endif
    }bF;
}TDA18260_Reg_pll_frac_2_value;
#define TDA18260_REG_ADD_pll_frac_2_value (0x1A)
#define TDA18260_REG_SZ_pll_frac_2_value  (0x01)

/* TDA18260 Register pll_frac_3_value 0x1B */
typedef union
{
    UInt8 pll_frac_3_value;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 pll_frac_3 : 8;
#else
    UInt8 pll_frac_3 : 8;
#endif
    }bF;
}TDA18260_Reg_pll_frac_3_value;
#define TDA18260_REG_ADD_pll_frac_3_value (0x1B)
#define TDA18260_REG_SZ_pll_frac_3_value  (0x01)

/* TDA18260 Register icp 0x1C */
typedef union
{
    UInt8 icp;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 unused_d7 : 1;
    UInt8 nkcorr : 3;
    UInt8 unused_d3_1 : 3;
    UInt8 icp_low : 1;
#else
    UInt8 icp_low : 1;
    UInt8 unused_d3_1 : 3;
    UInt8 nkcorr : 3;
    UInt8 unused_d7 : 1;
#endif
    }bF;
}TDA18260_Reg_icp;
#define TDA18260_REG_ADD_icp (0x1C)
#define TDA18260_REG_SZ_icp  (0x01)

/* TDA18260 Register pll_prog_start 0x1D */
typedef union
{
    UInt8 pll_prog_start;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 unused_d7_1 : 7;
    UInt8 freq_prog_start : 1;
#else
    UInt8 freq_prog_start : 1;
    UInt8 unused_d7_1 : 7;
#endif
    }bF;
}TDA18260_Reg_pll_prog_start;
#define TDA18260_REG_ADD_pll_prog_start (0x1D)
#define TDA18260_REG_SZ_pll_prog_start  (0x01)

/* TDA18260 Register agc2_gain_status 0x1E */
typedef union
{
    UInt8 agc2_gain_status;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 agc2_gain_read : 8;
#else
    UInt8 agc2_gain_read : 8;
#endif
    }bF;
}TDA18260_Reg_agc2_gain_status;
#define TDA18260_REG_ADD_agc2_gain_status (0x1E)
#define TDA18260_REG_SZ_agc2_gain_status  (0x01)

/* TDA18260 Register status 0x1F */
typedef union
{
    UInt8 status;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 unused_d7_6 : 2;
    UInt8 rssi_alarm : 1;
    UInt8 rssi_sense_ok : 1;
    UInt8 rssi_msm_end : 1;
    UInt8 rccal_msm_end : 1;
    UInt8 freq_end : 1;
    UInt8 lo_calc_msm_end : 1;
#else
    UInt8 lo_calc_msm_end : 1;
    UInt8 freq_end : 1;
    UInt8 rccal_msm_end : 1;
    UInt8 rssi_msm_end : 1;
    UInt8 rssi_sense_ok : 1;
    UInt8 rssi_alarm : 1;
    UInt8 unused_d7_6 : 2;
#endif
    }bF;
}TDA18260_Reg_status;
#define TDA18260_REG_ADD_status (0x1F)
#define TDA18260_REG_SZ_status  (0x01)

/* TDA18260 Register rssi_level 0x20 */
typedef union
{
    UInt8 rssi_level;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 rssi : 8;
#else
    UInt8 rssi : 8;
#endif
    }bF;
}TDA18260_Reg_rssi_level;
#define TDA18260_REG_ADD_rssi_level (0x20)
#define TDA18260_REG_SZ_rssi_level  (0x01)

/* TDA18260 Register dummy_value 0x21 */
typedef union
{
    UInt8 dummy_value;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 dummy_1 : 2;
    UInt8 lpf_4mhz : 1;
    UInt8 psm_buf1b : 2;
    UInt8 dummy_0 : 2;
    UInt8 pll_sd_rstn : 1;
#else
    UInt8 pll_sd_rstn : 1;
    UInt8 dummy_0 : 2;
    UInt8 psm_buf1b : 2;
    UInt8 lpf_4mhz : 1;
    UInt8 dummy_1 : 2;
#endif
    }bF;
}TDA18260_Reg_dummy_value;
#define TDA18260_REG_ADD_dummy_value (0x21)
#define TDA18260_REG_SZ_dummy_value  (0x01)

/* TDA18260 Register pll_da_2 0x28 */
typedef union
{
    UInt8 pll_da_2;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 unused_d7 : 1;
    UInt8 unused_d6 : 1;
    UInt8 unused_d4 : 1;
    UInt8 unused_d3 : 2;
    UInt8 pll_forcesrce : 1;
    UInt8 unused_d1 : 1;
    UInt8 unused_d0 : 1;
#else
    UInt8 unused_d0 : 1;
    UInt8 unused_d1 : 1;
    UInt8 pll_forcesrce : 1;
    UInt8 unused_d3 : 2;
    UInt8 unused_d4 : 1;
    UInt8 unused_d6 : 1;
    UInt8 unused_d7 : 1;
#endif
    }bF;
}TDA18260_Reg_pll_da_2;
#define TDA18260_REG_ADD_pll_da_2 (0x28)
#define TDA18260_REG_SZ_pll_da_2  (0x01)

/* TDA18260 Register if_lp 0x2F */
typedef union
{
    UInt8 if_lp;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 unused_d7_5 : 3;
    UInt8 detector4_fast : 1;
    UInt8 unused_d3 : 1;
    UInt8 rc_lpf_offset : 2;
    UInt8 rc_lpf_sign : 1;
#else
    UInt8 rc_lpf_sign : 1;
    UInt8 rc_lpf_offset : 2;
    UInt8 unused_d3 : 1;
    UInt8 detector4_fast : 1;
    UInt8 unused_d7_5 : 3;
#endif
    }bF;
}TDA18260_Reg_if_lp;
#define TDA18260_REG_ADD_if_lp (0x2F)
#define TDA18260_REG_SZ_if_lp  (0x01)

typedef union
{
    UInt8 dummy_1;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
        UInt8 bit_7 : 1;
        UInt8 bit_6 : 1;
        UInt8 bit_5 : 1;
        UInt8 bit_4 : 1;
        UInt8 bit_3 : 1;
        UInt8 bit_2 : 1;
        UInt8 bit_1 : 1;
        UInt8 bit_0 : 1;
#else
        UInt8 bit_0 : 1;
        UInt8 bit_1 : 1;
        UInt8 bit_2 : 1;
        UInt8 bit_3 : 1;
        UInt8 bit_4 : 1;
        UInt8 bit_5 : 1;
        UInt8 bit_6 : 1;
        UInt8 bit_7 : 1;
#endif
    }bF;
}TDA18260Stream_Dummy;

#ifdef _TMDD_TDA18260_REGMAP_DEFINED
 /* TDA18260Stream register map */
 typedef struct _TDA18260Stream_Reg_Map_t
 {                                                                              /* Base@ = 0x40 or 0x80 */
     TDA18260_Reg_power_ctrl Reg_power_ctrl;                                    /* Base@ + Reg@0x00 */
     TDA18260_Reg_msm Reg_msm;                                                  /* Base@ + Reg@0x01 */
     TDA18260_Reg_power_down_1 Reg_power_down_1;                                /* Base@ + Reg@0x02 */
     TDA18260_Reg_power_down_2 Reg_power_down_2;                                /* Base@ + Reg@0x03 */
     TDA18260_Reg_agc2_ctrl Reg_agc2_ctrl;                                      /* Base@ + Reg@0x04 */
     TDA18260_Reg_agc2_gain Reg_agc2_gain;                                      /* Base@ + Reg@0x05 */
     TDA18260_Reg_agc2_step Reg_agc2_step;                                      /* Base@ + Reg@0x06 */
     TDA18260_Reg_agc2_g_ud Reg_agc2_g_ud;                                      /* Base@ + Reg@0x07 */
     TDA18260_Reg_buf1b_agc1bis_da Reg_buf1b_agc1bis_da;                        /* Base@ + Reg@0x08 */
     TDA18260_Reg_rf_filter1_smooth_t_cst Reg_rf_filter1_smooth_t_cst;          /* Base@ + Reg@0x09 */
     TDA18260_Reg_rf_filter_da_0 Reg_rf_filter_da_0;                            /* Base@ + Reg@0x0A */
     TDA18260_Reg_rf_filter1_smooth_move Reg_rf_filter1_smooth_move;            /* Base@ + Reg@0x0B */
     TDA18260_Reg_rf_filter_da_1 Reg_rf_filter_da_1;                            /* Base@ + Reg@0x0C */
     TDA18260_Reg_rf_filter_da_2 Reg_rf_filter_da_2;                            /* Base@ + Reg@0x0D */
     TDA18260_Reg_rf_filter_da_3 Reg_rf_filter_da_3;                            /* Base@ + Reg@0x0E */
     TDA18260_Reg_rf_filter_da_4 Reg_rf_filter_da_4;                            /* Base@ + Reg@0x0F */
     TDA18260_Reg_rf_filter2_da Reg_rf_filter2_da;                              /* Base@ + Reg@0x10 */
     TDA18260_Reg_stob_da Reg_stob_da;                                          /* Base@ + Reg@0x11 */
     TDA18260_Reg_if_hp_combi_da Reg_if_hp_combi_da;                            /* Base@ + Reg@0x12 */
     TDA18260_Reg_if_lp_da Reg_if_lp_da;                                        /* Base@ + Reg@0x13 */
     TDA18260_Reg_ir_freq_value Reg_ir_freq_value;                              /* Base@ + Reg@0x14 */
     TDA18260_Reg_rssi_da Reg_rssi_da;                                          /* Base@ + Reg@0x15 */
     TDA18260_Reg_rssi_config Reg_rssi_config;                                  /* Base@ + Reg@0x16 */
     TDA18260_Reg_pll_delta Reg_pll_delta;                                      /* Base@ + Reg@0x17 */
     TDA18260_Reg_pll_frac_0_value Reg_pll_frac_0_value;                        /* Base@ + Reg@0x18 */
     TDA18260_Reg_pll_frac_1_value Reg_pll_frac_1_value;                        /* Base@ + Reg@0x19 */
     TDA18260_Reg_pll_frac_2_value Reg_pll_frac_2_value;                        /* Base@ + Reg@0x1A */
     TDA18260_Reg_pll_frac_3_value Reg_pll_frac_3_value;                        /* Base@ + Reg@0x1B */
     TDA18260_Reg_icp Reg_icp;                                                  /* Base@ + Reg@0x1C */
     TDA18260_Reg_pll_prog_start Reg_pll_prog_start;                            /* Base@ + Reg@0x1D */
     TDA18260_Reg_agc2_gain_status Reg_agc2_gain_status;                        /* Base@ + Reg@0x1E */
     TDA18260_Reg_status Reg_status;                                            /* Base@ + Reg@0x1F */
     TDA18260_Reg_rssi_level Reg_rssi_level;                                    /* Base@ + Reg@0x20 */
     TDA18260_Reg_dummy_value Reg_dummy_value;                                  /* Base@ + Reg@0x21 */
     TDA18260Stream_Dummy Reg_unused_1_value;                                   /* Base@ + Reg@0x22 */
     TDA18260Stream_Dummy Reg_unused_2_value;                                   /* Base@ + Reg@0x23 */
     TDA18260Stream_Dummy Reg_unused_3_value;                                   /* Base@ + Reg@0x24 */
     TDA18260Stream_Dummy Reg_unused_4_value;                                   /* Base@ + Reg@0x25 */
     TDA18260Stream_Dummy Reg_unused_5_value;                                   /* Base@ + Reg@0x26 */
     TDA18260Stream_Dummy Reg_unused_6_value;                                   /* Base@ + Reg@0x27 */
     TDA18260_Reg_pll_da_2 Reg_pll_da_2;                                        /* Base@ + Reg@0x28 */
     TDA18260Stream_Dummy Reg_unused_7_value;                                   /* Base@ + Reg@0x29 */
     TDA18260Stream_Dummy Reg_unused_8_value;                                   /* Base@ + Reg@0x2A */
     TDA18260Stream_Dummy Reg_unused_9_value;                                   /* Base@ + Reg@0x2B */
     TDA18260Stream_Dummy Reg_unused_10_value;                                  /* Base@ + Reg@0x2C */
     TDA18260Stream_Dummy Reg_unused_11_value;                                  /* Base@ + Reg@0x2D */
     TDA18260Stream_Dummy Reg_unused_12_value;                                  /* Base@ + Reg@0x2E */
     TDA18260_Reg_if_lp Reg_if_lp;                                              /* Base@ + Reg@0x2F */
 } TDA18260Stream_Reg_Map_t, *pTDA18260Stream_Reg_Map_t;
#endif

#ifdef __cplusplus
}
#endif

#endif /* _TMDD_TDA18260STREAM_REGDEF_H */
