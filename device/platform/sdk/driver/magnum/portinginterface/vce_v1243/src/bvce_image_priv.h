/***************************************************************************
 *     Copyright (c) 2003-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvce_image_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 10/7/10 11:09a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vce/7425/bvce_image_priv.h $
 * 
 * Hydra_Software_Devel/1   10/7/10 11:09a nilesh
 * SW7425-1: Implemented BVCE_Open/BVCE_Close
 *
 ***************************************************************************/

#ifndef BVCE_IMAGE_PRIV_H_
#define BVCE_IMAGE_PRIV_H_

#ifdef __cplusplus
extern "C" {
#if 0
}
#endif
#endif

typedef struct BVCE_IMAGE_ContextEntry
{
        const unsigned int *puiImageSize;
        const void *pImageData;
} BVCE_IMAGE_ContextEntry;

typedef struct BVCE_IMAGE_Container
{
        size_t uiImageSize;
        const void *pImageData;
} BVCE_IMAGE_Container;

#ifdef __cplusplus
}
#endif

#endif /* BVCE_IMAGE_PRIV_H_ */
