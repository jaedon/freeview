/**
	@file		mgr_output.c
	@brief    .

	Description: .
	Module: AP/AM								 	\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <get_enum_str.h>

#include <db_param.h>

#include <svc_pipe.h>
#include <svc_av.h>
#include <svc_output.h>

#include <bus.h>

#include <mgr_common.h>
#include <mgr_action.h>
//#include <mgr_live.h>
#include <mgr_output.h>

#include <pmgr_output.h>


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/


#if defined (CONFIG_MW_CAS) || defined (CONFIG_MW_CI)						/* CAS Model */
#if defined(CONFIG_PROD_IR4100HD) || defined (CONFIG_PROD_IR4000HD)
#define NO_AV_EVENT_TIMEOUT					8000
#else
#define NO_AV_EVENT_TIMEOUT					6000
#endif
#elif defined (CONFIG_MW_MM_PVR)
#define NO_AV_EVENT_TIMEOUT					5000
#else
#define NO_AV_EVENT_TIMEOUT					3000
#endif

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/******************* Static Function Prototypes  *************************/
/*******************************************************************/


/*******************************************************************/
/********************      API Functions   *************************/
/*******************************************************************/


/********************************************************************/
/********************	Application Proc.	*************************/
/********************************************************************/

STATIC BUS_Result_t proc_output(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{

	switch(message)
	{
		case eMEVT_BUS_CREATE:
			BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
			break;

		case eSEVT_OUT_CHECK_SCART:
			HxLOG_Debug("%s(eSEVT_OUT_CHECK_SCART-p1[%d], p2[%d], p3[%d])\n", p1, p2, p3);

			if (p1 == eSCART_EVENT_VCR_ASPECTRATIO)
			{

			}
			else if (p1 == eSCART_EVENT_VCR_POWER)
			{
				if(p2 == TRUE)
				{
					HxLOG_Debug(" set scart path, (eDxSCART_PATH_VCR2TV) \n");
					SVC_OUT_SetScartPath(eDxSCART_PATH_VCR2TV);
				}
				else
				{
					HxLOG_Debug(" set scart path, (eDxSCART_PATH_STB2TV) \n");
					SVC_OUT_SetScartPath(eDxSCART_PATH_STB2TV);
				}
			}
			else
			{
				HxLOG_Print("unknown scart event \n");
			}

			break;

		default:
			break;
	}

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}

HERROR MGR_OUTPUT_Init(void)
{
	BUS_Result_t	eRes;
	HUINT32			i;
	HUINT32 		ulLipSyncDealy = 0;

	SVC_OUT_SetInitParameters(pmgr_output_GetVbiTeletextSpec(),
						pmgr_output_GetVideoBlankSpec(),
						pmgr_output_GetCompositeSpec(),
						pmgr_output_GetInitialDfcSelect(),
						pmgr_output_GetInitialResolutionSelect(),
						pmgr_output_GetInitialTvAspectRatio(),
						pmgr_output_GetInitial3DMode(),
						pmgr_output_GetEdidAspectRatioSpec(),
						pmgr_output_GetWideSignalLetterBoxSpec(),
						pmgr_output_GetHdmiAfdEnableSpec(),
						pmgr_output_GetHdcpFailBlockSpec(),
						pmgr_output_GetHdcpNoDeviceBlockSpec(),
						pmgr_output_GetInitialTvScartFormat(),
						pmgr_output_GetInitialVcrScartFormat(),
						pmgr_output_GetInitialHdmiAudioOutFormat(),
						pmgr_output_GetInitialSpdifAudioOutFormat()
						);

	pmgr_output_SetInitialHdmiAudioTransCode();
	pmgr_output_SetInitialSpdifAudioTransCode();

	SVC_OUT_SetBackgroundColor(0, 0, 0);

	SVC_AV_SetAudioEvtTimeOut(NO_AV_EVENT_TIMEOUT);
	SVC_AV_SetVideoEvtTimeOut(NO_AV_EVENT_TIMEOUT);

	if(DB_PARAM_GetItem(eDB_PARAM_ITEM_LIPSYNC_DELAY, (HUINT32 *)&ulLipSyncDealy, 0) == ERR_OK)
	{
		SVC_AV_SetAudioLipSyncDelay(ulLipSyncDealy);
	}

	SVC_OUT_SetVolume(pmgr_output_GetInitialVolume());
	SVC_OUT_SetDecoderVolume(pmgr_output_GetInitialDecoderVolume());

	eRes = BUS_MGR_Create(NULL, APP_DEFAULT_PRIORITY, proc_output, 0, 0, 0, 0);

	for(i = 0; i < NUM_VIEW_ACTION; i++)
	{
		MGR_OUTPUT_ResetCopyProtect(i);
	}

	SVC_OUT_SyncVideoSetting();

	return (eRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
}


HERROR MGR_OUTPUT_SetCopyProtect(HUINT32 ulActionId, MgrCopyrightControl_OutputControlInfo_t stCpInfo)
{
	HERROR		hErr;

	HxLOG_Debug("%s(%d)\n", __FUNCTION__, ulActionId);

	if(stCpInfo.bSetHdcp == TRUE)
	{
		SVC_OUT_SetHdcp(ulActionId, stCpInfo.bHdcp);
	}
	else
	{
		SVC_OUT_SetHdcp(ulActionId, pmgr_output_GetDefaultHdcp());
	}

	if (stCpInfo.bSetMacrovision == TRUE)
	{
		SVC_OUT_SetMacrovision(ulActionId, stCpInfo.eMacrovision);
	}
	else
	{
		SVC_OUT_SetMacrovision(ulActionId, eMACROVISION_OFF);
	}

	if (stCpInfo.bSetScmsCopyRight == TRUE)
	{
		SVC_OUT_SetScmsCopyRight(ulActionId, stCpInfo.eScmsCopyRight);
	}
	else
	{
		SVC_OUT_SetScmsCopyRight(ulActionId, eCOPYRIGHT_COPY_FREE);
	}

	if (stCpInfo.bSetCgmsCopyRight == TRUE)
	{
		SVC_OUT_SetVideoBlankCopyRight(ulActionId, stCpInfo.eCgmsCopyRight);
	}
	else
	{
		SVC_OUT_SetVideoBlankCopyRight(ulActionId, eCOPYRIGHT_COPY_FREE);
	}

	if(stCpInfo.bSetCgmsMacrovision == TRUE)
	{
		SVC_OUT_SetCgmsMacrovision(ulActionId, stCpInfo.eCgmsMacrovision);
	}
	else
	{
		SVC_OUT_SetCgmsMacrovision(ulActionId, eMACROVISION_OFF);
	}

	if(stCpInfo.bDigitalOnlyConstraint == TRUE)
	{
		SVC_AV_SetVideoOutputEnable(ulActionId, eSvcAv_DisplayType_Sd, FALSE);
	}
	else
	{
		SVC_AV_SetVideoOutputEnable(ulActionId, eSvcAv_DisplayType_Sd, TRUE);
	}

	hErr = SVC_AV_SetVideoHide(ulActionId, eAvMuteFlag_Drm, stCpInfo.bOutputMute);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("SVC_AV_SetVideoHide ret :0x%X\n", hErr);
	}
	hErr = SVC_AV_SetAudioHide(ulActionId, eAvMuteFlag_Drm, stCpInfo.bOutputMute);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("SVC_AV_SetAudioHide ret :0x%X\n", hErr);
	}

	return ERR_OK;
}

HERROR MGR_OUTPUT_ResetCopyProtect(HUINT32 ulActionId)
{
	HxLOG_Debug("%s(%d)\n", __FUNCTION__, ulActionId);

	if(ulActionId == MGR_ACTION_GetMainActionId())
	{
		SVC_OUT_SetHdcp(ulActionId, pmgr_output_GetDefaultHdcp());
	}
	else
	{
		SVC_OUT_SetHdcp(ulActionId, FALSE);
	}

	(void)SVC_OUT_SetMacrovision(ulActionId, eMACROVISION_OFF);
	(void)SVC_OUT_SetScmsCopyRight(ulActionId, eCOPYRIGHT_COPY_FREE);
	(void)SVC_OUT_SetVideoBlankCopyRight(ulActionId, eCOPYRIGHT_COPY_FREE);
	(void)SVC_OUT_SetCgmsMacrovision(ulActionId, eMACROVISION_OFF);
	(void)SVC_AV_SetVideoHide(ulActionId, eAvMuteFlag_Drm, FALSE);
	(void)SVC_AV_SetVideoOutputEnable(ulActionId, eSvcAv_DisplayType_Sd, TRUE);

	return ERR_OK;
}

HBOOL MGR_OUTPUT_SupportTvScart(void)
{
	return pmgr_output_SupportTvScart();
}

HBOOL MGR_OUTPUT_SupportVcrScart(void)
{
	return pmgr_output_SupportVcrScart();
}

HBOOL MGR_OUTPUT_SupportSpdif(void)
{
	return pmgr_output_SupportSpdif();
}

HBOOL MGR_OUTPUT_SupportCec(void)
{
	return pmgr_output_SupportCec();
}

DxScartFormat_b MGR_OUTPUT_GetSupportTvScartFormat(void)
{
	return pmgr_output_GetSupportTvScartFormat();
}

DxScartFormat_b MGR_OUTPUT_GetSupportVcrScartFormat(void)
{
	return pmgr_output_GetSupportVcrScartFormat();
}


