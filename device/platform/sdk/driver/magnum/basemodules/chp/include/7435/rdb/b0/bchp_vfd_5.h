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
 * $brcm_Workfile: bchp_vfd_5.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 2/28/12 6:29p $
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
 * $brcm_Log: /magnum/basemodules/chp/7435/rdb/b0/bchp_vfd_5.h $
 * 
 * Hydra_Software_Devel/1   2/28/12 6:29p tdo
 * SW7435-40: Need 7435B0 public/central RDB (magnum) headers checked into
 * clearcase
 *
 ***************************************************************************/

#ifndef BCHP_VFD_5_H__
#define BCHP_VFD_5_H__

/***************************************************************************
 *VFD_5 - Video Feeder 5 Registers
 ***************************************************************************/
#define BCHP_VFD_5_REVISION_ID                   0x00601a00 /* MPEG/Video Feeder Revision Register */
#define BCHP_VFD_5_FEEDER_CNTL                   0x00601a04 /* MPEG/Video Feeder Control Register */
#define BCHP_VFD_5_FIXED_COLOUR                  0x00601a08 /* MPEG/Video Feeder Fixed Colour Value Register */
#define BCHP_VFD_5_LAC_CNTL                      0x00601a0c /* MPEG/Video Feeder LAC Control Register */
#define BCHP_VFD_5_STRIDE                        0x00601a10 /* MPEG/Video Feeder Stride Register */
#define BCHP_VFD_5_DISP_HSIZE                    0x00601a14 /* MPEG/Video Feeder Horizontal Display Size Register */
#define BCHP_VFD_5_DISP_VSIZE                    0x00601a18 /* MPEG/Video Feeder Vertical Display Size Register */
#define BCHP_VFD_5_PICTURE0_LINE_ADDR_0          0x00601a1c /* MPEG/Video Feeder Line Address0 Register */
#define BCHP_VFD_5_US_422_TO_444_DERING          0x00601a20 /* MPEG/Video Feeder 4:2:2 to 4:4:4 Conversion Register */
#define BCHP_VFD_5_DATA_MODE                     0x00601a24 /* MPEG/Video Feeder Data Mode Register */
#define BCHP_VFD_5_PIC_OFFSET                    0x00601a28 /* MPEG/Video Feeder Picture Offset Register */
#define BCHP_VFD_5_BYTE_ORDER                    0x00601a2c /* 8bit YCbCr PACKED_NEW Format byte order control */
#define BCHP_VFD_5_PIC_FEED_CMD                  0x00601a30 /* MPEG/Video Feeder Picture Feed Command Register */
#define BCHP_VFD_5_FEED_STATUS                   0x00601a58 /* MPEG/Video Feeder Feed Status Register */
#define BCHP_VFD_5_PICTURE0_LAC_LINE_ADDR_0      0x00601a5c /* MPEG/Video Feeder Line Address Computer Line Address0 Register */
#define BCHP_VFD_5_LAC_LINE_FEED_CNTL            0x00601a68 /* MPEG/Video Feeder Line Address Computer Line Feed Control Register */
#define BCHP_VFD_5_FEEDER_TIMEOUT_REPEAT_PIC_CNTL 0x00601a6c /* MPEG/Video Feeder Timeout and Repeat Picture Control Register */
#define BCHP_VFD_5_BVB_RX_STALL_TIMEOUT_CNTL     0x00601a70 /* MPEG/Video Feeder BVB Receiver Stall Timeout Control Register */
#define BCHP_VFD_5_FEEDER_ERROR_INTERRUPT_STATUS 0x00601a74 /* MPEG/Video Feeder Error Interrupt Status Register */
#define BCHP_VFD_5_FEEDER_ERROR_INTERRUPT_STATUS_CLR 0x00601a78 /* MPEG/Video Feeder Error Interrupt Status Clear Register */
#define BCHP_VFD_5_FEEDER_BVB_STATUS             0x00601a7c /* MPEG/Video Feeder BVB Status Register */
#define BCHP_VFD_5_FEEDER_BVB_STATUS_CLR         0x00601a80 /* MPEG/Video Feeder BVB Status Clear Register */
#define BCHP_VFD_5_TEST_MODE_CNTL                0x00601a84 /* MPEG/Video Feeder Test Mode Control Register */
#define BCHP_VFD_5_BVB_SAMPLE_DATA               0x00601a88 /* MPEG/Video Feeder BVB Output Sample Data Register */
#define BCHP_VFD_5_TEST_PORT_CNTL                0x00601a8c /* MPEG/Video Feeder Test port Control Register */
#define BCHP_VFD_5_TEST_PORT_DATA                0x00601a90 /* MPEG/Video Feeder Test port Data Register */
#define BCHP_VFD_5_CFG_STATUS                    0x00601a94 /* MPEG/Video Feeder Hardware Configuration Register */
#define BCHP_VFD_5_PICTURE0_LINE_ADDR_0_R        0x00601b50 /* MPEG/Video Feeder Line Address0 Register */
#define BCHP_VFD_5_SCRATCH_REGISTER_1            0x00601bfc /* MPEG/Video Feeder Scratch 1 Register */
#define BCHP_VFD_5_SCRATCH_REGISTER_0            0x00601bf8 /* Video Feeder Scratch 0 Register */

#endif /* #ifndef BCHP_VFD_5_H__ */

/* End of File */