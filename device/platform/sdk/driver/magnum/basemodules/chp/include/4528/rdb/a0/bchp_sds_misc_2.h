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
 * $brcm_Workfile: bchp_sds_misc_2.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 1/13/11 8:03a $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Wed Jan 12 18:41:09 2011
 *                 MD5 Checksum         6e6727f6c827233acdd395c9a9032c98
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/4528/rdb/a0/bchp_sds_misc_2.h $
 * 
 * Hydra_Software_Devel/1   1/13/11 8:03a pntruong
 * HW4528-2: Synced up with central rdb.
 *
 ***************************************************************************/

#ifndef BCHP_SDS_MISC_2_H__
#define BCHP_SDS_MISC_2_H__

/***************************************************************************
 *SDS_MISC_2 - SDS MISC Register Set 2
 ***************************************************************************/
#define BCHP_SDS_MISC_2_REVID                    0x000c0600 /* Revision ID */
#define BCHP_SDS_MISC_2_IICTL1                   0x000c0620 /* MI2C Control Register 1 */
#define BCHP_SDS_MISC_2_IICTL2                   0x000c0624 /* MI2C Control Register 2 */
#define BCHP_SDS_MISC_2_IICCNT                   0x000c0628 /* MI2C Byte Count */
#define BCHP_SDS_MISC_2_IICHPA                   0x000c062c /* MI2C Chip Address */
#define BCHP_SDS_MISC_2_MIICTX1                  0x000c0630 /* MI2C TX Data Register 1 */
#define BCHP_SDS_MISC_2_MIICTX2                  0x000c0634 /* MI2C TX Data Register 2 */
#define BCHP_SDS_MISC_2_MIICRX1                  0x000c0638 /* MI2C RX Data Register 1 */
#define BCHP_SDS_MISC_2_MIICRX2                  0x000c063c /* MI2C RX Data Register 2 */
#define BCHP_SDS_MISC_2_MI2CSA                   0x000c0640 /* MI2C Status */
#define BCHP_SDS_MISC_2_TMRCTL                   0x000c0650 /* Timer Control (Formerly, BCKTMR) */
#define BCHP_SDS_MISC_2_GENTMR3                  0x000c0654 /* General Timer 3 Start Count */
#define BCHP_SDS_MISC_2_GENTMR2                  0x000c0658 /* General Timer 2 Start Count */
#define BCHP_SDS_MISC_2_GENTMR1                  0x000c065c /* General Timer 1 Start Count */
#define BCHP_SDS_MISC_2_BERTMR                   0x000c0660 /* BERT Timer Start Count */
#define BCHP_SDS_MISC_2_BTMR                     0x000c0664 /* Baud Clock Timer Start Count */
#define BCHP_SDS_MISC_2_TPDIR                    0x000c0668 /* Testport I/O direction register */
#define BCHP_SDS_MISC_2_TPODS                    0x000c066c /* Testport output data select register 3 */
#define BCHP_SDS_MISC_2_TPDS                     0x000c0670 /* Testport data select register */
#define BCHP_SDS_MISC_2_TPCTL1                   0x000c0674 /* Testport control register 1 */
#define BCHP_SDS_MISC_2_TPCTL2                   0x000c0678 /* Testport control register 2 */
#define BCHP_SDS_MISC_2_TPCTL3                   0x000c067c /* Testport control register 3 */
#define BCHP_SDS_MISC_2_TPOUT                    0x000c0680 /* Testport client mux */
#define BCHP_SDS_MISC_2_MISCTL                   0x000c0690 /* Miscellaneous control */
#define BCHP_SDS_MISC_2_INTR_RAW_STS0            0x000c0694 /* Raw interrupt status 0 */
#define BCHP_SDS_MISC_2_INTR_RAW_STS1            0x000c0698 /* Raw interrupt status 1 */

#endif /* #ifndef BCHP_SDS_MISC_2_H__ */

/* End of File */