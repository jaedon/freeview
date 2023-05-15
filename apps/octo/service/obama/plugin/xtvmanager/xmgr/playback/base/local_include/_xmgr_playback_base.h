#ifndef	__XMGR_PLAYBACK_BASE_H__
#define	__XMGR_PLAYBACK_BASE_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <svc_si.h>
#include <mgr_playback.h>
#include <_xmgr_playback.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

#define _________INTERNAL_FUNC__________
/*	xmgr_playback_base.c
 */
xmgrPbContext_t	*xmgr_playback_GetContext_Base (HUINT32 ulActionId);
xmgrPbContext_t	*xmgr_playback_GetContextFromIndex_Base (HUINT32 ulPbIndex);

void		 xmgr_playback_SetAppState_Base(xmgrPbContext_t *pstContext, xmgrPb_State_e appState);
HUINT32		 xmgr_playback_GetMarginedPmtTime (HINT32 nPlaySpeed, HUINT32 ulPlayTime);
HERROR		 xmgr_playback_GetRefTime(SvcMeta_TimestampList_t *pstList, SvcMeta_TimeStampType_e eCheckType, HUINT32 *pulRefTime);
HERROR		 xmgr_playback_CheckTimeStamp_Base (xmgrPbContext_t *pstContext, HUINT32 ulSourceTime);
HERROR		 xmgr_playback_RefreshSvcMeta_Base (xmgrPbContext_t *pstContext, HUINT32 ulPlayTime);

BUS_Result_t xmgr_playback_MsgBusCreate_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
BUS_Result_t xmgr_playback_MsgBusDestroy_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
BUS_Result_t xmgr_playback_MsgBusTimer_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
BUS_Result_t xmgr_playback_MsgAmActionStopped_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
BUS_Result_t xmgr_playback_MsgAppRecordStop_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
BUS_Result_t xmgr_playback_MsgAppDbPidChanged_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
BUS_Result_t xmgr_playback_MsgAppPinOK_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
BUS_Result_t xmgr_playback_MsgAppPinDenied_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
BUS_Result_t xmgr_playback_MsgAppCheckRating_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
#if 0
BUS_Result_t xmgr_playback_MsgAppStartSubtitle_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
BUS_Result_t xmgr_playback_MsgAppStopSubtitle_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
BUS_Result_t xmgr_playback_MsgAppTeletextStarted_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
BUS_Result_t xmgr_playback_MsgAppTeletextStop_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
#endif
BUS_Result_t xmgr_playback_MsgEvtPbSetup_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
BUS_Result_t xmgr_playback_MsgEvtAvDecodingStarted_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
BUS_Result_t xmgr_playback_MsgEvtPbStart_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
BUS_Result_t xmgr_playback_MsgEvtSiPmtReceived_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
BUS_Result_t xmgr_playback_MsgEvtSiPmtTimeout_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
BUS_Result_t xmgr_playback_MsgEvtSiPmtPidChanged_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
BUS_Result_t xmgr_playback_MsgEvtSiEitReceived_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
BUS_Result_t xmgr_playback_MsgEvtSiEitTimeout_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
BUS_Result_t xmgr_playback_MsgEvtAvVideoUnderrun_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
BUS_Result_t xmgr_playback_MsgEvtAvVideoStarted_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
BUS_Result_t xmgr_playback_MsgEvtAvAudioUnderrun_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
BUS_Result_t xmgr_playback_MsgEvtAvAudioStarted_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
BUS_Result_t xmgr_playback_MsgEvtPbTimecodeChange_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
BUS_Result_t xmgr_playback_MsgEvtPbEOF_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
BUS_Result_t xmgr_playback_MsgEvtPbBOF_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
BUS_Result_t xmgr_playback_MsgEvtSbtlStopped_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
BUS_Result_t xmgr_playback_MsgEvtSbtlStarted_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
BUS_Result_t xmgr_playback_MsgEvtCasOk_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
BUS_Result_t xmgr_playback_MsgEvtCasFail_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
BUS_Result_t xmgr_playback_MsgEvtCasScInserted_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
BUS_Result_t xmgr_playback_MsgEvtCasScRemoved_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
BUS_Result_t xmgr_playback_MsgEvtAvBlankCtrlByCAS_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
BUS_Result_t xmgr_playback_MsgEvtAvBlankByDrmInfo_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
BUS_Result_t xmgr_playback_MsgEvtPbThumbnailCreate_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);

BUS_Result_t xmgr_playback_CiPlusMsgBusCreate_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t xmgr_playback_CiPlusMsgEvtPbTimecodeChange_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t xmgr_playback_CiPlusMsgEvtCasDrmSignalled_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t xmgr_playback_CiPlusMsgEvtRecordTimeStampUpdate_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

BUS_Result_t xmgr_playback_NagraMsgBusCreate_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t xmgr_playback_NagraMsgEvtRecordTimeStampUpdate_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t xmgr_playback_NagraMsgEvtCasDrmSignalled_Base(xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t xmgr_playback_NagraMsgEvtCasDrmSignalled_Ses(xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t xmgr_playback_NagraMsgEvtPbTimecodeChange_Base (xmgrPbContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

DxCopyrightControl_SiCcType_e xmgr_playback_NagraGetSiDrmType_Ses(DxCopyrightControl_t *pstDrmInfo);
DxCopyrightControl_CasCcType_e xmgr_playback_NagraGetCasDrmType_Ses(DxCopyrightControl_t *pstDrmInfo);
void xmgr_playback_NagraSendDrmInfo_Base(xmgrPbContext_t *pstContext);

HBOOL xmgr_playback_CheckAvDecUsed_Base (xmgrPbContext_t *pstContext);
HBOOL xmgr_playback_CheckPossibleSCPincode_Base(HUINT32 ulActionId);

#endif /* __XMGR_PLAYBACK_BASE_H__ */
