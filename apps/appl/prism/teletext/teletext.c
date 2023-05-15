
/*******************************************************************/
/********************      Header Files    		*************************/
/*******************************************************************/
#include <teletext.h>
#include <vkernel.h>
#include <hapi.h>
#include <ttx_errchk.h>

#ifndef AUTOCONF_INCLUDED
#include <autoconf.h>
#endif


/*******************************************************************/
/********************      Defines         			*************************/
/*******************************************************************/

// TTX PES 패킷 관련 상수
#define TTX_PES_PRIVATE_STREAM_1		0xBD
#define TTX_PES_HEADER_DATA_LENGTH		0x24
#define TTX_PES_DATA_UNIT_LENGTH		0x2C
#define TTX_PES_EBU_TELETEXT			0x02
#define TTX_PES_EBU_SUBTITLE			0x03
#define TTX_PES_DATA_FIELD_INDEX		45
#define TTX_PES_DATA_UNIT_FIELD_SIZE	46



// Teletext Decoder Memory Size
// 모델 별 Resource 가용 여부에 따라 설정 한다.
#if defined (CONFIG_PROD_NASD10T) || defined(CONFIG_PROD_VA5SD)
#define	TTX_MEMORY_SIZE					(256 * 1024)
#else
#define	TTX_MEMORY_SIZE					(1024 * 1024)
#endif

#ifdef CONFIG_PROD_PROFILE_MINIOCTO
#define MW_TLTXT_MSGQ_SIZE				64
#else
#define MW_TLTXT_MSGQ_SIZE				256
#endif

#define	TTX_CYCLE_TERM			200

#if 0
#if defined(CONFIG_MWC_OSD_SBTL_ON_UIPLANE)
#define	TTX_CYCLE_TERM			500
#else
#define	TTX_CYCLE_TERM			300
#endif
#endif
#define	TTX_CYCLE_SEARCH(cycle)		((cycle) % 1 == 0)
#define	TTX_CYCLE_TIME(cycle)		((cycle) % 2 == 0)
#define	TTX_CYCLE_FLASH(cycle)		((cycle) % 2 == 0)

/**@brief : 추가로 사용되는 Teletext Error Type */
enum
{
	ERR_CONTINUE	= 0x00008000,
	ERR_BREAK
};

#define	TTX_SUBCODE_BCD_TO_DEC(bcd)	(\
										(((bcd) >> 12) & 0xF) * 1000 \
									  + (((bcd) >>  8) & 0xF) * 100  \
									  + (((bcd) >>  4) & 0xF) * 10 	 \
									  + (((bcd) >>  0) & 0xF) * 1	 \
									)
#define	TTX_SUBCODE_DEC_TO_BCD(dec)	(\
										((((dec) / 1000) % 10) << 12)\
									  + ((((dec) / 100 ) % 10) << 8) \
									  + ((((dec) / 10  ) % 10) << 4) \
									  + ((((dec) / 1   ) % 10) << 0) \
									)


#if defined(CONFIG_MWC_OSD_SBTL_ON_UIPLANE)
static HULONG			s_ulTtxCtxSema;

#define	TTX_CTX_SEM_GET		VK_SEM_Get(s_ulTtxCtxSema)
#define	TTX_CTX_SEM_RELEASE	VK_SEM_Release(s_ulTtxCtxSema)
#else
#define	TTX_CTX_SEM_GET
#define	TTX_CTX_SEM_RELEASE
#endif

/*******************************************************************/
/********************      Typedef        		 	 *************************/
typedef struct
{
	HUINT8			*pageBuf;
	HUINT32			bufSize;
} ttxpage_dump;
ttxpage_dump gttxdump;


typedef enum
{
	eTTX_UPDATE_None,
	eTTX_UPDATE_PageUpdated,
	eTTX_UPDATE_SubPageChanged,
	eTTX_UPDATE_PageChanged,
	eTTX_UPDATE_LAST
} TTX_Update_t;


/** @brief Context 내용 */
typedef struct
{
	TTX_Status_t		status;			/**< Status */
	HUINT16				usPageNum;		/**< 현재 필터링 되고 있는 페이지 넘버 */
	HUINT16				usSubcode;		/**< 현재 필터링 되고 있는 서브 페이지 넘버 */

	TTX_Page_t	*		pTtxPage;		/**< 현재 그리는 페이지 */

	TTX_Controller_t	controller;		/**< Teletext Controller */

	HBOOL				bEbuSbtl;		/**< EBU subtitle 만을 위한 동작 시  */
	HBOOL				bVariableFont;	/**< EBU subtitle display시 Font width 적용 방법 지정*/
	TTX_Control_t		control;		/**< 마지막 설정 값 저장. */
	HUINT32				ulArg;			/**< 마지막 설정 값 저장. */

	HUINT32				ulTimerId_Cycle;
} TTX_Context_t;


typedef struct
{
    HUINT32             ulCmd;
//	Handle_t			hAction;			/**< Command를 발행한 action의 handle */
//	HUINT16				usPid;				/**< Teletext PID */
//	TTX_Type_t			eType;
} TTX_StartMsg_t;


typedef struct
{
    HUINT32             ulCmd;
	Handle_t			hAction;			/**< Command를 발행한 action의 handle */
	//ApiSyncMode_t		syncMode;			/**< Command를 발행한 API의 리턴 형식을 sync/async로 지정 */
} TTX_StopMsg_t;


typedef struct
{
    HUINT32             ulCmd;
	Handle_t			hAction;			/**< Command를 발행한 action의 handle */
	HUINT16				usPageNum;
	HUINT16				usSubcode;
	HBOOL				bSbtl;
	HBOOL				bVariableFont;
	HBOOL				bInitPage;
} TTX_ShowMsg_t;


typedef struct
{
    HUINT32             ulCmd;
	Handle_t			hAction;			/**< Command를 발행한 action의 handle */
	//ApiSyncMode_t		syncMode;			/**< Command를 발행한 API의 리턴 형식을 sync/async로 지정 */
} TTX_HideMsg_t;


typedef struct
{
	HUINT32				ulCmd;
	Handle_t			hAction;
	TTX_Control_t		control;
	HUINT32				ulArg;
	//ApiSyncMode_t		syncMode;			/**< Command를 발행한 API의 리턴 형식을 sync/async로 지정 */
	HUINT32				ulVersion;
} TTX_ControlMsg_t;


typedef struct
{
	HUINT32				ulCmd;
	HBOOL				bVariableFont;
} TTX_SetFontMsg_t;


typedef struct
{
	HUINT32				ulCmd;
	Handle_t			hAction;
	TTX_Refresh_t		refresh;
	HUINT32				ulVersion;
} TTX_RefreshMsg_t;


typedef struct
{
    HUINT32             ulCmd;
	HUINT32				ulMasterActionId;	/**< New Master action ID */
} TTX_SetMasterMsg_t;


typedef struct
{
	HUINT32				ulCmd;				/**< Command. union상에서의 공통 위치. */
	Handle_t			hAction;			/**< Command를 발행한 action의 handle */
	HBOOL				bOffDec;			/**< Decoder On/Off Flag. */
} TTX_SetOnOffDec_t;


typedef struct
{
	HUINT32				ulCmd;
	Handle_t			hAction;
	TTX_Update_t		update;
} TTX_UpdateMsg_t;


typedef struct
{
	HUINT32				ulCmd;
	Handle_t			hAction;
} TTX_CycleMsg_t;


typedef union
{
	HUINT32					ulCmd;
	TTX_StartMsg_t			startMsg;
	TTX_StopMsg_t			stopMsg;
	TTX_ShowMsg_t			showMsg;
	TTX_HideMsg_t			hideMsg;
	TTX_ControlMsg_t		controlMsg;
	TTX_SetFontMsg_t		setFontMsg;
	TTX_RefreshMsg_t		refreshMsg;
	TTX_UpdateMsg_t			updateMsg;
	TTX_CycleMsg_t			cycleMsg;
	TTX_SetMasterMsg_t		setMasterMsg;
	TTX_SetOnOffDec_t		setOnOffDec;
} TTX_Msg_t;

/**@brief : Teletext service command */
enum
{
	CMD_TTX_START = 100,		/**< Teletext 시작을 요청하는 명령. Master action이 아니면 보관만 된다. */
	CMD_TTX_STOP,				/**< Teletext stop을 요청하는 명령 */
	CMD_TTX_SHOW,
	CMD_TTX_HIDE,
	CMD_TTX_CHANGE_PID,			/**< PID 변경을 요청하는 명령 */
	CMD_TTX_SET_MASTER,			/**< Master action을 지정하는 명령 */
	CMD_TTX_SET_CONTROL,
	CMD_TTX_SET_FONT,
	CMD_TTX_ON_OFF_DEC,
	CMD_TTX_END,				/**< enum 마지막 표시. 명령 아님. */

	TTXM_PAGE_REFRESH,
	TTXM_PAGE_UPDATED,
	TTXM_CYCLE
};


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static HULONG			s_ulTtxDecSema;						/**< Decoder 세마포	*/
static HULONG			s_ulTtxMsgQId;						/**< TTX Message Queue ID */
static HULONG			s_ulTtxTaskId;						/**< TTX Task ID */
static Handle_t			s_hTtxDecoder	= HANDLE_NULL;		/**< TTX Decoder Handle	*/
static HUINT8	*		s_pucTtxMemory;						/**< 미리 할당한 TTX 디코더 메모리 		*/
static TTX_Context_t	s_stTtxContext;						/**< TTX Context. */
static TTX_Page_t		s_ttxPage;							/**< TTX_Page */
static HUINT32			s_ulTtxCycle;						/**< 100ms 반복 타이머 사이클 **/
static HUINT32			s_ulControlVersion;					/**< Control에 Version을 넣어 가장 최신 버전의 Control만 수행 */
static HUINT32			s_ulRefreshVersion;					/**< Refresh에 Version을 넣어 가장 최신 버전의 Refresh만 수행 */
static TTX_Refresh_t	s_eRefresh;							/**< Version Control 되고 있는 Refresh...??? ㅡㅡ;;	*/
ONDKWindowHandle		g_hTeletextWinHandle = NULL;
static HBOOL			s_bVariableFont = FALSE;
//static HBOOL			m_ISEBUSubtilePES;
/*******************************************************************/
/********************     Local Functions   		*************************/
/*******************************************************************/
#define _________LOCAL_FUNCTIONS_____________
/**
 * 메시지 큐 아이디를 얻는다.
 *
 * @return	MsgQId
 */
static INLINE HUINT32 local_ttx_GetTltxtMsgQId(void)
{
	return s_ulTtxMsgQId;
}




/**
 * Action Id에 해당되는 컨텍스트를 얻는다.
 *
 * @param	ulActId		액션 아이디
 * @return	컨텍스트
 */
static INLINE TTX_Context_t * local_ttx_GetContext (void)
{
	TTX_Context_t *pCtx	= &s_stTtxContext;
	if (pCtx->pTtxPage == NULL)
	{
		pCtx->pTtxPage = &s_ttxPage;
	}
    return pCtx;
}


static void	local_ttx_dump_page (char *buf)
{
	char 	i, count;
	int		j;

	HLIB_CMD_Printf("			magic_code: %c\n", buf[0]);
	HLIB_CMD_Printf("			   version: %d\n", buf[1]);
	HLIB_CMD_Printf("	  number of packet: %d\n", buf[2]);
	HLIB_CMD_Printf("	  page number(MXX): %X%02X\n", buf[3], buf[4]);
	HLIB_CMD_Printf("			   subcode: 0x%02X%02X\n", buf[5], buf[6]);
	HLIB_CMD_Printf("	   national option: %d\n", buf[7]);
	HLIB_CMD_Printf("		  control bit : 0x%02X (C11-C4) {\n", buf[8]);
	HLIB_CMD_Printf("				  Erase Page: %c\n", (buf[8] & 0x01) ? 'O' : 'X');
	HLIB_CMD_Printf("				   Newsflash: %c\n", (buf[8] & 0x02) ? 'O' : 'X');
	HLIB_CMD_Printf("					Subtitle: %c\n", (buf[8] & 0x04) ? 'O' : 'X');
	HLIB_CMD_Printf("			 Suppress Header: %c\n", (buf[8] & 0x08) ? 'O' : 'X');
	HLIB_CMD_Printf("			Update Indicator: %c\n", (buf[8] & 0x10) ? 'O' : 'X');
	HLIB_CMD_Printf("		   Interruped Seq...: %c\n", (buf[8] & 0x20) ? 'O' : 'X');
	HLIB_CMD_Printf("			 Inhibit Display: %c\n", (buf[8] & 0x40) ? 'O' : 'X');
	HLIB_CMD_Printf("			 Magazine Serial: %c\n", (buf[8] & 0x80) ? 'O' : 'X');
	HLIB_CMD_Printf("		  }\n");
	HLIB_CMD_Printf("			  has M/29: %s\n",buf[9] ? "true" : "false");

	if (buf[9])
	{
		HLIB_CMD_Printf( "	 character set code: 0x%02X\n", buf[10]);
		HLIB_CMD_Printf( "2nd character set code: 0x%02X\n", buf[11]);
		HLIB_CMD_Printf( "			  row color: %d\n", buf[12]);
		HLIB_CMD_Printf( "		   screen color: %d\n", buf[13]);
	}
	HLIB_CMD_Printf( " 			link type: %s\n", buf[20] == 'T' ? "TOP" : buf[20] == 'F' ? "FLOF" : "default");
	for (i = 0 ; i < 5 ; i++)
	{
		HLIB_CMD_Printf( "			link[%d] %X%02X-%02X%02X\n", i, buf[21+i*4], buf[22+i*4], buf[23+i*4], buf[24+i*4]);
	}
	HLIB_CMD_Printf("\n");
	HLIB_CMD_Printf("		0		  5 	   10		 15 	   20		 25 	   30		 35 	 39\n");

	count = buf[2];
	buf  += 41;

	for (i = 0 ; i < count ; i++)
	{
		HLIB_CMD_Printf("%5d: ", *buf++);
		for (j = 0 ; j < 40 ; j++)
			HLIB_CMD_Printf("%02X", buf[j]);

		HLIB_CMD_Printf(" ; ");
		for (j = 0 ; j < 40 ; j++)
		{
			if (isascii(buf[j]))
				HLIB_CMD_Printf("%c", buf[j]);
			else
				HLIB_CMD_Printf(".");
		}
		buf += 40;
		HLIB_CMD_Printf("\n");
	}

}

static HERROR	local_ttx_dump (HUINT16 usPageNum, HUINT16 usSubcode)
{
	HUINT32 		size;

	HxLOG_Trace();
	HxLOG_Print("Teletext_Dump page[%d]subcode[%d]\n",(int)usPageNum, (int)usSubcode);
	if (gttxdump.pageBuf == NULL)
	{
		gttxdump.bufSize = (1/* packet number */ + 40 /*packet size*/) * 40 /*max packet in page*/;
		gttxdump.pageBuf = (HUINT8 *)AP_Calloc(gttxdump.bufSize);
		if (gttxdump.pageBuf == NULL)
			return ERR_FAIL;
	}

	size = TTX_DECODER_GetPage(s_hTtxDecoder, usPageNum, usSubcode, gttxdump.pageBuf, gttxdump.bufSize);
	if (size == 0)
	{
		HxLOG_Error("Cannot get teletext page: %03d-0x%X\n", (int)usPageNum, (int)usSubcode);
		return ERR_FAIL;
	}

 	local_ttx_dump_page(gttxdump.pageBuf);

	return ERR_OK;
}

/**
 *
 */
static void	local_ttx_Invalidate (TTX_Refresh_t refresh)
{
	ONDK_Rect_t rc;
	TTX_Controller_t	*pCtrl ;
	TTX_Context_t		*pContext	= NULL;
	HBOOL				bRollingHeader	= FALSE;
	if(refresh == eTTX_REFRESH_NONE)		return;
	if(g_hTeletextWinHandle == NULL) 		return;



	pContext 	= local_ttx_GetContext();
	pCtrl =&pContext->controller;
	TTX_BITSET(s_eRefresh, refresh);
//	++s_ulRefreshVersion;

	// Search Mode에선 Rolling Header!!
	bRollingHeader	= (pCtrl->state & eTTX_STATE_SEARCH) ? TRUE : FALSE;
	(void)bRollingHeader;

	/**
	 * 화면을 가로로 아래와 같이 네 등분 하여 Invalidate를 한다.
	 * TTX는 Variable Font 및 Double Height, Double Width 등의 속성으로 인해 다 그려보기 전 까지는
	 * 그려질 영역을 예측하기 어렵기 때문에, 네 등분으로 나눠 화면을 갱신한다.
	 *
	 * +---------------------------------+
	 * |		  HEADER AREA			 |
	 * |								 |
	 * +---------------------------------+
	 * |								 |
	 * |			  BODY				 |
	 * +---------------------------------+
	 * |			  BODY				 |
	 * |								 |
	 * +---------------------------------+
	 * |								 |
	 * |	ROW24 & Subcode Bar AREA	 |
	 * +---------------------------------+
	 */

	// Default Refresh ALL
	rc.x =0 ;
	rc.y =0 ;
	rc.w = TELETEXT_SCREENWIDTH;
	rc.h = TELETEXT_SCREENHEIGHT;

	// only HEADER AREA refresh
	if ((refresh & ~(eTTX_REFRESH_HEADER)) == 0)
	{
		rc.h /= 4;
	}
	//ONDK_WINDOW_Update(m_hwindow,&rc);

	//(void) TTX_DECODER_Rendering(s_hTtxDecoder, pContext->pTtxPage, bRollingHeader, &pContext->controller);

}



/**
 * 화면을 갱신하라고 디코더에 요청한다.
 *
 * @param	pContext	컨텍스트
 * @param	refresh		리프레쉬 타입 (CONTROL_PANEL, HEADER, ALL)
 * @param	ulVersion	Version
 * @return	ERR_OK
 */
static HERROR	local_ttx_RefreshPage		(TTX_Context_t *pContext, TTX_Refresh_t refresh, HUINT32 ulVersion)
{
	HBOOL				bRollingHeader	= FALSE;
	TTX_Controller_t	*pCtrl		= &pContext->controller;

	if (ulVersion != s_ulRefreshVersion)
	{
		HxLOG_Print("\tThis MSG(%X) is older then current version(%X)\n", ulVersion, s_ulRefreshVersion);
		return ERR_FAIL;
	}
	pCtrl->refresh	= refresh;

	if (pCtrl->state & eTTX_STATE_EBU_SUBTITLE)
	{
		if (pContext->pTtxPage->bSubtitle == TRUE)
		{
			// do nothing...
		}
		else if (pContext->pTtxPage->bNewsflash == TRUE)
		{
			// Newsflash는 Var Font를 사용하면 안되므로, 아래와 같이 Var Font 속성을 임시로 제거하고
			// 그림을 그린 후 다시 복구한다.
			TTX_BITCLR(pCtrl->state, eTTX_STATE_VAR_FONT);
		}
		else	// Ebu Subtitle Mode이면서 Subtitle이나 Newsflash가 아니면, FALSE
		{
			return ERR_FAIL;
		}
	}
	// Search Mode에선 Rolling Header!!
	bRollingHeader	= (pCtrl->state & eTTX_STATE_SEARCH) ? TRUE : FALSE;
	if (!(pCtrl->state & eTTX_STATE_TAKE_SCREEN))
	{
		TTX_BITSET(pCtrl->state, eTTX_STATE_TAKE_SCREEN);
		pCtrl->refresh	= eTTX_REFRESH_ALL;
	}
	VK_SEM_Get(s_ulTtxDecSema);
	(void) TTX_DECODER_Rendering(s_hTtxDecoder, pContext->pTtxPage, bRollingHeader, &pContext->controller);
	VK_SEM_Release(s_ulTtxDecSema);

	s_eRefresh	= eTTX_REFRESH_NONE;

	if (pCtrl->state & eTTX_STATE_EBU_SUBTITLE && pContext->bVariableFont == TRUE)
	{
		TTX_BITSET(pCtrl->state, eTTX_STATE_VAR_FONT);
	}

	return ERR_OK;
}

static HERROR	local_ttx_UpdatePage		(TTX_Context_t *pContext, TTX_Update_t update)
{
	TTX_Controller_t	*pCtrl	= &pContext->controller;

	if (update == eTTX_UPDATE_None)
	{
		return ERR_FAIL;
	}

	local_ttx_Invalidate( eTTX_REFRESH_ALL);

	if (update >= eTTX_UPDATE_PageUpdated)
	{
	}

	if (update >= eTTX_UPDATE_SubPageChanged)
	{
		if (pCtrl->state & eTTX_STATE_SEARCH)
		{
			pCtrl->ulElapsedSec	= 0;
		}
		TTX_BITCLR(pCtrl->state, (eTTX_STATE_SEARCH | eTTX_STATE_REVEAL));
		if (pCtrl->state & eTTX_STATE_SUBCODE && !(pCtrl->state & eTTX_STATE_INPUT))
		{
			snprintf((void *)pCtrl->aucInput, 5, "%04X", pContext->pTtxPage->usSubcode);
		}
	}

	if (update == eTTX_UPDATE_PageChanged)
	{
		TTX_BITCLR(pCtrl->state, (eTTX_STATE_SUBCODE | eTTX_STATE_INPUT));
		//local_ttx_SetPageFilter(pContext, pContext->usPageNum, TTX_IGNORE_SUBCODE, FALSE);
	}

	return ERR_OK;
}

/**
 *
 */
static void	local_ttx_SendRefreshMsg (TTX_Context_t *pCtx, TTX_Refresh_t refresh)
{
	TTX_Msg_t	ttxMsg;

	(void)pCtx;

	if (refresh == eTTX_REFRESH_NONE)
		return;

	//ttxMsg.refreshMsg.hAction	= s_ulMasterActionid;
	ttxMsg.refreshMsg.ulCmd 	= TTXM_PAGE_REFRESH;
	ttxMsg.refreshMsg.refresh	= refresh;
	ttxMsg.refreshMsg.ulVersion	= ++s_ulRefreshVersion;

	VK_MSG_Send(local_ttx_GetTltxtMsgQId(), &ttxMsg, sizeof(TTX_Msg_t));

}




/**
 * 디코더에서 호출하는 콜백
 * - 필터에 걸려 있는 페이지가 새로 들어오면, 호출된다.
 *
 * @param	pPage		필터에 걸려있는 새로들어온 페이지
 */
static void 	local_ttx_CbPageReceiver	(TTX_Page_t *pPage)
{

	TTX_Context_t 		*pCt = NULL;
//	TTX_Controller_t	*pCtrl;
	TTX_Msg_t			ttxMsg;
	TTX_Update_t		update	= eTTX_UPDATE_None;
	pCt 	= local_ttx_GetContext();
//	pCtrl	= &pCt->controller;
	if (pCt->pTtxPage->usPageNum == pPage->usPageNum
	 && pCt->pTtxPage->usSubcode == pPage->usSubcode)
	{
		update	= eTTX_UPDATE_PageUpdated;
#if defined(TTX_PAGE_COMPARE)
		if (HxSTD_MemCmp(pCt->pTtxPage, pPage, sizeof(TTX_Page_t)) == 0)
			return;
#endif
	}

	if (pCt->pTtxPage->usSubcode != pPage->usSubcode)
	{
		update	= eTTX_UPDATE_SubPageChanged;
	}

	if (pCt->pTtxPage->usPageNum != pPage->usPageNum)
	{
		update	= eTTX_UPDATE_PageChanged;
	}
	HxSTD_MemCopy(pCt->pTtxPage, pPage, sizeof(TTX_Page_t));


	ttxMsg.updateMsg.ulCmd	= TTXM_PAGE_UPDATED;
	ttxMsg.updateMsg.update	= update;
	VK_MSG_Send(local_ttx_GetTltxtMsgQId(), &ttxMsg, sizeof(TTX_Msg_t));
}


/**
 * 컨텍스트 Status를 세팅한다.
 *
 * @param	pContext	컨텍스트
 * @param	eStatus		(Waiting/Ready/Running)
 */
static INLINE void local_ttx_SetContextStatus(TTX_Context_t *pContext, TTX_Status_t eStatus)
{
    pContext->status = eStatus;
}


/**
 * 디코더에 필터를 건다.
 *
 * @param	pContext	컨텍스트
 * @param	usPageNum	페이지 넘버
 * @param	usSubcode	서브코드
 * @return	ERR_OK / ERR_FAIL
 */
static HERROR	local_ttx_SetPageFilter	(TTX_Context_t *pContext, HUINT16 usPageNum, HUINT16 usSubcode, HBOOL bRecvCached)
{
	TTX_Result_t		ttxResult;
	TTX_Controller_t	*pCtrl	= &pContext->controller;
	usPageNum	%= 800;	// Magazine Number가 8이 될 경우를 대비한 필터링
		HxLOG_Print("[%s:%d]\n",__FUNCTION__,__LINE__);
	VK_SEM_Get(s_ulTtxDecSema);
	ttxResult	= TTX_DECODER_SetFilter(s_hTtxDecoder, usPageNum, usSubcode, bRecvCached, local_ttx_CbPageReceiver);
	VK_SEM_Release(s_ulTtxDecSema);
 	HxLOG_Print("[%s:%d] ttxResult [%d]\n",__FUNCTION__,__LINE__,ttxResult);
	// Filtering 하는 페이지가 Cache에 저장되어 있거나, 없거나..
	if (ttxResult == ERR_TTX_OK || ttxResult == ERR_TTX_NO_PAGE)
	{
		pContext->usPageNum	= usPageNum;
		pContext->usSubcode	= usSubcode;
		pCtrl->usFilterPageNum	= usPageNum;
		pCtrl->usFilterSubcode	= usSubcode;


		HxLOG_Print("[%s:%d] page[%d] subcode[%d]\n",__FUNCTION__,__LINE__,usPageNum,usSubcode);


		if (ttxResult == ERR_TTX_NO_PAGE)
		{
			TTX_BITSET(pCtrl->state, eTTX_STATE_SEARCH);
		}
		else
		{
			TTX_BITCLR(pCtrl->state, eTTX_STATE_SEARCH);
		}
		TTX_BITCLR(pCtrl->state, eTTX_STATE_HOLD);

		if (usSubcode == TTX_IGNORE_SUBCODE)
		{
			TTX_BITSET(pCtrl->state, eTTX_STATE_ROTATE_SUBPAGE);
		}
		else
		{
			TTX_BITCLR(pCtrl->state, eTTX_STATE_ROTATE_SUBPAGE);
		}
		return ERR_OK;
	}
	return ERR_FAIL;
}


/**
 * 필터를 푼다.
 *
 * @param	pContext	컨텍스트
 * @param	usPageNum	페이지 넘버
 * @param	usSubcode	서브코드
 * @return	ERR_OK / ERR_FAIL
 */
static HERROR	local_ttx_UnsetPageFilter (TTX_Context_t *pContext)
{
	HERROR	hError;

	VK_SEM_Get(s_ulTtxDecSema);
	hError	= TTX_DECODER_SetFilter(s_hTtxDecoder, pContext->usPageNum, pContext->usSubcode, FALSE, local_ttx_CbPageReceiver);
	VK_SEM_Release(s_ulTtxDecSema);

	TTX_BITCLR(pContext->controller.state, eTTX_STATE_SEARCH);

	return hError;
}

static HERROR local_ttx_SetLanguageGroup (const TTX_Language_t lang)
{
	TTX_Result_t	ttxerr	= ERR_TTX_OK;

	if (eTTX_LANG_LAST <= lang)
		return ERR_FAIL;

	VK_SEM_Get(s_ulTtxDecSema);
	switch (lang)
	{
	case eTTX_LANG_LATIN_WEST_EU:
		ttxerr	|=	TTX_DECODER_SetLanguage(s_hTtxDecoder, 0x0, eTTX_CODE_G0_LATIN, eTTX_CODE_G2_LATIN, eTTX_NOS_ENG);
		ttxerr	|=	TTX_DECODER_SetLanguage(s_hTtxDecoder, 0x1, eTTX_CODE_G0_LATIN, eTTX_CODE_G2_LATIN, eTTX_NOS_GER);
		ttxerr	|=	TTX_DECODER_SetLanguage(s_hTtxDecoder, 0x2, eTTX_CODE_G0_LATIN, eTTX_CODE_G2_LATIN, eTTX_NOS_SWE_FIN_HUN);
		ttxerr	|=	TTX_DECODER_SetLanguage(s_hTtxDecoder, 0x3, eTTX_CODE_G0_LATIN, eTTX_CODE_G2_LATIN, eTTX_NOS_ITA);
		ttxerr	|=	TTX_DECODER_SetLanguage(s_hTtxDecoder, 0x4, eTTX_CODE_G0_LATIN, eTTX_CODE_G2_LATIN, eTTX_NOS_FRE);
		ttxerr	|=	TTX_DECODER_SetLanguage(s_hTtxDecoder, 0x5, eTTX_CODE_G0_LATIN, eTTX_CODE_G2_LATIN, eTTX_NOS_POR_SPA);
		ttxerr	|=	TTX_DECODER_SetLanguage(s_hTtxDecoder, 0x6, eTTX_CODE_G0_LATIN, eTTX_CODE_G2_LATIN, eTTX_NOS_TUR);
		ttxerr	|=	TTX_DECODER_SetLanguage(s_hTtxDecoder, 0x7, eTTX_CODE_G0_GREEK, eTTX_CODE_G2_GREEK, eTTX_NOS_EMPTY);
		break;

	case eTTX_LANG_LATIN_EAST_EU:
		ttxerr	|=	TTX_DECODER_SetLanguage(s_hTtxDecoder, 0x0, eTTX_CODE_G0_LATIN, eTTX_CODE_G2_LATIN, eTTX_NOS_POL);
		ttxerr	|=	TTX_DECODER_SetLanguage(s_hTtxDecoder, 0x1, eTTX_CODE_G0_LATIN, eTTX_CODE_G2_LATIN, eTTX_NOS_GER);
		ttxerr	|=	TTX_DECODER_SetLanguage(s_hTtxDecoder, 0x2, eTTX_CODE_G0_LATIN, eTTX_CODE_G2_LATIN, eTTX_NOS_EST);
		ttxerr	|=	TTX_DECODER_SetLanguage(s_hTtxDecoder, 0x3, eTTX_CODE_G0_LATIN, eTTX_CODE_G2_LATIN, eTTX_NOS_LET_LIT);
		ttxerr	|=	TTX_DECODER_SetLanguage(s_hTtxDecoder, 0x4, eTTX_CODE_G0_LATIN, eTTX_CODE_G2_LATIN, eTTX_NOS_FRE);
		ttxerr	|=	TTX_DECODER_SetLanguage(s_hTtxDecoder, 0x5, eTTX_CODE_G0_LATIN, eTTX_CODE_G2_LATIN, eTTX_NOS_SCC_SCR_SLV);
		ttxerr	|=	TTX_DECODER_SetLanguage(s_hTtxDecoder, 0x6, eTTX_CODE_G0_LATIN, eTTX_CODE_G2_LATIN, eTTX_NOS_CZE_SLO);
		ttxerr	|=	TTX_DECODER_SetLanguage(s_hTtxDecoder, 0x7, eTTX_CODE_G0_LATIN, eTTX_CODE_G2_LATIN, eTTX_NOS_RUM);
		break;

	case eTTX_LANG_HEBREW_ARABIC:
		ttxerr	|=	TTX_DECODER_SetLanguage(s_hTtxDecoder, 0x0, eTTX_CODE_G0_LATIN, eTTX_CODE_G2_LATIN, eTTX_NOS_ENG);
		ttxerr	|=	TTX_DECODER_SetLanguage(s_hTtxDecoder, 0x1, eTTX_CODE_G0_LATIN, eTTX_CODE_G2_LATIN, eTTX_NOS_GER);
		ttxerr	|=	TTX_DECODER_SetLanguage(s_hTtxDecoder, 0x2, eTTX_CODE_G0_LATIN, eTTX_CODE_G2_LATIN, eTTX_NOS_SWE_FIN_HUN);
		ttxerr	|=	TTX_DECODER_SetLanguage(s_hTtxDecoder, 0x3, eTTX_CODE_G0_LATIN, eTTX_CODE_G2_LATIN, eTTX_NOS_ITA);
		ttxerr	|=	TTX_DECODER_SetLanguage(s_hTtxDecoder, 0x4, eTTX_CODE_G0_LATIN, eTTX_CODE_G2_LATIN, eTTX_NOS_FRE);
		ttxerr	|=	TTX_DECODER_SetLanguage(s_hTtxDecoder, 0x5, eTTX_CODE_G0_HEBREW, eTTX_CODE_G2_ARBIC, eTTX_NOS_EMPTY);
		ttxerr	|=	TTX_DECODER_SetLanguage(s_hTtxDecoder, 0x6, eTTX_CODE_G0_LATIN, eTTX_CODE_G2_LATIN, eTTX_NOS_TUR);
		ttxerr	|=	TTX_DECODER_SetLanguage(s_hTtxDecoder, 0x7, eTTX_CODE_G0_ARBIC, eTTX_CODE_G2_ARBIC, eTTX_NOS_EMPTY);
		break;

	case eTTX_LANG_CYRILLIC:
		//난 아래 주석 처 놓은게 맞다고 생각하지만... LG 세트는 아님..
		//ttxerr	|=	TTX_DECODER_SetLanguage(s_hTtxDecoder, 0x0, eTTX_CODE_G0_CYRILLIC_1, eTTX_CODE_G2_CYRILLIC, eTTX_NOS_EMPTY);
		ttxerr	|=	TTX_DECODER_SetLanguage(s_hTtxDecoder, 0x0, eTTX_CODE_G0_LATIN, eTTX_CODE_G2_LATIN, eTTX_NOS_ENG);
		ttxerr	|=	TTX_DECODER_SetLanguage(s_hTtxDecoder, 0x1, eTTX_CODE_G0_LATIN, eTTX_CODE_G2_LATIN, eTTX_NOS_GER);
		ttxerr	|=	TTX_DECODER_SetLanguage(s_hTtxDecoder, 0x2, eTTX_CODE_G0_LATIN, eTTX_CODE_G2_LATIN, eTTX_NOS_EST);
		ttxerr	|=	TTX_DECODER_SetLanguage(s_hTtxDecoder, 0x3, eTTX_CODE_G0_LATIN, eTTX_CODE_G2_LATIN, eTTX_NOS_LET_LIT);
		ttxerr	|=	TTX_DECODER_SetLanguage(s_hTtxDecoder, 0x4, eTTX_CODE_G0_CYRILLIC_2, eTTX_CODE_G2_CYRILLIC, eTTX_NOS_EMPTY);
		ttxerr	|=	TTX_DECODER_SetLanguage(s_hTtxDecoder, 0x5, eTTX_CODE_G0_CYRILLIC_3, eTTX_CODE_G2_CYRILLIC, eTTX_NOS_EMPTY);
		ttxerr	|=	TTX_DECODER_SetLanguage(s_hTtxDecoder, 0x6, eTTX_CODE_G0_LATIN, eTTX_CODE_G2_LATIN, eTTX_NOS_CZE_SLO);
		ttxerr	|=	TTX_DECODER_SetLanguage(s_hTtxDecoder, 0x7, eTTX_CODE_G0_LATIN, eTTX_CODE_G2_LATIN, eTTX_NOS_ENG);
		break;

	default:
		ttxerr	=	ERR_TTX_FAIL;
		break;
	}

	VK_SEM_Release(s_ulTtxDecSema);
	return (ttxerr == ERR_TTX_OK) ? ERR_OK : ERR_FAIL;
}



/**
 * 디코더에서 페이지를 검색한다.
 *
 * @param	pContext	컨텍스트
 * @param	search		search 방법
 * @param	usPageNum	결과: 페이지 넘버
 * @param	usSubcode	결과: 서브코드
 * @param	bWrapAround	결과: wrap around??
 * @return	ERR_OK / ERR_FAIL
 */
static HERROR	local_ttx_SearchTTXPage (TTX_Context_t *pContext, TTX_Search_t search,
										HUINT16 *pusPageNum, HUINT16 *pusSubcode, HBOOL *pbWrapAround)
{
	HERROR		hError;
	HUINT16		usPageNum;
	HUINT16		usSubcode;
	TTX_Page_t	*pPage		= pContext->pTtxPage;

	if (pContext->controller.state & eTTX_STATE_SEARCH)
	{
		usPageNum	= pContext->controller.usFilterPageNum;
		usSubcode	= pContext->controller.usFilterSubcode;
	}
	else
	{
		usPageNum	= pPage->usPageNum;
		usSubcode	= pPage->usSubcode;
	}

	VK_SEM_Get(s_ulTtxDecSema);
	hError	= TTX_DECODER_Search(s_hTtxDecoder, usPageNum, usSubcode, search, pusPageNum, pusSubcode);
	VK_SEM_Release(s_ulTtxDecSema);

	if (pbWrapAround)
	{
		switch (search)
		{
		case eTTX_SEARCH_NEXT_PAGE:		*pbWrapAround	= (usPageNum >= *pusPageNum) ? TRUE : FALSE;	break;
		case eTTX_SEARCH_PREV_PAGE:		*pbWrapAround	= (usPageNum <= *pusPageNum) ? TRUE : FALSE;	break;
		case eTTX_SEARCH_NEXT_SUBPAGE:	*pbWrapAround	= (usSubcode >= *pusSubcode) ? TRUE : FALSE;	break;
		case eTTX_SEARCH_PREV_SUBPAGE:	*pbWrapAround	= (usSubcode <= *pusSubcode) ? TRUE : FALSE;	break;
		default:	*pbWrapAround	= FALSE;	break;
		}
	}
	return (hError == ERR_TTX_OK) ? ERR_OK : ERR_FAIL;
}



static void	local_ttx_TimerTimeout (HUINT32 ulTimerId)
{
	TTX_Context_t		*pContext	= NULL;
	TTX_Msg_t			cycleMsg;

	pContext	= local_ttx_GetContext();
	if (pContext == NULL ||  pContext->status != eTTX_Status_Running)
	{
		return;
	}

	if (ulTimerId == pContext->ulTimerId_Cycle)
	{
		HINT32 nErr;
		cycleMsg.cycleMsg.ulCmd 	= TTXM_CYCLE;

		nErr = VK_MSG_SendTimeout(local_ttx_GetTltxtMsgQId(), &cycleMsg, sizeof(TTX_Msg_t), 0);
		if (VK_OK != nErr)
		{
			HxLOG_Critical("[%s:%d] VK_MSG_SendTimeout \n",__FUNCTION__,__LINE__);
		}

	}
	else
	{
		HxLOG_Critical("[%s:%d]\n",__FUNCTION__,__LINE__);
	}
}

static HERROR	local_ttx_KillTimer (TTX_Context_t *pContext, HUINT32 ulTimerId)
{
	(void)pContext;

	if (ulTimerId == 0)
	{
		return ERR_FAIL;
	}

	return VK_TIMER_Cancel(ulTimerId);
}

static HERROR	local_ttx_SetTimer (TTX_Context_t *pContext, HUINT32 *pulTimerId, HUINT32 ulTimeout, HBOOL bRepeat)
{
	int (*pfn_Timer)(unsigned long, void (*cbfn)(unsigned long, void *), void *, int, unsigned long *) = NULL;

	if (*pulTimerId != 0)
	{
		local_ttx_KillTimer(pContext, *pulTimerId);
		*pulTimerId = 0;
	}

	pfn_Timer = (bRepeat == TRUE) ? VK_TIMER_EventEvery : VK_TIMER_EventAfter;

	return pfn_Timer(
				(unsigned long)ulTimeout,
				(void (*)(unsigned long, void *))local_ttx_TimerTimeout,
				NULL,
				sizeof(HUINT32),
				(unsigned long *)pulTimerId
			);
	}



static HERROR	local_ttx_Cycle (TTX_Context_t *pContext)
{
	TTX_Controller_t	*pCtrl	= &pContext->controller;
	TTX_Refresh_t		refresh	= eTTX_REFRESH_NONE;
	TTX_State_t			state	= pCtrl->state;

	s_ulTtxCycle++;


	if (TTX_CYCLE_SEARCH(s_ulTtxCycle))
	{
		if (!(state & eTTX_STATE_EBU_SUBTITLE) && (state & eTTX_STATE_SEARCH))
			TTX_BITSET(refresh, eTTX_REFRESH_HEADER);
	}

	if (TTX_CYCLE_TIME(s_ulTtxCycle))
	{
		if (!(state & eTTX_STATE_EBU_SUBTITLE))
			TTX_BITSET(refresh, eTTX_REFRESH_TIME);
	}


	if (TTX_CYCLE_FLASH(s_ulTtxCycle))
	{
		pCtrl->ulElapsedSec++;
		TTX_BITFLP(  state, eTTX_STATE_FLASH);
		TTX_BITSET(refresh, eTTX_REFRESH_FLASH);
	}
	pCtrl->state	= state;

	local_ttx_Invalidate(refresh);


#if !defined(CONFIG_MWC_OSD_SBTL_ON_UIPLANE)
	if (TTX_CYCLE_SEARCH(s_ulTtxCycle))
	{
		/**
		 * Update 최소 주기인 Search 주기에, Refresh Flag를 체킹한다.
		 * 최소 주기 이내로 여러번 들어오는 refresh 메시지로 인한 performance 저하로
		 * 아래와 같이 최소 refresh 주기를 적어도 search 주기로 제한한다.
		 *
		 * @date 2010-03-20
		 * @author	jhlee4
		 */
		local_ttx_SendRefreshMsg(pContext, s_eRefresh);
	}
#endif

	VK_SEM_Get(s_ulTtxDecSema);
	(void)TTX_DECODER_Flush(s_hTtxDecoder);
	VK_SEM_Release(s_ulTtxDecSema);

	return ERR_OK;
}



/**
 * TTX_Graphics 에서 사용할 변수를 세팅
 *
 */
static void	local_ttx_SetFBVariable (TTX_Context_t *pCtx)
{
	TTX_Controller_t	*pCtrl	= &pCtx->controller;

	//2 SETTOP
		pCtrl->nCellWidth	= 15;
		pCtrl->nCellHeight	= 19;
		pCtrl->nMarginW 	= 60;
		pCtrl->nMarginH 	= 40;

}



/**
 * Subcode 모드에서 Color Key 입력을 받아 링크 페이지로 이동한다.
 *
 * @param	pContext	컨텍스트
 * @param	pCtrl		컨트롤러
 * @param	ulColorKey	0:RED, 1:GREEN, 2:YELLOW, 3:BLUE
 * @return	ERR_OK / ERR_FAIL / ERR_CONTINUE
 */
static HERROR local_ttx_Control_COLORKEY_Subcode (TTX_Context_t *pContext, TTX_Controller_t *pCtrl, const HUINT32 ulColorKey)
{
	HUINT16		usSubcode;

	if (!(pCtrl->state & eTTX_STATE_SUBCODE))
	{
		return ERR_CONTINUE;
	}

	if (2 <= ulColorKey)	// NO SUBPAGE or 'YELLOW', 'BLUE'
	{
		return ERR_FAIL;
	}
	usSubcode	= TTX_SUBCODE_BCD_TO_DEC(pContext->usSubcode);

	// Subcode Range [1~3979], but... [1~79]가 옳은듯 보인다...
	if (ulColorKey == 0)		// 'RED'
	{
		usSubcode	= (usSubcode == 0) ? 3979 : usSubcode - 1;
	}
	else if (ulColorKey == 1)	// 'GREEN'
	{
		usSubcode	= (usSubcode >= 3979) ? 0 : usSubcode + 1;
	}
	usSubcode = TTX_SUBCODE_DEC_TO_BCD(usSubcode);
	snprintf((void *)pCtrl->aucInput, 5, "%04X", usSubcode);

	local_ttx_SetPageFilter(pContext, pContext->usPageNum, usSubcode, TRUE);
	local_ttx_Invalidate( eTTX_REFRESH_ROW24);

	return ERR_OK;
}


/**
 * TOP 모드에서 Color Key 입력을 받아 링크 페이지로 이동한다.
 *
 * @param	pContext	컨텍스트
 * @param	pCtrl		컨트롤러
 * @param	ulColorKey	0:RED, 1:GREEN, 2:YELLOW, 3:BLUE
 * @return	ERR_OK / ERR_FAIL / ERR_CONTINUE
 */
static HERROR local_ttx_Control_COLORKEY_TOP (TTX_Context_t *pContext, TTX_Controller_t *pCtrl, const HUINT32 ulColorKey)
{
	HERROR			err;
	TTX_TOP_Link_t	*pLink	= NULL;
	TTX_Page_t		*pPage	= pContext->pTtxPage;

	if (pCtrl->navi != eTTX_NAVI_TOP || pPage->TOP.bEnabled == FALSE)
	{
		return ERR_CONTINUE;
	}

	pLink	= &(pPage->TOP.aLink[ulColorKey]);
	if (pLink->bEnabled == FALSE)
	{
		return ERR_FAIL;
	}

	/*
	 * pLink->usSubcode를 하는 것이 맞는것 같으나, TOP는 아직 완벽하게 구현된 것이 아니기 때문에
	 * 우선 Subcode 필터는 걸지 않는다. (이게 좀더 잘 동작한다.)
	 */
	err	= local_ttx_SetPageFilter(pContext, pLink->usPageNum, TTX_IGNORE_SUBCODE, TRUE);
	if (err == ERR_OK)
	{
		TTX_BITCLR(pCtrl->state, (eTTX_STATE_INPUT | eTTX_STATE_SUBCODE));
	}
	return ERR_OK;
}


/**
 * TRACE 모드에서 Color Key 입력을 받아 링크 페이지로 이동한다.
 *
 * @param	pContext	컨텍스트
 * @param	pCtrl		컨트롤러
 * @param	ulColorKey	0:RED, 1:GREEN, 2:YELLOW, 3:BLUE
 * @return	ERR_OK / ERR_FAIL / ERR_CONTINUE
 */
static HERROR local_ttx_Control_COLORKEY_TRACE (TTX_Context_t *pContext, TTX_Controller_t *pCtrl, const HUINT32 ulColorKey)
{
	HERROR		err;
	TTX_Page_t	*pPage	= pContext->pTtxPage;
	HUINT16		usPage	= 0;

	if (pPage->TRACE.ulVersion == 0)
	{
		return ERR_CONTINUE;
	}

	usPage	= pPage->TRACE.ausTrace[ulColorKey];

	err	= local_ttx_SetPageFilter(pContext, usPage, TTX_IGNORE_SUBCODE, TRUE);
	if (err == ERR_OK)
	{
		TTX_BITCLR(pCtrl->state, (eTTX_STATE_INPUT | eTTX_STATE_SUBCODE));
	}
	return ERR_OK;
}

/**
 * FLOF 모드에서 Color Key 입력을 받아 링크 페이지로 이동한다.
 *
 * @param	pContext	컨텍스트
 * @param	pCtrl		컨트롤러
 * @param	ulColorKey	0:RED, 1:GREEN, 2:YELLOW, 3:BLUE
 * @return	ERR_OK / ERR_FAIL / ERR_CONTINUE
 */
static HERROR local_ttx_Control_COLORKEY_FLOF (TTX_Context_t *pContext, TTX_Controller_t *pCtrl, const HUINT32 ulColorKey)
{
	HERROR		err;
	TTX_Page_t	*pPage	= pContext->pTtxPage;
	TTX_Link_t	*pLink	= NULL;

	// FLOF은 Default이기 때문에 Navigation_t를 검사하지 않는다.
	(void)pCtrl;

	if (pPage->FLOF.bEnabled == FALSE || pPage->FLOF.bLinkControl == FALSE)
	{
		return ERR_CONTINUE;
	}

	pLink	= &(pPage->FLOF.aColorKey[ulColorKey]);
	if (pLink->bEnabled == FALSE)
	{
		return ERR_FAIL;
	}

	err	= local_ttx_SetPageFilter(pContext, pLink->usPageNum, pLink->usSubcode, TRUE);
	if (err == ERR_OK)
	{
		TTX_BITCLR(pCtrl->state, (eTTX_STATE_INPUT | eTTX_STATE_SUBCODE));
	}
	return ERR_OK;
}



/**
 * Color Key 입력을 받아 링크 페이지로 이동한다.
 * 링크 네비게이션의 우선순위는 아래와 같다.
 *		Subcode > TOP > FLOF > CTT > +,-
 *
 * @param	pContext	컨텍스트
 * @param	pCtrl		컨트롤러
 * @param	ulColorKey	0:RED, 1:GREEN, 2:YELLOW, 3:BLUE
 * @return	ERR_OK / ERR_FAIL
 */
static HERROR local_ttx_Control_COLORKEY (TTX_Context_t *pContext, TTX_Controller_t *pCtrl, const HUINT32 ulColorKey)
{
	HERROR			hError		= ERR_FAIL;
	HUINT16			usPageNum	= 0;
	HUINT16 		usSubcode	= 0;
	TTX_Search_t	search;

	if (ulColorKey > 3)
	{
		return ERR_FAIL;
	}

	if (pContext->pTtxPage->bEnabled)	// 한번이라도 페이지가 그려진 상태여야.. 아래의 Function을 수행 할 수 있음
	{
		hError	= local_ttx_Control_COLORKEY_Subcode(pContext, pCtrl, ulColorKey);
		if (hError != ERR_CONTINUE)
		{
			return hError;
		}

		hError	= local_ttx_Control_COLORKEY_TOP(pContext, pCtrl, ulColorKey);
		if (hError != ERR_CONTINUE)
		{
			return hError;
		}

		hError	= local_ttx_Control_COLORKEY_FLOF(pContext, pCtrl, ulColorKey);
		if (hError != ERR_CONTINUE)
		{
			return hError;
		}

		hError	= local_ttx_Control_COLORKEY_TRACE(pContext, pCtrl, ulColorKey);
		if (hError != ERR_CONTINUE)
		{
			return hError;
		}
	}

	// Default
	if (2 <= ulColorKey)	// YELLOW, BLUE
	{
		return ERR_FAIL;
	}

	search	= (ulColorKey == 0) ? eTTX_SEARCH_PREV_PAGE : eTTX_SEARCH_NEXT_PAGE;

	hError	= local_ttx_SearchTTXPage(pContext, search, &usPageNum, &usSubcode, NULL);
	if (hError != ERR_OK)
	{
		return hError;
	}

	hError	= local_ttx_SetPageFilter(pContext, usPageNum, usSubcode, TRUE);
	if (hError == ERR_OK)
	{
		TTX_BITCLR(pCtrl->state, (eTTX_STATE_INPUT | eTTX_STATE_SUBCODE));
	}
	return ERR_OK;
}


/*
 * 방향키로 페이지 이동한다.
 *
 * @param	pContext	컨텍스트
 * @param	pCtrl		컨트롤러
 * @param	ulArrowKey	0:Up, 1:Down
 * @return	ERR_OK / ERR_FAIL
 */
static HERROR local_ttx_Control_ARROWKEY_UpDown  (TTX_Context_t *pContext, TTX_Controller_t *pCtrl, const HUINT32 ulArrowKey)
{
	HERROR			err;
	HUINT16			usPageNum;
	HUINT16			usSubcode;
	TTX_Search_t		search;
	TTX_DoubleSize_t	*pFocus;
	HxLOG_Trace();
	pFocus	= &pCtrl->doubleSize;
	switch (*pFocus)
	{
	case eTTX_DOUBLE_FOCUS_ON_TOP:
		if (ulArrowKey == 2)	// UP
		{
			*pFocus	= eTTX_DOUBLE_FOCUS_ON_BTM;
			search	= eTTX_SEARCH_PREV_PAGE;
		}
		else	// DOWN
		{
			*pFocus	= eTTX_DOUBLE_FOCUS_ON_BTM;
			local_ttx_Invalidate(eTTX_REFRESH_ALL);
			return ERR_OK;
		}
		break;

	case eTTX_DOUBLE_FOCUS_ON_BTM:
		if (ulArrowKey == 2)	// UP
		{
			*pFocus	= eTTX_DOUBLE_FOCUS_ON_TOP;
			local_ttx_Invalidate( eTTX_REFRESH_ALL);
			return ERR_OK;
		}
		else	// DOWN
		{
			*pFocus	= eTTX_DOUBLE_FOCUS_ON_TOP;
			search	= eTTX_SEARCH_NEXT_PAGE;
		}
		break;

	default:
		if (ulArrowKey == 2)	// UP
			search	= eTTX_SEARCH_NEXT_PAGE;
		else	// DOWN
			search	= eTTX_SEARCH_PREV_PAGE;
		break;
	}

	err	= local_ttx_SearchTTXPage (pContext, search, &usPageNum, &usSubcode, NULL);
	if (err != ERR_OK)
	{
		return err;
	}

	err = local_ttx_SetPageFilter(pContext, usPageNum, TTX_IGNORE_SUBCODE, TRUE);
	if (err == ERR_OK)
	{
		TTX_BITCLR(pCtrl->state, eTTX_STATE_SUBCODE);
	}
	return ERR_OK;
}


/*
 * 방향키로 페이지 이동한다.
 *
 * @param	pContext	컨텍스트
 * @param	pCtrl		컨트롤러
 * @param	ulArrowKey	0:Left, 1:Right
 * @return	ERR_OK / ERR_FAIL
 */
static HERROR local_ttx_Control_ARROWKEY_LeftRight  (TTX_Context_t *pContext, TTX_Controller_t *pCtrl, const HUINT32 ulArrowKey)
{
	HERROR			err;
	HUINT16			usPageNum;
	HUINT16			usSubcode;
	TTX_Search_t	search;
	HBOOL			bWrapAround;
	HxLOG_Trace();
	switch (ulArrowKey)
	{
	case 0:	// LEFT
		search	= (pCtrl->state & eTTX_STATE_ROTATE_SUBPAGE)
							? eTTX_SEARCH_LAST_SUBPAGE
							: eTTX_SEARCH_PREV_SUBPAGE;
		break;

	case 1:	// RIGHT
		search	= (pCtrl->state & eTTX_STATE_ROTATE_SUBPAGE)
							? eTTX_SEARCH_FIRST_SUBPAGE
							: eTTX_SEARCH_NEXT_SUBPAGE;
		break;

	default:	return ERR_FAIL;
	}

	err	= local_ttx_SearchTTXPage(pContext, search, &usPageNum, &usSubcode, &bWrapAround);
	if (err != ERR_OK || TTX_IS_ROTATING_PAGE(usSubcode))
	{
		return ERR_FAIL;
	}

	if (!(pCtrl->state & eTTX_STATE_ROTATE_SUBPAGE) && bWrapAround)
	{
		usSubcode	= TTX_IGNORE_SUBCODE;
	}

	if (pCtrl->state & eTTX_STATE_SUBCODE)
	{
		if (usSubcode == TTX_IGNORE_SUBCODE)
			VK_memset(pCtrl->aucInput, '-', 4);
		else
			snprintf((void *)pCtrl->aucInput, 5, "%04X", usSubcode);
	}

	local_ttx_SetPageFilter(pContext, usPageNum, usSubcode, TRUE);
	return ERR_OK;
}


/*
 * 방향키로 페이지 이동한다.
 * 디코더 메모리에 저장되지 않은 페이지는 Skip하여 받아진 페이지 위주로 페이지 이동을 한다.
 *
 * @param	pContext	컨텍스트
 * @param	pCtrl		컨트롤러
 * @param	ulArrowKey	 0:Left, 1:Right,2:Up, 3:Down,
 * @return	ERR_OK / ERR_FAIL
 */
static HERROR local_ttx_Control_ARROWKEY (TTX_Context_t *pContext, TTX_Controller_t *pCtrl, const HUINT32 ulArrowKey)
{
	HERROR	err	= ERR_FAIL;
	HxLOG_Trace();
	if (ulArrowKey <= 1)	// LEFT, RIGHT
	{
		err	= local_ttx_Control_ARROWKEY_LeftRight(pContext, pCtrl, ulArrowKey);
	}
	else if (ulArrowKey <= 3)	// UP, DOWN
	{
		err	= local_ttx_Control_ARROWKEY_UpDown(pContext, pCtrl, ulArrowKey);
	}

	if (err == ERR_OK)
	{
		TTX_BITCLR(pCtrl->state, eTTX_STATE_INPUT);
	}
	return err;
}



/**
 * 번호키 입력을 받는다.(페이지 번호)
 *
 * @param	pContext	컨텍스트
 * @param	pCtrl		컨트롤러
 * @param	ucNumber	입력된 숫자
 * @return	ERR_OK / ERR_FAIL
 */
static HERROR local_ttx_Control_NUMERIC (TTX_Context_t *pContext, TTX_Controller_t *pCtrl, const HUINT8 ucNumber)
{
	HUINT32 	i 			= 0;
	HUINT32		j			= 0;
	HUINT32		ulMaxInput	= 0;
	HUINT16 	usPageNum	= 0;
	HUINT16		usSubcode	= 0;
	HUINT8		aucPageNumLimit[3]	= {'8', '9', '9'};
	HUINT8		aucSubcodeLimit[4]	= {'3', '9', '7', '9'};
	HUINT8		*pucLimit	= NULL;
	TTX_Refresh_t	refresh;

	if (ucNumber > 9)
	{
		return ERR_FAIL;
	}

	ulMaxInput	= (pCtrl->state & eTTX_STATE_SUBCODE) ? 4 : 3;
	pucLimit	= (pCtrl->state & eTTX_STATE_SUBCODE) ? aucSubcodeLimit : aucPageNumLimit;
	refresh		= (pCtrl->state & eTTX_STATE_SUBCODE) ? eTTX_REFRESH_ROW24 : eTTX_REFRESH_CTRL_PANEL;

	if (!(pCtrl->state & eTTX_STATE_INPUT))	// 첫 입력
	{
		pCtrl->aucInput[3] = pCtrl->aucInput[4] = '\0';
		VK_memset(pCtrl->aucInput, '-', ulMaxInput);
	}

	while (i < ulMaxInput && pCtrl->aucInput[i] != '-')	i++;
	while (j < i && pucLimit[j] <= pCtrl->aucInput[j])	j++;
	if (i == ulMaxInput
		|| (i == j && pucLimit[i] < ucNumber + '0')
		|| (i == 0 && ulMaxInput == 3 && ucNumber == 0))// Limit Check
	{
		return ERR_FAIL;
	}

	TTX_BITSET(pCtrl->state, eTTX_STATE_INPUT);
	pCtrl->aucInput[i++] = ucNumber + '0';

	if (i == ulMaxInput) // INPUT DONE!!
	{
		if (pCtrl->state & eTTX_STATE_SUBCODE)
		{
			usPageNum	= pContext->pTtxPage->usPageNum;
			usSubcode	= 0;
			for (i = 0 ; i < ulMaxInput ; i++)
			{
				usSubcode	= usSubcode * 16 + (pCtrl->aucInput[i] - '0');
			}
			TTX_BITSET(refresh, eTTX_REFRESH_SUB_BAR);
		}
		else
		{
			usPageNum	= 0;
			usSubcode	= TTX_IGNORE_SUBCODE;
			for (i = 0 ; i < ulMaxInput ; i++)
			{
				usPageNum	= usPageNum * 10 + (pCtrl->aucInput[i] - '0');
			}
		}

		local_ttx_SetPageFilter(pContext, usPageNum, usSubcode, TRUE);
		TTX_BITCLR(pCtrl->state, eTTX_STATE_INPUT);
	}

	if (pCtrl->doubleSize == eTTX_DOUBLE_FOCUS_ON_BTM)
	{
		refresh	= eTTX_REFRESH_ALL;
		pCtrl->doubleSize = eTTX_DOUBLE_FOCUS_ON_TOP;
	}

	local_ttx_Invalidate(refresh);

	return ERR_OK;
}



/**
 * 기타 TTX 기능을 처리
 *
 * @param	pContext	컨텍스트
 * @param	pCtrl		컨트롤러
 * @param	pvArg		var args
 * @return	ERR_OK / ERR_FAIL
 */
static HERROR local_ttx_Control_FUNCTION (TTX_Context_t *pContext, TTX_Controller_t *pCtrl, TTX_Control_t control, HUINT32 ulArg)
{
	TTX_Page_t	*pPage	= pContext->pTtxPage;
	HxLOG_Trace();
	switch (control)
	{
	/*
	 * Header의 Control Panel(0 ~ 7번째 칸)을 제외한 나머지 부분을 그리지 않는다.
	 */
	case eTTX_CONTROL_CANCEL:
		TTX_BITFLP(pCtrl->state, eTTX_STATE_CANCEL);
		TTX_BITCLR(pCtrl->state, eTTX_STATE_SUBCODE);
		local_ttx_Invalidate( eTTX_REFRESH_ALL);
		break;

	/*
	 * Background Color Alpha 값을 변경 시킨다.
	 * Ap에서 직접 변경 가능하다. (Customer에 종속적이기 때문)
	 */
	case eTTX_CONTROL_MIX:
	{
		TTX_Mix_t	mix	= (TTX_Mix_t)ulArg;

		if (mix >= eTTX_MIX_LAST)
		{
			return ERR_FAIL;
		}

		pCtrl->mix		= mix;
		local_ttx_Invalidate( eTTX_REFRESH_ALL);
		break;
	}

	/*
	 * 감춰져(Conceal)있는 문자를 표시한다(토글).
	 */
	case eTTX_CONTROL_REVEAL:
		TTX_BITFLP(pCtrl->state, eTTX_STATE_REVEAL);
		local_ttx_Invalidate( eTTX_REFRESH_CONCEAL);
		break;

	/*
	 * 서브페이지를 이동할 수 있도록 상태를 변경한다.
	 */
	case eTTX_CONTROL_SUBPAGE:
		TTX_BITFLP(pCtrl->state, eTTX_STATE_SUBCODE);
		HxSTD_MemSet(pCtrl->aucInput, '-', 4);

		if (pCtrl->state & eTTX_STATE_INPUT)
			local_ttx_Invalidate( eTTX_REFRESH_CTRL_PANEL | eTTX_REFRESH_ROW24);
		else
			local_ttx_Invalidate( eTTX_REFRESH_ROW24);
		TTX_BITCLR(pCtrl->state, eTTX_STATE_INPUT);
		break;

	/*
	 * 화면을 세로 방향으로 두 배 늘려 그린다.
	 */
	case eTTX_CONTROL_DOUBLE:
	{
		TTX_DoubleSize_t	doubleSize	= (TTX_DoubleSize_t)ulArg;
		if (doubleSize >= eTTX_DOUBLE_LAST)
		{
			return ERR_FAIL;
		}

		pCtrl->doubleSize	= doubleSize;

		local_ttx_Invalidate( eTTX_REFRESH_ALL);
		break;
	}

	/*
	 * 새로 페이지를 Update하지 않는다.
	 */
	case eTTX_CONTROL_HOLD:
		if (pCtrl->state & eTTX_STATE_HOLD)	// hold -> normal
		{
			HUINT16	usPageNum;

			TTX_BITCLR(pCtrl->state, eTTX_STATE_HOLD);

			// Page가 한번이라도 그려진 경우, 필터링 걸린 페이지가 아닌, 현재 그려지는 페이지로 복귀해야 한다.
			usPageNum = (pContext->pTtxPage->bEnabled == TRUE) ? pContext->pTtxPage->usPageNum : pContext->usPageNum;

			local_ttx_SetPageFilter(pContext, usPageNum, TTX_IGNORE_SUBCODE, FALSE);
		}
		else	// normal -> hold
		{
			TTX_BITSET(pCtrl->state, eTTX_STATE_HOLD);
			local_ttx_UnsetPageFilter(pContext);
			TTX_BITCLR(pCtrl->state, (eTTX_STATE_INPUT | eTTX_STATE_SUBCODE));
		}
		local_ttx_Invalidate( eTTX_REFRESH_CTRL_PANEL);
		break;

	/*
	 * Index 페이지로 이동한다. 만약 FLOF가 Enable 상태라면, FLOF의 Index 페이지로 이동한다.
	 */
	case eTTX_CONTROL_INDEX:
		if (pPage->FLOF.bEnabled == TRUE && pPage->FLOF.indexKey.bEnabled == TRUE)
		{
			local_ttx_SetPageFilter(pContext, pPage->FLOF.indexKey.usPageNum, pPage->FLOF.indexKey.usSubcode, TRUE);
		}
		else
		{
			TTX_Result_t	result;
			HUINT16			usPageNum, usSubcode;

			VK_SEM_Get(s_ulTtxDecSema);
			result = TTX_DECODER_InitPage(s_hTtxDecoder, &usPageNum, &usSubcode);
			VK_SEM_Release(s_ulTtxDecSema);
			if (result != ERR_TTX_OK)
			{
				usPageNum	= 100;
				usSubcode	= TTX_IGNORE_SUBCODE;
			}
			local_ttx_SetPageFilter(pContext, usPageNum, usSubcode, TRUE);
		}

		TTX_BITCLR(pCtrl->state, (eTTX_STATE_INPUT | eTTX_STATE_SUBCODE));
		local_ttx_Invalidate( eTTX_REFRESH_ALL);
		break;

	/*
	 * Page Navigation을 변경한다.
	 */
	case eTTX_CONTROL_NAVI:
	{
		TTX_Navigation_t	navi	= (TTX_Navigation_t)ulArg;
		if (eTTX_NAVI_LAST <= navi || pCtrl->navi == navi)
		{
			return ERR_FAIL;
		}
		pCtrl->navi	= navi;

		TTX_BITCLR(pCtrl->state, eTTX_STATE_SUBCODE);
		local_ttx_Invalidate( eTTX_REFRESH_ROW24);
		break;
	}

	/**
	 * TTX Language Group을 설정
	 */
	case eTTX_CONTROL_LANGUAGE:
	{
		TTX_Language_t lang = (TTX_Language_t)ulArg;
		if (local_ttx_SetLanguageGroup(lang) != ERR_OK)
		{
			return ERR_FAIL;
		}

		if (pCtrl->langGroup != lang)
		{
			local_ttx_Invalidate( eTTX_REFRESH_ALL);
		}
		pCtrl->langGroup = lang;
		break;
	}

	default:
		return ERR_FAIL;
	}

	return ERR_OK;
}

static HERROR local_ttx_Show(void)
{
    TTX_Context_t 	*pCt = NULL;
	HINT32		winId =0;

	pCt = local_ttx_GetContext();
	s_ulTtxCycle	= 0;

	if(pCt->bEbuSbtl == TRUE)
		winId = ONDK_LOWER_WINDOWID;
	else
		winId = ONDK_UPPER_WINDOWID;

	VK_SEM_Get(s_ulTtxDecSema);

#if 0
	if(g_hTeletextWinHandle)
		ONDK_WINDOW_Destroy(g_hTeletextWinHandle);
	g_hTeletextWinHandle=	 ONDK_WINDOW_Create(winId,0,0, GWM_SCREEN_WIDTH, GWM_SCREEN_HEIGHT, "ARGB8888", !pCt->bEbuSbtl);
	ONDK_WINDOW_SetShow(g_hTeletextWinHandle,TRUE);
#else
	if(g_hTeletextWinHandle == NULL)
	{
		g_hTeletextWinHandle=	 ONDK_WINDOW_Create(winId,0,0, 1280,720, "ARGB8888", FALSE);
	}
	if(g_hTeletextWinHandle)
	{
		ONDK_WINDOW_Clear(g_hTeletextWinHandle);
		ONDK_WINDOW_Update(g_hTeletextWinHandle, NULL); 	
		
		ONDK_WINDOW_SetShow(g_hTeletextWinHandle,TRUE);
	}
#endif

	TTX_DECODER_Switch(g_hTeletextWinHandle,s_hTtxDecoder,TRUE);
	VK_SEM_Release(s_ulTtxDecSema);

	HxSTD_MemSet(pCt->pTtxPage, 0, sizeof(TTX_Page_t));
	HxSTD_MemSet(&pCt->controller, 0, sizeof(TTX_Controller_t));
	pCt->controller.mix	= eTTX_MIX_OFF;

	local_ttx_SetFBVariable(pCt);

	if (pCt->bEbuSbtl == TRUE)
	{
		TTX_BITSET(pCt->controller.state, eTTX_STATE_EBU_SUBTITLE);
	}
	if (pCt->bVariableFont == TRUE)
	{
		TTX_BITSET(pCt->controller.state, eTTX_STATE_VAR_FONT);
	}

	HxLOG_Print("[%s:%d]pCt->usPageNum[%d], pCt->usSubcode[%d]\n",__FUNCTION__,__LINE__,pCt->usPageNum, pCt->usSubcode);

	local_ttx_SetPageFilter(pCt, pCt->usPageNum, pCt->usSubcode, TRUE);
	local_ttx_SetTimer(pCt, &pCt->ulTimerId_Cycle, TTX_CYCLE_TERM, TRUE);

//	local_ttx_ShowSuccess(pCt);

    return ERR_OK;
}

static HERROR local_ttx_Hide(void)
{
    TTX_Context_t 	*pCt = NULL;
	if(g_hTeletextWinHandle == NULL) 					return ERR_OK;

    HxLOG_Print("\tHide Teletext \n");

    pCt = local_ttx_GetContext();

	local_ttx_KillTimer(pCt,pCt->ulTimerId_Cycle);
	pCt->ulTimerId_Cycle	= 0;

	local_ttx_UnsetPageFilter(pCt);

	VK_SEM_Get(s_ulTtxDecSema);
	TTX_DECODER_Switch(g_hTeletextWinHandle,s_hTtxDecoder, FALSE);

#if 0
	if(g_hTeletextWinHandle)
		ONDK_WINDOW_Destroy(g_hTeletextWinHandle);
	g_hTeletextWinHandle = NULL;
#else
	if(g_hTeletextWinHandle)
	{
		ONDK_WINDOW_SetShow(g_hTeletextWinHandle,FALSE);

		ONDK_WINDOW_Clear(g_hTeletextWinHandle);
		ONDK_WINDOW_Update(g_hTeletextWinHandle, NULL);
	}
#endif

	VK_SEM_Release(s_ulTtxDecSema);

//	local_ttx_HideSuccess(pCt);
//	ONDK_GWM_SendMessage(NULL, MSG_APP_TELETEXT_STOPPED, HANDLE_NULL, pCt->bEbuSbtl , 0, 0);
	ONDK_GWM_PostMessage(NULL, MSG_APP_TELETEXT_STOPPED, HANDLE_NULL, pCt->bEbuSbtl , 0, 0);
	HxSTD_MemSet(pCt->pTtxPage, 0, sizeof(TTX_Page_t));



	return ERR_OK;
}



static HERROR local_ttx_Stop(void)
{
	// TTX Decoder Stop!!
	VK_SEM_Get(s_ulTtxDecSema);
	TTX_DECODER_Destroy(s_hTtxDecoder);
	s_hTtxDecoder = HANDLE_NULL;
	VK_SEM_Release(s_ulTtxDecSema);

	HxLOG_Print("Stop Teletext\n");

	return ERR_OK;
}
static HERROR local_ttx_Start(void)
{
    HxLOG_Print("Start Teletext\n");

	if (s_pucTtxMemory == NULL)
	{
		s_pucTtxMemory	= (HUINT8 *)AP_Malloc(TTX_MEMORY_SIZE);
		if (s_pucTtxMemory == NULL)
		{
			HxLOG_Error("\tMemory Allocation Fail(TTX_DECODER)!!\n");
			return ERR_FAIL;
		}
	}

	// TTX Decoder Start!!
	VK_SEM_Get(s_ulTtxDecSema);
	s_hTtxDecoder	= TTX_DECODER_Create(s_pucTtxMemory, TTX_MEMORY_SIZE);
	VK_SEM_Release(s_ulTtxDecSema);


    return ERR_OK;
}


/**
 * User Control 처리
 *
 * @param	control		User control
 * @param	pvArg		var arg
 * @return	ERR_OK / ERR_FAIL
 */
static HERROR local_ttx_Control (TTX_Control_t control, HUINT32 ulArg)
{
	TTX_Context_t 		*pCt 	= NULL;
	TTX_Controller_t	*pCtrl	= NULL;

	pCt 	= local_ttx_GetContext();
	pCtrl	= &pCt->controller;
	pCtrl->ulElapsedSec = 0;

	switch (control)
	{
	case eTTX_CONTROL_NUMERIC:
		return local_ttx_Control_NUMERIC(pCt, pCtrl, (HUINT8)ulArg);

	case eTTX_CONTROL_COLORKEY:
		return local_ttx_Control_COLORKEY(pCt,pCtrl, ulArg);

	case eTTX_CONTROL_ARROWKEY:
		return local_ttx_Control_ARROWKEY(pCt, pCtrl, ulArg);

	default:
		return local_ttx_Control_FUNCTION(pCt, pCtrl, control, ulArg);
	}
	return ERR_OK;
}


/**
 * START 메시지를 핸들링
 *
 * @param	ttxMsg	텔레텍스트 메시지
 * @return	ERR_OK / ERR_FAIL
 */
static HERROR local_ttx_HandleMsg_START(TTX_Msg_t *ttxMsg)
{
	HERROR		hErr;
	TTX_Context_t *pCt = NULL;

#if 0
	/* Analog channel에서는 Demux를 사용하지 않는다. */
	if ( ttxMsg->startMsg.eType == eTTX_TYPE_DIGITAL )
	{
		hErr = MWC_RM_GetResourceId(ulActionId, eRmItem_DEMUX, (HINT32 *)&ulDemuxId);
		if(hErr != ERR_OK)
			  HxLOG_Critical("[%s:%d]\n",__FUNCTION__,__LINE__);
		HxLOG_Print("MESSAGE : CMD_TTX_START, hAction:0x%x, Demux ID:%d, PID:%d\n",
								(int)ttxMsg->startMsg.hAction, (int)ulDemuxId, (int)ttxMsg->startMsg.usPid);
	}

	/* Session expire 검사하고... */
	if ( MWC_RM_CheckIfLatestActionHandle(ttxMsg->startMsg.hAction) != ERR_OK )
	{
		HxLOG_Print("\tAction handle 0x%x is already expired\n", (int)ttxMsg->startMsg.hAction));
        return ERR_FAIL;
	}
#endif
	pCt = local_ttx_GetContext();
	if ( pCt == NULL )
	{
        HxLOG_Critical("[%s:%d]\n",__FUNCTION__,__LINE__);
        return ERR_FAIL;
	}

    /* 이미 running 중이라면 아무런 동작을 할 필요가 없다. */
	if ( pCt->status == eTTX_Status_Running )
	{	/* Runing or Ready 상태는 Master만 올 수 있다. */
	    HxLOG_Print("Check Action status = (%d)\n", (int)pCt->status);

		/* 여기로 오면 안 되지만 온다면 Hide & stop후 아래서 다시 start만 한다. */
		hErr = local_ttx_Hide();
		if(hErr != ERR_OK)
			HxLOG_Critical("[%s:%d]\n",__FUNCTION__,__LINE__);

		local_ttx_Stop();

		if(hErr != ERR_OK)
			HxLOG_Critical("[%s:%d]\n",__FUNCTION__,__LINE__);
	}
	else if ( pCt->status == eTTX_Status_Ready )
	{
		/* Start 중에 Start는 Restart해 준다. 따라서 master인 경우 여기서 stop하고 아래서
		   다시 start를 해 준다. */
		local_ttx_Stop();
	}
	local_ttx_SetContextStatus(pCt, eTTX_Status_Waiting);

	local_ttx_Start();	/* PID가 NULL 인 경우 Ready 상태로 그냥 있음. */
    local_ttx_SetContextStatus(pCt, eTTX_Status_Ready);

    return ERR_OK;
}


/**
 * STOP 메시지를 핸들링
 *
 * @param	ttxMsg	텔레텍스트 메시지
 * @return	ERR_OK / ERR_FAIL
 */static HERROR local_ttx_HandleMsg_STOP(TTX_Msg_t *ttxMsg)
{
	TTX_Context_t *pCt = NULL;

	HxLOG_Print("MESSAGE : CMD_TTX_STOP\n");

    pCt = local_ttx_GetContext();
    if ( pCt == NULL )
    {
        return ERR_FAIL;
    }


    switch (pCt->status)
    {
    /* Running인 경우 Hide & Stop */
	case eTTX_Status_Running :
		local_ttx_Hide();
	/* Ready인 경우 Stop */
	case eTTX_Status_Ready :
		local_ttx_Stop();
		break;
	default :
		break;
    }

	pCt->usPageNum	= 0xFFFF;
	pCt->usSubcode	= 0xFFFF;

    local_ttx_SetContextStatus(pCt, eTTX_Status_None);

	return ERR_OK;
}


/**
 * SHOW 메시지를 핸들링
 *
 * @param	ttxMsg	텔레텍스트 메시지
 * @return	ERR_OK / ERR_FAIL
 */
static void local_ttx_HandleMsg_SHOW(TTX_Msg_t *ttxMsg)
{
	HERROR			hErr;
	HUINT32 		EBUsubtitlepage = 88;
	//HUINT32			ulActionId	= MWC_RM_GetActionId(ttxMsg->showMsg.hAction);
	TTX_Context_t 	*pCt 		= NULL;
	HUINT16			usPageNum	= ttxMsg->showMsg.usPageNum;
	HUINT16			usSubcode	= ttxMsg->showMsg.usSubcode;

	HxLOG_Print("MESSAGE : CMD_TTX_SHOW\n");



	pCt = local_ttx_GetContext();
	if ( pCt == NULL )
	{
       HxLOG_Critical("[%s:%d]\n",__FUNCTION__,__LINE__);
        goto ShowFail;
	}

	if ( pCt->status == eTTX_Status_None )
	{
		HxLOG_Print("\tThis service  is not yet started.\n");
		goto ShowFail;
	}

	if ( ttxMsg->showMsg.bInitPage )
	{
		VK_SEM_Get(s_ulTtxDecSema);
		hErr	= (HERROR)TTX_DECODER_InitPage(s_hTtxDecoder, &usPageNum, &usSubcode);
		VK_SEM_Release(s_ulTtxDecSema);
		if (hErr != ERR_OK)
		{
			usPageNum	= 100;
			usSubcode	= TTX_IGNORE_SUBCODE;
		}
	}

    /* 이미 running 중이라면 페이지 전환으로 동작 */
	if ( pCt->status == eTTX_Status_Running )
	{
		local_ttx_SetPageFilter(pCt, usPageNum, usSubcode, TRUE);
        return;
	}

	pCt->usPageNum		= usPageNum;
	pCt->usSubcode		= usSubcode;
	pCt->bEbuSbtl 		= ttxMsg->showMsg.bSbtl;
	pCt->bVariableFont	= ttxMsg->showMsg.bVariableFont;

	if(pCt->bEbuSbtl  == TRUE ) //EBU Subtitle page..
	{
		HUINT32	ulViewId;
		OxMediaPlay_MediaType_e		 	eMediaType;
		HINT32 							nTrackIndex =0;
		OxMediaPlay_Component_t		  	stComponet;

		APK_MEDIA_PLAY_GetMainViewId(&ulViewId);
		APK_MEDIA_PLAY_GetPlayType(ulViewId, &eMediaType);

		HxLOG_Print("FONT TYPE [%s]\n",(pCt->bVariableFont == TRUE)?"Variable":"Fixed");
		if (pCt->bVariableFont == TRUE)
		{
			TTX_BITSET(pCt->controller.state, eTTX_STATE_VAR_FONT);
		}
		else
		{
			TTX_BITCLR(pCt->controller.state, eTTX_STATE_VAR_FONT);
		}

		if (APK_MEDIA_PLAY_GetComponentIndex(ulViewId, eOxMP_COMPONENT_SUBTITLE, &nTrackIndex, NULL) == ERR_OK)
		{
			HxLOG_Warning("[%s;%d] <--- getCurrentComponent Index : [%d]\n", __FUNCTION__, __LINE__, nTrackIndex);
			if (nTrackIndex != -1)
			{
				if (APK_MEDIA_PLAY_GetComponent (ulViewId, eOxMP_COMPONENT_SUBTITLE, nTrackIndex, &stComponet) == ERR_OK)
				{
					if(stComponet.stSubtitle.eSubtitleType == eOxMP_SUBTITLETYPE_Ebu)
					{
						EBUsubtitlepage = stComponet.stSubtitle.utArg.stEbuInfo.nPageNum / 16  * 10;
						EBUsubtitlepage += stComponet.stSubtitle.utArg.stEbuInfo.nPageNum % 16;
						EBUsubtitlepage += (stComponet.stSubtitle.utArg.stEbuInfo.nMagazineNum * 100);
						HxLOG_Warning("[%s:%d] eMediaType[%d],EBUsubtitlepage[%d]\n",__FUNCTION__,__LINE__,eMediaType,EBUsubtitlepage);
					}
				}

				HxLOG_Print("[%s:%d]EBUsubtitlepage[%d]\n",__FUNCTION__,__LINE__,EBUsubtitlepage);
				pCt->usPageNum		= EBUsubtitlepage;
				pCt->usSubcode		= TTX_IGNORE_SUBCODE;
			} else
				HxLOG_Error("[%s;%d] fail to get component info [%d]\n", __FUNCTION__, __LINE__, nTrackIndex);
		} else
			HxLOG_Error("[%s;%d] fail to get component index \n", __FUNCTION__, __LINE__);


	}
#if 0
    /* 시작 하려는 action이 Master가 아니면 동작하지 않는다. */
    if ( ulActionId != s_ulMasterActionid )
    {
        HxLOG_Print("\tThis action is not a master action or is not enabled. No effect.\n");
        pCt->bRuningAtMaster = TRUE;

        goto ShowFail;
    }
#endif
    local_ttx_SetContextStatus(pCt, eTTX_Status_Running);

	/* 요청된 action 이 master action인 경우에만 서비스 들어간다. */
	local_ttx_Show();
	return;

ShowFail:
    HxLOG_Print("\t Teletext -ShowFail.\n");

    ONDK_GWM_SendMessage(NULL,MSG_APP_TELETEXT_SHOWFAIL,HANDLE_NULL,0,0,0);

}


/**
 * HIDE 메시지를 핸들링
 *
 * @param	ttxMsg	텔레텍스트 메시지
 * @return	ERR_OK / ERR_FAIL
 */
static void local_ttx_HandleMsg_HIDE(TTX_Msg_t *ttxMsg)
{
	HERROR		hErr;
	TTX_Context_t *pCt = NULL;

	HxLOG_Print("MESSAGE : CMD_TTX_HIDE\n");


	pCt = local_ttx_GetContext();
	if ( pCt == NULL )
	{
        HxLOG_Critical("[%s:%d]\n",__FUNCTION__,__LINE__);
		goto ReleaseSema;
	}

    /* 이미 running 아니라면 아무런 동작을 할 필요가 없다. */
	if ( pCt->status != eTTX_Status_Running )
	{
		HxLOG_Print("is not showing!!\n");
		goto ReleaseSema;
	}
#if 0
    /* 시작 하려는 action이 Master가 아니면 동작하지 않는다. */
    if ( ulActionId != s_ulMasterActionid )
    {
        TTXPrint(DBG_PRINT, ("\tThis action is not a master action or is not enabled. No effect.\n"));
        pCt->bRuningAtMaster = FALSE;

		goto ReleaseSema;
    }
#endif
    local_ttx_SetContextStatus(pCt, eTTX_Status_Ready);

	/* 요청된 action 이 master action인 경우에만 서비스 들어간다. */
	hErr = local_ttx_Hide();
	if ( hErr != ERR_OK )
	{
	    HxLOG_Critical("[%s:%d]\n",__FUNCTION__,__LINE__);
		goto ReleaseSema;
	}

ReleaseSema:
	return;
#if 0
    /* Sync API 모드인 경우는 expire된 action handle 이라도 반드시 sema4를 릴리즈 해야 한다. 안그러면 시스템이 서버린다. */
    if ( ttxMsg->hideMsg.syncMode == eSyncMode )
    {
        VK_SEM_Release(s_ulMwTtxSyncSema);
    }
#endif
}


/**
 * CONTROL 메시지를 핸들링
 *
 * @param	ttxMsg	텔레텍스트 메시지
 * @return	ERR_OK / ERR_FAIL
 */
static void local_ttx_HandleMsg_SET_CONTROL(TTX_Msg_t *ttxMsg)
{
	//HUINT32			ulActionId = MWC_RM_GetActionId(ttxMsg->controlMsg.hAction);
	TTX_Context_t	*pCt = NULL;

	HxLOG_Print("MESSAGE : CMD_TTX_SET_CONTROL\n");
#if 0
	/* Session expire 검사하고... */
	if ( MWC_RM_CheckIfLatestActionHandle(ttxMsg->controlMsg.hAction) != ERR_OK )
	{
		TTXPrint(DBG_PRINT, ("\tAction handle 0x%x is already expired\n", (int)ttxMsg->controlMsg.hAction));
		goto ReleaseSema;
	}
#endif
	if (ttxMsg->controlMsg.ulVersion != s_ulControlVersion)
	{
		HxLOG_Print("\tThis control is older then current version\n");
		goto ReleaseSema;
	}

	pCt = local_ttx_GetContext();
	if ( pCt == NULL )
	{
        HxLOG_Critical("[%s:%d]\n",__FUNCTION__,__LINE__);
		goto ReleaseSema;
	}

	pCt->control = ttxMsg->controlMsg.control;
	pCt->ulArg = ttxMsg->controlMsg.ulArg;

    /* 이미 running 아니라면 아무런 동작을 할 필요가 없다. */
	if ( pCt->status != eTTX_Status_Running )
	{
		HxLOG_Print("is not showing!!\n");
		goto ReleaseSema;
	}
#if 0
    /* 시작 하려는 action이 Master가 아니면 동작하지 않는다. */
    if ( ulActionId != s_ulMasterActionid )
    {
        TTXPrint(DBG_PRINT, ("\tThis action is not a master action or is not enabled. No effect.\n"));
		goto ReleaseSema;
    }
#endif


    local_ttx_Control( ttxMsg->controlMsg.control, ttxMsg->controlMsg.ulArg);

ReleaseSema:
	return;
#if 0
    /* Sync API 모드인 경우는 expire된 action handle 이라도 반드시 sema4를 릴리즈 해야 한다. 안그러면 시스템이 서버린다. */
    if ( ttxMsg->controlMsg.syncMode == eSyncMode )
    {
        VK_SEM_Release(s_ulMwTtxSyncSema);
    }
#endif
}



/**
 * Set Font 메시지를 핸들링
 *
 * @param	ttxMsg	텔레텍스트 메시지
 * @return	ERR_OK / ERR_FAIL
 */
static void local_ttx_HandleMsg_SET_FONT(TTX_Msg_t *ttxMsg)
{
	TTX_Context_t	*pCt = NULL;

	HxLOG_Print("MESSAGE : CMD_TTX_SET_FONT(bVariableFont:%d)\n", (int)ttxMsg->setFontMsg.bVariableFont);

	pCt = local_ttx_GetContext();
	if ( pCt->status != eTTX_Status_Running )
	{
		return;
	}

	pCt->bVariableFont = ttxMsg->setFontMsg.bVariableFont;
	if (pCt->bVariableFont == TRUE)
	{
		TTX_BITSET(pCt->controller.state, eTTX_STATE_VAR_FONT);
	}
	else
	{
		TTX_BITCLR(pCt->controller.state, eTTX_STATE_VAR_FONT);
	}
}




static void local_ttx_ServiceTask(void *arg)
{
	HERROR			hError;
    TTX_Msg_t     	ttxMsg;
    TTX_Context_t 	*pCt = NULL;
	HxLOG_Trace();
    for (;;)
    {
		hError	= VK_MSG_Receive(local_ttx_GetTltxtMsgQId(), &ttxMsg, sizeof(TTX_Msg_t));
		if (hError != ERR_OK)
		{
			continue;
		}

		TTX_CTX_SEM_GET;
		HxLOG_Info("[%s:%d]CMD[%d]\n",__FUNCTION__,__LINE__,ttxMsg.ulCmd);
        switch(ttxMsg.ulCmd)
        {
        case CMD_TTX_START :
            local_ttx_HandleMsg_START(&ttxMsg);
            break;

        case CMD_TTX_STOP :
            local_ttx_HandleMsg_STOP(&ttxMsg);
            break;

		case CMD_TTX_SHOW :
			local_ttx_HandleMsg_SHOW(&ttxMsg);
			break;
		case CMD_TTX_HIDE :
			local_ttx_HandleMsg_HIDE(&ttxMsg);
			break;
#if 0
        case CMD_TTX_CHANGE_PID :
            local_ttx_HandleMsg_CHANGE_PID(&ttxMsg);
            break;

        case CMD_TTX_SET_MASTER :
            local_ttx_HandleMsg_SET_MASTER(&ttxMsg);
            break;
#endif
		case CMD_TTX_SET_CONTROL :
			local_ttx_HandleMsg_SET_CONTROL(&ttxMsg);
			break;

		case CMD_TTX_SET_FONT :
			local_ttx_HandleMsg_SET_FONT(&ttxMsg);
			break;
#if 0
		case CMD_TTX_ON_OFF_DEC:
			local_ttx_HandleMsg_SET_OnOffDec(&ttxMsg);
			break;
#endif
		case TTXM_PAGE_REFRESH :
		    pCt = local_ttx_GetContext();
		    if ( pCt == NULL )
		    {
		        HxLOG_Critical("[%s:%d]\n",__FUNCTION__,__LINE__);
				break;
		    }

			(void) local_ttx_RefreshPage(pCt, ttxMsg.refreshMsg.refresh, ttxMsg.refreshMsg.ulVersion);
			break;

		case TTXM_PAGE_UPDATED:
			pCt	= local_ttx_GetContext();
			if (pCt == NULL)
			{
				HxLOG_Assert(0);
				break;
			}
			(void) local_ttx_UpdatePage(pCt, ttxMsg.updateMsg.update);
			break;

		case TTXM_CYCLE:
			pCt	= local_ttx_GetContext();
			if (pCt == NULL)
			{
				HxLOG_Assert(0);
				break;
			}
			(void ) local_ttx_Cycle(pCt);
			break;

        default :
            HxLOG_Assert(0);
            break;
        }

		TTX_CTX_SEM_RELEASE;
    }

	if (s_pucTtxMemory)
	{
		AP_Free(s_pucTtxMemory);
		s_pucTtxMemory	= NULL;
	}
}
#if 0
void	local_ttx_GetEBUTeletextPageNum(HUINT8 *PES)
{
	HUINT8 *pucPacket,pucPES[TTX_PACKET_LENGTH];
	HUINT8	ucM;	// Magazine
	HUINT8	ucY;	// Packet number

	if (PES == NULL) return;

	HxSTD_MemCopy(pucPES,PES,TTX_PACKET_LENGTH);
	// if LSB first (Framing Code)
	if (pucPES[0] == 0x27)
	{
		TTX_ERRCHK_ReverseBit(pucPES, pucPES, TTX_PACKET_LENGTH);
	}
	// CHECK Framing Code
	if (pucPES[0] == 0xE4)
	{
		pucPacket = pucPES +1;
		if (TTX_ERRCHK_Hamming84(pucPacket, pucPacket, 2) != 0)
			return;
		// parse magazine and packet number
		ucM = pucPacket[0] & 0x07;
		ucY = (pucPacket[0] >> 3) | (pucPacket[1] << 1);
		// Skip Teletext Packet Address(Magazine, Packet number).
		pucPacket += 2;

		if (ucY == 0)		// header packet
		{
			if (TTX_ERRCHK_Hamming84(pucPacket, pucPacket, 8) != 0)
				return;
			// Time Filling Header
			if (pucPacket[0] == 0x0F && pucPacket[1] == 0x0F)
				return;
			m_EBUTeletextPageNum	= ucM * 100 + pucPacket[1] * 10 + pucPacket[0];

			HxLOG_Print("__________eub......[%d]\n",m_EBUTeletextPageNum);
		}
	}
}
#endif





/*******************************************************************/
/********************      API Functions   		*************************/
/*******************************************************************/
#define _______API_FUNCTIONS_____________
void   Teletext_Init(void)
{
	HERROR		hErr;

	//HLIB_LOG_SetLevel(HxLOG_DOMAIN,HxLOG_ALL);
	if(VK_SEM_Create((unsigned long *)&s_ulTtxDecSema, "TTX_SVC_SEM", VK_SUSPENDTYPE_PRIORITY) != ERR_OK)
		HxLOG_Critical("[%s:%d] Critical Eror!!!\n", __FUNCTION__, __HxLINE__);

#if defined(CONFIG_MWC_OSD_SBTL_ON_UIPLANE)
	if(VK_SEM_Create((unsigned long *)&s_ulTtxCtxSema, "TTX_CTX_SEM", VK_SUSPENDTYPE_PRIORITY) !=ERR_OK)
		HxLOG_Critical("create TTX_CTX_SEM fail..");
#endif


	/* Teletext service message queue 생성 */
	hErr = (HUINT32)VK_MSG_Create(MW_TLTXT_MSGQ_SIZE,
				 		 				sizeof(TTX_Msg_t),
				 		 				"TltxtMsgQ",
				 		 				(unsigned long *)&s_ulTtxMsgQId,
				 		 				VK_SUSPENDTYPE_FIFO);


	if ( hErr != VK_OK )
	{
	    HxLOG_Critical("Teletext_Init MSGQ fail..");
	}

	/* Teletext service task 생성 */
	hErr = (HUINT32)VK_TASK_Create(local_ttx_ServiceTask,
									40,	0x4000,
									"teletextsvc",
									NULL,&s_ulTtxTaskId,
									0);


	if ( hErr != VK_OK )
	{
		HxLOG_Critical("create Teletext service task fail..");
	}

	hErr = VK_TASK_Start(s_ulTtxTaskId);
	if ( hErr != VK_OK )
	{
	    HxLOG_Critical("Teletext_VK_TASK_Start fail..");
	}
	TTxMenuObject_Create();
}


HERROR Teletext_StartService(void)
{
	HERROR		hErr;
	TTX_Msg_t	  ttxMsg;
	TTX_Context_t *pCt = NULL;
	pCt = local_ttx_GetContext();
	if ( pCt == NULL )
	{
		return ERR_FAIL;
	}

	HxLOG_Print("Teletext_StartService\n");

	 ttxMsg.startMsg.ulCmd = CMD_TTX_START;
	//  ttxMsg.startMsg.hAction = hAction;
	//  ttxMsg.startMsg.usPid = usPid;
	//  ttxMsg.startMsg.eType = ulType;

	hErr = VK_MSG_Send(local_ttx_GetTltxtMsgQId(), &ttxMsg, sizeof(TTX_Msg_t));
	if ( hErr != VK_OK )
	{
		HxLOG_Print("Error from VK_MSG_Send() in SVC_TTX_StartService().\n");
		return ERR_FAIL;
	}
	return ERR_OK;

}
HERROR Teletext_StopService(void)
{
	TTX_Msg_t		ttxMsg;
	HUINT32 		hErr;

	HxLOG_Print("Teletext_StopService\n");

	ttxMsg.stopMsg.ulCmd = CMD_TTX_STOP;

	hErr = VK_MSG_Send(local_ttx_GetTltxtMsgQId(), &ttxMsg, sizeof(TTX_Msg_t));
	if ( hErr != VK_OK )
	{
		HxLOG_Print("Error from VK_MSG_Send() in SVC_TTX_StopService().\n");
		return ERR_FAIL;
	}

	return ERR_OK;

}

HERROR	Teletext_Decoding (HUINT8 *pucPES)
{
	HUINT32	ulLen;
	HUINT32	ulPacketCount;
	HUINT8	ucDataUnitId;

	//HLIB_LOG_SetLevel(HxLOG_DOMAIN, HxLOG_FATAL);
	if(s_hTtxDecoder == HANDLE_NULL) return ERR_FAIL;
	//	return Teletext_StartService(hwindow);

 	VK_SEM_Get(s_ulTtxDecSema);

	/* PES Header size는 0x24. */
	if (pucPES[8] != TTX_PES_HEADER_DATA_LENGTH)
	{
		VK_SEM_Release(s_ulTtxDecSema);
		return ERR_FAIL;
	}

	/* TTX Data Length */
	ulLen = ((pucPES[4] << 8) | pucPES[5]) - (TTX_PES_DATA_FIELD_INDEX - 6/* PES의 Length까지의 길이 */);

	/* TTX data 처음 field로...*/
	pucPES += TTX_PES_DATA_FIELD_INDEX;

	/*
	 *  ETSI EN 300 472 V1.3.1 Table 3: data_identifier
	 *  0x00 to 0x0F : reserved for future use
	 *  0x10 to 0x1F : EBU data
	 *  0x20 to 0x7F : reserved for future use
	 *  0x80 to 0xFF : user defined
	 */
	if ( (ulLen <= TTX_PES_DATA_UNIT_FIELD_SIZE)
		|| (*pucPES < 0x10)
		|| (*pucPES > 0x1F) )
	{
		VK_SEM_Release(s_ulTtxDecSema);
		return ERR_FAIL;
	}

	/* Skip data_identifier */
	pucPES++;
	ulLen--;

	/* 나머지 Data는 46배수가 되어야만 한다. */
	if ( (ulLen % TTX_PES_DATA_UNIT_FIELD_SIZE) != 0 )
	{
		VK_SEM_Release(s_ulTtxDecSema);
		return ERR_FAIL;
	}

	ulPacketCount	= ulLen / TTX_PES_DATA_UNIT_FIELD_SIZE;
	while (ulPacketCount--)
	{
		ucDataUnitId	= pucPES[0];
		if (pucPES[1] == TTX_PES_DATA_UNIT_LENGTH
			&& (	ucDataUnitId == TTX_PES_EBU_TELETEXT			// EBU Teletext non-subtitle data
				||	ucDataUnitId == TTX_PES_EBU_SUBTITLE			// EBU Teletext subtitle data
				|| (0x80 <= ucDataUnitId && ucDataUnitId <= 0xFE)))	// user defined (work around)
		{
			TTX_DECODER_Demux(s_hTtxDecoder, &pucPES[3]);
		}
		/*
		else {
			0x00, 0x01 : reserved for future use
			0x04 ~0x7F : reserved for future use
			0xFF : data_unit for stuffing
		}
		*/
		pucPES += TTX_PES_DATA_UNIT_FIELD_SIZE;
	}
	VK_SEM_Release(s_ulTtxDecSema);
	return ERR_OK;
}


HERROR Teletext_Show(HUINT8 ucMagNum,HUINT16 usPageNum, HUINT16 usSubcode, HBOOL bSbtl)
{
	TTX_Msg_t			 ttxMsg;
	HUINT32			 hErr;
//	HUINT32			 ulActionId = 0;
//	SI_ActionType_t	 eActionType = eSI_ACTION_NONE;

	HxLOG_Print("MW_TTX_ShowPage\n");
	ttxMsg.showMsg.ulCmd		 = CMD_TTX_SHOW;
	ttxMsg.showMsg.usSubcode	 = (usSubcode == 0) ? TTX_IGNORE_SUBCODE : usSubcode;
	ttxMsg.showMsg.bSbtl		 = bSbtl;
#if 0
	ulActionId = MWC_RM_GetActionId(hAction);
	eActionType = SI_MAIN_GetActTypebyActID(ulActionId);
	if(eActionType == eSI_CANAL_READY_LIVE) /* in case of TNTSAT Canal Ready, page number 0x88 is encoded 0x58 instead of 0x88 */
	 	ttxMsg.showMsg.usPageNum = TTX_HEX_ADDR_TO_DEC_TNTSAT(ucMagNum, usPageNum);
	else
#endif
	 	ttxMsg.showMsg.usPageNum = TTX_HEX_ADDR_TO_DEC(ucMagNum, usPageNum);

	if (ucMagNum == TTX_NULL_MAGAZINE || usPageNum == TTX_NULL_PAGE)
	{
	 	ttxMsg.showMsg.bInitPage	 = TRUE;
	}
	else
	{
	 	ttxMsg.showMsg.bInitPage	 = FALSE;
	}

	ttxMsg.showMsg.bVariableFont = s_bVariableFont;

	hErr = VK_MSG_Send(local_ttx_GetTltxtMsgQId(), &ttxMsg, sizeof(TTX_Msg_t));
	if ( hErr != VK_OK )
	{
	 	HxLOG_Print("Error from VK_MSG_Send() in SVC_TTX_StartService().\n");
	 	return ERR_FAIL;
	}

	return ERR_OK;
}


HERROR Teletext_Hide(void)
{
    TTX_Msg_t		ttxMsg;
    HUINT32			hErr;

    HxLOG_Print("Teletext_Hide\n");

    ttxMsg.hideMsg.ulCmd = CMD_TTX_HIDE;

    hErr = VK_MSG_Send(local_ttx_GetTltxtMsgQId(), &ttxMsg, sizeof(TTX_Msg_t));
    if ( hErr != VK_OK )
    {
        HxLOG_Print("Error from VK_MSG_Send() in SVC_TTX_StartService().\n");
        return ERR_FAIL;
    }
    return ERR_OK;
}


HERROR Teletext_GetPageAddress (HUINT8 *pucM, HUINT8 *pucX, HUINT16 *pusSubcode)
{
	TTX_Context_t		*pCt		= NULL;

	HxLOG_Print("MW_TTX_GetPageAddress\n");

	if (pucM == NULL || pucX == NULL || pusSubcode == NULL)
	{
		return ERR_FAIL;
	}

	VK_SEM_Get(s_ulTtxDecSema);

	pCt = local_ttx_GetContext();
	if ( (pCt == NULL) || (pCt->status != eTTX_Status_Running) )
	{
		VK_SEM_Release(s_ulTtxDecSema);
		return ERR_FAIL;
	}

	if (pCt->pTtxPage->bEnabled == FALSE)
	{
		VK_SEM_Release(s_ulTtxDecSema);
		return ERR_FAIL;
	}

	*pucM		= TTX_DEC_ADDR_TO_HEX_M(pCt->pTtxPage->usPageNum);
	*pucX		= TTX_DEC_ADDR_TO_HEX_X(pCt->pTtxPage->usPageNum);
	*pusSubcode	= pCt->pTtxPage->usSubcode;

	VK_SEM_Release(s_ulTtxDecSema);
	return ERR_OK;
}

HERROR Teletext_SetSutitleFontType(HBOOL bVariableFont)
{
	TTX_Context_t	*pCt = NULL;

	HxLOG_Print("Teletext_SetSutitleFontType(bVariableFont:%d)\n", (int)bVariableFont);

	pCt = local_ttx_GetContext();
#if 0
	if ( pCt->status != eTTX_Status_Running )
	{
		return ERR_FAIL;
	}
#endif

	pCt->bVariableFont = bVariableFont;
	s_bVariableFont = bVariableFont;


	if (pCt->bVariableFont == TRUE)
	{
		TTX_BITSET(pCt->controller.state, eTTX_STATE_VAR_FONT);
	}
	else
	{
		TTX_BITCLR(pCt->controller.state, eTTX_STATE_VAR_FONT);
	}
	return ERR_OK;
}


HERROR Teletext_SetControl (TTX_Control_t control, HUINT32 ulArg)
{
	TTX_Msg_t		ttxMsg;
	HUINT32			ulErr;

	HxLOG_Print("MW_TTX_SetControl\n");

	ttxMsg.controlMsg.ulCmd		= CMD_TTX_SET_CONTROL;
	//ttxMsg.controlMsg.hAction	= hAction;
	ttxMsg.controlMsg.control	= control;
	ttxMsg.controlMsg.ulArg		= ulArg;
	ttxMsg.controlMsg.ulVersion	= ++s_ulControlVersion;
	//ttxMsg.controlMsg.syncMode 	= syncMode;

	ulErr = VK_MSG_Send(local_ttx_GetTltxtMsgQId(), &ttxMsg, sizeof(TTX_Msg_t));
	if ( ulErr != VK_OK )
	{
		HxLOG_Print("Error from VK_MSG_Send() in MW_TTX_SetControl().\n");
		return ERR_FAIL;
	}
	return ERR_OK;
}


HERROR Teletext_GetControl (TTX_Control_t control, void *pvValue)
{
	HERROR				hErr		= ERR_OK;
	TTX_Context_t		*pCt		= NULL;
	TTX_Controller_t	*pCtrl		= NULL;
	TTX_State_t			state;

	HxLOG_Print("MW_TTX_GetControl\n");

	if (pvValue == NULL)
	{
		return ERR_FAIL;
	}

	VK_SEM_Get(s_ulTtxDecSema);

	pCt = local_ttx_GetContext();
	if ( (pCt == NULL) || (pCt->status != eTTX_Status_Running) )
	{
		VK_SEM_Release(s_ulTtxDecSema);
		return ERR_FAIL;
	}

	pCtrl	= &pCt->controller;
	state	= pCtrl->state;

	switch (control)
	{
	case eTTX_CONTROL_CANCEL:
	{
		HBOOL	*pbCancel	= (HBOOL *)pvValue;

		*pbCancel	= (state & eTTX_STATE_CANCEL) ? TRUE : FALSE;
		break;
	}

	case eTTX_CONTROL_MIX:
	{
		TTX_Mix_t	*pMix	= (TTX_Mix_t *)pvValue;

		*pMix	= pCtrl->mix;
		break;
	}

	case eTTX_CONTROL_REVEAL:
	{
		HBOOL	*pbReveal	= (HBOOL *)pvValue;

		*pbReveal	= (state & eTTX_STATE_REVEAL) ? TRUE : FALSE;
		break;
	}

	case eTTX_CONTROL_SUBPAGE:
	{
		HBOOL	*pbSubpage	= (HBOOL *)pvValue;

		*pbSubpage	= (state & eTTX_STATE_SUBCODE) ? TRUE : FALSE;
		break;
	}

	case eTTX_CONTROL_DOUBLE:
	{
		TTX_DoubleSize_t	*pDouble	= (TTX_DoubleSize_t *)pvValue;

		*pDouble	= pCtrl->doubleSize;
		break;
	}

	case eTTX_CONTROL_HOLD:
	{
		HBOOL	*pbHold		= (HBOOL *)pvValue;

		*pbHold	= (state & eTTX_STATE_HOLD) ? TRUE : FALSE;
		break;
	}

	case eTTX_CONTROL_NAVI:
	{
		TTX_Navigation_t	*pNavi	= (TTX_Navigation_t *)pvValue;

		*pNavi	= pCtrl->navi;
		break;
	}

	case eTTX_CONTROL_LANGUAGE:
	{
		TTX_Language_t	*pLang	= (TTX_Language_t *)pvValue;

		*pLang	= pCtrl->langGroup;
		break;
	}

	default:
		hErr	= ERR_FAIL;
		break;
	}
	VK_SEM_Release(s_ulTtxDecSema);

	return hErr;
}


/**
 * Set Font
 *
 */
HERROR Teletext_FONT(HBOOL bVariableFont)
{
    HUINT32			hErr;
    TTX_Msg_t		ttxMsg;

    ttxMsg.setFontMsg.ulCmd = CMD_TTX_SET_FONT;
    ttxMsg.setFontMsg.bVariableFont = bVariableFont;

    hErr = VK_MSG_Send(local_ttx_GetTltxtMsgQId(), &ttxMsg, sizeof(TTX_Msg_t));
    if ( hErr != VK_OK )
    {
        HxLOG_Print("Error from MW_TTX_SetMasterAction() in MW_TLTXT_Disable().\n");
        return ERR_FAIL;
    }
	return ERR_OK;
}

/*******************************************************************/
/******************** CMD  API Functions 	**************************/
/*******************************************************************/
#define ______CMD_API_FUNCTIONS_____________
#if defined(CONFIG_DEBUG)

int local_ttx_CMD_Show(void *szArgList)
{
	HUINT8			*aucArg_1=NULL,*aucArg_2=NULL;
	HUINT8		   aucStr1[20],aucStr2[20];
//	unsigned long   arg1 = 0;
	unsigned long	arg2 = 0;
	char 		   *szCmd = (char *)szArgList;
	HBOOL			subtitle;
//	HBOOL			bVariableFont;
	HxLOG_Trace();
	/* 1st argument */
	aucArg_1 = HLIB_CMD_GetNextArg((HCHAR**)&szCmd);
	if( aucArg_1 != NULL )
	{
		if( sscanf(aucArg_1, "%s", (char *)&aucStr1) == 0 )
			return HxCMD_ERR;
//		arg1 = atoi(aucStr1);
//		if(arg1 == 1 )
//			bVariableFont = TRUE;
//		else
//			bVariableFont = FALSE;
	}
	else
		return HxCMD_ERR;

	/* 2st argument */
	aucArg_2 = HLIB_CMD_GetNextArg((HCHAR**)&szCmd);
	if( aucArg_2 != NULL )
	{
		if( sscanf(aucArg_2, "%s", (char *)&aucStr2) == 0 )
			return HxCMD_ERR;
		arg2 = atoi(aucStr2);
		if(arg2 == 1 ) subtitle = TRUE;
		else		   subtitle = FALSE;
	}
	else
		return HxCMD_ERR;

	if(Teletext_Show(TTX_NULL_MAGAZINE,TTX_NULL_PAGE ,TTX_NULL_SUBCODE,subtitle) ==ERR_FAIL)
		HLIB_CMD_Printf("Teletext_Show Error.. \n" );

	return HxCMD_OK;
}

int local_ttx_CMD_Hide(void *szArgList)
{
	//if(Teletext_Hide() ==ERR_FAIL)
	//	HLIB_CMD_Printf("Teletext_Hide Error.. \n" );
	Teletext_Hide();
	return HxCMD_OK;
}

int local_ttx_CMD_ColorKey(void *szArgList)
{
	HUINT8			*aucArg_1=NULL;
	HUINT8		   aucStr1[20];
	unsigned long   arg1 = 0;
	char 		   *szCmd = (char *)szArgList;
	HxLOG_Trace();

	/* 1st argument */
	aucArg_1 = HLIB_CMD_GetNextArg((HCHAR**)&szCmd);
	if( aucArg_1 != NULL )
	{
		if( sscanf(aucArg_1, "%s", (char *)&aucStr1) == 0 )
			return HxCMD_ERR;
		arg1 = atoi(aucStr1);
	}
	else
		return HxCMD_ERR;


	if(Teletext_SetControl(eTTX_CONTROL_COLORKEY,arg1)== ERR_FAIL)
		HLIB_CMD_Printf("Teletext_CMD_ColorKey Error!\n");

	return HxCMD_OK;
}

int local_ttx_CMD_ArrowKey(void *szArgList)
{
	HUINT8			*aucArg_1=NULL;
	HUINT8		   aucStr1[20];
	unsigned long   arg1 = 0;
	char 		   *szCmd = (char *)szArgList;
	HxLOG_Trace();

	/* 1st argument */
	aucArg_1 = HLIB_CMD_GetNextArg((HCHAR**)&szCmd);
	if( aucArg_1 != NULL )
	{
		if( sscanf(aucArg_1, "%s", (char *)&aucStr1) == 0 )
			return HxCMD_ERR;
		arg1 = atoi(aucStr1);
	}
	else
		return HxCMD_ERR;

	if(Teletext_SetControl(eTTX_CONTROL_ARROWKEY,arg1)== ERR_FAIL) //0:Left, 1:Right,2:Up, 3:Down,
		HLIB_CMD_Printf("Teletext_CMD_ColorKey Error!\n");

	return HxCMD_OK;
}

int local_ttx_CMD_Input(void *szArgList)
{
	HUINT8			*aucArg_1=NULL;
	HUINT8		   aucStr1[20];
	unsigned long   arg1 = 0;
	char 		   *szCmd = (char *)szArgList;
	HxLOG_Trace();

	/* 1st argument */
	aucArg_1 = HLIB_CMD_GetNextArg((HCHAR**)&szCmd);
	if( aucArg_1 != NULL )
	{
		if( sscanf(aucArg_1, "%s", (char *)&aucStr1) == 0 )
			return HxCMD_ERR;
		arg1 = atoi(aucStr1);
	}
	else
		return HxCMD_ERR;

	if(Teletext_SetControl(eTTX_CONTROL_NUMERIC,arg1)== ERR_FAIL)
		HLIB_CMD_Printf("Teletext_CMD_NUMBER Error!\n");

	return HxCMD_OK;
}


//ETC Functions...
int local_ttx_CMD_Index(void *szArgList)
{
	if(Teletext_SetControl(eTTX_CONTROL_INDEX,0)== ERR_FAIL)
		HLIB_CMD_Printf("Teletext_CMD_Index Error!\n");
	return HxCMD_OK;
}
int local_ttx_CMD_Cancel(void *szArgList)
{
	if(Teletext_SetControl(eTTX_CONTROL_CANCEL,0)== ERR_FAIL)
		HLIB_CMD_Printf("Teletext_CMD_Cancel Error!\n");
	return HxCMD_OK;
}

int local_ttx_CMD_Mix(void *szArgList)
{
	HUINT8			*aucArg_1=NULL;
	HUINT8		   aucStr1[20];
	unsigned long   arg1 = 0;
	HERROR			hErr		= ERR_OK;
	char 		   *szCmd = (char *)szArgList;
	HxLOG_Trace();

	/* 1st argument */
	aucArg_1 = HLIB_CMD_GetNextArg((HCHAR**)&szCmd);
	if( aucArg_1 != NULL )
	{
		if( sscanf(aucArg_1, "%s", (char *)&aucStr1) == 0 )
			return HxCMD_ERR;
		arg1 = atoi(aucStr1);
	}
	else
		return HxCMD_ERR;


	switch(arg1)
	{
	case 0:			hErr=Teletext_SetControl(eTTX_CONTROL_MIX,eTTX_MIX_0_PERCENT); break;
	case 10:		hErr=Teletext_SetControl(eTTX_CONTROL_MIX,eTTX_MIX_10_PERCENT); break;
	case 20:		hErr=Teletext_SetControl(eTTX_CONTROL_MIX,eTTX_MIX_20_PERCENT); break;
	case 30:		hErr=Teletext_SetControl(eTTX_CONTROL_MIX,eTTX_MIX_30_PERCENT); break;
	case 40:		hErr=Teletext_SetControl(eTTX_CONTROL_MIX,eTTX_MIX_40_PERCENT); break;
	case 50:		hErr=Teletext_SetControl(eTTX_CONTROL_MIX,eTTX_MIX_50_PERCENT); break;
	case 60:		hErr=Teletext_SetControl(eTTX_CONTROL_MIX,eTTX_MIX_50_PERCENT); break;
	case 70:		hErr=Teletext_SetControl(eTTX_CONTROL_MIX,eTTX_MIX_60_PERCENT); break;
	case 80:		hErr=Teletext_SetControl(eTTX_CONTROL_MIX,eTTX_MIX_70_PERCENT); break;
	case 90:		hErr=Teletext_SetControl(eTTX_CONTROL_MIX,eTTX_MIX_80_PERCENT); break;
	case 100:		hErr=Teletext_SetControl(eTTX_CONTROL_MIX,eTTX_MIX_90_PERCENT); break;
	}

	if(hErr== ERR_FAIL) 		HLIB_CMD_Printf("Teletext_CMD_Mix Error!\n");

	return HxCMD_OK;
}

int local_ttx_CMD_Reveal(void *szArgList)
{
	if(Teletext_SetControl(eTTX_CONTROL_REVEAL,0)== ERR_FAIL)
		HLIB_CMD_Printf("Teletext_CMD_Reveal Error!\n");
	return HxCMD_OK;
}

int local_ttx_CMD_Subpage(void *szArgList)
{
	if(Teletext_SetControl(eTTX_CONTROL_SUBPAGE,0)== ERR_FAIL)
		HLIB_CMD_Printf("Teletext_CMD_Subpage Error!\n");
	return HxCMD_OK;
}

int local_ttx_CMD_Double(void *szArgList)
{

	HUINT8			*aucArg_1=NULL;
	HUINT8		   aucStr1[20];
	unsigned long   arg1 = 0;
	char 		   *szCmd = (char *)szArgList;
	HxLOG_Trace();

	/* 1st argument */
	aucArg_1 = HLIB_CMD_GetNextArg((HCHAR**)&szCmd);
	if( aucArg_1 != NULL )
	{
		if( sscanf(aucArg_1, "%s", (char *)&aucStr1) == 0 )
			return HxCMD_ERR;
		arg1 = atoi(aucStr1);
	}
	else
		return HxCMD_ERR;

	if(Teletext_SetControl(eTTX_CONTROL_DOUBLE,arg1)== ERR_FAIL)
		HLIB_CMD_Printf("Teletext_CMD_Subpage Error!\n");
	return HxCMD_OK;
}


int local_ttx_CMD_Hold(void *szArgList)
{
	if(Teletext_SetControl(eTTX_CONTROL_HOLD,0)== ERR_FAIL)
		HLIB_CMD_Printf("Teletext_CMD_Subpage Error!\n");
	return HxCMD_OK;
}


int local_ttx_CMD_Navi(void *szArgList)
{

	HUINT8			*aucArg_1=NULL;
	HUINT8		   aucStr1[20];
	unsigned long   arg1 = 0;
	HERROR			hErr		= ERR_OK;
	char 		   *szCmd = (char *)szArgList;
	HxLOG_Trace();

	/* 1st argument */
	aucArg_1 = HLIB_CMD_GetNextArg((HCHAR**)&szCmd);
	if( aucArg_1 != NULL )
	{
		if( sscanf(aucArg_1, "%s", (char *)&aucStr1) == 0 )
			return HxCMD_ERR;
		arg1 = atoi(aucStr1);
	}else
		return HxCMD_ERR;


	switch(arg1)
	{
	case 0: hErr =Teletext_SetControl(eTTX_CONTROL_NAVI,eTTX_NAVI_FLOF); break;
	case 1: hErr =Teletext_SetControl(eTTX_CONTROL_NAVI,eTTX_NAVI_TOP); break;
	case 2: hErr =Teletext_SetControl(eTTX_CONTROL_NAVI,eTTX_NAVI_TRACE); break;
	case 3: hErr =Teletext_SetControl(eTTX_CONTROL_NAVI,eTTX_NAVI_LAST); break;
	}

	if(hErr== ERR_FAIL)
		HLIB_CMD_Printf("Teletext_CMD_Navi Error!\n");
	return HxCMD_OK;
}


int local_ttx_CMD_Language(void *szArgList)
{

	HUINT8			*aucArg_1=NULL;
	HUINT8		   aucStr1[20];
	unsigned long   arg1 = 0;
	HERROR			hErr		= ERR_OK;
	char 		   *szCmd = (char *)szArgList;
	HxLOG_Trace();

	/* 1st argument */
	aucArg_1 = HLIB_CMD_GetNextArg((HCHAR**)&szCmd);
	if( aucArg_1 != NULL )
	{
		if( sscanf(aucArg_1, "%s", (char *)&aucStr1) == 0 )
			return HxCMD_ERR;
		arg1 = atoi(aucStr1);
	}else
		return HxCMD_ERR;


	switch(arg1)
	{
	case 0: hErr =Teletext_SetControl(eTTX_CONTROL_LANGUAGE,eTTX_LANG_LATIN_WEST_EU); break;
	case 1: hErr =Teletext_SetControl(eTTX_CONTROL_LANGUAGE,eTTX_LANG_LATIN_EAST_EU); break;
	case 2: hErr =Teletext_SetControl(eTTX_CONTROL_LANGUAGE,eTTX_LANG_HEBREW_ARABIC); break;
	case 3: hErr =Teletext_SetControl(eTTX_CONTROL_LANGUAGE,eTTX_LANG_CYRILLIC); break;
	case 4: hErr =Teletext_SetControl(eTTX_CONTROL_LANGUAGE,eTTX_LANG_LAST); break;
	}

	if(hErr== ERR_FAIL)
		HLIB_CMD_Printf("Teletext_CMD_Navi Error!\n");
	return HxCMD_OK;
}



int local_ttx_CMD_SetFont(void *szArgList)
{

	HUINT8			*aucArg_1=NULL;
	HUINT8		   aucStr1[20];
	unsigned long   arg1 = 0;
	char 		   *szCmd = (char *)szArgList;
	TTX_Context_t	*pCt = NULL;
	pCt = local_ttx_GetContext();
	HxLOG_Trace();

	/* 1st argument */
	aucArg_1 = HLIB_CMD_GetNextArg((HCHAR**)&szCmd);
	if( aucArg_1 != NULL )
	{
		if( sscanf(aucArg_1, "%s", (char *)&aucStr1) == 0 )
			return HxCMD_ERR;
		arg1 = atoi(aucStr1);
	}else
		return HxCMD_ERR;


	if ( pCt->status != eTTX_Status_Running )
	{
		HLIB_CMD_Printf("Teletext not Running\n");
		return HxCMD_ERR;
	}

	pCt->bVariableFont = arg1;
	if (pCt->bVariableFont == TRUE)
	{
		TTX_BITSET(pCt->controller.state, eTTX_STATE_VAR_FONT);
	}
	else
	{
		TTX_BITCLR(pCt->controller.state, eTTX_STATE_VAR_FONT);
	}

	HLIB_CMD_Printf("Teletext_CMD_SetFont(bVariableFont:%d)\n", arg1);

	return HxCMD_OK;
}



int local_ttx_CMD_Dump(void *szArgList)
{
	TTX_Context_t		*pCt		= NULL;
	pCt = local_ttx_GetContext();

	if(local_ttx_dump(pCt->usPageNum,pCt->usSubcode)== ERR_FAIL)
		HLIB_CMD_Printf("Cannot get teletext page: [%d-%x]\n",pCt->usPageNum,pCt->usSubcode );
	return HxCMD_OK;
}



void Teletext_CMD_Register(HUINT8 *cmdname)
{
#define	hCmdHandle		"TTX_CMD"
	HLIB_CMD_RegisterWord(hCmdHandle,local_ttx_CMD_Show,
			"show",																/* keyword */
			"[0)fixed,1)Variable],[0)teletext,1)EBUsubtite]",										/*   help  */
			"show [0 ~ 1] [0 ~1]");											/*  usage  */
	HLIB_CMD_RegisterWord(hCmdHandle,local_ttx_CMD_Hide,
			"hide",																/* keyword */
			"Teletext_hide page",												/*   help  */
			"hide");															/*  usage  */
	HLIB_CMD_RegisterWord(hCmdHandle,local_ttx_CMD_Dump,
			"dump",																/* keyword */
			"dump current page",												/*   help  */
			"dump");															/*  usage  */
   HLIB_CMD_RegisterWord(hCmdHandle,local_ttx_CMD_ColorKey,
			"color",															/* keyword */
			"0)Red,1)Green,2)Yellow,3)Blue",									/*	 help  */
			"color [0 ~ 3]");													/*	usage  */

   HLIB_CMD_RegisterWord(hCmdHandle,local_ttx_CMD_ArrowKey,
			"arrow",					  		 								/* keyword */
			"0)Left, 1)Right,2)Up, 3)Down",	  								/*   help  */
			"arrow [0 ~ 3]"); 				  	 								/*  usage  */
   HLIB_CMD_RegisterWord(hCmdHandle,local_ttx_CMD_Input,
			"input",						  									/* keyword */
			"input 0 ~ 9",	 													/*   help  */
			"input [0 ~ 9]"); 				  									/*  usage  */
   /////////////////////////////////////////////////
   //ETC funtions...
	HLIB_CMD_RegisterWord(hCmdHandle,local_ttx_CMD_Cancel,
			 "cancel",						   									/* keyword */
			 "cancel", 				  											/*   help  */
			 "cancel");				   											/*  usage  */
   HLIB_CMD_RegisterWord(hCmdHandle,local_ttx_CMD_Mix,
			"mix",						  										/* keyword */
			"mix percent [0 ~ 100]",				 							/*   help	*/
			"mix [0/10/20/30/.../10]");				  							/*  usage  */

	HLIB_CMD_RegisterWord(hCmdHandle,local_ttx_CMD_Index,
			"index",															/* keyword */
			"Go_index",															/*	 help  */
			"index");															/*	usage  */

	HLIB_CMD_RegisterWord(hCmdHandle,local_ttx_CMD_Reveal,
			"reveal",															/* keyword */
			"reveal",															/*	 help  */
			"reveal");															/*	usage  */

	HLIB_CMD_RegisterWord(hCmdHandle,local_ttx_CMD_Subpage,
			"subpage",															/* keyword */
			"control subpage",													/*	 help  */
			"subpage");															/*	usage  */
	HLIB_CMD_RegisterWord(hCmdHandle,local_ttx_CMD_Double,
				"double",														/* keyword */
				"0)off,1)on/top,2)btm,3)last",									/*	 help  */
				"double [0 ~3]"); 												/*	usage  */
	HLIB_CMD_RegisterWord(hCmdHandle,local_ttx_CMD_Hold,
				"hold",															/* keyword */
				"page hold",													/*	 help  */
				"hold"); 														/*	usage  */

	HLIB_CMD_RegisterWord(hCmdHandle,local_ttx_CMD_Navi,
				"navi",															/* keyword */
				"0)default/fast/FLOF,1)TOP,2)trace,3)last",						/*	 help  */
				"navi [0 ~3]"); 												/*	usage  */

	HLIB_CMD_RegisterWord(hCmdHandle,local_ttx_CMD_Language,
				"lang",															/* keyword */
				"0)west_eu,1)east_eu,2)arabic,3)cyrillic,4)last",				/*	 help  */
				"lang [0 ~4]"); 												/*	usage  */

	HLIB_CMD_RegisterWord(hCmdHandle,local_ttx_CMD_SetFont,
					"font", 													/* keyword */
					"0)fixed,1)variable",										/*	 help  */
					"font [0 ~1]"); 											/*	usage  */
}
#endif

