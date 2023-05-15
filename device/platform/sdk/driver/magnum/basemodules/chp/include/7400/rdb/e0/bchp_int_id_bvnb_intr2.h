/***************************************************************************
 *     Copyright (c) 1999-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bchp_int_id_bvnb_intr2.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 1/9/08 10:52a $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Wed Jan  9 09:07:43 2008
 *                 MD5 Checksum         847dc12a9d71c4c68a648bbf19a883e3
 *
 * Compiled with:  RDB Utility          unknown
 *                 RDB Parser           3.0
 *                 generate_int_id.pl   1.0
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7400/rdb/e0/bchp_int_id_bvnb_intr2.h $
 * 
 * Hydra_Software_Devel/1   1/9/08 10:52a rpan
 * PR38572: Initial revision.
 *
 ***************************************************************************/

#include "bchp.h"
#include "bchp_bvnb_intr2.h"

#ifndef BCHP_INT_ID_BVNB_INTR2_H__
#define BCHP_INT_ID_BVNB_INTR2_H__

#define BCHP_INT_ID_CAP0_INTR                 BCHP_INT_ID_CREATE(BCHP_BVNB_INTR2_CPU_STATUS, BCHP_BVNB_INTR2_CPU_STATUS_CAP0_INTR_SHIFT)
#define BCHP_INT_ID_CAP1_INTR                 BCHP_INT_ID_CREATE(BCHP_BVNB_INTR2_CPU_STATUS, BCHP_BVNB_INTR2_CPU_STATUS_CAP1_INTR_SHIFT)
#define BCHP_INT_ID_CAP2_INTR                 BCHP_INT_ID_CREATE(BCHP_BVNB_INTR2_CPU_STATUS, BCHP_BVNB_INTR2_CPU_STATUS_CAP2_INTR_SHIFT)
#define BCHP_INT_ID_CAP3_INTR                 BCHP_INT_ID_CREATE(BCHP_BVNB_INTR2_CPU_STATUS, BCHP_BVNB_INTR2_CPU_STATUS_CAP3_INTR_SHIFT)
#define BCHP_INT_ID_CMP0_G0_INTR              BCHP_INT_ID_CREATE(BCHP_BVNB_INTR2_CPU_STATUS, BCHP_BVNB_INTR2_CPU_STATUS_CMP0_G0_INTR_SHIFT)
#define BCHP_INT_ID_CMP0_V0_INTR              BCHP_INT_ID_CREATE(BCHP_BVNB_INTR2_CPU_STATUS, BCHP_BVNB_INTR2_CPU_STATUS_CMP0_V0_INTR_SHIFT)
#define BCHP_INT_ID_CMP0_V1_INTR              BCHP_INT_ID_CREATE(BCHP_BVNB_INTR2_CPU_STATUS, BCHP_BVNB_INTR2_CPU_STATUS_CMP0_V1_INTR_SHIFT)
#define BCHP_INT_ID_CMP1_G0_INTR              BCHP_INT_ID_CREATE(BCHP_BVNB_INTR2_CPU_STATUS, BCHP_BVNB_INTR2_CPU_STATUS_CMP1_G0_INTR_SHIFT)
#define BCHP_INT_ID_CMP1_V0_INTR              BCHP_INT_ID_CREATE(BCHP_BVNB_INTR2_CPU_STATUS, BCHP_BVNB_INTR2_CPU_STATUS_CMP1_V0_INTR_SHIFT)
#define BCHP_INT_ID_CMP1_V1_INTR              BCHP_INT_ID_CREATE(BCHP_BVNB_INTR2_CPU_STATUS, BCHP_BVNB_INTR2_CPU_STATUS_CMP1_V1_INTR_SHIFT)
#define BCHP_INT_ID_CMP2_G0_INTR              BCHP_INT_ID_CREATE(BCHP_BVNB_INTR2_CPU_STATUS, BCHP_BVNB_INTR2_CPU_STATUS_CMP2_G0_INTR_SHIFT)
#define BCHP_INT_ID_CMP2_V0_INTR              BCHP_INT_ID_CREATE(BCHP_BVNB_INTR2_CPU_STATUS, BCHP_BVNB_INTR2_CPU_STATUS_CMP2_V0_INTR_SHIFT)
#define BCHP_INT_ID_GFD0_INTR                 BCHP_INT_ID_CREATE(BCHP_BVNB_INTR2_CPU_STATUS, BCHP_BVNB_INTR2_CPU_STATUS_GFD0_INTR_SHIFT)
#define BCHP_INT_ID_GFD1_INTR                 BCHP_INT_ID_CREATE(BCHP_BVNB_INTR2_CPU_STATUS, BCHP_BVNB_INTR2_CPU_STATUS_GFD1_INTR_SHIFT)
#define BCHP_INT_ID_GFD2_INTR                 BCHP_INT_ID_CREATE(BCHP_BVNB_INTR2_CPU_STATUS, BCHP_BVNB_INTR2_CPU_STATUS_GFD2_INTR_SHIFT)
#define BCHP_INT_ID_LAB_HISTO_DONE_INTR       BCHP_INT_ID_CREATE(BCHP_BVNB_INTR2_CPU_STATUS, BCHP_BVNB_INTR2_CPU_STATUS_LAB_HISTO_DONE_INTR_SHIFT)
#define BCHP_INT_ID_LAB_MIN_MAX_DONE_INTR     BCHP_INT_ID_CREATE(BCHP_BVNB_INTR2_CPU_STATUS, BCHP_BVNB_INTR2_CPU_STATUS_LAB_MIN_MAX_DONE_INTR_SHIFT)

#endif /* #ifndef BCHP_INT_ID_BVNB_INTR2_H__ */

/* End of File */