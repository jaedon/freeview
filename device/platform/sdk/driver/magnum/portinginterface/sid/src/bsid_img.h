/******************************************************************************
*     Copyright (c) 2004-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bsid_img.h $
* $brcm_Revision: Hydra_Software_Devel/1 $
* $brcm_Date: 1/20/12 2:05p $
*
* Module Description:
*   See Module Overview below.
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/sid/7425/bsid_img.h $
* 
* Hydra_Software_Devel/1   1/20/12 2:05p fbasso
* SW7425-1619: release SID multi channel interface to
* Hydra_Software_Devel
* 
* Hydra_Software_Devel/3   6/21/11 11:13a parijat
* SW7425-674: Added $brcm_Log: /magnum/portinginterface/sid/7425/bsid_img.h $
* SW7425-674: Added 
* SW7425-674: Added Hydra_Software_Devel/1   1/20/12 2:05p fbasso
* SW7425-674: Added SW7425-1619: release SID multi channel interface to
* SW7425-674: Added Hydra_Software_Devel
*
* Hydra_Software_Devel/1   5/03/06 09:19a fbasso
* - Created this file
******************************************************************************/

#ifndef BSID_IMG_H__
#define BSID_IMG_H__

#include "bstd.h"
#include "bsid.h"

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************************
/////////////////////// Defines, Typedef, Structs ////////////////////////////////
*********************************************************************************/
#define BIMG_EOF                                               (uint32_t)(-1)

#if ((BCHP_CHIP==7440) && (BCHP_VER == BCHP_VER_A0))
#define SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_x_SHIFT  BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_60_SHIFT
#else
#define SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_x_SHIFT  BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_sgpio_08_SHIFT
#endif

/*********************************************************************************
////////////////////// Function prototypes declaration ///////////////////////////
*********************************************************************************/
BERR_Code BSID_P_BootArc(BSID_Handle hSid);

/*********************************************************************************
//////////////////////////////////////////////////////////////////////////////////
*********************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BSID_IMG_H__ */

/* end of file */
