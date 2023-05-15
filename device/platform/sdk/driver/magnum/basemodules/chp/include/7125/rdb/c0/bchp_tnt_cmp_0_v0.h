/***************************************************************************
 *     Copyright (c) 1999-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bchp_tnt_cmp_0_v0.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 1/25/10 9:56p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Fri Jan 22 20:24:06 2010
 *                 MD5 Checksum         a2d1f2163f65e87d228a0fb491cb442d
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7125/rdb/c0/bchp_tnt_cmp_0_v0.h $
 * 
 * Hydra_Software_Devel/1   1/25/10 9:56p albertl
 * SW7125-177: Initial revision.
 *
 ***************************************************************************/

#ifndef BCHP_TNT_CMP_0_V0_H__
#define BCHP_TNT_CMP_0_V0_H__

/***************************************************************************
 *TNT_CMP_0_V0 - Transient Adjustment Block in Video Compositor 0/Video Intra Surface 0
 ***************************************************************************/
#define BCHP_TNT_CMP_0_V0_REVISION_ID            0x00843000 /* TNT Revision register */
#define BCHP_TNT_CMP_0_V0_TOP_CONTROL            0x00843004 /* TNT Top Control register */
#define BCHP_TNT_CMP_0_V0_LUMA_CONTROL           0x00843008 /* Luma Path Control register */
#define BCHP_TNT_CMP_0_V0_LUMA_PEAKING           0x0084300c /* Luma Peaking Control register */
#define BCHP_TNT_CMP_0_V0_CHROMA_CONTROL         0x00843010 /* Chroma Path Control register */
#define BCHP_TNT_CMP_0_V0_DEMO_SETTING           0x00843014 /* Visual Effects Demo Setting */
#define BCHP_TNT_CMP_0_V0_SCRATCH_0              0x00843018 /* TNT Scratch register 0 */
#define BCHP_TNT_CMP_0_V0_LC_ALIGN_CONTROL       0x0084301c /* Luma/Chroma alignment control register */
#define BCHP_TNT_CMP_0_V0_WIDE_LUMA_CONTROL      0x00843020 /* Wide Luma Control */
#define BCHP_TNT_CMP_0_V0_SIMPLE_LUMA_CORE_CTRL  0x00843024 /* Simple Luma Core Control */
#define BCHP_TNT_CMP_0_V0_DEBUG_SIGNALS          0x00843030 /* Debug signals */
#define BCHP_TNT_CMP_0_V0_LC_ALIGN_COUNTER_CENTER 0x00843034 /* Luma/Chroma alignment center counter */
#define BCHP_TNT_CMP_0_V0_LC_ALIGN_COUNTER_LEFT  0x00843038 /* Luma/Chroma alignment left counter */
#define BCHP_TNT_CMP_0_V0_LC_ALIGN_COUNTER_RIGHT 0x0084303c /* Luma/Chroma alignment right counter */

/***************************************************************************
 *REVISION_ID - TNT Revision register
 ***************************************************************************/
/* TNT_CMP_0_V0 :: REVISION_ID :: reserved0 [31:16] */
#define BCHP_TNT_CMP_0_V0_REVISION_ID_reserved0_MASK               0xffff0000
#define BCHP_TNT_CMP_0_V0_REVISION_ID_reserved0_SHIFT              16

/* TNT_CMP_0_V0 :: REVISION_ID :: MAJOR [15:08] */
#define BCHP_TNT_CMP_0_V0_REVISION_ID_MAJOR_MASK                   0x0000ff00
#define BCHP_TNT_CMP_0_V0_REVISION_ID_MAJOR_SHIFT                  8

/* TNT_CMP_0_V0 :: REVISION_ID :: MINOR [07:00] */
#define BCHP_TNT_CMP_0_V0_REVISION_ID_MINOR_MASK                   0x000000ff
#define BCHP_TNT_CMP_0_V0_REVISION_ID_MINOR_SHIFT                  0

/***************************************************************************
 *TOP_CONTROL - TNT Top Control register
 ***************************************************************************/
/* TNT_CMP_0_V0 :: TOP_CONTROL :: reserved0 [31:05] */
#define BCHP_TNT_CMP_0_V0_TOP_CONTROL_reserved0_MASK               0xffffffe0
#define BCHP_TNT_CMP_0_V0_TOP_CONTROL_reserved0_SHIFT              5

/* TNT_CMP_0_V0 :: TOP_CONTROL :: UPDATE_SEL [04:04] */
#define BCHP_TNT_CMP_0_V0_TOP_CONTROL_UPDATE_SEL_MASK              0x00000010
#define BCHP_TNT_CMP_0_V0_TOP_CONTROL_UPDATE_SEL_SHIFT             4
#define BCHP_TNT_CMP_0_V0_TOP_CONTROL_UPDATE_SEL_NORMAL            0
#define BCHP_TNT_CMP_0_V0_TOP_CONTROL_UPDATE_SEL_EOP               1

/* TNT_CMP_0_V0 :: TOP_CONTROL :: DEMO_MODE [03:03] */
#define BCHP_TNT_CMP_0_V0_TOP_CONTROL_DEMO_MODE_MASK               0x00000008
#define BCHP_TNT_CMP_0_V0_TOP_CONTROL_DEMO_MODE_SHIFT              3
#define BCHP_TNT_CMP_0_V0_TOP_CONTROL_DEMO_MODE_DISABLE            0
#define BCHP_TNT_CMP_0_V0_TOP_CONTROL_DEMO_MODE_ENABLE             1

/* TNT_CMP_0_V0 :: TOP_CONTROL :: reserved_for_eco1 [02:02] */
#define BCHP_TNT_CMP_0_V0_TOP_CONTROL_reserved_for_eco1_MASK       0x00000004
#define BCHP_TNT_CMP_0_V0_TOP_CONTROL_reserved_for_eco1_SHIFT      2

/* TNT_CMP_0_V0 :: TOP_CONTROL :: reserved2 [01:01] */
#define BCHP_TNT_CMP_0_V0_TOP_CONTROL_reserved2_MASK               0x00000002
#define BCHP_TNT_CMP_0_V0_TOP_CONTROL_reserved2_SHIFT              1

/* TNT_CMP_0_V0 :: TOP_CONTROL :: ENABLE [00:00] */
#define BCHP_TNT_CMP_0_V0_TOP_CONTROL_ENABLE_MASK                  0x00000001
#define BCHP_TNT_CMP_0_V0_TOP_CONTROL_ENABLE_SHIFT                 0
#define BCHP_TNT_CMP_0_V0_TOP_CONTROL_ENABLE_BYPASS                0
#define BCHP_TNT_CMP_0_V0_TOP_CONTROL_ENABLE_ENABLE                1

/***************************************************************************
 *LUMA_CONTROL - Luma Path Control register
 ***************************************************************************/
/* TNT_CMP_0_V0 :: LUMA_CONTROL :: reserved0 [31:24] */
#define BCHP_TNT_CMP_0_V0_LUMA_CONTROL_reserved0_MASK              0xff000000
#define BCHP_TNT_CMP_0_V0_LUMA_CONTROL_reserved0_SHIFT             24

/* TNT_CMP_0_V0 :: LUMA_CONTROL :: SCRATCH [23:20] */
#define BCHP_TNT_CMP_0_V0_LUMA_CONTROL_SCRATCH_MASK                0x00f00000
#define BCHP_TNT_CMP_0_V0_LUMA_CONTROL_SCRATCH_SHIFT               20

/* TNT_CMP_0_V0 :: LUMA_CONTROL :: CORE [19:12] */
#define BCHP_TNT_CMP_0_V0_LUMA_CONTROL_CORE_MASK                   0x000ff000
#define BCHP_TNT_CMP_0_V0_LUMA_CONTROL_CORE_SHIFT                  12

/* TNT_CMP_0_V0 :: LUMA_CONTROL :: reserved1 [11:10] */
#define BCHP_TNT_CMP_0_V0_LUMA_CONTROL_reserved1_MASK              0x00000c00
#define BCHP_TNT_CMP_0_V0_LUMA_CONTROL_reserved1_SHIFT             10

/* TNT_CMP_0_V0 :: LUMA_CONTROL :: GAIN [09:04] */
#define BCHP_TNT_CMP_0_V0_LUMA_CONTROL_GAIN_MASK                   0x000003f0
#define BCHP_TNT_CMP_0_V0_LUMA_CONTROL_GAIN_SHIFT                  4

/* TNT_CMP_0_V0 :: LUMA_CONTROL :: BLUR [03:02] */
#define BCHP_TNT_CMP_0_V0_LUMA_CONTROL_BLUR_MASK                   0x0000000c
#define BCHP_TNT_CMP_0_V0_LUMA_CONTROL_BLUR_SHIFT                  2

/* TNT_CMP_0_V0 :: LUMA_CONTROL :: SOFTEN [01:01] */
#define BCHP_TNT_CMP_0_V0_LUMA_CONTROL_SOFTEN_MASK                 0x00000002
#define BCHP_TNT_CMP_0_V0_LUMA_CONTROL_SOFTEN_SHIFT                1
#define BCHP_TNT_CMP_0_V0_LUMA_CONTROL_SOFTEN_DISABLE              0
#define BCHP_TNT_CMP_0_V0_LUMA_CONTROL_SOFTEN_ENABLE               1

/* TNT_CMP_0_V0 :: LUMA_CONTROL :: H_ONLY [00:00] */
#define BCHP_TNT_CMP_0_V0_LUMA_CONTROL_H_ONLY_MASK                 0x00000001
#define BCHP_TNT_CMP_0_V0_LUMA_CONTROL_H_ONLY_SHIFT                0
#define BCHP_TNT_CMP_0_V0_LUMA_CONTROL_H_ONLY_DISABLE              0
#define BCHP_TNT_CMP_0_V0_LUMA_CONTROL_H_ONLY_ENABLE               1

/***************************************************************************
 *LUMA_PEAKING - Luma Peaking Control register
 ***************************************************************************/
/* TNT_CMP_0_V0 :: LUMA_PEAKING :: reserved0 [31:30] */
#define BCHP_TNT_CMP_0_V0_LUMA_PEAKING_reserved0_MASK              0xc0000000
#define BCHP_TNT_CMP_0_V0_LUMA_PEAKING_reserved0_SHIFT             30

/* TNT_CMP_0_V0 :: LUMA_PEAKING :: H_AVOID [29:24] */
#define BCHP_TNT_CMP_0_V0_LUMA_PEAKING_H_AVOID_MASK                0x3f000000
#define BCHP_TNT_CMP_0_V0_LUMA_PEAKING_H_AVOID_SHIFT               24

/* TNT_CMP_0_V0 :: LUMA_PEAKING :: reserved1 [23:22] */
#define BCHP_TNT_CMP_0_V0_LUMA_PEAKING_reserved1_MASK              0x00c00000
#define BCHP_TNT_CMP_0_V0_LUMA_PEAKING_reserved1_SHIFT             22

/* TNT_CMP_0_V0 :: LUMA_PEAKING :: V_AVOID [21:16] */
#define BCHP_TNT_CMP_0_V0_LUMA_PEAKING_V_AVOID_MASK                0x003f0000
#define BCHP_TNT_CMP_0_V0_LUMA_PEAKING_V_AVOID_SHIFT               16

/* TNT_CMP_0_V0 :: LUMA_PEAKING :: reserved2 [15:15] */
#define BCHP_TNT_CMP_0_V0_LUMA_PEAKING_reserved2_MASK              0x00008000
#define BCHP_TNT_CMP_0_V0_LUMA_PEAKING_reserved2_SHIFT             15

/* TNT_CMP_0_V0 :: LUMA_PEAKING :: PEAK_LIMIT [14:08] */
#define BCHP_TNT_CMP_0_V0_LUMA_PEAKING_PEAK_LIMIT_MASK             0x00007f00
#define BCHP_TNT_CMP_0_V0_LUMA_PEAKING_PEAK_LIMIT_SHIFT            8

/* TNT_CMP_0_V0 :: LUMA_PEAKING :: reserved3 [07:05] */
#define BCHP_TNT_CMP_0_V0_LUMA_PEAKING_reserved3_MASK              0x000000e0
#define BCHP_TNT_CMP_0_V0_LUMA_PEAKING_reserved3_SHIFT             5

/* TNT_CMP_0_V0 :: LUMA_PEAKING :: PEAK_VALUE [04:00] */
#define BCHP_TNT_CMP_0_V0_LUMA_PEAKING_PEAK_VALUE_MASK             0x0000001f
#define BCHP_TNT_CMP_0_V0_LUMA_PEAKING_PEAK_VALUE_SHIFT            0

/***************************************************************************
 *CHROMA_CONTROL - Chroma Path Control register
 ***************************************************************************/
/* TNT_CMP_0_V0 :: CHROMA_CONTROL :: reserved0 [31:28] */
#define BCHP_TNT_CMP_0_V0_CHROMA_CONTROL_reserved0_MASK            0xf0000000
#define BCHP_TNT_CMP_0_V0_CHROMA_CONTROL_reserved0_SHIFT           28

/* TNT_CMP_0_V0 :: CHROMA_CONTROL :: SCRATCH [27:24] */
#define BCHP_TNT_CMP_0_V0_CHROMA_CONTROL_SCRATCH_MASK              0x0f000000
#define BCHP_TNT_CMP_0_V0_CHROMA_CONTROL_SCRATCH_SHIFT             24

/* TNT_CMP_0_V0 :: CHROMA_CONTROL :: CORE [23:16] */
#define BCHP_TNT_CMP_0_V0_CHROMA_CONTROL_CORE_MASK                 0x00ff0000
#define BCHP_TNT_CMP_0_V0_CHROMA_CONTROL_CORE_SHIFT                16

/* TNT_CMP_0_V0 :: CHROMA_CONTROL :: WIDE_CHROMA [15:15] */
#define BCHP_TNT_CMP_0_V0_CHROMA_CONTROL_WIDE_CHROMA_MASK          0x00008000
#define BCHP_TNT_CMP_0_V0_CHROMA_CONTROL_WIDE_CHROMA_SHIFT         15
#define BCHP_TNT_CMP_0_V0_CHROMA_CONTROL_WIDE_CHROMA_DISABLE       0
#define BCHP_TNT_CMP_0_V0_CHROMA_CONTROL_WIDE_CHROMA_ENABLE        1

/* TNT_CMP_0_V0 :: CHROMA_CONTROL :: FALSE_COLOR [14:12] */
#define BCHP_TNT_CMP_0_V0_CHROMA_CONTROL_FALSE_COLOR_MASK          0x00007000
#define BCHP_TNT_CMP_0_V0_CHROMA_CONTROL_FALSE_COLOR_SHIFT         12

/* TNT_CMP_0_V0 :: CHROMA_CONTROL :: reserved1 [11:10] */
#define BCHP_TNT_CMP_0_V0_CHROMA_CONTROL_reserved1_MASK            0x00000c00
#define BCHP_TNT_CMP_0_V0_CHROMA_CONTROL_reserved1_SHIFT           10

/* TNT_CMP_0_V0 :: CHROMA_CONTROL :: GAIN [09:04] */
#define BCHP_TNT_CMP_0_V0_CHROMA_CONTROL_GAIN_MASK                 0x000003f0
#define BCHP_TNT_CMP_0_V0_CHROMA_CONTROL_GAIN_SHIFT                4

/* TNT_CMP_0_V0 :: CHROMA_CONTROL :: reserved2 [03:01] */
#define BCHP_TNT_CMP_0_V0_CHROMA_CONTROL_reserved2_MASK            0x0000000e
#define BCHP_TNT_CMP_0_V0_CHROMA_CONTROL_reserved2_SHIFT           1

/* TNT_CMP_0_V0 :: CHROMA_CONTROL :: H_ONLY [00:00] */
#define BCHP_TNT_CMP_0_V0_CHROMA_CONTROL_H_ONLY_MASK               0x00000001
#define BCHP_TNT_CMP_0_V0_CHROMA_CONTROL_H_ONLY_SHIFT              0
#define BCHP_TNT_CMP_0_V0_CHROMA_CONTROL_H_ONLY_DISABLE            0
#define BCHP_TNT_CMP_0_V0_CHROMA_CONTROL_H_ONLY_ENABLE             1

/***************************************************************************
 *DEMO_SETTING - Visual Effects Demo Setting
 ***************************************************************************/
/* TNT_CMP_0_V0 :: DEMO_SETTING :: reserved0 [31:17] */
#define BCHP_TNT_CMP_0_V0_DEMO_SETTING_reserved0_MASK              0xfffe0000
#define BCHP_TNT_CMP_0_V0_DEMO_SETTING_reserved0_SHIFT             17

/* TNT_CMP_0_V0 :: DEMO_SETTING :: DEMO_L_R [16:16] */
#define BCHP_TNT_CMP_0_V0_DEMO_SETTING_DEMO_L_R_MASK               0x00010000
#define BCHP_TNT_CMP_0_V0_DEMO_SETTING_DEMO_L_R_SHIFT              16
#define BCHP_TNT_CMP_0_V0_DEMO_SETTING_DEMO_L_R_LEFT               1
#define BCHP_TNT_CMP_0_V0_DEMO_SETTING_DEMO_L_R_RIGHT              0

/* TNT_CMP_0_V0 :: DEMO_SETTING :: reserved1 [15:11] */
#define BCHP_TNT_CMP_0_V0_DEMO_SETTING_reserved1_MASK              0x0000f800
#define BCHP_TNT_CMP_0_V0_DEMO_SETTING_reserved1_SHIFT             11

/* TNT_CMP_0_V0 :: DEMO_SETTING :: DEMO_BOUNDARY [10:00] */
#define BCHP_TNT_CMP_0_V0_DEMO_SETTING_DEMO_BOUNDARY_MASK          0x000007ff
#define BCHP_TNT_CMP_0_V0_DEMO_SETTING_DEMO_BOUNDARY_SHIFT         0

/***************************************************************************
 *SCRATCH_0 - TNT Scratch register 0
 ***************************************************************************/
/* TNT_CMP_0_V0 :: SCRATCH_0 :: VALUE [31:00] */
#define BCHP_TNT_CMP_0_V0_SCRATCH_0_VALUE_MASK                     0xffffffff
#define BCHP_TNT_CMP_0_V0_SCRATCH_0_VALUE_SHIFT                    0

/***************************************************************************
 *LC_ALIGN_CONTROL - Luma/Chroma alignment control register
 ***************************************************************************/
/* TNT_CMP_0_V0 :: LC_ALIGN_CONTROL :: reserved0 [31:02] */
#define BCHP_TNT_CMP_0_V0_LC_ALIGN_CONTROL_reserved0_MASK          0xfffffffc
#define BCHP_TNT_CMP_0_V0_LC_ALIGN_CONTROL_reserved0_SHIFT         2

/* TNT_CMP_0_V0 :: LC_ALIGN_CONTROL :: ALIGN_CR_CB [01:01] */
#define BCHP_TNT_CMP_0_V0_LC_ALIGN_CONTROL_ALIGN_CR_CB_MASK        0x00000002
#define BCHP_TNT_CMP_0_V0_LC_ALIGN_CONTROL_ALIGN_CR_CB_SHIFT       1
#define BCHP_TNT_CMP_0_V0_LC_ALIGN_CONTROL_ALIGN_CR_CB_SEL_CB      0
#define BCHP_TNT_CMP_0_V0_LC_ALIGN_CONTROL_ALIGN_CR_CB_SEL_CR      1

/* TNT_CMP_0_V0 :: LC_ALIGN_CONTROL :: RESET_COUNTER [00:00] */
#define BCHP_TNT_CMP_0_V0_LC_ALIGN_CONTROL_RESET_COUNTER_MASK      0x00000001
#define BCHP_TNT_CMP_0_V0_LC_ALIGN_CONTROL_RESET_COUNTER_SHIFT     0

/***************************************************************************
 *WIDE_LUMA_CONTROL - Wide Luma Control
 ***************************************************************************/
/* TNT_CMP_0_V0 :: WIDE_LUMA_CONTROL :: reserved0 [31:24] */
#define BCHP_TNT_CMP_0_V0_WIDE_LUMA_CONTROL_reserved0_MASK         0xff000000
#define BCHP_TNT_CMP_0_V0_WIDE_LUMA_CONTROL_reserved0_SHIFT        24

/* TNT_CMP_0_V0 :: WIDE_LUMA_CONTROL :: SCRATCH [23:20] */
#define BCHP_TNT_CMP_0_V0_WIDE_LUMA_CONTROL_SCRATCH_MASK           0x00f00000
#define BCHP_TNT_CMP_0_V0_WIDE_LUMA_CONTROL_SCRATCH_SHIFT          20

/* TNT_CMP_0_V0 :: WIDE_LUMA_CONTROL :: CORE [19:12] */
#define BCHP_TNT_CMP_0_V0_WIDE_LUMA_CONTROL_CORE_MASK              0x000ff000
#define BCHP_TNT_CMP_0_V0_WIDE_LUMA_CONTROL_CORE_SHIFT             12

/* TNT_CMP_0_V0 :: WIDE_LUMA_CONTROL :: reserved1 [11:02] */
#define BCHP_TNT_CMP_0_V0_WIDE_LUMA_CONTROL_reserved1_MASK         0x00000ffc
#define BCHP_TNT_CMP_0_V0_WIDE_LUMA_CONTROL_reserved1_SHIFT        2

/* TNT_CMP_0_V0 :: WIDE_LUMA_CONTROL :: MODE [01:00] */
#define BCHP_TNT_CMP_0_V0_WIDE_LUMA_CONTROL_MODE_MASK              0x00000003
#define BCHP_TNT_CMP_0_V0_WIDE_LUMA_CONTROL_MODE_SHIFT             0

/***************************************************************************
 *SIMPLE_LUMA_CORE_CTRL - Simple Luma Core Control
 ***************************************************************************/
/* TNT_CMP_0_V0 :: SIMPLE_LUMA_CORE_CTRL :: reserved0 [31:24] */
#define BCHP_TNT_CMP_0_V0_SIMPLE_LUMA_CORE_CTRL_reserved0_MASK     0xff000000
#define BCHP_TNT_CMP_0_V0_SIMPLE_LUMA_CORE_CTRL_reserved0_SHIFT    24

/* TNT_CMP_0_V0 :: SIMPLE_LUMA_CORE_CTRL :: SCRATCH [23:20] */
#define BCHP_TNT_CMP_0_V0_SIMPLE_LUMA_CORE_CTRL_SCRATCH_MASK       0x00f00000
#define BCHP_TNT_CMP_0_V0_SIMPLE_LUMA_CORE_CTRL_SCRATCH_SHIFT      20

/* TNT_CMP_0_V0 :: SIMPLE_LUMA_CORE_CTRL :: CORE [19:12] */
#define BCHP_TNT_CMP_0_V0_SIMPLE_LUMA_CORE_CTRL_CORE_MASK          0x000ff000
#define BCHP_TNT_CMP_0_V0_SIMPLE_LUMA_CORE_CTRL_CORE_SHIFT         12

/* TNT_CMP_0_V0 :: SIMPLE_LUMA_CORE_CTRL :: reserved1 [11:01] */
#define BCHP_TNT_CMP_0_V0_SIMPLE_LUMA_CORE_CTRL_reserved1_MASK     0x00000ffe
#define BCHP_TNT_CMP_0_V0_SIMPLE_LUMA_CORE_CTRL_reserved1_SHIFT    1

/* TNT_CMP_0_V0 :: SIMPLE_LUMA_CORE_CTRL :: MODE [00:00] */
#define BCHP_TNT_CMP_0_V0_SIMPLE_LUMA_CORE_CTRL_MODE_MASK          0x00000001
#define BCHP_TNT_CMP_0_V0_SIMPLE_LUMA_CORE_CTRL_MODE_SHIFT         0
#define BCHP_TNT_CMP_0_V0_SIMPLE_LUMA_CORE_CTRL_MODE_DISABLE       0
#define BCHP_TNT_CMP_0_V0_SIMPLE_LUMA_CORE_CTRL_MODE_ENABLE        1

/***************************************************************************
 *DEBUG_SIGNALS - Debug signals
 ***************************************************************************/
/* TNT_CMP_0_V0 :: DEBUG_SIGNALS :: VALUE [31:00] */
#define BCHP_TNT_CMP_0_V0_DEBUG_SIGNALS_VALUE_MASK                 0xffffffff
#define BCHP_TNT_CMP_0_V0_DEBUG_SIGNALS_VALUE_SHIFT                0

/***************************************************************************
 *LC_ALIGN_COUNTER_CENTER - Luma/Chroma alignment center counter
 ***************************************************************************/
/* TNT_CMP_0_V0 :: LC_ALIGN_COUNTER_CENTER :: VALUE [31:00] */
#define BCHP_TNT_CMP_0_V0_LC_ALIGN_COUNTER_CENTER_VALUE_MASK       0xffffffff
#define BCHP_TNT_CMP_0_V0_LC_ALIGN_COUNTER_CENTER_VALUE_SHIFT      0

/***************************************************************************
 *LC_ALIGN_COUNTER_LEFT - Luma/Chroma alignment left counter
 ***************************************************************************/
/* TNT_CMP_0_V0 :: LC_ALIGN_COUNTER_LEFT :: VALUE [31:00] */
#define BCHP_TNT_CMP_0_V0_LC_ALIGN_COUNTER_LEFT_VALUE_MASK         0xffffffff
#define BCHP_TNT_CMP_0_V0_LC_ALIGN_COUNTER_LEFT_VALUE_SHIFT        0

/***************************************************************************
 *LC_ALIGN_COUNTER_RIGHT - Luma/Chroma alignment right counter
 ***************************************************************************/
/* TNT_CMP_0_V0 :: LC_ALIGN_COUNTER_RIGHT :: VALUE [31:00] */
#define BCHP_TNT_CMP_0_V0_LC_ALIGN_COUNTER_RIGHT_VALUE_MASK        0xffffffff
#define BCHP_TNT_CMP_0_V0_LC_ALIGN_COUNTER_RIGHT_VALUE_SHIFT       0

#endif /* #ifndef BCHP_TNT_CMP_0_V0_H__ */

/* End of File */