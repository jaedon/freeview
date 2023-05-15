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
#ifndef	__GWM_INTERNAL_H__
#define	__GWM_INTERNAL_H__

/* ======================================================================== */
/* Include.																						*/
/* ======================================================================== */
//#include "pal_key.h"
#include <ondk_gwm.h>

#if defined(CONFIG_APUI_SUPPORT_ANIMATION)
#define CONFIG_GWM_O2ANIMATION
#endif

#define ONDK_UNUSED_ARGUMENT(x)	(void)x

/* ======================================================================== */
/* Exported Macros. 																				*/
/* ======================================================================== */
#define ONDK_GWM_MAIN_TASK_NAME			"taskGWM"

/* ------------------------------------------------------------ */
/*	Structure type definition.									*/
/* ------------------------------------------------------------ */
typedef	struct	tagONDK_MSG
{
	ONDK_GWM_Callback_t		appCallback;
	GWM_Message_t		message;
	Handle_t			handle;
	HINT32				param1;
	HINT32				param2;
	HINT32				param3;
	HINT32				param4;

	void				*pvData;
	void				(*pfFreeData)(void *);
} ONDK_MSG_t;


/* ------------------------------------------------------------ */
/*	Global variables for GWM.										 */
/* ------------------------------------------------------------ */
/* Application Management */
extern	ONDK_GWM_App_t			g_pstAppRoot, g_pstCurrentApp;
extern ONDK_GWM_Callback_t		g_pfnAppIgnitor;
extern void						*gMsgData;

/* ------------------------------------------------------------ */
/*	Functions .			 										 */
/* ------------------------------------------------------------ */
extern void						INT_ONDK_GWM_MainTask(void *arg1);
extern void						INT_ONDK_GWM_LOOP(void);
extern 	HBOOL 					INT_ONDK_GWM_DrawCheck(void);

typedef void 					(*DEF_ONDK_GWM_RECV_PostMsgToGWMCallback_t)(HINT32 msg, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3);
typedef void 					(*DEF_ONDK_GWM_RECV_PostDataToGWMCallback_t)(HINT32 msg, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3, void *pvData, void (*pfFreeData)(void *));
extern void						INT_ONDK_GWM_RedrawAllApp(ONDK_WINDOW_Position_e ePosition);
extern void						INT_ONDK_GWM_SetClip(ONDKSurfaceHandle scr, HINT32 x1, HINT32 y1, HINT32 x2, HINT32 y2);
extern void						INT_ONDK_GWM_GetClipElement(ONDKWindowHandle *scr, HINT32 *x1, HINT32 *y1, HINT32 *x2, HINT32 *y2);
extern void						INT_ONDK_GWM_GetClipRect(ONDKWindowHandle *scr, ONDK_Rect_t *pstOsdRect);
extern HBOOL					INT_ONDK_GWM_IsClippedArea(ONDKWindowHandle *scr, int x1, int y1, int x2, int y2);

extern void 					INT_ONDK_GWM_RECV_RegisterKeyCallback(ONDKWindowHandle mWindow, ONDK_KEY_KeyNotifyCallback_t pfnKeyNotifyCallback);
extern void						INT_ONDK_GWM_RECV_RegisterPostMsgToApCallBack(DEF_ONDK_GWM_RECV_PostMsgToGWMCallback_t pfncPostMsgCallback);
extern void						INT_ONDK_GWM_RECV_RegisterPostDataToApCallBack(DEF_ONDK_GWM_RECV_PostDataToGWMCallback_t pfncPostDataCallback);

extern const HBOOL				INT_ONDK_GWM_GetHaveToRedraw(ONDK_WINDOW_Position_e ePosition);
extern void						INT_ONDK_GWM_SetHaveToRedraw(ONDK_WINDOW_Position_e ePosition,HBOOL value);
extern const HBOOL				INT_ONDK_GWM_GetStackOderChanged(void);
extern void				   		INT_ONDK_GWM_SetStackOderChanged(HBOOL value);
extern const ONDK_Region_t		INT_ONDK_GWM_GetUpdateArea(ONDK_WINDOW_Position_e ePosition);
extern void						INT_ONDK_GWM_SetUpdateArea(ONDK_WINDOW_Position_e ePosition,ONDK_Region_t value);

extern	ONDK_Result_t     		INT_ONDK_GWM_TIMER_Init(void);
extern	ONDK_Result_t			INT_ONDK_GWM_TIMER_KillAllByAPP(ONDK_GWM_App_t app);


extern	ONDK_Result_t     		INT_ONDK_GWM_O2ANI_Init(void);
extern 	HBOOL 					INT_ONDK_GWM_O2ANI_IsAppAnimation(void);
extern	ONDK_GWM_O2Ani_Item_t *	INT_ONDK_GWM_O2ANI_GetProcAnimationItem(ONDK_GWM_App_t pApp);
extern	ONDK_Result_t			INT_ONDK_GWM_O2ANI_UpdateProcAnimation(ONDK_GWM_O2Ani_Item_t *pstProcAniItem, ONDKSurfaceHandle srcSurface, ONDKSurfaceHandle dstSurface);


extern	void					INT_ONDK_GWM_OBJ_InitObjects(void);

extern 	void					INT_ONDK_GWM_APP_InitApps(void);
extern	ONDK_Result_t			INT_ONDK_GWM_APP_AddObject(ONDK_GWM_App_t app, ONDK_GWM_GuiObject_t new_object);
extern	ONDK_Result_t			INT_ONDK_GWM_APP_RemoveObject(ONDK_GWM_App_t app, ONDK_GWM_GuiObject_t remove_object);
extern	void					INT_ONDK_GWM_APP_RedrawAllApplication(void);	// body가 없다. TBD?
extern	ONDK_GWM_GuiObject_t	INT_ONDK_GWM_APP_GetObjectList(ONDK_GWM_App_t app);
extern	char *					INT_ONDK_GWM_APP_GetAppName(ONDK_GWM_App_t app);
extern	int						INT_ONDK_GWM_APP_GetStackingOrder(ONDK_GWM_App_t app);
extern	int						INT_ONDK_GWM_APP_GetUiStackOrder(ONDK_GWM_App_t app);
extern	int						INT_ONDK_GWM_APP_SetUiStackOrder(ONDK_GWM_App_t app, int uiStackCnt);
extern	ONDK_Result_t			INT_ONDK_GWM_APP_Redraw(ONDK_GWM_App_t app, ONDK_Region_t *update_area);
extern	HBOOL					INT_ONDK_GWM_APP_IsO2AniProcAnimation(void);

extern	void   					INT_ONDK_GWM_OBJ_BasicSetting(ONDK_GWM_GuiObject_t object);
extern	ONDK_GWM_GuiObject_t	INT_ONDK_GWM_OBJ_FindObject(ONDK_GWM_GuiObject_t objlst, int ID);
extern	ONDK_GWM_GuiObject_t	INT_ONDK_GWM_OBJ_GetNextObject(ONDK_GWM_GuiObject_t object);
extern 	void					INT_ONDK_GWM_OBJ_GetObjectRect(ONDK_GWM_GuiObject_t object, ONDK_Rect_t *rect);
extern 	void					INT_ONDK_GWM_OBJ_SetDrawMethod(ONDK_GWM_GuiObject_t object, GWM_ObjDraw_t drawFunc);
extern	GWM_Proc_t				INT_ONDK_GWM_OBJ_ChangeObjectProc(ONDK_GWM_GuiObject_t object, GWM_Proc_t new_procfunc);
extern	ONDK_Result_t			INT_ONDK_GWM_OBJ_BaseDefaultMethod(GWM_Obj_MethodType_t proc_msg, ONDK_GWM_GuiObject_t object, HINT32 arg);
extern	ONDK_Result_t			INT_ONDK_GWM_OBJ_CallObjectProc(GWM_Obj_MethodType_t method, ONDK_GWM_GuiObject_t object, int arg);
extern	ONDK_Result_t			INT_ONDK_GWM_OBJ_SetObjectAttribute(ONDK_GWM_GuiObject_t object, int attribute);
extern	void					INT_ONDK_GWM_OBJ_SetObjectAnimated(ONDK_GWM_GuiObject_t object);
extern	void 					INT_ONDK_GWM_OBJ_ResetObjectAnimated(ONDK_GWM_GuiObject_t object);
extern	ONDK_Result_t			INT_ONDK_GWM_OBJ_MoveFocus(ONDK_GWM_GuiObject_t obj, int direction);


extern	HINT32					DBG_ONDK_GWM_TIMER_DumpList(void *arg);
extern	HINT32					DBG_ONDK_GWM_APP_DumpList(void *arg);

#endif	/* __GWM_INTERNAL_H__ */

