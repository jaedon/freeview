/***************************************************************************
 *     Copyright (c) 2006-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxvd_platform_7601_image.c $
 * $brcm_Revision: Hydra_Software_Devel/8 $
 * $brcm_Date: 4/21/09 4:01p $
 *
 * This file defines the video decoder FW image file to use. The data 
 * structures used by the FW load process are also defined within. 
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xvd/7401/bxvd_platform_7601_image.c $
 * 
 * Hydra_Software_Devel/8   4/21/09 4:01p davidp
 * PR53571: Use unified ITB supported AVD FW on 7635.
 * 
 * Hydra_Software_Devel/7   3/4/09 4:07p nilesh
 * PR52516: Fixed KERNELMODE build warning
 * 
 * Hydra_Software_Devel/6   3/4/09 3:43p nilesh
 * PR52516: Fixed KERNELMODE build error
 * 
 * Hydra_Software_Devel/2   1/26/09 5:12p davidp
 * PR51037: Change FW image name to bxvd_img_dec_elf_hermes.c
 * 
 * Hydra_Software_Devel/1   8/6/08 12:37p davidp
 * PR43585: Add 7601 platform specific support to XVD.
 * 
 ***************************************************************************/
#include "bstd.h"
#include "bxvd_image.h"
#include "bxvd_image_priv.h"

#if (BCHP_CHIP == 7635) 
/* Unified ITB FW version */
#include "bxvd_img_dec_elf_chronos.c" 
#else
/* Non unified ITB FW version */
#include "bxvd_img_dec_elf_hermes.c" 
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
