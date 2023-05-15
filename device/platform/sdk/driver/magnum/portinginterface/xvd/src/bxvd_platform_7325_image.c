/***************************************************************************
 *     Copyright (c) 2006-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxvd_platform_7325_image.c $
 * $brcm_Revision: Hydra_Software_Devel/8 $
 * $brcm_Date: 3/4/09 4:06p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xvd/7401/bxvd_platform_7325_image.c $
 * 
 * Hydra_Software_Devel/8   3/4/09 4:06p nilesh
 * PR52516: Fixed KERNELMODE build warning
 * 
 * Hydra_Software_Devel/7   3/4/09 3:43p nilesh
 * PR52516: Fixed KERNELMODE build error
 * 
 * Hydra_Software_Devel/3   11/13/07 3:09p nilesh
 * PR36450: FW Image Load now uses inner offset as defined in firmware
 * file instead of a hard coded value
 * 
 * Hydra_Software_Devel/2   11/6/07 3:41p pblanco
 * PR23100: Include 7405 FW instead of 7401
 * 
 * Hydra_Software_Devel/1   11/1/07 9:51a pblanco
 * PR23100: Add to source control.
 * 
 ***************************************************************************/

#include "bstd.h"
#include "bxvd_image.h"
#include "bxvd_image_priv.h"

#include "bxvd_img_dec_elf_7405a0.c"

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
