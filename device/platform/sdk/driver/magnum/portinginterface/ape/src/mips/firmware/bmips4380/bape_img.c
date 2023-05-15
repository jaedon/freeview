/***************************************************************************

*     Copyright (c) 2006-2010, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bape_img.c $
* $brcm_Revision: Hydra_Software_Devel/6 $
* $brcm_Date: 7/6/10 2:14p $
*
*
* Module Description:
*	This file is part of image interface implementation for APE PI.
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/ape/7408/image/bape_img.c $
* 
* Hydra_Software_Devel/6   7/6/10 2:14p jgarrett
* SW7408-102: Supporting removal of codecs based upon licensing
* 
* Hydra_Software_Devel/5   2/2/10 1:12p vsilyaev
* SW7408-63: Added PCM WAV module
* 
* Hydra_Software_Devel/4   12/10/09 7:57p vsilyaev
* SW7408-17: Changed names of FW images
* 
* Hydra_Software_Devel/3   12/8/09 5:26p jgarrett
* SW7408-17: Adding passthrough support
* 
 ***************************************************************************/

#include "bstd.h"
#include "bape_img.h"

BDBG_MODULE(ape_img);

/* External references of firmware binaries */
extern const void * const BAPE_IMG_bkernel[];
extern const void * const BAPE_IMG_aac_1_decoder[];
extern const void * const BAPE_IMG_ac3_decoder[];
extern const void * const BAPE_IMG_ddp_2_converter[];
extern const void * const BAPE_IMG_mpeg_1_decoder[];
extern const void * const BAPE_IMG_wma_1_decoder[];
extern const void * const BAPE_IMG_ac3_passthrough[];
extern const void * const BAPE_IMG_pcm_wav[];
 
static const void * const *BAPE_IMG_FirmwareImages [] = 
{
    BAPE_IMG_bkernel,
#if BAPE_AAC_SUPPORT
    BAPE_IMG_aac_1_decoder,
#else
    NULL,
#endif
#if BAPE_DDP_SUPPORT
    NULL,
    BAPE_IMG_ddp_2_converter,
#elif BAPE_AC3_SUPPORT
    BAPE_IMG_ac3_decoder,
    NULL,
#else
    NULL,
    NULL,
#endif
#if BAPE_MPEG_SUPPORT
    BAPE_IMG_mpeg_1_decoder,
#else
    NULL,
#endif
#if BAPE_WMA_SUPPORT
    BAPE_IMG_wma_1_decoder,
#else
    NULL,
#endif
#if BAPE_AC3_PASSTHROUGH_SUPPORT
    BAPE_IMG_ac3_passthrough,
#else
    NULL,
#endif
#if BAPE_PCMWAV_SUPPORT
    BAPE_IMG_pcm_wav
#else
    NULL
#endif
};

/*	This context is used by other modules to make use of this interface. They need to supply this as a parameter to BAPE_IMG_Open */
void *BAPE_IMG_Context = (void *)BAPE_IMG_FirmwareImages;

static BERR_Code BAPE_IMG_Open(void *context, void **image, unsigned image_id)
{
/*
	This function has to be used for opening a firmware image. The pointer to the firmware image array is given
	that contains the header and the chunks. This works based on the firmware image id  that is supplied.
*/
	if (image_id >=  sizeof(BAPE_IMG_FirmwareImages)/sizeof(*BAPE_IMG_FirmwareImages)) 
	{
		/* If the image ID is greater than available IDs return an error */
		BDBG_ERR(("Invalid image_id %u",image_id));
		return BERR_TRACE(BERR_INVALID_PARAMETER);	
	}	

	*image = ((void **)context)[image_id];	

	if (NULL == *image)
	{
	    BDBG_ERR (("not available image_id %u", image_id));
		return BERR_TRACE(BERR_INVALID_PARAMETER);	
	}

	return BERR_SUCCESS;
}

static BERR_Code BAPE_IMG_Next(void *image, unsigned chunk, const void **data, uint16_t length)
{
/*
	After opening the firmware image, the user of this interface will then be interested in getting the chunks
	and the header giving information about the chunks.
*/

	BDBG_ASSERT(data);	
	BSTD_UNUSED(length);

	if (chunk > ((uint32_t*)((const void **)image)[0])[1])
	{
		/* if the chunk number is greater than the available chunks in that image return error */
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	*data = ((const void **)image)[chunk];	
	
	if (NULL == *data) 
	{		
		return BERR_TRACE(BERR_INVALID_PARAMETER);	
	}	

	return BERR_SUCCESS;
}

static void BAPE_IMG_Close(void *image)
{	
/* This function is used to close the firmware image that was opened using BAPE_IMG_Open */
	BSTD_UNUSED(image);	
	return;
}

/* The interface is actually a set of three functions open, next and close. The same has been implemented here and their function pointers supplied */
const BIMG_Interface BAPE_IMG_Interface = {BAPE_IMG_Open, BAPE_IMG_Next, BAPE_IMG_Close};

