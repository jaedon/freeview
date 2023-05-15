/**
	@file     pal_thumbnail.c
	@brief    RCU, Front DI를 제어하고 MW/AP에 API를 제공한다.

	Description: 		\n
	Module: PAL/RCU			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <png.h>

#include <hlib.h>
#include <octo_common.h>

#include <di_err.h>
#include <di_osd.h>
#include <di_video.h>

#include <pal_thumbnail.h>


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/



#define palThumb_VERSION			0x01

#define palThumb_INDICATOR_SIZE	3
#define palThumb_HEADER_SIZE		12

// Thumbnail Type
#define palThumb_TYPE_ARGB32		0x01


#define Read8Bit(q)				(unsigned char)(*((unsigned char *)(q)) & 0xff) // (unsigned char)((q)[0])
#define Read10Bit(q)			(unsigned short)((((*((unsigned char *)(q))) << 8) | (*((unsigned char *)(q)+1))) & 0x03ff)/* 0000 0011 1111 1111 */
#define Read12Bit(q)			(unsigned short)((((*((unsigned char *)(q))) << 8) | (*((unsigned char *)(q)+1))) & 0x0fff)/* 0000 1111 1111 1111 */
#define Read13Bit(q)			(unsigned short)((((*((unsigned char *)(q))) << 8) | (*((unsigned char *)(q)+1))) & 0x1fff)/* 0001 1111 1111 1111 */
#define Read14Bit(q)			(unsigned short)((((*((unsigned char *)(q))) << 8) | (*((unsigned char *)(q)+1))) & 0x3fff)/* 0011 1111 1111 1111 */
#define Read16Bit(q)			(unsigned short)((((*((unsigned char *)(q))) << 8) | (*((unsigned char *)(q)+1))        ))

#define Read24Bit(q)			(unsigned int)(((*(unsigned char *)(q)) << 16) | (*((unsigned char *)(q)+1) << 8) | (*((unsigned char *)(q)+2)) )
#define Read32Bit(q)			(unsigned int)(((*(unsigned char *)(q)) << 24) | (*((unsigned char *)(q)+1) << 16) | (*((unsigned char *)(q)+2) << 8) | (*((unsigned char *)(q)+3)) )

#define Write8Bit(p, data)		{*((unsigned char *)(p)) = (unsigned char)(data); }
#define Write16Bit(p, data)		{*((unsigned char *)(p)) = (unsigned char)((data) >> 8);*((unsigned char *)((p) + 1)) = (unsigned char)(data);}
#define Write32Bit(p, data)		{*((unsigned char *)(p)) = (unsigned char)((data) >> 24);*((unsigned char *)((p) + 1)) = (unsigned char)((data) >> 16);*((unsigned char *)((p) + 2)) = (unsigned char)((data) >> 8);*((unsigned char *)((p) + 3)) = (unsigned char)(data);}



/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

STATIC HCHAR	 s_szPalThumbIndHeader[] = { 'T', 'H', 'M' };


/*******************************************************************/
/******************* Static Function Prototypes  *************************/
/*******************************************************************/


/*******************************************************************/
/******************* Extern Function 	   *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

#define __LOCAL_FUNCTIONS__

STATIC HERROR pal_thumb_readArgb32 (HxFILE_t hFileDes, HUINT32 ulWidth, HUINT32 ulHeight, HUINT32 ulBpp, HUINT8 *pucHeader, HUINT8 *pucBuffer)
{
	HUINT8			 aucTempBuffer[4];
	HUINT32			 ulBinSize;
	PAL_FS_UINT32	 ulSizeP;

	HxSTD_memset (aucTempBuffer, 0, 4);

	ulSizeP = HLIB_FILE_Read(hFileDes, (void *)aucTempBuffer, 1 , 4);
	if (ulSizeP <= 0)
	{
		return ERR_FAIL;
	}

	ulBinSize = Read32Bit (aucTempBuffer);
	if (0 == ulBinSize)
	{
		return ERR_FAIL;
	}

	ulSizeP = HLIB_FILE_Read(hFileDes, (void *)pucBuffer, 1 , ulBinSize);
	if (ulSizeP <= 0)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

#define _____CAPTURE_FUNCTIONS_____
STATIC DI_OSD_PIXEL_FORMAT pal_thumb_convPixelFormat2Di (PixelFormat_e ePixelFormat, HUINT32 *pulBpp)
{
	switch (ePixelFormat)
	{
	case ePixelFormat_ARGB_8888:			*pulBpp = 4;	return DI_OSD_PIXEL_FORMAT_ARGB_8888;
	case ePixelFormat_RGBA_8888:			*pulBpp = 4;	return DI_OSD_PIXEL_FORMAT_RGBA_8888;
	case ePixelFormat_ARGB_4444:			*pulBpp = 2;	return DI_OSD_PIXEL_FORMAT_ARGB_4444;
	case ePixelFormat_RGBA_4444:			*pulBpp = 2;	return DI_OSD_PIXEL_FORMAT_RGBA_4444;
	case ePixelFormat_ARGB_1555:			*pulBpp = 2;	return DI_OSD_PIXEL_FORMAT_ARGB_1555;
	case ePixelFormat_RGB_565:				*pulBpp = 2;	return DI_OSD_PIXEL_FORMAT_ARGB_4444;
	case ePixelFormat_CLUT_8:				*pulBpp = 1;	return DI_OSD_PIXEL_FORMAT_ARGB_8888;
	case ePixelFormat_A_8:					*pulBpp = 1;	return DI_OSD_PIXEL_FORMAT_RGBA_8888;
	case ePixelFormat_YCBCR_444:			*pulBpp = 2;	return DI_OSD_PIXEL_FORMAT_ARGB_4444;
	case ePixelFormat_YCBCR_422:			*pulBpp = 1;	return DI_OSD_PIXEL_FORMAT_ARGB_4444;

	default:								*pulBpp = 4;	return DI_OSD_PIXEL_FORMAT_ARGB_8888;
	}
}

STATIC HERROR pal_thumb_captureVideoFrame (HUINT32 ulVidDecId, HUINT32 ulWidth, HUINT32 ulHeight, DI_OSD_PIXEL_FORMAT eDiPixelFormat, DI_OSD_HANDLE *phDiOsd)
{
	DI_OSD_HANDLE		 hDiOsd = (DI_OSD_HANDLE)NULL;
	DI_ERR_CODE			 eDiErr;
	HERROR				 hResult = ERR_FAIL;

	*phDiOsd = (DI_OSD_HANDLE)NULL;

	eDiErr = DI_OSD_CreateFrameBuffer (ulWidth, ulHeight, eDiPixelFormat, &hDiOsd);
	if ((DI_ERR_OK != eDiErr) || ((DI_OSD_HANDLE)NULL == hDiOsd))
	{
		HxLOG_Error ("Error:\n");
		goto END_FUNC;
	}

	eDiErr = DI_VIDEO_StartCapture (ulVidDecId, 0, 0, (HUINT8 *)&hDiOsd);
	if (DI_ERR_OK != eDiErr)
	{
		HxLOG_Error ("Error:\n");
		goto END_FUNC;
	}

	*phDiOsd = hDiOsd;
	hDiOsd = (DI_OSD_HANDLE)NULL;
	hResult = ERR_OK;

END_FUNC:
	if ((DI_OSD_HANDLE)NULL != hDiOsd)
	{
		eDiErr = DI_OSD_DeleteFrameBuffer (hDiOsd);
		if (DI_ERR_OK != eDiErr)
		{
			HxLOG_Error ("Error:\n");
		}
	}

	return hResult;
}

STATIC HBOOL pal_thumb_savePngFromFrameBuffer(HCHAR *szFileName, HUINT32 ulWidth, HUINT32 ulHeight, HUINT32 *pBuffer)
{
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	size_t x, y;
	png_uint_32 bytes_per_row;
	png_byte **row_pointers = NULL;
	FILE *fp;

	if (pBuffer == NULL)
		return FALSE;

	fp = fopen(szFileName, "wb");
	if(fp == NULL)
		return FALSE;

	/* Initialize the write struct. */
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL)
	{
		fclose(fp);
		return FALSE;
	}

	/* Initialize the info struct. */
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL)
	{
		png_destroy_write_struct(&png_ptr, NULL);
		fclose(fp);
		return FALSE;
	}

	/* Set up error handling. */
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		png_destroy_write_struct(&png_ptr, &info_ptr);
		fclose(fp);
		return FALSE;
	}

	/* Set image attributes. */
	png_set_IHDR(png_ptr,
	             info_ptr,
	             ulWidth,
	             ulHeight,
	             8,
	             PNG_COLOR_TYPE_RGB_ALPHA ,
	             PNG_INTERLACE_NONE,
	             PNG_COMPRESSION_TYPE_DEFAULT,
	             PNG_FILTER_TYPE_DEFAULT);

	/* Initialize rows of PNG. */
	bytes_per_row = ulWidth * 4;
	row_pointers = png_malloc(png_ptr, ulHeight * sizeof(png_byte *));
	for (y = 0; y < ulHeight; ++y)
	{
		HUINT8 *row = png_malloc(png_ptr, ulWidth * sizeof(HUINT8) * 4);
		row_pointers[y] = (png_byte *)row;
		for (x = 0; x < ulWidth; ++x)
		{
			*row++ = (pBuffer[x + (y * ulWidth)] >> 16) & 0xFF;
			*row++ = (pBuffer[x + (y * ulWidth)] >> 8) & 0xFF;
			*row++ = (pBuffer[x + (y * ulWidth)] >> 0) & 0xFF;
			*row++ = (pBuffer[x + (y * ulWidth)] >> 24) & 0xFF;
		}
	}

	/* Actually write the image data. */
	png_init_io(png_ptr, fp);

	png_set_rows(png_ptr, info_ptr, row_pointers);

	png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

	/* Cleanup. */
	for (y = 0; y < ulHeight; y++)
	{
		png_free(png_ptr, row_pointers[y]);
	}
	png_free(png_ptr, row_pointers);

	/* Finish writing. */
	png_destroy_write_struct(&png_ptr, &info_ptr);

	fclose(fp);

	NOT_USED_PARAM(bytes_per_row);

	return TRUE;
}


#define __PAL_API_FUNCTIONS__

HERROR PAL_THUMB_CaptureImageToBuffer (HUINT32 ulVideoDecId, HUINT32 ulWidth, HUINT32 ulHeight, PixelFormat_e ePixelFormat, HUINT8 **ppucFrameBuffer, HUINT32 *pulBufferSize)
{
	HUINT32				 ulBufSize;
	HUINT32				 ulBpp = 0;
	HUINT8				*pucBuffer = NULL, *pucFrameBuffer = NULL;
	DI_OSD_HANDLE		 hDiOsd = (DI_OSD_HANDLE)NULL;
	DI_OSD_PIXEL_FORMAT	 eDiPixelFormat;
	DI_ERR_CODE			 eDiErr;
	HERROR				 hErr, hResult = ERR_FAIL;

	if ((NULL == ppucFrameBuffer) || (0 == ulWidth) || (0 == ulHeight))
	{
		HxLOG_Error ("Error:\n");
		goto END_FUNC;
	}

	*ppucFrameBuffer = NULL;

	eDiPixelFormat = pal_thumb_convPixelFormat2Di (ePixelFormat, &ulBpp);
	ulBufSize = ulWidth * ulHeight * ulBpp;

	pucBuffer = (HUINT8 *)HLIB_STD_MemAlloc (ulBufSize);
	if (NULL == pucBuffer)
	{
		HxLOG_Error ("Error:\n");
		goto END_FUNC;
	}

	hErr = pal_thumb_captureVideoFrame (ulVideoDecId, ulWidth, ulHeight, eDiPixelFormat, &hDiOsd);
	if ((ERR_OK != hErr) || ((DI_OSD_HANDLE)NULL == hDiOsd))
	{
		HxLOG_Error ("Error:\n");
		goto END_FUNC;
	}

	eDiErr = DI_OSD_GetFrameBuffer (hDiOsd, (void **)&pucFrameBuffer);
	if ((DI_ERR_OK != eDiErr) || (NULL == pucFrameBuffer))
	{
		HxLOG_Error ("Error:\n");
		goto END_FUNC;
	}

	HxSTD_MemCopy (pucBuffer, pucFrameBuffer, ulBufSize);

	*ppucFrameBuffer = pucBuffer;
	if (NULL != pulBufferSize)						{ *pulBufferSize = ulBufSize; }

	pucBuffer = NULL;
	hResult = ERR_OK;

END_FUNC:
	if (NULL != pucBuffer)
	{
		HLIB_STD_MemFree (pucBuffer);
	}

	if ((DI_OSD_HANDLE)NULL != hDiOsd)
	{
		eDiErr = DI_OSD_DeleteFrameBuffer (hDiOsd);
		if (DI_ERR_OK != eDiErr)
		{
			HxLOG_Error ("Error:\n");
		}
	}

	return hResult;
}

HERROR PAL_THUMB_FreeBuffer (HUINT8 *pucFrameBuffer)
{
	if (NULL == pucFrameBuffer)				{ return ERR_FAIL; }

	HLIB_STD_MemFree (pucFrameBuffer);
	return ERR_OK;
}

HERROR PAL_THUMB_CaptureImageToPngFile (HUINT32 ulVideoDecId, HUINT32 ulWidth, HUINT32 ulHeight, HCHAR *szFileName)
{
	HUINT8				*pucFrameBuffer = NULL;
	DI_OSD_HANDLE		 hDiOsd = (DI_OSD_HANDLE)NULL;
	DI_ERR_CODE			 eDiErr;
	HERROR				 hErr, hResult = ERR_FAIL;

	if ((NULL == szFileName) || (0 == ulWidth) || (0 == ulHeight))
	{
		HxLOG_Error ("Error:\n");
		goto END_FUNC;
	}

	hErr = pal_thumb_captureVideoFrame (ulVideoDecId, ulWidth, ulHeight, DI_OSD_PIXEL_FORMAT_ARGB_8888, &hDiOsd);
	if ((ERR_OK != hErr) || ((DI_OSD_HANDLE)NULL == hDiOsd))
	{
		HxLOG_Error ("Error:\n");
		goto END_FUNC;
	}

	eDiErr = DI_OSD_GetFrameBuffer (hDiOsd, (void **)&pucFrameBuffer);
	if ((DI_ERR_OK != eDiErr) || (NULL == pucFrameBuffer))
	{
		HxLOG_Error ("Error:\n");
		goto END_FUNC;
	}

	hErr = pal_thumb_savePngFromFrameBuffer(szFileName, ulWidth, ulHeight, (HUINT32 *)pucFrameBuffer);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("Error:\n");
		goto END_FUNC;
	}

	hResult = ERR_OK;

END_FUNC:
	if ((DI_OSD_HANDLE)NULL != hDiOsd)
	{
		eDiErr = DI_OSD_DeleteFrameBuffer (hDiOsd);
		if (DI_ERR_OK != eDiErr)
		{
			HxLOG_Error ("Error:\n");
		}
	}

	return hResult;
}

HERROR PAL_THUMB_ReadFile (HCHAR *szFileName, HUINT32 *pulWidth, HUINT32 *pulHeight, HUINT8 *pucBuffer)
{
	PAL_FS_UINT32	 ulFileDes = 0;
	HUINT32			 ulHeaderSize;
	HUINT32			 ulWidth, ulHeight, ulType, ulBpp;
	HUINT8			 aucTempBuffer[8];
	HUINT8			*pucHeader = NULL;
	PAL_FS_UINT32	 ulSizeP;
	HERROR			 hErr, hResult = ERR_FAIL;

	HxFILE_t		hFile = NULL;

	if ((NULL == szFileName) || (NULL == pulWidth) || (NULL == pulHeight) || (NULL == pucBuffer))
	{
		goto END_FUNC;
	}

	hFile = HLIB_FILE_Open(szFileName, "r");
	if(hFile == NULL)
	{
		HxLOG_Error("error occurred in HLIB_FILE_Open(%s).\n", szFileName);
		goto END_FUNC;
	}

	hErr = HLIB_FILE_Seek(hFile, 0, eHxFILE_SEEK_SET, NULL);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("error occurred in HLIB_FILE_Seek(%d).\n", hErr);
		goto END_FUNC;
	}
	ulSizeP = HLIB_FILE_Read(hFile, (PAL_FS_UINT8 *)aucTempBuffer, 1, 8);
	if (ulSizeP <= 0)
	{
		HxLOG_Error("error occurred in HLIB_FILE_Read.\n");
		goto END_FUNC;
	}

	if (HxSTD_memcmp (aucTempBuffer, s_szPalThumbIndHeader, palThumb_INDICATOR_SIZE) != 0)
	{
		goto END_FUNC;
	}

	if (aucTempBuffer[3] != palThumb_VERSION)
	{
		goto END_FUNC;
	}

	ulHeaderSize = Read32Bit (aucTempBuffer + 4);
	if (0 == ulHeaderSize)
	{
		goto END_FUNC;
	}

	pucHeader = (HUINT8 *)OxMW_Malloc (ulHeaderSize);
	if (NULL == pucHeader)
	{
		goto END_FUNC;
	}


	hErr = HLIB_FILE_Seek(hFile, 8, eHxFILE_SEEK_SET, NULL);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("error occurred in HLIB_FILE_Seek(%d).\n", hErr);
		goto END_FUNC;
	}
	ulSizeP = HLIB_FILE_Read(hFile, (PAL_FS_UINT8 *)pucHeader, 1, ulHeaderSize);
	if (ulSizeP <= 0)
	{
		HxLOG_Error("error occurred in HLIB_FILE_Read.\n");
		goto END_FUNC;
	}

	ulWidth = (HUINT32)Read16Bit (pucHeader);
	ulHeight = (HUINT32)Read16Bit (pucHeader + 2);
	ulType = (HUINT32)Read16Bit (pucHeader + 4);
	ulBpp = (HUINT32)Read16Bit (pucHeader + 6);

	switch (ulType)
	{
	case ePAL_THUMB_IMAGETYPE_ARGB32:
		hErr = HLIB_FILE_Seek(hFile, 8 + ulHeaderSize, eHxFILE_SEEK_SET, NULL);
		hErr |= pal_thumb_readArgb32 (hFile, ulWidth, ulHeight, ulBpp, pucHeader, pucBuffer);
		break;

	default:
		hErr = ERR_FAIL;
		break;
	}

	if (ERR_OK != hErr)
	{
		goto END_FUNC;
	}

	*pulWidth	= ulWidth;
	*pulHeight	= ulHeight;
	hResult		= ERR_OK;

END_FUNC:
	if (NULL != pucHeader)
	{
		OxMW_Free (pucHeader);
	}

	if (0 != ulFileDes)
	{
		HLIB_FILE_Close(hFile);
	}

	return hResult;
}



/*********************** End of File ******************************/
