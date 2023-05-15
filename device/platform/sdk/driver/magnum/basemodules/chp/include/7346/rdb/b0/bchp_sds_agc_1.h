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
 * $brcm_Workfile: bchp_sds_agc_1.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 4/14/11 5:20p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Wed Apr 13 13:32:01 2011
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
 * $brcm_Log: /magnum/basemodules/chp/7346/rdb/b0/bchp_sds_agc_1.h $
 * 
 * Hydra_Software_Devel/1   4/14/11 5:20p albertl
 * SW7346-143: Initial revision.
 *
 ***************************************************************************/

#ifndef BCHP_SDS_AGC_1_H__
#define BCHP_SDS_AGC_1_H__

/***************************************************************************
 *SDS_AGC_1 - SDS AGC Register Set 1
 ***************************************************************************/
#define BCHP_SDS_AGC_1_AGCCTL                    0x00701100 /* AGC Control (Formerly, AGCMISC, AGTCTL, AGICTL) */
#define BCHP_SDS_AGC_1_IAGCTH                    0x00701104 /* Internal tuner AGC threshold */
#define BCHP_SDS_AGC_1_DSGIN                     0x00701108 /* DSGIN Register */
#define BCHP_SDS_AGC_1_ATHR                      0x0070110c /* IF AGC loop threshold */
#define BCHP_SDS_AGC_1_ABW                       0x00701110 /* IF and RF AGC loop bandwidths */
#define BCHP_SDS_AGC_1_AII                       0x00701114 /* IF AGC integrator */
#define BCHP_SDS_AGC_1_AGI                       0x00701118 /* IF AGC gain threshold */
#define BCHP_SDS_AGC_1_AIT                       0x0070111c /* RF AGC integrator */
#define BCHP_SDS_AGC_1_AGT                       0x00701120 /* RF AGC gain threshold */
#define BCHP_SDS_AGC_1_AGCLI                     0x00701124 /* IF AGC delta-sigma fixed gain value and input select. */

#endif /* #ifndef BCHP_SDS_AGC_1_H__ */

/* End of File */