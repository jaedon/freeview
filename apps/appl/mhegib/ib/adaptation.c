/**
	@file     adaptation.c
	@brief    Compile, Link를 위한 adaptation code

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#if !defined(WIN32)
#include <stdbool.h>
#include <stdint.h>
#endif
#include <errno.h>
#include <sawman.h>
#include <ondk.h>
#include <hapi.h>
#include <hlib.h>

#include "utils.h" // "adaptation.h" 포함하고 있음.
#if 0
// OCTO app layer headers
#include <ap_actionmanager.h>
#include <ap_watchtv.h>
#include <mw_si.h>
#include <db_svc.h>
#endif

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/



/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#define	PRINT_TODO		(verbose_todo_porting("%s not yet ported", __FUNCTION__))

/* LITTLE ENDIAN System */
#define IB_GET_LONG_VALUE(q)	(int)(((*(unsigned char *)(q)+0) & 0x000000FF) \
								| (*((unsigned char *)(q)+1) << 8) \
								| (*((unsigned char *)(q)+2) << 16) \
								| ((*((unsigned char *)(q) + 3) << 24)))


#if 0
#define MHEG_IB_SCREEN_X					1101
#define MHEG_IB_SCREEN_Y					38
#define MHEG_IB_SCREEN_WIDTH				100
#define MHEG_IB_SCREEN_HEIGHT				486
#endif

#if 1
#define MHEG_IB_WINDOW_SCREEN_X					0
#define MHEG_IB_WINDOW_SCREEN_Y					0
#define MHEG_IB_WINDOW_SCREEN_WIDTH			1280
#define MHEG_IB_WINDOW_SCREEN_HEIGHT			720

#define MHEG_IB_SCREEN_X					0
#define MHEG_IB_SCREEN_Y					0
#define MHEG_IB_SCREEN_WIDTH				1280
#define MHEG_IB_SCREEN_HEIGHT				720

#define MHEG_IB_SUR_SCREEN_WIDTH			720
#define MHEG_IB_SUR_SCREEN_HEIGHT			576

#endif

#define MHEGIB_FIXEDFONT_PATH "/usr/fonts/DejaVuSans.ttf"

/*
 * Justification
 */
typedef enum
{
	Ad_Justification_start = 1,
	Ad_Justification_end = 2,
	Ad_Justification_centre = 3,
	Ad_Justification_justified = 4,
} Ad_Justification;

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static ONDKWindowHandle		s_hMhegIbWindow = NULL;
static ONDKSurfaceHandle	s_hMhegIbSurface = NULL;
static ONDKFontHandle		s_font = NULL;
static HUINT32				s_fontwidth =0;
static HUINT32				m_KeyId;

/********************************************************************
* Extern Variables / Function Prototypes
********************************************************************/
extern HBOOL MHEGIB_MAIN_CheckEngineStartCondition(void);


/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

static HERROR local_MHEGIB_CreateFont(int fontSize)
{
	HINT32 nFontHeight = 0;

	if(fontSize == 0)
	{
		HxLOG_Error("Invaild font size is %d\n", fontSize);
		return ERR_FAIL;
	}

	if(s_fontwidth == fontSize)
	{
		HxLOG_Print("[local_MHEGIB_CreateFont] same font size\n");
		return ERR_OK;
	}

	HxLOG_Print("[local_MHEGIB_CreateFont] font size = %d\n", fontSize);

	if(s_font != NULL)
	{
		ONDK_FONT_Release(s_font);
		s_font = NULL;
	}

	nFontHeight = (fontSize * 7) / 10;/* 일단 대충 가로 세로 비율을 7 : 10 정도로... */
	if(nFontHeight <= 0)
	{
		nFontHeight = fontSize;
	}

	s_font = ONDK_FONT_Create(NULL, nFontHeight, fontSize);
	if(s_font == NULL)
	{
		HxLOG_Print("[local_MHEGIB_CreateFont] font create failed\n");
		return ERR_FAIL;
	}

	s_fontwidth = fontSize;

	return ERR_OK;
}

#define	_____________LINUX_WARPPING_FOR_WIN______________

#if defined(WIN32)

int gettimeofday(struct timeval *tv, struct timezone *tz)
{
	unsigned long ulTick = 0;

	PRINT_TODO;

	if ( (tv == NULL) || (tz == NULL) )
		return -1;

	// set time
	ulTick = VK_TIMER_GetSystemTick();
	tv->tv_sec = (unsigned int) ulTick / 1000;
	tv->tv_usec = (unsigned int) ulTick % 1000;

	// set zone - not implement
	tz->tz_minuteswest = 0;
	tz->tz_dsttime = 0;

	return 1;
}

uint16_t htons(uint16_t hostnum)
{
	PRINT_TODO;
	return 0;
}

uint32_t htonl(uint32_t hostnum)
{
	PRINT_TODO;
	return 0;
}

#endif /* #if defined(WIN32) */

#define	_____________NOT_IMPLEMENTATION______________

// Dynamic line art 관련 미구현 함수들.. 원래는 Xwindow용 함수임.
// CI+ Profile에서는 사용하지 않으므로 proto type만 선언해 둠.

Pixmap MHEGIB_CreatePixmap(
    Drawable             d ,
    unsigned int         width ,
    unsigned int         height ,
    unsigned int         depth
)
{
	static Pixmap tmp;
	PRINT_TODO;
	return tmp;
}

Picture
MHEGIB_RenderCreatePicture (Drawable d)
{
	static Picture tmp;
	PRINT_TODO;
	return tmp;
}

GC MHEGIB_CreateGC(
    Drawable             d ,
    unsigned long        valuemask ,
    XGCValues*           values
)
{
	static GC tmp;
	PRINT_TODO;
	return tmp;
}

int MHEGIB_FreeGC(
    GC                   gc
)
{
	PRINT_TODO;
	return 0;
}

int MHEGIB_ChangeGC(
    GC                   gc ,
    unsigned long        valuemask ,
    XGCValues*           values
)
{
	PRINT_TODO;
	return 0;
}

int MHEGIB_FillRectangle( /* MHEGIB_DrawRectangle(), MHEGIB_DrawRectFill() 와 다른 함수임에 유의할 것 */
    Drawable             d ,
    GC                   gc ,
    int                  x ,
    int                  y ,
    unsigned int         width ,
    unsigned int         height
)
{
	PRINT_TODO;
	return 0;
}

int MHEGIB_DrawRectangle( /* MHEGIB_FillRectangle(), MHEGIB_DrawRectangle() 와 다른 함수임에 유의할 것 */
    Drawable             d ,
    GC                   gc ,
    int                  x ,
    int                  y ,
    unsigned int         width ,
    unsigned int         height
)
{
	PRINT_TODO;
	return 0;
}

int MHEGIB_SetClipRectangles(
    GC                   gc ,
    int                  clip_x_origin ,
    int                  clip_y_origin ,
    XRectangle*          rectangles ,
    int                  n ,
    int                  ordering
)
{
	PRINT_TODO;
	return 0;
}

int MHEGIB_FillArc(
    Drawable             d ,
    GC                   gc ,
    int                  x ,
    int                  y ,
    unsigned int         width ,
    unsigned int         height ,
    int                  angle1 ,
    int                  angle2
)
{
	PRINT_TODO;
	return 0;
}

int MHEGIB_FillPolygon(
    Drawable             d ,
    GC                   gc ,
    XPoint*              points ,
    int                  npoints ,
    int                  shape ,
    int                  mode
)
{
	PRINT_TODO;
	return 0;
}

int MHEGIB_DrawArc(
    Drawable             d ,
    GC                   gc ,
    int                  x ,
    int                  y ,
    unsigned int         width ,
    unsigned int         height ,
    int                  angle1 ,
    int                  angle2
)
{
	PRINT_TODO;
	return 0;
}

int MHEGIB_DrawLine(
    Drawable             d ,
    GC                   gc ,
    int                  x1 ,
    int                  y1 ,
    int                  x2 ,
    int                  y2
)
{
	PRINT_TODO;
	return 0;
}

int MHEGIB_DrawLines(
    Drawable             d ,
    GC                   gc ,
    XPoint*              points ,
    int                  npoints ,
    int                  mode
)
{
	PRINT_TODO;
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
#define	_____________OCTO_UI______________

HERROR		MHEGIB_OSD_GetColor(int *out, unsigned char in_r, unsigned char in_g, unsigned char in_b, unsigned char in_t)
{
	if (out == NULL)
	{
		error("invalid params");
		return ERR_FAIL;
	}

	*out = ((0xff - in_t) << 24) | (in_r << 16) | (in_g << 8) | (in_b);

	return ERR_OK;
}

HERROR		MHEGIB_UpdateScreen(int x, int y, int w, int h)
{
	ONDK_Rect_t   rc;
	ONDK_Rect_t	  rc2;

	HxLOG_Print("[MHEGIB_UpdateScreen] x = %d, y = %d, w= %d, h = %d\n", x, y, w, h);
	rc = ONDK_Rect( x, y, w, h);
	rc2 = ONDK_Rect( MHEG_IB_WINDOW_SCREEN_X, MHEG_IB_WINDOW_SCREEN_Y, MHEG_IB_WINDOW_SCREEN_WIDTH, MHEG_IB_WINDOW_SCREEN_HEIGHT);
	ONDK_WINDOW_StretchBlit(s_hMhegIbWindow, s_hMhegIbSurface, rc, rc2);
	ONDK_WINDOW_Update(s_hMhegIbWindow, &rc2);

	return ERR_OK;
}

HERROR	MHEGIB_DrawString(int orgX, int orgY, int color, int fontSize, bool tabs, int tabWidth, unsigned char *orgData, unsigned int orgSize, unsigned int hj, unsigned int vj)
{
	int 					numOfBytes;
	unsigned short		ucs2[512] = {0,};
	unsigned char			utf8[512] = {0,};
	HUINT8				auctxt[512] = {0,};
	ONDKSurfaceTextFlags	textflag = ONDKSTF_TOPLEFT;

	HxSTD_MemSet(ucs2, 0, 512);
	HxSTD_MemSet(utf8, 0, 512);
	HxSTD_MemSet(auctxt, 0, 512);

	HxSTD_MemCopy(auctxt, orgData, orgSize);

	HxLOG_Print("[MHEGIB_DrawString] %s, orgX = %d, orgY = %d, color = 0x%x\n", auctxt, orgX, orgY, color);

	if((local_MHEGIB_CreateFont(fontSize) != ERR_OK) && (s_font == NULL))
	{
		HxLOG_Error("[local_MHEGIB_CreateFont] Error\n");
		return ERR_FAIL;
	}

#if 0
 	if(hj == Ad_Justification_end) textflag = DSTF_RIGHT;
	else if(hj == Ad_Justification_centre) textflag = DSTF_CENTER;
	else textflag = DSTF_LEFT;

	if(vj == Ad_Justification_end) textflag |= DSTF_BOTTOM;
	else textflag |= DSTF_TOP;
#endif

	HLIB_CP_ConvertCodepageToUCS2(eHxCP_UNICODE_UTF8, auctxt, ucs2, 512);
	numOfBytes = HLIB_CP_ConvertUCS2ToCodepage (eHxCP_UNICODE_UTF8, (HUINT16 *)ucs2, utf8, 512);
	utf8[numOfBytes] = 0;
	HxLOG_Print("[MHEGIB_DrawString][UTF8] %s\n", utf8);


	ONDK_FONT_DrawStringAlign(s_hMhegIbSurface, s_font, utf8, orgX, orgY, -1, (HUINT32)color, textflag);
	return ERR_OK;
}

HERROR		MHEGIB_DrawRectFill(int x, int y, int w, int h, int color) /* MHEGIB_FillRectangle(), MHEGIB_DrawRectangle() 와 다른 함수임에 유의할 것 */
{
	ONDK_Rect_t rc = ONDK_Rect( x, y, w, h);

	if(w == 0 || h == 0)
	{
		HxLOG_Critical("invalid params, w = %d, h = %d\n", w, h);
		return ERR_FAIL;
	}
	HxLOG_Print("[MHEGIB_DrawRectFill][0x%x] x = %d, y = %d, w = %d, h = %d, color = 0x%x\n", s_hMhegIbSurface, x, y, w, h, color);

	ONDK_DRAW_FillRect(s_hMhegIbSurface, rc, color);
#if 0
#define LINE_WITH 10
	ONDK_DRAW_FillRect(s_hMhegIbSurface, ONDK_Rect( 0, 0, MHEG_IB_SUR_SCREEN_WIDTH, LINE_WITH), 0x8f0700ff);
	ONDK_DRAW_FillRect(s_hMhegIbSurface, ONDK_Rect( MHEG_IB_SUR_SCREEN_WIDTH - LINE_WITH, 0, LINE_WITH, MHEG_IB_SUR_SCREEN_HEIGHT), 0x8f0700ff);
	ONDK_DRAW_FillRect(s_hMhegIbSurface, ONDK_Rect( 0, MHEG_IB_SUR_SCREEN_HEIGHT - LINE_WITH, MHEG_IB_SUR_SCREEN_WIDTH, LINE_WITH), 0x8f0700ff);
	ONDK_DRAW_FillRect(s_hMhegIbSurface, ONDK_Rect( 0, 0, LINE_WITH, MHEG_IB_SUR_SCREEN_HEIGHT), 0x8f0700ff);
#endif
	return ERR_OK;
}


#if defined(CONFIG_DEBUG)
void MHEGIB_CheckSurface(char *name, int line)
{
	Hx_Print("[%s][%p], Called [%s][%d]\n", __FUNCTION__, s_hMhegIbSurface, name, line);
}
#endif


HERROR		MHEGIB_DrawBitmap(int x, int y, int w, int h, unsigned char *pucImg, bool bEnableTiling)
{
	int 			picW, picH;
	int 			curX, curY;
	int 			clipW, clipH;

	if (pucImg == NULL)
	{
		error("invalid params");
		return ERR_FAIL;
	}

#if 0
	{
		int i = 0;

		for(i = 1; i <= 48; i++)
		{
			printf("[%02X]", pucImg[i-1]);
			if((i%16) == 0)
			{
				printf("\n");
			}
		}
	}
#endif

	// check tiling
	picW = IB_GET_LONG_VALUE(pucImg);
	picH = IB_GET_LONG_VALUE(pucImg + 4);

	HxLOG_Print("[MHEGIB_DrawBitmap] x = %d, y = %d, w = %d, h = %d, bEnableTiling = %d, img_w = %d, img_h = %d\n", x, y, w, h, bEnableTiling, picW, picH);

	if ( (picW < 0) || (picH < 0))
	{
		error("Invalid image size w(%d) h(%d)", picW, picH);
		return ERR_FAIL;
	}

	if ( (bEnableTiling != FALSE)			/* tiling flag가 enable 이고 */
		&& ((picW < w) || (picH < h)) ) /* 실제 tiling이 가능한 picture size인 경우에만  tiling */
	{
		// bitmap tiling
		curY = y;
		while (curY < y+h)
		{
			curX = x;
			while (curX < x+w)
			{
				// 마지막 부분.. 원래 box의 w나 h를 넘치는 부분은 clipping 처리한다
				clipW = (curX + picW > x + w) ? (x+w)-curX : picW;
				clipH = (curY + picH > y + h) ? (y+h)-curY : picH;

				// TODO : 이미지가 transparency 를 가지고 있는 경우엔 그 밑의 이미지와 비쳐 보여지도록 처리되어야 함.
				//		  if 코드를 적용하면 현재로선 비디오 plane 까지 비쳐 보여지게 되어 화면이 이상하게 보여지므로
				//		  else 코드를 일단 임시 적용함. 단, DTV Labs 인증시에는 else 코드를 사용하면 안됨!!! 근본적으로 수정할 필요 있음.
				ONDK_DRAW_Bitmap(s_hMhegIbSurface, curX, curY, clipW, clipH, &pucImg[8]);

				curX += picW;
			}
			curY += picH;
		}
	}
	else
	{
		// normal case
		if (picW > w)
		{
			picW = w;
		}
		if (picH > h)
		{
			picH = h;
		}

		// TODO : 이미지가 transparency 를 가지고 있는 경우엔 그 밑의 이미지와 비쳐 보여지도록 처리되어야 함.
		//		  if 코드를 적용하면 현재로선 비디오 plane 까지 비쳐 보여지게 되어 화면이 이상하게 보여지므로
		//		  else 코드를 일단 임시 적용함. 단, DTV Labs 인증시에는 else 코드를 사용하면 안됨!!! 근본적으로 수정할 필요 있음.
		ONDK_DRAW_Bitmap(s_hMhegIbSurface, x, y, picW, picH, &pucImg[8]);
	}

	return ERR_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
#define	_____________OCTO_FONT______________

HERROR			MHEGIB_GetCharWidth(int fontSize, int charData, int *pCharWidth)
{
	unsigned char			szOrg[3] = {0,};
	unsigned char			utf8[3] = {0,};
	int 					numOfBytes;
	HUINT16					unUscChar[3] = {0,};

	if(pCharWidth == NULL)
	{
		HxLOG_Print("[MHEGIB_GetCharWidth] arg error\n");
		return ERR_FAIL;
	}

	if(local_MHEGIB_CreateFont(fontSize) != ERR_OK)
	{
		HxLOG_Error("[local_MHEGIB_CreateFont] Error\n");
	}

	szOrg[0] = charData;
	szOrg[1] = '\0';

	unUscChar[0] = (HUINT16)charData;
	if(s_font != NULL)
	{
		//HLIB_CP_ConvertCodepageToUCS2(eHxCP_UNICODE_UTF8, szOrg, unUscChar, 2);
		numOfBytes = HLIB_CP_ConvertUCS2ToCodepage (eHxCP_UNICODE_UTF8, unUscChar, utf8, 2);
	    *pCharWidth =ONDK_FONT_GetOneCharacterWidth(s_font,utf8, NULL);
	}
	else
	{
		*pCharWidth = 0;
	}

    return ERR_OK;
}

HERROR			MHEGIB_GetFontHeight(int fontSize, unsigned int *pFontHeight)
{
	if(pFontHeight == NULL)
	{
		HxLOG_Print("[MHEGIB_GetFontHeight] arg error\n");
		return ERR_FAIL;
	}

	if(local_MHEGIB_CreateFont(fontSize) != ERR_OK)
	{
		HxLOG_Error("[local_MHEGIB_CreateFont] Error\n");
	}

	if(s_font != NULL)
	{
	*pFontHeight = ONDK_FONT_GetHeight(s_font);
	}
	else
	{
		*pFontHeight = 0;
	}

	return ERR_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
#define	_____________OCTO_AV______________

HERROR		MHEGIB_PlayIFrame(int x, int y, int w, int h, unsigned char *pucBuf, int lBufSize)
{
	HxLOG_Print("[MHEGIB_PlayIFrame] \n");
	#if 0
	{
		Handle_t hCurAction;

		if ( (pucBuf == NULL) || (lBufSize < 1) )
		{
			error("invalid params");
			return ERR_FAIL;
		}

		hCurAction = MWC_RM_GetActionHandle(AP_AM_GetMainActionId() /* eActionId_VIEW_0 */);
		if (hCurAction == ERR_FAIL)
		{
			error("MWC_RM_GetActionHandle() fail");
			return ERR_FAIL;
		}

		if (MW_AV_PlayIFrame(hCurAction, pucBuf, lBufSize, eVIDEO_Codec_MPEG2) != ERR_OK)
		{
			error("MW_AV_PlayIFrame() fail : curAction(0x%X) bufSize(%d)", hCurAction, lBufSize);
			return ERR_FAIL;
		}

		// TODO: Howto scaling...?

		return ERR_OK;
	}
	#endif

	return ERR_OK;
}

HERROR		MHEGIB_StopIFrame(void)
{
	HxLOG_Print("[MHEGIB_StopIFrame] \n");
	#if 0
	{
		Handle_t hCurAction;

		hCurAction = MWC_RM_GetActionHandle(AP_AM_GetMainActionId() /* eActionId_VIEW_0 */);
		if (hCurAction == ERR_FAIL)
		{
			error("MWC_RM_GetActionHandle() fail");
			//return ERR_FAIL;
		}

		MW_AV_StopIFrame(hCurAction);

		return ERR_OK;
	}
	#endif

	return ERR_OK;
}

// ref : PORT_ITK_Display_SetVolume()
HERROR		MHEGIB_Audio_SetVolume(HINT32 lSetVol, HBOOL bIsAbsoluteValue)
{
	HxLOG_Print("[MHEGIB_Audio_SetVolume] \n");
	//MWC_UTIL_PostMsgToAp(MSG_MHEGIB_AUDIO_SET_VOLUME, HANDLE_NULL, lSetVol, (HINT32)bIsAbsoluteValue, 0);
	return ERR_OK;
}

HERROR		MHEGIB_Video_Freeze(void)
{
	HxLOG_Print("[MHEGIB_Video_Freeze] \n");
	// video freeze
	//MWC_UTIL_PostMsgToAp(MSG_MHEGIB_STREAM_SET_SPEED, HANDLE_NULL, (HINT32)TRUE, 0, 0);
	return ERR_OK;
}

HERROR		MHEGIB_Video_Resume(void)
{
	HxLOG_Print("[MHEGIB_Video_Resume] \n");
	// video resume
	//MWC_UTIL_PostMsgToAp(MSG_MHEGIB_STREAM_SET_SPEED, HANDLE_NULL, (HINT32)FALSE, 0, 0);
	return ERR_OK;
}

int 			MHEGIB_GetDisplayAspectRatio(void)
{
	HxLOG_Print("[MHEGIB_GetDisplayAspectRatio] \n");
	#if 0
	{
		// ref. D-Book5 : 13.10.8.3 GetDisplayAspectRatio
	 	// return value 1 = 4:3, 2=16:9
		HERROR					hErr;
		int						lRetValue = 0;
		VIDEO_AspectRatio_t	nValue; // VIDEO_AspectRatio_t

		hErr = OxSVC_OUT_GetTvAspectRatio(eVideoOutputPort_HDMI, &nValue);
		if (ERR_OK == hErr)
		{
			switch (nValue)
			{
				case eAspectRatio_4_3:
					lRetValue = 1;
					break;
				case eAspectRatio_16_9:
				default:
					// ??? Howto ??? - 확인전에는 일단 16x9로 return 하자
					lRetValue = 2;
					break;
			}
		}
		else
		{
			lRetValue = 2;
		}

		return lRetValue;
	}
	#endif
	return 2;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
#define	_____________OCTO_SI______________

HBOOL			MHEGIB_GetCurTripleID(int *plOnID, int *plTsID, int *plSID)
{
	HxLOG_Print("[MHEGIB_GetCurTripleID] \n");
	#if 0
	{
		HUINT32		ulActionId;
		SVC_Info_t 	MasterSvcInfo;

		if ((plOnID == NULL) || (plTsID == NULL) || (plSID == NULL))
		{
			error("invalid parameter");
			return FALSE;
		}

		ulActionId = AP_AM_GetMainActionId();
		if (AP_WTV_GetMasterSvcInfo(ulActionId, &MasterSvcInfo) != ERR_OK)
		{
			error("AP_WTV_GetMasterSvcInfo() fail");
			return FALSE;
		}

		*plOnID = (int)MasterSvcInfo.usOnId;
		*plTsID = (int)MasterSvcInfo.usTsId;
		*plSID = (int)MasterSvcInfo.usSvcId;

		HxLOG_Print("[MHEGIB_GetCurTripleID] onid(%d) tsid(%d) sid(%d)\n", *plOnID, *plTsID, *plSID);

		return TRUE;
	}
	#endif

	return TRUE;
}

HBOOL			MHEGIB_GetTripleIDbyLCN(int lcn, int *plOnID, int *plTsID, int *plSID)
{
	HxLOG_Print("[MHEGIB_GetTripleIDbyLCN] \n");
	#if 0
	{
		Handle_t			hRequestedSvc = HANDLE_NULL;
		SVC_Info_t 		stSvcInfo;

		if ((plOnID == NULL) || (plTsID == NULL) || (plSID == NULL))
		{
			error("invalid parameter");
			return FALSE;
		}

		*plOnID = *plTsID = *plSID = 0;

#if defined(CONFIG_MW_CH_SATELLITE)
		if (DB_SVC_GetServiceHandleByNumber(eDxDELIVERY_TYPE_SAT, eSvcType_All, lcn, &hRequestedSvc) != ERR_OK)
		{
	//		error("DB_SVC_GetServiceHandleByNumber() eDxDELIVERY_TYPE_SAT fail");
			hRequestedSvc = HANDLE_NULL;
		}
#endif

#if defined(CONFIG_MW_CH_CABLE)
		if (hRequestedSvc == HANDLE_NULL)
		{
			if (DB_SVC_GetServiceHandleByNumber(eDxDELIVERY_TYPE_CAB, eSvcType_All, lcn, &hRequestedSvc) != ERR_OK)
			{
	//			error("DB_SVC_GetServiceHandleByNumber() eDxDELIVERY_TYPE_CAB fail");
				hRequestedSvc = HANDLE_NULL;
			}
		}
#endif

#if defined(CONFIG_MW_CH_TERRESTRIAL)
		if (hRequestedSvc == HANDLE_NULL)
		{
			if (DB_SVC_GetServiceHandleByNumber(eDxDELIVERY_TYPE_TER, eSvcType_All, lcn, &hRequestedSvc) != ERR_OK)
			{
	//			error("DB_SVC_GetServiceHandleByNumber() eDxDELIVERY_TYPE_TER fail");
				hRequestedSvc = HANDLE_NULL;
			}
		}
#endif

		if (hRequestedSvc == HANDLE_NULL)
		{
			error("DB_SVC_GetServiceHandleByNumber() fail");
			return FALSE;
		}

		if (DB_SVC_GetServiceInfo(hRequestedSvc, &stSvcInfo) != ERR_OK)
		{
			error("DB_SVC_GetServiceInfo() fail");
			return FALSE;
		}

		if (stSvcInfo.bLcnFlag != TRUE)
		{
			HxLOG_Print("[MHEGIB_GetTripleIDbyLCN] No LCN !!\n");
			return FALSE;
		}

		*plOnID = (int)stSvcInfo.usOnId;
		*plTsID = (int)stSvcInfo.usTsId;
		*plSID = (int)stSvcInfo.usSvcId;

		HxLOG_Print("[MHEGIB_GetTripleIDbyLCN] onid(%d) tsid(%d) sid(%d)\n", *plOnID, *plTsID, *plSID);

		return TRUE;
	}
	#endif

	return ERR_OK;
}

HBOOL			MHEGIB_IsServiceAvailableByTripleID(unsigned int onid, unsigned int tsid, unsigned int sid)
{
	HxLOG_Print("[MHEGIB_IsServiceAvailableByTripleID] \n");
	#if 0
	{
		HERROR			hError = ERR_FAIL;
		SVC_TripleId_t		tripleId;
		Handle_t			hRequestedSvc = HANDLE_NULL;

		tripleId.usOnId = (HUINT16)onid;
		tripleId.usTsId = (HUINT16)tsid;
		tripleId.usSvcId = (HUINT16)sid;

#if defined(CONFIG_MW_CH_SATELLITE)
		hError = DB_SVC_FindServiceHandleByTripleId(eDxDELIVERY_TYPE_SAT, tripleId, &hRequestedSvc);
		if ( (hError != ERR_OK) || (hRequestedSvc == HANDLE_NULL) )
		{
	//		error("DB_SVC_FindServiceHandleByTripleId() eDxDELIVERY_TYPE_SAT fail : onid(%d) tsid(%d) sid(%d)", tripleId.usOnId, tripleId.usTsId, tripleId.usSvcId);
			hRequestedSvc = HANDLE_NULL;
		}
#endif

#if defined(CONFIG_MW_CH_CABLE)
		if (hRequestedSvc == HANDLE_NULL)
		{
			hError = DB_SVC_FindServiceHandleByTripleId(eDxDELIVERY_TYPE_CAB, tripleId, &hRequestedSvc);
			if ( (hError != ERR_OK) || (hRequestedSvc == HANDLE_NULL) )
			{
	//			error("DB_SVC_FindServiceHandleByTripleId() eDxDELIVERY_TYPE_CAB fail : onid(%d) tsid(%d) sid(%d)", tripleId.usOnId, tripleId.usTsId, tripleId.usSvcId);
				hRequestedSvc = HANDLE_NULL;
			}
		}
#endif

#if defined(CONFIG_MW_CH_TERRESTRIAL)
		if (hRequestedSvc == HANDLE_NULL)
		{
			hError = DB_SVC_FindServiceHandleByTripleId(eDxDELIVERY_TYPE_TER, tripleId, &hRequestedSvc);
			if ( (hError != ERR_OK) || (hRequestedSvc == HANDLE_NULL) )
			{
	//			error("DB_SVC_FindServiceHandleByTripleId() eDxDELIVERY_TYPE_TER fail : onid(%d) tsid(%d) sid(%d)", tripleId.usOnId, tripleId.usTsId, tripleId.usSvcId);
				hRequestedSvc = HANDLE_NULL;
			}
		}
#endif

		if (hRequestedSvc == HANDLE_NULL)
		{
			error("DB_SVC_FindServiceHandleByTripleId() fail : onid(%d) tsid(%d) sid(%d)", tripleId.usOnId, tripleId.usTsId, tripleId.usSvcId);
			return FALSE;
		}

		return TRUE;
	}
	#endif

	return TRUE;
}

HBOOL			MHEGIB_GetNetIdByTripleID(unsigned int onid, unsigned int tsid, unsigned int sid, unsigned int *pNid)
{
	HxLOG_Print("[MHEGIB_GetNetIdByTripleID] \n");
	#if 0
	{
		HERROR			hError = ERR_FAIL;
		SVC_TripleId_t		tripleId;
		Handle_t			hRequestedSvc = HANDLE_NULL;
		SVC_Info_t		svcInfo;
		SVC_TsInfo_t		tsInfo;
		SVC_NetInfo_t		netInfo;

		if (pNid == NULL)
		{
			error("invalid parameter");
			return FALSE;
		}
		*pNid = 0;

		tripleId.usOnId = (HUINT16)onid;
		tripleId.usTsId = (HUINT16)tsid;
		tripleId.usSvcId = (HUINT16)sid;

#if defined(CONFIG_MW_CH_SATELLITE)
		hError = DB_SVC_FindServiceHandleByTripleId(eDxDELIVERY_TYPE_SAT, tripleId, &hRequestedSvc);
		if ( (hError != ERR_OK) || (hRequestedSvc == HANDLE_NULL) )
		{
	//		error("DB_SVC_FindServiceHandleByTripleId() eDxDELIVERY_TYPE_SAT fail : onid(%d) tsid(%d) sid(%d)", tripleId.usOnId, tripleId.usTsId, tripleId.usSvcId);
			hRequestedSvc = HANDLE_NULL;
		}
#endif

#if defined(CONFIG_MW_CH_CABLE)
		if (hRequestedSvc == HANDLE_NULL)
		{
			hError = DB_SVC_FindServiceHandleByTripleId(eDxDELIVERY_TYPE_CAB, tripleId, &hRequestedSvc);
			if ( (hError != ERR_OK) || (hRequestedSvc == HANDLE_NULL) )
			{
	//			error("DB_SVC_FindServiceHandleByTripleId() eDxDELIVERY_TYPE_CAB fail : onid(%d) tsid(%d) sid(%d)", tripleId.usOnId, tripleId.usTsId, tripleId.usSvcId);
				hRequestedSvc = HANDLE_NULL;
			}
		}
#endif

#if defined(CONFIG_MW_CH_TERRESTRIAL)
		if (hRequestedSvc == HANDLE_NULL)
		{
			hError = DB_SVC_FindServiceHandleByTripleId(eDxDELIVERY_TYPE_TER, tripleId, &hRequestedSvc);
			if ( (hError != ERR_OK) || (hRequestedSvc == HANDLE_NULL) )
			{
	//			error("DB_SVC_FindServiceHandleByTripleId() eDxDELIVERY_TYPE_TER fail : onid(%d) tsid(%d) sid(%d)", tripleId.usOnId, tripleId.usTsId, tripleId.usSvcId);
				hRequestedSvc = HANDLE_NULL;
			}
		}
#endif

		if (hRequestedSvc == HANDLE_NULL)
		{
			error("DB_SVC_FindServiceHandleByTripleId() fail : onid(%d) tsid(%d) sid(%d)", tripleId.usOnId, tripleId.usTsId, tripleId.usSvcId);
			return FALSE;
		}

		hError = DB_SVC_GetServiceInfo(hRequestedSvc, &svcInfo);
		if(hError != ERR_OK)
		{
			error("DB_SVC_GetServiceInfo() fail : hRequestedSvc(0x%X)", hRequestedSvc);
			return FALSE;
		}

		hError = DB_SVC_GetTsInfo(svcInfo.usTsIdx, &tsInfo);
		if(hError != ERR_OK)
		{
			error("DB_SVC_GetServiceInfo() fail : hRequestedSvc(0x%X) usTsIdx(0x%X)", hRequestedSvc, svcInfo.usTsIdx);
			return FALSE;
		}

		hError= DB_SVC_GetNetworkInfo(tsInfo.usNetIdx, &netInfo);
		if(hError != ERR_OK)
		{
			error("DB_SVC_GetServiceInfo() fail : hRequestedSvc(0x%X) usNetIdx(0x%X)", hRequestedSvc, tsInfo.usNetIdx);
			return FALSE;
		}

		*pNid = netInfo.usNetId;

		HxLOG_Print("[MHEGIB_GetNetIdByTripleID] onid(%d) tsid(%d) sid(%d) => hSvc(0x%X) => nid(0x%X), usNetIdx(%d)\n", onid, tsid, sid, hRequestedSvc, netInfo.usNetId, tsInfo.usNetIdx);

		return TRUE;
	}
	#endif

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
#define	_____________Key______________

#if 1
static HERROR local_MhegIb_RegisterKey(void)
{
/* register key */
	HAPI_RegisterAcceptableKey(eAWINDOW_MHEGIB, DIKS_CURSOR_LEFT, TRUE);
	HAPI_RegisterAcceptableKey(eAWINDOW_MHEGIB, DIKS_CURSOR_RIGHT, TRUE);
	HAPI_RegisterAcceptableKey(eAWINDOW_MHEGIB, DIKS_CURSOR_UP, TRUE);
	HAPI_RegisterAcceptableKey(eAWINDOW_MHEGIB, DIKS_CURSOR_DOWN, TRUE);

	HAPI_RegisterAcceptableKey(eAWINDOW_MHEGIB, DIKS_RED, TRUE);
	HAPI_RegisterAcceptableKey(eAWINDOW_MHEGIB, DIKS_GREEN, TRUE);
	HAPI_RegisterAcceptableKey(eAWINDOW_MHEGIB, DIKS_YELLOW, TRUE);
	HAPI_RegisterAcceptableKey(eAWINDOW_MHEGIB, DIKS_BLUE, TRUE);

	HAPI_RegisterAcceptableKey(eAWINDOW_MHEGIB, DIKS_0, TRUE);
	HAPI_RegisterAcceptableKey(eAWINDOW_MHEGIB, DIKS_1, TRUE);
	HAPI_RegisterAcceptableKey(eAWINDOW_MHEGIB, DIKS_2, TRUE);
	HAPI_RegisterAcceptableKey(eAWINDOW_MHEGIB, DIKS_3, TRUE);
	HAPI_RegisterAcceptableKey(eAWINDOW_MHEGIB, DIKS_4, TRUE);
	HAPI_RegisterAcceptableKey(eAWINDOW_MHEGIB, DIKS_5, TRUE);
	HAPI_RegisterAcceptableKey(eAWINDOW_MHEGIB, DIKS_6, TRUE);
	HAPI_RegisterAcceptableKey(eAWINDOW_MHEGIB, DIKS_7, TRUE);
	HAPI_RegisterAcceptableKey(eAWINDOW_MHEGIB, DIKS_8, TRUE);
	HAPI_RegisterAcceptableKey(eAWINDOW_MHEGIB, DIKS_9, TRUE);

	HAPI_RegisterAcceptableKey(eAWINDOW_MHEGIB, DIKS_RETURN, TRUE);
	HAPI_RegisterAcceptableKey(eAWINDOW_MHEGIB, DIKS_BACK, TRUE);
	HAPI_RegisterAcceptableKey(eAWINDOW_MHEGIB, DIKS_LAST, TRUE);
	HAPI_RegisterAcceptableKey(eAWINDOW_MHEGIB, DIKS_EXIT, TRUE);
	HAPI_RegisterAcceptableKey(eAWINDOW_MHEGIB, DIKS_CHANNEL_UP, TRUE);
	HAPI_RegisterAcceptableKey(eAWINDOW_MHEGIB, DIKS_CHANNEL_DOWN, TRUE);

	HAPI_RegisterAcceptableKey(eAWINDOW_MHEGIB, DIKS_TEXT, TRUE);
	HAPI_RegisterAcceptableKey(eAWINDOW_MHEGIB, DIKS_EPG, TRUE);
	return ERR_OK;
}

HERROR local_MhegIb_KeyEvent(HUINT32 ucKeyCode)
{
	HERROR					hErr;

	HxLOG_Print("[%s:%d]key do key Event..[%x]\n", __FUNCTION__, __HxLINE__, ucKeyCode);

	hErr = MHEGIB_SendKey(0, ucKeyCode, 0, 0);

	if(hErr == ERR_OK)
	{
		return ONDK_MESSAGE_CONSUMED;
	}

	return ONDK_MESSAGE_NONCONSUMED;
}

static ONDK_Result_t local_MhegIb_KeyNotifyCallback(ONDK_WINDOW_Position_e ePosition, HUINT32 ucKeyCode, ONDK_KEY_PressMode_t pressMode, ONDK_KEY_Device_t device, void *keyTypeData)
{
    ONDK_Result_t lRet;

    if(pressMode == eKeyPress_UP)
    {
        HxLOG_Print("[%s:%d] (-)\n", __FUNCTION__, __LINE__);
        return ONDK_MESSAGE_NONCONSUMED;
    }

    lRet = local_MhegIb_KeyEvent(ucKeyCode);

    if(lRet == ONDK_MESSAGE_CONSUMED)
    {
      HxLOG_Print("[%s:%d] ONDK_MESSAGE_CONSUMED\n", __FUNCTION__, __LINE__);
      return ONDK_MESSAGE_CONSUMED;
    }

    HxLOG_Print("[%s:%d] ONDK_MESSAGE_NONCONSUMED\n", __FUNCTION__, __LINE__);
    return ONDK_MESSAGE_NONCONSUMED;
}

static HERROR local_MhegIb_InitKeyProc(ONDKWindowHandle hWndHandle)
{
	//int			nErr;
	//HULONG		ulKeyEventThreadId;

	if(local_MhegIb_RegisterKey() != ERR_OK)
	{
		HxLOG_Error("[local_MhegIb_RegisterKey] Err\n");
		return ERR_FAIL;
	}

    m_KeyId = ONDK_KEY_RegisterKeyCallback(hWndHandle, local_MhegIb_KeyNotifyCallback);
#if 0
	nErr = VK_TASK_Create(local_MhegIb_KeyProcess_Task, 40, 0x4000, "MhegIbKey_T", NULL, &ulKeyEventThreadId, 0);
	if(nErr != VK_OK)
	{
		HxLOG_Critical("Error:: mheg ib key task create error = %d\n", nErr);
		return ERR_FAIL;
	}

	nErr = VK_TASK_Start(ulKeyEventThreadId);
	if(nErr != VK_OK)
	{
		HxLOG_Critical("Error:: mheg ib key task create error = %d\n", nErr);
		return ERR_FAIL;
	}
#endif
	return ERR_OK;
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
#define	_____________MHEGIB_PROC______________

void		MHEGIB_CreateAppl(void)
{
	HxLOG_Print("[MHEGIB_CreateAppl]  Call ONDK_SURFACE_Create\n");

	s_hMhegIbSurface = ONDK_SURFACE_Create(MHEG_IB_SUR_SCREEN_WIDTH, MHEG_IB_SUR_SCREEN_HEIGHT, "ARGB8888");

	if(s_hMhegIbWindow == NULL)
	{
		HxLOG_Print("[MHEGIB_CreateAppl]  s_hMhegIbWindow is NULL\n");
		return;
	}

	HxLOG_Print("[MHEGIB_CreateAppl]  Call ONDK_WINDOW_Clear\n");
	ONDK_WINDOW_Clear(s_hMhegIbWindow);
	HxLOG_Print("[MHEGIB_CreateAppl]  Call ONDK_WINDOW_SetShow\n");
	ONDK_WINDOW_SetShow(s_hMhegIbWindow, TRUE);

	if(s_font == NULL)
	{
		s_fontwidth = 0;
		HxLOG_Print("[MHEGIB_CreateAppl]  Call local_MHEGIB_CreateFont\n");
		if(local_MHEGIB_CreateFont(20) != ERR_OK)
		{
			HxLOG_Error("[local_MHEGIB_CreateFont] Error\n");
		}
	}

	HxLOG_Print("[MHEGIB_CreateAppl] s_hMhegIbSurface = 0x%x\n", s_hMhegIbSurface);
	return;
}

void		MHEGIB_DestroyAppl(void)
{
	HxLOG_Print("[MHEGIB_DestroyAppl] local_MHEGIB_Proc Destroy\n");

	ONDK_FONT_Release(s_font);
	s_font = NULL;


	ONDK_WINDOW_SetShow(s_hMhegIbWindow, FALSE);
	ONDK_WINDOW_Clear(s_hMhegIbWindow);

	ONDK_SURFACE_Destroy(s_hMhegIbSurface);

	ONDK_WINDOW_Update(s_hMhegIbWindow, NULL);

	s_hMhegIbSurface = NULL;

	return;
}

HBOOL	MHEGIB_CheckEngineStartCondition(void)
{
	HBOOL bCondition = MHEGIB_MAIN_CheckEngineStartCondition();

	HxLOG_Print("[MHEGIB_CheckEngineStartCondition] bCondition : %d\n", bCondition);

	return bCondition;
}

HERROR MHEGIB_FileRequst(HUINT16 usSession, eMhegIb_Ad_ReqType_t eMhegIbReqType, HUINT8 *pucReqName, HUINT32 ulReqNameLen)
{
	HERROR			hErr;

	HxLOG_Print("[MHEGIB_FileRequst] usSession = %d, eMhegIbReqType = %d, pucReqName = %s, ulReqNameLen = %d\n", usSession, eMhegIbReqType, pucReqName, ulReqNameLen);

	if (eMhegIbReqType == 0)
		hErr = APK_EXE_MHEGIB_RequestFile(usSession, ulReqNameLen, pucReqName);
	else
		hErr = APK_EXE_MHEGIB_RequestData(usSession, ulReqNameLen, pucReqName);

	if(hErr != ERR_OK)
	{
		HxLOG_Error("APK_MHEGIB_FileRequest( Type : %d) error \n", eMhegIbReqType);
		return ERR_FAIL;
	}

	return ERR_OK;
}

HBOOL MHEGIB_AppAbort(HUINT16 usSession, eMhegIb_Ad_AbortReqCode_t eMhegIbAbortReqCode)
{
	HERROR			hErr;

	if(eMhegIbAbortReqCode == eMhegIb_Ad_AbortReqCode_UseCancel)
	{
		hErr = 	APK_EXE_MHEGIB_AbortByUser(usSession);
	}
	else
	{
		hErr = 	APK_EXE_MHEGIB_AbortBySystem(usSession);
	}

	if(hErr != ERR_OK)
	{
		HxLOG_Error("APK_MHEGIB_FileRequest( Type : %d) error \n", eMhegIbAbortReqCode);
		return FALSE;
	}

	return TRUE;
}

void MHEGIB_CreateWindow(int argc, char **argv)
{
	HERROR		hErr;
	ONDKWindowHandle hWndHandle;

	if(s_hMhegIbWindow == NULL)
	{
		ONDK_Init(argc, argv);

		ONDK_KEY_Active(TRUE);

		hWndHandle = ONDK_WINDOW_Create(eAWINDOW_MHEGIB, MHEG_IB_WINDOW_SCREEN_X, MHEG_IB_WINDOW_SCREEN_Y, MHEG_IB_WINDOW_SCREEN_WIDTH, MHEG_IB_WINDOW_SCREEN_HEIGHT, "ARGB8888", TRUE);
		ONDK_WINDOW_SetShow(hWndHandle, TRUE);
		ONDK_WINDOW_Update(hWndHandle, NULL);

		hErr = local_MhegIb_InitKeyProc(hWndHandle);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("[local_MhegIb_InitKeyProc] Err\n");
		}

		s_hMhegIbWindow = hWndHandle;
	}
}

/* End of File */
