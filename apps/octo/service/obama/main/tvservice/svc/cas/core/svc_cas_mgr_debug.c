/**
	@file     _svc_cas_mgr_debug.c
	@brief    _svc_cas_mgr_debug.c (CAS Service)

	Description: . \n
	Module: MW/CAS \n
	Remarks :\n

	Copyright (c) 2008 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <_svc_cas_mgr_common.h>
#include <_svc_cas_mgr_main.h>
#include <_svc_cas_mgr_debug.h>
#if 0
#include <pal_ci.h>
#include <pal_dsc.h>
#include <pal_sc.h>

#include <linkedlist.h>
#endif

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

const HINT8 *svc_cas_DbgMakeStrChType(SvcCas_ChannelType_e eType)
{
	switch (eType)
	{
		ENUM_TO_STR(eCasChannel_Unknown);
		ENUM_TO_STR(eCasChannel_FTA);
		ENUM_TO_STR(eCasChannel_Scramble);
	}
	return ("Unknown");
}

const HINT8 * svc_cas_DbgMakeStrContextState(SvcCas_State_e eState)
{
	switch (eState)
	{
		ENUM_TO_STR(eCAS_State_Idle);
		ENUM_TO_STR(eCAS_State_Started);
		ENUM_TO_STR(eCAS_State_SetPid);
		ENUM_TO_STR(eCAS_State_subCasNotSelected);
		ENUM_TO_STR(eCAS_State_subCasSelected);
		ENUM_TO_STR(eCAS_State_CasOK);
		ENUM_TO_STR(eCAS_State_CasFail);
		ENUM_TO_STR(eCAS_State_Max);
	}
	return ("Unknown");
}


const HINT8 *svc_cas_DbgMakeStrMmiType(SvcCas_MmiDataType_e eType)
{
	switch (eType)
	{
		ENUM_TO_STR(eCasMmiType_None);
		ENUM_TO_STR(eCasMmiType_Open);
		ENUM_TO_STR(eCasMmiType_Close);
		ENUM_TO_STR(eCasMmiType_Menu);
		ENUM_TO_STR(eCasMmiType_List);
		ENUM_TO_STR(eCasMmiType_Text);
		ENUM_TO_STR(eCasMmiType_Enq);
		ENUM_TO_STR(eCasMmiType_Max);
	}
	return ("Unknown");
}

const HINT8 *svc_cas_DbgMakeStrCasEvtType(SvcCas_EvtType_e eType)
{
	switch (eType)
	{
		ENUM_TO_STR(eCAS_EVT_TYPE_CIMmi);
		ENUM_TO_STR(eCAS_EVT_TYPE_Nagra);
		ENUM_TO_STR(eCAS_EVT_TYPE_IR);
		ENUM_TO_STR(eCAS_EVT_TYPE_Max);
	}
	return ("Unknown");
}

const HINT8 *svc_cas_DbgMakeStrMatchLevel(SvcCas_MatchLevel_e eType)
{
	switch (eType)
	{
		ENUM_TO_STR(eCAS_MATCH_None);
		ENUM_TO_STR(eCAS_MATCH_Prog);
		ENUM_TO_STR(eCAS_MATCH_Es);
		ENUM_TO_STR(eCAS_MATCH_Max);
	}
	return ("Unknown");
}

const HINT8 * svc_cas_DbgMakeStrDescrambleState(SvcCas_DscState_e eState)
{
	switch (eState)
	{
		ENUM_TO_STR(eCAS_DscState_Unknown);
		ENUM_TO_STR(eCAS_DscState_Scrambled);
		ENUM_TO_STR(eCAS_DscState_Descrambled);
		ENUM_TO_STR(eCAS_DscState_Max);
	}
	return ("Unknown");
}

#if 0
//#ifdef CONFIG_MW_CI
const HINT8 *svc_cas_DbgMakeStrMmiCmd(MMI_REQ cmd)
{
	switch (cmd)
	{
		ENUM_TO_STR(MMI_OPEN);
		ENUM_TO_STR(MMI_CLOSE);
		ENUM_TO_STR(MMI_TEXT);
		ENUM_TO_STR(MMI_ENQ);
		ENUM_TO_STR(MMI_MENU);
		ENUM_TO_STR(MMI_LIST);
		ENUM_TO_STR(MMI_ERROR);
	}
	return ("Unknown");
}
#endif

#if 0

HERROR svc_cas_DbgPrintPmtInfoByActionId(HUINT32 ulActionId)
{
	int i = 0;
	SvcCas_PmtInfo_t *pstPmtInfo;
	SvcCas_Context_t *pContext = NULL;

	if(ulActionId >= CAS_MAX_NUM_OF_ACTION)
	{
		return ERR_FAIL;
	}

	pContext = svc_cas_MgrGetActionContextByActionId(ulActionId);
	if(pContext == NULL || pContext->state == eCAS_State_Idle)
	{
		HxLOG_Print("%d-th context state is (%s) \n", ulActionId, svc_cas_DbgMakeStrContextState(pContext->state));
		return ERR_FAIL;
	}

	pstPmtInfo = &pContext->stPmtInfo;
	if(pstPmtInfo == NULL)
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}

	HxLOG_Print("\n\t=============== pmt info ===============\n\n");

	HxLOG_Print("\tsection state			(%d) \n", pstPmtInfo->ulSectionState);
	HxLOG_Print("\tservice id			(0x%x) \n", pstPmtInfo->usServiceId);
	HxLOG_Print("\tpcr pid				(0x%x) \n", pstPmtInfo->usPcrPid);
	HxLOG_Print("\tversion number			(%d) \n", pstPmtInfo->ucVerNum);
	HxLOG_Print("\tcurrent next indicator		(%d) \n", pstPmtInfo->bCurNextInd);

	HxLOG_Print("\tcrc value			(0x%x) \n", pstPmtInfo->ulCrc);
	HxLOG_Print("\tflag of global ca descriptor	(%d) \n", pstPmtInfo->bProgramCaFlag);
	HxLOG_Print("\tnumber of global ca descriptor	(%d) \n", pstPmtInfo->ulNumOfProgramCaDescriptor);

	for(i=0; i<pstPmtInfo->ulNumOfProgramCaDescriptor; i++)
	{
		HxLOG_Print("\t\tglobal ca descriptor[%d], ca sys id (0x%x), ca pid (0x%x) \n", i, pstPmtInfo->stProgramCaDescriptor[i].usCaSysId, pstPmtInfo->stProgramCaDescriptor[i].usCaPid);
	}

	svc_cas_DbgPrintPmtElementInfo(pstPmtInfo);

	HxLOG_Print("\n\t============ end of pmt info ===========\n");

	return ERR_OK;
}
#endif

HERROR svc_cas_DbgPrintPMTRawData(void)
{
	return ERR_OK;
}

HERROR svc_cas_DbgPrintCATRawData(void)
{
	return ERR_OK;
}

HERROR svc_cas_DbgPrintServiceElement(void)
{
	return ERR_OK;
}

HERROR svc_cas_DbgPrintCaDescInfo(void)
{
	return ERR_OK;
}

void svc_cas_DbgPrintMmiStatus(SvcCas_MmiData_t *pData)
{
	int i = 0;
	SvcCas_MmiMenuList_t *pMenuData = NULL;
	SvcCas_MmiEnq_t *pEnqData = NULL;

	if(pData == NULL)
	{
		HxLOG_Critical("\n\n");
		return;
	}

	HxLOG_Print("\n======== print mmi data ==============\n");

	HxLOG_Print("version 			(%d) \n", pData->ulVersion);
	HxLOG_Print("slot id 			(%d) \n", pData->usSlotId);
	HxLOG_Print("mmi handle 			(0x%x) \n", pData->hMmi);
	HxLOG_Print("data type 			(%s) \n", svc_cas_DbgMakeStrMmiType(pData->eDataType));

	if(pData->pvMmiData == NULL)
	{
		HxLOG_Print("mmi data is null \n");
		return;
	}

	switch(pData->eDataType)
	{
	case eCasMmiType_Menu:
	case eCasMmiType_List:
		pMenuData = (SvcCas_MmiMenuList_t *)pData->pvMmiData;
		HxLOG_Print("\t [%s] \n", pMenuData->pszTitle);
		HxLOG_Print("\t [%s] \n", pMenuData->pszSubTitle);
		for (i = 0; i < pMenuData->ulNumOfItem; i++)
			HxLOG_Print("\t\t [%02d:%s] \n", i+1, pMenuData->ppszItem[i]);
		HxLOG_Print("\t [%s] \n", pMenuData->pszBottom);
		break;

	case eCasMmiType_Text:
		HxLOG_Print("\t [%s] \n", (HUINT8 *)pData->pvMmiData);
		break;

	case eCasMmiType_Enq:
		pEnqData = (SvcCas_MmiEnq_t *)pData->pvMmiData;
		HxLOG_Print("string (%s) \n", pEnqData->pszPrompt);
		HxLOG_Print("answer length (%d) \n", pEnqData->ucAnswerLen);
		HxLOG_Print("blind flag (%d) \n", pEnqData->ucBlindOnOff);
		break;

	default:
		break;
	}

	HxLOG_Print("\n======== end of mmi data ==============\n\n");

	return;
}

#if 0
void svc_cas_DbgPrintPidInfo(SvcCas_SetPidInfo_t *pstCaDscInfo)
{
	int i = 0;

	if(pstCaDscInfo == NULL)
	{
		return;
	}

	HxLOG_Print("\n============== Pid Info ============== \n");

	HxLOG_Print("number of es pid 			: %d \n", pstCaDscInfo->ulPidNum);
	for(i=0; i<pstCaDscInfo->ulPidNum; i++)
	{
		HxLOG_Print("[%d]-th, Type [%s], pid (0x%x) \n", i, svc_cas_DbgMakeStrEsType(pstCaDscInfo->stPidInfo[i].esType), pstCaDscInfo->stPidInfo[i].usPid);
	}
	HxLOG_Print("\n\n");

	return;
}
#endif


HERROR svc_cas_DbgPrintNumOfCaInst(void)
{
	HxLOG_Error(" number of ca instance (%d) \n", CAS_MAX_NUM_OF_INSTANCE);

	return ERR_OK;
}

/*********************** End of File ******************************/
