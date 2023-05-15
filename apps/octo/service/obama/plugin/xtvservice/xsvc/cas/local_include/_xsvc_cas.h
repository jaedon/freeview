/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  layer_module.h File Name�� �����ϴ�.
	@brief	  file_name & simple comment.

	Description: File�� ���� ������ �����ϴ�.		\n
	Module: Layer�� ��� ���� �����ϴ�.			 	\n
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
#include "../base/ci/include/ci.h"		// TODO ������ ��...

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

/*	�Ʒ� �⺻ 8���� �ó������� �̿� �Ҽ� �ִ� ���� ������ �ó����� �������� �Ѹ� �ǰ� �׷��� ������
	���� ����� ��츦 ã�� �����Ͽ� �켱 ���� table�� ����� �ȴ�.

	���� �� ��� ���� )

	1. ���� Stop �ɶ� ������ ��Ǯ�������� ������ CAS�� ã�� �־�� �ϴ°�? �Ʒ� �� ���� ������ �� �ʿ� ����.
	-> VIEW�� �̷� ��� �߻� �ϸ�ȵȴ� �켱 ������ ���Ƽ� ���ʿ� ���� �����ϱ� ������
	-> Record�� �̷� ��� �߻��ص� �ٽ� �õ� ���Ѵ�. �ѹ� scramble�� ���·� ��ȭ�Ǹ� ��.. ���Ƶ���.. -> UX �ó�������..

	2. CAS ������ ����Ǹ� ��� �ϴ°�? eCAS_DEV_EVT_CasInfo �� ����� ��츦 �̾߱� �Ѵ�.
	-> �켱 ��Ǯ���� �༮�� �߿��� �ش� CAS �� ���� �ߴ� ����� ���� �߾��ٴ� ����� �����. ��? CAS������ ���� �Ǿ����� ��� �ٽ� �õ��� ���� �ϱ� ���÷�~
	-> ������ ��Ǯ���� �༮�� ã�Ƽ� ������ �õ��Ѵ�.

	3. sub CAS���� CAS fail�̸� ��� �ϴ°�? eCAS_MSG_TYPE_Fail ��� ���� �Դ�..
	eCAS_MSG_TYPE_Fail�� �޴� �ٰ� ������ fail �޽����� ap�� ������ �ʴ´�.
		�ٸ� CAS�� ������ �ִ��� Ȯ�� �ؾ߱� ������ �ٸ� CAS�� �õ��� ���� fail �޽����� ������.
		-> ó�� �޴� eCAS_State_CasOK, eCAS_MSG_TYPE_Fail�� �������� ������ �ִ��� ������ �Ǵ��Ѵ�.
		-> �߰��� ������ �������� ����(eCAS_MSG_TYPE_Fail) �ٽ� �õ����� �ʴ´�.
		-> �õ��� ���� ���� �༮�� ������ ã�Ƽ� �õ��� ����.
		-> ���� ��� CAS�� �õ��� ������ ���� ���� �켱������ CAS�� �����ϵ��� �Ѵ�. -> �ش� CAS�� error msg�� ������ �Ѵ�. �Ѥ�;

	4. Irdeto ó�� CAS�� slot�� �ΰ��� ������ ���� ���� �ΰ��� �ϳ��� ���� �ó������� ���� �ϵ��� �Ѵ�.
*/

/* scenario 1
	embedded CAS�� �Ѱ� ����Ǿ� �ִ�.
*/
#if defined(CONFIG_PROD_IR4000HD) || defined(CONFIG_PROD_HD9000I) || defined(CONFIG_PROD_YSR2000) || defined(CONFIG_PROD_YS1000)  || defined(CONFIG_PROD_HDNEO) || defined(CONFIG_PROD_CDIG2000C) || defined(CONFIG_PROD_M1)
#define CAS_SELECT_SUBCAS_SCENARIO_1
#endif


/* scenario 2
	CAM  �� �� �ִ� ���.
*/
#if defined(CONFIG_PROD_HDR1000S) || defined(CONFIG_PROD_HMSLITE)
#define CAS_SELECT_SUBCAS_SCENARIO_2
#endif

/* scenario 3
	CAM �� �� embedded CAS �� �� �ִ� ���.
*/
#if defined(CONFIG_PROD_ICORDPRO) || defined(CONFIG_PROD_HGS1000S)
#define CAS_SELECT_SUBCAS_SCENARIO_3
#endif

/* scenario 4
	Embedded CAS �� �� �ִ� ���.
*/
//#define CAS_SELECT_SUBCAS_SCENARIO_4

/* scenario 5
	CAM�� �ΰ� �ִ°��.
*/
#if defined(CONFIG_PROD_HMS1000S)
#define CAS_SELECT_SUBCAS_SCENARIO_5
#endif

/* scenario 6
	CAM �� �� embedded CAS �Ѱ� �ִ� ���.
*/
//#define CAS_SELECT_SUBCAS_SCENARIO_6

/* scenario 7
	CAM �� �� embedded CAS �ΰ� �ִ� ���.
*/
//#define CAS_SELECT_SUBCAS_SCENARIO_7

/* scenario 8
	CAM �� �� embedded CAS �ΰ� �ִ� ���.
*/
//#define CAS_SELECT_SUBCAS_SCENARIO_8


/* Public Function ���� ���� �����ϵ��� �� */
/**
  @brief     LAYER_MODULE_DoSomething �Լ�.

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

