/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <mgr_pg.h>
#include <svc_cas.h>
#include <mgr_action.h>

#include "local_include/_xmgr_cas_vmx_util.h"
#include "_xsvc_cas_vmx_ctrl.h"
#if defined(CONFIG_MW_CAS_VERIMATRIX_IPTV)
#include "_xsvc_cas_vmx_iptvh_ui.h"
#endif

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/



/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/



/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/



/*******************************************************************/
/********************      Structure       *************************/
/*******************************************************************/



/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/



/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/



/*******************************************************************/
/********************      Local Functions      ********************/
/*******************************************************************/
#define _____XMGR_CAS_VMX_PARENTAL_CONTROL_LOCAL_FUNCTION______________________________________________________



/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
#define _____XMGR_CAS_VMX_PARENTAL_CONTROL_PUBLIC_FUNCTION______________________________________________________
HERROR xmgr_cas_Vmx_SetRatingLevel(void)
{
	SvcCas_CtrlParam_t stParam;

	if(SVC_CAS_CtrlSet(eDxCAS_GROUPID_VX, eVmxIptvCtrl_SetRatingLevel, &stParam) != ERR_OK)
	{
		HxLOG_Error("SVC_CAS_CtrlSet(eVmxIptvCtrl_SetRatingLevel) error\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR xmgr_cas_Vmx_GetRatingLevel(HUINT8 *pucRating)
{
	SvcCas_CtrlParam_t stParam;

	VMXIPTV_GetRatingLevelOutParam_t	stOutParam;

	stParam.ulControlType	= eVmxIptvCtrl_GetRatingLevel;
	stParam.pvIn			= NULL;
	stParam.pvOut			= (void *)&stOutParam;


	if(SVC_CAS_CtrlGet(eDxCAS_GROUPID_VX, eVmxIptvCtrl_GetRatingLevel, &stParam) != ERR_OK)
	{
		HxLOG_Error("SVC_CAS_CtrlSet(eVmxIptvCtrl_SetRatingLevel) error\n");
		return ERR_FAIL;
	}

	* pucRating= stOutParam.ucRatingLevel;

	return ERR_OK;
}

HERROR xmgr_cas_Vmx_SetOverrideRatingLevel(Handle_t hAction, HUINT32 nActionId)
{
	SvcCas_CtrlParam_t				stParam;
	VMXIPTV_SetOverrideRatingLevelInParam_t	stInParam;

	stInParam.hAction		= hAction;
	stInParam.nActionId		= nActionId;
	stInParam.bUserPinPass	= TRUE;

	stParam.pvIn			= &stInParam;
	stParam.ulControlType	= eVmxIptvCtrl_SetOverrideRatingLevel;

	if(SVC_CAS_CtrlSet(eDxCAS_GROUPID_NA, stParam.ulControlType, &stParam) != ERR_OK)
	{
		HxLOG_Error("SVC_CAS_CtrlSet(eVmxIptvCtrl_SetOverrideRatingLevel) error\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HBOOL xmgr_cas_Vmx_IsWaitParentalRatingState(MgrPg_Info_t *pstPgInfo)
{
	return (pstPgInfo->bRatingValid == FALSE ? TRUE : FALSE);
}

HERROR xmgr_cas_Vmx_ConvertRatingBaseToVmx(HUINT8 ucBaseRating, HUINT8 *pucVmxRating)
{
	*pucVmxRating = ucBaseRating;// TODO_VMX : Convert
	return ERR_OK;
}

HERROR xmgr_cas_Vmx_ConvertRatingVmxToBase(HUINT32 ulVmxRating, HUINT32 *pulBaseRating)
{
	*pulBaseRating = ulVmxRating;// TODO_VMX : Convert
	return ERR_OK;
}

HERROR xmgr_cas_Vmx_GetRatingLevelFromCasMsg(void *pvCasMsg, HUINT32 *pulRating)
{
	VmxIptv_DisplayUi_t		*pstDisplayUi = NULL;

	if ((pvCasMsg == NULL) || (pulRating == NULL))
	{
		HxLOG_Error("Error! invalid param. \n");
		return ERR_FAIL;
	}

	pstDisplayUi = (VmxIptv_DisplayUi_t *)pvCasMsg;
	if(pstDisplayUi)
	{
		if(pstDisplayUi->eType == eVmxIptv_Display_PinDialog)
		{
			if(pstDisplayUi->item.stErrorMsg.eErrMsg == eVmxIptv_ErrMsg_DecodeFailurePinRequired)
			{
				xmgr_cas_Vmx_ConvertRatingVmxToBase(pstDisplayUi->item.stErrorMsg.ulValue, pulRating);
				return ERR_OK;
			}
		}
	}

	return ERR_FAIL;
}

#if 0
HBOOL xmgr_cas_Vmx_IsRatingOK(HUINT32 ulActionId, DxRatingAge_e eRatingSetup)
{
	HBOOL bRatingOK = TRUE;
	HUINT8 ucChannelRating = eDxRATING_VIEW_ALL;
	HBOOL bVmxIptvRatingValid;

	if(MGR_ACTION_GetMainActionId() == ulActionId)
	{
		// TODO_VMX : ucChannelRating�� eRatingSetup ���� �������� ����� rating�� �ö�;��Ѵ�.
		xmgr_cas_Vmx_GetRatingLevel(&ucChannelRating);
		// vmx client�δ� ������ override�� ��Ų��.
		//xmgr_cas_Vmx_SetOverrideRatingLevel(NULL, ulActionId);

		if(eRatingSetup == eDxRATING_AGE_LOCK_ALL)
		{
			bRatingOK = FALSE;
		}
		else if(eRatingSetup == eDxRATING_VIEW_ALL)
		{ /* View all���������� ������ �� �� �ִ�. */
			bRatingOK = TRUE;
		}
		else
		{ /* �� �ܿ��� ���ؾ� �ϴµ�... */
			if(ucChannelRating > eDxRATING_AGE_18) /* �� �� eDxRATING_AGE_18 �̻� ���� spec �����̹Ƿ� �ɷ�����. */
			{
				bRatingOK = TRUE;
			}
			else if(ucChannelRating > eRatingSetup) /* Content�� rating�� �¾����� ũ�Ƿ� PIN code check �ؾ� ��. */
			{
				bRatingOK = FALSE;
			}
			else
			{ /* Content�� rating�� �¾����� ���ų� �۴�. OK to watch. */
				bRatingOK = TRUE;
			}
		}
	}

	return bRatingOK;
}
#endif

