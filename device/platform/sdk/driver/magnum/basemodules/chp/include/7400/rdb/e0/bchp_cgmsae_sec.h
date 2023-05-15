/***************************************************************************
 *     Copyright (c) 1999-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bchp_cgmsae_sec.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 3/10/08 8:22p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Mon Mar 10 16:38:54 2008
 *                 MD5 Checksum         cf66689754e95804a8fa320d4ee9eb18
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7400/rdb/e0/bchp_cgmsae_sec.h $
 * 
 * Hydra_Software_Devel/2   3/10/08 8:22p pntruong
 * PR38572:  Synced with central rdb.
 *
 ***************************************************************************/

#ifndef BCHP_CGMSAE_SEC_H__
#define BCHP_CGMSAE_SEC_H__

/***************************************************************************
 *CGMSAE_SEC - SEC CGMSA Encoder
 ***************************************************************************/
#define BCHP_CGMSAE_SEC_Top_Control              0x00186084 /* CGMS-A Top-Field Control Register */
#define BCHP_CGMSAE_SEC_Bot_Control              0x00186088 /* CGMS-A Bottom-Field Control Register */
#define BCHP_CGMSAE_SEC_Top_Format               0x0018608c /* CGMS-A Top-Field Format Register */
#define BCHP_CGMSAE_SEC_Bot_Format               0x00186090 /* CGMS-A Bottom-Field Format Register */
#define BCHP_CGMSAE_SEC_Top_Data                 0x00186094 /* CGMS-A Top-Field Data Register */
#define BCHP_CGMSAE_SEC_Bot_Data                 0x00186098 /* CGMS-A Bottom-Field Data Register */
#define BCHP_CGMSAE_SEC_RevID                    0x0018609c /* CGMS-A Encoder Revision ID Register */
#define BCHP_CGMSAE_SEC_Top_Control_B            0x001860a0 /* CGMS-B Top-Field Control Register */
#define BCHP_CGMSAE_SEC_Bot_Control_B            0x001860a4 /* CGMS-B Bottom-Field Control Register */
#define BCHP_CGMSAE_SEC_Top_Format_B             0x001860a8 /* CGMS-B Top-Field Format Register */
#define BCHP_CGMSAE_SEC_Bot_Format_B             0x001860ac /* CGMS-B Bottom-Field Format Register */
#define BCHP_CGMSAE_SEC_Top_Data_B0              0x001860b0 /* CGMS-B Top-Field Data Register for 31:0 bits */
#define BCHP_CGMSAE_SEC_Top_Data_B1              0x001860b4 /* CGMS-B Top-Field Data Register for 63:32 bits */
#define BCHP_CGMSAE_SEC_Top_Data_B2              0x001860b8 /* CGMS-B Top-Field Data Register for 95:64 bits */
#define BCHP_CGMSAE_SEC_Top_Data_B3              0x001860bc /* CGMS-B Top-Field Data Register for 127:96 bits */
#define BCHP_CGMSAE_SEC_Top_Data_B4              0x001860c0 /* CGMS-B Top-Field Data Register for 159:128 bits */
#define BCHP_CGMSAE_SEC_Bot_Data_B0              0x001860c4 /* CGMS-B Bottom-Field Data Register for 31:0 bits */
#define BCHP_CGMSAE_SEC_Bot_Data_B1              0x001860c8 /* CGMS-B Bottom-Field Data Register for 63:32 bits */
#define BCHP_CGMSAE_SEC_Bot_Data_B2              0x001860cc /* CGMS-B Bottom-Field Data Register for 95:64 bits */
#define BCHP_CGMSAE_SEC_Bot_Data_B3              0x001860d0 /* CGMS-B Bottom-Field Data Register for 127:96 bits */
#define BCHP_CGMSAE_SEC_Bot_Data_B4              0x001860d4 /* CGMS-B Bottom-Field Data Register for 159:128 bits */
#define BCHP_CGMSAE_SEC_Reg_updt                 0x001860d8 /* Data Update Register */

#endif /* #ifndef BCHP_CGMSAE_SEC_H__ */

/* End of File */