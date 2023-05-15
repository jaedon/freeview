/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include 	<ondk.h>
#include 	<teletext.h>
#include 	<ttx_graphics.h>
#include 	<ttx_drcs.h>
#include 	<ttx_charmap.h>
#include	<ttx_menu.h>

#ifndef AUTOCONF_INCLUDED
#include <autoconf.h>
#endif

/*******************************************************************/
/********************      Defines         			*************************/
/*******************************************************************/

#define 	TELETEXT_FIXEDFONT_PATH "/usr/fonts/DejaVuSansMono.ttf"  // teletext 와 fixed font

#define		EBU_TELETEXT_FONTSIZE_FIXED_HEIGHT		30
#define		EBU_TELETEXT_FONTSIZE_FIXED_WIDTH		20

#define		EBU_TELETEXT_FONTSIZE_VARIABLE			30

//typedef	HUINT16	HUINT16;

#define	TTX_GFX_PAGE_NUM(dec_mxx)		(((dec_mxx) < 100) ? (dec_mxx) + 800 : (dec_mxx) % 800)
#define	TTX_GFX_HIDE_TOPBTM(sec)	((sec) >= 5)
#define TTX_PTU_BYTE_BITMAP_SIZE		(TTX_PTU_LENGTH * 6)
#define TTX_PTU_LINE_STRIDE				12
#define TTX_PTU_FP_SCALE_SHIFT			4


#define	TTX_GFX_MAX_FONT_CACHE	4



#if defined(WIN32)
#define	TTX_GFX_vsnprintf	vsprintf_s
#else
#define	TTX_GFX_vsnprintf	vsnprintf
#endif
#define	TTX_GFX_IS_VAR_FONT(state)	((state) & eTTX_STATE_EBU_SUBTITLE && (state) & eTTX_STATE_VAR_FONT)


/*******************************************************************/
/********************      Typedef        		 	 *************************/
/*******************************************************************/

#if 0
typedef struct
{
	OSD_Color_t (* pfnConvertColor)	(const HUINT16, const HUINT8);
	void		(* pfnFillRect) 	(ONDKWindowHandle *, const HINT32, const HINT32, const HINT32, const HINT32, const ONDK_Color_t);
} TTX_GC_Iface_t;	///< TTX GC Rendering Interface

#endif



/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


typedef struct
{
	IDirectFBFont 	*pSysFont;
	HINT32			BgWidth;
	HINT32			nBgHeight;
} TTX_Font_t;


typedef struct
{
	struct
	{
		HINT32	nWidth;
		HINT32	nHeight;
	} aScale[TTX_GFX_MAX_FONT_CACHE];

	struct
	{
		HUINT32	ulSize;
		HUINT32	ulRatio;
	} aAttr[TTX_GFX_MAX_FONT_CACHE];

	HUINT32		ulLast;
	HUINT32		ulCount;
} TTX_FontCache_t;

typedef struct
{
	//Handle_t			hDecoder;
	Handle_t			hDrcs;

	//OSD_Screen_t		*pFB;			///< Frame Buffer
	struct	{
		ONDK_Rect_t		rect;				///< by Rect
		ONDK_Rect_t		area;				///< by Area
	} Screen;							///< TTX Screen

	TTX_Controller_t	*pCtrl;			///< TTX Controller
	TTX_Page_t			*pPage;			///< TTX Page

	HBOOL				bBackup;
	ONDK_Rect_t			backupArea;

	HBOOL				bUpdated;		///< 화면이 업뎃되었는가?
	ONDK_Rect_t			updateArea;		///< 업뎃된 영역

	HINT32				nCellWidth;		///< Cell Width
	HINT32				nCellHeight;	///< Cell Height

	HUINT32				ulLastSec;		///< 마지막 렌더링한 시간 (TTX_Controller_t.ulElapsedSec 과 대응)

	TTX_FontCache_t		fontCache;

	struct {
		HUINT32	ulTOP;
		HUINT32	ulCTT;
		HUINT32	ulSubcodeBar;
	} Version;

	/**
	 *
	 */
	TTX_Cell_t			aSubcodeBar[TTX_MAX_COLUMN];
	HUINT8				aucRow24BgCLUT[TTX_MAX_COLUMN];

	//X_GC_Iface_t		drawIface;		///< TTX GC Rendering interface
} TTX_GC_t;



/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static ONDKSurfaceHandle		m_hsurface =NULL;
static HINT32 	m_font_height;
static HINT32 	m_font_width;


static  TTX_GC_t *s_pstGC =NULL;

/**
 * Colour Map
 *
 * ARGB4444 format color
 * @see ETSI EN 300 706 Table 30
 */
static const HUINT16	s_usCLUT[TTX_MAX_CLUT][TTX_MAX_COLOR_ENTRY] =
{
	{0xF000, 0xFF00, 0xF0F0, 0xFFF0, 0xF00F, 0xFF0F, 0xF0FF, 0xFFFF},	// [0]
	{0x0000, 0xF700, 0xF070, 0xF770, 0xF007, 0xF707, 0xF077, 0xF777},	// [1]
	{0xFF05, 0xFF70, 0xF0F7, 0xFFFB, 0xF0CA, 0xF500, 0xF652, 0xFC77},	// [2]
	{0xF333, 0xFF77, 0xF7F7, 0xFFF7, 0xF77F, 0xFF7F, 0xF7FF, 0xFDDD}	// [3]
};

/*******************************************************************/
/********************     Local Functions   		*************************/
/*******************************************************************/
#define ___LOCAL_FUNCTIONS_________

/**
 * Cell Line에 text를 쓴다.
 *
 * @param	pCell	cell line array
 * @param	fmt		string format
 * @param	...
 */
static void	local_ttxgfx_SetText (TTX_Cell_t *pCell, const char *fmt, ...)
{
	HINT32	i	= 0;
	HUINT8	szBuf[TTX_MAX_COLUMN + 1];
	va_list	ap;

	va_start(ap, fmt);
	TTX_GFX_vsnprintf((void *)szBuf, TTX_MAX_COLUMN + 1, (void *)fmt, ap);
	va_end(ap);

	while (szBuf[i] != '\0')
	{
		pCell++->ucChar	= szBuf[i++];
	}
}



/**
 * Row=0, Col=0~7을 만든다.
 *
 * @param	s_pstGC		ttx graphics context
 * @return	ERR_OK / ERR_FAIL
 */
static HERROR	local_ttxgfx_MakeCtrlPanel (void)
{
	HINT32		i;
	HINT32		start	= 3;
	HINT32		end		= 5;
	TTX_Page_t	*pPage	= s_pstGC->pPage;
	TTX_Cell_t	*aPanel	= pPage->aCellMap[0];
	TTX_State_t	state	= s_pstGC->pCtrl->state;

	if ((state & eTTX_STATE_EBU_SUBTITLE)
		|| s_pstGC->pCtrl->doubleSize == eTTX_DOUBLE_FOCUS_ON_BTM
		|| ((pPage->bNewsflash || pPage->bSubtitle) && TTX_GFX_HIDE_TOPBTM(s_pstGC->pCtrl->ulElapsedSec)))
	{
		return ERR_FAIL;
	}

	for (i = 0 ; i < 8 ; i++)
	{
		aPanel[i].ucChar	= TTX_CHAR_SPACE;
		aPanel[i].ucCode	= eTTX_CODE_G0_LATIN;
		aPanel[i].ucFgColor	= eTTX_COLOR_WHITE;
		aPanel[i].ucBgColor	= pPage->ucRowColor;
	}

	if (state & eTTX_STATE_INPUT && !(state & eTTX_STATE_SUBCODE))
	{
		local_ttxgfx_SetText(aPanel + 3, "%s", s_pstGC->pCtrl->aucInput);
	}
	else if (state & eTTX_STATE_HOLD)
	{
		start	= 2;
		local_ttxgfx_SetText(aPanel + 2, "STOP");
		aPanel[2].ucFgColor	= eTTX_COLOR_RED;
		aPanel[3].ucFgColor	= eTTX_COLOR_RED;
		aPanel[4].ucFgColor	= eTTX_COLOR_RED;
		aPanel[5].ucFgColor	= eTTX_COLOR_RED;
	}
	else
	{
		local_ttxgfx_SetText(aPanel + 3, "%03d", TTX_GFX_PAGE_NUM(s_pstGC->pCtrl->usFilterPageNum));
	}

	if (pPage->bSubtitle == FALSE && pPage->bNewsflash == FALSE && !(s_pstGC->pCtrl->state & eTTX_STATE_CANCEL))
	{
		start	= 0;
		end		= 7;
	}

	for (i = start ; i <= end ; i++)
	{
		aPanel[i].bDisplayable	= TRUE;
	}
	return ERR_OK;
}


/**
 * Subcode Row24를 만든다.
 *
 * @param	s_pstGC		ttx graphics context
 * @return	ERR_OK / ERR_FAIL
 */
static HERROR	local_ttxgfx_MakeRow24ForSubcode (void)
{
	HINT32	i;
	TTX_Page_t		*pPage	= s_pstGC->pPage;
	TTX_Cell_t		*aRow24	= pPage->aCellMap[24];

	for (i = 0 ; i < TTX_MAX_COLUMN ; i++)
	{
		if (i == 0)
		{
			if (pPage->bSubtitle || pPage->bNewsflash)
			{
				s_pstGC->aucRow24BgCLUT[i]	= 1;
				aRow24[i].ucBgColor	= 0;
			}
			else
			{
				aRow24[i].ucBgColor	= pPage->ucRowColor;
			}
		}
		else if (i <= 5)
		{
			aRow24[i].ucBgColor	= eTTX_COLOR_RED;
		}
		else if (i <= 10)
		{
			aRow24[i].ucBgColor	= eTTX_COLOR_GREEN;
		}
		else
		{
			aRow24[i].ucBgColor	= eTTX_COLOR_YELLOW;
		}

		aRow24[i].ucCode		= eTTX_CODE_G0_LATIN;
		aRow24[i].ucFgColor		= eTTX_COLOR_BLACK;
		aRow24[i].ucChar		= TTX_CHAR_SPACE;
		aRow24[i].bDisplayable	= TRUE;
	}
	aRow24[3].ucChar	= '-';
	aRow24[8].ucChar	= '+';

	local_ttxgfx_SetText(aRow24 + 23, "%s", s_pstGC->pCtrl->aucInput);

	return ERR_OK;
}


/**
 * TOP Row24를 만든다.
 *
 * @param	s_pstGC		ttx graphics context
 * @return	ERR_OK / ERR_FAIL
 */
static HERROR	local_ttxgfx_MakeRow24ForTOP (void)
{
	HINT32	i;
	TTX_Page_t		*pPage		= s_pstGC->pPage;
	TTX_Cell_t		*aRow24		= pPage->aCellMap[24];
	TTX_TOP_Link_t	*pYellow	= NULL;
	TTX_TOP_Link_t	*pBlue		= NULL;

	for (i = 0 ; i < TTX_MAX_COLUMN ; i++)
	{
		if (1 <= i && i <= 5)
		{
			aRow24[i].ucBgColor	= eTTX_COLOR_RED;
		}
		else if (6 <= i && i <= 10)
		{
			aRow24[i].ucBgColor	= eTTX_COLOR_GREEN;
		}
		else if (11 <= i && i <= 25)
		{
			aRow24[i].ucBgColor	= eTTX_COLOR_YELLOW;
		}
		else if (26 <= i)
		{
			aRow24[i].ucBgColor	= eTTX_COLOR_CYAN;
		}
		else	// 0
		{
			if (pPage->bSubtitle || pPage->bNewsflash)
			{
				s_pstGC->aucRow24BgCLUT[i]	= 1;
				aRow24[i].ucBgColor	= 0;
			}
			else
			{
				aRow24[i].ucBgColor	= pPage->ucRowColor;
			}
		}

		aRow24[i].ucChar		= TTX_CHAR_SPACE;
		aRow24[i].ucCode		= pPage->ucDefaultCode;
		aRow24[i].ucFgColor		= eTTX_COLOR_BLACK;
		aRow24[i].bDisplayable	= TRUE;
		aRow24[i].bCompositeChar	= FALSE;
		aRow24[i].ucNationalOption	= pPage->ucDefaultNOS;
	}

	aRow24[3].ucChar	= '-';
	aRow24[8].ucChar	= '+';

	pYellow	= &(pPage->TOP.aLink[2]);
	pBlue	= &(pPage->TOP.aLink[3]);

	for (i = 0 ; i < 12 ; i++)
	{
		aRow24[i + 13].ucChar	= pYellow->aucTitle[i];
		aRow24[i + 27].ucChar	= pBlue->aucTitle[i];
	}

	if (s_pstGC->Version.ulTOP != pPage->TOP.ulVersion)
	{
		TTX_BITSET(s_pstGC->pCtrl->refresh, eTTX_REFRESH_ROW24);
	}
	return ERR_OK;
}



/**
 * CTT Row24를 만든다. (Cycle Trace)
 *
 * @param	s_pstGC		ttx graphics context
 * @return	ERR_OK / ERR_FAIL
 */
static HERROR	local_ttxgfx_MakeRow24ForCTT (void)
{
	HINT32		i;
	TTX_Page_t	*pPage	= s_pstGC->pPage;
	TTX_Cell_t	*aRow24	= pPage->aCellMap[24];

	for (i = 0 ; i < TTX_MAX_COLUMN ; i++)
	{
		if (1 <= i && i <= 9)
		{
			aRow24[i].ucBgColor	= eTTX_COLOR_RED;
		}
		else if (11 <= i && i <= 19)
		{
			aRow24[i].ucBgColor	= eTTX_COLOR_GREEN;
		}
		else if (21 <= i && i <= 29)
		{
			aRow24[i].ucBgColor	= eTTX_COLOR_YELLOW;
		}
		else if (31 <= i && i <= 39)
		{
			aRow24[i].ucBgColor	= eTTX_COLOR_CYAN;
		}
		else
		{
			if (pPage->bSubtitle || pPage->bNewsflash)
			{
				s_pstGC->aucRow24BgCLUT[i]	= 1;
				aRow24[i].ucBgColor	= 0;
			}
			else
			{
				aRow24[i].ucBgColor	= pPage->ucRowColor;
			}
		}
		aRow24[i].ucChar		= TTX_CHAR_SPACE;
		aRow24[i].ucCode		= pPage->ucDefaultCode;
		aRow24[i].ucFgColor		= eTTX_COLOR_BLACK;
		aRow24[i].bDisplayable	= TRUE;
		aRow24[i].bCompositeChar	= FALSE;
		aRow24[i].ucNationalOption	= pPage->ucDefaultNOS;
	}

	for (i = 0 ; i < 4 ; i++)
	{
		local_ttxgfx_SetText(aRow24 + (i * 10 + 4), "%03d", TTX_GFX_PAGE_NUM(pPage->TRACE.ausTrace[i]));
	}

	if (s_pstGC->Version.ulCTT != pPage->TRACE.ulVersion)
	{
		TTX_BITSET(s_pstGC->pCtrl->refresh, eTTX_REFRESH_ROW24);
	}
	return ERR_OK;

}




/**
 * Row 24를 만든다.
 *
 * @param	s_pstGC		ttx graphics context
 * @return	ERR_OK / ERR_FAIL
 */
static HERROR	local_ttxgfx_MakeRow24 (void)
{
	HERROR				err;
	HINT32				i;
	TTX_Page_t			*pPage	= s_pstGC->pPage;
	TTX_Cell_t			*aRow24	= pPage->aCellMap[24];
	TTX_Controller_t	*pCtrl	= s_pstGC->pCtrl;

	if (pCtrl->state & (eTTX_STATE_CANCEL | eTTX_STATE_EBU_SUBTITLE))
	{
		return ERR_FAIL;
	}

	if ((pPage->bNewsflash || pPage->bSubtitle) && TTX_GFX_HIDE_TOPBTM(pCtrl->ulElapsedSec))
	{
		// TODO:
		// 이전에 Newsflash나 Subtitle이면서 Link Control이 1로 들어와
		// Row24가 보이지 않는 다는 이슈는... 있었지만, 다른 Navi 고려해서.. 우선 넣지 말아보자.
		return ERR_FAIL;
	}

	HxSTD_MemSet(s_pstGC->aucRow24BgCLUT, 0, TTX_MAX_COLUMN);
	if (pCtrl->state & eTTX_STATE_SUBCODE)
	{
		err	= local_ttxgfx_MakeRow24ForSubcode();
	}
	else if (pCtrl->navi == eTTX_NAVI_TOP && pPage->TOP.bEnabled == TRUE)
	{
		err	= local_ttxgfx_MakeRow24ForTOP();
	}
	else if (pPage->FLOF.bLinkControl == TRUE)	// FLOF
	{
		err	= ERR_OK;	// row 24가 packet을 통해 들어와 있음
	}
	else if (pPage->TRACE.ulVersion > 0)
	{
		err	= local_ttxgfx_MakeRow24ForCTT();
	}
	else
	{
		err	= ERR_FAIL;
	}

	if (err == ERR_OK && pPage->bEnabled)
		return ERR_OK;

	for (i = 0 ; i < TTX_MAX_COLUMN ; i++)
	{
		if (1 <= i && i <= 5)
		{
			aRow24[i].ucBgColor	= eTTX_COLOR_RED;
		}
		else if (6 <= i && i <= 10)
		{
			aRow24[i].ucBgColor	= eTTX_COLOR_GREEN;
		}
		else
		{
			aRow24[i].ucBgColor	= pPage->ucRowColor;
		}
		aRow24[i].ucCode		= eTTX_CODE_G0_LATIN;
		aRow24[i].ucFgColor		= eTTX_COLOR_BLACK;
		aRow24[i].ucChar		= TTX_CHAR_SPACE;
		aRow24[i].bDisplayable	= TRUE;
	}
	aRow24[3].ucChar	= '-';
	aRow24[8].ucChar	= '+';

	return ERR_OK;
}


/**
 * Row 25에 그려질 Subcode Bar를 만든다.
 *
 * @param	s_pstGC		ttx graphics context
 * @return	ERR_OK / ERR_FAIL
 */
static HERROR	local_ttxgfx_MakeSubcodeBar(void)
{
	HUINT32		i;
	HUINT32		ulHighlight	= 0;
	TTX_Page_t	*pPage		= s_pstGC->pPage;
	TTX_Cell_t	*pBar		= s_pstGC->aSubcodeBar;
	HUINT8		*pucList	= pPage->SubcodeBar.aucList;
	HUINT16		usSubcode	= 0;
//	OSD_Font_t	*pFont;

	if (pPage->SubcodeBar.ulVersion == 0 || pPage->SubcodeBar.ulCount == 0)
	{
		return ERR_FAIL;
	}

	HxSTD_MemSet(pBar, 0, sizeof(TTX_Cell_t) * TTX_MAX_COLUMN);
#if 0
	pFont	= ONDK_FONT_CreateSystemFont(eFont_SystemFixed);
	if (pFont)	// Use TTF Font(Unicode)
	{
		pBar[1].ucCode	= eTTX_CODE_USER_DEFINED;
		pBar[1].ucChar	= eTTX_CHAR_USER_ROTATE;
		pBar[1].ucFgColor	= eTTX_COLOR_BLUE;
		pBar[1].bDoubleWidth	= TRUE;
		pBar[1].bDisplayable	= TRUE;

		ONDK_FONT_Release(pFont);
	}
	else	// Use Default bmp font
#endif
	{
		pBar[1].ucCode	= eTTX_CODE_USER_DEFINED;
		pBar[2].ucCode	= eTTX_CODE_USER_DEFINED;
		pBar[1].ucChar	= eTTX_USERDEF_BMP_ROTATE_L;
		pBar[2].ucChar	= eTTX_USERDEF_BMP_ROTATE_R;
		pBar[1].ucFgColor	= eTTX_COLOR_BLUE;
		pBar[2].ucFgColor	= eTTX_COLOR_BLUE;
		pBar[1].bDisplayable	= TRUE;
		pBar[2].bDisplayable	= TRUE;
	}

	if (s_pstGC->pCtrl->state & eTTX_STATE_SEARCH)
		usSubcode	= s_pstGC->pCtrl->usFilterSubcode;
	else
		usSubcode	= pPage->usSubcode;

	for (i = 0 ; i < pPage->SubcodeBar.ulCount ; i++)
	{
		local_ttxgfx_SetText(&pBar[i * 3 + 4], "%02X", pucList[i]);
		pBar[i * 3 + 4].bDisplayable	= TRUE;
		pBar[i * 3 + 5].bDisplayable	= TRUE;
		pBar[i * 3 + 4].ucFgColor	= eTTX_COLOR_BLUE;
		pBar[i * 3 + 5].ucFgColor	= eTTX_COLOR_BLUE;

		if (usSubcode == (HUINT16)pPage->SubcodeBar.aucList[i])
		{
			ulHighlight	= i * 3 + 4;
		}
	}

	if (s_pstGC->pCtrl->state & eTTX_STATE_ROTATE_SUBPAGE)
	{
		ulHighlight	= 1;
	}

	if (ulHighlight != 0)
	{
		pBar[ulHighlight].ucBgColor	= eTTX_COLOR_BLUE;
		pBar[ulHighlight + 1].ucBgColor	= eTTX_COLOR_BLUE;
		pBar[ulHighlight].ucFgColor	= eTTX_COLOR_WHITE;
		pBar[ulHighlight + 1].ucFgColor	= eTTX_COLOR_WHITE;
	}

	if (s_pstGC->Version.ulSubcodeBar != pPage->SubcodeBar.ulVersion)
	{
		TTX_BITSET(s_pstGC->pCtrl->refresh, eTTX_REFRESH_SUB_BAR);
	}
	return ERR_OK;
}



/**
 * 화면의 그려진 영역을 기억한다. (나중에 한번에 Flush)
 *
 * @param	s_pstGC		TTX Graphics Context
 * @param	x
 * @param	y
 * @param	w
 * @param	h
 */
static void	local_ttxgfx_UpdateArea (const HINT32 x, const HINT32 y, const HINT32 w, const HINT32 h)
{
	HINT32		x1	= x;
	HINT32		y1	= y;
	HINT32		x2	= x + w - 1;
	HINT32		y2	= y + h - 1;
	ONDK_Rect_t	*pScr	= &s_pstGC->Screen.area;

	if (x1 > x2 || y1 > y2)
	{
		return;
	}

	if (x1 < pScr->x)	x1	= pScr->x;
	if (x1 > pScr->w)	x1	= pScr->w;
	if (y1 < pScr->y)	y1	= pScr->y;
	if (y1 > pScr->h)	y1	= pScr->h;
	if (x2 < pScr->x)	x2	= pScr->x;
	if (x2 > pScr->w)	x2	= pScr->w;
	if (y2 < pScr->y)	y2	= pScr->y;
	if (y2 > pScr->h)	y2	= pScr->h;

	if (s_pstGC->bUpdated == FALSE)
	{
		s_pstGC->updateArea.x	= x1;
		s_pstGC->updateArea.y	= y1;
		s_pstGC->updateArea.w	= x2;
		s_pstGC->updateArea.h	= y2;
		s_pstGC->bUpdated	= TRUE;
	}
	else
	{
		if (x1 < s_pstGC->updateArea.x)	s_pstGC->updateArea.x	= x1;
		if (y1 < s_pstGC->updateArea.y)	s_pstGC->updateArea.y	= y1;
		if (x2 > s_pstGC->updateArea.w)	s_pstGC->updateArea.w	= x2;
		if (y2 > s_pstGC->updateArea.h)	s_pstGC->updateArea.h	= y2;
	}
}

/**
 * Rendering 할 준비를 한다.
 * - Ctrl Panel
 * - Row24
 * - Subcode bar
 *   를 만든다.
 *
 * @param	s_pstGC		ttx graphics context
 * @return	ERR_OK / ERR_FAIL
 */
static HERROR	local_ttxgfx_Prepare(void)
{
	HERROR		err;
	HINT32		i;
	TTX_Page_t	*pPage	= s_pstGC->pPage;

	err	= local_ttxgfx_MakeCtrlPanel();

	if (err != ERR_OK)
	{
		for (i = 0 ; i < 8 ; i++)
			pPage->aCellMap[0][i].bDisplayable	= FALSE;
	}

	err	= local_ttxgfx_MakeRow24();
	if (err != ERR_OK)
	{
		for (i = 0 ; i < TTX_MAX_COLUMN ; i++)
			pPage->aCellMap[24][i].bDisplayable	= FALSE;
	}

	err	= local_ttxgfx_MakeSubcodeBar();
	if (err != ERR_OK)
	{
		for (i = 0 ; i < TTX_MAX_COLUMN ; i++)
			s_pstGC->aSubcodeBar[i].bDisplayable	= FALSE;
	}
	return ERR_OK;
}

static INLINE ONDK_Color_t	local_ttxgfx_To16BitColor (const HUINT16 argb4444, const HUINT8 alpha)
{
	HUINT8	alpha16bit	= alpha >> 4;

	return (ONDK_Color_t)((argb4444 & 0x0FFF) | (alpha16bit << 12));
}

/**
 * 16bit color의 TTX CLUT로부터 OSD_Color_t를 얻어온다.
 *
 * @param	s_pstGC			TTX Graphics Context
 * @param	ucCLUT		CLUT Number[0..3]
 * @param	ucEntry		Entry Number
 * @param	ucAlpha		ARGB의 Alpha Value
 * @return	OSD_Color_t 타입의 컬러
 */
static ONDK_Color_t	local_ttxgfx_GetColor (const HUINT8 ucCLUT, const HUINT8 ucEntry, const HUINT8 ucAlpha)
{
	HUINT16	argb4444;

	argb4444	= s_usCLUT[ucCLUT % TTX_MAX_CLUT][ucEntry % TTX_MAX_COLOR_ENTRY];
	if (argb4444 == 0)	// means 'Transparent', Valid at Presentation Lv 2.5 & 3.5
	{
		return (ONDK_Color_t)0;
	}
	return local_ttxgfx_To16BitColor(argb4444,ucAlpha);
}


/**
 * 화면을 Screen Color 또는 투명으로 Clear
 *
 * @param	s_pstGC		ttx graphics context
 * @param	pCtrl	ttx controller
 * @return	ERR_OK / ERR_FAIL
 */
static HERROR	local_ttxgfx_Clear	(TTX_Controller_t *pCtrl)
{
	TTX_Refresh_t	refresh	= pCtrl->refresh;
	TTX_Page_t		*pPage	= s_pstGC->pPage;
	ONDK_Color_t			clrColor;
	if (pPage->bSubtitle || pPage->bNewsflash || pCtrl->state & eTTX_STATE_CANCEL)
	{
		if (TTX_GFX_HIDE_TOPBTM(s_pstGC->ulLastSec) != TTX_GFX_HIDE_TOPBTM(pCtrl->ulElapsedSec))
		{
			pCtrl->refresh	= refresh	= eTTX_REFRESH_ALL;
		}
		clrColor	= 0x00000000;	// transparent
	}
	else
	{
		clrColor	= local_ttxgfx_GetColor(0, s_pstGC->pPage->ucScreenColor, pCtrl->mix);
	}

	if (refresh == eTTX_REFRESH_ALL)
	{
		ONDK_DRAW_FillRect16(m_hsurface,s_pstGC->Screen.rect,clrColor);
		local_ttxgfx_UpdateArea(s_pstGC->Screen.rect.x, s_pstGC->Screen.rect.y,s_pstGC->Screen.rect.w, s_pstGC->Screen.rect.h	);
	}

	return ERR_OK;
}

/**
 * 조건에 맞춰 CLUT 리턴
 *
 * @param	s_pstGC		ttx graphics context
 * @param	row		row
 * @param	col		column
 * @param	pucFg	[OUT] foreground CLUT
 * @param	pucBg	[OUT] background CLUT
 */
static void	local_ttxgfx_GetCLUT (const HINT32 row, const HINT32 col, HUINT8 *pucFg, HUINT8 *pucBg)
{
	TTX_Page_t	*pPage	= s_pstGC->pPage;

	if (row < 24)
	{
		*pucFg	= pPage->ucFgCLUT;
		*pucBg	= pPage->ucBgCLUT;
	}
	else if (row == 24)
	{
		*pucFg = 0;
		*pucBg = s_pstGC->aucRow24BgCLUT[col];
	}
	else if (row == 25)
	{
		*pucFg = *pucBg = 0;
	}
	else
	{
		*pucFg = *pucBg = 0;
	}
}


/**
 * 각 상태 및 조건에 따라 현재 Cell을 Skip 할지 여부에 대한 판단을 한다.
 *
 * @param	s_pstGC		ttx graphics context
 * @param	pCell	cell
 * @param	row		row
 * @param	col		column
 * @return	TRUE / FALSE
 */
static HBOOL	local_ttxgfx_Skip (const TTX_Cell_t *pCell, const HINT32 row, const HINT32 col)
{
	TTX_Controller_t *pCtrl	= s_pstGC->pCtrl;
	TTX_Refresh_t	refresh	= pCtrl->refresh;
	TTX_Page_t		*pPage	= s_pstGC->pPage;

	if (pCell->bDisplayable == FALSE)
	{
		return TRUE;
	}

	//2 Controller State Check
	if (pCtrl->state & eTTX_STATE_CANCEL)
	{
		if (row > 0)	return TRUE;
		if (col > 7)	return TRUE;
	}

	//2 EBU Subtitle Check
	if (pCtrl->state & eTTX_STATE_EBU_SUBTITLE)
	{
		if (row < 1 || 23 < row)
			return TRUE;
	}

	//2 Double Size Check
	if (pCtrl->doubleSize == eTTX_DOUBLE_FOCUS_ON_TOP)
	{
		if (12 <= row && row <= 23)
			return TRUE;
	}
	else if (pCtrl->doubleSize == eTTX_DOUBLE_FOCUS_ON_BTM)
	{
		if (0 <= row && row <= 11)	// 11은 반 잘려서 그려질 수 있기 때문에 Skip하지 않는다.
			return TRUE;
	}

	//2 Page Control Bit Check
	if (pPage->bSuppressHeader && row == 0)
	{
		if (8 <= col || TTX_GFX_HIDE_TOPBTM(pCtrl->ulElapsedSec))
			return TRUE;
	}
	if (pPage->bInhibitDisplay && 1 <= row && row <= 23)
		return TRUE;

	//2 Cell Attribute Check
	if (pCell->bFlash && refresh & eTTX_REFRESH_FLASH)		return FALSE;
	if (pCell->bConceal && refresh & eTTX_REFRESH_CONCEAL)	return FALSE;

	//2 Refresh Area Check
	if (row == 0)
	{
		if (col <= 7)
		{
			if (!(refresh & eTTX_REFRESH_CTRL_PANEL))
				return TRUE;
		}
		else if (col <= 31)
		{
			if (!(refresh & eTTX_REFRESH_TITLE))
				return TRUE;
		}
		else
		{
			if (!(refresh & eTTX_REFRESH_TIME))
				return TRUE;
		}
	}
	else if (row <= 23)
	{
		if (!(refresh & eTTX_REFRESH_BODY))
			return TRUE;
	}
	else if (row == 24)
	{
		if (!(refresh & eTTX_REFRESH_ROW24))
			return TRUE;
	}
	else
	{
		if (!(refresh & eTTX_REFRESH_SUB_BAR))
			return TRUE;
	}
	return FALSE;
}


// 9가지의 폰트만 나올수있다. 한번만 생성하자.
ONDKFontHandle local_ttxgfx_Createfont(HUINT32 index,HUINT32 width, HUINT32 height)
{
	static ONDKFontHandle pfixedfont_1_1 =NULL;

	static ONDKFontHandle pfixedfont_1_2 =NULL;
	static ONDKFontHandle pfixedfont_2_1 =NULL;
	static ONDKFontHandle pfixedfont_2_2 =NULL;

	static ONDKFontHandle pfixedfont_2_4 =NULL;
	static ONDKFontHandle pfixedfont_4_2 =NULL;
	static ONDKFontHandle pfixedfont_4_4 =NULL;

	static ONDKFontHandle pvariablefont =NULL;
	static ONDKFontHandle pfixedfont_EBU =NULL;

	static HBOOL		  createfont = TRUE;
	HINT32 font_width,font_height;

	if(createfont ==TRUE)
	{
		font_width =  width + 4;
		font_height=  height- 4;

		pfixedfont_1_1 = ONDK_FONT_Create(TELETEXT_FIXEDFONT_PATH, font_width, font_height);

		font_width =  width - 4;


		font_height <<= 1;
		pfixedfont_1_2 = ONDK_FONT_Create(TELETEXT_FIXEDFONT_PATH, font_width, font_height);

		font_width <<= 1;	  font_height >>= 1;
		pfixedfont_2_1 = ONDK_FONT_Create(TELETEXT_FIXEDFONT_PATH, font_width, font_height);

		font_height <<= 1;
		pfixedfont_2_2 = ONDK_FONT_Create(TELETEXT_FIXEDFONT_PATH, font_width, font_height);

		font_height <<= 1;
		pfixedfont_2_4 = ONDK_FONT_Create(TELETEXT_FIXEDFONT_PATH, font_width, font_height);

		font_width <<= 1;	  font_height >>= 1;
		pfixedfont_4_2 = ONDK_FONT_Create(TELETEXT_FIXEDFONT_PATH, font_width, font_height);

		font_height <<= 1;
		pfixedfont_4_4 = ONDK_FONT_Create(TELETEXT_FIXEDFONT_PATH, font_width, font_height);
		createfont =FALSE;

		pfixedfont_EBU = ONDK_FONT_Create(TELETEXT_FIXEDFONT_PATH,EBU_TELETEXT_FONTSIZE_FIXED_WIDTH,  EBU_TELETEXT_FONTSIZE_FIXED_HEIGHT);

		font_height=  EBU_TELETEXT_FONTSIZE_VARIABLE;
		pvariablefont = ONDK_FONT_CreateSystemFont(eFont_SystemNormal,	font_height);
	}

	switch(index)
	{
		case 0 :	return pfixedfont_1_1;
		case 1 :	return pfixedfont_2_1;
		case 10 :	 return pfixedfont_1_2;
		case 11 :	 return pfixedfont_2_2;

		case 100 :	  return pfixedfont_2_2;
		case 101 :	  return pfixedfont_4_2;
		case 110 :	  return pfixedfont_2_4;
		case 111 :	  return pfixedfont_4_4;

		case 20 :		return pvariablefont;
		case 21 :		return pfixedfont_EBU;
	}

	return NULL;
}


/**
 * 조건 및 상태에 맞게 TTX_Font_t를 만들어 리턴
 *
 * @param	s_pstGC		ttx graphics context
 * @param	pCell	이 Cell에 맞게 폰트 리턴
 * @param	row		row에 따라 사용하는 폰트가 다름
 * @return	TTX_Font_t 객체 or NULL
 */

static ONDKFontHandle 	local_ttxgfx_GetFont (const TTX_Cell_t *pCell, const HINT32 row)
{
	TTX_State_t 					state	= s_pstGC->pCtrl->state;
	HUINT32 					    selection=0;
	m_font_width =  s_pstGC->nCellWidth;
	m_font_height=  s_pstGC->nCellHeight;

    if (pCell->bDoubleWidth){
		m_font_width <<= 1; 	selection += 1;
	}

	if (pCell->bDoubleHeight){
		 m_font_height <<= 1; 	selection += 10;
	}
	if (s_pstGC->pCtrl->doubleSize != eTTX_DOUBLE_OFF && row >= 24)	{
		 m_font_height >>= 1;	selection = 0;
	}
    else    if(s_pstGC->pCtrl->doubleSize != eTTX_DOUBLE_OFF && row < 24)
    {
        selection += 100;
    }

	if((state) & eTTX_STATE_EBU_SUBTITLE)
	{
		if(TTX_GFX_IS_VAR_FONT(state))
		{
			return local_ttxgfx_Createfont(20,s_pstGC->nCellWidth, s_pstGC->nCellHeight);
		}
		else
		{
			return local_ttxgfx_Createfont(21,s_pstGC->nCellWidth, s_pstGC->nCellHeight);
		}

	/*
	if (TTX_GFX_IS_VAR_FONT(state)	&& 0 < row && row < 24
		&& pCell->ucCode != eTTX_CODE_G1_MOSAIC		&& pCell->ucCode != eTTX_CODE_G3_MOSAIC)
		return local_ttxgfx_Createfont(20,s_pstGC->nCellWidth, s_pstGC->nCellHeight);

	*/
	}
	else
	{
		return local_ttxgfx_Createfont(selection,s_pstGC->nCellWidth, s_pstGC->nCellHeight);
	}
}



/**
 * G1 Cell을 Rendering
 *
 * @param	s_pstGC		ttx graphics context
 * @param	pCell	cell
 * @param	x		x 좌표
 * @param	y		y 좌표
 * @param	w		width
 * @param	h		height
 * @param	color	OSD_Color_t
 * @return	ERR_OK / ERR_FAIL
 */
static HERROR	local_ttxgfx_RenderingG1(
											const TTX_Cell_t *pCell,
											const HINT32 x,
											const HINT32 y,
											const HINT32 w,
											const HINT32 h,
											const ONDK_Color_t color)
{
	HINT32 	nMarginW	= 0;
//	HINT32	nMarginH	= 0;

	HINT32	x1,x2,y1,y2,y3,width,heigth;

	if (!(pCell->ucChar & 0x20))
	{
		return ERR_FAIL;
	}

	if (pCell->Opt.bSeparatedMosaic)
	{
		nMarginW	= w / 10;
//		nMarginH	= h / 15;
	}
	x1 = x ;
	x2 = x + w/2;

	y1 = y;
	y2 = y+ h/3;
	y3 = y+ h/3 *2;

	width  = w /2 + nMarginW + 1;
	heigth = h /2 + nMarginW - 1;

	if (pCell->ucChar & 0x01)	// TOP, LEFT
	{
		ONDK_DRAW_FillRect16(m_hsurface,ONDK_Rect( x1, y1, width , heigth ),color);
	}
	if (pCell->ucChar & 0x02)	// TOP, RIGHT
	{
		ONDK_DRAW_FillRect16(m_hsurface,ONDK_Rect(  x2, y1, width, heigth ),color);
	}
	if (pCell->ucChar & 0x04)	// MIDDLE, LEFT
	{
		ONDK_DRAW_FillRect16(m_hsurface,ONDK_Rect( x1, y2, width , heigth),color);
	}
	if (pCell->ucChar & 0x08)	// MIDDLE, RIGHT
	{
		ONDK_DRAW_FillRect16(m_hsurface,ONDK_Rect(  x2, y2, width, heigth),color);
	}
	if (pCell->ucChar & 0x10)	// BOTTOM, LEFT
	{
		ONDK_DRAW_FillRect16(m_hsurface,ONDK_Rect( x1, y3, width , heigth),color);
	}
	if (pCell->ucChar & 0x40)	// BOTTOM, RIGHT
	{
		ONDK_DRAW_FillRect16(m_hsurface,ONDK_Rect(x2, y3, width, heigth),color);
	}
	return ERR_OK;
}

/**
 * row와 col 및 상태를 기준으로, xy좌표를 얻는다.
 *
 * @param	s_pstGC		ttx graphics context
 * @param	row		row
 * @param	col		column
 * @param	x		[OUT]
 * @param	y		[OUT]
 * @return	ERR_OK / ERR_FAIL
 */
static HERROR	local_ttxgfx_GetCoord (const HINT32 row, const HINT32 col, HINT32 *x, HINT32 *y)
{
	TTX_Controller_t *pCtrl	= s_pstGC->pCtrl;
	HINT32		startX		= pCtrl->nMarginW;
	HINT32		startY		= pCtrl->nMarginH;
	HINT32		nCellWidth	= s_pstGC->nCellWidth;
	HINT32		nCellHeight	= s_pstGC->nCellHeight;

	if (row <= 23)
	{
		if (pCtrl->doubleSize == eTTX_DOUBLE_FOCUS_ON_BTM)
		{
			startY	= pCtrl->nMarginH - s_pstGC->nCellHeight * 12;	// -좌표부터 그리기 시작한다...
		}
	}
	else	// row 24 이상은 더블사이즈속성에 영향을 받지 않는다.
	{
		if (pCtrl->doubleSize != eTTX_DOUBLE_OFF)
		{
			nCellHeight /= 2;
		}
	}

	*x	= startX + col * nCellWidth;
	*y	= startY + row * nCellHeight;
	return ERR_OK;
}


static inline HUINT8	teletextfont_GetPixel (const HUINT8 *ptu, HINT32 x, HINT32 y, const HUINT16 w,const HUINT16 h)
{
	HINT32	sx, sy;
	sx	= 12 * x / w;
	sy	= 10 * y / h;

	if(ptu[12*sy + sx])
	{
		return 1;
	}
	return 0;
}

/**
 * 1cell 의 PTU를 Rendering
 *
 * @param	s_pstGC		ttx graphics context
 * @param	pCell	cell
 * @param	pFont	TTX_Font_t
 * @param	x		x 좌표
 * @param	y		y 좌표
 * @param	ucCLUT	foreground CLUT
 * @return	ERR_OK / ERR_FAIL
 */
static HERROR	local_ttxgfx_RenderingPTU (
											const TTX_Cell_t *pCell,
											const HINT32 x,
											const HINT32 y,
											const HUINT8 ucFgCLUT,
											const HUINT8 ucBgCLUT,
											const HUINT8 ucAlpha)
{
	HERROR	err;
	HINT32	i,j;
	HUINT16	*pBuf32	= NULL;
	HUINT16 *pFB32	= NULL;
	HUINT8	aPTU[TTX_PTU_BYTE_BITMAP_SIZE];
	HUINT32 clrFg, clrBg;
	HINT32 	cellW, cellH;
	pFB32 = (HUINT16*)ONDK_SURFACE_Lock(m_hsurface);

	if (pFB32 == NULL)
	{
		HxLOG_Critical("Fail to Surface LOCK!!!\n");
		return ERR_FAIL;
	}
	clrFg = local_ttxgfx_GetColor(ucFgCLUT, pCell->ucFgColor, 0xFF);
	clrBg = local_ttxgfx_GetColor(ucBgCLUT, pCell->ucBgColor, ucAlpha);
	cellW = m_font_width;
	cellH = m_font_height;
	err = TTX_DRCS_GetPTU(s_pstGC->hDrcs, pCell, aPTU);
	if(err != ERR_TTX_OK)
	{
		// log
	}

	pFB32 += TELETEXT_SCREENWIDTH* y + x;
	for(j=0; j< cellH;j++)
	{

		pBuf32 =pFB32;

		for(i=0;i<cellW;i++)
		{
			if(teletextfont_GetPixel(aPTU,i,j,cellW,cellH))
				*(pBuf32+i) =clrFg;
			else
				*(pBuf32+i) =clrBg;
		}
		pFB32 += TELETEXT_SCREENWIDTH;
	}
	ONDK_SURFACE_Unlock(m_hsurface);
	return ERR_OK;
}



/**
 * PTU를 Rendering
 *
 * @param	s_pstGC		ttx graphics context
 * @param	pCell	cell
 * @param	pFont	TTX_Font_t
 * @param	x		x 좌표
 * @param	y		y 좌표
 * @param	ucCLUT	foreground CLUT
 * @return	ERR_OK / ERR_FAIL
 */
static HERROR	local_ttxgfx_RenderingChar (
											const TTX_Cell_t 	*pCell,
											ONDKFontHandle pFont,
											const HINT32 x,
											const HINT32 y,
											const HUINT8 ucFgCLUT,
											const HUINT8 ucBgCLUT,
											const HUINT8 ucAlpha,//)
											HINT32 *pXAdvance)
{
	HERROR	err		= ERR_FAIL;
	HUINT32 i;
	HUINT16	aUcs2[2] = { 0, 0 };
	HINT32	nCpRet,fontwidth,fontheight;
	HCHAR	sz300468Str[16];
//	ONDKSurfaceTextFlags textflag =DSTF_TOPLEFT;
    ONDK_Color_t color;

	if (pCell->ucCode == eTTX_CODE_G1_MOSAIC)
	{
		ONDK_DRAW_FillRect16(m_hsurface, ONDK_Rect(x, y,  m_font_width,  m_font_height),local_ttxgfx_GetColor( ucBgCLUT, pCell->ucBgColor, ucAlpha));
		err = local_ttxgfx_RenderingG1(pCell, x, y,m_font_width, m_font_height,	local_ttxgfx_GetColor(ucFgCLUT, pCell->ucFgColor, 0xFF));
	}
	if (err != ERR_OK && pFont != NULL)
	{
		err	= TTX_CHAR_ToUCS2(pCell, (void *)&aUcs2[1], (void *)&aUcs2[0]);
		if (err == ERR_OK)
		{
            fontheight = ONDK_FONT_GetHeight(pFont);

            if(s_pstGC->pCtrl->state & eTTX_STATE_EBU_SUBTITLE)
            {
				m_font_height= fontheight + 5;
            }
			// [0] Rendering 'diacritical mark'
			// [1] Rendering 'character'
			for (i = 0 ; i < 2 ; i++)
			{
				if (aUcs2[i] == 0)
					continue;
				HxSTD_MemSet(sz300468Str, 0, 16);
				nCpRet = HLIB_CP_ConvertUCS2ToCodepage(eHxCP_UNICODE_UTF8, &aUcs2[i], sz300468Str, 4);

				if( !(s_pstGC->pCtrl->state & eTTX_STATE_EBU_SUBTITLE)) // teletext
				{
					ONDK_DRAW_FillRect16(m_hsurface,ONDK_Rect(x, y, m_font_width, m_font_height),local_ttxgfx_GetColor(ucBgCLUT, pCell->ucBgColor, ucAlpha));
				}

				if (nCpRet <= eHxCP_ERROR_OUT_OF_NUM)
				{
					// 변환 실패
					HxLOG_Print("TTX HLIB_CP_ConvertUCS2ToCodepage Error!");
				}
				else
				{
                    fontwidth = ONDK_FONT_GetStringWidth(pFont,sz300468Str );

					if(s_pstGC->pCtrl->state & eTTX_STATE_EBU_SUBTITLE) // ebu subtitle  뒷배경 20% 알파 블랜딩
					{
						ONDK_DRAW_FillRect16(m_hsurface,ONDK_Rect(x, y, fontwidth, m_font_height),local_ttxgfx_GetColor(ucBgCLUT, pCell->ucBgColor, eTTX_MIX_20_PERCENT));
						*pXAdvance =m_font_width = fontwidth;
					}
					//else
					//	*pXAdvance =m_font_width;
					if ( (s_pstGC->pCtrl->state & eTTX_STATE_EBU_SUBTITLE) && (s_pstGC->pCtrl->state & eTTX_STATE_VAR_FONT))
					{
//						textflag = DSTF_TOPLEFT;
					}

                    color = local_ttxgfx_GetColor(ucFgCLUT, pCell->ucFgColor, 0xFF);
                    color = ((color & 0xF000) * 0x11000)   | ((color & 0x0F00) * 0x01100)   | ((color & 0x00F0) * 0x00110)    | ((color & 0x000F) * 0x00011);

                    ONDK_FONT_DrawOneCharacter(m_hsurface, pFont, sz300468Str,  x,  y, color,&m_font_width);
                   //ONDK_FONT_DrawString(m_hsurface,pFont,s_sz300468Str,x ,y  ,local_ttxgfx_GetColor(ucFgCLUT, pCell->ucFgColor, 0xFF));
				}


			}
		}
		return err;
	}
	// Draw TTX own bitmap Char
	if (err != ERR_OK)
	{
		err = local_ttxgfx_RenderingPTU(pCell, x, y, ucFgCLUT, ucBgCLUT, ucAlpha);
	}
	return err;
}



/**
 * Cell을 Rendering
 *
 * @param	s_pstGC		ttx graphics context
 * @param	pCell	cell
 * @param	row		row
 * @param	col		column
 * @param	x		x 좌표
 * @param	y		y 좌표
 * @param	pXAdvance	x좌표 방향으로 그려진 폭
 * @return	ERR_OK / ERR_FAIL
 */
static HERROR	local_ttxgfx_RenderingCell (const TTX_Cell_t *pCell,
											const HINT32 row,const HINT32 col,
											const HINT32 x,const HINT32 y,
											HINT32 *pXAdvance)
{
	HERROR				err;
	HUINT8				ucFgCLUT;
	HUINT8				ucBgCLUT;
	ONDKFontHandle		pFont;
	TTX_Controller_t 	*pCtrl = s_pstGC->pCtrl;
	TTX_State_t 		state	= pCtrl->state;
	ONDK_Color_t				color;
	local_ttxgfx_GetCLUT(row, col, &ucFgCLUT, &ucBgCLUT);
	/////////////////////////////////////////////////////////////////
	// STEP 1. SKIP?
	/////////////////////////////////////////////////////////////////

	if (local_ttxgfx_Skip(pCell, row, col))
	{
		return ERR_FAIL;
	}
	pFont	= local_ttxgfx_GetFont(pCell, row);
	*pXAdvance	= m_font_width ;

	// 만약 FLASH, REVEAL 또는 같은 Color이면 Foreground를 그리지 않는다.
	if ( ( (state & eTTX_STATE_FLASH)	&& pCell->bFlash == TRUE)
	  || (!(state & eTTX_STATE_REVEAL)	&& pCell->bConceal == TRUE)
	  || ( ucFgCLUT == ucBgCLUT && pCell->ucBgColor == pCell->ucFgColor))
	{
		color= local_ttxgfx_GetColor(ucBgCLUT, pCell->ucBgColor, (HUINT8)pCtrl->mix);
		ONDK_DRAW_FillRect16(m_hsurface,ONDK_Rect(x,y,m_font_width,m_font_height),color);
		local_ttxgfx_UpdateArea(x, y, m_font_width ,m_font_height );

		err = ERR_OK; // Do not rendering foreground
	}
	else
	{
		local_ttxgfx_UpdateArea(x, y, m_font_width , m_font_height );
		err = local_ttxgfx_RenderingChar(pCell, pFont,x, y, ucFgCLUT, ucBgCLUT, (HUINT8)pCtrl->mix,pXAdvance);
	}
	//	Font한번 만들고 재활용하는 구조 같은데... 왜 이렇게 된것인지...
#if	0
    if(pvariablefont != NULL)
	    ONDK_FONT_Release(pvariablefont);
	pvariablefont = NULL;
#endif
	return err;
}


/**
 * 한 Line을 Rendering
 *
 * @param	s_pstGC		ttx graphics context
 * @param	pCtrl	ttx controller
 * @param	aLine	line
 * @param	row		row
 * @param	y		y 좌표
 * @return	ERR_OK / ERR_FAIL
 */
static HERROR	local_ttxgfx_RenderingLine (TTX_Controller_t *pCtrl,const TTX_Cell_t *aLine,const HINT32 row,const HINT32 y)
{
	HERROR		err;
	HINT32		x;
	HINT32		advance;
	HINT32		col;

	x	= pCtrl->nMarginW;
	for (col = 0 ; col < TTX_MAX_COLUMN ; col++)
	{
		err	= local_ttxgfx_RenderingCell(&aLine[col], row, col, x, y, &advance);
		if (err != ERR_OK)
		{
			advance	= s_pstGC->nCellWidth;
		}
		if (err == ERR_OK && aLine[col].bDoubleWidth)	// Double Width 속성이면 다음 Cell Skip
		{
			col++;
		}
		x	+=	advance;
	}
	return ERR_OK;
}




/**
 * TTX Page를 Rendering
 *
 * @param	s_pstGC		ttx graphics context
 * @param	pCtrl	ttx controller
 * @return	ERR_OK / ERR_FAIL
 */
static HERROR	local_ttxgfx_Rendering	(TTX_Controller_t *pCtrl)
{
//	HERROR		err;
	HINT32		row;
	HINT32		x, y;
	TTX_Page_t	*pPage	= s_pstGC->pPage;

	s_pstGC->nCellWidth		= pCtrl->nCellWidth;
	s_pstGC->nCellHeight	= pCtrl->nCellHeight;
	if (pCtrl->doubleSize != eTTX_DOUBLE_OFF)
		s_pstGC->nCellHeight *= 2;

	(void) local_ttxgfx_Clear(pCtrl);
	(void) local_ttxgfx_GetCoord(0, 0, &x, &y);

#if 0
	//2 Draw Row 0~23
	err = local_ttxgfx_SetClip(s_pstGC);
	if (err != ERR_OK)
	{
		return ERR_FAIL;
	}
#endif
	for (row = 0 ; row <=23 ; row++)
	{
		local_ttxgfx_RenderingLine(pCtrl, pPage->aCellMap[row], row, y);
		y	+= s_pstGC->nCellHeight;
	}
	//2 Draw Row 24
	(void) local_ttxgfx_GetCoord( 24, 0, &x, &y);

//	local_ttxgfx_UnsetClip(s_pstGC);

	local_ttxgfx_RenderingLine( pCtrl, pPage->aCellMap[24], 24, y);

	(void) local_ttxgfx_GetCoord( 25, 0, &x, &y);
	//2 Draw Subcode Bar (Row 25)
	local_ttxgfx_RenderingLine( pCtrl, s_pstGC->aSubcodeBar, 25, y);

	// TODO: Presentation Level 2.5
	//2 Draw Object
	//2 Draw DRCS??
	return ERR_OK;
}


/**
 * TTX Graphics Context Constructor
 *
 * @param	s_pstGC		ttx graphics context
 * @return	ERR_OK / ERR_FAIL
 */
static HERROR	local_ttxgfx_Constructor (void)
{
#if 0
	if (local_ttxgfx_InitGCInterface() != ERR_OK)
	{
		return ERR_FAIL;
	}
#endif
//	ONDK_Rect_t		src_rect, dst_rect;


	s_pstGC->Screen.area.x = 0;
	s_pstGC->Screen.area.y = 0;
	s_pstGC->Screen.area.w = TELETEXT_SCREENWIDTH  - 1;
	s_pstGC->Screen.area.h = TELETEXT_SCREENHEIGHT - 1;

	s_pstGC->Screen.rect.x	= s_pstGC->Screen.area.x;
	s_pstGC->Screen.rect.y	= s_pstGC->Screen.area.y;
	s_pstGC->Screen.rect.w	= s_pstGC->Screen.area.w - s_pstGC->Screen.area.x + 1;
	s_pstGC->Screen.rect.h	= s_pstGC->Screen.area.h - s_pstGC->Screen.area.y + 1;

//	dst_rect.x = src_rect.x = 0;
//	dst_rect.y = src_rect.y = 0;
//	dst_rect.w = src_rect.w = TELETEXT_SCREENWIDTH;
//	dst_rect.h = src_rect.h = TELETEXT_SCREENHEIGHT;

	return ERR_OK;
}


/**
 * TTX Graphics Context Destructor
 *
 * @param	s_pstGC		ttx graphics context
 */
static void		local_ttxgfx_Destructor (void)
{
	if(g_hTeletextWinHandle)
	{
		ONDK_WINDOW_Clear(g_hTeletextWinHandle);
		ONDK_WINDOW_Update(g_hTeletextWinHandle, NULL);
	}
	//ONDK_FONT_Release(gfont); gfont= NULL;
	if(m_hsurface!=NULL)
	{
		ONDK_SURFACE_Destroy(m_hsurface);
	}
	AP_Free(s_pstGC);

	m_hsurface= NULL;
}

/*******************************************************************/
/********************      API Functions   		*************************/
/*******************************************************************/
#define ___API_FUNCTIONS_____________

HERROR TTX_GFX_Create (ONDKWindowHandle hwindow, Handle_t hDrcs)
{
	HERROR   err;
	if (hDrcs == (Handle_t)NULL || hDrcs == HANDLE_NULL || hwindow ==NULL)
	{
		return HANDLE_NULL;
	}

	if(m_hsurface!=NULL)
	{
		ONDK_SURFACE_Destroy(m_hsurface);
		if(s_pstGC)
			AP_Free(s_pstGC);
		s_pstGC = NULL;
	}
	m_hsurface = ONDK_SURFACE_Create(TELETEXT_SCREENWIDTH, TELETEXT_SCREENHEIGHT, "ARGB4444");
	s_pstGC	= (TTX_GC_t *)AP_Calloc (sizeof(TTX_GC_t));
	if (s_pstGC == NULL)
	{
		return HANDLE_NULL;
	}
	s_pstGC->hDrcs		= hDrcs;

	err	= local_ttxgfx_Constructor();
	if (err != ERR_OK)
	{
		AP_Free(s_pstGC);
		return HANDLE_NULL;
	}
	return ERR_OK;
}


void TTX_GFX_Destroy (void)
{
	if (s_pstGC == NULL)
	{
		return;
	}
	local_ttxgfx_Destructor();
}



TTX_Result_t TTX_GFX_RenderingPage (TTX_Page_t *pPage, va_list va)
{
	TTX_Controller_t	*pCtrl	= va_arg(va, TTX_Controller_t *);
//	ONDK_Rect_t		src_rect, dst_rect;

	if (s_pstGC == NULL || pPage == NULL || pCtrl == NULL)
	{
		return ERR_TTX_INVALID_ARGUMENT;
	}
#if 1
//	src_rect.x = 0;
//	src_rect.y = 0;
//	src_rect.w = TELETEXT_SCREENWIDTH;
//	src_rect.h = TELETEXT_SCREENHEIGHT;

//	dst_rect.x = 0;
//	dst_rect.y = 0;
//	dst_rect.w = GWM_SCREEN_WIDTH;
//	dst_rect.h = GWM_SCREEN_HEIGHT;

#else
	dst_rect.x = src_rect.x = s_pstGC->updateArea.x;
	dst_rect.y = src_rect.y = s_pstGC->updateArea.y;
	dst_rect.w = src_rect.w = s_pstGC->updateArea.w;
	dst_rect.h = src_rect.h = s_pstGC->updateArea.h;
#endif
	s_pstGC->pPage	= pPage;
	s_pstGC->pCtrl	= pCtrl;
	s_pstGC->bUpdated	= FALSE;

	if (local_ttxgfx_Prepare() != ERR_OK) //페이지 준비
		return ERR_TTX_FAIL;

	local_ttxgfx_Rendering(pCtrl); //실제 렌더링 포인트

	s_pstGC->Version.ulTOP	= pPage->TOP.ulVersion;
	s_pstGC->Version.ulCTT	= pPage->TRACE.ulVersion;
	s_pstGC->Version.ulSubcodeBar	= pPage->SubcodeBar.ulVersion;
	s_pstGC->ulLastSec		= pCtrl->ulElapsedSec;

	//HxLOG_Print("[%s:%d]x[%d]y[%d]w[%d]h[%d]\n",__FUNCTION__,__LINE__,s_pstGC->updateArea.x,s_pstGC->updateArea.y,s_pstGC->updateArea.w,s_pstGC->updateArea.h);

	if (s_pstGC->bUpdated && g_hTeletextWinHandle)
	{
		//HxLOG_Print("[%s:%d]\n",__FUNCTION__,__LINE__);
		//Teletext_GetMainSurface(g_hTeletextWinHandle,m_hsurface);
		//ONDK_WINDOW_StretchBlit(g_hTeletextWinHandle,m_hsurface,src_rect,dst_rect);
//		ONDK_WINDOW_UpdateFlipOnSync(g_hTeletextWinHandle,  NULL);
      //   ONDK_GWM_RECV_PostMsgToGWM(MSG_GWM_INVALIDATE_RECT, m_hsurface, 0, 0, 0);
		ONDK_WINDOW_StretchBlit(g_hTeletextWinHandle,m_hsurface,
		                     ONDK_Rect(0,0,TELETEXT_SCREENWIDTH,TELETEXT_SCREENHEIGHT),
		                     ONDK_Rect(0,0,GWM_SCREEN_WIDTH,GWM_SCREEN_HEIGHT));
		ONDK_WINDOW_Update(g_hTeletextWinHandle,  NULL);

#if 0
#if 0
		HxLOG_Print("REFRESH: %08X"
			" / "
			"UPDATE RECT: [%d,%d,%d,%d]"
			" / "
			"PIXEL(^2): %d\n",
			pCtrl->refresh,
			s_pstGC->updateArea.x1, s_pstGC->updateArea.y1,
			s_pstGC->updateArea.x2 - s_pstGC->updateArea.x1 + 1, s_pstGC->updateArea.y2 - s_pstGC->updateArea.y1 + 1,
			(s_pstGC->updateArea.x2 - s_pstGC->updateArea.x1 + 1) * (s_pstGC->updateArea.y2 - s_pstGC->updateArea.y1 + 1)
		);
#endif
		if (s_pstGC->pFB->type != eOSDScreen_MainOsd)
		{
			ONDK_GWM_GFX_UpdateScreen(s_pstGC->pFB, &s_pstGC->updateArea);
#if defined(CONFIG_MWC_OSD_SCALABLE_PLANE)
			OSD_GFX_SetOSDPlaneInOutRect(s_pstGC->pFB, NULL, NULL);
#endif
		}
#endif
	}
	//HxLOG_Print("[%s:%d]\n",__FUNCTION__,__LINE__);

	return ERR_TTX_OK;
}





