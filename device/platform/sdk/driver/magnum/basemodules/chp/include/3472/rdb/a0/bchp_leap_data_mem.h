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
 * $brcm_Workfile: bchp_leap_data_mem.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 3/29/12 3:31p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Thu Mar 29 14:38:30 2012
 *                 MD5 Checksum         d41d8cd98f00b204e9800998ecf8427e
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/3472/rdb/a0/bchp_leap_data_mem.h $
 * 
 * Hydra_Software_Devel/1   3/29/12 3:31p farshidf
 * SW3461-1: add 3472 header files
 *
 ***************************************************************************/

#ifndef BCHP_LEAP_DATA_MEM_H__
#define BCHP_LEAP_DATA_MEM_H__

/***************************************************************************
 *LEAP_DATA_MEM - CPU Data Memory
 ***************************************************************************/

/***************************************************************************
 *WORD%i - Data RAM
 ***************************************************************************/
#define BCHP_LEAP_DATA_MEM_WORDi_ARRAY_BASE                        0x00040000
#define BCHP_LEAP_DATA_MEM_WORDi_ARRAY_START                       0
#define BCHP_LEAP_DATA_MEM_WORDi_ARRAY_END                         32767
#define BCHP_LEAP_DATA_MEM_WORDi_ARRAY_ELEMENT_SIZE                32

/***************************************************************************
 *WORD%i - Data RAM
 ***************************************************************************/
/* LEAP_DATA_MEM :: WORDi :: WORD [31:00] */
#define BCHP_LEAP_DATA_MEM_WORDi_WORD_MASK                         0xffffffff
#define BCHP_LEAP_DATA_MEM_WORDi_WORD_SHIFT                        0


#endif /* #ifndef BCHP_LEAP_DATA_MEM_H__ */

/* End of File */