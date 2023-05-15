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
 * $brcm_Workfile: bchp_raaga_dsp_rgr.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 4/13/11 6:44p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Wed Apr 13 13:26:35 2011
 *                 MD5 Checksum         5014fc6b805cdf8eed48fe0da9f96997
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7231/rdb/b0/bchp_raaga_dsp_rgr.h $
 * 
 * Hydra_Software_Devel/1   4/13/11 6:44p albertl
 * SW7231-123: Initial revision.
 *
 ***************************************************************************/

#ifndef BCHP_RAAGA_DSP_RGR_H__
#define BCHP_RAAGA_DSP_RGR_H__

/***************************************************************************
 *RAAGA_DSP_RGR - Raaga DSP RGR Bridge Registers
 ***************************************************************************/
#define BCHP_RAAGA_DSP_RGR_REVISION              0x00800000 /* RGR Bridge Revision */
#define BCHP_RAAGA_DSP_RGR_CTRL                  0x00800004 /* RGR Bridge Control Register */
#define BCHP_RAAGA_DSP_RGR_RBUS_TIMER            0x00800008 /* RGR Bridge RBUS Timer Register */

/***************************************************************************
 *REVISION - RGR Bridge Revision
 ***************************************************************************/
/* RAAGA_DSP_RGR :: REVISION :: reserved0 [31:16] */
#define BCHP_RAAGA_DSP_RGR_REVISION_reserved0_MASK                 0xffff0000
#define BCHP_RAAGA_DSP_RGR_REVISION_reserved0_SHIFT                16

/* RAAGA_DSP_RGR :: REVISION :: MAJOR [15:08] */
#define BCHP_RAAGA_DSP_RGR_REVISION_MAJOR_MASK                     0x0000ff00
#define BCHP_RAAGA_DSP_RGR_REVISION_MAJOR_SHIFT                    8
#define BCHP_RAAGA_DSP_RGR_REVISION_MAJOR_DEFAULT                  2

/* RAAGA_DSP_RGR :: REVISION :: MINOR [07:00] */
#define BCHP_RAAGA_DSP_RGR_REVISION_MINOR_MASK                     0x000000ff
#define BCHP_RAAGA_DSP_RGR_REVISION_MINOR_SHIFT                    0
#define BCHP_RAAGA_DSP_RGR_REVISION_MINOR_DEFAULT                  0

/***************************************************************************
 *CTRL - RGR Bridge Control Register
 ***************************************************************************/
/* RAAGA_DSP_RGR :: CTRL :: reserved0 [31:02] */
#define BCHP_RAAGA_DSP_RGR_CTRL_reserved0_MASK                     0xfffffffc
#define BCHP_RAAGA_DSP_RGR_CTRL_reserved0_SHIFT                    2

/* RAAGA_DSP_RGR :: CTRL :: rbus_error_intr [01:01] */
#define BCHP_RAAGA_DSP_RGR_CTRL_rbus_error_intr_MASK               0x00000002
#define BCHP_RAAGA_DSP_RGR_CTRL_rbus_error_intr_SHIFT              1
#define BCHP_RAAGA_DSP_RGR_CTRL_rbus_error_intr_DEFAULT            0
#define BCHP_RAAGA_DSP_RGR_CTRL_rbus_error_intr_INTR_DISABLE       0
#define BCHP_RAAGA_DSP_RGR_CTRL_rbus_error_intr_INTR_ENABLE        1

/* RAAGA_DSP_RGR :: CTRL :: gisb_error_intr [00:00] */
#define BCHP_RAAGA_DSP_RGR_CTRL_gisb_error_intr_MASK               0x00000001
#define BCHP_RAAGA_DSP_RGR_CTRL_gisb_error_intr_SHIFT              0
#define BCHP_RAAGA_DSP_RGR_CTRL_gisb_error_intr_DEFAULT            0
#define BCHP_RAAGA_DSP_RGR_CTRL_gisb_error_intr_INTR_DISABLE       0
#define BCHP_RAAGA_DSP_RGR_CTRL_gisb_error_intr_INTR_ENABLE        1

/***************************************************************************
 *RBUS_TIMER - RGR Bridge RBUS Timer Register
 ***************************************************************************/
/* RAAGA_DSP_RGR :: RBUS_TIMER :: reserved0 [31:16] */
#define BCHP_RAAGA_DSP_RGR_RBUS_TIMER_reserved0_MASK               0xffff0000
#define BCHP_RAAGA_DSP_RGR_RBUS_TIMER_reserved0_SHIFT              16

/* RAAGA_DSP_RGR :: RBUS_TIMER :: timer_value [15:00] */
#define BCHP_RAAGA_DSP_RGR_RBUS_TIMER_timer_value_MASK             0x0000ffff
#define BCHP_RAAGA_DSP_RGR_RBUS_TIMER_timer_value_SHIFT            0
#define BCHP_RAAGA_DSP_RGR_RBUS_TIMER_timer_value_DEFAULT          65535

#endif /* #ifndef BCHP_RAAGA_DSP_RGR_H__ */

/* End of File */