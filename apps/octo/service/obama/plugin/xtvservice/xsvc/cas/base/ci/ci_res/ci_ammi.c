/***************************************************************
* $Workfile:   ci_ammi.c  $
* $Modtime:
* Auther : hmkim
*
* Desc : CI+ Browser Application MMI (CI+)
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
****************************************************************/

/***************************************************************
* #ifdef statements and include
***************************************************************/
#include <ci_ammi.h>



#include <ci_plus.h>

/***************************************************************
* debugging
***************************************************************/
#if defined(_CI_DEBUG_AMMI_USE_EMULATOR_FOR_CAM_)
	static unsigned long	s_ulAmmiEmulMsgQId = 0;
	static unsigned long	s_ulAmmiEmulTaskId = 0;
	static unsigned long	s_ulAmmiEmulTimerId = 0;
	#define CI_AMMI_EMUL_REQ_START_TIME  20000
	#define CI_AMMI_EMUL_FILE_ACK_TIME   1000
	typedef struct {
		CI_AMMI_MSG_TYPE command;
		void * data;
	} CI_AMMI_EMUL_TASK_MSG;
	typedef struct {
		unsigned short usSsNo;
		CI_AMMI_REQUEST_TYPE enReqType;
		unsigned char pucReqName[256]; // PATH_MAX
		unsigned long ulReqNameLen;
	} CI_AMMI_EMUL_FILE_REQ_PARAMS;
	static void CiAmmi_Emul_Task(void);
	static void CiAmmi_Emul_FileAck(unsigned long ulTimerId, void *pvParam);
	static int CiAmmi_LoadFileData(unsigned char ucFileNameLen, unsigned char *pucFileName, unsigned long *pulBytesLen, unsigned char **ppucBytes);
#endif

#ifdef _CI_DEBUG_AMMI_USE_EMULATOR_FOR_MHEG_ENGINE_
	static HUINT32 CiAmmi_Emul_MhegEngine(CI_AMMI_MSG_TYPE enMsgType, CI_AMMI_CONTENT *pustContent);
	static int CiAmmi_ParsingFileData(char *fileToBeParsing);
#endif

#ifdef _CI_DEBUG_AMMI_DECODE_FILE_DATA_
	static int CiAmmi_DecodingFileData(char *fileToBeDecoding);
#endif

#ifdef _CI_DEBUG_AMMI_SAVE_FILE_DATA_
	#include <stdio.h>
	#include <errno.h>
	#define CI_AMMI_FILE_DATA_PATH      "/opt/CI/"
	#define CI_AMMI_FILE_DATA_PATH_LEN 8 // strlen(CI_AMMI_FILE_DATA_PATH)
	static int CiAmmi_SaveFileData(unsigned char ucFileNameLen, unsigned char *pucFileName, unsigned long ulFileDataLen, unsigned char *pucFileData);
#endif

#ifdef _CI_DEBUG_AMMI_DATA_
	#include <string.h>
	#include <ctype.h>
#endif

//  mw_subCas_mw_api.h에 내용들어가면 MWC_DATASTREAMER_Broadcast는 삭제 되어야 한다.
HERROR	PAL_DSTREAM_Broadcast(HCHAR *pszDataType, HUINT32 ulSize, void *pData);

/***************************************************************
* local definition
***************************************************************/
#define CI_AMMI_VerifyReceivedFile	(1) // verify received file not corrupt
#if CI_AMMI_VerifyReceivedFile
#include <stdio.h>
#include <errno.h>
#endif

#define	DATA_STREAM_MHEG_IB	"mhegibdata"

/***************************************************************
* typedef
***************************************************************/


/***************************************************************
* static variables and structures
***************************************************************/
static CI_AMMI_CALLBACK_T s_pfnAmmiCallback = NULL; // local_itk_APPMMI_CIMessageCallback() in case of using S&T MHEG engine

/***************************************************************
* static function prototypes
***************************************************************/
static int CiAmmi_RequestStart(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen);
static int CiAmmi_FileAck(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen);
static int CiAmmi_AppAbort(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen);
static int CiAmmi_AppAbortAck(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen);

/***************************************************************
* function bodies
***************************************************************/
int local_CiAmmi_PumpData(CI_AMMI_MSG_TYPE enMsgType, CI_AMMI_CONTENT *pustContent)
{
	HUINT32			unDataLen = 0;
	HUINT8			*pucBuffer = NULL;
	HUINT8			*p = NULL;

	unDataLen += sizeof(CI_AMMI_MSG_TYPE);					// CI_AMMI_MSG_TYPE

	HxLOG_Debug("[%s] MSG Type(%d)\n", __FUNCTION__, enMsgType);

	switch(enMsgType)
	{
		case CI_AMMI_MSG_REQ_START:
		{
			CI_AMMI_TYPE_REQ_START		*pstTypeReqStart = &pustContent->typeReqStart;

			// calc alloc data length
			unDataLen += sizeof(pstTypeReqStart->usSessionId);			//unsigned short	usSessionId
			unDataLen += sizeof(pstTypeReqStart->ucAppDomainIdLen);		//unsigned char	ucAppDomainIdLen
			unDataLen += pstTypeReqStart->ucAppDomainIdLen;			//unsigned char * 	pucAppDomainId
			unDataLen += sizeof(pstTypeReqStart->ucInitialObjectLen);		//unsigned char	ucInitialObjectLen
			unDataLen += pstTypeReqStart->ucInitialObjectLen;				//unsigned char * 	pucInitialObject

			// alloc
			p = pucBuffer = (HUINT8*)HLIB_STD_MemAlloc(unDataLen);
			if(pucBuffer == NULL)
			{
				return -1;
			}

			// data copy
			HxSTD_MemCopy(p, &enMsgType, sizeof(CI_AMMI_MSG_TYPE)); 										p += sizeof(CI_AMMI_MSG_TYPE);
			HxSTD_MemCopy(p, &pstTypeReqStart->usSessionId, sizeof(pstTypeReqStart->usSessionId)); 				p += sizeof(pstTypeReqStart->usSessionId);
			HxSTD_MemCopy(p, &pstTypeReqStart->ucAppDomainIdLen, sizeof(pstTypeReqStart->ucAppDomainIdLen));	p += sizeof(pstTypeReqStart->ucAppDomainIdLen);
			HxSTD_MemCopy(p, pstTypeReqStart->pucAppDomainId, pstTypeReqStart->ucAppDomainIdLen);			p += pstTypeReqStart->ucAppDomainIdLen;
			HxSTD_MemCopy(p, &pstTypeReqStart->ucInitialObjectLen, sizeof(pstTypeReqStart->ucInitialObjectLen));	p += sizeof(pstTypeReqStart->ucInitialObjectLen);
			HxSTD_MemCopy(p, pstTypeReqStart->pucInitialObject, pstTypeReqStart->ucInitialObjectLen);				p += pstTypeReqStart->ucInitialObjectLen;
		}
		break;

	case CI_AMMI_MSG_FILE_ACK:
	{
		CI_AMMI_TYPE_FILE_ACK	*pstTypeFileAck = &pustContent->typeFileAck;

		// calc alloc data length
		unDataLen += sizeof(pstTypeFileAck->usSessionId);			//unsigned short			usSessionId
		unDataLen += sizeof(pstTypeFileAck->ucFileOK);			//unsigned char			ucFileOK
		unDataLen += sizeof(CI_AMMI_REQUEST_TYPE);			//CI_AMMI_REQUEST_TYPE	enReqType
		unDataLen += sizeof(pstTypeFileAck->ulBytesLen);			//unsigned long			ulBytesLen
		unDataLen += pstTypeFileAck->ulBytesLen;				//unsigned char * 			pucBytes

		// alloc
		p = pucBuffer = (HUINT8*)HLIB_STD_MemAlloc(unDataLen);
		if(pucBuffer == NULL)
		{
			return -1;
		}

		// data copy
		HxSTD_MemCopy(p, &enMsgType, sizeof(CI_AMMI_MSG_TYPE)); 							p += sizeof(CI_AMMI_MSG_TYPE);
		HxSTD_MemCopy(p, &pstTypeFileAck->usSessionId, sizeof(pstTypeFileAck->usSessionId)); 	p += sizeof(pstTypeFileAck->usSessionId);
		HxSTD_MemCopy(p, &pstTypeFileAck->ucFileOK, sizeof(pstTypeFileAck->ucFileOK));			p += sizeof(pstTypeFileAck->ucFileOK);
		HxSTD_MemCopy(p, &pstTypeFileAck->enReqType, sizeof(CI_AMMI_REQUEST_TYPE));		p += sizeof(CI_AMMI_REQUEST_TYPE);
		HxSTD_MemCopy(p, &pstTypeFileAck->ulBytesLen, sizeof(pstTypeFileAck->ulBytesLen));		p += sizeof(pstTypeFileAck->ulBytesLen);
		HxSTD_MemCopy(p, pstTypeFileAck->pucBytes, pstTypeFileAck->ulBytesLen);				p += pstTypeFileAck->ulBytesLen;
	}
		break;

	case CI_AMMI_MSG_REQ_APP_ABORT:
	{
		CI_AMMI_TYPE_REQ_APP_ABORT		*pstTypeReqAppAbort = &pustContent->typeReqAppAbort;

		// calc alloc data length
		unDataLen += sizeof(pstTypeReqAppAbort->usSessionId);	//unsigned short					usSessionId
		unDataLen += sizeof(CI_AMMI_ABORT_REQUEST_CODE);	//CI_AMMI_ABORT_REQUEST_CODE	enAbortReqCode

		// alloc
		p = pucBuffer = (HUINT8*)HLIB_STD_MemAlloc(unDataLen);
		if(pucBuffer == NULL)
		{
			return -1;
		}

		// data copy
		HxSTD_MemCopy(p, &enMsgType, sizeof(CI_AMMI_MSG_TYPE)); 										p += sizeof(CI_AMMI_MSG_TYPE);
		HxSTD_MemCopy(p, &pstTypeReqAppAbort->usSessionId, sizeof(pstTypeReqAppAbort->usSessionId));		p += sizeof(pstTypeReqAppAbort->usSessionId);
		HxSTD_MemCopy(p, &pstTypeReqAppAbort->enAbortReqCode, sizeof(CI_AMMI_ABORT_REQUEST_CODE));	p += sizeof(CI_AMMI_ABORT_REQUEST_CODE);
	}
		break;

	case CI_AMMI_MSG_APP_ABORT_ACK:
	{
		CI_AMMI_TYPE_APP_ABORT_ACK		*pstTypeAppAbortAck = &pustContent->typeAppAbortAck;

		// calc alloc data length
		unDataLen += sizeof(pstTypeAppAbortAck->usSessionId);			//unsigned short	usSessionId

		// alloc
		p = pucBuffer = (HUINT8*)HLIB_STD_MemAlloc(unDataLen);
		if(pucBuffer == NULL)
		{
			return -1;
		}

		HxSTD_MemCopy(p, &enMsgType, sizeof(CI_AMMI_MSG_TYPE)); 									p += sizeof(CI_AMMI_MSG_TYPE);
		HxSTD_MemCopy(p, &pstTypeAppAbortAck->usSessionId, sizeof(pstTypeAppAbortAck->usSessionId)); 	p += sizeof(pstTypeAppAbortAck->usSessionId);
	}
		break;

	case CI_AMMI_MSG_MAX:
	default:
		HxLOG_Error("unknown msg type (%d)\n", enMsgType);
		return -1;
	}

	HxLOG_Debug("[%s] Len(%d)\n", __FUNCTION__, unDataLen);

	PAL_DSTREAM_Broadcast(DATA_STREAM_MHEG_IB, unDataLen, (void*)pucBuffer);

	HLIB_STD_MemFree(pucBuffer);

	return 1;
}

int CiAmmi_Init(void)
{
#if defined(_CI_DEBUG_AMMI_USE_EMULATOR_FOR_CAM_) // CiAmmi_RequestStart()를 에뮬레이트함 : s_pfnAmmiCallback 이 이 시점에 available 하지 않기 때문에 타이머를 걸어서 처리하도록 한다...
	if (!s_ulAmmiEmulMsgQId)
	{
		if (VK_MSG_Create(64, sizeof(CI_AMMI_EMUL_TASK_MSG), "qAmmiEmul", &s_ulAmmiEmulMsgQId, VK_SUSPENDTYPE_PRIORITY))
		{
			HxLOG_Error("[CIAM]---> CiAmmi_Init()---> Error : VK_MSG_Create() !!!\n");
		}
	}
	if (!s_ulAmmiEmulTaskId)
	{
		if (VK_TASK_Create((void*)CiAmmi_Emul_Task, PAL_CI_ITC_LAYER_TASK_PRIORITY, SIZE_64K, "tAmmiEmul", NULL, &s_ulAmmiEmulTaskId, 0))
		{
			HxLOG_Error("[CIAM]---> CiAmmi_Init()---> Error : VK_TASK_Create() !!!\n");
		}
		if (VK_TASK_Start(s_ulAmmiEmulTaskId))
		{
			HxLOG_Error("[CIAM]---> CiAmmi_Init()---> Error : VK_TASK_Start() !!!\n");
		}
	}
	//VK_TIMER_EventAfter(CI_AMMI_EMUL_REQ_START_TIME, CiAmmi_Emul_RequestStart, NULL, 0, (unsigned long *)&s_ulAmmiEmulTimerId);
#endif

#if defined(CONFIG_SUPPORT_MHEG_IB)
	CiAmmi_RegisterCallback(local_CiAmmi_PumpData);
#endif

#ifdef _CI_DEBUG_AMMI_USE_EMULATOR_FOR_MHEG_ENGINE_
	s_pfnAmmiCallback = CiAmmi_Emul_MhegEngine;
#endif

	return ERR_OK;
}

int CiAmmi_SsOpenedProc(unsigned short usSsNo)
{
	return ERR_OK;
}

int CiAmmi_SsClosedProc(unsigned short usSsNo)
{
	CI_AMMI_CONTENT	ustContent;

	HxLOG_Print("[CIAM]---> CiAmmi_SsClosedProc()\n");

	// 캠이 제거되거나 하여 세션이 닫히면 CI+ MHEG porting layer 로 AppAbort 를 날리도록 한다.
	// MHEG application 이 런닝되어 있지 않다면 porting layer 에서 거르기 때문에 걱정하지 않아도 된다.

	if (s_pfnAmmiCallback)
	{
		HxSTD_memset(&ustContent, 0, sizeof(CI_AMMI_CONTENT));
		ustContent.typeReqAppAbort.usSessionId = usSsNo;
		ustContent.typeReqAppAbort.enAbortReqCode = CI_AMMI_ABORT_REQUEST_CODE_SYSTEM_CANCEL;
		s_pfnAmmiCallback(CI_AMMI_MSG_REQ_APP_ABORT, &ustContent); // AbortAckCode 는 받을 필요가 없다...
	}
	else // 이런 경우는 없겠지만 만약 발생한다면... (CONFIG_3RD_MHEG 및 CONFIG_3RD_MHEG_CIPLUS_PROFILE1 이 설정되어 있지 않거나 콜백 등록시 문제 발생한 경우)
	{
		HxLOG_Error("[CIAM]---> CiAmmi_SsClosedProc()---> Error : callback not registered !!!\n");
	}

	HxLOG_Print("[CIAM]---> CiAmmi_SsClosedProc()---> SsNo[%d]...\n", usSsNo);

	return ERR_OK;
}

int CiAmmi_ApduProc(unsigned short usSsNo, unsigned long ulApduTag, unsigned char *pucApduData, unsigned long ulApduLen)
{
	int nRtnCode = ERR_OK;

	switch (ulApduTag)
	{
	case CI_REQ_START:
		/*
		받은 APDU
		=========
		RequestStart() {
			RequestStartTag										= CI_REQ_START
			length_field()
			AppDomainIdentifierLength							8 bits
			InitialObjectLength									8 bits
			for (i = 0; i < AppDomainIdentifierLength; i + +) {
				AppDomainIdentifier								8 bits
			}
			for (i = 0; i < InitialObjectLength; i + +) {
				InitialObject									8 bits
			}
		}

		보낼 APDU
		=========
		RequestStartAck () {
			RequestStartAckTag									= CI_REQ_START_ACK
			length_field()
			AckCode												8 bits
		}
		*/


		nRtnCode = CiAmmi_RequestStart(usSsNo, pucApduData, ulApduLen);
		break;

	case CI_FlLE_ACK:
		/*
		받은 APDU
		=========
		FileAck() {
			FileAckTag											= CI_FlLE_ACK
			length_field()
			Reserved											7 bits
			FileOK												1 bit
			RequestType											8 bits
			if (RequestType == File) {
				FileNameLength									8 bits
				for (i=0; i<FileNameLength; i++) {
					FileNameByte								8 bits
				}
				FileDataLength									32 bits
				for (i=0; i<FileDataLength; i++) {
					FileDataByte								8 bits
				}
			}
			if (RequestType == Data) {
				for (i=0; i<(n-1); i++) {
					DataByte									8 bits
				}
			}
		}
		*/


		nRtnCode = CiAmmi_FileAck(usSsNo, pucApduData, ulApduLen);
		break;

	case CI_APP_ABORT_REQ:
		/*
		받은 APDU
		=========
		AppAbortReq () {
			AppAbortReqTag										= CI_APP_ABORT_REQ
			length_field()
			for (i = 0; i < N; i + +) {
				AbortReqCode									8 bits
			}
		}

		보낼 APDU
		=========
		AppAbortAck () {
			AppAbortAckTag										= CI_APP_ABORT_ACK
			length_field()
			for (i = 0; i < N; i + +) {
				AbortAckCode									8 bits
			}
		}
		*/


		nRtnCode = CiAmmi_AppAbort(usSsNo, pucApduData, ulApduLen);
		break;

	case CI_APP_ABORT_ACK:
		/*
		받은 APDU
		=========
		AppAbortAck () {
			AppAbortAckTag										= CI_APP_ABORT_ACK
			length_field()
			for (i = 0; i < N; i + +) {
				AbortAckCode									8 bits
			}
		}
		*/


		nRtnCode = CiAmmi_AppAbortAck(usSsNo, pucApduData, ulApduLen);
		break;

	default:
		nRtnCode = -1;
		HxLOG_Error("[CIAM]---> CiAmmi_ApduProc()---> Error : Unknown Apdu Tag[0x%lx] !!!\n", ulApduTag);
	}

	HxLOG_Print("[CIAM]---> CiAmmi_ApduProc()---> SsNo[%d], tag[0x%lx], len[%ld]---> %d\n", usSsNo, ulApduTag, ulApduLen, nRtnCode);

	return nRtnCode;
}

int CiAmmi_RegisterCallback(CI_AMMI_CALLBACK_T pfnCallback)
{
#ifndef _CI_DEBUG_AMMI_USE_EMULATOR_FOR_MHEG_ENGINE_

	if (pfnCallback == NULL)
	{
		HxLOG_Error("[CIAM]---> CiAmmi_RegisterCallback()---> Error : NULL callback registered !!!\n");
		return -1;
	}

	if (s_pfnAmmiCallback != NULL)
	{
		HxLOG_Error("[CIAM]---> CiAmmi_RegisterCallback()---> Warning : callback already registered (%p -> %p)\n", s_pfnAmmiCallback, pfnCallback);
	}

	s_pfnAmmiCallback = pfnCallback;

#endif

	return ERR_OK;
}

static int CiAmmi_RequestStart(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen)
{
	unsigned char	ucAppDomainIdLen = 0;
	unsigned char	ucInitialObjectLen = 0;

	unsigned char	*pucAppDomainId = NULL;	// just pointer to data (don't mem alloc for this)
	unsigned char	*pucInitialObject = NULL;	// just pointer to data (don't mem alloc for this)

	CI_AMMI_CONTENT					ustContent;
	CI_AMMI_REQUEST_START_ACK_CODE	enAckCode;

	unsigned char	ucMsg[5];

	if (pucApduData == NULL || ulApduLen < 2)
	{
		HxLOG_Error("[CIAM]---> CiAmmi_RequestStart()---> Error : Wrong ulApduLen[%ld] !!!\n", ulApduLen);
		return -1;
	}

	ucAppDomainIdLen = pucApduData[0];
	ucInitialObjectLen = pucApduData[1];

	HxLOG_Print("[CIAM]---> CiAmmi_RequestStart() : ucAppDomainIdLen = %d, ucInitialObjectLen = %d, ulApduLen = %ld\n", ucAppDomainIdLen, ucInitialObjectLen, ulApduLen);

	if (ulApduLen != 2 + ucAppDomainIdLen + ucInitialObjectLen)
	{
		HxLOG_Error("[CIAM]---> CiAmmi_RequestStart()---> Error : data is something wrong (%ld != 2+%d+%d) !!!\n", ulApduLen, ucAppDomainIdLen, ucInitialObjectLen);
		return -1;
	}

	if (ucAppDomainIdLen)
	{
		pucAppDomainId = &pucApduData[2];

		#if 0 // #ifdef _CI_DEBUG_AMMI_DATA_
		{
			int i;
			HxLOG_Print("[CIAM]---> CiAmmi_RequestStart() : AppDomainId = [%d][", ucAppDomainIdLen);
			for (i = 0; i < ucAppDomainIdLen; i++)
				HxLOG_Print("%c", pucAppDomainId[i]);
			HxLOG_Print("]\n");
		}
		#else
		HxLOG_Print("[CIAM]---> CiAmmi_RequestStart() : AppDomainId = [%d][%.*s]\n", ucAppDomainIdLen, ucAppDomainIdLen, pucAppDomainId);
		#endif
	}

	if (ucInitialObjectLen)
	{
		pucInitialObject = &pucApduData[2+ucAppDomainIdLen];

		#if 0 // #ifdef _CI_DEBUG_AMMI_DATA_
		{
			int i;
			HxLOG_Print("[CIAM]---> CiAmmi_RequestStart() : InitialObject = [%d][", ucInitialObjectLen);
			for (i = 0; i < ucInitialObjectLen; i++)
				HxLOG_Print("%c", pucInitialObject[i]);
			HxLOG_Print("]\n");
		}
		#else
		HxLOG_Print("[CIAM]---> CiAmmi_RequestStart() : InitialObject = [%d][%.*s]\n", ucInitialObjectLen, ucInitialObjectLen, pucInitialObject);
		#endif
	}

	if (s_pfnAmmiCallback)
	{
		HxSTD_memset(&ustContent, 0, sizeof(CI_AMMI_CONTENT));
		ustContent.typeReqStart.usSessionId = usSsNo;
		ustContent.typeReqStart.ucAppDomainIdLen = ucAppDomainIdLen;
		ustContent.typeReqStart.pucAppDomainId = pucAppDomainId;
		ustContent.typeReqStart.ucInitialObjectLen = ucInitialObjectLen;
		ustContent.typeReqStart.pucInitialObject = pucInitialObject;
		enAckCode = (CI_AMMI_REQUEST_START_ACK_CODE)s_pfnAmmiCallback(CI_AMMI_MSG_REQ_START, &ustContent);
		HxLOG_Print("[CIAM]---> CiAmmi_RequestStart() : AckCode = %d\n", enAckCode);
	}
	else // 이런 경우는 없겠지만 만약 발생한다면... (CONFIG_3RD_MHEG 및 CONFIG_3RD_MHEG_CIPLUS_PROFILE1 이 설정되어 있지 않거나 콜백 등록시 문제 발생한 경우)
	{
		// in case of CI+ : <applicationDomainIndentifier>[;<option1>;<option2>;…;<option#>], <applicationDomainIndentifier> = CI_AMMI_APP_DOMAIN_ID
		if ((ucAppDomainIdLen >= strlen(CI_AMMI_APP_DOMAIN_ID)) && (HxSTD_memcmp(pucAppDomainId, CI_AMMI_APP_DOMAIN_ID, strlen(CI_AMMI_APP_DOMAIN_ID)) == 0))
			enAckCode = CI_AMMI_REQUEST_START_ACK_CODE_API_BUSY;
		else if ((ucAppDomainIdLen == strlen(UK_AMMI_APP_DOMAIN_ID)) && (HxSTD_memcmp(pucAppDomainId, UK_AMMI_APP_DOMAIN_ID, ucAppDomainIdLen) == 0))
			enAckCode = CI_AMMI_REQUEST_START_ACK_CODE_API_BUSY;
		else
			enAckCode = CI_AMMI_REQUEST_START_ACK_CODE_WRONG_API;
		HxLOG_Error("[CIAM]---> CiAmmi_RequestStart()---> Error : callback not registered !!!\n");
	}

	ucMsg[0] = (unsigned char)((CI_REQ_START_ACK >> 16) & 0xff);
	ucMsg[1] = (unsigned char)((CI_REQ_START_ACK >> 8) & 0xff);
	ucMsg[2] = (unsigned char)(CI_REQ_START_ACK & 0xff);
	ucMsg[3] = 0x01;
	ucMsg[4] = enAckCode;

	if (CIRH_SendApdu(usSsNo, ucMsg, 5) != ERR_OK)
	{
		HxLOG_Error("[CIAM]---> CiAmmi_RequestStart()---> Error : fail to send CI_REQ_START_ACK for SsNo[%d] !!!\n", usSsNo);
		return -1;
	}

	HxLOG_Print("[CIAM]---> CiAmmi_RequestStart()---> SsNo[%d]...\n", usSsNo);

	return ERR_OK;
}

int CiAmmi_FileRequest(unsigned short usSsNo, CI_AMMI_REQUEST_TYPE enReqType, unsigned char *pucReqName, unsigned long ulReqNameLen)
{
	/*
	보낼 APDU
	=========
	FileReq() {
		FileReqTag						= CI_FILE_REQ
		length_field()
		RequestType						8 bits
		if (RequestType == 0) {
			for (i=0; i<(n-1); i++) {
				FileNameByte			8 bits
			}
		}
		if (RequestType == 1) {
			for (i=0; i<(n-1); i++) {
				DataByte				8 bits
			}
		}
	}
	*/

#if defined(_CI_DEBUG_AMMI_USE_EMULATOR_FOR_CAM_) // CiAmmi_FileAck()를 에뮬레이트함
	CI_AMMI_EMUL_FILE_REQ_PARAMS params;
#else
	unsigned char	*pucMsg = NULL;
	unsigned long	ulMsgPayloadLen = 1 + ulReqNameLen;
	unsigned char	ucLenFieldSize;
#endif

	if (usSsNo == 0) // 현재로선 ci_ammi 스택에선 인스턴스 관리나 RequestStart 시의 세션 정보를 저장하지 않으므로 세션이 유효하지 않은 경우만 체크하도록 한다.
	{
		HxLOG_Error("[CIAM]---> CiAmmi_FileRequest()---> Error : Wrong SsNo !!!\n");
		return -1;
	}

	if (enReqType != CI_AMMI_REQUEST_TYPE_FILE && enReqType != CI_AMMI_REQUEST_TYPE_DATA)
	{
		HxLOG_Error("[CIAM]---> CiAmmi_FileRequest()---> Error : Unknown request(0x%x) !!!\n", enReqType);
		return -1;
	}

	if (pucReqName == NULL || ulReqNameLen == 0)
	{
		HxLOG_Error("[CIAM]---> CiAmmi_FileRequest()---> Error : No name !!!\n");
		return -1;
	}

	#if 0 // #ifdef _CI_DEBUG_AMMI_DATA_
	{
		int i;
		HxLOG_Print("[CIAM]---> CiAmmi_FileRequest() : %s = [%ld][", (enReqType == CI_AMMI_REQUEST_TYPE_FILE) ? "File" : "Data", ulReqNameLen);
		for (i = 0; i < ulReqNameLen; i++)
			HxLOG_Print("%c", pucReqName[i]);
		HxLOG_Print("]\n");
	}
	#else
	HxLOG_Print("[CIAM]---> CiAmmi_FileRequest() : %s = [%ld][%.*s]\n",
		(enReqType == CI_AMMI_REQUEST_TYPE_FILE) ? "File" : "Data", ulReqNameLen, (unsigned int)ulReqNameLen, pucReqName);
	#endif

#if defined(_CI_DEBUG_AMMI_USE_EMULATOR_FOR_CAM_) // CiAmmi_FileAck()를 에뮬레이트함

	HxSTD_memset(&params, 0, sizeof(CI_AMMI_EMUL_FILE_REQ_PARAMS));
	params.usSsNo = usSsNo;
	params.enReqType = enReqType;
	HxSTD_memcpy(params.pucReqName, pucReqName, ulReqNameLen);
	params.ulReqNameLen = ulReqNameLen;

	VK_TIMER_EventAfter(CI_AMMI_EMUL_FILE_ACK_TIME, CiAmmi_Emul_FileAck, (void *)&params, sizeof(CI_AMMI_EMUL_FILE_REQ_PARAMS), (unsigned long *)&s_ulAmmiEmulTimerId);

#else

	pucMsg = (unsigned char *)OxCAS_Malloc(/*6*/ 8 + ulMsgPayloadLen); /* tag field (3 bytes) + max length field (3->5 bytes) + payload length */
	if(pucMsg != NULL)
	{
		/* set tag */
		pucMsg[0] = (unsigned char)((CI_FILE_REQ >> 16) & 0xff);
		pucMsg[1] = (unsigned char)((CI_FILE_REQ >> 8) & 0xff);
		pucMsg[2] = (unsigned char)(CI_FILE_REQ & 0xff);

		/* set length */
		CIRH_ConvertApduLengthField(ulMsgPayloadLen, &pucMsg[3], &ucLenFieldSize);

		/* set request type */
		pucMsg[3+ucLenFieldSize] = enReqType;

		/* set bytes */
		HxSTD_memcpy(&pucMsg[4+ucLenFieldSize], pucReqName, ulReqNameLen);

		if (CIRH_SendApdu(usSsNo, pucMsg, 3 + ucLenFieldSize + ulMsgPayloadLen) != ERR_OK)
		{
			HxLOG_Error("[CIAM]---> CiAmmi_FileRequest()---> Error : fail to send CI_FILE_REQ for SsNo[%d] !!!\n", usSsNo);
			OxCAS_Free(pucMsg);
			return -1;
		}

		OxCAS_Free(pucMsg);
	}
	else
	{		
		return -1;
	}
#endif

	HxLOG_Print("[CIAM]---> CiAmmi_FileRequest()---> SsNo[%d]...\n", usSsNo);

	return ERR_OK;
}

static int CiAmmi_FileAck(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen)
{
	unsigned char			ucFileOK;
	CI_AMMI_REQUEST_TYPE	enReqType;

	unsigned char	ucFileNameLen = 0;
	unsigned char	*pucFileName = NULL; // just pointer to data (don't mem alloc for this)

	unsigned long	ulFileDataLen = 0;
	unsigned char	*pucFileData = NULL; // just pointer to data (don't mem alloc for this)

	unsigned long	ulBytesLen = 0;
	unsigned char	*pucBytes = NULL; // just pointer to data (don't mem alloc for this)

	CI_AMMI_CONTENT	ustContent;

	if (pucApduData == NULL || ulApduLen < 2)
	{
		HxLOG_Error("[CIAM]---> CiAmmi_FileAck()---> Error : Wrong ulApduLen[%ld] !!!\n", ulApduLen);
		return -1;
	}

	ucFileOK = pucApduData[0] & 0x01;
	enReqType = pucApduData[1];

	HxLOG_Print("[CIAM]---> CiAmmi_FileAck() : %s %s, ulApduLen = %ld\n", enReqType == CI_AMMI_REQUEST_TYPE_FILE ? "File" : "Data", ucFileOK ? "OK" : "NG", ulApduLen);

	switch (enReqType)
	{
		case CI_AMMI_REQUEST_TYPE_FILE:

			if (ulApduLen < 7) // 최소 pucApduData[0..6] 은 유효해야...
			{
				HxLOG_Error("[CIAM]---> CiAmmi_FileAck()---> Error : Wrong ulApduLen[%ld] !!!\n", ulApduLen);
				return -1;
			}

			ucFileNameLen = pucApduData[2];
			pucFileName = ucFileNameLen ? &pucApduData[3] : NULL;

			ulFileDataLen = ((unsigned long)pucApduData[3+ucFileNameLen] << 24) & 0xff000000;
			ulFileDataLen |= (((unsigned long)pucApduData[4+ucFileNameLen] << 16) & 0x00ff0000);
			ulFileDataLen |= (((unsigned long)pucApduData[5+ucFileNameLen] << 8) & 0x0000ff00);
			ulFileDataLen |= ((unsigned long)pucApduData[6+ucFileNameLen] & 0x000000ff);
			pucFileData = ulFileDataLen ? &pucApduData[7+ucFileNameLen] : NULL;

			if (ulApduLen != 7 + ucFileNameLen + ulFileDataLen)
			{
				HxLOG_Error("[CIAM]---> CiAmmi_FileAck()---> Error : data is something wrong (%ld != 7+%d+%ld) !!!\n", ulApduLen, ucFileNameLen, ulFileDataLen);
				return -1;
			}

			#if CI_AMMI_VerifyReceivedFile // verify received file not corrupt
			if (ulFileDataLen)
			{
				FILE *tmp = tmpfile();
				if (tmp)
				{
					if (fwrite(pucFileData, 1, ulFileDataLen, tmp) != ulFileDataLen)
					{
						HxLOG_Error("[CIAM]---> CiAmmi_FileAck()---> Error : data is something wrong - %s !!!\n", strerror(errno));
						fclose(tmp);
						return -1;
					}
					fclose(tmp);
				}
				else
				{
					HxLOG_Error("[CIAM]---> CiAmmi_FileAck()---> Warning : unable to create tmpfile - %s !!!\n", strerror(errno));
				}
			}
			#endif

			ulBytesLen = ulApduLen - 2; // ulBytesLen = 1 + ucFileNameLen + 4 + ulFileDataLen;
			pucBytes = &pucApduData[2];

			#ifdef _CI_DEBUG_AMMI_DATA_
			{
				if (ucFileNameLen)
				{
					#if 0
					{
						int i;
						HxLOG_Print("[CIAM]---> CiAmmi_FileAck() : FileName = [%d][", ucFileNameLen);
						for (i = 0; i < ucFileNameLen; i++)
							HxLOG_Print("%c", pucFileName[i]);
						HxLOG_Print("]\n");
					}
					#else
					HxLOG_Print("[CIAM]---> CiAmmi_FileAck() : FileName = [%d][%.*s]\n", ucFileNameLen, ucFileNameLen, pucFileName);
					#endif
				}

				if (ulFileDataLen)
				{
					HxLOG_Print("[CIAM]---> CiAmmi_FileAck() : FileData = [%ld]\n", ulFileDataLen);
					#ifdef _CI_DEBUG_AMMI_SAVE_FILE_DATA_
					CiAmmi_SaveFileData(ucFileNameLen, pucFileName, ulFileDataLen, pucFileData);
					#else
					if (strstr(pucFileName, ".png") == NULL && strstr(pucFileName, ".PNG") == NULL)
					{
						int i;
						HxLOG_Print("[");
						for (i = 0; i < ulFileDataLen; i++)
						{
							if (isprint(pucFileData[i]) /* || isspace(pucFileData[i]) */)
								HxLOG_Print("%c", pucFileData[i]);
							else
								HxLOG_Print(" ");
						}
						HxLOG_Print("]\n");
					}
					#endif
				}
			}
			#endif

			break;

		case CI_AMMI_REQUEST_TYPE_DATA:

			if (ucFileOK)
			{
				ulBytesLen = ulApduLen - 2;
				pucBytes = ulBytesLen ? &pucApduData[2] : NULL;
			}

			#ifdef _CI_DEBUG_AMMI_DATA_
			if (ulBytesLen)
			{
				int i;
				HxLOG_Print("[CIAM]---> CiAmmi_FileAck() : DataByte = [%ld][", ulBytesLen);
				for (i = 0; i < ulBytesLen; i++)
				{
					if (isprint(pucBytes[i]) /* || isspace(pucBytes[i]) */)
						HxLOG_Print("%c", pucBytes[i]);
					else
						HxLOG_Print(" ");
				}
				HxLOG_Print("]\n");
			}
			#endif

			break;

		default:
			HxLOG_Error("[CIAM]---> CiAmmi_FileAck()---> Error : Unknown Request Type[0x%x] !!!\n", enReqType);
			return -1;
	}

	if (s_pfnAmmiCallback)
	{
		HxSTD_memset(&ustContent, 0, sizeof(CI_AMMI_CONTENT));
		ustContent.typeFileAck.usSessionId = usSsNo;
		ustContent.typeFileAck.ucFileOK = ucFileOK;
		ustContent.typeFileAck.enReqType = enReqType;
		ustContent.typeFileAck.ulBytesLen = ulBytesLen;
		ustContent.typeFileAck.pucBytes = pucBytes;
		s_pfnAmmiCallback(CI_AMMI_MSG_FILE_ACK, &ustContent); // 이 경우 리턴값은 don't care
	}
	else // 이런 경우는 없겠지만 만약 발생한다면... (CONFIG_3RD_MHEG 및 CONFIG_3RD_MHEG_CIPLUS_PROFILE1 이 설정되어 있지 않거나 콜백 등록시 문제 발생한 경우)
	{
		HxLOG_Error("[CIAM]---> CiAmmi_FileAck()---> Error : callback not registered !!!\n");
	}

	HxLOG_Print("[CIAM]---> CiAmmi_FileAck()---> SsNo[%d]...\n", usSsNo);

	return ERR_OK;
}

static int CiAmmi_AppAbort(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen)
{
	CI_AMMI_ABORT_REQUEST_CODE enAbortReqCode;
	CI_AMMI_CONTENT	ustContent;
	unsigned char ucMsg[5];
	int i = 0;

	HxLOG_Print("[CIAM]---> CiAmmi_AppAbort() : ulApduLen = %ld\n", ulApduLen);

	ucMsg[i++] = (unsigned char)((CI_APP_ABORT_ACK >> 16) & 0xff);
	ucMsg[i++] = (unsigned char)((CI_APP_ABORT_ACK >> 8) & 0xff);
	ucMsg[i++] = (unsigned char)(CI_APP_ABORT_ACK & 0xff);

	/* <TODO_CI_PLUS> : 이렇게 처리하는 것이 맞는지 확인 필요 (CI+ 스펙 1.2, TS 101 699 1.1.1, D-BOOK 5.0 모두 모호하게 기술되어 있음) */
	if (ulApduLen == 0)
	{
		ucMsg[i++] = 0x00;
		enAbortReqCode = CI_AMMI_ABORT_REQUEST_CODE_SYSTEM_CANCEL;
	}
	else
	{
		ucMsg[i++] = 0x01;
		ucMsg[i++] = pucApduData[0];
		enAbortReqCode = pucApduData[0];
	}

	if (s_pfnAmmiCallback)
	{
		HxSTD_memset(&ustContent, 0, sizeof(CI_AMMI_CONTENT));
		ustContent.typeReqAppAbort.usSessionId = usSsNo;
		ustContent.typeReqAppAbort.enAbortReqCode = enAbortReqCode;
		s_pfnAmmiCallback(CI_AMMI_MSG_REQ_APP_ABORT, &ustContent); // AbortAckCode 는 받을 필요가 없다...
	}
	else // 이런 경우는 없겠지만 만약 발생한다면... (CONFIG_3RD_MHEG 및 CONFIG_3RD_MHEG_CIPLUS_PROFILE1 이 설정되어 있지 않거나 콜백 등록시 문제 발생한 경우)
	{
		HxLOG_Error("[CIAM]---> CiAmmi_AppAbort()---> Error : callback not registered !!!\n");
	}

	if (CIRH_SendApdu(usSsNo, ucMsg, i) != ERR_OK)
	{
		HxLOG_Error("[CIAM]---> CiAmmi_AppAbort()---> Error : fail to send CI_APP_ABORT_ACK for SsNo[%d] !!!\n", usSsNo);
		return -1;
	}

	HxLOG_Print("[CIAM]---> CiAmmi_AppAbort()---> SsNo[%d]...\n", usSsNo);

	return ERR_OK;
}

int CiAmmi_AppAbortRequest(unsigned short usSsNo, CI_AMMI_ABORT_REQUEST_CODE enAbortReqCode)
{
	/*
	보낼 APDU
	=========
	AppAbortReq () {
		AppAbortReqTag				= CI_APP_ABORT_REQ
		length_field()
		for (i = 0; i < N; i + +) {
			AbortReqCode			8 bits
		}
	}
	*/

#if defined(_CI_DEBUG_AMMI_USE_EMULATOR_FOR_CAM_) // CiAmmi_AppAbortAck()를 에뮬레이트함
	CI_AMMI_CONTENT	ustContent;
#else
	unsigned char			ucMsg[5];
#endif

	if (usSsNo == 0) // 현재로선 ci_ammi 스택에선 인스턴스 관리나 RequestStart 시의 세션 정보를 저장하지 않으므로 세션이 유효하지 않은 경우만 체크하도록 한다.
	{
		HxLOG_Error("[CIAM]---> CiAmmi_AppAbortRequest()---> Error : Wrong SsNo !!!\n");
		return -1;
	}

	if (enAbortReqCode != CI_AMMI_ABORT_REQUEST_CODE_USER_CANCEL && enAbortReqCode != CI_AMMI_ABORT_REQUEST_CODE_SYSTEM_CANCEL)
	{
		HxLOG_Error("[CIAM]---> CiAmmi_AppAbortRequest()---> Error : Unknown abort request(0x%x) !!!\n", enAbortReqCode);
		return -1;
	}

	HxLOG_Print("[CIAM]---> CiAmmi_AppAbortRequest() : enAbortReqCode = 0x%x\n", enAbortReqCode);

#if defined(_CI_DEBUG_AMMI_USE_EMULATOR_FOR_CAM_) // CiAmmi_AppAbortAck()를 에뮬레이트함
	if (s_pfnAmmiCallback)
	{
		HxSTD_memset(&ustContent, 0, sizeof(CI_AMMI_CONTENT));
		ustContent.typeAppAbortAck.usSessionId = usSsNo;
		s_pfnAmmiCallback(CI_AMMI_MSG_APP_ABORT_ACK, &ustContent); // 이 경우 리턴값은 don't care
	}
	else // 이런 경우는 없겠지만 만약 발생한다면... (CONFIG_3RD_MHEG 및 CONFIG_3RD_MHEG_CIPLUS_PROFILE1 이 설정되어 있지 않거나 콜백 등록시 문제 발생한 경우)
	{
		HxLOG_Error("[CIAM]---> CiAmmi_Emul_AppAbortAck()---> Error : callback not registered !!!\n");
	}

	HxLOG_Print(("[CIAM]---> CiAmmi_Emul_AppAbortAck()---> SsNo[%d]...\n", usSsNo));


#else
	ucMsg[0] = (unsigned char)((CI_APP_ABORT_REQ >> 16) & 0xff);
	ucMsg[1] = (unsigned char)((CI_APP_ABORT_REQ >> 8) & 0xff);
	ucMsg[2] = (unsigned char)(CI_APP_ABORT_REQ & 0xff);
	ucMsg[3] = 0x01;
	ucMsg[4] = enAbortReqCode;

	if (CIRH_SendApdu(usSsNo, ucMsg, 5) != ERR_OK)
	{
		HxLOG_Error("[CIAM]---> CiAmmi_AppAbortRequest()---> Error : fail to send CI_APP_ABORT_REQ for SsNo[%d] !!!\n", usSsNo);
		return -1;
	}

	HxLOG_Print("[CIAM]---> CiAmmi_AppAbortRequest()---> SsNo[%d]...\n", usSsNo);

#endif

	return ERR_OK;
}

static int CiAmmi_AppAbortAck(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen)
{
//	unsigned char	ucAbortAckCode;
	CI_AMMI_CONTENT	ustContent;

	HxLOG_Print("[CIAM]---> CiAmmi_AppAbortAck() : ulApduLen = %ld\n", ulApduLen);

	/* ucAbortAckCode 처리 필요 (CI+ 스펙 1.2, TS 101 699 1.1.1, D-BOOK 5.0 모두 분명히 명시되어 있지 않아 일단 막아둠)
	   -> SmarDTV CI+ 캠으로 확인해 보니 굳이 처리할 필요 없음. */

	if (s_pfnAmmiCallback)
	{
		HxSTD_memset(&ustContent, 0, sizeof(CI_AMMI_CONTENT));
		ustContent.typeAppAbortAck.usSessionId = usSsNo;
//		ustContent.typeAppAbortAck.ucAbortAckCode = ucAbortAckCode;
		s_pfnAmmiCallback(CI_AMMI_MSG_APP_ABORT_ACK, &ustContent); // 이 경우 리턴값은 don't care
	}
	else // 이런 경우는 없겠지만 만약 발생한다면... (CONFIG_3RD_MHEG 및 CONFIG_3RD_MHEG_CIPLUS_PROFILE1 이 설정되어 있지 않거나 콜백 등록시 문제 발생한 경우)
	{
		HxLOG_Error("[CIAM]---> CiAmmi_AppAbortAck()---> Error : callback not registered !!!\n");
	}

	HxLOG_Print("[CIAM]---> CiAmmi_AppAbortAck()---> SsNo[%d]...\n", usSsNo);

	return ERR_OK;
}

////////////////////////////////////////////////////////////////

#ifdef _CI_DEBUG_AMMI_SAVE_FILE_DATA_

static int CiAmmi_SaveFileData(unsigned char ucFileNameLen, unsigned char *pucFileName, unsigned long ulFileDataLen, unsigned char *pucFileData)
{
	char *p[10];
	char str[256], fileToBeSaved[256];
	FILE *fpBinaryFile = NULL;
	int nDataSize = 0, i;

	if (ucFileNameLen <= 5 || pucFileName == NULL)	// "CI://" 보다 길어야 하니까...
	{
		HxLOG_Print("Len %d is wrong to save !!!\n", ucFileNameLen);
		return -1;
	}

	HxSTD_memset(str, 0, 256);
	strncpy(str, pucFileName, ucFileNameLen);	// 예: "CI://startup", "CI://images/red.png", "CI://Apps/AppStack/startup"

	p[0] = strtok(str, "/");	// -> "CI:"
	if (!p[0])
	{
		HxLOG_Print("1st token is wrong to save !!!\n");
		return -1;
	}

	HxSTD_memset(fileToBeSaved, 0, 256);
	strcpy(fileToBeSaved, CI_AMMI_FILE_DATA_PATH);

	// opt 밑에 CI 디렉토리를 생성한다
	if (svc_cas_DevFsIsDirExist(fileToBeSaved) == FALSE)
	{
		svc_cas_DevFsMkdir(fileToBeSaved);
	}

	p[1] = strtok(NULL, "/");	// -> "startup", "images", "Apps"
	if (!p[1])
	{
		HxLOG_Print("2nd token is wrong to save !!!\n");
		return -1;
	}

	strcat(fileToBeSaved, p[1]);

	for (i = 2; i < 10; i++)
	{
		p[i] = strtok(NULL, "/");	// -> NULL, "red.png", "AppStack"
		if (p[i])
		{
			// 서브 디렉토리를 생성한다
			if (svc_cas_DevFsIsDirExist(fileToBeSaved) == FALSE)
			{
				svc_cas_DevFsMkdir(fileToBeSaved);
			}

			strcat(fileToBeSaved, "/");
			strcat(fileToBeSaved, p[i]);
		}
		else
			break;
	}

	// 파일이 저장되어 있는지 확인... 같은 이름의 파일이 이미 저장되어 있으면 우쩔까나?
	if (svc_cas_DevFsIsFileExist(fileToBeSaved) == TRUE)
	{
		HxLOG_Print("%s already saved\n", &fileToBeSaved[CI_AMMI_FILE_DATA_PATH_LEN]);
		return ERR_OK;
	}

	// 파일 저장
	HxLOG_Print("%s saving...\n", &fileToBeSaved[CI_AMMI_FILE_DATA_PATH_LEN]);
	fpBinaryFile = fopen(fileToBeSaved, "wb");
	if (fpBinaryFile == NULL)
	{
		HxLOG_Print("Can't open file %s - %s !!!\n", &fileToBeSaved[CI_AMMI_FILE_DATA_PATH_LEN], strerror(errno));
		return -1;
	}

	nDataSize = fwrite(pucFileData, 1, ulFileDataLen, fpBinaryFile);
	fclose(fpBinaryFile);
	if (nDataSize != ulFileDataLen)
	{
		HxLOG_Print("Can't write file %s - %s !!!\n", &fileToBeSaved[CI_AMMI_FILE_DATA_PATH_LEN], strerror(errno));
		return -1;
	}

	#ifdef _CI_DEBUG_AMMI_DECODE_FILE_DATA_
	if (strstr(fileToBeSaved, ".png") == NULL && strstr(fileToBeSaved, ".PNG") == NULL
		&& strstr(fileToBeSaved, ".txt") == NULL && strstr(fileToBeSaved, ".TXT") == NULL)
	{
		if (CiAmmi_DecodingFileData(fileToBeSaved))
		{
			HxLOG_Print("Can't decode file %s !!!\n", &fileToBeSaved[CI_AMMI_FILE_DATA_PATH_LEN]);
			return -1;
		}
	}
	#endif

	return ERR_OK;
}

#endif

#ifdef _CI_DEBUG_AMMI_DECODE_FILE_DATA_

extern void output_init(void);
extern int asn1decode_InterchangedObject(FILE *der, FILE *out, int length);

static int CiAmmi_DecodingFileData(char *fileToBeDecoding) // mhegd.c 's main()
{
	char fileToBeOut[260];
	FILE *in_file = NULL;
	FILE *out_file = NULL;
	int filesize;
	int used;
	int nRtnCode = ERR_OK;

	strcpy(fileToBeOut, fileToBeDecoding);
	strcat(fileToBeOut, ".txt");

	// 파일이 저장되어 있는지 확인... 같은 이름의 파일이 이미 저장되어 있으면 우쩔까나?
	if (svc_cas_DevFsIsFileExist(fileToBeOut) == TRUE)
	{
		HxLOG_Print("%s already decoded\n", &fileToBeOut[CI_AMMI_FILE_DATA_PATH_LEN]);
		return ERR_OK;
	}

	if((in_file = fopen(fileToBeDecoding, "r")) == NULL)
	{
		HxLOG_Print("Can't open file %s for decoding - %s !!!\n", &fileToBeDecoding[CI_AMMI_FILE_DATA_PATH_LEN], strerror(errno));
		nRtnCode = -1;
		goto EXIT;
	}

	if((out_file = fopen(fileToBeOut, "w")) == NULL)
	{
		HxLOG_Print("Can't open file %s for writing - %s !!!\n", &fileToBeOut[CI_AMMI_FILE_DATA_PATH_LEN], strerror(errno));
		nRtnCode = -1;
		goto EXIT;
	}

	/* see how long the file is */
	fseek(in_file, 0, SEEK_END);
	filesize = (int) ftell(in_file);
	rewind(in_file);

	/* write text form of DER encoded in_file to out_file */
	output_init();
//	HxLOG_Print("%s decoding...\n", &fileToBeDecoding[CI_AMMI_FILE_DATA_PATH_LEN]);
	used = asn1decode_InterchangedObject(in_file, out_file, filesize);
	fprintf(out_file, "\n");
	if(used < 0)
	{
		HxLOG_Print("Parsing error !!!\n");
		nRtnCode = -1;
		goto EXIT;
	}
	else if(used != filesize)
	{
		HxLOG_Print("Unexpected data after InterchangedObject !!!\n");
		nRtnCode = -1;
		goto EXIT;
	}

EXIT :

	if (in_file)
		fclose(in_file);

	if (out_file)
		fclose(out_file);

	return nRtnCode;
}

#endif

#if defined(_CI_DEBUG_AMMI_USE_EMULATOR_FOR_CAM_)

static void CiAmmi_Emul_Task(void)
{
	CI_AMMI_EMUL_TASK_MSG msg;

	CI_AMMI_CONTENT					ustContent;
	CI_AMMI_REQUEST_START_ACK_CODE	enAckCode;

	unsigned char	ucAppDomainIdLen;
	unsigned char	ucInitialObjectLen;
	unsigned char	pucAppDomainId[] = CI_AMMI_APP_DOMAIN_ID;
	unsigned char	pucInitialObject[256]; // PATH_MAX

	CI_AMMI_EMUL_FILE_REQ_PARAMS *pFileReqParams;
	unsigned char	ucFileOK;
	unsigned long	ulBytesLen;
	unsigned char	*pucBytes;

	while (1)
	{
		if (VK_MSG_Receive(s_ulAmmiEmulMsgQId, &msg, sizeof(CI_AMMI_EMUL_TASK_MSG)) != VK_OK)
		{
			HxLOG_Error("[CIAM]---> CiAmmi_Emul_Task()---> Error : fail to get msg !!!\n");
			continue;
		}

		switch (msg.command)
		{
			case CI_AMMI_MSG_REQ_START:
				HxLOG_Error("[CIAM]---> CI_AMMI_MSG_REQ_START received !!!\n");

				HxSTD_memset(pucInitialObject, 0, 256);
				if (svc_cas_DevFsIsFileExist(CI_AMMI_FILE_DATA_PATH "a") == TRUE)
				{
					strcpy(pucInitialObject, "CI://a");
				}
				else if (svc_cas_DevFsIsFileExist(CI_AMMI_FILE_DATA_PATH "startup") == TRUE)
				{
					strcpy(pucInitialObject, "CI://startup");
				}
				else if (svc_cas_DevFsIsFileExist(CI_AMMI_FILE_DATA_PATH "startup_vod") == TRUE)
				{
					strcpy(pucInitialObject, "CI://startup_vod");
				}
				else
				{
					HxLOG_Error("[CIAM]---> CiAmmi_Emul_Task(RequestStart)---> Error : InitialObject not saved !!!\n");
				}
				ucAppDomainIdLen = strlen(pucAppDomainId); // sizeof(pucAppDomainId) 를 사용하면 +1 하여 계산되므로 주의할 것.
				ucInitialObjectLen = strlen(pucInitialObject); // sizeof(pucInitialObject) 를 사용하면 +1 하여 계산되므로 주의할 것.
				HxLOG_Print("[CIAM]---> CiAmmi_Emul_Task(RequestStart) : AppDomainId = [%d][%.*s]\n", ucAppDomainIdLen, ucAppDomainIdLen, pucAppDomainId);
				HxLOG_Print("[CIAM]---> CiAmmi_Emul_Task(RequestStart) : InitialObject = [%d][%.*s]\n", ucInitialObjectLen, ucInitialObjectLen, pucInitialObject);
				if (s_pfnAmmiCallback)
				{
					HxSTD_memset(&ustContent, 0, sizeof(CI_AMMI_CONTENT));
					ustContent.typeReqStart.usSessionId = 1;
					ustContent.typeReqStart.ucAppDomainIdLen = ucAppDomainIdLen;
					ustContent.typeReqStart.pucAppDomainId = pucAppDomainId;
					ustContent.typeReqStart.ucInitialObjectLen = ucInitialObjectLen;
					ustContent.typeReqStart.pucInitialObject = pucInitialObject;
					enAckCode = (CI_AMMI_REQUEST_START_ACK_CODE)s_pfnAmmiCallback(CI_AMMI_MSG_REQ_START, &ustContent);
					HxLOG_Print("[CIAM]---> CiAmmi_Emul_Task(RequestStart) : AckCode = %d\n", enAckCode);
				}
				else // 이런 경우는 없겠지만 만약 발생한다면... (CONFIG_3RD_MHEG 및 CONFIG_3RD_MHEG_CIPLUS_PROFILE1 이 설정되어 있지 않거나 콜백 등록시 문제 발생한 경우)
				{
					HxLOG_Error("[CIAM]---> CiAmmi_Emul_Task(RequestStart)---> Error : callback not registered !!!\n");
				}
				break;

			case CI_AMMI_MSG_FILE_ACK:
				pFileReqParams = (CI_AMMI_EMUL_FILE_REQ_PARAMS *)msg.data;
				ucFileOK = 1;
				if (pFileReqParams->enReqType == CI_AMMI_REQUEST_TYPE_FILE)
				{
					if (CiAmmi_LoadFileData((unsigned char)pFileReqParams->ulReqNameLen, pFileReqParams->pucReqName, &ulBytesLen, &pucBytes))
					{
						ucFileOK = 0;
						ulBytesLen = 1 + pFileReqParams->ulReqNameLen + 4;
						pucBytes = (unsigned char *)OxCAS_Malloc(ulBytesLen);
						HxSTD_memset(pucBytes, 0, ulBytesLen);
						pucBytes[0] = pFileReqParams->ulReqNameLen;
						HxSTD_memcpy(&pucBytes[1], pFileReqParams->pucReqName, pFileReqParams->ulReqNameLen);
					}
				}
				else
				{
					ulBytesLen = 1;
					pucBytes = (unsigned char *)OxCAS_Malloc(ulBytesLen);
					pucBytes[0] = 0;
				}
				HxLOG_Print("[CIAM]---> CiAmmi_Emul_Task(FileAck) : %s %s\n", pFileReqParams->enReqType == CI_AMMI_REQUEST_TYPE_FILE ? "File" : "Data", ucFileOK == 1 ? "OK" : "NG");
				if (s_pfnAmmiCallback)
				{
					HxSTD_memset(&ustContent, 0, sizeof(CI_AMMI_CONTENT));
					ustContent.typeFileAck.usSessionId = pFileReqParams->usSsNo;
					ustContent.typeFileAck.ucFileOK = ucFileOK;
					ustContent.typeFileAck.enReqType = pFileReqParams->enReqType;
					ustContent.typeFileAck.ulBytesLen = ulBytesLen;
					ustContent.typeFileAck.pucBytes = pucBytes;
					s_pfnAmmiCallback(CI_AMMI_MSG_FILE_ACK, &ustContent); // 이 경우 리턴값은 don't care
				}
				else // 이런 경우는 없겠지만 만약 발생한다면... (CONFIG_3RD_MHEG 및 CONFIG_3RD_MHEG_CIPLUS_PROFILE1 이 설정되어 있지 않거나 콜백 등록시 문제 발생한 경우)
				{
					HxLOG_Error("[CIAM]---> CiAmmi_Emul_Task(FileAck)---> Error : callback not registered !!!\n");
				}
				if (pucBytes != NULL)
					OxCAS_Free(pucBytes);
				if (pFileReqParams != NULL)
					OxCAS_Free(pFileReqParams);
				break;

			default:
				HxLOG_Error("[CIAM]---> CiAmmi_Emul_Task()---> Error : Unknown command %d !!!\n", msg.command);
				break;
		}
	}
}

void CiAmmi_Emul_RequestStart(unsigned long ulTimerId, void *pvParam) // CiAmmi_RequestStart()를 에뮬레이트함
{
	CI_AMMI_EMUL_TASK_MSG msg;

	msg.command = CI_AMMI_MSG_REQ_START;
	msg.data = NULL;

	HxLOG_Error("[CIAM]---> CI_AMMI_MSG_REQ_START send !!!\n");
	if (VK_MSG_SendTimeout(s_ulAmmiEmulMsgQId, &msg, sizeof(CI_AMMI_EMUL_TASK_MSG), 1000))
	{
		HxLOG_Error("[CIAM]---> CiAmmi_Emul_RequestStart()---> Error : fail to send msg !!!\n");
	}
}

static void CiAmmi_Emul_FileAck(unsigned long ulTimerId, void *pvParam) // CiAmmi_FileAck()를 에뮬레이트함
{
	CI_AMMI_EMUL_TASK_MSG msg;

	msg.command = CI_AMMI_MSG_FILE_ACK;
	msg.data = OxCAS_Malloc(sizeof(CI_AMMI_EMUL_FILE_REQ_PARAMS));
	HxSTD_memcpy(msg.data, pvParam, sizeof(CI_AMMI_EMUL_FILE_REQ_PARAMS));

	if (VK_MSG_SendTimeout(s_ulAmmiEmulMsgQId, &msg, sizeof(CI_AMMI_EMUL_TASK_MSG), 1000))
	{
		HxLOG_Error("[CIAM]---> CiAmmi_Emul_FileAck()---> Error : fail to send msg !!!\n");
		if(msg.data != NULL)
			OxCAS_Free(msg.data);
	}
}

static int CiAmmi_LoadFileData(unsigned char ucFileNameLen, unsigned char *pucFileName, unsigned long *pulBytesLen, unsigned char **ppucBytes)
{
	char *p[10];
	char str[256], fileToBeLoading[256];
	FILE *fpBinaryFile = NULL;
	int nDataSize = 0, i;
	unsigned long ulFileDataLen = 0;
	unsigned char *pucBytes = NULL;

	if (ucFileNameLen <= 5 || pucFileName == NULL)	// "CI://" 보다 길어야 하니까...
	{
		HxLOG_Print("Len %d is wrong to load !!!\n", ucFileNameLen);
		return -1;
	}

	HxSTD_memset(str, 0, 256);
	strncpy(str, pucFileName, ucFileNameLen);	// 예: "CI://startup", "CI://images/red.png", "CI://Apps/AppStack/startup"

	p[0] = strtok(str, "/");	// -> "CI:"
	if (!p[0])
	{
		HxLOG_Print("1st token is wrong to load !!!\n");
		return -1;
	}

	HxSTD_memset(fileToBeLoading, 0, 256);
	strcpy(fileToBeLoading, CI_AMMI_FILE_DATA_PATH);

	p[1] = strtok(NULL, "/");	// -> "startup", "images", "Apps"
	if (!p[1])
	{
		HxLOG_Print("2nd token is wrong to load !!!\n");
		return -1;
	}

	strcat(fileToBeLoading, p[1]);

	for (i = 2; i < 10; i++)
	{
		p[i] = strtok(NULL, "/");	// -> NULL, "red.png", "AppStack"
		if (p[i])
		{
			strcat(fileToBeLoading, "/");
			strcat(fileToBeLoading, p[i]);
		}
		else
			break;
	}

	// 파일이 저장되어 있는지 확인...
	if (svc_cas_DevFsIsFileExist(fileToBeLoading) == FALSE)
	{
		HxLOG_Print("%s not found !!!\n", &fileToBeLoading[CI_AMMI_FILE_DATA_PATH_LEN]);
		return -1;
	}

	// 파일 로드
	HxLOG_Print("%s loading...\n", &fileToBeLoading[CI_AMMI_FILE_DATA_PATH_LEN]);
	fpBinaryFile = fopen(fileToBeLoading, "rb");
	if (fpBinaryFile == NULL)
	{
		HxLOG_Print("Can't open file %s - %s !!!\n", &fileToBeLoading[CI_AMMI_FILE_DATA_PATH_LEN], strerror(errno));
		return -1;
	}

	fseek(fpBinaryFile, 0, SEEK_END);
	ulFileDataLen = ftell(fpBinaryFile);
	rewind(fpBinaryFile);

	pucBytes = (unsigned char *)OxCAS_Malloc(1 + ucFileNameLen + 4 + ulFileDataLen);
	if(pucBytes != NULL)
	{
		pucBytes[0] = ucFileNameLen;
		HxSTD_memcpy(&pucBytes[1], pucFileName, ucFileNameLen);
		pucBytes[1+ucFileNameLen] = (unsigned char)((ulFileDataLen >> 24) & 0xff);
		pucBytes[2+ucFileNameLen] = (unsigned char)((ulFileDataLen >> 16) & 0xff);
		pucBytes[3+ucFileNameLen] = (unsigned char)((ulFileDataLen >> 8) & 0xff);
		pucBytes[4+ucFileNameLen] = (unsigned char)(ulFileDataLen & 0xff);

		nDataSize = fread(&pucBytes[5+ucFileNameLen], 1, ulFileDataLen, fpBinaryFile);
		fclose(fpBinaryFile);
		if (nDataSize != ulFileDataLen)
		{
			HxLOG_Print("Can't read file %s - %s !!!\n", &fileToBeLoading[CI_AMMI_FILE_DATA_PATH_LEN], strerror(errno));
			OxCAS_Free(pucBytes);
			return -1;
		}

		*pulBytesLen = 1 + ucFileNameLen + 4 + ulFileDataLen;
		*ppucBytes = pucBytes;
	}
	else
	{
		HxLOG_Print("[CiAmmi_LoadFileData] pucBytes Malloc failed !!!\n");
		return -1;
	}
	
	#ifdef _CI_DEBUG_AMMI_DECODE_FILE_DATA_
	if (strstr(fileToBeLoading, ".png") == NULL && strstr(fileToBeLoading, ".PNG") == NULL
		&& strstr(fileToBeLoading, ".txt") == NULL && strstr(fileToBeLoading, ".TXT") == NULL)
	{
		if (CiAmmi_DecodingFileData(fileToBeLoading))
		{
			HxLOG_Print("Can't decode file %s !!!\n", &fileToBeLoading[CI_AMMI_FILE_DATA_PATH_LEN]);
//			return -1;
		}
	}
	#endif

	return ERR_OK;
}

#endif

#ifdef _CI_DEBUG_AMMI_USE_EMULATOR_FOR_MHEG_ENGINE_

static HUINT32 CiAmmi_Emul_MhegEngine(CI_AMMI_MSG_TYPE enMsgType, CI_AMMI_CONTENT *pustContent)
{
	HUINT32 nRtnCode = 0;

	HxLOG_Print("[CIAM]---> CiAmmi_Emul_MhegEngine() : enMsgType = %d\n", enMsgType);

	switch(enMsgType)
	{
		case CI_AMMI_MSG_REQ_START:
			// TODO ...
			break;

		case CI_AMMI_MSG_FILE_ACK:
			// TODO ...
			break;

		case CI_AMMI_MSG_REQ_APP_ABORT:
			// TODO ...
			break;

		case CI_AMMI_MSG_APP_ABORT_ACK:
			// TODO ...
			break;

		default:
			HxLOG_Error("[CIAM]---> CiAmmi_Emul_MhegEngine()---> Error : unknown enMsgType (%d) !!!\n", enMsgType);
			break;
	}

	return nRtnCode;
}

static int CiAmmi_ParsingFileData(char *fileToBeParsing)
{
	// TODO ...

	return ERR_OK;
}

#endif

