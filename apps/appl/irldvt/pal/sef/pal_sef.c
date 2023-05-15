/**
	@file     pal_sef.c
	@brief    demux control module (section filter part)

	Description: abstraction layer for section filtering \n
				 pid channel (audio/video/pcr) and pes filter (subtitle/teletext/etc.) will be removed in this module. \n
				 Those functions are belongs to pal_video / pal_audio / pal_subtitle. \n
				 Each module to use di_demux calls DI directly. \n
				 And pal_channel and pal_ci will be in charge of demux routing function.
	Module     : PAL DEMUX \n
	Remarks    : octo base							\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <htype.h>
#include <vkernel.h>
//#include <util.h>
//#include <octo_common.h>
#include <hlib.h>
#include <di_err.h>
#include <di_demux.h>

#include <pal_sef.h>
#include "_pal_sef.h"


#define	CONFIG_USE_SEM_IN_ONSECTION

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/



#ifdef CONFIG_DEBUG
//#define _PALDMX_SEM_DEBUG_
#endif
#ifdef _PALDMX_SEM_DEBUG_
#define	palSef_ENTER_CRITICAL		{ HxLOG_Print(C_CODE_F_CYAN), HxLOG_Print("\t++++ palSef_ENTER_CRITICAL(%s:%d)"C_CODE_RESET"\n", __FUNCTION__, __LINE__); VK_SEM_Get(s_stSefContainer.ulRequestIdSem);}
#define	palSef_LEAVE_CRITICAL		{ VK_SEM_Release(s_stSefContainer.ulRequestIdSem); HxLOG_Print(C_CODE_F_CYAN), HxLOG_Print("\t---- palSef_LEAVE_CRITICAL(%s:%d)"C_CODE_RESET"\n", __FUNCTION__, __LINE__);}
#else
#define palSef_ENTER_CRITICAL		VK_SEM_Get(s_stSefContainer.ulRequestIdSem)
#define palSef_LEAVE_CRITICAL		VK_SEM_Release(s_stSefContainer.ulRequestIdSem)
#endif

#define palSef_INVALID_REQUEST      		0xffff
#define palSef_MAX_SEF_SENDING_MSG_Q_NUM			16		/* �ý��ۿ��� ��� ������ �ִ� sending msg queue ���� - request id�� ���� 16��Ʈ�� Ȱ�� */
#define palSef_MAKE_REQUESTID_WITH_QID(ulReqId, nOrderIdx)			{ ulReqId |= (0x80000000 >> nOrderIdx); }
#define palSef_MAKE_ORDERIDX_FROM_REQUESTID(nOrderIdx, ulReqId)	\
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
		if(nOrderIdx >= (palSef_MAX_SEF_SENDING_MSG_Q_NUM-1))                  \
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

	/* msg queue ���� */
	HUINT32				 ulEmergencyStopMsgQid;	/* ��� �� 0�� �ƴ� ������ ���� */
	HBOOL				 bEmergencyMsgSended;

	/* section package ����� */
	HINT32				 nSecCntInPackage;
	HUINT32				 nStartTickInPackage;
	HUINT32				 nDurationInPackage;
	PalSef_RawData_t	*pstRawPackage;
} palSef_DmxRequest_t;

typedef struct
{
	HUINT32				 ulDemuxId;
	HBOOL				 bIsSefDemux;
	PalSef_DemuxType_e	 eType;
	HINT32				 nSendingMsgQNum;
	HUINT32				 aulSendingMsgQid[palSef_MAX_SEF_SENDING_MSG_Q_NUM];

	HINT32				 nSecFilterCapa;
	HINT32				 nRequestCount;
	palSef_DmxRequest_t	*astRequestList;	/* arrayó�� ����� ����. */
} palSef_Context_t;

typedef struct
{
	HUINT32				  ulRequestIdSem;

	HUINT32				  ulNumOfTotalDemux;
	HUINT32				  ulNumOfSefDemux;

	HINT32				  nNumOfUsedSecFilter;	/* ��ü Demux �� �� ��ģ ������� PSI Filter ���� */
	HINT32				  nNumOfTotalSecFilter;	/* ��ü Demux �� �� ��ģ PSI Filter ���� */

	palSef_DmxRequest_t	**ppstDmxRequestHead;	/* demux ���� ��ŭ */
	palSef_Context_t	 *pstContext;			/* demux ���� ��ŭ */

#ifdef _PAL_SEF_ALLOC_FREE_DEBUG_
	HUINT32				  ulUniqSecAllocNum;
#endif
} palSef_Container_t;

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC palSef_Container_t	s_stSefContainer;

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

/********************************************************************
                    local function prototypes
********************************************************************/
STATIC void 				pal_sef_MakeRequestArray(HINT32 nNumOfFilter, palSef_DmxRequest_t **ppstReqHead);
STATIC palSef_Context_t*	pal_sef_GetContext(HUINT8 ucDemuxId);
STATIC HBOOL				pal_sef_IsFilterAvailable (HUINT8 ucDemuxId);
STATIC void					pal_sef_IncreaseFilterUsageCount (HUINT8 ucDemuxId);
STATIC void					pal_sef_DecreaseFilterUsageCount (HUINT8 ucDemuxId);
STATIC HUINT16      		pal_sef_GetNextRequestId(palSef_Context_t *pstContext);
STATIC palSef_DmxRequest_t*	pal_sef_GetRequest(palSef_Context_t *pstContext, HUINT32 ulRequestId);
STATIC palSef_DmxRequest_t*	pal_sef_FindEmptyRequest(palSef_Context_t *pstContext);
STATIC DI_ERR_CODE			pal_sef_CbOnSection(HUINT8 ucDemuxId, HUINT8 *pucBuf, HUINT32 ulRequestId);
STATIC DI_ERR_CODE			pal_sef_CbOnRequestBuffer(HUINT16 usBufSize, HUINT8 **ppucBuf);

/********************************************************************
                         global functions
********************************************************************/
#define ______external_functions___________________________

#if	defined(CONFIG_USE_SEM_IN_ONSECTION)
DI_ERR_CODE	PAL_SEF_CbOnSection(HUINT8 ucDemuxId, HUINT8 *pucBuf, HUINT32 ulRequestId)
{
	DI_ERR_CODE	res;

	palSef_ENTER_CRITICAL;

	res = pal_sef_CbOnSection(ucDemuxId, pucBuf, ulRequestId);

	palSef_LEAVE_CRITICAL;

	return res;
}
#endif

#if 0
HERROR PAL_SEF_Init(void)
{
	HUINT32						i;
	DI_DEMUX_CAPABILITY			stCapability;
	DI_DEMUX_TOTALCAPABILITY	stTotalCap;
	DI_ERR_CODE					err;

	HxSTD_memset(&s_stSefContainer, 0, sizeof(palSef_Container_t));

	/* init. sef context */
	err = DI_DEMUX_GetTotalCapability (&stTotalCap);
	if (DI_ERR_OK != err)
	{
		HxLOG_Critical("Critical Eror!!!\n");
	}

	s_stSefContainer.ulNumOfTotalDemux		= stTotalCap.NumOfDemux;
	s_stSefContainer.nNumOfTotalSecFilter	= (HINT32)stTotalCap.NumOfTotalSectoinFilter;

	if(s_stSefContainer.ulNumOfTotalDemux == 0)
	{
		HxLOG_Error("num of demux = 0\n\n");
		return ERR_FAIL;
	}

	s_stSefContainer.pstContext = (palSef_Context_t *)HLIB_STD_MemAlloc(s_stSefContainer.ulNumOfTotalDemux * sizeof(palSef_Context_t));
	HxSTD_memset(s_stSefContainer.pstContext, 0, s_stSefContainer.ulNumOfTotalDemux * sizeof(palSef_Context_t));

	s_stSefContainer.ppstDmxRequestHead = (palSef_DmxRequest_t **)HLIB_STD_MemAlloc(s_stSefContainer.ulNumOfTotalDemux * sizeof(palSef_DmxRequest_t *));

	for(i=0, s_stSefContainer.ulNumOfSefDemux=0; i<s_stSefContainer.ulNumOfTotalDemux; i++)
	{
		s_stSefContainer.pstContext[i].ulDemuxId = i;

		/* get demux capability */
		err = DI_DEMUX_GetCapability(i, &stCapability);
		if(err != DI_ERR_OK)
		{
			HxLOG_Critical("Critical Eror!!!\n");
		}


		if(stCapability.ulCapability & (DI_DEMUX_CAP_SECTIONFILTER))
		{
			s_stSefContainer.ulNumOfSefDemux++;
			s_stSefContainer.pstContext[i].bIsSefDemux = TRUE;
		}

		if(stCapability.ulCapability & (DI_DEMUX_CAP_LIVE))
		{
			s_stSefContainer.pstContext[i].eType = eDMX_Type_LiveRec;
		}
		else if(stCapability.ulCapability & (DI_DEMUX_CAP_PLAYBACK))
		{
			s_stSefContainer.pstContext[i].eType = eDMX_Type_Playback;
		}
		else
		{
			s_stSefContainer.pstContext[i].eType = eDMX_Type_Unknown;
		}

		/* make request list */
		s_stSefContainer.pstContext[i].nSecFilterCapa = (HINT32)stCapability.NumOfSectoinFilter;
		pal_sef_MakeRequestArray(stCapability.NumOfSectoinFilter, &s_stSefContainer.pstContext[i].astRequestList);

		/* register callback functions for each */
#if	defined(CONFIG_USE_SEM_IN_ONSECTION)
		if (DI_ERR_OK != DI_DEMUX_RegisterSectionCallback(i, (pfnDI_DEMUX_Notify)PAL_SEF_CbOnSection, (pfnDI_DEMUX_AllocBuffer)pal_sef_CbOnRequestBuffer))
		{
			HxLOG_Critical("DI_DEMUX_RegisterSectionCallback error(%d)!\n", i);
		}
#else
		DI_DEMUX_RegisterSectionCallback(i, (pfnDI_DEMUX_Notify)pal_sef_CbOnSection, (pfnDI_DEMUX_AllocBuffer)pal_sef_CbOnRequestBuffer);
#endif
	}

	VK_SEM_Create((HULONG *)&s_stSefContainer.ulRequestIdSem, "PSefSem", VK_SUSPENDTYPE_PRIORITY);

	pal_sef_SiCreateFilterThread();

	return ERR_OK;
}

#endif
HERROR PAL_SEF_GetInfo(PalSef_DemuxInfo_e *pstDemuxInfo)
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

HERROR PAL_SEF_GetType(HUINT8 ucDemuxId, PalSef_DemuxType_e *pDemuxType)
{
	if(ucDemuxId >= s_stSefContainer.ulNumOfTotalDemux)
		return ERR_FAIL;

	if(pDemuxType == NULL)
		return ERR_FAIL;

	*pDemuxType = s_stSefContainer.pstContext[ucDemuxId].eType;

	return ERR_OK;
}

HERROR PAL_SEF_GetNumOfSectionFilter(HUINT8 ucDemuxId, HINT32 *pnNumOfSecFilter)
{
	if((ucDemuxId >= s_stSefContainer.ulNumOfTotalDemux) || (pnNumOfSecFilter == NULL))
		return ERR_FAIL;

	*pnNumOfSecFilter = s_stSefContainer.pstContext[ucDemuxId].nSecFilterCapa;

	return ERR_OK;
}

HERROR PAL_SEF_EnableSectionFilter(
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
	palSef_DmxRequest_t	*pstRequest;
	HBOOL				 bIsNewMsgQ;
	HUINT32				 ulReqId;
	HINT32				 nOrderIdx;
	palSef_Context_t	*pstContext;

	HxLOG_Info("++ ENABLE SEF(%d) - pid:0x%04x, %02X %02X %02X\n", ucDemuxId, usPid, aucFilterValue[0], aucFilterValue[1], aucFilterValue[2]);

	palSef_ENTER_CRITICAL;

	pstContext = pal_sef_GetContext(ucDemuxId);

	if( pstContext == NULL )
	{
		palSef_LEAVE_CRITICAL;
		HxLOG_Error("\tERROR : pstContext is NULL, ucDmxId:%d, usPid:0x%X\n", ucDemuxId, usPid);
		return ERR_FAIL;
	}

	if (pal_sef_IsFilterAvailable(ucDemuxId) != TRUE)
	{
		palSef_LEAVE_CRITICAL;
		HxLOG_Error("\tERROR : pal_sef_IsFilterAvailable FALSE\n");
		return ERR_FAIL;
	}

	/* assign unique ID to request */
	ulReqId = (HUINT32)pal_sef_GetNextRequestId(pstContext);
	pstRequest = pal_sef_FindEmptyRequest(pstContext);
	if(pstRequest == NULL)
	{
		palSef_LEAVE_CRITICAL;
		HxLOG_Error("\tERROR : pstRequest == NULL\n");
#if defined(CONFIG_DEBUG) && 0 /* �ʿ��� �� Ǯ�� ������ */
		pal_sef_PrintContext();
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

	if(nOrderIdx >= palSef_MAX_SEF_SENDING_MSG_Q_NUM)
	{
		palSef_LEAVE_CRITICAL;
		HxLOG_Error("\tERROR : nOrderIdx >= palSef_MAX_SEF_SENDING_MSG_Q_NUM\n");
		return ERR_FAIL;
	}

	pstContext->aulSendingMsgQid[nOrderIdx] = ulReceiveMsgQid;
	if(bIsNewMsgQ == TRUE)
		pstContext->nSendingMsgQNum++;

	palSef_MAKE_REQUESTID_WITH_QID(ulReqId, nOrderIdx);
	pstRequest->ulRequestId				= ulReqId;
	pstRequest->usPid 					= usPid;
	pstRequest->ucTableId 				= aucFilterValue[0];
	pstRequest->ulEmergencyStopMsgQid 	= ulEmergencyStopMsgQid;
	pstRequest->bEmergencyMsgSended		= FALSE;

#if 1
	/*
	  ulEmergencyStopMsgQid�� ���� ��츸 Package����� On�� �ȴٸ�
         Emergency����� ����ϰ� ������ �ʰ� Package��ɸ� ����ϰ� ���� ��� ������ �߻��Ѵ�.
         �����ٳ� PAL_SEF_EnableSectionFilter�� MHEG�� RAW EIT Manager���� ���ǵ��� ���ǵʿ� ����
         Emergency Stop����� Sithd.c������ ����� ����̹Ƿ� �̿� ���� ��ģ��.
         -- �Ͻ����� �������� �� �� ���� ���� RAW EIT Manager ���� ���Ǹ� �ؾ���.
	*/
	pstRequest->nSecCntInPackage	= nSecCntInPackage;
	pstRequest->nDurationInPackage	= nDurationInPackage;
	pstRequest->nStartTickInPackage = 0;
#else
	/*
		section package count�� ulEmergencyStopMsgQid�� �־�� �ǹ̰� �ִ�.
		demux section handler task���� �Ҹ��� section callback ���� filter�� stop�� ���� ���� �����̴�.
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
	pal_sef_IncreaseFilterUsageCount (ucDemuxId);
	palSef_LEAVE_CRITICAL;

	/* MW doesn't have to find same pid channel. New DI operates both pid channel and section filter independantly. */
	/* so, do nothing here */

	/* MW doesn't decide a section filter type. it will be determinded DI internally as the filter length. */
	/* so, do nothing here */

	/* set & enable section filter */
	if(ucFilterLen > 14)
		ucFilterLen = 14;

	err = DI_DEMUX_SetSectionFilter((HUINT32)ucDemuxId, usPid, pstRequest->ulRequestId, FALSE,
									(HUINT32)ucFilterLen, aucFilterValue, aucFilterMask, aucFilterMode, bCrcCheck);
	if(err != DI_ERR_OK)
	{
		HxLOG_Error( "Error in DI_DEMUX_SetSectionFilter(). - demux[%d] pid[%04x], filter[%02x]\n", ucDemuxId, usPid, aucFilterValue[0]);
		palSef_ENTER_CRITICAL;
		pstRequest->bUseFlag = FALSE;
		pal_sef_DecreaseFilterUsageCount (ucDemuxId);
		palSef_LEAVE_CRITICAL;
		return ERR_FAIL;
	}

	*pulRequestId = pstRequest->ulRequestId;

	HxLOG_Info("-- ENABLE SEF(%d) - req_id:%d\n", ucDemuxId, *pulRequestId);

	return ERR_OK;
}

HERROR PAL_SEF_DisableSectionFilter(HUINT8 ucDemuxId, HUINT32 ulRequestId)
{
	DI_ERR_CODE			err;
	palSef_DmxRequest_t	*pstRequest;
	palSef_Context_t		*pstContext;
	HUINT32				ulReqId;

	HxLOG_Info("++ DISABLE SEF(%d) - req_id:%d\n", ucDemuxId, ulRequestId);

	palSef_ENTER_CRITICAL;
	pstContext = pal_sef_GetContext(ucDemuxId);
	if( pstContext == NULL )
	{
		palSef_LEAVE_CRITICAL;
		HxLOG_Error("\tERROR : pstContext is NULL, ucDmxId:%d\n", ucDemuxId);
		return ERR_FAIL;
	}

	pstRequest = pal_sef_GetRequest(pstContext, ulRequestId);
	if(pstRequest == NULL)
	{
		palSef_LEAVE_CRITICAL;
		HxLOG_Error("Cannot find DEMUX request.(ucDemuxId:%d)\n",ucDemuxId);
		return ERR_FAIL;
	}
	ulReqId = pstRequest->ulRequestId;

	/* MW doesn't have to consider if same pid channel exist on New DI */
	/* so, do nothing here */

	pal_sef_DecreaseFilterUsageCount (ucDemuxId);
	pstRequest->bUseFlag = FALSE;

	if(pstRequest->pstRawPackage != NULL)
	{
		pal_sef_FreePackageData(pstRequest->pstRawPackage);
		pstRequest->pstRawPackage = NULL;
	}
	palSef_LEAVE_CRITICAL;

	/* disable & free section filter */
	err = DI_DEMUX_ReleaseSectionFilter((HUINT32)ucDemuxId, ulReqId);
	if(err != DI_ERR_OK)
	{
		HxLOG_Error("Error in DI_DEMUX_ReleaseSectionFilter().\n");
		return ERR_FAIL;
	}

	HxLOG_Info("-- DISABLE SEF\n");

	return ERR_OK;
}

#define ______pal_sef_internal_functions________________________________

HERROR pal_sef_GetDemuxNum(HUINT8 *pucNumDev)
{
	*pucNumDev = (HUINT8)s_stSefContainer.ulNumOfTotalDemux;
	return ERR_OK;
}

void pal_sef_FreeDemuxSection(HUINT8 *pucSec)
{
	if(pucSec)
	{
//++ for_debugging +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifdef _PAL_SEF_ALLOC_FREE_DEBUG_
		HINT16		sLen;

		sLen = (HINT16)get12bit(&pucSec[1]);
		HxLOG_Print("- F %d\n", (HUINT32)((pucSec[sLen+3] << 24) | (pucSec[sLen+4] << 16) | (pucSec[sLen+5] << 8) | (pucSec[sLen+6])));
#endif
//------------------------------------------------------------------------------------------------------------------------------------
		pal_sef_FreeDemuxSection(pucSec);
		pucSec = NULL;
	}
}

void pal_sef_FreePackageData(PalSef_RawData_t *pstRawPackage)
{
	if(pstRawPackage)
	{
		HINT32	i;

		for(i=0; i<pstRawPackage->num_sec; i++)
		{
			pal_sef_FreeDemuxSection(pstRawPackage->buf[i]);
		}
		HLIB_STD_MemFree(pstRawPackage);
		pstRawPackage = NULL;
	}
}

#if defined(CONFIG_DEBUG)
void pal_sef_PrintContext(void)
{
	HINT32			i, j;
	palSef_Context_t	*pstContext;

	palSef_ENTER_CRITICAL;


	HLIB_CMD_Printf(C_CODE_F_CYAN);
	HLIB_CMD_Printf("s_stSefContainer.ulRequestIdSem       = %d\n", s_stSefContainer.ulRequestIdSem);
	HLIB_CMD_Printf("s_stSefContainer.ulNumOfTotalDemux    = %d\n", s_stSefContainer.ulNumOfTotalDemux);
	HLIB_CMD_Printf("s_stSefContainer.ulNumOfSefDemux      = %d\n", s_stSefContainer.ulNumOfSefDemux);
	for(i=0; i<s_stSefContainer.ulNumOfTotalDemux; i++)
	{
		pstContext = pal_sef_GetContext((HUINT8)i);
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


	palSef_LEAVE_CRITICAL;
}
#endif
#define ______local_functions______________________

STATIC void pal_sef_MakeRequestArray(HINT32 nNumOfFilter, palSef_DmxRequest_t **ppstReqHead)
{
	palSef_DmxRequest_t	*pHeadList;

	pHeadList = (palSef_DmxRequest_t *)HLIB_STD_MemAlloc(nNumOfFilter * sizeof(palSef_DmxRequest_t));
	HxSTD_memset(pHeadList, 0, nNumOfFilter * sizeof(palSef_DmxRequest_t));

	*ppstReqHead = pHeadList;
}

STATIC palSef_Context_t*	pal_sef_GetContext(HUINT8 ucDemuxId)
{
	if(ucDemuxId >= s_stSefContainer.ulNumOfTotalDemux)
		return NULL;

	return &s_stSefContainer.pstContext[ucDemuxId];
}

STATIC HBOOL pal_sef_IsFilterAvailable (HUINT8 ucDemuxId)
{
	palSef_Context_t			*pstContext = pal_sef_GetContext (ucDemuxId);

	if (NULL != pstContext)
	{
		if ((s_stSefContainer.nNumOfUsedSecFilter < s_stSefContainer.nNumOfTotalSecFilter) &&
			(pstContext->nRequestCount < pstContext->nSecFilterCapa))
		{
			return TRUE;
		}
	}

	return FALSE;
}

STATIC void pal_sef_IncreaseFilterUsageCount (HUINT8 ucDemuxId)
{
	palSef_Context_t			*pstContext = pal_sef_GetContext (ucDemuxId);

	if (NULL != pstContext)
	{
		s_stSefContainer.nNumOfUsedSecFilter ++;
		pstContext->nRequestCount ++;
	}
}

STATIC void pal_sef_DecreaseFilterUsageCount (HUINT8 ucDemuxId)
{
	palSef_Context_t			*pstContext = pal_sef_GetContext (ucDemuxId);

	if (NULL != pstContext)
	{
		s_stSefContainer.nNumOfUsedSecFilter --;
		pstContext->nRequestCount  --;
	}
}



/**********************************************************
 * FuncName : pal_sef_CheckExistRequestId
 * Description : pal_sef_GetRequest()�Լ��� PAL_SEF_DisableSectionFilter()
                       �� pal_sef_CbOnSection()�Լ��ȿ��� ��Ȯ�ϰ� Match�Ǵ�
                       RequestId�� ã�� ���ؼ� �ʿ��ϰ�.
                       �� �Լ��� MessageQueueId�� ���� Order�κ��� ���� ���ϴ�
                       �κ��� �ʿ��ؼ� �Լ��� ������ ��.
 **********************************************************/
STATIC palSef_DmxRequest_t *pal_sef_CheckExistRequestId(palSef_Context_t *pstContext, HUINT32 ulRequestId)
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

STATIC HUINT16 pal_sef_GetNextRequestId(palSef_Context_t *pstContext)
{
	STATIC HUINT16 		s_usDemuxRequestId = 0;
	palSef_DmxRequest_t 	*pstRequest;

	/* �Ҵ��Ϸ��� �ϴ� request_id�� filtering ���� id �̸� skip�ϰ� ���� �Ҵ��Ѵ� */
	do
	{
		s_usDemuxRequestId++;

		if (palSef_INVALID_REQUEST == s_usDemuxRequestId)
			s_usDemuxRequestId = 1;

		pstRequest = pal_sef_CheckExistRequestId(pstContext, s_usDemuxRequestId);
	} while (pstRequest);

	return s_usDemuxRequestId;
}

STATIC palSef_DmxRequest_t *pal_sef_GetRequest(palSef_Context_t *pstContext, HUINT32 ulRequestId)
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

STATIC palSef_DmxRequest_t *pal_sef_FindEmptyRequest(palSef_Context_t *pstContext)
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

STATIC HBOOL pal_sef_CheckData(PalSef_ReceiveMessage_t *pstRcvMsg, palSef_DmxRequest_t *pstRequest, HUINT8 *pucBuf)
{
	if(pstRequest->nSecCntInPackage > 1)
	{
		if(pstRequest->pstRawPackage == NULL)
		{
			pstRequest->pstRawPackage = (PalSef_RawData_t *)HLIB_STD_MemAlloc(sizeof(PalSef_RawData_t));
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
				HxLOG_Info("[pal_sef_CbOnSection] ulDuration(%d)num_sec(%d)\n",	VK_TIMER_GetSystemTick() - pstRequest->nStartTickInPackage, pstRequest->pstRawPackage->num_sec);
				pstRcvMsg->ulDataAddr = (HUINT32)pstRequest->pstRawPackage;
				pstRequest->pstRawPackage = NULL;
				pstRcvMsg->eSecPackageFlag = eSectionPackage_On;	/* package_flag : ON */
				return FALSE;
			}
			else
			{
				return TRUE; /* package ������ �� : err ok */
			}
		}
		else
		{
			HxLOG_Error("[pal_sef_CbOnSection:%d] Error : can't reach here\n", __LINE__);
			pal_sef_FreePackageData(pstRequest->pstRawPackage);
			pstRequest->pstRawPackage = NULL;
			return TRUE; /* package buffer �ʰ� : err fail */
		}
	}
	else
	{
		pstRcvMsg->ulDataAddr = (HUINT32)pucBuf;
		pstRcvMsg->eSecPackageFlag = eSectionPackage_Off;	/* package_flag : OFF */
		return FALSE; /* err ok */
	}
}

STATIC HERROR pal_sef_CheckEmergency(PalSef_ReceiveMessage_t *pstRcvMsg, PalSef_SendMessage_t *pstEmergencyMsg, HUINT32 *pulMsgQ, HBOOL *pbIsEmergency, HBOOL bEmergencyMsgSended)
{
	HUINT32			ulCurMsgCnt, ulMaxMsgCnt;
	HINT32			nErr;
	PalSef_RawData_t		*pstRawPackage;

	if((pstRcvMsg == NULL) || (pstEmergencyMsg == NULL) || (pulMsgQ == NULL) || (pbIsEmergency == NULL))
		return ERR_FAIL;

	*pbIsEmergency = FALSE;

	nErr = VK_MSG_GetMessageCount(*pulMsgQ, (HULONG *)&ulCurMsgCnt, (HULONG *)&ulMaxMsgCnt);
	if(nErr)
	{
		HxLOG_Error("Error - can't get msg_q info (q_id = 0x%x)\n", *pulMsgQ);
		return ERR_FAIL;
	}

	if(ulCurMsgCnt >= (ulMaxMsgCnt / 2)) /* 50% �̻��̸� nearly full */
	{
		/*
			section filter�� ulEmergencyMsgQ�� ���ؼ� stop�Ѵ�. semaphore deadlock ������ callback���� stop�� �� ����!!
			ulEmergencyMsgQ������ ���͸� stop�ϰ�, restart�� ���� ó���� �Ѵ�.
			sithd������ msg_q_cnt�� ����͸� �ϴٰ� �� ó���Ǹ� ���� filtering�� �����Ѵ�.
		*/
		HxLOG_Print("[WARN] si recv msgq - nearly full (%d/%d) (req_id=%d)\n", ulCurMsgCnt, ulMaxMsgCnt, pstRcvMsg->ulRequestId);
		//HxLOG_Error("\\#");

		if(bEmergencyMsgSended == TRUE)
		{ /* msg ������ �޴� ���� �� ���� �� �����Ƿ�, �׳� free�Ѵ� */
			return ERR_FAIL;
		}

		pstEmergencyMsg->eMsgClass = eSiFilterMsg_EmergencyStop;
		pstEmergencyMsg->ucDemuxId = pstRcvMsg->ucDemuxId;
		pstEmergencyMsg->msg.stEmergencyStop.ulRequestId = pstRcvMsg->ulRequestId;

		if(eSectionPackage_On == pstRcvMsg->eSecPackageFlag)
		{
			pstRawPackage = (PalSef_RawData_t *)pstRcvMsg->ulDataAddr;
			pal_sef_FreePackageData(pstRawPackage);
		}
		else
		{
			pal_sef_FreeDemuxSection((HUINT8 *)pstRcvMsg->ulDataAddr);
		}
		pstRcvMsg->ulDataAddr = (HUINT32)NULL;
		*pbIsEmergency = TRUE;
	}

	return ERR_OK;
}

STATIC DI_ERR_CODE pal_sef_CbOnSection(HUINT8 ucDemuxId, HUINT8 *pucBuf, HUINT32 ulRequestId)
{
	HERROR					 err;
	HBOOL					 bReturn, bIsEmergency;
	PalSef_SendMessage_t	 stEmergencyMsg;
	PalSef_ReceiveMessage_t	 stRcvMsg;
	HUINT32					 ulMsgQ, ulEmergencyMsgQ;
	HINT32					 nErr, nOrderIdx;
	palSef_DmxRequest_t 	*pstRequest;
	palSef_Context_t		*pstContext;

	if(pucBuf == NULL)
	{
		HxLOG_Error("[pal_sef_CbOnSection:%d] Error : pucBuf is NULL : ulRequestId = 0x%08X\n", ucDemuxId, ulRequestId);
		return DI_ERR_ERROR;
	}
//++ for_debugging +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifdef _PAL_SEF_ALLOC_FREE_DEBUG_
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

	pstContext = pal_sef_GetContext(ucDemuxId);

	if(pstContext == NULL)
	{
		pal_sef_FreeDemuxSection(pucBuf);
		return DI_ERR_OK;
	}

	pstRequest = pal_sef_GetRequest(pstContext, ulRequestId);
	if(pstRequest == NULL)
	{
		HxLOG_Print("[pal_sef_CbOnSection:%d] Error : Cannot find Demux : ulRequestId = 0x%08X\n", ucDemuxId, ulRequestId);
		pal_sef_FreeDemuxSection(pucBuf);
		return DI_ERR_OK;
	}

	bReturn = pal_sef_CheckData(&stRcvMsg, pstRequest, pucBuf);
	if(bReturn == TRUE)
		return DI_ERR_OK;

	stRcvMsg.ucTableId 		= pucBuf[0];
	stRcvMsg.ulRequestId 	= ulRequestId;
	stRcvMsg.err 			= ERR_OK;
	stRcvMsg.ucDemuxId 		= ucDemuxId;

	palSef_MAKE_ORDERIDX_FROM_REQUESTID(nOrderIdx, ulRequestId);
	ulMsgQ 			= pstContext->aulSendingMsgQid[nOrderIdx];
	ulEmergencyMsgQ	= pstRequest->ulEmergencyStopMsgQid;

	bIsEmergency = FALSE;
	/* emergency queue�� ������ ��츸 �ǹ� ���� */
	if(ulEmergencyMsgQ != 0)
	{
		err = pal_sef_CheckEmergency(&stRcvMsg, &stEmergencyMsg, &ulMsgQ, &bIsEmergency, pstRequest->bEmergencyMsgSended);
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
		nErr = VK_MSG_SendTimeout(ulMsgQ, &stEmergencyMsg, sizeof(PalSef_SendMessage_t), 0);
		HxLOG_Error("\n\t\t\t--> sent!!\n\n");
		if(nErr)
		{
			HxLOG_Error("\\$");
		}
	}
	else
	{
		nErr = VK_MSG_SendTimeout(ulMsgQ, &stRcvMsg, sizeof(PalSef_ReceiveMessage_t), 0);
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
		pal_sef_FreePackageData((PalSef_RawData_t *)stRcvMsg.ulDataAddr);
	}
	else
	{
		pal_sef_FreeDemuxSection(pucBuf);
	}

	return DI_ERR_OK; /* DI_ERR_ERROR ó���� ���� �ʴ� ������, ������ notify callback �ȿ��� section free���� �̷������ ����. ��, di���忡���� ���� ���� ���� */
}

STATIC DI_ERR_CODE pal_sef_CbOnRequestBuffer(HUINT16 usBufSize, HUINT8 **ppucBuf)
{
	*ppucBuf = NULL;
	if (usBufSize == 0) {
		HxLOG_Error("request buffer size is 0\n");
		return ERR_FAIL;
	}

#ifdef _PAL_SEF_ALLOC_FREE_DEBUG_
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

