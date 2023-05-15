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
#ifndef	__PRISM_MAIN_H__
#define	__PRISM_MAIN_H__

/**************************************************************************************************/
#define _________PRISM_Internal_Include_________________________________________________
/**************************************************************************************************/
#include 	<ondk.h>
#include	<oapi.h>
#include	<hapi.h>
#include	<papi.h>
#include	<apk.h>
#include 	<teletext.h>
#include  <subtitle.h>

#if defined(CONFIG_DROP_ROOT_PRIVILEGES)//1 To Do 관련 API를 HAPI에 생성후 삭제 예정
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/resource.h>
#endif

/**************************************************************************************************/
#define _________PRISM_Internal_Macro_________________________________________________
/**************************************************************************************************/
//#define SCREENSAVER_TEST
//#define TTX_EXIT_SIGNAL
#define	PRISM_DB_LASTSERVICE		"LastMasterUid"
#define	SECONDS									1000
#define	MINUTES									(60 * SECONDS)

#if defined(SCREENSAVER_TEST)
#define	SCREENSAVER_OPERATION_LIVE_TIMEOUT		(SECONDS *5)
#define	SCREENSAVER_OPERATION_MEDIA_TIMEOUT		(SECONDS *3)
#else
#define	SCREENSAVER_OPERATION_LIVE_TIMEOUT		(5 * MINUTES)  // live
#define	SCREENSAVER_OPERATION_MEDIA_TIMEOUT		(3 * MINUTES)  // raido & music
#endif

#undef		COMMAND_NAME
#define		COMMAND_NAME						"APP_GWMPOPUP"

#define	AP_Malloc(size)				HLIB_MEM_Malloc(size)
#define	AP_Calloc(size)				HLIB_MEM_Calloc(size)
#define	AP_Realloc(mem,size)		HLIB_MEM_Realloc(mem, size)
#define	AP_StrDup(str)				HLIB_MEM_StrDup(str)
#define	AP_MemDup(mem,size)			HLIB_MEM_MemDup(mem, size)
#define	AP_Free(mem)				HLIB_MEM_Free(mem)


	/**************************************************************************************************/
	#define _________PRISM_Internal_typedef_________________________________________________
	/**************************************************************************************************/
typedef	enum
{
	GWMPOPUP_TIMER_MAIN =0x001,
	GWMPOPUP_TIMER_KEYBOARD,
	GWMPOPUP_TIMER_KEYBOARD_PW,
	GWMPOPUP_TIMER_KEYBOARD_MOVE,
	GWMPOPUP_TIMER_SIMPLEKEYBOARD,
	GWMPOPUP_TIMER_SCREENSAVER,
	GWMPOPUP_TIMER_SCREENSAVER_GETSERVIE,
	GWMPOPUP_TIMER_VOLID,
	GWMPOPUP_TIMER_STARTLOGOID,
GWMPOPUP_TIMER_STARTLOGO_LOADING,		// 사용자 Input이 들어왔을때우측 상단에 Web이 Loading중이라고 표시해주는 UI 를 위한 타이머
	GWMPOPUP_TIMER_STARTLOGO_SYSMSG,		// Web이 뜨기 전까지 Prism에서 System Message를 띄우기 위한 타이머
	GWMPOPUP_TIMER_TTXMENU,
	GWMPOPUP_TIMER_FRONT,
	GWMPOPUP_TIMER_FRONT_FADE,
	GWMPOPUP_TIMER_MSGBOX,
#if defined(CONFIG_CAS_CONAX_FINGERPRINT)
	GWMPOPUP_TIMER_FINGERPRINT,
	GWMPOPUP_TIMER_FINGERPRINT_DURATION,
#endif
	GWMPOPUP_TIMER_KEYBOARD_INPUT_INTERVAL,
	GWMPOPUP_TIMER_MAX
} GWMPOPUP_TIMER_e;

typedef enum
{
	eMODE_LIVE,
	eMODE_RADIO,
	eMODE_MUSIC,
} ScreenSaver_Mode_e;


/**************************************************************************************************/
#define _________PRISM_Internal_Value_________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________PRISM_Internal_Functions__________________________________________________________
/**************************************************************************************************/

extern ONDK_Result_t		AP_AudioVolume_Proc(HINT32 nMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern HBOOL 			AP_AuidoVolume_GetHideState(void);
extern ONDK_Result_t		AP_HDMI_Proc(HINT32 lMessage, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t		AP_KeyboardMain_Proc(HINT32 nMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t		AP_Radio_Proc(HINT32 nMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t		AP_Startlogo_Proc(HINT32 lMessage, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t		AP_Screensaver_Proc(HINT32 lMessage, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3);
#if !defined(CONFIG_APUI_NATIVE_APP)
extern ONDK_Result_t		AP_SimpleKeyboardMain_Proc(HINT32 nMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
#endif
extern ONDK_Result_t		AP_Front_Proc(HINT32 lMessage, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t		AP_MSGBOX_Proc(HINT32 lMessage, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3);
#if defined(CONFIG_CAS_CONAX_FINGERPRINT)
extern ONDK_Result_t		AP_Fingerprint_Proc(HINT32 lMessage, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3);
#endif
#if defined(CONFIG_APWEB_NATIVE_MEM_INFO)
extern ONDK_Result_t		AP_MemoryInfo_Proc(HINT32 lMessage, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3);
#endif

#endif /* !__PRISM_MAIN_H__ */

