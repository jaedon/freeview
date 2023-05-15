/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pal_channel.c
	@brief	  Octo pal channel common

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
//#include <mwc_util.h>
#include <pal_channel.h>

#include <di_channel.h>
#if defined(CONFIG_MW_CH_SATELLITE)
#include <di_channel_s.h>
#endif

#if defined(CONFIG_MW_CH_TERRESTRIAL)
#include <di_channel_t.h>
#endif

#if defined(CONFIG_MW_CH_CABLE)
#include <di_channel_c.h>
#endif

#include <di_demux.h>

//#include <db_param.h> // should be removed.
/*******************************************************************/
/************************     Definition   * ***********************/
/*******************************************************************/



#if defined(CONFIG_DEBUG)
#define	FuncEnter			HxLOG_Info("(+)Enter!!\n");
#define	FuncLeave(X)		HxLOG_Info("(-)Leave -- (0x%X)\n",X);
#else
#define	FuncEnter			OCTO_NULL_PRINT
#define	FuncLeave(X)		OCTO_NULL_PRINT
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

/* Tuner context는 튜너 갯수만큼만 필요하지만 max는 action 갯수만큼 잡는다. */
STATIC HUINT32						s_ulNumOfTunerGroup;
STATIC PalCh_EventCallback_t 		s_pfEventCallback;
//STATIC HUINT32						s_SupportDeliveryType[NUM_MAX_ACTION];
STATIC HUINT32						s_SupportDeliveryType[8];




/*******************************************************************/
/******************* Static Function Prototypes  *************************/
/*******************************************************************/

#define ____________LOCAL_FUNC______________

STATIC void pal_ch_DiNotifyCallback(HUINT32 ulInstanceId, HUINT32 ulRequestId, DI_CH_Type_e etChType, DI_CH_SignalInfo_e etSignal)
{
	switch (etSignal)
	{
		case DI_CH_SIGNAL_LOCKED:
			HxLOG_Debug("EVENT from DI : DI_CH_SIGNAL_LOCKED\n");
			s_pfEventCallback(ulInstanceId, ulRequestId, ePAL_CH_EVENT_LOCKED);
			break;

		case DI_CH_SIGNAL_UNLOCK:
			HxLOG_Debug("EVENT from DI : DI_CH_SIGNAL_UNLOCK\n");
			s_pfEventCallback(ulInstanceId, ulRequestId, ePAL_CH_EVENT_NO_SIGNAL);
			break;

		case DI_CH_SIGNAL_ANTENNA_FAULT:
			HxLOG_Debug("EVENT from DI : DI_CH_SIGNAL_ANTENNA_FAULT\n");
			s_pfEventCallback(ulInstanceId, ulRequestId, ePAL_CH_EVENT_ANTENNA_FAULT);
			break;

		case DI_CH_SIGNAL_ANTENNA_OK:
			HxLOG_Debug("EVENT from DI : DI_CH_SIGNAL_ANTENNA_OK\n");
			s_pfEventCallback(ulInstanceId, ulRequestId, ePAL_CH_EVENT_ANTENNA_OK);
			break;

		case DI_CH_SIGNAL_TSMF_GOOD:
			HxLOG_Debug("EVENT from DI : DI_CH_SIGNAL_TSMF_GOOD\n");
			s_pfEventCallback(ulInstanceId, ulRequestId, ePAL_CH_EVENT_TSMF_GOOD);
			break;

		case DI_CH_SIGNAL_TSMF_BAD:
			HxLOG_Debug("EVENT from DI : DI_CH_SIGNAL_TSMF_BAD\n");
			s_pfEventCallback(ulInstanceId, ulRequestId, ePAL_CH_EVENT_TSMF_BAD);
			break;

		case DI_CH_SIGNAL_TSMF_EWS_ON:
			HxLOG_Debug("EVENT from DI : DI_CH_SIGNAL_TSMF_EWS_ON\n");
			s_pfEventCallback(ulInstanceId, ulRequestId, ePAL_CH_EVENT_EWS_ON);
			break;

		case DI_CH_SIGNAL_TSMF_EWS_OFF:
			HxLOG_Debug("EVENT from DI : DI_CH_SIGNAL_TSMF_EWS_OFF\n");
			s_pfEventCallback(ulInstanceId, ulRequestId, ePAL_CH_EVENT_EWS_OFF);
			break;

		default :
			HxLOG_Critical("etSignal is invalid : 0x%X\n",etSignal);
			break;
	}
}

#define ______________EXTERN_API_______________

/*******************************************************************/
/*********************** EXTERN API Functions *************************/
/*******************************************************************/

/* Public Function 마다 필히 기재하도록 함 */


void PAL_CH_Init(void)
{
	HUINT32				ulTunerGroupId;
	DI_CHANNEL_ERR		err_DI;
	HUINT32				ulChType;
#if defined(CONFIG_MW_CH_TERRESTRIAL)
	HUINT32				bTerAntennaPower;
#endif

	FuncEnter;

	DI_CH_GetNumOfInstance(&s_ulNumOfTunerGroup);
	HxLOG_Debug("\n\t*********** Supported Tuner Number : %d ************\n", s_ulNumOfTunerGroup);

	/* 위성이 아닌 나머지 튜너를 뒤쪽에 배치 */
	for(ulTunerGroupId = 0; ulTunerGroupId < s_ulNumOfTunerGroup; ulTunerGroupId++)
	{
		err_DI = DI_CH_GetCapability(ulTunerGroupId, &ulChType);
		if(err_DI != DI_CH_OK)
			HxLOG_Error("DI_CH_GetCapability ret:0x%X\n",err_DI);

#if defined(CONFIG_MW_CH_SATELLITE)
		if( (ulChType & DI_CH_TYPE_SAT) != 0)
		{
			HxLOG_Debug("\t*********** Mapping <<SAT>> to Tuner Context[%d] ************\n", ulTunerGroupId);

			s_SupportDeliveryType[ulTunerGroupId] |= eDxDELIVERY_TYPE_SAT;
			err_DI = DI_CH_S_RegisterCallback(ulTunerGroupId, (pfnDI_CH_NotifySignalInfo)pal_ch_DiNotifyCallback);
			if(err_DI != DI_CH_OK)
				HxLOG_Error("DI_CH_S_RegisterCallback ret:0x%X\n",err_DI);
		}
#endif

#if defined(CONFIG_MW_CH_TERRESTRIAL)
		if((ulChType & DI_CH_TYPE_TER) != 0)
		{
			HERROR	hErr;

			HxLOG_Debug("\t*********** Mapping <<TER>> to Tuner Context[%d] ************\n", ulTunerGroupId);
			s_SupportDeliveryType[ulTunerGroupId] |= eDxDELIVERY_TYPE_TER;
			err_DI = DI_CH_T_RegisterCallback(ulTunerGroupId, (pfnDI_CH_NotifySignalInfo)pal_ch_DiNotifyCallback);
			if(err_DI != DI_CH_OK)			HxLOG_Critical("Critical Eror!!!\n");

			/* Apply antenna power setup */
			hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_TER_SRCH_POWER, (HUINT32 *)&bTerAntennaPower, 0);
			if(ERR_OK == hErr && bTerAntennaPower == TRUE)
			{
				err_DI = DI_CH_T_SetAntennaPower(ulTunerGroupId, DI_CH_ANT_POWER_ON);
			}
			else
			{
				err_DI = DI_CH_T_SetAntennaPower(ulTunerGroupId, DI_CH_ANT_POWER_OFF);
			}
			if(err_DI != DI_CH_OK)
				HxLOG_Error("DI_CH_T_SetAntennaPower ret:0x%X\n",err_DI);
		}
#endif

#if defined(CONFIG_MW_CH_CABLE)
		if((ulChType & DI_CH_TYPE_CAB) != 0)
		{
			HxLOG_Debug("\t*********** Mapping <<CAB>> to Tuner Context[%d] ************\n", ulTunerGroupId);
			s_SupportDeliveryType[ulTunerGroupId] |= eDxDELIVERY_TYPE_CAB;
			err_DI = DI_CH_C_RegisterCallback(ulTunerGroupId, (pfnDI_CH_NotifySignalInfo)pal_ch_DiNotifyCallback);
			if(err_DI != DI_CH_OK)			HxLOG_Critical("Critical Eror!!!\n");
		}
#endif
	}

#if defined(CONFIG_MW_CH_SATELLITE)
	PAL_CH_SAT_Init();
#endif

	FuncLeave(0);
}

void PAL_CH_ActivateDemod(HUINT32 ulTunerGroupId, DxDeliveryType_e deliveryType)
{
	FuncEnter;

	switch(deliveryType)
	{
#if defined(CONFIG_MW_CH_SATELLITE)
		case eDxDELIVERY_TYPE_SAT :
			DI_CH_S_ActivateChannel(ulTunerGroupId);
			break;
#endif

#if defined(CONFIG_MW_CH_TERRESTRIAL)
		case eDxDELIVERY_TYPE_TER :
			DI_CH_T_ActivateChannel(ulTunerGroupId);
			break;
#endif

#if defined(CONFIG_MW_CH_CABLE)
		case eDxDELIVERY_TYPE_CAB :
			DI_CH_C_ActivateChannel(ulTunerGroupId);
			break;
#endif

		default :
			HxLOG_Error("deliveryType is invalid : 0x%X\n",deliveryType);
			break;
	}

	FuncLeave(0);
}

void PAL_CH_DeactivateDemod(HUINT32 ulTunerGroupId, DxDeliveryType_e deliveryType)
{
	FuncEnter;

	switch(deliveryType)
	{
#if defined(CONFIG_MW_CH_SATELLITE)
		case eDxDELIVERY_TYPE_SAT :
			DI_CH_S_DeactivateChannel(ulTunerGroupId);
			break;
#endif

#if defined(CONFIG_MW_CH_TERRESTRIAL)
		case eDxDELIVERY_TYPE_TER :
			DI_CH_T_DeactivateChannel(ulTunerGroupId);
			break;
#endif

#if defined(CONFIG_MW_CH_CABLE)
		case eDxDELIVERY_TYPE_CAB :
			DI_CH_C_DeactivateChannel(ulTunerGroupId);
			break;
#endif

		default :
			HxLOG_Error("deliveryType is invalid : 0x%X\n",deliveryType);
			break;
	}

	FuncLeave(0);
}

void PAL_CH_SetTunerGroupDeliveryType(HUINT32 ulTunerGroupId, DxDeliveryType_e deliveryType)
{
	FuncEnter;

	switch(deliveryType)
	{
#if defined(CONFIG_MW_CH_SATELLITE)
		case eDxDELIVERY_TYPE_SAT :
			DI_DEMUX_BindChannel(ulTunerGroupId, DI_CH_TYPE_SAT);
			break;
#endif

#if defined(CONFIG_MW_CH_TERRESTRIAL)
		case eDxDELIVERY_TYPE_TER :
			DI_DEMUX_BindChannel(ulTunerGroupId, DI_CH_TYPE_TER);
			break;
#endif

#if defined(CONFIG_MW_CH_CABLE)
		case eDxDELIVERY_TYPE_CAB :
			DI_DEMUX_BindChannel(ulTunerGroupId, DI_CH_TYPE_CAB);
			break;
#endif

		default :
			HxLOG_Error("deliveryType is invalid : 0x%X\n",deliveryType);
			break;
	}

	FuncLeave(0);
}

//#if defined(CONFIG_MW_CAS_IRDETO_LOADER_DVT)
void PAL_CH_ConnectDemux(HUINT32 ulTunerGroupId, HUINT32 ulDemuxId, PalCh_DemuxOut_e eDemuxOut, HUINT32 ulOutDeviceId)
{
	DI_ERR_CODE		err;

	FuncEnter;

	switch(eDemuxOut)
	{
		case ePAL_CH_DEMUX_OUT_NONE:
			err = DI_DEMUX_SetPath(ulDemuxId, DI_DEMUX_INPUT_TUNER, ulTunerGroupId, DI_DEMUX_OUTPUT_NONE, 0, -1);
			if(err != DI_ERR_OK)
				HxLOG_Error("DI_DEMUX_SetPath ret : 0x%X\n",err);
			break;

		case ePAL_CH_DEMUX_OUT_AV:
			err = DI_DEMUX_SetPath(ulDemuxId, DI_DEMUX_INPUT_TUNER, ulTunerGroupId, DI_DEMUX_OUTPUT_DECODER, ulOutDeviceId, -1);
			if(err != DI_ERR_OK)
				HxLOG_Error("DI_DEMUX_SetPath ret : 0x%X\n",err);
			break;

		case ePAL_CH_DEMUX_OUT_REC:
			err = DI_DEMUX_SetPath(ulDemuxId, DI_DEMUX_INPUT_TUNER, ulTunerGroupId, DI_DEMUX_OUTPUT_RECORDER, ulOutDeviceId, -1);
			if(err != DI_ERR_OK)
				HxLOG_Error("DI_DEMUX_SetPath ret : 0x%X\n",err);
			break;

#if defined(CONFIG_MW_MM_EXT_RECORD)
		case ePAL_CH_DEMUX_OUT_EXT:
			err = DI_DEMUX_SetPath(ulDemuxId, DI_DEMUX_INPUT_TUNER, ulTunerGroupId, DI_DEMUX_OUTPUT_EXTERNAL, ulOutDeviceId, -1);
			if(err != DI_ERR_OK)
				HxLOG_Error("DI_DEMUX_SetPath ret : 0x%X\n",err);
			break;
#endif

		default:
			HxLOG_Error("eDemuxOut is invalid : 0x%X\n",eDemuxOut);
			break;
	}

	FuncLeave(0);
}

void PAL_CH_DisconnectDemux(HUINT32 ulDemuxId)
{
	DI_ERR_CODE		err;

	FuncEnter;

	err = DI_DEMUX_SetPath (ulDemuxId, DI_DEMUX_INPUT_NONE, 0, DI_DEMUX_OUTPUT_CURRENT, 0, -1);
//	err = DI_DEMUX_ReleasePath (ulDemuxId);

	if(err != DI_ERR_OK)
		HxLOG_Error("DI_DEMUX_SetPath ret : 0x%X\n",err);

	FuncLeave(0);
}
//#endif

void PAL_CH_GetSupportDeliveryType(HUINT32 ulTunerGroupId, HUINT32 *pulSupportDeliveryType)
{
	*pulSupportDeliveryType = s_SupportDeliveryType[ulTunerGroupId];
}

HUINT32 pal_ch_GetNumOfTunerGroup(void)
{
	return s_ulNumOfTunerGroup;
}

HUINT32 PAL_CH_GetNumOfTunerGroup(void)
{
	return pal_ch_GetNumOfTunerGroup();
}


void PAL_CH_RegisterCheckTunerCallback(PalCh_EventCallback_t pfEventCallback)
{
	s_pfEventCallback = pfEventCallback;
}


