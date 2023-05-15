/****************************************************************************
 *     Copyright (c) 1999-2014, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on              Thu Feb 20 15:33:52 2014
 *                 Full Compile MD5 Checksum 1766fea499add5f6ee91330ef96d35c5
 *                   (minus title and desc)
 *                 MD5 Checksum              4c358fb5b94802f03aec82d8df2c9afa
 *
 * Compiled with:  RDB Utility               combo_header.pl
 *                 RDB Parser                3.0
 *                 unknown                   unknown
 *                 Perl Interpreter          5.008008
 *                 Operating System          linux
 *
 *
 ***************************************************************************/



/****************************************************************************
 ***************************************************************************/

#ifndef BCHP_SDS_BERT_7_H__
#define BCHP_SDS_BERT_7_H__

/***************************************************************************
 *SDS_BERT_7 - SDS BERT Register Set
 ***************************************************************************/
#define BCHP_SDS_BERT_7_BERCTL                   0x04700540 /* BERT Control Register (Formerly,BERCTLA,BERCTLB) */
#define BCHP_SDS_BERT_7_BEIT                     0x04700544 /* BERT integration period and threshold */
#define BCHP_SDS_BERT_7_BIT_CNT_H                0x04700548 /* BERT bit Counter snapshot value, higher bits */
#define BCHP_SDS_BERT_7_BIT_CNT_L                0x0470054c /* BERT bit Counter snapshot value, lower bits */
#define BCHP_SDS_BERT_7_ERR_CNT_H                0x04700550 /* BERT error Counter snapshot value, higher bits */
#define BCHP_SDS_BERT_7_ERR_CNT_L                0x04700554 /* BERT error Counter snapshot value, lower bits */
#define BCHP_SDS_BERT_7_BEM1                     0x04700558 /* BERT 16-QAM or 8-PSK symbol mapping values */
#define BCHP_SDS_BERT_7_BEM2                     0x0470055c /* BERT 16-QAM symbol mapping values */
#define BCHP_SDS_BERT_7_BEM3                     0x04700560 /* BERT QPSK symbol mapping values */
#define BCHP_SDS_BERT_7_BEST                     0x04700564 /* BERT status register */
#define BCHP_SDS_BERT_7_ACMCTL                   0x04700568 /* BERT ACM Control register */

#endif /* #ifndef BCHP_SDS_BERT_7_H__ */

/* End of File */