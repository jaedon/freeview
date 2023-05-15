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
 * $brcm_Workfile: bchp_tfec_1.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 4/14/11 5:50p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Wed Apr 13 13:24:48 2011
 *                 MD5 Checksum         c1b53a9408066d1e6123c0860dfb8065
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7346/rdb/b0/bchp_tfec_1.h $
 * 
 * Hydra_Software_Devel/1   4/14/11 5:50p albertl
 * SW7346-143: Initial revision.
 *
 ***************************************************************************/

#ifndef BCHP_TFEC_1_H__
#define BCHP_TFEC_1_H__

/***************************************************************************
 *TFEC_1 - SDS TFEC Register Set 1
 ***************************************************************************/
#define BCHP_TFEC_1_TFECTL                       0x00703000 /* Advanced FEC Control */
#define BCHP_TFEC_1_TNBLK                        0x00703014 /* RS total block count */
#define BCHP_TFEC_1_TCBLK                        0x00703018 /* RS corrected block count */
#define BCHP_TFEC_1_TBBLK                        0x0070301c /* RS bad block count */
#define BCHP_TFEC_1_TCSYM                        0x00703020 /* RS corrected symbol count */
#define BCHP_TFEC_1_TFMT                         0x00703024 /* RS parameters */
#define BCHP_TFEC_1_TPAK                         0x00703028 /* MPEG packetizer parameters */
#define BCHP_TFEC_1_TSSQ                         0x0070302c /* Advanced FEC symbol sequence */
#define BCHP_TFEC_1_TSYN                         0x00703030 /* MPEG synchronizer parameters */
#define BCHP_TFEC_1_TTUR                         0x00703034 /* Advanced FEC configuration parameters */
#define BCHP_TFEC_1_TZPK                         0x00703038 /* RS synchronizer parameters */
#define BCHP_TFEC_1_TZSY                         0x0070303c /* RS synchronizer aquisition and retention parameters */
#define BCHP_TFEC_1_TITR                         0x00703040 /* Iterative decoder parameters */
#define BCHP_TFEC_1_TCIL                         0x00703044 /* Iterative decoder parameters */
#define BCHP_TFEC_1_TRSD                         0x00703048 /* RS decoder parameters */
#define BCHP_TFEC_1_TPN                          0x0070304c /* Iterative decoder internal point signature analyzer output */
#define BCHP_TFEC_1_TSIGCNT                      0x00703050 /* Signature analyzer data length count */
#define BCHP_TFEC_1_TSIGITD                      0x00703054 /* Iterative decoder signature analyzer output */
#define BCHP_TFEC_1_TSIGXPT                      0x00703058 /* Transport output signature analyzer output */
#define BCHP_TFEC_1_TTPCTL                       0x0070305c /* Advanced FEC testport control */
#define BCHP_TFEC_1_TRAWISR                      0x00703060 /* Advanced FEC raw interrupt status */

#endif /* #ifndef BCHP_TFEC_1_H__ */

/* End of File */