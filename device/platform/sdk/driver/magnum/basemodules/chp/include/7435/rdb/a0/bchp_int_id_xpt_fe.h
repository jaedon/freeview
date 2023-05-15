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
 * $brcm_Workfile: bchp_int_id_xpt_fe.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 10/11/11 7:25p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Tue Oct 11 11:45:31 2011
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
 * $brcm_Log: /magnum/basemodules/chp/7435/rdb/a0/bchp_int_id_xpt_fe.h $
 * 
 * Hydra_Software_Devel/1   10/11/11 7:25p pntruong
 * SW7435-3: Synced up with central rdb.
 *
 ***************************************************************************/

#include "bchp.h"
#include "bchp_xpt_fe.h"

#ifndef BCHP_INT_ID_XPT_FE_H__
#define BCHP_INT_ID_XPT_FE_H__

#define BCHP_INT_ID_INPUT_BUFFER_OVFL_ERR     BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS1_REG, BCHP_XPT_FE_INTR_STATUS1_REG_INPUT_BUFFER_OVFL_ERR_SHIFT)
#define BCHP_INT_ID_PARSER0_LENGTH_ERROR      BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS0_REG, BCHP_XPT_FE_INTR_STATUS0_REG_PARSER0_LENGTH_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER0_TRANSPORT_ERROR   BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS0_REG, BCHP_XPT_FE_INTR_STATUS0_REG_PARSER0_TRANSPORT_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER10_LENGTH_ERROR     BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS0_REG, BCHP_XPT_FE_INTR_STATUS0_REG_PARSER10_LENGTH_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER10_TRANSPORT_ERROR  BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS0_REG, BCHP_XPT_FE_INTR_STATUS0_REG_PARSER10_TRANSPORT_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER11_LENGTH_ERROR     BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS0_REG, BCHP_XPT_FE_INTR_STATUS0_REG_PARSER11_LENGTH_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER11_TRANSPORT_ERROR  BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS0_REG, BCHP_XPT_FE_INTR_STATUS0_REG_PARSER11_TRANSPORT_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER12_LENGTH_ERROR     BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS0_REG, BCHP_XPT_FE_INTR_STATUS0_REG_PARSER12_LENGTH_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER12_TRANSPORT_ERROR  BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS0_REG, BCHP_XPT_FE_INTR_STATUS0_REG_PARSER12_TRANSPORT_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER13_LENGTH_ERROR     BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS0_REG, BCHP_XPT_FE_INTR_STATUS0_REG_PARSER13_LENGTH_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER13_TRANSPORT_ERROR  BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS0_REG, BCHP_XPT_FE_INTR_STATUS0_REG_PARSER13_TRANSPORT_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER14_LENGTH_ERROR     BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS0_REG, BCHP_XPT_FE_INTR_STATUS0_REG_PARSER14_LENGTH_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER14_TRANSPORT_ERROR  BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS0_REG, BCHP_XPT_FE_INTR_STATUS0_REG_PARSER14_TRANSPORT_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER15_LENGTH_ERROR     BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS0_REG, BCHP_XPT_FE_INTR_STATUS0_REG_PARSER15_LENGTH_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER15_TRANSPORT_ERROR  BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS0_REG, BCHP_XPT_FE_INTR_STATUS0_REG_PARSER15_TRANSPORT_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER1_LENGTH_ERROR      BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS0_REG, BCHP_XPT_FE_INTR_STATUS0_REG_PARSER1_LENGTH_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER1_TRANSPORT_ERROR   BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS0_REG, BCHP_XPT_FE_INTR_STATUS0_REG_PARSER1_TRANSPORT_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER2_LENGTH_ERROR      BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS0_REG, BCHP_XPT_FE_INTR_STATUS0_REG_PARSER2_LENGTH_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER2_TRANSPORT_ERROR   BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS0_REG, BCHP_XPT_FE_INTR_STATUS0_REG_PARSER2_TRANSPORT_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER3_LENGTH_ERROR      BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS0_REG, BCHP_XPT_FE_INTR_STATUS0_REG_PARSER3_LENGTH_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER3_TRANSPORT_ERROR   BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS0_REG, BCHP_XPT_FE_INTR_STATUS0_REG_PARSER3_TRANSPORT_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER4_LENGTH_ERROR      BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS0_REG, BCHP_XPT_FE_INTR_STATUS0_REG_PARSER4_LENGTH_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER4_TRANSPORT_ERROR   BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS0_REG, BCHP_XPT_FE_INTR_STATUS0_REG_PARSER4_TRANSPORT_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER5_LENGTH_ERROR      BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS0_REG, BCHP_XPT_FE_INTR_STATUS0_REG_PARSER5_LENGTH_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER5_TRANSPORT_ERROR   BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS0_REG, BCHP_XPT_FE_INTR_STATUS0_REG_PARSER5_TRANSPORT_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER6_LENGTH_ERROR      BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS0_REG, BCHP_XPT_FE_INTR_STATUS0_REG_PARSER6_LENGTH_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER6_TRANSPORT_ERROR   BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS0_REG, BCHP_XPT_FE_INTR_STATUS0_REG_PARSER6_TRANSPORT_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER7_LENGTH_ERROR      BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS0_REG, BCHP_XPT_FE_INTR_STATUS0_REG_PARSER7_LENGTH_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER7_TRANSPORT_ERROR   BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS0_REG, BCHP_XPT_FE_INTR_STATUS0_REG_PARSER7_TRANSPORT_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER8_LENGTH_ERROR      BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS0_REG, BCHP_XPT_FE_INTR_STATUS0_REG_PARSER8_LENGTH_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER8_TRANSPORT_ERROR   BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS0_REG, BCHP_XPT_FE_INTR_STATUS0_REG_PARSER8_TRANSPORT_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER9_LENGTH_ERROR      BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS0_REG, BCHP_XPT_FE_INTR_STATUS0_REG_PARSER9_LENGTH_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER9_TRANSPORT_ERROR   BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS0_REG, BCHP_XPT_FE_INTR_STATUS0_REG_PARSER9_TRANSPORT_ERROR_SHIFT)

#endif /* #ifndef BCHP_INT_ID_XPT_FE_H__ */

/* End of File */