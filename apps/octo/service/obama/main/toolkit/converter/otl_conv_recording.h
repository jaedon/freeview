
#ifndef	__OTL_CONV_RECORDING_H__
#define	__OTL_CONV_RECORDING_H__


#include <dlib.h>

//#include <svc_fs.h>
#include <svc_rec.h>
#include <svc_meta.h>

#ifdef  __cplusplus
extern "C" {
#endif


HERROR				OTL_CONV_MW_PvrMeta2DxRecListData(DxRecListData_t *d, const SvcMeta_Record_t *rec, const SvcMeta_Service_t *svc, const SvcMeta_Event_t *evt);
DxRecListData_t *	OTL_CONV_MW_PvrRecord2DxRecListData(DxRecListData_t *d, const SvcMeta_Record_t *rec);
DxRecListData_t *	OTL_CONV_MW_PvrService2DxRecListData(DxRecListData_t *d, const SvcMeta_Service_t *svc);
DxRecListData_t *	OTL_CONV_MW_PvrEvent2DxRecListData(DxRecListData_t *d, const SvcMeta_Event_t *evt);
SvcMeta_Record_t *	OTL_CONV_DxRecListData2MW_PvrRecord(SvcMeta_Record_t *d, const DxRecListData_t *s);
SvcMeta_Service_t *	OTL_CONV_DxRecListData2MW_PvrService(SvcMeta_Service_t *d, const DxRecListData_t *s);
SvcMeta_Event_t *	OTL_CONV_DxRecListData2MW_PvrEvent(SvcMeta_Event_t *d, const DxRecListData_t *s);
HERROR				OTL_CONV_DxRecListData2MW_PvrMeta(SvcMeta_Record_t *rec, SvcMeta_Service_t *svc, SvcMeta_Event_t *evt, const DxRecListData_t *s);



#ifdef  __cplusplus
}
#endif

#endif /* __OTL_CONV_RECORDING_H__ */

