/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _svc_cas_sub_svc_api.h						\n
	@brief	  cas module header file		\n

	Description: cas module. \n
	Module: MW/CAS									 					\n

	Copyright (c) 2012 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__SVC_CAS_SUB_SVC_API_H__
#define	__SVC_CAS_SUB_SVC_API_H__

#ifdef __cplusplus
extern "C" {
#endif


/********************************************************************
* Header Files :  헤더 파일은 가능한 .c 에서 include 할 것 !
********************************************************************/
#define ___01_Header_Files___
#include <octo_common.h>
#include <svc_cas.h>

/********************************************************************
* Extern Variables / Function Prototypes
********************************************************************/
#define ___02_Extern_Vars_Func_Prototypes___

/********************************************************************
* Module Internal Definitions : #define / enum / struct / union
********************************************************************/
#define ___03_Module_Internal_Definitions___

#define CI_PLUS_MAX_CAM_BRAND_NUM	255	// MAX_CAM_BRAND_NUM와 같다.
#define CI_PLUS_CICAM_ID_LEN		8	// CICAM_ID_LEN와 같다.

// CAS Manger MW Port(casmgr_mw_port)의 stCasMgrPmtProgInfo_t와 mapping 되는 구조체. stCasMgrPmtProgInfo_t구조와 동일하게 유지 해야함.
typedef struct
{
	HUINT16		usProgNum;
	HUINT8		ucVer;
	HBOOL		bCurNextInd;
	HUINT16		usPCRPid;
	HUINT32		ulCrc32;
} SvcCas_SubPmtProgInfo_t;

// CAS Manger MW Port(casmgr_mw_port)의 stCasMgrPmtElemInfo_t와 mapping 되는 구조체. stCasMgrPmtElemInfo_t구조와 동일하게 유지 해야함.
typedef struct
{
	HUINT8		ucStreamType;
	HUINT16		usElemPid;
} SvcCas_SubPmtElemInfo_t;

typedef enum
{
	eCAS_SUB_TYPE_ALL			= 0,
	eCAS_SUB_TYPE_FTA			= 0x0001,
	eCAS_SUB_TYPE_NAGRA			= 0x0002,
	eCAS_SUB_TYPE_IRDETO 		= 0x0004,
	eCAS_SUB_TYPE_NDS			= 0x0008,
	eCAS_SUB_TYPE_VIACCESS		= 0x0010,
	eCAS_SUB_TYPE_CONAX			= 0x0020,
	eCAS_SUB_TYPE_MEDIAGUARD 	= 0x0040,
	eCAS_SUB_TYPE_CRYPTOWORKS	= 0x0080,
	eCAS_SUB_TYPE_JPBCAS 		= 0x0100,
	eCAS_SUB_TYPE_JPCCAS 		= 0x0200,
	eCAS_SUB_TYPE_VERIMATRIX 	= 0x0400
} SvcCas_SubType_e;

// BASE에 정리할 때 한꺼번에 정리예정. 아래 재 정의한 stCasPvrInfo_t으로 대체 예정임.
typedef struct
{
	// PVR Service
	HUINT16 				usTsId;
	HUINT16 				usOnId;
	HUINT16 				usSvcId;
	HUINT16 				usPmtPid;

	// PVR Record
	SvcCas_InstanceType_e	eInstType;
	HUINT16					usSlotId;
} SvcCas_PvrSvcInfo_t;

typedef	struct
{
	HUINT32		hAction;
	HUINT16		usTsUniqId;
	HUINT8		ucDemuxId;
	HUINT16		usPid;
	HUINT8		ucMode;
	HUINT8		ucHead;
	HUINT8		ucTimeoutMode;
	HBOOL		bTableFilter;
	HBOOL		bUseExtId;
	HBOOL		bUseVerNum;
	HBOOL		bCrc;
	HUINT16		usExtId;
	HUINT8		ucVerNum;
	HINT32		nSecCntInPackage;
	HUINT32		nDurationInPackage;
} svcCas_SiRetrieveParam_t;

// SI lib API의 PmtElementInfo_t와 mapping 되는 구조체. PmtElementInfo_t구조와 동일하게 유지 해야함.
typedef struct
{
	HUINT8		ucStreamType;
	HUINT16		usElemPid;
} svcCas_SiPmtElementInfo_t;

//typedef SvcCas_PvrSvcInfo_t	stCasPvrInfo_t;

/*-------------------------- CA System ID에 관련 된 정의는 모두 여기에 -----------------------------------------------------------------------------------*/
typedef enum
{
	eCA_SYS_ID_STANDARD = 0x00,
	eCA_SYS_ID_CANAL_PLUS,				// <--- eCA_SYS_ID_ASTON = eCA_SYS_ID_CANAL_PLUS
		eCA_SYS_ID_ASTON		= eCA_SYS_ID_CANAL_PLUS,
	eCA_SYS_ID_CCETT,
	eCA_SYS_ID_DEUTSCHE_TEL,
	eCA_SYS_ID_EURODEC,
	eCA_SYS_ID_FRANCE_TEL,				// <--- eCA_SYS_ID_VIACCESS = eCA_SYS_ID_FRANCE_TEL
		eCA_SYS_ID_VIACCESS		= eCA_SYS_ID_FRANCE_TEL,
	eCA_SYS_ID_IRDETO,					// <--- irdeto
	eCA_SYS_ID_JERROID_GI,
	eCA_SYS_ID_MATRA_COM,
	eCA_SYS_ID_NEWS_DATACOM,
	eCA_SYS_ID_NOKIA,
	eCA_SYS_ID_NORWEGIAN_TEL,			// <--- eCA_SYS_ID_CONAX = eCA_SYS_ID_NORWEGIAN_TEL
		eCA_SYS_ID_CONAX		= eCA_SYS_ID_NORWEGIAN_TEL,
	eCA_SYS_ID_NTL,
	eCA_SYS_ID_PHILIPS,					// <--- eCA_SYS_ID_CRYPTOWORKS = eCA_SYS_ID_PHILIPS,
		eCA_SYS_ID_CRYPTOWORKS	= eCA_SYS_ID_PHILIPS,
	eCA_SYS_ID_SCI_ATLANTA,
	eCA_SYS_ID_SONY,
	eCA_SYS_ID_TANDBERG,
	eCA_SYS_ID_THOMSON,
	eCA_SYS_ID_TV_COM,
	eCA_SYS_ID_HPT_CPT,
	eCA_SYS_ID_HRT_CRT,
	eCA_SYS_ID_IBM,
	eCA_SYS_ID_NERA,
	eCA_SYS_ID_BETA_TECH,				// <--- SkyD (premiere(BD))
	eCA_SYS_ID_KUDELSKI_SA,				// <--- eCA_SYS_ID_NAGRA = eCA_SYS_ID_KUDELSKI_SA
		eCA_SYS_ID_NAGRA		= eCA_SYS_ID_KUDELSKI_SA,
	eCA_SYS_ID_TITAN_INFO,
	eCA_SYS_ID_TELEFONICA,
	eCA_SYS_ID_STENTOR,
	eCA_SYS_ID_TADIRAN_SCOPUS,
	eCA_SYS_ID_BARCO_AS,
	eCA_SYS_ID_NUM
} SvcCas_CaSysIdSpecifier_e;

#define CRYPTOWORKS_CAM_MANUFACTURER	0xd0
#define ALPHACRYPT_CAM_MANUFACTURER		0x4A20
#define SKYCIPLUS_CAM_MANUFACTURER		0xCA

typedef struct
{
	HUINT16		usTsId;
	HUINT8		ucVer;
	HUINT16		usOrgNetId;
	HUINT8		ucReserved;
	HUINT32		ulCrc32;
} svcCas_SiSdtSectionInfo_t;

typedef struct
{
	HUINT16		usSvcId;
	HBOOL		bEitSched;
	HBOOL		bEitPreFollow;
	HUINT8		ucRunningStatus;
	HBOOL		bCaFree;
} SvcCas_StdServiceInfo_t;

typedef enum
{
	eCAS_RM_ITEM_TUNER_GROUP	,	/**< Tuner group 리소스 */
	eCAS_RM_ITEM_DEMUX,				/**< Demux 리소스 */
	eCAS_RM_ITEM_AV_DEC,			/**< Video decoder 리소스 */
	eCAS_RM_ITEM_HDD_REC,			/**< HDD recording 리소스 */
	eCAS_RM_ITEM_EXT_REC,			/**< External (Ext-Bluray Driver) recording 리소스 */
	eCAS_RM_ITEM_RFINPUT,	 		/**<	 RF Input 리소스 : Satellite 이외에서는 무조건 1 */

	eCAS_RM_ITEM_MAX				/**< 리소스 item의 최대 갯수를 표시함. */
} SvcCas_RmItem_e;

typedef struct
{
	HUINT16					usSvcId;
	HUINT16					usPmtPid;
	HUINT16					usOnId;
	HUINT32					ulUserFlag1;
	SvcCas_SubType_e		eCasMgrCasType;

	SvcCas_InstanceType_e	eInstType;
	HUINT16					usSlotId;

#if defined(CONFIG_MW_CI_PLUS)
	HUINT32					ulTimeStamp;										/** CI protection descriptor save time **/
	HUINT8					ucNumOfEntries;									/** number_of_entries, match_brand_flag가 false라면 이 값은 0 **/
	HUINT16					ausCamBrandId[CI_PLUS_MAX_CAM_BRAND_NUM];		/** cicam_brand_identifier **/
	HUINT8					aucCamId[CI_PLUS_CICAM_ID_LEN];
#endif
} SvcCas_DbSvcInfo_t;

/********************************************************************
* Module Internal Function Prototypes
********************************************************************/
#define ___04_Module_Internal_Func_Prototypes___


/********************************************************************
* Global Public Definitions : #define / enum / struct / union
********************************************************************/
#define ___03_Global_Public_Definitions___


/********************************************************************
* Global Public Function Prototypes
********************************************************************/
#define ___04_Global_Public_Func_Prototypes___

HUINT16 svc_cas_SubSvcDbGetAllUniqueId(void);
HERROR svc_cas_SubSvcDbGetTsInfoByServiceHandle(Handle_t hSvc, HUINT16 *pusUniqueId, HUINT16 *pusTsId, HUINT16 *pusOnId);
HERROR svc_cas_SubSvcDbGetSvcInfoByServiceHandle(Handle_t hSvc, SvcCas_DbSvcInfo_t *pstSvcInfo);
HERROR svc_cas_SubSvcDbUpdateServiceInfo(Handle_t hSvc, SvcCas_DbSvcInfo_t *pstSvcInfo);
HERROR svc_cas_SubSvcDbGetSameTpSvcHandleListByServiceHandle(Handle_t hSvc, HINT32 *pnCnt, Handle_t **pphSvcList);
void svc_cas_SubSvcDbFreeServiceList(Handle_t *phSvcList);

HUINT32	svc_cas_SubSvcRmGetActionId(Handle_t hAction);
HERROR svc_cas_SubSvcRmGetResourceId(HUINT32 ulActionId, SvcCas_RmItem_e eCasMgrRmItem, HUINT32 *pulResourceId);

HERROR svc_cas_SubSvcSiRegisterGetTableTimeoutCallback(SvcCas_SiGetTimeOutCb_t fnGetTableTimeout);
HERROR svc_cas_SiGetTableTimeOut(Handle_t hAction, SiTableType_e eTableType, HUINT32 *pulTimeout);
void *svc_cas_SubSvcSiParseDescriptor(SIxTextEncoding_e eTextEnc, HUINT8 *pucRaw);
HERROR svc_cas_SubSvcSiFreeDescriptor(void *pvDes);
HERROR svc_cas_SubSvcSiFreeDescriptorRaw (HUINT8 *pucDesRaw);

HERROR svc_cas_SubSvcSiGetPmtProgramInfoFromRaw(HUINT8 *pucRaw, SvcCas_SubPmtProgInfo_t *pstInfoDest);
HERROR svc_cas_SubSvcSiCountPmtElementFromRaw(HUINT8 *pucRaw, HUINT32 *pnCnt);
HERROR svc_cas_SubSvcSiGetPmtElementInfoFromRaw(HUINT8 *pucRaw, HUINT32 ulElemIdx, SvcCas_SubPmtElemInfo_t *pstInfoDest);
#if defined(CONFIG_MW_CI_PLUS)
HERROR svc_cas_SubSvcSiGetCaPidFromRaw(HUINT8 *pucRaw, SIxTextEncoding_e eTextEnc, HUINT32 idx, HUINT16 *pusCaPid);
HERROR svc_cas_SubSvcSiGetCaSysIdFromRaw(HUINT8 *pucRaw, SIxTextEncoding_e eTextEnc, HUINT32 idx, HUINT16 *pusCaSysId);
HERROR svc_cas_SubSvcSiGetPmtElementDesRawFromRaw(HUINT8 *pucRaw, HUINT32 ulElemIdx, HUINT32 ulTag, HUINT32 nDesIdx, HUINT32 *pulDesRawSize, HUINT8 **ppucDesRaw);
HERROR svc_cas_SubSvcSiGetTuningInfoFromSatDSD(void *pstDeliDes, SvcCas_TuningDesInfo_t *pstTuningParam);
HERROR svc_cas_SubSvcSiGetTuningInfoFromTerDSD(void *pstDeliDes, SvcCas_TuningDesInfo_t *pstTuningParam);
HERROR svc_cas_SubSvcSiGetTuningInfoFromCabDSD(void *pstDeliDes, SvcCas_TuningDesInfo_t *pstTuningParam);
#endif
HERROR svc_cas_SubSvcSiGetSdtSectionInfoFromRaw(HUINT8 *pucRaw, svcCas_SiSdtSectionInfo_t *pstInfo);
HERROR svc_cas_SubSvcSiCountSdtServiceFromRaw(HUINT8 *pucRaw, HUINT32 *pulCnt);
HERROR svc_cas_SubSvcSiGetSdtServiceInfoFromRaw(HUINT8 *pucRaw, HUINT32 ulSvcIdx, SvcCas_StdServiceInfo_t *pstInfo);
HERROR svc_cas_SubSvcSiCountSdtServiceDescriptorFromRaw(HUINT8 *pucRaw, HUINT32 ulSvcIdx, HUINT32 ulTag, HUINT32 *pnDesCnt);
HERROR svc_cas_SubSvcSiGetSdtServiceDescriptorFromRaw(HUINT8 *pucRaw, SIxTextEncoding_e eTextEnc, HUINT32 ulSvcIdx, HUINT32 ulTag, HUINT32 ulDesIdx, void **ppvDes);

HERROR svc_cas_SubSvcPvrGetService(Handle_t hAct, SvcCas_PvrSvcInfo_t *pstCasPvrSvcInfo);
HERROR svc_cas_SubSvcPvrGetPvrInfoFromMeta(Handle_t hAct, SvcCas_PvrSvcInfo_t *pstCasPvrInfo);
HERROR svc_cas_SubSvcPvrUpdatePvrInfoToMeta(Handle_t hAct, SvcCas_PvrSvcInfo_t *pstCasPvrInfo);

HERROR svc_cas_SubSvcDmxStopOneSection (HUINT16 usfilterId);

HERROR svc_cas_SubSvcDmxSetDefaultRetrieveParam (svcCas_SiRetrieveParam_t *pRetrieveParam, HUINT32 hAction, HUINT16 usTsUniqId, HUINT32 ulDemuxId);
HERROR svc_cas_SubSvcDmxRetrieveSectionBase (svcCas_SiRetrieveParam_t *pstTable, HUINT8 ucTableId, HUINT8 *pucFilterValue,
											 HUINT8 *pucFilterMask, HUINT32 ulFilterSize, HUINT32 ulFilterTimeout, void *pfnParse, void *pfnFree,	HUINT16 *pusFilterId);


#ifdef __cplusplus
}
#endif


#endif /* !__SVC_CAS_SUB_SVC_API_H__ */
