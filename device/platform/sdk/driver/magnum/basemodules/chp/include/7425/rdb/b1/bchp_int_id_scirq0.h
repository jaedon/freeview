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
 * $brcm_Workfile: bchp_int_id_scirq0.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 10/27/11 11:09p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Thu Oct 27 17:52:12 2011
 *                 MD5 Checksum         d0bb3b528cbe25f62f7a44e82cd25af7
 *
 * Compiled with:  RDB Utility          unknown
 *                 RDB Parser           3.0
 *                 generate_int_id.pl   1.0
 *                 Perl Interpreter     5.008005
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7425/rdb/b1/bchp_int_id_scirq0.h $
 * 
 * Hydra_Software_Devel/1   10/27/11 11:09p vanessah
 * SW7425-1620: add 7425 B0 rdb header file
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