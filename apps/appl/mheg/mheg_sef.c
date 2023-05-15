/**
	@file     mheg_sef.c
	@brief    demux control module for mheg(section filter part)

	Description: abstraction layer for section filtering \n
				 pid channel (audio/video/pcr) and pes filter (subtitle/teletext/etc.) will be removed in this module. \n
				 Those functions are belongs to pal_video / pal_audio / pal_subtitle. \n
				 Each module to use di_demux calls DI directly. \n
				 And pal_channel and pal_ci will be in charge of demux routing function.
	Module     : MHEG DEMUX \n
	Remarks    : mheg							\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

// last revision of pal_sef.c : 108371

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <vkernel.h>
#include <hlib.h>
#include <silib.h>


#include <di_err.h>
#include <di_demux_ext.h>

#include <mheg_sef.h>
#include <mheg_si.h>
#include "_mheg_sef.h"


#define	CONFIG_USE_SEM_IN_ONSECTION

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/



#ifdef CONFIG_DEBUG
//#define _MHEG_DMX_SEM_DEBUG_
#endif
#ifdef _MHEG_DMX_SEM_DEBUG_
#define	mhegSef_ENTER_CRITICAL		{ HxLOG_Print(C_CODE_F_CYAN), HxLOG_Print("\t++++ mhegSef_ENTER_CRITICAL(%s:%d)"C_CODE_RESET"\n", __FUNCTION__, __LINE__); VK_SEM_Get(s_stSefContainer.ulRequestIdSem);}
#define	mhegSef_LEAVE_CRITICAL		{ VK_SEM_Release(s_stSefContainer.ulRequestIdSem); HxLOG_Print(C_CODE_F_CYAN), HxLOG_Print("\t---- mhegSef_LEAVE_CRITICAL(%s:%d)"C_CODE_RESET"\n", __FUNCTION__, __LINE__);}
#else
#define mhegSef_ENTER_CRITICAL		VK_SEM_Get(s_stSefContainer.ulRequestIdSem)
#define mhegSef_LEAVE_CRITICAL		VK_SEM_Release(s_stSefContainer.ulRequestIdSem)
#endif

#define mhegSef_INVALID_REQUEST      		0xffff
#define mhegSef_MAX_SEF_SENDING_MSG_Q_NUM			16		/* 시스템에서 사용 가능한 최대 sending msg queue 개수 - request id의 상위 16비트를 활용 */
#define mhegSef_MAKE_REQUESTID_WITH_QID(ulReqId, nOrderIdx)			{ ulReqId |= (0x80000000 >> nOrderIdx); }
#define mhegSef_MAKE_ORDERIDX_FROM_REQUESTID(nOrderIdx, ulReqId)	\
{                                                                       \
	HUINT32		ulTempReqId = (ulReqId & 0xffff0000);                   \
                                                                        \
	nOrderIdx = 0;                                                      \
	ulTempReqId = ulTempReqId << 1;                                     \
	while(0 != ulTempReqId)                                             \
	{                                                                   \
		ulTempReqId = ulTempReqId << 1;                                 \
		nOrderIdx++;                                                    \
                                                                        \
		if(nOrderIdx >= (mhegSef_MAX_SEF_SENDING_MSG_Q_NUM-1))                  \
			break;                                                      \
	}                                                                   \
}

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct tagSefDmxRequest
{
	HBOOL				 bUseFlag;

	HUINT32				 ulRequestId;
	HUINT16				 usPid;
	HUINT8 				 ucTableId;
	HUINT8				 ucReserved_1;

	/* msg queue 정보 */
	HUINT32				 ulEmergencyStopMsgQid;	/* 사용 시 0이 아닌 값으로 지정 */
	HBOOL				 bEmergencyMsgSended;

	/* section package 저장소 */
	HINT32				 nSecCntInPackage;
	HUINT32				 nStartTickInPackage;
	HUINT32				 nDurationInPackage;
	MhegSef_RawData_t	*pstRawPackage;
} mhegSef_DmxRequest_t;

typedef struct
{
	HUINT32				 ulDemuxId;
	HBOOL				 bIsSefDemux;
	MhegSef_DemuxType_e	 eType;
	HINT32				 nSendingMsgQNum;
	HUINT32				 aulSendingMsgQid[mhegSef_MAX_SEF_SENDING_MSG_Q_NUM];

	HINT32				 nSecFilterCapa;
	HINT32				 nRequestCount;
	mhegSef_DmxRequest_t	*astRequestList;	/* array처럼 사용할 것임. */
} mhegSef_Context_t;

typedef struct
{
	HUINT32				  ulRequestIdSem;

	HUINT32				  ulNumOfTotalDemux;
	HUINT32				  ulNumOfSefDemux;

	HINT32				  nNumOfUsedSecFilter;	/* 전체 Demux 를 다 합친 사용중의 PSI Filter 갯수 */
	HINT32				  nNumOfTotalSecFilter;	/* 전체 Demux 를 다 합친 PSI Filter 갯수 */

	mhegSef_DmxRequest_t	**ppstDmxRequestHead;	/* demux 개수 만큼 */
	mhegSef_Context_t	 *pstContext;			/* demux 개수 만큼 */

#ifdef _MHEG_SEF_ALLOC_FREE_DEBUG_
	HUINT32				  ulUniqSecAllocNum;
#endif
} mhegSef_Container_t;

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC mhegSef_Container_t	s_stSefContainer;

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

/********************************************************************
                    local function prototypes
********************************************************************/
STATIC void 				mheg_sef_MakeRequestArray(HINT32 nNumOfFilter, mhegSef_DmxRequest_t **ppstReqHead);
STATIC mhegSef_Context_t*	mheg_sef_GetContext(HUINT8 ucDemuxId);
STATIC HBOOL				mheg_sef_IsFilterAvailable (HUINT8 ucDemuxId);
STATIC void					mheg_sef_IncreaseFilterUsageCount (HUINT8 ucDemuxId);
STATIC void					mheg_sef_DecreaseFilterUsageCount (HUINT8 ucDemuxId);
STATIC HUINT16      		mheg_sef_GetNextRequestId(mhegSef_Context_t *pstContext);
STATIC mhegSef_DmxRequest_t*	mheg_sef_GetRequest(mhegSef_Context_t *pstContext, HUINT32 ulRequestId);
STATIC mhegSef_DmxRequest_t*	mheg_sef_FindEmptyRequest(mhegSef_Context_t *pstContext);
STATIC DI_ERR_CODE			mheg_sef_CbOnSection(HUINT8 ucDemuxId, HUINT8 *pucBuf, HUINT32 ulRequestId);
STATIC DI_ERR_CODE			mheg_sef_CbOnRequestBuffer(HUINT16 usBufSize, HUINT8 **ppucBuf);

/********************************************************************
                         global functions
********************************************************************/
#define ______external_functions___________________________

#if	defined(CONFIG_USE_SEM_IN_ONSECTION)
DI_ERR_CODE	MHEG_SEF_CbOnSection(HUINT8 ucDemuxId, HUINT8 *pucBuf, HUINT32 ulRequestId)
{
	DI_ERR_CODE	res;

	mhegSef_ENTER_CRITICAL;

	res = mheg_sef_CbOnSection(ucDemuxId, pucBuf, ulRequestId);

	mhegSef_LEAVE_CRITICAL;

	return res;
}
#endif

HERROR MHEG_SEF_Init(void)
{
	HUINT32							i, hErr;
	DI_DEMUX_EXT_CAPABILITY			stCapability;
	DI_DEMUX_EXT_TOTALCAPABILITY	stTotalCap;
	DI_ERR_CODE						err;

	DI_DEMUX_EXT_Init();

	HxSTD_memset(&s_stSefContainer, 0, sizeof(mhegSef_Container_t));

	/* init. sef context */
	err = DI_DEMUX_EXT_GetTotalCapability (&stTotalCap);
	if (DI_ERR_OK != err)
	{
		HxLOG_Critical("Critical Eror!!!\n");
	}

	s_stSefContainer.ulNumOfTotalDemux		= stTotalCap.NumOfDemux;
	s_stSefContainer.nNumOfTotalSecFilter	= (HINT32)stTotalCap.NumOfTotalSectoinFilter;

	HxLOG_Debug("\033[30;43m[%s : %d] num of total demux : %d  \033[0m\n", __FUNCTION__, __LINE__, s_stSefContainer.ulNumOfTotalDemux);
	HxLOG_Debug("\033[30;43m[%s : %d] num of total Sec filter : %d  \033[0m\n", __FUNCTION__, __LINE__, s_stSefContainer.nNumOfTotalSecFilter);

	if(s_stSefContainer.ulNumOfTotalDemux == 0)
	{
		HxLOG_Error("num of demux = 0\n\n");
		return ERR_FAIL;
	}

	s_stSefContainer.pstContext = (mhegSef_Context_t *)HLIB_STD_MemAlloc(s_stSefContainer.ulNumOfTotalDemux * sizeof(mhegSef_Context_t));
	HxSTD_memset(s_stSefContainer.pstContext, 0, s_stSefContainer.ulNumOfTotalDemux * sizeof(mhegSef_Context_t));

	s_stSefContainer.ppstDmxRequestHead = (mhegSef_DmxRequest_t **)HLIB_STD_MemAlloc(s_stSefContainer.ulNumOfTotalDemux * sizeof(mhegSef_DmxRequest_t *));

	for(i=0, s_stSefContainer.ulNumOfSefDemux=0; i<s_stSefContainer.ulNumOfTotalDemux; i++)
	{
		s_stSefContainer.pstContext[i].ulDemuxId = i;

		/* get demux capability */
		err = DI_DEMUX_EXT_GetCapability(i, &stCapability);
		if(err != DI_ERR_OK)
		{
			HxLOG_Critical("Critical Eror!!!\n");
		}

		HxLOG_Debug("\033[30;43m[%s : %d] stCapability.NumOfSectoinFilter : %d  \033[0m\n", __FUNCTION__, __LINE__, stCapability.NumOfSectoinFilter);

		if(stCapability.ulCapability & (DI_DEMUX_EXT_CAP_SECTIONFILTER))
		{
			s_stSefContainer.ulNumOfSefDemux++;
			s_stSefContainer.pstContext[i].bIsSefDemux = TRUE;
		}

		if(stCapability.ulCapability & (DI_DEMUX_EXT_CAP_LIVE))
		{
			s_stSefContainer.pstContext[i].eType = eDMX_Type_LiveRec;
		}
		else if(stCapability.ulCapability & (DI_DEMUX_EXT_CAP_PLAYBACK))
		{
			s_stSefContainer.pstContext[i].eType = eDMX_Type_Playback;
		}
		else
		{
			s_stSefContainer.pstContext[i].eType = eDMX_Type_Unknown;
		}

		/* make request list */
		s_stSefContainer.pstContext[i].nSecFilterCapa = (HINT32)stCapability.NumOfSectoinFilter;
		mheg_sef_MakeRequestArray(stCapability.NumOfSectoinFilter, &s_stSefContainer.pstContext[i].astRequestList);

		/* register callback functions for each */
#if	defined(CONFIG_USE_SEM_IN_ONSECTION)
		if (DI_ERR_OK != DI_DEMUX_EXT_RegisterSectionCallback(i, (pfnDI_DEMUX_EXT_Notify)MHEG_SEF_CbOnSection, (pfnDI_DEMUX_EXT_AllocBuffer)mheg_sef_CbOnRequestBuffer))
		{
			HxLOG_Critical("DI_DEMUX_RegisterSectionCallback error(%d)!\n", i);
		}
#else
		DI_DEMUX_EXT_RegisterSectionCallback(i, (pfnDI_DEMUX_EXT_Notify)mheg_sef_CbOnSection, (pfnDI_DEMUX_EXT_AllocBuffer)mheg_sef_CbOnRequestBuffer);
#endif
	}

	hErr = VK_SEM_Create((HULONG *)&s_stSefContainer.ulRequestIdSem, "PSefSem", VK_SUSPENDTYPE_PRIORITY);
	if(hErr != ERR_OK)
	{
		HxLOG_Critical("VK_SEM_Create(PSefSem) Error\n");
		return ERR_FAIL;
	}
	mheg_sef_SiCreateFilterThread();

	return ERR_OK;
}

HERROR MHEG_SEF_GetInfo(MhegSef_DemuxInfo_e *pstDemuxInfo)
{
	HUINT32		i;

	if(pstDemuxInfo == NULL)
		return ERR_FAIL;

	pstDemuxInfo->ucNumLiveRecDemux = 0;
	pstDemuxInfo->ucNumPbDemux = 0;
	for(i=0; i<s_stSefContainer.ulNumOfTotalDemux; i++)
	{
		if(s_stSefContainer.pstContext[i].eType == eDMX_Type_LiveRec)
		{
			pstDemuxInfo->ucNumLiveRecDemux++;
		}
		else if(s_stSefContainer.pstContext[i].eType == eDMX_Type_Playback)
		{
			pstDemuxInfo->ucNumPbDemux++;
		}
	}

	return ERR_OK;
}

HERROR MHEG_SEF_GetType(HUINT8 ucDemuxId, MhegSef_DemuxType_e *pDemuxType)
{
	if(ucDemuxId >= s_stSefContainer.ulNumOfTotalDemux)
		return ERR_FAIL;

	if(pDemuxType == NULL)
		return ERR_FAIL;

	*pDemuxType = s_stSefContainer.pstContext[ucDemuxId].eType;

	return ERR_OK;
}

HERROR MHEG_SEF_GetNumOfSectionFilter(HUINT8 ucDemuxId, HINT32 *pnNumOfSecFilter)
{
	if((ucDemuxId >= s_stSefContainer.ulNumOfTotalDemux) || (pnNumOfSecFilter == NULL))
		return ERR_FAIL;

	*pnNumOfSecFilter = s_stSefContainer.pstContext[ucDemuxId].nSecFilterCapa;

	return ERR_OK;
}

HERROR MHEG_SEF_EnableSectionFilter(
									HUINT8 		ucDemuxId,
									HUINT16		usPid,
									HUINT32 	ulReceiveMsgQid,
									HUINT32 	ulEmergencyStopMsgQid,
									HINT32		nSecCntInPackage,
									HUINT32		nDurationInPackage,
									HUINT8		ucFilterLen,
									HUINT8		*aucFilterValue,
									HUINT8		*aucFilterMask,
									HUINT8		*aucFilterMode,
									HUINT32		*pulRequestId,
									HBOOL		bCrcCheck)
{
	DI_ERR_CODE			 err;
	mhegSef_DmxRequest_t	*pstRequest;
	HBOOL				 bIsNewMsgQ;
	HUINT32				 ulReqId;
	HINT32				 nOrderIdx;
	mhegSef_Context_t	*pstContext;

	HxLOG_Info("++ ENABLE SEF(%d) - pid:0x%04x, %02X %02X %02X\n", ucDemuxId, usPid, aucFilterValue[0], aucFilterValue[1], aucFilterValue[2]);

	mhegSef_ENTER_CRITICAL;

	pstContext = mheg_sef_GetContext(ucDemuxId);

	if( pstContext == NULL )
	{
		mhegSef_LEAVE_CRITICAL;
		HxLOG_Error("\tERROR : pstContext is NULL, ucDmxId:%d, usPid:0x%X\n", ucDemuxId, usPid);
		return ERR_FAIL;
	}

	if (mheg_sef_IsFilterAvailable(ucDemuxId) != TRUE)
	{
		mhegSef_LEAVE_CRITICAL;
		HxLOG_Error("\tERROR : mheg_sef_IsFilterAvailable FALSE\n");
		return ERR_FAIL;
	}

	/* assign unique ID to request */
	ulReqId = (HUINT32)mheg_sef_GetNextRequestId(pstContext);
	pstRequest = mheg_sef_FindEmptyRequest(pstContext);
	if(pstRequest == NULL)
	{
		mhegSef_LEAVE_CRITICAL;
		HxLOG_Error("\tERROR : pstRequest == NULL\n");
#if defined(CONFIG_DEBUG) && 0 /* 필요할 때 풀고 쓰세요 */
		mheg_sef_PrintContext();
#endif
		return ERR_FAIL;
	}

	for(nOrderIdx = 0, bIsNewMsgQ = TRUE; nOrderIdx < pstContext->nSendingMsgQNum; nOrderIdx++)
	{
		if(pstContext->aulSendingMsgQid[nOrderIdx] == ulReceiveMsgQid)
		{ // found same msg q
			bIsNewMsgQ = FALSE;
			break;
		}
	}

	if(nOrderIdx >= mhegSef_MAX_SEF_SENDING_MSG_Q_NUM)
	{
		mhegSef_LEAVE_CRITICAL;
		HxLOG_Error("\tERROR : nOrderIdx >= mhegSef_MAX_SEF_SENDING_MSG_Q_NUM\n");
		return ERR_FAIL;
	}

	pstContext->aulSendingMsgQid[nOrderIdx] = ulReceiveMsgQid;
	if(bIsNewMsgQ == TRUE)
		pstContext->nSendingMsgQNum++;

	mhegSef_MAKE_REQUESTID_WITH_QID(ulReqId, nOrderIdx);
	pstRequest->ulRequestId				= ulReqId;
	pstRequest->usPid 					= usPid;
	pstRequest->ucTableId 				= aucFilterValue[0];
	pstRequest->ulEmergencyStopMsgQid 	= ulEmergencyStopMsgQid;
	pstRequest->bEmergencyMsgSended		= FALSE;

#if 1
	/*
	  ulEmergencyStopMsgQid가 있을 경우만 Package기능이 On이 된다면
         Emergency기능을 사용하고 싶지는 않고 Package기능만 사용하고 싶을 경우 문제를 발생한다.
         더군다나 MHEG_SEF_EnableSectionFilter가 MHEG과 RAW EIT Manager에서 사용되도록 협의됨에 따라
         Emergency Stop기능은 Sithd.c에서만 사용한 기능이므로 이와 같이 고친다.
         -- 일시적인 수정으로 볼 수 있음 좀더 RAW EIT Manager 관련 논의를 해야함.
	*/
	pstRequest->nSecCntInPackage	= nSecCntInPackage;
	pstRequest->nDurationInPackage	= nDurationInPackage;
	pstRequest->nStartTickInPackage = 0;
#else
	/*
		section package count는 ulEmergencyStopMsgQid가 있어야 의미가 있다.
		demux section handler task에서 불리는 section callback 에서 filter를 stop할 수는 없기 때문이다.
	*/
	if(ulEmergencyStopMsgQid != 0)
	{
		pstRequest->nSecCntInPackage	= nSecCntInPackage;
		pstRequest->nDurationInPackage	= nDurationInPackage;
		pstRequest->nStartTickInPackage	= 0;
	}
	else
	{
		pstRequest->nSecCntInPackage	= 0;
		pstRequest->nDurationInPackage	= 0;
		pstRequest->nStartTickInPackage	= 0;
	}
#endif
	pstRequest->pstRawPackage 			= NULL;
	pstRequest->bUseFlag				= TRUE;
	mheg_sef_IncreaseFilterUsageCount (ucDemuxId);
	mhegSef_LEAVE_CRITICAL;

	/* MW doesn't have to find same pid channel. New DI operates both pid channel and section filter independantly. */
	/* so, do nothing here */

	/* MW doesn't decide a section filter type. it will be determinded DI internally as the filter length. */
	/* so, do nothing here */

	/* set & enable section filter */
	if(ucFilterLen > 14)
		ucFilterLen = 14;

	err = DI_DEMUX_EXT_SetSectionFilter((HUINT32)ucDemuxId, usPid, pstRequest->ulRequestId, FALSE,
									(HUINT32)ucFilterLen, aucFilterValue, aucFilterMask, aucFilterMode, bCrcCheck);
	if(err != DI_ERR_OK)
	{
		HxLOG_Error( "Error in DI_DEMUX_SetSectionFilter(). - demux[%d] pid[%04x], filter[%02x]\n", ucDemuxId, usPid, aucFilterValue[0]);
		mhegSef_ENTER_CRITICAL;
		pstRequest->bUseFlag = FALSE;
		mheg_sef_DecreaseFilterUsageCount (ucDemuxId);
		mhegSef_LEAVE_CRITICAL;
		return ERR_FAIL;
	}

	*pulRequestId = pstRequest->ulRequestId;

	HxLOG_Info("-- ENABLE SEF(%d) - req_id:%d\n", ucDemuxId, *pulRequestId);

	return ERR_OK;
}

HERROR MHEG_SEF_DisableSectionFilter(HUINT8 ucDemuxId, HUINT32 ulRequestId)
{
	DI_ERR_CODE			err;
	mhegSef_DmxRequest_t	*pstRequest;
	mhegSef_Context_t		*pstContext;
	HUINT32				ulReqId;

	HxLOG_Info("++ DISABLE SEF(%d) - req_id:%d\n", ucDemuxId, ulRequestId);

	mhegSef_ENTER_CRITICAL;
	pstContext = mheg_sef_GetContext(ucDemuxId);
	if( pstContext == NULL )
	{
		mhegSef_LEAVE_CRITICAL;
		HxLOG_Error("\tERROR : pstContext is NULL, ucDmxId:%d\n", ucDemuxId);
		return ERR_FAIL;
	}

	pstRequest = mheg_sef_GetRequest(pstContext, ulRequestId);
	if(pstRequest == NULL)
	{
		mhegSef_LEAVE_CRITICAL;
		HxLOG_Error("Cannot find DEMUX request.(ucDemuxId:%d)\n",ucDemuxId);
		return ERR_FAIL;
	}
	ulReqId = pstRequest->ulRequestId;

	/* MW doesn't have to consider if same pid channel exist on New DI */
	/* so, do nothing here */

	mheg_sef_DecreaseFilterUsageCount (ucDemuxId);
	pstRequest->bUseFlag = FALSE;

	if(pstRequest->pstRawPackage != NULL)
	{
		mheg_sef_FreePackageData(pstRequest->pstRawPackage);
		pstRequest->pstRawPackage = NULL;
	}
	mhegSef_LEAVE_CRITICAL;

	/* disable & free section filter */
	err = DI_DEMUX_EXT_ReleaseSectionFilter((HUINT32)ucDemuxId, ulReqId);
	if(err != DI_ERR_OK)
	{
		HxLOG_Error("Error in DI_DEMUX_ReleaseSectionFilter().\n");
		return ERR_FAIL;
	}

	HxLOG_Info("-- DISABLE SEF\n");

	return ERR_OK;
}

HERROR MHEG_SEF_SetDefaultRetrieveParam(SI_RETRIEVE_PARAM *pRetrieveParam,
														HUINT32 ulViewId,
														HUINT16 usTsUniqId,
														HUINT32 ucDemuxId)
{
	if (pRetrieveParam == NULL)			{ return ERR_FAIL; }

	HxSTD_memset(pRetrieveParam, 0, sizeof(SI_RETRIEVE_PARAM));

	pRetrieveParam->ulViewId		 = ulViewId;
	pRetrieveParam->usTsUniqId 	 = usTsUniqId;
	pRetrieveParam->ucDemuxId	 = ucDemuxId;
	pRetrieveParam->usPid		 = PID_NULL;
	pRetrieveParam->ucMode 		 = eSiFilterMode_Once;
	pRetrieveParam->ucHead		 = 0;//ACT_HEAD
	pRetrieveParam->bTableFilter = TRUE;
	pRetrieveParam->bUseExtId	 = FALSE;
	pRetrieveParam->bUseVerNum	 = FALSE;
	pRetrieveParam->bCrc		 = TRUE; /* crc 깨진 section에 대한 처리가 안되어 있는 경우도 있으므로, default가 TRUE인 것이 맞음 */
	pRetrieveParam->usExtId 	 = 0;
	pRetrieveParam->ucVerNum	 = 0;

	return ERR_OK;	
}

HERROR MHEG_SEF_RetrievePatSection (SI_RETRIEVE_PARAM *pstTable,
												void *pfnParse,
												void *pfnFree,
												HUINT16 *pusFilterId)
{
	HUINT16				usFilterId;
	MhegSef_FilterParam_t		stFilterParam;

	if (pstTable == NULL)					{ return ERR_FAIL; }
	if (pusFilterId == NULL)				{ return ERR_FAIL; }

	HxSTD_memset(&stFilterParam, 0 ,sizeof(MhegSef_FilterParam_t));
	MHEG_SEF_SiMakeDefaultFilterParam(&stFilterParam,
										PAT_TABLE_ID,
										pstTable->ucDemuxId,
										pstTable->usTsUniqId,
										PAT_PID,
										pstTable->bTableFilter,
										pstTable->bUseExtId,
										pstTable->bUseVerNum,
										pstTable->bCrc,
										pstTable->usExtId,
										pstTable->ucVerNum,
										pstTable->ucMode,
										10000,	//PAT_TIMEOUT_VALUE
										pstTable->ulViewId,
										(MhegSef_Callback_t)pfnParse,
										(MhegSef_Callback_t)pfnFree);

	usFilterId = MHEG_SEF_StartSecFilter(&stFilterParam);
	*pusFilterId = usFilterId;

	return ERR_OK;
}

HERROR MHEG_SEF_RetrievePmtSection (SI_RETRIEVE_PARAM *pstTable,
												void *pfnParse,
												void *pfnFree,
												HUINT16 *pusFilterId)
{
	HUINT16						usFilterId;
	MhegSef_FilterParam_t		stFilterParam;

	if (pstTable == NULL)					{ return ERR_FAIL; }
	if (pusFilterId == NULL)				{ return ERR_FAIL; }

	HxSTD_memset(&stFilterParam, 0 ,sizeof(MhegSef_FilterParam_t));
	MHEG_SEF_SiMakeDefaultFilterParam(&stFilterParam,
										PMT_TABLE_ID,
										pstTable->ucDemuxId,
										pstTable->usTsUniqId,
										pstTable->usPid,
										pstTable->bTableFilter,
										pstTable->bUseExtId,
										pstTable->bUseVerNum,
										pstTable->bCrc,
										pstTable->usExtId,
										pstTable->ucVerNum,
										pstTable->ucMode,
										10000,//PMT_TIMEOUT_VALUE
										pstTable->ulViewId,
										(MhegSef_Callback_t)pfnParse,
										(MhegSef_Callback_t)pfnFree);

	usFilterId = MHEG_SEF_StartSecFilter(&stFilterParam);
	*pusFilterId = usFilterId;

	return ERR_OK;
}




#define ______mheg_sef_internal_functions________________________________

HERROR mheg_sef_GetDemuxNum(HUINT8 *pucNumDev)
{
	*pucNumDev = (HUINT8)s_stSefContainer.ulNumOfTotalDemux;
	return ERR_OK;
}

void mheg_sef_FreeDemuxSection(HUINT8 *pucSec)
{
	if(pucSec)
	{
//++ for_debugging +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifdef _MHEG_SEF_ALLOC_FREE_DEBUG_
		HINT16		sLen;

		sLen = (HINT16)get12bit(&pucSec[1]);
		HxLOG_Print("- F %d\n", (HUINT32)((pucSec[sLen+3] << 24) | (pucSec[sLen+4] << 16) | (pucSec[sLen+5] << 8) | (pucSec[sLen+6])));
#endif
//------------------------------------------------------------------------------------------------------------------------------------
		HLIB_STD_MemFree(pucSec);
		pucSec = NULL;
	}
}

void mheg_sef_FreePackageData(MhegSef_RawData_t *pstRawPackage)
{
	if(pstRawPackage)
	{
		HINT32	i;

		for(i=0; i<pstRawPackage->num_sec; i++)
		{
			mheg_sef_FreeDemuxSection(pstRawPackage->buf[i]);
		}
		HLIB_STD_MemFree(pstRawPackage);
		pstRawPackage = NULL;
	}
}

#if defined(CONFIG_DEBUG)
void mheg_sef_PrintContext(void)
{
	HINT32			i, j;
	mhegSef_Context_t	*pstContext;

	mhegSef_ENTER_CRITICAL;


	HLIB_CMD_Printf(C_CODE_F_CYAN);
	HLIB_CMD_Printf("s_stSefContainer.ulRequestIdSem       = %d\n", s_stSefContainer.ulRequestIdSem);
	HLIB_CMD_Printf("s_stSefContainer.ulNumOfTotalDemux    = %d\n", s_stSefContainer.ulNumOfTotalDemux);
	HLIB_CMD_Printf("s_stSefContainer.ulNumOfSefDemux      = %d\n", s_stSefContainer.ulNumOfSefDemux);
	for(i=0; i<s_stSefContainer.ulNumOfTotalDemux; i++)
	{
		pstContext = mheg_sef_GetContext((HUINT8)i);
		if (NULL != pstContext)
		{
			if(pstContext->nRequestCount > 0)
			{
				HLIB_CMD_Printf("--------------------------------------------------------------\n");
				HLIB_CMD_Printf("\t pstContext->ulDemuxId 	   = %d\n", pstContext->ulDemuxId);
				HLIB_CMD_Printf("\t pstContext->bIsSefDemux	   = %d\n", pstContext->bIsSefDemux);
				HLIB_CMD_Printf("\t pstContext->eType 		   = %d\n", pstContext->eType);
				HLIB_CMD_Printf("\t pstContext->nSendingMsgQNum  = %d\n", pstContext->nSendingMsgQNum);
				for(j=0; j<pstContext->nSendingMsgQNum; j++)
				{
					HLIB_CMD_Printf("\t\t (%d) msg_qid	  = 0x%08X\n", j, pstContext->aulSendingMsgQid[j]);
				}
				HLIB_CMD_Printf("\t pstContext->nSecFilterCapa   = %d\n", pstContext->nSecFilterCapa);
				HLIB_CMD_Printf("\t pstContext->nRequestCount    = %d\n", pstContext->nRequestCount);
			}
			for(j=0; j<pstContext->nRequestCount; j++)
			{
				HLIB_CMD_Printf("\t\t (%d)", j);
				HLIB_CMD_Printf("\t, bUseFlag = %d", pstContext->astRequestList[j].bUseFlag);
				if(pstContext->astRequestList[j].bUseFlag)
				{
					HLIB_CMD_Printf(", ulRequestId = 0x%08X", pstContext->astRequestList[j].ulRequestId);
					HLIB_CMD_Printf(", usPid = 0x%X", pstContext->astRequestList[j].usPid);
					HLIB_CMD_Printf("\t, ucTableId = 0x%02X", pstContext->astRequestList[j].ucTableId);
				}
				HLIB_CMD_Printf("\n");
#if 0
				HxLOG_Print("\t\t ulEmergencyStopMsgQid  = %d\n", pstContext->astRequestList[j].ulEmergencyStopMsgQid);
				HxLOG_Print("\t\t bEmergencyMsgSended	 = %d\n", pstContext->astRequestList[j].bEmergencyMsgSended);
				HxLOG_Print("\t\t nSecCntInPackage		 = %d\n", pstContext->astRequestList[j].nSecCntInPackage);
				HxLOG_Print("\t\t nStartTickInPackage	 = %d\n", pstContext->astRequestList[j].nStartTickInPackage);
				HxLOG_Print("\t\t nDurationInPackage	 = %d\n", pstContext->astRequestList[j].nDurationInPackage);
#endif
				if(pstContext->astRequestList[j].pstRawPackage)
				{
					HLIB_CMD_Printf("\t\t\t ----> (%d) pstRawPackage->num_sec    = %d\n", j, pstContext->astRequestList[j].pstRawPackage->num_sec);
				}

			}
		}
	}

	HLIB_CMD_Printf(C_CODE_RESET);


	mhegSef_LEAVE_CRITICAL;
}
#endif
#define ______local_functions______________________

STATIC void mheg_sef_MakeRequestArray(HINT32 nNumOfFilter, mhegSef_DmxRequest_t **ppstReqHead)
{
	mhegSef_DmxRequest_t	*pHeadList;

	pHeadList = (mhegSef_DmxRequest_t *)HLIB_STD_MemAlloc(nNumOfFilter * sizeof(mhegSef_DmxRequest_t));
	HxSTD_memset(pHeadList, 0, nNumOfFilter * sizeof(mhegSef_DmxRequest_t));

	*ppstReqHead = pHeadList;
}

STATIC mhegSef_Context_t*	mheg_sef_GetContext(HUINT8 ucDemuxId)
{
	if(ucDemuxId >= s_stSefContainer.ulNumOfTotalDemux)
		return NULL;

	return &s_stSefContainer.pstContext[ucDemuxId];
}

STATIC HBOOL mheg_sef_IsFilterAvailable (HUINT8 ucDemuxId)
{
	mhegSef_Context_t			*pstContext = mheg_sef_GetContext (ucDemuxId);

	if (NULL != pstContext)
	{
		if ((s_stSefContainer.nNumOfUsedSecFilter < s_stSefContainer.nNumOfTotalSecFilter) &&
			(pstContext->nRequestCount < pstContext->nSecFilterCapa))
		{
			return TRUE;
		}
	}

	printf("\033[30;46m[%s : %d] FAIL - s_stSefContainer.nNumOfUsedSecFilter : %d  \033[0m\n", __FUNCTION__, __LINE__,s_stSefContainer.nNumOfUsedSecFilter);
	printf("\033[30;46m[%s : %d] FAIL - s_stSefContainer.nNumOfTotalSecFilter : %d	\033[0m\n", __FUNCTION__, __LINE__,s_stSefContainer.nNumOfTotalSecFilter);

	if(pstContext != NULL)
	{
		printf("\033[30;46m[%s : %d] FAIL - pstContext->nRequestCount : %d	\033[0m\n", __FUNCTION__, __LINE__,pstContext->nRequestCount);
		printf("\033[30;46m[%s : %d] FAIL - pstContext->nSecFilterCapa : %d  \033[0m\n", __FUNCTION__, __LINE__,pstContext->nSecFilterCapa);
	}
	else
	{
		printf("\033[30;46m[%s : %d] FAIL - pstContext is NULL \033[0m\n", __FUNCTION__, __LINE__ );
	}

	return FALSE;
}

STATIC void mheg_sef_IncreaseFilterUsageCount (HUINT8 ucDemuxId)
{
	mhegSef_Context_t			*pstContext = mheg_sef_GetContext (ucDemuxId);

	if (NULL != pstContext)
	{
		s_stSefContainer.nNumOfUsedSecFilter ++;
		pstContext->nRequestCount ++;
	}
}

STATIC void mheg_sef_DecreaseFilterUsageCount (HUINT8 ucDemuxId)
{
	mhegSef_Context_t			*pstContext = mheg_sef_GetContext (ucDemuxId);

	if (NULL != pstContext)
	{
		s_stSefContainer.nNumOfUsedSecFilter --;
		pstContext->nRequestCount  --;
	}
}



/**********************************************************
 * FuncName : mheg_sef_CheckExistRequestId
 * Description : mheg_sef_GetRequest()함수는 MHEG_SEF_DisableSectionFilter()
                       와 mheg_sef_CbOnSection()함수안에서 정확하게 Match되는
                       RequestId를 찾기 위해서 필요하고.
                       이 함수는 MessageQueueId를 위한 Order부분은 빼고 비교하는
                       부분이 필요해서 함수를 별도로 뺌.
 **********************************************************/
STATIC mhegSef_DmxRequest_t *mheg_sef_CheckExistRequestId(mhegSef_Context_t *pstContext, HUINT32 ulRequestId)
{
	HINT32		nReqIdx;

	if(pstContext->astRequestList == NULL)
	{
		HxLOG_Error("NULL pointer is returned for head node of DEMUX request list.\n");
		return NULL;
	}

	/* find a request with same request id */
	for(nReqIdx = 0; nReqIdx < pstContext->nSecFilterCapa; nReqIdx++)
	{
		if(pstContext->astRequestList[nReqIdx].bUseFlag == TRUE)
		{
			HxLOG_Print("[CheckExistRequestId]pstContext->astRequestList[%02d].ulRequestId:0x%X,ulRequestId:0x%X\n",
				nReqIdx,pstContext->astRequestList[nReqIdx].ulRequestId,ulRequestId);
			if((pstContext->astRequestList[nReqIdx].ulRequestId & 0xffff) == (ulRequestId & 0xffff))
			{ // found
				return &pstContext->astRequestList[nReqIdx];
			}
		}
	}

	return NULL;
}

STATIC HUINT16 mheg_sef_GetNextRequestId(mhegSef_Context_t *pstContext)
{
	STATIC HUINT16 		s_usDemuxRequestId = 0;
	mhegSef_DmxRequest_t 	*pstRequest;

	/* 할당하려고 하는 request_id가 filtering 중인 id 이면 skip하고 새로 할당한다 */
	do
	{
		s_usDemuxRequestId++;

		if (mhegSef_INVALID_REQUEST == s_usDemuxRequestId)
			s_usDemuxRequestId = 1;

		pstRequest = mheg_sef_CheckExistRequestId(pstContext, s_usDemuxRequestId);
	} while (pstRequest);

	return s_usDemuxRequestId;
}

STATIC mhegSef_DmxRequest_t *mheg_sef_GetRequest(mhegSef_Context_t *pstContext, HUINT32 ulRequestId)
{
	HINT32		nReqIdx;

	if(pstContext->astRequestList == NULL)
	{
		HxLOG_Error("NULL pointer is returned for head node of DEMUX request list.\n");
		return NULL;
	}

	/* find a request with same request id */
	for(nReqIdx = 0; nReqIdx < pstContext->nSecFilterCapa; nReqIdx++)
	{
		if(pstContext->astRequestList[nReqIdx].bUseFlag == TRUE)
		{
			HxLOG_Print("[GetRequest]pstContext->astRequestList[%02d].ulRequestId:0x%X,ulRequestId:0x%X\n",
				nReqIdx,pstContext->astRequestList[nReqIdx].ulRequestId,ulRequestId);
			if(pstContext->astRequestList[nReqIdx].ulRequestId == ulRequestId)
			{ // found
				return &pstContext->astRequestList[nReqIdx];
			}
		}
	}

	return NULL;
}

STATIC mhegSef_DmxRequest_t *mheg_sef_FindEmptyRequest(mhegSef_Context_t *pstContext)
{
	HINT32		nReqIdx;

	if(pstContext->astRequestList == NULL)
	{
		HxLOG_Error("NULL pointer is returned for head node of DEMUX request list.\n");
		return NULL;
	}

	for(nReqIdx = 0; nReqIdx < pstContext->nSecFilterCapa; nReqIdx++)
	{
		if(pstContext->astRequestList[nReqIdx].bUseFlag == FALSE)
		{ // found
			return &pstContext->astRequestList[nReqIdx];
		}
	}

	return NULL;
}

STATIC HBOOL mheg_sef_CheckData(MhegSef_ReceiveMessage_t *pstRcvMsg, mhegSef_DmxRequest_t *pstRequest, HUINT8 *pucBuf)
{
	if(pstRequest->nSecCntInPackage > 1)
	{
		if(pstRequest->pstRawPackage == NULL)
		{
			pstRequest->pstRawPackage = (MhegSef_RawData_t *)HLIB_STD_MemAlloc(sizeof(MhegSef_RawData_t));
			pstRequest->pstRawPackage->num_sec = 0;
			pstRequest->nStartTickInPackage = VK_TIMER_GetSystemTick();
		}

		if(pstRequest->pstRawPackage->num_sec < pstRequest->nSecCntInPackage)
		{
			pstRequest->pstRawPackage->buf[pstRequest->pstRawPackage->num_sec] = pucBuf;
			pstRequest->pstRawPackage->num_sec++;
			//HxLOG_Print("%02X - num:%d, req:0x%08X\n", pucBuf[0], pstRequest->pstRawPackage->num_sec, pstRequest->ulRequestId);
			if(	(pstRequest->pstRawPackage->num_sec == pstRequest->nSecCntInPackage)
				|| ((VK_TIMER_GetSystemTick() - pstRequest->nStartTickInPackage) >= pstRequest->nDurationInPackage)
			)
			{
				//HxLOG_Print("-----------> eSectionPackage_On : %02X - num:%d, req:0x%08X\n", pucBuf[0], pstRequest->pstRawPackage->num_sec, pstRequest->ulRequestId);
				HxLOG_Info("[mheg_sef_CbOnSection] ulDuration(%d)num_sec(%d)\n",	VK_TIMER_GetSystemTick() - pstRequest->nStartTickInPackage, pstRequest->pstRawPackage->num_sec);
				pstRcvMsg->ulDataAddr = (HUINT32)pstRequest->pstRawPackage;
				pstRequest->pstRawPackage = NULL;
				pstRcvMsg->eSecPackageFlag = eSectionPackage_On;	/* package_flag : ON */
				return FALSE;
			}
			else
			{
				return TRUE; /* package 모으는 중 : err ok */
			}
		}
		else
		{
			HxLOG_Error("[mheg_sef_CbOnSection:%d] Error : can't reach here\n", __LINE__);
			mheg_sef_FreePackageData(pstRequest->pstRawPackage);
			pstRequest->pstRawPackage = NULL;
			return TRUE; /* package buffer 초과 : err fail */
		}
	}
	else
	{
		pstRcvMsg->ulDataAddr = (HUINT32)pucBuf;
		pstRcvMsg->eSecPackageFlag = eSectionPackage_Off;	/* package_flag : OFF */
		return FALSE; /* err ok */
	}
}

STATIC HERROR mheg_sef_CheckEmergency(MhegSef_ReceiveMessage_t *pstRcvMsg, MhegSef_SendMessage_t *pstEmergencyMsg, HUINT32 *pulMsgQ, HBOOL *pbIsEmergency, HBOOL bEmergencyMsgSended)
{
	HUINT32			ulCurMsgCnt, ulMaxMsgCnt;
	HINT32			nErr;
	MhegSef_RawData_t		*pstRawPackage;

	if((pstRcvMsg == NULL) || (pstEmergencyMsg == NULL) || (pulMsgQ == NULL) || (pbIsEmergency == NULL))
		return ERR_FAIL;

	*pbIsEmergency = FALSE;

	nErr = VK_MSG_GetMessageCount(*pulMsgQ, (HULONG *)&ulCurMsgCnt, (HULONG *)&ulMaxMsgCnt);
	if(nErr)
	{
		HxLOG_Error("Error - can't get msg_q info (q_id = 0x%x)\n", *pulMsgQ);
		return ERR_FAIL;
	}

	if(ulCurMsgCnt >= (ulMaxMsgCnt / 2)) /* 50% 이상이면 nearly full */
	{
		/*
			section filter는 ulEmergencyMsgQ를 통해서 stop한다. semaphore deadlock 때문에 callback에서 stop할 수 없음!!
			ulEmergencyMsgQ에서는 필터를 stop하고, restart를 위한 처리를 한다.
			sithd에서는 msg_q_cnt를 모니터링 하다가 다 처리되면 새로 filtering을 시작한다.
		*/
		HxLOG_Print("[WARN] si recv msgq - nearly full (%d/%d) (req_id=%d)\n", ulCurMsgCnt, ulMaxMsgCnt, pstRcvMsg->ulRequestId);
		//HxLOG_Error("\\#");

		if(bEmergencyMsgSended == TRUE)
		{ /* msg 보내면 받는 놈이 또 쌓일 수 있으므로, 그냥 free한다 */
			return ERR_FAIL;
		}

		pstEmergencyMsg->eMsgClass = eSiFilterMsg_EmergencyStop;
		pstEmergencyMsg->ucDemuxId = pstRcvMsg->ucDemuxId;
		pstEmergencyMsg->msg.stEmergencyStop.ulRequestId = pstRcvMsg->ulRequestId;

		if(eSectionPackage_On == pstRcvMsg->eSecPackageFlag)
		{
			pstRawPackage = (MhegSef_RawData_t *)pstRcvMsg->ulDataAddr;
			mheg_sef_FreePackageData(pstRawPackage);
		}
		else
		{
			mheg_sef_FreeDemuxSection((HUINT8 *)pstRcvMsg->ulDataAddr);
		}
		pstRcvMsg->ulDataAddr = (HUINT32)NULL;
		*pbIsEmergency = TRUE;
	}

	return ERR_OK;
}

STATIC DI_ERR_CODE mheg_sef_CbOnSection(HUINT8 ucDemuxId, HUINT8 *pucBuf, HUINT32 ulRequestId)
{
	HERROR					 err;
	HBOOL					 bReturn, bIsEmergency;
	MhegSef_SendMessage_t	 stEmergencyMsg;
	MhegSef_ReceiveMessage_t	 stRcvMsg;
	HUINT32					 ulMsgQ, ulEmergencyMsgQ;
	HINT32					 nErr, nOrderIdx;
	mhegSef_DmxRequest_t 	*pstRequest;
	mhegSef_Context_t		*pstContext;

	if(pucBuf == NULL)
	{
		HxLOG_Error("[mheg_sef_CbOnSection:%d] Error : pucBuf is NULL : ulRequestId = 0x%08X\n", ucDemuxId, ulRequestId);
		return DI_ERR_ERROR;
	}
//++ for_debugging +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifdef _MHEG_SEF_ALLOC_FREE_DEBUG_
	{
		HINT16		sLen;

		sLen = (HINT16)get12bit(&pucBuf[1]);
		s_stSefContainer.ulUniqSecAllocNum++;
		pucBuf[sLen+3] = (HUINT8)((s_stSefContainer.ulUniqSecAllocNum >> 24) & 0xff);
		pucBuf[sLen+4] = (HUINT8)((s_stSefContainer.ulUniqSecAllocNum >> 16) & 0xff);
		pucBuf[sLen+5] = (HUINT8)((s_stSefContainer.ulUniqSecAllocNum >> 8) & 0xff);
		pucBuf[sLen+6] = (HUINT8)((s_stSefContainer.ulUniqSecAllocNum) & 0xff);
		HxLOG_Print("+ A %d\n", (HUINT32)((pucBuf[sLen+3] << 24) | (pucBuf[sLen+4] << 16) | (pucBuf[sLen+5] << 8) | (pucBuf[sLen+6])));
	}
#endif
//------------------------------------------------------------------------------------------------------------------------------------

	pstContext = mheg_sef_GetContext(ucDemuxId);

	if(pstContext == NULL)
	{
		mheg_sef_FreeDemuxSection(pucBuf);
		return DI_ERR_OK;
	}

	pstRequest = mheg_sef_GetRequest(pstContext, ulRequestId);
	if(pstRequest == NULL)
	{
		HxLOG_Print("[mheg_sef_CbOnSection:%d] Error : Cannot find Demux : ulRequestId = 0x%08X\n", ucDemuxId, ulRequestId);
		mheg_sef_FreeDemuxSection(pucBuf);
		return DI_ERR_OK;
	}

	bReturn = mheg_sef_CheckData(&stRcvMsg, pstRequest, pucBuf);
	if(bReturn == TRUE)
		return DI_ERR_OK;

	stRcvMsg.ucTableId 		= pucBuf[0];
	stRcvMsg.ulRequestId 	= ulRequestId;
	stRcvMsg.err 			= ERR_OK;
	stRcvMsg.ucDemuxId 		= ucDemuxId;

	mhegSef_MAKE_ORDERIDX_FROM_REQUESTID(nOrderIdx, ulRequestId);
	ulMsgQ 			= pstContext->aulSendingMsgQid[nOrderIdx];
	ulEmergencyMsgQ	= pstRequest->ulEmergencyStopMsgQid;

	bIsEmergency = FALSE;
	/* emergency queue가 설정된 경우만 의미 있음 */
	if(ulEmergencyMsgQ != 0)
	{
		err = mheg_sef_CheckEmergency(&stRcvMsg, &stEmergencyMsg, &ulMsgQ, &bIsEmergency, pstRequest->bEmergencyMsgSended);
		if(err != ERR_OK)
			goto err_rtn;

		if(bIsEmergency == TRUE)
		{
			pstRequest->bEmergencyMsgSended = TRUE;
			ulMsgQ = pstRequest->ulEmergencyStopMsgQid;
		}
	}

	if(bIsEmergency == TRUE)
	{
		HxLOG_Error(C_CODE_F_RED);
		nErr = VK_MSG_SendTimeout(ulMsgQ, &stEmergencyMsg, sizeof(MhegSef_SendMessage_t), 0);
		HxLOG_Error("\n\t\t\t--> sent!!\n\n");
		if(nErr)
		{
			HxLOG_Error("\\$");
		}
	}
	else
	{
		nErr = VK_MSG_SendTimeout(ulMsgQ, &stRcvMsg, sizeof(MhegSef_ReceiveMessage_t), 0);
		if(nErr)
		{
			HxLOG_Print("Error : msg send fail : dmxid = %d, req_id = 0x%08X\n", ucDemuxId, ulRequestId);
			HxLOG_Error("\\*");
			goto err_rtn;
		}
	}

	return DI_ERR_OK;

err_rtn:
	if(eSectionPackage_On == stRcvMsg.eSecPackageFlag)
	{
		mheg_sef_FreePackageData((MhegSef_RawData_t *)stRcvMsg.ulDataAddr);
	}
	else
	{
		mheg_sef_FreeDemuxSection(pucBuf);
	}

	return DI_ERR_OK; /* DI_ERR_ERROR 처리를 하지 않는 이유는, 어차피 notify callback 안에서 section free까지 이루어지기 때문. 즉, di입장에서는 해줄 것이 없음 */
}

STATIC DI_ERR_CODE mheg_sef_CbOnRequestBuffer(HUINT16 usBufSize, HUINT8 **ppucBuf)
{
	*ppucBuf = NULL;
	if (usBufSize == 0) {
		HxLOG_Error("request buffer size is 0\n");
		return ERR_FAIL;
	}

#ifdef _MHEG_SEF_ALLOC_FREE_DEBUG_
	*ppucBuf = (HUINT8 *)HLIB_STD_MemAlloc(usBufSize+4);
#else
	*ppucBuf = (HUINT8 *)HLIB_STD_MemAlloc(usBufSize);
#endif
	if (*ppucBuf == 0) {
		HxLOG_Error("can't alloc memory for section buffer\n");
		return ERR_FAIL;
	}

	return DI_ERR_OK;
}

/*********************** End of File ******************************/

