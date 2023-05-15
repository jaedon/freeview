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
 * $brcm_Workfile: bchp_bsp_cmdbuf.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 1/25/10 7:31p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Fri Jan 22 20:30:42 2010
 *                 MD5 Checksum         a2d1f2163f65e87d228a0fb491cb442d
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7125/rdb/c0/bchp_bsp_cmdbuf.h $
 * 
 * Hydra_Software_Devel/1   1/25/10 7:31p albertl
 * SW7125-177: Initial revision.
 *
 ***************************************************************************/

#ifndef BCHP_BSP_CMDBUF_H__
#define BCHP_BSP_CMDBUF_H__

/***************************************************************************
 *BSP_CMDBUF - BSP Command Buffer
 ***************************************************************************/

/***************************************************************************
 *DMEM%i - Data Memory Address 0..511
 ***************************************************************************/
#define BCHP_BSP_CMDBUF_DMEMi_ARRAY_BASE                           0x00327800
#define BCHP_BSP_CMDBUF_DMEMi_ARRAY_START                          0
#define BCHP_BSP_CMDBUF_DMEMi_ARRAY_END                            511
#define BCHP_BSP_CMDBUF_DMEMi_ARRAY_ELEMENT_SIZE                   32

/***************************************************************************
 *DMEM%i - Data Memory Address 0..511
 ***************************************************************************/
/* BSP_CMDBUF :: DMEMi :: DATA [31:00] */
#define BCHP_BSP_CMDBUF_DMEMi_DATA_MASK                            0xffffffff
#define BCHP_BSP_CMDBUF_DMEMi_DATA_SHIFT                           0


#endif /* #ifndef BCHP_BSP_CMDBUF_H__ */

/* End of File */