/** **********************************************************************************************************
	@file 		svc_cso_sub.c
	@date		2013/06/19

	ref. file is pal_media_sub.c
	change text log :
		PAL_MEDIA_SUB -> SVC_CSO_SUB
		PAL_MEDIA_EVT_MEDIA_SUB_UPDATE -> CSO_MEDIA_EVT_MEDIA_SUB_UPDATE
		PAL_MEDIA_HANDLE -> SvcCso_t *
		phMedia -> player
*********************************************************************************************************** */

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
/** @brief global header file  */
#include <octo_common.h>
#include <linkedlist.h>
#include <util.h>

/** @brief local header file  */
#include "_svc_cso_sub.h"
/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#define _CONFIG_DEBUG 0

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

#define SVC_CSO_SUB_MAX_CAPABILITY				4

#define SVC_CSO_SUB_SAMI_DEFAULT_DUR_TIME			(60*60*5)
#define SVC_CSO_SUB_SYNC_TIME						300
#define SVC_CSO_SUB_LINE_BUF_SIZE					512
#define SVC_CSO_SUB_LINE_SPACE					5

#define SVC_CSO_SUB_DEFAULT_FONT_COLOR			0xffffffff		/* white. */
#define SVC_CSO_SUB_DEFAULT_FONT_BGCOLOR			0xff000000		/* white. */
#define SVC_CSO_SUB_DEFAULT_FONT_SIZE				20

#define SVC_CSO_SUB_CRITICAL_SECTION_IN			VK_SEM_Get(s_ulSvcCsoSubSemaphore)
#define SVC_CSO_SUB_CRITICAL_SECTION_OUT			VK_SEM_Release(s_ulSvcCsoSubSemaphore)

#define MACRO_CHECK_TIME_BOUNDARY(x, a, b)	(( x < a || b < x ) ? FALSE : TRUE)
#define ISSPACE(c) ((0x09 <= (c) && (c) <= 0x0d) || (c) == ' ')

#define  DISPLAY_LANG_Hx 0

typedef enum
{
	eSVC_CSO_SUB_SAMI_COL_WHITE = 0,
	eSVC_CSO_SUB_SAMI_COL_ORANGE,
	eSVC_CSO_SUB_SAMI_COL_YELLOW,
	eSVC_CSO_SUB_SAMI_COL_GREEN,
	eSVC_CSO_SUB_SAMI_COL_BLUE,
	eSVC_CSO_SUB_SAMI_COL_BLACK,
	eSVC_CSO_SUB_SAMI_COL_RED,

	eSVC_CSO_SUB_SAMI_COL_MAX
} SvcCsoSub_Color_e;

typedef enum
{
	eSVC_CSO_SUB_STR_STEP_NONE = 0,
	eSVC_CSO_SUB_STR_STEP_NUMBER = 0,
	eSVC_CSO_SUB_STR_STEP_TIME,
	eSVC_CSO_SUB_STR_STEP_STRING
} SvcCsoSub_SrtStep_e;

typedef struct tagSVC_CSO_SUB_SAMI_COLOR
{
	HUINT32		argb;
	HUINT8		*pName;
} SvcCsoSub_SamiColor_t;

enum
{
	CMD_SVC_CSO_SUB_300_MS = 100,							/**< Timer CMD. */
};

/** @brief Media Subtitle Msg Structure. */
typedef struct tagSVC_CSO_SUB_MSG
{
	HUINT32		ulCmd;								/**< Command. */
} SvcCsoSub_Msg_t;

typedef struct tagSVC_CSO_SUB_SamiClass
{
	HINT8           szClassName[SVC_CSO_SUB_LANGUAGE_STRLEN_MAX];
	HINT8           szName[SVC_CSO_SUB_LANGUAGE_STRLEN_MAX];
} SvcCsoSub_SamiClass_t;

/** @brief Context .. */
typedef struct tagSVC_CSO_SUB_CONTEXT
{
	SvcCso_t * 			player;
	SvcCsoSub_Status_e        status;                                     /**< Status. */
	SvcCsoSub_Type_e        eFileType;                                  /**< Current File Type. */
//	eHxCP_CodePage_e             eEncoding;                                  /**< Encoding Type. */
	HUINT32                     ulTotalLang;                                /**< ���ð����� Language ����. */
	HUINT32                     ulLangIdx;                                  /**< ����ڰ� ������ Language index. */
	HUINT32                     ulTextPosition;
	HUINT32                     ulFontSize;

#if DISPLAY_LANG_Hx
	HxLANG_Id_e				eDispLang[SVC_CSO_SUB_LANGUAGE_MAX];      /* ��ü Language ����. */
#else
	HINT8						szDispLang[SVC_CSO_SUB_LANGUAGE_MAX][SVC_CSO_SUB_LANGUAGE_STRLEN_MAX];
#endif

	SvcCsoSub_SamiClass_t   stSamiClass[SVC_CSO_SUB_LANGUAGE_MAX];
	SvcCsoSub_Style_t       stStyle;                                    /* Default style. */

	HBOOL						bIsDisplay;
	HINT32						pNowPos;
	HINT32                      msOffsetTime;                               /* Offset Time. */
	HUINT32                     ulFirstTime[SVC_CSO_SUB_LANGUAGE_MAX];    /* Fisrt packet time. */
	HUINT32                     ulLastTime[SVC_CSO_SUB_LANGUAGE_MAX];     /* Last packet time. */

	HUINT32                     ulTotalPacket[SVC_CSO_SUB_LANGUAGE_MAX];  /* Total Packet. */
#if defined(_CONFIG_DEBUG)
	POINTER_LIST_T              *pHeadPacket[SVC_CSO_SUB_LANGUAGE_MAX];   /* packet�� Head Pointer. */
#endif
	POINTER_LIST_T              *pCurrPacket[SVC_CSO_SUB_LANGUAGE_MAX];   /* ���� Display �ǰ� �ִ� packet�� Pointer. */
} SvcCsoSub_Context_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static struct _sub_pool
{
	HUINT32						ulUsing;
	HUINT32						ulPlaying;

	SvcCso_t *			s_stSvcCsoSubContext[SVC_CSO_SUB_MAX_CAPABILITY];
} s_pstSvcCsoSubContextPool;

static HUINT8			s_ucLineBuffer[SVC_CSO_SUB_LINE_BUF_SIZE];

static HULONG			s_ulSvcCsoSubMsgQId;			/**< Message Queue ID */
static HULONG			s_ulSvcCsoSubTaskId;			/**< Task ID */
static HULONG			s_ulSvcCsoSubTimerId;			/**< Timer ID */
static HULONG			s_ulSvcCsoSubSemaphore;			/**< Data ��ȣ��. */

/** Static Sub Event Callnack for mw */
static SvcCsoSub_EventCallback_t	s_tSubEventCallback;

static SvcCsoSub_SamiColor_t s_stSVC_CSO_SUBSamiColorTbl[] =
{
	{	0xffffffff, 	"white" 	},
	{	0xffff6600, 	"orange"	},
	{	0xffffff00, 	"yellow"	},
	{	0xff009900, 	"green"		},
	{	0xff0033ff, 	"blue"		},
	{	0xff000000, 	"black"		},
	{	0xffff0000, 	"red"		},
};
/*******************************************************************/
/********************    Local Functions   *************************/
/*******************************************************************/
#define __________STATIC_FUNCTIONS___________
#if defined(_CONFIG_DEBUG)
static HUINT8 * local_sub_GetSVC_CSO_SUBStatusText(SvcCsoSub_Status_e status)
{
	switch (status)
	{
		ENUM_TO_STR(eSVC_CSO_SUB_STATUS_SETUP);
		ENUM_TO_STR(eSVC_CSO_SUB_STATUS_STOP);
		ENUM_TO_STR(eSVC_CSO_SUB_STATUS_PLAY);
		ENUM_TO_STR(eSVC_CSO_SUB_STATUS_RELEASE);

		default :
			break;
	}

	return NULL;
}
#endif

static void FREE_SVC_CSO_SUB_Packet(SvcCsoSub_Packet_t *pPacket)
{
	HUINT32		i;

	if ( pPacket == NULL )
	{
		return;
	}

	for ( i = 0; i < pPacket->ulStrNum; i++ )
	{
		if ( pPacket->pText[i] != NULL )
		{
			HLIB_STD_MemFree(pPacket->pText[i]);
		}
	}

	HLIB_STD_MemFree(pPacket);
}

static inline HLONG local_sub_GetMessageQId(void)
{
	return s_ulSvcCsoSubMsgQId;
}

static inline HUINT8 * local_sub_GetLineBuffer(void)
{
	return s_ucLineBuffer;
}

static void local_sub_InitSVC_CSO_SUBContext(SvcCsoSub_Context_t *pContext)
{
	HUINT32 i;

	for ( i = 0; i < pContext->ulTotalLang; i++ )
	{
		UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(pContext->pCurrPacket[i], (FREE_FUNC)FREE_SVC_CSO_SUB_Packet);
	}

	memset(pContext, 0x00, sizeof(SvcCsoSub_Context_t));
}

#if 0
static SvcCsoSub_Context_t * local_sub_GetContextbyMediahandle(SvcCso_t * pMediaHandle)
{
	int i;
	SvcCso_t *p;

	for(i=0; i<SVC_CSO_SUB_MAX_CAPABILITY; i++)
	{
		if(s_pstSVC_CSO_SUBContextPool.s_stSVC_CSO_SUBContext[i] == pMediaHandle)
		{
			p =s_pstSVC_CSO_SUBContextPool.s_stSVC_CSO_SUBContext[i];
			return p->pSubContext;
		}
	}
	return NULL;
}
#endif

static inline void local_sub_SetStatus(SvcCsoSub_Context_t *pContext, SvcCsoSub_Status_e status)
{
	pContext->status = status;
}

static inline SvcCsoSub_Status_e local_sub_GetStatus(SvcCsoSub_Context_t *pContext)
{
	return pContext->status;
}

static HERROR local_sub_FindMediaSubFile(HUINT8 *p, HxFILE_t *phFile)
{
	HUINT32		len;
	HUINT8		*pBuf = local_sub_GetLineBuffer();
	int			i;
	HUINT8 		*pszSubExt[] = { "smi", "sami", "smil", "srt", "sub", "txt", "ass", "ssa", "psb", };

	len = strlen(p);
	if ( len > SVC_CSO_SUB_LINE_BUF_SIZE - 5 )
	{
		return ERR_FAIL;
	}

	strncpy(pBuf, p, SVC_CSO_SUB_LINE_BUF_SIZE-1);
	pBuf[SVC_CSO_SUB_LINE_BUF_SIZE-1] = '\0';

	for (i = 0; i < sizeof(pszSubExt) / sizeof(pszSubExt[0]); i++) {
		snprintf(pBuf + len, SVC_CSO_SUB_LINE_BUF_SIZE - len, ".%s", pszSubExt[i]);

		if (HLIB_FILE_Exist(pBuf) == FALSE)
		{
			//HxLOG_Error("[%s:%d] ext .%s not existed[%s] \n", __FILE__, __LINE__, pszSubExt[i], pBuf);
			continue;
		}

		HxLOG_Print("[%s:%d] ext .%s existed \n", __FILE__, __LINE__, pszSubExt[i]);

		*phFile = HLIB_FILE_Open(pBuf, "r");
		if ( *phFile != NULL )
		{
		   	return ERR_OK;
		}
	}

	return ERR_FAIL;
}

static void local_sub_InsertSubPacket(SvcCsoSub_Context_t *pContext, int idx, SvcCsoSub_Packet_t *pSubPkt)
{
	HUINT32		i = 0;
	POINTER_LIST_T	*pFound = NULL;
	SvcCsoSub_Packet_t	*pCurPkt;

	if ( pSubPkt == NULL )
	{
		return;
	}

	if ( pContext->ulTotalPacket[idx] != 0 )
	{
		pFound = UTIL_LINKEDLIST_FindLastListItemFromPointerList(pContext->pCurrPacket[idx]);
	}

	/* �ڿ������� ã�� Ȯ�� �Ѵ�. ���� ���߿� ���� ���� ������ display �� packet�̴�. */
	do
	{
		if ( pFound == NULL )
		{
			pContext->pCurrPacket[idx] = UTIL_LINKEDLIST_InsertListItemToPointerList(NULL, pSubPkt, NULL);
			break;
		}

		pCurPkt = UTIL_LINKEDLIST_GetContents(pFound);
		if ( pCurPkt->startTime <= pSubPkt->startTime )
		{
			pContext->pCurrPacket[idx] = UTIL_LINKEDLIST_InsertListItemToPointerListNext(pContext->pCurrPacket[idx], pFound, pSubPkt);
			break;
		}

		pFound = UTIL_LINKEDLIST_GetPrevPointer(pFound);
	}
	while ( ++i < pContext->ulTotalPacket[idx] );

	pContext->ulTotalPacket[idx]++;
	UTIL_LINKEDLIST_CircularizePointerList(pContext->pCurrPacket[idx]);
}

static char *local_sub_Stristr(const char *haystack, const char *needle)
{
    int len = 0;
    const char *p = haystack;

    if ( !(haystack && needle) )
    {
    	return NULL;
    }

    len = strlen(needle);
    while (*p != '\0')
    {
		if (strncasecmp(p, needle, len) == 0) { return (char*)p; }
		p++;
    }

    return NULL;
}


static HUINT32 local_sub_ReadTextLine(HUINT8 *pText, HUINT8 *pBuf, HUINT32 *ulNlSize)
{
	HUINT32		cnt = 0;

	*ulNlSize = 0;

	if ( pText == NULL || pBuf == NULL )
	{
		return 0;
	}

	for ( cnt = 0; cnt < SVC_CSO_SUB_LINE_BUF_SIZE; cnt++ )
	{
		if ( (*(pText + cnt) == 0x0d && *(pText + cnt + 1) == 0x0a) )	{ cnt += 2; *ulNlSize = 2; break; }
		if ( *(pText + cnt) == 0x0a ) { cnt++; *ulNlSize = 1; break; }
	}

	memset(pBuf, 0x00, SVC_CSO_SUB_LINE_BUF_SIZE);
	memcpy(pBuf, pText, cnt);

	return cnt;
}


static void local_sub_SetTimeBoundary(SvcCsoSub_Context_t *pContext, HUINT32 langIdx)
{
	POINTER_LIST_T	*pFirst, *pLast;
	SvcCsoSub_Packet_t	*pFirstPacket = NULL, *pLastPacket = NULL;

	if ( pContext->ulTotalPacket[langIdx] == 0 )
	{
		return;
	}

	pFirst = pContext->pCurrPacket[langIdx];
	pLast = UTIL_LINKEDLIST_GetPrevPointer(pContext->pCurrPacket[langIdx]);

	pFirstPacket = UTIL_LINKEDLIST_GetContents(pFirst);
	pLastPacket = UTIL_LINKEDLIST_GetContents(pLast);

	pContext->ulFirstTime[langIdx] = pFirstPacket->startTime;
	pContext->ulLastTime[langIdx] = pLastPacket->endTime;
}


static void local_sub_SetEndTime(SvcCsoSub_Context_t *pContext, HUINT32 langIdx)
{
	HUINT32		i;
	POINTER_LIST_T	*pList, *pNextList;
	SvcCsoSub_Packet_t	*pPkt, *pNextPkt;

	if ( pContext->ulTotalPacket[langIdx] == 0 )
	{
		return;
	}

	pList = pContext->pCurrPacket[langIdx];

	/* Language ���� ������ �ʿ���. */
	for ( i = 0; i < pContext->ulTotalPacket[langIdx] - 1; i++ )
	{
		pPkt = UTIL_LINKEDLIST_GetContents(pList);
		if ( pPkt == NULL )
		{
			break;
		}

		pNextList = UTIL_LINKEDLIST_GetNextPointer(pList);
		if ( pNextList == NULL )
		{
			break;
		}

		pNextPkt = UTIL_LINKEDLIST_GetContents(pNextList);
		pPkt->endTime = pNextPkt->startTime;

		pList = UTIL_LINKEDLIST_GetNextPointer(pList);
	}

	/* ������ packet time ����. */
	pPkt = UTIL_LINKEDLIST_GetContents(pList);
	if ( pPkt == NULL )
	{
		HxLOG_Error("Contents List is NULL \n");
		return;
	}

	pPkt->endTime = pPkt->startTime + SVC_CSO_SUB_SAMI_DEFAULT_DUR_TIME;
}


static void local_sub_InsertText(HUINT8 *pBuf, HUINT32 size, HUINT32 nlsize, SvcCsoSub_Packet_t *pSubPkt)
{
	HUINT32		realSize;

	if ( pBuf == NULL || pSubPkt == NULL || pSubPkt->ulStrNum >= SVC_CSO_SUB_LINE_MAX)
	{
		return;
	}

	realSize =  size - nlsize;
	pSubPkt->pText[pSubPkt->ulStrNum] = (HUINT8 *)HLIB_STD_MemAlloc(realSize + 1);
	memcpy(pSubPkt->pText[pSubPkt->ulStrNum], pBuf, realSize);
	pSubPkt->pText[pSubPkt->ulStrNum][realSize] = '\0';
	pSubPkt->ulStrNum++;
}


static void local_sub_AdaptDefaultStyle(SvcCsoSub_Style_t *pStyle)
{
	pStyle->ulFtSize = SVC_CSO_SUB_DEFAULT_FONT_SIZE;
	pStyle->ftColor = SVC_CSO_SUB_DEFAULT_FONT_COLOR;
}


static int local_sub_timestr2msec(HUINT8 *p, HUINT32 *pmsec)
{
	int hour, min, sec, msec;
	HUINT8 *org_p = p;

	hour = min = sec = msec = 0;

	/* get hour */
	for (; '0' <= *p && *p <= '9'; p++)
		hour = hour * 10 + *p - '0';

	if (*p != ':')
		goto on_finish;

	/* get minute */
	for (p++; '0' <= *p && *p <= '9'; p++)
		min = min * 10 + *p - '0';

	if (*p != ':')
		goto on_finish;

	/* get second */
	for (p++; '0' <= *p && *p <= '9'; p++)
		sec = sec * 10 + *p - '0';

	if (*p == '.') {
		int point_cnt;
		/* get mili-second */
		for (p++, point_cnt = 0; '0' <= *p && *p <= '9' && point_cnt < 3; p++, point_cnt++)
			msec = msec * 10 + *p - '0';
		for (; point_cnt < 3; point_cnt++)
			msec *= 10;
		while ('0' <= *p && *p <= '9')
			p++;
	}

on_finish:
	*pmsec = hour * 60 * 60 * 1000 + min * 60 * 1000 + sec * 1000 + msec;

	return p - org_p;
}


#define ___________SRT_FUNCTIONS___________

static SvcCsoSub_Type_e local_sub_TestSrt(HUINT8 *p, HUINT32 ulSize)
{
	HUINT32		i, to, size, nlsize;
	HUINT8		*pBuf = local_sub_GetLineBuffer();

	if (p == NULL)
		return eSVC_CSO_SUB_TYPE_NOTHING;

	size = local_sub_ReadTextLine(p, pBuf, &nlsize);

	if (size - nlsize >= SVC_CSO_SUB_LINE_BUF_SIZE)
	{
		return eSVC_CSO_SUB_TYPE_NOTHING;
	}

	pBuf[size - nlsize] = '\0';
	to = strlen(pBuf);
	for (i = 0; i < to; i++) {
		if (pBuf[i] < '0' || pBuf[i] > '9')
			return eSVC_CSO_SUB_TYPE_NOTHING;
	}
	p += size;

	size = local_sub_ReadTextLine(p, pBuf, &nlsize);
	pBuf[size - nlsize] = '\0';
	if (strlen(pBuf) < 29)
		return eSVC_CSO_SUB_TYPE_NOTHING;
	to = strlen(pBuf);
	for (i = 0; i < to; i++) {
		if ('0' <= pBuf[i] && pBuf[i] <= '9')
			continue;
		if (pBuf[i] != ' ' && pBuf[i] != '-' && pBuf[i] != ':' && pBuf[i] != ',' && pBuf[i] != '>')
			return eSVC_CSO_SUB_TYPE_NOTHING;
	}

	return eSVC_CSO_SUB_TYPE_SRT;
}

static HERROR local_sub_ParseSrt(HUINT8 *pData, HUINT32 ulSize, SvcCsoSub_Context_t *pContext)
{
	HUINT32		cnt = 0, size, nlsize;
	HUINT32		startTime, endTime;
	HUINT8		*pBuf = local_sub_GetLineBuffer();
	HUINT8		*p = pData;
	SvcCsoSub_SrtStep_e	step = eSVC_CSO_SUB_STR_STEP_NONE;
	SvcCsoSub_Packet_t *pSubPkt = NULL;
	HUINT32		langIdx;

	HxLOG_Debug("============================== SRT Parsing ================================== \n");

	/* Set Default Style. */
	local_sub_AdaptDefaultStyle(&pContext->stStyle);

	/* default�� 1��. */
	pContext->ulTotalLang++;

	do
	{
		size = local_sub_ReadTextLine(p, pBuf, &nlsize);

		if ( size - nlsize == 0 )
		{
			p += size;
			cnt += size;
			step = eSVC_CSO_SUB_STR_STEP_NONE;

			local_sub_InsertSubPacket(pContext, 0, pSubPkt);
			pSubPkt = NULL;

			continue;
		}

		switch (step)
		{
			case eSVC_CSO_SUB_STR_STEP_NUMBER :
				if ( pSubPkt != NULL )
				{
					HLIB_STD_MemFree(pSubPkt);
				}

				/* Make sub packet. */
				pSubPkt = (SvcCsoSub_Packet_t *) HLIB_STD_MemAlloc(sizeof(SvcCsoSub_Packet_t));
				if ( pSubPkt == NULL )
				{
					HxLOG_Error("Memory Alloc Fail \n");
					break;
				}

				memset(pSubPkt, 0x00, sizeof(SvcCsoSub_Packet_t));
				step++;

				break;

			case eSVC_CSO_SUB_STR_STEP_TIME :
				startTime = atoi(pBuf) * (60*60*1000);
				startTime += atoi(pBuf + 3) * (60*1000);
				startTime += atoi(pBuf + 6) * (1000);
				startTime += atoi(pBuf + 9);

				endTime = atoi(pBuf + 17) * (60*60*1000);
				endTime += atoi(pBuf + 20) * (60*1000);
				endTime += atoi(pBuf + 23) * (1000);
				endTime += atoi(pBuf + 26);

				pSubPkt->startTime = startTime;
				pSubPkt->endTime = endTime;

				step++;
				break;

			case eSVC_CSO_SUB_STR_STEP_STRING :
				/* Set Default value. */
				pSubPkt->fontColor[pSubPkt->ulStrNum] = pContext->stStyle.ftColor;
				pSubPkt->bNewLine[pSubPkt->ulStrNum] = TRUE;

				local_sub_InsertText(pBuf, size, nlsize, pSubPkt);
				break;

			default :
				HxLOG_Error(".SRT Parsing Error \n");
				break;
		}

		p += size;
		cnt += size;
	} while( *p && cnt < ulSize );

	if ( pSubPkt != NULL )
	{
		local_sub_InsertSubPacket(pContext, 0, pSubPkt);
	}

	for ( langIdx = 0; langIdx < pContext->ulTotalLang; langIdx++ )
	{
		/* Set Time Boundary. */
		local_sub_SetTimeBoundary(pContext, langIdx);

#if defined (_CONFIG_DEBUG)
		pContext->pHeadPacket[langIdx] = pContext->pCurrPacket[langIdx];
#endif
	}

	return ERR_OK;
}

#define ___________SUBVIEWER_2_FUNCTIONS___________

static HERROR local_sub_GetSubviewerTiming_2(HUINT8 *p, SvcCsoSub_Packet_t *packet)
{
	int consumed;

	packet->startTime = 0;
	packet->endTime = 0;

	consumed = local_sub_timestr2msec(p, &packet->startTime);
	p += consumed;
	if (consumed < 5 || *p++ != ',')
		return ERR_FAIL;

	consumed = local_sub_timestr2msec(p, &packet->endTime);
	if (consumed < 5)
		return ERR_FAIL;

	return ERR_OK;
}

static HERROR local_sub_GetSubviewerText_2(HUINT8 *p, SvcCsoSub_Packet_t *packet)
{
	HUINT8 *line;
	int len;

	len = 0;
	line = p;
	for (; *p; p++) {
		if ((p[0] == '[') && ((p[1] | 0x20) == 'b') && ((p[2] | 0x20) == 'r') && (p[3] == ']')) {
			if (len == 0) {
				line[0] = ' ';
				len = 1;
			}
			packet->pText[packet->ulStrNum] = (HUINT8 *)HLIB_STD_MemAlloc(len + 1);
			if (packet->pText[packet->ulStrNum] == NULL) {
				while (packet->ulStrNum) {
					packet->ulStrNum--;
					HLIB_STD_MemFree(packet->pText[packet->ulStrNum]);
					packet->pText[packet->ulStrNum] = NULL;
				}
				return ERR_FAIL;
			}
			memcpy(packet->pText[packet->ulStrNum], line, len);
			packet->pText[packet->ulStrNum][len] = '\0';
			packet->bNewLine[packet->ulStrNum] = TRUE;
			packet->ulStrNum++;
			if (packet->ulStrNum >= SVC_CSO_SUB_LINE_MAX)
				return ERR_OK;
			p += 3;
			len = 0;
			line = p + 1;
			continue;
		} else if (p[0] == 0x0d || p[0] == 0x0a) {
			break;
		}
		len++;
	}

	if (len) {
		packet->pText[packet->ulStrNum] = (HUINT8 *)HLIB_STD_MemAlloc(len + 1);
		if (packet->pText[packet->ulStrNum] == NULL) {
			while (packet->ulStrNum) {
				packet->ulStrNum--;
				HLIB_STD_MemFree(packet->pText[packet->ulStrNum]);
				packet->pText[packet->ulStrNum] = NULL;
			}
			return ERR_FAIL;
		}
		memcpy(packet->pText[packet->ulStrNum], line, len);
		packet->pText[packet->ulStrNum][len] = '\0';
		packet->bNewLine[packet->ulStrNum] = TRUE;
		packet->ulStrNum++;
	}

	return ERR_OK;
}

static SvcCsoSub_Type_e local_sub_TestSubviewer_2(HUINT8 *p, HUINT32 ulsize)
{
	HUINT32		i, size, nlsize;
	HUINT8		*line = local_sub_GetLineBuffer();
	SvcCsoSub_Packet_t	packet;

	for (i = 0; i < ulsize; i += size, p += size) {
		size = local_sub_ReadTextLine(p, line, &nlsize);
		if (line[0] == '[')
			continue;
		if (size != nlsize) {
			if (local_sub_GetSubviewerTiming_2(line, &packet) == ERR_OK)
				return eSVC_CSO_SUB_TYPE_SUBVIEWER_2;
			else
				return eSVC_CSO_SUB_TYPE_NOTHING;
		}
	}
	return eSVC_CSO_SUB_TYPE_NOTHING;
}

static HERROR local_sub_ParseSubviewer_2(HUINT8 *p, HUINT32 ulsize, SvcCsoSub_Context_t *pContext)
{
	HUINT32		i, size, nlsize;
	HUINT8		*line = local_sub_GetLineBuffer();
	enum {
		PHASE_NONE,
		PHASE_TIME,
		PHASE_TEXT,
	} phase = PHASE_NONE;
	SvcCsoSub_Packet_t	*packet = NULL;
	HUINT32		langIdx;

	HxLOG_Debug("=================== %s[%d] ===================== \n", __FUNCTION__, __LINE__ );

	/* Set Default Style. */
	local_sub_AdaptDefaultStyle(&pContext->stStyle);

	/* default�� 1��. */
	pContext->ulTotalLang++;

	packet = (SvcCsoSub_Packet_t *) HLIB_STD_MemAlloc(sizeof(SvcCsoSub_Packet_t));
	if (packet == NULL) {
		HxLOG_Error("Memory Alloc Fail \n");
		return ERR_FAIL;
	}
	memset(packet, 0, sizeof(SvcCsoSub_Packet_t));

	for (i = 0; i < ulsize; i += size, p += size) {
		size = local_sub_ReadTextLine(p, line, &nlsize);
		if (phase == PHASE_NONE && line[0] == '[')
			continue;

		if (size - nlsize == 0)	/* empty line */
		{
			if (phase == PHASE_TEXT) {
				/* add packet */
				local_sub_InsertSubPacket(pContext, 0, packet);
				packet = (SvcCsoSub_Packet_t *) HLIB_STD_MemAlloc(sizeof(SvcCsoSub_Packet_t));
				if (packet == NULL) {
					HxLOG_Error("Memory Alloc Fail \n");
					break;
				}
				memset(packet, 0, sizeof(SvcCsoSub_Packet_t));
			}
			phase = PHASE_NONE;
			continue;
		}
		if (size - nlsize < SVC_CSO_SUB_LINE_BUF_SIZE)
		{
			line[size - nlsize] = '\0';
		}

		if (phase == PHASE_NONE) {
			/* try to get timing information */
			if (local_sub_GetSubviewerTiming_2(line, packet) == ERR_OK)
				phase = PHASE_TIME;
		} else if (phase == PHASE_TIME) {
			/* try to get text information */
			if (local_sub_GetSubviewerText_2(line, packet) == ERR_OK)
				phase = PHASE_TEXT;
			else
				phase = PHASE_NONE;
		}
	}

	if (packet)
	{
		HLIB_STD_MemFree(packet);
	}

	for ( langIdx = 0; langIdx < pContext->ulTotalLang; langIdx++ )
	{
		/* Set Time Boundary. */
		local_sub_SetTimeBoundary(pContext, langIdx);

#if defined (_CONFIG_DEBUG)
		pContext->pHeadPacket[langIdx] = pContext->pCurrPacket[langIdx];
#endif
	}

	return ERR_OK;
}

#define ___________POWERDIVX_FUNCTIONS___________
static HERROR local_sub_GetPowerdivxText(HUINT8 *p, SvcCsoSub_Packet_t *packet)
{
	HUINT8 *line;
	int len;

	len = 0;
	line = p;
	for (; *p; p++) {
		if (p[0] == '|') {
			if (len == 0) {
				line[0] = ' ';
				len = 1;
			}

			packet->pText[packet->ulStrNum] = (HUINT8 *)HLIB_STD_MemAlloc(len + 1);
			if (packet->pText[packet->ulStrNum] == NULL) {
				while (packet->ulStrNum) {
					packet->ulStrNum--;
					HLIB_STD_MemFree(packet->pText[packet->ulStrNum]);
					packet->pText[packet->ulStrNum] = NULL;
				}
				return ERR_FAIL;
			}
			memcpy(packet->pText[packet->ulStrNum], line, len);
			packet->pText[packet->ulStrNum][len] = '\0';
			packet->bNewLine[packet->ulStrNum] = TRUE;
			packet->ulStrNum++;
			if (packet->ulStrNum >= SVC_CSO_SUB_LINE_MAX)
				return ERR_OK;
			len = 0;
			line = p + 1;
			continue;
		} else if (p[0] == 0x0d || p[0] == 0x0a) {
			break;
		}
		len++;
	}

	if (packet->ulStrNum == 0 && len == 0) {
		/* add blank if there is no subtitle text */
		line[0] = ' ';
		len = 1;
	}

	if (len) {
		packet->pText[packet->ulStrNum] = (HUINT8 *)HLIB_STD_MemAlloc(len + 1);
		if (packet->pText[packet->ulStrNum] == NULL) {
			while (packet->ulStrNum) {
				packet->ulStrNum--;
				HLIB_STD_MemFree(packet->pText[packet->ulStrNum]);
				packet->pText[packet->ulStrNum] = NULL;
			}
			return ERR_FAIL;
		}
		memcpy(packet->pText[packet->ulStrNum], line, len);
		packet->pText[packet->ulStrNum][len] = '\0';
		packet->bNewLine[packet->ulStrNum] = TRUE;
		packet->ulStrNum++;
	}

	return ERR_OK;
}

static HERROR local_sub_GetPowerdivxTiming(HUINT8 *p, SvcCsoSub_Packet_t *packet, int *consumed)
{
	HUINT8 *org_p = p;
	int i, len;

	*consumed = 0;
	packet->startTime = 0;
	packet->endTime = 0;

	if (p[0] != '{')
		return ERR_FAIL;
	p++;
	len = local_sub_timestr2msec(p, &packet->startTime);
	for (i = 0; i < len; i++)
		if (p[i] == ':')
			break;
	if (i >= len)
		return ERR_FAIL;
	p += len;
	if (len == 0 || p[0] != '}' || p[1] != '{')
		return ERR_FAIL;
	p += 2;
	len = local_sub_timestr2msec(p, &packet->endTime);
	for (i = 0; i < len; i++)
		if (p[i] == ':')
			break;
	if (i >= len)
		return ERR_FAIL;
	p += len;
	if (len == 0 || p[0] != '}')
		return ERR_FAIL;
	p++;

	*consumed = p - org_p;
	return ERR_OK;
}

static SvcCsoSub_Type_e local_sub_TestPowerdivx(HUINT8 *p, HUINT32 ulsize)
{
	SvcCsoSub_Packet_t	packet;
	int consumed;

	if (local_sub_GetPowerdivxTiming(p, &packet, &consumed) != ERR_OK)
		return eSVC_CSO_SUB_TYPE_NOTHING;
	return eSVC_CSO_SUB_TYPE_POWERDIVX;
}

static HERROR local_sub_ParsePowerdivx(HUINT8 *p, HUINT32 ulsize, SvcCsoSub_Context_t *pContext)
{
	HUINT32		i, line_len, nlsize;
	HUINT8		*line = local_sub_GetLineBuffer();
	SvcCsoSub_Packet_t	*packet = NULL;
	int consumed;
	HUINT32		langIdx;

	HxLOG_Debug("=================== PSB %s[%d] ===================== \n", __FUNCTION__, __LINE__ );

	/* Set Default Style. */
	local_sub_AdaptDefaultStyle(&pContext->stStyle);

	/* default�� 1��. */
	pContext->ulTotalLang++;

	for (i = 0; i < ulsize; i += line_len, p += line_len) {
		line_len = local_sub_ReadTextLine(p, line, &nlsize);

		if (line_len - nlsize == 0)	/* empty line */
			continue;

		if (line_len - nlsize >= SVC_CSO_SUB_LINE_BUF_SIZE)		/*Buffer overFlow*/
			continue;

		line[line_len - nlsize] = '\0';

		packet = (SvcCsoSub_Packet_t *) HLIB_STD_MemAlloc(sizeof(SvcCsoSub_Packet_t));
		if (packet == NULL) {
			HxLOG_Error("Memory Alloc Fail \n");
			return ERR_FAIL;
		}
		memset(packet, 0, sizeof(SvcCsoSub_Packet_t));

		if (local_sub_GetPowerdivxTiming(line, packet, &consumed) != ERR_OK) {
			HLIB_STD_MemFree(packet);
			continue;
		}
		if (local_sub_GetPowerdivxText(line + consumed, packet) != ERR_OK) {
			HLIB_STD_MemFree(packet);
			continue;
		}

		local_sub_InsertSubPacket(pContext, 0, packet);
	}

	for ( langIdx = 0; langIdx < pContext->ulTotalLang; langIdx++ )
	{
		/* Set Time Boundary. */
		local_sub_SetTimeBoundary(pContext, langIdx);

#if defined (_CONFIG_DEBUG)
		pContext->pHeadPacket[langIdx] = pContext->pCurrPacket[langIdx];
#endif
	}

	return ERR_OK;
}


#define ___________SUBVIEWER_1_FUNCTIONS___________

static HERROR local_sub_GetSubviewerTiming_1(HUINT8 *p, HUINT32 *msec)
{
	if (p[0] != '[')
		return ERR_FAIL;
	if (local_sub_timestr2msec(p + 1, msec) != 8)
		return ERR_FAIL;
	if (p[9] != ']')
		return ERR_FAIL;
	return ERR_OK;
}

static SvcCsoSub_Type_e local_sub_TestSubviewer_1(HUINT8 *p, HUINT32 ulsize)
{
	HUINT32		i, line_len, nlsize;
	HUINT8		*line = local_sub_GetLineBuffer();
	int line_cnt;
	HUINT32 msec;

	for (i = 0, line_cnt = 0; i < ulsize && line_cnt < 100; i += line_len, p += line_len, line_cnt++) {
		line_len = local_sub_ReadTextLine(p, line, &nlsize);
		if (line_len - nlsize != 10)
			continue;
		if (p[0] != '[')
			continue;
		if (p[1] < '0' || p[1] > '9')
			continue;
		if (local_sub_GetSubviewerTiming_1(line, &msec) == ERR_FAIL)
			return eSVC_CSO_SUB_TYPE_NOTHING;
		else
			return eSVC_CSO_SUB_TYPE_SUBVIEWER_1;
	}
	return eSVC_CSO_SUB_TYPE_NOTHING;
}

static HERROR local_sub_ParseSubviewer_1(HUINT8 *p, HUINT32 ulsize, SvcCsoSub_Context_t *pContext)
{
	HUINT32		i, size, nlsize;
	HUINT8		*line = local_sub_GetLineBuffer();
	enum {
		PHASE_NONE,
		PHASE_STARTTIME,
		PHASE_TEXT,
		PHASE_ENDTIME,
	} phase = PHASE_NONE;
	SvcCsoSub_Packet_t	*packet = NULL;
	HUINT32		langIdx;

	HxLOG_Debug("=================== %s[%d] ===================== \n", __FUNCTION__, __LINE__ );

	/* Set Default Style. */
	local_sub_AdaptDefaultStyle(&pContext->stStyle);

	/* default�� 1��. */
	pContext->ulTotalLang++;

	packet = (SvcCsoSub_Packet_t *) HLIB_STD_MemAlloc(sizeof(SvcCsoSub_Packet_t));
	if (packet == NULL) {
		HxLOG_Error("Memory Alloc Fail \n");
		return ERR_FAIL;
	}
	memset(packet, 0, sizeof(SvcCsoSub_Packet_t));

	for (i = 0; i < ulsize; i += size, p += size) {
		size = local_sub_ReadTextLine(p, line, &nlsize);

		if (size - nlsize == 0)	/* empty line */
		{
			if (phase == PHASE_ENDTIME) {
				/* add packet */
				local_sub_InsertSubPacket(pContext, 0, packet);
				packet = (SvcCsoSub_Packet_t *) HLIB_STD_MemAlloc(sizeof(SvcCsoSub_Packet_t));
				if (packet == NULL) {
					HxLOG_Error("Memory Alloc Fail \n");
					break;
				}
				memset(packet, 0, sizeof(SvcCsoSub_Packet_t));
			}
			phase = PHASE_NONE;
			continue;
		}
		if (size - nlsize < SVC_CSO_SUB_LINE_BUF_SIZE)
		{
			line[size - nlsize] = '\0';
		}

		if (phase == PHASE_NONE) {
			/* try to get timing information */
			if (local_sub_GetSubviewerTiming_1(line, &packet->startTime) == ERR_OK)
				phase = PHASE_STARTTIME;
		} else if (phase == PHASE_STARTTIME) {
			/* try to get text information */
			if (local_sub_GetPowerdivxText(line, packet) == ERR_OK)
				phase = PHASE_TEXT;
			else
				phase = PHASE_NONE;
		} else if (phase == PHASE_TEXT) {
			/* try to get timing information */
			if (local_sub_GetSubviewerTiming_1(line, &packet->endTime) == ERR_OK)
				phase = PHASE_ENDTIME;
			else
				phase = PHASE_NONE;
		}
	}

	if (packet)
	{
		HLIB_STD_MemFree(packet);
	}

	for ( langIdx = 0; langIdx < pContext->ulTotalLang; langIdx++ )
	{
		/* Set Time Boundary. */
		local_sub_SetTimeBoundary(pContext, langIdx);

#if defined (_CONFIG_DEBUG)
		pContext->pHeadPacket[langIdx] = pContext->pCurrPacket[langIdx];
#endif
	}

	return ERR_OK;
}


#define ___________MICRODVD_FUNCTIONS___________

#define FRAME_PER_SEC	(1000 / 25)	/* XXX: divider should be the frame rate of movie video */

static HERROR local_sub_GetMicrodvdTiming(HUINT8 *p, SvcCsoSub_Packet_t *packet, int *consumed)
{
	HUINT8 *org_p = p;
	int len, frm;

	*consumed = 0;
	packet->startTime = 0;
	packet->endTime = 0;

	if (p[0] != '{')
		return ERR_FAIL;
	p++;

	/* get frm */
	for (frm = 0, len = 0; '0' <= *p && *p <= '9'; p++, len++)
		frm = frm * 10 + *p - '0';
	if (len == 0 || p[0] != '}' || p[1] != '{')
		return ERR_FAIL;
	p += 2;

	packet->startTime = frm * FRAME_PER_SEC;

	for (frm = 0, len = 0; '0' <= *p && *p <= '9'; p++, len++)
		frm = frm * 10 + *p - '0';
	if (len == 0 || p[0] != '}')
		return ERR_FAIL;
	p++;

	packet->endTime = frm * FRAME_PER_SEC;

	*consumed = p - org_p;
	return ERR_OK;
}

static SvcCsoSub_Type_e local_sub_TestMicrodvd(HUINT8 *p, HUINT32 ulsize)
{
	SvcCsoSub_Packet_t	packet;
	int consumed;

	if (local_sub_GetMicrodvdTiming(p, &packet, &consumed) != ERR_OK)
		return eSVC_CSO_SUB_TYPE_NOTHING;
	return eSVC_CSO_SUB_TYPE_MICRODVD;
}

static HERROR local_sub_ParseMicrodvd(HUINT8 *p, HUINT32 ulsize, SvcCsoSub_Context_t *pContext)
{
	HUINT32		i, line_len, nlsize;
	HUINT8		*line = local_sub_GetLineBuffer();
	SvcCsoSub_Packet_t	*packet = NULL;
	int consumed;
	HUINT32		langIdx;

	HxLOG_Debug("=================== SUB %s[%d] ===================== \n", __FUNCTION__, __LINE__ );

	/* Set Default Style. */
	local_sub_AdaptDefaultStyle(&pContext->stStyle);

	/* default�� 1��. */
	pContext->ulTotalLang++;

	for (i = 0; i < ulsize; i += line_len, p += line_len) {
		line_len = local_sub_ReadTextLine(p, line, &nlsize);

		if (line_len - nlsize == 0)	/* empty line */
			continue;

		if (line_len - nlsize >= SVC_CSO_SUB_LINE_BUF_SIZE)		/*Buffer overFlow*/
			continue;

		line[line_len - nlsize] = '\0';

		packet = (SvcCsoSub_Packet_t *) HLIB_STD_MemAlloc(sizeof(SvcCsoSub_Packet_t));
		if (packet == NULL) {
			HxLOG_Error("Memory Alloc Fail \n");
			return ERR_FAIL;
		}
		memset(packet, 0, sizeof(SvcCsoSub_Packet_t));

		if (local_sub_GetMicrodvdTiming(line, packet, &consumed) != ERR_OK) {
			HLIB_STD_MemFree(packet);
			continue;
		}
		if (local_sub_GetPowerdivxText(line + consumed, packet) != ERR_OK) {
			HLIB_STD_MemFree(packet);
			continue;
		}

		local_sub_InsertSubPacket(pContext, 0, packet);
	}

	for ( langIdx = 0; langIdx < pContext->ulTotalLang; langIdx++ )
	{
		/* Set Time Boundary. */
		local_sub_SetTimeBoundary(pContext, langIdx);

#if defined (_CONFIG_DEBUG)
		pContext->pHeadPacket[langIdx] = pContext->pCurrPacket[langIdx];
#endif
	}

	return ERR_OK;
}


#define ___________TEXT_FUNCTIONS___________

static HERROR local_sub_GetTextTiming(HUINT8 *p, HUINT32 *msec, int *consumed)
{
	int i, len, num_colon;

	*consumed = 0;
	*msec = 0;

	len = local_sub_timestr2msec(p, msec);
	if (len == 0)
		return ERR_FAIL;
	for (i = 0, num_colon = 0; i < len; i++)
		if (p[i] == ':')
			num_colon++;
	if (num_colon != 2)
		return ERR_FAIL;

	p += len;
	if (p[0] == ' ' || p[0] == ':')
		len += 1;
	else if (p[0] != '\0')
		return ERR_FAIL;

	*consumed = len;
	return ERR_OK;
}

static SvcCsoSub_Type_e local_sub_TestText(HUINT8 *p, HUINT32 ulsize)
{
	HUINT32		i, line_len, nlsize, msec;
	HUINT8		*line = local_sub_GetLineBuffer();
	int consumed, line_cnt;

	for (i = 0, line_cnt = 0; i < ulsize && line_cnt < 5; i += line_len, p += line_len, line_cnt++) {
		line_len = local_sub_ReadTextLine(p, line, &nlsize);

		if (line_len - nlsize < SVC_CSO_SUB_LINE_BUF_SIZE)
			line[line_len - nlsize] = '\0';

		if (local_sub_GetTextTiming(line, &msec, &consumed) != ERR_OK)
			return eSVC_CSO_SUB_TYPE_NOTHING;
	}

	return eSVC_CSO_SUB_TYPE_TEXT;
}

static HERROR local_sub_ParseText(HUINT8 *p, HUINT32 ulsize, SvcCsoSub_Context_t *pContext)
{
	HUINT32		i, line_len, nlsize, msec;
	HUINT8		*line = local_sub_GetLineBuffer();
	SvcCsoSub_Packet_t	*packet = NULL, *prev_packet = NULL;
	int consumed;
	HUINT32		langIdx;

	HxLOG_Debug("=================== %s[%d] ===================== \n", __FUNCTION__, __LINE__ );

	/* Set Default Style. */
	local_sub_AdaptDefaultStyle(&pContext->stStyle);

	/* default�� 1��. */
	pContext->ulTotalLang++;

	for (i = 0; i < ulsize; i += line_len, p += line_len) {
		line_len = local_sub_ReadTextLine(p, line, &nlsize);

		if (line_len - nlsize == 0)	/* empty line */
			continue;

		if (line_len - nlsize >= SVC_CSO_SUB_LINE_BUF_SIZE)		/*Buffer overFlow*/
			continue;

		line[line_len - nlsize] = '\0';

		if (local_sub_GetTextTiming(line, &msec, &consumed) != ERR_OK) {
			continue;
		}

		packet = (SvcCsoSub_Packet_t *) HLIB_STD_MemAlloc(sizeof(SvcCsoSub_Packet_t));
		if (packet == NULL) {
			HxLOG_Error("Memory Alloc Fail \n");
			return ERR_FAIL;
		}
		memset(packet, 0, sizeof(SvcCsoSub_Packet_t));
		packet->startTime = msec;

		if (local_sub_GetPowerdivxText(line + consumed, packet) != ERR_OK) {
			HLIB_STD_MemFree(packet);
			packet = NULL;
			continue;
		}

		if (prev_packet)
			prev_packet->endTime = msec - 1;
		prev_packet = packet;
		local_sub_InsertSubPacket(pContext, 0, packet);
	}

	/* set end time */
	if (packet)
		packet->endTime = packet->startTime + SVC_CSO_SUB_SAMI_DEFAULT_DUR_TIME;

	for ( langIdx = 0; langIdx < pContext->ulTotalLang; langIdx++ )
	{
		/* Set Time Boundary. */
		local_sub_SetTimeBoundary(pContext, langIdx);

#if defined (_CONFIG_DEBUG)
		pContext->pHeadPacket[langIdx] = pContext->pCurrPacket[langIdx];
#endif
	}

	return ERR_OK;
}


#define ___________SUBSTATION_ALPHA_FUNCTIONS___________

static int ssa_start_time_loc, ssa_end_time_loc, ssa_text_loc;

static int local_sub_CopySSAText(HUINT8 *dst, HUINT8 *src, int len)
{
	int command = FALSE, i, dst_len;

	dst_len = 0;
	for (i = 0; i < len; i++) {
		if (*src == '{' && *(src+1) == '\\') {
			command = TRUE;
			src += 2;
		} else if (command == TRUE) {
			if (*src == '}')
				command = FALSE;
			src++;
		} else {
			*dst++ = *src++;
			dst_len++;
		}
	}
	*dst = '\0';
	return dst_len;
}

static HERROR local_sub_GetSSAText(HUINT8 *p, SvcCsoSub_Packet_t *packet)
{
	HUINT8 *line;
	int len;

	len = 0;
	line = p;
	for (; *p; p++) {
		if ((p[0] == '\\') && ((p[1] | 0x20) == 'n')) {
			if (len == 0) {
				line[0] = ' ';
				len = 1;
			}
			packet->pText[packet->ulStrNum] = (HUINT8 *)HLIB_STD_MemAlloc(len + 1);
			if (packet->pText[packet->ulStrNum] == NULL) {
				while (packet->ulStrNum) {
					packet->ulStrNum--;
					HLIB_STD_MemFree(packet->pText[packet->ulStrNum]);
					packet->pText[packet->ulStrNum] = NULL;
				}
				return ERR_FAIL;
			}
			len = local_sub_CopySSAText(packet->pText[packet->ulStrNum], line, len);
			packet->bNewLine[packet->ulStrNum] = TRUE;
			packet->ulStrNum++;
			if (packet->ulStrNum >= SVC_CSO_SUB_LINE_MAX)
				return ERR_OK;
			p += 1;
			len = 0;
			line = p + 1;
			continue;
		} else if (p[0] == 0x0d || p[0] == 0x0a) {
			break;
		}
		len++;
	}

	if (len == 0) {
		line[0] = ' ';
		len = 1;
	}
	if (len) {
		packet->pText[packet->ulStrNum] = (HUINT8 *)HLIB_STD_MemAlloc(len + 1);
		if (packet->pText[packet->ulStrNum] == NULL) {
			while (packet->ulStrNum) {
				packet->ulStrNum--;
				HLIB_STD_MemFree(packet->pText[packet->ulStrNum]);
				packet->pText[packet->ulStrNum] = NULL;
			}
			return ERR_FAIL;
		}
		len = local_sub_CopySSAText(packet->pText[packet->ulStrNum], line, len);
		packet->bNewLine[packet->ulStrNum] = TRUE;
		packet->ulStrNum++;
	}

	return ERR_OK;
}

static HERROR local_sub_AddSSAPacket(HUINT8 *p, int line_len, SvcCsoSub_Context_t *pContext)
{
	int i, loc, command, num_got_fields;
	SvcCsoSub_Packet_t *packet;

	if (ssa_start_time_loc == 0 || ssa_end_time_loc == 0 || ssa_text_loc == 0)
		return ERR_FAIL;

	loc = 1;
	command = FALSE;
	i = 0;
	num_got_fields = 0;

	packet = (SvcCsoSub_Packet_t *) HLIB_STD_MemAlloc(sizeof(SvcCsoSub_Packet_t));
	if (packet == NULL) {
		HxLOG_Error("Memory Alloc Fail \n");
		return ERR_FAIL;
	}
	memset(packet, 0, sizeof(SvcCsoSub_Packet_t));

	while (i < line_len) {
		/* skip white space */
		for (; ISSPACE(p[i]); i++) {
			if (p[i] == '\0')
			{
				HLIB_STD_MemFree(packet);
				return ERR_FAIL;
			}
		}

		if (loc == ssa_start_time_loc) {
			i += local_sub_timestr2msec(p + i, &packet->startTime);
			num_got_fields++;
		} else if (loc == ssa_end_time_loc) {
			i += local_sub_timestr2msec(p + i, &packet->endTime);
			num_got_fields++;
		} else if (loc == ssa_text_loc) {
			if (local_sub_GetSSAText(p + i, packet) == ERR_OK)
				num_got_fields++;
			break;
		}

		/* find next ',' */
		for (; p[i] != ','; i++) {
			if (p[i] == '\0')
			{
				HLIB_STD_MemFree(packet);
				return ERR_FAIL;
			}
		}
		loc++;
		i++;
	}

	if (num_got_fields != 3) {
		HLIB_STD_MemFree(packet);
		return ERR_FAIL;
	}

	local_sub_InsertSubPacket(pContext, 0, packet);

	(void)(command);
	return ERR_OK;
}

static HERROR local_sub_SetSSAFormatLoc(HUINT8 *p)
{
	HUINT8 *word;
	int loc;

	ssa_start_time_loc = ssa_end_time_loc = ssa_text_loc = 0;
	word = p;
	loc = 1;
	for (; *p; p++) {
		if (*p == ',') {
			word = p + 1;
			loc++;
		} else if (ISSPACE(*p) || *p == ':') {
			word = p + 1;
		} else if (strncasecmp(word, "start", 5) == 0) {
			ssa_start_time_loc = loc;
			p += 4;
		} else if (strncasecmp(word, "end", 3) == 0) {
			ssa_end_time_loc = loc;
			p += 2;
		} else if (strncasecmp(word, "text", 4) == 0) {
			ssa_text_loc = loc;
			p += 3;
		}
	}
	if (ssa_start_time_loc && ssa_end_time_loc && ssa_text_loc)
		return ERR_OK;
	return ERR_FAIL;
}

static SvcCsoSub_Type_e local_sub_TestSubstationAlpha(HUINT8 *p, HUINT32 ulsize)
{
	HUINT32		i, line_len, nlsize, line_cnt;
	HUINT8		*line = local_sub_GetLineBuffer();
	int proceed;

	line_cnt = 0;

	proceed = FALSE;
	for (i = 0; i < ulsize && line_cnt < 100; i += line_len, p += line_len, line_cnt++) {
		line_len = local_sub_ReadTextLine(p, line, &nlsize);

		if (line_len - nlsize == 0)	/* empty line */
			continue;

		if (line_len - nlsize >= SVC_CSO_SUB_LINE_BUF_SIZE)		/*Buffer overFlow*/
			continue;

		line[line_len - nlsize] = '\0';

		if (strcasecmp(line, "[events]") == 0) {
			proceed = TRUE;
			break;
		}
	}
	if (proceed == FALSE)
		return eSVC_CSO_SUB_TYPE_NOTHING;

	for (i = 0; i < ulsize && line_cnt < 100; i += line_len, p += line_len, line_cnt++) {
		line_len = local_sub_ReadTextLine(p, line, &nlsize);

		if (line_len - nlsize == 0)	/* empty line */
			continue;

		if (line_len - nlsize >= SVC_CSO_SUB_LINE_BUF_SIZE)		/*Buffer overFlow*/
			continue;

		line[line_len - nlsize] = '\0';

		if (strncasecmp(line, "format:", 7) == 0) {
			if (local_sub_SetSSAFormatLoc(line) == ERR_OK)
				return eSVC_CSO_SUB_TYPE_SSA;
			else
				return eSVC_CSO_SUB_TYPE_NOTHING;
		}
	}
	return eSVC_CSO_SUB_TYPE_NOTHING;
}

static HERROR local_sub_ParseSubstationAlpha(HUINT8 *p, HUINT32 ulsize, SvcCsoSub_Context_t *pContext)
{
	HUINT32		i, line_len, nlsize;
	HUINT8		*line = local_sub_GetLineBuffer();
	int proceed;
	HUINT32		langIdx;

	HxLOG_Debug("=============================== SSA Parsing ================================ \n");

	/* Set Default Style. */
	local_sub_AdaptDefaultStyle(&pContext->stStyle);

	/* default�� 1��. */
	pContext->ulTotalLang++;

	proceed = FALSE;
	for (i = 0; i < ulsize; i += line_len, p += line_len) {
		line_len = local_sub_ReadTextLine(p, line, &nlsize);

		if (line_len - nlsize == 0)	/* empty line */
			continue;

		if (line_len - nlsize >= SVC_CSO_SUB_LINE_BUF_SIZE)		/*Buffer overFlow*/
			continue;

		line[line_len - nlsize] = '\0';

		if (strcasecmp(line, "[events]") == 0) {
			proceed = TRUE;
			break;
		}
	}
	if (proceed == FALSE)
		return eSVC_CSO_SUB_TYPE_NOTHING;

	for (i = 0; i < ulsize; i += line_len, p += line_len) {
		line_len = local_sub_ReadTextLine(p, line, &nlsize);

		if (line_len - nlsize == 0)	/* empty line */
			continue;

		if (line_len - nlsize >= SVC_CSO_SUB_LINE_BUF_SIZE)		/*Buffer overFlow*/
			continue;

		line[line_len - nlsize] = '\0';

		if (strncasecmp(line, "format:", 7) == 0) {
			local_sub_SetSSAFormatLoc(line);
		}
		else if (strncasecmp(line, "dialogue:", 9) == 0) {
			local_sub_AddSSAPacket(line + 9, line_len - nlsize, pContext);
		}
	}

	for ( langIdx = 0; langIdx < pContext->ulTotalLang; langIdx++ )
	{
		/* Set Time Boundary. */
		local_sub_SetTimeBoundary(pContext, langIdx);

#if defined (_CONFIG_DEBUG)
		pContext->pHeadPacket[langIdx] = pContext->pCurrPacket[langIdx];
#endif
	}

	return ERR_OK;
}


#define ___________SAMI_FUNCTIONS___________
static void local_sub_GetSamiColor(HUINT8 *p, OSD_Color_t *pColor)
{
	HUINT32		i;
	HUINT32		color = 0;

	if ( *p == '#' )
	{
		for ( i = 0; i < 6; i++ )
		{
			p++;

			if ( 0x30 <= *p && *p <= 0x39 )
			{
				color |= ((*p - 48) << (20 - (4 * i)));
			}
			else if ( 0x41 <= *p && *p <= 0x46 )
			{
				color |= ((*p - 55) << (20 - (4 * i)));
			}
			else if ( 0x61 <= *p && *p <= 0x66 )
			{
				color |= ((*p - 87) << (20 - (4 * i)));
			}
		}

		color |= 0xff000000;

		*pColor = color;
	}
	else
	{
		for ( i = 0; i < eSVC_CSO_SUB_SAMI_COL_MAX; i++ )
		{
			if ( strcasecmp(s_stSVC_CSO_SUBSamiColorTbl[i].pName, p) == 0 )	{ break; }
		}

		if ( i == eSVC_CSO_SUB_SAMI_COL_MAX )
		{
			/* Set Default Color(white). */
			*pColor = SVC_CSO_SUB_DEFAULT_FONT_COLOR;
		}
		else
		{
			*pColor = s_stSVC_CSO_SUBSamiColorTbl[i].argb;
		}
	}
}


static int local_sub_GetSamiClassIndex(HUINT8 *p, SvcCsoSub_Context_t *pContext)
{
	int i, j, to;

	for (i = 0; i < pContext->ulTotalLang; i++) {
		to = strlen(pContext->stSamiClass[i].szClassName);
		for (j = 0; j < to; j++) {
			if ((pContext->stSamiClass[i].szClassName[j] | 0x20) != (p[j] | 0x20))
				break;
		}
		if ((p[j] == ' ' || p[j] == '>') && j == to) {
			return i;
		}
	}
	return 0;
}


static int local_sub_AddSamiClass(HUINT8 *p, int len, SvcCsoSub_Context_t *pContext)
{
	int offset = 1;
	int i, index = pContext->ulTotalLang;
	char *pPos;
#if DISPLAY_LANG_Hx
	char langcode[4] = { 0, };
#else
	char langcode[10] = { 0, };
#endif

	if (pContext->ulTotalLang >= SVC_CSO_SUB_LANGUAGE_MAX)
		return offset;

	memset(&pContext->stSamiClass[index], 0, sizeof(SvcCsoSub_SamiClass_t));

	/* get class name */
	for (i = 0; i < sizeof(pContext->stSamiClass[0].szClassName) - 1; i++, p++) {
		if (*p == ' ')
			break;
		pContext->stSamiClass[index].szClassName[i] = *p;
	}
	offset += i;
	len -= i;

	/* get name */
	/* XXX: remove double quotation mark */
	pPos = local_sub_Stristr(p, "name:");
	if (pPos) {
		pPos += 5;
		for (i = 0; i < len; i++, pPos++) {
			if (*pPos != ' ')
				break;
			if (*pPos == '}')
				return offset + ((unsigned long)pPos - (unsigned long)p);
		}
		for (i = 0; i < sizeof(pContext->stSamiClass[0].szName) - 1; i++, pPos++) {
			if (*pPos == ';')
				break;
			pContext->stSamiClass[index].szName[i] = *pPos;
		}
	}

	/* get language */
#if DISPLAY_LANG_Hx
	pContext->eDispLang[index] = eLangID_NONE;
#else
	memcpy(&pContext->szDispLang[index] , "none", sizeof("none"));
	//pContext->szDispLang[index] = "none";
#endif
	pPos = local_sub_Stristr(p, "lang:");
	if (pPos) {
		pPos += 5;
		for (i = 0; i < len; i++, pPos++) {
			if (*pPos != ' ')
				break;
			if (*pPos == '}')
				return offset + ((unsigned long)pPos - (unsigned long)p);
		}

		for (i = 0; i < sizeof(langcode) - 1; i++, pPos++) {
#if DISPLAY_LANG_Hx
			if (*pPos == ';' || *pPos == '-')
#endif
			if (*pPos == ';')
				break;
			langcode[i] = *pPos;
		}

#if DISPLAY_LANG_Hx
		if (MWC_UTIL_GetLangIDBy639_1Code(langcode, &pContext->eDispLang[index]) != ERR_OK)
			pContext->eDispLang[index] = eLangID_English;
#else
		memcpy(&pContext->szDispLang[index] , langcode, sizeof(langcode));

#endif
	}

	pContext->ulTotalLang++;

	for (i = 0; i < len; i++, p++, offset++) {
		if (*p == '}')
			break;
	}

	return offset;
}


static HUINT32 local_sub_ParseSamiBody(HUINT8 *p, HUINT32 len, SvcCsoSub_Context_t *pContext)
{
	HUINT32		cnt = 0, charCnt = 0, size, nlsize, dataSize = 0;
	HUINT32		startTime;
	HUINT8		*pBuf = NULL, *pPos = NULL;
	SvcCsoSub_Packet_t	*pSubPkt = NULL;
	HBOOL		bNewLine;
	OSD_Color_t	color = 0;
	HUINT32		langIdx;
	int			sami_class_index = 0;

	do
	{
		pBuf = local_sub_GetLineBuffer();
		size = local_sub_ReadTextLine(p, pBuf, &nlsize);

		bNewLine = TRUE;
		charCnt = 0;

		/* Skip blank. */
		while ( ISSPACE(*pBuf) )	{ charCnt++; pBuf++; }

		/* Check Body end. */
		if ( strncasecmp(pBuf, "</BODY>", 7) == 0 )
		{
			local_sub_InsertSubPacket(pContext, sami_class_index, pSubPkt);
			pSubPkt = NULL;

			p += size;
			cnt += size;

			break;
		}

		while ( charCnt < size - nlsize )
		{
			/* Skip blank. */
			while ( ISSPACE(*pBuf) )	{ charCnt++; pBuf++; }
			if ( *pBuf == '<' )
			{
				if ( strncasecmp(pBuf, "<SYNC", 5) == 0 )
				{
					local_sub_InsertSubPacket(pContext, sami_class_index, pSubPkt);
					pSubPkt = NULL;

					/* Make sub packet. */
					pSubPkt = (SvcCsoSub_Packet_t *) HLIB_STD_MemAlloc(sizeof(SvcCsoSub_Packet_t));
					if ( pSubPkt == NULL )
					{
						HxLOG_Error("Memory Alloc Fail \n");
						break;
					}

					memset(pSubPkt, 0x00, sizeof(SvcCsoSub_Packet_t));

					pPos = local_sub_Stristr(pBuf, "Start");
					if ( pPos != NULL )
					{
						while ( *pPos != '=' )	{ pPos++; }
						pPos++;

						while ( ISSPACE(*pBuf) )	{ charCnt++; pBuf++; }

						/* Set Time. */
						startTime = atoi(pPos);

						pSubPkt->startTime = startTime;
					}

					while ( *pBuf != '>' )	{ charCnt++; pBuf++; }
					charCnt++;
					pBuf++;

					color = 0;
				}
				else if ( strncasecmp(pBuf, "<P", 2) == 0 )
				{
					HBOOL bNoClass = FALSE;
					pPos = pBuf;
					while ( *pPos != '='  )
					{
						if(*pPos == '>')
						{
							bNoClass = TRUE;
							break;
						}
						pPos++;
					}
					if(bNoClass != TRUE)
					{
						pPos++;

						if ( pSubPkt != NULL )
						{
							sami_class_index = local_sub_GetSamiClassIndex(pPos, pContext);
#if DISPLAY_LANG_Hx
							pSubPkt->eLangId = pContext->eDispLang[sami_class_index];
#endif
						}
					}

					while ( *pBuf != '>' )	{ charCnt++; pBuf++; }
					charCnt++;
					pBuf++;
				}
				else if ( strncasecmp(pBuf, "<font", 5) == 0 )
				{
					if ( pSubPkt != NULL )
					{
						pSubPkt->fontColor[pSubPkt->ulStrNum] = pContext->stStyle.ftColor;
					}

					/* Check Color. */
					pPos = local_sub_Stristr(pBuf, "color");
					if ( pPos != NULL )
					{
						while ( *pPos != '=' ) 	{ pPos++; }
						pPos++;

						while ( ISSPACE(*pBuf) )	{ charCnt++; pBuf++; }

						/* Get Color. */
						local_sub_GetSamiColor(pPos, &color);
					}

					pPos = local_sub_Stristr(pBuf, "size");
					if ( pPos != NULL )
					{
						/* �ϴ� ���� size�� skip �Ѵ�. */
					}

					while ( *pBuf != '>' )	{ charCnt++; pBuf++; }
					charCnt++;
					pBuf++;
				}
				else if ( strncasecmp(pBuf, "</font>", 7) == 0 )
				{
					color = 0;

					charCnt += 7;
					pBuf += 7;
				}
				else
				{
					while ( *pBuf != '>' && charCnt < size - nlsize )	{ charCnt++; pBuf++; }
					charCnt++;
					pBuf++;
				}
			}
			else if ( strncasecmp(pBuf, "&nbsp;", 6) == 0 )
			{
				charCnt += 6;
				pBuf += 6;
			}
			else if ( size - charCnt > nlsize )
			{
				while ( charCnt < size - nlsize )
				{

					if  ( ISSPACE(*pBuf) )	{ charCnt++; pBuf++; continue; }

					pPos = pBuf;
					dataSize = 0;

					if ( *pBuf == '<' )
					{
						if ( strncasecmp(pBuf, "<br>", 4) == 0 )
						{
							bNewLine = TRUE;
							charCnt += 4;
							pBuf += 4;
						}
						else
						{
							while ( *pBuf != '>' )	{ charCnt++; pBuf++; }
							charCnt++;
							pBuf++;
						}

						continue;
					}

					while ( *pPos != '<' && (charCnt + dataSize < size) ) { pPos++; dataSize++; }

					if ( pSubPkt != NULL )
					{
						/* Set Default value. */
						pSubPkt->fontColor[pSubPkt->ulStrNum] = (color == 0) ? pContext->stStyle.ftColor : color;
						pSubPkt->bNewLine[pSubPkt->ulStrNum] = bNewLine;
					}

					if ( charCnt + dataSize == size )
					{
						local_sub_InsertText(pBuf, dataSize, nlsize, pSubPkt);
					}
					else
					{
						local_sub_InsertText(pBuf, dataSize, 0, pSubPkt);
					}

					bNewLine = FALSE;

					charCnt += dataSize;
					pBuf += dataSize;
				}
			}
			else
			{
				charCnt++;
				pBuf++;
			}
		}

		p += size;
		cnt += size;
	} while ( *p && cnt < len );

	for ( langIdx = 0; langIdx < pContext->ulTotalLang; langIdx++ )
	{

		/* Set End Time. */
		local_sub_SetEndTime(pContext, langIdx);

		/* Set Time Boundary. */
		local_sub_SetTimeBoundary(pContext, langIdx);

#if defined (_CONFIG_DEBUG)
		pContext->pHeadPacket[langIdx] = pContext->pCurrPacket[langIdx];
#endif
	}

	if(pSubPkt != NULL)
	{
		HLIB_STD_MemFree(pSubPkt);
		pSubPkt = NULL;
	}

	return cnt;
}


static HUINT32 local_sub_ParseSamiHead(HUINT8 *p, HUINT32 len, SvcCsoSub_Context_t *pContext)
{
	HUINT32		i = 0, cnt = 0;
	HUINT8		*pos = NULL, *pTag = NULL;
	HUINT8		*pBuf = local_sub_GetLineBuffer();
	HBOOL		in_style = FALSE;

	while (*p && cnt < len)
	{
		if ( (cnt + 7) > len )	{ break; }
		if ( (*p == '<') && (*(p + 6) == '>')
			&& (*(p + 1) == '/')
			&& (*(p + 2) == 'h' || *(p + 2) == 'H')
			&& (*(p + 3) == 'e' || *(p + 3) == 'E')
			&& (*(p + 4) == 'a' || *(p + 4) == 'A')
			&& (*(p + 5) == 'd' || *(p + 5) == 'D') )
		{
			p += 7;
			cnt += 7;
			break;
		}

		if ( (cnt + 7) < len ) {
			if (strncasecmp(p, "<style", 6) == 0) {
				in_style = TRUE;
				p += 6;
				cnt += 6;
				continue;
			}
			if (strncasecmp(p, "</style", 7) == 0) {
				in_style = FALSE;
				p += 6;
				cnt += 6;
				continue;
			}
		}

		if ( *p == 'p' || *p == 'P' )
		{
			if ( *(p + 1) == ' ' )	{ p++; cnt++; }
			if ( *(p + 1) == '{' )
			{
				i++;
				pos = (p++);
				while ( *pos != '}' )	{ i++; pos++; }
				if ( i < SVC_CSO_SUB_LINE_BUF_SIZE )
				{
					memset(pBuf, 0x00, SVC_CSO_SUB_LINE_BUF_SIZE);
					memcpy (pBuf, p, i + 1);

					/* font size�� �����ϴ� ���� ���� ��. */
					pTag = local_sub_Stristr(pBuf, "font-size:");
					if ( pTag != NULL )
					{
						pTag += 10;

						while ( ISSPACE(*pTag) )	{ pTag++; }
					}

					pTag = local_sub_Stristr(pBuf, "color:");
					if ( pTag != NULL )
					{
						pTag += 6;
						while ( ISSPACE(*pTag) )	{ pTag++; }
						local_sub_GetSamiColor(pTag, &pContext->stStyle.ftColor);
					}

					cnt += i;
					p += i;
				}
			}
		}

		if ( *p == '.' && in_style)
		{
			int offset = local_sub_AddSamiClass(p + 1, len - cnt, pContext);
			p += offset;
			cnt += offset;
		}
		else
		{
			cnt++;
			p++;
		}
	}

	return cnt;
}

static SvcCsoSub_Type_e local_sub_TestSami(HUINT8 *pData, HUINT32 ulSize)
{
	int i;

	if (pData == NULL)
		return eSVC_CSO_SUB_TYPE_NOTHING;

	for (i = 0; i < ulSize; i++, pData++) {
		if (!ISSPACE(*pData))
			break;
	}
	if (i >= ulSize)
		return eSVC_CSO_SUB_TYPE_NOTHING;
	if (strncasecmp(pData, "<sami", 5) == 0)
		return eSVC_CSO_SUB_TYPE_SAMI;
	return eSVC_CSO_SUB_TYPE_NOTHING;
}

static HERROR local_sub_ParseSami(HUINT8 *pData, HUINT32 ulSize, SvcCsoSub_Context_t *pContext)
{
	HUINT32		cnt = 0, size;

	HxLOG_Debug("======================== SAMI Parsing ================================\n");

	if ( pData == NULL || pContext == NULL )
	{
		return ERR_FAIL;
	}

	/* Set Default Style. */
	local_sub_AdaptDefaultStyle(&pContext->stStyle);

	while( (*pData) && (cnt < ulSize) )
	{
		if ( ((ulSize - cnt) > 5) && (*pData == '<') && (*(pData + 5) == '>') )
		{
			if ( (*(pData + 1) == 'h' || *(pData + 1) == 'H')
				&& (*(pData + 2) == 'e' || *(pData + 2) == 'E')
				&& (*(pData + 3) == 'a' || *(pData + 3) == 'A')
				&& (*(pData + 4) == 'd' || *(pData + 4) == 'D') )
			{
				cnt += 6;
				pData += 6;

				size = local_sub_ParseSamiHead(pData, (ulSize - cnt), pContext);
				cnt += size;
				pData += size;
			}
			else if ( (*(pData + 1) == 'b' || *(pData + 1) == 'B')
				&& (*(pData + 2) == 'o' || *(pData + 2) == 'O')
				&& (*(pData + 3) == 'd' || *(pData + 3) == 'D')
				&& (*(pData + 4) == 'y' || *(pData + 4) == 'Y') )
			{
				cnt += 6;
				pData += 6;

				/* Head�� ���� ��츦 ����. */
				if (pContext->ulTotalLang == 0)
				{
					memset(&pContext->stSamiClass[0], 0, sizeof(SvcCsoSub_SamiClass_t));
#if DISPLAY_LANG_Hx
					pContext->eDispLang[0] = eLangID_NONE;
#else
					memcpy(&pContext->szDispLang[0] , "none", sizeof("none"));
#endif
					pContext->ulTotalLang++;
				}

				size = local_sub_ParseSamiBody(pData, (ulSize - cnt), pContext);
				cnt += size;
				pData += size;
			}
			else
			{
				pData += 6;
				cnt += 6;
			}
		}
		else
		{
			pData++;
			cnt++;
		}
	}

	HxLOG_Debug("======================== SAMI Parsing DONE ================================\n");
	return ERR_OK;
}

#define ______PACKET_PROCESSS_FUNC_______

SvcCsoSub_Display_Packet_t st_SubDiaplayData;
static void local_sub_DisplayPacket(SvcCsoSub_Context_t *pContext, SvcCsoSub_Packet_t *pPacket)
{
	HUINT32		i, lineCnt, strCnt;

	if ( pContext == NULL || pPacket == NULL )
	{
		return;
	}

	if(pPacket->ulStrNum == 0)
	{
		return;
	}

	lineCnt = 0;
	for ( i = 0; i < pPacket->ulStrNum; i++ )
	{
		/* ���� line�� Ȯ�� �Ѵ�. */
		if ( pPacket->bNewLine[i] == TRUE )
		{
			lineCnt++;
		}

		if ( lineCnt == 0 )
		{
			return;
		}
	}

	pContext->bIsDisplay = TRUE;

	HxSTD_memset(&st_SubDiaplayData, 0x00, sizeof(SvcCsoSub_Display_Packet_t));

	if(pPacket->ulStrNum > SVC_CSO_SUB_LINE_MAX)
		st_SubDiaplayData.ulStrNum = SVC_CSO_SUB_LINE_MAX;
	else
		st_SubDiaplayData.ulStrNum = pPacket->ulStrNum;

	/* default font size */
	if(pContext->ulFontSize == 0)
		st_SubDiaplayData.ulFontSize = 36;

	for(i =0; i<st_SubDiaplayData.ulStrNum; i++)
	{
		strCnt = 0;
		/* parsing error */
		if(pPacket->pText[i] == NULL)
			continue;

		strCnt = HxSTD_StrLen(pPacket->pText[i]);
		if(strCnt > SVC_CSO_SUB_STR_MAX) strCnt = SVC_CSO_SUB_STR_MAX;

		memcpy(&st_SubDiaplayData.pText[i], pPacket->pText[i], strCnt);
	}

	s_tSubEventCallback(pContext->player, eSVC_CSO_EVT_MEDIA_SUB_UPDATE, (HUINT8 *) &st_SubDiaplayData);
}

static HERROR local_sub_ProcessPacket(void)
{
	HERROR		hErr;
	HUINT32		i, cnt = 0;
	POINTER_LIST_T	*pPos, *pNextPos;
	SvcCsoSub_Packet_t	*pPacket = NULL, *pNextPacket = NULL;
	SvcCsoSub_Context_t *pContext;
	SvcCso_t *p;
	HUINT64		msTime;

	for(i = 0 ; i<SVC_CSO_SUB_MAX_CAPABILITY; i++)
	{
		if(s_pstSvcCsoSubContextPool.s_stSvcCsoSubContext[i])
		{
			p = s_pstSvcCsoSubContextPool.s_stSvcCsoSubContext[i];
			pContext = p->pSubContext;
		}
		else
			continue;

		if(pContext->status != eSVC_CSO_SUB_STATUS_PLAY)
		{
			HxLOG_Error("it's not play status \n");
			continue;
		}

		if ( pContext->ulTotalPacket[pContext->ulLangIdx] == 0 )
		{
			HxLOG_Error("it's not ready packets \n");
			return ERR_FAIL;
		}

		hErr = SVC_CSO_GetPlayPosition(p, &msTime);
		if ( hErr != ERR_OK )
		{
			/* TODO -���� ���Ͻ� �ش� Context ���� ���� ���� */
			return ERR_FAIL;
		}

		if ( MACRO_CHECK_TIME_BOUNDARY(msTime, pContext->ulFirstTime[pContext->ulLangIdx], pContext->ulLastTime[pContext->ulLangIdx]) == FALSE )
		{
			return ERR_FAIL;
		}

		/* Offset time ����. */
		msTime += pContext->msOffsetTime;

		pPos = pContext->pCurrPacket[pContext->ulLangIdx];

		while ( cnt++ < pContext->ulTotalPacket[pContext->ulLangIdx] )
		{
			if ( pPos == NULL )
			{
				break;
			}

			/* Get Current Packet. */
			pPacket = UTIL_LINKEDLIST_GetContents(pPos);

			if ( MACRO_CHECK_TIME_BOUNDARY(msTime, pPacket->startTime, pPacket->endTime) == TRUE )
			{
				pNextPos = UTIL_LINKEDLIST_GetNextPointer(pPos);
				if ( pNextPos == NULL )
				{
					break;
				}

				pNextPacket = UTIL_LINKEDLIST_GetContents(pNextPos);

				if ( MACRO_CHECK_TIME_BOUNDARY(msTime, pNextPacket->startTime, pNextPacket->endTime) == FALSE )
				{
					if(pContext->pNowPos != (HINT32) pPos)
					{
						pContext->pNowPos = (HINT32) pPos;
						local_sub_DisplayPacket(pContext, pPacket);
					}
					break;
				}

				if(pContext->bIsDisplay == TRUE)
				{
					pContext->bIsDisplay = FALSE;
					s_tSubEventCallback(pContext->player, eSVC_CSO_EVT_MEDIA_SUB_UPDATE, NULL);
				}

				pPos = pNextPos;
			}
			else
			{
				if(pContext->bIsDisplay == TRUE)
				{
					pContext->bIsDisplay = FALSE;
					s_tSubEventCallback(pContext->player, eSVC_CSO_EVT_MEDIA_SUB_UPDATE, NULL);
				}

				pPos = (msTime < pPacket->startTime) ? UTIL_LINKEDLIST_GetPrevPointer(pPos) : UTIL_LINKEDLIST_GetNextPointer(pPos);
			}
		}

		if ( pPos != NULL )
		{
			pContext->pCurrPacket[pContext->ulLangIdx] = pPos;
		}
	}

	return ERR_OK;
}

static void local_sub_300ms(HLONG ulTimerId, HUINT32 *param)
{
	HERROR 		hErr;
	SvcCsoSub_Msg_t	stMsg;

	if ( ulTimerId != s_ulSvcCsoSubTimerId )
	{
		return;
	}

	stMsg.ulCmd = CMD_SVC_CSO_SUB_300_MS;
	hErr = VK_MSG_SendTimeout(local_sub_GetMessageQId(), &stMsg, sizeof(SvcCsoSub_Msg_t), 0);
	if( hErr != ERR_OK )
	{
		HxLOG_Error("[local_sub_300ms] hErr:0x%X\n", hErr);
	}
}

static void local_sub_ServiceTask(void)
{
	HERROR			hErr;
	SvcCsoSub_Msg_t		stMsg;

	while (1)
	{
		hErr = VK_MSG_Receive(local_sub_GetMessageQId(), &stMsg, sizeof(SvcCsoSub_Msg_t));
		if ( hErr != ERR_OK )
		{
			continue;
		}

		SVC_CSO_SUB_CRITICAL_SECTION_IN;

		switch (stMsg.ulCmd)
		{
			case CMD_SVC_CSO_SUB_300_MS :
				local_sub_ProcessPacket();
				break;

			default :
				break;
		}

		SVC_CSO_SUB_CRITICAL_SECTION_OUT;
	}
}

/*******************************************************************/
/********************      API Functions   *************************/
/*******************************************************************/
static struct {
	SvcCsoSub_Type_e (*test)(HUINT8 *p, HUINT32 ulSize);
	HERROR (*parse)(HUINT8 *pData, HUINT32 ulSize, SvcCsoSub_Context_t *pContext);
} s_stMovieSubtitles[] =
{
	{
		local_sub_TestSami, /* .smi */
		local_sub_ParseSami
	},
	{
		local_sub_TestSrt, /* .srt */
		local_sub_ParseSrt
	},
	{
		local_sub_TestSubviewer_1,
		local_sub_ParseSubviewer_1
	},
	{
		local_sub_TestSubviewer_2,
		local_sub_ParseSubviewer_2
	},
	{
		local_sub_TestPowerdivx,
		local_sub_ParsePowerdivx /* .pub */
	},
	{
		local_sub_TestMicrodvd,
		local_sub_ParseMicrodvd /*. sub */
	},
	{
		local_sub_TestText,
		local_sub_ParseText
	},
	{
		local_sub_TestSubstationAlpha, /* .ssa .ssa */
		local_sub_ParseSubstationAlpha
	},
};

#define ____________API_FUNCTIONS_____________

#define SVC_CSO_SUB_MSGQ_SIZE              	 64
#define SVC_CSO_SUB_TASK_PRIORITY              VK_TASK_PRIORITY_MW_REF
#define SVC_CSO_SUB_TASK_STACK_SIZE            SIZE_64K

HERROR SVC_CSO_SUB_Init(SvcCsoSub_EventCallback_t pSubMediaEventCallback)
{
	HERROR          hErr;

	if( pSubMediaEventCallback == NULL )
	{
		HxLOG_Error("Empty callback function (NULL)\n");
		return ERR_FAIL;
	}

	s_tSubEventCallback = pSubMediaEventCallback;

	/* Media Subtitle service message queue ���� */
	hErr = (HUINT32)VK_MSG_Create(SVC_CSO_SUB_MSGQ_SIZE,
									sizeof(SvcCsoSub_Msg_t),
									"SVC_CSO_SUB_MsgQ",
									(unsigned long *)&s_ulSvcCsoSubMsgQId,
									VK_SUSPENDTYPE_FIFO);
	if ( hErr != VK_OK )
	{
		HxLOG_Error("Message Queue Create Fail \n");
	}

	/* Media Subtitle service task ���� */
	hErr = (HUINT32)VK_TASK_Create((void *)local_sub_ServiceTask,
									SVC_CSO_SUB_TASK_PRIORITY,
									SVC_CSO_SUB_TASK_STACK_SIZE,
									(char *)"tMwSVC_CSO_SUB",
									NULL,
									(unsigned long *)&s_ulSvcCsoSubTaskId,
									VK_SUSPENDTYPE_FIFO);
	if ( hErr != VK_OK )
	{
		HxLOG_Error("TASK Create Fail \n");
	}

	/* Media Semaphore ����. */
	hErr = VK_SEM_Create((unsigned long *)&s_ulSvcCsoSubSemaphore, "SVC_CSO_SUBSemaphore", VK_SUSPENDTYPE_PRIORITY);
	if ( hErr != VK_OK )
	{
		HxLOG_Error("Semaphore Create Fail \n");
	}

	hErr = VK_TASK_Start(s_ulSvcCsoSubTaskId);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("TASK Start Fail \n");
	}

	memset(&s_pstSvcCsoSubContextPool, 0x00,  sizeof(s_pstSvcCsoSubContextPool));

	return hErr;
}

HERROR SVC_CSO_SUB_Release(void)
{
	HINT32 i;
	SvcCsoSub_Context_t	*pContext = NULL;
	SvcCso_t *p;

	SVC_CSO_SUB_CRITICAL_SECTION_IN;

	for( i = 0; i<SVC_CSO_SUB_MAX_CAPABILITY; i++)
	{
		if(s_pstSvcCsoSubContextPool.s_stSvcCsoSubContext[i])
		{
			p = s_pstSvcCsoSubContextPool.s_stSvcCsoSubContext[i];
			pContext = p->pSubContext;

			if ( pContext->status == eSVC_CSO_SUB_STATUS_PLAY )
			{
				s_pstSvcCsoSubContextPool.ulPlaying--;
				local_sub_SetStatus(pContext, eSVC_CSO_SUB_STATUS_STOP);
			}

			local_sub_InitSVC_CSO_SUBContext(pContext);
			local_sub_SetStatus(pContext, eSVC_CSO_SUB_STATUS_RELEASE);

		}

		if(s_pstSvcCsoSubContextPool.ulPlaying == 0)
		{
			/* Cancel Timer. */
			VK_TIMER_Cancel(s_ulSvcCsoSubTimerId);
			s_ulSvcCsoSubTimerId = VK_TIMERID_NULL;
		}
	}

	SVC_CSO_SUB_CRITICAL_SECTION_OUT;

	return ERR_OK;
}

HERROR SVC_CSO_SUB_Create(SvcCso_t *player)
{
	int i, ret = ERR_FAIL;

	if(player == NULL)
	{
		HxLOG_Error("Media handel is null.\n");
		return ERR_FAIL;
	}

	for( i = 0; i<SVC_CSO_SUB_MAX_CAPABILITY; i++)
	{
		if(s_pstSvcCsoSubContextPool.s_stSvcCsoSubContext[i] == NULL)
		{
			s_pstSvcCsoSubContextPool.s_stSvcCsoSubContext[i] = player;
			ret = ERR_OK;
			break;
		}
	}

	if(ret != ERR_OK)
	{
		HxLOG_Error("Subtitle Capability is FULL !! \n");
		return ERR_FAIL;
	}

	player->pSubContext = (void *)HLIB_STD_MemAlloc(sizeof(SvcCsoSub_Context_t));
	if(player->pSubContext == NULL)
	{
		HxLOG_Error("Memery Alloc Error for Subtitle Context \n");
		return ERR_FAIL;
	}

	memset(player->pSubContext, 0x00, sizeof(SvcCsoSub_Context_t));

	return ERR_OK;
}

HERROR SVC_CSO_SUB_Destroy(SvcCso_t *player)
{
	SvcCsoSub_Context_t	*pContext = NULL;
	int i;

	if(player == NULL)
	{
		HxLOG_Error("Media handel is null.\n");
		return ERR_FAIL;
	}
	pContext = (SvcCsoSub_Context_t *)player->pSubContext;

	if(pContext == NULL)
	{
		HxLOG_Error("Media handel is null or Subtitle Context is not existed.\n");
		SVC_CSO_SUB_CRITICAL_SECTION_OUT;
		return ERR_FAIL;
	}

	HLIB_STD_MemFree(player->pSubContext);

	for( i = 0; i<SVC_CSO_SUB_MAX_CAPABILITY; i++)
	{
		if(s_pstSvcCsoSubContextPool.s_stSvcCsoSubContext[i] == player)
		{
			s_pstSvcCsoSubContextPool.s_stSvcCsoSubContext[i] = NULL;
			break;
		}
	}

	return ERR_OK;
}

HERROR SVC_CSO_SUB_Open(SvcCso_t *player, HUINT8 *ucFilePath)
{
	HERROR			hErr = ERR_OK;
	HINT32			fileSize = 0, tmpSize = 0;
	HUINT8			*pBuf = NULL;

	SvcCsoSub_Type_e	eSubType = eSVC_CSO_SUB_TYPE_NOTHING;
	SvcCsoSub_Context_t	*pContext = NULL;
	int 			i;
	HxFILE_t			hFile;

	SVC_CSO_SUB_CRITICAL_SECTION_IN;

	if ( ucFilePath == NULL )
	{
		HxLOG_Error("Wrong file path(NULL)\n");
		SVC_CSO_SUB_CRITICAL_SECTION_OUT;

		return ERR_FAIL;
	}

	if(player == NULL)
	{
		HxLOG_Error("Media handel is null.\n");
		return ERR_FAIL;
	}

	pContext = (SvcCsoSub_Context_t *)player->pSubContext;

	if(pContext == NULL)
	{
		HxLOG_Error("Media handel is null or Capacity of Subtitle Context is FULL \n");
		SVC_CSO_SUB_CRITICAL_SECTION_OUT;
		return ERR_FAIL;
	}

	if ( pContext->status != eSVC_CSO_SUB_STATUS_RELEASE )
	{
		HxLOG_Error("Check Status = (%s)\n", local_sub_GetSVC_CSO_SUBStatusText(pContext->status));
		SVC_CSO_SUB_CRITICAL_SECTION_OUT;

		return ERR_FAIL;
	}

	hErr = local_sub_FindMediaSubFile(ucFilePath, &hFile);
	if ( hErr == ERR_FAIL )
	{
		HxLOG_Error("File open fail.\n");
		SVC_CSO_SUB_CRITICAL_SECTION_OUT;

		return ERR_FAIL;
	}

	hErr = HLIB_FILE_Size32(hFile, &fileSize);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("Check file size (%d).\n", fileSize);
		SVC_CSO_SUB_CRITICAL_SECTION_OUT;
		HLIB_FILE_Close(hFile);
		return ERR_FAIL;
	}
	/* Alloc buffer. */
	pBuf = (HUINT8 *)HLIB_STD_MemAlloc(fileSize + 1);
	if ( pBuf == NULL )
	{
		HxLOG_Error("Alloc fail.\n");
		SVC_CSO_SUB_CRITICAL_SECTION_OUT;
		HLIB_FILE_Close(hFile);
		return ERR_FAIL;
	}

	pBuf[fileSize] = '\0';

	/* Load file to memory. */
	tmpSize = HLIB_FILE_Read(hFile, pBuf, 1, fileSize);
	if ( fileSize != tmpSize )
	{
		HLIB_STD_MemFree(pBuf);
		HxLOG_Error("Check file size (%d : %d) .\n", fileSize, tmpSize);
		SVC_CSO_SUB_CRITICAL_SECTION_OUT;
		HLIB_FILE_Close(hFile);
		return ERR_FAIL;
	}

	/* Close file. */
	HLIB_FILE_Close(hFile);


	/* Init Context. */
	local_sub_InitSVC_CSO_SUBContext(pContext);

	hErr = ERR_FAIL;

	/* test & parse subtitle */
	for (i = 0; i < sizeof(s_stMovieSubtitles) / sizeof(s_stMovieSubtitles[0]); i++) {
		eSubType = s_stMovieSubtitles[i].test(pBuf, fileSize);
		if (eSubType != eSVC_CSO_SUB_TYPE_NOTHING) {
			hErr = s_stMovieSubtitles[i].parse(pBuf, fileSize + 1, pContext);
			break;
		}
	}

	/* Free buffer. */
	HLIB_STD_MemFree(pBuf);

	if ( hErr != ERR_OK )
	{
		HxLOG_Error("Parsing fail!\n");
		SVC_CSO_SUB_CRITICAL_SECTION_OUT;

		return ERR_FAIL;
	}

	/* Get Screen. */
	pContext->player = player;
	pContext->eFileType = eSubType;
//	pContext->eEncoding = eHxCP_UNICODE_UTF8;	/* default encoding. */

	local_sub_SetStatus(pContext, eSVC_CSO_SUB_STATUS_SETUP);

	player->pSubContext = (void *)pContext;

	SVC_CSO_SUB_CRITICAL_SECTION_OUT;

	return ERR_OK;
}

HERROR SVC_CSO_SUB_Close( SvcCso_t *player )
{
	SvcCsoSub_Context_t	*pContext = NULL;
	if(player == NULL)
	{
		HxLOG_Error("Media handel is null.\n");
		return ERR_FAIL;
	}

	SVC_CSO_SUB_CRITICAL_SECTION_IN;

	pContext = (SvcCsoSub_Context_t *)player->pSubContext;

	if(pContext == NULL)
	{
		HxLOG_Error("Subtitle Context is not existed.\n");
		SVC_CSO_SUB_CRITICAL_SECTION_OUT;
		return ERR_FAIL;
	}

	if ( pContext->status == eSVC_CSO_SUB_STATUS_PLAY )
	{
		s_pstSvcCsoSubContextPool.ulPlaying--;
		local_sub_SetStatus(pContext, eSVC_CSO_SUB_STATUS_STOP);
	}

	if(s_pstSvcCsoSubContextPool.ulPlaying == 0)
	{
		/* Cancel Timer. */
		VK_TIMER_Cancel(s_ulSvcCsoSubTimerId);
		s_ulSvcCsoSubTimerId = VK_TIMERID_NULL;
	}

	/* Init Context. */
	local_sub_InitSVC_CSO_SUBContext(pContext);

	local_sub_SetStatus(pContext, eSVC_CSO_SUB_STATUS_RELEASE);

	SVC_CSO_SUB_CRITICAL_SECTION_OUT;

	return ERR_OK;
}

HERROR SVC_CSO_SUB_SetDisplay(SvcCso_t *player, HBOOL bEanble)
{
	SvcCsoSub_Context_t	*pContext = NULL;

	if(player == NULL)
	{
		HxLOG_Error("Media handel is null.\n");
		return ERR_FAIL;
	}

	pContext = (SvcCsoSub_Context_t *)player->pSubContext;

	SVC_CSO_SUB_CRITICAL_SECTION_IN;

	if(pContext == NULL )
	{
		HxLOG_Error("Media handel is null or Subtitle Context is not existed.\n");
		SVC_CSO_SUB_CRITICAL_SECTION_OUT;
		return ERR_FAIL;
	}

	/* Check Status. */
	if ( pContext->status == eSVC_CSO_SUB_STATUS_RELEASE )
	{
		HxLOG_Error("Check Status = (%s)\n", local_sub_GetSVC_CSO_SUBStatusText(pContext->status));
		SVC_CSO_SUB_CRITICAL_SECTION_OUT;
		return ERR_FAIL;
	}

	if( bEanble )
	{
		local_sub_SetStatus(pContext, eSVC_CSO_SUB_STATUS_PLAY);
		s_pstSvcCsoSubContextPool.ulPlaying++;

		if(s_pstSvcCsoSubContextPool.ulPlaying > 0)
		{
			/* Cancel Timer. */
			VK_TIMER_Cancel(s_ulSvcCsoSubTimerId);
			/* 300ms ���� �˷� �� Timer�� ��� �Ѵ�. */
			VK_TIMER_EventEvery((HUINT32)SVC_CSO_SUB_SYNC_TIME,
					(void *)local_sub_300ms,
					(void *)NULL, 0,
					&s_ulSvcCsoSubTimerId);
		}
	}
	else
	{
		local_sub_SetStatus(pContext, eSVC_CSO_SUB_STATUS_STOP);
		s_pstSvcCsoSubContextPool.ulPlaying--;

		if(s_pstSvcCsoSubContextPool.ulPlaying == 0)
		{
			/* Cancel Timer. */
			VK_TIMER_Cancel(s_ulSvcCsoSubTimerId);
			s_ulSvcCsoSubTimerId = VK_TIMERID_NULL;
		}
	}

	SVC_CSO_SUB_CRITICAL_SECTION_OUT;

	return ERR_OK;
}

HERROR SVC_CSO_SUB_SetLanguage(SvcCso_t *player, HUINT32 ulLangIdx)
{
	SvcCsoSub_Context_t	*pContext = NULL;

	if(player == NULL)
	{
		HxLOG_Error("Media handel is null.\n");
		return ERR_FAIL;
	}

	SVC_CSO_SUB_CRITICAL_SECTION_IN;
	pContext = (SvcCsoSub_Context_t *)player->pSubContext;

	if(pContext == NULL)
	{
		HxLOG_Error("Subtitle Context is not existed.\n");
		SVC_CSO_SUB_CRITICAL_SECTION_OUT;
		return ERR_FAIL;
	}

	/* Check Status. */
	if ( pContext->status == eSVC_CSO_SUB_STATUS_RELEASE )
	{
		HxLOG_Error("Check Status = (%s)\n", local_sub_GetSVC_CSO_SUBStatusText(pContext->status));
		SVC_CSO_SUB_CRITICAL_SECTION_OUT;

		return ERR_FAIL;
	}

	if (pContext->ulLangIdx != ulLangIdx) {
		pContext->ulLangIdx = ulLangIdx;
		local_sub_ProcessPacket();
	}

	SVC_CSO_SUB_CRITICAL_SECTION_OUT;

	return ERR_OK;
}

HERROR SVC_CSO_SUB_GetType(SvcCso_t * player, SvcCsoSub_Type_e *peFileType)
{
	SvcCsoSub_Context_t	*pContext = NULL;

	if(player == NULL)
	{
		HxLOG_Error("Media handel is null.\n");
		return ERR_FAIL;
	}

	SVC_CSO_SUB_CRITICAL_SECTION_IN;
	pContext = (SvcCsoSub_Context_t *)player->pSubContext;

	if(pContext == NULL)
	{
		HxLOG_Error("Subtitle Context is not existed.\n");
		SVC_CSO_SUB_CRITICAL_SECTION_OUT;
		return ERR_FAIL;
	}

	/* Check Status. */
	if ( pContext->status == eSVC_CSO_SUB_STATUS_RELEASE )
	{
		HxLOG_Error("Check Status = (%s)\n", local_sub_GetSVC_CSO_SUBStatusText(pContext->status));
		SVC_CSO_SUB_CRITICAL_SECTION_OUT;

		return ERR_FAIL;
	}

	*peFileType = pContext->eFileType;

	SVC_CSO_SUB_CRITICAL_SECTION_OUT;

	return ERR_OK;
}

HERROR SVC_CSO_SUB_GetLanguageInfo(SvcCso_t * player, SvcCsoSub_LangInfo_t *pstLanguageInfo)
{
	int i;
	SvcCsoSub_Context_t	*pContext = NULL;

	if(player == NULL)
	{
		HxLOG_Error("Media handel is null.\n");
		return ERR_FAIL;
	}

	SVC_CSO_SUB_CRITICAL_SECTION_IN;

	pContext = (SvcCsoSub_Context_t *)player->pSubContext;

	if(pContext == NULL)
	{
		HxLOG_Error("Subtitle Context is not existed.\n");
		SVC_CSO_SUB_CRITICAL_SECTION_OUT;
		return ERR_FAIL;
	}

	/* Check Status. */
	if ( pContext->status == eSVC_CSO_SUB_STATUS_RELEASE )
	{
		HxLOG_Error("Check Status = (%s)\n", local_sub_GetSVC_CSO_SUBStatusText(pContext->status));
		SVC_CSO_SUB_CRITICAL_SECTION_OUT;

		return ERR_FAIL;
	}

	pstLanguageInfo->ulTotalLang = pContext->ulTotalLang;
	pstLanguageInfo->ulCurIdx    = pContext->ulLangIdx;
	//pstLanguageInfo->eEncoding   = pContext->eEncoding;

	for(i=0; i<pContext->ulTotalLang; i++)
	{
		if(pContext->eFileType == eSVC_CSO_SUB_TYPE_SAMI)
		{
#if DISPLAY_LANG_Hx
			pstLanguageInfo->eDispLang[i] = pContext->eDispLang[i];
#else
			HxSTD_memcpy(pstLanguageInfo->szDispLang[i], pContext->szDispLang[i], sizeof(pContext->szDispLang[i]));
#endif
			HxSTD_memcpy(pstLanguageInfo->szName[i], pContext->stSamiClass[i].szName, SVC_CSO_SUB_LANGUAGE_STRLEN_MAX);
		}
		else
		{
			strncpy(pstLanguageInfo->szDispLang[i], "unknown" , SVC_CSO_SUB_LANGUAGE_STRLEN_MAX-1);
		}
	}

	SVC_CSO_SUB_CRITICAL_SECTION_OUT;

	return ERR_OK;
}

HERROR SVC_CSO_SUB_ChangeSyncOffsetTime(SvcCso_t *player, HINT32 ulMsTime)
{
	SvcCsoSub_Context_t *pContext = NULL;

	if(player == NULL)
	{
		HxLOG_Error("Media handel is null.\n");
		return ERR_FAIL;
	}

	SVC_CSO_SUB_CRITICAL_SECTION_IN;
	pContext = (SvcCsoSub_Context_t *)player->pSubContext;

	if(pContext == NULL)
	{
		HxLOG_Error("Subtitle Context is not existed.\n");
		SVC_CSO_SUB_CRITICAL_SECTION_OUT;
		return ERR_FAIL;
	}

	/* Check Status. */
	if ( pContext->status == eSVC_CSO_SUB_STATUS_RELEASE )
	{
		HxLOG_Error("Check Status = (%s)\n", local_sub_GetSVC_CSO_SUBStatusText(pContext->status));
		SVC_CSO_SUB_CRITICAL_SECTION_OUT;

		return ERR_FAIL;
	}

	if (pContext->msOffsetTime != ulMsTime)
		pContext->msOffsetTime = ulMsTime;


	SVC_CSO_SUB_CRITICAL_SECTION_OUT;

	return ERR_OK;
}

HERROR SVC_CSO_SUB_GetCurSyncOffsetTime(SvcCso_t *player, HINT32 *ulMsTime)
{
	SvcCsoSub_Context_t *pContext = NULL;

	if(player == NULL)
	{
		HxLOG_Error("Media handel is null.\n");
		return ERR_FAIL;
	}

	SVC_CSO_SUB_CRITICAL_SECTION_IN;

	pContext = (SvcCsoSub_Context_t *)player->pSubContext;

	if(pContext == NULL)
	{
		HxLOG_Error("Subtitle Context is not existed.\n");
		SVC_CSO_SUB_CRITICAL_SECTION_OUT;
		return ERR_FAIL;
	}

	/* Check Status. */
	if ( pContext->status == eSVC_CSO_SUB_STATUS_RELEASE )
	{
		HxLOG_Error("Check Status = (%s)\n", local_sub_GetSVC_CSO_SUBStatusText(pContext->status));
		SVC_CSO_SUB_CRITICAL_SECTION_OUT;

		return ERR_FAIL;
	}

	*ulMsTime = pContext->msOffsetTime;

	SVC_CSO_SUB_CRITICAL_SECTION_OUT;

	return ERR_OK;
}

HERROR SVC_CSO_SUB_GetDisplay(SvcCso_t *player, HBOOL *bEanble)
{
	SvcCsoSub_Context_t	*pContext = NULL;

	if(player == NULL)
	{
		HxLOG_Error("Media handel is null.\n");
		return ERR_FAIL;
	}

	pContext = (SvcCsoSub_Context_t *)player->pSubContext;

	SVC_CSO_SUB_CRITICAL_SECTION_IN;

	if(pContext == NULL )
	{
		HxLOG_Error("Media handel is null or Subtitle Context is not existed.\n");
		SVC_CSO_SUB_CRITICAL_SECTION_OUT;
		return ERR_FAIL;
	}

	if(pContext->status == eSVC_CSO_SUB_STATUS_PLAY)
		*bEanble = TRUE;
	else
		*bEanble = FALSE;

	SVC_CSO_SUB_CRITICAL_SECTION_OUT;

	return ERR_OK;
}
