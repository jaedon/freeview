/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: brap_wma_stub.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 9/11/12 4:50p $
 *
 * Module Description: Raptor WMA FW stub
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/rap/src/fmm130/firmware/zsp500/brap_wma_stub.c $
 * 
 * Hydra_Software_Devel/1   9/11/12 4:50p srajapur
 * SW7420-2341:Remove symbolic links and flatten the RAP directory
 * 
 * Hydra_Software_Devel/NEW_DIR_SW7420_2341/1   9/7/12 3:47p srajapur
 * SW7420-2341:Remove symbolic links and flatten the RAP directory
 * 
 * Hydra_Software_Devel/1   12/29/06 11:29a kwelch
 * PR26849: Initial version.
 * Empty function replacement for WMA decoder.
 * 
 *******************************************************************/
 
/*
This file is a stub for the following files:
brap_wma_std_framesync.c
brap_wma_std_interframe_buf.c
brap_wma_std_stg1_decode.c
brap_wma_std_stg1_decode_table.c
*/
#include "bchp.h"

const uint32_t BRAP_IMG_wma_std_framesync_array[] = {0};
const uint32_t BRAP_IMG_wma_std_framesync_header [2] = {sizeof(BRAP_IMG_wma_std_framesync_array), 1};
const void * BRAP_IMG_wma_std_framesync [2] = {BRAP_IMG_wma_std_framesync_header, BRAP_IMG_wma_std_framesync_array};

const uint32_t BRAP_IMG_wma_std_interframe_buf_array[] = {0};
const uint32_t BRAP_IMG_wma_std_interframe_buf_header [2] = {sizeof(BRAP_IMG_wma_std_interframe_buf_array), 1};
const void * BRAP_IMG_wma_std_interframe_buf [2] = {BRAP_IMG_wma_std_interframe_buf_header, BRAP_IMG_wma_std_interframe_buf_array};

const uint32_t BRAP_IMG_wma_std_stg1_decode_array[] = {0};
const uint32_t BRAP_IMG_wma_std_stg1_decode_header [2] = {sizeof(BRAP_IMG_wma_std_stg1_decode_array), 1};
const void * BRAP_IMG_wma_std_stg1_decode [2] = {BRAP_IMG_wma_std_stg1_decode_header, BRAP_IMG_wma_std_stg1_decode_array};

const uint32_t BRAP_IMG_wma_std_stg1_decode_table_array[] = {0};
const uint32_t BRAP_IMG_wma_std_stg1_decode_table_header [2] = {sizeof(BRAP_IMG_wma_std_stg1_decode_table_array), 1};
const void * BRAP_IMG_wma_std_stg1_decode_table [2] = {BRAP_IMG_wma_std_stg1_decode_table_header, BRAP_IMG_wma_std_stg1_decode_table_array};
