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
 * $brcm_Workfile: bchp_int_id_xpt_dpcr7.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 10/11/11 7:24p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Tue Oct 11 11:45:27 2011
 *                 MD5 Checksum         12f5dd38e0ec050efd2afb0b1c62e9d9
 *
 * Compiled with:  RDB Utility          unknown
 *                 RDB Parser           3.0
 *                 generate_int_id.pl   1.0
 *                 Perl Interpreter     5.008005
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7435/rdb/a0/bchp_int_id_xpt_dpcr7.h $
 * 
 * Hydra_Software_Devel/1   10/11/11 7:24p pntruong
 * SW7435-3: Synced up with central rdb.
 *
 ***************************************************************************/

#include "bchp.h"
#include "bchp_xpt_dpcr0.h"
#include "bchp_xpt_dpcr7.h"

#ifndef BCHP_INT_ID_XPT_DPCR7_H__
#define BCHP_INT_ID_XPT_DPCR7_H__

#define BCHP_INT_ID_XPT_DPCR7_DISCONTINUITY_INT BCHP_INT_ID_CREATE(BCHP_XPT_DPCR7_INTR_STATUS_REG, BCHP_XPT_DPCR0_INTR_STATUS_REG_DISCONTINUITY_INT_SHIFT)
#define BCHP_INT_ID_XPT_DPCR7_LOAD_INT        BCHP_INT_ID_CREATE(BCHP_XPT_DPCR7_INTR_STATUS_REG, BCHP_XPT_DPCR0_INTR_STATUS_REG_LOAD_INT_SHIFT)
#define BCHP_INT_ID_XPT_DPCR7_ONE_PCR_ERROR   BCHP_INT_ID_CREATE(BCHP_XPT_DPCR7_INTR_STATUS_REG, BCHP_XPT_DPCR0_INTR_STATUS_REG_ONE_PCR_ERROR_SHIFT)
#define BCHP_INT_ID_XPT_DPCR7_PCR_CNT_WRAP    BCHP_INT_ID_CREATE(BCHP_XPT_DPCR7_INTR_STATUS_REG, BCHP_XPT_DPCR0_INTR_STATUS_REG_PCR_CNT_WRAP_SHIFT)
#define BCHP_INT_ID_XPT_DPCR7_PHASE_CMP_INT   BCHP_INT_ID_CREATE(BCHP_XPT_DPCR7_INTR_STATUS_REG, BCHP_XPT_DPCR0_INTR_STATUS_REG_PHASE_CMP_INT_SHIFT)
#define BCHP_INT_ID_XPT_DPCR7_PHASE_SAT_INT   BCHP_INT_ID_CREATE(BCHP_XPT_DPCR7_INTR_STATUS_REG, BCHP_XPT_DPCR0_INTR_STATUS_REG_PHASE_SAT_INT_SHIFT)
#define BCHP_INT_ID_XPT_DPCR7_TWO_PCR_ERROR   BCHP_INT_ID_CREATE(BCHP_XPT_DPCR7_INTR_STATUS_REG, BCHP_XPT_DPCR0_INTR_STATUS_REG_TWO_PCR_ERROR_SHIFT)

#endif /* #ifndef BCHP_INT_ID_XPT_DPCR7_H__ */

/* End of File */