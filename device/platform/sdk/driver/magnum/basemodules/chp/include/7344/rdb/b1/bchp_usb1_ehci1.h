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
 * $brcm_Workfile: bchp_usb1_ehci1.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 4/4/11 2:30p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Fri Apr  1 16:39:37 2011
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
 * $brcm_Log: /magnum/basemodules/chp/7344/rdb/b0/bchp_usb1_ehci1.h $
 * 
 * Hydra_Software_Devel/1   4/4/11 2:30p albertl
 * SW7344-40: Initial revision.
 *
 ***************************************************************************/

#ifndef BCHP_USB1_EHCI1_H__
#define BCHP_USB1_EHCI1_H__

/***************************************************************************
 *USB1_EHCI1 - USB EHCI 1 Control Registers
 ***************************************************************************/
#define BCHP_USB1_EHCI1_HCCAPBASE                0x00490500 /* EHCI Capability Register */
#define BCHP_USB1_EHCI1_HCSPARAMS                0x00490504 /* EHCI Structural Parameter */
#define BCHP_USB1_EHCI1_HCCPARAMS                0x00490508 /* EHCI Capability Parameter */
#define BCHP_USB1_EHCI1_USBCMD                   0x00490510 /* USB Command Register */
#define BCHP_USB1_EHCI1_USBSTS                   0x00490514 /* USB Status  Register */
#define BCHP_USB1_EHCI1_USBINTR                  0x00490518 /* USB Interrupt Enable Register */
#define BCHP_USB1_EHCI1_FRINDEX                  0x0049051c /* USB Frame Index Register */
#define BCHP_USB1_EHCI1_PERIODICLISTBASE         0x00490524 /* Periodic Frame List Base Address Register */
#define BCHP_USB1_EHCI1_ASYNCLISTADDR            0x00490528 /* Asynchronous List Address */
#define BCHP_USB1_EHCI1_CONFIGFLAG               0x00490550 /* Configured Flag Register */
#define BCHP_USB1_EHCI1_PORTSC_0                 0x00490554 /* Port Status/Control Register for Port 0 */
#define BCHP_USB1_EHCI1_INSNREG00                0x00490590 /* Microframe Base Value Register */
#define BCHP_USB1_EHCI1_INSNREG01                0x00490594 /* Packet Buffer OUT/IN Threshold Register */
#define BCHP_USB1_EHCI1_INSNREG02                0x00490598 /* Packet Buffer Depth Register */
#define BCHP_USB1_EHCI1_INSNREG03                0x0049059c /* Break Memory Transfer Register */
#define BCHP_USB1_EHCI1_INSNREG04                0x004905a0 /* Debug Register */
#define BCHP_USB1_EHCI1_INSNREG05                0x004905a4 /* UTMI Control and Status Register */

#endif /* #ifndef BCHP_USB1_EHCI1_H__ */

/* End of File */