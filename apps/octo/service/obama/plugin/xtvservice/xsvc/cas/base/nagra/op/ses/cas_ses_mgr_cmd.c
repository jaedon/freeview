/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <namgr_main.h>
#include "cas_ses_mgr.h"
#include <_svc_cas_sub_svc_api.h>

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
#if (COMMAND_NAGRA_SES_TEST == 1)
static HERROR xmgr_cas_Naaps_CmdTest_Bdc(Handle_t hAction, NaCasAPSCmdTest_t *pstCmdTest)
{
	SvcCas_MsgType_e			eMsgType = eCAS_MSG_TYPE_Command;
	DxCAS_GroupId_e			eCasId = eDxCAS_GROUPID_NA;
	SvcCas_ShowType_e 			eShowType = eCasShowType_Both;

	static NaCmdData_t 		Message;
	static NaBdcItem_t		bdcMessage;

	bdcMessage.eBdcType = pstCmdTest->ucBdcType;
	bdcMessage.eBdcCmd = pstCmdTest->ucBdcCmd;
	bdcMessage.ucNumOfLang = 1;
	strncpy(bdcMessage.stBdcContent[0].xLang, "eng", 3);
#if 1
	strncpy(bdcMessage.stBdcContent[0].ucText, pstCmdTest->strBdcMessage, MAX_BDC_STRING_SIZE-1);
#else
	strncpy(bdcMessage.stBdcContent[0].ucText, "   nagra1\r nagra2    \n  nagra3  ", MAX_BDC_STRING_SIZE-1);
#endif
	bdcMessage.stBdcContent[0].ulNumBytes = strlen(bdcMessage.stBdcContent[0].ucText);

	Message.hAction = svc_cas_SubSvcRmGetActionId(hAction);
	Message.eCmdType = eCmd_Bdc;
	Message.item.pstBdcItem = &bdcMessage;

	if (svc_cas_MgrSubUpdateCasMessage(hAction, eMsgType, eCasId, (void*)&Message, eShowType) != ERR_OK)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

static HERROR xmgr_cas_Naaps_CmdTest_AlarmMsg(Handle_t hAction, NaCasAPSCmdTest_t *pstCmdTest)
{
	SvcCas_MsgType_e			eMsgType = eCAS_MSG_TYPE_Fail;
	DxCAS_GroupId_e			eCasId = eDxCAS_GROUPID_NA;
	SvcCas_ShowType_e 			eShowType = eCasShowType_Both;

	static NaAlarmData_t	alarmMessage;
	static NaScAlarmItem_t	SCalarmMessage;
	static NaAccessAlarmItem_t	ACalarmMessage;

	alarmMessage.hAction = svc_cas_SubSvcRmGetActionId(hAction);
	alarmMessage.eAlarmType = pstCmdTest->ucAlarmType;
	alarmMessage.ucNumOfAccessAlarms = 0;
	alarmMessage.ucNumOfScAlarms = 1;


	if (alarmMessage.eAlarmType == eAlarm_Access)
	{

		ACalarmMessage.bChanged = 0;
		ACalarmMessage.eAlarm = pstCmdTest->ulAlarmMsg;
		ACalarmMessage.ucElemType = 0;
		ACalarmMessage.ulSlotId = 1;
		ACalarmMessage.usSvcId = 0;

		alarmMessage.pstScAlarmItems = &ACalarmMessage;
	}
	else if (alarmMessage.eAlarmType == eAlarm_Smartcard)
	{
		SCalarmMessage.bChanged = 0;
		SCalarmMessage.eAlarm = pstCmdTest->ulAlarmMsg;
		SCalarmMessage.ulSlotId = 1;

		alarmMessage.pstScAlarmItems = &SCalarmMessage;
	}

	if (CASMGR_APS_UpdateCasMessage(hAction, eMsgType, eCasId, (void*)&alarmMessage, eShowType) != ERR_OK)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}
#endif

HERROR	CASMGR_APS_CMD_Test(Handle_t hAction, NaCasAPSCmdTest_t *pstCmdTest)
{
#if (COMMAND_NAGRA_SES_TEST == 1)
	if (pstCmdTest->ucTestType == 0)
	{
		xmgr_cas_Naaps_CmdTest_Bdc(hAction, pstCmdTest);
	}
	else if (pstCmdTest->ucTestType == 1)
	{
		xmgr_cas_Naaps_CmdTest_AlarmMsg(hAction, pstCmdTest);
	}
#endif
	return ERR_OK;
}

/*********************** End of File ******************************/

