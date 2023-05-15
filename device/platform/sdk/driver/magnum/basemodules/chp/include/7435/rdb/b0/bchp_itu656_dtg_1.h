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
 * $brcm_Workfile: bchp_itu656_dtg_1.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 2/28/12 5:12p $
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
 * $brcm_Log: /magnum/basemodules/chp/7435/rdb/b0/bchp_itu656_dtg_1.h $
 * 
 * Hydra_Software_Devel/1   2/28/12 5:12p tdo
 * SW7435-40: Need 7435B0 public/central RDB (magnum) headers checked into
 * clearcase
 *
 ***************************************************************************/

#ifndef BCHP_ITU656_DTG_1_H__
#define BCHP_ITU656_DTG_1_H__

/***************************************************************************
 *ITU656_DTG_1 - ITU 656 Timing Control
 ***************************************************************************/
#define BCHP_ITU656_DTG_1_DTG_REV_ID             0x006b5800 /* Revision ID register */
#define BCHP_ITU656_DTG_1_DTG_DATA_CTRL          0x006b5804 /* DTG BVB data type */
#define BCHP_ITU656_DTG_1_DTG_CONFIG             0x006b5808 /* DTG Configuration Register */
#define BCHP_ITU656_DTG_1_CCIR_PCL               0x006b5810 /* PCL Registers for CCIR control singals */
#define BCHP_ITU656_DTG_1_DVI_PCL                0x006b5814 /* PCL Registers for DVI control signals */
#define BCHP_ITU656_DTG_1_CNTL_PCL               0x006b5818 /* PCL Registers for Misc. control singals */
#define BCHP_ITU656_DTG_1_RAM_ADDR               0x006b581c /* DTG Starting Address Register */
#define BCHP_ITU656_DTG_1_DTG_BVB_SIZE           0x006b5820 /* BVB Size Register. */
#define BCHP_ITU656_DTG_1_DTG_BVB_RSTATUS        0x006b5824 /* BVB status read Register. */
#define BCHP_ITU656_DTG_1_DTG_BVB_CSTATUS        0x006b5828 /* BVB status clear Register. */
#define BCHP_ITU656_DTG_1_DTG_TRIGGER_0          0x006b582c /* DTG Trigger Register 0 */
#define BCHP_ITU656_DTG_1_DTG_TRIGGER_1          0x006b5830 /* DTG Trigger Register 1 */
#define BCHP_ITU656_DTG_1_DTG_TRIGGER_2          0x006b5834 /* DTG Trigger Register 2 */
#define BCHP_ITU656_DTG_1_DTG_TRIGGER_3          0x006b5838 /* DTG Trigger Register 3 */
#define BCHP_ITU656_DTG_1_DTG_CTRL_STAT          0x006b583c /* DTG Control Bus Status Register */
#define BCHP_ITU656_DTG_1_DTG_MSYNC_CTRL         0x006b5840 /* Master Sync Control */
#define BCHP_ITU656_DTG_1_DTG_SSYNC_CTRL         0x006b5844 /* Slave Sync Control */
#define BCHP_ITU656_DTG_1_DTG_MS_TIMEOUT         0x006b5848 /* Master Slave Time Out register */
#define BCHP_ITU656_DTG_1_DTG_MSSYNC_START       0x006b584c /* Master Slave Sync Start */
#define BCHP_ITU656_DTG_1_DTG_MSSYNC_PCL         0x006b5850 /* Master Slave flag select PCL */
#define BCHP_ITU656_DTG_1_DTG_MSYNC_PHASE        0x006b5854 /* Master Sync Phase */
#define BCHP_ITU656_DTG_1_DTG_EOF0_LINE          0x006b5858 /* Field0 End line number for interlaced format */
#define BCHP_ITU656_DTG_1_DTG_MSSYNC_STATUS      0x006b585c /* "Status register for MSSYNC" */
#define BCHP_ITU656_DTG_1_DTG_AS_CONTROL         0x006b58f0 /* Active Space Control register */
#define BCHP_ITU656_DTG_1_DTG_AS_PIXEL_C0_C1     0x006b58f4 /* Active Space Pixel Value Register */
#define BCHP_ITU656_DTG_1_DTG_AS_PIXEL_C2        0x006b58f8 /* Active Space Pixel Value Register */
#define BCHP_ITU656_DTG_1_DTG_AS_LINE_NUMBER     0x006b58fc /* Active Space Pixel Value Register */
#define BCHP_ITU656_DTG_1_PARAM_PROG_DONE        0x006b5950 /* Parameter Programming Done Register */
#define BCHP_ITU656_DTG_1_MC_FRM_BOUNDARY        0x006b5954 /* DTG Param Load Register */
#define BCHP_ITU656_DTG_1_DTG_LCNTR              0x006b5970 /* DTG Line Counter Register */
#define BCHP_ITU656_DTG_1_DTG_L_CTRL             0x006b5974 /* DTG 3D L Control register */
#define BCHP_ITU656_DTG_1_DTG_R_CTRL             0x006b5978 /* DTG 3D R Control register */

/***************************************************************************
 *DTG_SSYNC_LINE%i - Slave Sync Phase Lines
 ***************************************************************************/
#define BCHP_ITU656_DTG_1_DTG_SSYNC_LINEi_ARRAY_BASE               0x006b5860
#define BCHP_ITU656_DTG_1_DTG_SSYNC_LINEi_ARRAY_START              0
#define BCHP_ITU656_DTG_1_DTG_SSYNC_LINEi_ARRAY_END                15
#define BCHP_ITU656_DTG_1_DTG_SSYNC_LINEi_ARRAY_ELEMENT_SIZE       32

/***************************************************************************
 *DTG_SSYNC_LINE%i - Slave Sync Phase Lines
 ***************************************************************************/
/* ITU656_DTG_1 :: DTG_SSYNC_LINEi :: reserved0 [31:11] */
#define BCHP_ITU656_DTG_1_DTG_SSYNC_LINEi_reserved0_MASK           0xfffff800
#define BCHP_ITU656_DTG_1_DTG_SSYNC_LINEi_reserved0_SHIFT          11

/* ITU656_DTG_1 :: DTG_SSYNC_LINEi :: COUNT [10:00] */
#define BCHP_ITU656_DTG_1_DTG_SSYNC_LINEi_COUNT_MASK               0x000007ff
#define BCHP_ITU656_DTG_1_DTG_SSYNC_LINEi_COUNT_SHIFT              0
#define BCHP_ITU656_DTG_1_DTG_SSYNC_LINEi_COUNT_DEFAULT            0x00000000


/***************************************************************************
 *DTG_SSYNC_PHASE%i - Slave Sync Phases
 ***************************************************************************/
#define BCHP_ITU656_DTG_1_DTG_SSYNC_PHASEi_ARRAY_BASE              0x006b58a0
#define BCHP_ITU656_DTG_1_DTG_SSYNC_PHASEi_ARRAY_START             0
#define BCHP_ITU656_DTG_1_DTG_SSYNC_PHASEi_ARRAY_END               15
#define BCHP_ITU656_DTG_1_DTG_SSYNC_PHASEi_ARRAY_ELEMENT_SIZE      32

/***************************************************************************
 *DTG_SSYNC_PHASE%i - Slave Sync Phases
 ***************************************************************************/
/* ITU656_DTG_1 :: DTG_SSYNC_PHASEi :: VALUE [31:00] */
#define BCHP_ITU656_DTG_1_DTG_SSYNC_PHASEi_VALUE_MASK              0xffffffff
#define BCHP_ITU656_DTG_1_DTG_SSYNC_PHASEi_VALUE_SHIFT             0
#define BCHP_ITU656_DTG_1_DTG_SSYNC_PHASEi_VALUE_DEFAULT           0x00000000


/***************************************************************************
 *DMC_IMM_VAL_%i - DTG immediate register for CALLI/SCOUNTI
 ***************************************************************************/
#define BCHP_ITU656_DTG_1_DMC_IMM_VAL_i_ARRAY_BASE                 0x006b5900
#define BCHP_ITU656_DTG_1_DMC_IMM_VAL_i_ARRAY_START                0
#define BCHP_ITU656_DTG_1_DMC_IMM_VAL_i_ARRAY_END                  15
#define BCHP_ITU656_DTG_1_DMC_IMM_VAL_i_ARRAY_ELEMENT_SIZE         32

/***************************************************************************
 *DMC_IMM_VAL_%i - DTG immediate register for CALLI/SCOUNTI
 ***************************************************************************/
/* ITU656_DTG_1 :: DMC_IMM_VAL_i :: reserved0 [31:24] */
#define BCHP_ITU656_DTG_1_DMC_IMM_VAL_i_reserved0_MASK             0xff000000
#define BCHP_ITU656_DTG_1_DMC_IMM_VAL_i_reserved0_SHIFT            24

/* ITU656_DTG_1 :: DMC_IMM_VAL_i :: ADDR [23:12] */
#define BCHP_ITU656_DTG_1_DMC_IMM_VAL_i_ADDR_MASK                  0x00fff000
#define BCHP_ITU656_DTG_1_DMC_IMM_VAL_i_ADDR_SHIFT                 12

/* ITU656_DTG_1 :: DMC_IMM_VAL_i :: COUNT [11:00] */
#define BCHP_ITU656_DTG_1_DMC_IMM_VAL_i_COUNT_MASK                 0x00000fff
#define BCHP_ITU656_DTG_1_DMC_IMM_VAL_i_COUNT_SHIFT                0


#endif /* #ifndef BCHP_ITU656_DTG_1_H__ */

/* End of File */