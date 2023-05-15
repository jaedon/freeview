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
 * $brcm_Workfile: bchp_moca_secure.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 1/25/10 9:08p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Fri Jan 22 20:20:47 2010
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
 * $brcm_Log: /magnum/basemodules/chp/7125/rdb/c0/bchp_moca_secure.h $
 * 
 * Hydra_Software_Devel/1   1/25/10 9:08p albertl
 * SW7125-177: Initial revision.
 *
 ***************************************************************************/

#ifndef BCHP_MOCA_SECURE_H__
#define BCHP_MOCA_SECURE_H__

/***************************************************************************
 *MOCA_SECURE - Secure GISB Registers
 ***************************************************************************/
#define BCHP_MOCA_SECURE_PAGE_M2M_CTL            0x00310000 /* PageRAM M2M Control Register */

/***************************************************************************
 *PAGE_M2M_CTL - PageRAM M2M Control Register
 ***************************************************************************/
/* MOCA_SECURE :: PAGE_M2M_CTL :: reserved0 [31:01] */
#define BCHP_MOCA_SECURE_PAGE_M2M_CTL_reserved0_MASK               0xfffffffe
#define BCHP_MOCA_SECURE_PAGE_M2M_CTL_reserved0_SHIFT              1

/* MOCA_SECURE :: PAGE_M2M_CTL :: PageM2MDisa [00:00] */
#define BCHP_MOCA_SECURE_PAGE_M2M_CTL_PageM2MDisa_MASK             0x00000001
#define BCHP_MOCA_SECURE_PAGE_M2M_CTL_PageM2MDisa_SHIFT            0

#endif /* #ifndef BCHP_MOCA_SECURE_H__ */

/* End of File */