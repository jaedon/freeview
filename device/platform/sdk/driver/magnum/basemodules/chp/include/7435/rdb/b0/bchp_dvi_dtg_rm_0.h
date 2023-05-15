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
 * $brcm_Workfile: bchp_dvi_dtg_rm_0.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 2/28/12 4:20p $
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
 * $brcm_Log: /magnum/basemodules/chp/7435/rdb/b0/bchp_dvi_dtg_rm_0.h $
 * 
 * Hydra_Software_Devel/1   2/28/12 4:20p tdo
 * SW7435-40: Need 7435B0 public/central RDB (magnum) headers checked into
 * clearcase
 *
 ***************************************************************************/

#ifndef BCHP_DVI_DTG_RM_0_H__
#define BCHP_DVI_DTG_RM_0_H__

/***************************************************************************
 *DVI_DTG_RM_0 - DVI DTG Rate Manager 0
 ***************************************************************************/
#define BCHP_DVI_DTG_RM_0_CONTROL                0x006b4800 /* Rate Manager Controls */
#define BCHP_DVI_DTG_RM_0_RATE_RATIO             0x006b4804 /* Rate Manager Output Rate Setting I */
#define BCHP_DVI_DTG_RM_0_SAMPLE_INC             0x006b4808 /* Rate Manager Output Rate Setting II */
#define BCHP_DVI_DTG_RM_0_PHASE_INC              0x006b480c /* Rate Manager NCO Phase Increment */
#define BCHP_DVI_DTG_RM_0_INTEGRATOR             0x006b4810 /* Loop Filter Integrator Value */
#define BCHP_DVI_DTG_RM_0_SKIP_REPEAT_CONTROL    0x006b4814 /* Time Base Skip or Repeat Control Register */
#define BCHP_DVI_DTG_RM_0_SKIP_REPEAT_GAP        0x006b4818 /* Time Base Skip or Repeat Gap Count Register */
#define BCHP_DVI_DTG_RM_0_SKIP_REPEAT_NUMBER     0x006b481c /* Time Base Skip or Repeat Count Register */
#define BCHP_DVI_DTG_RM_0_STATUS                 0x006b4820 /* Rate Manager Status Register */
#define BCHP_DVI_DTG_RM_0_STATUS_CLEAR           0x006b4824 /* Rate Manager Status Clear Register */

#endif /* #ifndef BCHP_DVI_DTG_RM_0_H__ */

/* End of File */