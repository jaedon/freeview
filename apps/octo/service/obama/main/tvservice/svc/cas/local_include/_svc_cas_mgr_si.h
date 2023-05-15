/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _svc_cas_mgr_si.h						\n
	@brief	  cas module header file		\n

	Description: cas module. \n
	Module: MW/CAS									 					\n

	Copyright (c) 2012 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__CAS_MRG_SI_H__
#define	__CAS_MRG_SI_H__

/********************************************************************
* Header Files :  헤더 파일은 가능한 .c 에서 include 할 것 !
********************************************************************/
#define ___01_Header_Files___
#include <svc_cas.h>

#include "_svc_cas_mgr_main.h"

/********************************************************************
* Extern Variables / Function Prototypes
********************************************************************/
#define ___02_Extern_Vars_Func_Prototypes___


/********************************************************************
* Module Internal Definitions : #define / enum / struct / union
********************************************************************/
#define ___03_Module_Internal_Definitions___

typedef enum
{
	eCAS_FILTER_HEAD_Act = 0,
	eCAS_FILTER_HEAD_Oth = 1
} SvcCas_FilterHead_e;

/********************************************************************
* Module Internal Function Prototypes
********************************************************************/
#define ___04_Module_Internal_Func_Prototypes___


HERROR svc_cas_SiInitModule(void);
HERROR svc_cas_SiUpdateTableState(SvcCas_Context_t *pstContext, SiTableType_e eTblType, SvcCas_SiTableState_e eTblState);

#define _____PAT________________________________________________________________________________________________________

void svc_cas_SiParsePat(HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, Handle_t hAction);
void svc_cas_SiFreePat(HUINT32 ulTsUniqId, HUINT32 ulTableId, HUINT32 ulExternalValue, HUINT32 ulParam2);
HERROR svc_cas_SiStartPatFilter(SvcCas_Context_t *pstContext);
HERROR svc_cas_SiStopPatFilter(SvcCas_Context_t *pstContext);
HERROR svc_cas_SiUpdatePatInfo(SvcCas_Context_t *pstContext);

#define _____PMT________________________________________________________________________________________________________

void svc_cas_SiParsePmt(HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, Handle_t hAction);
void svc_cas_SiFreePmt(HUINT32 ulTsUniqId, HUINT32 ulTableId, HUINT32 ulExternalValue, HUINT32 ulParam2);
HERROR svc_cas_SiStartPmtFilter(SvcCas_Context_t *pstContext);
HERROR svc_cas_SiStopPmtFilter(SvcCas_Context_t *pstContext);
HERROR svc_cas_SiFreePmtInfo(SvcCas_PmtInfo_t *pstPmtInfo);
HERROR svc_cas_SiUpdatePmtInfo(SvcCas_Context_t *pstContext, SvcCas_PmtInfo_t *pstPmtInfo);
HERROR CasMgrSI_DefineChannelType(SvcCas_Context_t *pstContext, SvcCas_ChannelType_e *eChType);

#define _____CAT________________________________________________________________________________________________________

#if defined (CAS_SUPPORT_CAT)
void svc_cas_SiParseCat(HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, Handle_t hAction);
void svc_cas_SiFreeCat(HUINT32 ulTsUniqId, HUINT32 ulTableId, HUINT32 ulExternalValue, HUINT32 ulParam2);
HERROR svc_cas_SiStartCatFilter(SvcCas_Context_t *pstContext);
HERROR svc_cas_SiStopCatFilter(SvcCas_Context_t *pstContext);
HERROR svc_cas_SiUpdateCatInfo(SvcCas_Context_t *pstContext);
#endif

#define _____SDT________________________________________________________________________________________________________

#if defined (CAS_SUPPORT_SDT)
HERROR svc_cas_SiUpdateSdtRawData(SvcCas_Context_t *pstContext);
void svc_cas_SiParseSdt(HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, Handle_t hAction);
void svc_cas_SiFreeSdt(HUINT32 ulTsUniqId, HUINT32 ulTableId, HUINT32 ulExternalValue, HUINT32 ulParam2);
HERROR svc_cas_SiStartSdtFilter(SvcCas_Context_t *pstContext);
HERROR svc_cas_SiStopSdtFilter(SvcCas_Context_t *pstContext);
#endif	// CAS_SUPPORT_SDT

#define _____NIT________________________________________________________________________________________________________

#if defined(CAS_SUPPORT_NIT)
HERROR svc_cas_SiFreeNitRaw(SvcCas_Context_t *pstContext);
HERROR svc_cas_SiUpdateNitRawData(SvcCas_Context_t *pstContext);
void svc_cas_SiParseNit(HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, Handle_t hAction);
void svc_cas_SiFreeNit(HUINT32 ulTsUniqId, HUINT32 ulTableId, HUINT32 ulExternalValue, HUINT32 ulParam2);
HERROR svc_cas_SiStartNitFilter(SvcCas_Context_t *pstContext);
HERROR svc_cas_SiStopNitFilter(SvcCas_Context_t *pstContext);
#endif

#define _____BAT________________________________________________________________________________________________________

#if defined(CAS_SUPPORT_BAT)
HERROR svc_cas_SiUpdateBatRawData(SvcCas_Context_t *pstContext);
void svc_cas_SiParseBat(HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, Handle_t hAction);
void svc_cas_SiFreeBatRaw(SvcCas_Context_t *pstContext);
void svc_cas_SiFreeBat(HUINT32 ulTsUniqId, HUINT32 ulTableId, HUINT32 ulExternalValue, HUINT32 ulParam2);
HERROR svc_cas_SiStartBatFilter(SvcCas_Context_t *pstContext);
HERROR svc_cas_SiStopBatFilter(SvcCas_Context_t *pstContext);
#endif

#define _____EIT________________________________________________________________________________________________________

#if defined(CAS_SUPPORT_EIT)
HERROR svc_cas_SiFreeEitRaw(SvcCas_Context_t *pstContext);
HERROR svc_cas_SiUpdateEitRawData(SvcCas_Context_t *pstContext);
void svc_cas_SiParseEit(HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, Handle_t hAction);
void svc_cas_SiFreeEit(HUINT32 ulTsUniqId, HUINT32 ulTableId, HUINT32 ulExternalValue, HUINT32 ulParam2);
HERROR svc_cas_SiStartEitFilter(SvcCas_Context_t *pstContext);
HERROR svc_cas_SiStopEitFilter(SvcCas_Context_t *pstContext);
#endif

#define _____TDT_TOT________________________________________________________________________________________________________

#if defined(CAS_SUPPORT_TDT)
HERROR svc_cas_SiUpdateTdtRawData(SvcCas_Context_t *pstContext);
void svc_cas_SiParseTdt(HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, Handle_t hAction);
void svc_cas_SiFreeTdt(HUINT32 ulTsUniqId, HUINT32 ulTableId, HUINT32 ulExternalValue, HUINT32 ulParam2);
HERROR svc_cas_SiStartTdtFilter(SvcCas_Context_t *pstContext);
HERROR svc_cas_SiStopTdtFilter(SvcCas_Context_t *pstContext);
#endif

#if defined(CONFIG_DEBUG)
void svc_cas_SiPrintRawTable(void);
#endif

#endif /* !__CAS_MRG_SI_H__ */
