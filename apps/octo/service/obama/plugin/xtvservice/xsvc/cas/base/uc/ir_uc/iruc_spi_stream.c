/*
 * (c) 2011-2012 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include "octo_common.h"
#include "util.h"

#include <pal_pipe.h>
#include "pal_sef.h"
#include "pal_dsc.h"

#include "_svc_cas_mgr_common.h"
#include "_svc_cas_mgr_main.h"

#include "UniversalClient_Common_SPI.h"
#include "UniversalClient_Common_API.h"

#include "UniversalClient_API.h"

#include "iruc_mgr.h"
#include "iruc_drv.h"
#include "iruc_spi.h"
#include "iruc_drv.h"

#include <_svc_cas_sub_dev_api.h>
#include <_svc_cas_sub_svc_api.h>

HULONG	s_ulDMUXAPISem;

/*
	Digiturk, Skylink Live signal중에서 ECM Section page number가 Last section page number보다 큰 경우에 hang-up되는 증상이 발생.
	IRDETO측에서 이런 경우 Softcell로 Section data를 넘겨 주지 말아 달라고 하여 아래 코드 추가.
	ECM Section Num 에 대한 예외처리 이므로 define 은 무조건 켜주기 바람.
*/
//common define으로 적용.
#define	CHECK_ECM_SECTION_NUM

#ifdef CHECK_ECM_SECTION_NUM
#define ECM_SECTION_TAG_1	0x80
#define ECM_SECTION_TAG_2 	0x81

#define DVB_ECM_FILTER_SECTIOM_TAG(section)			section[0]
#define DVB_ECM_FILTER_SECTION_PAGE(section)			section[4]
#define DVB_ECM_FILTER_SECTION_LAST_PAGE(section)	section[5]
#endif

// debug 용도.
//#define PRINT_CW
// Local declaration
//#define IRUC_STREAM_SECTION_DEBUG // dump section & cw
// #define STREAM_DBG_FILTERFUMP

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HBOOL	bAllocated;
	uc_connection_handle connectionHandle;
	uc_notify_callback notifyCallback;

}IRUC_STREAM_CONNECT_INFO;

typedef struct
{
	HUINT32		AllocRefID;
	HUINT16		RequestID; //it represents filter id in octo system.
	HUINT8		Allocated;
	HUINT8		Started;
	HUINT8		HaveToStart;
	HUINT16		wFilterLength;
	HUINT8		aucFilterValue[IRUC_STREAM_FILTER_MAX_LEN];
	HUINT8		aucFilterMask[IRUC_STREAM_FILTER_MAX_LEN];
} IRUC_STREAM_FILTER_INFO;

typedef struct
{
	HBOOL	bAllocated;
	HBOOL	bOtherAllocated;
	HBOOL	bVirualComponent;

	HUINT32 	ulPalDscIdx;
	HUINT16 	usOtherPid;
	uc_elementary_component stUC_ES_COMPONENT;

	HUINT8 aucEven[8];
	HUINT8 aucOdd[8];

}IRUC_STREAM_COMPONENT;

typedef struct
{
	HBOOL	bUsed;
	HUINT32	SvcContext;

	uc_connection_stream_type		ucConnStreamType;
	HUINT16	usPid;

	IRUC_STREAM_CONNECT_INFO astConnection; //[IRUC_STREAM_MAX_CONNECTION];
	IRUC_STREAM_FILTER_INFO astFilter[IRUC_STREAM_MAX_FILTER_PER_CHANNEL];

	IRUC_STREAM_COMPONENT astComponent[IRUC_STREAM_MAX_COMPONENT];

} IRUC_STREAM_INFO;

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static IRUC_STREAM_INFO s_iruc_stream_info[IRUC_STREAM_MAX_TOTAL_CHANNEL];
static HUINT32	s_UcFilterRefIdInfo[IRUC_STREAM_MAX_TOTAL_FILTER]; ////if ref id is allocated, it has 1,(TRUE) otherwise 0(FALSE)

// 하기 static value가 null 되는 이슈가 발생하여 임시로 값을 할당함..
static HULONG s_ulUcStreamSem = 0x919cfa;
static HULONG s_ulUcStreamNotiSem = 0x911cea;

#define __NOTUSE_MUTEX__ // hjlee3 : defuault로 muxtex 사용 하지 않게 변경 한다.
#if defined( __NOTUSE_MUTEX__)
#define STREAM_SEM_GET						VK_SEM_Get(s_ulUcStreamSem)
#define STREAM_SEM_RELEASE					VK_SEM_Release(s_ulUcStreamSem)
#else
#define STREAM_SEM_GET						VK_MUTEX_Lock(s_ulUcStreamSem)
#define STREAM_SEM_RELEASE					VK_MUTEX_Unlock(s_ulUcStreamSem)
#endif
#define STREAM_NOTI_SEM_GET			VK_SEM_Get(s_ulUcStreamNotiSem)
#define STREAM_NOTI_SEM_RELEASE		VK_SEM_Release(s_ulUcStreamNotiSem)


/*******************************************************************/
/********************      local Functions     ********************/
/*******************************************************************/
HINT8* local_cas_iruc_Dbg_PrintStreamType(uc_connection_stream_type streamType);
HERROR local_cas_iruc_STREAM_ReInitComponent(uc_stream_handle streamHandle, HUINT16 usComponentIdx, HUINT16 usPid);
void  local_cas_iruc_STREAM_RemoveComponent(	uc_stream_handle streamHandle);
/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/


#define ______LOCAL_FUNCTION______

void local_cas_iruc_STREAM_CONVERT_FILTER(IRUC_STREAM_FILTER_INFO *pSelectedFilter ,
HUINT32 *pPalFilter_Length, HUINT8*pPalFilter_Value, HUINT8 *pPalFilter_Mask )
{

//	HxLOG_Print(" Convert Value and Mask, Length to adapt Octo filter system \n");
	if( pSelectedFilter->wFilterLength > 2 )
	{
		*pPalFilter_Length = pSelectedFilter->wFilterLength - 2; // section length item size included.
		pPalFilter_Value[0] =  pSelectedFilter->aucFilterValue[0] ;
		pPalFilter_Mask[0] =  pSelectedFilter->aucFilterMask[0] ;

		VK_MEM_Memcpy( &(pPalFilter_Value[1]), &(pSelectedFilter->aucFilterValue[3]), *pPalFilter_Length );
		VK_MEM_Memcpy (&(pPalFilter_Mask[1]), &(pSelectedFilter->aucFilterMask[3]), *pPalFilter_Length );

#if 0 //defSTREAM_DBG_FILTERFUMP
// hjlee3, test temp
//		if(pPalFilter_Value[0] == 0x82)
		{
			for(i=0; i< *pPalFilter_Length; i++)
			{
				VK_Print("[%02X]", pPalFilter_Value[i]);
			}
			VK_Print("\n");
			for(i=0; i< *pPalFilter_Length; i++)
			{
				VK_Print("[%02X]", pPalFilter_Mask[i]);
			}
			VK_Print("\n");
		}
#endif
	}
	else if(pSelectedFilter->wFilterLength ==  2)
	{
//		HxLOG_Print("Filter length = 2\n");

		pPalFilter_Value[0] =  pSelectedFilter->aucFilterValue[0] ;
		pPalFilter_Mask[0] =  pSelectedFilter->aucFilterMask[0] ;

		// even though you wannan receive open-channel data, the length shall be set. it's okay because of opened-mask.
		*pPalFilter_Length = 1;
	}
	else
	{
//		HxLOG_Print("Abnormal Filter length(=%d) is detected.\n", pSelectedFilter->wFilterLength );

		pPalFilter_Value[0] =  pSelectedFilter->aucFilterValue[0] ;
		pPalFilter_Mask[0] =  pSelectedFilter->aucFilterMask[0] ;

		// even though you wannan receive open-channel data, the length shall be set. it's okay because of opened-mask.
		*pPalFilter_Length = IRUC_STREAM_FILTER_MAX_LEN;
	}

}

static void local_cas_iruc_STREAM_CB_GetTableIdAndResult(HUINT32 value, HUINT16* usTableId, HUINT16 *result)
{
	*result = value & 0x0000ffff;
	*usTableId = (value & 0xffff0000) >> 16;
}

static void local_cas_iruc_STREAM_CB_GetFilterAndTsUniqueId(HUINT32 value, HUINT16 *usFilterId, HUINT16 *usTsUniqId)
{
	*usTsUniqId = value & 0x0000ffff;
	*usFilterId = (value & 0xffff0000) >> 16;
}

HINT32 local_cas_iruc_STREAM_Make_OctoFilter(Handle_t hAct, HUINT16 usPid, SiRetrieveParam_t *out_pstSiRetrieveParam )
{
	HUINT32			ulActId;
	HUINT16			usTsUniqueId;
	SvcCas_Context_t	*pContext;
	HERROR			ulRet;
	HUINT32	ulDemuxId;

//    HxLOG_Print("[%s] hAct=0x%X, HUINT16 usPid= 0x%X)\n",__FUNCTION__, (HUINT32) hAct, usPid );

	pContext = CAS_IRUC_MGR_GetContextByAction(hAct);
	if (hAct == HANDLE_NULL || pContext == NULL)
	{
		HxLOG_Print("[%s]  Handle is invalid\n",__FUNCTION__);
                return ERR_FAIL;
	}

	usTsUniqueId = pContext->usUniqueId;
	ulActId = svc_cas_SubSvcRmGetActionId(hAct);

	ulRet = svc_cas_SubSvcRmGetResourceId(ulActId, eRxRSCTYPE_DEMUX, &ulDemuxId);
	if (ulRet != ERR_OK)
	{
		HxLOG_Print("[%s]  error:0x%x, ts_id:0x%x\n",__FUNCTION__, ulRet, hAct);
		return ERR_FAIL;
	}

//	HxLOG_Print("[%s] Action[0x%x], Pid[0x%x], Timeout[%d], UniqueId [%x], ulDemuxId[%d]\n",__FUNCTION__,	hAct,usPid, ulTimeOut, usTsUniqueId, ulDemuxId);

	ulRet = svc_cas_SubSvcDmxSetDefaultRetrieveParam((svcCas_SiRetrieveParam_t*)out_pstSiRetrieveParam, hAct, usTsUniqueId, (HUINT32)ulDemuxId);
	if (ulRet != ERR_OK)
	{
		HxLOG_Print("[%s] error:0x%x, ts_id:0x%x\n",__FUNCTION__, ulRet, hAct);
		return ERR_FAIL;
	}

	out_pstSiRetrieveParam->ucMode = eSiFilterMode_Continue;
	out_pstSiRetrieveParam->ucTimeoutMode = eContinueTimeout_Repeat;

	out_pstSiRetrieveParam->bTableFilter = FALSE;
	out_pstSiRetrieveParam->bCrc = FALSE;
	out_pstSiRetrieveParam->bUseVerNum = FALSE;
	out_pstSiRetrieveParam->usPid = usPid;

	return ERR_OK;
}

HUINT32 local_cas_iruc_STREAM_GetVacancy_FilterRefID(void)
{
	HUINT32	i = 0;

//	HxLOG_Print("\n");

	for( i = 0; i < IRUC_STREAM_MAX_TOTAL_FILTER; i++ )
	{
		if( s_UcFilterRefIdInfo[i] != TRUE )
		{
			s_UcFilterRefIdInfo[i] = TRUE;
			return i;
		}
	}

	HxLOG_Print("[%s] ERROR : no more available reference ID\n",__FUNCTION__);
	return IRUC_STREAM_MAX_TOTAL_FILTER;
}

void local_cas_iruc_STREAM_Release_FilterRefID(HINT32 idx)
{
	HxLOG_Print("[%s] : release RefId = %d\n",__FUNCTION__, idx);
	if( (idx < IRUC_STREAM_MAX_TOTAL_FILTER) && (idx != IRUC_INVALID_FILTER_REFID) )
	{
		s_UcFilterRefIdInfo[idx] = FALSE;
//		HxLOG_Print("[%s] s_UcFilterRefIdInfo[%d]= %d \n",__FUNCTION__, idx, s_UcFilterRefIdInfo[idx] );
	}
	else
	{
		HxLOG_Print("[%s]  ERROR : Not Allocate Filter reference ID\n",__FUNCTION__);
	}
}

HUINT32 local_cas_iruc_STREAM_FilterIndexByAllocRefID(HUINT32 ulAllocRefID)
{
	HUINT32	i = 0,j = 0;
	IRUC_STREAM_FILTER_INFO *pFilter=NULL;
//	HxLOG_Print("[local_cas_iruc_STREAM_FilterIndexByAllocRefID] \n");

	for( i = 0; i < IRUC_STREAM_MAX_TOTAL_CHANNEL; i++ )
	{
		for( j = 0; j < IRUC_STREAM_MAX_FILTER_PER_CHANNEL; j++ )
		{
			pFilter = &(s_iruc_stream_info[i].astFilter[j]);
			if(pFilter->AllocRefID == ulAllocRefID)
			{
//				HxLOG_Print("[local_cas_iruc_STREAM_FilterIndexByAllocRefID] pFilter->RequestID(0x%X) AllocRefID(%d), filterIndex(%d), \n", pFilter->RequestID, ulAllocRefID, j);
				return j;
			}
		}
	}
	HxLOG_Print("[%s]  ERROR : no more available reference ID\n",__FUNCTION__);

	return UC_INVALID_HANDLE_VALUE;
}


uc_result local_cas_iruc_STREAM_Close_One_Filter( HUINT32 StreamIdx, HUINT32 FilterIdx)
{
	IRUC_STREAM_FILTER_INFO *pFilter;
    HERROR		err = ERR_FAIL;

	pFilter = &s_iruc_stream_info[StreamIdx].astFilter[FilterIdx];
	HxLOG_Print("[%s] addr = 0x%X \n",__FUNCTION__, &s_iruc_stream_info[StreamIdx]);
/* comment : 여기서 모든 filter에 해당 하는 RefID 전부 클리어 하는 것은 문제가 된다.
             해당 steamhandle에 등록된 RefId 인지 확인이 필요 하다.
             이전에 해당 streamhandle에서 사용이 된 filter 인지 확인 한다.*/
	if(pFilter->Allocated == TRUE)
	{
		HxLOG_Print("[%s] StreamIdx = 0x%X, FilterIdx = 0x%X\n",__FUNCTION__, StreamIdx ,  FilterIdx);
//clear all filters' items.
	err = svc_cas_SubSvcDmxStopOneSection(pFilter->RequestID);
	if(err != ERR_OK)
	{
		HxLOG_Print("[%s]  ++++++> Fail Filter Stop, RequestID(0x%X)  <++++++ \n",__FUNCTION__, pFilter->RequestID );
	}

	//clear filter ref id.
	if(pFilter->AllocRefID != IRUC_INVALID_FILTER_REFID)
	{
		local_cas_iruc_STREAM_Release_FilterRefID(pFilter->AllocRefID);
	}
		VK_MEM_Memset( pFilter, 0x00, sizeof(IRUC_STREAM_FILTER_INFO) );
		pFilter->Allocated = FALSE;
		pFilter->Started = FALSE;
	//	pFilter->HaveToStart = FALSE;
		pFilter->RequestID = IRUC_STREAM_INVALID_REQUESET_ID;
		pFilter->AllocRefID= IRUC_INVALID_FILTER_REFID;
	}
	else
	{
		/* comment : 해당 streamhandle 사용된 filter 가 아니다.*/
		return UC_ERROR_UNABLE_TO_CREATE;
	}
	return UC_ERROR_SUCCESS;
}

uc_result local_cas_iruc_STREAM_init_One_Filter( HUINT32 StreamIdx, HUINT32 FilterIdx)
{
	IRUC_STREAM_FILTER_INFO *pFilter=NULL;

	pFilter = &s_iruc_stream_info[StreamIdx].astFilter[FilterIdx];

	HxLOG_Print("[%s] Addr=0x%X StreamIdx = 0x%X, FilterIdx = 0x%X\n",__FUNCTION__, &s_iruc_stream_info[StreamIdx], StreamIdx ,  FilterIdx);

	VK_MEM_Memset( pFilter, 0x00, sizeof(IRUC_STREAM_FILTER_INFO) );
	pFilter->Allocated = FALSE;
	pFilter->Started   = FALSE;
	pFilter->HaveToStart = FALSE;
	pFilter->RequestID   = IRUC_STREAM_INVALID_REQUESET_ID;
	pFilter->AllocRefID  = IRUC_INVALID_FILTER_REFID;

	return UC_ERROR_SUCCESS;
}

void local_cas_iruc_STREAM_Close_One_Stream(HINT32 idx)
{
	HINT32 i = 0;

	HxLOG_Print("[%s] addr=0x%X \n",__FUNCTION__ ,&s_iruc_stream_info[idx]);

/*
UniversalClientSPI_Stream_CloseFilter 에서 호출을 해서 여기서 다시 filter를 close 할 필요 없다.
	for(i=0; i< IRUC_STREAM_MAX_FILTER_PER_CHANNEL; i++)
	{
		local_cas_iruc_STREAM_Close_One_Filter(idx, i);
	}
*/
	s_iruc_stream_info[idx].bUsed = FALSE;
	s_iruc_stream_info[idx].SvcContext = IRUC_INVALID_UC_SVC_CONTEXT;
	s_iruc_stream_info[idx].ucConnStreamType = UC_CONNECTION_STREAM_TYPE_FTA;
	s_iruc_stream_info[idx].usPid = IRUC_INVALID_PID;


	//filter information.
//	VK_MEM_Memset(s_iruc_stream_info[idx].astFilter, 0x00, sizeof(IRUC_STREAM_FILTER_INFO) );

	//component
	for(i=0; i< IRUC_STREAM_MAX_COMPONENT; i++)
	{
		VK_MEM_Memset(&s_iruc_stream_info[idx].astComponent[i], 0x00, sizeof(IRUC_STREAM_COMPONENT) );

		s_iruc_stream_info[idx].astComponent[i].bAllocated = FALSE;
		s_iruc_stream_info[idx].astComponent[i].bOtherAllocated = FALSE;
		s_iruc_stream_info[idx].astComponent[i].usOtherPid = IRUC_INVALID_PID;
		s_iruc_stream_info[idx].astComponent[i].bVirualComponent= FALSE;
		s_iruc_stream_info[idx].astComponent[i].ulPalDscIdx = IRUC_STREAM_INVALID_PALDSC_ID;

		s_iruc_stream_info[idx].astComponent[i].stUC_ES_COMPONENT.base.size=0;
		s_iruc_stream_info[idx].astComponent[i].stUC_ES_COMPONENT.componentStream.protocolType=0 ;//DVB type
		s_iruc_stream_info[idx].astComponent[i].stUC_ES_COMPONENT.componentStream.pid = IRUC_INVALID_PID;
	}
}



static HUINT32	local_cas_iruc_STREAM_GetMatchedFilterIdxByRequestID(HUINT32 RequestID, HUINT32 *pChannelIdx, HUINT32 *pFilterdx)
{
	HUINT32	i = 0, j = 0 ;

//	HxLOG_Print("[local_cas_iruc_STREAM_GetMatchedFilterIdxByRequestID] RequestID = 0x%X", RequestID );

	for( i = 0; i < IRUC_STREAM_MAX_TOTAL_CHANNEL; i++ )
	{
		for( j = 0; j < IRUC_STREAM_MAX_FILTER_PER_CHANNEL; j++ )
		{
//			HxLOG_Print("channel[%d].filter[%d]= 0x%x \n", i, j, s_iruc_stream_info[i].astFilter[j].RequestID);
			if( (s_iruc_stream_info[i].astFilter[j].RequestID == RequestID) &&
				(IRUC_STREAM_INVALID_REQUESET_ID != RequestID)  )
			{
				*pChannelIdx = i;
				*pFilterdx = j;
//				HxLOG_Print("[%s]Found RequestID = 0x%X, in channel idx = %d, filter idx = 0x%X\n",__FUNCTION__, RequestID , *pChannelIdx , *pFilterdx) ;
				return ERR_OK;
			}
		}
	}

	HxLOG_Print("[local_cas_iruc_STREAM_GetMatchedFilterIdxByRequestID] Cannot find RequestID = 0x%X\n", RequestID );

	return ERR_FAIL;
}

void local_cas_iruc_STREAM_CallBack_NotifySection(HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, Handle_t hAction)
{
	PalSef_RawData_t *pSiRawData=NULL;
	HUINT16	usTableId, RequestId, usTsUniqId, usFilterResult;
	HUINT16	nSecLength;
	HUINT32 	ChanIdx, FilterIdx;
	uc_bytes 	Section_Bytes;
	IRUC_STREAM_FILTER_INFO *pFilter=NULL;
	HUINT8  SecByte1,SecByte2;
	HUINT32	ulResult = ERR_FAIL;

	STREAM_NOTI_SEM_GET;
	// Get TableId, FilterId, UniqueId, Filter Result
	//	local_cassi_GetTableIdAndResult(ulTableIdAndResult, &usTableId, &usFilterResult);
	//	local_cassi_GetFilterAndTsUniqueId(ulFilterAndUniqId, &RequestId, &usTsUniqId);
	local_cas_iruc_STREAM_CB_GetTableIdAndResult(ulTableIdAndResult, &usTableId, &usFilterResult);
	local_cas_iruc_STREAM_CB_GetFilterAndTsUniqueId(ulFilterAndUniqId, &RequestId, &usTsUniqId);


#if 0
//	if(usTableId == 0x82)
	{
		HxLOG_Print("[EMM CallBack] Receieve EMM Notified.hAct(0x%X) TbId(0x%X), RequestID(0x%X), UniqueId(0x%X)\n ", hAction, usTableId, RequestId, usTsUniqId);
	}
#endif
	if(hAction == HANDLE_NULL)
	{
		HxLOG_Print("[%s] Input param is invalid \r\n",__FUNCTION__);
		STREAM_NOTI_SEM_RELEASE;
		return;
	}

	if(RequestId == 0xFFFF)
	{
		HxLOG_Print("[%s] Input param is invalid \r\n",__FUNCTION__);
		STREAM_NOTI_SEM_RELEASE;
		return;
	}

	if(usFilterResult != eSiFilterResult_Complete)
	{
		HxLOG_Print("[%s] result = %s\n",__FUNCTION__,(usFilterResult == eSiFilterResult_Timeout) ? "Time out" : "Error");
		STREAM_NOTI_SEM_RELEASE;
		return;
	}

	pSiRawData = (PalSef_RawData_t*)ulParam;
	if(pSiRawData == NULL)
	{
		HxLOG_Print("[%s] Null section  \r\n",__FUNCTION__);
		STREAM_NOTI_SEM_RELEASE;
		return;
	}

// hjlee3, test temp
#if 0
//[82][40][F1][C3][69][DE][C7]
	if(pSiRawData->buf[0]==0x82 && pSiRawData->buf[1]==0x40 &&
		pSiRawData->buf[2]==0xF1 && pSiRawData->buf[3]==0xC3 &&
		pSiRawData->buf[4]==0x69 && pSiRawData->buf[5]==0xDE && pSiRawData->buf[6]==0xC7)
	{
		HxLOG_Print("\n don't pass [82][40][F1][C3][69][DE][C7] to UCCA \n");
		STREAM_NOTI_SEM_RELEASE;
		return;
	}
#endif

	nSecLength = get12bit( (pSiRawData->buf[0]) +1) + 3;

#if 0
// hjlee3, test temp
	VK_Print("\n received secttion header: "));
	for (i=0 ; i < 16  ; i++)
	{
		VK_Print(" [%02X]", *(pSiRawData->buf[i]));
		if(i > 6) break;
	}
	VK_Print(" \n"));
#endif

	ulResult= local_cas_iruc_STREAM_GetMatchedFilterIdxByRequestID(RequestId, &ChanIdx, &FilterIdx);
	if(ERR_OK != ulResult)
	{
		HxLOG_Print("[%s] Cannot find Chan, Filter info with req ID = 0x%X\n",__FUNCTION__, RequestId);
		STREAM_NOTI_SEM_RELEASE;
		return ;
	}


	pFilter = &(s_iruc_stream_info[ChanIdx].astFilter[FilterIdx] );
#if 0
	VK_Print("\n section filter: ");
	for (i=0 ; i < 16  ; i++)
	{
		VK_Print("[%02X]", pFilter->aucFilterValue[i]);
	}
	VK_Print(" \n");
#endif

//	HxLOG_Print("Addr=0x%X \n", &s_iruc_stream_info[ChanIdx]);

	if( s_iruc_stream_info[ChanIdx].astConnection.bAllocated ==TRUE)
	{
//		HxLOG_Print("pFilter->AllocRefID= 0x%X\n", pFilter->AllocRefID );

		SecByte1 = pFilter->aucFilterValue[1] &(pFilter->aucFilterMask[1]) ;

#if 1 // comparion of section length.
		if(pFilter->wFilterLength == 2)
		{
			if( 	SecByte1	!= ( *( pSiRawData->buf[0] +1) & pFilter->aucFilterMask[1]  ) )
			{
					HxLOG_Print("[%s] Sec lengh is 2 byte \n",__FUNCTION__);
					STREAM_NOTI_SEM_RELEASE;
				return ;
		        }
		}
		else
		{
			SecByte2 = pFilter->aucFilterValue[2] &(pFilter->aucFilterMask[2]) ;

			if( 	(SecByte1	!= ( *( pSiRawData->buf[0] +1) & pFilter->aucFilterMask[1]  ) ) ||
				(SecByte2 	!= ( *( pSiRawData->buf[0] +2) & pFilter->aucFilterMask[2]  ) )    )
			{
				HxLOG_Print("[%s] Sec lengh not matched\n ",__FUNCTION__);
                                STREAM_NOTI_SEM_RELEASE;
				return ;
			}
		}
#endif
		Section_Bytes.bytes = pSiRawData->buf[0];
		Section_Bytes.length = nSecLength;

		if(s_iruc_stream_info[ChanIdx].astConnection.notifyCallback != NULL)
		{
#ifdef CHECK_ECM_SECTION_NUM
			#if defined(IRUC_STREAM_SECTION_DEBUG)
// hjlee3, test temp
//			if(Section_Bytes.bytes[0]==0x82)
			if(((DVB_ECM_FILTER_SECTIOM_TAG(pSiRawData->buf[0]) == ECM_SECTION_TAG_1 || DVB_ECM_FILTER_SECTIOM_TAG(pSiRawData->buf[0]) == ECM_SECTION_TAG_2))
				&& (DVB_ECM_FILTER_SECTION_PAGE(pSiRawData->buf[0]) > (DVB_ECM_FILTER_SECTION_LAST_PAGE(pSiRawData->buf[0]))))
			{
				int x;
				HxLOG_Print("[%s] Section notify, %d\n",__FUNCTION__,  s_iruc_stream_info[ChanIdx].ucConnStreamType );
				for(x=0; x<nSecLength; x++)
				{
					HxLOG_Print("[%02X]", Section_Bytes.bytes[x]);
					if(x%16 ==15)
					{
						HxLOG_Print("\n");
					}
				}
				HxLOG_Print("\n");
			}
			#endif	// End of defined(IRUC_STREAM_SECTION_DEBUG)
			if(((DVB_ECM_FILTER_SECTIOM_TAG(pSiRawData->buf[0]) == ECM_SECTION_TAG_1 || DVB_ECM_FILTER_SECTIOM_TAG(pSiRawData->buf[0]) == ECM_SECTION_TAG_2))
				&& (DVB_ECM_FILTER_SECTION_PAGE(pSiRawData->buf[0]) > (DVB_ECM_FILTER_SECTION_LAST_PAGE(pSiRawData->buf[0]))))
			{
				HxLOG_Print("[%s] ERROR : Invalid page No. cur[0x%02X] last[0x%02X]\n",__FUNCTION__,
					DVB_ECM_FILTER_SECTION_PAGE(pSiRawData->buf[0]), DVB_ECM_FILTER_SECTION_LAST_PAGE(pSiRawData->buf[0]));
				STREAM_NOTI_SEM_RELEASE;
	return;
			}
			#endif
			s_iruc_stream_info[ChanIdx].astConnection.notifyCallback( s_iruc_stream_info[ChanIdx].astConnection.connectionHandle,&Section_Bytes );
//			HxLOG_Print("[%s] Callback fn. OK, (%d)\n",__FUNCTION__,s_iruc_stream_info[ChanIdx].ucConnStreamType);

		}
		else
		{
			HxLOG_Print("[%s] Callback fn. is NULL !(Chan = 0x%d, connection=%d,%d\n",__FUNCTION__, \
				ChanIdx, FilterIdx,  s_iruc_stream_info[ChanIdx].ucConnStreamType) ;
		}
	}
	else
	{
		HxLOG_Print("[%s]Sec noti. Connection is not established\n",__FUNCTION__ );
	}

	// TODO: free memory ????
	STREAM_NOTI_SEM_RELEASE;
	return;
}

void CAS_IRUC_SPI_STREAM_Init(void)
{
	HINT32 i, ulRet,j;

	HxLOG_Print("\n" );

	VK_MEM_Memset( &s_iruc_stream_info, 0, sizeof(s_iruc_stream_info) );

	for(i=0; i< IRUC_STREAM_MAX_TOTAL_CHANNEL; i++)
	{

// stream init
		local_cas_iruc_STREAM_Close_One_Stream(i);

// filter init
		for(j=0;j<IRUC_STREAM_MAX_FILTER_PER_CHANNEL;j++)
		{
			local_cas_iruc_STREAM_init_One_Filter( i, j);
		}
	}

	for(i=0; i< IRUC_STREAM_MAX_TOTAL_FILTER; i++)
	{
		local_cas_iruc_STREAM_Release_FilterRefID(i);
	}

#if defined( __NOTUSE_MUTEX__)
	ulRet = VK_SEM_Create(&s_ulUcStreamSem, "s_ulUcStreamSem", VK_SUSPENDTYPE_FIFO);
	if( ulRet != VK_OK )
	{
		HxLOG_Print(" Semaphore Create Error\n");
		HxLOG_Critical("\n[%s:%d]\n",__FUNCTION__,__LINE__);
	}
#else
	ulRet = VK_MUTEX_Create(&s_ulUcStreamSem, "s_ulUcStreamSem");
	if( ulRet != VK_OK )
	{
		HxLOG_Print(" Semaphore Create Error\n");
		HxLOG_Critical("\n[%s:%d]\n",__FUNCTION__,__LINE__);
	}
#endif

	ulRet = VK_SEM_Create(&s_ulUcStreamNotiSem, "s_ulUcStreamNotiSem", VK_SUSPENDTYPE_FIFO);
	if( ulRet != VK_OK )
	{
		HxLOG_Print("Semaphore Create Error\n");
		HxLOG_Critical("\n[%s:%d]\n",__FUNCTION__,__LINE__);
	}

}

#define ______SPI_STREAM_CONTROL_______

void UniversalClientSPI_Stream_Init(void)
{
	HxLOG_Print("[%s]\n",__FUNCTION__  );
}

/**
 * Stream method for opening a new instance of a DVB stream.
 *
 * This method is called to create a new instance of a DVB stream.
 * This is intended to process the input data in \a pStreamOpenParams
 * and set up any implementation-specific resources and allocate a stream handle,
 * which is then passed to the other stream interface methods.
 *
 * When this method is called, the stream implementation should process it
 * and return ::UC_ERROR_SUCCESS. Any other result will be propagated through the
 * Universal Client API.
 *
 * \note This method is always called from the context of the thread that called
 * the Universal Client API.
 *
 * @param[in] isEcm Boolean value indicating type of stream. If this is
 *     ::UC_TRUE, the stream being opened is an ECM stream. Otherwise,
 *     it is an EMM stream. The difference is that ECM streams have additional
 *     methods for handling descrambling.
 *     The following stream methods will only be called if \a isEcm is ::UC_TRUE:
 *     - ::UniversalClientSPI_Stream_AddComponent
 *     - ::UniversalClientSPI_Stream_RemoveComponent
 *     - ::UniversalClientSPI_Stream_SetDescramblingKey
 * @param[in] pStreamOpenParams Pointer to structure that describes
 *      the parameters needed to open the stream.
 *
 * @param[out] pStreamHandle Receives an implementation-specific handle to the stream.
 *      This stream handle will be passed to other stream interface methods
 *      in order to identify the instance of the stream, and finally in a call to
 *      ::UniversalClientSPI_Stream_Close in order to free the handle.
 *      NULL is not a valid stream handle. If the function returns success, it must not
 *      set *pStreamHandle to NULL.
 *
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".\

 */

uc_result UniversalClientSPI_Stream_Open(
    uc_connection_stream_type streamType,
    const uc_stream_open_params *pStreamOpenParams,
    uc_stream_handle *pStreamHandle)
{
	HINT32 i;

	uc_stream_open_params *pStreamopenparams = pStreamOpenParams;

	HxLOG_Print("\n\n\t[%s] ===== Open [%s][%d] =======\n",__FUNCTION__ , local_cas_iruc_Dbg_PrintStreamType(streamType), streamType);
    STREAM_SEM_GET;

	for(i=0; i< IRUC_STREAM_MAX_TOTAL_CHANNEL; i++)
	{
		if(s_iruc_stream_info[i].bUsed == FALSE ) {
			break;
	}
	}

	if(i == IRUC_STREAM_MAX_TOTAL_CHANNEL)
	{
		HxLOG_Print("[%s] insufficient buffer \n",__FUNCTION__ );
		STREAM_SEM_RELEASE;
		return UC_ERROR_INSUFFICIENT_BUFFER;
	}

	//pass index of demux indexing.
	*pStreamHandle =  i ;

	HxLOG_Print("[%s] UC open pServiceContext = 0x%X, StreamHandle=%d \n",__FUNCTION__ ,(HUINT32) pStreamopenparams->pServiceContext, *pStreamHandle);
	s_iruc_stream_info[i].SvcContext = (HUINT32 ) (pStreamopenparams->pServiceContext) ;
	s_iruc_stream_info[i].ucConnStreamType  = streamType;
	s_iruc_stream_info[i].usPid = pStreamopenparams->caStream.pid;
	HxLOG_Print("[%s] s_iruc_stream_info[%d] pServiceContext = 0x%X:0x%X  address:0x%X \n",__FUNCTION__,i,(HUINT32)pStreamopenparams->pServiceContext, s_iruc_stream_info[i].SvcContext,&(s_iruc_stream_info[i]));
#if 0
	//open dscrambler, hereby
	if(ERR_OK != IRUC_DRV_API_Get_ActHandDmxId_BySvcContext(s_iruc_stream_info[i].SvcContext,&hAct, &ulDmxID) )
	{
		HxLOG_Print("Cannot get Dmx ID\n" );
		return UC_ERROR_INVALID_HANDLE;
	}

	if(ERR_OK !=	 svc_cas_DevDscOpen(ulDmxID,  ePAL_DSC_DSCTYPE_DVB, ePAL_DSC_DATATYPE_PES, &(s_iruc_stream_info[i].astComponent.ulPalDscIdx) ) )
	{
		HxLOG_Print("Cannot open descrambler\n" );
		return UC_ERROR_INVALID_HANDLE;
	}
#endif

	// allocate stream handle with index.
	s_iruc_stream_info[i].bUsed = TRUE;

	HxLOG_Print("[%s] Open OK! streamHandle= %d, Stream type= %d, PID = 0x%X, \n",__FUNCTION__,  \
		i, s_iruc_stream_info[i].ucConnStreamType, s_iruc_stream_info[i].usPid );

//	HxLOG_Print("%d Stream opened. line(%d)\n", s_iruc_stream_info[i].ucConnStreamType, __LINE__ );

	STREAM_SEM_RELEASE;
	return UC_ERROR_SUCCESS;
}

/**
 * Stream method to stop and cleanup a stream instance
 *
 * This method is called when the stream is being closed by UniversalClient_Stream_Close and is
 * intended to give the stream implementation a chance to clean up any leftover resources set
 * up during a call to open.
 *
 * @param[in,out] pStreamHandle On input, points to a stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 *      On output, the function must set this to #UC_INVALID_HANDLE_VALUE.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Stream_Close(    uc_stream_handle *pStreamHandle)
{

	STREAM_SEM_GET;
/*
 TODO : hjlee3
startfiler -> close -> setfilter 로 API가 호출되면
Started , HaveToStart 가 초기화 되어서 setfilter 에서 filter를 걸수 가 없다.
그래서 무조건 필터를 거는 방식으로 변경한다.
descramble 구간에 들어 갈 시에 감박 이는 현상이 잇는지 확인 필요 하다.
*/

#if 0
	HUINT16 i=0;
/*  closefilter 재사용에 대비해서 HaveToSart 는 stream close 시에   init 한다. */
	for(i=0; i< IRUC_STREAM_MAX_FILTER_PER_CHANNEL; i++)
	{
		s_iruc_stream_info[*pStreamHandle].astFilter[i].HaveToStart = FALSE;
	}
#endif

	local_cas_iruc_STREAM_Close_One_Stream(*pStreamHandle);


	STREAM_SEM_RELEASE;
	return UC_ERROR_SUCCESS;
}

/** @defgroup streamspi Universal Client Stream SPIs
 * Universal Client Stream SPIs
 *
 * These SPIs implement the functionality of a 'stream'. A stream is an object that the client
 * can connect to in order to set up filters and receive CA messages, like ECMs and EMMs, and receive
 * keys that can be used to descramble content.
 *
 * Streams are created by the client when a method that requires a stream instance is
 * called.
 *
 * A stream implementation is responsible for the following:
 *
 * - Managing lifetime of an instance of a stream, via ::UniversalClientSPI_Stream_Open and ::UniversalClientSPI_Stream_Close methods.
 * - Connecting to the CA processing aspect of the client via ::UniversalClientSPI_Stream_Connect and ::UniversalClientSPI_Stream_Disconnect.
 * - Handling filters via the ::UniversalClientSPI_Stream_OpenFilter and ::UniversalClientSPI_Stream_CloseFilter methods.
 * - Providing sections data via the ::uc_notify_callback method.
 * - Forwarding control words received via ::UniversalClientSPI_Stream_SetDescramblingKey to the corresponding descrambler(s).
 *
 * \note It is the integrator's responsibility to implement these methods for a particular platform.
 *
 * For more information on how streams are used, see \ref stream_interface_call_sequence "Stream Interface Call Sequence".
 *  @{
 */

/**
 * Starts all filters for this stream.
 *
 * This function is called to start all filters that were allocated and set for this stream
 * using ::UniversalClientSPI_Stream_OpenFilter and ::UniversalClientSPI_Stream_SetFilter.
 *
 * Once this method is called, a stream implementation is free to call the ::uc_notify_callback
 * function originally passed to ::UniversalClientSPI_Stream_Connect for this stream instance.
 *
 *
 * @param[in] streamHandle Stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
*/
uc_result UniversalClientSPI_Stream_Start(uc_stream_handle streamHandle)
{
	HINT32 i, result;
	IRUC_STREAM_FILTER_INFO *pSelectedFilter;
	SiRetrieveParam_t SiFilterParam;
	HUINT16 RequestID;
	Handle_t hAct;
	HUINT32 ulDemuxId;

	HUINT8 aucPalFilterValue[IRUC_STREAM_FILTER_MAX_LEN]= {0};
	HUINT8 aucPalFilterMask[IRUC_STREAM_FILTER_MAX_LEN]= {0};
	HINT32 PalFilterLength = 0, lResult = ERR_FAIL;

	HxLOG_Print("[%s] SvcContext0x=0x%X, streamHandle = %d\n",__FUNCTION__ , s_iruc_stream_info[streamHandle].SvcContext,streamHandle );
	STREAM_SEM_GET;


	/* For all filters on the channel */
	for( i = 0; i < IRUC_STREAM_MAX_FILTER_PER_CHANNEL; i++ )
	{
		pSelectedFilter = &(s_iruc_stream_info[streamHandle].astFilter[i]) ;

// because openfilter don't begins , setfiler be called at next time , steam_start skip
// just set HaveToStart=TRUE
		if(pSelectedFilter->Allocated == FALSE)
		{
			pSelectedFilter->HaveToStart = TRUE;
		//	HxLOG_Print("[%s] RequestID = 0x%x, HavetoSatrt(%d), steamhandle(%d),filterIdx(%d) \n",__FUNCTION__ , pSelectedFilter->RequestID, pSelectedFilter->HaveToStart, streamHandle, i);
			continue;
		}

/* TODO : hjlee3
startfiler -> close -> setfilter 로 API가 호출되면
Started , HaveToStart 가 초기화 되어서 setfilter 에서 filter를 걸수 가 없다.
그래서 무조건 필터를 거는 방식으로 변경한다.
descramble 구간에 들어 갈 시에 감박 이는 현상이 잇는지 확인 필요 하다.
          A/V are changed async mode 라는 프린트 문이 나온다. 연관 잇어 보인다. */

// stream_setfiler aleady begins so skip
		if(pSelectedFilter->Started == TRUE)
		{
			if(ERR_OK !=  svc_cas_SubSvcDmxStopOneSection(pSelectedFilter->RequestID) )
			{
				HxLOG_Print("[%s] Error !!! svc_cas_SubSvcDmxStopOneSection  (RequestID=0x%X)\n",__FUNCTION__ , pSelectedFilter->RequestID);
			}
			else
			{
				HxLOG_Print("[%s] This Stop Section for restart filter  octoFilterId:0x%X, UC_FilterId:0x%X \n", __FUNCTION__, pSelectedFilter->RequestID, pSelectedFilter->AllocRefID);
			}

//			continue;
		}

		if(ERR_OK != CAS_IRUC_GetActHandDmxIDbySvcContext( s_iruc_stream_info[streamHandle].SvcContext, &hAct , &ulDemuxId) )
		{
			HxLOG_Print("[%s] Cannot find Act handle\n",__FUNCTION__ );
			STREAM_SEM_RELEASE;
			return UC_ERROR_OUT_OF_MEMORY;
		}

		lResult = local_cas_iruc_STREAM_Make_OctoFilter(hAct, s_iruc_stream_info[streamHandle].usPid, &SiFilterParam);
		if(ERR_OK != lResult)
		{
			HxLOG_Print("[%s] Cannot make UC filter with octo resource\n",__FUNCTION__ );
			STREAM_SEM_RELEASE;
			return UC_ERROR_OUT_OF_MEMORY;
		}

		/* converting filter with octo policy */
		local_cas_iruc_STREAM_CONVERT_FILTER(pSelectedFilter, &PalFilterLength, aucPalFilterValue, aucPalFilterMask);

		result = svc_cas_SubSvcDmxRetrieveSectionBase((svcCas_SiRetrieveParam_t*)&SiFilterParam,
														aucPalFilterValue[0], //pSelectedFilter->aucFilterValue[0],
														aucPalFilterValue, //pSelectedFilter->aucFilterValue,
														aucPalFilterMask, //pSelectedFilter->aucFilterMask,
														PalFilterLength, //pSelectedFilter->wFilterLength,
														0,
														(void*)local_cas_iruc_STREAM_CallBack_NotifySection,
														NULL,
														&RequestID);
		if (result != ERR_OK)
		{
			HxLOG_Print("[%s] svc_cas_SubSvcDmxRetrieveSectionBase return ERROR (%x)\n",__FUNCTION__ , result);
			STREAM_SEM_RELEASE;
			return UC_ERROR_OUT_OF_MEMORY;
		}

		pSelectedFilter->Started = TRUE;
		pSelectedFilter->RequestID = RequestID;
		HxLOG_Print("[%s] Filter Set OK SetFilter => (RequestID= 0x%X), AllocRefID(%d), streamhandle(%d),Pid(0x%X),filteridx(%d)\n",__FUNCTION__ , RequestID,pSelectedFilter->AllocRefID,streamHandle,s_iruc_stream_info[streamHandle].usPid,i);
	}

	HxLOG_Print("[%s] Type(%d) Stream started. line(%d)\n",__FUNCTION__ , s_iruc_stream_info[streamHandle].ucConnStreamType, __LINE__ );
	STREAM_SEM_RELEASE;

	return UC_ERROR_SUCCESS;
}

/**
 * Stops all filters for this stream.
 *
 * This function is called to stop all filters that were allocated and set for this stream
 * using ::UniversalClientSPI_Stream_OpenFilter and ::UniversalClientSPI_Stream_SetFilter.
 *
 * Once this method is called, then no more data should be passed to ::uc_notify_callback function
 * originally passed to ::UniversalClientSPI_Stream_Connect for this stream instance.
 *
 *
 * @param[in] streamHandle Stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Stream_Stop(uc_stream_handle streamHandle)
{
	HINT32 i, result;
	IRUC_STREAM_FILTER_INFO * pSelectedFilter=NULL;

	HxLOG_Print("[%s]\n",__FUNCTION__ );
        STREAM_SEM_GET;

/* Comment : stream 이 stop 하는데 해당 streamHandle의 UniversalClientSPI_Stream_RemoveComponent 가
            호출 안되어서 여기서 해당 streamhandle 할당된 descrambler 를 해제 한다. */
	local_cas_iruc_STREAM_RemoveComponent(streamHandle);


	/* For all filters on the channel */
	for( i = 0; i < IRUC_STREAM_MAX_FILTER_PER_CHANNEL; i++ )
	{
		pSelectedFilter = &(s_iruc_stream_info[streamHandle].astFilter[i]) ;

		if( pSelectedFilter->Started == TRUE )
		{
			result = svc_cas_SubSvcDmxStopOneSection(pSelectedFilter->RequestID);
			if (result != ERR_OK)
			{
				HxLOG_Print("[%s] ERROR : svc_cas_SubSvcDmxStopOneSection : %08X\n",__FUNCTION__ , result);
			}

			pSelectedFilter->Started = FALSE;
			//pSelectedFilter->RequestID = IRUC_STREAM_INVALID_REQUESET_ID;
			HxLOG_Print("[%s] stopped s_iruc_stream_info[%d].SvcContext=0x%X, filter idx = 0x%X, RequestID = 0x%02X \n",__FUNCTION__, streamHandle, s_iruc_stream_info[streamHandle].SvcContext ,i, pSelectedFilter->RequestID);
		}
		else
		{
		//	HxLOG_Print("[%s] Already stopped, with fileter index = 0x%X\n",__FUNCTION__ , i);
		}

	}

	STREAM_SEM_RELEASE;
	return UC_ERROR_SUCCESS;
}

/**
 * Stream method for opening a filter.
 *
 * A stream is controlled by the adding
 * and removal of 'filters'. Each filter describes
 * what kinds of sections are expected to be received from the stream. The stream implementation
 * is only required to provide sections that match one of the currently active filters.
 *
 * This method is called to add a filter to an existing stream. Before this callback is
 * called, no data should be provided via the notification function.
 *
 * After an EMM stream is opened and connected, this method is called several times to set up different filters.
 * Each EMM streamm will need 7 filters.
 *
 * For an ECM stream, only a single filter is used.
 *
 * After this method is called, ::UniversalClientSPI_Stream_SetFilter is called to provide or change the filter
 * rules on the filter handle. After all filters are initially set up, ::UniversalClientSPI_Stream_Start is called.
 *
 * #UC_INVALID_HANDLE_VALUE is not a valid filter handle. If the function returns success, it must not
 * set *pFilterHandle to #UC_INVALID_HANDLE_VALUE.
 *
 *
 * @param[in] streamHandle Stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 * @param[out] pFilterHandle Receives an implementation defined filter handle that can be used to
 *      identify the filter in a subsequent call to ::UniversalClientSPI_Stream_CloseFilter.
 *
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Stream_OpenFilter(
    uc_stream_handle streamHandle,
    uc_filter_handle *pFilterHandle)
{
	HINT32 i;
	HUINT32 RefFilterId;

	HxLOG_Print("[%s] == start == addr=0x%X,streamHandle = 0x%X \n",__FUNCTION__,&s_iruc_stream_info[streamHandle], streamHandle);
    STREAM_SEM_GET;

	if(s_iruc_stream_info[streamHandle].bUsed != TRUE)
	{
		HxLOG_Print("[%s] Cannot find the Stream Handle. !\n",__FUNCTION__  );
        STREAM_SEM_RELEASE;
		return UC_ERROR_OUT_OF_MEMORY;
	}

	for(i=0; i< IRUC_STREAM_MAX_FILTER_PER_CHANNEL; i++ )
	{
		if (s_iruc_stream_info[streamHandle].astFilter[i].Allocated !=TRUE )
		{
			break;
		}
	}

	if(i == IRUC_STREAM_MAX_FILTER_PER_CHANNEL)
	{
		HxLOG_Print("[%s] No filter vacancy in dmx info\n",__FUNCTION__ );
                STREAM_SEM_RELEASE;
		return UC_ERROR_TOO_MANY_HANDLES;
	}

	 RefFilterId = local_cas_iruc_STREAM_GetVacancy_FilterRefID();
	HxLOG_Print("[%s] RefFilterId:%d \n",__FUNCTION__,RefFilterId);
	if(RefFilterId == IRUC_STREAM_MAX_TOTAL_FILTER)
	{
		HxLOG_Print("[%s] Cannot get RefFilter ID\n",__FUNCTION__ );
        STREAM_SEM_RELEASE;
		return UC_ERROR_TOO_MANY_HANDLES;
	}

	s_iruc_stream_info[streamHandle].astFilter[i].AllocRefID = RefFilterId;
	s_iruc_stream_info[streamHandle].astFilter[i].Allocated = TRUE;

	*pFilterHandle = s_iruc_stream_info[streamHandle].astFilter[i].AllocRefID;
	HxLOG_Print("[%s] addr=0x%X,  streamHandle(0x%X), type(%d) Filter open Ok, with AllocRefID(0x%X)\n",__FUNCTION__, \
					&s_iruc_stream_info[streamHandle],streamHandle, s_iruc_stream_info[streamHandle].ucConnStreamType, *pFilterHandle) ;

        STREAM_SEM_RELEASE;

	return UC_ERROR_SUCCESS;
}

/**
 * Stream method for setting filter information.
 *
 * This method is called after ::UniversalClientSPI_Stream_OpenFilter for each filter.  This method is also
 * called when a message is received that requires changing of filter rules.
 *
 *  - For EMM streams, filter rules change when a new group unique address is assigned to the client.
 *    In a typical Irdeto CA system, a group unique address can be reassigned as necessary to
 *    minimize bandwidth.
 *  - For ECM streams, filter rules change when a new ECM arrives with a different table ID than the
 *    previous ECM. In a typical Irdeto CA system, old ECMs are played out until the descrambling key
 *    cycles. After that, the new ECM is delivered with an alternate table IDs, and repeated with that
 *    table ID until the next key cycle. This filter is used to prevent the resubmission of duplicate
 *    ECMs to the client.
 *
 * @param[in] filterHandle Filter handle previously allocated by ::UniversalClientSPI_Stream_OpenFilter.
 * @param[in] pFilterRules The rule set of the filter to create.  If this is NULL, the implementation
 *      should return all data that arrives, regardless of pattern. I.e., NULL means data is unfiltered.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Stream_SetFilter(
    uc_filter_handle filterHandle,
    const uc_filter *pFilterRules)
{
	HUINT32 result, ulDmxID;
	HUINT16 RequestID=0;
	HINT32 i, j, bFound, lResult = ERR_FAIL;
	SiRetrieveParam_t	stFilterParam;
	Handle_t	hAct;
	IRUC_STREAM_FILTER_INFO	*pSelectedFilter;

	HUINT8 aucPalFilterValue[IRUC_STREAM_FILTER_MAX_LEN]= {0};
	HUINT8 aucPalFilterMask[IRUC_STREAM_FILTER_MAX_LEN]= {0};
	HINT32 PalFilterLength;

	HxLOG_Print("[%s] AllocRefID = %d\n",__FUNCTION__ , filterHandle );

	STREAM_SEM_GET;

#ifdef STREAM_DBG_FILTERFUMP
	if(pFilterRules ==NULL)
	{
		HxLOG_Print("[%s]Filter rule is NULL: %d\n",__FUNCTION__ ,__LINE__ );
	}
	else
	{
		HxLOG_Print("[%s] pFilterRules->filterDepth = %d\n",__FUNCTION__ , pFilterRules->filterDepth);
		for(i=0; i< pFilterRules->filterDepth; i++)
		{
			HxLOG_Print("[%02X]", pFilterRules->match[i]);
		}
		HxLOG_Print("\n");
		for(i=0; i< pFilterRules->filterDepth; i++)
		{
			HxLOG_Print("[%02X]", pFilterRules->mask[i]);
		}
		HxLOG_Print("\n");
	}
#endif

	bFound = FALSE;
	for(i=0; i<IRUC_STREAM_MAX_TOTAL_CHANNEL; i++)
	{
		for(j=0;j<IRUC_STREAM_MAX_FILTER_PER_CHANNEL; j++)
		{
			if( (s_iruc_stream_info[i].astFilter[j].AllocRefID == filterHandle) && (s_iruc_stream_info[i].astFilter[j].Allocated == TRUE) )
			{
				HxLOG_Print("[%s] Found allocated RefID(%d), in stream handle (%d) and filter idx (%d),\n",__FUNCTION__ , filterHandle, i, j );
				bFound = TRUE;
				break;
			}
		}

		if(bFound )	break;
	}

	if( TRUE != bFound)
	{
		HxLOG_Print("[%s] Cannot find the requesetd filter handle = 0x%X\n",__FUNCTION__ , filterHandle);
		STREAM_SEM_RELEASE;
		return UC_ERROR_TYPE_FILTER;
	}

	pSelectedFilter = &(s_iruc_stream_info[i].astFilter[j]) ;

	HxLOG_Print("using addr(0x%X) \n",&s_iruc_stream_info[i] ) ;
/*    filter set 이면 stop 하고 무조건 다시 건다. */
 	if( pSelectedFilter->Started == TRUE )
	{
		//HxLOG_Print("이미 스타트 가 된 시점에서 셋필터가 들어온 경우\n");
		// if the filter already was started, we stop it here.
		HxLOG_Print("[%s]Stop Filter for Restart Filter of Req ID (0x%x), Started(%d),(%d:%d) \n",__FUNCTION__ , pSelectedFilter->RequestID,pSelectedFilter->Started,i,j);

		// if the filter already was started, we stop it here.
		if(ERR_OK !=  svc_cas_SubSvcDmxStopOneSection(pSelectedFilter->RequestID) )
		{
			HxLOG_Print("[%s] Error !!! svc_cas_SubSvcDmxStopOneSection  (RequestID=0x%X)\n",__FUNCTION__ , pSelectedFilter->RequestID);
		}
		else
		{
			HxLOG_Print("[%s] This Stop Section for restart filter  \n", __FUNCTION__);
		}
	}
	else
	{
		// 스타트 아직 안된 시점에서 셋필터가 들어온 경우, StreamStart 가 setFilter 보다 먼저 call 될 경우
		HxLOG_Print("[%s] before set filter, stream start is aleady going   ,  set filter , i =%d, j= %d\n",__FUNCTION__ , i , j);
	}

	 // If this is NULL, the implementation  should return all data that arrives, regardless of pattern. I.e., NULL means data is unfiltered.
 	if(pFilterRules == NULL)
	{
		HxLOG_Print("[%s] pFilterRules is NULL !!!\n",__FUNCTION__ );

		VK_MEM_Memset(pSelectedFilter->aucFilterValue,  0x00, IRUC_STREAM_FILTER_MAX_LEN );
		VK_MEM_Memset(pSelectedFilter->aucFilterMask,  0x00, IRUC_STREAM_FILTER_MAX_LEN );
		pSelectedFilter->wFilterLength = 0;
	}
	else
	{
		HxSTD_memcpy(pSelectedFilter->aucFilterValue,  pFilterRules->match, pFilterRules->filterDepth );
		HxSTD_memcpy(pSelectedFilter->aucFilterMask,  pFilterRules->mask, pFilterRules->filterDepth );
		pSelectedFilter->wFilterLength = pFilterRules->filterDepth;
	}

	//if upper filter's start condition is true, we must start it again.
	// becasue stream_start aleady begins ,  here set filter
/*
 TODO : hjlee3
startfiler -> close -> setfilter 로 API가 호출되면
Started , HaveToStart 가 초기화 되어서 setfilter 에서 filter를 걸수 가 없다.
그래서 무조건 필터를 거는 방식으로 변경한다.
descramble 구간에 들어 갈 시에 감박 이는 현상이 잇는지 확인 필요 하다.
//	if( (pSelectedFilter->Started == TRUE) || (pSelectedFilter->HaveToStart == TRUE))
*/
	{
		HxLOG_Print("[%s] first filter start in SetFilter \n",__FUNCTION__);

		if(ERR_OK != CAS_IRUC_GetActHandDmxIDbySvcContext( s_iruc_stream_info[i].SvcContext, &hAct, &ulDmxID ) )
		{
			HxLOG_Print("[%s] Cannot find Act handle\n", __FUNCTION__ );
			STREAM_SEM_RELEASE;
			return UC_ERROR_OUT_OF_MEMORY;
		}

		lResult = local_cas_iruc_STREAM_Make_OctoFilter(hAct,s_iruc_stream_info[i].usPid,&stFilterParam);
		if(ERR_OK != lResult)
		{
			HxLOG_Print("[%s] Cannot make a filter\n", __FUNCTION__ );
			STREAM_SEM_RELEASE;
			return UC_ERROR_INVALID_HANDLE;
		}

		/* converting filter with octo policy */
		local_cas_iruc_STREAM_CONVERT_FILTER(pSelectedFilter, &PalFilterLength, aucPalFilterValue, aucPalFilterMask);

		result = svc_cas_SubSvcDmxRetrieveSectionBase((svcCas_SiRetrieveParam_t*)&stFilterParam,
														aucPalFilterValue[0], //pSelectedFilter->aucFilterValue[0],
														aucPalFilterValue, //pSelectedFilter->aucFilterValue,
														aucPalFilterMask, //pSelectedFilter->aucFilterMask,
														PalFilterLength, //pSelectedFilter->wFilterLength,
														0,
														(void*)local_cas_iruc_STREAM_CallBack_NotifySection,
														NULL,
														&RequestID);

		if (result != ERR_OK)
		{
			HxLOG_Print("[%s] svc_cas_SubSvcDmxRetrieveSectionBase return ERROR (%x)\n",__FUNCTION__ , result);
			STREAM_SEM_RELEASE;

			return UC_ERROR_OUT_OF_MEMORY;
		}

		pSelectedFilter->RequestID = RequestID;
		pSelectedFilter->Started = TRUE;
		HxLOG_Print("[%s] Filter Set OK => (RequestID= 0x%X),AllocRefID(%d), streamhandle(%d),Pid(0x%X),filteridx(%d)\n",__FUNCTION__ , RequestID,pSelectedFilter->AllocRefID,i,s_iruc_stream_info[i].usPid,j);

	}
	HxLOG_Print("[%s] streamType:%d Filter Set. completed .\n",__FUNCTION__ , s_iruc_stream_info[i].ucConnStreamType);

	STREAM_SEM_RELEASE;

	return UC_ERROR_SUCCESS;
}


/**
 * Stream method for closing a filter
 *
 * This function is called to close an existing filter from a stream that was previously
 * added by a call to ::UniversalClientSPI_Stream_OpenFilter.
 *
 *
 * @param[in] streamHandle Stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 * @param[in,out] pFilterHandle On input, points to a filter handle previously allocated by
 *      ::UniversalClientSPI_Stream_OpenFilter.  Any private data
 *      associated with filterHandle should be cleaned up by this method. When the method returns,
 *      it must set *pFilterHandle to #UC_INVALID_HANDLE_VALUE to indicate it has freed the handle.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_NOT_IMPLEMENTED
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Stream_CloseFilter(
    uc_stream_handle streamHandle,
    uc_filter_handle *pFilterHandle)
{
	IRUC_STREAM_FILTER_INFO *pFilter=NULL;
	HUINT32 ulFiterIndex = UC_INVALID_HANDLE_VALUE;

	HxLOG_Print("[%s] SvcContext=0x%X streamHandle = 0x%X, UC_FilterHandle = 0x%X\n",__FUNCTION__ , s_iruc_stream_info[streamHandle].SvcContext, streamHandle , (HUINT32) *pFilterHandle);
        STREAM_SEM_GET;

	if(s_iruc_stream_info[streamHandle].bUsed != TRUE)
	{
                STREAM_SEM_RELEASE;
		HxLOG_Print("[%s] Cannot find the Stream Handle(= 0x%X) !\n",__FUNCTION__ , streamHandle );
		return UC_ERROR_OUT_OF_MEMORY;
	}
/*
// hjlee , test temp
	return UC_ERROR_SUCCESS; */


	ulFiterIndex=local_cas_iruc_STREAM_FilterIndexByAllocRefID(*pFilterHandle);
	if (ulFiterIndex != UC_INVALID_HANDLE_VALUE) {
	pFilter = &(s_iruc_stream_info[streamHandle].astFilter[ulFiterIndex]);

	if(pFilter->Allocated == TRUE )
	{
		local_cas_iruc_STREAM_Close_One_Filter(streamHandle, ulFiterIndex);
		*pFilterHandle = UC_INVALID_HANDLE_VALUE;

	}
	else
	{
		HxLOG_Print("[%s] Already the filter handle (0x%X) is not allocated. \n",__FUNCTION__ , *pFilterHandle );
		*pFilterHandle = UC_INVALID_HANDLE_VALUE;
		//The return value has still successful value.
	}
	}
	else
	{

		*pFilterHandle = UC_INVALID_HANDLE_VALUE;
		HxLOG_Print("[%s] Error!! Not found filter index filter handle (0x%X) \n",__FUNCTION__ , *pFilterHandle );
	}

	//clear filter ref id.
//	local_cas_iruc_STREAM_Release_FilterRefID(*pFilterHandle);

	STREAM_SEM_RELEASE;
	return UC_ERROR_SUCCESS;
}

/**
 * Method to connect to a stream.
 *
 * After a stream is opened with a call to ::UniversalClientSPI_Stream_Open, internally the client will call
 * this stream interface method in order to complete the connection setup.
 *
 * Part of the connection process involves informing the stream of a notification callback and connection handle
 * that are used to deliver new ECM and EMM sections to the client.
 *
 * After connecting to a stream, the client may set up filters by calling ::UniversalClientSPI_Stream_OpenFilter.
 * After this is done, the stream may begin sending sections to the notification callback, specifying the
 * connection handle passed into this call to \a Connect.
 *
 *
 * @param[in] streamHandle Stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 * @param[in] connectionHandle Connection handle to associate with this stream. This must also be
 *     passed to the \a notifyCallback function when sections arrive.
 * @param[in] notifyCallback Function to be called when sections arrive.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Stream_Connect(
    uc_stream_handle streamHandle,
    uc_connection_handle connectionHandle,
    uc_notify_callback notifyCallback)
{
	HxLOG_Print("[%s] addr=0x%X, s_iruc_stream_info[%d].SvcContext=0x%X , connectionHandle = 0x%X\n",__FUNCTION__, &s_iruc_stream_info[streamHandle] ,streamHandle, s_iruc_stream_info[streamHandle].SvcContext, connectionHandle );

    STREAM_SEM_GET;

	if(s_iruc_stream_info[streamHandle].bUsed != TRUE)
	{
		HxLOG_Print("[%s] Cannot find the Stream Handle. !\n",__FUNCTION__  );
                STREAM_SEM_RELEASE;
		return UC_ERROR_OUT_OF_MEMORY;
	}


	if(s_iruc_stream_info[streamHandle].astConnection.bAllocated == TRUE)
	{
		HxLOG_Print("[%s] connection already allocated.!!!\n",__FUNCTION__  );
                STREAM_SEM_RELEASE;
		return UC_ERROR_ALREADY_STARTED;
	}

	s_iruc_stream_info[streamHandle].astConnection.connectionHandle = connectionHandle;
	s_iruc_stream_info[streamHandle].astConnection.notifyCallback = notifyCallback;
	s_iruc_stream_info[streamHandle].astConnection.bAllocated = TRUE;

	HxLOG_Print("[%s] address:0x%X, streamtype:%d,  Stream connected . streamHandle= 0x%X, connectionHandle = 0x%X\n",__FUNCTION__,\
						&s_iruc_stream_info[streamHandle],s_iruc_stream_info[streamHandle].ucConnStreamType,
						streamHandle, connectionHandle);

	STREAM_SEM_RELEASE;
	return UC_ERROR_SUCCESS;
}

/**
 * Stream method to disconnect a stream.
 *
 * This method is provided to give the stream an opportunity to clean up resources related to a connection previously
 * made by a call to ::UniversalClientSPI_Stream_Connect.
 *
 * A stream that has been disconnected should not continue to call ::uc_notify_callback
 * after this call returns.
 *
 * @param[in] streamHandle Stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 * @param[in] connectionHandle Connection handle originally passed to ::UniversalClientSPI_Stream_Connect.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Stream_Disconnect(
    uc_stream_handle streamHandle,
    uc_connection_handle connectionHandle)
{
	HxLOG_Print("[%s] addr=0x%X, streamHandle= 0x%X, connectionHandle = 0x%X\n",__FUNCTION__,&s_iruc_stream_info[streamHandle], streamHandle, connectionHandle );
        STREAM_SEM_GET;

	if(s_iruc_stream_info[streamHandle].bUsed != TRUE)
	{
		HxLOG_Print("[%s] Cannot find the Stream Handle. !\n",__FUNCTION__  );
        STREAM_SEM_RELEASE;
		return UC_ERROR_OUT_OF_MEMORY;
	}

	if( s_iruc_stream_info[streamHandle].astConnection.bAllocated == TRUE)
	{
		// followings are only for debuggin.
		if (s_iruc_stream_info[streamHandle].astConnection.connectionHandle == connectionHandle )
		{
			HxLOG_Print("[%s] Found matched connection handle!\n",__FUNCTION__  );
		}
		else
		{
			HxLOG_Print("[%s] Cannot find the matched Connection handle. !\n",__FUNCTION__  );
		}

		s_iruc_stream_info[streamHandle].astConnection.bAllocated = FALSE;
		s_iruc_stream_info[streamHandle].astConnection.connectionHandle = (uc_connection_handle) NULL;
		s_iruc_stream_info[streamHandle].astConnection.notifyCallback = NULL;
	}
	else
	{
		HxLOG_Print("[%s] the Connection is not allocated!\n",__FUNCTION__  );
	}

	STREAM_SEM_RELEASE;
	return UC_ERROR_SUCCESS;
}

#define _____SPI_STREAM_COMPONENTS_______
/**
 * Stream method to add a descrambling component to the stream.
 *
 * This method is used to add a component to a stream.
 * A 'component' is the information
 * associated with an audio or video stream.
 *
 * The implementation of this method should manage a list of active components,
 * such that when ::UniversalClientSPI_Stream_SetDescramblingKey is called, it can forward the
 * key to all PIDs that require it using the descrambler API.
 *
 * @param[in] streamHandle Stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 * @param[in] pComponent Component structure that identifies the elementary stream for this component.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
// #define __VIRTUALCOMPONENT__
uc_result UniversalClientSPI_Stream_AddComponent(
	uc_stream_handle streamHandle,
	const uc_elementary_component *pComponent)
{
	IRUC_STREAM_INFO *pStream=NULL ;
	HUINT32 otherCnt=1, ulDmxID, ucCompIdx=0;
	Handle_t hAct;
	HUINT8	ComponentCnt =0;
	HUINT32 i=0;
	HUINT32 ulDscIndex[IRUC_SVC_MAX_PID]={0,} ;
	HUINT16 usOtherPidList[IRUC_SVC_MAX_PID]={0,} ;
	HERROR	hError = ERR_FAIL;

	HxLOG_Print("\n [%s] ===> start with addr=0x%X, streamHandle(=0x%X), Trying ADD component pid = 0x%X\n", __FUNCTION__, \
		&s_iruc_stream_info[streamHandle], streamHandle,  pComponent->componentStream.pid );

	STREAM_SEM_GET;

	for(i=0; i<IRUC_STREAM_MAX_COMPONENT; i++)
	{
		if(TRUE != s_iruc_stream_info[streamHandle].astComponent[i].bAllocated)
		{
			/* search empty slot for allocating stream*/
			break;
		}
	}

	if(i == IRUC_STREAM_MAX_COMPONENT)
	{
		HxLOG_Print("[%s] no vacancy.\n",__FUNCTION__);
		STREAM_SEM_RELEASE;
		return UC_ERROR_ALREADY_STARTED;
	}

	pStream = &s_iruc_stream_info[streamHandle];

	HxLOG_Print("[%s] streamhandle %d, Addr 0x%X:0x%X, SvcContext 0x%X:0x%X \n", __FUNCTION__,streamHandle, &s_iruc_stream_info[streamHandle], pStream, s_iruc_stream_info[streamHandle].SvcContext, pStream->SvcContext);

	//open dscrambler, hereby
	if(ERR_OK != CAS_IRUC_GetActHandDmxIDbySvcContext(pStream->SvcContext,&hAct, &ulDmxID))
	{
		HxLOG_Print("[%s] Cannot get Dmx ID\n",__FUNCTION__);
		STREAM_SEM_RELEASE;
		return UC_ERROR_INVALID_HANDLE;
	}

	HxLOG_Print("[%s] hAct=0x%X, ulDmxID=%d \n", __FUNCTION__, hAct, ulDmxID);

	if(CAS_IRUC_CheckAllocateDscOtherPid(pStream->SvcContext, pComponent->componentStream.pid))
	{

/* Comment : 동일한 pid가 다른 streamhandle로 늦게 addComponent 될경우에 대한 처리.*/

		for(ucCompIdx=0 ; ucCompIdx < IRUC_STREAM_MAX_COMPONENT ; ucCompIdx++)
		{

/* Comment : StreamHandle이 이전 Humax SetPid할 시에 streamhandle과 같을 경우 */
			if(pComponent->componentStream.pid == pStream->astComponent[ucCompIdx].usOtherPid)
			{
				pStream->astComponent[ucCompIdx].bAllocated = TRUE;
				pStream->astComponent[ucCompIdx].bVirualComponent = FALSE;
				pStream->astComponent[ucCompIdx].bOtherAllocated = FALSE;
				pStream->astComponent[ucCompIdx].usOtherPid = IRUC_INVALID_PID;

				pStream->astComponent[ucCompIdx].stUC_ES_COMPONENT.base.size = pComponent->base.size;
				pStream->astComponent[ucCompIdx].stUC_ES_COMPONENT.componentStream.pid = pComponent->componentStream.pid;
				pStream->astComponent[ucCompIdx].stUC_ES_COMPONENT.componentStream.protocolType = pComponent->componentStream.protocolType; // DVB type

				HxLOG_Print("[%s] Humax SetPid Keep (CompIdx=%d), (PID=0x%X), (DscIdx=0x%X) \n", __FUNCTION__,ucCompIdx, pComponent->componentStream.pid,pStream->astComponent[ucCompIdx].ulPalDscIdx);

				STREAM_SEM_RELEASE;
				return UC_ERROR_SUCCESS;
			}
		}


/* Comment : StreamHandle이 이전 Humax SetPid할 시에 streamhandle과 다를 경우 */


/* Comment: 이전 other pid open 에 의해서 descrabmle 이 이미 open 된 pid인지를 체크 해서  맞는다면 stUC_ES_COMPONENT에 *pComponent 연결.
			pComponent 를 이용 해서 memcpy를 하면 다른 memory 지역을 access 한다.
			아무래도 pComponent가 사이즈가 넒게 잡혀서 침범 하는 것으로 보이는 데 흠. 좀 이상 하다.
			Irdeto 에게 문의가 필요해 보인다.
			stUC_ES_COMPONENT의 각각의 변수에 저장 한다.  */
		pStream->astComponent[i].stUC_ES_COMPONENT.base.size = pComponent->base.size;
		pStream->astComponent[i].stUC_ES_COMPONENT.componentStream.pid = pComponent->componentStream.pid;
		pStream->astComponent[i].stUC_ES_COMPONENT.componentStream.protocolType = pComponent->componentStream.protocolType; // DVB type

		pStream->astComponent[i].bAllocated = TRUE;
		pStream->astComponent[i].bVirualComponent = FALSE;
		pStream->astComponent[i].bOtherAllocated = FALSE;
		pStream->astComponent[i].usOtherPid = IRUC_INVALID_PID;

        hError = local_cas_iruc_STREAM_ReInitComponent(streamHandle, i, pComponent->componentStream.pid);
		if(ERR_OK !=  hError)
		{
			HxLOG_Print("[%s] Error! Local remove descrambler \n", __FUNCTION__ );
		}


		HxLOG_Print("[%s] Create New Stream from Humax comp Pid=0x%X,DscIdx = 0x%X  \n", __FUNCTION__,pComponent->componentStream.pid,pStream->astComponent[i].ulPalDscIdx );
	}
	else
	{

/* comment: 채널 전환시 아래 경우로 인해서 next cw가 나오기 전가지 audio가 안나오는 문제가 잇다.
		ex)
		add video pid component
		even cw : open video pid descrambler
		add audio pid componet
		odd cw	: open video pid descrambler and audio pid descrambler */
		if(TRUE != CAS_IRUC_CheckDescramblePid(pStream->SvcContext, pComponent->componentStream.pid ))
		{

	/* comment: humax mw 에서 setpid에 의해서 element PID 가 안들어온 상황
				그러나 UC에서 add component 한 상황이다.
			우선 이런 상황에서는 descrambler를 open 하지 않는 것이 맞아 보이고 virualcomponent로 남긴다. */
			pStream->astComponent[i].stUC_ES_COMPONENT.base.size = pComponent->base.size;
			pStream->astComponent[i].stUC_ES_COMPONENT.componentStream.pid = pComponent->componentStream.pid;
			pStream->astComponent[i].stUC_ES_COMPONENT.componentStream.protocolType = pComponent->componentStream.protocolType; // DVB type

			pStream->astComponent[i].bAllocated = TRUE;
			pStream->astComponent[i].bVirualComponent = TRUE;
			pStream->astComponent[i].bOtherAllocated = FALSE;
			pStream->astComponent[i].usOtherPid = IRUC_INVALID_PID;

/* Comment :MW 에서 second audio pid에 대해서 처리를 하지 않아서
			second audio pid가 생길 경우 여기로 들어오게 된다.
			그래서 MW에서 second audio를 받아야 하고 여기로 들어오지 안는지 확인 해야 한다.
			descrambler를 open할 필요가 없다.
			단순히 해당 pid로 ecm filter만 걸면 된다.*/

			HxLOG_Print("[%s] Virtual ok. pid = 0x%X\n", __FUNCTION__, pComponent->componentStream.pid );
		}
		else
		{
			if(ERR_OK !=	 svc_cas_DevDscOpen(ulDmxID,  ePAL_DSC_DSCTYPE_DVB, ePAL_DSC_DATATYPE_PES, &(pStream->astComponent[i].ulPalDscIdx)))
			{
				HxLOG_Print("[%s] Cannot open descrambler\n",__FUNCTION__ );
			}
			if(ERR_OK !=	 svc_cas_DevDscSetPid(pStream->astComponent[i].ulPalDscIdx, pComponent->componentStream.pid))
			{
				HxLOG_Print("[%s] Cannot set pid with descrambler\n",__FUNCTION__ );
			}

			pStream->astComponent[i].stUC_ES_COMPONENT.base.size = pComponent->base.size;
			pStream->astComponent[i].stUC_ES_COMPONENT.componentStream.pid = pComponent->componentStream.pid;
			pStream->astComponent[i].stUC_ES_COMPONENT.componentStream.protocolType = pComponent->componentStream.protocolType; // DVB type

			pStream->astComponent[i].bAllocated = TRUE;
			pStream->astComponent[i].bVirualComponent = FALSE;
			pStream->astComponent[i].bOtherAllocated = FALSE;
			pStream->astComponent[i].usOtherPid = IRUC_INVALID_PID;

			HxLOG_Print("[%s] Descrambler Setting of Component PID by UCCA is OK, streamhandle %d,  Pid[0x%04x]. \n", __FUNCTION__ ,streamHandle,pComponent->componentStream.pid);
		}

	/* Comment: Other PID set :: exception function
		   AddComponent는 처음 추가 될 시에 even odd 각 각 에 매칭 되어서 한번식 호출 된다.
		   그래서 humax  side 에서 나머지 pid를 걸어 줘야 한다.
		   현재 서비스에서 open이 안된 pid에 대해서 descrambler open 한다.
				AddComponent 로 불려지지 않는 나머지 pid에 대해서 component 를 관리 한다.
	   Comment : 이미 set 된 humax pid에 대해서 피해가게 처리 해야 한다.    */
		if (CAS_IRUC_SetOtherDescramblePid(pStream->SvcContext, pComponent->componentStream.pid, &ComponentCnt, ulDscIndex, usOtherPidList))
		{
			for(otherCnt=0 ; (otherCnt < ComponentCnt) && (otherCnt < IRUC_STREAM_MAX_COMPONENT) ; otherCnt++)
			{
					if((i+1+otherCnt) < IRUC_STREAM_MAX_COMPONENT)
				{
						pStream->astComponent[i+1+otherCnt].stUC_ES_COMPONENT.base.size = 0 ;
						pStream->astComponent[i+1+otherCnt].stUC_ES_COMPONENT.componentStream.pid = usOtherPidList[otherCnt] ; // IRUC_INVALID_PID
						pStream->astComponent[i+1+otherCnt].stUC_ES_COMPONENT.componentStream.protocolType=0; // DVB type

						pStream->astComponent[i+1+otherCnt].bAllocated			= TRUE;
						pStream->astComponent[i+1+otherCnt].bOtherAllocated 	= TRUE;
						pStream->astComponent[i+1+otherCnt].bVirualComponent	= FALSE;
						pStream->astComponent[i+1+otherCnt].ulPalDscIdx 		= ulDscIndex[otherCnt] ;
						pStream->astComponent[i+1+otherCnt].usOtherPid			= usOtherPidList[otherCnt];
						HxLOG_Print("[%s] Other DSC PID Open OK , component addr(0x%08X), Idx(%d) ,PID (0x%04X), DscIndex(0x%X) , \n", __FUNCTION__,
																&pStream->astComponent[i+1+otherCnt],
																i+1+otherCnt , pStream->astComponent[i+1+otherCnt].usOtherPid,
																pStream->astComponent[i+1+otherCnt].ulPalDscIdx);
				}
			}
		}
	}

	STREAM_SEM_RELEASE;
	return UC_ERROR_SUCCESS;
}

/**
 * Stream method to remove a descrambling component from the stream.
 *
 * This method is used to remove a component from a stream, given a component definition.
 * A 'component' is the information
 * associated with an audio or video stream.
 *
 * The implementation of this method should search for a component with a matching definition
 * in the list of active components, and remove it if it is found. A component 'matches' if it
 * has the same PID as an active component.
 *
 * @param[in] streamHandle Stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 * @param[in] pComponent Component structure that identifies the elementary stream for this component.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *    ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Stream_RemoveComponent(
	uc_stream_handle streamHandle,
	const uc_elementary_component *pComponent)
{
	int i;
	IRUC_STREAM_INFO *pStream=NULL;
	HUINT8 aucNULL_KEY[8] ={0,};
	HUINT16 usPid;
	HERROR result;

        STREAM_SEM_GET;
	HxLOG_Print("\n[%s] With streamHandle(=0x%X), Trying REMOVE component pid = 0x%X\n",__FUNCTION__, \
		streamHandle,  pComponent->componentStream.pid  );

	pStream = &s_iruc_stream_info[streamHandle];

	HxLOG_Print("[%s] streamhandle %d, Addr 0x%X:0x%X, SvcContext 0x%X:0x%X \n",__FUNCTION__,streamHandle, &s_iruc_stream_info[streamHandle], pStream, s_iruc_stream_info[streamHandle].SvcContext, pStream->SvcContext);
	for(i=0; i< IRUC_STREAM_MAX_COMPONENT; i++)
	{
		if( ((pStream->astComponent[i].stUC_ES_COMPONENT.componentStream.pid == pComponent->componentStream.pid ) ||\
			(pStream->astComponent[i].usOtherPid == pComponent->componentStream.pid )) &&\
			((pStream->astComponent[i].bAllocated) || (pStream->astComponent[i].bOtherAllocated)) &&\
			(pStream->astComponent[i].bVirualComponent != TRUE) )

		{
			result = svc_cas_DevDscSetKey(pStream->astComponent[i].ulPalDscIdx, ePAL_DSC_KEYPARITY_EVEN, aucNULL_KEY );
			result = svc_cas_DevDscSetKey(pStream->astComponent[i].ulPalDscIdx, ePAL_DSC_KEYPARITY_ODD, aucNULL_KEY );
			if(ERR_OK != result)
			{
				HxLOG_Print("[%s] PAL_DSC_SetKey => Error stream (=0x%X), comp index(=0x%X) Error\n",__FUNCTION__,streamHandle, i);
			}

			usPid = pComponent->componentStream.pid;

			/* hjlee3 : other PID 대한 제거 필요 할 시에 해당 pid로 맞춰 준다. */
			if(pStream->astComponent[i].bOtherAllocated) {
				usPid = pStream->astComponent[i].usOtherPid;
			}

			result =svc_cas_DevDscResetPid(pStream->astComponent[i].ulPalDscIdx,usPid);
			if(ERR_OK != result)
			{
				HxLOG_Print("[%s] PAL_DSC_ResetPid => Error component index(=%d) pid(0x%X)\n",__FUNCTION__,i , usPid);
			}

/* comment : IRUC_INVALID_PID가 cw set 안되게 하기 위해서 component 초기화  */
			pStream->astComponent[i].bAllocated = FALSE;
			pStream->astComponent[i].bOtherAllocated = FALSE;
			pStream->astComponent[i].bVirualComponent = FALSE;
			pStream->astComponent[i].usOtherPid = IRUC_INVALID_PID;

			pStream->astComponent[i].stUC_ES_COMPONENT.base.size = 0;
			pStream->astComponent[i].stUC_ES_COMPONENT.componentStream.pid = IRUC_INVALID_PID;
			pStream->astComponent[i].stUC_ES_COMPONENT.componentStream.protocolType = 0; // DVB type


/* comment : set pid에 의해서 들어온 pid로 descrambler 가 open 된경우에 descrambler flag 해제 해 줘야 한다. */
			if(CAS_IRUC_ResetOtherDescramblePid(pStream->SvcContext, usPid) )
			{
				HxLOG_Print("[%s] Set PID of Humax is Reset \n",__FUNCTION__ );
			}
			else
			{
				HxLOG_Print("[%s] Set PID by Humax have Failed Reset \n",__FUNCTION__ );
			}

/* Comment : 각 Pid당  open DSC를 close 한다*/
			if(svc_cas_DevDscClose(pStream->astComponent[i].ulPalDscIdx) )
			{
				HxLOG_Print("[%s] DSC Close Fail streamHandle(%d),DscIdx(0x%X),Pid(0x%04X) \n",__FUNCTION__,streamHandle, pStream->astComponent[i].ulPalDscIdx, usPid);
			}
			else
			{
				HxLOG_Print("[%s] DSC Close OK streamHandle(%d),DscIdx(0x%X),Pid(0x%04X) \n",__FUNCTION__,streamHandle, pStream->astComponent[i].ulPalDscIdx, usPid);
			}

/* Comment : 해당 DSC close 한후에 초기화 한다. */
			pStream->astComponent[i].ulPalDscIdx = IRUC_STREAM_INVALID_PALDSC_ID;

			break;
		}

	}

	STREAM_SEM_RELEASE;
	return UC_ERROR_SUCCESS;
}

void  local_cas_iruc_STREAM_RemoveComponent(	uc_stream_handle streamHandle)
	{
	int i;
	IRUC_STREAM_INFO *pStream=NULL;
	HUINT8 aucNULL_KEY[8] ={0,};
	HUINT16 usPid;
	HERROR result;

	pStream = &s_iruc_stream_info[streamHandle];

	HxLOG_Print("\n[%s] streamHandle(=0x%X), SvcContext = 0x%X \n",__FUNCTION__,streamHandle, pStream->SvcContext);
	for(i=0; i< IRUC_STREAM_MAX_COMPONENT; i++)
	{
		if( pStream->astComponent[i].ulPalDscIdx != IRUC_STREAM_INVALID_PALDSC_ID )
		{
			result = svc_cas_DevDscSetKey(pStream->astComponent[i].ulPalDscIdx, ePAL_DSC_KEYPARITY_EVEN, aucNULL_KEY );
			result = svc_cas_DevDscSetKey(pStream->astComponent[i].ulPalDscIdx, ePAL_DSC_KEYPARITY_ODD, aucNULL_KEY );
			if(ERR_OK != result)
			{
				HxLOG_Print("[%s] PAL_DSC_SetKey => Error stream (=0x%X), comp index(=0x%X) Error\n",__FUNCTION__,streamHandle, i);
			}

			usPid = pStream->astComponent[i].stUC_ES_COMPONENT.componentStream.pid;

			/* hjlee3 : other PID 대한 제거 필요 할 시에 해당 pid로 맞춰 준다. */
			if(pStream->astComponent[i].bOtherAllocated) {
				usPid = pStream->astComponent[i].usOtherPid;
			}

			result =svc_cas_DevDscResetPid(pStream->astComponent[i].ulPalDscIdx,usPid);
			if(ERR_OK != result)
			{
				HxLOG_Print("[%s] PAL_DSC_ResetPid => Error component index(=%d) pid(0x%X)\n",__FUNCTION__,i , usPid);
			}

/* comment : IRUC_INVALID_PID가 cw set 안되게 하기 위해서 component 초기화  */
		pStream->astComponent[i].bAllocated 	  = FALSE;
		pStream->astComponent[i].bOtherAllocated  = FALSE;
			pStream->astComponent[i].bVirualComponent = FALSE;
		pStream->astComponent[i].usOtherPid 	  = IRUC_INVALID_PID;

		pStream->astComponent[i].stUC_ES_COMPONENT.base.size=0;
			pStream->astComponent[i].stUC_ES_COMPONENT.componentStream.pid = IRUC_INVALID_PID;
		pStream->astComponent[i].stUC_ES_COMPONENT.componentStream.protocolType=0 ;//DVB type


/* comment : set pid에 의해서 들어온 pid로 descrambler가 open 된경우에 descrambler flag 해제 해 줘야 한다. */
			if(CAS_IRUC_ResetOtherDescramblePid(pStream->SvcContext, usPid) )
			{
				HxLOG_Print("[%s] Set PID of Humax is Reset \n",__FUNCTION__ );
			}
			else
			{
				HxLOG_Print("[%s] Set PID by Humax have Failed Reset \n",__FUNCTION__ );
			}

/* Comment : 각 Pid당  open DSC를 close 한다*/
			if(svc_cas_DevDscClose(pStream->astComponent[i].ulPalDscIdx) )
			{
				HxLOG_Print("[%s] DSC Close Fail streamHandle(%d),DscIdx(0x%X),Pid(0x%04X) \n",__FUNCTION__,streamHandle, pStream->astComponent[i].ulPalDscIdx, usPid);
			}
			else
			{
				HxLOG_Print("[%s] DSC Close OK streamHandle(%d),DscIdx(0x%X),Pid(0x%04X) \n",__FUNCTION__,streamHandle, pStream->astComponent[i].ulPalDscIdx, usPid);
			}

/* Comment : 해당 DSC close 한후에 초기화 한다. */
			pStream->astComponent[i].ulPalDscIdx = IRUC_STREAM_INVALID_PALDSC_ID;

			break;
		}

	}

	return UC_ERROR_SUCCESS;
}

HERROR local_cas_iruc_STREAM_ReInitComponent(uc_stream_handle streamHandle, HUINT16 usComponentIdx, HUINT16 usPid)
{
	int j, i;
	IRUC_STREAM_INFO *pStream=NULL,*pOutStream=NULL;
	HERROR result;
	HUINT8 aucNULL_KEY[8] ={0};
	HxLOG_Print("[%s] Componet Pidof Humax remove , Componet Pid= 0x%X \n",__FUNCTION__, usPid  );
    pOutStream = &s_iruc_stream_info[streamHandle] ;

/* 이함수의 목적은 addComponent call시에 streamhandle이
   이미 other에서 저장된 pid의 streamhandle이 달라서 other PID reset 및 paldscidx 를 복사기위한 용도 이다.*/

	for(i=0; i< IRUC_STREAM_MAX_TOTAL_CHANNEL; i++)
	{
		pStream = &s_iruc_stream_info[i] ;
		for(j=0; j< IRUC_STREAM_MAX_COMPONENT; j++)
		{
			if( (pStream->astComponent[j].usOtherPid == usPid ) &&\
				(pStream->astComponent[j].bOtherAllocated) &&\
				(pStream->astComponent[j].bVirualComponent != TRUE) )
			{

/* otherPid에 해당 하는 ulPalDscIdx 값 저장 한다.. */
				pOutStream->astComponent[usComponentIdx].ulPalDscIdx = pStream->astComponent[j].ulPalDscIdx;

/* otherPid에 해당 하는 값들 초기화 한다. */
				pStream->astComponent[j].bAllocated = FALSE;
				pStream->astComponent[j].bVirualComponent = FALSE;
				pStream->astComponent[j].bOtherAllocated = FALSE;
				pStream->astComponent[j].usOtherPid = IRUC_INVALID_PID;
				pStream->astComponent[j].stUC_ES_COMPONENT.base.size = 0;
				pStream->astComponent[j].stUC_ES_COMPONENT.componentStream.pid = IRUC_INVALID_PID;
				pStream->astComponent[j].stUC_ES_COMPONENT.componentStream.protocolType = 0; // DVB type


				HxLOG_Print("[%s] Humax Set PID Remove !!!! streamHandle=%d, component idx=%d  ,component  pid = 0x%X, , copied DscIdx(0x%X) \n",__FUNCTION__, i, j, usPid, pOutStream->astComponent[usComponentIdx].ulPalDscIdx);

				return ERR_OK;
			}

		}
	}

	return ERR_FAIL;
}
/**
 * Stream method to pass a key to the descrambler
 *
 * For ECM streams only: this method is provided to send descrambling keys extracted from ECMs on
 * this stream to the descrambler for each active component previously added by ::UniversalClientSPI_Stream_AddComponent.
 *
 * For EMM streams, this method will not be called.
 *
 * @param[in] streamHandle Stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 * @param[in] pKeyInfo Information about the key being used to descramble content. This contains key algorithm, protection,
 *     cryptography context, and the key material.
 * @param[in] keyVersion Key version that this descrambling key is associated with.
 *     Different key versions refer to potentially different keys. The descrambler
 *     should maintain separate descrambling keys for each key version. The key version
 *     typically cycles between 1 and N, where N is the number of different key
 *     versions supported by the scrambler. This value is used to
 *     allow for smooth transition from one key to the next when the descrambling key
 *     changes. At a minimum, implementations should be able to support at least 2
 *     key identifiers, 0 and 1.
 *     \note For DVB, 0 indicates Even Key and 1 indicates Odd Key.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Stream_SetDescramblingKey(
    uc_stream_handle streamHandle,
    const uc_key_info *pKeyInfo,
    uc_uint32 keyVersion)
{
	HINT32 i=0, result = ERR_OK;
	IRUC_STREAM_COMPONENT *pComp=NULL;
#if defined(CONFIG_MW_CAS_IRDETO_SECURE)
	HUINT8	*tmpCSSK = NULL;
#endif

#if defined(PRINT_CW) || defined(IRUC_STREAM_SECTION_DEBUG)
	HINT32	x=0,j=0;
#endif

	STREAM_SEM_GET;

	HxLOG_Print("\n [%s] Ok. We got descramble key *** !!!!!!!!!!!!!!!\n",__FUNCTION__);
	HxLOG_Print("[%s] streamHandle = 0x%X\n", __FUNCTION__, streamHandle);

// TODO: UC Manager 상에서의 SET PID 를 사용할 경우, 여기서 임의로 매치를 해줘야 하는 아픔이  잇 음.

#if defined(IRUC_STREAM_SECTION_DEBUG)
{
	HxLOG_Print("[UniversalClientSPI_Stream_SetDescramblingKey] Descramble KEY Value =(%s): ", (keyVersion==0)?"Even": "Odd");
	for(x = 0; x< pKeyInfo->pDescramblingKey->length; x++)
		HxLOG_Print("%02X ",pKeyInfo->pDescramblingKey->bytes[x] );
	HxLOG_Print("\n");
}
#endif

	for(i=0; i<IRUC_STREAM_MAX_COMPONENT; i++)
	{
		pComp = &(s_iruc_stream_info[streamHandle].astComponent[i]);

/* Comment : IRUC_INVALID_PID로 된 것은 set 하지 않는다. */
#if (1) //modified by MOONSG because the HDmini/me issue that is delayed audio playback have been fixed.
		if(pComp->ulPalDscIdx == IRUC_STREAM_INVALID_PALDSC_ID)
			continue;

		if( (pComp->bAllocated || pComp->bOtherAllocated)
			&& ((IRUC_INVALID_PID != pComp->stUC_ES_COMPONENT.componentStream.pid) || (IRUC_INVALID_PID != pComp->usOtherPid)) )
#else
		if((pComp->bAllocated || pComp->bOtherAllocated) && (IRUC_INVALID_PID != pComp->stUC_ES_COMPONENT.componentStream.pid))
#endif
		{
			//note For DVB, 0 indicates Even Key and 1 indicates Odd Key.
			if( keyVersion == 0)
			{
				VK_MEM_Memcpy( pComp->aucEven , pKeyInfo->pDescramblingKey->bytes, pKeyInfo->pDescramblingKey->length);
			}
			else
			{
				VK_MEM_Memcpy( pComp->aucOdd, pKeyInfo->pDescramblingKey->bytes, 	pKeyInfo->pDescramblingKey->length);
			}

/* Comment :
	bVirualComponent : humax mw 에서 set pid에 의해서 element PID 가 안들어온 상황이다.
 					   그러나 UC에서 add component 한 상황이다.
					   그래서 여기선 이것에 대해서 처리 하지 않게 막고 잇다. */
			if(pComp->bVirualComponent )
			{
				HxLOG_Print("[%s] Succeeded to set virtual key  !!!streamHandle= 0x%X, comp idx = %d\n",__FUNCTION__, streamHandle, i );
				continue;
			}

			HxLOG_Print("[%s] component addr(0x%08X), streamHandle=%d, comp pid=0x%04X, component idx = %d \n",__FUNCTION__, pComp, streamHandle, pComp->stUC_ES_COMPONENT.componentStream.pid, i);

#if defined(CONFIG_MW_CAS_IRDETO_SECURE)
			if ((CAS_IRUC_SPI_GetEnabledCSSK() == TRUE) && (pKeyInfo->descramblingKeyProtection != UC_DK_PROTECTION_CLEAR))
			{
				tmpCSSK = (HUINT8 *)CAS_IRUC_SPI_GetCSSK();

				svc_cas_DevDscSetProtectionKey(pComp->ulPalDscIdx, tmpCSSK);

#if defined(PRINT_CW)	// DEBUG
				{
					HxLOG_Print("[%s] tmpCSSK: ",__FUNCTION__);
					for(j = 0; j < 16; j++)
					{
						HxLOG_Print(".%02X", tmpCSSK[j]);
					}
					HxLOG_Print("\n");
				}
#endif
			}
#endif
			result =svc_cas_DevDscSetKey(pComp->ulPalDscIdx, (keyVersion==0)?ePAL_DSC_KEYPARITY_EVEN : ePAL_DSC_KEYPARITY_ODD, pKeyInfo->pDescramblingKey->bytes );
			if(ERR_OK != result)
			{
				HxLOG_Print("[%s] svc_cas_DevDscSetKey,stream (=0x%X), comp index(=0x%X) Error, result (=0x%X)\n", __FUNCTION__ ,streamHandle, i, result );
			}

			HxLOG_Print("[%s] Setting is done  !!!streamHandle= 0x%X, comp idx = %d\n",__FUNCTION__ , streamHandle, i );
		}
		else
		{
		//	HxLOG_Print("[%s]  not allocated idx(=0x%X)\n",__FUNCTION__,i );
		}

	}

	HxLOG_Print("[%s] Set CW -- END --  index(%d)  \n\n",__FUNCTION__,i);
	STREAM_SEM_RELEASE;


	return UC_ERROR_SUCCESS;

}

/**
 * Stream method to clean the descrambler key that stored in STB.
 *
 * IUC will invoke this SPI immediately when page not found to clean the descrambler key stored in STB.
 *
 * @param[in] streamHandle Stream handle originally returned from the stream's ::UniversalClientSPI_Stream_Open method.
 *
 *     \note The action of cleaning descrambler key must implement by application, and the descrambler key include even key and odd key.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *    ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Stream_CleanDescramblingKey(    uc_stream_handle streamHandle)
{
	HUINT8 aucNULL_KEY[8] ={0};
	IRUC_STREAM_COMPONENT *pComp=NULL;
	HINT32 i, result=ERR_OK;
	HxLOG_Print("[%s]=> start clean addr=0x%X, streamHandle = 0x%X\n",__FUNCTION__,&s_iruc_stream_info[streamHandle], streamHandle);

	STREAM_SEM_GET;

	for(i=0; i<IRUC_STREAM_MAX_COMPONENT; i++)
	{
		pComp = &(s_iruc_stream_info[streamHandle].astComponent[i]);
		if(((pComp->bAllocated) || (pComp->bOtherAllocated)) && (pComp->ulPalDscIdx != IRUC_STREAM_INVALID_PALDSC_ID))
		{
			//note For DVB, 0 indicates Even Key and 1 indicates Odd Key.
			result =svc_cas_DevDscSetKey(pComp->ulPalDscIdx, ePAL_DSC_KEYPARITY_EVEN, aucNULL_KEY );
			result |=svc_cas_DevDscSetKey(pComp->ulPalDscIdx, ePAL_DSC_KEYPARITY_ODD, aucNULL_KEY );
			if(ERR_OK != result)
			{
				HxLOG_Print("[%s] svc_cas_DevDscSetKey,stream (=0x%X), comp index(=0x%X) Error, result (=0x%X)\n",__FUNCTION__ ,streamHandle, i, result );
			}

/* Comment : descramble 되기 전에 key에 대한 clear를 해서  descrambler에 PID를 reset 하면 안된다.*/
			HxLOG_Print("[%s] CW is Cleared, Pid(0x%04X), DscIdx(%d)  !!!!!!\n",__FUNCTION__,pComp->stUC_ES_COMPONENT.componentStream.pid,pComp->ulPalDscIdx);
		}
		else
		{
			HxLOG_Print("[%s] not allocated idx(=0x%X) or invalid pal dsc id(=0x%X)\n",__FUNCTION__ , i , pComp->ulPalDscIdx);
		}
	}

	STREAM_SEM_RELEASE;

	return UC_ERROR_SUCCESS;
}


#define _______SPI_STREAM_DEBUG__________________________

HINT8* local_cas_iruc_Dbg_PrintStreamType(uc_connection_stream_type streamType)
{
	switch(streamType)
	{
		ENUM_TO_STR(UC_CONNECTION_STREAM_TYPE_EMM);
		ENUM_TO_STR(UC_CONNECTION_STREAM_TYPE_ECM);
		ENUM_TO_STR(UC_CONNECTION_STREAM_TYPE_PVR_RECORD);
		ENUM_TO_STR(UC_CONNECTION_STREAM_TYPE_PVR_PLAYBACK);
		ENUM_TO_STR(UC_CONNECTION_STREAM_TYPE_FTA);
		ENUM_TO_STR(UC_CONNECTION_STREAM_TYPE_RESERVED);
	}

	return "Unknown StreamType";
}


void local_cas_iruc_StreamSPI_DBG_Info(void)
{
	HUINT32	i, j;

	HxLOG_Print("==================== local_cas_iruc_StreamSPI_DBG_Info ====================\n");

	for( i = 0; i < IRUC_STREAM_MAX_TOTAL_CHANNEL; i++ )
	{
		if( s_iruc_stream_info[i].bUsed)
		{
			HxLOG_Print("[%d]bUsed : 0x%X\n", i, s_iruc_stream_info[i].bUsed);
			HxLOG_Print("[%d]SvcContext : 0x%X\n", i, s_iruc_stream_info[i].SvcContext);
			HxLOG_Print("[%d]ucConnStreamType : 0x%X\n", i, s_iruc_stream_info[i].ucConnStreamType);
			HxLOG_Print("[%d]usPid : 0x%X\n", i, s_iruc_stream_info[i].usPid);
			HxLOG_Print("[%d]astConnection.bAllocated : 0x%X\n", i, s_iruc_stream_info[i].astConnection.bAllocated);
			HxLOG_Print("[%d]astConnection.connectionHandle : 0x%X\n", i, s_iruc_stream_info[i].astConnection.connectionHandle);
			for( j = 0; j < IRUC_STREAM_MAX_TOTAL_CHANNEL; j++ )
			{
				if(s_iruc_stream_info[i].astFilter[j].Allocated)
				{
					HxLOG_Print("\t[%d]AllocRefID : 0x%X\n", j, s_iruc_stream_info[i].astFilter[j].AllocRefID);
					HxLOG_Print("\t[%d]RequestID : 0x%X\n", j, s_iruc_stream_info[i].astFilter[j].RequestID);
					HxLOG_Print("\t[%d]Allocated : 0x%X\n", j, s_iruc_stream_info[i].astFilter[j].Allocated);
					HxLOG_Print("\t[%d]Started : 0x%X\n", j, s_iruc_stream_info[i].astFilter[j].Started);

					HxLOG_Print("\t[%d]index : %d\n",j, j );
					HxLOG_Print("\t[%d]wFilterLength : %d\n",j , s_iruc_stream_info[i].astFilter[j].wFilterLength);

					HxLOG_Print("\t\t\t[%d]::[%02X][%02X][%02X][%02X][%02X][%02X][%02X][%02X]\n", j,
							s_iruc_stream_info[i].astFilter[j].aucFilterValue[0],
							s_iruc_stream_info[i].astFilter[j].aucFilterValue[1],
							s_iruc_stream_info[i].astFilter[j].aucFilterValue[2],
							s_iruc_stream_info[i].astFilter[j].aucFilterValue[3],
							s_iruc_stream_info[i].astFilter[j].aucFilterValue[4],
							s_iruc_stream_info[i].astFilter[j].aucFilterValue[5],
							s_iruc_stream_info[i].astFilter[j].aucFilterValue[6],
							s_iruc_stream_info[i].astFilter[j].aucFilterValue[7] );

					HxLOG_Print("\t\t\t[%d]::[%02X][%02X][%02X][%02X][%02X][%02X][%02X][%02X]\n", j,
							s_iruc_stream_info[i].astFilter[j].aucFilterMask[0],
							s_iruc_stream_info[i].astFilter[j].aucFilterMask[1],
							s_iruc_stream_info[i].astFilter[j].aucFilterMask[2],
							s_iruc_stream_info[i].astFilter[j].aucFilterMask[3],
							s_iruc_stream_info[i].astFilter[j].aucFilterMask[4],
							s_iruc_stream_info[i].astFilter[j].aucFilterMask[5],
							s_iruc_stream_info[i].astFilter[j].aucFilterMask[6],
							s_iruc_stream_info[i].astFilter[j].aucFilterMask[7] );
				}
				else
					HxLOG_Print("\t[%d]Allocated : 0x%X\n", j, s_iruc_stream_info[i].astFilter[j].Allocated);

			}
		}
		else
			HxLOG_Print("[%d]bUsed : 0x%X\n", i, s_iruc_stream_info[i].bUsed);

	}
	HxLOG_Print("\n\n========================== END ==========================\n");
}


/** @} */ /* End of streamspi */


