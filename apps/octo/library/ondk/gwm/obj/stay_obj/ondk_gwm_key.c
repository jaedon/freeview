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


/* Includes ----------------------------------------------------------------- */
#include <isosvc.h>
#include <string.h>
#include <osd_gfx.h>
#include <gwm_objdef.h>
#include <gwm_appobj.h>
#include <osd_font.h>
#include <gwm_guiobj.h>
#include "gwm_key.h"


#define DARK_RED		0xFF220000
#define KEYOBJ_DATA_SIZE	20

/* ------------------------------------------------------------ */
/*	MACRO DEFINITIONS.										     */
/* ------------------------------------------------------------ */

/* ------------------------------------------------------------ */
/*	GLOBAL VARIABLES.										     */
/* ------------------------------------------------------------ */

static	int						gGwmKeyObjectNumber;
static	struct tagKeyObject		gGwmKeyObjectArray[GWM_MAX_KEY];

static	KEYOBJ_MODE			s_inputMode;

ONDK_Result_t		DefaultKeyMethod(GWM_Obj_MethodType_t method_type, struct tagGuiObject *object, int arg);



/* ------------------------------------------------------------ */
/*	FUNCTION DEFINITIONS.									     */
/* ------------------------------------------------------------ */
void		GWM_InitializeKeyClass()
{
	int	i;

	for (i = 0; i < GWM_MAX_KEY; i++)
	{
		gGwmKeyObjectArray[i].ID = -1;
		gGwmKeyObjectArray[i].type = GUIOBJ_KEY;
		gGwmKeyObjectArray[i].Proc = DefaultKeyMethod;
		gGwmKeyObjectArray[i].objDrawMethod= NULL;
		gGwmKeyObjectArray[i].focus = GUIOBJ_NORMAL;
		gGwmKeyObjectArray[i].rect.x = -1;
		gGwmKeyObjectArray[i].rect.y = -1;
		gGwmKeyObjectArray[i].rect.w = 0;
		gGwmKeyObjectArray[i].rect.h = 0;
		gGwmKeyObjectArray[i].encoding = eHxCP_UNICODE_UCS2;

		gGwmKeyObjectArray[i].data[0]  = NULL;
		gGwmKeyObjectArray[i].data[1] = NULL;
		gGwmKeyObjectArray[i].data[2] = NULL;

		gGwmKeyObjectArray[i].index = 0;
		gGwmKeyObjectArray[i].popDownward = FALSE;

//		gGwmKeyObjectArray[i].pDrawFunc = NULL;
	}

	s_inputMode = OBJ_KEY_MODE_BROWSE;
}

static KEY_OBJECT 		AllocateKeyMemory(void)
{
	int 	i;

	if (gGwmKeyObjectNumber >= GWM_MAX_KEY)
	{
		GWM_DBG_Print(DBGMSG_OBJ, ("[ERR:keyObj] Too Many keys. Increase GWM_MAX_KEY in <obj_key.h>\n" ));
		return	NULL;
	}

	for (i = 0; i < gGwmKeyObjectNumber; i++)
	{
		if (-1 == gGwmKeyObjectArray[i].ID)
			break;
	}
	if (i >= gGwmKeyObjectNumber)
		gGwmKeyObjectNumber++;

	return (&gGwmKeyObjectArray[i]);
}

static void			FreeKeyMemory(KEY_OBJECT keyObj)
{
	int		i;

	for (i = 0; i < gGwmKeyObjectNumber; i++)
	{
		if (keyObj == &gGwmKeyObjectArray[i])
			break;
	}
	gGwmKeyObjectArray[i].ID = -1;

	if (i == gGwmKeyObjectNumber)
	{
		if (gGwmKeyObjectNumber > 0)
			gGwmKeyObjectNumber --;
	}
}




static ONDK_Result_t	DeleteKeyObject(KEY_OBJECT keyObj)
{
	keyObj->ID = -1;
	keyObj->type = GUIOBJ_KEY;
	keyObj->Proc = NULL;
	keyObj->objDrawMethod = NULL;
	keyObj->focus = GUIOBJ_NORMAL;
	keyObj->rect.x = -1;
	keyObj->rect.y = -1;
	keyObj->rect.w = 0;
	keyObj->rect.h = 0;

	// free allocated memory

	FreeKeyMemory(keyObj);

	return GWM_RET_OK;
}


int				_getKeyDataNum(KEY_OBJECT obj)
{
	int			i, num = 0;

	if (obj == NULL)
		return 0;

	for (i = 0; i < GWM_MAX_KEY_DATA_NUM; i++)
	{
		if (obj->data[i] != NULL)
			num++;
	}

	return num;
}


void				drawKeyDefault(ONDKSurfaceHandle pixmap, KEY_OBJECT obj)
{
	ONDK_Region_t	area;
	ONDKFontHandle pFont = NULL;


	HINT32			txtColor;
	HINT32			i, x, y, w, h, cWid;
	HINT32			num;
	HINT32			focus_width=0, fucus_height=0;
	HINT32			popup_x=0, popup_y=0, popup_width=0,popup_height=0;
	HINT32			lUsedByte, lUtf8Len;
	char		pDstStr[KEYOBJ_DATA_SIZE+1]={0,};

	pFont = ONDK_FONT_CreateSystemFont(eFont_SystemNormal, DEFAULT_OBJ_FONT_SIZE);
	ONDK_GWM_APP_GetAppArea(g_pstCurrentApp, &area);

	x = area.x1 + obj->rect.x;
	y = area.y1 + obj->rect.y;
	w = obj->rect.w;
	h = obj->rect.h;
	focus_width= x+(w-16);
	fucus_height = y+(h-7);

	if (obj->focus & GUIOBJ_HILIGHTED)
	{
		txtColor = 0xFFDCDCDC;	/*DARK_RED_COLOR*/
	}
	else
	{
		txtColor = 0xFFDCDCDC;	/*WHITE_COLOR;*/
	}


	if (obj->focus & GUIOBJ_HILIGHTED)
	{
		// draw background
		//이미지로 그릴 수 대체 방법을 찾자 !!
		ONDK_DRAW_FillRectElement(ONDK_GWM_GetSurface(), x, y, focus_width, fucus_height, 0xFF6B89EC);/*YELLOWISH_BROWN_FRAME_COLOR*/

		// draw key data or character
		if(obj->encoding == eHxCP_UNICODE_UCS2)
		{
			pDstStr[0]=0x15;
			MWC_UTIL_ConvertUCS2ToUtf8((const HUINT16 *)obj->data[0], 1, 128, &lUsedByte, &pDstStr[1], &lUtf8Len);
			pDstStr[1+lUtf8Len]='\0';

		}
		else if(obj->encoding == eHxCP_UNICODE_UTF8)
		{
			VK_strncpy(pDstStr, obj->data[0], KEYOBJ_DATA_SIZE);
		}

		cWid = ONDK_GWM_GFX_GetStringWidth(pFont, pDstStr);

		ONDK_FONT_DrawString(ONDK_GWM_GetSurface(), pFont, obj->rect.x,obj->rect.y, pDstStr, txtColor, ONDKSTF_LEFT);		// 0 is black
	}

	// key popup 그리기
	if (obj->focus & GUIOBJ_HILIGHTED && s_inputMode == OBJ_KEY_MODE_SELECT)
	{
		num = _getKeyDataNum(obj);

		popup_x = (x+focus_width);
		popup_y = y;
		popup_width = (w-16);
		popup_height = (fucus_height-focus_width);

		if (num > 1)
		{
			for (i = 0; i < num; i++)
			{
				cWid = ONDK_GWM_GFX_GetStringWidth(pFont, obj->data[i]);

				if (obj->popDownward)
				{
					ONDK_DRAW_FillRectElement(pixmap, x + (w-16), y + i * h, x + w + (w-16), y + i * h + h, COLOR_GWM_KEY_CELL_FOCUSED);
					//ONDK_Print("[%s][%d] x(%d) y(%d) w(%d) h(%d) \n", __FUNCTION__,__LINE__, (x+w)-16, y, x+((w*2)-32), y+(i*(h-7))+(h-7));
					//ONDK_DRAW_FillRectElement(pixmap,(x+w)-16, y, x+((w*2)-32), y+(i*(h-7))+(h-7), COLOR_GWM_KEY_CELL_FOCUSED);

					if (obj->index == i)
					{
						//ONDK_DRAW_FillRectElement(pixmap,(x+w)-16, y, x+((w*2)-32), y+(i*(h-7))+(h-7), COLOR_GWM_KEY_SELECTED);
						ONDK_DRAW_FillRectElement(pixmap, x + (w-16), y + i * h + 2, x + w + (w - 16), y + i * h + h - 2, COLOR_GWM_KEY_SELECTED);
						/*ONDK_Print("[%s][%d] x(%d) y(%d) w(%d) h(%d) \n", __FUNCTION__,__LINE__,
						(x+w)-16, y + (i * h) + 2, x+((w*2)-32), y+(i*(h-7))+(h-7));
						ONDK_DRAW_FillRectElement(pixmap, (x+w)-16, y + (i * h) + 2, x+((w*2)-32), y+(i*(h-7))+(h-7), COLOR_GWM_KEY_SELECTED);*/
					}

					if(obj->encoding == eHxCP_UNICODE_UCS2)
					{
						pDstStr[0]=0x15;
						MWC_UTIL_ConvertUCS2ToUtf8((const HUINT16 *)obj->data[i], 1, 128, &lUsedByte, &pDstStr[1], &lUtf8Len);
						pDstStr[1+lUtf8Len]='\0';
					}
					else if(obj->encoding == eHxCP_UNICODE_UTF8)
					{
						VK_strncpy(pDstStr, obj->data[i], KEYOBJ_DATA_SIZE);
					}

					ONDK_FONT_DrawString(pixmap, pFont, (x + w + (w / 2) - (cWid / 2))-16, y + (h / 2) - (pFont->height / 2) + (i * h), pDstStr, DARK_RED, ONDKSTF_LEFT);

				}
				else
				{
					ONDK_DRAW_FillRectElement(pixmap,(x+w)-16, y - h * ((num - 1) - i), x + w + (w-16), y - h * ((num - 1) - i) + h, COLOR_GWM_KEY_CELL_FOCUSED);
					//ONDK_DRAW_FillRectElement(pixmap, x + (w-16), y + i * h, x + w + (w-16), y + i * h + h, COLOR_GWM_KEY_CELL_FOCUSED);
					if (obj->index == i)
					{
						//ONDK_DRAW_FillRectElement(pixmap, x + w + 2, y - h * ((num - 1) - i) + 2, x + w + w - 2, y - h * ((num - 1) - i) + h - 2, COLOR_GWM_KEY_SELECTED);
						ONDK_DRAW_FillRectElement(pixmap, x + (w -16), y - h * ((num - 1) - i) + 2, x + w + (w - 16), y - h * ((num - 1) - i) + h - 2, COLOR_GWM_KEY_SELECTED);
					}

					if(obj->encoding == eHxCP_UNICODE_UCS2)
					{
						pDstStr[0]=0x15;
						MWC_UTIL_ConvertUCS2ToUtf8((const HUINT16 *)obj->data[i], 1, 128, &lUsedByte, &pDstStr[1], &lUtf8Len);
						pDstStr[1+lUtf8Len]='\0';
					}
					else if(obj->encoding == eHxCP_UNICODE_UTF8)
					{
						VK_strncpy(pDstStr, obj->data[i], KEYOBJ_DATA_SIZE);
					}
					ONDK_FONT_DrawString(pixmap, pFont, (x + w + (w / 2) - (cWid / 2))-16, y - h * ((num - 1) - i) + (h / 2) - (pFont->height / 2), pDstStr, DARK_RED, ONDKSTF_LEFT);
				}
			}
		}
	}

	ONDK_FONT_Release(pFont);
}


ONDK_Result_t		DefaultKeyMethod(GWM_Obj_MethodType_t method_type, struct tagGuiObject *object, int arg)
{
	KEY_OBJECT		keyObj;
	ONDK_Region_t		area;
	int				dataNum;
	int				x, y, w, h;

	keyObj = (KEY_OBJECT)object;
	dataNum = _getKeyDataNum(keyObj);
	ONDK_GWM_APP_GetAppArea(g_pstCurrentApp, &area);

	x = area.x1 + object->rect.x;
	y = area.y1 + object->rect.y;
	w = object->rect.w;
	h = object->rect.h;

	switch (method_type)
	{
		case MT_DRAW:

			if (NULL != keyObj->pDrawFunc)
			{
				keyObj->pDrawFunc(ONDK_GWM_GetSurface(), keyObj->data[0], object);
			}
			else
			{
				drawKeyDefault(ONDK_GWM_GetSurface(), keyObj);
			}
			return GWM_MESSAGE_BREAK;

		case MT_GET_FOCUS:
			object->focus = GUIOBJ_HILIGHTED;

			if (dataNum <= 1)
			{
				s_inputMode = OBJ_KEY_MODE_SELECT;
			}
			else
			{
				s_inputMode = OBJ_KEY_MODE_BROWSE;
			}
			//GWM_APP_InvalidateObject(keyObj->ID);		x2-x1 + 1	,y2-y1 + 1;
			//GWM_APP_InvalidateRect(x, y, w+1, h+1);
			ONDK_GWM_APP_InvalidateRelativeRect(x, y, (w-10), h+1);
			//GWM_APP_InvalidateRect(267,280,746,335);

			return GWM_MESSAGE_BREAK;

		case MT_LOST_FOCUS:
			object->focus = GUIOBJ_NORMAL;
			keyObj->index = 0;					// reset the data index
			//GWM_APP_InvalidateObject(keyObj->ID);
			ONDK_GWM_APP_InvalidateRelativeRect(x, y, (w-10), h+1);
			//GWM_APP_InvalidateRect(267,280,746,335);
			return GWM_MESSAGE_BREAK;

		case MT_SET_VALUE:
			return GWM_MESSAGE_BREAK;


		case MT_KEYDOWN:
			if (object->focus & GUIOBJ_HILIGHTED)
			{
				switch (arg)
				{
					/* Front OK 키에 대한 처리가 없어서 Object가 프론트 키에 반응을 안해서 추가한다.
					현재 Navy(UI) 기준이고.. 다른향에서 문제되면 해당 부분에 수정이 필요할 것 같다. */
					case KEY_FRONT_OK:
					case KEY_OK:
						if (dataNum <= 1)
						{
							ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_KEY_PRESSED, (Handle_t)NULL, (int)object->ID, (int)keyObj->data[keyObj->index], (int)keyObj, NULL);
						}
						else
						{
							if (s_inputMode == OBJ_KEY_MODE_SELECT)
							{
								ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_KEY_PRESSED, (Handle_t)NULL, (int)object->ID, (int)keyObj->data[keyObj->index], (int)keyObj, NULL);
								ONDK_GWM_APP_InvalidateRelativeRect(x+w, y, (w-10), (h*dataNum)+1);
							}
							else
							{
								s_inputMode = OBJ_KEY_MODE_SELECT;
								ONDK_GWM_APP_InvalidateAPP(NULL);
							}
						}

						//GWM_APP_InvalidateObject(keyObj->ID);

						//GWM_APP_InvalidateRect(x, y, (w-10), h+1);
						//GWM_APP_InvalidateRect(267,280,746,335);
						return GWM_MESSAGE_BREAK;

					case KEY_BACK:
						if (s_inputMode == OBJ_KEY_MODE_SELECT)
						{
							s_inputMode = OBJ_KEY_MODE_BROWSE;
							//GWM_APP_InvalidateObject(keyObj->ID);
							//GWM_APP_InvalidateRect(x, y, (w-10), h+1);
							ONDK_GWM_APP_InvalidateAPP(NULL);
							//GWM_APP_InvalidateRect(267,280,746,335);
							return GWM_MESSAGE_BREAK;
						}
						break;

					case KEY_ARROWUP:
						if (dataNum > 1)
						{
							if (s_inputMode == OBJ_KEY_MODE_SELECT)
							{
								// select the previous character
								if (keyObj->index > 0)
								{
									keyObj->index--;
								}
								else
								{
									keyObj->index = dataNum - 1;
								}
								//GWM_APP_InvalidateObject(keyObj->ID);
								ONDK_GWM_APP_InvalidateAPP(NULL);
								//GWM_APP_InvalidateRect(267,280,746,335);
								return GWM_MESSAGE_BREAK;
							}
						}
						break;

					case KEY_ARROWDOWN:
						if (dataNum > 1)
						{
							if (s_inputMode == OBJ_KEY_MODE_SELECT)
							{
								// select the next character
								if (keyObj->index < dataNum - 1)
								{
									keyObj->index++;
								}
								else
								{
									keyObj->index = 0;
								}
								//GWM_APP_InvalidateObject(keyObj->ID);
								ONDK_GWM_APP_InvalidateAPP(NULL);
								//GWM_APP_InvalidateRect(267,280,746,335);

								return GWM_MESSAGE_BREAK;
							}
						}
						break;
				}
			}
			return GWM_MESSAGE_BREAK;

		case MT_DESTROY:
			DeleteKeyObject(keyObj);
			//GWM_APP_InvalidateObject(keyObj->ID);
			ONDK_GWM_APP_InvalidateAPP(NULL);
			//GWM_APP_InvalidateRect(267,280,746,335);
			return GWM_MESSAGE_BREAK;

		default:
			break;
	}
	return GWM_MESSAGE_BREAK;
}


ONDK_Result_t		GWM_CreateKeyObject(HINT32 ID, ONDK_Rect_t *rect, HBOOL popDownward, KEY_OBJECT *keyPtr, eHxCP_CodePage_e encoding)
{
	ONDK_Result_t			hResult = GWM_RET_OK;
	ONDK_GWM_GuiObject_t		objlist;
	KEY_OBJECT			newKey;

	GWM_assert(g_pstCurrentApp);
	objlist = INT_ONDK_GWM_APP_GetObjectList(g_pstCurrentApp);

	newKey = (KEY_OBJECT)INT_ONDK_GWM_OBJ_FindObject(objlist, ID);
	if (newKey != NULL)
	{
		return GWM_RET_LINK_CONFLICT;
	}

	newKey = AllocateKeyMemory();
	if (newKey == NULL)
	{
		return GWM_RET_OUT_OF_MEMORY;

	}

	// initialize slider structure
	newKey->ID = ID;
	newKey->type = GUIOBJ_KEY;
	newKey->Proc = DefaultKeyMethod;
	newKey->objDrawMethod = NULL;
	newKey->focus = GUIOBJ_NORMAL;
	newKey->encoding = encoding;


	newKey->data[0] = NULL;
	newKey->data[1] = NULL;
	newKey->data[2] = NULL;

	memcpy(&(newKey->rect), rect, sizeof(ONDK_Rect_t));

	newKey->index = 0;
	newKey->popDownward = popDownward;

	hResult = INT_ONDK_GWM_APP_AddObject(g_pstCurrentApp, (ONDK_GWM_GuiObject_t)newKey);
	if (keyPtr !=NULL)
		*keyPtr = newKey;

	ONDK_GWM_APP_InvalidateRelativeRect(rect->x, rect->y, rect->w, rect->h);

	return hResult;
}


ONDK_Result_t 	GWM_SetKeyData(HINT32 ID, HINT8 *data1, HINT8 *data2, HINT8 *data3)
{
	KEY_OBJECT			obj = NULL;

	obj = (KEY_OBJECT)ONDK_GWM_APP_GetObject(ID);
	if (obj == NULL)
	{
		return GWM_RET_NO_OBJECT;
	}

	obj->data[0] = data1;
	obj->data[1] = data2;
	obj->data[2] = data3;

	return GWM_RET_OK;
}


HINT8 *			GWM_GetKeyData(HINT32 ID, HUINT8 index)
{
	KEY_OBJECT			obj = NULL;

	obj = (KEY_OBJECT)ONDK_GWM_APP_GetObject(ID);
	if (obj == NULL)
		return NULL;

	if (index >= GWM_MAX_KEY_DATA_NUM)
		return NULL;

	return obj->data[index];
}


ONDK_Result_t 	GWM_ChangeKeyDrawMethod(HINT32 ID, key_Draw_Func method)
{
	KEY_OBJECT			obj = NULL;

	obj = (KEY_OBJECT)ONDK_GWM_APP_GetObject(ID);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;

	obj->pDrawFunc = method;

	return GWM_RET_OK;
}


HBOOL			GWM_IsKeyInputProgressing(HINT32 ID)
{
	KEY_OBJECT			obj = NULL;

	obj = (KEY_OBJECT)ONDK_GWM_APP_GetObject(ID);
	if (obj == NULL)
		return GWM_RET_OK;

	if (_getKeyDataNum(obj) <= 1 && s_inputMode == OBJ_KEY_MODE_SELECT)
		return FALSE;

	if (s_inputMode == OBJ_KEY_MODE_BROWSE)
		return FALSE;

	return TRUE;
}


ONDK_Result_t		GWM_ResetKeyInput(HINT32 ID)
{
	KEY_OBJECT			obj = NULL;

	obj = (KEY_OBJECT)ONDK_GWM_APP_GetObject(ID);
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;

	s_inputMode = OBJ_KEY_MODE_BROWSE;
	obj->index = 0;

	return GWM_RET_OK;
}

