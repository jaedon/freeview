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
 * $brcm_Workfile: bchp_sata_port1_leg_s2.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 10/11/11 8:46p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Tue Oct 11 11:26:21 2011
 *                 MD5 Checksum         12f5dd38e0ec050efd2afb0b1c62e9d9
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7435/rdb/a0/bchp_sata_port1_leg_s2.h $
 * 
 * Hydra_Software_Devel/1   10/11/11 8:46p pntruong
 * SW7435-3: Synced up with central rdb.
 *
 ***************************************************************************/

#ifndef BCHP_SATA_PORT1_LEG_S2_H__
#define BCHP_SATA_PORT1_LEG_S2_H__

/***************************************************************************
 *SATA_PORT1_LEG_S2 - Legacy Port1 section 2 Register
 ***************************************************************************/
#define BCHP_SATA_PORT1_LEG_S2_SATA_STATUS       0x00181a40 /* SATA Status Register(SCR0) */
#define BCHP_SATA_PORT1_LEG_S2_SATA_ERROR        0x00181a44 /* SATA Error Register(SCR1) */
#define BCHP_SATA_PORT1_LEG_S2_SATA_CONTROL      0x00181a48 /* SATA Control Register(SCR2) */
#define BCHP_SATA_PORT1_LEG_S2_NOTIFY            0x00181a50 /* SATA NOTIFY REGISTER */
#define BCHP_SATA_PORT1_LEG_S2_SPARE0            0x00181a54 /* SATA SPARE REGISTER */

#endif /* #ifndef BCHP_SATA_PORT1_LEG_S2_H__ */

/* End of File */