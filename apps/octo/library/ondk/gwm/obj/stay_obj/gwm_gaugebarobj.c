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


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <string.h>
#include <osd_font.h>
#include <osd_gfx.h>
#include <gwm_objdef.h>
#include <gwm_appobj.h>


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define MAX_GAUGEBAR_OBJECT		32

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
struct tagGaugebarObject
{
	GUIOBJ_BASIC_PROPERTY
	/* gaugebar object properties */
	HINT8	cFocusType; /* Focus type (Adjust = 0, Balance = 1) */
	HINT8	*pcTitle;
	HINT32	iMin;
	HINT32	iMax;
	HINT32	iValue;
#if 0
	HINT32	iStepSize;
#endif
	HULONG	ulGBImgs_Addr; /* Gauge Bar Images(Bitmap data) Addr */
#if 0
	HUINT8	*pucGaugeBack_N_Img;
	HUINT8	*pucGaugeBack_F_Img;
	HUINT8	*pucGaugeFocus_N_Img;
	HUINT8	*pucGaugeFocus_F_Img;
#endif
};

typedef struct tagPARAMETERImg
{
	HUINT16	usCursorWidth;
	HUINT8	*pucCursor_Img;

	HUINT16	usBackWidth;
	HUINT8	*pucGaugeBack_N_Img;
	HUINT8	*pucGaugeBack_F_Img;
	HUINT8	*pucGaugeFocus_N_Img;
	HUINT8	*pucGaugeFocus_F_Img;
}sParameterImg_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static struct tagGaugebarObject	s_sGaugebarArray[MAX_GAUGEBAR_OBJECT];
static HINT8					isAllocated[MAX_GAUGEBAR_OBJECT];

/*******************************************************************/
/******************* Static Function Prototypes  *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Local Functions  ************************/
/*******************************************************************/
static PGAUGEBAR_OBJECT local_gwmgaugebar_AllocNewGaugebarObj(void)
{
	HINT32	iI;

	for(iI = 0; iI < MAX_GAUGEBAR_OBJECT; iI++)
	{
		if (!isAllocated[iI])
			break;
	}
	if (iI < MAX_GAUGEBAR_OBJECT)
	{
		isAllocated[iI] = 1;
		return (&s_sGaugebarArray[iI]);
	}
	return NULL;
}

static void local_gwmgaugebar_FreeGaugebarObj(PGAUGEBAR_OBJECT pGbObj)
{
	HINT32	iI;

	GWM_assert(pGbObj);
#ifdef CONFIG_DEBUG
	if (NULL != pGbObj->link)
		GWM_DBG_Print(DBGMSG_OBJ, ("[WARNING] Object Id(%d):%08X is currently linked.\r\n", pGbObj->ID, (HINT32)pGbObj));
#endif
	for (iI = 0; iI < MAX_GAUGEBAR_OBJECT; iI++)
	{
		if (pGbObj == &(s_sGaugebarArray[iI]))
			break;
	}
	if (iI < MAX_GAUGEBAR_OBJECT)
	{
		isAllocated[iI] = 0;
		INT_ONDK_GWM_OBJ_BasicSetting((ONDK_GWM_GuiObject_t)&(s_sGaugebarArray[iI]));
		s_sGaugebarArray[iI].cFocusType = 0;
		s_sGaugebarArray[iI].pcTitle = NULL;
		s_sGaugebarArray[iI].iMin = 0;
		s_sGaugebarArray[iI].iMax = 0;
		s_sGaugebarArray[iI].iValue = 0;
		s_sGaugebarArray[iI].ulGBImgs_Addr = (HULONG)NULL;
	}
	return;
}

static PGAUGEBAR_OBJECT local_gwmgaugebar_GetGaugebarObject(HINT32 iId)
{
	ONDK_GWM_GuiObject_t		pGuiObjList;
	PGAUGEBAR_OBJECT	pGaugebar;

	GWM_assert(g_pstCurrentApp);

	pGuiObjList = INT_ONDK_GWM_APP_GetObjectList(g_pstCurrentApp);
	pGaugebar = (PGAUGEBAR_OBJECT)INT_ONDK_GWM_OBJ_FindObject(pGuiObjList, iId);

	return pGaugebar;
}

static void local_gwmgaugebar_DrawDefaultGaugebarMethod(ONDKSurfaceHandle psScreen,
																ONDK_Region_t *psArea,
																PGAUGEBAR_OBJECT pGbObj,
																HBOOL isFrontMost)
{
	HINT32	iX, iY, iW, iH, iCursorImg_w, iValStr_w, iValStr_x, iBackImg_x, iBackImg_w;
	HINT32	iFocus;
	HINT8	cFocusType;
	HINT8	*pcTitle = NULL;
	HINT32	iMin, iMax, iValue, iCenterVal = 1;	/* iCenterVal은 나누기의 분모로 쓰이므로 0으로 초기화 하면 안됨. */

	sParameterImg_t	*psParamImg = NULL;

	HUINT8	*pucBackImg = NULL, *pucBarImg = NULL;
	HINT8	acStrBuf[10] = {0, };

	ONDKFontHandle	psFont = NULL;
	ONDK_Color_t	ulFontColor;

	HINT32		iMargin_x = 12, iCenter_x, iHalf_w;
	HFLOAT64	dFocus_x;

	iX = psArea->x1+pGbObj->rect.x;
	iY = psArea->y1+pGbObj->rect.y;
	iW = pGbObj->rect.w;
	iH = pGbObj->rect.h;

	iFocus = pGbObj->focus;

	cFocusType = pGbObj->cFocusType;
	pcTitle = pGbObj->pcTitle;
	iMin = pGbObj->iMin;
	iMax = pGbObj->iMax;
	iValue = pGbObj->iValue;
	psParamImg = (sParameterImg_t *)pGbObj->ulGBImgs_Addr;

	/* 좀 어거지가 ... */
	if(iFocus) /* (GUIOBJ_HILIGHTED & iFocus) */
	{
#if 0
		iCursorImg_w = psParamImg->usCursorWidth;
		OSD_GFX_DrawStockImage(psScreen, iX+((iW-iCursorImg_w)/2), iY, psParamImg->pucCursor_Img, 100);
#else
		iCursorImg_w = OSD_GFX_GetBitmapWidth(psParamImg->pucCursor_Img); // (Setup_Cursor_popup_Bsize);
		ONDK_GWM_GFX_DrawBitmap(psScreen, iX+((iW-iCursorImg_w)/2), iY, psParamImg->pucCursor_Img/* Setup_Cursor_popup_Bsize */, eBmpBlend_COPY);

#endif

		pucBackImg = psParamImg->pucGaugeBack_F_Img; // EQ_Bar_Back_F;
		pucBarImg = psParamImg->pucGaugeFocus_F_Img; // Balance_Bar_F;

		ulFontColor = 0xFFFAFAFA; // Focused Grey
	}
	else
	{
		pucBackImg = psParamImg->pucGaugeBack_N_Img; // EQ_Bar_Back_F;
		pucBarImg = psParamImg->pucGaugeFocus_N_Img; // Balance_Bar_F;

		ulFontColor = 0xFF1F243A; // List Blue
	}

	psFont = ONDK_FONT_CreateSystemFont(eFont_SystemNormal, 27);
	if(psFont != NULL) // FONT_SIZE_M
	{
		/* Draw title String */
		ONDK_FONT_DrawString(psScreen, psFont,
							iX+27,
							iY+31-psFont->height,
							pcTitle, ulFontColor, ONDKSTF_LEFT);

		iCenterVal = (iMax-iMin)/2;
		if(0 == cFocusType)
		{
			/* EQ Bar */
			if(iCenterVal > iValue)
			{
				snprintf(acStrBuf, 10, "-%d", (iCenterVal-iValue));
			}
			else if(iCenterVal <= iValue)
			{
				snprintf(acStrBuf, 10, "%d", iValue-iCenterVal);
			}
			else
			{
				GWM_assert(0);
			}
		}
		else if(1 == cFocusType)
		{
			/* Balance Bar */
			if(iCenterVal > iValue)
			{
				snprintf(acStrBuf, 10, "L%d", (iCenterVal-iValue));
			}
			else if(iCenterVal == iValue)
			{
				snprintf(acStrBuf, 10, "0");
			}
			else if(iCenterVal < iValue)
			{
				snprintf(acStrBuf, 10, "R%d", (iValue-iCenterVal));
			}
			else
			{
				GWM_assert(0);
			}
		}
		else
		{
			GWM_assert(0);
		}

		/* Draw Value String */
		ONDK_FONT_DrawString(psScreen, psFont,
							iX+27+159,
							iY+31-psFont->height,
							":", ONDKSTF_LEFT);

		iValStr_w = ONDK_GWM_GFX_GetStringWidth(psFont, acStrBuf);
		iValStr_x = iX+27+159+(ONDK_GWM_GFX_GetStringWidth(psFont, ":R00")-iValStr_w);
		ONDK_FONT_DrawString(psScreen, psFont,
							iValStr_x,
							iY+31-psFont->height,
							acStrBuf, ulFontColor, ONDKSTF_LEFT);
	}

	/* Draw Back Image */
#if 0
	iBackImg_w = psParamImg->usBackWidth;
	iBackImg_x = iX+iW-29-3-iBackImg_w;
	OSD_GFX_DrawStockImage(psScreen, iBackImg_x, iY+13, pucBackImg, 100);
#else
	iBackImg_w = OSD_GFX_GetBitmapWidth(pucBackImg);
	iBackImg_x = iX+iW-29-3-iBackImg_w;
	ONDK_GWM_GFX_DrawBitmap(psScreen, iBackImg_x, iY+13, pucBackImg, eBmpBlend_COPY);
#endif

	/* Draw Bar(Focus) Image */
	iCenter_x = (iBackImg_w/2)-2;
	iHalf_w = iCenter_x-iMargin_x;
	dFocus_x = ((HFLOAT64)iHalf_w/(HFLOAT64)iCenterVal)*(HFLOAT64)iValue;

#if 0
	OSD_GFX_DrawStockImage(psScreen, iBackImg_x+iMargin_x+(HINT32)dFocus_x, iY+13, pucBarImg, 100);
#else
	ONDK_GWM_GFX_DrawBitmap(psScreen, iBackImg_x+iMargin_x+(HINT32)dFocus_x, iY+13, pucBarImg, eBmpBlend_COPY);
#endif

	ONDK_FONT_Release(psFont);

	return;
}

static ONDK_Result_t local_gwmgaugebar_DefaultGaugebarProc(GWM_Obj_MethodType_t eProcMsg,
																ONDK_GWM_GuiObject_t pObject,
																HINT32 iArg)
{
	PGAUGEBAR_OBJECT	pGbObj;
	ONDK_Region_t			sArea;
	ONDK_Rect_t			sRect;

	GWM_assert(pObject->type == GUIOBJ_GAUGEBAR);
	pGbObj = (PGAUGEBAR_OBJECT)pObject;

	switch(eProcMsg)
	{
		case MT_DESTROY:
			INT_ONDK_GWM_OBJ_GetObjectRect(pObject, &sRect);
			ONDK_GWM_APP_InvalidateRelativeRect(sRect.x, sRect.y, sRect.w, sRect.h);
			local_gwmgaugebar_FreeGaugebarObj(pGbObj);
			return GWM_MESSAGE_BREAK;

		case MT_DRAW:
			ONDK_GWM_APP_GetAppArea(g_pstCurrentApp, &sArea);
			INT_ONDK_GWM_OBJ_GetObjectRect(pObject, &sRect);
			if (pObject->objDrawMethod != NULL)
			{
				/* Customization Drawing Method */
				pObject->objDrawMethod(ONDK_GWM_GetSurface(), &sArea, &sRect, pObject, pGbObj, iArg);
			}
			else
			{
				/* Default Drawing Method */
				local_gwmgaugebar_DrawDefaultGaugebarMethod(ONDK_GWM_GetSurface(), &sArea, pGbObj, iArg);
			}
			return GWM_MESSAGE_BREAK;

		case MT_KEYDOWN:
			switch(iArg)
			{
				case KEY_ARROWLEFT:
					pGbObj->iValue--;
					if(pGbObj->iValue < pGbObj->iMin)
					{
						pGbObj->iValue = pGbObj->iMin;
						/* 의미없는 메시지인가 ???, delete ??? */
						ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_GAUGEBAR_LEFT_NOMORE, (HUINT32)NULL, pGbObj->ID, pGbObj->iValue, (HINT32)pGbObj, NULL);
						return GWM_MESSAGE_BREAK;
					}

					ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_GAUGEBAR_CHANGED, (HUINT32)NULL, pGbObj->ID, pGbObj->iValue, (HINT32)pGbObj, NULL);
					ONDK_GWM_APP_InvalidateRelativeRect( pGbObj->rect.x, pGbObj->rect.y, pGbObj->rect.w, pGbObj->rect.h );
					return GWM_MESSAGE_BREAK;

				case KEY_ARROWRIGHT:
					pGbObj->iValue++;
					if(pGbObj->iValue > pGbObj->iMax)
					{
						pGbObj->iValue = pGbObj->iMax;
						/* 의미없는 메시지인가 ???, delete ??? */
						ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_GAUGEBAR_RIGHT_NOMORE, (HUINT32)NULL, pGbObj->ID, pGbObj->iValue, (HINT32)pGbObj, NULL);
						return GWM_MESSAGE_BREAK;
					}

					ONDK_GWM_APP_Call(g_pstCurrentApp, MSG_GWM_GAUGEBAR_CHANGED, (HUINT32)NULL, pGbObj->ID, pGbObj->iValue, (HINT32)pGbObj, NULL);
					ONDK_GWM_APP_InvalidateRelativeRect( pGbObj->rect.x, pGbObj->rect.y, pGbObj->rect.w, pGbObj->rect.h );
					return GWM_MESSAGE_BREAK;

				default:
					break;
			}
			return GWM_MESSAGE_BREAK;

		case MT_KEYUP:
			return GWM_MESSAGE_BREAK;

		case MT_RELOCATE:
		{
			ONDK_Rect_t	*psRect;

			psRect = (ONDK_Rect_t*)iArg;
			ONDK_GWM_APP_InvalidateRelativeRect(pGbObj->rect.x, pGbObj->rect.y, pGbObj->rect.w, pGbObj->rect.h);
			VK_memcpy(&(pGbObj->rect), psRect, sizeof(ONDK_Rect_t));
			ONDK_GWM_APP_InvalidateRelativeRect(pGbObj->rect.x, pGbObj->rect.y, pGbObj->rect.w, pGbObj->rect.h);
			return	GWM_MESSAGE_BREAK;
		}

		case MT_SET_VALUE:
			pGbObj->iValue = iArg;
			ONDK_GWM_APP_InvalidateRelativeRect(pGbObj->rect.x, pGbObj->rect.y, pGbObj->rect.w, pGbObj->rect.h);
			return GWM_MESSAGE_BREAK;

		case MT_GET_FOCUS:
		case MT_LOST_FOCUS:
			ONDK_GWM_APP_InvalidateRelativeRect(pGbObj->rect.x, pGbObj->rect.y, pGbObj->rect.w, pGbObj->rect.h);
			break;

		default:
			break;
	}
	return GWM_MESSAGE_PASS;
}

/*******************************************************************/
/********************     Grobal Functions  ************************/
/*******************************************************************/
void gwm_gaugebar_InitClass(void)
{
	HINT32	iI;

	for (iI = 0; iI < MAX_GAUGEBAR_OBJECT; iI++)
	{
		s_sGaugebarArray[iI].link = NULL;
		VK_memset(&(s_sGaugebarArray[iI].rect), 0, sizeof(ONDK_Rect_t));
		s_sGaugebarArray[iI].ID = -1;
		s_sGaugebarArray[iI].type = GUIOBJ_GAUGEBAR;
		s_sGaugebarArray[iI].attr = 0;
		s_sGaugebarArray[iI].focus = GUIOBJ_NORMAL;
		s_sGaugebarArray[iI].Proc = local_gwmgaugebar_DefaultGaugebarProc;
		s_sGaugebarArray[iI].objDrawMethod = NULL;

		s_sGaugebarArray[iI].cFocusType = 0;
		s_sGaugebarArray[iI].pcTitle = NULL;
		s_sGaugebarArray[iI].iMin = 0;
		s_sGaugebarArray[iI].iMax = 0;
		s_sGaugebarArray[iI].iValue = 0;
		s_sGaugebarArray[iI].ulGBImgs_Addr = (HLONG)NULL;

		isAllocated[iI] = 0;
	}
}

/* Create Gauge Bar Object */
ONDK_Result_t GWM_Gaugebar_Create(HINT32 iId,
										ONDK_Rect_t *psRect,
										HINT8 cFocusType,
										HINT8 *pcTitle,
										HINT32 iMin,
										HINT32 iMax,
										HINT32 iValue,
										HLONG ulItemAddr)
{
	PGAUGEBAR_OBJECT	pNewGbObj;
	ONDK_Result_t		hresult;

	pNewGbObj = local_gwmgaugebar_GetGaugebarObject(iId);
	if (pNewGbObj != NULL)
	{
		return GWM_RET_LINK_CONFLICT;
	}

	pNewGbObj = local_gwmgaugebar_AllocNewGaugebarObj();
	if (pNewGbObj == NULL)
	{
		return	GWM_RET_OUT_OF_MEMORY;
	}

	pNewGbObj->link = NULL;
	VK_memcpy(&(pNewGbObj->rect), psRect, sizeof(ONDK_Rect_t));
	pNewGbObj->ID = iId;
	pNewGbObj->type = GUIOBJ_GAUGEBAR;
	pNewGbObj->focus = GUIOBJ_NORMAL;
	pNewGbObj->Proc = local_gwmgaugebar_DefaultGaugebarProc;
	pNewGbObj->objDrawMethod = NULL;

	pNewGbObj->cFocusType = cFocusType;
	pNewGbObj->pcTitle= pcTitle;
	pNewGbObj->iMin = iMin;
	pNewGbObj->iMax = iMax;
	pNewGbObj->iValue = iValue;
	pNewGbObj->ulGBImgs_Addr = ulItemAddr;
#if 0
	INT_ONDK_GWM_OBJ_CallObjectProc(MT_CREATE, (ONDK_GWM_GuiObject_t)pNewGbObj, 0);
#endif
	hresult = INT_ONDK_GWM_APP_AddObject(g_pstCurrentApp, (ONDK_GWM_GuiObject_t)pNewGbObj);
	ONDK_GWM_APP_InvalidateRelativeONDKRect(&pNewGbObj->rect);
	return	hresult;
}

HUINT32 GWM_Gaugebar_GetStatus(PGAUGEBAR_OBJECT pGbObj)
{
	if(NULL == pGbObj)
	{
		return 0;
	}
		GWM_assert(GUIOBJ_GAUGEBAR==pGbObj->type);

	return pGbObj->focus;
}

/* Get title string */
HINT8* GWM_Gaugebar_GetTitlePtr(PGAUGEBAR_OBJECT pGbObj)
{
	if(NULL == pGbObj)
	{
		return NULL;
	}
	GWM_assert(GUIOBJ_GAUGEBAR==pGbObj->type);

	return pGbObj->pcTitle;
}

/* Get focus type, min, max, value of adjust range */
void GWM_Gaugebar_GetProperty(PGAUGEBAR_OBJECT pGbObj, HINT8 *pcFocusType, HINT32 *piMin, HINT32 *piMax, HINT32 *piValue)
{
	if(NULL == pGbObj)
	{
		return;
	}
	GWM_assert(GUIOBJ_GAUGEBAR==pGbObj->type);

	*pcFocusType = pGbObj->cFocusType;
	*piMin = pGbObj->iMin;
	*piMax = pGbObj->iMax;
	*piValue = pGbObj->iValue;
}

/* Get Adjust bar images */
HLONG GWM_Gaugebar_GetItemsAddr(PGAUGEBAR_OBJECT pGbObj)
{
	if(NULL == pGbObj)
	{
		return (HLONG)NULL;
	}
	GWM_assert(GUIOBJ_GAUGEBAR==pGbObj->type);

	return pGbObj->ulGBImgs_Addr;
}

/* Set Adjust value */
ONDK_Result_t GWM_Gaugebar_SetValue(HINT32 iId, HINT32 iValue)
{
	PGAUGEBAR_OBJECT	pGbObj;

	pGbObj = (PGAUGEBAR_OBJECT)ONDK_GWM_APP_GetObject(iId);
	if (pGbObj == NULL)
	{
		return	GWM_RET_NO_OBJECT;
	}
	if (pGbObj->type != GUIOBJ_GAUGEBAR)
	{
		return	GWM_RET_NO_OBJECT;
	}

	pGbObj->iValue = iValue;

	ONDK_GWM_APP_InvalidateRelativeRect( pGbObj->rect.x, pGbObj->rect.y, pGbObj->rect.w, pGbObj->rect.h );

	return	GWM_RET_OK;
}

/* Change gauge bar properties of Current App */
ONDK_Result_t GWM_Gaugebar_ChangeProperty(HINT32 iId,
													HINT8 cFocusType,
													HINT8 *pcTitle,
													HINT32 iMin,
													HINT32 iMax,
													HINT32 iValue,
													HLONG ulItemAddr)
{
	PGAUGEBAR_OBJECT	pGbObj;

	pGbObj = (PGAUGEBAR_OBJECT)ONDK_GWM_APP_GetObject(iId);
	if (pGbObj == NULL)
	{
		return	GWM_RET_NO_OBJECT;
	}
	if (pGbObj->type != GUIOBJ_GAUGEBAR)
	{
		return	GWM_RET_NO_OBJECT;
	}

	pGbObj->cFocusType = cFocusType;
	pGbObj->pcTitle= pcTitle;
	pGbObj->iMin = iMin;
	pGbObj->iMax = iMax;
	pGbObj->iValue = iValue;
	pGbObj->ulGBImgs_Addr = ulItemAddr;

	ONDK_GWM_APP_InvalidateRelativeRect( pGbObj->rect.x, pGbObj->rect.y, pGbObj->rect.w, pGbObj->rect.h );

	return	GWM_RET_OK;
}

/*********************** End of File ******************************/

