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

#ifndef BCHP_SDS_DSEC_AP_0_H__
#define BCHP_SDS_DSEC_AP_0_H__

/***************************************************************************
 *SDS_DSEC_AP_0 - SDS_DSEC AP Register Set
 ***************************************************************************/
#define BCHP_SDS_DSEC_AP_0_ANA_CTL               0x07400150 /* SDS_DSEC Analog Control */
#define BCHP_SDS_DSEC_AP_0_SW_SPARE0             0x07400154 /* SDS_DSEC CORE SW SPARE0 */
#define BCHP_SDS_DSEC_AP_0_SW_SPARE1             0x07400158 /* SDS_DSEC CORE SW SPARE1 */

/***************************************************************************
 *ANA_CTL - SDS_DSEC Analog Control
 ***************************************************************************/
/* SDS_DSEC_AP_0 :: ANA_CTL :: reserved0 [31:08] */
#define BCHP_SDS_DSEC_AP_0_ANA_CTL_reserved0_MASK                  0xffffff00
#define BCHP_SDS_DSEC_AP_0_ANA_CTL_reserved0_SHIFT                 8

/* SDS_DSEC_AP_0 :: ANA_CTL :: BYPASS_SH [07:07] */
#define BCHP_SDS_DSEC_AP_0_ANA_CTL_BYPASS_SH_MASK                  0x00000080
#define BCHP_SDS_DSEC_AP_0_ANA_CTL_BYPASS_SH_SHIFT                 7
#define BCHP_SDS_DSEC_AP_0_ANA_CTL_BYPASS_SH_DEFAULT               0x00000000

/* SDS_DSEC_AP_0 :: ANA_CTL :: PGA_GAIN [06:03] */
#define BCHP_SDS_DSEC_AP_0_ANA_CTL_PGA_GAIN_MASK                   0x00000078
#define BCHP_SDS_DSEC_AP_0_ANA_CTL_PGA_GAIN_SHIFT                  3
#define BCHP_SDS_DSEC_AP_0_ANA_CTL_PGA_GAIN_DEFAULT                0x00000000

/* SDS_DSEC_AP_0 :: ANA_CTL :: BG_ADJ [02:00] */
#define BCHP_SDS_DSEC_AP_0_ANA_CTL_BG_ADJ_MASK                     0x00000007
#define BCHP_SDS_DSEC_AP_0_ANA_CTL_BG_ADJ_SHIFT                    0
#define BCHP_SDS_DSEC_AP_0_ANA_CTL_BG_ADJ_DEFAULT                  0x00000000

/***************************************************************************
 *SW_SPARE0 - SDS_DSEC CORE SW SPARE0
 ***************************************************************************/
/* SDS_DSEC_AP_0 :: SW_SPARE0 :: SW_SPARE0 [31:00] */
#define BCHP_SDS_DSEC_AP_0_SW_SPARE0_SW_SPARE0_MASK                0xffffffff
#define BCHP_SDS_DSEC_AP_0_SW_SPARE0_SW_SPARE0_SHIFT               0
#define BCHP_SDS_DSEC_AP_0_SW_SPARE0_SW_SPARE0_DEFAULT             0x00000000

/***************************************************************************
 *SW_SPARE1 - SDS_DSEC CORE SW SPARE1
 ***************************************************************************/
/* SDS_DSEC_AP_0 :: SW_SPARE1 :: SW_SPARE1 [31:00] */
#define BCHP_SDS_DSEC_AP_0_SW_SPARE1_SW_SPARE1_MASK                0xffffffff
#define BCHP_SDS_DSEC_AP_0_SW_SPARE1_SW_SPARE1_SHIFT               0
#define BCHP_SDS_DSEC_AP_0_SW_SPARE1_SW_SPARE1_DEFAULT             0x00000000

#endif /* #ifndef BCHP_SDS_DSEC_AP_0_H__ */

/* End of File */