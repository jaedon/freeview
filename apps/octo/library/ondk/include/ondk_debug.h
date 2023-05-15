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
 *	CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
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

#ifndef ____ONDK_DEBUG_H____
#define ____ONDK_DEBUG_H____

//#define CONFIG_UART_ONLY

#if	defined(CONFIG_UART_ONLY)
	#define ONDK_Print(...) 	printf(__VA_ARGS__)
	#define ONDK_Info(...)		printf(__VA_ARGS__)
	#define ONDK_Trace(...) 	printf(__VA_ARGS__)
	#define ONDK_Error(...) 	printf(__VA_ARGS__)
	#define ONDK_Debug(...) 	printf(__VA_ARGS__)
	#define ONDK_Message(...) 	printf(__VA_ARGS__)
	//#define	ONDK_Assert(x)	{ if ((int)(x)==0) { HxLOG_Print("\n\n\t[%s:%d]ONDK Assertion Failure !!.\n[%s:%d]ONDK Assertion Failure !!.\n\n\n\n", __FUNCTION__, __LINE__, __FUNCTION__, __LINE__); HxLOG_Assert(x); /*HxLOG_Print("%d", (1/0) );*/ } }
	#define ONDK_Assert(x)		{ if ((HINT32)(x)==0) { printf("\n\n\t[%s:%d]ONDK Assertion Failure !!.\n[%s:%d]ONDK Assertion Failure !!.\n\n\n\n", __FUNCTION__, __LINE__, __FUNCTION__, __LINE__);	} }
#else
	#define	ONDK_Print			HxLOG_Print
	#define	ONDK_Info			HxLOG_Info
	#define	ONDK_Trace			HxLOG_Print
	#define	ONDK_Error			HxLOG_Error
	#define	ONDK_Debug			HxLOG_Debug
	#define ONDK_Message		HxLOG_Message
	#define	ONDK_Assert(x)		HxLOG_Assert(x)
#endif

#ifndef ENUM_TO_STR
#define ENUM_TO_STR(enumVal) 			case enumVal: return #enumVal;
#endif

#ifndef VALUE_TO_STR
#define VALUE_TO_STR(x)					#x
#endif

#ifndef NOT_USED_PARAM
#define NOT_USED_PARAM(x)				(void)(x)
#endif


/* function */

#if defined(CONFIG_DEBUG)
extern	ONDK_Result_t  	ONDK_DEBUG_Init(HINT32 argc, HCHAR **argv);
#endif
extern	HCHAR			*ONDK_GetCommandName(void);


extern void ONDK_TEST_Init(void);
extern void ONDK_TEST_Start(HCHAR *module);
extern void ONDK_TEST_OSD_Start(void);
extern void ONDK_TEST_IMAGE_Start(void);
extern void ONDK_TEST_TIMER_Start(void);
extern void ONDK_TEST_STRING_Start(void);
extern void ONDK_TEST_FONT_Start(void);
extern void ONDK_TEST_MSGHOOK_Start(void);
extern void ONDK_TEST_FPS_Start(void);
extern void ONDK_TEST_HOME_Start(void);
extern void ONDK_TEST_O2ANI_Start(void);


#endif /* ____ONDK_DEBUG_H____ */




