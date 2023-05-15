/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvce_platform_7425_image.c $
 * $brcm_Revision: Hydra_Software_Devel/20 $
 * $brcm_Date: 12/11/12 12:38p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vce/src/bvce_platform_7425_image.c $
 * 
 * Hydra_Software_Devel/20   12/11/12 12:38p nilesh
 * SW7425-4312: Removed 7425 B1 support
 * 
 * Hydra_Software_Devel/19   8/29/12 2:05p nilesh
 * SW7425-3853: Removed 7425 B0 support
 * 
 * Hydra_Software_Devel/18   2/9/12 11:35a nilesh
 * SW7425-2265: Re-Added 7425 B0 binary (unsupported)
 * 
 * Hydra_Software_Devel/17   2/8/12 12:39p nilesh
 * SW7425-2265: Removed 7425 B0 support
 * 
 * Hydra_Software_Devel/16   1/26/12 10:28a nilesh
 * SW7425-2265: Initial 7425 B2 FW. Untested.
 * 
 * Hydra_Software_Devel/15   12/5/11 2:12p nilesh
 * SW7425-960: Updated FW naming scheme
 * 
 * Hydra_Software_Devel/14   11/3/11 3:48p nilesh
 * SW7425-1663: Fixed compilation error
 * 
 * Hydra_Software_Devel/13   11/3/11 3:38p nilesh
 * SW7425-1663: Removed 7425 A1 support
 * 
 * Hydra_Software_Devel/12   11/3/11 3:34p nilesh
 * SW7425-1663: Added support for 7425 B1 fw image
 * 
 * Hydra_Software_Devel/11   10/3/11 12:15p nilesh
 * SW7425-891: Removed A0 support
 * 
 * Hydra_Software_Devel/10   9/8/11 1:58p nilesh
 * SW7425-891: Add B0 and Dual Encode Support
 * 
 * Hydra_Software_Devel/SW7425-891/1   9/7/11 12:28a nilesh
 * SW7425-891: B0 Test FW
 * 
 * Hydra_Software_Devel/9   6/6/11 5:33p nilesh
 * SW7425-682: v1.1.9.0 Phase 3.0 Engineering Drop Pre-Release
 * 
 * Hydra_Software_Devel/VICE2_FW_Dev/1   6/6/11 11:49a nilesh
 * SW7425-682: Enable A1 firmware
 * 
 * Hydra_Software_Devel/8   4/24/11 11:59p nilesh
 * SW7425-396: Updated to use A0 FW for both A0/A1
 * 
 * Hydra_Software_Devel/7   4/20/11 9:13p nilesh
 * SW7425-396: 7425 Phase 2.0 FW Release
 * 
 * Hydra_Software_Devel/7425_Phase2/1   4/19/11 10:35p nilesh
 * SW7425-1: Added A1 FW Image
 * 
 * Hydra_Software_Devel/6   3/31/11 1:21p nilesh
 * SW7425-1: Phase 2.0 Test Release
 * 
 * Hydra_Software_Devel/VCE_Phase_1_RC/1   3/29/11 4:55p nilesh
 * SW7425-1: Updated to use new firmware file naming convention
 * 
 * Hydra_Software_Devel/5   1/26/11 4:45p nilesh
 * SW7425-1: API Changes
 * 
 * Hydra_Software_Devel/4   11/17/10 12:41p nilesh
 * SW7425-1: Moved platform specific code
 * 
 * Hydra_Software_Devel/3   10/19/10 8:25a nilesh
 * SW7425-1: Changed firmware image name to match actual released name
 * from VCE FW team
 * 
 * Hydra_Software_Devel/2   10/12/10 1:40p nilesh
 * SW7425-1: Included actual firmware image
 * 
 * Hydra_Software_Devel/1   10/7/10 11:09a nilesh
 * SW7425-1: Implemented BVCE_Open/BVCE_Close
 *
 ***************************************************************************/

#include "bstd.h"
#include "bimg.h"
#include "bvce_platform.h"
#include "bvce_image.h"
#include "bvce_image_priv.h"

#if (BCHP_VER < BCHP_VER_B0)
#error "7425 Ax is no longer supported by VCE PI"
#elif (BCHP_VER == BCHP_VER_B0)
#error "7425 B0 is no longer supported by VCE PI"
#elif (BCHP_VER == BCHP_VER_B1)
#error "7425 B1 is no longer supported by VCE PI"
#else
#include "bvce_fw_image_rev_0_1_8_2.c"
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
