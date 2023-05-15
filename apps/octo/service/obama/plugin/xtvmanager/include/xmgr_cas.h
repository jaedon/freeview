/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xmgr_cas.h
	@brief

	Description: main main procedure 관련 내용들을 분류한 모듈이다. \n
	Module:
	Remarks : 										\n

	Copyright (c) 2011 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__XMGR_CAS_H__
#define	__XMGR_CAS_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <linkedlist.h>
#include <octo_common.h>

/*******************************************************************/
// DB
/*******************************************************************/
#include <db_common.h>
#include <db_param.h>

/*******************************************************************/
// MW
/*******************************************************************/
#include <svc_pipe.h>
#include <isosvc.h>
#include <svc_si.h>
#include <svc_resources.h>

#include <mgr_common.h>
#include <xmgr_common.h>
#include <mgr_pg.h>
#include <mgr_cas.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
enum
{
	eMEVT_XMGR_CAS			= eMEVT_XMGR_CAS_START,

	/*	Common Messages		>>>
	 */
	eMEVT_XMGR_CAS_APP_UI_TEST,

	eMEVT_XMGR_CAS_SYSINFO_UPDATED,

	eMEVT_XMGR_CAS_UPDATE_SMARTCARD_INFO,

	/*	Irdeto Messages 	>>>
	 */
	eMEVT_XMGR_CAS_IR_PPV_UPDATE_HISTORY_UI,
		/* - Description :	 irdeto cas의 ippv 관련 데이터를 갱신하라는 메시지. ippv관련 purchase history menu에서 받는다.
		   - Parameters Usage :
			   handle : N.A.
			   p1 : 0
			   p2 : 0
			   p3 : 0 */

	/*	Nagra Messages 	>>>
	 */

	/* - Description : APS IRD Command 받아서 메세지를 Update할 때 사용하는 메시지
	   - Parameters Usage :
		   handle : NULL.
		   p1 : ERR_OK : 성공, ERR_FAIL : 실패
		   p2 : NA
		   p3 : NA */
	eMEVT_XMGR_CAS_NA_APS_MESSAGE_UPDATED,

	/* - Description : send credit value to caller appl. when credit response is arrived
		   - Parameters Usage :
			   handle : N.A.
			   p1 : 0 (cas group id)
			   p2 : 0 (request result : success or fail)
			   p3 : 0 (remained credit) */
	eMEVT_XMGR_CAS_NA_NOTIFY_SMARTCARD_CREDIT,

	/* - Description : send tscheck result to caller appl.
		   - Parameters Usage :
			   handle : N.A.
			   p1 : (cas group id)
			   p2 : 0
			   p3 : 0  */
	eMEVT_XMGR_CAS_NA_CLOSE_PPV_WND,

	/* - Description : send tscheck result to caller appl.
		   - Parameters Usage :
			   handle : N.A.
			   p1 : (cas group id)
			   p2 : 0
			   p3 : 0  */
	eMEVT_XMGR_CAS_NA_NOTIFY_PPV_EVENT_CHANGED,

	/* - Description : send tscheck result to caller appl.
		   - Parameters Usage :
			   handle : service handle
			   p1 : (cas group id)
			   p2 : 0 (tsType)
			   p3 : 0 (isSCMatchedWithPMT) */
	eMEVT_XMGR_CAS_NA_NOTIFY_PPV_TSCHECK_RESULT,

	/* - Description : send event Info result to caller appl.
		   - Parameters Usage :
			   handle : TEventStatus
			   p1 : (cas group id)
			   p2 : 0 (TCaEvent)
			   p3 : 0 (PPV_EVENT_INFO) */
	eMEVT_XMGR_CAS_NA_NOTIFY_PPV_EVENTINFO_RESULT,

	/* - Description : send purchase history result to caller appl.
		   - Parameters Usage :
			   handle : TEventStatus
			   p1 : (cas group id)
			   p2 : 0 (TCaEvent)
			   p3 : 0 (PPV_EVENT_INFO) */
	eMEVT_XMGR_CAS_NA_NOTIFY_PPV_PURCHASE_HISTORY,

	/* - Description : send recharge list result to caller appl.
		   - Parameters Usage :
			   handle : xStatus
			   p1 : (cas group id)
			   p2 : 0 (xNumberOfItems)
			   p3 : 0 (RECHARGE_LIST_ITEM) */
	eMEVT_XMGR_CAS_NA_NOTIFY_PPV_RECHARGE_LIST,

	/* - Description : send package list result to caller appl.
		   - Parameters Usage :
			   handle : xStatus
			   p1 : (cas group id)
			   p2 : 0 (xNumberOfItems)
			   p3 : 0 (PPV_PACKAGE_LIST*) */
	eMEVT_XMGR_CAS_NA_NOTIFY_PPV_PACKAGE_LIST,

	/* - Description : send recharge list result to caller appl.
		   - Parameters Usage :
			   handle : TCaObjectStatus
			   p1 : (cas group id)
			   p2 : 0 (xNbItems)
			   p3 : 0 (OPPV_PURCHASE_INFO_ITEM) */
	eMEVT_XMGR_CAS_NA_NOTIFY_PPV_OPPV_LIST,

	/* - Description : send recharge list result to caller appl.
		   - Parameters Usage :
			   handle : TCaRequestStatus
			   p1 : (cas group id)
			   p2 : 0
			   p3 : 0 */
	eMEVT_XMGR_CAS_NA_NOTIFY_PPV_RECHARGE_ALARM,

	/* - Description : send update nagra op type to caller appl.
		   - Parameters Usage :
			   handle : N.A.
			   p1 : (cas group id)
			   p2 : 0 ()
			   p3 : 0 */
	eMEVT_XMGR_CAS_NA_CHANGE_PPV_OPTYPE,

	/* - Description : send OPPV or Recharge close message to display next popup
		   - Parameters Usage :
			   handle : N.A.
			   p1 : (cas group id)
			   p2 : 0 ()
			   p3 : 0 */
	eMEVT_XMGR_CAS_NA_NOTIFY_PPV_POPUP_CLOSED,

	/* - Description : send Event purchase result
		   - Parameters Usage :
			   handle : xStatus.
			   p1 : (cas group id)
			   p2 : 0 (nPurchaseType)
			   p3 : 0 */
	eMEVT_XMGR_CAS_NA_NOTIFY_PPV_PURCHASE_RESULT,

	/* - Description : send Event purchase result
		   - Parameters Usage :
			   handle : reqStatus.
			   p1 : (cas group id)
			   p2 : 0
			   p3 : 0 */
	eMEVT_XMGR_CAS_NA_NOTIFY_PPV_PURCHASE_PACKAGE_RESULT,

	/* - Description : initialize nLastPurchasedPPTEventId when entering into ppv menu
		   - Parameters Usage :
			   handle : reqStatus.
			   p1 : (cas group id)
			   p2 : 0
			   p3 : 0 */
	eMEVT_XMGR_CAS_NA_NOTIFY_PPV_PPT_CLEAR,

/*-------------- RCS Nagra Start -------------------*/

	/* - Description : send credit value to caller appl. when credit response is arrived
		   - Parameters Usage :
			   handle : N.A.
			   p1 : 0 (cas group id)
			   p2 : 0 (request result : success or fail)
			   p3 : 0 (remained credit) */
	eMEVT_XMGR_CAS_NA_NOTIFY_RCS_CREDIT,

	/* - Description : send PPV list to caller appl. when credit response is arrived
		   - Parameters Usage :
			   handle : N.A.
			   p1 : 0 (cas group id)
			   p2 : 0 ()
			   p3 : 0 () */
	eMEVT_XMGR_CAS_NA_NOTIFY_RCS_PPV_LIST,

	/* - Description : send Recharging notify to caller appl. when credit response is arrived
		   - Parameters Usage :
			   handle : N.A.
			   p1 : 0 (cas group id)
			   p2 : 0 ()
			   p3 : 0 () */
	eMEVT_XMGR_CAS_NA_NOTIFY_RCS_RECHARGE,

	/* - Description : send purchasing notify to caller appl. when credit response is arrived
		   - Parameters Usage :
			   handle : N.A.
			   p1 : 0 (cas group id)
			   p2 : 0 ()
			   p3 : 0 () */
	eMEVT_XMGR_CAS_NA_NOTIFY_RCS_PURCHASE_EVENT,

	/* - Description : send event notify to caller appl. when credit response is arrived
		   - Parameters Usage :
			   handle : N.A.
			   p1 : 0 (cas group id)
			   p2 : 0 ()
			   p3 : 0 () */
	eMEVT_XMGR_CAS_NA_NOTIFY_RCS_EVENT_NOTIFY,

	/* - Description : send purchasing history notify to caller appl. when credit response is arrived
		   - Parameters Usage :
			   handle : N.A.
			   p1 : 0 (cas group id)
			   p2 : 0 ()
			   p3 : 0 () */
	eMEVT_XMGR_CAS_NA_NOTIFY_RCS_PURCHASE_HISTORY,

	/* - Description : send Nagra IRD Command mail data update message
		   - Parameters Usage :
			   handle : N.A.
			   p1 : 0 ()
			   p2 : 0 ()
			   p3 : 0 () */
	eMEVT_XMGR_CAS_NA_NOTIFY_IRD_MAIL_UPDATE,

// TODO: For Nagra => move to xmgr_cas_na.h <<<

	/* - Description : UPC spec.
			   - Parameters Usage :
				   handle : N.A.
				   p1 : 0 ()
				   p2 : 0 ()
				   p3 : 0 () */
	eMEVT_XMGR_CAS_NA_7DAYTIMER_AV_CONTROL,


	// event 하나로 통일해도 됨.
	eMEVT_XMGR_CAS_CI_AMMI_REQ_START,
	eMEVT_XMGR_CAS_CI_AMMI_FILE_ACK,
	eMEVT_XMGR_CAS_CI_AMMI_REQ_APP_ABORT,
	eMEVT_XMGR_CAS_CI_AMMI_APP_ABORT_ACK,





};

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
BUS_Callback_t	XMGR_CAS_GetEmmupdateProc(void);

#if defined(CONFIG_CAS_B_C)
BUS_Result_t	XPROC_CAS_BcEmmTs (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t 	XPROC_CAS_BcEmmTsNit (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t 	XPROC_CAS_BcPioneerEmmTs (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

HERROR			XMGR_CAS_BcDisablePreview (Handle_t hAction, HBOOL bDisable);
void 			XMGR_CAS_BcShowHideVideoMessage (HBOOL bShow);
#endif

#if defined(CONFIG_MW_CAS_IRDETO)
void			XMGR_CAS_IrSetSiTableStateTimeout(HBOOL siTableState);
HBOOL			XMGR_CAS_IrIsWaitParentalRatingState(MgrPg_Info_t *pstPgInfo);

/*				pmgr_pg_IsRatingOK 구현하기 위한 IR 함수..
 */
HBOOL			XMGR_CAS_IrIsRatingOK(HUINT32 ulActionId, Handle_t hService, DxDeliveryType_e svcDeliveryType, DxRatingAge_e eRatingSetup, DxRatingAge_e eContentRating);
#if defined(CONFIG_MW_CAS_IRDETO_HOMINGCHANNEL) || defined(CONFIG_MW_CAS_IRDETO_RECOVERY_RESCAN_HOME)
HERROR			XMGR_CAS_IrGetSmcHomeTuneInfo (DbSvc_TuningInfo_t *pstTuningInfo);
#endif
#endif

HERROR XMGR_CAS_NaIsHdPlusSmartCard(HUINT8 ucSlotId, HUINT8 *pbHdPlusSc);
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
HERROR	XMGR_CAS_NaIsSameCasIdInSmartCard(HUINT8 ucSlotId, HUINT16 usCaSysId, HBOOL *pbSame);
#endif

#if defined(CONFIG_MW_CAS_VERIMATRIX_IPTV)
HBOOL 	xmgr_cas_Vmx_IsWaitParentalRatingState(MgrPg_Info_t *pstPgInfo);
HERROR 	xmgr_cas_Vmx_GetRatingLevelFromCasMsg(void *pvCasMsg, HUINT32 *pulRating);
#endif


#endif	/*	!__XMGR_CAS_H__	*/
