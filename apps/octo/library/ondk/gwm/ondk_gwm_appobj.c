/**************************************************************
*	@file		ondk.c
*	Minimized Graphic Library for DirectFB
*
*	http://www.humaxdigital.com
*
*	@data			2012/09/17
*	@author			humax
*	@brief			Minimized Graphic Library for DirectFB
**************************************************************/


/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding �� ����ϴ� source file �Դϴ�.
* MS949 encoding�����Ī��� source file�Ǫ���
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* �� 2011-2012 Humax Co., Ltd.
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

/* ======================================================================== */
/* Include. 																						*/
/* ======================================================================== */
#include <ondk.h>
#include <ondk_gfx.h>

#include <ondk_gwm_config.h>
#include <ondk_gwm_appobj.h>
#include "ondk_gwm_objdef.h"

#include "ondk_gwm_lnklist.h"
#include "ondk_gwm_internal.h"



/* ======================================================================== */
/* Private Macros. 																					*/
/* ======================================================================== */

#define	DEF_MAX_APPLICATION		DEF_GWM_MAX_APPLICATION		// �ִ� 16���� Application�� ���ÿ� �����.

/* ======================================================================== */
/* Global/Extern Variables.																			*/
/* ======================================================================== */
HBOOL			g_bIsAllHide_Flag;
ONDK_GWM_App_t	g_stAllHideApp;
HUINT32			g_ulPreventedDrawPriority;
ONDK_Region_t	g_rgAppErasedArea;

/* ======================================================================== */
/* Private Types.																					*/
/* ======================================================================== */
struct tagApplicationObject
{
	// Application Object�� ������ �׸��� �߰��Ϸ��� �ݵ�� �ؿ� �߰��ϵ��� �Ѵ�.
	// ������ link object�� �ݵ�� �� ���� �����ؾ� �ϴ� �׸��̴�.
	LINK_OBJECT			link;						/**< ��ũƮ����Ʈ ���� */
	/*GWM_ObjectType_t		type;*/					/**< Object�� ����.		*/

	HCHAR					name[DEF_APP_NAME_LENGTH];		/**<  RTTI ���ڿ�. */
	HINT32					priority;
	ONDK_GWM_Callback_t		callBack;
	ONDK_Region_t			Area;						/**< Application �� ��ǥ ����. */
	ONDK_Region_t 			ClipArea;					/**< Application Clip Area - it has the meaning only when CONFIG_GWM_APPCLIPAREA defined */
	ONDK_Region_t			UpdateArea;					/**< Application �� ��ǥ ����.	*/
	ONDK_GWM_GuiObject_t	ObjectList;					/**< GuiObject List. */
	ONDK_GWM_AppAttribute_e	Attribute;

	ONDK_GWM_AppUiHide_e	Hide_ableFlag;				/**< hide�� ���� �Ӽ��� �����Ѵ�. */
	HBOOL					UI_Hide_BySelf;				/**< �ڱ� ������ Hide �ߴ����� flag, counter�� �� �� ������ �ڱ� �ڽ��� ȥ�� �̹Ƿ� max�� 1 */
	HBOOL					UI_Hide_ByOther;			/**< ���鿡 ���� Hide�Ǿ��� ���� flag. counter�� �� �� ������ max�� ���� �� �ִ� ���ø����̼� �� */
	HINT32					CountOfCallHideOthers;		/**< for sync show/hide */
	HINT32					hideExceptionAppCount;
	ONDK_GWM_App_t			*pHideExceptionAppList;

	HBOOL					active;						/* alarm activity */
	HULONG             	 	lap;						/* alarming interval : tick-unit */
	HULONG					time_out;					/* alarm expiration */
	void 					(*callback_func)(void *);	/* executing callback */

	HINT32						stack_count;				/* application stacking order considering its priority */
	HINT32						orderUi_stack;

	HBOOL					bAllocated;
	HUINT32					ulUiState;
	ONDK_WINDOW_Position_e	eWindowPosition;
#if 0
	POINTER_LIST_T		*pstLoadedImgLink;
#endif
};

#if 0
extern HERROR AP_StockImg_UnregisterImage (HINT32 nStockImgId);
#endif

/* ======================================================================== */
/* Private Constants. 																				*/
/* ======================================================================== */


/* ======================================================================== */
/* Private Variables. 																				*/
/* ======================================================================== */
STATIC	struct	tagApplicationObject	s_arrAppArray[DEF_MAX_APPLICATION];
STATIC	HINT32							s_nAlarmRefNum;
STATIC	HINT32							s_nAppStackOrderMax;
STATIC	HBOOL							s_bAppDimMode = TRUE;	// default ON!

#if defined(CONFIG_GWM_O2ANIMATION)
STATIC	HBOOL							s_bO2ProcAnimation= FALSE;	// is current proc animation
#endif

/* ======================================================================== */
/* Private Function prototypes. 																		*/
/* ======================================================================== */
STATIC	HINT32		local_gwmapp_always_ReturnOne(LINK_OBJECT node1, LINK_OBJECT node2)
{						// ��ũƮ ����Ʈ���� �׻� �� �������� �߰��� �� �ֵ��� �ϱ����� �Լ�.
	GWM_assert(node1);
	GWM_assert(node2);
	return	1;
}

STATIC	HINT32		local_gwmapp_DirectCompare(LINK_OBJECT node1, LINK_OBJECT node2)
{						// ������ �ƴ����� ���Ѵ�.
	return	(HINT32)node1 - (HINT32)node2;
}

#if 0	 // not used -> remove warning
STATIC	HINT32		local_gwmapp_CompareObjectZorder(LINK_OBJECT node1, LINK_OBJECT node2)
{
	ONDK_GWM_GuiObject_t		obj1, obj2;

	obj1 = (ONDK_GWM_GuiObject_t)node1;
	obj2 = (ONDK_GWM_GuiObject_t)node2;

	return	obj1->z_order - obj2->z_order;
}
#endif

#if 0
STATIC ONDK_GWM_App_t		local_gwmapp_AllocApp(void)
{
	HINT32		i;

	for (i=0; i<DEF_MAX_APPLICATION; i++)
	{	if (s_arrAppArray[i].name[0] == 0)		// �� ����.
			break;
	}
	if (i<DEF_MAX_APPLICATION)
		return	&s_arrAppArray[i];
	return	NULL;
}
#endif

STATIC ONDK_GWM_App_t		local_gwmapp_AllocApp(void)
{
	STATIC HINT32 s_nCurrPos = 0;
	HINT32 i;

	i = (s_nCurrPos +1) % DEF_MAX_APPLICATION;
	while (i != s_nCurrPos)
	{
		if (s_arrAppArray[i].bAllocated == FALSE)
		{
			s_nCurrPos = i;
			s_arrAppArray[i].bAllocated = TRUE;

			return &s_arrAppArray[i];
		}

		i = (i+1) % DEF_MAX_APPLICATION;
	}

	return NULL;
}


STATIC void			local_gwmapp_FreeApp(ONDK_GWM_App_t	app)
{
	GWM_assert(app);

	app->link = NULL;
	app->bAllocated = FALSE;

	VK_memset(app->name, 0, DEF_APP_NAME_LENGTH);		// RTTI ���ڿ��� Ŭ����.
	app->priority = 0;

// Callback �� App Destroy �� Ÿ App�� Create �� �� Parent App�� callback�� �� �ʿ䰡 �����Ƿ� ���⼭ NULL�� ó������ �ʴ´�.
// ���⼭ NULL�� �� �ص� Alloc �ؼ� Create �� �� �� Callback���� ��������.
//	app->callBack = NULL;

	app->Area.x1 = 0;					app->Area.y1 = 0;
	app->Area.x2 = GWM_SCREEN_WIDTH;	app->Area.y2 = GWM_SCREEN_HEIGHT;
	app->ClipArea.x1 = 0;				app->ClipArea.y1 = 0;
	app->ClipArea.x2 = 0;				app->ClipArea.y2 = 0;
	app->UpdateArea.x1 = 0;				app->UpdateArea.y1 = 0;
	app->UpdateArea.x2 = 0;				app->UpdateArea.y2 = 0;
	app->ObjectList = NULL;
}

STATIC HINT32				local_gwmapp_CompareAppPriority(LINK_OBJECT node1, LINK_OBJECT node2)
{
	ONDK_GWM_App_t	app1, app2;

	GWM_assert(node1);
	GWM_assert(node2);

	app1 = (ONDK_GWM_App_t)node1;
	app2 = (ONDK_GWM_App_t)node2;
	return	app2->priority - app1->priority;
}


STATIC void			local_gwmapp_CheckAppUiOrderStacking(ONDK_GWM_App_t rootApp)
{
	ONDK_GWM_App_t		app;
	HINT32				count = 0;
	HINT32				ui_stack_count =0;

	app = (ONDK_GWM_App_t)rootApp;
	while (app != NULL)
	{
		if (count > DEF_GWM_MAX_APPLICATION)
			break;

		count++;

		/* �Ʒ� �� �˻� ������ �ٲ��� ���ƾ� �Ѵ�. hidden�� fake UI�� �����ϱ� ����.
			app attribute�� fake mask�� ������ UI�� �ִٰ� �Ǵ��Ѵ�.
			app attribute�� hidden mask�� ������ UI�� ���ٰ� �Ǵ��Ѵ�.
			app attribute�� normal �̸� UI app�� �����Ѵ�. wjmoh
		*/
		if((app->Attribute & GWM_APP_FAKE_MASK)
			|| ((INT_ONDK_GWM_APP_GetObjectList(app) != NULL) && (!(app->Attribute & GWM_APP_HIDDEN_MASK))))
		{
			/* ui_stack_counter 0 is the top most app with UI obj */
			app->Attribute |= GWM_RealUI_App;			/* GWM_RealUI_App �Ӽ��� �Ҵ�. */
			app->orderUi_stack = ui_stack_count++;
		}
		else
		{
			/* �ƹ��� hidden�� �־����� ���� objet�� ������, �׳� basic app�� �ȴ�. */
			app->Attribute &= (~GWM_APP_UI_ATTR_MASK);	/* GWM_RealUI_App �Ӽ��� ����. */
			app->Attribute |= GWM_Basic_App;			/* ���ǹ��ϱ� ������, �ǵ������� basic �ȴٴ� ���� �����ϴ� ��! �̴�. */
			app->orderUi_stack = -1;
		}

		app = (ONDK_GWM_App_t)gwm_lnklst_GetNext((LINK_OBJECT)app);
	}
}


STATIC void			local_gwmapp_OrderAppStacking(ONDK_GWM_App_t rootApp)
{
	ONDK_GWM_App_t		reverseApp[DEF_GWM_MAX_APPLICATION];
	ONDK_GWM_App_t		app;
	HINT32				count = 0, stack_count = 0;

	s_nAppStackOrderMax = 0;

	app = (ONDK_GWM_App_t)rootApp;
	while (app != NULL)
	{
		if (count >= DEF_GWM_MAX_APPLICATION)
			break;

		reverseApp[count] = app;
		count++;	/* count�� ++ positive ���길 �ؾ� �Ѵ�. */
		app = (ONDK_GWM_App_t)gwm_lnklst_GetNext((LINK_OBJECT)app);
	}

	while (count > 0)
	{
		reverseApp[count - 1]->stack_count = stack_count;

		stack_count++;
		count--;
	}

	/* s_nAppStackOrderMax is the key to find out which app is the frontmost */
	if (stack_count > 0)
		s_nAppStackOrderMax = stack_count - 1;
}

STATIC void	local_gwmapp_UnlinkAppObject(ONDK_GWM_App_t deleted_app)
{	ONDK_GWM_GuiObject_t		objList, temp_obj;
	ONDK_Result_t		hresult;

	GWM_assert(deleted_app);

	// 1. Application ����Ʈ���� ������ Application�� ����.
	gwm_lnklst_RemoveItemWithCompare( (LINK_OBJECT*)&g_pstAppRoot, (LINK_OBJECT)deleted_app, local_gwmapp_DirectCompare );

	// ������ Application�� MSG_GWM_DESTROY �޼��� ����.  // �޼����� �����ϴ� ���� ��ũƮ����Ʈ���� ���ŵ� �Ŀ� ȣ���� ��. �׷��� ������, GWM_DESTROY �޼����� �޾Ƽ� DestroyAllApplicationAfter �� ����ϴ� ���, Recursive�ϰ� ȣ��Ǳ� ������ ���ѷ����� ������.  				- GWM_DESTROY �� PostMessage �� Queue�� �־� ó������ �ʴ� ���
	ONDK_GWM_APP_Call(deleted_app, MSG_GWM_DESTROY, (Handle_t) NULL, 0, 0, 0, NULL);

	// 2. �����ִ� ��� Gui_Object���� ���� Destroy �ؾ߸� �Ѵ�.
	//   ***�߿�: GWM_DESTROY �ÿ� �� Object ��� ���� ��� �Ӽ��� ��� �� ���� �ֱ� ������, �ݵ�� GWM_DESTROY �޼����� ó���� �Ŀ� Object �� Delete ��ų ��.
	objList = deleted_app->ObjectList;
	while(objList != NULL)
	{
		temp_obj = objList;
		GWM_DBG_Print( DBGMSG_TRACE, ("Remove Object..ID=%d\n", ONDK_GWM_Obj_GetObjectID(temp_obj) ));
		hresult = INT_ONDK_GWM_APP_RemoveObject(deleted_app, temp_obj);
		// Object�� Destroy �Ǹ鼭 ������ �� ������ Invalidate �Ѵ�.
		INT_ONDK_GWM_OBJ_CallObjectProc(MT_DESTROY, temp_obj, 0);
		objList = deleted_app->ObjectList;
	}

	// 6. Application ��ü�� �޸𸮸� ����.
	local_gwmapp_FreeApp(deleted_app);
}

/* ======================================================================== */
/* Functions. 																*/
/* ======================================================================== */
STATIC void	local_gwmapp_DumpObject(ONDK_GWM_App_t app)
{
#ifdef CONFIG_DEBUG
	ONDK_GWM_GuiObject_t		ObjectList;

	if (app==NULL)
		return;

	ObjectList = app->ObjectList;
	while(ObjectList)
	{
		HLIB_CMD_Printf("__obj__ ID(0x%X) type(%d) rect(%d %d %d %d), focus(0x%X)\n", ObjectList->ID, ObjectList->type,
																			ObjectList->rect.x, ObjectList->rect.y, ObjectList->rect.w, ObjectList->rect.h,
																			ObjectList->focus);
		ObjectList = (ONDK_GWM_GuiObject_t)gwm_lnklst_GetNext((LINK_OBJECT)ObjectList);
	}
#endif
}

STATIC void	local_gwmapp_DumpApplication(ONDK_GWM_App_t app)
{
	HLIB_CMD_Printf("Application=[%s]%X:\n", app->name,(HINT32)app);
	if (app==NULL)
		return;
	//HxLOG_Print("\tlink=%08X, priority=%d, callBack=%08X, objList=%08X\n", (HINT32)app->link, app->priority, (HINT32)app->callBack, (HINT32)app->ObjectList);
	//HxLOG_Print("\t(%3d,%3d)-(%3d,%3d), name=%s\n", app->Area.x1, app->Area.y1, app->Area.x2, app->Area.y2, app->name );

	HLIB_CMD_Printf("\tpriority=%d, ui_state=%08x, callBack=%08X, stack_count=%d, \n", app->priority, app->ulUiState, (HINT32)app->callBack, app->stack_count);
	HLIB_CMD_Printf("\tapp UI Hide?(%s)\tUI_stack_count=%d UI Attribute(%s)\n", (((app->UI_Hide_BySelf == TRUE) || (app->UI_Hide_ByOther == TRUE))? ("TRUE"):("FALSE")),
						app->orderUi_stack,
						((app->Attribute&GWM_APP_UI_ATTR_MASK)?("UI APP"):
							((app->Attribute&GWM_APP_FAKE_MASK)?("FAKE UI APP"):
								((app->Attribute&GWM_APP_HIDDEN_MASK)?("HIDDEN UI APP"):("NORMAL APP")))));
	HLIB_CMD_Printf("\tArea : (%3d,%3d)-(%3d,%3d)\n", app->Area.x1, app->Area.y1, app->Area.x2, app->Area.y2);
	HLIB_CMD_Printf("\talarm : active(%d) lap(%lu) time_out(%lu)\n", app->active, app->lap, app->time_out);

	local_gwmapp_DumpObject(app);
	DBG_ONDK_STRINGS_Dump((HUINT32)app);
	DBG_ONDK_IMG_Dump((void *)app);
	HLIB_CMD_Printf("\r\n");
}


#define		_____INT_ONDK_GWM_APP______

// GWM ���ο����� ���Ǵ� ���. Application������ ȣ������ �� ��. ���� !!
ONDK_Result_t				INT_ONDK_GWM_APP_Redraw(ONDK_GWM_App_t app, ONDK_Region_t *pUpdateArea)
{
	ONDK_GWM_App_t			backupApp;
	ONDK_GWM_GuiObject_t		obj_list;
	ONDK_Result_t		hresult = ERR_OK;
	ONDK_Region_t			area;
	//HINT32					x1, y1, x2, y2;
	HBOOL				isFrontMost = FALSE;
	ONDK_Region_t		total_area, obj_area, intersect_area;
	ONDK_Rect_t			objRect;
#if defined(CONFIG_GWM_O2ANIMATION)
	ONDKSurfaceHandle		hOriginSurface	= NULL;
	ONDK_GWM_O2Ani_Item_t	*pstO2ProcAniItem = NULL;
#endif

	VK_memset(&total_area, 0x00, sizeof(ONDK_Region_t));

	/* all hide ����� ���� �ְ�, �׷����߸� �ϴ� ������ �ƴ϶��.*/
	if((g_bIsAllHide_Flag == TRUE) && (g_stAllHideApp != app))
	{
		return GWM_RET_DONT_NEED_TO_DRAW;
	}

	if((app->UI_Hide_BySelf == TRUE) || (app->UI_Hide_ByOther == TRUE))
	{
		return GWM_RET_DONT_NEED_TO_DRAW;
	}

	if(app->priority < g_ulPreventedDrawPriority)
	{
		return GWM_RET_DONT_NEED_TO_DRAW;
	}

	backupApp = g_pstCurrentApp;
	g_pstCurrentApp = app;

	if( s_bAppDimMode == TRUE )
	{
		// check if the current app is the frontmost, stacking all apps under itself.
		isFrontMost = (INT_ONDK_GWM_APP_GetUiStackOrder(app) == 0) ? TRUE : FALSE;
	}
	else
	{
		// UI Design Concept�� ���� Dim ó���� ���� ���� ���� �����Ƿ� �� ó����
		// Disable ��ų �� �ֵ��� �Ѵ�.
		isFrontMost = TRUE;
	}

#if defined(CONFIG_GWM_O2ANIMATION)
	pstO2ProcAniItem = INT_ONDK_GWM_O2ANI_GetProcAnimationItem(app);
	if (pstO2ProcAniItem != NULL)
	{
		hOriginSurface = ONDK_GWM_GetCurSurface();
		s_bO2ProcAnimation = TRUE;
		if (pstO2ProcAniItem->stSettings.bCompletedUiDraw == TRUE)
		{
			// proc animation �� �����ϴ� ���, ���� 1ȸ nx ani surface �� UI �� �����ϰ�,
			// �Ŀ��� COPY_PROC_ANI �� �̵��� animation value ó���� , blt �� �Ѵ�.
			goto COPY_PROC_ANI;
		}
		else
		{
			// ���� proc ani �� �����ϴ� ���, ��ü ������ clear �Ѵ�.
			ONDK_Region_t region= ONDK_Region(0, 0, (GWM_SCREEN_WIDTH-1), (GWM_SCREEN_HEIGHT-1));
			ONDKSurfaceHandle procAniSurface = ONDK_GWM_GetProcAniSurface();
			if (procAniSurface)
				ONDK_SURFACE_ClearRect(procAniSurface, region);
		}
	}
#endif

	ONDK_GWM_APP_GetAppArea(g_pstCurrentApp, &area);

	obj_list = INT_ONDK_GWM_APP_GetObjectList(app);
	while(obj_list != NULL)
	{
		if(ONDK_GWM_Obj_GetHideByObj(obj_list))
		{
			INT_ONDK_GWM_OBJ_GetObjectRect(obj_list, &objRect);
	 		obj_list = INT_ONDK_GWM_OBJ_GetNextObject(obj_list);
			continue;
		}

 		INT_ONDK_GWM_OBJ_GetObjectRect(obj_list, &objRect);
		if(!ONDK_GWM_APP_IsInvalidateRelativeRect(objRect.x, objRect.y, objRect.w, objRect.h))
		{
	 		obj_list = INT_ONDK_GWM_OBJ_GetNextObject(obj_list);
	 		continue;
		}

		obj_area.x1 = area.x1 + objRect.x;
		obj_area.y1 = area.y1 + objRect.y;
		obj_area.x2 = obj_area.x1 + objRect.w - 1;
		obj_area.y2 = obj_area.y1 + objRect.h - 1;
		intersect_area = ONDK_IntersectRegion(pUpdateArea, &obj_area);

		ONDK_SURFACE_SetClip(ONDK_GWM_GetCurSurface(), intersect_area);

		ONDK_Print("[%s:%d] obj_list draw isFrontMost(%d) (%d,%d,%d,%d)\r\n", __FUNCTION__, __LINE__, isFrontMost, objRect.x, objRect.y, objRect.w, objRect.h);

		hresult = INT_ONDK_GWM_OBJ_CallObjectProc(MT_DRAW, obj_list, (HINT32)isFrontMost);
		if(GWM_RET_NO_OBJECT != hresult)
		{
			total_area = ONDK_UnionRegion(&obj_area, &total_area);
		}

 		obj_list = INT_ONDK_GWM_OBJ_GetNextObject(obj_list);
	}

	// Appliction�� ��ü ũ���� Frame �� ���� ��� Frame ������ �ƹ��͵� �׸� �� ����. ����,
	// GWM_DRAW �޼����� GUI Object ���� ��� �� �׷��� ������ ȣ��Ǿ�߸� �Ѵ�.
	hresult = ONDK_GWM_APP_Call(app, MSG_GWM_DRAW, (Handle_t) NULL,
					((area.x1 << 16) | area.y1), ((area.x2 << 16) | area.y2), 0, NULL);
	//INT_ONDK_GWM_SetClip(ONDK_GWM_GetSurface(), x1, y1, x2, y2);

	g_pstCurrentApp = backupApp;

#if defined(CONFIG_PACKAGE_DIRECTFB)
#else
	// merged object ���� ������ update
	{
		pUpdateArea->x1 = total_area.x1;
		pUpdateArea->y1 = total_area.y1;
		pUpdateArea->x2 = total_area.x2;
		pUpdateArea->y2 = total_area.y2;
	}
#endif

#if defined(CONFIG_GWM_O2ANIMATION)
	// ���� 1ȸ UI draw �Ŀ��� copy �� �ϵ��� flag ó��.
	if (s_bO2ProcAnimation)
		pstO2ProcAniItem->stSettings.bCompletedUiDraw = TRUE;

COPY_PROC_ANI:
	if (s_bO2ProcAnimation)
	{
		ONDKSurfaceHandle pProcAniSurface = ONDK_GWM_GetCurSurface();
		// s_bNxAnimation flag �� FALSE ó���������,  ���� Apps �� Object ���� ONDK Surface �� �����ϰ� �ȴ�.
		s_bO2ProcAnimation = FALSE;
		INT_ONDK_GWM_O2ANI_UpdateProcAnimation(pstO2ProcAniItem, pProcAniSurface, hOriginSurface);

		g_pstCurrentApp = backupApp;
	}
#endif


	return	hresult;
}


ONDK_GWM_GuiObject_t	INT_ONDK_GWM_APP_GetObjectList(ONDK_GWM_App_t app)
{
	GWM_assert(app);
	return	app->ObjectList;
}

void	INT_ONDK_GWM_APP_InitApps(void)
{
	HINT32		i;

	for (i=0; i<DEF_MAX_APPLICATION; i++)
	{
		s_arrAppArray[i].bAllocated = FALSE;
		s_arrAppArray[i].link = NULL;
		memset(s_arrAppArray[i].name, 0, DEF_APP_NAME_LENGTH);		// RTTI ���ڿ��� Ŭ����.
		s_arrAppArray[i].priority = 0;
		s_arrAppArray[i].callBack = NULL;
		s_arrAppArray[i].Area.x1 = 0;					s_arrAppArray[i].Area.y1 = 0;
		s_arrAppArray[i].Area.x2 = GWM_SCREEN_WIDTH;	s_arrAppArray[i].Area.y2 = GWM_SCREEN_HEIGHT;
		s_arrAppArray[i].ObjectList = NULL;
	}

	/* init the number of alarm reference count */
	s_nAlarmRefNum = 0;
}

/* for GWM internal purpose only. */
HCHAR *INT_ONDK_GWM_APP_GetAppName(ONDK_GWM_App_t app)
{
	if (app != NULL)
		return	app->name;
	else
		return	NULL;
}


HINT32		INT_ONDK_GWM_APP_GetStackingOrder(ONDK_GWM_App_t app)
{
	if (app != NULL)
		return app->stack_count;
	else
		return 0;
}

HINT32		INT_ONDK_GWM_APP_GetUiStackOrder(ONDK_GWM_App_t app)
{
	if (app != NULL)
		return app->orderUi_stack;
	else
		return -1;
}

HINT32		INT_ONDK_GWM_APP_SetUiStackOrder(ONDK_GWM_App_t app, HINT32 uiStackCnt)
{
	if (app != NULL)
		return app->orderUi_stack = uiStackCnt;
	else
		return -1;
}

ONDK_Result_t		INT_ONDK_GWM_APP_AddObject(ONDK_GWM_App_t app, ONDK_GWM_GuiObject_t new_object)			// ��ũƮ ����Ʈ�� �� �������� �߰��ȴ�.
{
	GWM_assert(app);
	GWM_assert(new_object);

	// �̹� ���� ID �� Object �� ��ϵǾ� �ִ����� �� �Լ��� ȣ���ϱ� ���� �̸� �˻� �ȴ�.
	return	gwm_lnklst_AddItemWithCompareDescend( (LINK_OBJECT*)&(app->ObjectList), (LINK_OBJECT)new_object, local_gwmapp_always_ReturnOne );
}


ONDK_Result_t		INT_ONDK_GWM_APP_RemoveObject(ONDK_GWM_App_t app, ONDK_GWM_GuiObject_t remove_object)		// ��ũƮ ����Ʈ���� �����Ѵ�.
{
	ONDK_Result_t	hresult;
	ONDK_Region_t		invalidate_area,temp;
	ONDK_Rect_t		    rect;
	GWM_assert(app);
	GWM_assert(remove_object);

	hresult = gwm_lnklst_RemoveItemWithCompare((LINK_OBJECT*)&(app->ObjectList), (LINK_OBJECT)remove_object, local_gwmapp_DirectCompare );

	GWM_DBG_Print( DBGMSG_TRACE, ("Remove Object:\n"));
	INT_ONDK_GWM_OBJ_GetObjectRect(remove_object, &rect);
	invalidate_area.x1 = app->Area.x1+rect.x;
	invalidate_area.y1 = app->Area.y1+rect.y;
	invalidate_area.x2 = app->Area.x1+rect.x+rect.w;
	invalidate_area.y2 = app->Area.y1+rect.y+rect.h;
    temp = INT_ONDK_GWM_GetUpdateArea(ONDK_GWM_APP_GetPosition(app));
	INT_ONDK_GWM_SetUpdateArea(ONDK_GWM_APP_GetPosition(app),ONDK_UnionRegion(&invalidate_area, &temp));

	return	hresult;
}

HBOOL				INT_ONDK_GWM_APP_IsO2AniProcAnimation(void)
{
#if defined(CONFIG_GWM_O2ANIMATION)
	return s_bO2ProcAnimation;
#else
	return FALSE;
#endif
}

#define		_____ONDK_GWM_APP_DEBUGGING______

HINT32	DBG_ONDK_GWM_APP_DumpList(void *arg)
{
	ONDK_GWM_App_t		base_app;

	HLIB_CMD_Printf("============================================================================\n");
	HLIB_CMD_Printf("\n------ GWM_APP_DumpList (%d apps) : alarm_ref_cnt(%d) ------\n", (s_nAppStackOrderMax + 1), s_nAlarmRefNum);

	base_app = (ONDK_GWM_App_t)g_pstAppRoot;
	while(base_app != NULL)
	{
		local_gwmapp_DumpApplication(base_app);
		base_app = (ONDK_GWM_App_t)gwm_lnklst_GetNext((LINK_OBJECT)base_app);
	}
	HLIB_CMD_Printf("============================================================================\n");

	(void)arg;
	return 0;
}


#define		_____ONDK_GWM_APP_DRAW_DIM_MODE_____

void	ONDK_GWM_APP_DisableDimMode(void)
{
	s_bAppDimMode = FALSE;
}


// Application�� ȣ���Ͽ� ����� ���.

#define		_____ONDK_GWM_APP_ATTRIBUTTE______

/* �� �Լ��� MSG_CREATE ���� �ҷ��߸� ȿ���� �ִ�.
	�� �̿��� �޼������� ó������ ���Ѵ�.
	������ GWM_APP_Create()������ �ش� value�� üũ�ؼ� �Ӽ��� �����ϱ� �����̴�.
*/
void		ONDK_GWM_APP_SetAttribute(ONDK_GWM_AppAttribute_e eAppAttribute)
{
	if((eAppAttribute >= GWM_Basic_App) && (eAppAttribute < GWM_App_Attr_MAX))
	{
		g_pstCurrentApp->Attribute = eAppAttribute;
	}

	local_gwmapp_CheckAppUiOrderStacking(g_pstAppRoot);
}

ONDK_GWM_AppAttribute_e		ONDK_GWM_APP_GetAttribute(ONDK_GWM_App_t pInApp)
{
	if(pInApp == 0)
	{
		pInApp = g_pstCurrentApp;
	}

	return pInApp->Attribute;
}

void	ONDK_GWM_APP_CheckAppUiOrderStacking(void)
{
	local_gwmapp_CheckAppUiOrderStacking(g_pstAppRoot);
}


#define		_____ONDK_GWM_APP_SHOW_HIDE______

/*
	�� �Լ��� pair �̹Ƿ� ¦�� �� ���缭 ����ؾ� �Ѵ�. -_-..
	GWM_APP_DisablePreventDraw(void) ��� �� �ҷ� �� ��.
	GWM_APP_PreventDraw_LowPriority(0) �䷸�� ���൵ ��.
*/

HERROR		ONDK_GWM_APP_PreventDraw_LowPriority(ONDK_WINDOW_Position_e ePosition,HUINT32 priority)
{
	g_ulPreventedDrawPriority = priority;

	/* Ư�� priority�� draw�� �������Ƿ� ��ü������ ȭ���� refresh�Ѵ�. */
	// GWM_APP_InvalidateRect(0, 0, CONFIG_MWC_OSD_HORIZONTAL, CONFIG_MWC_OSD_VERTICAL);

	/**
	 * ���� ���� �� ���, app area�� x1, y1�� ���� 0�� �ƴ� ���, �����ǥ�� ȭ�� ������
	 * �ϱ� ������,  �ǵ��ʹ� �޸� ��� ȭ���� ���ŵ��� ���� �� �ִ�.
	 */
	INT_ONDK_GWM_SetUpdateArea(ePosition,ONDK_Region(0, 0, GWM_SCREEN_WIDTH - 1, GWM_SCREEN_HEIGHT - 1));
	INT_ONDK_GWM_SetHaveToRedraw(ePosition,TRUE);

	return ERR_OK;
}

void		ONDK_GWM_APP_DisablePreventDraw(ONDK_WINDOW_Position_e ePosition)
{
	g_ulPreventedDrawPriority = 0;

	/* Ư�� priority�� draw�� �������Ƿ� ��ü������ ȭ���� refresh�Ѵ�. */
	// GWM_APP_InvalidateRect(0, 0, CONFIG_MWC_OSD_HORIZONTAL, CONFIG_MWC_OSD_VERTICAL);

	/**
	 * ���� ���� �� ���, app area�� x1, y1�� ���� 0�� �ƴ� ���, �����ǥ�� ȭ�� ������
	 * �ϱ� ������,  �ǵ��ʹ� �޸� ��� ȭ���� ���ŵ��� ���� �� �ִ�.
	 */
	INT_ONDK_GWM_SetUpdateArea(ePosition,ONDK_Region(0, 0, GWM_SCREEN_WIDTH - 1, GWM_SCREEN_HEIGHT - 1));
	INT_ONDK_GWM_SetHaveToRedraw(ePosition,TRUE);
}

HUINT32		ONDK_GWM_APP_GetPriority_PreventDraw(void)
{
	return g_ulPreventedDrawPriority;
}


/*
	�ڽ� �̿��� ���ÿ� ���� UI flag�� �ٲ� ������ ���� ����.
	�ڽ��� ���� ���� hide�� flag�� �´� ������ �ϸ� �ȴ�.
*/
void		ONDK_GWM_APP_SetUIHideFlag(ONDK_GWM_AppUiHide_e eAppUiHideFlag)
{
	/* basic application�� �ƴ� ��쿡�� UI�� �ִٰ� �Ǵ��ϸ� �ȴ�. */
	if((eAppUiHideFlag >= GWM_UI_Hide_Never) && (eAppUiHideFlag < GWM_UI_Hide_Flag_Max))
	{
		g_pstCurrentApp->Hide_ableFlag = eAppUiHideFlag;
	}
}

HINT32		ONDK_GWM_APP_GetUIHideFlag(ONDK_GWM_App_t pInApp)
{
	if(pInApp == 0)
	{
		pInApp = g_pstCurrentApp;
	}

	return pInApp->Hide_ableFlag;
}

ONDK_Result_t	ONDK_GWM_APP_HideUI(void)
{
	ONDK_Region_t		area = {	GWM_SCREEN_OFFSET_X,
								GWM_SCREEN_OFFSET_Y,
								GWM_SCREEN_OFFSET_X + GWM_SCREEN_WIDTH - 1,
								GWM_SCREEN_OFFSET_Y + GWM_SCREEN_HEIGHT - 1,
							};

#if 0
	ONDK_Rect_t		rect;
#endif

	if(g_pstCurrentApp->Hide_ableFlag != GWM_UI_Hide_Never)
	{
		if(g_pstCurrentApp->UI_Hide_BySelf != FALSE)
		{
			return GWM_RET_ALREADY_HIDE;
		}


#if 0
		g_pstCurrentApp->UI_Hide_BySelf = TRUE;
		//ONDK_GWM_GFX_ClearScreenByRegion(ONDK_GWM_GetSurface(), &area);
		rect.x = g_pstCurrentApp->Area.x1;
		rect.y = g_pstCurrentApp->Area.y1;
		rect.w = g_pstCurrentApp->Area.x2 - g_pstCurrentApp->Area.x1 + 1;
		rect.h = g_pstCurrentApp->Area.y2 - g_pstCurrentApp->Area.y1 + 1;

		ONDK_GWM_APP_InvalidateRect(rect.x, rect.y, rect.w, rect.h);
#else

		ONDK_GWM_APP_GetAppArea(g_pstCurrentApp, &area);
		ONDK_GWM_GFX_ClearScreenByRegion(ONDK_GWM_GetSurface(g_pstCurrentApp->eWindowPosition), &area);
		ONDK_GWM_APP_InvalidateAPP(g_pstCurrentApp);

		g_pstCurrentApp->UI_Hide_BySelf = TRUE;

#endif /* CONFIG_GWM_APPCLIPAREA */
		return GWM_RET_OK;
	}

	return GWM_RET_CANNOT_HIDE;
}

ONDK_Result_t	ONDK_GWM_APP_ShowUI(void)
{
#if 0
	ONDK_Rect_t		rect;
#endif

	if(g_pstCurrentApp->UI_Hide_BySelf == TRUE)
	{
		g_pstCurrentApp->UI_Hide_BySelf = FALSE;

#if 0
		rect.x = g_pstCurrentApp->Area.x1;
		rect.y = g_pstCurrentApp->Area.y1;
		rect.w = g_pstCurrentApp->Area.x2 - g_pstCurrentApp->Area.x1 + 1;
		rect.h = g_pstCurrentApp->Area.y2 - g_pstCurrentApp->Area.y1 + 1;

		ONDK_GWM_APP_InvalidateRect(rect.x, rect.y, rect.w, rect.h);
#else
		ONDK_GWM_APP_InvalidateRect(GWM_SCREEN_OFFSET_X, GWM_SCREEN_OFFSET_Y, GWM_SCREEN_WIDTH, GWM_SCREEN_HEIGHT);
#endif

		return GWM_RET_OK;
	}

	return GWM_RET_ALREADY_SHOW;
}


/* current application�̳� �ƴϳĿ� ���� ���� self, others�� ������. */
HBOOL		ONDK_GWM_APP_GetState_UIHide(ONDK_GWM_App_t pInApp)
{
	HBOOL			bFound = FALSE;
	ONDK_GWM_App_t		appList;

	/* � ������ ������ ��� */
	if((pInApp == 0) || (pInApp == g_pstCurrentApp))
	{
		pInApp = g_pstCurrentApp;
		if(NULL == pInApp)
		{
			return FALSE;
		}
		return ((pInApp->UI_Hide_BySelf) || (pInApp->UI_Hide_ByOther));
	}
	else
	{
		appList = (ONDK_GWM_App_t)g_pstAppRoot;
		while(appList != NULL)
		{
			if (appList == pInApp)		/* application stack�� �ִ� app���� �˻��� �� �����Ѵ�. */
			{
				bFound = TRUE;
				break;
			}
			appList = (ONDK_GWM_App_t)gwm_lnklst_GetNext((LINK_OBJECT)appList);
		}

		/* ������ ������ ã�� ���ߴ�. */
		if(bFound == FALSE)
		{
			ONDK_Error("[%s:%d] can't find application\r\n", __FUNCTION__, __LINE__);
			return FALSE;
		}
	}

	return ((pInApp->UI_Hide_BySelf) || (pInApp->UI_Hide_ByOther));
}

HBOOL		ONDK_GWM_APP_GetSelfState_UIHide(ONDK_GWM_App_t pInApp)
{
	HBOOL			bFound = FALSE;
	ONDK_GWM_App_t		appList;

	if((pInApp == 0) || (pInApp == g_pstCurrentApp))
	{
		pInApp = g_pstCurrentApp;

		return pInApp->UI_Hide_BySelf;
	}
	else
	{
		/* � ������ ������ ��� */
		appList = (ONDK_GWM_App_t)g_pstAppRoot;
		while(appList != NULL)
		{
			if (appList == pInApp)		/* application stack�� �ִ� app���� �˻��� �� �����Ѵ�. */
			{
				bFound = TRUE;
				break;
			}
			appList = (ONDK_GWM_App_t)gwm_lnklst_GetNext((LINK_OBJECT)appList);
		}

		/* ������ ������ ã�� ���ߴ�. */
		if(bFound == FALSE)
		{
			return FALSE;
		}
	}

	return pInApp->UI_Hide_BySelf;
}

HBOOL		ONDK_GWM_APP_GetOtherState_UIHide(ONDK_GWM_App_t pInApp)
{
	HBOOL			bFound = FALSE;
	ONDK_GWM_App_t		appList;

	if((pInApp == 0) || (pInApp == g_pstCurrentApp))
	{
		pInApp = g_pstCurrentApp;

		return pInApp->UI_Hide_ByOther;
	}
	else
	{
		/* � ������ ������ ��� */
		appList = (ONDK_GWM_App_t)g_pstAppRoot;
		while(appList != NULL)
		{
			if (appList == pInApp)		/* application stack�� �ִ� app���� �˻��� �� �����Ѵ�. */
			{
				bFound = TRUE;
				break;
			}
			appList = (ONDK_GWM_App_t)gwm_lnklst_GetNext((LINK_OBJECT)appList);
		}

		/* ������ ������ ã�� ���ߴ�. */
		if(bFound == FALSE)
		{
			return FALSE;
		}
	}

	return pInApp->UI_Hide_ByOther;
}


ONDK_Result_t	ONDK_GWM_APP_SetUIHide(ONDK_GWM_App_t pInApp)
{
	HBOOL			bFound = FALSE;
	ONDK_GWM_App_t		appList = NULL;
//	HINT32				i = 0;
	ONDK_Region_t		area = {	GWM_SCREEN_OFFSET_X,
								GWM_SCREEN_OFFSET_Y,
								GWM_SCREEN_OFFSET_X + GWM_SCREEN_WIDTH - 1,
								GWM_SCREEN_OFFSET_Y + GWM_SCREEN_HEIGHT - 1,
							};

	if((pInApp == 0) || (pInApp == g_pstCurrentApp))
	{
		/* �ڱ� �ڽ��� ��� */
		pInApp = g_pstCurrentApp;

		if((pInApp->Attribute == GWM_Basic_App)
			|| (pInApp->Hide_ableFlag == GWM_UI_Hide_Never)
			|| (pInApp->Hide_ableFlag & GWM_UI_Hide_Only_Self))
		{
			/* �ڱ� �ڽ��� Hide���� ���ϵ��� �� ����
				exception list�� �����ϴ� ���� ���.
				�����Ǿ� �־ �˻����� �ʴ´�.
			*/
			return GWM_RET_CANNOT_HIDE;
		}

		if(pInApp->UI_Hide_BySelf == TRUE)
		{
			return GWM_RET_ALREADY_HIDE;
		}

		/* Hide Setting �Ǳ����� Invalidate�ؾ� ��. */
		ONDK_GWM_APP_GetAppArea(pInApp, &area);
		ONDK_GWM_GFX_ClearScreenByRegion(ONDK_GWM_GetSurface(pInApp->eWindowPosition), &area);
		ONDK_GWM_APP_InvalidateAPP(pInApp);

		//pInApp->UI_Hide_BySelf++;
		pInApp->UI_Hide_BySelf = TRUE;
	}
	else
	{
		/* � ������ ������ ��� */
		appList = (ONDK_GWM_App_t)g_pstAppRoot;
		while(appList != NULL)
		{
			if (appList == pInApp)		/* application stack�� �ִ� app���� �˻��� �� �����Ѵ�. */
			{
				bFound = TRUE;

				if((pInApp->Attribute == GWM_Basic_App)
					|| (pInApp->Hide_ableFlag == GWM_UI_Hide_Never)
					|| (pInApp->Hide_ableFlag & GWM_UI_Hide_Only_Others))
				{
#if 0
					/* flag�� �����Ǿ� ������ excetion list�� �ִٸ� hide�� �� �ִ�. */
					if((pInApp->hideExceptionAppCount > 0) && (pInApp->pHideExceptionAppList != NULL))
					{
						for(i = 0; i < pInApp->hideExceptionAppCount; i++)
						{
							/* parameter�� ������ app�� exception list��
								hide ��Ű���� �ϴ� current app�� ����Ͽ��� ��� */
							if(pInApp->pHideExceptionAppList[i] == g_pstCurrentApp)
							{
								goto SETUiAPP_Hide_SUCCESS;
							}
						}
					}
#endif

					return GWM_RET_CANNOT_HIDE;
				}

				break;
			}
			appList = (ONDK_GWM_App_t)gwm_lnklst_GetNext((LINK_OBJECT)appList);
		}

		/* ������ ������ ã�� ���ߴ�. */
		if(bFound == FALSE)
		{
			return GWM_RET_NO_OBJECT;
		}

//SETUiAPP_Hide_SUCCESS:
		if(pInApp->UI_Hide_ByOther == TRUE)
		{
			return GWM_RET_ALREADY_HIDE;
		}

		/* Hide Setting �Ǳ����� Invalidate�ؾ� ��. */
		ONDK_GWM_APP_GetAppArea(pInApp, &area);
		ONDK_GWM_GFX_ClearScreenByRegion(ONDK_GWM_GetSurface(pInApp->eWindowPosition), &area);
		ONDK_GWM_APP_InvalidateAPP(pInApp);

		//pInApp->UI_Hide_ByOther++;
		pInApp->UI_Hide_ByOther = TRUE;
		if(g_pstCurrentApp == NULL)
			g_pstCurrentApp = pInApp;

		g_pstCurrentApp->CountOfCallHideOthers++;
	}

	return GWM_RET_OK;
}


void	ONDK_GWM_APP_SetUIHide_All(ONDK_WINDOW_Position_e eWindowPosition)
{
	ONDK_GWM_App_t		appList;
    ONDK_Region_t       temp;
	HINT32				i = 0;
	ONDK_Region_t		area = {	GWM_SCREEN_OFFSET_X, GWM_SCREEN_OFFSET_Y,
									GWM_SCREEN_OFFSET_X + GWM_SCREEN_WIDTH - 1, GWM_SCREEN_OFFSET_Y + GWM_SCREEN_HEIGHT - 1};
	g_bIsAllHide_Flag = TRUE;
	g_stAllHideApp = g_pstCurrentApp;

	/* � ������ ������ ��� */
	appList = (ONDK_GWM_App_t)g_pstAppRoot;
	while(appList != NULL)
	{
		if(appList == g_pstCurrentApp)
		{
			if((appList->Attribute == GWM_Basic_App)
				|| (appList->Hide_ableFlag == GWM_UI_Hide_Never)
				|| (appList->Hide_ableFlag & GWM_UI_Hide_Only_Self))
			{
				goto SetUiApp_AllHide_CONTINUE;
			}

			//appList->UI_Hide_BySelf++;
			appList->UI_Hide_BySelf = TRUE;
		}
		else
		{
			if((appList->Attribute == GWM_Basic_App)
				|| (appList->Hide_ableFlag == GWM_UI_Hide_Never)
				|| (appList->Hide_ableFlag & GWM_UI_Hide_Only_Others))
			{
				if(appList->hideExceptionAppCount > 0)
				{
					for(i = 0; i < appList->hideExceptionAppCount; i++)
					{
						/* parameter�� ������ app�� exception list��
								hide ��Ű���� �ϴ� current app�� ����Ͽ��� ��� */
						if(appList->pHideExceptionAppList[i] == g_pstCurrentApp)
						{
							//appList->UI_Hide_ByOther++;
							appList->UI_Hide_ByOther = TRUE;

							goto SetUiApp_AllHide_CONTINUE;
						}
					}
				}

				goto SetUiApp_AllHide_CONTINUE;
			}

			//appList->UI_Hide_ByOther++;
			appList->UI_Hide_ByOther = TRUE;
		}

SetUiApp_AllHide_CONTINUE:
		appList = (ONDK_GWM_App_t)gwm_lnklst_GetNext((LINK_OBJECT)appList);
	}

	if(g_pstCurrentApp)
		g_pstCurrentApp->CountOfCallHideOthers++;

	//ONDK_GWM_GFX_ClearScreenByRegion(ONDK_GWM_GetSurface(eWindowPosition), &area);
    //temp = INT_ONDK_GWM_GetUpdateArea(eWindowPosition);
	INT_ONDK_GWM_SetUpdateArea(eWindowPosition, ONDK_UnionRegion(&area, &temp));
}

ONDK_Result_t	ONDK_GWM_APP_SetUIHide_AllexceptMe(ONDK_WINDOW_Position_e eWindowPosition)
{
	ONDK_GWM_App_t		appList;
	ONDK_Region_t		area = {	GWM_SCREEN_OFFSET_X,
								GWM_SCREEN_OFFSET_Y,
								GWM_SCREEN_OFFSET_X + GWM_SCREEN_WIDTH - 1,
								GWM_SCREEN_OFFSET_Y + GWM_SCREEN_HEIGHT - 1,
							};
    ONDK_Region_t       temp;

	/* � ������ ������ ��� */
	appList = (ONDK_GWM_App_t)g_pstAppRoot;
	while(appList != NULL)
	{
		if(appList != g_pstCurrentApp)
		{
			if((appList->Attribute == GWM_Basic_App)
				|| (appList->Hide_ableFlag == GWM_UI_Hide_Never)
				|| (appList->Hide_ableFlag & GWM_UI_Hide_Only_Others))
			{
#if 0
				if(appList->hideExceptionAppCount > 0)
				{

					HINT32		i = 0;

					for(i = 0; i < appList->hideExceptionAppCount; i++)
					{
						if(appList->pHideExceptionAppList[i] == g_pstCurrentApp)
						{
							//appList->UI_Hide_ByOther++;
							appList->UI_Hide_ByOther = TRUE;

							goto SetUiApp_AllHide_ExME_CONTINUE;
						}
					}
				}
#endif

				goto SetUiApp_AllHide_ExME_CONTINUE;
			}

			//appList->UI_Hide_ByOther++;
			appList->UI_Hide_ByOther = TRUE;
		}

SetUiApp_AllHide_ExME_CONTINUE:
		appList = (ONDK_GWM_App_t)gwm_lnklst_GetNext((LINK_OBJECT)appList);
	}

	g_pstCurrentApp->CountOfCallHideOthers++;
	INT_ONDK_GWM_SetUpdateArea(eWindowPosition, ONDK_UnionRegion(&area,&temp));
	return GWM_RET_OK;
}

void	ONDK_GWM_APP_SetUIHide_AllLowerPriority(ONDK_WINDOW_Position_e eWindowPosition,HINT32 priority)
{
	ONDK_GWM_App_t		appList;
	HINT32				curPriority = -1;
	ONDK_Region_t		area = {	GWM_SCREEN_OFFSET_X,
								GWM_SCREEN_OFFSET_Y,
								GWM_SCREEN_OFFSET_X + GWM_SCREEN_WIDTH - 1,
								GWM_SCREEN_OFFSET_Y + GWM_SCREEN_HEIGHT - 1,
							};
    ONDK_Region_t       temp;

	/* parameter�� 0���� ������ current application�� priority�� �۵��Ѵ�. */
	appList = (ONDK_GWM_App_t)g_pstAppRoot;

	if(priority >= 0)
	{
		curPriority = priority;
	}
	else
	{
		/* current application��  priority�� ã�´�. */
		appList = (ONDK_GWM_App_t)g_pstCurrentApp;
		curPriority = appList->priority;

		/* current ���� ���ú��� �˻��Ѵ�. */
		appList = (ONDK_GWM_App_t)gwm_lnklst_GetNext((LINK_OBJECT)appList);
	}

	while(appList != NULL)
	{
		if(appList->priority < curPriority)
		{
			if((appList->Attribute == GWM_Basic_App)
				|| (appList->Hide_ableFlag == GWM_UI_Hide_Never)
				|| (appList->Hide_ableFlag & GWM_UI_Hide_Only_Others))
			{
#if 0
				if(appList->hideExceptionAppCount > 0)
				{

					HINT32		i = 0;

					for(i = 0; i < appList->hideExceptionAppCount; i++)
					{
						if(appList->pHideExceptionAppList[i] == g_pstCurrentApp)
						{
							//appList->UI_Hide_ByOther++;
							appList->UI_Hide_ByOther = TRUE;

							goto SetUiApp_AllHide_ExME_CONTINUE;
						}
					}
				}
#endif

				goto SetUiApp_AllHide_LowerPrior_CONTINUE;
			}

			//appList->UI_Hide_ByOther++;
			appList->UI_Hide_ByOther = TRUE;
		}

SetUiApp_AllHide_LowerPrior_CONTINUE:
		appList = (ONDK_GWM_App_t)gwm_lnklst_GetNext((LINK_OBJECT)appList);
	}

	g_pstCurrentApp->CountOfCallHideOthers++;
	ONDK_GWM_GFX_ClearScreenByRegion(ONDK_GWM_GetSurface(eWindowPosition), &area);
    temp = INT_ONDK_GWM_GetUpdateArea(eWindowPosition);
	INT_ONDK_GWM_SetUpdateArea(eWindowPosition,ONDK_UnionRegion(&area, &temp));
}

ONDK_Result_t	ONDK_GWM_APP_SetUIShow(ONDK_GWM_App_t pInApp)
{
	HBOOL			bFound = FALSE;
	ONDK_GWM_App_t		appList;
#if 0
	ONDK_Region_t		area;
	ONDK_Rect_t		rect;
#endif

	if((pInApp == 0) || (pInApp == g_pstCurrentApp))
	{
		/* �ڱ� �ڽ��� ��� */
		pInApp = g_pstCurrentApp;

		if(pInApp->UI_Hide_BySelf < 0)
		{
			return GWM_RET_CANNOT_SHOW;
		}
		else if(pInApp->UI_Hide_BySelf == 0)
		{
			return GWM_RET_ALREADY_SHOW;
		}

		//pInApp->UI_Hide_BySelf--;
		pInApp->UI_Hide_BySelf = FALSE;
	}
	else
	{
		/* � ������ ������ ��� */
		appList = (ONDK_GWM_App_t)g_pstAppRoot;
		while(appList != NULL)
		{
			if (appList == pInApp)		/* application stack�� �ִ� app���� �˻��� �� �����Ѵ�. */
			{
				bFound = TRUE;

				break;
			}
			appList = (ONDK_GWM_App_t)gwm_lnklst_GetNext((LINK_OBJECT)appList);
		}

		/* ������ ������ ã�� ���ߴ�. */
		if(bFound == FALSE)
		{
			return GWM_RET_NO_OBJECT;
		}

		if(pInApp->UI_Hide_ByOther < 0)
		{
			return GWM_RET_CANNOT_SHOW;
		}
		else if(pInApp->UI_Hide_ByOther == 0)
		{
			return GWM_RET_ALREADY_SHOW;
		}

		//pInApp->UI_Hide_ByOther--;
		pInApp->UI_Hide_ByOther = FALSE;

	}

	if(g_pstCurrentApp == NULL)
		g_pstCurrentApp = pInApp;

	g_pstCurrentApp->CountOfCallHideOthers--;
	if(g_pstCurrentApp->CountOfCallHideOthers < 0)
	{
		GWM_assert(0);
		g_pstCurrentApp->CountOfCallHideOthers = 0;
	}

#if 0
	rect.x = g_pstCurrentApp->Area.x1;
	rect.y = g_pstCurrentApp->Area.y1;
	rect.w = g_pstCurrentApp->Area.x2 - g_pstCurrentApp->Area.x1 + 1;
	rect.h = g_pstCurrentApp->Area.y2 - g_pstCurrentApp->Area.y1 + 1;

	ONDK_GWM_APP_InvalidateRect(rect.x, rect.y, rect.w, rect.h);
#else
	ONDK_GWM_APP_InvalidateRect(GWM_SCREEN_OFFSET_X, GWM_SCREEN_OFFSET_Y, GWM_SCREEN_WIDTH, GWM_SCREEN_HEIGHT);
#endif

	return GWM_RET_OK;
}

void	ONDK_GWM_APP_SetUIShow_All(void)
{
	ONDK_GWM_App_t		appList;

	g_bIsAllHide_Flag = FALSE;
	g_stAllHideApp = NULL;

	/* � ������ ������ ��� */
	appList = (ONDK_GWM_App_t)g_pstAppRoot;
	while(appList != NULL)
	{
		if(appList == g_pstCurrentApp)
		{
			if(appList->UI_Hide_BySelf < 0)
			{
				GWM_assert(0);
				goto ALLSHOW_CONTINUE;
			}
			else if(appList->UI_Hide_BySelf == 0)
			{
				goto ALLSHOW_CONTINUE;
			}

			appList->UI_Hide_BySelf--;
		}
		else
		{
			if(appList->UI_Hide_ByOther < 0)
			{
				GWM_assert(0);
				goto ALLSHOW_CONTINUE;
			}
			else if(appList->UI_Hide_ByOther == 0)
			{
				goto ALLSHOW_CONTINUE;
			}

			//appList->UI_Hide_ByOther--;
			appList->UI_Hide_ByOther = FALSE;
		}

ALLSHOW_CONTINUE:
		appList = (ONDK_GWM_App_t)gwm_lnklst_GetNext((LINK_OBJECT)appList);
	}

	g_pstCurrentApp->CountOfCallHideOthers--;
	if(g_pstCurrentApp->CountOfCallHideOthers < 0)
	{
		GWM_assert(0);
		g_pstCurrentApp->CountOfCallHideOthers = 0;
	}

	ONDK_GWM_APP_InvalidateRect(GWM_SCREEN_OFFSET_X, GWM_SCREEN_OFFSET_Y, GWM_SCREEN_WIDTH, GWM_SCREEN_HEIGHT);
}

ONDK_Result_t	ONDK_GWM_APP_SetUIShow_AllexceptMe(void)
{
	ONDK_GWM_App_t		appList;

	g_bIsAllHide_Flag = FALSE;
	g_stAllHideApp = NULL;

	/* � ������ ������ ��� */
	appList = (ONDK_GWM_App_t)g_pstAppRoot;
	while(appList != NULL)
	{
		if(appList != g_pstCurrentApp)
		{
			if(appList->UI_Hide_ByOther < 0)
			{
				GWM_assert(0);
				goto ALLSHOW_ExMe_CONTINUE;
			}
			else if(appList->UI_Hide_ByOther == 0)
			{
				goto ALLSHOW_ExMe_CONTINUE;
			}

			//appList->UI_Hide_ByOther--;
			appList->UI_Hide_ByOther = FALSE;
		}

ALLSHOW_ExMe_CONTINUE:
		appList = (ONDK_GWM_App_t)gwm_lnklst_GetNext((LINK_OBJECT)appList);
	}

	g_pstCurrentApp->CountOfCallHideOthers--;
	if(g_pstCurrentApp->CountOfCallHideOthers < 0)
	{
		GWM_assert(0);
		g_pstCurrentApp->CountOfCallHideOthers = 0;
	}

	ONDK_GWM_APP_InvalidateRect(GWM_SCREEN_OFFSET_X, GWM_SCREEN_OFFSET_Y, GWM_SCREEN_WIDTH, GWM_SCREEN_HEIGHT);

	return GWM_RET_OK;
}


#define		_____ONDK_GWM_APP_INFO______

ONDK_GWM_App_t		ONDK_GWM_APP_Get(ONDK_GWM_Callback_t callback)
{
	ONDK_GWM_App_t		appList;

	if(callback != NULL)
	{
		appList = (ONDK_GWM_App_t)g_pstAppRoot;
		while(appList != NULL)
		{
			if (appList->callBack == callback)		// �̹� ���� �̸��� Application�� ������.
			{
				return	appList;
			}
			appList = (ONDK_GWM_App_t)gwm_lnklst_GetNext((LINK_OBJECT)appList);//(GWM_App_t)appList->link.nextObject;
		}
	}

	return NULL;
}

ONDK_GWM_App_t		ONDK_GWM_APP_GetThis(void)
{
	return g_pstCurrentApp;
}

ONDK_WINDOW_Position_e ONDK_GWM_APP_GetPosition(ONDK_GWM_App_t app)
{
	if (app != NULL)
	{
		return	app->eWindowPosition;
	}
	else
	{
		//HxLOG_Print("GWM_APP_GetAppPriority was called for INVALID application pointer\n");
		return	ONDK_MAX_WINDOW;
	}
}

HCHAR *ONDK_GWM_APP_GetAppName(ONDK_GWM_App_t app)
{
	if (app != NULL)
	{
		return	app->name;
	}
	else
	{
		return	"not exists app~!";
	}
}

void ONDK_GWM_APP_SetAppName(ONDK_GWM_App_t app, HCHAR *pszProcName)
{
	if (app == NULL)
	{
		return;
	}

	if ((pszProcName != NULL) && (pszProcName[0] != '\0'))
	{
		if(app->name[0] == '\0')
		{ /* ��� ������ ���� ���� ... */
			strncpy(app->name, pszProcName, DEF_APP_NAME_LENGTH - 1);		// RTTI ���ڿ��� ����.
			app->name[DEF_APP_NAME_LENGTH - 1] = '\0';
		}
		else
		{ /* �̹� ������ �� ����. */
		}
	}
	else
	{
		app->name[0] = '\0';
	}

}

HUINT32 ONDK_GWM_APP_GetPriority(ONDK_GWM_App_t app)
{
	if (app != NULL)
	{
		return	app->priority;
	}
	else
	{
		//HxLOG_Print("GWM_APP_GetAppPriority was called for INVALID application pointer\n");
		return	0;
	}
}

void	ONDK_GWM_APP_MoveAppArea(HINT32 x, HINT32 y, HINT32 w, HINT32 h)
{
	ONDK_Region_t	area,temp;

	GWM_assert(g_pstCurrentApp);

	ONDK_GWM_APP_GetAppArea(g_pstCurrentApp, &area);

    temp = INT_ONDK_GWM_GetUpdateArea(ONDK_GWM_APP_GetPosition(g_pstCurrentApp));
	INT_ONDK_GWM_SetUpdateArea(ONDK_GWM_APP_GetPosition(g_pstCurrentApp), ONDK_UnionRegion(&area, &temp));
	INT_ONDK_GWM_SetHaveToRedraw(ONDK_GWM_APP_GetPosition(g_pstCurrentApp),TRUE);

	ONDK_GWM_APP_SetAppArea(x, y, w, h);
}

ONDK_GWM_Callback_t	ONDK_GWM_APP_GetAppCallback(ONDK_GWM_App_t app)
{
	if (app != NULL)
	{
		return app->callBack;
	}
	else
	{
		GWM_assert(0);
		return NULL;
	}
}

ONDK_GWM_App_t		ONDK_GWM_APP_GetNextApp(ONDK_GWM_App_t app)
{
	if (app != NULL)
		return	(ONDK_GWM_App_t)gwm_lnklst_GetNext((LINK_OBJECT)app);//app->link.nextObject;
	else
		return	NULL;
}

ONDK_GWM_GuiObject_t	ONDK_GWM_APP_GetObject(HINT32 ID)
{
	if(g_pstCurrentApp)
	{
		return INT_ONDK_GWM_OBJ_FindObject(g_pstCurrentApp->ObjectList, ID);
	}
	return NULL;
}


void	ONDK_GWM_APP_GetAppArea(ONDK_GWM_App_t app, ONDK_Region_t *area)
{
	if (app != NULL)
	{
		VK_memcpy(area, &(app->Area), sizeof(ONDK_Region_t));
		return;
	}
	VK_memset(area, 0, sizeof(ONDK_Region_t));
	return;
}

void 	ONDK_GWM_APP_GetAppClipArea(ONDK_GWM_App_t app, ONDK_Region_t *area)
{
	if (app != NULL)
	{
		VK_memcpy(area, &(app->ClipArea), sizeof(ONDK_Region_t));
		return;
	}
	VK_memset(area, 0, sizeof(ONDK_Region_t));
	return;
}

void	ONDK_GWM_APP_SetAppArea(HINT32 x, HINT32 y, HINT32 w, HINT32 h)
{
	GWM_assert(g_pstCurrentApp);

	g_pstCurrentApp->Area.x1 = x;
	g_pstCurrentApp->Area.y1 = y;
	g_pstCurrentApp->Area.x2 = x+w-1;
	g_pstCurrentApp->Area.y2 = y+h-1;

	g_pstCurrentApp->UpdateArea.x1 = 0;		g_pstCurrentApp->UpdateArea.y1 = 0;
	g_pstCurrentApp->UpdateArea.x2 = 0;		g_pstCurrentApp->UpdateArea.y2 = 0;

	/* set the same clip region as the area */
	g_pstCurrentApp->ClipArea = g_pstCurrentApp->Area;
}

void 	ONDK_GWM_APP_SetAppClipArea(HINT32 x, HINT32 y, HINT32 w, HINT32 h)
{
	g_pstCurrentApp->ClipArea.x1 = x; 	g_pstCurrentApp->ClipArea.y1 = y;
	g_pstCurrentApp->ClipArea.x2 = x+w-1; 	g_pstCurrentApp->ClipArea.y2 = y+h-1;
}


GWM_Proc_t	ONDK_GWM_APP_ChangeObjectProc(HINT32 ID, GWM_Proc_t new_procfunc)
{
	ONDK_GWM_GuiObject_t		objlist, obj;

	GWM_assert(g_pstCurrentApp);
	objlist = g_pstCurrentApp->ObjectList;

	obj = INT_ONDK_GWM_OBJ_FindObject(objlist, ID);
	if (obj == NULL)
		return NULL;
	return	INT_ONDK_GWM_OBJ_ChangeObjectProc(obj, new_procfunc);
}

ONDK_Result_t		ONDK_GWM_APP_SetObjectDrawMethod(HINT32 ID, GWM_ObjDraw_t objDrawFunc)
{
	ONDK_GWM_GuiObject_t		objlist, obj;

	GWM_assert(g_pstCurrentApp);
	objlist = g_pstCurrentApp->ObjectList;

	obj = INT_ONDK_GWM_OBJ_FindObject(objlist, ID);
	if (obj == NULL)
		return ERR_FAIL;
	INT_ONDK_GWM_OBJ_SetDrawMethod(obj, objDrawFunc);

	return ERR_OK;
}


#define		_____ONDK_GWM_APP_STACK_____

ONDK_GWM_App_t		ONDK_GWM_APP_GetUITop(void)
{
	HINT32 		ui_stackOrder = -1;
	ONDK_GWM_App_t	curApp = NULL;

	/* app root���� �Ʒ��� Ÿ�� ��������. */
	curApp = g_pstAppRoot;

	/* -_- ������ ����. */
	if(curApp == NULL)
	{
		return NULL;
	}

	/* */
	local_gwmapp_CheckAppUiOrderStacking(g_pstAppRoot);

	/* ���� ���� ������ UI�� ���� Ȯ���� ����. */
	do
	{
		/* UI stack order == 0�� ������ top most �����̴�. */
		if ( (ui_stackOrder = INT_ONDK_GWM_APP_GetUiStackOrder(curApp)) == 0)
		{
			return curApp;
		}

		/* get next app */
	} while( (curApp = ONDK_GWM_APP_GetNextApp(curApp)) != NULL);

	/* ���� ���Ҵµ� 0�� ���ٸ� UI�� ���� app�� �ִ� ��, �Ǵ� �ƿ� app�� ����. -_- */
	return NULL;
}


ONDK_GWM_App_t		ONDK_GWM_APP_GetAppTop(void)
{
	return g_pstAppRoot;
}

HERROR ONDK_GWM_APP_SwapStackOrder(ONDK_GWM_Callback_t proc1, ONDK_GWM_Callback_t proc2)
{
	ONDK_GWM_App_t		app1, app2, temp;
	ONDK_GWM_App_t		preUpper_App = NULL, preLower_App = NULL, iterator = NULL;
	ONDK_GWM_App_t		upperApp = NULL, lowerApp = NULL;
	ONDK_GWM_App_t		*pPreUpper_Link = NULL, *pPreLower_Link = NULL;
	LINK_OBJECT		backup_upperLink, backup_lowerlink;
	HBOOL			bNeedRootAppUpdate = FALSE;

	/* parameter check */
	if(proc1 == NULL)
	{
		app1 = ONDK_GWM_APP_GetThis();
	}
	else
	{
		app1 = ONDK_GWM_APP_Get(proc1);
	}

	if(proc2 == NULL)
	{
		app2 = ONDK_GWM_APP_GetThis();
	}
	else
	{
		app2 = ONDK_GWM_APP_Get(proc2);
	}

	if( (app1 == NULL) || (app2 == NULL) )
	{
		return ERR_FAIL;
	}

	/* ���ð� ���� ���踦 �����Ѵ�. , stack count �� ���� ���� ����. */
	if(app1->stack_count > app2->stack_count)
	{
		upperApp = app1;
		lowerApp = app2;
	}
	else
	{
		upperApp = app2;
		lowerApp = app1;
	}

	/* upperlink, pre upper app�� ã�´�. */
	if(g_pstAppRoot == upperApp)
	{
		/* ���� upper app�� g_pstAppRoot �̶�� */
		pPreUpper_Link = &(g_pstAppRoot);
		preUpper_App = g_pstAppRoot;

		/* ���߿� root�� ������Ʈ ���־�� �Ѵ�. */
		bNeedRootAppUpdate = TRUE;
	}
	else
	{
		iterator = g_pstAppRoot;
		while(iterator)
		{
			if(iterator->link == (LINK_OBJECT) upperApp)
			{
				preUpper_App = iterator;
				pPreUpper_Link = (ONDK_GWM_App_t *)&(preUpper_App->link);
				break;
			}
			iterator = (ONDK_GWM_App_t)gwm_lnklst_GetNext((LINK_OBJECT)iterator);
		}
	}

	/* lowerlink, pre lower app�� ã�´�. */
	iterator = g_pstAppRoot;
	while(iterator)
	{
		if(iterator->link == (LINK_OBJECT) lowerApp)
		{
			preLower_App = iterator;
			pPreLower_Link = (ONDK_GWM_App_t *)&(preLower_App->link);
			break;
		}
		iterator = (ONDK_GWM_App_t)gwm_lnklst_GetNext((LINK_OBJECT)iterator);
	}

	/* �� app�� link�� back up �Ѵ�. */
	backup_upperLink = upperApp->link;
	backup_lowerlink = lowerApp->link;

	/* app ���� swap */
	temp = upperApp;
	upperApp = lowerApp;
	lowerApp = temp;

	/* ���� ������� �����Ѵ�. */

	/* upper app�� ���� app�� link ������ ���� �Ѵ�. lower�� ���� �� ������ ������. */
	if(pPreUpper_Link == NULL)
	{
		/* ���� app�� �� ������ gAppRoot�� �����Ƿ�, ������ �ȵǴ� �κ��̸�,
			�� ������ �߻��Ǿ��ٸ� �ɰ��� app ���� ������ trace �غ��� �Ѵ�.  */
		GWM_assert(0);
		GWM_DBG_Print(DBGMSG_ERROR, ("Cant Known Upper APP in APP stack swapping!! M.U.S.T Check~!\n"));
	}
	else
	{
		*pPreUpper_Link = upperApp;
	}

	/* ����� �ξ��� pre lower app�� link�� ���ҵ� upper�� ���ٸ�
		upper�� link�� ���ҵ� lower app�̴�. */
	if(backup_upperLink == (LINK_OBJECT) upperApp)
	{
		upperApp->link = (LINK_OBJECT) lowerApp;

		/* pre lower app�� link�� ������ �ʿ䰡 ����. */
	}
	else
	{
		/* swap�� app ���뿡 link ���� */
		upperApp->link = (LINK_OBJECT) backup_upperLink;

		/* pre lower app�� link�� �����Ѵ�.
			pre lower�� last ��ġ�� ���� �� �����Ƿ� NULL�� ��쿣 �ȳ־ �ȴ�.
			�׷���, �������̶�� trace �ʿ�~ */
		if(pPreLower_Link != NULL)
		{
			*pPreLower_Link = lowerApp;
		}
	}

	/* �׻� lower app�� link�� ����� ������ �����Ѵ�. */
	lowerApp->link = (LINK_OBJECT) backup_lowerlink;

	/* gAppRoot���� �ݿ� */
	if(bNeedRootAppUpdate == TRUE)
	{
		/* ó���� app1�� �������Ƿ� �ٲ� app1�� �־��ش�. */
		g_pstAppRoot = upperApp;
	}

	// new app�� linked list�� �߰��� ��, app tree�� traverse�ϸ鼭 stacking order�� ������ ��.
	local_gwmapp_OrderAppStacking(g_pstAppRoot);

	// new app�� linked list�� �߰��� ��, UI object�� ������ٸ� UI order�� üũ�Ѵ�.
	local_gwmapp_CheckAppUiOrderStacking(g_pstAppRoot);

	// new app �ݿ��� ���� invalidate
	ONDK_GWM_APP_InvalidateRect (0, 0, GWM_SCREEN_WIDTH, GWM_SCREEN_HEIGHT);

	INT_ONDK_GWM_SetStackOderChanged(TRUE);

	return ERR_OK;
}

#define		_____ONDK_GWM_APP_CREATE_DELETE_____

ONDK_Result_t		ONDK_GWM_APP_Create(ONDK_WINDOW_Position_e eWindowPosition,HCHAR *app_name, ONDK_GWM_Priority_t ulProperty, ONDK_GWM_Callback_t callback,
									HINT32 handle, HINT32 param1, HINT32 param2, HINT32 param3)
{
	HINT32				nPriority;
	HUINT32			ulUiState, ulParentUiState;
	ONDK_GWM_App_t		this_app, new_app;
	ONDK_GWM_Callback_t		this_callback;
	ONDK_Result_t		hresult;
	HINT32			len = 0;

	ONDK_Assert(ONDK_GWM_IsGwmTask() == TRUE); // <- we have two thread GWM and key

	if (callback == NULL)
	{
		return ERR_FAIL;
	}

	this_callback = NULL;
	ulParentUiState = 0;

	this_app = ONDK_GWM_APP_GetThis();
	if (this_app != NULL)
	{
		this_callback = ONDK_GWM_APP_GetAppCallback (this_app);
		ulParentUiState = this_app->ulUiState;
	}

	len = (app_name != NULL) ? strlen(app_name) : 0;
	if (len == 0)
	{
		//HxLOG_Print ("[GWM_APP_Create] application name is NULL or No string.\n");
	}

	// ����, �̹� ���� �̸��� Application�� �����ϰ� �ִ��� Ȯ���Ѵ�.
	new_app = ONDK_GWM_APP_Get(callback);
	if (new_app != NULL)					// �̹� ���� �̸��� Application�� ���� ���̴�.
	{
		/* MSG_GWM_CREATE�� ������ ��������, parameter�� �״�� ���� �ش�. */
		ONDK_GWM_APP_Call((ONDK_GWM_App_t)ONDK_GWM_APP_Get(callback), MSG_GWM_FAIL_RECREATE_NEW_INSTANCE, (Handle_t)NULL, param1, param2, param3, NULL);
		return GWM_RET_LINK_CONFLICT;
	}

	// ���ο� application�� �޸𸮸� �Ҵ�.
	new_app = local_gwmapp_AllocApp();
	if (new_app == NULL)			{ return GWM_RET_OUT_OF_MEMORY; }

	// �⺻�Ӽ��� ����.
	new_app->link = NULL;

	if (len > 0)
	{
		strncpy (new_app->name, app_name, DEF_APP_NAME_LENGTH - 1);		// RTTI ���ڿ��� ����.
		new_app->name[DEF_APP_NAME_LENGTH - 1] = '\0';
	}
	else
	{
		new_app->name[0] = '\0';
	}

	nPriority	= (HINT32)(ulProperty & GWM_MASK_PRIORITY);
	ulUiState	= (HUINT32)(ulProperty & GWM_MASK_UISTATE);
	if (ulUiState == UISTATE_INHERITANCE)
	{
		ulUiState = ulParentUiState;
	}

	new_app->priority	= nPriority;
	new_app->ulUiState	= ulUiState;
	new_app->callBack	= callback;
	new_app->Area.x1 = 0;					new_app->Area.y1 = 0;
	new_app->Area.x2 = 0;					new_app->Area.y2 = 0;
	new_app->ClipArea.x1 = 0;				new_app->Area.y1 = 0;
	new_app->ClipArea.x2 = 0;				new_app->Area.y2 = 0;
	new_app->UpdateArea.x1 = 0;				new_app->UpdateArea.y1 = 0;
	new_app->UpdateArea.x2 = 0;				new_app->UpdateArea.y2 = 0;
	new_app->ObjectList = NULL;
	new_app->Attribute = GWM_Basic_App;

	/* hide/show ���� */
	new_app->Hide_ableFlag = GWM_UI_Hide_Default;				/**< hide�� ���� �Ӽ��� �����Ѵ�. */
	new_app->UI_Hide_BySelf = FALSE;				/**< �ڱ� ������ Hide �ߴ����� flag, counter�� �� �� ������ �ڱ� �ڽ��� ȥ�� �̹Ƿ� max�� 1 */
	new_app->UI_Hide_ByOther = FALSE;			/**< ���鿡 ���� Hide�Ǿ��� ���� flag. counter�� �� �� ������ max�� ���� �� �ִ� ���ø����̼� �� */
	new_app->CountOfCallHideOthers = 0;			/**< for sync show/hide */
	new_app->hideExceptionAppCount = 0;
	new_app->pHideExceptionAppList = NULL;
	new_app->eWindowPosition	= eWindowPosition; /** ��� �����쿡 app �� ���� ���ΰ� **/
#if 0
	new_app->pstLoadedImgLink = NULL;
#endif

	// Linked List(g_pstAppRoot)�� �߰�.
	hresult = gwm_lnklst_AddItemWithCompareDescend( (LINK_OBJECT*)&g_pstAppRoot, (LINK_OBJECT)new_app, local_gwmapp_CompareAppPriority);

	if (hresult != GWM_RET_OK)
		return hresult;

	// new app�� linked list�� �߰��� ��, app tree�� traverse�ϸ鼭 stacking order�� ������ ��.
	local_gwmapp_OrderAppStacking(g_pstAppRoot);

	// ���ο� Application�� MSG_GWM_CREATE �޼��� ����.
	ONDK_GWM_APP_Call(new_app, MSG_GWM_CREATE, handle, param1, param2, param3, NULL);

	// new app�� linked list�� �߰��� ��, UI object�� ������ٸ� UI order�� üũ�Ѵ�.
	local_gwmapp_CheckAppUiOrderStacking(g_pstAppRoot);

	// new app �ݿ��� ���� invalidate
	ONDK_GWM_APP_InvalidateRect (0, 0, GWM_SCREEN_WIDTH, GWM_SCREEN_HEIGHT);

	INT_ONDK_GWM_SetStackOderChanged(TRUE);

	// Create�� ���� ���� ��� Application�� �� Application�� �����Ǿ����� �˸���.
	// p1 : application callback
	ONDK_GWM_SendMessage (NULL, MSG_GWM_APP_SPAWNED, handle, (HINT32)callback, (HINT32)this_callback, 0);

	return	hresult;
}


ONDK_Result_t			ONDK_GWM_APP_Destroy(ONDK_GWM_Callback_t callBack)
{
	ONDK_GWM_App_t		delete_app, AppBackupPtr;
	ONDK_GWM_Callback_t	delete_app_callback;

	//ONDK_Assert(ONDK_GWM_IsGwmTask() == TRUE);

	// �����Ǵ� ����� � Application���� ���� Ȯ��.
	if (callBack == NULL)
	{
		delete_app = g_pstCurrentApp;
	}
	else
	{
		delete_app = ONDK_GWM_APP_Get(callBack);
	}

	if (delete_app == NULL)			// �����Ϸ��� Application�� �������� �ƴϴ�.
	{
		return GWM_RET_NO_OBJECT;
	}
	GWM_DBG_Print( DBGMSG_TRACE, ("Destroying... %s\n", delete_app->name));

	delete_app_callback = ONDK_GWM_APP_GetAppCallback (delete_app);

	AppBackupPtr = g_pstCurrentApp;
	g_pstCurrentApp = delete_app;

	// 1. ������ TIMER�� �ִٸ� ��� �����Ѵ�.
	INT_ONDK_GWM_TIMER_KillAllByAPP(delete_app);

	/* hide�� �θ� app�� ��� ������ ���õ��� ���� �� �� �ֵ��� ����� �Ѵ�. */
	if(delete_app == g_stAllHideApp)
	{
		g_bIsAllHide_Flag = FALSE;
		g_stAllHideApp = NULL;
	}

	local_gwmapp_UnlinkAppObject(delete_app);

	ONDK_ReleaseStringByProcId((HUINT32)delete_app);
	ONDK_IMG_DestroyByProcId((HUINT32)delete_app);

	g_pstCurrentApp = AppBackupPtr;

	// app stacking order�� ������
	local_gwmapp_OrderAppStacking(g_pstAppRoot);

	// app UI stacking order�� ������
	local_gwmapp_CheckAppUiOrderStacking(g_pstAppRoot);

	// �������� �ݿ�
	ONDK_GWM_APP_InvalidateRect (0, 0, GWM_SCREEN_WIDTH, GWM_SCREEN_HEIGHT);

	INT_ONDK_GWM_SetStackOderChanged(TRUE);

	// Delete �� ���� ���� ��� Application�� �� Application�� �����Ǿ����� �˸���.
	// p1 : application callback
	if (delete_app_callback != NULL)
	{
		ONDK_GWM_SendMessage (NULL, MSG_GWM_APP_KILLED, HANDLE_NULL, (HINT32)delete_app_callback, 0, 0);
	}

	return	GWM_RET_OK;
}

/*
 *	DestroyAll ������ �Լ��� ���� DESTROY�Ǵ� Application�� �Ǵٽ� DeatroyAll ������ �Լ��� ȣ���ϰ� �Ǹ�
 *  Recursive �ϰ� ȣ��Ǿ� System HangUp�� ���ɼ��� ����.
 */
STATIC	HINT32		s_nflagNowDestroyAll = 0;

ONDK_Result_t			ONDK_GWM_APP_DestroyAllAppAfter(ONDK_GWM_Callback_t callBack)
{
	ONDK_GWM_App_t		base_app = 0;
	ONDK_GWM_App_t		traverse = 0;
	ONDK_GWM_App_t		reverseAppList[DEF_GWM_MAX_APPLICATION] = {0, };
	HINT32				app_count = 0;

	if (s_nflagNowDestroyAll)
		return GWM_RET_NO_OBJECT;
	s_nflagNowDestroyAll = 1;

	if (callBack == NULL)
	{
		base_app = g_pstCurrentApp;
	}
	else
	{
		base_app = ONDK_GWM_APP_Get(callBack);
	}

	if (base_app == NULL)			// ������ �Ǵ� Application�� �������� �ƴϴ�.
	{
		HxLOG_Print("is Not Running now\n");
		return GWM_RET_NO_OBJECT;
	}

	traverse = g_pstAppRoot;
	app_count = 0;
	while (traverse != base_app)
	{
		reverseAppList[app_count] = traverse;
		app_count++;
		traverse = ONDK_GWM_APP_GetNextApp(traverse);
	}

	while(app_count > 0)
	{
		app_count--;
		ONDK_GWM_APP_Destroy(reverseAppList[app_count]->callBack);
	}

	s_nflagNowDestroyAll = 0;
	return GWM_RET_OK;
}


ONDK_Result_t			ONDK_GWM_APP_DestroyByPriority(HINT32 priority, HINT32 flag_inclusive )
{
	ONDK_GWM_App_t		traverse = 0;
	ONDK_GWM_App_t		reverseAppList[DEF_GWM_MAX_APPLICATION] = {0, };
	HINT32				app_count = 0;

	if (s_nflagNowDestroyAll)
		return GWM_RET_NO_OBJECT;
	s_nflagNowDestroyAll = 1;

	traverse = g_pstAppRoot;
	app_count = 0;
	if (flag_inclusive == 0)		// ������ priority�� �������� �ʴ´�.
	{
		while (traverse->priority > priority)
		{
			reverseAppList[app_count] = traverse;
			app_count++;
			traverse = ONDK_GWM_APP_GetNextApp(traverse);
		}
	}
	else							// ������ priority�� �����Ѵ�.
	{
		while (traverse->priority >= priority)
		{
			reverseAppList[app_count] = traverse;
			app_count++;
			traverse = ONDK_GWM_APP_GetNextApp(traverse);
		}
	}

	while (app_count > 0)
	{
		app_count--;
		ONDK_GWM_APP_Destroy(reverseAppList[app_count]->callBack);
	}

	s_nflagNowDestroyAll = 0;
	return GWM_RET_OK;
}


ONDK_Result_t			ONDK_GWM_APP_DestroyAll(void)
{
	ONDK_GWM_App_t		traverse;
	ONDK_GWM_App_t		reverseAppList[DEF_GWM_MAX_APPLICATION] = {0, };
	HINT32				app_count;

	if (s_nflagNowDestroyAll)
		return GWM_RET_NO_OBJECT;
	s_nflagNowDestroyAll = 1;

	// Application�� �������� ����.
	traverse = g_pstAppRoot;
	app_count = 0;
	while (traverse != NULL)
	{
		reverseAppList[app_count] = traverse;
		app_count++;
		traverse = ONDK_GWM_APP_GetNextApp(traverse);
	}

	while (app_count > 0)
	{
		app_count--;
		if(g_pfnAppIgnitor != reverseAppList[app_count]->callBack)
		{
			ONDK_GWM_APP_Destroy(reverseAppList[app_count]->callBack);
		}
	}

	s_nflagNowDestroyAll = 0;
	return GWM_MESSAGE_BREAK;		//GWM_APP_Destroy(g_pstCurrentApp->callBack);
}

HBOOL	ONDK_GWM_APP_IsInvalidateRect(HINT32 x, HINT32 y, HINT32 w, HINT32 h)
{
	HBOOL				bIsOver = FALSE;
	ONDK_Region_t		check_area;
	ONDK_Region_t		screen_area;

	//GWM_assert(g_pstCurrentApp);
	if(g_pstCurrentApp == NULL)
	{
		return FALSE;
	}

	if(ONDK_GWM_APP_GetState_UIHide(g_pstCurrentApp) == TRUE)
	{
		return FALSE;
	}

	check_area.x1 = x;
	check_area.y1 = y;
	check_area.x2 = x + w - 1;
	check_area.y2 = y + h - 1;
    screen_area         = INT_ONDK_GWM_GetUpdateArea(ONDK_GWM_APP_GetPosition(g_pstCurrentApp));

	bIsOver = ONDK_IsOverlappedRegion(&screen_area, &check_area);

	return bIsOver;
}

HBOOL	ONDK_GWM_APP_IsInvalidateRelativeRect(HINT32 x, HINT32 y, HINT32 w, HINT32 h)
{
	HBOOL				bIsOver = FALSE;
	ONDK_Region_t		check_area;
	ONDK_Region_t		screen_area;

	//GWM_assert(g_pstCurrentApp);
	if(g_pstCurrentApp == NULL)
	{
		return FALSE;
	}

	if(ONDK_GWM_APP_GetState_UIHide(g_pstCurrentApp) == TRUE)
	{
		return FALSE;
	}

	VK_memset(&check_area, 0x00, sizeof(ONDK_Region_t));

	ONDK_GWM_APP_GetAppArea(g_pstCurrentApp, &check_area);

	check_area.x1 += x;
	check_area.y1 += y;
	check_area.x2 = check_area.x1 + w - 1;
	check_area.y2 = check_area.y1 + h - 1;
    screen_area         = INT_ONDK_GWM_GetUpdateArea(ONDK_GWM_APP_GetPosition(g_pstCurrentApp));

	bIsOver = ONDK_IsOverlappedRegion(&screen_area, &check_area);

	return bIsOver;
}

ONDK_Result_t	ONDK_GWM_APP_InvalidateONDKRect(ONDK_Rect_t *rt)
{
	return ONDK_GWM_APP_InvalidateRect(rt->x, rt->y, rt->w, rt->h);
}

ONDK_Result_t	ONDK_GWM_APP_InvalidateRelativeONDKRect(ONDK_Rect_t *rt)
{
	return ONDK_GWM_APP_InvalidateRelativeRect(rt->x, rt->y, rt->w, rt->h);
}

ONDK_Result_t	ONDK_GWM_APP_InvalidateRelativeRect(HINT32 x, HINT32 y, HINT32 w, HINT32 h)
{
	ONDK_Region_t		app_area = {0, 0, 0, 0,};
	ONDK_Region_t		new_area = {0, 0, 0, 0,};
	ONDK_Region_t		temp;

	if(g_pstCurrentApp == NULL)
	{
		return ERR_FAIL;
	}

	if(ONDK_GWM_APP_GetState_UIHide(g_pstCurrentApp) == TRUE)
	{
		return GWM_RET_DONT_NEED_TO_DRAW;
	}

	ONDK_GWM_APP_GetAppArea(g_pstCurrentApp, &app_area);

	new_area.x1 = app_area.x1 + x;
	new_area.y1 = app_area.y1 + y;
	new_area.x2 = app_area.x1 + x + w;
	new_area.y2 = app_area.y1 + y + h;

	temp = INT_ONDK_GWM_GetUpdateArea(ONDK_GWM_APP_GetPosition(g_pstCurrentApp));
	INT_ONDK_GWM_SetUpdateArea(ONDK_GWM_APP_GetPosition(g_pstCurrentApp),ONDK_UnionRegion(&new_area, &temp));

	INT_ONDK_GWM_SetHaveToRedraw(ONDK_GWM_APP_GetPosition(g_pstCurrentApp),TRUE);

	return	GWM_RET_OK;

}

ONDK_Result_t	ONDK_GWM_APP_InvalidateRect(HINT32 x, HINT32 y, HINT32 w, HINT32 h)
{
	ONDK_Region_t		new_area = {0, 0, 0, 0,};
	ONDK_Region_t		temp;

	if(g_pstCurrentApp == NULL)
	{
		return ERR_FAIL;
	}

	if(ONDK_GWM_APP_GetState_UIHide(g_pstCurrentApp) == TRUE)
	{
		return GWM_RET_DONT_NEED_TO_DRAW;
	}

	new_area.x1 = x;
	new_area.y1 = y;
	new_area.x2 = x + w;
	new_area.y2 = y + h;

    temp = INT_ONDK_GWM_GetUpdateArea(ONDK_GWM_APP_GetPosition(g_pstCurrentApp));
	INT_ONDK_GWM_SetUpdateArea(ONDK_GWM_APP_GetPosition(g_pstCurrentApp),ONDK_UnionRegion(&new_area, &temp));

	INT_ONDK_GWM_SetHaveToRedraw(ONDK_GWM_APP_GetPosition(g_pstCurrentApp),TRUE);

	return	GWM_RET_OK;
}


ONDK_Result_t	ONDK_GWM_APP_InvalidateObject(HINT32 objID)
{
	ONDK_Result_t		hRet = GWM_RET_INITIAL_FAILED;
	ONDK_Rect_t		stRect;
	ONDK_GWM_GuiObject_t	obj;

	GWM_assert(g_pstCurrentApp);
	if(g_pstCurrentApp == NULL)
	{
		return ERR_FAIL;
	}

	if(ONDK_GWM_APP_GetState_UIHide(g_pstCurrentApp) == TRUE)
	{
		return GWM_RET_DONT_NEED_TO_DRAW;
	}

	obj = ONDK_GWM_APP_GetObject(objID);
	if (obj == NULL)
	{
		return GWM_RET_NO_OBJECT;
	}

	hRet = ONDK_GWM_Obj_GetObjectRect(objID, &stRect);
	if (hRet == GWM_RET_NO_OBJECT)
	{
		return GWM_RET_NO_OBJECT;
	}

	hRet = ONDK_GWM_APP_InvalidateRelativeRect(stRect.x, stRect.y, stRect.w, stRect.h);

	return hRet;
}

ONDK_Result_t	ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_App_t app)
{
	ONDK_GWM_App_t		appList = NULL;
	HBOOL			bFound = FALSE;
	ONDK_Region_t 		area,temp;
	//ONDK_Rect_t		stRect;


	if(app == 0)
	{
		GWM_assert(g_pstCurrentApp);
		if(g_pstCurrentApp == NULL)
		{
			return ERR_FAIL;
		}

		app = g_pstCurrentApp;
	}
	else
	{
		appList = (ONDK_GWM_App_t)g_pstAppRoot;
		while(appList != NULL)
		{
			if (appList == app)		/* application stack�� �ִ� app���� �˻��� �� �����Ѵ�. */
			{
				bFound = TRUE;

				break;
			}
			appList = (ONDK_GWM_App_t)gwm_lnklst_GetNext((LINK_OBJECT)appList);
		}

		if(bFound != TRUE)
		{
			return GWM_RET_NO_OBJECT;
		}
	}

	if(ONDK_GWM_APP_GetState_UIHide(app) == TRUE)
	{
		return GWM_RET_DONT_NEED_TO_DRAW;
	}

	ONDK_GWM_APP_GetAppArea(app, &area);
	//stRect = ONDK_RegionToRect(area);

    temp = INT_ONDK_GWM_GetUpdateArea(ONDK_GWM_APP_GetPosition(app));
	INT_ONDK_GWM_SetUpdateArea(ONDK_GWM_APP_GetPosition(app),ONDK_UnionRegion(&temp, &area));

	INT_ONDK_GWM_SetHaveToRedraw(ONDK_GWM_APP_GetPosition(app),TRUE);
	//ONDK_GWM_DirectRedrawAll();
	//ONDK_GWM_PostMessage(NULL,MSG_GWM_INVALIDATE_RECT,NULL,0,0,0);

	return	GWM_RET_OK;
}

ONDK_Result_t		ONDK_GWM_APP_Call(ONDK_GWM_App_t app, HINT32 message, Handle_t handle, HINT32 param1, HINT32 param2, HINT32 param3, void *pvData)
{
	ONDK_GWM_App_t		backupApp;
	ONDK_Result_t	hresult;
	HBOOL			bHaveUiObjectBeforeCall = FALSE;
	HBOOL			bHaveUiObjectAfterCall = FALSE;

	//ONDK_Assert(ONDK_GWM_IsGwmTask() == TRUE);

	if (app == NULL)
	{
		GWM_DBG_Print(DBGMSG_ERROR, ("\n\n\t No Application in ONDK_GWM_APP_Call() !! \n\n\a"));
		return GWM_RET_NO_OBJECT;
	}

	backupApp = g_pstCurrentApp;
	g_pstCurrentApp = app;
	gMsgData = pvData;

	/* message�� ȣ���ϱ� ������ UI�� ���¸� backup�Ѵ�. */
	if(app->ObjectList != NULL)
	{
		bHaveUiObjectBeforeCall = TRUE;
	}

	if (app->callBack != NULL)
	{
		//ONDK_Print("[%s:%d] app->callBack(%08X) (+)\n", __FUNCTION__, __LINE__, app->callBack);
		hresult = app->callBack(message, handle, param1, param2, param3);
		//ONDK_Print("[%s:%d] app->callBack(%08X) (-)\n", __FUNCTION__, __LINE__, app->callBack);
	}
	else
	{
		hresult = GWM_RET_NO_OBJECT;
	}

	/* message�� ȣ���� �Ŀ� UI�� ���¸� backup�Ѵ�. */
	if(app->ObjectList != NULL)
	{
		bHaveUiObjectAfterCall = TRUE;
	}

	g_pstCurrentApp = backupApp;

	/* MESSAGE ȣ��� ���� object ���� ���°� ����Ǿ���. */
	if(((message != MSG_GWM_CREATE) || (message != MSG_GWM_DESTROY))
		&& (bHaveUiObjectBeforeCall != bHaveUiObjectAfterCall))
	{
		local_gwmapp_CheckAppUiOrderStacking(g_pstAppRoot);
	}

	gMsgData = NULL;

	return hresult;
}

HUINT32 ONDK_GWM_APP_CountAppByUiState (GWM_UiState_t eGwmUiState)
{
	HUINT32			 nCount = 0;
	ONDK_GWM_App_t		 base_app;

	base_app = (ONDK_GWM_App_t)g_pstAppRoot;
	while (base_app != NULL)
	{
		if (base_app->ulUiState == eGwmUiState)
		{
			nCount ++;
		}

		base_app = (ONDK_GWM_App_t)gwm_lnklst_GetNext((LINK_OBJECT)base_app);
	}

	return nCount;
}

ONDK_GWM_App_t ONDK_GWM_APP_GetAppByUiState (GWM_UiState_t eGwmUiState, HUINT32 ulIndex)
{
	HUINT32			 nCount = 0;
	ONDK_GWM_App_t		 base_app;

	base_app = (ONDK_GWM_App_t)g_pstAppRoot;
	while (base_app != NULL)
	{
		if (base_app->ulUiState == eGwmUiState)
		{
			if (ulIndex == nCount)
			{
				return base_app;
			}

			nCount ++;
		}

		base_app = (ONDK_GWM_App_t)gwm_lnklst_GetNext((LINK_OBJECT)base_app);
	}

	return NULL;
}

void ONDK_GWM_APP_SetUiState (GWM_UiState_t eGwmUiState)
{
	GWM_assert(g_pstCurrentApp);

	if (g_pstCurrentApp != NULL)
	{
		g_pstCurrentApp->ulUiState = eGwmUiState;
	}
}

HERROR ONDK_GWM_APP_GetUiState (ONDK_GWM_Callback_t pfCallback, GWM_UiState_t *peGwmUiState)
{
	ONDK_GWM_App_t		 pstApp;

	if (peGwmUiState == NULL)			{ return ERR_FAIL; }

	if (pfCallback != NULL)
	{
		pstApp = ONDK_GWM_APP_Get (pfCallback);
	}
	else
	{
		pstApp = g_pstCurrentApp;
	}

	if (pstApp == NULL)					{ return ERR_FAIL; }

	*peGwmUiState = (GWM_UiState_t)pstApp->ulUiState;
	return ERR_OK;
}



/* End of File. ---------------------------------------------------------- */

