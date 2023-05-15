/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pal_pes.c
	@brief

	Description:
	Module:

	Copyright (c) 2008 HUMAX Co., Ltd.			\n
	All rights reserved.						\n
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
#include <di_video.h>
#include <linkedlist.h>
/** @brief local header file  */
#include "pal_pes.h"
/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/



#define palPes_PACKET_START_CODE			0x000001BD
#define palPes_LIMIT_TIME					20 * 45000	/* 45khz : 20초  10초 -> 20 초 DVB-Subtitle on MediaPB 에서는 subt이 12.6초 이상 빨리와서 늘림.  */


#define palPes_SBTL_CMD_TIME_OUT			1000			/* 1000 msec */
#define palPes_SBTL_MAX_LIST_NUM			40	/* 20 -> 40 : limit time을 20초로 늘려서 같이 증가 시켜줌.  */


#define palPes_TTX_CMD_TIME_OUT			1000			/* 1000 msec */
#define palPes_TTX_MAX_LIST_NUM			2000 /* 1000 -> 2000 : limit time을 20초로 늘려서 같이 증가 시켜줌.*/

#define palPes_REQUEST_ID_MAX				0xffffffff
/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/** @brief Packet Info */
typedef struct tagPAL_PES_LIST
{
    HUINT32		ulPts;
    HUINT32		ulLen;
    HUINT8		*pBuf;
} palPes_List_t;


/** @Brief PES state */
typedef enum
{
    ePES_DISABLED = 0,
    ePES_ENABLED
} palPes_STATUS_e;

typedef enum
{
	ePAL_PES_TYPE_SBTL = 0,
	ePAL_PES_TYPE_TTX,
	ePAL_PES_TYPE_USER_DEFINE
} palPes_Type_e;

typedef struct
{
	HUINT32			ulCmd;			/**< Command. union상에서의 공통 위치. */
	Handle_t		hAction;		/**< Command를 발행한 action의 handle. */
	HUINT8			*pucBuf;		/**< PES data */
} palPes_ProcessPkt_t;


/** @Brief PAL PES Message Structure */
typedef union
{
	HUINT32				ulCmd;		/**< Command. union상에서의 공통 위치. */
	palPes_ProcessPkt_t	prosPktMsg;
} palPes_Msg_t;


/** @brief : Task Command */
enum
{
	PESM_PROCESS_PACKET = 100,		/**< 도착했을을 알리는 명령. */

	CMD_PAL_PES_END					/**< enum 마지막 표시. command 아님. */
};


/** @Brief Teletext Info structure */
typedef struct TTX_INFO
{
	Handle_t		hAction;		/**< service와 data sync로 사용 */
	palPes_STATUS_e		status;			/**< status */
	HUINT32			ulDemuxId;		/**< 사용중인 Demux ID */
	HUINT32			ulRqId;			/**< DI에 요구한 Request ID(DI와 sync로 사용) */
	POINTER_LIST_T	*pPesList;		/**< Pes packet list */
} palPes_TtxInfo_t;


/** @Brief Subtitle Info structure */
typedef struct tagSBTL_INFO
{
	Handle_t		hAction;		/**< Data sync로 사용(Demux의 request ID로도 사용됨) */
	palPes_STATUS_e		status;			/**< status */
	HUINT32			ulDemuxId;		/**< 사용중인 Demux ID */
	HUINT32			ulRqId;			/**< DI에 요구한 Request ID(DI와 sync로 사용) */
	POINTER_LIST_T	*pPesList;		/**< Pes packet list */
} palPes_SbtlInfo_t;


/** @Brief User Define Info structure */
typedef struct tagUSER_DEF_INFO
{
	palPes_STATUS_e		status;			/**< status */
	HUINT32			ulDemuxId;		/**< 사용중인 Demux ID */
	HUINT32			ulRqId;			/**< DI에 요구한 Request ID(DI와 sync로 사용) */
} palPes_UserDefInfo_t;


/** @Brief PAL PES Info structure */
typedef struct tagPES_INFO
{
	palPes_SbtlInfo_t		stSbtlInfo;		/**< Subtitle Info Structure. */
	palPes_TtxInfo_t		stTtxInfo;		/**< Teletext Info Structure. */
	palPes_UserDefInfo_t	stUserDefInfo;	/**< UserDef Info Structure. */
} palPes_Info_t;


 /** @brief Linked list Macro */
#define palPes_GetContents(a)			UTIL_LINKEDLIST_GetContents(a)
#define palPes_IsList(a, b, c, d, e) 	UTIL_LINKEDLIST_IsListItemInPointerList(a, b, c, d, e)
#define palPes_GetNumList(a, b, c)		UTIL_LINKEDLIST_GetNumListItemsInPointerList(a, b, c)
#define palPes_FindNthList(a, b, c, d)	UTIL_LINKEDLIST_FindNthListItemFromPointerList(a, b, c, d)
#define palPes_AppendList(a, b)			UTIL_LINKEDLIST_AppendListItemToPointerList(a, b)
#define palPes_DeleteList(a, b, c)		UTIL_LINKEDLIST_DeleteListItemFromPointerList(a, b, c)
#define palPes_FreeWholeList(a, b)		UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(a, b)

#define palPes_SBTL_CRITICAL_ENTER		VK_SEM_Get(s_ulPalSbtlSyncSema)
#define palPes_SBTL_CRITICAL_LEAVE		VK_SEM_Release(s_ulPalSbtlSyncSema)
#define palPes_TTX_CRITICAL_ENTER		VK_SEM_Get(s_ulPalTtxSyncSema)
#define palPes_TTX_CRITICAL_LEAVE		VK_SEM_Release(s_ulPalTtxSyncSema)
#define palPes_USER_CRITICAL_ENTER		VK_SEM_Get(s_ulPalUserSyncSema)
#define palPes_USER_CRITICAL_LEAVE		VK_SEM_Release(s_ulPalUserSyncSema)
#define palPes_RQ_CRITICAL_ENTER		VK_SEM_Get(s_ulPalResRqSema)
#define palPes_RQ_CRITICAL_LEAVE		VK_SEM_Release(s_ulPalResRqSema)

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HULONG		s_ulPalSbtlTaskId;				/**< Pal Task Id */
STATIC HULONG		s_ulPalSbtlMsgQId;				/**< Message Queue Id */
STATIC HULONG		s_ulPalSbtlSyncSema;			/**< Sync용 Sema */
STATIC PalPes_SBTL_Notify_t	s_pfnCbSubtitle;		/**< Subtitle Callback. */

STATIC HULONG		s_ulPalTtxTaskId;				/**< TTX Handler Task ID */
STATIC HULONG		s_ulPalTtxMsgQId;				/**< Message Queue Id */
STATIC HULONG		s_ulPalTtxSyncSema;				/**< Sync용 Sema */
STATIC PalPes_TTX_Notify_t	s_pfnCbTeletext;		/**< Teletext Callback. */

STATIC HULONG		s_ulPalUserSyncSema;			/**< Sync용 Sema */
STATIC PalPes_USERDEF_Notify_t	s_pfncbUserDef;		/**< User define Callback. */

STATIC HULONG		s_ulPalResRqSema;				/**< Sync용 Sema */
STATIC HUINT32		s_uPalPesRequestId;				/**< Request ID. */
STATIC palPes_Info_t	s_stPalPesContent;				/**< PES Info Content. */
/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
#define ________Internal_Function________
STATIC void pal_pes_DeletePesPacketFromList(palPes_List_t *pPkt)
{
	if ( pPkt->pBuf != NULL )
	{
		OxMW_Free(pPkt->pBuf);
	}

	OxMW_Free(pPkt);
}


STATIC INLINE palPes_SbtlInfo_t *pal_pes_GetSbtlContents(void)
{
	return &s_stPalPesContent.stSbtlInfo;
}


STATIC INLINE palPes_TtxInfo_t *pal_pes_GetTtxContents(void)
{
	return &s_stPalPesContent.stTtxInfo;
}


STATIC INLINE palPes_UserDefInfo_t *pal_pes_GetUserDefContents(void)
{
	return &s_stPalPesContent.stUserDefInfo;
}


STATIC INLINE HUINT32 pal_pes_GetRequestId(void)
{
	HUINT32		rqId;

	palPes_RQ_CRITICAL_ENTER;

	if ( s_uPalPesRequestId == palPes_REQUEST_ID_MAX )
	{
		s_uPalPesRequestId = 0;
	}

	s_uPalPesRequestId++;

	rqId = s_uPalPesRequestId;

	palPes_RQ_CRITICAL_LEAVE;

	return rqId;
}

STATIC INLINE void pal_pes_Convert45kHzPCR(HUINT32 ulHighBit, HUINT32 ulLowBit, HUINT32 *ulPcr)
{
	*ulPcr = (HUINT32)(((ulHighBit << 31) & 0x80000000) | ((ulLowBit >> 1) & 0x7fffffff));
}

STATIC HBOOL pal_pes_IsOnTimePesPacket(palPes_List_t *pPkt, HUINT32 ulDemuxId)
{
	DI_ERR_CODE diErr;
	HUINT32		ulHighBit, ulLowBit;
	HUINT32		ulPtsOrPcr = 0;

	/* Get Video PTS */
	diErr = DI_VIDEO_GetVideoPts(&ulPtsOrPcr);
	if(diErr != DI_ERR_OK || ulPtsOrPcr == 0)
	{
		/* Get STC(PCR) */
#if defined(CONFIG_SUPPORT_FCC)
		diErr = DI_DEMUX_GetPrimerSTC(&ulHighBit, &ulLowBit);
#else
		diErr = DI_DEMUX_GetSTC(ulDemuxId, &ulHighBit, &ulLowBit);
#endif
		if(diErr == DI_ERR_OK)
		{
			pal_pes_Convert45kHzPCR(ulHighBit, ulLowBit, &ulPtsOrPcr);
		}
		else
		{
			ulPtsOrPcr = 0;
		}
	}

	/* STC가 0일 경우 무조건 처리한다. 처리하지 않으면 계속 쌓일 수도 있음. */
	if ( (pPkt->ulPts <= ulPtsOrPcr)							/* Normal case */
		|| (pPkt->ulPts == 0)								/* No PTS. */
		|| (ulPtsOrPcr == 0)								/* No Video PTS or STC. */
		|| (pPkt->ulPts -ulPtsOrPcr > palPes_LIMIT_TIME))		/* 시간 차가 클 경우 그냥 Pass. */
	{
	    return 0;
	}

	return ((pPkt->ulPts - ulPtsOrPcr) / 45) + 1;
}


STATIC void pal_pes_AddPesPacket(palPes_Type_e eType, palPes_List_t *pPacket)
{
	palPes_SbtlInfo_t	*pSbtl;
	palPes_TtxInfo_t	*pTtx;

	switch (eType)
	{
		case ePAL_PES_TYPE_SBTL :
			pSbtl = (palPes_SbtlInfo_t *)pal_pes_GetSbtlContents();
			pSbtl->pPesList = palPes_AppendList(pSbtl->pPesList, pPacket);
			break;

		case ePAL_PES_TYPE_TTX :
			pTtx = (palPes_TtxInfo_t *)pal_pes_GetTtxContents();
			pTtx->pPesList = palPes_AppendList(pTtx->pPesList, pPacket);
			break;

		default :
			HxLOG_Critical("\n\n");
			return;
	}
}


STATIC void pal_pes_FreeAllPesPacket(palPes_Type_e eType)
{
	palPes_SbtlInfo_t	*pSbtl;
	palPes_TtxInfo_t	*pTtx;

	switch (eType)
	{
		case ePAL_PES_TYPE_SBTL :
			pSbtl = (palPes_SbtlInfo_t *)pal_pes_GetSbtlContents();
			pSbtl->pPesList = palPes_FreeWholeList(pSbtl->pPesList, (FREE_FUNC)pal_pes_DeletePesPacketFromList);
			break;

		case ePAL_PES_TYPE_TTX :
			pTtx = (palPes_TtxInfo_t *)pal_pes_GetTtxContents();
			pTtx->pPesList = palPes_FreeWholeList(pTtx->pPesList, (FREE_FUNC)pal_pes_DeletePesPacketFromList);
			break;

		default :
			break;
	}
}


STATIC void pal_pes_SetPesInfo(palPes_Type_e eType, Handle_t hAction, palPes_STATUS_e eStatus, HUINT32 ulDemuxId, HUINT32 ulRqId)
{
	palPes_SbtlInfo_t		*pSbtl = NULL;
	palPes_TtxInfo_t		*pTtx = NULL;
	palPes_UserDefInfo_t	*pUserDef = NULL;

	switch (eType)
	{
		case ePAL_PES_TYPE_SBTL :
			pSbtl = (palPes_SbtlInfo_t *)pal_pes_GetSbtlContents();
			pSbtl->hAction = hAction;
			pSbtl->status = eStatus;
			pSbtl->ulDemuxId = ulDemuxId;
			pSbtl->ulRqId = ulRqId;
			break;

		case ePAL_PES_TYPE_TTX :
			pTtx = (palPes_TtxInfo_t *)pal_pes_GetTtxContents();
			pTtx->hAction = hAction;
			pTtx->ulDemuxId = ulDemuxId;
			pTtx->ulRqId = ulRqId;
			break;

		case ePAL_PES_TYPE_USER_DEFINE :
			pUserDef = (palPes_UserDefInfo_t *)pal_pes_GetUserDefContents();
			pUserDef->status = eStatus;
			pUserDef->ulDemuxId = ulDemuxId;
			pUserDef->ulRqId = ulRqId;
			break;

		default :
			HxLOG_Critical("\n\n");
			return;
	}
}


STATIC void pal_pes_ResetPesInfo(palPes_Type_e eType)
{
	palPes_SbtlInfo_t		*pSbtl = NULL;
	palPes_TtxInfo_t		*pTtx = NULL;
	palPes_UserDefInfo_t	*pUserDef = NULL;

	switch (eType)
	{
		case ePAL_PES_TYPE_SBTL :
			pSbtl = (palPes_SbtlInfo_t *)pal_pes_GetSbtlContents();
			pSbtl->hAction = HANDLE_NULL;
			pSbtl->status = ePES_DISABLED;
			pSbtl->ulDemuxId = 0xFF;
			pSbtl->ulRqId = 0;
			break;

		case ePAL_PES_TYPE_TTX :
			pTtx = (palPes_TtxInfo_t *)pal_pes_GetTtxContents();
			pTtx->hAction = HANDLE_NULL;
			pTtx->status = ePES_DISABLED;
			pTtx->ulDemuxId = 0xFF;
			pTtx->ulRqId = 0;
			break;

		case ePAL_PES_TYPE_USER_DEFINE :
			pUserDef = (palPes_UserDefInfo_t *)pal_pes_GetUserDefContents();
			pUserDef->status = ePES_DISABLED;
			pUserDef->ulDemuxId = 0xFF;
			pUserDef->ulRqId = 0;
			break;

		default :
			HxLOG_Critical("\n\n");
			return;
	}

	pal_pes_FreeAllPesPacket(eType);
}

STATIC DI_ERR_CODE pal_pes_CbAllocate(HUINT32 ulSize, HUINT8 **ppBuf)
{
	if ( ppBuf != NULL )
	{
		*ppBuf = (HUINT8*)OxMW_Malloc(ulSize);
		if (*ppBuf != NULL)
		{
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}


STATIC DI_ERR_CODE pal_pes_CbErrorFree(HUINT8 *pBuf)
{
	if ( pBuf != NULL )
	{
		OxMW_Free (pBuf);
	}

	return ERR_OK;
}


#define _________SBTL_Function________
STATIC DI_ERR_CODE pal_pes_CbSbtlPesNotify(HUINT32 ulDemuxId, HUINT8 *pucBuf
										, HUINT32 ulRqId)
{
	HERROR			hErr;
	palPes_Msg_t	stMsg;
	palPes_SbtlInfo_t	*pSbtl = pal_pes_GetSbtlContents();

	if ( pucBuf == NULL )
	{
		HxLOG_Print("Check Alloc Buffer!!\n");
		return ERR_OK;
	}

	if ((ulRqId != pSbtl->ulRqId) )
	{
		HxLOG_Print("Invalid parser index!!\n");
		goto FreeBuf;
	}

	stMsg.prosPktMsg.ulCmd = PESM_PROCESS_PACKET;
	stMsg.prosPktMsg.hAction = pSbtl->hAction;
	stMsg.prosPktMsg.pucBuf = pucBuf;

	HxLOG_Print("Send packet!\n");

	hErr = VK_MSG_SendTimeout (s_ulPalSbtlMsgQId, &stMsg, sizeof(palPes_Msg_t), 0);
	if(ERR_OK!=hErr)			HxLOG_Critical("Critical Eror!!!\n");

	return ERR_OK;

FreeBuf :

	/* 필요없는 PES packet이면 Free한다. */
	if ( pucBuf != NULL )
	{
		OxMW_Free(pucBuf);
	}

	return ERR_OK;
}


STATIC HUINT32 pal_pes_ExcuteSbtlPacket(void)
{
	palPes_List_t	*pPacket = NULL;
	HUINT32			nList;
	POINTER_LIST_T  *pFound = NULL;
	palPes_SbtlInfo_t	*pSbtl = pal_pes_GetSbtlContents();
	HUINT32		ulSleep = 0;

	if( pSbtl->status == ePES_DISABLED )
	{
		return palPes_SBTL_CMD_TIME_OUT;
	}

	nList = palPes_GetNumList(pSbtl->pPesList, NULL, NULL);
	HxLOG_Print("[pal_pes_ExcuteSbtlPacket] nList(%d)\n", (int)nList);

	/*
	 *	Packet is not input into decoder ordered(UK stream). For example, PTS => 21, 22, 23, (28, 27, 26,) 24, 26 etc.
	 *	But pes data have to excute first in first out. Therefore we insert packet FIFO and if we find out just correct time,
	 *	we excute packet until the packet have incorrect time.
	 */
	pFound = palPes_FindNthList(pSbtl->pPesList, 0, NULL, NULL);
	if ( pFound == NULL )
	{
		return palPes_SBTL_CMD_TIME_OUT;
	}

	pPacket = palPes_GetContents(pFound);
	ulSleep = pal_pes_IsOnTimePesPacket(pPacket, pSbtl->ulDemuxId);
	if ( ulSleep == 0 )
	{
		/* Callback을 불러 준다. */
		(*s_pfnCbSubtitle)(pSbtl->hAction, pSbtl->ulDemuxId, pPacket->ulLen, pPacket->pBuf);

		/* Delete packet. */
		pSbtl->pPesList = palPes_DeleteList(pSbtl->pPesList, pFound, (FREE_FUNC)pal_pes_DeletePesPacketFromList);
	}

	/*
	 *  처리된 나머지 개수가 palPes_SBTL_MAX_LIST_NUM를 넘는다면
	 *  처리되지 않고 계속 쌓여 있을 수도 있다. 이런 경우
	 *  모두 지워 List를 비워 버린다.
	 */
	if ( nList > palPes_SBTL_MAX_LIST_NUM )
	{
		pSbtl->pPesList = palPes_FreeWholeList(pSbtl->pPesList, (FREE_FUNC)pal_pes_DeletePesPacketFromList);
	}

	return ulSleep != 0 ? ulSleep : 20;
}


STATIC HUINT32 pal_pes_ProcessSbtlPacket(palPes_SbtlInfo_t *pSbtl, palPes_Msg_t *pstMsg)
{
	HUINT32 	ulStartCode, pesLen;
	HUINT8		ptsDtsFlags;
	HUINT32		pts;
	HUINT8		*pPes = pstMsg->prosPktMsg.pucBuf;
	palPes_List_t	*pPacket = NULL;
	HUINT32		ulSleep;

	if ( pPes == NULL )
	{
		return palPes_SBTL_CMD_TIME_OUT;
	}

	/* Sync를 위해 확인.. */
	if ( (pSbtl->status != ePES_ENABLED) || (pSbtl->hAction != pstMsg->prosPktMsg.hAction) )
	{
		/* Enable이 아니거나 해당 버전이 아니면 Free 시킨다. */
		goto freeBuf;
	}

	/* Callback 확인.. */
	if ( s_pfnCbSubtitle == NULL )
	{
		HxLOG_Print("Cb Function NULL!!\n");
		goto freeBuf;
	}

	/* Check Start code */
	ulStartCode = get32bit(pPes);
	if ( ulStartCode != palPes_PACKET_START_CODE )
	{
		HxLOG_Print("[pal_pes_CbSbtlPesNotify] PES start code is invalid!!\r\n");
		goto freeBuf;
	}

	pesLen = get16bit(pPes + 4) + 6/* PES Header size until length fields */;

	/* Get PTS From PES packet. */
	ptsDtsFlags = (HUINT8) ((get8bit(pPes + 7) & 0xC0) >> 6);
	if ( ptsDtsFlags == 0x02 /* PTS flag */ )
	{
		/* 마지막 1bit는 제외함. */
		pts = (get8bit(pPes + 9) >> 1) & 0x07;
		pts = (pts << 8) | get8bit(pPes + 10);
		pts = (pts << 7) | ((get8bit(pPes + 11) >> 1) & 0x7f);
		pts = (pts << 8) | get8bit(pPes + 12);
		pts = (pts << 6) | ((get8bit(pPes + 13) >> 2) & 0x3f);
	}
	else if ( ptsDtsFlags == 0x03 /* PTS & DTS flag */ )     /* 이런 경우는 없다고 함. */
	{
		HxLOG_Critical("\n\n");
		goto freeBuf;
	}
	else    /* PTS가 없는 경우. */
	{
		pts = 0;
	}

	/* Alloc List. */
	pPacket = (palPes_List_t *)OxMW_Malloc(sizeof(palPes_List_t));
	if ( pPacket == NULL )
	{
		HxLOG_Critical("\n\n");
		goto freeBuf;
	}

	pPacket->ulPts = pts;
	pPacket->ulLen = pesLen;
	pPacket->pBuf = pPes;

	/* Add Packet!. */
	pal_pes_AddPesPacket(ePAL_PES_TYPE_SBTL, pPacket);

	/* 실행 되어야 할 Pakcet을 찾아 실행 시킨다. */
	ulSleep = pal_pes_ExcuteSbtlPacket();

	return ulSleep;

freeBuf :
	if ( pPes != NULL )
	{
		OxMW_Free(pPes);
	}

	return palPes_SBTL_CMD_TIME_OUT;
}


STATIC void pal_pes_SbtlHandlerTask(void)
{
	HERROR          hErr;
	palPes_Msg_t	stMsg;
	HUINT32		ulSleep = palPes_SBTL_CMD_TIME_OUT;

	while (1)
	{
		hErr = VK_MSG_ReceiveTimeout (s_ulPalSbtlMsgQId, &stMsg, sizeof (palPes_Msg_t), ulSleep);

		palPes_SBTL_CRITICAL_ENTER;

		if ( hErr != VK_OK )
		{
			ulSleep = pal_pes_ExcuteSbtlPacket();
			palPes_SBTL_CRITICAL_LEAVE;

			continue;
		}

		switch (stMsg.ulCmd)
		{
			case PESM_PROCESS_PACKET :
				ulSleep = pal_pes_ProcessSbtlPacket(pal_pes_GetSbtlContents(), &stMsg);
				break;

			default :
				HxLOG_Critical("\n\n");
				ulSleep = 20;
				break;
		}

		palPes_SBTL_CRITICAL_LEAVE;
 	}
}


#define _________TTX_Function________
STATIC DI_ERR_CODE pal_pes_CbTtxPesNotify(HUINT32 ulDemuxId, HUINT8 *pucBuf, HUINT32 ulRqId)
{
	HERROR			hErr;
    palPes_Msg_t	stMsg;
    palPes_TtxInfo_t	*pTtx = pal_pes_GetTtxContents();

    if ( pTtx->status == ePES_DISABLED )
    {
		goto freebuf;
    }

    if ( pucBuf == NULL )
    {
        HxLOG_Print("[pal_pes_CbTtxPesNotify] Check Alloc Buffer!!\n");
        goto freebuf;
    }

	/* 요구한 Packet인지 확인. */
    if ((ulRqId != pTtx->ulRqId))
    {
        HxLOG_Print("[pal_pes_CbTtxPesNotify] Invalid parser index!!\n");
        goto freebuf;
    }

	stMsg.prosPktMsg.ulCmd = PESM_PROCESS_PACKET;
	stMsg.prosPktMsg.hAction = pTtx->hAction;
	stMsg.prosPktMsg.pucBuf = pucBuf;

	/* Send Message. */
	hErr = VK_MSG_SendTimeout (s_ulPalTtxMsgQId, &stMsg, sizeof(palPes_Msg_t), 0);
	if(ERR_OK!=hErr)			HxLOG_Critical("Critical Eror!!!\n");

    return ERR_OK;

freebuf :

	/* Invalid한 PES packet이면 free 시킨다. */
	if ( pucBuf != NULL )
	{
		OxMW_Free(pucBuf);
	}

	return ERR_OK;
}


STATIC HUINT32 pal_pes_ExcuteTtxPacket(void)
{
	palPes_List_t	*pPacket = NULL;
	HUINT32			nList;
	POINTER_LIST_T  *pFound = NULL;
	palPes_TtxInfo_t	*pTtx = pal_pes_GetTtxContents();
	HUINT32			ulSleep = 0;

	if( pTtx->status == ePES_DISABLED )
	{
		return palPes_TTX_CMD_TIME_OUT;
	}

	nList = palPes_GetNumList(pTtx->pPesList, NULL, NULL);
	HxLOG_Print("[pal_pes_ExcuteTtxPacket] nList(%d)\n", (int)nList);

	pFound = palPes_FindNthList(pTtx->pPesList, 0, NULL, NULL);
	if ( pFound == NULL )
	{
		return palPes_TTX_CMD_TIME_OUT;
	}

	pPacket = palPes_GetContents(pFound);
	ulSleep = pal_pes_IsOnTimePesPacket(pPacket, pTtx->ulDemuxId);
	if ( ulSleep == 0 )
	{
		/* Callback을 불러 준다. */
		if(s_pfnCbTeletext)
			(*s_pfnCbTeletext)(pTtx->hAction, pTtx->ulDemuxId, pPacket->ulLen, pPacket->pBuf);

		/* Delete packet. */
		pTtx->pPesList = palPes_DeleteList(pTtx->pPesList, pFound, (FREE_FUNC)pal_pes_DeletePesPacketFromList);
	}

	if ( nList > palPes_TTX_MAX_LIST_NUM )
	{
		/* List에서 제거 한다. */
		pTtx->pPesList = palPes_FreeWholeList(pTtx->pPesList, (FREE_FUNC)pal_pes_DeletePesPacketFromList);
	}

	return ulSleep != 0 ? ulSleep : 20;
}


STATIC HUINT32 pal_pes_ProcessTtxPacket(palPes_TtxInfo_t *pTtx, palPes_Msg_t *pstMsg)
{
	HUINT32		ulStartCode, pesLen;
	HUINT8		ptsDtsFlags;
	HUINT32		pts;
	HUINT8		*pPes = pstMsg->prosPktMsg.pucBuf;
	palPes_List_t	*pPacket = NULL;
	HUINT32		ulSleep = 0;

	if ( (pTtx->status == ePES_DISABLED)
		|| (pstMsg->prosPktMsg.hAction != pTtx->hAction) )
	{
		goto freebuf;
	}

	/* Check Start code */
	ulStartCode = get32bit(pPes);
	if ( ulStartCode != palPes_PACKET_START_CODE ) //0x0x1BD
	{
		HxLOG_Print("[pal_pes_CbTtxPesNotify] PES start code is invalid!!\r\n");
		goto freebuf;
	}

	pesLen = get16bit(pPes + 4) + 6/* PES Header size until length fields */;

	/* Get PTS From PES packet. */
	ptsDtsFlags = (HUINT8) ((get8bit(pPes + 7) & 0xC0) >> 6);
	if ( ptsDtsFlags == 0x02 /* PTS flag */ )
	{
		/* 마지막 1bit는 제외함. */
		pts = (get8bit(pPes + 9) >> 1) & 0x07;
		pts = (pts << 8) | get8bit(pPes + 10);
		pts = (pts << 7) | ((get8bit(pPes + 11) >> 1) & 0x7f);
		pts = (pts << 8) | get8bit(pPes + 12);
		pts = (pts << 6) | ((get8bit(pPes + 13) >> 2) & 0x3f);
	}
	else if ( ptsDtsFlags == 0x03 /* PTS & DTS flag */ )     /* 이런 경우는 없다고 함. */
	{
		HxLOG_Critical("\n\n");
		goto freebuf;
	}
	else    /* PTS가 없는 경우. */
	{
		pts = 0;
	}

	/* Alloc List. */
	pPacket = (palPes_List_t *)OxMW_Malloc(sizeof(palPes_List_t));
	if ( pPacket == NULL )
	{
		HxLOG_Critical("\n\n");
		goto freebuf;
	}

	pPacket->ulPts = pts;
	pPacket->ulLen = pesLen;
	pPacket->pBuf = pPes;

	/* Add Packet!. */
	pal_pes_AddPesPacket(ePAL_PES_TYPE_TTX, pPacket);

	/* 실행 되어야 할 Pakcet을 찾아 실행 시킨다. */
	ulSleep = pal_pes_ExcuteTtxPacket();

	return ulSleep;

freebuf :

	/* Invalid한 PES packet이면 free 시킨다. */
	if ( pPacket != NULL )
	{
		OxMW_Free(pPacket);
	}

	return palPes_TTX_CMD_TIME_OUT;
}


STATIC void pal_pes_TtxHandlerTask(void)
{
	HERROR			hErr;
	palPes_Msg_t	stMsg;
	HUINT32		ulSleep = palPes_TTX_CMD_TIME_OUT;

	while(1)
	{
		hErr = VK_MSG_ReceiveTimeout(s_ulPalTtxMsgQId, &stMsg, sizeof(palPes_Msg_t), ulSleep);

		palPes_TTX_CRITICAL_ENTER;
		if ( hErr != VK_OK )
		{
			/*  Filtering 시작 후 Packet이 안 들어 오는 순간은 없겠지만
				있다면 남아 있는 Packet을 찾아 동작 시킨다. */
			ulSleep = pal_pes_ExcuteTtxPacket();
			palPes_TTX_CRITICAL_LEAVE;
			continue;
		}

		switch (stMsg.ulCmd)
		{
			case PESM_PROCESS_PACKET :
				ulSleep = pal_pes_ProcessTtxPacket(pal_pes_GetTtxContents(), &stMsg);
				break;

			default :
				HxLOG_Critical("\n\n");
				ulSleep = 20;
				break;
		}

		palPes_TTX_CRITICAL_LEAVE;
	}
}


#define ______User_defined_Function________
STATIC DI_ERR_CODE pal_pes_CbUserDefPesNotify(HUINT32 ulDemuxId, HUINT8 *pucBuf, HUINT32 ulRqId)
{
	HUINT32 	ulStartCode, pesLen;
	palPes_UserDefInfo_t *pUserDef = pal_pes_GetUserDefContents();

	if ( pucBuf == NULL )
	{
		return DI_ERR_OK;
	}

	/* Sync를 위해 확인.. */
	if ( pUserDef->status != ePES_ENABLED )
	{
		/* Enable이 아니거나 해당 버전이 아니면 Free 시킨다. */
		goto freeBuf;
	}

	if ( pUserDef->ulDemuxId != ulDemuxId || pUserDef->ulRqId != ulRqId )
	{
		goto freeBuf;
	}

	/* Callback 확인.. */
	if ( s_pfncbUserDef == NULL )
	{
		HxLOG_Print("Cb Function NULL!!\n");
		goto freeBuf;
	}

	/* Check Start code */
	ulStartCode = get32bit(pucBuf);
	if ( ulStartCode != palPes_PACKET_START_CODE )
	{
		HxLOG_Print("[pal_pes_CbSbtlPesNotify] PES start code is invalid!!\r\n");
		goto freeBuf;
	}

	pesLen = get16bit(pucBuf + 4) + 6/* PES Header size until length fields */;

	/* Excute Callback. */
	(*s_pfncbUserDef)(ulDemuxId, pesLen, pucBuf, ulRqId);

freeBuf :
	if ( pucBuf != NULL )
	{
		OxMW_Free(pucBuf);
	}

	return DI_ERR_OK;
}



#define _________External_Function________
HERROR PAL_PES_Init(void)
{
	HERROR      hErr;

	HxLOG_Info("[PAL_PES_Init] (+)\n");

	s_pfnCbSubtitle = NULL;
	s_pfnCbTeletext = NULL;
	s_pfncbUserDef = NULL;
	s_uPalPesRequestId = 0;

	/* message queue 생성 */
	/* message를 보내고 거의 바로바로 처리되기 때문에 32개라도 충분하다. */
	hErr = VK_MSG_Create(PAL_SBTL_MSGQ_SIZE,
						sizeof(palPes_Msg_t),
						"PalSbtlMsgQ",
						(unsigned long *)&s_ulPalSbtlMsgQId,
						VK_SUSPENDTYPE_FIFO);
	if ( hErr != VK_OK )
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

#ifdef	CONFIG_MW_TELETEXT
	/* Create Message Queue */
	hErr = VK_MSG_Create(PAL_TTX_MSGQ_SIZE,
						sizeof(palPes_Msg_t),
						"PalTtxMsgQ",
						(unsigned long *)&s_ulPalTtxMsgQId,
						VK_SUSPENDTYPE_FIFO);
	if ( hErr != VK_OK )
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}
#endif
	/* Create Task */
	/* Task에서 하는 함수 call 도 많지 않고 하는 일이 없어 stack size를 2K로 함. */
	hErr = VK_TASK_Create((void *)pal_pes_SbtlHandlerTask,
							PAL_SBTL_TASK_PRIORITY,
							PAL_SBTL_TASK_STACK_SIZE,
							"tPalSbtl",
							NULL,
							(unsigned long *)&s_ulPalSbtlTaskId,
							VK_SUSPENDTYPE_FIFO);
	if ( hErr != VK_OK )
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

#ifdef	CONFIG_MW_TELETEXT
	/* Task에서 하는 함수 call 도 많지 않고 하는 일이 없어 stack size를 2K로 함. */
	hErr = VK_TASK_Create((void *)pal_pes_TtxHandlerTask,
							PAL_TLTXT_TASK_PRIORITY,
							PAL_TLTXT_TASK_STACK_SIZE,
							"tPalTtx",
							NULL,
							(unsigned long *)&s_ulPalTtxTaskId,
							VK_SUSPENDTYPE_FIFO);
	if ( hErr != VK_OK )
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}
#endif

	/* Sync용 semaphore 생성. */
	hErr = VK_SEM_Create((unsigned long *)&s_ulPalSbtlSyncSema, "PalSbtlSyncSema", VK_SUSPENDTYPE_PRIORITY);
	if(VK_OK!=hErr)			HxLOG_Critical("Critical Eror!!!\n");

#ifdef	CONFIG_MW_TELETEXT
	/* Sync용 semaphore 생성. */
	hErr = VK_SEM_Create((unsigned long *)&s_ulPalTtxSyncSema, "PalTtxSyncSema", VK_SUSPENDTYPE_PRIORITY);
	if(VK_OK!=hErr)			HxLOG_Critical("Critical Eror!!!\n");
#endif
	/* Sync용 semaphore 생성. */
	hErr = VK_SEM_Create((unsigned long *)&s_ulPalUserSyncSema, "PalUserSyncSema", VK_SUSPENDTYPE_PRIORITY);
	if(VK_OK!=hErr)			HxLOG_Critical("Critical Eror!!!\n");

	/* Request ID semaphore 생성. */
	hErr = VK_SEM_Create((unsigned long *)&s_ulPalResRqSema, "PalPesRqSema", VK_SUSPENDTYPE_PRIORITY);
	if(VK_OK!=hErr)			HxLOG_Critical("Critical Eror!!!\n");

	pal_pes_ResetPesInfo(ePAL_PES_TYPE_SBTL);
	pal_pes_ResetPesInfo(ePAL_PES_TYPE_TTX);
	pal_pes_ResetPesInfo(ePAL_PES_TYPE_USER_DEFINE);

	HxLOG_Info("[PAL_PES_Init] (-)\n");

	return ERR_OK;
}


HERROR PAL_PES_StartSbtlTask(void)
{
	HERROR      hErr;

	hErr = VK_TASK_Start(s_ulPalSbtlTaskId);
	if(hErr!=VK_OK)			HxLOG_Critical("Critical Eror!!!\n");

	return ERR_OK;
}


HERROR PAL_PES_RegisterSbtlCallBack(PalPes_SBTL_Notify_t pfnNotify)
{
	HxLOG_Info("[PAL_PES_RegisterSbtlCallBack] (+)\n");

	palPes_SBTL_CRITICAL_ENTER;

	if ( pfnNotify == NULL )
	{
		palPes_SBTL_CRITICAL_LEAVE;

		return ERR_FAIL;
	}

	s_pfnCbSubtitle = pfnNotify;

	palPes_SBTL_CRITICAL_LEAVE;

	HxLOG_Info("[PAL_PES_RegisterSbtlCallBack] (-)\n");

	return ERR_OK;
}


HERROR PAL_PES_StartSbtl(Handle_t hAction, HUINT32 ulDemuxId, HUINT16 usPid)
{
	DI_ERR_CODE diErr=DI_ERR_OK;
	HERROR  		hErr=ERR_OK;
	palPes_SbtlInfo_t	*pSbtl;
	HUINT32			reqId;

	HxLOG_Info("[PAL_PES_StartSbtl] (+)\n");

	palPes_SBTL_CRITICAL_ENTER;

	pSbtl = pal_pes_GetSbtlContents();
	if ( pSbtl->status == ePES_ENABLED )
	{
		HxLOG_Print("Already Start!");
		palPes_SBTL_CRITICAL_LEAVE;

		return ERR_OK;
	}

	reqId = pal_pes_GetRequestId();

	/* Set Info. */
	pal_pes_SetPesInfo(ePAL_PES_TYPE_SBTL, hAction, ePES_ENABLED,	ulDemuxId, reqId);

	/* Callback 함수를 등록한다. */
	DI_DEMUX_RegisterPESCallback (	ulDemuxId,
									DI_DEMUX_PESTYPE_SUBTITLE,
									(pfnDI_DEMUX_PES_Notify) pal_pes_CbSbtlPesNotify,
									(pfnDI_DEMUX_AllocBuffer) pal_pes_CbAllocate,
									(pfnDI_DEMUX_FreeBufferForError) pal_pes_CbErrorFree);

	/* Demux를 start 시킨다. */
	diErr = DI_DEMUX_StartPESFilter (ulDemuxId, usPid, DI_DEMUX_PESTYPE_SUBTITLE, reqId);
	if(diErr!=DI_ERR_OK)
	{
		/* If fail to use PES resource, stop PES Filter and clear-up using its resources */
		diErr = DI_DEMUX_StopPESFilter (ulDemuxId, reqId);
		if ( diErr != DI_ERR_OK )
		{
			HxLOG_Print("[PAL_PES_StartSbtl] Error(0x%x) from DI_DEMUX_StopPESFilter()\n", (int)diErr);
		}

		/* Reset Callback. */
		DI_DEMUX_RegisterPESCallback (ulDemuxId, DI_DEMUX_PESTYPE_SUBTITLE, NULL, NULL, NULL);

		/* Reset Sbtl Info. */
		pal_pes_ResetPesInfo(ePAL_PES_TYPE_SBTL);

		hErr = ERR_FAIL;
		
	}

	palPes_SBTL_CRITICAL_LEAVE;

	HxLOG_Info("[PAL_PES_StartSbtl] (-)\n");

	return hErr;

}


HERROR PAL_PES_StopSbtl(HUINT32 ulDemuxId)
{
	HERROR 			hErr;
	palPes_SbtlInfo_t	*pSbtl;

	HxLOG_Info("[PAL_PES_StopSbtl] (+)\n");

	palPes_SBTL_CRITICAL_ENTER;

	pSbtl = pal_pes_GetSbtlContents();
	if ( pSbtl->status == ePES_DISABLED )
	{
		HxLOG_Print("Already stopped!\n");
		palPes_SBTL_CRITICAL_LEAVE;

		return ERR_OK;
	}

	if ( pSbtl->ulDemuxId != ulDemuxId )
	{
		HxLOG_Print("Wrong Demux Id!!!(DMX id = 0x%x)\n", (int)ulDemuxId);
		palPes_SBTL_CRITICAL_LEAVE;

		return ERR_OK;
	}

	/* Stop Filter. */
	hErr = DI_DEMUX_StopPESFilter (pSbtl->ulDemuxId, pSbtl->ulRqId);
	if ( hErr != DI_ERR_OK )
	{
		HxLOG_Print("[PAL_PES_StopSbtl] Error(0x%x) from D_DEMUX_StopPESFilter()\n", (int)hErr);
		palPes_SBTL_CRITICAL_LEAVE;

		return ERR_OK;
	}

	/* Reset Callback. */
	DI_DEMUX_RegisterPESCallback (pSbtl->ulDemuxId, DI_DEMUX_PESTYPE_SUBTITLE, NULL, NULL, NULL);

	/* Reset Sbtl Info. */
	pal_pes_ResetPesInfo(ePAL_PES_TYPE_SBTL);

	palPes_SBTL_CRITICAL_LEAVE;

	HxLOG_Info("[PAL_PES_StopSbtl] (-)\n");

	return ERR_OK;
}


HERROR PAL_PES_StartTtxTask(void)
{
#ifdef	CONFIG_MW_TELETEXT
	HERROR      hErr;

	hErr = VK_TASK_Start(s_ulPalTtxTaskId);
	if(hErr!=VK_OK)			HxLOG_Critical("Critical Eror!!!\n");

#endif
	return ERR_OK;
}


HERROR PAL_PES_RegisterTtxCallBack(PalPes_TTX_Notify_t pfnNotify)
{
#ifdef	CONFIG_MW_TELETEXT
	HxLOG_Info("[PAL_PES_RegisterTtxCallBack] (+)\n");

	palPes_TTX_CRITICAL_ENTER;

    if( pfnNotify == NULL )
    {
    	palPes_TTX_CRITICAL_LEAVE;

        return ERR_FAIL;
    }

    s_pfnCbTeletext = pfnNotify;

	palPes_TTX_CRITICAL_LEAVE;

	HxLOG_Info("[PAL_PES_RegisterTtxCallBack] (-)\n");
#endif
	HAPPY(pfnNotify);
    return ERR_OK;
}


HERROR PAL_PES_EnableTtx(Handle_t hAction, HUINT32 ulDemuxId)
{
#ifdef	CONFIG_MW_TELETEXT
    palPes_TtxInfo_t	*pTtx;

	HxLOG_Info("[PAL_PES_EnableTtx] (+)\n");

	palPes_TTX_CRITICAL_ENTER;

	pTtx = pal_pes_GetTtxContents();
	if ( pTtx->hAction != hAction )
	{
		palPes_TTX_CRITICAL_LEAVE;

		return ERR_FAIL;
	}

	if (pTtx->status == ePES_ENABLED)
	{
		palPes_TTX_CRITICAL_LEAVE;

		return ERR_FAIL;
	}

	/* Pes List를 초기화 한다. */
	pal_pes_FreeAllPesPacket(ePAL_PES_TYPE_TTX);

	pTtx->status = ePES_ENABLED;

	/* Set Callback */
	DI_DEMUX_RegisterPESCallback (	ulDemuxId,
									DI_DEMUX_PESTYPE_TELETEXT,
									(pfnDI_DEMUX_PES_Notify) pal_pes_CbTtxPesNotify,
									(pfnDI_DEMUX_AllocBuffer) pal_pes_CbAllocate,
									(pfnDI_DEMUX_FreeBufferForError) pal_pes_CbErrorFree);


	palPes_TTX_CRITICAL_LEAVE;

	HxLOG_Info("[PAL_PES_EnableTtx] (-)\n");
#endif
	HAPPY(hAction);
	HAPPY(ulDemuxId);
	return ERR_OK;
}


HERROR PAL_PES_DisableTtx(Handle_t hAction, HUINT32 ulDemuxId)
{
#ifdef	CONFIG_MW_TELETEXT
    palPes_TtxInfo_t	*pTtx;

	HxLOG_Info("[PAL_PES_DisableTtx] (+)\n");

	palPes_TTX_CRITICAL_ENTER;

	pTtx = pal_pes_GetTtxContents();
	if ( pTtx->hAction != hAction )
	{
		palPes_TTX_CRITICAL_LEAVE;

	    return ERR_FAIL;
	}

	if (pTtx->status == ePES_DISABLED)
	{
		palPes_TTX_CRITICAL_LEAVE;

		return ERR_FAIL;
	}

	/* Set Callback */
	DI_DEMUX_RegisterPESCallback(ulDemuxId, DI_DEMUX_PESTYPE_TELETEXT, NULL, NULL, NULL);

	/* Set Disable */
	pTtx->status = ePES_DISABLED;

	/* Pes List를 초기화 한다. */
	pal_pes_FreeAllPesPacket(ePAL_PES_TYPE_TTX);

	palPes_TTX_CRITICAL_LEAVE;

	HxLOG_Info("[PAL_PES_DisableTtx] (-)\n");
#endif
	HAPPY(hAction);
	HAPPY(ulDemuxId);
	return ERR_OK;
}


HERROR PAL_PES_StartTtx(Handle_t hAction, HUINT32 ulDemuxId, HUINT16 usPid)
{
#ifdef	CONFIG_MW_TELETEXT
    HERROR 			hErr;
    palPes_TtxInfo_t	*pTtx;
	HUINT32			reqId;

    HxLOG_Info("[PAL_PES_StartTtx] (+)\n");

	palPes_TTX_CRITICAL_ENTER;

	pTtx = pal_pes_GetTtxContents();
	if ( pTtx->status == ePES_ENABLED )
	{
	    HxLOG_Print("Already Start!");
		palPes_TTX_CRITICAL_LEAVE;

	    return ERR_OK;
	}

	reqId = pal_pes_GetRequestId();

	/* 매번 Unique한 ID를 만듬... hAction 활용. */
	pal_pes_SetPesInfo(ePAL_PES_TYPE_TTX,	hAction, ePES_DISABLED,	ulDemuxId, reqId);

	/* Demux를 start 시킨다. */
	hErr = DI_DEMUX_StartPESFilter (ulDemuxId, usPid, DI_DEMUX_PESTYPE_TELETEXT, reqId);
	if(hErr!=DI_ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

	palPes_TTX_CRITICAL_LEAVE;

	HxLOG_Info("[PAL_PES_StartTtx] (-)\n");
#endif
	HAPPY(hAction);
	HAPPY(ulDemuxId);
	HAPPY(usPid);
	return ERR_OK;
}


HERROR PAL_PES_StopTtx(HUINT32 ulDemuxId)
{
#ifdef	CONFIG_MW_TELETEXT
    HERROR 			hErr;
    palPes_TtxInfo_t	*pTtx;

    HxLOG_Info("[PAL_PES_StopTtx] (+)\n");

	palPes_TTX_CRITICAL_ENTER;

	pTtx = pal_pes_GetTtxContents();

	/* Enable 상태에서 stop이 내려 올 수 없다고 일단 가정. 여기 나오면 check 해 볼 것. */
	if ( pTtx->status == ePES_ENABLED )
	{
		HxLOG_Print("[PAL_PES_StopTtx] Already stopped!!!\n");
		palPes_TTX_CRITICAL_LEAVE;

		return ERR_OK;
	}

	if ( pTtx->ulDemuxId != ulDemuxId )
	{
		HxLOG_Print("[PAL_PES_StopTtx] Wrong Demux Id!!!(DMX id = 0x%x)\n", ulDemuxId);
		palPes_TTX_CRITICAL_LEAVE;

		return ERR_OK;
	}

	/* Stop filter!. */
	hErr = DI_DEMUX_StopPESFilter (pTtx->ulDemuxId, pTtx->ulRqId);
	if ( hErr != DI_ERR_OK )
	{
		HxLOG_Print("[pal_pes_StopTtx] Error(0x%x) from DDI_DEMUX_DisableChannel()\n", (int)hErr);
		palPes_TTX_CRITICAL_LEAVE;

		return ERR_OK;
	}

	/* Reset TTX Info. */
	pal_pes_ResetPesInfo(ePAL_PES_TYPE_TTX);

	palPes_TTX_CRITICAL_LEAVE;

	HxLOG_Info("[PAL_PES_StopTtx] (-)\n");
#endif
	HAPPY(ulDemuxId);
	return ERR_OK;
}


HERROR PAL_PES_RegisterUserDefCallBack(PalPes_USERDEF_Notify_t pfnNotify)
{
	HxLOG_Info("[PAL_PES_RegisterUserDefCallBack] (+)\n");

	palPes_USER_CRITICAL_ENTER;

    if( pfnNotify == NULL )
    {
    	palPes_USER_CRITICAL_LEAVE;

        return ERR_FAIL;
    }

    s_pfncbUserDef = pfnNotify;

	palPes_USER_CRITICAL_LEAVE;

	HxLOG_Info("[PAL_PES_RegisterUserDefCallBack] (-)\n");

    return ERR_OK;
}


HERROR PAL_PES_StartUserDef(HUINT32 ulDemuxId, HUINT16 usPid, HUINT32 ulRqId)
{
	HERROR		hErr;
	palPes_UserDefInfo_t *pUserDef = pal_pes_GetUserDefContents();

	HxLOG_Info("[PAL_PES_StartUserDef] (+)\n");

	palPes_USER_CRITICAL_ENTER;

	if ( pUserDef->status == ePES_ENABLED )
	{
		palPes_USER_CRITICAL_LEAVE;

		return ERR_FAIL;
	}

	pal_pes_SetPesInfo(ePAL_PES_TYPE_USER_DEFINE, HANDLE_NULL, ePES_ENABLED, ulDemuxId, ulRqId);

	/* Callback 함수를 등록한다. */
	DI_DEMUX_RegisterPESCallback (	ulDemuxId,
									DI_DEMUX_PESTYPE_USERDEFINE,
									(pfnDI_DEMUX_PES_Notify) pal_pes_CbUserDefPesNotify,
									(pfnDI_DEMUX_AllocBuffer) pal_pes_CbAllocate,
									(pfnDI_DEMUX_FreeBufferForError) pal_pes_CbErrorFree);

	/* Demux를 start 시킨다. */
	hErr = DI_DEMUX_StartPESFilter (ulDemuxId, usPid, DI_DEMUX_PESTYPE_USERDEFINE, ulRqId);
	if(hErr!=DI_ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

	palPes_USER_CRITICAL_LEAVE;

	HxLOG_Info("[PAL_PES_StartUserDef] (-)\n");

	return hErr;
}


HERROR PAL_PES_StopUserDef(HUINT32 ulDemuxId, HUINT32 ulRqId)
{
	HERROR			hErr;
	palPes_UserDefInfo_t *pUserDef = pal_pes_GetUserDefContents();

	HxLOG_Info("[PAL_PES_StopUserDef] (+)\n");

	palPes_USER_CRITICAL_ENTER;

	if ( pUserDef->status == ePES_DISABLED )
	{
		palPes_USER_CRITICAL_LEAVE;

		return ERR_OK;
	}

	/* Stop filter!. */
	hErr = DI_DEMUX_StopPESFilter (ulDemuxId, ulRqId);
	if ( hErr != DI_ERR_OK )
	{
		HxLOG_Print("[PAL_PES_StopUserDef] Error(0x%x) from DDI_DEMUX_DisableChannel()\n", (int)hErr);
	}

	/* Callback 함수를 등록한다. */
	DI_DEMUX_RegisterPESCallback (	ulDemuxId,
									DI_DEMUX_PESTYPE_USERDEFINE,
									NULL,
									NULL,
									NULL);

	pal_pes_ResetPesInfo(ePAL_PES_TYPE_USER_DEFINE);

	palPes_USER_CRITICAL_LEAVE;

	HxLOG_Info("[PAL_PES_StopUserDef] (-)\n");

	return ERR_OK;
}

/* end of file */
