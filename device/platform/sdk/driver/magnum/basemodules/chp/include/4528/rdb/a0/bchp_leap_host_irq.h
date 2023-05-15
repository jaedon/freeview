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
 * $brcm_Workfile: bchp_leap_host_irq.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 2/4/11 3:58p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Fri Feb  4 12:04:12 2011
 *                 MD5 Checksum         651635341c0017dd0a351cfe3d5e51fe
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/4528/rdb/a0/bchp_leap_host_irq.h $
 * 
 * Hydra_Software_Devel/1   2/4/11 3:58p ronchan
 * HW4528-2: sync up with local RDB
 *
 ***************************************************************************/

#ifndef BCHP_LEAP_HOST_IRQ_H__
#define BCHP_LEAP_HOST_IRQ_H__

/***************************************************************************
 *LEAP_HOST_IRQ - Host Interrupt Controller Registers
 ***************************************************************************/
#define BCHP_LEAP_HOST_IRQ_STATUS0               0x00080300 /* Host Interrupt Status Register */
#define BCHP_LEAP_HOST_IRQ_SET0                  0x00080304 /* Host Interrupt Set Register */
#define BCHP_LEAP_HOST_IRQ_CLEAR0                0x00080308 /* Host Interrupt Clear Register */
#define BCHP_LEAP_HOST_IRQ_MASK_STATUS0          0x0008030c /* Host Interrupt Mask Status Register */
#define BCHP_LEAP_HOST_IRQ_MASK_SET0             0x00080310 /* Host Interrupt Mask Set Register */
#define BCHP_LEAP_HOST_IRQ_MASK_CLEAR0           0x00080314 /* Host Interrupt Mask Clear Register */
#define BCHP_LEAP_HOST_IRQ_STATUS1               0x00080318 /* Host Interrupt Status Register */
#define BCHP_LEAP_HOST_IRQ_SET1                  0x0008031c /* Host Interrupt Set Register */
#define BCHP_LEAP_HOST_IRQ_CLEAR1                0x00080320 /* Host Interrupt Clear Register */
#define BCHP_LEAP_HOST_IRQ_MASK_STATUS1          0x00080324 /* Host Interrupt Mask Status Register */
#define BCHP_LEAP_HOST_IRQ_MASK_SET1             0x00080328 /* Host Interrupt Mask Set Register */
#define BCHP_LEAP_HOST_IRQ_MASK_CLEAR1           0x0008032c /* Host Interrupt Mask Clear Register */

/***************************************************************************
 *STATUS0 - Host Interrupt Status Register
 ***************************************************************************/
/* LEAP_HOST_IRQ :: STATUS0 :: IRQ_STATUS [31:00] */
#define BCHP_LEAP_HOST_IRQ_STATUS0_IRQ_STATUS_MASK                 0xffffffff
#define BCHP_LEAP_HOST_IRQ_STATUS0_IRQ_STATUS_SHIFT                0

/***************************************************************************
 *SET0 - Host Interrupt Set Register
 ***************************************************************************/
/* LEAP_HOST_IRQ :: SET0 :: IRQ_SET [31:00] */
#define BCHP_LEAP_HOST_IRQ_SET0_IRQ_SET_MASK                       0xffffffff
#define BCHP_LEAP_HOST_IRQ_SET0_IRQ_SET_SHIFT                      0

/***************************************************************************
 *CLEAR0 - Host Interrupt Clear Register
 ***************************************************************************/
/* LEAP_HOST_IRQ :: CLEAR0 :: IRQ_CLEAR [31:00] */
#define BCHP_LEAP_HOST_IRQ_CLEAR0_IRQ_CLEAR_MASK                   0xffffffff
#define BCHP_LEAP_HOST_IRQ_CLEAR0_IRQ_CLEAR_SHIFT                  0

/***************************************************************************
 *MASK_STATUS0 - Host Interrupt Mask Status Register
 ***************************************************************************/
/* LEAP_HOST_IRQ :: MASK_STATUS0 :: MASK_STATUS [31:00] */
#define BCHP_LEAP_HOST_IRQ_MASK_STATUS0_MASK_STATUS_MASK           0xffffffff
#define BCHP_LEAP_HOST_IRQ_MASK_STATUS0_MASK_STATUS_SHIFT          0

/***************************************************************************
 *MASK_SET0 - Host Interrupt Mask Set Register
 ***************************************************************************/
/* LEAP_HOST_IRQ :: MASK_SET0 :: MASK_SET [31:00] */
#define BCHP_LEAP_HOST_IRQ_MASK_SET0_MASK_SET_MASK                 0xffffffff
#define BCHP_LEAP_HOST_IRQ_MASK_SET0_MASK_SET_SHIFT                0

/***************************************************************************
 *MASK_CLEAR0 - Host Interrupt Mask Clear Register
 ***************************************************************************/
/* LEAP_HOST_IRQ :: MASK_CLEAR0 :: MASK_CLEAR [31:00] */
#define BCHP_LEAP_HOST_IRQ_MASK_CLEAR0_MASK_CLEAR_MASK             0xffffffff
#define BCHP_LEAP_HOST_IRQ_MASK_CLEAR0_MASK_CLEAR_SHIFT            0

/***************************************************************************
 *STATUS1 - Host Interrupt Status Register
 ***************************************************************************/
/* LEAP_HOST_IRQ :: STATUS1 :: IRQ_STATUS [31:00] */
#define BCHP_LEAP_HOST_IRQ_STATUS1_IRQ_STATUS_MASK                 0xffffffff
#define BCHP_LEAP_HOST_IRQ_STATUS1_IRQ_STATUS_SHIFT                0

/***************************************************************************
 *SET1 - Host Interrupt Set Register
 ***************************************************************************/
/* LEAP_HOST_IRQ :: SET1 :: IRQ_SET [31:00] */
#define BCHP_LEAP_HOST_IRQ_SET1_IRQ_SET_MASK                       0xffffffff
#define BCHP_LEAP_HOST_IRQ_SET1_IRQ_SET_SHIFT                      0

/***************************************************************************
 *CLEAR1 - Host Interrupt Clear Register
 ***************************************************************************/
/* LEAP_HOST_IRQ :: CLEAR1 :: IRQ_CLEAR [31:00] */
#define BCHP_LEAP_HOST_IRQ_CLEAR1_IRQ_CLEAR_MASK                   0xffffffff
#define BCHP_LEAP_HOST_IRQ_CLEAR1_IRQ_CLEAR_SHIFT                  0

/***************************************************************************
 *MASK_STATUS1 - Host Interrupt Mask Status Register
 ***************************************************************************/
/* LEAP_HOST_IRQ :: MASK_STATUS1 :: MASK_STATUS [31:00] */
#define BCHP_LEAP_HOST_IRQ_MASK_STATUS1_MASK_STATUS_MASK           0xffffffff
#define BCHP_LEAP_HOST_IRQ_MASK_STATUS1_MASK_STATUS_SHIFT          0

/***************************************************************************
 *MASK_SET1 - Host Interrupt Mask Set Register
 ***************************************************************************/
/* LEAP_HOST_IRQ :: MASK_SET1 :: MASK_SET [31:00] */
#define BCHP_LEAP_HOST_IRQ_MASK_SET1_MASK_SET_MASK                 0xffffffff
#define BCHP_LEAP_HOST_IRQ_MASK_SET1_MASK_SET_SHIFT                0

/***************************************************************************
 *MASK_CLEAR1 - Host Interrupt Mask Clear Register
 ***************************************************************************/
/* LEAP_HOST_IRQ :: MASK_CLEAR1 :: MASK_CLEAR [31:00] */
#define BCHP_LEAP_HOST_IRQ_MASK_CLEAR1_MASK_CLEAR_MASK             0xffffffff
#define BCHP_LEAP_HOST_IRQ_MASK_CLEAR1_MASK_CLEAR_SHIFT            0

#endif /* #ifndef BCHP_LEAP_HOST_IRQ_H__ */

/* End of File */