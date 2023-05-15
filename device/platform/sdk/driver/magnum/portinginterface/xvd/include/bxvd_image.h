/***************************************************************************
 *    Copyright (c) 2004-2010, Broadcom Corporation
 *    All Rights Reserved
 *    Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxvd_image.h $
 * $brcm_Revision: Hydra_Software_Devel/9 $
 * $brcm_Date: 10/15/10 6:40p $
 *
 * Module Description:
 *	 This module contains the definitions and prototypes for the XVD FW image
 *   interface.
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xvd/7401/bxvd_image.h $
 * 
 * Hydra_Software_Devel/9   10/15/10 6:40p davidp
 * SW7425-16: Add support for base layer ARC.
 * 
 * Hydra_Software_Devel/8   3/4/09 4:06p nilesh
 * PR52516: Fixed KERNELMODE build warning
 * 
 * Hydra_Software_Devel/7   3/4/09 3:43p nilesh
 * PR52516: Fixed KERNELMODE build error
 * 
 * Hydra_Software_Devel/4   11/17/06 4:52p nilesh
 * PR25868: Merged authenticated image support to mainline
 * 
 * Hydra_Software_Devel/PR25868/2   11/15/06 11:58a nilesh
 * PR25868: Added authenticated image support
 * 
 * Hydra_Software_Devel/PR25868/1   11/14/06 3:55p nilesh
 * PR25868: Added pre-relocated image support
 * 
 * Hydra_Software_Devel/2   11/13/06 2:03p nilesh
 * PR25365: Merged BIMG support to mainline
 * 
 * Hydra_Software_Devel/PR25365/1   11/10/06 11:06a nilesh
 * PR25365: Added BIMG support
 * 
 * Hydra_Software_Devel/1   10/30/06 2:43p pblanco
 * PR25365: Initial check in
 * 
 ****************************************************************************/
#ifndef BXVD_IMAGE_H__
#define BXVD_IMAGE_H__

#include "bimg.h"
#include "bxvd_image_header.h"

#ifdef __cplusplus
extern "C" {
#endif

/* FW image IDs used in the BXVD_IMAGE_Open function. The AVD0 IDs are used
 * for all architectures. Designs with more than 1 decoder can add image IDs
 *  here.
 */
typedef enum BXVD_IMAGE_FirmwareID
{       
        BXVD_IMAGE_FirmwareID_eOuterELF_AVD0 = 0,  /* AVD0 Outer ELF firmware image*/        
        BXVD_IMAGE_FirmwareID_eInnerELF_AVD0,      /* AVD0 Inner ELF firmware image */
        BXVD_IMAGE_FirmwareID_eOuterELF_AVD1,      /* AVD1 Outer ELF firmware image*/        
        BXVD_IMAGE_FirmwareID_eInnerELF_AVD1,      /* AVD1 Inner ELF firmware image */        
	
	BXVD_IMAGE_FirmwareID_eAuthenticated_AVD0, /* AVD0 Authenticated firmware image */
	BXVD_IMAGE_FirmwareID_eAuthenticated_AVD1, /* AVD1 Authenticated firmware image */

        /* Add additional image IDs ABOVE this line */
        BXVD_IMAGE_FirmwareID_Max
} BXVD_IMAGE_FirmwareID;

/* AVD/AVD core Rev K FW image Ids */
typedef enum BXVD_IMAGE_RevK_FirmwareID
{       
        BXVD_IMAGE_RevK_FirmwareID_eOuterELF_AVD = 0,  /* AVD Outer ELF firmware image*/        
        BXVD_IMAGE_RevK_FirmwareID_eInnerELF_AVD,      /* AVD Inner ELF firmware image */
        BXVD_IMAGE_RevK_FirmwareID_eBaseELF_SVD,      /* SVD BASE ELF firmware image */
        BXVD_IMAGE_RevK_FirmwareID_Max
} BXVD_IMAGE_RevK_FirmwareID;

extern void* const BXVD_IMAGE_Context;
extern const BIMG_Interface BXVD_IMAGE_Interface;

#ifdef __cplusplus
}
#endif

#endif /* BXVD_IMAGE_H__ */
/* End of file. */
 


