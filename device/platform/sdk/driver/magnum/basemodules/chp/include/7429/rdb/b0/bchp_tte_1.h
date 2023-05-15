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
 * $brcm_Workfile: bchp_tte_1.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 6/14/12 2:44p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Wed Jun 13 16:39:03 2012
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
 * $brcm_Log: /magnum/basemodules/chp/7429/rdb/b0/bchp_tte_1.h $
 * 
 * Hydra_Software_Devel/1   6/14/12 2:44p pntruong
 * SW7429-185: Initial version.
 *
 ***************************************************************************/

#ifndef BCHP_TTE_1_H__
#define BCHP_TTE_1_H__

/***************************************************************************
 *TTE_1 - TTE_1 registers
 ***************************************************************************/
#define BCHP_TTE_1_status                        0x00688b04 /* teletext_status Register */
#define BCHP_TTE_1_control                       0x00688b08 /* teletext_control Register */
#define BCHP_TTE_1_lines_active                  0x00688b0c /* teletext_lines_active Register */
#define BCHP_TTE_1_read_address_top              0x00688b10 /* Base Address of top Teletext buffer */
#define BCHP_TTE_1_read_address_bottom           0x00688b14 /* Base Address of bottom Teletext buffer */
#define BCHP_TTE_1_output_format                 0x00688b1c /* teletext_output_format Register */
#define BCHP_TTE_1_top_mask                      0x00688b20 /* Teletext Top Mask Register */
#define BCHP_TTE_1_bottom_mask                   0x00688b24 /* Teletext Top Mask Register */
#define BCHP_TTE_1_revid                         0x00688b28 /* Teletext Encoder Revision ID Register */

#endif /* #ifndef BCHP_TTE_1_H__ */

/* End of File */