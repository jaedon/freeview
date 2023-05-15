/***************************************************************************
 *     Copyright (c) 1999-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bchp_mcif.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 6/14/12 1:28p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Wed Jun 13 16:39:03 2012
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
 * $brcm_Log: /magnum/basemodules/chp/7429/rdb/b0/bchp_mcif.h $
 * 
 * Hydra_Software_Devel/1   6/14/12 1:28p pntruong
 * SW7429-185: Initial version.
 *
 ***************************************************************************/

#ifndef BCHP_MCIF_H__
#define BCHP_MCIF_H__

/***************************************************************************
 *MCIF - M-CARD CPU Interface
 ***************************************************************************/
#define BCHP_MCIF_CONTROL                        0x00406c00 /* MCIF CONTROL REGISTER */
#define BCHP_MCIF_TX_BUF_PTR                     0x00406c04 /* STARTING ADDRESS OF DATA TO BE WRITTEN TO M-CARD */
#define BCHP_MCIF_RX_BUF_PTR                     0x00406c08 /* STARTING ADDRESS OF BUFFER FOR DATA READ FROM M-CARD */
#define BCHP_MCIF_TX_LEN                         0x00406c0c /* M-CARD WRITE LENGTH REGISTER */
#define BCHP_MCIF_TX_CTRL                        0x00406c10 /* M-CARD WRITE CONTROL REGISTER */
#define BCHP_MCIF_RX_TEST                        0x00406c14 /* M-CARD READ DATA STATUS FOR TEST */
#define BCHP_MCIF_RX_STATUS                      0x00406c18 /* M-CARD HOST READY TO RECEIVE STATUS REGISTER */
#define BCHP_MCIF_RX_DATA_STATUS                 0x00406c1c /* M-CARD READ DATA STATUS REGISTER */
#define BCHP_MCIF_SPARE                          0x00406c20 /* RESERVED */

/***************************************************************************
 *CONTROL - MCIF CONTROL REGISTER
 ***************************************************************************/
/* MCIF :: CONTROL :: reserved0 [31:11] */
#define BCHP_MCIF_CONTROL_reserved0_MASK                           0xfffff800
#define BCHP_MCIF_CONTROL_reserved0_SHIFT                          11

/* MCIF :: CONTROL :: ignore_hr [10:10] */
#define BCHP_MCIF_CONTROL_ignore_hr_MASK                           0x00000400
#define BCHP_MCIF_CONTROL_ignore_hr_SHIFT                          10
#define BCHP_MCIF_CONTROL_ignore_hr_DEFAULT                        0x00000000

/* MCIF :: CONTROL :: ignore_cr [09:09] */
#define BCHP_MCIF_CONTROL_ignore_cr_MASK                           0x00000200
#define BCHP_MCIF_CONTROL_ignore_cr_SHIFT                          9
#define BCHP_MCIF_CONTROL_ignore_cr_DEFAULT                        0x00000000

/* MCIF :: CONTROL :: sisp_en [08:08] */
#define BCHP_MCIF_CONTROL_sisp_en_MASK                             0x00000100
#define BCHP_MCIF_CONTROL_sisp_en_SHIFT                            8
#define BCHP_MCIF_CONTROL_sisp_en_DEFAULT                          0x00000000

/* MCIF :: CONTROL :: sisp [07:06] */
#define BCHP_MCIF_CONTROL_sisp_MASK                                0x000000c0
#define BCHP_MCIF_CONTROL_sisp_SHIFT                               6
#define BCHP_MCIF_CONTROL_sisp_DEFAULT                             0x00000000

/* MCIF :: CONTROL :: endian_override [05:05] */
#define BCHP_MCIF_CONTROL_endian_override_MASK                     0x00000020
#define BCHP_MCIF_CONTROL_endian_override_SHIFT                    5
#define BCHP_MCIF_CONTROL_endian_override_DEFAULT                  0x00000000

/* MCIF :: CONTROL :: endian [04:04] */
#define BCHP_MCIF_CONTROL_endian_MASK                              0x00000010
#define BCHP_MCIF_CONTROL_endian_SHIFT                             4
#define BCHP_MCIF_CONTROL_endian_DEFAULT                           0x00000000

/* MCIF :: CONTROL :: tx_dis [03:03] */
#define BCHP_MCIF_CONTROL_tx_dis_MASK                              0x00000008
#define BCHP_MCIF_CONTROL_tx_dis_SHIFT                             3
#define BCHP_MCIF_CONTROL_tx_dis_DEFAULT                           0x00000000

/* MCIF :: CONTROL :: rx_dis [02:02] */
#define BCHP_MCIF_CONTROL_rx_dis_MASK                              0x00000004
#define BCHP_MCIF_CONTROL_rx_dis_SHIFT                             2
#define BCHP_MCIF_CONTROL_rx_dis_DEFAULT                           0x00000000

/* MCIF :: CONTROL :: sclk_en [01:01] */
#define BCHP_MCIF_CONTROL_sclk_en_MASK                             0x00000002
#define BCHP_MCIF_CONTROL_sclk_en_SHIFT                            1
#define BCHP_MCIF_CONTROL_sclk_en_DEFAULT                          0x00000000

/* MCIF :: CONTROL :: sctl_en [00:00] */
#define BCHP_MCIF_CONTROL_sctl_en_MASK                             0x00000001
#define BCHP_MCIF_CONTROL_sctl_en_SHIFT                            0
#define BCHP_MCIF_CONTROL_sctl_en_DEFAULT                          0x00000000

/***************************************************************************
 *TX_BUF_PTR - STARTING ADDRESS OF DATA TO BE WRITTEN TO M-CARD
 ***************************************************************************/
/* MCIF :: TX_BUF_PTR :: tx_buf_ptr [31:00] */
#define BCHP_MCIF_TX_BUF_PTR_tx_buf_ptr_MASK                       0xffffffff
#define BCHP_MCIF_TX_BUF_PTR_tx_buf_ptr_SHIFT                      0
#define BCHP_MCIF_TX_BUF_PTR_tx_buf_ptr_DEFAULT                    0x00000000

/***************************************************************************
 *RX_BUF_PTR - STARTING ADDRESS OF BUFFER FOR DATA READ FROM M-CARD
 ***************************************************************************/
/* MCIF :: RX_BUF_PTR :: rx_buf_ptr [31:00] */
#define BCHP_MCIF_RX_BUF_PTR_rx_buf_ptr_MASK                       0xffffffff
#define BCHP_MCIF_RX_BUF_PTR_rx_buf_ptr_SHIFT                      0
#define BCHP_MCIF_RX_BUF_PTR_rx_buf_ptr_DEFAULT                    0x00000000

/***************************************************************************
 *TX_LEN - M-CARD WRITE LENGTH REGISTER
 ***************************************************************************/
/* MCIF :: TX_LEN :: reserved0 [31:13] */
#define BCHP_MCIF_TX_LEN_reserved0_MASK                            0xffffe000
#define BCHP_MCIF_TX_LEN_reserved0_SHIFT                           13

/* MCIF :: TX_LEN :: tx_len [12:00] */
#define BCHP_MCIF_TX_LEN_tx_len_MASK                               0x00001fff
#define BCHP_MCIF_TX_LEN_tx_len_SHIFT                              0
#define BCHP_MCIF_TX_LEN_tx_len_DEFAULT                            0x00000000

/***************************************************************************
 *TX_CTRL - M-CARD WRITE CONTROL REGISTER
 ***************************************************************************/
/* MCIF :: TX_CTRL :: reserved0 [31:09] */
#define BCHP_MCIF_TX_CTRL_reserved0_MASK                           0xfffffe00
#define BCHP_MCIF_TX_CTRL_reserved0_SHIFT                          9

/* MCIF :: TX_CTRL :: go [08:08] */
#define BCHP_MCIF_TX_CTRL_go_MASK                                  0x00000100
#define BCHP_MCIF_TX_CTRL_go_SHIFT                                 8
#define BCHP_MCIF_TX_CTRL_go_DEFAULT                               0x00000000

/* MCIF :: TX_CTRL :: host_x7 [07:07] */
#define BCHP_MCIF_TX_CTRL_host_x7_MASK                             0x00000080
#define BCHP_MCIF_TX_CTRL_host_x7_SHIFT                            7
#define BCHP_MCIF_TX_CTRL_host_x7_DEFAULT                          0x00000000

/* MCIF :: TX_CTRL :: reserved1 [06:06] */
#define BCHP_MCIF_TX_CTRL_reserved1_MASK                           0x00000040
#define BCHP_MCIF_TX_CTRL_reserved1_SHIFT                          6

/* MCIF :: TX_CTRL :: host_ec [05:05] */
#define BCHP_MCIF_TX_CTRL_host_ec_MASK                             0x00000020
#define BCHP_MCIF_TX_CTRL_host_ec_SHIFT                            5
#define BCHP_MCIF_TX_CTRL_host_ec_DEFAULT                          0x00000000

/* MCIF :: TX_CTRL :: host_bl [04:04] */
#define BCHP_MCIF_TX_CTRL_host_bl_MASK                             0x00000010
#define BCHP_MCIF_TX_CTRL_host_bl_SHIFT                            4
#define BCHP_MCIF_TX_CTRL_host_bl_DEFAULT                          0x00000000

/* MCIF :: TX_CTRL :: host_bf [03:03] */
#define BCHP_MCIF_TX_CTRL_host_bf_MASK                             0x00000008
#define BCHP_MCIF_TX_CTRL_host_bf_SHIFT                            3
#define BCHP_MCIF_TX_CTRL_host_bf_DEFAULT                          0x00000000

/* MCIF :: TX_CTRL :: reserved2 [02:02] */
#define BCHP_MCIF_TX_CTRL_reserved2_MASK                           0x00000004
#define BCHP_MCIF_TX_CTRL_reserved2_SHIFT                          2

/* MCIF :: TX_CTRL :: force_er [01:01] */
#define BCHP_MCIF_TX_CTRL_force_er_MASK                            0x00000002
#define BCHP_MCIF_TX_CTRL_force_er_SHIFT                           1
#define BCHP_MCIF_TX_CTRL_force_er_DEFAULT                         0x00000000

/* MCIF :: TX_CTRL :: host_x0 [00:00] */
#define BCHP_MCIF_TX_CTRL_host_x0_MASK                             0x00000001
#define BCHP_MCIF_TX_CTRL_host_x0_SHIFT                            0
#define BCHP_MCIF_TX_CTRL_host_x0_DEFAULT                          0x00000000

/***************************************************************************
 *RX_TEST - M-CARD READ DATA STATUS FOR TEST
 ***************************************************************************/
/* MCIF :: RX_TEST :: reserved0 [31:23] */
#define BCHP_MCIF_RX_TEST_reserved0_MASK                           0xff800000
#define BCHP_MCIF_RX_TEST_reserved0_SHIFT                          23

/* MCIF :: RX_TEST :: rx_test_iqb [22:17] */
#define BCHP_MCIF_RX_TEST_rx_test_iqb_MASK                         0x007e0000
#define BCHP_MCIF_RX_TEST_rx_test_iqb_SHIFT                        17

/* MCIF :: RX_TEST :: reserved1 [16:13] */
#define BCHP_MCIF_RX_TEST_reserved1_MASK                           0x0001e000
#define BCHP_MCIF_RX_TEST_reserved1_SHIFT                          13

/* MCIF :: RX_TEST :: rx_test_len [12:00] */
#define BCHP_MCIF_RX_TEST_rx_test_len_MASK                         0x00001fff
#define BCHP_MCIF_RX_TEST_rx_test_len_SHIFT                        0
#define BCHP_MCIF_RX_TEST_rx_test_len_DEFAULT                      0x00000000

/***************************************************************************
 *RX_STATUS - M-CARD HOST READY TO RECEIVE STATUS REGISTER
 ***************************************************************************/
/* MCIF :: RX_STATUS :: reserved0 [31:01] */
#define BCHP_MCIF_RX_STATUS_reserved0_MASK                         0xfffffffe
#define BCHP_MCIF_RX_STATUS_reserved0_SHIFT                        1

/* MCIF :: RX_STATUS :: host_hr [00:00] */
#define BCHP_MCIF_RX_STATUS_host_hr_MASK                           0x00000001
#define BCHP_MCIF_RX_STATUS_host_hr_SHIFT                          0
#define BCHP_MCIF_RX_STATUS_host_hr_DEFAULT                        0x00000000

/***************************************************************************
 *RX_DATA_STATUS - M-CARD READ DATA STATUS REGISTER
 ***************************************************************************/
/* MCIF :: RX_DATA_STATUS :: reserved0 [31:22] */
#define BCHP_MCIF_RX_DATA_STATUS_reserved0_MASK                    0xffc00000
#define BCHP_MCIF_RX_DATA_STATUS_reserved0_SHIFT                   22

/* MCIF :: RX_DATA_STATUS :: card_iqb [21:17] */
#define BCHP_MCIF_RX_DATA_STATUS_card_iqb_MASK                     0x003e0000
#define BCHP_MCIF_RX_DATA_STATUS_card_iqb_SHIFT                    17
#define BCHP_MCIF_RX_DATA_STATUS_card_iqb_DEFAULT                  0x00000000

/* MCIF :: RX_DATA_STATUS :: reserved1 [16:13] */
#define BCHP_MCIF_RX_DATA_STATUS_reserved1_MASK                    0x0001e000
#define BCHP_MCIF_RX_DATA_STATUS_reserved1_SHIFT                   13

/* MCIF :: RX_DATA_STATUS :: rx_data_len [12:00] */
#define BCHP_MCIF_RX_DATA_STATUS_rx_data_len_MASK                  0x00001fff
#define BCHP_MCIF_RX_DATA_STATUS_rx_data_len_SHIFT                 0
#define BCHP_MCIF_RX_DATA_STATUS_rx_data_len_DEFAULT               0x00000000

/***************************************************************************
 *SPARE - RESERVED
 ***************************************************************************/
/* MCIF :: SPARE :: reserved0 [31:16] */
#define BCHP_MCIF_SPARE_reserved0_MASK                             0xffff0000
#define BCHP_MCIF_SPARE_reserved0_SHIFT                            16

/* MCIF :: SPARE :: SPARE [15:02] */
#define BCHP_MCIF_SPARE_SPARE_MASK                                 0x0000fffc
#define BCHP_MCIF_SPARE_SPARE_SHIFT                                2
#define BCHP_MCIF_SPARE_SPARE_DEFAULT                              0x00000000

/* MCIF :: SPARE :: undo_fix_card_len [01:01] */
#define BCHP_MCIF_SPARE_undo_fix_card_len_MASK                     0x00000002
#define BCHP_MCIF_SPARE_undo_fix_card_len_SHIFT                    1
#define BCHP_MCIF_SPARE_undo_fix_card_len_DEFAULT                  0x00000000

/* MCIF :: SPARE :: undo_fix_rx_clr [00:00] */
#define BCHP_MCIF_SPARE_undo_fix_rx_clr_MASK                       0x00000001
#define BCHP_MCIF_SPARE_undo_fix_rx_clr_SHIFT                      0
#define BCHP_MCIF_SPARE_undo_fix_rx_clr_DEFAULT                    0x00000000

#endif /* #ifndef BCHP_MCIF_H__ */

/* End of File */