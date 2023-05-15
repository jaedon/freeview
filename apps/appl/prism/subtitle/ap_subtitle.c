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

/**************************************************************************************************/
#define _________AP_Subtitle_Private_Include_________________________________________________
/**************************************************************************************************/
#include <prism.h>
#include <subtitle.h>

#ifndef AUTOCONF_INCLUDED
#include <autoconf.h>
#endif

/**************************************************************************************************/
#define _________AP_Subtile_Private_Macro_________________________________________________
/**************************************************************************************************/
#undef		COMMAND_NAME
#define		COMMAND_NAME				"APP_SUB"

#define		SUBTITLE_DVB_SEGMENT_TIMERID			0x1000

#ifdef  CONFIG_DEBUG
#if !defined(CONFIG_DROP_ROOT_PRIVILEGES)
	#define	DEBUG_PACKET_DUMP_FILE		"/packet_data.dat"
	#define	DEBUG_PACKET_PES_FILE		"/packet_read.dat"
#else
	#define DEBUG_PACKET_DUMP_FILE		"/tmp/packet_data.dat"
	#define DEBUG_PACKET_PES_FILE		"/tmp/packet_read.dat"
#endif

//#define	 DEBUG_PACKET_DUMP

static HBOOL	        s_bEmulatePumping = FALSE;
#endif

#define AP_SBT_PARAM_WARRING_REMOVE(arg1, arg2, arg3, arg4)				\
			{															\
			(void)arg1;													\
			(void)arg2;													\
			(void)arg3;													\
			(void)arg4;							 						\
			}

#define SUBTITLE_WIDTH				GWM_SCREEN_WIDTH
#define SUBTITLE_HEIGHT				GWM_SCREEN_HEIGHT

/**************************************************************************************************/
#define _________AP_Subtile_Private_Struct_________________________________________________
/**************************************************************************************************/
typedef	enum
{
	SUBTITLE_TYPE_NONE = 0,
	SUBTITLE_TYPE_DVB,
	SUBTITLE_TYPE_EBU,
	SUBTITLE_TYPE_SMI
} SUBTITLE_TYPE_e;

typedef	struct	_DVBData
{
	HINT32		page_timeout;
	HUINT16		usCompPageId;
	HUINT16		usAncPageId;

	HxList_t	*page_segment;
	HxList_t	*region_segment;
	HxList_t	*clut_segment;
	HxList_t	*object_segment;
	void		*display_segment;
	HBOOL		end_of_segment;
} DVBData_t;

typedef	struct	_EBUData
{
	HUINT16		usDummy;
} EBUData_t;

typedef struct _Subtitle_Contents
{
    APK_HANDLE              m_liveMedia;
    APK_HANDLE              m_tsrPlayback;
    APK_HANDLE              m_mediaPlayback;

	Handle_t				hCurrentSession;
	Handle_t				hLastSession;

	HBOOL				    initialized;
	HBOOL				    displaySubtitle;
	SUBTITLE_TYPE_e		    subtitle_type;
	HINT32				    shared_id;
	HINT32				    shared_size;
	HINT32				    last_offset;

    HBOOL                   m_bDisplayState;  // teletext 활성화되면 , display 중지 다시 비활성화되면 기존 플래그를 보고 재가동
    HBOOL					m_bLiveLock; // LIVE 가 LOCK 인상태
    HBOOL					m_bHDCPblock;
	union
	{
		DVBData_t	stDVBInfo;
		EBUData_t	stEBUInfo;
	} data;

} Subtitle_Contents_t;


/**************************************************************
	start of SMI Subtitle Display
 **************************************************************/

#define DEFAULT_FONT_SIZE 36
#define DEFAULT_TEXT_POSITON 600

typedef struct tag_PAL_MEDIA_SUB_Display_Packet
{
	HUINT32         ulFontSize;
	HUINT32         ulTextPosition;
	HUINT32         ulStrNum;
	HUINT8          pText[8][64];
} PAL_MEDIA_SUB_Display_Packet_t;

#include    "dvb_subtitle_parser.c"

/**************************************************************************************************/
#define _________AP_Subtile_Private_Value_________________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define _________AP_Subtile_Private_Prototype_________________________________________________
/**************************************************************************************************/
//init
static Subtitle_Contents_t* local_subtitle_GetContents (void );

//update

//process
static ONDK_Result_t	local_subtitle_Initialize(Subtitle_Contents_t *pstContents);
static void	            local_subtitle_displayDVBSubtitle(DVBData_t *pstInfo, HBOOL bSurfaceClear);
static void				local_subtitle_processMediaSubtitle(HUINT32 ulLen, HUINT8 *pucData);

//destroy

//ui

//msg run
static ONDK_Result_t 	local_subtitle_MsgGwmDestroy(Subtitle_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	local_subtitle_MsgGwmCreate(Subtitle_Contents_t *pstContents,Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	local_subtitle_MsgGwmTimer(Subtitle_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	local_subtitle_signal_RequestStandby(Subtitle_Contents_t *pConents);
static ONDK_Result_t 	local_subtitle_signal_ShowSubtitle(Subtitle_Contents_t *pConents,HINT32 param);
static void	        	local_subtitle_resetSubtitleModule(HUINT32 ulViewId);
static void	        	local_subtitle_cbTimeoutOccur(HUINT32 ulTimerId);


/**************************************************************************************************/
#define _________AP_Subtile_Private_Init__________________________________________________________
/**************************************************************************************************/
static Subtitle_Contents_t* local_subtitle_GetContents(void)
{
	static Subtitle_Contents_t			*pSubtitle_Contents = NULL;

	if(pSubtitle_Contents == NULL)
	{
		HxLOG_Debug("[%s] Subtitle_Contents_t created newly \n", __FUNCTION__);
		pSubtitle_Contents = (Subtitle_Contents_t*)AP_Malloc(sizeof(Subtitle_Contents_t));
		HxSTD_MemSet(pSubtitle_Contents,0x0,sizeof(Subtitle_Contents_t));
	}
	return pSubtitle_Contents;
}

static ONDKWindowHandle local_subtitle_GetWindowHandle(void)
{
    static ONDKWindowHandle        s_window = NULL;

    if(s_window == NULL)
    {
         s_window = ONDK_WINDOW_Create(ONDK_LOWER_WINDOWID,0,0, 1280,720, "ARGB8888", TRUE);
    }

    return s_window;
}

/**************************************************************************************************/
#define _________AP_Subtile_Private_Update__________________________________________________________
/**************************************************************************************************/



/**************************************************************************************************/
#define _________AP_Subtile_Private_Process__________________________________________________________
/**************************************************************************************************/
static ONDK_Result_t		local_subtitle_Initialize(Subtitle_Contents_t *pstContents)
{
	ONDK_Assert(pstContents);

	local_subtitle_resetSubtitleModule(0);
	HxSTD_MemSet(pstContents, 0, sizeof(Subtitle_Contents_t));

	pstContents->hCurrentSession = HANDLE_NULL;
	pstContents->hLastSession = HANDLE_NULL;

	return GWM_RET_OK;
}


void	local_subtitle_displayDVBSubtitle(DVBData_t *pstInfo, HBOOL bSurfaceClear)
{
	HUINT16				page_id;
	HINT32				display_x, display_y, display_w, display_h;
	ONDK_Rect_t			control_rect;
	PageSegment_t		*page_segment = NULL;
	DisplaySegment_t	*display_segment = NULL;

	static	ONDK_Rect_t		_oldRect = {0, 0, 0, 0};
	ONDK_Rect_t				updateRect;

	if (bSurfaceClear)
		_oldRect = ONDK_Rect(0, 0, SUBTITLE_WIDTH, SUBTITLE_HEIGHT);

	updateRect = ONDK_Rect(0, 0, 0, 0);

	ONDK_WINDOW_Clear(local_subtitle_GetWindowHandle());

	display_segment = pstInfo->display_segment;
	if (display_segment)
	{
		display_x = display_segment->u16DWHorPosMin;
		display_y = display_segment->u16DWVerPosMin;
		display_w = display_segment->u16DisplayWidth;
		display_h = display_segment->u16DisplayHeight;
	}
	else
	{
		display_x = 0;
		display_y = 0;
		display_w = 720;
		display_h = 576;
	}

	HxLOG_Print("<--- display [%d, %d, %d, %d]\n", display_x, display_y, display_w, display_h);

	control_rect.x = 0;
	control_rect.y = 0;
	control_rect.w = SUBTITLE_WIDTH;
	control_rect.h = SUBTITLE_HEIGHT;

	page_id = pstInfo->usCompPageId;
	page_segment = local_subtitle_find_page_segment(pstInfo, page_id);
	if (page_segment)
	{
		HINT32		i;
		HUINT16		region_id;
		HUINT32		pos_x, pos_y, pos_w, pos_h;
//		ONDK_Rect_t		rel_rect;
		RegionSegment_t	*region_segment = NULL;
		CLUTSegment_t	*color_segment = NULL;
		ONDK_Rect_t		src_rect, dst_rect;

		HxLOG_Print("page_segment->ulRegionNum : [%d]\n", page_segment->ulRegionNum);

		for (i = 0; i < page_segment->ulRegionNum; i++)
		{
			if (page_segment->pRgList)
			{
				pos_x = page_segment->pRgList[i].u16RegionHorAddr + display_x;
				pos_y = page_segment->pRgList[i].u16RegionVerAddr + display_y;
				region_id = page_segment->pRgList[i].u8RegionId;

				HxLOG_Print("position [%d, %d]\n", pos_x, pos_y);

				region_segment = local_subtitle_find_region_segment(pstInfo, page_id, region_id);
				if (region_segment)
				{
					color_segment = local_subtitle_find_clut_segment(pstInfo, page_id, region_segment->u8ClutId);
					if (color_segment == NULL)
					{
						HxLOG_Critical("No CLUT Segment...\n");
						continue;
					}

					if (region_segment->surface && local_subtitle_GetWindowHandle())
					{
						HxLOG_Print("Region [%d, %d, %d, %d]\n", pos_x, pos_y, region_segment->u16RegionWidth, region_segment->u16RegionHeight);
						pos_w = (control_rect.w * region_segment->u16RegionWidth) / display_w;
						pos_h = (control_rect.h * region_segment->u16RegionHeight) / display_h;
						pos_x = (control_rect.w * pos_x) / display_w;
						pos_y = (control_rect.h * pos_y) / display_h;
						HxLOG_Print("--> converted [%d, %d, %d, %d]\n", pos_x, pos_y, pos_w, pos_h);
//						rel_rect = ONDK_Rect(pos_x, pos_y, pos_w, pos_h);

						dst_rect.x = pos_x;
						dst_rect.y = pos_y;
						dst_rect.w = pos_w;
						dst_rect.h = pos_h;

						src_rect.x = 0;
						src_rect.y = 0;
						src_rect.w = region_segment->u16RegionWidth;
						src_rect.h = region_segment->u16RegionHeight;

						HxLOG_Print("region : src(%d, %d, %d, %d) dst[%d, %d, %d, %d]\n",
							src_rect.x, src_rect.y, src_rect.w, src_rect.h,
							dst_rect.x, dst_rect.y, dst_rect.w, dst_rect.h);

						ONDK_WINDOW_StretchBlit(local_subtitle_GetWindowHandle(), region_segment->surface, src_rect, dst_rect);

						updateRect = ONDK_UnionRect(&updateRect, &dst_rect);
					}
				}
			}
		}
	}

	{
		ONDK_Rect_t	tmpRect;

		tmpRect = ONDK_Rect(updateRect.x, updateRect.y, updateRect.w, updateRect.h);
		updateRect = ONDK_UnionRect(&updateRect, &_oldRect);
		_oldRect = ONDK_Rect(tmpRect.x, tmpRect.y, tmpRect.w, tmpRect.h);
	}

	HxLOG_Print("update_rect (%d, %d, %d, %d)\n", updateRect.x, updateRect.y, updateRect.w, updateRect.h);
	ONDK_WINDOW_Update(local_subtitle_GetWindowHandle(), &updateRect);
}

void	local_subtitle_resetSubtitleModule(HUINT32 ulViewId)
{
	Subtitle_Contents_t *pConents;
	pConents = local_subtitle_GetContents();

	HxLOG_Debug("[%s] subtitle type : %d \n", __FUNCTION__, pConents->subtitle_type);

	switch (pConents->subtitle_type)
	{
		case SUBTITLE_TYPE_DVB:
			{
				DVBData_t	*dvbInfo;

				dvbInfo = &(pConents->data.stDVBInfo);
				ONDK_GWM_KillTimer(SUBTITLE_DVB_SEGMENT_TIMERID);

				local_subtitle_delete_all_segment(dvbInfo);
				if (dvbInfo->display_segment)
					AP_Free(dvbInfo->display_segment);

				dvbInfo->display_segment = NULL;

				local_subtitle_displayDVBSubtitle(dvbInfo, TRUE);
			}
			break;
		case SUBTITLE_TYPE_EBU:
			break;
		case SUBTITLE_TYPE_SMI:
			{
				//1 #### erase by empty string
				PAL_MEDIA_SUB_Display_Packet_t t_subpkt = {0, };
				t_subpkt.ulStrNum = -1;
				local_subtitle_processMediaSubtitle(0, (HUINT8*)&t_subpkt);
			}
			break;
		case SUBTITLE_TYPE_NONE:
		default:
			pConents->subtitle_type = SUBTITLE_TYPE_NONE;
			pConents->initialized = FALSE;
			pConents->last_offset = -1;
			break;
	}


}


/**************************************************************
  Event Handler
 **************************************************************/

ONDK_Result_t local_subtitle_signal_RequestStandby(Subtitle_Contents_t *pConents)
{
	//  reset all data when other application broadcast request standby signal
	local_subtitle_resetSubtitleModule(0);
	return GWM_RET_OK;
}

ONDK_Result_t local_subtitle_signal_ShowSubtitle(Subtitle_Contents_t *pContents, HINT32  param)
{
	HBOOL  				 bDisplaySubtitle = (HBOOL)param;
	Subtitle_Contents_t 	*pConents;

	pConents = local_subtitle_GetContents();

	if (pContents == NULL)
	{
		HxLOG_Critical("[%s;%d] pContents is NULL\n", __FUNCTION__, __LINE__);
		return GWM_MESSAGE_BREAK;
	}

	HxLOG_Debug("bDisplaySubtitle(%d) pConents->m_bLiveLock(%d) , pConents->m_bHDCPblock (%d)\n", bDisplaySubtitle  ,pConents->m_bLiveLock,pConents->m_bHDCPblock );

	if (bDisplaySubtitle == TRUE && pConents->m_bLiveLock == FALSE && pConents->m_bHDCPblock == FALSE)
	{
		pContents->displaySubtitle = TRUE;

		//1 #### BUS Overhead 문제발생을 대비하여, BUS가 누락될 경우 생성되지 않은 윈도우를 다시 얻어오도록 함.
		HxLOG_Print(HxANSI_COLOR_YELLOW("######################################################\n"));
		HxLOG_Print(HxANSI_COLOR_YELLOW("######################################################\n"));
		HxLOG_Print(HxANSI_COLOR_YELLOW("################### Show Subtitle ####################\n"));
		HxLOG_Print(HxANSI_COLOR_YELLOW("######################################################\n"));
		HxLOG_Print(HxANSI_COLOR_YELLOW("######################################################\n"));
		ONDK_WINDOW_SetShow(local_subtitle_GetWindowHandle(),TRUE);
	}
	else
	{
		pContents->displaySubtitle = FALSE;
		HxLOG_Print(HxANSI_COLOR_YELLOW("######################################################\n"));
		HxLOG_Print(HxANSI_COLOR_YELLOW("######################################################\n"));
		HxLOG_Print(HxANSI_COLOR_YELLOW("################### Hide Subtitle ####################\n"));
		HxLOG_Print(HxANSI_COLOR_YELLOW("######################################################\n"));
		HxLOG_Print(HxANSI_COLOR_YELLOW("######################################################\n"));

		ONDK_WINDOW_SetShow(local_subtitle_GetWindowHandle(),FALSE);
	}
	return GWM_MESSAGE_BREAK;
}

void	local_subtitle_processDVBSubtitle(HUINT32 ulLen, HUINT8 *pucData)
{
	HUINT8				*p = pucData;
	Subtitle_Contents_t 	*pContents;
	Handle_t				 hSession;

	HxLOG_Info("[%s] len(%d)\n", __FUNCTION__, ulLen);
	pContents = local_subtitle_GetContents();

	ONDK_WINDOW_Clear(local_subtitle_GetWindowHandle());

	pContents->subtitle_type = SUBTITLE_TYPE_DVB;
	pContents->data.stDVBInfo.usCompPageId = HxMACRO_Get32Bit(p);		p += 4;
	pContents->data.stDVBInfo.usAncPageId  = HxMACRO_Get32Bit(p);		p += 4;
	hSession = (Handle_t)HxMACRO_Get32Bit(p);							p += 4;

	if (pContents->hCurrentSession == HANDLE_NULL)
	{
		if ((pContents->hLastSession != HANDLE_NULL) && (hSession == pContents->hLastSession))
		{
			HxLOG_Warning(">>> Last Session Subtitle Data!!!\n");
			return;
		}
	}

	pContents->hCurrentSession = hSession;

	{
		HBOOL			surface_clear = FALSE;
		PageSegment_t	*page_segment = NULL;
		PageSegment_t	*ancillary_page_segment = NULL;
		DVBData_t		*dvbInfo = NULL;

		dvbInfo = &(pContents->data.stDVBInfo);
		m_subtitle_decode_dvb(dvbInfo, p, ulLen - 12);

		page_segment = local_subtitle_find_page_segment(dvbInfo, dvbInfo->usCompPageId);
		ancillary_page_segment = local_subtitle_find_page_segment(dvbInfo, dvbInfo->usAncPageId);
		if (page_segment && page_segment->ulRegionNum == 0)
		{
			//	clear subtitle area
			surface_clear = TRUE;
		}
		else if (dvbInfo->usCompPageId != dvbInfo->usAncPageId)
		{
			if (ancillary_page_segment && ancillary_page_segment->ulRegionNum == 0)
			{
				//	clear subtitle area
				surface_clear = TRUE;
			}
		}

		if (page_segment)
		{
			ONDK_GWM_KillTimer(SUBTITLE_DVB_SEGMENT_TIMERID);
			ONDK_GWM_SetTimer(SUBTITLE_DVB_SEGMENT_TIMERID, page_segment->u8PageTimeOut * 1000);
		}
		else if(ancillary_page_segment)
		{
			ONDK_GWM_KillTimer(SUBTITLE_DVB_SEGMENT_TIMERID);
			ONDK_GWM_SetTimer(SUBTITLE_DVB_SEGMENT_TIMERID, ancillary_page_segment->u8PageTimeOut * 1000);
		}

		if (dvbInfo->usAncPageId != dvbInfo->usCompPageId)
			m_subtitle_process_dvb(local_subtitle_GetWindowHandle(),dvbInfo->usAncPageId, dvbInfo);

		m_subtitle_process_dvb(local_subtitle_GetWindowHandle(),dvbInfo->usCompPageId, dvbInfo);

		//	UPDATE!!!!
		local_subtitle_displayDVBSubtitle(&(pContents->data.stDVBInfo), surface_clear);
	}
}

void	local_subtitle_processSMISubtitle(HUINT32 ulLen, HUINT8 *pucData)
{
	// 미구현..뭐지????

}

void	local_subtitle_processMediaSubtitle(HUINT32 ulLen, HUINT8 *pucData)
{
	int i, nfntsize, npos;
	PAL_MEDIA_SUB_Display_Packet_t t_subpkt;
	Subtitle_Contents_t *pContents;

	pContents = local_subtitle_GetContents();

	if ( local_subtitle_GetWindowHandle() == NULL )
	{
		HxLOG_Debug("[%s] window has Null \n", __FUNCTION__);
		return;
	}

	pContents->subtitle_type = SUBTITLE_TYPE_SMI;

	HxSTD_MemSet(&t_subpkt, 0x00, sizeof(PAL_MEDIA_SUB_Display_Packet_t));
	HxSTD_MemCopy(&t_subpkt, pucData, sizeof(PAL_MEDIA_SUB_Display_Packet_t));

	if(t_subpkt.ulFontSize == 0)
		nfntsize = DEFAULT_FONT_SIZE;
	else
		nfntsize = t_subpkt.ulFontSize;

	if(t_subpkt.ulTextPosition == 0)
		npos = DEFAULT_TEXT_POSITON;
	else
		npos = t_subpkt.ulTextPosition;

	if(t_subpkt.ulStrNum  == -1)
	{
		HxLOG_Debug("[%s] clear Subtitle Window !!!! \n", __FUNCTION__);
		//1 ## ONDK_WINDOW_Clear 를 해도 자막이 사라지지않음.
		ONDK_WINDOW_Clear(local_subtitle_GetWindowHandle());
		//2 ## 그래서 아래와 같이 empty string 을 강제로 그리게 함.
		ONDK_FONT_DrawStringMediaSubtitle_ToWindow(local_subtitle_GetWindowHandle(), "", nfntsize, 0);
	}
	else
	{
		npos = npos - ((nfntsize * t_subpkt.ulStrNum )/2);
		for(i=0; i<t_subpkt.ulStrNum; i++)
		{
			ONDK_FONT_DrawStringMediaSubtitle_ToWindow(local_subtitle_GetWindowHandle(), t_subpkt.pText[i], nfntsize, npos + (i * (nfntsize +2)));
			HxLOG_Print("[%s:%d] ==> %s \n", __FUNCTION__, __LINE__, t_subpkt.pText[i]);
		}
	}
}

static ONDK_Result_t local_subtitle_checkhdcp (Subtitle_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t 			hErr = GWM_MESSAGE_BREAK;
	DxHdcpStatus_e			eHdcpStatus = (DxHdcpStatus_e)p1;
	Subtitle_Contents_t 	*pConents;

	pConents = local_subtitle_GetContents();

	if(eHdcpStatus == eDxHDCPSTATUS_OK || eHdcpStatus == eDxHDCPSTATUS_OFF)
	{
		pConents->m_bHDCPblock = FALSE;
		local_subtitle_signal_ShowSubtitle(pConents,TRUE);
	}
	else if(eHdcpStatus == eDxHDCPSTATUS_FAIL || eHdcpStatus == eDxHDCPSTATUS_NODEVICE)
	{
		pConents->m_bHDCPblock = TRUE;
		local_subtitle_signal_ShowSubtitle(pConents,FALSE);
	}

	HxLOG_Debug("HDCP Status Change.. [%d] Subtitle block[%d]\n",eHdcpStatus,pConents->m_bHDCPblock);

	return hErr;
}

#if defined(CMDCONSOLE_ENABLE) && defined(CONFIG_DEBUG)
void local_subtitle_EmulPESRun(void *args)
{
	HINT32	_offset = 0;
	FILE	*pesFile = fopen(DEBUG_PACKET_PES_FILE, "rb");

	if (pesFile == NULL)
	{
		HxLOG_Error("pesFile is not found : [%s]\n", DEBUG_PACKET_DUMP_FILE);
		return;
	}

	while (1)
	{
		if (s_bEmulatePumping)
		{
			HUINT8		aucData[4];
			HINT32		size;
			HUINT8		*pucBuffer = NULL;
			size_t 		len = 0;
			//HINT32		len, lenPes;

			len = fread(aucData, 4, 1, pesFile);
			if(len != 0)
			{
				//HxLOG_Print("[%02x %02x %02x %02x]\n", aucData[0], aucData[1], aucData[2], aucData[3]);
				size = aucData[0] | (aucData[1] << 8) | (aucData[2] << 16) | (aucData[3] << 24);
				pucBuffer = (HUINT8*)AP_Malloc(size);
				if(pucBuffer != NULL)
				{
					if( (len = fread((void *)pucBuffer, size, 1, pesFile)) != 0 )
					{
						HxLOG_Print("offset : [%d]\n", _offset);
						_offset += (size + 4);

						Subtitle_deliveryPacket("dvbsubtitle",size, (void *)pucBuffer);
					}
					AP_Free(pucBuffer);
				}
			}
		}
		HLIB_STD_TaskSleep(100);
	}

	fclose(pesFile);
}

static int	CMD_Emulate_PES_Pumping(void *szArgList)
{
	if (s_bEmulatePumping)
		s_bEmulatePumping = FALSE;
	else
		s_bEmulatePumping = TRUE;

	if (s_bEmulatePumping)
	{
		static HULONG		ulThreadId = 0;

		if (ulThreadId == 0)
		{
			VK_TASK_Create(local_subtitle_EmulPESRun, 40, 0x4000, "EmulPES", NULL, &ulThreadId, 0);
			VK_TASK_Start(ulThreadId);
		}
	}

	return HxCMD_OK;
}
static HBOOL s_bSubttlDBG = FALSE;
static int	CMD_SUBTTL_DBGSubtitle(void *szArgList)
{
	s_bSubttlDBG = TRUE;

	return HxCMD_OK;
}

static int	CMD_SUBTLE_DBGOffSubtitle(void *szArgList)
{
	s_bSubttlDBG = FALSE;

	return HxCMD_OK;
}

void	Subtitle_CMD_Register_PESEmul(HUINT8 *cmdname)
{
	HLIB_CMD_RegisterWord("EMUL_CMD", CMD_Emulate_PES_Pumping,
			"pump",							/* keyword */
			"pump /packet_data.red",		/*   help  */
			"pump");						/*  usage  */
	HLIB_CMD_RegisterWord("EMUL_CMD", CMD_SUBTTL_DBGSubtitle,
			"dbg",
			"dbg",
			"dbg");
	HLIB_CMD_RegisterWord("EMUL_CMD", CMD_SUBTLE_DBGOffSubtitle,
			"dbgoff",
			"dbgoff",
			"dbgoff");
}
#endif



/**************************************************************************************************/
#define _________AP_Subtitle_Private_Destroy__________________________________________________________
/**************************************************************************************************/
static ONDK_Result_t local_subtitle_Destroy(Subtitle_Contents_t *pstContents)
{
	local_subtitle_resetSubtitleModule(0);

	return GWM_RET_OK;
}

/**************************************************************************************************/
#define _________AP_Subtitle_Private_Proc_UI__________________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________AP_Subtitle_Private_Proc_MSG_RUNNIG__________________________________________________________
/**************************************************************************************************/

//MSG_GWM_CREATE
static ONDK_Result_t local_subtitle_MsgGwmCreate(Subtitle_Contents_t *pstContents,  Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Assert(pstContents);

	/* 절대 hide가 되지 않는다. */
	//ONDK_GWM_APP_SetUIHideFlag(GWM_UI_Hide_Never);

	ONDK_WINDOW_SetShow(local_subtitle_GetWindowHandle(),TRUE);

	HxLOG_Debug("[%s] pstContents->m_window : %p \n", __FUNCTION__, local_subtitle_GetWindowHandle());

	local_subtitle_Initialize(pstContents);

	AP_SBT_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);

	return GWM_RET_OK;
}

//MSG_GWM_TIMER
static ONDK_Result_t local_subtitle_MsgGwmTimer (Subtitle_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Assert(pstContents);

	switch (p1)
	{
		case SUBTITLE_DVB_SEGMENT_TIMERID:
			{
				Subtitle_Contents_t *pContents;

				pContents = local_subtitle_GetContents();

				switch (pContents->subtitle_type)
				{
					case SUBTITLE_TYPE_DVB:
						{
							DVBData_t	*dvbInfo = &(pContents->data.stDVBInfo);

							local_subtitle_delete_region_segment(dvbInfo, dvbInfo->usCompPageId, 0xFFFFFFFF);
							local_subtitle_displayDVBSubtitle(dvbInfo, TRUE);
						}
						break;
					case SUBTITLE_TYPE_EBU:
						break;
					case SUBTITLE_TYPE_SMI:
						break;
					default:
						break;
				}

				break;
			}

		default:
			break;
	}

	return GWM_MESSAGE_PASS;
}

//MSG_GWM_DESTROY
static ONDK_Result_t local_subtitle_MsgGwmDestroy(Subtitle_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Assert(pstContents);

	AP_SBT_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);
	return local_subtitle_Destroy(pstContents);
}


static HCHAR *local_subtitle_GetEventString (OxMediaPlay_Event_e eEvent)
{
	switch (eEvent)
	{
		ENUM_TO_STR (eOxMP_EVENT_TUNE_Locked)
		ENUM_TO_STR (eOxMP_EVENT_TUNE_NoSignal)

		ENUM_TO_STR (eOxMP_EVENT_TUNE_AntennaOk)
		ENUM_TO_STR (eOxMP_EVENT_TUNE_AntennaNok)

		ENUM_TO_STR (eOxMP_EVENT_LIVE_SvcChanged)
		ENUM_TO_STR (eOxMP_EVENT_LIVE_SvcStopped)
		ENUM_TO_STR (eOxMP_EVENT_LIVE_ConflictChanged)
		ENUM_TO_STR (eOxMP_EVENT_LIVE_EventRelay)
		ENUM_TO_STR (eOxMP_EVENT_LIVE_Ews)
		ENUM_TO_STR (eOxMP_EVENT_LIVE_TSRTimecodeChanged)
		ENUM_TO_STR (eOxMP_EVENT_LIVE_BsCtrlStarted)
		ENUM_TO_STR (eOxMP_EVENT_LIVE_PinCtrlChangedByBCAS)
		ENUM_TO_STR (eOxMP_EVENT_LIVE_ViewStateChanged)
		ENUM_TO_STR (eOxMP_EVENT_LIVE_LockStateChanged)

		ENUM_TO_STR (eOxMP_EVENT_MEDIA_ProbeErrored)
		ENUM_TO_STR (eOxMP_EVENT_MEDIA_StopErrored)
		ENUM_TO_STR (eOxMP_EVENT_MEDIA_BufferStateChanged)
		ENUM_TO_STR (eOxMP_EVENT_MEDIA_BufferInfoChanged)
		ENUM_TO_STR (eOxMP_EVENT_MEDIA_PlayInfo)
		ENUM_TO_STR (eOxMP_EVENT_MEDIA_PumpIndexChanged)
		ENUM_TO_STR (eOxMP_EVENT_MEDIA_SeekReady)


		ENUM_TO_STR (eOxMP_EVENT_BOF)
		ENUM_TO_STR (eOxMP_EVENT_EOF)
		ENUM_TO_STR (eOxMP_EVENT_STARTED)
		ENUM_TO_STR (eOxMP_EVENT_STOPPED)
		ENUM_TO_STR (eOxMP_EVENT_PLAYING)
		ENUM_TO_STR (eOxMP_EVENT_PAUSE)
		ENUM_TO_STR (eOxMP_EVENT_TRICK)
		ENUM_TO_STR (eOxMP_EVENT_RATING_CHANGED)
		ENUM_TO_STR (eOxMP_EVENT_AV_STARTED)
		ENUM_TO_STR (eOxMP_EVENT_AV_STOPPED)
		ENUM_TO_STR (eOxMP_EVENT_SPEED_CHANGED)
		ENUM_TO_STR (eOxMP_EVENT_AV_MEDIARECT_CHANGED)
		ENUM_TO_STR (eOxMP_EVENT_SupportedSpeedChanged)
		ENUM_TO_STR (eOxMP_EVENT_VIDEOSTREAMINFO)
		ENUM_TO_STR (eOxMP_EVENT_PinCtrlChanged)

		ENUM_TO_STR (eOxMP_EVENT_SI_PMT_CHANGED)
		ENUM_TO_STR (eOxMP_EVENT_SI_EIT_CHANGED)
		ENUM_TO_STR (eOxMP_EVENT_SI_SIT_CHANGED)
		ENUM_TO_STR (eOxMP_EVENT_SI_AIT_CHANGED)
		ENUM_TO_STR (eOxMP_EVENT_SI_RCT_CHANGED)
		ENUM_TO_STR (eOxMP_EVENT_SI_NETWORK_CHANGED)
		ENUM_TO_STR (eOxMP_EVENT_COMPONENT_VIDEO_CHANGED)
		ENUM_TO_STR (eOxMP_EVENT_COMPONENT_AUDIO_CHANGED)
		ENUM_TO_STR (eOxMP_EVENT_COMPONENT_SUBTITLE_CHANGED)
		ENUM_TO_STR (eOxMP_EVENT_COMPONENT_SELECTED)

		ENUM_TO_STR (eOxMP_EVENT_DRM_BLOCK)

		ENUM_TO_STR (eOxMP_EVENT_RESOURCE_TAKEN)
		ENUM_TO_STR (eOxMP_EVENT_RESOURCE_LOST)

		ENUM_TO_STR (eOxMP_EVENT_PLAYERROR)
		ENUM_TO_STR (eOxMP_EVENT_PLAYSTATE)
		ENUM_TO_STR (eOxMP_EVENT_PLAYTIME)

		ENUM_TO_STR (eOxMP_EVENT_RADIOBG_CHANGED)
		ENUM_TO_STR (eOxMP_EVENT_TTX_UPDATE)
		ENUM_TO_STR (eOxMP_EVENT_SBTL_UPDATE)


		ENUM_TO_STR (eOxMP_EVENT_TrickRestrictModeChanged)


	default:
		break;
	}

	return "Unknown";
}


static ONDK_Result_t	local_subtitle_APKMessage(Handle_t hAct,HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	GWM_APK_MSG_Type_e			eAPK_Type = (GWM_APK_MSG_Type_e)hAct;
	OxMediaPlay_SubtitleStatus_t 	subStatus;
	OxMediaPlay_LiveViewState_e	eViewState;
	OxMediaPlay_LiveLockState_e	 	eLockState;

	Subtitle_Contents_t 	*pContent = local_subtitle_GetContents();
	ONDK_Result_t		ret= GWM_RET_OK;

	HUINT32					ulViewId			=	(ulParam1 >> 24) & 0x000F;
	//OxMediaPlay_MediaType_e	eType			=	(ulParam1 >> 16) & 0x000F;
	OxMediaPlay_Event_e		eEvent			=	(ulParam1 & 0x00FF);

	if ( pContent == NULL ) return GWM_RET_OK;

	HxLOG_Debug("liveMediaNotifier...( eAPK_Type : %d) \n", eAPK_Type);

	if(eAPK_Type == GWM_APK_MEDIA)
	{
		HxLOG_Debug(" -- ulViewId(%d) event(%s)\n", ulViewId, local_subtitle_GetEventString(eEvent));

		switch (eEvent)
		{
			case eOxMP_EVENT_LIVE_LockStateChanged:
				{
					eLockState = ulParam2;

					HxLOG_Debug("eOxMP_EVENT_LIVE_LockStateChanged :: eLockState(%d) pContent->m_bDisplayState(%d) \n\n", eLockState, pContent->m_bDisplayState);

					if (eLockState != eOxMP_LiveLockState_OK)
					{
						pContent->m_bLiveLock = TRUE;
					}
					else
					{
						pContent->m_bLiveLock = FALSE;
						if(pContent->m_bDisplayState == TRUE)
						{
							local_subtitle_signal_ShowSubtitle(pContent, TRUE);
						}
						else
						{
							local_subtitle_signal_ShowSubtitle(pContent, FALSE);
						}
					}
				}
				ret =  GWM_RET_OK;
				break;

			case	eOxMP_EVENT_LIVE_ViewStateChanged:
				{
					eViewState = ulParam2;

					HxLOG_Debug("eOxMP_EVENT_LIVE_ViewStateChanged :: eLockState(%d) pContent->m_bDisplayState(%d) \n\n", eViewState, pContent->m_bDisplayState);

					if (eViewState != eOxMP_LiveViewState_OK)
					{
						local_subtitle_signal_ShowSubtitle(pContent, FALSE);
					}
					else
					{
						if(pContent->m_bDisplayState == TRUE)
						{
							local_subtitle_signal_ShowSubtitle(pContent, TRUE);
						}
						else
						{
							local_subtitle_signal_ShowSubtitle(pContent, FALSE);
						}
					}
				}
				ret =  GWM_RET_OK;
				break;

			case	eOxMP_EVENT_COMPONENT_SUBTITLE_CHANGED:
				{
					HINT32	lSubtitleIndex = (HINT32)ulParam2;

					if (lSubtitleIndex != -1)
					{
						local_subtitle_signal_ShowSubtitle(pContent, TRUE);
					}
				}
				break;

			case	eOxMP_EVENT_SI_PMT_CHANGED:
				// pmt update시에는 subtitle stop하지 않는다 - 임시
				break;

			case eOxMP_EVENT_LIVE_SvcChanged:
			case eOxMP_EVENT_LIVE_SvcStopped:
				HxLOG_Debug("<---------------- Encounter... LIVE_SVC_Changed Event !!!\n");
				local_subtitle_signal_ShowSubtitle(pContent, FALSE); // 윈도우를 감추자. show 를 호출할 것이다.
				local_subtitle_resetSubtitleModule(ulViewId);
				pContent->hLastSession = pContent->hCurrentSession;
				pContent->hCurrentSession = HANDLE_NULL;
				break;

			case eOxMP_EVENT_TUNE_NoSignal:
				HxLOG_Debug("<---------------- Encounter... LIVE_SVC_Changed Event !!!\n");
				local_subtitle_signal_ShowSubtitle(pContent, FALSE); // 윈도우를 감추자. show 를 호출할 것이다.
				local_subtitle_resetSubtitleModule(ulViewId);
				break;

			case eOxMP_EVENT_SBTL_UPDATE:
				{
					//1 나중에 mediaplayer말고 live, tsr 등이 추가되면 구분해서 들어가야함.
					APK_MEDIA_PLAY_GetSubtitleStatus(ulViewId, &subStatus);
					if ( subStatus.eState == eSUBTTLState_Start )
					{
						HxLOG_Debug("SubtitlePlayer: Media subtitle started.\n");
						local_subtitle_signal_ShowSubtitle(pContent, TRUE);
					}
					else if ( subStatus.eState == eSUBTTLState_Stopped )
					{
						HxLOG_Debug("SubtitlePlayer: Media subtitle stopped.\n");
						local_subtitle_signal_ShowSubtitle(pContent, FALSE);
					}
					else if (subStatus.eState == eSUBTTLState_Reset)
					{
						HxLOG_Debug("SubtitlePlayer: eSUBTTLState_Reset.\n");
						local_subtitle_resetSubtitleModule(ulViewId);
					}
					//1 ### 이걸 왜 여기서 호출하도록???
					//local_subtitle_resetSubtitleModule(eDest);
				}
				ret = GWM_MESSAGE_BREAK;
				break;
			default:
				//HxLOG_Debug("Subtitle module doesn't need other media control event [%s]\n", local_subtitle_GetEventString(eEvent));
				break;
		}
	}

	return ret;
}



/**************************************************************************************************/
#define _________AP_Subtitle_Public_Proc____________________________________________________
/**************************************************************************************************/


ONDK_Result_t			AP_Subtitle_Proc(HINT32 nMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult	= GWM_RET_OK;
	Subtitle_Contents_t     	*pstContents = local_subtitle_GetContents();

	ONDK_Debug(HxANSI_COLOR_YELLOW("nMessage(%d : %s)  p1(0x%x) p2(0x%x) p3(0x%x) \n"), nMessage, ONDK_GWM_GwmMsg2String(nMessage), p1, p2, p3);

	switch (nMessage)
	{
		case MSG_GWM_CREATE:
			HxLOG_Print("[MSG_GWM_CREATE]\n");
			eResult = local_subtitle_MsgGwmCreate(pstContents, handle, p1, p2, p3);
			break;
		case MSG_GWM_KEYUP:
		case MSG_GWM_KEYDOWN :
		    eResult= ONDK_MESSAGE_NONCONSUMED;
		  	break;
		case MSG_SIGNAL_REQUESTSTANDBY :
	       	eResult =  local_subtitle_signal_RequestStandby(pstContents);
	        break;
		case MSG_APP_RCV_SUBTITLE_PACKET:
		{
			HINT32		len;
			HCHAR		*dataType;
			HUINT8		*p = (HUINT8 *)ONDK_GWM_GetData();

			if (NULL != p)
			{
				len = HxMACRO_Get32Bit(p);			p += 4;
				dataType = p;						p += len;
				len = p2 - (len + 4);
				Subtitle_deliveryPacket((HCHAR*)dataType, len, (HUINT8 *)p);
			}
		}
			break;
		case MSG_GWM_APK_MESSAGE:
			eResult = local_subtitle_APKMessage(handle,p1, p2, p3);
			break;
    	case MSG_SIGNAL_SHOWSUBTITLE:
			HxLOG_Print("[MSG_SIGNAL_SHOWSUBTITLE] m_bDisplayState = %d\n", p3);
			pstContents->m_bDisplayState = p3;
	    	eResult = local_subtitle_signal_ShowSubtitle(pstContents, p3);
        	break;
    	case MSG_APP_TELETEXT_STARTED:
			HxLOG_Print("[MSG_APP_TELETEXT_STARTED]\n");
        	eResult = local_subtitle_signal_ShowSubtitle(pstContents,FALSE);
         	break;
    	case MSG_APP_TELETEXT_STOPPED:
			HxLOG_Print("[MSG_APP_TELETEXT_STOPPED] pstContents->m_bDisplayState = %d\n", pstContents->m_bDisplayState);
        	if(pstContents->m_bDisplayState == TRUE)
        	 	eResult = local_subtitle_signal_ShowSubtitle(pstContents,TRUE);
         	break;
		case MSG_GWM_TIMER:
			eResult = local_subtitle_MsgGwmTimer(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_DESTROY:
			eResult = local_subtitle_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;
		case MGS_APP_HDCP_STATUS:
			eResult = local_subtitle_checkhdcp(pstContents, p1, p2, p3);
			break;
		default:
			break;

	}
	if(eResult != GWM_RET_OK)
	{
		return eResult;
	}
	return ONDK_GWM_ProcessMessageDefault(nMessage, handle, p1, p2, p3);
}

/**************************************************************************************************/
#define _________AP_Subtitle_Public_Functions__________________________________________________________
/**************************************************************************************************/
OxMediaPlay_SubtitleType_e AP_SubtitleGetType(void)
{
	HUINT32						nTrackNum, nTrackIndex;
	HUINT32						ulViewId;
	OxMediaPlay_MediaType_e		eMediaType, eSubtitleType;
	OxMediaPlay_Component_t  	stSubttlTrack;
	HERROR						hErr = ERR_FAIL;


	eSubtitleType = eOxMP_SUBTITLETYPE_Unknown;
	APK_MEDIA_PLAY_GetMainViewId(&ulViewId);
	APK_MEDIA_PLAY_GetPlayType(ulViewId, &eMediaType);

	HxSTD_MemSet (&stSubttlTrack, 0x0, sizeof(OxMediaPlay_Component_t));

	HxLOG_Print("eMediaType :%d \n",eMediaType);
	switch (eMediaType)
	{
		case eOxMP_MEDIATYPE_LIVE:
		case eOxMP_MEDIATYPE_PVRPLAYBACK:
		case eOxMP_MEDIATYPE_TSRPLAYBACK:
			hErr = APK_MEDIA_PLAY_GetComponentNum (ulViewId, eOxMP_COMPONENT_SUBTITLE, &nTrackNum);
			if((hErr == ERR_OK) && (0 < nTrackNum))
			{
				hErr = APK_MEDIA_PLAY_GetComponentIndex (ulViewId, eOxMP_COMPONENT_SUBTITLE, &nTrackIndex, NULL);
				if(hErr == ERR_OK)
				{
					hErr = APK_MEDIA_PLAY_GetComponent (ulViewId, eOxMP_COMPONENT_SUBTITLE, nTrackIndex, &stSubttlTrack);
					eSubtitleType = stSubttlTrack.stSubtitle.eSubtitleType;
				}
			}
			break;

		case eOxMP_MEDIATYPE_MEDIAPLAY:
			eSubtitleType = eOxMP_SUBTITLETYPE_Media;

		default:
			break;
	}

	return eSubtitleType;
}


void                    Subtitle_deliveryPacket(HCHAR* datatype, HINT32 len, HUINT8 *p)
{
    Subtitle_Contents_t     *pstContents;

#ifdef  DEBUG_PACKET_DUMP
    static FILE *packetFile = NULL;

    if (packetFile == NULL)
        packetFile = fopen(DEBUG_PACKET_DUMP_FILE, "wb");

    fwrite(&len, 4, 1, packetFile);
    fwrite(p, len, 1, packetFile);
    fflush(packetFile);
#endif
#ifdef  CONFIG_DEBUG
    if (s_bEmulatePumping)
        return;
#endif

	pstContents = local_subtitle_GetContents();

	HxLOG_Debug("displaySubtitle : %d, subtitle type : %s, len : %d, p : %p \n", pstContents->displaySubtitle, datatype, len, p);

	if(pstContents->displaySubtitle)
	{
		if (HLIB_STD_StrCaseCmp(datatype, "dvbsubtitle") == 0)
		{
			local_subtitle_processDVBSubtitle(len, p);
		}
		else  if (HLIB_STD_StrCaseCmp(datatype, "smisubtitle") == 0)
		{
			local_subtitle_processSMISubtitle(len, p);
		}
		else   if (HLIB_STD_StrCaseCmp(datatype, "mediasubtitle") == 0)
		{
			local_subtitle_processMediaSubtitle(len, p);
		}
	}
	else
	{

	}
}

/**************************************************************************************************/
#define _________AP_Subtitle_End_Of_File____________________________________________________
/**************************************************************************************************/

