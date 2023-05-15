/**
	@file     xmgr_ciplus_parentalctrl.c
	@brief    xmgr_ciplus_parentalctrl.c

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    **************************/
/*******************************************************************/

#include "octo_common.h"
#include <bus.h>
#include "mgr_cas.h"
#include "ci_ctrl.h"
#include "ci_plus.h"


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/


/********************************************************************/
/********************      Local Functions	*************************/
/********************************************************************/
STATIC HERROR _xmgr_cas_CiPcGetCiSlotIdOfContent(HUINT32 ulActionId, HUINT16 *pusSlotId)
{
	SvcCas_CtrlParam_t					stParam;
	CI_GetSlotIdByActionIdInParam_t		stSlotIdInParam;
	CI_GetSlotIdByActionIdOutParam_t	stSlotIdOutParam;
	HERROR								hErr;

	stSlotIdInParam.ulActionId = ulActionId;
	stParam.pvIn = (void *)&stSlotIdInParam;
	stParam.pvOut = (void *)&stSlotIdOutParam;
	stParam.ulControlType = eCACTRL_CI_GetSlotIdByActionId;

	hErr = SVC_CAS_CtrlGet(eDxCAS_GROUPID_CAM, eCACTRL_CI_GetSlotIdByActionId, &stParam);
	if (hErr == ERR_FAIL)
	{
		HxLOG_Info("It is not the service which is using CAM...\n");
		// 혹시라도 현재 삽입되어 있는 CAM중 하나라도 pin capability가 2이거나 4 일 때 여기서 처리 되어야 하는지 파악하고 처리해야 한다면 코드 추가.
		return ERR_FAIL;
	}

	*pusSlotId = stSlotIdOutParam.usSlotId;

	return ERR_OK;
}

STATIC HERROR _xmgr_cas_CiPcGetCiPlusPinCapaInfo(HUINT16 usSlotId, CI_PIN_CAPA_INFO *pstPinCapaInfo)
{
	SvcCas_CtrlParam_t					stParam;
	CI_GetPinCapabilityInParam_t		stPinCapaInParam;
	CI_GetPinCapabilityOutParam_t		stPinCapaOutParam;
	HERROR								hErr;

	stPinCapaInParam.usSlotId = usSlotId;
	stParam.pvIn = (void *)&stPinCapaInParam;
	stParam.pvOut = (void *)&stPinCapaOutParam;
	stParam.ulControlType = eCACTRL_CI_GetPinCapa;

	HxLOG_Info("Using CAM Slot ID: [%02x]\n", usSlotId);

	hErr = SVC_CAS_CtrlGet(eDxCAS_GROUPID_CAM, eCACTRL_CI_GetPinCapa, &stParam);
	if (hErr == ERR_FAIL)
	{
		HxLOG_Error("ERROR: SVC_CAS_CtrlGet => eCACTRL_CI_GetPinCapa...\n");
		return ERR_FAIL;
	}

	HxSTD_memcpy(pstPinCapaInfo, &stPinCapaOutParam.stPinCapaInfo, sizeof(CI_PIN_CAPA_INFO));

	return ERR_OK;
}

STATIC HBOOL _mgr_cas_CiPcIsCiPlusPcContent(HUINT32 ulActionId)
{
	HUINT16		usSlotId;
	HERROR		hErr;
	CI_PIN_CAPA_INFO stPinCapaInfo;

	hErr = _xmgr_cas_CiPcGetCiSlotIdOfContent(ulActionId, &usSlotId);
	if (hErr == ERR_FAIL)
	{
		return FALSE;
	}

	hErr = _xmgr_cas_CiPcGetCiPlusPinCapaInfo(usSlotId, &stPinCapaInfo);
	if (hErr == ERR_FAIL)
	{
		return FALSE;
	}

	if (stPinCapaInfo.enPinCapa > CI_PIN_CAPABILITY_NONE)
	{
		// PIN code 관리를 CAM이 하기 때문에 Box의 EIT에 따른 Parental Control 시나리오가 수행 되면 안됨.
		return TRUE;
	}

	return FALSE;
}


HBOOL xmgr_cas_CiPcIsRatingOK(HUINT32 ulActionId, DxRatingAge_e eRatingSetup)
{
#if defined(CONFIG_OP_SES)
	HUINT16 usSlotId;
	HERROR hErr = ERR_FAIL;

	// OP SES 에서는 CAM으로 content가 Descramble 하고 있으면, 해당 content에 대해 STB PC 시나리오는 수행하지 않도록 한다. 따라서 Rating OK를 TRUE로 반환.
	hErr = _xmgr_cas_CiPcGetCiSlotIdOfContent(ulActionId, &usSlotId);
	if (hErr == ERR_OK)
	{
		HxLOG_Info("The content of action ID[%d] is using CAM... Slot ID:[%d]...\n ", usSlotId);
		return TRUE;
	}
#else
	HBOOL bIsCiPlusPcContent = FALSE;

	bIsCiPlusPcContent = _mgr_cas_CiPcIsCiPlusPcContent(ulActionId);

	// bIsCiPlusPcContent가 TRUE이면, CAM이 직접 PC 시나리오를 수행하므로, STB PC 시나리오는 수행하지 않도록 한다. 따라서 Rating OK를 TRUE로 반환.
	if(bIsCiPlusPcContent == TRUE)
	{
		return TRUE;
	}
#endif

	// 현재 content에 대한 Rating 값을 비교 해야할 경우가 생기면 eRatingSetup을 사용하도록 한다.
	NOT_USED_PARAM(eRatingSetup);
	return FALSE;
}


