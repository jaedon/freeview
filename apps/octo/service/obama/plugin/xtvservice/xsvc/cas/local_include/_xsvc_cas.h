/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  layer_module.h File Name을 적습니다.
	@brief	  file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/
#ifndef	__XSVC_CAS_INT_H_
#define	__XSVC_CAS_INT_H_
/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <svc_cas.h>
#include <_svc_cas_mgr_common.h>
#include "../base/ci/include/ci.h"		// TODO 검토할 것...

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

/*	아래 기본 8개의 시나리오를 이용 할수 있는 것이 있으면 시나리오 디파인을 켜면 되고 그렇지 않으면
	가장 비슷한 경우를 찾아 참고하여 우선 순위 table를 만들면 된다.

	참고 및 고려 사항 )

	1. 서비스 Stop 될때 기존에 안풀리던것을 적당한 CAS로 찾아 주어야 하는가? 아래 와 같은 이유로 할 필요 없다.
	-> VIEW는 이런 경우 발생 하면안된다 우선 순위가 높아서 애초에 뺏고 시작하기 때문에
	-> Record는 이런 경우 발생해도 다시 시도 안한다. 한번 scramble된 상태로 녹화되면 걍.. 놓아두자.. -> UX 시나리오임..

	2. CAS 정보가 변경되면 어떻게 하는가? eCAS_DEV_EVT_CasInfo 가 날라올 경우를 이야기 한다.
	-> 우선 안풀리는 녀석들 중에서 해당 CAS 를 선택 했던 놈들은 선택 했었다는 기록을 지운다. 왜? CAS정보가 변경 되었으니 당근 다시 시도해 봐야 하기 땜시로~
	-> 기존에 안풀리던 녀석을 찾아서 연결을 시도한다.

	3. sub CAS에서 CAS fail이면 어떻게 하는가? eCAS_MSG_TYPE_Fail 요게 날라 왔다..
	eCAS_MSG_TYPE_Fail을 받는 다고 무조건 fail 메시지를 ap로 보내지 않는다.
		다른 CAS가 권한이 있는지 확인 해야기 때문에 다른 CAS를 시도해 보고 fail 메시지를 보낸다.
		-> 처음 받는 eCAS_State_CasOK, eCAS_MSG_TYPE_Fail을 기준으로 권한이 있는지 없는지 판단한다.
		-> 중간에 권한이 없어지는 것은(eCAS_MSG_TYPE_Fail) 다시 시도하지 않는다.
		-> 시도해 보지 않은 녀석이 있으면 찾아서 시도해 본다.
		-> 만약 모든 CAS를 시도해 봤으면 가장 높은 우선순위의 CAS를 선택하도록 한다. -> 해당 CAS의 error msg를 보도록 한다. ㅡㅡ;

	4. Irdeto 처럼 CAS가 slot을 두개를 가지고 가는 경우는 두개를 하나로 보고 시나리오를 선택 하도록 한다.
*/

/* scenario 1
	embedded CAS가 한개 연결되어 있다.
*/
#if defined(CONFIG_PROD_IR4000HD) || defined(CONFIG_PROD_HD9000I) || defined(CONFIG_PROD_YSR2000) || defined(CONFIG_PROD_YS1000)  || defined(CONFIG_PROD_HDNEO) || defined(CONFIG_PROD_CDIG2000C) || defined(CONFIG_PROD_M1)
#define CAS_SELECT_SUBCAS_SCENARIO_1
#endif


/* scenario 2
	CAM  한 개 있는 경우.
*/
#if defined(CONFIG_PROD_HDR1000S) || defined(CONFIG_PROD_HMSLITE)
#define CAS_SELECT_SUBCAS_SCENARIO_2
#endif

/* scenario 3
	CAM 한 개 embedded CAS 한 개 있는 경우.
*/
#if defined(CONFIG_PROD_ICORDPRO) || defined(CONFIG_PROD_HGS1000S)
#define CAS_SELECT_SUBCAS_SCENARIO_3
#endif

/* scenario 4
	Embedded CAS 두 개 있는 경우.
*/
//#define CAS_SELECT_SUBCAS_SCENARIO_4

/* scenario 5
	CAM이 두개 있는경우.
*/
#if defined(CONFIG_PROD_HMS1000S)
#define CAS_SELECT_SUBCAS_SCENARIO_5
#endif

/* scenario 6
	CAM 두 개 embedded CAS 한개 있는 경우.
*/
//#define CAS_SELECT_SUBCAS_SCENARIO_6

/* scenario 7
	CAM 한 개 embedded CAS 두개 있는 경우.
*/
//#define CAS_SELECT_SUBCAS_SCENARIO_7

/* scenario 8
	CAM 두 개 embedded CAS 두개 있는 경우.
*/
//#define CAS_SELECT_SUBCAS_SCENARIO_8


/* Public Function 마다 필히 기재하도록 함 */
/**
  @brief     LAYER_MODULE_DoSomething 함수.

  Function Name:
  Function Description:
  Parameter:
  type: (Sync or Async)
  Return Value:
*/

HERROR xsvc_cas_InitCi(void);
HERROR xsvc_cas_InitNagra(void);
HERROR xsvc_cas_InitIrdeto(void);
HERROR xsvc_cas_InitIrdetoUc(void);
HERROR xsvc_cas_InitConax(void);
HERROR xsvc_cas_InitViaccess(void);
HERROR xsvc_cas_InitSatsa(void);
HERROR xsvc_cas_InitCiMgr(void);
HERROR xsvc_cas_InitBCas(void);
HERROR xsvc_cas_InitVmxIptv(void);

#if defined(CONFIG_MW_CAS_VIACCESS)
HERROR xsvc_cas_VaMgrInit(void);
#endif
#if defined(CONFIG_MW_CAS_CONAX)
HERROR xsvc_cas_CxMgrInit(void);
#endif
#if defined(CONFIG_MW_CAS_IRDETO_UCAS)
HERROR xsvc_cas_IrUcMgrInit(void);
#endif
#if defined(CONFIG_MW_CAS_IRDETO)
HERROR xsvc_cas_IrMgrInit(void);
#endif
#if defined(CONFIG_MW_CAS_NAGRA)
HERROR xsvc_cas_NaMgrInit(void);
#endif
#if defined(CONFIG_MW_CAS_SATSA)
HERROR xsvc_cas_SatsaInit(void);
#endif
#if defined(CONFIG_MW_CAS_VERIMATRIX_IPTV)
HERROR xsvc_cas_VmxMainInit(void);
#endif

HERROR xsvc_cas_CiGetSlotId(Handle_t hMmi, CI_SLOT_ID *pusSlotId);
HERROR xsvc_cas_CiEnterMenu(CI_SLOT_ID usSlotId);
HERROR xsvc_cas_CiMmiClose(Handle_t hMmi);
HERROR xsvc_cas_CiMmiSelect(Handle_t hMmi, HUINT8 ucItemNumber);
HERROR xsvc_cas_CiMmiAnswerString(Handle_t hMmi, unsigned char *pszStr, unsigned short usStrSize);
HERROR xsvc_cas_CiUiRegisterCallback(void);
#if defined(CONFIG_MW_CI_PLUS)
HERROR xsvc_cas_CiUiGetCcReport(HUINT16 usSlotId, HUINT32 ulVersion, SvcCas_CcReport_t **ppstCcReport);
HERROR xsvc_cas_CiGetCiPlusUpgradeStatus(HUINT16 usSlotId, HUINT32 ulVersion, SvcCas_CiPlusUpgrade_t **ppstCiPlusUpgrade);
#endif
HERROR xsvc_cas_CiChangePath(Handle_t hAction, HUINT16 usSlotId, HBOOL bConnect);
HERROR xsvc_cas_CiGetCamState(HUINT16 usSlotId, SvcCas_CamState_e *eCamState);
HERROR xsvc_cas_CiGetCamName(HUINT16 usSlotId, HUINT8 **ppszName);
HERROR xsvc_cas_CiGetOpName(HUINT16 usSlotId, HUINT8 **ppszOpName);
HERROR xsvc_cas_CiUpdateCamAppInfo(HUINT16 usSlotId, CI_APP_INFO *pAppInfo);
HERROR xsvc_cas_CiRegisterCamCallback(void);


HERROR xsvc_cas_CiInit(void);
HERROR xsvc_cas_CiMgrInit(void);

HERROR xsvc_cas_Scenario1(void);
HERROR xsvc_cas_Scenario2(void);
HERROR xsvc_cas_Scenario3(void);
HERROR xsvc_cas_Scenario4(void);
HERROR xsvc_cas_Scenario5(void);
HERROR xsvc_cas_Scenario6(void);
HERROR xsvc_cas_Scenario7(void);
HERROR xsvc_cas_Scenario8(void);
HERROR xsvc_cas_ScenarioDefault(void);

HINT32 xsvc_cas_CiGetBindedCamId(CI_SLOT_ID usSlotId, HUINT8 *pucBindedCamId);
HERROR xsvc_cas_SendCaPmt(HUINT16 usSlotId, HUINT8 *pucCaPmt, HUINT16 usCaPmtLen);
HERROR xsvc_cas_CiMgrAddMmiDataList(HUINT16 usSlotId, SvcCas_MmiData_t *pstMmiData);

#endif /* !__XSVC_CAS_INT_H_ */

