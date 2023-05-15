/****************************************************************************
 *     Copyright (c) 1999-2014, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on              Thu Feb 20 15:33:52 2014
 *                 Full Compile MD5 Checksum 1766fea499add5f6ee91330ef96d35c5
 *                   (minus title and desc)
 *                 MD5 Checksum              4c358fb5b94802f03aec82d8df2c9afa
 *
 * Compiled with:  RDB Utility               combo_header.pl
 *                 RDB Parser                3.0
 *                 unknown                   unknown
 *                 Perl Interpreter          5.008008
 *                 Operating System          linux
 *
 *
 ***************************************************************************/



/****************************************************************************
 ***************************************************************************/

#ifndef BCHP_SDS_BL_0_H__
#define BCHP_SDS_BL_0_H__

/***************************************************************************
 *SDS_BL_0 - SDS Baud Loop Register Set
 ***************************************************************************/
#define BCHP_SDS_BL_0_BLPCTL                     0x04000140 /* Baud Loop Control (Formerly, BLSCCP,BLPCTL3,BLPCTL2,BLPCTL1) */
#define BCHP_SDS_BL_0_PFCTL                      0x04000144 /* Baud Loop Pre-filter Control */
#define BCHP_SDS_BL_0_BRSW                       0x04000148 /* Baud Loop Sweep Configuration */
#define BCHP_SDS_BL_0_BRLC                       0x0400014c /* Baud Loop Linear Coefficient */
#define BCHP_SDS_BL_0_BRIC                       0x04000150 /* Baud Loop Integrator Coefficient */
#define BCHP_SDS_BL_0_BRI                        0x04000154 /* Baud Loop Integrator Value */
#define BCHP_SDS_BL_0_BFOS                       0x04000158 /* Baud Loop Frequency Offset */
#define BCHP_SDS_BL_0_BRFO                       0x0400015c /* Baud Loop Filter Output */
#define BCHP_SDS_BL_0_BNCO                       0x04000160 /* Baud Loop NCO Contents */

/***************************************************************************
 *BLPCTL - Baud Loop Control (Formerly, BLSCCP,BLPCTL3,BLPCTL2,BLPCTL1)
 ***************************************************************************/
/* SDS_BL_0 :: BLPCTL :: clip_thd [31:24] */
#define BCHP_SDS_BL_0_BLPCTL_clip_thd_MASK                         0xff000000
#define BCHP_SDS_BL_0_BLPCTL_clip_thd_SHIFT                        24
#define BCHP_SDS_BL_0_BLPCTL_clip_thd_DEFAULT                      0x000000ff

/* SDS_BL_0 :: BLPCTL :: tv_dly [23:21] */
#define BCHP_SDS_BL_0_BLPCTL_tv_dly_MASK                           0x00e00000
#define BCHP_SDS_BL_0_BLPCTL_tv_dly_SHIFT                          21
#define BCHP_SDS_BL_0_BLPCTL_tv_dly_DEFAULT                        0x00000000

/* SDS_BL_0 :: BLPCTL :: reserved_for_eco0 [20:19] */
#define BCHP_SDS_BL_0_BLPCTL_reserved_for_eco0_MASK                0x00180000
#define BCHP_SDS_BL_0_BLPCTL_reserved_for_eco0_SHIFT               19
#define BCHP_SDS_BL_0_BLPCTL_reserved_for_eco0_DEFAULT             0x00000000

/* SDS_BL_0 :: BLPCTL :: tv_dly_ov [18:18] */
#define BCHP_SDS_BL_0_BLPCTL_tv_dly_ov_MASK                        0x00040000
#define BCHP_SDS_BL_0_BLPCTL_tv_dly_ov_SHIFT                       18
#define BCHP_SDS_BL_0_BLPCTL_tv_dly_ov_DEFAULT                     0x00000000

/* SDS_BL_0 :: BLPCTL :: mu_phase [17:17] */
#define BCHP_SDS_BL_0_BLPCTL_mu_phase_MASK                         0x00020000
#define BCHP_SDS_BL_0_BLPCTL_mu_phase_SHIFT                        17
#define BCHP_SDS_BL_0_BLPCTL_mu_phase_DEFAULT                      0x00000001

/* SDS_BL_0 :: BLPCTL :: use_valid [16:16] */
#define BCHP_SDS_BL_0_BLPCTL_use_valid_MASK                        0x00010000
#define BCHP_SDS_BL_0_BLPCTL_use_valid_SHIFT                       16
#define BCHP_SDS_BL_0_BLPCTL_use_valid_DEFAULT                     0x00000000

/* SDS_BL_0 :: BLPCTL :: x2_vclk [15:15] */
#define BCHP_SDS_BL_0_BLPCTL_x2_vclk_MASK                          0x00008000
#define BCHP_SDS_BL_0_BLPCTL_x2_vclk_SHIFT                         15
#define BCHP_SDS_BL_0_BLPCTL_x2_vclk_DEFAULT                       0x00000000

/* SDS_BL_0 :: BLPCTL :: ip_sel [14:14] */
#define BCHP_SDS_BL_0_BLPCTL_ip_sel_MASK                           0x00004000
#define BCHP_SDS_BL_0_BLPCTL_ip_sel_SHIFT                          14
#define BCHP_SDS_BL_0_BLPCTL_ip_sel_DEFAULT                        0x00000000

/* SDS_BL_0 :: BLPCTL :: reserved_for_eco1 [13:13] */
#define BCHP_SDS_BL_0_BLPCTL_reserved_for_eco1_MASK                0x00002000
#define BCHP_SDS_BL_0_BLPCTL_reserved_for_eco1_SHIFT               13
#define BCHP_SDS_BL_0_BLPCTL_reserved_for_eco1_DEFAULT             0x00000000

/* SDS_BL_0 :: BLPCTL :: i_mu_offen [12:12] */
#define BCHP_SDS_BL_0_BLPCTL_i_mu_offen_MASK                       0x00001000
#define BCHP_SDS_BL_0_BLPCTL_i_mu_offen_SHIFT                      12
#define BCHP_SDS_BL_0_BLPCTL_i_mu_offen_DEFAULT                    0x00000000

/* SDS_BL_0 :: BLPCTL :: q_mu_offen [11:11] */
#define BCHP_SDS_BL_0_BLPCTL_q_mu_offen_MASK                       0x00000800
#define BCHP_SDS_BL_0_BLPCTL_q_mu_offen_SHIFT                      11
#define BCHP_SDS_BL_0_BLPCTL_q_mu_offen_DEFAULT                    0x00000000

/* SDS_BL_0 :: BLPCTL :: blp_obw [10:09] */
#define BCHP_SDS_BL_0_BLPCTL_blp_obw_MASK                          0x00000600
#define BCHP_SDS_BL_0_BLPCTL_blp_obw_SHIFT                         9
#define BCHP_SDS_BL_0_BLPCTL_blp_obw_DEFAULT                       0x00000000

/* SDS_BL_0 :: BLPCTL :: blp_rst [08:08] */
#define BCHP_SDS_BL_0_BLPCTL_blp_rst_MASK                          0x00000100
#define BCHP_SDS_BL_0_BLPCTL_blp_rst_SHIFT                         8
#define BCHP_SDS_BL_0_BLPCTL_blp_rst_DEFAULT                       0x00000000

/* SDS_BL_0 :: BLPCTL :: pd_ovd [07:07] */
#define BCHP_SDS_BL_0_BLPCTL_pd_ovd_MASK                           0x00000080
#define BCHP_SDS_BL_0_BLPCTL_pd_ovd_SHIFT                          7
#define BCHP_SDS_BL_0_BLPCTL_pd_ovd_DEFAULT                        0x00000000

/* SDS_BL_0 :: BLPCTL :: pd_stat [06:06] */
#define BCHP_SDS_BL_0_BLPCTL_pd_stat_MASK                          0x00000040
#define BCHP_SDS_BL_0_BLPCTL_pd_stat_SHIFT                         6
#define BCHP_SDS_BL_0_BLPCTL_pd_stat_DEFAULT                       0x00000000

/* SDS_BL_0 :: BLPCTL :: zr_stat [05:05] */
#define BCHP_SDS_BL_0_BLPCTL_zr_stat_MASK                          0x00000020
#define BCHP_SDS_BL_0_BLPCTL_zr_stat_SHIFT                         5
#define BCHP_SDS_BL_0_BLPCTL_zr_stat_DEFAULT                       0x00000000

/* SDS_BL_0 :: BLPCTL :: blp_freeze [04:04] */
#define BCHP_SDS_BL_0_BLPCTL_blp_freeze_MASK                       0x00000010
#define BCHP_SDS_BL_0_BLPCTL_blp_freeze_SHIFT                      4
#define BCHP_SDS_BL_0_BLPCTL_blp_freeze_DEFAULT                    0x00000001

/* SDS_BL_0 :: BLPCTL :: oqpsk [03:03] */
#define BCHP_SDS_BL_0_BLPCTL_oqpsk_MASK                            0x00000008
#define BCHP_SDS_BL_0_BLPCTL_oqpsk_SHIFT                           3
#define BCHP_SDS_BL_0_BLPCTL_oqpsk_DEFAULT                         0x00000000

/* SDS_BL_0 :: BLPCTL :: pass_q [02:02] */
#define BCHP_SDS_BL_0_BLPCTL_pass_q_MASK                           0x00000004
#define BCHP_SDS_BL_0_BLPCTL_pass_q_SHIFT                          2
#define BCHP_SDS_BL_0_BLPCTL_pass_q_DEFAULT                        0x00000000

/* SDS_BL_0 :: BLPCTL :: pass_i [01:01] */
#define BCHP_SDS_BL_0_BLPCTL_pass_i_MASK                           0x00000002
#define BCHP_SDS_BL_0_BLPCTL_pass_i_SHIFT                          1
#define BCHP_SDS_BL_0_BLPCTL_pass_i_DEFAULT                        0x00000000

/* SDS_BL_0 :: BLPCTL :: blp_en [00:00] */
#define BCHP_SDS_BL_0_BLPCTL_blp_en_MASK                           0x00000001
#define BCHP_SDS_BL_0_BLPCTL_blp_en_SHIFT                          0
#define BCHP_SDS_BL_0_BLPCTL_blp_en_DEFAULT                        0x00000000

/***************************************************************************
 *PFCTL - Baud Loop Pre-filter Control
 ***************************************************************************/
/* SDS_BL_0 :: PFCTL :: reserved0 [31:08] */
#define BCHP_SDS_BL_0_PFCTL_reserved0_MASK                         0xffffff00
#define BCHP_SDS_BL_0_PFCTL_reserved0_SHIFT                        8

/* SDS_BL_0 :: PFCTL :: reserved_for_eco1 [07:06] */
#define BCHP_SDS_BL_0_PFCTL_reserved_for_eco1_MASK                 0x000000c0
#define BCHP_SDS_BL_0_PFCTL_reserved_for_eco1_SHIFT                6
#define BCHP_SDS_BL_0_PFCTL_reserved_for_eco1_DEFAULT              0x00000000

/* SDS_BL_0 :: PFCTL :: pd_sel [05:03] */
#define BCHP_SDS_BL_0_PFCTL_pd_sel_MASK                            0x00000038
#define BCHP_SDS_BL_0_PFCTL_pd_sel_SHIFT                           3
#define BCHP_SDS_BL_0_PFCTL_pd_sel_DEFAULT                         0x00000000

/* SDS_BL_0 :: PFCTL :: pf_byp [02:02] */
#define BCHP_SDS_BL_0_PFCTL_pf_byp_MASK                            0x00000004
#define BCHP_SDS_BL_0_PFCTL_pf_byp_SHIFT                           2
#define BCHP_SDS_BL_0_PFCTL_pf_byp_DEFAULT                         0x00000000

/* SDS_BL_0 :: PFCTL :: reserved_for_eco2 [01:01] */
#define BCHP_SDS_BL_0_PFCTL_reserved_for_eco2_MASK                 0x00000002
#define BCHP_SDS_BL_0_PFCTL_reserved_for_eco2_SHIFT                1
#define BCHP_SDS_BL_0_PFCTL_reserved_for_eco2_DEFAULT              0x00000000

/* SDS_BL_0 :: PFCTL :: pf_ebw [00:00] */
#define BCHP_SDS_BL_0_PFCTL_pf_ebw_MASK                            0x00000001
#define BCHP_SDS_BL_0_PFCTL_pf_ebw_SHIFT                           0
#define BCHP_SDS_BL_0_PFCTL_pf_ebw_DEFAULT                         0x00000000

/***************************************************************************
 *BRSW - Baud Loop Sweep Configuration
 ***************************************************************************/
/* SDS_BL_0 :: BRSW :: sweep_range [31:16] */
#define BCHP_SDS_BL_0_BRSW_sweep_range_MASK                        0xffff0000
#define BCHP_SDS_BL_0_BRSW_sweep_range_SHIFT                       16
#define BCHP_SDS_BL_0_BRSW_sweep_range_DEFAULT                     0x00000000

/* SDS_BL_0 :: BRSW :: sweep_delta [15:00] */
#define BCHP_SDS_BL_0_BRSW_sweep_delta_MASK                        0x0000ffff
#define BCHP_SDS_BL_0_BRSW_sweep_delta_SHIFT                       0
#define BCHP_SDS_BL_0_BRSW_sweep_delta_DEFAULT                     0x00000000

/***************************************************************************
 *BRLC - Baud Loop Linear Coefficient
 ***************************************************************************/
/* SDS_BL_0 :: BRLC :: bl_lin_coef [31:08] */
#define BCHP_SDS_BL_0_BRLC_bl_lin_coef_MASK                        0xffffff00
#define BCHP_SDS_BL_0_BRLC_bl_lin_coef_SHIFT                       8
#define BCHP_SDS_BL_0_BRLC_bl_lin_coef_DEFAULT                     0x00000000

/* SDS_BL_0 :: BRLC :: reserved0 [07:00] */
#define BCHP_SDS_BL_0_BRLC_reserved0_MASK                          0x000000ff
#define BCHP_SDS_BL_0_BRLC_reserved0_SHIFT                         0

/***************************************************************************
 *BRIC - Baud Loop Integrator Coefficient
 ***************************************************************************/
/* SDS_BL_0 :: BRIC :: bl_int_coef [31:00] */
#define BCHP_SDS_BL_0_BRIC_bl_int_coef_MASK                        0xffffffff
#define BCHP_SDS_BL_0_BRIC_bl_int_coef_SHIFT                       0
#define BCHP_SDS_BL_0_BRIC_bl_int_coef_DEFAULT                     0x00000000

/***************************************************************************
 *BRI - Baud Loop Integrator Value
 ***************************************************************************/
/* SDS_BL_0 :: BRI :: bl_int [31:00] */
#define BCHP_SDS_BL_0_BRI_bl_int_MASK                              0xffffffff
#define BCHP_SDS_BL_0_BRI_bl_int_SHIFT                             0
#define BCHP_SDS_BL_0_BRI_bl_int_DEFAULT                           0x00000000

/***************************************************************************
 *BFOS - Baud Loop Frequency Offset
 ***************************************************************************/
/* SDS_BL_0 :: BFOS :: bl_fos [31:08] */
#define BCHP_SDS_BL_0_BFOS_bl_fos_MASK                             0xffffff00
#define BCHP_SDS_BL_0_BFOS_bl_fos_SHIFT                            8
#define BCHP_SDS_BL_0_BFOS_bl_fos_DEFAULT                          0x000c0000

/* SDS_BL_0 :: BFOS :: reserved0 [07:00] */
#define BCHP_SDS_BL_0_BFOS_reserved0_MASK                          0x000000ff
#define BCHP_SDS_BL_0_BFOS_reserved0_SHIFT                         0

/***************************************************************************
 *BRFO - Baud Loop Filter Output
 ***************************************************************************/
/* SDS_BL_0 :: BRFO :: bl_lpfilt_out [31:08] */
#define BCHP_SDS_BL_0_BRFO_bl_lpfilt_out_MASK                      0xffffff00
#define BCHP_SDS_BL_0_BRFO_bl_lpfilt_out_SHIFT                     8
#define BCHP_SDS_BL_0_BRFO_bl_lpfilt_out_DEFAULT                   0x00000000

/* SDS_BL_0 :: BRFO :: reserved0 [07:00] */
#define BCHP_SDS_BL_0_BRFO_reserved0_MASK                          0x000000ff
#define BCHP_SDS_BL_0_BRFO_reserved0_SHIFT                         0

/***************************************************************************
 *BNCO - Baud Loop NCO Contents
 ***************************************************************************/
/* SDS_BL_0 :: BNCO :: bl_nco [31:00] */
#define BCHP_SDS_BL_0_BNCO_bl_nco_MASK                             0xffffffff
#define BCHP_SDS_BL_0_BNCO_bl_nco_SHIFT                            0
#define BCHP_SDS_BL_0_BNCO_bl_nco_DEFAULT                          0x00000000

#endif /* #ifndef BCHP_SDS_BL_0_H__ */

/* End of File */