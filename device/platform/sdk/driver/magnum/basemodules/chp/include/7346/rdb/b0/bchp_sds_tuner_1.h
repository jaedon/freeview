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
 * $brcm_Workfile: bchp_sds_tuner_1.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 4/14/11 5:34p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Wed Apr 13 13:20:14 2011
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
 * $brcm_Log: /magnum/basemodules/chp/7346/rdb/b0/bchp_sds_tuner_1.h $
 * 
 * Hydra_Software_Devel/1   4/14/11 5:34p albertl
 * SW7346-143: Initial revision.
 *
 ***************************************************************************/

#ifndef BCHP_SDS_TUNER_1_H__
#define BCHP_SDS_TUNER_1_H__

/***************************************************************************
 *SDS_TUNER_1 - SDS Tuner Register Set 1
 ***************************************************************************/
#define BCHP_SDS_TUNER_1_DAISY_R01               0x00701900 /* DAISY_R01 Register */
#define BCHP_SDS_TUNER_1_BGBIAS_R01              0x00701904 /* BGBIAS_R01 Register */
#define BCHP_SDS_TUNER_1_REFPLL_R01              0x00701908 /* REFPLL_R01 Register */
#define BCHP_SDS_TUNER_1_REFPLL_R02              0x0070190c /* REFPLL_R02 Register */
#define BCHP_SDS_TUNER_1_REFPLL_R03              0x00701910 /* REFPLL_R03 Register */
#define BCHP_SDS_TUNER_1_REFPLL_R04              0x00701914 /* REFPLL_R04 Register */
#define BCHP_SDS_TUNER_1_ADC_R01                 0x00701918 /* ADC_R01    Register */
#define BCHP_SDS_TUNER_1_ADC_R02                 0x0070191c /* ADC_R02    Register */
#define BCHP_SDS_TUNER_1_ADC_R03                 0x00701920 /* ADC_R03    Register */
#define BCHP_SDS_TUNER_1_ADC_R04                 0x00701924 /* ADC_R04    Register */
#define BCHP_SDS_TUNER_1_IFPGA_R01               0x00701928 /* IFPGA_R01    Register */
#define BCHP_SDS_TUNER_1_LNA_R01                 0x0070192c /* LNA_R01    Register */
#define BCHP_SDS_TUNER_1_LNA_R02                 0x00701930 /* LNA_R02    Register */
#define BCHP_SDS_TUNER_1_LO_R01                  0x00701934 /* LO_R01    Register */
#define BCHP_SDS_TUNER_1_LO_R02                  0x00701938 /* LO_R02    Register */
#define BCHP_SDS_TUNER_1_LO_R03                  0x0070193c /* LO_R03    Register */
#define BCHP_SDS_TUNER_1_LO_R04                  0x00701940 /* LO_R04    Register */
#define BCHP_SDS_TUNER_1_LO_R05                  0x00701944 /* LO_R05    Register */
#define BCHP_SDS_TUNER_1_LO_R06                  0x00701948 /* LO_R06    Register */
#define BCHP_SDS_TUNER_1_LO_R07                  0x0070194c /* LO_R07    Register */
#define BCHP_SDS_TUNER_1_LPF_R01                 0x00701950 /* LPF_R01    Register */
#define BCHP_SDS_TUNER_1_RFPGA_R01               0x00701954 /* RFPGA_R01    Register */
#define BCHP_SDS_TUNER_1_BBAGC_R01               0x00701958 /* BBAGC_R01    Register */
#define BCHP_SDS_TUNER_1_BBAGC_R02               0x0070195c /* BBAGC_R02    Register */
#define BCHP_SDS_TUNER_1_BBAGC_R03               0x00701960 /* BBAGC_R03    Register */
#define BCHP_SDS_TUNER_1_BBAGC_R04               0x00701964 /* BBAGC_R04    Register */
#define BCHP_SDS_TUNER_1_BBAGC_R05               0x00701968 /* BBAGC_R05    Register */
#define BCHP_SDS_TUNER_1_CALDAC_R01              0x0070196c /* CALDAC_R01    Register */
#define BCHP_SDS_TUNER_1_LODDFS_R01              0x00701970 /* LODDFS_R01    Register */
#define BCHP_SDS_TUNER_1_LODDFS_R02              0x00701974 /* LODDFS_R02    Register */
#define BCHP_SDS_TUNER_1_MXFGA_R01               0x00701978 /* MXFGA_R01    Register */
#define BCHP_SDS_TUNER_1_MXFGA_R02               0x0070197c /* MXFGA_R02    Register */
#define BCHP_SDS_TUNER_1_POSTDCO_R01             0x00701980 /* POSTDCO_R01    Register */
#define BCHP_SDS_TUNER_1_PREDCO_R01              0x00701984 /* PREDCO_R01    Register */
#define BCHP_SDS_TUNER_1_RFAGC_R01               0x00701988 /* RFAGC_R01    Register */
#define BCHP_SDS_TUNER_1_RFAGC_R02               0x0070198c /* RFAGC_R02    Register */
#define BCHP_SDS_TUNER_1_RFAGC_R03               0x00701990 /* RFAGC_R03    Register */
#define BCHP_SDS_TUNER_1_RFAGC_R04               0x00701994 /* RFAGC_R04    Register */
#define BCHP_SDS_TUNER_1_RFAGC_R05               0x00701998 /* RFAGC_R05    Register */
#define BCHP_SDS_TUNER_1_DDFS_R01                0x007019a0 /* DDFS_R01    Register */
#define BCHP_SDS_TUNER_1_PLLSTAT_R01             0x007019a4 /* PLLSTAT_R01    Register */
#define BCHP_SDS_TUNER_1_PREDCOI_R01             0x007019a8 /* PREDCOI_R01    Register */
#define BCHP_SDS_TUNER_1_PREDCOI_R02             0x007019ac /* PREDCOI_R02    Register */
#define BCHP_SDS_TUNER_1_PREDCOI_R03             0x007019b0 /* PREDCOI_R03    Register */
#define BCHP_SDS_TUNER_1_PREDCOI_R04             0x007019b4 /* PREDCOI_R04    Register */
#define BCHP_SDS_TUNER_1_PREDCOQ_R01             0x007019b8 /* PREDCOQ_R01    Register */
#define BCHP_SDS_TUNER_1_PREDCOQ_R02             0x007019bc /* PREDCOQ_R02    Register */
#define BCHP_SDS_TUNER_1_PREDCOQ_R03             0x007019c0 /* PREDCOQ_R03    Register */
#define BCHP_SDS_TUNER_1_PREDCOQ_R04             0x007019c4 /* PREDCOQ_R04    Register */
#define BCHP_SDS_TUNER_1_POSTDCOI_R01            0x007019c8 /* POSTDCOI_R01    Register */
#define BCHP_SDS_TUNER_1_POSTDCOI_R02            0x007019cc /* POSTDCOI_R02    Register */
#define BCHP_SDS_TUNER_1_POSTDCOI_R03            0x007019d0 /* POSTDCOI_R03    Register */
#define BCHP_SDS_TUNER_1_POSTDCOI_R04            0x007019d4 /* POSTDCOI_R04    Register */
#define BCHP_SDS_TUNER_1_POSTDCOQ_R01            0x007019d8 /* POSTDCOQ_R01    Register */
#define BCHP_SDS_TUNER_1_POSTDCOQ_R02            0x007019dc /* POSTDCOQ_R02    Register */
#define BCHP_SDS_TUNER_1_POSTDCOQ_R03            0x007019e0 /* POSTDCOQ_R03    Register */
#define BCHP_SDS_TUNER_1_POSTDCOQ_R04            0x007019e4 /* POSTDCOQ_R04    Register */
#define BCHP_SDS_TUNER_1_RESET_R01               0x007019e8 /* RESET_R01       Register */
#define BCHP_SDS_TUNER_1_AGC_IF_CTL              0x007019ec /* AGC_IF_CTL Register */
#define BCHP_SDS_TUNER_1_FILCAL_DDFS_CTL         0x007019f0 /* FILCAL_DDFS_CTL Register */
#define BCHP_SDS_TUNER_1_TPCTL                   0x007019f4 /* TPCTL Register */
#define BCHP_SDS_TUNER_1_PWRUP_COMMON_R01        0x007019f8 /* PWRUP_COMMON_R01    Register */
#define BCHP_SDS_TUNER_1_PWRUP_R01               0x007019fc /* PWRUP_R01    Register */

#endif /* #ifndef BCHP_SDS_TUNER_1_H__ */

/* End of File */