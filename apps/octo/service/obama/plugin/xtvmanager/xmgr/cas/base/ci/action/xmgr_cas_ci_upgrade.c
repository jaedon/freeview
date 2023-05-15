/**
	@file     xmgr_ciplus_upgrade.c
	@brief    xmgr_ciplus_upgrade.c

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    **************************/
/*******************************************************************/

#include <octo_common.h>

#include <ci.h>
#include <svc_cas.h>

#include <bus.h>

#include <mgr_cas.h>
#include <ci_ctrl.h>

#include "../local_include/_xmgr_cas_ci.h"


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

//extern BUS_Result_t PROC_CAS_TuneMgr(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

STATIC SvcCas_CiPlusUpgrade_t s_stUpgradeStatus;

/********************************************************************/
/********************      Local Functions	*************************/
/********************************************************************/

BUS_Result_t xproc_cas_CiCamUpgrade(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 nResult = 0;

	switch(message)
	{
		case eMEVT_BUS_CREATE:
			HxSTD_memset(&s_stUpgradeStatus, 0x00, sizeof(SvcCas_CiPlusUpgrade_t));
			return MESSAGE_BREAK;

		case eMEVT_BUS_TIMER:
			break;

#if 0 //To comile for Octo2
		case MSG_GWM_QUIT_UI:
			HxLOG_Info("AP_CiPlus_UpgradeProc : MSG_GWM_QUIT_UI - non-ui proc. So msg pass !!\n");
			return MESSAGE_PASS;
#endif

		case eMEVT_BUS_DESTROY:
			break;

		case eMEVT_CAS_CIPLUS_UPGRADE:
			{
				HUINT16 				usSlotId = (HUINT16)p1;
				SvcCas_CiPlusUpgrade_t *pData = NULL;
				SvcCas_CtrlParam_t		stParam;
				HINT32					eCasEvent = 0;
				BUS_Result_t			hBusResult = ERR_BUS_NO_ERROR;

				/*
					p1 : slot id
					p2 : Group ID
					p3 : version id
				*/

				stParam.ulControlType = eCACTRL_CI_GetUpgradeStatus;
				stParam.pvIn	= (void *)&usSlotId;
				stParam.pvOut	= &pData;

				nResult = SVC_CAS_CtrlGet(eDxCAS_GROUPID_CAM, eCACTRL_CI_GetUpgradeStatus, &stParam);
				if(nResult != ERR_OK || NULL == pData)
				{
					break;
				}

				switch (pData->eCamUpgradeMode)
				{
					case eCiPlus_CamUpgrade_Start:
					case eCiPlus_CamUpgrade_Processing:
						eCasEvent = eMEVT_CAS_ACTION_UPDATE_START;
						hBusResult = BUS_PostMessageToOM(NULL, eCasEvent, HANDLE_NULL,
														(HUINT32)eDxCAS_GROUPID_CAM,
														(HINT32)usSlotId,
														(HINT32)0);
						if(hBusResult != ERR_BUS_NO_ERROR)
						{
							HxLOG_Info("bus result %d\n", hBusResult);
						}
						break;

					case eCiPlus_CamUpgrade_DoneOrCancelled:
					case eCiPlus_CamUpgrade_Failed:
						if (pData->eCamUpgradeResetMode == eCiPlus_CamUpgradeReset_PCMCIA)
						{
							CI_ResetSlot(usSlotId);
						}

						if (pData->eCamUpgradeResetMode == eCiPlus_CamUpgradeReset_CAM)
						{
							CI_SetRS(usSlotId);
						}

						eCasEvent = eMEVT_CAS_ACTION_UPDATE_STOP;
						hBusResult = BUS_PostMessageToOM(NULL, eCasEvent, HANDLE_NULL,
														(HUINT32)eDxCAS_GROUPID_CAM,
														(HINT32)usSlotId,
														(HINT32)0);
						if(hBusResult != ERR_BUS_NO_ERROR)
						{
							HxLOG_Info("bus result %d\n", hBusResult);
						}

						BUS_MGR_Destroy(0);
						break;

					case eCiPlus_CamUpgrade_Broken:
						HxLOG_Info("eCiPlus_CamUpgrade_Broken...\n");
						eCasEvent = eMEVT_CAS_ACTION_UPDATE_STOP;
						hBusResult = BUS_PostMessageToOM(NULL, eCasEvent, HANDLE_NULL,
														(HUINT32)eDxCAS_GROUPID_CAM,
														(HINT32)usSlotId,
														(HINT32)0);
						if(hBusResult != ERR_BUS_NO_ERROR)
						{
							HxLOG_Info("bus result %d\n", hBusResult);
						}

						BUS_MGR_Destroy(0);
						break;

					default:
						HxLOG_Error("AP_CiPlus_UpgradeProc : Unknown value (%d) error !!\n", pData->eCamUpgradeMode);
						break;
				}

				s_stUpgradeStatus.eCamUpgradeMode = pData->eCamUpgradeMode;
			}
			/* 여기서 받으면 굳이 action mgr에서 다시 create 할 필요가 없다. msg break 하자 */
			return MESSAGE_BREAK;

#if 0 //To comile for Octo2
		case MSG_GWM_KEYDOWN:
			//if(s_stUpgradeStatus.eCamUpgradeMode != eCiPlus_CamUpgrade_Processing)
			if(s_stUpgradeStatus.eCamUpgradeMode > eCiPlus_CamUpgrade_Processing)	// start, process 인 경우에만 키를 막는다
			{
				HxLOG_Info("AP_CiPlus_UpgradeProc : MSG_GWM_KEYDOWN - key blocked~~ \n");
				break;
			}

			switch(p1) // 아래에 허락된 키 외에는 모두 막는다.
			{
				case KEY_STANDBY:		// CAM F/W upgrade 중에도 사용자가 세트를 끌 수 있도록 허용한다. 다만, 세트가 다시 켜지면 CAM 에 의해 CAM F/W upgrade 가 재개될 것이다.
				case KEY_ARROWUP:		// CAM 이 보여주는 MMI 상에서 스크롤 가능하도록 허용한다.
				case KEY_ARROWDOWN:
				case KEY_VOLUMEUP:		// 불륨 컨트롤은 전혀 지장이 되지 않으므로 허용한다.
				case KEY_VOLUMEDOWN:
				case KEY_MUTE:
					break;

				default: // 위에 허락된 키 외에는 모두 막는다.
					HxLOG_Info("AP_CiPlus_UpgradeProc : Key 0x%X is blocked because CAM F/W upgrade is processing. Wait until it is done !!\n", p1);
					// TODO : Draw warning message... (구현하면 좋겠지만 안해도 그만)
					return MESSAGE_BREAK;
			}
#endif

		default:
			break;
	}

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}


