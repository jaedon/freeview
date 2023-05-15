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
 * $brcm_Workfile: bchp_int_id_hif_intr2.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 1/9/08 10:54a $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Wed Jan  9 09:07:55 2008
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
 * $brcm_Log: /magnum/basemodules/chp/7400/rdb/e0/bchp_int_id_hif_intr2.h $
 * 
 * Hydra_Software_Devel/1   1/9/08 10:54a rpan
 * PR38572: Initial revision.
 *
 ***************************************************************************/

#include "bchp.h"
#include "bchp_hif_intr2.h"

#ifndef BCHP_INT_ID_HIF_INTR2_H__
#define BCHP_INT_ID_HIF_INTR2_H__

#define BCHP_INT_ID_EBI_TEA_INTR              BCHP_INT_ID_CREATE(BCHP_HIF_INTR2_CPU_STATUS, BCHP_HIF_INTR2_CPU_STATUS_EBI_TEA_INTR_SHIFT)
#define BCHP_INT_ID_EBI_TIMEOUT_INTR          BCHP_INT_ID_CREATE(BCHP_HIF_INTR2_CPU_STATUS, BCHP_HIF_INTR2_CPU_STATUS_EBI_TIMEOUT_INTR_SHIFT)
#define BCHP_INT_ID_HIF_RGR_BRIDGE_INTR       BCHP_INT_ID_CREATE(BCHP_HIF_INTR2_CPU_STATUS, BCHP_HIF_INTR2_CPU_STATUS_HIF_RGR_BRIDGE_INTR_SHIFT)
#define BCHP_INT_ID_MICH_INST_RD_INTR         BCHP_INT_ID_CREATE(BCHP_HIF_INTR2_CPU_STATUS, BCHP_HIF_INTR2_CPU_STATUS_MICH_INST_RD_INTR_SHIFT)
#define BCHP_INT_ID_MICH_WR_INTR              BCHP_INT_ID_CREATE(BCHP_HIF_INTR2_CPU_STATUS, BCHP_HIF_INTR2_CPU_STATUS_MICH_WR_INTR_SHIFT)
#define BCHP_INT_ID_NAND_BLKERA_INTR          BCHP_INT_ID_CREATE(BCHP_HIF_INTR2_CPU_STATUS, BCHP_HIF_INTR2_CPU_STATUS_NAND_BLKERA_INTR_SHIFT)
#define BCHP_INT_ID_NAND_CORR_INTR            BCHP_INT_ID_CREATE(BCHP_HIF_INTR2_CPU_STATUS, BCHP_HIF_INTR2_CPU_STATUS_NAND_CORR_INTR_SHIFT)
#define BCHP_INT_ID_NAND_CPYBK_INTR           BCHP_INT_ID_CREATE(BCHP_HIF_INTR2_CPU_STATUS, BCHP_HIF_INTR2_CPU_STATUS_NAND_CPYBK_INTR_SHIFT)
#define BCHP_INT_ID_NAND_CTLRDY_INTR          BCHP_INT_ID_CREATE(BCHP_HIF_INTR2_CPU_STATUS, BCHP_HIF_INTR2_CPU_STATUS_NAND_CTLRDY_INTR_SHIFT)
#define BCHP_INT_ID_NAND_NP_READ_INTR         BCHP_INT_ID_CREATE(BCHP_HIF_INTR2_CPU_STATUS, BCHP_HIF_INTR2_CPU_STATUS_NAND_NP_READ_INTR_SHIFT)
#define BCHP_INT_ID_NAND_PGMPG_INTR           BCHP_INT_ID_CREATE(BCHP_HIF_INTR2_CPU_STATUS, BCHP_HIF_INTR2_CPU_STATUS_NAND_PGMPG_INTR_SHIFT)
#define BCHP_INT_ID_NAND_RBPIN_INTR           BCHP_INT_ID_CREATE(BCHP_HIF_INTR2_CPU_STATUS, BCHP_HIF_INTR2_CPU_STATUS_NAND_RBPIN_INTR_SHIFT)
#define BCHP_INT_ID_NAND_UNC_INTR             BCHP_INT_ID_CREATE(BCHP_HIF_INTR2_CPU_STATUS, BCHP_HIF_INTR2_CPU_STATUS_NAND_UNC_INTR_SHIFT)
#define BCHP_INT_ID_PCI_ADR_PERR_INTR         BCHP_INT_ID_CREATE(BCHP_HIF_INTR2_CPU_STATUS, BCHP_HIF_INTR2_CPU_STATUS_PCI_ADR_PERR_INTR_SHIFT)
#define BCHP_INT_ID_PCI_DMA_DONE              BCHP_INT_ID_CREATE(BCHP_HIF_INTR2_CPU_STATUS, BCHP_HIF_INTR2_CPU_STATUS_PCI_DMA_DONE_SHIFT)
#define BCHP_INT_ID_PCI_DMA_ERROR             BCHP_INT_ID_CREATE(BCHP_HIF_INTR2_CPU_STATUS, BCHP_HIF_INTR2_CPU_STATUS_PCI_DMA_ERROR_SHIFT)
#define BCHP_INT_ID_PCI_RBUS_MSTR_ERR_INTR    BCHP_INT_ID_CREATE(BCHP_HIF_INTR2_CPU_STATUS, BCHP_HIF_INTR2_CPU_STATUS_PCI_RBUS_MSTR_ERR_INTR_SHIFT)
#define BCHP_INT_ID_PCI_REC_DATA_PERR_INTR    BCHP_INT_ID_CREATE(BCHP_HIF_INTR2_CPU_STATUS, BCHP_HIF_INTR2_CPU_STATUS_PCI_REC_DATA_PERR_INTR_SHIFT)
#define BCHP_INT_ID_PCI_REC_MSTR_ABORT_INTR   BCHP_INT_ID_CREATE(BCHP_HIF_INTR2_CPU_STATUS, BCHP_HIF_INTR2_CPU_STATUS_PCI_REC_MSTR_ABORT_INTR_SHIFT)
#define BCHP_INT_ID_PCI_REC_TAR_ABORT_INTR    BCHP_INT_ID_CREATE(BCHP_HIF_INTR2_CPU_STATUS, BCHP_HIF_INTR2_CPU_STATUS_PCI_REC_TAR_ABORT_INTR_SHIFT)
#define BCHP_INT_ID_PCI_RG_BRIDGE_INTR        BCHP_INT_ID_CREATE(BCHP_HIF_INTR2_CPU_STATUS, BCHP_HIF_INTR2_CPU_STATUS_PCI_RG_BRIDGE_INTR_SHIFT)
#define BCHP_INT_ID_PCI_SERR_DET_INTR         BCHP_INT_ID_CREATE(BCHP_HIF_INTR2_CPU_STATUS, BCHP_HIF_INTR2_CPU_STATUS_PCI_SERR_DET_INTR_SHIFT)
#define BCHP_INT_ID_PCI_SRC_DATA_PERR_INTR    BCHP_INT_ID_CREATE(BCHP_HIF_INTR2_CPU_STATUS, BCHP_HIF_INTR2_CPU_STATUS_PCI_SRC_DATA_PERR_INTR_SHIFT)

#endif /* #ifndef BCHP_INT_ID_HIF_INTR2_H__ */

/* End of File */