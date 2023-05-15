/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _svc_cas_mgr_svc_port.h						\n
	@brief	  cas module header file		\n

	Description: cas module. \n
	Module: MW/CAS									 					\n

	Copyright (c) 2012 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__CAS_MGR_MW_PORT_H__
#define	__CAS_MGR_MW_PORT_H__

/********************************************************************
* Header Files :  헤더 파일은 가능한 .c 에서 include 할 것 !
********************************************************************/
#define ___01_Header_Files___
#include <svc_cas.h>
#include <_svc_cas_sub_svc_api.h>

/********************************************************************
* Extern Variables / Function Prototypes
********************************************************************/
#define ___02_Extern_Vars_Func_Prototypes___


/********************************************************************
* Module Internal Definitions : #define / enum / struct / union
********************************************************************/
#define ___03_Module_Internal_Definitions___

/* SiFilterMode_t와 내용이 같아야 한다.
*/
typedef enum
{
	eCAS_SI_FILTER_MODE_Once,
	eCAS_SI_FILTER_MODE_Monitor,	/* monitor by repeat */
	eCAS_SI_FILTER_MODE_Continue
} SvcCas_SiFilterMode_e;

/*PalSef_TimeOutMode_e와 내용이 같아야 한다.
*/
typedef enum
{
	eCas_SI_FILTER_Timeout_Once = 0,	/* 최초 한번 만 발생 */
	eCas_SI_FILTER_Timeout_Repeat	/* 최초 발생 후 timeout 주기에 따라 계속 발생 */
} SvcCas_TimeOutMode_e;

/* SiFilterResult_t와 내용이 같아야 한다.
*/
typedef enum
{
	eCAS_SI_FILTER_RESULT_Complete = 1,
	eCAS_SI_FILTER_RESULT_Timeout,
	eCAS_SI_FILTER_RESULT_Error
} SvcCas_SiFilterResult_e;

typedef struct
{
	HUINT8		ucDesTag;
	HUINT16		usCaSysId;
	HUINT16		usCaPid;
} SvcCas_CaDes_t;

// SI lib API의 svcCas_SiPmtProgramInfo_t와 mapping 되는 구조체. svcCas_SiPmtProgramInfo_t구조와 동일하게 유지 해야함.
typedef struct
{
	HUINT16		usProgNum;
	HUINT8		ucVer;
	HBOOL		bCurNextInd;
	HUINT16		usPCRPid;
	HUINT32		ulCrc32;
} SvcCas_PmtProgInfo_t;

typedef void *						SvcCas_SiRawPool_h;
typedef svcCas_SiRetrieveParam_t	SvcCas_FilterParam_t;

/********************************************************************
* Module Internal Function Prototypes
********************************************************************/
#define ___04_Module_Internal_Func_Prototypes___

#define _____DB________________________________________________________________________________________________________

HUINT16 svc_cas_SvcPortDbGetAllUniqueId(void);

HERROR svc_cas_SvcPortDbGetTsInfoByServiceHandle(Handle_t hSvc, HUINT16 *pusUniqueId, HUINT16 *pusTsId, HUINT16 *pusOnId);

HERROR svc_cas_SvcPortDbGetSvcInfoByServiceHandle(Handle_t hSvc, SvcCas_DbSvcInfo_t *pstSvcInfo);

HERROR svc_cas_SvcPortDbGetPmtPidbyServiceHandle(Handle_t hSvc, HUINT16 *pusPmtPid);

HERROR svc_cas_SvcPortDbUpdateServiceInfo(Handle_t hSvc, SvcCas_DbSvcInfo_t *pstSvcInfo);

void svc_cas_SvcPortDbFreeServiceList(Handle_t *phSvcList);

HERROR svc_cas_SvcPortDbUpdatePmtPidInServiceInfo(Handle_t hSvc, HUINT16 usPmtPid);

HERROR svc_cas_SvcPortDbGetSameTpSvcHandleListByServiceHandle(Handle_t hSvc, HINT32 *pnCnt, Handle_t **pphSvcList);


#define _____RM________________________________________________________________________________________________________

HERROR svc_cas_SvcPortRmCheckIfLatestActionHandle(Handle_t hAction);

HUINT32 svc_cas_SvcPortRmGetActionId(Handle_t hAction);

HERROR svc_cas_SvcPortRmGetResourceId(HUINT32 ulActionId, SvcCas_RmItem_e eCasMgrRmItem, HUINT32 *pulResourceId);

Handle_t svc_cas_SvcPortRmMakeActionHandle(HUINT32 ulActionId, HUINT32 ulSessionVer);


#define _____SI_RAW________________________________________________________________________________________________________

SvcCas_SiRawPool_h svc_cas_SvcPortSiRawAllocPool(HUINT16 usMaxRawSec, HUINT8 *szPoolName);

HERROR svc_cas_SvcPortSiRawFreeSection(SvcCas_SiRawPool_h pCasMgrSiRawPool, HUINT16 usUniqueId, SiTableType_e eSiTableType, HBOOL bUseExtId, HUINT32 ulExtId);

HERROR svc_cas_SvcPortSiRawAddRawSection(SvcCas_SiRawPool_h pCasMgrSiRawPool, HUINT16 usUniqueId, SiTableType_e eSiTableType, HUINT8 *pucRawSec);

HUINT8 **svc_cas_SvcPortSiRawGetRawArrays(SvcCas_SiRawPool_h	 pCasMgrSiRawPool,
											HUINT16 			 usTsUniqId,
											SiTableType_e 		 eSiType,
											HBOOL				 bUseExtId,
											HUINT16				 usExtId,
											HUINT32				*pulArrayLen);

HERROR svc_cas_SvcPortSiRawFreeRawArrays(HUINT8 **ppucRawArrays);

#define _____SI_PARSE_________________________________________________________________________________________________________

HERROR svc_cas_SvcPortSiParseGetPmtPidByServiceId(SvcCas_SiRawPool_h	 pCasMgrSiRawPool,
													HUINT16				 usTsUniqId,
													HUINT16				 usSvcId,
													HUINT16				*pusPmtPid);

HERROR svc_cas_SvcPortSiParseGetPmtIndexByServiceId(SvcCas_SiRawPool_h		 pCasMgrSiRawPool,
														HUINT16				 usTsUniqId,
														HUINT16				 usSvcId,
														HUINT32				*pulPmtIndex);

HERROR svc_cas_SvcPortSiParseGetPmtProgramInfoByServiceId(SvcCas_SiRawPool_h		 pstPool,
															HUINT16 				 usTsUniqId,
															HUINT16					 usSvcId,
															SvcCas_PmtProgInfo_t	*pstInfoDest);

HERROR svc_cas_SvcPortSiParseGetPmtProgramInfoFromRaw(HUINT8 *pucRaw, SvcCas_PmtProgInfo_t *pstInfoDest);

HERROR svc_cas_SvcPortSiParseCountPmtElementFromRaw(HUINT8 *pucRaw, HUINT32 *pnCnt);

HERROR svc_cas_SvcPortSiParseCountPmtElement(SvcCas_SiRawPool_h pstPool, HUINT16 usTsUniqId, HUINT32 ulIdx, HUINT32 *pnCnt);

HERROR svc_cas_SvcPortSiParseGetPmtElementInfoFromRaw(HUINT8 *pucRaw, HUINT32 ulElemIdx, svcCas_SiPmtElementInfo_t *pstInfoDest);

HERROR svc_cas_SvcPortSiParseGetPmtElementInfo(SvcCas_SiRawPool_h		pstPool,
												HUINT16					usTsUniqId,
												HUINT32					ulIdx,
												HUINT32					nSubIdx,
												svcCas_SiPmtElementInfo_t	*pstInfo);

HERROR svc_cas_SvcPortSiParseCountPmtProgramDescriptor(SvcCas_SiRawPool_h	 pstPool,
														HUINT16				 usTsUniqId,
														HUINT32				 ulIdx,
														HUINT32				 ulTag,
														HUINT32				*pnCnt);

HERROR svc_cas_SvcPortSiParseCountPmtElementDescriptor(SvcCas_SiRawPool_h	 pstPool,
														HUINT16				 usTsUniqId,
														HUINT32				 ulIdx,
														HUINT32				 nSubIdx,
														HUINT32				 ulTag,
														HUINT32				*pnDesCnt);

HERROR svc_cas_SvcPortSiParseGetPmtProgramCaDescriptor(SvcCas_SiRawPool_h	 pstPool,
														SIxTextEncoding_e	 eTextEnc,
														HUINT16				 usTsUniqId,
														HUINT32				 ulIdx,
														HUINT32				 nDesIdx,
														SvcCas_CaDes_t 		*pstCaMgrCaDes);

HERROR svc_cas_SvcPortSiParseGetPmtElementCaDescriptor(SvcCas_SiRawPool_h	 pstPool,
														SIxTextEncoding_e	 eTextEnc,
														HUINT16				 usTsUniqId,
														HUINT32				 ulIdx,
														HUINT32				 nSubIdx,
														HUINT32				 nDesIdx,
														SvcCas_CaDes_t		*pstCaMgrCaDes);

HERROR svc_cas_SvcPortSiParseGetPmtProgramDesRaw (SvcCas_SiRawPool_h	  pstPool,
													HUINT16				  usTsUniqId,
													HUINT32				  ulIdx,
													HUINT32				  ulTag,
													HUINT32				  nDesIdx,
													HUINT32				 *pulDesRawSize,
													HUINT8				**ppucDesRaw);

HERROR svc_cas_SvcPortSiParseGetPmtElementDesRaw (SvcCas_SiRawPool_h	  pstPool,
													HUINT16 			  usTsUniqId,
													HUINT32				  ulIdx,
													HUINT32				  nSubIdx,
													HUINT32				  ulTag,
													HUINT32				  ulDesIdx,
													HUINT32				 *pulDesRawSize,
													HUINT8				**ppucDesRaw);

HERROR svc_cas_SvcPortSiParseCountCatSection(SvcCas_SiRawPool_h pstPool, HUINT16 usTsUniqId, HUINT32 *pulCnt);

HERROR svc_cas_SvcPortSiParseCountCatDescriptor (SvcCas_SiRawPool_h	 pstPool,
												HUINT16				 usTsUniqId,
												HUINT32				 ulIdx,
												HUINT32				 ulTag,
												HUINT32				*pnCnt);

HERROR svc_cas_SvcPortSiParseGetCatDescriptor(SvcCas_SiRawPool_h	 pstPool,
												SIxTextEncoding_e	 eTextEnc,
												HUINT16				 usTsUniqId,
												HUINT32				 ulIdx,
												HUINT32				 ulTag,
												HUINT32				 nDesIdx,
												SvcCas_CaDes_t		*pstCaMgrCaDes);

HERROR svc_cas_SvcPortSiParseFreeDescriptorRaw (HUINT8 *pucDesRaw);

HERROR svc_cas_SvcPortSiParseFreeDescriptor(void *pvDes);

HERROR svc_cas_SvcPortSiParseGetCaPidFromRaw(HUINT8 *pucRaw, SIxTextEncoding_e eTextEnc, HUINT32 idx, HUINT16 *pusCaPid);

HERROR svc_cas_SvcPortSiParseGetCaSysIdFromRaw(HUINT8 *pucRaw, SIxTextEncoding_e eTextEnc, HUINT32 idx, HUINT16 *pusCaSysId);

HERROR svc_cas_SvcPortSiParseGetPmtElementDesRawFromRaw(HUINT8 *pucRaw, HUINT32 ulElemIdx, HUINT32 ulTag, HUINT32 nDesIdx, HUINT32 *pulDesRawSize, HUINT8 **ppucDesRaw);


#define _____SI_________________________________________________________________________________________________________

HERROR svc_cas_SvcPortSiSetDefaultRetrieveParam(SvcCas_FilterParam_t	*pstCasMgrFilterParam,
												HUINT32 				 hAction,
												HUINT16					 usTsUniqId,
												HUINT32					 ulDemuxId);

HERROR svc_cas_SvcPortSiRetrievePatSection (SvcCas_FilterParam_t	*pstCasMgrFilterParam,
											SvcCas_SiParseCb_t		 fnParse,
											SvcCas_SiFreeCb_t		 fnFree,
											HUINT16					*pusFilterId);

HERROR svc_cas_SvcPortSiRetrievePmtSection (SvcCas_FilterParam_t	*pstCasMgrFilterParam,
											SvcCas_SiParseCb_t		 fnParse,
											SvcCas_SiFreeCb_t		 fnFree,
											HUINT16					*pusFilterId);

HERROR svc_cas_SvcPortSiRetrieveCatSection (SvcCas_FilterParam_t	*pstCasMgrFilterParam,
											SvcCas_SiParseCb_t		 fnParse,
											SvcCas_SiFreeCb_t		 fnFree,
											HUINT16					*pusFilterId);

HERROR svc_cas_SvcPortSiRetrieveSdtSection (SvcCas_FilterParam_t	*pstCasMgrFilterParam,
											SvcCas_SiParseCb_t		 fnParse,
											SvcCas_SiFreeCb_t		 fnFree,
											HUINT16					*pusFilterId);

HERROR svc_cas_SvcPortSiRetrieveNitSection (SvcCas_FilterParam_t	*pstCasMgrFilterParam,
											SvcCas_SiParseCb_t		 fnParse,
											SvcCas_SiFreeCb_t		 fnFree,
											HUINT16					*pusFilterId);

HERROR svc_cas_SvcPortSiRetrieveBatSection (SvcCas_FilterParam_t	*pstCasMgrFilterParam,
											SvcCas_SiParseCb_t		 fnParse,
											SvcCas_SiFreeCb_t		 fnFree,
											HUINT16					*pusFilterId);

HERROR svc_cas_SvcPortSiRetrieveTdtSection (SvcCas_FilterParam_t	*pstCasMgrFilterParam,
											SvcCas_SiParseCb_t		 fnParse,
											SvcCas_SiFreeCb_t		 fnFree,
											HUINT16					*pusFilterId);

HERROR svc_cas_SvcPortSiStopOneSection(HUINT16 usfilterId);

void *svc_cas_SvcPortSiParseDescriptor(SIxTextEncoding_e eTextEnc, HUINT8 *pucRaw);

HERROR svc_cas_SvcPortSiFreeDescriptor(void *pvDes);
HERROR svc_cas_SvcPortSiRetrieveSectionBase (SvcCas_FilterParam_t	*pstTable,
												HUINT8				 ucTableId,
												HUINT8				*pucFilterValue,
												HUINT8				*pucFilterMask,
												HUINT32				 ulFilterSize,
												HUINT32				 ulFilterTimeout,
												SvcCas_SiParseCb_t	 fnParse,
												SvcCas_SiFreeCb_t	 fnFree,
												HUINT16				*pusFilterId);


#if defined(CONFIG_MW_CI_PLUS)
HERROR svc_cas_SvcPortSiGetTuningInfoFromSatDSD(void *pstDeliDes, SvcCas_TuningDesInfo_t *pstTuningParam);

HERROR svc_cas_SvcPortSiGetTuningInfoFromTerDSD(void *pstDeliDes, SvcCas_TuningDesInfo_t *pstTuningParam);

HERROR svc_cas_SvcPortSiGetTuningInfoFromCabDSD(void *pstDeliDes, SvcCas_TuningDesInfo_t *pstTuningParam);
#endif
HERROR svc_cas_SvcPortSiGetSdtSectionInfoFromRaw(HUINT8 *pucRaw, svcCas_SiSdtSectionInfo_t *pstInfo);

HERROR svc_cas_SvcPortSiCountSdtServiceFromRaw(HUINT8 *pucRaw, HUINT32 *pulCnt);

HERROR svc_cas_SvcPortSiGetSdtServiceInfoFromRaw(HUINT8 *pucRaw, HUINT32 ulSvcIdx, SvcCas_StdServiceInfo_t *pstInfo);

HERROR svc_cas_SvcPortSiCountSdtServiceDescriptorFromRaw(HUINT8 *pucRaw, HUINT32 ulSvcIdx, HUINT32 ulTag, HUINT32 *pnDesCnt);

HERROR svc_cas_SvcPortSiGetSdtServiceDescriptorFromRaw(HUINT8 *pucRaw, SIxTextEncoding_e eTextEnc, HUINT32 ulSvcIdx, HUINT32 ulTag, HUINT32 ulDesIdx, void **ppvDes);

#define _____PVR________________________________________________________________________________________________________

HERROR svc_cas_SvcPortPvrGetService (Handle_t hAct, SvcCas_PvrSvcInfo_t *pstCasPvrSvcInfo);

HERROR svc_cas_SvcPortPvrGetPvrInfoFromMeta (Handle_t hAct, SvcCas_PvrSvcInfo_t *pstCasPvrInfo);

HERROR svc_cas_SvcPortPvrUpdatePvrInfoToMeta (Handle_t hAct, SvcCas_PvrSvcInfo_t *pstCasPvrInfo);

#define _____UTIL________________________________________________________________________________________________________

void svc_cas_SvcPortUtilPostMsgToAp(HINT32 nMsg, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);
void svc_cas_SvcPortUtilPostDataToAp(HINT32 nMsg, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, void *pvData, HUINT32 ulSize);

HERROR	svc_cas_SvcPortUtilRegisterNotifyCallback(PostMsgToMgrCb_t fnPostMsg, PostDataToMgrCb_t fnPostData);


/********************************************************************
* For Debugging
********************************************************************/

#define ___05_Debugging_APIs___

#if defined(CONFIG_DEBUG)
void	svc_cas_SvcPortSiRawPrintRawPool(SvcCas_SiRawPool_h pCasMgrSiRawPool);
#endif

#endif /* !__CAS_MGR_MW_PORT_H__ */
