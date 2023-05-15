/*******************************************************************
	File Description
********************************************************************/
/**
	@file	ap_cas_na_dal.c
	@brief	ap_cas_na_dal.c

	Description: dal test 용 요구사항 구현 							\n
	Module: AP /CORE /CAS / NA										\n

	Copyright (c) 2009 HUMAX Co., Ltd.								\n
	All rights reserved.											\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <svc_pipe.h>
#include <svc_resources.h>
#include <svc_ch.h>
#include <db_svc.h>
#include <svc_cas.h>
#include <svc_si.h>
#include <mgr_action.h>
#include <mgr_live.h>


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

extern void CASACT_UpdateTunerState(HBOOL bFlag);

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/



/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC Handle_t s_hAction_Castune;
STATIC Handle_t s_hCurSvc;

STATIC HUINT32 s_CasSvcState;

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/


#define _____DEBUG____________________________________________________________________________________________________________________________

#define _____CONVERT___________________________________________________________________________________________________________________________

#define _____CAPABILITY________________________________________________________________________________________________________________________

#define _____SCALER_API________________________________________________________________________________________________________________________

#define _____DEVICE_EVENT______________________________________________________________________________________________________________________

#define _____PUBLIC_APIs_______________________________________________________________________________________________________________________


BUS_Result_t xmgr_cas_NaDAL_Proc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t eGwmResult = ERR_BUS_NO_ERROR;
	HUINT32 nResult = 0;
	HERROR err;

	switch(message)
	{
		case eMEVT_BUS_CREATE:
			break;

		case eSEVT_CH_NO_SIGNAL:
			if(hAction != s_hAction_Castune)
			{
				break;
			}
			CASACT_UpdateTunerState(FALSE);
			break;

		case eSEVT_CH_LOCKED:
			if(hAction != s_hAction_Castune)
			{
				break;
			}
			CASACT_UpdateTunerState(TRUE);
			break;
#if (0)
		case eSEVT_CH_NO_SIGNAL:
			{
				HxLOG_Print("eSEVT_CH_NO_SIGNAL, ActionHandle(0x%x), CasTune ActionHandle (0x%x)\n", hAction, s_hAction_Castune);
				if(hAction != s_hAction_Castune)
				{
					break;
				}
#if 0
				nResult = MW_SI_StopLive(s_hAction_Castune, HANDLE_NULL, eSyncMode);
				if(eResult != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");
#endif

				nResult = SVC_CAS_StopService(s_hAction_Castune, HANDLE_NULL, eSyncMode);
				if(eResult != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

				s_CasSvcState = 0;
			}
			break;

		case eSEVT_CH_LOCKED:
			{
				DbSvc_Info_t service_info;
				SI_ActionType_t eSiLiveActionType;

				HxLOG_Print("eSEVT_CH_LOCKED, ActionHandle(0x%x), CasTune ActionHandle (0x%x)\n", hAction, s_hAction_Castune);

				if(hAction != s_hAction_Castune)
				{
					break;
				}

				err = DB_SVC_GetServiceInfo(s_hCurSvc, &service_info);
				if(err != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");
#if 0
				err = AP_SPEC_GetLiveSiActionType(service_info.eOrgDeliType, &eSiLiveActionType);
				if(err != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

				err = MW_SI_StartLive(s_hAction_Castune, eSiLiveActionType, s_hCurSvc);
				if(err != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");
#endif
				err = SVC_CAS_StartService(s_hAction_Castune, s_hCurSvc, eCAS_ActionType_Live);
				if(err != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

				s_CasSvcState = 1;

				err = SVC_CAS_SetPid(s_hAction_Castune,
									eCAS_ActionType_Live,
									service_info.usVideoPid,
									service_info.usAudioPid,
									service_info.usAudioAuxPid,
									service_info.usDolbyPid,
									service_info.usTtxPid,
									PID_CURRENT);
				if(err != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

			}
			break;

		case eSEVT_SI_PMT:
			{
				DbSvc_Info_t service_info;

				HxLOG_Print("eSEVT_SI_PMT, ActionHandle(0x%x), CasTune ActionHandle (0x%x)\n", hAction, s_hAction_Castune);
				if(hAction != s_hAction_Castune)
				{
					if(SVC_PIPE_GetActionId(hAction) != eActionId_VIEW_0)
					{
						break;
					}
				}

				if(	s_CasSvcState == 0)
				{
					err = SVC_CAS_StartService(s_hAction_Castune, s_hCurSvc, eCAS_ActionType_Live);
					if(err != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

					break;
				}

				err = DB_SVC_GetServiceInfo(s_hCurSvc, &service_info);
				if(err != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

				err = SVC_CAS_SetPid(s_hAction_Castune,
									eCAS_ActionType_Live,
									service_info.usVideoPid,
									service_info.usAudioPid,
									service_info.usAudioAuxPid,
									service_info.usDolbyPid,
									service_info.usTtxPid,
									PID_CURRENT);
				if(err != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");
			}
			break;

		case eMEVT_LIVE_CHANGE_TEMP_PID:

			break;

		case eSEVT_SI_PMT_PIDCHANGED:
		case eSEVT_SI_PMT_PIDREMOVED:
		case eSEVT_SI_PMT_TIMEOUT:
			{
				HxLOG_Print("eSEVT_SI_PMT_PIDCHANGED, ActionHandle(0x%x), CasTune ActionHandle (0x%x)\n", hAction, s_hAction_Castune);
				nResult = SVC_CAS_StopService(s_hAction_Castune, HANDLE_NULL, eSyncMode);
				if(nResult != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

				s_CasSvcState = 0;
			}
			break;

		case eMEVT_BUS_DESTROY:
			{
				HxLOG_Print("eMEVT_BUS_DESTROY, ActionHandle(0x%x), CasTune ActionHandle (0x%x)\n", hAction, s_hAction_Castune);

				nResult = SVC_CAS_StopService(s_hAction_Castune, HANDLE_NULL, eSyncMode);
				if(nResult != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

				s_CasSvcState = 0;

				nResult = SVC_CH_StopTune(s_hAction_Castune, (HUINT16)-1, eSyncMode);
				if(nResult != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

				MGR_ACTION_Release(MGR_ACTION_GetSubActionId(), eActionType_CAS_TUNE);
			}
			break;

#endif
		case eMEVT_BUS_DESTROY:
			{
#if (0)
				HxLOG_Print("eMEVT_BUS_DESTROY, ActionHandle(0x%x), CasTune ActionHandle (0x%x)\n", hAction, s_hAction_Castune);

				nResult = SVC_CH_StopTune(s_hAction_Castune, (HUINT16)-1, eSyncMode);
				if(nResult != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

				MGR_ACTION_Release(MGR_ACTION_GetSubActionId(), eActionType_CAS_TUNE);
#endif
			}
			break;

		default:
			break;
	}

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}




#define _____DEBUG_API______________________________________________________________________________________________________________________________



/*********************** End of File ******************************/
