/*
  Copyright (C) 2006-2009 NXP B.V., All Rights Reserved.
  This source code and any compilation or derivative thereof is the proprietary
  information of NXP B.V. and is confidential in nature. Under no circumstances
  is this software to be  exposed to or placed under an Open Source License of
  any type without the expressed written permission of NXP B.V.
 *
 * \file          tmddTDA18260Shared_RegAccess.h
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


#ifndef _TMDD_TDA18260SHARED_REGACCESS_H
#define _TMDD_TDA18260SHARED_REGACCESS_H

#ifdef __cplusplus
extern "C"
{
#endif

/*============================================================================*/
/* Exported functions:                                                        */
/*============================================================================*/

extern tmErrorCode_t
tmddTDA18260Shared_Get_ident_1
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_master_notslave
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_ident_2
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_minor_rev
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_major_rev
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_por
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Set_lock_shared_reg
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_lock_shared_reg
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Set_msm_mode
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_msm_mode
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Set_shared_reg_done
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_shared_reg_done
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Set_msm_rst
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_msm_rst
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Set_msm_launch
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_msm_launch
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Set_xtal_cal_launch
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_xtal_cal_launch
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Set_sm
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_sm
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Set_agc1_digck_on
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_agc1_digck_on
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Set_pd_agc1
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_pd_agc1
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Set_pd_buf1b
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_pd_buf1b
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Set_pd_xtal
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_pd_xtal
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Set_pd_lt
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_pd_lt
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Set_thermo_on
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_thermo_on
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Set_sa_enable_sd_ck_out
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_sa_enable_sd_ck_out
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Set_sb_enable_sd_ck_out
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_sb_enable_sd_ck_out
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Set_sa_force_switch_dig_not_sd
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_sa_force_switch_dig_not_sd
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Set_sb_force_switch_dig_not_sd
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_sb_force_switch_dig_not_sd
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Set_switch_i2c_clock
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_switch_i2c_clock
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Set_switch_sd_dig_clock
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_switch_sd_dig_clock
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Set_agc1_gain_force
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_agc1_gain_force
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Set_agc1_loop_off
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_agc1_loop_off
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Set_agc1_bypass
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_agc1_bypass
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Set_lt_offset
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_lt_offset
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Set_lt_manual
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_lt_manual
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Set_agc1_gain
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_agc1_gain
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Set_lt_gain
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_lt_gain
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Set_agc1_up_step
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_agc1_up_step
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Set_agc1_down_step
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_agc1_down_step
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Set_agc1_g_up_down
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_agc1_g_up_down
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Set_agc1_a_up_down
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_agc1_a_up_down
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Set_agc1_smooth_t_cst
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_agc1_smooth_t_cst
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Set_lt_smooth_t_cst
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_lt_smooth_t_cst
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Set_hp_mto_t_cst
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_hp_mto_t_cst
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Set_rfsw_mto_lt_rfin
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_rfsw_mto_lt_rfin
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Set_hp_mto1_state
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_hp_mto1_state
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Set_hp_mto2_state
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_hp_mto2_state
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Set_hp_mto1_move
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_hp_mto1_move
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Set_hp_mto2_move
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_hp_mto2_move
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Set_xtal_xtout_drive
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_xtal_xtout_drive
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Set_xtal_xtout_mode
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_xtal_xtout_mode
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_agc1_gain_read
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_lt_gain_read
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Set_sa_ircal_msm_end
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_sa_ircal_msm_end
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Set_sa_ircal_pb
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_sa_ircal_pb
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Set_sb_ircal_msm_end
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_sb_ircal_msm_end
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Set_sb_ircal_pb
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_sb_ircal_pb
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Set_xtalcal_end
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_xtalcal_end
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Set_ircal_wanted_image_end_status
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_ircal_wanted_image_end_status
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_agc1_smooth_transition_on
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_sa_lo_lock
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_sb_lo_lock
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_thermo_data
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Set_pd_detect1
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_pd_detect1
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Set_ir_target
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_ir_target
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Set_ir_loop
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_ir_loop
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Shared_Set_psm_agc1
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Shared_Get_psm_agc1
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


#ifdef __cplusplus
}
#endif

#endif /* _TMDD_TDA18260SHARED_REGACCESS_H */
