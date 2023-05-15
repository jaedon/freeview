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
 * $brcm_Workfile: bchp_int_id_xpt_msg.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 8/25/10 1:05p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Tue Aug 24 17:59:49 2010
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
 * $brcm_Log: /magnum/basemodules/chp/7231/rdb/a0/bchp_int_id_xpt_msg.h $
 * 
 * Hydra_Software_Devel/1   8/25/10 1:05p albertl
 * HW7231-39: Initial revision.
 *
 ***************************************************************************/

#include "bchp.h"
#include "bchp_xpt_msg.h"

#ifndef BCHP_INT_ID_XPT_MSG_H__
#define BCHP_INT_ID_XPT_MSG_H__

#define BCHP_INT_ID_CRC_ERROR                 BCHP_INT_ID_CREATE(BCHP_XPT_MSG_DAT_ERR_INTR, BCHP_XPT_MSG_DAT_ERR_INTR_CRC_ERROR_SHIFT)
#define BCHP_INT_ID_DMA_BURST_OVFL            BCHP_INT_ID_CREATE(BCHP_XPT_MSG_DAT_ERR_INTR, BCHP_XPT_MSG_DAT_ERR_INTR_DMA_BURST_OVFL_SHIFT)
#define BCHP_INT_ID_DTV_CF_ERROR              BCHP_INT_ID_CREATE(BCHP_XPT_MSG_DAT_ERR_INTR, BCHP_XPT_MSG_DAT_ERR_INTR_DTV_CF_ERROR_SHIFT)
#define BCHP_INT_ID_DTV_HD_ERROR              BCHP_INT_ID_CREATE(BCHP_XPT_MSG_DAT_ERR_INTR, BCHP_XPT_MSG_DAT_ERR_INTR_DTV_HD_ERROR_SHIFT)
#define BCHP_INT_ID_MPT_ERROR                 BCHP_INT_ID_CREATE(BCHP_XPT_MSG_DAT_ERR_INTR, BCHP_XPT_MSG_DAT_ERR_INTR_MPT_ERROR_SHIFT)
#define BCHP_INT_ID_PES_ERROR                 BCHP_INT_ID_CREATE(BCHP_XPT_MSG_DAT_ERR_INTR, BCHP_XPT_MSG_DAT_ERR_INTR_PES_ERROR_SHIFT)
#define BCHP_INT_ID_PSC_ERROR                 BCHP_INT_ID_CREATE(BCHP_XPT_MSG_DAT_ERR_INTR, BCHP_XPT_MSG_DAT_ERR_INTR_PSC_ERROR_SHIFT)
#define BCHP_INT_ID_PSI_ERROR                 BCHP_INT_ID_CREATE(BCHP_XPT_MSG_DAT_ERR_INTR, BCHP_XPT_MSG_DAT_ERR_INTR_PSI_ERROR_SHIFT)
#define BCHP_INT_ID_XPT_MSG_INTR_FLAG         BCHP_INT_ID_CREATE(BCHP_XPT_MSG_BUF_DAT_RDY_INTR_96_127, BCHP_XPT_MSG_BUF_DAT_RDY_INTR_96_127_INTR_FLAG_SHIFT)

#endif /* #ifndef BCHP_INT_ID_XPT_MSG_H__ */

/* End of File */