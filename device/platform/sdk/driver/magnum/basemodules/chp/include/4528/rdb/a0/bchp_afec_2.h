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
 * $brcm_Workfile: bchp_afec_2.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 1/12/11 7:43p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Wed Jan 12 18:41:53 2011
 *                 MD5 Checksum         6e6727f6c827233acdd395c9a9032c98
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/4528/rdb/a0/bchp_afec_2.h $
 * 
 * Hydra_Software_Devel/1   1/12/11 7:43p pntruong
 * HW4528-2: Synced up with central rdb.
 *
 ***************************************************************************/

#ifndef BCHP_AFEC_2_H__
#define BCHP_AFEC_2_H__

/***************************************************************************
 *AFEC_2 - AFEC Register Set 2
 ***************************************************************************/
#define BCHP_AFEC_2_RST                          0x000c4000 /* AFEC core reset register */
#define BCHP_AFEC_2_CNTR_CTRL                    0x000c4004 /* AFEC counter-control register */
#define BCHP_AFEC_2_TEST_CONFIG                  0x000c4008 /* AFEC Test Configuration Register */
#define BCHP_AFEC_2_ACM_MAX_ITER_OVERIDE         0x000c4104 /* ACM Controller MODCOD MAX_ITER_OVERIDE selection register */
#define BCHP_AFEC_2_ACM_MODCOD_1                 0x000c4030 /* ACM Controller MODCOD Configuration Register 1 */
#define BCHP_AFEC_2_ACM_MODCOD_2                 0x000c4034 /* ACM Controller MODCOD Configuration Register 2 */
#define BCHP_AFEC_2_ACM_MODCOD_3                 0x000c4038 /* ACM Controller MODCOD Configuration Register 3 */
#define BCHP_AFEC_2_ACM_MODCOD_4                 0x000c403c /* ACM Controller MODCOD Configuration Register 4 */
#define BCHP_AFEC_2_ACM_MODCOD_5                 0x000c4040 /* ACM Controller MODCOD Configuration Register 5 */
#define BCHP_AFEC_2_ACM_MODCOD_6                 0x000c4044 /* ACM Controller MODCOD Configuration Register 6 */
#define BCHP_AFEC_2_ACM_MODCOD_7                 0x000c4048 /* ACM Controller MODCOD Configuration Register 7 */
#define BCHP_AFEC_2_ACM_MODCOD_8                 0x000c404c /* ACM Controller MODCOD Configuration Register 8 */
#define BCHP_AFEC_2_ACM_MODCOD_9                 0x000c4050 /* ACM Controller MODCOD Configuration Register 9 */
#define BCHP_AFEC_2_ACM_MODCOD_10                0x000c4054 /* ACM Controller MODCOD Configuration Register 10 */
#define BCHP_AFEC_2_ACM_MODCOD_11                0x000c4058 /* ACM Controller MODCOD Configuration Register 11 */
#define BCHP_AFEC_2_ACM_MODCOD_12                0x000c405c /* ACM Controller MODCOD Configuration Register 12 */
#define BCHP_AFEC_2_ACM_MODCOD_13                0x000c4060 /* ACM Controller MODCOD Configuration Register 13 */
#define BCHP_AFEC_2_ACM_MODCOD_14                0x000c4064 /* ACM Controller MODCOD Configuration Register 14 */
#define BCHP_AFEC_2_ACM_MODCOD_15                0x000c4068 /* ACM Controller MODCOD Configuration Register 15 */
#define BCHP_AFEC_2_ACM_MODCOD_16                0x000c406c /* ACM Controller MODCOD Configuration Register 16 */
#define BCHP_AFEC_2_ACM_MODCOD_17                0x000c4070 /* ACM Controller MODCOD Configuration Register 17 */
#define BCHP_AFEC_2_ACM_MODCOD_18                0x000c4074 /* ACM Controller MODCOD Configuration Register 18 */
#define BCHP_AFEC_2_ACM_MODCOD_19                0x000c4078 /* ACM Controller MODCOD Configuration Register 19 */
#define BCHP_AFEC_2_ACM_MODCOD_20                0x000c407c /* ACM Controller MODCOD Configuration Register 20 */
#define BCHP_AFEC_2_ACM_MODCOD_21                0x000c4080 /* ACM Controller MODCOD Configuration Register 21 */
#define BCHP_AFEC_2_ACM_MODCOD_22                0x000c4084 /* ACM Controller MODCOD Configuration Register 22 */
#define BCHP_AFEC_2_ACM_MODCOD_23                0x000c4088 /* ACM Controller MODCOD Configuration Register 23 */
#define BCHP_AFEC_2_ACM_MODCOD_24                0x000c408c /* ACM Controller MODCOD Configuration Register 24 */
#define BCHP_AFEC_2_ACM_MODCOD_25                0x000c4090 /* ACM Controller MODCOD Configuration Register 25 */
#define BCHP_AFEC_2_ACM_MODCOD_26                0x000c4094 /* ACM Controller MODCOD Configuration Register 26 */
#define BCHP_AFEC_2_ACM_MODCOD_27                0x000c4098 /* ACM Controller MODCOD Configuration Register 27 */
#define BCHP_AFEC_2_ACM_MODCOD_28                0x000c409c /* ACM Controller MODCOD Configuration Register 28 */
#define BCHP_AFEC_2_ACM_MODCOD_29_EXT            0x000c40a0 /* ACM Controller MODCOD Configuration Register 29 Extension */
#define BCHP_AFEC_2_ACM_MODCOD_29_LDPC0_EXT      0x000c40a4 /* ACM Controller LDPC Configuration Register */
#define BCHP_AFEC_2_ACM_MODCOD_30_EXT            0x000c40a8 /* ACM Controller MODCOD Configuration Register 30 Extension */
#define BCHP_AFEC_2_ACM_MODCOD_30_LDPC0_EXT      0x000c40ac /* ACM Controller LDPC Configuration Register */
#define BCHP_AFEC_2_ACM_MODCOD_31_EXT            0x000c40b0 /* ACM Controller MODCOD Configuration Register 31 Extension */
#define BCHP_AFEC_2_ACM_MODCOD_31_LDPC0_EXT      0x000c40b4 /* ACM Controller LDPC Configuration Register */
#define BCHP_AFEC_2_ACM_SYM_CNT_0                0x000c40b8 /* ACM Controller symbol counter */
#define BCHP_AFEC_2_ACM_SYM_CNT_1                0x000c40bc /* ACM Controller symbol counter */
#define BCHP_AFEC_2_ACM_SYM_CNT_2                0x000c40c0 /* ACM Controller symbol counter */
#define BCHP_AFEC_2_ACM_CYCLE_CNT_0              0x000c40c4 /* ACM Controller LDPC clk counter for QPSK */
#define BCHP_AFEC_2_ACM_CYCLE_CNT_1              0x000c40c8 /* ACM Controller LDPC clk counter for 8PSK */
#define BCHP_AFEC_2_ACM_CYCLE_CNT_2              0x000c40cc /* ACM Controller LDPC clk counter for 16APSK */
#define BCHP_AFEC_2_ACM_CYCLE_CNT_3              0x000c40d0 /* ACM Controller LDPC clk counter for 32APSK */
#define BCHP_AFEC_2_ACM_CYCLE_CNT_4              0x000c40d4 /* ACM Controller LDPC clk counter for BPSK */
#define BCHP_AFEC_2_ACM_MISC_0                   0x000c40d8 /* ACM Controller MISC_0 register */
#define BCHP_AFEC_2_ACM_MISC_1                   0x000c40dc /* ACM Controller MISC_1 register */
#define BCHP_AFEC_2_ACM_MODCOD_OVERIDE           0x000c40e0 /* ACM Controller MODCOD Overide Configuration Register */
#define BCHP_AFEC_2_ACM_MODCOD_STATS_CONFIG      0x000c40ec /* ACM MODCOD statistics configuration block */
#define BCHP_AFEC_2_ACM_DONT_DEC_CNT             0x000c40f0 /* ACM Dont decode counter */
#define BCHP_AFEC_2_ACM_LDPC_ITER_CNT            0x000c40f4 /* ACM Iteration counter */
#define BCHP_AFEC_2_ACM_LDPC_FAIL_CNT            0x000c40f8 /* ACM LDPC FAIL COUNTER */
#define BCHP_AFEC_2_ACM_LDPC_FRM_CNT             0x000c40fc /* ACM LDPC FRAME COUNTER */
#define BCHP_AFEC_2_LDPC_CONFIG_0                0x000c4300 /* LDPC Configuration Register 0 */
#define BCHP_AFEC_2_LDPC_STATUS                  0x000c4328 /* LDPC Status Register */
#define BCHP_AFEC_2_LDPC_MET_CRC                 0x000c432c /* Metric Generator Signature */
#define BCHP_AFEC_2_LDPC_EDGE_CRC                0x000c4330 /* Edge Output Signature */
#define BCHP_AFEC_2_LDPC_PSL_CTL                 0x000c4334 /* Power Saving Loop Control register */
#define BCHP_AFEC_2_LDPC_PSL_INT_THRES           0x000c4338 /* PSL Integrator Threshold */
#define BCHP_AFEC_2_LDPC_PSL_INT                 0x000c433c /* PSL Integrator Value */
#define BCHP_AFEC_2_LDPC_PSL_AVE                 0x000c4340 /* PSL Integrator Average Value */
#define BCHP_AFEC_2_LDPC_PSL_XCS                 0x000c4344 /* PSL Excess Value */
#define BCHP_AFEC_2_LDPC_PSL_FILTER              0x000c4348 /* PSL Filter for ACM */
#define BCHP_AFEC_2_LDPC_MEM_POWER               0x000c434c /* Dynamic memory shut down */
#define BCHP_AFEC_2_BCH_TPCTL                    0x000c43bc /* BCH Block Testport Control Register */
#define BCHP_AFEC_2_BCH_TPSIG                    0x000c43c0 /* BCH Block Signature Analyzer */
#define BCHP_AFEC_2_BCH_CTRL                     0x000c43c4 /* BCH Decoder Control Register */
#define BCHP_AFEC_2_BCH_DECNBLK                  0x000c43c8 /* BCH Number of Block Counter */
#define BCHP_AFEC_2_BCH_DECCBLK                  0x000c43cc /* BCH Number of Corrected Block Counter */
#define BCHP_AFEC_2_BCH_DECBBLK                  0x000c43d0 /* BCH Number of Bad Block Counter */
#define BCHP_AFEC_2_BCH_DECCBIT                  0x000c43d4 /* BCH Number of Corrected Bit Counter */
#define BCHP_AFEC_2_BCH_DECMCOR                  0x000c43d8 /* BCH Number of Miscorrected Block Counter */
#define BCHP_AFEC_2_BCH_BBHDR0                   0x000c43dc /* BBHEADER Register 0 */
#define BCHP_AFEC_2_BCH_BBHDR1                   0x000c43e0 /* BBHEADER Register 1 */
#define BCHP_AFEC_2_BCH_BBHDR2                   0x000c43e4 /* BBHEADER Register 2 */
#define BCHP_AFEC_2_BCH_BBHDR3                   0x000c43e8 /* BBHEADER Register 3 */
#define BCHP_AFEC_2_BCH_BBHDR4                   0x000c43ec /* BBHEADER Register 4 */
#define BCHP_AFEC_2_BCH_BBHDR5                   0x000c43f0 /* BBHEADER Register 5 */
#define BCHP_AFEC_2_BCH_MPLCK                    0x000c43f4 /* MPEG Lock Detector Configuration Register */
#define BCHP_AFEC_2_BCH_MPCFG                    0x000c43f8 /* MPEG Packetizer Configuration Register */
#define BCHP_AFEC_2_BCH_SMCFG                    0x000c43fc /* Smoother FIFO Configuration Register */

#endif /* #ifndef BCHP_AFEC_2_H__ */

/* End of File */