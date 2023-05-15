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

#include "gwm_multiline.h"


#ifdef CONFIG_DEBUG
HUINT32 g_GwmMultilineDebug_Level = (DBG_ASSERT); //DBG_TRACE|DBG_PRINT|DBG_DATA;
HUINT32 g_GwmMultilineDebug_Color =	0;

#define GwmMultilinePrint(level, message)		(((level) & g_GwmMultilineDebug_Level) ? (HxLOG_Print("\033[01;%dm", g_GwmMultilineDebug_Color), HxLOG_Print("[GWM:Animation]"), HxLOG_Print message, HxLOG_Print("\033[0m")) : 0)
#define GwmMultilineAssert(c) 				((c) ? 0 : (HxLOG_Print("\033[01;%dm", g_GwmMultilineDebug_Color), HxLOG_Print("\nAnimation assertion on %s: line%d\n\n", __FILE__, __LINE__), HxLOG_Print("\033[00m")))
#define GwmMultilineData(level, message)		(((level) & g_GwmMultilineDebug_Level) ? ( HxLOG_Print message) : 0)
#else
#define GwmMultilinePrint(level, message)
#define GwmMultilineAssert(c)
#define GwmMultilineData(level, message)
#endif

/* ------------------------------------------------------------ */
/*	DEFINITIONS.										     		     */
/* ------------------------------------------------------------ */
#define PT_IN_LINE				2
#define PT_IN_BOUND				3
#define SCROLL_WIDTH			26

/* ------------------------------------------------------------ */
/*	GLOBAL VARIABLES.										     */
/* ------------------------------------------------------------ */

STATIC struct tagMultilineTextObject	gGwmMultilineObject;

/* ------------------------------------------------------------ */
/*	FUNCTION DEFINITIONS.									     */
/* ------------------------------------------------------------ */

HINT32				getChByteNum(HINT8 *str, HINT32 pos, HINT8 *retCh)
{
	HINT32			num = 0, index = 0;
	HBOOL		isRoman;
	HUINT8	ch;

	if (pos <= 0)
		return 0;

	while (index < pos)
	{
		ch = str[index];

		if (ch & 0x80)
		{
			index += 2;
			isRoman = FALSE;
		}
		else
		{
			index++;
			isRoman = TRUE;
		}

		if (index >= pos)
		{
			if (isRoman)
			{
				num = 1;
				retCh[0] = str[index - 1];
				retCh[1] = '\0';
			}
			else
			{
				num = 2;
				retCh[0] = str[index - 1];
				retCh[1] = str[index];
			}
			break;
		}
	}

	return num;
}

HINT32 					getRoughPosition(HINT8 *str, HINT32 offset)
{
	HUINT8			ch;
	int				pos = 0, len = 0;

	if (str == NULL || offset == 0)
		return 0;

	len = VK_strlen(str);
	while (pos < len)
	{
		ch = str[pos];

		if (ch & 0x80)
			pos++;

		pos++;

		if (pos + 1 >= offset)
			break;
	}

	return pos;
}


HUINT8 *				getLineText(MULTILINE_OBJECT obj, HINT32 w, HINT8 *string, HINT32 *outCharNum)
{
	HINT32				tx, width;
	STATIC HINT8		str[256];
	HINT32				count, total;

	tx = 0;
	count = 1;
	total = 0;

	// if this cannot draw a line at least...
	if (w < obj->font->width)
		return NULL;

	VK_memset(str, 0x00, 256);
	while ((*string) != '\0')
	{
		if(string[0] != 0x15)
		{
			width = OSD_GFX_JapanGetStringWidth(obj->font, (const HUINT8*)string);
		}
		else
		{
			width = ONDK_GWM_GFX_GetStringWidth(obj->font, string);
		}

		// actually this doesn't draw a character, just getting the number of bytes processed

		count = VK_strlen(string);

		// If the component is for password, encrypt the actual data
		if (obj->isEncrypt && *string != '\n')
			VK_strncat(str, "**", count);
		else
			VK_strncat(str, string, count);

		string += count;
		total += count;

		tx += width;

		if (tx + width > w)
		{
			tx = 0;
			break;
		}

		if (*string == '\n')
			break;
	}
	*outCharNum = total;

	return str;
}

LINE_Data_t *			splitTextByLine(MULTILINE_OBJECT obj, HUINT16 *outLineNum)
{
	HINT32				lineWidth = 0, charNum = 0, startPos = 0, lineNum = 0, len = 0;
	HINT8				*pStr = NULL, *pLineStr = NULL;
	LINE_Data_t		*pLine = NULL, *elem = NULL;

	if (obj == NULL)
		return NULL;
	// break the text line by line
	lineWidth = obj->rect.w - SCROLL_WIDTH;
	pStr = obj->buf;
	startPos = charNum = 0;
	pLine = (LINE_Data_t *)ONDK_Calloc(sizeof(LINE_Data_t)*1);
	if (pLine==NULL)
		return	NULL;

	while (pStr || charNum > 0)
	{


		pLineStr = getLineText(obj, lineWidth, pStr, &charNum);
		//pLineStr = OSD_GFX_GetStringLine(obj->font, lineWidth, pStr, &charNum);
		if (pLineStr != NULL && charNum > 0)
		{
			len = VK_strlen(pLineStr);			// in order to except '\n' character

			// create line data element
			// the splitted string may have '\n' character in the 1st of the array
			elem = (LINE_Data_t *)ONDK_Calloc(sizeof(LINE_Data_t)*1);

			if (elem == NULL)
			{
				GwmMultilineAssert(elem);
				ONDK_Free(pLine);

				return NULL;
			}

			elem->start = startPos;
			elem->end = startPos + len;
			elem->lineIndex = lineNum;
			elem->data = (char *)ONDK_Calloc(sizeof(char)*(len + 1));

			if (elem->data == NULL)
			{
				GwmMultilineAssert(elem->data);
				ONDK_Free(elem);
				ONDK_Free(pLine);

				return NULL;
			}

			VK_strncpy(elem->data, pLineStr, len);

			// make linked list
			if (pLine->pNext == NULL)
				pLine->pNext = elem;
			else
			{
				LINE_Data_t		*pTemp = NULL;

				pTemp = pLine->pNext;
				while (pTemp)
				{
					if (pTemp->pNext == NULL)
					{
						pTemp->pNext = elem;
						break;
					}
					pTemp = pTemp->pNext;
				}
			}
			lineNum++;
		}
		else
			break;

		pStr = pStr + charNum;
		startPos += charNum;
	}
	*outLineNum = lineNum;

	// reset the scroll position
	elem = pLine->pNext;
	while (elem)
	{
		// the cursor always shows on a visible page,
		// so sometimes, it should adjust its position by itself.
		if ((obj->moveCursor > elem->start && obj->moveCursor <= elem->end)
			|| (obj->moveCursor == elem->end && lineNum - 1 == elem->lineIndex))
		{
			obj->lineOfCursor = elem->lineIndex;

			if (obj->lineOfCursor < obj->pageStart)
			{
				obj->pageStart = obj->lineOfCursor;
				obj->pageEnd = obj->lineOfCursor + (obj->viewLineNum - 1);
			}
			else if (obj->lineOfCursor >= obj->pageStart
					&& obj->lineOfCursor <= obj->pageEnd)
			{
				;		// don't need to scroll
			}
			else if (obj->lineOfCursor > obj->pageEnd)
			{
				obj->pageStart++;
				obj->pageEnd++;
			}
		}
		elem = elem->pNext;
	}

	return pLine;
}



void					disposeLineText(LINE_Data_t *pLineText)
{
	LINE_Data_t		*elem = NULL;

	if (pLineText != NULL)
	{
		LINE_Data_t		*pNextElem = NULL;

		elem = pLineText->pNext;
		while (elem)
		{
			if (elem != NULL)
			{
				pNextElem = elem->pNext;
				ONDK_Free(elem->data);
				ONDK_Free(elem);
			}
			elem = pNextElem;
		}
		pLineText->pNext = NULL;
		ONDK_Free(pLineText);
	}
}


LINE_Data_t *			getLineByCursor(MULTILINE_OBJECT obj, HUINT32 cursorPos)
{
	LINE_Data_t		*pElem = NULL, *pLine = NULL;

	if (obj == NULL)
		return NULL;
	if (obj->pLine == NULL)		// by ccash@humax
		return NULL;

	if (cursorPos < 0)
		return NULL;

	pElem = obj->pLine->pNext;
	while (pElem)
	{
		if (obj->lineOfCursor == pElem->lineIndex)
		{
			pLine = pElem;
			break;
		}

		pElem = pElem->pNext;
	}

	return pLine;
}


LINE_Data_t *			getLineByIndex(MULTILINE_OBJECT obj, HINT32 index)
{
	LINE_Data_t		*pElem = NULL, *pLine = NULL;
	HUINT16			num = 0;

	if (obj == NULL)
		return NULL;
	if (obj->pLine == NULL)		// by ccash@humax
		return NULL;

	if (index < 0)
		return NULL;

	pElem = obj->pLine->pNext;
	while (pElem)
	{
		if (index == num)
		{
			pLine = pElem;
			break;
		}

		pElem = pElem->pNext;
		num++;
	}

	return pLine;
}


void		GWM_InitializeMultilineTextClass(void)
{
	gGwmMultilineObject.ID = -1;
	gGwmMultilineObject.type = GUIOBJ_MULTILINETEXT;
	gGwmMultilineObject.Proc = NULL;
	gGwmMultilineObject.objDrawMethod = NULL;
	gGwmMultilineObject.focus = GUIOBJ_NORMAL;
	gGwmMultilineObject.rect.x = -1;
	gGwmMultilineObject.rect.y = -1;
	gGwmMultilineObject.rect.w = 0;
	gGwmMultilineObject.rect.h = 0;

	gGwmMultilineObject.buf = NULL;
	gGwmMultilineObject.size = 0;

	gGwmMultilineObject.selStart = POS_NOT_DEF;
	gGwmMultilineObject.selEnd = POS_NOT_DEF;
	gGwmMultilineObject.cursor = 0;
	gGwmMultilineObject.moveCursor = POS_NOT_DEF;
	gGwmMultilineObject.pageStart = 0;
	gGwmMultilineObject.pageEnd = 0;
	gGwmMultilineObject.lineOfCursor = 0;
	gGwmMultilineObject.lineNum = 0;
	gGwmMultilineObject.pLine = NULL;

	gGwmMultilineObject.viewLineNum = 0;
	gGwmMultilineObject.font = NULL;

	gGwmMultilineObject.sInverted = POS_NOT_DEF;
	gGwmMultilineObject.eInverted = POS_NOT_DEF;

	gGwmMultilineObject.isEncrypt = FALSE;

	gGwmMultilineObject.pDrawFunc = NULL;
}

STATIC MULTILINE_OBJECT 		getMultilineTextObject(void)
{
	return &gGwmMultilineObject;
}



STATIC ONDK_Result_t	DeleteMultilineTextObject(MULTILINE_OBJECT MultitextObj)
{
	MultitextObj->ID = -1;
	MultitextObj->type = GUIOBJ_MULTILINETEXT;
	MultitextObj->Proc = NULL;
	MultitextObj->objDrawMethod = NULL;
	MultitextObj->focus = GUIOBJ_NORMAL;
	MultitextObj->rect.x = -1;
	MultitextObj->rect.y = -1;
	MultitextObj->rect.w = 0;
	MultitextObj->rect.h = 0;

	// CAUTIOUS : NEVER deallocate data storage by itself
	MultitextObj->size = 0;
	MultitextObj->selStart = POS_NOT_DEF;
	MultitextObj->selEnd = POS_NOT_DEF;
	MultitextObj->cursor = 0;
	MultitextObj->moveCursor = POS_NOT_DEF;
	MultitextObj->pageStart = POS_NOT_DEF;
	MultitextObj->pageEnd = POS_NOT_DEF;
	MultitextObj->lineOfCursor = POS_NOT_DEF;
	MultitextObj->lineNum = 0;

	// dispose the line text data
	disposeLineText(MultitextObj->pLine);

	MultitextObj->sInverted = POS_NOT_DEF;
	MultitextObj->eInverted = POS_NOT_DEF;

	MultitextObj->isEncrypt = FALSE;

	return GWM_RET_OK;
}


void			drawSelection(ONDKSurfaceHandle pixmap, int lineStartX, int lineStartY,	MULTILINE_OBJECT obj, LINE_Data_t *pElem)
{
	char				temp[LINE_BUFF_SIZE];
	int				startX = 0, endX = 0;

	if (obj->sInverted != POS_NOT_DEF && obj->eInverted != POS_NOT_DEF)
	{
		if ((obj->sInverted >= pElem->start && obj->sInverted < pElem->end)
			&& (obj->eInverted > pElem->start && obj->eInverted <= pElem->end))
		{	/* included in a line */
			memset(temp, 0x00, LINE_BUFF_SIZE);
			VK_strncpy(temp, pElem->data, obj->sInverted - pElem->start);
			startX = OSD_GFX_JapanGetStringWidth(obj->font, temp);

			memset(temp, 0x00, LINE_BUFF_SIZE);
			VK_strncpy(temp, pElem->data, obj->eInverted - pElem->start);
			endX = OSD_GFX_JapanGetStringWidth(obj->font, temp);
		}
		else if ((obj->sInverted >= pElem->start && obj->sInverted < pElem->end)
			&& (obj->eInverted > pElem->end))
		{	/* only start included */
			memset(temp, 0x00, LINE_BUFF_SIZE);
			VK_strncpy(temp, pElem->data, obj->sInverted - pElem->start);
			startX = OSD_GFX_JapanGetStringWidth(obj->font, temp);
			endX = OSD_GFX_JapanGetStringWidth(obj->font, pElem->data);
		}
		else if ((obj->sInverted < pElem->start)
			&& (obj->eInverted > pElem->start && obj->eInverted <= pElem->end))
		{	/* only end included */
			memset(temp, 0x00, LINE_BUFF_SIZE);
			VK_strncpy(temp, pElem->data, obj->eInverted - pElem->start);
			startX = 0;
			endX = OSD_GFX_JapanGetStringWidth(obj->font, temp);
		}
		else if ((obj->sInverted < pElem->start && obj->eInverted > pElem->end))
		{	/* whole line is selected */
			startX = 0;
			endX = OSD_GFX_JapanGetStringWidth(obj->font, pElem->data);
		}
		else
		{
			return;
		}

		ONDK_DRAW_FillRectElement	(pixmap,
				lineStartX + startX, lineStartY,
				lineStartX + endX, lineStartY + obj->font->height + 2,
				obj->colorSelection);
	}
}

void	drawCaret(ONDKSurfaceHandle pixmap,		HINT32 lineStartX, HINT32 lineStartY, MULTILINE_OBJECT obj, LINE_Data_t *pElem)
{
	HUINT8				temp[LINE_BUFF_SIZE];
	HINT32				caretX;

	if (pElem->lineIndex == obj->lineOfCursor)
	{
		memset(temp, 0x00, LINE_BUFF_SIZE);
		VK_strncpy(temp, pElem->data, obj->moveCursor - pElem->start);

		caretX = lineStartX +  OSD_GFX_JapanGetStringWidth(obj->font, temp);

		ONDK_DRAW_FillRectElement(pixmap, caretX, lineStartY, caretX + 2, lineStartY + obj->font->height, obj->colorCaret);
	}
}


void	drawUndeline(ONDKSurfaceHandle pixmap,	HINT32 lineStartX, HINT32 lineStartY, MULTILINE_OBJECT obj, LINE_Data_t *pElem)
{
	HINT8	temp[LINE_BUFF_SIZE];
	HINT32		startX = 0, endX = 0;

	if ((obj->selEnd - obj->selStart) > 0)
	{
		if ((obj->selStart >= pElem->start && obj->selStart < pElem->end)
			&& (obj->selEnd > pElem->start && obj->selEnd <= pElem->end))
		{	/* included in a line */
			memset(temp, 0x00, LINE_BUFF_SIZE);
			VK_strncpy(temp, pElem->data, obj->selStart - pElem->start);

			startX = OSD_GFX_JapanGetStringWidth(obj->font, temp);

			memset(temp, 0x00, LINE_BUFF_SIZE);
			VK_strncpy(temp, pElem->data, obj->selEnd - pElem->start);
			endX = OSD_GFX_JapanGetStringWidth(obj->font, temp);

		}
		else if ((obj->selStart >= pElem->start && obj->selStart < pElem->end)
			&& (obj->selEnd > pElem->end))
		{	/* only start included */
			memset(temp, 0x00, LINE_BUFF_SIZE);
			VK_strncpy(temp, pElem->data, obj->selStart - pElem->start);

			startX = OSD_GFX_JapanGetStringWidth(obj->font, temp);
			endX = OSD_GFX_JapanGetStringWidth(obj->font, pElem->data);
		}
		else if ((obj->selStart < pElem->start)
			&& (obj->selEnd > pElem->start && obj->selEnd <= pElem->end))
		{	/* only end included */
			memset(temp, 0x00, LINE_BUFF_SIZE);
			VK_strncpy(temp, pElem->data, obj->selEnd - pElem->start);
			startX = 0;
			endX = OSD_GFX_JapanGetStringWidth(obj->font, temp);

		}
		else if ((obj->selStart < pElem->start && obj->selEnd > pElem->end))
		{	/* whole line is selected */
			startX = 0;
			endX = OSD_GFX_JapanGetStringWidth(obj->font, pElem->data);

		}
		else
		{
			return;
		}

		ONDK_DRAW_FillRectElement(pixmap,
				lineStartX + startX, lineStartY + obj->font->height,
				lineStartX + endX, lineStartY + obj->font->height + 1,
				obj->colorUnderline);
	}
}


void	drawEditingText(ONDKSurfaceHandle pixmap, MULTILINE_OBJECT obj)
{
	ONDK_Region_t		area;
	HINT32				x, y, w, h, offset = 0;
	LINE_Data_t		*pElem = NULL;
	HINT32			    nUsedByte = 0;
	HINT32				nUcs2Len = 0;
	HINT32				nUtf8Len = 0;
	HUINT16			szUcs2Str[256];
	HINT8			szString[256];

	ONDK_GWM_APP_GetAppArea(g_pstCurrentApp, &area);
	x = area.x1 + obj->rect.x;
	y = area.y1 + obj->rect.y;
	w = obj->rect.w;
	h = obj->rect.h;

	VK_memset(szUcs2Str,0x00, sizeof(char)*256);
	VK_memset(szString,0x00, sizeof(char)*256);
	// draw background
	if (obj->pageStart != POS_NOT_DEF && obj->pageEnd != POS_NOT_DEF)
	{
		GWM_assert(obj->pLine);
		pElem = obj->pLine->pNext;
		if (pElem == NULL)
		{	/* In case that there's no text data to render */
			ONDK_DRAW_FillRectElement(pixmap, x + PT_IN_BOUND, y + PT_IN_BOUND, x + PT_IN_BOUND + 2, y + PT_IN_BOUND + obj->font->height, obj->colorCaret);
		}

		while (pElem)
		{
			if ((pElem->lineIndex >= obj->pageStart)
				&& (pElem->lineIndex <= obj->pageEnd))
			{
				// draw inverted rect for text which converting progressing
				drawSelection(pixmap, x + PT_IN_BOUND, y + PT_IN_BOUND + offset, obj, pElem);

				// draw a line of text
				if (obj->sInverted != POS_NOT_DEF && obj->eInverted != POS_NOT_DEF)
				{
					if(pElem->data[0] != 0x20)
					{
						//sjis->ucs2
						MWC_UTIL_ConvertEncodingToUCS2 (eHxCP_ISO_JAPAN_SJIS, (char *)pElem->data,
							VK_strlen(pElem->data), &nUsedByte, szUcs2Str, &nUcs2Len);

						//ucs2->utf8
						szString[0]=0x15;
						//MWC_UTIL_ConvertUCS2ToUtf8 (szUcs2Str, nUcs2Len, 128, &nUsedByte, &szString[1], &nUtf8Len);
						szString[1+nUtf8Len]='\0';
					} else {
						szString[0] = 0x20;
					}
					ONDK_FONT_DrawString(pixmap, obj->font, x+ PT_IN_BOUND, y + PT_IN_BOUND + offset, (char *)szString, obj->colorSelectedText, ONDKSTF_LEFT);
				}
				else
				{
					if(pElem->data[0] != 0x20)
					{
						//sjis->ucs2
						MWC_UTIL_ConvertEncodingToUCS2 (eHxCP_ISO_JAPAN_SJIS, (char *)pElem->data, VK_strlen(pElem->data), &nUsedByte, szUcs2Str, &nUcs2Len);

						//ucs2->utf8
						szString[0]=0x15;
						//MWC_UTIL_ConvertUCS2ToUtf8 (szUcs2Str, nUcs2Len, 128, &nUsedByte, &szString[1], &nUtf8Len);
						szString[1+nUtf8Len]='\0';
					} else {
						szString[0] = 0x20;
					}
					ONDK_FONT_DrawString(pixmap, obj->font, x+ PT_IN_BOUND, y + PT_IN_BOUND + offset, (char *)szString, obj->colorText, ONDKSTF_LEFT);
				}

				// draw a caret
				drawCaret(pixmap, x + PT_IN_BOUND, y + PT_IN_BOUND + offset, obj, pElem);

				// draw an underline for text to convert
				drawUndeline(pixmap, x + PT_IN_BOUND, y + PT_IN_BOUND + offset, obj, pElem);

				offset += obj->font->height + 2;
			}
			pElem = pElem->pNext;
		}
	}
	else
	{	/* In case that there's no text data to render */
		ONDK_DRAW_FillRectElement(pixmap, x + PT_IN_BOUND, y + PT_IN_BOUND,
			x + PT_IN_BOUND + 2, y + PT_IN_BOUND + obj->font->height, obj->colorCaret);
	}


	if (obj->pageStart > 0 && obj->pageStart != POS_NOT_DEF)
	{
		//osdDraw_Bitmap(pixmap, x+w+16, y+3, (unsigned char *)&icon_03_keyboard_ar_01);
	}
	else
	{
		//osdDraw_Bitmap(pixmap, x+w+16, y+3, (unsigned char *)&icon_03_keyboard_ar_02);
	}

	if (obj->pageEnd < obj->lineNum - 1 && obj->pageEnd != POS_NOT_DEF)
	{
		//osdDraw_Bitmap(pixmap, x+w+16, y+h-13, (unsigned char *)&icon_03_keyboard_ar_03);
	}
	else
	{
		//osdDraw_Bitmap(pixmap, x+w+16, y+h-13, (unsigned char *)&icon_03_keyboard_ar_04);
	}
}



ONDK_Result_t		DefaultMultilineTextMethod(GWM_Obj_MethodType_t method_type, struct tagGuiObject *object, int arg)
{
	MULTILINE_OBJECT		MultilineObj;

	MultilineObj = (MULTILINE_OBJECT)object;

	switch (method_type)
	{
		case MT_DRAW:

			if (NULL != MultilineObj->pDrawFunc)
			{
				MultilineObj->pDrawFunc(object->ID, ONDK_GWM_GetSurface(), &object->rect);
			}
			else
			{
				drawEditingText(ONDK_GWM_GetSurface(), MultilineObj);
			}
			return GWM_MESSAGE_BREAK;

		case MT_DESTROY:
			DeleteMultilineTextObject(MultilineObj);
			ONDK_GWM_APP_InvalidateObject(object->ID);
			return GWM_MESSAGE_BREAK;

		default:
			break;

	}
	//GWM_APP_InvalidateAPP(NULL);
	return GWM_MESSAGE_PASS;
}




ONDK_Result_t				GWM_CreateMultilineTextObject(int ID, ONDK_Rect_t *rect,
											char *buffer, HUINT32 buffSize, 	int viewLine,
											HBOOL isPassword, ONDKFontHandle font,
											MULTILINE_OBJECT *objPtr, int originalKeyboard)
{
	ONDK_Result_t				hResult = GWM_RET_OK;
	ONDK_GWM_GuiObject_t			objlist;
	MULTILINE_OBJECT		newMulText;
	LINE_Data_t				*pLine = NULL, *elem = NULL;
	HUINT32					dataLen = 0;
	HUINT16					lineNum = 0;

	GWM_assert(g_pstCurrentApp);
	objlist = INT_ONDK_GWM_APP_GetObjectList(g_pstCurrentApp);

	newMulText = (MULTILINE_OBJECT)INT_ONDK_GWM_OBJ_FindObject(objlist, ID);

	if (newMulText != NULL)
	{
		return GWM_RET_LINK_CONFLICT;
	}
	newMulText = getMultilineTextObject();

	if (newMulText == NULL)
	{
		return GWM_RET_OUT_OF_MEMORY;
	}

	// initialize structure
	newMulText->ID = ID;
	newMulText->type = GUIOBJ_MULTILINETEXT;
	newMulText->Proc = DefaultMultilineTextMethod;
	newMulText->objDrawMethod = NULL;
	newMulText->focus = GUIOBJ_NORMAL;

	newMulText->buf = buffer;
	newMulText->size = buffSize;

	newMulText->selStart = POS_NOT_DEF;
	newMulText->selEnd = POS_NOT_DEF;
	newMulText->cursor = 0;
	newMulText->moveCursor = 0;
	newMulText->pageStart = POS_NOT_DEF;
	newMulText->pageEnd = POS_NOT_DEF;
	newMulText->lineOfCursor = 0;
	newMulText->lineNum = 0;
	newMulText->pLine = NULL;

	newMulText->viewLineNum = viewLine;
	newMulText->font = font;
	if(originalKeyboard == 1)
	{
		newMulText->colorCaret= 0xFF003399;
		newMulText->colorText= 0xFF003399;
		newMulText->colorUnderline= 0xFF003399;
		newMulText->colorSelection = 0xFF003399;
		newMulText->colorSelectedText = 0xFFDCDCDC;
	}
	else
	{
		newMulText->colorCaret= 0;
		newMulText->colorText= 0;
		newMulText->colorUnderline= 0;
		newMulText->colorSelectedText = 0;
		newMulText->colorSelection = 147;
	}

	newMulText->sInverted = POS_NOT_DEF;
	newMulText->eInverted = POS_NOT_DEF;

	newMulText->isEncrypt = isPassword;

	memcpy(&(newMulText->rect), rect, sizeof(ONDK_Rect_t));

	// adjust the height of the bound rectangle
	if (rect->h > (viewLine * (font->height + PT_IN_LINE) + PT_IN_BOUND * 2))
		newMulText->rect.h = viewLine * (font->height + PT_IN_LINE) + PT_IN_BOUND * 2;


	hResult = INT_ONDK_GWM_APP_AddObject(g_pstCurrentApp, (ONDK_GWM_GuiObject_t)newMulText);
	if (objPtr != NULL)
		*objPtr = newMulText;



	// If there's any initial data, make up scroll position
	dataLen = VK_strlen(buffer);
	if (dataLen > 0)
	{
		// the last cursor position will be the end of the data
		newMulText->cursor = dataLen;
		newMulText->moveCursor = dataLen;

		// split the data line by line
		pLine = splitTextByLine(newMulText, &lineNum);

		newMulText->lineNum = lineNum;
		newMulText->pLine = pLine;

		// set up the scroll & cursor position
		elem = pLine->pNext;
		while (elem)
		{
			if (newMulText->cursor == elem->end && lineNum - 1 == elem->lineIndex)
			{
				if (elem->lineIndex < newMulText->viewLineNum)
				{
					newMulText->pageStart = 0;
					newMulText->pageEnd = elem->lineIndex;
				}
				else
				{
					newMulText->pageStart = elem->lineIndex - (newMulText->viewLineNum - 1);
					newMulText->pageEnd = elem->lineIndex;
				}
				// remember the line index which the cursor stays
				newMulText->lineOfCursor = elem->lineIndex;
			}
			elem = elem->pNext;
		}
	}

	ONDK_GWM_APP_InvalidateRelativeRect(rect->x, rect->y, rect->w, rect->h);

	return hResult;
}


ONDK_Result_t				GWM_SetMultilinePosition(MULTILINE_OBJECT obj,
											HUINT32 caret,
											HUINT32 moveCaret,
											HUINT32 underlineStart,
											HUINT32 underlineEnd,
											HBOOL isDirty)
{
	LINE_Data_t			*pLine = NULL;
	HUINT16				lineNum = 0;

	if (obj == NULL)
		return GWM_RET_NO_OBJECT;

	// update position information
	obj->selStart = underlineStart;
	obj->selEnd = underlineEnd;
	obj->cursor = caret;
	obj->moveCursor = moveCaret;

	// Dirty bit turning on, make text line data and reset position info again
	if (isDirty)
	{
		// re-split the text by line
		if (obj->pLine != NULL)
		{
			disposeLineText(obj->pLine);
			obj->pLine = NULL;
		}

		pLine = splitTextByLine(obj, &lineNum);
		obj->lineNum = lineNum;
		obj->pLine = pLine;
	}

	return GWM_RET_OK;
}


ONDK_Result_t				GWM_MoveLeftMultilineCursor(MULTILINE_OBJECT obj, HUINT32 *outPos)
{
	HBOOL			isInInput;
	int				byteNum;
	LINE_Data_t		*pLine = NULL;
	char				retCh[2];

	if (obj == NULL)
		return GWM_RET_NO_OBJECT;

	// cursor is located at the beggining of the text
	if (obj->cursor <= 0)
		return GWM_RET_OK;

	if (obj->moveCursor <= 0)
		return GWM_RET_OK;

	// restrict the cursor's moves in underlined range
	isInInput = (obj->cursor - obj->selStart > 0) ? TRUE : FALSE;

	pLine = getLineByCursor(obj, obj->moveCursor);
	if (pLine != NULL)
	{
		// move to the end of the previous line
		if ((obj->moveCursor - pLine->start) <= 0)
		{
			if (isInInput && obj->moveCursor - obj->selStart <= 0)
				return -1;

			if (pLine->lineIndex == obj->pageStart)
			{	// the first line of the page
				if (pLine->lineIndex > 0 && obj->pageStart > 0)
				{
					pLine = getLineByIndex(obj, pLine->lineIndex - 1);
					obj->lineOfCursor--;
					obj->moveCursor = pLine->end;
					obj->pageStart--;
					obj->pageEnd--;
					*outPos = obj->moveCursor;
					return -1;
				}
			}
			else
			{	/* goes to the last of previous line, NOT to a new-line inserted line */
				pLine = getLineByIndex(obj, pLine->lineIndex - 1);
				obj->lineOfCursor--;
				obj->moveCursor = pLine->end;
				*outPos = obj->moveCursor;
				return -1;
			}
		}

		byteNum = getChByteNum(pLine->data, (obj->moveCursor - pLine->start), retCh);

		// each line string may start with '\n' character for line breaking
		if (byteNum == 1 && retCh[0] == '\n')
		{
			if (pLine->lineIndex == obj->pageStart)
			{	// the first line of the page
				if (isInInput)
					return -1;

				if (pLine->lineIndex > 0 && obj->pageStart > 0)
				{
					pLine = getLineByIndex(obj, pLine->lineIndex - 1);
					GWM_assert(pLine);
					if (pLine==NULL)
						return -1;
					obj->lineOfCursor--;
					obj->moveCursor = pLine->end;
					obj->pageStart--;
					obj->pageEnd--;
					*outPos = obj->moveCursor;
					return GWM_RET_OK;
				}
			}
			else
			{	/* goes to the last of previous line, to a new-line inserted line */
				if (isInInput)
					return -1;

				pLine = getLineByIndex(obj, pLine->lineIndex - 1);
				if (pLine==NULL)
					return -1;
				obj->lineOfCursor--;
				obj->moveCursor = pLine->end;
				*outPos = obj->moveCursor;
				return GWM_RET_OK;
			}
		}

		if (obj->cursor > 0)
		{
			if (isInInput)
			{
				if (obj->moveCursor > obj->selStart)
				{
					obj->moveCursor -= byteNum;
				}
			}
			else
			{
				if (obj->moveCursor > 0)
				{
					obj->moveCursor -= byteNum;
				}
			}

			if (obj->moveCursor < 0)
				obj->moveCursor = 0;
		}
		*outPos = obj->moveCursor;
	}
	else
		return -1;

	return GWM_RET_OK;
}


ONDK_Result_t				GWM_MoveRightMultilineCursor(MULTILINE_OBJECT obj, HUINT32 *outPos)
{
	HBOOL			isInInput;
	int				len;
	LINE_Data_t		*pLine = NULL;
	unsigned char		ch;

	if (obj == NULL)
		return GWM_RET_NO_OBJECT;

	// restrict the cursor's moves in underlined range
	isInInput = (obj->cursor - obj->selStart > 0) ? TRUE : FALSE;

	pLine = getLineByCursor(obj, obj->moveCursor);
	if (pLine != NULL)
	{
		// cursor is located at the last of the text
		if (obj->moveCursor >= pLine->end && obj->lineNum - 1 == pLine->lineIndex)
			return GWM_RET_OK;

		ch = pLine->data[obj->moveCursor - pLine->start];
		len = VK_strlen(pLine->data);

		if (obj->moveCursor - pLine->start > len - 1)
		{
			if (pLine->lineIndex < obj->lineNum - 1)
			{	/* goes to the first character of next line, considering of '\n' */
				if ((pLine->lineIndex == obj->pageEnd)
					&& (pLine->lineIndex < obj->lineNum))
				{
					pLine = getLineByIndex(obj, pLine->lineIndex + 1);
					if (pLine != NULL && pLine->data != NULL)
					{
						obj->lineOfCursor++;
						obj->moveCursor = (pLine->data[0] == '\n')
											? pLine->start + 1 : pLine->start;
						obj->pageStart++;
						obj->pageEnd++;
						*outPos = obj->moveCursor;
						return -1;
					}
				}
				else
				{
					pLine = getLineByIndex(obj, pLine->lineIndex + 1);
					if (pLine != NULL && pLine->data != NULL)
					{
						obj->lineOfCursor++;
						obj->moveCursor = (pLine->data[0] == '\n')
											? pLine->start + 1 : pLine->start;
						*outPos = obj->moveCursor;
						return -1;
					}
				}
			}
		}

		if (isInInput)
		{
			if (obj->moveCursor < obj->cursor)
			{
				if (ch & 0x80)
					obj->moveCursor += 2;
				else
					obj->moveCursor += 1;
			}
		}
		else
		{
			if (obj->moveCursor < len + pLine->start)
			{
				if (ch & 0x80)
					obj->moveCursor += 2;
				else
					obj->moveCursor += 1;
			}
		}
		*outPos = obj->moveCursor;
	}
	else
	{
		return -1;
	}

	return GWM_RET_OK;
}


/* 커서가 위/아래로 이동시에는, 이동 전의 column 위치를 기억했다가, 다음 line 의 같은 column 에 위치하도록 한다. */
ONDK_Result_t				GWM_MoveUpMultilineCursor(MULTILINE_OBJECT obj, HUINT32 *outPos)
{
	LINE_Data_t			*pLine = NULL;
	HUINT16				offset = 0;
	HUINT8				ch;

	if (obj == NULL)
		return GWM_RET_NO_OBJECT;

	if ( (obj->lineOfCursor == 0) && (obj->pageStart == 0) )
		return GWM_RET_OK;

	if (obj->selEnd - obj->selStart > 0)
		return GWM_RET_OK;

	// get the offset of current line, offset 은 cursor 의 column position 을 의미한다.
	pLine = getLineByIndex(obj, obj->lineOfCursor);
	if (pLine != NULL)
		offset = obj->moveCursor - pLine->start;
	else
		offset = 0;		// pLine 이 NULL 인 경우에는 cursor 의 위梁?column 은 0 으로 하여, 가장 왼쪽에 커서가 위치하는 것으로 가정함.

	if (obj->lineOfCursor == obj->pageStart)
	{	/* shift 1 line upward */
		obj->pageStart--;
		obj->pageEnd--;
	}

	pLine = getLineByIndex(obj, obj->lineOfCursor - 1);
	if (pLine != NULL && pLine->data != NULL)
	{
		obj->lineOfCursor--;
		if (offset <= 0)
		{
			ch = pLine->data[0];
			obj->moveCursor = (ch == '\n') ? pLine->start + 1 : pLine->start;
		}
		else if (offset > 0 && offset <= (pLine->end - pLine->start))
		{
			obj->moveCursor = pLine->start + getRoughPosition(pLine->data, offset);
		}
		else
		{
			obj->moveCursor = pLine->end;
		}
	}

	*outPos = obj->moveCursor;

	return GWM_RET_OK;
}



/* 커서가 위/아래로 이동시에는, 이동 전의 column 위치를 기억했다가, 다음 line 의 같은 column 에 위치하도록 한다. */
ONDK_Result_t				GWM_MoveDownMultilineCursor(MULTILINE_OBJECT obj,	HUINT32 *outPos)
{
	LINE_Data_t			*pLine = NULL;
	HUINT16				offset = 0;
	HUINT8				ch;

	if (obj == NULL)
		return GWM_RET_NO_OBJECT;

	if (obj->lineOfCursor >= obj->pageEnd
		&& obj->pageEnd >= obj->lineNum - 1)
		return GWM_RET_OK;

	if (obj->selEnd - obj->selStart > 0)
		return GWM_RET_OK;

	// get the offset of current line, offset 은 cursor 의 column position 을 의미한다.
	pLine = getLineByIndex(obj, obj->lineOfCursor);
	if (pLine != NULL)
		offset = obj->moveCursor - pLine->start;
	else
		offset = 0;		// pLine 이 NULL 인 경우에는 cursor 의 위梁?column 은 0 으로 하여, 가장 왼쪽에 커서가 위치하는 것으로 가정함.

	if (obj->lineOfCursor == obj->pageEnd)
	{	/* shift 1 line downward */
		obj->pageStart++;
		obj->pageEnd++;
	}

	pLine = getLineByIndex(obj, obj->lineOfCursor + 1);
	if (pLine != NULL && pLine->data != NULL)
	{
		obj->lineOfCursor++;
		if (offset <= 0)
		{
			ch = pLine->data[0];
			obj->moveCursor = (ch == '\n') ? pLine->start + 1 : pLine->start;
		}
		else if (offset > 0 && offset <= (pLine->end - pLine->start))
		{
			obj->moveCursor = pLine->start + getRoughPosition(pLine->data, offset);
		}
		else
		{
			obj->moveCursor = pLine->end;
		}
	}

	*outPos = obj->moveCursor;

	return GWM_RET_OK;
}




ONDK_Result_t				GWM_SetInvertRange(MULTILINE_OBJECT obj, HUINT32 invertStart, HUINT32 invertEnd)
{
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;

	obj->sInverted = invertStart;
	obj->eInverted = invertEnd;

	return GWM_RET_OK;
}

ONDK_Result_t				GWM_ResetInvertRange(MULTILINE_OBJECT obj)
{
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;

	obj->sInverted = POS_NOT_DEF;
	obj->eInverted = POS_NOT_DEF;

	return GWM_RET_OK;
}

ONDK_Result_t				GWM_SetFontColor(MULTILINE_OBJECT obj, ONDK_Color_t text, ONDK_Color_t face)
{
	if (obj == NULL)
		return GWM_RET_NO_OBJECT;

	/* set 함수가 이상하다. */
	obj->colorCaret= 168;
	obj->colorText= text;
	obj->colorUnderline= face;
	obj->colorSelection = 147;

	return GWM_RET_OK;

}







