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

#ifndef	__PSVC_CAS_H__
#define	__PSVC_CAS_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <svc_cas.h>

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

HERROR psvc_cas_InitProduct(void);
HERROR psvc_cas_InitModule(void);

HERROR psvc_cas_ScenarioInit(void);

HERROR psvc_cas_CiGetBindedCamId(HUINT16 usSlotId, HUINT8 *pucBindedCamId);

HERROR psvc_cas_CiGetSlotId(Handle_t hMmi, CI_SLOT_ID *pusSlotId);
HERROR psvc_cas_CiEnterMenu(CI_SLOT_ID usSlotId);
HERROR psvc_cas_CiMmiClose(Handle_t hMmi);
HERROR psvc_cas_CiMmiSelect(Handle_t hMmi, HUINT8 ucItemNumber);
HERROR psvc_cas_CiMmiAnswerString(Handle_t hMmi, unsigned char *pszStr, unsigned short usStrSize);
HERROR psvc_cas_CiUiRegisterCallback(void);

HERROR psvc_cas_CiUiGetCcReport(HUINT16 usSlotId, HUINT32 ulVersion, SvcCas_CcReport_t **ppstCcReport);
HERROR psvc_cas_CiGetCiPlusUpgradeStatus(HUINT16 usSlotId, HUINT32 ulVersion, SvcCas_CiPlusUpgrade_t **ppstCiPlusUpgrade);

//HERROR CASINST_ChangeCIPath(SvcCas_Context_t *pContext, HUINT16 usSlotId, HBOOL bConnect);
HERROR psvc_cas_CiChangePath(Handle_t hAction, HUINT16 usSlotId, HBOOL bConnect);

//HERROR CASINST_GetCamState(HUINT16 usSlotId, SvcCas_CamState_e *eCamState);
HERROR psvc_cas_CiGetCamState(HUINT16 usSlotId, SvcCas_CamState_e *eCamState);

//void CASINST_RegisterCamCallback(void);
HERROR psvc_cas_CiRegisterCamCallback(void);

//HERROR CASINST_GetCamName(HUINT16 usSlotId, HUINT8 **pName);
HERROR psvc_cas_CiGetCamName(HUINT16 usSlotId, HUINT8 **ppszName);

HERROR psvc_cas_CiGetOpName(HUINT16 usSlotId, HUINT8 **ppszOpName);

HERROR psvc_cas_MgrSubCasInit(void);

HERROR psvc_cas_GetUiData(HUINT8 *pucUiData, DxCAS_UiDataType_e eType);
HERROR psvc_cas_SendCaPmt(HUINT16 usSlotId, HUINT8 *pucCaPmt, HUINT16 usCaPmtLen);

#endif /* !__EPG_PORT_H__ */
