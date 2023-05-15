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
 * $brcm_Workfile: bchp_int_id_thd_intr2.h $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 5/18/11 4:20p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Tue May 17 04:43:30 2011
 *                 MD5 Checksum         f5f09b2bf7ad40890d2e5dc57d4789b6
 *
 * Compiled with:  RDB Utility          unknown
 *                 RDB Parser           3.0
 *                 generate_int_id.pl   1.0
 *                 Perl Interpreter     5.008005
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7552/rdb/a0/bchp_int_id_thd_intr2.h $
 * 
 * Hydra_Software_Devel/3   5/18/11 4:20p xhuang
 * SW7552-2: update with central RDB
 *
 ***************************************************************************/

#include "bchp.h"
#include "bchp_thd_intr2.h"

#ifndef BCHP_INT_ID_THD_INTR2_H__
#define BCHP_INT_ID_THD_INTR2_H__

#define BCHP_INT_ID_FBCNT2_ZERO_INTR          BCHP_INT_ID_CREATE(BCHP_THD_INTR2_CPU_STATUS, BCHP_THD_INTR2_CPU_STATUS_FBCNT2_ZERO_INTR_SHIFT)
#define BCHP_INT_ID_FBCNT3_ZERO_INTR          BCHP_INT_ID_CREATE(BCHP_THD_INTR2_CPU_STATUS, BCHP_THD_INTR2_CPU_STATUS_FBCNT3_ZERO_INTR_SHIFT)
#define BCHP_INT_ID_FBCNT_ZERO_INTR           BCHP_INT_ID_CREATE(BCHP_THD_INTR2_CPU_STATUS, BCHP_THD_INTR2_CPU_STATUS_FBCNT_ZERO_INTR_SHIFT)
#define BCHP_INT_ID_FEC_SYNC_INTR             BCHP_INT_ID_CREATE(BCHP_THD_INTR2_CPU_STATUS, BCHP_THD_INTR2_CPU_STATUS_FEC_SYNC_INTR_SHIFT)
#define BCHP_INT_ID_FEC_SYNC_LOSS_INTR        BCHP_INT_ID_CREATE(BCHP_THD_INTR2_CPU_STATUS, BCHP_THD_INTR2_CPU_STATUS_FEC_SYNC_LOSS_INTR_SHIFT)
#define BCHP_INT_ID_FSCNT2_ZERO_INTR          BCHP_INT_ID_CREATE(BCHP_THD_INTR2_CPU_STATUS, BCHP_THD_INTR2_CPU_STATUS_FSCNT2_ZERO_INTR_SHIFT)
#define BCHP_INT_ID_FSCNT3_ZERO_INTR          BCHP_INT_ID_CREATE(BCHP_THD_INTR2_CPU_STATUS, BCHP_THD_INTR2_CPU_STATUS_FSCNT3_ZERO_INTR_SHIFT)
#define BCHP_INT_ID_FSCNT_ZERO_INTR           BCHP_INT_ID_CREATE(BCHP_THD_INTR2_CPU_STATUS, BCHP_THD_INTR2_CPU_STATUS_FSCNT_ZERO_INTR_SHIFT)
#define BCHP_INT_ID_FW_CORR_MAX_RDY_INTR      BCHP_INT_ID_CREATE(BCHP_THD_INTR2_CPU_STATUS, BCHP_THD_INTR2_CPU_STATUS_FW_CORR_MAX_RDY_INTR_SHIFT)
#define BCHP_INT_ID_FW_SYNC_INTR              BCHP_INT_ID_CREATE(BCHP_THD_INTR2_CPU_STATUS, BCHP_THD_INTR2_CPU_STATUS_FW_SYNC_INTR_SHIFT)
#define BCHP_INT_ID_GUARD_START_INTR          BCHP_INT_ID_CREATE(BCHP_THD_INTR2_CPU_STATUS, BCHP_THD_INTR2_CPU_STATUS_GUARD_START_INTR_SHIFT)
#define BCHP_INT_ID_NCH_HI_INTR               BCHP_INT_ID_CREATE(BCHP_THD_INTR2_CPU_STATUS, BCHP_THD_INTR2_CPU_STATUS_NCH_HI_INTR_SHIFT)
#define BCHP_INT_ID_NCH_LO_INTR               BCHP_INT_ID_CREATE(BCHP_THD_INTR2_CPU_STATUS, BCHP_THD_INTR2_CPU_STATUS_NCH_LO_INTR_SHIFT)
#define BCHP_INT_ID_SP_SYNC_INTR              BCHP_INT_ID_CREATE(BCHP_THD_INTR2_CPU_STATUS, BCHP_THD_INTR2_CPU_STATUS_SP_SYNC_INTR_SHIFT)
#define BCHP_INT_ID_SP_SYNC_LOSS_INTR         BCHP_INT_ID_CREATE(BCHP_THD_INTR2_CPU_STATUS, BCHP_THD_INTR2_CPU_STATUS_SP_SYNC_LOSS_INTR_SHIFT)
#define BCHP_INT_ID_THD_INTR2_GISB_ERROR_INTR BCHP_INT_ID_CREATE(BCHP_THD_INTR2_CPU_STATUS, BCHP_THD_INTR2_CPU_STATUS_GISB_ERROR_INTR_SHIFT)
#define BCHP_INT_ID_TMCC_CHANGE_INTR          BCHP_INT_ID_CREATE(BCHP_THD_INTR2_CPU_STATUS, BCHP_THD_INTR2_CPU_STATUS_TMCC_CHANGE_INTR_SHIFT)
#define BCHP_INT_ID_TMCC_SYNC_INTR            BCHP_INT_ID_CREATE(BCHP_THD_INTR2_CPU_STATUS, BCHP_THD_INTR2_CPU_STATUS_TMCC_SYNC_INTR_SHIFT)
#define BCHP_INT_ID_TMCC_SYNC_LOSS_INTR       BCHP_INT_ID_CREATE(BCHP_THD_INTR2_CPU_STATUS, BCHP_THD_INTR2_CPU_STATUS_TMCC_SYNC_LOSS_INTR_SHIFT)
#define BCHP_INT_ID_TPS_CHANGE_INTR           BCHP_INT_ID_CREATE(BCHP_THD_INTR2_CPU_STATUS, BCHP_THD_INTR2_CPU_STATUS_TPS_CHANGE_INTR_SHIFT)
#define BCHP_INT_ID_TPS_SYNC_INTR             BCHP_INT_ID_CREATE(BCHP_THD_INTR2_CPU_STATUS, BCHP_THD_INTR2_CPU_STATUS_TPS_SYNC_INTR_SHIFT)
#define BCHP_INT_ID_TPS_SYNC_LOSS_INTR        BCHP_INT_ID_CREATE(BCHP_THD_INTR2_CPU_STATUS, BCHP_THD_INTR2_CPU_STATUS_TPS_SYNC_LOSS_INTR_SHIFT)
#define BCHP_INT_ID_TS_FULL_INTR              BCHP_INT_ID_CREATE(BCHP_THD_INTR2_CPU_STATUS, BCHP_THD_INTR2_CPU_STATUS_TS_FULL_INTR_SHIFT)
#define BCHP_INT_ID_TS_HALFFULL_INTR          BCHP_INT_ID_CREATE(BCHP_THD_INTR2_CPU_STATUS, BCHP_THD_INTR2_CPU_STATUS_TS_HALFFULL_INTR_SHIFT)
#define BCHP_INT_ID_TS_ONEPKT_INTR            BCHP_INT_ID_CREATE(BCHP_THD_INTR2_CPU_STATUS, BCHP_THD_INTR2_CPU_STATUS_TS_ONEPKT_INTR_SHIFT)
#define BCHP_INT_ID_TS_ONEWORD_INTR           BCHP_INT_ID_CREATE(BCHP_THD_INTR2_CPU_STATUS, BCHP_THD_INTR2_CPU_STATUS_TS_ONEWORD_INTR_SHIFT)
#define BCHP_INT_ID_VIT_SYNC_INTR             BCHP_INT_ID_CREATE(BCHP_THD_INTR2_CPU_STATUS, BCHP_THD_INTR2_CPU_STATUS_VIT_SYNC_INTR_SHIFT)
#define BCHP_INT_ID_VIT_SYNC_LOSS_INTR        BCHP_INT_ID_CREATE(BCHP_THD_INTR2_CPU_STATUS, BCHP_THD_INTR2_CPU_STATUS_VIT_SYNC_LOSS_INTR_SHIFT)

#endif /* #ifndef BCHP_INT_ID_THD_INTR2_H__ */

/* End of File */