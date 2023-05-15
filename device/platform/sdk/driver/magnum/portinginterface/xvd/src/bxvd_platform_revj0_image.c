/***************************************************************************
 *     Copyright (c) 2006-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxvd_platform_revj0_image.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 9/18/09 1:45p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 ***************************************************************************/

#include "bstd.h"
#include "bxvd_image.h"
#include "bxvd_image_priv.h"

#if (BCHP_CHIP == 7630)
#include "bxvd_img_dec_elf_chronos.c" 
#else
#include "bxvd_img_dec_elf_poseidon.c" 
#endif

const unsigned int BXVD_IMG_ELF_DecOuter_offset = 0;

static const BXVD_IMAGE_ContextEntry outerELF =
{
	&BXVD_IMG_ELF_DecOuter_size,
	BXVD_IMG_ELF_DecOuter,
        &BXVD_IMG_ELF_DecOuter_offset
};

static const BXVD_IMAGE_ContextEntry innerELF =
{
	&BXVD_IMG_ELF_DecInner_size,
	BXVD_IMG_ELF_DecInner,
        &BXVD_IMG_ELF_DecInner_offset
};

static const BXVD_IMAGE_ContextEntry* const BXVD_IMAGE_FirmwareImages[BXVD_IMAGE_FirmwareID_Max] = 
{	
	&outerELF, /* BXVD_IMG_FirmwareID_eOuterELF_AVD0 */
	&innerELF, /* BXVD_IMG_FirmwareID_eInnerELF_AVD0 */
};

void* const BXVD_IMAGE_Context = (void*) BXVD_IMAGE_FirmwareImages;
