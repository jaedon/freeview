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
 * $brcm_Workfile: bchp_hscl_3.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 10/11/11 7:06p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Tue Oct 11 11:14:19 2011
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
 * $brcm_Log: /magnum/basemodules/chp/7435/rdb/a0/bchp_hscl_3.h $
 * 
 * Hydra_Software_Devel/1   10/11/11 7:06p pntruong
 * SW7435-3: Synced up with central rdb.
 *
 ***************************************************************************/

#ifndef BCHP_HSCL_3_H__
#define BCHP_HSCL_3_H__

/***************************************************************************
 *HSCL_3 - BVN_MADR_TOP HSCL Registers 3
 ***************************************************************************/
#define BCHP_HSCL_3_REVISION_ID                  0x00690400 /* Scaler Revision register */
#define BCHP_HSCL_3_HW_CONFIGURATION             0x00690404 /* Hardware Configuration Register */
#define BCHP_HSCL_3_TOP_CONTROL                  0x00690408 /* Scaler Top Level Control register */
#define BCHP_HSCL_3_HORIZ_CONTROL                0x0069040c /* Video Horizontal Scaler Control register */
#define BCHP_HSCL_3_BVB_IN_SIZE                  0x00690410 /* BVB Input Picture Size Information */
#define BCHP_HSCL_3_PIC_OFFSET                   0x00690414 /* BVB Input Picture OFFSET Information for 2D or 3D left view */
#define BCHP_HSCL_3_PIC_OFFSET_R                 0x00690418 /* BVB Input Picture OFFSET Information for 3D right view */
#define BCHP_HSCL_3_SRC_PIC_SIZE                 0x0069041c /* Scaler Source Picture Size Information */
#define BCHP_HSCL_3_DEST_PIC_SIZE                0x00690420 /* Scaler Destination Picture Size Information */
#define BCHP_HSCL_3_SRC_PIC_HORIZ_PAN_SCAN       0x00690424 /* Scaler Source Picture Horizontal Pan/Scan Information */
#define BCHP_HSCL_3_HORIZ_FIR_LUMA_SRC_PIC_OFFSET 0x00690428 /* Horizontal Poly-Phase Filter Source Picture Luma Offset */
#define BCHP_HSCL_3_HORIZ_FIR_CHROMA_SRC_PIC_OFFSET 0x0069042c /* Horizontal Poly-Phase Filter Source Picture Chroma Offset */
#define BCHP_HSCL_3_HORIZ_FIR_INIT_PHASE_ACC     0x00690430 /* Horizontal Poly-Phase Filter Inital Phase Accumulate Value */
#define BCHP_HSCL_3_HORIZ_FIR_INIT_PHASE_ACC_R   0x00690434 /* Horizontal Poly-Phase Filter Initial Phase Accumulate Value */
#define BCHP_HSCL_3_HORIZ_FIR_INIT_STEP_FRAC     0x00690438 /* Horizontal Poly-Phase Filter Initial Stepping Size (fractional part) for Region 0 */
#define BCHP_HSCL_3_HORIZ_FIR_INIT_STEP_INT      0x0069043c /* Horizontal Poly-Phase Filter Initial Stepping Size (integer part) for Region 0 */
#define BCHP_HSCL_3_HORIZ_FIR_DEST_PIC_REGION_0_STEP_DELTA 0x00690440 /* Horizontal Poly-Phase Filter Picture Delta Increment for Region 0 Stepping Size */
#define BCHP_HSCL_3_HORIZ_FIR_DEST_PIC_REGION_2_STEP_DELTA 0x00690444 /* Horizontal Poly-Phase Filter Picture Delta Increment for Region 2 Stepping Size */
#define BCHP_HSCL_3_HORIZ_DEST_PIC_REGION_N1_END 0x00690448 /* Horizontal Poly-Phase Filter Destination Region N1 Ending Position */
#define BCHP_HSCL_3_HORIZ_DEST_PIC_REGION_0_END  0x0069044c /* Horizontal Poly-Phase Filter Destination Region 0 Ending Position */
#define BCHP_HSCL_3_HORIZ_DEST_PIC_REGION_1_END  0x00690450 /* Horizontal Poly-Phase Filter Destination Region 1 Ending Position */
#define BCHP_HSCL_3_HORIZ_DEST_PIC_REGION_2_END  0x00690454 /* Horizontal Poly-Phase Filter Destination Region 2 Ending Position */
#define BCHP_HSCL_3_HORIZ_DEST_PIC_REGION_3_END  0x00690458 /* Horizontal Poly-Phase Filter Destination Region 3 Ending Position */
#define BCHP_HSCL_3_DERINGING                    0x0069045c /* Horizontal only Scaler Deringing Control */
#define BCHP_HSCL_3_VIDEO_3D_MODE                0x00690460 /* Video 3D mode */
#define BCHP_HSCL_3_ENABLE                       0x00690464 /* Video Scaler Enable */
#define BCHP_HSCL_3_TEST_PORT_CONTROL            0x00690468 /* Testportl control register */
#define BCHP_HSCL_3_TEST_PORT_DATA               0x0069046c /* Testport data register */
#define BCHP_HSCL_3_SCRATCH_0                    0x00690470 /* Scaler Scratch register 0 */
#define BCHP_HSCL_3_SCRATCH_1                    0x00690474 /* Scaler Scratch register 1 */
#define BCHP_HSCL_3_BVB_IN_STATUS_CLEAR          0x00690478 /* Scaler Broadcom Video Bus Input Status Clear */
#define BCHP_HSCL_3_BVB_IN_STATUS                0x0069047c /* Scaler Broadcom Video Bus Input Status */
#define BCHP_HSCL_3_HORIZ_FIR_LUMA_COEFF_PHASE0_00_01 0x00690740 /* Horizontal Scaler 12-tap Poly-Phase Filter Phase 0 Tap 0 and 1 LUMA Coefficients */
#define BCHP_HSCL_3_HORIZ_FIR_LUMA_COEFF_PHASE0_02_03 0x00690744 /* Horizontal Scaler 12-tap Poly-Phase Filter Phase 0 Tap 2 and 3 LUMA Coefficients */
#define BCHP_HSCL_3_HORIZ_FIR_LUMA_COEFF_PHASE0_04_05 0x00690748 /* Horizontal Scaler 12-tap Poly-Phase Filter Phase 0 Tap 4 and 5 LUMA Coefficients */
#define BCHP_HSCL_3_HORIZ_FIR_LUMA_COEFF_PHASE1_00_01 0x0069074c /* Horizontal Scaler 12-tap Poly-Phase Filter Phase 1 Tap 0 and 1 LUMA Coefficients */
#define BCHP_HSCL_3_HORIZ_FIR_LUMA_COEFF_PHASE1_02_03 0x00690750 /* Horizontal Scaler 12-tap Poly-Phase Filter Phase 1 Tap 2 and 3 LUMA Coefficients */
#define BCHP_HSCL_3_HORIZ_FIR_LUMA_COEFF_PHASE1_04_05 0x00690754 /* Horizontal Scaler 12-tap Poly-Phase Filter Phase 1 Tap 4 and 5 LUMA Coefficients */
#define BCHP_HSCL_3_HORIZ_FIR_LUMA_COEFF_PHASE2_00_01 0x00690758 /* Horizontal Scaler 12-tap Poly-Phase Filter Phase 2 Tap 0 and 1 LUMA Coefficients */
#define BCHP_HSCL_3_HORIZ_FIR_LUMA_COEFF_PHASE2_02_03 0x0069075c /* Horizontal Scaler 12-tap Poly-Phase Filter Phase 2 Tap 2 and 3 LUMA Coefficients */
#define BCHP_HSCL_3_HORIZ_FIR_LUMA_COEFF_PHASE2_04_05 0x00690760 /* Horizontal Scaler 12-tap Poly-Phase Filter Phase 2 Tap 4 and 5 LUMA Coefficients */
#define BCHP_HSCL_3_HORIZ_FIR_LUMA_COEFF_PHASE3_00_01 0x00690764 /* Horizontal Scaler 12-tap Poly-Phase Filter Phase 3 Tap 0 and 1 LUMA Coefficients */
#define BCHP_HSCL_3_HORIZ_FIR_LUMA_COEFF_PHASE3_02_03 0x00690768 /* Horizontal Scaler 12-tap Poly-Phase Filter Phase 3 Tap 2 and 3 LUMA Coefficients */
#define BCHP_HSCL_3_HORIZ_FIR_LUMA_COEFF_PHASE3_04_05 0x0069076c /* Horizontal Scaler 12-tap Poly-Phase Filter Phase 3 Tap 4 and 5 LUMA Coefficients */
#define BCHP_HSCL_3_HORIZ_FIR_LUMA_COEFF_PHASE4_00_01 0x00690770 /* Horizontal Scaler 12-tap Poly-Phase Filter Phase 4 Tap 0 and 1 LUMA Coefficients */
#define BCHP_HSCL_3_HORIZ_FIR_LUMA_COEFF_PHASE4_02_03 0x00690774 /* Horizontal Scaler 12-tap Poly-Phase Filter Phase 4 Tap 2 and 3 LUMA Coefficients */
#define BCHP_HSCL_3_HORIZ_FIR_LUMA_COEFF_PHASE4_04_05 0x00690778 /* Horizontal Scaler 12-tap Poly-Phase Filter Phase 4 Tap 4 and 5 LUMA Coefficients */
#define BCHP_HSCL_3_HORIZ_FIR_LUMA_COEFF_PHASE5_00_01 0x0069077c /* Horizontal Scaler 12-tap Poly-Phase Filter Phase 5 Tap 0 and 1 LUMA Coefficients */
#define BCHP_HSCL_3_HORIZ_FIR_LUMA_COEFF_PHASE5_02_03 0x00690780 /* Horizontal Scaler 12-tap Poly-Phase Filter Phase 5 Tap 2 and 3 LUMA Coefficients */
#define BCHP_HSCL_3_HORIZ_FIR_LUMA_COEFF_PHASE5_04_05 0x00690784 /* Horizontal Scaler 12-tap Poly-Phase Filter Phase 5 Tap 4 and 5 LUMA Coefficients */
#define BCHP_HSCL_3_HORIZ_FIR_LUMA_COEFF_PHASE6_00_01 0x00690788 /* Horizontal Scaler 12-tap Poly-Phase Filter Phase 6 Tap 0 and 1 LUMA Coefficients */
#define BCHP_HSCL_3_HORIZ_FIR_LUMA_COEFF_PHASE6_02_03 0x0069078c /* Horizontal Scaler 12-tap Poly-Phase Filter Phase 6 Tap 2 and 3 LUMA Coefficients */
#define BCHP_HSCL_3_HORIZ_FIR_LUMA_COEFF_PHASE6_04_05 0x00690790 /* Horizontal Scaler 12-tap Poly-Phase Filter Phase 6 Tap 4 and 5 LUMA Coefficients */
#define BCHP_HSCL_3_HORIZ_FIR_LUMA_COEFF_PHASE7_00_01 0x00690794 /* Horizontal Scaler 12-tap Poly-Phase Filter Phase 7 Tap 0 and 1 LUMA Coefficients */
#define BCHP_HSCL_3_HORIZ_FIR_LUMA_COEFF_PHASE7_02_03 0x00690798 /* Horizontal Scaler 12-tap Poly-Phase Filter Phase 7 Tap 2 and 3 LUMA Coefficients */
#define BCHP_HSCL_3_HORIZ_FIR_LUMA_COEFF_PHASE7_04_05 0x0069079c /* Horizontal Scaler 12-tap Poly-Phase Filter Phase 7 Tap 4 and 5 LUMA Coefficients */
#define BCHP_HSCL_3_HORIZ_FIR_CHROMA_COEFF_PHASE0_00_01 0x006907a0 /* Horizontal Scaler 12-tap Poly-Phase Filter Phase 0 Tap 0 and 1 CHROMA Coefficients */
#define BCHP_HSCL_3_HORIZ_FIR_CHROMA_COEFF_PHASE0_02_03 0x006907a4 /* Horizontal Scaler 12-tap Poly-Phase Filter Phase 0 Tap 2 and 3 CHROMA Coefficients */
#define BCHP_HSCL_3_HORIZ_FIR_CHROMA_COEFF_PHASE0_04_05 0x006907a8 /* Horizontal Scaler 12-tap Poly-Phase Filter Phase 0 Tap 4 and 5 CHROMA Coefficients */
#define BCHP_HSCL_3_HORIZ_FIR_CHROMA_COEFF_PHASE1_00_01 0x006907ac /* Horizontal Scaler 12-tap Poly-Phase Filter Phase 1 Tap 0 and 1 CHROMA Coefficients */
#define BCHP_HSCL_3_HORIZ_FIR_CHROMA_COEFF_PHASE1_02_03 0x006907b0 /* Horizontal Scaler 12-tap Poly-Phase Filter Phase 1 Tap 2 and 3 CHROMA Coefficients */
#define BCHP_HSCL_3_HORIZ_FIR_CHROMA_COEFF_PHASE1_04_05 0x006907b4 /* Horizontal Scaler 12-tap Poly-Phase Filter Phase 1 Tap 4 and 5 CHROMA Coefficients */
#define BCHP_HSCL_3_HORIZ_FIR_CHROMA_COEFF_PHASE2_00_01 0x006907b8 /* Horizontal Scaler 12-tap Poly-Phase Filter Phase 2 Tap 0 and 1 CHROMA Coefficients */
#define BCHP_HSCL_3_HORIZ_FIR_CHROMA_COEFF_PHASE2_02_03 0x006907bc /* Horizontal Scaler 12-tap Poly-Phase Filter Phase 2 Tap 2 and 3 CHROMA Coefficients */
#define BCHP_HSCL_3_HORIZ_FIR_CHROMA_COEFF_PHASE2_04_05 0x006907c0 /* Horizontal Scaler 12-tap Poly-Phase Filter Phase 2 Tap 4 and 5 CHROMA Coefficients */
#define BCHP_HSCL_3_HORIZ_FIR_CHROMA_COEFF_PHASE3_00_01 0x006907c4 /* Horizontal Scaler 12-tap Poly-Phase Filter Phase 3 Tap 0 and 1 CHROMA Coefficients */
#define BCHP_HSCL_3_HORIZ_FIR_CHROMA_COEFF_PHASE3_02_03 0x006907c8 /* Horizontal Scaler 12-tap Poly-Phase Filter Phase 3 Tap 2 and 3 CHROMA Coefficients */
#define BCHP_HSCL_3_HORIZ_FIR_CHROMA_COEFF_PHASE3_04_05 0x006907cc /* Horizontal Scaler 12-tap Poly-Phase Filter Phase 3 Tap 4 and 5 CHROMA Coefficients */
#define BCHP_HSCL_3_HORIZ_FIR_CHROMA_COEFF_PHASE4_00_01 0x006907d0 /* Horizontal Scaler 12-tap Poly-Phase Filter Phase 4 Tap 0 and 1 CHROMA Coefficients */
#define BCHP_HSCL_3_HORIZ_FIR_CHROMA_COEFF_PHASE4_02_03 0x006907d4 /* Horizontal Scaler 12-tap Poly-Phase Filter Phase 4 Tap 2 and 3 CHROMA Coefficients */
#define BCHP_HSCL_3_HORIZ_FIR_CHROMA_COEFF_PHASE4_04_05 0x006907d8 /* Horizontal Scaler 12-tap Poly-Phase Filter Phase 4 Tap 4 and 5 CHROMA Coefficients */
#define BCHP_HSCL_3_HORIZ_FIR_CHROMA_COEFF_PHASE5_00_01 0x006907dc /* Horizontal Scaler 12-tap Poly-Phase Filter Phase 5 Tap 0 and 1 CHROMA Coefficients */
#define BCHP_HSCL_3_HORIZ_FIR_CHROMA_COEFF_PHASE5_02_03 0x006907e0 /* Horizontal Scaler 12-tap Poly-Phase Filter Phase 5 Tap 2 and 3 CHROMA Coefficients */
#define BCHP_HSCL_3_HORIZ_FIR_CHROMA_COEFF_PHASE5_04_05 0x006907e4 /* Horizontal Scaler 12-tap Poly-Phase Filter Phase 5 Tap 4 and 5 CHROMA Coefficients */
#define BCHP_HSCL_3_HORIZ_FIR_CHROMA_COEFF_PHASE6_00_01 0x006907e8 /* Horizontal Scaler 12-tap Poly-Phase Filter Phase 6 Tap 0 and 1 CHROMA Coefficients */
#define BCHP_HSCL_3_HORIZ_FIR_CHROMA_COEFF_PHASE6_02_03 0x006907ec /* Horizontal Scaler 12-tap Poly-Phase Filter Phase 6 Tap 2 and 3 CHROMA Coefficients */
#define BCHP_HSCL_3_HORIZ_FIR_CHROMA_COEFF_PHASE6_04_05 0x006907f0 /* Horizontal Scaler 12-tap Poly-Phase Filter Phase 6 Tap 4 and 5 CHROMA Coefficients */
#define BCHP_HSCL_3_HORIZ_FIR_CHROMA_COEFF_PHASE7_00_01 0x006907f4 /* Horizontal Scaler 12-tap Poly-Phase Filter Phase 7 Tap 0 and 1 CHROMA Coefficients */
#define BCHP_HSCL_3_HORIZ_FIR_CHROMA_COEFF_PHASE7_02_03 0x006907f8 /* Horizontal Scaler 12-tap Poly-Phase Filter Phase 7 Tap 2 and 3 CHROMA Coefficients */
#define BCHP_HSCL_3_HORIZ_FIR_CHROMA_COEFF_PHASE7_04_05 0x006907fc /* Horizontal Scaler 12-tap Poly-Phase Filter Phase 7 Tap 4 and 5 CHROMA Coefficients */

#endif /* #ifndef BCHP_HSCL_3_H__ */

/* End of File */