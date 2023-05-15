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
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ⓒ 2011-2012 Humax Co., Ltd.
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
#ifndef	__STARTLOGO_MAIN_H__
#define	__STARTLOGO_MAIN_H__

#if defined(CONFIG_PRISM_ZAPPING)
#include <prism.h>
#include <clib.h>
#endif

/**************************************************************************************************/
#define _________StartLogo_Private_Include_________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________StartLogo_Private_Macro_________________________________________________
/**************************************************************************************************/
#define 		STARTLOGO_LOW_QUALITY

#if defined (CONFIG_PRISM_ZAPPING)
typedef enum
{
	STARTLOGO_IMG_ID	= 0x1000,
	STARTLOGO_LOADING_BG_ID,
	STARTLOGO_LOADING_IMG_ID,
	STARTLOGO_LOADING_TEXT_ID,
	STARTLOGO_SYSMSG_ICON_ID,
	STARTLOGO_SYSMSG_YELLOWLINE_UP_ID,
	STARTLOGO_SYSMSG_YELLOWLINE_DOWN_ID,
	STARTLOGO_SYSMSG_ID = 0x2000
	// STARTLOGO_SYSMSG_ID 는 Line수에 따라 확장이 가능하므로, 뒤에 추가 하지 않는다. 혹은 0x3000부터 시작하도록 한다.
} startLogoDisplayID_e;

#else
#define			STARTLOGO_IMG_ID							1
#endif


#if defined (CONFIG_PRISM_ZAPPING)

	#define 		STARTLOGO_X 								0
	#define 		STARTLOGO_Y 								0
	#define 		STARTLOGO_W 								1280
	#define 		STARTLOGO_H 								720

	#define 		STARTLOGO_IMG_X 							480
	#define 		STARTLOGO_IMG_Y 							250
	#define 		STARTLOGO_IMG_W 							320
	#define 		STARTLOGO_IMG_H 							220
	#define 		STARTLOGO_IMG								"/usr/image/startlogo.gif"

	#define 		LOADING_BG_X								1040
	#define 		LOADING_BG_Y								0
	#define 		LOADING_BG_W								240
	#define 		LOADING_BG_H								100

	#define 		LOADING_IMG_X								1050
	#define 		LOADING_IMG_Y								30
	#define 		LOADING_IMG_W								60
	#define 		LOADING_IMG_H								60

	#define			LOADING_IMG_TEXT_GAP						5
	#define			LOADING_TEXT_X								LOADING_IMG_X + LOADING_IMG_W + LOADING_IMG_TEXT_GAP
	#define			LOADING_TEXT_Y								LOADING_IMG_Y + 40
	#define			LOADING_TEXT_W								165
	#define			LOADING_TEXT_H								40
	#define			LOADING_TEXT_FONT_SIZE						20
	#define			LOADING_TEXT_FONT_COLOR						0xffffffff

// for 1 Line System Message

	#define			SYSTEM_MSG_FONT_COLOR						0xffffff00
	#define			SYSTEM_MSG_FONT_SIZE						28

	#define			SYSTEM_MSG_YELLOWLINE_UP_X					240
	//화면 위에서부터 310px 내려옴
	#define			SYSTEM_MSG_YELLOWLINE_UP_Y					STARTLOGO_Y + 310
	#define			SYSTEM_MSG_YELLOWLINE_UP_W					800
	#define			SYSTEM_MSG_YELLOWLINE_UP_H					1

	#define 		SYSTEM_MSG_ICON_W							55
	#define 		SYSTEM_MSG_ICON_H							48
	#define 		SYSTEM_MSG_ICON_X							SYSTEM_MSG_YELLOWLINE_UP_X + 70
	//화면 중앙에서 이미지 절반만큼 위로
	#define 		SYSTEM_MSG_ICON_Y							(STARTLOGO_Y + STARTLOGO_H)/2 -SYSTEM_MSG_ICON_H/2

	#define 		SYSTEM_MSG_TEXT_W							580
	#define 		SYSTEM_MSG_TEXT_H							34
	#define 		SYSTEM_MSG_TEXT_X							SYSTEM_MSG_ICON_X + SYSTEM_MSG_ICON_W + 40
	#define 		SYSTEM_MSG_TEXT_Y							(STARTLOGO_Y + STARTLOGO_H)/2 + 10

	#define			SYSTEM_MSG_YELLOWLINE_DOWN_X				SYSTEM_MSG_YELLOWLINE_UP_X
	// 화면 아래에서부터 310px 올라옴
	#define			SYSTEM_MSG_YELLOWLINE_DOWN_Y				STARTLOGO_Y + STARTLOGO_H - 310
	#define			SYSTEM_MSG_YELLOWLINE_DOWN_W				SYSTEM_MSG_YELLOWLINE_UP_W
	#define			SYSTEM_MSG_YELLOWLINE_DOWN_H				SYSTEM_MSG_YELLOWLINE_UP_H

	#define			SYSTEM_MSG_YELLOWLINE						"/usr/image/107_Line_DW_Yellow.png"
	#define			SYSTEM_MSG_IMG_ICON							"/usr/image/311_DW_Icon_Status.png"


	typedef enum
	{
		STARTLOGO_LOADING_IMG_1 = 0,
		STARTLOGO_LOADING_IMG_2,
		STARTLOGO_LOADING_IMG_3,
		STARTLOGO_LOADING_IMG_4,
		STARTLOGO_LOADING_IMG_5,
		STARTLOGO_LOADING_IMG_6,
		STARTLOGO_LOADING_IMG_7,
		STARTLOGO_LOADING_IMG_8,
		STARTLOGO_LOADING_IMG_9,
		STARTLOGO_LOADING_IMG_10,
		STARTLOGO_LOADING_IMG_11,
		STARTLOGO_LOADING_IMG_12,
		STARTLOGO_LOADING_IMG_13,
		STARTLOGO_LOADING_IMG_14,
		STARTLOGO_LOADING_IMG_15,
		IMG_URL_MAX
	} startLogoImageURL_e;




#elif defined(STARTLOGO_LOW_QUALITY)
	#define			STARTLOGO_X									480
	#define			STARTLOGO_Y									250
	#define			STARTLOGO_W									320
	#define			STARTLOGO_H									220
	#define			STARTLOGO_IMG									"/usr/image/startlogo.gif"
#else
	#define			STARTLOGO_X									0
	#define			STARTLOGO_Y									0
	#define			STARTLOGO_W									1280
	#define			STARTLOGO_H									720
	#define			STARTLOGO_IMG									"/usr/image/startlogo.gif"
#endif

#if defined(CONFIG_OP_TDC)
#undef			STARTLOGO_X
#undef			STARTLOGO_Y
#undef			STARTLOGO_W
#undef			STARTLOGO_H
#undef			STARTLOGO_IMG
#define 		STARTLOGO_X 								0//535
#define 		STARTLOGO_Y 								0//348
#define 		STARTLOGO_W 								1280//209
#define 		STARTLOGO_H 								720//24
#define 		STARTLOGO_IMG								"/usr/image/startlogo.gif"
#endif

#if defined(CONFIG_PRISM_ZAPPING)
typedef enum
{
	STARTLOGO_CHANNEL_CURRENT = 0,
		STARTLOGO_CHANNEL_PREV,
		STARTLOGO_CHANNEL_NEXT
}startLogoChannel_e;
#endif

#define 		STARTLOGO_FONT_PATH							"/usr/fonts/HelveticaNeueLTW1G-MdCnUpdv3.001.otf"
#define			STARTLOGO_FONT_SIZE							28
#define			STARTLOGO_FONT_COLOR							0xffffff00

#define			STARTLOGO_TEXT_WIDTH_SIZE					50

#define			VFD_STRING_COUNT		(sizeof(s_ucVFDStrings) / sizeof(s_ucVFDStrings[0]))

/**************************************************************************************************/
#define _________StartLogo_Private_Value_________________________________________________
/**************************************************************************************************/


#if defined(CONFIG_OP_FREESAT)
	#define	VFD_STRING_DIVIDE		7
	#define	LOGO_LOOP_COUNT			250
	static char *s_ucVFDStrings[] = {
		"            ",
		"     <>     ",
		"    <fr>    ",
		"   <free>   ",
		"  <free t>  ",
		" <free tim> ",
		"<free time>",
		" ",
		"<free time>",
		" ",
		"<free time>",
		" ",
		"<free time>",
	};

#elif defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE) || defined(CONFIG_PROD_HMS1000T)
	#define	VFD_STRING_DIVIDE		3
	#define	LOGO_LOOP_COUNT			250
	static char *s_ucVFDStrings[] = {
		"STARTING... ",
	};

#elif defined(CONFIG_PROD_IR4000HD)
	#define	VFD_STRING_DIVIDE		3
	#define	LOGO_LOOP_COUNT			250
	static char *s_ucVFDStrings[] = {
		"BOOT",
		"OOT ",
		"OT  ",
		"T   ",
		"    ",
		"   B",
		"  BO",
		" BOO",
	};

#else
	#define	VFD_STRING_DIVIDE		3
	#define	LOGO_LOOP_COUNT			250
	static char *s_ucVFDStrings[] = {
		"BOOT",
		"OOT ",
		"OT  ",
		"T   ",
		"    ",
	};
#endif


/**************************************************************************************************/
#define _________StartLogo_Private_typedef_________________________________________________
/**************************************************************************************************/



/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

#endif /* !__STARTLOGO_MAIN_H__ */

