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
 * $brcm_Workfile: bchp_sds_snr.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 5/31/11 3:22p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Tue May 31 13:59:40 2011
 *                 MD5 Checksum         b0a0062767e487fe78d25d07b5b21b78
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7344/rdb/b0/bchp_sds_snr.h $
 * 
 * Hydra_Software_Devel/2   5/31/11 3:22p albertl
 * SW7344-40: Updated to match RDB.
 *
 ***************************************************************************/

#ifndef BCHP_SDS_SNR_H__
#define BCHP_SDS_SNR_H__

/***************************************************************************
 *SDS_SNR - SDS SNR Register Set
 ***************************************************************************/
#define BCHP_SDS_SNR_SNRCTL                      0x01100500 /* SNR Control Register */
#define BCHP_SDS_SNR_SNRHT                       0x01100504 /* SNR Estimator high threshold. */
#define BCHP_SDS_SNR_SNRLT                       0x01100508 /* SNR Estimator low threshold. */
#define BCHP_SDS_SNR_SNRE                        0x0110050c /* SNR Estimate. */
#define BCHP_SDS_SNR_SNORETP                     0x01100510 /* SNOR Estimate total power. */
#define BCHP_SDS_SNR_SNORESP                     0x01100514 /* SNOR Estimate signal power. */
#define BCHP_SDS_SNR_SNORECTL                    0x01100518 /* SNORE Control Register. */

/***************************************************************************
 *SNRCTL - SNR Control Register
 ***************************************************************************/
/* SDS_SNR :: SNRCTL :: reserved0 [31:08] */
#define BCHP_SDS_SNR_SNRCTL_reserved0_MASK                         0xffffff00
#define BCHP_SDS_SNR_SNRCTL_reserved0_SHIFT                        8

/* SDS_SNR :: SNRCTL :: snr_srst [07:07] */
#define BCHP_SDS_SNR_SNRCTL_snr_srst_MASK                          0x00000080
#define BCHP_SDS_SNR_SNRCTL_snr_srst_SHIFT                         7
#define BCHP_SDS_SNR_SNRCTL_snr_srst_DEFAULT                       1

/* SDS_SNR :: SNRCTL :: snr_err_sel [06:06] */
#define BCHP_SDS_SNR_SNRCTL_snr_err_sel_MASK                       0x00000040
#define BCHP_SDS_SNR_SNRCTL_snr_err_sel_SHIFT                      6
#define BCHP_SDS_SNR_SNRCTL_snr_err_sel_DEFAULT                    0

/* SDS_SNR :: SNRCTL :: snr_estsel [05:04] */
#define BCHP_SDS_SNR_SNRCTL_snr_estsel_MASK                        0x00000030
#define BCHP_SDS_SNR_SNRCTL_snr_estsel_SHIFT                       4
#define BCHP_SDS_SNR_SNRCTL_snr_estsel_DEFAULT                     0

/* SDS_SNR :: SNRCTL :: snr_sq_mode [03:03] */
#define BCHP_SDS_SNR_SNRCTL_snr_sq_mode_MASK                       0x00000008
#define BCHP_SDS_SNR_SNRCTL_snr_sq_mode_SHIFT                      3
#define BCHP_SDS_SNR_SNRCTL_snr_sq_mode_DEFAULT                    0

/* SDS_SNR :: SNRCTL :: snr_tpsel [02:02] */
#define BCHP_SDS_SNR_SNRCTL_snr_tpsel_MASK                         0x00000004
#define BCHP_SDS_SNR_SNRCTL_snr_tpsel_SHIFT                        2
#define BCHP_SDS_SNR_SNRCTL_snr_tpsel_DEFAULT                      0

/* SDS_SNR :: SNRCTL :: snr_tc [01:00] */
#define BCHP_SDS_SNR_SNRCTL_snr_tc_MASK                            0x00000003
#define BCHP_SDS_SNR_SNRCTL_snr_tc_SHIFT                           0
#define BCHP_SDS_SNR_SNRCTL_snr_tc_DEFAULT                         0

/***************************************************************************
 *SNRHT - SNR Estimator high threshold.
 ***************************************************************************/
/* SDS_SNR :: SNRHT :: snr_thrh [31:00] */
#define BCHP_SDS_SNR_SNRHT_snr_thrh_MASK                           0xffffffff
#define BCHP_SDS_SNR_SNRHT_snr_thrh_SHIFT                          0
#define BCHP_SDS_SNR_SNRHT_snr_thrh_DEFAULT                        0

/***************************************************************************
 *SNRLT - SNR Estimator low threshold.
 ***************************************************************************/
/* SDS_SNR :: SNRLT :: snr_thrl [31:00] */
#define BCHP_SDS_SNR_SNRLT_snr_thrl_MASK                           0xffffffff
#define BCHP_SDS_SNR_SNRLT_snr_thrl_SHIFT                          0
#define BCHP_SDS_SNR_SNRLT_snr_thrl_DEFAULT                        0

/***************************************************************************
 *SNRE - SNR Estimate.
 ***************************************************************************/
/* SDS_SNR :: SNRE :: snr_est [31:00] */
#define BCHP_SDS_SNR_SNRE_snr_est_MASK                             0xffffffff
#define BCHP_SDS_SNR_SNRE_snr_est_SHIFT                            0
#define BCHP_SDS_SNR_SNRE_snr_est_DEFAULT                          0

/***************************************************************************
 *SNORETP - SNOR Estimate total power.
 ***************************************************************************/
/* SDS_SNR :: SNORETP :: reserved0 [31:26] */
#define BCHP_SDS_SNR_SNORETP_reserved0_MASK                        0xfc000000
#define BCHP_SDS_SNR_SNORETP_reserved0_SHIFT                       26

/* SDS_SNR :: SNORETP :: snore_tot_pow [25:00] */
#define BCHP_SDS_SNR_SNORETP_snore_tot_pow_MASK                    0x03ffffff
#define BCHP_SDS_SNR_SNORETP_snore_tot_pow_SHIFT                   0
#define BCHP_SDS_SNR_SNORETP_snore_tot_pow_DEFAULT                 0

/***************************************************************************
 *SNORESP - SNOR Estimate signal power.
 ***************************************************************************/
/* SDS_SNR :: SNORESP :: reserved0 [31:26] */
#define BCHP_SDS_SNR_SNORESP_reserved0_MASK                        0xfc000000
#define BCHP_SDS_SNR_SNORESP_reserved0_SHIFT                       26

/* SDS_SNR :: SNORESP :: snore_est_sig_pow [25:00] */
#define BCHP_SDS_SNR_SNORESP_snore_est_sig_pow_MASK                0x03ffffff
#define BCHP_SDS_SNR_SNORESP_snore_est_sig_pow_SHIFT               0
#define BCHP_SDS_SNR_SNORESP_snore_est_sig_pow_DEFAULT             0

/***************************************************************************
 *SNORECTL - SNORE Control Register.
 ***************************************************************************/
/* SDS_SNR :: SNORECTL :: reserved0 [31:08] */
#define BCHP_SDS_SNR_SNORECTL_reserved0_MASK                       0xffffff00
#define BCHP_SDS_SNR_SNORECTL_reserved0_SHIFT                      8

/* SDS_SNR :: SNORECTL :: snore_rstb [07:07] */
#define BCHP_SDS_SNR_SNORECTL_snore_rstb_MASK                      0x00000080
#define BCHP_SDS_SNR_SNORECTL_snore_rstb_SHIFT                     7
#define BCHP_SDS_SNR_SNORECTL_snore_rstb_DEFAULT                   1

/* SDS_SNR :: SNORECTL :: snore_power_snap [06:06] */
#define BCHP_SDS_SNR_SNORECTL_snore_power_snap_MASK                0x00000040
#define BCHP_SDS_SNR_SNORECTL_snore_power_snap_SHIFT               6
#define BCHP_SDS_SNR_SNORECTL_snore_power_snap_DEFAULT             0

/* SDS_SNR :: SNORECTL :: snore_ldpc [05:05] */
#define BCHP_SDS_SNR_SNORECTL_snore_ldpc_MASK                      0x00000020
#define BCHP_SDS_SNR_SNORECTL_snore_ldpc_SHIFT                     5
#define BCHP_SDS_SNR_SNORECTL_snore_ldpc_DEFAULT                   0

/* SDS_SNR :: SNORECTL :: update_trn_only [04:04] */
#define BCHP_SDS_SNR_SNORECTL_update_trn_only_MASK                 0x00000010
#define BCHP_SDS_SNR_SNORECTL_update_trn_only_SHIFT                4
#define BCHP_SDS_SNR_SNORECTL_update_trn_only_DEFAULT              0

/* SDS_SNR :: SNORECTL :: snore_alpha [03:00] */
#define BCHP_SDS_SNR_SNORECTL_snore_alpha_MASK                     0x0000000f
#define BCHP_SDS_SNR_SNORECTL_snore_alpha_SHIFT                    0
#define BCHP_SDS_SNR_SNORECTL_snore_alpha_DEFAULT                  0

#endif /* #ifndef BCHP_SDS_SNR_H__ */

/* End of File */