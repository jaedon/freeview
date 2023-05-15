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
 * $brcm_Workfile: bchp_sid_bigram.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 4/14/11 5:40p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Wed Apr 13 13:16:10 2011
 *                 MD5 Checksum         c1b53a9408066d1e6123c0860dfb8065
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7346/rdb/b0/bchp_sid_bigram.h $
 * 
 * Hydra_Software_Devel/1   4/14/11 5:40p albertl
 * SW7346-143: Initial revision.
 *
 ***************************************************************************/

#ifndef BCHP_SID_BIGRAM_H__
#define BCHP_SID_BIGRAM_H__

/***************************************************************************
 *SID_BIGRAM - Big Ram
 ***************************************************************************/
#define BCHP_SID_BIGRAM_START_ADDR               0x00448000 /* Start of RAM */
#define BCHP_SID_BIGRAM_END_ADDR                 0x0044fffc /* End of RAM */

/***************************************************************************
 *START_ADDR - Start of RAM
 ***************************************************************************/
/* SID_BIGRAM :: START_ADDR :: DATA [31:00] */
#define BCHP_SID_BIGRAM_START_ADDR_DATA_MASK                       0xffffffff
#define BCHP_SID_BIGRAM_START_ADDR_DATA_SHIFT                      0

/***************************************************************************
 *END_ADDR - End of RAM
 ***************************************************************************/
/* SID_BIGRAM :: END_ADDR :: DATA [31:00] */
#define BCHP_SID_BIGRAM_END_ADDR_DATA_MASK                         0xffffffff
#define BCHP_SID_BIGRAM_END_ADDR_DATA_SHIFT                        0

#endif /* #ifndef BCHP_SID_BIGRAM_H__ */

/* End of File */