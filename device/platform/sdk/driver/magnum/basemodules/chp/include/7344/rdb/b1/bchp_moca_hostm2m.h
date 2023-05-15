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
 * $brcm_Workfile: bchp_moca_hostm2m.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 4/4/11 1:58p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Fri Apr  1 16:32:41 2011
 *                 MD5 Checksum         d03d08c4839c3311c9d35c4cd5e10373
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7344/rdb/b0/bchp_moca_hostm2m.h $
 * 
 * Hydra_Software_Devel/1   4/4/11 1:58p albertl
 * SW7344-40: Initial revision.
 *
 ***************************************************************************/

#ifndef BCHP_MOCA_HOSTM2M_H__
#define BCHP_MOCA_HOSTM2M_H__

/***************************************************************************
 *MOCA_HOSTM2M - MOCA_HOSTM2M registers
 ***************************************************************************/
#define BCHP_MOCA_HOSTM2M_SRC_ADDR               0x002a2000 /* Mem-to-Mem Source Address Register */
#define BCHP_MOCA_HOSTM2M_DEST_ADDR              0x002a2004 /* Mem-to-Mem Destination Address Register */
#define BCHP_MOCA_HOSTM2M_CMD                    0x002a2008 /* Mem-to-Mem Command Register */
#define BCHP_MOCA_HOSTM2M_STATUS                 0x002a200c /* Mem-to-Mem Status Register */

/***************************************************************************
 *SRC_ADDR - Mem-to-Mem Source Address Register
 ***************************************************************************/
/* MOCA_HOSTM2M :: SRC_ADDR :: mem2mem_src_addr [31:00] */
#define BCHP_MOCA_HOSTM2M_SRC_ADDR_mem2mem_src_addr_MASK           0xffffffff
#define BCHP_MOCA_HOSTM2M_SRC_ADDR_mem2mem_src_addr_SHIFT          0
#define BCHP_MOCA_HOSTM2M_SRC_ADDR_mem2mem_src_addr_DEFAULT        0

/***************************************************************************
 *DEST_ADDR - Mem-to-Mem Destination Address Register
 ***************************************************************************/
/* MOCA_HOSTM2M :: DEST_ADDR :: mem2mem_dest_addr [31:00] */
#define BCHP_MOCA_HOSTM2M_DEST_ADDR_mem2mem_dest_addr_MASK         0xffffffff
#define BCHP_MOCA_HOSTM2M_DEST_ADDR_mem2mem_dest_addr_SHIFT        0
#define BCHP_MOCA_HOSTM2M_DEST_ADDR_mem2mem_dest_addr_DEFAULT      0

/***************************************************************************
 *CMD - Mem-to-Mem Command Register
 ***************************************************************************/
/* MOCA_HOSTM2M :: CMD :: src_scb [31:31] */
#define BCHP_MOCA_HOSTM2M_CMD_src_scb_MASK                         0x80000000
#define BCHP_MOCA_HOSTM2M_CMD_src_scb_SHIFT                        31
#define BCHP_MOCA_HOSTM2M_CMD_src_scb_DEFAULT                      0

/* MOCA_HOSTM2M :: CMD :: dest_scb [30:30] */
#define BCHP_MOCA_HOSTM2M_CMD_dest_scb_MASK                        0x40000000
#define BCHP_MOCA_HOSTM2M_CMD_dest_scb_SHIFT                       30
#define BCHP_MOCA_HOSTM2M_CMD_dest_scb_DEFAULT                     0

/* MOCA_HOSTM2M :: CMD :: swap_data [29:28] */
#define BCHP_MOCA_HOSTM2M_CMD_swap_data_MASK                       0x30000000
#define BCHP_MOCA_HOSTM2M_CMD_swap_data_SHIFT                      28
#define BCHP_MOCA_HOSTM2M_CMD_swap_data_DEFAULT                    0

/* MOCA_HOSTM2M :: CMD :: update_status [27:27] */
#define BCHP_MOCA_HOSTM2M_CMD_update_status_MASK                   0x08000000
#define BCHP_MOCA_HOSTM2M_CMD_update_status_SHIFT                  27
#define BCHP_MOCA_HOSTM2M_CMD_update_status_DEFAULT                0

/* MOCA_HOSTM2M :: CMD :: reserved0 [26:19] */
#define BCHP_MOCA_HOSTM2M_CMD_reserved0_MASK                       0x07f80000
#define BCHP_MOCA_HOSTM2M_CMD_reserved0_SHIFT                      19

/* MOCA_HOSTM2M :: CMD :: length [18:00] */
#define BCHP_MOCA_HOSTM2M_CMD_length_MASK                          0x0007ffff
#define BCHP_MOCA_HOSTM2M_CMD_length_SHIFT                         0
#define BCHP_MOCA_HOSTM2M_CMD_length_DEFAULT                       0

/***************************************************************************
 *STATUS - Mem-to-Mem Status Register
 ***************************************************************************/
/* MOCA_HOSTM2M :: STATUS :: done [31:31] */
#define BCHP_MOCA_HOSTM2M_STATUS_done_MASK                         0x80000000
#define BCHP_MOCA_HOSTM2M_STATUS_done_SHIFT                        31
#define BCHP_MOCA_HOSTM2M_STATUS_done_DEFAULT                      0

/* MOCA_HOSTM2M :: STATUS :: mismatch [30:30] */
#define BCHP_MOCA_HOSTM2M_STATUS_mismatch_MASK                     0x40000000
#define BCHP_MOCA_HOSTM2M_STATUS_mismatch_SHIFT                    30
#define BCHP_MOCA_HOSTM2M_STATUS_mismatch_DEFAULT                  0

/* MOCA_HOSTM2M :: STATUS :: error [29:29] */
#define BCHP_MOCA_HOSTM2M_STATUS_error_MASK                        0x20000000
#define BCHP_MOCA_HOSTM2M_STATUS_error_SHIFT                       29
#define BCHP_MOCA_HOSTM2M_STATUS_error_DEFAULT                     0

/* MOCA_HOSTM2M :: STATUS :: reserved_for_eco0 [28:19] */
#define BCHP_MOCA_HOSTM2M_STATUS_reserved_for_eco0_MASK            0x1ff80000
#define BCHP_MOCA_HOSTM2M_STATUS_reserved_for_eco0_SHIFT           19
#define BCHP_MOCA_HOSTM2M_STATUS_reserved_for_eco0_DEFAULT         0

/* MOCA_HOSTM2M :: STATUS :: status_length [18:00] */
#define BCHP_MOCA_HOSTM2M_STATUS_status_length_MASK                0x0007ffff
#define BCHP_MOCA_HOSTM2M_STATUS_status_length_SHIFT               0
#define BCHP_MOCA_HOSTM2M_STATUS_status_length_DEFAULT             0

#endif /* #ifndef BCHP_MOCA_HOSTM2M_H__ */

/* End of File */