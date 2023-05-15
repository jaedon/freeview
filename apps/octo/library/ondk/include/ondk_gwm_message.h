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

#ifndef	__MESSAGE_H__
#define	__MESSAGE_H__



typedef enum
{
	MSG_MODULE_MIN_MAGICNUM = 0x00000991,

	MSG_GWM_START 			= 0x00010000,	/* GWM service message */
	MSG_SYSINFO_START		= 0x00020000,

#if defined(CONFIG_APUI_NATIVE_APP)
	MSG_AP_CORE_START		= 0x00030000, 	/* Core application message */
	MSG_AP_CORE2UI_START	= 0x00040000, 	/* Core->UI App Creation/Destruction message */
	MSG_AP_UI_START			= 0x00050000, 	/* UI application message */
#endif

	MSG_MODULE_MAX_MAGICNUM = 0x20110307,
} MSG_MODULE_START_NUM_e;

/* ======================================================================== */
/* Exported types. 															*/
/* ======================================================================== */

typedef enum
{
	GWM_APK_START =0,
	GWM_APK_MEDIA ,
	GWM_APK_BOX,
	GWM_APK_BLUETOOTH,
	GWM_APK_HDMI,
	GWM_APK_SQC_SCENARIO,
	GWM_APK_SQC_AUDIO,
	GWM_APK_SYSINFO,
	GWM_APK_POWER,
	GWM_APK_THERMAL,
	GWM_APK_NETWORK,
	GWM_APK_CAS_MESSAGE_EVENT,
	GWM_APK_CAS_UI_TRIGGERED,
	GWM_APK_CAS_SCSTATE_EVENT,
	GWM_APK_DLNA_DMP,
	GWM_APK_MAX
}GWM_APK_MSG_Type_e;


/* 각 메세지들에 대해서, 메세지에 부가적으로 따라오는 Param 값들의 의미를 설명해 줄 필요가 있다. */
typedef	enum
{
/* GWM Object 공통 */
	MSG_GWM_CREATE    = MSG_GWM_START,
	MSG_GWM_DESTROY,

/* Parameter Usage:
	p1 : spawned callback function
	p2 : parent callback function	*/
	MSG_GWM_APP_SPAWNED,				// Application이 Create 되었음을 다른 모든 App에 알리는 메시지
/* Parameter Usage:
	p1 : callback function */
	MSG_GWM_APP_KILLED,					// Application이 Kill 되었음을 다른 모든 App에 알리는 메시지

	MSG_GWM_DRAW,
	MSG_GWM_CLICKED,

/* Parameter Usage:
	p1 : key code(HUINT8)
	p2 : key device(KEY_Device_t)
	p3 : repeat flag(HBOOL) */
	MSG_GWM_KEYUP,
	MSG_GWM_KEYDOWN,
 	MSG_GWM_KEY_PRESSED,	//used??

/* String Inputs from other mothod like mobile app
	p1 : string pointer */
	MSG_GWM_KEYQWERTY,

/* Parameter Usage:
	p1 : Timer ID
	p2 : N.A.
	p3 : N.A. */
	MSG_GWM_TIMER,

/* Parameter Usage:
	p1 : N.A.
	p2 : N.A.
	p3 : N.A. */
	MSG_GWM_FAIL_RECREATE_NEW_INSTANCE,		/* 동일한 app callback을 create하려고 할 때 실패한 후 생성하려던 callback에게 new instance를 생성하려는 시도가 있었음을 알리는 메세지 */

	MSG_GWM_SCROLLLEFT,
	MSG_GWM_SCROLLRIGHT,
	MSG_GWM_SCROLLUP,
	MSG_GWM_SCROLLDOWN,				// = 0xa
	// 여기까지는 기본적으로 필요한 것들.

	MSG_GWM_BUTTON_RIGHT,
	MSG_GWM_BUTTON_CLICKED,
	MSG_GWM_SELECTOR_CHANGED,
	MSG_GWM_SELECTOR_LEFT_NOMORE,
	MSG_GWM_SELECTOR_RIGHT_NOMORE,
	MSG_GWM_SELECTOR_CLICKED,		//=0x10
	MSG_GWM_SPINCONTROL_CHANGED,		// Param1 = object ID, Param2 = spin-Control의 변경된 값, Param3 = 변경되는 값의 방향.(증가=1, 감소=-1, 그외=0)
	MSG_GWM_SPINCONTROL_BIGGEST,		// Param1 = object ID, Param2 = spin-Control의 범위 최대값, Param3 = 0
	MSG_GWM_SPINCONTROL_SMALLEST,		// Param1 = object ID, Param2 = spin-Control의 범위 최소값, Param3 = 0
	MSG_GWM_SPINCONTROL_COMPLETED,		// Param1 = object ID, Param2 = spin-Control의 현재 값, Param3 = 0
	MSG_GWM_INPUT_LEFT_NOMORE,
	MSG_GWM_INPUT_RIGHT_NOMORE,
	MSG_GWM_INPUTCOMPLETE,
	MSG_GWM_INPUTCANCELD,
	MSG_GWM_LIST_CHANGED,
	MSG_GWM_LIST_UP_NOMORE,			//=0x1c
	MSG_GWM_LIST_DOWN_NOMORE,
	MSG_GWM_LIST_SELECTED,

	MSG_GWM_FOCUS_GET,					// p1 = object ID which getting focus
	MSG_GWM_FOCUS_LOSE,					// =0x20 p1 = object ID which lost focus

	MSG_GWM_LIST_RIGHT_CHANGED,	//horizontal list에서 사용
	MSG_GWM_LIST_LEFT_CHANGED,

	MSG_GWM_OBJ_FOCUS_CHANGED,

	MSG_GWM_STACK_CHANGED,				/* Application stack order changed. */

	MSG_GWM_QUIT_UI,					/* =0x23 Destroy UI menus. If application doesn't block this message, GWM_ProcessMessageDefault() will destroy 'this' proc. */

	/* - Description : Application들에게 standby로 간다고 알림. If application doesn't block this message, GWM_ProcessMessageDefault() will destroy 'this' proc.
	   - Parameters Usage :
		   handle : N.A.
		   p1 : TRUE: Block shutdown process and stay calm.
		   p2 : KEY_Device_t. RCU or Front key.
		   p3 : TBD */
	MSG_GWM_GO_STANDBY,
	MSG_GWM_QUIT_STANDBY,				/* Standby 에서 벗어나 Normal로 진행한다고 알림. Standby appl들이 동작을 중지하는 용도로 사용한다.
											Standby appl이 많지 않기 때문에 MSG_GWM_GO_STANDBY와는 다르게 default proc에서 처리하지 않는다. */
	MSG_GWM_TAB_CHANGED,
	MSG_GWM_TAB_RIGHT_NOMORE,
	MSG_GWM_TAB_LEFT_NOMORE,

	MSG_GWM_GAUGEBAR_CHANGED,
	MSG_GWM_GAUGEBAR_LEFT_NOMORE,
	MSG_GWM_GAUGEBAR_RIGHT_NOMORE,


	MSG_GWM_O2ANIMATION_UPDATE,
	MSG_GWM_O2ANIMATION_END,

	MSG_GWM_NOT_AVAILABLE,



	/* add to message */

	/*
	 * handle : NULL
	 * p1 : HSIGNAL_e
	 * p2 : (ONDK_HAPI_Msg_t *)
	 * p3 :
	 * 주의 : 이 메시지는 3개가 loop 돌면서 사용된다. message 수신시 복사하여 사용하세요.
	 */
	MSG_GWM_HAPI_MESSAGE,				//	hAction : NULL, ulParam1 : GWMHAPI_t



	MSG_GWM_APK_MESSAGE,

	MSG_GWM_APK_INITIALIZED,			//	APPKIT INIT message
	MSG_GWM_APK_BLUETOOTH,
	MSG_GWM_APK_KEYBOARD,
   	MSG_GWM_APK_SYSINFO,
    MSG_GWM_INVALIDATE_RECT,


	MSG_GWM_MAX,

/*+++++++++++++++++++++++++++++++++ HAPI RECEIVED SIGNAL ++++++++++++++++++++++++++*/

    MSG_SIGNAL_START ,

    MSG_SIGNAL_REQUESTSTANDBY,
    MSG_SIGNAL_SHOWSUBTITLE,
    MSG_SIGNAL_SHOWSUBTITLELIVE,
    MSG_SIGNAL_VARIABLEFONT,
    MSG_SIGNAL_COUNTRYID,
    MSG_SIGNAL_REMOTESTING,

    MSG_SIGNAL_FINGERPRINT,

    MSG_SIGNAL_END,


    MSG_APP_START ,
    /*++++++++++++++++++++++++ Subtitle/Teletext Events +++++++++++++++++++++++++*/
	/* - Description : Hapi Msg 가 아닌, Msg 로 teletext Proc 을 시작 시키기 위한 msg.
	   - Parameters Usage :
		   handle : Action handle
		   p1 : bShow
		   p2 : TBD
		   p3 : TBD */
	MSG_APP_START_TELETEXT,

	/* - Description : Teletext를 시작하겠다는 예고 이벤트.
					Teletext를 시작하기 전에 subtitle을 off시키기 위하여 broadcast해야 함.
	   - Parameters Usage :
		   handle : Action handle
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	MSG_APP_TELETEXT_STARTED,

	/* - Description : Teletext를 중단했다는 notice 이벤트.
					Teletext를 중단한 다음 subtitle등을 restart하기 위하여 broadcast해야 함.
	   - Parameters Usage :
		   handle : Action handle
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	MSG_APP_TELETEXT_STOPPED,
    /* - Description : Teletext SHOW 실패 ...
           - Parameters Usage :
               handle : Action handle
               p1 : TBD
               p2 : TBD
               p3 : TBD */
    MSG_APP_TELETEXT_SHOWFAIL,

	/* - Description : Subtitle 시작하거나 변경이 생겼으므로 WTV또는 다른 action들에게 반영하라는 통보 이벤트.
	   - Parameters Usage :
		   handle : Action handle
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	MSG_APP_START_SUBTITLE,

	/* - Description : MSG_APP_START_SUBTITLE과 같으나, DVB/EBU 의 각 Page ID를 설정함.
	   - Parameters Usage :
		   handle : Action handle
		   p1 : Subtitle 종류 (eSI_EBU_SUBTITLE or eSI_DVB_SUBTITLE)
		   p2 : DVB : Manazine number, EBU : C Page ID
		   p3 : DVB : Page Number, EBU : A Page ID */
	MSG_APP_START_SUBTITLE_WITH_PAGEID,

	/* - Description : Subtitle off하라고 WTV또는 다른 action들에게 반영하라는 통보 이벤트.
	   - Parameters Usage :
		   handle : Action handle
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */

	MSG_APP_STOP_SUBTITLE,

	MSG_APP_RCV_SUBTITLE_PACKET,	// received subtitle delivery packet.

	MSG_APP_RCV_TELETEXT_PACKET,	// received teletext delivery packet.

	MSG_APP_EXECUTE_CALLBACK,

	/*
	GWM_APK_HDMI 에서 HDCP 부분의 상태 변화를 전송한다.
	*/
	MGS_APP_HDCP_STATUS,

	/*
	HAPI_SetSysMonitor / HAPI_SysMonitorListener ->save USB
	*/
	MSG_APP_SAVE_SYSTEM_STAUS,


    MSG_GWM_END
} GWM_Message_t;


#endif	//__MESSAGE_H__
/* End of File. ---------------------------------------------------------- */

