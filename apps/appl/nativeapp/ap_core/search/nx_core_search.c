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
 * @file	  		nx_core_search.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#include <nx_core_search.h>
#include <nx_core_message.h>

#define INVALID_BUILDER_HANDLE			(0xFFFF)

static	HUINT32 g_builder_handle = INVALID_BUILDER_HANDLE;
static	int	(*_nx_builder_callback)(void *pstData) = NULL;


static HxList_t*	nx_builder_MakeSatTuningInfos(HxList_t *tune_list, DVBS_SCANPARAMS_t *pstParams);
static void 		nx_builder_SetOptions(NX_SCANOPTIONS_t *pstOptions, DxBuilder_Option_t *pstBuilderOption);
static HxList_t* 	nx_builder_SetScanParams(HINT32 num, NX_SCANPARAMS_t *pstParams, ApkBuilder_Action_t *pstBuilderAction, DxBuilder_Option_t *pstBuilderOption);
static void 		nx_builder_Notifier(HUINT32 ulBuilderId, DxBuilder_Event_e eEvent, DxBuilder_NotifierData_t *pstNotifyData, void *pvUserData);
static void 		nx_builder_NotifierConnection(HUINT32 ulBuilderId, DxBuilder_Event_e eEvent, DxBuilder_NotifierData_t *pstNotifyData, void *pvUserData);
static int 			nx_builder_ConvertTunedParam( DxDeliveryType_e eTunerType, DxTuneParam_t *pstTpInfo, NX_SCANPARAMS_t **ppstOplTunedParam);
static void 		nx_builder_SendEvent(NX_SCANEVENT_e evtType, HINT32 type,
											HINT32 compTp, HINT32 compSvc, HINT32 totalTp,
											HINT32 strength, HINT32 quality, HINT32 infoCount,
											HINT32 frequency, NX_SEARCHEDINFO_t *searchedInfos,
											NX_SCANPARAMS_t *pstTunedParam);
static int			nx_builder_channelConfigEvent(void *pvData);



#if defined(CONFIG_DEBUG)
static void 	nx_search_Command_Init(void);
#endif

/**************************************************************************************************/
#define ___________________Private_functions________________________________________________
/**************************************************************************************************/

HCHAR*		get_int_from_array(HCHAR *str, HINT32 *pnValue)
{
	HCHAR	tempBuf[NX_TEXT_SIZE_32], *p = NULL, *q = NULL;

	HxSTD_MemSet(tempBuf, 0, NX_TEXT_SIZE_32);
	p = tempBuf;

	q = str;
	while (*q)
	{
		if (*q == ',')
		{
			q++;
			break;
		}

		if ((*q >= '0') && (*q <= '9'))
		{
			*p++ = *q;
		}

		q++;
	}

	*pnValue = HLIB_STD_StrToUINT32(tempBuf);

	return q;
}

static	HxList_t*	nx_builder_MakeCabTuningInfos(HxList_t *tune_list, DVBC_SCANPARAMS_t *pstParams)
{
	HCHAR		*str;
	HINT32		symbolRateList[32], symbolRateNo = 0, i;
	HINT32		startfreq, endfreq;
	HxList_t	*list = tune_list;

	DxTuneParam_t	*pstTuningInfo;
	HINT32			raster;
	DxCab_Constellation_e 	eContellation;
	DxCabTuningInfo_t		*pstCabInfo = NULL;
	DVBC_SCANPARAMS_t		*scanParam;

	HxLOG_Trace();

	scanParam = pstParams;

	str = scanParam->symbolRate;
	while (str && *str)
	{
		str = get_int_from_array(str, &symbolRateList[symbolRateNo]);
		if (symbolRateList[symbolRateNo] == 65535000)
		{
			symbolRateList[symbolRateNo] = 0xFFFF;
		}
		else
		{
			symbolRateList[symbolRateNo] = symbolRateList[symbolRateNo] / 1000;
		}
		HxLOG_Debug("symbolRateList : [%d] - [%d]\n", symbolRateNo, symbolRateList[symbolRateNo]);
		symbolRateNo++;
	}

	eContellation = eDxCAB_CONSTELLATION_AUTO;
	switch(scanParam->modulationModes)
	{
		case 4:
			eContellation = eDxCAB_CONSTELLATION_16QAM;
			break;
		case 8:
			eContellation = eDxCAB_CONSTELLATION_32QAM;
			break;
		case 16:
			eContellation = eDxCAB_CONSTELLATION_64QAM;
			break;
		case 32:
			eContellation = eDxCAB_CONSTELLATION_128QAM;
			break;
		case 64:
			eContellation = eDxCAB_CONSTELLATION_256QAM;
			break;
		case 128:
			eContellation = eDxCAB_CONSTELLATION_1024QAM;
			break;
		case 256:
			eContellation = eDxCAB_CONSTELLATION_4096QAM;
			break;
	}

	raster = scanParam->raster;
	startfreq = scanParam->startFreq;
	endfreq   = scanParam->endFreq;

	while (startfreq <= endfreq)
	{
		for (i = 0; i < symbolRateNo; i++)
		{
			pstTuningInfo = (DxTuneParam_t*)NX_APP_Calloc(sizeof(DxTuneParam_t));
			pstTuningInfo->eDeliType = eDxDELIVERY_TYPE_CAB;
			pstCabInfo = &pstTuningInfo->cab;

			pstCabInfo->ulFrequency = startfreq;
			pstCabInfo->ulSymbolRate = symbolRateList[i];
			pstCabInfo->constellation = eContellation;
			pstCabInfo->spectrum = eDxCAB_SPECTRUM_AUTO;
			pstCabInfo->nNetId = pstParams->networkId;

			HxLOG_Debug("OPL-AddTuneInfo [%d, %d, %d]\n", pstCabInfo->ulFrequency, pstCabInfo->ulSymbolRate, pstCabInfo->nNetId);

			list = HLIB_LIST_Append(list, (void*)pstTuningInfo);
		}

		startfreq += raster;

		if (raster == 0)
		{
			break;
		}
	}

	return list;
}


static HxList_t*	nx_builder_MakeSatTuningInfos(HxList_t *tune_list, DVBS_SCANPARAMS_t *pstParams)
{
	HCHAR			*str = NULL;
	HINT32			symbolRateList[32] ={0,}, symbolRateNo = 0, i = 0;
	HINT32			startfreq = 0, endfreq = 0;
	HUINT32 		ulAntId = 0;
	HxList_t		*list = tune_list;
	DxTuneParam_t			*pstTuningInfo = NULL;
	DxSat_CodeRate_e		eCodeRate = eDxSAT_CODERATE_AUTO;
	DxSat_Polarization_e	ePolar = eDxSAT_POLAR_AUTO;
	DxSat_PskModulation_e	eModulation = eDxSAT_PSK_AUTO;
	DVBS_SCANPARAMS_t		*scanParam = NULL;

	HxLOG_Debug("\n++++++++++++++++++ ENTER  MakeSaTTunningInfo +++++++++++++\n");

	HxLOG_Trace();

	scanParam = pstParams;

	str = scanParam->symbolRate;
	while (str && *str)
	{
		str = get_int_from_array(str, &symbolRateList[symbolRateNo]);
		symbolRateList[symbolRateNo] = symbolRateList[symbolRateNo] / 1000;
		symbolRateNo++;
	}

	ulAntId = scanParam->antennaId;

	HxLOG_Debug("symbolRateList : [%d] - [%d, %d, %d]\n", symbolRateNo, symbolRateList[0], symbolRateList[1], symbolRateList[2]);

	eCodeRate = eDxSAT_CODERATE_AUTO;
	if (HxSTD_StrStr(scanParam->codeRate, "1/2"))
		eCodeRate = eDxSAT_CODERATE_1_2;
	else if (HxSTD_StrStr(scanParam->codeRate, "2/3"))
		eCodeRate = eDxSAT_CODERATE_2_3;
	else if (HxSTD_StrStr(scanParam->codeRate, "3/4"))
		eCodeRate = eDxSAT_CODERATE_3_4;
	else if (HxSTD_StrStr(scanParam->codeRate, "5/6"))
		eCodeRate = eDxSAT_CODERATE_5_6;
	else if (HxSTD_StrStr(scanParam->codeRate, "7/8"))
		eCodeRate = eDxSAT_CODERATE_7_8;
	else if (HxSTD_StrStr(scanParam->codeRate, "3/5"))
		eCodeRate = eDxSAT_CODERATE_3_5;
	else if (HxSTD_StrStr(scanParam->codeRate, "4/5"))
		eCodeRate = eDxSAT_CODERATE_4_5;
	else if (HxSTD_StrStr(scanParam->codeRate, "5/11"))
		eCodeRate = eDxSAT_CODERATE_5_11;
	else if (HxSTD_StrStr(scanParam->codeRate, "6/7"))
		eCodeRate = eDxSAT_CODERATE_6_7;
	else if (HxSTD_StrStr(scanParam->codeRate, "8/9"))
		eCodeRate = eDxSAT_CODERATE_8_9;
	else if (HxSTD_StrStr(scanParam->codeRate, "9/10"))
		eCodeRate = eDxSAT_CODERATE_9_10;

	ePolar = 0;
	if (scanParam->polarisation & 0x01)
		ePolar |= eDxSAT_POLAR_HOR;
	if (scanParam->polarisation & 0x02)
		ePolar |= eDxSAT_POLAR_VER;
	if (scanParam->polarisation & 0x04)
		ePolar |= eDxSAT_POLAR_RIGHT;
	if (scanParam->polarisation & 0x08)
		ePolar |= eDxSAT_POLAR_LEFT;

	eModulation = 0;
	if (scanParam->modulationModes & 0x01)
		eModulation |= eDxSAT_PSK_QPSK;
	if (scanParam->modulationModes & 0x02)
		eModulation |= eDxSAT_PSK_8PSK;

	startfreq = scanParam->startFreq / 1000;
	endfreq   = scanParam->endFreq / 1000;

	HxLOG_Debug("\n[%s.%d] startFreq(%d), endFreq(%d), symbolRateNo(%d) \n",
		__FUNCTION__,__LINE__, startfreq, endfreq, symbolRateNo);

	while (startfreq <= endfreq)
	{
		DxSat_TuningTPInfo_t	*pstSatInfo;

		for (i = 0; i < symbolRateNo; i++)
		{
			pstTuningInfo = (DxTuneParam_t*)NX_APP_Calloc(sizeof(DxTuneParam_t));
			pstTuningInfo->eDeliType = eDxDELIVERY_TYPE_SAT;
			pstTuningInfo->sat.deliveryType= eDxDELIVERY_TYPE_SAT;
			pstSatInfo = &pstTuningInfo->sat.tuningInfo;

			pstSatInfo->ulFrequency = startfreq;
			pstSatInfo->ulSymbolRate = symbolRateList[i];
			pstSatInfo->ePolarization = ePolar;
			pstSatInfo->eFecCodeRate = eCodeRate;
			if (eModulation)
			{
				pstSatInfo->eTransSpec = eDxSAT_TRANS_DVBS2;
			}
			else
			{
				pstSatInfo->eTransSpec = eDxSAT_TRANS_DVBS;
			}
			pstSatInfo->ePskMod = eModulation;
			pstSatInfo->ePilot = eDxSAT_PILOT_AUTO;
			pstSatInfo->eRollOff = eDxSAT_ROLL_035;

			pstSatInfo->antuid = ulAntId;

			HxLOG_Debug("AddTuneInfo [%d, %d, %d]\n", pstSatInfo->ulFrequency, pstSatInfo->ulSymbolRate, pstSatInfo->eTransSpec);

			list = HLIB_LIST_Append(list, (void*)pstTuningInfo);
		}
		startfreq += 4;			//	TODO: fec와 SR를 보고 Jump하는 부분 추가 되어야 함.
	}

	return list;
}


static void		nx_builder_SetOptions(NX_SCANOPTIONS_t *pstOptions, DxBuilder_Option_t *pstBuilderOption)
{
	switch (pstOptions->channelType)
	{
		case	eDxSVC_TYPE_TV:
			pstBuilderOption->eSvcType = eDxSVC_TYPE_TV;
			break;
		case	eDxSVC_TYPE_RADIO:
			pstBuilderOption->eSvcType = eDxSVC_TYPE_RADIO;
			break;
		default:
			pstBuilderOption->eSvcType = eDxSVC_TYPE_All;
			break;
	}

	pstBuilderOption->eCasType = pstOptions->casType;
	pstBuilderOption->ulNetworkId = 0;
	pstBuilderOption->bNetworkSearch = pstOptions->enableNetworkScan;
	pstBuilderOption->bReplaceChannel = pstOptions->isReplaceChannel;
	pstBuilderOption->bWithNetworkId = FALSE;

	HxLOG_Debug("\n[%s.%d] eSvcType(%d), casType(%d), ulNetworkId(%d), bNetworkSearch(%d) \n", __FUNCTION__,__LINE__,
		pstBuilderOption->eSvcType, pstBuilderOption->eCasType, pstBuilderOption->ulNetworkId, pstBuilderOption->bNetworkSearch);
}


static HxList_t* nx_builder_SetScanParams(HINT32 num, NX_SCANPARAMS_t *pstParams, ApkBuilder_Action_t *pstBuilderAction, DxBuilder_Option_t *pstBuilderOption)
{
	HINT32		i = 0;
	HxList_t	*tuning_info_list = NULL;
	DxDeliveryType_e		eDeliveryType = eDxDELIVERY_TYPE_SAT;

	if (num == 0)
		num = 1;

	for (i = 0; i < num; i++)
	{
		NX_SCANPARAMS_t	*paramPtr;

		paramPtr = &pstParams[i];

		switch (paramPtr->tunerType)
		{
			case	eDxDELIVERY_TYPE_SAT:
				eDeliveryType = eDxDELIVERY_TYPE_SAT;
				{
					DVBS_SCANPARAMS_t	*scanParams = &paramPtr->data.stDVBSScanParams;

					//	set builder action
					pstBuilderAction->utInfo.satInfo.ulAntId = scanParams->antennaId;
					HxSTD_StrNCpy(pstBuilderAction->utInfo.satInfo.szSatelliteName, scanParams->antennaName, DxBUILDER_SATELLITE_NAME_LEN);

					//	set builder option
					if ((scanParams->networkId) && pstBuilderOption)
					{
						pstBuilderOption->ulNetworkId = scanParams->networkId;
						pstBuilderOption->bWithNetworkId = TRUE;
					}

					//	make tuning infos..
					tuning_info_list = nx_builder_MakeSatTuningInfos(tuning_info_list, scanParams);
				}
				break;

			case	eDxDELIVERY_TYPE_CAB:
				eDeliveryType = eDxDELIVERY_TYPE_CAB;
				{
					DVBC_SCANPARAMS_t	*scanParams = &paramPtr->data.stDVBCScanParams;

					paramPtr->tunerType = eDxDELIVERY_TYPE_CAB;
					HxSTD_StrCpy(scanParams->symbolRate, "\"6875000\"");
					scanParams->startFreq = 602000;
					scanParams->endFreq = 602000;
					scanParams->raster	= 0;
					scanParams->networkId = 0;
					scanParams->startNoetworkScanOnNIT = FALSE;
					scanParams->modulationModes = 0;

					//	set builder option
					if(NULL != pstBuilderOption)
					{
						pstBuilderOption->ulNetworkId = scanParams->networkId;
						if(0 != scanParams->networkId)
						{
							pstBuilderOption->bWithNetworkId = TRUE; // for NIT Oth Based Search
						}
						else
						{
							pstBuilderOption->bWithNetworkId = FALSE; // for NIT Act Based Search
						}
						pstBuilderOption->bNetworkSearch = scanParams->startNoetworkScanOnNIT; // There are two options: NIT Act/Oth Based Search or NO NIT
						if(pstBuilderOption->bNetworkSearch)
						{
							HAPI_SetRegistryInt(DxDB_NETWORK_ID_FOR_NETWORK_SEARCH, pstBuilderOption->ulNetworkId);
						}
					}

					//	make tuning infos..
					tuning_info_list = nx_builder_MakeCabTuningInfos(tuning_info_list, scanParams);
				}
				break;

			default:
				HxLOG_Critical("Not defined Delivery Type!!!\n");
  				break;
		}
	}
	pstBuilderAction->eDeliveryType = eDeliveryType;

	return tuning_info_list;
}


static void		nx_builder_Notifier(HUINT32 ulBuilderId, DxBuilder_Event_e eEvent, DxBuilder_NotifierData_t *pstNotifyData, void *pvUserData)
{
	HERROR		err;
	HINT32		nResultNo, i;
	HINT32		nSignalStrength, nSignalQuality;
	HUINT32		ulTotalTuningNum, ulCompleteTpNum, ulSearchedServiceNum, ulSearchState, ulFrequency;
	DxBuilder_SearchedInfo_t	*pstDetailInfo;
	NX_SEARCHEDINFO_t			*pstSearchedInfo = NULL;
	NX_SCANPARAMS_t			*pstTunedParam = NULL;
	DxDeliveryType_e	eTunerType;
	DxTuneParam_t			 stTpInfo;

	HxLOG_Trace();

	NX_COMMON_NOT_USED_PARAM(ulBuilderId);
	NX_COMMON_NOT_USED_PARAM(eEvent);
	NX_COMMON_NOT_USED_PARAM(pvUserData);
	NX_COMMON_NOT_USED_PARAM(pstNotifyData);

	err = APK_BUILDER_GetBuilderStatus(g_builder_handle, &ulTotalTuningNum, &ulCompleteTpNum, &ulSearchedServiceNum, &ulSearchState, &ulFrequency, &eTunerType, &stTpInfo);
	if (err != ERR_OK)
		goto ERROR;

	err = APK_BUILDER_GetBuilderSignalStatus(g_builder_handle, &nSignalStrength, &nSignalQuality);
	if (err != ERR_OK)
		goto ERROR;

	err = APK_BUILDER_GetBuilderSvcInfo(g_builder_handle, 0, ulSearchedServiceNum, &nResultNo, &pstDetailInfo);
	if (err != ERR_OK)
		goto ERROR;

	if (nResultNo > 0)
	{
		pstSearchedInfo = (NX_SEARCHEDINFO_t*)NX_APP_Calloc(sizeof(NX_SEARCHEDINFO_t) * nResultNo);
		for (i = 0; i < nResultNo; i++)
		{
			pstSearchedInfo[i].channelType	= (HINT32)pstDetailInfo[i].ulSvcType;
			pstSearchedInfo[i].casType		= (HINT32)pstDetailInfo[i].ulCasType;
			pstSearchedInfo[i].lcn			= (HINT32)pstDetailInfo[i].ulLcn;
			HxSTD_StrNCpy(pstSearchedInfo[i].name, (HCHAR *)pstDetailInfo[i].szSvcName, NX_SEARCH_MAX_INFO_LEN - 1);
		}
	}
	if (pstDetailInfo)
	{
		APK_BUILDER_FreeBuilderSvcInfo(pstDetailInfo);
	}

	nx_builder_ConvertTunedParam(eTunerType, &stTpInfo, &pstTunedParam);
	// we don't need check if it(pstTunedParam) is null, it is ok...

	//	TODO: make detail info to Event info.

	NX_APP_Print("\n=====> nx_builder_Notifier() \n");

	if ((ulCompleteTpNum == ulTotalTuningNum) && (ulTotalTuningNum))
	{
		nx_builder_SendEvent(eBUILDEREVENT_SCAN, eBUILDER_COMPLETE, ulCompleteTpNum, ulSearchedServiceNum, ulTotalTuningNum, nSignalStrength, nSignalQuality, nResultNo, ulFrequency, pstSearchedInfo, pstTunedParam);
	}
	else
	{
		nx_builder_SendEvent(eBUILDEREVENT_SCAN, eBUILDER_INDICATE_PROGRESS, ulCompleteTpNum, ulSearchedServiceNum, ulTotalTuningNum, nSignalStrength, nSignalQuality, nResultNo, ulFrequency, pstSearchedInfo, pstTunedParam);
	}
	return;

ERROR:
	HxLOG_Warning("\n\t%s(ERROR!!!!!!!!!!!!!!!!!!)\n", __FUNCTION__);
	nx_builder_SendEvent(eBUILDEREVENT_SCAN, eBUILDER_COMPLETE, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
}


static void		nx_builder_NotifierConnection(HUINT32 ulBuilderId, DxBuilder_Event_e eEvent, DxBuilder_NotifierData_t *pstNotifyData, void *pvUserData)
{
	HERROR				hError = ERR_FAIL;
	HUINT32				ulTotalTuningNum = 0, ulCompleteTpNum = 0, ulSearchedServiceNum = 0, ulSearchState = 0, ulFrequency = 0;
	HINT32				lSignalStrength = 0, lSignalQuality = 0;
	DxDeliveryType_e	eTunerType = eDxDELIVERY_TYPE_ALL;
	DxTuneParam_t		stTpInfo;
	NX_SCANPARAMS_t		*pstTunedParam = NULL;
	HxLOG_Debug("\n[%s.%d] received BuilderNoti CB! ulBuilderID(%u), eEvent(%d)\n",
						__FUNCTION__,__LINE__, ulBuilderId, eEvent);

	NX_COMMON_NOT_USED_PARAM(pstNotifyData);
	NX_COMMON_NOT_USED_PARAM(pvUserData);

	APK_BUILDER_GetBuilderStatus(g_builder_handle, &ulTotalTuningNum, &ulCompleteTpNum, &ulSearchedServiceNum, &ulSearchState, &ulFrequency, &eTunerType, &stTpInfo);
	nx_builder_ConvertTunedParam(eTunerType, &stTpInfo, &pstTunedParam);

	hError = APK_BUILDER_GetBuilderSignalStatus(g_builder_handle, &lSignalStrength, &lSignalQuality);

	NX_APP_Print("\n=====> nx_builder_NotifierConnection() - ulTotalTuningNum:(%d), ulCompleteTpNum:(%d), nSignalStrength:(%u), nSignalQuality:(%d), ConnType:(%d)\n", ulTotalTuningNum, ulCompleteTpNum, lSignalStrength, lSignalQuality, ulSearchedServiceNum);

	if ((ulTotalTuningNum > 0) && (ulCompleteTpNum == ulTotalTuningNum))
	{
		NX_APP_Print("\n\n\n=====> nx_builder_NotifierConnection() - eBUILDER_COMPLETE!! \n");
		nx_builder_SendEvent(eBUILDEREVENT_CONNECTION, eBUILDER_COMPLETE, ulCompleteTpNum, ulSearchedServiceNum, ulTotalTuningNum, lSignalStrength, lSignalQuality, 0, 0, NULL, pstTunedParam);
	}
	else
	{
		NX_APP_Print("\n\n\n=====> nx_builder_NotifierConnection() - eBUILDER_COMPLETE!! \n");
		nx_builder_SendEvent(eBUILDEREVENT_CONNECTION, eBUILDER_INDICATE_PROGRESS, ulCompleteTpNum, ulSearchedServiceNum, ulTotalTuningNum, lSignalStrength, lSignalQuality, 0, 0, NULL, pstTunedParam);
	}
}


static int	nx_builder_ConvertTunedParam( DxDeliveryType_e eTunerType, DxTuneParam_t *pstTpInfo, NX_SCANPARAMS_t **ppstOplTunedParam)
{
	NX_SCANPARAMS_t *pstOplTunedParam;

	if (NULL != ppstOplTunedParam)
	{
		*ppstOplTunedParam = NULL;
	}
	else
	{
		return -1;
	}

	if (eTunerType > eDxDELIVERY_TYPE_CAB) // check valid tuner type..
	{
		return -1;
	}

	pstOplTunedParam = (NX_SCANPARAMS_t*)NX_APP_Calloc(sizeof(NX_SCANPARAMS_t));
	if (pstOplTunedParam)
	{
		pstOplTunedParam->tunerType = eTunerType;
		if (eDxDELIVERY_TYPE_SAT == eTunerType)
		{
			pstOplTunedParam->data.stDVBSScanParams.antennaId = pstTpInfo->sat.tuningInfo.antuid;
			// we have to convert, more detailly...
			pstOplTunedParam->data.stDVBSScanParams.startFreq = pstTpInfo->sat.tuningInfo.ulFrequency;
			pstOplTunedParam->data.stDVBSScanParams.endFreq = pstTpInfo->sat.tuningInfo.ulFrequency;
			pstOplTunedParam->data.stDVBSScanParams.modulationModes = pstTpInfo->sat.tuningInfo.ePskMod;
			pstOplTunedParam->data.stDVBSScanParams.polarisation = pstTpInfo->sat.tuningInfo.ePolarization;

			HxSTD_snprintf(pstOplTunedParam->data.stDVBSScanParams.symbolRate,NX_SEARCH_MAX_SYMBOLRATE_LEN,"%d",(pstTpInfo->sat.tuningInfo.ulSymbolRate));
			switch(pstTpInfo->sat.tuningInfo.eFecCodeRate)
			{
				case eDxSAT_CODERATE_1_2:
					HxSTD_StrNCpy(pstOplTunedParam->data.stDVBSScanParams.codeRate, "1/2", sizeof ("1/2"));
					break;

				case eDxSAT_CODERATE_2_3:
					HxSTD_StrNCpy(pstOplTunedParam->data.stDVBSScanParams.codeRate, "2/3", sizeof ("2/3"));
					break;

				case eDxSAT_CODERATE_3_4:
					HxSTD_StrNCpy(pstOplTunedParam->data.stDVBSScanParams.codeRate, "3/4", sizeof ("3/4"));
					break;

				case eDxSAT_CODERATE_5_6:
					HxSTD_StrNCpy(pstOplTunedParam->data.stDVBSScanParams.codeRate, "5/6", sizeof ("5/6"));
					break;

				case eDxSAT_CODERATE_7_8:
					HxSTD_StrNCpy(pstOplTunedParam->data.stDVBSScanParams.codeRate, "7/8", sizeof ("7/8"));
					break;

				case eDxSAT_CODERATE_3_5:
					HxSTD_StrNCpy(pstOplTunedParam->data.stDVBSScanParams.codeRate, "3/5", sizeof ("3/5"));
					break;

				case eDxSAT_CODERATE_4_5:
					HxSTD_StrNCpy(pstOplTunedParam->data.stDVBSScanParams.codeRate, "4/5", sizeof ("4/5"));
					break;

				case eDxSAT_CODERATE_5_11:
					HxSTD_StrNCpy(pstOplTunedParam->data.stDVBSScanParams.codeRate, "5/11", sizeof ("5/11"));
					break;

				case eDxSAT_CODERATE_6_7:
					HxSTD_StrNCpy(pstOplTunedParam->data.stDVBSScanParams.codeRate, "6/7", sizeof ("6/7"));
					break;

				case eDxSAT_CODERATE_8_9:
					HxSTD_StrNCpy(pstOplTunedParam->data.stDVBSScanParams.codeRate, "8/9", sizeof ("8/9"));
					break;

				case eDxSAT_CODERATE_9_10:
					HxSTD_StrNCpy(pstOplTunedParam->data.stDVBSScanParams.codeRate, "9/10", sizeof ("9/10"));
					break;

				default:
					HxSTD_StrNCpy(pstOplTunedParam->data.stDVBSScanParams.codeRate, "AUTO", sizeof ("AUTO"));
					break;
			}
		}
		else
		{
			NX_APP_Free(pstOplTunedParam);
			goto ERR_OUT;
		}

		*ppstOplTunedParam = pstOplTunedParam;

		return -1;
	}

ERR_OUT:
	return 1;
}


void	nx_builder_SendEvent(NX_SCANEVENT_e evtType, HINT32 type,
							HINT32 compTp, HINT32 compSvc, HINT32 totalTp,
							HINT32 strength, HINT32 quality, HINT32 infoCount,
							HINT32 frequency, NX_SEARCHEDINFO_t *searchedInfos,
							NX_SCANPARAMS_t *pstTunedParam)
{
	HxLOG_Debug("\n[%s.%d] received: evtType(%d), totalTp(%d), strength(%d)\n\n",
							__FUNCTION__,__LINE__, evtType, totalTp, strength);
	if (_nx_builder_callback)
	{
		NX_BUILDEREVENT_t	stScanEvent;

		HxSTD_MemSet(&stScanEvent, 0, sizeof(NX_BUILDEREVENT_t));
		stScanEvent.eventType = evtType;

		switch (evtType)
		{
			case	eBUILDEREVENT_SCAN:
				NX_APP_Print("\n=====> nx_builder_SendEvent() - eBUILDEREVENT_SCAN \n");
				stScanEvent.u.scanEvent.type = type;
				stScanEvent.u.scanEvent.completeTpNum = compTp;
				stScanEvent.u.scanEvent.searchedSvcNum = compSvc;
				stScanEvent.u.scanEvent.totalTpNum = totalTp;
				stScanEvent.u.scanEvent.signalStrength = strength;
				stScanEvent.u.scanEvent.signalQuality = quality;
				stScanEvent.u.scanEvent.frequency= frequency;

				stScanEvent.u.scanEvent.infoCount = infoCount;
				stScanEvent.u.scanEvent.searchedInfos = searchedInfos;
				stScanEvent.u.scanEvent.pstTunedParam = pstTunedParam;
				break;

			case	eBUILDEREVENT_CONNECTION:
				NX_APP_Print("\n=====> nx_builder_SendEvent() - eBUILDEREVENT_CONNECTION \n");
				stScanEvent.u.connectionEvent.type = type;
				stScanEvent.u.connectionEvent.connType = compSvc;//use this for searched channel num on diseqcdetection

				if (totalTp > 0)
					stScanEvent.u.connectionEvent.progress = (compTp * 100)/totalTp;
				else
					stScanEvent.u.connectionEvent.progress = 0;

				stScanEvent.u.connectionEvent.signalStrength = strength;
				stScanEvent.u.connectionEvent.signalQuality = quality;
				break;

			default:
				NX_APP_Error("Error!!! Unknow Event Type-(%d)\n", evtType);
				break;
		}

		_nx_builder_callback((void*)&stScanEvent);
    }
}


static int	nx_builder_channelConfigEvent(void *pvData)
{
	HINT32					p1 = 0, p2 = 0, p3 = 0;
	const HCHAR				*szScanType = NULL;
	NX_BUILDEREVENT_t		*pstEvent = (NX_BUILDEREVENT_t*)pvData;
#if defined(CONFIG_DEBUG)
	HINT32					i;
	SearchedInfo_Data		*searchedInfos = NULL;
#endif

	if (g_builder_handle == INVALID_BUILDER_HANDLE)
	{
		NX_APP_Error("Builder is already destroyed..\nThe Function will be ignored...\n");
		return	ERR_FAIL;
	}

	switch (pstEvent->eventType)
	{
		case	eBUILDEREVENT_SCAN:
			if (pstEvent->u.scanEvent.totalTpNum)
			{
				HxLOG_Debug("\n[%s.%d] #### :: totalTpNum (%d),  progress (%d)\n", __FUNCTION__,__LINE__,
					pstEvent->u.scanEvent.totalTpNum, ((pstEvent->u.scanEvent.completeTpNum * 100) / pstEvent->u.scanEvent.totalTpNum));
			}
			else
			{
				HxLOG_Debug("\n[%s.%d] total num is <=0 (%d) \n\n",__FUNCTION__,__LINE__, pstEvent->u.scanEvent.totalTpNum);
			}

			HxLOG_Debug("[%s.%d] scanevent: frequency(%d), signalStrength(%d), searchedSvcNum (%d), completeTpNum (%d)\n",
				__FUNCTION__,__LINE__, pstEvent->u.scanEvent.frequency,	pstEvent->u.scanEvent.signalStrength,
				pstEvent->u.scanEvent.searchedSvcNum, pstEvent->u.scanEvent.completeTpNum);

			HxLOG_Debug("[%s.%d] inCount (%d) \n",__FUNCTION__,__LINE__, pstEvent->u.scanEvent.infoCount);

#if defined(CONFIG_DEBUG)
			if (pstEvent->u.scanEvent.infoCount > 0)
			{
				searchedInfos = (SearchedInfo_Data*)NX_APP_Calloc(sizeof(SearchedInfo_Data) * pstEvent->u.scanEvent.infoCount);
				HxSTD_memset(searchedInfos, 0, sizeof(SearchedInfo_Data));
				for (i = 0; i < pstEvent->u.scanEvent.infoCount; i++)
				{
					SearchedInfo_Data	*info = &searchedInfos[i];

					info->channelType	= pstEvent->u.scanEvent.searchedInfos[i].channelType;
					info->casType		= pstEvent->u.scanEvent.searchedInfos[i].casType;
					info->lcn			= pstEvent->u.scanEvent.searchedInfos[i].lcn;
					HxSTD_StrNCpy(info->name, pstEvent->u.scanEvent.searchedInfos[i].name, (128 - 1));

					HxLOG_Debug("\n[%s.%d] i(%d) , info: channelType(%d), casType(%d), lcn(%d), name(%s)\n",
						__FUNCTION__,__LINE__, i, info->channelType, info->casType, info->lcn, info->name);
				}
			}
			if (NULL != pstEvent->u.scanEvent.pstTunedParam)
			{
				ChannelTuned_Data *pTmpInfo = NULL;

				pTmpInfo = (ChannelTuned_Data*)NX_APP_Calloc(sizeof(ChannelTuned_Data));
				if (pTmpInfo)
				{
					HxSTD_memset(pTmpInfo, 0, sizeof(ChannelTuned_Data));
					pTmpInfo->tunerType = pstEvent->u.scanEvent.pstTunedParam->tunerType;

					if (eDxDELIVERY_TYPE_SAT == pstEvent->u.scanEvent.pstTunedParam->tunerType) 				// SAT
					{
						pTmpInfo->data.stDVBSTunedData.antennaId		= pstEvent->u.scanEvent.pstTunedParam->data.stDVBSScanParams.antennaId;
						pTmpInfo->data.stDVBSTunedData.startFreq		= pstEvent->u.scanEvent.pstTunedParam->data.stDVBSScanParams.startFreq;
						pTmpInfo->data.stDVBSTunedData.endFreq			= pstEvent->u.scanEvent.pstTunedParam->data.stDVBSScanParams.endFreq;
						pTmpInfo->data.stDVBSTunedData.modulationModes	= pstEvent->u.scanEvent.pstTunedParam->data.stDVBSScanParams.modulationModes;
						pTmpInfo->data.stDVBSTunedData.polarisation		= pstEvent->u.scanEvent.pstTunedParam->data.stDVBSScanParams.polarisation;
						pTmpInfo->data.stDVBSTunedData.orbitalPosition	= pstEvent->u.scanEvent.pstTunedParam->data.stDVBSScanParams.orbitalPosition;
						pTmpInfo->data.stDVBSTunedData.networkId		= pstEvent->u.scanEvent.pstTunedParam->data.stDVBSScanParams.networkId;

						HxSTD_StrNCpy(pTmpInfo->data.stDVBSTunedData.antennaName, pstEvent->u.scanEvent.pstTunedParam->data.stDVBSScanParams.antennaName, (NX_SEARCH_MAX_DATAS_LEN-1));
						HxSTD_StrNCpy(pTmpInfo->data.stDVBSTunedData.symbolRate, pstEvent->u.scanEvent.pstTunedParam->data.stDVBSScanParams.symbolRate, (NX_SEARCH_MAX_SYMBOL_LEN-1));
						HxSTD_StrNCpy(pTmpInfo->data.stDVBSTunedData.codeRate, pstEvent->u.scanEvent.pstTunedParam->data.stDVBSScanParams.codeRate, (NX_SEARCH_MAX_DATAS_LEN-1));

						HxLOG_Debug("[%s.%d] TuneInfo : antennaID(%d), startFreq(%d), endFreq(%d), anttenaName:(%s)\n",	__FUNCTION__,__LINE__, pTmpInfo->data.stDVBSTunedData.antennaId,
							pTmpInfo->data.stDVBSTunedData.startFreq, pTmpInfo->data.stDVBSTunedData.endFreq, pTmpInfo->data.stDVBSTunedData.antennaName);
					}
					else
					{
						// TODO: Implement Ter(eChDeliveryType_TER) & CAB(eChDeliveryType_CAB)
						NX_APP_Free(pTmpInfo);
						pTmpInfo = NULL;
					}
				}

				if (pTmpInfo)
				{
					NX_APP_Free(pTmpInfo);
					pTmpInfo = NULL;
				}
			}

			if (pstEvent->u.scanEvent.infoCount > 0)
			{
				if (searchedInfos)
					NX_APP_Free(searchedInfos);
			}
#endif
			if (NULL != pstEvent->u.scanEvent.pstTunedParam)
			{
				NX_APP_Free(pstEvent->u.scanEvent.pstTunedParam);
				pstEvent->u.scanEvent.pstTunedParam = NULL;
			}
			if (NULL != pstEvent->u.scanEvent.searchedInfos)
			{
				NX_APP_Free(pstEvent->u.scanEvent.searchedInfos);
				pstEvent->u.scanEvent.searchedInfos = NULL;
			}

			p1 = pstEvent->u.scanEvent.type;
			NX_APP_Print("\n=====> MSG_CORE_SERVICE_EVT_SEARCH_INFO_UPDATE - p1:(%d), p2:(%d), p3:(%d)\n", p1, p2, p3);
			ONDK_GWM_PostMessage(NULL, MSG_CORE_SERVICE_EVT_SEARCH_INFO_UPDATE, 0, p1, 0, 0);
			break;

		case	eBUILDEREVENT_CONNECTION:
			szScanType = APK_BUILDER_GetScanType(g_builder_handle);
			NX_APP_Print(HxANSI_COLOR_YELLOW("scanType = %s\n"), szScanType);
			if (HLIB_STD_StrCaseCmp((HCHAR*)szScanType, (HCHAR*)NX_SCANTYPE_ANTENNACONNECTION) == 0)
			{
				p1 = pstEvent->u.connectionEvent.type;
				p2 = pstEvent->u.connectionEvent.progress;
				p3 = pstEvent->u.connectionEvent.connType;

				NX_APP_Print("\n=====> MSG_CORE_SERVICE_EVT_ANTENNA_CONNECTION - p1:(%d), p2:(%d), p3:(%d)\n", p1, p2, p3);
				ONDK_GWM_PostMessage(NULL, MSG_CORE_SERVICE_EVT_ANTENNA_CONNECTION, 0, p1, p2, p3);
			}
			if (HLIB_STD_StrCaseCmp((HCHAR*)szScanType, (HCHAR*)NX_SCANTYPE_DISEQCDETECTION) == 0)
            {
                p1 = pstEvent->u.connectionEvent.type;
				p2 = pstEvent->u.connectionEvent.progress;
				p3 = pstEvent->u.connectionEvent.connType; //searched service number

                //info->channel_scan_data.channelNumber
				ONDK_GWM_PostMessage(NULL, MSG_CORE_SERVICE_EVT_DISEQCDETECTION, 0, p1, p2, p3);
            }
			else
			{
				p1 = pstEvent->u.connectionEvent.type;
				p2 = pstEvent->u.connectionEvent.signalStrength;
				p3 = pstEvent->u.connectionEvent.signalQuality;

				NX_APP_Print("\n=====> MSG_CORE_SERVICE_EVT_SIGNAL_STATE - p1:(%d), p2:(%d), p3:(%d)\n", p1, p2, p3);
				ONDK_GWM_PostMessage(NULL, MSG_CORE_SERVICE_EVT_SIGNAL_STATE, 0, p1, p2, p3);
			}
			break;
		default:
			break;
	}

	return 0;
}


/**************************************************************************************************/
#define ___________________Public_functions________________________________________________
/**************************************************************************************************/
HERROR	NX_SEARCH_Init(void)
{
#if defined(CONFIG_DEBUG)
	nx_search_Command_Init();
#endif

	return ERR_OK;
}


HERROR	NX_SEARCH_GetSearchOption(NX_SCANOPTIONS_t *stScanOpt)
{
	HINT32		lValue = -1;
	HERROR		hError = ERR_FAIL;

	if (stScanOpt == NULL)
	{
		NX_APP_Error("[%s:%d] Error!! stScanOpt param is nULL\n", __FUNCTION__, __LINE__);
		return	ERR_FAIL;
	}

	hError = NX_PORT_DB_GetInt(USERDB_SEARCH_OPT_NETWORK_SEARCH, &lValue);
	if (hError != ERR_OK)
	{
		NX_APP_Debug("[%s:%d] Fail!!! USERDB_SEARCH_OPT_NETWORK_SEARCH\n", __FUNCTION__, __LINE__);
		lValue = (HINT32)TRUE;
	}
	stScanOpt->enableNetworkScan = (HBOOL)lValue;

	hError = NX_PORT_DB_GetInt(USERDB_SEARCH_OPT_SCRAMBLE_TYPE, &lValue);
	if (hError != ERR_OK)
	{
		NX_APP_Debug("[%s:%d] Fail!!! USERDB_SEARCH_OPT_SCRAMBLE_TYPE\n", __FUNCTION__, __LINE__);
		lValue = (HINT32)eDxCAS_TYPE_All;
	}
	stScanOpt->casType = (DxCasType_b)lValue;


	hError = NX_PORT_DB_GetInt(USERDB_SEARCH_OPT_CHANNEL_TYPE, &lValue);
	if (hError != ERR_OK)
	{
		NX_APP_Debug("[%s:%d] Fail!!! USERDB_SEARCH_OPT_CHANNEL_TYPE\n", __FUNCTION__, __LINE__);
		lValue = (HINT32)eDxSVC_TYPE_All;
	}
	stScanOpt->channelType = (DxSvcType_e)lValue;

	return	ERR_OK;
}


HERROR	NX_SEARCH_SetSearchOption(NX_SCANOPTIONS_t stScanOpt)
{
	HERROR		hError = ERR_FAIL;


	hError = NX_PORT_DB_SetInt(USERDB_SEARCH_OPT_NETWORK_SEARCH, stScanOpt.enableNetworkScan);
	if (hError != ERR_OK)
	{
		NX_APP_Debug("[%s:%d] Fail!!! USERDB_SEARCH_OPT_NETWORK_SEARCH\n", __FUNCTION__, __LINE__);
	}

	hError = NX_PORT_DB_SetInt(USERDB_SEARCH_OPT_NETWORK_SEARCH, stScanOpt.casType);
	if (hError != ERR_OK)
	{
		NX_APP_Debug("[%s:%d] Fail!!! USERDB_SEARCH_OPT_NETWORK_SEARCH\n", __FUNCTION__, __LINE__);
	}

	hError = NX_PORT_DB_SetInt(USERDB_SEARCH_OPT_CHANNEL_TYPE, stScanOpt.channelType);
	if (hError != ERR_OK)
	{
		NX_APP_Debug("[%s:%d] Fail!!! USERDB_SEARCH_OPT_CHANNEL_TYPE\n", __FUNCTION__, __LINE__);
	}

	return	ERR_OK;
}


HERROR	NX_SEARCH_ConvertSatSvcTuningInfoToChTuningParam (DxDeliveryType_e eDeliverType, DxAntInfo_t *pstAntInfo, NX_TP_TuningInfo_t *pstSvcTuneInfo, NX_SCANPARAMS_t *pstChTuneInfo)
{
	pstChTuneInfo->tunerType = eDeliverType;
	if (eDxDELIVERY_TYPE_SAT == eDeliverType)
	{
		pstChTuneInfo->data.stDVBSScanParams.antennaId = pstAntInfo->uid;
		HxSTD_snprintf(pstChTuneInfo->data.stDVBSScanParams.antennaName, NX_SEARCH_MAX_PARAMS_LEN, "%s", (pstAntInfo->satType));
		pstChTuneInfo->data.stDVBSScanParams.startFreq = pstSvcTuneInfo->sat.ulFrequency;
		pstChTuneInfo->data.stDVBSScanParams.endFreq = pstSvcTuneInfo->sat.ulFrequency;
		pstChTuneInfo->data.stDVBSScanParams.modulationModes = pstSvcTuneInfo->sat.ePskMod;
		if (pstSvcTuneInfo->sat.eTransSpec == eDxSAT_TRANS_DVBS)
		{
			// TODO: Check !!! 왜 이렇게 해야 하나요? 음.....
			pstChTuneInfo->data.stDVBSScanParams.modulationModes = eDxSAT_PSK_AUTO;
		}
		pstChTuneInfo->data.stDVBSScanParams.polarisation = pstSvcTuneInfo->sat.ePolarization;
		pstChTuneInfo->data.stDVBSScanParams.networkId = 0;

		HxSTD_snprintf(pstChTuneInfo->data.stDVBSScanParams.symbolRate, NX_SEARCH_MAX_SYMBOLRATE_LEN, "%d", (pstSvcTuneInfo->sat.ulSymbolRate));
		switch (pstSvcTuneInfo->sat.eFecCodeRate)
		{
			case eDxSAT_CODERATE_1_2:
				HxSTD_StrNCpy(pstChTuneInfo->data.stDVBSScanParams.codeRate, "1/2", sizeof ("1/2"));
				break;

			case eDxSAT_CODERATE_2_3:
				HxSTD_StrNCpy(pstChTuneInfo->data.stDVBSScanParams.codeRate, "2/3", sizeof ("2/3"));
				break;

			case eDxSAT_CODERATE_3_4:
				HxSTD_StrNCpy(pstChTuneInfo->data.stDVBSScanParams.codeRate, "3/4", sizeof ("3/4"));
				break;

			case eDxSAT_CODERATE_5_6:
				HxSTD_StrNCpy(pstChTuneInfo->data.stDVBSScanParams.codeRate, "5/6", sizeof ("5/6"));
				break;

			case eDxSAT_CODERATE_7_8:
				HxSTD_StrNCpy(pstChTuneInfo->data.stDVBSScanParams.codeRate, "7/8", sizeof ("7/8"));
				break;

			case eDxSAT_CODERATE_3_5:
				HxSTD_StrNCpy(pstChTuneInfo->data.stDVBSScanParams.codeRate, "3/5", sizeof ("3/5"));
				break;

			case eDxSAT_CODERATE_4_5:
				HxSTD_StrNCpy(pstChTuneInfo->data.stDVBSScanParams.codeRate, "4/5", sizeof ("4/5"));
				break;

			case eDxSAT_CODERATE_5_11:
				HxSTD_StrNCpy(pstChTuneInfo->data.stDVBSScanParams.codeRate, "5/11", sizeof ("5/11"));
				break;

			case eDxSAT_CODERATE_6_7:
				HxSTD_StrNCpy(pstChTuneInfo->data.stDVBSScanParams.codeRate, "6/7", sizeof ("6/7"));
				break;

			case eDxSAT_CODERATE_8_9:
				HxSTD_StrNCpy(pstChTuneInfo->data.stDVBSScanParams.codeRate, "8/9", sizeof ("8/9"));
				break;

			case eDxSAT_CODERATE_9_10:
				HxSTD_StrNCpy(pstChTuneInfo->data.stDVBSScanParams.codeRate, "9/10", sizeof ("9/10"));
				break;

			default:
				HxSTD_StrNCpy(pstChTuneInfo->data.stDVBSScanParams.codeRate, "AUTO", sizeof ("AUTO"));
				break;
		}
	}
	else if (eDxDELIVERY_TYPE_CAB == eDeliverType)
	{
		// TODO: Add Data
	}
	else if (eDxDELIVERY_TYPE_TER == eDeliverType)
	{
		// TODO: Add Data
	}
	else
	{
		// TODO: Add Data
	}

	return	ERR_OK;
}


HERROR	NX_SEARCH_ConvertSatDxTunParamToChTuningParam (DxDeliveryType_e eDeliverType, DxTuneParam_t *pstTuneParam, NX_SCANPARAMS_t *pstChTuneInfo)
{
	pstChTuneInfo->tunerType = eDeliverType;

	if (eDxDELIVERY_TYPE_SAT == eDeliverType)
	{
		pstChTuneInfo->data.stDVBSScanParams.antennaId = pstTuneParam->sat.antInfo.uid;
		HxSTD_snprintf(pstChTuneInfo->data.stDVBSScanParams.antennaName, NX_SEARCH_MAX_PARAMS_LEN, "%s", (pstTuneParam->sat.antInfo.satType));
		pstChTuneInfo->data.stDVBSScanParams.startFreq = pstTuneParam->sat.tuningInfo.ulFrequency;
		pstChTuneInfo->data.stDVBSScanParams.endFreq = pstTuneParam->sat.tuningInfo.ulFrequency;
		pstChTuneInfo->data.stDVBSScanParams.modulationModes = pstTuneParam->sat.tuningInfo.ePskMod;
		if (pstTuneParam->sat.tuningInfo.eTransSpec == eDxSAT_TRANS_DVBS)
		{
			// TODO: Check !!! 왜 이렇게 해야 하나요? 음.....
			pstChTuneInfo->data.stDVBSScanParams.modulationModes = eDxSAT_PSK_AUTO;
		}
		pstChTuneInfo->data.stDVBSScanParams.polarisation = pstTuneParam->sat.tuningInfo.ePolarization;
		pstChTuneInfo->data.stDVBSScanParams.networkId = 0;

		HxSTD_snprintf(pstChTuneInfo->data.stDVBSScanParams.symbolRate, NX_SEARCH_MAX_SYMBOLRATE_LEN, "%d", (pstTuneParam->sat.tuningInfo.ulSymbolRate));
	}
	else if (eDxDELIVERY_TYPE_CAB == eDeliverType)
	{
		// TODO: Add Data
	}
	else if (eDxDELIVERY_TYPE_TER == eDeliverType)
	{
		// TODO: Add Data
	}
	else
	{
		// TODO: Add Data
	}

	return	ERR_OK;
}


HERROR	NX_SEARCH_GetSearchTPInfo(HUINT32 *ulTotalTuningNum, HUINT32 *ulCompleteTpNum, HUINT32 *ulSearchedServiceNum, HUINT32 *ulSearchState, HUINT32 *ulFrequency, DxDeliveryType_e *eTunerType, DxTuneParam_t *stTpInfo)
{
	HERROR	hError = ERR_FAIL;

	hError = APK_BUILDER_GetBuilderStatus(g_builder_handle, ulTotalTuningNum, ulCompleteTpNum, ulSearchedServiceNum, ulSearchState, ulFrequency, eTunerType, stTpInfo);

	return	hError;
}


HERROR	NX_SEARCH_GetSearchServiceListInfo(HINT32 lStartIndex, HINT32 lCount, HINT32 *plResultNo, DxBuilder_SearchedInfo_t **ppstSvcInfoList)
{
	HERROR	hError = ERR_FAIL;

	hError = APK_BUILDER_GetBuilderSvcInfo(g_builder_handle, lStartIndex, lCount, plResultNo, ppstSvcInfoList);

	return	hError;
}


HERROR	NX_SEARCH_FreeSearchServiceListInfo(DxBuilder_SearchedInfo_t *pstSvcInfoList)
{
	return	APK_BUILDER_FreeBuilderSvcInfo (pstSvcInfoList);
}


void	NX_Builder_AddNotiSignalInfo(void)
{
	_nx_builder_callback = nx_builder_channelConfigEvent;
}


void	NX_Builder_ReleaseNotiSignalInfo(void)
{
	_nx_builder_callback = NULL;
}


HERROR NX_Builder_StartScan(HCHAR *scanType, NX_SCANOPTIONS_t *pstOptions, HINT32 num, NX_SCANPARAMS_t *pstParams)
{
	HUINT32 	handle = INVALID_BUILDER_HANDLE;
	HxList_t	*list;
	HBOOL		bUserScanOption;

	ApkBuilder_Action_t		stBuilderAction;
	DxBuilder_Option_t		stBuilderOption;

	#if defined(CONFIG_PROD_OS_EMULATOR) && !defined(CONFIG_SINGLE_PROCESS_MULTI_THREAD)
		ONDK_GWM_PostMessage(NULL, MSG_CORE_SERVICE_EVT_DISEQCDETECTION, 0, eDxANT_TYPE_LNB_ONLY, 100, 0);
        return ERR_OK;
    #endif

	bUserScanOption = FALSE;
	if ((HLIB_STD_StrCaseCmp(scanType, NX_SCANTYPE_NORMAL) == 0) ||
		(HLIB_STD_StrCaseCmp(scanType, NX_SCANTYPE_FSATUSERSCAN) == 0) ||
		(HLIB_STD_StrCaseCmp(scanType, NX_SCANTYPE_FSATPOSTCODESKIP) == 0))
	{
		bUserScanOption = TRUE;
	}

	HxLOG_Debug("[%s.%d] num:(%d) ,scanType : %s \n",__FUNCTION__,__LINE__, num, scanType);

	#if 0
	{
		int i = 0;
		for(i = 0; i < num; i++)
		{
			HxLOG_Debug("\n[%s.%d][%d] tunerType(%d) ,antennaID(%d), name(%s), codeRate(%s), endFreq(%d), moduleMode(%d),networkID(%d),position(%f), polar(%d), startFreq(%d),)\n",
				__FUNCTION__,__LINE__, i,
					pstParams[i].tunerType,//->tunerType,
					pstParams[i].data.stDVBSScanParams.antennaId,
					pstParams[i].data.stDVBSScanParams.antennaName,
					pstParams[i].data.stDVBSScanParams.codeRate,
					pstParams[i].data.stDVBSScanParams.endFreq,
					pstParams[i].data.stDVBSScanParams.modulationModes,
					pstParams[i].data.stDVBSScanParams.networkId,
					pstParams[i].data.stDVBSScanParams.orbitalPosition,
					pstParams[i].data.stDVBSScanParams.polarisation,
					pstParams[i].data.stDVBSScanParams.startFreq);
		}
	}
	#endif

	if (bUserScanOption)
	{
		HxLOG_Debug("[%s.%d] casType : %d, channelType(%d), enableNetworkScan(%d), isReplaceChannel(%d) \n",
		__FUNCTION__,__LINE__, pstOptions->casType,	pstOptions->channelType, pstOptions->enableNetworkScan,	pstOptions->isReplaceChannel);
		nx_builder_SetOptions(pstOptions, &stBuilderOption);

		HxLOG_Debug("[%s.%d] num(%d), bNetworkSearch(%d), replaceChannel(%d), bWithNetworkId(%d), casType(%d), svcType(%d), ulNetworkId(%u)\n",
			__FUNCTION__,__LINE__, num,
			stBuilderOption.bNetworkSearch,
			stBuilderOption.bReplaceChannel,
			stBuilderOption.bWithNetworkId,
			stBuilderOption.eCasType,
			stBuilderOption.eSvcType,
			stBuilderOption.ulNetworkId);

		list = nx_builder_SetScanParams(num, pstParams, &stBuilderAction, &stBuilderOption);
	}
	else
	{
		list = nx_builder_SetScanParams(num, pstParams, &stBuilderAction, NULL);
	}

	APK_BUILDER_OpenSession(stBuilderAction.eDeliveryType, scanType, &stBuilderAction, &handle);
	if (handle == (0xFFFF))
	{
		HxLOG_Critical("other builder action is now running...\n");
		return  ERR_FAIL;
	}

	g_builder_handle = handle;
	if (bUserScanOption)
	{
		APK_BUILDER_SetOption(g_builder_handle, &stBuilderOption);

		HxLOG_Debug("[%s.%d] bNetworksearch(%d), replaceChannel(%d), bWithNet(%d), casType(%d), svcType(%d), ulNetworkID(%d) \n",
			__FUNCTION__,__LINE__,
			stBuilderOption.bNetworkSearch,
			stBuilderOption.bReplaceChannel,
			stBuilderOption.bWithNetworkId,
			stBuilderOption.eCasType,
			stBuilderOption.eSvcType,
			stBuilderOption.ulNetworkId);
	}

	while (list)
	{
		DxTuneParam_t	*pstInfo = (DxTuneParam_t*)HLIB_LIST_Data(list);

		APK_BUILDER_AddBuildInfo(g_builder_handle, pstInfo);
		NX_APP_Free(pstInfo);

		list = list->next;
	}
	HLIB_LIST_RemoveAll(list);

	if ((HLIB_STD_StrCaseCmp(scanType, NX_SCANTYPE_NORMAL) == 0) ||
		(HLIB_STD_StrCaseCmp(scanType, NX_SCANTYPE_FSATUSERSCAN) == 0) ||
		(HLIB_STD_StrCaseCmp(scanType, NX_SCANTYPE_FSATPOSTCODESKIP) == 0))
	{
		APK_BUILDER_RegisterNotifier(g_builder_handle, nx_builder_Notifier, NULL);
	}
	else
	{
		APK_BUILDER_RegisterNotifier(g_builder_handle, nx_builder_NotifierConnection, NULL);
	}

	NX_APP_Print("\n=====> APK_BUILDER_StartScan() \n");
	APK_BUILDER_StartScan(g_builder_handle);

	return ERR_OK;
}


HERROR	NX_Builder_StopScan (HCHAR *scanType)
{
	HERROR		hError = ERR_FAIL;

	if ((HLIB_STD_StrCaseCmp(scanType, NX_SCANTYPE_NORMAL) == 0) ||
		(HLIB_STD_StrCaseCmp(scanType, NX_SCANTYPE_FSATUSERSCAN) == 0) ||
		(HLIB_STD_StrCaseCmp(scanType, NX_SCANTYPE_FSATPOSTCODESKIP) == 0))
	{
		APK_BUILDER_UnregisterNotifier(g_builder_handle, nx_builder_Notifier, NULL);
	}
	else
	{
		APK_BUILDER_UnregisterNotifier(g_builder_handle, nx_builder_NotifierConnection, NULL);
	}

	hError = APK_BUILDER_StopScan(g_builder_handle);
	if (ERR_OK != hError)
	{
		HxLOG_Error("APK_BUILDER_StopScan err: hBuilderHandle(0x%08x)\n", g_builder_handle);
	}

	hError = APK_BUILDER_ClearBuildInfo(g_builder_handle);
	if (ERR_OK != hError)
	{
		HxLOG_Error("APK_BUILDER_StopScan err: hBuilderHandle(0x%08x)\n", g_builder_handle);
	}

	hError = APK_BUILDER_CloseSession(g_builder_handle);
	if (ERR_OK != hError)
	{
		HxLOG_Error("APK_BUILDER_StopScan err: hBuilderHandle(0x%08x)\n", g_builder_handle);
	}

	return ERR_OK;
}


#if 0
void	NX_BUILDER_SetOptions(NX_SCANOPTIONS_t *pstOptions, DxBuilder_Option_t *pstBuilderOption)
{
	switch (pstOptions->channelType)
	{
		case	0:		//	CHANNEL_TYPE_TV
			pstBuilderOption->eSvcType = eDxSVC_TYPE_TV;
			break;
		case	1:		//	CHANNEL_TYPE_RADIO
			pstBuilderOption->eSvcType = eDxSVC_TYPE_RADIO;
			break;
		case	100:	//	CHANNEL_TYPE_ALL
			pstBuilderOption->eSvcType = eDxSVC_TYPE_All;
			break;
		case	2:		//	CHANNEL_TYPE_OTHER
		default:
			pstBuilderOption->eSvcType = eDxSVC_TYPE_All;
			break;
	}

	pstBuilderOption->eCasType = pstOptions->casType;
	pstBuilderOption->ulNetworkId = 0;
 	pstBuilderOption->bNetworkSearch = pstOptions->enableNetworkScan;
 	pstBuilderOption->bReplaceChannel = pstOptions->isReplaceChannel;
	pstBuilderOption->bWithNetworkId = FALSE;
}
#endif

HERROR		NX_Builder_ControlScan(HCHAR *controlScan)
{
	const HCHAR		*scanType = NULL;

	if (g_builder_handle == INVALID_BUILDER_HANDLE)
	{
		NX_APP_Error("Builder is already destroyed..\nThe Function will be ignored...\n");
		return	ERR_FAIL;
	}

	scanType = APK_BUILDER_GetScanType(g_builder_handle);
	HxLOG_Print(HxANSI_COLOR_YELLOW("controlScan = %s, scanType = %s\n"), controlScan, scanType);

	if (HLIB_STD_StrCaseCmp(controlScan, NX_SCANTYPE_PAUSE) == 0)
	{
		APK_BUILDER_PauseScan(g_builder_handle);
	}
	else if (HLIB_STD_StrCaseCmp(controlScan, NX_SCANTYPE_RESUME) == 0)
	{
		APK_BUILDER_ResumeScan(g_builder_handle);
	}
	else if (HLIB_STD_StrCaseCmp(controlScan, NX_SCANTYPE_SAVE) == 0)
	{
		if ((HLIB_STD_StrCaseCmp(scanType, NX_SCANTYPE_NORMAL) == 0) ||
			(HLIB_STD_StrCaseCmp(scanType, NX_SCANTYPE_FSATPOSTCODESKIP) == 0))
		{
			APK_META_SVC_Save();
		}
		else if (HLIB_STD_StrCaseCmp(scanType, NX_SCANTYPE_FSATUSERSCAN) == 0)
		{
			APK_META_SVC_RemoveServiceWithFlag(eDxSVC_UPDATE_READONLY);
			APK_META_SVC_Save();
		}
		else if (HLIB_STD_StrCaseCmp(scanType, NX_SCANTYPE_DISEQCDETECTION) == 0)
		{
			APK_META_ANTENNA_Save();
		}
		else
		{
			return	ERR_FAIL;
		}
	}
	else if (HLIB_STD_StrCaseCmp(controlScan, NX_SCANTYPE_LOAD) == 0)
	{
		if ((HLIB_STD_StrCaseCmp(scanType, NX_SCANTYPE_NORMAL) == 0) ||
			(HLIB_STD_StrCaseCmp(scanType, NX_SCANTYPE_FSATUSERSCAN) == 0) ||
			(HLIB_STD_StrCaseCmp(scanType, NX_SCANTYPE_FSATPOSTCODESKIP) == 0))
		{
			APK_META_SVC_LoadupdatedList();
		}
		else if (HLIB_STD_StrCaseCmp(scanType, NX_SCANTYPE_DISEQCDETECTION) == 0)
		{
			APK_META_ANTENNA_RemoveAll();
			APK_META_ANTENNA_Load();
		}
		else
		{
			return	ERR_FAIL;
		}
	}
	else if (HLIB_STD_StrCaseCmp(controlScan, NX_SCANTYPE_SETADDITIONALNUM) == 0)
	{
		APK_BUILDER_SetAdditionalNumbers(g_builder_handle);
	}
	else if (HLIB_STD_StrCaseCmp(controlScan, NX_SCANTYPE_LOADDEFCHANNELS) == 0)
	{
		APK_META_SVC_LoadPreferredList();
	}
	else if (HLIB_STD_StrCaseCmp(controlScan, NX_SCANTYPE_RESET) == 0)
	{
		APK_META_SVC_Reset();
	}
	else
	{
		return	ERR_FAIL;
	}

	return	ERR_OK;
}


#if 0
HERROR	NX_ANTINFO_SetInfo(HINT32 uid, NX_AntennaInfo_t *pstAntInfo)
{
	DxAntInfo_t			*pstInfo;
	NX_AntennaInfo_t ant;


	HxLOG_Debug("\n[%s.%d] GET ANT COUNT : %d \n\n",__FUNCTION__,__LINE__, APK_META_ANTENNA_Count());

	NX_COMMON_NOT_USED_PARAM(pstAntInfo);

	uid = 1;
	ant.eAntennaType = eNxSat_AntType_DiSEqC_1_0;
	HxSTD_StrCpy(ant.satelliteType, "ASTRA_1");
	HxSTD_StrCpy(ant.satelliteName, "ASTRA_1");
	ant.lnb_freq = 4294967295;
	ant.lnb_voltage = 0;
	ant.diseqc_version = 0;
	ant.switch_input = 3;
	ant.enable_22khztone = TRUE;
	ant.position = 2;
	ant.longitude = 192;
	ant.scd_band_num[0] = 0;
	ant.scd_band_num[1] = 0;
	ant.scd_band_freq[0] = 0;
	ant.scd_band_freq[1] = 0;

	APK_META_ANTENNA_GetUidAt(0);

	APK_META_ANTENNA_RemoveAll();

	APK_META_ANTENNA_Load();

	HxLOG_Debug("\n[%s.%d] GET ANT COUNT After Load Antenna : %d \n",__FUNCTION__,__LINE__, APK_META_ANTENNA_Count());

	pstInfo = APK_META_ANTENNA_GetInfo(uid);

	if (pstInfo == NULL)		return ERR_FAIL;

	HxSTD_MemSet(pstInfo, 0, sizeof(DxAntInfo_t));
	pstInfo->uid			= uid;
	pstInfo->antType		= ant.eAntennaType;
	HxSTD_StrNCpy(pstInfo->satType, ant.satelliteType, (DxANTINFO_NAME_LEN - 1));
	HxSTD_StrNCpy(pstInfo->satName, ant.satelliteName, (DxANTINFO_NAME_LEN - 1));

	switch (pstInfo->antType)
	{
		case	eDxANT_TYPE_LNB_ONLY:
			pstInfo->info.lnb.ulLnbFreq 	= ant.lnb_freq;
			pstInfo->info.lnb.lnbVoltage	= ant.lnb_voltage;
			break;
		case	eDxANT_TYPE_DISEQC:
			pstInfo->info.diseqc.ulLnbFreq		= ant.lnb_freq;
			pstInfo->info.diseqc.lnbVoltage 	= ant.lnb_voltage;
			pstInfo->info.diseqc.diseqcVersion	= ant.diseqc_version;
			pstInfo->info.diseqc.diseqcInput	= ant.switch_input;
			pstInfo->info.diseqc.b22kTone		= ant.enable_22khztone;
			pstInfo->info.diseqc.toneBurst		= ant.tone_burst;
			pstInfo->info.diseqc.ucMotorPosition= ant.position;
			pstInfo->info.diseqc.sSatLonggitude = ant.longitude;
			break;
		case	eDxANT_TYPE_SCD:
			pstInfo->info.scd.ulLnbFreq 		= ant.lnb_freq;
			pstInfo->info.scd.lnbVoltage		= ant.lnb_voltage;
			pstInfo->info.scd.b22kTone			= ant.enable_22khztone;
			pstInfo->info.scd.scdInput			= ant.switch_input;
			pstInfo->info.scd.ulScdUserBandNum	= ant.scd_band_num[0];
			pstInfo->info.scd.ulScdUserBandNum1 = ant.scd_band_num[1];
			pstInfo->info.scd.ulScdUserBandNum2	= ant.scd_band_num[2]; // MOON_SCD
			pstInfo->info.scd.ulScdUserBandNum3 = 0;//ant.scd_band_num[3];
			pstInfo->info.scd.ulScdUserBandFreq = ant.scd_band_freq[0];
			pstInfo->info.scd.ulScdUserBandFreq1 = ant.scd_band_freq[1];
			pstInfo->info.scd.ulScdUserBandFreq2 = ant.scd_band_freq[2];
			pstInfo->info.scd.ulScdUserBandFreq3 = 0;//ant.scd_band_freq[3];
			break;
		default:
			NX_APP_Free(pstInfo);
			return ERR_FAIL;
	}
	APK_META_ANTENNA_SetInfo(uid, pstInfo);
	APK_META_ANTENNA_InfoFree(pstInfo);

	APK_META_ANTENNA_Save();

	return ERR_OK;
}
#endif

/***************************************************************************
Related Search Command
***************************************************************************/
#if defined(CONFIG_DEBUG)

NX_SCANPARAMS_t *MakeTpList(void)
{
	NX_SCANPARAMS_t *pstParam  = NULL;

#if 0
	pstParam = NX_APP_Calloc(sizeof(NX_SCANPARAMS_t) * 104);

	for(i = 0; i < 104; i++)
	{
		pstParam[i].tunerType = 0;
		pstParam[i].data.stDVBSScanParams.antennaId = 1;
		HxSTD_StrCpy(pstParam[i].data.stDVBSScanParams.antennaName, "ASTRA_1");

		switch(s_SatDefaultTpList_Astra1[i].eFecCodeRate)
		{
		case 0:
			HxSTD_StrCpy(pstParam[i].data.stDVBSScanParams.codeRate, "auto");
			break;
		case 1:
			HxSTD_StrCpy(pstParam[i].data.stDVBSScanParams.codeRate, "none");
			break;
		case 12:
			HxSTD_StrCpy(pstParam[i].data.stDVBSScanParams.codeRate, "9/10");
			break;

		default:
			HxSTD_StrCpy(pstParam[i].data.stDVBSScanParams.codeRate, "auto");
			break;
		}

		pstParam[i].data.stDVBSScanParams.startFreq = s_SatDefaultTpList_Astra1[i].ulFrequency;
		pstParam[i].data.stDVBSScanParams.endFreq 	= s_SatDefaultTpList_Astra1[i].ulFrequency;

		pstParam[i].data.stDVBSScanParams.modulationModes = s_SatDefaultTpList_Astra1[i].ePskMod;
		pstParam[i].data.stDVBSScanParams.networkId = 0;
		pstParam[i].data.stDVBSScanParams.orbitalPosition = 0;
		pstParam[i].data.stDVBSScanParams.polarisation = s_SatDefaultTpList_Astra1[i].ePolarization;

		HxSTD_sprintf(pstParam[i].data.stDVBSScanParams.symbolRate,"\"%d\"" ,s_SatDefaultTpList_Astra1[i].ulSymbolRate);

	}
#endif
	return pstParam;
}

static HINT32 cmdStartCabScan(void *szArgs)
{
	NX_SCANPARAMS_t sParams;
	NX_SCANOPTIONS_t scanOption;

	DVBC_SCANPARAMS_t	*scanParams = NULL;

	NX_COMMON_NOT_USED_PARAM(szArgs);
	_nx_builder_callback = nx_builder_channelConfigEvent;

	HxSTD_memset(&scanOption, 0, sizeof(NX_SCANOPTIONS_t));

	sParams.tunerType = eDxDELIVERY_TYPE_CAB;
	scanParams = &(sParams.data.stDVBCScanParams);
	HxSTD_sprintf(scanParams->symbolRate,"\"%d\"" , 6875000);
	scanParams->startFreq = 602000;
	scanParams->endFreq = 602000;
	scanParams->raster  = 0;
	scanParams->networkId = 100;
	scanParams->startNoetworkScanOnNIT = TRUE;
	scanParams->modulationModes = 0;

	scanOption.casType = 0;
	scanOption.channelType = 100;
	scanOption.enableNetworkScan = 1;
	scanOption.isReplaceChannel = 0;

	HxLOG_Debug("\n[%s.%d] CALLL START SCAN \n",__FUNCTION__,__LINE__);
	NX_Builder_StartScan("normal", &scanOption, 1, &sParams);

	return HxCMD_OK;
}

static HINT32 cmdStartSatScan(void *szArgs)
{
	NX_SCANPARAMS_t scanParam;
	NX_SCANOPTIONS_t scanOption;

	NX_SCANPARAMS_t *pstScanParams;

	NX_COMMON_NOT_USED_PARAM(szArgs);
	_nx_builder_callback = nx_builder_channelConfigEvent;

	HxSTD_memset(&scanParam, 0, sizeof(NX_SCANPARAMS_t));
	HxSTD_memset(&scanOption, 0, sizeof(NX_SCANOPTIONS_t));

	scanOption.casType = 0;
	scanOption.channelType = 100;
	scanOption.enableNetworkScan = 0;
	scanOption.isReplaceChannel = 0;


	pstScanParams = MakeTpList();

	HxLOG_Debug("\n[%s.%d] CALLL START SCAN \n",__FUNCTION__,__LINE__);
	NX_Builder_StartScan("normal", &scanOption, 104, pstScanParams);

	return HxCMD_OK;
}

static HINT32 cmdLoad(void *szArgs)
{
	HCHAR *scanType = "normal";

	NX_COMMON_NOT_USED_PARAM(szArgs);

	//if (HLIB_STD_StrCaseCmp(controlScan, "load") == 0)
	{
		if ((HLIB_STD_StrCaseCmp(scanType, NX_SCANTYPE_NORMAL) == 0) ||
			(HLIB_STD_StrCaseCmp(scanType, NX_SCANTYPE_FSATUSERSCAN) == 0) ||
			(HLIB_STD_StrCaseCmp(scanType, NX_SCANTYPE_FSATPOSTCODESKIP) == 0))
		{
			APK_META_SVC_LoadupdatedList();

		}
		else
		if (HLIB_STD_StrCaseCmp(scanType, NX_SCANTYPE_DISEQCDETECTION) == 0)
		{
			APK_META_ANTENNA_RemoveAll();
			APK_META_ANTENNA_Load();
		}
	}
	return HxCMD_OK;
}

static HINT32 cmdSave(void *szArgs)
{
	HCHAR *scanType = "normal";

	NX_COMMON_NOT_USED_PARAM(szArgs);

	//if (HLIB_STD_StrCaseCmp(controlScan, "save") == 0)
	{
		if ((HLIB_STD_StrCaseCmp(scanType, NX_SCANTYPE_NORMAL) == 0) ||
			(HLIB_STD_StrCaseCmp(scanType, NX_SCANTYPE_FSATPOSTCODESKIP) == 0))
		{
			APK_META_SVC_Save();
		}
		else
		if (HLIB_STD_StrCaseCmp(scanType, NX_SCANTYPE_FSATUSERSCAN) == 0)
		{
			APK_META_SVC_RemoveServiceWithFlag(eDxSVC_UPDATE_READONLY);
			APK_META_SVC_Save();
		}
		else
		if (HLIB_STD_StrCaseCmp(scanType, NX_SCANTYPE_DISEQCDETECTION) == 0)
		{
			APK_META_ANTENNA_Save();
		}
		else
		{
			return -1;
		}
	}

	return HxCMD_OK;
}

static void nx_search_Command_Init(void)
{
#define cmdHandle		"nxsearchcmd"
	HLIB_CMD_RegisterWord(cmdHandle, cmdStartCabScan,
						 (HINT8 *)"startcabscan",
						 (HINT8 *)"start cab scan",
						 (HINT8 *)"startcabscan");
	HLIB_CMD_RegisterWord(cmdHandle, cmdStartSatScan,
						 (HINT8 *)"startsatscan",
						 (HINT8 *)"start sat scan",
						 (HINT8 *)"startsatscan");
	HLIB_CMD_RegisterWord(cmdHandle, cmdLoad,
						 (HINT8 *)"load",
						 (HINT8 *)"service data load",
						 (HINT8 *)"load");
	HLIB_CMD_RegisterWord(cmdHandle, cmdSave,
						 (HINT8 *)"save",
						 (HINT8 *)"service data save",
						 (HINT8 *)"save");
}

#endif		// #if defined(CONFIG_DEBUG)

