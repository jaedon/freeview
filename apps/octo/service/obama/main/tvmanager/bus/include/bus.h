/****************************************************************************
*
* File Name	: bus.h
*
* Description	: BUS 마스터 include 파일.
*				  Application 및 각종 Service 에서는 bus.h을 include 해야만 한다.
*
*	* DO NOT MODIFY ! 	 DO NOT MODIFY ! 	 DO NOT MODIFY ! 	 DO NOT MODIFY ! *
*
* programed by Ahnsik, Choi.
*
* Revision History	:
*
* Date			Modification						Name
* -----------	----------------------------------  -------------------------
* 2004/11/05	File Created.						AhnSik, Choi.
* 2008/01/14	revised								JK Baek
* 2011/01/29	gwm간소화작업, graphic기능제거		누군가
*
****************************************************************************/

#ifndef	__BUS_H__
#define	__BUS_H__

/* ======================================================================== */
/* Include.					기본적인 System Include 						*/
/* ======================================================================== */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <htype.h>
#include <octo_common.h>

/* ======================================================================== */
/* Exported Macros. 														*/
/* ======================================================================== */
#define BUS_DEFAULT_PRIORITY			100
#define BUS_MAX_PRIORITY				0xFF			// 255
#define BUS_MASK_PRIORITY				0x000000FF

/* ------------------------------------------------------------------------ */
/*	Enumulation.															*/
/*			외부에 별도의 파일을 만들어 bus.h의 직접적인 수정을 방지한다.	*/
/* ------------------------------------------------------------------------ */

/* ************************************************************************ */
typedef	enum
{
	ERR_BUS_NO_ERROR		= ERR_OK	,
	ERR_BUS_INITIAL_FAILED	= 1,
	ERR_BUS_NO_OBJECT,
	ERR_BUS_LINK_CONFLICT,
	ERR_BUS_OUT_OF_RANGE,
	ERR_BUS_OUT_OF_MEMORY,
	ERR_BUS_MESSAGE_BREAK,
	ERR_BUS_MESSAGE_PASS,
	// 여기까지는 기본적으로 필요한 것들.

	// argument 관련 error들
	ERR_BUS_SOURCE_NULL,
	ERR_BUS_TARGET_NULL,

	ERR_BUS_ACTION_FAILED,
	// Last Item in the GWM Error Code
	ERR_BUS_LAST
} BUS_Result_t;

#define MESSAGE_BREAK		ERR_BUS_MESSAGE_BREAK
#define MESSAGE_PASS		ERR_BUS_MESSAGE_PASS

typedef	enum
{
	eDESTROY_BUS_MYSELF,
	eDESTROY_BUS_ACTION,
	eDESTROY_BUS_END
}BUS_DestroyReason_t;


typedef	enum
{
	eMEVT_BUS_CREATE    = eMEVT_BUS_START,
	eMEVT_BUS_DESTROY,

	eMEVT_BUS_SIGNAL,						//	for synchronizing external signal.

	eMEVT_BUS_MGR_STANDBY_INIT,


	/* Parameter Usage:
		p1 : callback to create
		p2 : parent callback function	*/
	eMEVT_BUS_MGR_ALREADY_EXIST,			// 이미 존재하는 Manager를 또 Create 하려 했을 때 보내는 메시지

	/* Parameter Usage:
		p1 : spawned callback function
		p2 : parent callback function	*/
	eMEVT_BUS_REQUEST_CREATE,				// 타 Task에 의해 APPLICATION을 Create 한다. 절대 Manager 내에서 따로 쓰면 안된다.

	/* Parameter Usage:
		p1 : spawned callback function
		p2 : parent callback function	*/
	eMEVT_BUS_MGR_SPAWNED,				// Application이 Create 되었음을 다른 모든 App에 알리는 메시지

	/* Parameter Usage:
		p1 : callback function */
	eMEVT_BUS_MGR_KILLED,					// Application이 Kill 되었음을 다른 모든 App에 알리는 메시지

	/* Parameter Usage:
		p1 : Timer ID
		p2 : N.A.
		p3 : N.A. */
	eMEVT_BUS_TIMER,

	/* Parameter Usage:
		handle : N.A.
		p1 : TRUE: Block shutdown process and stay calm.
		p2 : KEY_Device_t. RCU or Front key.
		p3 : TBD */
	eMEVT_BUS_READY_SHUTDOWN,					// Application들에게 shutdown로 간다고 알림. If application doesn't block this message, BUS_ProcessMessageDefault() will destroy 'this' proc.
	eMEVT_BUS_GO_SHUTDOWN,					// Application들에게 shutdown로 간다고 알림. 만약 할일 이 존재 한다면 PAPI_WaitShutdown을 요청해야 함. 주기적으로 shutdown 될때까지 계속 보냄.
	eMEVT_BUS_SHUTDOWN_DONE,				// Application들에게 shutdown이 완료 되어짐을 알림.
	eMEVT_BUS_REBOOT_DONE,					// Application들에게 reboot이 완료 되어짐을 알림.
	eMEVT_BUS_GO_OPERATION, 				// Application들에게 operation 상태로 진입 함을 알림.
	eMEVT_BUS_START_BATCH,					// Batch proc을 실행 시키도록 명령. 현재는 mgr_action에서 reday_shutdown을 받아 실행. 추후 webapp에서 처리하도록 준비중.


// TODO: module 로 이동해야함.
/* - Description :
	   - Parameters Usage :
		   handle : N.A
		   p1 :  0
		   p2 :  0
		   p3 :  0 */
	eMEVT_BUS_IRDETO_UPDATE_UI,

	/* - Description :
		   - Parameters Usage :
			   handle : N.A
			   p1 :  0
			   p2 :  0
			   p3 :  0 */
	eMEVT_BUS_IRDETO_HIDE_UI,

	eMEVT_BUS_RPC_READY,

	eMEVT_BUS_END
} BUS_Message_t;


typedef	struct	tagManagerObject	*BUS_Mgr_t;

/* GWM application procedure */
typedef BUS_Result_t	(*BUS_Callback_t)(HINT32 message, Handle_t handle,
										HINT32 param1, HINT32 param2, HINT32 param3);

/* param1,2,3 에 alloc된 값이 들어올 경우 이 callback 함수를 통해 gwm 수행후 free 한다. */
typedef void (*BUS_FreeCallback_t)(void *ptr);

/* ************************************************************************ */

// wrapper 를 두는 이유는 추후에 (x)mgr -> (x)om 으로 보내는 것들을 분리하기 쉽도록. 띄어내야 할 수도 있으므로.
#define BUS_PostMessageToOM(a,b,c,d,e,f) BUS_PostMessage(a,b,c,d,e,f)
#define BUS_SendMessageToOM(a,b,c,d,e,f) BUS_SendMessage(a,b,c,d,e,f)
#define BUS_PostDataToOM(a,b,c,d,e,f,g) BUS_PostData(a,b,c,d,e,f,g)
/* ------------------------------------------------------------ */
/*	Functions .													*/
/* ------------------------------------------------------------ */

/* Use when the service send a message to application(s) */
extern BUS_Result_t		BUS_PostMessage(BUS_Callback_t appCallback, HINT32 message,
										Handle_t hAct, HINT32 param1, HINT32 param2, HINT32 param3);

/* Use when need message exchanges between applications */
extern BUS_Result_t		BUS_SendMessage(BUS_Callback_t appCallback, HINT32 message,
										Handle_t hAct, HINT32 param1, HINT32 param2, HINT32 param3);

/* Use when the sevice send a message with data to application(s) */
extern BUS_Result_t 	BUS_PostData(BUS_Callback_t appCallback, HINT32 message,
										Handle_t hAct, HINT32 param1, HINT32 param2, void *pData, HUINT32 ulSize);

/* Use when need message exchanges between applications with data */
extern BUS_Result_t		BUS_SendData(BUS_Callback_t appCallback, HINT32 message,
										Handle_t hAct, HINT32 param1, HINT32 param2, void *pData, HUINT32 ulSize);

/* default application procedure */
extern BUS_Result_t 	BUS_ProcessMessageDefault(HINT32 message,
										Handle_t hAct, HINT32 param1, HINT32 param2, HINT32 param3);
// Application이 호출하여 사용할 기능.

extern BUS_Mgr_t		BUS_MGR_Get(BUS_Callback_t callback);
extern BUS_Mgr_t		BUS_MGR_GetThis(void);
extern BUS_Callback_t	BUS_MGR_GetParentCallback(void);
extern BUS_Mgr_t		BUS_MGR_GetMgrTop(void);
extern BUS_Callback_t	BUS_MGR_GetMgrCallback(BUS_Mgr_t app);
extern HINT8 * 			BUS_MGR_GetMgrName(BUS_Mgr_t app);
extern void				BUS_MGR_SetMgrName(BUS_Mgr_t app, HINT8 *pszProcName);

extern BUS_Result_t		BUS_MGR_Create(HINT8 *szAppName, HUINT32 property, BUS_Callback_t callback,
											HINT32 handle, HINT32 param1, HINT32 param2, HINT32 param3);
/*
	BUS_MGR_RequestToCreate 은 GWM 안에서 불리면 절대 안 되는 함수
*/
extern BUS_Result_t		BUS_MGR_RequestToCreate (HINT8 *szAppName, HUINT32 ulProperty, BUS_Callback_t fnAppProc,
									HINT32 handle, HINT32 param1, HINT32 param2, HINT32 param3, HBOOL bSync);

#define BUS_MGR_Destroy(callback)							__BUS_MGR_Destroy(callback, __FUNCTION__, __LINE__)
#define BUS_MGR_DestroyWithReason(callback, reason, proc)	__BUS_MGR_DestroyWithReason(callback, reason, proc, __FUNCTION__, __LINE__)

BUS_Result_t			__BUS_MGR_Destroy(BUS_Callback_t fnAppProc, const char *szFunction, HUINT32 ulLine);
extern BUS_Result_t		__BUS_MGR_DestroyWithReason(BUS_Callback_t fnAppProc, BUS_DestroyReason_t eDestroyReason, HINT32 procReason, const char *szFunction, HUINT32 ulLine);
extern BUS_Result_t		BUS_MGR_DestroyAll(void);


/**
  @remark	set a timer on current application, usually expires every the given laps(seconds) passed. \n
  @remark	API Type : INLINE \n
  @remark	param : ulTimerId - timer instance id, unique on every application scope.	\n
  					ulPeriod_msec - period time in msec, 1/10 seconds resolution.
  @return		error code
*/
extern BUS_Result_t		BUS_SetTimer(HUINT32 ulTimerId, HUINT32 ulPeriod_msec);

/**
  @remark	destroy the timer instance. \n
  @remark	API Type : INLINE \n
  @remark	param : 	ID - timer instance id to be destroyed	\n
  @return		error code
*/
extern BUS_Result_t		BUS_KillTimer(HUINT32 ulTimerId);

/**
  @remark	reset timer's expiration cycle. \n
  @remark	API Type : INLINE \n
  @remark	param : 	ID - timer instance id to be reset	\n
  @return		error code
*/
extern BUS_Result_t		BUS_ResetTimer(HUINT32 ulTimerId);

extern void 			BUS_Init(void);
extern void				BUS_Start(BUS_Callback_t pfnAppIgnitor);

//	Send RPC Ready Message to OBAMA bus
extern void				BUS_PostRPCReady(void);

extern HINT8*			BUS_DBG_GetMsgName(HINT32 message);

// Command
extern	HINT32			BUS_DBG_DumpMgrList(void *arg);
extern  HINT32			BUS_DBG_ShowLastMgr(void *arg);
extern	HINT32			BUS_DBG_ShowTimer (void *arg);

extern	HERROR			BUS_DBG_RegisterMsgBreakTracer (HUINT32 ulMsgId, HBOOL bTraceAll);
extern	HERROR			BUS_DBG_UnregisterMsgBreakTracer (HUINT32 ulDelIndex);
extern	HERROR			BUS_DBG_FindMsgBreakTracer (HUINT32 ulMsgValue, HBOOL *pbTraceAll);
extern	HERROR			BUS_DBG_DumpMsgBreakTracer (void);
extern 	HERROR 			BUS_DBG_SetDestroyLog (HBOOL bDisplay);

#endif	/* __BUS_H__ */

