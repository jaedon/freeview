/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvce_platform_7425.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 1/10/12 1:25p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vce/7425/bvce_platform_7425.h $
 * 
 * Hydra_Software_Devel/1   1/10/12 1:25p nilesh
 * SW7435-1: Added support for multiple instances
 *
 ***************************************************************************/

#ifndef BVCE_PLATFORM_7425_H_
#define BVCE_PLATFORM_7425_H_

#ifdef __cplusplus
extern "C" {
#endif

#if (BCHP_VER < BCHP_VER_B0)
#define BVCE_PLATFORM_P_NUM_ENCODE_CHANNELS 1
#endif

#ifdef __cplusplus
}
#endif

#endif /* BVCE_PLATFORM_7425_H_ */
/* End of File */