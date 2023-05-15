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
 * $brcm_Workfile: bchp_int_id_moca_l2.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 1/17/12 3:25p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Tue Jan 17 13:31:35 2012
 *                 MD5 Checksum         d41d8cd98f00b204e9800998ecf8427e
 *
 * Compiled with:  RDB Utility          unknown
 *                 RDB Parser           3.0
 *                 generate_int_id.pl   1.0
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7425/rdb/b2/bchp_int_id_moca_l2.h $
 * 
 * Hydra_Software_Devel/1   1/17/12 3:25p vanessah
 * SW7425-2202: add B2 headers
 *
 ***************************************************************************/

#include "bchp.h"
#include "bchp_moca_l2.h"

#ifndef BCHP_INT_ID_MOCA_L2_H__
#define BCHP_INT_ID_MOCA_L2_H__

#define BCHP_INT_ID_END_HANDOFF_INTR          BCHP_INT_ID_CREATE(BCHP_MOCA_L2_CPU_STATUS, BCHP_MOCA_L2_CPU_STATUS_END_HANDOFF_INTR_SHIFT)
#define BCHP_INT_ID_GENERAL_PURPOSE0_INTR     BCHP_INT_ID_CREATE(BCHP_MOCA_L2_CPU_STATUS, BCHP_MOCA_L2_CPU_STATUS_GENERAL_PURPOSE0_INTR_SHIFT)
#define BCHP_INT_ID_GENERAL_PURPOSE10_INTR    BCHP_INT_ID_CREATE(BCHP_MOCA_L2_CPU_STATUS, BCHP_MOCA_L2_CPU_STATUS_GENERAL_PURPOSE10_INTR_SHIFT)
#define BCHP_INT_ID_GENERAL_PURPOSE11_INTR    BCHP_INT_ID_CREATE(BCHP_MOCA_L2_CPU_STATUS, BCHP_MOCA_L2_CPU_STATUS_GENERAL_PURPOSE11_INTR_SHIFT)
#define BCHP_INT_ID_GENERAL_PURPOSE12_INTR    BCHP_INT_ID_CREATE(BCHP_MOCA_L2_CPU_STATUS, BCHP_MOCA_L2_CPU_STATUS_GENERAL_PURPOSE12_INTR_SHIFT)
#define BCHP_INT_ID_GENERAL_PURPOSE13_INTR    BCHP_INT_ID_CREATE(BCHP_MOCA_L2_CPU_STATUS, BCHP_MOCA_L2_CPU_STATUS_GENERAL_PURPOSE13_INTR_SHIFT)
#define BCHP_INT_ID_GENERAL_PURPOSE14_INTR    BCHP_INT_ID_CREATE(BCHP_MOCA_L2_CPU_STATUS, BCHP_MOCA_L2_CPU_STATUS_GENERAL_PURPOSE14_INTR_SHIFT)
#define BCHP_INT_ID_GENERAL_PURPOSE15_INTR    BCHP_INT_ID_CREATE(BCHP_MOCA_L2_CPU_STATUS, BCHP_MOCA_L2_CPU_STATUS_GENERAL_PURPOSE15_INTR_SHIFT)
#define BCHP_INT_ID_GENERAL_PURPOSE1_INTR     BCHP_INT_ID_CREATE(BCHP_MOCA_L2_CPU_STATUS, BCHP_MOCA_L2_CPU_STATUS_GENERAL_PURPOSE1_INTR_SHIFT)
#define BCHP_INT_ID_GENERAL_PURPOSE2_INTR     BCHP_INT_ID_CREATE(BCHP_MOCA_L2_CPU_STATUS, BCHP_MOCA_L2_CPU_STATUS_GENERAL_PURPOSE2_INTR_SHIFT)
#define BCHP_INT_ID_GENERAL_PURPOSE3_INTR     BCHP_INT_ID_CREATE(BCHP_MOCA_L2_CPU_STATUS, BCHP_MOCA_L2_CPU_STATUS_GENERAL_PURPOSE3_INTR_SHIFT)
#define BCHP_INT_ID_GENERAL_PURPOSE4_INTR     BCHP_INT_ID_CREATE(BCHP_MOCA_L2_CPU_STATUS, BCHP_MOCA_L2_CPU_STATUS_GENERAL_PURPOSE4_INTR_SHIFT)
#define BCHP_INT_ID_GENERAL_PURPOSE5_INTR     BCHP_INT_ID_CREATE(BCHP_MOCA_L2_CPU_STATUS, BCHP_MOCA_L2_CPU_STATUS_GENERAL_PURPOSE5_INTR_SHIFT)
#define BCHP_INT_ID_GENERAL_PURPOSE6_INTR     BCHP_INT_ID_CREATE(BCHP_MOCA_L2_CPU_STATUS, BCHP_MOCA_L2_CPU_STATUS_GENERAL_PURPOSE6_INTR_SHIFT)
#define BCHP_INT_ID_GENERAL_PURPOSE7_INTR     BCHP_INT_ID_CREATE(BCHP_MOCA_L2_CPU_STATUS, BCHP_MOCA_L2_CPU_STATUS_GENERAL_PURPOSE7_INTR_SHIFT)
#define BCHP_INT_ID_GENERAL_PURPOSE8_INTR     BCHP_INT_ID_CREATE(BCHP_MOCA_L2_CPU_STATUS, BCHP_MOCA_L2_CPU_STATUS_GENERAL_PURPOSE8_INTR_SHIFT)
#define BCHP_INT_ID_GENERAL_PURPOSE9_INTR     BCHP_INT_ID_CREATE(BCHP_MOCA_L2_CPU_STATUS, BCHP_MOCA_L2_CPU_STATUS_GENERAL_PURPOSE9_INTR_SHIFT)
#define BCHP_INT_ID_GR_BRIDGE_ERR_INTR        BCHP_INT_ID_CREATE(BCHP_MOCA_L2_CPU_STATUS, BCHP_MOCA_L2_CPU_STATUS_GR_BRIDGE_ERR_INTR_SHIFT)
#define BCHP_INT_ID_M2M_DONE_INTR             BCHP_INT_ID_CREATE(BCHP_MOCA_L2_CPU_STATUS, BCHP_MOCA_L2_CPU_STATUS_M2M_DONE_INTR_SHIFT)
#define BCHP_INT_ID_PM_MOCA_EXCEPTION_INTR    BCHP_INT_ID_CREATE(BCHP_MOCA_L2_CPU_STATUS, BCHP_MOCA_L2_CPU_STATUS_PM_MOCA_EXCEPTION_INTR_SHIFT)
#define BCHP_INT_ID_START_HANDOFF_INTR        BCHP_INT_ID_CREATE(BCHP_MOCA_L2_CPU_STATUS, BCHP_MOCA_L2_CPU_STATUS_START_HANDOFF_INTR_SHIFT)
#define BCHP_INT_ID_WATCHDOG_CPU_HIGH_INTR    BCHP_INT_ID_CREATE(BCHP_MOCA_L2_CPU_STATUS, BCHP_MOCA_L2_CPU_STATUS_WATCHDOG_CPU_HIGH_INTR_SHIFT)
#define BCHP_INT_ID_WATCHDOG_CPU_LOW_INTR     BCHP_INT_ID_CREATE(BCHP_MOCA_L2_CPU_STATUS, BCHP_MOCA_L2_CPU_STATUS_WATCHDOG_CPU_LOW_INTR_SHIFT)

#endif /* #ifndef BCHP_INT_ID_MOCA_L2_H__ */

/* End of File */