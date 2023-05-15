
#include "octo_common.h"

#include <pal_sc.h>

#include "_svc_cas_mgr_main.h"
#include <_svc_cas_sub_dev_api.h>

#include "vd_humax.h"

#include <pal_sef.h>
#include <pal_pipe.h>

#include "s3_osdrv.h"
#include "s3_vddf.h"

#include "util.h"

#include <_svc_cas_mgr_svc_port.h>
#include <_svc_cas_sub_svc_api.h>
#include <_svc_cas_sub_dev_api.h>
#include "vd_humax.h"
#include "ir_mgr.h"


//#define VD_DUMP_FILTER


//common define으로 적용.
#define	CHECK_ECM_SECTION_NUM

//if ref id is allocated, it has 1,(TRUE) otherwise 0(FALSE)
static HUINT32	s_bDmxRefIdInfo[HMX_VD_DMX_MAX_TOTAL_CHANNEL];

HVD_DMX_DEMUX_INFO_STRUCT	s_stIR_DemuxInfo[HMX_VD_DMX_MAX_NUM];

vd_df_config_event_st	g_stEventCB;

HULONG	s_ulDMUXAPISem;
HULONG	s_ulDMUXNotiSecSem;
#define DEMUX_SEM_GET()				VK_SEM_Get(s_ulDMUXAPISem)
#define DEMUX_SEM_RELEASE()		VK_SEM_Release(s_ulDMUXAPISem)
#define DEMUX_SEM_SECNOTI_GET()		VK_SEM_Get(s_ulDMUXNotiSecSem)
#define DEMUX_SEM_SECNOTI_RELEASE()	VK_SEM_Release(s_ulDMUXNotiSecSem)




#ifdef CHECK_ECM_SECTION_NUM
#define ECM_SECTION_TAG_1	0x80
#define ECM_SECTION_TAG_2 	0x81

#define DVB_ECM_FILTER_SECTIOM_TAG(section)			section[0]
#define DVB_ECM_FILTER_SECTION_PAGE(section)			section[4]
#define DVB_ECM_FILTER_SECTION_LAST_PAGE(section)	section[5]
#endif
/********************************************************************
*	Static functions Prototypes
*	The following functions will be used as inner functions
********************************************************************/
static inline void local_VdDmx_GetTableIdAndResult(HUINT32 value, HUINT16* usTableId, HUINT16 *result);
static inline void local_VdDmx_GetFilterAndTsUniqueId(HUINT32 value, HUINT16 *usFilterId, HUINT16 *usTsUniqId);
static HVD_Result local_VdDmx_SetVDdemuxInfo(vd_rsc_id resourceID, vd_ref_id referenceID, HUINT8 *DemuxID, HUINT32 *DemuxRefIdx);
static HUINT8 local_VdDmx_GetVDdemuxInfoIndex(vd_ref_id referenceID, HUINT32	*DemuxInfoIdx, HUINT32 *RefInfoIdx);
static vd_ref_id local_VdDmx_GetAvailableReferenceID(void);
static void local_VdDmx_ResetReferenceID(vd_ref_id referenceID);
static ia_result local_VdDmx_AllocateFilter( vd_ref_id wRefId, ia_word32 wDataWriteSize, void *pvDataWrite, ia_word32 *pwDataReadSize, void *pvDataRead );
static ia_result local_VdDmx_FreeFilter( vd_ref_id wRefId, ia_word32 wDataWriteSize, void *pvDataWrite, ia_word32 *pwDataReadSize, void *pvDataRead );
static ia_result local_VdDmx_SetPid( vd_ref_id wRefId, ia_word32 wDataWriteSize, void *pvDataWrite, ia_word32 *pwDataReadSize, void *pvDataRead );
static ia_result local_VdDmx_SetFilter( vd_ref_id wRefId, ia_word32 wDataWriteSize, void *pvDataWrite, ia_word32 *pwDataReadSize, void *pvDataRead );
static ia_result local_VdDmx_StartFilter( vd_ref_id wRefId, ia_word32 wDataWriteSize, void *pvDataWrite, ia_word32 *pwDataReadSize, void *pvDataRead );
static ia_result local_VdDmx_StopFilter( vd_ref_id wRefId, ia_word32 wDataWriteSize, void *pvDataWrite, ia_word32 *pwDataReadSize, void *pvDataRead );
static ia_result local_VdDmx_Reset( vd_ref_id wRefId, ia_word32 wDataWriteSize, void *pvDataWrite, ia_word32 *pwDataReadSize, void *pvDataRead );
static ia_result local_VdDmx_Make_OctoFilter(HUINT32 ulDemuxId, HUINT16 usPid, SvcCas_FilterParam_t *out_pstSiRetrieveParam );
static HUINT32	local_VdDmx_GetNeededIdxFromRequestID(HUINT32 RequestID, HUINT32 *DemuxInfoIdx, HUINT32 *RefInfoIdx, HUINT32 *FilterInfoIdx);
static void local_VdDmx_SecNoti_Callback(HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, Handle_t hAction);
#if (VD_FILTER_VERSION ==1)
HVD_Result	local_VdDmx_AllocSectionBuffer(HUINT32	DemuxInfoIdx, HUINT32 RefInfoIdx, HUINT16 BufferLength);
HVD_Result	local_VdDmx_FreeSectionBuffer(HUINT32	DemuxInfoIdx, HUINT32 RefInfoIdx);
ia_result	local_VdDmx_WriteSection(HUINT32 RequestID, HUINT8 *pRaw);
ia_result	local_VdDmx_ReadSection( vd_ref_id wRefId, ia_word32 wDataWriteSize, void *pvDataWrite, ia_word32 *pwDataReadSize, void *pvDataRead );
#endif


#define ______EXTERN_FUNCTION___________________
extern HIR_ERR	CAS_IR_DRV_GetActionHandleByDemuxId(HUINT8 in_ucDemuxId, Handle_t *out_hAct);
extern HERROR svc_cas_SubSvcDmxRetrieveSectionBase (SvcCas_FilterParam_t *pstTable,
									HUINT8 ucTableId,
									HUINT8 *pucFilterValue,
									HUINT8 *pucFilterMask,
									HUINT32 ulFilterSize,
									HUINT32 ulFilterTimeout,
									void *pfnParse,
									void *pfnFree,
									HUINT16 *pusFilterId);
extern HERROR svc_cas_SubSvcDmxSetDefaultRetrieveParam (SvcCas_FilterParam_t *pRetrieveParam,
														HUINT32 hAction,
														HUINT16 usTsUniqId,
														HUINT32 ulDemuxId);


#define ____VD_DMX_LOCAL_FUNCTION______________

static inline void local_VdDmx_GetTableIdAndResult(HUINT32 value, HUINT16* usTableId, HUINT16 *result)
{
	*result = value & 0x0000ffff;
	*usTableId = (value & 0xffff0000) >> 16;
	return;
}
static inline void local_VdDmx_GetFilterAndTsUniqueId(HUINT32 value, HUINT16 *usFilterId, HUINT16 *usTsUniqId)
{
	*usTsUniqId = value & 0x0000ffff;
	*usFilterId = (value & 0xffff0000) >> 16;
	return;
}

static void local_VdDmx_ClearOneFilterInfo(HVD_DMX_FILTER_INFO_STRUCT *pFilter)
{
	pFilter->RequestID = HMX_VD_DMX_INVALID_REQUESET_ID;
	pFilter->Allocated = FALSE;
	pFilter->Started = FALSE;
	pFilter->wFilterLength = FALSE;
	VK_MEM_Memset(pFilter->FilterValue, 0x00, HMX_VD_DMX_MAX_FILTER_DEPTH );
	VK_MEM_Memset(pFilter->FilterMask, 0x00, HMX_VD_DMX_MAX_FILTER_DEPTH );
}

static HVD_Result local_VdDmx_SetVDdemuxInfo(vd_rsc_id resourceID, vd_ref_id referenceID, HUINT8 *DemuxID, HUINT32 *DemuxRefIdx)
{
	HUINT32	i, j;

	HxLOG_Info("\n");

	/* Step 1. first find already allocated dmx resouce */
	for( i = 0; i < HMX_VD_DMX_MAX_NUM; i++ )
	{
		if( s_stIR_DemuxInfo[i].ResourceID == resourceID )
		{
			// 이미 있던 resourceID이면( 즉, 동일한 demux를 사용하는 service가 하나 추가되면.. ), RefInfo에 추가.
			HxLOG_Print(" : Already exist resource ID(0x%X)\n", (HUINT32) resourceID);
			for( j = 0; j < HMX_VD_DMX_MAX_CHANNEL_PER_DEMUX; j++ )
			{
				if( s_stIR_DemuxInfo[i].RefInfo[j].ReferenceID == VD_REF_ID_ERROR )
				{
					HxLOG_Print(" : save DemuxInfoIdx(0x%X), RefInfoIdx(0x%X)\n", i, j);

					s_stIR_DemuxInfo[i].RefInfo[j].ReferenceID = referenceID;

					*DemuxID =i;
					*DemuxRefIdx = j;

					return eHVD_OK;
				}
			}
			if (j ==HMX_VD_DMX_MAX_CHANNEL_PER_DEMUX)
			{
				HxLOG_Print(" : found DemuxInfoIdx(0x%X), but there is no empty ref id vacancy.\n", i);
				return eHVD_ERR;
			}

		}
	}

	/* Step 2. allocate new reference for input resource id  */

	if( i == HMX_VD_DMX_MAX_NUM )
	{
		HxLOG_Print(" : not exist resource ID(0x%X)\n", (HUINT32) resourceID);
		for( i = 0;i < HMX_VD_DMX_MAX_NUM; i++ )
		{
			if( s_stIR_DemuxInfo[i].ResourceID == VD_RSC_ID_INVALID )
			{
				HxLOG_Print(" : FIND DemuxInfoIdx(0x%X)\n", i);

				for(j=0; j<HMX_VD_DMX_MAX_CHANNEL_PER_DEMUX; j++)
				{
					if(s_stIR_DemuxInfo[i].RefInfo[j].ReferenceID == VD_REF_ID_ERROR  )
					{
						HxLOG_Print(" : save DemuxInfoIdx(0x%X), RefInfoIdx(0x%X)\n", i, j);

						s_stIR_DemuxInfo[i].ResourceID = resourceID;
						s_stIR_DemuxInfo[i].RefInfo[j].ReferenceID = referenceID;

						*DemuxID =i;
						*DemuxRefIdx = j;

						return eHVD_OK;
					}
				}
			}
		}
	}

	return eHVD_ERR;
}

static HUINT8 local_VdDmx_GetVDdemuxInfoIndex(vd_ref_id referenceID, HUINT32	*DemuxInfoIdx, HUINT32 *RefInfoIdx)
{
	HUINT8 i, j;

	HxLOG_Info("\n");
	for( i = 0; i < HMX_VD_DMX_MAX_NUM; i++ )
	{
		for( j = 0; j < HMX_VD_DMX_MAX_CHANNEL_PER_DEMUX; j++ )
		{
			if( s_stIR_DemuxInfo[i].RefInfo[j].ReferenceID == referenceID )
			{
				*DemuxInfoIdx = i;
				*RefInfoIdx = j;
				return eHVD_OK;
			}
		}
	}

	HxLOG_Print(" ERROR - GetVDdemuxInfoIndex : No matched demux resource space\n");
	return eHVD_ERR;
}


/********************************************************************
*	function name :
*	input :
*	output :
*	description :
********************************************************************/
static vd_ref_id local_VdDmx_GetAvailableReferenceID(void)
{

	HUINT32	i;

	HxLOG_Info("\n");

	for( i = 0; i < HMX_VD_DMX_MAX_TOTAL_CHANNEL; i++ )
	{
		if( s_bDmxRefIdInfo[i] != TRUE )
		{
			s_bDmxRefIdInfo[i] = TRUE;
			return (vd_ref_id)i;
		}
	}

	HxLOG_Print(" ERROR : no more available reference ID\n");
	return VD_REF_ID_ERROR;
}

static void local_VdDmx_ResetReferenceID(vd_ref_id referenceID)
{

	if( referenceID <	HMX_VD_DMX_MAX_CHANNEL_PER_DEMUX)
		s_bDmxRefIdInfo[referenceID] = FALSE;
}


/********************************************************************
*	function name :
*	input :
*	output :
*	description :
********************************************************************/
static ia_result local_VdDmx_AllocateFilter( vd_ref_id wRefId, ia_word32 wDataWriteSize, void *pvDataWrite, ia_word32 *pwDataReadSize, void *pvDataRead )
{
	vd_df_alloc_filter_st	DF_AllocFilter;
	vd_df_filter_st			DF_Filter;
	HUINT8					*p = (HUINT8 *)pvDataWrite;
	HUINT8					ret;
	HUINT32					i, DemuxInfoIdx, RefInfoIdx;
	ia_result				result;

	HxLOG_Info("\n");

	result = IA_FAIL;

	// prevent fix(added)
	if ( (pwDataReadSize == NULL) || (pvDataRead == NULL))
	{
		HxLOG_Critical("\n\n");
		return IA_INVALID_PARAMETER;
	}
        // the end of fix

	HxSTD_memcpy((HUINT8 *)&DF_AllocFilter, p, sizeof(vd_df_alloc_filter_st));

#ifdef VD_DEMUX_DEBUG

	if( DF_AllocFilter.eFilterType == VD_DF_INEQUALITY )
		HxLOG_Print("\t IO - FilterType : INEQUALITY\n");

#endif
	ret = local_VdDmx_GetVDdemuxInfoIndex(wRefId, &DemuxInfoIdx, &RefInfoIdx);
	HxLOG_Print("(ref id #%d)[resource id #%d(%d:%d)] ALLOCFILTER => "	, (HUINT32) wRefId, (HUINT32) s_stIR_DemuxInfo[DemuxInfoIdx].ResourceID, DemuxInfoIdx, RefInfoIdx);
	if( ret != eHVD_OK )
	{
		HxLOG_Print(" ERROR(ALLOC) : invalid referenceID(0x%X)\n", (HUINT32) wRefId);
		return IA_INVALID_PARAMETER;
	}

	for( i = 0; i < HMX_VD_DMX_MAX_FILTER_PER_CHANNEL; i++ )
	{
		if( s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].IR_FilterInfo[i].Allocated == FALSE )
		{
			if( *pwDataReadSize < sizeof(vd_df_filter_st) )
			{
				HxLOG_Print("  ERROR(ALLOC) : invalid ReadSize(0x%X)\n", (HUINT32)*pwDataReadSize);
				*pwDataReadSize = sizeof(vd_df_filter_st);
				result = IA_FULL;
			}
			else
			{
				s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].IR_FilterInfo[i].Allocated = TRUE;
				s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].IR_FilterInfo[i].wFilterLength = DF_AllocFilter.wFilterLength;
				DF_Filter.wFilterId = (vd_df_filter_id) i;
				HxSTD_memcpy((HUINT8 *)pvDataRead, (HUINT8 *)&DF_Filter, sizeof(DF_Filter));
				*pwDataReadSize = sizeof(DF_Filter);
				HxLOG_Print("F_Len(0x%X), F_ID(0x%X)\n", DF_AllocFilter.wFilterLength, i);
				result = IA_SUCCESS;
			}
			break;
		}
	}

	if( i == HMX_VD_DMX_MAX_FILTER_PER_CHANNEL )
	{
		HxLOG_Print(" ERROR(ALLOC) : no empty filter info (ref ed=%X )\n",(HUINT32)  wRefId);
		result = IA_FAIL;
	}

	return result;
}


static ia_result local_VdDmx_FreeFilter( vd_ref_id wRefId, ia_word32 wDataWriteSize, void *pvDataWrite, ia_word32 *pwDataReadSize, void *pvDataRead )
{
	vd_df_filter_st		DF_Filter;
	HUINT8	*p = (HUINT8 *)pvDataWrite;
	HUINT32	DemuxInfoIdx, RefInfoIdx;
	HUINT8	ret;
	HERROR hErr;
	ia_result	iaResult = IA_SUCCESS;
	HVD_DMX_FILTER_INFO_STRUCT *pFilterInfo;

	HxLOG_Info("wRefId(0x%X)\n", (HUINT32) wRefId);

	HxSTD_memcpy((HUINT8 *)&DF_Filter, p, sizeof(vd_df_filter_st));

	ret = local_VdDmx_GetVDdemuxInfoIndex(wRefId, &DemuxInfoIdx, &RefInfoIdx);
	HxLOG_Print("ResourceID=%d, ref id=%d,  FREE => F_ID(0x%X)\n",
						(HUINT32) s_stIR_DemuxInfo[DemuxInfoIdx].ResourceID,(HUINT32)wRefId , (HUINT32)DF_Filter.wFilterId);
	if( ret != eHVD_OK )
	{
		HxLOG_Print(" ERROR(FREE) : invalid referenceID(0x%X)\n", (HUINT32) wRefId);
		iaResult = IA_INVALID_PARAMETER;
		goto Exit;
	}

	pFilterInfo = &s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].IR_FilterInfo[DF_Filter.wFilterId];

	if( pFilterInfo->Started == TRUE )
	{
		/* Condition: the process when filter was not stopped yet */
		HxLOG_Print("filter was not stopped yet.\n");
		HxLOG_Print("dmx(%d), ref id = 0x%X, filter id = 0x%X, requeset id = 0x%X\n", \
			DemuxInfoIdx, RefInfoIdx, (HUINT32)DF_Filter.wFilterId, pFilterInfo->RequestID);

		hErr = svc_cas_SubSvcDmxStopOneSection(pFilterInfo->RequestID);

		if (hErr != ERR_OK)
		{
			HxLOG_Print(" ERROR : svc_cas_SubSvcDmxStopOneSection : %08X\n", hErr);
			iaResult = IA_FAIL;
			goto Exit;
		}
	}

	if( pFilterInfo->Allocated == FALSE )
	{
		HxLOG_Print(" ERROR(FREE) : invalid filterID(0x%X)\n", (HUINT32)DF_Filter.wFilterId);
		iaResult = IA_INVALID_PARAMETER;
		goto Exit;
 	}
	/*clear filter info */
	local_VdDmx_ClearOneFilterInfo(pFilterInfo);

Exit:


	return iaResult;
}

static ia_result local_VdDmx_SetPid( vd_ref_id wRefId, ia_word32 wDataWriteSize, void *pvDataWrite, ia_word32 *pwDataReadSize, void *pvDataRead )
{
	vd_df_pid_st			DF_Pid;
	HUINT8				*p = (HUINT8 *)pvDataWrite;
	HUINT32				DemuxInfoIdx, RefInfoIdx;
	HUINT8				ret;
	ia_result				result;

	HxLOG_Info("wRefId(0x%X)\n", (HUINT32)wRefId);


	HxSTD_memcpy((HUINT8 *)&DF_Pid, p, sizeof(vd_df_pid_st));
	ret = local_VdDmx_GetVDdemuxInfoIndex(wRefId, &DemuxInfoIdx, &RefInfoIdx);
	HxLOG_Print("(ref id #%d)[ResourceID#%d(%d:%d)] SET PID => PID(0x%X)\n",
		(HUINT32) wRefId, (HUINT32) s_stIR_DemuxInfo[DemuxInfoIdx].ResourceID, DemuxInfoIdx, RefInfoIdx, DF_Pid.wPid);
	if( ret == eHVD_OK )
	{
		s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].PID = DF_Pid.wPid;
		result = IA_SUCCESS;
	}
	else
	{
		HxLOG_Print(" ERROR(SetPID) : invalid referenceID(0x%X)\n", (HUINT32) wRefId);
		result = IA_INVALID_PARAMETER;
	}


	return result;
}

static ia_result local_VdDmx_SetFilter( vd_ref_id wRefId, ia_word32 wDataWriteSize, void *pvDataWrite, ia_word32 *pwDataReadSize, void *pvDataRead )
{
	vd_df_filter_st		DF_Filter;
	HUINT8			*p = (HUINT8 *)pvDataWrite;
	HUINT32			DemuxInfoIdx, RefInfoIdx;
	HUINT16			RequestID;
	HUINT32			hErr;
	ia_result			result = IA_SUCCESS;
#if defined(VD_DUMP_FILTER)
	HUINT32			i;
#endif

	HVD_DMX_FILTER_INFO_STRUCT *SelectedFilter;
	SvcCas_FilterParam_t	stFilterParam;

	HxLOG_Info("wRefId(0x%X)\n",(HUINT32)  wRefId);


	HxSTD_memcpy((HUINT8 *)&DF_Filter, p, sizeof(vd_df_filter_st));
	p += sizeof(vd_df_filter_st);

	hErr = local_VdDmx_GetVDdemuxInfoIndex(wRefId, &DemuxInfoIdx, &RefInfoIdx);

	HxLOG_Print("rsc id = 0x%X, ref id = 0x%X, dmx info idx= 0x%X, refid idx = 0x%X, filter info idx =0x%X \n",
		(HUINT32) s_stIR_DemuxInfo[DemuxInfoIdx].ResourceID,(HUINT32) wRefId, DemuxInfoIdx, RefInfoIdx, (HUINT32)DF_Filter.wFilterId);

	if( hErr != eHVD_OK )
	{
		HxLOG_Print(" - ERROR : invalid referenceID(0x%X)\n", (HUINT32) wRefId);
 		result = IA_INVALID_PARAMETER;
		goto Exit;
	}

	SelectedFilter = 	&(s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].IR_FilterInfo[DF_Filter.wFilterId]);

 	if(SelectedFilter->Allocated == FALSE)
 	{
		HxLOG_Print(" - ERROR : invalid filterID(0x%X)\n", (HUINT32)DF_Filter.wFilterId);
 		result = IA_INVALID_PARAMETER;
		goto Exit;
 	}

 	if( SelectedFilter->Started == TRUE )
	{
		// 이미 start되었던 것은 stop하고..
		if(ERR_OK !=  svc_cas_SubSvcDmxStopOneSection(SelectedFilter->RequestID) )
		{
			HxLOG_Print("svc_cas_SubSvcDmxStopOneSection(RequestID=0x%X)\n", SelectedFilter->RequestID);
			result = IA_FAIL;
			goto Exit;
		}
	}
#if defined(VD_DUMP_FILTER)
	// Set..
	HxLOG_Print("\t\t Set -Value:");
	for( i = 0; i < SelectedFilter->wFilterLength; i++ )
	{
		HxLOG_Print("%02x.", p[i]);
	}
	HxLOG_Print("\n");
#endif
	HxSTD_memcpy(	SelectedFilter->FilterValue, p, SelectedFilter->wFilterLength);

	p += s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].IR_FilterInfo[DF_Filter.wFilterId].wFilterLength;

	HxSTD_memcpy(	SelectedFilter->FilterMask,p, SelectedFilter->wFilterLength);

	if(SelectedFilter->Started == TRUE )
	{
		HxLOG_Print("Request change already started filter's value\n");

		if(IA_SUCCESS != local_VdDmx_Make_OctoFilter(s_stIR_DemuxInfo[DemuxInfoIdx].ResourceID,
											s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].PID,
											&stFilterParam) )
		{
			HxLOG_Critical("\n\n");
			result = IA_FAIL;
			goto Exit;
		}
#if defined(VD_DUMP_FILTER)
{
		int j;
		HxLOG_Print(" Filter info :  \n\tMode=0x%x\n\tTableFilter=0x%x\n\tCrc=%d\n\tUse_Ver_Num=%d\n\tPid =0x%X\n",
			stFilterParam.ucMode,	stFilterParam.bTableFilter, stFilterParam.bCrc, stFilterParam.bUseVerNum, stFilterParam.usPid);

		HxLOG_Print("Filter value info(length=%d) \n", SelectedFilter->wFilterLength );
		for(j=0;j < SelectedFilter->wFilterLength; j++)
		{
			HxLOG_Print("%02X ", SelectedFilter->FilterValue[j] );
		}
		HxLOG_Print("\nFilter mask info \n" );
		for(j=0;j < SelectedFilter->wFilterLength; j++)
		{
			HxLOG_Print("%02X ", SelectedFilter->FilterMask[j] );
		}
		HxLOG_Print("\n______________ \n" );
}
#endif
		hErr = svc_cas_SubSvcDmxRetrieveSectionBase(&stFilterParam,
														SelectedFilter->FilterValue[0],
														SelectedFilter->FilterValue,
														SelectedFilter->FilterMask,
														SelectedFilter->wFilterLength,
														0,
														(void*)local_VdDmx_SecNoti_Callback,
														NULL,
														&RequestID);
		if (hErr != ERR_OK)
		{
			HxLOG_Print("svc_cas_SubSvcDmxRetrieveSectionBase return ERROR (%x)\n", hErr);
			result = IA_FAIL;
			goto Exit;
		}

		HxLOG_Print("svc_cas_SubSvcDmxRetrieveSectionBase OK !(RequestID= 0x%X)\n", RequestID);
		SelectedFilter->RequestID = RequestID;
		SelectedFilter->Started = TRUE;

	}

	//--> 이 Reference ID에 대해서 이미 start가 온 상태이면서 아직 start되지 않은 경우 filter를 setting하고 바로 start하도록 함.
	if( (s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].HaveToStart == TRUE) && (SelectedFilter->Started == FALSE) )
	{
		HxLOG_Info(" DemuxInfoIdx=%d, ResourceID=0x%x\n", DemuxInfoIdx,(HUINT32) s_stIR_DemuxInfo[DemuxInfoIdx].ResourceID);

		if(IA_SUCCESS != local_VdDmx_Make_OctoFilter(s_stIR_DemuxInfo[DemuxInfoIdx].ResourceID,
											s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].PID,
											&stFilterParam) )
		{
			HxLOG_Critical("\n\n");
			result = IA_FAIL;
			goto Exit;
		}
#if defined(VD_DUMP_FILTER)
		{
		int j;
		HxLOG_Print(" Filter info :  \n\tMode=0x%x\n\tTableFilter=0x%x\n\tCrc=%d\n\tUse_Ver_Num=%d\n\tPid =0x%X\n",
			stFilterParam.ucMode,	stFilterParam.bTableFilter, stFilterParam.bCrc, stFilterParam.bUseVerNum, stFilterParam.usPid);

		HxLOG_Print("Filter value info(length=%d) \n", SelectedFilter->wFilterLength );
		for(j=0;j < SelectedFilter->wFilterLength; j++)
		{
			HxLOG_Print("%02X ", SelectedFilter->FilterValue[j] );
		}
		HxLOG_Print("\nFilter mask info \n" );
		for(j=0;j < SelectedFilter->wFilterLength; j++)
		{
			HxLOG_Print("%02X ", SelectedFilter->FilterMask[j] );
		}
		HxLOG_Print("\n______________ \n" );
		}
#endif
		hErr = svc_cas_SubSvcDmxRetrieveSectionBase(&stFilterParam,
														SelectedFilter->FilterValue[0],
														SelectedFilter->FilterValue,
														SelectedFilter->FilterMask,
														SelectedFilter->wFilterLength,
														0,
														(void*)local_VdDmx_SecNoti_Callback,
														NULL,
														&RequestID);
		if (hErr != ERR_OK)
		{
			HxLOG_Print("svc_cas_SubSvcDmxRetrieveSectionBase return ERROR (%x)\n", hErr);
			result = IA_FAIL;
			goto Exit;
		}

		HxLOG_Print("svc_cas_SubSvcDmxRetrieveSectionBase OK !(RequestID= 0x%X)\n", RequestID);
		SelectedFilter->RequestID = RequestID;
		SelectedFilter->Started = TRUE;

	}

Exit:

	return result;
}

static ia_result local_VdDmx_StartFilter( vd_ref_id wRefId, ia_word32 wDataWriteSize, void *pvDataWrite, ia_word32 *pwDataReadSize, void *pvDataRead )
{
	HUINT32	i, DemuxInfoIdx, RefInfoIdx;
	ia_result	result = IA_SUCCESS;
	HUINT16	RequestID;
	HUINT32	hErr;

	HVD_DMX_FILTER_INFO_STRUCT *SelectedFilter;
	SvcCas_FilterParam_t stFilterParam;

	HxLOG_Info("wRefId(0x%X)\n", (HUINT32)wRefId);


	hErr = local_VdDmx_GetVDdemuxInfoIndex(wRefId, &DemuxInfoIdx, &RefInfoIdx);
	HxLOG_Print("ResourceID=%d, refID=0x%X (%d:%d)]\n", (HUINT32) 	s_stIR_DemuxInfo[DemuxInfoIdx].ResourceID,(HUINT32) wRefId, DemuxInfoIdx, RefInfoIdx);

	if( hErr == eHVD_OK )
	{
		/* For all filters on the channel */
		for( i = 0; i < HMX_VD_DMX_MAX_FILTER_PER_CHANNEL; i++ )
		{
			SelectedFilter = &(s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].IR_FilterInfo[i]);

			if( (SelectedFilter->Started == FALSE) && (SelectedFilter->Allocated == TRUE) )
			{
				if(IA_SUCCESS != local_VdDmx_Make_OctoFilter(s_stIR_DemuxInfo[DemuxInfoIdx].ResourceID,
											s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].PID,
											&stFilterParam))
				{
					HxLOG_Critical("\n\n");
					result = IA_FAIL;
					goto Exit;
				}
#if defined(VD_DUMP_FILTER)
				{
				int j;
				HxLOG_Print(" Filter info :  \n\tMode=0x%x\n\tTableFilter=0x%x\n\tCrc=%d\n\tUse_Ver_Num=%d\n\tPid =0x%X\n",
					stFilterParam.ucMode,	stFilterParam.bTableFilter, stFilterParam.bCrc, stFilterParam.bUseVerNum, stFilterParam.usPid);

				HxLOG_Print("Filter value info(length=%d) \n", SelectedFilter->wFilterLength );
				for(j=0;j < SelectedFilter->wFilterLength; j++)
				{
					HxLOG_Print("%02X ", SelectedFilter->FilterValue[j] );
				}
				HxLOG_Print("\nFilter mask info \n" );
				for(j=0;j < SelectedFilter->wFilterLength; j++)
				{
					HxLOG_Print("%02X ", SelectedFilter->FilterMask[j] );
				}
				HxLOG_Print("\n______________ \n" );
				}
#endif
				hErr = svc_cas_SubSvcDmxRetrieveSectionBase(&stFilterParam,
																SelectedFilter->FilterValue[0],
																SelectedFilter->FilterValue,
																SelectedFilter->FilterMask,
																SelectedFilter->wFilterLength,
																0,
																(void*)local_VdDmx_SecNoti_Callback,
																NULL,
																&RequestID);
				if (hErr != ERR_OK)
				{
					HxLOG_Print("svc_cas_SubSvcDmxRetrieveSectionBase return ERROR (%x)\n", hErr);
					result = IA_FAIL;
					goto Exit;
				}

				SelectedFilter->Started = TRUE;
				SelectedFilter->RequestID = RequestID;
				HxLOG_Info("svc_cas_SubSvcDmxRetrieveSectionBase OK !(RequestID= 0x%X)\n", RequestID);

			}
		}

		result = IA_SUCCESS;
		//--> Alloc/Set하지 않고 Start가 먼저 오는 경우가 있음 -> HaveToStart를 settin해두고 SetFilter에서 처리.
		s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].HaveToStart = TRUE;
	}
	else
	{
		HxLOG_Print(" IO(Start) - ERROR : invalid referenceID(0x%X)\n", (HUINT32) wRefId);
		result = IA_INVALID_PARAMETER;
	}

Exit:

	return result;
}

static ia_result local_VdDmx_StopFilter( vd_ref_id wRefId, ia_word32 wDataWriteSize, void *pvDataWrite, ia_word32 *pwDataReadSize, void *pvDataRead )
{
	HUINT32					i, DemuxInfoIdx, RefInfoIdx;
	ia_result				result = IA_SUCCESS;
	HUINT32					hErr;

	HxLOG_Info("wRefId(0x%X)\n", (HUINT32) wRefId);


	hErr= local_VdDmx_GetVDdemuxInfoIndex(wRefId, &DemuxInfoIdx, &RefInfoIdx);
	HxLOG_Print("[RscID=%d, wRefId= 0x%X, (dmxinfo idx:%d:ref id info:%d)] \n", \
		(HUINT32) s_stIR_DemuxInfo[DemuxInfoIdx].ResourceID, (HUINT32) wRefId,DemuxInfoIdx, RefInfoIdx);
	if( hErr != eHVD_OK )
	{
		HxLOG_Print(" invalid referenceID(0x%X)\n", (HUINT32)wRefId);
		result = IA_INVALID_PARAMETER;
		goto Exit;
	}

	for( i = 0; i < HMX_VD_DMX_MAX_FILTER_PER_CHANNEL; i++ )
	{
		if( s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].IR_FilterInfo[i].Started == TRUE )
		{
			hErr= svc_cas_SubSvcDmxStopOneSection(s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].IR_FilterInfo[i].RequestID);
			if (hErr != ERR_OK)
			{
				HxLOG_Print(" ERROR : MXI_SM_StopFilter: %08X\n", hErr);
				result = IA_FAIL;
				goto Exit;
			}

			s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].IR_FilterInfo[i].Started = FALSE;
			s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].IR_FilterInfo[i].RequestID = 0;
			result = IA_SUCCESS;
		}
	}
	s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].HaveToStart = FALSE;

Exit:

	return result;
}

static ia_result local_VdDmx_Reset( vd_ref_id wRefId, ia_word32 wDataWriteSize, void *pvDataWrite, ia_word32 *pwDataReadSize, void *pvDataRead )
{
	ia_result	result= IA_SUCCESS;
	HxLOG_Info("wRefId(0x%X)\n",(HUINT32)  wRefId);

#if (VD_FILTER_VERSION == 1)
	HUINT8	ret;
	HUINT32	DemuxInfoIdx, RefInfoIdx;


	ret = local_VdDmx_GetVDdemuxInfoIndex(wRefId, &DemuxInfoIdx, &RefInfoIdx);
	HxLOG_Print("IO (%d)[ResourceID#%d(%d:%d)] RESET\n", wRefId, s_stIR_DemuxInfo[DemuxInfoIdx].ResourceID, DemuxInfoIdx, RefInfoIdx);
	if( ret == eHVD_OK )
	{
		s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].pRead = s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].pBuffer;
		s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].pWrite = s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].pBuffer;
		s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].ReadPosition = 0;
		s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].WritePosition = 0;
		HxSTD_memset(s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].pBuffer, 0, s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].BufferLength);
		result = IA_SUCCESS;
	}
	else
	{
		HxLOG_Print(" IO(Reset) - ERROR : invalid referenceID(0x%X)\n", wRefId);
		result = IA_INVALID_PARAMETER;
	}
#else
	//nothing to do in vd dmx ver 2.
#endif


	return result;
}

static ia_result local_VdDmx_Make_OctoFilter(HUINT32 ulDemuxId, HUINT16 usPid, SvcCas_FilterParam_t *out_pstSiRetrieveParam )
{
	Handle_t hAct;
#ifdef CONFIG_DEBUG // remove warning
	HUINT32 ulTimeOut =0;
#endif
	HUINT32 ulActId;
	HUINT16 usTsUniqueId;
	SvcCas_Context_t *pContext;
	HERROR ulRet;
	HUINT32 ulTmpDmxId;

	CAS_IR_DRV_GetActionHandleByDemuxId((HUINT8)ulDemuxId, &hAct);
	pContext = CAS_IR_MGR_GetContextByAction(hAct);
	if (hAct == HANDLE_NULL || pContext == NULL)
	{
		HxLOG_Print(" Handle is invalid\n");
		return IA_FAIL;
	}
	usTsUniqueId = pContext->usUniqueId;
	ulActId = svc_cas_SubSvcRmGetActionId(hAct);

	ulRet = svc_cas_SubSvcRmGetResourceId(ulActId, eRxRSCTYPE_DEMUX, &ulTmpDmxId);
	if (ulRet != ERR_OK)
	{
		HxLOG_Print(" error:0x%x, ts_id:0x%x\n", ulRet, (HUINT32) usTsUniqueId);
		return IA_FAIL;
	}
#ifdef CONFIG_DEBUG // remove warning
	HxLOG_Print("Action[0x%x], Pid[0x%x], Timeout[%d], UniqueId [%x]\n",hAct,usPid, ulTimeOut, usTsUniqueId);
#endif
	ulRet = svc_cas_SubSvcDmxSetDefaultRetrieveParam(out_pstSiRetrieveParam, hAct, usTsUniqueId, (HUINT32)ulTmpDmxId);
	if (ulRet != ERR_OK)
	{
		HxLOG_Print("error:0x%x, ts_id:0x%x\n", ulRet, (HUINT32) hAct);
		return IA_FAIL;
	}

	out_pstSiRetrieveParam->ucMode = eSiFilterMode_Continue;
	out_pstSiRetrieveParam->ucTimeoutMode = eContinueTimeout_Repeat;

	out_pstSiRetrieveParam->bTableFilter = FALSE;
	out_pstSiRetrieveParam->bCrc = FALSE;
	out_pstSiRetrieveParam->bUseVerNum = FALSE;
	out_pstSiRetrieveParam->usPid = usPid;

	return IA_SUCCESS;

}

/********************************************************************
*	function name :
*	input :
*	output :
*	description :
********************************************************************/
static HUINT32	local_VdDmx_GetNeededIdxFromRequestID(HUINT32 RequestID, HUINT32 *DemuxInfoIdx, HUINT32 *RefInfoIdx, HUINT32 *FilterInfoIdx)
{
	HUINT32	i, j, k;

	for( i = 0; i < HMX_VD_DMX_MAX_NUM; i++ )
	{
		for( j = 0; j < HMX_VD_DMX_MAX_CHANNEL_PER_DEMUX; j++ )
		{
			if(s_stIR_DemuxInfo[i].RefInfo[j].ReferenceID == VD_REF_ID_ERROR)
				continue;

			for( k = 0; k < HMX_VD_DMX_MAX_FILTER_PER_CHANNEL; k++ )
			{
				if( (s_stIR_DemuxInfo[i].RefInfo[j].IR_FilterInfo[k].RequestID == RequestID) &&\
					(s_stIR_DemuxInfo[i].RefInfo[j].IR_FilterInfo[k].Allocated== TRUE) )
				{
					*DemuxInfoIdx = i;
					*RefInfoIdx = j;
					*FilterInfoIdx = k;
					return eHVD_OK;
				}
			}
		}
	}

	return eHVD_ERR;
}

static void local_VdDmx_SecNoti_Callback(HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, Handle_t hAction)
{
	PalSef_RawData_t *pSiRawData;
	HUINT16	usTableId, usFilterId, usTsUniqId, usFilterResult;
	HUINT32	DemuxInfoIdx, RefInfoIdx, FilterInfoIdx;
	HUINT16	nSecLength;
	HUINT32	result;

	//this mutex must be used for protecting stack's value overlab.
	DEMUX_SEM_SECNOTI_GET();


	// Get TableId, FilterId, UniqueId, Filter Result
	//	local_cassi_GetTableIdAndResult(ulTableIdAndResult, &usTableId, &usFilterResult);
	//	local_cassi_GetFilterAndTsUniqueId(ulFilterAndUniqId, &usFilterId, &usTsUniqId);
	local_VdDmx_GetTableIdAndResult(ulTableIdAndResult, &usTableId, &usFilterResult);
	local_VdDmx_GetFilterAndTsUniqueId(ulFilterAndUniqId, &usFilterId, &usTsUniqId);

	HxLOG_Info("ActionHandle (0x%X) TableId(0x%X), FilterId(0x%X), UniqueId(0x%X) \n", hAction, usTableId, usFilterId, usTsUniqId);

	if(hAction == HANDLE_NULL)
	{
		HxLOG_Info("Input param is invalid \n");
		goto Exit;
	}

	if(usFilterId == HMX_VD_DMX_INVALID_REQUESET_ID)
	{
		HxLOG_Info("Input param is invalid \n");
		goto Exit;
	}

	if(usFilterResult != eSiFilterResult_Complete)
	{
		HxLOG_Info("result = %s\n",(usFilterResult == eSiFilterResult_Timeout) ? "Time out" : "Error");
		goto Exit;
	}

	pSiRawData = (PalSef_RawData_t*)ulParam;
	if(pSiRawData == NULL)
	{
		HxLOG_Print(", Null section  \n");
		goto Exit;
	}

	nSecLength = get12bit( (pSiRawData->buf[0]) +1) + 3;
	/* usFilter ID로 s_stIR_DemuxInfo 의 resouce ID를 찾는다. */
	result = local_VdDmx_GetNeededIdxFromRequestID(usFilterId, &DemuxInfoIdx, &RefInfoIdx, &FilterInfoIdx );
	if( result != eHVD_OK )
	{
		// this case means ex-section table came to CAS module even though the filter was alread freed with requested ID
		// it's due to octo si system problem regarding async filter management. you can see first requested section after booting or when lots of filters are set.
		HxLOG_Print("[local_VdDmx_SecNoti_Callback]  ERROR : invalid request ID (0x%X)\n", usFilterId);
		goto Exit;
	}

#if (VD_FILTER_VERSION == 1)
	if(nSecLength > s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].HUMAX_vd_df_open_st.wMaxSectionSize )
#else
	if(nSecLength > s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].HUMAX_vd_df_open_v2_st.wMaxSectionSize )
#endif
	{
		// it couldn't occured in logical, but if occured, it shall be put into exception process.
		HxLOG_Critical("\n\n");
		goto Exit;
	}
	//hereby, it don't need to make allocated buffer because doing copy is done by softcell.
#if (VD_FILTER_VERSION == 1)
		if ( s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].HUMAX_vd_df_open_st.pfnReceiveNotify )
		{
			s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].HUMAX_vd_df_open_st.pfnReceiveNotify( \
												s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].ReferenceID,\
												s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].HUMAX_vd_df_open_st.wSoftCellVal\
												);
		}
#else
		if ( s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].HUMAX_vd_df_open_v2_st.pfnReceiveNotify )
		{
#ifdef CHECK_ECM_SECTION_NUM
#if defined(VD_DUMP_FILTER)
			if(((DVB_ECM_FILTER_SECTIOM_TAG(pSiRawData->buf[0]) == ECM_SECTION_TAG_1 || DVB_ECM_FILTER_SECTIOM_TAG(pSiRawData->buf[0]) == ECM_SECTION_TAG_2))
				&& (DVB_ECM_FILTER_SECTION_PAGE(pSiRawData->buf[0]) > (DVB_ECM_FILTER_SECTION_LAST_PAGE(pSiRawData->buf[0]))))
			{
				HUINT16 i;

				HxLOG_Info("Section Len[%d]\n",
							nSecLength);

				for(i = 1; i <= nSecLength; i++)
				{
					HxLOG_Info("[%02X]", pSiRawData->buf[0][i - 1]);

					if(i%16 == 0)
					{
						HxLOG_Info("\n");
					}
				}
				HxLOG_Info("\n");
			}
#endif
			if(((DVB_ECM_FILTER_SECTIOM_TAG(pSiRawData->buf[0]) == ECM_SECTION_TAG_1 || DVB_ECM_FILTER_SECTIOM_TAG(pSiRawData->buf[0]) == ECM_SECTION_TAG_2))
				&& (DVB_ECM_FILTER_SECTION_PAGE(pSiRawData->buf[0]) > (DVB_ECM_FILTER_SECTION_LAST_PAGE(pSiRawData->buf[0]))))
			{
				HxLOG_Print("ERROR : Invalid page No. cur[0x%02X] last[0x%02X]\n",
					DVB_ECM_FILTER_SECTION_PAGE(pSiRawData->buf[0]), DVB_ECM_FILTER_SECTION_LAST_PAGE(pSiRawData->buf[0]));
				goto Exit;
			}
#endif
			s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].HUMAX_vd_df_open_v2_st.pfnReceiveNotify( \
												s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].ReferenceID,\
												s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].HUMAX_vd_df_open_v2_st.wSoftCellVal,\
												nSecLength,\
												pSiRawData->buf[0]\
												);
		}
#endif


Exit:
	// TODO: free memory required ??

	DEMUX_SEM_SECNOTI_RELEASE();


	return;
}

/////////////////////////////////////////////////////////////
// Following functions are not used in VD octo's  but, here are as history
#if (VD_FILTER_VERSION ==1)

HVD_Result	local_VdDmx_AllocSectionBuffer(HUINT32	DemuxInfoIdx, HUINT32 RefInfoIdx, HUINT16 BufferLength)
{
	HUINT32 TotalBufferLength = BufferLength ;

	HxLOG_Print(" Alloc Buffer - DemuxInfoIdx(0x%X), RefInfoIdx(0x%X)\n", DemuxInfoIdx, RefInfoIdx);
	if( TotalBufferLength == 0 )
	{
		return eHVD_ERR;
	}

	// +2는 나중에 buffer가 wrap되어야 하는 경우 0x1234를 넣어두기 위한 공간.
	s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].pBuffer = OxCAS_Malloc(TotalBufferLength + 2);
	if( s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].pBuffer == NULL )
	{
		HxLOG_Print(" Alloc Fail\n");
		s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].pRead = NULL;
		s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].pWrite = NULL;
		s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].BufferLength = 0;
		s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].ReadPosition = 0;
		s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].WritePosition = 0;
		return eHVD_ERR;
	}
	else
	{
		HxLOG_Print(" &pBuffer(0x%X), TotalBufferLength(0x%X)\n", s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].pBuffer, TotalBufferLength);
		s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].pRead = s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].pBuffer;
		s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].pWrite = s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].pBuffer;
		s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].BufferLength = TotalBufferLength;
		s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].ReadPosition = 0;
		s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].WritePosition = 0;
		return eHVD_OK;
	}

}


HVD_Result	local_VdDmx_FreeSectionBuffer(HUINT32	DemuxInfoIdx, HUINT32 RefInfoIdx)
{
	HxLOG_Info("DemuxInfoIdx(0x%X), RefInfoIdx(0x%X)\n", DemuxInfoIdx, RefInfoIdx);
	if( s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].pBuffer != NULL )
	{
		if(s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].pBuffer!=NULL)
		{
			OxCAS_Free(s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].pBuffer);
			s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].pBuffer = NULL;
		}
		s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].pBuffer = NULL;
	}

	s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].pRead = NULL;
	s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].pWrite = NULL;
	s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].BufferLength = 0;
	s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].ReadPosition = 0;
	s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].WritePosition = 0;

	return eHVD_OK;
}
ia_result	local_VdDmx_WriteSection(HUINT32 RequestID, HUINT8 *pRaw)
{
	HUINT32	DemuxInfoIdx, RefInfoIdx, FilterInfoIdx;
	HUINT16	nSecLength;
	HUINT32	result;

	result = local_VdDmx_GetNeededIdxFromRequestID(RequestID, &DemuxInfoIdx, &RefInfoIdx, &FilterInfoIdx );
	if( result != eHVD_OK )
	{
		HxLOG_Print(" Write - ERROR : invalid request ID (0x%X)\n", RequestID);
		return eHVD_ERR;
	}

	nSecLength = get12bit(pRaw + 1) + 3;

	// EMM의 Max. section size는 1024를 넘을 수 없다.
	// I-CORD에서 EMM section이 이상하게 올라오는 경우가 있다. 근본적으로 수정을 해야겠지만, ( 흠.. 누가.. )
	// 여기서도 error처리를 해주자.
	if( 1024 < (nSecLength + 3) )
	{
#if defined(CONFIG_DEBUG)
		HUINT8	i;

		HxLOG_Print("\n\n#############\n Write - ERROR : CORRUPTED EMM!!!!\n");

		for( i = 0; i < 64; i++ )
		{
			HxLOG_Print("%02X ", pRaw[i]);
			if( i % 16 == 15 )
				HxLOG_Print("\n");
		}
		HxLOG_Print("#############\n");
#endif
		return eHVD_ERR;
	}

	// 하나의 section에 대해서 buffer가 linear해야 하므로 남은 buffer의 양이 하나의 section보다 작을 경우 buffer의 처음으로 돌아감.
	// +4는 filter ID를 적을 공간.
	HxLOG_Print("[pipeHandle#%d(%d:%d)]Write - WP(0x%X), Len(0x%X) => ",\
							s_stIR_DemuxInfo[DemuxInfoIdx].ResourceID, DemuxInfoIdx, RefInfoIdx, s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].WritePosition, nSecLength));

	if( (s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].WritePosition + nSecLength + 4) >= s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].BufferLength)
	{
		HxLOG_Print("BUFFER WRAP\n");
		// 처음으로 돌아갈 경우 다음 위치에 0x1234를 적어두어서 read pointer도 처음으로 돌아가게 함.
		*( s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].pWrite ) = 0x12;
		*( s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].pWrite + 1) = 0x34;

		s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].pWrite = s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].pBuffer;
		s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].WritePosition = 0;
	}

	HxLOG_Print("FIlterID(0x%X), Data(%02X, %02X)\n", FilterInfoIdx, pRaw[0], pRaw[3]);
	HxSTD_memcpy(s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].pWrite, (HUINT8 *)&FilterInfoIdx, sizeof(HUINT32));
	s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].pWrite += sizeof(HUINT32);
	HxSTD_memcpy(s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].pWrite, pRaw, nSecLength);

	s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].WritePosition += (nSecLength + sizeof(HUINT32));
	s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].pWrite += nSecLength;

	if( s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].HUMAX_vd_df_open_st.pfnReceiveNotify )
	{
		HxLOG_Print("\t\t CallBack : ReferenceID(0x%X)\n", s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].ReferenceID);
		s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].HUMAX_vd_df_open_st.pfnReceiveNotify(s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].ReferenceID, s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].HUMAX_vd_df_open_st.wSoftCellVal);
	}

	return eHVD_OK;

}

ia_result	local_VdDmx_ReadSection( vd_ref_id wRefId, ia_word32 wDataWriteSize, void *pvDataWrite, ia_word32 *pwDataReadSize, void *pvDataRead )
{
	vd_df_read_st	DF_Read;
	HUINT8			ret;
	ia_result		result;
	HUINT32			FilterID, DemuxInfoIdx, RefInfoIdx;
	HUINT8			*p = pvDataRead;

	// prevent fix(added)
	if ( (pwDataReadSize == NULL) || (pvDataRead == NULL))
	{
		HxLOG_Critical("\n\n");
		return IA_INVALID_PARAMETER;
	}

	ret = local_VdDmx_GetVDdemuxInfoIndex(wRefId, &DemuxInfoIdx, &RefInfoIdx);
	HxLOG_Print("IO (%d)[pipeHandle#%d(%d:%d)]READ => ", wRefId, s_stIR_DemuxInfo[DemuxInfoIdx].ResourceID, DemuxInfoIdx, RefInfoIdx);
	if( ret == eHVD_OK )
	{
		if( s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].pBuffer == NULL )
		{
			HxLOG_Print("\n IO - ERROR(Read) : pBuffer of DemuxInfoIndex(0x%X), RefInfoIdx(0x%X) is NULL\n", DemuxInfoIdx, RefInfoIdx);
			result = IA_FAIL;
		}
		else
		{
			if( s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].ReadPosition == s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].WritePosition )
			{
				HxLOG_Print("\n IO - ERROR(Read) : no available section DemuxInfoIndex(0x%X), RefInfoIdx(0x%X)\n", DemuxInfoIdx, RefInfoIdx);
				*pwDataReadSize = 0;
				result = IA_SUCCESS;
			}
			else
			{
				// Read pointer로 부터 읽어온 값이 0x1234인 경우는 write pointer가 buffer의 처음으로 돌아간 경우이므로 read pointer도 처음으로 돌림.
				if( (*(s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].pRead) == 0x12) && (*(s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].pRead+1) == 0x34) )
				{
					*(s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].pRead) = 0;
					*(s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].pRead+1) = 0;
					s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].pRead = s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].pBuffer;
					s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].ReadPosition = 0;
				}

				HxSTD_memcpy((HUINT8 *)&FilterID, s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].pRead, sizeof(HUINT32));
				DF_Read.wFilterId = FilterID;
				DF_Read.wSectionLength = get12bit(s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].pRead + sizeof(HUINT32) + 1) + 3;

				if( *pwDataReadSize < DF_Read.wSectionLength + sizeof(vd_df_read_st) )
				{
					HxLOG_Print("\n IO - ERROR(Read) : not enough ReadDataBufferSize( *pwDataReadSize = %d : DF_Read.wSectionLength = %d : sizeof(vd_df_read_st) = %d\n",
								*pwDataReadSize, DF_Read.wSectionLength, sizeof(vd_df_read_st) ) );
					*pwDataReadSize = DF_Read.wSectionLength + sizeof(vd_df_read_st);
					result = IA_FULL;
				}
				else
				{
					HxSTD_memset(s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].pRead, 0, sizeof(HUINT32));	// 앞에서 Filter ID 읽어낸 곳 clear.
					s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].pRead += sizeof(HUINT32);
					HxSTD_memcpy(p, (HUINT8 *)&DF_Read, sizeof(vd_df_read_st));
					p += sizeof(vd_df_read_st);
					HxSTD_memcpy(p, s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].pRead, DF_Read.wSectionLength);

					HxLOG_Print("RP(0x%X), FID(0x%X), Len(0x%X), TID(0x%X)\n", s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].ReadPosition, DF_Read.wFilterId, DF_Read.wSectionLength, p[0]);

					HxSTD_memset(s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].pRead, 0, DF_Read.wSectionLength);
					s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].pRead += DF_Read.wSectionLength;
					s_stIR_DemuxInfo[DemuxInfoIdx].RefInfo[RefInfoIdx].ReadPosition += (sizeof(HUINT32) + DF_Read.wSectionLength);
					result = IA_SUCCESS;
				}

			}
		}

	}
	else
	{
		HxLOG_Print("\n IO - ERROR(Read) : invalid referenceID(0x%X)\n", wRefId);
		result = IA_INVALID_PARAMETER;
	}
	return result;
}
#endif

#define ____VD_DMX_API______________
ia_result HVD_DMX_Initialise(void)
{
	ia_result	result = IA_SUCCESS;
	HUINT32		i, j, k;
	HUINT32		ulRet;

	HxLOG_Info("\n");

	HxSTD_memset(s_stIR_DemuxInfo, 0, sizeof(HVD_DMX_DEMUX_INFO_STRUCT) * HMX_VD_DMX_MAX_NUM);
	HxSTD_memset(s_bDmxRefIdInfo, 0 , sizeof (s_bDmxRefIdInfo));

	for( i = 0; i < HMX_VD_DMX_MAX_NUM; i++ )
	{
		s_stIR_DemuxInfo[i].ResourceID = VD_RSC_ID_INVALID;

		for( j = 0; j < HMX_VD_DMX_MAX_CHANNEL_PER_DEMUX; j++ )
		{
			s_stIR_DemuxInfo[i].RefInfo[j].ReferenceID = VD_REF_ID_ERROR;

			// whole ref id clear.
			local_VdDmx_ResetReferenceID( (i *HMX_VD_DMX_MAX_CHANNEL_PER_DEMUX) + j);
			// clear filter info
			for(k=0; k< HMX_VD_DMX_MAX_FILTER_PER_CHANNEL; k++)
			{
				local_VdDmx_ClearOneFilterInfo( &(s_stIR_DemuxInfo[i].RefInfo[j].IR_FilterInfo[k]) );
			}
		}
	}


	ulRet = VK_SEM_Create(&s_ulDMUXAPISem, "s_ulDMUXAPISem", VK_SUSPENDTYPE_FIFO);
	if( ulRet != VK_OK )
	{
		HxLOG_Print(" Semaphore Create Error, s_ulDMUXAPISem\n");
		HxLOG_Critical("\n\n");
		result = IA_FAIL;
	}

	ulRet = VK_SEM_Create(&s_ulDMUXNotiSecSem, "s_ulDMUXNotiSecSem", VK_SUSPENDTYPE_FIFO);
	if( ulRet != VK_OK )
	{
		HxLOG_Print(" Semaphore Create Error, s_ulDMUXNotiSecSem\n");
		HxLOG_Critical("\n\n");
		result = IA_FAIL;
	}


	g_stEventCB.pfnEventNotify = NULL;

	return result;
}

HUINT32 HVD_DMX_GetResources(void **ppvList, ia_word16 *pwCount )
{
	//currently it's not used.
	HxLOG_Info("\n");
#if (VD_FILTER_VERSION ==1)
	HUINT8				i;
	ia_word16			Count = HMX_VD_DMX_MAX_NUM;
	vd_demux_resource	*List;

	*pwCount = Count;

	List = (vd_demux_resource *)OxCAS_Malloc(Count * sizeof(vd_rsc_id));

	for( i = 0; i < Count; i++ )
	{
		List->awResources[i] = i;
	}

	*ppvList = List;
	return eHVD_OK;
#else
	HxLOG_Info("It must not be called. \n");
	return eHVD_OK;
#endif

}


vd_ref_id HVD_DMX_Open( vd_rsc_id wResourceID, void *pvData )
{

	HUINT8			index;
	HUINT32			RefIdx;
	vd_ref_id		referenceID;

#if (VD_FILTER_VERSION ==1)
	vd_df_open_st 	*pvd_df_open;
	pvd_df_open = (vd_df_open_st *)pvData;
#else
	vd_df_open_v2_st	*pvd_df_v2_open, *pvd_df_v2_temp;
	pvd_df_v2_open = ( vd_df_open_v2_st *) pvData;
#endif

	HxLOG_Info("wResourceID(0x%X), pvData@(0x%X)\n", (HUINT32) wResourceID, (HUINT32) pvData);

	DEMUX_SEM_GET();

	referenceID = local_VdDmx_GetAvailableReferenceID();
	if( referenceID == VD_REF_ID_ERROR )
	{
		HxLOG_Print(" - ERROR for GetAvailableReferenceID\n");
		DEMUX_SEM_RELEASE();
		return VD_REF_ID_ERROR;
	}

	HxLOG_Print(" - wResourceID(0x%X), referenceID(0x%X)\n",(HUINT32)  wResourceID, (HUINT32) referenceID);

	if( eHVD_OK != local_VdDmx_SetVDdemuxInfo(wResourceID, referenceID, &index, &RefIdx) )
	{
		HxLOG_Print(" - ERROR for SetVDdemuxInfo\n");
		s_bDmxRefIdInfo[referenceID] = FALSE;
		DEMUX_SEM_RELEASE();
		return VD_REF_ID_ERROR;
	}

#if (VD_FILTER_VERSION ==1)

	if( local_VdDmx_AllocSectionBuffer(index, RefIdx, pvd_df_open->wMaxSectionSize) == HIR_NOT_OK )
	{
		HxLOG_Print(" - ERROR : OutOfMemory\n");
		local_VdDmx_ResetReferenceID(referenceID);
		DEMUX_SEM_RELEASE();
		return VD_OUT_OF_MEMORY;
	}
	else
	{
		s_stIR_DemuxInfo[index].RefInfo[RefIdx].HUMAX_vd_df_open_st.pfnReceiveNotify = pvd_df_open->pfnReceiveNotify;
		s_stIR_DemuxInfo[index].RefInfo[RefIdx].HUMAX_vd_df_open_st.wSoftCellVal = pvd_df_open->wSoftCellVal;
		s_stIR_DemuxInfo[index].RefInfo[RefIdx].HUMAX_vd_df_open_st.wMaxSectionSize = pvd_df_open->wMaxSectionSize;
		s_stIR_DemuxInfo[index].RefInfo[RefIdx].HUMAX_vd_df_open_st.eChannelType = pvd_df_open->eChannelType;
	}
	HxLOG_Print(" Notify CB(0x%X), wSoftCellVal(0x%X), MaxSectionSize(0x%X), eChannelType(0x%X)\n",\
					pvd_df_open->pfnReceiveNotify, pvd_df_open->wSoftCellVal, pvd_df_open->wMaxSectionSize, pvd_df_open->eChannelType));
#else
	pvd_df_v2_temp = &(s_stIR_DemuxInfo[index].RefInfo[RefIdx].HUMAX_vd_df_open_v2_st);
	pvd_df_v2_temp->pfnReceiveNotify = pvd_df_v2_open->pfnReceiveNotify;
	pvd_df_v2_temp->wSoftCellVal = pvd_df_v2_open->wSoftCellVal;
	pvd_df_v2_temp->wMaxSectionSize = pvd_df_v2_open->wMaxSectionSize;
	pvd_df_v2_temp->eChannelType =  pvd_df_v2_open->eChannelType;
	pvd_df_v2_temp->eTransportType = pvd_df_v2_open->eTransportType;

#endif

	DEMUX_SEM_RELEASE();
	return referenceID;
}

ia_result HVD_DMX_Close( vd_ref_id wRefId )
{
	HINT32		i, j;

	HxLOG_Info("wRefId(0x%X)\n", (HUINT32) wRefId);

	DEMUX_SEM_GET();

	local_VdDmx_ResetReferenceID(wRefId);

	for( i = 0; i < HMX_VD_DMX_MAX_NUM; i++ )
	{
		for( j = 0; j < HMX_VD_DMX_MAX_CHANNEL_PER_DEMUX; j++ )
		{
			if( s_stIR_DemuxInfo[i].RefInfo[j].ReferenceID == wRefId )
			{
#if (VD_FILTER_VERSION ==1)
				local_VdDmx_FreeSectionBuffer(i, j);
#endif
				HxSTD_memset((HUINT8 *)&(s_stIR_DemuxInfo[i].RefInfo[j]), 0, sizeof(HVD_DMX_REF_INFO_STRUCT));
				s_stIR_DemuxInfo[i].RefInfo[j].ReferenceID = VD_REF_ID_ERROR;
			}
		}

		for( j = 0; j < HMX_VD_DMX_MAX_CHANNEL_PER_DEMUX; j++ )
		{
			if( s_stIR_DemuxInfo[i].RefInfo[j].ReferenceID != VD_REF_ID_ERROR )
				break;
		}

		if( j == HMX_VD_DMX_MAX_CHANNEL_PER_DEMUX )
		{
			s_stIR_DemuxInfo[i].ResourceID= VD_RSC_ID_INVALID;
		}

	}
	DEMUX_SEM_RELEASE();
	return IA_SUCCESS;
}

ia_result HVD_DMX_IO( vd_ref_id wRefId, ia_word16 wOpCode, ia_word32 wDataWriteSize, void *pvDataWrite, ia_word32 *pwDataReadSize, void *pvDataRead )
{
	ia_result				result = IA_SUCCESS;

	HxLOG_Info("wRefId(0x%X), wOpCode(0x%X), wDataWriteSize(0x%X)\n", (HUINT32) wRefId, (HUINT32) wOpCode, (HUINT32) wDataWriteSize);

	DEMUX_SEM_GET();

	switch(wOpCode)
	{
		case VD_DF_ALLOCATE_FILTER:
			result = local_VdDmx_AllocateFilter(wRefId, wDataWriteSize, pvDataWrite, pwDataReadSize, pvDataRead);
			break;

		case VD_DF_FREE_FILTER:
			result = local_VdDmx_FreeFilter(wRefId, wDataWriteSize, pvDataWrite, pwDataReadSize, pvDataRead);
			break;

		case VD_DF_SET_PID:
			result = local_VdDmx_SetPid(wRefId, wDataWriteSize, pvDataWrite, pwDataReadSize, pvDataRead);
			break;

		case VD_DF_SET_FILTER:
			result = local_VdDmx_SetFilter(wRefId, wDataWriteSize, pvDataWrite, pwDataReadSize, pvDataRead);
			break;
#if (VD_FILTER_VERSION == 1)
		case VD_DF_READ_SECTION:
			result = local_VdDmx_ReadSection(wRefId, wDataWriteSize, pvDataWrite, pwDataReadSize, pvDataRead);
			break;
#endif
		case VD_DF_START:
			result = local_VdDmx_StartFilter(wRefId, wDataWriteSize, pvDataWrite, pwDataReadSize, pvDataRead);
			break;

		case VD_DF_STOP:
			result = local_VdDmx_StopFilter(wRefId, wDataWriteSize, pvDataWrite, pwDataReadSize, pvDataRead);
			break;

		case VD_DF_RESET:
			result = local_VdDmx_Reset(wRefId, wDataWriteSize, pvDataWrite, pwDataReadSize, pvDataRead);
			break;

		case VD_DF_CONFIG_EVENT_CALLBACK:
			if (pvDataWrite != NULL)
			{
				vd_df_config_event_st	*pstEvent = (vd_df_config_event_st *) pvDataWrite;

				g_stEventCB.pfnEventNotify = pstEvent->pfnEventNotify;
				HxLOG_Print("\t IO - VD_DF_CONFIG_EVENT_CALLBACK ########## (0x%X)\n", (HUINT32) wOpCode);
			}
			break;

		default:
			HxLOG_Print("\t IO - INVALID OpCode(0x%X)\n", (HUINT32) wOpCode);
			result = VD_NOT_SUPPORTED;
			break;
	}

	DEMUX_SEM_RELEASE();
	return result;
}

HUINT32 HVD_DMX_SendJumpEvent(void)
{
	HUINT32 i, j, k;
	vd_ref_id	RefId[HMX_VD_DMX_MAX_CHANNEL_PER_DEMUX] = {VD_REF_ID_ERROR, };
	ia_word32	SoftCellVal[HMX_VD_DMX_MAX_CHANNEL_PER_DEMUX] = {0, };
	HINT32		nCnt = 0;


	if( g_stEventCB.pfnEventNotify == NULL)
		return 0;

	for (i=0; i<HMX_VD_DMX_MAX_NUM; i++)
	{
		DEMUX_SEM_GET();
		if (s_stIR_DemuxInfo[i].ResourceID == VD_RSC_ID_INVALID)
		{
			DEMUX_SEM_RELEASE();
			continue;
		}

		nCnt = 0;
		for (j=0; j<HMX_VD_DMX_MAX_CHANNEL_PER_DEMUX; j++)
		{
			if (s_stIR_DemuxInfo[i].RefInfo[j].ReferenceID == VD_REF_ID_ERROR)
				continue;

			HxLOG_Print("\t %d : reference : %d, PID : %04X, HaveToStart (%d)\n", j,
					(HUINT32)s_stIR_DemuxInfo[i].RefInfo[j].ReferenceID,
					s_stIR_DemuxInfo[i].RefInfo[j].PID,
					s_stIR_DemuxInfo[i].RefInfo[j].HaveToStart);

			RefId[nCnt] = s_stIR_DemuxInfo[i].RefInfo[j].ReferenceID;
#if (VD_FILTER_VERSION == 1)
			SoftCellVal[nCnt] = s_stIR_DemuxInfo[i].RefInfo[j].HUMAX_vd_df_open_st.wSoftCellVal;
#else
			SoftCellVal[nCnt] = s_stIR_DemuxInfo[i].RefInfo[j].HUMAX_vd_df_open_v2_st.wSoftCellVal;
#endif
			nCnt++;
		}

		DEMUX_SEM_RELEASE();

		if (nCnt > 0)
		{

			for (k = 0; k < nCnt; k++)
			{
				HxLOG_Print("\t IO - Send VD_DF_EVENT_JUMP ########## (%d - SoftcellVal - %d)\n", (unsigned int)RefId[k], (unsigned int)SoftCellVal[k]);
				g_stEventCB.pfnEventNotify(RefId[k], SoftCellVal[k], VD_DF_EVENT_JUMP, 0, 0);
			}
		}

	}


	return 1;
}

#define _____VD_DMX_DEBUG_API____

#if defined(CONFIG_DEBUG)
HUINT32 HVD_DMX_PrintDemuxInfo(void)
{
	HUINT32 i, j;

	DEMUX_SEM_GET();

	for (i=0; i<HMX_VD_DMX_MAX_NUM; i++)
	{
		HLIB_CMD_Printf("IR_DEMUX (%d) ===========================================\n", i);

		if (s_stIR_DemuxInfo[i].ResourceID != VD_RSC_ID_INVALID)
		{
			for (j=0; j<HMX_VD_DMX_MAX_CHANNEL_PER_DEMUX; j++)
			{
				HLIB_CMD_Printf("\t %d : reference : %d, PID : %04X, HaveToStart (%d)\n",
					j,
					(HUINT32)s_stIR_DemuxInfo[i].RefInfo[j].ReferenceID,
					s_stIR_DemuxInfo[i].RefInfo[j].PID,
					s_stIR_DemuxInfo[i].RefInfo[j].HaveToStart);
			}
		}
	}
	DEMUX_SEM_RELEASE();
	return ERR_OK;
}
#endif

/* end of file */

