/****************************************************************************
 *     Copyright (c) 1999-2014, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on              Thu Feb 20 15:28:49 2014
 *                 Full Compile MD5 Checksum 008a7d6e5c6075780b5099a6ea7d8bad
 *                   (minus title and desc)
 *                 MD5 Checksum              eecc054f9cb653f7c8cde54335a8c3b6
 *
 * Compiled with:  RDB Utility               combo_header.pl
 *                 RDB Parser                3.0
 *                 unknown                   unknown
 *                 Perl Interpreter          5.008008
 *                 Operating System          linux
 *
 *
 ***************************************************************************/



/****************************************************************************
 ***************************************************************************/

#ifndef BCHP_CSR_H__
#define BCHP_CSR_H__

/***************************************************************************
 *CSR - BBSI Control & Status Registers
 ***************************************************************************/
#define BCHP_CSR_SER_PROT_REV                    0x00000000 /* Serial protocol revision ID */
#define BCHP_CSR_CHIP_FAM0                       0x00000001 /* Bits [15:08] of the 32-bit chip family ID */
#define BCHP_CSR_CHIP_FAM1                       0x00000002 /* Bits [07:00] of the 32-bit chip family ID */
#define BCHP_CSR_CHIP_FAM2                       0x00000003 /* Bits [15:08] of the 32-bit chip family ID */
#define BCHP_CSR_CHIP_FAM3                       0x00000004 /* Bits [07:00] of the 32-bit chip family ID */
#define BCHP_CSR_PROGRAM                         0x00000005 /* Program status register */
#define BCHP_CSR_STATUS                          0x00000006 /* Status register of the serial interface */
#define BCHP_CSR_CONFIG                          0x00000007 /* Configuration register of the serial interface */
#define BCHP_CSR_RBUS_ADDR0                      0x00000008 /* Bits [31:24] of the 32-bit RBUS address */
#define BCHP_CSR_RBUS_ADDR1                      0x00000009 /* Bits [23:16] of the 32-bit RBUS address */
#define BCHP_CSR_RBUS_ADDR2                      0x0000000a /* Bits [15:08] of the 32-bit RBUS address */
#define BCHP_CSR_RBUS_ADDR3                      0x0000000b /* Bits [07:00] of the 32-bit RBUS address */
#define BCHP_CSR_RBUS_DATA0                      0x0000000c /* Bits [31:24] of the 32-bit RBUS data */
#define BCHP_CSR_RBUS_DATA1                      0x0000000d /* Bits [23:16] of the 32-bit RBUS data */
#define BCHP_CSR_RBUS_DATA2                      0x0000000e /* Bits [15:08] of the 32-bit RBUS data */
#define BCHP_CSR_RBUS_DATA3                      0x0000000f /* Bits [07:00] of the 32-bit RBUS data */
#define BCHP_CSR_CHIP_PROD0                      0x00000010 /* Bits [31:24] of the 32-bit chip product ID */
#define BCHP_CSR_CHIP_PROD1                      0x00000011 /* Bits [23:16] of the 32-bit chip product ID */
#define BCHP_CSR_CHIP_PROD2                      0x00000012 /* Bits [15:08] of the 32-bit chip product ID */
#define BCHP_CSR_CHIP_PROD3                      0x00000013 /* Bits [07:00] of the 32-bit chip product ID internal revision */

/***************************************************************************
 *SER_PROT_REV - Serial protocol revision ID
 ***************************************************************************/
/* CSR :: SER_PROT_REV :: SER_PROT_REV [07:00] */
#define BCHP_CSR_SER_PROT_REV_SER_PROT_REV_MASK                    0xff
#define BCHP_CSR_SER_PROT_REV_SER_PROT_REV_SHIFT                   0
#define BCHP_CSR_SER_PROT_REV_SER_PROT_REV_DEFAULT                 0x08

/***************************************************************************
 *CHIP_FAM0 - Bits [15:08] of the 32-bit chip family ID
 ***************************************************************************/
/* CSR :: CHIP_FAM0 :: CHIP_FAMILY [07:00] */
#define BCHP_CSR_CHIP_FAM0_CHIP_FAMILY_MASK                        0xff
#define BCHP_CSR_CHIP_FAM0_CHIP_FAMILY_SHIFT                       0
#define BCHP_CSR_CHIP_FAM0_CHIP_FAMILY_DEFAULT                     0x04

/***************************************************************************
 *CHIP_FAM1 - Bits [07:00] of the 32-bit chip family ID
 ***************************************************************************/
/* CSR :: CHIP_FAM1 :: CHIP_FAMILY [07:00] */
#define BCHP_CSR_CHIP_FAM1_CHIP_FAMILY_MASK                        0xff
#define BCHP_CSR_CHIP_FAM1_CHIP_FAMILY_SHIFT                       0
#define BCHP_CSR_CHIP_FAM1_CHIP_FAMILY_DEFAULT                     0x52

/***************************************************************************
 *CHIP_FAM2 - Bits [15:08] of the 32-bit chip family ID
 ***************************************************************************/
/* CSR :: CHIP_FAM2 :: CHIP_REV [07:00] */
#define BCHP_CSR_CHIP_FAM2_CHIP_REV_MASK                           0xff
#define BCHP_CSR_CHIP_FAM2_CHIP_REV_SHIFT                          0
#define BCHP_CSR_CHIP_FAM2_CHIP_REV_DEFAULT                        0x16

/***************************************************************************
 *CHIP_FAM3 - Bits [07:00] of the 32-bit chip family ID
 ***************************************************************************/
/* CSR :: CHIP_FAM3 :: CHIP_REV [07:00] */
#define BCHP_CSR_CHIP_FAM3_CHIP_REV_MASK                           0xff
#define BCHP_CSR_CHIP_FAM3_CHIP_REV_SHIFT                          0
#define BCHP_CSR_CHIP_FAM3_CHIP_REV_DEFAULT                        0x00

/***************************************************************************
 *PROGRAM - Program status register
 ***************************************************************************/
/* CSR :: PROGRAM :: reserved0 [07:01] */
#define BCHP_CSR_PROGRAM_reserved0_MASK                            0xfe
#define BCHP_CSR_PROGRAM_reserved0_SHIFT                           1

/* CSR :: PROGRAM :: LOADED [00:00] */
#define BCHP_CSR_PROGRAM_LOADED_MASK                               0x01
#define BCHP_CSR_PROGRAM_LOADED_SHIFT                              0
#define BCHP_CSR_PROGRAM_LOADED_DEFAULT                            0x00

/***************************************************************************
 *STATUS - Status register of the serial interface
 ***************************************************************************/
/* CSR :: STATUS :: reserved0 [07:07] */
#define BCHP_CSR_STATUS_reserved0_MASK                             0x80
#define BCHP_CSR_STATUS_reserved0_SHIFT                            7

/* CSR :: STATUS :: CPU_RUNNING [06:06] */
#define BCHP_CSR_STATUS_CPU_RUNNING_MASK                           0x40
#define BCHP_CSR_STATUS_CPU_RUNNING_SHIFT                          6
#define BCHP_CSR_STATUS_CPU_RUNNING_DEFAULT                        0x00

/* CSR :: STATUS :: HAB_REQ [05:05] */
#define BCHP_CSR_STATUS_HAB_REQ_MASK                               0x20
#define BCHP_CSR_STATUS_HAB_REQ_SHIFT                              5
#define BCHP_CSR_STATUS_HAB_REQ_DEFAULT                            0x00

/* CSR :: STATUS :: BUSY [04:04] */
#define BCHP_CSR_STATUS_BUSY_MASK                                  0x10
#define BCHP_CSR_STATUS_BUSY_SHIFT                                 4
#define BCHP_CSR_STATUS_BUSY_DEFAULT                               0x00

/* CSR :: STATUS :: RBUS_UNEXP_TX [03:03] */
#define BCHP_CSR_STATUS_RBUS_UNEXP_TX_MASK                         0x08
#define BCHP_CSR_STATUS_RBUS_UNEXP_TX_SHIFT                        3
#define BCHP_CSR_STATUS_RBUS_UNEXP_TX_DEFAULT                      0x00

/* CSR :: STATUS :: RBUS_TIMEOUT [02:02] */
#define BCHP_CSR_STATUS_RBUS_TIMEOUT_MASK                          0x04
#define BCHP_CSR_STATUS_RBUS_TIMEOUT_SHIFT                         2
#define BCHP_CSR_STATUS_RBUS_TIMEOUT_DEFAULT                       0x00

/* CSR :: STATUS :: RBUS_ERR_ACK [01:01] */
#define BCHP_CSR_STATUS_RBUS_ERR_ACK_MASK                          0x02
#define BCHP_CSR_STATUS_RBUS_ERR_ACK_SHIFT                         1
#define BCHP_CSR_STATUS_RBUS_ERR_ACK_DEFAULT                       0x00

/* CSR :: STATUS :: ERROR [00:00] */
#define BCHP_CSR_STATUS_ERROR_MASK                                 0x01
#define BCHP_CSR_STATUS_ERROR_SHIFT                                0
#define BCHP_CSR_STATUS_ERROR_DEFAULT                              0x00

/***************************************************************************
 *CONFIG - Configuration register of the serial interface
 ***************************************************************************/
/* CSR :: CONFIG :: reserved0 [07:05] */
#define BCHP_CSR_CONFIG_reserved0_MASK                             0xe0
#define BCHP_CSR_CONFIG_reserved0_SHIFT                            5

/* CSR :: CONFIG :: TRANSFER_MODE [04:03] */
#define BCHP_CSR_CONFIG_TRANSFER_MODE_MASK                         0x18
#define BCHP_CSR_CONFIG_TRANSFER_MODE_SHIFT                        3
#define BCHP_CSR_CONFIG_TRANSFER_MODE_DEFAULT                      0x00

/* CSR :: CONFIG :: NO_RBUS_ADDR_INC [02:02] */
#define BCHP_CSR_CONFIG_NO_RBUS_ADDR_INC_MASK                      0x04
#define BCHP_CSR_CONFIG_NO_RBUS_ADDR_INC_SHIFT                     2
#define BCHP_CSR_CONFIG_NO_RBUS_ADDR_INC_DEFAULT                   0x00

/* CSR :: CONFIG :: SPECULATIVE_READ_EN [01:01] */
#define BCHP_CSR_CONFIG_SPECULATIVE_READ_EN_MASK                   0x02
#define BCHP_CSR_CONFIG_SPECULATIVE_READ_EN_SHIFT                  1
#define BCHP_CSR_CONFIG_SPECULATIVE_READ_EN_DEFAULT                0x00

/* CSR :: CONFIG :: READ_RBUS [00:00] */
#define BCHP_CSR_CONFIG_READ_RBUS_MASK                             0x01
#define BCHP_CSR_CONFIG_READ_RBUS_SHIFT                            0
#define BCHP_CSR_CONFIG_READ_RBUS_DEFAULT                          0x00

/***************************************************************************
 *RBUS_ADDR0 - Bits [31:24] of the 32-bit RBUS address
 ***************************************************************************/
/* CSR :: RBUS_ADDR0 :: RBUS_ADDR0 [07:00] */
#define BCHP_CSR_RBUS_ADDR0_RBUS_ADDR0_MASK                        0xff
#define BCHP_CSR_RBUS_ADDR0_RBUS_ADDR0_SHIFT                       0
#define BCHP_CSR_RBUS_ADDR0_RBUS_ADDR0_DEFAULT                     0x00

/***************************************************************************
 *RBUS_ADDR1 - Bits [23:16] of the 32-bit RBUS address
 ***************************************************************************/
/* CSR :: RBUS_ADDR1 :: RBUS_ADDR1 [07:00] */
#define BCHP_CSR_RBUS_ADDR1_RBUS_ADDR1_MASK                        0xff
#define BCHP_CSR_RBUS_ADDR1_RBUS_ADDR1_SHIFT                       0
#define BCHP_CSR_RBUS_ADDR1_RBUS_ADDR1_DEFAULT                     0x00

/***************************************************************************
 *RBUS_ADDR2 - Bits [15:08] of the 32-bit RBUS address
 ***************************************************************************/
/* CSR :: RBUS_ADDR2 :: RBUS_ADDR2 [07:00] */
#define BCHP_CSR_RBUS_ADDR2_RBUS_ADDR2_MASK                        0xff
#define BCHP_CSR_RBUS_ADDR2_RBUS_ADDR2_SHIFT                       0
#define BCHP_CSR_RBUS_ADDR2_RBUS_ADDR2_DEFAULT                     0x00

/***************************************************************************
 *RBUS_ADDR3 - Bits [07:00] of the 32-bit RBUS address
 ***************************************************************************/
/* CSR :: RBUS_ADDR3 :: RBUS_ADDR3 [07:02] */
#define BCHP_CSR_RBUS_ADDR3_RBUS_ADDR3_MASK                        0xfc
#define BCHP_CSR_RBUS_ADDR3_RBUS_ADDR3_SHIFT                       2
#define BCHP_CSR_RBUS_ADDR3_RBUS_ADDR3_DEFAULT                     0x00

/* CSR :: RBUS_ADDR3 :: RBUS_ADDR3_LSB [01:00] */
#define BCHP_CSR_RBUS_ADDR3_RBUS_ADDR3_LSB_MASK                    0x03
#define BCHP_CSR_RBUS_ADDR3_RBUS_ADDR3_LSB_SHIFT                   0
#define BCHP_CSR_RBUS_ADDR3_RBUS_ADDR3_LSB_DEFAULT                 0x00

/***************************************************************************
 *RBUS_DATA0 - Bits [31:24] of the 32-bit RBUS data
 ***************************************************************************/
/* CSR :: RBUS_DATA0 :: RBUS_DATA0 [07:00] */
#define BCHP_CSR_RBUS_DATA0_RBUS_DATA0_MASK                        0xff
#define BCHP_CSR_RBUS_DATA0_RBUS_DATA0_SHIFT                       0
#define BCHP_CSR_RBUS_DATA0_RBUS_DATA0_DEFAULT                     0x00

/***************************************************************************
 *RBUS_DATA1 - Bits [23:16] of the 32-bit RBUS data
 ***************************************************************************/
/* CSR :: RBUS_DATA1 :: RBUS_DATA1 [07:00] */
#define BCHP_CSR_RBUS_DATA1_RBUS_DATA1_MASK                        0xff
#define BCHP_CSR_RBUS_DATA1_RBUS_DATA1_SHIFT                       0
#define BCHP_CSR_RBUS_DATA1_RBUS_DATA1_DEFAULT                     0x00

/***************************************************************************
 *RBUS_DATA2 - Bits [15:08] of the 32-bit RBUS data
 ***************************************************************************/
/* CSR :: RBUS_DATA2 :: RBUS_DATA2 [07:00] */
#define BCHP_CSR_RBUS_DATA2_RBUS_DATA2_MASK                        0xff
#define BCHP_CSR_RBUS_DATA2_RBUS_DATA2_SHIFT                       0
#define BCHP_CSR_RBUS_DATA2_RBUS_DATA2_DEFAULT                     0x00

/***************************************************************************
 *RBUS_DATA3 - Bits [07:00] of the 32-bit RBUS data
 ***************************************************************************/
/* CSR :: RBUS_DATA3 :: RBUS_DATA3 [07:00] */
#define BCHP_CSR_RBUS_DATA3_RBUS_DATA3_MASK                        0xff
#define BCHP_CSR_RBUS_DATA3_RBUS_DATA3_SHIFT                       0
#define BCHP_CSR_RBUS_DATA3_RBUS_DATA3_DEFAULT                     0x00

/***************************************************************************
 *CHIP_PROD0 - Bits [31:24] of the 32-bit chip product ID
 ***************************************************************************/
/* CSR :: CHIP_PROD0 :: CHIP_PRODUCT [07:00] */
#define BCHP_CSR_CHIP_PROD0_CHIP_PRODUCT_MASK                      0xff
#define BCHP_CSR_CHIP_PROD0_CHIP_PRODUCT_SHIFT                     0
#define BCHP_CSR_CHIP_PROD0_CHIP_PRODUCT_DEFAULT                   0x04

/***************************************************************************
 *CHIP_PROD1 - Bits [23:16] of the 32-bit chip product ID
 ***************************************************************************/
/* CSR :: CHIP_PROD1 :: CHIP_PRODUCT [07:00] */
#define BCHP_CSR_CHIP_PROD1_CHIP_PRODUCT_MASK                      0xff
#define BCHP_CSR_CHIP_PROD1_CHIP_PRODUCT_SHIFT                     0
#define BCHP_CSR_CHIP_PROD1_CHIP_PRODUCT_DEFAULT                   0x52

/***************************************************************************
 *CHIP_PROD2 - Bits [15:08] of the 32-bit chip product ID
 ***************************************************************************/
/* CSR :: CHIP_PROD2 :: CHIP_PRODUCT [07:00] */
#define BCHP_CSR_CHIP_PROD2_CHIP_PRODUCT_MASK                      0xff
#define BCHP_CSR_CHIP_PROD2_CHIP_PRODUCT_SHIFT                     0
#define BCHP_CSR_CHIP_PROD2_CHIP_PRODUCT_DEFAULT                   0x16

/***************************************************************************
 *CHIP_PROD3 - Bits [07:00] of the 32-bit chip product ID internal revision
 ***************************************************************************/
/* CSR :: CHIP_PROD3 :: CHIP_PRODUCT [07:00] */
#define BCHP_CSR_CHIP_PROD3_CHIP_PRODUCT_MASK                      0xff
#define BCHP_CSR_CHIP_PROD3_CHIP_PRODUCT_SHIFT                     0
#define BCHP_CSR_CHIP_PROD3_CHIP_PRODUCT_DEFAULT                   0x00

#endif /* #ifndef BCHP_CSR_H__ */

/* End of File */