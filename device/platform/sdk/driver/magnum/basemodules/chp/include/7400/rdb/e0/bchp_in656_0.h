/***************************************************************************
 *     Copyright (c) 1999-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bchp_in656_0.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 1/9/08 10:50a $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Wed Jan  9 08:59:50 2008
 *                 MD5 Checksum         847dc12a9d71c4c68a648bbf19a883e3
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7400/rdb/e0/bchp_in656_0.h $
 * 
 * Hydra_Software_Devel/1   1/9/08 10:50a rpan
 * PR38572: Initial revision.
 *
 ***************************************************************************/

#ifndef BCHP_IN656_0_H__
#define BCHP_IN656_0_H__

/***************************************************************************
 *IN656_0
 ***************************************************************************/
#define BCHP_IN656_0_REV_ID                      0x001c3200 /* Revision ID register */
#define BCHP_IN656_0_RESET                       0x001c3204 /* 656 Input Decoder Software Reset register */
#define BCHP_IN656_0_ERROR_STATUS                0x001c3208 /* 656 Input Decoder Error Status register */
#define BCHP_IN656_0_STRM_CNTRL                  0x001c320c /* 656 Input Stream Splitter Control  register */
#define BCHP_IN656_0_STRM_WIN                    0x001c3210 /* 656 Input Stream Splitter Counters register */
#define BCHP_IN656_0_FLD_0_PTR                   0x001c3214 /* 656 Input Ancillary Data DRAM Field 0 (top) Pointer */
#define BCHP_IN656_0_FLD_1_PTR                   0x001c3218 /* 656 Input Ancillary Data DRAM Field 1 (bottom) Pointer */
#define BCHP_IN656_0_WRITE_COMPLETE              0x001c3220 /* 656 Input Ancillary Write Complete */
#define BCHP_IN656_0_BUFFER                      0x001c3224 /* 656 Max Buffer Size */
#define BCHP_IN656_0_SCB_NMB                     0x001c3228 /* 656 Input NMB Value */

/***************************************************************************
 *REV_ID - Revision ID register
 ***************************************************************************/
/* IN656_0 :: REV_ID :: reserved0 [31:16] */
#define BCHP_IN656_0_REV_ID_reserved0_MASK                         0xffff0000
#define BCHP_IN656_0_REV_ID_reserved0_SHIFT                        16

/* IN656_0 :: REV_ID :: ID [15:00] */
#define BCHP_IN656_0_REV_ID_ID_MASK                                0x0000ffff
#define BCHP_IN656_0_REV_ID_ID_SHIFT                               0

/***************************************************************************
 *RESET - 656 Input Decoder Software Reset register
 ***************************************************************************/
/* IN656_0 :: RESET :: RESET [31:00] */
#define BCHP_IN656_0_RESET_RESET_MASK                              0xffffffff
#define BCHP_IN656_0_RESET_RESET_SHIFT                             0

/***************************************************************************
 *ERROR_STATUS - 656 Input Decoder Error Status register
 ***************************************************************************/
/* IN656_0 :: ERROR_STATUS :: reserved0 [31:28] */
#define BCHP_IN656_0_ERROR_STATUS_reserved0_MASK                   0xf0000000
#define BCHP_IN656_0_ERROR_STATUS_reserved0_SHIFT                  28

/* IN656_0 :: ERROR_STATUS :: F1_PARITY_ERROR [27:27] */
#define BCHP_IN656_0_ERROR_STATUS_F1_PARITY_ERROR_MASK             0x08000000
#define BCHP_IN656_0_ERROR_STATUS_F1_PARITY_ERROR_SHIFT            27

/* IN656_0 :: ERROR_STATUS :: F0_PARITY_ERROR [26:26] */
#define BCHP_IN656_0_ERROR_STATUS_F0_PARITY_ERROR_MASK             0x04000000
#define BCHP_IN656_0_ERROR_STATUS_F0_PARITY_ERROR_SHIFT            26

/* IN656_0 :: ERROR_STATUS :: VFRAME_CNT [25:03] */
#define BCHP_IN656_0_ERROR_STATUS_VFRAME_CNT_MASK                  0x03fffff8
#define BCHP_IN656_0_ERROR_STATUS_VFRAME_CNT_SHIFT                 3

/* IN656_0 :: ERROR_STATUS :: FIFO_FULL [02:02] */
#define BCHP_IN656_0_ERROR_STATUS_FIFO_FULL_MASK                   0x00000004
#define BCHP_IN656_0_ERROR_STATUS_FIFO_FULL_SHIFT                  2

/* IN656_0 :: ERROR_STATUS :: BAD_AP_PREFIX [01:01] */
#define BCHP_IN656_0_ERROR_STATUS_BAD_AP_PREFIX_MASK               0x00000002
#define BCHP_IN656_0_ERROR_STATUS_BAD_AP_PREFIX_SHIFT              1

/* IN656_0 :: ERROR_STATUS :: BAD_SAV [00:00] */
#define BCHP_IN656_0_ERROR_STATUS_BAD_SAV_MASK                     0x00000001
#define BCHP_IN656_0_ERROR_STATUS_BAD_SAV_SHIFT                    0

/***************************************************************************
 *STRM_CNTRL - 656 Input Stream Splitter Control  register
 ***************************************************************************/
/* IN656_0 :: STRM_CNTRL :: reserved0 [31:21] */
#define BCHP_IN656_0_STRM_CNTRL_reserved0_MASK                     0xffe00000
#define BCHP_IN656_0_STRM_CNTRL_reserved0_SHIFT                    21

/* IN656_0 :: STRM_CNTRL :: MODIFY_FF_AND_00 [20:20] */
#define BCHP_IN656_0_STRM_CNTRL_MODIFY_FF_AND_00_MASK              0x00100000
#define BCHP_IN656_0_STRM_CNTRL_MODIFY_FF_AND_00_SHIFT             20
#define BCHP_IN656_0_STRM_CNTRL_MODIFY_FF_AND_00_DISABLE           0
#define BCHP_IN656_0_STRM_CNTRL_MODIFY_FF_AND_00_ENABLE            1

/* IN656_0 :: STRM_CNTRL :: ENCODE_LINE_NUMBER [19:19] */
#define BCHP_IN656_0_STRM_CNTRL_ENCODE_LINE_NUMBER_MASK            0x00080000
#define BCHP_IN656_0_STRM_CNTRL_ENCODE_LINE_NUMBER_SHIFT           19
#define BCHP_IN656_0_STRM_CNTRL_ENCODE_LINE_NUMBER_DISABLE         0
#define BCHP_IN656_0_STRM_CNTRL_ENCODE_LINE_NUMBER_ENABLE          1

/* IN656_0 :: STRM_CNTRL :: ANCIL_DATA_WIDTH [18:18] */
#define BCHP_IN656_0_STRM_CNTRL_ANCIL_DATA_WIDTH_MASK              0x00040000
#define BCHP_IN656_0_STRM_CNTRL_ANCIL_DATA_WIDTH_SHIFT             18
#define BCHP_IN656_0_STRM_CNTRL_ANCIL_DATA_WIDTH_BIT8              0
#define BCHP_IN656_0_STRM_CNTRL_ANCIL_DATA_WIDTH_BIT10             1

/* IN656_0 :: STRM_CNTRL :: SMPTE_COUNT_TYPE [17:17] */
#define BCHP_IN656_0_STRM_CNTRL_SMPTE_COUNT_TYPE_MASK              0x00020000
#define BCHP_IN656_0_STRM_CNTRL_SMPTE_COUNT_TYPE_SHIFT             17
#define BCHP_IN656_0_STRM_CNTRL_SMPTE_COUNT_TYPE_BYTE              0
#define BCHP_IN656_0_STRM_CNTRL_SMPTE_COUNT_TYPE_DWORD             1

/* IN656_0 :: STRM_CNTRL :: TEST [16:16] */
#define BCHP_IN656_0_STRM_CNTRL_TEST_MASK                          0x00010000
#define BCHP_IN656_0_STRM_CNTRL_TEST_SHIFT                         16
#define BCHP_IN656_0_STRM_CNTRL_TEST_BUS_0                         0
#define BCHP_IN656_0_STRM_CNTRL_TEST_BUS_1                         1

/* IN656_0 :: STRM_CNTRL :: MODE_FP [15:15] */
#define BCHP_IN656_0_STRM_CNTRL_MODE_FP_MASK                       0x00008000
#define BCHP_IN656_0_STRM_CNTRL_MODE_FP_SHIFT                      15
#define BCHP_IN656_0_STRM_CNTRL_MODE_FP_AUTO                       0
#define BCHP_IN656_0_STRM_CNTRL_MODE_FP_RUL                        1

/* IN656_0 :: STRM_CNTRL :: VCNT_LAST [14:08] */
#define BCHP_IN656_0_STRM_CNTRL_VCNT_LAST_MASK                     0x00007f00
#define BCHP_IN656_0_STRM_CNTRL_VCNT_LAST_SHIFT                    8

/* IN656_0 :: STRM_CNTRL :: VBLANK [07:07] */
#define BCHP_IN656_0_STRM_CNTRL_VBLANK_MASK                        0x00000080
#define BCHP_IN656_0_STRM_CNTRL_VBLANK_SHIFT                       7
#define BCHP_IN656_0_STRM_CNTRL_VBLANK_COUNTER                     0
#define BCHP_IN656_0_STRM_CNTRL_VBLANK_VBIT                        1

/* IN656_0 :: STRM_CNTRL :: TYPE [06:04] */
#define BCHP_IN656_0_STRM_CNTRL_TYPE_MASK                          0x00000070
#define BCHP_IN656_0_STRM_CNTRL_TYPE_SHIFT                         4
#define BCHP_IN656_0_STRM_CNTRL_TYPE_SAA7113                       0
#define BCHP_IN656_0_STRM_CNTRL_TYPE_SAA7114A                      1
#define BCHP_IN656_0_STRM_CNTRL_TYPE_SAA7114B                      2
#define BCHP_IN656_0_STRM_CNTRL_TYPE_SAA7114C                      3
#define BCHP_IN656_0_STRM_CNTRL_TYPE_SAA7115                       4
#define BCHP_IN656_0_STRM_CNTRL_TYPE_SMPTE291                      5

/* IN656_0 :: STRM_CNTRL :: reserved1 [03:03] */
#define BCHP_IN656_0_STRM_CNTRL_reserved1_MASK                     0x00000008
#define BCHP_IN656_0_STRM_CNTRL_reserved1_SHIFT                    3

/* IN656_0 :: STRM_CNTRL :: DATA_MSB_POLARITY [02:02] */
#define BCHP_IN656_0_STRM_CNTRL_DATA_MSB_POLARITY_MASK             0x00000004
#define BCHP_IN656_0_STRM_CNTRL_DATA_MSB_POLARITY_SHIFT            2
#define BCHP_IN656_0_STRM_CNTRL_DATA_MSB_POLARITY_ZERO             0
#define BCHP_IN656_0_STRM_CNTRL_DATA_MSB_POLARITY_ONE              1

/* IN656_0 :: STRM_CNTRL :: ANCILLARY_WIN [01:01] */
#define BCHP_IN656_0_STRM_CNTRL_ANCILLARY_WIN_MASK                 0x00000002
#define BCHP_IN656_0_STRM_CNTRL_ANCILLARY_WIN_SHIFT                1
#define BCHP_IN656_0_STRM_CNTRL_ANCILLARY_WIN_DISABLE              0
#define BCHP_IN656_0_STRM_CNTRL_ANCILLARY_WIN_ENABLE               1

/* IN656_0 :: STRM_CNTRL :: ANCILLARY_PKT [00:00] */
#define BCHP_IN656_0_STRM_CNTRL_ANCILLARY_PKT_MASK                 0x00000001
#define BCHP_IN656_0_STRM_CNTRL_ANCILLARY_PKT_SHIFT                0
#define BCHP_IN656_0_STRM_CNTRL_ANCILLARY_PKT_ABSENT               0
#define BCHP_IN656_0_STRM_CNTRL_ANCILLARY_PKT_PRESENT              1

/***************************************************************************
 *STRM_WIN - 656 Input Stream Splitter Counters register
 ***************************************************************************/
/* IN656_0 :: STRM_WIN :: reserved0 [31:18] */
#define BCHP_IN656_0_STRM_WIN_reserved0_MASK                       0xfffc0000
#define BCHP_IN656_0_STRM_WIN_reserved0_SHIFT                      18

/* IN656_0 :: STRM_WIN :: END [17:09] */
#define BCHP_IN656_0_STRM_WIN_END_MASK                             0x0003fe00
#define BCHP_IN656_0_STRM_WIN_END_SHIFT                            9

/* IN656_0 :: STRM_WIN :: START [08:00] */
#define BCHP_IN656_0_STRM_WIN_START_MASK                           0x000001ff
#define BCHP_IN656_0_STRM_WIN_START_SHIFT                          0

/***************************************************************************
 *FLD_0_PTR - 656 Input Ancillary Data DRAM Field 0 (top) Pointer
 ***************************************************************************/
/* IN656_0 :: FLD_0_PTR :: FIELD_POINTER [31:00] */
#define BCHP_IN656_0_FLD_0_PTR_FIELD_POINTER_MASK                  0xffffffff
#define BCHP_IN656_0_FLD_0_PTR_FIELD_POINTER_SHIFT                 0

/***************************************************************************
 *FLD_1_PTR - 656 Input Ancillary Data DRAM Field 1 (bottom) Pointer
 ***************************************************************************/
/* IN656_0 :: FLD_1_PTR :: FIELD_POINTER [31:00] */
#define BCHP_IN656_0_FLD_1_PTR_FIELD_POINTER_MASK                  0xffffffff
#define BCHP_IN656_0_FLD_1_PTR_FIELD_POINTER_SHIFT                 0

/***************************************************************************
 *WRITE_COMPLETE - 656 Input Ancillary Write Complete
 ***************************************************************************/
/* IN656_0 :: WRITE_COMPLETE :: F1_BUFFER_LENGTH [31:18] */
#define BCHP_IN656_0_WRITE_COMPLETE_F1_BUFFER_LENGTH_MASK          0xfffc0000
#define BCHP_IN656_0_WRITE_COMPLETE_F1_BUFFER_LENGTH_SHIFT         18

/* IN656_0 :: WRITE_COMPLETE :: reserved0 [17:16] */
#define BCHP_IN656_0_WRITE_COMPLETE_reserved0_MASK                 0x00030000
#define BCHP_IN656_0_WRITE_COMPLETE_reserved0_SHIFT                16

/* IN656_0 :: WRITE_COMPLETE :: F0_BUFFER_LENGTH [15:02] */
#define BCHP_IN656_0_WRITE_COMPLETE_F0_BUFFER_LENGTH_MASK          0x0000fffc
#define BCHP_IN656_0_WRITE_COMPLETE_F0_BUFFER_LENGTH_SHIFT         2

/* IN656_0 :: WRITE_COMPLETE :: F1_COMPLETE [01:01] */
#define BCHP_IN656_0_WRITE_COMPLETE_F1_COMPLETE_MASK               0x00000002
#define BCHP_IN656_0_WRITE_COMPLETE_F1_COMPLETE_SHIFT              1

/* IN656_0 :: WRITE_COMPLETE :: F0_COMPLETE [00:00] */
#define BCHP_IN656_0_WRITE_COMPLETE_F0_COMPLETE_MASK               0x00000001
#define BCHP_IN656_0_WRITE_COMPLETE_F0_COMPLETE_SHIFT              0

/***************************************************************************
 *BUFFER - 656 Max Buffer Size
 ***************************************************************************/
/* IN656_0 :: BUFFER :: MAX_BUFFER_ENABLE [31:31] */
#define BCHP_IN656_0_BUFFER_MAX_BUFFER_ENABLE_MASK                 0x80000000
#define BCHP_IN656_0_BUFFER_MAX_BUFFER_ENABLE_SHIFT                31
#define BCHP_IN656_0_BUFFER_MAX_BUFFER_ENABLE_DISABLE              0
#define BCHP_IN656_0_BUFFER_MAX_BUFFER_ENABLE_ENABLE               1

/* IN656_0 :: BUFFER :: reserved0 [30:30] */
#define BCHP_IN656_0_BUFFER_reserved0_MASK                         0x40000000
#define BCHP_IN656_0_BUFFER_reserved0_SHIFT                        30

/* IN656_0 :: BUFFER :: F1_MAX_SIZE [29:16] */
#define BCHP_IN656_0_BUFFER_F1_MAX_SIZE_MASK                       0x3fff0000
#define BCHP_IN656_0_BUFFER_F1_MAX_SIZE_SHIFT                      16

/* IN656_0 :: BUFFER :: reserved1 [15:14] */
#define BCHP_IN656_0_BUFFER_reserved1_MASK                         0x0000c000
#define BCHP_IN656_0_BUFFER_reserved1_SHIFT                        14

/* IN656_0 :: BUFFER :: F0_MAX_SIZE [13:00] */
#define BCHP_IN656_0_BUFFER_F0_MAX_SIZE_MASK                       0x00003fff
#define BCHP_IN656_0_BUFFER_F0_MAX_SIZE_SHIFT                      0

/***************************************************************************
 *SCB_NMB - 656 Input NMB Value
 ***************************************************************************/
/* IN656_0 :: SCB_NMB :: reserved0 [31:10] */
#define BCHP_IN656_0_SCB_NMB_reserved0_MASK                        0xfffffc00
#define BCHP_IN656_0_SCB_NMB_reserved0_SHIFT                       10

/* IN656_0 :: SCB_NMB :: nmb [09:00] */
#define BCHP_IN656_0_SCB_NMB_nmb_MASK                              0x000003ff
#define BCHP_IN656_0_SCB_NMB_nmb_SHIFT                             0

#endif /* #ifndef BCHP_IN656_0_H__ */

/* End of File */