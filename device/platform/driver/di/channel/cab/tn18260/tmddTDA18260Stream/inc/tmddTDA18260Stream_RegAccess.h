/*
  Copyright (C) 2006-2009 NXP B.V., All Rights Reserved.
  This source code and any compilation or derivative thereof is the proprietary
  information of NXP B.V. and is confidential in nature. Under no circumstances
  is this software to be  exposed to or placed under an Open Source License of
  any type without the expressed written permission of NXP B.V.
 *
 * \file          tmddTDA18260Stream_RegAccess.h
 *
 *                CFR_FEAP#5
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


#ifndef _TMDD_TDA18260STREAM_REGACCESS_H
#define _TMDD_TDA18260STREAM_REGACCESS_H

#ifdef __cplusplus
extern "C"
{
#endif

/*============================================================================*/
/* Exported functions:                                                        */
/*============================================================================*/

extern tmErrorCode_t
tmddTDA18260Stream_Set_softreset
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_softreset
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_rssi_launch
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_rssi_launch
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_rccal_launch
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_rccal_launch
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_pd_agc1bis
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_pd_agc1bis
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_pd_rfagc_ifout
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_pd_rfagc_ifout
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_hp_bypass
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_hp_bypass
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_pd_lo_synthe
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_pd_lo_synthe
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_pd_reg2v8
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_pd_reg2v8
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_pd_buf1b
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_pd_buf1b
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_agc2_digck_on
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_agc2_digck_on
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_pd_agc2
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_pd_agc2
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_pd_agc2_detect2
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_pd_agc2_detect2
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_pd_agc2_detect3
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_pd_agc2_detect3
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_pd_agc2_detect4
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_pd_agc2_detect4
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_pd_notch
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_pd_notch
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_sm
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_sm
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_psm
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_psm
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_agc2_gain_force
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_agc2_gain_force
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_agc2_bypass
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_agc2_bypass
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_agc2_gain
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_agc2_gain
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_agc2_up_step
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_agc2_up_step
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_agc2_down_step
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_agc2_down_step
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_agc2_g_up_down
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_agc2_g_up_down
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_agc1bis_gain
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_agc1bis_gain
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_rfsw_t_cst
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_rfsw_t_cst
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_rf_filter1_mode_2
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_rf_filter1_mode_2
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_rf_filter1_mode_3
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_rf_filter1_mode_3
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_rf_filter1_move_2
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_rf_filter1_move_2
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_rf_filter1_move_3
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_rf_filter1_move_3
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_rf_filter1_mode_0
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_rf_filter1_mode_0
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_rf_filter1_mode_1
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_rf_filter1_mode_1
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_ind_div2
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_ind_div2
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_bp_ind1
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_bp_ind1
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_bp_ind2
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_bp_ind2
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_cap_l10n
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_cap_l10n
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_cap_l6n
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_cap_l6n
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_cap_hp_right
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_cap_hp_right
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_cap_hp_mid
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_cap_hp_mid
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_cap_hp_left
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_cap_hp_left
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_rf_filter2_mode
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_rf_filter2_mode
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_agc2_dc
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_agc2_dc
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_cfi_shift
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_cfi_shift
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_stob_dc
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_stob_dc
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_psm_stob
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_psm_stob
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_stob_gain
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_stob_gain
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_lf_freq
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_lf_freq
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_shift_lf_rccal
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_shift_lf_rccal
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_combi_gain_3db
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_combi_gain_3db
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_hp_freq
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_hp_freq
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_lp_fc
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_lp_fc
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_If_notch_freq
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_If_notch_freq
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_if_level
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_if_level
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_agc_low_bw
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_agc_low_bw
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_ir_freq
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_ir_freq
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_rssi_cap_val
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_rssi_cap_val
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_rssi_cap_reset_en
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_rssi_cap_reset_en
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_rssi_dicho_not
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_rssi_dicho_not
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_rssi_ck_speed
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_rssi_ck_speed
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_rssi_sinus_qam_not
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_rssi_sinus_qam_not
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_rssi_av
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_rssi_av
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_ircal_delta
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_ircal_delta
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_pll_frac_0
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_pll_frac_0
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_pll_frac_1
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_pll_frac_1
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_pll_frac_2
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_pll_frac_2
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_pll_frac_3
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_pll_frac_3
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_icp_low
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_icp_low
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_nkcorr
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_nkcorr
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_freq_prog_start
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_freq_prog_start
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_agc2_gain_read
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_lo_calc_msm_end
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_lo_calc_msm_end
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_freq_end
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_freq_end
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_rccal_msm_end
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_rccal_msm_end
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_rssi_msm_end
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_rssi_msm_end
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_rssi_sense_ok
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_rssi_sense_ok
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_rssi_alarm
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_rssi_alarm
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_rssi
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_pll_sd_rstn
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_pll_sd_rstn
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_dummy_0
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_dummy_0
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_psm_buf1b
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_psm_buf1b
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_lpf_4mhz
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_lpf_4mhz
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_dummy_1
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_dummy_1
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_pll_forcesrce
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_pll_forcesrce
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_rc_lpf_sign
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_rc_lpf_sign
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


extern tmErrorCode_t
tmddTDA18260Stream_Set_rc_lpf_offset
(
    tmUnitSelect_t    tUnit,    /* I: Unit number       */
    UInt8             uValue    /* I: item value to set */
);


extern tmErrorCode_t
tmddTDA18260Stream_Get_rc_lpf_offset
(
    tmUnitSelect_t    tUnit,     /* I: Unit number                                  */
    UInt8*            puValue    /* O: Address of the variable to output item value */
);


#ifdef __cplusplus
}
#endif

#endif /* _TMDD_TDA18260STREAM_REGACCESS_H */
