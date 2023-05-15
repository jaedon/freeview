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
 * $brcm_Workfile: bchp_ufe_hrc0_0.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 3/29/12 3:23p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Thu Mar 29 14:38:30 2012
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
 * $brcm_Log: /magnum/basemodules/chp/3472/rdb/a0/bchp_ufe_hrc0_0.h $
 * 
 * Hydra_Software_Devel/1   3/29/12 3:23p farshidf
 * SW3461-1: add 3472 header files
 *
 ***************************************************************************/

#ifndef BCHP_UFE_HRC0_0_H__
#define BCHP_UFE_HRC0_0_H__

/***************************************************************************
 *UFE_HRC0_0 - UFE core registers HRC0 0
 ***************************************************************************/
#define BCHP_UFE_HRC0_0_CTRL                     0x00120a00 /* clock/misc control register */
#define BCHP_UFE_HRC0_0_BYP                      0x00120a04 /* bypass register */
#define BCHP_UFE_HRC0_0_RST                      0x00120a08 /* reset control register */
#define BCHP_UFE_HRC0_0_FRZ                      0x00120a0c /* freeze control register */
#define BCHP_UFE_HRC0_0_AGC1                     0x00120a10 /* AGC1 control register */
#define BCHP_UFE_HRC0_0_AGC2                     0x00120a14 /* AGC2 control register */
#define BCHP_UFE_HRC0_0_AGC3                     0x00120a18 /* AGC3 control register */
#define BCHP_UFE_HRC0_0_AGC1_THRESH              0x00120a1c /* AGC1 threshold register */
#define BCHP_UFE_HRC0_0_AGC2_THRESH              0x00120a20 /* AGC2 threshold register */
#define BCHP_UFE_HRC0_0_AGC3_THRESH              0x00120a24 /* AGC3 threshold register */
#define BCHP_UFE_HRC0_0_AGC1_LF                  0x00120a28 /* AGC1 loop filter register */
#define BCHP_UFE_HRC0_0_AGC2_LF                  0x00120a2c /* AGC2 loop filter register */
#define BCHP_UFE_HRC0_0_AGC3_LF                  0x00120a30 /* AGC3 loop filter register */
#define BCHP_UFE_HRC0_0_IQIMB_AMP_CTRL           0x00120a34 /* IQ-Imbalance amplitude correction control register */
#define BCHP_UFE_HRC0_0_IQIMB_PHS_CTRL           0x00120a38 /* IQ-Imbalance phase     correction control register */
#define BCHP_UFE_HRC0_0_IQIMB_AMP_LF             0x00120a3c /* IQ-Imbalance amplitude loop filter register */
#define BCHP_UFE_HRC0_0_IQIMB_PHS_LF             0x00120a40 /* IQ-Imbalance phase     loop filter register */
#define BCHP_UFE_HRC0_0_DCO_CTRL                 0x00120a44 /* DCO canceller control register */
#define BCHP_UFE_HRC0_0_DCOINTI                  0x00120a48 /* DCO integrator I */
#define BCHP_UFE_HRC0_0_DCOINTQ                  0x00120a4c /* DCO integrator Q */
#define BCHP_UFE_HRC0_0_BMIX_FCW                 0x00120a50 /* FCW register for back mixer */
#define BCHP_UFE_HRC0_0_FMIX_FCW                 0x00120a54 /* FCW register for front mixer */
#define BCHP_UFE_HRC0_0_CRC_EN                   0x00120a58 /* CRC enable register */
#define BCHP_UFE_HRC0_0_CRC                      0x00120a5c /* CRC signature analyzer register */
#define BCHP_UFE_HRC0_0_LFSR_SEED                0x00120a60 /* LFSR initial seed */
#define BCHP_UFE_HRC0_0_TP                       0x00120a64 /* Testport register */
#define BCHP_UFE_HRC0_0_SPARE                    0x00120a68 /* Software spare register */

#endif /* #ifndef BCHP_UFE_HRC0_0_H__ */

/* End of File */