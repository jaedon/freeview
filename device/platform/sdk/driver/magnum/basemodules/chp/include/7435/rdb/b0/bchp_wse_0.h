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
 * $brcm_Workfile: bchp_wse_0.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 2/28/12 6:50p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Tue Feb 28 11:03:20 2012
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
 * $brcm_Log: /magnum/basemodules/chp/7435/rdb/b0/bchp_wse_0.h $
 * 
 * Hydra_Software_Devel/1   2/28/12 6:50p tdo
 * SW7435-40: Need 7435B0 public/central RDB (magnum) headers checked into
 * clearcase
 *
 ***************************************************************************/

#ifndef BCHP_WSE_0_H__
#define BCHP_WSE_0_H__

/***************************************************************************
 *WSE_0 - WSE_0 registers
 ***************************************************************************/
#define BCHP_WSE_0_control                       0x006b8604 /* WSS Control Register */
#define BCHP_WSE_0_wss_data                      0x006b8608 /* WSS & VPS Data 0 Register */
#define BCHP_WSE_0_vps_data_1                    0x006b860c /* VPS Data 1 Register */
#define BCHP_WSE_0_vps_control                   0x006b8610 /* VPS Control Register */
#define BCHP_WSE_0_revid                         0x006b8614 /* WSS Revision ID register */

/***************************************************************************
 *control - WSS Control Register
 ***************************************************************************/
/* WSE_0 :: control :: output_attenuation [31:24] */
#define BCHP_WSE_0_control_output_attenuation_MASK                 0xff000000
#define BCHP_WSE_0_control_output_attenuation_SHIFT                24
#define BCHP_WSE_0_control_output_attenuation_DEFAULT              0x000000a9

/* WSE_0 :: control :: AUTO_PARITY_TYP [23:23] */
#define BCHP_WSE_0_control_AUTO_PARITY_TYP_MASK                    0x00800000
#define BCHP_WSE_0_control_AUTO_PARITY_TYP_SHIFT                   23
#define BCHP_WSE_0_control_AUTO_PARITY_TYP_DEFAULT                 0x00000000
#define BCHP_WSE_0_control_AUTO_PARITY_TYP_ODD                     0
#define BCHP_WSE_0_control_AUTO_PARITY_TYP_EVEN                    1

/* WSE_0 :: control :: AUTO_PARITY_EN [22:22] */
#define BCHP_WSE_0_control_AUTO_PARITY_EN_MASK                     0x00400000
#define BCHP_WSE_0_control_AUTO_PARITY_EN_SHIFT                    22
#define BCHP_WSE_0_control_AUTO_PARITY_EN_DEFAULT                  0x00000000

/* WSE_0 :: control :: AUTO_PARITY_EN_656 [21:21] */
#define BCHP_WSE_0_control_AUTO_PARITY_EN_656_MASK                 0x00200000
#define BCHP_WSE_0_control_AUTO_PARITY_EN_656_SHIFT                21
#define BCHP_WSE_0_control_AUTO_PARITY_EN_656_DEFAULT              0x00000000

/* WSE_0 :: control :: OUTPUT_ORDER [20:19] */
#define BCHP_WSE_0_control_OUTPUT_ORDER_MASK                       0x00180000
#define BCHP_WSE_0_control_OUTPUT_ORDER_SHIFT                      19
#define BCHP_WSE_0_control_OUTPUT_ORDER_DEFAULT                    0x00000000
#define BCHP_WSE_0_control_OUTPUT_ORDER_Low_Pad_2nd                0
#define BCHP_WSE_0_control_OUTPUT_ORDER_High_Pad_1st               1
#define BCHP_WSE_0_control_OUTPUT_ORDER_High_Pad_2nd               2
#define BCHP_WSE_0_control_OUTPUT_ORDER_Low_Pad_1st                3

/* WSE_0 :: control :: anci656_enable [18:18] */
#define BCHP_WSE_0_control_anci656_enable_MASK                     0x00040000
#define BCHP_WSE_0_control_anci656_enable_SHIFT                    18
#define BCHP_WSE_0_control_anci656_enable_DEFAULT                  0x00000000

/* WSE_0 :: control :: invert_data [17:17] */
#define BCHP_WSE_0_control_invert_data_MASK                        0x00020000
#define BCHP_WSE_0_control_invert_data_SHIFT                       17
#define BCHP_WSE_0_control_invert_data_DEFAULT                     0x00000000
#define BCHP_WSE_0_control_invert_data_Off                         0
#define BCHP_WSE_0_control_invert_data_On                          1

/* WSE_0 :: control :: active_line [16:08] */
#define BCHP_WSE_0_control_active_line_MASK                        0x0001ff00
#define BCHP_WSE_0_control_active_line_SHIFT                       8
#define BCHP_WSE_0_control_active_line_DEFAULT                     0x00000016

/* WSE_0 :: control :: enable [07:07] */
#define BCHP_WSE_0_control_enable_MASK                             0x00000080
#define BCHP_WSE_0_control_enable_SHIFT                            7
#define BCHP_WSE_0_control_enable_DEFAULT                          0x00000000

/* WSE_0 :: control :: DECIMATOR_EN [06:06] */
#define BCHP_WSE_0_control_DECIMATOR_EN_MASK                       0x00000040
#define BCHP_WSE_0_control_DECIMATOR_EN_SHIFT                      6
#define BCHP_WSE_0_control_DECIMATOR_EN_DEFAULT                    0x00000000
#define BCHP_WSE_0_control_DECIMATOR_EN_DISABLE                    0
#define BCHP_WSE_0_control_DECIMATOR_EN_ENABLE                     1

/* WSE_0 :: control :: start_delay [05:00] */
#define BCHP_WSE_0_control_start_delay_MASK                        0x0000003f
#define BCHP_WSE_0_control_start_delay_SHIFT                       0
#define BCHP_WSE_0_control_start_delay_DEFAULT                     0x00000021

/***************************************************************************
 *wss_data - WSS & VPS Data 0 Register
 ***************************************************************************/
/* WSE_0 :: wss_data :: vps_byte_5 [31:24] */
#define BCHP_WSE_0_wss_data_vps_byte_5_MASK                        0xff000000
#define BCHP_WSE_0_wss_data_vps_byte_5_SHIFT                       24

/* WSE_0 :: wss_data :: vps_byte_11 [23:16] */
#define BCHP_WSE_0_wss_data_vps_byte_11_MASK                       0x00ff0000
#define BCHP_WSE_0_wss_data_vps_byte_11_SHIFT                      16

/* WSE_0 :: wss_data :: reserved0 [15:14] */
#define BCHP_WSE_0_wss_data_reserved0_MASK                         0x0000c000
#define BCHP_WSE_0_wss_data_reserved0_SHIFT                        14

/* WSE_0 :: wss_data :: wss_data [13:00] */
#define BCHP_WSE_0_wss_data_wss_data_MASK                          0x00003fff
#define BCHP_WSE_0_wss_data_wss_data_SHIFT                         0

/***************************************************************************
 *vps_data_1 - VPS Data 1 Register
 ***************************************************************************/
/* WSE_0 :: vps_data_1 :: vps_byte_12 [31:24] */
#define BCHP_WSE_0_vps_data_1_vps_byte_12_MASK                     0xff000000
#define BCHP_WSE_0_vps_data_1_vps_byte_12_SHIFT                    24

/* WSE_0 :: vps_data_1 :: vps_byte_13 [23:16] */
#define BCHP_WSE_0_vps_data_1_vps_byte_13_MASK                     0x00ff0000
#define BCHP_WSE_0_vps_data_1_vps_byte_13_SHIFT                    16

/* WSE_0 :: vps_data_1 :: vps_byte_14 [15:08] */
#define BCHP_WSE_0_vps_data_1_vps_byte_14_MASK                     0x0000ff00
#define BCHP_WSE_0_vps_data_1_vps_byte_14_SHIFT                    8

/* WSE_0 :: vps_data_1 :: vps_byte_15 [07:00] */
#define BCHP_WSE_0_vps_data_1_vps_byte_15_MASK                     0x000000ff
#define BCHP_WSE_0_vps_data_1_vps_byte_15_SHIFT                    0

/***************************************************************************
 *vps_control - VPS Control Register
 ***************************************************************************/
/* WSE_0 :: vps_control :: reserved0 [31:18] */
#define BCHP_WSE_0_vps_control_reserved0_MASK                      0xfffc0000
#define BCHP_WSE_0_vps_control_reserved0_SHIFT                     18

/* WSE_0 :: vps_control :: vps_bit_order [17:17] */
#define BCHP_WSE_0_vps_control_vps_bit_order_MASK                  0x00020000
#define BCHP_WSE_0_vps_control_vps_bit_order_SHIFT                 17
#define BCHP_WSE_0_vps_control_vps_bit_order_DEFAULT               0x00000000
#define BCHP_WSE_0_vps_control_vps_bit_order_msb_to_lsb            0
#define BCHP_WSE_0_vps_control_vps_bit_order_lsb_to_msb            1

/* WSE_0 :: vps_control :: invert_data [16:16] */
#define BCHP_WSE_0_vps_control_invert_data_MASK                    0x00010000
#define BCHP_WSE_0_vps_control_invert_data_SHIFT                   16
#define BCHP_WSE_0_vps_control_invert_data_DEFAULT                 0x00000000
#define BCHP_WSE_0_vps_control_invert_data_Off                     0
#define BCHP_WSE_0_vps_control_invert_data_On                      1

/* WSE_0 :: vps_control :: active_line [15:07] */
#define BCHP_WSE_0_vps_control_active_line_MASK                    0x0000ff80
#define BCHP_WSE_0_vps_control_active_line_SHIFT                   7
#define BCHP_WSE_0_vps_control_active_line_DEFAULT                 0x0000000f

/* WSE_0 :: vps_control :: enable [06:06] */
#define BCHP_WSE_0_vps_control_enable_MASK                         0x00000040
#define BCHP_WSE_0_vps_control_enable_SHIFT                        6
#define BCHP_WSE_0_vps_control_enable_DEFAULT                      0x00000000

/* WSE_0 :: vps_control :: start_delay [05:00] */
#define BCHP_WSE_0_vps_control_start_delay_MASK                    0x0000003f
#define BCHP_WSE_0_vps_control_start_delay_SHIFT                   0
#define BCHP_WSE_0_vps_control_start_delay_DEFAULT                 0x00000000

/***************************************************************************
 *revid - WSS Revision ID register
 ***************************************************************************/
/* WSE_0 :: revid :: reserved0 [31:16] */
#define BCHP_WSE_0_revid_reserved0_MASK                            0xffff0000
#define BCHP_WSE_0_revid_reserved0_SHIFT                           16

/* WSE_0 :: revid :: REVID [15:00] */
#define BCHP_WSE_0_revid_REVID_MASK                                0x0000ffff
#define BCHP_WSE_0_revid_REVID_SHIFT                               0
#define BCHP_WSE_0_revid_REVID_DEFAULT                             0x00003000

#endif /* #ifndef BCHP_WSE_0_H__ */

/* End of File */