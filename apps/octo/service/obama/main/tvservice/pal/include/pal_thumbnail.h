/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  	pal_thumbnail.h
	@brief 		thumbnail file make/read

	Description: 		\n
	Module: pal/include			 	\n

	Copyright (c) 2011 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef __PAL_THUMBNAIL_H__
#define	__PAL_THUMBNAIL_H__


typedef enum
{
	ePAL_THUMB_IMAGETYPE_ARGB32,			// ARGB 순으로 구성된 Bitmap Image (32bit)
	ePAL_THUMB_IMAGETYPE_PNG,					// Png Image (32bit)
} PalThumb_ImageType_e;

HERROR PAL_THUMB_CaptureImageToBuffer (HUINT32 ulVideoDecId, HUINT32 ulWidth, HUINT32 ulHeight, PixelFormat_e ePixelFormat, HUINT8 **ppucFrameBuffer, HUINT32 *pulBufferSize);
HERROR PAL_THUMB_FreeBuffer (HUINT8 *pucFrameBuffer);
HERROR PAL_THUMB_CaptureImageToPngFile (HUINT32 ulVideoDecId, HUINT32 ulWidth, HUINT32 ulHeight, HCHAR *szFileName);

HERROR PAL_THUMB_ReadFile (HCHAR *szFileName, HUINT32 *pulWidth, HUINT32 *pulHeight, HUINT8 *pucBuffer);

#endif /* __PAL_THUMBNAIL_H__ */
