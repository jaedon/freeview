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
 * $brcm_Workfile: bchp_int_id_xpt_pb1.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 4/4/11 1:44p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Fri Apr  1 16:56:44 2011
 *                 MD5 Checksum         d03d08c4839c3311c9d35c4cd5e10373
 *
 * Compiled with:  RDB Utility          unknown
 *                 RDB Parser           3.0
 *                 generate_int_id.pl   1.0
 *                 Perl Interpreter     5.008005
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7344/rdb/b0/bchp_int_id_xpt_pb1.h $
 * 
 * Hydra_Software_Devel/1   4/4/11 1:44p albertl
 * SW7344-40: Initial revision.
 *
 ***************************************************************************/

#include "bchp.h"
#include "bchp_xpt_pb0.h"
#include "bchp_xpt_pb1.h"

#ifndef BCHP_INT_ID_XPT_PB1_H__
#define BCHP_INT_ID_XPT_PB1_H__

#define BCHP_INT_ID_XPT_PB1_ASF_COMPRESSED_DATA_RECEIVED BCHP_INT_ID_CREATE(BCHP_XPT_PB1_INTR, BCHP_XPT_PB0_INTR_ASF_COMPRESSED_DATA_RECEIVED_SHIFT)
#define BCHP_INT_ID_XPT_PB1_ASF_FATAL_ERROR   BCHP_INT_ID_CREATE(BCHP_XPT_PB1_INTR, BCHP_XPT_PB0_INTR_ASF_FATAL_ERROR_SHIFT)
#define BCHP_INT_ID_XPT_PB1_ASF_LENGTH_ERROR  BCHP_INT_ID_CREATE(BCHP_XPT_PB1_INTR, BCHP_XPT_PB0_INTR_ASF_LENGTH_ERROR_SHIFT)
#define BCHP_INT_ID_XPT_PB1_ASF_PADDING_LEN_ERROR BCHP_INT_ID_CREATE(BCHP_XPT_PB1_INTR, BCHP_XPT_PB0_INTR_ASF_PADDING_LEN_ERROR_SHIFT)
#define BCHP_INT_ID_XPT_PB1_ASF_PROTOCOL_ERROR BCHP_INT_ID_CREATE(BCHP_XPT_PB1_INTR, BCHP_XPT_PB0_INTR_ASF_PROTOCOL_ERROR_SHIFT)
#define BCHP_INT_ID_XPT_PB1_DONE_INT          BCHP_INT_ID_CREATE(BCHP_XPT_PB1_INTR, BCHP_XPT_PB0_INTR_DONE_INT_SHIFT)
#define BCHP_INT_ID_XPT_PB1_MULTI_BUF_PKT_ERROR BCHP_INT_ID_CREATE(BCHP_XPT_PB1_INTR, BCHP_XPT_PB0_INTR_MULTI_BUF_PKT_ERROR_SHIFT)
#define BCHP_INT_ID_XPT_PB1_PARSER_LENGTH_ERROR BCHP_INT_ID_CREATE(BCHP_XPT_PB1_INTR, BCHP_XPT_PB0_INTR_PARSER_LENGTH_ERROR_SHIFT)
#define BCHP_INT_ID_XPT_PB1_PARSER_TRANSPORT_ERROR BCHP_INT_ID_CREATE(BCHP_XPT_PB1_INTR, BCHP_XPT_PB0_INTR_PARSER_TRANSPORT_ERROR_SHIFT)
#define BCHP_INT_ID_XPT_PB1_PB_COPYRIGHT_CHANGE BCHP_INT_ID_CREATE(BCHP_XPT_PB1_INTR, BCHP_XPT_PB0_INTR_PB_COPYRIGHT_CHANGE_SHIFT)
#define BCHP_INT_ID_XPT_PB1_SE_OUT_OF_SYNC_INT BCHP_INT_ID_CREATE(BCHP_XPT_PB1_INTR, BCHP_XPT_PB0_INTR_SE_OUT_OF_SYNC_INT_SHIFT)
#define BCHP_INT_ID_XPT_PB1_TS_PARITY_ERROR   BCHP_INT_ID_CREATE(BCHP_XPT_PB1_INTR, BCHP_XPT_PB0_INTR_TS_PARITY_ERROR_SHIFT)
#define BCHP_INT_ID_XPT_PB1_TS_RANGE_ERROR    BCHP_INT_ID_CREATE(BCHP_XPT_PB1_INTR, BCHP_XPT_PB0_INTR_TS_RANGE_ERROR_SHIFT)

#endif /* #ifndef BCHP_INT_ID_XPT_PB1_H__ */

/* End of File */