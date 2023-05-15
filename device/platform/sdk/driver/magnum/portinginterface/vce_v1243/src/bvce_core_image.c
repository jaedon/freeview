/***************************************************************************
 *     Copyright (c) 2003-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvce_core_image.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 4/8/13 2:56p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vce/src/bvce_core_image.c $
 * 
 * Hydra_Software_Devel/3   4/8/13 2:56p nilesh
 * SW7425-4676: Removed support for VCE Core v1.1.1.0 (7435 A0)
 * 
 * Hydra_Software_Devel/2   3/28/13 1:57p nilesh
 * SW7425-4676: VCE FW v4.0.9.0
 * 
 * Hydra_Software_Devel/1   3/6/13 5:54p nilesh
 * SW7425-4615: Merge refactor of platform vs core parameters
 * 
 * Hydra_Software_Devel/SW7425-4615/1   3/6/13 5:42p nilesh
 * SW7425-4615: Refactor platform vs core specific parameters
 *
 ***************************************************************************/

#include "bstd.h"
#include "bimg.h"
#include "bvce_platform.h"
#include "bvce_image.h"
#include "bvce_image_priv.h"

#if ( ( BVCE_P_CORE_MAJOR == 0 ) && ( BVCE_P_CORE_MINOR == 1 ) && ( BVCE_P_CORE_SUBMINOR == 8 ) && ( BVCE_P_CORE_REVISION == 2 ) )
#include "bvce_fw_image_rev_0_1_8_2.c"
#elif ( ( BVCE_P_CORE_MAJOR == 1 ) && ( BVCE_P_CORE_MINOR == 1 ) && ( BVCE_P_CORE_SUBMINOR == 1 ) && ( BVCE_P_CORE_REVISION == 1 ) )
#include "bvce_fw_image_rev_1_1_1_1.c"
#elif ( ( BVCE_P_CORE_MAJOR == 2 ) && ( BVCE_P_CORE_MINOR == 0 ) && ( BVCE_P_CORE_SUBMINOR == 0 ) && ( BVCE_P_CORE_REVISION == 2 ) )
#include "bvce_fw_image_rev_2_0_0_2.c"
#elif ( ( BVCE_P_CORE_MAJOR == 2 ) && ( BVCE_P_CORE_MINOR == 0 ) && ( BVCE_P_CORE_SUBMINOR == 1 ) && ( BVCE_P_CORE_REVISION == 2 ) )
#include "bvce_fw_image_rev_2_0_1_2.c"
#elif ( ( BVCE_P_CORE_MAJOR == 2 ) && ( BVCE_P_CORE_MINOR == 0 ) && ( BVCE_P_CORE_SUBMINOR == 15 ) && ( BVCE_P_CORE_REVISION == 2 ) )
#include "bvce_fw_image_rev_2_0_f_2.c"
#elif ( ( BVCE_P_CORE_MAJOR == 2 ) && ( BVCE_P_CORE_MINOR == 1 ) && ( BVCE_P_CORE_SUBMINOR == 0 ) && ( BVCE_P_CORE_REVISION == 3 ) )
#include "bvce_fw_image_rev_2_1_0_3.c"
#else
#error Unrecognized core version
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

