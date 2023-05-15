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

#ifndef BCHP_SDS_OI_1_H__
#define BCHP_SDS_OI_1_H__

/***************************************************************************
 *SDS_OI_1 - SDS Output Interface Register Set
 ***************************************************************************/
#define BCHP_SDS_OI_1_OIFCTL00                   0x04100480 /* Output Interface Control 00 (Formerly, OIFCTL2,OIFCTL,PSCTL) */
#define BCHP_SDS_OI_1_OIFCTL01                   0x04100484 /* Output Interface Control 01 (Formerly, OIFCTL4,OIFCTL3[15:0]) */
#define BCHP_SDS_OI_1_OIFCTL02                   0x04100488 /* Output Interface Control 02 (Formerly, OIFCTL5) */
#define BCHP_SDS_OI_1_OPLL                       0x0410048c /* Output Interface NCO clock rate numerator */
#define BCHP_SDS_OI_1_OPLL2                      0x04100490 /* Output Interface NCO clock rate delta */
#define BCHP_SDS_OI_1_FERC                       0x04100494 /* Output Interface MPEG frame error counter */
#define BCHP_SDS_OI_1_FRC                        0x04100498 /* Output Interface MPEG frame counter */
#define BCHP_SDS_OI_1_OSIGPN                     0x0410049c /* Output Interface signature analyzer */
#define BCHP_SDS_OI_1_OSUBD                      0x041004a0 /* Output Interface PLL Sub-divider control */
#define BCHP_SDS_OI_1_OCOEF                      0x041004a4 /* Output Interface Loop Filter Coefficient */
#define BCHP_SDS_OI_1_OFI                        0x041004a8 /* Output Interface Loop Filter Integrator */
#define BCHP_SDS_OI_1_OPLL_NPDIV                 0x041004b0 /* Output Interface PLL Feedback Divider Control */
#define BCHP_SDS_OI_1_OPLL_MDIV_CTRL             0x041004b4 /* Output Interface PLL Post-divider Control */
#define BCHP_SDS_OI_1_OPLL_CTRL                  0x041004b8 /* Output Interface PLL Control */
#define BCHP_SDS_OI_1_OPLL_SSC_CTRL1             0x041004bc /* Output Interface PLL Spread Spectrum Control 1 */
#define BCHP_SDS_OI_1_OPLL_SSC_CTRL0             0x041004c0 /* Output Interface PLL Spread Spectrum Control 0 */
#define BCHP_SDS_OI_1_OPLL_STATUS                0x041004c4 /* Output Interface PLL Status */
#define BCHP_SDS_OI_1_OPLL_PWRDN_RST             0x041004c8 /* Output Interface PLL Power Down and Reset Control */
#define BCHP_SDS_OI_1_OPLL_CTRL2                 0x041004cc /* Output Interface PLL Control 2 */

#endif /* #ifndef BCHP_SDS_OI_1_H__ */

/* End of File */