/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  	pal_exif.h
	@brief		EXIF meta data handler's header

	Description: 		\n
	Module: 	pal			 	\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__PAL_EXIF_H__
#define	__PAL_EXIF_H__

/// EXIF 파싱 모듈은 이미지 파일 내의 메타 데이터인 EXIF(Exchangeable Image File
/// Format)을 파싱하여 다양한 정보를 얻어오는 역할을 한다. @n
/// 이 모듈은 libexif 라이브러리를 사용한다.
///
/// 현재 이 모듈이 지원하는 정보는 아래와 같다.
///
/// ==========================================================================
///
/// 1. 썸네일 데이터
///
/// 2. 촬영 일시 (Date and Time)
///
/// 3. 화상 사이즈 (Dimensions)
///
/// 4. 노출 시간 (Exposure Time)
///
/// 5. 노출 프로그램 (Exposure Program)
///
///   @li 미정의 (Not defined)
///   @li 노멀 프로그램 (Normal program)
///   @li 노출 우선 (Aperture priority)
///   @li 셔터 우선 (Shutter priority)
///   @li creative 프로그램 (Creative program)
///   @li action 프로그램 (Action program)
///   @li 포트래이트 모드 (Portrait mode)
///   @li 랜드스캐이프 모드 (Landscape mode)
///
/// 6. ISO 스피드 래이트 (ISO Speed Ratings)
///
/// 7. 노출 보정값 (Exposure Bias Value)
///
/// 8. 플래시 (Flash)
///   @li strobe 발광 (Fired)
///   @li strobe 발광 안함 (Not fired)
///
/// 9. F 값 (F Number)
///
///===========================================================================


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include "octo_common.h"


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
	PAL_Exif_Date_time = 0,
	PAL_Exif_Dimensions,
	PAL_Exif_Orientation,
	PAL_Exif_Exposure_Time,
	PAL_Exif_Exposure_Program,
	PAL_Exif_ISO_Speed_Ratings,
	PAL_Exif_Exposure_Bias_Value,
	PAL_Exif_Flash,
	PAL_Exif_F_Number,
	PAL_Exif_Entry_Num
} PAL_Exif_Entry_e;


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

void*	PAL_EXIF_Open(HUINT8 *filename);
void	PAL_EXIF_Close(void*	pvExif);
HUINT8*	PAL_EXIF_GetThumbnail(void* pvExif, HINT32 *pnSize);
HUINT8*	PAL_EXIF_GetInfoString(void* pvExif, PAL_Exif_Entry_e eEntry, HUINT8 *pucBuf);
int		PAL_EXIF_GetDimension(void* pvExif, HINT32 *out_width, HINT32 *out_height);
int		PAL_EXIF_GetOrientation(void* pvExif, HUINT32 *out_orientation);

#endif /* !__PAL_EXIF_H__ */


