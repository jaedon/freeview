/**************************************************************
*	http://www.humaxdigital.com
*	@author			humax
**************************************************************/


/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ¨Ï 2011-2012 Humax Co., Ltd.
* This program is produced by Humax Co., Ltd. ("Humax") and
* the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
* non-assignable, non-transferable and non-exclusive license to use this Software.
* You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
* you agree to the responsibility to take all reasonable efforts to protect the any information
* you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
* reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
* If you have no authorized license, discontinue using this Software immediately.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
* IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
* IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/



/* ======================================================================== */
/* Include. 																						*/
/* ======================================================================== */
#include <ondk.h>

#include <ondk_gwm_appobj.h>
#include <ondk_gwm_component.h>

#include "../ondk_gwm_objdef.h"
#include "../ondk_gwm_internal.h"

/* ======================================================================== */
/* Private Macros. 																					*/
/* ======================================================================== */


/* ======================================================================== */
/* Global/Extern Variables.																			*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Types.																					*/
/* ======================================================================== */


/* ======================================================================== */
/* Private Constants. 																				*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Variables. 																				*/
/* ======================================================================== */

STATIC ONDK_Result_t	ondk_guicom_HBox(ONDKSurfaceHandle pOsdScreen, ONDK_Rect_t pstObjRect, HCHAR *lImg,HCHAR *mImg, HCHAR *rImg)
{
	ONDK_Rect_t		midRect;	
	ONDK_Size_t		lImgSize = {0, 0}, mImgSize = {0, 0}, rImgSize = { 0, 0 };
	ONDKImageHandle	leftImg = NULL;
	ONDKImageHandle	midImg = NULL;
	ONDKImageHandle	rightImg = NULL;

	// Draw Left Image
	if(lImg != NULL)
	{
		leftImg = ONDK_IMG_CreateSelf(lImg);
		lImgSize = ONDK_IMG_GetSize(leftImg);

		ONDK_IMG_Blit(leftImg, pOsdScreen, pstObjRect.x, pstObjRect.y);
	}

	// Draw Right Image
	if(rImg != NULL)
	{
		rightImg = ONDK_IMG_CreateSelf(rImg);
		rImgSize = ONDK_IMG_GetSize(rightImg);
		ONDK_IMG_Blit(rightImg, pOsdScreen, pstObjRect.x + pstObjRect.w - rImgSize.w, pstObjRect.y);
	}

	// Draw Center Image
	if(mImg != NULL)
	{
		midImg = ONDK_IMG_CreateSelf(mImg);
		mImgSize = ONDK_IMG_GetSize(midImg);

		midRect.x = pstObjRect.x+lImgSize.w;
		midRect.y = pstObjRect.y;
		midRect.w = pstObjRect.w - lImgSize.w - rImgSize.w;
		midRect.h = pstObjRect.h;

		ONDK_IMG_StretchBlit(midImg, pOsdScreen, midRect);
	}

	return ONDK_RET_OK;
}


STATIC ONDK_Result_t	ondk_guicom_HBoxAlpha(ONDKSurfaceHandle pOsdScreen, ONDK_Rect_t pstObjRect, HCHAR *lImg,HCHAR *mImg, HCHAR *rImg, HINT32 Alpha)
{
	ONDK_Rect_t		drawRect;
	ONDK_Rect_t		midRect;	
	ONDK_Size_t		lImgSize = {0, 0}, mImgSize = {0, 0}, rImgSize = { 0, 0 };
	ONDKImageHandle	leftImg = NULL;
	ONDKImageHandle	midImg = NULL;
	ONDKImageHandle	rightImg = NULL;

	// Draw Left Image
	if(lImg != NULL)
	{
		leftImg = ONDK_IMG_CreateSelf(lImg);
		lImgSize = ONDK_IMG_GetSize(leftImg);

		drawRect = ONDK_Rect(pstObjRect.x, pstObjRect.y, lImgSize.w, pstObjRect.h);
		ONDK_IMG_StretchBlitAlpha(leftImg, pOsdScreen, drawRect, Alpha);
	}

	// Draw Right Image
	if(rImg != NULL)
	{
		rightImg = ONDK_IMG_CreateSelf(rImg);
		rImgSize = ONDK_IMG_GetSize(rightImg);
		drawRect = ONDK_Rect(pstObjRect.x + pstObjRect.w - rImgSize.w, pstObjRect.y, lImgSize.w, pstObjRect.h);
		ONDK_IMG_StretchBlitAlpha(rightImg, pOsdScreen, drawRect, Alpha);
	}

	// Draw Center Image
	if(mImg != NULL)
	{
		midImg = ONDK_IMG_CreateSelf(mImg);
		mImgSize = ONDK_IMG_GetSize(midImg);

		midRect.x = pstObjRect.x+lImgSize.w;
		midRect.y = pstObjRect.y;
		midRect.w = pstObjRect.w - lImgSize.w - rImgSize.w;
		midRect.h = pstObjRect.h;

		ONDK_IMG_StretchBlitAlpha(midImg, pOsdScreen, midRect, Alpha);
	}

	return ONDK_RET_OK;
}

STATIC ONDK_Result_t	ondk_guicom_VBox(ONDKSurfaceHandle pOsdScreen, ONDK_Rect_t pstObjRect, HCHAR *TImg,HCHAR *MImg, HCHAR *BImg)
{	
	ONDK_Rect_t		drawRect;
	ONDK_Rect_t		midRect;
	ONDK_Size_t		tImgSize = {0, 0}, mImgSize = {0, 0}, bImgSize = { 0, 0 };
	ONDKImageHandle	topImg = NULL;
	ONDKImageHandle	midImg = NULL;
	ONDKImageHandle	bottomImg = NULL;

	drawRect = ONDK_Rect(pstObjRect.x, pstObjRect.y, pstObjRect.w, pstObjRect.h);

	if(TImg != NULL)
	{
		topImg = ONDK_IMG_CreateSelf(TImg);
		tImgSize = ONDK_IMG_GetSize(topImg);

		ONDK_IMG_Blit(topImg, pOsdScreen, drawRect.x, drawRect.y);
	}

	if(BImg != NULL)
	{
		bottomImg = ONDK_IMG_CreateSelf(BImg);
		bImgSize = ONDK_IMG_GetSize(bottomImg);
		ONDK_IMG_Blit(bottomImg, pOsdScreen, drawRect.x, drawRect.y + drawRect.h - bImgSize.h);
	}

	if(MImg != NULL)
	{
		midImg = ONDK_IMG_CreateSelf(MImg);
		mImgSize = ONDK_IMG_GetSize(midImg);

		midRect.x = drawRect.x;
		midRect.y = drawRect.y + tImgSize.h;
		midRect.w = drawRect.w;
		midRect.h = drawRect.h - tImgSize.h - bImgSize.h;

		ONDK_IMG_StretchBlit(midImg, pOsdScreen, midRect);
	}

	return ONDK_RET_OK;
}

STATIC ONDK_Result_t	ondk_guicom_imgBlit(ONDKSurfaceHandle pOsdScreen, ONDK_Rect_t pstObjRect, HCHAR *Img)
{
	ONDKImageHandle	ImgHandle = NULL;

	if(Img == NULL)
	{
		return ONDK_RET_FAIL;
	}

	ImgHandle = ONDK_IMG_CreateSelf(Img);
	ONDK_IMG_Blit(ImgHandle, pOsdScreen, pstObjRect.x, pstObjRect.y);

	return ONDK_RET_OK;
}


/* ======================================================================== */
/* Private Function prototypes. 																		*/
/* ======================================================================== */
ONDK_Result_t	ONDK_GUICOM_HBox(ONDKSurfaceHandle pOsdScreen, ONDK_Rect_t pstObjRect, HCHAR *lImg,HCHAR *mImg, HCHAR *rImg)
{
	return ondk_guicom_HBox(pOsdScreen, pstObjRect, lImg, mImg, rImg);
}

ONDK_Result_t	ONDK_GUICOM_HBoxAlpha(ONDKSurfaceHandle pOsdScreen, ONDK_Rect_t pstObjRect, HCHAR *lImg,HCHAR *mImg, HCHAR *rImg, HINT32 alpha)
{
	return ondk_guicom_HBoxAlpha(pOsdScreen, pstObjRect, lImg, mImg, rImg, alpha);
}


ONDK_Result_t	ONDK_GUICOM_VBox(ONDKSurfaceHandle pOsdScreen, ONDK_Rect_t pstObjRect, HCHAR *TImg,HCHAR *mImg, HCHAR *BImg)
{
	return ondk_guicom_VBox(pOsdScreen, pstObjRect, TImg, mImg, BImg);
}

ONDK_Result_t	ONDK_GUICOM_IMG_Blit(ONDKSurfaceHandle pOsdScreen, ONDK_Rect_t pstObjRect, HCHAR *Img)
{
	return ondk_guicom_imgBlit(pOsdScreen, pstObjRect, Img);
}


/* End of File. ---------------------------------------------------------- */


