/***************************************************************************
 *     Copyright (c) 2006-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxvd_platform_7401_image.c $
 * $brcm_Revision: Hydra_Software_Devel/11 $
 * $brcm_Date: 4/13/09 3:18p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xvd/7401/bxvd_platform_7401_image.c $
 * 
 * Hydra_Software_Devel/11   4/13/09 3:18p davidp
 * PR54113: Change AVD FW file name to bxvd_img_dec_elf_apollo.c
 * 
 * Hydra_Software_Devel/10   3/4/09 4:06p nilesh
 * PR52516: Fixed KERNELMODE build warning
 * 
 * Hydra_Software_Devel/9   3/4/09 3:43p nilesh
 * PR52516: Fixed KERNELMODE build error
 * 
 * Hydra_Software_Devel/5   1/2/08 4:20p rayshap
 * PR38488: Add unified ITB support as default
 * 
 * Hydra_Software_Devel/4   11/13/07 3:10p nilesh
 * PR36450: FW Image Load now uses inner offset as defined in firmware
 * file instead of a hard coded value
 * 
 * Hydra_Software_Devel/3   6/1/07 3:24p nilesh
 * PR23100: Added explicit header include so file can be compiled
 * independently of XVD
 * 
 * Hydra_Software_Devel/2   6/1/07 3:08p nilesh
 * PR23100: Fixed compilation on non-7401 platforms
 * 
 * Hydra_Software_Devel/1   6/1/07 12:03p pblanco
 * PR23100: Initial checkin. Proxy for Adrian Hulse's kernel mode change.
 * 
 ***************************************************************************/

#include "bstd.h"
#include "bxvd_image.h"
#include "bxvd_image_priv.h"

#include "bxvd_img_dec_elf_apollo.c"

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
