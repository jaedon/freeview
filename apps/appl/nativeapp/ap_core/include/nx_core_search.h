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

/******************************************************************************/
/**
 * @file	  		nx_core_search.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#ifndef __NX_CORE_SEARCH_H__
#define __NX_CORE_SEARCH_H__

/*******************************************************************/
/********************	Header Files		************************/
/*******************************************************************/
#include <ondk.h>
#include <hapi.h>
#include <papi.h>
#include <oapi.h>
#include <apk.h>

#include <nx_common.h>
#include <nx_core_sat_tp_data.h>

#include <nx_core_antenna.h>

/*******************************************************************/
/********************	Extern variables	************************/
/*******************************************************************/


/*******************************************************************/
/********************	Macro Definition	 ***********************/
/*******************************************************************/
// define scantype
#define	NX_SCANTYPE_NORMAL							"normal"
#define	NX_SCANTYPE_ANTENNACONNECTION				"antennaconnection"
#define	NX_SCANTYPE_DISEQCDETECTION					"diseqcdetection"
#define	NX_SCANTYPE_FSATUSERSCAN					"fsatuserscan"
#define	NX_SCANTYPE_TUNEONLY						"tuneonly"
#define	NX_SCANTYPE_FSATPOSTCODESKIP				"fsatpostcodeskip"
#define	NX_SCANTYPE_FSATTUNETWIN					"tunetwin"
#define	NX_SCANTYPE_SESLCNUPDATE					"seslcnupdate"

#define	NX_SCANTYPE_PAUSE							"pause"
#define	NX_SCANTYPE_RESUME							"resume"
#define	NX_SCANTYPE_SAVE							"save"
#define	NX_SCANTYPE_LOAD							"load"
#define	NX_SCANTYPE_SETADDITIONALNUM				"setadditionalnum"
#define	NX_SCANTYPE_LOADDEFCHANNELS					"loadDefChannels"
#define	NX_SCANTYPE_RESET							"reset"

#define	NX_SEARCH_MAX_TP_TUNEONLY					1
#define	NX_SEARCH_TUNER_SINGLE_COUNT				1

#define	NX_SEARCH_MAX_PARAMS_LEN					64
#define	NX_SEARCH_MAX_SYMBOLRATE_LEN				(NX_SEARCH_MAX_PARAMS_LEN * 2)
#define	NX_SEARCH_MAX_INFO_LEN						128

#define	NX_SEARCH_MAX_DATAS_LEN						64
#define	NX_SEARCH_MAX_SYMBOL_LEN					(NX_SEARCH_MAX_DATAS_LEN * 2)
#define	NX_SEARCH_MAX_INFOS_LEN						(NX_SEARCH_MAX_DATAS_LEN * 2)


/*******************************************************************/
/********************		Typedef			************************/
/*******************************************************************/
typedef	enum
{
	eBUILDEREVENT_SCAN,
	eBUILDEREVENT_CONNECTION
} NX_SCANEVENT_e;

typedef	struct
{
	DxSvcType_e		channelType;
	DxCasType_b		casType;
	HBOOL			isReplaceChannel;
	HBOOL			enableNetworkScan;
} NX_SCANOPTIONS_t;

typedef	struct
{
	HINT32		antennaId;
	HCHAR		antennaName[NX_SEARCH_MAX_PARAMS_LEN];
	HINT32		startFreq;
	HINT32		endFreq;
	HINT32		modulationModes;
	HCHAR		symbolRate[NX_SEARCH_MAX_SYMBOLRATE_LEN];
	HINT32		polarisation;
	HCHAR		codeRate[NX_SEARCH_MAX_PARAMS_LEN];
	float		orbitalPosition;
	HINT32		networkId;
} DVBS_SCANPARAMS_t;

typedef	struct
{
	HINT32		startFreq;
	HINT32		endFreq;
	HINT32		raster;
	HBOOL		startNoetworkScanOnNIT;
	HINT32		modulationModes;
	HCHAR		symbolRate[NX_SEARCH_MAX_SYMBOLRATE_LEN];
	HINT32		networkId;
} DVBC_SCANPARAMS_t;

typedef	struct
{
	HINT32		startFreq;
	HINT32		endFreq;
	HCHAR		transmission[NX_SEARCH_MAX_PARAMS_LEN];
	HINT32		raster;

	HCHAR		ofdm[NX_SEARCH_MAX_PARAMS_LEN];
	HINT32		modulationModes;
	HCHAR		bandwidth[NX_SEARCH_MAX_PARAMS_LEN];
} DVBT_SCANPARAMS_t;


typedef	struct
{
	HINT32		tunerType;
	union
	{
		DVBS_SCANPARAMS_t	stDVBSScanParams;
		DVBC_SCANPARAMS_t	stDVBCScanParams;
		DVBT_SCANPARAMS_t	stDVBTScanParams;
	} data;
} NX_SCANPARAMS_t;

typedef	struct
{
	HINT32		channelType;
	HINT32		casType;
	HINT32		lcn;
	HCHAR		name[NX_SEARCH_MAX_INFO_LEN];
} NX_SEARCHEDINFO_t;

typedef	struct
{
	HINT32		type;
	HINT32		signalStrength, signalQuality;
	HUINT32		totalTpNum, completeTpNum, searchedSvcNum;
	HUINT32		frequency;

	HINT32				infoCount;
	NX_SEARCHEDINFO_t	*searchedInfos;
	NX_SCANPARAMS_t	*pstTunedParam;
} NX_SCANEVENT_t;

typedef	struct
{
	HINT32		type;
	HINT32		signalStrength, signalQuality;
	HINT32		progress;
	HUINT32		connType;
} NX_CONNECTIONEVENT_t;


typedef	struct
{
	NX_SCANEVENT_e		eventType;
	union
	{
		NX_SCANEVENT_t			scanEvent;
		NX_CONNECTIONEVENT_t	connectionEvent;
	} u;
} NX_BUILDEREVENT_t;


typedef struct
{
	int 	antennaId;
	char	antennaName[NX_SEARCH_MAX_DATAS_LEN];
	int 	startFreq;
	int 	endFreq;
	int 	modulationModes;
	char	symbolRate[NX_SEARCH_MAX_SYMBOL_LEN];
	int 	polarisation;
	char	codeRate[NX_SEARCH_MAX_DATAS_LEN];
	float	orbitalPosition;
	int 	networkId;
} DVBS_TunedData;

typedef struct
{
	int		tunerType;
	union
	{
		DVBS_TunedData	stDVBSTunedData;

	} data;
} ChannelTuned_Data;


typedef	struct
{
	int		channelType;
	int		casType;
	int		lcn;
	char	name[NX_TEXT_SIZE_128];
} SearchedInfo_Data;


typedef	enum
{
	OPL_TUNER_SAT,
	OPL_TUNER_CAB,
	OPL_TUNER_TER
}NX_TUNERTYPE_e;

typedef	enum
{
	eBUILDER_STARTED = 0,
	eBUILDER_INDICATE_PROGRESS,
	eBUILDER_NEW_CHANNEL_FOUND,
	eBUILDER_NEW_TP_FOUND,
	eBUILDER_COMPLETE,
	eBUILDER_ABORTED
} NX_BUILDER_Status_e;


/*******************************************************************/
/********************	Private Variables		********************/
/*******************************************************************/

/*******************************************************************/
/********************	Global Variables		********************/
/*******************************************************************/
HERROR 	NX_SEARCH_Init(void);
HERROR	NX_SEARCH_GetSearchOption(NX_SCANOPTIONS_t *stScanOpt);
HERROR	NX_SEARCH_SetSearchOption(NX_SCANOPTIONS_t stScanOpt);
HERROR	NX_SEARCH_ConvertSatSvcTuningInfoToChTuningParam (DxDeliveryType_e eDeliverType, DxAntInfo_t *pstAntInfo, NX_TP_TuningInfo_t *pstSvcTuneInfo, NX_SCANPARAMS_t *pstChTuneInfo);
HERROR	NX_SEARCH_ConvertSatDxTunParamToChTuningParam (DxDeliveryType_e eDeliverType, DxTuneParam_t *pstTuneParam, NX_SCANPARAMS_t *pstChTuneInfo);
HERROR	NX_SEARCH_GetSearchTPInfo(HUINT32 *ulTotalTuningNum, HUINT32 *ulCompleteTpNum, HUINT32 *ulSearchedServiceNum, HUINT32 *ulSearchState, HUINT32 *ulFrequency, DxDeliveryType_e *eTunerType, DxTuneParam_t *stTpInfo);
HERROR	NX_SEARCH_GetSearchServiceListInfo(HINT32 lStartIndex, HINT32 lCount, HINT32 *plResultNo, DxBuilder_SearchedInfo_t **ppstSvcInfoList);
HERROR	NX_SEARCH_FreeSearchServiceListInfo(DxBuilder_SearchedInfo_t *pstSvcInfoList);

HERROR 	NX_Builder_StartScan(HCHAR *scanType, NX_SCANOPTIONS_t *pstOptions, HINT32 num, NX_SCANPARAMS_t *pstParams);
HERROR	NX_Builder_StopScan (HCHAR *scanType);
HERROR	NX_Builder_ControlScan(HCHAR *controlScan);

void	NX_Builder_AddNotiSignalInfo(void);
void	NX_Builder_ReleaseNotiSignalInfo(void);

HERROR	NX_ANTINFO_SetInfo(HINT32 uid, NX_AntennaInfo_t *pstAntInfo);

#endif // __NX_CORE_SEARCH_H__


