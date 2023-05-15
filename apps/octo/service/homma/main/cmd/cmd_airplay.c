/**************************************************************
*	@file	cmd_airplay.c
*	http://www.humaxdigital.com
*	@author			humax
*	@brief			AIRPLAY
**************************************************************/

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

#define	________________Header_Files_______________________________
#include <vkernel.h>
#include <hlib.h>

#include "hom_int.h"
#include "homma_int.h"
#include "homma_bus.h"
#include "common.h"
#include "pl_airplay.h"
#include "airplay.h"

#define ________________Static_Variables___________________________
#define __MODULE__ "CMD:AIRPLAY"
#define	AIRPLAY_TEST_SERVICE_PORT (80)

#define ________________Internal_Functions_________________________

HBOOL gStop = FALSE;
#if defined(CONFIG_DEBUG)

static void run_time(void* args)
{

	PL_AIRPLAY_PLAYBACK_INFO_t *pbinfo = args;
	while(!gStop)
	{
		if(pbinfo->ePlaybackStatus == PL_AIRPLAY_PLAYBACK_PAUSE)
		{
			usleep(100000);
			continue;
		}
		if(pbinfo->ullPosition > pbinfo->ullDuration)
		{
			pbinfo->ePlaybackStatus = PL_AIRPLAY_PLAYBACK_REACHED_EOF;
		}
		pbinfo->ullPosition += 500;
		usleep(500000);
	}

}


static void __cmd_airplay_event(HINT32 p1, HINT32 p2, HINT64 p3)
{
	static int32_t task_id;
	static PL_AIRPLAY_PLAYBACK_INFO_t pbinfo;

	switch(p1)
	{
		case eAIRPLAY_OP_PLAY:
			{
				//PL_DMP_DmsInfo_t	*pDms = (PL_DMP_DmsInfo_t *)p2;
				HxLOG_Print("#################### AIRPLAY PLAY ##################### \n");
				HxLOG_Print("[ %s:%d ] Play request\n", __MODULE__, __LINE__);
				HxLOG_Print("[ %s:%d ] URL : %s\n", __MODULE__, __LINE__, (HCHAR*)p2);
				HxLOG_Print("[ %s:%d ] Start from : %lldms\n", __MODULE__, __LINE__, p3);

				gStop = FALSE;

				VK_TASK_Create(run_time, VK_TASK_PRIORITY_MW_REF, 512*1024, "runtime_task", &pbinfo,
										(long unsigned int*)&task_id, 0);
				VK_TASK_Start(task_id);

				pbinfo.ullDuration = 0xFFFF;
				pbinfo.ullPosition = 0;
				pbinfo.ePlaybackStatus = PL_AIRPLAY_PLAYBACK_PLAYING;
			}
			break;
		case eAIRPLAY_OP_SETPOSITION:
			{
				HxLOG_Print("#################### AIRPLAY SETPOSITION ##################### \n");
				HxLOG_Print("[ %s:%d ] SetPosition request\n", __MODULE__, __LINE__);
				HxLOG_Print("[ %s:%d ] Jump to : %lldms\n", __MODULE__, __LINE__, p3);

				pbinfo.ullPosition = p3;
			}
			break;
		case eAIRPLAY_OP_SETRATE:
			{
				HxLOG_Print("#################### AIRPLAY SETRATE ##################### \n");
				HxLOG_Print("[ %s:%d ] SetRate request\n", __MODULE__, __LINE__);
				HxLOG_Print("[ %s:%d ] Multiplier : x%lld\n", __MODULE__, __LINE__, p3);

				pbinfo.unRate = (HINT32)p3;
				if(p2 == 0)
				{
					pbinfo.ePlaybackStatus = PL_AIRPLAY_PLAYBACK_PAUSE;
				}
				else
				{
					pbinfo.ePlaybackStatus = PL_AIRPLAY_PLAYBACK_PLAYING;
				}
			}
			break;
		case eAIRPLAY_OP_STOP:
			{
				HxLOG_Print("#################### AIRPLAY STOP ##################### \n");
				HxLOG_Print("[ %s:%d ] Stop request\n", __MODULE__, __LINE__);

				gStop = TRUE;
				VK_TASK_Stop(task_id);
				VK_TASK_Destroy(task_id);
				task_id = 0;
				pbinfo.unRate = 100;
				pbinfo.ullPosition = 0;
				pbinfo.ullDuration = 0;
				pbinfo.ePlaybackStatus = PL_AIRPLAY_PLAYBACK_STOPPED;
			}
			break;
		case eAIRPLAY_OP_GET_PLAYBACK_INFO:
			{
				HxLOG_Print("#################### AIRPLAY PLAYBACK INFO ##################### \n");
				HxLOG_Print("[ %s:%d ] Playback info request\n", __MODULE__, __LINE__);

				pbinfo.ullBufferedRange = pbinfo.ullDuration;
				pbinfo.ullSeekableRange = pbinfo.ullDuration;
				AIRPLAY_SetPlaybackInfo(pbinfo);
			}
			break;
		default:
			break;
	}
}

HINT32   cmd_airplay_do_action(void *szArgList)
{
	HCHAR    *aucArg=NULL, aucStr[20];

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL ) 	return HxCMD_ERR;
	if( sscanf(aucArg, "%s", (char *)&aucStr) == 0 )   	return HxCMD_ERR;

	HLIB_STD_StrUpper(aucStr);

	if (VK_strcmp(aucStr, "START") == 0)
	{
		HERROR	hErr = ERR_FAIL;
		HOMMA_BUS_Data_t retData;
		PL_COMMON_MacInfo_t mac_info; /* Changed from PL_COMMON_NetworkInfo_t() */
		PL_AIRPLAY_SERVICE_PARAM_t tServiceParam;
		HxSTD_MemSet(&tServiceParam, 0x00, sizeof(PL_AIRPLAY_SERVICE_PARAM_t));
		HxSTD_MemSet(&retData, 0x00, sizeof(HOMMA_BUS_Data_t));
		HxSTD_MemSet(&mac_info, 0x00, sizeof(PL_COMMON_MacInfo_t));

		HLIB_CMD_Printf("airplay start\n");

		/* Get Ethernet information */
		hErr = PL_COMMON_GetDefaultMac(&mac_info); /* Caution to meaning of "default", not "ethernet" */
		if (hErr != ERR_OK)
		{
			HxLOG_Error("[%s:%d] Error> Cant get MAC info!\n", __MODULE__, __LINE__);
			/* What TODO? */
		}

		HOM_SNPRINTF(tServiceParam.aDeviceModel, 20, "HumaxTest(%c%c%c%c)", mac_info.szMacAddr[8]
																		 , mac_info.szMacAddr[9]
																		 , mac_info.szMacAddr[10]
																		 , mac_info.szMacAddr[11]
																		 );
		HOM_SNPRINTF(tServiceParam.aServiceName, 20, "(%c%c%c%c)HumaxTest", mac_info.szMacAddr[8]
																			 , mac_info.szMacAddr[9]
																			 , mac_info.szMacAddr[10]
																			 , mac_info.szMacAddr[11]
																			 );

		HOM_SNPRINTF(tServiceParam.szMacAddr, 20, "%c%c:%c%c:%c%c:%c%c:%c%c:%c%c)",
				mac_info.szMacAddr[0],
				mac_info.szMacAddr[1],
				mac_info.szMacAddr[2],
				mac_info.szMacAddr[3],
				mac_info.szMacAddr[4],
				mac_info.szMacAddr[5],
				mac_info.szMacAddr[6],
				mac_info.szMacAddr[7],
				mac_info.szMacAddr[8],
				mac_info.szMacAddr[9],
				mac_info.szMacAddr[10],
				mac_info.szMacAddr[11]);

		tServiceParam.nHttpServicePort = AIRPLAY_TEST_SERVICE_PORT;
		tServiceParam.nRtspServicePort = AIRPLAY_TEST_SERVICE_PORT+1;

		if(AIRPLAY_SetServiceParameter(&tServiceParam) == ERR_FAIL)
		{
			HxLOG_Error("[%s:%d] Can not set service parameter \n", __MODULE__, __LINE__);
			return HxCMD_ERR;
		}
		AIRPLAY_RegisterEventCallback(__cmd_airplay_event);
		if (AIRPLAY_Start() != ERR_OK)
		{
			HxLOG_Error("[%s:%d] Error> failed to start airplay!\n", __MODULE__, __LINE__);
			return HxCMD_ERR;
		}
	}
	else if(VK_strcmp(aucStr, "STOP") == 0)
	{
		HLIB_CMD_Printf("airplay stop\n");
		AIRPLAY_UnregisterEventCallback();
		AIRPLAY_Stop();
	}
	else if(VK_strcmp(aucStr, "INFO") == 0)
	{
		HINT32 retErr;
		HOMMA_BUS_Data_t retData;
		CO_NET_Info_t *pNetInfo = NULL;
		HxSTD_MemSet(&retData, 0x00, sizeof(HOMMA_BUS_Data_t));

		retErr = HOMMA_BUS_Request( eBUS_TARGET_SYS, eBUS_REQUEST_NET_INFO, &retData, &retErr );
		if ( ERR_OK != retErr )
		{
			HxLOG_Error("[%s:%d] network information request is faild. \n", __MODULE__, __LINE__);
			return ERR_FAIL;
		}
		if ( eBUS_DATATYPE_USER_DEF <= retData.eDataType )
		{
			pNetInfo = (CO_NET_Info_t *)retData.pvData;
			if ( 0 == pNetInfo->nDevCnt )
			{
				HxLOG_Error("[%s:%d] avaiable network interface is not existed. \n", __MODULE__, __LINE__);
			}
		}

		if(pNetInfo)
		{
			HxLOG_Debug("dev cnt : %d\n", pNetInfo->nDevCnt);
			HxLOG_Debug("def idx : %d\n", pNetInfo->nDefIdx);
			HxLOG_Debug("mac addr : %s\n", pNetInfo->pstIfaces[pNetInfo->nDefIdx].szMacAddr);
			HxLOG_Debug("connected : %d\n", pNetInfo->pstIfaces[pNetInfo->nDefIdx].bConnected);
			HxLOG_Debug("enabled : %d\n", pNetInfo->pstIfaces[pNetInfo->nDefIdx].bEnabled);
			HxLOG_Debug("IP : %s\n", pNetInfo->pstIfaces[pNetInfo->nDefIdx].szIpAddr);
		}

		//DMS_SetNetworkInfo( pNetInfo );
	}
	else
	{

	}

	return HxCMD_OK;
}
#endif

#define ________________Public_Interfaces__________________________

void	HOMMA_CMD_Register_AIRPLAY(void)
{
#if defined(CONFIG_DEBUG)
#define     hCmdHandle      "airplay"
	HLIB_CMD_RegisterWord( hCmdHandle, cmd_airplay_do_action, "do", "start | stop", "airplay do [action]");
#endif
}
#define _____END_OF_FILE_________
