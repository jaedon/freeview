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
 * $brcm_Workfile: bchp_sds_vit_0.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 12/12/11 3:40p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Mon Dec 12 12:00:26 2011
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
 * $brcm_Log: /magnum/basemodules/chp/7346/rdb/b0/bchp_sds_vit_0.h $
 * 
 * Hydra_Software_Devel/2   12/12/11 3:40p pntruong
 * SW7346-143: Resynced with central rdb.
 *
 ***************************************************************************/

#ifndef BCHP_SDS_VIT_0_H__
#define BCHP_SDS_VIT_0_H__

/***************************************************************************
 *SDS_VIT_0 - SDS Viterbi Decoder Register Set 0
 ***************************************************************************/
#define BCHP_SDS_VIT_0_VTCTL                     0x00700400 /* Viterbi Decoder Configuration Register (Formerly,VTCTL2,VTCTL1) */
#define BCHP_SDS_VIT_0_V10                       0x00700404 /* Viterbi decoder sync error threshold 1/0 (Formerly, V1,V0, DVB/DTV 2/3, DC 1/2 */
#define BCHP_SDS_VIT_0_V32                       0x00700408 /* Viterbi decoder sync error threshold 3/2 (Formerly, V3,V2, DVB/DTV 5/6, DC 2/3 */
#define BCHP_SDS_VIT_0_V54                       0x0070040c /* Viterbi decoder sync error threshold 5/4 (Formerly, V5,V4, DVB/DTV 7/8, DC 4/5 */
#define BCHP_SDS_VIT_0_V76                       0x00700410 /* Viterbi decoder sync error threshold 7/6 (Formerly, V7,V6, DVB/DTV unused, DC 7/8 */
#define BCHP_SDS_VIT_0_VINT                      0x00700414 /* Viterbi decoder sync integration period */
#define BCHP_SDS_VIT_0_VCNT                      0x00700418 /* Viterbi decoder integration counter value */
#define BCHP_SDS_VIT_0_VSTC                      0x0070041c /* Viterbi decoder state change counter values */
#define BCHP_SDS_VIT_0_VST                       0x00700420 /* Viterbi decoder internal status monitor */
#define BCHP_SDS_VIT_0_VREC                      0x00700424 /* Viterbi re-encoding register error value */
#define BCHP_SDS_VIT_0_VRCV                      0x00700428 /* Viterbi re-encoding register count value */

/***************************************************************************
 *VTCTL - Viterbi Decoder Configuration Register (Formerly,VTCTL2,VTCTL1)
 ***************************************************************************/
/* SDS_VIT_0 :: VTCTL :: reserved0 [31:16] */
#define BCHP_SDS_VIT_0_VTCTL_reserved0_MASK                        0xffff0000
#define BCHP_SDS_VIT_0_VTCTL_reserved0_SHIFT                       16

/* SDS_VIT_0 :: VTCTL :: vit_rec_cnt_snap [15:15] */
#define BCHP_SDS_VIT_0_VTCTL_vit_rec_cnt_snap_MASK                 0x00008000
#define BCHP_SDS_VIT_0_VTCTL_vit_rec_cnt_snap_SHIFT                15
#define BCHP_SDS_VIT_0_VTCTL_vit_rec_cnt_snap_DEFAULT              0x00000000

/* SDS_VIT_0 :: VTCTL :: Reset [14:14] */
#define BCHP_SDS_VIT_0_VTCTL_Reset_MASK                            0x00004000
#define BCHP_SDS_VIT_0_VTCTL_Reset_SHIFT                           14
#define BCHP_SDS_VIT_0_VTCTL_Reset_DEFAULT                         0x00000001

/* SDS_VIT_0 :: VTCTL :: vit_rec_ov [13:13] */
#define BCHP_SDS_VIT_0_VTCTL_vit_rec_ov_MASK                       0x00002000
#define BCHP_SDS_VIT_0_VTCTL_vit_rec_ov_SHIFT                      13
#define BCHP_SDS_VIT_0_VTCTL_vit_rec_ov_DEFAULT                    0x00000000

/* SDS_VIT_0 :: VTCTL :: freeze [12:12] */
#define BCHP_SDS_VIT_0_VTCTL_freeze_MASK                           0x00001000
#define BCHP_SDS_VIT_0_VTCTL_freeze_SHIFT                          12
#define BCHP_SDS_VIT_0_VTCTL_freeze_DEFAULT                        0x00000000

/* SDS_VIT_0 :: VTCTL :: Bpsk [11:11] */
#define BCHP_SDS_VIT_0_VTCTL_Bpsk_MASK                             0x00000800
#define BCHP_SDS_VIT_0_VTCTL_Bpsk_SHIFT                            11
#define BCHP_SDS_VIT_0_VTCTL_Bpsk_DEFAULT                          0x00000000

/* SDS_VIT_0 :: VTCTL :: spltmx [10:10] */
#define BCHP_SDS_VIT_0_VTCTL_spltmx_MASK                           0x00000400
#define BCHP_SDS_VIT_0_VTCTL_spltmx_SHIFT                          10
#define BCHP_SDS_VIT_0_VTCTL_spltmx_DEFAULT                        0x00000000

/* SDS_VIT_0 :: VTCTL :: opqsk [09:09] */
#define BCHP_SDS_VIT_0_VTCTL_opqsk_MASK                            0x00000200
#define BCHP_SDS_VIT_0_VTCTL_opqsk_SHIFT                           9
#define BCHP_SDS_VIT_0_VTCTL_opqsk_DEFAULT                         0x00000000

/* SDS_VIT_0 :: VTCTL :: dciflg [08:08] */
#define BCHP_SDS_VIT_0_VTCTL_dciflg_MASK                           0x00000100
#define BCHP_SDS_VIT_0_VTCTL_dciflg_SHIFT                          8
#define BCHP_SDS_VIT_0_VTCTL_dciflg_DEFAULT                        0x00000000

/* SDS_VIT_0 :: VTCTL :: slpsyn [07:07] */
#define BCHP_SDS_VIT_0_VTCTL_slpsyn_MASK                           0x00000080
#define BCHP_SDS_VIT_0_VTCTL_slpsyn_SHIFT                          7
#define BCHP_SDS_VIT_0_VTCTL_slpsyn_DEFAULT                        0x00000001

/* SDS_VIT_0 :: VTCTL :: invctl [06:05] */
#define BCHP_SDS_VIT_0_VTCTL_invctl_MASK                           0x00000060
#define BCHP_SDS_VIT_0_VTCTL_invctl_SHIFT                          5
#define BCHP_SDS_VIT_0_VTCTL_invctl_DEFAULT                        0x00000003

/* SDS_VIT_0 :: VTCTL :: vitmod [04:00] */
#define BCHP_SDS_VIT_0_VTCTL_vitmod_MASK                           0x0000001f
#define BCHP_SDS_VIT_0_VTCTL_vitmod_SHIFT                          0
#define BCHP_SDS_VIT_0_VTCTL_vitmod_DEFAULT                        0x00000010

/***************************************************************************
 *V10 - Viterbi decoder sync error threshold 1/0 (Formerly, V1,V0, DVB/DTV 2/3, DC 1/2
 ***************************************************************************/
/* SDS_VIT_0 :: V10 :: vth1 [31:16] */
#define BCHP_SDS_VIT_0_V10_vth1_MASK                               0xffff0000
#define BCHP_SDS_VIT_0_V10_vth1_SHIFT                              16
#define BCHP_SDS_VIT_0_V10_vth1_DEFAULT                            0x00000600

/* SDS_VIT_0 :: V10 :: vth0 [15:00] */
#define BCHP_SDS_VIT_0_V10_vth0_MASK                               0x0000ffff
#define BCHP_SDS_VIT_0_V10_vth0_SHIFT                              0
#define BCHP_SDS_VIT_0_V10_vth0_DEFAULT                            0x00000480

/***************************************************************************
 *V32 - Viterbi decoder sync error threshold 3/2 (Formerly, V3,V2, DVB/DTV 5/6, DC 2/3
 ***************************************************************************/
/* SDS_VIT_0 :: V32 :: vth3 [31:16] */
#define BCHP_SDS_VIT_0_V32_vth3_MASK                               0xffff0000
#define BCHP_SDS_VIT_0_V32_vth3_SHIFT                              16
#define BCHP_SDS_VIT_0_V32_vth3_DEFAULT                            0x00000866

/* SDS_VIT_0 :: V32 :: vth2 [15:00] */
#define BCHP_SDS_VIT_0_V32_vth2_MASK                               0x0000ffff
#define BCHP_SDS_VIT_0_V32_vth2_SHIFT                              0
#define BCHP_SDS_VIT_0_V32_vth2_DEFAULT                            0x00000780

/***************************************************************************
 *V54 - Viterbi decoder sync error threshold 5/4 (Formerly, V5,V4, DVB/DTV 7/8, DC 4/5
 ***************************************************************************/
/* SDS_VIT_0 :: V54 :: vth5 [31:16] */
#define BCHP_SDS_VIT_0_V54_vth5_MASK                               0xffff0000
#define BCHP_SDS_VIT_0_V54_vth5_SHIFT                              16
#define BCHP_SDS_VIT_0_V54_vth5_DEFAULT                            0x00000900

/* SDS_VIT_0 :: V54 :: vth4 [15:00] */
#define BCHP_SDS_VIT_0_V54_vth4_MASK                               0x0000ffff
#define BCHP_SDS_VIT_0_V54_vth4_SHIFT                              0
#define BCHP_SDS_VIT_0_V54_vth4_DEFAULT                            0x00000880

/***************************************************************************
 *V76 - Viterbi decoder sync error threshold 7/6 (Formerly, V7,V6, DVB/DTV unused, DC 7/8
 ***************************************************************************/
/* SDS_VIT_0 :: V76 :: vth7 [31:16] */
#define BCHP_SDS_VIT_0_V76_vth7_MASK                               0xffff0000
#define BCHP_SDS_VIT_0_V76_vth7_SHIFT                              16
#define BCHP_SDS_VIT_0_V76_vth7_DEFAULT                            0x00000900

/* SDS_VIT_0 :: V76 :: vth6 [15:00] */
#define BCHP_SDS_VIT_0_V76_vth6_MASK                               0x0000ffff
#define BCHP_SDS_VIT_0_V76_vth6_SHIFT                              0
#define BCHP_SDS_VIT_0_V76_vth6_DEFAULT                            0x00000900

/***************************************************************************
 *VINT - Viterbi decoder sync integration period
 ***************************************************************************/
/* SDS_VIT_0 :: VINT :: Vint [31:16] */
#define BCHP_SDS_VIT_0_VINT_Vint_MASK                              0xffff0000
#define BCHP_SDS_VIT_0_VINT_Vint_SHIFT                             16
#define BCHP_SDS_VIT_0_VINT_Vint_DEFAULT                           0x000027e7

/* SDS_VIT_0 :: VINT :: reserved0 [15:00] */
#define BCHP_SDS_VIT_0_VINT_reserved0_MASK                         0x0000ffff
#define BCHP_SDS_VIT_0_VINT_reserved0_SHIFT                        0

/***************************************************************************
 *VCNT - Viterbi decoder integration counter value
 ***************************************************************************/
/* SDS_VIT_0 :: VCNT :: Vcnt [31:16] */
#define BCHP_SDS_VIT_0_VCNT_Vcnt_MASK                              0xffff0000
#define BCHP_SDS_VIT_0_VCNT_Vcnt_SHIFT                             16
#define BCHP_SDS_VIT_0_VCNT_Vcnt_DEFAULT                           0x00000000

/* SDS_VIT_0 :: VCNT :: reserved0 [15:00] */
#define BCHP_SDS_VIT_0_VCNT_reserved0_MASK                         0x0000ffff
#define BCHP_SDS_VIT_0_VCNT_reserved0_SHIFT                        0

/***************************************************************************
 *VSTC - Viterbi decoder state change counter values
 ***************************************************************************/
/* SDS_VIT_0 :: VSTC :: Vstc [31:16] */
#define BCHP_SDS_VIT_0_VSTC_Vstc_MASK                              0xffff0000
#define BCHP_SDS_VIT_0_VSTC_Vstc_SHIFT                             16
#define BCHP_SDS_VIT_0_VSTC_Vstc_DEFAULT                           0x00000000

/* SDS_VIT_0 :: VSTC :: reserved0 [15:00] */
#define BCHP_SDS_VIT_0_VSTC_reserved0_MASK                         0x0000ffff
#define BCHP_SDS_VIT_0_VSTC_reserved0_SHIFT                        0

/***************************************************************************
 *VST - Viterbi decoder internal status monitor
 ***************************************************************************/
/* SDS_VIT_0 :: VST :: reserved_for_eco0 [31:30] */
#define BCHP_SDS_VIT_0_VST_reserved_for_eco0_MASK                  0xc0000000
#define BCHP_SDS_VIT_0_VST_reserved_for_eco0_SHIFT                 30
#define BCHP_SDS_VIT_0_VST_reserved_for_eco0_DEFAULT               0x00000000

/* SDS_VIT_0 :: VST :: spinv_state [29:28] */
#define BCHP_SDS_VIT_0_VST_spinv_state_MASK                        0x30000000
#define BCHP_SDS_VIT_0_VST_spinv_state_SHIFT                       28
#define BCHP_SDS_VIT_0_VST_spinv_state_DEFAULT                     0x00000000

/* SDS_VIT_0 :: VST :: punct_mode [27:24] */
#define BCHP_SDS_VIT_0_VST_punct_mode_MASK                         0x0f000000
#define BCHP_SDS_VIT_0_VST_punct_mode_SHIFT                        24
#define BCHP_SDS_VIT_0_VST_punct_mode_DEFAULT                      0x00000000

/* SDS_VIT_0 :: VST :: reserved_for_eco1 [23:23] */
#define BCHP_SDS_VIT_0_VST_reserved_for_eco1_MASK                  0x00800000
#define BCHP_SDS_VIT_0_VST_reserved_for_eco1_SHIFT                 23
#define BCHP_SDS_VIT_0_VST_reserved_for_eco1_DEFAULT               0x00000000

/* SDS_VIT_0 :: VST :: punct_dly [22:19] */
#define BCHP_SDS_VIT_0_VST_punct_dly_MASK                          0x00780000
#define BCHP_SDS_VIT_0_VST_punct_dly_SHIFT                         19
#define BCHP_SDS_VIT_0_VST_punct_dly_DEFAULT                       0x00000000

/* SDS_VIT_0 :: VST :: phs_rot [18:17] */
#define BCHP_SDS_VIT_0_VST_phs_rot_MASK                            0x00060000
#define BCHP_SDS_VIT_0_VST_phs_rot_SHIFT                           17
#define BCHP_SDS_VIT_0_VST_phs_rot_DEFAULT                         0x00000000

/* SDS_VIT_0 :: VST :: Bpsk_timing [16:16] */
#define BCHP_SDS_VIT_0_VST_Bpsk_timing_MASK                        0x00010000
#define BCHP_SDS_VIT_0_VST_Bpsk_timing_SHIFT                       16
#define BCHP_SDS_VIT_0_VST_Bpsk_timing_DEFAULT                     0x00000000

/* SDS_VIT_0 :: VST :: reserved2 [15:00] */
#define BCHP_SDS_VIT_0_VST_reserved2_MASK                          0x0000ffff
#define BCHP_SDS_VIT_0_VST_reserved2_SHIFT                         0

/***************************************************************************
 *VREC - Viterbi re-encoding register error value
 ***************************************************************************/
/* SDS_VIT_0 :: VREC :: Vrec [31:00] */
#define BCHP_SDS_VIT_0_VREC_Vrec_MASK                              0xffffffff
#define BCHP_SDS_VIT_0_VREC_Vrec_SHIFT                             0
#define BCHP_SDS_VIT_0_VREC_Vrec_DEFAULT                           0x00000000

/***************************************************************************
 *VRCV - Viterbi re-encoding register count value
 ***************************************************************************/
/* SDS_VIT_0 :: VRCV :: Vrcv [31:00] */
#define BCHP_SDS_VIT_0_VRCV_Vrcv_MASK                              0xffffffff
#define BCHP_SDS_VIT_0_VRCV_Vrcv_SHIFT                             0
#define BCHP_SDS_VIT_0_VRCV_Vrcv_DEFAULT                           0x00000000

#endif /* #ifndef BCHP_SDS_VIT_0_H__ */

/* End of File */