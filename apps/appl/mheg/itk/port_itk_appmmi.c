/**
	@file	  port_itk_appmmi.c													\n
	@brief	  itk file for CI+ Application MMI									\n

	Module: MW/mheg	/itk								 						\n

	Copyright (c) 2008 HUMAX Co., Ltd.											\n
	All rights reserved.														\n

	[Description]

*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <dlib.h>
#include <apk.h>

#include <mheg_int.h>

#include "port_itk_main.h"
#include "port_itk_service.h"
#include "port_itk_appmmi.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define MAX_LEN_APP_MMI_DOMAIN_NAME			256
#define MAX_LEN_APP_MMI_INIT_OBJ			256

#if defined(CONFIG_MW_CI_PLUS_V_1_3)
#define ITK_AMMI_APP_DOMAIN_ID_OPTION_SSM_0 	"SSM=0"
#define ITK_AMMI_APP_DOMAIN_ID_OPTION_SSM_1 	"SSM=1"
#define ITK_AMMI_APP_DOMAIN_ID_OPTION_SSM_2 	"SSM=2"
#endif
/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	ItkMmi_status_reqStart 			= 0x0001,
	ItkMmi_status_fileReq			= 0x0002,
	ItkMmi_status_fileAck			= 0x0004,
	ItkMmi_status_abort_by_user		= 0x0008,
	ItkMmi_status_abort_by_system 	= 0x0010,
	ItkMmi_status_abort   			= (ItkMmi_status_abort_by_user | ItkMmi_status_abort_by_system),
	ItkMmi_status_abortAck 			= 0x0020

}ITK_AppMMIContext_Status_t;

typedef struct tagItkAppMmiSessionHandle
{
	itk_appmmi_session_t		vtbl;

	HUINT16						usSessionId;
	itk_appmmi_callback_t		callback;
	void*						pContext;
}ITK_AppMmiSessionHandle_t;

typedef struct tagItkAppMmiContext
{
	ITK_AppMmiSessionHandle_t*	pSession;

	HUINT8						ucDomainId[MAX_LEN_APP_MMI_DOMAIN_NAME];
	HUINT8						ucDomainIdLen;

	HUINT8						ucInitialObject[MAX_LEN_APP_MMI_INIT_OBJ];
	HUINT8						ucInitialObjLen;

	ITK_AppMMIContext_Status_t	status;
}ITK_AppMmiContext_t;


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static itk_appmmi_t					s_stAppMmiObject;
static ITK_AppMmiContext_t			s_stContext;

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static void 				mheg_itk_port_appmmiReleaseSession(struct itk_appmmi_session_t *thiz);
static itk_appmmi_status_t	mheg_itk_port_appmmiSendMessage(struct itk_appmmi_session_t *thiz, itk_appmmi_message_t type, itk_appmmi_msg_t msg);

static int 					mheg_itk_port_appmmiCiMessageProcess(CI_AMMI_MSG_TYPE enMsgType, CI_AMMI_CONTENT *pustContent);
static void					mheg_itk_port_appmmiCiMessageCallback(CI_AMMI_MSG_TYPE ulMsgType, void* pvUserData, HUINT32 ulSize);
static HUINT32 				mheg_itk_port_appmmiProcessReqStart(CI_AMMI_CONTENT* pstContent);
static HUINT32 				mheg_itk_port_appmmiProcessFileAck(CI_AMMI_CONTENT* pstContent);
static HUINT32 				mheg_itk_port_appmmiProcessAppAbortBySystem(CI_AMMI_CONTENT* pstContent);
static HUINT32 				mheg_itk_port_appmmiProcessAppAbortAck(CI_AMMI_CONTENT* pstContent);
#if defined(CONFIG_MW_CI_PLUS_V_1_3)
static void local_itk_processAdiOptions(itk_appmmi_message_t sessionMsg, itk_appmmi_msg_t mmiMsg);
#endif

/********************************************************************/
/********************      proto-type functions  ********************/
/********************************************************************/


/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

/**
 * Release session
 */
static void mheg_itk_port_appmmiReleaseSession(struct itk_appmmi_session_t *thiz)
{
	if(thiz == NULL)
	{
		HxLOG_Error("ERROR : input parameter is NULL !!! \n");
		return;
	}

	HLIB_STD_MemFree((void*)thiz);
	s_stContext.pSession = NULL;
	HxLOG_Debug("Release AppMmi Session ! \n");
}

 /**
  * Sends a file request or a CI message.
  *
  * The call creates either a FileRequest or CI message. The request provides
  * either the FleNameByte data or the CI message data.
  * AppMMI is not able to handle multiple simultaneous requests, if
  * a request is made before the previous one has been handled the
  * call returns ITK_APPMMI_STATUS_BUSY.
  *
  * @param[in] thiz The appmmi session
  * @param[in] type The message type, only ITK_APPMMI_FILE_REQUEST
  * @param[in] msg  The file request.
  *
  * @return ITK_APPMMI_STATUS_OK if the request is sent
  * @return ITK_APPMMI_STATUS_BUSY if there is a pending request
  */
static itk_appmmi_status_t mheg_itk_port_appmmiSendMessage(struct itk_appmmi_session_t *thiz, itk_appmmi_message_t type, itk_appmmi_msg_t msg)
{
	ITK_AppMmiSessionHandle_t*	pMmiSession = NULL;
	itk_appmmi_status_t			ret = ITK_APPMMI_STATUS_NOT_AVAILABLE;

	HxLOG_Debug("File Request : type(%d/%d), length(%ld), status(0x%x) \n", type, msg.fileReq.requestType, msg.fileReq.length, s_stContext.status);

	if((pMmiSession = (ITK_AppMmiSessionHandle_t*)thiz) == NULL)
	{
		HxLOG_Error("ERROR : invalid parameter !!! \n");
		return ITK_APPMMI_STATUS_BUSY;
	}

	if(pMmiSession->usSessionId != s_stContext.pSession->usSessionId)
	{
		HxLOG_Error("ERROR : mismatched sessionId(%d, %d) \n", s_stContext.pSession->usSessionId, pMmiSession->usSessionId);
		return ITK_APPMMI_STATUS_BUSY;
	}

	if(type != ITK_APPMMI_FILE_REQUEST)
	{
		HxLOG_Error("ERROR : invalid type (%d) \n", type);
		return ITK_APPMMI_STATUS_BUSY;
	}

	if(s_stContext.status & ItkMmi_status_reqStart)
	{
	#if 0
		if((s_stContext.status & ItkMmi_status_fileReq) && !(s_stContext.status & ItkMmi_status_fileAck))
		{
			HxLOG_Debug("current MMI Session(%d) has already requested FILE, Status(0x%x)  \n", s_stContext.pSession->usSessionId, s_stContext.status );
			return ITK_APPMMI_STATUS_BUSY;
		}
	#endif
		/* reset status */
		s_stContext.status &= ~ItkMmi_status_fileAck;
		s_stContext.status &= ~ItkMmi_status_fileReq;

		if (msg.fileReq.requestType == 2) // CI_AMMI_REQUEST_TYPE_FILEHASH
		{
			HxLOG_Debug("Request File: %d, %s \n", msg.fileReq.requestType, msg.fileReq.byte+16);
		}
		else
		{
			HxLOG_Debug("Request File: %d, %s \n", msg.fileReq.requestType, msg.fileReq.byte);
		}

		// should be replaced using async technique
		if(APK_CAS_CI_RequestFileorData(s_stContext.pSession->usSessionId, msg.fileReq.requestType, (unsigned char *)msg.fileReq.byte, msg.fileReq.length) != ERR_OK)
		{
			HxLOG_Error("ERROR : fail to call CI_RequestFileorData ~~\n");
			ret = ITK_APPMMI_STATUS_BUSY;
		}
		else
		{
			ret = ITK_APPMMI_STATUS_OK;
			s_stContext.status |= ItkMmi_status_fileReq;
		}
	}

	HxLOG_Debug("--- (%d)\n", ret);

	return ret;
}

static itk_appmmi_status_t mheg_itk_port_appmmiSendMessageToClient(itk_appmmi_message_t sessionMsg, itk_appmmi_msg_t mmiMsg)
{
	itk_appmmi_status_t	ret = ITK_APPMMI_STATUS_OK;
	HUINT8				ucLoopCnt = 0;

	if(s_stContext.pSession->callback)
	{
#if defined(CONFIG_MW_CI_PLUS_V_1_3)
		// AppMmi 실행이전에 수행
		local_itk_processAdiOptions(sessionMsg, mmiMsg);

		if (sessionMsg == ITK_APPMMI_START)
		{
			// ADI parameter 를 제외한 length를 설정해야 Redkey에서 error 가 발생하지 않음
			mmiMsg.reqStart.appDomainIdentifierLength = strlen(CI_AMMI_APP_DOMAIN_ID);
		}
#endif

		while(ucLoopCnt < 200)
		{
			if((ret = s_stContext.pSession->callback(s_stContext.pSession->pContext, sessionMsg, mmiMsg)) == ITK_APPMMI_STATUS_OK)
				break;
			else
			{
				if (ret == ITK_APPMMI_STATUS_BUSY)
				{
					ucLoopCnt++;
				}
				else
				{
					break;
				}
			}

			HLIB_STD_TaskSleep(30);
		}

		if (sessionMsg == ITK_APPMMI_START)
		{
			/* set context status*/
			HxLOG_Debug("request to ITK : REQ START !!\n");
			HxLOG_Debug("DomainID: %s, len(%d) \n", mmiMsg.reqStart.appDomainIdentifier, mmiMsg.reqStart.appDomainIdentifierLength);
			HxLOG_Debug("initialObj :%s, len(%d) \n", mmiMsg.reqStart.initialObject, mmiMsg.reqStart.initialObjectLength);

			s_stContext.status |= ItkMmi_status_reqStart;

		}
		else if (sessionMsg == ITK_APPMMI_FILE_ACK	)
		{
			/* send FileAck message to CI+ Browser */
			HxLOG_Debug("req ITK File ACK : len=%ld, datatytes=0x%x \n", mmiMsg.fileAck.data.length, (HUINT32)mmiMsg.fileAck.data.byte);

			s_stContext.status |= ItkMmi_status_fileAck;
		}
		else if (sessionMsg == ITK_APPMMI_ABORT)
		{
			HxLOG_Debug("Request Session(0x%x) Abort !! \n", s_stContext.pSession->usSessionId);
			s_stContext.status |= ItkMmi_status_abort_by_system;
		}
	}

	return ret;
}

#if defined(CONFIG_MW_CI_PLUS_V_1_3)
static void local_itk_processAdiOptions(itk_appmmi_message_t sessionMsg, itk_appmmi_msg_t mmiMsg)
{
	HUINT32 hAction;

	PORT_ITK_GetMastActionHandle(&hAction);

	if (sessionMsg == ITK_APPMMI_START)
	{
		// in case of CI+ : <applicationDomainIndentifier>[;<option1>;<option2>;…;<option#>], <applicationDomainIndentifier> = CI_AMMI_APP_DOMAIN_ID
		if ((mmiMsg.reqStart.appDomainIdentifierLength > strlen(CI_AMMI_APP_DOMAIN_ID)) && (HxSTD_MemCmp(mmiMsg.reqStart.appDomainIdentifier, CI_AMMI_APP_DOMAIN_ID, strlen(CI_AMMI_APP_DOMAIN_ID)) == 0))
		{
			unsigned char	*delimiter = ";";
			unsigned char	*p = NULL;
			HINT32			nDelimiterSize = HxSTD_StrLen(delimiter);

			// get Application Domain Identifier options that separated by a semi-colon (;)
			p = (HINT8*)HxSTD_StrStr((const char*)mmiMsg.reqStart.appDomainIdentifier, (const char*)delimiter);
			while ( p != NULL )
			{
				p += nDelimiterSize;
				if (HxSTD_MemCmp(p, ITK_AMMI_APP_DOMAIN_ID_OPTION_SSM_0, HxSTD_StrLen(ITK_AMMI_APP_DOMAIN_ID_OPTION_SSM_0)) == 0)
				{
					// Subtitles (RTGraphics) shall be disabled before the CI Plus Application is started,
					// subtitles shall be returned to their existing running state when the CI Plus Application terminates.
					ItkTrace(("ADI Launch Options SSM=0 \n")); // Application Domain Identifier Launch Options

					port_itk_UtilPostMsgToAp(MSG_MHEG_DISPLAY_SUBTITLE_CONTROL, hAction, FALSE, 0, 0);
				}
				else if (HxSTD_MemCmp(p, ITK_AMMI_APP_DOMAIN_ID_OPTION_SSM_1, HxSTD_StrLen(ITK_AMMI_APP_DOMAIN_ID_OPTION_SSM_1)) == 0)
				{
					// Subtitles (RTGraphics) shall be displayed when enabled by any user preference,
					// if the CI Plus Application and subtitles are not able to co-exist then the CI Plus Application shall not start.
					ItkTrace(("ADI Launch Options SSM=1 \n")); // Application Domain Identifier Launch Option

					port_itk_UtilPostMsgToAp(MSG_MHEG_DISPLAY_SUBTITLE_CONTROL, hAction, TRUE, 0, 0);
				}
				else if (HxSTD_MemCmp(p, ITK_AMMI_APP_DOMAIN_ID_OPTION_SSM_2, HxSTD_StrLen(ITK_AMMI_APP_DOMAIN_ID_OPTION_SSM_2)) == 0)
				{
					// Subtitles (RTGraphics) shall optionally be displayed when enabled by any user preference,
					// if the CI Plus Application and subtitles are not able to co-exist then subtitles shall be disabled
					// and the CI Plus Application shall launch. Where the subtitle state temporarily over-rides the user preference
					// and are disabled then the existing subtitle state shall be restored when the application terminates.
					// This option is the default state that shall be assumed when the SSM option is omitted from the application domain specifier.
					ItkTrace(("ADI Launch Options SSM=2 \n")); // Application Domain Identifier Launch Option

					port_itk_UtilPostMsgToAp(MSG_MHEG_DISPLAY_SUBTITLE_CONTROL, hAction, TRUE, 0, 0);
				}

				p = (HINT8*)HxSTD_StrStr((const char*)p, (const char*)delimiter);
			}
		}
		else if ((mmiMsg.reqStart.appDomainIdentifierLength == HxSTD_StrLen(UK_AMMI_APP_DOMAIN_ID)) && (HxSTD_MemCmp(mmiMsg.reqStart.appDomainIdentifier, UK_AMMI_APP_DOMAIN_ID, mmiMsg.reqStart.appDomainIdentifierLength) == 0))
		{
			// TODO: UK profile에서 CI 모듈을 사용할 경우에 처리
		}
	}
}
#endif

static HUINT32 mheg_itk_port_appmmiProcessReqStart(CI_AMMI_CONTENT* pstContent)
{
	CI_AMMI_TYPE_REQ_START*		pReqStart = NULL;
	itk_appmmi_msg_t			mmiMsg;
	itk_appmmi_status_t		ret = ITK_APPMMI_STATUS_OK;
	HUINT32 				ulViewId;

	HxLOG_Debug(":: CI_AMMI_MSG_REQ_START \n");

	if(pstContent == NULL)
	{
		HxLOG_Error("ERROR : invalid parameter !!! \n");
		return ITK_APPMMI_STATUS_DOMAIN;
	}

	if(s_stContext.pSession == NULL)
	{
		HxLOG_Error("ERROR : has not been created MMI Session Handler yet !!! \n");
		return ITK_APPMMI_STATUS_DOMAIN;
	}

	/* check if previous session has been aborted, if not we should abort first !!!!! */
	if(!(s_stContext.status & ItkMmi_status_abort))
	{
		/* make abort forcely */
		HxLOG_Debug("session(0x%x) Aborted forcely \n", s_stContext.pSession->usSessionId);
		ret = mheg_itk_port_appmmiSendMessageToClient(ITK_APPMMI_ABORT, mmiMsg);
		if (ITK_APPMMI_STATUS_OK != ret)
		{
			HxLOG_Error("mheg_itk_port_appmmiSendMessageToClient ret:(%d)\n", ret);
		}
	}

	s_stContext.status = 0;

	/* initiate req start value */
	pReqStart = (CI_AMMI_TYPE_REQ_START*)pstContent;

	/* update AppMMI context */
	/* initiate MMI ID */
	s_stContext.pSession->usSessionId = pReqStart->usSessionId;
	/*
	if(pReqStart->ucAppDomainIdLength >= MAX_LEN_APP_MMI_DOMAIN_NAME)
	{
		HxLOG_Error("ERROR : too big Domain Id length (%d) \n", pReqStart->ucAppDomainIdLength);
		return ITK_APPMMI_STATUS_DOMAIN;
	}
	*/
	HxSTD_memset(s_stContext.ucDomainId, 0x00, MAX_LEN_APP_MMI_DOMAIN_NAME);

	HxSTD_memcpy(s_stContext.ucDomainId, pReqStart->pucAppDomainId, pReqStart->ucAppDomainIdLen);
	s_stContext.ucDomainIdLen = pReqStart->ucAppDomainIdLen;

	/* initiate MMI Object */
	/*
	if(pReqStart->ucInitialObjectLength >= MAX_LEN_APP_MMI_INIT_OBJ)
	{
		HxLOG_Error("ERROR : too big Domain Id length (%d) \n", pReqStart->ucInitialObjectLength);
		return ITK_APPMMI_STATUS_DOMAIN;
	}

	*/

	HxSTD_memset(s_stContext.ucInitialObject, 0x00, MAX_LEN_APP_MMI_INIT_OBJ);
	HxSTD_memcpy(s_stContext.ucInitialObject, pReqStart->pucInitialObject, pReqStart->ucInitialObjectLen);
	s_stContext.ucInitialObjLen = pReqStart->ucInitialObjectLen;

	// should be replaced.
	PORT_ITK_GetMastViewId(&ulViewId);
	PORT_ITK_PostMsg(eMHEG_ENGINE_START, ulViewId, 0, 0, 0, 0);

	HLIB_STD_TaskSleep(5);	/* in case of waiting change of redkey status */

	/* make mmi Message */
	mmiMsg.reqStart.appDomainIdentifier 		= s_stContext.ucDomainId;
	mmiMsg.reqStart.appDomainIdentifierLength	= s_stContext.ucDomainIdLen;
	mmiMsg.reqStart.initialObject				= s_stContext.ucInitialObject;
	mmiMsg.reqStart.initialObjectLength			= s_stContext.ucInitialObjLen;

	ret = mheg_itk_port_appmmiSendMessageToClient(ITK_APPMMI_START, mmiMsg);

	return ret;
}

static HUINT32 mheg_itk_port_appmmiProcessFileAck(CI_AMMI_CONTENT* pstContent)
{
	CI_AMMI_TYPE_FILE_ACK*	pFileAck;
	itk_appmmi_msg_t		mmiMsg;
	itk_appmmi_status_t		ret = ITK_APPMMI_STATUS_OK;

	HxLOG_Debug(":: CI_AMMI_MSG_FILE_ACK \n");

	if(pstContent == NULL || s_stContext.pSession == NULL)
	{
		HxLOG_Error("ERROR : invalid parameter !!! \n");
		return ITK_APPMMI_STATUS_BUSY;
	}

//	if(!(s_stContext.status & ItkMmi_status_fileReq))
//	{
//		HxLOG_Error("ERROR : mismatch status (0x%x) \n", s_stContext.status);
//		return ITK_APPMMI_STATUS_BUSY;
//	}

	/* get file ack information */
	pFileAck = (CI_AMMI_TYPE_FILE_ACK*)pstContent;

	/* notify file ACK to CI+ mheg browser */
	HxLOG_Debug("FileOk(%d), reqType(%d), Length(%ld), Byte(0x%x)\n", pFileAck->ucFileOK, pFileAck->enReqType, pFileAck->ulBytesLen, (HUINT32)pFileAck->pucBytes);

	/* check session ID */
	if(pFileAck->usSessionId != s_stContext.pSession->usSessionId)
	{
		HxLOG_Error("ERROR : session mismatched (%d) <->(%d) \n", s_stContext.pSession->usSessionId, pFileAck->usSessionId);
		return ITK_APPMMI_STATUS_BUSY;
	}

	if (0 == pFileAck->ulBytesLen || NULL == pFileAck->pucBytes)
	{
		HxLOG_Error("ERROR : something was wrong on file ack data ! -> return STATUS_BUSY\n");
		return ITK_APPMMI_STATUS_BUSY;
	}

	mmiMsg.fileAck.fileOK = pFileAck->ucFileOK;
//	mmiMsg.fileAck.requestOK = pFileAck->ucRequestOK;
	mmiMsg.fileAck.data.requestType = pFileAck->enReqType;
	mmiMsg.fileAck.data.length = pFileAck->ulBytesLen;
	mmiMsg.fileAck.data.byte = pFileAck->pucBytes;

	ret = mheg_itk_port_appmmiSendMessageToClient(ITK_APPMMI_FILE_ACK, mmiMsg);

	return ret;
}

static HUINT32 mheg_itk_port_appmmiProcessAppAbortBySystem(CI_AMMI_CONTENT* pstContent)
{
	CI_AMMI_TYPE_REQ_APP_ABORT* 	pAbortMsg = NULL;
	itk_appmmi_msg_t				mmiMsg;
	itk_appmmi_status_t				ret = ITK_APPMMI_STATUS_OK;

	HxLOG_Debug(":: CI_AMMI_MSG_REQ_APP_ABORT ~~ \n");

	if(pstContent == NULL || s_stContext.pSession == NULL)
		return ITK_APPMMI_STATUS_DOMAIN;

	pAbortMsg = (CI_AMMI_TYPE_REQ_APP_ABORT*)pstContent;

	if(!(s_stContext.status & ItkMmi_status_reqStart))
	{
		HxLOG_Error("ERROR : Status Error (0x%x) \n", s_stContext.status);
		return ITK_APPMMI_STATUS_DOMAIN;
	}

	/* check session id */
	if(pAbortMsg->usSessionId != s_stContext.pSession->usSessionId)
	{
		HxLOG_Error("ERROR : Session Mismatch (%d)<->(%d) \n", s_stContext.pSession->usSessionId, pAbortMsg->usSessionId);
		return ITK_APPMMI_STATUS_DOMAIN;
	}

#if 1
	ret = mheg_itk_port_appmmiSendMessageToClient(ITK_APPMMI_ABORT, mmiMsg);
#else
	HxLOG_Debug("Request Session(0x%x) Abort !! \n", s_stContext.pSession->usSessionId);
	s_stContext.status |= ItkMmi_status_abort_by_system;
#endif

	return ret;
}

static HUINT32 mheg_itk_port_appmmiProcessAppAbortAck(CI_AMMI_CONTENT* pstContent)
{
	CI_AMMI_TYPE_APP_ABORT_ACK	*		pAbortAckMsg = NULL;

	HxLOG_Debug(":: CI_AMMI_MSG_APP_ABORT_ACK ~ \n");

	if((pAbortAckMsg = (CI_AMMI_TYPE_APP_ABORT_ACK*)pstContent) == NULL)
	{
		HxLOG_Error("ERROR : input parameter is NULL \n");
		return ITK_APPMMI_STATUS_DOMAIN;
	}

	if(!(s_stContext.status & ItkMmi_status_abort))
		HxLOG_Debug("WARNING : has not been sent Request Abort !!! \n");

	if(pAbortAckMsg->usSessionId != s_stContext.pSession->usSessionId)
	{
		HxLOG_Debug("session mismatched : (%d)<-->(%d) \n", s_stContext.pSession->usSessionId, pAbortAckMsg->usSessionId);
		return ITK_APPMMI_STATUS_BUSY;
	}

#if 1
	s_stContext.pSession->usSessionId = 0;
	s_stContext.status = (ItkMmi_status_abortAck | ItkMmi_status_abort);
#else
	s_stContext.status |= (ItkMmi_status_abortAck | ItkMmi_status_abort);
#endif
	return ITK_APPMMI_STATUS_OK;
}

/**
 * callback function from CI module.
 *
 * @param[in] ulType  : types of message from CI Module, whose type is CI_AMMI_MSG_TYPE
 * @param[in] pPipeContent, message contents from CI module.
 *
 */

static void mheg_itk_port_appmmiCiMessageCallback(CI_AMMI_MSG_TYPE enMsgType, void* pvUserData, HUINT32 ulSize)
{
	HxObject_t		*pstObject;
	CI_AMMI_CONTENT		stContent;

	HxLOG_Trace();

	HxLOG_Assert(ulSize && pvUserData);

	pstObject = HLIB_SERIALIZER_Unpack(pvUserData, ulSize);
	if (pstObject == NULL)
	{
		HxLOG_Error("cannot unpack the data \n");
		return ;
	}

	HxLOG_Debug("enMsgType: %d \n", enMsgType);
	switch(enMsgType)
	{
		case CI_AMMI_MSG_REQ_START:
			if (HxOBJECT_ARR_LEN(pstObject) != 3)
			{
				HxLOG_Error("invalid object arr len. \n");
				return ;
			}
			stContent.typeReqStart.usSessionId = HxOBJECT_ARR_VAL(pstObject, 0)->u.num;
			stContent.typeReqStart.ucAppDomainIdLen = HxOBJECT_ARR_VAL(pstObject, 1)->u.bin.size;
			stContent.typeReqStart.pucAppDomainId = HxOBJECT_ARR_VAL(pstObject, 1)->u.bin.data;
			stContent.typeReqStart.ucInitialObjectLen = HxOBJECT_ARR_VAL(pstObject, 2)->u.bin.size;
			stContent.typeReqStart.pucInitialObject = HxOBJECT_ARR_VAL(pstObject, 2)->u.bin.data;

			HxLOG_Debug("usSessionId: %d \n", stContent.typeReqStart.usSessionId);

			break;

		case CI_AMMI_MSG_FILE_ACK:
			if (HxOBJECT_ARR_LEN(pstObject) != 4)
			{
				HxLOG_Error("invalid object arr len. \n");
				return ;
			}
			stContent.typeFileAck.usSessionId = HxOBJECT_ARR_VAL(pstObject, 0)->u.num;
			stContent.typeFileAck.ucFileOK = HxOBJECT_ARR_VAL(pstObject, 1)->u.num;
			stContent.typeFileAck.enReqType = HxOBJECT_ARR_VAL(pstObject, 2)->u.num;
			stContent.typeFileAck.ulBytesLen = HxOBJECT_ARR_VAL(pstObject, 3)->u.bin.size;
			stContent.typeFileAck.pucBytes = HxOBJECT_ARR_VAL(pstObject, 3)->u.bin.data;

			HxLOG_Debug("usSessionId: %d \n", stContent.typeFileAck.usSessionId);

			break;

		case CI_AMMI_MSG_REQ_APP_ABORT:
			if (HxOBJECT_ARR_LEN(pstObject) != 2)
			{
				HxLOG_Error("invalid object arr len. \n");
				return ;
			}
			stContent.typeReqAppAbort.usSessionId = HxOBJECT_ARR_VAL(pstObject, 0)->u.num;
			stContent.typeReqAppAbort.enAbortReqCode = HxOBJECT_ARR_VAL(pstObject, 1)->u.num;

			HxLOG_Debug("usSessionId: %d \n", stContent.typeReqAppAbort.usSessionId);

			break;

		case CI_AMMI_MSG_APP_ABORT_ACK:
			if (HxOBJECT_ARR_LEN(pstObject) != 1)
			{
				HxLOG_Error("invalid object arr len. \n");
				return ;
			}
			stContent.typeAppAbortAck.usSessionId = HxOBJECT_ARR_VAL(pstObject, 0)->u.num;

			HxLOG_Debug("usSessionId: %d \n", stContent.typeAppAbortAck.usSessionId);

			break;

		default:
			HxLOG_Error("ERROR : invalid message type (%d) \n", enMsgType);
			return ;

	}

	mheg_itk_port_appmmiCiMessageProcess(enMsgType, &stContent);

	return ;

}


static int mheg_itk_port_appmmiCiMessageProcess(CI_AMMI_MSG_TYPE enMsgType, CI_AMMI_CONTENT *pustContent)
{
	HUINT32					ret;

	HxLOG_Debug("+++ (%d)\n", enMsgType);

	switch(enMsgType)
	{
		case CI_AMMI_MSG_REQ_START:
			ret = mheg_itk_port_appmmiProcessReqStart(pustContent);
			break;

		case CI_AMMI_MSG_FILE_ACK:
			ret = mheg_itk_port_appmmiProcessFileAck(pustContent);
			break;

		case CI_AMMI_MSG_REQ_APP_ABORT:
			ret = mheg_itk_port_appmmiProcessAppAbortBySystem(pustContent);
			break;

		case CI_AMMI_MSG_APP_ABORT_ACK:
			ret = mheg_itk_port_appmmiProcessAppAbortAck(pustContent);
			break;

		default:
			HxLOG_Error("ERROR : invalid message type (%d) \n", enMsgType);
			return ITK_APPMMI_STATUS_DOMAIN;
	}

	return ret;

}

itk_appmmi_session_t* mheg_itk_port_appmmiNewAppMMISession(struct itk_appmmi_t *thiz, itk_appmmi_callback_t callback, void *callbackContext)
{
	ITK_AppMmiSessionHandle_t*		pstSession = NULL;

	/* check validation of input parameter */
	if(callback == NULL || thiz == NULL)
	{
		HxLOG_Error("WARNING : input parameter is invalid -> NULL \n");
		return NULL;
	}

	/* allocation new memory block for AppMMISession */
	if((pstSession = (ITK_AppMmiSessionHandle_t*)HLIB_STD_MemAlloc(sizeof(ITK_AppMmiSessionHandle_t))) == NULL)
	{
		HxLOG_Error("ERROR : insufficient Memory !!!!!!!!! \n");
		return NULL;
	}

	/* init new memory buffer */
	pstSession->vtbl.release		= mheg_itk_port_appmmiReleaseSession;
	pstSession->vtbl.sendMessage	= mheg_itk_port_appmmiSendMessage;
	pstSession->callback			= callback;
	pstSession->pContext			= callbackContext;

	/* assign new session handler */
	s_stContext.pSession			= pstSession;
	HxLOG_Debug("new APPMMI session Creation success ! \n");

	return (itk_appmmi_session_t*)pstSession;
}

itk_uint32_t mheg_itk_port_appmmiResourceVersion(struct itk_appmmi_t *thiz)
{
	if(thiz == NULL)
	{
		HxLOG_Error("WARNING : input parameter is invalid -> NULL \n");
		return 0;
	}

#if defined (CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE)
	return 2; // CI+ 1.3
#else
	return 1; // CI+ 1.2
#endif
}

void PORT_ITK_APPMMI_Init(void)
{
	/* initiate session handler point */
	HxSTD_memset(&s_stContext, 0x00, sizeof(ITK_AppMmiContext_t));
	s_stContext.status = ItkMmi_status_abort;		/* initial status of Session */

	/* register callback function to CI Layer */
	#ifndef CONFIG_3RD_MHEG_USE_HUMAX_ENGINE
	// 휴맥스 자체 엔진을 S&T 엔진과 함께 사용하도록 configuration 되어 있는 경우에 CI Application MMI 처리는 휴맥스 자체 엔진이 하도록 한다.
	// S&T 엔진과 동작 비교를 하기 위한 용도를 위해서는 시리얼 커맨드 "MhegIbExit" 를 사용하고 다시 자체 엔진으로 복귀할 때는 "MhegIbInit" 을 사용한다.
	APK_CAS_CI_RegisterAppMmiCallback(mheg_itk_port_appmmiCiMessageCallback);
	#endif

	HxLOG_Debug("Initiage Application MMI Object \n");
}

void PORT_ITK_APPMMI_CreateObject(void)
{
	/* create Applicaiton MMI Object */
	s_stAppMmiObject.newAppMMISession	= mheg_itk_port_appmmiNewAppMMISession;
	s_stAppMmiObject.resourceVersion	= mheg_itk_port_appmmiResourceVersion;

	/* register it to Client */
	ITK_registerManager(ITK_APPMMI, &s_stAppMmiObject);

	HxLOG_Debug("Create APP MMI Object and register it to Client  \n");
}

HERROR PORT_ITK_APPMMI_NotifyEngineStatus(redkey_mheg_app_state_t status)
{
	HxLOG_Debug("+++ \n");

	switch(status)
	{
		case REDKEY_MHEG_APP_STOPPING:
		{
			/* get current session id */
			if(s_stContext.pSession)
			{
				if(s_stContext.pSession->usSessionId != 0 && s_stContext.status & ItkMmi_status_reqStart)
				{
					if(s_stContext.status & ItkMmi_status_fileAck)
					{
						if(s_stContext.status & ItkMmi_status_abort_by_system)
						{
							/* In Octo CI+ stack, we don't need to send AbortAck message to CI+Stack, smj */
							HxLOG_Debug("AbortAck by requesting abort_by_system \n");
						}
						else
						{
							HxLOG_Debug("Status(0x%x) Request MMI Abort Request by User \n",s_stContext.status);
							s_stContext.status |= ItkMmi_status_abort_by_user;
							APK_CAS_CI_RequestAppAbort(s_stContext.pSession->usSessionId, CI_AMMI_ABORT_REQUEST_CODE_USER_CANCEL);
						}
					}
					else
					{
						HxLOG_Debug("Ignore Abork Request to CI+ Stack \n");
					}
				}
			}
		}
			break;

		case REDKEY_MHEG_APP_NOT_RUNNING:
			HxLOG_Debug("Received NOT RUNNING APP ~~~~ \n");
			/* get current session id */

			if(s_stContext.pSession)
			{
				/* CI+ stack은 request start를 ITK MHEG Engine이 not running이 되는 경우에는 request abort를 실행하여 재 실행하도록 함 (ITK app running status 를 debugging 하기 난해함)*/
				if(s_stContext.pSession->usSessionId != 0 && s_stContext.status == ItkMmi_status_reqStart)
				{
					HxLOG_Debug("request APP Abort\n");
					APK_CAS_CI_RequestAppAbort(s_stContext.pSession->usSessionId, CI_AMMI_ABORT_REQUEST_CODE_SYSTEM_CANCEL);

					// s_stContext.status |= ItkMmi_status_abort_by_system;
				}
			}
			break;

		case REDKEY_MHEG_APP_STARTING:
			HxLOG_Debug("Received STARTING APP ~~~~ \n");
			break;

		case REDKEY_MHEG_APP_RUNNING:
			HxLOG_Debug("Received RUNING APP ~~~~ \n");
			break;

		default:
			break;
	}

	return ERR_OK;
}

itk_appmmi_status_t PORT_ITK_APPMMI_STOP(itk_service_tune_event_t eTuneEvent)
{
	itk_appmmi_msg_t	mmiMsg;
	itk_appmmi_status_t	ret = ITK_APPMMI_STATUS_OK;

	if (s_stContext.status == ItkMmi_status_reqStart)
	{
		/* make abort forcely */
		HxLOG_Debug("stop : session(0x%x) Aborted forcely \n", s_stContext.pSession->usSessionId);

		ret = mheg_itk_port_appmmiSendMessageToClient(ITK_APPMMI_ABORT, mmiMsg);

		APK_CAS_CI_RequestAppAbort(s_stContext.pSession->usSessionId, CI_AMMI_ABORT_REQUEST_CODE_SYSTEM_CANCEL);
	}

	return ret;
}

#ifdef CONFIG_DEBUG
void PORT_ITK_APPMMI_DEBUG_ShowStatus()
{
	HxLOG_Debug("------------------------------------------ \n");
	HxLOG_Debug("     CI+ MHEG Broswer APP MMI Status \n");
	HxLOG_Debug("------------------------------------------ \n");
	if(s_stContext.pSession == NULL)
	{
		HxLOG_Debug("No Session is linking \n");
		return;
	}

	HxLOG_Debug("SessionId : 0x%x \n", s_stContext.pSession->usSessionId);
	HxLOG_Debug("Status : 0x%x \n", s_stContext.status);
	if(s_stContext.ucDomainId[0])
		HxLOG_Debug("Domain : %s\n", s_stContext.ucDomainId);
	if(s_stContext.ucInitialObject[0])
		HxLOG_Debug("InitialObject : %s \n", s_stContext.ucInitialObject);

}


#endif /* CONFIG_DEBUG */

//HERROR PORT_ITK_APPMMI_SendMessage(HUINT32 ulSessionId,

