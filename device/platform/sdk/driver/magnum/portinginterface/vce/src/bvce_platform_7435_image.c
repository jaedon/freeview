/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvce_platform_7435_image.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 9/28/12 10:10a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vce/src/bvce_platform_7435_image.c $
 * 
 * Hydra_Software_Devel/3   9/28/12 10:10a nilesh
 * SW7435-383: Added 7435 B0 FW image for bring-up (unqualified)
 * 
 * Hydra_Software_Devel/2   1/26/12 10:27a nilesh
 * SW7435-2: Initial 7435 A0 FW (Untested)
 * 
 * Hydra_Software_Devel/1   11/3/11 5:08p nilesh
 * SW7435-2: Added 7435 support for VICE2_0 instance
 * 
 ***************************************************************************/

#include "bstd.h"
#include "bimg.h"
#include "bvce_platform.h"
#include "bvce_image.h"
#include "bvce_image_priv.h"

#if (BCHP_VER == BCHP_VER_A0)
#include "bvce_fw_image_rev_1_1_1_0.c"
#else
#warning "7435 B0 VCE FW (v1.1.1.1) is an unqualified engineering release"
#include "bvce_fw_image_rev_1_1_1_1.c"
#endif

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
