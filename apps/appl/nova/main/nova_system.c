/**************************************************************
*	http://www.humaxdigital.com
*	@author			humax
**************************************************************/


/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ¨Ï 2011-2012 Humax Co., Ltd.
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

/**************************************************************************************************/
#define _________Nova_Private_Include_________________________________________________
/**************************************************************************************************/
#include <nova_int.h>
/**************************************************************************************************/
#define _________Nova_Private_Macro_________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________Nova_Private_Struct_________________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define _________Nova_Private_Value_________________________________________________
/**************************************************************************************************/
extern NOVA_Server_t g_ServerContext;

/**************************************************************************************************/
#define _________Nova_Private_Prototype_________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________Nova_Private_functions________________________________________________
/**************************************************************************************************/

/**
@param eEvent APKE_NET_STATUS_e
*/
static void nova_NetworkNoti( HINT32 eEvent, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	NOVA_MSG_t stMsg;

	HxSTD_MemSet(&stMsg,0x0,sizeof(NOVA_MSG_t));

	stMsg.message = MGS_NOVA_SYSTEM_NETWORK_STATUS_CHANGE;
	stMsg.param1 = eEvent;

	NOVA_MSG_Send(&stMsg);
}
static HERROR   nova_onFactoryDefault(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	NOVA_MSG_t stMsg;

	printf(HxANSI_COLOR_PURPLE("[%s:%d] (+)\n"), __FUNCTION__, __LINE__);

	HxSTD_MemSet(&stMsg,0x0,sizeof(NOVA_MSG_t));

	stMsg.message = MGS_NOVA_SYSTEM_FACTORY_DEFAULT;

	NOVA_MSG_Send(&stMsg);
	return ERR_OK;
}

static HERROR   nova_onSystemStateChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	NOVA_MSG_t stMsg;

	printf(HxANSI_COLOR_PURPLE("[%s:%d] (+)\n"), __FUNCTION__, __LINE__);

	HxSTD_MemSet(&stMsg,0x0,sizeof(NOVA_MSG_t));

	stMsg.message = MGS_NOVA_SYSTEM_POWER_STATUS_CHANGE;
	stMsg.param1 = HxOBJECT_INT(apArgv[0]);

	NOVA_MSG_Send(&stMsg);
	return ERR_OK;
}

/**************************************************************************************************/
#define _________Nova_Public_Functions__________________________________________________________
/**************************************************************************************************/
void NOVA_SYS_Init(void)
{
	HAPI_Init();

	APK_NETWORK_Init();
	APK_NETWORK_RegisterNotifier(nova_NetworkNoti);

	//   APK_SYSINFO_TUNER_Init();
	APK_SYSINFO_STB_Init();

	APK_OUTPUT_VIDEO_Init();
	APK_OUTPUT_AUDIO_Init();


	HAPI_ConnectSignal("signal:onFactoryDefault",NULL, nova_onFactoryDefault);
	HAPI_ConnectSignal("signal:onSystemStateChanged", NULL, nova_onSystemStateChanged);


	HAPI_SendReadySignal(HLIB_STD_GetPID());

	HxLOG_Debug("************** NOVA_SYSTEM_Init Done ******************\n");
}

void NOVA_SYS_Msg(NOVA_MSG_t *pstMsg)
{
	NOVA_MsgType_t msgtype;

	msgtype = pstMsg->message;
	HxLOG_Info("msgtype[0x%x] param1:%d\n",msgtype, pstMsg->param1);

	switch(msgtype)
	{
		case  MGS_NOVA_SYSTEM_POWER_STATUS_CHANGE:	
			HxLOG_Error(HxANSI_COLOR_PURPLE("[%s:%d] MGS_NOVA_SYSTEM_POWER_STATUS_CHANGE, event:%d \n"), __FUNCTION__, __LINE__, pstMsg->param1);

			if(eAPK_POWER_STATUS_ShutdownDone == pstMsg->param1 ) // == ePAMA_STATUS_ShutdownDone
			{
				if (NOVA_ServerShutdown() == false)
				{
					HxLOG_Error("NOVA_ServerShutdown failed\n");
				}
				else
				{
					HxLOG_Print("NOVA_ServerShutdown is called successfully!\n");
				}
			}

			break;

		case  MGS_NOVA_SYSTEM_NETWORK_STATUS_CHANGE:
			HxLOG_Error(HxANSI_COLOR_PURPLE("[%s:%d] MGS_NOVA_SYSTEM_NETWORK_STATUS_CHANGE, event:%d \n"), __FUNCTION__, __LINE__, pstMsg->param1);

			switch (pstMsg->param1) //Network event type
			{
				// 2015/12/01 mhkang: http://svn.humaxdigital.com:3000/issues/109083
				case eNETWORK_STATUS_CON_SUCCESS:
				case eNETWORK_STATUS_CON_AUTOIP_SUCCESS:
					g_ServerContext.isNetworkActive = true;

					if (NOVA_ServerRestart() == false)
					{
						HxLOG_Error("NOVA_ServerRestart failed\n");
					}
					break;

				case eNETWORK_STATUS_CON_FAIL:
				case eNETWORK_STATUS_CON_AUTOIP_FAIL:
					g_ServerContext.isNetworkActive = false;

					NOVA_ServerShutdown();

					break;

				case eNETWORK_STATUS_CABLE_LINK_DISCONNECTED:
					if (NOVA_IsActiveNetwork() == TRUE)
					{
						g_ServerContext.isNetworkActive = true;

						if (NOVA_ServerRestart() == false)
						{
							HxLOG_Error("NOVA_ServerRestart failed\n");
						}					
					}
					else
					{
						g_ServerContext.isNetworkActive = false;

						NOVA_ServerShutdown();
					}

					break;

				default:
					break;
			}

			break;

		case  MGS_NOVA_SYSTEM_FACTORY_DEFAULT:
			HxLOG_Error(HxANSI_COLOR_PURPLE("[%s:%d] MGS_NOVA_SYSTEM_FACTORY_DEFAULT \n"), __FUNCTION__, __LINE__);

			if(NOVA_ServerFactoryReset() == FALSE)
			{
				HxLOG_Error("NovaspreadServerFactoryReset() ERROR\n");
			}
			break;

		default:
			HxLOG_Warning("[NotImplement]msg:0x%x,p1:%x,p2:%x,p3:%x,p4:%x",	pstMsg->message,pstMsg->param1,pstMsg->param2,pstMsg->param3,pstMsg->param4);
			break;
	}
}


/**************************************************************************************************/
#define _________Nova__End_Of_File____________________________________________________
/**************************************************************************************************/


