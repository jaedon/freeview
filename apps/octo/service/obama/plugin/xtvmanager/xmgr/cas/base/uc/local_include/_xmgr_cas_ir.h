#ifndef	__XMGR_CAS_IR_INT_H__
#define	__XMGR_CAS_IR_INT_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <mgr_live.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#if defined(CONFIG_MW_CAS_IRDETO_SDT_RUNNING_STATUS_BLOCK) || defined(CONFIG_MW_CAS_IRDETO_SOL) || defined(CONFIG_MW_CAS_IRDETO_MATURITY_RATING) || defined(CONFIG_MW_CAS_IRDETO_FTA_BLOCK)
#define CONFIG_SUPPORT_IRDETO_AP_STATE
#endif

#define		TEXT_SIZE64			64
#define		TEXT_SIZE128		128
#define		TEXT_SIZE256		256
#define		TEXT_SIZE512		512

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
#if defined(CONFIG_SUPPORT_IRDETO_AP_STATE)
typedef enum
{
	eIrApRunningState,
	eIrApSolState,
	eIrApMaturityState,
	eIrApFTABlockState,
	eIrApStatusMax
} XmgrCas_IrApState_e;

typedef enum
{
	eRunningStateWaitSdt,
	eRunningStateOK,
	eRunningStateFail,
	eRunningStateSdtTimeOut,
	eRunningStateMax
} XmgrCas_IrRunningState_e;

typedef enum
{
	eSolStateWaitSol,
	eSolStateOK,
	eSolStateFail,
	eSolStateTimeOut,
	eSolStateMax
} XmgrCas_IrSolState_e;

typedef enum
{
	eMRStateWait,
	eMRStateOK,
	eMRStateFail,
	eMRStateMax
} XmgrCas_IrMRState_e;
#endif

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
void	xmgr_cas_IrScReadUserDataReply(HUINT8 *pData);
void	xmgr_cas_IrScPinCodeReply(HUINT8 *pData);

#if defined(CONFIG_SUPPORT_IRDETO_AP_STATE)
void	xmgr_cas_IrInitApState(void);
HERROR	xmgr_cas_IrGetApState(XmgrCas_IrApState_e eIrApState, HUINT32 *pulState);
HERROR	xmgr_cas_IrSetApState(XmgrCas_IrApState_e eIrApState, HUINT32 ulState);
HINT8*	xmgr_cas_IrGetStateStr(XmgrCas_IrApState_e eIrApState, HUINT32 ulState);
/* should be removed
HERROR	AP_CAS_IR_ProcessWtvState(WTV_AppState_t *peWtvState);
*/

#if defined(CONFIG_MW_CAS_IRDETO_SOL)
HERROR	xmgr_cas_IrProcessSolState(Handle_t hAction, HUINT16 usOnId, HUINT16 usTsId);
#endif

#if defined(CONFIG_MW_CAS_IRDETO_FTA_BLOCK)
HERROR	xmgr_cas_IrProcessFTABlockState(Handle_t hAction);;
#endif

#if defined(CONFIG_MW_CAS_IRDETO_SDT_RUNNING_STATUS_BLOCK)
HERROR	xmgr_cas_IrSdtProcess(HUINT32 eRunningStatus);
#endif

#endif

//1 must be implemented

#if defined(CONFIG_MW_CAS_IRDETO_MATURITY_RATING)
HBOOL	xmgr_cas_IrCheckIsMrDisable(void);
HERROR	xmgr_cas_IrMrUpdatePin(Handle_t hAction, HUINT8 *szDECPinCode);
#endif

HERROR	xmgr_cas_IrSetCountryCodeFromEIT(HUINT8 *aucCountryCode);

#endif /* !__XMGR_CAS_IR_INT_H__ */

