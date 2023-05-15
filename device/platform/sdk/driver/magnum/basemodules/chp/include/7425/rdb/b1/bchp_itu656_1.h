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
 * $brcm_Workfile: bchp_itu656_1.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 10/27/11 11:27p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Thu Oct 27 17:28:52 2011
 *                 MD5 Checksum         d0bb3b528cbe25f62f7a44e82cd25af7
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7425/rdb/b1/bchp_itu656_1.h $
 * 
 * Hydra_Software_Devel/1   10/27/11 11:27p vanessah
 * SW7425-1620: add 7425 B0 rdb header file
 *
 ***************************************************************************/

#ifndef BCHP_ITU656_1_H__
#define BCHP_ITU656_1_H__

/***************************************************************************
 *ITU656_1 - ITU 656 Formatter
 ***************************************************************************/
#define BCHP_ITU656_1_ITU656_REV_ID              0x00695c00 /* Revision ID Register */
#define BCHP_ITU656_1_ITU656_CONFIG              0x00695c08 /* ITU656 Encoder Config Register */
#define BCHP_ITU656_1_ITU656_STATUS              0x00695c0c /* ITU656 Encoder Status Register */
#define BCHP_ITU656_1_ITU656_SA_STATUS           0x00695c10 /* ITU656 Encoder SA Status Register */
#define BCHP_ITU656_1_ITU656_SA_CONFIG           0x00695c14 /* ITU656 Encoder SA Configuration Register */
#define BCHP_ITU656_1_ITU656_DELAY_COUNT         0x00695c18 /* ITU656 VBI Data Delay Count */
#define BCHP_ITU656_1_ITU656_TTE_CTRL            0x00695c1c /* TTE Serial Port Control Register */
#define BCHP_ITU656_1_ITU656_TTE_STATUS          0x00695c20 /* TTE Serial Port Status Register */

#endif /* #ifndef BCHP_ITU656_1_H__ */

/* End of File */