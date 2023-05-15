/***************************************************************************
*     Copyright (c) 2006-2010, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bape_img_ac3_decoder.c $
* $brcm_Revision: Hydra_Software_Devel/1 $
* $brcm_Date: 10/28/10 2:56p $
*
*
* Module Description:
*	This file is part of image interface implementation for APE PI.
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/ape/7408/image/bape_img_ac3_decoder.c $
* 
* Hydra_Software_Devel/1   10/28/10 2:56p vsilyaev
* SW7408-136: Added Big Endian firmware
* 
***************************************************************************/
#include "bstd.h"
#include "bape_img.h"

#if BSTD_CPU_ENDIAN==BSTD_ENDIAN_BIG
#include "bape_img_ac3_decoder_be.h"
#elif BSTD_CPU_ENDIAN==BSTD_ENDIAN_LITTLE
#include "bape_img_ac3_decoder_le.h"
#else
#error "Not supported"
#endif

