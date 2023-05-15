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
 * $brcm_Workfile: bchp_tert_sm.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 7/23/07 4:31p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Mon Jul 23 11:52:53 2007
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
 * $brcm_Log: /magnum/basemodules/chp/7400/rdb/d0/bchp_tert_sm.h $
 * 
 * Hydra_Software_Devel/1   7/23/07 4:31p maivu
 * PR 32842: Initial revision
 *
 ***************************************************************************/

#ifndef BCHP_TERT_SM_H__
#define BCHP_TERT_SM_H__

/***************************************************************************
 *TERT_SM - Primary Subcarrier Modulator
 ***************************************************************************/
#define BCHP_TERT_SM_SM_REV_ID                   0x00181e00 /* REVISION ID REGISTER */
#define BCHP_TERT_SM_ENVELOPE_GENERATOR          0x00181e08 /* ENVELOPE GENERATOR CONTROL REGISTER */
#define BCHP_TERT_SM_PG_CNTRL                    0x00181e0c /* PHASE GENERATOR CONTROL REGISTER */
#define BCHP_TERT_SM_PG_CONFIG                   0x00181e10 /* PHASE GENERATOR CONFIG REGISTER */
#define BCHP_TERT_SM_SC_FREQ_0                   0x00181e14 /* SUBCARRIER FREQUENCY 0 */
#define BCHP_TERT_SM_COMP_CNTRL                  0x00181e18 /* COMPOSITOR CONTROL REGISTER */
#define BCHP_TERT_SM_SA_CH2                      0x00181e1c /* CHANNEL2 SA STATUS REGISTER */
#define BCHP_TERT_SM_SA_CH1                      0x00181e20 /* CHANNEL1 SA STATUS REGISTER */
#define BCHP_TERT_SM_SA_CH0                      0x00181e24 /* CHANNEL0 SA STATUS REGISTER */
#define BCHP_TERT_SM_SA_CONFIG                   0x00181e28 /* SA CONFIGURATION REGISTER */
#define BCHP_TERT_SM_COMP_CONFIG                 0x00181e2c /* COMPOSITOR CONFIG REGISTER */
#define BCHP_TERT_SM_FM_CONTROL                  0x00181e30 /* FM Control Register */
#define BCHP_TERT_SM_FM_FMAMP                    0x00181e34 /* FM AMP Register */
#define BCHP_TERT_SM_FM_SHAPER                   0x00181e38 /* FM Shaper Register */
#define BCHP_TERT_SM_FM_SC_FREQ                  0x00181e3c /* FM Base Sub-carrier Frequency */

#endif /* #ifndef BCHP_TERT_SM_H__ */

/* End of File */