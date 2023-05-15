/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  layer_module.h File Name�� �����ϴ�.
	@brief	  file_name & simple comment.

	Description: File�� ���� ������ �����ϴ�.		\n
	Module: Layer�� ��� ���� �����ϴ�.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__HLIB_PNG_H__
#define	__HLIB_PNG_H__

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
typedef struct
{
	HINT32		nWidth;
	HINT32		nHeight;
	HUINT32		ulPitch;
	HUINT8		ucFormat;
	HUINT8		ucBitPerPixel;
} HxPNG_Info_t;

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
HERROR	HLIB_PNG_GetFileInfo(const HCHAR *pszFileName, HxPNG_Info_t *pstInfo);
HBOOL	HLIB_PNG_WriteFile(const HCHAR *pszFileName, HUINT32 ulWidth, HUINT32 ulHeight, HUINT32 *pulBuffer);

#endif /* !__HLIB_PNG_H__ */