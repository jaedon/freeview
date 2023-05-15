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
 * $brcm_Workfile: bchp_pm_aon.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 1/17/12 4:24p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Tue Jan 17 13:26:47 2012
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
 * $brcm_Log: /magnum/basemodules/chp/7425/rdb/b2/bchp_pm_aon.h $
 * 
 * Hydra_Software_Devel/1   1/17/12 4:24p vanessah
 * SW7425-2202: add B2 headers
 *
 ***************************************************************************/

#ifndef BCHP_PM_AON_H__
#define BCHP_PM_AON_H__

/***************************************************************************
 *PM_AON - Peripheral Module Configuration
 ***************************************************************************/
#define BCHP_PM_AON_CONFIG                       0x00409040 /* PERIPHERAL MODULE CONFIGURATION REGISTER */
#define BCHP_PM_AON_CLK_CTRL                     0x00409044 /* UPG Clock control register */
#define BCHP_PM_AON_RST_CTRL                     0x00409048 /* UPG reset control register */

/***************************************************************************
 *CONFIG - PERIPHERAL MODULE CONFIGURATION REGISTER
 ***************************************************************************/
/* PM_AON :: CONFIG :: reserved_for_eco0 [31:30] */
#define BCHP_PM_AON_CONFIG_reserved_for_eco0_MASK                  0xc0000000
#define BCHP_PM_AON_CONFIG_reserved_for_eco0_SHIFT                 30
#define BCHP_PM_AON_CONFIG_reserved_for_eco0_DEFAULT               0x00000000

/* PM_AON :: CONFIG :: bicap1_sel [29:28] */
#define BCHP_PM_AON_CONFIG_bicap1_sel_MASK                         0x30000000
#define BCHP_PM_AON_CONFIG_bicap1_sel_SHIFT                        28
#define BCHP_PM_AON_CONFIG_bicap1_sel_DEFAULT                      0x00000000
#define BCHP_PM_AON_CONFIG_bicap1_sel_IR_IN0                       0
#define BCHP_PM_AON_CONFIG_bicap1_sel_IR_IN1                       1
#define BCHP_PM_AON_CONFIG_bicap1_sel_UHF_RX1                      2
#define BCHP_PM_AON_CONFIG_bicap1_sel_None                         3

/* PM_AON :: CONFIG :: bicap0_sel [27:26] */
#define BCHP_PM_AON_CONFIG_bicap0_sel_MASK                         0x0c000000
#define BCHP_PM_AON_CONFIG_bicap0_sel_SHIFT                        26
#define BCHP_PM_AON_CONFIG_bicap0_sel_DEFAULT                      0x00000000
#define BCHP_PM_AON_CONFIG_bicap0_sel_IR_IN0                       0
#define BCHP_PM_AON_CONFIG_bicap0_sel_IR_IN1                       1
#define BCHP_PM_AON_CONFIG_bicap0_sel_UHF_RX1                      2
#define BCHP_PM_AON_CONFIG_bicap0_sel_None                         3

/* PM_AON :: CONFIG :: reserved_for_eco1 [25:06] */
#define BCHP_PM_AON_CONFIG_reserved_for_eco1_MASK                  0x03ffffc0
#define BCHP_PM_AON_CONFIG_reserved_for_eco1_SHIFT                 6
#define BCHP_PM_AON_CONFIG_reserved_for_eco1_DEFAULT               0x00000000

/* PM_AON :: CONFIG :: irr3_in [05:04] */
#define BCHP_PM_AON_CONFIG_irr3_in_MASK                            0x00000030
#define BCHP_PM_AON_CONFIG_irr3_in_SHIFT                           4
#define BCHP_PM_AON_CONFIG_irr3_in_DEFAULT                         0x00000000
#define BCHP_PM_AON_CONFIG_irr3_in_UHF_RX1                         0
#define BCHP_PM_AON_CONFIG_irr3_in_AON_GPIO_17                     1
#define BCHP_PM_AON_CONFIG_irr3_in_IR_IN0                          2
#define BCHP_PM_AON_CONFIG_irr3_in_IR_IN1                          3

/* PM_AON :: CONFIG :: irr2_in [03:02] */
#define BCHP_PM_AON_CONFIG_irr2_in_MASK                            0x0000000c
#define BCHP_PM_AON_CONFIG_irr2_in_SHIFT                           2
#define BCHP_PM_AON_CONFIG_irr2_in_DEFAULT                         0x00000000
#define BCHP_PM_AON_CONFIG_irr2_in_IR_IN1                          0
#define BCHP_PM_AON_CONFIG_irr2_in_UHF_RX1                         1
#define BCHP_PM_AON_CONFIG_irr2_in_AON_GPIO_17                     2
#define BCHP_PM_AON_CONFIG_irr2_in_IR_IN0                          3

/* PM_AON :: CONFIG :: irr1_in [01:00] */
#define BCHP_PM_AON_CONFIG_irr1_in_MASK                            0x00000003
#define BCHP_PM_AON_CONFIG_irr1_in_SHIFT                           0
#define BCHP_PM_AON_CONFIG_irr1_in_DEFAULT                         0x00000000
#define BCHP_PM_AON_CONFIG_irr1_in_IR_IN0                          0
#define BCHP_PM_AON_CONFIG_irr1_in_IR_IN1                          1
#define BCHP_PM_AON_CONFIG_irr1_in_UHF_RX1                         2
#define BCHP_PM_AON_CONFIG_irr1_in_AON_GPIO_17                     3

/***************************************************************************
 *CLK_CTRL - UPG Clock control register
 ***************************************************************************/
/* PM_AON :: CLK_CTRL :: reserved0 [31:16] */
#define BCHP_PM_AON_CLK_CTRL_reserved0_MASK                        0xffff0000
#define BCHP_PM_AON_CLK_CTRL_reserved0_SHIFT                       16

/* PM_AON :: CLK_CTRL :: gpio [15:15] */
#define BCHP_PM_AON_CLK_CTRL_gpio_MASK                             0x00008000
#define BCHP_PM_AON_CLK_CTRL_gpio_SHIFT                            15
#define BCHP_PM_AON_CLK_CTRL_gpio_DEFAULT                          0x00000001

/* PM_AON :: CLK_CTRL :: reserved_for_eco1 [14:14] */
#define BCHP_PM_AON_CLK_CTRL_reserved_for_eco1_MASK                0x00004000
#define BCHP_PM_AON_CLK_CTRL_reserved_for_eco1_SHIFT               14
#define BCHP_PM_AON_CLK_CTRL_reserved_for_eco1_DEFAULT             0x00000000

/* PM_AON :: CLK_CTRL :: kbd3 [13:13] */
#define BCHP_PM_AON_CLK_CTRL_kbd3_MASK                             0x00002000
#define BCHP_PM_AON_CLK_CTRL_kbd3_SHIFT                            13
#define BCHP_PM_AON_CLK_CTRL_kbd3_DEFAULT                          0x00000001

/* PM_AON :: CLK_CTRL :: kbd2 [12:12] */
#define BCHP_PM_AON_CLK_CTRL_kbd2_MASK                             0x00001000
#define BCHP_PM_AON_CLK_CTRL_kbd2_SHIFT                            12
#define BCHP_PM_AON_CLK_CTRL_kbd2_DEFAULT                          0x00000001

/* PM_AON :: CLK_CTRL :: kbd1 [11:11] */
#define BCHP_PM_AON_CLK_CTRL_kbd1_MASK                             0x00000800
#define BCHP_PM_AON_CLK_CTRL_kbd1_SHIFT                            11
#define BCHP_PM_AON_CLK_CTRL_kbd1_DEFAULT                          0x00000001

/* PM_AON :: CLK_CTRL :: icap [10:10] */
#define BCHP_PM_AON_CLK_CTRL_icap_MASK                             0x00000400
#define BCHP_PM_AON_CLK_CTRL_icap_SHIFT                            10
#define BCHP_PM_AON_CLK_CTRL_icap_DEFAULT                          0x00000001

/* PM_AON :: CLK_CTRL :: reserved_for_eco2 [09:08] */
#define BCHP_PM_AON_CLK_CTRL_reserved_for_eco2_MASK                0x00000300
#define BCHP_PM_AON_CLK_CTRL_reserved_for_eco2_SHIFT               8
#define BCHP_PM_AON_CLK_CTRL_reserved_for_eco2_DEFAULT             0x00000000

/* PM_AON :: CLK_CTRL :: mspi [07:07] */
#define BCHP_PM_AON_CLK_CTRL_mspi_MASK                             0x00000080
#define BCHP_PM_AON_CLK_CTRL_mspi_SHIFT                            7
#define BCHP_PM_AON_CLK_CTRL_mspi_DEFAULT                          0x00000001

/* PM_AON :: CLK_CTRL :: bscb [06:06] */
#define BCHP_PM_AON_CLK_CTRL_bscb_MASK                             0x00000040
#define BCHP_PM_AON_CLK_CTRL_bscb_SHIFT                            6
#define BCHP_PM_AON_CLK_CTRL_bscb_DEFAULT                          0x00000001

/* PM_AON :: CLK_CTRL :: bsca [05:05] */
#define BCHP_PM_AON_CLK_CTRL_bsca_MASK                             0x00000020
#define BCHP_PM_AON_CLK_CTRL_bsca_SHIFT                            5
#define BCHP_PM_AON_CLK_CTRL_bsca_DEFAULT                          0x00000001

/* PM_AON :: CLK_CTRL :: reserved_for_eco3 [04:02] */
#define BCHP_PM_AON_CLK_CTRL_reserved_for_eco3_MASK                0x0000001c
#define BCHP_PM_AON_CLK_CTRL_reserved_for_eco3_SHIFT               2
#define BCHP_PM_AON_CLK_CTRL_reserved_for_eco3_DEFAULT             0x00000000

/* PM_AON :: CLK_CTRL :: ldk [01:01] */
#define BCHP_PM_AON_CLK_CTRL_ldk_MASK                              0x00000002
#define BCHP_PM_AON_CLK_CTRL_ldk_SHIFT                             1
#define BCHP_PM_AON_CLK_CTRL_ldk_DEFAULT                           0x00000001

/* PM_AON :: CLK_CTRL :: reserved_for_eco4 [00:00] */
#define BCHP_PM_AON_CLK_CTRL_reserved_for_eco4_MASK                0x00000001
#define BCHP_PM_AON_CLK_CTRL_reserved_for_eco4_SHIFT               0
#define BCHP_PM_AON_CLK_CTRL_reserved_for_eco4_DEFAULT             0x00000000

/***************************************************************************
 *RST_CTRL - UPG reset control register
 ***************************************************************************/
/* PM_AON :: RST_CTRL :: reserved0 [31:16] */
#define BCHP_PM_AON_RST_CTRL_reserved0_MASK                        0xffff0000
#define BCHP_PM_AON_RST_CTRL_reserved0_SHIFT                       16

/* PM_AON :: RST_CTRL :: gpio [15:15] */
#define BCHP_PM_AON_RST_CTRL_gpio_MASK                             0x00008000
#define BCHP_PM_AON_RST_CTRL_gpio_SHIFT                            15
#define BCHP_PM_AON_RST_CTRL_gpio_DEFAULT                          0x00000000

/* PM_AON :: RST_CTRL :: reserved_for_eco1 [14:14] */
#define BCHP_PM_AON_RST_CTRL_reserved_for_eco1_MASK                0x00004000
#define BCHP_PM_AON_RST_CTRL_reserved_for_eco1_SHIFT               14
#define BCHP_PM_AON_RST_CTRL_reserved_for_eco1_DEFAULT             0x00000000

/* PM_AON :: RST_CTRL :: kbd3 [13:13] */
#define BCHP_PM_AON_RST_CTRL_kbd3_MASK                             0x00002000
#define BCHP_PM_AON_RST_CTRL_kbd3_SHIFT                            13
#define BCHP_PM_AON_RST_CTRL_kbd3_DEFAULT                          0x00000000

/* PM_AON :: RST_CTRL :: kbd2 [12:12] */
#define BCHP_PM_AON_RST_CTRL_kbd2_MASK                             0x00001000
#define BCHP_PM_AON_RST_CTRL_kbd2_SHIFT                            12
#define BCHP_PM_AON_RST_CTRL_kbd2_DEFAULT                          0x00000000

/* PM_AON :: RST_CTRL :: kbd1 [11:11] */
#define BCHP_PM_AON_RST_CTRL_kbd1_MASK                             0x00000800
#define BCHP_PM_AON_RST_CTRL_kbd1_SHIFT                            11
#define BCHP_PM_AON_RST_CTRL_kbd1_DEFAULT                          0x00000000

/* PM_AON :: RST_CTRL :: icap [10:10] */
#define BCHP_PM_AON_RST_CTRL_icap_MASK                             0x00000400
#define BCHP_PM_AON_RST_CTRL_icap_SHIFT                            10
#define BCHP_PM_AON_RST_CTRL_icap_DEFAULT                          0x00000000

/* PM_AON :: RST_CTRL :: reserved_for_eco2 [09:08] */
#define BCHP_PM_AON_RST_CTRL_reserved_for_eco2_MASK                0x00000300
#define BCHP_PM_AON_RST_CTRL_reserved_for_eco2_SHIFT               8
#define BCHP_PM_AON_RST_CTRL_reserved_for_eco2_DEFAULT             0x00000000

/* PM_AON :: RST_CTRL :: mspi [07:07] */
#define BCHP_PM_AON_RST_CTRL_mspi_MASK                             0x00000080
#define BCHP_PM_AON_RST_CTRL_mspi_SHIFT                            7
#define BCHP_PM_AON_RST_CTRL_mspi_DEFAULT                          0x00000000

/* PM_AON :: RST_CTRL :: bsce [06:06] */
#define BCHP_PM_AON_RST_CTRL_bsce_MASK                             0x00000040
#define BCHP_PM_AON_RST_CTRL_bsce_SHIFT                            6
#define BCHP_PM_AON_RST_CTRL_bsce_DEFAULT                          0x00000000

/* PM_AON :: RST_CTRL :: bscd [05:05] */
#define BCHP_PM_AON_RST_CTRL_bscd_MASK                             0x00000020
#define BCHP_PM_AON_RST_CTRL_bscd_SHIFT                            5
#define BCHP_PM_AON_RST_CTRL_bscd_DEFAULT                          0x00000000

/* PM_AON :: RST_CTRL :: reserved_for_eco3 [04:02] */
#define BCHP_PM_AON_RST_CTRL_reserved_for_eco3_MASK                0x0000001c
#define BCHP_PM_AON_RST_CTRL_reserved_for_eco3_SHIFT               2
#define BCHP_PM_AON_RST_CTRL_reserved_for_eco3_DEFAULT             0x00000000

/* PM_AON :: RST_CTRL :: ldk [01:01] */
#define BCHP_PM_AON_RST_CTRL_ldk_MASK                              0x00000002
#define BCHP_PM_AON_RST_CTRL_ldk_SHIFT                             1
#define BCHP_PM_AON_RST_CTRL_ldk_DEFAULT                           0x00000000

/* PM_AON :: RST_CTRL :: reserved_for_eco4 [00:00] */
#define BCHP_PM_AON_RST_CTRL_reserved_for_eco4_MASK                0x00000001
#define BCHP_PM_AON_RST_CTRL_reserved_for_eco4_SHIFT               0
#define BCHP_PM_AON_RST_CTRL_reserved_for_eco4_DEFAULT             0x00000000

#endif /* #ifndef BCHP_PM_AON_H__ */

/* End of File */