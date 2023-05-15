#ifndef __XMGR_NTLS_BASE_H__
#define __XMGR_NTLS_BASE_H__

typedef enum
{
	eNtlsState_IDLE			= 0,
	eNtlsState_TUNE,
	eNtlsState_CHECK_PMT,
	eNtlsState_NO_SIGNAL,
	eNtlsState_WAIT_DRM,

	eNtlsState_CAS_BLOCKED,
	eNtlsState_CAS_DRM_BLOCKED,
	eNtlsState_RECORDING,
	eNtlsState_STOPPED,
} xMgrNtlsState_e;

typedef struct
{

	Handle_t			hAction;
	HUINT32				ulActionId;

	MgrNtls_Setup_t		stSetup;

	xMgrNtlsState_e		eState;
	BUS_Callback_t		pfActionProc;

	SvcRec_PIDList_t	stRecPidInfo;

}xMgrNtlsContext_t;


BUS_Result_t	XMGR_NTLS_MsgBusCreate_Base (xMgrNtlsContext_t  *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	XMGR_NTLS_MsgBusDestroy_Base (xMgrNtlsContext_t  *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	XMGR_NTLS_MsgBusTimer_Base (xMgrNtlsContext_t  *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t  	XMGR_NTLS_MsgBusGoStandby_Base (xMgrNtlsContext_t  *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	XMGR_NTLS_MsgEvtChLocked_Base (xMgrNtlsContext_t  *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	XMGR_NTLS_MsgEvtChNoSignal_Base( xMgrNtlsContext_t  *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	XMGR_NTLS_MsgEvtChMovingMotor_Base( xMgrNtlsContext_t  *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	XMGR_NTLS_MsgEvtRecTimecodeChange_Base( xMgrNtlsContext_t  *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	XMGR_NTLS_MsgEvtSiPmtReceived_Base( xMgrNtlsContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	XMGR_NTLS_MsgEvtSiPmtTimeout_Base( xMgrNtlsContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	XMGR_NTLS_MsgEvtSiPmtPidChanged_Base( xMgrNtlsContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	XMGR_NTLS_MsgEvtSiEitReceived_Base( xMgrNtlsContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	XMGR_NTLS_MsgEvtSiEitTimeout_Base( xMgrNtlsContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	XMGR_NTLS_MsgEvtSiMultifeedDetected_Base( xMgrNtlsContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	XMGR_NTLS_MsgEvtSiSdtFreeCaModeChanged_Base( xMgrNtlsContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	XMGR_NTLS_MsgEvtSiSdtRunningStatusChanged_Base( xMgrNtlsContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);


HERROR			xmgr_ntls_Init_BASE (void);
HERROR			xmgr_ntls_Start_BASE (MgrNtls_Setup_t *pstSetup, HUINT32 *pulActionId);
HERROR			xmgr_ntls_ChangePids_BASE (HUINT32 ulActionId, HINT32 nPidsCount, HUINT16 *pusPids);
HERROR			xmgr_ntls_Stop_BASE (HUINT32 ulActionId);

#endif /* __XMGR_NTLS_BASE_H__ */
