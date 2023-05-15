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
 * $brcm_Workfile: bchp_cgmsae_1.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 2/28/12 3:26p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Tue Feb 28 11:03:18 2012
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
 * $brcm_Log: /magnum/basemodules/chp/7435/rdb/b0/bchp_cgmsae_1.h $
 * 
 * Hydra_Software_Devel/1   2/28/12 3:26p tdo
 * SW7435-40: Need 7435B0 public/central RDB (magnum) headers checked into
 * clearcase
 *
 ***************************************************************************/

#ifndef BCHP_CGMSAE_1_H__
#define BCHP_CGMSAE_1_H__

/***************************************************************************
 *CGMSAE_1 - CGMSAE_1 registers
 ***************************************************************************/
#define BCHP_CGMSAE_1_Top_Control                0x006b8904 /* CGMS-A Top-Field Control Register */
#define BCHP_CGMSAE_1_Bot_Control                0x006b8908 /* CGMS-A Bottom-Field Control Register */
#define BCHP_CGMSAE_1_Top_Format                 0x006b890c /* CGMS-A Top-Field Format Register */
#define BCHP_CGMSAE_1_Bot_Format                 0x006b8910 /* CGMS-A Bottom-Field Format Register */
#define BCHP_CGMSAE_1_Top_Data                   0x006b8914 /* CGMS-A Top-Field Data Register */
#define BCHP_CGMSAE_1_Bot_Data                   0x006b8918 /* CGMS-A Bottom-Field Data Register */
#define BCHP_CGMSAE_1_RevID                      0x006b891c /* CGMS-A Encoder Revision ID Register */
#define BCHP_CGMSAE_1_Top_Control_B              0x006b8920 /* CGMS-B Top-Field Control Register */
#define BCHP_CGMSAE_1_Bot_Control_B              0x006b8924 /* CGMS-B Bottom-Field Control Register */
#define BCHP_CGMSAE_1_Top_Format_B               0x006b8928 /* CGMS-B Top-Field Format Register */
#define BCHP_CGMSAE_1_Bot_Format_B               0x006b892c /* CGMS-B Bottom-Field Format Register */
#define BCHP_CGMSAE_1_Top_Data_B0                0x006b8930 /* CGMS-B Top-Field Data Register for 31:0 bits */
#define BCHP_CGMSAE_1_Top_Data_B1                0x006b8934 /* CGMS-B Top-Field Data Register for 63:32 bits */
#define BCHP_CGMSAE_1_Top_Data_B2                0x006b8938 /* CGMS-B Top-Field Data Register for 95:64 bits */
#define BCHP_CGMSAE_1_Top_Data_B3                0x006b893c /* CGMS-B Top-Field Data Register for 127:96 bits */
#define BCHP_CGMSAE_1_Top_Data_B4                0x006b8940 /* CGMS-B Top-Field Data Register for 159:128 bits */
#define BCHP_CGMSAE_1_Bot_Data_B0                0x006b8944 /* CGMS-B Bottom-Field Data Register for 31:0 bits */
#define BCHP_CGMSAE_1_Bot_Data_B1                0x006b8948 /* CGMS-B Bottom-Field Data Register for 63:32 bits */
#define BCHP_CGMSAE_1_Bot_Data_B2                0x006b894c /* CGMS-B Bottom-Field Data Register for 95:64 bits */
#define BCHP_CGMSAE_1_Bot_Data_B3                0x006b8950 /* CGMS-B Bottom-Field Data Register for 127:96 bits */
#define BCHP_CGMSAE_1_Bot_Data_B4                0x006b8954 /* CGMS-B Bottom-Field Data Register for 159:128 bits */
#define BCHP_CGMSAE_1_Reg_updt                   0x006b8958 /* Data Update Register */

#endif /* #ifndef BCHP_CGMSAE_1_H__ */

/* End of File */