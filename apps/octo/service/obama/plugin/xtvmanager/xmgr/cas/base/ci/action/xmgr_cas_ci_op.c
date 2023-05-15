/**
	@file     xmgr_cas_ci_op.c
	@brief    xmgr_cas_ci_op.c

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

#include <bus.h>
#include <sapi.h>
#include <svc_pipe.h>
#include <svc_cas.h>

#include <mgr_rscmap.h>

// #include <db_svc.h>
// #include <db_param.h>


#ifdef CONFIG_MW_CI
#include <ci.h>
#ifdef CONFIG_MW_CI_PLUS
#include <ci_plus.h>
#include <ci_ctrl.h>
#endif
#endif

#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_search.h>

#include "../local_include/_xmgr_cas_ci.h"
#include <mgr_cas.h>
#include <mgr_search.h>
#include <mgr_live.h>
#include <mgr_spec.h> /* fix for prevent #170104 */
#include <xmgr_spec.h>

#include <otl.h>
typedef struct
{
	HUINT32					ulActionId;
	Handle_t				hAction;
	HUINT16					usSlotId;				// 두개의 CICAM에서 올라오더라도 하나가 잡고 있으면 튕구자.
	DxTuneParam_t			stChTuningInfo;
	SvcCas_OperatorTune_t	stOpTune;

	HUINT32					ulTunerId;
	HUINT32					ulAntennaIndex;
	DbSvc_AntInfo_t			stAntInfo;

	HBOOL				bOnlyUpdate;
} XmgrCas_CiCamSearchContents_t;

/*
typedef struct
{
	DbSvc_NetInfo_t	stNetInfo;
	DbSvc_TsInfo_t	*pstTsInfo;
}
*/

typedef enum
{
	eCH_Tune_Locked,
	eCH_Tune_NotSupported,
	eCH_Tune_InvalidParam,
	eCH_Tune_NoSignal,
} XmgrCas_CiChTuneStatus_e;



/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC XmgrCas_CiCamSearchContents_t	s_stCiCamSearchContents;

/*******************************************************************/
/********************      API Functions   *************************/
/*******************************************************************/
STATIC HERROR xmgr_cas_CiOpTuneStatus(XmgrCas_CiCamSearchContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);

#define SEARCH_TIMER_LOCKING_ID						0x2001
#define SEARCH_TIMER_LOCKING_TIME					(10*1000)	/* 10 sec */

STATIC XmgrCas_CiCamSearchContents_t *xmgr_cas_CiOpGetContents(void)
{
	return &s_stCiCamSearchContents;
}

static HERROR xmgr_cas_CiOpGetTuneRequestData(CI_OP_REFRESH_REQUEST	eCiOpRefreshReq, HUINT16 usSlotId, HCHAR *pucData)
{
	// sub Data 구성
	SvcCas_CtrlParam_t				stParam;
	CI_GetOperatorNameInParam_t		stInParam;
	CI_GetOperatorNameOutParam_t	stOutParam;
	HUINT8							szOpName[MGR_CAS_STR_LENGTH_LONG];
	HCHAR							*pszCamName = NULL;
	HCHAR							*pzProperties = NULL;
	HINT32							slStream = 0;
	HERROR							hErr = ERR_OK;

	switch(eCiOpRefreshReq)
	{
		case CI_OP_REFRESH_REQUEST_TUNE:
		case CI_OP_REFRESH_REQUEST_FORCED_TUNE:
		{
			hErr = SVC_CAS_GetCamName(usSlotId, (HUINT8 **)&pszCamName);
			if(hErr != ERR_OK)
			{
				HxLOG_Error("[ERROR] Fail to Get CAM Name...\n");
				return ERR_FAIL;
			}

			stInParam.usSlotId		= usSlotId;
			stOutParam.pucOpName	= szOpName;
			stParam.pvIn			= &stInParam;
			stParam.pvOut			= &stOutParam;
			stParam.ulControlType	= eCACTRL_CI_GetOpName;

			if(SVC_CAS_CtrlGet(eDxCAS_GROUPID_CAM, eCACTRL_CI_GetOpName, &stParam) != ERR_OK)
			{
				HxLOG_Print("[%s:%d] SVC_CAS_CtrlGet - eCACTRL_CI_GetOpName error\n", __FUNCTION__, __HxLINE__);
				return ERR_FAIL;
			}

			slStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
			HLIB_RWSTREAM_Print(slStream, "{");
			HLIB_RWSTREAM_Print(slStream, "\"camname\":\"%s\"", pszCamName);
			HLIB_RWSTREAM_Print(slStream, ",\"opname\":\"%s\"", szOpName);
			HLIB_RWSTREAM_Print(slStream, "}");
			pzProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);

			HxSTD_MemCopy(pucData, pzProperties, MGR_CAS_STR_LENGTH_LONG);
			HLIB_STD_MemFree(pzProperties);

			break;
		}
		case CI_OP_REFRESH_REQUEST_RESERVED_TUNE:
			// TODO: Reserved Tune 기능이 필요할 경우 추가 하도록.
			break;
		default:
			HxLOG_Error("ERROR: Not defined Refresh Request ENUM!!\n");
			return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC void xmgr_cas_CiOpIncreaseSessionVersion (XmgrCas_CiCamSearchContents_t *pstContents)
{
	pstContents->hAction = SVC_PIPE_IncreaseActionVersion (pstContents->ulActionId);
}

STATIC void xmgr_cas_CiOpSearchDestroy(void)
{
	BUS_MGR_Destroy(NULL);
}

STATIC void xmgr_cas_CiOpSearchStatusChanged(XmgrCas_CiCamSearchContents_t *pstContents)
{
	BUS_PostMessage (NULL, eMEVT_SEARCH_STATUS_CHANGED, pstContents->hAction, 0, 0, 0);
}

STATIC void xmgr_cas_CiOpSearchFinished(XmgrCas_CiCamSearchContents_t *pstContents)
{
	// TODO:
/*	search가 완료되었음을 opl로 알려준다.(실패했든 성공했든 해당 event는 무조건 날라갈 것이다.)
	eMEVT_SEARCH_FINISHED event로 알려주며 이 때 opl에서는 buffer에 가지고 있는 search 정보를 db 파일로 저장한다.
	(DAMA 동작이 어떤식으로 동작이 되는지 되는지는 확인이 필요한 상황임)
	시나리오가 이후에 정해진 것으로 searched channel list number 등이 추가 인자로 전달되어야 하는 것으로 기억함.
	이후 필요에 따라서 CI stack 쪽으로 search가 완료되었음을 알릴 필요가 있다면 event 정의 후 알려준다.
*/
	BUS_PostMessage (NULL, eMEVT_SEARCH_FINISHED, pstContents->hAction, 0, 0, 0);
}

STATIC void xmgr_cas_CiOpActionRelease(XmgrCas_CiCamSearchContents_t *pstContents)
{
	MGR_ACTION_Release (pstContents->ulActionId, eActionType_SEARCH);
}

STATIC HERROR xmgr_cas_CiOpCheckDeliveryCapability(XmgrCas_CiCamSearchContents_t *pstContents)
{
	switch (pstContents->stOpTune.eDeliType)
	{
#if defined(CONFIG_MW_CH_SATELLITE)
		case eDxDELIVERY_TYPE_SAT:	break;
#endif
#if defined(CONFIG_MW_CH_TERRESTRIAL)
		case eDxDELIVERY_TYPE_TER:	break;
#endif
#if defined(CONFIG_MW_CH_CABLE)
		case eDxDELIVERY_TYPE_CAB:	break;
#endif
		default:
			return ERR_FAIL;
	}

	return ERR_OK;
}

static HERROR xmgr_cas_CiOpCheckTuningParameter(XmgrCas_CiCamSearchContents_t *pstContents)
{
	switch (pstContents->stOpTune.eDeliType)
	{
#if defined(CONFIG_MW_CH_SATELLITE)
		case eDxDELIVERY_TYPE_SAT:
			HxLOG_Print("ulFrequency (%d)\n", pstContents->stOpTune.stTuningDesInfo.sat.ulFrequency);
			if (pstContents->stOpTune.stTuningDesInfo.sat.ulFrequency > 0xFFFFF)
			{
				HxLOG_Print(">> INVALID PARAMETER !!!\n");
				return ERR_FAIL;
			}

			break;
#endif
#if defined(CONFIG_MW_CH_TERRESTRIAL)
		case eDxDELIVERY_TYPE_TER:
			break;
#endif
#if defined(CONFIG_MW_CH_CABLE)
		case eDxDELIVERY_TYPE_CAB:
			break;
#endif
		default:
			return ERR_FAIL;
	}

	return ERR_OK;
}


STATIC void xmgr_cas_CiOpConvertCasParamToSvcParam(DxDeliveryType_e eDeliType, SvcCas_TuningDesInfo_t *pstCasTuningDes, DbSvc_TuningInfo_t *pstSvcTuningInfo)
{
	switch (eDeliType)
	{
		case eDxDELIVERY_TYPE_SAT:
			pstSvcTuningInfo->sat.ulFrequency				= pstCasTuningDes->sat.ulFrequency;
			pstSvcTuningInfo->sat.ulSymbolRate				= pstCasTuningDes->sat.ulSymbolRate;
			pstSvcTuningInfo->sat.ePolarization				= pstCasTuningDes->sat.ePolarization;
			pstSvcTuningInfo->sat.eFecCodeRate				= pstCasTuningDes->sat.eFecCodeRate;
			pstSvcTuningInfo->sat.eTransSpec				= pstCasTuningDes->sat.eTransSpec;
			pstSvcTuningInfo->sat.ePskMod					= pstCasTuningDes->sat.ePskMod;
			pstSvcTuningInfo->sat.ePilot					= pstCasTuningDes->sat.ePilot;
			pstSvcTuningInfo->sat.eRollOff					= pstCasTuningDes->sat.eRollOff;
			break;

		case eDxDELIVERY_TYPE_TER:
			pstSvcTuningInfo->ter.ulFreq					= pstCasTuningDes->ter.ulFreq;
			pstSvcTuningInfo->ter.ucBandwidth				= pstCasTuningDes->ter.ucBandwidth;
 			pstSvcTuningInfo->ter.ucConstellation			= pstCasTuningDes->ter.ucConstellation;
			pstSvcTuningInfo->ter.ucHierachy				= pstCasTuningDes->ter.ucHierachy;
			pstSvcTuningInfo->ter.ucCodeRate				= pstCasTuningDes->ter.ucCodeRate;
			pstSvcTuningInfo->ter.ucTransMode				= pstCasTuningDes->ter.ucTransMode;
			pstSvcTuningInfo->ter.ucGuardInterval			= pstCasTuningDes->ter.ucGuardInterval;
			pstSvcTuningInfo->ter.ucStream					= pstCasTuningDes->ter.ucStream;
			pstSvcTuningInfo->ter.ucOffset					= pstCasTuningDes->ter.ucOffset;
		#if defined(CONFIG_MW_CH_DVB_T2)
			pstSvcTuningInfo->ter.ucSystem					= pstCasTuningDes->ter.ucSystem;
			pstSvcTuningInfo->ter.stT2param.ulPlpId			= pstCasTuningDes->ter.stT2param.ulPlpId;
			pstSvcTuningInfo->ter.stT2param.ucPreambleFormat= pstCasTuningDes->ter.stT2param.ucPreambleFormat;
			pstSvcTuningInfo->ter.stT2param.ucMixed			= pstCasTuningDes->ter.stT2param.ucMixed;
			pstSvcTuningInfo->ter.stT2param.ucPilotPattern	= pstCasTuningDes->ter.stT2param.ucPilotPattern;
			pstSvcTuningInfo->ter.stT2param.ucExtenedCarrier= pstCasTuningDes->ter.stT2param.ucExtenedCarrier;
			pstSvcTuningInfo->ter.stT2param.ucPAPRreduction	= pstCasTuningDes->ter.stT2param.ucPAPRreduction;
			pstSvcTuningInfo->ter.stT2param.ulNumPlpBlocks	= pstCasTuningDes->ter.stT2param.ulNumPlpBlocks;
		#endif
			break;

		case eDxDELIVERY_TYPE_CAB:
			pstSvcTuningInfo->cab.ulFreq					= pstCasTuningDes->cab.ulFreq;
			pstSvcTuningInfo->cab.usSR						= pstCasTuningDes->cab.usSR;
			pstSvcTuningInfo->cab.ucCon						= pstCasTuningDes->cab.ucCon;
			pstSvcTuningInfo->cab.ucSpec					= pstCasTuningDes->cab.ucSpec;
			pstSvcTuningInfo->cab.usNetworkId				= pstCasTuningDes->cab.usNetworkId;
			break;

		default:
			break;
	}
}

STATIC HERROR xmgr_cas_CiOpRetrieveResults (XmgrCas_CiCamSearchContents_t *pstContents)
{
	Handle_t					hSvc, *phSvcList;
	DbSvc_TsInfo_t				stTsInfo, *pstTsInfoList;
	DbSvc_NetInfo_t 			stNetInfo;
	DbSvc_Info_t				stSvcInfo, stDbSvcInfo;
	DbSvc_TsCondition_t 		stTsCond;
	DbSvc_Condition_t			stSvcCond;

	HUINT16 					usCiNetNum, usCiTsNum, usCiSvcNum, usDbNetIdx, usDbTsIdx;
	HINT32						nCiNetIdx, nCiTsIdx, nCiSvcIdx, nDbTsIdx, nDbTsNum, nDbSvcIdx, nDbSvcNum;

	HUINT8						szOpName[MAX_CAM_OPERATOR_NAME_LEN];
	HBOOL						bNewTs, bNewService, bDeleteProfileSvc = FALSE;

	SvcCas_CtrlParam_t			stParam;
	CI_GetOperatorInParam_t 	stInParam;
	CI_GetOperatorNameOutParam_t	stOutParam;
	CI_GetBindedCamIdInParam_t		stCamIdInParam;
	CI_GetBindedCamIdOutParam_t 	stCamIdOutParam;
	HUINT8							aucBindedCamId[8];
	SvcCas_OperatorNit_t		stOpNit;
	SvcCas_OperatorNitTs_t		stOpNitTsLoop;
	SvcCas_OperatorSvc_t		*pstSvcArray = NULL;

	HERROR						hErr;

	HxLOG_Print("+[%s]...\n", __FUNCTION__);

	stCamIdInParam.usSlotId = pstContents->usSlotId;
	stCamIdOutParam.pucBindedCamId = aucBindedCamId;
	stParam.pvIn = (void *)&stCamIdInParam;
	stParam.pvOut = (void *)&stCamIdOutParam;
	stParam.ulControlType = eCACTRL_CI_GetBindedCamId;
	hErr = SVC_CAS_CtrlGet(eDxCAS_GROUPID_CAM, eCACTRL_CI_GetBindedCamId, &stParam);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("SVC_CAS_CtrlGet() ERROR!\n");
		return hErr;
	}

	stInParam.usSlotId	= pstContents->usSlotId;
	stParam.pvIn		= (void *)&stInParam;
	stParam.pvOut		= (void *)&usCiNetNum;

	if(SVC_CAS_CtrlGet(eDxCAS_GROUPID_CAM, eCACTRL_CI_GetOpNetNum, &stParam) != ERR_OK)
	{
		HxLOG_Error("SVC_CAS_CtrlGet - eCACTRL_CI_GetOpNetNum error\n");
		usCiNetNum = 0;
	}

	if(usCiNetNum == 0)
	{
		HxLOG_Error("error\n");
		return ERR_FAIL;
	}

	stInParam.usSlotId		= pstContents->usSlotId;
	stOutParam.pucOpName	= szOpName;
	stParam.pvIn		= (void *)&stInParam;
	stParam.pvOut		= (void *)&stOutParam;

	if(SVC_CAS_CtrlGet(eDxCAS_GROUPID_CAM, eCACTRL_CI_GetOpName, &stParam) != ERR_OK)
	{
		HxLOG_Error("SVC_CAS_CtrlGet - eCACTRL_CI_GetOpNetInfo error\n");
		return ERR_FAIL;
	}

	for(nCiNetIdx = 0 ; nCiNetIdx < usCiNetNum ; nCiNetIdx++)
	{
		stInParam.usNetIdx	= nCiNetIdx;
		stParam.pvIn		= (void *)&stInParam;
		stParam.pvOut		= (void *)&stOpNit;

		if(SVC_CAS_CtrlGet(eDxCAS_GROUPID_CAM, eCACTRL_CI_GetOpNetInfo, &stParam) != ERR_OK)
		{
			HxLOG_Print("[%s:%d] SVC_CAS_CtrlGet - eCACTRL_CI_GetOpNetInfo error\n", __FUNCTION__, __HxLINE__);
			continue;
		}

		stNetInfo.eOrgDeliType	= eDxDELIVERY_TYPE_SAT;
		stNetInfo.usNetId		= stOpNit.usNetId;
		HxSTD_memcpy(stNetInfo.szNetName, stOpNit.szNetName, DxMAX_NET_NAME_LEN);

		if(DB_SVC_FindNetworkIndexById(stNetInfo.eOrgDeliType, stNetInfo.usNetId, &usDbNetIdx) != ERR_OK)
		{
			HxLOG_Print("[%s:%d] DB_SVC_FindNetworkIndexById() error\n", __FUNCTION__, __HxLINE__);

			if(DB_SVC_AddNetworkInfo (&stNetInfo, &usDbNetIdx) != ERR_OK)
				HxLOG_Error("DB_SVC_AddNetworkInfo() error\n");
		}

		stInParam.usNetIdx	= nCiNetIdx;
		stParam.pvIn		= (void *)&stInParam;
		stParam.pvOut		= (void *)&usCiTsNum;

		if(SVC_CAS_CtrlGet(eDxCAS_GROUPID_CAM, eCACTRL_CI_GetOpTsNum, &stParam) != ERR_OK)
		{
			HxLOG_Print("[%s:%d] SVC_CAS_CtrlGet - eCACTRL_CI_GetOpTsNum error\n", __FUNCTION__, __HxLINE__, nCiNetIdx);
			usCiTsNum = 0;
		}

		if(usCiTsNum == 0)
		{
			HxLOG_Warning("usCiTsNum == 0\n");
			continue;
		}

		for(nCiTsIdx = 0 ; nCiTsIdx < usCiTsNum ; nCiTsIdx++)
		{
			stInParam.usNetIdx	= nCiNetIdx;
			stInParam.usTsIdx	= nCiTsIdx;
			stParam.pvIn		= (void *)&stInParam;
			stParam.pvOut		= (void *)&stOpNitTsLoop;

			if(SVC_CAS_CtrlGet(eDxCAS_GROUPID_CAM, eCACTRL_CI_GetOpTsInfo, &stParam) != ERR_OK)
			{
				HxLOG_Error("SVC_CAS_CtrlGet - eCACTRL_CI_GetOpTsInfo error\n");
				continue;
			}

			stTsInfo.usNetIdx					= usDbNetIdx;
			stTsInfo.eDeliType					= stOpNitTsLoop.eDeliType;
			stTsInfo.usTsId 					= stOpNitTsLoop.usTsId;
			stTsInfo.usOnId 					= stOpNitTsLoop.usOnId;
			stTsInfo.ucTunerId					= pstContents->ulTunerId;

			xmgr_cas_CiOpConvertCasParamToSvcParam(stTsInfo.eDeliType, &stOpNitTsLoop.stTuningDes, &stTsInfo.stTuningParam);

			if(stOpNitTsLoop.eDeliType == eDxDELIVERY_TYPE_SAT)
			{
				HUINT32 ulLnbFreq = 0;

				switch(pstContents->stAntInfo.eAntennaType)
				{
					case eDxANT_TYPE_LNB_ONLY:	ulLnbFreq = pstContents->stAntInfo.utAntInfo.stLnbAnt.ulLnbFreq;		break;
					case eDxANT_TYPE_DISEQC:		ulLnbFreq = pstContents->stAntInfo.utAntInfo.stDiseqcAnt.ulLnbFreq; break;
					case eDxANT_TYPE_SCD:			ulLnbFreq = pstContents->stAntInfo.utAntInfo.stScdAnt.ulLnbFreq;		break;
					default:	break;
				}

				stTsInfo.stTuningParam.sat.ulFrequency		= stOpNitTsLoop.stTuningDes.sat.ulFrequency;
				stTsInfo.stTuningParam.sat.ucAntId			= pstContents->ulAntennaIndex;
				stTsInfo.stTuningParam.sat.eSatType 		= pstContents->stAntInfo.eSatType;
			}


			// TS Info가 OnId, TsId로 등록되어 있는 지 확인한다.
			DB_SVC_InitTsFindCondition (&stTsCond);
			stTsCond.nDeliType	= stOpNitTsLoop.eDeliType;
			stTsCond.nOnId		= (HINT32)stTsInfo.usOnId;
			stTsCond.nTsId		= (HINT32)stTsInfo.usTsId;

			pstTsInfoList		= NULL;
			bNewTs				= TRUE;

			hErr = DB_SVC_FindTsInfoList (&stTsCond, &nDbTsNum, &pstTsInfoList);
			if(hErr == ERR_OK && nDbTsNum > 0)
			{
				if(pstTsInfoList != NULL)
				{
					if(stOpNitTsLoop.eDeliType == eDxDELIVERY_TYPE_SAT)
					{
						for (nDbTsIdx = 0; nDbTsIdx < nDbTsNum; nDbTsIdx++)
						{
							if (pstTsInfoList[nDbTsIdx].stTuningParam.sat.ucAntId == pstContents->ulAntennaIndex)
							{
								bNewTs = FALSE;
								usDbTsIdx = pstTsInfoList[nDbTsIdx].usUniqueId;

								if (DB_SVC_UtilCompareDbTuningParam (stOpNitTsLoop.eDeliType, &stTsInfo.stTuningParam, &pstTsInfoList[nDbTsIdx].stTuningParam) != 0)
								{
									HxLOG_Print("[%s:%d] DB_SVC_UtilCompareDbTuningParam()...\n", __FUNCTION__, __HxLINE__);
								}
								break;
							}
						}
					}
					else
					{
						bNewTs = FALSE;
						usDbTsIdx = pstTsInfoList[0].usUniqueId;

						if (DB_SVC_UtilCompareDbTuningParam (stOpNitTsLoop.eDeliType, &stTsInfo.stTuningParam, &pstTsInfoList[0].stTuningParam) != 0)
						{
							HxLOG_Print("[%s:%d] DB_SVC_UtilCompareDbTuningParam()...\n", __FUNCTION__, __HxLINE__);
						}
					}

				}
			}

			if(pstTsInfoList != NULL)			DB_SVC_FreeTsInfoList (pstTsInfoList);

			if (bNewTs == TRUE)
			{
				DB_SVC_AddTsInfo (&stTsInfo, &usDbTsIdx);
			}

			stInParam.usNetIdx	= nCiNetIdx;
			stInParam.usTsIdx	= nCiTsIdx;
			stParam.pvIn		= (void *)&stInParam;
			stParam.pvOut		= (void *)&usCiSvcNum;

			if(SVC_CAS_CtrlGet(eDxCAS_GROUPID_CAM, eCACTRL_CI_GetOpSvcNum, &stParam) != ERR_OK)
			{
				HxLOG_Error("SVC_CAS_CtrlGet - eCACTRL_CI_GetOpSvcNum error\n");
				usCiSvcNum = 0;
			}

			if (usCiSvcNum == 0)
			{
				HxLOG_Warning("usCiSvcNum == 0\n");
				continue;
			}

			if(pstSvcArray)
			{
				OxAP_Free(pstSvcArray);
				pstSvcArray = NULL;
			}

			pstSvcArray = (SvcCas_OperatorSvc_t *)OxAP_Malloc (usCiSvcNum * sizeof(SvcCas_OperatorSvc_t));
			if (pstSvcArray == NULL)
			{
				HxLOG_Error("error\n");
				continue;
			}

			HxSTD_memset(pstSvcArray, 0, usCiSvcNum * sizeof(SvcCas_OperatorSvc_t));

			stInParam.usNetIdx	= nCiNetIdx;
			stInParam.usTsIdx	= nCiTsIdx;
			stParam.pvIn		= (void *)&stInParam;
			stParam.pvOut		= (void *)pstSvcArray;

			if(SVC_CAS_CtrlGet(eDxCAS_GROUPID_CAM, eCACTRL_CI_GetOpSvcInfo, &stParam) != ERR_OK)
			{
				HxLOG_Error("error\n");
				continue;
			}

			// 이전에 저장되어 있는 operator profile service를 삭제 (추후 nCiNetIdx loop 이전 처리하도록 개선 필요)
			if (bDeleteProfileSvc == FALSE)
			{
				DB_SVC_InitSvcFindCondition(&stSvcCond);

				stSvcCond.nTsIdx		= (HINT32)usDbTsIdx;
				stSvcCond.nOnId 		= (HINT32)stTsInfo.usOnId;
				stSvcCond.nTsId 		= (HINT32)stTsInfo.usTsId;

				HxLOG_Error("pre db svc info:[%d, 0x%x, 0x%x]\n", usDbTsIdx, stTsInfo.usOnId, stTsInfo.usTsId);

				hErr = DB_SVC_FindSvcHandleList(&stSvcCond, &nDbSvcNum, &phSvcList);
				if(hErr == ERR_OK && nDbSvcNum > 0)
				{
					HxLOG_Info("nDbSvcNum:[%d]\n", nDbSvcNum);
					if(phSvcList != NULL)
					{
						for(nDbSvcIdx = 0; nDbSvcIdx < nDbSvcNum; nDbSvcIdx++)
						{
							if(DB_SVC_GetServiceInfo(phSvcList[nDbSvcIdx], &stDbSvcInfo) == ERR_OK)
							{
								if(HxSTD_memcmp(aucBindedCamId, DbSvc_GetCamId(&stDbSvcInfo), 8/*CI_PLUS_CICAM_ID_LEN*/) == 0)
								{
									DB_SVC_DeleteServiceInfo(phSvcList[nDbSvcIdx]);
								}
							}
						}

						bDeleteProfileSvc = TRUE;
					}
				}

				if(phSvcList != NULL)
				{
					DB_SVC_FreeServiceInfoList (NULL, phSvcList, NULL);
					phSvcList = NULL;
				}
			}

			// 6-5. Service Info
			for (nCiSvcIdx = 0 ; nCiSvcIdx < usCiSvcNum ; nCiSvcIdx++)
			{
				DxOperator_e	eSvcOpType = 0;

				HxSTD_memset(&stSvcInfo, 0, sizeof(DbSvc_Info_t));

				DbSvc_SetOrgSvcType(&stSvcInfo, eDxSVC_TYPE_TV);
				MGR_SPEC_GetOperatorCode(&eSvcOpType);
				DbSvc_SetOpType(&stSvcInfo, eSvcOpType);
//				stSvcInfo.eSatType				= eSatType_ASTRA_1;
				DbSvc_SetCasType(&stSvcInfo, eDxCAS_TYPE_CIPLUS);
				DbSvc_SetDeliType(stSvcInfo, stNetInfo.eOrgDeliType);
				DbSvc_SetTsId(&stSvcInfo, stTsInfo.usTsId);
				DbSvc_SetOnId(&stSvcInfo, stTsInfo.usOnId);
				DbSvc_SetTsIdx(&stSvcInfo, usDbTsIdx);
				DbSvc_SetPrvIdx(&stSvcInfo, 0);
				DbSvc_SetSvcId(&stSvcInfo, pstSvcArray[nCiSvcIdx].usSvcId);
				DbSvc_SetSvcType(&stSvcInfo, pstSvcArray[nCiSvcIdx].ucSvcType);
				DbSvc_SetVisibleFlag(&stSvcInfo, pstSvcArray[nCiSvcIdx].bVisible);
				DbSvc_SetSelectablSvcFlag(&stSvcInfo, pstSvcArray[nCiSvcIdx].bSelectable);
				DbSvc_SetLcn(&stSvcInfo, pstSvcArray[nCiSvcIdx].usLcn);
				DbSvc_SetLcnFlag(&stSvcInfo, TRUE);
				DbSvc_SetName(&stSvcInfo, pstSvcArray[nCiSvcIdx].szSvcName);
				DbSvc_SetCamOperatorName(&stSvcInfo, szOpName);
				DbSvc_SetCamId(&stSvcInfo, aucBindedCamId);
				if (DbSvc_GetDeliType(stSvcInfo) == eDxDELIVERY_TYPE_SAT)
					DbSvc_SetAntIdx(&stSvcInfo, pstContents->ulAntennaIndex);

				DB_SVC_InitSvcFindCondition(&stSvcCond);

				stSvcCond.nCasType		= (HINT32)eDxCAS_TYPE_CIPLUS;
				stSvcCond.nTsIdx		= (HINT32)DbSvc_GetTsIdx(&stSvcInfo);
				stSvcCond.nOnId 		= (HINT32)DbSvc_GetOnId(&stSvcInfo);
				stSvcCond.nTsId 		= (HINT32)DbSvc_GetTsId(&stSvcInfo);
				stSvcCond.nSvcId		= (HINT32)DbSvc_GetSvcId(&stSvcInfo);
				stSvcCond.nOrgDeliType	= (HINT32)DbSvc_GetDeliType(stSvcInfo);
				if (DbSvc_GetDeliType(stSvcInfo) == eDxDELIVERY_TYPE_SAT)
					stSvcCond.nAntIdx	= (HINT32)DbSvc_GetAntIdx(&stSvcInfo);

				phSvcList	= NULL;
				bNewService = TRUE;

				hErr = DB_SVC_FindSvcHandleList(&stSvcCond, &nDbSvcNum, &phSvcList);
				if(hErr == ERR_OK && nDbSvcNum > 0)
				{
					if(phSvcList != NULL)
					{
						for(nDbSvcIdx = 0; nDbSvcIdx < nDbSvcNum; nDbSvcIdx++)
						{
							if(DB_SVC_GetServiceInfo(phSvcList[nDbSvcIdx], &stDbSvcInfo) == ERR_OK)
							{
								if((HxSTD_memcmp(aucBindedCamId, DbSvc_GetCamId(&stDbSvcInfo), CICAM_ID_LEN) == 0) && (DbSvc_GetLcn(&stSvcInfo) == DbSvc_GetLcn(&stDbSvcInfo)))
								{
									DB_SVC_DeleteServiceInfo(phSvcList[nDbSvcIdx]);
								}
							}
						}
					}
				}

				if(phSvcList != NULL)
				{
					DB_SVC_FreeServiceInfoList (NULL, phSvcList, NULL);
					phSvcList = NULL;
				}

				DB_SVC_AddServiceInfo(&stSvcInfo, &hSvc);
				DbSvc_SetSvcIdx(&stSvcInfo, DB_SVC_GetServiceUID(hSvc));
				MGR_SEARCHRESULT_AddService(pstContents->ulActionId, &stSvcInfo);
			}

			(void)MGR_SEARCHRESULT_AddOnTsId(pstContents->ulActionId, stTsInfo.usOnId, stTsInfo.usTsId);
		}
	}

	DB_SVC_SyncHash();

	xmgr_cas_CiOpSearchStatusChanged(pstContents);

	if (pstSvcArray != NULL)
		OxAP_Free (pstSvcArray);

	HxLOG_Info("-[%s]...\n", __FUNCTION__);

	return ERR_OK;
}



STATIC HERROR xmgr_cas_CiOpSetTuningParamToSearch (XmgrCas_CiCamSearchContents_t *pstContents)
{
	DbSvc_TsInfo_t		stSvcTsInfo;

	HxLOG_Print ("+[%s]...\n", __FUNCTION__);

	HxSTD_memset(&stSvcTsInfo, 0, sizeof(DbSvc_TsInfo_t));

	stSvcTsInfo.ucTunerId = pstContents->ulTunerId;
	stSvcTsInfo.eDeliType = pstContents->stOpTune.eDeliType;

	xmgr_cas_CiOpConvertCasParamToSvcParam(stSvcTsInfo.eDeliType, &pstContents->stOpTune.stTuningDesInfo, &stSvcTsInfo.stTuningParam);

	if(stSvcTsInfo.eDeliType == eDxDELIVERY_TYPE_SAT)
	{
		HUINT32 ulLnbFreq = 0;

		switch(pstContents->stAntInfo.eAntennaType)
		{
			case eDxANT_TYPE_LNB_ONLY:	ulLnbFreq = pstContents->stAntInfo.utAntInfo.stLnbAnt.ulLnbFreq;		break;
			case eDxANT_TYPE_DISEQC:		ulLnbFreq = pstContents->stAntInfo.utAntInfo.stDiseqcAnt.ulLnbFreq;	break;
			case eDxANT_TYPE_SCD:			ulLnbFreq = pstContents->stAntInfo.utAntInfo.stScdAnt.ulLnbFreq;		break;
			default:	break;
		}

		stSvcTsInfo.stTuningParam.sat.ucAntId		= pstContents->ulAntennaIndex;
		stSvcTsInfo.stTuningParam.sat.ulFrequency	= pstContents->stOpTune.stTuningDesInfo.sat.ulFrequency;

		/* Antenna Info 변경 */
		MGR_SEARCHRESULT_SetAntennaInfo (pstContents->ulActionId, pstContents->stAntInfo);
		MGR_SEARCHRESULT_SetAntennaId(pstContents->ulActionId, pstContents->ulAntennaIndex);
	}

	OTL_CONV_DbTuningInfo2DxTuneParam((const DbSvc_TsInfo_t *)&stSvcTsInfo, &pstContents->stChTuningInfo);

	HxLOG_Print ("-[%s]...\n", __FUNCTION__);

	return ERR_OK;
}

STATIC HERROR xmgr_cas_CiOpTuneSearchTp (XmgrCas_CiCamSearchContents_t *pstContents)
{
	SvcRsc_Info_t			 stRscInfo;
	HERROR				 hErr;

	HxLOG_Info("+[%s]...\n", __FUNCTION__);

	MGR_RSC_GetResourceInfo (pstContents->ulActionId, &stRscInfo);

	hErr = xmgr_cas_CiOpSetTuningParamToSearch (pstContents);
	if (hErr != ERR_OK)
	{
		HxLOG_Print ("[%s] No Tp left to tune\n", __FUNCTION__);
		return ERR_FAIL;
	}

	hErr = SVC_CH_StopTune (pstContents->hAction, (HUINT16)-1, eSyncMode);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("SVC_CH_StopTune() error...\n");
	}

	xmgr_cas_CiOpIncreaseSessionVersion (pstContents);	// Tuning 전에 반드시 Action Version을 업데이트해야 한다.
	hErr = SVC_CH_StartTune(pstContents->hAction, &stRscInfo, (HUINT16)-1, &pstContents->stChTuningInfo);
	if (hErr == ERR_OK)
	{
		// DD에서 Tuning Signal이 올라오지 않아 무한으로 기다리는 것을 막기 위해 여기서 timer를 처리하도록 한다.
		BUS_SetTimer (SEARCH_TIMER_LOCKING_ID, SEARCH_TIMER_LOCKING_TIME);
		return ERR_OK;
	}

	HxLOG_Info("-[%s]...\n", __FUNCTION__);

	return ERR_FAIL;
}

STATIC HERROR xmgr_cas_CiOpSearchPrepareSearchAction (XmgrCas_CiCamSearchContents_t *pstContents)
{
	HERROR hErr;

	HxLOG_Info("+[%s]...\n", __FUNCTION__);

	MGR_SEARCH_InitResult(pstContents->ulActionId);

	hErr = MGR_ACTION_SetupSearch (pstContents->ulActionId, &pstContents->stChTuningInfo, eAmPriority_SEARCH);
	if (hErr != ERR_OK)
	{
		HxLOG_Error ("Error Log !!\n");
		return ERR_FAIL;
	}

	hErr = MGR_ACTION_Take (pstContents->ulActionId, eActionType_SEARCH, BUS_MGR_GetMgrCallback(BUS_MGR_GetThis()));
	if (hErr != ERR_OK)
	{
		HxLOG_Error ("Error Log !!\n");
		return ERR_FAIL;
	}

	HxLOG_Info("-[%s]...\n", __FUNCTION__);

	return ERR_OK;
}

STATIC HERROR xmgr_cas_CiOpSearchMsgGwmCreate (XmgrCas_CiCamSearchContents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	MgrSearch_Option_t	*pstOption;
	HERROR				hErr = ERR_FAIL; /* fix for prevent #150646 */

	HxLOG_Info("+[%s]...\n", __FUNCTION__);

	if((HBOOL)p3 == TRUE)
	{
		DbSvc_TsInfo_t		 stSvcTsInfo;

		pstContents->bOnlyUpdate = TRUE;
		pstContents->usSlotId = (HUINT16)p1;

		pstContents->hAction	= hAction;
		pstContents->ulActionId = (hAction == HANDLE_NULL ? MGR_ACTION_GetMainActionId() : SVC_PIPE_GetActionId(hAction));

		hErr = MGR_LIVE_GetTsInfo(pstContents->ulActionId, &stSvcTsInfo);
		if(ERR_OK == hErr)
		{
	#if defined(CONFIG_MW_CH_SATELLITE)
			pstContents->ulAntennaIndex = stSvcTsInfo.stTuningParam.sat.ucAntId;
			hErr = DB_SVC_GetAntInfo(pstContents->ulAntennaIndex, &pstContents->stAntInfo);
			if(ERR_OK != hErr)
			{
				HxLOG_Error("DB_SVC_GetAntInfo() error\n");
			}
	#endif
		}
		return ERR_OK;
	}

	pstOption = (MgrSearch_Option_t *)p1;
	if(pstOption == NULL)
	{
		HxLOG_Error("pstOption == NULL!!\n");
		return ERR_FAIL;
	}

	pstContents->hAction	= hAction;
	pstContents->ulActionId = (hAction == HANDLE_NULL ? MGR_ACTION_GetMainActionId() : SVC_PIPE_GetActionId(hAction));

	if(pstOption->pstTpArray != NULL && pstOption->ulTpNum > 0)
	{
		DbSvc_TsInfo_t		 stSvcTsInfo;

		HxSTD_memset(&stSvcTsInfo, 0x00, sizeof(DbSvc_TsInfo_t));

		// current Tp로 튜닝info 채워 준다.
		stSvcTsInfo.eDeliType = eDxDELIVERY_TYPE_SAT;
		stSvcTsInfo.ucTunerId = (HUINT8)pstContents->ulTunerId;
		HxSTD_memcpy (&(stSvcTsInfo.stTuningParam), &pstOption->pstTpArray[0].stTuningParam, sizeof(DbSvc_TuningInfo_t));

		OTL_CONV_DbTuningInfo2DxTuneParam (&stSvcTsInfo, &pstContents->stChTuningInfo);

#if defined(CONFIG_MW_CH_SATELLITE)
		pstContents->ulAntennaIndex = pstOption->pstTpArray[0].stTuningParam.sat.ucAntId;
		hErr = DB_SVC_GetAntInfo(pstContents->ulAntennaIndex, &pstContents->stAntInfo);
		if(ERR_OK != hErr)
		{
			HxLOG_Error("DB_SVC_GetAntInfo() error\n");
		}
#endif
	}

	HxLOG_Info("-[%s]...\n", __FUNCTION__);

	NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);
	return ERR_OK;
}

// eMEVT_BUS_DESTROY:
STATIC HERROR xmgr_cas_CiOpSearchMsgGwmDestroyed (XmgrCas_CiCamSearchContents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);
	return ERR_OK;
}

STATIC HERROR xmgr_cas_CiOpChTuneStatus (XmgrCas_CiCamSearchContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxLOG_Info("+[%s]...\n", __FUNCTION__);

	BUS_KillTimer (SEARCH_TIMER_LOCKING_ID);

	// no signal 인 경우는 descriptor loop 가 empty 이어야 한다.
	if (p3 == (XmgrCas_CiChTuneStatus_e)eCH_Tune_NoSignal)
	{
		HxLOG_Info("[%s] No Signal ->> OpTune is set to empty...\n", __FUNCTION__);
		pstContents->stOpTune.pucDes = NULL;
		pstContents->stOpTune.ucDesLen = 0;
	}

	if(xmgr_cas_CiOpTuneStatus(pstContents, p1, p2, p3) != ERR_OK)
	{
		HxLOG_Error("[%s] - xmgr_cas_CiOpTuneStatus() error\n", __FUNCTION__);
	}

	HxLOG_Info("-[%s]...\n", __FUNCTION__);
	return ERR_OK;
}

STATIC HERROR xmgr_cas_CiOpSearchMsgTimer (XmgrCas_CiCamSearchContents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32 nTimerId = p1;

	switch(nTimerId)
	{
		case SEARCH_TIMER_LOCKING_ID:
			HxLOG_Info("[%s] - xmgr_cas_CiOpSearchMsgTimer : SEARCH_TIMER_LOCKING_ID\n", __FUNCTION__);
			if(xmgr_cas_CiOpChTuneStatus(pstContents, 0, 0, eCH_Tune_NoSignal) != ERR_OK)
			{
				HxLOG_Error("[%s] - xmgr_cas_CiOpChTuneStatus() error\n", __FUNCTION__);
			}
			return MESSAGE_BREAK;
	}

	return MESSAGE_PASS;
}

STATIC HERROR xmgr_cas_CiOpSearchEvtChLocked (XmgrCas_CiCamSearchContents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	DxDeliveryType_e	eDelivery;
	CH_SignalInfo_t		stSignalInfo;

	HxLOG_Info("+[%s]...\n", __FUNCTION__);

	HxSTD_memset(&stSignalInfo, 0, sizeof(CH_SignalInfo_t));

	if(SVC_CH_GetSignalInfo(pstContents->ulActionId, &eDelivery, &stSignalInfo) != ERR_OK)
	{
		HxLOG_Error("[%s] - SVC_CH_GetSignalInfo() error\n", __FUNCTION__);
	}

	if(xmgr_cas_CiOpChTuneStatus(pstContents, stSignalInfo.signalLevel, stSignalInfo.signalQuality, eCH_Tune_Locked) != ERR_OK)
	{
		HxLOG_Error("[%s] - xmgr_cas_CiOpChTuneStatus() error\n", __FUNCTION__);
	}

	HxLOG_Info("-[%s]...\n", __FUNCTION__);

	return ERR_OK;
}

STATIC HERROR xmgr_cas_CiOpSearchEvtChNoSignal (XmgrCas_CiCamSearchContents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxLOG_Info("+[%s]...\n", __FUNCTION__);

	if(xmgr_cas_CiOpChTuneStatus(pstContents, 0, 0, eCH_Tune_NoSignal) != ERR_OK)
	{
		HxLOG_Error("[%s] - xmgr_cas_CiOpChTuneStatus() error\n", __FUNCTION__);
	}

	HxLOG_Info("-[%s]...\n", __FUNCTION__);

	return ERR_OK;
}

STATIC HERROR xmgr_cas_CiOpOperatorPrepareReq(XmgrCas_CiCamSearchContents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32				eCasEvent = 0;
	CI_OP_REFRESH_REQUEST	eCiOpRefreshReq = (CI_OP_REFRESH_REQUEST)p3;
	HCHAR					*pucSubData = NULL;
#ifdef CONFIG_PROD_PROFILE_OCTO2
	BUS_Result_t			hBusResult = ERR_BUS_NO_ERROR;
	HUINT16 				usSlotIdForWeb = 0;
#endif

	HxLOG_Info("+[%s]...\n", __FUNCTION__);

	pstContents->usSlotId = (HUINT16)p1;

	switch(eCiOpRefreshReq)
	{
		case CI_OP_REFRESH_REQUEST_TUNE:
			eCasEvent = eMEVT_CAS_ACTION_TUNE_REQUEST;
			break;
		case CI_OP_REFRESH_REQUEST_FORCED_TUNE:
			eCasEvent = eMEVT_CAS_ACTION_FORCED_TUNE;
			break;
		case CI_OP_REFRESH_REQUEST_RESERVED_TUNE:
#if defined(CONFIG_MW_CI_PLUS_V_1_3)
		{
			HUINT32 				ulRsvSlot;
			DxSchedule_t			stRsvItem;
			HERROR					hErr;
			MgrAction_State_e		eActionState;

			eActionState = MGR_ACTION_GetState();

			if(eActionState != eAmState_NORMAL)
				return ERR_OK;

			hErr = SAPI_GetAvailableSlot(&ulRsvSlot);
			if (hErr == ERR_OK)
			{
				SvcCas_CtrlParam_t				stParam;
				CI_GetOperatorInParam_t 		stOpInParam;
				CI_GetBindedCamIdInParam_t		stCamIdInParam;
				CI_GetBindedCamIdOutParam_t 	stCamIdOutParam;
				UNIXTIME						utTime;
				HUINT8							aucBindedCamId[8];

				stOpInParam.usSlotId	= pstContents->usSlotId;
				stParam.pvIn			= (void *)&stOpInParam;
				stParam.pvOut			= (void *)&utTime;

				stParam.ulControlType	= eCACTRL_CI_GetOpRsvTime;

				if(SVC_CAS_CtrlGet(eDxCAS_GROUPID_CAM, eCACTRL_CI_GetOpRsvTime, &stParam) != ERR_OK)
				{
					HxLOG_Print("SVC_CAS_CtrlGet - eCACTRL_CI_GetOpRsvTime error\n");
					return ERR_FAIL;
				}

				stCamIdInParam.usSlotId = pstContents->usSlotId;
				stCamIdOutParam.pucBindedCamId = aucBindedCamId;
				stParam.pvIn = (void *)&stCamIdInParam;
				stParam.pvOut = (void *)&stCamIdOutParam;
				stParam.ulControlType = eCACTRL_CI_GetBindedCamId;
				hErr = SVC_CAS_CtrlGet(eDxCAS_GROUPID_CAM, eCACTRL_CI_GetBindedCamId, &stParam);
				if (hErr != ERR_OK)
				{
					HxLOG_Error("\nSVC_CAS_CtrlGet() ERROR!\n");
					return hErr;
				}

				HxSTD_MemCopy(stRsvItem.aucCamId, aucBindedCamId, 8);
				stRsvItem.eRsvType 		= DxRSVTYPE_CIPLUS_OP_TUNE;
				stRsvItem.eRsvReady 	= DxRSVREADY_0_SEC;
				HLIB_DATETIME_ConvertUnixTimeToDateTime(utTime, &stRsvItem.stStartTime);
				stRsvItem.ulDuration 	= 600;
				stRsvItem.eRepeat 		= DxRSVREPEAT_ONCE;

				hErr = SAPI_MakeSchedule (ulRsvSlot, &stRsvItem, NULL);
			}
			return ERR_OK;
			break;
		}
#endif
		default:
			HxLOG_Error("ERROR: Not defined Refresh Request ENUM!!\n");
			break;
	}

	pucSubData = HLIB_STD_MemCalloc(MGR_CAS_STR_LENGTH_LONG);
	if(xmgr_cas_CiOpGetTuneRequestData(eCiOpRefreshReq, pstContents->usSlotId, pucSubData) != ERR_OK)
	{
		HxLOG_Critical("CR ERROR from local_cas_ciop_GetCiPlusTuneRequestData()...\n");
		HLIB_STD_MemFree(pucSubData);
		return ERR_FAIL;
	}

#ifdef CONFIG_PROD_PROFILE_OCTO2
#if CONFIG_MW_CI_NUM_SLOT == 2
	// 2Slot 모델인 경우 DI에서부터 올려주고 관리하는 slot ID는 아래 슬롯은 0, 위 슬롯은 1번으로 각각 관리 되는데,
	// Web쪽에서는 위 slot을 1, 아래 slot을 2번으로 관리하여 UI로 뿌려준다. 따라서 Web에 UI를 그리기 위해서 올려 줄 때에 변경하여 올려줘야 한다.
	if(pstContents->usSlotId == 0)
		usSlotIdForWeb = 2;
	else if(pstContents->usSlotId == 1)
		usSlotIdForWeb = 1;
	else
		HxLOG_Error("ERROR: Wrong Slot ID!!\n");
#else
	// 일단 single slot인 경우에는 아래쪽에서 받은 slot ID를 그대로 올려주도록...
	usSlotIdForWeb = pstContents->usSlotId; 	// single slot 모델에서 필요하다면 이부분 변경해도 됩니다.
#endif

	hBusResult = BUS_PostMessageToOM(NULL, eCasEvent, HANDLE_NULL, (HUINT32)eDxCAS_GROUPID_CAM, (HINT32)usSlotIdForWeb, (HINT32)pucSubData);
	if(hBusResult != ERR_BUS_NO_ERROR)
	{
		HxLOG_Info("bus result %d\n", hBusResult);
	}
#endif /* CONFIG_PROD_PROFILE_OCTO2 */

	HxLOG_Info("-[%s]...\n", __FUNCTION__);
	return ERR_OK;
}


static HERROR xmgr_cas_CiOpOperatorPrepareFail(XmgrCas_CiCamSearchContents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	SvcCas_CtrlParam_t			stParam;
	CI_OperatorPrepareStatus_t	stInParam;
	HUINT16		usSlotId = (HUINT16)p1;

	if (pstContents->usSlotId != usSlotId)
		HxLOG_Warning("Warning: Check Slot ID...\n");

	stInParam.bSearchAvailable 	= FALSE;
	stInParam.usSlotId			= pstContents->usSlotId;

	stParam.ulControlType		= eCACTRL_CI_OperatorPrepareStatus;
	stParam.pvIn				= (void *)&stInParam;

	if(SVC_CAS_CtrlSet(eDxCAS_GROUPID_CAM, stParam.ulControlType, &stParam) != ERR_OK)
	{
		HxLOG_Error("Critical Eror!!!\n");
	}

	return ERR_OK;
}


// OPL_BUILDER 로부터 event를 받아 처리 되는 함수.
STATIC HERROR xmgr_cas_CiOpSearchOperatorPrepareStatus(XmgrCas_CiCamSearchContents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	SvcCas_CtrlParam_t			stParam;
	CI_OperatorPrepareStatus_t	stInParam;

	HxLOG_Info("+[%s]...\n", __FUNCTION__);

	if(xmgr_cas_CiOpSearchPrepareSearchAction(pstContents) == ERR_OK)
	{
		stInParam.bSearchAvailable = TRUE;
	}
	else
	{
		stInParam.bSearchAvailable = FALSE;
	}

	stInParam.usSlotId		= pstContents->usSlotId;

	stParam.ulControlType	= eCACTRL_CI_OperatorPrepareStatus;
	stParam.pvIn			= (void *)&stInParam;

	if(SVC_CAS_CtrlSet(eDxCAS_GROUPID_CAM, stParam.ulControlType, &stParam) != ERR_OK)
	{
		HxLOG_Error("Critical Error!!!\n");
	}

	if(stInParam.bSearchAvailable == FALSE)
	{
		xmgr_cas_CiOpSearchFinished(pstContents);
		xmgr_cas_CiOpActionRelease(pstContents);
		xmgr_cas_CiOpSearchDestroy();
	}

	HxLOG_Info("-[%s]...\n", __FUNCTION__);

	return ERR_OK;
}

STATIC HERROR xmgr_cas_CiOpSearchOperatorTune(XmgrCas_CiCamSearchContents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	SvcCas_OperatorTune_t *pstOpTune = (SvcCas_OperatorTune_t *)p3;

	if(pstOpTune == NULL)
	{
		HxLOG_Error("pstOpTune == NULL\n");
		return ERR_FAIL;
	}

	pstContents->stOpTune = *pstOpTune;

	if(xmgr_cas_CiOpCheckDeliveryCapability(pstContents) == ERR_OK)
	{
		if(xmgr_cas_CiOpCheckTuningParameter(pstContents) == ERR_FAIL)
		{
			return xmgr_cas_CiOpTuneStatus(pstContents, 0, 0, eCH_Tune_InvalidParam);
		}

		return xmgr_cas_CiOpTuneSearchTp(pstContents);
	}
	else
	{
		return xmgr_cas_CiOpTuneStatus(pstContents, 0, 0, eCH_Tune_NotSupported);
	}
}

STATIC HERROR xmgr_cas_CiOpTuneStatus(XmgrCas_CiCamSearchContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	SvcCas_CtrlParam_t			stParam;
	CI_SetOperatorTuneStatus_t	stOpTune;

	HxLOG_Info("+[%s]...\n", __FUNCTION__);

	stOpTune.usSlotId		= (HUINT16)pstContents->usSlotId;
	stOpTune.ucStrength		= (HUINT8)(p1 & 0xff);
	stOpTune.ucQuality		= (HUINT8)(p2 & 0xff);
	stOpTune.ucStatus		= (HUINT8)p3;
	stOpTune.pucDes			= pstContents->stOpTune.pucDes;
	stOpTune.ucDesLen		= pstContents->stOpTune.ucDesLen;
	stOpTune.ucNextDesNum	= pstContents->stOpTune.ucNextDesNum;

	stParam.ulControlType = eCACTRL_CI_SetOperatorTuneStatus;
	stParam.pvIn = (void *)&stOpTune;

	if(SVC_CAS_CtrlSet(eDxCAS_GROUPID_CAM, stParam.ulControlType, &stParam) != ERR_OK)
	{
		HxLOG_Error("Critical Error!!!\n");
	}

	HxLOG_Info("-[%s]...\n", __FUNCTION__);

	return ERR_OK;
}

STATIC HERROR xmgr_cas_CiOpSearchOperatorNit(XmgrCas_CiCamSearchContents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	SvcCas_CtrlParam_t		stParam;
	CI_GetOperatorInParam_t stInParam;
	HBOOL					bErrOccured;

	HxLOG_Print("+[%s]...\n", __FUNCTION__);

	stInParam.usSlotId	= pstContents->usSlotId;
	stParam.pvIn		= (void *)&stInParam;
	stParam.pvOut		= (void *)&bErrOccured;

	if(SVC_CAS_CtrlGet(eDxCAS_GROUPID_CAM, eCACTRL_CI_GetOpErrStatus, &stParam) != ERR_OK)
	{
		HxLOG_Error("SVC_CAS_CtrlGet - eCACTRL_CI_GetOpErrStatus error\n");
	}

	if(bErrOccured)
	{
		HxLOG_Info("Error is Occured during OP operation... Service list will not be made...\n");
		if(pstContents->bOnlyUpdate == FALSE)
		{
			xmgr_cas_CiOpRetrieveResults(pstContents);
			xmgr_cas_CiOpSearchFinished(pstContents);
			xmgr_cas_CiOpActionRelease(pstContents);
		}
		xmgr_cas_CiOpSearchDestroy();

		return ERR_OK;
	}

	xmgr_cas_CiOpRetrieveResults(pstContents);

	if(pstContents->bOnlyUpdate == TRUE)
	{
		HxLOG_Info("pstContents->bOnlyUpdate == TRUE...\n");
		BUS_PostMessage (NULL, eSEVT_DB_NOTIFY_UPDATED, hAction, 0, 0, 0);
	}
	else
	{
		xmgr_cas_CiOpSearchFinished(pstContents);
		xmgr_cas_CiOpActionRelease(pstContents);
	}

	xmgr_cas_CiOpSearchDestroy();

	return ERR_OK;
}


static HERROR xmgr_cas_CiOpMsgGwmCreate (XmgrCas_CiCamSearchContents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxSTD_memset(pstContents, 0, sizeof(XmgrCas_CiCamSearchContents_t));

	NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);
	return ERR_OK;
}

static HERROR xmgr_cas_CiOpMsgGwmDestroyed (XmgrCas_CiCamSearchContents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);
	return ERR_OK;
}

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
BUS_Result_t xproc_cas_CiOperatorProfile(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XmgrCas_CiCamSearchContents_t *pstContents = xmgr_cas_CiOpGetContents();

	switch(message)
	{
		case eMEVT_BUS_CREATE:
			if(xmgr_cas_CiOpMsgGwmCreate(pstContents, hAction, p1, p2, p3) != ERR_OK)
				HxLOG_Error("local_cas_ciop_MsgGwmCreate() ERROR!!!\n");
			return MESSAGE_BREAK;

		case eMEVT_BUS_DESTROY:
			if(xmgr_cas_CiOpMsgGwmDestroyed(pstContents, hAction, p1, p2, p3) != ERR_OK)
				HxLOG_Error("local_cas_ciop_MsgGwmDestroyed() ERROR!!!\n");
			return MESSAGE_BREAK;

		case eMEVT_CAS_CIPLUS_OPERATOR_PREPARE_REQ:
			if(xmgr_cas_CiOpOperatorPrepareReq(pstContents, hAction, p1, p2, p3) != ERR_OK)
				HxLOG_Error("xmgr_cas_CiOpOperatorPrepareReq() ERROR!!!\n");
			return MESSAGE_BREAK;

		default:
			break;
	}

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}

BUS_Result_t xproc_cas_CiOpSearchOperatorProfile(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XmgrCas_CiCamSearchContents_t *pstContents = xmgr_cas_CiOpGetContents();

	switch(message)
	{
		case eMEVT_BUS_CREATE:
			if(xmgr_cas_CiOpSearchMsgGwmCreate(pstContents, hAction, p1, p2, p3) != ERR_OK)
				HxLOG_Error("xmgr_cas_CiOpSearchMsgGwmCreate() ERROR!!!\n");
			return MESSAGE_BREAK;

		case eMEVT_BUS_DESTROY:
			if(xmgr_cas_CiOpSearchMsgGwmDestroyed(pstContents, hAction, p1, p2, p3) != ERR_OK)
				HxLOG_Error("xmgr_cas_CiOpSearchMsgGwmDestroyed() ERROR!!!\n");
			return MESSAGE_BREAK;

		case eMEVT_SEARCH_ACTION_START:
			// TODO: From OPL builder
			/*	webapp 로 부터 search 가능여부가 전달된다. 이 이벤트를 가지고 search를 계속 할 것인지 아닐 것인지 CI stack에 판단한다.
			*/
			if(xmgr_cas_CiOpSearchOperatorPrepareStatus(pstContents, hAction, p1, p2, p3) != ERR_OK)
				HxLOG_Error("xmgr_cas_CiOpSearchOperatorPrepareStatus() ERROR!!!\n");
			return MESSAGE_BREAK;

		case eMEVT_BUS_TIMER:
			return xmgr_cas_CiOpSearchMsgTimer(pstContents, hAction, p1, p2, p3);

		case eSEVT_CH_LOCKED:
			if(xmgr_cas_CiOpSearchEvtChLocked(pstContents, hAction, p1, p2, p3) != ERR_OK)
				HxLOG_Error("xmgr_cas_CiOpSearchEvtChLocked() ERROR!!!\n");
			break;

		case eSEVT_CH_NO_SIGNAL:
			if(xmgr_cas_CiOpSearchEvtChNoSignal(pstContents, hAction, p1, p2, p3) != ERR_OK)
				HxLOG_Error("xmgr_cas_CiOpSearchEvtChNoSignal() ERROR!!!\n");
			break;

		case eMEVT_CAS_CIPLUS_OPERATOR_TUNE:
			if(xmgr_cas_CiOpSearchOperatorTune(pstContents, hAction, p1, p2, p3) != ERR_OK)
				HxLOG_Error("xmgr_cas_CiOpSearchOperatorTune() ERROR!!!\n");
			return MESSAGE_BREAK;

		case eMEVT_CAS_ACTION_PREPARE_TUNE_FAIL:
			if(xmgr_cas_CiOpOperatorPrepareFail(pstContents, hAction, p1, p2, p3) != ERR_OK)
				HxLOG_Error("xmgr_cas_CiOpOperatorPrepareFail() ERROR!!!\n");
			return MESSAGE_BREAK;

		case eMEVT_CAS_CIPLUS_OPERATOR_NIT:
			if(xmgr_cas_CiOpSearchOperatorNit(pstContents, hAction, p1, p2, p3) != ERR_OK)
				HxLOG_Error("xmgr_cas_CiOpSearchOperatorNit() ERROR!!!\n");
			return MESSAGE_BREAK;

		default:
			break;
	}

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}


/*********************** End of File ******************************/

