/***************************************************************************
 *     Copyright (c) 1999-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bchp_hfb_ram_0.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 6/23/10 3:23p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Mon Jun 21 20:07:19 2010
 *                 MD5 Checksum         ca6a65ea070ab31476b927e4308136d1
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7125/rdb/c0/bchp_hfb_ram_0.h $
 * 
 * Hydra_Software_Devel/1   6/23/10 3:23p albertl
 * SW7125-1: Updated to match RDB.
 *
 ***************************************************************************/

#ifndef BCHP_HFB_RAM_0_H__
#define BCHP_HFB_RAM_0_H__

/***************************************************************************
 *HFB_RAM_0 - HFB RAM Memory 0
 ***************************************************************************/

/***************************************************************************
 *hfb%i - hfb_word 0..1023
 ***************************************************************************/
#define BCHP_HFB_RAM_0_hfbi_ARRAY_BASE                             0x02031000
#define BCHP_HFB_RAM_0_hfbi_ARRAY_START                            0
#define BCHP_HFB_RAM_0_hfbi_ARRAY_END                              1023
#define BCHP_HFB_RAM_0_hfbi_ARRAY_ELEMENT_SIZE                     32

/***************************************************************************
 *hfb%i - hfb_word 0..1023
 ***************************************************************************/
/* HFB_RAM_0 :: hfbi :: reserved0 [31:20] */
#define BCHP_HFB_RAM_0_hfbi_reserved0_MASK                         0xfff00000
#define BCHP_HFB_RAM_0_hfbi_reserved0_SHIFT                        20

/* HFB_RAM_0 :: hfbi :: hfb_mask [19:16] */
#define BCHP_HFB_RAM_0_hfbi_hfb_mask_MASK                          0x000f0000
#define BCHP_HFB_RAM_0_hfbi_hfb_mask_SHIFT                         16

/* HFB_RAM_0 :: hfbi :: hdb_pattern [15:00] */
#define BCHP_HFB_RAM_0_hfbi_hdb_pattern_MASK                       0x0000ffff
#define BCHP_HFB_RAM_0_hfbi_hdb_pattern_SHIFT                      0


#endif /* #ifndef BCHP_HFB_RAM_0_H__ */

/* End of File */