/***************************************************************************
 *     Copyright (c) 2006-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxvd_image_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 3/4/09 3:43p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xvd/7401/bxvd_image_priv.h $
 * 
 * Hydra_Software_Devel/4   3/4/09 3:43p nilesh
 * PR52516: Fixed KERNELMODE build error
 * 
 * Hydra_Software_Devel/2   11/13/07 3:09p nilesh
 * PR36450: FW Image Load now uses inner offset as defined in firmware
 * file instead of a hard coded value
 * 
 * Hydra_Software_Devel/1   11/13/06 2:02p nilesh
 * PR25365: Merged BIMG support to mainline
 * 
 * Hydra_Software_Devel/PR25365/1   11/10/06 11:51a nilesh
 * PR25365: Added BIMG support
 *
 ***************************************************************************/

#ifndef BXVD_IMAGE_PRIV_H__
#define BXVD_IMAGE_PRIV_H__

typedef struct BXVD_IMAGE_ContextEntry
{
	const unsigned int *puiImageSize;
	const void *pImageData;
        const unsigned int *puiImageOffset;
} BXVD_IMAGE_ContextEntry;

typedef struct BXVD_IMAGE_Container
{
	BXVD_AvdImgHdr imageHeader;
	const void *pImageData;
} BXVD_IMAGE_Container;

#endif /* BXVD_IMAGE_PRIV_H__ */
/* End of file. */

