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
 * $brcm_Workfile: bchp_int_id_aon_pm_l2.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 8/17/11 1:51p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Thu Aug 11 22:09:50 2011
 *                 MD5 Checksum         30eb4d41dba6899590ea7664187e6ba5
 *
 * Compiled with:  RDB Utility          unknown
 *                 RDB Parser           3.0
 *                 generate_int_id.pl   1.0
 *                 Perl Interpreter     5.008005
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7429/rdb/a0/bchp_int_id_aon_pm_l2.h $
 * 
 * Hydra_Software_Devel/1   8/17/11 1:51p pntruong
 * SW7429-2: Initial version.
 *
 ***************************************************************************/

#include "bchp.h"
#include "bchp_aon_pm_l2.h"

#ifndef BCHP_INT_ID_AON_PM_L2_H__
#define BCHP_INT_ID_AON_PM_L2_H__

#define BCHP_INT_ID_AON_PM_L2_MOCA            BCHP_INT_ID_CREATE(BCHP_AON_PM_L2_CPU_STATUS, BCHP_AON_PM_L2_CPU_STATUS_MOCA_SHIFT)
#define BCHP_INT_ID_BOUNDARY_SCAN_REQ         BCHP_INT_ID_CREATE(BCHP_AON_PM_L2_CPU_STATUS, BCHP_AON_PM_L2_CPU_STATUS_BOUNDARY_SCAN_REQ_SHIFT)
#define BCHP_INT_ID_CEC_INTR                  BCHP_INT_ID_CREATE(BCHP_AON_PM_L2_CPU_STATUS, BCHP_AON_PM_L2_CPU_STATUS_CEC_INTR_SHIFT)
#define BCHP_INT_ID_FP_RESET                  BCHP_INT_ID_CREATE(BCHP_AON_PM_L2_CPU_STATUS, BCHP_AON_PM_L2_CPU_STATUS_FP_RESET_SHIFT)
#define BCHP_INT_ID_GPIO                      BCHP_INT_ID_CREATE(BCHP_AON_PM_L2_CPU_STATUS, BCHP_AON_PM_L2_CPU_STATUS_GPIO_SHIFT)
#define BCHP_INT_ID_IRR_INTR                  BCHP_INT_ID_CREATE(BCHP_AON_PM_L2_CPU_STATUS, BCHP_AON_PM_L2_CPU_STATUS_IRR_INTR_SHIFT)
#define BCHP_INT_ID_KPD_INTR                  BCHP_INT_ID_CREATE(BCHP_AON_PM_L2_CPU_STATUS, BCHP_AON_PM_L2_CPU_STATUS_KPD_INTR_SHIFT)
#define BCHP_INT_ID_NMI_B_INTR                BCHP_INT_ID_CREATE(BCHP_AON_PM_L2_CPU_STATUS, BCHP_AON_PM_L2_CPU_STATUS_NMI_B_INTR_SHIFT)
#define BCHP_INT_ID_SPARE_WAKEUP_EVENT_0      BCHP_INT_ID_CREATE(BCHP_AON_PM_L2_CPU_STATUS, BCHP_AON_PM_L2_CPU_STATUS_SPARE_WAKEUP_EVENT_0_SHIFT)
#define BCHP_INT_ID_TIMER_INTR                BCHP_INT_ID_CREATE(BCHP_AON_PM_L2_CPU_STATUS, BCHP_AON_PM_L2_CPU_STATUS_TIMER_INTR_SHIFT)
#define BCHP_INT_ID_USB0                      BCHP_INT_ID_CREATE(BCHP_AON_PM_L2_CPU_STATUS, BCHP_AON_PM_L2_CPU_STATUS_USB0_SHIFT)
#define BCHP_INT_ID_USB1                      BCHP_INT_ID_CREATE(BCHP_AON_PM_L2_CPU_STATUS, BCHP_AON_PM_L2_CPU_STATUS_USB1_SHIFT)
#define BCHP_INT_ID_WOL_ENET                  BCHP_INT_ID_CREATE(BCHP_AON_PM_L2_CPU_STATUS, BCHP_AON_PM_L2_CPU_STATUS_WOL_ENET_SHIFT)
#define BCHP_INT_ID_WOL_MOCA                  BCHP_INT_ID_CREATE(BCHP_AON_PM_L2_CPU_STATUS, BCHP_AON_PM_L2_CPU_STATUS_WOL_MOCA_SHIFT)
#define BCHP_INT_ID_XPT_PMU                   BCHP_INT_ID_CREATE(BCHP_AON_PM_L2_CPU_STATUS, BCHP_AON_PM_L2_CPU_STATUS_XPT_PMU_SHIFT)

#endif /* #ifndef BCHP_INT_ID_AON_PM_L2_H__ */

/* End of File */