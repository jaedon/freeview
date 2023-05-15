/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvce_platform_7445_image.c $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 10/1/12 11:27a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vce/src/bvce_platform_7445_image.c $
 * 
 * Hydra_Software_Devel/4   10/1/12 11:27a nilesh
 * SW7445-39: Added 7445 A0 FW Image
 * 
 * Hydra_Software_Devel/3   9/28/12 10:10a nilesh
 * SW7445-39: Added VCE FW image warning
 * 
 * Hydra_Software_Devel/2   9/28/12 9:50a nilesh
 * SW7445-39: Added 7445 A0 FW
 * 
 * Hydra_Software_Devel/1   9/18/12 10:24a nilesh
 * SW7445-39: Initial 7445 support
 * 
 ***************************************************************************/

#include "bstd.h"
#include "bimg.h"
#include "bvce_platform.h"
#include "bvce_image.h"
#include "bvce_image_priv.h"

#include "bvce_fw_image_rev_2_0_0_2.c"

static const BVCE_IMAGE_ContextEntry PicArc =
{
        &BVCE_IMG_ELF_PicArc_size,
        BVCE_IMG_ELF_PicArc,
};

static const BVCE_IMAGE_ContextEntry MBArc =
{
        &BVCE_IMG_ELF_MBArc_size,
        BVCE_IMG_ELF_MBArc,
};

static const BVCE_IMAGE_ContextEntry* const BVCE_IMAGE_FirmwareImages[BVCE_IMAGE_FirmwareID_Max] =
{
        &PicArc, /* BVCE_IMAGE_FirmwareID_ePicArc */
        &MBArc, /* BVCE_IMAGE_FirmwareID_eMBArc */
};

void* const BVCE_IMAGE_Context = (void*) BVCE_IMAGE_FirmwareImages;
