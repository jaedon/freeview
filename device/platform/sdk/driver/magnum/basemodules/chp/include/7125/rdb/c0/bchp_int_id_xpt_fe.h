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
 * $brcm_Workfile: bchp_int_id_xpt_fe.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 1/25/10 8:40p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Fri Jan 22 20:03:26 2010
 *                 MD5 Checksum         a2d1f2163f65e87d228a0fb491cb442d
 *
 * Compiled with:  RDB Utility          unknown
 *                 RDB Parser           3.0
 *                 generate_int_id.pl   1.0
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7125/rdb/c0/bchp_int_id_xpt_fe.h $
 * 
 * Hydra_Software_Devel/1   1/25/10 8:40p albertl
 * SW7125-177: Initial revision.
 *
 ***************************************************************************/

#include "bchp.h"
#include "bchp_xpt_fe.h"

#ifndef BCHP_INT_ID_XPT_FE_H__
#define BCHP_INT_ID_XPT_FE_H__

#define BCHP_INT_ID_INPUT_BUFFER_OVFL_ERR     BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS_REG, BCHP_XPT_FE_INTR_STATUS_REG_INPUT_BUFFER_OVFL_ERR_SHIFT)
#define BCHP_INT_ID_MPOD_BUFFER_OVFL_ERR      BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS_REG, BCHP_XPT_FE_INTR_STATUS_REG_MPOD_BUFFER_OVFL_ERR_SHIFT)
#define BCHP_INT_ID_PARSER0_CONTINUITY_ERROR  BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS_REG, BCHP_XPT_FE_INTR_STATUS_REG_PARSER0_CONTINUITY_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER0_LENGTH_ERROR      BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS_REG, BCHP_XPT_FE_INTR_STATUS_REG_PARSER0_LENGTH_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER0_SEC_CC_ERROR      BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS_REG, BCHP_XPT_FE_INTR_STATUS_REG_PARSER0_SEC_CC_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER0_TRANSPORT_ERROR   BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS_REG, BCHP_XPT_FE_INTR_STATUS_REG_PARSER0_TRANSPORT_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER1_CONTINUITY_ERROR  BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS_REG, BCHP_XPT_FE_INTR_STATUS_REG_PARSER1_CONTINUITY_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER1_LENGTH_ERROR      BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS_REG, BCHP_XPT_FE_INTR_STATUS_REG_PARSER1_LENGTH_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER1_SEC_CC_ERROR      BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS_REG, BCHP_XPT_FE_INTR_STATUS_REG_PARSER1_SEC_CC_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER1_TRANSPORT_ERROR   BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS_REG, BCHP_XPT_FE_INTR_STATUS_REG_PARSER1_TRANSPORT_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER2_CONTINUITY_ERROR  BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS_REG, BCHP_XPT_FE_INTR_STATUS_REG_PARSER2_CONTINUITY_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER2_LENGTH_ERROR      BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS_REG, BCHP_XPT_FE_INTR_STATUS_REG_PARSER2_LENGTH_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER2_SEC_CC_ERROR      BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS_REG, BCHP_XPT_FE_INTR_STATUS_REG_PARSER2_SEC_CC_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER2_TRANSPORT_ERROR   BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS_REG, BCHP_XPT_FE_INTR_STATUS_REG_PARSER2_TRANSPORT_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER3_CONTINUITY_ERROR  BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS_REG, BCHP_XPT_FE_INTR_STATUS_REG_PARSER3_CONTINUITY_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER3_LENGTH_ERROR      BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS_REG, BCHP_XPT_FE_INTR_STATUS_REG_PARSER3_LENGTH_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER3_SEC_CC_ERROR      BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS_REG, BCHP_XPT_FE_INTR_STATUS_REG_PARSER3_SEC_CC_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER3_TRANSPORT_ERROR   BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS_REG, BCHP_XPT_FE_INTR_STATUS_REG_PARSER3_TRANSPORT_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER4_CONTINUITY_ERROR  BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS_REG, BCHP_XPT_FE_INTR_STATUS_REG_PARSER4_CONTINUITY_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER4_LENGTH_ERROR      BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS_REG, BCHP_XPT_FE_INTR_STATUS_REG_PARSER4_LENGTH_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER4_SEC_CC_ERROR      BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS_REG, BCHP_XPT_FE_INTR_STATUS_REG_PARSER4_SEC_CC_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER4_TRANSPORT_ERROR   BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS_REG, BCHP_XPT_FE_INTR_STATUS_REG_PARSER4_TRANSPORT_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER5_CONTINUITY_ERROR  BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS_REG, BCHP_XPT_FE_INTR_STATUS_REG_PARSER5_CONTINUITY_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER5_LENGTH_ERROR      BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS_REG, BCHP_XPT_FE_INTR_STATUS_REG_PARSER5_LENGTH_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER5_SEC_CC_ERROR      BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS_REG, BCHP_XPT_FE_INTR_STATUS_REG_PARSER5_SEC_CC_ERROR_SHIFT)
#define BCHP_INT_ID_PARSER5_TRANSPORT_ERROR   BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS_REG, BCHP_XPT_FE_INTR_STATUS_REG_PARSER5_TRANSPORT_ERROR_SHIFT)
#define BCHP_INT_ID_PSG_PROTOCOL_ERROR        BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS_REG, BCHP_XPT_FE_INTR_STATUS_REG_PSG_PROTOCOL_ERROR_SHIFT)

#endif /* #ifndef BCHP_INT_ID_XPT_FE_H__ */

/* End of File */