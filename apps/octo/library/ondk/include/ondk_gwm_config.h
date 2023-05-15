/**************************************************************
 *	@file		mlib.h
 *	Minimized Graphic Library for DirectFB
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/03/07
 *	@author			humax
 *	@brief			Minimized Graphic Library for DirectFB
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
//#include "octo_config.h"

#ifndef	__GWMCFG_H__
#define __GWMCFG_H__


//////////////////////////////////////////////////////
// 기본 OSD 표시영역을 정의한다.
//////////////////////////////////////////////////////
#define GWM_SCREEN_OFFSET_X			0
#define GWM_SCREEN_OFFSET_Y			0

#if	defined(CONFIG_MWC_OSD_HORIZONTAL) && defined(CONFIG_MWC_OSD_VERTICAL)
	#define OSD_SCREEN_WIDTH			CONFIG_MWC_OSD_HORIZONTAL
	#define OSD_SCREEN_HEIGHT			CONFIG_MWC_OSD_VERTICAL
#else
	#define OSD_SCREEN_WIDTH			1280
	#define OSD_SCREEN_HEIGHT			720
#endif

#define GWM_SCREEN_WIDTH			1280
#define GWM_SCREEN_HEIGHT			720


//////////////////////////////////////////////////////
// 그 외의 GWM 의 설정
//////////////////////////////////////////////////////

#define	DEF_APP_NAME_LENGTH					32

#define	DEF_GWM_TIMER_SLEEP_TIME			50			// Timer task의 sleep 간격.
#define	DEF_GWM_MAX_TIMER					64			/* MSG_GWM_TIMER 의 total 갯수.			(16~256 권장.)*/
#define	DEF_GWM_MAX_APPLICATION				48			/* GWM_Application의 total 갯수.*/
#define	DEF_GWM_ENDIAN_LITTLE				CONFIG_PROD_ENDIAN_LITTLE

//#define CONFIG_GWM_O2_ANIMATION
#endif	/*__GWMCFG_H__*/

