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
 * $brcm_Workfile: bchp_int_id_scirq0.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 8/25/10 1:03p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Tue Aug 24 17:59:36 2010
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
 * $brcm_Log: /magnum/basemodules/chp/7231/rdb/a0/bchp_int_id_scirq0.h $
 * 
 * Hydra_Software_Devel/1   8/25/10 1:03p albertl
 * HW7231-39: Initial revision.
 *
 ***************************************************************************/

#include "bchp.h"
#include "bchp_scirq0.h"

#ifndef BCHP_INT_ID_SCIRQ0_H__
#define BCHP_INT_ID_SCIRQ0_H__

#define BCHP_INT_ID_sca_irqen                 BCHP_INT_ID_CREATE(BCHP_SCIRQ0_SCIRQEN, BCHP_SCIRQ0_SCIRQEN_sca_irqen_SHIFT)
#define BCHP_INT_ID_scb_irqen                 BCHP_INT_ID_CREATE(BCHP_SCIRQ0_SCIRQEN, BCHP_SCIRQ0_SCIRQEN_scb_irqen_SHIFT)

#endif /* #ifndef BCHP_INT_ID_SCIRQ0_H__ */

/* End of File */