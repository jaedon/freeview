/***************************************************************************
*     Copyright (c) 2006-2009, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bape_img.h $
* $brcm_Revision: Hydra_Software_Devel/3 $
* $brcm_Date: 12/8/09 5:26p $
*
* Module Description:
*	This file is part of image interface implementation for APE PI.
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/ape/7408/image/bape_img.h $
* 
* Hydra_Software_Devel/3   12/8/09 5:26p jgarrett
* SW7408-17: Adding passthrough support
* 
***************************************************************************/
#ifndef BAPE_IMG_H__
#define BAPE_IMG_H__

#include "bimg.h"

/* 
	Firmware Image IDs that are to be used in the BAPE_IMG_Open function of the BAPE_IMG_Interface
*/

extern void *BAPE_IMG_Context;
extern const BIMG_Interface BAPE_IMG_Interface;

#endif /* BAPE_IMG_H__ */


