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
				// 자신이 live가 아니면서, live와 같은 svc를 처리하는 경우만 true.
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
			다음과 같은 CASE 문제 발생시 EEPROM을 초기화 해 주면 해결 될 수 있음
			1. 간혹 VA ACS library 스타트 시 STB가 부팅이 되지 않을 때
			2. OP MSG 테스트시 메시지를 받은 후 이벤트가 발생하지 않을 때

			사용 후 반드시 주석처리 해 주세요~~!!
		*/
//		VA_EraseAllAcsData();
//		VA_EraseAllExtData();

		// va task start
		VA_Start();

		// task가 바로 start를 하지 못했을 수도 있으니 잠시만 기다려주자..
		while (VA_MAIN_TaskStarted() == FALSE)
		{
			VK_TASK_Sleep(10);
		}

		// casmgr에 연결
		VAACT_Init();

		bFirstTime = FALSE;
	}
	else
	{
		HxLOG_Print("VAMGR_Task_Init() already called.\n");

#if defined(CONFIG_PROD_BXRHD)  || defined(CONFIG_PROD_BXRHDPLUS)
		{
			// industrial CAM test중 active standby->pwr on시에 적절한 msg 나오지 않는 문제 수정
			// 현재 코드로는 VA가 active standby에 진입후 다시 pwr on되었다는 사실을 모르므로 다시 적절한 msg(ex-camlock ko)를 보내지 않아 msg popup이 나오지 않는다
			// va standby control를 구현하는 것이 좋을 것 같으나 현재 구현되어 있지 않으므로,  아래와 같이 card re-insertion하는 것처럼 처리한다
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
	VAMGR_Task_Init(); // local_am_MsgGwmCreate() -> AP_CAS_VA_Start() 에서 호출되도록 변경됨.
#endif

	// 예약 녹화시에는 VAMGR_Task_Init 호출이 안된다.. 여기서 확인해보고 호출해주자..
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

