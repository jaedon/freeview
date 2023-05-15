/***************************************************************************
 *     Copyright (c) 1999-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bchp_csc_1.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 9/14/09 4:09p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Fri Sep 11 19:44:01 2009
 *                 MD5 Checksum         957f01e03a68c1766fd2e8ad6484f5f9
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7468/rdb/a0/bchp_csc_1.h $
 * 
 * Hydra_Software_Devel/1   9/14/09 4:09p albertl
 * SW7468-3: Initial revision.
 *
 ***************************************************************************/

#ifndef BCHP_CSC_1_H__
#define BCHP_CSC_1_H__

/***************************************************************************
 *CSC_1 - Color Space Converter CSC_1
 ***************************************************************************/
#define BCHP_CSC_1_CSC_REV_ID                    0x00483580 /* Revision ID register */
#define BCHP_CSC_1_CSC_MODE                      0x00483588 /* Color space converter mode register */
#define BCHP_CSC_1_CSC_MIN_MAX                   0x0048358c /* Color space converter min_max clamp register */
#define BCHP_CSC_1_CSC_COEFF_C01_C00             0x00483590 /* Video Encoder Color Matrix coefficients c01 and c00 */
#define BCHP_CSC_1_CSC_COEFF_C03_C02             0x00483594 /* Video Encoder Color Matrix coefficients c03 and c02 */
#define BCHP_CSC_1_CSC_COEFF_C11_C10             0x00483598 /* Video Encoder Color Matrix coefficients c11 and c10 */
#define BCHP_CSC_1_CSC_COEFF_C13_C12             0x0048359c /* Video Encoder Color Matrix coefficients c13 and c12 */
#define BCHP_CSC_1_CSC_COEFF_C21_C20             0x004835a0 /* Video Encoder Color Matrix coefficients c21 and c20 */
#define BCHP_CSC_1_CSC_COEFF_C23_C22             0x004835a4 /* Video Encoder Color Matrix coefficients c23 and c22 */
#define BCHP_CSC_1_DITHER_CONTROL                0x004835a8 /* Color Space Converter Dither Control */
#define BCHP_CSC_1_DITHER_LFSR                   0x004835ac /* Color Space Converter Dither LFSR Control */
#define BCHP_CSC_1_DITHER_LFSR_INIT              0x004835b0 /* Color Space Converter Dither LFSR Init value and control */

#endif /* #ifndef BCHP_CSC_1_H__ */

/* End of File */