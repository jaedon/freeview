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
 * $brcm_Workfile: bchp_sds_oi_4.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 1/13/11 8:09a $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Wed Jan 12 18:40:56 2011
 *                 MD5 Checksum         6e6727f6c827233acdd395c9a9032c98
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/4528/rdb/a0/bchp_sds_oi_4.h $
 * 
 * Hydra_Software_Devel/1   1/13/11 8:09a pntruong
 * HW4528-2: Synced up with central rdb.
 *
 ***************************************************************************/

#ifndef BCHP_SDS_OI_4_H__
#define BCHP_SDS_OI_4_H__

/***************************************************************************
 *SDS_OI_4 - SDS Output Interface Register Set 4
 ***************************************************************************/
#define BCHP_SDS_OI_4_OIFCTL00                   0x000d0480 /* Output Interface Control 00 (Formerly, OIFCTL2,OIFCTL,PSCTL) */
#define BCHP_SDS_OI_4_OIFCTL01                   0x000d0484 /* Output Interface Control 01 (Formerly, OIFCTL4,OIFCTL3[15:0]) */
#define BCHP_SDS_OI_4_OIFCTL02                   0x000d0488 /* Output Interface Control 02 (Formerly, OIFCTL5) */
#define BCHP_SDS_OI_4_OPLL                       0x000d048c /* Output Interface NCO clock rate numerator */
#define BCHP_SDS_OI_4_OPLL2                      0x000d0490 /* Output Interface NCO clock rate delta */
#define BCHP_SDS_OI_4_FERC                       0x000d0494 /* Output Interface MPEG frame error counter */
#define BCHP_SDS_OI_4_FRC                        0x000d0498 /* Output Interface MPEG frame counter */
#define BCHP_SDS_OI_4_OSIGPN                     0x000d049c /* Output Interface signature analyzer */
#define BCHP_SDS_OI_4_OSUBD                      0x000d04a0 /* Output Interface PLL Sub-divider control */
#define BCHP_SDS_OI_4_OCOEF                      0x000d04a4 /* Output Interface Loop Filter Coefficient */
#define BCHP_SDS_OI_4_OFI                        0x000d04a8 /* Output Interface Loop Filter Integrator */
#define BCHP_SDS_OI_4_OPLL_NPDIV                 0x000d04b0 /* Output Interface PLL Feedback Divider Control */
#define BCHP_SDS_OI_4_OPLL_MDIV_CTRL             0x000d04b4 /* Output Interface PLL Post-divider Control */
#define BCHP_SDS_OI_4_OPLL_CTRL                  0x000d04b8 /* Output Interface PLL Control */
#define BCHP_SDS_OI_4_OPLL_SSC_CTRL1             0x000d04bc /* Output Interface PLL Spread Spectrum Control 1 */
#define BCHP_SDS_OI_4_OPLL_SSC_CTRL0             0x000d04c0 /* Output Interface PLL Spread Spectrum Control 0 */
#define BCHP_SDS_OI_4_OPLL_STATUS                0x000d04c4 /* Output Interface PLL Status */
#define BCHP_SDS_OI_4_OPLL_PWRDN_RST             0x000d04c8 /* Output Interface PLL Power Down and Reset Control */

#endif /* #ifndef BCHP_SDS_OI_4_H__ */

/* End of File */