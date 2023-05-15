/***************************************************************************
 *     Copyright (c) 1999-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bchp_int_id_bsp_control_intr2.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 8/25/10 12:59p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Tue Aug 24 17:59:13 2010
 *                 MD5 Checksum         39761c6e4b9d69cf41e7412c1f6df022
 *
 * Compiled with:  RDB Utility          unknown
 *                 RDB Parser           3.0
 *                 generate_int_id.pl   1.0
 *                 Perl Interpreter     5.008005
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7231/rdb/a0/bchp_int_id_bsp_control_intr2.h $
 * 
 * Hydra_Software_Devel/1   8/25/10 12:59p albertl
 * HW7231-39: Initial revision.
 *
 ***************************************************************************/

#include "bchp.h"
#include "bchp_bsp_control_intr2.h"

#ifndef BCHP_INT_ID_BSP_CONTROL_INTR2_H__
#define BCHP_INT_ID_BSP_CONTROL_INTR2_H__

#define BCHP_INT_ID_BSP_TO_HOST_INTR          BCHP_INT_ID_CREATE(BCHP_BSP_CONTROL_INTR2_CPU_STATUS, BCHP_BSP_CONTROL_INTR2_CPU_STATUS_BSP_TO_HOST_INTR_SHIFT)
#define BCHP_INT_ID_MEM_DMA_0_INTR            BCHP_INT_ID_CREATE(BCHP_BSP_CONTROL_INTR2_CPU_STATUS, BCHP_BSP_CONTROL_INTR2_CPU_STATUS_MEM_DMA_0_INTR_SHIFT)
#define BCHP_INT_ID_NSK_INTR                  BCHP_INT_ID_CREATE(BCHP_BSP_CONTROL_INTR2_CPU_STATUS, BCHP_BSP_CONTROL_INTR2_CPU_STATUS_NSK_INTR_SHIFT)
#define BCHP_INT_ID_OCMDQUE1_INTR             BCHP_INT_ID_CREATE(BCHP_BSP_CONTROL_INTR2_CPU_STATUS, BCHP_BSP_CONTROL_INTR2_CPU_STATUS_OCMDQUE1_INTR_SHIFT)
#define BCHP_INT_ID_OCMDQUE2_INTR             BCHP_INT_ID_CREATE(BCHP_BSP_CONTROL_INTR2_CPU_STATUS, BCHP_BSP_CONTROL_INTR2_CPU_STATUS_OCMDQUE2_INTR_SHIFT)
#define BCHP_INT_ID_OCMDQUE_CR_INTR           BCHP_INT_ID_CREATE(BCHP_BSP_CONTROL_INTR2_CPU_STATUS, BCHP_BSP_CONTROL_INTR2_CPU_STATUS_OCMDQUE_CR_INTR_SHIFT)
#define BCHP_INT_ID_SECTOP_GR_ERROR_INTR      BCHP_INT_ID_CREATE(BCHP_BSP_CONTROL_INTR2_CPU_STATUS, BCHP_BSP_CONTROL_INTR2_CPU_STATUS_SECTOP_GR_ERROR_INTR_SHIFT)
#define BCHP_INT_ID_UNUSED_INTR               BCHP_INT_ID_CREATE(BCHP_BSP_CONTROL_INTR2_CPU_STATUS, BCHP_BSP_CONTROL_INTR2_CPU_STATUS_UNUSED_INTR_SHIFT)

#endif /* #ifndef BCHP_INT_ID_BSP_CONTROL_INTR2_H__ */

/* End of File */