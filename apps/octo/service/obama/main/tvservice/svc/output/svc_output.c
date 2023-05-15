/*
 * (c) 2011-2012 Humax Co., Ltd.
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
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/
/*******************************************************************
	File Description
********************************************************************/
/**
	@file	mw_output.c
	@brief	mw_output.c

	Description: video, audio 등의 output에 대한 정의 및 처리.		\n
	Module: MW /OUTPUT												\n

	Copyright (c) 2009 HUMAX Co., Ltd.								\n
	All rights reserved.											\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <rlib.h>

#include <get_enum_str.h>

#include <pal_audio.h>
#include <pal_video.h>
#include <pal_output.h>
#include <pal_hdmi.h>
#include <pal_sys.h>
#include <pal_param.h>
#include <pal_thumbnail.h>

#if defined(CONFIG_MW_OUTPUT_SCART)
#include <pal_scart.h>
#endif

#include <pal_scaler.h>
#include <pal_pipe.h>

#include <db_param.h>
#include <svc_output.h>



/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/




/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	CopyRight_t			eVideoBlankCopyRight;
	CopyRight_t			eScmsCopyRight;
	Macrovision_t		eVideoMacrovision;
	Macrovision_t		eCgmsMacrovision;
	HBOOL				bHdcpEnable;
} CpContext_t;


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

STATIC HULONG				s_ulHdmiMsgQId;
STATIC HULONG				s_ulHdmiTaskId;
STATIC HULONG				s_ulSvcOutSemId;

/* Copy protect setting of action */
STATIC CpContext_t			s_stCopyRightContext[NUM_VIEW_ACTION];

STATIC HdcpBlockMode_e 		s_eHdcpFailBlockMode; 			/* HDCP 기기에서 HDCP 설정 fail 의 경우 */
STATIC HdcpBlockMode_e 		s_eHdcpNoDeviceBlockMode;		/* HDCP 지원 기기가 아닐 경우 */

STATIC DxNflixDrmStatus_e   s_eNetflixDrmStatus;			/* Netflix의 DRM Status */

STATIC PostMsgToMgrCb_t	s_fnSvcOutputPostMsgToMgr;

#if defined(CONFIG_MW_SYS_THERMAL_PROTECTION)
STATIC HBOOL s_bOverTemp = FALSE;
STATIC HBOOL s_bFanLock = FALSE;
#endif

#define	ENTER_CRITICAL_SVCOUT		VK_SEM_Get(s_ulSvcOutSemId)
#define	LEAVE_CRITICAL_SVCOUT		VK_SEM_Release(s_ulSvcOutSemId)

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

STATIC HINT32 svc_out_PostMsgToAp(HINT32 msg, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3)
{

	if (s_fnSvcOutputPostMsgToMgr)
	{
		return (*s_fnSvcOutputPostMsgToMgr)(msg, hAct, p1, p2, p3);
	}

	return 0;
}

STATIC void svc_out_RegisterPostMsgToApCallBack(PostMsgToMgrCb_t pfncPostMsgCallback)
{

	if(pfncPostMsgCallback == NULL)
	{
		HxLOG_Critical("Critical Eror!!!\n");
	}

	s_fnSvcOutputPostMsgToMgr = pfncPostMsgCallback;
}





#ifdef CONFIG_MW_OUTPUT_HDMI

STATIC HERROR svc_out_CheckActionId(HUINT32 ulActionId)
{
	if(ulActionId < NUM_VIEW_ACTION)
	{
		return ERR_OK;
	}
	else
	{
		HxLOG_Error("**** Invalid action ID [%d]\n", ulActionId);
		return ERR_FAIL;
	}
}

STATIC void svc_out_HdmiEventTask (void)
{
	HERROR			hErr;
	PalHdmi_Event_e		eHdmiEvent;
	PalHdmi_Status_t	stHdmiStatus;
	HUINT32 		i;
	HUINT32	ulDecId;
	HUINT32			nResult = 0;

	while(1)
	{
		nResult = (HUINT32)VK_MSG_Receive(s_ulHdmiMsgQId, &eHdmiEvent, sizeof(PalHdmi_Event_e));
		if(nResult == ERR_OK)
		{
			switch(eHdmiEvent)
			{
				case eHDMI_EVENT_STATUS_CHANGED:
					HxLOG_Debug("\t%s(eHDMI_EVENT_STATUS_CHANGED)\n", __FUNCTION__);

					PAL_HDMI_GetStatus(&stHdmiStatus);

					if(stHdmiStatus.bPlug == TRUE)
					{
						/* 해상도 변경 가능성을 체크해야 한다. */
						PAL_SCALER_SyncVideoSetting();
						PAL_OUT_CheckDigitalAudioOut();
					}

					svc_out_PostMsgToAp(eSEVT_OUT_CHECK_HDMI, HANDLE_NULL, stHdmiStatus.bPlug, 0, 0);

					break;

				case eHDMI_EVENT_HDCP_CHANGED:
					HxLOG_Debug("\t%s(eHDMI_EVENT_HDCP_CHANGED)\n", __FUNCTION__);

					PAL_HDMI_GetStatus(&stHdmiStatus);

					HxLOG_Debug("\t\tHDCP status is [%s]\n", OTL_ENUM2STR_HdcpStatus(stHdmiStatus.eHdcpStatus));
					switch(stHdmiStatus.eHdcpStatus)
					{
						case eDxHDCPSTATUS_OFF:
						case eDxHDCPSTATUS_OK:
							{
							PAL_HDMI_SetBlank(FALSE);
							PAL_SCALER_SetResolutionLimit(FALSE);

							for(i = 0; i < NUM_VIEW_ACTION; i++)
							{
								PAL_VIDEO_SetHide(i, eAvHideClient_Hdcp, FALSE);
							}

							PAL_OUT_SetHdcpAudioMute(FALSE);
							}
							break;

						case eDxHDCPSTATUS_FAIL:

							HxLOG_Debug("\t\tHDCP block mode is [%s]\n", OTL_ENUM2STR_HdcpBlockMode(s_eHdcpFailBlockMode));
						  	if(s_eNetflixDrmStatus != eNFLIX_DRM_INIT)
							{
								switch(s_eHdcpFailBlockMode)
								{
									case eHDCP_BLOCK_HDMIBLANK:
									case eHDCP_BLOCK_DOWNSCALE:
										PAL_HDMI_SetBlank(TRUE);
										break;

									case eHDCP_BLOCK_AVMUTE:
									{
										for(i = 0; i < NUM_VIEW_ACTION; i++)
										{
											if(s_stCopyRightContext[i].bHdcpEnable == TRUE)
											{
												hErr = PAL_PIPE_GetResourceId(i, eRxRSCTYPE_AV_DEC, &ulDecId);
												if(hErr == ERR_OK)
												{
													PAL_VIDEO_SetHide(ulDecId, eAvHideClient_Hdcp, TRUE);
													PAL_OUT_SetHdcpAudioMute(TRUE);
												}
											}

										}
										break;
									}

									default:
										break;
								}
							}
							else
							{
								PAL_HDMI_SetBlank(FALSE);
								PAL_SCALER_SetResolutionLimit(FALSE);

								for(i = 0; i < NUM_VIEW_ACTION; i++)
								{
									PAL_VIDEO_SetHide(i, eAvHideClient_Hdcp, FALSE);
								}

								PAL_OUT_SetHdcpAudioMute(FALSE);
							}
							break;

						case eDxHDCPSTATUS_NODEVICE:

							HxLOG_Debug("\t\tHDCP block mode is [%s]\n", OTL_ENUM2STR_HdcpBlockMode(s_eHdcpFailBlockMode));
							if(s_eNetflixDrmStatus != eNFLIX_DRM_INIT)
							{
								switch(s_eHdcpNoDeviceBlockMode)
								{
									case eHDCP_BLOCK_HDMIBLANK:
										PAL_HDMI_SetBlank(TRUE);
										break;

									case eHDCP_BLOCK_DOWNSCALE:
										PAL_SCALER_SetResolutionLimit(TRUE);
										break;

									case eHDCP_BLOCK_AVMUTE:
									{
										for(i = 0; i < NUM_VIEW_ACTION; i++)
										{
											if(s_stCopyRightContext[i].bHdcpEnable == TRUE)
											{
												hErr = PAL_PIPE_GetResourceId(i, eRxRSCTYPE_AV_DEC, &ulDecId);
												if(hErr == ERR_OK)
												{
													PAL_VIDEO_SetHide(ulDecId, eAvHideClient_Hdcp, TRUE);
													PAL_OUT_SetHdcpAudioMute(TRUE);
												}
											}

										}

										break;
									}

									default:
										break;
								}
							}
							else
							{
								PAL_HDMI_SetBlank(FALSE);
								PAL_SCALER_SetResolutionLimit(FALSE);

								for(i = 0; i < NUM_VIEW_ACTION; i++)
								{
									PAL_VIDEO_SetHide(i, eAvHideClient_Hdcp, FALSE);
								}

								PAL_OUT_SetHdcpAudioMute(FALSE);
							}
							break;

						default:
							break;
					}

					svc_out_PostMsgToAp(eSEVT_OUT_CHECK_HDCP, HANDLE_NULL, stHdmiStatus.eHdcpStatus, 0, 0);

					break;

				default:
					HxLOG_Error("Unknown HDMI event : 0x%X)\n", eHdmiEvent);
					break;
			}
		}
	}

	return;
}

STATIC void svc_out_HdmiEvtCbFunc(PalHdmi_Event_e eHdmiEvent)
{
	HUINT32			nResult = 0;

	nResult = (HUINT32)VK_MSG_SendTimeout(s_ulHdmiMsgQId, &eHdmiEvent, sizeof(PalHdmi_Event_e), 0);
	if(nResult != ERR_OK)
	{
		HxLOG_Error("\n");
		return;
	}

	return;
}
#endif // CONFIG_MW_OUTPUT_HDMI

#if defined(CONFIG_MW_OUTPUT_SCART)
STATIC void svc_out_ScartEvtCbFunc(ScartEvent_t eEvent, HUINT32 ulScartId, HUINT32 ulParam1)
{
	HxLOG_Error("eEvent (%d), Scart (%d) param (%d) \n", eEvent, ulScartId, ulParam1);

	switch (eEvent)
	{
	case eSCART_EVENT_VCR_ASPECTRATIO:
	case eSCART_EVENT_VCR_POWER:
		svc_out_PostMsgToAp (eSEVT_OUT_CHECK_SCART, HANDLE_NULL, (HINT32)eEvent, (HINT32)ulParam1, (HINT32)ulScartId);
		break;

	default:
		HxLOG_Error("\n");
		break;
	}

	return;
}
#endif

#if defined(CONFIG_MW_SYS_THERMAL_PROTECTION)
STATIC void svc_out_cbOverTempCbFunc(void)
{
	HxLOG_Error("[%s : %d] send eSEVT_OUT_OVERTEMPERATURE \n", __FUNCTION__,__LINE__);
	s_bOverTemp = TRUE;
	svc_out_PostMsgToAp (eSEVT_OUT_OVERTEMPERATURE, HANDLE_NULL, (HINT32)0, (HINT32)0, (HINT32)0);
}

STATIC void svc_out_cbFanLockCbFunc(void)
{
	HxLOG_Error("[%s : %d] send eSEVT_OUT_FANLOCK \n", __FUNCTION__,__LINE__);
	s_bFanLock = TRUE;
	svc_out_PostMsgToAp (eSEVT_OUT_FANLOCK, HANDLE_NULL, (HINT32)0, (HINT32)0, (HINT32)0);
}
#endif

#if defined(CONFIG_DEBUG)
STATIC void svc_out_Ping(void)
{
	HLIB_CMD_Printf("Ping SVC_OUT API semaphore ...\n");

	ENTER_CRITICAL_SVCOUT;

	HLIB_CMD_Printf("\t\ts_ulSvcOutSemId alive ...\n");

	LEAVE_CRITICAL_SVCOUT;
}

STATIC void svc_out_PrintStatus(void)
{
	HUINT32			i;

	HLIB_CMD_Printf("===================== SVC_OUT status ============================ \n");

	HLIB_CMD_Printf("\t\teHdcpFailBlock\t\t\t: %s\n", OTL_ENUM2STR_HdcpBlockMode(s_eHdcpFailBlockMode));
	HLIB_CMD_Printf("\t\teHdcpNoDeviceBlock\t\t\t: %s\n", OTL_ENUM2STR_HdcpBlockMode(s_eHdcpNoDeviceBlockMode));

	for(i = 0; i < NUM_VIEW_ACTION; i++)
	{
		HLIB_CMD_Printf("\t\t-------- Copy Protection Context[%d] --------\n", i);
		HLIB_CMD_Printf("\t\t\teVideoBlankCopyRight\t\t\t: %s \n", OTL_ENUM2STR_CopyRight(s_stCopyRightContext[i].eVideoBlankCopyRight));
		HLIB_CMD_Printf("\t\t\teScmsCopyRight\t\t\t: %s \n", OTL_ENUM2STR_CopyRight(s_stCopyRightContext[i].eScmsCopyRight));
		HLIB_CMD_Printf("\t\t\teVideoMacrovision\t\t\t: %s \n", OTL_ENUM2STR_Macrovision(s_stCopyRightContext[i].eVideoMacrovision));
		HLIB_CMD_Printf("\t\t\teCgmsMacrovision\t\t\t: %s \n", OTL_ENUM2STR_Macrovision(s_stCopyRightContext[i].eCgmsMacrovision));
		HLIB_CMD_Printf("\t\t\tbHdcpEnable\t\t\t: %d \n", s_stCopyRightContext[i].bHdcpEnable);
		HLIB_CMD_Printf("\t\t---------------------------------------------\n", i);
	}
	HLIB_CMD_Printf("===================== end of output status ============================ \n");

	return;
}

STATIC int svc_out_ProcessCommand_cgmsa(void *szArgList)
{
	char			*aucArg=NULL;
	char			aucStr[20];
	HUINT32			ulActionId;

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg != NULL )
	{
		if( sscanf(aucArg, "%d", &ulActionId) != 0 )
		{
			aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
			if( aucArg != NULL )
			{
				if( sscanf(aucArg, "%s", aucStr) != 0 )
				{
					if(strcmp(aucStr, "free") == 0)
					{
						SVC_OUT_SetVideoBlankCopyRight(ulActionId, eCOPYRIGHT_COPY_FREE);
					}
					else if(strcmp(aucStr, "once") == 0)
					{
						SVC_OUT_SetVideoBlankCopyRight(ulActionId, eCOPYRIGHT_COPY_ONCE);
					}
					else if(strcmp(aucStr, "nomore") == 0)
					{
						SVC_OUT_SetVideoBlankCopyRight(ulActionId, eCOPYRIGHT_COPY_NOMORE);
					}
					else if(strcmp(aucStr, "never") == 0)
					{
						SVC_OUT_SetVideoBlankCopyRight(ulActionId, eCOPYRIGHT_COPY_NEVER);
					}
				}
			}
		}
		else
		{
			return HxCMD_ERR;
		}
	}
	else
	{
		return HxCMD_ERR;
	}

	return HxCMD_OK;
}

STATIC int svc_out_ProcessCommand_scms(void *szArgList)
{
	char			*aucArg=NULL;
	char			aucStr[20];
	HUINT32			ulActionId;

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg != NULL )
	{
		if( sscanf(aucArg, "%d", &ulActionId) != 0 )
		{
			aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
			if( aucArg != NULL )
			{
				if( sscanf(aucArg, "%s", aucStr) != 0 )
				{
					if(strcmp(aucStr, "free") == 0)
					{
						SVC_OUT_SetScmsCopyRight(ulActionId, eCOPYRIGHT_COPY_FREE);
					}
					else if(strcmp(aucStr, "once") == 0)
					{
						SVC_OUT_SetScmsCopyRight(ulActionId, eCOPYRIGHT_COPY_ONCE);
					}
					else if(strcmp(aucStr, "nomore") == 0)
					{
						SVC_OUT_SetScmsCopyRight(ulActionId, eCOPYRIGHT_COPY_NOMORE);
					}
					else if(strcmp(aucStr, "never") == 0)
					{
						SVC_OUT_SetScmsCopyRight(ulActionId, eCOPYRIGHT_COPY_NEVER);
					}
				}
			}
		}
		else
		{
			return HxCMD_ERR;
		}
	}
	else
	{
		return HxCMD_ERR;
	}

	return HxCMD_OK;
}

STATIC int svc_out_ProcessCommand_macrovision(void *szArgList)
{
	char			*aucArg=NULL;
	char			aucStr[20];
	HUINT32			ulActionId;

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg != NULL )
	{
		if( sscanf(aucArg, "%d", &ulActionId) != 0 )
		{
			aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
			if( aucArg != NULL )
			{
				if( sscanf(aucArg, "%s", aucStr) != 0 )
				{
					if(strcmp(aucStr, "off") == 0)
					{
						SVC_OUT_SetMacrovision(ulActionId, eMACROVISION_OFF);
					}
					else if(strcmp(aucStr, "agc") == 0)
					{
						SVC_OUT_SetMacrovision(ulActionId, eMACROVISION_AGC);
					}
					else if(strcmp(aucStr, "2line") == 0)
					{
						SVC_OUT_SetMacrovision(ulActionId, eMACROVISION_2LINE);
					}
					else if(strcmp(aucStr, "4line") == 0)
					{
						SVC_OUT_SetMacrovision(ulActionId, eMACROVISION_4LINE);
					}
				}
			}
		}
	}

	return HxCMD_OK;
}

STATIC int svc_out_ProcessCommand_cgmsmacrovision(void *szArgList)
{
	char			*aucArg=NULL;
	char			aucStr[20];
	HUINT32			ulActionId;

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg != NULL )
	{
		if( sscanf(aucArg, "%d", &ulActionId) != 0 )
		{
			aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
			if( aucArg != NULL )
			{
				if( sscanf(aucArg, "%s", aucStr) != 0 )
				{
					if(strcmp(aucStr, "off") == 0)
					{
						SVC_OUT_SetCgmsMacrovision(ulActionId, eMACROVISION_OFF);
					}
					else if(strcmp(aucStr, "agc") == 0)
					{
						SVC_OUT_SetCgmsMacrovision(ulActionId, eMACROVISION_AGC);
					}
					else if(strcmp(aucStr, "2line") == 0)
					{
						SVC_OUT_SetCgmsMacrovision(ulActionId, eMACROVISION_2LINE);
					}
					else if(strcmp(aucStr, "4line") == 0)
					{
						SVC_OUT_SetCgmsMacrovision(ulActionId, eMACROVISION_4LINE);
					}
				}
			}
		}
	}

	return HxCMD_OK;
}

STATIC int svc_out_ProcessCommand_hdcp(void *szArgList)
{
	char			*aucArg=NULL;
	char			aucStr[20];
	HUINT32			ulActionId;

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg != NULL )
	{
		if( sscanf(aucArg, "%d", &ulActionId) != 0 )
		{
			aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
			if( aucArg != NULL )
			{
				if( sscanf(aucArg, "%s", aucStr) != 0 )
				{
					if(strcmp(aucStr, "on") == 0)
					{
						SVC_OUT_SetHdcp(ulActionId, TRUE);
					}
					else if(strcmp(aucStr, "off") == 0)
					{
						SVC_OUT_SetHdcp(ulActionId, FALSE);
					}
				}
			}
		}
	}

	return HxCMD_OK;
}

STATIC int svc_out_ProcessCommand_ping(void *szArgList)
{
	svc_out_Ping();

	return HxCMD_OK;
}

STATIC int svc_out_ProcessCommand_status(void *szArgList)
{
	svc_out_PrintStatus();

	return HxCMD_OK;
}

STATIC void svc_out_RegisterCommand(void)
{
#define	hCmdHandle		"svc_output"

	HLIB_CMD_RegisterWord(hCmdHandle,
						svc_out_ProcessCommand_cgmsa,
						"cgmsa",
						"Set video blank CGMS_A copy right",
						"cgmsa [action id] [free|once|nomore|never]");

	HLIB_CMD_RegisterWord(hCmdHandle,
						svc_out_ProcessCommand_cgmsa,
						"cgmsa",
						"Set video blank CGMS_A copy right",
						"cgmsa [action id] [free|once|nomore|never]");

	HLIB_CMD_RegisterWord(hCmdHandle,
						svc_out_ProcessCommand_scms,
						"scma",
						"Set digital audio copy right",
						"scms [action id] [free|once|nomore|never]");

	HLIB_CMD_RegisterWord(hCmdHandle,
						svc_out_ProcessCommand_macrovision,
						"macrovision",
						"Set video macrovision.",
						"macrovision [action id] [off|agc|2line|4line]");

	HLIB_CMD_RegisterWord(hCmdHandle,
						svc_out_ProcessCommand_cgmsmacrovision,
						"cgmsmacrovision",
						"Set CGMS macrovision",
						"cgmsmacrovision [action id] [off|agc|2line|4line]");

	HLIB_CMD_RegisterWord(hCmdHandle,
						svc_out_ProcessCommand_hdcp,
						"hdcp",
						"Set HDCP copy protection",
						"hdcp [action id] [default|on|off]");

	HLIB_CMD_RegisterWord(hCmdHandle,
						svc_out_ProcessCommand_ping,
						"ping",
						"Probe if semaphore is alive.",
						"ping");

	HLIB_CMD_RegisterWord(hCmdHandle,
						svc_out_ProcessCommand_status,
						"status",
						"Print svc_output status",
						"status");
}
#endif
HERROR SVC_OUT_Init(PostMsgToMgrCb_t pfncPostMsgCallback)
{
	svc_out_RegisterPostMsgToApCallBack(pfncPostMsgCallback);

#ifdef CONFIG_MW_OUTPUT_HDMI
	{
		HUINT32 nResult = 0;

		nResult = VK_SEM_Create(&s_ulSvcOutSemId, "SvcOut", VK_SUSPENDTYPE_PRIORITY);
		if(nResult != ERR_OK)
		{
			HxLOG_Error("ERROR VK_SEM_Create ret:0x%X\n",nResult);
		}

		/* task for hdmi & hdcp process */
		nResult = (HUINT32)VK_MSG_Create(MW_CAS_MSGQ_SIZE, sizeof(PalHdmi_Event_e), "MwOutHdmiQ", &s_ulHdmiMsgQId, VK_SUSPENDTYPE_PRIORITY);
		if (nResult != ERR_OK)
		{
			HxLOG_Error(" \n");
			return ERR_FAIL;
		}

		nResult = (HUINT32)VK_TASK_Create((void*)svc_out_HdmiEventTask, MW_CAS_TASK_PRIORITY, MW_CAS_TASK_STACK_SIZE, "MwOutHdmiT", NULL, &s_ulHdmiTaskId, 0);
		if (nResult != ERR_OK)
		{
			HxLOG_Error(" \n");
			return ERR_FAIL;
		}

		nResult = (HUINT32)VK_TASK_Start(s_ulHdmiTaskId);
		if (nResult != ERR_OK)
		{
			HxLOG_Error(" \n");
			return ERR_FAIL;
		}

		PAL_HDMI_RegisterCallback(svc_out_HdmiEvtCbFunc);
	}
#endif	/* CONFIG_MW_OUTPUT_HDMI */

#if defined(CONFIG_MW_OUTPUT_SCART)
	PAL_SCART_RegisterCallback(svc_out_ScartEvtCbFunc);
#endif

#if defined(CONFIG_MW_SYS_THERMAL_PROTECTION)
	PAL_SYS_RegistOverTemperatureCallback(svc_out_cbOverTempCbFunc);
	PAL_SYS_RegistFanLockCallback(svc_out_cbFanLockCbFunc);
#endif

#if defined(CONFIG_DEBUG)
	svc_out_RegisterCommand();
#endif
	return ERR_OK;
}

HERROR SVC_OUT_SetInitParameters(
							HBOOL						bVbiTeletextEnable,
							VideoBlankSpec_e			eVideoBlankSpec,
							DxCompositeStandard_e		eCompositeStandard,
							DxDfcSelect_e 				eDfc,
							DxResolutionSelect_e		eResolutionSelect,
							DxAspectRatio_e 			eTvAspectRatio,
							DxHdmi3DSelect_e			e3DMode,
							HBOOL						bUseEdidAspectRatio,
							HBOOL						bWideSignaloLetterBox,
							HBOOL						bEnableHdmiAfd,
							HdcpBlockMode_e				eHdcpFailBlockMode,
							HdcpBlockMode_e				eHdcpNoDeviceBlockMode,
							DxScartFormat_e				eTvScartFormat,
							DxScartFormat_e				eVcrScartFormat,
							DxDigitalAudioFormat_e		eHdmiAudioOutFormat,
							DxDigitalAudioFormat_e		eSpdifAudioOutFormat
							)
{
	HxLOG_Debug("%s()\n", __FUNCTION__);
	HxLOG_Debug("\tbVbiTeletextEnable : %d\n", bVbiTeletextEnable);
	HxLOG_Debug("\teVideoBlankSpec : %s\n", OTL_ENUM2STR_VideoBlankSpec(eVideoBlankSpec));
	HxLOG_Debug("\teCompositeStandard : %s\n", OTL_ENUM2STR_CompositeStandard(eCompositeStandard));
	HxLOG_Debug("\teDfc : %s\n", OTL_ENUM2STR_DfcSelect(eDfc));
	HxLOG_Debug("\teResolutionSelect : %s\n", OTL_ENUM2STR_ResolutionSelect(eResolutionSelect));
	HxLOG_Debug("\teTvAspectRatio : %s\n", OTL_ENUM2STR_AspectRatio(eTvAspectRatio));
	HxLOG_Debug("\te3DMode : %s\n", OTL_ENUM2STR_Video3DSelect(e3DMode));
	HxLOG_Debug("\tbUseEdidAspectRatio : %d\n", bUseEdidAspectRatio);
	HxLOG_Debug("\tbWideSignaloLetterBox : %d\n", bWideSignaloLetterBox);
	HxLOG_Debug("\tbEnableHdmiAfd : %d\n", bEnableHdmiAfd);
	HxLOG_Debug("\teHdcpFailBlockMode : %s\n", OTL_ENUM2STR_HdcpBlockMode(eHdcpFailBlockMode));
	HxLOG_Debug("\teHdcpNoDeviceBlockMode : %s\n", OTL_ENUM2STR_HdcpBlockMode(eHdcpNoDeviceBlockMode));
	HxLOG_Debug("\teTvScartFormat : %s\n", OTL_ENUM2STR_ScartFormat(eTvScartFormat));
	HxLOG_Debug("\teVcrScartFormat : %s\n", OTL_ENUM2STR_ScartFormat(eVcrScartFormat));
	HxLOG_Debug("\teHdmiAudioOutFormat : %s\n", OTL_ENUM2STR_DigitalAudioOutput(eHdmiAudioOutFormat));
	HxLOG_Debug("\teSpdifAudioOutFormat : %s\n", OTL_ENUM2STR_DigitalAudioOutput(eSpdifAudioOutFormat));

	PAL_OUT_SetInitParameters(bVbiTeletextEnable,
							eVideoBlankSpec,
							eHdmiAudioOutFormat,
							eSpdifAudioOutFormat
							);

	PAL_SCALER_SetInitParameters(eCompositeStandard,
							eDfc,
							eResolutionSelect,
							eTvAspectRatio,
							e3DMode,
							bUseEdidAspectRatio,
							bWideSignaloLetterBox,
							bEnableHdmiAfd
							);

	s_eHdcpFailBlockMode = eHdcpFailBlockMode;
	s_eHdcpNoDeviceBlockMode = eHdcpNoDeviceBlockMode;

#if defined(CONFIG_MW_OUTPUT_SCART)
	PAL_SCART_SetTvFormat(eTvScartFormat);
	PAL_SCART_SetVcrFormat(eVcrScartFormat);
#endif

	return ERR_OK;
}

HERROR SVC_OUT_SetAvEnable(HBOOL bEnable)
{
	return PAL_OUT_SetAvEnable(bEnable);
}

HERROR SVC_OUT_GetAvEnable(HBOOL *pbEnable)
{
	return PAL_OUT_GetAvEnable(pbEnable);
}

HERROR SVC_OUT_GetHdmiPlugStatus(HBOOL *pbPlugIn)
{
	PalHdmi_Status_t stHdmiStatus;

	PAL_HDMI_GetStatus(&stHdmiStatus);
	*pbPlugIn = stHdmiStatus.bPlug;

	return ERR_OK;
}

HERROR SVC_OUT_SetHighPriorityHD(HBOOL bEnable)
{
	return PAL_OUT_SetHighPriorityHD(bEnable);
}

void SVC_OUT_CheckPriorityOutput(void)
{
	HERROR		hErr;
	HBOOL		bEnable = FALSE;

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_PRIORITY_HDMIOUTPUT, (HUINT32 *)&bEnable, 0);
	if (ERR_OK == hErr)
	{
		SVC_OUT_SetHighPriorityHD(bEnable);
	}
}


#define _____COPYRIGHT_API______________________________________________________________________________________________________________________________

HERROR SVC_OUT_SetVideoBlankCopyRight(HUINT32 ulActionId, CopyRight_t eCopyRight)
{
	HERROR						hErr = ERR_OK;
	CopyRight_t					eFinalCopyRight = eCOPYRIGHT_COPY_FREE;
	HUINT32						i;

	ENTER_CRITICAL_SVCOUT;

	HxLOG_Debug("%s(%d, %s)\n", __FUNCTION__, ulActionId, OTL_ENUM2STR_CopyRight(eCopyRight));

	if(svc_out_CheckActionId(ulActionId) != ERR_OK)
	{
		hErr = ERR_FAIL;
		goto exit;
	}
	else if(eCopyRight == s_stCopyRightContext[ulActionId].eVideoBlankCopyRight)
	{
		HxLOG_Debug("\tSame context ... Exit ...\n");

		hErr = ERR_OK;
		goto exit;
	}
	else
	{
		s_stCopyRightContext[ulActionId].eVideoBlankCopyRight = eCopyRight;
	}

	for(i = 0; i < NUM_VIEW_ACTION; i++)
	{
		if(s_stCopyRightContext[i].eVideoBlankCopyRight > eFinalCopyRight)
		{ /* 더 강력한 copy right를 선택 */
			HxLOG_Debug("\tContext[%d] has higher copy right[%s]\n"
							, i
							, OTL_ENUM2STR_CopyRight(s_stCopyRightContext[i].eVideoBlankCopyRight)
							);
			eFinalCopyRight = s_stCopyRightContext[i].eVideoBlankCopyRight;
		}
	}

	hErr = PAL_OUT_SetVideoBlankCopyRight(eFinalCopyRight);

exit:
	LEAVE_CRITICAL_SVCOUT;

	return hErr;
}

HERROR SVC_OUT_SetMacrovision(HUINT32 ulActionId, Macrovision_t eMacrovision)
{
	HERROR				hErr = ERR_OK;
	Macrovision_t		eFinalMacrovision = eMACROVISION_OFF;
	HUINT32				i;

	ENTER_CRITICAL_SVCOUT;

	HxLOG_Debug("%s(%d, %s)\n", __FUNCTION__, ulActionId, OTL_ENUM2STR_Macrovision(eMacrovision));

	if(svc_out_CheckActionId(ulActionId) != ERR_OK)
	{
		hErr = ERR_FAIL;
		goto exit;
	}
	else if(eMacrovision == s_stCopyRightContext[ulActionId].eVideoMacrovision)
	{
		HxLOG_Debug("\tSame context ... Exit ...\n");

		hErr = ERR_OK;
		goto exit;
	}
	else
	{
		s_stCopyRightContext[ulActionId].eVideoMacrovision = eMacrovision;
	}

	for(i = 0; i < NUM_VIEW_ACTION; i++)
	{
		if(s_stCopyRightContext[i].eVideoMacrovision != eMACROVISION_OFF)
		{
			eFinalMacrovision = s_stCopyRightContext[i].eVideoMacrovision;
			HxLOG_Debug("\tSelect context[%d]'s %s)\n", ulActionId, OTL_ENUM2STR_Macrovision(eFinalMacrovision));
			break;
		}
	}

	if(i == NUM_VIEW_ACTION)
	{
		HxLOG_Debug("\tFine video macrovision is eMACROVISION_OFF\n");
	}

	hErr = PAL_OUT_SetMacrovision(eFinalMacrovision);

exit:
	LEAVE_CRITICAL_SVCOUT;

	return hErr;
}

HERROR SVC_OUT_SetCgmsMacrovision(HUINT32 ulActionId, Macrovision_t eMacrovision)
{
	HERROR						hErr = ERR_OK;
	Macrovision_t				eFinalMacrovision = eMACROVISION_OFF;
	HUINT32						i;

	ENTER_CRITICAL_SVCOUT;

	HxLOG_Debug("%s(%d, %s)\n", __FUNCTION__, ulActionId, OTL_ENUM2STR_Macrovision(eMacrovision));

	if(svc_out_CheckActionId(ulActionId) != ERR_OK)
	{
		hErr = ERR_FAIL;
		goto exit;
	}
	else if(eMacrovision == s_stCopyRightContext[ulActionId].eCgmsMacrovision)
	{
		HxLOG_Debug("\tSame context ... Exit ...\n");

		hErr = ERR_OK;
		goto exit;
	}
	else
	{
		s_stCopyRightContext[ulActionId].eCgmsMacrovision = eMacrovision;
	}

	for(i = 0; i < NUM_VIEW_ACTION; i++)
	{
		if(s_stCopyRightContext[i].eCgmsMacrovision != eMACROVISION_OFF)
		{
			eFinalMacrovision = s_stCopyRightContext[i].eCgmsMacrovision;
			HxLOG_Debug("\tSelect context[%d]'s %s)\n", ulActionId, OTL_ENUM2STR_Macrovision(eFinalMacrovision));
			break;
		}
	}

	if(i == NUM_VIEW_ACTION)
	{
		HxLOG_Debug("\tFinal CGMS macrovision is eMACROVISION_OFF\n");
	}

	hErr = PAL_OUT_SetCgmsMacrovision(eFinalMacrovision);

exit:
	LEAVE_CRITICAL_SVCOUT;

	return hErr;
}

HERROR SVC_OUT_SetScmsCopyRight(HUINT32 ulActionId, CopyRight_t eCopyRight)
{
	HERROR						hErr = ERR_OK;
	CopyRight_t					eFinalCopyRight = eCOPYRIGHT_COPY_FREE;
	HUINT32						i;

	ENTER_CRITICAL_SVCOUT;

	HxLOG_Debug("%s(%d, %s)\n", __FUNCTION__, ulActionId, OTL_ENUM2STR_CopyRight(eCopyRight));

	if(svc_out_CheckActionId(ulActionId) != ERR_OK)
	{
		hErr = ERR_FAIL;
		goto exit;
	}
	else if(eCopyRight == s_stCopyRightContext[ulActionId].eScmsCopyRight)
	{
		HxLOG_Debug("\tSame context ... Exit ...\n");

		hErr = ERR_OK;
		goto exit;
	}
	else
	{
		s_stCopyRightContext[ulActionId].eScmsCopyRight = eCopyRight;
	}

	for(i = 0; i < NUM_VIEW_ACTION; i++)
	{
		if(s_stCopyRightContext[i].eScmsCopyRight > eFinalCopyRight)
		{ /* 더 강력한 copy right를 선택 */
			HxLOG_Debug("\tContext[%d] has higher copy right[%s]\n"
							, i
							, OTL_ENUM2STR_CopyRight(s_stCopyRightContext[i].eScmsCopyRight)
							);
			eFinalCopyRight = s_stCopyRightContext[i].eScmsCopyRight;
		}
	}

	hErr = PAL_OUT_SetScmsCopyRight(eFinalCopyRight);

exit:
	LEAVE_CRITICAL_SVCOUT;

	return hErr;
}

HERROR SVC_OUT_GetHdcp_Version(DxHdcpVersion_e *pVersion)
{
	return PAL_HDMI_GetHdcpVersion(pVersion);
}

HERROR SVC_OUT_GetHdcp(HBOOL *pbEnable)
{
	PAL_HDMI_GetHdcp(pbEnable);
	return ERR_OK;
}

HERROR SVC_OUT_SetHdcp(HUINT32 ulActionId, HBOOL bEnable)
{
	HERROR						hErr = ERR_OK;
	HBOOL						bActualHdcp;
	HUINT32 					i;

	ENTER_CRITICAL_SVCOUT;

	HxLOG_Debug("%s(%d, %d)\n", __FUNCTION__, ulActionId, bEnable);

	if(svc_out_CheckActionId(ulActionId) != ERR_OK)
	{
		hErr = ERR_FAIL;
		goto exit;
	}
	else
	{
		s_stCopyRightContext[ulActionId].bHdcpEnable = bEnable;
	}

	bActualHdcp = FALSE;
	for(i = 0; i < NUM_VIEW_ACTION; i++)
	{
		if(s_stCopyRightContext[i].bHdcpEnable == TRUE)
		{ /* 더 강력한 HDCP를 선택 */
			HxLOG_Debug("\tContext[%d] has HDCP enabled\n", i);
			bActualHdcp = TRUE;
			break;
		}
	}

	hErr = PAL_HDMI_SetHdcp(bActualHdcp);

exit:
	LEAVE_CRITICAL_SVCOUT;

	return hErr;
}

HERROR SVC_OUT_GetHdcpStatus(DxHdcpStatus_e *peHdcpStatus)
{
	PalHdmi_Status_t	stHdmiStatus;

	PAL_HDMI_GetStatus(&stHdmiStatus);
	*peHdcpStatus = stHdmiStatus.eHdcpStatus;

	return ERR_OK;
}

HERROR SVC_OUT_GetNetflixDrmStatus(DxNflixDrmStatus_e *peNflixDrmStaus)
{
    *peNflixDrmStaus = s_eNetflixDrmStatus;
    return ERR_OK;
}

HERROR SVC_OUT_SetNetflixDrmStatus(HUINT32 ulActionId, DxNflixDrmStatus_e eNflixDrmStaus)
{
	HERROR			    hErr;
	PalHdmi_Status_t	stHdmiStatus;
	HUINT32 		    i;
	HUINT32	            ulDecId;

	s_eNetflixDrmStatus = eNflixDrmStaus;

	PAL_HDMI_GetStatus(&stHdmiStatus);

	HxLOG_Debug("\t\tHDCP status is [%s]\n", OTL_ENUM2STR_HdcpStatus(stHdmiStatus.eHdcpStatus));
	switch(stHdmiStatus.eHdcpStatus)
	{
		case eDxHDCPSTATUS_OFF:
		case eDxHDCPSTATUS_OK:
			{
				PAL_HDMI_SetBlank(FALSE);
				PAL_SCALER_SetResolutionLimit(FALSE);

				for(i = 0; i < NUM_VIEW_ACTION; i++)
				{
					PAL_VIDEO_SetHide(i, eAvHideClient_Hdcp, FALSE);
				}

				PAL_OUT_SetHdcpAudioMute(FALSE);
			}
			break;

		case eDxHDCPSTATUS_FAIL:

			HxLOG_Debug("\t\tHDCP block mode is [%s]\n", OTL_ENUM2STR_HdcpBlockMode(s_eHdcpFailBlockMode));
			if(s_eNetflixDrmStatus != eNFLIX_DRM_INIT)
			{
				switch(s_eHdcpFailBlockMode)
				{
					case eHDCP_BLOCK_HDMIBLANK:
					case eHDCP_BLOCK_DOWNSCALE:
						PAL_HDMI_SetBlank(TRUE);
						break;

					case eHDCP_BLOCK_AVMUTE:
					{
						for(i = 0; i < NUM_VIEW_ACTION; i++)
						{
							if(s_stCopyRightContext[i].bHdcpEnable == TRUE)
							{
								hErr = PAL_PIPE_GetResourceId(i, eRxRSCTYPE_AV_DEC, &ulDecId);
								if(hErr == ERR_OK)
								{
									PAL_VIDEO_SetHide(ulDecId, eAvHideClient_Hdcp, TRUE);
									PAL_OUT_SetHdcpAudioMute(TRUE);
								}
							}

						}
						break;
					}

					default:
						break;
				}
			}
			else
			{
				PAL_HDMI_SetBlank(FALSE);
				PAL_SCALER_SetResolutionLimit(FALSE);

				for(i = 0; i < NUM_VIEW_ACTION; i++)
				{
					PAL_VIDEO_SetHide(i, eAvHideClient_Hdcp, FALSE);
				}

				PAL_OUT_SetHdcpAudioMute(FALSE);
			}
			break;

		case eDxHDCPSTATUS_NODEVICE:

			HxLOG_Debug("\t\tHDCP block mode is [%s]\n", OTL_ENUM2STR_HdcpBlockMode(s_eHdcpFailBlockMode));
			if(s_eNetflixDrmStatus != eNFLIX_DRM_INIT)
			{
				switch(s_eHdcpNoDeviceBlockMode)
				{
					case eHDCP_BLOCK_HDMIBLANK:
						PAL_HDMI_SetBlank(TRUE);
						break;

					case eHDCP_BLOCK_DOWNSCALE:
						PAL_SCALER_SetResolutionLimit(TRUE);
						break;

					case eHDCP_BLOCK_AVMUTE:
					{
						for(i = 0; i < NUM_VIEW_ACTION; i++)
						{
							if(s_stCopyRightContext[i].bHdcpEnable == TRUE)
							{
								hErr = PAL_PIPE_GetResourceId(i, eRxRSCTYPE_AV_DEC, &ulDecId);
								if(hErr == ERR_OK)
								{
									PAL_VIDEO_SetHide(ulDecId, eAvHideClient_Hdcp, TRUE);
									PAL_OUT_SetHdcpAudioMute(TRUE);
								}
							}

						}

						break;
					}

					default:
						break;
				}
			}
			else
			{
				PAL_HDMI_SetBlank(FALSE);
				PAL_SCALER_SetResolutionLimit(FALSE);

				for(i = 0; i < NUM_VIEW_ACTION; i++)
				{
					PAL_VIDEO_SetHide(i, eAvHideClient_Hdcp, FALSE);
				}

				PAL_OUT_SetHdcpAudioMute(FALSE);
			}
			break;

		default:
			break;
	}

	return ERR_OK;
}

void SVC_OUT_SetHdcpFailBlockMode(HdcpBlockMode_e eFailBlockMode)
{
	s_eHdcpFailBlockMode = eFailBlockMode;
}

void SVC_OUT_GetHdcpFailBlockMode(HdcpBlockMode_e *peFailBlockMode)
{
	*peFailBlockMode = s_eHdcpFailBlockMode;
}

void SVC_OUT_SetHdcpNoDeviceBlockMode(HdcpBlockMode_e eNoDeviceBlockMode)
{
	s_eHdcpNoDeviceBlockMode = eNoDeviceBlockMode;
}

void SVC_OUT_GetHdcpNoDeviceBlockMode(HdcpBlockMode_e *peNoDeviceBlockMode)
{
	*peNoDeviceBlockMode = s_eHdcpNoDeviceBlockMode;
}

#define _____VIDEO_API______________________________________________________________________________________________________________________________
HERROR SVC_OUT_SetCartOutput(HBOOL bEnable)
{
#if defined(CONFIG_MW_OUTPUT_SCART)
	return PAL_SCART_SetOutput (bEnable);
#else
	return ERR_FAIL;
#endif
}

HERROR SVC_OUT_GetTvScartFormat(DxScartFormat_e *peScartFormat)
{
#if defined(CONFIG_MW_OUTPUT_SCART)
	return PAL_SCART_GetTvFormat(peScartFormat);
#else
	return ERR_FAIL;
#endif
}

HERROR SVC_OUT_SetTvScartFormat(DxScartFormat_e eScartFormat)
{
#if defined(CONFIG_MW_OUTPUT_SCART)
	return PAL_SCART_SetTvFormat (eScartFormat);
#else
	return ERR_FAIL;
#endif
}

HERROR SVC_OUT_GetVcrScartFormat(DxScartFormat_e *peScartFormat)
{
#if defined(CONFIG_MW_OUTPUT_SCART)
	return PAL_SCART_GetVcrFormat(peScartFormat);
#else
	return ERR_FAIL;
#endif
}

HERROR SVC_OUT_SetVcrScartFormat(DxScartFormat_e eScartFormat)
{
#if defined(CONFIG_MW_OUTPUT_SCART)
	return PAL_SCART_SetVcrFormat (eScartFormat);
#else
	return ERR_FAIL;
#endif
}

HERROR SVC_OUT_GetScartPath(DxScartPath_e *peScartPath)
{
#if defined(CONFIG_MW_OUTPUT_SCART)
	return PAL_SCART_GetPath(peScartPath);
#else
	return ERR_FAIL;
#endif
}

HERROR SVC_OUT_SetScartPath(DxScartPath_e eScartPath)
{
#if defined(CONFIG_MW_OUTPUT_SCART)
	return PAL_SCART_SetPath(eScartPath);
#else
	return ERR_FAIL;
#endif
}

HERROR SVC_OUT_GetSupportHdmiVideoTimingList(HUINT32 *pulNumOfList, VIDEO_Timing_t **ppstVideoTimingList)
{
#ifdef CONFIG_MW_OUTPUT_HDMI
	return PAL_HDMI_GetSupportVideoTimingList(pulNumOfList, ppstVideoTimingList);
#else
	return ERR_FAIL;
#endif
}

HERROR SVC_OUT_GetSupportHdmiAudioCodecList(HUINT32 *pulNumOfList, DxAudioCodec_e **ppeAudioCodecList)
{
#ifdef CONFIG_MW_OUTPUT_HDMI
	return PAL_HDMI_GetSupportAudioCodecList(pulNumOfList, ppeAudioCodecList);
#else
	return ERR_FAIL;
#endif
}

HERROR SVC_OUT_CheckVideoTimingAvailable(VIDEO_Timing_t stTiming)
{
	return PAL_HDMI_CheckVideoTimingAvailable(stTiming);
}

HERROR SVC_OUT_CheckAudioCodecAvailable(DxAudioCodec_e eAudioCodec)
{
	return PAL_HDMI_CheckAudioCodecAvailable(eAudioCodec);
}

HERROR SVC_OUT_CheckHdmiAudioOutput(DxAudioCodec_e eAudioFormat, DxHdmiAudioSR_e eSampleRate, HBOOL bIsHRA, HBOOL *pbSupport)
{
        DxHdmiAudioSR_e eTransSR = eDxHDMI_AUDIO_SR_Max;
        if ( ERR_OK != PAL_OUT_GetAudioTransSampleRate(eSampleRate, bIsHRA, &eTransSR))
                return ERR_FAIL;

        return PAL_HDMI_IsSupportSampleRate(eAudioFormat, eTransSR, pbSupport);
}

HERROR SVC_OUT_GetHdmi3DFormatCapability(DxVideo3DFormat_b *pulHdmi3DCapa)
{
	return PAL_HDMI_Get3DFormatCapability(pulHdmi3DCapa);
}

HERROR SVC_OUT_SetCompositeStandard(DxCompositeStandard_e eCompStandard)
{
	return PAL_SCALER_SetCompositeStandard(eCompStandard);
}

HERROR SVC_OUT_GetCompositeStandard(DxCompositeStandard_e *peCompStandard)
{
	return PAL_SCALER_GetCompositeStandard(peCompStandard);
}

HERROR SVC_OUT_SetBackgroundColor(HUINT8 ucR, HUINT8 ucG, HUINT8 ucB)
{
	HERROR				hErr;

	hErr = PAL_SCALER_SetBackgroundColor(ucR, ucG, ucB);

	DB_PARAM_SetItem(eDB_PARAM_ITEM_BACKGROUND_COLOR_R, (HUINT32)ucR, 0);
	DB_PARAM_SetItem(eDB_PARAM_ITEM_BACKGROUND_COLOR_G, (HUINT32)ucG, 0);
	DB_PARAM_SetItem(eDB_PARAM_ITEM_BACKGROUND_COLOR_B, (HUINT32)ucB, 0);

	return hErr;
}

HERROR SVC_OUT_GetBackgroundColor(HUINT8 *pucR, HUINT8 *pucG, HUINT8 *pucB)
{
	return PAL_SCALER_GetBackgroundColor(pucR, pucG, pucB);
}

HERROR SVC_OUT_CaptureVideoToBuffer (HUINT32 ulVideoDecId, HUINT32 ulWidth, HUINT32 ulHeight, PixelFormat_e ePixelFormat, HUINT8 **ppucFrameBuffer, HUINT32 *pulBufferSize)
{
	return PAL_THUMB_CaptureImageToBuffer (ulVideoDecId, ulWidth, ulHeight, ePixelFormat, ppucFrameBuffer, pulBufferSize);
}

HERROR SVC_OUT_FreeCaptureBuffer (HUINT8 *pucFrameBuffer)
{
	return PAL_THUMB_FreeBuffer (pucFrameBuffer);
}

HERROR SVC_OUT_CaptureVideoToPngFile (HUINT32 ulVideoDecId, HUINT32 ulWidth, HUINT32 ulHeight, HCHAR *szFileName)
{
	return PAL_THUMB_CaptureImageToPngFile (ulVideoDecId, ulWidth, ulHeight, szFileName);
}

HERROR SVC_OUT_SetHdmi3DSelect(DxHdmi3DSelect_e e3DSelect)
{
	return PAL_SCALER_SetHdmi3DSelect(e3DSelect);
}

HERROR SVC_OUT_GetHdmi3DSelect(DxHdmi3DSelect_e *pe3DSelect)
{
	return PAL_SCALER_GetHdmi3DSelect(pe3DSelect);
}

#define _____SCALER_API______________________________________________________________________________________________________________________________
void SVC_OUT_SetEdidAspectRatio(HBOOL bEnable)
{
	PAL_SCALER_SetEdidAspectRatio(bEnable);
}

void SVC_OUT_SetWideSignalLetterbox(HBOOL bEnable)
{
	PAL_SCALER_SetWideSignalLetterbox(bEnable);
}

HERROR SVC_OUT_SetTvAspectRatio(DxAspectRatio_e eTvAspectRatio, HBOOL bSyncNow)
{
	HERROR		hErr;

	HxLOG_Debug("%s(%s, %d)\n", __FUNCTION__
							, OTL_ENUM2STR_AspectRatio(eTvAspectRatio)
							, bSyncNow
					);

	hErr = PAL_SCALER_SetTvAspectRatio(eTvAspectRatio, bSyncNow);
	if(hErr == ERR_OK)
	{
		if (PalParam_Activated())
		{
			PAL_PARAM_SetField(ePAL_PARAM_ASPECT_RATIO, (void *)&eTvAspectRatio, sizeof(DxAspectRatio_e));
		}
		else
		{
			PAL_SYS_SetAspectRatio(eTvAspectRatio);
		}
	}
	else
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR SVC_OUT_GetTvAspectRatio(DxAspectRatio_e *peTvAspectRatio)
{
	return PAL_SCALER_GetTvAspectRatio(peTvAspectRatio);
}

HERROR SVC_OUT_SetDfcSelect(DxDfcSelect_e eDfc, HBOOL bSyncNow)
{
	HERROR		hErr;

	HxLOG_Debug("%s(%s, %d)\n", __FUNCTION__, OTL_ENUM2STR_DfcSelect(eDfc), bSyncNow);

	hErr = PAL_SCALER_SetDfcSelect(eDfc, bSyncNow);
	if(hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR SVC_OUT_GetDfcSelect(DxDfcSelect_e *peDfc)
{
	return PAL_SCALER_GetDfcSelect(peDfc);
}

HERROR SVC_OUT_SetResolutionSelect(DxResolutionSelect_e eResolutionSelect, HBOOL bSyncNow)
{
	HERROR		hErr;

	HxLOG_Debug("%s(%s, %d)\n", __FUNCTION__, OTL_ENUM2STR_ResolutionSelect(eResolutionSelect), bSyncNow);

	hErr = PAL_SCALER_SetResolutionSelect(eResolutionSelect, bSyncNow);
	if(hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR SVC_OUT_GetResolutionSelect(DxResolutionSelect_e *peResolutionSelect)
{
	return PAL_SCALER_GetResolutionSelect(peResolutionSelect);
}

HERROR SVC_OUT_SetMediaRect(HUINT32 ulActionId, DxRect_t stMediaRect, HUINT32 ulRefW, HUINT32 ulRefH)
{
	return PAL_SCALER_SetMediaRect(ulActionId, stMediaRect, ulRefW, ulRefH);
}

HERROR SVC_OUT_GetMediaRect(HUINT32 ulActionId, DxRect_t *pstMediaRect, HUINT32 *pulRefW, HUINT32 *pulRefH)
{
	return PAL_SCALER_GetMediaRect(ulActionId, pstMediaRect, pulRefW, pulRefH);
}

HERROR SVC_OUT_SetOsdLayerOutput(OsdZOrder_t		ulZOrder,
							DxAspectRatio_e		eOsdAspectRatio,
							DxDfcSelect_e 			eDfc,
							PIG_Rect_t				stPigRect,
							DxAspectRatio_e 	ePigAspectRatio
							)
{
	return PAL_SCALER_SetOsdLayerOutput(ulZOrder, eOsdAspectRatio, eDfc, stPigRect, ePigAspectRatio);
}

HERROR SVC_OUT_GetOsdLayerOutput(OsdZOrder_t		ulZOrder,
							DxAspectRatio_e 	*peOsdAspectRatio,
							DxDfcSelect_e 			*peDfc,
							PIG_Rect_t				*pstPigRect,
							DxAspectRatio_e 	*pePigAspectRatio
							)
{
	return PAL_SCALER_GetOsdLayerOutput(ulZOrder, peOsdAspectRatio, peDfc, pstPigRect, pePigAspectRatio);

}

HERROR SVC_OUT_SyncVideoSetting(void)
{
	HxLOG_Debug("%s()\n", __FUNCTION__);

	PAL_SCALER_SyncVideoSetting();

	return ERR_OK;
}

HERROR SVC_OUT_GetOsdDisplayPoint(HUINT32 ulRefWidth, HUINT32 ulRefHeight, HINT32 nX, HINT32 nY, HINT32 *pnScaledX, HINT32 *pnScaledY)
{
	return PAL_SCALER_GetOsdDisplayPoint(ulRefWidth, ulRefHeight, nX, nY, pnScaledX, pnScaledY);
}

HERROR SVC_OUT_GetActiveVideoRect(HUINT32 *pulScreenWidth, HUINT32 *pulScreenHeight, DxRect_t *pstActiveRect, HBOOL *pbPig)
{
	return PAL_SCALER_GetActiveVideoRect(pulScreenWidth, pulScreenHeight, pstActiveRect, pbPig);
}

HERROR	SVC_OUT_SetSupportScalerByVideoFrame(HBOOL bEnable)
{
	return PAL_SCALER_SetSupportVideoFrame(bEnable);
}

#define _____AUDIO_API______________________________________________________________________________________________________________________________

HERROR SVC_OUT_SetVolume(HUINT32 ulVolume)
{
	return PAL_OUT_SetVolume(ulVolume);
}

HERROR SVC_OUT_GetVolume(HUINT32 *pulVolume)
{
	return PAL_OUT_GetVolume(pulVolume);
}

HERROR SVC_OUT_SetDecoderVolume(HUINT32 ulVolume)
{
	return PAL_OUT_SetDecoderVolume(ulVolume);
}

HERROR SVC_OUT_GetDecoderVolume(HUINT32 *pulVolume)
{
	return PAL_OUT_GetDecoderVolume(pulVolume);
}

HERROR SVC_OUT_GetVolumeRange(HUINT32 *pulVolumRange)
{
	return PAL_OUT_GetVolumeRange(pulVolumRange);
}

HERROR SVC_OUT_SetMasterAudioMute(HBOOL bMute)
{
	return PAL_OUT_SetMasterAudioMute(bMute);
}

HBOOL SVC_OUT_GetMasterAudioMute(void)
{
	return PAL_OUT_GetMasterAudioMute();
}

HERROR SVC_OUT_SetHdmiAudioTransCode(DxAudioCodec_e eSourceCodec, DxAudioCodec_e eTransCodec)
{
	return PAL_OUT_SetHdmiAudioTransCode(eSourceCodec, eTransCodec);
}

HERROR SVC_OUT_GetHdmiAudioTransCode(DxAudioCodec_e eSourceCodec, DxAudioCodec_e *peTransCodec)
{
	return PAL_OUT_GetHdmiAudioTransCode(eSourceCodec, peTransCodec);
}

HERROR SVC_OUT_SetHdmiAudioFormat(DxDigitalAudioFormat_e eDigitalAudioOut)
{
	return PAL_OUT_SetHdmiAudioFormat(eDigitalAudioOut);
}

HERROR SVC_OUT_SetHdmiAudioMute(HBOOL bMute)
{
        return PAL_OUT_SetHdmiAudioMute(bMute);
}

HERROR SVC_OUT_GetHdmiAudioFormat(DxDigitalAudioFormat_e *peDigitalAudioOut)
{
	return PAL_OUT_GetHdmiAudioFormat(peDigitalAudioOut);
}

HERROR SVC_OUT_SetSpdifAudioTransCode(DxAudioCodec_e eSourceCodec, DxAudioCodec_e eTransCodec)
{
	return PAL_OUT_SetSpdifAudioTransCode(eSourceCodec, eTransCodec);
}

HERROR SVC_OUT_GetSpdifAudioTransCode(DxAudioCodec_e eSourceCodec, DxAudioCodec_e *peTransCodec)
{
	return PAL_OUT_GetSpdifAudioTransCode(eSourceCodec, peTransCodec);
}

HERROR SVC_OUT_SetSpdifAudioFormat(DxDigitalAudioFormat_e eDigitalAudioOut)
{
	return PAL_OUT_SetSpdifAudioFormat(eDigitalAudioOut);
}

HERROR SVC_OUT_GetSpdifAudioFormat(DxDigitalAudioFormat_e *peDigitalAudioOut)
{
	return PAL_OUT_GetSpdifAudioFormat(peDigitalAudioOut);
}

HERROR SVC_OUT_GetOutputStatus(OutputStatus_t *pstOutputStatus)
{
	PalHdmi_Status_t	stHdmiStatus;

	PAL_OUT_GetOutputStatus(
						&pstOutputStatus->eHdmiAudioCodec,
						&pstOutputStatus->eSpdifAudioCodec,
						&pstOutputStatus->eVideoMacrovision,
						&pstOutputStatus->eCgmsMacrovision,
						&pstOutputStatus->eCsmsCopyRight,
						&pstOutputStatus->eVideoBlankCopyRight,
						&pstOutputStatus->eVideoBlankWss
						);

	PAL_SCALER_GetOutputStatus(
						&pstOutputStatus->stOutputTiming,
						&pstOutputStatus->ulDfcStatus,
						&pstOutputStatus->eHdmi3DStatus
						);

	PAL_HDMI_GetStatus(&stHdmiStatus);
	pstOutputStatus->bHdmiActive = (stHdmiStatus.bOutputEnable == TRUE) && (stHdmiStatus.bPlug == TRUE);
	pstOutputStatus->eHdmiAspectRatio = stHdmiStatus.eAspectRatio;
	pstOutputStatus->eHdmiAfd = stHdmiStatus.eAfd;
	HxSTD_MemCopy(&pstOutputStatus->stHdmiEdidInfo, &stHdmiStatus.stHdmiEdidInfo, sizeof(stHdmiStatus.stHdmiEdidInfo));

	return ERR_OK;
}

#define _____THERMAL_PROTECTION_API______________________________________________________________________________________________________________________________
#if defined(CONFIG_MW_SYS_THERMAL_PROTECTION)
HERROR SVC_OUT_GetOverTemp(HBOOL *bTemp)
{
	*bTemp = s_bOverTemp;
	return ERR_OK;
}

HERROR SVC_OUT_GetFanLock(HBOOL *bTemp)
{
	*bTemp = s_bFanLock;
	return ERR_OK;
}
#endif

/*********************** End of File ******************************/
