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
 * $brcm_Workfile: bchp_hd_anr_and_0.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 12/7/11 2:26p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Tue Dec  6 18:45:28 2011
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
 * $brcm_Log: /magnum/basemodules/chp/7435/rdb/a0/bchp_hd_anr_and_0.h $
 * 
 * Hydra_Software_Devel/2   12/7/11 2:26p mward
 * SW7435-3: Synced up with central rdb.
 *
 ***************************************************************************/

#ifndef BCHP_HD_ANR_AND_0_H__
#define BCHP_HD_ANR_AND_0_H__

/***************************************************************************
 *HD_ANR_AND_0 - MCVP AND Registers 0
 ***************************************************************************/
#define BCHP_HD_ANR_AND_0_REVISION_ID            0x00661800 /* AND Revision register */
#define BCHP_HD_ANR_AND_0_SW_RESET               0x00661804 /* AND Software Reset */
#define BCHP_HD_ANR_AND_0_AND_ENABLE             0x00661808 /* AND Enable */
#define BCHP_HD_ANR_AND_0_AND_MODE               0x0066180c /* AND Window Size */
#define BCHP_HD_ANR_AND_0_NOISE_LOWER_TH_0       0x00661810 /* Noise Range Lower Threshold 0 */
#define BCHP_HD_ANR_AND_0_NOISE_LOWER_TH_1       0x00661814 /* Noise Range Lower Threshold 1 */
#define BCHP_HD_ANR_AND_0_NOISE_LOWER_TH_2       0x00661818 /* Noise Range Lower Threshold 2 */
#define BCHP_HD_ANR_AND_0_NOISE_LOWER_TH_3       0x0066181c /* Noise Range Lower Threshold 3 */
#define BCHP_HD_ANR_AND_0_NOISE_LOWER_TH_4       0x00661820 /* Noise Range Lower Threshold 4 */
#define BCHP_HD_ANR_AND_0_NOISE_UPPER_TH_0       0x00661824 /* Noise Range upper Threshold 0 */
#define BCHP_HD_ANR_AND_0_NOISE_UPPER_TH_1       0x00661828 /* Noise Range upper Threshold 1 */
#define BCHP_HD_ANR_AND_0_NOISE_UPPER_TH_2       0x0066182c /* Noise Range upper Threshold 2 */
#define BCHP_HD_ANR_AND_0_NOISE_UPPER_TH_3       0x00661830 /* Noise Range upper Threshold 3 */
#define BCHP_HD_ANR_AND_0_NOISE_UPPER_TH_4       0x00661834 /* Noise Range upper Threshold 4 */
#define BCHP_HD_ANR_AND_0_EDGE_TH                0x00661838 /* Edge gradient threshold */
#define BCHP_HD_ANR_AND_0_CONTENT_TH             0x0066183c /* Content level threshold */
#define BCHP_HD_ANR_AND_0_NOISY_SAMPLE_NUM_BIN_0 0x00661840 /* Number of noisy pixel in frame for noise level category 0 */
#define BCHP_HD_ANR_AND_0_NOISY_SAMPLE_NUM_BIN_1 0x00661844 /* Number of noisy pixel in frame for noise level category 1 */
#define BCHP_HD_ANR_AND_0_NOISY_SAMPLE_NUM_BIN_2 0x00661848 /* Number of noisy pixel in frame for noise level category 2 */
#define BCHP_HD_ANR_AND_0_NOISY_SAMPLE_NUM_BIN_3 0x0066184c /* Number of noisy pixel in frame for noise level category 3 */
#define BCHP_HD_ANR_AND_0_NOISY_SAMPLE_NUM_BIN_4 0x00661850 /* Number of noisy pixel in frame for noise level category 4 */
#define BCHP_HD_ANR_AND_0_NOISE_LEVEL_0_MSB      0x00661854 /* The means result of noise category 0 (MSB) */
#define BCHP_HD_ANR_AND_0_NOISE_LEVEL_0_LSB      0x00661858 /* The means result of noise category 0 (LSB) */
#define BCHP_HD_ANR_AND_0_NOISE_LEVEL_1_MSB      0x0066185c /* The means result of noise category 1 (MSB) */
#define BCHP_HD_ANR_AND_0_NOISE_LEVEL_1_LSB      0x00661860 /* The means result of noise category 1 (LSB) */
#define BCHP_HD_ANR_AND_0_NOISE_LEVEL_2_MSB      0x00661864 /* The means result of noise category 2 (MSB) */
#define BCHP_HD_ANR_AND_0_NOISE_LEVEL_2_LSB      0x00661868 /* The means result of noise category 2 (LSB) */
#define BCHP_HD_ANR_AND_0_NOISE_LEVEL_3_MSB      0x0066186c /* The means result of noise category 3 (MSB) */
#define BCHP_HD_ANR_AND_0_NOISE_LEVEL_3_LSB      0x00661870 /* The means result of noise category 3 (LSB) */
#define BCHP_HD_ANR_AND_0_NOISE_LEVEL_4_MSB      0x00661874 /* The means result of noise category 4 (MSB) */
#define BCHP_HD_ANR_AND_0_NOISE_LEVEL_4_LSB      0x00661878 /* The means result of noise category 4 (LSB) */
#define BCHP_HD_ANR_AND_0_X_LOWER_0              0x0066187c /* The lower boundary for statistics collection at horizontal direction */
#define BCHP_HD_ANR_AND_0_X_UPPER_0              0x00661880 /* The upper boundary for statistics collection at horizontal direction */
#define BCHP_HD_ANR_AND_0_Y_LOWER_0              0x00661884 /* The lower boundary for statistics collection at vertical direction */
#define BCHP_HD_ANR_AND_0_Y_UPPER_0              0x00661888 /* The upper boundary for statistics collection at vertical direction */

/***************************************************************************
 *REVISION_ID - AND Revision register
 ***************************************************************************/
/* HD_ANR_AND_0 :: REVISION_ID :: reserved0 [31:16] */
#define BCHP_HD_ANR_AND_0_REVISION_ID_reserved0_MASK               0xffff0000
#define BCHP_HD_ANR_AND_0_REVISION_ID_reserved0_SHIFT              16

/* HD_ANR_AND_0 :: REVISION_ID :: MAJOR [15:08] */
#define BCHP_HD_ANR_AND_0_REVISION_ID_MAJOR_MASK                   0x0000ff00
#define BCHP_HD_ANR_AND_0_REVISION_ID_MAJOR_SHIFT                  8
#define BCHP_HD_ANR_AND_0_REVISION_ID_MAJOR_DEFAULT                0x00000002

/* HD_ANR_AND_0 :: REVISION_ID :: MINOR [07:00] */
#define BCHP_HD_ANR_AND_0_REVISION_ID_MINOR_MASK                   0x000000ff
#define BCHP_HD_ANR_AND_0_REVISION_ID_MINOR_SHIFT                  0
#define BCHP_HD_ANR_AND_0_REVISION_ID_MINOR_DEFAULT                0x00000013

/***************************************************************************
 *SW_RESET - AND Software Reset
 ***************************************************************************/
/* HD_ANR_AND_0 :: SW_RESET :: reserved0 [31:01] */
#define BCHP_HD_ANR_AND_0_SW_RESET_reserved0_MASK                  0xfffffffe
#define BCHP_HD_ANR_AND_0_SW_RESET_reserved0_SHIFT                 1

/* HD_ANR_AND_0 :: SW_RESET :: SW_RESET [00:00] */
#define BCHP_HD_ANR_AND_0_SW_RESET_SW_RESET_MASK                   0x00000001
#define BCHP_HD_ANR_AND_0_SW_RESET_SW_RESET_SHIFT                  0
#define BCHP_HD_ANR_AND_0_SW_RESET_SW_RESET_DEFAULT                0x00000000

/***************************************************************************
 *AND_ENABLE - AND Enable
 ***************************************************************************/
/* HD_ANR_AND_0 :: AND_ENABLE :: reserved0 [31:02] */
#define BCHP_HD_ANR_AND_0_AND_ENABLE_reserved0_MASK                0xfffffffc
#define BCHP_HD_ANR_AND_0_AND_ENABLE_reserved0_SHIFT               2

/* HD_ANR_AND_0 :: AND_ENABLE :: AND_DRAIN [01:01] */
#define BCHP_HD_ANR_AND_0_AND_ENABLE_AND_DRAIN_MASK                0x00000002
#define BCHP_HD_ANR_AND_0_AND_ENABLE_AND_DRAIN_SHIFT               1
#define BCHP_HD_ANR_AND_0_AND_ENABLE_AND_DRAIN_DEFAULT             0x00000000

/* HD_ANR_AND_0 :: AND_ENABLE :: AND_ENABLE [00:00] */
#define BCHP_HD_ANR_AND_0_AND_ENABLE_AND_ENABLE_MASK               0x00000001
#define BCHP_HD_ANR_AND_0_AND_ENABLE_AND_ENABLE_SHIFT              0
#define BCHP_HD_ANR_AND_0_AND_ENABLE_AND_ENABLE_DEFAULT            0x00000000

/***************************************************************************
 *AND_MODE - AND Window Size
 ***************************************************************************/
/* HD_ANR_AND_0 :: AND_MODE :: reserved0 [31:02] */
#define BCHP_HD_ANR_AND_0_AND_MODE_reserved0_MASK                  0xfffffffc
#define BCHP_HD_ANR_AND_0_AND_MODE_reserved0_SHIFT                 2

/* HD_ANR_AND_0 :: AND_MODE :: AND_WIN_SIZE [01:00] */
#define BCHP_HD_ANR_AND_0_AND_MODE_AND_WIN_SIZE_MASK               0x00000003
#define BCHP_HD_ANR_AND_0_AND_MODE_AND_WIN_SIZE_SHIFT              0
#define BCHP_HD_ANR_AND_0_AND_MODE_AND_WIN_SIZE_DEFAULT            0x00000001
#define BCHP_HD_ANR_AND_0_AND_MODE_AND_WIN_SIZE_WIN_SIZE_3X3       0
#define BCHP_HD_ANR_AND_0_AND_MODE_AND_WIN_SIZE_WIN_SIZE_5X3       1
#define BCHP_HD_ANR_AND_0_AND_MODE_AND_WIN_SIZE_WIN_SIZE_7X3       2

/***************************************************************************
 *NOISE_LOWER_TH_0 - Noise Range Lower Threshold 0
 ***************************************************************************/
/* HD_ANR_AND_0 :: NOISE_LOWER_TH_0 :: reserved0 [31:12] */
#define BCHP_HD_ANR_AND_0_NOISE_LOWER_TH_0_reserved0_MASK          0xfffff000
#define BCHP_HD_ANR_AND_0_NOISE_LOWER_TH_0_reserved0_SHIFT         12

/* HD_ANR_AND_0 :: NOISE_LOWER_TH_0 :: NOISE_LTH [11:00] */
#define BCHP_HD_ANR_AND_0_NOISE_LOWER_TH_0_NOISE_LTH_MASK          0x00000fff
#define BCHP_HD_ANR_AND_0_NOISE_LOWER_TH_0_NOISE_LTH_SHIFT         0
#define BCHP_HD_ANR_AND_0_NOISE_LOWER_TH_0_NOISE_LTH_DEFAULT       0x00000000

/***************************************************************************
 *NOISE_LOWER_TH_1 - Noise Range Lower Threshold 1
 ***************************************************************************/
/* HD_ANR_AND_0 :: NOISE_LOWER_TH_1 :: reserved0 [31:12] */
#define BCHP_HD_ANR_AND_0_NOISE_LOWER_TH_1_reserved0_MASK          0xfffff000
#define BCHP_HD_ANR_AND_0_NOISE_LOWER_TH_1_reserved0_SHIFT         12

/* HD_ANR_AND_0 :: NOISE_LOWER_TH_1 :: NOISE_LTH [11:00] */
#define BCHP_HD_ANR_AND_0_NOISE_LOWER_TH_1_NOISE_LTH_MASK          0x00000fff
#define BCHP_HD_ANR_AND_0_NOISE_LOWER_TH_1_NOISE_LTH_SHIFT         0
#define BCHP_HD_ANR_AND_0_NOISE_LOWER_TH_1_NOISE_LTH_DEFAULT       0x0000001a

/***************************************************************************
 *NOISE_LOWER_TH_2 - Noise Range Lower Threshold 2
 ***************************************************************************/
/* HD_ANR_AND_0 :: NOISE_LOWER_TH_2 :: reserved0 [31:12] */
#define BCHP_HD_ANR_AND_0_NOISE_LOWER_TH_2_reserved0_MASK          0xfffff000
#define BCHP_HD_ANR_AND_0_NOISE_LOWER_TH_2_reserved0_SHIFT         12

/* HD_ANR_AND_0 :: NOISE_LOWER_TH_2 :: NOISE_LTH [11:00] */
#define BCHP_HD_ANR_AND_0_NOISE_LOWER_TH_2_NOISE_LTH_MASK          0x00000fff
#define BCHP_HD_ANR_AND_0_NOISE_LOWER_TH_2_NOISE_LTH_SHIFT         0
#define BCHP_HD_ANR_AND_0_NOISE_LOWER_TH_2_NOISE_LTH_DEFAULT       0x00000034

/***************************************************************************
 *NOISE_LOWER_TH_3 - Noise Range Lower Threshold 3
 ***************************************************************************/
/* HD_ANR_AND_0 :: NOISE_LOWER_TH_3 :: reserved0 [31:12] */
#define BCHP_HD_ANR_AND_0_NOISE_LOWER_TH_3_reserved0_MASK          0xfffff000
#define BCHP_HD_ANR_AND_0_NOISE_LOWER_TH_3_reserved0_SHIFT         12

/* HD_ANR_AND_0 :: NOISE_LOWER_TH_3 :: NOISE_LTH [11:00] */
#define BCHP_HD_ANR_AND_0_NOISE_LOWER_TH_3_NOISE_LTH_MASK          0x00000fff
#define BCHP_HD_ANR_AND_0_NOISE_LOWER_TH_3_NOISE_LTH_SHIFT         0
#define BCHP_HD_ANR_AND_0_NOISE_LOWER_TH_3_NOISE_LTH_DEFAULT       0x00000069

/***************************************************************************
 *NOISE_LOWER_TH_4 - Noise Range Lower Threshold 4
 ***************************************************************************/
/* HD_ANR_AND_0 :: NOISE_LOWER_TH_4 :: reserved0 [31:12] */
#define BCHP_HD_ANR_AND_0_NOISE_LOWER_TH_4_reserved0_MASK          0xfffff000
#define BCHP_HD_ANR_AND_0_NOISE_LOWER_TH_4_reserved0_SHIFT         12

/* HD_ANR_AND_0 :: NOISE_LOWER_TH_4 :: NOISE_LTH [11:00] */
#define BCHP_HD_ANR_AND_0_NOISE_LOWER_TH_4_NOISE_LTH_MASK          0x00000fff
#define BCHP_HD_ANR_AND_0_NOISE_LOWER_TH_4_NOISE_LTH_SHIFT         0
#define BCHP_HD_ANR_AND_0_NOISE_LOWER_TH_4_NOISE_LTH_DEFAULT       0x000000d1

/***************************************************************************
 *NOISE_UPPER_TH_0 - Noise Range upper Threshold 0
 ***************************************************************************/
/* HD_ANR_AND_0 :: NOISE_UPPER_TH_0 :: reserved0 [31:12] */
#define BCHP_HD_ANR_AND_0_NOISE_UPPER_TH_0_reserved0_MASK          0xfffff000
#define BCHP_HD_ANR_AND_0_NOISE_UPPER_TH_0_reserved0_SHIFT         12

/* HD_ANR_AND_0 :: NOISE_UPPER_TH_0 :: NOISE_UTH [11:00] */
#define BCHP_HD_ANR_AND_0_NOISE_UPPER_TH_0_NOISE_UTH_MASK          0x00000fff
#define BCHP_HD_ANR_AND_0_NOISE_UPPER_TH_0_NOISE_UTH_SHIFT         0
#define BCHP_HD_ANR_AND_0_NOISE_UPPER_TH_0_NOISE_UTH_DEFAULT       0x00000037

/***************************************************************************
 *NOISE_UPPER_TH_1 - Noise Range upper Threshold 1
 ***************************************************************************/
/* HD_ANR_AND_0 :: NOISE_UPPER_TH_1 :: reserved0 [31:12] */
#define BCHP_HD_ANR_AND_0_NOISE_UPPER_TH_1_reserved0_MASK          0xfffff000
#define BCHP_HD_ANR_AND_0_NOISE_UPPER_TH_1_reserved0_SHIFT         12

/* HD_ANR_AND_0 :: NOISE_UPPER_TH_1 :: NOISE_UTH [11:00] */
#define BCHP_HD_ANR_AND_0_NOISE_UPPER_TH_1_NOISE_UTH_MASK          0x00000fff
#define BCHP_HD_ANR_AND_0_NOISE_UPPER_TH_1_NOISE_UTH_SHIFT         0
#define BCHP_HD_ANR_AND_0_NOISE_UPPER_TH_1_NOISE_UTH_DEFAULT       0x0000006d

/***************************************************************************
 *NOISE_UPPER_TH_2 - Noise Range upper Threshold 2
 ***************************************************************************/
/* HD_ANR_AND_0 :: NOISE_UPPER_TH_2 :: reserved0 [31:12] */
#define BCHP_HD_ANR_AND_0_NOISE_UPPER_TH_2_reserved0_MASK          0xfffff000
#define BCHP_HD_ANR_AND_0_NOISE_UPPER_TH_2_reserved0_SHIFT         12

/* HD_ANR_AND_0 :: NOISE_UPPER_TH_2 :: NOISE_UTH [11:00] */
#define BCHP_HD_ANR_AND_0_NOISE_UPPER_TH_2_NOISE_UTH_MASK          0x00000fff
#define BCHP_HD_ANR_AND_0_NOISE_UPPER_TH_2_NOISE_UTH_SHIFT         0
#define BCHP_HD_ANR_AND_0_NOISE_UPPER_TH_2_NOISE_UTH_DEFAULT       0x000000df

/***************************************************************************
 *NOISE_UPPER_TH_3 - Noise Range upper Threshold 3
 ***************************************************************************/
/* HD_ANR_AND_0 :: NOISE_UPPER_TH_3 :: reserved0 [31:12] */
#define BCHP_HD_ANR_AND_0_NOISE_UPPER_TH_3_reserved0_MASK          0xfffff000
#define BCHP_HD_ANR_AND_0_NOISE_UPPER_TH_3_reserved0_SHIFT         12

/* HD_ANR_AND_0 :: NOISE_UPPER_TH_3 :: NOISE_UTH [11:00] */
#define BCHP_HD_ANR_AND_0_NOISE_UPPER_TH_3_NOISE_UTH_MASK          0x00000fff
#define BCHP_HD_ANR_AND_0_NOISE_UPPER_TH_3_NOISE_UTH_SHIFT         0
#define BCHP_HD_ANR_AND_0_NOISE_UPPER_TH_3_NOISE_UTH_DEFAULT       0x000001bd

/***************************************************************************
 *NOISE_UPPER_TH_4 - Noise Range upper Threshold 4
 ***************************************************************************/
/* HD_ANR_AND_0 :: NOISE_UPPER_TH_4 :: reserved0 [31:12] */
#define BCHP_HD_ANR_AND_0_NOISE_UPPER_TH_4_reserved0_MASK          0xfffff000
#define BCHP_HD_ANR_AND_0_NOISE_UPPER_TH_4_reserved0_SHIFT         12

/* HD_ANR_AND_0 :: NOISE_UPPER_TH_4 :: NOISE_UTH [11:00] */
#define BCHP_HD_ANR_AND_0_NOISE_UPPER_TH_4_NOISE_UTH_MASK          0x00000fff
#define BCHP_HD_ANR_AND_0_NOISE_UPPER_TH_4_NOISE_UTH_SHIFT         0
#define BCHP_HD_ANR_AND_0_NOISE_UPPER_TH_4_NOISE_UTH_DEFAULT       0x00000378

/***************************************************************************
 *EDGE_TH - Edge gradient threshold
 ***************************************************************************/
/* HD_ANR_AND_0 :: EDGE_TH :: reserved0 [31:08] */
#define BCHP_HD_ANR_AND_0_EDGE_TH_reserved0_MASK                   0xffffff00
#define BCHP_HD_ANR_AND_0_EDGE_TH_reserved0_SHIFT                  8

/* HD_ANR_AND_0 :: EDGE_TH :: EDGE_TH [07:00] */
#define BCHP_HD_ANR_AND_0_EDGE_TH_EDGE_TH_MASK                     0x000000ff
#define BCHP_HD_ANR_AND_0_EDGE_TH_EDGE_TH_SHIFT                    0
#define BCHP_HD_ANR_AND_0_EDGE_TH_EDGE_TH_DEFAULT                  0x0000000f

/***************************************************************************
 *CONTENT_TH - Content level threshold
 ***************************************************************************/
/* HD_ANR_AND_0 :: CONTENT_TH :: reserved0 [31:08] */
#define BCHP_HD_ANR_AND_0_CONTENT_TH_reserved0_MASK                0xffffff00
#define BCHP_HD_ANR_AND_0_CONTENT_TH_reserved0_SHIFT               8

/* HD_ANR_AND_0 :: CONTENT_TH :: CONTENT_TH [07:00] */
#define BCHP_HD_ANR_AND_0_CONTENT_TH_CONTENT_TH_MASK               0x000000ff
#define BCHP_HD_ANR_AND_0_CONTENT_TH_CONTENT_TH_SHIFT              0
#define BCHP_HD_ANR_AND_0_CONTENT_TH_CONTENT_TH_DEFAULT            0x0000002a

/***************************************************************************
 *NOISY_SAMPLE_NUM_BIN_0 - Number of noisy pixel in frame for noise level category 0
 ***************************************************************************/
/* HD_ANR_AND_0 :: NOISY_SAMPLE_NUM_BIN_0 :: reserved0 [31:23] */
#define BCHP_HD_ANR_AND_0_NOISY_SAMPLE_NUM_BIN_0_reserved0_MASK    0xff800000
#define BCHP_HD_ANR_AND_0_NOISY_SAMPLE_NUM_BIN_0_reserved0_SHIFT   23

/* HD_ANR_AND_0 :: NOISY_SAMPLE_NUM_BIN_0 :: noisy_sample_num_bin [22:00] */
#define BCHP_HD_ANR_AND_0_NOISY_SAMPLE_NUM_BIN_0_noisy_sample_num_bin_MASK 0x007fffff
#define BCHP_HD_ANR_AND_0_NOISY_SAMPLE_NUM_BIN_0_noisy_sample_num_bin_SHIFT 0
#define BCHP_HD_ANR_AND_0_NOISY_SAMPLE_NUM_BIN_0_noisy_sample_num_bin_DEFAULT 0x00000000

/***************************************************************************
 *NOISY_SAMPLE_NUM_BIN_1 - Number of noisy pixel in frame for noise level category 1
 ***************************************************************************/
/* HD_ANR_AND_0 :: NOISY_SAMPLE_NUM_BIN_1 :: reserved0 [31:23] */
#define BCHP_HD_ANR_AND_0_NOISY_SAMPLE_NUM_BIN_1_reserved0_MASK    0xff800000
#define BCHP_HD_ANR_AND_0_NOISY_SAMPLE_NUM_BIN_1_reserved0_SHIFT   23

/* HD_ANR_AND_0 :: NOISY_SAMPLE_NUM_BIN_1 :: noisy_sample_num_bin [22:00] */
#define BCHP_HD_ANR_AND_0_NOISY_SAMPLE_NUM_BIN_1_noisy_sample_num_bin_MASK 0x007fffff
#define BCHP_HD_ANR_AND_0_NOISY_SAMPLE_NUM_BIN_1_noisy_sample_num_bin_SHIFT 0
#define BCHP_HD_ANR_AND_0_NOISY_SAMPLE_NUM_BIN_1_noisy_sample_num_bin_DEFAULT 0x00000000

/***************************************************************************
 *NOISY_SAMPLE_NUM_BIN_2 - Number of noisy pixel in frame for noise level category 2
 ***************************************************************************/
/* HD_ANR_AND_0 :: NOISY_SAMPLE_NUM_BIN_2 :: reserved0 [31:23] */
#define BCHP_HD_ANR_AND_0_NOISY_SAMPLE_NUM_BIN_2_reserved0_MASK    0xff800000
#define BCHP_HD_ANR_AND_0_NOISY_SAMPLE_NUM_BIN_2_reserved0_SHIFT   23

/* HD_ANR_AND_0 :: NOISY_SAMPLE_NUM_BIN_2 :: noisy_sample_num_bin [22:00] */
#define BCHP_HD_ANR_AND_0_NOISY_SAMPLE_NUM_BIN_2_noisy_sample_num_bin_MASK 0x007fffff
#define BCHP_HD_ANR_AND_0_NOISY_SAMPLE_NUM_BIN_2_noisy_sample_num_bin_SHIFT 0
#define BCHP_HD_ANR_AND_0_NOISY_SAMPLE_NUM_BIN_2_noisy_sample_num_bin_DEFAULT 0x00000000

/***************************************************************************
 *NOISY_SAMPLE_NUM_BIN_3 - Number of noisy pixel in frame for noise level category 3
 ***************************************************************************/
/* HD_ANR_AND_0 :: NOISY_SAMPLE_NUM_BIN_3 :: reserved0 [31:23] */
#define BCHP_HD_ANR_AND_0_NOISY_SAMPLE_NUM_BIN_3_reserved0_MASK    0xff800000
#define BCHP_HD_ANR_AND_0_NOISY_SAMPLE_NUM_BIN_3_reserved0_SHIFT   23

/* HD_ANR_AND_0 :: NOISY_SAMPLE_NUM_BIN_3 :: noisy_sample_num_bin [22:00] */
#define BCHP_HD_ANR_AND_0_NOISY_SAMPLE_NUM_BIN_3_noisy_sample_num_bin_MASK 0x007fffff
#define BCHP_HD_ANR_AND_0_NOISY_SAMPLE_NUM_BIN_3_noisy_sample_num_bin_SHIFT 0
#define BCHP_HD_ANR_AND_0_NOISY_SAMPLE_NUM_BIN_3_noisy_sample_num_bin_DEFAULT 0x00000000

/***************************************************************************
 *NOISY_SAMPLE_NUM_BIN_4 - Number of noisy pixel in frame for noise level category 4
 ***************************************************************************/
/* HD_ANR_AND_0 :: NOISY_SAMPLE_NUM_BIN_4 :: reserved0 [31:23] */
#define BCHP_HD_ANR_AND_0_NOISY_SAMPLE_NUM_BIN_4_reserved0_MASK    0xff800000
#define BCHP_HD_ANR_AND_0_NOISY_SAMPLE_NUM_BIN_4_reserved0_SHIFT   23

/* HD_ANR_AND_0 :: NOISY_SAMPLE_NUM_BIN_4 :: noisy_sample_num_bin [22:00] */
#define BCHP_HD_ANR_AND_0_NOISY_SAMPLE_NUM_BIN_4_noisy_sample_num_bin_MASK 0x007fffff
#define BCHP_HD_ANR_AND_0_NOISY_SAMPLE_NUM_BIN_4_noisy_sample_num_bin_SHIFT 0
#define BCHP_HD_ANR_AND_0_NOISY_SAMPLE_NUM_BIN_4_noisy_sample_num_bin_DEFAULT 0x00000000

/***************************************************************************
 *NOISE_LEVEL_0_MSB - The means result of noise category 0 (MSB)
 ***************************************************************************/
/* HD_ANR_AND_0 :: NOISE_LEVEL_0_MSB :: reserved0 [31:05] */
#define BCHP_HD_ANR_AND_0_NOISE_LEVEL_0_MSB_reserved0_MASK         0xffffffe0
#define BCHP_HD_ANR_AND_0_NOISE_LEVEL_0_MSB_reserved0_SHIFT        5

/* HD_ANR_AND_0 :: NOISE_LEVEL_0_MSB :: noise_level_msb [04:00] */
#define BCHP_HD_ANR_AND_0_NOISE_LEVEL_0_MSB_noise_level_msb_MASK   0x0000001f
#define BCHP_HD_ANR_AND_0_NOISE_LEVEL_0_MSB_noise_level_msb_SHIFT  0
#define BCHP_HD_ANR_AND_0_NOISE_LEVEL_0_MSB_noise_level_msb_DEFAULT 0x00000000

/***************************************************************************
 *NOISE_LEVEL_0_LSB - The means result of noise category 0 (LSB)
 ***************************************************************************/
/* HD_ANR_AND_0 :: NOISE_LEVEL_0_LSB :: noise_level_lsb [31:00] */
#define BCHP_HD_ANR_AND_0_NOISE_LEVEL_0_LSB_noise_level_lsb_MASK   0xffffffff
#define BCHP_HD_ANR_AND_0_NOISE_LEVEL_0_LSB_noise_level_lsb_SHIFT  0
#define BCHP_HD_ANR_AND_0_NOISE_LEVEL_0_LSB_noise_level_lsb_DEFAULT 0x00000000

/***************************************************************************
 *NOISE_LEVEL_1_MSB - The means result of noise category 1 (MSB)
 ***************************************************************************/
/* HD_ANR_AND_0 :: NOISE_LEVEL_1_MSB :: reserved0 [31:05] */
#define BCHP_HD_ANR_AND_0_NOISE_LEVEL_1_MSB_reserved0_MASK         0xffffffe0
#define BCHP_HD_ANR_AND_0_NOISE_LEVEL_1_MSB_reserved0_SHIFT        5

/* HD_ANR_AND_0 :: NOISE_LEVEL_1_MSB :: noise_level_msb [04:00] */
#define BCHP_HD_ANR_AND_0_NOISE_LEVEL_1_MSB_noise_level_msb_MASK   0x0000001f
#define BCHP_HD_ANR_AND_0_NOISE_LEVEL_1_MSB_noise_level_msb_SHIFT  0
#define BCHP_HD_ANR_AND_0_NOISE_LEVEL_1_MSB_noise_level_msb_DEFAULT 0x00000000

/***************************************************************************
 *NOISE_LEVEL_1_LSB - The means result of noise category 1 (LSB)
 ***************************************************************************/
/* HD_ANR_AND_0 :: NOISE_LEVEL_1_LSB :: noise_level_lsb [31:00] */
#define BCHP_HD_ANR_AND_0_NOISE_LEVEL_1_LSB_noise_level_lsb_MASK   0xffffffff
#define BCHP_HD_ANR_AND_0_NOISE_LEVEL_1_LSB_noise_level_lsb_SHIFT  0
#define BCHP_HD_ANR_AND_0_NOISE_LEVEL_1_LSB_noise_level_lsb_DEFAULT 0x00000000

/***************************************************************************
 *NOISE_LEVEL_2_MSB - The means result of noise category 2 (MSB)
 ***************************************************************************/
/* HD_ANR_AND_0 :: NOISE_LEVEL_2_MSB :: reserved0 [31:05] */
#define BCHP_HD_ANR_AND_0_NOISE_LEVEL_2_MSB_reserved0_MASK         0xffffffe0
#define BCHP_HD_ANR_AND_0_NOISE_LEVEL_2_MSB_reserved0_SHIFT        5

/* HD_ANR_AND_0 :: NOISE_LEVEL_2_MSB :: noise_level_msb [04:00] */
#define BCHP_HD_ANR_AND_0_NOISE_LEVEL_2_MSB_noise_level_msb_MASK   0x0000001f
#define BCHP_HD_ANR_AND_0_NOISE_LEVEL_2_MSB_noise_level_msb_SHIFT  0
#define BCHP_HD_ANR_AND_0_NOISE_LEVEL_2_MSB_noise_level_msb_DEFAULT 0x00000000

/***************************************************************************
 *NOISE_LEVEL_2_LSB - The means result of noise category 2 (LSB)
 ***************************************************************************/
/* HD_ANR_AND_0 :: NOISE_LEVEL_2_LSB :: noise_level_lsb [31:00] */
#define BCHP_HD_ANR_AND_0_NOISE_LEVEL_2_LSB_noise_level_lsb_MASK   0xffffffff
#define BCHP_HD_ANR_AND_0_NOISE_LEVEL_2_LSB_noise_level_lsb_SHIFT  0
#define BCHP_HD_ANR_AND_0_NOISE_LEVEL_2_LSB_noise_level_lsb_DEFAULT 0x00000000

/***************************************************************************
 *NOISE_LEVEL_3_MSB - The means result of noise category 3 (MSB)
 ***************************************************************************/
/* HD_ANR_AND_0 :: NOISE_LEVEL_3_MSB :: reserved0 [31:05] */
#define BCHP_HD_ANR_AND_0_NOISE_LEVEL_3_MSB_reserved0_MASK         0xffffffe0
#define BCHP_HD_ANR_AND_0_NOISE_LEVEL_3_MSB_reserved0_SHIFT        5

/* HD_ANR_AND_0 :: NOISE_LEVEL_3_MSB :: noise_level_msb [04:00] */
#define BCHP_HD_ANR_AND_0_NOISE_LEVEL_3_MSB_noise_level_msb_MASK   0x0000001f
#define BCHP_HD_ANR_AND_0_NOISE_LEVEL_3_MSB_noise_level_msb_SHIFT  0
#define BCHP_HD_ANR_AND_0_NOISE_LEVEL_3_MSB_noise_level_msb_DEFAULT 0x00000000

/***************************************************************************
 *NOISE_LEVEL_3_LSB - The means result of noise category 3 (LSB)
 ***************************************************************************/
/* HD_ANR_AND_0 :: NOISE_LEVEL_3_LSB :: noise_level_lsb [31:00] */
#define BCHP_HD_ANR_AND_0_NOISE_LEVEL_3_LSB_noise_level_lsb_MASK   0xffffffff
#define BCHP_HD_ANR_AND_0_NOISE_LEVEL_3_LSB_noise_level_lsb_SHIFT  0
#define BCHP_HD_ANR_AND_0_NOISE_LEVEL_3_LSB_noise_level_lsb_DEFAULT 0x00000000

/***************************************************************************
 *NOISE_LEVEL_4_MSB - The means result of noise category 4 (MSB)
 ***************************************************************************/
/* HD_ANR_AND_0 :: NOISE_LEVEL_4_MSB :: reserved0 [31:05] */
#define BCHP_HD_ANR_AND_0_NOISE_LEVEL_4_MSB_reserved0_MASK         0xffffffe0
#define BCHP_HD_ANR_AND_0_NOISE_LEVEL_4_MSB_reserved0_SHIFT        5

/* HD_ANR_AND_0 :: NOISE_LEVEL_4_MSB :: noise_level_msb [04:00] */
#define BCHP_HD_ANR_AND_0_NOISE_LEVEL_4_MSB_noise_level_msb_MASK   0x0000001f
#define BCHP_HD_ANR_AND_0_NOISE_LEVEL_4_MSB_noise_level_msb_SHIFT  0
#define BCHP_HD_ANR_AND_0_NOISE_LEVEL_4_MSB_noise_level_msb_DEFAULT 0x00000000

/***************************************************************************
 *NOISE_LEVEL_4_LSB - The means result of noise category 4 (LSB)
 ***************************************************************************/
/* HD_ANR_AND_0 :: NOISE_LEVEL_4_LSB :: noise_level_lsb [31:00] */
#define BCHP_HD_ANR_AND_0_NOISE_LEVEL_4_LSB_noise_level_lsb_MASK   0xffffffff
#define BCHP_HD_ANR_AND_0_NOISE_LEVEL_4_LSB_noise_level_lsb_SHIFT  0
#define BCHP_HD_ANR_AND_0_NOISE_LEVEL_4_LSB_noise_level_lsb_DEFAULT 0x00000000

/***************************************************************************
 *X_LOWER_0 - The lower boundary for statistics collection at horizontal direction
 ***************************************************************************/
/* HD_ANR_AND_0 :: X_LOWER_0 :: reserved0 [31:11] */
#define BCHP_HD_ANR_AND_0_X_LOWER_0_reserved0_MASK                 0xfffff800
#define BCHP_HD_ANR_AND_0_X_LOWER_0_reserved0_SHIFT                11

/* HD_ANR_AND_0 :: X_LOWER_0 :: X_LOWER [10:00] */
#define BCHP_HD_ANR_AND_0_X_LOWER_0_X_LOWER_MASK                   0x000007ff
#define BCHP_HD_ANR_AND_0_X_LOWER_0_X_LOWER_SHIFT                  0
#define BCHP_HD_ANR_AND_0_X_LOWER_0_X_LOWER_DEFAULT                0x00000000

/***************************************************************************
 *X_UPPER_0 - The upper boundary for statistics collection at horizontal direction
 ***************************************************************************/
/* HD_ANR_AND_0 :: X_UPPER_0 :: reserved0 [31:11] */
#define BCHP_HD_ANR_AND_0_X_UPPER_0_reserved0_MASK                 0xfffff800
#define BCHP_HD_ANR_AND_0_X_UPPER_0_reserved0_SHIFT                11

/* HD_ANR_AND_0 :: X_UPPER_0 :: X_UPPER [10:00] */
#define BCHP_HD_ANR_AND_0_X_UPPER_0_X_UPPER_MASK                   0x000007ff
#define BCHP_HD_ANR_AND_0_X_UPPER_0_X_UPPER_SHIFT                  0
#define BCHP_HD_ANR_AND_0_X_UPPER_0_X_UPPER_DEFAULT                0x00000780

/***************************************************************************
 *Y_LOWER_0 - The lower boundary for statistics collection at vertical direction
 ***************************************************************************/
/* HD_ANR_AND_0 :: Y_LOWER_0 :: reserved0 [31:12] */
#define BCHP_HD_ANR_AND_0_Y_LOWER_0_reserved0_MASK                 0xfffff000
#define BCHP_HD_ANR_AND_0_Y_LOWER_0_reserved0_SHIFT                12

/* HD_ANR_AND_0 :: Y_LOWER_0 :: Y_LOWER [11:00] */
#define BCHP_HD_ANR_AND_0_Y_LOWER_0_Y_LOWER_MASK                   0x00000fff
#define BCHP_HD_ANR_AND_0_Y_LOWER_0_Y_LOWER_SHIFT                  0
#define BCHP_HD_ANR_AND_0_Y_LOWER_0_Y_LOWER_DEFAULT                0x00000000

/***************************************************************************
 *Y_UPPER_0 - The upper boundary for statistics collection at vertical direction
 ***************************************************************************/
/* HD_ANR_AND_0 :: Y_UPPER_0 :: reserved0 [31:12] */
#define BCHP_HD_ANR_AND_0_Y_UPPER_0_reserved0_MASK                 0xfffff000
#define BCHP_HD_ANR_AND_0_Y_UPPER_0_reserved0_SHIFT                12

/* HD_ANR_AND_0 :: Y_UPPER_0 :: Y_UPPER [11:00] */
#define BCHP_HD_ANR_AND_0_Y_UPPER_0_Y_UPPER_MASK                   0x00000fff
#define BCHP_HD_ANR_AND_0_Y_UPPER_0_Y_UPPER_SHIFT                  0
#define BCHP_HD_ANR_AND_0_Y_UPPER_0_Y_UPPER_DEFAULT                0x00000438

#endif /* #ifndef BCHP_HD_ANR_AND_0_H__ */

/* End of File */