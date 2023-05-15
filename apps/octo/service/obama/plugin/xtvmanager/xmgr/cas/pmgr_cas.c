/**
	@file     xmgr_sw_update.c
	@brief    file_name & simple comment.

	Description: Applicatuion for searching Services\n
	Module: Application(Layer), Menu(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <octo_common.h>
#include <mgr_common.h>
#include <xmgr_action.h>
#include <xmgr_cas.h>
#include <xmgr_cas_res_str.h>
#include "./local_include/_xmgr_cas.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
//#undef	HxLOG_DOMAIN
//#define	HxLOG_DOMAIN	DOMAIN_TVMGR_SW_UPDATE

#define CASMGR_PARAM_WARNING_REMOVE(arg1, arg2, arg3, arg4, arg5)	\
			{													\
				(void)arg1;										\
				(void)arg2;										\
				(void)arg3;										\
				(void)arg4;				 						\
				(void)arg5;				 						\
			}

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


BUS_Result_t pmgr_cas_CreateSubCasProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	AP_CasResStr_Init();
	AP_CasResStr_PrepareResource("eng");

#if defined(CONFIG_MW_CAS_NAGRA)
	BUS_MGR_Create("xproc_cas_NaMain", APP_CAS_PRIORITY, xproc_cas_NaMain, hAction, p1, p2, p3);
#endif

#if defined(CONFIG_MW_CI)
	BUS_MGR_Create("xproc_cas_CiMain", APP_CAS_PRIORITY, xproc_cas_CiMain, hAction, p1, p2, p3);
#endif

#if defined(CONFIG_MW_CAS_IRDETO)
	BUS_MGR_Create("xproc_cas_IrMain", APP_CAS_PRIORITY, xproc_cas_IrMain, hAction, p1, p2, p3);
#endif

#if defined(CONFIG_MW_CAS_VIACCESS)
	BUS_MGR_Create("AP_CAS_VA_MainProc", APP_CAS_PRIORITY, xporc_cas_VaMain, hAction, p1, p2, p3);
#endif

#if defined(CONFIG_MW_CAS_CONAX)
	BUS_MGR_Create("xproc_cas_CxMain", APP_CAS_PRIORITY, xproc_cas_CxMain, hAction, p1, p2, p3);
#endif

#if defined(CONFIG_MW_CAS_VERIMATRIX_IPTV)
	BUS_MGR_Create("xproc_cas_VmxMain", APP_CAS_PRIORITY, xproc_cas_VmxMain, hAction, p1, p2, p3);
#endif
	CASMGR_PARAM_WARNING_REMOVE(message, hAction, p1, p2, p3);
	return ERR_BUS_MESSAGE_PASS;
}

HERROR pmgr_cas_Init (void)
{
	HERROR		hErr = ERR_FAIL;

#if defined(CONFIG_MW_CAS_NAGRA)
	hErr = xmgr_cas_NaInitialize();
#endif

#if defined(CONFIG_MW_CAS_CONAX)
	hErr = xmgr_cas_CxInitialize();
#endif

#if defined(CONFIG_MW_CAS_IRDETO)
	hErr = xmgr_cas_IrInitialize();
#endif

#if defined(CONFIG_MW_CAS_VIACCESS)
	hErr = xmgr_cas_VaInitialize();
#endif

	return hErr;
}

HERROR pmgr_cas_InitInActionMgr (void)
{
	HERROR		hErr = ERR_FAIL;

#if defined(CONFIG_MW_CAS_VIACCESS)
	hErr = xmgr_cas_VaStart();
#endif

	return hErr;
}

HERROR pmgr_cas_DefaultSetting(void)
{
	HERROR		hErr = ERR_FAIL;

#if defined(CONFIG_MW_CAS_NAGRA)
	hErr = xmgr_cas_NaDefaultSetting();
#endif

#if defined(CONFIG_MW_CAS_CONAX)
	hErr = xmgr_cas_CxDefaultSetting();
#endif

#if defined(CONFIG_MW_CAS_VERIMATRIX_IPTV)
	hErr = xmgr_cas_VmxDefaultSetting();
#endif

	return hErr;
}

HERROR pmgr_cas_TotTdtUpdated(void)
{
#if defined(CONFIG_MW_CAS_NAGRA)
	return xmgr_cas_NaTotTdtUpdated();
#endif

	return ERR_FAIL;
}

HERROR pmgr_cas_GetCaSystemId(Handle_t hAction, HUINT32 *pucCaSysId)
{
	return ERR_FAIL;
}

HERROR pmgr_cas_OnShowProgramCasControl (Handle_t hAction, Handle_t hSvc, HUINT32 *pulMsg, HUINT32 *pulParam1, HUINT32 *pulParam2, HUINT32 *pulParam3)
{
	HERROR		hErr = ERR_FAIL;

	if (NULL == pulMsg)			return ERR_FAIL;
	if (NULL == pulParam1)		return ERR_FAIL;
	if (NULL == pulParam2)		return ERR_FAIL;
	if (NULL == pulParam3)		return ERR_FAIL;

	*pulMsg = 0;	// No Message
	*pulParam1 = *pulParam2 = *pulParam3 = 0;

	return hErr;
}

HERROR pmgr_cas_IsSmartcardRemoved(HUINT8 ucSlotIndex, HBOOL *pbRemoved)
{
	if (NULL == pbRemoved)
		return ERR_FAIL;

#if defined(CONFIG_MW_CAS_NAGRA) && defined(CONFIG_MW_CAS_NAGRA_OP_UPC)
	return xmgr_cas_NaIsScRemoved(ucSlotIndex, pbRemoved);
#else
	return ERR_FAIL;
#endif
}

HBOOL pmgr_cas_NeedCasRatingCheck(void)
{
	HBOOL bNeeded = FALSE;

#if defined(CONFIG_MW_CAS_IRDETO)&& (CONFIG_MW_CAS_NUM_EMBEDDED <= 1)
	bNeeded = TRUE;
#elif defined(CONFIG_MW_CAS_NAGRA_FEATURE_PARENTAL_CONTROL)
	bNeeded = TRUE;
#endif

	return bNeeded;
}

HBOOL pmgr_cas_IsRatingOK(HUINT32 ulActionId, Handle_t hService, DxDeliveryType_e svcDeliveryType, DxRatingAge_e eRatingSetup, DxRatingAge_e eContentRating)
{
#if defined(CONFIG_MW_CAS_IRDETO)&& (CONFIG_MW_CAS_NUM_EMBEDDED <= 1)
	return XMGR_CAS_IrIsRatingOK(ulActionId, hService, svcDeliveryType, eRatingSetup, eContentRating);
#elif defined(CONFIG_MW_CAS_NAGRA_FEATURE_PARENTAL_CONTROL)
	return xmgr_cas_Na_IsRatingOK(ulActionId, eRatingSetup);
#elif defined(CONFIG_MW_CI_PLUS)
	return xmgr_cas_CiPcIsRatingOK(ulActionId, eRatingSetup);
#endif

	HxLOG_Error("Undefined call!! check this function!! \n");
	return TRUE;
}

HBOOL pmgr_cas_NeedCasRatingWaitCheck(void)
{
	HBOOL bNeeded = FALSE;

#if defined(CONFIG_MW_CAS_IRDETO) && (CONFIG_MW_CAS_NUM_EMBEDDED <= 1)
	bNeeded = TRUE;
#elif defined(CONFIG_MW_CAS_NAGRA_FEATURE_PARENTAL_CONTROL)
	bNeeded = TRUE;
#elif defined(CONFIG_MW_CAS_VERIMATRIX_IPTV)
	bNeeded = TRUE;
#endif

	return bNeeded;
}

HBOOL pmgr_cas_IsWaitParentalRatingState(MgrPg_Info_t *pstPgInfo)
{
#if defined(CONFIG_MW_CAS_IRDETO) && (CONFIG_MW_CAS_NUM_EMBEDDED <= 1)
	return XMGR_CAS_IrIsWaitParentalRatingState(pstPgInfo);
#elif defined(CONFIG_MW_CAS_NAGRA_FEATURE_PARENTAL_CONTROL)
	return xmgr_cas_Na_IsWaitParentalRatingState(pstPgInfo);
#elif defined(CONFIG_MW_CAS_VERIMATRIX_IPTV)
	return xmgr_cas_Vmx_IsWaitParentalRatingState(pstPgInfo);
#endif

	HxLOG_Error("Undefined call!! check this function!! \n");
	return FALSE;
}

HERROR pmgr_cas_GetRatingFromCasMsg(void *pvCasMsg, HUINT32 *pulRating)
{
#if defined(CONFIG_MW_CAS_VERIMATRIX_IPTV)
	return xmgr_cas_Vmx_GetRatingLevelFromCasMsg(pvCasMsg, pulRating);
#endif

	HxLOG_Error("Undefined case. Check this function. \n");
	return ERR_FAIL;
}

HERROR pmgr_cas_IsSameCasIdInSc(HUINT8 ucSlotId, HUINT16 usCaSysId, HUINT8 *pbSame)
{
#if defined(CONFIG_OP_SES) && defined(CONFIG_MW_CAS_NAGRA)
	return xmgr_cas_NaIsSameCasIdInSc(ucSlotId, usCaSysId, pbSame);
#endif

	HxLOG_Error("Undefined case. Check this function. \n");
	return ERR_FAIL;
}

/********************************************************************
* For Debugging
********************************************************************/
#if defined (CONFIG_DEBUG)
void pmgr_cas_CmdInit(HxCMD_Handle_t hCmdHandle)
{

#if defined(CONFIG_MW_CI)
#endif

#if defined(CONFIG_MW_CAS_NAGRA)
	xmgr_cas_NaCmdInit(hCmdHandle);
#endif

#if defined(CONFIG_MW_CAS_IRDETO)
#endif

#if defined(CONFIG_MW_CAS_IRDETO_UCAS)
#endif

#if defined(CONFIG_MW_CAS_CONAX)
	xmgr_cas_CxCmdInit(hCmdHandle);
#endif

#if defined(CONFIG_MW_CAS_VIACCESS)
#endif

#if defined(CONFIG_MW_CAS_VERIMATRIX_IPTV)
	xmgr_cas_VmxIptvCmd(hCmdHandle);
#endif
}

#endif



