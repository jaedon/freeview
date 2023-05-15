/***************************************************************************
 *     Copyright (c) 2006-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: brap_ac3_stub.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 8/8/07 2:14p $
 *
 * Module Description: Raptor DDP FW stub
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/rap/7400/b0/image/brap_ac3_stub.c $
 * 
 * Hydra_Software_Devel/1   8/8/07 2:14p arult
 * PR33582: Added stub for AC3 decoder.
 * 
 *******************************************************************/
 
/*
This file is a stub for the following files:
brap_ac3_be_decode.c
brap_ac3_decode_table.c
brap_ac3_fe_decode.c
brap_ac3_framesync.c
brap_ac3_interframe_buf.c
brap_ac3_passthru.c
brap_ac3_passthru_interframe_buf.c
brap_ac3_passthru_table.c
*/
#include "bchp.h"

const uint32_t BRAP_IMG_ac3_be_decode_array[] = {0};
const uint32_t BRAP_IMG_ac3_be_decode_header [2] = {sizeof(BRAP_IMG_ac3_be_decode_array), 1};
const void * BRAP_IMG_ac3_be_decode [2] = {BRAP_IMG_ac3_be_decode_header, BRAP_IMG_ac3_be_decode_array};

const uint32_t BRAP_IMG_ac3_decode_table_array[] = {0};
const uint32_t BRAP_IMG_ac3_decode_table_header [2] = {sizeof(BRAP_IMG_ac3_decode_table_array), 1};
const void * BRAP_IMG_ac3_decode_table [2] = {BRAP_IMG_ac3_decode_table_header, BRAP_IMG_ac3_decode_table_array};

const uint32_t BRAP_IMG_ac3_fe_decode_array[] = {0};
const uint32_t BRAP_IMG_ac3_fe_decode_header [2] = {sizeof(BRAP_IMG_ac3_fe_decode_array), 1};
const void * BRAP_IMG_ac3_fe_decode [2] = {BRAP_IMG_ac3_fe_decode_header, BRAP_IMG_ac3_fe_decode_array};

const uint32_t BRAP_IMG_ac3_framesync_array[] = {0};
const uint32_t BRAP_IMG_ac3_framesync_header [2] = {sizeof(BRAP_IMG_ac3_framesync_array), 1};
const void * BRAP_IMG_ac3_framesync [2] = {BRAP_IMG_ac3_framesync_header, BRAP_IMG_ac3_framesync_array};

const uint32_t BRAP_IMG_ac3_interframe_buf_array[] = {0};
const uint32_t BRAP_IMG_ac3_interframe_buf_header [2] = {sizeof(BRAP_IMG_ac3_interframe_buf_array), 1};
const void * BRAP_IMG_ac3_interframe_buf [2] = {BRAP_IMG_ac3_interframe_buf_header, BRAP_IMG_ac3_interframe_buf_array};

const uint32_t BRAP_IMG_ac3_passthru_array[] = {0};
const uint32_t BRAP_IMG_ac3_passthru_header [2] = {sizeof(BRAP_IMG_ac3_passthru_array), 1};
const void * BRAP_IMG_ac3_passthru [2] = {BRAP_IMG_ac3_passthru_header, BRAP_IMG_ac3_passthru_array};

const uint32_t BRAP_IMG_ac3_passthru_interframe_buf_array[] = {0};
const uint32_t BRAP_IMG_ac3_passthru_interframe_buf_header [2] = {sizeof(BRAP_IMG_ac3_passthru_interframe_buf_array), 1};
const void * BRAP_IMG_ac3_passthru_interframe_buf [2] = {BRAP_IMG_ac3_passthru_interframe_buf_header, BRAP_IMG_ac3_passthru_interframe_buf_array};

const uint32_t BRAP_IMG_ac3_passthru_table_array[] = {0};
const uint32_t BRAP_IMG_ac3_passthru_table_header [2] = {sizeof(BRAP_IMG_ac3_passthru_table_array), 1};
const void * BRAP_IMG_ac3_passthru_table [2] = {BRAP_IMG_ac3_passthru_table_header, BRAP_IMG_ac3_passthru_table_array};

const uint32_t BRAP_IMG_ac3_convert_array[] = {0};
const uint32_t BRAP_IMG_ac3_convert_header [2] = {sizeof(BRAP_IMG_ac3_convert_array), 1};
const void * BRAP_IMG_ac3_convert [2] = {BRAP_IMG_ac3_convert_header, BRAP_IMG_ac3_convert_array};

