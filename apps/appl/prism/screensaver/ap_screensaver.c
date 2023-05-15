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
#define _________SCREEN_SAVER_Private_Include_________________________________________________
/**************************************************************************************************/
#include <prism.h>
#include <dlib.h>
#include <dapi.h>
/**************************************************************************************************/
#define _________SCREEN_SAVER_Private_Macro_________________________________________________
/**************************************************************************************************/


#define	SCREENSAVER_LIVE_PATH		"/usr/image/05_LiveTV_13Screensaver_01.gif"  // av 가 나오는 화면에서의 screensaver
#define	SCREENSAVER_MEDIA_PATH	"/usr/image/05_LiveTV_13Screensaver_02.gif"  // music 과 radio 채널에서의 screensaver
#define	SCREENSAVER_FONTPATH		"/usr/fonts/DejaVuSans.ttf"
#define SCREENSAVER_BG_PATH		"/usr/image/Screensaver_bg.png"				// video provider 가 player 될때 surface를 투명하게 만들어버린다.
#define FONT_TITLE_SIZE 			42
#define	FONT_INFO_SIZE				24
#define SCREENSAVER_IMG_BG_ID		1
#define	URI_STARTPATH				"file://"
#define SCREENSAVER_SEMA_NAME		"ScreenSaverSemaphore"


/**************************************************************************************************/
#define _________SCREEN_SAVER_Private_Struct_________________________________________________
/**************************************************************************************************/
typedef struct _ScreenSaver_Contents
{
	ScreenSaver_Mode_e	eMode;
	HBOOL				bExit;
	ONDK_Rect_t			drawRect;

	HCHAR*				pszTitle;
	HCHAR*				pszInfo1;
	HCHAR*				pszInfo2;

	ONDKVideoHandle		video;
	ONDKFontHandle		*pFontTitle;
	ONDKFontHandle		*pFontInfo;

} ScreenSaver_Contents_t;


/**************************************************************************************************/
#define _________SCREEN_SAVER_Private_Value_________________________________________________
/**************************************************************************************************/
static ScreenSaver_Contents_t	s_stScreenSaverContents;
static HLONG					s_ScreenSaverSemId = 0;

/**************************************************************************************************/
#define _________SCREEN_SAVER_Private_prototype_functions_________________________________________
/**************************************************************************************************/
static void 			_screensaver_getservice(void);
static void 			_screensaver_getprogramme(void);
static ONDK_Result_t	_screensaver_MsgGwmCreate(ScreenSaver_Contents_t	*pContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	_screensaver_MsgGwmDestroy(ScreenSaver_Contents_t	*pContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	_screensaver_MsgGwmTimer(ScreenSaver_Contents_t	*pContents, HINT32 p1, HINT32 p2, HINT32 p3);


/**************************************************************************************************/
#define _________SCREEN_SAVER_Private_functions___________________________________________
/**************************************************************************************************/
static ScreenSaver_Contents_t	*_screensaver_GetContents(void)
{
	return &s_stScreenSaverContents;
}


static ONDK_Result_t 	_screensaver_init(ScreenSaver_Contents_t *contents)
{
	ONDK_Rect_t 		rect;
	ONDK_Print("[%s:%d] (+)\n", __FUNCTION__, __LINE__);

	HxSTD_MemSet(contents, 0, sizeof(ScreenSaver_Contents_t));
	contents->pszTitle  =NULL;
	contents->pszInfo1  =NULL;
	contents->pszInfo2  =NULL;
	contents->eMode		= eMODE_LIVE;
	contents->bExit = FALSE;

	ONDK_Print("[%s:%d] (-)\n", __FUNCTION__, __LINE__);
	rect = ONDK_Rect(0, 0, GWM_SCREEN_WIDTH, GWM_SCREEN_HEIGHT);
	ONDK_GWM_COM_Rect_Create(SCREENSAVER_IMG_BG_ID, &rect,0xFF000000);
	return ONDK_RET_OK;
}

static HUINT32	_screensaver_rand_get(HINT32 maxcount)
{
	return (HUINT32)(HLIB_MATH_Random() % maxcount);
}



static void _screensaver_UpdateWindow( void *userdata )
{
	ScreenSaver_Contents_t* pstContents = (ScreenSaver_Contents_t *) userdata;
	static HUINT32 ulTick =0;
	static HINT32	x=0,y=0,index=0; HUINT32 ulCurrentTick = HLIB_STD_GetSystemTick(),max_x =0,max_y =0;
	ONDK_Color_t colors[7] = { 0x00000000,0x19191919,0x33333333,0x66666666,0x99999999,0xCCCCCCCC,0xFFFFFFFF };

	if ((NULL != pstContents->pFontTitle) && (NULL != pstContents->pFontInfo))
	{
		if( abs(ulTick - ulCurrentTick) > 200)
		{
			if(abs(ulTick - ulCurrentTick) <= 1000 ) // 알파 증가
			{
				if(++index > 6)
				{
					index = 6;
				}
			}
			else if(abs(ulTick - ulCurrentTick) >= 6000 && abs(ulTick - ulCurrentTick) <= 8000) // 알파 감소
			{
				if(--index < 0)
				{
					index = 0;
				}
			}
			else if(abs(ulTick - ulCurrentTick) > 8000 && abs(ulTick - ulCurrentTick) < 8200) // 혹시 모를 텀..
			{
				index = 0;
			}
			else if(abs(ulTick - ulCurrentTick) >= 8200)
			{
				x = 0, y =0;
				if(pstContents->pszTitle)
				{
					x = MAX(x, ONDK_FONT_GetStringWidth(pstContents->pFontTitle,pstContents->pszTitle));
					HxLOG_Print("X[%d],%s -> [%d]\n",x, pstContents->pszTitle,ONDK_FONT_GetStringWidth(pstContents->pFontTitle,pstContents->pszTitle));
				}
				if(pstContents->pszInfo1)
				{
					x = MAX(x, ONDK_FONT_GetStringWidth(pstContents->pFontInfo,pstContents->pszInfo1));
					HxLOG_Print("X[%d],%s -> [%d]\n",x,pstContents->pszTitle,ONDK_FONT_GetStringWidth(pstContents->pFontInfo,pstContents->pszInfo1));
				}
				if(pstContents->pszInfo2)
				{
					x = MAX(x, ONDK_FONT_GetStringWidth(pstContents->pFontInfo,pstContents->pszInfo2));
					HxLOG_Print("X[%d],%s -> [%d]\n",x, pstContents->pszTitle,ONDK_FONT_GetStringWidth(pstContents->pFontInfo,pstContents->pszInfo2));
				}
				if(x >= GWM_SCREEN_WIDTH)
				{
					max_x = 75;
				}
				else
				{
					max_x = ( GWM_SCREEN_WIDTH - x - 150); // width 여백 75px
				}
				x = _screensaver_rand_get(max_x);
				x += 75;

				if(pstContents->pszTitle)
				{
					y = 42;
				}
				if(pstContents->pszInfo1)
				{
					y += 24;
				}
				if(pstContents->pszInfo2)
				{
					y += 24;
				}

				max_y = ( GWM_SCREEN_HEIGHT- y - 100); // hegith 여백 50px
				y = _screensaver_rand_get(max_y);
				y += 50;

				HxLOG_Print("X:[%d]Y[%d] max_x[%d],max_y[%d]\n",x,y,max_x,max_y);
				ulTick = ulCurrentTick;
			}
			else
			{
				index = 6; // 6초간 유지.
			}
		}

		if(index > 0)
		{
			if(pstContents->pszTitle)
			{
				ONDK_FONT_DrawString(ONDK_GWM_GetSurface(ONDK_UPPER_WINDOW),pstContents->pFontTitle,pstContents->pszTitle,
				x,y,colors[index], ONDKSTF_LEFT);
			}
			if(pstContents->pszInfo1)
			{
				ONDK_FONT_DrawString(ONDK_GWM_GetSurface(ONDK_UPPER_WINDOW),pstContents->pFontInfo,pstContents->pszInfo1,
					x,y + 30,colors[index], ONDKSTF_LEFT);
			}
			if(pstContents->pszInfo2)
			{
				ONDK_FONT_DrawString(ONDK_GWM_GetSurface(ONDK_UPPER_WINDOW),pstContents->pFontInfo,pstContents->pszInfo2,
					x,y + 60,colors[index], ONDKSTF_LEFT);
			}
		}

		ONDK_WINDOW_Update(ONDK_GWM_GetWindow(ONDK_UPPER_WINDOW),NULL);
	}
	else
	{
		HxLOG_Error("Error! Null Handle! pstContents->pFontTitle(0x%x), pstContents->pFontInfo(0x%x)\n", pstContents->pFontTitle, pstContents->pFontInfo);
	}
}

static HCHAR	s_szScreenSaverSQL[1024];

static HCHAR* _screensaver_query_for_service(HUINT32 ulUID)
{
	HINT32	sd;

	sd = HLIB_RWSTREAM_Open(s_szScreenSaverSQL, 1024);
	HLIB_RWSTREAM_Print(sd, "select * from DxService_t where  uid = %d  ", ulUID);
	HLIB_RWSTREAM_Close(sd);
	HxLOG_Print("SErvice Query)[%s]\n",s_szScreenSaverSQL);
	return s_szScreenSaverSQL;
}


static HCHAR* _screensaver_query_for_programme(HUINT32 ulUID)
{
	HINT32	sd;

	sd = HLIB_RWSTREAM_Open(s_szScreenSaverSQL, 1024);
	HLIB_RWSTREAM_Print(sd, "select * from eDxEPG_TYPE_PF where svcuid = %d ", ulUID);
	HLIB_RWSTREAM_Close(sd);
	HxLOG_Print("programe Query)[%s]\n",s_szScreenSaverSQL);
	return s_szScreenSaverSQL;
}

static void _screensaver_query_responed_for_service(const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata)
{
	DAxIterator_t	 		iter;
	DxService_t				*pService=NULL;
	HUINT32 				ulCount =0;
	ScreenSaver_Contents_t	*pContents = _screensaver_GetContents();
	HCHAR					pszTile[512] ={0,};
	HUINT32					ulViewId = 0;
	HUINT32					ulMajorCHNum = 0;

	HxLOG_Debug("\n\n[%s](%d) callback from DAPI\n", __FUNCTION__, __LINE__);

	/*
     *	현재 MainView 의 ChannelNumber를 가져온다.
	 */
	APK_MEDIA_PLAY_GetMainViewId (&ulViewId);
	APK_MEDIA_PLAY_GetMajorChannel (ulViewId, &ulMajorCHNum);

	VK_SEM_Get(s_ScreenSaverSemId);
	DAPI_InitIterator(&iter, data, size, 0);
	while (DAPI_NextIterator(&iter))
	{
		ulCount++;
		pService = (DxService_t *)DLIB_NewType(iter.name, iter.uid, iter.data, iter.size);
		if(pService)
		{
			HxSTD_snprintf(pszTile, 512, "%d %s", /*pService->lcn*/ulMajorCHNum ,pService->name);
			if(pContents->pszTitle)
			{
				if(HxSTD_StrCmp(pContents->pszTitle,pszTile) !=0 )
				{
					if(pContents->pszTitle)
					{
						AP_Free(pContents->pszTitle);
						pContents->pszTitle = AP_StrDup(pszTile);
					}
				}
			}
			else
			{
				pContents->pszTitle = AP_StrDup(pszTile);
			}

			//_screensaver_setAVCondition(FALSE);
			//GWMPOPUP_ScreenSaver_SetIsWorking(FALSE,eMctrlEvent_LIVE_ViewStateChanged);
			DLIB_FreeType(iter.name, pService);
			VK_SEM_Release(s_ScreenSaverSemId);
			return;
		}
	}
	VK_SEM_Release(s_ScreenSaverSemId);
}


static void _screensaver_query_responed_for_programme(const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata)
{
	DAxIterator_t	 		iter;
	DxEvent_t				*pEvent=NULL;
	ScreenSaver_Contents_t	*pContents = _screensaver_GetContents();
	HCHAR *eventname;

	HUINT64					ullCurrentTime;
	HxLOG_Print("\n\n[%s](%d) callback from DAPI\n", __FUNCTION__, __LINE__);
	VK_SEM_Get(s_ScreenSaverSemId);
	DAPI_InitIterator(&iter, data, size, 0);
	while (DAPI_NextIterator(&iter))
	{
		if(pContents->pszInfo2)
		{
			AP_Free(pContents->pszInfo2);
			pContents->pszInfo2 = NULL;
		}

		pEvent = (DxEvent_t *)DLIB_NewType(iter.name, iter.uid, iter.data, iter.size);
		if(pEvent)
		{
			ullCurrentTime = (HUINT64)HLIB_STD_GetSystemTime();
			HxLOG_Print("[%s]- C[%lld],S[%lld],Du[%lld]\n",pEvent->name, ullCurrentTime ,(HUINT64)pEvent->startTime,(HUINT64)pEvent->duration);
			if( ullCurrentTime >= (HUINT64)pEvent->startTime
				&& ullCurrentTime <= ((HUINT64)pEvent->startTime + (HUINT64)pEvent->duration )
				&& pEvent->name
				&& pContents->eMode == eMODE_RADIO)
			{
				eventname = ((DxEventString_t *)HLIB_VECTOR_First(pEvent->name))->string;
				if(pContents->pszInfo1)
				{
					if(HxSTD_StrCmp(pContents->pszInfo1,eventname)!= 0)
					{
						AP_Free(pContents->pszInfo1);
						pContents->pszInfo1 = AP_StrDup(eventname);
					}
				}
				else
				{
					pContents->pszInfo1 = AP_StrDup(eventname);
				}

				DLIB_FreeType(iter.name, pEvent);
				VK_SEM_Release(s_ScreenSaverSemId);
				return;
			}
			DLIB_FreeType(iter.name, pEvent);
		}
	}
	VK_SEM_Release(s_ScreenSaverSemId);
}


static HUINT32 _screensaver_getMasterServiceUID(void)
{
	HUINT32		ulViewId;
	HINT32	uid;
	OxMediaPlay_MediaType_e eType;

	APK_MEDIA_PLAY_GetMainViewId(&ulViewId);
	APK_MEDIA_PLAY_GetPlayType ( ulViewId, &eType);
	APK_MEDIA_PLAY_GetRequestId( ulViewId, eType, &uid);

	return uid;
}

static void _screensaver_getservice(void)
{
	HCHAR							 *pszQuery;

	pszQuery = _screensaver_query_for_service(_screensaver_getMasterServiceUID());
	if (DAPI_Query(pszQuery, _screensaver_query_responed_for_service, NULL) != DERR_OK)
	{
		HxLOG_Error("pszQuery Error [%s]\n",pszQuery);
		return;
	}
}

static void _screensaver_getprogramme(void)
{
	HCHAR *pszQuery;
	pszQuery = _screensaver_query_for_programme(_screensaver_getMasterServiceUID());
	if (DAPI_Query(pszQuery, _screensaver_query_responed_for_programme, NULL) != DERR_OK)
	{
		HxLOG_Error("pszQuery Error [%s]\n",pszQuery);
		return;
	}
}



static HBOOL _screensaver_music(HCHAR *pszFileName)
{
	HCHAR *pszExtension;

	pszExtension = HxSTD_StrRChar(pszFileName, '.');
	if (pszExtension)
	{
		if(HLIB_STD_StrCaseCmp(".mp3",pszExtension) == 0)
		{
			return TRUE;
		}

	}
	return FALSE;
}

static void _screensaver_get_musicinfo(void)
{
	HCHAR 						pszURI[2048]={0,},*pszFilePath,*p;
	ScreenSaver_Contents_t 		*pContents = _screensaver_GetContents();
	HUINT32		ulViewId;

	APK_MEDIA_PLAY_GetMainViewId(&ulViewId);

	if(APK_MEDIA_PLAY_GetURI(ulViewId,pszURI)!= ERR_OK)
	{
		return;
	}

	if(HxSTD_StrLen(pszURI))
	{
		HUINT32 ulLen;
		p =  pszURI;
		ulLen =	HxSTD_StrLen(URI_STARTPATH) -1;
		p += ulLen;
		pszFilePath = AP_StrDup(p);
		HxLOG_Print("Playing...URI[%s]->[%s]\n", pszURI,pszFilePath );

		if(_screensaver_music(pszFilePath))
		{
			HxID3_MusicInfo_t		*pstMusicInfo = NULL;

			pstMusicInfo = (HxID3_MusicInfo_t*)AP_Malloc(sizeof(HxID3_MusicInfo_t));
			if (HLIB_ID3_GetID3MusicInfo(pszFilePath, pstMusicInfo) == ERR_OK)
			{
				if(pContents->pszTitle)
				{
					if(HxSTD_StrCmp(pContents->pszTitle, pstMusicInfo->szTitle) !=0)
					{
						AP_Free(pContents->pszTitle);
						pContents->pszTitle = AP_StrDup(pstMusicInfo->szTitle);
					}
				}
				else
				{
					pContents->pszTitle = AP_StrDup(pstMusicInfo->szTitle);
				}


				if(pContents->pszInfo1)
				{
					if(HxSTD_StrCmp(pContents->pszInfo1, pstMusicInfo->szTitle) !=0)
					{
						AP_Free(pContents->pszInfo1);
						pContents->pszInfo1 = AP_StrDup(pstMusicInfo->szTitle);
					}
				}
				else
				{
					pContents->pszInfo1 = AP_StrDup(pstMusicInfo->szTitle);
				}

				if(pContents->pszInfo2)
				{
					if(HxSTD_StrCmp(pContents->pszInfo2, pstMusicInfo->szAlbum) !=0)
					{
						AP_Free(pContents->pszInfo2);
						pContents->pszInfo2 = AP_StrDup(pstMusicInfo->szAlbum);
					}
				}
				else
				{
					pContents->pszInfo2 = AP_StrDup(pstMusicInfo->szAlbum);
				}

				HxLOG_Print("Music Info...szTitle[%s],szArtist[%s]->szAlbum[%s]\n",pstMusicInfo->szTitle,pstMusicInfo->szArtist,pstMusicInfo->szAlbum );
			}

			if (pstMusicInfo)
			{
				AP_Free(pstMusicInfo);
			}
		}
		AP_Free(pszFilePath);
	}
}

static void _screensaver_setStartInfo(void)
{
	ScreenSaver_Contents_t *pstContents = _screensaver_GetContents();

	ONDK_GWM_SetTimer(GWMPOPUP_TIMER_SCREENSAVER_GETSERVIE,5000); // service 를 가져오자.

	if(pstContents->eMode == eMODE_LIVE)
	{
		ONDK_Size_t size;
		ONDK_Rect_t  Array[3] = { {-128,60,0,0},{150,180,0,0},{733,79,0,0}};
		pstContents->video = ONDK_VIDEO_Create(SCREENSAVER_LIVE_PATH);
		ONDK_Assert(pstContents->video);
		size	= ONDK_IMG_NonCreateOnlyGetSize(SCREENSAVER_LIVE_PATH);

		pstContents->drawRect.x = Array[0].x;
		pstContents->drawRect.y = Array[0].y;
		pstContents->drawRect.w = size.w;
		pstContents->drawRect.h = size.h;
		ONDK_GWM_SetTimer(GWMPOPUP_TIMER_SCREENSAVER,10000); // live gif 는 3개의 좌표로 이동됨
	}
	else
	{
		pstContents->drawRect.x = 0;
		pstContents->drawRect.y = 0;
		pstContents->drawRect.w = GWM_SCREEN_WIDTH;
		pstContents->drawRect.h = GWM_SCREEN_HEIGHT;
		pstContents->video = ONDK_VIDEO_Create(SCREENSAVER_MEDIA_PATH);
		ONDK_Assert(pstContents->video);
	}
}


static void	_screensaver_start(ScreenSaver_Contents_t *pstContents)
{
	if (pstContents->video)
	{
		if (ONDK_VIDEO_IsBusy(pstContents->video) == ONDK_RET_OK)
		{
			//	Close Video Provider for restarting video from begin
			ONDK_VIDEO_Release(pstContents->video);
			pstContents->video = NULL;

		}
		else
		{
			return;
		}
	}

	if(pstContents->pFontTitle)
	{
		ONDK_FONT_Release(pstContents->pFontTitle);
	}
	if(pstContents->pFontInfo)
	{
		ONDK_FONT_Release(pstContents->pFontInfo);
	}

	pstContents->pFontTitle = NULL;
	pstContents->pFontInfo = NULL;


	_screensaver_setStartInfo();

	pstContents->pFontTitle = ONDK_FONT_CreateSystemFont(eFont_SystemBold,FONT_TITLE_SIZE);
	pstContents->pFontInfo = ONDK_FONT_CreateSystemFont(eFont_SystemNormal,FONT_INFO_SIZE);

	if(pstContents->video)
	{
		ONDK_VIDEO_SetSpeed(pstContents->video,1.0); // 느리다고 요구가 들어온다면. 이미지를 교체해야 한다. 소프트 웨어적으로는 제대로 동작되지 않는다.3
		if(pstContents->eMode == eMODE_LIVE)
		{
			ONDK_VIDEO_PlayOnece(pstContents->video, ONDK_GWM_GetSurface(ONDK_UPPER_WINDOW), &pstContents->drawRect, (void *)_screensaver_UpdateWindow, pstContents);
		}
		else
		{
			ONDK_VIDEO_PlayLoop(pstContents->video, ONDK_GWM_GetSurface(ONDK_UPPER_WINDOW), &pstContents->drawRect, (void *)_screensaver_UpdateWindow, pstContents);
		}
	}
}


static ONDK_Result_t _screensaver_MsgGwmCreate(ScreenSaver_Contents_t	*pContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		hRet = ONDK_RET_OK;

	if ( VK_SEM_Create(&s_ScreenSaverSemId, SCREENSAVER_SEMA_NAME, VK_SUSPENDTYPE_PRIORITY ) != VK_OK )
	{
		return ONDK_RET_FAIL;
	}

	hRet = _screensaver_init(pContents);
	ONDK_GWM_APP_SetAppArea(0, 0, GWM_SCREEN_WIDTH, GWM_SCREEN_HEIGHT);

	pContents->eMode 	= (ScreenSaver_Mode_e)p1;

	if(pContents->eMode == eMODE_RADIO)
	{
/*
 * author : hsseo
 * note : PVR contents의 event name을 ScreenSaver의 pszTitle로 설정해야 하는데
 		  현재 event name을 아래로 얻으면 null이 나와서 추구 구현이 필요하다.
 */
#if 0
		APKE_MediaCtrlType_e eMainActionType;
		APKE_MediaCtrl_Destination_e eMainDestination;

		eMainDestination = APK_MEDIACTRL_GetMainViewDestination();
		eMainActionType = APK_MEDIACTRL_GetCurrentMediaType(eMainDestination);

		if (eMctrlBase_PvrPlayback == eMainActionType )
		{
			/*
			 * author : hsseo
			 * note : ScreenSaver 에 event name을 넣어 준다.
			 */
			HERROR hErr;
			DxEvent_t stEventInfo;

			HxSTD_MemSet(&stEventInfo, 0, sizeof(stEventInfo));
			hErr = APK_MEDIACTRL_USRPB_EventInfo(eMainDestination, &stEventInfo);
			if( ERR_OK == hErr )
			{
				if( NULL != pContents->pszTitle )
				{
					AP_Free(pContents->pszTitle);
					pContents->pszTitle = NULL;
				}

				/*no need sem.*/
				pContents->pszTitle = AP_StrDup(stEventInfo.name);
			}
		}
		else
#endif
		{
			_screensaver_getservice();
			_screensaver_getprogramme();
		}
	}
	else if(pContents->eMode == eMODE_MUSIC)
	{
		_screensaver_get_musicinfo();
	}

	_screensaver_start(pContents);
	if (HAPI_BroadcastSignal("signal:onScreenSaverStateChanged", 1, "i", 1) != ERR_OK)
	{
		HxLOG_Error("Error> failed to HAPI_BroadcastSignal(onScreenSaverStateChanged:1)\n");
	}

	return hRet;
}

static ONDK_Result_t _screensaver_MsgGwmDestroy(ScreenSaver_Contents_t	*pContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_GWM_KillTimer(GWMPOPUP_TIMER_SCREENSAVER);

	if (pContents->video)
	{
		ONDK_VIDEO_Stop(pContents->video);
		ONDK_VIDEO_Release(pContents->video);
	}

	if(pContents->pFontTitle)
	{
		ONDK_FONT_Release(pContents->pFontTitle);
	}
	if(pContents->pFontInfo)
	{
		ONDK_FONT_Release(pContents->pFontInfo);
	}

	pContents->pFontTitle = NULL;
	pContents->pFontInfo = NULL;

	ONDK_SURFACE_Clear(ONDK_GWM_GetSurface(ONDK_UPPER_WINDOW));
	ONDK_WINDOW_Update(ONDK_GWM_GetWindow(ONDK_UPPER_WINDOW), NULL);

	if (HAPI_BroadcastSignal("signal:onScreenSaverStateChanged", 1, "i", 0) != ERR_OK)
	{
		HxLOG_Error("Error> failed to HAPI_BroadcastSignal(onScreenSaverStateChanged:0)\n");
	}

	pContents->video = NULL;
	VK_SEM_Destroy(s_ScreenSaverSemId);
	return GWM_RET_OK;
}


//MSG_GWM_TIMER
static ONDK_Result_t  _screensaver_MsgGwmTimer(ScreenSaver_Contents_t *pContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32 ulTimerID = p1;
	static HINT32 index = 1;
	ONDK_Size_t size;
	ONDK_Rect_t  Array[3] = { {-128,60,0,0},{150,180,0,0},{733,79,0,0}};
	size	= ONDK_IMG_NonCreateOnlyGetSize(SCREENSAVER_LIVE_PATH);

	switch(ulTimerID)
	{
		case GWMPOPUP_TIMER_SCREENSAVER:
			HxLOG_Debug("GWMPOPUP_TIMER_SCREENSAVER : %d\r\n",ulTimerID);
			ONDK_VIDEO_Stop(pContents->video);
			ONDK_VIDEO_Release(pContents->video);
			pContents->video = NULL;
			pContents->video = ONDK_VIDEO_Create(SCREENSAVER_LIVE_PATH);
			pContents->drawRect.x = Array[index].x;
			pContents->drawRect.y = Array[index].y;
			pContents->drawRect.w = size.w;
			pContents->drawRect.h = size.h;

			ONDK_VIDEO_PlayOnece(pContents->video, ONDK_GWM_GetSurface(ONDK_UPPER_WINDOW), &pContents->drawRect, (void *)_screensaver_UpdateWindow, pContents);
			index++;

			if(index > 2)
			{
				index = 0;
			}
			break;
		case GWMPOPUP_TIMER_SCREENSAVER_GETSERVIE:
			if(pContents->eMode == eMODE_RADIO)
			{
				_screensaver_getservice();
				_screensaver_getprogramme();
			}
			else if(pContents->eMode == eMODE_MUSIC)
			{
				_screensaver_get_musicinfo();
			}
			ONDK_GWM_ResetTimer(GWMPOPUP_TIMER_SCREENSAVER_GETSERVIE);
			break;
	}
	return GWM_RET_OK;
}



static ONDK_Result_t local_screensaver_APKMessage(ScreenSaver_Contents_t	*pContents, Handle_t hAct,HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t					 ret = GWM_RET_OK;
//	GWM_APK_MSG_Type_e	eAPK_Type = (GWM_APK_MSG_Type_e)hAct;

#if 0
	if(eAPK_Type == GWM_APK_MEDIA)
	{
		PrismApkEventParam_t	*pParam =	(PrismApkEventParam_t*)p3;

		if( NULL == pParam )
			return ret;

		switch (pParam->eEvent)
		{
			default:
				ret  =GWM_RET_OK;
				break;
		}
		if(ret == GWM_MESSAGE_BREAK)
		{

			if( NULL != pParam )
				AP_Free(pParam);
		}
	}
#endif
	return ret;
}


/**************************************************************************************************/
#define _________SCREEN_SAVER_Public_Functions____________________________________________
/**************************************************************************************************/
ONDK_Result_t		AP_Screensaver_Proc(HINT32 lMessage, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{

	ONDK_Result_t			lRet = GWM_RET_OK;
	ScreenSaver_Contents_t	*pContents = _screensaver_GetContents();
	ONDK_Print(HxANSI_COLOR_YELLOW("[%s:%d] Message(%d) : %s  p1(0x%x) p2(0x%x) p3(0x%x) \n"), __FUNCTION__, __LINE__, lMessage, ONDK_GWM_GwmMsg2String(lMessage), p1, p2, p3);

	switch(lMessage)
	{
		case MSG_GWM_CREATE:
			lRet = _screensaver_MsgGwmCreate(pContents, p1, p2, p3);
			break;
		case MSG_GWM_TIMER:
			lRet = _screensaver_MsgGwmTimer(pContents, p1, p2, p3);
			break;
		case MSG_GWM_KEYDOWN:
			ONDK_GWM_APP_Destroy(0);
			if(p1 != KEY_STANDBY)
				lRet =  ONDK_MESSAGE_CONSUMED;
			break;

		case MSG_GWM_DESTROY:
			lRet = _screensaver_MsgGwmDestroy(pContents, p1, p2, p3);
			break;
#if 0
		case MSG_GWM_HAPI_MESSAGE:
			{
				HSIGNAL_e eSignal = (HSIGNAL_e)hHandle;
				switch(eSignal)
				{
					case eSSIG_InputNotify:
						ONDK_GWM_APP_Destroy(0);
						lRet =  ONDK_MESSAGE_BREAK;
						break;

	#if	0		//	screen saver의 disable이 되어야 하는 것이지 전체를 끝내면 안됨.
					case eHSIG_GoShutDown:
					case eHSIG_GoReboot:
						pContents->bExit = TRUE;
						break;
	#endif
					default:
						break;
				}
			}
			break;
#endif
		case MSG_GWM_APK_MESSAGE:
			lRet = local_screensaver_APKMessage(pContents,hHandle, p1, p2, p3);
			break;
	}

	if(lRet != GWM_RET_OK)
	{
		return lRet;
	}

	return ONDK_GWM_ProcessMessageDefault(lMessage, hHandle, p1, p2, p3);
}



/**************************************************************************************************/
#define _________SCREEN_SAVER_End_Of_File_________________________________________________
/**************************************************************************************************/


