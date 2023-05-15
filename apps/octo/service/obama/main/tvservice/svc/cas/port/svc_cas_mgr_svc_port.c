/**
	@file     _svc_cas_mgr_svc_port.c
	@brief    _svc_cas_mgr_svc_port.c (CAS Service)

	Description: . \n
	Module:  \n
	Remarks :\n

	Copyright (c) 2012 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

/********************************************************************
* Header Files
********************************************************************/
#define ___01_Header_Files___

#include <octo_common.h>

#include <db_svc.h>

#include <pal_pipe.h>

#include "local_include/_svc_cas_si_api.h"
#include <_svc_cas_sub_svc_api.h>
#include <_svc_cas_mgr_svc_port.h>

#include <svc_rec.h>
#include <svc_meta.h>

/********************************************************************
* For Debugging
********************************************************************/
#define ___02_Debugging___

/********************************************************************
* Local Definitions : #define / enum / struct / union
********************************************************************/
#define ___03_Local_Definitions___

#define	SvcCas_GetSiRawPoolFromHandle(hRawPool)		((HANDLE_NULL == (HUINT32)hRawPool) ? NULL : ((svcCas_RawPool_t *)hRawPool))

/********************************************************************
* Extern Variables / Function Prototypes
********************************************************************/
#define ___04_Extern_Vars_Func_Prototypes___

/********************************************************************
* Static Variables / Function Prototypes
********************************************************************/
#define ___05_Local_Vars_Func_Prototypes___

STATIC PostMsgToMgrCb_t		s_fnPostMsgToMgr = NULL;
STATIC PostDataToMgrCb_t	s_fnPostDataToMgr = NULL;


/********************************************************************
* Static Functions
* Naming rule : "local"_"CasMgrMain" _ "Function Name"
********************************************************************/
#define ___06_Local_APIs___

STATIC SvcCas_SubType_e svc_cas_SvcPortDbConvertCasType(DxCasType_e eCasType)
{
	SvcCas_SubType_e eSubCasType = eDxCAS_TYPE_All;

	switch(eCasType)
	{
	case eDxCAS_TYPE_All:			return eCAS_SUB_TYPE_ALL;
	case eDxCAS_TYPE_FTA:			return eCAS_SUB_TYPE_FTA;
	case eDxCAS_TYPE_NAGRA:			return eCAS_SUB_TYPE_NAGRA;
	case eDxCAS_TYPE_IRDETO:		return eCAS_SUB_TYPE_IRDETO;
	case eDxCAS_TYPE_NDS:			return eCAS_SUB_TYPE_NDS;
	case eDxCAS_TYPE_VIACCESS:		return eCAS_SUB_TYPE_VIACCESS;
	case eDxCAS_TYPE_CONAX:			return eCAS_SUB_TYPE_CONAX;
	case eDxCAS_TYPE_MEDIAGUARD:	return eCAS_SUB_TYPE_MEDIAGUARD;
	case eDxCAS_TYPE_CRYPTOWORKS:	return eCAS_SUB_TYPE_CRYPTOWORKS;
	case eDxCAS_TYPE_JPBCAS:		return eCAS_SUB_TYPE_JPBCAS;
	case eDxCAS_TYPE_JPCCAS:		return eCAS_SUB_TYPE_JPCCAS;
	case eDxCAS_TYPE_VERIMATRIX:	return eCAS_SUB_TYPE_VERIMATRIX;
	default:
		if(eCasType & eDxCAS_TYPE_FTA)			eSubCasType |= eCAS_SUB_TYPE_FTA;
		if(eCasType & eDxCAS_TYPE_NAGRA)		eSubCasType |= eCAS_SUB_TYPE_NAGRA;
		if(eCasType & eDxCAS_TYPE_IRDETO)		eSubCasType |= eCAS_SUB_TYPE_IRDETO;
		if(eCasType & eDxCAS_TYPE_NDS)			eSubCasType |= eCAS_SUB_TYPE_NDS;
		if(eCasType & eDxCAS_TYPE_VIACCESS)		eSubCasType |= eCAS_SUB_TYPE_VIACCESS;
		if(eCasType & eDxCAS_TYPE_CONAX)		eSubCasType |= eCAS_SUB_TYPE_CONAX;
		if(eCasType & eDxCAS_TYPE_MEDIAGUARD)	eSubCasType |= eCAS_SUB_TYPE_MEDIAGUARD;
		if(eCasType & eDxCAS_TYPE_CRYPTOWORKS)	eSubCasType |= eCAS_SUB_TYPE_CRYPTOWORKS;
		if(eCasType & eDxCAS_TYPE_JPBCAS)		eSubCasType |= eCAS_SUB_TYPE_JPBCAS;
		if(eCasType & eDxCAS_TYPE_JPCCAS)		eSubCasType |= eCAS_SUB_TYPE_JPCCAS;
		if(eCasType & eDxCAS_TYPE_VERIMATRIX)	eSubCasType |= eCAS_SUB_TYPE_VERIMATRIX;
		if(eSubCasType)
			return eSubCasType;
	}

	return eCAS_SUB_TYPE_ALL;
}

STATIC RxRscType_e svc_cas_SvcPortRmConvertRmItem(SvcCas_RmItem_e eCasMgrRmItem)
{
	switch(eCasMgrRmItem)
	{
	case eCAS_RM_ITEM_TUNER_GROUP:		return eRxRSCTYPE_TUNER_GROUP;
	case eCAS_RM_ITEM_DEMUX:			return eRxRSCTYPE_DEMUX;
	case eCAS_RM_ITEM_AV_DEC:			return eRxRSCTYPE_AV_DEC;
	case eCAS_RM_ITEM_HDD_REC:			return eRxRSCTYPE_HDD_REC;
	case eCAS_RM_ITEM_RFINPUT:			return eRxRSCTYPE_RFINPUT;
	default:							break;
	}
	return eRxRSCTYPE_MAX;
}

////////////////////////////////////////////////////////////
#define ___LOCAL_FUNCS_FOR_Tuning___
////////////////////////////////////////////////////////////

#if defined(CONFIG_MW_CI_PLUS)
// Tuning Parameter Converting function : Satellite
STATIC HUINT8 svc_cas_SvcPortSiConvertSatDeliDesToPalPolar(HUINT32 ulValue)
{
	switch (ulValue)
	{
	case 0x00:	return eDxSAT_POLAR_HOR;
	case 0x01:	return eDxSAT_POLAR_VER;
	case 0x02:	return eDxSAT_POLAR_LEFT;
	case 0x03:	return eDxSAT_POLAR_RIGHT;
	default:	break;
	}
	return eDxSAT_POLAR_AUTO;
}

STATIC HUINT8 svc_cas_SvcPortSiConvertSatDeliDesToPalFec(HUINT32 ulValue)
{
	switch (ulValue)
	{
	case 0x01:	return eDxSAT_CODERATE_1_2;
	case 0x02:	return eDxSAT_CODERATE_2_3;
	case 0x03:	return eDxSAT_CODERATE_3_4;
	case 0x04:	return eDxSAT_CODERATE_5_6;
	case 0x05:	return eDxSAT_CODERATE_7_8;
	case 0x06:	return eDxSAT_CODERATE_8_9;
	case 0x07:	return eDxSAT_CODERATE_3_5;
	case 0x08:	return eDxSAT_CODERATE_4_5;
	case 0x09:	return eDxSAT_CODERATE_9_10;
	case 0x0F:	return eDxSAT_CODERATE_NONE;
	default:	break;
	}
	return eDxSAT_CODERATE_AUTO;
}

STATIC HUINT8 svc_cas_SvcPortSiConvertSatDeliDesToPalTransspec(HUINT32 ulValue)
{
	switch (ulValue)
	{
	case 0x00:	return eDxSAT_TRANS_DVBS;
	case 0x01:	return eDxSAT_TRANS_DVBS2;
	default:	break;
	}
	return eDxSAT_TRANS_DVBS;
}

STATIC HUINT8 svc_cas_SvcPortSiConvertSatDeliDesToPalRollOff(HUINT32 ulValue)
{
	switch (ulValue)
	{
	case 0x00:	return eDxSAT_ROLL_035;
	case 0x01:	return eDxSAT_ROLL_025;
	case 0x02:	return eDxSAT_ROLL_020;
	case 0x03:	return eDxSAT_ROLL_035;
	default:	break;
	}
	return eDxSAT_ROLL_035;
}

STATIC HUINT8 svc_cas_SvcPortSiConvertSatDeliDesToPalModType(HUINT32 ulValue)
{
	switch (ulValue)
	{
	case 0x00:	return eDxSAT_PSK_AUTO;
	case 0x01:	return eDxSAT_PSK_QPSK;
	case 0x02:	return eDxSAT_PSK_8PSK;
	case 0x03:	return eDxSAT_PSK_AUTO;	// 16 QAM (Not available for DVB-S2)
	default:	break;
	}
	return eDxSAT_PSK_AUTO;
}


// Tuning Parameter Converting function : Cable
STATIC HUINT8 svc_cas_SvcPortSiConvertCabDeliDesToPalQam(HUINT32 ulValue)
{
	switch (ulValue)
	{
	case 0x01:	return eDxCAB_CONSTELLATION_16QAM;
	case 0x02:	return eDxCAB_CONSTELLATION_32QAM;
	case 0x03:	return eDxCAB_CONSTELLATION_64QAM;
	case 0x04:	return eDxCAB_CONSTELLATION_128QAM;
	case 0x05:	return eDxCAB_CONSTELLATION_256QAM;
	default:	break;
	}
	return eDxCAB_CONSTELLATION_AUTO;
}

// Tuning Parameter Converting function : Terrestrial
STATIC HUINT8 svc_cas_SvcPortSiConvertTerDeliDesToPalCodeRate(HUINT32 ulValue)
{
	switch (ulValue)
	{
	case 0x00:	return eDxTER_CODERATE_1_2;
	case 0x01:	return eDxTER_CODERATE_2_3;
	case 0x02:	return eDxTER_CODERATE_3_4;
	case 0x03:	return eDxTER_CODERATE_5_6;
	case 0x04:	return eDxTER_CODERATE_7_8;
	default:	break;
	}
	return eDxTER_CODERATE_AUTO;
}

STATIC HUINT8 svc_cas_SvcPortSiConvertTerDeliDesToPalTransMode(HUINT32 ulValue)
{
	switch(ulValue)
	{
	case 0x00:	return eDxTER_TRANSMODE_2K;
	case 0x01:	return eDxTER_TRANSMODE_8K;
	default:	break;
	}
	return eDxTER_TRANSMODE_AUTO;
}

STATIC HUINT8 svc_cas_SvcPortSiConvertTerDeliDesToPalGuardInterval(HUINT32 ulValue)
{
	switch (ulValue)
	{
	case 0x00:	return eDxTER_GUARDINTERVAL_1_32;
	case 0x01:	return eDxTER_GUARDINTERVAL_1_16;
	case 0x02:	return eDxTER_GUARDINTERVAL_1_8;
	case 0x03:	return eDxTER_GUARDINTERVAL_1_4;
	default:	break;
	}
	return eDxTER_GUARDINTERVAL_AUTO;
}

STATIC HUINT8 svc_cas_SvcPortSiConvertTerDeliDesToPalConstellation(HUINT32 ulValue)
{
	switch (ulValue)
	{
	case 0x00:	return eDxTER_CONSTELLATION_QPSK;
	case 0x01:	return eDxTER_CONSTELLATION_16QAM;
	case 0x02:	return eDxTER_CONSTELLATION_64QAM;
	default:	break;
	}
	return eDxTER_CONSTELLATION_AUTO;
}

STATIC HUINT8 svc_cas_SvcPortSiConvertTerDeliDesToPalHierarchy(HUINT32 ulValue)
{
	switch (ulValue)
	{
	case 0x00:	return eDxTER_HIERACHY_NONE;
	case 0x01:	return eDxTER_HIERACHY_1;
	case 0x02:	return eDxTER_HIERACHY_2;
	case 0x03:	return eDxTER_HIERACHY_4;
	default:	break;
	}
	return eDxTER_HIERACHY_AUTO;
}

STATIC HUINT8 svc_cas_SvcPortSiConvertTerDeliDesToPalBandWidth(HUINT32 ulValue)
{
	switch (ulValue)
	{
	case 0x00:	return eDxTER_BANDWIDTH_8M;
	case 0x01:	return eDxTER_BANDWIDTH_7M;
	case 0x02:	return eDxTER_BANDWIDTH_6M;
	default:	break;
	}
	return eDxTER_BANDWIDTH_8M;
}
#endif

STATIC HERROR	svc_cas_SvcPortSiConvertSdtSectionInfoPalToRm(const svcCas_SiSdtSectionInfo_t *pstInType, svcCas_SiSdtSectionInfo_t *pstCnvtType)
{
	pstCnvtType->usTsId			= pstInType->usTsId;
	pstCnvtType->ucVer			= pstInType->ucVer;
	pstCnvtType->usOrgNetId		= pstInType->usOrgNetId;
	pstCnvtType->ucReserved		= pstInType->ucReserved;
	pstCnvtType->ulCrc32		= pstInType->ulCrc32;

	return ERR_OK;
}

STATIC HERROR	svc_cas_SvcPortSiConvertSdtServiceInfoPalToRM(const svcCas_SiSdtServiceInfo_t *pstInType, SvcCas_StdServiceInfo_t *pstCnvtType)
{
	pstCnvtType->usSvcId			= pstInType->usSvcId;
	pstCnvtType->bEitSched			= pstInType->bEitSched;
	pstCnvtType->bEitPreFollow		= pstInType->bEitPreFollow;
	pstCnvtType->ucRunningStatus	= pstInType->ucRunningStatus;
	pstCnvtType->bCaFree			= pstInType->bCaFree;

	return ERR_OK;
}

/********************************************************************
* Module Internal Functions
* Naming rule : "CasMgrMain" _ "Function Name"
********************************************************************/
#define ___07_Module_Internal_APIs___

#define _____DB________________________________________________________________________________________________________

HUINT16 svc_cas_SvcPortDbGetAllUniqueId(void)
{
	return DB_SVC_GetAllUniqueId();
}

HERROR svc_cas_SvcPortDbGetTsInfoByServiceHandle(Handle_t hSvc, HUINT16 *pusUniqueId, HUINT16 *pusTsId, HUINT16 *pusOnId)
{
	HERROR			hErr;
	DbSvc_TsInfo_t		stTsInfo;

	hErr = DB_SVC_GetTsInfoByServiceHandle(hSvc, &stTsInfo);
	if(hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	if(pusUniqueId != NULL)
	{
		*pusUniqueId = stTsInfo.usUniqueId;
	}

	if(pusTsId != NULL)
	{
		*pusTsId = stTsInfo.usTsId;
	}

	if(pusOnId != NULL)
	{
		*pusOnId = stTsInfo.usOnId;
	}

	return ERR_OK;
}

HERROR svc_cas_SvcPortDbGetSvcInfoByServiceHandle(Handle_t hSvc, SvcCas_DbSvcInfo_t *pstSvcInfo)
{
	HERROR			hErr;
	DbSvc_Info_t		svcInfo;

	if(pstSvcInfo == NULL)
	{
		HxLOG_Error(" pstSvcInfo is NULL \n");
		return ERR_FAIL;
	}

	hErr = DB_SVC_GetServiceInfo(hSvc, &svcInfo);
	if(hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	pstSvcInfo->usSvcId			= DbSvc_GetSvcId(&svcInfo);
	pstSvcInfo->usPmtPid		= DbSvc_GetPmtPid(&svcInfo);
	pstSvcInfo->usOnId			= DbSvc_GetOnId(&svcInfo);
	pstSvcInfo->eCasMgrCasType	= svc_cas_SvcPortDbConvertCasType(DbSvc_GetCasType(&svcInfo));
//	pstSvcInfo->ulUserFlag1		= svcInfo.eUserFlag1;
	(void)DB_SVC_ServiceGetUserFlag(&pstSvcInfo->ulUserFlag1, (const DbSvc_Info_t *)&svcInfo);

#if defined(CONFIG_MW_CI)
	pstSvcInfo->eInstType		= (SvcCas_InstanceType_e)DbSvc_GetCiInstType(&svcInfo);
	pstSvcInfo->usSlotId		= DbSvc_GetCamSlotId(&svcInfo);

#if defined(CONFIG_MW_CI_PLUS)
	pstSvcInfo->ulTimeStamp		= DbSvc_GetCiTimeStamp(&svcInfo);
	pstSvcInfo->ucNumOfEntries	= DbSvc_GetCiNumOfEntries(&svcInfo);

	VK_memcpy32(pstSvcInfo->ausCamBrandId, DbSvc_GetCamBrandId(&svcInfo), sizeof(HUINT16) * DbSvc_GetCiNumOfEntries(&svcInfo));
	VK_memcpy32(pstSvcInfo->aucCamId, DbSvc_GetCamId(&svcInfo), CI_PLUS_CICAM_ID_LEN);
#endif
#endif

	return ERR_OK;
}

HERROR svc_cas_SvcPortDbGetPmtPidbyServiceHandle(Handle_t hSvc, HUINT16 *pusPmtPid)
{
	HERROR		hErr;
	DbSvc_Info_t	stSvcInfo;

	hErr = DB_SVC_GetServiceInfo (hSvc, &stSvcInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Error(" DB_SVC_GetServiceInfo() \n");
		return ERR_FAIL;
	}

	*pusPmtPid = DbSvc_GetPmtPid(&stSvcInfo);

	return ERR_OK;
}

HERROR svc_cas_SvcPortDbUpdateServiceInfo(Handle_t hSvc, SvcCas_DbSvcInfo_t *pstSvcInfo)
{
	HERROR		hErr;
	DbSvc_Info_t	stSvcInfo;

	if(pstSvcInfo == NULL)
	{
		HxLOG_Error(" pstSvcInfo is NULL\n");
		return ERR_FAIL;
	}

	hErr = DB_SVC_GetServiceInfo (hSvc, &stSvcInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Error(" DB_SVC_GetServiceInfo() \n");
		return ERR_FAIL;
	}

	// SUB CAS에 의해 아래 usSvcId, usPmtPid, eCasType가 업데이트 될 일이 있나?
	DbSvc_SetSvcId(&stSvcInfo, pstSvcInfo->usSvcId);
	DbSvc_SetPmtPid(&stSvcInfo, pstSvcInfo->usPmtPid);
	DbSvc_SetOnId(&stSvcInfo, pstSvcInfo->usOnId);
//	stSvcInfo.eUserFlag1		= pstSvcInfo->ulUserFlag1;
	DB_SVC_ServiceSetUserFlag(&stSvcInfo, (const DbSvc_UserFlag1_e)pstSvcInfo->ulUserFlag1);

	DbSvc_SetCasType(&stSvcInfo, pstSvcInfo->eCasMgrCasType);

#if defined(CONFIG_MW_CI)
	DbSvc_SetCiInstType(&stSvcInfo, pstSvcInfo->eInstType);
	DbSvc_SetCamSlotId(&stSvcInfo, pstSvcInfo->usSlotId);

#if defined(CONFIG_MW_CI_PLUS)
	DbSvc_SetCiTimeStamp(&stSvcInfo, pstSvcInfo->ulTimeStamp);
	DbSvc_SetCiNumOfEntries(&stSvcInfo, pstSvcInfo->ucNumOfEntries);
	DbSvc_SetCamBrandId(&stSvcInfo, pstSvcInfo->ausCamBrandId, (sizeof(HUINT16) * DbSvc_GetCiNumOfEntries(&stSvcInfo)));
#endif
#endif

	hErr = DB_SVC_UpdateServiceInfo (hSvc, &stSvcInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Error(" DB_SVC_UpdateServiceInfo() \n");
		return ERR_FAIL;
	}

	svc_cas_SvcPortUtilPostMsgToAp(eSEVT_DB_NOTIFY_UPDATED, HANDLE_NULL, 0, 0, 0);

	return ERR_OK;
}

HERROR svc_cas_SvcPortDbGetSameTpSvcHandleListByServiceHandle(Handle_t hSvc, HINT32 *pnCnt, Handle_t **pphSvcList)
{
	HERROR		hErr;
	DbSvc_Info_t	stSvcInfo;
	DbSvc_Condition_t	stCond;

	if(pnCnt == NULL || pphSvcList == NULL)
	{
		HxLOG_Error("pnCnt (0x%x), pphSvcList (0x%x)\n", pnCnt, pphSvcList);
		return ERR_FAIL;
	}

	hErr = DB_SVC_GetServiceInfo (hSvc, &stSvcInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("DB_SVC_GetServiceInfo() error\n");
		return ERR_FAIL;
	}

	DB_SVC_InitSvcFindCondition(&stCond);

	stCond.nTsIdx = (HINT32)DbSvc_GetTsIdx(&stSvcInfo);
	stCond.nOrgDeliType = (HINT32)DbSvc_GetDeliType(stSvcInfo);
	if(DbSvc_GetDeliType(stSvcInfo) == eDxDELIVERY_TYPE_SAT)
		stCond.nAntIdx = (HINT32)DbSvc_GetAntIdx(&stSvcInfo);

	hErr = DB_SVC_FindSvcHandleList(&stCond, pnCnt, pphSvcList);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("DB_SVC_FindSvcHandleList() error\n");
		return hErr;
	}

	return ERR_OK;
}

void svc_cas_SvcPortDbFreeServiceList(Handle_t *phSvcList)
{
	DB_SVC_FreeServiceList(phSvcList);
}

HERROR svc_cas_SvcPortDbUpdatePmtPidInServiceInfo(Handle_t hSvc, HUINT16 usPmtPid)
{
	HERROR		hErr;
	DbSvc_Info_t	stSvcInfo;

	hErr = DB_SVC_GetServiceInfo (hSvc, &stSvcInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Error(" DB_SVC_GetServiceInfo() \n");
		return ERR_FAIL;
	}

	if (DbSvc_GetPmtPid(&stSvcInfo) != PID_NULL)
	{
		HxLOG_Error(" stSvcInfo.usPmtPid = %d \n", DbSvc_GetPmtPid(&stSvcInfo));
		return ERR_FAIL;
	}

	DbSvc_SetPmtPid(&stSvcInfo, usPmtPid);

	hErr = DB_SVC_UpdateServiceInfo (hSvc, &stSvcInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Error(" DB_SVC_UpdateServiceInfo() \n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

#define _____RM________________________________________________________________________________________________________

HERROR svc_cas_SvcPortRmCheckIfLatestActionHandle(Handle_t hAction)
{
	return PAL_PIPE_IsActionHandleLatest(hAction);
}

HUINT32 svc_cas_SvcPortRmGetActionId(Handle_t hAction)
{
	return PAL_PIPE_GetActionId(hAction);
}

HERROR svc_cas_SvcPortRmGetResourceId(HUINT32 ulActionId, SvcCas_RmItem_e eCasMgrRmItem, HUINT32 *pulResourceId)
{
	RxRscType_e eRmItem;

	eRmItem = svc_cas_SvcPortRmConvertRmItem(eCasMgrRmItem);

	return PAL_PIPE_GetResourceId(ulActionId, eRmItem, (HUINT32 *)pulResourceId);
}

Handle_t svc_cas_SvcPortRmMakeActionHandle(HUINT32 ulActionId, HUINT32 ulSessionVer)
{
	return PAL_PIPE_GetActionHandle (ulActionId);
}

#define _____SI_RAW________________________________________________________________________________________________________

SvcCas_SiRawPool_h svc_cas_SvcPortSiRawAllocPool(HUINT16 usMaxRawSec, HUINT8 *szPoolName)
{
	return (SvcCas_SiRawPool_h)svc_cas_RawAllocRawPool(usMaxRawSec, szPoolName);
}

HERROR svc_cas_SvcPortSiRawFreeSection(SvcCas_SiRawPool_h hSiRawPool, HUINT16 usUniqueId, SiTableType_e eSiTableType, HBOOL bUseExtId, HUINT32 ulExtId)
{
	svcCas_RawPool_t	*pstRawPool = SvcCas_GetSiRawPoolFromHandle(hSiRawPool);

	if (NULL == pstRawPool)
		return ERR_FAIL;

	return svc_cas_RawFreeSection(pstRawPool, usUniqueId, eSiTableType, bUseExtId, (HUINT16)ulExtId);
}

HERROR svc_cas_SvcPortSiRawAddRawSection(SvcCas_SiRawPool_h hSiRawPool, HUINT16 usUniqueId, SiTableType_e eSiTableType, HUINT8 *pucRawSec)
{
	svcCas_RawPool_t	*pstRawPool = SvcCas_GetSiRawPoolFromHandle(hSiRawPool);

	if (NULL == pstRawPool)
		return ERR_FAIL;

	return svc_cas_RawAddRawSection(pstRawPool, usUniqueId, eSiTableType, pucRawSec);
}

HUINT8 **svc_cas_SvcPortSiRawGetRawArrays(SvcCas_SiRawPool_h	 hSiRawPool,
											HUINT16 			 usTsUniqId,
											SiTableType_e		 eSiType,
											HBOOL				 bUseExtId,
											HUINT16				 usExtId,
											HUINT32				*pulArrayLen)
{
	svcCas_RawPool_t	*pstRawPool = SvcCas_GetSiRawPoolFromHandle(hSiRawPool);

	if (NULL == pstRawPool)
		return NULL;

	return svc_cas_RawGetRawArrays(hSiRawPool, usTsUniqId, eSiType, bUseExtId, usExtId, pulArrayLen);
}

HERROR svc_cas_SvcPortSiRawFreeRawArrays(HUINT8 **ppucRawArrays)
{
	return svc_cas_RawFreeRawArrays(ppucRawArrays);
}


#define _____SI_PARSE_________________________________________________________________________________________________________

HERROR svc_cas_SvcPortSiParseGetPmtPidByServiceId(SvcCas_SiRawPool_h	 hSiRawPool,
													HUINT16				 usTsUniqId,
													HUINT16				 usSvcId,
													HUINT16				*pusPmtPid)
{
	svcCas_RawPool_t	*pstRawPool = SvcCas_GetSiRawPoolFromHandle(hSiRawPool);

	if (NULL == pstRawPool)
		return ERR_FAIL;

	return svc_cas_SiGetPmtPidByServiceId(hSiRawPool, usTsUniqId, usSvcId, pusPmtPid);
}

HERROR svc_cas_SvcPortSiParseGetPmtIndexByServiceId(SvcCas_SiRawPool_h 	 hSiRawPool,
													HUINT16				 usTsUniqId,
													HUINT16				 usSvcId,
													HUINT32				*pulPmtIndex)
{
	svcCas_RawPool_t	*pstRawPool = SvcCas_GetSiRawPoolFromHandle(hSiRawPool);

	if (NULL == pstRawPool)
		return ERR_FAIL;

	return svc_cas_SiGetPmtIndexByServiceId(hSiRawPool, usTsUniqId, usSvcId, pulPmtIndex);
}

HERROR svc_cas_SvcPortSiParseGetPmtProgramInfoByServiceId(SvcCas_SiRawPool_h hSiRawPool, HUINT16 usTsUniqId, HUINT16 usSvcId, SvcCas_PmtProgInfo_t *pstInfoDest)
{
	svcCas_SiPmtProgramInfo_t 	stInfoSrc;
	HERROR						hErr = ERR_FAIL;
	svcCas_RawPool_t	*pstRawPool = SvcCas_GetSiRawPoolFromHandle(hSiRawPool);

	if (NULL == pstRawPool)
		return ERR_FAIL;

	hErr = svc_cas_SiGetPmtProgramInfoByServiceId(pstRawPool, usTsUniqId, usSvcId, &stInfoSrc);
	if(hErr == ERR_OK)
	{
		pstInfoDest->usProgNum		= stInfoSrc.usProgNum;
		pstInfoDest->ucVer			= stInfoSrc.ucVer;
		pstInfoDest->bCurNextInd	= stInfoSrc.bCurNextInd;
		pstInfoDest->usPCRPid		= stInfoSrc.usPCRPid;
		pstInfoDest->ulCrc32		= stInfoSrc.ulCrc32;
	}
	return hErr;
}

HERROR svc_cas_SvcPortSiParseGetPmtProgramInfoFromRaw(HUINT8 *pucRaw, SvcCas_PmtProgInfo_t *pstInfoDest)
{
	svcCas_SiPmtProgramInfo_t 	stInfoSrc;
	HERROR				hErr = ERR_FAIL;

	hErr = svc_cas_SiGetPmtProgramInfoFromRaw(pucRaw, &stInfoSrc);
	if(hErr == ERR_OK)
	{
		pstInfoDest->usProgNum		= stInfoSrc.usProgNum;
		pstInfoDest->ucVer			= stInfoSrc.ucVer;
		pstInfoDest->bCurNextInd	= stInfoSrc.bCurNextInd;
		pstInfoDest->usPCRPid		= stInfoSrc.usPCRPid;
		pstInfoDest->ulCrc32		= stInfoSrc.ulCrc32;
	}
	return hErr;
}

HERROR svc_cas_SvcPortSiParseCountPmtElementFromRaw(HUINT8 *pucRaw, HUINT32 *pnCnt)
{
	return svc_cas_SiCountPmtElementFromRaw(pucRaw, pnCnt);
}


HERROR svc_cas_SvcPortSiParseCountPmtElement(SvcCas_SiRawPool_h hSiRawPool, HUINT16 usTsUniqId, HUINT32 ulIdx, HUINT32 *pnCnt)
{
	svcCas_RawPool_t	*pstRawPool = SvcCas_GetSiRawPoolFromHandle(hSiRawPool);

	if (NULL == pstRawPool)
		return ERR_FAIL;

	return svc_cas_SiCountPmtElement(pstRawPool, usTsUniqId, ulIdx, pnCnt);
}

HERROR svc_cas_SvcPortSiParseGetPmtElementInfoFromRaw(HUINT8 *pucRaw, HUINT32 ulElemIdx, svcCas_SiPmtElementInfo_t *pstInfoDest)
{
	svcCas_SiPmtElementInfo_t	stInfoSrc;
	HERROR				hErr = ERR_FAIL;

	hErr = svc_cas_SiGetPmtElementInfoFromRaw(pucRaw, ulElemIdx, &stInfoSrc);
	if(hErr == ERR_OK)
	{
		pstInfoDest->ucStreamType 	= stInfoSrc.ucStreamType;
		pstInfoDest->usElemPid		= stInfoSrc.usElemPid;
	}
	return hErr;
}

HERROR svc_cas_SvcPortSiParseGetPmtElementInfo(SvcCas_SiRawPool_h			 hSiRawPool,
												HUINT16						 usTsUniqId,
												HUINT32						 ulIdx,
												HUINT32						 nSubIdx,
												svcCas_SiPmtElementInfo_t	*pstInfo)
{
	svcCas_RawPool_t	*pstRawPool = SvcCas_GetSiRawPoolFromHandle(hSiRawPool);

	if (NULL == pstRawPool)
		return ERR_FAIL;

	return svc_cas_SiGetPmtElementInfo(pstRawPool, usTsUniqId, ulIdx, nSubIdx, (svcCas_SiPmtElementInfo_t *)pstInfo);
}

HERROR svc_cas_SvcPortSiParseCountPmtProgramDescriptor(SvcCas_SiRawPool_h	 hSiRawPool,
														HUINT16				 usTsUniqId,
														HUINT32				 ulIdx,
														HUINT32				 ulTag,
														HUINT32				*pnCnt)
{
	svcCas_RawPool_t	*pstRawPool = SvcCas_GetSiRawPoolFromHandle(hSiRawPool);

	if (NULL == pstRawPool)
		return ERR_FAIL;

	return svc_cas_SiCountPmtProgramDescriptor(pstRawPool, usTsUniqId, ulIdx, ulTag, pnCnt);
}

HERROR svc_cas_SvcPortSiParseCountPmtElementDescriptor(SvcCas_SiRawPool_h	 hSiRawPool,
														HUINT16				 usTsUniqId,
														HUINT32				 ulIdx,
														HUINT32				 nSubIdx,
														HUINT32				 ulTag,
														HUINT32				*pnDesCnt)
{
	svcCas_RawPool_t	*pstRawPool = SvcCas_GetSiRawPoolFromHandle(hSiRawPool);

	if (NULL == pstRawPool)
		return ERR_FAIL;

	return svc_cas_SiCountPmtElementDescriptor(pstRawPool, usTsUniqId, ulIdx, nSubIdx, ulTag, pnDesCnt);
}

HERROR svc_cas_SvcPortSiParseGetPmtProgramCaDescriptor(SvcCas_SiRawPool_h	 hSiRawPool,
														SIxTextEncoding_e	 eTextEnc,
														HUINT16				 usTsUniqId,
														HUINT32				 ulIdx,
														HUINT32				 nDesIdx,
														SvcCas_CaDes_t		*pstCaMgrCaDes)
{
	HERROR				 hErr;
	svcCas_CaDes_t		*pstCaDes = NULL;
	svcCas_RawPool_t	*pstRawPool = SvcCas_GetSiRawPoolFromHandle(hSiRawPool);

	if (NULL == pstRawPool)
		return ERR_FAIL;


	if(pstCaMgrCaDes == NULL)
	{
		return ERR_FAIL;
	}

	hErr = svc_cas_SiGetPmtProgramDescriptor(pstRawPool, eTextEnc, usTsUniqId, ulIdx, TAG_CA, nDesIdx, (void **)&pstCaDes);
	if(hErr != ERR_OK)
	{
		HxLOG_Error(" svc_cas_SiGetPmtProgramDescriptor() \n");
		return ERR_FAIL;
	}

	pstCaMgrCaDes->ucDesTag		= pstCaDes->ucDesTag;
	pstCaMgrCaDes->usCaSysId	= pstCaDes->usCaSysId;
	pstCaMgrCaDes->usCaPid		= pstCaDes->usCaPid;

	hErr = svc_cas_SiFreeDescriptor((void *)pstCaDes);
	if(hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR svc_cas_SvcPortSiParseGetPmtElementCaDescriptor(SvcCas_SiRawPool_h	 hSiRawPool,
														SIxTextEncoding_e	 eTextEnc,
														HUINT16				 usTsUniqId,
														HUINT32				 ulIdx,
														HUINT32				 nSubIdx,
														HUINT32				 nDesIdx,
														SvcCas_CaDes_t		*pstCaMgrCaDes)
{
	HERROR				 hErr;
	svcCas_CaDes_t		*pstCaDes = NULL;
	svcCas_RawPool_t	*pstRawPool = SvcCas_GetSiRawPoolFromHandle(hSiRawPool);

	if (NULL == pstRawPool)
		return ERR_FAIL;


	if(pstCaMgrCaDes == NULL)
	{
		return ERR_FAIL;
	}

	hErr = svc_cas_SiGetPmtElementDescriptor(pstRawPool, eTextEnc, usTsUniqId, ulIdx, nSubIdx, TAG_CA, nDesIdx, (void **)&pstCaDes);
	if(hErr != ERR_OK)
	{
		HxLOG_Error(" svc_cas_SiGetPmtElementDescriptor() \n");
		return ERR_FAIL;
	}

	pstCaMgrCaDes->ucDesTag 	= pstCaDes->ucDesTag;
	pstCaMgrCaDes->usCaSysId	= pstCaDes->usCaSysId;
	pstCaMgrCaDes->usCaPid		= pstCaDes->usCaPid;

	hErr = svc_cas_SiFreeDescriptor((void *)pstCaDes);
	if(hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR svc_cas_SvcPortSiParseGetPmtProgramDesRaw(SvcCas_SiRawPool_h	  hSiRawPool,
												HUINT16				  usTsUniqId,
												HUINT32				  ulIdx,
												HUINT32				  ulTag,
												HUINT32				  nDesIdx,
												HUINT32				 *pulDesRawSize,
												HUINT8				**ppucDesRaw)
{
	svcCas_RawPool_t	*pstRawPool = SvcCas_GetSiRawPoolFromHandle(hSiRawPool);

	if (NULL == pstRawPool)
		return ERR_FAIL;

	return svc_cas_SiGetPmtProgramDesRaw(pstRawPool, usTsUniqId, ulIdx, ulTag, nDesIdx, pulDesRawSize, ppucDesRaw);
}

HERROR svc_cas_SvcPortSiParseGetPmtElementDesRaw(SvcCas_SiRawPool_h	  hSiRawPool,
												HUINT16 			  usTsUniqId,
												HUINT32				  ulIdx,
												HUINT32				  nSubIdx,
												HUINT32				  ulTag,
												HUINT32				  ulDesIdx,
												HUINT32				 *pulDesRawSize,
												HUINT8				**ppucDesRaw)
{
	svcCas_RawPool_t	*pstRawPool = SvcCas_GetSiRawPoolFromHandle(hSiRawPool);

	if (NULL == pstRawPool)
		return ERR_FAIL;

	return svc_cas_SiGetPmtElementDesRaw(pstRawPool, usTsUniqId, ulIdx, nSubIdx, ulTag, ulDesIdx, pulDesRawSize, ppucDesRaw);
}

HERROR svc_cas_SvcPortSiParseCountCatSection(SvcCas_SiRawPool_h hSiRawPool, HUINT16 usTsUniqId, HUINT32 *pulCnt)
{
	svcCas_RawPool_t	*pstRawPool = SvcCas_GetSiRawPoolFromHandle(hSiRawPool);

	if (NULL == pstRawPool)
		return ERR_FAIL;

	return svc_cas_SiCountCatSection(pstRawPool, usTsUniqId, pulCnt);
}

HERROR svc_cas_SvcPortSiParseCountCatDescriptor(SvcCas_SiRawPool_h		 hSiRawPool,
													HUINT16				 usTsUniqId,
													HUINT32				 ulIdx,
													HUINT32				 ulTag,
													HUINT32				*pnCnt)
{
	svcCas_RawPool_t	*pstRawPool = SvcCas_GetSiRawPoolFromHandle(hSiRawPool);

	if (NULL == pstRawPool)
		return ERR_FAIL;

	return svc_cas_SiCountCatDescriptor(pstRawPool, usTsUniqId, ulIdx, ulTag, pnCnt);
}

HERROR svc_cas_SvcPortSiParseGetCatDescriptor(SvcCas_SiRawPool_h	 hSiRawPool,
												SIxTextEncoding_e	 eTextEnc,
												HUINT16				 usTsUniqId,
												HUINT32				 ulIdx,
												HUINT32				 ulTag,
												HUINT32				 nDesIdx,
												SvcCas_CaDes_t		*pstCaMgrCaDes)
{
	HERROR				 hErr;
	svcCas_CaDes_t		*pstCaDes = NULL;
	svcCas_RawPool_t	*pstRawPool = SvcCas_GetSiRawPoolFromHandle(hSiRawPool);

	if (NULL == pstRawPool)
		return ERR_FAIL;

	hErr =  svc_cas_SiGetCatDescriptor(pstRawPool, eTextEnc, usTsUniqId, ulIdx, ulTag, nDesIdx, (void **)&pstCaDes);
	if(hErr != ERR_OK)
	{
		HxLOG_Error(" svc_cas_SiGetCatDescriptor() \n");
		return ERR_FAIL;
	}

	pstCaMgrCaDes->ucDesTag		= pstCaDes->ucDesTag;
	pstCaMgrCaDes->usCaSysId	= pstCaDes->usCaSysId;
	pstCaMgrCaDes->usCaPid		= pstCaDes->usCaPid;

	hErr = svc_cas_SiFreeDescriptor((void *)pstCaDes);
	if(hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR svc_cas_SvcPortSiParseFreeDescriptorRaw(HUINT8 *pucDesRaw)
{
	return svc_cas_SiFreeDescriptorRaw(pucDesRaw);
}

HERROR svc_cas_SvcPortSiParseFreeDescriptor(void *pvDes)
{
	return svc_cas_SiFreeDescriptor(pvDes);
}

HERROR svc_cas_SvcPortSiParseGetCaPidFromRaw(HUINT8 *pucRaw, SIxTextEncoding_e eTextEnc, HUINT32 idx, HUINT16 *pusCaPid)
{
	svcCas_CaDes_t *pstCaDes = (svcCas_CaDes_t *)NULL;
	HERROR	hErr = ERR_FAIL;

	hErr = svc_cas_SiGetPmtElementDescriptorFromRaw(pucRaw, eTextEnc, idx, TAG_CA, 0, (void **)&pstCaDes);
	if (hErr == ERR_OK && pstCaDes != NULL)
	{
		*pusCaPid = pstCaDes->usCaPid;
		svc_cas_SiFreeDescriptor((void *)pstCaDes);
	}

	return hErr;
}

HERROR svc_cas_SvcPortSiParseGetCaSysIdFromRaw(HUINT8 *pucRaw, SIxTextEncoding_e eTextEnc, HUINT32 idx, HUINT16 *pusCaSysId)
{
	svcCas_CaDes_t *pstCaDes = (svcCas_CaDes_t *)NULL;
	HERROR	hErr = ERR_FAIL;

	hErr = svc_cas_SiGetPmtElementDescriptorFromRaw(pucRaw, eTextEnc, idx, TAG_CA, 0, (void **)&pstCaDes);
	if (hErr == ERR_OK && pstCaDes != NULL)
	{
		*pusCaSysId = pstCaDes->usCaPid;
		svc_cas_SiFreeDescriptor((void *)pstCaDes);
	}

	return hErr;
}


HERROR svc_cas_SvcPortSiParseGetPmtElementDesRawFromRaw(HUINT8 *pucRaw, HUINT32 ulElemIdx, HUINT32 ulTag, HUINT32 nDesIdx, HUINT32 *pulDesRawSize, HUINT8 **ppucDesRaw)
{
	return svc_cas_SiGetPmtElementDesRawFromRaw(pucRaw, ulElemIdx, ulTag, nDesIdx, pulDesRawSize, ppucDesRaw);
}

#define _____SI_________________________________________________________________________________________________________

HERROR svc_cas_SvcPortSiSetDefaultRetrieveParam(SvcCas_FilterParam_t	*pstCasMgrFilterParam,
													HUINT32				 hAction,
													HUINT16				 usTsUniqId,
													HUINT32				 ulDemuxId)
{
	return svc_cas_DmxSetDefaultRetrieveParam((svcCas_SiRetrieveParam_t *)pstCasMgrFilterParam, hAction, usTsUniqId, ulDemuxId);
}

HERROR svc_cas_SvcPortSiRetrievePatSection (SvcCas_FilterParam_t	*pstCasMgrFilterParam,
											SvcCas_SiParseCb_t		 fnParse,
											SvcCas_SiFreeCb_t		 fnFree,
											HUINT16					*pusFilterId)
{
	return svc_cas_DmxRetrievePatSection((svcCas_SiRetrieveParam_t *)pstCasMgrFilterParam, fnParse, fnFree, pusFilterId);
}

HERROR svc_cas_SvcPortSiRetrievePmtSection (SvcCas_FilterParam_t	*pstCasMgrFilterParam,
											SvcCas_SiParseCb_t		 fnParse,
											SvcCas_SiFreeCb_t		 fnFree,
											HUINT16					*pusFilterId)
{
	return svc_cas_DmxRetrievePmtSection((svcCas_SiRetrieveParam_t *)pstCasMgrFilterParam, fnParse, fnFree, pusFilterId);
}

HERROR svc_cas_SvcPortSiRetrieveCatSection(	SvcCas_FilterParam_t	*pstCasMgrFilterParam,
											SvcCas_SiParseCb_t		 fnParse,
											SvcCas_SiFreeCb_t		 fnFree,
											HUINT16					*pusFilterId)
{
	return svc_cas_DmxRetrieveCatSection ((svcCas_SiRetrieveParam_t *)pstCasMgrFilterParam, fnParse, fnFree, pusFilterId);
}

HERROR svc_cas_SvcPortSiRetrieveSdtSection(SvcCas_FilterParam_t		*pstCasMgrFilterParam,
											SvcCas_SiParseCb_t		 fnParse,
											SvcCas_SiFreeCb_t		 fnFree,
											HUINT16					*pusFilterId)
{
	return svc_cas_DmxRetrieveSdtSection((svcCas_SiRetrieveParam_t *)pstCasMgrFilterParam, fnParse, fnFree, pusFilterId);
}

HERROR svc_cas_SvcPortSiRetrieveNitSection(SvcCas_FilterParam_t		*pstCasMgrFilterParam,
											SvcCas_SiParseCb_t		 fnParse,
											SvcCas_SiFreeCb_t		 fnFree,
											HUINT16					*pusFilterId)
{
	return svc_cas_DmxRetrieveNitSection((svcCas_SiRetrieveParam_t *)pstCasMgrFilterParam, fnParse, fnFree, pusFilterId);
}

HERROR svc_cas_SvcPortSiRetrieveBatSection(SvcCas_FilterParam_t		*pstCasMgrFilterParam,
											SvcCas_SiParseCb_t		 fnParse,
											SvcCas_SiFreeCb_t		 fnFree,
											HUINT16					*pusFilterId)
{
	return svc_cas_DmxRetrieveBatSection((svcCas_SiRetrieveParam_t *)pstCasMgrFilterParam, fnParse, fnFree, pusFilterId);
}

HERROR svc_cas_SvcPortSiRetrieveTdtSection(SvcCas_FilterParam_t		*pstCasMgrFilterParam,
											SvcCas_SiParseCb_t		 fnParse,
											SvcCas_SiFreeCb_t		 fnFree,
											HUINT16					*pusFilterId)
{
	return svc_cas_DmxRetrieveTdtSection((svcCas_SiRetrieveParam_t *)pstCasMgrFilterParam, fnParse, fnFree, pusFilterId);
}

HERROR svc_cas_SvcPortSiStopOneSection(HUINT16 usfilterId)
{
	return svc_cas_DmxStopOneSection(usfilterId);
}

void *svc_cas_SvcPortSiParseDescriptor(SIxTextEncoding_e eTextEnc, HUINT8 *pucRaw)
{
	return svc_cas_SiParseDescriptor(eTextEnc, pucRaw);
}

HERROR svc_cas_SvcPortSiFreeDescriptor(void *pvDes)
{
	return svc_cas_SiFreeDescriptor(pvDes);
}


HERROR svc_cas_SvcPortSiRetrieveSectionBase (SvcCas_FilterParam_t	*pstTable,
												HUINT8				 ucTableId,
												HUINT8				*pucFilterValue,
												HUINT8				*pucFilterMask,
												HUINT32				 ulFilterSize,
												HUINT32				 ulFilterTimeout,
											SvcCas_SiParseCb_t		 fnParse,
											SvcCas_SiFreeCb_t		 fnFree,
												HUINT16				*pusFilterId)
{
	return svc_cas_DmxRetrieveSectionBase((svcCas_SiRetrieveParam_t *)pstTable, ucTableId, pucFilterValue, pucFilterMask, ulFilterSize, ulFilterTimeout, fnParse, fnFree, pusFilterId);
}

#if defined(CONFIG_MW_CI_PLUS)

HERROR svc_cas_SvcPortSiGetTuningInfoFromSatDSD (void *pstDeliDes, SvcCas_TuningDesInfo_t *pstTuningParam)
{
	svcCas_SatDeliSystemDes_t *pstSatDeliDes = (svcCas_SatDeliSystemDes_t *)pstDeliDes;

	if (pstDeliDes == NULL)					{ return ERR_FAIL; }
	if (pstTuningParam == NULL)				{ return ERR_FAIL; }

	HxSTD_memset(pstTuningParam, 0, sizeof(SvcCas_TuningDesInfo_t));

	pstTuningParam->sat.ulFrequency		= pstSatDeliDes->ulFreq / 100;						// MHz
	pstTuningParam->sat.ulSymbolRate	= (pstSatDeliDes->ulSymbolRate / 10);		// KHz
	pstTuningParam->sat.ePolarization	= svc_cas_SvcPortSiConvertSatDeliDesToPalPolar(pstSatDeliDes->ucPolar);
	pstTuningParam->sat.eFecCodeRate	= svc_cas_SvcPortSiConvertSatDeliDesToPalFec(pstSatDeliDes->ucFECInner);
	pstTuningParam->sat.eTransSpec		= svc_cas_SvcPortSiConvertSatDeliDesToPalTransspec(pstSatDeliDes->ucTransSpec);
	pstTuningParam->sat.eRollOff		= svc_cas_SvcPortSiConvertSatDeliDesToPalRollOff(pstSatDeliDes->ucRollOff);
	pstTuningParam->sat.ePskMod			= svc_cas_SvcPortSiConvertSatDeliDesToPalModType(pstSatDeliDes->ucModType);
	pstTuningParam->sat.ePilot			= (pstTuningParam->sat.eTransSpec == eDxSAT_TRANS_DVBS2) ? eDxSAT_PILOT_AUTO : eDxSAT_PILOT_OFF;

	return ERR_OK;
}

HERROR svc_cas_SvcPortSiGetTuningInfoFromTerDSD (void *pstDeliDes, SvcCas_TuningDesInfo_t *pstTuningParam)
{
	svcCas_TerDeliSystemDes_t *pstTerDeliDes = (svcCas_TerDeliSystemDes_t *)pstDeliDes;

	if (pstTerDeliDes == NULL)					{ return ERR_FAIL; }
	if (pstTuningParam == NULL)				{ return ERR_FAIL; }

	HxSTD_memset(pstTuningParam, 0, sizeof(SvcCas_TuningDesInfo_t));

	// Hierarchy information이 0 이외에 priority가 0이면 LP, 그 외의 경우에는 HP
	if (pstTerDeliDes->ucPriority == 0 && pstTerDeliDes->ucHierInfo != 0)
	{
		pstTuningParam->ter.ulFreq		 	= pstTerDeliDes->ulFreq / 100;
		pstTuningParam->ter.ucStream	 	= eDxTER_STREAM_LP;			// LP
		pstTuningParam->ter.ucCodeRate	 	= svc_cas_SvcPortSiConvertTerDeliDesToPalCodeRate((HUINT32)pstTerDeliDes->ucCodeRateLPStream);
		pstTuningParam->ter.ucTransMode 	= svc_cas_SvcPortSiConvertTerDeliDesToPalTransMode((HUINT32)pstTerDeliDes->ucTransmissionMode);
		pstTuningParam->ter.ucGuardInterval	= svc_cas_SvcPortSiConvertTerDeliDesToPalGuardInterval((HUINT32)pstTerDeliDes->ucGuardInterval);
		pstTuningParam->ter.ucConstellation = svc_cas_SvcPortSiConvertTerDeliDesToPalConstellation((HUINT32)pstTerDeliDes->ucConstellation);
		pstTuningParam->ter.ucHierachy	 	= svc_cas_SvcPortSiConvertTerDeliDesToPalHierarchy((HUINT32)pstTerDeliDes->ucHierInfo);
		pstTuningParam->ter.ucOffset	 	= 0;
		pstTuningParam->ter.ucBandwidth 	= svc_cas_SvcPortSiConvertTerDeliDesToPalBandWidth((HUINT32)pstTerDeliDes->ucBandWidth);
	}
	else
	{
		pstTuningParam->ter.ulFreq			= pstTerDeliDes->ulFreq / 100;
		pstTuningParam->ter.ucStream		= eDxTER_STREAM_HP;			// HP
		pstTuningParam->ter.ucCodeRate		= svc_cas_SvcPortSiConvertTerDeliDesToPalCodeRate((HUINT32)pstTerDeliDes->ucCodeRateHPStream);
		pstTuningParam->ter.ucTransMode 	= svc_cas_SvcPortSiConvertTerDeliDesToPalTransMode((HUINT32)pstTerDeliDes->ucTransmissionMode);
		pstTuningParam->ter.ucGuardInterval = svc_cas_SvcPortSiConvertTerDeliDesToPalGuardInterval((HUINT32)pstTerDeliDes->ucGuardInterval);
		pstTuningParam->ter.ucConstellation = svc_cas_SvcPortSiConvertTerDeliDesToPalConstellation((HUINT32)pstTerDeliDes->ucConstellation);
		pstTuningParam->ter.ucHierachy		= svc_cas_SvcPortSiConvertTerDeliDesToPalHierarchy((HUINT32)pstTerDeliDes->ucHierInfo);
		pstTuningParam->ter.ucOffset		= 0;
		pstTuningParam->ter.ucBandwidth 	= svc_cas_SvcPortSiConvertTerDeliDesToPalBandWidth((HUINT32)pstTerDeliDes->ucBandWidth);
	}

	return ERR_OK;
}

HERROR svc_cas_SvcPortSiGetTuningInfoFromCabDSD(void *pstDeliDes, SvcCas_TuningDesInfo_t *pstTuningParam)
{
	svcCas_CabDeliSystemDes_t *pstCabDeliDes = (svcCas_CabDeliSystemDes_t *)pstDeliDes;
	HUINT32	ulValue;
	HERROR	hErr = ERR_FAIL;

	if (pstCabDeliDes == NULL)				{ return ERR_FAIL; }
	if (pstTuningParam == NULL)				{ return ERR_FAIL; }

	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_CAB_SRCH_NETWORK_ID, &ulValue, 0);
	if (hErr != ERR_OK)						{ return ERR_FAIL; }

	HxSTD_memset(pstTuningParam, 0, sizeof(SvcCas_TuningDesInfo_t));

	pstTuningParam->cab.ulFreq			= pstCabDeliDes->ulFreq / 10;						// KHz
	pstTuningParam->cab.usSR			= (HUINT16)(pstCabDeliDes->ulSymbolRate / 10);		// KHz
	pstTuningParam->cab.ucCon			= svc_cas_SvcPortSiConvertCabDeliDesToPalQam(pstCabDeliDes->ucModul);
	pstTuningParam->cab.ucSpec			= eDxCAB_SPECTRUM_AUTO;
	pstTuningParam->cab.usNetworkId		= (HUINT16)ulValue;

	return ERR_OK;
}
#endif
HERROR svc_cas_SvcPortSiGetSdtSectionInfoFromRaw(HUINT8 *pucRaw, svcCas_SiSdtSectionInfo_t *pstInfo)
{
	HERROR hErr;
	svcCas_SiSdtSectionInfo_t stInfo;

	if(pucRaw == NULL || pstInfo == NULL)
	{
		HxLOG_Critical("\npucRaw = %d, pstInfo = %d\n", pucRaw, pstInfo);
		return ERR_FAIL;
	}

	hErr = svc_cas_SiGetSdtSectionInfoFromRaw(pucRaw, &stInfo);

	if(hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return hErr;
	}

	if(svc_cas_SvcPortSiConvertSdtSectionInfoPalToRm((const svcCas_SiSdtSectionInfo_t *)&stInfo, pstInfo) != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR svc_cas_SvcPortSiCountSdtServiceFromRaw(HUINT8 *pucRaw, HUINT32 *pulCnt)
{
	return svc_cas_SiCountSdtServiceFromRaw (pucRaw, pulCnt);
}

HERROR svc_cas_SvcPortSiGetSdtServiceInfoFromRaw(HUINT8 *pucRaw, HUINT32 ulSvcIdx, SvcCas_StdServiceInfo_t *pstInfo)
{
	HERROR				hErr;
	svcCas_SiSdtServiceInfo_t	stInfo;

	hErr = svc_cas_SiGetSdtServiceInfoFromRaw(pucRaw, ulSvcIdx, &stInfo);
	if(hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	hErr = svc_cas_SvcPortSiConvertSdtServiceInfoPalToRM((const svcCas_SiSdtServiceInfo_t *)&stInfo, pstInfo);
	if(hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR svc_cas_SvcPortSiCountSdtServiceDescriptorFromRaw(HUINT8 *pucRaw, HUINT32 ulSvcIdx, HUINT32 ulTag, HUINT32 *pnDesCnt)
{
	return svc_cas_SiCountSdtServiceDescriptorFromRaw(pucRaw, ulSvcIdx, ulTag, pnDesCnt);
}

HERROR svc_cas_SvcPortSiGetSdtServiceDescriptorFromRaw(HUINT8 *pucRaw, SIxTextEncoding_e eTextEnc, HUINT32 ulSvcIdx, HUINT32 ulTag, HUINT32 ulDesIdx, void **ppvDes)
{
	return svc_cas_SiGetSdtServiceDescriptorFromRaw(pucRaw, eTextEnc, ulSvcIdx, ulTag, ulDesIdx, ppvDes);
}

#define _____PVR________________________________________________________________________________________________________

#if defined(CONFIG_MW_MM_PVR)
HERROR svc_cas_SvcPortPvrGetService(Handle_t hAction, SvcCas_PvrSvcInfo_t *pstCasPvrSvcInfo)
{
	HERROR				hErr;
	SvcMeta_Service_t	stMwPvrSvcInfo;

	HxSTD_memset(&stMwPvrSvcInfo, 0, sizeof(SvcMeta_Service_t));

	if(pstCasPvrSvcInfo == NULL)
	{
		return ERR_FAIL;
	}

	/* get a service by hAction */
	{
		HUINT32 ulContentId;

		hErr = SVC_META_GetContentIdByhAction(hAction, &ulContentId );
		if( ERR_OK == hErr )
			hErr = SVC_META_GetServiceInfo(ulContentId, &stMwPvrSvcInfo );
	}

	if(hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	pstCasPvrSvcInfo->usTsId		= stMwPvrSvcInfo.usTsId;
	pstCasPvrSvcInfo->usOnId		= stMwPvrSvcInfo.usOnId;
	pstCasPvrSvcInfo->usSvcId		= stMwPvrSvcInfo.usSvcId;
	pstCasPvrSvcInfo->usPmtPid		= stMwPvrSvcInfo.usPmtPid;

	return ERR_OK;
}

HERROR svc_cas_SvcPortPvrGetPvrInfoFromMeta(Handle_t hAction, SvcCas_PvrSvcInfo_t *pstCasPvrInfo)
{
	HUINT32			ulContentsId;
	SvcMeta_Record_t	stRecordInfo;

	if(SVC_META_GetContentIdByhAction(hAction, &ulContentsId) != ERR_OK)
	{
		return ERR_FAIL;
	}

	if(SVC_META_GetRecordInfo(ulContentsId, &stRecordInfo) != ERR_OK)
	{
		return ERR_FAIL;
	}

#if defined(CONFIG_MW_CI)
	pstCasPvrInfo->eInstType	= (SvcCas_InstanceType_e)stRecordInfo.ulInstType;
	pstCasPvrInfo->usSlotId		= stRecordInfo.usSlotId;
#endif

	return ERR_OK;
}

HERROR svc_cas_SvcPortPvrUpdatePvrInfoToMeta(Handle_t hAction, SvcCas_PvrSvcInfo_t *pstCasPvrInfo)
{
	HUINT32			ulContentsId;
	SvcMeta_Record_t	stRecordInfo;

	if(SVC_META_GetContentIdByhAction(hAction, &ulContentsId) != ERR_OK)
	{
		return ERR_FAIL;
	}

	if(SVC_META_GetRecordInfo(ulContentsId, &stRecordInfo) != ERR_OK)
	{
		return ERR_FAIL;
	}

#if defined(CONFIG_MW_CI)
	stRecordInfo.ulInstType = (HUINT32)pstCasPvrInfo->eInstType;
	stRecordInfo.usSlotId = pstCasPvrInfo->usSlotId;
#endif

	if(SVC_META_SetRecordInfo(ulContentsId, &stRecordInfo) != ERR_OK)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}
#endif

#define _____UTIL________________________________________________________________________________________________________

void svc_cas_SvcPortUtilPostMsgToAp(HINT32 nMsg, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{

	if (s_fnPostMsgToMgr)
	{
		(* s_fnPostMsgToMgr)(nMsg, hAction, nParam1, nParam2, nParam3);
		return;
	}

	return;
}

void svc_cas_SvcPortUtilPostDataToAp(HINT32 nMsg, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, void *pvData, HUINT32 ulSize)
{
	if (s_fnPostDataToMgr)
	{
		(* s_fnPostDataToMgr)(nMsg, hAction, nParam1, nParam2, pvData, ulSize);
		return;
	}

	return;
}

HERROR	svc_cas_SvcPortUtilRegisterNotifyCallback(PostMsgToMgrCb_t fnPostMsg, PostDataToMgrCb_t fnPostData)
{
	s_fnPostMsgToMgr = fnPostMsg;
	s_fnPostDataToMgr = fnPostData;

	return ERR_OK;
}

/********************************************************************
* For Debugging
********************************************************************/

#define ___09_Debugging_APIs___

#if defined(CONFIG_DEBUG)
void	svc_cas_SvcPortSiRawPrintRawPool(SvcCas_SiRawPool_h hSiRawPool)
{
	svcCas_RawPool_t	*pstRawPool = SvcCas_GetSiRawPoolFromHandle(hSiRawPool);

	if (NULL == pstRawPool)
		return;

	svc_cas_RawPrintRawPool(pstRawPool);
	return;
}
#endif

/*********************** End of File ******************************/
