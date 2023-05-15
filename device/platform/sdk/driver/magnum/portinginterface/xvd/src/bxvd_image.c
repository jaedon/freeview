/***************************************************************************
 *    Copyright (c) 2004-2009, Broadcom Corporation
 *    All Rights Reserved
 *    Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxvd_image.c $
 * $brcm_Revision: Hydra_Software_Devel/9 $
 * $brcm_Date: 12/8/09 1:53p $
 *
 * Module Description:
 *	 This module contains the code for the XVD FW image
 *   interface.
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xvd/7401/bxvd_image.c $
 * 
 * Hydra_Software_Devel/9   12/8/09 1:53p davidp
 * SW7405-3531: Decoder EOC value incorrrectly calculated during
 * authenticated FW load.
 * 
 * Hydra_Software_Devel/8   11/13/07 3:09p nilesh
 * PR36450: FW Image Load now uses inner offset as defined in firmware
 * file instead of a hard coded value
 * 
 * Hydra_Software_Devel/7   9/6/07 5:35p nilesh
 * PR29915: Added BERR_TRACE wrapper around all return codes
 * 
 * Hydra_Software_Devel/6   8/20/07 11:33a nilesh
 * PR34118: Removed compilation warning
 * 
 * Hydra_Software_Devel/5   8/16/07 4:04p nilesh
 * PR34118: Added support for loading an authenticated FW image using
 * XVD's built in BIMG interface
 * 
 * Hydra_Software_Devel/4   12/4/06 12:04p nilesh
 * PR26289: Changed error reporting during firmware load
 * 
 * Hydra_Software_Devel/3   11/17/06 4:52p nilesh
 * PR25868: Merged authenticated image support to mainline
 * 
 * Hydra_Software_Devel/PR25868/2   11/16/06 5:01p nilesh
 * PR25868: Removed false authenticated firmware error message
 * 
 * Hydra_Software_Devel/PR25868/1   11/16/06 3:36p nilesh
 * PR25868: Added error checking, cleaned up incorrect comments
 * 
 * Hydra_Software_Devel/2   11/13/06 2:02p nilesh
 * PR25365: Merged BIMG support to mainline
 * 
 * Hydra_Software_Devel/PR25365/1   11/10/06 11:06a nilesh
 * PR25365: Added BIMG support
 * 
 * Hydra_Software_Devel/1   10/30/06 2:43p pblanco
 * PR25365: Initial check in
 * 
 ****************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "bxvd_image.h"
#include "bxvd_image_priv.h"

BDBG_MODULE(BXVD_IMAGE);

/* This file contains the reference implementation for XVD's BIMG
 * firmware retrieval interface.  It provides access to the AVD Core's
 * inner and outer ELF firmware images using the standard BIMG
 * interface.  This interface can be overridden by the application by
 * setting the pImgInterface and pImgContext pointers accordingly.  If
 * a custom interface/context is used, to save code space, you can
 * also define BXVD_USE_CUSTOM_IMAGE=1 during compilation to prevent
 * the default firmware images from being linked in.  
 *
 * If a custom interface is provided, the custom BIMG_Next() call MUST
 * adhere to the following rules:
 *
 * - Chunk #0 ALWAYS returns a header of type BXVD_AvdImgHdr.  The
 *   length should be sizeof(BXVD_AvdImgHdr)
 *
 * - Chunk #1-n is the actual firmware image. The provided length can
 *   be any size, but MUST be the same for all chunks.  The caller
 *   will know how many bytes of the image is valid
 *
 * See
 * http://twiki-01.broadcom.com/bin/view/Bseavsw/XVDNdsSvpDesign#Updating_XVD_to_use_the_BIMG_Int
 * for details */

#if !(BXVD_USE_CUSTOM_IMAGE)
static BERR_Code BXVD_IMAGE_Open(void *context,
				 void **image,
				 unsigned image_id)
{
	BXVD_IMAGE_ContextEntry *pContextEntry = NULL;
	BXVD_IMAGE_Container *pImageContainer = NULL;

	BDBG_ENTER(BXVD_IMAGE_Open);

	BDBG_ASSERT(context);
	BDBG_ASSERT(image);
	
	/* Validate the firmware ID range */
	BDBG_MSG(("Validating image ID range"));
	if (image_id >= BXVD_IMAGE_FirmwareID_Max)
	{
		BDBG_ERR(("Invalid image id %d", image_id));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Validate the image referenced by firmware ID exists in the context */
	pContextEntry = ((BXVD_IMAGE_ContextEntry**)context)[image_id];
	
	if (pContextEntry == NULL) {
		/* A NULL entry does not necessarily mean a hard error
		 * since we could be trying to load authenticated
		 * firmware.  So, we fail silently here and expect
		 * this error to be reported by the caller depending
		 * on the situation */
		return BERR_INVALID_PARAMETER;		
	}
	
	/* Allocate an image container struct */
	BDBG_MSG(("Allocating image container"));
	pImageContainer = (BXVD_IMAGE_Container*) BKNI_Malloc(sizeof(BXVD_IMAGE_Container));
	if (pImageContainer == NULL) {
		BDBG_ERR(("Cannot allocate image container"));
		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
	}
	BKNI_Memset(pImageContainer, 0, sizeof(BXVD_IMAGE_Container));
	
	/* Fill in the image container struct */
        if ((image_id == BXVD_IMAGE_FirmwareID_eAuthenticated_AVD0) ||
            (image_id == BXVD_IMAGE_FirmwareID_eAuthenticated_AVD1))
        {
           /* The image data is the binary output of the svp_relocate
            * tool, where the header is followed by relocated
            * firmware.  We copy the header into our container and
            * then set the pImageData to start immediately after the
            * header */
           BKNI_Memcpy(&pImageContainer->imageHeader, 
                       pContextEntry->pImageData, 
                       sizeof(BXVD_AvdImgHdr));
           pImageContainer->pImageData = (void*) ((uint32_t) pContextEntry->pImageData + sizeof(BXVD_AvdImgHdr));
        }
        else
        {
           pImageContainer->imageHeader.uiImageSize = *(pContextEntry->puiImageSize);
           pImageContainer->imageHeader.uiRelocationBase = *(pContextEntry->puiImageOffset);
           pImageContainer->pImageData = pContextEntry->pImageData;   
        }

	*image = pImageContainer;

	BDBG_LEAVE(BXVD_IMAGE_Open);
	return BERR_TRACE(BERR_SUCCESS);
}

static BERR_Code BXVD_IMAGE_Next(void *image,
				 unsigned chunk,
				 const void **data,
				 uint16_t length)
{
	BXVD_IMAGE_Container *pImageContainer = (BXVD_IMAGE_Container*) image;

	BDBG_ENTER(BXVD_IMAGE_Next);
	BDBG_ASSERT(image);
	BDBG_ASSERT(data);
	BSTD_UNUSED(length);

	if (chunk == 0) {
		BDBG_MSG(("Returning image chunk[%d]: %d bytes of image header", chunk, length));

		/* Validate length requested is same as our header size */
		if (length != sizeof(BXVD_AvdImgHdr)) {
			BDBG_ERR(("Incorrect image header length requested"));
			return BERR_TRACE(BERR_INVALID_PARAMETER);			
		}
		
		/* Return a pointer to the header */
		*data = &pImageContainer->imageHeader;
	} else {	
		BDBG_MSG(("Returning image chunk[%d]: %d bytes of image data", chunk, length));
		*data = (void *)((uint8_t *)(pImageContainer->pImageData) + (chunk-1)*length);
	}

	BDBG_LEAVE(BXVD_IMAGE_Next);

	return BERR_TRACE(BERR_SUCCESS); 
}

static void BXVD_IMAGE_Close(void *image)
{
	/* Free the image container struct */
	BDBG_ENTER(BXVD_IMAGE_Close);
	if (image != NULL) {
		BKNI_Free(image);
	}
	BDBG_LEAVE(BXVD_IMAGE_Close);
	return;
}

const BIMG_Interface BXVD_IMAGE_Interface = 
{
	BXVD_IMAGE_Open,
	BXVD_IMAGE_Next,
	BXVD_IMAGE_Close
};
#endif																						 
