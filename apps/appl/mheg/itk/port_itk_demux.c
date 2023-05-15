/**
	@file	  port_itk_demux.c													\n
	@brief	  itk file for DEMUX porting layer									\n

	Description: ITK에서 DEMUX Porting 에 사용되는 기능을 정의한 Body파일  		\n
	Module: MW/mheg	/itk								 						\n

	Copyright (c) 2008 HUMAX Co., Ltd.											\n
	All rights reserved.														\n

	[Description]
	1. s_pstSiRawList 은 ITK Main Task에서만 Access를 하므로 Semaphore를 사용할 필요가
	  없다.

*/


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <vkernel.h>
#include <silib.h>
#include <apk.h>

#include "mheg_sef.h"
#include "mheg_si.h"
#include "mheg_param.h"
#include "mheg_int.h"
#include "itk_service.h"

#include "port_itk_main.h"
#include "port_itk_demux.h"
#include "port_itk_service.h"


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#if 0	// This is a trade-off. you have to choose only one.
#define ITK_DEMUX_USE_DSMCC_TASK				1
#else
#define ITK_DEMUX_USE_DSMCC_TASK_2				1
#endif

//#define USE_VK_TIMER_TASK						1

#if defined(USE_VK_TIMER_TASK)
#define SUPPORT_SI_CB_TIMER						1
#endif

#define ITK_DEMUX_INVALID_REQUEST_ID			0xFFFFFFFF

#define ITK_DEMUX_SITABLE_ID_MIN				0x0001
#define ITK_DEMUX_SITABLE_ID_MAX				0xFFFF
#if defined(USE_VK_TIMER_TASK)
#define ITK_DEMUX_SITABLE_TIME_OUT				1000
#else
#define ITK_DEMUX_SITABLE_MONITOR_TIME				500
#endif

#define ITK_SCTION_TIMEOUT_VALUE				10000
#define ITK_MAX_FILTER_ID_VALUE					0xFFF0

#define ITK_DEMUX_VALUE_DONT_CARE				0xFFFE
#if !defined(USE_VK_TIMER_TASK)
#define ITK_DEMUX_TABLE_ID_DISABLED				0xFF
#endif

/* REMARK : let's consider if we should check CRC value in release version or not....
   S&T doc says Testing the CRC in software on the reciever side is time consuming and will
   normally only be enabled for debug builds */
#ifdef CONFIG_DEBUG
	#define ITK_SECTION_CRC_FLAG				1
#else
	#define ITK_SECTION_CRC_FLAG				1					// choiyj - there is a rebooting problem.
#endif

#define GET_SECTION_LEN_FROM_RAWDATA(data)	(((data[1] << 8) | data[2]) & 0x0FFF)
#define GET_TABLE_ID_FROM_RAWDATA(data)			(data[0])
#define GET_EXT_TABLE_ID_FROM_RAWDATA(data)	((data[3] << 8) | data[4])
#if !defined(USE_VK_TIMER_TASK)
#define ITK_MAX_TABLE_NUM					(32)
#endif

#define	HIGHWORD(longWord)	((HUINT16)( sizeof(longWord) == 4 ? ((HUINT16)((0xFFFF0000 & longWord) >> 16)) : 0 ))
#define	LOWWORD(longWord)	((HUINT16)( sizeof(longWord) == 4 ? ((HUINT16)(0x0000FFFF & longWord)) : longWord ))

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct tagITK_SessionHandle_t
{
	itk_demux_session_t 			vtbl;

	itk_uint16_t 					onid;
	itk_uint16_t 					tsid;
	itk_demux_session_callback_t		callback;
	void 							*callback_context;
	HUINT32 						ulFilterNum;
	//Handle_t						hAction;			/* action handler */
	HUINT32							ulViewId;

} ITK_SessionHandle_t;


typedef struct tagITK_FilterHandle_t
{
	itk_demux_filter_t 			vtbl;

	itk_control_t 				control;
	itk_uint16_t 				pid;
	HUINT16						filterHandle;		/* unique id of filter handler */
	HUINT32						ulRequestId;

	itk_uint8_t 				match[MHEG_SEF_MAX_FILTER_LEN];
	itk_uint8_t 				mask[MHEG_SEF_MAX_FILTER_LEN];
	itk_uint8_t 				depth;
	HUINT8						ucDemuxId;

	void 						*caller;

	//struct tagITK_FilterHandle_t	*pPrev;
	struct tagITK_FilterHandle_t	*pNext;

}ITK_FilterHandle_t;


typedef struct tagITK_SITableHandle_t
{
	itk_demux_filter_t 				vtbl;
	itk_control_t 					control;
	itk_uint16_t 					onid;
	itk_uint16_t 					tsid;
	itk_uint16_t 					pid;
	itk_uint8_t 					table_id;
	itk_uint16_t 					table_id_extn;
	itk_demux_si_callback_t 		callback;
	void 							*callback_context;

	HUINT8							ucDemuxId;
	HUINT8							dummy[2];

	HULONG							ulTimerId;
	HUINT32 						ulSITableId;

	//struct tagITK_SITableHandle_t	*pPrev;
	struct tagITK_SITableHandle_t	*pNext;
} ITK_SITableHandle_t;


typedef struct tagITK_SiRawDataList_t
{
	Handle_t	hAction;				/* action handle for Watch TV related with current SI Event */
	HUINT16		uniqeId;				/* tsIdx */
	HUINT16		pid;
	HUINT8		tableId;
	HUINT16		tableIdExtn; 			/* 4th ~ 5th bytes of Section data */
	HUINT16 	savedRawDataSize;
	HUINT8		*savedRawData;

	struct tagITK_SiRawDataList_t	*pNext;

}ITK_SiRawDataList_t;

#if !defined(USE_VK_TIMER_TASK)
typedef struct tagITK_SiMonitor_t
{
	HUINT32			ulSiTableId;
	HBOOL			bControl;
}ITK_SiMonitor_t;
#endif

enum {
	MHEG_MSG_DSMCC_DATA = 100,
	MHEG_MSG_DSMCC_CLEAR,
 };


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static itk_demux_t				s_stDemuxObject;
static HULONG					s_ulSessionFilterSema;
static HULONG					s_ulSiTblSema;
static HULONG					s_ulItkDmxRcvQId;
static ITK_FilterHandle_t*		s_pstFilterHeadList;
static ITK_SITableHandle_t*		s_pstSiHandleList;
#ifdef CONFIG_DEBUG
static ITK_SiRawDataList_t*		s_pstSiRawList;
#endif
static HUINT16					s_usFilterHandleIdGenerator;
static HBOOL					s_bIsObjectAvailable = FALSE;
#if (ITK_DEMUX_USE_DSMCC_TASK || ITK_DEMUX_USE_DSMCC_TASK_2)
static HULONG					s_ulItkDmxRcvTaskId;
#endif
//static HUINT8					s_usDebugCnt;

#if !defined(USE_VK_TIMER_TASK)
static HULONG					s_ulItkSiMonitorTaskId;
static HULONG 					s_ulitkSiMonitorSema;
static ITK_SiMonitor_t			s_itkSiMonitorList[ITK_MAX_TABLE_NUM];
//static HUINT32			s_itkSiMonitorCnt=0; // linked list로 만들 경우 사용
#endif

/********************************************************************/
/********************      proto-type functions  ********************/
/********************************************************************/

static itk_demux_filter_t* 		local_itk_DEMUX_GetFilter(struct itk_demux_session_t* thiz, itk_uint16_t pid,
															const itk_uint8_t* match, const itk_uint8_t* mask, itk_uint8_t depth);
static void 					local_itk_DEMUX_ControlFilter(struct itk_demux_filter_t* thiz, itk_control_t control);
static void 					local_itk_DEMUX_ReleaseFilter(itk_demux_filter_t* thiz);
static void 					local_itk_DEMUX_ReleaseSession(struct itk_demux_session_t* thiz);
static void 					local_itk_DEMUX_ReleaseSiTable(struct itk_demux_filter_t *thiz);


static HUINT16 					local_itk_DEMUX_GetNewFilterHandleId(void);
static ITK_FilterHandle_t* 		local_itk_DEMUX_AppendFilterToList(ITK_FilterHandle_t* pstFilter);
#if ITK_DEMUX_USE_DSMCC_TASK
static ITK_FilterHandle_t* 		local_itk_DEMUX_GetFilterHandleByReqId(HUINT32 ulRequestId, HUINT8 ucDmxId);
#endif
static ITK_FilterHandle_t* 		local_itk_DEMUX_GetFilterHandlerByHandlerId(HUINT16 usHandlerId, HUINT8 ucDmxId);
static ITK_FilterHandle_t* 		local_itk_DEMUX_RemoveFilterFromList(ITK_FilterHandle_t* pstFilter);
static HERROR 					local_itk_DEMUX_DisableFilter(ITK_FilterHandle_t* pstFilter);
static HERROR 					local_itk_DEMUX_EnableFilter(ITK_FilterHandle_t* pstFilter);
//static HERROR 					local_itk_DEMUX_AppendSiTable(Handle_t hAction, HUINT16 usPid, HUINT16 usTsUniq, HUINT8* pRawData);
static HUINT32 					local_itk_DEMUX_GetNewSITableId(void);
static ITK_SITableHandle_t* 	local_itk_DEMUX_GetSiTableHandler(HUINT32 ulSITableId);
static ITK_SITableHandle_t* 	local_itk_DEMUX_AppendSiTableToList(ITK_SITableHandle_t* pstSiTbl);
static ITK_SITableHandle_t* 	local_itk_DEMUX_RemoveSiTableFromList(ITK_SITableHandle_t* pstSiTbl);
static void 					local_itk_DEMUX_DisableSiTable(ITK_SITableHandle_t* pstSiTbl);
static void 					local_itk_DEMUX_EnableSiTable(ITK_SITableHandle_t* pstSiTbl);
#if defined(USE_VK_TIMER_TASK)
static void 					local_itk_DEMUX_CbOnTimerSITable(unsigned long ulTimerId, void* pContext);
#else
static void 					local_itk_DEMUX_MonitorTask(void* pArg);
#endif
static void 					local_itk_DEMUX_ResetAllFilter(void);
static void 					local_itk_DEMUX_ResetAllSiTable(void);
static void 					local_itk_DEMUX_ParseDsmccSection(HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, Handle_t hAction);
#if 0	// not used
static void 					local_itk_DEMUX_FreeDsmccSection(HUINT32 usTsUniqId, HUINT32 ulTableId, HUINT32 ulExternalValue, HUINT32 ulParam2);
#endif
#if (ITK_DEMUX_USE_DSMCC_TASK || ITK_DEMUX_USE_DSMCC_TASK_2)
static void 					local_itk_DEMUX_RcvTask(void* pArg);
static void 					local_itk_DEMUX_ProcessRecvSection(HMSG* pstMsg);
#endif

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

ITK_SITableHandle_t* local_itk_DEMUX_AppendSiTableToList(ITK_SITableHandle_t* pstSiTbl)
{
	ITK_SITableHandle_t*		pstList = NULL;

	HxLOG_Debug(" Append SiTbl (%p): table_id(0x%x) \n", pstSiTbl, pstSiTbl->table_id);

	if(pstSiTbl == NULL)
		return NULL;
	
	pstSiTbl->pNext = NULL;

	if(s_pstSiHandleList == NULL)
	{
		s_pstSiHandleList = pstSiTbl;
		return s_pstSiHandleList;
	}

	pstList = s_pstSiHandleList;
	while(pstList->pNext)
		pstList = pstList->pNext;
#if 0 //prevent #184058
	if(pstList == NULL)
		return NULL;
#endif
	pstList->pNext = pstSiTbl;

	return NULL;
}


ITK_SITableHandle_t* local_itk_DEMUX_RemoveSiTableFromList(ITK_SITableHandle_t* pstSiTbl)
{
	/*
	    This function must be locked with semaphore before enter here !!!
	*/

	ITK_SITableHandle_t*	pstList = NULL, *pstPrevList = NULL;

	if(pstSiTbl == NULL)
		return NULL;

	if (s_pstSiHandleList == NULL)
	{
		HxLOG_Error(" Remove SiTbl :  All filter already removed !!!!!\n");
		return NULL;
	}

	HxLOG_Debug(" Remove SiTbl (%p): table_id(0x%x), table_id_extn(0x%x) \n", pstSiTbl, pstSiTbl->table_id, pstSiTbl->table_id_extn);

	pstList = s_pstSiHandleList;
	while(pstList)
	{
		if(pstList == pstSiTbl)
		{
			if(pstPrevList == NULL)
				s_pstSiHandleList = pstSiTbl->pNext;
			else
				pstPrevList->pNext = pstSiTbl->pNext;

			break;
		}

		pstPrevList = pstList;
		pstList = pstList->pNext;
	}

	return s_pstSiHandleList;

}


ITK_FilterHandle_t* local_itk_DEMUX_GetFilterHandlerByHandlerId(HUINT16 usHandlerId, HUINT8 ucDmxId)
{
	/*
	    This function must be locked with semaphore before enter here !!!
	*/
	ITK_FilterHandle_t*		pstFilter = NULL;

	pstFilter = s_pstFilterHeadList;
	while(pstFilter)
	{
		if(pstFilter->filterHandle == usHandlerId && pstFilter->ucDemuxId == ucDmxId)
			break;

		pstFilter = pstFilter->pNext;
	}

	HxLOG_Debug(" Get filter (%p) usHandlerId (%d), ucDmxId (%d)\n", pstFilter, usHandlerId, ucDmxId);

	return pstFilter;
}

#if ITK_DEMUX_USE_DSMCC_TASK
ITK_FilterHandle_t* local_itk_DEMUX_GetFilterHandleByReqId(HUINT32 ulRequestId, HUINT8 ucDmxId)
{
	/*
	    This function must be locked with semaphore before enter here !!!
	*/
	ITK_FilterHandle_t*	pstFilter = NULL;

	pstFilter = s_pstFilterHeadList;
	while(pstFilter)
	{
		if(pstFilter->ulRequestId == ulRequestId && pstFilter->ucDemuxId == ucDmxId)
			break;

		pstFilter = pstFilter->pNext;
	}

	HxLOG_Debug(" Get filter (%p) ulRequestId (%d), ucDmxId (%d)\n", pstFilter, ulRequestId, ucDmxId);

	return pstFilter;
}
#endif

ITK_FilterHandle_t* local_itk_DEMUX_AppendFilterToList(ITK_FilterHandle_t* pstFilter)
{
	/*
	    This function must be locked with semaphore before enter here !!!
	*/
	ITK_FilterHandle_t*		pstList = NULL;

	if(pstFilter == NULL)
		return NULL;

	HxLOG_Debug(" Append Filter (%p) : pid(0x%x) \n", pstFilter, pstFilter->pid);

	pstFilter->pNext = NULL;
	if(s_pstFilterHeadList == NULL)
	{
		s_pstFilterHeadList 	= pstFilter;
		return s_pstFilterHeadList;
	}

	pstList = s_pstFilterHeadList;
	while(pstList && pstList->pNext)
		pstList = pstList->pNext;

	if(pstList == NULL)
		return NULL;
	
	pstList->pNext = pstFilter;

	return s_pstFilterHeadList;

}


ITK_FilterHandle_t* local_itk_DEMUX_RemoveFilterFromList(ITK_FilterHandle_t* pstFilter)
{
	ITK_FilterHandle_t*		pstList = NULL, *pstPrevList = NULL;

	/*
	    This function must be locked with semaphore before enter here !!!
	*/
	if(pstFilter == NULL)
		return NULL;

	if(s_pstFilterHeadList == NULL)
	{
		HxLOG_Error("Remove Filter : All filter already removed !!!!!\n");
		return NULL;
	}

	HxLOG_Debug("Remove Filter (%p): pid(0x%x) \n", pstFilter, pstFilter->pid);

	pstList = s_pstFilterHeadList;
	while(pstList)
	{
		if(pstList == pstFilter)
		{
			if(pstPrevList)
				pstPrevList->pNext = pstFilter->pNext;
			else
				s_pstFilterHeadList = pstFilter->pNext;

			break;
		}

		pstPrevList = pstList;
		pstList = pstList->pNext;
	}

	return s_pstFilterHeadList;

}


HUINT16 local_itk_DEMUX_GetNewFilterHandleId()
{
	if(s_usFilterHandleIdGenerator >= ITK_MAX_FILTER_ID_VALUE)
		s_usFilterHandleIdGenerator = 1;

	return s_usFilterHandleIdGenerator++;
}

void local_itk_DEMUX_ReleaseSession(struct itk_demux_session_t* thiz)
{
	ITK_SessionHandle_t*		pstSession = NULL;
	ITK_FilterHandle_t*		pstFilterList = NULL, *pstNextFilterList = NULL;

	/* check validation of inputparameter */
	if((pstSession = (ITK_SessionHandle_t*)thiz) == NULL)
	{
		return;
	}

	if(pstSession->ulFilterNum > 0)
	{
		/* we must release all filters created in this session before release Session context */
		VK_SEM_Get(s_ulSessionFilterSema);

		pstFilterList = s_pstFilterHeadList;
		while(pstFilterList)
		{
			pstNextFilterList = pstFilterList->pNext;
			if(pstFilterList->caller == pstSession)
			{
				/* disable filter */
				local_itk_DEMUX_DisableFilter(pstFilterList);

				/* remove filter handler in list */
				local_itk_DEMUX_RemoveFilterFromList(pstFilterList);

				/* mem free */
				HLIB_STD_MemFree(pstFilterList);
				pstFilterList = NULL;
				pstSession->ulFilterNum--;
			}

			pstFilterList = pstNextFilterList;
		}

		if (pstSession->ulFilterNum == 0)
		{
			s_pstFilterHeadList = NULL;
		}

		VK_SEM_Release(s_ulSessionFilterSema);
	}
	
	if(pstSession->ulFilterNum != 0)
	{
		HxLOG_Error("Release session fail!!");
	}
	
	HLIB_STD_MemFree(pstSession);
	pstSession = NULL;
	HxLOG_Debug(" success to release session \n");

}


itk_demux_filter_t* local_itk_DEMUX_GetFilter(struct itk_demux_session_t* thiz, itk_uint16_t pid,
												const itk_uint8_t* match, const itk_uint8_t* mask, itk_uint8_t depth)
{
	ITK_SessionHandle_t*	pstSession = (ITK_SessionHandle_t*)thiz;
	ITK_FilterHandle_t*		pstFilter = NULL;
	//HUINT32					ulActionId = 0;
	HUINT32					ulViewId = 0;
	HUINT32					ulDmxId = 0;
	HERROR					ulError = 0;
	HUINT16					ucIndex = 0;

	HxLOG_Debug(" Call !!!! pid(0x%x) depth(%d) \n", pid, depth);
	/* check validation of input parameters */
	if(pstSession == NULL)
	{
		return NULL;
	}

	if(mask == NULL || match == NULL)
	{

		HxLOG_Error(" Invalid Mask and Match bits !!\n");
		return NULL;
	}

	/* get demux Id from hAction */
	//ulActionId = SVC_PIPE_GetActionId(pstSession->hAction);
	//ulError = SVC_PIPE_GetResourceId(ulActionId, eRxRSCTYPE_DEMUX, &ulDmxId);
	//if(ulError != ERR_OK)
		//return NULL;
	
	ulViewId	= pstSession->ulViewId;
	ulDmxId	= PORT_ITK_GetDemuxId(ulViewId);
	
	/* creates Filter handler */
	pstFilter = (ITK_FilterHandle_t*)HLIB_STD_MemAlloc(sizeof(ITK_FilterHandle_t));
	if(pstFilter == NULL)
		return NULL;

	pstFilter->vtbl.control	= local_itk_DEMUX_ControlFilter;
	pstFilter->vtbl.release	= local_itk_DEMUX_ReleaseFilter;

	pstFilter->filterHandle	= local_itk_DEMUX_GetNewFilterHandleId();
	pstFilter->depth		= depth;
	pstFilter->pid			= pid;
	pstFilter->caller		= (void*)pstSession;
	pstFilter->control		= ITK_DISABLE;	/* initial value -> ITK_DISABLE */
	pstFilter->ucDemuxId	= (HUINT8)ulDmxId;
	pstFilter->ulRequestId	= ITK_DEMUX_INVALID_REQUEST_ID;
	pstFilter->pNext		= NULL;
	//pstFilter->pPrev		= NULL;

#if 1
	/* for debugging */
	HxLOG_Debug("Match : ");
	for(ucIndex = 0; ucIndex < depth; ucIndex++)
	{
		HxLOG_Debug("[0x%x] ",match[ucIndex]);
	}
	HxLOG_Debug("\n");

	/* for debugging */
	HxLOG_Debug("Mask : ");
	for(ucIndex = 0; ucIndex < depth; ucIndex++)
	{
		HxLOG_Debug("[0x%x] ",mask[ucIndex]);
	}
	HxLOG_Debug("\n");
	/* to here */
#endif

	for(ucIndex = 0; ucIndex < MHEG_SEF_MAX_FILTER_LEN; ucIndex++)
	{
		if(ucIndex < depth)
		{
			pstFilter->match[ucIndex] = match[ucIndex];
			pstFilter->mask[ucIndex] =  mask[ucIndex] ^ 0xFF;
		}
		else
		{
			pstFilter->match[ucIndex] = 0xff;
			pstFilter->mask[ucIndex] = 0x00;
		}
	}

	VK_SEM_Get(s_ulSessionFilterSema);			/* lock filter Semaphore */

	/* append Filter list */
	local_itk_DEMUX_AppendFilterToList(pstFilter);

	/* increase filter count in session handler */
	if(ulError == ERR_OK)
		pstSession->ulFilterNum++;

	VK_SEM_Release(s_ulSessionFilterSema);		/* release Filter Semaphore */

	return (itk_demux_filter_t*)pstFilter;

}


void local_itk_DEMUX_ControlFilter(struct itk_demux_filter_t* thiz, itk_control_t control)
{
	ITK_FilterHandle_t*			pstFilter = (ITK_FilterHandle_t*)thiz;

	if(pstFilter == NULL)
	{
		return;
	}

	HxLOG_Debug(" control filter --> %d \n", control);
	VK_SEM_Get(s_ulSessionFilterSema);

	switch(control)
	{
		case ITK_ENABLE:
			if(pstFilter->control == ITK_DISABLE)
				local_itk_DEMUX_EnableFilter(pstFilter);
			break;

		case ITK_DISABLE:
			if(pstFilter->control == ITK_ENABLE)
				local_itk_DEMUX_DisableFilter(pstFilter);
			break;

		default:
			break;
	}

	VK_SEM_Release(s_ulSessionFilterSema);

}



/*
   parameter :
           ulUserParam - Hiword(FilterHandler Id) | Low Word(dmuxID)

   Remark : This function is running at SiRecv Task !!!!!!
*/
#if 0
void local_itk_DEMUX_ParseDsmccSection(HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, HUINT32 ulUserParam)
{
	HUINT16					usResult, usFilterHandlerId;
	HUINT16					i, cnt;
	ITK_FilterHandle_t*		pstFilterHandler = NULL;
	ITK_SessionHandle_t*	pstSession = NULL;
	MhegSef_RawData_t*			pSiRawData = NULL;
	HUINT8					ucDmxId;


	//HxLOG_Debug(" ulTableAndresult(0x%x), ulParam(0x%x), userParam(0x%x) \n", ulTableIdAndResult, ulParam, ulUserParam);
	PARAM_UNUSED(ulFilterAndUniqId);
	usResult = LOWWORD(ulTableIdAndResult);
	if((usResult != (HUINT16)eSiFilterResult_Complete) || ulParam == 0)
	{
		// TODO:
		/* do something, here !!! */

		HxLOG_Debug(" Error or Time out !!!!! \n");
		return;
	}

	pSiRawData 			= (MhegSef_RawData_t*)ulParam;
	usFilterHandlerId 	= HIGHWORD(ulUserParam);
	ucDmxId				= (HUINT8)LOWWORD(ulUserParam);

	//HxLOG_Debug(" ParseDsmSecion: filterHandle(0x%x), dmxId(%d)\n ", usFilterHandlerId, ucDmxId);

	VK_SEM_Get(s_ulSessionFilterSema);

	if((pstFilterHandler = local_itk_DEMUX_GetFilterHandlerByHandlerId(usFilterHandlerId, ucDmxId)) == NULL)
	{
		HxLOG_Debug(" No filter exist, filterHandlerId(0x%x), dmxId(%d) \n", usFilterHandlerId, ucDmxId);
		VK_SEM_Release(s_ulSessionFilterSema);
		return;
	}

	pstSession = (ITK_SessionHandle_t*)pstFilterHandler->caller;
	if(pstFilterHandler->control == ITK_ENABLE && pstSession != NULL)
	{
	#if 1
		HUINT16	 	usRawLen = 0;
		HUINT8		*pucNewSec;

		pucNewSec = pSiRawData->buf[0];
		usRawLen = GET_SECTION_LEN_FROM_RAWDATA(pucNewSec) + 3;

		if(pstSession->callback && usRawLen)
		{
			HxLOG_Debug(" callback: tableId(0x%x) usRawLen(%d), dmxId(%d)  \n ", pucNewSec[0], usRawLen, ucDmxId);
			usRawLen = GET_SECTION_LEN_FROM_RAWDATA(pSiRawData->buf[0]) + 3;
			pstSession->callback(pstSession->callback_context, (itk_demux_session_t*)pstSession, pucNewSec, usRawLen);
		}

	#else
		for ( i=0, cnt = 0; cnt < pSiRawData->num_sec && i < SvcSi_MAX_SECTION_NUM; i++)
		{
			if(pSiRawData->buf[i])
			{
				HUINT16	 	usRawLen;
				HUINT8		*pucTemp, *pucNewSec;

				pucTemp = pSiRawData->buf[i];
				usRawLen = GET_SECTION_LEN_FROM_RAWDATA(pucTemp) + 3;
				pucNewSec = HLIB_STD_MemAlloc(usRawLen);
				if(pucNewSec == NULL)
					return;
				HxSTD_memcpy(pucNewSec, pucTemp, usRawLen);

				if(pstSession->callback)
				{
					//HxLOG_Debug(" callback: usRawLen(%d), pucNewSec(0x%x) ,[%d] num_sec(%d)  \n ", usRawLen, pucNewSec, i, pSiRawData->num_sec);
					pstSession->callback(pstSession->callback_context, (itk_demux_session_t*)pstSession, pucNewSec, usRawLen);
				}

				HLIB_STD_MemFree(pucNewSec);
				cnt++;

				//HxLOG_Debug("@");	/* zombie trace !! */
			}
		}
	#endif
	}
	else
		HxLOG_Debug(" Can not invoke callback func : control status=%d \n",pstFilterHandler->control);

	VK_SEM_Release(s_ulSessionFilterSema);

}
#else
void local_itk_DEMUX_ParseDsmccSection(HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, HUINT32 ulUserParam)
{
	HUINT16					usResult, usFilterHandlerId;
	MhegSef_RawData_t*		pSiRawData = NULL;
	HUINT8					ucDmxId;
	HMSG					outDsmCCMsg;
	HERROR					hErr = ERR_OK;
	HUINT16	 				usRawLen = 0;
	HUINT8					*pucTemp,*pucNewSec;

	//HxLOG_Debug(" ulTableAndresult(0x%x), ulParam(0x%x), userParam(0x%x) \n", ulTableIdAndResult, ulParam, ulUserParam);
	PARAM_UNUSED(ulFilterAndUniqId);
	usResult = LOWWORD(ulTableIdAndResult);
	if((usResult != (HUINT16)eSiFilterResult_Complete) || ulParam == 0)
	{
		// TODO:
		/* do something, here !!! */

		HxLOG_Error(" Error or Time out !!!!! \n");
		return;
	}

	pSiRawData 			= (MhegSef_RawData_t*)ulParam;
	usFilterHandlerId 	= HIGHWORD(ulUserParam);
	ucDmxId				= (HUINT8)LOWWORD(ulUserParam);

	//HxLOG_Debug(" ParseDsmSecion: filterHandle(0x%x), dmxId(%d)\n ", usFilterHandlerId, ucDmxId);

	pucTemp = pSiRawData->buf[0];
	usRawLen = GET_SECTION_LEN_FROM_RAWDATA(pucTemp) + 3;

	if(usRawLen == 3 || usRawLen > 4096)
	{
		HxLOG_Error(" invalid sectionLen(%d) \n",usRawLen);
		return;
	}

	pucNewSec = HLIB_STD_MemAlloc(usRawLen);
	if(pucNewSec == NULL)
	{
		HxLOG_Error(" Memory Allocation Error(0x%x), dmxId(%d) \n", usFilterHandlerId, ucDmxId);
		return;
	}
	HxSTD_memcpy(pucNewSec,pucTemp, usRawLen);

	HxSTD_memset(&outDsmCCMsg, 0, sizeof(HMSG));
	outDsmCCMsg.usMsgClass 	= MHEG_MSG_DSMCC_DATA;
	outDsmCCMsg.ulParam 	= (HUINT32)pucNewSec;
	outDsmCCMsg.usParam		= usFilterHandlerId;
	outDsmCCMsg.unInfo.ausS[0] = ucDmxId;
	outDsmCCMsg.unInfo.ausS[1] = usRawLen;

	hErr = VK_MSG_SendTimeout(s_ulItkDmxRcvQId, (void*)&outDsmCCMsg, sizeof(HMSG), 0);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("pucNewSec (%p), usRawLen (%d), usFilterHandlerId (0x%x), ucDmxId (0x%x)\n", pucNewSec, usRawLen, usFilterHandlerId, ucDmxId);
		HxLOG_Error("Error from VK_MSG_SendTimeout()\n");
		HLIB_STD_MemFree(pucNewSec);
		return ;
	}

}

#endif

#if 0		// not used
void local_itk_DEMUX_FreeDsmccSection(HUINT32 usTsUniqId, HUINT32 ulTableId, HUINT32 ulExternalValue, HUINT32 ulParam2)
{
	HxLOG_Debug(" do not use this function in DSM-CC section \n");
}
#endif



HERROR local_itk_DEMUX_EnableFilter(ITK_FilterHandle_t* pstFilter)
{
	HERROR			ulRet = ERR_OK;
	//HUINT8			aucMode[MHEG_SEF_MAX_FILTER_LEN];
	MhegSef_FilterParam_t	stFilterParam;
	HINT32			nCurSvcUid, nDefSvcUid;
	HUINT32			ulUserParam;
	HUINT16			usTsIdx = 0;
	DxService_t		*pstSvcInfo;

	if(pstFilter == NULL)
		return ERR_FAIL;

	HxLOG_Debug(" Enable Session Filter Pid(0x%x) \n", pstFilter->pid);

	/* we take care of the value Control in whose value is ITK_DISABLE */
	if(pstFilter->control == ITK_DISABLE)
	{

#if ITK_DEMUX_USE_DSMCC_TASK
		HUINT8		aucMode[MHEG_SEF_MAX_FILTER_LEN];

		/* currently, below code has not been compiled */
		HxSTD_memset(aucMode, 0x00, MHEG_SEF_MAX_FILTER_LEN);
		ulRet = MHEG_SEF_EnableSectionFilter(pstFilter->ucDemuxId,
									   pstFilter->pid,
									   s_ulItkDmxRcvQId,
									   0,
									   0,
									   0,
									   pstFilter->depth,
									   pstFilter->match,
									   pstFilter->mask,
									   aucMode,
									   &(pstFilter->ulRequestId),
									   TRUE);
#else // ITK_DEMUX_USE_DSMCC_TASK_2

		//DB_SVC_GetUniqueIdFromSvcHandle(hDefSvc, &usUniqId);
		PORT_ITK_GetSvcUid(&nDefSvcUid, &nCurSvcUid);
		pstSvcInfo = APK_META_SVC_GetService(nDefSvcUid);
		if (pstSvcInfo == NULL)
		{
			return ERR_FAIL;
		}
		usTsIdx = pstSvcInfo->tsuid;
		APK_META_SVC_Delete(pstSvcInfo);
		
		ulUserParam = (HUINT32)((pstFilter->filterHandle << 16) | (HUINT16)pstFilter->ucDemuxId);

		MHEG_SEF_SiMakeDefaultFilterParam(&stFilterParam,
        								  0x00, pstFilter->ucDemuxId, usTsIdx, pstFilter->pid,
        								  0, 0,  					/* do not use table filter and ext table id */
        								  0,
        								  ITK_SECTION_CRC_FLAG, 	/* check CRC */
        								  0x0000, 0x00,				/* we're using Itk own filtering depth */
        								  eSiFilterMode_Continue,
        								  0,						/* if timeout is 0, no timeout. original value is ITK_SCTION_TIMEOUT_VALUE */
        								  ulUserParam,
        								  local_itk_DEMUX_ParseDsmccSection,
        								  NULL);					/* don't have to use free callback */

		stFilterParam.eFilterLen = ((MhegSef_FilterLength_e)pstFilter->depth > eSefFilterLen_8Bytes) ? eSefFilterLen_16Bytes : eSefFilterLen_8Bytes;

		/* set filtering depth manually */
		HxSTD_memcpy(stFilterParam.pFilterValue, pstFilter->match, MHEG_SEF_MAX_FILTER_LEN);
		HxSTD_memcpy(stFilterParam.pFilterMask, pstFilter->mask, MHEG_SEF_MAX_FILTER_LEN);

		pstFilter->ulRequestId = MHEG_SEF_StartSecFilter(&stFilterParam);

		HxLOG_Debug(C_CODE_F_WHITE_B_PURPLE" uniqId(%d) demux(%d), pid(0x%x), UserParam(0x%x), filterLen(%d), ulRequestId (0x%x)"C_CODE_RESET"\n",
			usTsIdx, pstFilter->ucDemuxId, pstFilter->pid, ulUserParam, stFilterParam.eFilterLen, pstFilter->ulRequestId);
#endif
		pstFilter->control = ITK_ENABLE;

	}

	return ulRet;
}


HERROR local_itk_DEMUX_DisableFilter(ITK_FilterHandle_t* pstFilter)
{
	HERROR		ulErr = ERR_OK;

	if(pstFilter == NULL)
		return ERR_FAIL;

	/* we take care of the value Control in whose value is ITK_ENABLE */
	if(pstFilter->control == ITK_ENABLE)
	{
		pstFilter->control = ITK_DISABLE;
		if(pstFilter->ulRequestId != ITK_DEMUX_INVALID_REQUEST_ID)
		{
			HxLOG_Debug("ulRequestId (0x%x)\n", pstFilter->ulRequestId);
	#if ITK_DEMUX_USE_DSMCC_TASK
			ulErr = MHEG_SEF_DisableSectionFilter(pstFilter->ucDemuxId, pstFilter->ulRequestId);
			if(ulErr != ERR_OK)
			{
				HxLOG_Error("PAL_SEF_DisableSectionFilter fail");
				return ulErr;
			}
	#else
			/* currently using below one */
			MHEG_SEF_StopSecFilter(pstFilter->ulRequestId);
	#endif
			pstFilter->ulRequestId = ITK_DEMUX_INVALID_REQUEST_ID;
			ulErr = ERR_OK;
		}
		else
		{
			HxLOG_Debug(" WARNING : filter request Id in Invalid !!!\n");
			ulErr = ERR_FAIL;
		}
	}

	return ulErr;
}


void local_itk_DEMUX_ReleaseFilter(itk_demux_filter_t* thiz)
{
	ITK_SessionHandle_t*		pstSession = NULL;
	ITK_FilterHandle_t*		pstFilter = (ITK_FilterHandle_t*)thiz;

	if(pstFilter == NULL)
	{
		return;
	}

	VK_SEM_Get(s_ulSessionFilterSema);

	pstSession = (ITK_SessionHandle_t*)pstFilter->caller;

	/* disable filter */
	local_itk_DEMUX_DisableFilter(pstFilter);

	/* remove filter handler in list */
	local_itk_DEMUX_RemoveFilterFromList(pstFilter);

	/* decrese filter count in session handler */
	if(pstSession->ulFilterNum <= 0)
	{
		HxLOG_Error("Error : filter num already 0");
		return;
	}
	
	pstSession->ulFilterNum--;

	/* mem free */
	HLIB_STD_MemFree(pstFilter);
	pstFilter = NULL;

	VK_SEM_Release(s_ulSessionFilterSema);

}


HUINT32 local_itk_DEMUX_GetNewSITableId(void)
{
	static HUINT32 s_ulSITableId = ITK_DEMUX_SITABLE_ID_MAX;

	VK_SEM_Get(s_ulSiTblSema);

	do
	{
		if (s_ulSITableId >= ITK_DEMUX_SITABLE_ID_MAX)
			s_ulSITableId = ITK_DEMUX_SITABLE_ID_MIN;
		else
			s_ulSITableId++;

	} while (local_itk_DEMUX_GetSiTableHandler(s_ulSITableId) != NULL);

	VK_SEM_Release(s_ulSiTblSema);

	return s_ulSITableId;
}

ITK_SITableHandle_t* local_itk_DEMUX_GetSiTableHandler(HUINT32 ulSITableId)
{
	ITK_SITableHandle_t *pItemSITable = NULL;

	pItemSITable = s_pstSiHandleList;

	while (pItemSITable != NULL)
	{
		if (pItemSITable->ulSITableId == ulSITableId)
		{
			break;
		}

		pItemSITable = pItemSITable->pNext;
	}

	return pItemSITable;
}

#if defined(USE_VK_TIMER_TASK)
void local_itk_DEMUX_CbOnTimerSITable(unsigned long ulTimerId, void* pContext)
{
	HUINT32		ulTableId = 0;
	HERROR		ulErr = 0;
	Handle_t 	hAction = HANDLE_NULL;

	PARAM_UNUSED(ulTimerId);

	if(pContext)
	{
		HxSTD_memcpy(&ulTableId, pContext, sizeof(HUINT32));
		PORT_ITK_GetMastActionHandle(&hAction);
		BUS_PostMessage(NULL, eMEVT_MHEG_SITABLE_NOTIFY, hAction, MSG_ITK_SYSTEM_DMX_SI_ENABLE, ulTableId, 0);

		ItkFatal(ulErr == VK_OK);
	}
}


void local_itk_DEMUX_EnableSiTable(ITK_SITableHandle_t* pstSiTbl)
{
	HERROR		ulErr = 0;
	Handle_t 	hAction = HANDLE_NULL;

	if(!pstSiTbl)
	{
		ItkFatal(0);
		return;
	}

	if(pstSiTbl->control == ITK_DISABLE)
	{
		/* send notify si table */
		PORT_ITK_GetMastActionHandle(&hAction);
		BUS_PostMessage(NULL, eMEVT_MHEG_SITABLE_NOTIFY, hAction, MSG_ITK_SYSTEM_DMX_SI_ENABLE, pstSiTbl->ulSITableId, 0);

	#if SUPPORT_SI_CB_TIMER
		/* set timer to invoke send message */
		if(pstSiTbl->ulTimerId == 0)
		{
			ulErr = (HUINT32)VK_TIMER_EventEvery(ITK_DEMUX_SITABLE_TIME_OUT,
								local_itk_DEMUX_CbOnTimerSITable, (void*)&pstSiTbl->ulSITableId, sizeof(HUINT32), &pstSiTbl->ulTimerId);
		}
	#endif
	HxLOG_Debug("[MHEG SI TABLE] Enable SI Table(0x%x), current Control status(%d), timerId(%d) \n", pstSiTbl->ulSITableId, pstSiTbl->control, pstSiTbl->ulTimerId);

		ItkFatal(ulErr == VK_OK);

		/* send messag to ITK Main */
		pstSiTbl->control 	= ITK_ENABLE;

	}

}

void local_itk_DEMUX_DisableSiTable(ITK_SITableHandle_t* pstSiTbl)
{
	HERROR			ulErr = 0;

	ItkFatal(pstSiTbl != NULL);


	HxLOG_Debug("[MHEG SI TABLE] Disable SI Table(0x%x), Control (%d), timerId(%d) \n", pstSiTbl->ulSITableId, pstSiTbl->control, pstSiTbl->ulTimerId);
	if(pstSiTbl->control == ITK_ENABLE)
	{
	#if 0
		HMSG		stMsg;

		pstSiTbl->control 	= ITK_DISABLE;

		stMsg.usMsgClass	= MSG_ITK_CLASS_SYSTEM;
		stMsg.usParam		= MSG_ITK_SYSTEM_DMX_SI_DISABLE;
		stMsg.ulParam		= pstSiTbl->ulSITableId;

		PORT_ITK_SendMsg(&stMsg);
	#else
		pstSiTbl->control = ITK_DISABLE;
	#if SUPPORT_SI_CB_TIMER
		if(pstSiTbl->ulTimerId)
		{
			ulErr = (HERROR)VK_TIMER_Cancel(pstSiTbl->ulTimerId);
			ItkFatal(ulErr == VK_OK);

			pstSiTbl->ulTimerId = 0;
		}
	#endif

	#endif
	}

}
#else
// this is NOT timer Task...just monitor task....
static void		local_itk_DEMUX_MonitorTask(void *arg1)
{
	//HUINT32		ulCurrentTick, ulTickDiff;
	HINT32		i;
	HUINT32		ulViewId;

	NOT_USED_PARAM(arg1);

	/* Timer List ¸| AE±aE­ CN´U. */
	for (i = 0; i < ITK_MAX_TABLE_NUM; i++)
	{
		s_itkSiMonitorList[i].ulSiTableId = ITK_DEMUX_TABLE_ID_DISABLED;
		s_itkSiMonitorList[i].bControl   = FALSE;
	}

	while (1)
	{
		VK_SEM_Get(s_ulitkSiMonitorSema);

		PORT_ITK_GetMastViewId(&ulViewId);

		for (i = 0; i < ITK_MAX_TABLE_NUM; i++)
		{
			if(s_itkSiMonitorList[i].bControl == TRUE)
			{
				PORT_ITK_PostMsg(eMHEG_DEMUX_SITABLE_NOTIFY, ulViewId, MSG_ITK_SYSTEM_DMX_SI_ENABLE, s_itkSiMonitorList[i].ulSiTableId, 0, 0);
			}
		}

		VK_SEM_Release(s_ulitkSiMonitorSema);
		/* Task Delay */
		VK_TASK_Sleep(ITK_DEMUX_SITABLE_MONITOR_TIME);
	}

}


void local_itk_DEMUX_EnableSiTable(ITK_SITableHandle_t* pstSiTbl)
{
	//HERROR		ulErr = 0;
	HUINT32		i;
	HBOOL		bTableIdFound;

	if(pstSiTbl == NULL)
	{
		return;
	}

	HxLOG_Debug(" Enable SI Table, Control (%d), ulSITableId (%d), table_id(0x%x), table_id_extn(0x%x)\n", pstSiTbl->control, pstSiTbl->ulSITableId, pstSiTbl->table_id, pstSiTbl->table_id_extn);

	if(pstSiTbl->control == ITK_DISABLE)
	{
		VK_SEM_Get(s_ulitkSiMonitorSema);

		bTableIdFound = FALSE;
		for (i = 0; i < ITK_MAX_TABLE_NUM; i++)
		{
			if (s_itkSiMonitorList[i].bControl == TRUE)
			{
				if(s_itkSiMonitorList[i].ulSiTableId == pstSiTbl->ulSITableId)
				{
					bTableIdFound = TRUE;
					break;
				}
			}
		}

		if(bTableIdFound == FALSE)
		{
			for (i = 0; i < ITK_MAX_TABLE_NUM; i++)
			{
				if(s_itkSiMonitorList[i].bControl == FALSE)
				{
					s_itkSiMonitorList[i].bControl = TRUE;
					s_itkSiMonitorList[i].ulSiTableId = pstSiTbl->ulSITableId;
					break;
				}
			}
			if(i >= ITK_MAX_TABLE_NUM)
			{
				HxLOG_Error(" [ERROR] Can't assign slot for si table  !!!!\n");
			}
			else
			{
				HxLOG_Debug("\n\n(%d) s_itkSiMonitorList[i].bControl  : %d, s_itkSiMonitorList[i].ulSiTableId  : %d\n", i, s_itkSiMonitorList[i].bControl, s_itkSiMonitorList[i].ulSiTableId);
			}
		}

		VK_SEM_Release(s_ulitkSiMonitorSema);

		//HxLOG_Error(ulErr == VK_OK);

		/* send messag to ITK Main */
		pstSiTbl->control 	= ITK_ENABLE;

	}

}

void local_itk_DEMUX_DisableSiTable(ITK_SITableHandle_t* pstSiTbl)
{
	HUINT32			i;

	if(pstSiTbl == NULL)
		return;

	HxLOG_Debug(" Disable SI Table, current Control status(%d), SITableId (%d), table id(0x%x), extn(0x%x)\n", pstSiTbl->control, pstSiTbl->ulSITableId, pstSiTbl->table_id, pstSiTbl->table_id_extn);

	if(pstSiTbl->control == ITK_ENABLE)
	{
	#if 0
		HMSG		stMsg;

		pstSiTbl->control 	= ITK_DISABLE;

		stMsg.usMsgClass	= MSG_ITK_CLASS_SYSTEM;
		stMsg.usParam		= MSG_ITK_SYSTEM_DMX_SI_DISABLE;
		stMsg.ulParam		= pstSiTbl->ulSITableId;

		PORT_ITK_SendMsg(&stMsg);
	#else
		pstSiTbl->control = ITK_DISABLE;
		VK_SEM_Get(s_ulitkSiMonitorSema);

		for (i = 0; i < ITK_MAX_TABLE_NUM; i++)
		{
			if(s_itkSiMonitorList[i].bControl == TRUE && s_itkSiMonitorList[i].ulSiTableId == pstSiTbl->ulSITableId)
			{
				s_itkSiMonitorList[i].bControl = FALSE;
				s_itkSiMonitorList[i].ulSiTableId = ITK_DEMUX_TABLE_ID_DISABLED;
				break;
			}
		}

		VK_SEM_Release(s_ulitkSiMonitorSema);
	#endif
	}

}
#endif


void local_itk_DEMUX_ControlSiTable(struct itk_demux_filter_t *thiz, itk_control_t control)
{
	ITK_SITableHandle_t*	pstSiTbl = (ITK_SITableHandle_t*)thiz;

	HxLOG_Debug("  ==> %s) \n", control ? "ENABLE" : "DISABLE");
	if(pstSiTbl == NULL)
	{
		return;
	}

	//VK_SEM_Get(s_ulSiTblSema);
	switch(control)
	{
		case ITK_DISABLE:
			local_itk_DEMUX_DisableSiTable(pstSiTbl);
			break;

		case ITK_ENABLE:
			local_itk_DEMUX_EnableSiTable(pstSiTbl);
			break;

		default:
			HxLOG_Error("Error : unknown itk_control_t type");
			break;
	}

	//VK_SEM_Release(s_ulSiTblSema);


}

void local_itk_DEMUX_ReleaseSiTable(struct itk_demux_filter_t *thiz)
{
	ITK_SITableHandle_t*		pstSiTbl = (ITK_SITableHandle_t*)thiz;

	if(pstSiTbl == NULL)
	{
		return;
	}

	HxLOG_Debug(" Release Si TableId(0x%x), PID(0x%x) !!! \n", pstSiTbl->table_id, pstSiTbl->pid);

#if 0
	/* modiied below routine for check point #53 */
	stMsg.usMsgClass 	= MSG_ITK_CLASS_SYSTEM;
	stMsg.usParam		= MSG_ITK_SYSTEM_DMX_SI_RELEASE;
	stMsg.ulParam		= pstSiTbl->ulSITableId;

	PORT_ITK_SendMsg(&stMsg);

#else

	VK_SEM_Get(s_ulSiTblSema);

	local_itk_DEMUX_DisableSiTable(pstSiTbl);
	local_itk_DEMUX_RemoveSiTableFromList(pstSiTbl);
	HLIB_STD_MemFree(pstSiTbl);
	pstSiTbl = NULL;

	VK_SEM_Release(s_ulSiTblSema);

#endif

}

void local_itk_DEMUX_ResetAllFilter()
{
	ITK_FilterHandle_t*		pstFilter = NULL;
	HMSG					outDsmCCMsg;
	HERROR					hErr = ERR_OK;

#if 0//ITK_DEMUX_USE_DSMCC_TASK_2
	HMSG		stMsg;
	HUINT32		ulRet = 0;
	HUINT32		ulCurMsgCnt,ulMaxMsgCnt;
#endif
	HxLOG_Debug(" Reset All Si Filter (+)\n");

	HxSTD_memset(&outDsmCCMsg, 0, sizeof(HMSG));
	outDsmCCMsg.usMsgClass 	= MHEG_MSG_DSMCC_CLEAR;

	hErr = VK_MSG_SendTimeout(s_ulItkDmxRcvQId, (void*)&outDsmCCMsg, sizeof(HMSG), 0);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("Error from VK_MSG_SendTimeout()\n");
		return ;
	}

	pstFilter = s_pstFilterHeadList;
	while(pstFilter)
	{
		local_itk_DEMUX_DisableFilter(pstFilter);
		pstFilter = pstFilter->pNext;
	}

	s_pstFilterHeadList = NULL;
#if 0//ITK_DEMUX_USE_DSMCC_TASK_2
	ulRet = VK_MSG_GetMessageCount(s_ulItkDmxRcvQId, (HULONG *)&ulCurMsgCnt, (HULONG *)&ulMaxMsgCnt);

	if(ulRet == ERR_OK)
	{
		HxLOG_Debug("ulCurMsgCnt : %d, (HULONG *)&ulMaxMsgCnt : %d\n",ulCurMsgCnt,ulMaxMsgCnt);
		while(ulCurMsgCnt )
		{
			ulRet = VK_MSG_ReceiveTimeout(	s_ulItkDmxRcvQId, &stMsg, sizeof(HMSG),0);
			if(ulRet == ERR_OK)
			{
				HxLOG_Debug("^ ");
				HLIB_STD_MemFree((HUINT8 *)stMsg.unInfo.aulL[0]);
			}
			ulCurMsgCnt--;
		}
	}
#endif
	HxLOG_Debug(" Reset All Si Filter (-)\n");
}

void local_itk_DEMUX_ResetAllSiTable()
{
	ITK_SITableHandle_t*		pstSiTblList = NULL;

	HxLOG_Debug(" Reset All Si Table (+)\n");
	pstSiTblList = s_pstSiHandleList;
	while(pstSiTblList)
	{
		local_itk_DEMUX_DisableSiTable(pstSiTblList);
		pstSiTblList = pstSiTblList->pNext;
	}

	s_pstSiHandleList = NULL;
	HxLOG_Debug(" Reset All Si Table (-)\n");
}

#if ITK_DEMUX_USE_DSMCC_TASK
void local_itk_DEMUX_ProcessRecvSection(PalSef_ReceiveMessage_t* pstMsg)
{
	HUINT8*					pucSection = NULL;
	HUINT16					usSecSize;
	HUINT32					ulRequestId;
	HUINT8					ucDmxId;
	ITK_FilterHandle_t*		pstFilter = NULL;
	ITK_SessionHandle_t*	pstSession = NULL;

	if(pstMsg == NULL)
		return;

	/* get section information */
	pucSection 	= (HUINT8*)pstMsg->ulDataAddr;
	ulRequestId	= pstMsg->ulRequestId;
	ucDmxId		= pstMsg->ucDemuxId;
	usSecSize 	= GET_SECTION_LEN_FROM_RAWDATA(pucSection) + 3;
	if(usSecSize == 3 || usSecSize > 4096)
	{
		HxLOG_Debug(" invalid sectionLen(%d) \n",usSecSize);
		HLIB_STD_MemFree(pucSection);
		return;
	}

	HxLOG_Debug("@ : tableid(0x%x) SecSize(%d) \n",GET_TABLE_ID_FROM_RAWDATA(pucSection), usSecSize);
	VK_SEM_Get(s_ulSessionFilterSema);
	pstFilter = local_itk_DEMUX_GetFilterHandleByReqId(ulRequestId, ucDmxId);

	if((pstFilter == NULL) || (pstFilter->control == ITK_DISABLE))
	{
		HxLOG_Debug(" No filter exist, ulRequestId(0x%x), dmxId(%d) \n", ulRequestId, ucDmxId);
		VK_SEM_Release(s_ulSessionFilterSema);
		HLIB_STD_MemFree(pucSection);
		return;
	}

	if(pstFilter)
	{
		if(pstFilter->control == ITK_ENABLE)
		{
			pstSession = (ITK_SessionHandle_t*)pstFilter->caller;
			if(pstSession)
			{
				if(pstSession->callback)
				{
					//HxLOG_Debug("@ : tableid(0x%x) SecSize(%d) \n",GET_TABLE_ID_FROM_RAWDATA(pucSection), usSecSize);
					pstSession->callback(pstSession->callback_context, (itk_demux_session_t*)pstSession, pucSection, usSecSize);
				}
			}
		}
	}
	else
		HxLOG_Debug(" fail to get filter handle by dmxId(%d), requestId(0x%x) \n",ucDmxId, ulRequestId);

	VK_SEM_Release(s_ulSessionFilterSema);
	ITK_MEM_SI_FREE(pucSection);

}


void local_itk_DEMUX_RcvTask(void* pArg)
{
	PalSef_ReceiveMessage_t	stMsg;
	HUINT32				ulRet = 0;
	HUINT32				ulCurMsgCnt,ulMaxMsgCnt;

	while(1)
	{
		ulRet = VK_MSG_Receive(	s_ulItkDmxRcvQId, &stMsg, sizeof(PalSef_ReceiveMessage_t));
		if(ulRet != VK_OK)
		{
			VK_TASK_Sleep(3);
			continue;
		}

		ulRet = VK_MSG_GetMessageCount(s_ulItkDmxRcvQId, (HULONG *)&ulCurMsgCnt, (HULONG *)&ulMaxMsgCnt);

		if((((ulMaxMsgCnt /2) - 10)) < ulCurMsgCnt) // demux에서 반이상 넘는 겨우 stop 해줌. 우리 는 MHEG이 enable/disable 관리하기 때문에  emergency경우를 만들어 주기가 까다롭다
		{
			HxLOG_Debug("[local_itk_DEMUX_RcvTask()] ulCurMsgCnt : %d, (HULONG *)&ulMaxMsgCnt : %d\n",ulCurMsgCnt,ulMaxMsgCnt);
			HLIB_STD_MemFree((HUINT8 *)stRcvMsg.ulDataAddr);
		}
		else
			local_itk_DEMUX_ProcessRecvSection(&stMsg);
	}
}
#elif ITK_DEMUX_USE_DSMCC_TASK_2
void local_itk_DEMUX_ProcessRecvSection(HMSG* pstMsg)
{
	HUINT8*					pucSection = NULL;
	HUINT16					usRawLen;
	HUINT16					usFilterHandlerId;
	HUINT8					ucDmxId;
	ITK_SessionHandle_t*	pstSession = NULL;
	ITK_FilterHandle_t*		pstFilterHandler = NULL;
 
	if(pstMsg == NULL)
		return;

	/* get section information */
	pucSection 	= (HUINT8*)pstMsg->ulParam;
	usFilterHandlerId = pstMsg->usParam;
	ucDmxId		= (HUINT8)pstMsg->unInfo.ausS[0];
	usRawLen	= pstMsg->unInfo.ausS[1];

	VK_SEM_Get(s_ulSessionFilterSema);

	pstFilterHandler = local_itk_DEMUX_GetFilterHandlerByHandlerId(usFilterHandlerId, ucDmxId);

	if((pstFilterHandler == NULL) || (pstFilterHandler->control == ITK_DISABLE))
	{
		HxLOG_Error(" No filter exist, filterHandlerId(0x%x), dmxId(%d) \n", usFilterHandlerId, ucDmxId);
		VK_SEM_Release(s_ulSessionFilterSema);
		if(pucSection)		HLIB_STD_MemFree(pucSection);
		return;
	}

	pstSession = (ITK_SessionHandle_t*)pstFilterHandler->caller;
	if(pstSession != NULL)
	{
		if(pstSession->callback && usRawLen && pucSection)
		{
			HxLOG_Debug(" callback %x: tableId(0x%x) usRawLen(%d), dmxId(%d)  \n ",(HUINT32)pstSession->callback, pucSection[0], usRawLen, ucDmxId);
			pstSession->callback(pstSession->callback_context, (itk_demux_session_t*)pstSession, pucSection, usRawLen);
		}
		else
		{
			HxLOG_Error(" No filter exist, filterHandlerId(0x%x), dmxId(%d) \n", usFilterHandlerId, ucDmxId);
			VK_SEM_Release(s_ulSessionFilterSema);
			HLIB_STD_MemFree(pucSection);
			return;
		}
	}
	else
		HxLOG_Debug(" Can not invoke callback func : control status=%d \n",pstFilterHandler->control);

	VK_SEM_Release(s_ulSessionFilterSema);
	if(pucSection)		HLIB_STD_MemFree(pucSection);
}


void local_itk_DEMUX_RcvTask(void* pArg)
{
	HMSG		stMsg;
	HUINT32		ulRet = 0;
	HUINT32		ulCurMsgCnt,ulMaxMsgCnt;

	while(1)
	{
		ulRet = VK_MSG_Receive(	s_ulItkDmxRcvQId, &stMsg, sizeof(HMSG));
		if(ulRet != VK_OK)
		{
			VK_TASK_Sleep(3);
			continue;
		}

		switch(stMsg.usMsgClass)
		{
			case MHEG_MSG_DSMCC_DATA:
				ulRet = VK_MSG_GetMessageCount(s_ulItkDmxRcvQId, (HULONG *)&ulCurMsgCnt, (HULONG *)&ulMaxMsgCnt);

				if((((ulMaxMsgCnt /2) - 10)) < ulCurMsgCnt)
				{
					//HxLOG_Debug("ulCurMsgCnt : %d, (HULONG *)&ulMaxMsgCnt : %d\n",ulCurMsgCnt,ulMaxMsgCnt);
					//MHEGDmxMallocCnt--;
					HLIB_STD_MemFree((HUINT8 *)stMsg.ulParam);
				}
				else
				{
					local_itk_DEMUX_ProcessRecvSection(&stMsg);
				}
				break;

			case MHEG_MSG_DSMCC_CLEAR:
				ulRet = VK_MSG_GetMessageCount(s_ulItkDmxRcvQId, (HULONG *)&ulCurMsgCnt, (HULONG *)&ulMaxMsgCnt);
				while(ulCurMsgCnt)
				{
					ulRet = VK_MSG_ReceiveTimeout(	s_ulItkDmxRcvQId, &stMsg, sizeof(HMSG),0);
					/* stop mheg application이 두번 호출 되는 경우 MHEG_MSG_DSMCC_CLEAR 메세지가 두번 날라오게 되는데
					    이런 경우 stMsg.ulParam 값이 없어서 crash 발생함 (아래 루틴의 목적은 message queue에 남은 dsmcc data를 정리하는 것)
					      -> msg class가 MHEG_MSG_DSMCC_DATA 인 경우에만 memory 정리하도록 수정함 (yjlim)*/
					if( (ulRet == VK_OK) && (stMsg.usMsgClass == MHEG_MSG_DSMCC_DATA) )
					{
						HLIB_STD_MemFree((HUINT8 *)stMsg.ulParam);
					}
					ulCurMsgCnt--;
				}
				break;

			default:
				break;
		}

	}
}

#endif


#define _____PUBLIC_APIs________________________________________________________________

void PORT_ITK_DEMUX_Init()
{
	HUINT32		ulError = 0;

	s_pstFilterHeadList 		= NULL;

	s_pstSiHandleList			= NULL;
	s_usFilterHandleIdGenerator	= 1;

	//s_usDebugCnt				= 0;

	ulError = VK_SEM_Create(&s_ulSessionFilterSema, "ITK_DX_SEM", VK_SUSPENDTYPE_PRIORITY);
	if(ulError != VK_OK)
	{
		HxLOG_Error("Can not create s_ulSessionFilterSema");
		return;
	}

	ulError = VK_SEM_Create(&s_ulSiTblSema, "ITK_SI_SEM", VK_SUSPENDTYPE_PRIORITY);
	if(ulError != VK_OK)
	{
		HxLOG_Error("Can not create s_ulSiTblSema!");
		return;
	}

#if !defined(USE_VK_TIMER_TASK)
	ulError = VK_SEM_Create(&s_ulitkSiMonitorSema, "ITK_SI_Monitor_SEM", VK_SUSPENDTYPE_PRIORITY);
	if(ulError != VK_OK)
	{
		HxLOG_Error("Can not create s_ulitkSiMonitorSema!");
		return;
	}
#endif

#if (ITK_DEMUX_USE_DSMCC_TASK || ITK_DEMUX_USE_DSMCC_TASK_2)
	/* create message queue */
	ulError = (HUINT32)VK_MSG_Create(1024, sizeof(MhegSef_ReceiveMessage_t), "ItkRcvQ", &s_ulItkDmxRcvQId, VK_SUSPENDTYPE_FIFO);
	if(ulError != ERR_OK)
	{
		HxLOG_Error(" Can not create RCV message Queue\n");
		return;
	}

	/* create task for main porting layer */
	ulError = (HUINT32)VK_TASK_Create(local_itk_DEMUX_RcvTask, MHEG_RCV_TASK_PRIORITY, MHEG_RCV_TASK_STACK_SIZE, "tMwItkRcv", NULL, &s_ulItkDmxRcvTaskId, 0);
	if(ulError != ERR_OK)
	{
		HxLOG_Error(" Can not create new RCV Task...\n");
		return;
	}

	/* start ITK Task */
	ulError = VK_TASK_Start(s_ulItkDmxRcvTaskId);
	if(ulError != ERR_OK)
	{
		HxLOG_Error(" Can not start ITK RCV task !!\n");
		return;
	}
#endif

#if !defined(USE_VK_TIMER_TASK)
	/* create task for main porting layer */
	if((ulError = (HUINT32)VK_TASK_Create(local_itk_DEMUX_MonitorTask, MHEG_SI_MONITOR_TASK_PRIORITY, MHEG_SI_TASK_STACK_SIZE, "tMwItkSiM", NULL, &s_ulItkSiMonitorTaskId, 0)) != ERR_OK)
	{
		HxLOG_Error(" Can not create new SI monitor Task...\n");
		return;
	}

	/* start ITK Task */
	if((ulError = VK_TASK_Start(s_ulItkSiMonitorTaskId)) != ERR_OK)
	{
		HxLOG_Error(" Can not start ITK SI monitor task !!\n");
		return;
	}
#endif

}


void PORT_ITK_DEMUX_RegisterObject()
{
	ITK_registerManager(ITK_DEMUX, &s_stDemuxObject);
}

void PORT_ITK_DEMUX_CreateObject()
{
	s_stDemuxObject.newSession = PORT_ITK_DEMUX_NewSession;
	s_stDemuxObject.newSITable = PORT_ITK_DEMUX_NewSiTable;
	s_bIsObjectAvailable = TRUE;
	PORT_ITK_DEMUX_RegisterObject();
}


itk_demux_filter_t* PORT_ITK_DEMUX_NewSiTable(struct itk_demux_t* thiz, itk_uint16_t onid, itk_uint16_t tsid, itk_uint16_t pid,
										itk_uint8_t table_id, itk_uint16_t table_id_extn, itk_demux_si_callback_t callback, void *callback_context)
{
	ITK_SITableHandle_t*		pstSiTable = NULL;
	//HERROR						ulErr = 0;
	//Handle_t					hAction;
	//HUINT32						ulMastActionId = 0;
	HUINT32						ulDmxId = 0;
	HUINT32						ulViewId;

	HxLOG_Debug(" onid(0x%x), tsid(0x%x), pid(0x%x) table_id(0x%x), table_id_extn(0x%x), callback_context(%p) \n", onid, tsid, pid, table_id, table_id_extn, callback_context);
	
	/* for test
	if(s_usDebugCnt >= 2)
		return NULL;*/

	/* check validation of parameters */
	if(thiz == NULL || callback == NULL || s_bIsObjectAvailable == FALSE)
	{
		return NULL;
	}

	/* get Action handle from MHEG context */
	//PORT_ITK_GetMastActionId(&ulMastActionId);
	//PORT_ITK_GetMastActionHandle(&hAction);

	PORT_ITK_GetMastViewId(&ulViewId);
	
	//if(SVC_PIPE_IsActionHandleLatest(hAction) != ERR_OK)
	//{
	//	HxLOG_Error(" Action handle is Older than one in RM Manager !!! \n");
	//	return NULL;
	//}

	
	/* get demux resource id */
		//ulErr = SVC_PIPE_GetResourceId(ulMastActionId, eRxRSCTYPE_DEMUX, &ulDmxId);
		//if(ulErr != ERR_OK)
		//{
		//	return NULL;
		//}

	// need to get demux id by view id
	ulDmxId = PORT_ITK_GetDemuxId(ulViewId);

	/* allocate new SITable handler */
	pstSiTable = (ITK_SITableHandle_t*)HLIB_STD_MemAlloc(sizeof(ITK_SITableHandle_t));
	if(pstSiTable == NULL)
		return NULL;
	pstSiTable->vtbl.control	= local_itk_DEMUX_ControlSiTable;
	pstSiTable->vtbl.release	= local_itk_DEMUX_ReleaseSiTable;

	pstSiTable->control			= ITK_DISABLE;			/* initial value */
	pstSiTable->callback		= callback;
	pstSiTable->callback_context = callback_context;
	pstSiTable->onid			= onid;
	pstSiTable->pid				= pid;
	pstSiTable->table_id		= table_id;
	pstSiTable->table_id_extn	= table_id_extn;		/* 4th and 5th bytes of section data */
	pstSiTable->tsid			= tsid;
	pstSiTable->ucDemuxId		= (HUINT8)ulDmxId;
	pstSiTable->ulTimerId		= 0;					/* initial value --> DISABLE */
	pstSiTable->ulSITableId		= local_itk_DEMUX_GetNewSITableId();
	pstSiTable->pNext			= NULL;
	//pstSiTable->pPrev			= NULL;

	VK_SEM_Get(s_ulSiTblSema);
	local_itk_DEMUX_AppendSiTableToList(pstSiTable);
	VK_SEM_Release(s_ulSiTblSema);

	//HxLOG_Debug(" Success New SI Table Object \n");
	//s_usDebugCnt++;

	return (itk_demux_filter_t*)pstSiTable;

}




itk_demux_session_t* PORT_ITK_DEMUX_NewSession(struct itk_demux_t* thiz, itk_uint16_t onid, itk_uint16_t tsid,
														itk_demux_session_callback_t callback, void* callbackContext)
{
	ITK_SessionHandle_t*		pstSession = NULL;
	//Handle_t					hAction = HANDLE_NULL;
	HUINT32						ulViewId;

	/* check validation of input parameter */
	if(thiz == NULL || callback == NULL || s_bIsObjectAvailable == FALSE)
	{
		return NULL;
	}

	/* check validation of current Action handle */
	//PORT_ITK_GetMastActionHandle(&hAction);
	//if(SVC_PIPE_IsActionHandleLatest(hAction) != ERR_OK)
	//{
	//	HxLOG_Debug(" NewSession Call is Old, new Svc Change has been doing ~~\n");
	//	return NULL;
	//}

	PORT_ITK_GetMastViewId(&ulViewId);

	/* creates new session handler */
	pstSession = (ITK_SessionHandle_t*)HLIB_STD_MemAlloc(sizeof(ITK_SessionHandle_t));
	if(pstSession == NULL)
	{
		return NULL;
	}

	/* set session methods */
	pstSession->vtbl.release		= local_itk_DEMUX_ReleaseSession;
	pstSession->vtbl.getFilter		= local_itk_DEMUX_GetFilter;

	/* set session attributes */
	pstSession->callback			= callback;
	pstSession->callback_context	= callbackContext;
	pstSession->onid				= onid;
	pstSession->tsid				= tsid;
	pstSession->ulFilterNum			= 0;			/* inital number of filter */
	//pstSession->hAction				= hAction;		/* register current action handle */
	pstSession->ulViewId			= ulViewId;

	HxLOG_Debug(" new Session : ulViewId(%d) onid(0x%x), tsid(0x%x) context(%p), callback(%p)\n", ulViewId, onid, tsid, callbackContext, callback);
	
	return (itk_demux_session_t*)pstSession;

}




HERROR PORT_ITK_DEMUX_ProcessFilter(HUINT8 ucType, HUINT32 ulCmdType, HUINT32 ulTableId)
{
	HERROR					ulErr = 0;
	HUINT8*					pucSecBuffer = NULL;
	ITK_SITableHandle_t*	pstSiTbl = NULL;

	if(s_bIsObjectAvailable == FALSE)
		return ERR_FAIL;

	/* get mast action handle from context */

	HxLOG_Debug(" ucType(0x%x) \n", ucType);
	switch(ucType)
	{
		case ITK_DEMUX_SESSION_NOTIFY:
			break;

		case ITK_DEMUX_SITABLE_NOTIFY:
			{
				HINT32			nDefSvcUid, nCurSvcUid;
				HUINT16			usSecLen;
				HUINT16			usTsIdx = 0;
				DxService_t		*pstSvcInfo;
				
				/* get SI TABLE Handler address */
				HxLOG_Debug(" Request SiTableHandler Id (%d) \n", ulTableId );
				VK_SEM_Get(s_ulSiTblSema);
				pstSiTbl = local_itk_DEMUX_GetSiTableHandler(ulTableId);
				if(pstSiTbl == NULL)
				{
					HxLOG_Error("TableId (0x%x)\n", ulTableId);
					VK_SEM_Release(s_ulSiTblSema);
					return ERR_FAIL;
				}

				if(ulCmdType == MSG_ITK_SYSTEM_DMX_SI_ENABLE)
				{
					/* get Si Raw Data matching with pid, tsid, table_id and table_id_ext */
					//DB_SVC_GetUniqueIdFromSvcHandle(hDefSvc, &usUniqId);
					PORT_ITK_GetSvcUid(&nDefSvcUid, &nCurSvcUid);
					pstSvcInfo = APK_META_SVC_GetService(nDefSvcUid);
					if (pstSvcInfo == NULL)
					{
						VK_SEM_Release(s_ulSiTblSema);
						return ERR_FAIL;
					}
					usTsIdx = pstSvcInfo->tsuid;
					APK_META_SVC_Delete(pstSvcInfo);
					
					ulErr = MHEG_SI_GetFromMHEGRawData(usTsIdx, pstSiTbl->pid, pstSiTbl->table_id, pstSiTbl->table_id_extn, &usSecLen, &pucSecBuffer);
					if(ulErr != ERR_OK)
					{
						HxLOG_Error("[ERROR] : MHEG_SI_GetFromMHEGRawData (0x%x) hDefSvc/hCurSvc (0x%x/0x%x) \n", ulErr, nDefSvcUid, nCurSvcUid);
					}

					if(pucSecBuffer != NULL && usSecLen != 0)
					{
						if(pstSiTbl->control == ITK_ENABLE)
						{
							if(pstSiTbl->callback)
							{
								HxLOG_Debug("Callback SI Table(usUniqId:%d, pid:0x%x, id:0x%x,extn Id:0x%x), RawData(%p), Len(%d) callbackContext(%p) \n", usTsIdx, pstSiTbl->pid, pstSiTbl->table_id, pstSiTbl->table_id_extn, pucSecBuffer,usSecLen, pstSiTbl->callback_context);
								pstSiTbl->callback(pstSiTbl->callback_context, (itk_demux_filter_t*)pstSiTbl, pucSecBuffer, (size_t)usSecLen);
								HxLOG_Debug("After Callback\n");
							}
						}
						ulErr = ERR_OK;
					}
					else
					{
						ulErr = ERR_FAIL;
						HxLOG_Error(" fail to get Si Raw Data , uniqId(%d) pid(0x%x), table_id(0x%x), extId(0x%x) \n", usTsIdx, pstSiTbl->pid, pstSiTbl->table_id, pstSiTbl->table_id_extn);
					}

					if (pucSecBuffer)
					{
						HLIB_STD_MemFree(pucSecBuffer);
						pucSecBuffer = NULL;
					}
				}
				VK_SEM_Release(s_ulSiTblSema);

			}
			break;

		default:
			HxLOG_Error("fatal error");
			ulErr = ERR_FAIL;
			break;
	}

	return ulErr;
}

#if 0 // unused function
/*
   REMARK : SI task에서 PMT를 받거나 PMT가 변경이 되면, EVT_SI_PMT를 WatchTV로 보내게 되고,
            MHEG Manager를 이를 받아서 PMT가 Received된것을 ITK에 알린다. 이 함수에 의해서
            ITK는 PMT Raw Data를 관리하게 됨. PMT를 받으면 PAT 데이타도 같이 저장 관리하고,
            PMT, PAT 데이터 리스트는 Redkey가 다시 구동되지 않으면 지속적으로 관리 된다.
            이 작업은 MHEG Manager task(GWM)에 의해서 이루어 진다. */
HERROR PORT_ITK_DEMUX_NotifyTableReceived(Handle_t hAction,SiTableType_e eSiTableType)
{
	Handle_t				hActionMheg, hDefSvc, hCurSvc;
	HERROR					ulRet = 0;
	HUINT16 				usTsUniqId;
	HUINT16 				usSvcId;
	HUINT16					usSecIndex;
	HUINT32 				ulSecCnt = 0;
	HUINT8*					pucRawData = NULL, *pTmpRaw = NULL;

	if(s_bIsObjectAvailable == FALSE)
		return ERR_FAIL;

	PORT_ITK_GetMastActionHandle(&hActionMheg);
	HxLOG_Debug(" notify PMT / PAT Received !!! hAction(0x%x) \n",hAction);

	if(hActionMheg != hAction)
	{
		HxLOG_Debug(" different Action handle(0x%x : 0x%x ) \n",hActionMheg, hAction);
		return ERR_FAIL;
	}

	PORT_ITK_GetSvcUid(&hDefSvc, &hCurSvc);
	usTsUniqId = SVC_SI_UTIL_GetTsUniqIdFromSvcHandle(hDefSvc);
	usSvcId= SVC_SI_UTIL_GetSvcIdFromSvcHandle(hDefSvc);

	ulRet = MW_SI_CountRawSection(usTsUniqId, eSiTableType,((eSiTableType == eSI_TableType_PAT)?FALSE:TRUE), usSvcId, &ulSecCnt);
	HxLOG_Debug("ulErr = %d, usSecCnt = %d\n", ulRet, ulSecCnt);
	if (ulRet != ERR_OK)
	{
		HxLOG_Error("[MW_SI_CountRawSection] Error (0x%x) \n", ulRet);
		return ERR_FAIL;
	}

	for(usSecIndex = 0; usSecIndex < ulSecCnt; usSecIndex++)
	{
		ulRet = MW_SI_GetRawSection(usTsUniqId, eSiTableType, ((eSiTableType == eSI_TableType_PAT)?FALSE:TRUE), usSvcId, usSecIndex, &pTmpRaw);
		if (ulRet == ERR_OK)
		{
			pucRawData = pTmpRaw;
			break;
		}
	}

	if (pucRawData && ulRet == ERR_OK)
	{
		XSVC_SI_MHEG_AddCurTableFromBaseSI(hAction, pucRawData, hDefSvc,eSiTableType);
	}

	return ulRet;
}
#endif
void PORT_ITK_DEMUX_Restore(itk_service_tune_flag_t eTuneFlag)
{
#if !defined(USE_VK_TIMER_TASK)
	HUINT32	i;
#endif

	if(s_bIsObjectAvailable == FALSE)
		return;

	if (eTuneFlag & ITK_TUNE_KEEP_CONTEXT) // quite tune 일 경우 redkey 가 reset 하도록 함
	{
		return;
	}

#if !defined(USE_VK_TIMER_TASK)
	VK_SEM_Get(s_ulitkSiMonitorSema);
	/* Timer List 를 초기화 한다. */
	for (i = 0; i < ITK_MAX_TABLE_NUM; i++)
	{
		s_itkSiMonitorList[i].ulSiTableId = ITK_DEMUX_TABLE_ID_DISABLED;
		s_itkSiMonitorList[i].bControl = FALSE;

	}
	VK_SEM_Release(s_ulitkSiMonitorSema);
#endif

	VK_SEM_Get(s_ulSessionFilterSema);
	local_itk_DEMUX_ResetAllFilter();
	VK_SEM_Release(s_ulSessionFilterSema);

	VK_SEM_Get(s_ulSiTblSema);
	local_itk_DEMUX_ResetAllSiTable();
	VK_SEM_Release(s_ulSiTblSema);

	HxLOG_Debug("Reset demux buffer allocated before \n");
}


#define _____DEBUG_APIs________________________________________________________________

#ifdef CONFIG_DEBUG
HINT32 PORT_ITK_DEMUX_DEBUG_ShowRawTbInfo(void *pvArg)
{
	ITK_SiRawDataList_t*	pstRawList = NULL;

	if(s_bIsObjectAvailable == FALSE)
		return 0;

	VK_SEM_Get(s_ulSiTblSema);

	pstRawList = s_pstSiRawList;
	HLIB_CMD_Printf(" =============== Show Raw Tbl List ================\n");
	while(pstRawList)
	{
		HLIB_CMD_Printf("UniqId(%d) Table Id(0x%x), Pid(0x%x), ExtTableId(0x%x)  RawDataLen(%d), SectionPointer(%p) \n",pstRawList->uniqeId,
						pstRawList->tableId, pstRawList->pid, pstRawList->tableIdExtn, pstRawList->savedRawDataSize, pstRawList->savedRawData);

		pstRawList = pstRawList->pNext;
	}

	VK_SEM_Release(s_ulSiTblSema);
	return 0;
}

HINT32 PORT_ITK_DEMUX_DEBUG_GetXXX(void *pvArg)
{
#if 1
	/* X Test for sectiio filtering */

	/* in case filteroptionvar is 0 :
				mask(0xFF)->fail, mask(0x30)-->almost fail, but can retrieve table id 0x3b very slowly
				mast(0x3F)->fail

		in case filteroptionvar is 1:
				mask(0x30)->
	*/
	HUINT8			aFilter[16] = {0x3B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	HUINT8			aMask[16] = {0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	HUINT8			mode = 0x08/*SITABLE_CONTINUE*/;
	HUINT32			ulRequestId = 0x1010BABA;
	HERROR			ulErr = ERR_OK;

	#if 1
		ulErr = MHEG_SEF_EnableSectionFilter(0, 0x04BB, s_ulItkDmxRcvQId, 0, 0, 0,
									1, aFilter, aMask, &mode, &ulRequestId, FALSE);

		if(ulErr != ERR_OK)
		{
			HLIB_CMD_Printf("[DEBUG] Fail to enable section filter !!!! \n");
		}
		else
			HLIB_CMD_Printf("[DEBUG] Success filtering !!!! \n");

	#else
		MHEG_SEF_SiMakeDefaultFilterParam(&stFilterParam,
        								  0x00, 0, 1, 0x04BB,
        								  0, 0,  					/* do not use table filter and ext table id */
        								  0,
        								  ITK_SECTION_CRC_FLAG, 	/* check CRC */
        								  0x0000, 0x00,				/* we're using Itk own filtering depth */
        								  eSiFilterMode_Continue,
        								  0,						/* if timeout is 0, no timeout. original value is ITK_SCTION_TIMEOUT_VALUE */
        								  0,
        								  local_itk_DEMUX_ParseDsmccSection,
        								  NULL);					/* don't have to use free callback */

		stFilterParam.eFilterLen = eSefFilterLen_8Bytes;

		/* set filtering depth manually */
		HxSTD_memcpy(stFilterParam.pFilterValue, aFilter, MHEG_SEF_MAX_FILTER_LEN);
		HxSTD_memcpy(stFilterParam.pFilterMask, aMask, MHEG_SEF_MAX_FILTER_LEN);

		HxLOG_Debug(" uniqId(%d) demux(%d), pid(0x%x), UserParam(0x%x), filterLen(%d) \n",1, 0, 0x04B5, 0, stFilterParam.eFilterLen);
		ulRequestId = MHEG_SEF_StartSecFilter(&stFilterParam);

		HxLOG_Debug(" Filtering : request Id (0x%x) \n", ulRequestId);
	#endif

#endif

#if 0
	HUINT16			usLen = 0;
	HUINT32			ulCnt = 0, index = 0;
	HUINT8*			pSection = NULL;
	PatProgInfo_t	programInfo;

	local_itk_DEMUX_GetSiRawData(2, 0x00, 0x00, 0x800, &usLen, &pSection);
	MWC_SI_CountPatProgramFromRaw(pSection, &ulCnt);

	HxLOG_Debug("==============Show Total program Count================\n");
	HxLOG_Debug(" sectionLen(%d),  Program count(%d)\n",usLen, ulCnt);

	for(index = 0; index < ulCnt; index++)
	{
		MWC_SI_GetPatProgramInfoFromRaw(pSection, index, &programInfo);
		HxLOG_Debug("[%2d] program_num(0x%x), PmtPid(0x%x) \n",index, programInfo.usProgNum, programInfo.usProgMapPid);
	}

	HLIB_STD_MemFree(pSection);
#endif
	return 0;
}

HINT32 PORT_ITK_DEMUX_DEBUG_ShowSiFilterList(void *pvArg)
{
	ITK_SITableHandle_t*		pstSiTblList = NULL;
	ITK_FilterHandle_t*			pstList = NULL;

	if(s_bIsObjectAvailable == FALSE)
		return 0;

	VK_SEM_Get(s_ulSiTblSema);
	pstSiTblList = s_pstSiHandleList;

	HLIB_CMD_Printf("============== Show Si Filter List Info ================\n");
	while(pstSiTblList)
	{
		HLIB_CMD_Printf("[%2d] timerId(%ld) control(%d), pid(0x%x), tableId(0x%x) tableExtid(0x%x) onid(0x%x), tsid(0x%x) \n", pstSiTblList->ulSITableId, pstSiTblList->ulTimerId,
					pstSiTblList->control, pstSiTblList->pid, pstSiTblList->table_id, pstSiTblList->table_id_extn, pstSiTblList->onid, pstSiTblList->tsid);

		pstSiTblList = pstSiTblList->pNext;
	}

	VK_SEM_Release(s_ulSiTblSema);

	HLIB_CMD_Printf("============== Show Si Session Filter List Info ================\n");
	VK_SEM_Get(s_ulSessionFilterSema);
	pstList = s_pstFilterHeadList;
	while(pstList)
	{
		HLIB_CMD_Printf("[%2d] control(%d) requestId(0x%x) pid(0x%x)  depth(%d) \n", pstList->filterHandle, pstList->control, pstList->ulRequestId, pstList->pid, pstList->depth);
		pstList = pstList->pNext;
	}

	VK_SEM_Release(s_ulSessionFilterSema);

	return 0;
}

HINT32 PORT_ITK_DEMUX_DEBUG_ShowSiMonitorList(void *pvArg)
{
#if !defined (USE_VK_TIMER_TASK)
	int i;

	VK_SEM_Get(s_ulitkSiMonitorSema);
	/* Timer List 를 초기화 한다. */
	for (i = 0; i < ITK_MAX_TABLE_NUM; i++)
	{
		HLIB_CMD_Printf("===================== s_itkSiMonitorList =====================\n");
		HLIB_CMD_Printf("Index [%d]\n", i);
		HLIB_CMD_Printf("Table Id [%d]\n", s_itkSiMonitorList[i].ulSiTableId);
		HLIB_CMD_Printf("Control [%d]\n", s_itkSiMonitorList[i].bControl);
	}
	VK_SEM_Release(s_ulitkSiMonitorSema);
#endif
	return 0;
}
#endif

