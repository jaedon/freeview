/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  svc_pipe.c
	@brief

	Description:  									\n
	Module: OBAMA/SVC/PIPE			\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

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
 * (c) 2011-2013 Humax Co., Ltd.
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

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <dlib.h>
#include <rlib.h>

#include <stb_params.h>
#include <pal_pipe.h>
#include <pal_channel.h>
#include <pal_video.h>
#include <pal_sef.h>
#include <pal_pb.h>
#include <pal_rec.h>
#include <pal_transcoder.h>

#include <db_param.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

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
/********************      Functions     ********************/
/*******************************************************************/
#define ___LOCAL_FUNCTION___
#define _____CONVERSION_FUNCTIONS_____
STATIC RxUsageType_e svc_pipe_ConvDecType2RxRscUsageType (VIDEO_DecType_t eDecType)
{
	switch (eDecType)
	{
	case eVIDEO_Dec_Main:
		return eRxUSAGETYPE_AVDEC_MAIN;

	case eVIDEO_Dec_Sub:
		return eRxUSAGETYPE_AVDEC_SUB;

	default:
		break;
	}

	return eRxUSAGETYPE_AVDEC_MAIN;
}

STATIC RxUsageType_e svc_pipe_ConvDemuxType2RxRscUsageType (PalSef_DemuxType_e eDmxType)
{
	switch (eDmxType)
	{
	case eDMX_Type_LiveRec:
		return eRxUSAGETYPE_DEMUX_LIVEREC;

	case eDMX_Type_Playback:
		return eRxUSAGETYPE_DEMUX_PLAYBACK;

	default:
		break;
	}

	return eRxUSAGETYPE_NONE;
}

STATIC RxUsageType_e svc_pipe_ConvRecType2RxRscUsageType (PalRec_DeviceType_e eRecType)
{
	switch (eRecType)
	{
		case eREC_Type_Rec:
			return eRxUSAGETYPE_RECHDD_REC;
		case eREC_Type_Trans:
			return eRxUSAGETYPE_RECHDD_TRANS;
		case eREC_Type_SatIp:
			return eRxUSAGETYPE_RECHDD_SATIP;
		default :
			break;
	}

	return eRxUSAGETYPE_NONE;
}

#define _____RESOURCE_INITIALIZATION_____
STATIC HERROR svc_pipe_InitRfInput (void)
{
	HUINT32 					 ulRfCnt = 0, ulNumDevice = 0;
	Handle_t					 hRscHandle = PAL_PIPE_GetRscSessionHandle();
	RxRfInfoCapacity_t			 stRfInfo;
	HERROR						 hErr;

#if defined(CONFIG_MW_CH_SATELLITE)
	ulNumDevice = PAL_CH_SAT_CountRfInput();
#endif
#if defined(CONFIG_MW_CH_CABLE)
	ulNumDevice ++;
#endif
#if defined(CONFIG_MW_CH_TERRESTRIAL)
	ulNumDevice ++;
#endif

	if (ulNumDevice == 0)
	{
		HxLOG_Error("Device Num = 0\n");
		return ERR_FAIL;
	}

	HxLOG_Print(HxANSI_COLOR_YELLOW("ulNumDevice = %d\n"), ulNumDevice);

	hErr = RLIB_SetResourceNumber(hRscHandle, eRxRSCTYPE_RFINPUT, ulNumDevice);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("RLIB_SetResourceNumber err: eRxRSCTYPE_RFINPUT, RscNum(%d)\n", ulNumDevice);
		return ERR_FAIL;
	}

#if defined(CONFIG_MW_CH_SATELLITE)
{
	HUINT32 				 ulSatRfInputNum = PAL_CH_SAT_CountRfInput();
	PalCh_SatRfInput_t		 stPalRfInput;
	HERROR					 hErr;

	for (; ulRfCnt < ulSatRfInputNum; ulRfCnt++)
	{
		hErr = PAL_CH_SAT_GetRfInput(ulRfCnt, &stPalRfInput);
		if (ERR_OK == hErr)
		{
			stRfInfo.eSupportedDeliType = eDxDELIVERY_TYPE_SAT;
			stRfInfo.ulConnectableTunerNum = (stPalRfInput.ulConnectableTunerNum > RxTUNER_NUM_MAX) ? RxTUNER_NUM_MAX : stPalRfInput.ulConnectableTunerNum;
			HxSTD_MemCopy (stRfInfo.aulConnectableTunerId, stPalRfInput.aulConnectableTunerId, sizeof(HUINT32) * stRfInfo.ulConnectableTunerNum);

			hErr = RLIB_SetRfInfoCapacity(hRscHandle, ulRfCnt, &stRfInfo);
			if (ERR_OK != hErr)
			{
				HxLOG_Error("RLIB_SetRfInfoCapacity Err: ulRscId(%d)\n", ulRfCnt);
				return ERR_FAIL;
			}
		}
	}
}
#endif

#if defined(CONFIG_MW_CH_CABLE)
	stRfInfo.eSupportedDeliType = eDxDELIVERY_TYPE_CAB;
	stRfInfo.ulConnectableTunerNum = 0;

	hErr = RLIB_SetRfInfoCapacity(hRscHandle, ulRfCnt, &stRfInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("RLIB_SetRfInfoCapacity Err: ulRscId(%d)\n", ulRfCnt);
		return ERR_FAIL;
	}

	ulRfCnt++;
#endif

#if defined(CONFIG_MW_CH_TERRESTRIAL)
		stRfInfo.eSupportedDeliType = eDxDELIVERY_TYPE_TER;
		stRfInfo.ulConnectableTunerNum = 0;

		hErr = RLIB_SetRfInfoCapacity(hRscHandle, ulRfCnt, &stRfInfo);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("RLIB_SetRfInfoCapacity Err: ulRscId(%d)\n", ulRfCnt);
			return ERR_FAIL;
		}

		ulRfCnt++;
#endif

	/*
		RF-Input Ability 조정을 해야 한다.
	*/
#if defined(CONFIG_MW_CH_SATELLITE)
	{
		HUINT32 				 ulSatRfInputNum = PAL_CH_SAT_CountRfInput();
		DxAntennaConnection_e		 eAntConn;

//		eAntConn = MWC_PARAM_GetAntennaConnectionType();
		hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_ANT_CONN_TYPE, (HUINT32 *)&eAntConn, 0);
		if (ERR_OK != hErr)
		{
			HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_ANT_CONN_TYPE) Error!!!\n");
		}

		if (ulSatRfInputNum >= 1)
		{
			RLIB_SetRfInputAbility (PAL_PIPE_GetRscSessionHandle(), 0, TRUE);
		}

		if (ulSatRfInputNum >= 2)
		{
			switch (eAntConn)
			{
			case eDxANTCONN_TWOSAME:
				RLIB_SetRfInputAbility (PAL_PIPE_GetRscSessionHandle(), 1, TRUE);
				break;

			case eDxANTCONN_ONECABLE:
			default:
				RLIB_SetRfInputAbility (PAL_PIPE_GetRscSessionHandle(), 1, FALSE);
				break;
			}
		}

		if (ulSatRfInputNum >= 3)
		{
			HxLOG_Error("\n\n\nSatellite RF Input Num (%d): Need Some Logic Here!!!\n\n\n", ulSatRfInputNum);
		}
	}
#endif

	return ERR_OK;
}

STATIC HERROR svc_pipe_InitTunerGroup (void)
{
	HUINT32 					 ulCnt, ulNumDevice = 0;
	Handle_t					 hRscHandle = PAL_PIPE_GetRscSessionHandle();
	HERROR						 hErr;

	ulNumDevice = PAL_CH_GetNumOfTunerGroup();
	if (ulNumDevice > 0)
	{
		DxDeliveryType_e	 eDxDeliType = eDxDELIVERY_TYPE_ALL;

		HxLOG_Print(HxANSI_COLOR_YELLOW("ulNumDevice = %d\n"), ulNumDevice);

		hErr = RLIB_SetResourceNumber(hRscHandle, eRxRSCTYPE_TUNER_GROUP, ulNumDevice);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("RLIB_SetResourceNumber err: eRxRSCTYPE_TUNER_GROUP, RscNum(%d)\n", ulNumDevice);
			return ERR_FAIL;
		}

		for (ulCnt = 0; ulCnt < ulNumDevice; ulCnt++)
		{
			PAL_CH_GetSupportDeliveryType(ulCnt, &eDxDeliType);

			HxLOG_Print(HxANSI_COLOR_YELLOW("[%d] eDxDeliType = %d\n"), ulCnt, eDxDeliType);
			hErr = RLIB_SetTunerCapacity(hRscHandle, ulCnt, eDxDeliType);
			if (ERR_OK != hErr)
			{
				HxLOG_Error("RLIB_SetTunerCapacity err: ulRscId(%d), DeliType(0x%08x)\n", ulCnt, eDxDeliType);
				return ERR_FAIL;
			}
		}
	}

	return ERR_OK;
}

STATIC HERROR svc_pipe_InitAvDec (void)
{
	HUINT32 					 ulCnt, ulDecoderCnt;
	Handle_t					 hRscHandle = PAL_PIPE_GetRscSessionHandle();
	RxUsageType_e				 eRscUsageType;
	VIDEO_DecType_t				 eVideoDecType;
	HERROR						 hErr;

	ulDecoderCnt = PAL_VIDEO_GetSupportNumberOfDecoder();
	if (ulDecoderCnt > 0)
	{
#if defined(CONFIG_SUPPORT_FCC)
		// rsc setup을 편하게 하기위해 fake av dec를 2개 늘려준다
		ulDecoderCnt += 2;
#endif
		HxLOG_Print(HxANSI_COLOR_YELLOW("ulDecoderCnt = %d\n"), ulDecoderCnt);

		hErr = RLIB_SetResourceNumber(hRscHandle, eRxRSCTYPE_AV_DEC, ulDecoderCnt);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("RLIB_SetResourceNumber err: eRxRSCTYPE_AV_DEC, RscNum(%d)\n", ulDecoderCnt);
			return ERR_FAIL;
		}

		for (ulCnt = 0; ulCnt < ulDecoderCnt; ulCnt++)
		{
			eVideoDecType = PAL_VIDEO_GetDecoderType(ulCnt);
			eRscUsageType = svc_pipe_ConvDecType2RxRscUsageType (PAL_VIDEO_GetDecoderType(ulCnt));

			HxLOG_Print(HxANSI_COLOR_YELLOW("[%d] eVideoDecType = %d, eRscUsageType = %d\n"), ulCnt, eVideoDecType, eRscUsageType);
			hErr = RLIB_SetResourceUsageType(hRscHandle, eRxRSCTYPE_AV_DEC, ulCnt, eRscUsageType);
			if (ERR_OK != hErr)
			{
				HxLOG_Error("RLIB_SetResourceUsageType err: eRxRSCTYPE_AV_DEC, eRscUsageType(%d)\n", eRscUsageType);
				return ERR_FAIL;
			}
		}
	}

	return ERR_OK;
}

STATIC HERROR svc_pipe_InitDemux (void)
{
	HUINT32 					 ulCnt, ulNumDevice;
	Handle_t					 hRscHandle = PAL_PIPE_GetRscSessionHandle();
	RxUsageType_e				 eRscUsageType;
	PalSef_DemuxType_e			 eDemuxType;
	PalSef_DemuxInfo_e			 stDemuxInfo;
	HERROR						 hErr;

	hErr = PAL_SEF_GetInfo (&stDemuxInfo);
	if (ERR_OK == hErr)
	{
		ulNumDevice = (HUINT32)stDemuxInfo.ucNumLiveRecDemux + (HUINT32)stDemuxInfo.ucNumPbDemux;
		if (ulNumDevice > 0)
		{
			HxLOG_Print(HxANSI_COLOR_YELLOW("ulNumDevice = %d\n"), ulNumDevice);
			hErr = RLIB_SetResourceNumber(hRscHandle, eRxRSCTYPE_DEMUX, ulNumDevice);
			if (ERR_OK != hErr)
			{
				HxLOG_Error("RLIB_SetResourceNumber err: eRxRSCTYPE_DEMUX, RscNum(%d)\n", ulNumDevice);
				return ERR_FAIL;
			}

			for (ulCnt = 0; ulCnt < ulNumDevice; ulCnt++)
			{
				hErr = PAL_SEF_GetType (ulCnt, &eDemuxType);
				if (ERR_OK == hErr)
				{
					eRscUsageType = svc_pipe_ConvDemuxType2RxRscUsageType(eDemuxType);
					if (eRxUSAGETYPE_NONE != eRscUsageType)
					{
						HxLOG_Print(HxANSI_COLOR_YELLOW("[%d] eDemuxType = %d, eRscUsageType = %d\n"), ulCnt, eDemuxType, eRscUsageType);
						hErr = RLIB_SetResourceUsageType(hRscHandle, eRxRSCTYPE_DEMUX, ulCnt, eRscUsageType);
						if (ERR_OK != hErr)
						{
							HxLOG_Error("RLIB_SetResourceUsageType err: eRxRSCTYPE_DEMUX, eRscUsageType(%d)\n", eRscUsageType);
							return ERR_FAIL;
						}
					}
				}
			}
		}
	}

	return ERR_OK;
}

STATIC HERROR svc_pipe_InitPvrPlayer (void)
{
#if defined(CONFIG_MW_MULTIMEDIA) && defined(CONFIG_MW_MM_PVR)
	HUINT32 					 ulNumDevice = 0;
	HERROR						 hErr;

	hErr = PAL_PB_GetCapability (&ulNumDevice);
	if ((ERR_OK == hErr) && (ulNumDevice > 0))
	{
		HxLOG_Print(HxANSI_COLOR_YELLOW("ulNumDevice = %d\n"), ulNumDevice);
		hErr = RLIB_SetResourceNumber(PAL_PIPE_GetRscSessionHandle(), eRxRSCTYPE_PVR_PLAYER, ulNumDevice);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("RLIB_SetResourceNumber err: eRxRSCTYPE_PVR_PLAYER, RscNum(%d)\n", ulNumDevice);
			return ERR_FAIL;
		}
	}
#endif
	return ERR_OK;
}

STATIC HERROR svc_pipe_InitHddRecorder (void)
{
#if defined(CONFIG_MW_MM_PVR)
	HUINT32 					 ulCnt, ulNumDevice;
	HERROR						 hErr;
	RxUsageType_e				 eRscUsageType;
	PalRec_DeviceType_e 		 eRecType;
	Handle_t					 hRscHandle = PAL_PIPE_GetRscSessionHandle();

	hErr = PAL_REC_GetCapability (&ulNumDevice);
	if ((ERR_OK == hErr) && (ulNumDevice > 0))
	{
		hErr = RLIB_SetResourceNumber(hRscHandle, eRxRSCTYPE_HDD_REC, ulNumDevice);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("RLIB_SetResourceNumber err: eRxRSCTYPE_HDD_REC, RscNum(%d)\n", ulNumDevice);
			return ERR_FAIL;
		}


		for (ulCnt = 0; ulCnt < ulNumDevice; ulCnt++)
		{
			hErr = PAL_REC_GetType (ulCnt, &eRecType);
			if (ERR_OK == hErr)
			{
				eRscUsageType = svc_pipe_ConvRecType2RxRscUsageType(eRecType);
				if (eRxUSAGETYPE_NONE != eRscUsageType)
				{
					HxLOG_Print(HxANSI_COLOR_YELLOW("[%d] eRecType = %d, eRscUsageType = %d\n"), ulCnt, eRecType, eRscUsageType);
					hErr = RLIB_SetResourceUsageType(hRscHandle, eRxRSCTYPE_HDD_REC, ulCnt, eRscUsageType);
					if (ERR_OK != hErr)
					{
						HxLOG_Error("RLIB_SetResourceUsageType err: eRxRSCTYPE_HDD_REC, eRscUsageType(%d)\n", eRscUsageType);
						return ERR_FAIL;
					}
				}
			}
		}
	}
#endif
	return ERR_OK;
}

STATIC HERROR svc_pipe_InitTranscoder (void)
{
#if defined(CONFIG_MW_MM_TRANSCODING)
	HUINT32 					 ulNumDevice = 0;
	HERROR						 hErr;

	hErr = PAL_TRANSC_GetNumOfDevice (&ulNumDevice);
	if ((ERR_OK == hErr) && (ulNumDevice > 0))
	{
		HxLOG_Print(HxANSI_COLOR_YELLOW("ulNumDevice = %d\n"), ulNumDevice);
		hErr = RLIB_SetResourceNumber(PAL_PIPE_GetRscSessionHandle(), eRxRSCTYPE_TRANSCODER, ulNumDevice);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("RLIB_SetResourceNumber err: eRxRSCTYPE_TRANSCODER, RscNum(%d)\n", ulNumDevice);
			return ERR_FAIL;
		}
	}
#endif
	return ERR_OK;
}


#define ___API_FUNCTION___
HERROR SVC_PIPE_Init (void)
{
	(void)RLIB_SetActionNumber(PAL_PIPE_GetRscSessionHandle(), eActionId_MAX);

	svc_pipe_InitRfInput();
	svc_pipe_InitTunerGroup();
	svc_pipe_InitAvDec();
	svc_pipe_InitDemux();
	svc_pipe_InitPvrPlayer();
	svc_pipe_InitHddRecorder();
	svc_pipe_InitTranscoder();

	return ERR_OK;
}

/*
	ActionID, Action Handle
*/
HUINT32 SVC_PIPE_GetActionId (Handle_t hAction)
{
	return PAL_PIPE_GetActionId (hAction);
}

HUINT32 SVC_PIPE_GetSessionVersion (Handle_t hAction)
{
	return PAL_PIPE_GetSessionVersion (hAction);
}

Handle_t SVC_PIPE_GetActionHandle (HUINT32 ulActionId)
{
	return PAL_PIPE_GetActionHandle (ulActionId);
}

Handle_t SVC_PIPE_IncreaseActionVersion (HUINT32 ulActionId)
{
	return PAL_PIPE_IncreaseActionVersion (ulActionId);
}

HERROR SVC_PIPE_IsActionHandleLatest (Handle_t hAction)
{
	return PAL_PIPE_IsActionHandleLatest (hAction);
}

/*
	Resource ID
*/
HERROR SVC_PIPE_GetResourceNumber (RxRscType_e eRscType, HUINT32 *pulRscNum)
{
	return PAL_PIPE_GetResourceNumber(eRscType, pulRscNum);
}

HERROR SVC_PIPE_GetResourceId (HUINT32 ulActionId, RxRscType_e eRscType, HUINT32 *pulRscId)
{
	return PAL_PIPE_GetResourceId (ulActionId, eRscType, pulRscId);
}

HERROR SVC_PIPE_ChangeResourceId (HUINT32 ulActionId, RxRscType_e eRscType, HUINT32 ulRscId)
{
	return PAL_PIPE_ChangeResourceId (ulActionId, eRscType, ulRscId);
}

/*
	Resource Take / Release
*/
Handle_t SVC_PIPE_MakeResourceRequestHandle (HUINT32 ulActionId, HUINT32 ulActionPriority)
{
	return (Handle_t)RLIB_MakeResourceRequestHandle(PAL_PIPE_GetRscSessionHandle(), ulActionId, ulActionPriority);
}

HERROR SVC_PIPE_AddRemovableActionIdToRequestHandle (Handle_t hRscReq, HUINT32 ulActionId, HUINT32 ulActionPriority)
{
	return RLIB_AddRemovableActionIdToRequestHandle ((RxRscReqHandle_t)hRscReq, ulActionId, ulActionPriority);
}

HERROR SVC_PIPE_AddIgnorableActionIdToRequestHandle (Handle_t hRscReq, HUINT32 ulActionId)
{
	return RLIB_AddIgnorableActionIdToRequestHandle ((RxRscReqHandle_t)hRscReq, ulActionId);
}

HERROR SVC_PIPE_AddRequiredResourceToRequestHandle (Handle_t hRscReq, RxRscType_e eRscType, HUINT32 ulResourceId, HBOOL bSharable, RxUsageType_e eUsageType)
{
	return RLIB_AddRequiredResourceToRequestHandle ((RxRscReqHandle_t)hRscReq, eRscType, ulResourceId, bSharable, eUsageType);
}

HERROR SVC_PIPE_AddArgumentToRequestHandle (Handle_t hRscReq, RxTrialArg_t *punArg)
{
	return RLIB_AddArgumentToRequestHandle ((RxRscReqHandle_t)hRscReq, punArg);
}

HERROR SVC_PIPE_TryResourceRequest (Handle_t hRscReq)
{
	HERROR		 hErr;

	hErr = RLIB_TryResourceRequest ((RxRscReqHandle_t)hRscReq);
	if (ERR_OK != hErr)
	{
#if defined(CONFIG_DEBUG)
		RLIB_PrintRscInfo ((RxDbgPrintCallback_t)HLIB_CMD_Printf, -1);
#endif
	}

	return hErr;
}

HERROR SVC_PIPE_SetResourcesByTrialResults (Handle_t hRscReq)
{
	return RLIB_SetResourcesByTrialResults ((RxRscReqHandle_t)hRscReq);
}

RxRscTrialResult_t *SVC_PIPE_GetRequestTrialResult (Handle_t hRscReq)
{
	return RLIB_GetRequestTrialResult ((RxRscReqHandle_t)hRscReq);
}

HERROR SVC_PIPE_DestroyResourceRequestHandle (Handle_t hRscReq)
{
	return RLIB_DestroyResourceRequestHandle ((RxRscReqHandle_t)hRscReq);
}

HERROR SVC_PIPE_ClearAction (HUINT32 ulActionId)
{
	HERROR			 hErr, hResult = ERR_OK;

	hErr = PAL_PIPE_ResetPipe (ulActionId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("PAL_PIPE_ResetPipe (ulActionId:%d)\n", ulActionId);
		hResult = ERR_FAIL;
	}

	hErr = RLIB_ClearActionResource(PAL_PIPE_GetRscSessionHandle(), ulActionId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("RLIB_ClearActionResource (ulActionId:%d)\n", ulActionId);
		hResult = ERR_FAIL;
	}

	return hResult;
}

HERROR SVC_PIPE_GetResourceObject (HUINT32 ulActionId, RxRscType_e eRscType, RxRscObject_t *pstRscObj)
{
	return PAL_PIPE_GetResourceObject(ulActionId, eRscType, pstRscObj);
}

/*
	RF-INPUT Function
*/
HERROR SVC_PIPE_GetRfInputAbility (HUINT32 ulRfInputId, HBOOL *pbAbility)
{
	return PAL_PIPE_GetRfInputAbility(ulRfInputId, pbAbility);
}

HERROR SVC_PIPE_SetRfInputAbility (HUINT32 ulRfInputId, HBOOL bAbility)
{
	return PAL_PIPE_SetRfInputAbility(ulRfInputId, bAbility);
}

HERROR SVC_PIPE_GetRfInputCapacity (HUINT32 ulRscId, RxRfInfoCapacity_t *pstCapacity)
{
	return PAL_PIPE_GetRfInputCapacity(ulRscId, pstCapacity);
}

HERROR SVC_PIPE_GetTunerCapacity (HUINT32 ulRscId, DxDeliveryType_e *peSupportedDeliType)
{
	return PAL_PIPE_GetTunerCapacity(ulRscId, peSupportedDeliType);
}

HERROR SVC_PIPE_GetResourceUsageType (RxRscType_e eRscType, HUINT32 ulRscId, RxUsageType_e *peUsageType)
{
	return PAL_PIPE_GetResourceUsageType(eRscType, ulRscId, peUsageType);
}

/* end of file */
