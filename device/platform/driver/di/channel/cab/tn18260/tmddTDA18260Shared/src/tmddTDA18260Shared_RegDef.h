/*
  Copyright (C) 2006-2009 NXP B.V., All Rights Reserved.
  This source code and any compilation or derivative thereof is the proprietary
  information of NXP B.V. and is confidential in nature. Under no circumstances
  is this software to be  exposed to or placed under an Open Source License of
  any type without the expressed written permission of NXP B.V.
 *
 * \file          tmddTDA18260Shared_RegDef.h
 *
 *                %version%
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


#ifndef _TMDD_TDA18260SHARED_REGDEF_H
#define _TMDD_TDA18260SHARED_REGDEF_H

#ifdef __cplusplus
extern "C"
{
#endif

#pragma GCC system_header
/* #define _TMDD_TDA18260SHARED_REGMAP_DEFINED */

/*============================================================================*/
/* Registers definitions:                                                     */
/*============================================================================*/
/* TDA18260 Register id_1 0x00 */
typedef union
{
    UInt8 id_1;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 master_notslave : 1;
    UInt8 ident_1 : 7;
#else
    UInt8 ident_1 : 7;
    UInt8 master_notslave : 1;
#endif
    }bF;
}TDA18260_Reg_id_1;
#define TDA18260_REG_ADD_id_1 (0x00)
#define TDA18260_REG_SZ_id_1  (0x01)

/* TDA18260 Register id_2 0x01 */
typedef union
{
    UInt8 id_2;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 ident_2 : 8;
#else
    UInt8 ident_2 : 8;
#endif
    }bF;
}TDA18260_Reg_id_2;
#define TDA18260_REG_ADD_id_2 (0x01)
#define TDA18260_REG_SZ_id_2  (0x01)

/* TDA18260 Register id_3 0x02 */
typedef union
{
    UInt8 id_3;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 major_rev : 4;
    UInt8 minor_rev : 4;
#else
    UInt8 minor_rev : 4;
    UInt8 major_rev : 4;
#endif
    }bF;
}TDA18260_Reg_id_3;
#define TDA18260_REG_ADD_id_3 (0x02)
#define TDA18260_REG_SZ_id_3  (0x01)

/* TDA18260 Register power_state 0x03 */
typedef union
{
    UInt8 power_state;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 unused_d7_1 : 7;
    UInt8 por : 1;
#else
    UInt8 por : 1;
    UInt8 unused_d7_1 : 7;
#endif
    }bF;
}TDA18260_Reg_power_state;
#define TDA18260_REG_ADD_power_state (0x03)
#define TDA18260_REG_SZ_power_state  (0x01)

/* TDA18260 Register lock_shared_reg_ctrl 0x04 */
typedef union
{
    UInt8 lock_shared_reg_ctrl;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 unused_d7_2 : 6;
    UInt8 lock_shared_reg : 2;
#else
    UInt8 lock_shared_reg : 2;
    UInt8 unused_d7_2 : 6;
#endif
    }bF;
}TDA18260_Reg_lock_shared_reg_ctrl;
#define TDA18260_REG_ADD_lock_shared_reg_ctrl (0x04)
#define TDA18260_REG_SZ_lock_shared_reg_ctrl  (0x01)

/* TDA18260 Register msm_1 0x05 */
typedef union
{
    UInt8 msm_1;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 unused_d7_5 : 3;
    UInt8 shared_reg_done : 1;
    UInt8 msm_mode : 4;
#else
    UInt8 msm_mode : 4;
    UInt8 shared_reg_done : 1;
    UInt8 unused_d7_5 : 3;
#endif
    }bF;
}TDA18260_Reg_msm_1;
#define TDA18260_REG_ADD_msm_1 (0x05)
#define TDA18260_REG_SZ_msm_1  (0x01)

/* TDA18260 Register msm_2 0x06 */
typedef union
{
    UInt8 msm_2;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 unused_d7_1 : 7;
    UInt8 msm_rst : 1;
#else
    UInt8 msm_rst : 1;
    UInt8 unused_d7_1 : 7;
#endif
    }bF;
}TDA18260_Reg_msm_2;
#define TDA18260_REG_ADD_msm_2 (0x06)
#define TDA18260_REG_SZ_msm_2  (0x01)

/* TDA18260 Register msm_3 0x07 */
typedef union
{
    UInt8 msm_3;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 unused_d7_2 : 6;
    UInt8 xtal_cal_launch : 1;
    UInt8 msm_launch : 1;
#else
    UInt8 msm_launch : 1;
    UInt8 xtal_cal_launch : 1;
    UInt8 unused_d7_2 : 6;
#endif
    }bF;
}TDA18260_Reg_msm_3;
#define TDA18260_REG_ADD_msm_3 (0x07)
#define TDA18260_REG_SZ_msm_3  (0x01)

/* TDA18260 Register power_ctrl 0x08 */
typedef union
{
    UInt8 power_ctrl;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 unused_d7_1 : 7;
    UInt8 sm : 1;
#else
    UInt8 sm : 1;
    UInt8 unused_d7_1 : 7;
#endif
    }bF;
}TDA18260_Reg_power_ctrl;
#define TDA18260_REG_ADD_power_ctrl (0x08)
#define TDA18260_REG_SZ_power_ctrl  (0x01)

/* TDA18260 Register power_down 0x09 */
typedef union
{
    UInt8 power_down;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 unused_d7_6 : 2;
    UInt8 thermo_on : 1;
    UInt8 pd_lt : 1;
    UInt8 pd_xtal : 1;
    UInt8 pd_buf1b : 1;
    UInt8 pd_agc1 : 1;
    UInt8 agc1_digck_on : 1;
#else
    UInt8 agc1_digck_on : 1;
    UInt8 pd_agc1 : 1;
    UInt8 pd_buf1b : 1;
    UInt8 pd_xtal : 1;
    UInt8 pd_lt : 1;
    UInt8 thermo_on : 1;
    UInt8 unused_d7_6 : 2;
#endif
    }bF;
}TDA18260_Reg_power_down;
#define TDA18260_REG_ADD_power_down (0x09)
#define TDA18260_REG_SZ_power_down  (0x01)

/* TDA18260 Register switch_clock_ctrl 0x0A */
typedef union
{
    UInt8 switch_clock_ctrl;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 unused_d7_4 : 4;
    UInt8 sb_force_switch_dig_not_sd : 1;
    UInt8 sa_force_switch_dig_not_sd : 1;
    UInt8 sb_enable_sd_ck_out : 1;
    UInt8 sa_enable_sd_ck_out : 1;
#else
    UInt8 sa_enable_sd_ck_out : 1;
    UInt8 sb_enable_sd_ck_out : 1;
    UInt8 sa_force_switch_dig_not_sd : 1;
    UInt8 sb_force_switch_dig_not_sd : 1;
    UInt8 unused_d7_4 : 4;
#endif
    }bF;
}TDA18260_Reg_switch_clock_ctrl;
#define TDA18260_REG_ADD_switch_clock_ctrl (0x0A)
#define TDA18260_REG_SZ_switch_clock_ctrl  (0x01)

/* TDA18260 Register switch_clock_config 0x0B */
typedef union
{
    UInt8 switch_clock_config;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 unused_d7_3 : 5;
    UInt8 switch_sd_dig_clock : 2;
    UInt8 switch_i2c_clock : 1;
#else
    UInt8 switch_i2c_clock : 1;
    UInt8 switch_sd_dig_clock : 2;
    UInt8 unused_d7_3 : 5;
#endif
    }bF;
}TDA18260_Reg_switch_clock_config;
#define TDA18260_REG_ADD_switch_clock_config (0x0B)
#define TDA18260_REG_SZ_switch_clock_config  (0x01)

/* TDA18260 Register agc1_ctrl 0x0C */
typedef union
{
    UInt8 agc1_ctrl;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 unused_d7 : 1;
    UInt8 lt_manual : 1;
    UInt8 lt_offset : 2;
    UInt8 unused_d3 : 1;
    UInt8 agc1_bypass : 1;
    UInt8 agc1_loop_off : 1;
    UInt8 agc1_gain_force : 1;
#else
    UInt8 agc1_gain_force : 1;
    UInt8 agc1_loop_off : 1;
    UInt8 agc1_bypass : 1;
    UInt8 unused_d3 : 1;
    UInt8 lt_offset : 2;
    UInt8 lt_manual : 1;
    UInt8 unused_d7 : 1;
#endif
    }bF;
}TDA18260_Reg_agc1_ctrl;
#define TDA18260_REG_ADD_agc1_ctrl (0x0C)
#define TDA18260_REG_SZ_agc1_ctrl  (0x01)

/* TDA18260 Register agc1_lt_gain 0x0D */
typedef union
{
    UInt8 agc1_lt_gain;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 lt_gain : 4;
    UInt8 agc1_gain : 4;
#else
    UInt8 agc1_gain : 4;
    UInt8 lt_gain : 4;
#endif
    }bF;
}TDA18260_Reg_agc1_lt_gain;
#define TDA18260_REG_ADD_agc1_lt_gain (0x0D)
#define TDA18260_REG_SZ_agc1_lt_gain  (0x01)

/* TDA18260 Register agc1_step 0x0E */
typedef union
{
    UInt8 agc1_step;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 unused_d7_4 : 4;
    UInt8 agc1_down_step : 2;
    UInt8 agc1_up_step : 2;
#else
    UInt8 agc1_up_step : 2;
    UInt8 agc1_down_step : 2;
    UInt8 unused_d7_4 : 4;
#endif
    }bF;
}TDA18260_Reg_agc1_step;
#define TDA18260_REG_ADD_agc1_step (0x0E)
#define TDA18260_REG_SZ_agc1_step  (0x01)

/* TDA18260 Register agc1_g_ud 0x0F */
typedef union
{
    UInt8 agc1_g_ud;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 unused_d7_5 : 3;
    UInt8 agc1_g_up_down : 5;
#else
    UInt8 agc1_g_up_down : 5;
    UInt8 unused_d7_5 : 3;
#endif
    }bF;
}TDA18260_Reg_agc1_g_ud;
#define TDA18260_REG_ADD_agc1_g_ud (0x0F)
#define TDA18260_REG_SZ_agc1_g_ud  (0x01)

/* TDA18260 Register agc1_a_ud 0x10 */
typedef union
{
    UInt8 agc1_a_ud;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 unused_d7_3 : 5;
    UInt8 agc1_a_up_down : 3;
#else
    UInt8 agc1_a_up_down : 3;
    UInt8 unused_d7_3 : 5;
#endif
    }bF;
}TDA18260_Reg_agc1_a_ud;
#define TDA18260_REG_ADD_agc1_a_ud (0x10)
#define TDA18260_REG_SZ_agc1_a_ud  (0x01)

/* TDA18260 Register agc1_lt_smooth_t_cst 0x11 */
typedef union
{
    UInt8 agc1_lt_smooth_t_cst;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 unused_d7 : 1;
    UInt8 lt_smooth_t_cst : 3;
    UInt8 unused_d3 : 1;
    UInt8 agc1_smooth_t_cst : 3;
#else
    UInt8 agc1_smooth_t_cst : 3;
    UInt8 unused_d3 : 1;
    UInt8 lt_smooth_t_cst : 3;
    UInt8 unused_d7 : 1;
#endif
    }bF;
}TDA18260_Reg_agc1_lt_smooth_t_cst;
#define TDA18260_REG_ADD_agc1_lt_smooth_t_cst (0x11)
#define TDA18260_REG_SZ_agc1_lt_smooth_t_cst  (0x01)

/* TDA18260 Register mto_smooth_t_cst 0x12 */
typedef union
{
    UInt8 mto_smooth_t_cst;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 unused_d7_5 : 3;
    UInt8 rfsw_mto_lt_rfin : 1;
    UInt8 unused_d3 : 1;
    UInt8 hp_mto_t_cst : 3;
#else
    UInt8 hp_mto_t_cst : 3;
    UInt8 unused_d3 : 1;
    UInt8 rfsw_mto_lt_rfin : 1;
    UInt8 unused_d7_5 : 3;
#endif
    }bF;
}TDA18260_Reg_mto_smooth_t_cst;
#define TDA18260_REG_ADD_mto_smooth_t_cst (0x12)
#define TDA18260_REG_SZ_mto_smooth_t_cst  (0x01)

/* TDA18260 Register mto_smooth_state 0x13 */
typedef union
{
    UInt8 mto_smooth_state;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 unused_d7_2 : 6;
    UInt8 hp_mto2_state : 1;
    UInt8 hp_mto1_state : 1;
#else
    UInt8 hp_mto1_state : 1;
    UInt8 hp_mto2_state : 1;
    UInt8 unused_d7_2 : 6;
#endif
    }bF;
}TDA18260_Reg_mto_smooth_state;
#define TDA18260_REG_ADD_mto_smooth_state (0x13)
#define TDA18260_REG_SZ_mto_smooth_state  (0x01)

/* TDA18260 Register mto_smooth_move 0x14 */
typedef union
{
    UInt8 mto_smooth_move;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 unused_d7_2 : 6;
    UInt8 hp_mto2_move : 1;
    UInt8 hp_mto1_move : 1;
#else
    UInt8 hp_mto1_move : 1;
    UInt8 hp_mto2_move : 1;
    UInt8 unused_d7_2 : 6;
#endif
    }bF;
}TDA18260_Reg_mto_smooth_move;
#define TDA18260_REG_ADD_mto_smooth_move (0x14)
#define TDA18260_REG_SZ_mto_smooth_move  (0x01)

/* TDA18260 Register xtout_1 0x15 */
typedef union
{
    UInt8 xtout_1;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 unused_d7 : 1;
    UInt8 xtal_xtout_mode : 3;
    UInt8 unused_d3 : 1;
    UInt8 xtal_xtout_drive : 3;
#else
    UInt8 xtal_xtout_drive : 3;
    UInt8 unused_d3 : 1;
    UInt8 xtal_xtout_mode : 3;
    UInt8 unused_d7 : 1;
#endif
    }bF;
}TDA18260_Reg_xtout_1;
#define TDA18260_REG_ADD_xtout_1 (0x15)
#define TDA18260_REG_SZ_xtout_1  (0x01)

/* TDA18260 Register agc1_lt_gain_status 0x16 */
typedef union
{
    UInt8 agc1_lt_gain_status;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 lt_gain_read : 4;
    UInt8 agc1_gain_read : 4;
#else
    UInt8 agc1_gain_read : 4;
    UInt8 lt_gain_read : 4;
#endif
    }bF;
}TDA18260_Reg_agc1_lt_gain_status;
#define TDA18260_REG_ADD_agc1_lt_gain_status (0x16)
#define TDA18260_REG_SZ_agc1_lt_gain_status  (0x01)

/* TDA18260 Register shared_status_1 0x17 */
typedef union
{
    UInt8 shared_status_1;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 unused_d7_6 : 2;
    UInt8 ircal_wanted_image_end_status : 1;
    UInt8 xtalcal_end : 1;
    UInt8 sb_ircal_pb : 1;
    UInt8 sb_ircal_msm_end : 1;
    UInt8 sa_ircal_pb : 1;
    UInt8 sa_ircal_msm_end : 1;
#else
    UInt8 sa_ircal_msm_end : 1;
    UInt8 sa_ircal_pb : 1;
    UInt8 sb_ircal_msm_end : 1;
    UInt8 sb_ircal_pb : 1;
    UInt8 xtalcal_end : 1;
    UInt8 ircal_wanted_image_end_status : 1;
    UInt8 unused_d7_6 : 2;
#endif
    }bF;
}TDA18260_Reg_shared_status_1;
#define TDA18260_REG_ADD_shared_status_1 (0x17)
#define TDA18260_REG_SZ_shared_status_1  (0x01)

/* TDA18260 Register shared_status_2 0x18 */
typedef union
{
    UInt8 shared_status_2;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 unused_d7_3 : 5;
    UInt8 sb_lo_lock : 1;
    UInt8 sa_lo_lock : 1;
    UInt8 agc1_smooth_transition_on : 1;
#else
    UInt8 agc1_smooth_transition_on : 1;
    UInt8 sa_lo_lock : 1;
    UInt8 sb_lo_lock : 1;
    UInt8 unused_d7_3 : 5;
#endif
    }bF;
}TDA18260_Reg_shared_status_2;
#define TDA18260_REG_ADD_shared_status_2 (0x18)
#define TDA18260_REG_SZ_shared_status_2  (0x01)

/* TDA18260 Register thermo_status 0x19 */
typedef union
{
    UInt8 thermo_status;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 unused_d7 : 1;
    UInt8 thermo_data : 7;
#else
    UInt8 thermo_data : 7;
    UInt8 unused_d7 : 1;
#endif
    }bF;
}TDA18260_Reg_thermo_status;
#define TDA18260_REG_ADD_thermo_status (0x19)
#define TDA18260_REG_SZ_thermo_status  (0x01)

/* TDA18260 Register agc1_speed_and_co 0x1C */
typedef union
{
    UInt8 agc1_speed_and_co;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 unused_d7_6 : 2;
    UInt8 pd_detect1 : 1;
    UInt8 agc1_smooth_only_sw1 : 1;
    UInt8 agc1_speed_down : 1;
    UInt8 agc1_speed_up : 1;
    UInt8 agc1_speed : 2;
#else
    UInt8 agc1_speed : 2;
    UInt8 agc1_speed_up : 1;
    UInt8 agc1_speed_down : 1;
    UInt8 agc1_smooth_only_sw1 : 1;
    UInt8 pd_detect1 : 1;
    UInt8 unused_d7_6 : 2;
#endif
    }bF;
}TDA18260_Reg_agc1_speed_and_co;
#define TDA18260_REG_ADD_agc1_speed_and_co (0x1C)
#define TDA18260_REG_SZ_agc1_speed_and_co  (0x01)

/* TDA18260 Register ir_cal_1 0x20 */
typedef union
{
    UInt8 ir_cal_1;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 ir_mode_ram_store : 1;
    UInt8 ir_loop : 3;
    UInt8 unused_d3 : 1;
    UInt8 ir_target : 3;
#else
    UInt8 ir_target : 3;
    UInt8 unused_d3 : 1;
    UInt8 ir_loop : 3;
    UInt8 ir_mode_ram_store : 1;
#endif
    }bF;
}TDA18260_Reg_ir_cal_1;
#define TDA18260_REG_ADD_ir_cal_1 (0x20)
#define TDA18260_REG_SZ_ir_cal_1  (0x01)

/* TDA18260 Register xtal_osc_1 0x23 */
typedef union
{
    UInt8 xtal_osc_1;
    struct
    {
#ifdef _TARGET_PLATFORM_MSB_FIRST
    UInt8 unused_d7_6 : 2;
    UInt8 digclk_on : 1;
    UInt8 unused_d4_2 : 3;
    UInt8 psm_agc1 : 2;
#else
    UInt8 psm_agc1 : 2;
    UInt8 unused_d4_2 : 3;
    UInt8 digclk_on : 1;
    UInt8 unused_d7_6 : 2;
#endif
    }bF;
}TDA18260_Reg_xtal_osc_1;
#define TDA18260_REG_ADD_xtal_osc_1 (0x23)
#define TDA18260_REG_SZ_xtal_osc_1  (0x01)

typedef union
{
    UInt8 xtal_osc_1;
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
}TDA18260_Dummy;

#ifdef _TMDD_TDA18260_REGMAP_DEFINED
 /* TDA18260Shared register map */
 typedef struct _TDA18260Shared_Reg_Map_t
 {
     TDA18260_Reg_id_1 Reg_id_1;                                                /* Reg@0x00 */
     TDA18260_Reg_id_2 Reg_id_2;                                                /* Reg@0x01 */
     TDA18260_Reg_id_3 Reg_id_3;                                                /* Reg@0x02 */
     TDA18260_Reg_power_state Reg_power_state;                                  /* Reg@0x03 */
     TDA18260_Reg_lock_shared_reg_ctrl Reg_lock_shared_reg_ctrl;                /* Reg@0x04 */
     TDA18260_Reg_msm_1 Reg_msm_1;                                              /* Reg@0x05 */
     TDA18260_Reg_msm_2 Reg_msm_2;                                              /* Reg@0x06 */
     TDA18260_Reg_msm_3 Reg_msm_3;                                              /* Reg@0x07 */
     TDA18260_Reg_power_ctrl Reg_power_ctrl;                                    /* Reg@0x08 */
     TDA18260_Reg_power_down Reg_power_down;                                    /* Reg@0x09 */
     TDA18260_Reg_switch_clock_ctrl Reg_switch_clock_ctrl;                      /* Reg@0x0A */
     TDA18260_Reg_switch_clock_config Reg_switch_clock_config;                  /* Reg@0x0B */
     TDA18260_Reg_agc1_ctrl Reg_agc1_ctrl;                                      /* Reg@0x0C */
     TDA18260_Reg_agc1_lt_gain Reg_agc1_lt_gain;                                /* Reg@0x0D */
     TDA18260_Reg_agc1_step Reg_agc1_step;                                      /* Reg@0x0E */
     TDA18260_Reg_agc1_g_ud Reg_agc1_g_ud;                                      /* Reg@0x0F */
     TDA18260_Reg_agc1_a_ud Reg_agc1_a_ud;                                      /* Reg@0x10 */
     TDA18260_Reg_agc1_lt_smooth_t_cst Reg_agc1_lt_smooth_t_cst;                /* Reg@0x11 */
     TDA18260_Reg_mto_smooth_t_cst Reg_mto_smooth_t_cst;                        /* Reg@0x12 */
     TDA18260_Reg_mto_smooth_state Reg_mto_smooth_state;                        /* Reg@0x13 */
     TDA18260_Reg_mto_smooth_move Reg_mto_smooth_move;                          /* Reg@0x14 */
     TDA18260_Reg_xtout_1 Reg_xtout_1;                                          /* Reg@0x15 */
     TDA18260_Reg_agc1_lt_gain_status Reg_agc1_lt_gain_status;                  /* Reg@0x16 */
     TDA18260_Reg_shared_status_1 Reg_shared_status_1;                          /* Reg@0x17 */
     TDA18260_Reg_shared_status_2 Reg_shared_status_2;                          /* Reg@0x18 */
     TDA18260_Reg_thermo_status Reg_thermo_status;                              /* Reg@0x19 */
     TDA18260_Dummy Dummy_1;                                                    /* Reg@0x1A */
     TDA18260_Dummy Dummy_2;                                                    /* Reg@0x1B */
     TDA18260_Reg_agc1_speed_and_co Reg_agc1_speed_and_co;                      /* Reg@0x1C */
     TDA18260_Dummy Dummy_4;                                                    /* Reg@0x1D */
     TDA18260_Dummy Dummy_5;                                                    /* Reg@0x1E */
     TDA18260_Dummy Dummy_6;                                                    /* Reg@0x1F */
     TDA18260_Reg_ir_cal_1 Reg_ir_cal_1;                                        /* Reg@0x20 */
     TDA18260_Dummy Dummy_8;                                                    /* Reg@0x21 */
     TDA18260_Dummy Dummy_9;                                                    /* Reg@0x22 */
     TDA18260_Reg_xtal_osc_1 Reg_xtal_osc_1;                                    /* Reg@0x23 */
 } TDA18260Shared_Reg_Map_t, *pTDA18260Shared_Reg_Map_t;
#endif


#ifdef __cplusplus
}
#endif

#endif /* _TMDD_TDA18260SHARED_REGDEF_H */
