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
 * $brcm_Workfile: bchp_sds_cwc_1.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 5/24/11 5:55p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Tue May 24 13:01:00 2011
 *                 MD5 Checksum         20406eb4287081a441630135cf8f15e2
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/7346/rdb/b0/bchp_sds_cwc_1.h $
 * 
 * Hydra_Software_Devel/2   5/24/11 5:55p albertl
 * SW7346-143: Updated to match RDB.
 *
 ***************************************************************************/

#ifndef BCHP_SDS_CWC_1_H__
#define BCHP_SDS_CWC_1_H__

/***************************************************************************
 *SDS_CWC_1 - SDS CW Canceller Register Set 1
 ***************************************************************************/
#define BCHP_SDS_CWC_1_CTRL1                     0x00701600 /* CWC Control 1 */
#define BCHP_SDS_CWC_1_CTRL2                     0x00701604 /* CWC Control 2 */
#define BCHP_SDS_CWC_1_LEAK                      0x00701608 /* CWC Leak Control */
#define BCHP_SDS_CWC_1_SPUR_FCW1                 0x00701610 /* CWC1 NCO frequency control word */
#define BCHP_SDS_CWC_1_SPUR_FCW2                 0x00701614 /* CWC2 NCO frequency control word */
#define BCHP_SDS_CWC_1_SPUR_FCW3                 0x00701618 /* CWC3 NCO frequency control word */
#define BCHP_SDS_CWC_1_SPUR_FCW4                 0x0070161c /* CWC4 NCO frequency control word */
#define BCHP_SDS_CWC_1_SPUR_FCW5                 0x00701620 /* CWC5 NCO frequency control word */
#define BCHP_SDS_CWC_1_SPUR_FCW6                 0x00701624 /* CWC6 NCO frequency control word */
#define BCHP_SDS_CWC_1_LFC1                      0x00701630 /* CWC1 Loop Filter Coefficients */
#define BCHP_SDS_CWC_1_LFC2                      0x00701634 /* CWC2 Loop Filter Coefficients */
#define BCHP_SDS_CWC_1_LFC3                      0x00701638 /* CWC3 Loop Filter Coefficients */
#define BCHP_SDS_CWC_1_LFC4                      0x0070163c /* CWC4 Loop Filter Coefficients */
#define BCHP_SDS_CWC_1_LFC5                      0x00701640 /* CWC5 Loop Filter Coefficients */
#define BCHP_SDS_CWC_1_LFC6                      0x00701644 /* CWC6 Loop Filter Coefficients */
#define BCHP_SDS_CWC_1_INT1                      0x00701650 /* CWC1 Loop Filter INtegrator */
#define BCHP_SDS_CWC_1_INT2                      0x00701654 /* CWC2 Loop Filter INtegrator */
#define BCHP_SDS_CWC_1_INT3                      0x00701658 /* CWC3 Loop Filter INtegrator */
#define BCHP_SDS_CWC_1_INT4                      0x0070165c /* CWC4 Loop Filter INtegrator */
#define BCHP_SDS_CWC_1_INT5                      0x00701660 /* CWC5 Loop Filter INtegrator */
#define BCHP_SDS_CWC_1_INT6                      0x00701664 /* CWC6 Loop Filter INtegrator */
#define BCHP_SDS_CWC_1_COEFF_RWCTRL              0x00701670 /* CWC Coefficient Read/Write Control */
#define BCHP_SDS_CWC_1_COEFF                     0x00701674 /* CWC Coefficients */
#define BCHP_SDS_CWC_1_FOFS1                     0x00701680 /* Frequency Offset for CWC1 */
#define BCHP_SDS_CWC_1_FOFS2                     0x00701684 /* Frequency Offset for CWC2 */
#define BCHP_SDS_CWC_1_FOFS3                     0x00701688 /* Frequency Offset for CWC3 */
#define BCHP_SDS_CWC_1_FOFS4                     0x0070168c /* Frequency Offset for CWC4 */
#define BCHP_SDS_CWC_1_FOFS5                     0x00701690 /* Frequency Offset for CWC5 */
#define BCHP_SDS_CWC_1_FOFS6                     0x00701694 /* Frequency Offset for CWC6 */

#endif /* #ifndef BCHP_SDS_CWC_1_H__ */

/* End of File */