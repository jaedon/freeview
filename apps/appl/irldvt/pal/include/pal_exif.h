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

/// EXIF �Ľ� ����� �̹��� ���� ���� ��Ÿ �������� EXIF(Exchangeable Image File
/// Format)�� �Ľ��Ͽ� �پ��� ������ ������ ������ �Ѵ�. @n
/// �� ����� libexif ���̺귯���� ����Ѵ�.
///
/// ���� �� ����� �����ϴ� ������ �Ʒ��� ����.
///
/// ==========================================================================
///
/// 1. ����� ������
///
/// 2. �Կ� �Ͻ� (Date and Time)
///
/// 3. ȭ�� ������ (Dimensions)
///
/// 4. ���� �ð� (Exposure Time)
///
/// 5. ���� ���α׷� (Exposure Program)
///
///   @li ������ (Not defined)
///   @li ��� ���α׷� (Normal program)
///   @li ���� �켱 (Aperture priority)
///   @li ���� �켱 (Shutter priority)
///   @li creative ���α׷� (Creative program)
///   @li action ���α׷� (Action program)
///   @li ��Ʈ����Ʈ ��� (Portrait mode)
///   @li ���彺ĳ���� ��� (Landscape mode)
///
/// 6. ISO ���ǵ� ����Ʈ (ISO Speed Ratings)
///
/// 7. ���� ������ (Exposure Bias Value)
///
/// 8. �÷��� (Flash)
///   @li strobe �߱� (Fired)
///   @li strobe �߱� ���� (Not fired)
///
/// 9. F �� (F Number)
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
/// EXIF ����.
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


