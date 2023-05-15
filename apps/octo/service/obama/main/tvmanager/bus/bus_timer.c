/****************************************************************************
*
* File Name	: gwm_timer.c
*
* Description	: GWM timer task
*
*
* Revision History	:
*
* Date			Modification				Name
* -----------	-------------------------------------	------------------
* 2008/01/09		initial revision		JK Baek
* 2008/02/25		embeding alarm in GWM	JK Baek
* 2011/01/28		코드 분리 from gwm_main	누군가
*
****************************************************************************/

/* ======================================================================== */
/* Include. 																*/
/* ======================================================================== */
#include <string.h>

#include <octo_common.h>
//#include <mwc_util.h>
#include <svc_sys.h>

#include <bus.h>

#include "_bus.h"

/* ======================================================================== */
/* Global/Extern Variables.													*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Macros. 															*/
/* ======================================================================== */


/* ======================================================================== */
/* Private Types.															*/
/* ======================================================================== */
typedef struct tagTimerObject
{
	BUS_Callback_t		appCallback;	/* 타이머를 설정한 Application callback */
	HUINT32				ulTimerID;		/* 설정시의 Timer_ID */
	HUINT32				ulStartTick;	/* Session start tick */
	HUINT32				ulTickPeriod;	/* 설정된 값. */
} BUS_TimerObj_t;

/* ======================================================================== */
/* Global Variables. 														*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Constants. 														*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Variables. 														*/
/* ======================================================================== */
STATIC	HxList_t		*s_pstTimerList;

/* ======================================================================== */
/* Private Function prototypes. 											*/
/* ======================================================================== */
STATIC void		bus_timer_Free(void *p)
{
	HLIB_MEM_Free(p);
}

STATIC HBOOL	bus_timer_CompCallback(void *userdata, void *data)
{
	BUS_Mgr_t	app = (BUS_Mgr_t)userdata;
	BUS_TimerObj_t	*timerObj = (BUS_TimerObj_t*)data;

	if (timerObj->appCallback == BUS_MGR_GetMgrCallback(app))
		return TRUE;

	return FALSE;
}

STATIC HINT32	bus_timer_CompStartTick(void *userdata, void *data)
{
	BUS_TimerObj_t	*userTimer = (BUS_TimerObj_t*)userdata;
	BUS_TimerObj_t	*dataTimer = (BUS_TimerObj_t*)data;

	return (HINT32)((dataTimer->ulStartTick + dataTimer->ulTickPeriod) - (userTimer->ulStartTick + userTimer->ulTickPeriod));
}

STATIC HBOOL	bus_timer_CompIdCallback(void *userdata, void *data)
{
	BUS_TimerObj_t	*userTimer = (BUS_TimerObj_t*)userdata;
	BUS_TimerObj_t	*dataTimer = (BUS_TimerObj_t*)data;

	if ((userTimer->ulTimerID == dataTimer->ulTimerID) && (userTimer->appCallback == dataTimer->appCallback))
		return TRUE;

	return FALSE;
}

/* ======================================================================== */
/* Exported Functions. 														*/
/* ======================================================================== */

void bus_timer_Init (void)
{
	/* Timer List 를 초기화 한다. */
	s_pstTimerList = NULL;
//	HxSTD_memset (s_astTimerList, 0, sizeof(BUS_TimerObj_t) * BUS_MAX_TIMER);
}

HERROR bus_timer_KillAll(BUS_Mgr_t app)
{
	s_pstTimerList = HLIB_LIST_RemoveIf(s_pstTimerList, (void*)app, bus_timer_CompCallback, bus_timer_Free);
#if 0
	HINT32		 nCount;

	for (nCount = 0; nCount < BUS_MAX_TIMER; nCount++)
	{
		if (s_astTimerList[nCount].appCallback == BUS_MGR_GetMgrCallback(app))
		{
			s_astTimerList[nCount].appCallback = NULL;
			s_astTimerList[nCount].ulTimerID = 0;
		}
	}
#endif

	return ERR_OK;
}

HINT32	bus_timer_CheckTimerOut (BUS_Callback_t *ppfAppCallback, HUINT32 *pulTimerId)
{
	HUINT32			ulCurrTick;
	BUS_TimerObj_t	*timerObj;

	ulCurrTick = HLIB_STD_GetSystemTick();
	timerObj = (BUS_TimerObj_t*)HLIB_LIST_Data(s_pstTimerList);
	if (timerObj)
	{
		HUINT32		ulTargetTick = timerObj->ulStartTick + timerObj->ulTickPeriod;

		if (ulTargetTick <= ulCurrTick)
		{
			*ppfAppCallback = timerObj->appCallback;
			*pulTimerId     = timerObj->ulTimerID;

			s_pstTimerList = HLIB_LIST_Remove(s_pstTimerList, timerObj);
			timerObj->ulStartTick = ulCurrTick;
			s_pstTimerList = HLIB_LIST_AppendSortEx(s_pstTimerList, timerObj, bus_timer_CompStartTick);

			return 0;
		}

		//	always plus value
		return ulTargetTick - ulCurrTick;
	}

	return -1;
#if 0
	unsigned long		 ulCurrTick, ulTickDiff;
	HINT32				 nCount;

	for (nCount = 0; nCount < BUS_MAX_TIMER; nCount++)
	{
		if (s_astTimerList[nCount].appCallback == NULL)
		{
			continue;
		}

		ulCurrTick = VK_TIMER_GetSystemTick();

		if (s_astTimerList[nCount].ulStartTick <= ulCurrTick)
		{
			ulTickDiff = ulCurrTick - s_astTimerList[nCount].ulStartTick;
		}
		else
		{
			ulTickDiff = 0xffffffff - s_astTimerList[nCount].ulStartTick + ulCurrTick;
		}

		if(ulTickDiff >= s_astTimerList[nCount].ulTickPeriod)
		{
			*ppfAppCallback	= s_astTimerList[nCount].appCallback;
			*pulTimerId		= s_astTimerList[nCount].ulTimerID;
			s_astTimerList[nCount].ulStartTick = ulCurrTick;

			return ERR_OK;
		}
	}

	return ERR_FAIL;
#endif
}

#if defined(CONFIG_DEBUG)
HERROR bus_timer_DumpAllList (void *arg)
{
	HxList_t	*list;
	BUS_TimerObj_t	*timerObj;

	list = s_pstTimerList;
	while (list)
	{
		timerObj = (BUS_TimerObj_t*)HLIB_LIST_Data(list);

		HxLOG_Warning("Owner:%s[0x%X]\tID:%d[0x%x]\tPeriod:%d\tStart tick:%d\tremain tick:%d\n",
				BUS_MGR_GetMgrName(BUS_MGR_Get(timerObj->appCallback)),
				(HUINT32)timerObj->appCallback,	/* %x 는 HUINT32이다. 주소를 찍을려면 explicit type-casting */
				timerObj->ulTimerID,
				timerObj->ulTimerID,
				timerObj->ulTickPeriod,
				timerObj->ulStartTick,
				(timerObj->ulStartTick + timerObj->ulTickPeriod - HLIB_STD_GetSystemTick())
				);

		list = HLIB_LIST_Next(list);
	}
#if 0
	HINT32			i;

	(void)arg;

	for (i = 0; i < BUS_MAX_TIMER; i++)
	{
		if (s_astTimerList[i].appCallback != NULL)		// 존재하는 타이머라면,
		{
			HxLOG_Print("Owner:%s[0x%X]\tID:%d[0x%x]\tPeriod:%d\tStart tick:%d\tDiff:%lu\n",
					BUS_MGR_GetMgrName(BUS_MGR_Get(s_astTimerList[i].appCallback)),
					(HUINT32)s_astTimerList[i].appCallback,	/* %x 는 HUINT32이다. 주소를 찍을려면 explicit type-casting */
					s_astTimerList[i].ulTimerID,
					s_astTimerList[i].ulTimerID,
					s_astTimerList[i].ulTickPeriod,
					s_astTimerList[i].ulStartTick,
					(VK_TIMER_GetSystemTick()-s_astTimerList[i].ulStartTick)
					);
		}
	}
#endif

	return ERR_OK;
}
#endif

BUS_Result_t BUS_SetTimer (HUINT32 ulTimerID, HUINT32 ulPeriod_msec)
{
	HxList_t	*list;
	BUS_TimerObj_t	*timerInfo;

	list = s_pstTimerList;
	while (list)
	{
		timerInfo = (BUS_TimerObj_t*)HLIB_LIST_Data(list);
		if ((timerInfo->ulTimerID == ulTimerID) && (timerInfo->appCallback == BUS_MGR_GetMgrCallback(g_stCurrentMgr)))
		{
			HxLOG_Warning("Same ID(%d) is already exist!! %s\n",
						ulTimerID, BUS_MGR_GetMgrName(g_stCurrentMgr));
			return ERR_BUS_LINK_CONFLICT;
		}

		list = HLIB_LIST_Next(list);
	}

	timerInfo = (BUS_TimerObj_t*)HLIB_MEM_Calloc(sizeof(BUS_TimerObj_t));
	timerInfo->appCallback = BUS_MGR_GetMgrCallback(g_stCurrentMgr);
	timerInfo->ulTimerID = ulTimerID;
	timerInfo->ulStartTick = HLIB_STD_GetSystemTick();
	timerInfo->ulTickPeriod = (ulPeriod_msec < BUS_TIMER_SLEEP_TIME) ? BUS_TIMER_SLEEP_TIME : ulPeriod_msec;
	s_pstTimerList = HLIB_LIST_AppendSortEx(s_pstTimerList, (void*)timerInfo, bus_timer_CompStartTick);

	BUS_PostRPCReady();
//	bus_timer_DumpAllList(NULL);
#if 0
	HINT32		i;

	for (i = 0; i < BUS_MAX_TIMER; i++)
	{
		if ((s_astTimerList[i].ulTimerID == ulTimerID) && (s_astTimerList[i].appCallback == BUS_MGR_GetMgrCallback(g_stCurrentMgr)))
		{
			HxLOG_Error("Same ID(%d) is already exist!! %s\n",
							ulTimerID, BUS_MGR_GetMgrName(g_stCurrentMgr));
			return ERR_BUS_LINK_CONFLICT;
		}
	}

	for (i = 0; i < BUS_MAX_TIMER; i++)	/* 비어 있는 타이머 슬롯을 찾는다.*/
	{
		if (s_astTimerList[i].appCallback == NULL)
		{
			break;
		}
	}

	if (i >= BUS_MAX_TIMER)			/* 더이상 설정할 타이머가 없음.*/
	{
		return ERR_BUS_INITIAL_FAILED;
	}

	s_astTimerList[i].appCallback = BUS_MGR_GetMgrCallback(g_stCurrentMgr);
	s_astTimerList[i].ulTimerID = ulTimerID;
	s_astTimerList[i].ulStartTick = VK_TIMER_GetSystemTick();
	s_astTimerList[i].ulTickPeriod = (ulPeriod_msec < BUS_TIMER_SLEEP_TIME) ? BUS_TIMER_SLEEP_TIME : ulPeriod_msec;
#endif

	return ERR_BUS_NO_ERROR;
}


BUS_Result_t BUS_KillTimer (HUINT32 ulTimerID)
{
	BUS_TimerObj_t	stTimer;

	stTimer.ulTimerID = ulTimerID;
	stTimer.appCallback = BUS_MGR_GetMgrCallback(g_stCurrentMgr);
	s_pstTimerList = HLIB_LIST_RemoveIf(s_pstTimerList, (void*)&stTimer, bus_timer_CompIdCallback, bus_timer_Free);

#if 0
	HINT32		i;

	for (i = 0; i < BUS_MAX_TIMER; i++)
	{
		if ((s_astTimerList[i].ulTimerID == ulTimerID) && (s_astTimerList[i].appCallback == BUS_MGR_GetMgrCallback(g_stCurrentMgr)))
		{
			s_astTimerList[i].appCallback = NULL;
			s_astTimerList[i].ulTimerID = 0;
		}
	}
#endif
	return ERR_BUS_NO_ERROR;
}

BUS_Result_t BUS_ResetTimer (HUINT32 ulTimerID)
{
	HxList_t	*list;
	BUS_TimerObj_t	stTimer, *timerObj;

	stTimer.ulTimerID = ulTimerID;
	stTimer.appCallback = BUS_MGR_GetMgrCallback(g_stCurrentMgr);
	list = HLIB_LIST_FindEx(s_pstTimerList, (const void*)&stTimer, bus_timer_CompIdCallback);

	timerObj = (BUS_TimerObj_t*)HLIB_LIST_Data(list);
	if (timerObj)
	{
		s_pstTimerList = HLIB_LIST_Remove(s_pstTimerList, timerObj);
		timerObj->ulStartTick = HLIB_STD_GetSystemTick();
		s_pstTimerList = HLIB_LIST_AppendSortEx(s_pstTimerList, timerObj, bus_timer_CompStartTick);
	}

	BUS_PostRPCReady();
#if 0
	HINT32		i;

	for (i = 0; i < BUS_MAX_TIMER; i++)
	{
		if ((s_astTimerList[i].ulTimerID == ulTimerID) && (s_astTimerList[i].appCallback == BUS_MGR_GetMgrCallback(g_stCurrentMgr)))
		{
			s_astTimerList[i].ulStartTick = VK_TIMER_GetSystemTick();
			break;
		}
	}
#endif

//	bus_timer_DumpAllList(NULL);
	return ERR_BUS_NO_ERROR;
}


HINT32	BUS_DBG_ShowTimer (void *arg)
{
	HxList_t	*list;
	BUS_TimerObj_t	*timerObj;

	list = s_pstTimerList;
	while (list)
	{
		timerObj = (BUS_TimerObj_t*)HLIB_LIST_Data(list);

		HLIB_CMD_Printf("Owner:%s[0x%X]\tID:%d[0x%x]\tPeriod:%d\tStart tick:%d\tremain tick:%d\n",
				BUS_MGR_GetMgrName(BUS_MGR_Get(timerObj->appCallback)),
				(HUINT32)timerObj->appCallback, /* %x 는 HUINT32이다. 주소를 찍을려면 explicit type-casting */
				timerObj->ulTimerID,
				timerObj->ulTimerID,
				timerObj->ulTickPeriod,
				timerObj->ulStartTick,
				(timerObj->ulStartTick + timerObj->ulTickPeriod - HLIB_STD_GetSystemTick())
				);

		list = HLIB_LIST_Next(list);
	}

	return ERR_OK;
}



/* end of file */
