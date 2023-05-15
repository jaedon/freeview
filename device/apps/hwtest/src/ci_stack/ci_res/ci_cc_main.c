/***************************************************************
* $Workfile:   ci_cc_main.c  $
* $Modtime:
* Auther : hmkim / dslee for X.509 functions
*
* Desc : Content Control (CI+) Main
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
****************************************************************/


/***************************************************************
* #ifdef statements and include ////////////////////////////////
***************************************************************/

#include "ci_cc_main.h"
#include "ci_cc_cipher.h"
#include "ci_cc_x509.h"
#include <cb_list.h>


/***************************************************************
* debugging ////////////////////////////////////////////////////
***************************************************************/

#if 0 // #if defined(CONFIG_DEBUG) && defined(CONFIG_LF7300) // Ziggo 향 LF7300 은 휴맥스 최초 CI+ 모델이다. CI+ 동작 테스트가 주요 관심사이므로 LF7300 에서는 기본 설정한다.
#define _CI_DEBUG_CC_DATA_
#else
//#define _CI_DEBUG_CC_DATA_
#endif

//#define _CI_DEBUG_CC_SELF_CHECKING_
//#define _CI_DEBUG_CC_TIME_ISSUE_
//#define _CI_DEBUG_CC_PROC_TIME_
//#define _CI_DEBUG_CC_FUNC_TIME_


/***************************************************************
* local definition /////////////////////////////////////////////
***************************************************************/

/* host license constants data downloading related (on booting process) */
#define CI_CC_LICENSE_DATA_DOWN_CHECK_MAX_COUNT	1000

/* 배타적 선언 필요 : see USE_SUB_TASK_TO_CALCULATE_KEYS also */
#define CI_CC_CalculateCckAndSetDescramblerBeforeSync
//#define CI_CC_CalculateCckBeforeSyncAndSetDescramblerOnSync

/* verify received sac_message() - 스펙대로 하면 1 이어야 하나 퍼포먼스가 안좋다면 0 사용. 단, 인증 받을 때는 반드시 1 로 셋팅해야한다!! */
#define CI_CC_VerifyReceivedSacMessage	(0)


/***************************************************************
* CC static / extern variables and structures //////////////////
***************************************************************/

/* host license constants */
static CI_HOST_LICENSE_CONST s_stCiHostLicenseConst;

/* authentication contexts */
/* static */ CI_AUTH_CTXs s_stCiAuthCtxs[CI_MAX_NUM_OF_CAM];

/* CC instance */
static CI_CC_INSTANCE	s_aCiCc[MAX_CI_CC_SS];
static unsigned char	s_ucCiCcCnt;
static HBOOL			s_bCiCcInitSuccess = 0;

/* CC message queue id and task id */
static unsigned long	s_ulCiCcMsgQId;
static unsigned long	s_ulCiCcTaskId;
#ifdef USE_SUB_TASK_TO_CALCULATE_KEYS
static unsigned long	s_ulCiCcSubMsgQId;
static unsigned long	s_ulCiCcSubTaskId;
#endif

/* CC status or error reporting */
extern CALLBACK_HANDLE	g_hCbNotifyCcReport;
static CI_NOTIFY_CC_REPORT_FUNC s_pfnNotifyCcReport;
static CIPLUS_CcReportInfo_t s_stCcReportInfo;

/* CC debugging */
#ifdef _CI_DEBUG_CC_TIME_ISSUE_		// 여기 있는 변수들은 2 slot 고려 안되어 있는 것임. 단지 디버깅 용도로만 사용할 것.
	static HBOOL	s_bCcProcBusy = 0;
	#ifdef USE_SUB_TASK_TO_CALCULATE_KEYS
	static HBOOL	s_bCalculateSacDone = 0;
	static HBOOL	s_bCalculateCckDone = 0;
	#endif
#endif
#ifdef _CI_DEBUG_CC_PROC_TIME_
static unsigned long s_ulCcProcStartTime, s_ulCcProcEndTime;
#endif
#ifdef _CI_DEBUG_CC_FUNC_TIME_
static unsigned long s_ulCcFuncStartTime, s_ulCcFuncEndTime;
#endif


/***************************************************************
* CC static function prototypes ////////////////////////////////
***************************************************************/

static void CiCc_Task(void);
#ifdef USE_SUB_TASK_TO_CALCULATE_KEYS
static void CiCc_SubTask(void);
#endif
static void CiCc_CredentialsErrorTimerCb(unsigned long ulTimerId, void *pvParam);
static int CiCc_Proc(unsigned short usSsNo, unsigned long ulApduTag, unsigned char *pucApduData, unsigned long ulApduLen);

static int CiCc_OpenConfirm(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen);
static int CiCc_DataConfirm(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen, unsigned char ucSacFlag);
static int CiCc_SyncConfirm(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen, unsigned char ucSacFlag);

static void CiCc_VerifyAndSaveSendData(CI_CC_INSTANCE *hCc, unsigned char ucSendDataId, unsigned short usSendDataLen, unsigned char *pucSendData);
static void CiCc_MakeReqDataAndDoSomething(CI_CC_INSTANCE *hCc, unsigned char ucReqDataId, unsigned short *pusReqDataLen, unsigned char **ppucReqData);

static CI_ARC_ERR CiCc_GenerateSignature(CI_CC_INSTANCE *hCc, unsigned char *pucSignature);
static CI_ARC_ERR CiCc_VerifyCamAndSaveInfo(CI_CC_INSTANCE *hCc);

static int CiCc_LoadAkh(CI_CC_INSTANCE *hCc, unsigned char *pucAkh);
static CI_ARC_ERR CiCc_GenerateHostNonVolatileKeys(CI_CC_INSTANCE *hCc);

#ifdef USE_SUB_TASK_TO_CALCULATE_KEYS
static CI_ARC_ERR CiCc_CalculateSacKeysOnSubTask(CI_CC_INSTANCE *hCc);
static CI_ARC_ERR CiCc_CalculateCckAndSetDescramblerOnSubTask(CI_CC_INSTANCE *hCc, HBOOL bDoCalculateCck, HBOOL bDoSetDescrambler);
#endif
static CI_ARC_ERR CiCc_CalculateSacKeys(CI_CC_INSTANCE *hCc);
static CI_ARC_ERR CiCc_CalculateCckAndSetDescrambler(CI_CC_INSTANCE *hCc, HBOOL bDoCalculateCck, HBOOL bDoSetDescrambler);

static void CiCc_GenerateUriConfirm(CI_CC_INSTANCE *hCc, unsigned char *pucUriConfirm);
static void CiCc_UriDoCopyCtrl(CI_CC_INSTANCE *hCc, unsigned char ucMode);
static void CiCc_UriTimerCallback(unsigned long ulTimerId, void *pvParam);

static void CiCc_DoActionForArcErr(CI_CC_INSTANCE *hCc, CI_ARC_ERR enArcErr);

static CI_AUTH_CTX * CiCc_GetAuthContext (CI_SLOT_ID usSlotId, int index);
static int CiCc_PutAuthContext (CI_SLOT_ID usSlotId, int index, CI_AUTH_CTX *pstCiAuthCtx);

static int CiCc_AddInst(unsigned short usSsNo);
static int CiCc_DeleteInst(unsigned short usSsNo);
static CI_CC_INSTANCE * CiCc_GetNewInst(void);
static CI_CC_INSTANCE * CiCc_GetInst(unsigned short usSsNo);


/***************************************************************
* CC functions /////////////////////////////////////////////////
***************************************************************/

int CiCc_Init(void)
{
	VK_memset(s_aCiCc, 0, sizeof(CI_CC_INSTANCE) * MAX_CI_CC_SS);
	VK_memset(&s_stCiHostLicenseConst, 0, sizeof(CI_HOST_LICENSE_CONST));
	VK_memset(s_stCiAuthCtxs, 0, sizeof(CI_AUTH_CTXs) * CI_MAX_NUM_OF_CAM);

	s_ucCiCcCnt = 0;

	// create message queue and task
	if (VK_MSG_Create(PAL_CI_CC_MSG_Q_SIZE, sizeof(CI_CC_TASK_MSG), "qCICC", &s_ulCiCcMsgQId, VK_SUSPENDTYPE_PRIORITY))
	{
		PrintErr(("[CICC]---> CiCc_Init()---> Error : VK_MSG_Create() !!!\n"));
		return -1;
	}
	#ifdef USE_SUB_TASK_TO_CALCULATE_KEYS
	if (VK_MSG_Create(PAL_CI_CC_MSG_Q_SIZE, sizeof(CI_CC_SUB_TASK_MSG), "qCICCSUB", &s_ulCiCcSubMsgQId, VK_SUSPENDTYPE_PRIORITY))
	{
		PrintErr(("[CICC]---> CiCc_Init()---> Error : VK_MSG_Create() for sub !!!\n"));
		return -1;
	}
	#endif
	if (VK_TASK_Create((void*)CiCc_Task, PAL_CI_CC_MAIN_TASK_PRIORITY, PAL_CI_CC_TASK_STACK_SIZE, "tCICC", NULL, &s_ulCiCcTaskId, 0))
	{
		PrintErr(("[CICC]---> CiCc_Init()---> Error : VK_TASK_Create() !!!\n"));
		return -1;
	}
	#ifdef USE_SUB_TASK_TO_CALCULATE_KEYS
	if (VK_TASK_Create((void*)CiCc_SubTask, PAL_CI_CC_SUB_TASK_PRIORITY, PAL_CI_CC_TASK_STACK_SIZE, "tCICCSUB", NULL, &s_ulCiCcSubTaskId, 0))
	{
		PrintErr(("[CICC]---> CiCc_Init()---> Error : VK_TASK_Create() for sub !!!\n"));
		return -1;
	}
	#endif
	if (VK_TASK_Start(s_ulCiCcTaskId))
	{
		PrintErr(("[CICC]---> CiCc_Init()---> Error : VK_TASK_Start() !!!\n"));
		return -1;
	}
	#ifdef USE_SUB_TASK_TO_CALCULATE_KEYS
	if (VK_TASK_Start(s_ulCiCcSubTaskId))
	{
		PrintErr(("[CICC]---> CiCc_Init()---> Error : VK_TASK_Start() for sub !!!\n"));
		return -1;
	}
	#endif

	return ERR_OK;
}

static void CiCc_Task(void)
{
	CI_CC_TASK_MSG msg;

	// wait until host license constants data downloading task complete
	if (TRUE == CI_MES_IsLicenseDownCbRegSuccess())
	{
		int iCbCheckCount = 0;

		while (FALSE == CI_MES_IsLicenseDownTaskEnd() && iCbCheckCount < CI_CC_LICENSE_DATA_DOWN_CHECK_MAX_COUNT)
		{
			VK_TASK_Sleep(10);
			iCbCheckCount++;
			PrintErr(("[CICC]---> CiCc_Task()---> iCbCheckCount = %d !!!\n", iCbCheckCount));
		}
	}

	// load host license constants
	if (CI_LoadHostLicenseConstants(&s_stCiHostLicenseConst))
	{
		PrintErr(("[CICC]---> CiCc_Task()---> Error : Can't load CI+ host license constants !!!\n"));
		s_stCcReportInfo.eCcReport = eCiPlus_CcReport_CanNotLoadHostLicenseConstants;
		goto ERROR;
	}
	if (s_stCiHostLicenseConst.ucSavedFlag != 1 && s_stCiHostLicenseConst.ucSavedFlag != 2)
	{
		PrintErr(("[CICC]---> CiCc_Task()---> Error : CI+ host license constants not saved !!!\n"));
		s_stCcReportInfo.eCcReport = eCiPlus_CcReport_HostLicenseConstantsNotSaved;
		goto ERROR;
	}

	// load authentication contexts
	if (PAL_CI_GetNumberOfModule() == 1) // 1 slot 모델
	{
		if (CI_LoadAuthContexts(0, &s_stCiAuthCtxs[0]))
		{
			PrintErr(("[CICC]---> CiCc_Task()---> CI+ authentication contexts not initialized. Now it will be initialized...\n"));

			if (CI_InitAuthContexts(0))
			{
				PrintErr(("[CICC]---> CiCc_Task()---> Error : Can't init CI+ authentication contexts for 1st slot !!!\n")); // 이런 상황이 발생하면 곤란. 디버깅 필요!!
			}
		}
	}
	else if (PAL_CI_GetNumberOfModule() == 2) // 2 slot 모델
	{
		if (CI_LoadAuthContexts(0, &s_stCiAuthCtxs[0]))
		{
			PrintErr(("[CICC]---> CiCc_Task()---> CI+ authentication contexts for 1st slot not initialized. Now it will be initialized...\n"));

			if (CI_InitAuthContexts(0))
			{
				PrintErr(("[CICC]---> CiCc_Task()---> Error : Can't init CI+ authentication contexts for 1st slot !!!\n")); // 이런 상황이 발생하면 곤란. 디버깅 필요!!
			}
		}

		if (CI_LoadAuthContexts(1, &s_stCiAuthCtxs[1]))
		{
			PrintErr(("[CICC]---> CiCc_Task()---> CI+ authentication contexts for 2nd slot not initialized. Now it will be initialized...\n"));

			if (CI_InitAuthContexts(1))
			{
				PrintErr(("[CICC]---> CiCc_Task()---> Error : Can't init CI+ authentication contexts for 2nd slot !!!\n")); // 이런 상황이 발생하면 곤란. 디버깅 필요!!
			}
		}
	}
	else
	{
		PrintErr(("[CICC]---> CiCc_Task()---> Error : too many slots !!!\n")); // 코딩 미스일 것이기 때문에 이런 상황을 대응하여 에러 핸들링하지는 않겠다...
	}

	s_bCiCcInitSuccess = 1;

	while (1)
	{
		if (VK_MSG_Receive(s_ulCiCcMsgQId, &msg, sizeof(CI_CC_TASK_MSG)) != VK_OK)
		{
			PrintErr(("[CICC]---> CiCc_Task()---> Error : fail to get msg !!!\n"));
			continue;
		}

		if (CiCc_Proc(msg.usSsNo, msg.ulApduTag, msg.pucApduData, msg.ulApduLen))
		{
			PrintErr(("[CICC]---> CiCc_Task()---> Error : CiCc_Proc(%d, 0x%lx) !!!\n", msg.usSsNo, msg.ulApduTag));
		}

		if (msg.pucApduData)
			CA_MEM_Free(msg.pucApduData);
	}

ERROR :

	{
		// UI 처리를 위한 콜백이 이 시점에 available 하지 않기 때문에 타이머를 걸어서 처리하도록 한다...
		unsigned long ulTimerID = 0;
//		unsigned long ulTaskID = 0;
		VK_TIMER_EventAfter(5000, CiCc_CredentialsErrorTimerCb, NULL, 0, (unsigned long *)&ulTimerID);
		#if 0 // 아래 함수에 문제가 있는 듯... crash 가 나기 때문에 일단 #else 처럼 처리한다...
		if (VK_TASK_GetCurrentId(&ulTaskID) == VK_OK)
		{
			VK_TASK_Stop(ulTaskID);
			VK_TASK_Destroy(ulTaskID);
		}
		#else
		while (1)
		{
			VK_TASK_Sleep(10000);
		}
		#endif
	}

}

#ifdef USE_SUB_TASK_TO_CALCULATE_KEYS
static void CiCc_SubTask(void)
{
	CI_CC_SUB_TASK_MSG msg;

	while (1)
	{
		if (VK_MSG_Receive(s_ulCiCcSubMsgQId, &msg, sizeof(CI_CC_SUB_TASK_MSG)) != VK_OK)
		{
			PrintErr(("[CICC]---> CiCc_SubTask()---> Error : fail to get msg !!!\n"));
			continue;
		}

		switch (msg.command)
		{
			case CI_CC_DO_CALCULATE_CCK:
				if (CiCc_CalculateCckAndSetDescrambler(msg.hCc, msg.bDoCalculateCck, msg.bDoSetDescrambler))
				{
					PrintErr(("[CICC]---> CiCc_SubTask()---> Error : CiCc_CalculateCckAndSetDescrambler() !!!\n"));
				}
				break;

			case CI_CC_DO_CALCULATE_SAC:
				if (CiCc_CalculateSacKeys(msg.hCc))
				{
					PrintErr(("[CICC]---> CiCc_SubTask()---> Error : CiCc_CalculateSacKeys() !!!\n"));
				}
				break;

			default:
				PrintErr(("[CICC]---> CiCc_SubTask()---> Error : Unknown command %d !!!\n", msg.command));
				break;
		}
	}
}
#endif

static void CiCc_CredentialsErrorTimerCb(unsigned long ulTimerId, void *pvParam)
{
	PrintErr(("CiCc_CredentialsErrorTimerCb !!!\n"));

	// UI 처리를 위한 콜백
	if (g_hCbNotifyCcReport)
	{
		Callback_Reset(g_hCbNotifyCcReport);
		while ((s_pfnNotifyCcReport = (CI_NOTIFY_CC_REPORT_FUNC)Callback_GetNextFunc(g_hCbNotifyCcReport)) != NULL)
		{
			(*s_pfnNotifyCcReport)(0, &s_stCcReportInfo); // 이 경우 슬롯 아이디는 의미가 없다.
		}
	}
	else
	{
		PrintErr(("[CICC]---> CiCc_CredentialsErrorTimerCb()---> Error : g_hCbNotifyCcReport is NULL !!!\n"));
	}
}

int CiCc_SsOpenedProc(unsigned short usSsNo)
{
	if (CiCc_AddInst(usSsNo) < 0)
	{
		return -1;
	}

	return ERR_OK;
}

int CiCc_SsClosedProc(unsigned short usSsNo)
{
	if (CiCc_DeleteInst(usSsNo) < 0)
	{
		return -1;
	}

	return ERR_OK;
}

int CiCc_ApduProc(unsigned short usSsNo, unsigned long ulApduTag,
						unsigned char *pucApduData, unsigned long ulApduLen)
{
	CI_CC_TASK_MSG msg;

#ifdef _CI_DEBUG_CC_PROC_TIME_
	VK_Print("[CICC]---> CiCc_ApduProc() for Tag[0x%lx] : %ld tick\n", ulApduTag, VK_TIMER_GetSystemTick());
#endif

	if (!s_bCiCcInitSuccess)
	{
		PrintErr(("[CICC]---> CiCc_ApduProc()---> Error : No credentials in this set !!!\n"));
		return -1;
	}

#ifdef _CI_DEBUG_CC_TIME_ISSUE_
	if (s_bCcProcBusy)
	{
		PrintErr(("[CICC]---> CiCc_ApduProc()---> Error : CiCc_Proc() is busy !!!\n"));
	}
	s_bCcProcBusy = 1;
#endif

	msg.usSsNo = usSsNo;
	msg.ulApduTag = ulApduTag;
	if (ulApduLen)
	{
		msg.pucApduData = (unsigned char *)CA_MEM_Alloc(sizeof(unsigned char) * ulApduLen);
		VK_memcpy(msg.pucApduData, pucApduData, ulApduLen);
		msg.ulApduLen = ulApduLen;
	}
	else
	{
		msg.pucApduData = NULL;
		msg.ulApduLen = 0;
	}

	if (VK_MSG_Send(s_ulCiCcMsgQId, &msg, sizeof(CI_CC_TASK_MSG)))
	{
		PrintErr(("[CICC]---> CiCc_ApduProc()---> Error : fail to send msg !!!\n"));
		CA_MEM_Free(msg.pucApduData);
		return -1;
	}

	return ERR_OK;
}

static int CiCc_Proc(unsigned short usSsNo, unsigned long ulApduTag,
						unsigned char *pucApduData, unsigned long ulApduLen)
{
	int nRtnCode = ERR_OK;

#ifdef _CI_DEBUG_CC_PROC_TIME_
	s_ulCcProcStartTime = VK_TIMER_GetSystemTick();
#endif

	switch (ulApduTag)
	{
	case CI_CC_OPEN_REQ:
		/*
		용도 : Host capability evaluation

		받은 APDU
		=========
		cc_open_req() {
			cc_open_req_tag								= CI_CC_OPEN_REQ
			length_field()								= 0
		}

		보낼 APDU
		=========
		cc_open_cnf() {
			cc_open_cnf_tag								= CI_CC_OPEN_CNF
			length_field()								= 1
			cc_system_id_bitmask						8 bits
		}
		*/

		PrintNotify(("CI_CC_OPEN_REQ received\n"));
		nRtnCode = CiCc_OpenConfirm(usSsNo, pucApduData, ulApduLen);
		break;

	case CI_CC_DATA_REQ:
		/*
		용도 : Authentication, Auth key verification, SAC key calculation

		받은 APDU
		=========
		cc_data_req() {
			cc_data_req_tag								= CI_CC_DATA_REQ
			length_field()
			cc_system_id_bitmask						8 bits
			send_datatype_nbr							8 bits
			for (i=0; i<send_datatype_nbr; i++) {
				datatype_id								8 bits
				datatype_length							16 bits
				data_type								8*datatype_length bits
			}
			request_datatype_nbr						8 bits
			for (i=0; i<request_datatype_nbr; i++) {
				datatype_id								8 bits
			}
		}

		보낼 APDU
		=========
		cc_data_cnf() {
			cc_data_cnf_tag								= CI_CC_DATA_CNF
			length_field()
			cc_system_id_bitmask						8 bits
			send_datatype_nbr							8 bits
			for (i=0; i<send_datatype_nbr; i++) {
				datatype_id								8 bits
				datatype_length							16 bits
				data_type								8*datatype_length bits
			}
		}
		*/

		PrintNotify(("CI_CC_DATA_REQ received\n"));
		nRtnCode = CiCc_DataConfirm(usSsNo, pucApduData, ulApduLen, CI_CC_SAC_MSG_FLAG_FALSE);
		break;

	case CI_CC_SAC_DATA_REQ:
		/*
		용도 : CC key calculation, URI transmission and acknowledgement, URI version negotiation, SRM transmission and acknowledgement

		받은 APDU
		=========
		cc_sac_data_req() {
			cc_sac_data_req_tag							= CI_CC_SAC_DATA_REQ
			length_field()
			sac_message()
		}
		sac_message()의 encrypted_payload {
			cc_data_req()의 payload (encrypted)
		}

		보낼 APDU
		=========
		cc_sac_data_cnf() {
			cc_sac_data_cnf_tag							= CI_CC_SAC_DATA_CNF
			length_field()
			sac_message()
		}
		sac_message()의 encrypted_payload {
			cc_data_cnf()의 payload (encrypted)
		}
		*/

		PrintNotify(("CI_CC_SAC_DATA_REQ received\n"));
		nRtnCode = CiCc_DataConfirm(usSsNo, pucApduData, ulApduLen, CI_CC_SAC_MSG_FLAG_TRUE);
		break;

	case CI_CC_SYNC_REQ:
		/*
		용도 : SAC key calculation

		받은 APDU
		=========
		cc_sync_req() {
			cc_sync_req_tag								= CI_CC_SYNC_REQ
			length_field()								= 0
		}

		보낼 APDU
		=========
		cc_sync_cnf() {
			cc_sync_cnf_tag								= CI_CC_SYNC_CNF
			length_field()								= 1
			status_field								8 bits
		}
		*/

		PrintNotify(("CI_CC_SYNC_REQ received\n"));
		nRtnCode = CiCc_SyncConfirm(usSsNo, pucApduData, ulApduLen, CI_CC_SAC_MSG_FLAG_FALSE);
		break;

	case CI_CC_SAC_SYNC_REQ:
		/*
		용도 : CC key calculation

		받은 APDU
		=========
		cc_sac_sync_req() {
			cc_sac_sync_req_tag							= CI_CC_SAC_SYNC_REQ
			length_field()
			sac_message()
		}
		sac_message()의 encrypted_payload {
			empty
		}

		보낼 APDU
		=========
		cc_sac_sync_cnf() {
			cc_sac_sync_cnf_tag							= CI_CC_SAC_SYNC_CNF
			length_field()
			sac_message()
		}
		sac_message()의 encrypted_payload {
			cc_sync_cnf()의 payload (encrypted)
		}
		*/

		PrintNotify(("CI_CC_SAC_SYNC_REQ received\n"));
		nRtnCode = CiCc_SyncConfirm(usSsNo, pucApduData, ulApduLen, CI_CC_SAC_MSG_FLAG_TRUE);
		break;

	default:
		nRtnCode = -1;
		PrintErr(("[CICC]---> CiCc_Proc()---> Error : Unknown Apdu Tag[0x%lx] !!!\n", ulApduTag));
	}

#ifdef _CI_DEBUG_CC_TIME_ISSUE_
	s_bCcProcBusy = 0;
#endif

	PrintDbg(("[CICC]---> CiCc_Proc()---> SsNo[%d], tag[0x%lx] done\n", usSsNo, ulApduTag));

#ifdef _CI_DEBUG_CC_PROC_TIME_
	s_ulCcProcEndTime = VK_TIMER_GetSystemTick();
	VK_Print("[CICC]---> CiCc_Proc() for Tag[0x%lx] : %ld ms from %ld tick\n", ulApduTag, s_ulCcProcEndTime - s_ulCcProcStartTime, s_ulCcProcStartTime);
#endif

	return nRtnCode;
}

static int CiCc_OpenConfirm(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen)
{
	CI_CC_INSTANCE	* hCc;
	unsigned char	ucMsg[5];

	hCc = CiCc_GetInst(usSsNo);
	if (hCc == NULL)
	{
		PrintErr(("[CICC]---> CiCc_OpenConfirm()---> Error : CiCc_GetInst() SsNo[%d] - NULL hCc\n", usSsNo));
		return -1;
	}

	ucMsg[0] = (unsigned char)((CI_CC_OPEN_CNF >> 16) & 0xff);
	ucMsg[1] = (unsigned char)((CI_CC_OPEN_CNF >> 8) & 0xff);
	ucMsg[2] = (unsigned char)(CI_CC_OPEN_CNF & 0xff);
	ucMsg[3] = 0x01;
	ucMsg[4] = CI_CC_SYSTEM_ID_BITMASK_V1;

	if (CIRH_SendApdu(usSsNo, ucMsg, 5) != ERR_OK)
	{
		PrintErr(("[CICC]---> CiCc_OpenConfirm()---> Error : send APDU msg. SsNo[%d] !!!\n", usSsNo));
		return -1;
	}

	hCc->enPhase = CI_CC_PHASE_Host_Capability_Evaluation_Replied;

	PrintDbg(("[CICC]---> CiCc_OpenConfirm()---> SsNo[%d]...\n", usSsNo));

	return ERR_OK;
}

static int CiCc_DataConfirm(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen, unsigned char ucSacFlag)
{
	CI_CC_INSTANCE	* hCc;

	CI_CC_SAC_MSG	stSacMsg;							// for host received sac_message()
	unsigned char	* pucDecryptedSacPayload = NULL;	// for host received sac_message()'s payload
	unsigned short	usOriginalSacPayloadLen;			// for host sending sac_message()'s payload
	unsigned short	usAlignedSacPayloadLen;			//
	unsigned char	* pucOriginalSacPayload = NULL;	//
	unsigned char	* pucEncryptedSacPayload = NULL;	//
	unsigned char	ucSacBuf[8];

	unsigned char	* pucData;							// just pointer to data (don't mem alloc for this)
	unsigned short	usDataLen;

	unsigned char	uc_cc_system_id_bitmask;

	unsigned char	uc_send_data_nbr;
	unsigned char	uc_send_data_id[CI_CC_MAX_DATA_NBR];
	unsigned short	us_send_data_length[CI_CC_MAX_DATA_NBR];
	unsigned char	* puc_send_data[CI_CC_MAX_DATA_NBR] = {NULL,};

	unsigned char	uc_request_data_nbr;
	unsigned char	uc_request_data_id[CI_CC_MAX_DATA_NBR];
	unsigned short	us_request_data_length[CI_CC_MAX_DATA_NBR];
	unsigned char	* puc_request_data[CI_CC_MAX_DATA_NBR] = {NULL,};

	int				pos, i;

	unsigned char	* pucMsg = NULL;
	unsigned long	ulMsgPayloadLen;
	unsigned char	ucLenFieldSize;

	int nRtnCode = ERR_OK;

	hCc = CiCc_GetInst(usSsNo);
	if (hCc == NULL)
	{
		PrintErr(("[CICC]---> CiCc_DataConfirm()---> Error : CiCc_GetInst() SsNo[%d] - NULL hCc\n", usSsNo));
		nRtnCode = -1;
		goto EXIT;
	}

	/* ------------------------------------------------ */
	/* --- parse cc_data_req() or cc_sac_data_req() --- */
	/* ------------------------------------------------ */

	if (ucSacFlag) // parse received sac_message(), decrypt its encrypted_payload and verify sac_message() in case of cc_sac_data_req()
	{
		if (pucApduData == NULL || ulApduLen < 9)
		{
			PrintErr(("[CICC]---> CiCc_DataConfirm()---> Error : Invalid ulApduLen[%ld] !!!\n", ulApduLen));
			nRtnCode = -1;
			goto EXIT;
		}

		stSacMsg.ulMsgCnt = ((unsigned long)pucApduData[0] << 24) & 0xff000000;
		stSacMsg.ulMsgCnt |= (((unsigned long)pucApduData[1] << 16) & 0x00ff0000);
		stSacMsg.ulMsgCnt |= (((unsigned long)pucApduData[2] << 8) & 0x0000ff00);
		stSacMsg.ulMsgCnt |= ((unsigned long)pucApduData[3] & 0x000000ff);

		stSacMsg.ucHdr1 = pucApduData[4];
		stSacMsg.ucHdr2 = pucApduData[5];

		stSacMsg.usPayloadLen = ((unsigned short)pucApduData[6] << 8) & 0xff00;
		stSacMsg.usPayloadLen |= ((unsigned short)pucApduData[7] & 0x00ff);

		stSacMsg.pucPayloadData = &pucApduData[8];

		PrintNotify(("[CICC]---> CiCc_DataConfirm()---> MsgCnt = %ld, Hdr = %02X %02X, PayloadLen = %d\n", stSacMsg.ulMsgCnt, stSacMsg.ucHdr1, stSacMsg.ucHdr2, stSacMsg.usPayloadLen));

		// ignore received sac_message() if...
		if (stSacMsg.ulMsgCnt == hCc->ulSacMsgCnt + 1) // The first message shall use number 0x1 and wraps back to 0x1 (not zero).
		{
			hCc->ulSacMsgCnt = (stSacMsg.ulMsgCnt == CI_CC_SAC_MSG_CNT_MAX) ? 0 : stSacMsg.ulMsgCnt;
		}
		else
		{
			PrintErr(("[CICC]---> CiCc_DataConfirm()---> Error : Invalid ulMsgCnt[%ld]---> %ld expected !!!\n", stSacMsg.ulMsgCnt, hCc->ulSacMsgCnt+1));
			nRtnCode = -1;
			goto EXIT;
		}
		if (stSacMsg.ucHdr1 != 0x01)
		{
			PrintErr(("[CICC]---> CiCc_DataConfirm()---> Error : Invalid ucHdr1[0x%x] !!!\n", stSacMsg.ucHdr1));
			nRtnCode = -1;
			goto EXIT;
		}
		if ((stSacMsg.ucHdr2 >> 5) != 0x0)
		{
			PrintErr(("[CICC]---> CiCc_DataConfirm()---> Error : Invalid ucHdr2[0x%x] !!!\n", stSacMsg.ucHdr2));
			nRtnCode = -1;
			goto EXIT;
		}
		if (stSacMsg.usPayloadLen & CI_CC_CIPHER_BLOCK_SIZE_1)
		{
			PrintErr(("[CICC]---> CiCc_DataConfirm()---> Error : Invalid usPayloadLen[%d] !!!\n", stSacMsg.usPayloadLen));
			nRtnCode = -1;
			goto EXIT;
		}

		// decrypt received sac_message()'s encrypted_payload
		pucDecryptedSacPayload = (unsigned char *)CA_MEM_Alloc(sizeof(unsigned char) * (stSacMsg.usPayloadLen + 16));
		D_AES_CBC(hCc->stHostData.SEK, stSacMsg.pucPayloadData, pucDecryptedSacPayload, stSacMsg.usPayloadLen + 16, s_stCiHostLicenseConst.SIV); // 이 시점에 SEK 값은 유효할 것이다.

		#if CI_CC_VerifyReceivedSacMessage // verify received sac_message()
		VK_memcpy(stSacMsg.ucAuth, &pucDecryptedSacPayload[stSacMsg.usPayloadLen], 16);
		{
			unsigned char *pucTempBuf = NULL;
			unsigned char MAC[16];
			pucTempBuf = (unsigned char *)CA_MEM_Alloc(sizeof(unsigned char) * (1+8+stSacMsg.usPayloadLen));
			pucTempBuf[0] = 4;
			VK_memcpy(&pucTempBuf[1], pucApduData, 8);
			VK_memcpy(&pucTempBuf[1+8], pucDecryptedSacPayload, stSacMsg.usPayloadLen);
			MAC_AES_XCBC(hCc->stHostData.SAK, pucTempBuf, 1+8+stSacMsg.usPayloadLen, MAC); // 이 시점에 SAK 값은 유효할 것이다.
			CA_MEM_Free(pucTempBuf);
			if (VK_memcmp(MAC, stSacMsg.ucAuth, 16) != 0)
			{
				PrintErr(("[CICC]---> CiCc_DataConfirm()---> Error : Invalid authentication !!!\n"));
				nRtnCode = -1;
				goto EXIT;
			}
		}
		#endif

		pucData = pucDecryptedSacPayload;
		usDataLen = stSacMsg.usPayloadLen;
	}
	else
	{
		pucData = pucApduData;
		usDataLen = (unsigned short)ulApduLen;
	}

	if (pucData == NULL || usDataLen < 3)
	{
		PrintErr(("[CICC]---> CiCc_DataConfirm()---> Error : Invalid usDataLen[%d] !!!\n", usDataLen));
		nRtnCode = -1;
		goto EXIT;
	}

	// get cc_system_id_bitmask
	uc_cc_system_id_bitmask = pucData[0]; /* check with CI_CC_SYSTEM_ID_BITMASK_V1 ? */

	// get send_data_nbr
	uc_send_data_nbr = pucData[1];
	if (uc_send_data_nbr > CI_CC_MAX_DATA_NBR)
	{
		PrintErr(("[CICC]---> CiCc_DataConfirm()---> Error : CI_CC_MAX_DATA_NBR(%d) is too small for send %d !!!\n", CI_CC_MAX_DATA_NBR, uc_send_data_nbr));
		nRtnCode = -1;
		goto EXIT;
	}

	// get send_data
	pos = 2;
	for (i = 0; i < uc_send_data_nbr; i++)
	{
		uc_send_data_id[i] = pucData[pos];
		us_send_data_length[i] = ((unsigned short)pucData[pos+1] << 8) & 0xff00;
		us_send_data_length[i] |= ((unsigned short)pucData[pos+2] & 0x00ff);
		if (us_send_data_length[i])
		{
			puc_send_data[i] = (unsigned char *)CA_MEM_Alloc(sizeof(unsigned char) * us_send_data_length[i]);
			VK_memcpy(puc_send_data[i], &pucData[pos+3], us_send_data_length[i]);
		}
		pos += (3 + us_send_data_length[i]);
	}

	// get request_data_nbr
	uc_request_data_nbr = pucData[pos++];
	if (uc_request_data_nbr > CI_CC_MAX_DATA_NBR)
	{
		PrintErr(("[CICC]---> CiCc_DataConfirm()---> Error : CI_CC_MAX_DATA_NBR(%d) is too small for request %d !!!\n", CI_CC_MAX_DATA_NBR, uc_request_data_nbr));
		nRtnCode = -1;
		goto EXIT;
	}

	// get request_data_id
	for (i = 0; i < uc_request_data_nbr; i++)
	{
		uc_request_data_id[i] = pucData[pos++];
	}

	// checking : 데이터들 간에는 B를 생성하기 위해서는 A가 필요하다는 식의 선후 관계가 존재한다. 따라서 스펙에 명시된 순서가 지켜지지 않으면 문제가 발생한다.
	#if 0 // #ifdef _CI_DEBUG_CC_DATA_
	VK_Print("\n - CiCc_DataConfirm() - \n");
	VK_Print("\t cc_system_id_bitmask = 0x%x \n", uc_cc_system_id_bitmask);
	VK_Print("\t send_data_nbr = %d \n", uc_send_data_nbr);
	for (i = 0; i < uc_send_data_nbr; i++)
		VK_Print("\t\t %d : id = %d (%d bits) \n", i, uc_send_data_id[i], us_send_data_length[i] * 8);
	VK_Print("\t request_data_nbr = %d \n", uc_request_data_nbr);
	for (i = 0; i < uc_request_data_nbr; i++)
		VK_Print("\t\t %d : id = %d \n", i, uc_request_data_id[i]);
	VK_Print("\n");
	#endif

	/* -------------------------------------------- */
	/* --- save send_data and make request_data --- */
	/* -------------------------------------------- */

	// verify and save send_data
	for (i = 0; i < uc_send_data_nbr; i++)
	{
		CiCc_VerifyAndSaveSendData(hCc, uc_send_data_id[i], us_send_data_length[i], puc_send_data[i]);
	}

	// make request_data
	for (i = 0; i < uc_request_data_nbr; i++)
	{
		CiCc_MakeReqDataAndDoSomething(hCc, uc_request_data_id[i], &us_request_data_length[i], &puc_request_data[i]);
	}

	/* ----------------------------------------------------------- */
	/* --- make cc_data_cnf() or cc_sac_data_cnf() and send it --- */
	/* ----------------------------------------------------------- */

	if (ucSacFlag)
	{
		/*** make sending sac_message()'s encrypted_payload in case of cc_sac_data_cnf() ***/

		// calculate original & aligned length
		usOriginalSacPayloadLen = 2;
		for (i = 0; i < uc_request_data_nbr; i++)
			usOriginalSacPayloadLen += (3 + us_request_data_length[i]);
		usAlignedSacPayloadLen = GetAlignedLen(usOriginalSacPayloadLen);

		// set sending sac_message()'s message_counter and header
		ucSacBuf[0] = (unsigned char)((stSacMsg.ulMsgCnt >> 24) & 0xff);
		ucSacBuf[1] = (unsigned char)((stSacMsg.ulMsgCnt >> 16) & 0xff);
		ucSacBuf[2] = (unsigned char)((stSacMsg.ulMsgCnt >> 8) & 0xff);
		ucSacBuf[3] = (unsigned char)(stSacMsg.ulMsgCnt & 0xff);
		ucSacBuf[4] = stSacMsg.ucHdr1;
		ucSacBuf[5] = stSacMsg.ucHdr2;
		ucSacBuf[6] = (unsigned char)((usAlignedSacPayloadLen >> 8) & 0xff);
		ucSacBuf[7] = (unsigned char)(usAlignedSacPayloadLen & 0xff);

		// allocate aligned buffers and set data
		pucOriginalSacPayload = (unsigned char *)CA_MEM_Alloc(sizeof(unsigned char) * (usAlignedSacPayloadLen + 16));
		pucEncryptedSacPayload = (unsigned char *)CA_MEM_Alloc(sizeof(unsigned char) * (usAlignedSacPayloadLen + 16));
		pucOriginalSacPayload[0] = uc_cc_system_id_bitmask;
		pucOriginalSacPayload[1] = uc_request_data_nbr;
		pos = 2;
		for (i = 0; i < uc_request_data_nbr; i++)
		{
			pucOriginalSacPayload[pos] = uc_request_data_id[i];
			pucOriginalSacPayload[pos+1] = (unsigned char)((us_request_data_length[i] >> 8) & 0xff);
			pucOriginalSacPayload[pos+2] = (unsigned char)(us_request_data_length[i] & 0xff);
			VK_memcpy(&pucOriginalSacPayload[pos+3], puc_request_data[i], us_request_data_length[i]);
			pos += (3 + us_request_data_length[i]);
		}

		// padding
		OneAndZeroesPadding(pucOriginalSacPayload, usOriginalSacPayloadLen);

		// authentication
		{
			unsigned char *pucTempBuf = NULL;
			unsigned char MAC[16];
			pucTempBuf = (unsigned char *)CA_MEM_Alloc(sizeof(unsigned char) * (1+8+usAlignedSacPayloadLen));
			pucTempBuf[0] = 4;
			VK_memcpy(&pucTempBuf[1], ucSacBuf, 8);
			VK_memcpy(&pucTempBuf[1+8], pucOriginalSacPayload, usAlignedSacPayloadLen);
			MAC_AES_XCBC(hCc->stHostData.SAK, pucTempBuf, 1+8+usAlignedSacPayloadLen, MAC); // 이 시점에 SAK 값은 유효할 것이다.
			CA_MEM_Free(pucTempBuf);
			VK_memcpy(&pucOriginalSacPayload[usAlignedSacPayloadLen], MAC, 16);
		}

		// encrypt
		E_AES_CBC(hCc->stHostData.SEK, pucOriginalSacPayload, pucEncryptedSacPayload, usAlignedSacPayloadLen + 16, s_stCiHostLicenseConst.SIV); // 이 시점에 SEK 값은 유효할 것이다.

		/*** make message to send ***/

		// calculate message payload length
		ulMsgPayloadLen = 8 + usAlignedSacPayloadLen + 16;

		// allocate message buffer
		pucMsg = (unsigned char *)CA_MEM_Alloc(sizeof(unsigned char) * (/*6*/ 8 + ulMsgPayloadLen)); /* tag field (3 bytes) + max length field (3->5 bytes) + payload length */

		// set message tag
		pucMsg[0] = (unsigned char)((CI_CC_SAC_DATA_CNF >> 16) & 0xff);
		pucMsg[1] = (unsigned char)((CI_CC_SAC_DATA_CNF >> 8) & 0xff);
		pucMsg[2] = (unsigned char)(CI_CC_SAC_DATA_CNF & 0xff);

		// set message length
		CIRH_ConvertApduLengthField(ulMsgPayloadLen, &pucMsg[3], &ucLenFieldSize);

		// set message payload
		VK_memcpy(&pucMsg[3+ucLenFieldSize], ucSacBuf, 8);
		VK_memcpy(&pucMsg[11+ucLenFieldSize], pucEncryptedSacPayload, usAlignedSacPayloadLen + 16);
	}
	else
	{
		/*** make message to send ***/

		// calculate message payload length
		ulMsgPayloadLen = 2;
		for (i = 0; i < uc_request_data_nbr; i++)
			ulMsgPayloadLen += (3 + us_request_data_length[i]);

		// allocate message buffer
		pucMsg = (unsigned char *)CA_MEM_Alloc(sizeof(unsigned char) * (/*6*/ 8 + ulMsgPayloadLen)); /* tag field (3 bytes) + max length field (3->5 bytes) + payload length */

		// set message tag
		pucMsg[0] = (unsigned char)((CI_CC_DATA_CNF >> 16) & 0xff);
		pucMsg[1] = (unsigned char)((CI_CC_DATA_CNF >> 8) & 0xff);
		pucMsg[2] = (unsigned char)(CI_CC_DATA_CNF & 0xff);

		// set message length
		CIRH_ConvertApduLengthField(ulMsgPayloadLen, &pucMsg[3], &ucLenFieldSize);

		// set message payload
		pucMsg[3+ucLenFieldSize] = uc_cc_system_id_bitmask;
		pucMsg[4+ucLenFieldSize] = uc_request_data_nbr;
		pos = 5 + ucLenFieldSize;
		for (i = 0; i < uc_request_data_nbr; i++)
		{
			pucMsg[pos] = uc_request_data_id[i];
			pucMsg[pos+1] = (unsigned char)((us_request_data_length[i] >> 8) & 0xff);
			pucMsg[pos+2] = (unsigned char)(us_request_data_length[i] & 0xff);
			VK_memcpy(&pucMsg[pos+3], puc_request_data[i], us_request_data_length[i]);
			pos += (3 + us_request_data_length[i]);
		}
	}

	if (CIRH_SendApdu(usSsNo, pucMsg, 3 + ucLenFieldSize + ulMsgPayloadLen) != ERR_OK)
	{
		PrintErr(("[CICC]---> CiCc_DataConfirm()---> Error : send APDU msg. SsNo[%d] !!!\n", usSsNo));
		nRtnCode = -1;
		goto EXIT;
	}

	PrintNotify(("[CICC]---> CiCc_DataConfirm()---> %ld bytes payload sent\n", ulMsgPayloadLen));
	#if 0 // #ifdef _CI_DEBUG_CC_DATA_
	{
		for (i = 0; i < 3 + ucLenFieldSize; i++)
		{
			if ((i & 15) == 0)
				VK_Print("\n");
			VK_Print(" %02x", pucMsg[i]);
		}
		for (i = 0; i < usMsgPayloadLen; i++)
		{
			if ((i & 15) == 0)
				VK_Print("\n");
			VK_Print(" %02x", pucMsg[3 + ucLenFieldSize + i]);
		}
		VK_Print("\n\n");
	}
	#endif

EXIT :

	if (ucSacFlag)
	{
		if (pucDecryptedSacPayload)
			CA_MEM_Free(pucDecryptedSacPayload);
		if (pucOriginalSacPayload)
			CA_MEM_Free(pucOriginalSacPayload);
		if (pucEncryptedSacPayload)
			CA_MEM_Free(pucEncryptedSacPayload);
	}

	for (i = 0; i < CI_CC_MAX_DATA_NBR; i++)
	{
		if (puc_send_data[i])
			CA_MEM_Free(puc_send_data[i]);
		if (puc_request_data[i])
			CA_MEM_Free(puc_request_data[i]);
	}

	if (pucMsg)
		CA_MEM_Free(pucMsg);

	PrintDbg(("[CICC]---> CiCc_DataConfirm()---> SsNo[%d]...\n", usSsNo));

	return nRtnCode;
}

static int CiCc_SyncConfirm(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen, unsigned char ucSacFlag)
{
	CI_CC_INSTANCE	* hCc;

	CI_CC_SAC_MSG	stSacMsg;							// for host received sac_message()
	unsigned char	* pucDecryptedSacPayload = NULL;	// for host received sac_message()'s payload
	unsigned short	usOriginalSacPayloadLen;			// for host sending sac_message()'s payload
	unsigned short	usAlignedSacPayloadLen;			//
	unsigned char	* pucOriginalSacPayload = NULL;	//
	unsigned char	* pucEncryptedSacPayload = NULL;	//
	unsigned char	ucSacBuf[8];

	unsigned char	* pucMsg = NULL;
	unsigned long	ulMsgPayloadLen;
	unsigned char	ucLenFieldSize;

	int nRtnCode = ERR_OK;

	hCc = CiCc_GetInst(usSsNo);
	if (hCc == NULL)
	{
		PrintErr(("[CICC]---> CiCc_SyncConfirm()---> Error : CiCc_GetInst() SsNo[%d] - NULL hCc\n", usSsNo));
		nRtnCode = -1;
		goto EXIT;
	}

	/* ------------------------------------------------ */
	/* --- parse cc_sync_req() or cc_sac_sync_req() --- */
	/* ------------------------------------------------ */

	if (ucSacFlag) // parse received sac_message(), decrypt its encrypted_payload and verify sac_message() in case of cc_sac_sync_req()
	{
		if (pucApduData == NULL || ulApduLen < 9)
		{
			PrintErr(("[CICC]---> CiCc_SyncConfirm()---> Error : Invalid ulApduLen[%ld] !!!\n", ulApduLen));
			nRtnCode = -1;
			goto EXIT;
		}

		stSacMsg.ulMsgCnt = ((unsigned long)pucApduData[0] << 24) & 0xff000000;
		stSacMsg.ulMsgCnt |= (((unsigned long)pucApduData[1] << 16) & 0x00ff0000);
		stSacMsg.ulMsgCnt |= (((unsigned long)pucApduData[2] << 8) & 0x0000ff00);
		stSacMsg.ulMsgCnt |= ((unsigned long)pucApduData[3] & 0x000000ff);

		stSacMsg.ucHdr1 = pucApduData[4];
		stSacMsg.ucHdr2 = pucApduData[5];

		stSacMsg.usPayloadLen = ((unsigned short)pucApduData[6] << 8) & 0xff00;
		stSacMsg.usPayloadLen |= ((unsigned short)pucApduData[7] & 0x00ff);

		stSacMsg.pucPayloadData = &pucApduData[8];

		PrintNotify(("[CICC]---> CiCc_SyncConfirm()---> MsgCnt = %ld, Hdr = %02X %02X, PayloadLen = %d\n", stSacMsg.ulMsgCnt, stSacMsg.ucHdr1, stSacMsg.ucHdr2, stSacMsg.usPayloadLen));

		// ignore received sac_message() if...
		if (stSacMsg.ulMsgCnt == hCc->ulSacMsgCnt + 1) // The first message shall use number 0x1 and wraps back to 0x1 (not zero).
		{
			hCc->ulSacMsgCnt = (stSacMsg.ulMsgCnt == CI_CC_SAC_MSG_CNT_MAX) ? 0 : stSacMsg.ulMsgCnt;
		}
		else
		{
			PrintErr(("[CICC]---> CiCc_SyncConfirm()---> Error : Invalid ulMsgCnt[%ld]---> %ld expected !!!\n", stSacMsg.ulMsgCnt, hCc->ulSacMsgCnt+1));
			nRtnCode = -1;
			goto EXIT;
		}
		if (stSacMsg.ucHdr1 != 0x01)
		{
			PrintErr(("[CICC]---> CiCc_SyncConfirm()---> Error : Invalid ucHdr1[0x%x] !!!\n", stSacMsg.ucHdr1));
			nRtnCode = -1;
			goto EXIT;
		}
		if ((stSacMsg.ucHdr2 >> 5) != 0x0)
		{
			PrintErr(("[CICC]---> CiCc_SyncConfirm()---> Error : Invalid ucHdr2[0x%x] !!!\n", stSacMsg.ucHdr2));
			nRtnCode = -1;
			goto EXIT;
		}
		if (stSacMsg.usPayloadLen & CI_CC_CIPHER_BLOCK_SIZE_1)
		{
			PrintErr(("[CICC]---> CiCc_SyncConfirm()---> Error : Invalid usPayloadLen[%d] !!!\n", stSacMsg.usPayloadLen));
			nRtnCode = -1;
			goto EXIT;
		}

		// decrypt received sac_message()'s encrypted_payload
		pucDecryptedSacPayload = (unsigned char *)CA_MEM_Alloc(sizeof(unsigned char) * (stSacMsg.usPayloadLen + 16));
		D_AES_CBC(hCc->stHostData.SEK, stSacMsg.pucPayloadData, pucDecryptedSacPayload, stSacMsg.usPayloadLen + 16, s_stCiHostLicenseConst.SIV); // 이 시점에 SEK 값은 유효할 것이다.

		#if CI_CC_VerifyReceivedSacMessage // verify received sac_message()
		VK_memcpy(stSacMsg.ucAuth, &pucDecryptedSacPayload[stSacMsg.usPayloadLen], 16);
		{
			unsigned char *pucTempBuf = NULL;
			unsigned char MAC[16];
			pucTempBuf = (unsigned char *)CA_MEM_Alloc(sizeof(unsigned char) * (1+8+stSacMsg.usPayloadLen));
			pucTempBuf[0] = 4;
			VK_memcpy(&pucTempBuf[1], pucApduData, 8);
			VK_memcpy(&pucTempBuf[1+8], pucDecryptedSacPayload, stSacMsg.usPayloadLen);
			MAC_AES_XCBC(hCc->stHostData.SAK, pucTempBuf, 1+8+stSacMsg.usPayloadLen, MAC); // 이 시점에 SAK 값은 유효할 것이다.
			CA_MEM_Free(pucTempBuf);
			if (VK_memcmp(MAC, stSacMsg.ucAuth, 16) != 0)
			{
				PrintErr(("[CICC]---> CiCc_SyncConfirm()---> Error : Invalid authentication !!!\n"));
				nRtnCode = -1;
				goto EXIT;
			}
		}
		#endif

		/* nothing to do more for cc_sac_sync_req() */

	}
	else
	{
		/* nothing to do for cc_sync_req() */
	}

	/* --------------------- */
	/* --- calculate key --- */
	/* --------------------- */

	if (ucSacFlag)
	{
		#ifdef CI_CC_CalculateCckBeforeSyncAndSetDescramblerOnSync
		#ifdef USE_SUB_TASK_TO_CALCULATE_KEYS
		if (CiCc_CalculateCckAndSetDescramblerOnSubTask(hCc, FALSE, TRUE) == CI_ARC_ERR_CCK_Failed)
		#else
		if (CiCc_CalculateCckAndSetDescrambler(hCc, FALSE, TRUE) == CI_ARC_ERR_CCK_Failed)
		#endif
		{
			PrintErr(("[CICC]---> CiCc_SyncConfirm()---> Error : CCK failed !!!\n")); // 이런 상황이 발생하면 곤란. 디버깅 필요!!
			CiCc_DoActionForArcErr(hCc, CI_ARC_ERR_CCK_Failed);
		}
		#endif

		#if defined(USE_SUB_TASK_TO_CALCULATE_KEYS) && defined(_CI_DEBUG_CC_TIME_ISSUE_)
		if (!s_bCalculateCckDone)
		{
			PrintErr(("[CICC]---> CiCc_SyncConfirm()---> Error : CCK calculation not done yet !!!\n")); // 이런 상황이 발생하면 곤란. 디버깅 필요!!
//			CiCc_DoActionForArcErr(hCc, CI_ARC_ERR_CCK_Failed);
		}
		#endif
	}
	else
	{
		#if defined(USE_SUB_TASK_TO_CALCULATE_KEYS) && defined(_CI_DEBUG_CC_TIME_ISSUE_)
		if (!s_bCalculateSacDone)
		{
			PrintErr(("[CICC]---> CiCc_SyncConfirm()---> Error : SAC calculation not done yet !!!\n")); // 이런 상황이 발생하면 곤란. 디버깅 필요!!
//			CiCc_DoActionForArcErr(hCc, CI_ARC_ERR_SAC_Failed);
		}
		#endif

		if (hCc->ulSacMsgCnt)
		{
			// 이런 상황이 정상적일 수도 있으나 캠 삽입시 SAC 채널이 재설정되는 경우라면 뭔가 이상한 것이다!
			// 어쨌든 SAC key 가 refresh 되면 ulSacMsgCnt 를 리셋해야 한다.
			PrintErr(("[CICC]---> CiCc_SyncConfirm()---> SAC key refresh on MsgCnt = %ld !!!\n", hCc->ulSacMsgCnt));
			hCc->ulSacMsgCnt = 0;
		}
	}

	/* ----------------------------------------------------------- */
	/* --- make cc_sync_cnf() or cc_sac_sync_cnf() and send it --- */
	/* ----------------------------------------------------------- */

	/* 여기서 status_field 응답으로 CI_CC_STATUS_HOST_BUSY 도 가능하겠지만 사실상 항상 CI_CC_STATUS_OK 로 응답해야 할 듯... */

	if (ucSacFlag)
	{
		/*** make sending sac_message()'s encrypted_payload in case of cc_sac_sync_cnf() ***/

		// calculate original & aligned length
		usOriginalSacPayloadLen = 1;
		usAlignedSacPayloadLen = GetAlignedLen(usOriginalSacPayloadLen);

		// set sending sac_message()'s message_counter and header
		ucSacBuf[0] = (unsigned char)((stSacMsg.ulMsgCnt >> 24) & 0xff);
		ucSacBuf[1] = (unsigned char)((stSacMsg.ulMsgCnt >> 16) & 0xff);
		ucSacBuf[2] = (unsigned char)((stSacMsg.ulMsgCnt >> 8) & 0xff);
		ucSacBuf[3] = (unsigned char)(stSacMsg.ulMsgCnt & 0xff);
		ucSacBuf[4] = stSacMsg.ucHdr1;
		ucSacBuf[5] = stSacMsg.ucHdr2;
		ucSacBuf[6] = (unsigned char)((usAlignedSacPayloadLen >> 8) & 0xff);
		ucSacBuf[7] = (unsigned char)(usAlignedSacPayloadLen & 0xff);

		// allocate aligned buffers and set data
		pucOriginalSacPayload = (unsigned char *)CA_MEM_Alloc(sizeof(unsigned char) * (usAlignedSacPayloadLen + 16));
		pucEncryptedSacPayload = (unsigned char *)CA_MEM_Alloc(sizeof(unsigned char) * (usAlignedSacPayloadLen + 16));
		pucOriginalSacPayload[0] = CI_CC_STATUS_OK;

		// padding
		OneAndZeroesPadding(pucOriginalSacPayload, usOriginalSacPayloadLen);

		// authentication
		{
			unsigned char *pucTempBuf = NULL;
			unsigned char MAC[16];
			pucTempBuf = (unsigned char *)CA_MEM_Alloc(sizeof(unsigned char) * (1+8+usAlignedSacPayloadLen));
			pucTempBuf[0] = 4;
			VK_memcpy(&pucTempBuf[1], ucSacBuf, 8);
			VK_memcpy(&pucTempBuf[1+8], pucOriginalSacPayload, usAlignedSacPayloadLen);
			MAC_AES_XCBC(hCc->stHostData.SAK, pucTempBuf, 1+8+usAlignedSacPayloadLen, MAC); // 이 시점에 SAK 값은 유효할 것이다.
			CA_MEM_Free(pucTempBuf);
			VK_memcpy(&pucOriginalSacPayload[usAlignedSacPayloadLen], MAC, 16);
		}

		// encrypt
		E_AES_CBC(hCc->stHostData.SEK, pucOriginalSacPayload, pucEncryptedSacPayload, usAlignedSacPayloadLen + 16, s_stCiHostLicenseConst.SIV); // 이 시점에 SEK 값은 유효할 것이다.

		/*** make message to send ***/

		// calculate message payload length
		ulMsgPayloadLen = 8 + usAlignedSacPayloadLen + 16;

		// allocate message buffer
		pucMsg = (unsigned char *)CA_MEM_Alloc(sizeof(unsigned char) * (/*6*/ 8 + ulMsgPayloadLen)); /* tag field (3 bytes) + max length field (3->5 bytes) + payload length */

		// set message tag
		pucMsg[0] = (unsigned char)((CI_CC_SAC_SYNC_CNF >> 16) & 0xff);
		pucMsg[1] = (unsigned char)((CI_CC_SAC_SYNC_CNF >> 8) & 0xff);
		pucMsg[2] = (unsigned char)(CI_CC_SAC_SYNC_CNF & 0xff);

		// set message length
		CIRH_ConvertApduLengthField(ulMsgPayloadLen, &pucMsg[3], &ucLenFieldSize);

		// set message payload
		VK_memcpy(&pucMsg[3+ucLenFieldSize], ucSacBuf, 8);
		VK_memcpy(&pucMsg[11+ucLenFieldSize], pucEncryptedSacPayload, usAlignedSacPayloadLen + 16);
	}
	else
	{
		ulMsgPayloadLen = 1;
		ucLenFieldSize = 1;
		pucMsg = (unsigned char *)CA_MEM_Alloc(sizeof(unsigned char) * (3 + ucLenFieldSize + ulMsgPayloadLen));
		pucMsg[0] = (unsigned char)((CI_CC_SYNC_CNF >> 16) & 0xff);
		pucMsg[1] = (unsigned char)((CI_CC_SYNC_CNF >> 8) & 0xff);
		pucMsg[2] = (unsigned char)(CI_CC_SYNC_CNF & 0xff);
		pucMsg[3] = 0x01;
		pucMsg[4] = CI_CC_STATUS_OK;
	}

	if (CIRH_SendApdu(usSsNo, pucMsg, 3 + ucLenFieldSize + ulMsgPayloadLen) != ERR_OK)
	{
		PrintErr(("[CICC]---> CiCc_SyncConfirm()---> Error : send APDU msg. SsNo[%d] !!!\n", usSsNo));
		nRtnCode = -1;
		goto EXIT;
	}

	PrintNotify(("[CICC]---> CiCc_SyncConfirm()---> %ld bytes payload sent\n", ulMsgPayloadLen));
	#if 0 // #ifdef _CI_DEBUG_CC_DATA_
	{
		int i;
		for (i = 0; i < 3 + ucLenFieldSize; i++)
		{
			if ((i & 15) == 0)
				VK_Print("\n");
			VK_Print(" %02x", pucMsg[i]);
		}
		for (i = 0; i < usMsgPayloadLen; i++)
		{
			if ((i & 15) == 0)
				VK_Print("\n");
			VK_Print(" %02x", pucMsg[3 + ucLenFieldSize + i]);
		}
		VK_Print("\n\n");
	}
	#endif

EXIT :

	if (ucSacFlag)
	{
		if (pucDecryptedSacPayload)
			CA_MEM_Free(pucDecryptedSacPayload);
		if (pucOriginalSacPayload)
			CA_MEM_Free(pucOriginalSacPayload);
		if (pucEncryptedSacPayload)
			CA_MEM_Free(pucEncryptedSacPayload);
	}

	if (pucMsg)
		CA_MEM_Free(pucMsg);

	PrintDbg(("[CICC]---> CiCc_SyncConfirm()---> SsNo[%d]...\n", usSsNo));

	return nRtnCode;
}

/* 함수 설명 : cc_data_req() or cc_sac_data_req() 에 의해 캠에서 호스트로 전달된 데이터들을 검증하고 필요하다면 호스트 사이드에 저장한다.
               이 외의 액션은 CiCc_MakeReqDataAndDoSomething()에서 처리하도록 한다! */
static void CiCc_VerifyAndSaveSendData(CI_CC_INSTANCE *hCc, unsigned char ucSendDataId, unsigned short usSendDataLen, unsigned char *pucSendData)
{
	switch (ucSendDataId)
	{
		case CI_CC_CICAM_ID:
			if (usSendDataLen != 8)
			{
				PrintErr(("[CICC]---> CiCc_VerifyAndSaveSendData()---> Error : Invalid length[%d] of SendDataId[%d] !!!\n", usSendDataLen, ucSendDataId));
				CiCc_DoActionForArcErr(hCc, hCc->enPhase == CI_CC_PHASE_Authentication_Key_Verification_Replied ? CI_ARC_ERR_SAC_Failed : CI_ARC_ERR_CCK_Failed);
			}
			else
			{
				CI_AUTH_CTX * pstCiAuthCtx = NULL;
				if ((pstCiAuthCtx = CiCc_GetAuthContext(hCc->usSlotId, hCc->nCurAuthCtxIdx)) == NULL)
				{
					PrintErr(("[CICC]---> CiCc_VerifyAndSaveSendData()---> Error : Can't load binded CICAM_ID !!!\n")); // 이런 상황이 발생하면 곤란. 디버깅 필요!!
				}
				else
				{
					if (VK_memcmp(pucSendData, pstCiAuthCtx->ucBindedCamId, usSendDataLen) != 0) // 이 시점에 ucBindedCamId 값은 유효할 것이다.
					{
						PrintErr(("[CICC]---> CiCc_VerifyAndSaveSendData()---> Error : Not matched CICAM_ID !!!\n"));
						PrintErr(("[%02x %02x %02x %02x %02x %02x %02x %02x] vs [%02x %02x %02x %02x %02x %02x %02x %02x]\n",
							pucSendData[0], pucSendData[1], pucSendData[2], pucSendData[3], pucSendData[4], pucSendData[5], pucSendData[6], pucSendData[7],
							pstCiAuthCtx->ucBindedCamId[0], pstCiAuthCtx->ucBindedCamId[1], pstCiAuthCtx->ucBindedCamId[2], pstCiAuthCtx->ucBindedCamId[3],
							pstCiAuthCtx->ucBindedCamId[4], pstCiAuthCtx->ucBindedCamId[5], pstCiAuthCtx->ucBindedCamId[6], pstCiAuthCtx->ucBindedCamId[7]));
						CiCc_DoActionForArcErr(hCc, hCc->enPhase == CI_CC_PHASE_Authentication_Key_Verification_Replied ? CI_ARC_ERR_SAC_Failed : CI_ARC_ERR_CCK_Failed);
					}
				}
			}
			PrintNotify(("CI_CC_CICAM_ID done\n"));
			break;

		case CI_CC_CICAM_BrandCert:
			// Authentication 절차에서 CI_CC_DHPM, CI_CC_Signature_B, CI_CC_CICAM_DevCert, CI_CC_CICAM_BrandCert 간에는 선후 관계가 존재하므로
			// CI_CC_CICAM_BrandCert 에 대한 검증은 여기서 처리하지 않고 CiCc_MakeReqDataAndDoSomething()에서 처리하도록 한다.
			VK_memcpy(hCc->stCicamData.CICAM_BrandCert, pucSendData, usSendDataLen); // keep CICAM_BrandCert for further use
			hCc->stCicamData.CICAM_BrandCertLen = usSendDataLen;
			PrintNotify(("CI_CC_CICAM_BrandCert done\n"));
			break;

		case CI_CC_Kp:
			if (usSendDataLen != 32)
			{
				PrintErr(("[CICC]---> CiCc_VerifyAndSaveSendData()---> Error : Invalid length[%d] of SendDataId[%d] !!!\n", usSendDataLen, ucSendDataId));
				CiCc_DoActionForArcErr(hCc, CI_ARC_ERR_CCK_Failed);
			}
			else
			{
				VK_memcpy(hCc->stCicamData.Kp, pucSendData, usSendDataLen); // keep Kp for further use
				#ifdef CI_CC_CalculateCckBeforeSyncAndSetDescramblerOnSync
				#ifdef USE_SUB_TASK_TO_CALCULATE_KEYS
				if (CiCc_CalculateCckAndSetDescramblerOnSubTask(hCc, TRUE, FALSE) == CI_ARC_ERR_CCK_Failed)
				#else
				if (CiCc_CalculateCckAndSetDescrambler(hCc, TRUE, FALSE) == CI_ARC_ERR_CCK_Failed)
				#endif
				{
					PrintErr(("[CICC]---> CiCc_VerifyAndSaveSendData()---> Error : CCK failed !!!\n")); // 이런 상황이 발생하면 곤란. 디버깅 필요!!
					CiCc_DoActionForArcErr(hCc, CI_ARC_ERR_CCK_Failed);
				}
				#endif
				hCc->enPhase = CI_CC_PHASE_CCK_Calculating_Requested;
			}
			PrintNotify(("CI_CC_Kp done\n"));
			break;

		case CI_CC_DHPM:
			// Authentication 절차에서 CI_CC_DHPM, CI_CC_Signature_B, CI_CC_CICAM_DevCert, CI_CC_CICAM_BrandCert 간에는 선후 관계가 존재하므로
			// CI_CC_DHPM 에 대한 검증은 여기서 처리하지 않고 CiCc_MakeReqDataAndDoSomething()에서 처리하도록 한다.
			if (usSendDataLen != 256)
			{
				PrintErr(("[CICC]---> CiCc_VerifyAndSaveSendData()---> Error : Invalid length[%d] of SendDataId[%d] !!!\n", usSendDataLen, ucSendDataId));
				CiCc_DoActionForArcErr(hCc, CI_ARC_ERR_Auth_Failed_DH);
			}
			else
			{
				VK_memcpy(hCc->stCicamData.DHPM, pucSendData, usSendDataLen); // keep DHPM for further use
			}
			PrintNotify(("CI_CC_DHPM done\n"));
			break;

		case CI_CC_CICAM_DevCert:
			// Authentication 절차에서 CI_CC_DHPM, CI_CC_Signature_B, CI_CC_CICAM_DevCert, CI_CC_CICAM_BrandCert 간에는 선후 관계가 존재하므로
			// CI_CC_CICAM_DevCert 에 대한 검증은 여기서 처리하지 않고 CiCc_MakeReqDataAndDoSomething()에서 처리하도록 한다.
			VK_memcpy(hCc->stCicamData.CICAM_DevCert, pucSendData, usSendDataLen); // keep CICAM_DevCert for further use
			hCc->stCicamData.CICAM_DevCertLen = usSendDataLen;
			PrintNotify(("CI_CC_CICAM_DevCert done\n"));
			break;

		case CI_CC_Signature_B:
			// Authentication 절차에서 CI_CC_DHPM, CI_CC_Signature_B, CI_CC_CICAM_DevCert, CI_CC_CICAM_BrandCert 간에는 선후 관계가 존재하므로
			// CI_CC_Signature_B 에 대한 검증은 여기서 처리하지 않고 CiCc_MakeReqDataAndDoSomething()에서 처리하도록 한다.
			if (usSendDataLen != 256)
			{
				PrintErr(("[CICC]---> CiCc_VerifyAndSaveSendData()---> Error : Invalid length[%d] of SendDataId[%d] !!!\n", usSendDataLen, ucSendDataId));
				CiCc_DoActionForArcErr(hCc, CI_ARC_ERR_Auth_Failed_Invalid_Signature);
			}
			else
			{
				VK_memcpy(hCc->stCicamData.Signature_B, pucSendData, usSendDataLen); // keep Signature_B for further use
			}
			PrintNotify(("CI_CC_Signature_B done\n"));
			break;

		case CI_CC_auth_nonce:
			if (usSendDataLen != 32)
			{
				PrintErr(("[CICC]---> CiCc_VerifyAndSaveSendData()---> Error : Invalid length[%d] of SendDataId[%d] !!!\n", usSendDataLen, ucSendDataId));
				CiCc_DoActionForArcErr(hCc, CI_ARC_ERR_Auth_Failed_DH);
			}
			else
			{
				VK_memcpy(hCc->stCicamData.auth_nonce, pucSendData, usSendDataLen); // keep auth_nonce for further use
				hCc->enPhase = CI_CC_PHASE_Authentication_Requested;

				// UI 처리를 위한 콜백
				s_stCcReportInfo.eCcReport = eCiPlus_CcReport_VirginCamInsertedAndAuthenticationInProgress;
				Callback_Reset(g_hCbNotifyCcReport);
				while ((s_pfnNotifyCcReport = (CI_NOTIFY_CC_REPORT_FUNC)Callback_GetNextFunc(g_hCbNotifyCcReport)) != NULL)
				{
					(*s_pfnNotifyCcReport)(hCc->usSlotId, &s_stCcReportInfo);
				}
			}
			PrintNotify(("CI_CC_auth_nonce done\n"));
			break;

		case CI_CC_Ns_module:
			if (usSendDataLen != 8)
			{
				PrintErr(("[CICC]---> CiCc_VerifyAndSaveSendData()---> Error : Invalid length[%d] of SendDataId[%d] !!!\n", usSendDataLen, ucSendDataId));
				CiCc_DoActionForArcErr(hCc, CI_ARC_ERR_SAC_Failed);
			}
			else
			{
				VK_memcpy(hCc->stCicamData.Ns_module, pucSendData, usSendDataLen); // keep Ns_module for further use
				hCc->enPhase = CI_CC_PHASE_SAC_Establishment_Requested;

				// CAS/CI mgr 처리를 위한 콜백
				s_stCcReportInfo.eCcReport = eCiPlus_CcReport_AuthenticationDoneAndStartSacEstablishment;
				Callback_Reset(g_hCbNotifyCcReport);
				while ((s_pfnNotifyCcReport = (CI_NOTIFY_CC_REPORT_FUNC)Callback_GetNextFunc(g_hCbNotifyCcReport)) != NULL)
				{
					(*s_pfnNotifyCcReport)(hCc->usSlotId, &s_stCcReportInfo);
				}
			}
			PrintNotify(("CI_CC_Ns_module done\n"));
			break;

		case CI_CC_uri_message:
			if (usSendDataLen != 8)
			{
				PrintErr(("[CICC]---> CiCc_VerifyAndSaveSendData()---> Error : Invalid length[%d] of SendDataId[%d] !!!\n", usSendDataLen, ucSendDataId));
				CiCc_DoActionForArcErr(hCc, CI_ARC_ERR_Not_Defined);
			}
			else
			{
				if (pucSendData[0] != CI_CC_URI_PROTOCOL_DEFAULT)
				{
					PrintErr(("[CICC]---> CiCc_VerifyAndSaveSendData()---> Error : URI protocol not supported[0x%x] !!!\n", pucSendData[0]));
					/* CI_CC_URI_PROTOCOL_DEFAULT 가 아니어도 호환성 차원에서 아래 CopyCtrl 필드는 유효하지 않을까? 일단 경고만 주고 처리는 CI_CC_URI_PROTOCOL_DEFAULT 처럼 해주자... */
				}
				VK_memcpy(hCc->stCicamData.uri_message, pucSendData, usSendDataLen); // keep uri_message for further use
				hCc->stCicamData.stUri.ucApsCopyCtrl = (pucSendData[1] & 0xC0) >> 6;
				hCc->stCicamData.stUri.ucEmiCopyCtrl = (pucSendData[1] & 0x30) >> 4;
				hCc->stCicamData.stUri.ucIctCopyCtrl = (pucSendData[1] & 0x08) >> 3;
				hCc->stCicamData.stUri.ucRctCopyCtrl = (pucSendData[1] & 0x04) >> 2;
				hCc->stCicamData.stUri.ucRlCopyCtrl = pucSendData[2] & 0x3F;
			}
			PrintNotify(("CI_CC_uri_message done\n"));
			break;

		case CI_CC_program_number:
			if (usSendDataLen != 2)
			{
				PrintErr(("[CICC]---> CiCc_VerifyAndSaveSendData()---> Error : Invalid length[%d] of SendDataId[%d] !!!\n", usSendDataLen, ucSendDataId));
				CiCc_DoActionForArcErr(hCc, CI_ARC_ERR_Not_Defined);
			}
			else
			{
				hCc->stCicamData.usProgramNumber = ((unsigned short)pucSendData[0] << 8) & 0xff00; // keep program_number for further use
				hCc->stCicamData.usProgramNumber |= ((unsigned short)pucSendData[1] & 0x00ff);
			}
			PrintNotify(("CI_CC_program_number done\n"));
			break;

		case CI_CC_key_register:
			if (usSendDataLen != 1)
			{
				PrintErr(("[CICC]---> CiCc_VerifyAndSaveSendData()---> Error : Invalid length[%d] of SendDataId[%d] !!!\n", usSendDataLen, ucSendDataId));
				CiCc_DoActionForArcErr(hCc, hCc->enPhase == CI_CC_PHASE_SAC_Establishment_Requested ? CI_ARC_ERR_SAC_Failed : CI_ARC_ERR_CCK_Failed);
			}
			else
			{
				if (pucSendData[0] != CI_CC_KEY_MODE_EVEN && pucSendData[0] != CI_CC_KEY_MODE_ODD)
				{
					PrintErr(("[CICC]---> CiCc_VerifyAndSaveSendData()---> Error : Invalid Key[0x%x] !!!\n", pucSendData[0]));
					CiCc_DoActionForArcErr(hCc, hCc->enPhase == CI_CC_PHASE_SAC_Establishment_Requested ? CI_ARC_ERR_SAC_Failed : CI_ARC_ERR_CCK_Failed);
				}
				else
				{
					hCc->stCicamData.enKeyRegister = pucSendData[0]; // keep key_register for further use
				}
			}
			PrintNotify(("CI_CC_key_register done (%d)\n", pucSendData[0]));
			break;

		/* 이하 data id 들은 호스트로 보내지지 않는 것들이거나 CI+ 스펙 1.2 프로토콜 상에 실제 운용되지 않는 것들 */
		case CI_CC_Brand_ID:
		case CI_CC_Host_ID:
		case CI_CC_Host_BrandCert:
		case CI_CC_DHPH:
		case CI_CC_Host_DevCert:
		case CI_CC_Signature_A:
		case CI_CC_Ns_Host:
		case CI_CC_AKH:
		case CI_CC_AKM:
		case CI_CC_uri_confirm:
		case CI_CC_uri_versions:
		case CI_CC_status_field:
		case CI_CC_srm_data:
		case CI_CC_srm_confirm:
			PrintErr(("[CICC]---> CiCc_VerifyAndSaveSendData()---> Error : Invalid SendDataId[%d] !!!\n", ucSendDataId)); // 이 경우는 처리하지 않고 무시
			break;

		default:
			PrintErr(("[CICC]---> CiCc_VerifyAndSaveSendData()---> Error : Unknown SendDataId[%d] !!!\n", ucSendDataId)); // 이 경우는 처리하지 않고 무시
			break;
	}
}

/* 함수 설명 : cc_data_cnf() or cc_sac_data_cnf() 에 의해 호스트에서 캠으로 전달될 데이터들을 생성하고 필요한 액션이 있다면 여기서 수행한다. */
static void CiCc_MakeReqDataAndDoSomething(CI_CC_INSTANCE *hCc, unsigned char ucReqDataId, unsigned short *pusReqDataLen, unsigned char **ppucReqData)
{
	unsigned short	usDataLen = 0;
	unsigned char	* pucData = NULL;

	switch (ucReqDataId)
	{
		case CI_CC_Host_ID:
			usDataLen = 8;
			pucData = (unsigned char *)CA_MEM_Alloc(sizeof(unsigned char) * usDataLen);
			VK_memcpy(pucData, s_stCiHostLicenseConst.Host_ID, usDataLen);
			PrintNotify(("CI_CC_Host_ID done\n"));
			break;

		case CI_CC_Host_BrandCert:
			usDataLen = s_stCiHostLicenseConst.Host_BrandCertLen;
			pucData = (unsigned char *)CA_MEM_Alloc(sizeof(unsigned char) * usDataLen);
			VK_memcpy(pucData, s_stCiHostLicenseConst.Host_BrandCert, usDataLen);
			PrintNotify(("CI_CC_Host_BrandCert done\n"));
			break;

		case CI_CC_DHPH:
			usDataLen = 256;
			pucData = (unsigned char *)CA_MEM_Alloc(sizeof(unsigned char) * usDataLen);
			VK_memcpy(pucData, s_stCiHostLicenseConst.DHPH, usDataLen);
			PrintNotify(("CI_CC_DHPH done\n"));
			break;

		case CI_CC_Host_DevCert:
			usDataLen = s_stCiHostLicenseConst.Host_DevCertLen;
			pucData = (unsigned char *)CA_MEM_Alloc(sizeof(unsigned char) * usDataLen);
			VK_memcpy(pucData, s_stCiHostLicenseConst.Host_DevCert, usDataLen);
			PrintNotify(("CI_CC_Host_DevCert done\n"));
			break;

		case CI_CC_Signature_A:
			usDataLen = 256;
			pucData = (unsigned char *)CA_MEM_Alloc(sizeof(unsigned char) * usDataLen);
			if (CiCc_GenerateSignature(hCc, pucData) == CI_ARC_ERR_Auth_Failed_Invalid_Signature)
			{
				PrintErr(("[CICC]---> CiCc_MakeReqData()---> Error : host signature !!!\n")); // 이런 상황이 발생하면 곤란. 디버깅 필요!!
				CiCc_DoActionForArcErr(hCc, CI_ARC_ERR_Auth_Failed_Invalid_Signature);
				goto ERROR;
			}
			PrintNotify(("CI_CC_Signature_A done\n"));
			break;

		case CI_CC_Ns_Host:
			usDataLen = 8;
			// Ns_Host[8] -> CI_CC_CIPHER_BLOCK_SIZE 만큼의 공간을 alloc -> PRNG(Ns_Host) 1번 처리 -> truncated LSB to 8 bytes -> OK?
			pucData = (unsigned char *)CA_MEM_Alloc(sizeof(unsigned char) * CI_CC_CIPHER_BLOCK_SIZE);
			PRNG(s_stCiHostLicenseConst.PRNG_seed, pucData);
			VK_memcpy(hCc->stHostData.Ns_Host, pucData, usDataLen); // keep Ns_Host for further use
			#ifdef USE_SUB_TASK_TO_CALCULATE_KEYS
			if (CiCc_CalculateSacKeysOnSubTask(hCc) == CI_ARC_ERR_SAC_Failed)
			#else
			if (CiCc_CalculateSacKeys(hCc) == CI_ARC_ERR_SAC_Failed)
			#endif
			{
				PrintErr(("[CICC]---> CiCc_MakeReqData()---> Error : SAC failed !!!\n")); // 이런 상황이 발생하면 곤란. 디버깅 필요!!
				CiCc_DoActionForArcErr(hCc, CI_ARC_ERR_SAC_Failed);
			}
			hCc->enPhase = CI_CC_PHASE_SAC_Establishment_Replied;
			PrintNotify(("CI_CC_Ns_Host done\n"));
			break;

		case CI_CC_AKH:
			usDataLen = 32;
			pucData = (unsigned char *)CA_MEM_Alloc(sizeof(unsigned char) * usDataLen);
			if (CiCc_LoadAkh(hCc, pucData) < 0) // 저장된 AKH가 없는 경우 -> invalid AKH 를 보낸다 -> 새로 인증이 일어날 것이다...
			{
				VK_memset(pucData, 0x00, usDataLen);
			}
			hCc->enPhase = CI_CC_PHASE_Authentication_Key_Verification_Replied;
			PrintNotify(("CI_CC_AKH done\n"));
			break;

		case CI_CC_uri_confirm:
			usDataLen = 32;
			pucData = (unsigned char *)CA_MEM_Alloc(sizeof(unsigned char) * usDataLen);
			if (hCc->ulUriTimerID)
			{
				VK_TIMER_Cancel(hCc->ulUriTimerID);
				hCc->ulUriTimerID = 0;
			}
			CiCc_GenerateUriConfirm(hCc, pucData);
			CiCc_UriDoCopyCtrl(hCc, CI_CC_URI_MODE_FROMCAM);
			PrintNotify(("CI_CC_uri_confirm done\n"));
			break;

		case CI_CC_uri_versions:
			usDataLen = 32;
			pucData = (unsigned char *)CA_MEM_Alloc(sizeof(unsigned char) * usDataLen);
			VK_memset(pucData, 0x00, 31);
			pucData[31] = CI_CC_URI_PROTOCOL_DEFAULT;
			hCc->enPhase = CI_CC_PHASE_URI_Version_Negotiation_Replied;
			#if 0 // CAS/CI mgr 처리를 위한 콜백 -> 현재로선 딱히 처리할 것이 없어서 일단 막다둔다.
			s_stCcReportInfo.eCcReport = eCiPlus_CcReport_UriVersionNegotiationDone;
			Callback_Reset(g_hCbNotifyCcReport);
			while ((s_pfnNotifyCcReport = (CI_NOTIFY_CC_REPORT_FUNC)Callback_GetNextFunc(g_hCbNotifyCcReport)) != NULL)
			{
				(*s_pfnNotifyCcReport)(hCc->usSlotId, &s_stCcReportInfo);
			}
			#endif
			PrintNotify(("CI_CC_uri_versions done\n"));
			break;

		case CI_CC_status_field:
			usDataLen = 1;
			pucData = (unsigned char *)CA_MEM_Alloc(sizeof(unsigned char) * usDataLen);
			if (hCc->enPhase == CI_CC_PHASE_CCK_Calculating_Requested) // CCK's
			{
				#ifdef CI_CC_CalculateCckAndSetDescramblerBeforeSync
				#ifdef USE_SUB_TASK_TO_CALCULATE_KEYS
				if (CiCc_CalculateCckAndSetDescramblerOnSubTask(hCc, TRUE, TRUE) == CI_ARC_ERR_CCK_Failed)
				#else
				if (CiCc_CalculateCckAndSetDescrambler(hCc, TRUE, TRUE) == CI_ARC_ERR_CCK_Failed)
				#endif
				{
					PrintErr(("[CICC]---> CiCc_MakeReqData()---> Error : CCK failed !!!\n")); // 이런 상황이 발생하면 곤란. 디버깅 필요!!
					CiCc_DoActionForArcErr(hCc, CI_ARC_ERR_CCK_Failed);
				}
				#endif
				if (hCc->enArcErr != CI_ARC_ERR_NONE)
					pucData[0] = CI_CC_STATUS_NO_CC_SUPPORT;
				else
					pucData[0] = CI_CC_STATUS_OK;
				hCc->enPhase = CI_CC_PHASE_CCK_Calculating_Replied;
			}
			else if (hCc->enPhase == CI_CC_PHASE_Authentication_Requested) // Authentication's
			{
				CI_ARC_ERR err;
				err = CiCc_VerifyCamAndSaveInfo(hCc);
				if (err != CI_ARC_ERR_NONE)
				{
					PrintErr(("[CICC]---> CiCc_MakeReqData()---> Error : CiCc_VerifyCamAndSaveInfo() return %d !!!\n", err));
					CiCc_DoActionForArcErr(hCc, err);
				}
				else
				{
					if (CiCc_GenerateHostNonVolatileKeys(hCc) != CI_ARC_ERR_NONE)
					{
						PrintErr(("[CICC]---> CiCc_MakeReqData()---> Error : host non-volatile keys !!!\n")); // 이런 상황이 발생하면 곤란. 디버깅 필요!!
						CiCc_DoActionForArcErr(hCc, CI_ARC_ERR_Auth_Failed_Key_Computation);
					}
				}
				if (hCc->enArcErr != CI_ARC_ERR_NONE)
				{
					pucData[0] = CI_CC_STATUS_AUTH_FAILED_OR_NOT_REQ;
				}
				else
				{
					pucData[0] = CI_CC_STATUS_OK;

					#if 0 // UI 처리를 위한 콜백 -> SMiT 요청으로 이 경우의 배너 표시를 하지 않도록 수정함.
					s_stCcReportInfo.eCcReport = eCiPlus_CcReport_AuthenticationSuccess;
					Callback_Reset(g_hCbNotifyCcReport);
					while ((s_pfnNotifyCcReport = (CI_NOTIFY_CC_REPORT_FUNC)Callback_GetNextFunc(g_hCbNotifyCcReport)) != NULL)
					{
						(*s_pfnNotifyCcReport)(hCc->usSlotId, &s_stCcReportInfo);
					}
					#endif
				}
				hCc->enPhase = CI_CC_PHASE_Authentication_Replied;
			}
			else // 여기 온다는 것은 뭔가 스펙대로 운용되지 않는다는 것이다 (일단 CI_CC_STATUS_HOST_BUSY 리턴). 디버깅 필요!! (SRM transmission and acknowledgement 과 관련될 수 있다)
			{
				pucData[0] = CI_CC_STATUS_HOST_BUSY;
				PrintErr(("[CICC]---> CiCc_MakeReqData()---> Error : Invalid phase[%d] !!!\n", hCc->enPhase));
			}
			PrintNotify(("CI_CC_status_field done (0x%x)\n", pucData[0]));
			break;

		/* 이하 data id 들은 호스트에서 보내지지 않는 것들이거나 CI+ 스펙 1.2 프로토콜 상에 실제 운용되지 않는 것들 */
		case CI_CC_Brand_ID:
		case CI_CC_CICAM_ID:
		case CI_CC_CICAM_BrandCert:
		case CI_CC_Kp:
		case CI_CC_DHPM:
		case CI_CC_CICAM_DevCert:
		case CI_CC_Signature_B:
		case CI_CC_auth_nonce:
		case CI_CC_Ns_module:
		case CI_CC_AKM:
		case CI_CC_uri_message:
		case CI_CC_program_number:
		case CI_CC_key_register:
		case CI_CC_srm_data:
		case CI_CC_srm_confirm:
			PrintErr(("[CICC]---> CiCc_MakeReqData()---> Error : Invalid ReqDataId[%d] !!!\n", ucReqDataId)); // 이 경우는 처리하지 않고 무시
			break;

		default:
			PrintErr(("[CICC]---> CiCc_MakeReqData()---> Error : Unknown ReqDataId[%d] !!!\n", ucReqDataId)); // 이 경우는 처리하지 않고 무시
			break;
	}

	*pusReqDataLen = usDataLen;
	*ppucReqData = pucData;
	return;

ERROR : // 이런 상황이 발생하면 곤란. 디버깅 필요!!

	if (pucData)
		CA_MEM_Free(pucData);
	*pusReqDataLen = 0;
	*ppucReqData = NULL;
	return;
}

/******************************************/
/*** Local functions for Authentication ***/
/******************************************/

static CI_ARC_ERR CiCc_GenerateSignature(CI_CC_INSTANCE *hCc, unsigned char *pucSignature)
{
	unsigned char buf[302];
	RSA_private_key HDQ;
	bignum n, d, p, q, dP, dQ, qInv;
	bignum M, S;
	bignum salt = {20, {0x62,0x79,0x20,0x68,0x6D,0x6B,0x69,0x6D,0x2C,0x20,0x68,0x75,0x6D,0x61,0x78,0x2E,0x2E,0x2E,0x2E,0x2E}};

	#ifdef _CI_DEBUG_CC_FUNC_TIME_
	s_ulCcFuncStartTime = VK_TIMER_GetSystemTick();
	#endif

	if (s_stCiHostLicenseConst.ucSavedFlag == 1)
	{
		bn_init(&n, s_stCiHostLicenseConst.HDQ_n, CI_HDQ_N_LEN);
		bn_init(&d, s_stCiHostLicenseConst.HDQ_d, CI_HDQ_D_LEN);
//		bn_init(&p, s_stCiHostLicenseConst.HDQ_p, CI_HDQ_P_LEN);
//		bn_init(&q, s_stCiHostLicenseConst.HDQ_q, CI_HDQ_Q_LEN);
//		bn_init(&dP, s_stCiHostLicenseConst.HDQ_dP, CI_HDQ_DP_LEN);
//		bn_init(&dQ, s_stCiHostLicenseConst.HDQ_dQ, CI_HDQ_DQ_LEN);
//		bn_init(&qInv, s_stCiHostLicenseConst.HDQ_qInv, CI_HDQ_QINV_LEN);

		HDQ.n = &n;
		HDQ.d = &d;
		HDQ.p = NULL;
		HDQ.q = NULL;
		HDQ.dP = NULL;
		HDQ.dQ = NULL;
		HDQ.qInv = NULL;
	}
	else
	{
		bn_init(&n, s_stCiHostLicenseConst.HDQ_n, CI_HDQ_N_LEN);
//		bn_init(&d, s_stCiHostLicenseConst.HDQ_d, CI_HDQ_D_LEN);
		bn_init(&p, s_stCiHostLicenseConst.HDQ_p, CI_HDQ_P_LEN);
		bn_init(&q, s_stCiHostLicenseConst.HDQ_q, CI_HDQ_Q_LEN);
		bn_init(&dP, s_stCiHostLicenseConst.HDQ_dP, CI_HDQ_DP_LEN);
		bn_init(&dQ, s_stCiHostLicenseConst.HDQ_dQ, CI_HDQ_DQ_LEN);
		bn_init(&qInv, s_stCiHostLicenseConst.HDQ_qInv, CI_HDQ_QINV_LEN);

		HDQ.n = &n;
		HDQ.d = NULL;
		HDQ.p = &p;
		HDQ.q = &q;
		HDQ.dP = &dP;
		HDQ.dQ = &dQ;
		HDQ.qInv = &qInv;
	}

	buf[0] = CI_CC_TLF_TAG_VER;
	buf[1] = (unsigned char)((CI_CC_TLF_LEN_VER >> 8) & 0xff);
	buf[2] = (unsigned char)(CI_CC_TLF_LEN_VER & 0xff);
	buf[3] = CI_CC_TLF_VER_DEFAULT;

	buf[4] = CI_CC_TLF_TAG_MSG_LABEL;
	buf[5] = (unsigned char)((CI_CC_TLF_LEN_MSG_LABEL >> 8) & 0xff);
	buf[6] = (unsigned char)(CI_CC_TLF_LEN_MSG_LABEL & 0xff);
	buf[7] = CI_CC_TLF_MSG_LABEL_HOST_A;

	buf[8] = CI_CC_TLF_TAG_AUTH_NONCE;
	buf[9] = (unsigned char)((CI_CC_TLF_LEN_AUTH_NONCE >> 8) & 0xff);
	buf[10] = (unsigned char)(CI_CC_TLF_LEN_AUTH_NONCE & 0xff);
	VK_memcpy(&buf[11], hCc->stCicamData.auth_nonce, 32); // 이 시점에 auth_nonce 값은 유효할 것이다.

	buf[43] = CI_CC_TLF_TAG_DHPH;
	buf[44] = (unsigned char)((CI_CC_TLF_LEN_DHPH >> 8) & 0xff);
	buf[45] = (unsigned char)(CI_CC_TLF_LEN_DHPH & 0xff);
	VK_memcpy(&buf[46], s_stCiHostLicenseConst.DHPH, 256);

	bn_init(&M, buf, 302);

	if (A_RSA(&HDQ, &M, &salt, &S))
	{
		PrintErr(("[CICC]---> CiCc_GenerateSignature()---> Error : A_RSA !!!\n"));
		return CI_ARC_ERR_Auth_Failed_Invalid_Signature;
	}

	if (S.len != 256)
	{
		PrintErr(("[CICC]---> CiCc_GenerateSignature()---> Error : Invalid len (%d) !!!\n", S.len));
		return CI_ARC_ERR_Auth_Failed_Invalid_Signature;
	}

	VK_memcpy(pucSignature, S.num, 256);

	#ifdef _CI_DEBUG_CC_FUNC_TIME_
	s_ulCcFuncEndTime = VK_TIMER_GetSystemTick();
	VK_Print("[CICC]---> CiCc_GenerateSignature() : %ld ms\n", s_ulCcFuncEndTime - s_ulCcFuncStartTime);
	#endif

	#if 0 // #ifdef _CI_DEBUG_CC_DATA_
	{
		bn_len_type i;
		VK_Print("\n CiCc_GenerateSignature : M (%d)", M.len);
		for (i = 0; i < M.len; i++)
		{
			if ((i & 15) == 0)
				VK_Print("\n");
			VK_Print(" %02x", M.num[i]);
		}
		VK_Print("\n CiCc_GenerateSignature : S (%d)", S.len);
		for (i = 0; i < S.len; i++)
		{
			if ((i & 15) == 0)
				VK_Print("\n");
			VK_Print(" %02x", S.num[i]);
		}
		VK_Print("\n\n");
	}
	#endif

	#ifdef _CI_DEBUG_CC_SELF_CHECKING_
	{
		RSA_public_key HDP;
		HDP.n = &n;
		HDP.e = &bn_e;
		if (!V_RSA(&HDP, &M, &S))
			VK_Print("[CICC]---> CiCc_GenerateSignature()---> Error : Invalid Signature !!!\n");
	}
	#endif

	return CI_ARC_ERR_NONE;
}

static CI_ARC_ERR CiCc_VerifyCamAndSaveInfo(CI_CC_INSTANCE *hCc)
{
	Certificate_t * pstRootCertData = NULL;
	Certificate_t * pstCamBrandCertData = NULL;
	Certificate_t * pstCamDevCertData = NULL;

	unsigned char	ucBindedCamId[8];
	unsigned short	usBindedCamBrandId = 0;
	int nScramblerCapabilities, nNegotiatedCckCipher;
	CI_AUTH_CTX * pstCiAuthCtx = NULL;
	int i, ret;

	CI_ARC_ERR err = CI_ARC_ERR_NONE;

	/*--- Verify DHPM ---*/
	{
		bignum bn_DHPM, bn_DH_p, bn_DH_q, out;

		#ifdef _CI_DEBUG_CC_FUNC_TIME_
		s_ulCcFuncStartTime = VK_TIMER_GetSystemTick();
		#endif

		// The host verifies that the DHPM key received from the CICAM is valid by checking the length and values : if 1 < DHPM < DH_p and DHPM ^ DH_q mod DH_p = 1
		bn_init(&bn_DHPM, hCc->stCicamData.DHPM, 256); // 이 시점에 DHPM 값은 유효할 것이다.
		bn_init(&bn_DH_p, s_stCiHostLicenseConst.DH_p, CI_DH_P_LEN);
		bn_init(&bn_DH_q, s_stCiHostLicenseConst.DH_q, CI_DH_Q_LEN);
		if (bn_cmp(&bn_one, &bn_DHPM) >= 0)
		{
			PrintErr(("[CICC]---> CiCc_VerifyCamAndSaveInfo()---> Error : DHPM not bigger than 1 !!!\n"));
			err = CI_ARC_ERR_Auth_Failed_DH;
			goto EXIT;
		}
		if (bn_cmp(&bn_DHPM, &bn_DH_p) >= 0)
		{
			PrintErr(("[CICC]---> CiCc_VerifyCamAndSaveInfo()---> Error : DHPM not less than DH_p !!!\n"));
			err = CI_ARC_ERR_Auth_Failed_DH;
			goto EXIT;
		}
		bn_mod_exp(&bn_DHPM, &bn_DH_q, &bn_DH_p, &out);
		if (bn_cmp(&out, &bn_one) != 0)
		{
			PrintErr(("[CICC]---> CiCc_VerifyCamAndSaveInfo()---> Error : DHPM ^ DH_q mod DH_p not 1 !!!\n"));
			err = CI_ARC_ERR_Auth_Failed_DH;
			goto EXIT;
		}

		#ifdef _CI_DEBUG_CC_FUNC_TIME_
		s_ulCcFuncEndTime = VK_TIMER_GetSystemTick();
		VK_Print("[CICC]---> CiCc_VerifyCamAndSaveInfo() for DHPM : %ld ms\n", s_ulCcFuncEndTime - s_ulCcFuncStartTime);
		#endif
	}

	/*--- Verify RootCert ---*/
	{
		pstRootCertData = X509_ParseRawCert(s_stCiHostLicenseConst.RootCert, s_stCiHostLicenseConst.RootCertLen);
		if (pstRootCertData == NULL)
		{
			PrintErr(("[CICC]---> CiCc_VerifyCamAndSaveInfo()---> Error : Invalid RootCert !!!\n"));
			err = CI_ARC_ERR_Not_Defined;
			goto EXIT;
		}
	}

	/*--- Verify CICAM_BrandCert ---*/
	{
		pstCamBrandCertData = X509_ParseRawCert(hCc->stCicamData.CICAM_BrandCert, hCc->stCicamData.CICAM_BrandCertLen);
		if (pstCamBrandCertData == NULL)
		{
			PrintErr(("[CICC]---> CiCc_VerifyCamAndSaveInfo()---> Error : Invalid BrandCert !!!\n"));
			err = CI_ARC_ERR_CAM_Cert_Syntax_Incorrect;
			goto EXIT;
		}

		/* 1) Check version (CI+ implementations shall use X.509 version 3 (02)).
		   2) Check that the Issuer of the brand certificate is identical to the Subject of the root certificate.
		   3) Check that the validity period of the brand certificate includes the current date and time.
		   4) Check that each mandatory extension listed in section 9.3.9 exists and the values are valid.
		      Check that no other extension is marked as critical.
		   5) Verify that the KeyIdentifier in the brand certificate's authority key identifier extension
		      is identical to the KeyIdentifier in the root certificate's subject key identifier extension.
		   6) Verify the certificate's signature by using the RSASSA-PSS verification. */

		ret = VerificateBrandCertificate(pstCamBrandCertData, pstRootCertData);
		if (ret)
		{
			PrintErr(("[CICC]---> CiCc_VerifyCamAndSaveInfo()---> Error : Invalid BrandCert (%d) !!!\n", ret));
			err = (ret == -3) ? CI_ARC_ERR_CAM_Cert_Expired : CI_ARC_ERR_CAM_Cert_Syntax_Incorrect;
			goto EXIT;
		}

		if (FALSE == IsSignatureValid(pstRootCertData, pstCamBrandCertData, hCc->stCicamData.CICAM_BrandCert, hCc->stCicamData.CICAM_BrandCertLen))
		{
			PrintErr(("[CICC]---> CiCc_VerifyCamAndSaveInfo()---> Error : Invalid BrandCert Signature !!!\n"));
			err = CI_ARC_ERR_CAM_Cert_Invalid_Signature;
			goto EXIT;
		}
	}

	/*--- Verify CICAM_DevCert ---*/
	{
		pstCamDevCertData = X509_ParseRawCert(hCc->stCicamData.CICAM_DevCert, hCc->stCicamData.CICAM_DevCertLen);
		if (pstCamDevCertData == NULL)
		{
			PrintErr(("[CICC]---> CiCc_VerifyCamAndSaveInfo()---> Error : Invalid DevCert !!!\n"));
			err = CI_ARC_ERR_CAM_Cert_Syntax_Incorrect;
			goto EXIT;
		}

		/* 1) Check version (CI+ implementations shall use X.509 version 3 (02)).
		   2) Check that the Issuer of the device certificate is identical to the Subject of the brand certificate.
		   3) Check that the validity period of the device certificate includes the current date and time.
		   4) Check that each mandatory extension listed in CI+ spec section 9.3.9 exists and the values are valid.
		      Check that no other extension is marked as critical.
		   5) Verify that the KeyIdentifier in the device certificate's authority key identifier extension
		      is identical to the KeyIdentifier in the brand certificate's subject key identifier extension.
		   6) Verify the certificate's signature by using the RSASSA-PSS verification.
		   7) Verify that the device ID (which is part of the Subject field) contains a valid value. See CI+ spec Annex B and C.
		   8) ucBindedCamId, usBindedCamBrandId, nNegotiatedCckCipher (negotiate with CAM's and s_stCiHostLicenseConst.Host_CckCipher) 셋팅 */

		ret = VerificateDeviceCertificate(pstCamDevCertData, pstCamBrandCertData);
		if (ret)
		{
			PrintErr(("[CICC]---> CiCc_VerifyCamAndSaveInfo()---> Error : Invalid DevCert (%d) !!!\n", ret));
			err = (ret == -3) ? CI_ARC_ERR_CAM_Cert_Expired : CI_ARC_ERR_CAM_Cert_Syntax_Incorrect;
			goto EXIT;
		}

		if (FALSE == IsSignatureValid(pstCamBrandCertData, pstCamDevCertData, hCc->stCicamData.CICAM_DevCert, hCc->stCicamData.CICAM_DevCertLen))
		{
			PrintErr(("[CICC]---> CiCc_VerifyCamAndSaveInfo()---> Error : Invalid DevCert Signature !!!\n"));
			err = CI_ARC_ERR_CAM_Cert_Invalid_Signature;
			goto EXIT;
		}

		// ucBindedCamId 설정
		if (FALSE == GetDeviceID(pstCamDevCertData, ucBindedCamId))
		{
			PrintErr(("[CICC]---> CiCc_VerifyCamAndSaveInfo()---> Error : GetDeviceID() !!!\n"));
			err = CI_ARC_ERR_CAM_Cert_Syntax_Incorrect;
			goto EXIT;
		}

		// usBindedCamBrandId 설정
		if (FALSE == GetBrandID(pstCamDevCertData, &usBindedCamBrandId))
		{
			PrintErr(("[CICC]---> CiCc_VerifyCamAndSaveInfo()---> Error : GetBrandID() !!!\n"));
			err = CI_ARC_ERR_CAM_Cert_Syntax_Incorrect;
			goto EXIT;
		}

		// nNegotiatedCckCipher 설정
		if (FALSE == GetScramblerCapabilities(pstCamDevCertData, &nScramblerCapabilities))
		{
			PrintErr(("[CICC]---> CiCc_VerifyCamAndSaveInfo()---> Error : GetScramblerCapabilities() !!!\n"));
			err = CI_ARC_ERR_CAM_Cert_Syntax_Incorrect;
			goto EXIT;
		}
		if (nScramblerCapabilities == 0 || nScramblerCapabilities == 1)
		{
			nNegotiatedCckCipher = nScramblerCapabilities & s_stCiHostLicenseConst.Host_CckCipher;
		}
		else // unknown value (reserved for future use) -> CI+ 스펙 1.2 기준에서 정의되지 않은 값일 경우엔 일단 CI_CC_KEY_CIPHER_DES_56_ECB 로 설정하도록 하자...
		{
			PrintErr(("[CICC]---> CiCc_VerifyCamAndSaveInfo()---> Error : GetScramblerCapabilities()---> unknown value %d !!!\n", nScramblerCapabilities));
			nNegotiatedCckCipher = CI_CC_KEY_CIPHER_DES_56_ECB;
		}
	}

	/*--- Verify Signature_B ---*/
	{
		unsigned char buf[561];
		RSA_public_key MDP;
		bignum n, M, S;

		#ifdef _CI_DEBUG_CC_FUNC_TIME_
		s_ulCcFuncStartTime = VK_TIMER_GetSystemTick();
		#endif

		if (FALSE == GetRSAPublicKey_N(pstCamDevCertData, n.num, &n.len))
		{
			PrintErr(("[CICC]---> CiCc_VerifyCamAndSaveInfo()---> Error : GetRSAPublicKey_N() !!!\n"));
			err = CI_ARC_ERR_Auth_Failed_Invalid_Signature;
			goto EXIT;
		}

		MDP.n = &n;
		MDP.e = &bn_e;

		buf[0] = CI_CC_TLF_TAG_VER;
		buf[1] = (unsigned char)((CI_CC_TLF_LEN_VER >> 8) & 0xff);
		buf[2] = (unsigned char)(CI_CC_TLF_LEN_VER & 0xff);
		buf[3] = CI_CC_TLF_VER_DEFAULT;

		buf[4] = CI_CC_TLF_TAG_MSG_LABEL;
		buf[5] = (unsigned char)((CI_CC_TLF_LEN_MSG_LABEL >> 8) & 0xff);
		buf[6] = (unsigned char)(CI_CC_TLF_LEN_MSG_LABEL & 0xff);
		buf[7] = CI_CC_TLF_MSG_LABEL_HOST_V;

		buf[8] = CI_CC_TLF_TAG_AUTH_NONCE;
		buf[9] = (unsigned char)((CI_CC_TLF_LEN_AUTH_NONCE >> 8) & 0xff);
		buf[10] = (unsigned char)(CI_CC_TLF_LEN_AUTH_NONCE & 0xff);
		VK_memcpy(&buf[11], hCc->stCicamData.auth_nonce, 32); // 이 시점에 auth_nonce 값은 유효할 것이다.

		buf[43] = CI_CC_TLF_TAG_DHPH;
		buf[44] = (unsigned char)((CI_CC_TLF_LEN_DHPH >> 8) & 0xff);
		buf[45] = (unsigned char)(CI_CC_TLF_LEN_DHPH & 0xff);
		VK_memcpy(&buf[46], s_stCiHostLicenseConst.DHPH, 256);

		buf[302] = CI_CC_TLF_TAG_DHPM;
		buf[303] = (unsigned char)((CI_CC_TLF_LEN_DHPM >> 8) & 0xff);
		buf[304] = (unsigned char)(CI_CC_TLF_LEN_DHPM & 0xff);
		VK_memcpy(&buf[305], hCc->stCicamData.DHPM, 256); // 이 시점에 DHPM 값은 유효할 것이다.

		bn_init(&M, buf, 561);
		bn_init(&S, hCc->stCicamData.Signature_B, 256); // 이 시점에 Signature_B 값은 유효할 것이다.

		if (!V_RSA(&MDP, &M, &S))
		{
			PrintErr(("[CICC]---> CiCc_VerifyCamAndSaveInfo()---> Error : Invalid Signature !!!\n"));
			err = CI_ARC_ERR_Auth_Failed_Invalid_Signature;
			goto EXIT;
		}

		#ifdef _CI_DEBUG_CC_FUNC_TIME_
		s_ulCcFuncEndTime = VK_TIMER_GetSystemTick();
		VK_Print("[CICC]---> CiCc_VerifyCamAndSaveInfo() for Signature_B : %ld ms\n", s_ulCcFuncEndTime - s_ulCcFuncStartTime);
		#endif
	}

	/*--- save ucBindedCamId, usBindedCamBrandId and nNegotiatedCckCipher to non-volatile memory (shift saved authentication contexts and save new one to first) ---*/
	for (i = CI_NUM_OF_AUTH_CTX - 1; i > 0; i--)
	{
		if ((pstCiAuthCtx = CiCc_GetAuthContext(hCc->usSlotId, i-1)) == NULL)
		{
			PrintErr(("[CICC]---> CiCc_VerifyCamAndSaveInfo()---> Error : Can't load context !!!\n"));
			err = CI_ARC_ERR_Not_Defined;
			goto EXIT;
		}
		// 아래 주석 코드와 같이 ucBindedCamId 도 체크하는 것이 맞겠지만
		// SmarDTV CI+ 테스트 킷 캠 2개가 모두 같은 디바이스 인증서를 사용하므로 ucBindedCamId 를 체크하게 되면 캠을 바꿀 때마다 다시 바인딩해야 한다.
		// 따라서 일단 ucBindedCamId 는 체크하지 않도록 한다...
		if (pstCiAuthCtx->ucSavedFlag == 2) // if ((pstCiAuthCtx->ucSavedFlag == 2) && (VK_memcmp(pstCiAuthCtx->ucBindedCamId, ucBindedCamId, 8) != 0))
		{
			if (CiCc_PutAuthContext(hCc->usSlotId, i, pstCiAuthCtx))
			{
				PrintErr(("[CICC]---> CiCc_VerifyCamAndSaveInfo()---> Error : Can't shift context !!!\n"));
				err = CI_ARC_ERR_Not_Defined;
				goto EXIT;
			}
		}
	}
	if ((pstCiAuthCtx = CiCc_GetAuthContext(hCc->usSlotId, 0)) == NULL)
	{
		PrintErr(("[CICC]---> CiCc_VerifyCamAndSaveInfo()---> Error : Can't load first context !!!\n"));
		err = CI_ARC_ERR_Not_Defined;
		goto EXIT;
	}
	VK_memset(pstCiAuthCtx, 0, sizeof(CI_AUTH_CTX));
	VK_memcpy(pstCiAuthCtx->ucBindedCamId, ucBindedCamId, 8);
	pstCiAuthCtx->usBindedCamBrandId = usBindedCamBrandId;
	pstCiAuthCtx->nNegotiatedCckCipher = nNegotiatedCckCipher;
	pstCiAuthCtx->ucSavedFlag = 1;
//	여기선 저장할 필요가 없지...
//	if (CI_SaveAuthContexts(hCc->usSlotId, &s_stCiAuthCtxs[hCc->usSlotId]))
//	{
//		PrintErr(("[CICC]---> CiCc_VerifyCamAndSaveInfo()---> Error : Can't save context !!!\n"));
//		err = CI_ARC_ERR_Not_Defined;
//		goto EXIT;
//	}
	hCc->nCurAuthCtxIdx = hCc->nTryAuthCtxIdx = 0;

EXIT :

	if (pstRootCertData)
		X509_FreeCert(pstRootCertData);
	if (pstCamBrandCertData)
		X509_FreeCert(pstCamBrandCertData);
	if (pstCamDevCertData)
		X509_FreeCert(pstCamDevCertData);

	return err;
}

/**************************************************/
/*** Local functions for host non-volatile keys ***/
/**************************************************/

static int CiCc_LoadAkh(CI_CC_INSTANCE *hCc, unsigned char *pucAkh)
{
	CI_AUTH_CTX * pstCiAuthCtx = NULL;
	int i;

	for (i = hCc->nTryAuthCtxIdx; i < CI_NUM_OF_AUTH_CTX; i++)
	{
		if ((pstCiAuthCtx = CiCc_GetAuthContext(hCc->usSlotId, i)) == NULL)
		{
			PrintErr(("[CICC]---> CiCc_LoadAkh()---> Error : Can't load context !!!\n")); // 이런 상황이 발생하면 곤란. 디버깅 필요!!
			return -1;
		}
		if (pstCiAuthCtx->ucSavedFlag == 2)
			break;
	}

	if (i == CI_NUM_OF_AUTH_CTX)
	{
		/* <TODO_CI_PLUS> : 2 슬롯 모델의 경우 기존에 바인딩되어 있는 캠이 다른 슬롯으로 꽂히는 경우를 고려할 필요가 있다...
		                    만약 해당 슬롯에 저장된 authentication contexts가 CI_NUM_OF_AUTH_CTX 개 미만이라면 hCc->usSlotId^0x1 도 시도할 필요가 있다... */
		PrintErr(("[CICC]---> CiCc_LoadAkh()---> Error : Can't load AKH or AKH not saved !!!\n"));
		return -1;
	}

	hCc->nCurAuthCtxIdx = i;
	hCc->nTryAuthCtxIdx = i+1;

	VK_memcpy(pucAkh, pstCiAuthCtx->AKH, 32);

	return ERR_OK;
}

static CI_ARC_ERR CiCc_GenerateHostNonVolatileKeys(CI_CC_INSTANCE *hCc) /* 여기서 DHSK는 원래 DHSK의 LSB 128 bits 만 저장 */
{
	bignum bn_DHPM, bn_DHH, bn_DH_p, bn_DHSK;
	unsigned char AKH[32];
	unsigned char M[8+8+256];
	CI_AUTH_CTX * pstCiAuthCtx = NULL;

	#ifdef _CI_DEBUG_CC_FUNC_TIME_
	s_ulCcFuncStartTime = VK_TIMER_GetSystemTick();
	#endif

	// compute DHSK
	bn_init(&bn_DHPM, hCc->stCicamData.DHPM, 256); // 이 시점에 DHPM 값은 유효할 것이다.
	bn_init(&bn_DHH, s_stCiHostLicenseConst.DHH, 256);
	bn_init(&bn_DH_p, s_stCiHostLicenseConst.DH_p, CI_DH_P_LEN);
	bn_mod_exp(&bn_DHPM, &bn_DHH, &bn_DH_p, &bn_DHSK);
	if (bn_DHSK.len != 256)
	{
		PrintErr(("[CICC]---> CiCc_GenerateHostNonVolatileKeys()---> Error : Invalid DHSK len (%d) !!!\n", bn_DHSK.len));
		return CI_ARC_ERR_Auth_Failed_Key_Computation;
	}

	// load context
	if ((pstCiAuthCtx = CiCc_GetAuthContext(hCc->usSlotId, hCc->nCurAuthCtxIdx)) == NULL)
	{
		PrintErr(("[CICC]---> CiCc_GenerateHostNonVolatileKeys()---> Error : Can't load context !!!\n"));
		return CI_ARC_ERR_Auth_Failed_Key_Computation;
	}
	if (pstCiAuthCtx->ucSavedFlag != 1)
	{
		PrintErr(("[CICC]---> CiCc_GenerateHostNonVolatileKeys()---> Error : authentication context not saved !!!\n"));
		return CI_ARC_ERR_Auth_Failed_Key_Computation;
	}

	// compute AKH
	VK_memcpy(M, pstCiAuthCtx->ucBindedCamId, 8); // 이 시점에 ucBindedCamId 값은 유효할 것이다.
	VK_memcpy(&M[8], s_stCiHostLicenseConst.Host_ID, 8);
	VK_memcpy(&M[16], bn_DHSK.num, bn_DHSK.len);
	SHA_256(M, 8+8+bn_DHSK.len, AKH);

	// save DHSK' and AKH to non-volatile memory
	VK_memcpy(pstCiAuthCtx->DHSK, &bn_DHSK.num[bn_DHSK.len-16], 16);
	VK_memcpy(pstCiAuthCtx->AKH, AKH, 32);
	pstCiAuthCtx->ucSavedFlag = 2;
	if (CI_SaveAuthContexts(hCc->usSlotId, &s_stCiAuthCtxs[hCc->usSlotId]))
	{
		PrintErr(("[CICC]---> CiCc_GenerateHostNonVolatileKeys()---> Error : Can't save DHSK and AKH !!!\n"));
		return CI_ARC_ERR_Auth_Failed_Key_Computation;
	}

	#ifdef _CI_DEBUG_CC_FUNC_TIME_
	s_ulCcFuncEndTime = VK_TIMER_GetSystemTick();
	VK_Print("[CICC]---> CiCc_GenerateHostNonVolatileKeys() : %ld ms\n", s_ulCcFuncEndTime - s_ulCcFuncStartTime);
	#endif

	return CI_ARC_ERR_NONE;
}

/********************************************************/
/*** Local functions for calculating keys (SAC / CCK) ***/
/********************************************************/

/* calculating the new SAC key material - SEK for de/en-cryption and SAK for authentication or uri_confirm (host side) */
#ifdef USE_SUB_TASK_TO_CALCULATE_KEYS
static CI_ARC_ERR CiCc_CalculateSacKeysOnSubTask(CI_CC_INSTANCE *hCc)
{
	CI_CC_SUB_TASK_MSG msg;

	#ifdef _CI_DEBUG_CC_TIME_ISSUE_
	s_bCalculateSacDone = 0;
	#endif

	msg.command = CI_CC_DO_CALCULATE_SAC;
	msg.hCc = hCc;

	if (VK_MSG_Send(s_ulCiCcSubMsgQId, &msg, sizeof(CI_CC_SUB_TASK_MSG)))
	{
		PrintErr(("[CICC]---> CiCc_CalculateSacKeysOnSubTask()---> Error : fail to send msg !!!\n"));
		return CI_ARC_ERR_SAC_Failed;
	}

	return CI_ARC_ERR_NONE;
}
#endif
static CI_ARC_ERR CiCc_CalculateSacKeys(CI_CC_INSTANCE *hCc)
{
	CI_AUTH_CTX * pstCiAuthCtx = NULL;
	unsigned char M[16+32+8+8];
	unsigned char Ks_Host[32];
	unsigned char Km[32];
	int i;

	#ifdef _CI_DEBUG_CC_FUNC_TIME_
	s_ulCcFuncStartTime = VK_TIMER_GetSystemTick();
	#endif

	// load context
	if ((pstCiAuthCtx = CiCc_GetAuthContext(hCc->usSlotId, hCc->nCurAuthCtxIdx)) == NULL)
	{
		PrintErr(("[CICC]---> CiCc_CalculateSacKeys()---> Error : Can't load context !!!\n"));
		return CI_ARC_ERR_SAC_Failed;
	}
	if (pstCiAuthCtx->ucSavedFlag != 2)
	{
		PrintErr(("[CICC]---> CiCc_CalculateSacKeys()---> Error : authentication context not saved !!!\n"));
		return CI_ARC_ERR_SAC_Failed;
	}

	// Key seed calculation : compute Ks_Host
	VK_memcpy(M, pstCiAuthCtx->DHSK, 16); // 이 시점에 DHSK 값은 유효할 것이다.
	VK_memcpy(&M[16], pstCiAuthCtx->AKH, 32); // 이 시점에 AKH 값은 유효할 것이다.
	VK_memcpy(&M[48], hCc->stHostData.Ns_Host, 8); // 이 시점에 Ns_Host 값은 유효할 것이다.
	VK_memcpy(&M[56], hCc->stCicamData.Ns_module, 8); // 이 시점에 Ns_module 값은 유효할 것이다.
	SHA_256(M, 16+32+8+8, Ks_Host);

	// Key material computation : compute Km
	#if defined(CI_PLUS_TEST_Technology_from_TrustCenter) || defined(CI_PLUS_TEST_Technology_from_SmarDTV)
	SHA_256(Ks_Host, 32, Km);
	for (i = 0; i < 16; i++)
		Km[i] ^= s_stCiHostLicenseConst.SLK[i];
	for (i = 0; i < 16; i++)
		Km[16+i] ^= s_stCiHostLicenseConst.SLK[i];
	#else // CI_PLUS_PRODUCTION_Credentials : CI Plus License Specification 에는 AES-128-CBC 라고 되어 있으나 IV 가 없어 사실상 AES-128-ECB 라고 판단된다 -> v1.6r2 기준에서 ECB 맞다.
	E_AES_ECB(s_stCiHostLicenseConst.SLK, Ks_Host, Km);
	E_AES_ECB(s_stCiHostLicenseConst.SLK, &Ks_Host[16], &Km[16]);
	for (i = 0; i < 32; i++)
		Km[i] ^= Ks_Host[i];
	#endif

	// SEK and SAK key derivation : compute SEK, SAK
	VK_memcpy(hCc->stHostData.SEK, Km, 16); // keep SEK for further use
	VK_memcpy(hCc->stHostData.SAK, &Km[16], 16); // keep SAK for further use

	// UCK
	SHA_256(hCc->stHostData.SAK, 16, hCc->stHostData.UCK);

	#if defined(USE_SUB_TASK_TO_CALCULATE_KEYS) && defined(_CI_DEBUG_CC_TIME_ISSUE_)
	s_bCalculateSacDone = 1;
	#endif

	#ifdef _CI_DEBUG_CC_FUNC_TIME_
	s_ulCcFuncEndTime = VK_TIMER_GetSystemTick();
	VK_Print("[CICC]---> CiCc_CalculateSacKeys() : %ld ms\n", s_ulCcFuncEndTime - s_ulCcFuncStartTime);
	#endif

	return CI_ARC_ERR_NONE;
}

/* calculating the new CC key (host side) and set descrambler */
#ifdef USE_SUB_TASK_TO_CALCULATE_KEYS
static CI_ARC_ERR CiCc_CalculateCckAndSetDescramblerOnSubTask(CI_CC_INSTANCE *hCc, HBOOL bDoCalculateCck, HBOOL bDoSetDescrambler)
{
	CI_CC_SUB_TASK_MSG msg;

	#ifdef _CI_DEBUG_CC_TIME_ISSUE_
	if (bDoCalculateCck)
		s_bCalculateCckDone = 0;
	#endif

	msg.command = CI_CC_DO_CALCULATE_CCK;
	msg.hCc = hCc;
	msg.bDoCalculateCck = bDoCalculateCck;
	msg.bDoSetDescrambler = bDoSetDescrambler;

	if (VK_MSG_Send(s_ulCiCcSubMsgQId, &msg, sizeof(CI_CC_SUB_TASK_MSG)))
	{
		PrintErr(("[CICC]---> CiCc_CalculateCckAndSetDescramblerOnSubTask()---> Error : fail to send msg !!!\n"));
		return CI_ARC_ERR_CCK_Failed;
	}

	return CI_ARC_ERR_NONE;
}
#endif
static CI_ARC_ERR CiCc_CalculateCckAndSetDescrambler(CI_CC_INSTANCE *hCc, HBOOL bDoCalculateCck, HBOOL bDoSetDescrambler)
{
	CI_AUTH_CTX * pstCiAuthCtx = NULL;
	unsigned char Km[32];
	unsigned char * CCK = hCc->stHostData.CCK;
	unsigned char * CIV = hCc->stHostData.CIV;
	int i, j, cnt;

	#ifdef _CI_DEBUG_CC_FUNC_TIME_
	s_ulCcFuncStartTime = VK_TIMER_GetSystemTick();
	#endif

	// load context
	if ((pstCiAuthCtx = CiCc_GetAuthContext(hCc->usSlotId, hCc->nCurAuthCtxIdx)) == NULL)
	{
		PrintErr(("[CICC]---> CiCc_CalculateCckAndSetDescrambler()---> Error : Can't load context !!!\n"));
		return CI_ARC_ERR_CCK_Failed;
	}
	if (pstCiAuthCtx->ucSavedFlag != 2)
	{
		PrintErr(("[CICC]---> CiCc_CalculateCckAndSetDescrambler()---> Error : authentication context not saved !!!\n"));
		return CI_ARC_ERR_CCK_Failed;
	}

	if (pstCiAuthCtx->nNegotiatedCckCipher == CI_CC_KEY_CIPHER_DES_56_ECB) // 이 시점에 nNegotiatedCckCipher 값은 유효할 것이다
	{
		if (bDoCalculateCck)
		{
			// Key precursor calculation : Kp shall be calculated on the CICAM, so we can just use hCc->stCicamData.Kp[32] (이 시점에 Kp 값은 유효할 것이다)

			// Key material computation : compute Km
			#if defined(CI_PLUS_TEST_Technology_from_TrustCenter) || defined(CI_PLUS_TEST_Technology_from_SmarDTV)
			SHA_256(hCc->stCicamData.Kp, 32, Km);
			for (i = 0; i < 16; i++)
				Km[i] ^= s_stCiHostLicenseConst.CLK[i];
			for (i = 0; i < 16; i++)
				Km[16+i] ^= s_stCiHostLicenseConst.CLK[i];
			#else // CI_PLUS_PRODUCTION_Credentials : CI Plus License Specification 에는 AES-128-CBC 라고 되어 있으나 IV 가 없어 사실상 AES-128-ECB 라고 판단된다 -> v1.6r2 기준에서 ECB 맞다.
			E_AES_ECB(s_stCiHostLicenseConst.CLK, hCc->stCicamData.Kp, Km);
			E_AES_ECB(s_stCiHostLicenseConst.CLK, &hCc->stCicamData.Kp[16], &Km[16]);
			for (i = 0; i < 32; i++)
				Km[i] ^= hCc->stCicamData.Kp[i];
			#endif

			// CCK and CIV key derivation : compute CCK, CIV
			CCK[0] = Km[0];
			CCK[1] = (Km[0] << 7) | (Km[1] >> 1);
			CCK[2] = (Km[1] << 6) | (Km[2] >> 2);
			CCK[3] = (Km[2] << 5) | (Km[3] >> 3);
			CCK[4] = (Km[3] << 4) | (Km[4] >> 4);
			CCK[5] = (Km[4] << 3) | (Km[5] >> 5);
			CCK[6] = (Km[5] << 2) | (Km[6] >> 6);
			CCK[7] = Km[6] << 1;
			for (i = 0; i < 8; i++)
			{
				cnt = 0;
				for (j = 1; j < 8; j++)
				{
					if (CCK[i] >> j & 0x1) cnt++;
				}
				if (cnt % 2)
					CCK[i] &= 0xFE;
				else
					CCK[i] |= 0x01;
			}
		}

		if (bDoSetDescrambler)
		{
			if (CI_SetCckToDesc(hCc->usSlotId, CI_CC_KEY_CIPHER_DES_56_ECB, hCc->stCicamData.enKeyRegister, CCK, NULL)) // 이 시점에 enKeyRegister 값은 유효할 것이다
			{
				PrintErr(("[CICC]---> CiCc_CalculateCckAndSetDescrambler()---> Error : Can't set descrambler for DES !!!\n"));
//				디스크램블러 쪽에 문제가 있어서 일시적으로 셋팅을 못한 경우라면 에러 리턴하지 않게 하는게 맞지 않을까... 일단 에러만 찍어주고 에러 처리는 하지 않도록 한다.
//				return CI_ARC_ERR_CCK_Failed;
			}
		}
 	}
	else
	{
		if (bDoCalculateCck)
		{
			// Key precursor calculation : Kp shall be calculated on the CICAM, so we can just use hCc->stCicamData.Kp[32] (이 시점에 Kp 값은 유효할 것이다)

			// Key material computation : compute Km
			#if defined(CI_PLUS_TEST_Technology_from_TrustCenter) || defined(CI_PLUS_TEST_Technology_from_SmarDTV)
			SHA_256(hCc->stCicamData.Kp, 32, Km);
			for (i = 0; i < 16; i++)
				Km[i] ^= s_stCiHostLicenseConst.CLK[i];
			for (i = 0; i < 16; i++)
				Km[16+i] ^= s_stCiHostLicenseConst.CLK[i];
			#else // CI_PLUS_PRODUCTION_Credentials : CI Plus License Specification 에는 AES-128-CBC 라고 되어 있으나 IV 가 없어 사실상 AES-128-ECB 라고 판단된다 -> v1.6r2 기준에서 ECB 맞다.
			E_AES_ECB(s_stCiHostLicenseConst.CLK, hCc->stCicamData.Kp, Km);
			E_AES_ECB(s_stCiHostLicenseConst.CLK, &hCc->stCicamData.Kp[16], &Km[16]);
			for (i = 0; i < 32; i++)
				Km[i] ^= hCc->stCicamData.Kp[i];
			#endif

			// CCK and CIV key derivation : compute CCK, CIV
			VK_memcpy(CCK, Km, 16);
			VK_memcpy(CIV, &Km[16], 16);
		}

		if (bDoSetDescrambler)
		{
			if (CI_SetCckToDesc(hCc->usSlotId, CI_CC_KEY_CIPHER_AES_128_CBC, hCc->stCicamData.enKeyRegister, CCK, CIV)) // 이 시점에 enKeyRegister 값은 유효할 것이다
			{
				PrintErr(("[CICC]---> CiCc_CalculateCckAndSetDescrambler()---> Error : Can't set descrambler for AES !!!\n"));
//				디스크램블러 쪽에 문제가 있어서 일시적으로 셋팅을 못한 경우라면 에러 리턴하지 않게 하는게 맞지 않을까... 일단 에러만 찍어주고 에러 처리는 하지 않도록 한다.
//				return CI_ARC_ERR_CCK_Failed;
			}
		}
	}

	#if defined(USE_SUB_TASK_TO_CALCULATE_KEYS) && defined(_CI_DEBUG_CC_TIME_ISSUE_)
	if (bDoCalculateCck)
		s_bCalculateCckDone = 1;
	#endif

	#ifdef _CI_DEBUG_CC_DATA_
	VK_Print("\n - CiCc_%s_&_%s() - \n", bDoCalculateCck ? "DoCalculateCck" : "DontCalculateCck", bDoSetDescrambler ? "DoSetDescrambler" : "DontSetDescrambler");
	#if 0
	if (bDoCalculateCck)
	{
		if (pstCiAuthCtx->nNegotiatedCckCipher == CI_CC_KEY_CIPHER_DES_56_ECB)
		{
			VK_Print("\t CCK = %02x %02x %02x %02x %02x %02x %02x %02x \n",
				CCK[0], CCK[1], CCK[2], CCK[3], CCK[4], CCK[5], CCK[6], CCK[7]);
		}
		else
		{
			VK_Print("\t CCK = %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x \n",
				CCK[0], CCK[1], CCK[2], CCK[3], CCK[4], CCK[5], CCK[6], CCK[7], CCK[8], CCK[9], CCK[10], CCK[11], CCK[12], CCK[13], CCK[14], CCK[15]);
			VK_Print("\t CIV = %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x \n",
				CIV[0], CIV[1], CIV[2], CIV[3], CIV[4], CIV[5], CIV[6], CIV[7], CIV[8], CIV[9], CIV[10], CIV[11], CIV[12], CIV[13], CIV[14], CIV[15]);
		}
	}
	#endif
	if (bDoSetDescrambler)
	{
		VK_Print("\t nNegotiatedCckCipher = %s \n", pstCiAuthCtx->nNegotiatedCckCipher == CI_CC_KEY_CIPHER_DES_56_ECB ? "DES" : "AES");
		VK_Print("\t enKeyRegister = %s \n", hCc->stCicamData.enKeyRegister == CI_CC_KEY_MODE_EVEN ? "even" : "odd");
	}
	VK_Print("\n");
	#endif

	#ifdef _CI_DEBUG_CC_FUNC_TIME_
	s_ulCcFuncEndTime = VK_TIMER_GetSystemTick();
	VK_Print("[CICC]---> CiCc_CalculateCckAndSetDescrambler() : %ld ms\n", s_ulCcFuncEndTime - s_ulCcFuncStartTime);
	#endif

	return CI_ARC_ERR_NONE;
}

/*******************************/
/*** Local functions for URI ***/
/*******************************/

static void CiCc_GenerateUriConfirm(CI_CC_INSTANCE *hCc, unsigned char *pucUriConfirm)
{
	unsigned char M[8+32];

	#ifdef _CI_DEBUG_CC_FUNC_TIME_
	s_ulCcFuncStartTime = VK_TIMER_GetSystemTick();
	#endif

	VK_memcpy(M, hCc->stCicamData.uri_message, 8); // 이 시점에 uri_message 값은 유효할 것이다.
	VK_memcpy(&M[8], hCc->stHostData.UCK, 32); // 이 시점에 UCK 값은 유효할 것이다.

	SHA_256(M, 8+32, pucUriConfirm);

	#ifdef _CI_DEBUG_CC_FUNC_TIME_
	s_ulCcFuncEndTime = VK_TIMER_GetSystemTick();
	VK_Print("[CICC]---> CiCc_GenerateUriConfirm() : %ld ms\n", s_ulCcFuncEndTime - s_ulCcFuncStartTime);
	#endif
}

static void CiCc_UriDoCopyCtrl(CI_CC_INSTANCE *hCc, unsigned char ucMode)
{
	unsigned long  ulMinutesForRL;

	#if 0 // #ifdef _CI_DEBUG_CC_DATA_ // ci mgr 에서 찍어 주므로 여기서는 막도록 한다.
	if (ucMode == CI_CC_URI_MODE_FROMCAM)
	{
		VK_Print("\n - CiCc_UriDoCopyCtrl() - \n");
		VK_Print("\t stCicamData.usProgramNumber = 0x%x (%d) \n", hCc->stCicamData.usProgramNumber, hCc->stCicamData.usProgramNumber);
		VK_Print("\t stCicamData.uri_message = %02X %02X %02X %02X %02X %02X %02X %02X \n",
			hCc->stCicamData.uri_message[0], hCc->stCicamData.uri_message[1], hCc->stCicamData.uri_message[2], hCc->stCicamData.uri_message[3],
			hCc->stCicamData.uri_message[4], hCc->stCicamData.uri_message[5], hCc->stCicamData.uri_message[6], hCc->stCicamData.uri_message[7]);
		VK_Print("\t APS (%x), EMI (%x), ICT (%x), RCT (%x), RL (%x) \n\n",
			hCc->stCicamData.stUri.ucApsCopyCtrl,
			hCc->stCicamData.stUri.ucEmiCopyCtrl,
			hCc->stCicamData.stUri.ucIctCopyCtrl,
			hCc->stCicamData.stUri.ucRctCopyCtrl,
			hCc->stCicamData.stUri.ucRlCopyCtrl);
	}
	#endif

	// 각 CopyCtrl 에 해당하는 작업을 수행하라 (apply URI to external interface)

	#if 0 // 여기선 에러 확인하는 것 밖에 할 것이 없는데 아래 에러 상황은 근본적으로 발생할 여지가 없으므로 막는다.
	switch (hCc->stCicamData.stUri.ucApsCopyCtrl)
	{
		case CI_CC_URI_APS_OFF:
			break;
		case CI_CC_URI_APS_ON_1:
			break;
		case CI_CC_URI_APS_ON_2:
			break;
		case CI_CC_URI_APS_ON_3:
			break;
		default:
			PrintErr(("[CICC]---> CiCc_UriDoCopyCtrl()---> Error : Unknown ApsCopyCtrl[0x%x] !!!\n", hCc->stCicamData.stUri.ucApsCopyCtrl));
			return;
	}
	#endif

	#if 0 // 여기선 에러 확인하는 것 밖에 할 것이 없는데 아래 에러 상황은 근본적으로 발생할 여지가 없으므로 막는다.
	switch (hCc->stCicamData.stUri.ucEmiCopyCtrl)
	{
		case CI_CC_URI_EMI_COPY_FREE:
			break;
		case CI_CC_URI_EMI_COPY_NO_MORE:
			break;
		case CI_CC_URI_EMI_COPY_ONCE:
			break;
		case CI_CC_URI_EMI_COPY_NEVER:
			break;
		default:
			PrintErr(("[CICC]---> CiCc_UriDoCopyCtrl()---> Error : Unknown EmiCopyCtrl[0x%x] !!!\n", hCc->stCicamData.stUri.ucEmiCopyCtrl));
			return;
	}
	#endif

	#if 0 // 여기선 에러 확인하는 것 밖에 의미가 없으므로 막는다.
	switch (hCc->stCicamData.stUri.ucIctCopyCtrl)
	{
		case CI_CC_URI_ICT_OFF:
			break;
		case CI_CC_URI_ICT_ON:
			break;
		default:
			PrintErr(("[CICC]---> CiCc_UriDoCopyCtrl()---> Error : Unknown IctCopyCtrl[0x%x] !!!\n", hCc->stCicamData.stUri.ucIctCopyCtrl));
			return;
	}
	#endif

	#if 0 // 여기선 에러 확인하는 것 밖에 할 것이 없는데 아래 에러 상황은 근본적으로 발생할 여지가 없으므로 막는다.
	switch (hCc->stCicamData.stUri.ucRctCopyCtrl)
	{
		case CI_CC_URI_RCT_OFF:
			break;
		case CI_CC_URI_RCT_ON:
			break;
		default:
			PrintErr(("[CICC]---> CiCc_UriDoCopyCtrl()---> Error : Unknown ucRctCopyCtrl[0x%x] !!!\n", hCc->stCicamData.stUri.ucRctCopyCtrl));
			return;
	}
	#endif

	switch (hCc->stCicamData.stUri.ucRlCopyCtrl)
	{
		case CI_CC_URI_RL_DEFAULT:								// Default retention limit of 90 minutes applies
			ulMinutesForRL = 90;
			break;
		case 0x01:												// Retention limit of 6 hours applies
			ulMinutesForRL = 6 * 60;
			break;
		case 0x02:												// Retention limit of 12 hours applies
			ulMinutesForRL = 12 * 60;
			break;
		default:
			if (hCc->stCicamData.stUri.ucRlCopyCtrl <= 0x3F)	// Retention limit of 1-61 multiples of 24 Hrs applies as signalled by bits
			{
				ulMinutesForRL = (hCc->stCicamData.stUri.ucRlCopyCtrl - 0x02) * 24 * 60;
				break;
			}
			else
			{
				PrintErr(("[CICC]---> CiCc_UriDoCopyCtrl()---> Error : Unknown ucRlCopyCtrl[0x%x] !!!\n", hCc->stCicamData.stUri.ucRlCopyCtrl));
				return;
			}
	}

	if (CI_ApplyUriToExternalOutput(hCc->usSlotId,
										hCc->stCicamData.usProgramNumber,
										ucMode,
										hCc->stCicamData.stUri.ucApsCopyCtrl,
										hCc->stCicamData.stUri.ucEmiCopyCtrl,
										hCc->stCicamData.stUri.ucIctCopyCtrl,
										hCc->stCicamData.stUri.ucRctCopyCtrl,
										ulMinutesForRL))
	{
		PrintErr(("[CICC]---> CiCc_UriDoCopyCtrl()---> Error : CI_ApplyUriToExternalOutput() !!!\n"));
		return;
	}
}

static void CiCc_UriTimerCallback(unsigned long ulTimerId, void *pvParam)
{
	CI_CC_INSTANCE	* hCc = (CI_CC_INSTANCE *)pvParam;

	PrintNotify(("CiCc_UriTimerCallback !!!\n"));

	if (hCc == NULL)
	{
		PrintErr(("[CICC]---> CiCc_UriTimerCallback()---> Error : hCc is NULL !!!\n"));
		return;
	}

	hCc->ulUriTimerID = 0;

	hCc->stCicamData.stUri.ucApsCopyCtrl = CI_CC_URI_APS_DEFAULT;
	hCc->stCicamData.stUri.ucEmiCopyCtrl = CI_CC_URI_EMI_DEFAULT;
	hCc->stCicamData.stUri.ucIctCopyCtrl = CI_CC_URI_ICT_TIMEOUT;
	hCc->stCicamData.stUri.ucRctCopyCtrl = CI_CC_URI_RCT_DEFAULT;
	hCc->stCicamData.stUri.ucRlCopyCtrl = CI_CC_URI_RL_DEFAULT;

	CiCc_UriDoCopyCtrl(hCc, CI_CC_URI_MODE_TIMEOUT);
}

/**********************************************/
/*** Local functions for ARC Error Handling ***/
/**********************************************/

static void CiCc_DoActionForArcErr(CI_CC_INSTANCE *hCc, CI_ARC_ERR enArcErr)
{
	CI_ARC_ACTION enArcAction;

	hCc->enArcErr = enArcErr;

	switch (enArcErr)
	{
		case CI_ARC_ERR_Not_Defined :					enArcAction = CI_ARC_ACTION_STOP_CICAM; s_stCcReportInfo.eCcReport = eCiPlus_CcReport_OperationFailed; break;
//		case CI_ARC_ERR_NONE :							enArcAction = CI_ACR_ACTION_NONE; return;
//		case CI_ARC_ERR_Module_Revoked :				enArcAction = CI_ACR_ACTION_NONE; return;
//		case CI_ARC_ERR_Host_Revoked :					enArcAction = CI_ACR_ACTION_NONE; return;
		case CI_ARC_ERR_SAC_Failed :					enArcAction = CI_ARC_ACTION_STOP_CICAM; s_stCcReportInfo.eCcReport = eCiPlus_CcReport_OperationFailed; break;
		case CI_ARC_ERR_CCK_Failed :					enArcAction = CI_ARC_ACTION_STOP_CICAM; s_stCcReportInfo.eCcReport = eCiPlus_CcReport_OperationFailed; break;
//		case CI_ARC_ERR_CAM_Upgrade_Failed_Bootloader :	enArcAction = CI_ACR_ACTION_NONE; return;
//		case CI_ARC_ERR_CAM_Upgrade_Failed_Location :	enArcAction = CI_ACR_ACTION_NONE; return;
//		case CI_ARC_ERR_CAM_Upgrade_Failed_Signature :	enArcAction = CI_ACR_ACTION_NONE; return;
//		case CI_ARC_ERR_Auth_Failed_Retries_Exhausted :	enArcAction = CI_ACR_ACTION_NONE; return;
		case CI_ARC_ERR_Auth_Failed_Invalid_Signature :	enArcAction = CI_ARC_ACTION_STOP_CICAM; s_stCcReportInfo.eCcReport = eCiPlus_CcReport_AuthenticationFailed; break;
		case CI_ARC_ERR_Auth_Failed_Invalid_Auth_Key :	enArcAction = CI_ARC_ACTION_STOP_CICAM; s_stCcReportInfo.eCcReport = eCiPlus_CcReport_AuthenticationFailed; break;
		case CI_ARC_ERR_Auth_Failed_Key_Computation :	enArcAction = CI_ARC_ACTION_STOP_CICAM; s_stCcReportInfo.eCcReport = eCiPlus_CcReport_AuthenticationFailed; break;
		case CI_ARC_ERR_Auth_Failed_DH :				enArcAction = CI_ARC_ACTION_STOP_CICAM; s_stCcReportInfo.eCcReport = eCiPlus_CcReport_AuthenticationFailed; break;
		case CI_ARC_ERR_CAM_Cert_Syntax_Incorrect :		enArcAction = CI_ARC_ACTION_STOP_CICAM; s_stCcReportInfo.eCcReport = eCiPlus_CcReport_AuthenticationFailed; break;
		case CI_ARC_ERR_CAM_Cert_Expired :				enArcAction = CI_ARC_ACTION_GOTO_NORMAL_MODE; s_stCcReportInfo.eCcReport = eCiPlus_CcReport_AuthenticationFailed; break;
		case CI_ARC_ERR_CAM_Cert_Invalid_Signature :	enArcAction = CI_ARC_ACTION_STOP_CICAM; s_stCcReportInfo.eCcReport = eCiPlus_CcReport_AuthenticationFailed; break;
//		case CI_ARC_ERR_Host_Cert_Syntax_Incorrect :	enArcAction = CI_ACR_ACTION_NONE; return;
//		case CI_ARC_ERR_Host_Cert_Expired :				enArcAction = CI_ACR_ACTION_NONE; return;
//		case CI_ARC_ERR_Host_Cert_Invalid_Signature :	enArcAction = CI_ACR_ACTION_NONE; return;
//		case CI_ARC_ERR_OP_Cert_Syntax_Incorrect :		enArcAction = CI_ACR_ACTION_NONE; return;
//		case CI_ARC_ERR_OP_Cert_Expired :				enArcAction = CI_ACR_ACTION_NONE; return;
//		case CI_ARC_ERR_OP_Cert_Invalid_Signature :		enArcAction = CI_ACR_ACTION_NONE; return;
//		case CI_ARC_ERR_CAM_Requires_Update :			enArcAction = CI_ACR_ACTION_NONE; return;
//		case CI_ARC_ERR_Reserved_for_CI_Plus :			enArcAction = CI_ACR_ACTION_NONE; return;
//		case CI_ARC_ERR_Private_Use_for_OP :			enArcAction = CI_ACR_ACTION_NONE; return;
		default :
			PrintErr(("[CICC]---> CiCc_DoActionForArcErr()---> Error : Wrong coding case 1 !!!\n")); // 이런 상황이 발생하면 코딩 에러다!!
			return;
	}

	// UI 처리를 위한 콜백
	s_stCcReportInfo.enArcErr = enArcErr;
	Callback_Reset(g_hCbNotifyCcReport);
	while ((s_pfnNotifyCcReport = (CI_NOTIFY_CC_REPORT_FUNC)Callback_GetNextFunc(g_hCbNotifyCcReport)) != NULL)
	{
		(*s_pfnNotifyCcReport)(hCc->usSlotId, &s_stCcReportInfo);
	}

	// 액션 : stop cam or goto normal mode...
	switch (enArcAction)
	{
		case CI_ARC_ACTION_STOP_CICAM :
			if (PAL_CI_SetTsPath(hCc->usSlotId, PAL_CI_BYPASS) != ERR_OK)
				PrintErr(("[CICC]---> CiCc_DoActionForArcErr()---> Error : PAL_CI_SetTsPath() failed !!!\n"));
			break;

		case CI_ARC_ACTION_GOTO_NORMAL_MODE :
			/* <TODO_CI_PLUS> : 이 경우에 대한 CI+ 스펙의 내용은 모호하나...
								일단 Host Service Shunning 에서 일반 캠으로 간주하고 또한 CCK 및 URI 동작하지 않도록 하자 (CC 리소스를 아예 막아?) ...
			                    Host Service Shunning 에서의 처리에 대해선 CI_SetPlusMode(), CI_GetPlusMode() 참고할 것.
			                    -> 현재로선 딱히 처리할 필요가 없을 듯 하니 일단 내버려둔다... */
			break;

		default :
			PrintErr(("[CICC]---> CiCc_DoActionForArcErr()---> Error : Wrong coding case 2 !!!\n")); // 이런 상황이 발생하면 코딩 에러다!!
			break;
	}

	return;
}

/***************************************************/
/*** Local functions for authentication contexts ***/
/***************************************************/

static CI_AUTH_CTX * CiCc_GetAuthContext (CI_SLOT_ID usSlotId, int index)
{
	if ((usSlotId >= PAL_CI_GetNumberOfModule()) || (index >= CI_NUM_OF_AUTH_CTX))
	{
		PrintErr(("[CICC]---> CiCc_GetAuthContext()---> Error : Invalid params !!!\n"));
		return NULL;
	}

	return &s_stCiAuthCtxs[usSlotId].ctx[index];
}

static int CiCc_PutAuthContext (CI_SLOT_ID usSlotId, int index, CI_AUTH_CTX *pstCiAuthCtx)
{
	if ((usSlotId >= PAL_CI_GetNumberOfModule()) || (index >= CI_NUM_OF_AUTH_CTX) || (pstCiAuthCtx == NULL))
	{
		PrintErr(("[CICC]---> CiCc_PutAuthContext()---> Error : Invalid params !!!\n"));
		return -1;
	}

	VK_memcpy(&s_stCiAuthCtxs[usSlotId].ctx[index], pstCiAuthCtx, sizeof(CI_AUTH_CTX));

	return ERR_OK;
}

/***************************************/
/*** Local functions for CC instance ***/
/***************************************/

static int CiCc_AddInst(unsigned short usSsNo)
{
	CI_CC_INSTANCE	* hCc;
	CI_SS_HANDLE	hSs;

	hSs = CIRH_GetSs(usSsNo);
	if (hSs == NULL)
	{
		PrintErr(("[CICC]---> CiCc_AddInst()---> Error : SsNo[%d] - NULL hSs !!!\n", usSsNo));
		return -1;
	}
	if (hSs->hTc == NULL)
	{
		PrintErr(("[CICC]---> CiCc_AddInst()---> Error : SsNo[%d] - NULL hTc !!!\n", usSsNo));
		return -1;
	}
	if (hSs->hTc->ucTcId == 0)
	{
		PrintErr(("[CICC]---> CiCc_AddInst()---> Error : SsNo[%d] - Invalid TcId !!!\n", usSsNo));
		return -1;
	}
	if (hSs->hTc->usSlotId >= PAL_CI_GetNumberOfModule())
	{
		PrintErr(("[CICC]---> CiCc_AddInst()---> Error : SsNo[%d] - Invalid SlotId !!!\n", usSsNo));
		return -1;
	}

	if (s_ucCiCcCnt >= MAX_CI_CC_SS)
	{
		PrintErr(("[CICC]---> CiCc_AddInst()---> Error : session is full !!!\n"));
		return -1;
	}

	hCc = CiCc_GetNewInst();
	if (hCc == NULL)
	{
		PrintErr(("[CICC]---> CiCc_AddInst()---> Error : instance is full !!!\n"));
		return -1;
	}

	hCc->usSsNo = usSsNo;
	hCc->enPhase = CI_CC_PHASE_SS_Opened;
	hCc->usSlotId = hSs->hTc->usSlotId;

	s_ucCiCcCnt++;

	return ERR_OK;
}

static int CiCc_DeleteInst(unsigned short usSsNo)
{
	CI_CC_INSTANCE	* hCc;

	hCc = CiCc_GetInst(usSsNo);
	if (hCc == NULL)
	{
		PrintErr(("[CICC]---> CiCc_DeleteInst()---> Error : SsNo[%d] incorrect !!!\n", usSsNo));
		return -1;
	}

	if (hCc->ulUriTimerID)
	{
		VK_TIMER_Cancel(hCc->ulUriTimerID);
	}

	VK_memset(hCc, 0, sizeof(CI_CC_INSTANCE));

	s_ucCiCcCnt--;

	return ERR_OK;
}

static CI_CC_INSTANCE * CiCc_GetNewInst(void)
{
	int	i;

	for (i = 0; i < MAX_CI_CC_SS; i++)
	{
		if (s_aCiCc[i].usSsNo == 0)
			return &s_aCiCc[i];
	}

	return NULL;
}

static CI_CC_INSTANCE * CiCc_GetInst(unsigned short usSsNo)
{
	int i;

	for (i = 0; i < MAX_CI_CC_SS; i++)
	{
		if(s_aCiCc[i].usSsNo == usSsNo)
		{
			PrintDbg(("[CICC]---> CiCc_GetInst()---> %d th instance found\n", i));
			return &s_aCiCc[i];
		}
	}

	return NULL;
}

/******************************************/
/*** Public functions for other modules ***/
/******************************************/

int CiCc_GetHostId(unsigned char *pucHostId)
	// 이 함수의 호출은 CiCc_Task() 기동이 완료된 후에야 유효한 결과를 줌에 유의할 것
{
	if (pucHostId == NULL)
	{
		PrintErr(("[CICC]---> CiCc_GetHostId()---> Error : Invalid params !!!\n"));
		return -1;
	}

	if (!s_bCiCcInitSuccess)
	{
		PrintErr(("[CICC]---> CiCc_GetHostId()---> Error : No credentials in this set !!!\n"));
		return -1;
	}

	VK_memcpy(pucHostId, s_stCiHostLicenseConst.Host_ID, 8);
	return ERR_OK;
}

int CiCc_GetBindedCamId(CI_SLOT_ID usSlotId, unsigned char *pucBindedCamId)
	// 이 함수의 호출은 캠 삽입후 일정 시간 경과된 후에야 유효한 결과를 줌에 유의할 것
{
	CI_CC_INSTANCE	* hCc = NULL;
	CI_AUTH_CTX * pstCiAuthCtx = NULL;
	int i;

	if (pucBindedCamId == NULL)
	{
		PrintErr(("[CICC]---> CiCc_GetBindedCamId()---> Error : Invalid params !!!\n"));
		return -1;
	}

	if (!s_bCiCcInitSuccess)
	{
		PrintErr(("[CICC]---> CiCc_GetBindedCamId()---> Error : No credentials in this set !!!\n"));
		return -1;
	}

	for (i = 0; i < MAX_CI_CC_SS; i++)
	{
		if(s_aCiCc[i].usSsNo)
		{
			if (s_aCiCc[i].usSlotId == usSlotId)
			{
				hCc = &s_aCiCc[i];

				if (hCc->enPhase < CI_CC_PHASE_SAC_Establishment_Requested)
				{
					PrintErr(("[CICC]---> CiCc_GetBindedCamId()---> Error : enPhase %d !!!\n", hCc->enPhase));
					return -1; // 아직 프로세스가 끝나지 않아 유효한 결과를 만들 수 없다 (캠 삽입 직후라면 정상).
				}

				if (hCc->enArcErr != CI_ARC_ERR_NONE)
				{
					PrintErr(("[CICC]---> CiCc_GetBindedCamId()---> Error : enArcErr %d !!!\n", hCc->enArcErr));
					return -1; // 프로세스 상에서 에러 발생하여 유효한 결과를 만들 수 없다 (캠 쪽 문제라면 정상).
				}

				if ((pstCiAuthCtx = CiCc_GetAuthContext(hCc->usSlotId, hCc->nCurAuthCtxIdx)) == NULL)
				{
					PrintErr(("[CICC]---> CiCc_GetBindedCamId()---> Error : Can't load context !!!\n")); // 이런 상황이 발생하면 곤란. 디버깅 필요!!
					return -1;
				}

				if (pstCiAuthCtx->ucSavedFlag != 2)
				{
					PrintErr(("[CICC]---> CiCc_GetBindedCamId()---> Error : ucSavedFlag %d !!!\n", pstCiAuthCtx->ucSavedFlag));
					return -1; // 저장된 것이 없다 (즉, 바인딩이 안되어 있다?).
				}

				VK_memcpy(pucBindedCamId, pstCiAuthCtx->ucBindedCamId, 8);
				return ERR_OK;
			}
		}
	}

	return -1; // 캠이 꽂혀 있지 않다.
}

int CiCc_GetBindedCamBrandId(CI_SLOT_ID usSlotId, unsigned short *pusBindedCamBrandId)
	// 이 함수의 호출은 캠 삽입후 일정 시간 경과된 후에야 유효한 결과를 줌에 유의할 것
{
	CI_CC_INSTANCE	* hCc = NULL;
	CI_AUTH_CTX * pstCiAuthCtx = NULL;
	int i;

	if (pusBindedCamBrandId == NULL)
	{
		PrintErr(("[CICC]---> CiCc_GetBindedCamBrandId()---> Error : Invalid params !!!\n"));
		return -1;
	}

	if (!s_bCiCcInitSuccess)
	{
		PrintErr(("[CICC]---> CiCc_GetBindedCamBrandId()---> Error : No credentials in this set !!!\n"));
		return -1;
	}

	for (i = 0; i < MAX_CI_CC_SS; i++)
	{
		if(s_aCiCc[i].usSsNo)
		{
			if (s_aCiCc[i].usSlotId == usSlotId)
			{
				hCc = &s_aCiCc[i];

				if (hCc->enPhase < CI_CC_PHASE_SAC_Establishment_Requested)
				{
					PrintErr(("[CICC]---> CiCc_GetBindedCamBrandId()---> Error : enPhase %d !!!\n", hCc->enPhase));
					return -1; // 아직 프로세스가 끝나지 않아 유효한 결과를 만들 수 없다 (캠 삽입 직후라면 정상).
				}

				if (hCc->enArcErr != CI_ARC_ERR_NONE)
				{
					PrintErr(("[CICC]---> CiCc_GetBindedCamBrandId()---> Error : enArcErr %d !!!\n", hCc->enArcErr));
					return -1; // 프로세스 상에서 에러 발생하여 유효한 결과를 만들 수 없다 (캠 쪽 문제라면 정상).
				}

				if ((pstCiAuthCtx = CiCc_GetAuthContext(hCc->usSlotId, hCc->nCurAuthCtxIdx)) == NULL)
				{
					PrintErr(("[CICC]---> CiCc_GetBindedCamBrandId()---> Error : Can't load context !!!\n")); // 이런 상황이 발생하면 곤란. 디버깅 필요!!
					return -1;
				}

				if (pstCiAuthCtx->ucSavedFlag != 2)
				{
					PrintErr(("[CICC]---> CiCc_GetBindedCamBrandId()---> Error : ucSavedFlag %d !!!\n", pstCiAuthCtx->ucSavedFlag));
					return -1; // 저장된 것이 없다 (즉, 바인딩이 안되어 있다?).
				}

				*pusBindedCamBrandId = pstCiAuthCtx->usBindedCamBrandId; // SMiT : 14399 (0x383F), Neotion : 23308 (0x5B0C)
				return ERR_OK;
			}
		}
	}

	return -1; // 캠이 꽂혀 있지 않다.
}

int CiCc_GetNegotiatedCckCipher(CI_SLOT_ID usSlotId, CI_CC_KEY_CIPHER *penNegotiatedCckCipher)
	// 이 함수의 호출은 캠 삽입후 일정 시간 경과된 후에야 유효한 결과를 줌에 유의할 것
{
	CI_CC_INSTANCE	* hCc = NULL;
	CI_AUTH_CTX * pstCiAuthCtx = NULL;
	int i;

	if (penNegotiatedCckCipher == NULL)
	{
		PrintErr(("[CICC]---> CiCc_GetNegotiatedCckCipher()---> Error : Invalid params !!!\n"));
		return -1;
	}

	if (!s_bCiCcInitSuccess)
	{
		PrintErr(("[CICC]---> CiCc_GetNegotiatedCckCipher()---> Error : No credentials in this set !!!\n"));
		return -1;
	}

	for (i = 0; i < MAX_CI_CC_SS; i++)
	{
		if(s_aCiCc[i].usSsNo)
		{
			if (s_aCiCc[i].usSlotId == usSlotId)
			{
				hCc = &s_aCiCc[i];

				if (hCc->enPhase < CI_CC_PHASE_SAC_Establishment_Requested)
				{
					PrintErr(("[CICC]---> CiCc_GetNegotiatedCckCipher()---> Error : enPhase %d !!!\n", hCc->enPhase));
					return -1; // 아직 프로세스가 끝나지 않아 유효한 결과를 만들 수 없다 (캠 삽입 직후라면 정상).
				}

				if (hCc->enArcErr != CI_ARC_ERR_NONE)
				{
					PrintErr(("[CICC]---> CiCc_GetNegotiatedCckCipher()---> Error : enArcErr %d !!!\n", hCc->enArcErr));
					return -1; // 프로세스 상에서 에러 발생하여 유효한 결과를 만들 수 없다 (캠 쪽 문제라면 정상).
				}

				if ((pstCiAuthCtx = CiCc_GetAuthContext(hCc->usSlotId, hCc->nCurAuthCtxIdx)) == NULL)
				{
					PrintErr(("[CICC]---> CiCc_GetNegotiatedCckCipher()---> Error : Can't load context !!!\n")); // 이런 상황이 발생하면 곤란. 디버깅 필요!!
					return -1;
				}

				if (pstCiAuthCtx->ucSavedFlag != 2)
				{
					PrintErr(("[CICC]---> CiCc_GetNegotiatedCckCipher()---> Error : ucSavedFlag %d !!!\n", pstCiAuthCtx->ucSavedFlag));
					return -1; // 저장된 것이 없다 (즉, 바인딩이 안되어 있다?).
				}

				*penNegotiatedCckCipher = pstCiAuthCtx->nNegotiatedCckCipher;

				return ERR_OK;
			}
		}
	}

	return -1; // 캠이 꽂혀 있지 않다.
}

int CiCc_SetNegotiatedCckCipher(CI_SLOT_ID usSlotId, CI_CC_KEY_CIPHER enNegotiatedCckCipher)
	// 테스트 용도의 함수임. 다른 용도로 사용하지 말 것!
{
#ifdef CONFIG_DEBUG

	CI_CC_INSTANCE	* hCc = NULL;
	CI_AUTH_CTX * pstCiAuthCtx = NULL;
	int i;

	if (!s_bCiCcInitSuccess)
	{
		PrintErr(("[CICC]---> CiCc_SetNegotiatedCckCipher()---> Error : No credentials in this set !!!\n"));
		return -1;
	}

	for (i = 0; i < MAX_CI_CC_SS; i++)
	{
		if(s_aCiCc[i].usSsNo)
		{
			if (s_aCiCc[i].usSlotId == usSlotId)
			{
				hCc = &s_aCiCc[i];

				if (hCc->enPhase < CI_CC_PHASE_SAC_Establishment_Requested)
				{
					PrintErr(("[CICC]---> CiCc_SetNegotiatedCckCipher()---> Error : enPhase %d !!!\n", hCc->enPhase));
					return -1; // 아직 프로세스가 끝나지 않아 유효한 결과를 만들 수 없다 (캠 삽입 직후라면 정상).
				}

				if (hCc->enArcErr != CI_ARC_ERR_NONE)
				{
					PrintErr(("[CICC]---> CiCc_SetNegotiatedCckCipher()---> Error : enArcErr %d !!!\n", hCc->enArcErr));
					return -1; // 프로세스 상에서 에러 발생하여 유효한 결과를 만들 수 없다 (캠 쪽 문제라면 정상).
				}

				if ((pstCiAuthCtx = CiCc_GetAuthContext(hCc->usSlotId, hCc->nCurAuthCtxIdx)) == NULL)
				{
					PrintErr(("[CICC]---> CiCc_SetNegotiatedCckCipher()---> Error : Can't load context !!!\n")); // 이런 상황이 발생하면 곤란. 디버깅 필요!!
					return -1;
				}

				if (pstCiAuthCtx->ucSavedFlag != 2)
				{
					PrintErr(("[CICC]---> CiCc_SetNegotiatedCckCipher()---> Error : ucSavedFlag %d !!!\n", pstCiAuthCtx->ucSavedFlag));
					return -1; // 저장된 것이 없다 (즉, 바인딩이 안되어 있다?).
				}

				pstCiAuthCtx->nNegotiatedCckCipher = enNegotiatedCckCipher;

//				테스트 용도라 저장할 필요가 없지...
//				if (CI_SaveAuthContexts(hCc->usSlotId, &s_stCiAuthCtxs[hCc->usSlotId]))
//				{
//					PrintErr(("[CICC]---> CiCc_SetNegotiatedCckCipher()---> Error : Can't save context !!!\n"));
//					return -1;
//				}

				return ERR_OK;
			}
		}
	}

#endif

	return -1; // 캠이 꽂혀 있지 않다.
}

void CiCc_ReqSetCckToDesc(CI_SLOT_ID usSlotId)
	// 이 함수의 호출은 캠 삽입후 일정 시간 경과된 후에야 유효한 결과를 줌에 유의할 것 -> 더이상 사용되지 않으나 추후 필요한 용도가 있을 수도 있어 일단 남겨두도록 한다.
{
	CI_CC_INSTANCE	* hCc = NULL;
	CI_AUTH_CTX * pstCiAuthCtx = NULL;
	int i;

	if (!s_bCiCcInitSuccess)
	{
		PrintErr(("[CICC]---> CiCc_ReqSetCckToDesc()---> Error : No credentials in this set !!!\n"));
		return;
	}

	for (i = 0; i < MAX_CI_CC_SS; i++)
	{
		if(s_aCiCc[i].usSsNo)
		{
			if (s_aCiCc[i].usSlotId == usSlotId)
			{
				hCc = &s_aCiCc[i];

				if (hCc->enPhase < CI_CC_PHASE_SAC_Establishment_Requested)
				{
					PrintErr(("[CICC]---> CiCc_ReqSetCckToDesc()---> Error : enPhase %d !!!\n", hCc->enPhase));
					return; // 아직 프로세스가 끝나지 않아 유효한 결과를 만들 수 없다 (캠 삽입 직후라면 정상).
				}

				if (hCc->enArcErr != CI_ARC_ERR_NONE)
				{
					PrintErr(("[CICC]---> CiCc_ReqSetCckToDesc()---> Error : enArcErr %d !!!\n", hCc->enArcErr));
					return; // 프로세스 상에서 에러 발생하여 유효한 결과를 만들 수 없다 (캠 쪽 문제라면 정상).
				}

				if ((pstCiAuthCtx = CiCc_GetAuthContext(hCc->usSlotId, hCc->nCurAuthCtxIdx)) == NULL)
				{
					PrintErr(("[CICC]---> CiCc_ReqSetCckToDesc()---> Error : Can't load context !!!\n")); // 이런 상황이 발생하면 곤란. 디버깅 필요!!
					return;
				}

				if (pstCiAuthCtx->ucSavedFlag != 2)
				{
					PrintErr(("[CICC]---> CiCc_ReqSetCckToDesc()---> Error : ucSavedFlag %d !!!\n", pstCiAuthCtx->ucSavedFlag));
					return; // 저장된 것이 없다 (즉, 바인딩이 안되어 있다?).
				}

				// set descrambler (이 시점에 nNegotiatedCckCipher, enKeyRegister 값은 유효할 것이다)
				if (pstCiAuthCtx->nNegotiatedCckCipher == CI_CC_KEY_CIPHER_DES_56_ECB)
				{
					if (CI_SetCckToDesc(hCc->usSlotId, CI_CC_KEY_CIPHER_DES_56_ECB, hCc->stCicamData.enKeyRegister, hCc->stHostData.CCK, NULL))
					{
						PrintErr(("[CICC]---> CiCc_ReqSetCckToDesc()---> Error : Can't set descrambler for DES !!!\n"));
						return;
					}
				}
				else
				{
					if (CI_SetCckToDesc(hCc->usSlotId, CI_CC_KEY_CIPHER_AES_128_CBC, hCc->stCicamData.enKeyRegister, hCc->stHostData.CCK, hCc->stHostData.CIV))
					{
						PrintErr(("[CICC]---> CiCc_ReqSetCckToDesc()---> Error : Can't set descrambler for AES !!!\n"));
						return;
					}
				}
			}
		}
	}
}

void CiCc_SetUriDefault(CI_SLOT_ID usSlotId, HBOOL bOn)
	// 이 함수의 호출은 캠 삽입후 일정 시간 경과된 후에야 유효한 결과를 줌에 유의할 것
{
	CI_CC_INSTANCE	* hCc = NULL;
	int i;

	if (!s_bCiCcInitSuccess)
	{
		PrintErr(("[CICC]---> CiCc_SetUriDefault()---> Error : No credentials in this set !!!\n"));
		return;
	}

	for (i = 0; i < MAX_CI_CC_SS; i++)
	{
		if(s_aCiCc[i].usSsNo)
		{
			if (s_aCiCc[i].usSlotId == usSlotId)
			{
				hCc = &s_aCiCc[i];

				if (hCc->enPhase < CI_CC_PHASE_SAC_Establishment_Requested)
				{
					PrintErr(("[CICC]---> CiCc_SetUriDefault()---> Error : enPhase %d !!!\n", hCc->enPhase));
					return; // 아직 프로세스가 끝나지 않아 유효한 결과를 만들 수 없다 (캠 삽입 직후라면 정상).
				}

				if (hCc->enArcErr != CI_ARC_ERR_NONE)
				{
					PrintErr(("[CICC]---> CiCc_SetUriDefault()---> Error : enArcErr %d !!!\n", hCc->enArcErr));
					return; // 프로세스 상에서 에러 발생하여 유효한 결과를 만들 수 없다 (캠 쪽 문제라면 정상).
				}

				// if URI timer is running, cancel it
				if (hCc->ulUriTimerID)
				{
					VK_TIMER_Cancel(hCc->ulUriTimerID);
					hCc->ulUriTimerID = 0;
				}

				if (bOn == TRUE)
				{
					// set URI default values and do control external interface with them
					hCc->stCicamData.stUri.ucApsCopyCtrl = CI_CC_URI_APS_DEFAULT;
					hCc->stCicamData.stUri.ucEmiCopyCtrl = CI_CC_URI_EMI_DEFAULT;
					hCc->stCicamData.stUri.ucIctCopyCtrl = CI_CC_URI_ICT_DEFAULT;
					hCc->stCicamData.stUri.ucRctCopyCtrl = CI_CC_URI_RCT_DEFAULT;
					hCc->stCicamData.stUri.ucRlCopyCtrl = CI_CC_URI_RL_DEFAULT;
					CiCc_UriDoCopyCtrl(hCc, CI_CC_URI_MODE_DEFAULT);

					// set URI timer
					if (hCc->ulUriTimerID == 0)
					{
						VK_TIMER_EventAfter(CI_CC_URI_TIMEOUT, CiCc_UriTimerCallback, (void *)hCc, sizeof(CI_CC_INSTANCE), (unsigned long *)&hCc->ulUriTimerID);
					}
				}

				return;
			}
		}
	}

	/* nothing to do in this case */

}

int CiCc_IsInitialized(void) /* CI_CC_PHASE_Init */
	// 이 함수의 호출은 CiCc_Task() 기동이 완료된 후에야 유효한 결과를 줌에 유의할 것
{
	return (int)s_bCiCcInitSuccess;
}

int CiCc_IsOpened(CI_SLOT_ID usSlotId) /* CI_CC_PHASE_SS_Opened */
	// 이 함수의 호출은 캠 삽입후 일정 시간 경과된 후에야 유효한 결과를 줌에 유의할 것
{
	int i;

	for (i = 0; i < MAX_CI_CC_SS; i++)
	{
		if(s_aCiCc[i].usSsNo)
		{
			if (s_aCiCc[i].usSlotId == usSlotId)
			{
				return 1;
			}
		}
	}

	return 0;
}

int CiCc_IsBusy(CI_SLOT_ID usSlotId)
	// 이 함수의 호출은 캠 삽입후 일정 시간 경과된 후에야 유효한 결과를 줌에 유의할 것
{
	int i;

	for (i = 0; i < MAX_CI_CC_SS; i++)
	{
		if(s_aCiCc[i].usSsNo)
		{
			if (s_aCiCc[i].usSlotId == usSlotId)
			{
//				VK_Print("CiCc_IsBusy : %d\n", s_aCiCc[i].enPhase);
				if (s_aCiCc[i].enArcErr == CI_ARC_ERR_NONE && s_aCiCc[i].enPhase < CI_CC_PHASE_CCK_Calculating_Replied)
					return 1;
				else
					return 0;
			}
		}
	}

	return 0;
}

void CiCc_PrintDebugInfo(CI_SLOT_ID usSlotIdToCheck, int flagPrintMoreDetail)
{
#ifdef CONFIG_DEBUG

//	#define Print_CI_Plus_Credentials /* 제품 릴리즈 시에 절대 define 되어서는 안된다! */

	CI_SLOT_ID			usSlotId;
	CI_AUTH_CTX			* pstCiAuthCtx = NULL;
	CI_CC_INSTANCE		* hCc = NULL;
	CI_CC_KEY_CIPHER	enNegotiatedCckCipher = CI_CC_KEY_CIPHER_DES_56_ECB;
	int					i, j;

	/* print host license constants */

	if (s_stCiHostLicenseConst.ucSavedFlag != 1 && s_stCiHostLicenseConst.ucSavedFlag != 2)
	{
		VK_Print("\n There is no host license constants\n\n");
		return;
	}

	VK_Print("\n --- Host license constants (%d) ---\n", s_stCiHostLicenseConst.ucSavedFlag);

	#ifdef Print_CI_Plus_Credentials
	VK_Print("\n RootCert              :");
	for (i = 0; i < s_stCiHostLicenseConst.RootCertLen; i++)
	{
		if ((i & 15) == 0)
			VK_Print("\n");
		VK_Print(" %02x", s_stCiHostLicenseConst.RootCert[i]);
	}
	VK_Print("\n Host_BrandCert        :");
	for (i = 0; i < s_stCiHostLicenseConst.Host_BrandCertLen; i++)
	{
		if ((i & 15) == 0)
			VK_Print("\n");
		VK_Print(" %02x", s_stCiHostLicenseConst.Host_BrandCert[i]);
	}
	VK_Print("\n Host_DevCert          :");
	for (i = 0; i < s_stCiHostLicenseConst.Host_DevCertLen; i++)
	{
		if ((i & 15) == 0)
			VK_Print("\n");
		VK_Print(" %02x", s_stCiHostLicenseConst.Host_DevCert[i]);
	}
	#endif
	VK_Print("\n Host_ID               :");
	for (i = 0; i < 8; i++)
	{
		VK_Print(" %02X", s_stCiHostLicenseConst.Host_ID[i]);
	}
	VK_Print("\n Host_CckCipher        : %s", s_stCiHostLicenseConst.Host_CckCipher == CI_CC_KEY_CIPHER_DES_56_ECB ? "DES" : "AES");
	#ifdef Print_CI_Plus_Credentials
	VK_Print("\n DH_p                  :");
	for (i = 0; i < CI_DH_P_LEN; i++)
	{
		if ((i & 15) == 0)
			VK_Print("\n");
		VK_Print(" %02x", s_stCiHostLicenseConst.DH_p[i]);
	}
	VK_Print("\n DH_g                  :");
	for (i = 0; i < CI_DH_G_LEN; i++)
	{
		if ((i & 15) == 0)
			VK_Print("\n");
		VK_Print(" %02x", s_stCiHostLicenseConst.DH_g[i]);
	}
	VK_Print("\n DH_q                  :");
	for (i = 0; i < CI_DH_Q_LEN; i++)
	{
		if ((i & 15) == 0)
			VK_Print("\n");
		VK_Print(" %02x", s_stCiHostLicenseConst.DH_q[i]);
	}
	VK_Print("\n HDQ_n                 :");
	for (i = 0; i < CI_HDQ_N_LEN; i++)
	{
		if ((i & 15) == 0)
			VK_Print("\n");
		VK_Print(" %02x", s_stCiHostLicenseConst.HDQ_n[i]);
	}
	VK_Print("\n HDQ_d                 :");
	for (i = 0; i < CI_HDQ_D_LEN; i++)
	{
		if ((i & 15) == 0)
			VK_Print("\n");
		VK_Print(" %02x", s_stCiHostLicenseConst.HDQ_d[i]);
	}
	VK_Print("\n HDQ_p                 :");
	for (i = 0; i < CI_HDQ_P_LEN; i++)
	{
		if ((i & 15) == 0)
			VK_Print("\n");
		VK_Print(" %02x", s_stCiHostLicenseConst.HDQ_p[i]);
	}
	VK_Print("\n HDQ_q                 :");
	for (i = 0; i < CI_HDQ_Q_LEN; i++)
	{
		if ((i & 15) == 0)
			VK_Print("\n");
		VK_Print(" %02x", s_stCiHostLicenseConst.HDQ_q[i]);
	}
	VK_Print("\n HDQ_dP                :");
	for (i = 0; i < CI_HDQ_DP_LEN; i++)
	{
		if ((i & 15) == 0)
			VK_Print("\n");
		VK_Print(" %02x", s_stCiHostLicenseConst.HDQ_dP[i]);
	}
	VK_Print("\n HDQ_dQ                :");
	for (i = 0; i < CI_HDQ_DQ_LEN; i++)
	{
		if ((i & 15) == 0)
			VK_Print("\n");
		VK_Print(" %02x", s_stCiHostLicenseConst.HDQ_dQ[i]);
	}
	VK_Print("\n HDQ_qInv              :");
	for (i = 0; i < CI_HDQ_QINV_LEN; i++)
	{
		if ((i & 15) == 0)
			VK_Print("\n");
		VK_Print(" %02x", s_stCiHostLicenseConst.HDQ_qInv[i]);
	}
	VK_Print("\n PRNG_seed             :");
	for (i = 0; i < 16; i++)
	{
		VK_Print(" %02x", s_stCiHostLicenseConst.PRNG_seed[i]);
	}
	VK_Print("\n SLK                   :");
	for (i = 0; i < 16; i++)
	{
		VK_Print(" %02x", s_stCiHostLicenseConst.SLK[i]);
	}
	VK_Print("\n SIV                   :");
	for (i = 0; i < 16; i++)
	{
		VK_Print(" %02x", s_stCiHostLicenseConst.SIV[i]);
	}
	VK_Print("\n CLK                   :");
	for (i = 0; i < 16; i++)
	{
		VK_Print(" %02x", s_stCiHostLicenseConst.CLK[i]);
	}
	VK_Print("\n DHH                   :");
	for (i = 0; i < 256; i++)
	{
		if ((i & 15) == 0)
			VK_Print("\n");
		VK_Print(" %02x", s_stCiHostLicenseConst.DHH[i]);
	}
	VK_Print("\n DHPH                  :");
	for (i = 0; i < 256; i++)
	{
		if ((i & 15) == 0)
			VK_Print("\n");
		VK_Print(" %02x", s_stCiHostLicenseConst.DHPH[i]);
	}
	#endif
	VK_Print("\n");

	/* print authentication contexts for all slots */

	for (usSlotId = 0; usSlotId < PAL_CI_GetNumberOfModule(); usSlotId++)
	{
		VK_Print("\n --- Authentication contexts data for slot %d ---\n", usSlotId);

		for (i = 0; i < CI_NUM_OF_AUTH_CTX; i++)
		{
			if ((pstCiAuthCtx = CiCc_GetAuthContext(usSlotId, i)) == NULL)
				break;
			VK_Print("\n - context %d (%s) -", i, (pstCiAuthCtx->ucSavedFlag == 1 || pstCiAuthCtx->ucSavedFlag == 2) ? "saved" : "not saved");
			if (pstCiAuthCtx->ucSavedFlag == 1 || pstCiAuthCtx->ucSavedFlag == 2)
			{
				VK_Print("\n\n ucBindedCamId         :");
				for (j = 0; j < 8; j++)
				{
					VK_Print(" %02X", pstCiAuthCtx->ucBindedCamId[j]);
				}
				// SMiT : 14399 (0x383F), Neotion : 23308 (0x5B0C)
				VK_Print("\n usBindedCamBrandId    : %d (0x%X)", pstCiAuthCtx->usBindedCamBrandId, pstCiAuthCtx->usBindedCamBrandId);
				VK_Print("\n nNegotiatedCckCipher  : %s", pstCiAuthCtx->nNegotiatedCckCipher == CI_CC_KEY_CIPHER_DES_56_ECB ? "DES" : "AES");
			}
			#ifdef Print_CI_Plus_Credentials
			if (pstCiAuthCtx->ucSavedFlag == 2)
			{
				VK_Print("\n DHSK                  :");
				for (j = 0; j < 16; j++)
				{
					VK_Print(" %02x", pstCiAuthCtx->DHSK[j]);
				}
				VK_Print("\n AKH                   :");
				for (j = 0; j < 32; j++)
				{
					VK_Print(" %02x", pstCiAuthCtx->AKH[j]);
				}
			}
			#else
			if (pstCiAuthCtx->ucSavedFlag == 2)
				VK_Print("\n DHSK & AKH            : saved");
			#endif
			VK_Print("\n");
		}
	}

	/* print CC instance values */

	for (i = 0; i < MAX_CI_CC_SS; i++)
	{
		if(s_aCiCc[i].usSsNo)
		{
			if (s_aCiCc[i].usSlotId == usSlotIdToCheck)
			{
				hCc = &s_aCiCc[i];
				break;
			}
		}
	}

	if (hCc == NULL)
	{
		VK_Print("\n There is no CC instance for slot %d\n\n", usSlotIdToCheck);
		return;
	}

	VK_Print("\n --- CC instance for slot %d ---\n", usSlotIdToCheck);

	VK_Print("\n usSsNo                : %d", hCc->usSsNo);
	VK_Print("\n enPhase               : %d", hCc->enPhase);
	VK_Print("\n enArcErr              : %d", hCc->enArcErr);
	VK_Print("\n nCurAuthCtxIdx        : %d", hCc->nCurAuthCtxIdx);
	VK_Print("\n nTryAuthCtxIdx        : %d", hCc->nTryAuthCtxIdx);
	VK_Print("\n ulSacMsgCnt           : %ld", hCc->ulSacMsgCnt);
	VK_Print("\n ulCcTimerID           : %s", hCc->ulCcTimerID ? "timer running" : "timer not running");
	VK_Print("\n ulUriTimerID          : %s", hCc->ulUriTimerID ? "timer running" : "timer not running");

	VK_Print("\n\n - CAM data -\n");

	#ifdef Print_CI_Plus_Credentials
	VK_Print("\n CICAM_BrandCert       : %d bytes", hCc->stCicamData.CICAM_BrandCertLen);
	for (i = 0; i < hCc->stCicamData.CICAM_BrandCertLen; i++)
	{
		if ((i & 15) == 0)
			VK_Print("\n");
		VK_Print(" %02x", hCc->stCicamData.CICAM_BrandCert[i]);
	}
	VK_Print("\n Kp                    :");
	for (i = 0; i < 32; i++)
	{
		VK_Print(" %02x", hCc->stCicamData.Kp[i]);
	}
	VK_Print("\n DHPM                  :");
	for (i = 0; i < 256; i++)
	{
		if ((i & 15) == 0)
			VK_Print("\n");
		VK_Print(" %02x", hCc->stCicamData.DHPM[i]);
	}
	VK_Print("\n CICAM_DevCertLen      : %d bytes", hCc->stCicamData.CICAM_DevCertLen);
	for (i = 0; i < hCc->stCicamData.CICAM_DevCertLen; i++)
	{
		if ((i & 15) == 0)
			VK_Print("\n");
		VK_Print(" %02x", hCc->stCicamData.CICAM_DevCert[i]);
	}
	VK_Print("\n Signature_B           :");
	for (i = 0; i < 256; i++)
	{
		if ((i & 15) == 0)
			VK_Print("\n");
		VK_Print(" %02x", hCc->stCicamData.Signature_B[i]);
	}
	VK_Print("\n auth_nonce            :");
	for (i = 0; i < 32; i++)
	{
		VK_Print(" %02x", hCc->stCicamData.auth_nonce[i]);
	}
	VK_Print("\n Ns_module             :");
	for (i = 0; i < 8; i++)
	{
		VK_Print(" %02x", hCc->stCicamData.Ns_module[i]);
	}
	#endif
	VK_Print("\n latest uri_message    :");
	for (i = 0; i < 8; i++)
	{
		VK_Print(" %02x", hCc->stCicamData.uri_message[i]);
	}
	VK_Print("\n latest program_number : 0x%x (%d)", hCc->stCicamData.usProgramNumber, hCc->stCicamData.usProgramNumber);
	VK_Print("\n latest key_register   : %s", hCc->stCicamData.enKeyRegister == CI_CC_KEY_MODE_EVEN ? "even" : "odd");

	if (flagPrintMoreDetail)
	{
		VK_Print("\n\n - Host data -\n");
		#ifdef Print_CI_Plus_Credentials
		VK_Print("\n Ns_Host               :");
		for (i = 0; i < 8; i++)
		{
			VK_Print(" %02x", hCc->stHostData.Ns_Host[i]);
		}
		VK_Print("\n SEK                   :");
		for (i = 0; i < 16; i++)
		{
			VK_Print(" %02x", hCc->stHostData.SEK[i]);
		}
		VK_Print("\n SAK                   :");
		for (i = 0; i < 16; i++)
		{
			VK_Print(" %02x", hCc->stHostData.SAK[i]);
		}
		VK_Print("\n UCK                   :");
		for (i = 0; i < 32; i++)
		{
			VK_Print(" %02x", hCc->stHostData.UCK[i]);
		}
		#endif
		CiCc_GetNegotiatedCckCipher(usSlotIdToCheck, &enNegotiatedCckCipher);
		VK_Print("\n CCK                   :");
		for (i = 0; i < 8; i++)
		{
			VK_Print(" %02x", hCc->stHostData.CCK[i]);
		}
		if (enNegotiatedCckCipher == CI_CC_KEY_CIPHER_AES_128_CBC)
		{
			for (i = 8; i < 16; i++)
			{
				VK_Print(" %02x", hCc->stHostData.CCK[i]);
			}
			VK_Print("\n CIV                   :");
			for (i = 0; i < 16; i++)
			{
				VK_Print(" %02x", hCc->stHostData.CIV[i]);
			}
		}
	}

	VK_Print("\n\n");

#endif /* #ifdef CONFIG_DEBUG */
}

