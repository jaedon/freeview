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
* 2011/01/29		internal header�� ����, ����	������
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
// �� ���� BUS �� ���� (from old gwm_config.h)
//////////////////////////////////////////////////////

#define	MGR_NAME_LENGTH				32

#define	BUS_TIMER_SLEEP_TIME		100			// Timer task�� sleep ����.
#define	BUS_MAX_TIMER				64			/* eMEVT_BUS_TIMER �� total ����.			(16~256 ����.)*/
#define	BUS_MAX_MANAGER				92			/* GWM_Application�� total ����.*/



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
	// Application Object�� ������ �׸��� �߰��Ϸ��� �ݵ�� �ؿ� �߰��ϵ��� �Ѵ�.
	// ������ link object�� �ݵ�� �� ���� �����ؾ� �ϴ� �׸��̴�.
	LINK_OBJECT			link;						/**< ��ũƮ����Ʈ ���� */ /* ����Ʈ �뵵�� ��� ���̴� ������ ����... */

	HINT8				szName[MGR_NAME_LENGTH];	/**<  RTTI ���ڿ�. */
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

