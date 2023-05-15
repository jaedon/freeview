/***************************************************************************
 *     Copyright (c) 2006-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: brap_aac_stub.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 10/19/10 11:19a $
 *
 * Module Description: Raptor AAC FW stub
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/rap/7400/d0/image/brap_aac_stub.c $
 * 
 * Hydra_Software_Devel/3   10/19/10 11:19a mohamedp
 * SW7400-2923:Adding compilation support for AAC algorithm.
 * 
 * Hydra_Software_Devel/2   10/18/10 1:06p mohamedp
 * SW7400-2923:Adding compilation support for AAC algorithm.
 * 
 * Hydra_Software_Devel/1   10/8/10 10:37a mohamedp
 * SW7400-2923:Adding compilation support for AAC algorithm.
 * 
 * Hydra_Software_Devel/SanJose_CDI_Devel/1   3/11/10 1:31p bhsu
 * SW7401-4372: Dynamic Loadable Audio AAC FW support and fixed incorrect
 * pointers.
 * 
 * Hydra_Software_Devel/1   6/23/09 4:03p amitver
 * PR 56249 : [7401] Adding compilation support for AAC algorithm
 * 
 * 
 *******************************************************************/
 
#include "bchp.h"

const uint32_t BRAP_IMG_aache_decode_stg0_array[] = {0};
const uint32_t BRAP_IMG_aache_decode_stg0_header [2] = {sizeof(BRAP_IMG_aache_decode_stg0_array), 1};
const void * BRAP_IMG_aache_decode_stg0 [2] = {BRAP_IMG_aache_decode_stg0_header, BRAP_IMG_aache_decode_stg0_array};

const uint32_t BRAP_IMG_aache_decode_stg1_array[] = {0};
const uint32_t BRAP_IMG_aache_decode_stg1_header [2] = {sizeof(BRAP_IMG_aache_decode_stg1_array), 1};
const void * BRAP_IMG_aache_decode_stg1 [2] = {BRAP_IMG_aache_decode_stg1_header, BRAP_IMG_aache_decode_stg1_array};

const uint32_t BRAP_IMG_aache_decode_stg2_array[] = {0};
const uint32_t BRAP_IMG_aache_decode_stg2_header [2] = {sizeof(BRAP_IMG_aache_decode_stg2_array), 1};
const void * BRAP_IMG_aache_decode_stg2 [2] = {BRAP_IMG_aache_decode_stg2_header, BRAP_IMG_aache_decode_stg2_array};

const uint32_t BRAP_IMG_aache_decode_stg3_array[] = {0};
const uint32_t BRAP_IMG_aache_decode_stg3_header [2] = {sizeof(BRAP_IMG_aache_decode_stg3_array), 1};
const void * BRAP_IMG_aache_decode_stg3 [2] = {BRAP_IMG_aache_decode_stg3_header, BRAP_IMG_aache_decode_stg3_array};

const uint32_t BRAP_IMG_aache_decode_stg4_array[] = {0};
const uint32_t BRAP_IMG_aache_decode_stg4_header [2] = {sizeof(BRAP_IMG_aache_decode_stg4_array), 1};
const void * BRAP_IMG_aache_decode_stg4 [2] = {BRAP_IMG_aache_decode_stg4_header, BRAP_IMG_aache_decode_stg4_array};

const uint32_t BRAP_IMG_aache_decode_table_stg0_array[] = {0};
const uint32_t BRAP_IMG_aache_decode_table_stg0_header [2] = {sizeof(BRAP_IMG_aache_decode_table_stg0_array), 1};
const void * BRAP_IMG_aache_decode_table_stg0 [2] = {BRAP_IMG_aache_decode_table_stg0_header, BRAP_IMG_aache_decode_table_stg0_array};

const uint32_t BRAP_IMG_aache_decode_table_stg1_array[] = {0};
const uint32_t BRAP_IMG_aache_decode_table_stg1_header [2] = {sizeof(BRAP_IMG_aache_decode_table_stg1_array), 1};
const void * BRAP_IMG_aache_decode_table_stg1 [2] = {BRAP_IMG_aache_decode_table_stg1_header, BRAP_IMG_aache_decode_table_stg1_array};

const uint32_t BRAP_IMG_aache_decode_table_stg2_array[] = {0};
const uint32_t BRAP_IMG_aache_decode_table_stg2_header [2] = {sizeof(BRAP_IMG_aache_decode_table_stg2_array), 1};
const void * BRAP_IMG_aache_decode_table_stg2 [2] = {BRAP_IMG_aache_decode_table_stg2_header, BRAP_IMG_aache_decode_table_stg2_array};

const uint32_t BRAP_IMG_aache_decode_table_stg3_array[] = {0};
const uint32_t BRAP_IMG_aache_decode_table_stg3_header [2] = {sizeof(BRAP_IMG_aache_decode_table_stg3_array), 1};
const void * BRAP_IMG_aache_decode_table_stg3 [2] = {BRAP_IMG_aache_decode_table_stg3_header, BRAP_IMG_aache_decode_table_stg3_array};

const uint32_t BRAP_IMG_aache_decode_table_stg4_array[] = {0};
const uint32_t BRAP_IMG_aache_decode_table_stg4_header [2] = {sizeof(BRAP_IMG_aache_decode_table_stg4_array), 1};
const void * BRAP_IMG_aache_decode_table_stg4 [2] = {BRAP_IMG_aache_decode_table_stg4_header, BRAP_IMG_aache_decode_table_stg4_array};

const uint32_t BRAP_IMG_aache_interframe_buf_array1[] = {0};
const uint32_t BRAP_IMG_aache_interframe_buf_array2[] = {0};
const uint32_t BRAP_IMG_aache_interframe_buf_array3[] = {0};
const uint32_t BRAP_IMG_aache_interframe_buf_header [2] = {sizeof(BRAP_IMG_aache_interframe_buf_array1) + sizeof(BRAP_IMG_aache_interframe_buf_array2) + sizeof(BRAP_IMG_aache_interframe_buf_array3), 3};
const void * const BRAP_IMG_aache_interframe_buf [4] = {BRAP_IMG_aache_interframe_buf_header, BRAP_IMG_aache_interframe_buf_array1, BRAP_IMG_aache_interframe_buf_array2, BRAP_IMG_aache_interframe_buf_array3};

const uint32_t BRAP_IMG_aac_framesync_array[] = {0};
const uint32_t BRAP_IMG_aac_framesync_header [2] = {sizeof(BRAP_IMG_aac_framesync_array), 1};
const void * BRAP_IMG_aac_framesync [2] = {BRAP_IMG_aac_framesync_header, BRAP_IMG_aac_framesync_array};

const uint32_t BRAP_IMG_aac_passthru_array[] = {0};
const uint32_t BRAP_IMG_aac_passthru_header [2] = {sizeof(BRAP_IMG_aac_passthru_array), 1};
const void * BRAP_IMG_aac_passthru [2] = {BRAP_IMG_aac_passthru_header, BRAP_IMG_aac_passthru_array};

const uint32_t BRAP_IMG_aache_framesync_array[] = {0};
const uint32_t BRAP_IMG_aache_framesync_header [2] = {sizeof(BRAP_IMG_aache_framesync_array), 1};
const void * BRAP_IMG_aache_framesync [2] = {BRAP_IMG_aache_framesync_header, BRAP_IMG_aache_framesync_array};

const uint32_t BRAP_IMG_aac_downmix_array[] = {0};
const uint32_t BRAP_IMG_aac_downmix_header [2] = {sizeof(BRAP_IMG_aac_downmix_array), 1};
const void * BRAP_IMG_aac_downmix [2] = {BRAP_IMG_aac_downmix_header, BRAP_IMG_aac_downmix_array};


const uint32_t BRAP_IMG_aache_passthru_array[] = {0};
const uint32_t BRAP_IMG_aache_passthru_header [2] = {sizeof(BRAP_IMG_aache_passthru_array), 1};
const void * BRAP_IMG_aache_passthru [2] = {BRAP_IMG_aache_passthru_header, BRAP_IMG_aache_passthru_array};

