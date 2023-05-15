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
 * $brcm_Workfile: bchp_xpt_rmx1_io.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 8/17/11 4:17p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Thu Aug 11 21:41:46 2011
 *                 MD5 Checksum         30eb4d41dba6899590ea7664187e6ba5
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7429/rdb/a0/bchp_xpt_rmx1_io.h $
 * 
 * Hydra_Software_Devel/1   8/17/11 4:17p pntruong
 * SW7429-2: Initial version.
 *
 ***************************************************************************/

#ifndef BCHP_XPT_RMX1_IO_H__
#define BCHP_XPT_RMX1_IO_H__

/***************************************************************************
 *XPT_RMX1_IO - XPT RMX1 I/O Control Registers
 ***************************************************************************/
#define BCHP_XPT_RMX1_IO_FORMAT                  0x00200500 /* RMX Format Register */
#define BCHP_XPT_RMX1_IO_MISC_DLY_SEL            0x00200504 /* RMX Output Clk/Sync/Valid Delay Register */
#define BCHP_XPT_RMX1_IO_DATA_DLY_SEL            0x00200508 /* RMX Output Data Delay Register */
#define BCHP_XPT_RMX1_IO_PKT_DLY_CNT             0x0020050c /* RMX Packet Delay Control Register */
#define BCHP_XPT_RMX1_IO_TV_STATUS               0x00200520 /* TV Status */

#endif /* #ifndef BCHP_XPT_RMX1_IO_H__ */

/* End of File */