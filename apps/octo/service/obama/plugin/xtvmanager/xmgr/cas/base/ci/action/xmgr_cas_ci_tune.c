/**
	@file     xmgr_cas_tunemgr.c
	@brief    xmgr_cas_tunemgr.c

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <octo_common.h>

#include <svc_pipe.h>
#include <svc_cas.h>
#include <svc_av.h>

#include <bus.h>
#include <svc_pipe.h>
#include <mgr_rscmap.h>
#include <otl.h>
#include <db_svc.h>

//	TODO: OCTO 2.0 구조에 맞게 header 추가 및 ap_cas_tunemgr.c 파일 자체 변경 필요.
#ifdef CONFIG_MW_CI
#include <ci.h>
#ifdef CONFIG_MW_CI_PLUS
#include <ci_plus.h>
#include <ci_ctrl.h>
#endif
#endif

#include <mgr_common.h>
#include <mgr_action.h>


#include "../local_include/_xmgr_cas_ci.h"
#include <mgr_cas.h>

#include <mgr_live.h>
#include <mgr_search.h>
#include <mgr_antenna.h>


/*******************************************************************/
/********************      typedef			    ********************/
/*******************************************************************/

#define CITUNE_TIMER_LOCKING_ID					0x2003
#define CITUNE_TIMER_LOCKING_TIME				(50*1000)	/* 50 sec */

#define CITUNE_TIMER_SI_ID						0x2004
#define CITUNE_TIMER_SI_TIME					(10*1000)	/* 10 sec */

#if defined(CONFIG_DEBUG)		// 디버깅용으로 테스트시에만 필요하다.
#define	HC_TUNE_026_C	0
#define	HC_TUNE_029_C	0

#define	HC_TUNE_030_C	0
#define	HC_TUNE_033_C	0

#define	HC_TUNE_214_C	0
#endif


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

typedef struct
{
	HUINT32				ulActionId;
	Handle_t 			hPreSvc;
	Handle_t			hRequestedSvc;
	ChListUiGroup_t		eSvcGroup;	// eChListUiGroup_CURRENT 사용하면 이건 불필요 ?
	SvcCas_TuneMode_e	eTuneMode;
	HUINT32 			ulDevSlotId;
} XmgrCas_CiTuneStatus_t;

typedef struct
{
	HUINT32				ulActionId;
	Handle_t			hAction;
	HUINT32				ulTunerId;
	DxDeliveryType_e	eDeliType;
	HBOOL				bNewTs;
	HBOOL				bPmtFlag;
	HUINT16 			usPCRPid;
	HUINT16 			usVideoPid;
	HUINT16 			usAudioPid;
	HUINT16				usOnId;
	HUINT16				usTsId;

	// === Tuning Informations ===
	DxTuneParam_t		stTuningInfo;

	HUINT32				ulAntennaIndex;		// Antenna Index
	DbSvc_AntInfo_t		stAntennaInfo;

	Handle_t 			hPreSvc;
	Handle_t			hRequestedSvc;
	ChListUiGroup_t		eSvcGroup;	// eChListUiGroup_CURRENT 사용하면 이건 불필요 ?
	SvcCas_TuneMode_e	eTuneMode;
	HUINT32 			ulDevSlotId;
	HUINT16				usServiceId;
} XmgrCas_CiTuneContext_t;

STATIC XmgrCas_CiTuneContext_t	s_stCasTuneContext[XMGR_CAS_CI_SLOT_MAX];
STATIC XmgrCas_CiTuneStatus_t 	s_stCasTuneStatus[2] = {{0, HANDLE_NULL, HANDLE_NULL, 0}, {0, HANDLE_NULL, HANDLE_NULL, 0}}; // for two ci slot model...
STATIC HUINT32 					s_ulDevSlotId;
static DxDeliveryType_e			s_eDeliType = eDxDELIVERY_TYPE_ALL;



/*******************************************************************/
/********************      Local Functions      ********************/
/*******************************************************************/
#define ____LOCAL_FUNCTION_____
INLINE XmgrCas_CiTuneContext_t *xmgr_cas_CiTuneGetContext(HUINT8 ucSlotNum)
{
	return &(s_stCasTuneContext[ucSlotNum]);
}

static INLINE HERROR xmgr_cas_CiTuneCheckActionId (XmgrCas_CiTuneContext_t *pstContents, Handle_t hAction)
{
	HUINT32			 ulActionId;

	ulActionId = SVC_PIPE_GetActionId(hAction);

	if (pstContents->ulActionId != ulActionId)
	{
		HxLOG_Info("is not matched actionID(0x%x)! line(%d)\n", ulActionId, __LINE__);
		return MESSAGE_PASS;
	}

	return ERR_BUS_NO_ERROR;
}

static void xmgr_cas_CiTuneIncreaseSessionVersion (XmgrCas_CiTuneContext_t *pstContents)
{
	pstContents->hAction = SVC_PIPE_IncreaseActionVersion(pstContents->ulActionId);
}


static HERROR xmgr_cas_CiTuneInitContents (XmgrCas_CiTuneContext_t *pstContents, Handle_t hAction)
{
	HxSTD_memset((void *)pstContents, 0x00, sizeof(XmgrCas_CiTuneContext_t));

	pstContents->hAction	= hAction;
	pstContents->ulActionId = SVC_PIPE_GetActionId(hAction);

	pstContents->usPCRPid = PID_NULL;
	pstContents->usVideoPid = PID_NULL;
	pstContents->usAudioPid = PID_NULL;

	pstContents->hPreSvc = HANDLE_NULL;
	pstContents->hRequestedSvc = HANDLE_NULL;
	pstContents->eSvcGroup = eChListUiGroup_CURRENT;

	return ERR_BUS_NO_ERROR;
}


static HERROR xmgr_cas_CiTuneSetSatCasCiTuneTPTsInfo (XmgrCas_CiTuneContext_t *pstContents, DbSvc_TuningInfo_t stTuningParam)
{
	DbSvc_SatTuningInfo_t 		stSatTuneInfo;
	DbSvc_TsInfo_t				stTsInfo;

	/* fix for prevent #150748 */
	HxSTD_memset(&stSatTuneInfo, 0x00, sizeof(DbSvc_SatTuningInfo_t));
	HxSTD_memset(&stTsInfo, 0x00, sizeof(DbSvc_TsInfo_t));

	stSatTuneInfo.ulFrequency	= stTuningParam.sat.ulFrequency;
	stSatTuneInfo.ulSymbolRate	= stTuningParam.sat.ulSymbolRate;
	stSatTuneInfo.eFecCodeRate	= stTuningParam.sat.eFecCodeRate;
	stSatTuneInfo.ePilot		= stTuningParam.sat.ePilot;
	stSatTuneInfo.ePolarization	= stTuningParam.sat.ePolarization;
	stSatTuneInfo.ePskMod		= stTuningParam.sat.ePskMod;
	stSatTuneInfo.eRollOff		= stTuningParam.sat.eRollOff;
	stSatTuneInfo.eTransSpec	= stTuningParam.sat.eTransSpec;

	stSatTuneInfo.eSatType		= stTuningParam.sat.eSatType;
	stSatTuneInfo.ucAntId		= stTuningParam.sat.ucAntId;

	stTsInfo.eDeliType = eDxDELIVERY_TYPE_SAT;
	stTsInfo.stTuningParam.sat = stSatTuneInfo;

	return ERR_BUS_NO_ERROR;
}

static HERROR xmgr_cas_CiTuneSetCabCasCiTuneTPTsInfo (XmgrCas_CiTuneContext_t *pstContents, DbSvc_TuningInfo_t stTuningParam)
{
	DbSvc_CabTuningInfo_t 		stCabTuneInfo;
	DbSvc_TsInfo_t				stTsInfo;

	HxSTD_memset(&stCabTuneInfo, 0x00, sizeof(DbSvc_CabTuningInfo_t));
	HxSTD_memset(&stTsInfo, 0x00, sizeof(DbSvc_TsInfo_t));

	stCabTuneInfo.ulFreq = stTuningParam.cab.ulFreq;
	stCabTuneInfo.usSR = stTuningParam.cab.usSR;
	stCabTuneInfo.usNetworkId = stTuningParam.cab.usNetworkId;
	stCabTuneInfo.ucCon = stTuningParam.cab.ucCon;
	stCabTuneInfo.ucSpec = stTuningParam.cab.ucSpec;

	stTsInfo.eDeliType = eDxDELIVERY_TYPE_CAB;
	stTsInfo.stTuningParam.cab	= stCabTuneInfo;

	return ERR_BUS_NO_ERROR;
}

static HERROR xmgr_cas_CiTuneSetTerCasCiTuneTPTsInfo (XmgrCas_CiTuneContext_t *pstContents, DbSvc_TuningInfo_t stTuningParam)
{
	DbSvc_TerTuningInfo_t 		stTerTuneInfo;
	DbSvc_TsInfo_t				stTsInfo;

	/* fix for prevent #150749 */
	HxSTD_memset(&stTerTuneInfo, 0x00, sizeof(DbSvc_TerTuningInfo_t));
	HxSTD_memset(&stTsInfo, 0x00, sizeof(DbSvc_TsInfo_t));

	stTerTuneInfo.ulFreq = stTuningParam.ter.ulFreq;
	stTerTuneInfo.ucCodeRate = stTuningParam.ter.ucCodeRate;
	stTerTuneInfo.ucConstellation = stTuningParam.ter.ucConstellation;
	stTerTuneInfo.ucHierachy = stTuningParam.ter.ucHierachy;
	stTerTuneInfo.ucOffset = stTuningParam.ter.ucOffset;
	stTerTuneInfo.ucStream = stTuningParam.ter.ucStream;
	stTerTuneInfo.ucTransMode = stTuningParam.ter.ucTransMode;
	stTerTuneInfo.ucGuardInterval = stTuningParam.ter.ucGuardInterval;
	stTerTuneInfo.ucBandwidth = stTuningParam.ter.ucBandwidth;

	stTerTuneInfo.ucChanNum = stTuningParam.ter.ucChanNum;
	stTerTuneInfo.ucLevel = stTuningParam.ter.ucLevel;
	stTerTuneInfo.ucQuality = stTuningParam.ter.ucQuality;

	stTsInfo.eDeliType = eDxDELIVERY_TYPE_TER;
	stTsInfo.stTuningParam.ter	= stTerTuneInfo;

	return ERR_BUS_NO_ERROR;
}

HERROR xmgr_cas_CiTuneConvertChTuningParamToSvcTuningInfo (DxTuneParam_t *pstChTuneInfo, DbSvc_TuningInfo_t *pstSvcTuneInfo)
{
	if (pstChTuneInfo == NULL)
	{
		return ERR_BUS_SOURCE_NULL;
	}

	if (pstSvcTuneInfo == NULL)
	{
		return ERR_BUS_TARGET_NULL;
	}

	HxSTD_memset(pstSvcTuneInfo, 0, sizeof(DbSvc_TuningInfo_t));

	switch (pstChTuneInfo->eDeliType)
	{
		case  eDxDELIVERY_TYPE_SAT:
			pstSvcTuneInfo->sat.ulFrequency		= pstChTuneInfo->sat.tuningInfo.ulFrequency;
			pstSvcTuneInfo->sat.ulSymbolRate	= pstChTuneInfo->sat.tuningInfo.ulSymbolRate;
			pstSvcTuneInfo->sat.ePolarization	= pstChTuneInfo->sat.tuningInfo.ePolarization;
			pstSvcTuneInfo->sat.eFecCodeRate	= pstChTuneInfo->sat.tuningInfo.eFecCodeRate;
			pstSvcTuneInfo->sat.eTransSpec		= pstChTuneInfo->sat.tuningInfo.eTransSpec;
			pstSvcTuneInfo->sat.ePskMod			= pstChTuneInfo->sat.tuningInfo.ePskMod;
			pstSvcTuneInfo->sat.ePilot			= pstChTuneInfo->sat.tuningInfo.ePilot;
			pstSvcTuneInfo->sat.eRollOff		= pstChTuneInfo->sat.tuningInfo.eRollOff;
			break;
		case eDxDELIVERY_TYPE_TER:
			pstSvcTuneInfo->ter.ulFreq			= (HUINT32)pstChTuneInfo->ter.ulFrequency;
			pstSvcTuneInfo->ter.ucBandwidth		= (HUINT8)pstChTuneInfo->ter.eBandWidth;
			pstSvcTuneInfo->ter.ucConstellation	= (HUINT8)pstChTuneInfo->ter.eConstellation;
			pstSvcTuneInfo->ter.ucHierachy		= (HUINT8)pstChTuneInfo->ter.eHierachy;
			pstSvcTuneInfo->ter.ucCodeRate		= (HUINT8)pstChTuneInfo->ter.eCodeRate;
			pstSvcTuneInfo->ter.ucTransMode		= (HUINT8)pstChTuneInfo->ter.eTransMode;
			pstSvcTuneInfo->ter.ucGuardInterval	= (HUINT8)pstChTuneInfo->ter.eGuardInterval;
			pstSvcTuneInfo->ter.ucStream			= (HUINT8)pstChTuneInfo->ter.eStream;
			pstSvcTuneInfo->ter.ucOffset			= (HUINT8)pstChTuneInfo->ter.eOffset;
#if defined(CONFIG_MW_CH_DVB_T2)
			pstSvcTuneInfo->ter.ucSystem					= (HUINT8)pstChTuneInfo->ter.eSystem;
			pstSvcTuneInfo->ter.stT2param.ulPlpId			= (HUINT8)pstChTuneInfo->ter.stT2Param.ulPlpId;
			pstSvcTuneInfo->ter.stT2param.ucPreambleFormat	= (HUINT8)pstChTuneInfo->ter.stT2Param.preambleFormat;
			pstSvcTuneInfo->ter.stT2param.ucMixed			= (HUINT8)pstChTuneInfo->ter.stT2Param.mixed;
			pstSvcTuneInfo->ter.stT2param.ucPilotPattern	= (HUINT8)pstChTuneInfo->ter.stT2Param.pilotPattern;
			pstSvcTuneInfo->ter.stT2param.ucExtenedCarrier	= (HUINT8)pstChTuneInfo->ter.stT2Param.extenedCarrier;
			pstSvcTuneInfo->ter.stT2param.ucPAPRreduction	= (HUINT8)pstChTuneInfo->ter.stT2Param.PAPRreduction;
			pstSvcTuneInfo->ter.stT2param.ulNumPlpBlocks	= (HUINT8)pstChTuneInfo->ter.stT2Param.numPlpBlocks;
#endif
			break;
		case eDxDELIVERY_TYPE_CAB:
			pstSvcTuneInfo->cab.ulFreq			= (HUINT32)pstChTuneInfo->cab.ulFrequency;
			pstSvcTuneInfo->cab.usSR			= (HUINT16)pstChTuneInfo->cab.ulSymbolRate;
			pstSvcTuneInfo->cab.ucCon			= (HUINT8)pstChTuneInfo->cab.constellation;
			pstSvcTuneInfo->cab.ucSpec			= (HUINT8)pstChTuneInfo->cab.spectrum;
			break;
		default:
			return ERR_FAIL;
			break;
	}

	return ERR_BUS_NO_ERROR;
}

static HERROR xmgr_cas_CiTuneFindSvcHandleByVideoPidAudioPid (DxDeliveryType_e eDeliType, DbSvc_TripleId_t triple_id, HUINT16 usVideoPid, HUINT16 usAudioPid, Handle_t *hSvc, HUINT16 *pusSvcId)
{
	HERROR			 	hErr = ERR_FAIL;

	HINT32				 nDbTsCnt, nDbTsNum;
	DbSvc_TsInfo_t		*pstTsInfoList = NULL;
	DbSvc_TsCondition_t	 stTsCond;

	DbSvc_Condition_t	 stSvcCond;
	HINT32				nDbSvcCnt, nSvcCount;
	DbSvc_Info_t		*astSvcInfoList = NULL;
	Handle_t			hTmpSvc = HANDLE_NULL;

	HxLOG_Info("%s +++\n", __FUNCTION__);

	*hSvc = HANDLE_NULL;
	*pusSvcId = 0;

	DB_SVC_InitTsFindCondition (&stTsCond);
	stTsCond.nDeliType	= eDeliType;
	stTsCond.nOnId	= (HINT32)triple_id.usOnId;
	stTsCond.nTsId	= (HINT32)triple_id.usTsId;

	HxLOG_Print ("\t\t finding usOnId(0x%x), usTsId(0x%x), eDeliType(0x%x), usVideoPid(0x%x), usAudioPid(0x%x)\n",
								triple_id.usOnId, triple_id.usTsId, eDeliType, usVideoPid, usAudioPid);

	hErr = DB_SVC_FindTsInfoList (&stTsCond, &nDbTsNum, &pstTsInfoList);
	if (hErr == ERR_OK && pstTsInfoList != NULL)
	{
		for (nDbTsCnt = 0; nDbTsCnt < nDbTsNum; nDbTsCnt++)
		{
			HxLOG_Print ("\t\t usTsIdx (0x%x)\n", pstTsInfoList[nDbTsCnt].usTsIdx);

			DB_SVC_InitSvcFindCondition (&stSvcCond);
			stSvcCond.nOrgDeliType 	= eDeliType;
			stSvcCond.nTsIdx 		= (HINT32)pstTsInfoList[nDbTsCnt].usTsIdx;

			hErr = DB_SVC_FindSvcInfoList (&stSvcCond, &nSvcCount, &astSvcInfoList);
			if (hErr == ERR_OK && astSvcInfoList != NULL)
			{
				HxLOG_Print ("\t\t nSvcCount (%d)\n", nSvcCount);

				for (nDbSvcCnt = 0; nDbSvcCnt < nSvcCount; nDbSvcCnt++)
				{
					HxLOG_Print ("\t\t astSvcInfoList[%d].usSvcIdx = 0x%x\n", nDbSvcCnt, DbSvc_GetSvcIdx(&astSvcInfoList[nDbSvcCnt]));
					hErr = DB_SVC_GetServiceHandleBySvcIdx(DbSvc_GetSvcIdx(&astSvcInfoList[nDbSvcCnt]), &hTmpSvc);
					if (hErr != ERR_OK)
					{
						HxLOG_Print ("\t\t OxDB_SVC_GetServiceHandleBySvcIdx() fail\n");
						continue;
					}
					HxLOG_Print ("\t\t\t\t usVideoPid = 0x%x, usAudioPid = 0x%x, eSvcType = 0x%x\n",
									DbSvc_GetVideoPid(&astSvcInfoList[nDbSvcCnt]), DbSvc_GetAudioPid(&astSvcInfoList[nDbSvcCnt])
									, DbSvc_GetSvcType(&astSvcInfoList[nDbSvcCnt]));

					if (usAudioPid == PID_NULL)
					{
						if (DbSvc_GetVideoPid(&astSvcInfoList[nDbSvcCnt]) == usVideoPid)
						{
							*hSvc = hTmpSvc;
							hErr = ERR_OK;

							if (DbSvc_GetName(&astSvcInfoList[nDbSvcCnt]) != NULL)
								HxLOG_Print ("\t\t -> found hSvc(0x%x), %d. [%s]\n", *hSvc, DbSvc_GetLcn(&astSvcInfoList[nDbSvcCnt]), DbSvc_GetName(&astSvcInfoList[nDbSvcCnt]));
							else
								HxLOG_Print ("\t\t -> found hSvc(0x%x), %d. [unknown]\n", *hSvc, DbSvc_GetLcn(&astSvcInfoList[nDbSvcCnt]));
							break;
						}
					}
					else
					{
						if (DbSvc_GetVideoPid(&astSvcInfoList[nDbSvcCnt]) == usVideoPid && DbSvc_GetAudioPid(&astSvcInfoList[nDbSvcCnt]) == usAudioPid)
						{
							*hSvc = hTmpSvc;
							*pusSvcId = DbSvc_GetSvcId(&astSvcInfoList[nDbSvcCnt]);
							hErr = ERR_OK;

							if (DbSvc_GetName(&astSvcInfoList[nDbSvcCnt]) != NULL)
								HxLOG_Print ("\t\t -> found hSvc(0x%x), %d. [%s]\n", *hSvc, DbSvc_GetLcn(&astSvcInfoList[nDbSvcCnt]), DbSvc_GetName(&astSvcInfoList[nDbSvcCnt]));
							else
								HxLOG_Print ("\t\t -> found hSvc(0x%x), %d. [unknown]\n", *hSvc, DbSvc_GetLcn(&astSvcInfoList[nDbSvcCnt]));
							break;
						}
					}
				}
				DB_SVC_FreeServiceInfoList (NULL, NULL, astSvcInfoList);
			}

		}

		DB_SVC_FreeTsInfoList (pstTsInfoList);
	}

	HxLOG_Info("%s ---\n", __FUNCTION__);

	return hErr;
}


// not used
static HERROR xmgr_cas_CiTuneFindSvcHandleByVideoPid (DxDeliveryType_e eDeliType, DbSvc_TripleId_t triple_id, HUINT16 usVideoPid, HUINT16 usAudioPid, Handle_t *hSvc)
{
	HERROR			 	hErr;

	HINT32				 nDbTsCnt, nDbTsNum;
	DbSvc_TsInfo_t		*pstTsInfoList = NULL;
	DbSvc_TsCondition_t	 stTsCond;

	DbSvc_Condition_t	 stSvcCond;
	HINT32				nDbSvcCnt, nSvcCount;
	DbSvc_Info_t		*astSvcInfoList = NULL;
	Handle_t			hTmpSvc = HANDLE_NULL;

	HxLOG_Info("%s +++\n", __FUNCTION__);

	*hSvc = HANDLE_NULL;

	DB_SVC_InitTsFindCondition (&stTsCond);
	stTsCond.nDeliType	= eDeliType;
	stTsCond.nOnId	= (HINT32)triple_id.usOnId;
	stTsCond.nTsId	= (HINT32)triple_id.usTsId;

	HxLOG_Print ("\t\t finding usOnId(0x%x), usTsId(0x%x), eDeliType(0x%x), usVideoPid(0x%x), usAudioPid(0x%x)\n",
								triple_id.usOnId, triple_id.usTsId, eDeliType, usVideoPid, usAudioPid);

	hErr = DB_SVC_FindTsInfoList (&stTsCond, &nDbTsNum, &pstTsInfoList);
	if (hErr == ERR_OK && pstTsInfoList != NULL)
	{
		for (nDbTsCnt = 0; nDbTsCnt < nDbTsNum; nDbTsCnt++)
		{
			HxLOG_Print ("\t\t usTsIdx (0x%x)\n", pstTsInfoList[nDbTsCnt].usTsIdx);

			DB_SVC_InitSvcFindCondition (&stSvcCond);
			stSvcCond.nOrgDeliType 	= eDeliType;
			stSvcCond.nTsIdx 		= (HINT32)pstTsInfoList[nDbTsCnt].usTsIdx;

			hErr = DB_SVC_FindSvcInfoList (&stSvcCond, &nSvcCount, &astSvcInfoList);
			if (hErr == ERR_OK && astSvcInfoList != NULL)
			{
				HxLOG_Print ("\t\t nSvcCount (%d)\n", nSvcCount);

				for (nDbSvcCnt = 0; nDbSvcCnt < nSvcCount; nDbSvcCnt++)
				{
					HxLOG_Print ("\t\t astSvcInfoList[%d].usSvcIdx = 0x%x\n", nDbSvcCnt, DbSvc_GetSvcIdx(&astSvcInfoList[nDbSvcCnt]));
					hErr = DB_SVC_GetServiceHandleBySvcIdx(DbSvc_GetSvcIdx(&astSvcInfoList[nDbSvcCnt]), &hTmpSvc);
					if (hErr != ERR_OK)
					{
						HxLOG_Print ("\t\t OxDB_SVC_GetServiceHandleBySvcIdx() fail\n");
						continue;
					}
					HxLOG_Print ("\t\t\t\t usVideoPid = 0x%x, usAudioPid = 0x%x, eSvcType = 0x%x\n",
									DbSvc_GetVideoPid(&astSvcInfoList[nDbSvcCnt]), DbSvc_GetAudioPid(&astSvcInfoList[nDbSvcCnt])
									, DbSvc_GetSvcType(&astSvcInfoList[nDbSvcCnt]));

					if (DbSvc_GetVideoPid(&astSvcInfoList[nDbSvcCnt]) == usVideoPid)
					{
						*hSvc = hTmpSvc;

						if (DbSvc_GetName(&astSvcInfoList[nDbSvcCnt]) != NULL)
							HxLOG_Print ("\t\t -> found hSvc(0x%x), %d. [%s]\n", *hSvc, DbSvc_GetLcn(&astSvcInfoList[nDbSvcCnt]), DbSvc_GetName(&astSvcInfoList[nDbSvcCnt]));
						else
							HxLOG_Print ("\t\t -> found hSvc(0x%x), %d. [unknown]\n", *hSvc, DbSvc_GetLcn(&astSvcInfoList[nDbSvcCnt]));
						break;
					}
				}

				DB_SVC_FreeServiceInfoList (NULL, NULL, astSvcInfoList);
			}

		}

		DB_SVC_FreeTsInfoList (pstTsInfoList);
	}

	HxLOG_Info("%s ---\n", __FUNCTION__);

	return ERR_OK;
}


static HERROR xmgr_cas_CiTuneSatRetrieveResults (XmgrCas_CiTuneContext_t* pstContents, Handle_t *phSvc, HUINT16 *usTsId, HUINT16 *usOnId)
{
	HUINT32				ulIndex, ulItemNum;
	HUINT32				ulSvcNum, ulSelectedCnt= 0;
	HINT32				nDbTsCnt, nDbTsNum;
	Handle_t			hSvc;
	HBOOL				bNewTs, bSameSvcId = FALSE;
	HUINT16				usNetIdx, usTsIdx;
	DbSvc_TsInfo_t		stTsInfo;
	DbSvc_TsInfo_t		*pstTsInfoList;
	DbSvc_NetInfo_t		stNetInfo;
	DbSvc_TsCondition_t	stTsCond;
	DbSvc_TsInfo_t		*pstTsInfo = NULL;
	DbSvc_NetInfo_t		*pstNetInfo = NULL;
	DbSvc_Info_t		*astSvcArray = NULL;
	DbSvc_TuningInfo_t	*pstCurrTuneInfo;
	DbSvc_TuningInfo_t	stSvcTuneInfo;
	DxTuneParam_t		stChTuneInfo;
	HERROR				hErr;
	HUINT32				ulInfoNum = 0;
	HUINT16 			usLcnLocal = 2000;

	HxLOG_Info("%s +++\n", __FUNCTION__);

	//------------------------------------------------------------------------------
	// 1. Network Info
	// 현재 Network는 단 하나일 수 밖에 없다.

	hErr = SVC_SI_CountSearchedResult(pstContents->ulActionId, eSI_SRCH_INFOTYPE_Network, &ulItemNum);
	if (hErr != ERR_OK)					{ ulItemNum = 0; }

	if (ulItemNum == 0)
	{
		HxLOG_Critical("\n\n");
		hErr = ERR_BUS_NO_OBJECT;
		goto ERROR_FAULT_OCCURRED;
	}

	pstNetInfo = (DbSvc_NetInfo_t *)OxAP_Malloc (sizeof(DbSvc_NetInfo_t));
	if (pstNetInfo == NULL)
	{
		HxLOG_Critical("\n\n");
		hErr = ERR_BUS_OUT_OF_MEMORY;
		goto ERROR_FAULT_OCCURRED;
	}

	hErr = SVC_SI_GetSearchedResults(pstContents->ulActionId, eSI_SRCH_INFOTYPE_Network, 1, &ulInfoNum, (void *)pstNetInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		hErr = ERR_BUS_NO_OBJECT;
		goto ERROR_FAULT_OCCURRED;
	}

	HxLOG_Print("[%s (%04d)] MW_SI_GetNumNetInfo() Network Name [%s]\n", __FILE__, __LINE__, pstNetInfo->szNetName);

	pstNetInfo->eOrgDeliType = eDxDELIVERY_TYPE_SAT;
	HxLOG_Print("\t= [NET] usNetId = 0x%x \n", pstNetInfo->usNetId);

	//------------------------------------------------------------------------------
	// 2. TS Info
	// TS Info를 얻어오는 것이 제일 중요하다.
	// 여기서 TS Info를 얻어오지 못하면 search fail로 간주한다.
	hErr = SVC_SI_CountSearchedResult(pstContents->ulActionId, eSI_SRCH_INFOTYPE_Transponder, &ulItemNum);
	if (hErr != ERR_OK)					{ ulItemNum = 0; }

	if (ulItemNum == 0)
	{
		HxLOG_Critical("\n\n");
		hErr = ERR_BUS_NO_OBJECT;
		goto ERROR_FAULT_OCCURRED;
	}
	HxLOG_Print ("\t= Ts ulItemNum = %d\n", ulItemNum);

	// 현재 TS는 단 하나일 수 밖에 없다.
	pstTsInfo = (DbSvc_TsInfo_t *)OxAP_Malloc (sizeof(DbSvc_TsInfo_t));
	if (pstTsInfo == NULL)
	{
		HxLOG_Critical("\n\n");
		hErr = ERR_BUS_OUT_OF_MEMORY;
		goto ERROR_FAULT_OCCURRED;
	}

	hErr = SVC_SI_GetSearchedResults(pstContents->ulActionId, eSI_SRCH_INFOTYPE_Transponder, 1, &ulInfoNum, (void *)pstTsInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		hErr = ERR_BUS_NO_OBJECT;
		goto ERROR_FAULT_OCCURRED;
	}
	HxLOG_Print ("\t= usOnId(0x%x), usTsId(0x%x)\n", pstTsInfo->usOnId, pstTsInfo->usTsId);
	*usTsId = pstTsInfo->usTsId;
	*usOnId = pstTsInfo->usOnId;

	// SI에서 올려준 TS Info에 실제 물리적 Tuning Info 등을 추가하여 DB에 저장할 수 있는 data로 변환한다.
	hErr = SVC_CH_GetLockedInfo (pstContents->ulActionId, &stChTuneInfo);
	if (hErr == ERR_OK)
	{
		xmgr_cas_CiTuneConvertChTuningParamToSvcTuningInfo(&stChTuneInfo, &stSvcTuneInfo);
		pstCurrTuneInfo = &stSvcTuneInfo;
	}
	else
	{
		HxLOG_Error ("SVC_CH_GetLockedInfo() fail\n");
	}

	if (pstCurrTuneInfo == NULL)
	{
		HxLOG_Critical("\n\n");
		hErr = ERR_BUS_NO_OBJECT;
		goto ERROR_FAULT_OCCURRED;
	}

	// Current TP Info
	HxSTD_memcpy (&(pstTsInfo->stTuningParam), pstCurrTuneInfo, sizeof(DbSvc_TuningInfo_t));

	// Tuner ID
	pstTsInfo->ucTunerId = (HUINT8)pstContents->ulTunerId;
	// Antenna Idx
	pstTsInfo->stTuningParam.sat.ucAntId = (HUINT8)(pstContents->ulAntennaIndex);
	// Sat Type
	pstTsInfo->stTuningParam.sat.eSatType = pstContents->stAntennaInfo.eSatType;


	//------------------------------------------------------------------------------
	// 3. Bouquet Info

	//------------------------------------------------------------------------------
	// 4. Provider Info

	//------------------------------------------------------------------------------
	// 5. Service Info
	hErr = SVC_SI_CountSearchedResult(pstContents->ulActionId, eSI_SRCH_INFOTYPE_Service, &ulSvcNum);
	if (hErr != ERR_OK)					{ ulSvcNum = 0; }

	if (ulSvcNum == 0)
	{
		HxLOG_Critical("\n\n");
		hErr = ERR_BUS_NO_OBJECT;
		goto ERROR_FAULT_OCCURRED;
	}
	HxLOG_Print ("\t= ulSvcNum = %d\n", ulSvcNum);

	astSvcArray = (DbSvc_Info_t *)OxAP_Malloc (sizeof(DbSvc_Info_t) * ulSvcNum);
	if (astSvcArray == NULL)
	{
		HxLOG_Critical("\n\n");
		hErr = ERR_BUS_OUT_OF_MEMORY;
		goto ERROR_FAULT_OCCURRED;
	}

	hErr = SVC_SI_GetSearchedResults(pstContents->ulActionId, eSI_SRCH_INFOTYPE_Service, ulSvcNum, &ulInfoNum, (void *)astSvcArray);
	if (astSvcArray == NULL)
	{
		HxLOG_Critical("\n\n");
		hErr = ERR_BUS_NO_OBJECT;
		goto ERROR_FAULT_OCCURRED;
	}

	//------------------------------------------------------------------------------
	// 6. Store the searched results to the contents :
	// 6-1. Network Info

	//------------------------------------------------------------------------------
	// 1. adding/updating a network
	//------------------------------------------------------------------------------
	if (pstNetInfo != NULL)
	{
		pstNetInfo->eOrgDeliType = eDxDELIVERY_TYPE_SAT;
		// Network Info가 이미 등록되어 있는 것인지 체크한다.
		hErr =	DB_SVC_FindNetworkIndexById(pstNetInfo->eOrgDeliType, pstNetInfo->usNetId, &usNetIdx);
		if (hErr == ERR_OK)
		{
			if (pstNetInfo->szNetName[0] != '\0')
			{
				DB_SVC_GetNetworkInfo (usNetIdx, &stNetInfo);
				if (MWC_UTIL_DvbStrcmp (pstNetInfo->szNetName, stNetInfo.szNetName) != 0)
				{
					// Network Name이 달라졌으면 Update 한다.
					DB_SVC_UpdateNetworkInfo (usNetIdx, pstNetInfo);
					HxLOG_Print("\t= [NET] netIdx = %d --> update\n", usNetIdx);
				}
			}
		}
		else
		{
			// 등록되어 있지 않으므로 새로 추가한다.
			DB_SVC_AddNetworkInfo (pstNetInfo, &usNetIdx);
			HxLOG_Print("\t= [NET] netIdx = %d --> add\n", usNetIdx);
		}
	}

	// 6-2. Ts Info
	// Network Info를 넣을 때 나온 index 값을 temporary index로써 TsInfo에 넣는다.
	pstTsInfo->usNetIdx = (HUINT16)usNetIdx;
	pstTsInfo->eDeliType = eDxDELIVERY_TYPE_SAT;

	bNewTs = TRUE;
	//------------------------------------------------------------------------------
	// 2. adding/updating a ts
	//------------------------------------------------------------------------------
	if (pstTsInfo != NULL)
	{
		pstTsInfo->usNetIdx = usNetIdx;
		pstTsInfo->eDeliType = eDxDELIVERY_TYPE_SAT;

		// TS Info가 OnId, TsId로 등록되어 있는 지 확인한다.
		DB_SVC_InitTsFindCondition (&stTsCond);
		stTsCond.nDeliType	= eDxDELIVERY_TYPE_SAT;
		stTsCond.nOnId		= (HINT32)pstTsInfo->usOnId;
		stTsCond.nTsId		= (HINT32)pstTsInfo->usTsId;

		pstTsInfoList = NULL;

		hErr = DB_SVC_FindTsInfoList (&stTsCond, &nDbTsNum, &pstTsInfoList);

		if (hErr == ERR_OK && pstTsInfoList != NULL)
		{
			for (nDbTsCnt = 0; nDbTsCnt < nDbTsNum; nDbTsCnt++)
			{
				if (pstTsInfoList[nDbTsCnt].stTuningParam.sat.ucAntId == pstContents->ulAntennaIndex)
				// Found the same TsInfo including the antenna index
				{
					bNewTs = FALSE;
					stTsInfo = pstTsInfoList[nDbTsCnt];
					usTsIdx = stTsInfo.usUniqueId;

					if (DB_SVC_UtilCompareDbTuningParam (eDxDELIVERY_TYPE_SAT, &(pstTsInfo->stTuningParam), &(stTsInfo.stTuningParam)) != 0)
					{
						DB_SVC_UpdateTsInfo (usTsIdx, pstTsInfo);
						HxLOG_Print ("\t\t= [TS] tsIdx = %d --> update\n", usTsIdx);
					}

					break;
				}
			}

			DB_SVC_FreeTsInfoList (pstTsInfoList);
		}

		if (bNewTs == TRUE)
		{
			DB_SVC_AddTsInfo (pstTsInfo, &usTsIdx);
			HxLOG_Print ("\t\t= [TS] tsIdx = %d(0x%x) --> add\n", usTsIdx, usTsIdx);
		}
	}

	// 6-4. Provider Info

	// 6-5. Service Info
	for (ulIndex = 0; ulIndex < ulSvcNum; ulIndex++)
	{
		DbSvc_SetLcn(&astSvcArray[ulIndex], usLcnLocal++);
		DbSvc_SetDeliType(astSvcArray[ulIndex], eDxDELIVERY_TYPE_SAT);
		DbSvc_SetSatType(&astSvcArray[ulIndex], pstContents->stAntennaInfo.eSatType);
		DbSvc_SetAntIdx(&astSvcArray[ulIndex], pstTsInfo->stTuningParam.sat.ucAntId);
		DbSvc_SetOnId(&astSvcArray[ulIndex], pstTsInfo->usOnId);
		DbSvc_SetTsId(&astSvcArray[ulIndex], pstTsInfo->usTsId);
		DbSvc_SetTsIdx(&astSvcArray[ulIndex], usTsIdx);
		DbSvc_SetPrvIdx(&astSvcArray[ulIndex], 0); // TODO:
		DbSvc_SetSvcType(&astSvcArray[ulIndex], eDxSVC_TYPE_HIDDEN);

		if (pstContents->usServiceId == 0)// wildcard SID
		{
			HxLOG_Print ("\t\t= wildcard SID (SvcId=0x00) --> add the first service for tuning...\n");
			ulSelectedCnt = 0;
		}
		else
		{
			HCHAR	*pszSvcName = NULL;

			HxLOG_Print("\t\t= searched usSvcId(0x%x),   SvcId(0x%x)\n", DbSvc_GetSvcId(&astSvcArray[ulIndex]), pstContents->usServiceId);
			if (DbSvc_GetSvcId(&astSvcArray[ulIndex]) == pstContents->usServiceId)
			{
				ulSelectedCnt = ulIndex;
				bSameSvcId = TRUE;
				HxLOG_Print("\t\t= the same usSvcId(0x%x) \n\n", DbSvc_GetSvcId(&astSvcArray[ulIndex]));
			}
			else
			{
				if ( (bSameSvcId == FALSE) && (ulIndex+1) == ulSvcNum)
				{
					// si info 가 없는 service가 존재한다는 것이다.
					ulSelectedCnt = ulIndex;
					bSameSvcId = TRUE;

					DbSvc_SetSvcId(&astSvcArray[ulIndex], pstContents->usServiceId);
					HxLOG_Print("\t\t= new usSvcId(0x%x) without SI info \n\n", DbSvc_GetSvcId(&astSvcArray[ulIndex]));
				}
			}

			DbSvc_SetName(&astSvcArray[ulIndex], "CAM Tuning Service");
			pszSvcName = DbSvc_GetName(&astSvcArray[ulIndex]);
			pszSvcName[DbSvc_GetNameLength()-1] = '\0';
		}
		HxLOG_Print("\t\t= usSvcId(0x%x), szSvcName(%s)\n\n", DbSvc_GetSvcId(&astSvcArray[ulIndex]), DbSvc_GetName(&astSvcArray[ulIndex]));
	}

	for (ulIndex = 0; ulIndex < ulSvcNum; ulIndex++)
	{
		if  (bNewTs == TRUE)
		{
			HxLOG_Print (">>> ADD    : \t%d. (SvcId=0x%x) \t%s\n", DbSvc_GetLcn(&astSvcArray[ulIndex]), DbSvc_GetSvcId(&astSvcArray[ulIndex]), DbSvc_GetName(&astSvcArray[ulIndex]));
			DB_SVC_AddServiceInfo (&astSvcArray[ulIndex], &hSvc);
			HxLOG_Print("\t\t= added hSvc(0x%x)\n", hSvc);
			if (ulSelectedCnt == ulIndex)
			{
				*phSvc = hSvc;
				HxLOG_Print("\t\t= returned *phSvc(0x%x)\n", *phSvc);
			}
		}
	}

	hErr = ERR_BUS_NO_ERROR;

ERROR_FAULT_OCCURRED:
	if (pstTsInfo != NULL)
	{
		OxAP_Free (pstTsInfo);
	}

	if (pstNetInfo != NULL)
	{
		OxAP_Free (pstNetInfo);
	}

	if (astSvcArray != NULL)
	{
		OxAP_Free (astSvcArray);
	}

	return hErr;
}

static HERROR xmgr_cas_CiTuneCabRetrieveResults (XmgrCas_CiTuneContext_t* pstContents, Handle_t *hSvc)
{
	HxLOG_Info("%s \n", __FUNCTION__);
	//TODO
	HxLOG_Print("YOU MUST IMPLEMENT THIS FUNCTION !!!!!\n\n\n");
	return ERR_OK;
}

static HERROR xmgr_cas_CiTuneTerRetrieveResults (XmgrCas_CiTuneContext_t* pstContents, Handle_t *hSvc)
{
	HxLOG_Info("%s \n", __FUNCTION__);
	//TODO
	HxLOG_Print("YOU MUST IMPLEMENT THIS FUNCTION !!!!!\n\n\n");
	return ERR_OK;
}

static void xmgr_cas_CiTuneReleaseAction (XmgrCas_CiTuneContext_t *pstContents)
{
	HERROR			 hErr;

	HxLOG_Info("%s \n", __FUNCTION__);

	hErr = SVC_CH_StopTune (pstContents->hAction, (HUINT16)-1, eSyncMode);
	if (hErr != ERR_OK)
	{
		HxLOG_Error ("Error Log !!\n");
	}

	// Release the action :
	MGR_ACTION_Release (pstContents->ulActionId, eActionType_SEARCH);
}


/*******************************************************************/
/********************      API Functions   *************************/
/*******************************************************************/
#define ____API_FUNCTION_____

STATIC HERROR xmgr_cas_CiTuneByTripleId_v1(XmgrCas_CiTuneContext_t *pstContents, Handle_t hAction, HUINT32 ulDevSlotId, SvcCas_TuneInfo_t* pstCasTuneInfo)
{
	HERROR				nResult = ERR_FAIL;
	HUINT32 			ulActionId = 0;
	DbSvc_TripleId_t	triple_id;
	HUINT32 			idx = 0;
	DxDeliveryType_e	eDeliType = 0;
	Handle_t			hCurSvc = HANDLE_NULL, hRequestedSvc = HANDLE_NULL;
	ChListUiGroup_t 	eSvcGroup = 0; // eChListUiGroup_CURRENT 사용하면 이건 불필요 ?
	HBOOL				bSvcFound = FALSE;
	HUINT16 			usSvcId_Db = 0;

	HxLOG_Info("%s \n", __FUNCTION__);

	if (hAction == HANDLE_NULL)
	{
		/*
			FTA에서 요청된 경우... 이 경우 처리 필요...
			Main Action Id는 Zapper 인 경우에만 사용 가능하다.
		*/
		ulActionId = MGR_ACTION_GetMainActionId();
		pstContents->ulActionId = ulActionId;
		HxLOG_Print("hAction == HANDLE_NULL & ulActionId = %d\n", ulActionId);
	}
	else
	{
		ulActionId = SVC_PIPE_GetActionId(hAction);
		pstContents->ulActionId = ulActionId;
		HxLOG_Print("ulActionId = %d\n", ulActionId);
	}

	if (MGR_LIVE_GetServiceHandle(ulActionId, eLiveViewPlaySvc_ACTUAL, &hCurSvc) != ERR_OK)
	{
		HxLOG_Print("[xmgr_cas_CiTuneByTripleId_v1] OxMGR_WTV_GetSvcHandle() error !!\n");
	}
	if (hCurSvc == HANDLE_NULL)
	{
		HxLOG_Print("[xmgr_cas_CiTuneByTripleId_v1] hCurSvc is HANDLE_NULL error !!\n"); // 설마 이런 일이...
	}

	eSvcGroup = eChListUiGroup_All;

	triple_id.usOnId = pstCasTuneInfo->utInfo.stTripleId.usOnId;
	triple_id.usTsId = pstCasTuneInfo->utInfo.stTripleId.usTsId;
	triple_id.usSvcId = pstCasTuneInfo->utInfo.stTripleId.usSvcId;

	HxLOG_Print("[xmgr_cas_CiTuneByTripleId_v1] The requested service : OnId 0x%x, TsId 0x%x, SvcId 0x%x\n", triple_id.usOnId, triple_id.usTsId, triple_id.usSvcId);

	for (idx = 0; idx < 3; idx++)
	{
		if (idx == 0)
		{
			pstContents->eDeliType = eDeliType = eDxDELIVERY_TYPE_SAT;
		}
		else if (idx == 1)
		{
			pstContents->eDeliType = eDeliType = eDxDELIVERY_TYPE_TER;
		}
		else if (idx == 2)
		{
			pstContents->eDeliType = eDeliType = eDxDELIVERY_TYPE_CAB;
		}

		nResult = DB_SVC_FindServiceHandleByTripleId(eDeliType, triple_id, &hRequestedSvc);
		if(ERR_OK == nResult)
		{
			/* fix for prevent #150645 */
			nResult = DB_SVC_GetServiceIdFromSvcHandle(hRequestedSvc, &usSvcId_Db);

			HxLOG_Print("hRequestedSvc (0x%x), pstCasTuneInfo->usSvcId (%x) DB_SVC_GetServiceIdFromSvcHandle(hRequestedSvc) (%x) \n",
				hRequestedSvc, pstCasTuneInfo->utInfo.stTripleId.usSvcId, usSvcId_Db);

			if (nResult == ERR_OK && pstCasTuneInfo->utInfo.stTripleId.usSvcId == usSvcId_Db)
			{
				HxLOG_Print("[xmgr_cas_CiTuneByTripleId_v1] The requested service is found by DB_SVC_FindServiceHandleByTripleId()\n");
				bSvcFound = TRUE;
				break;
			}
		}
	}

	if (hRequestedSvc == HANDLE_NULL || bSvcFound == FALSE)
	{
		/*
			Request triple id로 찾은 service 가 없다.
			-> 데이터 서비스라면 당연히 없을 수 있을 것.
			-> 이 경우엔 SvcId 를 무시하고 OnId, TsId 만 가지고서 해당 TP의 첫번째 서비스를 찾도록 한다.
		*/
		DbSvc_Condition_t	stFindCondition;
		HINT32 lSvcCount = 0;
		Handle_t *phHomeHandleList = NULL;

		DB_SVC_InitSvcFindCondition (&stFindCondition);
		stFindCondition.nOrgDeliType = eDxDELIVERY_TYPE_SAT;
		stFindCondition.nOnId = triple_id.usOnId;
		stFindCondition.nTsId = triple_id.usTsId;

		nResult = DB_SVC_FindSvcHandleList(&stFindCondition, &lSvcCount, &phHomeHandleList);
		if (nResult != ERR_OK || phHomeHandleList == NULL)
		{
			HxLOG_Error("[xmgr_cas_CiTuneByTripleId_v1] The requested service is not found !!\n");
			hRequestedSvc = HANDLE_NULL;
		}
		else
		{
			HxLOG_Print("[xmgr_cas_CiTuneByTripleId_v1] The requested service is found by DB_SVC_FindSvcHandleList()\n");
			hRequestedSvc = phHomeHandleList[0];
		}

		if (phHomeHandleList)
		{
			OxDB_Free(phHomeHandleList);
		}
	}

	HxSTD_memset(&s_stCasTuneStatus[ulDevSlotId], 0x00, sizeof(XmgrCas_CiTuneStatus_t));
	s_stCasTuneStatus[ulDevSlotId].hPreSvc = HANDLE_NULL;

	if (hRequestedSvc == HANDLE_NULL)
	{
		HxLOG_Print("[xmgr_cas_CiTuneByTripleId_v1] Can't tuned to the requested service (not found) !!\n");
	}
	else if (hRequestedSvc == hCurSvc)
	{
		HxLOG_Print("[xmgr_cas_CiTuneByTripleId_v1] No need to tuned to the requested service (it is current service) !!\n");
	}
	else
	{
		// SAMA 대체.
#if 0//defined(CONFIG_MW_MM_PVR) /* <TODO_CI_PLUS> : CI+ 가 권한을 갖고 있을 때만 이 proc 이 수행되도록 해야 하고 이런 시나리오에 맞춰 작업이 필요함 */
		MgrAction_ConflictInfo_t 	stConflict;
		HUINT32 err;

		HxSTD_memset(&stConflict, 0, sizeof(MgrAction_ConflictInfo_t));
		err = MGR_ACTION_CheckTuningConflict(hCurSvc, &stConflict);
		HxLOG_Print("[xmgr_cas_CiTuneByTripleId_v1][MSG_GWM_CREATE] err=%x, hSvc=%x, stConflict.eType=%d\n", err, hCurSvc, stConflict.eType);
		if(err == ERR_OK && stConflict.eType == eAM_Conflict_Nothing)
		{
		}
		else
		{
			// Create Message "레코딩으로 인해 채널 전환할 수 없다"는 스트링 출력
			// -> 캠에서 요청이 오면 반드시 채널 전환해야 함 (CI+ CAM F/W upgrade 케이스). 따라서 이 경우 레코딩을 중단해야 합니다.
			// break;
			return ERR_FAIL;
		}
#endif

		s_stCasTuneStatus[ulDevSlotId].ulActionId = ulActionId;
		s_stCasTuneStatus[ulDevSlotId].hPreSvc = hCurSvc;
		s_stCasTuneStatus[ulDevSlotId].hRequestedSvc = hRequestedSvc;
		s_stCasTuneStatus[ulDevSlotId].eSvcGroup = eSvcGroup; // eChListUiGroup_CURRENT 사용하면 이건 불필요 ?
		s_stCasTuneStatus[ulDevSlotId].eTuneMode = eCasTuneMode_Start_v1_TuneByTripleId;
		s_stCasTuneStatus[ulDevSlotId].ulDevSlotId = ulDevSlotId;

		pstContents->hPreSvc = hCurSvc;
		pstContents->hRequestedSvc = hRequestedSvc;
		pstContents->eSvcGroup = eSvcGroup;

		HxLOG_Print("[xmgr_cas_CiTuneByTripleId_v1] Now will be tuned to the requested service...\n");

		// OCTO 2.0 구조 변경 전에는 인증시 branch 에서 아래와 같이 사용했는데, eNO_IPLATE|eFORCE_TUNE 인자 처리가 불명확 함. 정상 동작 하면 이 주석은 지울 것.
		// APC_ZAP_SendServiceStartByActionId(ulActionId, hRequestedSvc, eSvcGroup, eNO_IPLATE|eFORCE_TUNE);
		MGR_LIVE_CmdSendServiceStartByActionId(ulActionId, hRequestedSvc);
	}
	return ERR_OK;
}


#ifdef CONFIG_MW_CI_PLUS_HCI_V_2

STATIC HERROR	xmgr_cas_CiNotifyTuneStatus(HUINT16 usSlotId, HUINT32 ulTargetResource, HUINT8 ucTuneResult)
{
	HERROR									hErr;
	SvcCas_CtrlParam_t						stParam;
	CI_NotifyTuneStatusInParam_t			stInParam;

	stInParam.usSlotId			= usSlotId;
	stInParam.ulTargetResource 	= ulTargetResource;
	stInParam.ucTuneResult		= ucTuneResult;

	stParam.pvIn = (void *)&stInParam;
	stParam.ulControlType 		= eCACTRL_CI_NotifyTuneStatus;

	hErr = SVC_CAS_CtrlNoti(eDxCAS_GROUPID_CAM, eCACTRL_CI_NotifyTuneStatus, &stParam);
	if( hErr != ERR_OK)
	{
		HxLOG_Error("\nSVC_CAS_CtrlNoti() error!!\n");
	}

	return hErr;
}

static HERROR xmgr_cas_CiTuneNotifyHciPmtUse(Handle_t hAction, HUINT32 ulSlotNumber)
{
	HERROR							hErr;
	SvcCas_CtrlParam_t				stParam;
	CI_NotifyHciPmtUseInParam_t		stInParam;

	stInParam.hAction = (HUINT16)hAction;
	stInParam.usSlotId = (HUINT16)ulSlotNumber;

	stParam.pvIn					= &stInParam;
	stParam.ulControlType			= eCACTRL_CI_NotifyHciPmtUse;

	HxLOG_Print("[xmgr_cas_CiTuneNotifyHciPmtUse] >> SVC_CAS_CtrlSet() - eCACTRL_CI_NotifyHciPmtUse \n");
	hErr = SVC_CAS_CtrlSet(eDxCAS_GROUPID_CAM, eCACTRL_CI_NotifyHciPmtUse, &stParam);
	if( hErr != ERR_OK)
	{
		HxLOG_Error("\nSVC_CAS_CtrlSet() error!!\n");
	}

	return hErr;
}


STATIC HBOOL	xmgr_cas_CiCompareTunedSvc(Handle_t hAction, HUINT32 *unSlotNumber)
{
	HUINT32 			ulActionId = 0;
	Handle_t			hCurSvc = HANDLE_NULL;
	HUINT8	 			i = 0;

	HxLOG_Info("%s \n", __FUNCTION__);

	if (hAction == HANDLE_NULL)
	{
		/*
			FTA에서 요청된 경우... 이 경우 처리 필요...
			Main Action Id는 Zapper 인 경우에만 사용 가능하다.
		*/
		ulActionId = MGR_ACTION_GetMainActionId();
	}
	else
	{
		ulActionId = SVC_PIPE_GetActionId(hAction);
	}

	for(i=0; i<2; i++)
	{
		HxLOG_Print("[xmgr_cas_CiCompareTunedSvc] Action ID of %dth Slot [%d]!!\n", i, s_stCasTuneStatus[i].ulActionId);
		if (s_stCasTuneStatus[i].ulActionId == ulActionId)
		{
			HxLOG_Print("[xmgr_cas_CiCompareTunedSvc] Find same Action ID [%d]!!\n", ulActionId);

			MGR_LIVE_GetServiceHandle(ulActionId, eLiveViewPlaySvc_ACTUAL, &hCurSvc);
			HxLOG_Print("[xmgr_cas_CiCompareTunedSvc] hCurSvc[%08x], Previously requested Svc[%08x]\n", hCurSvc, s_stCasTuneStatus[i].hRequestedSvc);
			if(hCurSvc == s_stCasTuneStatus[i].hRequestedSvc)
			{
				HxLOG_Print("[xmgr_cas_CiCompareTunedSvc] Tuned Service OK!!\n");
				*unSlotNumber = i;
				return TRUE;
			}
		}
	}

	return FALSE;
}

STATIC HERROR xmgr_cas_CiTuneByTripleId_v2(XmgrCas_CiTuneContext_t *pstContents, Handle_t hAction, HUINT32 ulDevSlotId, SvcCas_TuneInfo_t* pstCasTuneInfo)
{

	HERROR				nResult = ERR_FAIL;
	HUINT32 			ulActionId = 0;
	DbSvc_TripleId_t	triple_id;
	DxDeliveryType_e	eDeliType = 0;
	Handle_t			hCurSvc = HANDLE_NULL, hRequestedSvc = HANDLE_NULL;
	ChListUiGroup_t 	eSvcGroup = 0; // eChListUiGroup_CURRENT 사용하면 이건 불필요 ?

	DbSvc_Condition_t	stSvcCond;
	HINT32				ulSvcCount;
#if 0		// not used
	HINT32				ulSvcIdx;
#endif
	Handle_t			*phSvcList = NULL;

	HxLOG_Info("%s \n", __FUNCTION__);

	if (hAction == HANDLE_NULL)
	{
		/*
			FTA에서 요청된 경우... 이 경우 처리 필요...
			Main Action Id는 Zapper 인 경우에만 사용 가능하다.
		*/
		ulActionId = MGR_ACTION_GetMainActionId();
		pstContents->ulActionId = ulActionId;
	}
	else
	{
		ulActionId = SVC_PIPE_GetActionId(hAction);
		pstContents->ulActionId = ulActionId;
	}

	HxSTD_memset(&s_stCasTuneStatus[ulDevSlotId], 0x00, sizeof(XmgrCas_CiTuneStatus_t));
	s_stCasTuneStatus[ulDevSlotId].hPreSvc = HANDLE_NULL;

	nResult = MGR_LIVE_GetServiceHandle(ulActionId, eLiveViewPlaySvc_ACTUAL, &hCurSvc);
	if (nResult != ERR_OK || hCurSvc == HANDLE_NULL)
	{
		HxLOG_Print("[xmgr_cas_CiTuneByTripleId_v2] MGR_LIVE_GetServiceHandle() error !!\n");
		return ERR_FAIL;
	}
	if (hCurSvc == HANDLE_NULL)
	{
		HxLOG_Error("[xmgr_cas_CiTuneByTripleId_v2] hCurSvc is HANDLE_NULL error !!\n"); // 설마 이런 일이...
	}

	triple_id.usOnId = pstCasTuneInfo->utInfo.stTripleId.usOnId;
	triple_id.usTsId = pstCasTuneInfo->utInfo.stTripleId.usTsId;
	triple_id.usSvcId = pstCasTuneInfo->utInfo.stTripleId.usSvcId;

	HxLOG_Print("[xmgr_cas_CiTuneByTripleId_v2] The requested service : OnId 0x%x, TsId 0x%x, SvcId 0x%x\n", triple_id.usOnId, triple_id.usTsId, triple_id.usSvcId);

	if (triple_id.usSvcId == 0)
	{
		// find DeliveryType
		DbSvc_TsInfo_t 		stTsInfo;
		DxDeliveryType_e	eDeliType = eDxDELIVERY_TYPE_ALL;

		nResult = DB_SVC_GetTsInfoByServiceHandle (hCurSvc, & stTsInfo);
		if(nResult != ERR_OK)
		{
			HxLOG_Error("OxDB_SVC_GetTsInfoByServiceHandle() error\n");
		}
		eDeliType = stTsInfo.eDeliType;


		DB_SVC_InitSvcFindCondition (&stSvcCond);
		stSvcCond.nOrgDeliType = eDeliType;
		stSvcCond.nOnId = triple_id.usOnId;
		stSvcCond.nTsId = triple_id.usTsId;

		nResult = DB_SVC_FindSvcHandleList(&stSvcCond, &ulSvcCount, &phSvcList);
		HxLOG_Print("ulSvcCount = %d, eDeliType = 0x%x\n", ulSvcCount, eDeliType);
		if (nResult != ERR_OK || ulSvcCount < 0 || phSvcList == NULL)
		{
			HxLOG_Error("OxDB_SVC_FindSvcHandleList() error -> Not Found Service !!!\n");
			hRequestedSvc = HANDLE_NULL;
		}
		else
		{
#if 1
			HxLOG_Error("Wildcard SID value, then set the first service in this TP.\n");
			hRequestedSvc = phSvcList[0];	// 일단 첫번째 것만.
#else
			for (ulSvcIdx = 0; ulSvcIdx < ulSvcCount; ulSvcIdx++)
			{
				hRequestedSvc = phSvcList[ulSvcIdx];
			}
#endif
		}

		if(phSvcList != NULL)
		{
			DB_SVC_FreeServiceList(phSvcList);
			phSvcList = NULL;
		}

	}
	else
	{
		eDeliType = eDxDELIVERY_TYPE_ALL;
		nResult = DB_SVC_FindServiceHandleByTripleId(eDeliType, triple_id, &hRequestedSvc);
		if (nResult != ERR_OK || hRequestedSvc == HANDLE_NULL)
		{
			HxLOG_Error("OxDB_SVC_FindServiceHandleByTripleId() error -> Not Found Service !!!\n");
			hRequestedSvc = HANDLE_NULL;
		}

	}

	if (hRequestedSvc == HANDLE_NULL)
	{
		HxLOG_Print("[xmgr_cas_CiTuneByTripleId_v2] Can't tuned to the requested service (not found) !!\n");
		xmgr_cas_CiNotifyTuneStatus(ulDevSlotId, 0x0020, CI_TUNE_STATUS_ERR_Service_Not_Found);
		xproc_cas_CiDestroyCamTune(ulDevSlotId);
		return ERR_FAIL;
	}
	else if (hRequestedSvc == hCurSvc)
	{
		HxLOG_Print("[xmgr_cas_CiTuneByTripleId_v2] No need to tuned to the requested service (it is current service) !!\n");
		xmgr_cas_CiNotifyTuneStatus(ulDevSlotId, 0x0020, CI_TUNE_STATUS_OK);
		xproc_cas_CiDestroyCamTune(ulDevSlotId);
	}
	else
	{
		// TODO: SAMA대체
#if 0//defined(CONFIG_MW_MM_PVR) /* <TODO_CI_PLUS> : CI+ 가 권한을 갖고 있을 때만 이 proc 이 수행되도록 해야 하고 이런 시나리오에 맞춰 작업이 필요함 */
		MgrAction_ConflictInfo_t 	stConflict;
		HUINT32 err;

		HxSTD_memset(&stConflict, 0, sizeof(MgrAction_ConflictInfo_t));
		err = MGR_ACTION_CheckTuningConflict(hCurSvc, &stConflict);
		HxLOG_Print("[xmgr_cas_CiTuneByTripleId_v2][MSG_GWM_CREATE] err=%x, hSvc=%x, stConflict.eType=%d\n", err, hCurSvc, stConflict.eType);
		if(err == ERR_OK && stConflict.eType == eAM_Conflict_Nothing)
		{
		}
		else
		{
			// Create Message "레코딩으로 인해 채널 전환할 수 없다"는 스트링 출력
			// -> 캠에서 요청이 오면 반드시 채널 전환해야 함 (CI+ CAM F/W upgrade 케이스). 따라서 이 경우 레코딩을 중단해야 합니다.
			// break;
			return ERR_FAIL;
		}
#endif

		s_stCasTuneStatus[ulDevSlotId].ulActionId = ulActionId;
		s_stCasTuneStatus[ulDevSlotId].hPreSvc = hCurSvc;
		s_stCasTuneStatus[ulDevSlotId].hRequestedSvc = hRequestedSvc;
		s_stCasTuneStatus[ulDevSlotId].eSvcGroup = eSvcGroup; // eChListUiGroup_CURRENT 사용하면 이건 불필요 ?
		s_stCasTuneStatus[ulDevSlotId].eTuneMode = eCasTuneMode_Start_v2_TuneByTripleId;
		s_stCasTuneStatus[ulDevSlotId].ulDevSlotId = ulDevSlotId;

		pstContents->hPreSvc = hCurSvc;
		pstContents->hRequestedSvc = hRequestedSvc;
		pstContents->eSvcGroup = eSvcGroup;

		HxLOG_Print("[xmgr_cas_CiTuneByTripleId_v2] Now will be tuned to the requested service...\n");

		// OCTO 2.0 구조 변경 전에는 인증시 branch 에서 아래와 같이 사용했는데, eNO_IPLATE|eFORCE_TUNE 인자 처리가 불명확 함. 정상 동작 하면 이 주석은 지울 것.
		// APC_ZAP_SendServiceStartByActionId(ulActionId, hRequestedSvc, eSvcGroup, eNO_IPLATE|eFORCE_TUNE);
		MGR_LIVE_CmdSendServiceStartByActionId(ulActionId, hRequestedSvc);
//		MW_CAS_TuneReply(ulDevSlotId, 0x0020, CI_TUNE_STATUS_OK);
	}
	return ERR_OK;
}


STATIC HERROR xmgr_cas_CiTuneByLocation(XmgrCas_CiTuneContext_t *pstContents, Handle_t hAction, HUINT32 ulDevSlotId, SvcCas_TuneInfo_t* pstCasTuneInfo)
{
	HUINT32 			ulActionId = 0;
	Handle_t			hTmpAction = 0;
	DbSvc_TsInfo_t		stTsInfo;
	DbSvc_TsInfo_t		stCurTsInfo;
	HUINT16				usTsIdx = 0;
	HUINT16				usTsId = 0;
	HUINT16				usOnId = 0;
	DbSvc_TripleId_t	triple_id;
	HERROR				hErr = ERR_FAIL;
	DbSvc_Info_t		stCurSvcInfo, stSvcInfo;
	Handle_t			hCurSvc = HANDLE_NULL, hRequestedSvc = HANDLE_NULL, hNewSvc = HANDLE_NULL;
	ChListUiGroup_t 	eSvcGroup = eChListUiGroup_All; // eChListUiGroup_CURRENT 사용하면 이건 불필요 ?
	HBOOL				bTsExistence = FALSE;
	DbSvc_TsCondition_t	stTsCond;
	HUINT32				nCnt = 0, i = 0;
	DbSvc_TsInfo_t		*pstTsInfoList = NULL;
	DbSvc_AntInfo_t		stAntInfo;
	HUINT16 			usPcrPid = PID_NULL;
	HUINT16 			usVideoPid = PID_NULL;
	HUINT16 			usAudioPid = PID_NULL;
	HUINT16 			usFoundSvcId = 0;

	HxLOG_Info("%s +++\n", __FUNCTION__);

	if (hAction == HANDLE_NULL)
	{
		/*
			FTA에서 요청된 경우... 이 경우 처리 필요...
			Main Action Id는 Zapper 인 경우에만 사용 가능하다.
		*/
		ulActionId = MGR_ACTION_GetMainActionId();
		hTmpAction = SVC_PIPE_GetActionHandle(ulActionId);

		pstContents->ulActionId = ulActionId;
		pstContents->hAction = hTmpAction;

		HxLOG_Print("Action ID:: [%x]\n", ulActionId);
	}
	else
	{
		ulActionId = SVC_PIPE_GetActionId(hAction);

		pstContents->ulActionId = ulActionId;
		pstContents->hAction = hAction;

		HxLOG_Print("Action ID:: [%x]\n", ulActionId);
	}

	HxSTD_memset(&s_stCasTuneStatus[ulDevSlotId], 0x00, sizeof(XmgrCas_CiTuneStatus_t));
	s_stCasTuneStatus[ulDevSlotId].hPreSvc = HANDLE_NULL;

	HxSTD_memset(&stTsInfo, 0x00, sizeof(DbSvc_TsInfo_t));

	if (MGR_LIVE_CurrentSvcInfoGet(ulActionId, &hCurSvc) == ERR_OK)
	{
		if (DB_SVC_GetServiceInfo(hCurSvc, &stCurSvcInfo) == ERR_OK)
		{
			if (DB_SVC_GetTsInfoByServiceHandle(hCurSvc, &stCurTsInfo) == ERR_OK)
			{
				stTsInfo.ucTunerId = stCurTsInfo.ucTunerId;
			}
			else
			{
				HxLOG_Error("DB_SVC_GetTsInfoByServiceHandle() error!! \n");
			}
		}
		else
		{
			hCurSvc = HANDLE_NULL;
			HxLOG_Error("DB_SVC_GetServiceInfo() error!! \n");
		}
	}
	else
	{
		HxLOG_Error("MGR_LIVE_CurrentSvcInfoGet() error !!\n");
		eSvcGroup = eChListUiGroup_All;
		hCurSvc = HANDLE_NULL;
	}
	HxLOG_Print("Tuner ID[%d]\n", stTsInfo.ucTunerId);

	pstContents->hPreSvc = hCurSvc;
	pstContents->eSvcGroup = eSvcGroup;
	pstContents->usServiceId = pstCasTuneInfo->utInfo.stBroadcastReq.usServiceId;
	pstContents->bPmtFlag = pstCasTuneInfo->utInfo.stBroadcastReq.bPmtFlag;

	HxLOG_Print("eDeliType = %d\n", pstCasTuneInfo->utInfo.stBroadcastReq.eDeliType);

	switch (pstCasTuneInfo->utInfo.stBroadcastReq.eDeliType)
	{
		case	eDxDELIVERY_TYPE_SAT:
			if(hCurSvc != HANDLE_NULL)
			{
				HxLOG_Print("AntId [%d]\n", DbSvc_GetAntIdx(&stCurSvcInfo));
				stTsInfo.stTuningParam.sat.ucAntId = DbSvc_GetAntIdx(&stCurSvcInfo);
			}
			else
			{
				HxLOG_Print("Current Service Handel is NULL");
			}

			// Set the antenna info :
			hErr = DB_SVC_GetAntInfo ((HUINT16)stTsInfo.stTuningParam.sat.ucAntId, &stAntInfo);
			if (hErr != ERR_OK)
			{
				// Finish the search
				//
			}

			if(stAntInfo.eSatType != eSatType_ALL)
			{	//pFilterInfo->stAttr.eSatType
				MGR_ANTENNA_GetSatelliteName(stAntInfo.eSatType, stAntInfo.szName, MAX_ANT_NAME);
			}

			pstContents->ulAntennaIndex = (HUINT32)stTsInfo.stTuningParam.sat.ucAntId;
			HxSTD_memcpy (&(pstContents->stAntennaInfo), &stAntInfo, sizeof(DbSvc_AntInfo_t));
			stTsInfo.eDeliType = pstCasTuneInfo->utInfo.stBroadcastReq.eDeliType;
			stTsInfo.stTuningParam.sat.ulFrequency = pstCasTuneInfo->utInfo.stBroadcastReq.stTuningDesInfo.sat.ulFrequency;
			stTsInfo.stTuningParam.sat.ulSymbolRate = pstCasTuneInfo->utInfo.stBroadcastReq.stTuningDesInfo.sat.ulSymbolRate;
			stTsInfo.stTuningParam.sat.eFecCodeRate = pstCasTuneInfo->utInfo.stBroadcastReq.stTuningDesInfo.sat.eFecCodeRate;
			stTsInfo.stTuningParam.sat.ePilot = pstCasTuneInfo->utInfo.stBroadcastReq.stTuningDesInfo.sat.ePilot;
			stTsInfo.stTuningParam.sat.ePolarization = pstCasTuneInfo->utInfo.stBroadcastReq.stTuningDesInfo.sat.ePolarization;
			stTsInfo.stTuningParam.sat.ePskMod = pstCasTuneInfo->utInfo.stBroadcastReq.stTuningDesInfo.sat.ePskMod;
			stTsInfo.stTuningParam.sat.eRollOff = pstCasTuneInfo->utInfo.stBroadcastReq.stTuningDesInfo.sat.eRollOff;
			stTsInfo.stTuningParam.sat.eTransSpec = pstCasTuneInfo->utInfo.stBroadcastReq.stTuningDesInfo.sat.eTransSpec;

			HxLOG_Print("\t\t ePolarization = [%d] -> auto로 강제 변경.\n", stTsInfo.stTuningParam.sat.ePolarization);
			stTsInfo.stTuningParam.sat.ePolarization = eDxSAT_POLAR_AUTO;
			HxLOG_Print("\t\t eFecCodeRate = [%d] -> auto로 강제 변경.\n", stTsInfo.stTuningParam.sat.eFecCodeRate);
			stTsInfo.stTuningParam.sat.eFecCodeRate = eDxSAT_CODERATE_AUTO;

#if 0//defined(CONFIG_DEBUG)		// 디버깅용으로 테스트시에만 필요하다.
	#if HC_TUNE_026_C
						// tuning 을 위해 임시로...
						// 2.3.3 절 step 7 test - service 3 플레이중인 서버로 맞추어야 한다.
						stTsInfo.stTuningParam.sat.ulFrequency = 3350;
						stTsInfo.stTuningParam.sat.ulSymbolRate = 27500;
						stTsInfo.stTuningParam.sat.eFecCodeRate = 6;
						stTsInfo.stTuningParam.sat.ePolarization = 1;
						stTsInfo.stTuningParam.sat.ePilot = 0;
	#endif
	#if HC_TUNE_029_C
						// tuning 을 위해 임시로...
						// 2.3.3 절 step 25 test - service 3 플레이중인 서버로 맞추어야 한다.
						stTsInfo.stTuningParam.sat.ulFrequency = 3750;
						stTsInfo.stTuningParam.sat.ulSymbolRate = 27500;
						stTsInfo.stTuningParam.sat.eFecCodeRate = 4;
						stTsInfo.stTuningParam.sat.ePolarization = 1;
						stTsInfo.stTuningParam.sat.ePilot = 0;
	#endif

	#if HC_TUNE_030_C
						// tuning 을 위해 임시로...
						// 2.3.4 절 step 7 test - service 3 플레이중인 서버로 맞추어야 한다.
						stTsInfo.stTuningParam.sat.ulFrequency = 4050;
						stTsInfo.stTuningParam.sat.ulSymbolRate = 27500;
						stTsInfo.stTuningParam.sat.eFecCodeRate = 4;
						stTsInfo.stTuningParam.sat.ePolarization = 1;
						stTsInfo.stTuningParam.sat.ePilot = 0;
	#endif
	#if HC_TUNE_033_C
						// tuning 을 위해 임시로...
						// 2.3.4 절 step 25 test - service 3 플레이중인 서버로 맞추어야 한다.
						stTsInfo.stTuningParam.sat.ulFrequency = 4100;
						stTsInfo.stTuningParam.sat.ulSymbolRate = 27500;
						stTsInfo.stTuningParam.sat.eFecCodeRate = 4;
						stTsInfo.stTuningParam.sat.ePolarization = 1;
						stTsInfo.stTuningParam.sat.ePilot = 0;
	#endif
	#if HC_TUNE_214_C
						// tuning 을 위해 임시로...
						// 2.7.1 절 step 7 test
						stTsInfo.stTuningParam.sat.ulFrequency = 0;
	#endif
#endif
			stTsInfo.eDeliType = eDxDELIVERY_TYPE_SAT;
			s_eDeliType = eDxDELIVERY_TYPE_SAT;
			OTL_CONV_DbTuningInfo2DxTuneParam (&stTsInfo, &(pstContents->stTuningInfo));

			pstContents->eDeliType = eDxDELIVERY_TYPE_SAT;
			break;

		case	eDxDELIVERY_TYPE_TER:
			stTsInfo.eDeliType = pstCasTuneInfo->utInfo.stBroadcastReq.eDeliType;
			stTsInfo.stTuningParam.ter.ulFreq = pstCasTuneInfo->utInfo.stBroadcastReq.stTuningDesInfo.ter.ulFreq;
			stTsInfo.stTuningParam.ter.ucCodeRate = pstCasTuneInfo->utInfo.stBroadcastReq.stTuningDesInfo.ter.ucCodeRate;
 			stTsInfo.stTuningParam.ter.ucConstellation = pstCasTuneInfo->utInfo.stBroadcastReq.stTuningDesInfo.ter.ucConstellation;
			stTsInfo.stTuningParam.ter.ucHierachy = pstCasTuneInfo->utInfo.stBroadcastReq.stTuningDesInfo.ter.ucHierachy;
			stTsInfo.stTuningParam.ter.ucOffset = pstCasTuneInfo->utInfo.stBroadcastReq.stTuningDesInfo.ter.ucOffset;
			stTsInfo.stTuningParam.ter.ucStream = pstCasTuneInfo->utInfo.stBroadcastReq.stTuningDesInfo.ter.ucStream;
			stTsInfo.stTuningParam.ter.ucTransMode = pstCasTuneInfo->utInfo.stBroadcastReq.stTuningDesInfo.ter.ucTransMode;
			stTsInfo.stTuningParam.ter.ucGuardInterval = pstCasTuneInfo->utInfo.stBroadcastReq.stTuningDesInfo.ter.ucGuardInterval;
			stTsInfo.stTuningParam.ter.ucBandwidth = pstCasTuneInfo->utInfo.stBroadcastReq.stTuningDesInfo.ter.ucBandwidth;
/* DVB T2에 대해서 casmgr_mgr.c 파일의 CASMGR_RequestTuneByCiPlus함수에서 준비가 되면 추가 하도록 할 것!
#if defined(CONFIG_MW_CH_DVB_T2)
			stTsInfo.stTuningParam.ter.stT2param.ulPlpId = pstCasTuneInfo->utInfo.stBroadcastReq.TuningDesInfo.ter.stT2param.ulPlpId;
			stTsInfo.stTuningParam.ter.stT2param.ucPreambleFormat = pstCasTuneInfo->utInfo.stBroadcastReq.TuningDesInfo.ter.stT2param.ucPreambleFormat;
			stTsInfo.stTuningParam.ter.stT2param.ucMixed = pstCasTuneInfo->utInfo.stBroadcastReq.TuningDesInfo.ter.stT2param.ucMixed;
			stTsInfo.stTuningParam.ter.stT2param.ucPilotPattern = pstCasTuneInfo->utInfo.stBroadcastReq.TuningDesInfo.ter.stT2param.ucPilotPattern;
			stTsInfo.stTuningParam.ter.stT2param.ucExtenedCarrier = pstCasTuneInfo->utInfo.stBroadcastReq.TuningDesInfo.ter.stT2param.ucExtenedCarrier;
			stTsInfo.stTuningParam.ter.stT2param.ucPAPRreduction = pstCasTuneInfo->utInfo.stBroadcastReq.TuningDesInfo.ter.stT2param.ucPAPRreduction;
			stTsInfo.stTuningParam.ter.stT2param.ulNumPlpBlocks = pstCasTuneInfo->utInfo.stBroadcastReq.TuningDesInfo.ter.stT2param.ulNumPlpBlocks;
#endif
*/
			stTsInfo.eDeliType = eDxDELIVERY_TYPE_TER;
			s_eDeliType = eDxDELIVERY_TYPE_TER;
			OTL_CONV_DbTuningInfo2DxTuneParam (&stTsInfo, &(pstContents->stTuningInfo));

			pstContents->eDeliType = eDxDELIVERY_TYPE_TER;
			break;

		case	eDxDELIVERY_TYPE_CAB:
			stTsInfo.eDeliType = pstCasTuneInfo->utInfo.stBroadcastReq.eDeliType;
			stTsInfo.stTuningParam.cab.ulFreq = pstCasTuneInfo->utInfo.stBroadcastReq.stTuningDesInfo.cab.ulFreq;
			stTsInfo.stTuningParam.cab.usSR = pstCasTuneInfo->utInfo.stBroadcastReq.stTuningDesInfo.cab.usSR;
			stTsInfo.stTuningParam.cab.usNetworkId = pstCasTuneInfo->utInfo.stBroadcastReq.stTuningDesInfo.cab.usNetworkId;
			stTsInfo.stTuningParam.cab.ucCon = pstCasTuneInfo->utInfo.stBroadcastReq.stTuningDesInfo.cab.ucCon;
			stTsInfo.stTuningParam.cab.ucSpec = pstCasTuneInfo->utInfo.stBroadcastReq.stTuningDesInfo.cab.ucSpec;

			stTsInfo.eDeliType = eDxDELIVERY_TYPE_CAB;
			s_eDeliType = eDxDELIVERY_TYPE_CAB;
			OTL_CONV_DbTuningInfo2DxTuneParam (&stTsInfo, &(pstContents->stTuningInfo));

			pstContents->eDeliType = eDxDELIVERY_TYPE_CAB;
			break;
		default:
			s_eDeliType = eDxDELIVERY_TYPE_ALL;
			break;
	}

	DB_SVC_InitTsFindCondition(&stTsCond);
	stTsCond.nDeliType = pstCasTuneInfo->utInfo.stBroadcastReq.eDeliType;

	hErr = DB_SVC_FindTsInfoList(&stTsCond, &nCnt, &pstTsInfoList);
	if ((hErr == ERR_OK) && (0 < nCnt) && (NULL != pstTsInfoList))
	{
		switch (stTsCond.nDeliType)
		{
			case	eDxDELIVERY_TYPE_SAT:
				for (i=0; i<nCnt; i++)
				{
					if ((stTsInfo.stTuningParam.sat.ulFrequency == pstTsInfoList[i].stTuningParam.sat.ulFrequency) &&
						(stTsInfo.stTuningParam.sat.ulSymbolRate == pstTsInfoList[i].stTuningParam.sat.ulSymbolRate))
					{
						bTsExistence = TRUE;
						usTsIdx = pstTsInfoList[i].usTsIdx;
						usTsId = pstTsInfoList[i].usTsId;
						usOnId = pstTsInfoList[i].usOnId;
						break;
					}
				}
				break;
			case	eDxDELIVERY_TYPE_TER:
				for (i=0; i < nCnt; i++)
				{
					if ((pstTsInfoList[i].stTuningParam.ter.ulFreq == stTsInfo.stTuningParam.ter.ulFreq) &&
#if defined(CONFIG_MW_CH_DVB_T2)
						(pstTsInfoList[i].stTuningParam.ter.ucSystem == stTsInfo.stTuningParam.ter.ucSystem) &&
#endif
						(pstTsInfoList[i].stTuningParam.ter.ucBandwidth == stTsInfo.stTuningParam.ter.ucBandwidth))
					{
						bTsExistence = TRUE;
						usTsIdx = pstTsInfoList[i].usTsIdx;
						usTsId = pstTsInfoList[i].usTsId;
						usOnId = pstTsInfoList[i].usOnId;
						break;
					}
				}
				break;
			case	eDxDELIVERY_TYPE_CAB:
				for (i=0; i < nCnt; i++)
				{
					if ((pstTsInfoList[i].stTuningParam.cab.ulFreq == stTsInfo.stTuningParam.cab.ulFreq) &&
						((stTsInfo.stTuningParam.cab.usSR == CAB_SYMBOLRATE_AUTO) ||
						(pstTsInfoList[i].stTuningParam.cab.usSR == stTsInfo.stTuningParam.cab.usSR)) &&
						(pstTsInfoList[i].stTuningParam.cab.ucSpec == stTsInfo.stTuningParam.cab.ucSpec))
					{
						bTsExistence = TRUE;
						usTsIdx = pstTsInfoList[i].usTsIdx;
						usTsId = pstTsInfoList[i].usTsId;
						usOnId = pstTsInfoList[i].usOnId;
						break;
					}
				}
				break;
			default:
				break;
		}
	}

	if (pstTsInfoList)
	{
		DB_SVC_FreeTsInfoList(pstTsInfoList);
		pstTsInfoList = NULL;
	}

{
	HCHAR	*pszSvcName = NULL;

	//bPmtFlag == TRUE 인경우를 대비해서 미리 저장이 필요하다.
	DB_SVC_InitServiceInfo(&stSvcInfo);
	DbSvc_SetTsIdx(&stSvcInfo, usTsIdx);
	DbSvc_SetTsId(&stSvcInfo, usTsId);
	DbSvc_SetOnId(&stSvcInfo, usOnId);
	DbSvc_SetSvcType(&stSvcInfo, eDxSVC_TYPE_HIDDEN);
	DbSvc_SetDeliType(stSvcInfo, stTsInfo.eDeliType);
	DbSvc_SetName(&stSvcInfo, "CAM Tuning Service");
	pszSvcName = DbSvc_GetName(&stSvcInfo);
	pszSvcName[DbSvc_GetNameLength()-1] = '\0';
}

	if (pstContents->bPmtFlag)
	{
		//	TODO: add service infomation and then tune & change the service
		HxLOG_Print("(bPmtFlag = 1) CHECK THIS CASE !!! >>>>>>>>>>>>>>>>>>>>>>\n\n\n");

		DbSvc_SetSvcId(&stSvcInfo, pstCasTuneInfo->utInfo.stBroadcastReq.usServiceId);
		HxLOG_Print("SvcId is 0x%x, then add the Service Info\n", DbSvc_GetSvcId(&stSvcInfo));

		usPcrPid = pstCasTuneInfo->utInfo.stBroadcastReq.stPmtInfo.usPcrPid;

		if (pstCasTuneInfo->utInfo.stBroadcastReq.stPmtInfo.stEleInfo[0].ucstream_type == 0x2)
		{
			usVideoPid = pstCasTuneInfo->utInfo.stBroadcastReq.stPmtInfo.stEleInfo[0].usElePid;
			usAudioPid = pstCasTuneInfo->utInfo.stBroadcastReq.stPmtInfo.stEleInfo[1].usElePid;
			HxLOG_Print("1 usVideoPid(0x%x), usAudioPid(0x%x)\n", usVideoPid, usAudioPid);
		}
		else if (pstCasTuneInfo->utInfo.stBroadcastReq.stPmtInfo.stEleInfo[1].ucstream_type == 0x2)
		{
			usVideoPid = pstCasTuneInfo->utInfo.stBroadcastReq.stPmtInfo.stEleInfo[1].usElePid;
			usAudioPid = pstCasTuneInfo->utInfo.stBroadcastReq.stPmtInfo.stEleInfo[0].usElePid;
			HxLOG_Print("2 usVideoPid(0x%x), usAudioPid(0x%x)\n", usVideoPid, usAudioPid);
		}
		else
		{
			usVideoPid = pstCasTuneInfo->utInfo.stBroadcastReq.stPmtInfo.stEleInfo[0].usElePid;
			usAudioPid = pstCasTuneInfo->utInfo.stBroadcastReq.stPmtInfo.stEleInfo[1].usElePid;
			HxLOG_Print("3 usVideoPid(0x%x), usAudioPid(0x%x)\n", usVideoPid, usAudioPid);
		}
		DbSvc_SetVideoPid(&stSvcInfo, usVideoPid);
		DbSvc_SetAudioPid(&stSvcInfo, usAudioPid);

		pstContents->usPCRPid = usPcrPid;
		pstContents->usVideoPid = usVideoPid;
		pstContents->usAudioPid = usAudioPid;

		pstContents->usOnId = DbSvc_GetOnId(&stSvcInfo);
		pstContents->usTsId = DbSvc_GetTsId(&stSvcInfo);

		triple_id.usOnId = DbSvc_GetOnId(&stSvcInfo);
		triple_id.usTsId = DbSvc_GetTsId(&stSvcInfo);
		triple_id.usSvcId = DbSvc_GetSvcId(&stSvcInfo);


		// video pid 를 갖는 서비스를 찾는다.
		// 즉, 이 vidoe pid 를 갖고 있는 서비스로 채널전환을 한 후에, 아래에서 audio를 변경하는 방법 사용.
		// Video Pid & Audio Pid로 1차 검색
		hErr = xmgr_cas_CiTuneFindSvcHandleByVideoPidAudioPid(DbSvc_GetDeliType(stSvcInfo), triple_id, usVideoPid, usAudioPid, &hRequestedSvc, &usFoundSvcId);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("\nxmgr_cas_CiTuneFindSvcHandleByVideoPidAudioPid() error!!\n");
			hRequestedSvc = HANDLE_NULL;
		}

		if (hRequestedSvc == HANDLE_NULL)
		{
			// Video Pid로만 2차 검색
			HxLOG_Print("2'nd searching...\n");
			hErr = xmgr_cas_CiTuneFindSvcHandleByVideoPidAudioPid(DbSvc_GetDeliType(stSvcInfo), triple_id, usVideoPid, PID_NULL, &hRequestedSvc, &usFoundSvcId);
			if (hErr != ERR_OK)
			{
				HxLOG_Error("\nxmgr_cas_CiTuneFindSvcHandleByVideoPidAudioPid() error!!\n");
				hRequestedSvc = HANDLE_NULL;
			}
		}

		// 일반캠인 경우 - // 2.8.2 절 test
		// 2.6.2 - step 10 와의 유일한 차이는 CI+ v1.3이냐 일반캠이냐
		if (CI_IsCiPlusCam(ulDevSlotId) == 0)
		{
			HxLOG_Print("CAM V1 \n");
			if ( (hErr == ERR_OK) && (usFoundSvcId != DbSvc_GetSvcId(&stSvcInfo)) )
			{
				HxLOG_Error("\nfound by pid, but mismatched SvcId !!!\n");
				HxLOG_Print("notify CI_TUNE_STATUS_ERR_Tuner_Not_Locking (0x04)\n");
				xmgr_cas_CiNotifyTuneStatus(ulDevSlotId, 0x0020, CI_TUNE_STATUS_ERR_Parameters);
				xproc_cas_CiDestroyCamTune(ulDevSlotId);
				return ERR_FAIL;
			}
		}

		//hRequestedSvc = 0x10003;
		HxLOG_Print("hRequestedSvc = 0x%x\n", hRequestedSvc);
		pstContents->hRequestedSvc = hRequestedSvc;


		// tuning하려는 SvcId 존재 여부 판단.
		hErr = DB_SVC_FindServiceHandleByTripleId(DbSvc_GetDeliType(stSvcInfo), triple_id, &hNewSvc);
		if (hErr != ERR_OK || hNewSvc == HANDLE_NULL)
		{
			HxLOG_Print("OxDB_SVC_FindServiceHandleByTripleId() error -> Not Found Service !!!\n");
			DbSvc_SetSvcType(&stSvcInfo, eDxSVC_TYPE_HIDDEN);
			HxLOG_Print("SvcId is 0x%x, then add the Service Info\n", DbSvc_GetSvcId(&stSvcInfo));
			hErr = DB_SVC_AddServiceInfo(&stSvcInfo, &hNewSvc);
			if(hErr != ERR_OK)
			{
				HxLOG_Error("\nOxDB_SVC_AddServiceInfo() error!!\n");
				hNewSvc = HANDLE_NULL;
			}
		}
		else
		{
			HxLOG_Print("SvcId(0x%x) is existed.\n", DbSvc_GetSvcId(&stSvcInfo));
		}
	}

	if (bTsExistence)
	{
		HxLOG_Print("The TS(From TUNE_Broadcast_Req) is alread added in DB.\n");
		pstContents->bNewTs = FALSE;
	}
	else
	{
		// existing TsInfo 없다.
		HxLOG_Print(">> There is not existing TsInfo (usTsIdx=0x%x)\n", usTsIdx);
		pstContents->bNewTs = TRUE;

		BUS_PostMessage (NULL, eMEVT_SEARCH_ACTION_START, hAction, (HINT32)ulDevSlotId, 0, (HINT32)pstCasTuneInfo);

		return ERR_FAIL;
	}

	if (pstContents->bPmtFlag)
	{
		hErr = SVC_CAS_StartService(pstContents->hAction, pstContents->hRequestedSvc, eCAS_ActionType_Live, NULL);
		if (hErr != ERR_OK)
		{
			HxLOG_Print("\t SVC_CAS_StartService fail!!\n");
			return ERR_FAIL;
		}

		HxLOG_Print("MGR_LIVE_CmdChangeTempPid() - usPcrPid(0x%x), usVideoPid(0x%x), usAudioPid(0x%x)\n",
					usPcrPid, usVideoPid, usAudioPid);
		MGR_LIVE_CmdChangeTempPid(pstContents->hAction, CODEC_CURRENT, usVideoPid, CODEC_CURRENT, usAudioPid, usPcrPid);	// service 5

		xmgr_cas_CiTuneNotifyHciPmtUse(pstContents->hAction, ulDevSlotId);

		return ERR_OK;
	}
	else
	{
		if (pstCasTuneInfo->utInfo.stBroadcastReq.usServiceId == 0)
		{
			//	TODO: Do just tune using the added TS information
			// 튜닝만 하고 서비스 변경은 안할 수 있나??
			HxLOG_Print("SvcId is 0x00, then add the Service Info\n");

			hErr = DB_SVC_AddServiceInfo(&stSvcInfo, &hRequestedSvc);
			if(hErr != ERR_OK)
			{
				HxLOG_Error("DB_SVC_AddServiceInfo() Error!! \n");
				hRequestedSvc = HANDLE_NULL;
			}
		}
		else
		{
			//	TODO: Change the current service using the added TS information and service id
			DbSvc_SetSvcId(&stSvcInfo, pstCasTuneInfo->utInfo.stBroadcastReq.usServiceId);
			HxLOG_Print("SvcId is 0x%x, then just find the service\n", DbSvc_GetSvcId(&stSvcInfo));

			triple_id.usOnId = DbSvc_GetOnId(&stSvcInfo);
			triple_id.usTsId = DbSvc_GetTsId(&stSvcInfo);
			triple_id.usSvcId = DbSvc_GetSvcId(&stSvcInfo);

			hErr = DB_SVC_FindServiceHandleByTripleId(DbSvc_GetDeliType(stSvcInfo), triple_id, &hRequestedSvc);
			if (hErr != ERR_OK || hRequestedSvc == HANDLE_NULL)
			{
				HxLOG_Print("OxDB_SVC_FindServiceHandleByTripleId() error -> Not Found Service !!!\n");
#if 0
				HxLOG_Print("need not to save this svcId(0x%x) \n", stSvcInfo.usSvcId);
#else

				HxLOG_Print("SvcId is 0x%x, then add the Service Info\n", DbSvc_GetSvcId(&stSvcInfo));
				hErr = DB_SVC_AddServiceInfo(&stSvcInfo, &hRequestedSvc);
				if(hErr != ERR_OK)
				{
					HxLOG_Error("OxDB_SVC_AddServiceInfo() Error!! \n");
					hRequestedSvc = HANDLE_NULL;
				}
#endif
			}
		}
	}

	HxLOG_Print("\t hCurSvc[0x%x] <-> stSvcInfo.usSvcId[0x%x], hRequestedSvc[0x%x]\n", hCurSvc, DbSvc_GetSvcId(&stSvcInfo), hRequestedSvc);

	if (hRequestedSvc == HANDLE_NULL)
	{
		HxLOG_Error("Can't tuned to the requested service (not found, HANDLE_NULL) !!\n");
		xmgr_cas_CiNotifyTuneStatus(ulDevSlotId, 0x0020, CI_TUNE_STATUS_ERR_Service_Not_Found);
		xproc_cas_CiDestroyCamTune(ulDevSlotId);
		return ERR_FAIL;
	}
	else if (hRequestedSvc == hCurSvc)
	{
		HxLOG_Print("No need to tuned to the requested service (it is current service) !!\n");
		xmgr_cas_CiNotifyTuneStatus(ulDevSlotId, 0x0020, CI_TUNE_STATUS_OK);
		xproc_cas_CiDestroyCamTune(ulDevSlotId);
		return ERR_OK;
	}

	s_stCasTuneStatus[ulDevSlotId].ulActionId = ulActionId;
	s_stCasTuneStatus[ulDevSlotId].hPreSvc = hCurSvc;
	s_stCasTuneStatus[ulDevSlotId].hRequestedSvc = hRequestedSvc;
	s_stCasTuneStatus[ulDevSlotId].eSvcGroup = eSvcGroup; // eChListUiGroup_CURRENT 사용하면 이건 불필요 ?
	s_stCasTuneStatus[ulDevSlotId].eTuneMode = eCasTuneMode_Start_v2_TuneByLocation;
	s_stCasTuneStatus[ulDevSlotId].ulDevSlotId = ulDevSlotId;

	pstContents->hRequestedSvc = hRequestedSvc;

	// OCTO 2.0 구조 변경 전에는 인증시 branch 에서 아래와 같이 사용했는데, eNO_IPLATE|eFORCE_TUNE 인자 처리가 불명확 함. 정상 동작 하면 이 주석은 지울 것.
	// APC_ZAP_SendServiceStartByActionId(ulActionId, hRequestedSvc, eSvcGroup, eNO_IPLATE|eFORCE_TUNE);
	MGR_LIVE_CmdSendServiceStartByActionId(ulActionId, hRequestedSvc);	// eFORCE_TUNE = 0x10	// 동작 문제 없는지 확인 필요. branch code rev. 94106 참고

/*
	hErr = OxDB_SVC_DeleteServiceInfo(hNewSvc);
	if(hErr != ERR_OK)
	{
		ApCasTuneAssert(0);
		return ERR_FAIL;
	}
*/
	HxLOG_Info("%s ---\n", __FUNCTION__);

	return ERR_OK;
}
#endif


/*******************************************************************/
/********************      MSG Functions   *************************/
/*******************************************************************/
#define	_____INTERNAL_MSG_PROCESS_FUNCTION_____


//eMEVT_BUS_CREATE
static HERROR xmgr_cas_CiTuneMsgGwmCreate (XmgrCas_CiTuneContext_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR		 hErr;
	HUINT32		i = 0;

	for ( i = 0; i < XMGR_CAS_CI_SLOT_MAX; i++)
	{
		pstContents = xmgr_cas_CiTuneGetContext(i);
		if(pstContents != NULL)
		{
			hErr = xmgr_cas_CiTuneInitContents (pstContents, hAction);
			if (hErr != ERR_BUS_NO_ERROR)
			{
				BUS_MGR_Destroy (0);
				return hErr;
			}
		}
		else
		{
			HxLOG_Critical("Get CI+ UI Context FAIL!!\n");
		}
	}

	return ERR_BUS_NO_ERROR;
}


//MGR_EVT_CAS_CAM_TUNE
static HERROR xmgr_cas_CiTuneMsgGwmCasCamTune (XmgrCas_CiTuneContext_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	//HERROR		 	hErr;
	//HUINT32			i = 0;
	SvcCas_TuneInfo_t	*pstCasTuneInfo = NULL;
	HUINT32 			ulSlotNumber = 0;
	HUINT32 			ulActionId = 0;
	Handle_t			hTmpAction = 0;

	HxLOG_Info("%s \n", __FUNCTION__);

	if (hAction == HANDLE_NULL)
	{
		/*
			FTA에서 요청된 경우... 이 경우 처리 필요...
			Main Action Id는 Zapper 인 경우에만 사용 가능하다.
		*/
		ulActionId = MGR_ACTION_GetMainActionId();
		hTmpAction = SVC_PIPE_GetActionHandle(ulActionId);

		pstContents->ulActionId = ulActionId;
		pstContents->hAction = hTmpAction;

		HxLOG_Print("hAction == HANDLE_NULL -> ulActionId = %d, hTmpAction = 0x%x\n", ulActionId, hTmpAction);
	}
	else
	{
		ulActionId = SVC_PIPE_GetActionId(hAction);

		pstContents->ulActionId = ulActionId;
		pstContents->hAction = hAction;

		HxLOG_Print("ulActionId = %d\n", ulActionId);
	}

	ulSlotNumber = (HUINT32)p1;
	pstCasTuneInfo = (SvcCas_TuneInfo_t *)p3;

	pstContents->eTuneMode = pstCasTuneInfo->eMode;
	pstContents->ulDevSlotId = ulSlotNumber;
	HxLOG_Print("hAction = 0x%x, ulActionId = %d\n", pstContents->hAction, pstContents->ulActionId);
	HxLOG_Print("ulDevSlotId = %d, eTuneMode = %d\n", pstContents->ulDevSlotId, pstContents->eTuneMode);

	//	TODO:	Ask user using MMI that whether Tune or not.
	// 만약 튜닝 하라는 입력을 받으면 튜닝 하고, 하지 말라는 입력을 받으면 TUNE Reply 에 Fail 정보를 담아서 CAM에 전송

	switch (pstCasTuneInfo->eMode)
	{
		case eCasTuneMode_Start_v1_TuneByTripleId:
			HxLOG_Print("eCasTuneMode_Start_v1_TuneByTripleId\n");

			xmgr_cas_CiTuneByTripleId_v1(pstContents, pstContents->hAction, ulSlotNumber, pstCasTuneInfo);
			xproc_cas_CiDestroyCamTune(ulSlotNumber);
			break;
#if defined(CONFIG_MW_CI_PLUS_HCI_V_2)
		case eCasTuneMode_Start_v2_TuneByTripleId:
			HxLOG_Print("eCasTuneMode_Start_v2_TuneByTripleId\n");

			xmgr_cas_CiTuneByTripleId_v2(pstContents, pstContents->hAction, ulSlotNumber, pstCasTuneInfo);
			break;
		case eCasTuneMode_Start_v2_TuneByLocation:
			HxLOG_Print("eCasTuneMode_Start_v2_TuneByLocation\n");

			xmgr_cas_CiTuneByLocation(pstContents, pstContents->hAction, ulSlotNumber, pstCasTuneInfo);
			break;
#endif
		default:
			break;
	}

	BUS_SetTimer (CITUNE_TIMER_LOCKING_ID, CITUNE_TIMER_LOCKING_TIME);

	return MESSAGE_BREAK;
}

static BUS_Result_t xmgr_cas_CiTuneMsgTimer (XmgrCas_CiTuneContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32 		nTimerId = p1;

	HxLOG_Info("%s \n", __FUNCTION__);

	switch(nTimerId)
	{
		case CITUNE_TIMER_LOCKING_ID:
			BUS_KillTimer (nTimerId);

			HxLOG_Info("CITUNE_TIMER_LOCKING_ID: \n");

			// Stop SI & Cancel the search :
			SVC_SI_StopSearch (pstContents->hAction, TRUE);

			// Search가 끝나지 않았다면 Action Release
			xmgr_cas_CiTuneReleaseAction (pstContents);

			HxLOG_Print("notify CI_TUNE_STATUS_ERR_Tuner_Not_Locking (0x02)\n");
			xmgr_cas_CiNotifyTuneStatus(pstContents->ulDevSlotId, 0x0020, CI_TUNE_STATUS_ERR_Tuner_Not_Locking);
			xproc_cas_CiDestroyCamTune(0);
			xproc_cas_CiDestroyCamTune(1);

			return MESSAGE_BREAK;

		case CITUNE_TIMER_SI_ID:
			BUS_KillTimer (nTimerId);

			HxLOG_Print("notify CI_TUNE_STATUS_ERR_Tuner_Not_Locking (0x02)\n");
			xmgr_cas_CiNotifyTuneStatus(pstContents->ulDevSlotId, 0x0020, CI_TUNE_STATUS_ERR_Tuner_Not_Locking);
			xproc_cas_CiDestroyCamTune(0);
			xproc_cas_CiDestroyCamTune(1);

			return MESSAGE_BREAK;
	}

	return MESSAGE_PASS;
}

static HERROR xmgr_cas_CiTuneMsgGwmSearchActionStart(XmgrCas_CiTuneContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR				hError = ERR_OK;
	SvcRsc_Info_t		stRscInfo;

	HxLOG_Info("%s +++\n", __FUNCTION__);

	hError = MGR_ACTION_SetupSearch (pstContents->ulActionId, &pstContents->stTuningInfo, eAmPriority_SEARCH);
	if (hError != ERR_OK)
	{
		HxLOG_Error("\n\n");
		return ERR_FAIL;
	}

	hError = MGR_ACTION_Take(pstContents->ulActionId, eActionType_SEARCH, BUS_MGR_GetMgrCallback(BUS_MGR_GetThis()));
	if (hError != ERR_OK)
	{
		HxLOG_Error("\n\n");
		return ERR_FAIL;
	}


	MGR_RSC_GetResourceInfo (pstContents->ulActionId, &stRscInfo);

	hError = SVC_CH_StopTune (pstContents->hAction, (HUINT16)-1, eSyncMode);
	if (hError != ERR_OK)
	{
		HxLOG_Error("SVC_CH_StopTune fail!!\n");
		return ERR_FAIL;
	}

	xmgr_cas_CiTuneIncreaseSessionVersion (pstContents);

	hError = SVC_CH_StartTune(pstContents->hAction, &stRscInfo, (HUINT16)-1, &pstContents->stTuningInfo);
	if (hError != ERR_OK)
	{
		HxLOG_Error("SVC_CH_StartTune fail!!\n");
		return ERR_FAIL;
	}

	HxLOG_Info("%s ---\n", __FUNCTION__);

	return MESSAGE_BREAK;
}

static HERROR xmgr_cas_CiTuneMsgGwmEvtChLocked(XmgrCas_CiTuneContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR			hError = ERR_OK;
	HUINT32			ulDemuxId;
#if 0		// not used
	Handle_t		hSiModule;
#endif

	// TODO: xmgr_cas_CiTuneMsgGwmEvtChLocked함수가 제대로 동작 하지 않을 가능성이 큼.
	// TODO: xmgr_search_SatEvtChLocked() 함수 참고하여 제대로 수정하고, 동작 여부 테스트 필요 함.
	// (xmgr_live_satrec_EvtChLocked_Ses(), XMGR_MEMREC_MsgEvtChLocked_Base(), xmgr_ertelecomsrch_EvtChLocked(), xmgr_search_CabEvtChLocked())
	HxLOG_Info("%s \n", __FUNCTION__);

	hError = MGR_RSC_GetResourceId(pstContents->ulActionId, eRxRSCTYPE_DEMUX, &ulDemuxId);
	if(hError == ERR_OK)
	{
		SvcSi_SearchSpec_t		stSrchSpec;
		SvcSi_SearchOption_t	stSrchOption;

		HxSTD_memset(&stSrchOption, 0x00, sizeof(SvcSi_SearchOption_t));
		stSrchOption.ulSearchType	= (HUINT32)eSVCSI_SEARCHTYPE_Normal;
		stSrchOption.ulSvcType		= (HUINT32)eDxSVC_TYPE_All;
		stSrchOption.ulCasType		= (HUINT32)eDxCAS_TYPE_All;
		stSrchOption.hSession		= (SvcSi_SessionH_t)HANDLE_NULL;	// not used param
		stSrchOption.ulNetId		= SvcSi_NETID_SEARCH_NOTNIT;

		hError = MGR_SEARCH_GetSiSpec(&stSrchOption, &stSrchSpec);
		if (hError != ERR_FAIL)
		{
			hError = SVC_SI_StartSearch(pstContents->hAction,
											&stSrchSpec,
											&stSrchOption,
											TRUE);
		}
	}

	return MESSAGE_BREAK;
}

static HERROR xmgr_cas_CiTuneMsgGwmEvtSiSearchDone(XmgrCas_CiTuneContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR			hError = ERR_OK;
	Handle_t 		hSvc = HANDLE_NULL;
	HUINT32 		ulDevSlotId;
	Handle_t		hRequestedSvc = HANDLE_NULL;
	DbSvc_TripleId_t	triple_id;
	HUINT16 			usTsId = 0;
	HUINT16				usOnId = 0;
	HUINT16 			usFoundSvcId = 0;

	HxLOG_Info("%s +++\n", __FUNCTION__);
	pstContents->eDeliType = s_eDeliType;

	if (pstContents->eDeliType == eDxDELIVERY_TYPE_SAT)
	{
		hError = xmgr_cas_CiTuneSatRetrieveResults (pstContents, &hSvc, &usTsId, &usOnId);
	}
	else if (pstContents->eDeliType == eDxDELIVERY_TYPE_CAB)
	{
		hError = xmgr_cas_CiTuneCabRetrieveResults (pstContents, &hSvc);
	}
	else if (pstContents->eDeliType == eDxDELIVERY_TYPE_TER)
	{
		hError = xmgr_cas_CiTuneTerRetrieveResults (pstContents, &hSvc);
	}

	// Stop SI & Cancel the search :
	SVC_SI_StopSearch (pstContents->hAction, TRUE);

	// Search가 끝나지 않았다면 Action Release
	xmgr_cas_CiTuneReleaseAction (pstContents);

	if (hError != ERR_OK)
	{
		HxLOG_Error("\nxmgr_cas_CiTuneSatRetrieveResults() error!!\n");
		return ERR_FAIL;
	}

	ulDevSlotId = pstContents->ulDevSlotId;

	s_stCasTuneStatus[ulDevSlotId].ulActionId = pstContents->ulActionId;
	s_stCasTuneStatus[ulDevSlotId].hPreSvc = pstContents->hPreSvc;
	s_stCasTuneStatus[ulDevSlotId].hRequestedSvc = hSvc;
	s_stCasTuneStatus[ulDevSlotId].eSvcGroup = pstContents->eSvcGroup; // eChListUiGroup_CURRENT 사용하면 이건 불필요 ?
	s_stCasTuneStatus[ulDevSlotId].eTuneMode = eCasTuneMode_Start_v2_TuneByLocation;
	s_stCasTuneStatus[ulDevSlotId].ulDevSlotId = pstContents->ulDevSlotId;

	pstContents->bNewTs = FALSE;			// reset해줘야 다시 eSEVT_CH_LOCKED 에서 끝낼 수 있다.

	if (pstContents->bPmtFlag == TRUE)
	{
		HxLOG_Print("pstContents->hRequestedSvc = 0x%x\n", pstContents->hRequestedSvc);
		if (pstContents->hRequestedSvc == HANDLE_NULL)
		{
			triple_id.usOnId = usOnId;
			triple_id.usTsId = usTsId;
			triple_id.usSvcId = PID_NULL;

			// video pid 를 갖는 서비스를 찾는다.
			// 즉, 이 vidoe pid 를 갖고 있는 서비스로 채널전환을 한 후에, 아래에서 audio를 변경하는 방법 사용.
			// Video Pid & Audio Pid로 1차 검색
			hError = xmgr_cas_CiTuneFindSvcHandleByVideoPidAudioPid(pstContents->eDeliType, triple_id, pstContents->usVideoPid, pstContents->usAudioPid, &hRequestedSvc, &usFoundSvcId);
			if (hError != ERR_OK)
			{
				HxLOG_Error("\nerror!!\n");
				hRequestedSvc = HANDLE_NULL;
			}
			if (hRequestedSvc == HANDLE_NULL)
			{
				// Video Pid로만 2차 검색
				HxLOG_Print("2'nd searching...\n");
				hError = xmgr_cas_CiTuneFindSvcHandleByVideoPidAudioPid(pstContents->eDeliType, triple_id, pstContents->usVideoPid, PID_NULL, &hRequestedSvc, &usFoundSvcId);
				if (hError != ERR_OK)
				{
					HxLOG_Error("\nerror!!\n");
					hRequestedSvc = HANDLE_NULL;
				}
			}

			HxLOG_Print("hRequestedSvc = 0x%x\n", hRequestedSvc);
			pstContents->hRequestedSvc = hRequestedSvc;
		}

#if 1
		// valid service 0x8, 0x9는 이걸로 정상 동작.
		HxLOG_Print("\t >> zapping to the hSvc(0x%x) \n", pstContents->hRequestedSvc);
		MGR_LIVE_CmdSendServiceStartByActionId(pstContents->ulActionId, hSvc);	// eFORCE_TUNE = 0x10
#else
				// 이 경우는 local_CiMgr_ProcPmtChangedLocation() 에서 pCiContext->pCam == NULL 이 되어 버린다. 안되요....
		HxLOG_Print("SVC_CAS_StartService() \n");
		hError = SVC_CAS_StartService(pstContents->hAction, pstContents->hRequestedSvc, eCAS_ActionType_Live, NULL);
		if (hError != ERR_OK)
		{
			HxLOG_Print("\t MW_CAS_StartService fail!!\n");
			return ERR_FAIL;
		}

		HxLOG_Print("usPcrPid(0x%x), usVideoPid(0x%x), usAudioPid(0x%x)\n",
					pstContents->usPCRPid, pstContents->usVideoPid, pstContents->usAudioPid);
		MGR_LIVE_CmdChangeTempPid(pstContents->hAction, CODEC_CURRENT, pstContents->usVideoPid, CODEC_CURRENT, pstContents->usAudioPid, pstContents->usPCRPid);	// service 5

		xmgr_cas_CiTuneNotifyHciPmtUse(pstContents->hAction, pstContents->ulDevSlotId);
#endif
	}
	else
	{
		HxLOG_Print("\t >> zapping to the hSvc(0x%x) \n", hSvc);
		// OCTO 2.0 구조 변경 전에는 인증시 branch 에서 아래와 같이 사용했는데, eNO_IPLATE|eFORCE_TUNE 인자 처리가 불명확 함. 정상 동작 하면 이 주석은 지울 것.
		// APC_ZAP_SendServiceStartByActionId(pstContents->ulActionId, hSvc, pstContents->eSvcGroup, eNO_IPLATE|eFORCE_TUNE);	// eFORCE_TUNE = 0x10
		MGR_LIVE_CmdSendServiceStartByActionId(pstContents->ulActionId, hSvc);	// eFORCE_TUNE = 0x10
	}

	HxLOG_Info("%s ---\n", __FUNCTION__);

	return MESSAGE_BREAK;
}

static HERROR xmgr_cas_CiTuneMsgGwmDestroy (XmgrCas_CiTuneContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
//	SvcCas_TuneInfo_t	*pstCasTuneInfo = NULL;
//	HUINT32 			ulSlotNumber = 0;

//	ulSlotNumber = (HUINT32)p1;
//	pstCasTuneInfo = (SvcCas_TuneInfo_t *)p3;

	HxLOG_Info("%s \n", __FUNCTION__);

	BUS_KillTimer (CITUNE_TIMER_LOCKING_ID);

#if 0
	//if (s_stCasTuneStatus[ulSlotNumber].hPreSvc != HANDLE_NULL)
	if (pstContents->hPreSvc != HANDLE_NULL)
	{
		// SAMA대체
#if 0//defined(CONFIG_MW_MM_PVR) /* <TODO_CI_PLUS> : CI+ 가 권한을 갖고 있을 때만 이 proc 이 수행되도록 해야 하고 이런 시나리오에 맞춰 작업이 필요함 */
		AM_Conflict_Info_t	stConflict;
		HUINT32 err;

		HxSTD_memset(&stConflict, 0, sizeof(AM_Conflict_Info_t));
		//err = AP_AM_CheckTuningConflict(s_stCasTuneStatus[ulSlotNumber].hPreSvc, &stConflict);
		err = AP_AM_CheckTuningConflict(pstContents->hPreSvc, &stConflict);
		if(err == ERR_OK && stConflict.eType == eAM_Conflict_Nothing)
		{
		}
		else
		{
			// Create Message "레코딩으로 인해 채널 전환할 수 없다"는 스트링 출력
			// -> 캠에서 요청이 오면 반드시 채널 전환해야 함 (CI+ CAM F/W upgrade 케이스). 따라서 이 경우 레코딩을 중단해야 합니다.
			// break;
		}
#endif

		HxLOG_Print("Now will be tuned back to the original service...\n");

		//APC_ZAP_SendServiceStartByActionId(s_stCasTuneStatus[ulSlotNumber].ulActionId, CURRENT_SVC_HANDLE, eChListUiGroup_CURRENT, 0);
		// TODO: 아래 동작사항 확인 필요 (이전에는 pstContents->hPreSvc이 아니고, CURRENT_SVC_HANDLE 이였음.
		// OCTO 2.0 구조 변경 전에는 인증시 branch 에서 아래와 같이 사용했는데, eNO_IPLATE|eFORCE_TUNE 인자 처리가 불명확 함. 정상 동작 하면 이 주석은 지울 것.
		// APC_ZAP_SendServiceStartByActionId(pstContents->ulActionId, CURRENT_SVC_HANDLE, eChListUiGroup_CURRENT, eNO_IPLATE|eFORCE_TUNE);	// eFORCE_TUNE = 0x10
		MGR_LIVE_CmdSendServiceStartByActionId(pstContents->ulActionId, pstContents->hPreSvc);

		HxSTD_memset(&s_stCasTuneStatus[ulSlotNumber], 0x00, sizeof(XmgrCas_CiTuneStatus_t));
		s_stCasTuneStatus[ulSlotNumber].hPreSvc = HANDLE_NULL;
	}
#endif

	return MESSAGE_BREAK;
}



BUS_Result_t xproc_cas_CiTuneMgr(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t 			eBusRes = ERR_BUS_NO_ERROR;
	XmgrCas_CiTuneContext_t	*pstContents = NULL;
	HUINT32 				unSlotNumber = 0;
	SvcCas_TuneInfo_t	*	pstCasTuneInfo = NULL;

	HxLOG_Info("%s \n", __FUNCTION__);
	switch(message)
	{
		case eMEVT_BUS_CREATE :
			HxLOG_Print(">>> [AP_CAS_CI_TuneMgr_Proc] eMEVT_BUS_CREATE:\n");
			eBusRes = xmgr_cas_CiTuneMsgGwmCreate(pstContents, hAction, p1, p2, p3);
			if (eBusRes == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }
			break;

		case eMEVT_CAS_CAM_TUNE :
			// TODO: 아래 break 이전 부분 OCTO 2.0 구조에 맞춰 구현.
			unSlotNumber = (HUINT32)p1;
			pstCasTuneInfo = (SvcCas_TuneInfo_t *)p3;

			if (unSlotNumber >= 2)
			{
				HxLOG_Error("[xproc_cas_CiTuneMgr][eMEVT_CAS_CAM_TUNE] unSlotNumber %d error !!\n", unSlotNumber);
				break;
			}

			if (pstCasTuneInfo == NULL)
			{
				HxLOG_Error("[xproc_cas_CiTuneMgr] pstCasTuneInfo is NULL error !!\n");
				break;
			}

			pstContents = xmgr_cas_CiTuneGetContext(unSlotNumber);
			if(pstContents == NULL)
			{
				HxLOG_Error("Error !!\n", unSlotNumber);
				break;
			}

			eBusRes = xmgr_cas_CiTuneMsgGwmCasCamTune(pstContents, hAction, p1, p2, p3);

			s_ulDevSlotId = unSlotNumber;
			if (eBusRes == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }
			break;

		case eMEVT_BUS_TIMER:
			HxLOG_Print("[%s] >>> eMEVT_BUS_TIMER:\n", __FUNCTION__);
			HxLOG_Print("\t\t hAction = 0x%x, p1 = 0x%x \n", hAction, p1);

			//ulSlotNumber = (HUINT32)p1;
			unSlotNumber = s_ulDevSlotId;
			HxLOG_Print("\t\t ulSlotNumber = %d \n", unSlotNumber);

			pstContents = xmgr_cas_CiTuneGetContext(unSlotNumber);
			if(pstContents == NULL)
			{
				HxLOG_Error("\nerror!!\n");
				break;
			}

			eBusRes = xmgr_cas_CiTuneMsgTimer(pstContents, p1, p2, p3);
			if (eBusRes == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }
			break;

		case eMEVT_BUS_DESTROY :
			unSlotNumber = s_ulDevSlotId;

			pstContents = xmgr_cas_CiTuneGetContext(unSlotNumber);
			if(pstContents == NULL)
			{
				HxLOG_Error("Error !!\n", unSlotNumber);
				break;
			}

			eBusRes = xmgr_cas_CiTuneMsgGwmDestroy(pstContents, p1, p2, p3);
			if (eBusRes == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }
			break;

		case eMEVT_SEARCH_ACTION_START:
			HxLOG_Print(">>> MSG_APP_SEARCH_ACTION_START\n");

			unSlotNumber = (HUINT32)p1;

			pstContents = xmgr_cas_CiTuneGetContext(unSlotNumber);
			if (pstContents == NULL)
			{
				HxLOG_Error("xmgr_cas_CiTuneGetContext() error !!\n");
				break;
			}

			eBusRes = xmgr_cas_CiTuneMsgGwmSearchActionStart(pstContents, p1, p2, p3);
			if (eBusRes == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }
			break;

		case eMEVT_SEARCH_STARTED:
			HxLOG_Print(">>> MSG_APP_SEARCH_STARTED\n");
			break;

		case eSEVT_SI_SEARCH_FAIL:
			HxLOG_Print(">>> eSEVT_SI_SEARCH_FAIL\n");
			break;

		case eSEVT_CH_LOCKED:
			HxLOG_Print(">>> eSEVT_CH_LOCKED\n");
			unSlotNumber = s_ulDevSlotId;

			pstContents = xmgr_cas_CiTuneGetContext(unSlotNumber);
			if(pstContents == NULL)
			{
				HxLOG_Error("xmgr_cas_CiTuneGetContext() error !!\n", unSlotNumber);
				break;
			}

			if (pstContents->bNewTs == TRUE)
			{
				// eCasTuneMode_Start_v2_TuneByLocation 는 여기서 처리된다.
				eBusRes = xmgr_cas_CiTuneMsgGwmEvtChLocked (pstContents, p1, p2, p3);
			}
			else
			{
				switch (pstContents->eTuneMode)
				{
					case eCasTuneMode_Start_v1_TuneByTripleId:
						HxLOG_Print("what to do ???\n");
						break;
#if defined(CONFIG_MW_CI_PLUS_HCI_V_2)
					case eCasTuneMode_Start_v2_TuneByTripleId:
						HxLOG_Print("what to do ???\n");
						break;
					case eCasTuneMode_Start_v2_TuneByLocation:
						HxLOG_Print(">> Do nothing!!!\n");
						BUS_SetTimer (CITUNE_TIMER_SI_ID, CITUNE_TIMER_SI_TIME);
						break;
#endif
					default:
						break;
				}
				return MESSAGE_BREAK;
			}
			if (eBusRes == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }

			break;

		case eSEVT_SI_SEARCH_DONE:
			HxLOG_Print(">>> eSEVT_SI_SEARCH_DONE\n");

			//ulSlotNumber = (HUINT32)p1;
			unSlotNumber = s_ulDevSlotId;

			pstContents = xmgr_cas_CiTuneGetContext(unSlotNumber);
			if(pstContents == NULL)
			{
				HxLOG_Error("xmgr_cas_CiTuneGetContext() error !!\n");
				break;
			}

			eBusRes = xmgr_cas_CiTuneMsgGwmEvtSiSearchDone(pstContents, p1, p2, p3);
			if (eBusRes == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }
			break;

		case eSEVT_AV_AUDIO_STARTED:
			HxLOG_Print(">>> eSEVT_AV_AUDIO_STARTED\n");
			break;

		case eSEVT_AV_AUDIO_UNDERRUN:
			HxLOG_Print(">>> eSEVT_AV_AUDIO_UNDERRUN\n");
			break;

		case eSEVT_AV_VIDEO_STARTED:
			HxLOG_Print(">>> eSEVT_AV_VIDEO_STARTED\n");

			// eCasTuneMode_Start_v1_TuneByTripleId, eCasTuneMode_Start_v2_TuneByTripleId 는 여기서 처리된다.

			//ulSlotNumber = (HUINT32)p1;
			unSlotNumber = s_ulDevSlotId;

			pstContents = xmgr_cas_CiTuneGetContext(unSlotNumber);
			if(pstContents == NULL)
			{
				HxLOG_Error("xmgr_cas_CiTuneGetContext() error !!\n");
				break;
			}

			HxLOG_Print("notify CI_TUNE_STATUS_OK (0x00)\n");
			xmgr_cas_CiNotifyTuneStatus(unSlotNumber, 0x0020, CI_TUNE_STATUS_OK);
			xproc_cas_CiDestroyCamTune(unSlotNumber);
			return MESSAGE_BREAK;

		case eSEVT_AV_VIDEO_UNDERRUN:
			HxLOG_Print(">>> eSEVT_AV_VIDEO_UNDERRUN\n");

			unSlotNumber = s_ulDevSlotId;

			pstContents = xmgr_cas_CiTuneGetContext(unSlotNumber);
			if(pstContents == NULL)
			{
				HxLOG_Error("xmgr_cas_CiTuneGetContext() error !!\n");
				break;
			}

			HxLOG_Print("notify CI_TUNE_STATUS_ERR_Tuner_Not_Locking (0x02)\n");
			xmgr_cas_CiNotifyTuneStatus(unSlotNumber, 0x0020, CI_TUNE_STATUS_ERR_Tuner_Not_Locking);
			xproc_cas_CiDestroyCamTune(0);
			xproc_cas_CiDestroyCamTune(1);

			break;

		case eSEVT_CH_NO_SIGNAL:
			HxLOG_Print("eSEVT_CH_NO_SIGNAL\n");

			unSlotNumber = s_ulDevSlotId;

			pstContents = xmgr_cas_CiTuneGetContext(unSlotNumber);
			if(pstContents == NULL)
			{
				HxLOG_Error("xmgr_cas_CiTuneGetContext() error !!\n");
				break;
			}

			HxLOG_Print("notify CI_TUNE_STATUS_ERR_Tuner_Not_Locking (0x04)\n");
			xmgr_cas_CiNotifyTuneStatus(unSlotNumber, 0x0020, CI_TUNE_STATUS_ERR_Parameters);
			xproc_cas_CiDestroyCamTune(0);
			xproc_cas_CiDestroyCamTune(1);

			break;

		case eSEVT_SI_PMT_TIMEOUT:
			HxLOG_Print("eSEVT_SI_PMT_TIMEOUT\n");

			unSlotNumber = s_ulDevSlotId;

			pstContents = xmgr_cas_CiTuneGetContext(unSlotNumber);
			if(pstContents == NULL)
			{
				HxLOG_Error("xmgr_cas_CiTuneGetContext() error !!\n");
				break;
			}

			HxLOG_Print("notify CI_TUNE_STATUS_ERR_Service_Not_Found (0x05)\n");
			xmgr_cas_CiNotifyTuneStatus(unSlotNumber, 0x0020, CI_TUNE_STATUS_ERR_Service_Not_Found);
			xproc_cas_CiDestroyCamTune(0);
			xproc_cas_CiDestroyCamTune(1);

			break;
	}

	eBusRes = BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);

	return eBusRes;
}


/*********************** End of File ******************************/

