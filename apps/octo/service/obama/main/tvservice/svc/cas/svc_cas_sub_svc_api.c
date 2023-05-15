/**
	@file     casmgr_subCas_mw_api.c
	@brief    casmgr_subCas_mw_api.c (CAS Service)

	Description: \n
	Module: MW/CAS \n
	Remarks :\n

	Copyright (c) 2012 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

/********************************************************************
* Header Files
********************************************************************/
#define ___01_Header_Files___

#include <octo_common.h>

#include <_svc_cas_sub_svc_api.h>
#include <_svc_cas_mgr_svc_port.h>

/********************************************************************
* For Debugging
********************************************************************/
#define ___02_Debugging___


/********************************************************************
* Local Definitions : #define / enum / struct / union
********************************************************************/
#define ___03_Local_Definitions___

/********************************************************************
* Extern Variables / Function Prototypes
********************************************************************/
#define ___04_Extern_Vars_Func_Prototypes___

/********************************************************************
* Static Variables / Function Prototypes
********************************************************************/
#define ___05_Local_Vars_Func_Prototypes___
STATIC SvcCas_SiGetTimeOutCb_t		s_fnSvcCasSiTimeout = NULL;

/********************************************************************
* Static Functions
* Naming rule : "local"_"CasMgrMain" _ "Function Name"
********************************************************************/
#define ___06_Local_APIs___

/********************************************************************
* Module Internal Functions
* Naming rule : "CasMgrMain" _ "Function Name"
********************************************************************/
#define ___07_Module_Internal_APIs___

/********************************************************************
* Global Public Functions
* Naming rule : "Module Prefix" _ "Function Name"
********************************************************************/
#define ___08_Global_Public_APIs___

#define _____DB________________________________________________________________________________________________________

HUINT16 svc_cas_SubSvcDbGetAllUniqueId(void)
{
	return svc_cas_SvcPortDbGetAllUniqueId();
}

HERROR svc_cas_SubSvcDbGetTsInfoByServiceHandle(Handle_t hSvc, HUINT16 *pusUniqueId, HUINT16 *pusTsId, HUINT16 *pusOnId)
{
	return svc_cas_SvcPortDbGetTsInfoByServiceHandle(hSvc, pusUniqueId, pusTsId, pusOnId);
}

HERROR svc_cas_SubSvcDbGetSvcInfoByServiceHandle(Handle_t hSvc, SvcCas_DbSvcInfo_t *pstSvcInfo)
{
	return svc_cas_SvcPortDbGetSvcInfoByServiceHandle(hSvc, pstSvcInfo);
}

HERROR svc_cas_SubSvcDbUpdateServiceInfo(Handle_t hSvc, SvcCas_DbSvcInfo_t *pstSvcInfo)
{
	return svc_cas_SvcPortDbUpdateServiceInfo(hSvc, pstSvcInfo);
}

HERROR svc_cas_SubSvcDbGetSameTpSvcHandleListByServiceHandle(Handle_t hSvc, HINT32 *pnCnt, Handle_t **pphSvcList)
{
	return svc_cas_SvcPortDbGetSameTpSvcHandleListByServiceHandle(hSvc, pnCnt, pphSvcList);
}

void svc_cas_SubSvcDbFreeServiceList(Handle_t *phSvcList)
{
	svc_cas_SvcPortDbFreeServiceList(phSvcList);
}

#define _____RM________________________________________________________________________________________________________

HUINT32	svc_cas_SubSvcRmGetActionId(Handle_t hAction)
{
	return svc_cas_SvcPortRmGetActionId(hAction);
}

HERROR svc_cas_SubSvcRmGetResourceId(HUINT32 ulActionId, SvcCas_RmItem_e eCasMgrRmItem, HUINT32 *pulResourceId)
{
	return svc_cas_SvcPortRmGetResourceId(ulActionId, eCasMgrRmItem, pulResourceId);
}


#define _____SI________________________________________________________________________________________________________

HERROR svc_cas_SubSvcSiRegisterGetTableTimeoutCallback(SvcCas_SiGetTimeOutCb_t fnGetTableTimeout)
{
	s_fnSvcCasSiTimeout = fnGetTableTimeout;
	return ERR_OK;
}

HERROR svc_cas_SiGetTableTimeOut(Handle_t hAction, SiTableType_e eTableType, HUINT32 *pulTimeout)
{
	if (NULL == pulTimeout)
		return ERR_FAIL;

	if (s_fnSvcCasSiTimeout)
	{
		return (* s_fnSvcCasSiTimeout)(hAction, eTableType, pulTimeout);
	}

	return ERR_FAIL;
}

void *svc_cas_SubSvcSiParseDescriptor(SIxTextEncoding_e eTextEnc, HUINT8 *pucRaw)
{
	return svc_cas_SvcPortSiParseDescriptor(eTextEnc, pucRaw);
}

HERROR svc_cas_SubSvcSiFreeDescriptor (void *pvDes)
{
	return svc_cas_SvcPortSiFreeDescriptor (pvDes);
}

HERROR svc_cas_SubSvcSiFreeDescriptorRaw (HUINT8 *pucDesRaw)
{
	return svc_cas_SvcPortSiParseFreeDescriptorRaw (pucDesRaw);
}


HERROR svc_cas_SubSvcSiGetPmtProgramInfoFromRaw(HUINT8 *pucRaw, SvcCas_SubPmtProgInfo_t *pstInfoDest)
{
	SvcCas_PmtProgInfo_t 	stInfoSrc;
	HERROR					hErr = ERR_FAIL;

	hErr = svc_cas_SvcPortSiParseGetPmtProgramInfoFromRaw(pucRaw, &stInfoSrc);

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

HERROR svc_cas_SubSvcSiCountPmtElementFromRaw(HUINT8 *pucRaw, HUINT32 *pnCnt)
{
	return svc_cas_SvcPortSiParseCountPmtElementFromRaw(pucRaw, pnCnt);
}

HERROR svc_cas_SubSvcSiGetPmtElementInfoFromRaw (HUINT8 *pucRaw, HUINT32 ulElemIdx, SvcCas_SubPmtElemInfo_t *pstInfoDest)
{
	svcCas_SiPmtElementInfo_t 	stInfoSrc;
	HERROR					hErr = ERR_FAIL;

	hErr = svc_cas_SvcPortSiParseGetPmtElementInfoFromRaw (pucRaw, ulElemIdx, &stInfoSrc);

	if(hErr == ERR_OK)
	{
		pstInfoDest->ucStreamType 	= stInfoSrc.ucStreamType;
		pstInfoDest->usElemPid		= stInfoSrc.usElemPid;
	}
	return hErr;
}
#if defined(CONFIG_MW_CI_PLUS)
HERROR svc_cas_SubSvcSiGetCaPidFromRaw(HUINT8 *pucRaw, SIxTextEncoding_e eTextEnc, HUINT32 idx, HUINT16 *pusCaPid)
{
	return svc_cas_SvcPortSiParseGetCaPidFromRaw(pucRaw, eTextEnc, idx, pusCaPid);
}

HERROR svc_cas_SubSvcSiGetCaSysIdFromRaw(HUINT8 *pucRaw, SIxTextEncoding_e eTextEnc, HUINT32 idx, HUINT16 *pusCaSysId)
{
	return svc_cas_SvcPortSiParseGetCaSysIdFromRaw(pucRaw, eTextEnc, idx, pusCaSysId);
}

HERROR svc_cas_SubSvcSiGetPmtElementDesRawFromRaw(HUINT8 *pucRaw, HUINT32 ulElemIdx, HUINT32 ulTag, HUINT32 nDesIdx, HUINT32 *pulDesRawSize, HUINT8 **ppucDesRaw)
{
	return svc_cas_SvcPortSiParseGetPmtElementDesRawFromRaw(pucRaw, ulElemIdx, ulTag, nDesIdx, pulDesRawSize, ppucDesRaw);
}

HERROR svc_cas_SubSvcSiGetTuningInfoFromSatDSD (void *pstDeliDes, SvcCas_TuningDesInfo_t *pstTuningParam)
{
	return svc_cas_SvcPortSiGetTuningInfoFromSatDSD(pstDeliDes, pstTuningParam);
}

HERROR svc_cas_SubSvcSiGetTuningInfoFromTerDSD(void *pstDeliDes, SvcCas_TuningDesInfo_t *pstTuningParam)
{
	return svc_cas_SvcPortSiGetTuningInfoFromTerDSD(pstDeliDes, pstTuningParam);
}

HERROR svc_cas_SubSvcSiGetTuningInfoFromCabDSD (void *pstDeliDes, SvcCas_TuningDesInfo_t *pstTuningParam)
{
	return svc_cas_SvcPortSiGetTuningInfoFromCabDSD(pstDeliDes, pstTuningParam);
}
#endif
HERROR svc_cas_SubSvcSiGetSdtSectionInfoFromRaw (HUINT8 *pucRaw, svcCas_SiSdtSectionInfo_t *pstInfo)
{
	return svc_cas_SvcPortSiGetSdtSectionInfoFromRaw(pucRaw, pstInfo);
}

HERROR svc_cas_SubSvcSiCountSdtServiceFromRaw (HUINT8 *pucRaw, HUINT32 *pulCnt)
{
	return svc_cas_SvcPortSiCountSdtServiceFromRaw(pucRaw, pulCnt);
}

HERROR svc_cas_SubSvcSiGetSdtServiceInfoFromRaw(HUINT8 *pucRaw, HUINT32 ulSvcIdx, SvcCas_StdServiceInfo_t *pstInfo)
{
	return svc_cas_SvcPortSiGetSdtServiceInfoFromRaw(pucRaw, ulSvcIdx, pstInfo);
}

HERROR svc_cas_SubSvcSiCountSdtServiceDescriptorFromRaw(HUINT8 *pucRaw, HUINT32 ulSvcIdx, HUINT32 ulTag, HUINT32 *pnDesCnt)
{
	return svc_cas_SvcPortSiCountSdtServiceDescriptorFromRaw(pucRaw, ulSvcIdx, ulTag, pnDesCnt);
}

HERROR svc_cas_SubSvcSiGetSdtServiceDescriptorFromRaw(HUINT8 *pucRaw, SIxTextEncoding_e eTextEnc, HUINT32 ulSvcIdx, HUINT32 ulTag, HUINT32 ulDesIdx, void **ppvDes)
{
	return svc_cas_SvcPortSiGetSdtServiceDescriptorFromRaw(pucRaw, eTextEnc, ulSvcIdx, ulTag, ulDesIdx, ppvDes);
}

#define _____PVR________________________________________________________________________________________________________

#if defined(CONFIG_MW_MM_PVR)
HERROR svc_cas_SubSvcPvrGetService(Handle_t hAct, SvcCas_PvrSvcInfo_t *pstCasPvrSvcInfo)
{
	return svc_cas_SvcPortPvrGetService(hAct, pstCasPvrSvcInfo);
}

HERROR svc_cas_SubSvcPvrGetPvrInfoFromMeta(Handle_t hAct, SvcCas_PvrSvcInfo_t *pstCasPvrInfo)
{
	return svc_cas_SvcPortPvrGetPvrInfoFromMeta(hAct, pstCasPvrInfo);
}

HERROR svc_cas_SubSvcPvrUpdatePvrInfoToMeta(Handle_t hAct, SvcCas_PvrSvcInfo_t *pstCasPvrInfo)
{
	return svc_cas_SvcPortPvrUpdatePvrInfoToMeta(hAct, pstCasPvrInfo);
}
#endif

#define _____SI________________________________________________________________________________________________________

HERROR svc_cas_SubSvcDmxStopOneSection (HUINT16 usfilterId)
{
	return svc_cas_SvcPortSiStopOneSection(usfilterId);
}

HERROR svc_cas_SubSvcDmxRetrieveSectionBase (svcCas_SiRetrieveParam_t *pstTable,
									HUINT8 ucTableId,
									HUINT8 *pucFilterValue,
									HUINT8 *pucFilterMask,
									HUINT32 ulFilterSize,
									HUINT32 ulFilterTimeout,
									void *pfnParse,
									void *pfnFree,
									HUINT16 *pusFilterId)
{
	return svc_cas_SvcPortSiRetrieveSectionBase (pstTable,	 ucTableId, pucFilterValue, pucFilterMask, ulFilterSize, ulFilterTimeout, pfnParse, pfnFree, pusFilterId);
}

HERROR svc_cas_SubSvcDmxSetDefaultRetrieveParam (svcCas_SiRetrieveParam_t *pRetrieveParam,
														HUINT32 hAction,
														HUINT16 usTsUniqId,
														HUINT32 ulDemuxId)
{
	return svc_cas_SvcPortSiSetDefaultRetrieveParam (pRetrieveParam, hAction, usTsUniqId,	ulDemuxId);
}


#define _____UTIL________________________________________________________________________________________________________

/*********************** End of File ******************************/
