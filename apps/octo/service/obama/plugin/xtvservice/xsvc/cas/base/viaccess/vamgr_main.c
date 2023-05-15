/**
	@file     VaMgr_main.c
	@brief    VaMgr_main.c (Viaccess MAIN)

	Description:  \n
	Module: MW/CAS/ Viaccess \n
	Remarks :\n

	Copyright (c) 2009 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include "vamgr_main.h"
#include "vamgr_action.h"
#include "_svc_cas_mgr_main.h"
#include "va_main.h"

#include "pal_power.h" // for PAL_PWR_GetWakeUpReason()

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

STATIC VaInstance_t		s_stVaInstance;

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
STATIC void local_vamgr_InitInstance()
{
	int i;

	for (i = 0; i < CAS_MAX_NUM_OF_ACTION; i++)
	{
		s_stVaInstance.stActionList[i].bConnect		= FALSE;
		s_stVaInstance.stActionList[i].bPmt			= FALSE;
		s_stVaInstance.stActionList[i].hAction 		= HANDLE_NULL;
		s_stVaInstance.stActionList[i].hSvc 			= HANDLE_NULL;
	}
}

VaInstance_t *VAMGR_GetInstance()
{
	return &s_stVaInstance;
}

Handle_t VAMGR_GetVaHandle(Handle_t hAction)
{
	int i;

	for (i = 0; i < CAS_MAX_NUM_OF_ACTION; i++)
	{
		if (s_stVaInstance.stActionList[i].hAction == hAction)
			return GET_STBHANDLE(s_stVaInstance.stActionList[i].hAction, s_stVaInstance.stActionList[i].hSvc);
	}

	return HANDLE_NULL;
}

Handle_t VAMGR_GetActionHandle(Handle_t hVaHandle)
{
	HINT32			nActionId;

#if 1
	if (hVaHandle == HANDLE_NULL)
	{
		HBOOL bFlag = TRUE;
		int i;

		for (i = 0; i < CAS_MAX_NUM_OF_ACTION; i++)
		{
			if (s_stVaInstance.stActionList[i].hAction != HANDLE_NULL &&
				s_stVaInstance.stActionList[i].eActType == eCAS_ActionType_Live &&
				s_stVaInstance.stActionList[i].bConnect == TRUE)
			{
				bFlag = FALSE;
				break;
			}
		}
	}
#endif

	nActionId = GET_ACTION_ID_FROM_STBHANDLE(hVaHandle);

	if (nActionId < CAS_MAX_NUM_OF_ACTION)
	{
		return s_stVaInstance.stActionList[nActionId].hAction;
	}

	return HANDLE_NULL;
}

SvcCas_ActionType_e VAMGR_GetActType(Handle_t hVaHandle)
{
	HINT32			nActionId;

	nActionId = GET_ACTION_ID_FROM_STBHANDLE(hVaHandle);

	if (nActionId < CAS_MAX_NUM_OF_ACTION)
	{
		return s_stVaInstance.stActionList[nActionId].eActType;
	}

	return eCAS_ActionType_None;
}

#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
HBOOL VAMGR_IsLive(Handle_t hVaHandle)
{
	HINT32			nActionId;

	nActionId = GET_ACTION_ID_FROM_STBHANDLE(hVaHandle);

	if (nActionId < CAS_MAX_NUM_OF_ACTION)
	{
		if (s_stVaInstance.stActionList[nActionId].eActType == eCAS_ActionType_Live)
			return TRUE;
	}

	return FALSE;
}

HBOOL VAMGR_IsSameSvcWithLive(Handle_t hVaHandle)
{
	HINT32			nActionId;
	int i;

	for (i = 0; i < CAS_MAX_NUM_OF_ACTION; i++)
	{
		if (s_stVaInstance.stActionList[i].eActType == eCAS_ActionType_Live)
		{
			nActionId = GET_ACTION_ID_FROM_STBHANDLE(hVaHandle);

			if (nActionId < CAS_MAX_NUM_OF_ACTION)
			{
				// �ڽ��� live�� �ƴϸ鼭, live�� ���� svc�� ó���ϴ� ��츸 true.
				if (i != nActionId && s_stVaInstance.stActionList[i].hSvc == s_stVaInstance.stActionList[nActionId].hSvc)
					return TRUE;
			}

			break;
		}
	}

	return FALSE;
}
#endif

HERROR VAMGR_GetTripleID(Handle_t hVaHandle, HUINT16 *pusOrgNetId, HUINT16 *pusTsId, HUINT16 *pusUniqueId, HUINT16 *pusServiceId)
{
	HINT32			nActionId;
	SvcCas_Context_t 	*pContext;

	nActionId = GET_ACTION_ID_FROM_STBHANDLE(hVaHandle);

	if (nActionId >= CAS_MAX_NUM_OF_ACTION || s_stVaInstance.stActionList[nActionId].hAction == HANDLE_NULL)
	{
		return ERR_FAIL;
	}

	pContext = svc_cas_MgrGetActionContextByAction(s_stVaInstance.stActionList[nActionId].hAction);
	if (pContext == NULL)
		return ERR_FAIL;

	*pusOrgNetId = pContext->usOrgNetId;
	*pusTsId = pContext->usTsId;
	*pusUniqueId = pContext->usUniqueId;
	*pusServiceId = pContext->usSvcId;

	return ERR_OK;
}

void VAMGR_Task_Init()
{
	static HBOOL bFirstTime = TRUE;

	if (bFirstTime)
	{
		/*
			������ ���� CASE ���� �߻��� EEPROM�� �ʱ�ȭ �� �ָ� �ذ� �� �� ����
			1. ��Ȥ VA ACS library ��ŸƮ �� STB�� ������ ���� ���� ��
			2. OP MSG �׽�Ʈ�� �޽����� ���� �� �̺�Ʈ�� �߻����� ���� ��

			��� �� �ݵ�� �ּ�ó�� �� �ּ���~~!!
		*/
//		VA_EraseAllAcsData();
//		VA_EraseAllExtData();

		// va task start
		VA_Start();

		// task�� �ٷ� start�� ���� ������ ���� ������ ��ø� ��ٷ�����..
		while (VA_MAIN_TaskStarted() == FALSE)
		{
			VK_TASK_Sleep(10);
		}

		// casmgr�� ����
		VAACT_Init();

		bFirstTime = FALSE;
	}
	else
	{
		HxLOG_Print("VAMGR_Task_Init() already called.\n");

#if defined(CONFIG_PROD_BXRHD)  || defined(CONFIG_PROD_BXRHDPLUS)
		{
			// industrial CAM test�� active standby->pwr on�ÿ� ������ msg ������ �ʴ� ���� ����
			// ���� �ڵ�δ� VA�� active standby�� ������ �ٽ� pwr on�Ǿ��ٴ� ����� �𸣹Ƿ� �ٽ� ������ msg(ex-camlock ko)�� ������ �ʾ� msg popup�� ������ �ʴ´�
			// va standby control�� �����ϴ� ���� ���� �� ������ ���� �����Ǿ� ���� �����Ƿ�,  �Ʒ��� ���� card re-insertion�ϴ� ��ó�� ó���Ѵ�
			#include "va_sc_api.h"
			VA_SC_ResetByHost(0);
		}
#endif
	}
}

HERROR xsvc_cas_VaMgrInit(void)
{
	WakeUpReason_t eWakeUpReason = eWAKEUP_BY_REBOOT;

	HxLOG_Print("[Enter]\n");

	local_vamgr_InitInstance();

	// va init
	VA_Init();

#if 0
	VAMGR_Task_Init(); // local_am_MsgGwmCreate() -> AP_CAS_VA_Start() ���� ȣ��ǵ��� �����.
#endif

	// ���� ��ȭ�ÿ��� VAMGR_Task_Init ȣ���� �ȵȴ�.. ���⼭ Ȯ���غ��� ȣ��������..
	PAL_PWR_GetWakeUpReason(&eWakeUpReason);

	if ( eWakeUpReason == eWAKEUP_BY_TIMER )
	{
		HxLOG_Print("VAMGR_Task_Init() called by XSVC_CAS_VAMGR_Init()\n");
		VAMGR_Task_Init();
	}

	HxLOG_Print("[Exit]\n");

	return ERR_OK;
}


/*********************** End of File ******************************/

