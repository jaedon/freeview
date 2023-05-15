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
 * $brcm_Workfile: bchp_itu656_csc_0.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 10/11/11 7:35p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Tue Oct 11 11:25:05 2011
 *                 MD5 Checksum         12f5dd38e0ec050efd2afb0b1c62e9d9
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7435/rdb/a0/bchp_itu656_csc_0.h $
 * 
 * Hydra_Software_Devel/1   10/11/11 7:35p pntruong
 * SW7435-3: Synced up with central rdb.
 *
 ***************************************************************************/

#ifndef BCHP_ITU656_CSC_0_H__
#define BCHP_ITU656_CSC_0_H__

/***************************************************************************
 *ITU656_CSC_0 - ITU 656 Color Space Converter
 ***************************************************************************/
#define BCHP_ITU656_CSC_0_CSC_REV_ID             0x006b5200 /* Revision ID register */
#define BCHP_ITU656_CSC_0_CSC_MODE               0x006b5208 /* Color space converter mode register */
#define BCHP_ITU656_CSC_0_CSC_MIN_MAX            0x006b520c /* Color space converter min_max clamp register */
#define BCHP_ITU656_CSC_0_CSC_COEFF_C01_C00      0x006b5210 /* Video Encoder Color Matrix coefficients c01 and c00 */
#define BCHP_ITU656_CSC_0_CSC_COEFF_C03_C02      0x006b5214 /* Video Encoder Color Matrix coefficients c03 and c02 */
#define BCHP_ITU656_CSC_0_CSC_COEFF_C11_C10      0x006b5218 /* Video Encoder Color Matrix coefficients c11 and c10 */
#define BCHP_ITU656_CSC_0_CSC_COEFF_C13_C12      0x006b521c /* Video Encoder Color Matrix coefficients c13 and c12 */
#define BCHP_ITU656_CSC_0_CSC_COEFF_C21_C20      0x006b5220 /* Video Encoder Color Matrix coefficients c21 and c20 */
#define BCHP_ITU656_CSC_0_CSC_COEFF_C23_C22      0x006b5224 /* Video Encoder Color Matrix coefficients c23 and c22 */
#define BCHP_ITU656_CSC_0_DITHER_CONTROL         0x006b5228 /* Color Space Converter Dither Control */
#define BCHP_ITU656_CSC_0_DITHER_LFSR            0x006b522c /* Color Space Converter Dither LFSR Control */
#define BCHP_ITU656_CSC_0_DITHER_LFSR_INIT       0x006b5230 /* Color Space Converter Dither LFSR Init value and control */

#endif /* #ifndef BCHP_ITU656_CSC_0_H__ */

/* End of File */