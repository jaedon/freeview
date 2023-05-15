/*
 * (c) 2011-2013 Humax Co., Ltd.
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
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

/******************************************************************************/
/**
 * @file	  		nx_system_startlogo.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#ifndef __NX_SYSTEM_STARTLOGO_H__
#define __NX_SYSTEM_STARTLOGO_H__


/**************************************************************************************************/
#define _________StartLogo_Private_Include_________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________StartLogo_Private_Macro_________________________________________________
/**************************************************************************************************/
#define 		STARTLOGO_LOW_QUALITY

#define			STARTLOGO_IMG_ID							1

#if defined(STARTLOGO_LOW_QUALITY)
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

#define 		STARTLOGO_FONT_PATH							"/usr/fonts/HelveticaNeueLTW1G-MdCnUpdv3.001.otf"
#define			STARTLOGO_FONT_SIZE							28
#define			STARTLOGO_FONT_COLOR							0xffffff00

#define			STARTLOGO_TEXT_WIDTH_SIZE					50

#define			VFD_STRING_COUNT		(sizeof(s_ucVFDStrings) / sizeof(s_ucVFDStrings[0]))

/**************************************************************************************************/
#define _________StartLogo_Private_Value_________________________________________________
/**************************************************************************************************/





/**************************************************************************************************/
#define _________StartLogo_Private_typedef_________________________________________________
/**************************************************************************************************/



/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
ONDK_Result_t		NX_SYSTEM_Startlogo_Proc(HINT32 lMessage, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3);


#endif /* !__NX_SYSTEM_STARTLOGO_H__ */

