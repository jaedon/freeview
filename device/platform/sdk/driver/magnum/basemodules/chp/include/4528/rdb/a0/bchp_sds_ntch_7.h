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
 * $brcm_Workfile: bchp_sds_ntch_7.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 1/13/11 8:07a $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Wed Jan 12 18:41:14 2011
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
 * $brcm_Log: /magnum/basemodules/chp/4528/rdb/a0/bchp_sds_ntch_7.h $
 * 
 * Hydra_Software_Devel/1   1/13/11 8:07a pntruong
 * HW4528-2: Synced up with central rdb.
 *
 ***************************************************************************/

#ifndef BCHP_SDS_NTCH_7_H__
#define BCHP_SDS_NTCH_7_H__

/***************************************************************************
 *SDS_NTCH_7 - SDS Notch Filter Register Set 7
 ***************************************************************************/
#define BCHP_SDS_NTCH_7_CTRL                     0x000e85c0 /* Notch Filter Controb */
#define BCHP_SDS_NTCH_7_FCWADJ_SCL               0x000e85c4 /* Notch Filter FCW Adjustment Scale */
#define BCHP_SDS_NTCH_7_FCW0                     0x000e85c8 /* Notch Filter 0 DDFS Frequency Control Word */
#define BCHP_SDS_NTCH_7_FCW1                     0x000e85cc /* Notch Filter 1 DDFS Frequency Control Word */
#define BCHP_SDS_NTCH_7_FCW2                     0x000e85d0 /* Notch Filter 2 DDFS Frequency Control Word */
#define BCHP_SDS_NTCH_7_FCW3                     0x000e85d4 /* Notch Filter DDFS Combined Frequency Control Word */
#define BCHP_SDS_NTCH_7_DCO0_INT                 0x000e85d8 /* Notch Filter 0 BB DC Canceller Loop Integrator */
#define BCHP_SDS_NTCH_7_DCO1_INT                 0x000e85dc /* Notch Filter 1 BB DC Canceller Loop Integrator */
#define BCHP_SDS_NTCH_7_DCO2_INT                 0x000e85e0 /* Notch Filter 2 BB DC Canceller Loop Integrator */

#endif /* #ifndef BCHP_SDS_NTCH_7_H__ */

/* End of File */