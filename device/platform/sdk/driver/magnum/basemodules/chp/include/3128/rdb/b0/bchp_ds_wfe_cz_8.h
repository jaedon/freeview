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
 * $brcm_Workfile: bchp_ds_wfe_cz_8.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 9/13/11 10:48a $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Thu Apr 14 10:09:55 2011
 *                 MD5 Checksum         b0ba84764310f22a148b48dae3bcde09
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/3128/rdb/b0/bchp_ds_wfe_cz_8.h $
 * 
 * Hydra_Software_Devel/2   9/13/11 10:48a farshidf
 * SW3128-1: update the the header files from Dorothy
 * 
 * Hydra_Software_Devel/2   9/13/11 10:46a farshidf
 * SW3128-1: update the the header files from Dorothy
 * 
 * Hydra_Software_Devel/1   4/14/11 12:09p farshidf
 * SW3128-5: add 3128 RDb for b0
 *
 ***************************************************************************/

#ifndef BCHP_DS_WFE_CZ_8_H__
#define BCHP_DS_WFE_CZ_8_H__

/***************************************************************************
 *DS_WFE_CZ_8 - WFE Channel Processor 8 Configuration Registers
 ***************************************************************************/
#define BCHP_DS_WFE_CZ_8_SRC_ID                  0x0009c000 /* Source channel bin ID */
#define BCHP_DS_WFE_CZ_8_QMIX_FCW                0x0009c004 /* Channel Processor Mixer Frequency Control Word */
#define BCHP_DS_WFE_CZ_8_AGF                     0x0009c008 /* Digital AGC(Fine) Control Register */
#define BCHP_DS_WFE_CZ_8_AGFI                    0x0009c00c /* Digital AGC(Fine) Integrator Value */
#define BCHP_DS_WFE_CZ_8_AGFLI                   0x0009c010 /* Digital AGC(Fine) Leaky Integrator Value */

/***************************************************************************
 *SRC_ID - Source channel bin ID
 ***************************************************************************/
/* DS_WFE_CZ_0 :: SRC_ID :: BIN_ID [07:02] */
#define BCHP_DS_WFE_CZ_8_SRC_ID_BIN_ID_MASK                        0x000000fc
#define BCHP_DS_WFE_CZ_8_SRC_ID_BIN_ID_SHIFT                       2
#define BCHP_DS_WFE_CZ_8_SRC_ID_BIN_ID_DEFAULT                     0

/***************************************************************************
 *QMIX_FCW - Channel Processor Mixer Frequency Control Word
 ***************************************************************************/
/* DS_WFE_CZ_0 :: QMIX_FCW :: reserved0 [31:30] */
#define BCHP_DS_WFE_CZ_8_QMIX_FCW_reserved0_MASK                   0xc0000000
#define BCHP_DS_WFE_CZ_8_QMIX_FCW_reserved0_SHIFT                  30

/* DS_WFE_CZ_0 :: QMIX_FCW :: FCW [29:00] */
#define BCHP_DS_WFE_CZ_8_QMIX_FCW_FCW_MASK                         0x3fffffff
#define BCHP_DS_WFE_CZ_8_QMIX_FCW_FCW_SHIFT                        0
#define BCHP_DS_WFE_CZ_8_QMIX_FCW_FCW_DEFAULT                      0

#endif /* #ifndef BCHP_DS_WFE_CZ_8_H__ */

/* End of File */