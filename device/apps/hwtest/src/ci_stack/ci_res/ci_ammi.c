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
#include "ci_ammi.h"


/***************************************************************
* definition
***************************************************************/


/***************************************************************
* static variables and structures
***************************************************************/
static CI_AMMI_CALLBACK_T s_pfnAmmiCallback = NULL; // local_itk_APPMMI_CIMessageCallback()


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
int CiAmmi_Init(void)
{
	return ERR_OK;
}

int CiAmmi_SsOpenedProc(unsigned short usSsNo)
{
	return ERR_OK;
}

int CiAmmi_SsClosedProc(unsigned short usSsNo)
{
	CI_AMMI_CONTENT	ustContent;

	PrintNotify(("[CIAM]---> CiAmmi_SsClosedProc()\n"));

	// 캠이 제거되거나 하여 세션이 닫히면 CI+ MHEG porting layer 로 AppAbort 를 날리도록 한다.
	// MHEG application 이 런닝되어 있지 않다면 porting layer 에서 거르기 때문에 걱정하지 않아도 된다.

	if (s_pfnAmmiCallback)
	{
		VK_memset(&ustContent, 0, sizeof(CI_AMMI_CONTENT));
		ustContent.typeReqAppAbort.usSessionId = usSsNo;
		ustContent.typeReqAppAbort.ucAbortReqCode = CI_AMMI_ABORT_REQUEST_CODE_SYSTEM_CANCEL;
		s_pfnAmmiCallback(CI_AMMI_MSG_REQ_APP_ABORT, ustContent); // AbortAckCode 는 받을 필요가 없다...
	}
	else // 이런 경우는 없겠지만 만약 발생한다면...
	{
		PrintErr(("[CIAM]---> CiAmmi_SsClosedProc()---> Error : callback not registered !!!\n"));
	}

	PrintDbg(("[CIAM]---> CiAmmi_SsClosedProc()---> SsNo[%d]...\n", usSsNo));

	return ERR_OK;
}

int CiAmmi_ApduProc(unsigned short usSsNo, unsigned long ulApduTag,
						unsigned char *pucApduData, unsigned long ulApduLen)
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

//		PrintNotify(("CI_REQ_START received\n"));
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

//		PrintNotify(("CI_FlLE_ACK received\n"));
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

//		PrintNotify(("CI_APP_ABORT_REQ received\n"));
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

//		PrintNotify(("CI_APP_ABORT_ACK received\n"));
		nRtnCode = CiAmmi_AppAbortAck(usSsNo, pucApduData, ulApduLen);
		break;

	default:
		nRtnCode = -1;
		PrintErr(("[CIAM]---> CiAmmi_ApduProc()---> Error : Unknown Apdu Tag[0x%lx] !!!\n", ulApduTag));
	}

	PrintDbg(("[CIAM]---> CiAmmi_ApduProc()---> SsNo[%d], tag[0x%lx]...\n", usSsNo, ulApduTag));

	return nRtnCode;
}

int CiAmmi_RegisterCallback(CI_AMMI_CALLBACK_T pfnCallback)
{
	if (pfnCallback == NULL)
	{
		PrintErr(("[CIAM]---> CiAmmi_RegisterCallback()---> Error : NULL callback registered !!!\n"));
		return -1;
	}

	s_pfnAmmiCallback = pfnCallback;

	return ERR_OK;
}

static int CiAmmi_RequestStart(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen)
{
	unsigned char	ucAppDomainIdLength = 0;
	unsigned char	*pucAppDomainId = NULL;

	unsigned char	ucInitialObjectLength = 0;
	unsigned char	*pucInitialObject = NULL;

	CI_AMMI_CONTENT					ustContent;
	CI_AMMI_REQUEST_START_ACK_CODE	enAckCode;

	unsigned char	ucMsg[5];

	if (pucApduData == NULL || ulApduLen < 2)
	{
		PrintErr(("[CIAM]---> CiAmmi_RequestStart()---> Error : Wrong ulApduLen[%ld] !!!\n", ulApduLen));
		return -1;
	}

	ucAppDomainIdLength = pucApduData[0];
	ucInitialObjectLength = pucApduData[1];

	PrintNotify(("[CIAM]---> CiAmmi_RequestStart() : ucAppDomainIdLength = %d, ucInitialObjectLength = %d, ulApduLen = %ld\n",
		ucAppDomainIdLength, ucInitialObjectLength, ulApduLen));

	if (ucAppDomainIdLength)
	{
		pucAppDomainId = (unsigned char *)CA_MEM_Alloc(sizeof(unsigned char) * (ucAppDomainIdLength + 1)); // '+ 1'은 끝에 NULL 코드 붙이기 위해. 사실상 불필요하지만서두...
		VK_memset(pucAppDomainId, 0, ucAppDomainIdLength + 1);
		VK_memcpy(pucAppDomainId, &pucApduData[2], ucAppDomainIdLength);
		PrintNotify(("[CIAM]---> CiAmmi_RequestStart() : AppDomainId = [%s]\n", pucAppDomainId));
	}

	if (ucInitialObjectLength)
	{
		pucInitialObject = (unsigned char *)CA_MEM_Alloc(sizeof(unsigned char) * (ucInitialObjectLength + 1)); // '+ 1'은 끝에 NULL 코드 붙이기 위해. 사실상 불필요하지만서두...
		VK_memset(pucInitialObject, 0, ucInitialObjectLength + 1);
		VK_memcpy(pucInitialObject, &pucApduData[2+ucAppDomainIdLength], ucInitialObjectLength);
		PrintNotify(("[CIAM]---> CiAmmi_RequestStart() : InitialObject = [%s]\n", pucInitialObject));
	}

	if (s_pfnAmmiCallback)
	{
		VK_memset(&ustContent, 0, sizeof(CI_AMMI_CONTENT));
		ustContent.typeReqStart.usSessionId = usSsNo;
		ustContent.typeReqStart.ucAppDomainIdLength = ucAppDomainIdLength;
		ustContent.typeReqStart.pucAppDomainId = pucAppDomainId;
		ustContent.typeReqStart.ucInitialObjectLength = ucInitialObjectLength;
		ustContent.typeReqStart.pucInitialObject = pucInitialObject;
		enAckCode = (CI_AMMI_REQUEST_START_ACK_CODE)s_pfnAmmiCallback(CI_AMMI_MSG_REQ_START, ustContent);
		PrintNotify(("[CIAM]---> CiAmmi_RequestStart() : AckCode = %d\n", enAckCode));
	}
	else // 이런 경우는 없겠지만 만약 발생한다면...
	{
		if ((ucAppDomainIdLength == strlen(CI_AMMI_APP_DOMAIN_ID)) && (VK_memcmp(pucAppDomainId, CI_AMMI_APP_DOMAIN_ID, ucAppDomainIdLength) == 0))
			enAckCode = CI_AMMI_REQUEST_START_ACK_CODE_API_BUSY;
		else
			enAckCode = CI_AMMI_REQUEST_START_ACK_CODE_WRONG_API;
		PrintErr(("[CIAM]---> CiAmmi_RequestStart()---> Error : callback not registered !!!\n"));
	}

	if (pucAppDomainId)
		CA_MEM_Free(pucAppDomainId);
	if (pucInitialObject)
		CA_MEM_Free(pucInitialObject);

	ucMsg[0] = (unsigned char)((CI_REQ_START_ACK >> 16) & 0xff);
	ucMsg[1] = (unsigned char)((CI_REQ_START_ACK >> 8) & 0xff);
	ucMsg[2] = (unsigned char)(CI_REQ_START_ACK & 0xff);
	ucMsg[3] = 0x01;
	ucMsg[4] = enAckCode;

	if (CIRH_SendApdu(usSsNo, ucMsg, 5) != ERR_OK)
	{
		PrintErr(("[CIAM]---> CiAmmi_RequestStart()---> Error : send APDU msg. SsNo[%d] !!!\n", usSsNo));
		return -1;
	}

	PrintDbg(("[CIAM]---> CiAmmi_RequestStart()---> SsNo[%d]...\n", usSsNo));

	return ERR_OK;
}

int CiAmmi_FileRequest(unsigned short usSsNo, unsigned char ucReqType /* CI_AMMI_REQUEST_TYPE */, unsigned char *pucBytes, unsigned long ulBytesLen)
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

	unsigned char	*pucMsg = NULL;
	unsigned short	ulMsgPayloadLen = 1 + ulBytesLen;
	unsigned char	ucLenFieldSize;

	if (ucReqType != CI_AMMI_REQUEST_TYPE_FILE && ucReqType != CI_AMMI_REQUEST_TYPE_DATA)
	{
		PrintErr(("[CIAM]---> CiAmmi_FileRequest()---> Error : Unknown request(0x%x) !!!\n", ucReqType));
		return -1;
	}

	if (pucBytes == NULL || ulBytesLen == 0)
	{
		PrintErr(("[CIAM]---> CiAmmi_FileRequest()---> Error : No data !!!\n"));
		return -1;
	}

	PrintNotify(("[CIAM]---> CiAmmi_FileRequest() : ucReqType = 0x%x, ulBytesLen = %ld\n", ucReqType, ulBytesLen));

	pucMsg = (unsigned char *)CA_MEM_Alloc(sizeof(unsigned char) * (/*6*/ 8 + ulMsgPayloadLen)); /* tag field (3 bytes) + max length field (3->5 bytes) + payload length */

	/* set tag */
	pucMsg[0] = (unsigned char)((CI_FILE_REQ >> 16) & 0xff);
	pucMsg[1] = (unsigned char)((CI_FILE_REQ >> 8) & 0xff);
	pucMsg[2] = (unsigned char)(CI_FILE_REQ & 0xff);

	/* set length */
	CIRH_ConvertApduLengthField(ulMsgPayloadLen, &pucMsg[3], &ucLenFieldSize);

	/* set request type */
	pucMsg[3+ucLenFieldSize] = ucReqType;

	/* set bytes */
	VK_memcpy(&pucMsg[4+ucLenFieldSize], pucBytes, ulBytesLen);

	if (CIRH_SendApdu(usSsNo, pucMsg, 3 + ucLenFieldSize + ulMsgPayloadLen) != ERR_OK)
	{
		PrintErr(("[CIAM]---> CiAmmi_FileRequest()---> Error : send APDU msg. SsNo[%d] !!!\n", usSsNo));
		CA_MEM_Free(pucMsg);
		return -1;
	}

	CA_MEM_Free(pucMsg);

	PrintDbg(("[CIAM]---> CiAmmi_FileRequest()---> SsNo[%d]...\n", usSsNo));

	return ERR_OK;
}

static int CiAmmi_FileAck(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen)
{
	unsigned char	ucFileOK;
	unsigned char	ucReqType;			/* CI_AMMI_REQUEST_TYPE */

	unsigned char	ucFileNameLength = 0;
	unsigned char	*pucFileNameByte = NULL;

	unsigned long	ulFileDataLength = 0;
	unsigned char	*pucFileDataByte = NULL;

	unsigned long	ulDataByteLength = 0;
	unsigned char	*pucDataByte = NULL;

	CI_AMMI_CONTENT	ustContent;

	if (pucApduData == NULL || ulApduLen < 2)
	{
		PrintErr(("[CIAM]---> CiAmmi_FileAck()---> Error : Wrong ulApduLen[%ld] !!!\n", ulApduLen));
		return -1;
	}

	ucFileOK = pucApduData[0] & 0x01;
	ucReqType = pucApduData[1];

	PrintNotify(("[CIAM]---> CiAmmi_FileAck() : ucFileOK = %d, ucReqType = 0x%x, ulApduLen = %ld\n", ucFileOK, ucReqType, ulApduLen));

	if (ucFileOK)
	{
		switch (ucReqType)
		{
			case CI_AMMI_REQUEST_TYPE_FILE:
				ucFileNameLength = pucApduData[2];
				ulFileDataLength = ((unsigned long)pucApduData[3+ucFileNameLength] << 24) & 0xff000000;
				ulFileDataLength |= (((unsigned long)pucApduData[4+ucFileNameLength] << 16) & 0x00ff0000);
				ulFileDataLength |= (((unsigned long)pucApduData[5+ucFileNameLength] << 8) & 0x0000ff00);
				ulFileDataLength |= ((unsigned long)pucApduData[6+ucFileNameLength] & 0x000000ff);
				if (ulApduLen == 2 + 1 + ucFileNameLength + 4 + ulFileDataLength)
				{
					if (ucFileNameLength)
					{
						pucFileNameByte = (unsigned char *)CA_MEM_Alloc(sizeof(unsigned char) * (ucFileNameLength + 1)); // '+ 1'은 끝에 NULL 코드 붙이기 위해. 사실상 불필요하지만서두...
						VK_memset(pucFileNameByte, 0, ucFileNameLength + 1);
						VK_memcpy(pucFileNameByte, &pucApduData[3], ucFileNameLength);
						PrintNotify(("[CIAM]---> CiAmmi_FileAck() : FileNameByte = [%d][%s]\n", ucFileNameLength, pucFileNameByte));
					}
					if (ulFileDataLength)
					{
						pucFileDataByte = (unsigned char *)CA_MEM_Alloc(sizeof(unsigned char) * (ulFileDataLength + 1)); // '+ 1'은 끝에 NULL 코드 붙이기 위해. 사실상 불필요하지만서두...
						VK_memset(pucFileDataByte, 0, ulFileDataLength + 1);
						VK_memcpy(pucFileDataByte, &pucApduData[7+ucFileNameLength], ulFileDataLength);
						PrintNotify(("[CIAM]---> CiAmmi_FileAck() : FileDataByte = [%ld]\n", ulFileDataLength));
					}
				}
				else
				{
					PrintErr(("[CIAM]---> CiAmmi_FileAck()---> Error : requested file is wrong (%ld != %ld) !!!\n",
						ulApduLen, 2 + 1 + ucFileNameLength + 4 + ulFileDataLength));
					ucFileOK = 0;
				}
				break;

			case CI_AMMI_REQUEST_TYPE_DATA:
				ulDataByteLength = ulApduLen - 2;
				if (ulDataByteLength)
				{
					pucDataByte = (unsigned char *)CA_MEM_Alloc(sizeof(unsigned char) * (ulDataByteLength + 1)); // '+ 1'은 끝에 NULL 코드 붙이기 위해. 사실상 불필요하지만서두...
					VK_memset(pucDataByte, 0, ulDataByteLength + 1);
					VK_memcpy(pucDataByte, &pucApduData[2], ulDataByteLength);
					PrintNotify(("[CIAM]---> CiAmmi_FileAck() : DataByte = [%ld]\n", ulDataByteLength));
				}
				break;

			default:
				PrintErr(("[CIAM]---> CiAmmi_FileAck()---> Error : Unknown Request Type[0x%x] !!!\n", ucReqType));
				return -1;
		}
	}
	else // if the requested file or data is not available
	{
		PrintErr(("[CIAM]---> CiAmmi_FileAck()---> Error : requested file or data is not available !!!\n"));
	}

	if (s_pfnAmmiCallback)
	{
		VK_memset(&ustContent, 0, sizeof(CI_AMMI_CONTENT));
		ustContent.typeFileAck.usSessionId = usSsNo;
		ustContent.typeFileAck.ucFileOK = ucFileOK;
		ustContent.typeFileAck.ucReqType = ucReqType;
		ustContent.typeFileAck.ucFileNameLength = ucFileNameLength;
		ustContent.typeFileAck.pucFileNameByte = pucFileNameByte;
		ustContent.typeFileAck.ulFileDataLength = ulFileDataLength;
		ustContent.typeFileAck.pucFileDataByte = pucFileDataByte;
		ustContent.typeFileAck.ulDataByteLength = ulDataByteLength;
		ustContent.typeFileAck.pucDataByte = pucDataByte;
		s_pfnAmmiCallback(CI_AMMI_MSG_FILE_ACK, ustContent); // 이 경우 리턴값은 don't care
	}
	else // 이런 경우는 없겠지만 만약 발생한다면...
	{
		PrintErr(("[CIAM]---> CiAmmi_FileAck()---> Error : callback not registered !!!\n"));
	}

	if (pucFileNameByte)
		CA_MEM_Free(pucFileNameByte);
	if (pucFileDataByte)
		CA_MEM_Free(pucFileDataByte);
	if (pucDataByte)
		CA_MEM_Free(pucDataByte);

	return ERR_OK;
}

static int CiAmmi_AppAbort(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen)
{
	unsigned char	ucAbortReqCode; /* CI_AMMI_ABORT_REQUEST_CODE */
	CI_AMMI_CONTENT	ustContent;
	unsigned char ucMsg[5];
	int i = 0;

	PrintNotify(("[CIAM]---> CiAmmi_AppAbort() : ulApduLen = %ld\n", ulApduLen));

	ucMsg[i++] = (unsigned char)((CI_APP_ABORT_ACK >> 16) & 0xff);
	ucMsg[i++] = (unsigned char)((CI_APP_ABORT_ACK >> 8) & 0xff);
	ucMsg[i++] = (unsigned char)(CI_APP_ABORT_ACK & 0xff);

	/* <TODO_CI_PLUS> : 이렇게 처리하는 것이 맞는지 확인 필요 (CI+ 스펙 1.2, TS 101 699 1.1.1, D-BOOK 5.0 모두 모호하게 기술되어 있음) */
	if (ulApduLen == 0)
	{
		ucMsg[i++] = 0x00;
		ucAbortReqCode = CI_AMMI_ABORT_REQUEST_CODE_SYSTEM_CANCEL;
	}
	else
	{
		ucMsg[i++] = 0x01;
		ucMsg[i++] = pucApduData[0];
		ucAbortReqCode = pucApduData[0];
	}

	if (s_pfnAmmiCallback)
	{
		VK_memset(&ustContent, 0, sizeof(CI_AMMI_CONTENT));
		ustContent.typeReqAppAbort.usSessionId = usSsNo;
		ustContent.typeReqAppAbort.ucAbortReqCode = ucAbortReqCode;
		s_pfnAmmiCallback(CI_AMMI_MSG_REQ_APP_ABORT, ustContent); // AbortAckCode 는 받을 필요가 없다...
	}
	else // 이런 경우는 없겠지만 만약 발생한다면...
	{
		PrintErr(("[CIAM]---> CiAmmi_AppAbort()---> Error : callback not registered !!!\n"));
	}

	if (CIRH_SendApdu(usSsNo, ucMsg, i) != ERR_OK)
	{
		PrintErr(("[CIAM]---> CiAmmi_AppAbort()---> Error : send APDU msg. SsNo[%d] !!!\n", usSsNo));
		return -1;
	}

	PrintDbg(("[CIAM]---> CiAmmi_AppAbort()---> SsNo[%d]...\n", usSsNo));

	return ERR_OK;
}

int CiAmmi_AppAbortRequest(unsigned short usSsNo, unsigned char ucAbortReqCode /* CI_AMMI_ABORT_REQUEST_CODE */)
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

	unsigned char ucMsg[5];

	if (ucAbortReqCode != CI_AMMI_ABORT_REQUEST_CODE_USER_CANCEL && ucAbortReqCode != CI_AMMI_ABORT_REQUEST_CODE_SYSTEM_CANCEL)
	{
		PrintErr(("[CIAM]---> CiAmmi_AppAbortRequest()---> Error : Unknown abort request(0x%x) !!!\n", ucAbortReqCode));
		return -1;
	}

	PrintNotify(("[CIAM]---> CiAmmi_AppAbortRequest() : ucAbortReqCode = 0x%x\n", ucAbortReqCode));

	ucMsg[0] = (unsigned char)((CI_APP_ABORT_REQ >> 16) & 0xff);
	ucMsg[1] = (unsigned char)((CI_APP_ABORT_REQ >> 8) & 0xff);
	ucMsg[2] = (unsigned char)(CI_APP_ABORT_REQ & 0xff);
	ucMsg[3] = 0x01;
	ucMsg[4] = ucAbortReqCode;

	if (CIRH_SendApdu(usSsNo, ucMsg, 5) != ERR_OK)
	{
		PrintErr(("[CIAM]---> CiAmmi_AppAbortRequest()---> Error : send APDU msg. SsNo[%d] !!!\n", usSsNo));
		return -1;
	}

	PrintDbg(("[CIAM]---> CiAmmi_AppAbortRequest()---> SsNo[%d]...\n", usSsNo));

	return ERR_OK;
}

static int CiAmmi_AppAbortAck(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen)
{
//	unsigned char	ucAbortAckCode;
	CI_AMMI_CONTENT	ustContent;

	PrintNotify(("[CIAM]---> CiAmmi_AppAbortAck() : ulApduLen = %ld\n", ulApduLen));

	/* ucAbortAckCode 처리 필요 (CI+ 스펙 1.2, TS 101 699 1.1.1, D-BOOK 5.0 모두 분명히 명시되어 있지 않아 일단 막아둠)
	   -> SmarDTV CI+ 캠으로 확인해 보니 굳이 처리할 필요 없음. */

	if (s_pfnAmmiCallback)
	{
		VK_memset(&ustContent, 0, sizeof(CI_AMMI_CONTENT));
		ustContent.typeAppAbortAck.usSessionId = usSsNo;
//		ustContent.typeAppAbortAck.ucAbortAckCode = ucAbortAckCode;
		s_pfnAmmiCallback(CI_AMMI_MSG_REQ_APP_ABORT_ACK, ustContent); // 이 경우 리턴값은 don't care
	}
	else // 이런 경우는 없겠지만 만약 발생한다면...
	{
		PrintErr(("[CIAM]---> CiAmmi_AppAbortAck()---> Error : callback not registered !!!\n"));
	}

	PrintDbg(("[CIAM]---> CiAmmi_AppAbortAck()---> SsNo[%d]...\n", usSsNo));

	return ERR_OK;
}

