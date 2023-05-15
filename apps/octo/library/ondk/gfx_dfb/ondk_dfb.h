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

#ifndef ____ONDK_DFB_H____
#define ____ONDK_DFB_H____


/*
 * CONFIG_PACKAGE_DIRECTFB를 사용 할 경우
 */

#ifdef CONFIG_PACKAGE_DIRECTFB

#include	<directfb.h>

#include	<sawman.h>


#if defined(OS_LINUX)

#define DFBCHECK(x...)                                                     \
               err = x;                                                    \
               if (err != DFB_OK) {                                        \
                    fprintf( stdout, "%s <%d>:\n\t", __FILE__, __LINE__ ); \
                    DirectFBErrorFatal( #x, err );                         \
               }


#else

#define DFBCHECK(x)                                               \
               err = x;                                                    \
               if (err != DFB_OK) {                                        \
                    ONDK_Error("dfb error."); \
                    DirectFBErrorFatal( #x, err );                         \
               }



#endif /* #if defined(OS_LINUX) */


extern IDirectFB					*ONDK_DFB_GetIDirectFB(void);
extern IDirectFBDisplayLayer		*ONDK_DFB_GetIDirectFBDisplayLayer(void);


#endif /* #ifdef CONFIG_PACKAGE_DIRECTFB */







/*
 * CONFIG_PACKAGE_DIRECTFB를 사용하지 않을 경우
 */

#ifndef CONFIG_PACKAGE_DIRECTFB

/*

typedef struct  _IDirectFB 				{ int dummy; }	IDirectFB;
typedef struct  _IDirectFBSurface  		{ int dummy; }	IDirectFBSurface;
typedef struct  _IDirectFBPalette  		{ int dummy; }	IDirectFBPalette;
typedef struct  _IDirectFBInputDevice	{ int dummy; }	IDirectFBInputDevice;
typedef struct  _IDirectFBDisplayLayer  { int dummy; }	IDirectFBDisplayLayer;
typedef struct  _IDirectFBWindow  		{ int dummy; }	IDirectFBWindow;
typedef struct  _IDirectFBEventBuffer	{ int dummy; }	IDirectFBEventBuffer;
typedef struct  _IDirectFBFont  		{ int dummy; }	IDirectFBFont;
typedef struct  _IDirectFBImageProvider { int dummy; }	IDirectFBImageProvider;
typedef struct  _IDirectFBVideoProvider { int dummy; }	IDirectFBVideoProvider;
typedef struct  _IDirectFBDataBuffer  	{ int dummy; }	IDirectFBDataBuffer;
typedef struct  _IDirectFBScreen  		{ int dummy; }	IDirectFBScreen;
typedef struct  _IDirectFBGL  			{ int dummy; }	IDirectFBGL;


typedef struct  _IDirectFB 				IDirectFB;
typedef struct  _IDirectFBSurface  		IDirectFBSurface;
typedef struct  _IDirectFBPalette  		IDirectFBPalette;
typedef struct  _IDirectFBInputDevice	IDirectFBInputDevice;
typedef struct  _IDirectFBDisplayLayer  IDirectFBDisplayLayer;
typedef struct  _IDirectFBWindow  		IDirectFBWindow;
typedef struct  _IDirectFBEventBuffer	IDirectFBEventBuffer;
typedef struct  _IDirectFBFont  		IDirectFBFont;
typedef struct  _IDirectFBImageProvider IDirectFBImageProvider;
typedef struct  _IDirectFBVideoProvider IDirectFBVideoProvider;
typedef struct  _IDirectFBDataBuffer  	IDirectFBDataBuffer;
typedef struct  _IDirectFBScreen  		IDirectFBScreen;
typedef struct  _IDirectFBGL  			IDirectFBGL;

typedef HINT32	DFBResult;
typedef HINT32	ISaWMan;


typedef HINT32 	DFBFontDescription;
typedef HINT32	DFBWindowID;
typedef HINT32	DFBWindowDescription;


#define DFBCHECK(x)                                                     \
               err = x;                                                    \
               if (err != ONDK_RET_OK) {                                        \
                    fprintf( stdout, "%s <%d>:\n\t", __FILE__, __LINE__ ); \
               }

*/


#endif /* CONFIG_PACKAGE_DIRECTFB */

#endif





