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
 * $brcm_Workfile: bchp_mmscram.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 1/24/12 4:45p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Mon Jan 23 14:23:48 2012
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
 * $brcm_Log: /magnum/basemodules/chp/7231/rdb/b2/bchp_mmscram.h $
 * 
 * Hydra_Software_Devel/1   1/24/12 4:45p katrep
 * SW7231-1:rdb header files for 7231B2
 *
 ***************************************************************************/

#ifndef BCHP_MMSCRAM_H__
#define BCHP_MMSCRAM_H__

/***************************************************************************
 *MMSCRAM - Memory Scrambler Secure Control Registers
 ***************************************************************************/

/***************************************************************************
 *MMSCRAM%i - Data Memory Address 0..2047
 ***************************************************************************/
#define BCHP_MMSCRAM_MMSCRAMi_ARRAY_BASE                           0x00370000
#define BCHP_MMSCRAM_MMSCRAMi_ARRAY_START                          0
#define BCHP_MMSCRAM_MMSCRAMi_ARRAY_END                            2047
#define BCHP_MMSCRAM_MMSCRAMi_ARRAY_ELEMENT_SIZE                   32

/***************************************************************************
 *MMSCRAM%i - Data Memory Address 0..2047
 ***************************************************************************/
/* MMSCRAM :: MMSCRAMi :: DATA [31:00] */
#define BCHP_MMSCRAM_MMSCRAMi_DATA_MASK                            0xffffffff
#define BCHP_MMSCRAM_MMSCRAMi_DATA_SHIFT                           0


#endif /* #ifndef BCHP_MMSCRAM_H__ */

/* End of File */