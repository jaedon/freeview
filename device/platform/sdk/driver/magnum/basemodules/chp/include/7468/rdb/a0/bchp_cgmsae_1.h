/***************************************************************************
 *     Copyright (c) 1999-2009, Broadcom Corporation
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
 * $brcm_Date: 9/14/09 4:07p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Fri Sep 11 19:41:34 2009
 *                 MD5 Checksum         957f01e03a68c1766fd2e8ad6484f5f9
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7468/rdb/a0/bchp_cgmsae_1.h $
 * 
 * Hydra_Software_Devel/1   9/14/09 4:07p albertl
 * SW7468-3: Initial revision.
 *
 ***************************************************************************/

#ifndef BCHP_CGMSAE_1_H__
#define BCHP_CGMSAE_1_H__

/***************************************************************************
 *CGMSAE_1 - CGMSAE_1 registers
 ***************************************************************************/
#define BCHP_CGMSAE_1_Top_Control                0x00485804 /* CGMS-A Top-Field Control Register */
#define BCHP_CGMSAE_1_Bot_Control                0x00485808 /* CGMS-A Bottom-Field Control Register */
#define BCHP_CGMSAE_1_Top_Format                 0x0048580c /* CGMS-A Top-Field Format Register */
#define BCHP_CGMSAE_1_Bot_Format                 0x00485810 /* CGMS-A Bottom-Field Format Register */
#define BCHP_CGMSAE_1_Top_Data                   0x00485814 /* CGMS-A Top-Field Data Register */
#define BCHP_CGMSAE_1_Bot_Data                   0x00485818 /* CGMS-A Bottom-Field Data Register */
#define BCHP_CGMSAE_1_RevID                      0x0048581c /* CGMS-A Encoder Revision ID Register */
#define BCHP_CGMSAE_1_Top_Control_B              0x00485820 /* CGMS-B Top-Field Control Register */
#define BCHP_CGMSAE_1_Bot_Control_B              0x00485824 /* CGMS-B Bottom-Field Control Register */
#define BCHP_CGMSAE_1_Top_Format_B               0x00485828 /* CGMS-B Top-Field Format Register */
#define BCHP_CGMSAE_1_Bot_Format_B               0x0048582c /* CGMS-B Bottom-Field Format Register */
#define BCHP_CGMSAE_1_Top_Data_B0                0x00485830 /* CGMS-B Top-Field Data Register for 31:0 bits */
#define BCHP_CGMSAE_1_Top_Data_B1                0x00485834 /* CGMS-B Top-Field Data Register for 63:32 bits */
#define BCHP_CGMSAE_1_Top_Data_B2                0x00485838 /* CGMS-B Top-Field Data Register for 95:64 bits */
#define BCHP_CGMSAE_1_Top_Data_B3                0x0048583c /* CGMS-B Top-Field Data Register for 127:96 bits */
#define BCHP_CGMSAE_1_Top_Data_B4                0x00485840 /* CGMS-B Top-Field Data Register for 159:128 bits */
#define BCHP_CGMSAE_1_Bot_Data_B0                0x00485844 /* CGMS-B Bottom-Field Data Register for 31:0 bits */
#define BCHP_CGMSAE_1_Bot_Data_B1                0x00485848 /* CGMS-B Bottom-Field Data Register for 63:32 bits */
#define BCHP_CGMSAE_1_Bot_Data_B2                0x0048584c /* CGMS-B Bottom-Field Data Register for 95:64 bits */
#define BCHP_CGMSAE_1_Bot_Data_B3                0x00485850 /* CGMS-B Bottom-Field Data Register for 127:96 bits */
#define BCHP_CGMSAE_1_Bot_Data_B4                0x00485854 /* CGMS-B Bottom-Field Data Register for 159:128 bits */
#define BCHP_CGMSAE_1_Reg_updt                   0x00485858 /* Data Update Register */

#endif /* #ifndef BCHP_CGMSAE_1_H__ */

/* End of File */