/**
	@file     file_name.c File Name을 적습니다.
	@brief    file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
/** @brief global header file  */
#include <stdio.h>
#include <string.h>
#include <octo_common.h>

#include <xsvc_cas.h>

#include "local_include/_xsvc_cas.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

/**
  @brief     LAYER_MODULE_DoSomething 함수.

  simple comment \n

  @return    return 값.
*/

HERROR psvc_cas_InitProduct(void)
{
	return ERR_OK;

}

HERROR psvc_cas_InitModule(void)
{
	return ERR_OK;

}

HERROR psvc_cas_CiGetSlotId(Handle_t hMmi, CI_SLOT_ID *pusSlotId)
{
#ifdef CONFIG_MW_CI
	return xsvc_cas_CiGetSlotId(hMmi, pusSlotId);
#else
	return ERR_FAIL;
#endif
}

HERROR psvc_cas_CiEnterMenu(CI_SLOT_ID usSlotId)
{
#ifdef CONFIG_MW_CI
	return xsvc_cas_CiEnterMenu(usSlotId);
#else
	return ERR_FAIL;
#endif
}

HERROR psvc_cas_CiMmiClose(Handle_t hMmi)
{
#ifdef CONFIG_MW_CI
	return xsvc_cas_CiMmiClose(hMmi);
#else
	return ERR_FAIL;
#endif
}

HERROR psvc_cas_CiMmiSelect(Handle_t hMmi, HUINT8 ucItemNumber)
{
#ifdef CONFIG_MW_CI
	return xsvc_cas_CiMmiSelect(hMmi, ucItemNumber);
#else
	return ERR_FAIL;
#endif
}

HERROR psvc_cas_CiMmiAnswerString(Handle_t hMmi, unsigned char *pszStr, unsigned short usStrSize)
{
#ifdef CONFIG_MW_CI
	return xsvc_cas_CiMmiAnswerString(hMmi, pszStr, usStrSize);
#else
	return ERR_FAIL;
#endif
}

HERROR psvc_cas_CiUiRegisterCallback(void)
{
#ifdef CONFIG_MW_CI
	return xsvc_cas_CiUiRegisterCallback();
#else
	return ERR_FAIL;
#endif
}


HERROR psvc_cas_CiUiGetCcReport(HUINT16 usSlotId, HUINT32 ulVersion, SvcCas_CcReport_t **ppstCcReport)
{
#if defined(CONFIG_MW_CI) && defined(CONFIG_MW_CI_PLUS)
	return xsvc_cas_CiUiGetCcReport(usSlotId, ulVersion, ppstCcReport);
#else
	return ERR_FAIL;
#endif
}

HERROR psvc_cas_CiGetCiPlusUpgradeStatus(HUINT16 usSlotId, HUINT32 ulVersion, SvcCas_CiPlusUpgrade_t **ppstCiPlusUpgrade)
{
#if defined(CONFIG_MW_CI) && defined(CONFIG_MW_CI_PLUS)
	return psvc_cas_CiGetCiPlusUpgradeStatus(usSlotId, ulVersion, ppstCiPlusUpgrade);
#else
	return ERR_FAIL;
#endif
}

HERROR psvc_cas_CiChangePath(Handle_t hAction, HUINT16 usSlotId, HBOOL bConnect)
{
#if defined(CONFIG_MW_CI)
	return xsvc_cas_CiChangePath(hAction, usSlotId, bConnect);
#else
	return ERR_FAIL;
#endif
}

HERROR psvc_cas_CiGetCamState(HUINT16 usSlotId, SvcCas_CamState_e *eCamState)
{
#ifdef CONFIG_MW_CI
	return psvc_cas_CiGetCamState(usSlotId, eCamState);
#else
	return ERR_FAIL;
#endif
}

HERROR psvc_cas_CiGetCamName(HUINT16 usSlotId, HUINT8 **ppszName)
{
#ifdef CONFIG_MW_CI
	return xsvc_cas_CiGetCamName(usSlotId, ppszName);
#else
	return ERR_FAIL;
#endif
}

HERROR psvc_cas_CiGetOpName(HUINT16 usSlotId, HUINT8 **ppszOpName)
{
#ifdef CONFIG_MW_CI
	return xsvc_cas_CiGetOpName(usSlotId, ppszOpName);
#else
	return ERR_FAIL;
#endif
}

HERROR psvc_cas_CiUpdateCamAppInfo(HUINT16 usSlotId, CI_APP_INFO *pAppInfo)
{
#ifdef CONFIG_MW_CI
	return xsvc_cas_CiUpdateCamAppInfo(usSlotId, pAppInfo);
#else
	return ERR_FAIL;
#endif
}

HERROR psvc_cas_CiRegisterCamCallback(void)
{
#ifdef CONFIG_MW_CI
	return xsvc_cas_CiRegisterCamCallback();
#else
	return ERR_FAIL;
#endif
}

HERROR psvc_cas_ScenarioInit(void)
{
#if defined(CAS_SELECT_SUBCAS_SCENARIO_1)
	return xsvc_cas_Scenario1();
#elif defined(CAS_SELECT_SUBCAS_SCENARIO_2)
	return xsvc_cas_Scenario2();
#elif defined(CAS_SELECT_SUBCAS_SCENARIO_3)
	return xsvc_cas_Scenario3();
#elif defined(CAS_SELECT_SUBCAS_SCENARIO_4)
	return xsvc_cas_Scenario4();
#elif defined(CAS_SELECT_SUBCAS_SCENARIO_5)
	return xsvc_cas_Scenario5();
#elif defined(CAS_SELECT_SUBCAS_SCENARIO_6)
	return xsvc_cas_Scenario6();
#elif defined(CAS_SELECT_SUBCAS_SCENARIO_7)
	return xsvc_cas_Scenario7();
#elif defined(CAS_SELECT_SUBCAS_SCENARIO_8)
	return xsvc_cas_Scenario8();
#else
	return xsvc_cas_ScenarioDefault();
#endif

}

HERROR psvc_cas_CiGetBindedCamId(HUINT16 usSlotId, HUINT8 *pucBindedCamId)
{
#if defined(CONFIG_MW_CI)
	HINT32	nRet;
	nRet = xsvc_cas_CiGetBindedCamId(usSlotId, pucBindedCamId);
	return (nRet != ERR_OK) ? ERR_FAIL : ERR_OK;
#else
	return ERR_FAIL;
#endif
}

HERROR psvc_cas_MgrSubCasInit(void)
{
	HERROR		hErr = ERR_OK;

#if defined(CONFIG_MW_CAS_NAGRA)
	hErr |= xsvc_cas_InitNagra();
	if(hErr != ERR_OK)
	{
		HxLOG_Error("sub cas init error - xsvc_cas_InitNagra() \n");
		return ERR_FAIL;
	}
#endif

#if defined(CONFIG_MW_CAS_VIACCESS)
	hErr |= xsvc_cas_InitViaccess();
	if(hErr != ERR_OK)
	{
		HxLOG_Error("sub cas init error - xsvc_cas_InitViaccess() \n");
		return ERR_FAIL;
	}
#endif

#if defined(CONFIG_MW_CAS_IRDETO) && defined(CONFIG_MW_CAS_IRDETO_SCELL)
	hErr |= xsvc_cas_InitIrdeto();
	if(hErr != ERR_OK)
	{
		HxLOG_Error("sub cas init error - xsvc_cas_InitIrdeto() \n");
		return ERR_FAIL;
	}
#endif

#if defined(CONFIG_MW_CAS_IRDETO) && defined(CONFIG_MW_CAS_IRDETO_UCAS)
	hErr |= xsvc_cas_InitIrdetoUc();
	if(hErr != ERR_OK)
	{
		HxLOG_Error("sub cas init error - xsvc_cas_InitIrdetoUc() \n");
		return ERR_FAIL;
	}
#endif

#if defined(CONFIG_MW_CAS_CONAX)
	hErr |= xsvc_cas_InitConax();
	if(hErr != ERR_OK)
	{
		HxLOG_Error("sub cas init error - xsvc_cas_InitConax() \n");
		return ERR_FAIL;
	}
#endif

#if defined(CONFIG_MW_CAS_SATSA)
	hErr |= ERR_FAIL;
#endif

#if defined(CONFIG_MW_CAS_VERIMATRIX_IPTV)
	hErr |= xsvc_cas_InitVmxIptv();
	if(hErr != ERR_OK)
	{
		HxLOG_Error("sub cas init error - xsvc_cas_InitViaccess() \n");
		return ERR_FAIL;
	}
#endif

#if defined(CONFIG_MW_CI)
	hErr |= xsvc_cas_CiMgrInit();
	if(hErr != ERR_OK)
	{
		HxLOG_Error("sub cas init error - xsvc_cas_CiMgrInit() \n");
		return ERR_FAIL;
	}
#endif

	return hErr;
}

#if defined(CONFIG_MW_CAS_IRDETO)
extern HERROR	xsvc_cas_IrGetUiData(HUINT8 *pucUiData, DxCAS_UiDataType_e eType);
#endif

HERROR psvc_cas_GetUiData(HUINT8 *pucUiData, DxCAS_UiDataType_e eType)
{
	HERROR		hErr = ERR_OK;

#if defined(CONFIG_MW_CAS_IRDETO)
	if(eType & eDxCAS_UIDATATYPE_IRDETO_START)
	{
		hErr = xsvc_cas_IrGetUiData(pucUiData, eType);
	}
#else
	hErr = ERR_FAIL;
#endif

	return hErr;
}

HERROR psvc_cas_SendCaPmt(HUINT16 usSlotId, HUINT8 *pucCaPmt, HUINT16 usCaPmtLen)
{
#if defined(CONFIG_MW_CI) && defined(CONFIG_MW_CI_USE_EXTERNAL_CONTROLLER)
	return xsvc_cas_SendCaPmt( usSlotId,pucCaPmt, usCaPmtLen);
#else
	return ERR_FAIL;
#endif
}
/*********************** End of File ******************************/
