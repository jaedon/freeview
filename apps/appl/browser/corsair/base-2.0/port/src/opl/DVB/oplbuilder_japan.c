/**************************************************************************************/
/**
 * @file oplbuilder_japan.c
 *
 * Defines the builder structure.
 *
 * @author  HUMAX
 * @date    2011/11/21
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#ifdef CONFIG_OP_JAPAN
#include "oplbuilder.h"

#include "oapi.h"
#include <apk.h>
#include <hlib.h>
#include <hapi.h>

#if 1// TODO: 다른 파일로 분리하는 것을 고려 해라.
typedef enum
{
	eOPL_CAB_FREQ_MODEL_DEFAULT,
	eOPL_CAB_FREQ_MODEL_JAPAN,
	eOPL_CAB_FREQ_MODEL_MAXNUM
} _OPL_CAB_FREQ_MODEL_t;

// Cable Search Mode : Linear, Auto 등
typedef enum
{
	eOPL_CAB_SEARCH_MODE_AUTO,				// Auto Search TP :
	eOPL_CAB_SEARCH_MODE_LINEAR,			// Linear Search TP
	eOPL_CAB_SEARCH_MODE_MAX
} _OPL_CAB_SEARCH_MODE_t;

typedef struct _OPL_CAB_TS_INFO {
	HUINT32					freq;
	HUINT32					SR;
	DxCab_Constellation_e	constellation;
} _OPL_CAB_TsInfo_t;

typedef struct _OPL_CAB_TS_INFO_LIST_t
{
	_OPL_CAB_FREQ_MODEL_t	 eFreqModel;
	_OPL_CAB_TsInfo_t		*pstTpArray;
} _OPL_CAB_TsInfoList_t;
#endif

#define	____OPL_BUILDER_PRIVATE_____________________________________________________________________
#define INVALID_BUILDER_HANDLE			(0xFFFF)
static	HUINT32 _builder_handle = INVALID_BUILDER_HANDLE;
static	int	(*_opl_builder_callback)(void *pstData) = NULL;

#if 1 // TODO: 다른 파일로 분리하는 것을 고려 해라.
static _OPL_CAB_TsInfo_t	s_stOplCableTp_Default[] =			// Default TPs :
{
	{369000,	6875,		eDxCAB_CONSTELLATION_AUTO},
	{372000,	6875,		eDxCAB_CONSTELLATION_AUTO},
	{356000,	6875,		eDxCAB_CONSTELLATION_AUTO},
	{0,			0,			0}						// Last Item : 0, 0, 0
};

static _OPL_CAB_TsInfo_t	s_stOplJapanCableAutoScanTsInfo[] =
{
	{ 93000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{ 99000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{105000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{111000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{117000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{123000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{129000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{135000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{141000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{147000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{153000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{159000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{167000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{173000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{179000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{185000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{191000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{195000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{201000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{207000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{213000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{219000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{225000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{231000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{233000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{237000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{239000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{243000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{245000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{249000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{251000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{255000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{261000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{267000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{273000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{279000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{285000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{291000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{297000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{303000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{309000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{315000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{321000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{327000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{333000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{339000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{345000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{351000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{357000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{363000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{369000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{375000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{381000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{387000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{393000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{399000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{405000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{411000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{417000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{423000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{429000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{435000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{441000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{447000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{453000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{459000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{465000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{473000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{479000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{485000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{491000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{497000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{503000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{509000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{515000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{521000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{527000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{533000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{539000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{545000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{551000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{557000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{563000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{569000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{575000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{581000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{587000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{593000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{599000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{605000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{611000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{617000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{623000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{629000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{635000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{641000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{647000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{653000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{659000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{665000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{671000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{677000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{683000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{689000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{695000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{701000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{707000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{713000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{719000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{725000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{731000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{737000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{743000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{749000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{755000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{761000, 5274, eDxCAB_CONSTELLATION_AUTO},
	{767000, 5274, eDxCAB_CONSTELLATION_AUTO},

	{0,			0,			0}						// Last Item : 0, 0, 0
};

//--------------------------------------
// - Auto TP List -
//--------------------------------------
static _OPL_CAB_TsInfoList_t	s_stOplCableAutoTp[] =
{
	{eOPL_CAB_FREQ_MODEL_DEFAULT,		s_stOplCableTp_Default},
	{eOPL_CAB_FREQ_MODEL_JAPAN,		s_stOplJapanCableAutoScanTsInfo},

	// 사양이 추가 될 때마다 이 위로 추가
	{eOPL_CAB_FREQ_MODEL_MAXNUM,		NULL}
};

//--------------------------------------
// - Linear TP List -
//--------------------------------------
static _OPL_CAB_TsInfoList_t	s_stOplCableLinearTp[] =
{
	{eOPL_CAB_FREQ_MODEL_DEFAULT,		s_stOplCableTp_Default},
	// 사양이 추가 될 때마다 이 위로 추가
	{eOPL_CAB_FREQ_MODEL_MAXNUM,		NULL}
};
#endif

#if defined(CONFIG_OP_JAPAN)
/*	octo에서 올려주는 scan event를 oplbuilder에서 받으면 status를 octo쪽에 다시 물어보는 구조라서
	event 올려줄 당시의 상태와 oplbuilder에서 status를 체크할 때의 다를 수 있음.
	그러나 일본향에서는 oipf에 save하는 방법이 정의되어 있지 않아서 scan finish 타이밍에 save를 하도록 하였는데,
	위의 async 처리로 인해 finish 타이밍이 여러번이 될 수 있음. 이로 인해 save가 여러번 되는 것을 막기 위해 flag를 두기로 함.
	(example)
	1. octo scaning event 발생
	2. octo scan finish event 발생
	3. oplbuilder에서 1의 scanning event 받아서 octo에 status를 물어봤는데 finish 임. 그럼 save.
	4. oplbuilder에서 2의 scan finish event 받아서 octo에 status를 물어봤는데 finish임. 그럼 save?? 그러면 안됨 ! */
static HBOOL		s_bScanFinish = FALSE;
#endif

static	HCHAR*		_get_int_from_array(HCHAR *str, HINT32 *pnValue)
{
	HCHAR	tempBuf[32], *p, *q;

	HxSTD_MemSet(tempBuf, 0, 32);
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
			*p++ = *q;

		q++;
	}

	*pnValue = HLIB_STD_StrToUINT32(tempBuf);

	return q;
}

#if 1 // TODO: 다른 파일로 분리하는 것을 고려 해라.
static _OPL_CAB_TsInfo_t *_get_defaultcable_tslist (_OPL_CAB_FREQ_MODEL_t eModelType, _OPL_CAB_SEARCH_MODE_t eSearchMode)
{
	HUINT32		 ulCount;
	_OPL_CAB_TsInfoList_t	*pstTsInfoList;

	switch (eSearchMode)
	{
	case eOPL_CAB_SEARCH_MODE_AUTO:
		pstTsInfoList = s_stOplCableAutoTp;
		break;

	case eOPL_CAB_SEARCH_MODE_LINEAR:
		pstTsInfoList = s_stOplCableLinearTp;
		break;

	default:
		return NULL;
	}

	for (ulCount = 0; ulCount < eOPL_CAB_FREQ_MODEL_MAXNUM; ulCount++)
	{
		if (pstTsInfoList[ulCount].eFreqModel == eModelType)
		{
			return pstTsInfoList[ulCount].pstTpArray;
		}
	}

	return NULL;
}

static void	_get_defaultcable_counttslist (_OPL_CAB_FREQ_MODEL_t eModelType, _OPL_CAB_SEARCH_MODE_t eSearchMode, HUINT32 *pulNumOfTps)
{
	HUINT32				 ulCount;
	_OPL_CAB_TsInfo_t	*pstTsList;

	pstTsList = _get_defaultcable_tslist (eModelType, eSearchMode);
	if (pstTsList == NULL)
	{
		return;
	}

	ulCount = 0;
	while (1)
	{
		if (pstTsList[ulCount].freq == 0)
		{
			break;
		}

		ulCount++;
	}

	*pulNumOfTps = ulCount;
}

static void	_get_defaultcable_tsinfo(_OPL_CAB_FREQ_MODEL_t eModelType, _OPL_CAB_SEARCH_MODE_t eSearchMode, HUINT32 ulIndex, _OPL_CAB_TsInfo_t *pstTsInfo)
{
	HUINT32				 ulTotalNum;
	_OPL_CAB_TsInfo_t	*pstTsList;

	pstTsList = _get_defaultcable_tslist (eModelType, eSearchMode);
	if (pstTsList == NULL)
	{
		return;
	}

	_get_defaultcable_counttslist (eModelType, eSearchMode, &ulTotalNum);
	if ((ulTotalNum == 0) || (ulTotalNum <= ulIndex))
	{
		return;
	}

	memcpy(pstTsInfo, &pstTsList[ulIndex], sizeof(_OPL_CAB_TsInfo_t));
}
#endif

static	HxList_t*	_OPL_BUILDER_MakeSatTuningInfos(HxList_t *tune_list, DVBS_SCANPARAMS_t *pstParams)
{
	HCHAR		*str;
	HINT32		symbolRateList[32], symbolRateNo = 0, i;

	HINT32		startfreq, endfreq;
	HxList_t	*list = tune_list;

	DxTuneParam_t			*pstTuningInfo;
	DxSat_CodeRate_e		 eCodeRate;
	DxSat_Polarization_e	 ePolar;
	DxSat_PskModulation_e	 eModulation;

	// MOON-BUILDER
#if (1)
	HUINT32 	ulAntId;
#endif

	HxLOG_Trace();

	{
		DVBS_SCANPARAMS_t	*scanParam;

		scanParam = pstParams;

		str = scanParam->symbolRate;
		while (str && *str)
		{
			str = _get_int_from_array(str, &symbolRateList[symbolRateNo]);
			symbolRateList[symbolRateNo] = symbolRateList[symbolRateNo] / 1000;
			symbolRateNo++;
		}

	// MOON-BUILDER
#if (1)
		ulAntId = scanParam->antennaId;
#endif

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

		while (startfreq <= endfreq)
		{
			DxSat_TuningTPInfo_t	*pstSatInfo;

			for (i = 0; i < symbolRateNo; i++)
			{
				pstTuningInfo = (DxTuneParam_t*)HLIB_STD_MemCalloc(sizeof(DxTuneParam_t));
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


				HxLOG_Debug("OPL-AddTuneInfo [%d, %d, %d]\n", pstSatInfo->ulFrequency, pstSatInfo->ulSymbolRate, pstSatInfo->eTransSpec);

				list = HLIB_LIST_Append(list, (void*)pstTuningInfo);
			}

			startfreq += 4;			//	TODO: fec와 SR를 보고 Jump하는 부분 추가 되어야 함.
		}

	}

	return list;
}

static	HxList_t*	_OPL_BUILDER_MakeCabTuningInfos(HxList_t *tune_list, DVBC_SCANPARAMS_t *pstParams)
{
	HCHAR		*str;
	HINT32		symbolRateList[32], symbolRateNo = 0, i;
	HINT32		startfreq, endfreq;
	HxList_t	*list = tune_list;

	DxTuneParam_t	*pstTuningInfo;
	HINT32			raster;
#if defined(CONFIG_OP_JAPAN)
	HINT32				j;
	HUINT32 			numOfTps;

	_OPL_CAB_TsInfo_t 		stTsInfo;
#endif
	DxCab_Constellation_e 	eContellation;
	DxCabTuningInfo_t		*pstCabInfo = NULL;
	DVBC_SCANPARAMS_t		*scanParam;

	HxLOG_Trace();

	scanParam = pstParams;

	str = scanParam->symbolRate;
	while (str && *str)
	{
		str = _get_int_from_array(str, &symbolRateList[symbolRateNo]);
#if !defined(CONFIG_OP_JAPAN)
		if (symbolRateList[symbolRateNo] == 65535000)
		{
			symbolRateList[symbolRateNo] = 0xFFFF;
		}
		else
		{
			symbolRateList[symbolRateNo] = symbolRateList[symbolRateNo] / 1000;
		}
#else
		symbolRateList[symbolRateNo] = symbolRateList[symbolRateNo] / 1000;
#endif
		HxLOG_Debug("symbolRateList : [%d] - [%d]\n", symbolRateNo, symbolRateList[symbolRateNo]);
		symbolRateNo++;
	}

	switch(scanParam->modulationModes)
	{
		case eOplModulationMode_Qam16:		eContellation = eDxCAB_CONSTELLATION_16QAM;		break;
		case eOplModulationMode_Qam32:		eContellation = eDxCAB_CONSTELLATION_32QAM;		break;
		case eOplModulationMode_Qam64:		eContellation = eDxCAB_CONSTELLATION_64QAM;		break;
		case eOplModulationMode_Qam128:		eContellation = eDxCAB_CONSTELLATION_128QAM;	break;
		case eOplModulationMode_Qam256:		eContellation = eDxCAB_CONSTELLATION_256QAM;	break;
		case eOplModulationMode_Qam1024:	eContellation = eDxCAB_CONSTELLATION_1024QAM;	break;
		case eOplModulationMode_Qam4096:	eContellation = eDxCAB_CONSTELLATION_4096QAM;	break;
		default:							eContellation = eDxCAB_CONSTELLATION_AUTO;		break;
	}

	raster = scanParam->raster;
	startfreq = scanParam->startFreq;
	endfreq   = scanParam->endFreq;

#if defined(CONFIG_OP_JAPAN)
	if(startfreq == endfreq)		/* Mannual */
	{
		for (j = 0; j < symbolRateNo; j++)
		{
			pstTuningInfo = (DxTuneParam_t*)HLIB_STD_MemCalloc(sizeof(DxTuneParam_t));
			pstTuningInfo->eDeliType = eDxDELIVERY_TYPE_CAB;
			pstCabInfo = &pstTuningInfo->cab;

			pstCabInfo->ulFrequency = startfreq;
			pstCabInfo->ulSymbolRate = symbolRateList[j];
			pstCabInfo->constellation = eContellation;
			pstCabInfo->spectrum = eDxCAB_SPECTRUM_AUTO;
			pstCabInfo->nNetId = pstParams->networkId;

			HxLOG_Debug("OPL-AddTuneInfo [%d, %d, %d]\n", pstCabInfo->ulFrequency, pstCabInfo->ulSymbolRate, pstCabInfo->netId);

			list = HLIB_LIST_Append(list, (void*)pstTuningInfo);
		}
	}
	else						/* Auto */
	{
		_get_defaultcable_counttslist(eOPL_CAB_FREQ_MODEL_JAPAN, eOPL_CAB_SEARCH_MODE_AUTO, &numOfTps);

		for(i=0;i<numOfTps;i++)
		{
			memset(&stTsInfo, 0, sizeof(_OPL_CAB_TsInfo_t));
			_get_defaultcable_tsinfo(eOPL_CAB_FREQ_MODEL_JAPAN, eOPL_CAB_SEARCH_MODE_AUTO, i, &stTsInfo);
			if((stTsInfo.freq >= startfreq) &&
				(stTsInfo.freq <= endfreq))
			{
				for (j = 0; j < symbolRateNo; j++)
				{
					pstTuningInfo = (DxTuneParam_t*)HLIB_STD_MemCalloc(sizeof(DxTuneParam_t));
					pstTuningInfo->eDeliType = eDxDELIVERY_TYPE_CAB;
					pstCabInfo = &pstTuningInfo->cab;

					pstCabInfo->ulFrequency = stTsInfo.freq;
					pstCabInfo->ulSymbolRate = symbolRateList[j];
					pstCabInfo->constellation = eContellation;
					pstCabInfo->spectrum = eDxCAB_SPECTRUM_AUTO;
					pstCabInfo->nNetId = pstParams->networkId;

					HxLOG_Debug("OPL-AddTuneInfo [%d, %d, %d]\n", pstCabInfo->ulFrequency, pstCabInfo->ulSymbolRate, pstCabInfo->netId);

					list = HLIB_LIST_Append(list, (void*)pstTuningInfo);
				}
			}
		}
	}
#else
	while (startfreq <= endfreq)
	{
		for (i = 0; i < symbolRateNo; i++)
		{
			pstTuningInfo = (DxTuneParam_t*)HLIB_STD_MemCalloc(sizeof(DxTuneParam_t));
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
#endif

	return list;
}

static	HxList_t*	_OPL_BUILDER_MakeTerTuningInfos(HxList_t *tune_list, DVBT_SCANPARAMS_t *pstParams)
{
	HINT32		startfreq, endfreq;
	HxList_t		*list = tune_list;

	DxTuneParam_t	*pstTuningInfo;
	HINT32			raster;

	DxTer_TransMode_e 	eTransMode;
	DxTer_Constellation_e 	eConstellation;
	DxTer_BandWidth_e 	eBandwidth;
	DxTer_System_e 		eSystem = eDxTER_DVB_T;	// How can we use both of T and T2??

	DVBT_SCANPARAMS_t	*scanParam;

	HxLOG_Trace();

	scanParam = pstParams;

	eTransMode = eDxTER_TRANSMODE_AUTO;
	if (HxSTD_StrStr(scanParam->ofdm, "1K"))
		eTransMode = eDxTER_T2TRANSMODE_1K;
	else if (HxSTD_StrStr(scanParam->ofdm, "2K"))
		eTransMode = eDxTER_TRANSMODE_2K;
	else if (HxSTD_StrStr(scanParam->ofdm, "4K"))
		eTransMode = eDxTER_T2TRANSMODE_4K;
	else if (HxSTD_StrStr(scanParam->ofdm, "8K"))
		eTransMode = eDxTER_TRANSMODE_8K;
	else if (HxSTD_StrStr(scanParam->ofdm, "16K"))
		eTransMode = eDxTER_T2TRANSMODE_16K;
	else if (HxSTD_StrStr(scanParam->ofdm, "32K"))
		eTransMode = eDxTER_T2TRANSMODE_32K;

	eConstellation = eDxTER_CONSTELLATION_AUTO;
	switch(scanParam->modulationModes)
	{
		case 1:
			eConstellation = eDxTER_CONSTELLATION_QPSK;
			break;
		case 4:
			eConstellation = eDxTER_CONSTELLATION_16QAM;
			break;
		case 8:
//			eConstellation = eDxTER_CONSTELLATION_32QAM;
			break;
		case 16:
			eConstellation = eDxTER_CONSTELLATION_64QAM;
			break;
		case 32:
//			eConstellation = eDxTER_CONSTELLATION_128QAM;
			break;
		case 64:
			eConstellation = eDxTER_CONSTELLATION_256QAM;
			break;
	}

	if (HxSTD_StrStr(scanParam->bandwidth, "1.7Mhz"))
		eBandwidth = eDxTER_T2BANDWIDTH_1P7MHZ;
	else if (HxSTD_StrStr(scanParam->bandwidth, "5Mhz"))
		eBandwidth = eDxTER_T2BANDWIDTH_5MHZ;
	else if (HxSTD_StrStr(scanParam->bandwidth, "6Mhz"))
		eBandwidth = eDxTER_BANDWIDTH_6M;
	else if (HxSTD_StrStr(scanParam->bandwidth, "7Mhz"))
		eBandwidth = eDxTER_BANDWIDTH_7M;
	else if (HxSTD_StrStr(scanParam->bandwidth, "8Mhz"))
		eBandwidth = eDxTER_BANDWIDTH_8M;
	else if (HxSTD_StrStr(scanParam->bandwidth, "10Mhz"))
		eBandwidth = eDxTER_T2BANDWIDTH_10MHZ;
	else
		eBandwidth = eDxTER_BANDWIDTH_8M;

	if (HxSTD_StrStr(scanParam->transmission, "DVB-T2"))
		eSystem = eDxTER_DVB_T2;

	startfreq = scanParam->startFreq;
	endfreq   = scanParam->endFreq;
	raster = scanParam->raster;

	if (raster <= 0) raster += 1;

	while (startfreq <= endfreq)
	{
		DxTerTuningInfo_t	*pstTerInfo;

		pstTuningInfo = (DxTuneParam_t*)HLIB_STD_MemCalloc(sizeof(DxTuneParam_t));
		pstTuningInfo->eDeliType = eDxDELIVERY_TYPE_TER;
		pstTerInfo = &pstTuningInfo->ter;

		pstTerInfo->ulFrequency = startfreq;
		pstTerInfo->eBandWidth = eBandwidth;
		pstTerInfo->eConstellation = eConstellation;
		pstTerInfo->eHierachy = eDxTER_HIERACHY_AUTO;
		pstTerInfo->eCodeRate = eDxTER_CODERATE_AUTO;
		pstTerInfo->eTransMode = eTransMode;
		pstTerInfo->eGuardInterval = eDxTER_GUARDINTERVAL_AUTO;
		pstTerInfo->eStream = eDxTER_STREAM_HP;
		pstTerInfo->eOffset = eDxTER_OFFSET_AUTO;
		pstTerInfo->eSystem = eSystem;
		memset(&pstTerInfo->stT2Param, 0, sizeof(DxTer_T2_Param_t));

		HxLOG_Debug("OPL-AddTuneInfo [%d, %d, %d, %d, %d]\n", pstTerInfo->ulFrequency, pstTerInfo->eBandWidth, pstTerInfo->eConstellation, pstTerInfo->eTransMode, pstTerInfo->eSystem);

		list = HLIB_LIST_Append(list, (void*)pstTuningInfo);

		startfreq += raster;
	}

	return list;
}

static	void	_OPL_BUILDER_SetOptions(OPL_SCANOPTIONS_t *pstOptions, DxBuilder_Option_t *pstBuilderOption)
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
#ifndef	CONFIG_OP_JAPAN
	pstBuilderOption->bNetworkSearch = pstOptions->enableNetworkScan;
#endif
	pstBuilderOption->bReplaceChannel = pstOptions->isReplaceChannel;
	pstBuilderOption->bWithNetworkId = FALSE;
}

static	HxList_t*	_OPL_BUILDER_SetScanParams(HINT32 num, OPL_SCANPARAMS_t *pstParams, ApkBuilder_Action_t *pstBuilderAction, DxBuilder_Option_t *pstBuilderOption)
{
	HINT32		i;
	HxList_t	*tuning_info_list = NULL;

	DxDeliveryType_e		eDeliveryType = eDxDELIVERY_TYPE_SAT;

	if (num == 0)
		num = 1;

	for (i = 0; i < num; i++)
	{
		OPL_SCANPARAMS_t	*paramPtr;

		paramPtr = &pstParams[i];

		switch (paramPtr->tunerType)
		{
			case	OPL_TUNER_SAT:
				eDeliveryType = eDxDELIVERY_TYPE_SAT;
				{
					DVBS_SCANPARAMS_t	*scanParams = &paramPtr->data.stDVBSScanParams;

					//	set builder action
					pstBuilderAction->utInfo.satInfo.ulAntId = scanParams->antennaId;
					strncpy(pstBuilderAction->utInfo.satInfo.szSatelliteName, scanParams->antennaName, 31);

					//	set builder option
					if ((scanParams->networkId) && pstBuilderOption)
					{
						pstBuilderOption->ulNetworkId = scanParams->networkId;
						pstBuilderOption->bWithNetworkId = TRUE;
					}

					//	make tuning infos..
					tuning_info_list = _OPL_BUILDER_MakeSatTuningInfos(tuning_info_list, scanParams);
				}
				break;
			case	OPL_TUNER_CAB:
				eDeliveryType = eDxDELIVERY_TYPE_CAB;
				{
					DVBC_SCANPARAMS_t	*scanParams = &paramPtr->data.stDVBCScanParams;

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

						HxLOG_Debug("pstBuilderOption->bNetworkSearch: %d \n", pstBuilderOption->bNetworkSearch);
						if(pstBuilderOption->bNetworkSearch)
						{
							HxLOG_Debug("pstBuilderOption->ulNetworkId: 0x%x \n", (HINT32)pstBuilderOption->ulNetworkId);
							HAPI_SetRegistryInt(DxDB_NETWORK_ID_FOR_NETWORK_SEARCH, pstBuilderOption->ulNetworkId);
						}
					}

					//	make tuning infos..
					tuning_info_list = _OPL_BUILDER_MakeCabTuningInfos(tuning_info_list, scanParams);
				}
				break;
			case	OPL_TUNER_TER:
				eDeliveryType = eDxDELIVERY_TYPE_TER;
				{
					DVBT_SCANPARAMS_t	*scanParams = &paramPtr->data.stDVBTScanParams;

					if (pstBuilderOption)
						pstBuilderOption->bNetworkSearch = FALSE;

					//	make tuning infos
					tuning_info_list = _OPL_BUILDER_MakeTerTuningInfos(tuning_info_list, scanParams);
				}
				break;
			default:
				HxLOG_Critical("Not defined Delivery Type!!!\n");
				HxLOG_Assert(0);
  				break;
		}
	}
	pstBuilderAction->eDeliveryType = eDeliveryType;

	return tuning_info_list;
}

static BPLStatus _OPL_BUILDER_ConvertTunedParam( DxDeliveryType_e eTunerType, DxTuneParam_t *pstTpInfo,
												OPL_SCANPARAMS_t **ppstOplTunedParam)
{
	OPL_SCANPARAMS_t *pstOplTunedParam;

	if (NULL != ppstOplTunedParam)
	{
		*ppstOplTunedParam = NULL;
	}
	else
	{
		return BPL_STATUS_ERROR;
	}

	if (eTunerType > eDxDELIVERY_TYPE_CAB) // check valid tuner type..
	{
		return BPL_STATUS_NOT_SUPPORTED;
	}

	pstOplTunedParam = (OPL_SCANPARAMS_t*)HLIB_STD_MemCalloc(sizeof(OPL_SCANPARAMS_t));
	if (pstOplTunedParam)
	{
		pstOplTunedParam->tunerType = eTunerType;
		if (eDxDELIVERY_TYPE_CAB == eTunerType)
		{
			pstOplTunedParam->data.stDVBCScanParams.startFreq = pstTpInfo->cab.ulFrequency;
			pstOplTunedParam->data.stDVBCScanParams.endFreq = pstTpInfo->cab.ulFrequency;

			switch(pstTpInfo->cab.constellation)
			{
				case eDxCAB_CONSTELLATION_16QAM:	pstOplTunedParam->data.stDVBCScanParams.modulationModes = eOplModulationMode_Qam16; break;
				case eDxCAB_CONSTELLATION_32QAM:	pstOplTunedParam->data.stDVBCScanParams.modulationModes = eOplModulationMode_Qam32; break;
				case eDxCAB_CONSTELLATION_64QAM:	pstOplTunedParam->data.stDVBCScanParams.modulationModes = eOplModulationMode_Qam64; break;
				case eDxCAB_CONSTELLATION_128QAM:	pstOplTunedParam->data.stDVBCScanParams.modulationModes = eOplModulationMode_Qam128; break;
				case eDxCAB_CONSTELLATION_256QAM:	pstOplTunedParam->data.stDVBCScanParams.modulationModes = eOplModulationMode_Qam256; break;
				case eDxCAB_CONSTELLATION_1024QAM:	pstOplTunedParam->data.stDVBCScanParams.modulationModes = eOplModulationMode_Qam1024; break;
				case eDxCAB_CONSTELLATION_4096QAM:	pstOplTunedParam->data.stDVBCScanParams.modulationModes = eOplModulationMode_Qam4096; break;
				case eDxCAB_CONSTELLATION_AUTO:		
				default:
					pstOplTunedParam->data.stDVBCScanParams.modulationModes = eOplModulationMode_Auto;
					break;
			}
			HxSTD_snprintf(pstOplTunedParam->data.stDVBCScanParams.symbolRate, MAX_SYMBOLRATE_LEN,"%d", (pstTpInfo->cab.ulSymbolRate * 1000));
			pstOplTunedParam->data.stDVBCScanParams.networkId = pstTpInfo->cab.nNetId;
		}
		else if (eDxDELIVERY_TYPE_TER == eTunerType)
		{
			pstOplTunedParam->data.stDVBTScanParams.startFreq = pstTpInfo->ter.ulFrequency;
			pstOplTunedParam->data.stDVBTScanParams.endFreq = pstTpInfo->ter.ulFrequency;
#if 1
			switch(pstTpInfo->ter.eTransMode)
			{
				case eDxTER_T2TRANSMODE_1K:
					HxSTD_StrNCpy(pstOplTunedParam->data.stDVBTScanParams.ofdm, "1K", sizeof ("1K"));
					break;

				case eDxTER_TRANSMODE_2K:
					HxSTD_StrNCpy(pstOplTunedParam->data.stDVBTScanParams.ofdm, "2K", sizeof ("2K"));
					break;

				case eDxTER_T2TRANSMODE_4K:
					HxSTD_StrNCpy(pstOplTunedParam->data.stDVBTScanParams.ofdm, "4K", sizeof ("4K"));
					break;

				case eDxTER_TRANSMODE_8K:
					HxSTD_StrNCpy(pstOplTunedParam->data.stDVBTScanParams.ofdm, "8K", sizeof ("8K"));
					break;

				case eDxTER_T2TRANSMODE_16K:
					HxSTD_StrNCpy(pstOplTunedParam->data.stDVBTScanParams.ofdm, "16K", sizeof ("16K"));
					break;

				case eDxTER_T2TRANSMODE_32K:
					HxSTD_StrNCpy(pstOplTunedParam->data.stDVBTScanParams.ofdm, "32K", sizeof ("32K"));
					break;

				case eDxTER_TRANSMODE_AUTO:
					HxSTD_StrNCpy(pstOplTunedParam->data.stDVBTScanParams.ofdm, "AUTO", sizeof ("AUTO"));
					break;
			}

			switch(pstTpInfo->ter.eConstellation)
			{
				case eDxTER_CONSTELLATION_QPSK:	pstOplTunedParam->data.stDVBTScanParams.modulationModes = 1; break;
				case eDxTER_CONSTELLATION_16QAM:	pstOplTunedParam->data.stDVBTScanParams.modulationModes = 4; break;
				case eDxTER_CONSTELLATION_64QAM:	pstOplTunedParam->data.stDVBTScanParams.modulationModes = 16; break;
				case eDxTER_CONSTELLATION_256QAM:	pstOplTunedParam->data.stDVBTScanParams.modulationModes = 256; break;
				case eDxTER_CONSTELLATION_AUTO:	pstOplTunedParam->data.stDVBTScanParams.modulationModes = 0; break;
			}

			switch(pstTpInfo->ter.eBandWidth)
			{
				case eDxTER_T2BANDWIDTH_1P7MHZ:
					HxSTD_StrNCpy(pstOplTunedParam->data.stDVBTScanParams.bandwidth, "1.7Mhz", sizeof ("1.7Mhz"));
					break;

				case eDxTER_T2BANDWIDTH_5MHZ:
					HxSTD_StrNCpy(pstOplTunedParam->data.stDVBTScanParams.bandwidth, "5Mhz", sizeof ("5Mhz"));
					break;

				case eDxTER_BANDWIDTH_6M:
					HxSTD_StrNCpy(pstOplTunedParam->data.stDVBTScanParams.bandwidth, "6Mhz", sizeof ("6Mhz"));
					break;

				case eDxTER_BANDWIDTH_7M:
					HxSTD_StrNCpy(pstOplTunedParam->data.stDVBTScanParams.bandwidth, "7Mhz", sizeof ("7Mhz"));
					break;

				case eDxTER_BANDWIDTH_8M:
					HxSTD_StrNCpy(pstOplTunedParam->data.stDVBTScanParams.bandwidth, "8Mhz", sizeof ("8Mhz"));
					break;

				case eDxTER_T2BANDWIDTH_10MHZ:
					HxSTD_StrNCpy(pstOplTunedParam->data.stDVBTScanParams.bandwidth, "10Mhz", sizeof ("10Mhz"));
					break;
			}
#endif
		}
		else if (eDxDELIVERY_TYPE_SAT == eTunerType)
		{
			pstOplTunedParam->data.stDVBSScanParams.antennaId = pstTpInfo->sat.tuningInfo.antuid;
			// we have to convert, more detailly...
			pstOplTunedParam->data.stDVBSScanParams.startFreq = pstTpInfo->sat.tuningInfo.ulFrequency;
			pstOplTunedParam->data.stDVBSScanParams.endFreq = pstTpInfo->sat.tuningInfo.ulFrequency;
			pstOplTunedParam->data.stDVBSScanParams.modulationModes = pstTpInfo->sat.tuningInfo.ePskMod;
			pstOplTunedParam->data.stDVBSScanParams.polarisation = pstTpInfo->sat.tuningInfo.ePolarization;

			HxSTD_snprintf(pstOplTunedParam->data.stDVBSScanParams.symbolRate,MAX_SYMBOLRATE_LEN,"%d",(pstTpInfo->sat.tuningInfo.ulSymbolRate));
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
			HLIB_STD_MemFree(pstOplTunedParam);
			goto ERR_OUT;
		}

		*ppstOplTunedParam = pstOplTunedParam;

		return BPL_STATUS_OK;
	}

ERR_OUT:
	return BPL_STATUS_ERROR;
}


static	void	_OPL_BUILDER_SendEvent(OPL_SCANEVENT_e evtType, HINT32 type,
							HINT32 compTp, HINT32 compSvc, HINT32 totalTp,
							HINT32 strength, HINT32 quality, HINT32 infoCount,
							HINT32 frequency, OPL_SEARCHEDINFO_t *searchedInfos,
							OPL_SCANPARAMS_t *pstTunedParam)
{
	if (_opl_builder_callback)
	{
		OPL_BUILDEREVENT_t	stScanEvent;

		HxSTD_MemSet(&stScanEvent, 0, sizeof(OPL_BUILDEREVENT_t));
		stScanEvent.eventType = evtType;
		switch (evtType)
		{
			case	eBUILDEREVENT_SCAN:
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
				stScanEvent.u.connectionEvent.type = type;
				stScanEvent.u.connectionEvent.connType = compSvc;
				if (totalTp > 0)
					stScanEvent.u.connectionEvent.progress = compTp * 100 / totalTp;
				else
					stScanEvent.u.connectionEvent.progress = 0;
				break;
		}

		_opl_builder_callback((void*)&stScanEvent);
	}
}



static	void	_OPL_BUILDER_Notifier(HUINT32 ulBuilderId, DxBuilder_Event_e eEvent, DxBuilder_NotifierData_t *pstNotifyData, void *pvUserData)
{
	BPLStatus	nBplStatus;
	HERROR		err;
	HINT32		nResultNo, i;
	HINT32		nSignalStrength, nSignalQuality;
	HUINT32		ulTotalTuningNum, ulCompleteTpNum, ulSearchedServiceNum, ulSearchState, ulFrequency;
	DxBuilder_SearchedInfo_t	*pstDetailInfo;
	OPL_SEARCHEDINFO_t			*pstSearchedInfo = NULL;
	OPL_SCANPARAMS_t			*pstTunedParam = NULL;
	DxDeliveryType_e	eTunerType;
	DxTuneParam_t			 stTpInfo;

	HxLOG_Trace();

	err = APK_BUILDER_GetBuilderStatus(_builder_handle, &ulTotalTuningNum, &ulCompleteTpNum, &ulSearchedServiceNum, &ulSearchState, &ulFrequency, &eTunerType, &stTpInfo);

	if (err != ERR_OK)
	{
		HxLOG_Debug("APK_BUILDER_GetBuilderStatus() fail \n");
		goto ERROR;
	}

	err = APK_BUILDER_GetBuilderSignalStatus(_builder_handle, &nSignalStrength, &nSignalQuality);
	if (err != ERR_OK)
	{
		HxLOG_Debug("APK_BUILDER_GetBuilderSignalStatus() fail \n");
		goto ERROR;
	}

	HxLOG_Debug("nSignalStrength: %d, nSignalQuality: %d \n", nSignalStrength, nSignalQuality);

	err = APK_BUILDER_GetBuilderSvcInfo(_builder_handle, 0, ulSearchedServiceNum, &nResultNo, &pstDetailInfo);
	if (err != ERR_OK)
	{
		HxLOG_Debug("APK_BUILDER_GetBuilderSvcInfo() fail \n");
		goto ERROR;
	}

	if (nResultNo > 0)
	{
		pstSearchedInfo = (OPL_SEARCHEDINFO_t*)HLIB_STD_MemCalloc(sizeof(OPL_SEARCHEDINFO_t) * nResultNo);
		for (i = 0; i < nResultNo; i++)
		{
			pstSearchedInfo[i].onId			= (HINT32)pstDetailInfo[i].onId;
			pstSearchedInfo[i].tsId			= (HINT32)pstDetailInfo[i].tsId;
			pstSearchedInfo[i].svcId 		= (HINT32)pstDetailInfo[i].svcId;
			pstSearchedInfo[i].channelType	= (HINT32)pstDetailInfo[i].ulSvcType;
			pstSearchedInfo[i].casType		= (HINT32)pstDetailInfo[i].ulCasType;
			pstSearchedInfo[i].lcn			= (HINT32)pstDetailInfo[i].ulLcn;
			HxSTD_StrNCpy(pstSearchedInfo[i].name, (HCHAR *)pstDetailInfo[i].szSvcName, MAX_INFO_LEN - 1);
		}
	}
	if (pstDetailInfo)
	{
		APK_BUILDER_FreeBuilderSvcInfo(pstDetailInfo);
	}

	nBplStatus = _OPL_BUILDER_ConvertTunedParam(eTunerType, &stTpInfo, &pstTunedParam);
	// we don't need check if it(pstTunedParam) is null, it is ok...

	//	TODO: make detail info to Event info.

	HxLOG_Debug("ulCompleteTpNum: %d, ulTotalTuningNum: %d \n", ulCompleteTpNum, ulTotalTuningNum);

	if ((ulCompleteTpNum == ulTotalTuningNum) && (ulTotalTuningNum))
	{
#if defined(CONFIG_OP_JAPAN)
		/*	일본향에서는 progress bar 또는 진행 퍼센트 표시가 어렵다. 검색 속도를 높이기 위해 octo 내부에서 TP list를 skip 하는 처리가 있다.
			실제 scan 과정에서도 ulCompleteTpNum == ulTotalTuningNum가 같은 상황이 여러번 발생할 수 있기 때문에 state가 finish인 경우만 save 하기 위해 state를 octo로부터 받음. */
		if((s_bScanFinish == FALSE) && (ulSearchState == APKE_BUILDER_STATE_FINISH))
		{
			s_bScanFinish = TRUE;

			HxLOG_Debug("Scan success - LoadFromOcto and Save\n");
			OPL_Builder_ControlScan("load");
			OPL_Builder_ControlScan("reset");
			OPL_Builder_ControlScan("save");
			OPL_Builder_ControlScan("setadditionalnum");
			_OPL_BUILDER_SendEvent(eBUILDEREVENT_SCAN, eBUILDER_COMPLETE, ulCompleteTpNum, ulSearchedServiceNum, ulTotalTuningNum, nSignalStrength, nSignalQuality, nResultNo, ulFrequency, pstSearchedInfo, pstTunedParam);
		}
#else
		HxLOG_Debug("send eBUILDER_COMPLETE, : ulSearchedServiceNum: %d, nSignalStrength: %d, nSignalQuality: %d, nResultNo: %d, ulFrequency: %d \n", ulCompleteTpNum, nSignalStrength, nSignalQuality, nResultNo, ulFrequency);
		_OPL_BUILDER_SendEvent(eBUILDEREVENT_SCAN, eBUILDER_COMPLETE, ulCompleteTpNum, ulSearchedServiceNum, ulTotalTuningNum, nSignalStrength, nSignalQuality, nResultNo, ulFrequency, pstSearchedInfo, pstTunedParam);
#endif

	}
	else
	{
		HxLOG_Debug("send eBUILDER_INDICATE_PROGRESS, : ulSearchedServiceNum: %d, nSignalStrength: %d, nSignalQuality: %d, nResultNo: %d, ulFrequency: %d \n", ulCompleteTpNum, nSignalStrength, nSignalQuality, nResultNo, ulFrequency);
		_OPL_BUILDER_SendEvent(eBUILDEREVENT_SCAN, eBUILDER_INDICATE_PROGRESS, ulCompleteTpNum, ulSearchedServiceNum, ulTotalTuningNum, nSignalStrength, nSignalQuality, nResultNo, ulFrequency, pstSearchedInfo, pstTunedParam);
	}
	return;

ERROR:
	HxLOG_Warning("\n\t%s(ERROR!!!!!!!!!!!!!!!!!!)\n", __FUNCTION__);
	_OPL_BUILDER_SendEvent(eBUILDEREVENT_SCAN, eBUILDER_COMPLETE, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
}

static	void	_OPL_BUILDER_NotifierConnection(HUINT32 ulBuilderId, DxBuilder_Event_e eEvent, DxBuilder_NotifierData_t *pstNotifyData, void *pvUserData)
{
	HUINT32		ulTotalTuningNum, ulCompleteTpNum, ulSearchedServiceNum, ulSearchState, ulFrequency;
	DxDeliveryType_e	eTunerType;
	DxTuneParam_t			 stTpInfo;
	OPL_SCANPARAMS_t		*pstTunedParam = NULL;

	APK_BUILDER_GetBuilderStatus(_builder_handle, &ulTotalTuningNum, &ulCompleteTpNum, &ulSearchedServiceNum, &ulSearchState, &ulFrequency, &eTunerType, &stTpInfo);

	_OPL_BUILDER_ConvertTunedParam(eTunerType, &stTpInfo, &pstTunedParam);
	// we don't need check if it(pstTunedParam) is null, it is ok...

	if (ulCompleteTpNum == ulTotalTuningNum)
		_OPL_BUILDER_SendEvent(eBUILDEREVENT_CONNECTION, eBUILDER_COMPLETE, ulCompleteTpNum, ulSearchedServiceNum, ulTotalTuningNum, 0, 0, 0, 0, NULL, pstTunedParam);
	else
		_OPL_BUILDER_SendEvent(eBUILDEREVENT_CONNECTION, eBUILDER_INDICATE_PROGRESS, ulCompleteTpNum, ulSearchedServiceNum, ulTotalTuningNum, 0, 0, 0, 0, NULL, pstTunedParam);
}


/*********************************************************************************************************
	Public Functions
*********************************************************************************************************/
//BPLStatus	OPL_Builder_RegisterListener(

BPLStatus	OPL_Builder_RegisterListener(int (*cbFunc)(void *pstData))
{
	_opl_builder_callback = cbFunc;

	return BPL_STATUS_OK;
}

BPLStatus	OPL_Builder_UnRegisterListener(void)
{
	_opl_builder_callback = NULL;

	return BPL_STATUS_OK;
}

BPLStatus	OPL_Builder_StartScan(HCHAR *scanType, OPL_SCANOPTIONS_t *pstOptions, HINT32 num, OPL_SCANPARAMS_t *pstParams)
{
	HUINT32 	handle = INVALID_BUILDER_HANDLE;
	HxList_t	*list;
	HBOOL		bUserScanOption;

	ApkBuilder_Action_t		stBuilderAction;
	DxBuilder_Option_t		stBuilderOption;

	HxLOG_Trace();
	HxLOG_Debug("OPL_Builder_StartScan : Input TP Count [%d]\n", num);

#if defined(CONFIG_OP_JAPAN)
	s_bScanFinish = FALSE;
#endif

/********************************************************************************************************

	2015-01-20, webapp - octo 서치 관련 협의 내용

	From: 이호(Tim Yi)
	Sent: Tuesday, January 20, 2015 1:29 PM
	To: 강민석(Min-Seok Kang, SW); 조동원(Dong-Won Cho)
	Cc: 지승용(Seung-Yong Chi); 구지혜(Jihye Gu); 강희완(Heewan Kang)
	Subject: RE: [문의] Search Parameter 관련

	방금 강민석 사원과 전화통화한 내용 기반으로 정리를 해보겠습니다.

	UI에서 설정되는 값 두 가지
	1)	Network Search - On/Off
	2)	Network Id - int 값

	현재 사용 중인 Param
	1)	위성/지상파: enableNetworkScan (OPL_SCANOPTIONS_t 구조체)
	2)	케이블: startNoetworkScanOnNIT, networkId (OPL_SCANPARAMS_t 내의 DVBC_SCANPARAMS_t 구조체)

	종합해보면 케이블 모델의 경우,
	           UI에서 설정되는 값이 결국에 startNoetworkScanOnNIT, networkId
	           이 쪽으로 들어옵니다.

	케이블 서치의 경우
	Webapp 수정 없이, 일단 startNoetworkScanOnNIT 값으로 활용되도록 OPL 이하 코드 정리하겠습니다.
	다른 의견 있으시면 말씀 주시고요.
	앞으로 케이블 모델 파생 시 가이드 부탁드려요.
	그리고 케이블 이외에, Network Id 넣고 NIT Other 서치를 하는 경우도 마찬가지로 처리하면 될 듯해요.

********************************************************************************************************/

	bUserScanOption = FALSE;
	if ((HLIB_STD_StrCaseCmp(scanType, OPLSCANTYPE_NORMAL) == 0) ||
		(HLIB_STD_StrCaseCmp(scanType, OPLSCANTYPE_FSATUSERSCAN) == 0) ||
		(HLIB_STD_StrCaseCmp(scanType, OPLSCANTYPE_FSATPOSTCODESKIP) == 0) ||
		(HLIB_STD_StrCaseCmp(scanType, OPLSCANTYPE_ERTSEARCH) == 0))
	{
		bUserScanOption = TRUE;
	}

	HxLOG_Print(HxANSI_COLOR_YELLOW("[SCAN_OPTION] enableNetworkScan = %d\n"), pstOptions->enableNetworkScan);
	HxLOG_Print(HxANSI_COLOR_YELLOW("[SCAN_PARAM] %s, %d ~ %d Khz, startNoetworkScanOnNIT(%d), NID = %d, bUserScanOption = %d\n")
		, scanType
		, pstParams->data.stDVBCScanParams.startFreq
		, pstParams->data.stDVBCScanParams.endFreq
		, pstParams->data.stDVBCScanParams.startNoetworkScanOnNIT
		, pstParams->data.stDVBCScanParams.networkId
		, bUserScanOption
		);

	if (bUserScanOption)
	{
		_OPL_BUILDER_SetOptions(pstOptions, &stBuilderOption);
		list = _OPL_BUILDER_SetScanParams(num, pstParams, &stBuilderAction, &stBuilderOption);

		HxLOG_Print(HxANSI_COLOR_YELLOW("stBuilderOption.bNetworkSearch = %d\n"), stBuilderOption.bNetworkSearch);
		HxLOG_Print(HxANSI_COLOR_YELLOW("stBuilderOption.bWithNetworkId = %d\n"), stBuilderOption.bWithNetworkId);
		HxLOG_Print(HxANSI_COLOR_YELLOW("stBuilderOption.ulNetworkId    = %d\n"), stBuilderOption.ulNetworkId);
	}
	else
	{
		list = _OPL_BUILDER_SetScanParams(num, pstParams, &stBuilderAction, NULL);
	}

	APK_BUILDER_OpenSession(stBuilderAction.eDeliveryType, scanType, &stBuilderAction, &handle);
	if (handle == INVALID_BUILDER_HANDLE)
	{
		HxLOG_Critical("other builder action is now running...\n");
		return  BPL_STATUS_ERROR;
	}

	_builder_handle = handle;
	if (bUserScanOption)
	{
		APK_BUILDER_SetOption(_builder_handle, &stBuilderOption);
	}

	while (list)
	{
		DxTuneParam_t	*pstInfo = (DxTuneParam_t*)HLIB_LIST_Data(list);

		APK_BUILDER_AddBuildInfo(_builder_handle, pstInfo);
		HLIB_STD_MemFree(pstInfo);

		list = list->next;
	}
	HLIB_LIST_RemoveAll(list);

	if ((HLIB_STD_StrCaseCmp(scanType, OPLSCANTYPE_NORMAL) == 0) ||
		(HLIB_STD_StrCaseCmp(scanType, OPLSCANTYPE_FSATUSERSCAN) == 0) ||
		(HLIB_STD_StrCaseCmp(scanType, OPLSCANTYPE_FSATPOSTCODESKIP) == 0) ||
		(HLIB_STD_StrCaseCmp(scanType, OPLSCANTYPE_ERTSEARCH) == 0))
	{
		APK_BUILDER_RegisterNotifier(_builder_handle, _OPL_BUILDER_Notifier, NULL);
		//	Send event for search is now starting...
		_OPL_BUILDER_SendEvent(eBUILDEREVENT_SCAN, eBUILDER_STARTED, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);
	}
	else
	{
		APK_BUILDER_RegisterNotifier(_builder_handle, _OPL_BUILDER_NotifierConnection, NULL);
	}

	APK_BUILDER_StartScan(_builder_handle);

	return BPL_STATUS_OK;
#if	0
	_OPL_BUILDER_SetOptions(pstOptions, &stBuilderOption);
	list = _OPL_BUILDER_SetScanParams(num, pstParams, &stBuilderAction, &stBuilderOption);

	APK_BUILDER_OpenSession(stBuilderAction.eDeliveryType, scanType, &stBuilderAction, &handle);
	if (handle == NULL)
	{
		HxLOG_Critical("other builder action is now running....\n");
		return BPL_STATUS_ERROR;
	}
	_builder_handle = handle;

	APK_BUILDER_SetOption(_builder_handle, &stBuilderOption);

	while (list)
	{
		DxTuneParam_t	*pstInfo = (DxTuneParam_t*)HLIB_LIST_Data(list);

		APK_BUILDER_AddBuildInfo(_builder_handle, pstInfo);
		HLIB_STD_MemFree(pstInfo);

		list = list->next;
	}
	HLIB_LIST_RemoveAll(list);

	APK_BUILDER_RegisterNotifier(_builder_handle, _OPL_BUILDER_Notifier, NULL);

	//	Send event for search is now starting...
	_OPL_BUILDER_SendEvent(eBUILDEREVENT_SCAN, eBUILDER_STARTED, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);

	APK_BUILDER_StartScan(_builder_handle);

	return BPL_STATUS_OK;
#endif
}

BPLStatus	OPL_Builder_StopScan(void)
{
	HxLOG_Trace();

	if (_builder_handle)
	{
		const HCHAR	*scanType;

		scanType = APK_BUILDER_GetScanType(_builder_handle);
		if ((HLIB_STD_StrCaseCmp(scanType, OPLSCANTYPE_NORMAL) == 0) ||
			(HLIB_STD_StrCaseCmp(scanType, OPLSCANTYPE_FSATUSERSCAN) == 0) ||
			(HLIB_STD_StrCaseCmp(scanType, OPLSCANTYPE_FSATPOSTCODESKIP) == 0) ||
			(HLIB_STD_StrCaseCmp(scanType, OPLSCANTYPE_ERTSEARCH) == 0))
		{
			APK_BUILDER_UnregisterNotifier(_builder_handle, _OPL_BUILDER_Notifier, NULL);
		}
		else
		{
			APK_BUILDER_UnregisterNotifier(_builder_handle, _OPL_BUILDER_NotifierConnection, NULL);
		}

		APK_BUILDER_StopScan(_builder_handle);

#if defined(CONFIG_OP_JAPAN)
		/* 검색 취소에 대응하기 위해 기존 meta service를 load함. */
		/* APK_BUILDER_CloseSession() 전에 호출되어야 save 처리 됨. */
		if(s_bScanFinish == FALSE)
		{
			HCHAR		buf[256];
			/* controlScan 정의되어 있지 않아 scan complete 시에 save하도록 함. */
//			APK_META_SVC_Load(HLIB_STD_FmtString(buf, 256, "%s/%s", APKD_STORAGE_PATH_BACKUP, APKD_STORAGE_SERVICE));
			OPL_Builder_ControlScan("save");
		}
#endif

		APK_BUILDER_CloseSession(_builder_handle);
		_builder_handle = INVALID_BUILDER_HANDLE;

		_OPL_BUILDER_SendEvent(eBUILDEREVENT_SCAN, eBUILDER_ABORTED, 0, 0, 0, 0, 0, 0, 0, NULL, NULL);

		return BPL_STATUS_OK;
	}

	return BPL_STATUS_ERROR;
}

BPLStatus	OPL_Builder_ControlScan(HCHAR *controlScan)
{
	const HCHAR	*scanType;

	if (_builder_handle == INVALID_BUILDER_HANDLE)
	{
		HxLOG_Critical("Builder is already destroyed..\nThe Function will be ignored...\n");
		return BPL_STATUS_ERROR;
	}

	scanType = APK_BUILDER_GetScanType(_builder_handle);


	if (HLIB_STD_StrCaseCmp(controlScan, "pause") == 0)
	{
		APK_BUILDER_PauseScan(_builder_handle);
	}
	else	if (HLIB_STD_StrCaseCmp(controlScan, "resume") == 0)
	{
		APK_BUILDER_ResumeScan(_builder_handle);
	}
	else	if (HLIB_STD_StrCaseCmp(controlScan, "save") == 0)
	{
		if ((HLIB_STD_StrCaseCmp(scanType, OPLSCANTYPE_NORMAL) == 0) ||
			(HLIB_STD_StrCaseCmp(scanType, OPLSCANTYPE_FSATPOSTCODESKIP) == 0) ||
			(HLIB_STD_StrCaseCmp(scanType, OPLSCANTYPE_ERTSEARCH) == 0))
		{
			APK_META_SVC_Save();
		}
		else if (HLIB_STD_StrCaseCmp(scanType, OPLSCANTYPE_FSATUSERSCAN) == 0)
		{
			APK_META_SVC_RemoveServiceWithFlag(eDxSVC_UPDATE_READONLY);
			APK_META_SVC_Save();
		}
		else if (HLIB_STD_StrCaseCmp(scanType, OPLSCANTYPE_DISEQCDETECTION) == 0)
		{
			APK_META_ANTENNA_Save();
		}
		else
		{
			return BPL_STATUS_ERROR;
		}
	}
	else	if (HLIB_STD_StrCaseCmp(controlScan, "load") == 0)
	{
		if ((HLIB_STD_StrCaseCmp(scanType, OPLSCANTYPE_NORMAL) == 0) ||
			(HLIB_STD_StrCaseCmp(scanType, OPLSCANTYPE_FSATUSERSCAN) == 0) ||
			(HLIB_STD_StrCaseCmp(scanType, OPLSCANTYPE_FSATPOSTCODESKIP) == 0) ||
			(HLIB_STD_StrCaseCmp(scanType, OPLSCANTYPE_ERTSEARCH) == 0))
		{
#if defined(CONFIG_SUPPORT_MHEGPLAYER_APP)
			HAPI_BroadcastSignal("signal:onLoadServiceListToMheg", 1,"i", 0);
#endif
			APK_META_SVC_LoadupdatedList();
		}
		else if (HLIB_STD_StrCaseCmp(scanType, OPLSCANTYPE_DISEQCDETECTION) == 0)
		{
			APK_META_ANTENNA_RemoveAll();
			APK_META_ANTENNA_Load();
		}
		else
		{
			return BPL_STATUS_ERROR;
		}
	}
	else if (HLIB_STD_StrCaseCmp(controlScan, "setadditionalnum") == 0)
	{
		APK_BUILDER_SetAdditionalNumbers(_builder_handle);
	}
	else if (HLIB_STD_StrCaseCmp(controlScan, "loadDefChannels") == 0)
	{
		APK_META_SVC_LoadPreferredList();
	}
	else if (HLIB_STD_StrCaseCmp(controlScan, "reset") == 0)
	{
		APK_META_SVC_Reset();
	}
	else
	{
		return BPL_STATUS_ERROR;
	}

	return BPL_STATUS_OK;
}

BPLStatus	OPL_Builder_Motor_GotoPosition(HINT32 position)
{
	APK_BUILDER_MotorGotoPosition(_builder_handle, position);

	return BPL_STATUS_OK;
}

BPLStatus	OPL_Builder_Motor_StorePosition(HINT32 position)
{
	APK_BUILDER_MotorStorePosition(_builder_handle, position);

	return BPL_STATUS_OK;
}

BPLStatus	OPL_Builder_Motor_ResetPosition(void)  // MOON_motor
{
	APK_BUILDER_MotorResetPosition(_builder_handle);

	return BPL_STATUS_OK;
}

BPLStatus	OPL_Builder_Motor_Recalculate(HINT32 position)
{
	APK_BUILDER_MotorRecalculate(_builder_handle, position);

	return BPL_STATUS_OK;
}

BPLStatus	OPL_Builder_Motor_Move(HINT32 direction, HINT32 mode, HINT32 step)
{
	APK_BUILDER_MotorMove(_builder_handle, direction, mode, step);

	return BPL_STATUS_OK;
}

BPLStatus	OPL_Builder_Motor_Stop(void)
{
	APK_BUILDER_MotorStop(_builder_handle);

	return BPL_STATUS_OK;
}

BPLStatus	OPL_Builder_Motor_SetLimit(HINT32 direction)
{
	APK_BUILDER_MotorSetLimit(_builder_handle, direction);

	return BPL_STATUS_OK;
}

BPLStatus	OPL_Builder_Motor_EnableLimit(void)
{
	APK_BUILDER_MotorEnableLimit(_builder_handle);

	return BPL_STATUS_OK;
}

BPLStatus	OPL_Builder_Motor_DisableLimit(void)
{
	APK_BUILDER_MotorDisableLimit(_builder_handle);

	return BPL_STATUS_OK;
}

BPLStatus	OPL_Builder_Motor_GotoDegree(HINT32 degree)
{
	APK_BUILDER_MotorGotoDegree(_builder_handle, degree);

	return BPL_STATUS_OK;
}

BPLStatus	OPL_Builder_Motor_GotoSatellite(HINT32 satellite)
{
	APK_BUILDER_MotorGotoSatellite(_builder_handle, satellite);

	return BPL_STATUS_OK;
}

BPLStatus	OPL_Builder_Motor_SetMyLatitude(HINT32 mylatitude)
{
	APK_BUILDER_MotorSetMyLatitude(mylatitude);

	return BPL_STATUS_OK;
}

BPLStatus	OPL_Builder_Motor_SetMyLongitude(HINT32 mylongitude)
{
	APK_BUILDER_MotorSetMyLongitude(mylongitude);

	return BPL_STATUS_OK;
}

HINT32	OPL_Builder_Motor_GetMyLatitude(void)
{
	HINT32	latitude;

	APK_BUILDER_MotorGetMyLatitude(&latitude);

	return latitude;
}

HINT32	OPL_Builder_Motor_GetMyLongitude(void)
{
	HINT32	longitude;

	APK_BUILDER_MotorGetMyLongitude(&longitude);

	return longitude;
}

HINT32	OPL_Builder_SetSelectedConflictLcn(OPL_SEARCHEDINFO_t *selectedInfos, int count)
{
	HERROR	hErr = BPL_STATUS_ERROR;
	int i;
	DxBuilder_SearchedInfo_t *dxSelectedInfos = (DxBuilder_SearchedInfo_t*)HLIB_STD_MemAlloc(sizeof(DxBuilder_SearchedInfo_t) * count);

	if (dxSelectedInfos)
	{
		for (i = 0; i < count; i++)
		{
			dxSelectedInfos[i].onId = selectedInfos[i].onId;
			dxSelectedInfos[i].tsId = selectedInfos[i].tsId;
			dxSelectedInfos[i].svcId = selectedInfos[i].svcId;
			dxSelectedInfos[i].ulSvcType = selectedInfos[i].channelType;
			dxSelectedInfos[i].ulCasType = selectedInfos[i].casType;
			dxSelectedInfos[i].ulLcn = selectedInfos[i].lcn;
			HxSTD_StrNCpy(dxSelectedInfos[i].szSvcName, (HUINT8*)selectedInfos[i].name, DxBUILDER_SVC_NAME_LEN-1); dxSelectedInfos[i].szSvcName[DxBUILDER_SVC_NAME_LEN-1] = '\0';
		}

		hErr = APK_BUILDER_SetSelectedConflictLcn(_builder_handle, dxSelectedInfos, count);
	}

	return hErr;
}

#endif //CONFIG_OP_JAPAN

/* end of file */
