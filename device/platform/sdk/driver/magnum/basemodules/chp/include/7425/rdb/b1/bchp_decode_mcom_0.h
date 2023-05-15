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
 * $brcm_Workfile: bchp_decode_mcom_0.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 10/27/11 9:14p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Thu Oct 27 16:56:09 2011
 *                 MD5 Checksum         d0bb3b528cbe25f62f7a44e82cd25af7
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7425/rdb/b1/bchp_decode_mcom_0.h $
 * 
 * Hydra_Software_Devel/1   10/27/11 9:14p vanessah
 * SW7425-1620: add 7425 B0 rdb header file
 *
 ***************************************************************************/

#ifndef BCHP_DECODE_MCOM_0_H__
#define BCHP_DECODE_MCOM_0_H__

/***************************************************************************
 *DECODE_MCOM_0
 ***************************************************************************/
#define BCHP_DECODE_MCOM_0_REG_MCOM_CTL          0x00a00300 /* Motion Compensation Control */
#define BCHP_DECODE_MCOM_0_REG_MCOM_SRC_A        0x00a00304 /* Motion Compensation Source A */
#define BCHP_DECODE_MCOM_0_REG_MCOM_SRC_B        0x00a00308 /* Motion Compensation Source */
#define BCHP_DECODE_MCOM_0_REG_WPRD_VC1_PIC      0x00a0030c /* VC-1 Mocomp  Picture-Level Control */
#define BCHP_DECODE_MCOM_0_REG_WPRD_VC1_BOT_PIC  0x00a00310 /* VC-1 Mocomp Bottom Pic */
#define BCHP_DECODE_MCOM_0_REG_WPRD_SEL          0x00a00314 /* Weighted Prediction Selection */
#define BCHP_DECODE_MCOM_0_REG_WPRD_VC1_BACK_PIC 0x00a00318 /* VC-1 Mocomp Back Pic */
#define BCHP_DECODE_MCOM_0_REG_MCOM_END          0x00a0031c /* REG_MCOM_END */

#endif /* #ifndef BCHP_DECODE_MCOM_0_H__ */

/* End of File */