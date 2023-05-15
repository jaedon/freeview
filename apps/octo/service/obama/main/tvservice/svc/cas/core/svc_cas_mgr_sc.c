/**
	@file     _svc_cas_mgr_sc.c
	@brief    _svc_cas_mgr_sc.c

	Description:
	Module: MW/CAS \n
	Remarks :\n

	Copyright (c) 2009 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <util.h>

#include <_svc_cas_mgr_common.h>
#include <_svc_cas_mgr_svc_port.h>
#include <_svc_cas_mgr_pal_port.h>
#include <_svc_cas_mgr_main.h>
#include <_svc_cas_mgr_sc.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/



/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC unsigned long		s_ulCasSCDataSemId;

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/



/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

HERROR svc_cas_ScInitModule(void)
{
	HINT32 nResult = 0;

	HxLOG_Info(" \n");

	nResult = VK_SEM_Create(&s_ulCasSCDataSemId, "CasSC", VK_SUSPENDTYPE_PRIORITY);
	if(nResult != ERR_OK)
	{
		HxLOG_Error("Failure in Semaphore Creation \n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR svc_cas_ScSendCmd2MgrTask(DxCAS_GroupId_e eGroupId, HUINT32 ulCommand, HUINT32 ulMsgKind, HUINT8* pDataToSend)
{
	SvcCas_Message_t 	casMsg;
	HUINT32 		nResult = 0;

	VK_SEM_Get(s_ulCasSCDataSemId);
	HxSTD_memset(&casMsg, 0x00, sizeof(SvcCas_Message_t));

	casMsg.checkSCReply.pstReplyData = (SvcCas_SmartcardReplyData_t*)OxCAS_Malloc(sizeof(SvcCas_SmartcardReplyData_t));
	if(casMsg.checkSCReply.pstReplyData== NULL)
	{
		VK_SEM_Release(s_ulCasSCDataSemId);
		return ERR_FAIL;
	}

	HxSTD_memset(casMsg.checkSCReply.pstReplyData, 0x00, sizeof(SvcCas_SmartcardReplyData_t));

	casMsg.checkSCReply.command = ulCommand;
	casMsg.checkSCReply.usCASGroup = eGroupId;
	casMsg.checkSCReply.pstReplyData->ulMsg= ulMsgKind;
	casMsg.checkSCReply.pstReplyData->pvData = pDataToSend;

	HxLOG_Print("##### svc_cas_ScSendCmd2MgrTask is sending message --> 0x%08x\n", casMsg.checkSCReply.pstReplyData->ulMsg);
	nResult = VK_MSG_Send(svc_cas_MgrGetMsgQueueId(), &casMsg, sizeof(SvcCas_Message_t));

	VK_SEM_Release(s_ulCasSCDataSemId);
	if(nResult != ERR_OK)
	{
		HxLOG_Error("Failure in message sending. \n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

/*********************** End of File ******************************/
