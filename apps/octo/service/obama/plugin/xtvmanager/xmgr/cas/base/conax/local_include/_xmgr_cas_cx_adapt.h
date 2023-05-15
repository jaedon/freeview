#ifndef	__XMGR_CAS_CX_ADAPT_INT_H__
#define	__XMGR_CAS_CX_ADAPT_INT_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <conax.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

#define	CAS_CX_MAX_SAVED_LIST			100


/*******************************************************************/
/********************      	Global Prototype	********************/
/*******************************************************************/

HERROR		xmgr_cas_CxErrorDataProcessing (void *pDataCx, HUINT8 *ucAlarm_type);
HERROR		xmgr_cas_CxCheckDataProcessing (void *pDataCx, HINT32 *lCkeck_type);
HERROR		xmgr_cas_CxCommandProcessing (void *pDataCx, HINT32 *lCmd_type);

/* Command msg Process */
void		xmgr_cas_CxCmdProcessing (HUINT8 *pszCmd, HUINT8 *pszMsgType, HUINT8 *pszmsg1, HUINT8 *pszmsg2);

void		xmgr_cas_CxSetAcceptCAPincode (HUINT8 *szPin);
void		xmgr_cas_CxGetAcceptCAPincode (HUINT8 *szPin);

/* PPV Control */
void		xmgr_cas_CxSetPpvMgrProductID (CxUiMsgType_t	eUiMsgType, HUINT32 ulProductID);
void		xmgr_cas_CxSetPpvMgrAction (CxUiMsgType_t eUiMsgType, HUINT8* szPin);

/* Order Info  */
void		xmgr_cas_CxSetOrderProductId (HUINT32 ulOrderID);

/* Accept Viewing Info  */
void		xmgr_cas_CxSetAcceptViewProductId (HUINT32 ulAcceptViewID);
void		xmgr_cas_CxSetAcceptViewProductIDList (HUINT8* szPin);
HBOOL		xmgr_cas_CxCompareAcceptViewProductIDList (HUINT32 ulAcceptProductID);
void		xmgr_cas_CxSetAcceptViewService (Handle_t hMainSvcHanel);
void		xmgr_cas_CxGetAcceptViewService (Handle_t *hSvcAccept);
void		xmgr_cas_CxClearAcceptViewProductIDList (void);

/* Tokens Debit Info  */
void		xmgr_cas_CxSetAcceptTokensProductId (HUINT32 ulAcceptTokensID);
void		xmgr_cas_CxSetAcceptTokensProgramLable (HUINT8 *szPrgramLable);
void		xmgr_cas_CxSetAcceptTokensProductIDList (HUINT8 *szPin);
HBOOL		xmgr_cas_CxCompareAcceptTokensProductIDList (HUINT16 nAcceptDebitProductID, HUINT8 *ucAcceptDebitProgramLabel, HUINT8 ucInsuffientToken);
void		xmgr_cas_CxSetAcceptTokensService (Handle_t hSvcAccept);
void		xmgr_cas_CxGetAcceptTokensService (Handle_t *hSvcAccept);
void		xmgr_cas_CxClearAcceptTokensProductIDList (void);

#if defined (CONFIG_MW_CAS_CONAX_FINGERPRINT)
/* Finger Print  */
HERROR		xmgr_cas_CxOnFingerPrint (HINT32 lMsg, HBOOL *bTimePrint, HULONG *ulTime, HUINT16 *usDuration);
void		xmgr_cas_CxSetFingerPrintDisplayed (void);
#endif	// End of defined (CONFIG_MW_CAS_CONAX_FINGERPRINT)

#if defined (CONFIG_MW_CAS_CONAX_TEXT)
/* User Text  */
HERROR		xmgr_cas_CxOnUserText (HINT32 lMsg, CxUiMsgInfo_t	*stPrevCxUiUTInfo, HBOOL *bUserTextTime, HULONG *ulTime);
HERROR		xmgr_cas_CxSetUserTextDisplayed (HUINT8 ucSeqNo, CxUTDisplayCount_t ulOpt);
HERROR		xmgr_cas_CxRemoveUserText (HUINT8 ucSeqNo);
#endif	// End of defined (CONFIG_MW_CAS_CONAX_TEXT)

#endif /* !__XMGR_CAS_CX_ADAPT_INT_H__ */

