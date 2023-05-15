/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  svc_si_lib_delivery.c
	@brief

	Description:  									\n
	Module: SVC / SI_NEW					\n

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
#include <silib.h>
#include <octo_common.h>
#include <pal_sirawpool.h>
#include <db_svc.h>
#include <isosvc.h>
#include <svc_si.h>
//#include "../../si/local_include/si_service.h"
#include <_svc_si.h>
#include <_svc_si_lib.h>

#include <psvc_si.h>


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
#define ___LOCAL_FUNCTIONS___
STATIC HUINT8 svc_si_LibConvertSatDeliDesToPalPolar (HUINT32 ulValue)
{
	switch (ulValue)
	{
	case 0x00:		return eDxSAT_POLAR_HOR;
	case 0x01:		return eDxSAT_POLAR_VER;
	case 0x02:		return eDxSAT_POLAR_LEFT;
	case 0x03:		return eDxSAT_POLAR_RIGHT;
	default:		break;
	}

	return eDxSAT_POLAR_AUTO;
}

STATIC HUINT8 svc_si_LibConvertSatDeliDesToPalFec (HUINT32 ulValue)
{
	switch (ulValue)
	{
	case 0x01:		return eDxSAT_CODERATE_1_2;
	case 0x02:		return eDxSAT_CODERATE_2_3;
	case 0x03:		return eDxSAT_CODERATE_3_4;
	case 0x04:		return eDxSAT_CODERATE_5_6;
	case 0x05:		return eDxSAT_CODERATE_7_8;
	case 0x06:		return eDxSAT_CODERATE_8_9;
	case 0x07:		return eDxSAT_CODERATE_3_5;
	case 0x08:		return eDxSAT_CODERATE_4_5;
	case 0x09:		return eDxSAT_CODERATE_9_10;
	case 0x0F:		return eDxSAT_CODERATE_NONE;
	default:		break;
	}

	return eDxSAT_CODERATE_AUTO;
}

STATIC HUINT8 svc_si_LibConvertSatDeliDesToPalTransspec (HUINT32 ulValue)
{
	switch (ulValue)
	{
	case 0x00:		return eDxSAT_TRANS_DVBS;
	case 0x01:		return eDxSAT_TRANS_DVBS2;
	default:		break;
	}

	return eDxSAT_TRANS_DVBS;
}

STATIC HUINT8 svc_si_LibConvertSatDeliDesToPalRollOff (HUINT32 ulValue)
{
	switch (ulValue)
	{
	case 0x00:		return eDxSAT_ROLL_035;
	case 0x01:		return eDxSAT_ROLL_025;
	case 0x02:		return eDxSAT_ROLL_020;
	case 0x03:		return eDxSAT_ROLL_035;
	default:		break;
	}

	return eDxSAT_ROLL_035;
}

STATIC HUINT8	svc_si_LibConvertSatDeliDesToPalModType (HUINT32 ulValue)
{
	switch (ulValue)
	{
	case 0x00:		return eDxSAT_PSK_AUTO;
	case 0x01:		return eDxSAT_PSK_QPSK;
	case 0x02:		return eDxSAT_PSK_8PSK;
	case 0x03:		return eDxSAT_PSK_AUTO;	// 16 QAM (Not available for DVB-S2)
	default:		break;
	}

	return eDxSAT_PSK_AUTO;
}

// Tuning Parameter Converting function : Terrestrial
STATIC HUINT8 svc_si_LibConvertCabDeliDesToPalQam (HUINT32 ulValue)
{
	switch (ulValue)
	{
	case 0x01:		return eDxCAB_CONSTELLATION_16QAM;
	case 0x02:		return eDxCAB_CONSTELLATION_32QAM;
	case 0x03:		return eDxCAB_CONSTELLATION_64QAM;
	case 0x04:		return eDxCAB_CONSTELLATION_128QAM;
	case 0x05:		return eDxCAB_CONSTELLATION_256QAM;
	default:		return eDxCAB_CONSTELLATION_AUTO;
	}
}

// Tuning Parameter Converting function : Terrestrial
STATIC HUINT8 svc_si_LibConvertTerDeliDesToPalCodeRate (HUINT32 ulValue)
{
	switch (ulValue)
	{
	case 0x00:		return eDxTER_CODERATE_1_2;
	case 0x01:		return eDxTER_CODERATE_2_3;
	case 0x02:		return eDxTER_CODERATE_3_4;
	case 0x03:		return eDxTER_CODERATE_5_6;
	case 0x04:		return eDxTER_CODERATE_7_8;
	default:		return eDxTER_CODERATE_AUTO;
	}
}

STATIC HUINT8 svc_si_LibConvertTerDeliDesToPalTransMode (HUINT32 ulValue)
{
	switch(ulValue)
	{
	case 0x00:		return eDxTER_TRANSMODE_2K;
	case 0x01:		return eDxTER_TRANSMODE_8K;
	default:		return eDxTER_TRANSMODE_AUTO;
	}
}

STATIC HUINT8 svc_si_LibConvertTerDeliDesToPalGuardInterval (HUINT32 ulValue)
{
	switch (ulValue)
	{
	case 0x00:		return eDxTER_GUARDINTERVAL_1_32;
	case 0x01:		return eDxTER_GUARDINTERVAL_1_16;
	case 0x02:		return eDxTER_GUARDINTERVAL_1_8;
	case 0x03:		return eDxTER_GUARDINTERVAL_1_4;
	default:		return eDxTER_GUARDINTERVAL_AUTO;
	}
}

STATIC HUINT8 svc_si_LibConvertTerDeliDesToPalConstellation (HUINT32 ulValue)
{
	switch (ulValue)
	{
	case 0x00:		return eDxTER_CONSTELLATION_QPSK;
	case 0x01:		return eDxTER_CONSTELLATION_16QAM;
	case 0x02:		return eDxTER_CONSTELLATION_64QAM;
	default:		return eDxTER_CONSTELLATION_AUTO;
	}
}

STATIC HUINT8 svc_si_LibConvertTerDeliDesToPalHierarchy (HUINT32 ulValue)
{
	switch (ulValue)
	{
	case 0x00:		return eDxTER_HIERACHY_NONE;
	case 0x01:		return eDxTER_HIERACHY_1;
	case 0x02:		return eDxTER_HIERACHY_2;
	case 0x03:		return eDxTER_HIERACHY_4;
	default:		return eDxTER_HIERACHY_AUTO;
	}
}

STATIC HUINT8 svc_si_LibConvertTerDeliDesToPalBandWidth (HUINT32 ulValue)
{
	switch (ulValue)
	{
	case 0x00:		return eDxTER_BANDWIDTH_8M;
	case 0x01:		return eDxTER_BANDWIDTH_7M;
	case 0x02:		return eDxTER_BANDWIDTH_6M;
	default:		return eDxTER_BANDWIDTH_8M;
	}
}

STATIC HERROR svc_si_LibGetFreqFromFrequencyListDes (SIxFrequencyListDes_t *pstFreqDes, HUINT32 ulIndex, HUINT32 *pulFreq)
{
	if (pstFreqDes == NULL)								{ return ERR_FAIL; }
	if (pulFreq == NULL)								{ return ERR_FAIL; }

	if ((HUINT32)pstFreqDes->ucNumFreq <= ulIndex)		{ return ERR_FAIL; }

	switch (pstFreqDes->ucCodingType)
	{
	case 0x01:					// Satellite
		*pulFreq = (pstFreqDes->aulCentreFreq[ulIndex] / 100);				// 10000Hz Unit -> MHz
		break;

	case 0x02:					// Cable
		*pulFreq = (pstFreqDes->aulCentreFreq[ulIndex] / 10);				// 100Hz Unit -> KHz
		break;

	case 0x03:
		*pulFreq = (pstFreqDes->aulCentreFreq[ulIndex] / 100);				// 10Hz Unit -> KHz
		break;

	default:
		return ERR_FAIL;
	}

	return ERR_OK;
}


#define ___MEMBER_FUNCTIONS___
HERROR svc_si_LibGetAllDeliveryTsInfoFromNit (HUINT32 ulActionId, HBOOL bNitAct, HBOOL bIncludeFreqList, HUINT32 *pulTsNum, DbSvc_TsInfo_t **ppstTsArray)
{
	HUINT32					 ulTotalCnt = 0;
	HxList_t				*pstItem1, *pstItem2;
	DbSvc_TsInfo_t			*pstTsArray = NULL;
	SIxTable_PsiTable_t		*pstNitTable = NULL;
	HERROR					 hErr, hResult = ERR_FAIL;

	if ((NULL == pulTsNum) || (NULL == ppstTsArray))
	{
		HxLOG_Error ("Invalid paremeters \n");
		goto END_FUNC;
	}

	if (bNitAct == TRUE)
	{
		pstNitTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_NitAct, SILIB_TABLE_EXT_ID_ALL);
	}
	else
	{
		pstNitTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_NitOth, SILIB_TABLE_EXT_ID_ALL);
	}

	if (pstNitTable == NULL)
	{
		HxLOG_Error ("Invalid pstNitTable \n");
		goto END_FUNC;
	}

	// Memory Allocation 을 위해 대략적인 개수를 잰다.
	for (pstItem1 = pstNitTable->pst2ndLoop; NULL != pstItem1; pstItem1 = pstItem1->next)
	{
		SIxTable_2ndLoop_t		*pst2ndLoop = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pstItem1);

		for (pstItem2 = pst2ndLoop->pstDesLoop; NULL != pstItem2; pstItem2 = pstItem2->next)
		{
			HUINT8					*pucDesRaw = (HUINT8 *)HLIB_LIST_Data (pstItem2);

			switch (*pucDesRaw)
			{
#if defined(CONFIG_MW_CH_SATELLITE)
			case eSIxDESCTAG_SATELLITE_DELIVERY_SYSTEM:
				ulTotalCnt ++;
				break;
#endif
#if defined(CONFIG_MW_CH_TERRESTRIAL)
			case eSIxDESCTAG_TERRESTRIAL_DELIVERY_SYSTEM:
				ulTotalCnt ++;
				break;
#endif
#if defined(CONFIG_MW_CH_CABLE)
			case eSIxDESCTAG_CABLE_DELIVERY_SYSTEM:
				ulTotalCnt ++;
				break;
#endif
			case eSIxDESCTAG_FREQUENCY_LIST:
				{
					SIxFrequencyListDes_t	*pstFreqDes = (SIxFrequencyListDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucDesRaw);

					if (NULL != pstFreqDes)
					{
						ulTotalCnt += pstFreqDes->ucNumFreq;
						PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstFreqDes);
					}
				}

				break;

			default:
				break;
			}
		}
	}

	if (0 == ulTotalCnt)
	{
		HxLOG_Error ("No Delivery System... \n");
		goto END_FUNC;
	}

	HxLOG_Debug("ulTotalCnt: %d \n", ulTotalCnt);
	pstTsArray = (DbSvc_TsInfo_t *)OxSI_Malloc (sizeof(DbSvc_TsInfo_t) * ulTotalCnt);
	if (NULL == pstTsArray)
	{
		HxLOG_Error ("Memory allocation failed\n");
		goto END_FUNC;
	}

	ulTotalCnt = 0;

	for (pstItem1 = pstNitTable->pst2ndLoop; NULL != pstItem1; pstItem1 = pstItem1->next)
	{
		HBOOL					 bDsFound = FALSE;
		DbSvc_TsInfo_t			 stTsInfo;
		SIxTable_2ndLoop_t		*pst2ndLoop = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pstItem1);

		HxSTD_MemSet (&stTsInfo, 0, sizeof(DbSvc_TsInfo_t));
		stTsInfo.usOnId = pst2ndLoop->unInfo.stNit.usOrgNetId;
		stTsInfo.usTsId = pst2ndLoop->unInfo.stNit.usTsId;

		for (pstItem2 = pst2ndLoop->pstDesLoop; NULL != pstItem2; pstItem2 = pstItem2->next)
		{
			HUINT8					*pucDesRaw = (HUINT8 *)HLIB_LIST_Data (pstItem2);

			switch (*pucDesRaw)
			{
#if defined(CONFIG_MW_CH_SATELLITE)
			case eSIxDESCTAG_SATELLITE_DELIVERY_SYSTEM:
				{
					SIxSatDeliSystemDes_t	*pstSatDes = (SIxSatDeliSystemDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucDesRaw);

					if (NULL != pstSatDes)
					{
						hErr = svc_si_LibGetTuningInfoFromSatDsd (pstSatDes, &(stTsInfo.stTuningParam));
						if (ERR_OK == hErr)
						{
							bDsFound = TRUE;
							stTsInfo.eDeliType = eDxDELIVERY_TYPE_SAT;
							HxSTD_MemCopy (&(pstTsArray[ulTotalCnt]), &stTsInfo, sizeof(DbSvc_TsInfo_t));

							ulTotalCnt++;
						}

						PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstSatDes);
					}
				}

				break;
#endif
#if defined(CONFIG_MW_CH_TERRESTRIAL)
			case eSIxDESCTAG_TERRESTRIAL_DELIVERY_SYSTEM:
				{
					SIxTerDeliSystemDes_t	*pstTerDes = (SIxTerDeliSystemDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucDesRaw);

					if (NULL != pstTerDes)
					{
						hErr = svc_si_LibGetTuningInfoFromTerDsd(pstTerDes, &(stTsInfo.stTuningParam));
						if (ERR_OK == hErr)
						{
							bDsFound = TRUE;
							stTsInfo.eDeliType = eDxDELIVERY_TYPE_TER;
							HxSTD_MemCopy (&(pstTsArray[ulTotalCnt]), &stTsInfo, sizeof(DbSvc_TsInfo_t));

							ulTotalCnt++;
						}

						PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstTerDes);
					}
				}

				break;
#endif
#if defined(CONFIG_MW_CH_CABLE)
			case eSIxDESCTAG_CABLE_DELIVERY_SYSTEM:
				{
					SIxCabDeliSystemDes_t	*pstCabDes = (SIxCabDeliSystemDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucDesRaw);

					if (NULL != pstCabDes)
					{
						hErr = svc_si_LibGetTuningInfoFromCabDsd (pstCabDes, &(stTsInfo.stTuningParam));
						if (ERR_OK == hErr)
						{
							bDsFound = TRUE;
							stTsInfo.eDeliType = eDxDELIVERY_TYPE_CAB;
							HxSTD_MemCopy (&(pstTsArray[ulTotalCnt]), &stTsInfo, sizeof(DbSvc_TsInfo_t));

							ulTotalCnt++;
						}

						PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstCabDes);
					}
				}

				break;
#endif
			default:
				break;
			}
		}

		if ((TRUE == bIncludeFreqList) && (TRUE == bDsFound))
		{
			for (pstItem2 = pst2ndLoop->pstDesLoop; NULL != pstItem2; pstItem2 = pstItem2->next)
			{
				HUINT8					*pucDesRaw = (HUINT8 *)HLIB_LIST_Data (pstItem2);

				switch (*pucDesRaw)
				{
				case eSIxDESCTAG_FREQUENCY_LIST:
					{
						HUINT32					 ulFreqIdx;
						SIxFrequencyListDes_t	*pstFreqDes = (SIxFrequencyListDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucDesRaw);

						if (NULL != pstFreqDes)
						{
							for (ulFreqIdx = 0; ulFreqIdx < (HUINT32)pstFreqDes->ucNumFreq; ulFreqIdx++)
							{
								switch (stTsInfo.eDeliType)
								{
								case eDxDELIVERY_TYPE_SAT:
									hErr = svc_si_LibGetFreqFromFrequencyListDes (pstFreqDes, ulFreqIdx, &(stTsInfo.stTuningParam.sat.ulFrequency));
									break;

								case eDxDELIVERY_TYPE_TER:
									hErr = svc_si_LibGetFreqFromFrequencyListDes (pstFreqDes, ulFreqIdx, &(stTsInfo.stTuningParam.ter.ulFreq));
									break;

								case eDxDELIVERY_TYPE_CAB:
									hErr = svc_si_LibGetFreqFromFrequencyListDes (pstFreqDes, ulFreqIdx, &(stTsInfo.stTuningParam.cab.ulFreq));
									break;

								default:
									hErr = ERR_FAIL;
									break;
								}

								if (ERR_OK == hErr)
								{
									HxSTD_MemCopy (&(pstTsArray[ulTotalCnt]), &stTsInfo, sizeof(DbSvc_TsInfo_t));
									ulTotalCnt++;
								}
							}

							PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstFreqDes);
						}
					}

					break;

				default:
					break;
				}
			}
		}
	}

	if (ulTotalCnt > 0)
	{
		*pulTsNum		= ulTotalCnt;
		*ppstTsArray	= pstTsArray;
		pstTsArray		= NULL;
		hResult			= ERR_OK;
	}

	HxLOG_Debug("*pulTsNum: %d, addr of *ppstTsArray: %p \n", *pulTsNum, *ppstTsArray);

END_FUNC:
	if (NULL != pstNitTable)				{ PAL_SIRAWPOOL_FreeTable (pstNitTable); }
	if (NULL != pstTsArray)					{ OxSI_Free (pstTsArray); }

	return hResult;
}

HERROR svc_si_LibGetTuningInfoFromSatDsd (SIxSatDeliSystemDes_t *pstDeliDes, DbSvc_TuningInfo_t *pstTuningParam)
{
	if (pstDeliDes == NULL) 				{ return ERR_FAIL; }
	if (pstTuningParam == NULL) 			{ return ERR_FAIL; }

	HxSTD_memset(pstTuningParam, 0, sizeof(DbSvc_TuningInfo_t));

	pstTuningParam->sat.ulFrequency 	 = pstDeliDes->ulFreq / 100;						// MHz
	pstTuningParam->sat.ulSymbolRate	 = (pstDeliDes->ulSymbolRate / 10); 	// KHz
	pstTuningParam->sat.ePolarization	 = svc_si_LibConvertSatDeliDesToPalPolar (pstDeliDes->ucPolar);
	pstTuningParam->sat.eFecCodeRate	 = svc_si_LibConvertSatDeliDesToPalFec (pstDeliDes->ucFECInner);
	pstTuningParam->sat.eTransSpec		 = svc_si_LibConvertSatDeliDesToPalTransspec (pstDeliDes->ucTransSpec);
	pstTuningParam->sat.eRollOff		 = svc_si_LibConvertSatDeliDesToPalRollOff (pstDeliDes->ucRollOff);
	pstTuningParam->sat.ePskMod 		 = svc_si_LibConvertSatDeliDesToPalModType (pstDeliDes->ucModType);
	pstTuningParam->sat.ePilot			 = (pstTuningParam->sat.eTransSpec == eDxSAT_TRANS_DVBS2) ? eDxSAT_PILOT_AUTO : eDxSAT_PILOT_OFF;

	return ERR_OK;
}

HERROR svc_si_LibGetTuningInfoFromCabDsd (SIxCabDeliSystemDes_t *pstDeliDes, DbSvc_TuningInfo_t *pstTuningParam)
{
	if (pstDeliDes == NULL) 				{ return ERR_FAIL; }
	if (pstTuningParam == NULL) 			{ return ERR_FAIL; }

	HxSTD_memset(pstTuningParam, 0, sizeof(DbSvc_TuningInfo_t));

	pstTuningParam->cab.ulFreq			= pstDeliDes->ulFreq / 10;						// KHz
	pstTuningParam->cab.usSR			= (HUINT16)(pstDeliDes->ulSymbolRate / 10); 	// KHz
	pstTuningParam->cab.ucCon			= svc_si_LibConvertCabDeliDesToPalQam (pstDeliDes->ucModul);
	pstTuningParam->cab.ucSpec			= eDxCAB_SPECTRUM_AUTO;

	return ERR_OK;
}

HERROR svc_si_LibGetTuningInfoFromTerDsd (SIxTerDeliSystemDes_t *pstDeliDes, DbSvc_TuningInfo_t *pstTuningParam)
{
	if (pstDeliDes == NULL) 				{ return ERR_FAIL; }
	if (pstTuningParam == NULL) 			{ return ERR_FAIL; }

	HxSTD_memset(pstTuningParam, 0, sizeof(DbSvc_TuningInfo_t));

	// Hierarchy information이 0 이외에 priority가 0이면 LP, 그 외의 경우에는 HP
	if (pstDeliDes->ucPriority == 0 && pstDeliDes->ucHierInfo != 0)
	{
		pstTuningParam->ter.ulFreq			= pstDeliDes->ulFreq / 100;
		pstTuningParam->ter.ucStream		= eDxTER_STREAM_LP;			// LP
		pstTuningParam->ter.ucCodeRate		= svc_si_LibConvertTerDeliDesToPalCodeRate ((HUINT32)pstDeliDes->ucCodeRateLPStream);
		pstTuningParam->ter.ucTransMode 	= svc_si_LibConvertTerDeliDesToPalTransMode ((HUINT32)pstDeliDes->ucTransmissionMode);
		pstTuningParam->ter.ucGuardInterval = svc_si_LibConvertTerDeliDesToPalGuardInterval ((HUINT32)pstDeliDes->ucGuardInterval);
		pstTuningParam->ter.ucConstellation = svc_si_LibConvertTerDeliDesToPalConstellation ((HUINT32)pstDeliDes->ucConstellation);
		pstTuningParam->ter.ucHierachy		= svc_si_LibConvertTerDeliDesToPalHierarchy ((HUINT32)pstDeliDes->ucHierInfo);
		pstTuningParam->ter.ucOffset		= 0;
		pstTuningParam->ter.ucBandwidth 	= svc_si_LibConvertTerDeliDesToPalBandWidth ((HUINT32)pstDeliDes->ucBandWidth);
	}
	else
	{
		pstTuningParam->ter.ulFreq			= pstDeliDes->ulFreq / 100;
		pstTuningParam->ter.ucStream		= eDxTER_STREAM_HP;			// HP
		pstTuningParam->ter.ucCodeRate		= svc_si_LibConvertTerDeliDesToPalCodeRate ((HUINT32)pstDeliDes->ucCodeRateHPStream);
		pstTuningParam->ter.ucTransMode 	= svc_si_LibConvertTerDeliDesToPalTransMode ((HUINT32)pstDeliDes->ucTransmissionMode);
		pstTuningParam->ter.ucGuardInterval = svc_si_LibConvertTerDeliDesToPalGuardInterval ((HUINT32)pstDeliDes->ucGuardInterval);
		pstTuningParam->ter.ucConstellation = svc_si_LibConvertTerDeliDesToPalConstellation ((HUINT32)pstDeliDes->ucConstellation);
		pstTuningParam->ter.ucHierachy		= svc_si_LibConvertTerDeliDesToPalHierarchy ((HUINT32)pstDeliDes->ucHierInfo);
		pstTuningParam->ter.ucOffset		= 0;
		pstTuningParam->ter.ucBandwidth 	= svc_si_LibConvertTerDeliDesToPalBandWidth ((HUINT32)pstDeliDes->ucBandWidth);
	}

	return ERR_OK;
}

HERROR svc_si_LibGetDeliveryTsInfoFromDsd (HUINT32 ulActionId, HUINT8 *pucDesRaw, DbSvc_TsInfo_t *pstTsInfo)
{
	HERROR		hErr;

	if ((NULL == pucDesRaw) || (NULL == pstTsInfo))
	{
		HxLOG_Error ("Invalid paremeters \n");
		return ERR_FAIL;
	}

	switch (*pucDesRaw)
	{
#if defined(CONFIG_MW_CH_SATELLITE)
	case eSIxDESCTAG_SATELLITE_DELIVERY_SYSTEM:
		{
			SIxSatDeliSystemDes_t	*pstSatDes = (SIxSatDeliSystemDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucDesRaw);

			if (NULL != pstSatDes)
			{
				hErr = svc_si_LibGetTuningInfoFromSatDsd (pstSatDes, &(pstTsInfo->stTuningParam));
				if (ERR_OK == hErr)
				{
					pstTsInfo->eDeliType = eDxDELIVERY_TYPE_SAT;
				}

				PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstSatDes);
				return ERR_OK;
			}
		}
		break;
#endif

#if defined(CONFIG_MW_CH_TERRESTRIAL)
	case eSIxDESCTAG_TERRESTRIAL_DELIVERY_SYSTEM:
		{
			SIxTerDeliSystemDes_t	*pstTerDes = (SIxTerDeliSystemDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucDesRaw);

			if (NULL != pstTerDes)
			{
				hErr = svc_si_LibGetTuningInfoFromTerDsd(pstTerDes, &(pstTsInfo->stTuningParam));
				if (ERR_OK == hErr)
				{
					pstTsInfo->eDeliType = eDxDELIVERY_TYPE_TER;
				}

				PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstTerDes);
				return ERR_OK;
			}
		}
		break;
#endif

#if defined(CONFIG_MW_CH_CABLE)
	case eSIxDESCTAG_CABLE_DELIVERY_SYSTEM:
		{
			SIxCabDeliSystemDes_t	*pstCabDes = (SIxCabDeliSystemDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucDesRaw);

			if (NULL != pstCabDes)
			{
				hErr = svc_si_LibGetTuningInfoFromCabDsd (pstCabDes, &(pstTsInfo->stTuningParam));
				if (ERR_OK == hErr)
				{
					pstTsInfo->eDeliType = eDxDELIVERY_TYPE_CAB;
				}

				PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstCabDes);
				return ERR_OK;
			}
		}
		break;
#endif

	default:
		break;
	}

	return ERR_FAIL;
}

/* end of file */

