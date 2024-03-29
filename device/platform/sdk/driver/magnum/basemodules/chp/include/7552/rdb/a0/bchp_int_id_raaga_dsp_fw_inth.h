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
 * $brcm_Workfile: bchp_int_id_raaga_dsp_fw_inth.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 5/18/11 4:19p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Tue May 17 04:43:05 2011
 *                 MD5 Checksum         f5f09b2bf7ad40890d2e5dc57d4789b6
 *
 * Compiled with:  RDB Utility          unknown
 *                 RDB Parser           3.0
 *                 generate_int_id.pl   1.0
 *                 Perl Interpreter     5.008005
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7552/rdb/a0/bchp_int_id_raaga_dsp_fw_inth.h $
 * 
 * Hydra_Software_Devel/2   5/18/11 4:19p xhuang
 * SW7552-2: update with central RDB
 *
 ***************************************************************************/

#include "bchp.h"
#include "bchp_raaga_dsp_fw_inth.h"

#ifndef BCHP_INT_ID_RAAGA_DSP_FW_INTH_H__
#define BCHP_INT_ID_RAAGA_DSP_FW_INTH_H__

#define BCHP_INT_ID_ASYNC_MSG                 BCHP_INT_ID_CREATE(BCHP_RAAGA_DSP_FW_INTH_HOST_STATUS, BCHP_RAAGA_DSP_FW_INTH_HOST_STATUS_ASYNC_MSG_SHIFT)
#define BCHP_INT_ID_HOST_MSG                  BCHP_INT_ID_CREATE(BCHP_RAAGA_DSP_FW_INTH_HOST_STATUS, BCHP_RAAGA_DSP_FW_INTH_HOST_STATUS_HOST_MSG_SHIFT)
#define BCHP_INT_ID_SYNC_MSG                  BCHP_INT_ID_CREATE(BCHP_RAAGA_DSP_FW_INTH_HOST_STATUS, BCHP_RAAGA_DSP_FW_INTH_HOST_STATUS_SYNC_MSG_SHIFT)

#endif /* #ifndef BCHP_INT_ID_RAAGA_DSP_FW_INTH_H__ */

/* End of File */
