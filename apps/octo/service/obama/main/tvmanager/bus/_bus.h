/****************************************************************************
*
* File Name	: bus_internal.h
*
* Description	: BUS internal
*
*
* Revision History	:
*
* Date			Modification				Name
* -----------	-------------------------------------	------------------
* 2008/01/09		initial revision				JK Baek
* 2011/01/29		internal header로 모음, 정리	누군가
*
****************************************************************************/

#ifndef	__BUS_INTERNAL_H__
#define	__BUS_INTERNAL_H__

/* ======================================================================== */
/* Include.																	*/
/* ======================================================================== */

/* ======================================================================== */
/* Exported Macros. 														*/
/* ======================================================================== */

//////////////////////////////////////////////////////
// 그 외의 BUS 의 설정 (from old gwm_config.h)
//////////////////////////////////////////////////////

#define	MGR_NAME_LENGTH				32

#define	BUS_TIMER_SLEEP_TIME		100			// Timer task의 sleep 간격.
#define	BUS_MAX_TIMER				64			/* eMEVT_BUS_TIMER 의 total 갯수.			(16~256 권장.)*/
#define	BUS_MAX_MANAGER				92			/* GWM_Application의 total 갯수.*/



/* ------------------------------------------------------------ */
/*	Structure type definition.									*/
/* ------------------------------------------------------------ */
typedef	struct	tagMessageType
{
	BUS_Callback_t		appCallback;
	BUS_Message_t		message;
	Handle_t			handle;
	HINT32				param1;
	HINT32				param2;
	HINT32				param3;

	BUS_FreeCallback_t	fnFreeParam;

} BUS_InternalMsg_t;

typedef struct tagLinkObject
{
	struct tagLinkObject	*nextLink;
} *LINK_OBJECT;

typedef struct tagManagerObject
{
	// Application Object에 앞으로 항목을 추가하려면 반드시 밑에 추가하도록 한다.
	// 윗줄의 link object는 반드시 맨 위에 존재해야 하는 항목이다.
	LINK_OBJECT			link;						/**< 링크트리스트 관리 */ /* 리스트 용도로 사용 중이니 지우지 말것... */

	HINT8				szName[MGR_NAME_LENGTH];	/**<  RTTI 문자열. */
	HINT32				nPriority;
	BUS_Callback_t		fnMgrProc, fnParentMgrProc;


#if 0
	HBOOL				bActive;					/* alarm activity */
	HUINT32				ulLap;						/* alarming interval : tick-unit */
	HUINT32				ulTimeout;					/* alarm expiration */
	void 				(*fnAlarmCallback)(void *);	/* executing callback */
#endif
	HBOOL				bAllocated;
} BUS_MgrObj_t;

typedef struct tagMgrCreateRequest
{
	BUS_MgrObj_t		 stMgrStatus;

	// Options
	Handle_t			 hHandle;
	HINT32				 p1, p2, p3;
} BUS_MgrReq_t;


/* ------------------------------------------------------------ */
/*	Global variables for GWM.								    */
/* ------------------------------------------------------------ */
extern BUS_Mgr_t	g_stMgrRoot, g_stCurrentMgr;


/* ------------------------------------------------------------ */
/*	Functions .			 									    */
/* ------------------------------------------------------------ */
/* gwm_main */

/* gwm_timer */
extern void 		bus_timer_Init(void);
extern HERROR		bus_timer_KillAll(BUS_Mgr_t app);
extern HINT32		bus_timer_CheckTimerOut (BUS_Callback_t *ppfAppCallback, HUINT32 *pulTimerId);
#if defined(CONFIG_DEBUG)
extern HERROR		bus_timer_DumpAllList (void *arg);
#endif

/* gwm_appobj */
extern void 		bus_mgr_Init(void);
extern BUS_Mgr_t	bus_mgr_GetNext(BUS_Mgr_t app);
extern BUS_Result_t	bus_mgr_Call(BUS_Mgr_t app, HINT32 message, Handle_t handle, HINT32 param1, HINT32 param2, HINT32 param3 );
extern void			bus_mgr_GetReqMgrSem (void);
extern void			bus_mgr_ReleaseReqMgrSem (void);
extern BUS_Result_t	bus_mgr_CreateByRequest (BUS_MgrReq_t *pstReq);

extern	LINK_OBJECT		bus_lnklist_GetNext(LINK_OBJECT item);
extern	BUS_Result_t	bus_lnklist_AddItemWithCompareDescend(LINK_OBJECT *root, LINK_OBJECT new_item, int compare_func(LINK_OBJECT, LINK_OBJECT) );
extern	BUS_Result_t	bus_lnklist_RemoveItemWithCompare(LINK_OBJECT *root, LINK_OBJECT item, int compare_func(LINK_OBJECT, LINK_OBJECT) );


#endif	/* __BUS_INTERNAL_H__ */

