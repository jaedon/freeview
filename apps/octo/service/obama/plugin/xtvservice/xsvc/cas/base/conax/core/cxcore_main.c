/*******************************************************************
	File Description
*******************************************************************/
/*++
	@file   	 cxcore_main.c
	@brief  	 Conax Core Main Procedure

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2011 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
--*/
/*******************************************************************/
/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ⓒ 2011-2012 Humax Co., Ltd.
* This program is produced by Humax Co., Ltd. ("Humax") and
* the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
* non-assignable, non-transferable and non-exclusive license to use this Software.
* You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
* you agree to the responsibility to take all reasonable efforts to protect the any information
* you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
* reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
* If you have no authorized license, discontinue using this Software immediately.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
* IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
* IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/
/*******************************************************************/
#define ________CXCORE_MAIN_Private_Include___________________________________
/*******************************************************************/
#include <cxmgr_action.h>
#include "../local_include/cxcore_main.h"
#include "../local_include/cxcore_os.h"

#include "../local_include/cxcore_ecm.h"
#include "../local_include/cxcore_emm.h"
#include "../local_include/cxcore_stream.h"
#include "../local_include/cxcore_session.h"
#include "../local_include/cxcore_sc.h"
#include "../local_include/cxcore_ui.h"
#include "../local_include/cxcore_nvm.h"
#include "../local_include/cxcore_cpm.h"
#include "../local_include/cxcore_pvr.h"
#include "../local_include/cx_nvram.h"
#include "../local_include/cx_dscr.h"
#include "../local_include/cx_dmx.h"

//#include <pal_video.h>
//#include <pal_audio.h>
/*******************************************************************/
#define ________CXCORE_MAIN_Golbal_Value______________________________________
/*******************************************************************/
/*******************************************************************/
#define ________CXCORE_MAIN_Private_Define____________________________________
/*******************************************************************/
#define CX_TASK_MSGQ_SIZE						64
#define CX_TASK_PRIORITY 						(VK_TASK_PRIORITY_MW_REF + 3)
#define CX_TASK_STACK_SIZE						SIZE_32K

typedef enum
{
	eCxEcmPause_None = 0x00,
	eCxEcmPause_WaitCat = 0x01,
	eCxEcmPause_DisplayUi = 0x02,
}eCxEcmPauseReason_t;

/*******************************************************************/
#define ________CXCORE_MAIN_Private_Static_Value______________________________
/*******************************************************************/

STATIC HBOOL	s_bCxCoreMainTaskRun;
STATIC HULONG	s_ulCxCoreMainTaskId		= 0;
STATIC HULONG	s_ulCxCoreMainTaskMsgQId	= 0;

STATIC HULONG	s_ulCxCoreScheduleTaskId	= 0;

STATIC HULONG	s_ulWaitCatTimerId			= 0;
STATIC HULONG	s_ulHideUiTimerId			= 0;
STATIC eCxEcmPauseReason_t s_eCxEcmPauseReason = eCxEcmPause_None;

/*******************************************************************/
#define ________CXCORE_MAIN_Global_Value______________________________________
/*******************************************************************/
/*******************************************************************/
#define ________CXCORE_MAIN_Private_Static_Prototype__________________________
/*******************************************************************/
STATIC void local_cxcore_HandleMsg(CxMsg_t* pstCxMsg);
STATIC void local_cxcore_HandleEcmMsg(void);
STATIC void local_cxcore_HandleEmmMsg(void);
STATIC void local_cxcore_CreateTask(void);
STATIC void local_cxcore_CreateMsgQ(void);
STATIC void local_cxcore_StartTask(void* argv);
#if defined(CONFIG_DEBUG)
STATIC HUINT8* local_cxcore_GetMsgName(CxMsgType_t eType);
#endif
/*******************************************************************/
#define ________CXCORE_MAIN_Private_Static_Prototype_Body_____________________
/*******************************************************************/

#define Conax_Task_________________________________________________

STATIC void local_cxcore_ResumeEcmProcess(eCxEcmPauseReason_t eReason)
{
	s_eCxEcmPauseReason &= (~eReason);

	if(s_eCxEcmPauseReason == eCxEcmPause_None)
	{
		CXCORE_SendMsg(eCxMsg_Ecm, (Handle_t)HANDLE_NULL, (HUINT32)0, (HUINT32)0);
	}
}

STATIC void local_cxsc_cbCatTimeout(unsigned long ulTimerId, void* pvParam)
{
	if (ulTimerId != s_ulWaitCatTimerId)
	{
		return;
	}

	s_ulWaitCatTimerId = VK_TIMERID_NULL;

	if(s_eCxEcmPauseReason & eCxEcmPause_WaitCat)
	{
		local_cxcore_ResumeEcmProcess(eCxEcmPause_WaitCat);
	}
}

STATIC HERROR local_cxsc_CatReceived(void)
{
	if (s_ulWaitCatTimerId != VK_TIMERID_NULL)
	{
		VK_TIMER_Cancel(s_ulWaitCatTimerId);
		s_ulWaitCatTimerId = VK_TIMERID_NULL;
	}

	if(s_eCxEcmPauseReason & eCxEcmPause_WaitCat)
	{
		local_cxcore_ResumeEcmProcess(eCxEcmPause_WaitCat);
	}

	return ERR_OK;
}

STATIC void local_cxcore_WaitCat(HUINT32 ulMsTime)
{
	if(s_ulWaitCatTimerId != VK_TIMERID_NULL)
	{
		VK_TIMER_Cancel(s_ulWaitCatTimerId);
		s_ulWaitCatTimerId = VK_TIMERID_NULL;
	}

	s_eCxEcmPauseReason |= eCxEcmPause_WaitCat;

#if defined(CONFIG_MW_CAS_CONAX_PRETEST)
	// 뭐 할까..?
#else
	VK_TIMER_EventAfter(ulMsTime, local_cxsc_cbCatTimeout, NULL, 0, &s_ulWaitCatTimerId);
#endif
}

STATIC void local_cxsc_cbHideUi(unsigned long ulTimerId, void* pvParam)
{
	if (ulTimerId != s_ulHideUiTimerId)
	{
		return;
	}

	s_ulHideUiTimerId = VK_TIMERID_NULL;

	if(s_eCxEcmPauseReason & eCxEcmPause_DisplayUi)
	{
		local_cxcore_ResumeEcmProcess(eCxEcmPause_DisplayUi);
	}
}

STATIC void local_cxcore_DisplayUi(HUINT32 ulMsTime)
{
	if(s_ulHideUiTimerId != VK_TIMERID_NULL)
	{
		VK_TIMER_Cancel(s_ulHideUiTimerId);
		s_ulHideUiTimerId = VK_TIMERID_NULL;
	}

	s_eCxEcmPauseReason |= eCxEcmPause_DisplayUi;

	VK_TIMER_EventAfter(ulMsTime, local_cxsc_cbHideUi, NULL, 0, &s_ulHideUiTimerId);
}

STATIC void local_cxcore_HandleMsg(CxMsg_t* pstCxMsg)
{
	ENTER_FUNCTION;

#if defined(CONFIG_DEBUG)
	HxLOG_Info("MSG NAME : %s\r\n", local_cxcore_GetMsgName(pstCxMsg->eMsgType));
#endif

	switch (pstCxMsg->eMsgType)
	{
		// messages to smartcard ------------------------------
	case eCxMsg_Sc_Reset:
		/*
			labatus : CAT를 보내기 전에 ECM을 보내지 말란다. 하여 처음에 타이머 걸어 놓고 CAT 받으면 풀자...
			TATA : CAT가 없거나 늦게 오면...? 그래도 3초 기다려 보고 ECM을 보내란다... 길게 잡아서 5초로 하자.
		*/
		local_cxcore_WaitCat(5000);
		CXSC_ColdReset((HINT32)pstCxMsg->ulParam);
		break;

	case eCxMsg_Sc_RecoveryReset:
		if (pstCxMsg->ulParam2 == TRUE)
		{
			/*
				s/c에서 comm fail 등으로 인해 message를 출력해야 하는데 ecm이 처리되면서 CAS_OK가 올라간다...
				5초 동안 ecm이 처리되지 않도록 작업함...
			*/
			local_cxcore_DisplayUi(5000);
		}

		local_cxcore_WaitCat(5000);
		CXSC_RecoveryReset((HINT32)pstCxMsg->ulParam, (HBOOL)pstCxMsg->ulParam2);
		break;

	case eCxMsg_Sc_CatReceived:
		local_cxsc_CatReceived();
		break;

	case eCxMsg_Ecm:
		// filter를 다시 건다 (toggle 80 <-> 81)
		// hSession에는 hSession이 아닌 hEcm이 들어있다.

#if 0	// ecm filter는 여기서 restart 시킨다.
		CXECM_RestartFilter(pstCxMsg->hHandle, (CxEcmMode_t)pstCxMsg->ulParam);
#else
		local_cxcore_HandleEcmMsg();
#endif
		break;

	case eCxMsg_Emm:
		// smartcard로 emm을 전달한다
		local_cxcore_HandleEmmMsg();
		break;

		// messages from smartcard ------------------------------
	case eCxMsg_Sc_Extracted:
		CXSESSION_ScExtracted((HINT32)pstCxMsg->ulParam);
		break;

	case eCxMsg_Sc_Ready:
		CXSESSION_ScInserted((HINT32)pstCxMsg->ulParam);
		// 카드 number도 미리 읽어옴
		CXSC_RequestCardNumber((HINT32)pstCxMsg->ulParam);
		break;

	case eCxMsg_Ecm_FilterRestart:
		CXSTREAM_RestartAllEcmFilter(pstCxMsg->hHandle);
		break;

		// messages from CasMgr ----------------------------

	case eCxMsg_Svc_Start:
		CXSESSION_StartSvc(pstCxMsg->hHandle);
		break;

	case eCxMsg_Svc_Stop:
		CXSESSION_StopSvc(pstCxMsg->hHandle, (HBOOL)pstCxMsg->ulParam);
		break;

	case eCxMsg_Svc_SetPid:
		CXSESSION_SetPid(pstCxMsg->hHandle, (CxEsPidInfo_t *)pstCxMsg->ulParam);
		//CXSESSION_SetPid(pstCxMsg->hHandle, (HUINT16)pstCxMsg->ulParam, (HBOOL)pstCxMsg->ulParam2);
		break;

	case eCxMsg_Svc_UpdatePmt:
		CXSESSION_UpdatePmt(pstCxMsg->hHandle, (SvcCas_PmtInfo_t *)pstCxMsg->ulParam);
		break;

	case eCxMsg_Svc_UpdateCat:
		CXSESSION_UpdateCat(pstCxMsg->hHandle, (HUINT16)pstCxMsg->ulParam, (HUINT8 *)pstCxMsg->ulParam2);
		break;

	case eCxMsg_Svc_Select:
		CXSESSION_Select(pstCxMsg->hHandle, (HBOOL)pstCxMsg->ulParam);
		break;

#if defined(CONFIG_MW_MM_DELAYED_REC)
	case eCxMsg_Svc_Move:
		CXSESSION_MoveSvc(pstCxMsg->hHandle);
		break;
#endif

#if defined(CONFIG_SUPPORT_FCC)
	case eCxMsg_Svc_SetMainView:
		CXSESSION_SetMainService(pstCxMsg->hHandle, (Handle_t)pstCxMsg->ulParam);
		break;
#endif

	case eCxMsg_Ui_CheckUserTextAtStandby:
		CXUI_Trick_UserTextReceived();
		break;

	case eCxMsg_Ui_CheckFingerprintAtStandby:
		CXUI_Trick_FingerprintReceived();
		break;

#if defined(CONFIG_MW_CAS_CONAX_TEXT)
	case eCxMsg_Ui_CheckUserTextAtBoot:
		CXUI_CheckUserTextAvailable();
		break;

	case eCxMsg_Ui_RetryUserText:
		CXUI_RetryUserText(pstCxMsg->ulParam);
		break;
#endif

#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)
	case eCxMsg_Ui_RetryFingerprint:
		CXUI_RetryFingerprint();
		break;
#endif

	case eCxMsg_ScheduleActionDone:
		if(s_ulCxCoreScheduleTaskId != 0)
		{
			VK_TASK_Stop(s_ulCxCoreScheduleTaskId);
			VK_TASK_Destroy(s_ulCxCoreScheduleTaskId);
			s_ulCxCoreScheduleTaskId = 0;
		}
		break;

#if defined(CONFIG_MW_CAS_CONAX_URI)
	case eCxMsg_UriTimeout:
		CXSESSION_UriTimeout(pstCxMsg->hHandle);
		break;
#endif

	default :
#if defined(CONFIG_DEBUG)
		HxLOG_Error("Doesn't have handle function !!! %s\r\n", local_cxcore_GetMsgName(pstCxMsg->eMsgType));
#endif
		break;
	}

	LEAVE_FUNCTION;
}

STATIC void local_cxcore_HandleEcmMsg(void)
{
	HUINT8* 	pucData;
	Handle_t	hEcm, hSession;

	ENTER_FUNCTION;
	while (1)
	{
		if (s_eCxEcmPauseReason != eCxEcmPause_None)
		{
			break;
		}

		pucData = CXECM_QueueGet(&hSession, &hEcm);
		if (pucData == NULL)
		{
			break;
		}

		// smartcard로 ecm을 전달한다
		CXSC_SendEcm(hSession, hEcm, pucData);
		CX_MEM_Free(pucData);
	}
	LEAVE_FUNCTION;
}

STATIC void local_cxcore_HandleEmmMsg(void)
{
	HUINT8* pucData;
	HINT32	nScSlot;

	ENTER_FUNCTION;
	pucData = CXEMM_QueueGet(&nScSlot);
	if (pucData)
	{
		CXSC_SendEmm(nScSlot, pucData);

		CX_MEM_Free(pucData);
	}
	LEAVE_FUNCTION;
}

STATIC void local_cxcore_Task(void)
{
	HUINT32	nResult	= 0;
	CxMsg_t	stCxMsg;

	while (s_bCxCoreMainTaskRun)
	{
		nResult = CX_MSG_Receive(s_ulCxCoreMainTaskMsgQId, &stCxMsg, sizeof(CxMsg_t));

#if defined(CONFIG_DEBUG)
		HxLOG_Print("RCV_MSG[%s]\n", local_cxcore_GetMsgName(stCxMsg.eMsgType));
#endif
		if (nResult == ERR_OK && stCxMsg.eMsgType < eCxMsg_Max) /* 메시지를 받음 */
		{
			//## ECM 처리가 최우선이므로 ecm 큐에 뭐가 있나 보고 있으면 처리
			// 메시지가 ecm 메시지 였으면 다음 메시지를 얻어온다
			local_cxcore_HandleEcmMsg();
			local_cxcore_HandleMsg(&stCxMsg);
		}
		else
		{
			HxLOG_Error("GetMessage Error !!! : 0x%X\n", nResult);
			continue;
		}
	}

	CX_MSG_Destroy(s_ulCxCoreMainTaskMsgQId);
	return;
}


STATIC void local_cxcore_CreateTask(void)
{
	HUINT32	nResult	= 0;

	nResult = (HUINT32)CX_TASK_Create((void*)local_cxcore_StartTask, CX_TASK_PRIORITY, CX_TASK_STACK_SIZE, "CXCORE", NULL, &s_ulCxCoreMainTaskId, 0);
	if (nResult == ERR_OK)
	{
		nResult = CX_TASK_Start(s_ulCxCoreMainTaskId);
	}
}

STATIC void local_cxcore_CreateMsgQ(void)
{
	HUINT32	nResult	= 0;

	// create message queue in advance
	nResult = (HUINT32)CX_MSG_Create(CX_TASK_MSGQ_SIZE, sizeof(CxMsg_t), "CxTaskQ", &s_ulCxCoreMainTaskMsgQId);
	if (nResult != ERR_OK)
	{
		HxLOG_Error("CXAPI_StartTask: fail to create msg queue\n");
		return;
	}
}


STATIC void local_cxcore_StartTask(void* argv)
{
	ENTER_FUNCTION;

	(void)argv;

	if (s_bCxCoreMainTaskRun)
	{
		HxLOG_Error("Core Main Task is already started\n");
		LEAVE_FUNCTION;
		return;
	}

	local_cxcore_CreateMsgQ();

	s_bCxCoreMainTaskRun = TRUE;

	local_cxcore_Task();
	LEAVE_FUNCTION;
	return;
}

STATIC void local_cxcore_TaskScheduleAction(void* argv)
{
	while(1)
	{
		if(CXACT_GetActionHandle(HANDLE_NULL) != HANDLE_NULL)
		{
//			CXUI_CheckUserTextAvailable();
			CXUI_CheckFingerprintAvailable();
			break;
		}

		VK_TASK_Sleep(1000);
	}

	CXCORE_SendMsg(eCxMsg_ScheduleActionDone, HANDLE_NULL, 0, 0);
}

/*******************************************************************/
#define ________CXCORE_MAIN_Public_Functions_Body_____________________________
/*******************************************************************/
HERROR CXAPI_ReadyShutdown(HBOOL bPowerSavingStandby)
{
	if(bPowerSavingStandby == TRUE)
	{
#if defined(CONFIG_SUPPORT_S3MODE_FASTBOOT)
		CXSESSION_DeInitCore();
#endif
	}

	return ERR_OK;
}

//Sandby -> Wakeup 시에 예약 메시지 Display
HERROR CXAPI_QuitStandbyForMMI(void)
{
	CX_TASK_Sleep(1500);
	CXCORE_SendMsg(eCxMsg_Ui_CheckUserTextAtStandby, HANDLE_NULL, (HUINT32)0, (HUINT32)0);

	return ERR_OK;
}

HERROR CXAPI_GoOperation(void)
{
#if defined(CONFIG_MW_CAS_CONAX_TEXT) || defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)
	if(s_ulCxCoreScheduleTaskId != 0)
	{
		VK_TASK_Stop(s_ulCxCoreScheduleTaskId);
		VK_TASK_Destroy(s_ulCxCoreScheduleTaskId);
	}

	if(VK_TASK_Create(local_cxcore_TaskScheduleAction, CX_TASK_PRIORITY, CX_TASK_STACK_SIZE, "CXCORE_SCHEDULE", NULL, &s_ulCxCoreScheduleTaskId, 0) != ERR_OK)
	{
		HxLOG_Error("CX_TASK_Create() error\n");
		return ERR_FAIL;
	}

	CX_TASK_Start(s_ulCxCoreScheduleTaskId);
#endif
	return ERR_OK;
}

HERROR CXAPI_WakeUpStandby(HBOOL bPowerSavingStandby)
{
	if(bPowerSavingStandby == TRUE)
	{
#if defined(CONFIG_SUPPORT_S3MODE_FASTBOOT)
		HxLOG_Print("S3 FAST BOOT mode...\n");
#endif
	}
	else
	{
		CXSESSION_ResetAllSession();
	}

	// SMARTCARD외 다른 Action이 필요하면 넣으세요..
	CXACT_ReInit();
	return ERR_OK;
}

HERROR CXAPI_StartTask(void)
{
	ENTER_FUNCTION;
	local_cxcore_StartTask(NULL);
	LEAVE_FUNCTION;
	return ERR_OK;
}

HERROR CXAPI_StopTask(void)
{
	s_bCxCoreMainTaskRun = FALSE;

	return ERR_OK;
}


HERROR CXAPI_Initialize(void)
{
	// core module
	CXSESSION_InitCore();
	CXSTREAM_InitCore();

	CXDMX_Init();

	CXECM_InitCore();
	CXEMM_InitCore();

	//CXSC_InitCore();

	CXUI_InitCore();

	CXNVM_Init();
	CXNVM_InitCore();

#if defined(CONFIG_MW_CAS_CONAX_CPM)
	CXCPM_InitCore();
#endif

#if defined(CONFIG_MW_MM_PVR)
	CXPVR_InitCore();
#endif

	local_cxcore_CreateTask();

	// adaptation layer init
	CXDSCR_Init();

	// task가 바로 start를 하지 못했을 수도 있으니 잠시만 기다려주자..
	while (s_bCxCoreMainTaskRun == FALSE)
	{
		CX_TASK_Sleep(10);
	}

	CXSC_InitCore();

#if defined(CONFIG_MW_CAS_CONAX_TEXT) || defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)
	CXAPI_QuitStandbyForMMI();
#endif

	return ERR_OK;
}

//hHandle은 대부분 hSession이고 특별한 경우 hEcm등이 전달될 수도 있다.
HERROR CXCORE_SendMsg(CxMsgType_t eMsgType, Handle_t hHandle, HUINT32 ulParam, HUINT32 ulParam2)
{
	CxMsg_t	stCxMsg;

	if (s_bCxCoreMainTaskRun)
	{
		stCxMsg.eMsgType = eMsgType;
		stCxMsg.hHandle = hHandle;
		stCxMsg.ulParam = ulParam;
		stCxMsg.ulParam2 = ulParam2;

		CX_MSG_Send(s_ulCxCoreMainTaskMsgQId, &stCxMsg, sizeof(CxMsg_t));
	}

	return ERR_OK;
}

#define Debugging__________________________________________________
#if defined(CONFIG_DEBUG)
STATIC HUINT8* local_cxcore_GetMsgName(CxMsgType_t eType)
{
	switch (eType)
	{
		ENUM_TO_STR(eCxMsg_Sc_Reset);
		ENUM_TO_STR(eCxMsg_Sc_RecoveryReset);
		ENUM_TO_STR(eCxMsg_Sc_Extracted);
		ENUM_TO_STR(eCxMsg_Sc_Ready);

		ENUM_TO_STR(eCxMsg_Ecm);
		ENUM_TO_STR(eCxMsg_Ecm_FilterRestart);

		ENUM_TO_STR(eCxMsg_Emm);

		ENUM_TO_STR(eCxMsg_PullEmm_EmmConfig);
		ENUM_TO_STR(eCxMsg_PullEmm_Emm);

		ENUM_TO_STR(eCxMsg_Ui_CheckUserTextAtBoot);
		ENUM_TO_STR(eCxMsg_Ui_RetryUserText);
		ENUM_TO_STR(eCxMsg_Ui_CheckFingerprintAtBoot);
		ENUM_TO_STR(eCxMsg_Ui_RetryFingerprint);
		ENUM_TO_STR(eCxMsg_Ui_CheckUserTextAtStandby);
		ENUM_TO_STR(eCxMsg_Ui_CheckFingerprintAtStandby);

		ENUM_TO_STR(eCxMsg_Svc_Start);
		ENUM_TO_STR(eCxMsg_Svc_Stop);
		ENUM_TO_STR(eCxMsg_Svc_SetPid);
		ENUM_TO_STR(eCxMsg_Svc_UpdatePmt);
		ENUM_TO_STR(eCxMsg_Svc_UpdateCat);
		ENUM_TO_STR(eCxMsg_Svc_Select);
		ENUM_TO_STR(eCxMsg_Max);

	default:
		break;
	}

	return (HUINT8 *)"eCxMsg_Unknown";
}

static int local_cxcore_ConaxCommand(void* szArgList)
{
	char* 			aucArg			= NULL;
	HUINT8			aucStr[20 + 1]	=
	{
		0,
	};

	/* 1st argument */
	aucArg = (char*)HLIB_CMD_GetNextArg((HCHAR * *)&szArgList);
	if (aucArg == NULL)
	{
		return HxCMD_ERR;
	}
	if (snprintf(aucStr, 20, "%s", aucArg) == 0)
	{
		return HxCMD_ERR;
	}

	HLIB_STD_StrUpper(aucStr);

	if (strcmp(aucStr, "SC") == 0)
	{
		aucArg = (char*)HLIB_CMD_GetNextArg((HCHAR * *)&szArgList);
		if (aucArg == NULL)
		{
			return HxCMD_ERR;
		}
		if (snprintf(aucStr, 20, "%s", aucArg) == 0)
		{
			return HxCMD_ERR;
		}
		HLIB_STD_StrUpper(aucStr);

		if (strcmp(aucStr, "SHOW") == 0)
		{
			CMD_CXSC_PrintScContext();
			return HxCMD_OK;
		}
		else
		{
			HLIB_CMD_Printf("choose SC command\n");
		}
	}
	else
	if (strcmp(aucStr, "SR") == 0)
	{
		aucArg = (char*)HLIB_CMD_GetNextArg((HCHAR * *)&szArgList);
		if (aucArg == NULL)
		{
			return HxCMD_ERR;
		}
		if (snprintf(aucStr, 20, "%s", aucArg) == 0)
		{
			return HxCMD_ERR;
		}
		HLIB_STD_StrUpper(aucStr);

		if (strcmp(aucStr, "SHOW") == 0)
		{
			CMD_CXSTREAM_PrintStreamContext();
			return HxCMD_OK;
		}
		else
		{
			HLIB_CMD_Printf("choose STREAM command\n");
		}
	}
	else
	if (strcmp(aucStr, "SS") == 0)
	{
		aucArg = (char*)HLIB_CMD_GetNextArg((HCHAR * *)&szArgList);
		if (aucArg == NULL)
		{
			return HxCMD_ERR;
		}
		if (snprintf(aucStr, 20, "%s", aucArg) == 0)
		{
			return HxCMD_ERR;
		}
		HLIB_STD_StrUpper(aucStr);

		if (strcmp(aucStr, "SHOW") == 0)
		{
			CMD_CXSESSION_PrintSessionContext();
			return HxCMD_OK;
		}
		else
		{
			HLIB_CMD_Printf("choose ECM command\n");
		}
	}
	else
	if (strcmp(aucStr, "ECM") == 0)
	{
		aucArg = (char*)HLIB_CMD_GetNextArg((HCHAR * *)&szArgList);
		if (aucArg == NULL)
		{
			return HxCMD_ERR;
		}
		if (snprintf(aucStr, 20, "%s", aucArg) == 0)
		{
			return HxCMD_ERR;
		}
		HLIB_STD_StrUpper(aucStr);

		if (strcmp(aucStr, "SHOW") == 0)
		{
			CMD_CXECM_PrintEcmContext();
			return HxCMD_OK;
		}
		else
		{
			HLIB_CMD_Printf("choose ECM command\n");
		}
	}
	else
	if (strcmp(aucStr, "EMM") == 0)
	{
		aucArg = (char*)HLIB_CMD_GetNextArg((HCHAR * *)&szArgList);
		if (aucArg == NULL)
		{
			return HxCMD_ERR;
		}
		if (snprintf(aucStr, 20, "%s", aucArg) == 0)
		{
			return HxCMD_ERR;
		}
		HLIB_STD_StrUpper(aucStr);

		if (strcmp(aucStr, "SHOW") == 0)
		{
			CMD_CXEMM_PrintEmmContext();
			return HxCMD_OK;
		}
		else
		{
			HLIB_CMD_Printf("choose ECM command\n");
		}
	}


	return HxCMD_ERR;
}

void CMD_Register_SVC_CAS_ConaxCoreCmd(HxCMD_Handle_t hCasCxCmdHandle)
{
	HLIB_CMD_RegisterWord(hCasCxCmdHandle,		//
		local_cxcore_ConaxCommand,				//
		"cxcore", /* keyword */
		"conax command : cxcore [SC|SR|ECM|EMM] [SHOW]", /* help   */
		"cxcore [SC|SR|ECM|EMM] [SHOW]");	/* usage  */

	return;
}
#endif

