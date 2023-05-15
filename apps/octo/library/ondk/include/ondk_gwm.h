/**************************************************************
 *	@file		ondk_gwm.h
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


#ifndef	__GWM_H__
#define	__GWM_H__

/* ======================================================================== */
/* Include.					기본적인 System Include 						*/
/* ======================================================================== */
#include <ondk.h>

/* 모든 파일에 공통으로 include 해야 할 것들은 octo_common.h 에 추가할 것.	*/
#include <ondk_gwm_config.h>

#ifdef CONFIG_DEBUG
	#define	DBGMSG_ERROR	0x01
	#define	DBGMSG_WARNING	0x02
	#define	DBGMSG_TRACE	0x04
	#define	DBGMSG_APP		0x08
	#define	DBGMSG_OBJ		0x10
	#define	DBGMSG_PRINT	0x20

	#define	GWM_assert(c)					HxLOG_Assert(c)
	#define	GWM_DBG_Print(level, message)	{ \
												if(level & DBGMSG_ERROR) 	HxLOG_Error 	message; \
												if(level & DBGMSG_WARNING) 	HxLOG_Warning	message; \
												if(level & DBGMSG_TRACE) 	HxLOG_Print		message; \
												if(level & DBGMSG_OBJ)	 	HxLOG_Print 	message; \
												if(level & DBGMSG_APP)	 	HxLOG_Print 	message; \
												if(level & DBGMSG_PRINT) 	HxLOG_Print 	message; \
											}
#else
	#define	GWM_assert(c)
	#define	GWM_DBG_Print(level, message)
#endif

/* ======================================================================== */
/* Exported Macros. 														*/
/* ======================================================================== */

#define	GWM_DEFAULT_PRIORITY			100
#define GWM_MAX_PRIORITY				0xFF			// 255
#define GWM_MASK_PRIORITY				0x000000FF


typedef enum _ONDK_GWM_Priority
{
	APP_BGIMAGE_PRIORITY = 1,

	APP_PVR_MGR_PRIORITY,
	APP_ACTION_PRIORITY,
	APP_PIP_PRIORITY,
	APP_UIMGR_PRIORITY,
	APP_MENUROOT_PRIORITY,

	APP_RSV_MGR_PRIORITY,
	APP_USB_MGR_PRIORITY,
	APP_CEC_MGR_PRIORITY,
	APP_CAS_PRIORITY,
	APP_CAS_PPV_PRIORITY,
	APP_TELETEXT_PRIOTITY,
	APP_OSD_DRAW_SBTL_TTX_PRIOTITY,
	APP_PVR_PRIORITY,
	APP_BML_MGR_PRIORITY,
	APP_HTML_MGR_PRIORITY,
	APP_INFOHUB_PRIORITY,
	APP_PASSWORD_PRIORITY,							/* must APP_UIMGR_PRIORITY < APP_PASSWORD_PRIORITY < APP_BACKGROUND_OSD_PRIORITY */
	APP_BACKGROUND_OSD_PRIORITY,					/* APP_DEFAULT_PRIORITY 바로 전에 위치 해야 함. Menu/EPG 등 full OSD의 background drawing proc용 */

	APP_DEFAULT_PRIORITY = GWM_DEFAULT_PRIORITY,

	APP_DEFAULT_POPUP_PRIORITY,
	APP_DIALOGBOX_PRIORITY,				// 각종 Dialog Box 표시를 위해 DEFAULT 보다 높은 Priority.
	APP_ANTENNA_FAULT_PRIORITY,
	APP_MENUMGR_PRIORITY,				// Menu Manager Priority : Menu App보다도 우선적으로 메시지를 먼저 받아야 한다.
	APP_NOT_AVAILABLE_MSG_PRIORITY,
	APP_FRONTKEY_MONITOR_PRIORITY,
	APP_CAS_HIGH_PRIORITY,

	APP_WEB_PORTAL_MGR_PRIORITY,
	APP_WATCHING_RSVM_POPUP_PRIORITY,
	APP_INDEPENDENT_OSD_GRP_PRIORITY,	//Independent OSD Group에 해당하는 Proiority (CPP Spec.)
										//(Volume control, Mute icon, Input of channel number, Resolution, Sleep, Wide, Not available icon)
	APP_DVD_MGR_PRIORITY,
	APP_BLURAY_UIMGR_PRIORITY,			// Mode 전환시 , 모든 RCU Key 메시지를 막아야 하므로 Priority가 매우 높다.
	APP_HOTELMODE_KEYBLOCK_PRIORITY,
	APP_MULTI_CUSTOMCODE_PRIORITY,
	APP_SCREEN_SAVER_PRIORITY,
	APP_AUTOPOWERDOWN_PRIORITY,

	APP_FAN_PRIORITY, //죽기전에 사용되어야 하므로  priority 가 가장 높아야 한다.

	APP_DBG_MSGHOOK_PRIORITY, //Msg hook용이다. 다른곳에서 절대 사용하지 않기를...

	// APP Priority 는 GWM_MAX_PRIORITY 보다 많을 수 없다.
	APP_MAX_PRIORITY = GWM_MAX_PRIORITY
} ONDK_GWM_Priority_t;



/*
	GWM UI Status Values
*/
#define GWM_MASK_UISTATE		0x0000FF00

typedef enum
{
	UISTATE_NORMAL				= 0,
	UISTATE_MENU				= 0x00001000,
	UISTATE_EPG					= 0x00002000,
	UISTATE_RECORDINGS			= 0x00003000,
	UISTATE_MEDIA				= 0x00004000,
	UISTATE_IPLATE				= 0x00005000,
	UISTATE_INSTALLWIZARD		= 0x00006000,
	UISTATE_SCHEDULE			= 0x00007000,
	UISTATE_SHAHID				= 0x00008000,

	UISTATE_INHERITANCE			= 0x0000FE00,				// 부모로부터 상속. 실제 App 내에서는 존재하지 않는 값
	GWM_MAX_UISTATE				= 0x0000FF00
} GWM_UiState_t;


#define	DEFAULT_DIMMED_LEVEL			80


#define	CHECK_ERROR(x)					( ((x)==GWM_RET_OK)?(0):(HxLOG_Print("Error(%X) on %s(line:%d)\n", (x), __FILE__, __LINE__)) )
#define	GWM_GetFocusedObjectID()		ONDK_GWM_Obj_GetObjectID(ONDK_GWM_Obj_GetFocusedObject())


#define	ANIMGUI_TICK_MARGIN				15

/* 총 프레임 수 & the last frame index */
#define	ANIMGUI_FRAME_COUNT				20	/* cut 수 */
#define ANIMGUI_LAST_FRAME_INDEX		(ANIMGUI_FRAME_COUNT - 1)

/* 총 드로잉 시간  */
#define	ANIMGUI_FRAME_DURATION			1000	/* tick */

/* frame per second */
#define	ANIMGUI_FPS						(ANIMGUI_FRAME_COUNT / ANIMGUI_FRAME_DURATION) /* -_- round */

/* second per frame */
#define	ANIMGUI_SPF						(ANIMGUI_FRAME_DURATION / ANIMGUI_FRAME_COUNT)

/* UI stock image */
#define STOCK_SCREEN_WIDTH				GWM_SCREEN_WIDTH
#define STOCK_SCREEN_HEIGHT				GWM_SCREEN_HEIGHT

#define GWM_MSGQ_SIZE					256
#define GWM_TIMERTASK_PRIORITY			40
#define GWM_TIMERTASK_STACK_SIZE		SIZE_4K
#define GWM_TASK_PRIORITY				40
#define GWM_TASK_STACK_SIZE				SIZE_256K
#define GWM_TIMER_TASK_NAME				"tGwmTimer"

/* ------------------------------------------------------------------------ */
/*	Service header files include.											*/
/* ------------------------------------------------------------------------ */


/* ------------------------------------------------------------ */
/*	Structure type definition.									*/
/* ------------------------------------------------------------ */


// Scroll Definition
typedef enum
{
	SCROLL_UP,
	SCROLL_DOWN,
	SCROLL_LEFT,
	SCROLL_RIGHT
} GWM_ScrollDir_t;

/* hapi message type */
typedef struct _ONDK_HAPI_Msg
{
	/*
	 * header끼리 종속성 없애기 위해 HINT32로 선언
	 */
	HINT32	 		eSignal;	 /* HSIGNAL_e */
	HINT32		 	eSignalType; /* HSIGNALTYPE_e*/
	HINT32 			size;
	void 			*data;
} ONDK_HAPI_Msg_t;

typedef struct	tagObjectCreateStructure
{
	void*					pfnProc;						// cast GWM_Proc_t
	void*					pfnObjDrawMethod;			// cast GWM_ObjDraw_t
	void*					pfnObjUserMethod1;			// cast GWM_User_t
	void*					pfnObjUserMethod2;			// cast GWM_User_t
	void*					pfnObjUserMethod3;			// cast GWM_User_t
	void*					pfnObjUserMethod4;			// cast GWM_User_t

	HUINT16					usSelectedIndex;			// index of selected item
	HUINT16 				usItemsPerPage;			// display items(items per a page)

	ONDK_Rect_t				stFrameRect;
	ONDK_Rect_t				stMarginRect;

	ONDK_Rect_t				stItemRect;
	ONDK_Rect_t				stItemMargin;
	ONDK_Rect_t				stItemRect1;
	ONDK_Rect_t				stItemMargin1;
	ONDK_Rect_t				stItemRect2;
	ONDK_Rect_t				stItemMargin2;
}OBJ_CREATE_t;

typedef enum tagONDK_GWM_OBJ_Z_Order{
	eGWMObj_Z_Ord_Frame			= 0,
	eGWMObj_Z_Ord_BGImage		= 0x10,
	eGWMObj_Z_Ord_Default		= 0x100,
	eGWMObj_Z_Ord_ConfirmButton	= 0x10000,
	eGWMObj_Z_Ord_Text			= 0x100000,
	eGWMObj_Z_Ord_MAX			= 0xFFFFFFFF
} ONDK_GWM_OBJ_Z_Order_t;

/*	application의 속성으로 UI를 가지고 있는지를 나타내는 속성
	현재는 UI를 가지고 있는가에 대한 속성만 있으나
	필요한 다른 속성도 추가해 활용해 볼 수 있을 것 같다. wjmoh
*/
typedef enum tagONDK_GWM_AppAttribute {
	GWM_Basic_App			= 0x00,		/**< UI가 없는 기본 어플이다. */
	GWM_FakeUI_App		= 0x01,		/**< UI가 없는 어플인데 UI가 있는 척 UI stack count를 가진다. */
	GWM_HiddenUI_App		= 0x02,		/**< UI가 있는 어플인데 UI가 없는 척 행동하게 한다. */
	GWM_RealUI_App		= 0x04,		/**< UI가 있는 어플이다. */

	GWM_App_Attr_MAX
} ONDK_GWM_AppAttribute_e;

/* show/hide를 시키는 주체로 자기 자신(Self)과 다른 어플(the others)이 있다.
	이때 절대 Hide될 수 없는 속성과 Hide되더라도 자신과 남의 상관관계를 지정할 수 있다.

	GWM_UI_Hide_Never -> 자기 자신이나 남들에 의해 절대 Hide되지 않는다.

	GWM_UI_Hide_Only_Self -> 자신 이외는 절대 Hide 시킬 수 없다. (active flag)
	GWM_UI_Hide_Only_Others -> 자신은 절대 Hide 시킬 수 없다. 남들만 가능하다. (passive flag)
	GWM_UI_Hide_Default -> 평번하게 자기 자신 혹은 남들에 의해 Hide될 수 있다.
							위 두 flag을 or하면 이 flag이 된다. 혹은 그냥 지정해도 된다.

	wjmoh
*/
typedef enum tag_gwm_app_ui_hide {
	GWM_UI_Hide_Default 		= 0x0000,
#if 0
	GWM_UI_Hide_Able			= 0x0001,
#else
	GWM_UI_Hide_Only_Self		= 0x0001,
	GWM_UI_Hide_Only_Others	= 0x0002,
	GWM_UI_Hide_Never		= 0x0003,
#endif
	GWM_UI_Hide_Flag_Max
} ONDK_GWM_AppUiHide_e;


typedef	struct	tagGWMSystemColor
{
	ONDK_Color_t	background;									// 100% 투명한 색.

	ONDK_Color_t	btn_hilight, btn_light, btn_face,			// 객체(Button)의 기본색상.
					btn_shadow, btn_darkshadow;

	ONDK_Color_t	focus_hilight, focus_light, focus_face, 	// Focus를 얻은 객체의 색상.
					focus_shadow, focus_darkshadow;

	ONDK_Color_t	disabled_hilight, disabled_light, 			// Focus가 불가능한 객체의 색상.
					disabled_face, disabled_shadow, disabled_darkshadow;

	ONDK_Color_t	frame_hilight, frame_light, frame_face, 	// 윈도우 바탕이 되는 Frame의 기본 색상.
					frame_shadow, frame_darkshadow,	frame_caption_bar,
					frame_caption_text, frame_message;

	ONDK_Color_t	inactive_hilight, inactive_light, 			// Disabled 와 별도로 활성화 되지 않는 객체(Frame, Text message, etc.)의 색상.
					inactive_face, inactive_shadow, inactive_darkshadow,
					inactive_caption_bar, inactive_caption_text, inactive_message;

	ONDK_Color_t	default_text, hilighted_text, disabled_text,// 기본 Text의  색상.
					info_text;

	ONDK_Color_t	red, green, yellow, blue;					// 주로 사용되는 기본 4색.

} GWM_SystemColor_t;


/* ------------------------------------------------------------------------ */
/*	Enumulation.																*/
/*			외부에 별도의 파일을 만들어 gwm.h의 직접적인 수정을 방지한다.				*/
/* ------------------------------------------------------------------------ */

#include <ondk_gwm_message.h>
#include <ondk_gwm_guiobj.h>

/* ************************************************************************ */
typedef	struct	tagApplicationObject	*ONDK_GWM_App_t;
typedef	struct	tagGuiObject 			*ONDK_GWM_GuiObject_t;


/* GWM application procedure */
typedef ONDK_Result_t		(*ONDK_GWM_Callback_t)(HINT32 message, Handle_t handle,
										HINT32 param1, HINT32 param2, HINT32 param3);

/* GWM object event procedure */
typedef	ONDK_Result_t		(*GWM_Proc_t)(GWM_Obj_MethodType_t proc_msg,
										ONDK_GWM_GuiObject_t object, int arg);


/* GWM object user specific data set callback */
typedef	ONDK_Result_t		(*GWM_User_t)(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3, HUINT32 lParam4);


/* Customized GWM object drawing procedure */
/* arg will usually be used to access object's own data member */
/* param shows that parent app is the front-most or under-covered. */
typedef ONDK_Result_t		(*GWM_ObjDraw_t)(ONDKSurfaceHandle screen, ONDK_Region_t *area, ONDK_Rect_t *rect,
											ONDK_GWM_GuiObject_t object, void *arg, int param1);

/* Customized Sprite Drawing procedure */
typedef ONDK_Result_t		(*GWM_SpriteDraw_t)(ONDKSurfaceHandle screen, ONDK_Rect_t stRect, void *pvArgs);

/* Sprite Argument Free Procedure */
typedef void				    (*GWM_SpriteFreeArg_t)(void *pvArgs);


/* ************************************************************************ */
/*
 * O2 Animation
 */

typedef enum
{
	eONDK_O2ANITYPE_None	= 0x0000,
	eONDK_O2ANITYPE_User	= 0x0001,
	eONDK_O2ANITYPE_MoveTo	= 0x0002,
	eONDK_O2ANITYPE_Fade	= 0x0004,
	eONDK_O2ANITYPE_Proc	= 0x0008,
	eONDK_O2ANITYPE_ExitProc= 0x0010,
}ONDK_GWM_O2Ani_Type_e;

typedef struct tagONDK_GWM_O2Ani_Settings
{
	HINT32			nAniType;
	ONDK_Rect_t		rtStart;
	ONDK_Rect_t		rtEnd;
	HINT32			nSpeed;
	HINT32			nStartAlpha;
	HINT32			nEndAlpha;
	HBOOL			bNotify;

	// O2ANITYPE_User
	HINT32			nUserEndVal;
	HINT32			nUserInterval;

	// O2ANITYPE_Proc
	HBOOL			bCompletedUiDraw;
	ONDK_Rect_t		drawProcRect;

	// O2ANITYPE_Fade

} ONDK_GWM_O2Ani_Settings_t;

typedef struct	tagGWM_O2Ani_Item_t
{
	HINT32			nAniType;
	HINT32			nAniId;
	ONDK_GWM_App_t	pApp;

	double			x,y,w,h, a;
	HINT32			nCurAlpha;
	ONDK_Rect_t 	rtCurrent;
	HINT32			nLastKey;
	UNIXTIME		utStartTime;
	ONDK_WINDOW_Position_e ePosition;

	ONDK_GWM_O2Ani_Settings_t stSettings;
}ONDK_GWM_O2Ani_Item_t;

#define DEF_O2ANI_EFFECT_IGNORE_KEYPROCESS 0xFF


/* ------------------------------------------------------------ */
/*	Global variables for GWM.									*/
/* ------------------------------------------------------------ */
		/* Application Management */
extern	ONDKSurfaceHandle		ONDK_GWM_GetSurface(ONDK_WINDOW_Position_e ePositon);
extern 	ONDKWindowHandle 		ONDK_GWM_GetWindow(ONDK_WINDOW_Position_e ePositon);
extern	ONDK_WINDOW_Position_e 	ONDK_GWM_GetCurPosition(void);
extern	ONDKSurfaceHandle	 	ONDK_GWM_GetCurSurface(void);
extern	ONDKSurfaceHandle 		ONDK_GWM_GetProcAniSurface(void);



extern	GWM_SystemColor_t	g_stSystemColor;


/* ------------------------------------------------------------ */
/*	Functions .													*/
/* ------------------------------------------------------------ */
extern  ONDK_Result_t		ONDK_GWM_Init(void);
extern	void 				ONDK_GWM_RECV_Init(HLONG semid);
extern	HUINT32				ONDK_GWM_RECV_GetMessageQId(void);
extern	void				ONDK_GWM_RECV_RegisterLoop(HUINT32 (*pfnLoopFunction)(void));
extern	void 				INT_ONDK_GWM_GetMessage(void *msg);


extern 	ONDK_Result_t		ONDK_GWM_Destroy(void);


/* Use when the service send a message to application(s) */
extern ONDK_Result_t		    ONDK_GWM_PostMessage(ONDK_GWM_Callback_t appCallback, HINT32 message,
										Handle_t hAct, HINT32 param1, HINT32 param2, HINT32 param3);
extern ONDK_Result_t			ONDK_GWM_PostData(ONDK_GWM_Callback_t appCallback, HINT32 message,
										Handle_t hAct, HINT32 param1, HINT32 param2, HINT32 param3, void *pvData, void (*pfFreeData)(void *));

/* Use when need message exchanges between applications */
extern ONDK_Result_t		    ONDK_GWM_SendMessage(ONDK_GWM_Callback_t appCallback, HINT32 message,
										Handle_t hAct, HINT32 param1, HINT32 param2, HINT32 param3);

/* default application procedure */
extern ONDK_Result_t		    ONDK_GWM_ProcessMessageDefault(HINT32 message,
										Handle_t hAct, HINT32 param1, HINT32 param2, HINT32 param3);

extern void 					ONDK_GWM_RECV_PostMsgToGWM(HINT32 msg, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3);
extern void						ONDK_GWM_RECV_PostDataToGWM(HINT32 msg, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3, void *pvData, void (*pfFreeData)(void *));


// Application이 호출하여 사용할 기능.
extern void				    	ONDK_GWM_APP_SetAppArea(HINT32 x, HINT32 y, HINT32 w, HINT32 h);
extern void				    	ONDK_GWM_APP_GetAppArea(ONDK_GWM_App_t app, ONDK_Region_t *area);
extern void				    	ONDK_GWM_APP_SetAttribute(ONDK_GWM_AppAttribute_e eAppAttribute);
extern ONDK_GWM_AppAttribute_e	ONDK_GWM_APP_GetAttribute(ONDK_GWM_App_t pInApp);
extern void				    	ONDK_GWM_APP_CheckAppUiOrderStacking(void);

extern HERROR			        ONDK_GWM_APP_PreventDraw_LowPriority(ONDK_WINDOW_Position_e ePosition,HUINT32 priority);
extern HUINT32			        ONDK_GWM_APP_GetPriority_PreventDraw(void);
extern void				  		ONDK_GWM_APP_DisablePreventDraw(ONDK_WINDOW_Position_e ePosition);

extern void				    	ONDK_GWM_APP_SetUIHideFlag(ONDK_GWM_AppUiHide_e eAppUiHideFlag);
extern int					    ONDK_GWM_APP_GetUIHideFlag(ONDK_GWM_App_t pInApp);
extern ONDK_Result_t		    ONDK_GWM_APP_HideUI(void);
extern ONDK_Result_t		    ONDK_GWM_APP_ShowUI(void);
extern HBOOL				    ONDK_GWM_APP_GetState_UIHide(ONDK_GWM_App_t pInApp);
extern HBOOL				    ONDK_GWM_APP_GetSelfState_UIHide(ONDK_GWM_App_t pInApp);
extern HBOOL				    ONDK_GWM_APP_GetOtherState_UIHide(ONDK_GWM_App_t pInApp);
extern ONDK_Result_t		    ONDK_GWM_APP_SetUIHide(ONDK_GWM_App_t pInApp);
//extern void	GWM_APP_SetUIHide_All(void);
extern ONDK_Result_t		    ONDK_GWM_APP_SetUIHide_AllexceptMe(ONDK_WINDOW_Position_e eWindowPosition);
extern void				   	 	ONDK_GWM_APP_SetUIHide_AllLowerPriority(ONDK_WINDOW_Position_e eWindowPosition,HINT32 priority);
extern ONDK_Result_t		    ONDK_GWM_APP_SetUIShow(ONDK_GWM_App_t pInApp);
extern void				    	ONDK_GWM_APP_SetUIShow_All(void);
extern ONDK_Result_t		    ONDK_GWM_APP_SetUIShow_AllexceptMe(void);

extern ONDK_GWM_App_t		    ONDK_GWM_APP_Get(ONDK_GWM_Callback_t callback);
extern ONDK_GWM_App_t		    ONDK_GWM_APP_GetThis(void);
extern ONDK_WINDOW_Position_e   ONDK_GWM_APP_GetPosition(ONDK_GWM_App_t app);
extern ONDK_GWM_App_t		    ONDK_GWM_APP_GetUITop(void);
extern ONDK_GWM_App_t		    ONDK_GWM_APP_GetAppTop(void);
extern ONDK_GWM_Callback_t	    ONDK_GWM_APP_GetAppCallback(ONDK_GWM_App_t app);
extern HCHAR * 			        ONDK_GWM_APP_GetAppName(ONDK_GWM_App_t app);
extern void				    	ONDK_GWM_APP_SetAppName(ONDK_GWM_App_t app, HCHAR *pszProcName);
extern HUINT32			        ONDK_GWM_APP_GetPriority(ONDK_GWM_App_t app);

extern ONDK_GWM_GuiObject_t	    ONDK_GWM_APP_GetObject(HINT32 ID);
extern ONDK_GWM_GuiObject_t		ONDK_GWM_Obj_GetObject(HINT32 ID);
extern GWM_Proc_t		        ONDK_GWM_Obj_ChangeObjectProc(HINT32 ID, GWM_Proc_t new_procfunc);
extern ONDK_Result_t		    ONDK_GWM_Obj_ChangeObjectRect(HINT32 ID, ONDK_Rect_t *rect);
extern ONDK_Result_t		    ONDK_GWM_Obj_Destroy(HINT32 id);
extern ONDK_Result_t		    ONDK_GWM_Obj_DestroyAllGuiObject(void);

extern GWM_Proc_t		        ONDK_GWM_APP_ChangeObjectProc(HINT32 ID, GWM_Proc_t new_procfunc);
extern ONDK_Result_t		    ONDK_GWM_APP_ChangeObjectRect(HINT32 ID, ONDK_Rect_t *rect);
extern ONDK_Result_t		    ONDK_GWM_APP_Create(ONDK_WINDOW_Position_e eWindowPosition,HCHAR *app_name, ONDK_GWM_Priority_t property, ONDK_GWM_Callback_t callback,
											HINT32 handle, HINT32 param1, HINT32 param2, HINT32 param3);
extern HERROR			        ONDK_GWM_APP_SwapStackOrder(ONDK_GWM_Callback_t proc1, ONDK_GWM_Callback_t proc2);
extern ONDK_Result_t		    ONDK_GWM_APP_Destroy(ONDK_GWM_Callback_t callBack);
extern ONDK_Result_t		    ONDK_GWM_APP_DestroyAllAppAfter(ONDK_GWM_Callback_t callBack);
extern ONDK_Result_t		    ONDK_GWM_APP_DestroyByPriority(HINT32 priority, HINT32 flag_inclusive );
extern ONDK_Result_t		    ONDK_GWM_APP_DestroyAll(void);


extern ONDK_Result_t			ONDK_GWM_APP_InvalidateONDKRect(ONDK_Rect_t *rt);
extern ONDK_Result_t			ONDK_GWM_APP_InvalidateRelativeONDKRect(ONDK_Rect_t *rt);
extern HBOOL			        ONDK_GWM_APP_IsInvalidateRect(HINT32 x, HINT32 y, HINT32 w, HINT32 h);
extern HBOOL			        ONDK_GWM_APP_IsInvalidateRelativeRect(HINT32 x, HINT32 y, HINT32 w, HINT32 h);
extern ONDK_Result_t		    ONDK_GWM_APP_InvalidateRect(HINT32 x, HINT32 y, HINT32 w, HINT32 h);
extern ONDK_Result_t			ONDK_GWM_APP_InvalidateRelativeRect(HINT32 x, HINT32 y, HINT32 w, HINT32 h);
extern ONDK_Result_t		    ONDK_GWM_APP_InvalidateObject(int objID);
extern ONDK_Result_t		    ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_App_t app);


extern void				        ONDK_GWM_Obj_SetHide(HINT32 id, HBOOL hide);
extern void				        ONDK_GWM_Obj_SetVisible(HINT32 id, HBOOL visible);
extern HBOOL			        ONDK_GWM_Obj_GetHide(HINT32 id);
extern HBOOL			        ONDK_GWM_Obj_GetVisible(HINT32 id);
extern void				        ONDK_GWM_Obj_SetHideByObj(ONDK_GWM_GuiObject_t obj, HBOOL hide);
extern void				        ONDK_GWM_Obj_SetVisibleByObj(ONDK_GWM_GuiObject_t obj, HBOOL visible);
extern HBOOL			        ONDK_GWM_Obj_GetHideByObj(ONDK_GWM_GuiObject_t obj);
extern HBOOL			        ONDK_GWM_Obj_GetVisibleByObj(ONDK_GWM_GuiObject_t obj);

extern GWM_ObjectType_t	        ONDK_GWM_Obj_GetObjectType(HINT32 id);
extern ONDK_Result_t		    ONDK_GWM_Obj_Resize(HINT32 ID, ONDK_Rect_t *rect);
extern ONDK_Result_t		    ONDK_GWM_Obj_SetValue(HINT32 id, int value);
extern ONDK_Result_t		    ONDK_GWM_Obj_ChangeText(HINT32 id, HCHAR *text);
extern ONDK_Result_t		    ONDK_GWM_Obj_EnableObject(HINT32 ID);
extern ONDK_Result_t		    ONDK_GWM_Obj_DisableObject(HINT32 ID);
extern ONDK_Result_t			ONDK_GWM_Obj_SetObjectDrawMethod(HINT32 ID, GWM_ObjDraw_t objDrawFunc);
extern ONDK_Result_t		    ONDK_GWM_Obj_SetFocus(HINT32 ID);
extern HINT32			        ONDK_GWM_Obj_GetObjectStatus(ONDK_GWM_GuiObject_t obj);
extern ONDK_GWM_GuiObject_t	    ONDK_GWM_Obj_GetFocusedObject(void);
extern ONDK_GWM_GuiObject_t	    ONDK_GWM_Obj_GetObjectByID(HINT32 ID);
extern HINT32			        ONDK_GWM_Obj_GetObjectID(ONDK_GWM_GuiObject_t object);
extern ONDK_Result_t		    ONDK_GWM_Obj_SetObjectAttribute(HINT32 ID, HINT32 attribute);
extern ONDK_Result_t		    ONDK_GWM_APP_SetObjectDrawMethod(HINT32 ID, GWM_ObjDraw_t objDrawFunc);
extern ONDK_Result_t		    ONDK_GWM_Obj_GetObjectRect(HINT32 id, ONDK_Rect_t *rc);
extern ONDK_Result_t		    ONDK_GWM_Obj_GetObjectSize(HINT32 id, HINT32 *x, HINT32 *y, HINT32 *w, HINT32 *h);
extern HINT32			        ONDK_GWM_Obj_GetObjectAttr(ONDK_GWM_GuiObject_t obj);
extern ONDK_Result_t		    ONDK_GWM_COM_Image_Create(HINT32 id, ONDK_Rect_t *rect);
extern void		            	ONDK_GWM_COM_Image_SetImage(HINT32 id, HCHAR *imgPath);
extern HCHAR*                  	ONDK_GWM_COM_Image_GetImage(HINT32 id);
extern void						ONDK_GWM_COM_Image_GetRect(HINT32 id, ONDK_Rect_t *ret_pRect);
extern void						ONDK_GWM_COM_Image_GetOrgRect(HINT32 id, ONDK_Rect_t *ret_pRect);
extern void						ONDK_GWM_COM_Image_ChangeRect(HINT32 id, const ONDK_Rect_t *pRect);
extern void						ONDK_GWM_COM_Image_ChangeAlpha(HINT32 id, HINT32 alpha);

extern void						ONDK_GWM_COM_Image_Change(HINT32 id,ONDKImageHandle *hHandle);


extern ONDK_Result_t		    ONDK_GWM_SetTimer(HUINT32 ulTimerID, HUINT32 ulPeriod_msec);
extern ONDK_Result_t		    ONDK_GWM_KillTimer(HUINT32 ulTimerID);
extern ONDK_Result_t		    ONDK_GWM_ResetTimer(HUINT32 ulTimerID);
extern HBOOL					ONDK_GWM_IsTimer(HUINT32 ulTimerID);


extern  HBOOL 			        ONDK_GWM_IsGwmTask(void);
extern	void *					ONDK_GWM_GetData (void);

extern void				    ONDK_GWM_Start(ONDK_GWM_Callback_t pfnAppIgnitor);

/* screen management */


extern void					ONDK_GWM_O2ANI_GetDefaultSettings(HINT32 nAniType/*ONDK_GWM_O2Ani_Type_e*/, ONDK_GWM_O2Ani_Settings_t *pstO2AniSettings, ONDK_Rect_t *pOrgRect);
extern ONDK_Result_t    	ONDK_GWM_O2ANI_AddItem(HINT32 o2aniId, ONDK_GWM_O2Ani_Settings_t *pstO2AniSettings, HINT32 nLastKey);
extern ONDK_Result_t    	ONDK_GWM_O2ANI_Remove(HINT32 o2aniId);
extern ONDK_Result_t     	ONDK_GWM_O2ANI_GetValue(HINT32 o2aniId, ONDK_Rect_t *ret_pDrawRect, HINT32 *ret_alpha);

extern void					ONDK_GWM_DirectRedrawAll(ONDK_WINDOW_Position_e ePosition);
extern void					ONDK_GWM_APP_MoveAppArea(HINT32 x, HINT32 y, HINT32 w, HINT32 h);
extern HBOOL				ONDK_GWM_IsEnabledRedraw(void);
extern void					ONDK_GWM_EnableRedraw(void);
extern void					ONDK_GWM_DisableRedraw(void);


extern HCHAR 				*ONDK_GWM_GwmMsg2String(HINT32 message);


#endif	/* __GWM_H__ */

