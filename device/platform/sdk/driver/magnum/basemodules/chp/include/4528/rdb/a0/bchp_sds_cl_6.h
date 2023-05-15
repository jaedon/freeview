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
 * $brcm_Workfile: bchp_sds_cl_6.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 1/12/11 8:15p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Wed Jan 12 18:41:17 2011
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
 * $brcm_Log: /magnum/basemodules/chp/4528/rdb/a0/bchp_sds_cl_6.h $
 * 
 * Hydra_Software_Devel/1   1/12/11 8:15p pntruong
 * HW4528-2: Synced up with central rdb.
 *
 ***************************************************************************/

#ifndef BCHP_SDS_CL_6_H__
#define BCHP_SDS_CL_6_H__

/***************************************************************************
 *SDS_CL_6 - SDS Carrier Loop Register Set 6
 ***************************************************************************/
#define BCHP_SDS_CL_6_CLCTL1                     0x000e0180 /* Carrier Loop Control 1 (Formerly, CLQCFD,CLPDCTL,CLCTL) */
#define BCHP_SDS_CL_6_CLCTL2                     0x000e0184 /* Carrier Loop Control 2 (Formerly, CLOON,CLSTS,CLMISC2,CLMISC) */
#define BCHP_SDS_CL_6_FLLC                       0x000e0188 /* Set 1 front carrier loop linear coefficient and scaling values */
#define BCHP_SDS_CL_6_FLLC1                      0x000e018c /* Set 2 front carrier loop linear coefficient and scaling values */
#define BCHP_SDS_CL_6_FLIC                       0x000e0190 /* Set 1 front carrier loop integrator coefficient and scaling values */
#define BCHP_SDS_CL_6_FLIC1                      0x000e0194 /* Set 2 front carrier loop integrator coefficient and scaling values */
#define BCHP_SDS_CL_6_FLSW                       0x000e0198 /* Front carrier loop sweep rate */
#define BCHP_SDS_CL_6_FLI                        0x000e019c /* Front carrier loop integrator value */
#define BCHP_SDS_CL_6_FLIF                       0x000e01a0 /* Front carrier loop IF frequency control word */
#define BCHP_SDS_CL_6_FLPA                       0x000e01a4 /* Front carrier loop phase accumulator value */
#define BCHP_SDS_CL_6_FLTD                       0x000e01a8 /* Front carrier loop frequency detector threshold */
#define BCHP_SDS_CL_6_PEEST                      0x000e01ac /* Back carrier loop phase error estimator output */
#define BCHP_SDS_CL_6_PLTD                       0x000e01b0 /* Back carrier loop frequency detector threshold */
#define BCHP_SDS_CL_6_PLC                        0x000e01b4 /* 1st set of back carrier loop linear and integrator coefficients and scaling values */
#define BCHP_SDS_CL_6_PLC1                       0x000e01b8 /* 2nd set of back carrier loop linear and integrator coefficients and scaling values */
#define BCHP_SDS_CL_6_PLSW                       0x000e01bc /* Back carrier loop sweep rate */
#define BCHP_SDS_CL_6_PLI                        0x000e01c0 /* Back carrier loop integrator value */
#define BCHP_SDS_CL_6_PLPA                       0x000e01c4 /* Back carrier loop phase accumulator value */
#define BCHP_SDS_CL_6_CRBFD                      0x000e01c8 /* Carrier loop QAM/8PSK phase detector threshold */
#define BCHP_SDS_CL_6_CLHT                       0x000e01cc /* Carrier loop lock high threshold value */
#define BCHP_SDS_CL_6_CLLT                       0x000e01d0 /* Carrier loop lock low threshold value */
#define BCHP_SDS_CL_6_CLLA                       0x000e01d4 /* Carrier loop lock accumulator value */
#define BCHP_SDS_CL_6_CLCT                       0x000e01d8 /* Carrier loop lock control */
#define BCHP_SDS_CL_6_CLFFCTL                    0x000e01dc /* Fine Frequency Mixer Control */
#define BCHP_SDS_CL_6_FFLPA                      0x000e01e0 /* Fine frequency adjustment phase accumulator */
#define BCHP_SDS_CL_6_CLFBCTL                    0x000e01e4 /* Forward Backward Loop Control (Formerly, CLFBCTL2,CLFBCTL) */
#define BCHP_SDS_CL_6_FBLC                       0x000e01e8 /* Forward Backward Loop Filter Coefficient */
#define BCHP_SDS_CL_6_FBLI                       0x000e01ec /* Forward Backward Loop Filter Integrator */
#define BCHP_SDS_CL_6_FBPA                       0x000e01f0 /* Forward Backward Loop Phase Accumulator */
#define BCHP_SDS_CL_6_CLDAFECTL                  0x000e01f4 /* DAFE Loop Control */
#define BCHP_SDS_CL_6_DAFELI                     0x000e01f8 /* DAFE Loop Filter Integrator */
#define BCHP_SDS_CL_6_DAFEINT                    0x000e01fc /* DAFE Loop Filter Integrator */

#endif /* #ifndef BCHP_SDS_CL_6_H__ */

/* End of File */