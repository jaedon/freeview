/***************************************************************************
 *     Copyright (c) 1999-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bchp_cgmsae_tert.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 7/23/07 1:54p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Mon Jul 23 11:53:29 2007
 *                 MD5 Checksum         61f9c4d8dcdcd06017506dddbf23f434
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008004
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7400/rdb/d0/bchp_cgmsae_tert.h $
 * 
 * Hydra_Software_Devel/1   7/23/07 1:54p maivu
 * PR 32842: Initial revision
 *
 ***************************************************************************/

#ifndef BCHP_CGMSAE_TERT_H__
#define BCHP_CGMSAE_TERT_H__

/***************************************************************************
 *CGMSAE_TERT - TERT CGMSA Encoder
 ***************************************************************************/
#define BCHP_CGMSAE_TERT_Top_Control             0x00188084 /* CGMS-A Top-Field Control Register */
#define BCHP_CGMSAE_TERT_Bot_Control             0x00188088 /* CGMS-A Bottom-Field Control Register */
#define BCHP_CGMSAE_TERT_Top_Format              0x0018808c /* CGMS-A Top-Field Format Register */
#define BCHP_CGMSAE_TERT_Bot_Format              0x00188090 /* CGMS-A Bottom-Field Format Register */
#define BCHP_CGMSAE_TERT_Top_Data                0x00188094 /* CGMS-A Top-Field Data Register */
#define BCHP_CGMSAE_TERT_Bot_Data                0x00188098 /* CGMS-A Bottom-Field Data Register */
#define BCHP_CGMSAE_TERT_RevID                   0x0018809c /* CGMS-A Encoder Revision ID Register */

#endif /* #ifndef BCHP_CGMSAE_TERT_H__ */

/* End of File */