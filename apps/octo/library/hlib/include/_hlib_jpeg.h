/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  layer_module.h File NameÀ» Àû½À´Ï´Ù.
	@brief	  file_name & simple comment.

	Description: File¿¡ ´ëÇÑ ¼³¸íÀ» Àû½À´Ï´Ù.		\n
	Module: Layer¿Í ¸ðµâ ¸íÀ» Àû½À´Ï´Ù.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__HX_JPEG_H__
#define	__HX_JPEG_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
HERROR	HLIB_JPEG_GetFileInfo(const HCHAR *pszFileName, HUINT32 *out_width, HUINT32 *out_height);
HERROR	HLIB_JPEG_GetBufferInfo(HUINT8 *pucData, HINT32 size, HUINT32 *out_width, HUINT32 *out_height);
HERROR	HLIB_JPEG_CreateScaledJPEG(const HCHAR *pszInFileName, const HCHAR *pszOutFileName, HINT32 nScaledWidth, HINT32 nScaledHeight);
HERROR	HLIB_JPEG_CreateScaledJPEGMEM(HUINT8 *pucData, HINT32 size, const HCHAR *pszOutFileName, HINT32 nScaledWidth, HINT32 nScaledHeight);
HERROR	HLIB_JPEG_CreateFile(const HCHAR *pszOutName, HINT32 nWidth, HINT32 nHeight, const HUINT8 *pucBuffer, HERROR (*cvtFunc)(HUINT8*, const HUINT8 *, HINT32, HINT32));

HERROR	HLIB_JPEG_ConvertBGRA2RGB(HUINT8 *pstBuffer, const HUINT8 *pucSrc, HINT32 nWidth, HINT32 nScanline);

#endif /* !__PALXML_H__ */

