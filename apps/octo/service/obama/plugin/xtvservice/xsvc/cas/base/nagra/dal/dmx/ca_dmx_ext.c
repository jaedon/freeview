/**************************************************************************************************/
#define ________CA_DMX_EXIT_Private_Include________________________________________________
/**************************************************************************************************/
#include "vkernel.h"
#include "na_glob.h"

#include <pal_pipe.h>
#include "_svc_cas_mgr_main.h"
#include "_svc_cas_mgr_svc_port.h"
#include "_svc_cas_mgr_common.h"

//#include "pal_sef.h"
//#include <_svc_cas_mgr_common.h>


/**************************************************************************************************/
#define ________CA_DMX_EXIT_Golbal_Value___________________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________CA_DMX_EXIT_Private_Define_________________________________________________
/**************************************************************************************************/
#if defined(CONFIG_MW_CAS_NAGRA_DALTEST)
#define DmxExtLogTrace(message) 	//(HLIB_CMD_Printf(C_CODE_F_PURPLE"[CA DMX EXT](%s:%d)"C_CODE_RESET, __FUNCTION__, __LINE__), (HLIB_CMD_Printf message))
#define DmxExtLogError(message) 	//(HLIB_CMD_Printf(C_CODE_F_RED"[CA DMX EXT](%s:%d)"C_CODE_RESET, __FUNCTION__, __LINE__), (HLIB_CMD_Printf message))
#define DmxExtLogWarning(message) 	//(HLIB_CMD_Printf(C_CODE_F_RED_B_BLACK"[CA DMX EXT](%s:%d)"C_CODE_RESET, __FUNCTION__, __LINE__), (HLIB_CMD_Printf message))


#if defined (HxLOG_Print)
#undef	HxLOG_Print
#define HxLOG_Print(...)	DmxExtLogTrace((__VA_ARGS__))
#endif

#if defined (HxLOG_Error)
#undef	HxLOG_Error
#define HxLOG_Error(...)	DmxExtLogError((__VA_ARGS__))
#endif

#if defined (HxLOG_Warning)
#undef	HxLOG_Warning
#define HxLOG_Warning(...)	DmxExtLogWarning((__VA_ARGS__))
#endif

#endif


#if defined(CONFIG_MW_MM_PVR) && !defined(CONFIG_MW_CAS_NAGRA_DALTEST)
extern HERROR CA_NA_PsiGetEmmAction(TUnsignedInt16 usUniqueId, Handle_t *phAction);
#endif

/**************************************************************************************************/
#define ________CA_DMX_EXIT_External_Functions_Body________________________________________
/**************************************************************************************************/
HERROR CA_NA_DmxStartFilter(TUnsignedInt32 hAction, TUnsignedInt16 usPid, TUnsignedInt32 ulTimeOut, TUnsignedInt8 ucEvtMode, TUnsignedInt8 ucFilterValue[], TUnsignedInt8 ucMaskValue[], TUnsignedInt16 *pusFilterId)
{
	TSignedInt32			nDmxId = 0;
	TUnsignedInt32			ulErrCode;
	TUnsignedInt16			usTsUniqueId;
	SvcCas_FilterParam_t	stFilterParam;
	SvcCas_Context_t 		*pContext = NULL;

#if defined(CONFIG_MW_MM_PVR) && !defined(CONFIG_MW_CAS_NAGRA_DALTEST)
	if (svc_cas_SubSvcRmGetActionId((unsigned int)hAction) == EMM_HANDLE_ID)
	{
		/* EMM Filtering */
		TUnsignedInt32 hEmmAction;
		ulErrCode = CA_NA_PsiGetEmmAction((TUnsignedInt16)(hAction & 0xFFFF), &hEmmAction);
		if (ulErrCode != ERR_OK)
		{
			HxLOG_Error("CA_NA_PsiGetEmmAction : error:0x%x, action:0x%x\n", (unsigned int)ulErrCode, (unsigned int)hEmmAction);
			return ulErrCode;
		}

		hAction = hEmmAction;
		HxLOG_Print("Get EMM Action for EMM Filtering:0x%x\n", hEmmAction);
	}
#endif

	pContext = svc_cas_MgrSubGetActionContextByActionId(svc_cas_SubSvcRmGetActionId(hAction));
	if (hAction == HANDLE_NULL || pContext == NULL)
	{
		HxLOG_Error("Handle is invalid\r\n");
		return ERR_FAIL;
	}

	usTsUniqueId = pContext->usUniqueId;
	HxLOG_Print("Action[0x%x], Pid[0x%x], Timeout[%d], EvtMode[%d], UniqueId [%x]\n", (unsigned int)hAction, usPid, (int)ulTimeOut, ucEvtMode, usTsUniqueId);

#if 0//defined(CONFIG_MW_CAS_NAGRA_DALTEST)
	nDmxId = 0;
#else
	ulErrCode = svc_cas_SubSvcRmGetResourceId(svc_cas_SubSvcRmGetActionId((unsigned int)hAction), eRxRSCTYPE_DEMUX, (int*)&nDmxId);
	if (ulErrCode != ERR_OK)
	{
		HxLOG_Error("svc_cas_SubSvcRmGetResourceId error:0x%x, action:0x%x\n", (unsigned int)ulErrCode, (unsigned int)hAction);
		return ulErrCode;
	}
#endif

	ulErrCode = svc_cas_SvcPortSiSetDefaultRetrieveParam(&stFilterParam, (unsigned int)hAction, usTsUniqueId, nDmxId);
	if(ulErrCode != ERR_OK)
	{
		HxLOG_Critical("svc_cas_SvcPortSiSetDefaultRetrieveParam() error!!!\n");
	}

	if (ucEvtMode & SI_EVENT_CONT_MODE)
	{
		stFilterParam.ucMode = eCAS_SI_FILTER_MODE_Continue;
		stFilterParam.ucTimeoutMode = eCas_SI_FILTER_Timeout_Repeat;
	}

	stFilterParam.bTableFilter = FALSE;
	if (ucEvtMode & 0x80)
		stFilterParam.bCrc = TRUE;
	else
		stFilterParam.bCrc = FALSE;
	stFilterParam.bUseVerNum = FALSE;
	stFilterParam.usPid = usPid;

	HxLOG_Print("Filter info : Mode[0x%x], TableFilter [0x%x], Crc [%d], Use_Ver_Num [%d], Pid [%x], Dmx [%d]\n", stFilterParam.ucMode,
																		stFilterParam.bTableFilter, stFilterParam.bCrc, stFilterParam.bUseVerNum, stFilterParam.usPid, (int)nDmxId);

	ulErrCode = svc_cas_SvcPortSiRetrieveSectionBase(&stFilterParam,
												ucFilterValue[0],
												ucFilterValue,
												ucMaskValue,
												8,
												ulTimeOut,
												(void*)CA_NA_DmxCallbackTable,
												NULL,
												pusFilterId);
	if (ulErrCode != ERR_OK)
	{
		HxLOG_Error("svc_cas_SvcPortSiRetrieveSectionBase return ERROR (%x)\n", (unsigned int)ulErrCode);
		return ulErrCode;
	}

	HxLOG_Print("Filter_Id (0x%x)\n", *pusFilterId);
	return ERR_OK;
}


HERROR CA_NA_DmxStopFilter(TUnsignedInt16 usFilterId)
{
	HxLOG_Print("Filter_Id (0x%x)\n", usFilterId);

	return svc_cas_SvcPortSiStopOneSection(usFilterId);
}


