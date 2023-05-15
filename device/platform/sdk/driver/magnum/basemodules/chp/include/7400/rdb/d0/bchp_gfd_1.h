/***************************************************************************
 *     Copyright (c) 1999-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bchp_gfd_1.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 7/23/07 2:44p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Mon Jul 23 12:03:30 2007
 *                 MD5 Checksum         61f9c4d8dcdcd06017506dddbf23f434
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008004
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7400/rdb/d0/bchp_gfd_1.h $
 * 
 * Hydra_Software_Devel/1   7/23/07 2:44p maivu
 * PR 32842: Initial revision
 *
 ***************************************************************************/

#ifndef BCHP_GFD_1_H__
#define BCHP_GFD_1_H__

/***************************************************************************
 *GFD_1 - Graphics Feeder 1 Registers
 ***************************************************************************/
#define BCHP_GFD_1_REVISION                      0x00141400 /* Graphics Feeder Revision register */
#define BCHP_GFD_1_ENABLE                        0x00141404 /* Graphics Feeder Enable */
#define BCHP_GFD_1_LOAD_PALETTE                  0x00141408 /* Load Palette Operation */
#define BCHP_GFD_1_CTRL                          0x0014140c /* Graphics Feeder Control */
#define BCHP_GFD_1_FORMAT_DEF_1                  0x00141410 /* Graphics Feeder Format Definition 1 */
#define BCHP_GFD_1_FORMAT_DEF_2                  0x00141414 /* Graphics Feeder Format Definition 2 */
#define BCHP_GFD_1_HORIZ_FIR_SRC_STEP            0x00141418 /* Horizontal Poly-Phase Filter Source Picture Stepping Size */
#define BCHP_GFD_1_HORIZ_FIR_INIT_PHASE          0x0014141c /* Horizontal Poly-Phase Filter Source Picture Initial Phase */
#define BCHP_GFD_1_WIN_COLOR                     0x00141420 /* Window Color Value For Alpha Format */
#define BCHP_GFD_1_W_ALPHA                       0x00141424 /* Expanded Alpha Value For W Bit In W_RGB Format */
#define BCHP_GFD_1_KEY_MAX                       0x00141428 /* Color Key Max Value */
#define BCHP_GFD_1_KEY_MIN                       0x0014142c /* Color Key Min Value */
#define BCHP_GFD_1_KEY_MASK                      0x00141430 /* Color Key Mask Bits */
#define BCHP_GFD_1_KEY_ALPHA                     0x00141434 /* Color Keyed Alpha Value */
#define BCHP_GFD_1_SRC_OFFSET                    0x00141438 /* Graphics Surface Source Image Offset */
#define BCHP_GFD_1_SRC_HSIZE                     0x0014143c /* Graphics Surface Source Image Horizontal Size */
#define BCHP_GFD_1_SRC_START                     0x00141440 /* Graphics Surface Memory Starting Address */
#define BCHP_GFD_1_SRC_PITCH                     0x00141444 /* Graphics Surface Memory Pitch */
#define BCHP_GFD_1_ALPHA_START                   0x00141448 /* Alpha Plane Memory Starting Address */
#define BCHP_GFD_1_ALPHA_PITCH                   0x0014144c /* Alpha Plane Memory Pitch */
#define BCHP_GFD_1_ALPHA_OFFSET                  0x00141450 /* Alpha Plane Source Image Offset */
#define BCHP_GFD_1_PALETTE_START                 0x00141454 /* Palette Memory Starting Address */
#define BCHP_GFD_1_PALETTE_SIZE                  0x00141458 /* Palette Size */
#define BCHP_GFD_1_DISP_PIC_SIZE                 0x0014145c /* Display Image Size */
#define BCHP_GFD_1_HORIZ_FIR_COEFF_PHASE0_00_01  0x001414f0 /* Horizontal Scaler Poly-Phase Filter Phase 0 Tap 0 and 1 Coefficients */
#define BCHP_GFD_1_HORIZ_FIR_COEFF_PHASE0_02     0x001414f4 /* Horizontal Scaler Poly-Phase Filter Phase 0 Tap 2 Coefficients */
#define BCHP_GFD_1_HORIZ_FIR_COEFF_PHASE1_00_01  0x001414f8 /* Horizontal Scaler Poly-Phase Filter Phase 1 Tap 0 and 1 Coefficients */
#define BCHP_GFD_1_HORIZ_FIR_COEFF_PHASE1_02     0x001414fc /* Horizontal Scaler Poly-Phase Filter Phase 1 Tap 2 Coefficients */
#define BCHP_GFD_1_CM_C00_C01                    0x00141500 /* Color Conversion Matrix Coefficients C00 and C01 */
#define BCHP_GFD_1_CM_C02_C03                    0x00141504 /* Color Conversion Matrix Coefficients C02 and C03 */
#define BCHP_GFD_1_CM_C04                        0x00141508 /* Color Conversion Matrix Coefficients C04 */
#define BCHP_GFD_1_CM_C10_C11                    0x0014150c /* Color Conversion Matrix Coefficients C10 and C11 */
#define BCHP_GFD_1_CM_C12_C13                    0x00141510 /* Color Conversion Matrix Coefficients C12 and C13 */
#define BCHP_GFD_1_CM_C14                        0x00141514 /* Color Conversion Matrix Coefficients C14 */
#define BCHP_GFD_1_CM_C20_C21                    0x00141518 /* Color Conversion Matrix Coefficients C20 and C21 */
#define BCHP_GFD_1_CM_C22_C23                    0x0014151c /* Color Conversion Matrix Coefficients C22 and C23 */
#define BCHP_GFD_1_CM_C24                        0x00141520 /* Color Conversion Matrix Coefficients C24 */
#define BCHP_GFD_1_STATUS_CLEAR                  0x00141544 /* Graphics Feeder Status Clear */
#define BCHP_GFD_1_STATUS                        0x00141548 /* Graphics Feeder Status */
#define BCHP_GFD_1_SCRATCH0                      0x00141550 /* Graphics Feeder Scratch register 0 */
#define BCHP_GFD_1_SCRATCH1                      0x00141554 /* Graphics Feeder Scratch register 1 */

#endif /* #ifndef BCHP_GFD_1_H__ */

/* End of File */