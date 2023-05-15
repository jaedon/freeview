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
 * $brcm_Workfile: bchp_gse_1.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 4/4/11 1:28p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Fri Apr  1 16:50:45 2011
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
 * $brcm_Log: /magnum/basemodules/chp/7344/rdb/b0/bchp_gse_1.h $
 * 
 * Hydra_Software_Devel/1   4/4/11 1:28p albertl
 * SW7344-40: Initial revision.
 *
 ***************************************************************************/

#ifndef BCHP_GSE_1_H__
#define BCHP_GSE_1_H__

/***************************************************************************
 *GSE_1 - GSE_1 registers
 ***************************************************************************/
#define BCHP_GSE_1_REVID                         0x00186d00 /* GSTAR Encoder Revision ID Register */
#define BCHP_GSE_1_CONTROL                       0x00186d08 /* Control Register */
#define BCHP_GSE_1_ACTIVE_LINE_TOP               0x00186d0c /* Top Field Active Line and Pedestal Enable Register */
#define BCHP_GSE_1_ACTIVE_LINE_BOT               0x00186d10 /* Bottom Field Active Line and Pedestal Enable Register */
#define BCHP_GSE_1_GAIN_TOP                      0x00186d14 /* Top Field Gain Register */
#define BCHP_GSE_1_GAIN_EXT_TOP                  0x00186d18 /* Top Field Line5 Gain Register */
#define BCHP_GSE_1_GAIN_BOT                      0x00186d1c /* Bottom Field Gain Register */
#define BCHP_GSE_1_GAIN_EXT_BOT                  0x00186d20 /* Bottom Field Line5 Gain Register */
#define BCHP_GSE_1_DATA_LINE1_BANK0              0x00186d24 /* Bank 0 Line 1 Data Register */
#define BCHP_GSE_1_DATA_LINE2_BANK0              0x00186d28 /* Bank 0 Line 2 Data Register */
#define BCHP_GSE_1_DATA_LINE3_BANK0              0x00186d2c /* Bank 0 Line 3 Data Register */
#define BCHP_GSE_1_DATA_LINE4_BANK0              0x00186d30 /* Bank 0 Line 4 Data Register */
#define BCHP_GSE_1_DATA_LINE5_BANK0              0x00186d34 /* Bank 0 Line 5 Data Register */
#define BCHP_GSE_1_DATA_LINE1_BANK1              0x00186d38 /* Bank 1 Line 1 Data Register */
#define BCHP_GSE_1_DATA_LINE2_BANK1              0x00186d3c /* Bank 1 Line 2 Data Register */
#define BCHP_GSE_1_DATA_LINE3_BANK1              0x00186d40 /* Bank 1 Line 3 Data Register */
#define BCHP_GSE_1_DATA_LINE4_BANK1              0x00186d44 /* Bank 1 Line 4 Data Register */
#define BCHP_GSE_1_DATA_LINE5_BANK1              0x00186d48 /* Bank 1 Line 5 Data Register */
#define BCHP_GSE_1_DATA_LINE1_BANK2              0x00186d4c /* Bank 2 Line 1 Data Register */
#define BCHP_GSE_1_DATA_LINE2_BANK2              0x00186d50 /* Bank 2 Line 2 Data Register */
#define BCHP_GSE_1_DATA_LINE3_BANK2              0x00186d54 /* Bank 2 Line 3 Data Register */
#define BCHP_GSE_1_DATA_LINE4_BANK2              0x00186d58 /* Bank 2 Line 4 Data Register */
#define BCHP_GSE_1_DATA_LINE5_BANK2              0x00186d5c /* Bank 2 Line 5 Data Register */
#define BCHP_GSE_1_DATA_LINE1_BANK3              0x00186d60 /* Bank 3 Line 1 Data Register */
#define BCHP_GSE_1_DATA_LINE2_BANK3              0x00186d64 /* Bank 3 Line 2 Data Register */
#define BCHP_GSE_1_DATA_LINE3_BANK3              0x00186d68 /* Bank 3 Line 3 Data Register */
#define BCHP_GSE_1_DATA_LINE4_BANK3              0x00186d6c /* Bank 3 Line 4 Data Register */
#define BCHP_GSE_1_DATA_LINE5_BANK3              0x00186d70 /* Bank 3 Line 5 Data Register */
#define BCHP_GSE_1_NULL                          0x00186d74 /* NULL Register */
#define BCHP_GSE_1_WRPTR                         0x00186d78 /* Write Pointer Register */
#define BCHP_GSE_1_RDPTR                         0x00186d7c /* Read Pointer Register */
#define BCHP_GSE_1_STATUS                        0x00186d80 /* Status Register */

#endif /* #ifndef BCHP_GSE_1_H__ */

/* End of File */