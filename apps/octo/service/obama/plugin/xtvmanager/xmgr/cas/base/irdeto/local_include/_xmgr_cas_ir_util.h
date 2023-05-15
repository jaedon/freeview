#ifndef	__XMGR_CAS_IR_UTIL_INT_H__
#define	__XMGR_CAS_IR_UTIL_INT_H__


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <mgr_pg.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#define XMGR_CAS_IR_PARAM_UNUSED(x) ((void)x)
#define XMGR_CAS_IR_PARAM_WARRING_REMOVE(arg1, arg2, arg3, arg4)		\
	{													\
		(void)arg1; 									\
		(void)arg2; 									\
		(void)arg3; 									\
		(void)arg4; 									\
	}


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
#define	IR_UI_SC_NUMBER_STRING_LEN				64
#define	IR_UI_TA_LEFT			0
#define	IR_UI_TA_RIGHT			1
#define	IR_UI_TA_CENTER			2

typedef enum
{
	eCasIr_DDEMM_UI_ForcedOta,
	eCasIr_DDEMM_UI_RetuneFromHome,
	eCasIr_DDEMM_UI_DisplayCardNo,
	eCasIr_DDEMM_UI_ColourDisplay,
	eCasIr_DDEMM_UI_EasySMS,
	eCasIr_DDEMM_UI_RemoteTagging,
	eCasIr_DDEMM_UI_RemoteRecord
} XmgrCas_IrDDEMM_UI_e;

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

#if defined (CONFIG_MW_CAS_IRDETO_PPV)
HBOOL	xmgr_cas_IrProcessPPVQuery(HUINT32 msg, HUINT8 *pData);
HBOOL	xmgr_cas_IrProcessPPVReply(HUINT32 msg, HUINT8 *pData);
#endif
#if defined (CONFIG_MW_CAS_IRDETO_HOMINGCHANNEL) || defined(CONFIG_MW_CAS_IRDETO_RECOVERY_RESCAN_HOME)
HERROR	xmgr_cas_IrUpdateHomingChannelInfo(void *pvData);
#endif
HBOOL	xmgr_cas_IrGetCardInOutStatus(HUINT8 ucSlotId);
void	xmgr_cas_IrSetCardInOutStatus(HUINT8 ucSlotId, HUINT8 bInserted);
HBOOL	xmgr_cas_IrGetNationalityAvailable(void);
void 	xmgr_cas_IrSetNationalityAvailable(HBOOL bAvailable);

#if defined(CONFIG_SUPPORT_IRDETO_AP_STATE)
void	xmgr_cas_IrInitApState(void);
HERROR	xmgr_cas_IrGetApState(XmgrCas_IrApState_e eIrApState, HUINT32 *pulState);
HERROR	xmgr_cas_IrSetApState(XmgrCas_IrApState_e eIrApState, HUINT32 ulState);
HINT8*	xmgr_cas_IrGetStateStr(XmgrCas_IrApState_e eIrApState, HUINT32 ulState);
/* should be removed
HERROR	AP_CAS_IR_ProcessWtvState(WTV_AppState_t *peWtvState);
*/
#if defined(CONFIG_MW_CAS_IRDETO_SOL)
HERROR	xmgr_cas_IrProcessSolState(Handle_t	hAction, HUINT16 usOnId, HUINT16 usTsId);
#endif
#if defined(CONFIG_MW_CAS_IRDETO_FTA_BLOCK)
HERROR	xmgr_cas_IrProcessFTABlockState(Handle_t hAction);
#endif
#if defined(CONFIG_MW_CAS_IRDETO_SDT_RUNNING_STATUS_BLOCK)
HERROR	xmgr_cas_IrSdtProcess(HUINT32 ulStatus);
#endif
#endif

#if defined(CONFIG_MW_CAS_IRDETO_MATURITY_RATING)
HBOOL	xmgr_cas_IrCheckIsMrDisable(void);
HERROR	xmgr_cas_IrMrUpdatePin(Handle_t hAction, HUINT8 *szPinCodeStr);
#endif

HINT32	xmgr_cas_IrGetRatingErrorCode (void);
void	xmgr_cas_IrGetParentalRatingErrorCode(HUINT8 *pPincodeStr);

HERROR	xmgr_cas_IrScInfo(HUINT8 ucSlotId, void *pvData);

#endif /* !__XMGR_CAS_IR_UTIL_INT_H__ */

