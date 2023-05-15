/*
	Viaccess UI API
		User API Manual ACS 3.0 - Doc. Ref. 2218
*/

/********************************************************************
*	Including Headers
********************************************************************/
#include "va_def.h"

#include "va_ui.h"
#include "va_ui_api.h"
#include "va_ui_adapt.h"

#include "va_pi_api.h"

#include "vkernel.h"

#include <string.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/







#define VA_UI_REQUEST_TIMEOUT			5000

#define VA_UI_EVENT_MEM_ALLOC_RETRY		10
#define VA_UI_EVENT_RECEIVE_TIMER_ID	0x500
#define VA_UI_EVENT_RECEIVE_TIMEOUT		500

#define VA_UI_NONE						0x0
#define VA_UI_ERROR						(VA_UI_NONE + 1)
#define VA_UI_PREVIEW					(VA_UI_NONE + 2)
#define VA_UI_MATURITY_RATE				(VA_UI_NONE + 3)

#define GET_ACTION_ID_FROM_STBHANDLE(a)		((a >> 24) & 0xFF)

/********************************************************************
*	External variables & function prototypes
********************************************************************/

typedef struct
{
	VA_UI_EVENT_TYPE eEventType;
	HUINT32 uiEsHandle;
	HUINT32 uiDscrType;
	HBOOL bDscrState;
}VA_UI_ES_EVENT;

typedef struct
{
	VA_UI_ES_EVENT stEsInfo[eEstype_Max];
	VA_UI_EVENT stEvent;
	HBOOL bChangeService;
	HUINT16 usAudioPid;
	HBOOL bChangeAudio;
	HUINT8 ucGetEsCnt;
	HBOOL bSetTimer;
	HUINT32 uiTimerId;
}VA_UI_EVENT_STATE;

/********************************************************************
*	Static variables
********************************************************************/
static UINT32 sEventHandle;

static UINT32 sVaUiRequestQid;
static UINT32 sVaUiSemId;
static tVA_UI_Request stRequest;

static VA_UI_EVENT_STATE s_stEventState[CAS_MAX_NUM_OF_ACTION];

/********************************************************************
*	Static functions Prototypes
*	The following functions will be used as inner functions
********************************************************************/
static void VA_UI_RequestCallback(tVA_UI_Request *pRequest);
static void VA_UI_EventCallback(tVA_UI_Event *pEvent);


VA_UI_EVENT_STATE *VA_UI_GetEventState(HINT32 nActionId)
{
	if(nActionId >= CAS_MAX_NUM_OF_ACTION)
	{
		return NULL;
	}

	return &s_stEventState[nActionId];
}

/*
	initialize variables and start task
*/
void VA_UI_Init()
{
	int nErr, i, j;

	HxLOG_Print("[Enter]\n");

	for(i = 0 ; i < CAS_MAX_NUM_OF_ACTION ; i++)
	{
		s_stEventState[i].bSetTimer = FALSE;
		s_stEventState[i].uiTimerId = (VA_UI_EVENT_RECEIVE_TIMER_ID + i);
		s_stEventState[i].bChangeService = FALSE;
		s_stEventState[i].usAudioPid = VA_INVALID_PID;
		s_stEventState[i].bChangeAudio = FALSE;
		s_stEventState[i].ucGetEsCnt = 0;
		VK_memset32(&s_stEventState[i].stEvent, 0, sizeof(VA_UI_EVENT));
		VK_memset32(&s_stEventState[i].stEsInfo, 0, sizeof(VA_UI_ES_EVENT));
	}

	VA_UI_Adapt_Init();

//	VA_INIT_InitializeAcs() 후에 VA_UI_RegisterEventCallbackFunction() 로 별도 처리함.
//	sEventHandle = VA_UI_AddEventListener(VA_UI_EventCallback, NULL);

	// for handling request
	VA_MSG_Create(1, sizeof(UINT32), VA_UI_REQUEST_QUEUE_NAME, &sVaUiRequestQid);

	nErr = VA_SEM_Create(VA_UI_SEM_NAME, &sVaUiSemId);
	if (nErr)
	{
		HxLOG_Error("[VA_UI_Init] failed to create semaphore (%d)\n", nErr);
		VA_ASSERT(nErr == VK_OK);
	}

	HxLOG_Print("[Exit]\n");
}

void VA_UI_RegisterEventCallbackFunction()
{
	sEventHandle = VA_UI_AddEventListener(VA_UI_EventCallback, NULL);
}

//## 전반적으루다가.. 여기서는 아무것도 안하고 일단 다 넘기겠음..
static void VA_UI_EventCallback(tVA_UI_Event *pEvent)
{
	static VA_UI_EVENT stEventCopy;

	HxLOG_Print("[Enter]\n");

	VA_ASSERT(pEvent != NULL);

	stEventCopy.eEventType 				= pEvent->eEventType;
	stEventCopy.ulScSlot 				= pEvent->dwScSlot;
	stEventCopy.ulAcsId 				= pEvent->dwAcsId;
	stEventCopy.ulStbStreamHandle 		= pEvent->dwStbStreamHandle;
	stEventCopy.pUserArg 				= pEvent->pUserArg;
	stEventCopy.bIsScrambledByProgram 	= pEvent->bIsScrambledByProgram;
	stEventCopy.nStreamNumber 			= pEvent->uiStreamsNumber;
	stEventCopy.nStreamType				= (stEventCopy.ulStbStreamHandle == VA_INVALID_HANDLE) ? eEsType_None : VA_PI_Adapt_GetStreamTypeFromStreamHandle(pEvent->dwStbStreamHandle);

	VA_UI_Adapt_MsgHandler(&stEventCopy);

	HxLOG_Print("[Exit]\n");
}

// Request functions
/*
    Below requests are not implemented

    <PVR Request>
    eVA_UI_GET_PVR_INFO_REQUEST,

    <ip or purple box>
    eVA_UI_ASK_HEADEND_PROCESS_REQUEST,
    eVA_UI_HEADEND_CREDIT_REQUEST,
    eVA_UI_HEADEND_PROGRAM_NUMBER_REQUEST,

    <SmartCardLess Requests>
    eVA_UI_GET_TERMINAL_ACCESS_CONDITION_REQUEST,
    eVA_UI_GET_TERMINAL_INFO_REQUEST,
    eVA_UI_GET_TERMINAL_UA_REQUEST,
    eVA_UI_GET_TERMINAL_SOID_LIST_REQUEST,
    eVA_UI_GET_TERMINAL_LABEL_REQUEST,
    eVA_UI_GET_TERMINAL_PIN_CODE_ATTEMPTS_REQUEST,
    eVA_UI_MODIFY_TERMINAL_PIN_CODE_REQUEST,
    eVA_UI_GET_TERMINAL_CLASS_LIST_REQUEST,
    eVA_UI_GET_TERMINAL_PREBOOKED_LIST_REQUEST,
    eVA_UI_GET_TERMINAL_PREVIEW_INFO_REQUEST,
    eVA_UI_GET_TERMINAL_MATURITY_RATING_REQUEST,
    eVA_UI_MODIFY_TERMINAL_MATURITY_RATING_REQUEST,
    eVA_UI_ACCEPT_TERMINAL_MATURITY_RATING_REQUEST,
    eVA_UI_GET_TERMINAL_FAC_DATA_REQUEST,
    eVA_UI_SUBMIT_TERMINAL_EXM_REQUEST,
    eVA_UI_GET_TERMINAL_ID_REQUEST,
    eVA_UI_SET_TERMINAL_ACTIVATION_CODE_REQUEST,
    eVA_UI_GET_TERMINAL_NETWORK_SETUP_REQUEST,
*/
static void VA_UI_RequestCallback(tVA_UI_Request *pRequest)
{
	UINT32 ulType;

	HxLOG_Print("[Enter]\n");

	VA_ASSERT(pRequest != NULL);

	// send a message to inform VA_UI_SendRequest() that the result of the request is returned
	ulType = pRequest->stRequestHeader.eType;
	if (VA_MSG_Send(sVaUiRequestQid, &ulType, sizeof(UINT32)) != VK_OK)
	{
		HxLOG_Error("fail to send message\n");
	}

	HxLOG_Print("[Exit]\n");
}

// return NULL when it fails to get result of the request
// or return a pointer to the request
static tVA_UI_RequestArg *VA_UI_SendRequest(tVA_UI_RequestType eType, UINT32 ulScSlot, tVA_UI_RequestStatus *pStatus, tVA_UI_Request *pRequest)
{
	UINT32 ulType;
	int nErr;

	HxLOG_Print("[Enter]\n");

	VA_ASSERT(pStatus != NULL);
	VA_ASSERT(pRequest != NULL);

	VA_ASSERT(sVaUiSemId);
	VA_SEM_Get(sVaUiSemId);

	pRequest->stRequestHeader.eType = eType;
	pRequest->stRequestHeader.dwScSlot = ulScSlot;
	pRequest->stRequestHeader.eStatus = eVA_UI_LAST_STATUS;
	pRequest->stRequestHeader.pCallback = VA_UI_RequestCallback;
	pRequest->stRequestHeader.pUserArg = NULL;

	nErr = VA_UI_Request(pRequest);
	if (nErr != kVA_OK)
	{
//		VA_SEM_Release(sVaUiSemId);
		HxLOG_Error("VA_UI_Request return %d\n", nErr);
		return NULL;
	}

	if (VA_MSG_ReceiveTimeout(sVaUiRequestQid, (void *)&ulType, sizeof(UINT32), VA_UI_REQUEST_TIMEOUT) == VK_OK)
	{
		if (ulType != eType)
		{
//			VA_SEM_Release(sVaUiSemId);
			HxLOG_Error("wrong type returned [%d, %d]\n", ulType, eType);
			return NULL;
		}
	}
	else
	{
//		VA_SEM_Release(sVaUiSemId);
		HxLOG_Error("fail to receive msg\n");
		return NULL;
	}

	*pStatus = pRequest->stRequestHeader.eStatus;

//	VA_SEM_Release(sVaUiSemId);

	HxLOG_Print("[Exit]\n");

	// pRequest 가 전역 변수이기 때문에 세마포로 충분히 보호되지 못하는 문제점이 있다!
	// 원래 로컬 변수였던 것 같은데... 성능 향상 등을 이유로 전역 변수로 변경된 듯...
	// 여기서는 세마포 릴리즈를 하지 않도록 하고 VA_UI_SendRequestDone() 함수를 이용하여 해결하자.
	return &pRequest->uRequest;
}

static void VA_UI_SendRequestDone(void) // VA_UI_SendRequest() 가 호출되는 곳에선 반드시 이 함수가 불려야 한다!
{
	VA_SEM_Release(sVaUiSemId);
}

// the size of the array pointed by <pName> must be larger than <kVA_UI_MAX_VERSION_NAME_SIZE + 1>
int VA_UI_GetVersionName(UINT8 *pName)
{
	tVA_UI_RequestStatus eStatus = eVA_UI_ERROR_STATUS;
	tVA_UI_GetVersionNameRequest *pArg = NULL;
//	tVA_UI_Request stRequest;

	HxLOG_Print("[Enter]\n");

	VA_ASSERT(pName != NULL);

	pArg = (tVA_UI_GetVersionNameRequest *)VA_UI_SendRequest(eVA_UI_GET_VERSION_NAME_REQUEST, kVA_UI_NOT_USED, &eStatus, &stRequest);
	if (pArg == NULL || eStatus != eVA_UI_OK_STATUS)
	{
		VA_UI_SendRequestDone();
		HxLOG_Error("fail to send request [%x, %d]\n", pArg, eStatus);
		return VA_ERR;
	}

	VA_memcpy(pName, pArg->pVersionName, kVA_UI_MAX_VERSION_NAME_SIZE + 1);

	VA_UI_SendRequestDone();
	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}

int VA_UI_GetVersion(tVA_UI_GetVersionRequest *pVersion)
{
	tVA_UI_RequestStatus eStatus = eVA_UI_ERROR_STATUS;
	tVA_UI_GetVersionRequest *pArg = NULL;
//	tVA_UI_Request stRequest;

	HxLOG_Print("[Enter]\n");

	VA_ASSERT(pVersion != NULL);

	pArg = (tVA_UI_GetVersionRequest *)VA_UI_SendRequest(eVA_UI_GET_VERSION_REQUEST, kVA_UI_NOT_USED, &eStatus, &stRequest);
	if (pArg == NULL || eStatus != eVA_UI_OK_STATUS)
	{
		VA_UI_SendRequestDone();
		HxLOG_Error("fail to send request [%x, %d]\n", pArg, eStatus);
		return VA_ERR;
	}

	VA_memcpy(pVersion, pArg, sizeof(tVA_UI_GetVersionRequest));

	VA_UI_SendRequestDone();
	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}

int VA_UI_GetAccessCondition(UINT32 ulAcsId, UINT32 ulStbStreamHandle, UINT32 ulStreamType, tVA_UI_GetAccessConditionRequest *pParam)
{
	tVA_UI_RequestStatus eStatus = eVA_UI_ERROR_STATUS;
	tVA_UI_GetAccessConditionRequest *pArg = NULL;
//	tVA_UI_Request stRequest;

	HxLOG_Print("[Enter]\n");

	VA_ASSERT(pParam != NULL);

	stRequest.uRequest.stGetAccessConditionRequest.dwAcsId = ulAcsId;
	stRequest.uRequest.stGetAccessConditionRequest.dwStbStreamHandle = ulStbStreamHandle;
	stRequest.uRequest.stGetAccessConditionRequest.dwStreamType = ulStreamType;

	pArg = (tVA_UI_GetAccessConditionRequest *)VA_UI_SendRequest(eVA_UI_GET_ACCESS_CONDITION_REQUEST, kVA_UI_NOT_USED, &eStatus, &stRequest);
	if (pArg == NULL || eStatus != eVA_UI_OK_STATUS)
	{
		VA_UI_SendRequestDone();
		HxLOG_Error("fail to send request [%x, %d]\n", pArg, eStatus);
		return VA_ERR;
	}

	VA_memcpy(pParam, pArg, sizeof(tVA_UI_GetAccessConditionRequest));

	VA_UI_SendRequestDone();
	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}

int VA_UI_GetSmartcardInfo(UINT32 ulScSlot, tVA_UI_SmartcardInfo *pInfo)
{
	tVA_UI_RequestStatus eStatus = eVA_UI_ERROR_STATUS;
	tVA_UI_GetSmartcardInfoRequest *pArg = NULL;
//	tVA_UI_Request stRequest;

	HxLOG_Print("[Enter]\n");

	VA_ASSERT(pInfo != NULL);

	pArg = (tVA_UI_GetSmartcardInfoRequest *)VA_UI_SendRequest(eVA_UI_GET_SMARTCARD_INFO_REQUEST, ulScSlot, &eStatus, &stRequest);
	if (pArg == NULL || eStatus != eVA_UI_OK_STATUS)
	{
		VA_UI_SendRequestDone();
		HxLOG_Error("fail to send request [%x, %d]\n", pArg, eStatus);
		return VA_ERR;
	}

	VA_memcpy(pInfo, &pArg->stSmartcardInfo, sizeof(tVA_UI_SmartcardInfo));

	VA_UI_SendRequestDone();
	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}

int VA_UI_GetSmartcardListInfo(tVA_UI_GetSmartcardListInfoRequest *pInfo)
{
	tVA_UI_RequestStatus eStatus = eVA_UI_ERROR_STATUS;
	tVA_UI_GetSmartcardListInfoRequest *pArg = NULL;
//	tVA_UI_Request stRequest;

	HxLOG_Print("[Enter]\n");

	VA_ASSERT(pInfo != NULL);

	pArg = (tVA_UI_GetSmartcardListInfoRequest *)VA_UI_SendRequest(eVA_UI_GET_SMARTCARD_LIST_INFO_REQUEST, kVA_UI_NOT_USED, &eStatus, &stRequest);
	if (pArg == NULL || eStatus != eVA_UI_OK_STATUS)
	{
		VA_UI_SendRequestDone();
		HxLOG_Error("fail to send request [%x, %d]\n", pArg, eStatus);
		return VA_ERR;
	}

	VA_memcpy(pInfo, pArg, sizeof(tVA_UI_GetSmartcardListInfoRequest));

	VA_UI_SendRequestDone();
	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}

int VA_UI_GetUA(UINT32 ulScSlot, tVA_UI_Ua *pUA)
{
	tVA_UI_RequestStatus eStatus = eVA_UI_ERROR_STATUS;
	tVA_UI_GetUaRequest *pArg = NULL;
//	tVA_UI_Request stRequest;

	HxLOG_Print("[Enter]\n");

	VA_ASSERT(pUA != NULL);

	pArg = (tVA_UI_GetUaRequest *)VA_UI_SendRequest(eVA_UI_GET_UA_REQUEST, ulScSlot, &eStatus, &stRequest);
	if (pArg == NULL || eStatus != eVA_UI_OK_STATUS)
	{
		VA_UI_SendRequestDone();
		HxLOG_Error("fail to send request [%x, %d]\n", pArg, eStatus);
		return VA_ERR;
	}

	VA_memcpy(pUA, pArg->pUa, sizeof(tVA_UI_Ua));

	VA_UI_SendRequestDone();
	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}

// if ok, return a pointer to the array which contains SOID list (it MUST be freed in the caller function)
//         <pulCount> indicates the number of SOID's which were read from the smartcard
// if fail, return NULL
tVA_UI_Soid *VA_UI_GetSoidList(UINT32 ulScSlot, UINT32 *pulCount)
{
	tVA_UI_RequestStatus eStatus = eVA_UI_ERROR_STATUS;
	tVA_UI_GetSoidListRequest *pArg = NULL;
//	tVA_UI_Request stRequest;
	tVA_UI_Soid *pSoid = NULL;
	int nSize = 16;

	HxLOG_Print("[Enter]\n");

	VA_ASSERT(pulCount != NULL);

	pSoid = VA_MEM_Alloc(nSize * sizeof(tVA_UI_Soid));
	if (pSoid  == NULL)
	{
		HxLOG_Error("fail to alloc memory\n");
		return NULL;
	}

	stRequest.uRequest.stGetSoidListRequest.uiMaxItemNumber = nSize;
	stRequest.uRequest.stGetSoidListRequest.pItemArray = pSoid;

	pArg = (tVA_UI_GetSoidListRequest *)VA_UI_SendRequest(eVA_UI_GET_SOID_LIST_REQUEST, ulScSlot, &eStatus, &stRequest);

	if (eStatus == eVA_UI_ARRAY_TOO_SMALL_STATUS)
	{
		// the array is too small: reallocate the array by the specified size and send a request again
		nSize = pArg->uiItemNumber;

		VA_UI_SendRequestDone();

		VA_MEM_Free(pSoid);
		pSoid = VA_MEM_Alloc(nSize * sizeof(tVA_UI_Soid));
		if (pSoid  == NULL)
		{
			HxLOG_Error("fail to alloc memory\n");
			return NULL;
		}

		stRequest.uRequest.stGetSoidListRequest.uiMaxItemNumber = nSize;
		stRequest.uRequest.stGetSoidListRequest.pItemArray = pSoid;

		HxLOG_Info("too small array: new size [%d]\n", nSize);

		pArg = (tVA_UI_GetSoidListRequest *)VA_UI_SendRequest(eVA_UI_GET_SOID_LIST_REQUEST, ulScSlot, &eStatus, &stRequest);
		if (pArg == NULL || eStatus != eVA_UI_OK_STATUS)
		{
			VA_UI_SendRequestDone();
			VA_MEM_Free(pSoid);
			HxLOG_Error("fail to send request [%x, %d]\n", pArg, eStatus);
			return NULL;
		}
	}
	else if (eStatus != eVA_UI_OK_STATUS)
	{
		VA_UI_SendRequestDone();
		VA_MEM_Free(pSoid);
		HxLOG_Error("fail to send request [%d]\n", eStatus);
		return NULL;
	}
	else if (pArg == NULL)
	{
		VA_UI_SendRequestDone();
		VA_MEM_Free(pSoid);
		HxLOG_Error("fail to send request\n");
		return NULL;
	}

	*pulCount = pArg->uiItemNumber;

	VA_UI_SendRequestDone();
	HxLOG_Print("[Exit]\n");
	return pSoid;
}

// the size of the array pointed by <pLabel> must be larger than <kVA_UI_MAX_LABEL_SIZE + 1>
int VA_UI_GetLabel(UINT32 ulScSlot, UINT32 ulSoid, UINT8 *pLabel)
{
	tVA_UI_RequestStatus eStatus = eVA_UI_ERROR_STATUS;
	tVA_UI_GetLabelRequest *pArg = NULL;
//	tVA_UI_Request stRequest;

	HxLOG_Print("[Enter]\n");

	VA_ASSERT(pLabel != NULL);
	VA_ASSERT(sizeof(UINT32) == sizeof(tVA_UI_Soid));

	stRequest.uRequest.stGetLabelRequest.uiSoid = (tVA_UI_Soid)ulSoid;

	pArg = (tVA_UI_GetLabelRequest *)VA_UI_SendRequest(eVA_UI_GET_LABEL_REQUEST, ulScSlot, &eStatus, &stRequest);
	if (pArg == NULL || eStatus != eVA_UI_OK_STATUS)
	{
		VA_UI_SendRequestDone();
		HxLOG_Error("fail to send request [%x, %d]\n", pArg, eStatus);
		return VA_ERR;
	}

	VA_memcpy(pLabel, pArg->pLabel, kVA_UI_MAX_LABEL_SIZE + 1);

	VA_UI_SendRequestDone();
	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}

int VA_UI_GetPinCodeAttempts(UINT32 ulScSlot, UINT8 *pucRemainingCount, UINT8 *pucMaxCount)
{
	tVA_UI_RequestStatus eStatus = eVA_UI_ERROR_STATUS;
	tVA_UI_GetPinCodeAttemptsRequest *pArg = NULL;
//	tVA_UI_Request stRequest;

	HxLOG_Print("[Enter]\n");

	VA_ASSERT(pucRemainingCount != NULL);
	VA_ASSERT(pucMaxCount != NULL);

	pArg = (tVA_UI_GetPinCodeAttemptsRequest *)VA_UI_SendRequest(eVA_UI_GET_PIN_CODE_ATTEMPTS_REQUEST, ulScSlot, &eStatus, &stRequest);
	if (pArg == NULL || eStatus != eVA_UI_OK_STATUS)
	{
		VA_UI_SendRequestDone();
		HxLOG_Error("fail to send request [%x, %d]\n", pArg, eStatus);
		return VA_ERR;
	}

	*pucRemainingCount = stRequest.uRequest.stGetPinCodeAttemptsRequest.uiRemainingAttemptsCount;
	*pucMaxCount = stRequest.uRequest.stGetPinCodeAttemptsRequest.uiMaxAttempts;

	VA_UI_SendRequestDone();
	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}

int VA_UI_ModifyPinCode(UINT32 ulScSlot, UINT8 *pPinCode, UINT8 *pNewPinCode)
{
	tVA_UI_RequestStatus eStatus = eVA_UI_ERROR_STATUS;
	tVA_UI_ModifyPinCodeRequest *pArg = NULL;
//	tVA_UI_Request stRequest;

	HxLOG_Print("[Enter]\n");

	VA_ASSERT(pPinCode != NULL);
	VA_ASSERT(pNewPinCode != NULL);

	VA_memcpy(stRequest.uRequest.stModifyPinCodeRequest.pPinCode, pPinCode, sizeof(tVA_UI_PinCode));
	VA_memcpy(stRequest.uRequest.stModifyPinCodeRequest.pNewPinCode, pNewPinCode, sizeof(tVA_UI_PinCode));

	pArg = (tVA_UI_ModifyPinCodeRequest *)VA_UI_SendRequest(eVA_UI_MODIFY_PIN_CODE_REQUEST, ulScSlot, &eStatus, &stRequest);
	if (eStatus != eVA_UI_OK_STATUS)
	{
		VA_UI_SendRequestDone();
		HxLOG_Error("fail to send request [%x, %d]\n", pArg, eStatus);
		switch (eStatus)
		{
			case eVA_UI_WRONG_PIN_CODE_STATUS:					return VA_ERR_UI_WRONG_PIN_CODE;
			case eVA_UI_LOCKED_PIN_CODE_STATUS:					return VA_ERR_UI_LOCKED_PIN_CODE;
			case eVA_UI_MAX_CHANGES_PIN_CODE_STATUS:				return VA_ERR_UI_MAX_CHANGES_PIN_CODE;
			case eVA_UI_FORBIDDEN_PIN_CODE_STATUS:					return VA_ERR_UI_FORBIDDEN_PIN_CODE;
			case eVA_UI_NO_PIN_CODE_STATUS:						return VA_ERR_UI_NO_PIN_CODE;
			case eVA_UI_SMARTCARD_INSUFFICIENT_MEMORY_STATUS:		return VA_ERR_UI_SMARTCARD_INSUFFICIENT_MEMORY;
			case eVA_UI_WRONG_SLOT_STATUS:						return VA_ERR_UI_WRONG_SLOT;
			case eVA_UI_SMARTCARD_ERROR_STATUS:					return VA_ERR_UI_SMARTCARD_ERROR;
			default:													return VA_ERR;
		}
	}
	else if (pArg == NULL)
	{
		VA_UI_SendRequestDone();
		HxLOG_Error("fail to send request\n");
		return VA_ERR;
	}

	VA_UI_SendRequestDone();
	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}

// this function must be called when eVA_UI_CONSULTATION_LOCKED_STATUS is returned (in case of PC2.3 and PC2.4 smartcard)
int VA_UI_UnlockConsultation(UINT32 ulScSlot, UINT8 *pPinCode)
{
	tVA_UI_RequestStatus eStatus = eVA_UI_ERROR_STATUS;
	tVA_UI_UnlockConsultationRequest *pArg = NULL;
//	tVA_UI_Request stRequest;

	HxLOG_Print("[Enter]\n");

	VA_ASSERT(pPinCode != NULL);

	VA_memcpy(stRequest.uRequest.stUnlockConsultationRequest.pPinCode, pPinCode, sizeof(tVA_UI_PinCode));

	pArg = (tVA_UI_UnlockConsultationRequest *)VA_UI_SendRequest(eVA_UI_UNLOCK_CONSULTATION_REQUEST, ulScSlot, &eStatus, &stRequest);
	if (eStatus != eVA_UI_OK_STATUS)
	{
		VA_UI_SendRequestDone();
		HxLOG_Error("fail to send request [%x, %d]\n", pArg, eStatus);
		switch (eStatus)
		{
			case eVA_UI_WRONG_PIN_CODE_STATUS:					return VA_ERR_UI_WRONG_PIN_CODE;
			case eVA_UI_LOCKED_PIN_CODE_STATUS:					return VA_ERR_UI_LOCKED_PIN_CODE;
			case eVA_UI_NO_PIN_CODE_STATUS:						return VA_ERR_UI_NO_PIN_CODE;
			case eVA_UI_WRONG_SLOT_STATUS:						return VA_ERR_UI_WRONG_SLOT;
			case eVA_UI_SMARTCARD_ERROR_STATUS:					return VA_ERR_UI_SMARTCARD_ERROR;
			default:													return VA_ERR;
		}
	}
	else if (pArg == NULL)
	{
		VA_UI_SendRequestDone();
		HxLOG_Error("fail to send request\n");
		return VA_ERR;
	}

	VA_UI_SendRequestDone();
	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}

// get all of the class subscription values
int VA_UI_GetClassListAll(UINT32 ulScSlot, UINT32 ulSoid, UINT32 *pulCount, tVA_UI_ClassItem **pClass)
{
	tVA_UI_DateRange stDate;

	VA_ASSERT(pulCount != NULL);
	VA_ASSERT(pClass != NULL);
	VA_ASSERT(sizeof(UINT32) == sizeof(tVA_UI_Soid));

	stDate.stMin.uiYear = 1980;
	stDate.stMin.uiMonth = 1;
	stDate.stMin.uiDay = 1;

	stDate.stMax.uiYear = 2107;
	stDate.stMax.uiMonth = 12;
	stDate.stMax.uiDay = 31;

	return VA_UI_GetClassList(ulScSlot, ulSoid, &stDate, pulCount, pClass);
}

// <pClass> MUST be freed in the caller function
// <pulCount> indicates the number of Classes which were read from the smartcard
int VA_UI_GetClassList(UINT32 ulScSlot, UINT32 ulSoid, tVA_UI_DateRange *pDate, UINT32 *pulCount, tVA_UI_ClassItem **pClass)
{
	tVA_UI_RequestStatus eStatus = eVA_UI_ERROR_STATUS;
	tVA_UI_GetClassListRequest *pArg = NULL;
//	tVA_UI_Request stRequest;
	int nSize = 60;
	tVA_UI_ClassItem *pClassList = NULL;

	HxLOG_Print("[Enter]\n");

	VA_ASSERT(pDate != NULL);
	VA_ASSERT(pulCount != NULL);
	VA_ASSERT(pClass != NULL);
	VA_ASSERT(sizeof(UINT32) == sizeof(tVA_UI_Soid));

	// VA_ERR_OK 가 아닌 경우는 그 리턴값에 대해 모두 VA_UI_Adapt_GetClassList()에서 핸들링 하도록 했음.

	pClassList = VA_MEM_Alloc(nSize * sizeof(tVA_UI_ClassItem));
	if (pClassList  == NULL)
	{
		HxLOG_Error("fail to alloc memory\n");
		return VA_ERR;
	}

	stRequest.uRequest.stGetClassListRequest.uiSoid = ulSoid;
	stRequest.uRequest.stGetClassListRequest.uiMaxItemNumber = nSize;
	stRequest.uRequest.stGetClassListRequest.pItemArray = pClassList;
	VA_memcpy(&stRequest.uRequest.stGetClassListRequest.stDateRange, pDate, sizeof(tVA_UI_DateRange));

	pArg = (tVA_UI_GetClassListRequest *)VA_UI_SendRequest(eVA_UI_GET_CLASS_LIST_REQUEST, ulScSlot, &eStatus, &stRequest);

	if (eStatus == eVA_UI_CONSULTATION_LOCKED_STATUS)
	{
		VA_UI_SendRequestDone();
		VA_MEM_Free(pClassList);
		HxLOG_Info("eVA_UI_CONSULTATION_LOCKED_STATUS\n");
		return VA_ERR_UI_CONSULTATION_LOCKED;
	}
	else if (eStatus == eVA_UI_INFO_NOT_AVAILABLE_STATUS)
	{
		VA_UI_SendRequestDone();
		VA_MEM_Free(pClassList);
		HxLOG_Info("eVA_UI_INFO_NOT_AVAILABLE_STATUS\n");
		return VA_ERR;
	}
	else if (eStatus == eVA_UI_ARRAY_TOO_SMALL_STATUS)
	{
		// the array is too small: reallocate the array by the specified size and send a request again
		nSize = pArg->uiItemNumber;

		VA_UI_SendRequestDone();

		VA_MEM_Free(pClassList);
		pClassList = VA_MEM_Alloc(nSize * sizeof(tVA_UI_ClassItem));
		if (pClassList  == NULL)
		{
			HxLOG_Error("fail to alloc memory\n");
			return VA_ERR;
		}

		stRequest.uRequest.stGetClassListRequest.uiMaxItemNumber = nSize;
		stRequest.uRequest.stGetClassListRequest.pItemArray = pClassList;

		HxLOG_Info("too small array: new size [%d]\n", nSize);

		pArg = (tVA_UI_GetClassListRequest *)VA_UI_SendRequest(eVA_UI_GET_CLASS_LIST_REQUEST, ulScSlot, &eStatus, &stRequest);
		if (pArg == NULL)
		{
			VA_UI_SendRequestDone();
			VA_MEM_Free(pClassList);
			HxLOG_Error("fail to send request\n");
			return VA_ERR;
		}
	}
	else if (eStatus != eVA_UI_OK_STATUS)
	{
		VA_UI_SendRequestDone();
		VA_MEM_Free(pClassList);
		HxLOG_Error("fail to send request [%x, %d]\n", pArg, eStatus);
		return VA_ERR;
	}
	else if (pArg == NULL)
	{
		VA_UI_SendRequestDone();
		VA_MEM_Free(pClassList);
		HxLOG_Error("fail to send request\n");
		return VA_ERR;
	}

	*pClass = pClassList;
	*pulCount = pArg->uiItemNumber;

	VA_UI_SendRequestDone();
	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}

// get all of the theme/level subscription values
int VA_UI_GetThemeLevelListAll(UINT32 ulScSlot, UINT32 ulSoid, UINT32 *pulCount, tVA_UI_ThemeLevelItem **pTheme)
{
	tVA_UI_DateRange stDate;

	VA_ASSERT(pTheme != NULL);
	VA_ASSERT(pulCount != NULL);
	VA_ASSERT(sizeof(UINT32) == sizeof(tVA_UI_Soid));

	stDate.stMin.uiYear = 1980;
	stDate.stMin.uiMonth = 1;
	stDate.stMin.uiDay = 1;

	stDate.stMax.uiYear = 2107;
	stDate.stMax.uiMonth = 12;
	stDate.stMax.uiDay = 31;

	return VA_UI_GetThemeLevelList(ulScSlot, ulSoid, &stDate, pulCount, pTheme);
}

// <pTheme> MUST be freed in the caller function
// <pulCount> indicates the number of Classes which were read from the smartcard
int VA_UI_GetThemeLevelList(UINT32 ulScSlot, UINT32 ulSoid, tVA_UI_DateRange *pDate, UINT32 *pulCount, tVA_UI_ThemeLevelItem **pTheme)
{
	tVA_UI_RequestStatus eStatus = eVA_UI_ERROR_STATUS;
	tVA_UI_GetThemeLevelListRequest *pArg = NULL;
//	tVA_UI_Request stRequest;
	int nSize = 16;
	tVA_UI_ThemeLevelItem *pThemeList = NULL;

	HxLOG_Print("[Enter]\n");

	VA_ASSERT(pTheme != NULL);
	VA_ASSERT(pulCount != NULL);
	VA_ASSERT(pDate != NULL);
	VA_ASSERT(sizeof(UINT32) == sizeof(tVA_UI_Soid));

	// VA_ERR_OK 가 아닌 경우는 그 리턴값에 대해 모두 VA_UI_Adapt_GetThemeLevelList()에서 핸들링 하도록 했음.

	pThemeList = VA_MEM_Alloc(nSize * sizeof(tVA_UI_ThemeLevelItem));
	if (pThemeList  == NULL)
	{
		HxLOG_Error("fail to alloc memory\n");
		return VA_ERR;
	}

	stRequest.uRequest.stGetThemeLevelListRequest.uiSoid = ulSoid;
	stRequest.uRequest.stGetThemeLevelListRequest.uiMaxItemNumber = nSize;
	stRequest.uRequest.stGetThemeLevelListRequest.pItemArray = pThemeList;
	VA_memcpy(&stRequest.uRequest.stGetThemeLevelListRequest.stDateRange, pDate, sizeof(tVA_UI_DateRange));

	pArg = (tVA_UI_GetThemeLevelListRequest *)VA_UI_SendRequest(eVA_UI_GET_THEME_LEVEL_LIST_REQUEST, ulScSlot, &eStatus, &stRequest);

	if (eStatus == eVA_UI_CONSULTATION_LOCKED_STATUS)
	{
		VA_UI_SendRequestDone();
		VA_MEM_Free(pThemeList);
		HxLOG_Info("eVA_UI_CONSULTATION_LOCKED_STATUS\n");
		return VA_ERR_UI_CONSULTATION_LOCKED;
	}
	else if (eStatus == eVA_UI_INFO_NOT_AVAILABLE_STATUS)
	{
		VA_UI_SendRequestDone();
		VA_MEM_Free(pThemeList);
		HxLOG_Info("eVA_UI_INFO_NOT_AVAILABLE_STATUS\n");
		return VA_ERR;
	}
	else if (eStatus == eVA_UI_ARRAY_TOO_SMALL_STATUS)
	{
		// the array is too small: reallocate the array by the specified size and send a request again
		nSize = pArg->uiItemNumber;

		VA_UI_SendRequestDone();

		VA_MEM_Free(pThemeList);
		pThemeList = VA_MEM_Alloc(nSize * sizeof(tVA_UI_ThemeLevelItem));
		if (pThemeList  == NULL)
		{
			HxLOG_Error("fail to alloc memory\n");
			return VA_ERR;
		}

		stRequest.uRequest.stGetThemeLevelListRequest.uiMaxItemNumber = nSize;
		stRequest.uRequest.stGetThemeLevelListRequest.pItemArray = pThemeList;

		HxLOG_Info("too small array: new size [%d]\n", nSize);

		pArg = (tVA_UI_GetThemeLevelListRequest *)VA_UI_SendRequest(eVA_UI_GET_THEME_LEVEL_LIST_REQUEST, ulScSlot, &eStatus, &stRequest);
		if (pArg == NULL)
		{
			VA_UI_SendRequestDone();
			VA_MEM_Free(pThemeList);
			HxLOG_Error("fail to send request\n");
			return VA_ERR;
		}
	}
	else if (eStatus != eVA_UI_OK_STATUS)
	{
		VA_UI_SendRequestDone();
		VA_MEM_Free(pThemeList);
		HxLOG_Error("fail to send request [%x, %d]\n", pArg, eStatus);
		return VA_ERR;
	}
	else if (pArg == NULL)
	{
		VA_UI_SendRequestDone();
		VA_MEM_Free(pThemeList);
		HxLOG_Error("fail to send request\n");
		return VA_ERR;
	}

	*pulCount = pArg->uiItemNumber;
	*pTheme = pThemeList;

	VA_UI_SendRequestDone();
	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}

// get all of the theme/level subscription values
int VA_UI_GetPrebookedListAll(UINT32 ulScSlot, UINT32 ulSoid, UINT32 *pulCount, tVA_UI_ProgramNumberRange **pProgNum)
{
	VA_ASSERT(pProgNum != NULL);
	VA_ASSERT(pulCount != NULL);
	VA_ASSERT(sizeof(UINT32) == sizeof(tVA_UI_Soid));

	return VA_UI_GetPrebookedList(ulScSlot, ulSoid, 0, 0xFFFFFF, pulCount, pProgNum);
}

// <pProgNum> MUST be freed in the caller function
// <pulCount> indicates the number of Classes which were read from the smartcard
int VA_UI_GetPrebookedList(UINT32 ulScSlot, UINT32 ulSoid, UINT32 ulMin, UINT32 ulMax, UINT32 *pulCount, tVA_UI_ProgramNumberRange **pProgNum)
{
	tVA_UI_RequestStatus eStatus = eVA_UI_ERROR_STATUS;
	tVA_UI_GetPreBookedListRequest *pArg = NULL;
//	tVA_UI_Request stRequest;
	int nSize = 16;
	tVA_UI_ProgramNumberRange *pProgNumList = NULL;

	HxLOG_Print("[Enter]\n");

	VA_ASSERT(pProgNum != NULL);
	VA_ASSERT(pulCount != NULL);
	VA_ASSERT(sizeof(UINT32) == sizeof(tVA_UI_Soid));

	// VA_ERR_OK 가 아닌 경우는 그 리턴값에 대해 모두 VA_UI_Adapt_GetPrebookedlList()에서 핸들링 하도록 했음.

	pProgNumList = VA_MEM_Alloc(nSize * sizeof(tVA_UI_ProgramNumberRange));
	if (pProgNumList  == NULL)
	{
		HxLOG_Error("fail to alloc memory\n");
		return VA_ERR;
	}

	stRequest.uRequest.stGetPreBookedListRequest.uiSoid = ulSoid;
	stRequest.uRequest.stGetPreBookedListRequest.uiMaxItemNumber = nSize;
	stRequest.uRequest.stGetPreBookedListRequest.pItemArray = pProgNumList;
	stRequest.uRequest.stGetPreBookedListRequest.stProgramNumberRange.uiMin = ulMin;
	stRequest.uRequest.stGetPreBookedListRequest.stProgramNumberRange.uiMax = ulMax;

	pArg = (tVA_UI_GetPreBookedListRequest *)VA_UI_SendRequest(eVA_UI_GET_PREBOOKED_LIST_REQUEST, ulScSlot, &eStatus, &stRequest);

	if (eStatus == eVA_UI_CONSULTATION_LOCKED_STATUS)
	{
		VA_UI_SendRequestDone();
		VA_MEM_Free(pProgNumList);
		HxLOG_Info("eVA_UI_CONSULTATION_LOCKED_STATUS\n");
		return VA_ERR_UI_CONSULTATION_LOCKED;
	}
	else if (eStatus == eVA_UI_INFO_NOT_AVAILABLE_STATUS)
	{
		VA_UI_SendRequestDone();
		VA_MEM_Free(pProgNumList);
		HxLOG_Info("eVA_UI_INFO_NOT_AVAILABLE_STATUS\n");
		return VA_ERR;
	}
	else if (eStatus == eVA_UI_ARRAY_TOO_SMALL_STATUS)
	{
		// the array is too small: reallocate the array by the specified size and send a request again
		nSize = pArg->uiItemNumber;

		VA_UI_SendRequestDone();

		VA_MEM_Free(pProgNumList);
		pProgNumList = VA_MEM_Alloc(nSize * sizeof(tVA_UI_ProgramNumberRange));
		if (pProgNumList  == NULL)
		{
			HxLOG_Error("fail to alloc memory\n");
			return VA_ERR;
		}

		stRequest.uRequest.stGetPreBookedListRequest.uiMaxItemNumber = nSize;
		stRequest.uRequest.stGetPreBookedListRequest.pItemArray = pProgNumList;

		HxLOG_Info("too small array: new size [%d]\n", nSize);

		pArg = (tVA_UI_GetPreBookedListRequest *)VA_UI_SendRequest(eVA_UI_GET_PREBOOKED_LIST_REQUEST, ulScSlot, &eStatus, &stRequest);
		if (pArg == NULL)
		{
			VA_UI_SendRequestDone();
			VA_MEM_Free(pProgNumList);
			HxLOG_Error("fail to send request\n");
			return VA_ERR;
		}
	}
	else if (eStatus != eVA_UI_OK_STATUS)
	{
		VA_UI_SendRequestDone();
		VA_MEM_Free(pProgNumList);
		HxLOG_Error("fail to send request [%x, %d]\n", pArg, eStatus);
		return VA_ERR;
	}
	else if (pArg == NULL)
	{
		VA_UI_SendRequestDone();
		VA_MEM_Free(pProgNumList);
		HxLOG_Error("fail to send request\n");
		return VA_ERR;
	}

	*pProgNum = pProgNumList;
	*pulCount = pArg->uiItemNumber;

	VA_UI_SendRequestDone();
	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}

// after eVA_UI_DESCRAMBLING_OK_PREVIEW_EVENT is received, this function can be called to get more info
int VA_UI_GetPreviewInfo(UINT32 ulScSlot, UINT32 ulSoid, UINT8 ucPreviewIndex, UINT16 usPreviewNumber, UINT8 *ucUsedCwNumber)
{
	tVA_UI_RequestStatus eStatus = eVA_UI_ERROR_STATUS;
	tVA_UI_GetPreviewInfoRequest *pArg = NULL;
//	tVA_UI_Request stRequest;
//	int nSize = 16;

	HxLOG_Print("[Enter]\n");

	VA_ASSERT(sizeof(UINT32) == sizeof(tVA_UI_Soid));
	VA_ASSERT(ucUsedCwNumber != NULL);

	stRequest.uRequest.stGetPreviewInfoRequest.uiSoid = ulSoid;
	stRequest.uRequest.stGetPreviewInfoRequest.uiPreviewIndex= ucPreviewIndex;
	stRequest.uRequest.stGetPreviewInfoRequest.uiPreviewNumber= usPreviewNumber;

	pArg = (tVA_UI_GetPreviewInfoRequest *)VA_UI_SendRequest(eVA_UI_GET_PREVIEW_INFO_REQUEST, ulScSlot, &eStatus, &stRequest);

	if (eStatus == eVA_UI_CONSULTATION_LOCKED_STATUS)
	{
		VA_UI_SendRequestDone();
		HxLOG_Info("eVA_UI_CONSULTATION_LOCKED_STATUS\n");
		return VA_ERR_UI_CONSULTATION_LOCKED;
	}
	else if (eStatus != eVA_UI_OK_STATUS)
	{
		VA_UI_SendRequestDone();
		HxLOG_Error("fail to send request [%x, %d]\n", pArg, eStatus);
		return VA_ERR;
	}
	else if (pArg == NULL)
	{
		VA_UI_SendRequestDone();
		HxLOG_Error("fail to send request\n");
		return VA_ERR;
	}

	*ucUsedCwNumber = pArg->uiUsedCwNumber;

	VA_UI_SendRequestDone();
	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}

int VA_UI_GetMaturityRating(UINT32 ulScSlot, UINT8 *ucMaturityRating, UINT8 *ucMaxMaturityRating)
{
	tVA_UI_RequestStatus eStatus = eVA_UI_ERROR_STATUS;
	tVA_UI_GetMaturityRatingRequest *pArg = NULL;
//	tVA_UI_Request stRequest;

	HxLOG_Print("[Enter]\n");

	VA_ASSERT(ucMaturityRating != NULL);
	VA_ASSERT(ucMaxMaturityRating != NULL);

	pArg = (tVA_UI_GetMaturityRatingRequest *)VA_UI_SendRequest(eVA_UI_GET_MATURITY_RATING_REQUEST, ulScSlot, &eStatus, &stRequest);
	if (pArg == NULL || eStatus != eVA_UI_OK_STATUS)
	{
		VA_UI_SendRequestDone();
		HxLOG_Error("fail to send request [%x, %d]\n", pArg, eStatus);
		return VA_ERR;
	}

	*ucMaturityRating = pArg->uiMaturityRating;
	*ucMaxMaturityRating = pArg->uiMaxMaturityRating;

	VA_UI_SendRequestDone();
	HxLOG_Info("[current = %d, max = %d]\n", *ucMaturityRating, *ucMaxMaturityRating);
	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}

int VA_UI_ModifyMaturityRating(UINT32 ulScSlot, UINT8 *pPinCode, UINT8 ucMaturityRating)
{
	tVA_UI_RequestStatus eStatus = eVA_UI_ERROR_STATUS;
	tVA_UI_ModifyMaturityRatingRequest *pArg = NULL;
//	tVA_UI_Request stRequest;

	HxLOG_Print("[Enter]\n");

	VA_ASSERT(pPinCode != NULL);

	VA_memcpy(stRequest.uRequest.stModifyMaturityRatingRequest.pPinCode, pPinCode, sizeof(tVA_UI_PinCode));
	stRequest.uRequest.stModifyMaturityRatingRequest.uiNewMaturityRating = ucMaturityRating;

	pArg = (tVA_UI_ModifyMaturityRatingRequest *)VA_UI_SendRequest(eVA_UI_MODIFY_MATURITY_RATING_REQUEST, ulScSlot, &eStatus, &stRequest);

	if (eStatus != eVA_UI_OK_STATUS)
	{
		VA_UI_SendRequestDone();
		HxLOG_Error("fail to send request [%x, %d]\n", pArg, eStatus);
		switch (eStatus)
		{
			case eVA_UI_WRONG_PIN_CODE_STATUS:					return VA_ERR_UI_WRONG_PIN_CODE;
			case eVA_UI_LOCKED_PIN_CODE_STATUS:					return VA_ERR_UI_LOCKED_PIN_CODE;
			case eVA_UI_NO_PIN_CODE_STATUS:						return VA_ERR_UI_NO_PIN_CODE;
			case eVA_UI_INVALID_MATURITY_RATING_STATUS:			return VA_ERR_UI_INVALID_MATURITY_RATING;
			case eVA_UI_SMARTCARD_INSUFFICIENT_MEMORY_STATUS:		return VA_ERR_UI_SMARTCARD_INSUFFICIENT_MEMORY;
			case eVA_UI_WRONG_SLOT_STATUS:						return VA_ERR_UI_WRONG_SLOT;
			case eVA_UI_SMARTCARD_ERROR_STATUS:					return VA_ERR_UI_SMARTCARD_ERROR;
			default:													return VA_ERR;
		}
	}
	else if (pArg == NULL)
	{
		VA_UI_SendRequestDone();
		HxLOG_Error("fail to send request\n");
		return VA_ERR;
	}

	VA_UI_SendRequestDone();
	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}

int VA_UI_AcceptMaturityRating(UINT32 ulAcsId, UINT32 ulScSlot, UINT8 *pPinCode)
{
	tVA_UI_RequestStatus eStatus = eVA_UI_ERROR_STATUS;
	tVA_UI_AcceptMaturityRatingRequest *pArg = NULL;
//	tVA_UI_Request stRequest;

	HxLOG_Print("[Enter]\n");

	VA_ASSERT(pPinCode != NULL);

	VA_memcpy(stRequest.uRequest.stAcceptMaturityRatingRequest.pPinCode, pPinCode, sizeof(tVA_UI_PinCode));
	stRequest.uRequest.stAcceptMaturityRatingRequest.dwAcsId = ulAcsId;

	pArg = (tVA_UI_AcceptMaturityRatingRequest *)VA_UI_SendRequest(eVA_UI_ACCEPT_MATURITY_RATING_REQUEST, ulScSlot, &eStatus, &stRequest);

	if (eStatus != eVA_UI_OK_STATUS)
	{
		VA_UI_SendRequestDone();
		HxLOG_Error("fail to send request [%x, %d]\n", pArg, eStatus);
		switch (eStatus)
		{
			case eVA_UI_WRONG_PIN_CODE_STATUS:					return VA_ERR_UI_WRONG_PIN_CODE;
			case eVA_UI_LOCKED_PIN_CODE_STATUS:					return VA_ERR_UI_LOCKED_PIN_CODE;
			case eVA_UI_NO_PIN_CODE_STATUS:						return VA_ERR_UI_NO_PIN_CODE;
			case eVA_UI_WRONG_SLOT_STATUS:						return VA_ERR_UI_WRONG_SLOT;
			case eVA_UI_SMARTCARD_ERROR_STATUS:					return VA_ERR_UI_SMARTCARD_ERROR;
			default:													return VA_ERR;
		}
	}
	else if (pArg == NULL)
	{
		VA_UI_SendRequestDone();
		HxLOG_Error("fail to send request\n");
		return VA_ERR;
	}

	VA_UI_SendRequestDone();
	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}

// <pParam> MUST be freed in the caller function
// <pulCount> indicates the number of preselection reference which were read from the smartcard
int VA_UI_GetPreselection(UINT32 ulScSlot, UINT32 *pulCount, tVA_UI_ReferenceItem **pParam)
{
	tVA_UI_RequestStatus eStatus = eVA_UI_ERROR_STATUS;
	tVA_UI_GetPreselectionRequest *pArg = NULL;
//	tVA_UI_Request stRequest;
	tVA_UI_ReferenceItem *pReference = NULL;
	int nSize = 16;

	HxLOG_Print("[Enter]\n");

	VA_ASSERT(pulCount != NULL);
	VA_ASSERT(pParam != NULL);

	pReference = VA_MEM_Alloc(nSize * sizeof(tVA_UI_ReferenceItem));
	if (pReference == NULL)
	{
		HxLOG_Error("fail to alloc memory\n");
		return VA_ERR;
	}

	stRequest.uRequest.stGetPreselectionRequest.uiMaxItemNumber = nSize;
	stRequest.uRequest.stGetPreselectionRequest.pItemArray = pReference;

	pArg = (tVA_UI_GetPreselectionRequest *)VA_UI_SendRequest(eVA_UI_GET_PRESELECTION_REQUEST, ulScSlot, &eStatus, &stRequest);

	if (eStatus == eVA_UI_CONSULTATION_LOCKED_STATUS)
	{
		VA_UI_SendRequestDone();
		VA_MEM_Free(pReference);
		HxLOG_Info("eVA_UI_CONSULTATION_LOCKED_STATUS\n");
		return VA_ERR_UI_CONSULTATION_LOCKED;
	}
	else if (eStatus == eVA_UI_INFO_NOT_AVAILABLE_STATUS)
	{
		VA_UI_SendRequestDone();
		VA_MEM_Free(pReference);
		HxLOG_Info("eVA_UI_INFO_NOT_AVAILABLE_STATUS\n");
		return VA_ERR;
	}
	else if (eStatus == eVA_UI_ARRAY_TOO_SMALL_STATUS)
	{
		// the array is too small: reallocate the array by the specified size and send a request again
		nSize = pArg->uiItemNumber;

		VA_UI_SendRequestDone();

		VA_MEM_Free(pReference);
		pReference = VA_MEM_Alloc(nSize * sizeof(tVA_UI_ReferenceItem));
		if (pReference == NULL)
		{
			HxLOG_Error("fail to alloc memory\n");
			return VA_ERR;
		}

		stRequest.uRequest.stGetPreselectionRequest.uiMaxItemNumber = nSize;
		stRequest.uRequest.stGetPreselectionRequest.pItemArray = pReference;

		HxLOG_Info("too small array: new size [%d]\n", nSize);

		pArg = (tVA_UI_GetPreselectionRequest *)VA_UI_SendRequest(eVA_UI_GET_PRESELECTION_REQUEST, ulScSlot, &eStatus, &stRequest);
		if (pArg == NULL)
		{
			VA_UI_SendRequestDone();
			VA_MEM_Free(pReference);
			HxLOG_Error("fail to send request\n");
			return VA_ERR;
		}
	}
	else if (eStatus != eVA_UI_OK_STATUS)
	{
		VA_UI_SendRequestDone();
		VA_MEM_Free(pReference);
		HxLOG_Error("fail to send request [%x, %d]\n", pArg, eStatus);
		return VA_ERR;
	}
	else if (pArg == NULL)
	{
		VA_UI_SendRequestDone();
		VA_MEM_Free(pReference);
		HxLOG_Error("fail to send request\n");
		return VA_ERR;
	}

	*pulCount = pArg->uiItemNumber;
	*pParam = pReference;

	VA_UI_SendRequestDone();
	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}

// <pParam> MUST be freed in the caller function
// <pulCount> indicates the number of preselection reference which were read from the smartcard
int VA_UI_GetOperatorPreselection(UINT32 ulScSlot, UINT32 ulSoid, UINT32 *pulCount, tVA_UI_ReferenceItem **pParam)
{
	tVA_UI_RequestStatus eStatus = eVA_UI_ERROR_STATUS;
	tVA_UI_GetOperatorPreselectionRequest *pArg = NULL;
//	tVA_UI_Request stRequest;
	tVA_UI_ReferenceItem *pReference = NULL;
	int nSize = 16;

	HxLOG_Print("[Enter]\n");

	VA_ASSERT(pulCount != NULL);
	VA_ASSERT(pReference != NULL);
	VA_ASSERT(sizeof(UINT32) == sizeof(tVA_UI_Soid));

	pReference = VA_MEM_Alloc(nSize * sizeof(tVA_UI_ReferenceItem));
	if (pReference == NULL)
	{
		HxLOG_Error("fail to alloc memory\n");
		return VA_ERR;
	}

	stRequest.uRequest.stGetOperatorPreselectionRequest.uiSoid = ulSoid;
	stRequest.uRequest.stGetOperatorPreselectionRequest.uiMaxItemNumber = nSize;
	stRequest.uRequest.stGetOperatorPreselectionRequest.pItemArray = pReference;

	pArg = (tVA_UI_GetOperatorPreselectionRequest *)VA_UI_SendRequest(eVA_UI_GET_OPERATOR_PRESELECTION_REQUEST, ulScSlot, &eStatus, &stRequest);

	if (eStatus == eVA_UI_CONSULTATION_LOCKED_STATUS)
	{
		VA_UI_SendRequestDone();
		VA_MEM_Free(pReference);
		HxLOG_Info("eVA_UI_CONSULTATION_LOCKED_STATUS\n");
		return VA_ERR_UI_CONSULTATION_LOCKED;
	}
	else if (eStatus == eVA_UI_INFO_NOT_AVAILABLE_STATUS)
	{
		VA_UI_SendRequestDone();
		VA_MEM_Free(pReference);
		HxLOG_Info("eVA_UI_INFO_NOT_AVAILABLE_STATUS\n");
		return VA_ERR;
	}
	else if (eStatus == eVA_UI_ARRAY_TOO_SMALL_STATUS)
	{
		// the array is too small: reallocate the array by the specified size and send a request again
		nSize = pArg->uiItemNumber;

		VA_UI_SendRequestDone();

		VA_MEM_Free(pReference);
		pReference = VA_MEM_Alloc(nSize * sizeof(tVA_UI_ReferenceItem));
		if (pReference == NULL)
		{
			HxLOG_Error("fail to alloc memory\n");
			return VA_ERR;
		}

		stRequest.uRequest.stGetOperatorPreselectionRequest.uiMaxItemNumber = nSize;
		stRequest.uRequest.stGetOperatorPreselectionRequest.pItemArray = pReference;

		HxLOG_Info("too small array: new size [%d]\n", nSize);

		pArg = (tVA_UI_GetOperatorPreselectionRequest *)VA_UI_SendRequest(eVA_UI_GET_OPERATOR_PRESELECTION_REQUEST, ulScSlot, &eStatus, &stRequest);
		if (pArg == NULL)
		{
			VA_UI_SendRequestDone();
			VA_MEM_Free(pReference);
			HxLOG_Error("fail to send request\n");
			return VA_ERR;
		}
	}
	else if (eStatus != eVA_UI_OK_STATUS)
	{
		VA_UI_SendRequestDone();
		VA_MEM_Free(pReference);
		HxLOG_Error("fail to send request [%x, %d]\n", pArg, eStatus);
		return VA_ERR;
	}
	else if (pArg == NULL)
	{
		VA_UI_SendRequestDone();
		VA_MEM_Free(pReference);
		HxLOG_Error("fail to send request\n");
		return VA_ERR;
	}

	*pulCount = pArg->uiItemNumber;
	*pParam = pReference;

	VA_UI_SendRequestDone();
	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}

int VA_UI_ModifyPreselection(UINT32 ulScSlot, UINT32 ulSoid, UINT8 *pPinCode, UINT32 ulAction, tVA_UI_ReferenceType eType, tVA_UI_ReferenceInfo *pInfo)
{
	tVA_UI_RequestStatus eStatus = eVA_UI_ERROR_STATUS;
	tVA_UI_ModifyPreselectionRequest *pArg = NULL;
//	tVA_UI_Request stRequest;

	HxLOG_Print("[Enter]\n");

	VA_ASSERT(pPinCode != NULL);
	VA_ASSERT(pInfo != NULL);
	VA_ASSERT(sizeof(UINT32) == sizeof(tVA_UI_Soid));

	VA_memcpy(stRequest.uRequest.stModifyPreselectionRequest.pPinCode, pPinCode, sizeof(tVA_UI_PinCode));
	VA_memcpy(&stRequest.uRequest.stModifyPreselectionRequest.uInfo, pInfo, sizeof(tVA_UI_ReferenceInfo));
	stRequest.uRequest.stModifyPreselectionRequest.uiSoid = ulSoid;
	stRequest.uRequest.stModifyPreselectionRequest.eAction = ulAction;
	stRequest.uRequest.stModifyPreselectionRequest.eType = eType;

	pArg = (tVA_UI_ModifyPreselectionRequest *)VA_UI_SendRequest(eVA_UI_MODIFY_PRESELECTION_REQUEST, ulScSlot, &eStatus, &stRequest);

	if (eStatus != eVA_UI_OK_STATUS)
	{
		VA_UI_SendRequestDone();
		HxLOG_Error("fail to send request [%x, %d]\n", pArg, eStatus);
		switch (eStatus)
		{
			case eVA_UI_WRONG_PIN_CODE_STATUS:									return VA_ERR_UI_WRONG_PIN_CODE;
			case eVA_UI_LOCKED_PIN_CODE_STATUS:									return VA_ERR_UI_LOCKED_PIN_CODE;
			case eVA_UI_NO_PIN_CODE_STATUS:										return VA_ERR_UI_NO_PIN_CODE;
			case eVA_UI_OPERATION_NOT_ACCEPTED_STATUS:							return VA_ERR_UI_OPERATION_NOT_ACCEPTED;
			case eVA_UI_PRESELECTION_AREA_FULL_STATUS:								return VA_ERR_UI_PRESELECTION_AREA_FULL;
			case eVA_UI_REFERENCE_ALREADY_STORED_IN_PRESELECTION_AREA_STATUS:	return VA_ERR_UI_REFERENCE_ALREADY_STORED_IN_PRESELECTION_AREA;
			case eVA_UI_REFERENCE_NOT_FOUND_IN_PRESELECTION_AREA_STATUS:			return VA_ERR_UI_REFERENCE_NOT_FOUND_IN_PRESELECTION_AREA;
			case eVA_UI_PRESELECTION_MODE_NOT_ACCEPTED_STATUS:					return VA_ERR_UI_PRESELECTION_MODE_NOT_ACCEPTED;
			case eVA_UI_WRONG_SLOT_STATUS:										return VA_ERR_UI_WRONG_SLOT;
			case eVA_UI_SMARTCARD_ERROR_STATUS:									return VA_ERR_UI_SMARTCARD_ERROR;
			default:																	return VA_ERR;
		}
	}
	else if (pArg == NULL)
	{
		VA_UI_SendRequestDone();
		HxLOG_Error("fail to send request\n");
		return VA_ERR;
	}

	VA_UI_SendRequestDone();
	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}

// the size of the array pointed by <pGca> must be larger than <kVA_UI_GCA_SIZE>
int VA_UI_GetGeographicalCodeAddress(UINT32 ulScSlot, UINT32 ulSoid, UINT8 *pGca)
{
	tVA_UI_RequestStatus eStatus = eVA_UI_ERROR_STATUS;
	tVA_UI_GetGeographicalCodeAddressRequest *pArg = NULL;
//	tVA_UI_Request stRequest;

	HxLOG_Print("[Enter]\n");

	VA_ASSERT(pGca != NULL);
	VA_ASSERT(sizeof(UINT32) == sizeof(tVA_UI_Soid));

	stRequest.uRequest.stGetGeographicalCodeAddressRequest.uiSoid = ulSoid;

	pArg = (tVA_UI_GetGeographicalCodeAddressRequest *)VA_UI_SendRequest(eVA_UI_GET_GEOGRAPHICAL_CODE_ADDRESS_REQUEST, ulScSlot, &eStatus, &stRequest);
	if (pArg == NULL || eStatus != eVA_UI_OK_STATUS)
	{
		VA_UI_SendRequestDone();
		HxLOG_Error("fail to send request [%x, %d]\n", pArg, eStatus);
		return VA_ERR;
	}

	VA_memcpy(pGca, stRequest.uRequest.stGetGeographicalCodeAddressRequest.pGca, sizeof(tVA_UI_Gca));

	VA_UI_SendRequestDone();
	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}

// <pFac> MUST be freed in the caller function
// <pulCount> indicates the number of FAC
int VA_UI_GetFacData(UINT32 ulScSlot, UINT32 ulSoid, UINT8 ucNP1, UINT8 ucNP2, UINT32 *pulCount, UINT8 *pFac)
{
	tVA_UI_RequestStatus eStatus = eVA_UI_ERROR_STATUS;
	tVA_UI_GetFacDataRequest *pArg = NULL;
//	tVA_UI_Request stRequest;
	UINT32 ulSize = 1024;

	HxLOG_Print("[Enter]\n");

	VA_ASSERT(pulCount != NULL);
	VA_ASSERT(pFac != NULL);
	VA_ASSERT(sizeof(UINT32) == sizeof(tVA_UI_Soid));

	pFac = VA_MEM_Alloc(ulSize);
	if (pFac == NULL)
	{
		HxLOG_Error("fail to alloc memory\n");
		return VA_ERR;
	}

	stRequest.uRequest.stGetFacDataRequest.uiSoid = ulSoid;
	stRequest.uRequest.stGetFacDataRequest.uiNP1= ucNP1;
	stRequest.uRequest.stGetFacDataRequest.uiNP2= ucNP2;
	stRequest.uRequest.stGetFacDataRequest.uiMaxByteNumber = ulSize;
	stRequest.uRequest.stGetFacDataRequest.pFacDataBuffer = pFac;

	pArg = (tVA_UI_GetFacDataRequest *)VA_UI_SendRequest(eVA_UI_GET_FAC_DATA_REQUEST, ulScSlot, &eStatus, &stRequest);

	if (eStatus == eVA_UI_ARRAY_TOO_SMALL_STATUS)
	{
		// the array is too small: reallocate the array by the specified size and send a request again
		ulSize= pArg->uiByteNumber;

		VA_UI_SendRequestDone();

		VA_MEM_Free(pFac);
		pFac = VA_MEM_Alloc(ulSize);
		if (pFac == NULL)
		{
			HxLOG_Error("fail to alloc memory\n");
			return VA_ERR;
		}

		stRequest.uRequest.stGetFacDataRequest.uiMaxByteNumber = ulSize;
		stRequest.uRequest.stGetFacDataRequest.pFacDataBuffer = pFac;

		HxLOG_Info("too small array: new size [%d]\n", ulSize);

		pArg = (tVA_UI_GetFacDataRequest *)VA_UI_SendRequest(eVA_UI_GET_FAC_DATA_REQUEST, ulScSlot, &eStatus, &stRequest);
		if (pArg == NULL)
		{
			VA_UI_SendRequestDone();
			VA_MEM_Free(pFac);
			HxLOG_Error("fail to send request\n");
			return VA_ERR;
		}
	}
	else if (eStatus != eVA_UI_OK_STATUS)
	{
		VA_UI_SendRequestDone();
		VA_MEM_Free(pFac);
		HxLOG_Error("fail to send request [%x, %d]\n", pArg, eStatus);
		return VA_ERR;
	}
	else if (pArg == NULL)
	{
		VA_UI_SendRequestDone();
		VA_MEM_Free(pFac);
		HxLOG_Error("fail to send request\n");
		return VA_ERR;
	}

	*pulCount = pArg->uiByteNumber;

	VA_UI_SendRequestDone();
	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}

// <pFac> MUST be freed in the caller function
// <pulCount> indicates the number of FAC
int VA_UI_GetFacList(UINT32 ulScSlot, UINT32 ulSoid, UINT32 *pulCount, tVA_UI_FacItem *pFac)
{
	tVA_UI_RequestStatus eStatus = eVA_UI_ERROR_STATUS;
	tVA_UI_GetFacListRequest *pArg = NULL;
//	tVA_UI_Request stRequest;
	int nSize = 16;

	HxLOG_Print("[Enter]\n");

	VA_ASSERT(pulCount != NULL);
	VA_ASSERT(pFac != NULL);
	VA_ASSERT(sizeof(UINT32) == sizeof(tVA_UI_Soid));

	pFac = VA_MEM_Alloc(nSize * sizeof(tVA_UI_FacItem));
	if (pFac == NULL)
	{
		HxLOG_Error("fail to alloc memory\n");
		return VA_ERR;
	}

	stRequest.uRequest.stGetFacListRequest.uiSoid = ulSoid;
	stRequest.uRequest.stGetFacListRequest.uiMaxItemNumber = nSize;
	stRequest.uRequest.stGetFacListRequest.pItemArray = pFac;

	pArg = (tVA_UI_GetFacListRequest *)VA_UI_SendRequest(eVA_UI_GET_FAC_LIST_REQUEST, ulScSlot, &eStatus, &stRequest);

	if (eStatus == eVA_UI_ARRAY_TOO_SMALL_STATUS)
	{
		// the array is too small: reallocate the array by the specified size and send a request again
		nSize= pArg->uiItemNumber;

		VA_UI_SendRequestDone();

		VA_MEM_Free(pFac);
		pFac = VA_MEM_Alloc(nSize * sizeof(tVA_UI_FacItem));
		if (pFac == NULL)
		{
			HxLOG_Error("fail to alloc memory\n");
			return VA_ERR;
		}

		stRequest.uRequest.stGetFacListRequest.uiMaxItemNumber = nSize;
		stRequest.uRequest.stGetFacListRequest.pItemArray = pFac;

		HxLOG_Info("too small array: new size [%d]\n", nSize);

		pArg = (tVA_UI_GetFacListRequest *)VA_UI_SendRequest(eVA_UI_GET_FAC_LIST_REQUEST, ulScSlot, &eStatus, &stRequest);
		if (pArg == NULL)
		{
			VA_UI_SendRequestDone();
			VA_MEM_Free(pFac);
			HxLOG_Error("fail to send request\n");
			return VA_ERR;
		}
	}
	else if (eStatus != eVA_UI_OK_STATUS)
	{
		VA_UI_SendRequestDone();
		VA_MEM_Free(pFac);
		HxLOG_Error("fail to send request [%x, %d]\n", pArg, eStatus);
		return VA_ERR;
	}
	else if (pArg == NULL)
	{
		VA_UI_SendRequestDone();
		VA_MEM_Free(pFac);
		HxLOG_Error("fail to send request\n");
		return VA_ERR;
	}

	*pulCount = pArg->uiItemNumber;

	VA_UI_SendRequestDone();
	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}

// pExmBuffer should contain the section payload (data after section length)
// <pReply> MUST be freed in the caller function
// <pulCount> indicates the number of bytes of pReply
int VA_UI_SubmitExm(UINT32 ulScSlot, UINT32 ulExmType, UINT32 ulSize, UINT8 *pExm, UINT32 *pulStatus, UINT32 *pulCount, UINT8 *pReply)
{
	tVA_UI_RequestStatus eStatus = eVA_UI_ERROR_STATUS;
	tVA_UI_SubmitExmRequest *pArg = NULL;
//	tVA_UI_Request stRequest;
	int nSize = 1024;

	HxLOG_Print("[Enter]\n");

	VA_ASSERT(pExm != NULL);
	VA_ASSERT(pulStatus != NULL);
	VA_ASSERT(pulCount != NULL);
	VA_ASSERT(pReply != NULL);

	pReply = VA_MEM_Alloc(nSize);
	if (pReply == NULL)
	{
		HxLOG_Error("fail to alloc memory\n");
		return VA_ERR;
	}

	stRequest.uRequest.stSubmitExmRequest.eExmType = ulExmType;
	stRequest.uRequest.stSubmitExmRequest.uiExmSize = ulSize;
	stRequest.uRequest.stSubmitExmRequest.pExmBuffer = pExm;
	stRequest.uRequest.stSubmitExmRequest.uiMaxReplyBufferSize = nSize;
	stRequest.uRequest.stSubmitExmRequest.pReplyBuffer = pReply;

	pArg = (tVA_UI_SubmitExmRequest *)VA_UI_SendRequest(eVA_UI_SUBMIT_EXM_REQUEST, ulScSlot, &eStatus, &stRequest);

	if (eStatus == eVA_UI_ARRAY_TOO_SMALL_STATUS)
	{
		// the array is too small: reallocate the array by the specified size and send a request again
		nSize= pArg->uiReplyBufferSize;

		VA_UI_SendRequestDone();

		VA_MEM_Free(pReply);
		pReply = VA_MEM_Alloc(nSize);
		if (pReply == NULL)
		{
			HxLOG_Error("fail to alloc memory\n");
			return VA_ERR;
		}

		stRequest.uRequest.stSubmitExmRequest.uiMaxReplyBufferSize= nSize;
		stRequest.uRequest.stSubmitExmRequest.pReplyBuffer = pReply;

		HxLOG_Info("too small array: new size [%d]\n", nSize);

		pArg = (tVA_UI_SubmitExmRequest *)VA_UI_SendRequest(eVA_UI_SUBMIT_EXM_REQUEST, ulScSlot, &eStatus, &stRequest);
		if (pArg == NULL)
		{
			VA_UI_SendRequestDone();
			VA_MEM_Free(pReply);
			HxLOG_Error("fail to send request\n");
			return VA_ERR;
		}
	}
	else if (eStatus != eVA_UI_OK_STATUS)
	{
		VA_UI_SendRequestDone();
		VA_MEM_Free(pReply);
		HxLOG_Error("fail to send request [%x, %d]\n", pArg, eStatus);
		return VA_ERR;
	}
	else if (pArg == NULL)
	{
		VA_UI_SendRequestDone();
		VA_MEM_Free(pReply);
		HxLOG_Error("fail to send request\n");
		return VA_ERR;
	}

	*pulCount = pArg->uiReplyBufferSize;
	*pulStatus = pArg->eExmSubmissionStatus;

	VA_UI_SendRequestDone();
	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}

// the size of the array pointed by <pMsg> must be larger than <kVA_UI_MAX_OPERATOR_ITEM_SIZE>
// if *pulSoid == kVA_UI_NOT_USED means that a message from any operator can be retrieved.
int VA_UI_GetOperatorMessage(UINT32 ulScSlot, UINT32 *pulSoid, UINT32 ulPriority, UINT32 *pulEncode, UINT32 *pulCount, UINT8 *pMsg)
{
	tVA_UI_RequestStatus eStatus = eVA_UI_ERROR_STATUS;
	tVA_UI_GetOperatorMessageRequest *pArg = NULL;
//	tVA_UI_Request stRequest;

	HxLOG_Print("[Enter]\n");

	VA_ASSERT(pulSoid != NULL);
	VA_ASSERT(pulEncode != NULL);
	VA_ASSERT(pulCount != NULL);
	VA_ASSERT(pMsg != NULL);
	VA_ASSERT(sizeof(UINT32) == sizeof(tVA_UI_Soid));

	stRequest.uRequest.stGetOperatorMessageRequest.uiSoid = *pulSoid;
	stRequest.uRequest.stGetOperatorMessageRequest.ePriority = ulPriority;

	pArg = (tVA_UI_GetOperatorMessageRequest *)VA_UI_SendRequest(eVA_UI_GET_OPERATOR_MESSAGE_REQUEST, ulScSlot, &eStatus, &stRequest);
	if (pArg == NULL || eStatus != eVA_UI_OK_STATUS)
	{
		VA_UI_SendRequestDone();
		HxLOG_Error("fail to send request [%x, %d]\n", pArg, eStatus);
		return VA_ERR;
	}

	*pulSoid = pArg->uiSoid;
	*pulEncode = pArg->eEncoding;
	*pulCount = pArg->uiMessageSize;
	VA_memcpy(pMsg, pArg->pOperatorMessageBuffer, kVA_UI_MAX_OPERATOR_ITEM_SIZE);

	VA_UI_SendRequestDone();
	HxLOG_Info("[SOID = %x, Encode = %d, MessageSize = %d]\n", *pulSoid, *pulEncode, *pulCount);
	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}

// the size of the array pointed by <pMsg> must be larger than <kVA_UI_MAX_OPERATOR_ITEM_SIZE>
// if *pulSoid == kVA_UI_NOT_USED means that a message from any operator can be retrieved.
int VA_UI_GetOperatorData(UINT32 ulScSlot, UINT32 *pulSoid, UINT32 ulPriority, UINT32 *pulCount, UINT8 *pData)
{
	tVA_UI_RequestStatus eStatus = eVA_UI_ERROR_STATUS;
	tVA_UI_GetOperatorDataRequest *pArg = NULL;
//	tVA_UI_Request stRequest;

	HxLOG_Print("[Enter]\n");

	VA_ASSERT(pulSoid != NULL);
	VA_ASSERT(pulCount != NULL);
	VA_ASSERT(pData != NULL);
	VA_ASSERT(sizeof(UINT32) == sizeof(tVA_UI_Soid));

	stRequest.uRequest.stGetOperatorDataRequest.uiSoid = *pulSoid;
	stRequest.uRequest.stGetOperatorDataRequest.ePriority = ulPriority;

	pArg = (tVA_UI_GetOperatorDataRequest *)VA_UI_SendRequest(eVA_UI_GET_OPERATOR_DATA_REQUEST, ulScSlot, &eStatus, &stRequest);
	if (pArg == NULL || eStatus != eVA_UI_OK_STATUS)
	{
		VA_UI_SendRequestDone();
		HxLOG_Error("fail to send request [%x, %d]\n", pArg, eStatus);
		return VA_ERR;
	}

	*pulSoid = pArg->uiSoid;
	*pulCount = pArg->uiBufferSize;
	VA_memcpy(pData, pArg->pOperatorDataBuffer, kVA_UI_MAX_OPERATOR_ITEM_SIZE);

	VA_UI_SendRequestDone();
	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}

// the size of the array pointed by <pCamlock> must be larger than <kVA_UI_MAX_CAMLOCK_SIZE + 1>
int VA_UI_GetCamlockInfo(UINT32 *pulState, UINT8 *pCamlock)
{
	tVA_UI_RequestStatus eStatus = eVA_UI_ERROR_STATUS;
	tVA_UI_GetCamlockInfoRequest *pArg = NULL;
//	tVA_UI_Request stRequest;

	HxLOG_Print("[Enter]\n");

//	VA_ASSERT(pulState != NULL);
	VA_ASSERT(pCamlock != NULL);

	pArg = (tVA_UI_GetCamlockInfoRequest *)VA_UI_SendRequest(eVA_UI_GET_CAMLOCK_INFO_REQUEST, kVA_UI_NOT_USED, &eStatus, &stRequest);
	if (pArg == NULL || eStatus != eVA_UI_OK_STATUS)
	{
		VA_UI_SendRequestDone();
		HxLOG_Error("fail to send request [%x, %d]\n", pArg, eStatus);
		return VA_ERR;
	}

	if (pulState)
		*pulState = pArg->eCamlockDynamicState;

	VA_memcpy(pCamlock, pArg->pCamlock, kVA_UI_MAX_CAMLOCK_SIZE + 1);

	VA_UI_SendRequestDone();
	HxLOG_Info("[%s]\n", pCamlock);
	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}

int VA_UI_GetPairingMode(UINT32 *pulMode)
{
	tVA_UI_RequestStatus eStatus = eVA_UI_ERROR_STATUS;
	tVA_UI_GetPairingModeRequest *pArg = NULL;
//	tVA_UI_Request stRequest;

	HxLOG_Print("[Enter]\n");

	VA_ASSERT(pulMode != NULL);

	pArg = (tVA_UI_GetPairingModeRequest *)VA_UI_SendRequest(eVA_UI_GET_PAIRING_MODE_REQUEST, kVA_UI_NOT_USED, &eStatus, &stRequest);
	if (pArg == NULL || eStatus != eVA_UI_OK_STATUS)
	{
		VA_UI_SendRequestDone();
		HxLOG_Error("fail to send request [%x, %d]\n", pArg, eStatus);
		return VA_ERR;
	}

	*pulMode = pArg->ePairingMode;

	VA_UI_SendRequestDone();
	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}

int VA_UI_GetPairingSmartcard(UINT32 *pulMaxNum, UINT32 *pulNum)
{
	tVA_UI_RequestStatus eStatus = eVA_UI_ERROR_STATUS;
	tVA_UI_GetPairingSmartcardRequest *pArg = NULL;
//	tVA_UI_Request stRequest;

	HxLOG_Print("[Enter]\n");

	VA_ASSERT(pulMaxNum != NULL);
	VA_ASSERT(pulNum != NULL);

	pArg = (tVA_UI_GetPairingSmartcardRequest *)VA_UI_SendRequest(eVA_UI_GET_PAIRING_SMARTCARD_REQUEST, kVA_UI_NOT_USED, &eStatus, &stRequest);
	if (pArg == NULL || eStatus != eVA_UI_OK_STATUS)
	{
		VA_UI_SendRequestDone();
		HxLOG_Error("fail to send request [%x, %d]\n", pArg, eStatus);
		return VA_ERR;
	}

	*pulMaxNum = pArg->uiMaxNumberOfSmartcard;
	*pulNum = pArg->uiNumberOfSmartcard;
	HxLOG_Info("uiMaxNumber [%d], uiNumber [%d]\n", pArg->uiMaxNumberOfSmartcard, pArg->uiNumberOfSmartcard);

	VA_UI_SendRequestDone();
	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}

// <return value> MUST be freed in the caller function
// <pulUaNum> indicates the number of UA
 tVA_UI_Ua *VA_UI_GetPairingUaList(UINT32 *pulUaNum)
{
	tVA_UI_RequestStatus eStatus = eVA_UI_ERROR_STATUS;
	tVA_UI_GetPairingUaListRequest *pArg = NULL;
//	tVA_UI_Request stRequest;
	tVA_UI_Ua *pUa = NULL;

	HxLOG_Print("[Enter]\n");

	VA_ASSERT(pulUaNum != NULL);

	pArg = (tVA_UI_GetPairingUaListRequest *)VA_UI_SendRequest(eVA_UI_GET_PAIRING_UA_LIST_REQUEST, kVA_UI_NOT_USED, &eStatus, &stRequest);
	if (pArg == NULL || eStatus != eVA_UI_OK_STATUS)
	{
		VA_UI_SendRequestDone();
		HxLOG_Error("fail to send request [%x, %d]\n", pArg, eStatus);
		return NULL;
	}

	*pulUaNum = pArg->uiUaNumber;
	HxLOG_Info("uiUaNumber [%d]\n", pArg->uiUaNumber);

	if (pArg->uiUaNumber)
	{
		pUa = VA_MEM_Alloc(pArg->uiUaNumber * sizeof(tVA_UI_Ua));
		if (pUa == NULL)
		{
			VA_UI_SendRequestDone();
			HxLOG_Error("fail to alloc memory\n");
			return NULL;
		}

		VA_memcpy(pUa, pArg->pUaArray, pArg->uiUaNumber * sizeof(tVA_UI_Ua));
	}

	VA_UI_SendRequestDone();
	HxLOG_Print("[Exit]\n");
	return pUa;
}

// the size of the array pointed by <pStbId> must be larger than <kVA_UI_STB_ID_SIZE>
int VA_UI_GetStbId(UINT8 *pStbId, UINT32 *pbIsDateValid, tVA_UI_DateEx *pDate)
{
	tVA_UI_RequestStatus eStatus = eVA_UI_ERROR_STATUS;
	tVA_UI_GetStbIdRequest *pArg = NULL;
//	tVA_UI_Request stRequest;

	HxLOG_Print("[Enter]\n");

	VA_ASSERT(pStbId != NULL);
//	VA_ASSERT(pbIsDateValid != NULL);
//	VA_ASSERT(pDate != NULL);

	pArg = (tVA_UI_GetStbIdRequest *)VA_UI_SendRequest(eVA_UI_GET_STB_ID_REQUEST, kVA_UI_NOT_USED, &eStatus, &stRequest);
	if (pArg == NULL || eStatus != eVA_UI_OK_STATUS)
	{
		VA_UI_SendRequestDone();
		HxLOG_Error("fail to send request [%x, %d]\n", pArg, eStatus);
		return VA_ERR;
	}

	if (pbIsDateValid)
		*pbIsDateValid = pArg->bIsDateValid;
	if (pDate)
		VA_memcpy(pDate, &pArg->stDate, sizeof(tVA_UI_DateEx));
	VA_memcpy(pStbId, pArg->pStbId, sizeof(tVA_UI_StbId));

	VA_UI_SendRequestDone();
	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}

int VA_UI_GetTraceOutput(UINT32 *pulType, UINT32 *pulMode, UINT32 *pulHeaderType, UINT32 *pulLineWidth, tVA_UI_TraceOutputParameters *pParam)
{
	tVA_UI_RequestStatus eStatus = eVA_UI_ERROR_STATUS;
	tVA_UI_GetTraceOutputRequest *pArg = NULL;
//	tVA_UI_Request stRequest;

	HxLOG_Print("[Enter]\n");

	VA_ASSERT(pulType != NULL);
	VA_ASSERT(pulMode != NULL);
	VA_ASSERT(pulHeaderType != NULL);
	VA_ASSERT(pulLineWidth != NULL);
	VA_ASSERT(pParam != NULL);

	pArg = (tVA_UI_GetTraceOutputRequest *)VA_UI_SendRequest(eVA_UI_GET_TRACE_OUTPUT_REQUEST, kVA_UI_NOT_USED, &eStatus, &stRequest);
	if (pArg == NULL || eStatus != eVA_UI_OK_STATUS)
	{
		VA_UI_SendRequestDone();
		HxLOG_Error("fail to send request [%x, %d]\n", pArg, eStatus);
		return VA_ERR;
	}

	*pulType = pArg->eOutputType;
	*pulMode = pArg->eTraceMode;
	*pulHeaderType = pArg->eHeaderType;
	*pulLineWidth = pArg->uiMaxLineWidth;
	VA_memcpy(pParam, &pArg->uTraceOutputParam, sizeof(tVA_UI_TraceOutputParameters));

	VA_UI_SendRequestDone();
	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}

int VA_UI_SetTraceOutput(UINT32 ulType, UINT32 ulMode, UINT32 ulHeaderType, UINT32 ulLineWidth, tVA_UI_TraceOutputParameters *pParam)
{
	tVA_UI_RequestStatus eStatus = eVA_UI_ERROR_STATUS;
	tVA_UI_SetTraceOutputRequest *pArg = NULL;
//	tVA_UI_Request stRequest;

	HxLOG_Print("[Enter]\n");

	VA_ASSERT(pParam != NULL);

	stRequest.uRequest.stSetTraceOutputRequest.eOutputType = ulType;
	stRequest.uRequest.stSetTraceOutputRequest.eTraceMode = ulMode;
	stRequest.uRequest.stSetTraceOutputRequest.eHeaderType = ulHeaderType;
	stRequest.uRequest.stSetTraceOutputRequest.uiMaxLineWidth = ulLineWidth;
	VA_memcpy(&stRequest.uRequest.stSetTraceOutputRequest.uTraceOutputParam, pParam, sizeof(tVA_UI_TraceOutputParameters));

	pArg = (tVA_UI_SetTraceOutputRequest *)VA_UI_SendRequest(eVA_UI_SET_TRACE_OUTPUT_REQUEST, kVA_UI_NOT_USED, &eStatus, &stRequest);
	if (pArg == NULL || eStatus != eVA_UI_OK_STATUS)
	{
		VA_UI_SendRequestDone();
		HxLOG_Error("fail to send request [%x, %d]\n", pArg, eStatus);
		return VA_ERR;
	}

	VA_UI_SendRequestDone();
	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}

// <pTrace> MUST be freed in the caller function
// <pulCount> indicates the number of trace module
int VA_UI_GetTraceList(UINT32 *pulCount, tVA_UI_TraceModule **pTrace)
{
	tVA_UI_RequestStatus eStatus = eVA_UI_ERROR_STATUS;
	tVA_UI_GetTraceListRequest *pArg = NULL;
//	tVA_UI_Request stRequest;
	tVA_UI_TraceModule *pTraceList = NULL;
	int nSize = 16;

	HxLOG_Print("[Enter]\n");

	VA_ASSERT(pulCount != NULL);
	VA_ASSERT(pTrace != NULL);

	pTraceList = VA_MEM_Alloc(nSize * sizeof(tVA_UI_TraceModule));
	if (pTraceList == NULL)
	{
		HxLOG_Error("fail to alloc memory\n");
		return VA_ERR;
	}

	stRequest.uRequest.stGetTraceListRequest.uiMaxItemNumber= nSize;
	stRequest.uRequest.stGetTraceListRequest.pItemArray = pTraceList;

	pArg = (tVA_UI_GetTraceListRequest *)VA_UI_SendRequest(eVA_UI_GET_TRACE_LIST_REQUEST, kVA_UI_NOT_USED, &eStatus, &stRequest);

	if (eStatus == eVA_UI_ARRAY_TOO_SMALL_STATUS)
	{
		// the array is too small: reallocate the array by the specified size and send a request again
		nSize= pArg->uiItemNumber;

		VA_UI_SendRequestDone();

		VA_MEM_Free(pTraceList);
		pTraceList = VA_MEM_Alloc(nSize * sizeof(tVA_UI_TraceModule));
		if (pTraceList == NULL)
		{
			HxLOG_Error("fail to alloc memory\n");
			return VA_ERR;
		}

		stRequest.uRequest.stGetTraceListRequest.uiMaxItemNumber = nSize;
		stRequest.uRequest.stGetTraceListRequest.pItemArray = pTraceList;

		HxLOG_Info("too small array: new size [%d]\n", nSize);

		pArg = (tVA_UI_GetTraceListRequest *)VA_UI_SendRequest(eVA_UI_GET_TRACE_LIST_REQUEST, kVA_UI_NOT_USED, &eStatus, &stRequest);
		if (pArg == NULL)
		{
			VA_UI_SendRequestDone();
			VA_MEM_Free(pTraceList);
			HxLOG_Error("fail to send request\n");
			return VA_ERR;
		}
	}
	else if (eStatus != eVA_UI_OK_STATUS)
	{
		VA_UI_SendRequestDone();
		VA_MEM_Free(pTraceList);
		HxLOG_Error("fail to send request [%x, %d]\n", pArg, eStatus);
		return VA_ERR;
	}
	else if (pArg == NULL)
	{
		VA_UI_SendRequestDone();
		VA_MEM_Free(pTraceList);
		HxLOG_Error("fail to send request\n");
		return VA_ERR;
	}

	*pulCount = pArg->uiItemNumber;
	*pTrace = pTraceList;

	VA_UI_SendRequestDone();
	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}

int VA_UI_SetTraceLevel(tVA_UI_TraceModule *pTrace)
{
	tVA_UI_RequestStatus eStatus = eVA_UI_ERROR_STATUS;
	tVA_UI_SetTraceLevelRequest *pArg = NULL;
//	tVA_UI_Request stRequest;

	HxLOG_Print("[Enter]\n");

	VA_ASSERT(pTrace != NULL);

	VA_memcpy(&stRequest.uRequest.stSetTraceLevelRequest.stNewLevel, pTrace, sizeof(tVA_UI_TraceModule));

	pArg = (tVA_UI_SetTraceLevelRequest *)VA_UI_SendRequest(eVA_UI_SET_TRACE_LEVEL_REQUEST, kVA_UI_NOT_USED, &eStatus, &stRequest);
	if (pArg == NULL || eStatus != eVA_UI_OK_STATUS)
	{
		VA_UI_SendRequestDone();
		HxLOG_Error("fail to send request [%x, %d]\n", pArg, eStatus);
		return VA_ERR;
	}

	VA_UI_SendRequestDone();
	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}

int VA_UI_FlushTrace()
{
	tVA_UI_RequestStatus eStatus = eVA_UI_ERROR_STATUS;
//	tVA_UI_Request stRequest;

	HxLOG_Print("[Enter]\n");

	VA_UI_SendRequest(eVA_UI_FLUSH_TRACE_REQUEST, kVA_UI_NOT_USED, &eStatus, &stRequest);
	if (eStatus != eVA_UI_OK_STATUS)
	{
		VA_UI_SendRequestDone();
		HxLOG_Error("fail to send request [%d]\n", eStatus);
		return VA_ERR;
	}

	VA_UI_SendRequestDone();
	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}

#ifdef CONFIG_MW_CAS_VIACCESS_IPPV
// get all of the IPPV-P / IPPV-T list
// <ulIppvType> = IPPV_P or IPPV_T
int VA_UI_GetIPPVListAll(UINT32 ulScSlot, UINT32 ulSoid, UINT32 ulIppvType, UINT32 *pulCount, tVA_UI_IppvItem **pIppvItem)
{
	VA_ASSERT(pIppvItem != NULL);
	VA_ASSERT(pulCount != NULL);
	VA_ASSERT(sizeof(UINT32) == sizeof(tVA_UI_Soid));

	return VA_UI_GetIPPVList(ulScSlot, ulSoid, ulIppvType, 0, 0xFFFFFF, pulCount, pIppvItem);
}

// <ulIppvType> = IPPV_P or IPPV_T
// <pIppvItem> MUST be freed in the caller function
// <pulCount> indicates the number of items which were read from the smartcard
int VA_UI_GetIPPVList(UINT32 ulScSlot, UINT32 ulSoid, UINT32 ulIppvType, UINT32 ulMin, UINT32 ulMax, UINT32 *pulCount, tVA_UI_IppvItem **pIppvItem)
{
	tVA_UI_RequestStatus eStatus = eVA_UI_ERROR_STATUS;
	tVA_UI_GetIppvPListRequest *pArg = NULL;
//	tVA_UI_Request stRequest;
	UINT32 ulRequestType;
	int nSize = 16;
	tVA_UI_IppvItem *pIppvItemList;

	HxLOG_Print("[ENTER(IPPV%c)]\n", ulIppvType == IPPV_P?'P':'T');

	if (ulIppvType == IPPV_P)
		ulRequestType = eVA_UI_GET_IPPVP_LIST_REQUEST;
	else
		ulRequestType = eVA_UI_GET_IPPVT_LIST_REQUEST;

	VA_ASSERT(pIppvItem != NULL);
	VA_ASSERT(pulCount != NULL);
	VA_ASSERT(sizeof(UINT32) == sizeof(tVA_UI_Soid));

	pIppvItemList = VA_MEM_Alloc(nSize * sizeof(tVA_UI_IppvItem));
	if (pIppvItemList  == NULL)
	{
		HxLOG_Info("fail to alloc memory\n");
		return VA_ERR;
	}

	stRequest.uRequest.stGetIppvPListRequest.uiSoid = ulSoid;
	stRequest.uRequest.stGetIppvPListRequest.uiMaxItemNumber = nSize;
	stRequest.uRequest.stGetIppvPListRequest.pItemArray = pIppvItemList;
	stRequest.uRequest.stGetIppvPListRequest.stProgramNumberRange.uiMin = ulMin;
	stRequest.uRequest.stGetIppvPListRequest.stProgramNumberRange.uiMax = ulMax;

	pArg = (tVA_UI_GetIppvPListRequest *)VA_UI_SendRequest(ulRequestType, ulScSlot, &eStatus, &stRequest);

	if (eStatus == eVA_UI_ARRAY_TOO_SMALL_STATUS)
	{
		// the array is too small: reallocate the array by the specified size and send a request again
		nSize = pArg->uiItemNumber;

		VA_MEM_Free(pIppvItemList);
		pIppvItemList = VA_MEM_Alloc(nSize * sizeof(tVA_UI_IppvItem));
		if (pIppvItemList  == NULL)
		{
			VA_UI_SendRequestDone();
			HxLOG_Info("fail to alloc memory\n");
			return VA_ERR;
		}
		stRequest.uRequest.stGetIppvPListRequest.uiMaxItemNumber = nSize;
		stRequest.uRequest.stGetIppvPListRequest.pItemArray = pIppvItemList;

		HxLOG_Info("too small array: new size[%d]\n", nSize);

		pArg = (tVA_UI_GetIppvPListRequest *)VA_UI_SendRequest(ulRequestType, ulScSlot, &eStatus, &stRequest);
		if (pArg == NULL)
		{
			VA_UI_SendRequestDone();
			VA_MEM_Free(pIppvItemList);
			HxLOG_Info("fail to send request2!\n");
			return VA_ERR;
		}
	}
	else if (eStatus == eVA_UI_CONSULTATION_LOCKED_STATUS)
	{
		VA_UI_SendRequestDone();
		VA_MEM_Free(pIppvItemList);
		HxLOG_Info("eVA_UI_CONSULTATION_LOCKED_STATUS\n");
		return VA_ERR_UI_CONSULTATION_LOCKED;
	}
	else if (eStatus != eVA_UI_OK_STATUS)
	{
		VA_UI_SendRequestDone();
		VA_MEM_Free(pIppvItemList);
		HxLOG_Info("fail to send request[%d]\n", eStatus);
		return VA_ERR;
	}
	else if (pArg == NULL)
	{
		VA_UI_SendRequestDone();
		VA_MEM_Free(pIppvItemList);
		HxLOG_Info("fail to send request!\n");
		return VA_ERR;
	}

	*pIppvItem = pIppvItemList;
	*pulCount = pArg->uiItemNumber;

	VA_UI_SendRequestDone();
	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}

int VA_UI_GetCreditInfo(UINT32 ulScSlot, UINT32 ulSoid, tVA_UI_GetCreditInfoRequest *pInfo)
{
	tVA_UI_RequestStatus eStatus = eVA_UI_ERROR_STATUS;
	tVA_UI_GetCreditInfoRequest *pArg = NULL;
//	tVA_UI_Request stRequest;

	HxLOG_Print("[Enter]\n");

	VA_ASSERT(pInfo != NULL);
	VA_ASSERT(sizeof(UINT32) == sizeof(tVA_UI_Soid));

	stRequest.uRequest.stGetCreditInfoRequest	.uiSoid = ulSoid;

	pArg = (tVA_UI_GetCreditInfoRequest *)VA_UI_SendRequest(eVA_UI_GET_CREDIT_INFO_REQUEST, ulScSlot, &eStatus, &stRequest);

	if (eStatus == eVA_UI_CONSULTATION_LOCKED_STATUS)
	{
		VA_UI_SendRequestDone();
		HxLOG_Info("[VA_UI_GetCreditInfo] eVA_UI_CONSULTATION_LOCKED_STATUS\n");
		return VA_ERR_UI_CONSULTATION_LOCKED;
	}
	else if (pArg == NULL || eStatus != eVA_UI_OK_STATUS)
	{
		VA_UI_SendRequestDone();
		HxLOG_Info("[VA_UI_GetCreditInfo] fail to send request[%x, %d]\n", pArg, eStatus);
		return VA_ERR;
	}

	VA_memcpy(pInfo, &stRequest.uRequest.stGetCreditInfoRequest, sizeof(tVA_UI_GetCreditInfoRequest));

	VA_UI_SendRequestDone();
	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}

int VA_UI_GetIPPVInfo(UINT32 ulAcsId, UINT32 ulScSlot, UINT32 ulStbStreamHandle, tVA_UI_GetIppvInfoRequest *pInfo)
{
	tVA_UI_RequestStatus eStatus = eVA_UI_ERROR_STATUS;
	tVA_UI_GetIppvInfoRequest *pArg = NULL;
//	tVA_UI_Request stRequest;

	HxLOG_Print("[Enter]\n");

	VA_ASSERT(pInfo != NULL);

	stRequest.uRequest.stGetIppvInfoRequest.dwAcsId = ulAcsId;
	stRequest.uRequest.stGetIppvInfoRequest.dwStbStreamHandle = ulStbStreamHandle;

	pArg = (tVA_UI_GetIppvInfoRequest *)VA_UI_SendRequest(eVA_UI_GET_IPPV_INFO_REQUEST, ulScSlot, &eStatus, &stRequest);

	if (pArg == NULL || eStatus != eVA_UI_OK_STATUS)
	{
		VA_UI_SendRequestDone();
		HxLOG_Info("[VA_UI_GetIPPVInfo] fail to send request[%x, %d]\n", pArg, eStatus);
		return VA_ERR;
	}

	VA_memcpy(pInfo, &stRequest.uRequest.stGetIppvInfoRequest, sizeof(tVA_UI_GetIppvInfoRequest));

	VA_UI_SendRequestDone();
	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}

int VA_UI_AcceptIPPVP(UINT32 ulAcsId, UINT32 ulScSlot, UINT32 ulStbStreamHandle, UINT8 *pPinCode, UINT32 ulProgNum)
{
	tVA_UI_RequestStatus eStatus = eVA_UI_ERROR_STATUS;
	tVA_UI_AcceptIppvPRequest *pArg = NULL;
//	tVA_UI_Request stRequest;

	HxLOG_Print("[Enter]\n");

	VA_ASSERT(pPinCode != NULL);
	VA_ASSERT(sizeof(UINT32) == sizeof(tVA_UI_ProgramNumber));

	VA_memcpy(stRequest.uRequest.stAcceptIppvPRequest.pPinCode, pPinCode, sizeof(tVA_UI_PinCode));
	stRequest.uRequest.stAcceptIppvPRequest.dwAcsId = ulAcsId;
	stRequest.uRequest.stAcceptIppvPRequest.dwStbStreamHandle = ulStbStreamHandle;
	stRequest.uRequest.stAcceptIppvPRequest.uiProgramNumber = ulProgNum;

	pArg = (tVA_UI_AcceptIppvPRequest *)VA_UI_SendRequest(eVA_UI_ACCEPT_IPPVP_REQUEST, ulScSlot, &eStatus, &stRequest);

	if (eStatus != eVA_UI_OK_STATUS)
	{
		VA_UI_SendRequestDone();
		HxLOG_Info("[VA_UI_AcceptIPPVP] fail to send request[%d]\n", eStatus);
		switch (eStatus)
		{
			case eVA_UI_WRONG_PIN_CODE_STATUS:					return VA_ERR_UI_WRONG_PIN_CODE;
			case eVA_UI_LOCKED_PIN_CODE_STATUS:					return VA_ERR_UI_LOCKED_PIN_CODE;
			case eVA_UI_NO_PIN_CODE_STATUS:						return VA_ERR_UI_NO_PIN_CODE;
			case eVA_UI_WRONG_SLOT_STATUS:						return VA_ERR_UI_WRONG_SLOT;
			case eVA_UI_SMARTCARD_ERROR_STATUS:					return VA_ERR_UI_SMARTCARD_ERROR;
		}
		return VA_ERR;
	}
	else if (pArg == NULL)
	{
		VA_UI_SendRequestDone();
		HxLOG_Info("[VA_UI_AcceptIPPVP] fail to send request!\n");
		return VA_ERR;
	}

	VA_UI_SendRequestDone();
	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}

int VA_UI_AcceptIPPVT(UINT32 ulAcsId, UINT32 ulScSlot, UINT32 ulStbStreamHandle, UINT8 *pPinCode, UINT32 ulProgNum, tVA_UI_Price stCeiling)
{
	tVA_UI_RequestStatus eStatus = eVA_UI_ERROR_STATUS;
	tVA_UI_AcceptIppvTRequest *pArg = NULL;
//	tVA_UI_Request stRequest;

	HxLOG_Print("[Enter]\n");

	VA_ASSERT(pPinCode != NULL);
	VA_ASSERT(sizeof(UINT32) == sizeof(tVA_UI_ProgramNumber));

	VA_memcpy(stRequest.uRequest.stAcceptIppvTRequest.pPinCode, pPinCode, sizeof(tVA_UI_PinCode));
	stRequest.uRequest.stAcceptIppvTRequest.dwAcsId = ulAcsId;
	stRequest.uRequest.stAcceptIppvTRequest.dwStbStreamHandle = ulStbStreamHandle;
	stRequest.uRequest.stAcceptIppvTRequest.uiProgramNumber = ulProgNum;
	stRequest.uRequest.stAcceptIppvTRequest.stCeiling.iInteger = stCeiling.iInteger;
	stRequest.uRequest.stAcceptIppvTRequest.stCeiling.uiFraction = stCeiling.uiFraction;

	pArg = (tVA_UI_AcceptIppvTRequest *)VA_UI_SendRequest(eVA_UI_ACCEPT_IPPVT_REQUEST, ulScSlot, &eStatus, &stRequest);

	if (eStatus != eVA_UI_OK_STATUS)
	{
		VA_UI_SendRequestDone();
		HxLOG_Info("[VA_UI_AcceptIPPVT] fail to send request[%d]\n", eStatus);
		switch (eStatus)
		{
			case eVA_UI_WRONG_PIN_CODE_STATUS:					return VA_ERR_UI_WRONG_PIN_CODE;
			case eVA_UI_LOCKED_PIN_CODE_STATUS:					return VA_ERR_UI_LOCKED_PIN_CODE;
			case eVA_UI_NO_PIN_CODE_STATUS:						return VA_ERR_UI_NO_PIN_CODE;
			case eVA_UI_WRONG_SLOT_STATUS:						return VA_ERR_UI_WRONG_SLOT;
			case eVA_UI_SMARTCARD_ERROR_STATUS:					return VA_ERR_UI_SMARTCARD_ERROR;
		}
		return VA_ERR;
	}
	else if (pArg == NULL)
	{
		VA_UI_SendRequestDone();
		HxLOG_Info("[VA_UI_AcceptIPPVT] fail to send request!\n");
		return VA_ERR;
	}

	VA_UI_SendRequestDone();
	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}

int VA_UI_GetThreshold(UINT32 ulScSlot, tVA_UI_Price *pstThreshold)
{
	tVA_UI_RequestStatus eStatus = eVA_UI_ERROR_STATUS;
	tVA_UI_GetThresholdRequest *pArg = NULL;
//	tVA_UI_Request stRequest;

	HxLOG_Print("[Enter]\n");

	VA_ASSERT(pstThreshold != NULL);

	pArg = (tVA_UI_GetThresholdRequest *)VA_UI_SendRequest(eVA_UI_GET_THRESHOLD_REQUEST, ulScSlot, &eStatus, &stRequest);

	if (pArg == NULL || eStatus != eVA_UI_OK_STATUS)
	{
		VA_UI_SendRequestDone();
		HxLOG_Info("[VA_UI_GetThreshold] fail to send request[%x, %d]\n", pArg, eStatus);
		return VA_ERR;
	}

	VA_memcpy(pstThreshold, &stRequest.uRequest.stGetThresholdRequest.stThreshold, sizeof(tVA_UI_Price));

	VA_UI_SendRequestDone();
	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}

int VA_UI_ModifyThreshold(UINT32 ulScSlot, UINT8 *pPinCode,  tVA_UI_Price stThreshold)
{
	tVA_UI_RequestStatus eStatus = eVA_UI_ERROR_STATUS;
	tVA_UI_ModifyThresholdRequest *pArg = NULL;
//	tVA_UI_Request stRequest;

	HxLOG_Print("[Enter]\n");

	VA_ASSERT(pPinCode != NULL);

	VA_memcpy(stRequest.uRequest.stModifyThresholdRequest.pPinCode, pPinCode, sizeof(tVA_UI_PinCode));
	stRequest.uRequest.stModifyThresholdRequest.stNewThreshold.iInteger = stThreshold.iInteger;
	stRequest.uRequest.stModifyThresholdRequest.stNewThreshold.uiFraction = stThreshold.uiFraction;

	pArg = (tVA_UI_ModifyThresholdRequest *)VA_UI_SendRequest(eVA_UI_MODIFY_THRESHOLD_REQUEST, ulScSlot, &eStatus, &stRequest);

	if (eStatus != eVA_UI_OK_STATUS)
	{
		VA_UI_SendRequestDone();
		HxLOG_Info("[VA_UI_ModifyThreshold] fail to send request[%d]\n", eStatus);
		switch (eStatus)
		{
			case eVA_UI_WRONG_PIN_CODE_STATUS:					return VA_ERR_UI_WRONG_PIN_CODE;
			case eVA_UI_LOCKED_PIN_CODE_STATUS:					return VA_ERR_UI_LOCKED_PIN_CODE;
			case eVA_UI_NO_PIN_CODE_STATUS:						return VA_ERR_UI_NO_PIN_CODE;
			case eVA_UI_WRONG_SLOT_STATUS:						return VA_ERR_UI_WRONG_SLOT;
			case eVA_UI_SMARTCARD_ERROR_STATUS:					return VA_ERR_UI_SMARTCARD_ERROR;
		}
		return VA_ERR;
	}
	else if (pArg == NULL)
	{
		VA_UI_SendRequestDone();
		HxLOG_Info("[VA_UI_ModifyThreshold] fail to send request!\n");
		return VA_ERR;
	}

	VA_UI_SendRequestDone();
	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}

#ifdef CONFIG_MW_CAS_VIACCESS_IPPV_SURVEY
int VA_UI_StartIPPVSurvey(UINT32 ulScSlot, UINT32 ulSoid)
{
	tVA_UI_RequestStatus eStatus = eVA_UI_ERROR_STATUS;
	tVA_UI_StartIppvSurveyRequest *pArg = NULL;
//	tVA_UI_Request stRequest;

	HxLOG_Print("[Enter]\n");

	VA_ASSERT(sizeof(UINT32) == sizeof(tVA_UI_Soid));

	stRequest.uRequest.stStartIppvSurveyRequest.uiSoid = ulSoid;

	pArg = (tVA_UI_StartIppvSurveyRequest *)VA_UI_SendRequest(eVA_UI_START_IPPV_SURVEY_REQUEST, ulScSlot, &eStatus, &stRequest);

	if (eStatus == eVA_UI_RESOURCE_BUSY_STATUS)
	{
		VA_UI_SendRequestDone();
		HxLOG_Info("[VA_UI_StartIPPVSurvey] RESOURCE_BUSY\n");
		return VA_ERR_UI_RESOURCE_BUSY;
	}
	else if (eStatus == eVA_UI_CONNECTION_ERROR_STATUS)
	{
		VA_UI_SendRequestDone();
		HxLOG_Info("[VA_UI_StartIPPVSurvey] CONNECTION_ERROR\n");
		return VA_ERR_UI_CONNECTION_ERROR;
	}
	else if (pArg == NULL || eStatus != eVA_UI_OK_STATUS)
	{
		VA_UI_SendRequestDone();
		HxLOG_Info("[VA_UI_StartIPPVSurvey] fail to send request[%x, %d]\n", pArg, eStatus);
		return VA_ERR;
	}

	VA_UI_SendRequestDone();
	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}

int VA_UI_CancelIPPVSurvey()
{
	tVA_UI_RequestStatus eStatus = eVA_UI_ERROR_STATUS;
//	tVA_UI_Request stRequest;

	HxLOG_Print("[Enter]\n");

	VA_UI_SendRequest(eVA_UI_CANCEL_IPPV_SURVEY_REQUEST, kVA_UI_NOT_USED, &eStatus, &stRequest);

	if (eStatus != eVA_UI_OK_STATUS)
	{
		VA_UI_SendRequestDone();
		HxLOG_Info("[VA_UI_CancelIPPVSurvey] fail to send request[%d]\n", eStatus);
		return VA_ERR;
	}

	VA_UI_SendRequestDone();
	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}
#endif	// CONFIG_MW_CAS_VIACCESS_IPPV_SURVEY
#endif	// CONFIG_MW_CAS_VIACCESS_IPPV_SURVEY

#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
int VA_UI_GetRecordInfo(UINT32 ulAcsId, UINT32 ulStbStreamHandle, tVA_UI_Date *pDate, UINT32 *pulPlaybackDuration, UINT32 *pulMaxNumberOfPlayback)
{
	tVA_UI_RequestStatus eStatus;
	tVA_UI_GetRecordInfoRequest *pArg;
//	tVA_UI_Request stRequest;
//	int nSize = 16;

	HxLOG_Print("[Enter]\n");

	stRequest.uRequest.stGetRecordInfoRequest.dwAcsId = ulAcsId;
	stRequest.uRequest.stGetRecordInfoRequest.dwStbStreamHandle = ulStbStreamHandle;

	pArg = (tVA_UI_GetRecordInfoRequest *)VA_UI_SendRequest(eVA_UI_GET_RECORD_INFO_REQUEST, kVA_UI_NOT_USED, &eStatus, &stRequest);

	if (eStatus == eVA_UI_PVR_NO_RIGHTS_STATUS)
	{
		VA_UI_SendRequestDone();
		HxLOG_Info("[VA_UI_GetRecordInfo] eVA_UI_PVR_NO_RIGHTS_STATUS\n");
		return VA_ERR_UI_WRONG_PVR_NO_RIGHT;
	}
	else if (eStatus != eVA_UI_OK_STATUS)
	{
		VA_UI_SendRequestDone();
		HxLOG_Info("[VA_UI_GetRecordInfo] fail to send request[%d]\n", eStatus);
		return VA_ERR;
	}
	else if (pArg == NULL)
	{
		VA_UI_SendRequestDone();
		HxLOG_Info("[VA_UI_GetRecordInfo] fail to send request!\n");
		return VA_ERR;
	}

	VA_memcpy(pDate, &pArg->stExpirationDate, sizeof(tVA_UI_Date));
	*pulPlaybackDuration = pArg->uiPlaybackDurationValidityInDay;
	*pulMaxNumberOfPlayback = pArg->uiMaxNumberOfPlayback;

	VA_UI_SendRequestDone();
	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}

int VA_UI_GetPlaybackInfo(UINT32 ulAcsId, HUINT32 *pulPlaybackCounter, tVA_UI_DateEx *pRecordDate, tVA_UI_DateEx *pFirstPlaybackDate)
{
	tVA_UI_RequestStatus eStatus;
	tVA_UI_GetPlaybackInfoRequest *pArg;
//	tVA_UI_Request stRequest;
//	int nSize = 16;

	HxLOG_Print("[Enter]\n");

	stRequest.uRequest.stGetPlaybackInfoRequest.dwAcsId = ulAcsId;

	pArg = (tVA_UI_GetPlaybackInfoRequest *)VA_UI_SendRequest(eVA_UI_GET_PLAYBACK_INFO_REQUEST, kVA_UI_NOT_USED, &eStatus, &stRequest);

	if (eStatus == eVA_UI_PVR_NO_RIGHTS_STATUS)
	{
		VA_UI_SendRequestDone();
		HxLOG_Info("[VA_UI_GetRecordInfo] eVA_UI_PVR_NO_RIGHTS_STATUS\n");
		return VA_ERR_UI_WRONG_PVR_NO_RIGHT;
	}
	else if (eStatus != eVA_UI_OK_STATUS)
	{
		VA_UI_SendRequestDone();
		HxLOG_Info("[VA_UI_GetRecordInfo] fail to send request[%d]\n", eStatus);
		return VA_ERR;
	}
	else if (pArg == NULL)
	{
		VA_UI_SendRequestDone();
		HxLOG_Info("[VA_UI_GetRecordInfo] fail to send request!\n");
		return VA_ERR;
	}

	*pulPlaybackCounter = pArg->uiHeaderPlaybackCounter;
	VA_memcpy(pRecordDate, &pArg->stRecordDate, sizeof(tVA_UI_DateEx));
	VA_memcpy(pFirstPlaybackDate, &pArg->stFirstPlaybackDate, sizeof(tVA_UI_DateEx));

	VA_UI_SendRequestDone();
	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}

int VA_UI_IncrementPlaybackCount(UINT32 ulAcsId)
{
	tVA_UI_RequestStatus eStatus;
	tVA_UI_IncrementPlaybackCounterRequest *pArg;
//	tVA_UI_Request stRequest;
//	int nSize = 16;

	HxLOG_Print("[Enter]\n");

	stRequest.uRequest.stIncrementPlaybackCounterRequest.dwAcsId = ulAcsId;

	pArg = (tVA_UI_IncrementPlaybackCounterRequest *)VA_UI_SendRequest(eVA_UI_INCREMENT_PLAYBACK_COUNTER_REQUEST, kVA_UI_NOT_USED, &eStatus, &stRequest);

	if (eStatus != eVA_UI_OK_STATUS)
	{
		VA_UI_SendRequestDone();
		HxLOG_Info("[VA_UI_GetRecordInfo] fail to send request[%d]\n", eStatus);
		return VA_ERR;
	}
	else if (pArg == NULL)
	{
		VA_UI_SendRequestDone();
		HxLOG_Info("[VA_UI_GetRecordInfo] fail to send request!\n");
		return VA_ERR;
	}

	VA_UI_SendRequestDone();
	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}
#endif //CONFIG_VA_PVR


