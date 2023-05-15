/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  layer_module.h File Name을 적습니다.
	@brief	  file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__HLIB_EXIF_H__
#define	__HLIB_EXIF_H__

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
/// EXIF 정보.
typedef enum {
	eHxEXIF_Date_time = 0,
	eHxEXIF_Dimensions,
	eHxEXIF_Orientation,
	eHxEXIF_Exposure_Time,
	eHxEXIF_Exposure_Program,
	eHxEXIF_ISO_Speed_Ratings,
	eHxEXIF_Exposure_Bias_Value,
	eHxEXIF_Flash,
	eHxEXIF_F_Number,
	eHxEXIF_Entry_Num
} HxEXIF_Entry_e;

typedef struct
{
	HINT32	nWidth;
	HINT32	nHeight;

	HCHAR	szDate[32];
} HxEXIF_Info_t;

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
extern HERROR	HLIB_EXIF_GetFileInfo(const HCHAR *pszFileName, HxEXIF_Info_t *pstInfo);
extern HUINT8*	HLIB_EXIF_GetThumbnail(const HCHAR *pszFileName, HINT32 *pnSize);
extern void		HLIB_EXIF_FreeBuffer(HUINT8 *pucBuffer);

#endif /* !__PALXML_H__ */

