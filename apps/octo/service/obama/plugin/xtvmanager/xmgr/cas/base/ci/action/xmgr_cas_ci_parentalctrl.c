/**
	@file     xmgr_ciplus_parentalctrl.c
	@brief    xmgr_ciplus_parentalctrl.c

	Description: File�� ���� ������ �����ϴ�.		\n
	Module: Layer�� ��� ���� �����ϴ�.			 	\n
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
		// Ȥ�ö� ���� ���ԵǾ� �ִ� CAM�� �ϳ��� pin capability�� 2�̰ų� 4 �� �� ���⼭ ó�� �Ǿ�� �ϴ��� �ľ��ϰ� ó���ؾ� �Ѵٸ� �ڵ� �߰�.
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
		// PIN code ������ CAM�� �ϱ� ������ Box�� EIT�� ���� Parental Control �ó������� ���� �Ǹ� �ȵ�.
		return TRUE;
	}

	return FALSE;
}


HBOOL xmgr_cas_CiPcIsRatingOK(HUINT32 ulActionId, DxRatingAge_e eRatingSetup)
{
#if defined(CONFIG_OP_SES)
	HUINT16 usSlotId;
	HERROR hErr = ERR_FAIL;

	// OP SES ������ CAM���� content�� Descramble �ϰ� ������, �ش� content�� ���� STB PC �ó������� �������� �ʵ��� �Ѵ�. ���� Rating OK�� TRUE�� ��ȯ.
	hErr = _xmgr_cas_CiPcGetCiSlotIdOfContent(ulActionId, &usSlotId);
	if (hErr == ERR_OK)
	{
		HxLOG_Info("The content of action ID[%d] is using CAM... Slot ID:[%d]...\n ", usSlotId);
		return TRUE;
	}
#else
	HBOOL bIsCiPlusPcContent = FALSE;

	bIsCiPlusPcContent = _mgr_cas_CiPcIsCiPlusPcContent(ulActionId);

	// bIsCiPlusPcContent�� TRUE�̸�, CAM�� ���� PC �ó������� �����ϹǷ�, STB PC �ó������� �������� �ʵ��� �Ѵ�. ���� Rating OK�� TRUE�� ��ȯ.
	if(bIsCiPlusPcContent == TRUE)
	{
		return TRUE;
	}
#endif

	// ���� content�� ���� Rating ���� �� �ؾ��� ��찡 ����� eRatingSetup�� ����ϵ��� �Ѵ�.
	NOT_USED_PARAM(eRatingSetup);
	return FALSE;
}


