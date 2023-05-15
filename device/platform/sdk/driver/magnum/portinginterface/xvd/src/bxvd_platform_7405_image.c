/***************************************************************************
 *     Copyright (c) 2006-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxvd_platform_7405_image.c $
 * $brcm_Revision: Hydra_Software_Devel/10 $
 * $brcm_Date: 4/13/09 3:19p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xvd/7401/bxvd_platform_7405_image.c $
 * 
 * Hydra_Software_Devel/10   4/13/09 3:19p davidp
 * PR54113: Change AVD FW file name to bxvd_img_dec_elf_poseidon.c
 * 
 * Hydra_Software_Devel/9   3/4/09 4:06p nilesh
 * PR52516: Fixed KERNELMODE build warning
 * 
 * Hydra_Software_Devel/8   3/4/09 3:43p nilesh
 * PR52516: Fixed KERNELMODE build error
 * 
 * Hydra_Software_Devel/5   11/13/07 3:10p nilesh
 * PR36450: FW Image Load now uses inner offset as defined in firmware
 * file instead of a hard coded value
 * 
 * Hydra_Software_Devel/4   6/28/07 3:57p pblanco
 * PR28215: Bringup changes - sanity checkin.
 * 
 * Hydra_Software_Devel/3   6/21/07 2:40p pblanco
 * PR28215: Bringup-in-progress checkin.
 * 
 * Hydra_Software_Devel/2   6/14/07 9:59a pblanco
 * PR27645: Use new unified ITB format firmware.
 * 
 * Hydra_Software_Devel/1   6/12/07 11:37a nilesh
 * PR31900: Merged makefile cleanup to mainline to support bsettop
 * B_CONFIG_IMAGE build option
 * 
 * Hydra_Software_Devel/PR31900/1   6/7/07 11:47a nilesh
 * PR31900: Moved firmware image to separate file
 * 
 ***************************************************************************/

#include "bstd.h"
#include "bxvd_image.h"
#include "bxvd_image_priv.h"

#include "bxvd_img_dec_elf_poseidon.c" 

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
