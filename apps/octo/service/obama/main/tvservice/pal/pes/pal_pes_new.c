/**
	@file pal_pes.c
	@brief PES 패킷을 파싱한다.
    @section copyright	Copyright(c) 2010, Humax Co., Ltd. All rights reserved.
	@section module PAL/PES

	@remark Action마다 TTX, Subt, AribCC, UserDef 각각 CallBack를 저장한다.
		Action이 바뀌면, 파싱중인 Action의 상태가 Wait로 변경되고, 다음 Action의 필터링을
		시작한다.
		TTX, Subt는 여러개 필터를 걸 수 있는데, 구분은 reqId로 구분하다.
		MW에서 Action만큼 Callback를 걸어준다. Action이 바뀌면 변경한다.
		ReqId 관리와 Filter Add/Remove가 순서가 약간 어긋나 있음. - 의도한 것임
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
/** @brief global header file  */
#include <htype.h>
#include <vkernel.h>
#include <util.h>
#include <octo_common.h>
#include <di_demux.h>
#include <linkedlist.h>
/** @brief local header file  */
#include "pal_pes.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/



//#define PES_PACKET_START_CODE			0x000001BD
#define PES_PACKET_START_CODE			0x000001
#define PES_REQUEST_ID_MAX				0xffffffff

 /** @brief Linked list Macro */
#define MACRO_PES_GetContents(a)			UTIL_LINKEDLIST_GetContents(a)
#define MACRO_PES_IsList(a, b, c, d, e) 	UTIL_LINKEDLIST_IsListItemInPointerList(a, b, c, d, e)
#define MACRO_PES_GetNumList(a, b, c)		UTIL_LINKEDLIST_GetNumListItemsInPointerList(a, b, c)
#define MACRO_PES_FindNthList(a, b, c, d)	UTIL_LINKEDLIST_FindNthListItemFromPointerList(a, b, c, d)
#define MACRO_PES_AppendList(a, b)			UTIL_LINKEDLIST_AppendListItemToPointerList(a, b)
#define MACRO_PES_DeleteList(a, b, c)		UTIL_LINKEDLIST_DeleteListItemFromPointerList(a, b, c)
#define MACRO_PES_FreeWholeList(a, b)		UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(a, b)
#define MACRO_PES_FindList(a, b, c, d)		UTIL_LINKEDLIST_FindListItemFromPointerList(a, b, c, d)

#define PAL_PES_ConvertPALPesTypeToDIPesType(x)	((DI_DEMUX_PESTYPE)x)

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
/** @Brief PES state */
typedef enum
{
    ePAL_PES_NONE = 0,
    ePAL_PES_WAITING,
    ePAL_PES_RUNNING
} PalPes_Status_t;

typedef struct tagPAL_PES_REQUEST
{
    HUINT16		usPid;
    HUINT32		ulReqId;
} PalPes_Request_t;


typedef struct tagPalPes_Filter_t
{
	pfnPAL_PES_Notify	pfnCb;			/**< Callback함수 */
	POINTER_LIST_T		*pReqList;		/**< Pes Request list */
	HLONG				ulSema;
} PalPes_Filter_t;

typedef struct tagPalPes_context_t
{
	HUINT32				ulDemuxId;		/**< 할당된 Demux ID */
	PalPes_Status_t		eStatus;		/**< status */
	PalPes_Filter_t		aFilter[ePAL_PES_TYPE_MAX];		/**< Subtitle/AribCC, TTX, UserDefine용 Filter */
} PalPes_Context_t;

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static POINTER_LIST_T	*s_pPalPesDemuxList; /**< 배열로 하고 싶으나 MAX Demux 갯수를 시작전에 알수 없다. */

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

static DI_ERR_CODE pal_pes_CbFree(HUINT8 *pBuf);

static void *pal_pes_GetNthContents(POINTER_LIST_T *pRoot, HUINT32 ulIndex);

static void pal_pes_PrintRequestList(void)
{
	PalPes_Context_t	*pstDemux = NULL;
	POINTER_LIST_T 		*pReqList = NULL;
	HUINT32				i, j, k, uiNumOfReq, ulNumOfDemux = 0;
	PalPes_Request_t	*pReq = NULL;

	HxLOG_Error("%s() >\n", __FUNCTION__);

	ulNumOfDemux = MACRO_PES_GetNumList(s_pPalPesDemuxList, NULL, NULL);

	HxLOG_Error("Number of Demux is %d\n", ulNumOfDemux);

	for (k=0;k<ulNumOfDemux;k++)
	{
		pstDemux = pal_pes_GetNthContents(s_pPalPesDemuxList, k);
		if (!pstDemux)
		{
			HxLOG_Error("%s() Fail to find Demux Id=%d\n", __FUNCTION__, k);
			goto exit;
		}

		HxLOG_Error("Demux(%d) Status(%d) \n", pstDemux->ulDemuxId, pstDemux->eStatus);

		for (i=0; i<ePAL_PES_TYPE_MAX; i++)
		{
			pReqList = pstDemux->aFilter[i].pReqList;

			uiNumOfReq = MACRO_PES_GetNumList(pReqList, NULL, NULL);
			if (uiNumOfReq == 0)
			{
				continue;
			}

			HxLOG_Error("Type(%d=%p) reqNum(%d) ", i, pstDemux->aFilter[i].pfnCb, uiNumOfReq);
			for (j=0; j<uiNumOfReq; j++)
			{
				pReq = pal_pes_GetNthContents(pReqList, j);
				if (!pReq)
				{
					HxLOG_Error("%s() Fail to find Request Index=%d\n", __FUNCTION__, j);
					continue;
				}

				HxLOG_Error("Pid(0x%x) Req(0x%.8x) ", pReq->usPid, pReq->ulReqId);
			}
			HxLOG_Error("\n");
		}
	}

exit:
	HxLOG_Error("%s() <\n", __FUNCTION__);
	return;
}

static void pal_pes_FreeReq(PalPes_Request_t *pReq)
{
	HxLOG_Debug("%s() >\n", __FUNCTION__);
	OxMW_Free(pReq);
	HxLOG_Debug("%s() <\n", __FUNCTION__);
	return;
}

static HBOOL pal_pes_FindReq(PalPes_Request_t *pReq, HUINT32 ulReqId)
{
	HBOOL ret = FALSE;
	HxLOG_Debug("%s() >\n", __FUNCTION__);

	if (pReq == NULL)
	{
		HxLOG_Error("%s() PalPes Request is NULL\n", __FUNCTION__);
		goto exit;
	}

	if (pReq->ulReqId != ulReqId)
	{
		HxLOG_Info("%s() Fail to find reqid(0x%.8x)\n", __FUNCTION__, ulReqId);
		goto exit;
	}

	ret = TRUE;

exit:
	HxLOG_Debug("%s() <\n", __FUNCTION__);
	return ret;
}

static DI_ERR_CODE pal_pes_CheckPES(HUINT32 ulDemuxId, PalPes_Type_e ePesType
										, HUINT32 ulReqId, HUINT8 *pucBuf)
{
	DI_ERR_CODE 		dErr = DI_ERR_OK;
	HUINT32 			ulStartCode;
	PalPes_Context_t	*pstDemux = NULL;
	POINTER_LIST_T 		*pFound = NULL;
	POINTER_LIST_T 		*pReqList = NULL;
	PalPes_Request_t	*pReq = NULL;

	HxLOG_Debug("%s() >\n", __FUNCTION__);

    if (pucBuf == NULL)
	{
		HxLOG_Critical("%s() illegal parameter.\n", __FUNCTION__);
		dErr = DI_ERR_ERROR;
		goto exit;
	}

	pstDemux = pal_pes_GetNthContents(s_pPalPesDemuxList, ulDemuxId);
	if (!pstDemux)
	{
		HxLOG_Error("%s() Fail to find Demux Id=%d\n", __FUNCTION__, ulDemuxId);
		dErr = DI_ERR_ERROR;
		goto exit;
	}

	if (pstDemux->eStatus != ePAL_PES_RUNNING)
	{
		dErr = DI_ERR_ERROR;
		goto exit;
	}


	pReqList = pstDemux->aFilter[ePesType].pReqList;
	pFound = MACRO_PES_FindList(pReqList, NULL, (FILTER_FUNC)pal_pes_FindReq, (void*)ulReqId);
	if (pFound == NULL)
	{
		HxLOG_Error("%s() Fail to find RequestId=%d\n", __FUNCTION__, ulReqId);
		dErr = DI_ERR_ERROR;
		goto exit;
	}

	pReq = MACRO_PES_GetContents(pFound);
	if (!pReq)
	{
		HxLOG_Error("%s() Fail to get RequestId=%d\n", __FUNCTION__, ulReqId);
		dErr = DI_ERR_ERROR;
		goto exit;
	}

	//스펙 확인 by dhkim2
	//ulStartCode = get32bit(pPes);
	//if ( ulStartCode != PES_PACKET_START_CODE )
	ulStartCode = get24bit(pucBuf);
	if ( ulStartCode != PES_PACKET_START_CODE )
	{
		HxLOG_Error("%s() PES start code is invalid!!\n", __FUNCTION__);
		dErr = DI_ERR_ERROR;
		goto exit;
	}

exit:
	return dErr;

}

static DI_ERR_CODE pal_pes_CbSbtlNotify(HUINT32 ulDemuxId, HUINT8 *pucBuf
										, HUINT32 ulReqId)
{
	DI_ERR_CODE 		dErr = DI_ERR_OK;
	HUINT32				ulPesLen = 0;
	PalPes_Context_t	*pstDemux = NULL;

	HxLOG_Debug("%s() > reqid=0x%x\n", __FUNCTION__, ulReqId);
	dErr = pal_pes_CheckPES(ulDemuxId, ePAL_PES_TYPE_SBTL, ulReqId, pucBuf);
	if (dErr != DI_ERR_OK)
	{
		pal_pes_CbFree(pucBuf);
		pucBuf = NULL;
		goto exit;
	}

	pstDemux = pal_pes_GetNthContents(s_pPalPesDemuxList, ulDemuxId);

	ulPesLen = get16bit(pucBuf + 4) + 6/* PES Header size until length fields */;

	pstDemux->aFilter[ePAL_PES_TYPE_SBTL].pfnCb(ulDemuxId, ulReqId, ulPesLen, pucBuf);

exit:
	return dErr;
}

static DI_ERR_CODE pal_pes_CbTtxNotify(HUINT32 ulDemuxId, HUINT8 *pucBuf
										, HUINT32 ulReqId)
{
	DI_ERR_CODE dErr = DI_ERR_OK;
	HUINT32				ulPesLen = 0;
	PalPes_Context_t	*pstDemux = NULL;

	HxLOG_Debug("%s() >\n", __FUNCTION__);
	dErr = pal_pes_CheckPES(ulDemuxId, ePAL_PES_TYPE_TTX, ulReqId, pucBuf);
	if (dErr != DI_ERR_OK)
	{
		pal_pes_CbFree(pucBuf);
		pucBuf = NULL;
		goto exit;
	}

	pstDemux = pal_pes_GetNthContents(s_pPalPesDemuxList, ulDemuxId);

	ulPesLen = get16bit(pucBuf + 4) + 6/* PES Header size until length fields */;

	pstDemux->aFilter[ePAL_PES_TYPE_TTX].pfnCb(ulDemuxId, ulReqId, ulPesLen, pucBuf);

exit:
	return dErr;
}

static DI_ERR_CODE pal_pes_CbUserDefNotify(HUINT32 ulDemuxId, HUINT8 *pucBuf
										, HUINT32 ulReqId)
{
	DI_ERR_CODE dErr = DI_ERR_OK;
	HUINT32				ulPesLen = 0;
	PalPes_Context_t	*pstDemux = NULL;

	HxLOG_Debug("%s() >\n", __FUNCTION__);
	dErr = pal_pes_CheckPES(ulDemuxId, ePAL_PES_TYPE_USER_DEFINE, ulReqId, pucBuf);
	if (dErr != DI_ERR_OK)
	{
		pal_pes_CbFree(pucBuf);
		pucBuf = NULL;
		goto exit;
	}

	pstDemux = pal_pes_GetNthContents(s_pPalPesDemuxList, ulDemuxId);

	ulPesLen = get16bit(pucBuf + 4) + 6/* PES Header size until length fields */;

	pstDemux->aFilter[ePAL_PES_TYPE_USER_DEFINE].pfnCb(ulDemuxId, ulReqId, ulPesLen, pucBuf);

exit:
	return dErr;
}

static DI_ERR_CODE pal_pes_CbAllocate(HUINT32 ulSize, HUINT8 **ppBuf)
{
	DI_ERR_CODE dErr = DI_ERR_OK;

	HxLOG_Debug("%s() >\n", __FUNCTION__);
	if ( ppBuf != NULL )
	{
		*ppBuf = (HUINT8*)OxPAL_Malloc(ulSize);
		if (*ppBuf == NULL)
		{
			dErr = DI_ERR_NO_ALLOC;
			goto exit;
		}
	}
	else
	{
		dErr = DI_ERR_INVALID_PARAM;
	}

exit:
	return dErr;
}

static DI_ERR_CODE pal_pes_CbFree(HUINT8 *pBuf)
{
	DI_ERR_CODE dErr = DI_ERR_OK;
	HINT32 err = ERR_OK;

	HxLOG_Debug("%s() >\n", __FUNCTION__);

	if ( pBuf != NULL )
	{
		OxPAL_Free(pBuf);
		pBuf = NULL;
#if	0
		err = OxPAL_Free(pBuf);
		if (err)
		{
			dErr = DI_ERR_ERROR;
			goto exit;
		}
		pBuf=NULL;
#endif
	}
	else
	{
		dErr = DI_ERR_INVALID_PARAM;
	}

	HxLOG_Debug("%s() <\n", __FUNCTION__);
exit:
	return dErr;
}

static void *pal_pes_GetNthContents(POINTER_LIST_T *pRoot, HUINT32 ulIndex)
{
	void				*pRet = NULL;
	POINTER_LIST_T 		*pFound = NULL;

	HxLOG_Info("%s() >\n", __FUNCTION__);
	pFound = MACRO_PES_FindNthList(pRoot, ulIndex, NULL, NULL);
	if ( pFound == NULL )
	{
		HxLOG_Error("%s() Fail to alloc MEM\n", __FUNCTION__);
		pRet = NULL;
		goto exit;
	}

	pRet = MACRO_PES_GetContents(pFound);
	HxLOG_Print("pal_pes_GetNthContents() error... (%d)\n",pRet);

exit:
	return pRet;
}

HERROR pal_pes_AddRequest(PalPes_Context_t *pstDemux, PalPes_Type_e ePesType
										, HUINT16 usPid, HUINT32 ulReqId)
{
	HERROR      		hErr = ERR_OK;
	DI_ERR_CODE		dErr = DI_ERR_OK;
	DI_DEMUX_PESTYPE	diType;
	PalPes_Request_t	*pReq = NULL;

	HxLOG_Debug("%s() > ReqId=0x%x\n", __FUNCTION__, ulReqId);

	if (!pstDemux)
	{
		HxLOG_Error("%s() pstDemux is NULL\n", __FUNCTION__);
		hErr = ERR_FAIL;
		goto exit;
	}

	if (ePesType >= ePAL_PES_TYPE_MAX)
	{
		HxLOG_Error("%s() PES Type(%d) is unknown\n", __FUNCTION__, ePesType);
		hErr = ERR_FAIL;
		goto exit;
	}

	if (!pstDemux->aFilter[ePesType].pfnCb)
	{
		HxLOG_Error("%s() not set callback func for Type=%d\n", __FUNCTION__, ePesType);
		hErr = ERR_FAIL;
		goto exit;
	}
	pReq = (PalPes_Request_t *)OxPAL_Malloc(sizeof(PalPes_Request_t));
	if ( pReq == NULL )
	{
		HxLOG_Error("\n\n");
		goto exit;
	}

	HxSTD_memset(pReq, 0, sizeof(PalPes_Request_t));

	pReq->usPid = usPid;
	pReq->ulReqId = ulReqId;

	diType = PAL_PES_ConvertPALPesTypeToDIPesType(ePesType);
	hErr = DI_DEMUX_StartPESFilter (pstDemux->ulDemuxId, pReq->usPid
			, diType, pReq->ulReqId);
	if (dErr)
	{
		HxLOG_Error("%s() Fail to Start PES Filter (demuxId:%d, pid=0x%x, reqid=%d\n"
										, __FUNCTION__, pstDemux->ulDemuxId, pReq->usPid
										, pReq->ulReqId);
		OxPAL_Free(pReq);
		pReq = NULL;
		hErr = ERR_FAIL;
		goto exit;
	}

	VK_SEM_Get(pstDemux->aFilter[ePesType].ulSema);
	pstDemux->aFilter[ePesType].pReqList = MACRO_PES_AppendList
												(pstDemux->aFilter[ePesType].pReqList, pReq);
	VK_SEM_Release(pstDemux->aFilter[ePesType].ulSema);

	HxLOG_Debug("%s() < ReqId=0x%x\n", __FUNCTION__, pReq->ulReqId);
exit:
	return hErr;
}

HERROR pal_pes_SetCallBack(PalPes_Context_t *pstDemux, PalPes_Type_e ePesType
										, pfnPAL_PES_Notify pfnNotify)
{
	HERROR      hErr = ERR_OK;

	HxLOG_Debug("%s() >\n", __FUNCTION__);

	if (!pstDemux)
	{
		HxLOG_Error("%s() pstDemux is NULL\n", __FUNCTION__);
		hErr = ERR_FAIL;
		goto exit;
	}

	if (ePesType >= ePAL_PES_TYPE_MAX)
	{
		HxLOG_Error("%s() PES Type(%d) is unknown\n", __FUNCTION__, ePesType);
		hErr = ERR_FAIL;
		goto exit;
	}

	if (pstDemux->aFilter[ePesType].pfnCb && pstDemux->aFilter[ePesType].pfnCb != pfnNotify)
	{
		HxLOG_Error("%s change callback func for type(%d)\n"
				, __FUNCTION__, ePesType);
	}
	if (pfnNotify)
	{
		pstDemux->aFilter[ePesType].pfnCb = pfnNotify;
	}

exit:
	return hErr;
}


#define _________External_Function________
/*
   @brief PAL/PES를 초기화한다.
   @remark 총 사용할 수 있는 Demux 갯수를 가지고 와서 DemuxList를 생성한다.
*/
HERROR PAL_PES_Init(void)
{
	HERROR      		hErr = ERR_OK;
	DI_ERR_CODE		dErr = DI_ERR_OK;
	HUINT32				i, ulNumOfDemux;
	PalPes_Context_t	*pstDemux;

	HxLOG_Info("%s() <\n", __FUNCTION__);

	dErr = DI_DEMUX_GetNumOfDemux(&ulNumOfDemux);
	if (dErr)
	{
		HxLOG_Error("%s() Fail to get Number of Demux\n", __FUNCTION__);
		hErr = ERR_FAIL;
		goto exit;
	}

	for (i=0;i<ulNumOfDemux;i++)
	{
		pstDemux = OxMW_Calloc(sizeof(PalPes_Context_t));
		if (pstDemux == NULL)
		{
			HxLOG_Error("%s() Fail to alloc MEM\n", __FUNCTION__);
			hErr = ERR_FAIL;
			goto exit;
		}

		HxSTD_memset(pstDemux, 0, sizeof(PalPes_Context_t));

		pstDemux->ulDemuxId = i;
		pstDemux->eStatus = ePAL_PES_RUNNING;

		dErr = DI_DEMUX_RegisterPESCallback (pstDemux->ulDemuxId,
				DI_DEMUX_PESTYPE_SUBTITLE,
				(pfnDI_DEMUX_PES_Notify) pal_pes_CbSbtlNotify,
				(pfnDI_DEMUX_AllocBuffer) pal_pes_CbAllocate,
				(pfnDI_DEMUX_FreeBufferForError) pal_pes_CbFree);

		hErr =  VK_SEM_Create((unsigned long *)&(pstDemux->aFilter[ePAL_PES_TYPE_SBTL].ulSema)
										, "ReqListSema", VK_SUSPENDTYPE_PRIORITY);
		if (hErr)
		{
			HxLOG_Error("%s() Fail to create sema PesType(%d) for demux(%d)\n"
										, __FUNCTION__, ePAL_PES_TYPE_SBTL, i);
			OxPAL_Free(pstDemux);
			pstDemux = NULL;
			goto exit;
		}

		dErr = DI_DEMUX_RegisterPESCallback (pstDemux->ulDemuxId,
				DI_DEMUX_PESTYPE_TELETEXT,
				(pfnDI_DEMUX_PES_Notify) pal_pes_CbTtxNotify,
				(pfnDI_DEMUX_AllocBuffer) pal_pes_CbAllocate,
				(pfnDI_DEMUX_FreeBufferForError) pal_pes_CbFree);
		hErr =  VK_SEM_Create((unsigned long *)&(pstDemux->aFilter[ePAL_PES_TYPE_TTX].ulSema)
										, "ReqListSema", VK_SUSPENDTYPE_PRIORITY);
		if (hErr)
		{
			HxLOG_Error("%s() Fail to create sema PesType(%d) for demux(%d)\n"
										, __FUNCTION__, ePAL_PES_TYPE_TTX, i);
			OxPAL_Free(pstDemux);
			pstDemux = NULL;
			goto exit;
		}

		dErr = DI_DEMUX_RegisterPESCallback (pstDemux->ulDemuxId,
				DI_DEMUX_PESTYPE_USERDEFINE,
				(pfnDI_DEMUX_PES_Notify) pal_pes_CbUserDefNotify,
				(pfnDI_DEMUX_AllocBuffer) pal_pes_CbAllocate,
				(pfnDI_DEMUX_FreeBufferForError) pal_pes_CbFree);
		hErr =  VK_SEM_Create((unsigned long *)&(pstDemux->aFilter[ePAL_PES_TYPE_USER_DEFINE].ulSema)
										, "ReqListSema", VK_SUSPENDTYPE_PRIORITY);
		if (hErr)
		{
			HxLOG_Error("%s() Fail to create sema PesType(%d) for demux(%d)\n"
										, __FUNCTION__, ePAL_PES_TYPE_USER_DEFINE, i);
			OxPAL_Free(pstDemux);
			pstDemux = NULL;
			goto exit;
		}


		s_pPalPesDemuxList = MACRO_PES_AppendList(s_pPalPesDemuxList, pstDemux);
	}

exit:
	HxLOG_Info("%s() >\n", __FUNCTION__);
	return hErr;
}


/*
   @brief PAL/PES를 초기화한다.
   @remark 실제로 사용 될 일은 없다. Init에서 Demux를 위해 Alloc한 메모리를 해제한다.
 */
HERROR PAL_PES_Destory(void)
{
	HERROR      		hErr = ERR_OK;
	HUINT32				i, ulNumOfDemux;
	PalPes_Context_t	*pstDemux = NULL;

	HxLOG_Debug("%s() >\n", __FUNCTION__);

	ulNumOfDemux = MACRO_PES_GetNumList(s_pPalPesDemuxList, NULL, NULL);
	for (i=0;i<ulNumOfDemux;i++)
	{
		pstDemux = pal_pes_GetNthContents(s_pPalPesDemuxList, i);

		hErr = PAL_PES_RemoveAllFilterByDemux(pstDemux->ulDemuxId);
		HxLOG_Print("PAL_PES_Destory() error... (%d)\n",hErr);
	}

	s_pPalPesDemuxList = MACRO_PES_FreeWholeList(s_pPalPesDemuxList
										, (FREE_FUNC)pal_pes_CbFree);
	return hErr;
}

/*
   @remark
   			한 Demux에 여러개의 Task가 Add, Remove하기 때문에 Semapore를 없애기 위해선
			Task(Instnace, Action)별로 두어야 한다. by dhkim2
*/

HERROR PAL_PES_AddFilter(HUINT32 ulDemuxId, PalPes_Type_e ePesType, HUINT16 usPid
										, HUINT32 ulReqId, pfnPAL_PES_Notify pfnNotify)
{
	HERROR      hErr = ERR_OK;
	PalPes_Context_t	*pstDemux = NULL;

	HxLOG_Debug("%s() > reqid=0x%.8x\n", __FUNCTION__, ulReqId);

	pstDemux = pal_pes_GetNthContents(s_pPalPesDemuxList, ulDemuxId);
	if (!pstDemux)
	{
		HxLOG_Error("%s() Fail to find Demux Id=%d\n", __FUNCTION__, ulDemuxId);
		hErr = ERR_FAIL;
		goto exit;
	}

	if (pstDemux->eStatus != ePAL_PES_RUNNING)
	{
		HxLOG_Error("%s() Demux(id=%d) is not RUNNING.\n", __FUNCTION__, ulDemuxId);
		hErr = ERR_FAIL;
		goto exit;
	}

	if (ePesType >= ePAL_PES_TYPE_MAX)
	{
		HxLOG_Error("%s() PES Type(%d) is unknown\n", __FUNCTION__, ePesType);
		hErr = ERR_FAIL;
		goto exit;
	}

	hErr = pal_pes_SetCallBack(pstDemux, ePesType, pfnNotify);
	if (hErr)
	{
		goto exit;
	}

	hErr = pal_pes_AddRequest(pstDemux, ePesType, usPid, ulReqId);
	if (hErr)
	{
		goto exit;
	}

exit:
	if (hErr == ERR_FAIL)
	{
		pal_pes_PrintRequestList();
	}
	return hErr;
}

/*
   @remark
   				Add와 Remove는 한 task에서 이루어지기 때문에 Find까지 세마포어에 포함 할
				필요가 없다.
*/

HERROR PAL_PES_RemoveFilter(HUINT32 ulDemuxId, PalPes_Type_e ePesType, HUINT32 ulReqId)
{
	HERROR      		hErr = ERR_OK;
	DI_ERR_CODE			dErr = DI_ERR_OK;
	PalPes_Context_t	*pstDemux = NULL;
	POINTER_LIST_T 		*pFound = NULL;
	POINTER_LIST_T 		*pReqList = NULL;

	HxLOG_Debug("%s ( demuxid=%d, reqid=0x%.8x )\n", __FUNCTION__, ulDemuxId, ulReqId);

	pstDemux = pal_pes_GetNthContents(s_pPalPesDemuxList, ulDemuxId);
	if (!pstDemux)
	{
		HxLOG_Error("%s() Fail to find Demux Id=%d\n", __FUNCTION__, ulDemuxId);
		hErr = ERR_FAIL;
		goto exit;
	}

	if (ePesType >= ePAL_PES_TYPE_MAX)
	{
		HxLOG_Error("%s() PES Type(%d) is unknown\n", __FUNCTION__, ePesType);
		hErr = ERR_FAIL;
		goto exit;
	}

	dErr = DI_DEMUX_StopPESFilter (pstDemux->ulDemuxId, ulReqId);
	if ( dErr )
	{
		HxLOG_Error("%s() Fail to Stop PES Filter DemuxId=%d, ReqId=0x%.8x err_code(0x%x)\n", __FUNCTION__, pstDemux->ulDemuxId, ulReqId, dErr);
		hErr = ERR_FAIL;
		goto exit;
	}

	HxLOG_Info("%s() Stop PES Filter DemuxId=%d, ReqId=0x%.8x\n", __FUNCTION__, pstDemux->ulDemuxId, ulReqId);

	VK_SEM_Get(pstDemux->aFilter[ePesType].ulSema);

	pReqList = pstDemux->aFilter[ePesType].pReqList;

	pFound = MACRO_PES_FindList(pReqList, NULL, (FILTER_FUNC)pal_pes_FindReq
										, (void*)ulReqId);
	if (pFound == NULL)
	{
		HxLOG_Error("%s() Fail to find RequestId=%d\n", __FUNCTION__, ulReqId);
		hErr = ERR_FAIL;
		goto exit;
	}

	pstDemux->aFilter[ePesType].pReqList = MACRO_PES_DeleteList(pReqList, pFound
										, (FREE_FUNC) pal_pes_FreeReq);

	VK_SEM_Release(pstDemux->aFilter[ePesType].ulSema);

exit:
	if ( dErr )
	{
		pal_pes_PrintRequestList();
	}
	return hErr;
}

HERROR PAL_PES_RemoveAllFilterByType(HUINT32 ulDemuxId, PalPes_Type_e ePesType)
{
	HERROR      		hErr = ERR_OK;

	HxLOG_Debug("%s() >\n", __FUNCTION__);
	return hErr;
}

/*
   @brief Demux에 할당된 모든 request를 정지 시키고, 삭제한다.
*/
HERROR PAL_PES_RemoveAllFilterByDemux(HUINT32 ulDemuxId)
{
	HERROR      		hErr = ERR_OK;

	HxLOG_Debug("%s() >\n", __FUNCTION__);
	return hErr;
}

/*
   @brief Demux에 할당된 모든 request를 정지 시킨다.
*/
HERROR PAL_PES_PauseDemux(HUINT32 ulDemuxId)
{
	HERROR      		hErr = ERR_OK;
	DI_ERR_CODE			dErr = DI_ERR_OK;
	PalPes_Context_t	*pstDemux = NULL;
	POINTER_LIST_T 		*pReqList = NULL;
	HUINT32				i, j, uiNumOfReq = 0;
	PalPes_Request_t	*pReq = NULL;

	HxLOG_Debug("%s() >\n", __FUNCTION__);

	pstDemux = pal_pes_GetNthContents(s_pPalPesDemuxList, ulDemuxId);
	if (!pstDemux)
	{
		HxLOG_Error("%s() Fail to find Demux Id=%d\n", __FUNCTION__, ulDemuxId);
		hErr = ERR_FAIL;
		goto exit;
	}

	if (pstDemux->eStatus != ePAL_PES_RUNNING)
	{
		HxLOG_Error("%s() Demux(id=%d) is not RUNNING.\n", __FUNCTION__, ulDemuxId);
		hErr = ERR_FAIL;
		goto exit;
	}

	for (i=0; i<ePAL_PES_TYPE_MAX; i++)
	{
		pReqList = pstDemux->aFilter[i].pReqList;
		uiNumOfReq = MACRO_PES_GetNumList(pReqList, NULL, NULL);
		for (j=0; j<uiNumOfReq; j++)
		{
			pReq = pal_pes_GetNthContents(pReqList, j);
			if (!pReq)
			{
				HxLOG_Error("%s() Fail to find Request Index=%d\n", __FUNCTION__, j);
				continue;
			}

			dErr = DI_DEMUX_StopPESFilter (pstDemux->ulDemuxId, pReq->ulReqId);
			if ( dErr )
			{
				HxLOG_Error("%s() Error(0x%x) from DI_DEMUX_StopPESFilter\n", __FUNCTION__
							, dErr);
			}
		}
	}

exit:
	return hErr;
}

/*
   @brief Demux에 할당된 모든 request를 다시 가동한다.
*/
HERROR PAL_PES_ResumeDemux(HUINT32 ulDemuxId)
{
	HERROR      		hErr = ERR_OK;
	DI_ERR_CODE			dErr = DI_ERR_OK;
	PalPes_Context_t	*pstDemux = NULL;
	POINTER_LIST_T 		*pReqList = NULL;
	HUINT32				uiNumOfReq = 0;
	PalPes_Request_t	*pReq = NULL;
	DI_DEMUX_PESTYPE	diType;
	HUINT32				i, j;

	HxLOG_Debug("%s() >\n", __FUNCTION__);

	pstDemux = pal_pes_GetNthContents(s_pPalPesDemuxList, ulDemuxId);
	if (!pstDemux)
	{
		HxLOG_Error("%s() Fail to find Demux Id=%d\n", __FUNCTION__, ulDemuxId);
		hErr = ERR_FAIL;
		goto exit;
	}

	if (pstDemux->eStatus != ePAL_PES_WAITING)
	{
		HxLOG_Error("%s() Demux(id=%d) is not Waiting.\n", __FUNCTION__, ulDemuxId);
		hErr = ERR_FAIL;
		goto exit;
	}

	for (i=0; i<ePAL_PES_TYPE_MAX; i++)
	{
		pReqList = pstDemux->aFilter[i].pReqList;
		uiNumOfReq = MACRO_PES_GetNumList(pReqList, NULL, NULL);
		for (j=0; j<uiNumOfReq; j++)
		{
			pReq = pal_pes_GetNthContents(pReqList, j);

			diType = PAL_PES_ConvertPALPesTypeToDIPesType(i);

			dErr = DI_DEMUX_StartPESFilter (pstDemux->ulDemuxId, pReq->usPid
					, diType, pReq->ulReqId);
			if ( dErr )
			{
				HxLOG_Error("%s() from DI_DEMUX_StopPESFilter rt=%d\n", __FUNCTION__
							, dErr);
			}
		}
	}


exit:
	return hErr;
}

/*
   @brief PES Buffer를 삭제한다.
 */

HERROR PAL_PES_FreePesPacket(HUINT8 *pBuf)
{
	HERROR hErr = ERR_OK;
	DI_ERR_CODE diErr = DI_ERR_OK;

	HxLOG_Debug("%s() >\n", __FUNCTION__);
	diErr = pal_pes_CbFree(pBuf);
	if (diErr)
	{
		hErr = ERR_FAIL;
	}

	return hErr;
}

HERROR PAL_PES_GetSTC(HUINT32 ulDemuxId, HUINT32 *pStc)
{
	HERROR				retError = ERR_OK;
	DI_ERR_CODE			diErr;
	HUINT32 			ulHighBit, ulLowBit;

	HxLOG_Debug("%s() >\n", __FUNCTION__);
	if (pStc == NULL)
	{
		HxLOG_Error("%s() input parameter is NULL\n", __FUNCTION__);
		retError = ERR_FAIL;
		goto exit;
	}

	diErr = DI_DEMUX_GetSTC(ulDemuxId, &ulHighBit, &ulLowBit/* Always, this value is 0 by DI */);
	if (diErr == DI_ERR_OK)
	{
		*pStc = ulLowBit;
	}
	else
	{
		*pStc = 0;
		retError = ERR_FAIL;
		HxLOG_Error("%s() Fail to get STC diErr=%d\n", __FUNCTION__, diErr);
		goto exit;
	}

exit:
	HxLOG_Debug("%s() <\n", __FUNCTION__);
	return retError;

}


void PAL_PES_SetLogLevel(HUINT32	level)
{
	HxLOG_Debug("%s(level=%d)\n", __FUNCTION__, level);
	return;
}


void PAL_PES_PrintRequestList(void)
{
	pal_pes_PrintRequestList();

	return;
}

void PAL_PES_PrintStc(HUINT32 ulDemuxId)
{
	HUINT32 stc;

	PAL_PES_GetSTC(ulDemuxId, &stc);
	HxLOG_Error("%s() Demux=%d stc=%#x\n", __FUNCTION__, ulDemuxId, stc);
	return;
}

void pal_pes_CMD_Cb(HUINT32 ulDemuxId, HUINT32 ulReqId, HUINT16 usBufLen, HUINT8 *pBuf)
{
	HxLOG_Error("%s() Demux=%d, Reqid=%d, buflen=%d, buf=0x%p\n"
										, __FUNCTION__, ulDemuxId, ulReqId, usBufLen, pBuf);
	return;
}

void PAL_PES_CMD_AddFilter(HUINT32 reqid, HUINT32 pesPid)
{
	HxLOG_Debug("%s() >\n", __FUNCTION__);
	PAL_PES_AddFilter(0, ePAL_PES_TYPE_SBTL, pesPid, reqid, (pfnPAL_PES_Notify)pal_pes_CMD_Cb);
	HxLOG_Debug("%s() <\n", __FUNCTION__);

	return;
}

void PAL_PES_CMD_RemoveFilter(HUINT32 reqid)
{
	HxLOG_Debug("%s() >\n", __FUNCTION__);
	PAL_PES_RemoveFilter(0, ePAL_PES_TYPE_SBTL, reqid);
	HxLOG_Debug("%s() <\n", __FUNCTION__);

	return;
}


