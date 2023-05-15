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


/**********************************************************************************************************/
#define ____INCLUDE_______________________
/**********************************************************************************************************/
#include 	<ondk.h>

#include 	"ondk_dfb.h"
#include 	<ondk_arabic_support.h>


/**********************************************************************************************************/
#define ____INTERNAL_STRUCTURES_AND_VARIABLES_______________________
/**********************************************************************************************************/
#if	defined(CONFIG_PROD_LANG_ARABIC) || defined(CONFIG_PROD_LANG_PERSIAN) || defined(CONFIG_APUI_NATIVE_APP)
#define DEF_ONDK_DEFAULT_FONT_NORMAL_PATH 	"/usr/fonts/DejaVuSansCondensed.ttf"
#define DEF_ONDK_DEFAULT_FONT_BOLD_PATH 	"/usr/fonts/DejaVuSansCondensed-Bold.ttf"
#else
#define DEF_ONDK_DEFAULT_FONT_NORMAL_PATH 	"/usr/fonts/HelveticaNeueLTW1G-LtCn.otf"
#define DEF_ONDK_DEFAULT_FONT_BOLD_PATH 	"/usr/fonts/HelveticaNeueLTW1G-BdCn.otf"
#endif

#define DEF_ONDK_DEFAULT_FONT_FIXED_PATH 	"/usr/fonts/DejaVuSansCondensed.ttf"

#define DEF_ONDK_VOLUME_FONT_PATH 	"/usr/browser/webapps/font/HelveticaNeueLTW1G-MdCn.otf"

#define	DEF_ONDK_FONT_OUTLINE		0x0002

#define DEF_ONDK_FONT_MAX_LENGTH 	256

/*
typedef struct tagFontMultiLine_Item_t
{
	HCHAR 	*pszString;
	HINT32 	lineBytes;
	HINT32	lineWidth;
	HINT32	textWidth;
	HINT32	spaceWidth;
	HINT32	spaceCount;
	HBOOL	endLine;
} FontMultiLine_Item_t;
*/
#define DEF_FONT_MAX_MULTILINE_COUNT 48

typedef	struct tagFontContext
{
	void 		*fontdata;
	HINT32		w, h;
	HINT32		ascender;
	HINT32		descender;
	HINT32		attribute;
   	HINT32		size;		/**< point */
	ONDK_Size_t	fullSize;
} FontContext_t;

typedef struct tagFontPool_Item
{
	DFBFontDescription	fontdesc;
	FontContext_t		stFont;
	HBOOL				bSystemFont;
	const HCHAR			*path;
} FontPool_Item_t;

typedef struct tagFontPool_Package
{
	HCHAR			path[DEF_ONDK_FONT_MAX_LENGTH];
	HxHASH_t		*pItemHash; /* FontPool_Item_t */
} FontPool_Package_t;

typedef struct tagFontPool_Mgr
{
	HxHASH_t			*pPoolHash; /* FontPool_Package */
	FontPool_Package_t	*arrSystemFont[eFont_IdMax];
} FontPool_Mgr_t;

STATIC FontPool_Mgr_t	*s_pFontPoolMgr = NULL;
STATIC HULONG			s_ulFontPoolSem = 0;



#define DEF_FONT_CHECK_INVALID_HANDLE(ah)	if((HANDLE_NULL == (HUINT32)(ah)) || (NULL == (void *)(ah))) { ONDK_Error("[%s:%d] fontHandle(0x%x)\r\n", __FUNCTION__, __LINE__, (HUINT32)(ah)); break;}
#define DEF_FONT_CHECK_AREA(ax1,ay1)	 	if(((ax1) > GWM_SCREEN_WIDTH) || ((ay1) > GWM_SCREEN_HEIGHT)) { ONDK_Error("[%s:%d] Check Area x(%d) y(%d)\r\n", __FUNCTION__, __LINE__, (ax1), (ay1)); break;}
#define DEF_FONT_CHECK_NULLPOINTER(astr)	 if(NULL == (astr)) { ONDK_Error("[%s:%d] Check Str Pointer str(%s)\r\n", __FUNCTION__, __LINE__, (astr)); break;}
#define DEF_FONT_CHECK_ALPHA(acolor)	 	if((COL_A((acolor)) == 0x00)) { ONDK_Print("[%s:%d] Check Color (0x%08X)\r\n", __FUNCTION__, __LINE__, (acolor)); break;}


/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/
STATIC FontPool_Mgr_t *local_font_GetFontMgr(void)
{
	return s_pFontPoolMgr;
}

STATIC HUINT32		local_fontpool_Item_GetKey(const void *key)
{
	DFBFontDescription* 	p = (DFBFontDescription*)key;
	HUINT32					h = -1;

	if (p)
	{
		h = p->height;
	}

	return h;
}

STATIC HBOOL		local_fontpool_Item_IsEqual(const void *a, const void *b)
{
	DFBFontDescription* 	k1 = (DFBFontDescription*)a;
	DFBFontDescription* 	k2 = (DFBFontDescription*)b;

	if((k1->flags == k2->flags)
		&& (k1->attributes == k2->attributes))
	{
		if((k1->height == k2->height)
			&& (k1->width == k2->width)
			&& (k1->rotation == k2->rotation))
		{
			return TRUE;
		}
	}

	return FALSE;
}

STATIC void	local_fontpool_Item_DestroyData(void* data)
{
	FontPool_Item_t* 	d = (FontPool_Item_t*)data;

	ONDK_Free(d);
}


STATIC FontPool_Package_t	*local_fontpool_Package_New(HCHAR *pFontName)
{
	FontPool_Package_t	*pPack = NULL;

	ONDK_Assert(pFontName);

	pPack = ONDK_Calloc(sizeof(FontPool_Package_t));
	ONDK_Assert(pPack);

	HLIB_STD_StrNCpySafe(pPack->path, pFontName, DEF_ONDK_FONT_MAX_LENGTH);

	pPack->pItemHash = HLIB_HASH_NewFull(local_fontpool_Item_GetKey, local_fontpool_Item_IsEqual, NULL, local_fontpool_Item_DestroyData);
	ONDK_Assert(pPack->pItemHash);

	return pPack;
}

STATIC FontPool_Item_t	*local_fontpool_Package_InsertItem(FontPool_Package_t *pPack, DFBFontDescription	*pFontDesc, IDirectFBFont *font, HBOOL bSystemFont)
{
	FontPool_Item_t *pItem = NULL;
	HINT32			size = 0;

	pItem = HLIB_HASH_Lookup(pPack->pItemHash, pFontDesc);
	if(NULL != pItem)
	{
		return pItem;
	}

	pItem = (FontPool_Item_t *)ONDK_Calloc(sizeof(FontPool_Item_t));
	ONDK_Assert(pItem);

	HxSTD_MemCopy(&pItem->fontdesc, pFontDesc, sizeof(DFBFontDescription));

	size = pFontDesc->height;
	pItem->stFont.fontdata = (void *)font;
	pItem->stFont.h = size;
	pItem->stFont.w = size;
	pItem->stFont.ascender = 0;
	pItem->stFont.descender= 0;
	pItem->stFont.size = size;
	pItem->stFont.fullSize = ONDK_Size(size, (size + (size/5)));
	pItem->bSystemFont = bSystemFont;
	pItem->path = pPack->path;

	HLIB_HASH_Insert(pPack->pItemHash, &pItem->fontdesc, pItem);

	return pItem;
}

STATIC void local_fontpool_Package_RemoveItem(FontPool_Package_t *pPack, DFBFontDescription	*pFontDesc)
{
	HLIB_HASH_Remove(pPack->pItemHash, pFontDesc);
}


STATIC void local_font_DrawStringBytes(ONDKSurfaceHandle _surface,ONDKFontHandle fontHandle, HCHAR *pstr ,HUINT32 x ,HUINT32 y ,ONDK_Color_t color,DFBSurfaceTextFlags textflag, HINT32 nBytes)
{
	DFBResult err;
	FontPool_Item_t 	*font = (FontPool_Item_t *)fontHandle;
	IDirectFBFont		*dfbFont = (IDirectFBFont*) font->stFont.fontdata;
	IDirectFBSurface	 *dfbSurface = (IDirectFBSurface*)_surface;
/*
	color	 = ((color & 0xF000) * 0x11000)
		| ((color & 0x0F00) * 0x01100)
		| ((color & 0x00F0) * 0x00110)
		| ((color & 0x000F) * 0x00011);
*/

	DFBCHECK (dfbSurface->SetFont (dfbSurface, dfbFont));
	DFBCHECK (dfbSurface->SetColor (dfbSurface, COL_R(color), COL_G(color), COL_B(color), COL_A(color)));
	if(pstr==NULL)	return ;

	DFBCHECK (dfbSurface->DrawString (dfbSurface, pstr, nBytes, x, y, textflag));
}


STATIC HINT32 local_font_DrawOneCharacter(ONDKSurfaceHandle _surface, ONDKFontHandle fontHandle, HCHAR *utf8pstr, HINT32 x, HINT32 y, ONDK_Color_t color, HINT32 *out_width)
{
	DFBResult			err;
	HINT32				used_bytes = 0;
	FontPool_Item_t		*font = (FontPool_Item_t *)fontHandle;
	IDirectFBFont	 	*dfbFont = (IDirectFBFont*) font->stFont.fontdata;
	IDirectFBSurface	 *dfbSurface = (IDirectFBSurface*)_surface;
	HCHAR				szChar[8] = {0, };
	DFBSurfaceTextFlags flag = DSTF_TOPLEFT;

	used_bytes = HLIB_CP_CharAt(eHxCP_UNICODE_UTF8, utf8pstr, 0, szChar);

	if(dfbSurface != NULL)
	{
		DFBCHECK (dfbSurface->SetFont (dfbSurface, dfbFont));
		DFBCHECK (dfbSurface->SetColor (dfbSurface, COL_R(color), COL_G(color), COL_B(color), COL_A(color)));
		DFBCHECK (dfbSurface->DrawString (dfbSurface, utf8pstr, used_bytes, x, y, flag ));

		if (out_width != NULL)
		{
			DFBCHECK (dfbFont->GetStringWidth(dfbFont, utf8pstr, used_bytes, out_width));
		}
	}

	return  used_bytes;

}

STATIC void local_font_DrawStringEqualSpace(ONDKSurfaceHandle _surface,ONDKFontHandle fontHandle, HUINT32 x ,HUINT32 y, HUINT32 w,ONDK_Color_t color, FontMultiLine_Item_t *pItem)
{

	HINT32		nBytes = pItem->lineBytes;
	HINT32		addPoint = 0, maxAddPoint = 0;
	HINT32		out_width = 0;
	HINT32		use_bytes = 0;
	HCHAR		*pstr = pItem->pszString;

	maxAddPoint = w - pItem->lineWidth;
	if(pItem->spaceCount)
	{
		addPoint = (maxAddPoint / pItem->spaceCount) + 1;
	}

	if(TRUE == pItem->endLine)
	{
		local_font_DrawStringBytes(_surface, fontHandle, pItem->pszString, x, y, color, DSTF_TOPLEFT, pItem->lineBytes);
		return;
	}

	while(nBytes > 0)
	{
		use_bytes = local_font_DrawOneCharacter(_surface, fontHandle, pstr, x, y, color, &out_width);
		if(*pstr == ' ')
		{
			if((maxAddPoint-addPoint) > 0)
			{
				x += addPoint;
				maxAddPoint -= addPoint;
			}
			else
			{
				x += maxAddPoint;
				maxAddPoint = 0;
			}
		}
		x += out_width;
		nBytes -= use_bytes;
		pstr += use_bytes;
	}

}


STATIC HINT32 local_font_GetOneCharacterWidth(ONDKFontHandle fontHandle, HUINT8 *utf8pstr, HUINT32 *ret_pUsedBytes)
{
	HINT32				used_bytes, out_width = 0;
	DFBResult			err;
	FontPool_Item_t 	*font = (FontPool_Item_t *)fontHandle;
	IDirectFBFont		*dfbFont = NULL;
	HCHAR				szChar[8] = {0, };

	if(NULL == font || NULL == font->stFont.fontdata)
	{
		return 0;
	}

	dfbFont = (IDirectFBFont*) font->stFont.fontdata;
	used_bytes = HLIB_CP_CharAt(eHxCP_UNICODE_UTF8, utf8pstr, 0, szChar);

	DFBCHECK (dfbFont->GetStringWidth(dfbFont, utf8pstr, used_bytes, &out_width));

	if(NULL != ret_pUsedBytes)
	{
		*ret_pUsedBytes = used_bytes;
	}

	return	out_width;
}


STATIC HINT32 local_font_GetStringWidth(ONDKFontHandle fontHandle,HCHAR *pstr)
{
#if 0
	FontPool_Item_t 	*font = (FontPool_Item_t *)fontHandle;
	IDirectFBFont		*dfbFont;
	HINT32				strWidth =0;
	DFBResult			err;

	if(NULL == font || NULL == font->stFont.fontdata)
	{
		return 0;
	}

	dfbFont = (IDirectFBFont *)font->stFont.fontdata;
	DFBCHECK (dfbFont->GetStringWidth (dfbFont, pstr, HxSTD_StrLen(pstr), &strWidth));

	return strWidth;
#else
	HINT32 nUsedBytes = 0;
	HINT32 nWidth = 0;
	HINT32 nLen = 0;

	nLen = HxSTD_StrLen(pstr);
	while(nLen > 0)
	{
		nWidth += local_font_GetOneCharacterWidth(fontHandle, pstr, &nUsedBytes);
		nLen -= nUsedBytes;
		pstr += nUsedBytes;
	}

	return nWidth;
#endif
}

STATIC HBOOL local_font_DrawStringPosition(ONDKSurfaceHandle _surface,ONDKFontHandle fontHandle,
																HCHAR *pstr ,
																HUINT32 drawStartPos, HUINT32 drawEndPos,
																HUINT32 x ,HUINT32 y, HUINT32 w,
																ONDK_Color_t color,
																ONDKSurfaceTextFlags textflag)
{
	HINT32 	used_width, font_width, used_bytes;
	HINT32	cur_width = 0;
	HINT32	target_width = (w);
	HINT32 	target_bytes = 0;
	HCHAR	*p = pstr;
	HCHAR	*startPstr = NULL;
	DFBSurfaceTextFlags flag = DSTF_TOPLEFT;

	FontPool_Item_t		*font = (FontPool_Item_t *)fontHandle;

	font_width = 0;
	used_bytes = 0;

	if(0 < drawStartPos)
	{
		while(*p != '\0')
		{
			font_width += local_font_GetOneCharacterWidth(font, p, &used_bytes);
			if(font_width > drawStartPos)
			{
				break;
			}
			p += used_bytes;
		}
		if(*p == '\0')
		{
			return FALSE;
		}
	}

	used_width = 0;
	startPstr = p;
	target_width = MAX((w), (drawEndPos-drawStartPos));

	while(*p != '\0')
	{
		cur_width = local_font_GetOneCharacterWidth(font, p, &used_bytes);
		if(used_width+cur_width > target_width)
		{

			break;
		}
		used_width += cur_width;
		p += used_bytes;
		target_bytes += used_bytes;
	}

	if(0 == target_bytes)
	{
		return FALSE;
	}

	if(textflag & ONDKSTF_CENTER)		x = x + ((target_width-used_width) / 2);
	else if(textflag & ONDKSTF_RIGHT)	x = x + (target_width-used_width);

	local_font_DrawStringBytes(_surface, fontHandle, startPstr, x, y, color, flag, target_bytes);

	return TRUE;
}

STATIC void local_font_DrawStringAlignAbbr(ONDKSurfaceHandle _surface,ONDKFontHandle fontHandle,
																HCHAR *pstr ,
																HUINT32 x ,HUINT32 y, HUINT32 w,
																ONDK_Color_t color,
																ONDKSurfaceTextFlags textflag,
																HBOOL bIsAbbr,
																HINT32 *ret_pWidth)
{
	HINT32 	font_width, used_bytes;
	HBOOL	isPrintAbbr = FALSE;
	HINT32	dot_width = 0;
	HINT32	abbrIdx = 0;
	HINT32	target_width = (w);
	HCHAR	*p = pstr;
	HCHAR 	*pTempStr = pstr;
	HCHAR	*pAbbrStr = NULL;
	HCHAR 	dot[4] = {"..."};
	DFBSurfaceTextFlags flag = DSTF_TOPLEFT;

	FontPool_Item_t		*font = (FontPool_Item_t *)fontHandle;

#if defined(CONFIG_APUI_NATIVE_APP) && defined(CONFIG_PROD_LANG_ARABIC)
	HxSTR_t		*pstTotal_str = NULL;
	HBOOL		bAra  = FALSE;
#endif

	if(-1 == target_width)
	{
		target_width = (GWM_SCREEN_WIDTH-x);
	}

	if(TRUE == bIsAbbr)
	{
		dot_width = local_font_GetOneCharacterWidth(font, dot, NULL);
		dot_width *= 3;
	}

	font_width = 0;
	used_bytes = 0;

#if defined(CONFIG_APUI_NATIVE_APP) && defined(CONFIG_PROD_LANG_ARABIC)
	pstTotal_str = HLIB_STR_New((HUINT8*)pstr);
	if ((pstTotal_str != NULL) && (ONDK_ARABIC_SUPPORT_CheckUnicodeArabicCharacterInString(HLIB_STR_toUCS2(pstTotal_str)) == TRUE))
	{
		pTempStr = ONDK_ARABIC_SUPPORT_ConvertToArabicStr(pstr);
		if (pTempStr != NULL)
		{
			if (textflag & ONDKSTF_LEFT)
			{
				textflag -= ONDKSTF_LEFT;
				textflag |= ONDKSTF_RIGHT;
			}
			bAra = TRUE;
			p = pTempStr;
		}
		else
		{
			pTempStr = pstr;
			p = pstr;
		}
	}
	if (pstTotal_str != NULL)
		HLIB_STR_Delete(pstTotal_str);
#endif

	if(target_width <= dot_width)
	{
		while(1)
		{
			used_bytes += local_font_DrawOneCharacter(_surface, font, dot, x, y, color, &font_width);
			pTempStr += used_bytes;
			if(target_width < font_width)
			{
				break;
			}
			target_width -= font_width;
			if(ret_pWidth)
			{
				*ret_pWidth += font_width;
			}
		}
		goto END_FUNC;
	}

	abbrIdx = -1;
	while(*p != '\0')
	{
		font_width += local_font_GetOneCharacterWidth(font, p, &used_bytes);
		if(abbrIdx == -1 && (font_width > (target_width-dot_width)))
		{
			abbrIdx = (p-pTempStr);
		}

		if(font_width > target_width)
		{
			isPrintAbbr = TRUE;
			break;
		}
		p += used_bytes;
	}

	if(FALSE == isPrintAbbr)
	{
		if(textflag & ONDKSTF_CENTER)		x = x + ((target_width-font_width) / 2);
		else if(textflag & ONDKSTF_RIGHT)	x = x + (target_width-font_width);

		local_font_DrawStringBytes(_surface, fontHandle, pTempStr, x, y, color, flag, -1);
		if(ret_pWidth)
		{
			*ret_pWidth = font_width;
		}
		goto END_FUNC;
	}

	if(0 > abbrIdx)
	{
		goto END_FUNC;
	}

	pAbbrStr = ONDK_Calloc((abbrIdx + 4));
	ONDK_Assert(pAbbrStr);

	HxSTD_MemCopy(pAbbrStr, pTempStr, abbrIdx);
	if(TRUE == bIsAbbr)
	{
		HxSTD_MemCopy(pAbbrStr+abbrIdx, dot, 3);
		font_width+=dot_width;
	}

	local_font_DrawStringBytes(_surface, fontHandle, pAbbrStr, x, y, color, flag, -1);

	if(ret_pWidth)
	{
		*ret_pWidth = font_width;
	}

	ONDK_Free(pAbbrStr);

END_FUNC:
#if defined(CONFIG_APUI_NATIVE_APP) && defined(CONFIG_PROD_LANG_ARABIC)
	if (pTempStr && bAra)
		ONDK_Free(pTempStr);
#endif
	return;
}


STATIC HINT32 local_font_ParseMultLine(ONDKFontHandle fontHandle,
										HUINT32 x, HUINT32 y, HUINT32 w, HUINT32 h, HUINT8 *pstr,
										HUINT32 vspace,
										ONDKSurfaceTextFlags textflag,
										HUINT32 *ret_out_height,
										FontMultiLine_Item_t *ret_arrItem)
{
	HINT32 	font_height = 0, text_width = 0, used_bytes = 0;
	HINT32	line_bytes = 0, space_width = 0, char_width = 0;
	HINT32	space_count = 0, line_width = 0;
	HUINT32	cur_height = 0;
	HBOOL	isSkipLastInfo = FALSE;
	HINT32	itemIdx = 0;
	HCHAR	*p = pstr, *lastSpacePtr = NULL;

	FontMultiLine_Item_t	*pItem = NULL;
	FontPool_Item_t			*font = (FontPool_Item_t *)fontHandle;

	font_height = ONDK_FONT_GetHeight(fontHandle);
	if(h < (font_height+vspace))
	{
		return 0;
	}

	cur_height = (font_height+vspace);
	for(itemIdx = 0; itemIdx < DEF_FONT_MAX_MULTILINE_COUNT; itemIdx++)
	{
		pItem = &ret_arrItem[itemIdx];
		line_bytes	= 0; used_bytes	= 0; space_count	= 0;
		line_width	= 0; space_width= 0; text_width		= 0;
		isSkipLastInfo = FALSE;

		pItem->pszString	= p;

		pItem->endLine		= TRUE;
		while(*p != '\0')
		{
			if(*p == '\n')
			{
				p++;
				pItem->endLine		= TRUE;
				break;
			}
			char_width = local_font_GetOneCharacterWidth(font, p, &used_bytes);
			if((line_width+char_width) > w)
			{
				if((textflag & ONDKSTF_WORDWRAP) && (NULL != lastSpacePtr))
				{
					p = lastSpacePtr;
					isSkipLastInfo = TRUE;
				}
				else if(*(p-1) == ' ')
				{
					isSkipLastInfo = TRUE;
				}
				pItem->endLine	= FALSE;
				break;
			}
			if(*p == ' ')
			{
				lastSpacePtr = p+1;
				pItem->lineBytes	= line_bytes;
				pItem->lineWidth	= line_width;
				pItem->textWidth	= text_width;
				pItem->spaceWidth	= space_width;
				pItem->spaceCount	= space_count;
				pItem->lineBytes	= line_bytes;

				space_width += char_width;
				space_count++;
			}
			else
			{
				text_width += char_width;
			}

			p += used_bytes;
			line_width += char_width;
			line_bytes += used_bytes;
		}

		if(TRUE == isSkipLastInfo)
		{
			//
		}
		else
		{
			pItem->lineBytes	= line_bytes;
			pItem->lineWidth	= line_width;
			pItem->textWidth	= text_width;
			pItem->spaceWidth	= space_width;
			pItem->spaceCount	= space_count;
			pItem->lineBytes	= line_bytes;
		}

		if(*p == '\0')
		{
			break;
		}

		if((cur_height+font_height+vspace) > h)
		{
			break;
		}
		cur_height += (font_height + vspace);
	}

	*ret_out_height = cur_height;
	return (itemIdx+1);
}


STATIC HINT32 local_font_ParseMultiLineWithMaxLine(ONDKFontHandle fontHandle,
										HUINT32 x, HUINT32 y, HUINT32 w, HUINT32 h, HUINT8 *pstr,
										HUINT32 vspace, HINT32 lMaxLine,
										ONDKSurfaceTextFlags textflag,
										HUINT32 *ret_out_height,
										FontMultiLine_Item_t *ret_arrItem)
{
	HINT32 	font_height = 0, text_width = 0, used_bytes = 0;
	HINT32	line_bytes = 0, space_width = 0, char_width = 0;
	HINT32	space_count = 0, line_width = 0;
	HUINT32	cur_height = 0;
	HBOOL	isSkipLastInfo = FALSE;
	HINT32	itemIdx = 0;
	HCHAR	*p = pstr, *lastSpacePtr = NULL;

	FontMultiLine_Item_t	*pItem = NULL;
	FontPool_Item_t			*font = (FontPool_Item_t *)fontHandle;


	font_height = ONDK_FONT_GetHeight(fontHandle);
	if(h < (font_height+vspace))
	{
		return 0;
	}

	cur_height = (font_height+vspace);
	for(itemIdx = 0; itemIdx < lMaxLine; itemIdx++)
	{
		pItem = &ret_arrItem[itemIdx];
		line_bytes	= 0; used_bytes	= 0; space_count	= 0;
		line_width	= 0; space_width= 0; text_width		= 0;
		isSkipLastInfo = FALSE;

		pItem->pszString	= p;

		pItem->endLine		= TRUE;
		while(*p != '\0')
		{
			if(*p == '\n')
			{
				p++;
				pItem->endLine		= TRUE;
				break;
			}
			char_width = local_font_GetOneCharacterWidth(font, p, &used_bytes);
			if((line_width+char_width) > w)
			{
				if((textflag & ONDKSTF_WORDWRAP) && (NULL != lastSpacePtr))
				{
					p = lastSpacePtr;
					isSkipLastInfo = TRUE;
				}
				else if(*(p-1) == ' ')
				{
					isSkipLastInfo = TRUE;
				}
				pItem->endLine	= FALSE;
				break;
			}
			if(*p == ' ')
			{
				lastSpacePtr = p+1;
				pItem->lineBytes	= line_bytes;
				pItem->lineWidth	= line_width;
				pItem->textWidth	= text_width;
				pItem->spaceWidth	= space_width;
				pItem->spaceCount	= space_count;
				pItem->lineBytes	= line_bytes;

				space_width += char_width;
				space_count++;
			}
			else
			{
				text_width += char_width;
			}

			p += used_bytes;
			line_width += char_width;
			line_bytes += used_bytes;
		}

		if(TRUE == isSkipLastInfo)
		{
			//
		}
		else
		{
			pItem->lineBytes	= line_bytes;
			pItem->lineWidth	= line_width;
			pItem->textWidth	= text_width;
			pItem->spaceWidth	= space_width;
			pItem->spaceCount	= space_count;
			pItem->lineBytes	= line_bytes;
		}

		if(*p == '\0')
		{
			break;
		}

		if((cur_height+font_height+vspace) > h)
		{
			break;
		}
		cur_height += (font_height + vspace);
	}

	*ret_out_height = cur_height;
	return (itemIdx+1);
}

STATIC HINT32 local_font_DrawMultiLineAlign(ONDKSurfaceHandle _surface,
												ONDKFontHandle fontHandle,
												HUINT32 x, HUINT32 y, HUINT32 w, HUINT32 h, HUINT8 *pstr,
												HUINT32 vspace,
												ONDK_Color_t color,
												ONDKSurfaceTextFlags textflag,
												ONDK_Point_t	*ret_pLastPoint)
{
	DFBSurfaceTextFlags 	flag = DSTF_TOPLEFT;
	FontMultiLine_Item_t	arrItem[DEF_FONT_MAX_MULTILINE_COUNT];

	HUINT32			out_height = 0;
	HINT32			font_height = 0, itemIdx = 0, lineCnt = 0;
	HINT32			tx =0;
	HCHAR			*pTempStr = pstr;

#if defined(CONFIG_APUI_NATIVE_APP) && defined(CONFIG_PROD_LANG_ARABIC)
	HxSTR_t 	*pstTotal_str = NULL;
	HBOOL		bAra  = FALSE;
#endif

	HxSTD_MemSet(arrItem, 0, (sizeof(FontMultiLine_Item_t) * DEF_FONT_MAX_MULTILINE_COUNT));

#if defined(CONFIG_APUI_NATIVE_APP) && defined(CONFIG_PROD_LANG_ARABIC)
	pstTotal_str = HLIB_STR_New((HUINT8*)pstr);
	if ((pstTotal_str != NULL) && (ONDK_ARABIC_SUPPORT_CheckUnicodeArabicCharacterInString(HLIB_STR_toUCS2(pstTotal_str)) == TRUE))
	{
		pTempStr = ONDK_ARABIC_SUPPORT_ConvertToArabicStr(pstr);
		if (pTempStr != NULL)
		{
			if (textflag & ONDKSTF_LEFT)
			{
				textflag -= ONDKSTF_LEFT;
				textflag |= ONDKSTF_RIGHT;
			}
			bAra = TRUE;
		}
		else
			pTempStr = pstr;
	}
	if (pstTotal_str != NULL)
		HLIB_STR_Delete(pstTotal_str);
#endif

	lineCnt = local_font_ParseMultLine(fontHandle, x, y, w, h, pTempStr, vspace, textflag, &out_height, arrItem);

	font_height = ONDK_FONT_GetHeight(fontHandle);
	font_height += vspace;

	for(itemIdx = 0; itemIdx < lineCnt; itemIdx++)
	{
		if(0 == arrItem[itemIdx].lineBytes)
		{
			break;
		}

		if(textflag & ONDKSTF_EQUALSPACE)
		{
			tx = x;
			local_font_DrawStringEqualSpace(_surface, fontHandle, tx, y, w, color, &arrItem[itemIdx]);
		}
		else if(textflag & ONDKSTF_CENTER)
		{
			tx = x + ((w - arrItem[itemIdx].lineWidth)/2);
			local_font_DrawStringBytes(_surface, fontHandle, arrItem[itemIdx].pszString, tx, y, color, flag, arrItem[itemIdx].lineBytes);
		}
		else if(textflag & ONDKSTF_RIGHT)
		{
			tx = x + ((w - arrItem[itemIdx].lineWidth));
			local_font_DrawStringBytes(_surface, fontHandle, arrItem[itemIdx].pszString, tx, y, color, flag, arrItem[itemIdx].lineBytes);
		}
		else // left = default
		{
			tx = x;
			local_font_DrawStringBytes(_surface, fontHandle, arrItem[itemIdx].pszString, tx, y, color, flag, arrItem[itemIdx].lineBytes);
		}


		y += font_height;
	}

	if(ret_pLastPoint && 0 < lineCnt)
	{
		ret_pLastPoint->x = tx + local_font_GetStringWidth (fontHandle, arrItem[lineCnt-1].pszString);
		ret_pLastPoint->y = out_height;
	}

#if defined(CONFIG_APUI_NATIVE_APP) && defined(CONFIG_PROD_LANG_ARABIC)
	if (pTempStr && bAra)
		ONDK_Free(pTempStr);
#endif

	return lineCnt;
}

STATIC HINT32 local_font_GetLineCount(ONDKSurfaceHandle _surface, ONDKFontHandle fontHandle, HUINT32 x, HUINT32 y, HUINT32 w, HUINT8 *pstr, HUINT32 vspace, ONDKSurfaceTextFlags textflag, HUINT32 *ret_out_height)
{
	FontMultiLine_Item_t	arrItem[DEF_FONT_MAX_MULTILINE_COUNT];

	HxSTD_MemSet(arrItem, 0, (sizeof(FontMultiLine_Item_t) * DEF_FONT_MAX_MULTILINE_COUNT));
	return local_font_ParseMultLine(fontHandle, x, y, w, 0xFFFFFFFF, pstr, vspace, textflag, ret_out_height, arrItem);
}




/**********************************************************************************************************/
#define ____PUBLIC_FUNCTIONS_______________________
/**********************************************************************************************************/




/**********************************************************************************************************/
#define __________FONT_CONTROL______________________
/**********************************************************************************************************/

ONDKFontHandle ONDK_FONT_Create(const HINT8 *path, HINT32 w, HINT32 h)
{
	DFBResult		 	err;
	DFBFontDescription 	fontdesc;
	IDirectFBFont		*font = NULL;
	IDirectFB			*pDirectFB = NULL;
	FontPool_Mgr_t		*pMgr = local_font_GetFontMgr();
	FontPool_Package_t	*pPack = NULL;
	FontPool_Item_t		*pItem = NULL;

	if(NULL == path)
	{
		return (ONDKFontHandle)NULL;
	}

	HxSTD_memset(&fontdesc, 0, sizeof(DFBFontDescription));
	fontdesc.height = h;
	fontdesc.width = w;
	fontdesc.rotation = DFB_DEGREES(270);
	fontdesc.flags = DFDESC_HEIGHT | DFDESC_WIDTH;// | DFDESC_ROTATION;
	//fontdesc.outline_width = 2;
	//if (attrib & DEF_ONDK_FONT_OUTLINE)
	//	fontdesc.flags |= DFDESC_OUTLINE_WIDTH;
	//fontdesc.attributes = DFFA_OUTLINED;

	HxSEMT_Get(s_ulFontPoolSem);
	pPack = HLIB_HASH_Lookup(pMgr->pPoolHash, path);
	if(NULL == pPack)
	{
		pPack  = local_fontpool_Package_New((HCHAR *)path);
		ONDK_Assert(pPack);

		HLIB_HASH_Insert(pMgr->pPoolHash, pPack->path, pPack);
	}
	pItem = HLIB_HASH_Lookup(pPack->pItemHash, &fontdesc);

	HxSEMT_Release(s_ulFontPoolSem);


	if(NULL == pItem)
	{
		pDirectFB = ONDK_DFB_GetIDirectFB();
		ONDK_Assert(pDirectFB);

		DFBCHECK(pDirectFB->CreateFont  ( pDirectFB, path, &fontdesc, &font ));
		ONDK_Assert(font);

		HxSEMT_Get(s_ulFontPoolSem);
		pItem = local_fontpool_Package_InsertItem(pPack, &fontdesc, font, FALSE);
		HxSEMT_Release(s_ulFontPoolSem);
	}
	ONDK_Assert(pItem);

	return (ONDKFontHandle)pItem;

}

ONDKFontHandle ONDK_FONT_CreateRotationFont(ONDK_FontId_t id, HINT32 w, HINT32 angle)
{
	DFBResult		 	err;
	DFBFontDescription 	fontdesc;
	IDirectFBFont		*font = NULL;
	HCHAR				*path = NULL;
	IDirectFB			*pDirectFB = NULL;
	FontPool_Mgr_t		*pMgr = local_font_GetFontMgr();
	FontPool_Package_t	*pPack = NULL;
	FontPool_Item_t		*pItem = NULL;

	switch(id)
	{
	case eFont_SystemFixed: /* font 미정 */
		path = DEF_ONDK_DEFAULT_FONT_FIXED_PATH;
		break;
	case eFont_SystemBold: /* font 미정 */
		path = DEF_ONDK_DEFAULT_FONT_BOLD_PATH;
		break;

	case eFont_SystemNormal:
	default:
		path = DEF_ONDK_DEFAULT_FONT_NORMAL_PATH;
		break;
	}

	HxSTD_memset(&fontdesc, 0, sizeof(DFBFontDescription));
	fontdesc.width = w;
	fontdesc.rotation = DFB_DEGREES(angle);
	fontdesc.flags = DFDESC_WIDTH | DFDESC_ROTATION;
	//fontdesc.outline_width = 2;
	//if (attrib & DEF_ONDK_FONT_OUTLINE)
	//	fontdesc.flags |= DFDESC_OUTLINE_WIDTH;
	//fontdesc.attributes = DFFA_OUTLINED;

	HxSEMT_Get(s_ulFontPoolSem);
	pPack = HLIB_HASH_Lookup(pMgr->pPoolHash, path);
	if(NULL == pPack)
	{
		pPack  = local_fontpool_Package_New((HCHAR *)path);
		ONDK_Assert(pPack);

		HLIB_HASH_Insert(pMgr->pPoolHash, pPack->path, pPack);
	}
	pItem = HLIB_HASH_Lookup(pPack->pItemHash, &fontdesc);

	HxSEMT_Release(s_ulFontPoolSem);


	if(NULL == pItem)
	{
		pDirectFB = ONDK_DFB_GetIDirectFB();
		ONDK_Assert(pDirectFB);

		DFBCHECK(pDirectFB->CreateFont  ( pDirectFB, path, &fontdesc, &font ));
		ONDK_Assert(font);

		HxSEMT_Get(s_ulFontPoolSem);
		pItem = local_fontpool_Package_InsertItem(pPack, &fontdesc, font, FALSE);
		HxSEMT_Release(s_ulFontPoolSem);
	}
	ONDK_Assert(pItem);

	return (ONDKFontHandle)pItem;

}


ONDKFontHandle ONDK_FONT_CreateSystemFont(ONDK_FontId_t id,  HUINT32 size)
{
	DFBResult		 	err;
	DFBFontDescription 	fontdesc;
	IDirectFBFont		*font = NULL;
	IDirectFB			*pDirectFB = NULL;
	HCHAR				*path = NULL;
	FontPool_Mgr_t		*pMgr = local_font_GetFontMgr();
	FontPool_Package_t	*pPack = NULL;
	FontPool_Item_t		*pItem = NULL;

	switch(id)
	{
	case eFont_SystemFixed: /* font 미정 */
		path = DEF_ONDK_DEFAULT_FONT_FIXED_PATH;
		break;
	case eFont_SystemBold: /* font 미정 */
		path = DEF_ONDK_DEFAULT_FONT_BOLD_PATH;
		break;

	case eFont_SystemNormal:
	default:
		path = DEF_ONDK_DEFAULT_FONT_NORMAL_PATH;
		break;
	}

	HxSTD_memset(&fontdesc, 0, sizeof(DFBFontDescription));
	fontdesc.height = size;
	fontdesc.width = size;
	fontdesc.flags = DFDESC_HEIGHT | DFDESC_WIDTH;// | DFDESC_ROTATION;
	//fontdesc.rotation = DFB_DEGREES(270);
	//fontdesc.outline_width = 2;
	//if (attrib & DEF_ONDK_FONT_OUTLINE)
	//	fontdesc.flags |= DFDESC_OUTLINE_WIDTH;
	//fontdesc.attributes = DFFA_OUTLINED;

	HxSEMT_Get(s_ulFontPoolSem);
	if(NULL == pMgr->arrSystemFont[id])
	{
		pPack = HLIB_HASH_Lookup(pMgr->pPoolHash, path);
		if(NULL == pPack)
		{
			pPack  = local_fontpool_Package_New(path);
			ONDK_Assert(pPack);

			HLIB_HASH_Insert(pMgr->pPoolHash, pPack->path, pPack);
		}

		pMgr->arrSystemFont[id] = pPack;
	}
	else
	{
		pPack = pMgr->arrSystemFont[id];
	}

	pItem = HLIB_HASH_Lookup(pPack->pItemHash, &fontdesc);
	HxSEMT_Release(s_ulFontPoolSem);

	if(NULL == pItem)
	{
		pDirectFB = ONDK_DFB_GetIDirectFB();
		ONDK_Assert(pDirectFB);

		DFBCHECK(pDirectFB->CreateFont  ( pDirectFB, path, &fontdesc, &font ));
		ONDK_Assert(font);

		HxSEMT_Get(s_ulFontPoolSem);
		pItem = local_fontpool_Package_InsertItem(pPack, &fontdesc, font, TRUE);
		HxSEMT_Release(s_ulFontPoolSem);

		ONDK_Print("[%s:%d] Create Font = %s size(%d)\r\n", __FUNCTION__, __LINE__, path, size);

	}
	ONDK_Assert(pItem);

	return (ONDKFontHandle)pItem;
}


void ONDK_FONT_Release(ONDKFontHandle fontHandle)
{
	DFBResult			err;
	FontPool_Item_t		*font = (FontPool_Item_t *)fontHandle;
	IDirectFBFont		*dfbFont = NULL;
	FontPool_Mgr_t		*pMgr = local_font_GetFontMgr();
	FontPool_Package_t	*pPack = NULL;

	ONDK_Assert(font);

	if(FALSE == font->bSystemFont)
	{
		HxSEMT_Get(s_ulFontPoolSem);
		pPack = HLIB_HASH_Lookup(pMgr->pPoolHash, font->path);
		ONDK_Assert(pPack);

		dfbFont = font->stFont.fontdata;
		DFBCHECK(dfbFont->Release (dfbFont));

		HLIB_HASH_RemoveAll(pPack->pItemHash);
		HLIB_HASH_Remove(pMgr->pPoolHash, pPack->path);

		ONDK_Free(pPack);
		HxSEMT_Release(s_ulFontPoolSem);
	}
}

HINT32 ONDK_FONT_GetSize(ONDKFontHandle fontHandle)
{
	FontPool_Item_t		*font = (FontPool_Item_t *)fontHandle;
	if(font)
	{
		return font->stFont.size;
	}

	return 0;
}

HINT32 ONDK_FONT_GetWidth(ONDKFontHandle fontHandle)
{
	FontPool_Item_t		*font = (FontPool_Item_t *)fontHandle;
	if(font)
	{
		return font->stFont.fullSize.w;
	}

	return 0;
}

HINT32 ONDK_FONT_GetHeight(ONDKFontHandle fontHandle)
{
	FontPool_Item_t		*font = (FontPool_Item_t *)fontHandle;
	if(font)
	{
		return font->stFont.fullSize.h;
	}

	return 0;
}

/**********************************************************************************************************/
#define __________DRAW_FONT_______________________
/**********************************************************************************************************/


ONDK_Result_t  ONDK_FONT_Init(HINT32 argc, HCHAR **argv)
{
	HERROR hErr = ERR_OK;

	s_pFontPoolMgr =  ONDK_Calloc(sizeof(FontPool_Mgr_t));
	ONDK_Assert (s_pFontPoolMgr);

	s_pFontPoolMgr->pPoolHash = HLIB_HASH_New(HLIB_HASH_StrHash, HLIB_HASH_StrEqual);
	ONDK_Assert (s_pFontPoolMgr->pPoolHash);

	hErr = HxSEMT_Create((HUINT32 *)&s_ulFontPoolSem, "ONDK_FONTPOOL", VK_SUSPENDTYPE_FIFO);
	if(hErr != ERR_OK)
	{
		return ONDK_RET_INITFAIL;
	}

	return ONDK_RET_OK;
}


HINT32 ONDK_FONT_GetStringWidth(ONDKFontHandle fontHandle,HCHAR *pstr)
{
	do
	{
		DEF_FONT_CHECK_INVALID_HANDLE(fontHandle);
		DEF_FONT_CHECK_NULLPOINTER(pstr);

		return local_font_GetStringWidth(fontHandle, pstr);
	} while(0);

	return 0;
}

HBOOL ONDK_FONT_DrawStringPosition(ONDKSurfaceHandle _surface,ONDKFontHandle fontHandle,
																HCHAR *pstr ,
																HUINT32 drawStartPos, HUINT32 drawEndPos,
																HUINT32 x ,HUINT32 y, HUINT32 w,
																ONDK_Color_t color,
																ONDKSurfaceTextFlags textflag)
{
	do
	{
		DEF_FONT_CHECK_INVALID_HANDLE(fontHandle);
		DEF_FONT_CHECK_AREA(x,y);
		DEF_FONT_CHECK_NULLPOINTER(pstr);
		DEF_FONT_CHECK_ALPHA(color);

		return local_font_DrawStringPosition(_surface, fontHandle, pstr, drawStartPos, drawEndPos, x, y, w, color, textflag);
	} while(0);

	return FALSE;
}


void ONDK_FONT_DrawStringAlignAbbrRetPos(ONDKSurfaceHandle _surface,ONDKFontHandle fontHandle,
																HCHAR *pstr ,
																HUINT32 x ,HUINT32 y, HUINT32 w,
																ONDK_Color_t color,
																ONDKSurfaceTextFlags textflag,
																HINT32 *ret_pWidth)
{
	do
	{
		DEF_FONT_CHECK_INVALID_HANDLE(fontHandle);
		DEF_FONT_CHECK_AREA(x,y);
		DEF_FONT_CHECK_NULLPOINTER(pstr);
		DEF_FONT_CHECK_ALPHA(color);

		local_font_DrawStringAlignAbbr(_surface, fontHandle, pstr, x, y, w, color, textflag, TRUE, ret_pWidth);
	} while(0);
}

void ONDK_FONT_DrawStringAlignRetPos(ONDKSurfaceHandle _surface,ONDKFontHandle fontHandle,
																HCHAR *pstr ,
																HUINT32 x ,HUINT32 y, HUINT32 w,
																ONDK_Color_t color,
																ONDKSurfaceTextFlags textflag,
																HINT32 *ret_pWidth)
{

	do
	{
		DEF_FONT_CHECK_INVALID_HANDLE(fontHandle);
		DEF_FONT_CHECK_AREA(x,y);
		DEF_FONT_CHECK_NULLPOINTER(pstr);
		DEF_FONT_CHECK_ALPHA(color);

		local_font_DrawStringAlignAbbr(_surface, fontHandle, pstr, x, y, w, color, textflag, FALSE, ret_pWidth);
	} while(0);
}

void ONDK_FONT_DrawStringAlignAbbr(ONDKSurfaceHandle _surface,ONDKFontHandle fontHandle,
																HCHAR *pstr ,
																HUINT32 x ,HUINT32 y, HUINT32 w,
																ONDK_Color_t color,
																ONDKSurfaceTextFlags textflag)
{
	do
	{
		DEF_FONT_CHECK_INVALID_HANDLE(fontHandle);
		DEF_FONT_CHECK_AREA(x,y);
		DEF_FONT_CHECK_NULLPOINTER(pstr);
		DEF_FONT_CHECK_ALPHA(color);

		local_font_DrawStringAlignAbbr(_surface, fontHandle, pstr, x, y, w, color, textflag, TRUE, NULL);
	} while(0);
}

void ONDK_FONT_DrawStringAlign(ONDKSurfaceHandle _surface,ONDKFontHandle fontHandle,
																HCHAR *pstr ,
																HUINT32 x ,HUINT32 y, HUINT32 w,
																ONDK_Color_t color,
																ONDKSurfaceTextFlags textflag)
{

	do
	{
		DEF_FONT_CHECK_INVALID_HANDLE(fontHandle);
		DEF_FONT_CHECK_AREA(x,y);
		DEF_FONT_CHECK_NULLPOINTER(pstr);
		DEF_FONT_CHECK_ALPHA(color);

		local_font_DrawStringAlignAbbr(_surface, fontHandle, pstr, x, y, w, color, textflag, FALSE, NULL);
	} while(0);
}

void ONDK_FONT_DrawStringAbbr(ONDKSurfaceHandle _surface,ONDKFontHandle fontHandle,
																HCHAR *pstr ,
																HUINT32 x ,HUINT32 y, HUINT32 w,
																ONDK_Color_t color)
{

	do
	{
		DEF_FONT_CHECK_INVALID_HANDLE(fontHandle);
		DEF_FONT_CHECK_AREA(x,y);
		DEF_FONT_CHECK_NULLPOINTER(pstr);
		DEF_FONT_CHECK_ALPHA(color);

		local_font_DrawStringAlignAbbr(_surface, fontHandle, pstr, x, y, w, color, ONDKSTF_LEFT, TRUE, NULL);
	} while(0);
}

HINT32 ONDK_FONT_GetOneCharacterWidth(ONDKFontHandle fontHandle, HUINT8 *utf8pstr, HUINT32 *ret_pUsedBytes)
{
	do
	{
		DEF_FONT_CHECK_INVALID_HANDLE(fontHandle);
		DEF_FONT_CHECK_NULLPOINTER(utf8pstr);
		DEF_FONT_CHECK_NULLPOINTER(ret_pUsedBytes);

		return local_font_GetOneCharacterWidth(fontHandle, utf8pstr, ret_pUsedBytes);
	} while(0);

	return 0;
}


HINT32	 ONDK_FONT_DrawOneCharacter(ONDKSurfaceHandle _surface, ONDKFontHandle fontHandle,
										HCHAR *pstr, HINT32 x, HINT32 y,
										ONDK_Color_t color,
										HINT32 *out_width)
{
	do
	{
		DEF_FONT_CHECK_INVALID_HANDLE(fontHandle);
		DEF_FONT_CHECK_AREA(x,y);
		DEF_FONT_CHECK_NULLPOINTER(pstr);
		DEF_FONT_CHECK_ALPHA(color);
		DEF_FONT_CHECK_NULLPOINTER(out_width);

		return local_font_DrawOneCharacter(_surface, fontHandle, pstr, x, y, color, out_width);
	} while(0);

	return 0;
}

HINT32	 ONDK_FONT_GetLineCount(ONDKSurfaceHandle _surface, ONDKFontHandle fontHandle,
										HUINT32 x, HUINT32 y, HUINT32 w, HUINT8 *pstr,
										HUINT32 vspace,
										ONDKSurfaceTextFlags textflag,
										HUINT32 *ret_out_height)
{
	do
	{
		DEF_FONT_CHECK_INVALID_HANDLE(fontHandle);
		DEF_FONT_CHECK_AREA(x,y);
		DEF_FONT_CHECK_NULLPOINTER(pstr);
		DEF_FONT_CHECK_NULLPOINTER(ret_out_height);

		return local_font_GetLineCount(_surface, fontHandle, x, y, w, pstr, vspace, textflag, ret_out_height);
	} while(0);

	return 0;
}


HINT32 ONDK_FONT_GetLineCountWithParseMultiLine(ONDKFontHandle fontHandle, HUINT32 x, HUINT32 y, HUINT32 w, HUINT32 h, HUINT8 *pstr,
										HUINT32 vspace, HINT32 lMaxLine, ONDKSurfaceTextFlags textflag, HUINT32 *ret_out_height, FontMultiLine_Item_t *ret_arrItem)
{
	HINT32		lLineCount = 0;
//	FontMultiLine_Item_t	*arrItem = NULL;

//	arrItem = (FontMultiLine_Item_t *)ONDK_Calloc(sizeof(FontMultiLine_Item_t) * lMaxLine);
	if (ret_arrItem == NULL)
	{
		return	0;
	}

	HxSTD_MemSet(ret_arrItem, 0, (sizeof(FontMultiLine_Item_t) * lMaxLine));
	lLineCount = local_font_ParseMultiLineWithMaxLine(fontHandle, x, y, w, 0xFFFFFFFF, pstr, vspace, lMaxLine, textflag, ret_out_height, ret_arrItem);
	if (lLineCount < 1)
	{
		return	0;
	}

	return	lLineCount;
}


HINT32 ONDK_FONT_DrawMultiLineAlign(ONDKSurfaceHandle _surface, ONDKFontHandle fontHandle,
									HUINT32 x, HUINT32 y, HUINT32 w, HUINT32 h, HUINT8 *pstr,
									HUINT32 vspace,
									ONDK_Color_t color,
									ONDKSurfaceTextFlags textflag)
{
	do
	{
		DEF_FONT_CHECK_INVALID_HANDLE(fontHandle);
		DEF_FONT_CHECK_AREA(x,y);
		DEF_FONT_CHECK_NULLPOINTER(pstr);
		DEF_FONT_CHECK_ALPHA(color);

		return local_font_DrawMultiLineAlign(_surface, fontHandle, x, y, w, h, pstr, vspace, color, textflag, NULL);
	} while(0);

	return 0;
}

HINT32 ONDK_FONT_DrawMultiLine(ONDKSurfaceHandle _surface, ONDKFontHandle fontHandle,
								HUINT32 x, HUINT32 y, HUINT32 w, HUINT32 h, HUINT8 *pstr,
								HUINT32 vspace,
								ONDK_Color_t color)
{
	do
	{
		DEF_FONT_CHECK_INVALID_HANDLE(fontHandle);
		DEF_FONT_CHECK_AREA(x,y);
		DEF_FONT_CHECK_NULLPOINTER(pstr);
		DEF_FONT_CHECK_ALPHA(color);

		return local_font_DrawMultiLineAlign(_surface, fontHandle, x, y, w, h, pstr, vspace, color, (ONDKSTF_WORDWRAP | ONDKSTF_EQUALSPACE), NULL);
	} while(0);

	return 0;
}

HINT32 ONDK_FONT_DrawMultiLineAlignRetPos(ONDKSurfaceHandle _surface, ONDKFontHandle fontHandle,
													HUINT32 x, HUINT32 y, HUINT32 w, HUINT32 h, HUINT8 *pstr,
													HUINT32 vspace,
													ONDK_Color_t color,
													ONDKSurfaceTextFlags textflag,
													ONDK_Point_t *ret_pLastPoint)
{
	do
	{
		DEF_FONT_CHECK_INVALID_HANDLE(fontHandle);
		DEF_FONT_CHECK_AREA(x,y);
		DEF_FONT_CHECK_NULLPOINTER(pstr);
		DEF_FONT_CHECK_ALPHA(color);

		return local_font_DrawMultiLineAlign(_surface, fontHandle, x, y, w, h, pstr, vspace, color, textflag, ret_pLastPoint);
	} while(0);

	return 0;
}


/**********************************************************************************************************/
#define __________DEBUG_______________________
/**********************************************************************************************************/


STATIC void	local_fontpool_Package_DBG_Dump(void* key , void* value , void* user_data)
{
	FontPool_Item_t	*pItem = (FontPool_Item_t *)value;
	HBOOL			bIsSystem = (HBOOL)user_data;

	if(bIsSystem != pItem->bSystemFont)
	{
		return;
	}

	HLIB_CMD_Printf("\t - h(%d) w(%d) flags(0x%04x)\r\n", pItem->stFont.h, pItem->stFont.h, pItem->fontdesc.flags);
}

STATIC void	local_fontpool_DBG_Dump(void* key , void* value , void* user_data)
{
	FontPool_Package_t	*pPack = (FontPool_Package_t*)value;

	HLIB_HASH_Foreach (pPack->pItemHash, local_fontpool_Package_DBG_Dump, user_data);
}


void DBG_ONDK_FONT_Dump(void *arg)
{
	HINT32				i = 0;
	FontPool_Mgr_t		*pMgr = local_font_GetFontMgr();
	FontPool_Package_t	*pPack = NULL;

	if(NULL == pMgr)
	{
		return;
	}

	HLIB_CMD_Printf("System Font\r\n");
	for(i = 0; i < eFont_IdMax; i++)
	{
		pPack = pMgr->arrSystemFont[i];
		if(NULL == pPack)
		{
			HLIB_CMD_Printf("__fnt__ %d. Systemfont empty\r\n", i);
			continue;
		}

		HLIB_CMD_Printf("__fnt__ %d. Name(%s)\r\n", i, pPack->path);
		HLIB_HASH_Foreach (pPack->pItemHash, local_fontpool_Package_DBG_Dump, (void *)TRUE);
	}

	HLIB_CMD_Printf("\r\nNon-System Font\r\n");
	HLIB_HASH_Foreach (pMgr->pPoolHash, local_fontpool_DBG_Dump, FALSE);
}

/**********************************************************************************************************/
#define __________NEED_OPTIMIZE_______________________
/**********************************************************************************************************/


/*
 * need optimize code
 */
void ONDK_FONT_DrawStringMediaSubtitle_ToWindow(ONDKWindowHandle _window, char *pstr , HUINT32 nFontSize, HUINT32 nPosition )
{
	int str_width, nScreenWidth, nScreenHeight;

	DFBResult err;
	IDirectFBWindow 	  *dfbWindow = (IDirectFBWindow*)_window;
	IDirectFBSurface		  *dfbSurface;
	IDirectFBFont		  *font = NULL;
	IDirectFB		*pDirectFB = NULL;
	DFBFontDescription font_dsc;

	pDirectFB = ONDK_DFB_GetIDirectFB();
	ONDK_Assert(pDirectFB);

	DFBCHECK(dfbWindow->GetSurface(dfbWindow, &dfbSurface));
	DFBCHECK(dfbSurface->GetSize(dfbSurface, &nScreenWidth, &nScreenHeight));

#if 0
	dsc.flags = DSDESC_CAPS;
	dsc.caps	= DSCAPS_PRIMARY | DSCAPS_FLIPPING;
#endif

	font_dsc.height = nFontSize;
	font_dsc.flags = DFDESC_HEIGHT;

	DFBCHECK (pDirectFB->CreateFont (pDirectFB, DEF_ONDK_DEFAULT_FONT_NORMAL_PATH, &font_dsc, &font));

	DFBCHECK (dfbSurface->SetFont (dfbSurface, font));
	DFBCHECK (font->GetStringWidth (font, pstr, -1, &str_width));
	DFBCHECK (dfbSurface->SetColor (dfbSurface, 0xFF, 0xFF, 0xFE, 0xFF)); //white
	DFBCHECK (dfbSurface->DrawString (dfbSurface, pstr, -1, (nScreenWidth-str_width)/2, nPosition, DSTF_LEFT));
	DFBCHECK (dfbSurface->Flip (dfbSurface, NULL, DSFLIP_WAITFORSYNC));

	HxLOG_Debug("(s:%dw:%d font(%d) position(%d) ) ---> %s \n", nScreenWidth, str_width, nFontSize, nPosition, pstr);

	font->Release (font);
}


/*
 * need optimize code
 */
void ONDK_FONT_DrawStringMediaSubtitle_ToSurface(ONDKSurfaceHandle _surface, char *pstr , HUINT32 nFontSize, HUINT32 nPosition_X,HUINT32 nPosition_Y ,ONDK_Color_t color,HUINT8 *path)
{
	DFBResult err;
	IDirectFBSurface	 *dfbSurface = (IDirectFBSurface*)_surface;
	IDirectFBFont		 *font = NULL;
	DFBFontDescription	font_dsc;
	IDirectFB			*pDirectFB = NULL;

	pDirectFB = ONDK_DFB_GetIDirectFB();
	ONDK_Assert(pDirectFB);


#if defined(CONFIG_APUI_NATIVE_APP)
	/*
	 * 호환성을 위해 둔것임. native app에서 사용하지 마세요.
	 */
	ONDK_Error("[%s:%d] DO NOT Support Function !!\r\n", __FUNCTION__, __LINE__);
	ONDK_Error("[%s:%d] DO NOT Support Function !!\r\n", __FUNCTION__, __LINE__);
#endif

	font_dsc.height = nFontSize;
	font_dsc.flags = DFDESC_HEIGHT;


	if(path == NULL) path = DEF_ONDK_DEFAULT_FONT_NORMAL_PATH;

	DFBCHECK (pDirectFB->CreateFont (pDirectFB, path, &font_dsc, &font));
	DFBCHECK (dfbSurface->SetFont (dfbSurface, font));
	DFBCHECK (dfbSurface->SetColor (dfbSurface, COL_R(color), COL_G(color), COL_B(color), COL_A(color))); //white
	DFBCHECK (dfbSurface->DrawString (dfbSurface, pstr, -1, nPosition_X, nPosition_Y, DSTF_LEFT));
	//	DFBCHECK (dfbSurface->Flip (dfbSurface, NULL, DSFLIP_WAITFORSYNC));

	font->Release (font);
}


HINT32 ONDK_FONT_DrawStringForMultiLineAlign(ONDKSurfaceHandle _surface, ONDKFontHandle fontHandle, HUINT32 x, HUINT32 y, HUINT32 w, HUINT32 h, HUINT8 *pstr, HUINT32 vspace, ONDK_Color_t color, HINT32 align)
{

#if defined(CONFIG_APUI_NATIVE_APP)
	/*
	 * 호환성을 위해 둔것임. native app에서 사용하지 마세요.
	 */
	ONDK_Error("[%s:%d] DO NOT Support Function !!\r\n", __FUNCTION__, __LINE__);
	ONDK_Error("[%s:%d] DO NOT Support Function !!\r\n", __FUNCTION__, __LINE__);
#endif

	ONDK_Error("[%s:%d] DO NOT Support!!! need implementation !!\r\n", __FUNCTION__, __LINE__);
	ONDK_Error("[%s:%d] DO NOT Support!!! need implementation !!\r\n", __FUNCTION__, __LINE__);
	ONDK_Error("[%s:%d] DO NOT Support!!! need implementation !!\r\n", __FUNCTION__, __LINE__);
	ONDK_Error("[%s:%d] DO NOT Support!!! need implementation !!\r\n", __FUNCTION__, __LINE__);
	return 0;
}


HINT32 ONDK_FONT_DrawStringForMultiLine(ONDKSurfaceHandle _surface, ONDKFontHandle fontHandle, HUINT32 x, HUINT32 y, HUINT32 w, HUINT32 h, HUINT8 *pstr, HUINT32 vspace, ONDK_Color_t color)
{
#if defined(CONFIG_APUI_NATIVE_APP)
	/*
	 * 호환성을 위해 둔것임. native app에서 사용하지 마세요.
	 */
	ONDK_Error("[%s:%d] DO NOT Support Function !!\r\n", __FUNCTION__, __LINE__);
	ONDK_Error("[%s:%d] DO NOT Support Function !!\r\n", __FUNCTION__, __LINE__);

	return	0;
#else
	HINT32 used_bytes, font_width, font_h, tx, ty;
	FontPool_Item_t		*font = (FontPool_Item_t *)fontHandle;
	char	 *ptr;
	HINT32	lineCount = 1;


	font_h = font->stFont.h;		 // dfbFont->GetHeight (dfbFont, &font_h);

	ptr = pstr;	 tx = x; ty = y;
	while (*ptr !='\0')
	{
		if (*ptr == '\n')
		{
			ptr += 1;
			ty += (font_h+vspace);  tx = x;
			if ( (ty+font_h) > (y+h) )
				break;
		}

		font_width = local_font_GetOneCharacterWidth(font, ptr, NULL);
		if ( (tx + font_width) > (x+w) )
		{
			ty += (font_h+vspace);
			tx = x;
			lineCount++;
			if ( (ty+font_h) > (y+h) )
				break;
		}
		used_bytes = local_font_DrawOneCharacter(_surface, font, ptr, tx, ty, color, NULL);
		ptr += used_bytes;
		tx+=font_width;
	}

	return lineCount;
#endif
}


/* 문자열의 라인 수를 리턴하고, 문장이 차지하는 스크린 영역의 크기를 픽셀 수로 얻어 온다. */
/* 	입력 :	font - 글꼴 구조체
			pstr - NULL 로 끝나는 UTF8 문자열 포인터.
			max_w - Window 의 가로 너비 MAX 크기. (자동 줄바꿈을 위한 표시영역의 크기를 지정.)
				  - 0이거나 0보다 작으면, 자동 줄바꿈 없음.
	출력 :	out_area_width, - 문장을 그렸을 때, 화면에 그려질 영역의 크기(가로).
			out_area_height -  문장을 그렸을 때, 화면에 그려질 영역의 크기(세로) - 실제로는 줄간격vspace가 있을 수 있으므로,
							이 출력값을 사용하지 말고,  리턴값인 라인 수 * 줄간격 을 계산해서 써야 한다.
   	리턴값 : 문장의 라인 수, - 실제로는 이 값에 줄간격을 곱하여 출력 영역의 크기를 계산할 것.
*/
HINT32	ONDK_FONT_GetWidthMultiLine( ONDKFontHandle font, unsigned char *pstr, HINT32 max_w, HINT32 vspace, HINT32 *out_area_width, HINT32 *out_area_height)
{
#if defined(CONFIG_APUI_NATIVE_APP)
	/*
	 * 호환성을 위해 둔것임. native app에서 사용하지 마세요.
	 */
	ONDK_Error("[%s:%d] DO NOT Support Function !!\r\n", __FUNCTION__, __LINE__);
	ONDK_Error("[%s:%d] DO NOT Support Function !!\r\n", __FUNCTION__, __LINE__);
	return 0;
#else
	HINT32		used_bytes = 0, font_width = 0, font_h = 0, tx = 0, ty = 0, max_x = 0, line_count = 0;
	HCHAR		*ptr = NULL;

	ONDK_Assert(out_area_width);
	ONDK_Assert(out_area_height);
	if (font == NULL)
	{	HxLOG_Print("\n!!\tWARNING : Font struct_pointer is NULL !!\n");
		return 0;
	}
	if (pstr == NULL)
	{	HxLOG_Print("\n!!\tWARNING : pstr pointer is NULL !!.\n");
		return 0;
	}

	font_h = ONDK_FONT_GetHeight(font);
	font_h += vspace;

	ptr = pstr;		max_x = tx = 0;	ty = 0;	line_count = 0;
	while (*ptr !='\0')
	{
		if (*ptr == '\n')
		{	ptr += 1;
			ty += font_h;
			line_count += 1;
			if (max_x < tx)
				max_x = tx;
			tx = 0;
		}
		else
		{
			font_width = local_font_GetOneCharacterWidth(font, ptr, &used_bytes);
			if ( (max_w > 0) && ((tx + font_width) > max_w) )		// max_w 가 0 보다 크면, 자동 줄바꿈 처리
			{	ty += font_h;
				line_count += 1;
				if (max_x < tx)
					max_x = tx;
				tx = 0;
			}

			ptr += used_bytes;
			tx+=font_width;
		}

	}

	if (max_x < tx)
		max_x = tx;

	*out_area_width = max_x;
	*out_area_height = ty + font_h;

	return	(line_count + 1);
#endif
}


void ONDK_FONT_DrawString(ONDKSurfaceHandle _surface,ONDKFontHandle fontHandle, HCHAR *pstr ,HUINT32 x ,HUINT32 y ,ONDK_Color_t color,ONDKSurfaceTextFlags textflag)
{
#if defined(CONFIG_APUI_NATIVE_APP) && !defined(CONFIG_MW_CAS_IRDETO_LOADER_DVT_V5)
	/*
	 * 호환성을 위해 둔것임. native app에서 사용하지 마세요.
	 */
	ONDK_Error("[%s:%d] DO NOT Support Function !!\r\n", __FUNCTION__, __LINE__);
	ONDK_Error("[%s:%d] DO NOT Support Function !!\r\n", __FUNCTION__, __LINE__);
#else
	local_font_DrawStringBytes(_surface, fontHandle, pstr, x, y, color, textflag, -1);
#endif
}


/*
 * need optimize code
 */
/*	8bpp Indexed Mode (LUT8) Font function. */
ONDKFontHandle ONDK_FONT_CreateGreyFont( const HINT8 *path, HINT32 width, HINT32 height)
{
#if defined(CONFIG_APUI_NATIVE_APP)
	/*
	 * 호환성을 위해 둔것임. native app에서 사용하지 마세요.
	 */
	ONDK_Error("[%s:%d] DO NOT Support Function !!\r\n", __FUNCTION__, __LINE__);
	ONDK_Error("[%s:%d] DO NOT Support Function !!\r\n", __FUNCTION__, __LINE__);
	return 0;
#else
	DFBResult			err;
	DFBFontDescription	fontdesc;
	IDirectFBFont		*font = NULL;
	HINT32				ascender, descender;
	IDirectFB			*pDirectFB = NULL;
	FontPool_Item_t 	*pItem = NULL;

	pDirectFB = ONDK_DFB_GetIDirectFB();
	ONDK_Assert(pDirectFB);

	pItem = (FontPool_Item_t *)ONDK_Calloc(sizeof(FontPool_Item_t));
	ONDK_Assert(pItem);

	fontdesc.height = height;
	fontdesc.width = width;
	fontdesc.flags = DFDESC_HEIGHT | DFDESC_WIDTH;
	DFBCHECK(pDirectFB->CreateFont	( pDirectFB, path, &fontdesc, &font ));
	DFBCHECK(font->SetEncoding ( font, DTEID_UTF8 ));

	DFBCHECK(font->GetDescender ( font, &ascender ));
	DFBCHECK(font->GetDescender ( font, &descender ));

	pItem->stFont.fontdata = (void *)font;
	pItem->stFont.w = width;
	pItem->stFont.h = height;
	pItem->stFont.ascender = ascender;
	pItem->stFont.descender = descender;
	pItem->stFont.attribute = 0;
	pItem->stFont.size = height;

	return (ONDKFontHandle)pItem;
#endif
}

/*
 * need optimize code
 */
/*	8bpp Indexed Mode (LUT8) Font function. */
void ONDK_FONT_ReleaseGreyFont(ONDKFontHandle fontHandle)
{
#if defined(CONFIG_APUI_NATIVE_APP)
	/*
	 * 호환성을 위해 둔것임. native app에서 사용하지 마세요.
	 */
	ONDK_Error("[%s:%d] DO NOT Support Function !!\r\n", __FUNCTION__, __LINE__);
	ONDK_Error("[%s:%d] DO NOT Support Function !!\r\n", __FUNCTION__, __LINE__);
#else
	DFBResult			err;
	FontPool_Item_t 	*font = (FontPool_Item_t *)fontHandle;
	IDirectFBFont		*dfbFont = (IDirectFBFont *)font->stFont.fontdata;

	if(dfbFont)
	{
		DFBCHECK(dfbFont ->Release (dfbFont));
	}

	ONDK_Free(font);
#endif
}

HINT32 ONDK_FONT_GetBitmapFont(ONDKFontHandle fontHandle, char *utf8pstr, HUINT32 w , HUINT32 h, HUINT8 *outBuffer)
{
#if defined(CONFIG_APUI_NATIVE_APP)
	/*
	 * 호환성을 위해 둔것임. native app에서 사용하지 마세요.
	 */
	ONDK_Error("[%s:%d] DO NOT Support Function !!\r\n", __FUNCTION__, __LINE__);
	ONDK_Error("[%s:%d] DO NOT Support Function !!\r\n", __FUNCTION__, __LINE__);
	return 0;
#else
	HINT32	 i, j, used_bytes;
	HUINT32 *ptr;

	ONDKSurfaceHandle  tempo_surface;
	FontPool_Item_t		*font = (FontPool_Item_t *)fontHandle;
	IDirectFBFont	 	*dfbFont = (IDirectFBFont*) font->stFont.fontdata;

	HCHAR				szChar[8] = {0, };

	used_bytes = HLIB_CP_CharAt(eHxCP_UNICODE_UTF8, utf8pstr, 0, szChar);
	tempo_surface = ONDK_SURFACE_Create(w, h, "ARGB8888" );
	ONDK_SURFACE_Clear(tempo_surface);
	ONDK_FONT_DrawString(tempo_surface, dfbFont, szChar, 0, 0, 0xFFFFFFFF, DSTF_TOPLEFT );

	ptr = ONDK_SURFACE_Lock(tempo_surface);
	for (i=0; i<h; i++)
	{
		for (j=0; j<w; j++)
		{
			outBuffer[i*w+j] = (HUINT8) ptr[i*w+j];
		}
	}
	ONDK_SURFACE_Unlock(tempo_surface);
	ONDK_SURFACE_Destroy(tempo_surface);

	return 	 used_bytes;
#endif
}

HINT32	 ONDK_FONT_DrawBitmapFont256Grey(ONDKSurfaceHandle surface, HUINT32 x, HUINT32 y, HUINT8 *fontBmp, HUINT32 font_w , HUINT32 font_h, ONDK_Color_t *color)
{
#if defined(CONFIG_APUI_NATIVE_APP)
	/*
	 * 호환성을 위해 둔것임. native app에서 사용하지 마세요.
	 */
	ONDK_Error("[%s:%d] DO NOT Support Function !!\r\n", __FUNCTION__, __LINE__);
	ONDK_Error("[%s:%d] DO NOT Support Function !!\r\n", __FUNCTION__, __LINE__);
	return 0;
#else
	DFBResult			 err;
	IDirectFBSurface	 *dfbSurface = (IDirectFBSurface*)surface;
	HUINT32		 *ptr, *tptr;
	HINT32			 pitch, i, j;

	DFBCHECK(dfbSurface->Lock (dfbSurface, DSLF_READ|DSLF_WRITE, (void**)&ptr, &pitch));
	for (i=0; i<font_h; i++)
	{
		tptr = (ptr+(y+i)*pitch/4+x );
		for (j=0; j<font_w; j++)
		{
			//*tptr  = ONDK_COLOR_BlendWithAlpha(color, (ONDK_Color_t)*tptr , fontBmp[i*font_w+j] );
			switch( (fontBmp[i*font_w+j]>>6) )
			{
			case 0:		 //printf(" ");
				//*tptr  = color_4grey[3]; 	 //  .
				break;
			case 1:		 //printf(".");
				*tptr	= color[2];
				break;
			case 2:		 //printf("*");
				*tptr	= color[1];
				break;
			case 3:		 //printf("#");
				*tptr	= color[0];
				break;
			default:
				break;
			}
			tptr++;
		}
		/*printf("\n");*/
	}
	DFBCHECK(dfbSurface->Unlock (dfbSurface));

	return  font_w;
#endif
}


HINT32	 ONDK_FONT_DrawBitmapFont4GreyLookupTable(ONDKSurfaceHandle surface, HUINT32 x, HUINT32 y, HUINT8 *fontBmp, HUINT32 font_w , HUINT32 font_h, HUINT8 *color_4grey)
{
#if defined(CONFIG_APUI_NATIVE_APP)
		/*
		 * 호환성을 위해 둔것임. native app에서 사용하지 마세요.
		 */
		ONDK_Error("[%s:%d] DO NOT Support Function !!\r\n", __FUNCTION__, __LINE__);
		ONDK_Error("[%s:%d] DO NOT Support Function !!\r\n", __FUNCTION__, __LINE__);
		return 0;
#else
	DFBResult			 err;
	IDirectFBSurface	 *dfbSurface = (IDirectFBSurface*)surface;
	HUINT8			*ptr = NULL, *tptr = NULL;
	HINT32			pitch, i, j;


	DFBCHECK(dfbSurface->Lock (dfbSurface, DSLF_READ|DSLF_WRITE, (void**)&ptr, &pitch));
	for (i=0; i<font_h; i++)
	{
		tptr = (ptr+(y+i)*pitch+x );
		for (j=0; j<font_w; j++)
		{
			switch( (fontBmp[i*font_w+j]>>6) )
			{
			case 0:		 //printf(" ");
				//*tptr  = color_4grey[3]; 	 //  .
				break;
			case 1:		 //printf(".");
				*tptr	= color_4grey[2];
				break;
			case 2:		 //printf("*");
				*tptr	= color_4grey[1];
				break;
			case 3:		 //printf("#");
				*tptr	= color_4grey[0];
				break;
			default:		 HxLOG_Print("Invalid Color Info.\n");
				break;
			}
			tptr++;
		}
		//printf("\n");
	}
	DFBCHECK(dfbSurface->Unlock (dfbSurface));

	return  font_w;
#endif
}


