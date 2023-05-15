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
 * $brcm_Workfile: bchp_int_id_xpt_pcroffset.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 8/5/11 9:58p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Thu Aug  4 13:56:09 2011
 *                 MD5 Checksum         83f408cc25eb2d099cc58e22e4e239e9
 *
 * Compiled with:  RDB Utility          unknown
 *                 RDB Parser           3.0
 *                 generate_int_id.pl   1.0
 *                 Perl Interpreter     5.008005
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7552/rdb/b0/bchp_int_id_xpt_pcroffset.h $
 * 
 * Hydra_Software_Devel/1   8/5/11 9:58p albertl
 * SW7552-89: Initial revision.
 *
 ***************************************************************************/

#include "bchp.h"
#include "bchp_xpt_pcroffset.h"

#ifndef BCHP_INT_ID_XPT_PCROFFSET_H__
#define BCHP_INT_ID_XPT_PCROFFSET_H__

#define BCHP_INT_ID_CONTEXT0_PCR_DISCONT      BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS_CONTEXT0_PCR_DISCONT_SHIFT)
#define BCHP_INT_ID_CONTEXT0_PCR_NEW          BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS_CONTEXT0_PCR_NEW_SHIFT)
#define BCHP_INT_ID_CONTEXT0_PCR_ONE_ERROR    BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS_CONTEXT0_PCR_ONE_ERROR_SHIFT)
#define BCHP_INT_ID_CONTEXT0_PCR_TWO_ERROR    BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS_CONTEXT0_PCR_TWO_ERROR_SHIFT)
#define BCHP_INT_ID_CONTEXT0_SPLICE_DONE      BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS_CONTEXT0_SPLICE_DONE_SHIFT)
#define BCHP_INT_ID_CONTEXT0_SPLICE_ERROR     BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS_CONTEXT0_SPLICE_ERROR_SHIFT)
#define BCHP_INT_ID_CONTEXT0_STC_CAPTURED     BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS_CONTEXT0_STC_CAPTURED_SHIFT)
#define BCHP_INT_ID_CONTEXT10_PCR_DISCONT     BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS_CONTEXT10_PCR_DISCONT_SHIFT)
#define BCHP_INT_ID_CONTEXT10_PCR_NEW         BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS_CONTEXT10_PCR_NEW_SHIFT)
#define BCHP_INT_ID_CONTEXT10_PCR_ONE_ERROR   BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS_CONTEXT10_PCR_ONE_ERROR_SHIFT)
#define BCHP_INT_ID_CONTEXT10_PCR_TWO_ERROR   BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS_CONTEXT10_PCR_TWO_ERROR_SHIFT)
#define BCHP_INT_ID_CONTEXT10_SPLICE_DONE     BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS_CONTEXT10_SPLICE_DONE_SHIFT)
#define BCHP_INT_ID_CONTEXT10_SPLICE_ERROR    BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS_CONTEXT10_SPLICE_ERROR_SHIFT)
#define BCHP_INT_ID_CONTEXT10_STC_CAPTURED    BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS_CONTEXT10_STC_CAPTURED_SHIFT)
#define BCHP_INT_ID_CONTEXT11_PCR_DISCONT     BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS_CONTEXT11_PCR_DISCONT_SHIFT)
#define BCHP_INT_ID_CONTEXT11_PCR_NEW         BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS_CONTEXT11_PCR_NEW_SHIFT)
#define BCHP_INT_ID_CONTEXT11_PCR_ONE_ERROR   BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS_CONTEXT11_PCR_ONE_ERROR_SHIFT)
#define BCHP_INT_ID_CONTEXT11_PCR_TWO_ERROR   BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS_CONTEXT11_PCR_TWO_ERROR_SHIFT)
#define BCHP_INT_ID_CONTEXT11_SPLICE_DONE     BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS_CONTEXT11_SPLICE_DONE_SHIFT)
#define BCHP_INT_ID_CONTEXT11_SPLICE_ERROR    BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS_CONTEXT11_SPLICE_ERROR_SHIFT)
#define BCHP_INT_ID_CONTEXT11_STC_CAPTURED    BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS_CONTEXT11_STC_CAPTURED_SHIFT)
#define BCHP_INT_ID_CONTEXT12_PCR_DISCONT     BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS_CONTEXT12_PCR_DISCONT_SHIFT)
#define BCHP_INT_ID_CONTEXT12_PCR_NEW         BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS_CONTEXT12_PCR_NEW_SHIFT)
#define BCHP_INT_ID_CONTEXT12_PCR_ONE_ERROR   BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS_CONTEXT12_PCR_ONE_ERROR_SHIFT)
#define BCHP_INT_ID_CONTEXT12_PCR_TWO_ERROR   BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS_CONTEXT12_PCR_TWO_ERROR_SHIFT)
#define BCHP_INT_ID_CONTEXT12_SPLICE_DONE     BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS_CONTEXT12_SPLICE_DONE_SHIFT)
#define BCHP_INT_ID_CONTEXT12_SPLICE_ERROR    BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS_CONTEXT12_SPLICE_ERROR_SHIFT)
#define BCHP_INT_ID_CONTEXT12_STC_CAPTURED    BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS_CONTEXT12_STC_CAPTURED_SHIFT)
#define BCHP_INT_ID_CONTEXT13_PCR_DISCONT     BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS_CONTEXT13_PCR_DISCONT_SHIFT)
#define BCHP_INT_ID_CONTEXT13_PCR_NEW         BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS_CONTEXT13_PCR_NEW_SHIFT)
#define BCHP_INT_ID_CONTEXT13_PCR_ONE_ERROR   BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS_CONTEXT13_PCR_ONE_ERROR_SHIFT)
#define BCHP_INT_ID_CONTEXT13_PCR_TWO_ERROR   BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS_CONTEXT13_PCR_TWO_ERROR_SHIFT)
#define BCHP_INT_ID_CONTEXT13_SPLICE_DONE     BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS_CONTEXT13_SPLICE_DONE_SHIFT)
#define BCHP_INT_ID_CONTEXT13_SPLICE_ERROR    BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS_CONTEXT13_SPLICE_ERROR_SHIFT)
#define BCHP_INT_ID_CONTEXT13_STC_CAPTURED    BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS_CONTEXT13_STC_CAPTURED_SHIFT)
#define BCHP_INT_ID_CONTEXT14_PCR_DISCONT     BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS_CONTEXT14_PCR_DISCONT_SHIFT)
#define BCHP_INT_ID_CONTEXT14_PCR_NEW         BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS_CONTEXT14_PCR_NEW_SHIFT)
#define BCHP_INT_ID_CONTEXT14_PCR_ONE_ERROR   BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS_CONTEXT14_PCR_ONE_ERROR_SHIFT)
#define BCHP_INT_ID_CONTEXT14_PCR_TWO_ERROR   BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS_CONTEXT14_PCR_TWO_ERROR_SHIFT)
#define BCHP_INT_ID_CONTEXT14_SPLICE_DONE     BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS_CONTEXT14_SPLICE_DONE_SHIFT)
#define BCHP_INT_ID_CONTEXT14_SPLICE_ERROR    BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS_CONTEXT14_SPLICE_ERROR_SHIFT)
#define BCHP_INT_ID_CONTEXT14_STC_CAPTURED    BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS_CONTEXT14_STC_CAPTURED_SHIFT)
#define BCHP_INT_ID_CONTEXT15_PCR_DISCONT     BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS_CONTEXT15_PCR_DISCONT_SHIFT)
#define BCHP_INT_ID_CONTEXT15_PCR_NEW         BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS_CONTEXT15_PCR_NEW_SHIFT)
#define BCHP_INT_ID_CONTEXT15_PCR_ONE_ERROR   BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS_CONTEXT15_PCR_ONE_ERROR_SHIFT)
#define BCHP_INT_ID_CONTEXT15_PCR_TWO_ERROR   BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS_CONTEXT15_PCR_TWO_ERROR_SHIFT)
#define BCHP_INT_ID_CONTEXT15_SPLICE_DONE     BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS_CONTEXT15_SPLICE_DONE_SHIFT)
#define BCHP_INT_ID_CONTEXT15_SPLICE_ERROR    BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS_CONTEXT15_SPLICE_ERROR_SHIFT)
#define BCHP_INT_ID_CONTEXT15_STC_CAPTURED    BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS_CONTEXT15_STC_CAPTURED_SHIFT)
#define BCHP_INT_ID_CONTEXT1_PCR_DISCONT      BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS_CONTEXT1_PCR_DISCONT_SHIFT)
#define BCHP_INT_ID_CONTEXT1_PCR_NEW          BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS_CONTEXT1_PCR_NEW_SHIFT)
#define BCHP_INT_ID_CONTEXT1_PCR_ONE_ERROR    BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS_CONTEXT1_PCR_ONE_ERROR_SHIFT)
#define BCHP_INT_ID_CONTEXT1_PCR_TWO_ERROR    BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS_CONTEXT1_PCR_TWO_ERROR_SHIFT)
#define BCHP_INT_ID_CONTEXT1_SPLICE_DONE      BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS_CONTEXT1_SPLICE_DONE_SHIFT)
#define BCHP_INT_ID_CONTEXT1_SPLICE_ERROR     BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS_CONTEXT1_SPLICE_ERROR_SHIFT)
#define BCHP_INT_ID_CONTEXT1_STC_CAPTURED     BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS_CONTEXT1_STC_CAPTURED_SHIFT)
#define BCHP_INT_ID_CONTEXT2_PCR_DISCONT      BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS_CONTEXT2_PCR_DISCONT_SHIFT)
#define BCHP_INT_ID_CONTEXT2_PCR_NEW          BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS_CONTEXT2_PCR_NEW_SHIFT)
#define BCHP_INT_ID_CONTEXT2_PCR_ONE_ERROR    BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS_CONTEXT2_PCR_ONE_ERROR_SHIFT)
#define BCHP_INT_ID_CONTEXT2_PCR_TWO_ERROR    BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS_CONTEXT2_PCR_TWO_ERROR_SHIFT)
#define BCHP_INT_ID_CONTEXT2_SPLICE_DONE      BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS_CONTEXT2_SPLICE_DONE_SHIFT)
#define BCHP_INT_ID_CONTEXT2_SPLICE_ERROR     BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS_CONTEXT2_SPLICE_ERROR_SHIFT)
#define BCHP_INT_ID_CONTEXT2_STC_CAPTURED     BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS_CONTEXT2_STC_CAPTURED_SHIFT)
#define BCHP_INT_ID_CONTEXT3_PCR_DISCONT      BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS_CONTEXT3_PCR_DISCONT_SHIFT)
#define BCHP_INT_ID_CONTEXT3_PCR_NEW          BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS_CONTEXT3_PCR_NEW_SHIFT)
#define BCHP_INT_ID_CONTEXT3_PCR_ONE_ERROR    BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS_CONTEXT3_PCR_ONE_ERROR_SHIFT)
#define BCHP_INT_ID_CONTEXT3_PCR_TWO_ERROR    BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS_CONTEXT3_PCR_TWO_ERROR_SHIFT)
#define BCHP_INT_ID_CONTEXT3_SPLICE_DONE      BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS_CONTEXT3_SPLICE_DONE_SHIFT)
#define BCHP_INT_ID_CONTEXT3_SPLICE_ERROR     BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS_CONTEXT3_SPLICE_ERROR_SHIFT)
#define BCHP_INT_ID_CONTEXT3_STC_CAPTURED     BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS_CONTEXT3_STC_CAPTURED_SHIFT)
#define BCHP_INT_ID_CONTEXT4_PCR_DISCONT      BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS_CONTEXT4_PCR_DISCONT_SHIFT)
#define BCHP_INT_ID_CONTEXT4_PCR_NEW          BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS_CONTEXT4_PCR_NEW_SHIFT)
#define BCHP_INT_ID_CONTEXT4_PCR_ONE_ERROR    BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS_CONTEXT4_PCR_ONE_ERROR_SHIFT)
#define BCHP_INT_ID_CONTEXT4_PCR_TWO_ERROR    BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS_CONTEXT4_PCR_TWO_ERROR_SHIFT)
#define BCHP_INT_ID_CONTEXT4_SPLICE_DONE      BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS_CONTEXT4_SPLICE_DONE_SHIFT)
#define BCHP_INT_ID_CONTEXT4_SPLICE_ERROR     BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS_CONTEXT4_SPLICE_ERROR_SHIFT)
#define BCHP_INT_ID_CONTEXT4_STC_CAPTURED     BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS_CONTEXT4_STC_CAPTURED_SHIFT)
#define BCHP_INT_ID_CONTEXT5_PCR_DISCONT      BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS_CONTEXT5_PCR_DISCONT_SHIFT)
#define BCHP_INT_ID_CONTEXT5_PCR_NEW          BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS_CONTEXT5_PCR_NEW_SHIFT)
#define BCHP_INT_ID_CONTEXT5_PCR_ONE_ERROR    BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS_CONTEXT5_PCR_ONE_ERROR_SHIFT)
#define BCHP_INT_ID_CONTEXT5_PCR_TWO_ERROR    BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS_CONTEXT5_PCR_TWO_ERROR_SHIFT)
#define BCHP_INT_ID_CONTEXT5_SPLICE_DONE      BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS_CONTEXT5_SPLICE_DONE_SHIFT)
#define BCHP_INT_ID_CONTEXT5_SPLICE_ERROR     BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS_CONTEXT5_SPLICE_ERROR_SHIFT)
#define BCHP_INT_ID_CONTEXT5_STC_CAPTURED     BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS_CONTEXT5_STC_CAPTURED_SHIFT)
#define BCHP_INT_ID_CONTEXT6_PCR_DISCONT      BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS_CONTEXT6_PCR_DISCONT_SHIFT)
#define BCHP_INT_ID_CONTEXT6_PCR_NEW          BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS_CONTEXT6_PCR_NEW_SHIFT)
#define BCHP_INT_ID_CONTEXT6_PCR_ONE_ERROR    BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS_CONTEXT6_PCR_ONE_ERROR_SHIFT)
#define BCHP_INT_ID_CONTEXT6_PCR_TWO_ERROR    BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS_CONTEXT6_PCR_TWO_ERROR_SHIFT)
#define BCHP_INT_ID_CONTEXT6_SPLICE_DONE      BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS_CONTEXT6_SPLICE_DONE_SHIFT)
#define BCHP_INT_ID_CONTEXT6_SPLICE_ERROR     BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS_CONTEXT6_SPLICE_ERROR_SHIFT)
#define BCHP_INT_ID_CONTEXT6_STC_CAPTURED     BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS_CONTEXT6_STC_CAPTURED_SHIFT)
#define BCHP_INT_ID_CONTEXT7_PCR_DISCONT      BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS_CONTEXT7_PCR_DISCONT_SHIFT)
#define BCHP_INT_ID_CONTEXT7_PCR_NEW          BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS_CONTEXT7_PCR_NEW_SHIFT)
#define BCHP_INT_ID_CONTEXT7_PCR_ONE_ERROR    BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS_CONTEXT7_PCR_ONE_ERROR_SHIFT)
#define BCHP_INT_ID_CONTEXT7_PCR_TWO_ERROR    BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS_CONTEXT7_PCR_TWO_ERROR_SHIFT)
#define BCHP_INT_ID_CONTEXT7_SPLICE_DONE      BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS_CONTEXT7_SPLICE_DONE_SHIFT)
#define BCHP_INT_ID_CONTEXT7_SPLICE_ERROR     BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS_CONTEXT7_SPLICE_ERROR_SHIFT)
#define BCHP_INT_ID_CONTEXT7_STC_CAPTURED     BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS_CONTEXT7_STC_CAPTURED_SHIFT)
#define BCHP_INT_ID_CONTEXT8_PCR_DISCONT      BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS_CONTEXT8_PCR_DISCONT_SHIFT)
#define BCHP_INT_ID_CONTEXT8_PCR_NEW          BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS_CONTEXT8_PCR_NEW_SHIFT)
#define BCHP_INT_ID_CONTEXT8_PCR_ONE_ERROR    BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS_CONTEXT8_PCR_ONE_ERROR_SHIFT)
#define BCHP_INT_ID_CONTEXT8_PCR_TWO_ERROR    BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS_CONTEXT8_PCR_TWO_ERROR_SHIFT)
#define BCHP_INT_ID_CONTEXT8_SPLICE_DONE      BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS_CONTEXT8_SPLICE_DONE_SHIFT)
#define BCHP_INT_ID_CONTEXT8_SPLICE_ERROR     BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS_CONTEXT8_SPLICE_ERROR_SHIFT)
#define BCHP_INT_ID_CONTEXT8_STC_CAPTURED     BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS_CONTEXT8_STC_CAPTURED_SHIFT)
#define BCHP_INT_ID_CONTEXT9_PCR_DISCONT      BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS_CONTEXT9_PCR_DISCONT_SHIFT)
#define BCHP_INT_ID_CONTEXT9_PCR_NEW          BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS_CONTEXT9_PCR_NEW_SHIFT)
#define BCHP_INT_ID_CONTEXT9_PCR_ONE_ERROR    BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS_CONTEXT9_PCR_ONE_ERROR_SHIFT)
#define BCHP_INT_ID_CONTEXT9_PCR_TWO_ERROR    BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS_CONTEXT9_PCR_TWO_ERROR_SHIFT)
#define BCHP_INT_ID_CONTEXT9_SPLICE_DONE      BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS_CONTEXT9_SPLICE_DONE_SHIFT)
#define BCHP_INT_ID_CONTEXT9_SPLICE_ERROR     BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS_CONTEXT9_SPLICE_ERROR_SHIFT)
#define BCHP_INT_ID_CONTEXT9_STC_CAPTURED     BCHP_INT_ID_CREATE(BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS, BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS_CONTEXT9_STC_CAPTURED_SHIFT)

#endif /* #ifndef BCHP_INT_ID_XPT_PCROFFSET_H__ */

/* End of File */