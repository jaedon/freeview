/***************************************************************************
 *     Copyright (c) 1999-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bchp_int_id_video_enc_intr2.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 2/28/12 5:02p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Tue Feb 28 11:08:31 2012
 *                 MD5 Checksum         d41d8cd98f00b204e9800998ecf8427e
 *
 * Compiled with:  RDB Utility          unknown
 *                 RDB Parser           3.0
 *                 generate_int_id.pl   1.0
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7435/rdb/b0/bchp_int_id_video_enc_intr2.h $
 * 
 * Hydra_Software_Devel/1   2/28/12 5:02p tdo
 * SW7435-40: Need 7435B0 public/central RDB (magnum) headers checked into
 * clearcase
 *
 ***************************************************************************/

#include "bchp.h"
#include "bchp_video_enc_intr2.h"

#ifndef BCHP_INT_ID_VIDEO_ENC_INTR2_H__
#define BCHP_INT_ID_VIDEO_ENC_INTR2_H__

#define BCHP_INT_ID_ANCIL_VBI_0_0_INTR        BCHP_INT_ID_CREATE(BCHP_VIDEO_ENC_INTR2_CPU_STATUS, BCHP_VIDEO_ENC_INTR2_CPU_STATUS_ANCIL_VBI_0_0_INTR_SHIFT)
#define BCHP_INT_ID_ANCIL_VBI_0_1_INTR        BCHP_INT_ID_CREATE(BCHP_VIDEO_ENC_INTR2_CPU_STATUS, BCHP_VIDEO_ENC_INTR2_CPU_STATUS_ANCIL_VBI_0_1_INTR_SHIFT)
#define BCHP_INT_ID_ANCIL_VBI_1_0_INTR        BCHP_INT_ID_CREATE(BCHP_VIDEO_ENC_INTR2_CPU_STATUS, BCHP_VIDEO_ENC_INTR2_CPU_STATUS_ANCIL_VBI_1_0_INTR_SHIFT)
#define BCHP_INT_ID_ANCIL_VBI_1_1_INTR        BCHP_INT_ID_CREATE(BCHP_VIDEO_ENC_INTR2_CPU_STATUS, BCHP_VIDEO_ENC_INTR2_CPU_STATUS_ANCIL_VBI_1_1_INTR_SHIFT)
#define BCHP_INT_ID_BVB_RCVR_656_0_INTR       BCHP_INT_ID_CREATE(BCHP_VIDEO_ENC_INTR2_CPU_STATUS, BCHP_VIDEO_ENC_INTR2_CPU_STATUS_BVB_RCVR_656_0_INTR_SHIFT)
#define BCHP_INT_ID_BVB_RCVR_656_1_INTR       BCHP_INT_ID_CREATE(BCHP_VIDEO_ENC_INTR2_CPU_STATUS, BCHP_VIDEO_ENC_INTR2_CPU_STATUS_BVB_RCVR_656_1_INTR_SHIFT)
#define BCHP_INT_ID_BVB_RCVR_DVI_0_INTR       BCHP_INT_ID_CREATE(BCHP_VIDEO_ENC_INTR2_CPU_STATUS, BCHP_VIDEO_ENC_INTR2_CPU_STATUS_BVB_RCVR_DVI_0_INTR_SHIFT)
#define BCHP_INT_ID_BVB_RCVR_IT_0_INTR        BCHP_INT_ID_CREATE(BCHP_VIDEO_ENC_INTR2_CPU_STATUS, BCHP_VIDEO_ENC_INTR2_CPU_STATUS_BVB_RCVR_IT_0_INTR_SHIFT)
#define BCHP_INT_ID_BVB_RCVR_IT_1_INTR        BCHP_INT_ID_CREATE(BCHP_VIDEO_ENC_INTR2_CPU_STATUS, BCHP_VIDEO_ENC_INTR2_CPU_STATUS_BVB_RCVR_IT_1_INTR_SHIFT)
#define BCHP_INT_ID_DSCL_0_INTR               BCHP_INT_ID_CREATE(BCHP_VIDEO_ENC_INTR2_CPU_STATUS, BCHP_VIDEO_ENC_INTR2_CPU_STATUS_DSCL_0_INTR_SHIFT)
#define BCHP_INT_ID_FIELD_SYNC_656_0_INTR     BCHP_INT_ID_CREATE(BCHP_VIDEO_ENC_INTR2_CPU_STATUS, BCHP_VIDEO_ENC_INTR2_CPU_STATUS_FIELD_SYNC_656_0_INTR_SHIFT)
#define BCHP_INT_ID_FIELD_SYNC_656_1_INTR     BCHP_INT_ID_CREATE(BCHP_VIDEO_ENC_INTR2_CPU_STATUS, BCHP_VIDEO_ENC_INTR2_CPU_STATUS_FIELD_SYNC_656_1_INTR_SHIFT)
#define BCHP_INT_ID_FIELD_SYNC_DVI_0_INTR     BCHP_INT_ID_CREATE(BCHP_VIDEO_ENC_INTR2_CPU_STATUS, BCHP_VIDEO_ENC_INTR2_CPU_STATUS_FIELD_SYNC_DVI_0_INTR_SHIFT)
#define BCHP_INT_ID_FIELD_SYNC_VF_0_INTR      BCHP_INT_ID_CREATE(BCHP_VIDEO_ENC_INTR2_CPU_STATUS, BCHP_VIDEO_ENC_INTR2_CPU_STATUS_FIELD_SYNC_VF_0_INTR_SHIFT)
#define BCHP_INT_ID_FIELD_SYNC_VF_1_INTR      BCHP_INT_ID_CREATE(BCHP_VIDEO_ENC_INTR2_CPU_STATUS, BCHP_VIDEO_ENC_INTR2_CPU_STATUS_FIELD_SYNC_VF_1_INTR_SHIFT)
#define BCHP_INT_ID_VBI_0_0_INTR              BCHP_INT_ID_CREATE(BCHP_VIDEO_ENC_INTR2_CPU_STATUS, BCHP_VIDEO_ENC_INTR2_CPU_STATUS_VBI_0_0_INTR_SHIFT)
#define BCHP_INT_ID_VBI_0_1_INTR              BCHP_INT_ID_CREATE(BCHP_VIDEO_ENC_INTR2_CPU_STATUS, BCHP_VIDEO_ENC_INTR2_CPU_STATUS_VBI_0_1_INTR_SHIFT)
#define BCHP_INT_ID_VBI_1_0_INTR              BCHP_INT_ID_CREATE(BCHP_VIDEO_ENC_INTR2_CPU_STATUS, BCHP_VIDEO_ENC_INTR2_CPU_STATUS_VBI_1_0_INTR_SHIFT)
#define BCHP_INT_ID_VBI_1_1_INTR              BCHP_INT_ID_CREATE(BCHP_VIDEO_ENC_INTR2_CPU_STATUS, BCHP_VIDEO_ENC_INTR2_CPU_STATUS_VBI_1_1_INTR_SHIFT)

#endif /* #ifndef BCHP_INT_ID_VIDEO_ENC_INTR2_H__ */

/* End of File */