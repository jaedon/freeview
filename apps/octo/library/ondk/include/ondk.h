/**************************************************************
 *	@file		ondk.h
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
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/

/*
* ¨Ï 2011-2012 Humax Co., Ltd.
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


#ifndef ____ONDK_H____
#define ____ONDK_H____

#if 0 //: warning: "CONFIG_PRODUCT_NAME" redefined

#ifdef CONFIG_PRODUCT_NAME
#undef CONFIG_PRODUCT_NAME
#endif

#endif

#if defined(CONFIG_SUPPORT_NATIVE_APP)
#include	"res_strid.h"
#endif

#include 	<hlib.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<stdarg.h>
//#include	<oapi.h>
#include	<vkernel.h>
#include	<signal.h>

#include 	<ondk_type.h>
#include 	<ondk_debug.h>
#include	<directfb.h>
#include	<sawman.h>

typedef	enum
{
	MOSD_FULLSCREEN = 0,
	MOSD_LETTERBOX,
	MOSD_PILLARBOX,
} MOSD_ASPECTRATIO;


#define ONDK_UPPER_WINDOWID 	eAWINDOW_GWMPOPUP
#define ONDK_LOWER_WINDOWID 	eAWINDOW_SUBTITLEPLAYER

#define ONDK_Malloc					HLIB_STD_MemAlloc
#define ONDK_Calloc					HLIB_STD_MemCalloc
#define	ONDK_Realloc				HLIB_STD_MemRealloc
#define	ONDK_StrDup					HLIB_STD_StrDup
#define	ONDK_MemDup					HLIB_STD_MemDup
#define ONDK_Free					HLIB_STD_MemFree

#include	<ondk_gfx.h>
#include 	<ondk_gwm.h>
#include 	<ondk_strings.h>
#include	<ondk_gwm_message.h>
#include   <ondk_gwm_config.h>
//#define ONDK_DFB_KEY_USE_DFBBUFFER

// resolved proplem that calling prism local funtion of gwm thread at DFB Video Provider thread.
#ifndef ONDK_DFB_VIDEO_CALLBACK_TO_GWM_THREAD
#define ONDK_DFB_VIDEO_CALLBACK_TO_GWM_THREAD
#endif

#endif





