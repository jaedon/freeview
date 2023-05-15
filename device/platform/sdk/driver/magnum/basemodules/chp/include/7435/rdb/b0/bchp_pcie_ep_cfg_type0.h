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
 * $brcm_Workfile: bchp_pcie_ep_cfg_type0.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 2/28/12 5:39p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Tue Feb 28 11:03:21 2012
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
 * $brcm_Log: /magnum/basemodules/chp/7435/rdb/b0/bchp_pcie_ep_cfg_type0.h $
 * 
 * Hydra_Software_Devel/1   2/28/12 5:39p tdo
 * SW7435-40: Need 7435B0 public/central RDB (magnum) headers checked into
 * clearcase
 *
 ***************************************************************************/

#ifndef BCHP_PCIE_EP_CFG_TYPE0_H__
#define BCHP_PCIE_EP_CFG_TYPE0_H__

/***************************************************************************
 *PCIE_EP_CFG_TYPE0
 ***************************************************************************/
#define BCHP_PCIE_EP_CFG_TYPE0_DEVICE_VENDOR_ID  0x00412000 /* device_vendor_id */
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND    0x00412004 /* status_command */
#define BCHP_PCIE_EP_CFG_TYPE0_REV_ID_CLASS_CODE 0x00412008 /* rev_id_class_code */
#define BCHP_PCIE_EP_CFG_TYPE0_HEADERTYPE_LAT_CACHELINESIZE 0x0041200c /* headertype_lat_cachelinesize */
#define BCHP_PCIE_EP_CFG_TYPE0_BAR_1             0x00412010 /* bar_1 */
#define BCHP_PCIE_EP_CFG_TYPE0_BAR_2             0x00412014 /* bar_2 */
#define BCHP_PCIE_EP_CFG_TYPE0_BAR_3             0x00412018 /* bar_3 */
#define BCHP_PCIE_EP_CFG_TYPE0_BAR_4             0x0041201c /* bar_4 */
#define BCHP_PCIE_EP_CFG_TYPE0_BAR_5             0x00412020 /* bar_5 */
#define BCHP_PCIE_EP_CFG_TYPE0_BAR_6             0x00412024 /* bar_6 */
#define BCHP_PCIE_EP_CFG_TYPE0_CARDBUS_CIS       0x00412028 /* cardbus_cis */
#define BCHP_PCIE_EP_CFG_TYPE0_SUBSYSTEM_ID_VENDOR_ID 0x0041202c /* subsystem_id_vendor_id */
#define BCHP_PCIE_EP_CFG_TYPE0_EXP_ROM_BAR       0x00412030 /* exp_rom_bar */
#define BCHP_PCIE_EP_CFG_TYPE0_CAP_POINTER       0x00412034 /* cap_pointer */
#define BCHP_PCIE_EP_CFG_TYPE0_RESERVED          0x00412038 /* Reserved */
#define BCHP_PCIE_EP_CFG_TYPE0_LAT_MIN_GRANT_INT_PIN_INT_LINE 0x0041203c /* lat_min_grant_int_pin_int_line */

/***************************************************************************
 *DEVICE_VENDOR_ID - device_vendor_id
 ***************************************************************************/
/* PCIE_EP_CFG_TYPE0 :: DEVICE_VENDOR_ID :: DEVICE_ID [31:16] */
#define BCHP_PCIE_EP_CFG_TYPE0_DEVICE_VENDOR_ID_DEVICE_ID_MASK     0xffff0000
#define BCHP_PCIE_EP_CFG_TYPE0_DEVICE_VENDOR_ID_DEVICE_ID_SHIFT    16

/* PCIE_EP_CFG_TYPE0 :: DEVICE_VENDOR_ID :: VENDOR_ID [15:00] */
#define BCHP_PCIE_EP_CFG_TYPE0_DEVICE_VENDOR_ID_VENDOR_ID_MASK     0x0000ffff
#define BCHP_PCIE_EP_CFG_TYPE0_DEVICE_VENDOR_ID_VENDOR_ID_SHIFT    0
#define BCHP_PCIE_EP_CFG_TYPE0_DEVICE_VENDOR_ID_VENDOR_ID_DEFAULT  0x000014e4

/***************************************************************************
 *STATUS_COMMAND - status_command
 ***************************************************************************/
/* PCIE_EP_CFG_TYPE0 :: STATUS_COMMAND :: PRI_PAR_ERR [31:31] */
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_PRI_PAR_ERR_MASK     0x80000000
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_PRI_PAR_ERR_SHIFT    31
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_PRI_PAR_ERR_DEFAULT  0x00000000

/* PCIE_EP_CFG_TYPE0 :: STATUS_COMMAND :: PRI_SIG_SERR [30:30] */
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_PRI_SIG_SERR_MASK    0x40000000
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_PRI_SIG_SERR_SHIFT   30
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_PRI_SIG_SERR_DEFAULT 0x00000000

/* PCIE_EP_CFG_TYPE0 :: STATUS_COMMAND :: PRI_RCV_MSTR_ABT [29:29] */
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_PRI_RCV_MSTR_ABT_MASK 0x20000000
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_PRI_RCV_MSTR_ABT_SHIFT 29
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_PRI_RCV_MSTR_ABT_DEFAULT 0x00000000

/* PCIE_EP_CFG_TYPE0 :: STATUS_COMMAND :: PRI_RCV_TGT_ABT [28:28] */
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_PRI_RCV_TGT_ABT_MASK 0x10000000
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_PRI_RCV_TGT_ABT_SHIFT 28
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_PRI_RCV_TGT_ABT_DEFAULT 0x00000000

/* PCIE_EP_CFG_TYPE0 :: STATUS_COMMAND :: PRI_SIG_TGT_ABT [27:27] */
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_PRI_SIG_TGT_ABT_MASK 0x08000000
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_PRI_SIG_TGT_ABT_SHIFT 27
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_PRI_SIG_TGT_ABT_DEFAULT 0x00000000

/* PCIE_EP_CFG_TYPE0 :: STATUS_COMMAND :: DEVSEL_TIMING [26:25] */
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_DEVSEL_TIMING_MASK   0x06000000
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_DEVSEL_TIMING_SHIFT  25
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_DEVSEL_TIMING_DEFAULT 0x00000000

/* PCIE_EP_CFG_TYPE0 :: STATUS_COMMAND :: PRI_MSTR_PERR [24:24] */
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_PRI_MSTR_PERR_MASK   0x01000000
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_PRI_MSTR_PERR_SHIFT  24
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_PRI_MSTR_PERR_DEFAULT 0x00000000

/* PCIE_EP_CFG_TYPE0 :: STATUS_COMMAND :: FAST_B2B_CAP [23:23] */
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_FAST_B2B_CAP_MASK    0x00800000
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_FAST_B2B_CAP_SHIFT   23
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_FAST_B2B_CAP_DEFAULT 0x00000000

/* PCIE_EP_CFG_TYPE0 :: STATUS_COMMAND :: RESERVED2 [22:22] */
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_RESERVED2_MASK       0x00400000
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_RESERVED2_SHIFT      22
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_RESERVED2_DEFAULT    0x00000000

/* PCIE_EP_CFG_TYPE0 :: STATUS_COMMAND :: CAP_66MHZ [21:21] */
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_CAP_66MHZ_MASK       0x00200000
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_CAP_66MHZ_SHIFT      21
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_CAP_66MHZ_DEFAULT    0x00000000

/* PCIE_EP_CFG_TYPE0 :: STATUS_COMMAND :: CAP_LIST [20:20] */
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_CAP_LIST_MASK        0x00100000
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_CAP_LIST_SHIFT       20
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_CAP_LIST_DEFAULT     0x00000001

/* PCIE_EP_CFG_TYPE0 :: STATUS_COMMAND :: INT_STATUS [19:19] */
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_INT_STATUS_MASK      0x00080000
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_INT_STATUS_SHIFT     19
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_INT_STATUS_DEFAULT   0x00000000

/* PCIE_EP_CFG_TYPE0 :: STATUS_COMMAND :: RESERVED1 [18:16] */
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_RESERVED1_MASK       0x00070000
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_RESERVED1_SHIFT      16
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_RESERVED1_DEFAULT    0x00000000

/* PCIE_EP_CFG_TYPE0 :: STATUS_COMMAND :: RESERVED [15:11] */
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_RESERVED_MASK        0x0000f800
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_RESERVED_SHIFT       11
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_RESERVED_DEFAULT     0x00000000

/* PCIE_EP_CFG_TYPE0 :: STATUS_COMMAND :: INT_DISABLE [10:10] */
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_INT_DISABLE_MASK     0x00000400
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_INT_DISABLE_SHIFT    10
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_INT_DISABLE_DEFAULT  0x00000000

/* PCIE_EP_CFG_TYPE0 :: STATUS_COMMAND :: FAST_B2B [09:09] */
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_FAST_B2B_MASK        0x00000200
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_FAST_B2B_SHIFT       9
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_FAST_B2B_DEFAULT     0x00000000

/* PCIE_EP_CFG_TYPE0 :: STATUS_COMMAND :: SERR_ENA [08:08] */
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_SERR_ENA_MASK        0x00000100
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_SERR_ENA_SHIFT       8
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_SERR_ENA_DEFAULT     0x00000000

/* PCIE_EP_CFG_TYPE0 :: STATUS_COMMAND :: STEPPING [07:07] */
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_STEPPING_MASK        0x00000080
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_STEPPING_SHIFT       7
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_STEPPING_DEFAULT     0x00000000

/* PCIE_EP_CFG_TYPE0 :: STATUS_COMMAND :: PERR_ENA [06:06] */
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_PERR_ENA_MASK        0x00000040
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_PERR_ENA_SHIFT       6
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_PERR_ENA_DEFAULT     0x00000000

/* PCIE_EP_CFG_TYPE0 :: STATUS_COMMAND :: VGA_SNOOP [05:05] */
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_VGA_SNOOP_MASK       0x00000020
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_VGA_SNOOP_SHIFT      5
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_VGA_SNOOP_DEFAULT    0x00000000

/* PCIE_EP_CFG_TYPE0 :: STATUS_COMMAND :: MWI_CYCLES [04:04] */
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_MWI_CYCLES_MASK      0x00000010
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_MWI_CYCLES_SHIFT     4
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_MWI_CYCLES_DEFAULT   0x00000000

/* PCIE_EP_CFG_TYPE0 :: STATUS_COMMAND :: SPECIAL_CYCLES [03:03] */
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_SPECIAL_CYCLES_MASK  0x00000008
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_SPECIAL_CYCLES_SHIFT 3
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_SPECIAL_CYCLES_DEFAULT 0x00000000

/* PCIE_EP_CFG_TYPE0 :: STATUS_COMMAND :: BUS_MASTER [02:02] */
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_BUS_MASTER_MASK      0x00000004
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_BUS_MASTER_SHIFT     2
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_BUS_MASTER_DEFAULT   0x00000000

/* PCIE_EP_CFG_TYPE0 :: STATUS_COMMAND :: MEM_SPACE [01:01] */
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_MEM_SPACE_MASK       0x00000002
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_MEM_SPACE_SHIFT      1
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_MEM_SPACE_DEFAULT    0x00000000

/* PCIE_EP_CFG_TYPE0 :: STATUS_COMMAND :: IO_SPACE [00:00] */
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_IO_SPACE_MASK        0x00000001
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_IO_SPACE_SHIFT       0
#define BCHP_PCIE_EP_CFG_TYPE0_STATUS_COMMAND_IO_SPACE_DEFAULT     0x00000000

/***************************************************************************
 *REV_ID_CLASS_CODE - rev_id_class_code
 ***************************************************************************/
/* PCIE_EP_CFG_TYPE0 :: REV_ID_CLASS_CODE :: CLASS_CODE [31:08] */
#define BCHP_PCIE_EP_CFG_TYPE0_REV_ID_CLASS_CODE_CLASS_CODE_MASK   0xffffff00
#define BCHP_PCIE_EP_CFG_TYPE0_REV_ID_CLASS_CODE_CLASS_CODE_SHIFT  8
#define BCHP_PCIE_EP_CFG_TYPE0_REV_ID_CLASS_CODE_CLASS_CODE_DEFAULT 0x00020000

/* PCIE_EP_CFG_TYPE0 :: REV_ID_CLASS_CODE :: REV_ID [07:00] */
#define BCHP_PCIE_EP_CFG_TYPE0_REV_ID_CLASS_CODE_REV_ID_MASK       0x000000ff
#define BCHP_PCIE_EP_CFG_TYPE0_REV_ID_CLASS_CODE_REV_ID_SHIFT      0
#define BCHP_PCIE_EP_CFG_TYPE0_REV_ID_CLASS_CODE_REV_ID_DEFAULT    0x00000000

/***************************************************************************
 *HEADERTYPE_LAT_CACHELINESIZE - headertype_lat_cachelinesize
 ***************************************************************************/
/* PCIE_EP_CFG_TYPE0 :: HEADERTYPE_LAT_CACHELINESIZE :: BIST [31:24] */
#define BCHP_PCIE_EP_CFG_TYPE0_HEADERTYPE_LAT_CACHELINESIZE_BIST_MASK 0xff000000
#define BCHP_PCIE_EP_CFG_TYPE0_HEADERTYPE_LAT_CACHELINESIZE_BIST_SHIFT 24
#define BCHP_PCIE_EP_CFG_TYPE0_HEADERTYPE_LAT_CACHELINESIZE_BIST_DEFAULT 0x00000000

/* PCIE_EP_CFG_TYPE0 :: HEADERTYPE_LAT_CACHELINESIZE :: HEADER_TYPE [23:16] */
#define BCHP_PCIE_EP_CFG_TYPE0_HEADERTYPE_LAT_CACHELINESIZE_HEADER_TYPE_MASK 0x00ff0000
#define BCHP_PCIE_EP_CFG_TYPE0_HEADERTYPE_LAT_CACHELINESIZE_HEADER_TYPE_SHIFT 16
#define BCHP_PCIE_EP_CFG_TYPE0_HEADERTYPE_LAT_CACHELINESIZE_HEADER_TYPE_DEFAULT 0x00000000

/* PCIE_EP_CFG_TYPE0 :: HEADERTYPE_LAT_CACHELINESIZE :: LATENCY_TIMER [15:08] */
#define BCHP_PCIE_EP_CFG_TYPE0_HEADERTYPE_LAT_CACHELINESIZE_LATENCY_TIMER_MASK 0x0000ff00
#define BCHP_PCIE_EP_CFG_TYPE0_HEADERTYPE_LAT_CACHELINESIZE_LATENCY_TIMER_SHIFT 8
#define BCHP_PCIE_EP_CFG_TYPE0_HEADERTYPE_LAT_CACHELINESIZE_LATENCY_TIMER_DEFAULT 0x00000000

/* PCIE_EP_CFG_TYPE0 :: HEADERTYPE_LAT_CACHELINESIZE :: CACHE_LINE_SIZE [07:00] */
#define BCHP_PCIE_EP_CFG_TYPE0_HEADERTYPE_LAT_CACHELINESIZE_CACHE_LINE_SIZE_MASK 0x000000ff
#define BCHP_PCIE_EP_CFG_TYPE0_HEADERTYPE_LAT_CACHELINESIZE_CACHE_LINE_SIZE_SHIFT 0
#define BCHP_PCIE_EP_CFG_TYPE0_HEADERTYPE_LAT_CACHELINESIZE_CACHE_LINE_SIZE_DEFAULT 0x00000000

/***************************************************************************
 *BAR_1 - bar_1
 ***************************************************************************/
/* PCIE_EP_CFG_TYPE0 :: BAR_1 :: ADDRESS [31:04] */
#define BCHP_PCIE_EP_CFG_TYPE0_BAR_1_ADDRESS_MASK                  0xfffffff0
#define BCHP_PCIE_EP_CFG_TYPE0_BAR_1_ADDRESS_SHIFT                 4
#define BCHP_PCIE_EP_CFG_TYPE0_BAR_1_ADDRESS_DEFAULT               0x00000000

/* PCIE_EP_CFG_TYPE0 :: BAR_1 :: PREFETCH [03:03] */
#define BCHP_PCIE_EP_CFG_TYPE0_BAR_1_PREFETCH_MASK                 0x00000008
#define BCHP_PCIE_EP_CFG_TYPE0_BAR_1_PREFETCH_SHIFT                3
#define BCHP_PCIE_EP_CFG_TYPE0_BAR_1_PREFETCH_DEFAULT              0x00000001

/* PCIE_EP_CFG_TYPE0 :: BAR_1 :: SPACE_TYPE [02:01] */
#define BCHP_PCIE_EP_CFG_TYPE0_BAR_1_SPACE_TYPE_MASK               0x00000006
#define BCHP_PCIE_EP_CFG_TYPE0_BAR_1_SPACE_TYPE_SHIFT              1
#define BCHP_PCIE_EP_CFG_TYPE0_BAR_1_SPACE_TYPE_DEFAULT            0x00000002

/* PCIE_EP_CFG_TYPE0 :: BAR_1 :: MEM_SPACE [00:00] */
#define BCHP_PCIE_EP_CFG_TYPE0_BAR_1_MEM_SPACE_MASK                0x00000001
#define BCHP_PCIE_EP_CFG_TYPE0_BAR_1_MEM_SPACE_SHIFT               0
#define BCHP_PCIE_EP_CFG_TYPE0_BAR_1_MEM_SPACE_DEFAULT             0x00000000

/***************************************************************************
 *BAR_2 - bar_2
 ***************************************************************************/
/* PCIE_EP_CFG_TYPE0 :: BAR_2 :: ADDR [31:00] */
#define BCHP_PCIE_EP_CFG_TYPE0_BAR_2_ADDR_MASK                     0xffffffff
#define BCHP_PCIE_EP_CFG_TYPE0_BAR_2_ADDR_SHIFT                    0
#define BCHP_PCIE_EP_CFG_TYPE0_BAR_2_ADDR_DEFAULT                  0x00000000

/***************************************************************************
 *BAR_3 - bar_3
 ***************************************************************************/
/* PCIE_EP_CFG_TYPE0 :: BAR_3 :: ADDRESS [31:04] */
#define BCHP_PCIE_EP_CFG_TYPE0_BAR_3_ADDRESS_MASK                  0xfffffff0
#define BCHP_PCIE_EP_CFG_TYPE0_BAR_3_ADDRESS_SHIFT                 4
#define BCHP_PCIE_EP_CFG_TYPE0_BAR_3_ADDRESS_DEFAULT               0x00000000

/* PCIE_EP_CFG_TYPE0 :: BAR_3 :: PREFETCH [03:03] */
#define BCHP_PCIE_EP_CFG_TYPE0_BAR_3_PREFETCH_MASK                 0x00000008
#define BCHP_PCIE_EP_CFG_TYPE0_BAR_3_PREFETCH_SHIFT                3
#define BCHP_PCIE_EP_CFG_TYPE0_BAR_3_PREFETCH_DEFAULT              0x00000001

/* PCIE_EP_CFG_TYPE0 :: BAR_3 :: SPACE_TYPE [02:01] */
#define BCHP_PCIE_EP_CFG_TYPE0_BAR_3_SPACE_TYPE_MASK               0x00000006
#define BCHP_PCIE_EP_CFG_TYPE0_BAR_3_SPACE_TYPE_SHIFT              1
#define BCHP_PCIE_EP_CFG_TYPE0_BAR_3_SPACE_TYPE_DEFAULT            0x00000002

/* PCIE_EP_CFG_TYPE0 :: BAR_3 :: MEM_SPACE [00:00] */
#define BCHP_PCIE_EP_CFG_TYPE0_BAR_3_MEM_SPACE_MASK                0x00000001
#define BCHP_PCIE_EP_CFG_TYPE0_BAR_3_MEM_SPACE_SHIFT               0
#define BCHP_PCIE_EP_CFG_TYPE0_BAR_3_MEM_SPACE_DEFAULT             0x00000000

/***************************************************************************
 *BAR_4 - bar_4
 ***************************************************************************/
/* PCIE_EP_CFG_TYPE0 :: BAR_4 :: ADDR [31:00] */
#define BCHP_PCIE_EP_CFG_TYPE0_BAR_4_ADDR_MASK                     0xffffffff
#define BCHP_PCIE_EP_CFG_TYPE0_BAR_4_ADDR_SHIFT                    0
#define BCHP_PCIE_EP_CFG_TYPE0_BAR_4_ADDR_DEFAULT                  0x00000000

/***************************************************************************
 *BAR_5 - bar_5
 ***************************************************************************/
/* PCIE_EP_CFG_TYPE0 :: BAR_5 :: ADDRESS [31:04] */
#define BCHP_PCIE_EP_CFG_TYPE0_BAR_5_ADDRESS_MASK                  0xfffffff0
#define BCHP_PCIE_EP_CFG_TYPE0_BAR_5_ADDRESS_SHIFT                 4
#define BCHP_PCIE_EP_CFG_TYPE0_BAR_5_ADDRESS_DEFAULT               0x00000000

/* PCIE_EP_CFG_TYPE0 :: BAR_5 :: PREFETCH [03:03] */
#define BCHP_PCIE_EP_CFG_TYPE0_BAR_5_PREFETCH_MASK                 0x00000008
#define BCHP_PCIE_EP_CFG_TYPE0_BAR_5_PREFETCH_SHIFT                3
#define BCHP_PCIE_EP_CFG_TYPE0_BAR_5_PREFETCH_DEFAULT              0x00000001

/* PCIE_EP_CFG_TYPE0 :: BAR_5 :: SPACE_TYPE [02:01] */
#define BCHP_PCIE_EP_CFG_TYPE0_BAR_5_SPACE_TYPE_MASK               0x00000006
#define BCHP_PCIE_EP_CFG_TYPE0_BAR_5_SPACE_TYPE_SHIFT              1
#define BCHP_PCIE_EP_CFG_TYPE0_BAR_5_SPACE_TYPE_DEFAULT            0x00000002

/* PCIE_EP_CFG_TYPE0 :: BAR_5 :: MEM_SPACE [00:00] */
#define BCHP_PCIE_EP_CFG_TYPE0_BAR_5_MEM_SPACE_MASK                0x00000001
#define BCHP_PCIE_EP_CFG_TYPE0_BAR_5_MEM_SPACE_SHIFT               0
#define BCHP_PCIE_EP_CFG_TYPE0_BAR_5_MEM_SPACE_DEFAULT             0x00000000

/***************************************************************************
 *BAR_6 - bar_6
 ***************************************************************************/
/* PCIE_EP_CFG_TYPE0 :: BAR_6 :: ADDR [31:00] */
#define BCHP_PCIE_EP_CFG_TYPE0_BAR_6_ADDR_MASK                     0xffffffff
#define BCHP_PCIE_EP_CFG_TYPE0_BAR_6_ADDR_SHIFT                    0
#define BCHP_PCIE_EP_CFG_TYPE0_BAR_6_ADDR_DEFAULT                  0x00000000

/***************************************************************************
 *CARDBUS_CIS - cardbus_cis
 ***************************************************************************/
/* PCIE_EP_CFG_TYPE0 :: CARDBUS_CIS :: CARDBUS_CIS [31:00] */
#define BCHP_PCIE_EP_CFG_TYPE0_CARDBUS_CIS_CARDBUS_CIS_MASK        0xffffffff
#define BCHP_PCIE_EP_CFG_TYPE0_CARDBUS_CIS_CARDBUS_CIS_SHIFT       0
#define BCHP_PCIE_EP_CFG_TYPE0_CARDBUS_CIS_CARDBUS_CIS_DEFAULT     0x00000000

/***************************************************************************
 *SUBSYSTEM_ID_VENDOR_ID - subsystem_id_vendor_id
 ***************************************************************************/
/* PCIE_EP_CFG_TYPE0 :: SUBSYSTEM_ID_VENDOR_ID :: SUBSYSTEM_ID [31:16] */
#define BCHP_PCIE_EP_CFG_TYPE0_SUBSYSTEM_ID_VENDOR_ID_SUBSYSTEM_ID_MASK 0xffff0000
#define BCHP_PCIE_EP_CFG_TYPE0_SUBSYSTEM_ID_VENDOR_ID_SUBSYSTEM_ID_SHIFT 16

/* PCIE_EP_CFG_TYPE0 :: SUBSYSTEM_ID_VENDOR_ID :: SUBSYSTEM_VENDOR_ID [15:00] */
#define BCHP_PCIE_EP_CFG_TYPE0_SUBSYSTEM_ID_VENDOR_ID_SUBSYSTEM_VENDOR_ID_MASK 0x0000ffff
#define BCHP_PCIE_EP_CFG_TYPE0_SUBSYSTEM_ID_VENDOR_ID_SUBSYSTEM_VENDOR_ID_SHIFT 0
#define BCHP_PCIE_EP_CFG_TYPE0_SUBSYSTEM_ID_VENDOR_ID_SUBSYSTEM_VENDOR_ID_DEFAULT 0x000014e4

/***************************************************************************
 *EXP_ROM_BAR - exp_rom_bar
 ***************************************************************************/
/* PCIE_EP_CFG_TYPE0 :: EXP_ROM_BAR :: ADDRESS [31:24] */
#define BCHP_PCIE_EP_CFG_TYPE0_EXP_ROM_BAR_ADDRESS_MASK            0xff000000
#define BCHP_PCIE_EP_CFG_TYPE0_EXP_ROM_BAR_ADDRESS_SHIFT           24
#define BCHP_PCIE_EP_CFG_TYPE0_EXP_ROM_BAR_ADDRESS_DEFAULT         0x00000000

/* PCIE_EP_CFG_TYPE0 :: EXP_ROM_BAR :: SIZE [23:11] */
#define BCHP_PCIE_EP_CFG_TYPE0_EXP_ROM_BAR_SIZE_MASK               0x00fff800
#define BCHP_PCIE_EP_CFG_TYPE0_EXP_ROM_BAR_SIZE_SHIFT              11
#define BCHP_PCIE_EP_CFG_TYPE0_EXP_ROM_BAR_SIZE_DEFAULT            0x00000000

/* PCIE_EP_CFG_TYPE0 :: EXP_ROM_BAR :: LOW [10:01] */
#define BCHP_PCIE_EP_CFG_TYPE0_EXP_ROM_BAR_LOW_MASK                0x000007fe
#define BCHP_PCIE_EP_CFG_TYPE0_EXP_ROM_BAR_LOW_SHIFT               1
#define BCHP_PCIE_EP_CFG_TYPE0_EXP_ROM_BAR_LOW_DEFAULT             0x00000000

/* PCIE_EP_CFG_TYPE0 :: EXP_ROM_BAR :: BAR_ENA [00:00] */
#define BCHP_PCIE_EP_CFG_TYPE0_EXP_ROM_BAR_BAR_ENA_MASK            0x00000001
#define BCHP_PCIE_EP_CFG_TYPE0_EXP_ROM_BAR_BAR_ENA_SHIFT           0
#define BCHP_PCIE_EP_CFG_TYPE0_EXP_ROM_BAR_BAR_ENA_DEFAULT         0x00000000

/***************************************************************************
 *CAP_POINTER - cap_pointer
 ***************************************************************************/
/* PCIE_EP_CFG_TYPE0 :: CAP_POINTER :: RESERVED0 [31:08] */
#define BCHP_PCIE_EP_CFG_TYPE0_CAP_POINTER_RESERVED0_MASK          0xffffff00
#define BCHP_PCIE_EP_CFG_TYPE0_CAP_POINTER_RESERVED0_SHIFT         8

/* PCIE_EP_CFG_TYPE0 :: CAP_POINTER :: CAP_POINTER [07:00] */
#define BCHP_PCIE_EP_CFG_TYPE0_CAP_POINTER_CAP_POINTER_MASK        0x000000ff
#define BCHP_PCIE_EP_CFG_TYPE0_CAP_POINTER_CAP_POINTER_SHIFT       0
#define BCHP_PCIE_EP_CFG_TYPE0_CAP_POINTER_CAP_POINTER_DEFAULT     0x00000048

/***************************************************************************
 *RESERVED - Reserved
 ***************************************************************************/
/* PCIE_EP_CFG_TYPE0 :: RESERVED :: RESERVED [31:00] */
#define BCHP_PCIE_EP_CFG_TYPE0_RESERVED_RESERVED_MASK              0xffffffff
#define BCHP_PCIE_EP_CFG_TYPE0_RESERVED_RESERVED_SHIFT             0

/***************************************************************************
 *LAT_MIN_GRANT_INT_PIN_INT_LINE - lat_min_grant_int_pin_int_line
 ***************************************************************************/
/* PCIE_EP_CFG_TYPE0 :: LAT_MIN_GRANT_INT_PIN_INT_LINE :: MAXIMUM_LATENCY [31:24] */
#define BCHP_PCIE_EP_CFG_TYPE0_LAT_MIN_GRANT_INT_PIN_INT_LINE_MAXIMUM_LATENCY_MASK 0xff000000
#define BCHP_PCIE_EP_CFG_TYPE0_LAT_MIN_GRANT_INT_PIN_INT_LINE_MAXIMUM_LATENCY_SHIFT 24
#define BCHP_PCIE_EP_CFG_TYPE0_LAT_MIN_GRANT_INT_PIN_INT_LINE_MAXIMUM_LATENCY_DEFAULT 0x00000000

/* PCIE_EP_CFG_TYPE0 :: LAT_MIN_GRANT_INT_PIN_INT_LINE :: MIN_GRANT [23:16] */
#define BCHP_PCIE_EP_CFG_TYPE0_LAT_MIN_GRANT_INT_PIN_INT_LINE_MIN_GRANT_MASK 0x00ff0000
#define BCHP_PCIE_EP_CFG_TYPE0_LAT_MIN_GRANT_INT_PIN_INT_LINE_MIN_GRANT_SHIFT 16
#define BCHP_PCIE_EP_CFG_TYPE0_LAT_MIN_GRANT_INT_PIN_INT_LINE_MIN_GRANT_DEFAULT 0x00000000

/* PCIE_EP_CFG_TYPE0 :: LAT_MIN_GRANT_INT_PIN_INT_LINE :: INT_PIN [15:08] */
#define BCHP_PCIE_EP_CFG_TYPE0_LAT_MIN_GRANT_INT_PIN_INT_LINE_INT_PIN_MASK 0x0000ff00
#define BCHP_PCIE_EP_CFG_TYPE0_LAT_MIN_GRANT_INT_PIN_INT_LINE_INT_PIN_SHIFT 8
#define BCHP_PCIE_EP_CFG_TYPE0_LAT_MIN_GRANT_INT_PIN_INT_LINE_INT_PIN_DEFAULT 0x00000001

/* PCIE_EP_CFG_TYPE0 :: LAT_MIN_GRANT_INT_PIN_INT_LINE :: INT_LINE [07:00] */
#define BCHP_PCIE_EP_CFG_TYPE0_LAT_MIN_GRANT_INT_PIN_INT_LINE_INT_LINE_MASK 0x000000ff
#define BCHP_PCIE_EP_CFG_TYPE0_LAT_MIN_GRANT_INT_PIN_INT_LINE_INT_LINE_SHIFT 0
#define BCHP_PCIE_EP_CFG_TYPE0_LAT_MIN_GRANT_INT_PIN_INT_LINE_INT_LINE_DEFAULT 0x00000000

#endif /* #ifndef BCHP_PCIE_EP_CFG_TYPE0_H__ */

/* End of File */