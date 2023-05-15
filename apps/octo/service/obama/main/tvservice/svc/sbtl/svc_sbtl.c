/**
	@file     mw_sbtl.c
	@brief    MW�� subtitle service ���

	Description: AP�� action��κ��� subtitle ��� �޾� service�Ѵ�. ��� action�� ���񽺸� ��û�� ���� ������, \n
				subtitle�� enable�Ǿ� �ְ� master action�� ���ؼ��� �����Ѵ�. ��û�� �ϴ� �޾Ƶΰ� �ִٰ�, \n
				master�� �ٲ�� �ش� action���� ���񽺰� �̵��ȴ�.
	Module: MW/SBTL			 	\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
/** @brief global header file  */
#include <octo_common.h>
#include <pal_pipe.h>
//#include <mwc_util.h>
#include <pal_datastream.h>
#include <db_param.h>
#include <util.h>
#include <pal_pes.h>
/** @brief local header file  */
#include "svc_sbtl.h"

#define	CONFIG_OCTO2_SUBTITLE

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/





#define	DATA_STREAM_DVB_SUBTITLE	"dvbsubtitle"

#define SUBTITLE_PAGE_VALID_LENGTH		7
/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/** @brief Decoder Off status */
typedef enum
{
	eSBTL_DecOff_Disable = 0,
	eSBTL_DecOff_Enable
} SvcSbtl_DecOffStatus_e;


/** @brief Context status */
typedef enum
{
	eSBTL_STATUS_NONE = 0,
	eSBTL_STATUS_RUNNING,
	eSBTL_STATUS_WAITING
} SvcSbtl_Status_e;


/** @brief Context ���� */
typedef struct tagSBTL_CONTEXT
{
	Handle_t			hAction;			/**< Action handle */
	SvcSbtl_Status_e       status;				/**< Status */
	HUINT32		ulDemuxId;			/**< Action�� �Ҵ�� demux ID */
	HUINT16				usPid;				/**< Subtitle PID */
	HULONG				ulTimerId;			/**< Timer ID */
	HBOOL				bCbTimeOut;			/**< Time Out Callback �Լ��� ���� �Ǿ����� ����. */
	HUINT16             usCompPageId;		/**< Composition Page ID. */
	HUINT16				usAncPageId;		/**< Anc Page ID. */
	HBOOL				bDisableDec;		/**< Decoder On/Off flag. */
} SvcSbtl_Context_t;


typedef struct
{
	HUINT32				ulCmd;				/**< Command. union�󿡼��� ���� ��ġ. */
	Handle_t			hAction;			/**< Command�� ������ action�� handle */
	HUINT16				usPid;				/**< Subtitle PID */
	HUINT16				usCompPageId;
	HUINT16				usAncPageId;
} SvcSbtl_StartMsg_t;


typedef struct
{
	HUINT32				ulCmd;				/**< Command. union�󿡼��� ���� ��ġ. */
	Handle_t			hAction;			/**< Command�� ������ action�� handle */
	ApiSyncMode_t		syncMode;			/**< Command�� ������ API�� ���� ������ sync/async�� ���� */
} SvcSbtl_StopMsg_t;


typedef struct
{
	HUINT32				ulCmd;				/**< Command. union�󿡼��� ���� ��ġ. */
	Handle_t			hAction;			/**< Command�� ������ action�� handle */
	HUINT16				usPid;				/**< Subtitle PID */
} SvcSbtl_ChangePidMsg_t;


typedef struct
{
	HUINT32				ulCmd;				/**< Command. union�󿡼��� ���� ��ġ. */
	HUINT32				ulMasterActionId;	/**< New Master action ID */
} SvcSbtl_SetMasterMsg_t;


typedef struct
{
	HUINT32				ulCmd;				/**< Command. union�󿡼��� ���� ��ġ. */
	Handle_t			hAction;			/**< Command�� ������ action�� handle */
	HBOOL				bOffDec;			/**< Decoder On/Off Flag. */
} SvcSbtl_SetOnOffDec_t;


typedef struct
{
	HUINT32				ulCmd;				/**< Command. union�󿡼��� ���� ��ġ. */
	HULONG				ulTimerId;
} SvcSbtl_PkTimeOut_t;


typedef struct
{
	HUINT32				ulCmd;				/**< Command. union�󿡼��� ���� ��ġ. */
	Handle_t			hAction;			/**< Command�� ������ action�� handle */
	HUINT32				ulDemuxId;
	HUINT8				*pBuf;
	HUINT32				ulLen;
} SvcSbtl_PkDecoding_t;


typedef union
{
	HUINT32				ulCmd;
	SvcSbtl_StartMsg_t		startMsg;
	SvcSbtl_StopMsg_t		stopMsg;
	SvcSbtl_ChangePidMsg_t changePidMsg;
	SvcSbtl_SetMasterMsg_t	setMasterMsg;
	SvcSbtl_SetOnOffDec_t	setOnOffDec;
	SvcSbtl_PkTimeOut_t	pkTimeOut;
	SvcSbtl_PkDecoding_t	pkDecode;
} SvcSbtl_Msg_t;


/** @brief : Subtitle service command */
enum {
	CMD_SBTL_START = 100,		/**< Subtitle ������ ��û�ϴ� ���. Master action�� �ƴϸ� ������ �ȴ�. */
	CMD_SBTL_STOP,				/**< Subtitle stop�� ��û�ϴ� ��� */
	CMD_SBTL_CHANGE_PID,		/**< PID ������ ��û�ϴ� ��� */
	CMD_SBTL_SET_MASTER,		/**< Master action�� �����ϴ� ��� */
	SBTLM_PACKET_DECODING,		/**< Packet decoding message.*/

	CMD_SBTL_END				/**< enum ������ ǥ��. ��� �ƴ�. */
 };

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static SvcSbtl_Context_t	s_stSbtlContext[NUM_MAX_ACTION];			/**< Subtitle Context. */
static HUINT32			s_ulMasterActionid = eActionId_VIEW_0;		/**< Master Action Id. */

static HULONG			s_ulSbtlMsgQId;			/**< Message Queue ID */
static HULONG			s_ulSbtlTaskId;			/**< Task ID */

STATIC PostMsgToMgrCb_t s_fnSvcSbtl_PostMsgToAp = NULL;

void SVC_SBTL_InitSbtlDecoderForAction(Handle_t hAction);
void SVC_SBTL_TermSbtlDecoder(Handle_t hAction);
void SVC_SBTL_SendSubtitleData(Handle_t hAction, HUINT8 *buffer, HINT32 len);
HERROR SVC_SBTL_DecodeSubtitle(HUINT16 usCompPageId, HUINT16 usAncPageId, HUINT8 *pPes, HUINT32 ulPesSize, HUINT32 *ulTimeOut);
void SVC_SBTL_ClearSubtitleArea(void);
void SVC_SBTL_HideSubtitleScreen(void);
void SVC_SBTL_ShowSubtitleScreen(void);

/*******************************************************************/
/********************    Local Functions   *************************/
/*******************************************************************/
#define __________STATIC_FUNCTIONS___________
STATIC void svc_sbtl_PostMsgToAp(HINT32 nMsg, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	if (s_fnSvcSbtl_PostMsgToAp)
	{
		 (* s_fnSvcSbtl_PostMsgToAp)(nMsg, hAction, nParam1, nParam2, nParam3);
	}
}

STATIC INLINE SvcSbtl_Context_t * svc_sbtl_GetContext (HUINT32 ulActId)
{
	return &s_stSbtlContext[ulActId];
}


STATIC INLINE void svc_sbtl_SetContextStatus(SvcSbtl_Context_t *pContext, SvcSbtl_Status_e eStatus)
{
	pContext->status = eStatus;
}


STATIC INLINE HULONG svc_sbtl_GetMessageQId(void)
{
	return s_ulSbtlMsgQId;
}

STATIC HERROR svc_sbtl_CbExcutePacket(Handle_t hAction, HUINT32 ulDemuxId, HUINT32 ulPesLength, HUINT8 *pucBuf)
{
	HUINT8      pkHeaderLenth;
	SvcSbtl_Msg_t	stMsg;

	if ( pucBuf == NULL )
	{
	    return ERR_FAIL;
	}

	/* Start code etc�� Pal���� �̹� Ȯ�� �Ǿ���. */
	pkHeaderLenth = get8bit(pucBuf + 8);
	if ( ulPesLength < ((HUINT32)pkHeaderLenth + SUBTITLE_PAGE_VALID_LENGTH + 3) )
	{
	    return ERR_FAIL;
	}

	stMsg.pkDecode.ulCmd = SBTLM_PACKET_DECODING;
	stMsg.pkDecode.hAction = hAction;
	stMsg.pkDecode.ulDemuxId = ulDemuxId;
	stMsg.pkDecode.ulLen = ulPesLength;
	stMsg.pkDecode.pBuf = (HUINT8 *)OxMW_Malloc(ulPesLength);
	if ( stMsg.pkDecode.pBuf == NULL )
	{
		return ERR_FAIL;
	}

	memcpy(stMsg.pkDecode.pBuf, pucBuf, ulPesLength);

	VK_MSG_Send (svc_sbtl_GetMessageQId(), &stMsg, sizeof(SvcSbtl_Msg_t));

	return ERR_OK;
}


STATIC HERROR svc_sbtl_Start(HUINT32 ulActionId)
{
	HERROR		hErr;
	SvcSbtl_Context_t	*pCt = NULL;

	pCt = svc_sbtl_GetContext(ulActionId);
	if ( pCt->usPid >= PID_NULL )
	{
	    HxLOG_Print("\tSubtitle PID is invalid. It won't start.\n");
	    return ERR_FAIL;
	}

	HxLOG_Print("\tStart subtitle for PID[0x%X].\n", (int)pCt->usPid);

	/* Initailize Decoder */
	SVC_SBTL_InitSbtlDecoderForAction(pCt->hAction);

	/* Retriving for PES packet is started  */
	hErr = PAL_PES_StartSbtl(pCt->hAction, (HUINT8)pCt->ulDemuxId, pCt->usPid);

	return hErr;
}

STATIC HERROR svc_sbtl_Stop(HUINT32 ulActionId)
{
	HERROR		hErr;
	SvcSbtl_Context_t *pCt = NULL;

	pCt = svc_sbtl_GetContext(ulActionId);
	if ( pCt->usPid >= PID_NULL )
	{
		HxLOG_Print("\tSubtitle is already stopped. No need to stop.\n");
		return ERR_FAIL;
	}

	HxLOG_Print("\tStop subtitle for PID[0x%X].\n", (int)s_stSbtlContext[ulActionId].usPid);

	/* Stop to retrive */
	hErr = PAL_PES_StopSbtl((HUINT8)pCt->ulDemuxId);

	/* Stop Decoder */
	SVC_SBTL_TermSbtlDecoder(pCt->hAction);

	if ( pCt->ulTimerId != VK_TIMERID_NULL )
	{
		VK_TIMER_Cancel(pCt->ulTimerId);
		pCt->ulTimerId = VK_TIMERID_NULL;
	}

	return hErr;
}

STATIC HERROR svc_sbtl_HandleMsgStart(SvcSbtl_Msg_t *sbtlMsg)
{
	HUINT32			ulActionId = PAL_PIPE_GetActionId(sbtlMsg->startMsg.hAction);
	HUINT32	ulDemuxId;
	HERROR			hErr;
	SvcSbtl_Context_t	*pCt = NULL;

	hErr = PAL_PIPE_GetResourceId(ulActionId, eRxRSCTYPE_DEMUX, &ulDemuxId);
	if(hErr != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");
	HxLOG_Print("MESSAGE : CMD_SBTL_START, hAction:0x%x, Demux ID:0x%x, PID:0x%x\n",
									(int)sbtlMsg->startMsg.hAction, (int)ulDemuxId, (int)sbtlMsg->startMsg.usPid);

	/* Session expire �˻��ϰ�... */
	if ( PAL_PIPE_IsActionHandleLatest(sbtlMsg->startMsg.hAction) != ERR_OK )
	{
		HxLOG_Print("\tAction handle 0x%x is already expired\n", (int)sbtlMsg->startMsg.hAction);
	    return ERR_FAIL;
	}

	pCt = svc_sbtl_GetContext(ulActionId);
	if ( pCt == NULL )
	{
	    HxLOG_Critical("\n\n");
	    return ERR_FAIL;
	}

	/* �̹� running ���̶�� �ƹ��� ������ �� �ʿ䰡 ����. */
	if ( pCt->status == eSBTL_STATUS_RUNNING )
	{	/* ���� ���� �ȵȴ�.(mater�̴�) */
	    HxLOG_Print("\tCheck Action status = (%d)\n", (int)pCt->status);

		hErr = svc_sbtl_Stop(ulActionId);
		if(hErr != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");
	}

	/* Action context �� ���� ����... */
	pCt->hAction		= sbtlMsg->startMsg.hAction;
	pCt->ulDemuxId		= ulDemuxId;
	pCt->usPid			= sbtlMsg->startMsg.usPid;
	pCt->usCompPageId	= sbtlMsg->startMsg.usCompPageId;
	pCt->usAncPageId	= sbtlMsg->startMsg.usAncPageId;

	svc_sbtl_SetContextStatus(pCt, eSBTL_STATUS_WAITING);

	/* ���� �Ϸ��� action�� Master�� �ƴϰų� Enable�� �ƴϸ� �������� �ʴ´�. */
	if ( ulActionId != s_ulMasterActionid )
	{
	    HxLOG_Print("\tThis action is not a master action or is not enabled. No effect.\n");
	    return ERR_OK;
	}

	/* ��û�� action �� master action�� ��쿡�� ���� ����. */
	hErr = svc_sbtl_Start(s_ulMasterActionid);
	if ( hErr != ERR_OK)
	{
	    HxLOG_Critical("\n\n");
	    return hErr;
	}

	svc_sbtl_SetContextStatus(pCt, eSBTL_STATUS_RUNNING);

	svc_sbtl_PostMsgToAp(eSEVT_SBTL_STARTED, pCt->hAction, 0, 0, 0);

	return ERR_OK;
}


STATIC HERROR svc_sbtl_HandleMsgStop(SvcSbtl_Msg_t *sbtlMsg)
{
	HERROR      hErr;
	HUINT32		ulActionId = PAL_PIPE_GetActionId(sbtlMsg->stopMsg.hAction);
	SvcSbtl_Context_t *pCt = NULL;

	HxLOG_Print("MESSAGE : CMD_SBTL_STOP, hAction:0x%x\n", (int)sbtlMsg->stopMsg.hAction);

	pCt = svc_sbtl_GetContext(ulActionId);
	if ( pCt == NULL )
	{
	    HxLOG_Critical("\n\n");

	    return ERR_FAIL;
	}

	/* Session expire �˻��ϰ�... �ֽ� ������ �ƴϸ� �׳� return �Ѵ�.*/
	if ( PAL_PIPE_IsActionHandleLatest(sbtlMsg->stopMsg.hAction) != ERR_OK )
	{
	    return ERR_OK;
	}

	/*
	 * Running ���̸�.. �� MasterAction���� �������� �ִٸ�..
	 * ���� ����ϰ� �ִ� subtitle data���� ��� ����� context��
	 * �ʱ�ȭ �Ѵ�.
	 */
	if ( pCt->status == eSBTL_STATUS_RUNNING )
	{
	    /* Ȥ�ó� �ؼ� Ȯ��.. */
	    if ( ulActionId == s_ulMasterActionid )
	    {
	        /* ��û�� action �� master action�� ��츸 ���� */
	        hErr = svc_sbtl_Stop(s_ulMasterActionid);
	    	if ( hErr != ERR_OK)
	    	{
	    		HxLOG_Critical("\n\n");
	    	}
	    }
	    else
	    {
	        /* ����� ������ �ȵ�. Status�� �� �� �Ǿ��ٴ� ���� Ȯ�� ���. */
	        HxLOG_Critical("\n\n");
	    }
	}

	/*
	 * 1. None �߿� Stop�� WTV ���� �ʱ� ���� �� Stop msg�� �����ϹǷ� �׷� ���� �߻��� �� �ְ�
	 * �� �� ������ �ٷ� app���� �˷� �ְų� waiting ���¿��� �ϴ� ��ó�� �Ʒ��� ���� ����
	 * �ص� �ȴ�.
	 *
	 * 2. Waiting �߿� Stop�� start �� Master action�� �ƴϰų� Enable flag�� disable
	 * ���¿��� ��� ���µ� �� ���� �Ʒ�ó�� context�� update�ϰ� app���� �˷� �ش�.
	 */

	pCt->hAction 	= sbtlMsg->stopMsg.hAction;
	pCt->ulDemuxId	= 0xFFFFFFFF;    /* start �ÿ� �ٽ� �ʱ�ȭ �� ��� �ȴ�. */
	pCt->usPid		= PID_NULL;      /* stop ���¸� ��Ÿ���� ���Ͽ� PID�� NULL�� �����... */
	pCt->usCompPageId = 0xFFFF;
	pCt->usAncPageId = 0xFFFF;

	svc_sbtl_SetContextStatus(pCt, eSBTL_STATUS_NONE);

#if defined(CONFIG_MW_SUBTITLE)
	svc_sbtl_PostMsgToAp(eSEVT_SBTL_STOPPED, pCt->hAction, 0, 0, 0);
#endif

	return ERR_OK;
}


STATIC HERROR svc_sbtl_HandleMsgChangePid(SvcSbtl_Msg_t *sbtlMsg)
{
	HERROR      hErr;
	HUINT32		ulActionId = PAL_PIPE_GetActionId(sbtlMsg->changePidMsg.hAction);
	SvcSbtl_Context_t *pCt = NULL;

	HxLOG_Print("MESSAGE : CMD_SBTL_CHANGE_PID, hAction:0x%x, PID:%d\n",
									(int)sbtlMsg->changePidMsg.hAction, (int)sbtlMsg->changePidMsg.usPid);

	/* Session expire �˻��ϰ�... */
	if ( PAL_PIPE_IsActionHandleLatest(sbtlMsg->changePidMsg.hAction) != ERR_OK )
	{
	    HxLOG_Print("\tAction handle 0x%x is already expired\n", (int)sbtlMsg->changePidMsg.hAction);
	    return ERR_OK;
	}

	pCt = svc_sbtl_GetContext(ulActionId);
	if ( pCt == NULL )
	{
	    HxLOG_Critical("\n\n");
	    return ERR_FAIL;
	}

	if ( pCt->status == eSBTL_STATUS_NONE )
	{
	    HxLOG_Print("\tAction is not started\n");
	    return ERR_OK;
	}

	/* PID change�� ���� session ������ ��û�Ǿ�� ��. */
	if(pCt->hAction != sbtlMsg->changePidMsg.hAction)			HxLOG_Critical("Critical Eror!!!\n");

	if ( pCt->usPid == sbtlMsg->changePidMsg.usPid )
	{ /* PID ������ �׳� ����. */
		HxLOG_Print("\tSame PID. No effect.\n");
		return ERR_OK;
	}

	/* PID �޶���. */
	HxLOG_Print("\tPID changed from 0x%X to 0x%X.\n", (int)s_stSbtlContext[ulActionId].usPid, (int)sbtlMsg->changePidMsg.usPid);

	if ( pCt->status == eSBTL_STATUS_WAITING )
	{
	    /* waiting ���¿����� PID�� �����ϰ� ������. */
	    pCt->usPid = sbtlMsg->changePidMsg.usPid;
	    return ERR_OK;
	}

	/* ������ʹ� Running ���·� Master action�� ���ÿ� enable �� �����̴�. */
	HxLOG_Print("\tStop current subtitle service.\n");
	hErr = svc_sbtl_Stop(s_ulMasterActionid);
	if ( hErr != ERR_OK)
	{
	    HxLOG_Critical("\n\n");
	    return hErr;
	}

	svc_sbtl_SetContextStatus(pCt, eSBTL_STATUS_WAITING);

	/* Action context �� ���� ����... */
	pCt->usPid = sbtlMsg->changePidMsg.usPid;

	/* �ٽ� �����Ѵ�. */
	HxLOG_Print("\tStarting subtitle service for new PID.\n");
	hErr = svc_sbtl_Start(s_ulMasterActionid);
	if ( hErr != ERR_OK)
	{
	    HxLOG_Critical("\n\n");
	    return hErr;
	}

	svc_sbtl_SetContextStatus(pCt, eSBTL_STATUS_RUNNING);

	return ERR_OK;
}

STATIC HERROR svc_sbtl_HandleMsgSetMaster(SvcSbtl_Msg_t *sbtlMsg)
{
	HERROR      hErr;
	HUINT32		ulNewMasterActionId = sbtlMsg->setMasterMsg.ulMasterActionId;
	SvcSbtl_Context_t *pCt = NULL;

	HxLOG_Print("MESSAGE : CMD_SBTL_SET_MASTER, Old:%d -> New:%d\n", (int)s_ulMasterActionid, (int)ulNewMasterActionId);

	if ( ulNewMasterActionId == s_ulMasterActionid )
	{
		HxLOG_Print("\tThe same master action ID. No effect.\n");
		return ERR_OK;
	}

	/* ���� master action ID stop */
	pCt = svc_sbtl_GetContext(s_ulMasterActionid);
	if ( pCt == NULL )
	{
	    HxLOG_Critical("\n\n");
	    return ERR_FAIL;
	}

	/* Running �� ��츸 Stop �Ѵ�. */
	if ( pCt->status == eSBTL_STATUS_RUNNING )
	{
		/* ���� master�� waiting ���� �����ϰ�..*/
		hErr = svc_sbtl_Stop(s_ulMasterActionid);
		if ( hErr != ERR_OK)
		{
		    HxLOG_Critical("\n\n");
		    return hErr;
		}
		svc_sbtl_SetContextStatus(pCt, eSBTL_STATUS_WAITING);
	}

	/* ���ο� master action ID�� �����ϰ�... */
	s_ulMasterActionid = ulNewMasterActionId;

	pCt = svc_sbtl_GetContext(s_ulMasterActionid);
	if ( pCt == NULL ) /* ������ �Ǿ����.. */
	{
	    HxLOG_Critical("\n\n");
	    return ERR_FAIL;
	}

	/* Waiting�� ��� Master�� �Ǹ� Filtering�� Start ���� �ش�. */
	if ( pCt->status == eSBTL_STATUS_WAITING )
	{
		/* Master action ID restart */
		hErr = svc_sbtl_Start(s_ulMasterActionid);
		if ( hErr != ERR_OK)
		{
		    HxLOG_Critical("\n\n");
		    return hErr;
		}
		svc_sbtl_SetContextStatus(pCt, eSBTL_STATUS_RUNNING);
	}

	return ERR_OK;
}

STATIC HERROR svc_sbtl_DecodePacket(SvcSbtl_Msg_t *sbtlMsg)
{
	SvcSbtl_Context_t  *pCt= NULL;

	HxLOG_Print("MESSAGE : SBTLM_PACKET_DECODING, hAction:0x%x\n", (int)sbtlMsg->pkDecode.hAction);

	/* Session expire �˻��ϰ�... */
	if ( PAL_PIPE_IsActionHandleLatest(sbtlMsg->pkDecode.hAction) != ERR_OK )
	{
	    HxLOG_Print("\tAction handle 0x%x is already expired\n", (int)sbtlMsg->pkDecode.hAction);
	    goto freebuf;
	}

	pCt = svc_sbtl_GetContext(s_ulMasterActionid);
	if ( (pCt->status != eSBTL_STATUS_RUNNING) || (pCt->ulDemuxId != sbtlMsg->pkDecode.ulDemuxId) )
	{
	    goto freebuf;
	}

	SVC_SBTL_SendSubtitleData(pCt->hAction, sbtlMsg->pkDecode.pBuf, sbtlMsg->pkDecode.ulLen);
	svc_sbtl_PostMsgToAp(eSEVT_SBTL_UPDATED, pCt->hAction, 0, 0, 0);

freebuf:

	if ( sbtlMsg->pkDecode.pBuf != NULL )
	{
		OxMW_Free(sbtlMsg->pkDecode.pBuf);
	}

	return ERR_OK;
}


STATIC void svc_sbtl_ServiceTask(void)
{
	HERROR			hErr;
	SvcSbtl_Msg_t      sbtlMsg;

	while (1)
	{
		hErr = VK_MSG_Receive(svc_sbtl_GetMessageQId(), &sbtlMsg, sizeof(SvcSbtl_Msg_t));
		if ( hErr != VK_OK )
		{
			continue;
		}

		switch (sbtlMsg.ulCmd)
		{
			case CMD_SBTL_START :
				svc_sbtl_HandleMsgStart(&sbtlMsg);
				break;

			case CMD_SBTL_STOP :
				svc_sbtl_HandleMsgStop(&sbtlMsg);
				break;

			case CMD_SBTL_CHANGE_PID :
				svc_sbtl_HandleMsgChangePid(&sbtlMsg);
				break;

			case CMD_SBTL_SET_MASTER :
				svc_sbtl_HandleMsgSetMaster(&sbtlMsg);
				break;

			case SBTLM_PACKET_DECODING :
				svc_sbtl_DecodePacket(&sbtlMsg);
				break;

			default :
				HxLOG_Critical("\n\n");
				break;
		}
	}
}


/*******************************************************************/
/********************      API Functions   *************************/
/*******************************************************************/
#define ____________API_FUNCTIONS_____________
void SVC_SBTL_Init(PostMsgToMgrCb_t fnPostMsg)
{
	HERROR          hErr;
	HUINT32         i;


	s_fnSvcSbtl_PostMsgToAp = fnPostMsg;


	/* Subtitle service message queue ���� */
	hErr = (HUINT32)VK_MSG_Create(MW_SBTL_MSGQ_SIZE,
									sizeof(SvcSbtl_Msg_t),
									"SbtlMsgQ",
									(unsigned long *)&s_ulSbtlMsgQId,
									VK_SUSPENDTYPE_FIFO);
	if (hErr != VK_OK)
	{
	    HxLOG_Critical("\n\n");
	}

	/* Subtitle service task ���� */
	hErr = (HUINT32)VK_TASK_Create((void*)svc_sbtl_ServiceTask,
									MW_SBTL_TASK_PRIORITY,
									MW_SBTL_TASK_STACK_SIZE,
									(char *)"tMwSbtl",
									NULL,
									(unsigned long *)&s_ulSbtlTaskId,
									VK_SUSPENDTYPE_FIFO);
	if (hErr != VK_OK)
	{
	    HxLOG_Critical("\n\n");
	}

	/* Action context �ʱ�ȭ */
	for(i = 0; i  < NUM_MAX_ACTION; i++)
	{
	    HxSTD_memset(&s_stSbtlContext[i], 0x00, sizeof(SvcSbtl_Context_t));

	    /* Active flag �ʱ�ȭ */
	    s_stSbtlContext[i].usPid = PID_NULL;
	}

	hErr = VK_TASK_Start(s_ulSbtlTaskId);
	if(hErr != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

	/* Callback�� ��� �Ѵ�. */
	hErr = PAL_PES_RegisterSbtlCallBack((PalPes_SBTL_Notify_t)svc_sbtl_CbExcutePacket);
	if(hErr != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

	/* PES subtitle�� ���� �Ѵ�. */
	hErr = PAL_PES_StartSbtlTask();
	if(hErr != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

#ifdef	CONFIG_DEBUG
	SVC_SBTL_InitCmd();
#endif
}


HERROR SVC_SBTL_StartService(Handle_t hAction, HUINT16 usPid, HUINT16 usCompPageId, HUINT16 usAncPageId)
{
	HERROR     	hErr;
	SvcSbtl_Msg_t  sbtlMsg;

	HxLOG_Info("SVC_SBTL_StartService(hAction:0x%x, PID:%d)\n", (int)hAction, (int)usPid);

	sbtlMsg.startMsg.ulCmd = CMD_SBTL_START;
	sbtlMsg.startMsg.hAction = hAction;
	sbtlMsg.startMsg.usPid = usPid;
	sbtlMsg.startMsg.usCompPageId = usCompPageId;
	sbtlMsg.startMsg.usAncPageId = usAncPageId;

	hErr = VK_MSG_Send(s_ulSbtlMsgQId, &sbtlMsg, sizeof(SvcSbtl_Msg_t));
	if(hErr != VK_OK)
	{
	    HxLOG_Critical("\n\n");
	    return ERR_FAIL;
	}

	return ERR_OK;
}


HERROR SVC_SBTL_StopService(Handle_t hAction, ApiSyncMode_t syncMode)
{
	HERROR     	hErr;
	SvcSbtl_Msg_t  sbtlMsg;

	HxLOG_Info("SVC_SBTL_StopService(hAction:0x%x, syncMode:%d)\n", (int)hAction, (int)syncMode);

	sbtlMsg.stopMsg.ulCmd = CMD_SBTL_STOP;
	sbtlMsg.stopMsg.hAction = hAction;
	sbtlMsg.stopMsg.syncMode = syncMode;			//	We don't need sync function in multi-process model.

	hErr = VK_MSG_Send(s_ulSbtlMsgQId, &sbtlMsg, sizeof(SvcSbtl_Msg_t));
	if ( hErr != VK_OK )
	{
	    HxLOG_Critical("\n\n");
	    return ERR_FAIL;
	}

	return ERR_OK;
}


HERROR SVC_SBTL_ChangePid(Handle_t hAction, HUINT16 usPid)
{
	HERROR     	hErr;
	SvcSbtl_Msg_t  sbtlMsg;

	HxLOG_Info("SVC_SBTL_ChangePid(hAction:0x%x, PID:%d)\n", (int)hAction, (int)usPid);

	sbtlMsg.changePidMsg.ulCmd = CMD_SBTL_CHANGE_PID;
	sbtlMsg.changePidMsg.hAction = hAction;
	sbtlMsg.changePidMsg.usPid = usPid;

	hErr = VK_MSG_Send(s_ulSbtlMsgQId, &sbtlMsg, sizeof(SvcSbtl_Msg_t));
	if ( hErr != VK_OK )
	{
	    HxLOG_Critical("\n\n");
	    return ERR_FAIL;
	}

	return ERR_OK;
}


HERROR SVC_SBTL_SetMasterAction(HUINT32 ulMasterActionId)
{
	HERROR     	hErr;
	SvcSbtl_Msg_t  sbtlMsg;

	HxLOG_Info("SVC_SBTL_SetMasterAction(ulMasterActionId:0x%x)\n", (int)ulMasterActionId);

	sbtlMsg.setMasterMsg.ulCmd = CMD_SBTL_SET_MASTER;
	sbtlMsg.setMasterMsg.ulMasterActionId = ulMasterActionId;

	hErr = VK_MSG_Send(s_ulSbtlMsgQId, &sbtlMsg, sizeof(SvcSbtl_Msg_t));
	if ( hErr != VK_OK )
	{
	    HxLOG_Critical("\n\n");
	    return ERR_FAIL;
	}

	return ERR_OK;
}


HERROR SVC_SBTL_SetOnOffDecoder(Handle_t hAction, HBOOL	bOffDec)
{
	HxLOG_Critical("Please Remove function (%s) in your module!!\n", __FUNCTION__);
	HxLOG_Critical("OCTO 2.0 is based on Multi-Process Model\nYou can remove called function by other method.\n");

	return ERR_OK;
}

void SVC_SBTL_InitSbtlDecoderForAction(Handle_t hAction)
{
	HxLOG_Critical("--> nothing to do in broadcasting mode...\n");
}

void SVC_SBTL_TermSbtlDecoder(Handle_t hAction)
{
	HxLOG_Critical("--> nothing to do in broadcasting mode...\n");
}

void SVC_SBTL_SendSubtitleData(Handle_t hAction, HUINT8 *buffer, HINT32 len)
{
	HINT32			newLen;
	HUINT8			*pucBuffer, *p;
	SvcSbtl_Context_t	*pCt = svc_sbtl_GetContext(PAL_PIPE_GetActionId(hAction));

	newLen = len + 12;
	p = pucBuffer = (HUINT8*)HLIB_STD_MemAlloc(newLen);
	HxMACRO_Put32Bit(p, pCt->usCompPageId);		p += 4;
	HxMACRO_Put32Bit(p, pCt->usAncPageId);		p += 4;
	HxMACRO_Put32Bit(p, hAction);				p += 4;
	HxSTD_MemCopy(p, buffer, len);

	HxLOG_Debug("[%s] Len(%d)\n", __FUNCTION__, newLen);
	PAL_DSTREAM_Broadcast(DATA_STREAM_DVB_SUBTITLE, newLen, (void*)pucBuffer);
	HLIB_STD_MemFree(pucBuffer);
}

HERROR SVC_SBTL_DecodeSubtitle(HUINT16 usCompPageId, HUINT16 usAncPageId, HUINT8 *pPes, HUINT32 ulPesSize, HUINT32 *ulTimeOut)
{
	HxLOG_Critical("Please Remove function (%s) in your module!!\n", __FUNCTION__);
	HxLOG_Critical("OCTO 2.0 is based on Multi-Process Model\nYou can remove called function by other method.\n");

	return ERR_FAIL;
}

void SVC_SBTL_ClearSubtitleArea(void)
{
	HxLOG_Critical("Please Remove function (%s) in your module!!\n", __FUNCTION__);
	HxLOG_Critical("OCTO 2.0 is based on Multi-Process Model\nYou can remove called function by other method.\n");
}

void SVC_SBTL_HideSubtitleScreen(void)
{
	HxLOG_Critical("Please Remove function (%s) in your module!!\n", __FUNCTION__);
	HxLOG_Critical("OCTO 2.0 is based on Multi-Process Model\nYou can remove called function by other method.\n");
}

void SVC_SBTL_ShowSubtitleScreen(void)
{
	HxLOG_Critical("Please Remove function (%s) in your module!!\n", __FUNCTION__);
	HxLOG_Critical("OCTO 2.0 is based on Multi-Process Model\nYou can remove called function by other method.\n");
}

#ifdef CONFIG_DEBUG
STATIC HINT32 svc_sbtl_DbgPrint(void *szArgList);
STATIC HINT32 svc_sbtl_DbgStartSbtl(void *szArgList);
STATIC HINT32 svc_sbtl_DbgStopSbtl(void *szArgList);
STATIC HINT32 svc_sbtl_DbgChPidSbtl(void *szArgList);
STATIC HINT32 svc_sbtl_DbgSetMasterSbtl(void *szArgList);
STATIC HINT32 svc_sbtl_DbgOnOffDecSbtl(void *szArgList);

HERROR SVC_SBTL_PrintStatus(void)
{
	HUINT32		i;
	SvcSbtl_Context_t	*pCt = NULL;

	HLIB_CMD_Printf("[[[Subtitle Action Status!]]]\n");
	HLIB_CMD_Printf("Master Action ID = (%d)\n", (int)s_ulMasterActionid);

	for ( i = 0; i < NUM_MAX_ACTION; i++ )
	{
	    pCt = svc_sbtl_GetContext(i);
	    HLIB_CMD_Printf("\tAction ID = (%d) & Status = (%s) & PID = (0x%x) & CompPageId = (%d) & AncPageId = (%d)\n", (int)((pCt->hAction & 0xff000000) >> 24)
	    , (char *)(pCt->status == eSBTL_STATUS_NONE) ? "eSBTL_STATUS_NONE" : ((pCt->status == eSBTL_STATUS_RUNNING) ? "eSBTL_STATUS_RUNNING" : "eSBTL_STATUS_WAITING")
	    , (int)pCt->usPid, (int)pCt->usCompPageId, (int)pCt->usAncPageId);
	}

	return HxCMD_OK;
}

void SVC_SBTL_InitCmd(void)
{
#define	hCmdHandle		"svc_sbtl"

	HLIB_CMD_RegisterWord(hCmdHandle,svc_sbtl_DbgPrint,
			"printsbtl",	 /* keyword */
			"print context info(Subtitle)",		/* help  */
			"printsbtl");/*  usage  */

	HLIB_CMD_RegisterWord(hCmdHandle,svc_sbtl_DbgStartSbtl,
			"startsbtl",	/* keyword */
			"Start Subtitle",		/*   help  */
			"startsbtl [action_id] [pid(%d)]");/*  usage  */

	HLIB_CMD_RegisterWord(hCmdHandle,svc_sbtl_DbgStopSbtl,
			"stopsbtl",    /* keyword */
			"stop Subtitle",       /*   help  */
			"stopsbtl [action_id] [sync or async(HBOOL)]");/*  usage  */

	HLIB_CMD_RegisterWord(hCmdHandle,svc_sbtl_DbgChPidSbtl,
			"chpidsbtl",    /* keyword */
			"Change pid",       /*   help  */
			"chpidsbtl [action_id] [pid(%d)]");/*  usage  */

	HLIB_CMD_RegisterWord(hCmdHandle,svc_sbtl_DbgSetMasterSbtl,
			"mastersbtl",    /* keyword */
			"Set Master Action Id",       /*   help  */
			"mastersbtl");/*  usage  */

	HLIB_CMD_RegisterWord(hCmdHandle,svc_sbtl_DbgOnOffDecSbtl,
			"sbtldec",    /* keyword */
			"Set dec On/Off",       /*   help  */
			"sbtldec [hAction] [bOff(BOOL)]");/*  usage  */
}

STATIC HINT32 svc_sbtl_DbgPrint(void *szArgList)
{
    SVC_SBTL_PrintStatus();

	return HxCMD_OK;
}

STATIC HINT32 svc_sbtl_DbgStartSbtl(void *szArgList)
{
    HCHAR          *aucArg_1=NULL, *aucArg_2=NULL;
    HCHAR          aucStr1[20], aucStr2[20];
    unsigned long   arg1 = 0, arg2 = 0;
    char			*szCmd = (char *)szArgList;
    Handle_t hAction = HANDLE_NULL;

    /* 1st argument */
    aucArg_1 = HLIB_CMD_GetNextArg((HCHAR**)&szCmd);
    if( aucArg_1 != NULL )
    {
        if( sscanf(aucArg_1, "%s", (HCHAR*)&aucStr1) == 0 )
            return HxCMD_ERR;

        arg1 = atoi(aucStr1);
    }

    /* 2nd argument */
    aucArg_2 = HLIB_CMD_GetNextArg((HCHAR**)&szCmd );
    if( aucArg_2 != NULL )
    {
        if( sscanf(aucArg_2, "%s", (HCHAR*)&aucStr2) == 0 )
            return HxCMD_ERR;

        arg2 = atoi(aucStr2);
    }

    hAction = PAL_PIPE_GetActionHandle(arg1);
    if (HANDLE_NULL==hAction)
    {
        return HxCMD_ERR;
    }

    SVC_SBTL_StartService(hAction, (HUINT16)arg2, 0xff, 0xff);

    return HxCMD_OK;
}

STATIC HINT32 svc_sbtl_DbgStopSbtl(void *szArgList)
{
    HCHAR          *aucArg_1=NULL, *aucArg_2=NULL;
    HCHAR          aucStr1[20], aucStr2[20];
    unsigned long   arg1 = 0, arg2 = 0;
    char			*szCmd = (char *)szArgList;
    Handle_t hAction = HANDLE_NULL;

    /* 1st argument */
    aucArg_1 = HLIB_CMD_GetNextArg((HCHAR**)&szCmd);
    if( aucArg_1 != NULL )
    {
        if( sscanf(aucArg_1, "%s", (HCHAR*)&aucStr1) == 0 )
            return HxCMD_ERR;

        arg1 = atoi(aucStr1);
    }

    /* 2nd argument */
    aucArg_2 = HLIB_CMD_GetNextArg((HCHAR**)&szCmd );
    if( aucArg_2 != NULL )
    {
        if( sscanf(aucArg_2, "%s", (HCHAR*)&aucStr2) == 0 )
            return HxCMD_ERR;

        arg2 = atoi(aucStr2);
    }

    hAction = PAL_PIPE_GetActionHandle(arg1);
    if (HANDLE_NULL==hAction)
	{
        return HxCMD_ERR;
    }

    SVC_SBTL_StopService(hAction,  arg2);

    return HxCMD_OK;
}

STATIC HINT32 svc_sbtl_DbgChPidSbtl(void *szArgList)
{
    HCHAR		*aucArg_1=NULL, *aucArg_2=NULL;
    HCHAR		aucStr1[20], aucStr2[20];
    unsigned long	arg1 = 0, arg2 = 0;
    char			*szCmd = (char *)szArgList;

    /* 1st argument */
    aucArg_1 = HLIB_CMD_GetNextArg((HCHAR**)&szCmd);
    if( aucArg_1 != NULL )
    {
        if( sscanf(aucArg_1, "%s", (HCHAR*)&aucStr1) == 0 )
            return HxCMD_ERR;

        arg1 = atoi(aucStr1);
    }

    /* 2nd argument */
    aucArg_2 = HLIB_CMD_GetNextArg((HCHAR**)&szCmd );
    if( aucArg_2 != NULL )
    {
        if( sscanf(aucArg_2, "%s", (HCHAR*)&aucStr2) == 0 )
            return HxCMD_ERR;

        arg2 = atoi(aucStr2);
    }

    SVC_SBTL_ChangePid((arg1 << 24),  arg2);

    return HxCMD_OK;
}

STATIC HINT32 svc_sbtl_DbgSetMasterSbtl(void *szArgList)
{
    HCHAR		*aucArg_1=NULL;
    HCHAR		aucStr1[20];
    unsigned long	arg1 = 0;
    char			*szCmd = (char *)szArgList;

    /* 1st argument */
    aucArg_1 = HLIB_CMD_GetNextArg((HCHAR**)&szCmd);
    if( aucArg_1 != NULL )
    {
        if( sscanf(aucArg_1, "%s", (HCHAR*)&aucStr1) == 0 )
            return HxCMD_ERR;

        arg1 = atoi(aucStr1);
    }

    SVC_SBTL_SetMasterAction(arg1);

    return HxCMD_OK;

}

STATIC HINT32 svc_sbtl_DbgOnOffDecSbtl(void *szArgList)
{
    HCHAR		*aucArg_1=NULL, *aucArg_2=NULL;
    HCHAR		aucStr1[20], aucStr2[20];
    unsigned long	arg1 = 0, arg2 = 0;
    char			*szCmd = (char *)szArgList;

    /* 1st argument */
    aucArg_1 = HLIB_CMD_GetNextArg((HCHAR**)&szCmd);
    if( aucArg_1 != NULL )
    {
        if( sscanf(aucArg_1, "%s", (HCHAR*)&aucStr1) == 0 )
            return HxCMD_ERR;

        arg1 = atoi(aucStr1);
    }

    /* 2nd argument */
    aucArg_2 = HLIB_CMD_GetNextArg((HCHAR**)&szCmd );
    if( aucArg_2 != NULL )
    {
        if( sscanf(aucArg_2, "%s", (HCHAR*)&aucStr2) == 0 )
            return HxCMD_ERR;

        arg2 = atoi(aucStr2);
    }

	SVC_SBTL_SetOnOffDecoder((arg1 << 24),  arg2);

    return HxCMD_OK;

}

#endif

