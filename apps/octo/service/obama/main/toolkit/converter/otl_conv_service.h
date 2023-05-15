
#ifndef	__OTL_CONV_SERVICE_H__
#define	__OTL_CONV_SERVICE_H__


#include <dlib.h>
#include <db_svc.h>
#include <svc_ch.h>

#ifdef  __cplusplus
extern "C" {
#endif

/**
 * DxService_t
 * <-> DbSvc_Info_t
 */
DxService_t *			OTL_CONV_SVC_Info2DxService (DxService_t* d,const DbSvc_Info_t *s);
DbSvc_Info_t *			OTL_CONV_DxService2SVC_Info(DbSvc_Info_t *d, const DxService_t *s);

/**
 * DxTransponder_t
 * <-> DbSvc_TsInfo_t
 */
DxTransponder_t *		OTL_CONV_SVC_TsInfo2DxTransponder (DxTransponder_t *d,  const DbSvc_TsInfo_t *s);
DbSvc_TsInfo_t * 			OTL_CONV_DxTransponder2SVC_TsInfo(DbSvc_TsInfo_t *d, const DxTransponder_t *s);

/**
 * DxTuneParam_t
 * <-> DbSvc_TuningInfo_t
 */
HERROR OTL_CONV_DxTuneParam2DbTuningInfo(const DxTuneParam_t *pstDxTuneParam, DbSvc_TuningInfo_t *pstSvcTuningInfo);
HERROR OTL_CONV_DbTuningInfo2DxTuneParam(const DbSvc_TsInfo_t *pstSvcTsInfo, DxTuneParam_t *pstChTuningParam);

/**
 * DxProvider_t
 * <-> DbSvc_PrvInfo_t
 */
DxProvider_t *			OTL_CONV_SVC_PrvInfo2DxProvider (DxProvider_t *d,  const DbSvc_PrvInfo_t *s);
DbSvc_PrvInfo_t * 		OTL_CONV_DxProvider2SVC_PrvInfo(DbSvc_PrvInfo_t *d, const DxProvider_t *s);

/**
 * DxGroup_t
 * <-> DbSvc_GroupInfo_t
 */
DxGroup_t *				OTL_CONV_SVC_GroupInfo2DxGroup (DxGroup_t *d,const DbSvc_GroupInfo_t *s);
DbSvc_GroupInfo_t*		OTL_CONV_DxGroup2SVC_GroupInfo(DbSvc_GroupInfo_t *d, const DxGroup_t *s);

/**
 * DxBouquet_t
 * <-> DbSvc_BouqInfo_t
 */
DxBouquet_t *			OTL_CONV_SVC_BouqInfo2DxBouquet (DxBouquet_t *d,  const DbSvc_BouqInfo_t *s);
DbSvc_BouqInfo_t* 		OTL_CONV_DxBouquet2SVC_BouqInfo(DbSvc_BouqInfo_t *d, const DxBouquet_t *s);

/**
 * DxNetwork_t
 * <-> DbSvc_NetInfo_t
 */
DxNetwork_t *			OTL_CONV_SVC_NetInfo2DxNetwork (DxNetwork_t *d, const DbSvc_NetInfo_t *s);
DbSvc_NetInfo_t*			OTL_CONV_DxNetwork2SVC_NetInfo(DbSvc_NetInfo_t *d, const DxNetwork_t *s);

/**
 * DxSatTuningInfo_t
 * <-> DbSvc_SatTuningInfo_t
 */
DxSatTuningInfo_t *		OTL_CONV_SVC_SatTuningInfo2DxSatTuningInfo (DxSatTuningInfo_t *d, const DbSvc_SatTuningInfo_t *s);
DbSvc_SatTuningInfo_t *	OTL_CONV_DxSatTuningInfo2SVC_SatTuningInfo(DbSvc_SatTuningInfo_t *d, const DxSatTuningInfo_t *s);

/**
 * DxCabTuningInfo_t
 * <-> DbSvc_CabTuningInfo_t
 */
DxCabTuningInfo_t *		OTL_CONV_SVC_CabTuningInfo2DxCabTuningInfo (DxCabTuningInfo_t *d, const DbSvc_CabTuningInfo_t *s);
DbSvc_CabTuningInfo_t *	OTL_CONV_DxCabTuningInfo2SVC_CabTuningInfo(DbSvc_CabTuningInfo_t   *d, const DxCabTuningInfo_t *s);

/**
 * DxTerTuningInfo_t
 * <-> DbSvc_TerTuningInfo_t
 */
DxTerTuningInfo_t *		OTL_CONV_SVC_TerTuningInfo2DxTerTuningInfo (DxTerTuningInfo_t *d, const DbSvc_TerTuningInfo_t*s);
DbSvc_TerTuningInfo_t *	OTL_CONV_DxTerTuningInfo2SVC_TerTuningInfo(DbSvc_TerTuningInfo_t *d, const DxTerTuningInfo_t *s);


/**
 * DxLnbAntenna_t
 * <-> DbSvc_LnbAntInfo_t
 */
DxLnbAntenna_t *		OTL_CONV_DbLnbAntInfo2DxLnbAntenna (DxLnbAntenna_t *d, const DbSvc_LnbAntInfo_t *s);
DbSvc_LnbAntInfo_t *		OTL_CONV_DxLnbAntenna2DbLnbAntInfo (DbSvc_LnbAntInfo_t *d, const DxLnbAntenna_t *s);

/**
 * DxDiseqcAntenna_t
 * <-> DbSvc_DiseqcAntInfo_t
 */
DxDiseqcAntenna_t *		OTL_CONV_DbDiseqcAntInfo2DxDiseqcAntenna (DxDiseqcAntenna_t *d, const DbSvc_DiseqcAntInfo_t *s);
DbSvc_DiseqcAntInfo_t *		OTL_CONV_DxDiseqcAntenna2DbDiseqcAntInfo (DbSvc_DiseqcAntInfo_t *d, const DxDiseqcAntenna_t *s);

/**
 * DxScdAntenna_t
 * <-> DbSvc_ScdAntInfo_t
 */
DxScdAntenna_t *		OTL_CONV_DbScdAntInfo2DxScdAntenna (DxScdAntenna_t *d, const DbSvc_ScdAntInfo_t *s);
DbSvc_ScdAntInfo_t *		OTL_CONV_DxScdAntenna2DbScdAntInfo (DbSvc_ScdAntInfo_t *d, const DxScdAntenna_t *s);

/**
 * DxAntInfo_t
 * <-> DbSvc_AntInfo_t
 */
DxAntInfo_t *			OTL_CONV_DbAntInfo2DxAntInfo (DxAntInfo_t *d, const DbSvc_AntInfo_t *s);
DbSvc_AntInfo_t *			OTL_CONV_DxAntInfo2DbAntInfo (DbSvc_AntInfo_t *d, const DxAntInfo_t *s);


#ifdef  __cplusplus
}
#endif

#endif /* __OTL_CONV_SERVICE_H__ */

