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


#ifndef ____ONDK_TYPE_H____
#define ____ONDK_TYPE_H____

#include <ondk_keytype.h>
#include <ondk_errortype.h>
#include <ondk_gwm_config.h>

typedef	void*		ONDKSurfaceHandle;
typedef	void*		ONDKWindowHandle;
typedef	void*		ONDKKeyHandle;
typedef	void*		ONDKImageHandle;
typedef	void*		ONDKFontHandle;
typedef	void*		ONDKEvnetBufferHandle;
typedef void*		ONDKVideoHandle;

typedef	void*		ONDKHumaxKeyHandle;


typedef	HUINT32		ONDK_Color_t;

//	GWMHAPI_MESSAGE_DEFINITION <-- temporary solution -->
typedef enum
{
	eHMSG_APP_READY = 0x00010000,
	eHMSG_BOOTUPSTATE_CHANGE,
	eHMSG_OSK_SHOW,
	eHMSG_OSK_DESTROY,
	eHMSG_OSK_MODECHANGE,
	eHMSG_OSK_SHOW_SIMPLE,
	eHMSG_REMOTEAPP_STRING,
	eHMSG_SYSSTATE_CHANGE,
	eHMSG_SUBTITLE_STATE,
	eHMSG_TELETEXT_STATE,
	eHMSG_SUBTITLE_TYPE_CHANGE,
	eHMSG_SUBTITLE_FONTTYPE_CHANGE,
	eHMSG_MAINAPP_READY,
	eHMSG_MAINLANG_CHANGED,
	eHMSG_INPUT_NOTIFY,
	eHMSG_PANEL_PRIORITY,
	eHMSG_USER_ACTION_IDLE_NOTIFY,
	eHMSG_IS_OTA_CHECK,
	eHMSG_MSGBOX_SHOW,
	eHMSG_BLUETOOTH_MUTE,
	eHMSG_FINGERPRINT_STATE
} HAPIMSG_e;

typedef	struct _ONDK_Size
{
	HINT32		w;
	HINT32		h;
} ONDK_Size_t;

typedef	struct _ONDK_Point
{
	HINT32		x;
	HINT32		y;
} ONDK_Point_t;

typedef	struct _ONDK_Rect
{
	HINT32		x;
	HINT32		y;
	HINT32		w;
	HINT32		h;
} ONDK_Rect_t;

typedef struct _ONDK_Region
{
	HINT32		x1;
	HINT32		y1;
	HINT32		x2;
	HINT32		y2;
} ONDK_Region_t;

/*
 * GFX
 */
typedef struct _ONDK_GFX
{
	ONDKWindowHandle 	window;
	ONDKSurfaceHandle	surface;
} ONDK_GFX_t;


/*
 * Timer
 */
typedef	enum
{
	eTimer_ONCE		= 0,
	eTimer_REPEAT
} ONDK_TimerType_t;

typedef	struct
{
	HUINT32				ulTimeout;
	HUINT32				ulStartTime;
	ONDK_TimerType_t	eTimerMode;
	void				(*cbTimeoutExec)(HUINT32);
} ONDK_TIMER_INFO_t;

/*
 * op
 */


#ifndef SWAP
#define SWAP(a,b) { a ^= b; b ^= a; a ^= b; }
#endif

#ifdef CONFIG_PROD_ENDIAN_LITTLE
#define	COL_A(_color)	((_color >> 24) & 0xFF)
#define	COL_R(_color)	((_color >> 16) & 0xFF)
#define	COL_G(_color)	((_color >>  8) & 0xFF)
#define	COL_B(_color)	((_color >>  0) & 0xFF)


#else
#define	COL_A(_color)	((_color >>  0) & 0xFF)
#define	COL_R(_color)	((_color >>  8) & 0xFF)
#define	COL_G(_color)	((_color >> 16) & 0xFF)
#define	COL_B(_color)	((_color >> 24) & 0xFF)

#endif

#define ONDK_TASK_STACK_SIZE			SIZE_16K
#define ONDK_TASK_PRIORITY				VK_TASK_PRIORITY_MW_REF
#define ONDK_LANGUAGE_STRING_MAX_LENGTH 128
#define ONDK_FULLPATH_MAX_LENGTH 		MAX_HXDIR_FILENAME_LENGTH
#define ONDK_FILENAME_MAX_LENGTH 		256//(MAX_HXDIR_FILENAME_LENGTH/2)
#define ONDK_FILEEXT_MAX_LENGTH 		32//(MAX_HXDIR_FILENAME_LENGTH/16)





#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

#ifndef SIGN
#define SIGN(x)  (((x) < 0) ?  -1  :  (((x) > 0) ? 1 : 0))
#endif

#ifndef ABS
#define ABS(x)   ((x) > 0 ? (x) : -(x))
#endif

#ifndef CLAMP
#define CLAMP(x,min,max) ((x) < (min) ? (min) : (x) > (max) ? (max) : (x))
#endif

#ifndef BSWAP16
#define BSWAP16(x) (((u16)(x)>>8) | ((u16)(x)<<8))
#endif

#ifndef BSWAP32
#define BSWAP32(x) ((((u32)(x)>>24) & 0x000000ff) | (((u32)(x)>> 8) & 0x0000ff00) | \
                    (((u32)(x)<< 8) & 0x00ff0000) | (((u32)(x)<<24) & 0xff000000))
#endif

#undef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))

#undef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))


typedef enum
{
    eKeyPress_DOWN,
    eKeyPress_UP,
    eKeyPress_REPEAT,

	eKeyPress_MAX
} ONDK_KEY_PressMode_t;

typedef enum
{
	eKeyDevice_RCU,
	eKeyDevice_FRONT,
	eKeyDevice_BROWSER	/* for WEB_BROWSER */
} ONDK_KEY_Device_t;

typedef enum
{
	eKeyInput_KEYBOARD=0x0000,
	eKeyInput_MOUSE,
	eKeyInput_JOYSTICK,
	eKeyInput_RCU,
	eKeyInput_ANY = 0x0010,
} ONDK_KEY_InputDevice_t;

typedef enum
{
	ONDK_EMPTY_WINDOW = 0,

	ONDK_UPPER_WINDOW = 1,
	ONDK_LOWER_WINDOW,
	/* add area*/

	ONDK_MAX_WINDOW,
}ONDK_WINDOW_Position_e;


typedef ONDK_Result_t	(*ONDK_KEY_KeyNotifyCallback_t)(ONDK_WINDOW_Position_e ePosition,HUINT32 ucKeyCode, ONDK_KEY_PressMode_t pressMode, ONDK_KEY_Device_t device, void *keyTypeData);




//typedef	HUINT32         Handle_t;

/** @brief bitmap blending mode */
typedef enum
{
	eBmpBlend_COPY				= 0,			/* copy the source maintaing its alpha */
	eBmpBlend_Ignore_SrcAlpha					/* don't care the source's alpha, so reflect the background */
} OSD_BmpBlendMode_t;


#endif





