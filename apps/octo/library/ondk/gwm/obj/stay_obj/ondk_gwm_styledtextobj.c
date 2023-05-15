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


/* ======================================================================== */
/* Include. 																						*/
/* ======================================================================== */
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#include <gwm_objdef.h>

#include <linkedlist.h>

//#define	STXT_DEBUG
#if defined(CONFIG_DEBUG)
#define	STXT_Trace(msg)			(ONDK_Print("[GWM StyledText:TRACE]"), ONDK_Print msg)
#define	STXT_Warning(msg)		(ONDK_Print("[GWM StyledText:Warning]"), ONDK_Print msg)
#define	STXT_Error(msg)			(ONDK_Print("[GWM StyledText:Error(%s:%d)]",__FILE__,__LINE__), ONDK_Print msg)
#define	STXT_Print(msg)			(ONDK_Print("[GWM StyledText]"), ONDK_Print msg)
#else
#define	STXT_Trace(msg)
#define	STXT_Warning(msg)
#define	STXT_Error(msg)
#define	STXT_Print(msg)
#endif

/* ======================================================================== */
/* Private Macros. 																					*/
/* ======================================================================== */
#define	MAX_STYLEDTEXT_OBJECT				32

#define	STXT_MAX_LINE						300
#define	STXT_MAX_STYLE						(STXT_MAX_LINE / 2)
#define	STXT_MAX_PAGE						(STXT_MAX_LINE / 4)
#define	STXT_MAX_TAG_LEN					32
#define	STXT_MAX_ELEM_DATA					10
#define	STXT_SCROLL_MARGIN					15
#define	STXT_TEXT_BUF_COUNT					2
#define	STXT_TEXT_BUF_SIZE					1024
#define	STXT_DOC_BUF_SIZE					256

#define	STXT_ROOT_TAG						"root"
/* ======================================================================== */
/* Global/Extern Variables.																			*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Types.																					*/
/* ======================================================================== */

typedef enum
{
	eSTXT_TAG_UNKNOWN,

	eSTXT_TAG_FONT,
	eSTXT_TAG_BOLD,
	eSTXT_TAG_UNDERLINE,
	eSTXT_TAG_RETURN,
	eSTXT_TAG_INDENT,
	eSTXT_TAG_UNINDENT,
	eSTXT_TAG_LINESPACE,

	eSTXT_TAG_MAX
} STXT_Tag_t;


typedef struct
{
	STXT_Tag_t	tag;
	HUINT8		szTag[STXT_MAX_TAG_LEN];
	HINT32		argc;
} STXT_Type_t;


typedef struct
{
	HINT32	start;
	HINT32	end;

	HUINT32	fontsize;
	HUINT32	fontratio;
	HUINT32	fontcolor;
	HUINT32	fontalign;
	HBOOL	bold;
	HBOOL	underline;
	HINT32	indent;
	HINT32	unindent;
	HINT32	linespace;
} STXT_Style_t;


struct tagStyledTextObject
{
	GUIOBJ_BASIC_PROPERTY					/**< GUI object 기본 속성들. 반드시 맨 앞에 존재하도록 할 것. */

	HINT32			currline;
	HINT32			lastline;

	HINT32			baseline;

	HUINT8			**textdoc;
	HINT32			linecount;
	HINT32			lineperpage;

	STXT_Style_t	style1st;
	STXT_Style_t	**style;
	HINT32			stylecount;

	HINT32			pagecount;

	HINT32			height;

	HBOOL			bHideText;
	GWM_StyledText_DrawScroll_t	pfnDraw;
	HINT32						scrollW;
};


/* ======================================================================== */
/* Private Constants. 																				*/
/* ======================================================================== */

static STXT_Type_t	s_TypeList[] =
{
	{eSTXT_TAG_FONT,		"font", 4},
	{eSTXT_TAG_BOLD,		"b", 1},
	{eSTXT_TAG_UNDERLINE,	"u", 1},
	{eSTXT_TAG_RETURN,		"br", 0},
	{eSTXT_TAG_INDENT,		"indent", 1},
	{eSTXT_TAG_UNINDENT,	"unindent", 1},
	{eSTXT_TAG_LINESPACE,	"linespace", 1}
};


/* ======================================================================== */
/* Private Variables. 																				*/
/* ======================================================================== */
static struct tagStyledTextObject	s_ObjectArray[MAX_STYLEDTEXT_OBJECT];
static HUINT8						s_IsAllocated[MAX_STYLEDTEXT_OBJECT];

static const STXT_Style_t			s_DefaultStyle =
{
	0, 0,
	20,
	100,
	0xFF000000,
	TEXT_ALIGN_LEFT,
	FALSE,
	FALSE,
	0,
	0,
	0
};

static HUINT8						s_szTextBuf[STXT_TEXT_BUF_COUNT][STXT_TEXT_BUF_SIZE];
/* ======================================================================== */
/* Private Function prototypes. 																		*/
/* ======================================================================== */

#define	____STYLED_TEXT_OBJECT_MEMPOOL____

static void	local_gwmstxt_ClearDocument (STYLEDTEXT_OBJECT obj)
{
	HINT32	i;

	if (obj->textdoc)
	{
		for (i = 0 ; i < obj->linecount ; i++)
		{
			if (obj->textdoc[i])
				ONDK_Free(obj->textdoc[i]);
		}
		ONDK_Free(obj->textdoc);
	}
	if (obj->style)
	{
		for (i = 1 /* 0 은 s_DefaultStyle */ ; i < obj->stylecount ; i++)
		{
			if (obj->style[i])
				ONDK_Free(obj->style[i]);
		}
		ONDK_Free(obj->style);
	}
	obj->lastline	= 0;
	obj->currline	= 0;
	obj->textdoc	= NULL;
	obj->linecount	= 0;
	obj->lineperpage= 0;
	VK_memset(&obj->style1st, 0, sizeof(STXT_Style_t));
	obj->style		= NULL;
	obj->stylecount	= 0;
	obj->pagecount	= 0;
	obj->height		= obj->baseline;
	obj->bHideText	= FALSE;
}


static void	local_gwmstxt_SetDefault (STYLEDTEXT_OBJECT obj)
{
	local_gwmstxt_ClearDocument(obj);

	VK_memset(obj, 0, sizeof(struct tagStyledTextObject));

	INT_ONDK_GWM_OBJ_BasicSetting((ONDK_GWM_GuiObject_t)obj);
}


static STYLEDTEXT_OBJECT	local_gwmstxt_AllocNewTextObj(void)
{
	HINT32	i;

	for (i = 0 ; i < MAX_STYLEDTEXT_OBJECT ; i++)
	{
		if (!s_IsAllocated[i])
			break;
	}
	if (i < MAX_STYLEDTEXT_OBJECT)
	{
		s_IsAllocated[i] = TRUE;
		return &s_ObjectArray[i];
	}
	return NULL;
}

static void			local_gwmstxt_FreeTextObj(STYLEDTEXT_OBJECT obj)
{
	HINT32		i;

	GWM_assert(obj);

#ifdef CONFIG_DEBUG
	if (obj->link != NULL)
	{
		GWM_DBG_Print(DBGMSG_OBJ, ("[ WARNING ] Object(%d):%08X is currently linked.\n", obj->ID, (int)obj));
	}
#endif
	for (i = 0; i < MAX_STYLEDTEXT_OBJECT; i++)
	{
		if (obj == &(s_ObjectArray[i]))
			break;
	}
	if (i == MAX_STYLEDTEXT_OBJECT)
	{
		return;
	}

	s_IsAllocated[i] = FALSE;

	local_gwmstxt_SetDefault(obj);
}

#define	____STYLED_TEXT_LOCAL_FUNCTIONS____

static HUINT8 *	local_gwmstxt_GetTextBuf (void)
{
	static HINT32	s_index;

	s_index	= (s_index + 1) % STXT_TEXT_BUF_COUNT;
	if (s_szTextBuf[s_index][0] != '\0')
	{
		VK_memset(s_szTextBuf[s_index], 0, STXT_TEXT_BUF_SIZE * sizeof(HUINT8));
	}
	return s_szTextBuf[s_index];
}



static HINT32	local_gwmstxt_StrCaseCmp (const HUINT8 *szS1, const HUINT8 *szS2)
{
	HINT32	ch1;
	HINT32	ch2;

	do
	{
		ch1	= toupper(*szS1++);
		ch2	= toupper(*szS2++);
	} while (ch1 == ch2 && ch1 != 0);

	return ch1 - ch2;
}


static STXT_Type_t *local_gwmstxt_GetType (const HUINT8 *szType)
{
	HINT32	i, max;

	max = sizeof(s_TypeList) / sizeof(s_TypeList[0]);

	for (i = 0 ; i < max ; i++)
	{
		if (local_gwmstxt_StrCaseCmp(s_TypeList[i].szTag, szType) == 0)
		{
			return &s_TypeList[i];
		}
	}
	return NULL;
}



static HUINT8 *	local_gwmstxt_StrNDup (const HUINT8 *szLoc, const HUINT8 *szStr, HINT32 n)
{
	HUINT8	*szDup;
	HUINT32	size;

	size	= n;
	if (szLoc)
	{
		size += MWC_UTIL_DvbStrlen(szLoc);
	}
	szDup	= (HUINT8 *)ONDK_Malloc(size + 1);
	if (szDup)
	{
		if (szLoc)
			MWC_UTIL_DvbStrcpy(szDup, szLoc);
		MWC_UTIL_DvbStrncpy(szDup + (size - n), szStr, (HUINT32)n);
		szDup[size] = '\0';
	}
	return szDup;
}


static HINT32	local_gwmstxt_NextLine (STXT_Style_t *style)
{
	ONDKFontHandle font;
	HUINT32		height;
	HUINT32		descender;
	HERROR		err;

	if (style->linespace)
	{
		return style->linespace;
	}

	font	= OSD_FONT_GetFont(
					style->bold ? eFont_SystemBold : eFont_SystemNormal,
					style->fontsize,
					style->fontratio
				);
	if (font == NULL)
	{
		return 0;
	}
	err  = OSD_FONT_GetFontHeight(font, &height);
	err |= OSD_FONT_GetDescender(font, &descender);
	if (err != ERR_OK)
	{
		height = descender = 0;
	}
	ONDK_FONT_Release(font);
	return (HINT32) (height + descender);
}


static STXT_Style_t *local_gwmstxt_GetStyle (STYLEDTEXT_OBJECT obj, HINT32 line)
{
	HINT32	i;

	if (obj->style == NULL)
	{
		obj->style = (STXT_Style_t **) ONDK_Calloc (sizeof(STXT_Style_t *) * STXT_MAX_STYLE);
		if (obj->style == NULL)
		{
			return NULL;
		}
		VK_memcpy(&obj->style1st, &s_DefaultStyle, sizeof(STXT_Style_t));
		obj->style[0] = &obj->style1st;
		obj->stylecount = 1;
	}

	for (i = 0 ; i < obj->stylecount ; i++)
	{
		if (obj->style[i]->start <= line && line <= obj->style[i]->end)
		{
			return obj->style[i];
		}
	}
	return NULL;
}


static HERROR	local_gwmstxt_SetStyle (STYLEDTEXT_OBJECT obj, STXT_Type_t *type, HINT32 *argv)
{
	STXT_Style_t	*style;
	STXT_Style_t	*last;

	last	= local_gwmstxt_GetStyle(obj, obj->linecount);
	if (last == NULL)
	{
		return ERR_FAIL;
	}

	if (last->start == last->end)
	{
		style	= last;
	}
	else
	{
		style	= (STXT_Style_t *) ONDK_Malloc (sizeof(STXT_Style_t));
		if (style == NULL)
		{
			return ERR_FAIL;
		}
		VK_memcpy(style, last, sizeof(STXT_Style_t));
		style->start	= obj->linecount;
		style->end		= obj->linecount;
		obj->style[obj->stylecount++] = style;
		last->end--;
	}

	switch (type->tag)
	{
	case eSTXT_TAG_FONT:
		style->fontsize		= (HUINT32) *argv++;
		style->fontratio	= (HUINT32) *argv++;
		style->fontcolor	= (HUINT32) *argv++;
		style->fontalign	= (HUINT32) *argv++;
		break;

	case eSTXT_TAG_BOLD:
		style->bold			= (HBOOL) *argv;
		break;

	case eSTXT_TAG_UNDERLINE:
		style->underline	= (HBOOL) *argv;
		break;

	case eSTXT_TAG_RETURN:
		obj->height	+= local_gwmstxt_NextLine(style);
		obj->textdoc[obj->linecount++] = NULL;
		style->end++;
		break;

	case eSTXT_TAG_INDENT:
		style->indent		= *argv;
		break;

	case eSTXT_TAG_UNINDENT:
		style->unindent		= *argv;
		break;

	case eSTXT_TAG_LINESPACE:
		style->linespace	= *argv;
		break;

	default:
		return ERR_FAIL;
	}
	return ERR_OK;
}



static HERROR	local_gwmstxt_SetText (STYLEDTEXT_OBJECT obj, HUINT8 *text)
{
	HERROR			err;
	HUINT8			preCode[5];
	HUINT8			*buf;
	HUINT8			*locCode;
	HINT32			locLen, strLen, remain, count;
	HINT32 			textW, fontH, linespace, descender;
	HINT32			scrollMargin;
	HUINT8			*line;
	STXT_Style_t	*style;
	ONDKFontHandle		*font;

	buf		= local_gwmstxt_GetTextBuf();
	style	= local_gwmstxt_GetStyle(obj, obj->linecount);
	if (buf == NULL || buf == text || style == NULL)
	{
		return ERR_FAIL;
	}

	font	= OSD_FONT_GetFont(
					style->bold ? eFont_SystemBold : eFont_SystemNormal,
					style->fontsize,
					style->fontratio
				);
	if (font == NULL)
	{
		return ERR_FAIL;
	}
	err  = OSD_FONT_GetFontHeight(font, (HUINT32 *)&fontH);
	err |= OSD_FONT_GetDescender(font, (HUINT32 *)&descender);
	if (err != ERR_OK)
	{
		ONDK_FONT_Release(font);
		return err;
	}
	linespace 	= (style->linespace == 0)
				? fontH + descender
				: style->linespace;

	VK_memset(preCode, 0, sizeof(HUINT8) * 5);
	locCode	= preCode + 1;
	locLen	= (HINT32)MWC_UTIL_GetLocaleCode(text, locCode);
	strLen	= (HINT32)MWC_UTIL_DvbStrlen(text) - locLen;
	remain	= strLen;
	text	+= locLen;

	scrollMargin = obj->scrollW ? obj->scrollW + STXT_SCROLL_MARGIN : 0;

	textW	= obj->rect.w - style->indent - scrollMargin;
	do
	{
		if (strLen > 100)
		{
			VK_memcpy(buf, text, 100);
			buf[100] = '\0';
			err = OSD_FONT_CountChByWidth_300468(font, textW, locCode, locLen, MWC_UTIL_DvbStrlen(buf), buf, (HUINT32 *)&count);
		}
		else
		{
			err = OSD_FONT_CountChByWidth_300468(font, textW, locCode, locLen, strLen, text, (HUINT32 *)&count);
		}
		if (err != ERR_OK)
		{
			break;
		}

		line = local_gwmstxt_StrNDup(preCode[0] == '\n' ? preCode : locCode, text, count);
		if (line == NULL)
		{
			err = ERR_FAIL;
			break;
		}
		if (obj->linecount > 0)
		{
			obj->height += linespace;
		}

		obj->textdoc[obj->linecount++] = line;
		style->end++;

		remain	= remain - count;
		if (remain <= 0)
			break;	// 아래 while에서 체크하지만.. strlen을 막기 위해 여기서도 검사하자.

		textW	= obj->rect.w - style->unindent - scrollMargin;
		text	= text + count;
		strLen	= MWC_UTIL_DvbStrlen(text);	// TODO: strLen - count로 대체 가능??
		preCode[0] = '\n';
	} while (0 < remain);

	ONDK_FONT_Release(font);
	return err;
}


static HERROR	local_gwmstxt_CalcPageInfo (STYLEDTEXT_OBJECT obj)
{
	HERROR				err;
	HINT32				i, temp;
	STXT_Style_t		*style;
	ONDKFontHandle			*font;
	HINT32				fontH, ascender, descender;
	HINT32				remainH;

	style = local_gwmstxt_GetStyle(obj, obj->linecount - 1);
	if (style)
	{
		font = OSD_FONT_GetFont(
					style->bold ? eFont_SystemBold : eFont_SystemNormal,
					style->fontsize,
					style->fontratio
				);
		if (font)
		{
			err = OSD_FONT_GetDescender(font, (HUINT32 *)&descender);
			if (err == ERR_OK)
				obj->height += descender;
			ONDK_FONT_Release(font);
		}
	}

	temp = obj->height / obj->rect.h;
	if (obj->height % obj->rect.h != 0)
	{
		temp++;
	}
	obj->pagecount = temp;

	temp = obj->linecount / obj->pagecount;
	if (obj->linecount % obj->pagecount != 0)
	{
		temp++;
	}
	obj->lineperpage = temp;

	if (obj->pagecount == 1)
	{
		obj->lastline = 0;
		return ERR_OK;
	}

	remainH = obj->rect.h;
	for (i = obj->linecount - 1 ; i >= 0 ; i--)
	{
		style	= local_gwmstxt_GetStyle(obj, i);
		if (style == NULL)
			continue;

		font = OSD_FONT_GetFont(
					style->bold ? eFont_SystemBold : eFont_SystemNormal,
					style->fontsize,
					style->fontratio
				);
		if (font == NULL)
			continue;

		err  = OSD_FONT_GetFontHeight(font, (HUINT32 *)&fontH);
		err |= OSD_FONT_GetAscender(font, (HUINT32 *)&ascender);
		err |= OSD_FONT_GetDescender(font, (HUINT32 *)&descender);
		ONDK_FONT_Release(font);
		if (err != ERR_OK)
			continue;

		if (i == obj->linecount - 1)
		{
			remainH -= descender;
		}

		if (remainH - obj->baseline < 0)
		{
			obj->lastline = i + 1;
			break;
		}
		remainH -= style->linespace;
	}
	return ERR_OK;
}


#define	____STYLED_TEXT_DEFAULT_PROCS____

static ONDK_Result_t	local_gwmstxt_DefaultDrawProc(
									ONDKSurfaceHandle screen,
									ONDK_Region_t *area,
									ONDK_Rect_t *rect,
									ONDK_GWM_GuiObject_t object,
									void *arg,
									int param1
								)
{
	HERROR				err = ERR_FAIL;
	HINT32				i;
	HINT32				x, y, w, h;
	HINT32				dx, dy;
	HUINT8				*text;
	STYLEDTEXT_OBJECT	stxt;
	STXT_Style_t		*style;
	ONDKFontHandle			*font;
	HINT32				ascender1st;
	HINT32				scrollX, scrollY, scrollH, view;
	HINT32				fontH, textW, ascender, descender;

	HAPPY(arg);	HAPPY(param1);

	stxt	= (STYLEDTEXT_OBJECT)object;
	if (stxt == NULL)
	{
		return ERR_FAIL;
	}
	x	= area->x1 + rect->x;
	y	= area->y1 + rect->y;
	w	= rect->w;
	h	= rect->h;

	scrollX = x + w - stxt->scrollW;
	scrollY = y + stxt->baseline / 2;
	scrollH = h - (scrollY - y);

	ascender1st = 0;
	dy	= y + stxt->baseline;

	if (stxt->bHideText)
	{
		return GWM_RET_OK;
	}

	// draw text
	for (i = stxt->currline, view = 0 ; i < stxt->linecount ; i++)
	{
		style	= local_gwmstxt_GetStyle(stxt, i);
		if (style == NULL)
			continue;

		font = OSD_FONT_GetFont(
					style->bold ? eFont_SystemBold : eFont_SystemNormal,
					style->fontsize,
					style->fontratio
				);
		if (font == NULL)
			continue;

		err  = OSD_FONT_GetFontHeight(font, (HUINT32 *)&fontH);
		err |= OSD_FONT_GetAscender(font, (HUINT32 *)&ascender);
		err |= OSD_FONT_GetDescender(font, (HUINT32 *)&descender);
		if (err != ERR_OK)
		{
			ONDK_FONT_Release(font);
			continue;
		}

		if (ascender1st == 0)
			ascender1st = ascender;

		if (dy + descender > y + h)
		{
			STXT_Error(("dy :%d, descender:%d, y:%d, h:%d\n", dy, descender, y, h));
			ONDK_FONT_Release(font);
			break;
		}

		text = stxt->textdoc[i];
		if (text)
		{
			if (text[0] == '\n')
			{
				dx	= x + style->unindent;
				text++;
			}
			else
			{
				dx	= x + style->indent;
			}
			textW = ONDK_GWM_GFX_GetStringWidth(font, text);
			ONDK_FONT_DrawString(screen, font, dx, dy - ascender, stxt->textdoc[i]
										, style->fontcolor, ONDKSTF_LEFT);
			if (style->underline)
			{
				OSD_GFX_DrawLine(screen, dx, dy, dx + textW - 1, dy, style->fontcolor);
				if (style->bold)
					OSD_GFX_DrawLine(screen, dx, dy + 1, dx + textW - 1, dy + 1, style->fontcolor);
			}
		}
		dy += ((style->linespace) ? style->linespace : fontH + descender);
		view++;

		ONDK_FONT_Release(font);
	}

	// draw scroll bar
	if (stxt->pagecount > 1 && stxt->pfnDraw && stxt->lineperpage)
	{
		stxt->pfnDraw(screen, scrollX, scrollY, scrollH, stxt->currline, stxt->linecount, view);
	}
	return GWM_RET_OK;
}


static ONDK_Result_t	local_gwmstxt_DefaultTextProc(GWM_Obj_MethodType_t proc_msg, ONDK_GWM_GuiObject_t object, int arg)
{
	STYLEDTEXT_OBJECT	stxtobj;
	ONDK_Region_t		area;
	ONDK_Rect_t		rect;

	GWM_assert(object->type == GUIOBJ_STYLEDTEXT);
	stxtobj = (STYLEDTEXT_OBJECT)object;

	if ((proc_msg == MT_DRAW || proc_msg == MT_KEYDOWN) && stxtobj->pagecount == 0)
	{
		if (local_gwmstxt_CalcPageInfo(stxtobj) != ERR_OK)
		{
			return GWM_MESSAGE_PASS;
		}
	}

	switch (proc_msg)
	{
		case MT_DESTROY:
		{
			INT_ONDK_GWM_OBJ_GetObjectRect(object, &rect);
			ONDK_GWM_APP_InvalidateRelativeRect(rect.x, rect.y, rect.w, rect.h);
			local_gwmstxt_FreeTextObj(stxtobj);
			return GWM_MESSAGE_BREAK;
		}
		case MT_DRAW:
		{
			ONDK_GWM_APP_GetAppArea(g_pstCurrentApp, &area);
			INT_ONDK_GWM_OBJ_GetObjectRect(object, &rect);

			if (object->objDrawMethod != NULL)	/* draw with customized drawing method */
			{
				object->objDrawMethod(ONDK_GWM_GetSurface(), &area, &rect, object, stxtobj, arg);
			}
			else	/* draw with default method */
			{
				local_gwmstxt_DefaultDrawProc(ONDK_GWM_GetSurface(), &area, &rect, object, stxtobj, arg);
			}
			return GWM_MESSAGE_BREAK;
		}
		case MT_KEYDOWN:
		{
			if (stxtobj->pagecount == 1)
			{
				return GWM_MESSAGE_PASS;
			}
			switch (arg)
			{
			case KEY_ARROWUP:
				if (stxtobj->currline > 0)
				{
					stxtobj->currline--;
				}
				break;

			case KEY_ARROWDOWN:
				if (stxtobj->currline < stxtobj->lastline)
				{
					stxtobj->currline++;
				}
				break;

			case KEY_ARROWLEFT:
				if (stxtobj->currline > stxtobj->lineperpage)
				{
					stxtobj->currline -= stxtobj->lineperpage;
				}
				else
				{
					stxtobj->currline = 0;
				}
				break;

			case KEY_ARROWRIGHT:
				if (stxtobj->currline + stxtobj->lineperpage <= stxtobj->lastline)
				{
					stxtobj->currline += stxtobj->lineperpage;
				}
				else
				{
					stxtobj->currline = stxtobj->lastline;
				}
				break;

			default:
				return GWM_MESSAGE_PASS;
			}
			ONDK_GWM_APP_InvalidateRelativeRect(stxtobj->rect.x, stxtobj->rect.y, stxtobj->rect.w, stxtobj->rect.h);
			return GWM_MESSAGE_BREAK;
		}
		case MT_RELOCATE:
		{
			ONDK_Rect_t	*rect;

			rect = (ONDK_Rect_t*)arg;
			ONDK_GWM_APP_InvalidateRelativeRect(stxtobj->rect.x, stxtobj->rect.y, stxtobj->rect.w, stxtobj->rect.h);
			VK_memcpy(&(stxtobj->rect), rect, sizeof(ONDK_Rect_t));
			ONDK_GWM_APP_InvalidateRelativeRect(stxtobj->rect.x, stxtobj->rect.y, stxtobj->rect.w, stxtobj->rect.h);
			return GWM_MESSAGE_BREAK;
		}
		default:
			break;
	}
	return GWM_MESSAGE_PASS;	//MESSAGE_NOTUSED;
}


#define	____STYLED_TEXT_INIT____

void			gwm_styledtext_InitClass(void)
{
	HINT32			i;

	for (i = 0; i < MAX_STYLEDTEXT_OBJECT; i++)
	{
		local_gwmstxt_SetDefault(&s_ObjectArray[i]);

		s_ObjectArray[i].link	= NULL;
		VK_memset(&(s_ObjectArray[i].rect), 0, sizeof(ONDK_Rect_t));
		s_ObjectArray[i].ID	= -1;
		s_ObjectArray[i].type	= GUIOBJ_STYLEDTEXT;
		s_ObjectArray[i].focus	= GUIOBJ_NORMAL;
		s_ObjectArray[i].Proc	= local_gwmstxt_DefaultTextProc;
		s_ObjectArray[i].objDrawMethod	= NULL;

		s_IsAllocated[i]	= FALSE;
	}
}


/* ======================================================================== */
/* Exported Functions. 																				*/
/* ======================================================================== */
#define	____STYLED_TEXT_GLOBAL_FUNCTIONS____

/**
 * Styled Text
 *
 * XML Tag로 Style을 지정하여, Text를 표현
 * XML 기반이기 때문에, 모든 tag는 open/close pair를 맞춰 주어야 한다.
 *
 * @param	id		object id
 * @param	rect	object rect
 * @param	baseline	text baseline
 * @return	GWM_RET_OK 등
 */
ONDK_Result_t	GWM_StyledText_Create(HINT32 id, ONDK_Rect_t *rect, HINT32 baseline)
{
	ONDK_GWM_GuiObject_t		objlist;
	STYLEDTEXT_OBJECT	newobj;
	ONDK_Result_t		result;

	STXT_Trace(("GWM_StyledText_Create(id:0x%X)\n", id));

	GWM_assert(g_pstCurrentApp);
	objlist = INT_ONDK_GWM_APP_GetObjectList(g_pstCurrentApp);

	newobj = (STYLEDTEXT_OBJECT)INT_ONDK_GWM_OBJ_FindObject(objlist, id);
	if (newobj != NULL)
	{
		return GWM_RET_LINK_CONFLICT;
	}

	newobj = local_gwmstxt_AllocNewTextObj();
	if (newobj == NULL)
	{
		return GWM_RET_OUT_OF_MEMORY;
	}

	//3 Init "GUIOBJ_BASIC_PROPERTY"
	newobj->link = NULL;
	VK_memcpy(&(newobj->rect), rect, sizeof(ONDK_Rect_t));
	newobj->ID	= id;
	newobj->type	= GUIOBJ_STYLEDTEXT;
	newobj->focus	= GUIOBJ_NORMAL;
	newobj->Proc	= local_gwmstxt_DefaultTextProc;
	newobj->objDrawMethod = NULL;

	newobj->baseline	= baseline;
	newobj->pagecount	= 0;
	newobj->height		= baseline;

	INT_ONDK_GWM_OBJ_CallObjectProc(MT_CREATE, (ONDK_GWM_GuiObject_t)newobj, 0);

	result = INT_ONDK_GWM_APP_AddObject(g_pstCurrentApp, (ONDK_GWM_GuiObject_t)newobj);
	ONDK_GWM_APP_InvalidateRelativeRect(newobj->rect.x, newobj->rect.y, newobj->rect.w, newobj->rect.h);

	return result;
}


/**
 * Style
 * "font"
 *		1: size (HINT32)
 *		2: ratio (HINT32)
 *		3: color (OSD_Color_t)
 *		4: align (TEXT_ALIGN_LEFT 등)
 *
 * "b" = bold
 *		1: HBOOL
 * "u" = underline
 *		1: HBOOL
 * "indent"	= 들여쓰기
 *		1: HINT32
 * "unindent" = 내어쓰기
 *		1: HINT32
 * "br"	: new line
 *
 * @param	id		object id
 * @param	szStyle		"font", "b", "u", "indent",
 * @param	...		vargs
 * @return	GWM_RET_OK 등
 */
ONDK_Result_t	GWM_StyledText_SetStyle (HINT32 id, const HUINT8 *szType, ...)
{
	STYLEDTEXT_OBJECT	obj;
	HINT32			i, argv[10];
	STXT_Type_t		*type;
	va_list			va;

	STXT_Trace(("GWM_StyledText_SetStyle(id:0x%X)\n", id));
	if (szType == NULL)
	{
		return GWM_RET_SOURCE_NULL;
	}

	obj = (STYLEDTEXT_OBJECT)ONDK_GWM_APP_GetObject((int)id);
	if (obj == NULL)
	{
		return GWM_RET_NO_OBJECT;
	}

	if (obj->type != GUIOBJ_STYLEDTEXT)
	{
		return GWM_RET_NO_OBJECT;
	}

	type	= local_gwmstxt_GetType(szType);
	if (type == NULL)
	{
		return GWM_RET_TARGET_NULL;
	}

	va_start(va, szType);
	for (i = 0 ; i < type->argc ; i++)
	{
		argv[i] = va_arg(va, HINT32);
	}
	va_end(va);
	return local_gwmstxt_SetStyle(obj, type, argv);
}


/**
 * Set Text
 *
 * @param	id		object id
 * @param	text
 * @return	GWM_RET_OK 등
 */
ONDK_Result_t	GWM_StyledText_SetText (HINT32 id, const HUINT8 *text)
{
	STYLEDTEXT_OBJECT	obj;
	HERROR				err;
	HUINT8				*pText = (HUINT8*)text;

	STXT_Trace(("GWM_StyledText_SetText(id:0x%X)\n", id));
	if (pText == NULL)
	{
		return GWM_RET_SOURCE_NULL;
	}

	obj  = (STYLEDTEXT_OBJECT)ONDK_GWM_APP_GetObject((int)id);
	if (obj == NULL)
	{
		return GWM_RET_NO_OBJECT;
	}

	if (obj->type != GUIOBJ_STYLEDTEXT)
	{
		return GWM_RET_NO_OBJECT;
	}

	if (obj->textdoc == NULL)
	{
		obj->textdoc	= (HUINT8 **) ONDK_Calloc(sizeof(HUINT8 *) * STXT_MAX_LINE);
		if (obj->textdoc == NULL)
		{
			return ERR_FAIL;
		}
		obj->linecount	= 0;
		obj->lineperpage = 0;
	}

	err = local_gwmstxt_SetText(obj, pText);

	ONDK_GWM_APP_InvalidateRelativeRect(obj->rect.x, obj->rect.y, obj->rect.w, obj->rect.h);

	return err;
}


/**
 * Show가 아닌 경우, text를 그리지 않는다.
 *
 */
ONDK_Result_t	GWM_StyledText_ShowText (HINT32 id, HBOOL bShow)
{
	STYLEDTEXT_OBJECT	obj;

	STXT_Trace(("GWM_StyledText_ShowText(id:0x%X, Show ? %s)\n", id, bShow ? "Yes" : "No"));

	obj  = (STYLEDTEXT_OBJECT)ONDK_GWM_APP_GetObject((int)id);
	if (obj == NULL)
	{
		return GWM_RET_NO_OBJECT;
	}

	if (obj->type != GUIOBJ_STYLEDTEXT)
	{
		return GWM_RET_NO_OBJECT;
	}

	obj->bHideText	= !bShow;

	ONDK_GWM_APP_InvalidateRelativeRect(obj->rect.x, obj->rect.y, obj->rect.w, obj->rect.h);
	return GWM_RET_OK;
}


/**
 * Document를 Clear
 *
 */
ONDK_Result_t	GWM_StyledText_Clear (HINT32 id)
{
	STYLEDTEXT_OBJECT	obj;

	STXT_Trace(("GWM_StyledText_Clear(id:0x%X)\n", id));

	obj  = (STYLEDTEXT_OBJECT)ONDK_GWM_APP_GetObject((int)id);
	if (obj == NULL)
	{
		return GWM_RET_NO_OBJECT;
	}

	if (obj->type != GUIOBJ_STYLEDTEXT)
	{
		return GWM_RET_NO_OBJECT;
	}

	local_gwmstxt_ClearDocument(obj);

	ONDK_GWM_APP_InvalidateRelativeRect(obj->rect.x, obj->rect.y, obj->rect.w, obj->rect.h);
	return GWM_RET_OK;
}


ONDK_Result_t GWM_StyledText_SetScrollDraw (HINT32 id, GWM_StyledText_DrawScroll_t pfnDraw, HINT32 width)
{
	STYLEDTEXT_OBJECT	obj;

	STXT_Trace(("GWM_StyledText_SetScrollDraw(id:0x%X)\n", id));

	if (pfnDraw == NULL || width == 0)
	{
		return GWM_RET_SOURCE_NULL;
	}

	obj  = (STYLEDTEXT_OBJECT)ONDK_GWM_APP_GetObject((int)id);
	if (obj == NULL)
	{
		return GWM_RET_NO_OBJECT;
	}

	if (obj->type != GUIOBJ_STYLEDTEXT)
	{
		return GWM_RET_NO_OBJECT;
	}

	obj->pfnDraw	= pfnDraw;
	obj->scrollW	= width;

	ONDK_GWM_APP_InvalidateRelativeRect(obj->rect.x, obj->rect.y, obj->rect.w, obj->rect.h);

	return GWM_RET_OK;
}


/* End of File. ---------------------------------------------------------- */



