/**************************************************************************************************/
#define ________CA_CAK_PPV_Private_Include_________________________________________________
/**************************************************************************************************/
#include "vkernel.h"
#include "na_glob.h"

#include "na_mgr_cak_ext.h"
#include <na_mgr_cak_ppv.h>
#include "util.h"

#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)
/**************************************************************************************************/
#define ________CA_CAK_PPV_Golbal_Value___________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define ________CA_CAK_PPV_Private_Define__________________________________________________
/**************************************************************************************************/





#define QUEUE_SIZE_1						sizeof(NaCasMsg_t)
#define QUEUE_SIZE_2						(sizeof(TUnsignedInt32) * 4)

#define VK_CreateMessageQueue(a, b, c, d)	VK_MSG_Create(a, d, c, b, VK_SUSPENDTYPE_PRIORITY)
#define VK_PostMessage(a, b, c) 			VK_MSG_Send(a, b, c)
#define VK_GetMessage(a ,b, c)				VK_MSG_Receive(a, b, c)
#define VK_WaitGetMessage(a, b, c, d)		VK_MSG_ReceiveTimeout(a, b, d, c)
#define VK_PostMessageTimeout(a, b, c)		VK_MSG_SendTimeout(a, b, c, 5000)

#define NA_MAX_CALLBACK_NUM 				2		// MHP xlet을 위해 2 item array로 만듬

#define NA_CAS_ALL_RANGE							0x1800 //0x1800 ~ 0x18ff
#define NA_CAS_MEDIASE_RANGE_FROM				0x1802
#define NA_CAS_MEDIASE_RANGE_TO					0x1805

#define MEDIASET_PREMIUM_ONID						272 //Mediaset Premium original network id :FIXED; CAK6_I ext, implementation guide ver2.02 p33
#define INVALID_NIT_ONID							0xffff

#define MAX_EIT_DES_SIZE 							4096
#define MAX_NIT_WAIT_COUNTER				20
#define NIT_WAIT_TIMER_INTERVAL				1000
#define MAX_PDT_LOADED_WAIT_TIME_FOR_PRODUCT  60000
#define MAX_PDT_LOADED_WAIT_TIME 			5000 // 5초 동악 기다려 준다. pdt loading을 기다림.  banner에서는 이 이상의 기다림은 무리. blocking 됨
#define MAX_PDT_LOADED_WAIT_TIME_SC 		2000
#define MAX_PDT_LOADED_WAIT_TIME_INTERVAL 	100


/**************************************************************************************************/
#define ________CA_CAK_PPV_Private_Type_Define____________________________________________
/**************************************************************************************************/
typedef enum
{
	CA_PPV_CALLBACK_CREDIT = 0,
	CA_PPV_CALLBACK_PURCHASE_HISTORY
} CA_PPV_CALLBACK_TYPE_T;


typedef enum
{	PDT_FILTER_NONE = 0,
	PDT_FILTER_CPDT =1,
	PDT_FILTER_HPDT =2,
	PDT_FILTER_CPDT_AND_HPDT =3
} PDT_FILTER_MODE;

/* Current NIT Info struct */
typedef struct CUR_NIT_INFO_STRUCT
{
	TBoolean				bArrived; //current TS에 관한 NIT 도착 유무 확인 flag..
	TUnsignedInt16			orgNetId; //NIT의 original Net ID
} CUR_NIT_INFO;


/* PPV State Process */
typedef enum
{
	S_TSCHK_NAGRA_PROC = 0,
}PPV_STATE_PROCESS;


/* PDT Data structure */
typedef struct PDT_DATA_STRUCT
{
	TUnsignedInt16		tsId;
	TUnsignedInt16		svcId;
	TUnsignedInt16		orgNetId;
	TUnsignedInt16		pid;
} PDT_DATA;

/* Event purchase */
typedef struct EVT_PURCHASE_STRUCT
{
	unsigned char	EvtName[MAX_STR_LEN];
	unsigned short	EvtId;
	TCalendarTime	xEventStartTime;
	unsigned long	DesLength;
	unsigned char	Des[MAX_EIT_DES_SIZE];
	unsigned char	rawEvtNameLength;
	unsigned char*	rawEvtName;
	TEventType		xType;
	UNIXTIME		startTime;
	UNIXTIME		duration;
} EVT_PURCHASE;


//현재 current TS에 관한 NIT가 왔는 지 기다림..
typedef enum
{
	eNIT_WAIT_INIT = 0,
	eNIT_WAIT_ING,
 	eNIT_WAIT_DONE
} NIT_WAIT_FLAG;

typedef enum
{
	eNIT_WAIT_OWNER_IPPV = 0, 	//banner에서 IPPV가 기다림
	eNIT_WAIT_OWNER_MENU, 		//menu에서 기다림.
	eNIT_WAIT_OWNER_NONE 		//eNIT_WAIT_NONE
} NIT_WAIT_OWNER;

typedef enum
{
	ePRODUCT_LOAD_WAIT_INIT = 0,
 	ePRODUCT_LOAD_WAIT_ING
}PRODUCT_LOAD_WAIT_FLAG;

typedef enum
{
	eEIT_PF_PRESENT = 0,
 	eEIT_PF_FOLLOWING
} EVENT_PF_INFO;


typedef struct NIT_WAIT_ITEM_STRUCT
{
	NIT_WAIT_OWNER 	eOwner;
	TPpid 			provider;
	NIT_WAIT_FLAG 	flag;
} NIT_WAIT_ITEM;

/**************************************************************************************************/
#define ________CA_CAK_PPV_Private_Static_Value____________________________________________
/**************************************************************************************************/
static EVT_PURCHASE				s_stPresentEvtPurchase;
static EVT_PURCHASE				s_stFollowingEvtPurchase;

static TCreditItem 				s_stCurCreditList;
static PACKAGE_INFO				s_stCurPackageInfo;
static PPV_PACKAGE_LIST			s_stPackageList;
static PPV_PURCHASE_LIST		s_stPurchaseList;
static RECHARGE_LIST			s_stRechargeList;

//static NAGRA_PPV_STATE			s_eCurPpvState;

static CUR_NIT_INFO				s_stNitArrivedInfo;	//current TS에 관한 NIT 도착 유무 확인! 초기화 어디에선가 해줘야함!!!
static TBoolean					s_bIsNagraTS = FALSE;
static Handle_t					s_hCurSvc = HANDLE_NULL;
static Handle_t					s_hCurAction = HANDLE_NULL;

static PRODUCT_LOAD_WAIT_FLAG 	s_eProductLoadFlag = ePRODUCT_LOAD_WAIT_INIT;
static TBoolean 				s_bIsProductsLoaded = FALSE;
static TBoolean					s_bTdtReceived = FALSE;				// tp 전환 후 처음 TDT를 받으면 Eit를 받은 것처럼 처리. Eit를 받을 당시에 TDT를 못 받았을 수 있기 때문
static NIT_WAIT_FLAG 			s_eNitWaitFlag = eNIT_WAIT_INIT;
static TUnsignedInt32			s_ulNITwaitTimerId = 0;
static TUnsignedInt32 			s_ulProductLoadTimerId = 0;
static TUnsignedInt32			s_ulEITwaitTimerId = 0;				// IPPV 시간에 다시 Event 체크하도록
static TBoolean					s_bIsFullInfoPackageRequested = FALSE;


static TUnsignedInt32 			s_ulPackageListSemId;
static TUnsignedInt32 			s_ulRechargeListSemId;
static TUnsignedInt32 			s_ulPurchaseListSemId;
static TUnsignedInt32 			s_ulSmartcardSemId; //CAK HANG-up 방지용 , SMARTCARD request를 동시에 한개 이상이 create하고  request를 send하면 send에서 CAK가 hang-Up 된다.
static TUnsignedInt32			s_ulEitSemId;
static TUnsignedInt32			s_ulNitSemId;

static TUnsignedInt32 			s_ulPpvTaskId;
static TUnsignedInt32 			s_ulPpvQueueId;

static TCardCreditCallback					s_pfnCardCreditCallBack[NA_MAX_CALLBACK_NUM] = { NULL, NULL };
static TPackageListNotifyCallback 			s_pfnPackageListNofityCallback[NA_MAX_CALLBACK_NUM] = {NULL,NULL};
static TPackageListNotifyCallback			s_pfnPackageListFullInfoNofityCallback = NULL;
static TPurchasePackageNotifyCallback 		s_pfnPurchasePackageNotifyCallback = NULL; //request package purchase notify callback
static TFullInfoPackageListNotifyCallback 	s_pfnFullInfoPackageListNotifyCallback = NULL;//request Full Info package list notify callback // 현재 MHP xlet에서만 사용
static TCaListenerArrivedNotifyCallback		s_pfnCaListenerArrivedNotifyCallback = NULL; //ca에서 listener가 올때 마다 전달 해 주는 callback
static TTSCheckNotifyCallback				s_pfnTSCheckNotifyCallback = NULL; //현재 TUNING된 TS가 request 된 ProviderID의 TS인지 확인.
static TRequestEventNotifyCallback 			s_pfnEventInfoNofityCallback[NA_MAX_CALLBACK_NUM] = {NULL,NULL};
static TPurchaseEventNotifyCallback			s_pfnPurchaseEventNotifyCallback = NULL;
static TPurchaseHistoryNotifyCallback		s_pfnPurchaseHistoryNotifyCallback[NA_MAX_CALLBACK_NUM] = {NULL,NULL};
static TRechargeListNotifyCallback			s_pfnRechargeListNotifyCallback[NA_MAX_CALLBACK_NUM] = {NULL,NULL};
static TOPPVPurchaseListNotifyCallback		s_pfnOPPVPurchaseListNotifyCallback[NA_MAX_CALLBACK_NUM] = {NULL, NULL};
static TRechargeNotifyCallback				s_pfnRechargeAlarmNotifyCallback[NA_MAX_CALLBACK_NUM] = {NULL, NULL};

static TCreditAmount			s_ulCardRemainedCredit = 0;
static PDT_DATA 				s_CPDT_DATA, s_HPDT_DATA;
//static PPV_EVENT_INFO			s_stLastPpvEvent;
static OPPV_PURCHASE_INFO 		s_stLastOPPVPurchaseInfo; //최신 전달 된 OPPV info 저장 . sNewPurchaseListenerCallBack


HxLOG_Print("[CAPPV_PACKAGE_SEM_GET] 1---->\n");
HxLOG_Print("[CAPPV_PACKAGE_SEM_RELEASE] 1<----\n");

HxLOG_Print("[CAPPV_RECHARGE_SEM_GET] 1---->\n");
HxLOG_Print("[CAPPV_RECHARGE_SEM_RELEASE] 1<----\n");

HxLOG_Print("[CAPPV_PURCHASELIST_SEM_GET] 1---->\n");
HxLOG_Print("[CAPPV_PURCHASELIST_SEM_RELEASE] 1<----\n");

HxLOG_Print("[CAPPV_SMARTCARD_SEM_GET] 1---->");
HxLOG_Print("[CAPPV_SMARTCARD_SEM_RELEASE] 1<----");

HxLOG_Print("[CAPPV_EIT_SEM_GET] 1---->\n");
HxLOG_Print("[CAPPV_EIT_SEM_RELEASE] 1<----\n");

HxLOG_Print("[CAPPV_NIT_SEM_GET] 1---->\n");
HxLOG_Print("[CAPPV_NIT_SEM_RELEASE] 1<----\n");


/**************************************************************************************************/
#define ________CA_CAK_PPV_Private_Static_Prototype_________________________________________
/**************************************************************************************************/
static HERROR 		local_caPpv_RequestAsyncGetCredit(void);
static void 		local_caPpv_CallbackCardCredit(TCaRequest *pxRequest);
static void 		local_caPpv_SendCakMsg(TUnsignedInt8 ucMsg, TUnsignedInt16 param1, TUnsignedInt8 *param2, TUnsignedInt32 param3);
static void 		local_caPpv_InitPackageInfo(PACKAGE_INFO *PackageInfo);
static void 		local_caPpv_InitPackageList(PPV_PACKAGE_LIST* pPackage_list, TBoolean bDispose);
static TEventStatus local_caPpv_RequestPackageDataList(unsigned long nTimerId,  void *params);
static TEventStatus local_caPpv_RequestMediasetPackageList(TUnsignedInt32 waitCounter);
static HBOOL 		local_caPpv_CheckBusinessProductTypeWithProviderID(TPpid providerID, TUnsignedInt8 businessType);
static HERROR 		local_caPpv_WaitForGivenTimeWithGivenInterval(UNIXTIME waitInterval, UNIXTIME maxWaitTime);
static void 		local_caPpv_PRINT_PACKAGE_LIST(PPV_PACKAGE_LIST *packList);
static void 		local_caPpv_ResponsePackageList(TCaRequest* pRequest);
static void 		local_caPpv_WaitPackageList_TimerCallback(unsigned long nTimerId,  void *pArg);
static void 		local_caPpv_PackageListCallback(TCaRequest* pxRequest);
static void 		local_caPpv_NotifyNITWaitTimeoutForPackageList(TUnsignedInt32 waitCounter, TPpid providerID);
static void 		local_caPpv_SetNITArrivedFlag(TBoolean flag, TUnsignedInt16 orgNetId);
static CUR_NIT_INFO* local_caPpv_GetNITArrivedFlag(void);
static void 		local_caPpv_WaitNIT_TimerCallback(unsigned long TimerId,  void *pArg);
static void 		local_caPpv_CheckNIT(TUnsignedInt32 ulTimerId, void* pArg);
static HBOOL 		local_caPpv_CheckGivenProviderIDInPMT(TUnsignedInt8 *numOfCADesc, const TUnsignedInt16 providerID);
static HBOOL 		local_caPpv_CheckGivenCaSysIdInPMT(TUnsignedInt8 *numOfCADesc, const TUnsignedInt16 ecmCaSystemId);
static void 		local_caPpv_ExecuteTsCheck(Handle_t svcHandle, NIT_WAIT_FLAG flag);
static void 		local_caPpv_ExecuteEventInfo(void);
static HERROR 		local_caPpv_GetPurchaseWIndowOfProduct(TCaProduct *pxProduct, UNIXTIME *pnStartTime, UNIXTIME *pnEndTime);
static TEventStatus local_caPpv_ProcessPPT(TCaEvent *ppEvent, TCaProduct *pxProduct, TPurchasableEventType *xPuchaseType, UNIXTIME *xPurchaseStartTime, UNIXTIME *xPurchaseEndTime);
static TEventStatus local_caPpv_ProcessSpecialEvent(TCaProduct *pxProduct, TPurchasableEventType *xPuchaseType, UNIXTIME *xPurchaseStartTime, UNIXTIME *xPurchaseEndTime);
static HERROR 		local_caPpv_AddLocalTimeOffset(UNIXTIME *startTime, UNIXTIME *endTime);
static void 		local_caPpv_ResponsePresentEventCallback(TCaRequest*  pxRequest);
static void 		local_caPpv_ResponseFollowingEventCallback(TCaRequest*  pxRequest);
static void 		local_caPpv_CheckFollowingEvent(void);
static void 		local_caPpv_SetProductsLoadable(TBoolean flag);
static void 		local_caPpv_SendCaListenerArrivedNotify(TCaListenerType listenerType);
static void 		local_caPpv_CakPpv_Task(void* p);
//static HERROR 		local_caPpv_GetEventData(Handle_t hSvcHandle);
static void 		local_caPpv_PurchaseHistoryCallback(TCaRequest* pxRequest);
static void 		local_caPpv_ClearRechargeList(RECHARGE_LIST* pList);
static TEventStatus local_caPpv_RequestRechargeList(void);
static void 		local_caPpv_RechargeListCallback(TCaRequest* pxRequest);
static void 		local_caPpv_ResponseRechargeListCallBack(TCaRequest*  pxRequest);
static void 		local_caPpv_ClearPurchaseInfo(EVT_PURCHASE *pEventPurchaseInfo);
static TBoolean 	local_caPpv_IsSamePurchaseInfo(EVT_PURCHASE stEvtPurchase1, EVT_PURCHASE stEvtPurchase2);

static HERROR 		local_caPpv_RequestPurchaseSpecialEvent(TCaProduct *pProduct);
static HERROR 		local_caPpv_RequestPurchasePPT(TCaEvent *pEvent, TCaProduct *pProduct);
static TCaRequestStatus local_caPpv_RequestWatchPPT(TCaProcessingStatus *procStatus);
static void 		local_caPpv_PurchaseEventCallback(TCaRequest* pxRequest);
static void 		local_caPpv_WaitEIT_TimerCallback(unsigned long TimerId,  void *pArg);


static HERROR local_caPpv_RequestAsyncGetCredit(void)
{
	TCaRequest			*pCaRequest;
	TCaRequestStatus	eStatus;

	eStatus = CA_NA_PpvCreateRequest(CA_REQUEST_TYPE_CREDITS, &pCaRequest);
	if (eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Info("error in caRequestCreate (0x%x)\n", eStatus);
		return ERR_FAIL;
	}

	eStatus = caRequestSetAsynchronousResponse(pCaRequest, local_caPpv_CallbackCardCredit);
	if (eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Info("error in caRequestSetAsynchronousResponse (0x%x)\n", eStatus);
		CA_NA_PpvDisposeRequest(pCaRequest);
		return ERR_FAIL;
	}

	eStatus = caRequestSend(pCaRequest);
	if (eStatus != CA_REQUEST_NO_ERROR)
	{
		HxLOG_Info("error in caRequestSend (0x%x)\n", eStatus);
		CA_NA_PpvDisposeRequest(pCaRequest);
	}

	return ERR_OK;
}

static void local_caPpv_CallbackCardCredit(TCaRequest *pxRequest)
{
	TCaRequestStatus 		eStatus;
	TCaObjectStatus 		eObjStatus;
	TCaProcessingStatus		eProcStatus;
	TUnsignedInt32 			creditNb = 0;
	TCaCredit				**ppCredit;
	TUnsignedInt16			index = 0;
	TPpid 					responseProviderId;
	TCreditAmount 			nCredit = 0;
	TCreditFlags			flag = 0;
	TEventStatus 			result = CA_EVENT_ERROR;

	NaCasMsg_t 				stMsg;
	TSignedInt32 			nErrCode;

	if (pxRequest == NULL)
		return;

	eStatus = caRequestGetProcessingStatus(pxRequest, &eProcStatus);
	if ( (eStatus != CA_REQUEST_NO_ERROR) || ( eProcStatus != CA_PROCESSING_NO_ERROR) )
	{
		HxLOG_Print("error in caRequestGetProcessingStatus (0x%x, 0x%x)\n", eStatus, eProcStatus);
		CA_NA_PpvDisposeRequest(pxRequest);
		return;
	}

	eStatus = caRequestGetObjects(pxRequest, &creditNb, &ppCredit);
	HxLOG_Print("caRequestGetObjects reqStatus = 0x%x, creditNb = %ld\n", eStatus, creditNb);
	if ((eStatus != CA_REQUEST_NO_ERROR) || (creditNb <= 0) || (ppCredit == NULL))
	{
		CA_NA_PpvDisposeRequest(pxRequest);
		return;
	}

	for (index = 0; index < creditNb; index++)
	{
		eObjStatus = caCreditGetProviderId(ppCredit[index], &responseProviderId);
		if (eObjStatus == CA_OBJECT_NO_ERROR)
		{
			if ( (creditNb > 1) && (index != creditNb - 1))
			{
				TSmartcardInfoItem		tSysScInfo[NO_SMC_SLOT];

				// 1개 이상이 올떄는 providerID 확인
				CA_NA_CakGetSystemInfo(NULL, NULL, NULL, &tSysScInfo);
				if (responseProviderId == tSysScInfo[0].provider)
				{
					HxLOG_Print("\t[%d]BINGO, found provID(0x%x)\n", index, responseProviderId);

					eObjStatus = caCreditGetAmount(ppCredit[index], &nCredit);
					if (eObjStatus != CA_OBJECT_NO_ERROR)
						nCredit = 0;

					eObjStatus = caCreditGetFlags(ppCredit[index], &flag);
					if (eObjStatus != CA_OBJECT_NO_ERROR)
						flag = 0xff;						// Mediaset에서는 credit suspended는 체크하고 expiration 체크 안함
					HxLOG_Print("\t[%d]BINGO credit:%ld, flag:0x%x\n",index, nCredit, flag);

					result = CA_EVENT_SUCCESS;
					break;
				}
				else
				{
					HxLOG_Print("\t[%d]NOT, found givenProviderID(0x%x)\n", index, responseProviderId);
				}
			}
			else
			{
				HxLOG_Print("\t[%d]creditNb == 1, provID(0x%x)\n", index, responseProviderId);

				eObjStatus = caCreditGetAmount(ppCredit[index], &nCredit);
				if (eObjStatus != CA_OBJECT_NO_ERROR)
					nCredit = 0;

				eObjStatus = caCreditGetFlags(ppCredit[index], &flag);
				if (eObjStatus != CA_OBJECT_NO_ERROR)
					flag = 0;								// Mediaset에서는 credit suspended는 체크하고 expiration 체크 안함
				HxLOG_Print("\t[%d]credit:%ld, flag:0x%x\n",index, nCredit, flag);

				result = CA_EVENT_SUCCESS;
				break;
			}

		}
		else
		{
			HxLOG_Print("caCreditGetProviderId objStatus = 0x%x\n", eObjStatus);
		}

	} // 4

	CA_NA_PpvDisposeRequest(pxRequest);

	//CAK_CREDIT_CALL_BACK 를 날려준다.

	// Send Msg
	stMsg.wMsg = NA_CAK_ID;
	stMsg.lParam1 = (TSignedInt32)nCredit;
	stMsg.lParam2 = CAK_CREDIT_CALL_BACK;
	stMsg.lParam3 = (TUnsignedInt16)result;
	stMsg.hWnd = 0;
	stMsg.info = 0;

	nErrCode = VK_PostMessage(s_ulPpvQueueId, (void *)&stMsg, QUEUE_SIZE_1);
	if (nErrCode != ERR_OK)
	{
		HxLOG_Print("error in VK_PostMessage (0x%x)\n", (unsigned int)nErrCode);
	}
}

static void local_caPpv_SendCakMsg(TUnsignedInt8 ucMsg, TUnsignedInt16 param1, TUnsignedInt8 *param2, TUnsignedInt32 param3)
{
	TUnsignedInt16 i = 0;
	switch(ucMsg)
	{
		case CA_PPV_CALLBACK_CREDIT:
			for (i = 0; i < NA_MAX_CALLBACK_NUM; i ++)
			{
				if (s_pfnCardCreditCallBack[i] != NULL)
				{
					(*s_pfnCardCreditCallBack[i])(0, param1, param3);
					HxLOG_Print("Sent [%d]CA_PPV_CALLBACK_CREDIT msg\n", i);
				}
			}
			break;

		case CA_PPV_CALLBACK_PURCHASE_HISTORY:
//			if (s_pfnSysInfoCallBack != NULL) (*s_pfnSysInfoCallBack)(s_eSysInfoStatus, &s_stSysIrdInfo, s_usScInfoNb, s_stSysScInfo);
			HxLOG_Print("Sent CA_PPV_CALLBACK_PURCHASE_HISTORY msg\n");
			break;

		default:
			break;
	}
}

static void local_caPpv_InitPackageInfo(PACKAGE_INFO *PackageInfo)
{
	//arg로 받은 PACKAGE_INFO를 clear 시켜서 초기화 시켜 준다.  또 쓰게...
	if (PackageInfo)
	{
		PackageInfo->pRequest = NULL;
		PackageInfo->pProduct = NULL;
		PackageInfo->pServiceFilter = NULL;
		PackageInfo->nDesSize = 0;
		HxSTD_memset(PackageInfo->aucDescription, 0x00, MAX_MSET_PPV_DESCRIPTION_LENGTH+1);
		PackageInfo->nDisclaimerSize = 0;
		HxSTD_memset(PackageInfo->aucDisclaimer, 0x00, MAX_MSET_PPV_DISCLAIMER_LENGTH+1);

		HxLOG_Print("PackageInfo.xNbEvents = %ld\n", PackageInfo->ulNbEvents);

		PackageInfo->pEvents = NULL;
		PackageInfo->ulNbEvents = 0;
	}
}

static void local_caPpv_InitPackageList(PPV_PACKAGE_LIST* pPackage_list, TBoolean bDispose)
{
	HxLOG_Print("ENTER - (dispose:%d)\n", bDispose);

	if (pPackage_list == NULL)
	{
		HxLOG_Print("EXIT - pPackage_list (NULL)\n");
		return;
	}

	CAPPV_PACKAGE_SEM_GET();

	//arg로 받은 PACKAGE_LIST를 clear 시켜서 초기화 시켜 준다.  또 쓰게...
	//여기서 AP가 이전에 request한 list가 이미 있는 지 살펴 보고, 있으면 기존 것을 dispose하고 불러라...
	//여기....여기 구현 해라..
	if (bDispose)
	{
		CA_NA_PpvDisposeRequest(pPackage_list->pRequest);
		pPackage_list->pRequest = NULL;
		HxLOG_Print("dispose pRequest., set pRequest to NULL\n");
	}

	//여기 이 xNbEvents의 숫자를 믿는다..
	if ((pPackage_list->pList!= NULL) && (pPackage_list->xNbItems > 0))
	{
		PPV_PACKAGE_LIST_ITEM *holder, *traveler;

		holder =  traveler = pPackage_list->pList;

		HxLOG_Print("pPackage_list->xNbItems = %ld\n", pPackage_list->xNbItems);

		while (traveler)
		{

			//이제 pPackage_list의 한 product씩 claer, 먼저 내부 service list를 clear 시킨다.
			if ((traveler->pServices != NULL) &&(traveler->NumOfServices > 0))
			{
				HxLOG_Print("traveler->NumOfServices = %d\n", traveler->NumOfServices);

				OxCAS_Free(traveler->pServices);
			}

			//PACKAGE INFO 확인..
			local_caPpv_InitPackageInfo(&traveler->PackageInfo);

			//hold next node;
			holder = traveler->pNext;

			OxCAS_Free(traveler);
			traveler = NULL;

			//move to next node
			traveler = holder;
		}//clear each package list one by one.

	}

	pPackage_list->pList = NULL;
	pPackage_list->xNbItems = 0;


	CAPPV_PACKAGE_SEM_RELEASE();

	HxLOG_Print("EXIT\n");
}

static TEventStatus local_caPpv_RequestPackageDataList(unsigned long nTimerId,  void *params)
{
	//AP에서 CAK_UIT_REQUEST_PACKAGE_LIST를 uitask에 전달 하면 uitask에서 call 함.
	TCaRequestStatus 	status;
	TCaRequest			*pRequest;
	TEventStatus 		errMsg = CA_EVENT_SUCCESS;
	TUnsignedInt16 		i = 0;

	HxLOG_Print("ENTER - CPDT is available now...or timer_CANCELLED!! s_bIsProductsLoaded(%d), nTimerId(%ld), s_ulProductLoadTimerId(%ld)\n", s_bIsProductsLoaded, nTimerId, s_ulProductLoadTimerId);

	// TimerId != 0 : TimerId가 0일 경우는 timer에 의한 call이 아닌 direct call이므로 무조건 수행 되어야 한다.
	if ((nTimerId != 0) && ((s_ulProductLoadTimerId == 0) || (nTimerId != s_ulProductLoadTimerId)))
	{
		// s_ulProductLoadTimerId : 다른 request에 의하여 timer가 초기화 되었다.
		// TimerId != s_ulProductLoadTimerId : 현재 원하는 request가 아니다.
		HxLOG_Print("wrong s_ProductLoadTimerId\n");
		return CA_EVENT_ERROR;
	}

	// direct call 이거나,  기다리던 timer id 다 .....  s_ulNITwaitTimerId 초기화....
	s_ulProductLoadTimerId = 0;

	s_eProductLoadFlag = ePRODUCT_LOAD_WAIT_INIT; //무조건 여기까지 들어 온 이상 s_uitProductLoadFlag는 INIT가 된다.  time out 이 되어서 여기 들어 올 수 있다.

	//이전것 clear....
	//만일 이전 PackageList request data가 남아 있으면 지워라...
	local_caPpv_InitPackageList(&s_stPackageList, TRUE);

	// Package List Request
	status = CA_NA_PpvCreateRequest(CA_REQUEST_TYPE_PRODUCTS, &pRequest);
	if(status == CA_REQUEST_NO_ERROR)
	{
		status = caRequestSetAsynchronousResponse(pRequest, local_caPpv_PackageListCallback);
		if(status == CA_REQUEST_NO_ERROR)
		{
			status = caRequestSend(pRequest);
			if (status == CA_REQUEST_NO_ERROR)
			{
				HxLOG_Print("request CA_REQUEST_TYPE_PRODUCTS SUEESSS, now wait for call back, pRequest(%ld)\n", pRequest);
				s_stPackageList.pRequest = pRequest;

				//여기서는 AP callBack으로 넘겨 주지 않는다.  local_caPpv_PackageListCallback 에서 기다려라...
				return CA_EVENT_SUCCESS;
			}
			else
			{
				errMsg = CA_EVENT_ERROR;
			}
		}
		else
		{
			HxLOG_Print("caRequestSetAsynchronousResponse - CA_REQUEST_TYPE_PRODUCTS FAIL , give noti to callback\n");
		}
	}
	else
	{
		HxLOG_Print("CA_NA_PpvCreateRequest - CA_REQUEST_TYPE_PRODUCTS FAIL , give noti to callback\n");
	}

	CA_NA_PpvDisposeRequest(pRequest);

	//CAK로 "CA_REQUEST_TYPE_PRODUCTS" 가 fail이 났다.  아직 request 자체가 안 되었으므로 ap에서 다시 request 한다.
	errMsg = CA_EVENT_ERROR;

	//ap에서는 이 callback을 받고, package list가 아직 loadable하지 않다고 알아야 함.
	//질문 : CAK에서 계속 noti가 오는 지 아님 한번 request 한것에 대하여서는 한번만 오는 지 확인 해야 함.
	//둘 중에 하나 일텐데, 서로 구현 방법이 틀림. 계속 오면  loaded noti가 올 때까지 기다려 야 하고,
	//한번만 오면 다시 request를 해야 함.
	for (i = 0; i < NA_MAX_CALLBACK_NUM; i ++)
	{
		if (s_pfnPackageListNofityCallback[i])
		{
			HxLOG_Print("s_pfnPackageListNofityCallback[%d] CALLED.(%d)\n", i, errMsg);
			//ap에서는 이 callback을 받고, 현재 Mediaset TP에 있지 않다고 Error Message를 출력 하여야 함.
			s_pfnPackageListNofityCallback[i](errMsg, NULL,  0, NULL);
		}
		else
		{
			HxLOG_Print("no callback[%d]\n", i);
		}
	}

	HxLOG_Print("EXIT - FAIL\n");

	return errMsg;
}


static TEventStatus local_caPpv_RequestMediasetPackageList(TUnsignedInt32 waitCounter)
{
	TEventStatus 	errMsg = CA_EVENT_SUCCESS;
	TUnsignedInt32	waitTimer = MAX_PDT_LOADED_WAIT_TIME_FOR_PRODUCT;
	TUnsignedInt32	nitWaitTimer;

	HxLOG_Print("ENTER - waitCounter(%ld)\n", waitCounter);

	//현재 Mediaset Package List 이다.
	//CAK impl guide v2.0.4 p46
	// 1. PDT Loaded가 왔는 지 확인 해야 함.
	if (!s_bIsProductsLoaded)
	{
		HxLOG_Print("\t[UIT][INT_UIT_sRequestMediasetPackageList]--> CPDT is NOT avail YET!!!.\n");

		// 2. 현재 MediasetTP 이다.  PDT data가 Load 될 때 까지  MAX 60초 무조건 기다려라.  그 중간이라도 load 되면 그때 request 해라 .
		//CA_PRODUCTS_LOADED_TIME_OUT 이 되면 무조건 그냥 package list를 request해서 무엇이 되던 그 결과를 전달 한다.
		s_eProductLoadFlag = ePRODUCT_LOAD_WAIT_ING;

		nitWaitTimer = NIT_WAIT_TIMER_INTERVAL * waitCounter;
		if (nitWaitTimer > 0)
		{
			//NIT 때문에 기다린 시간은 빼 주어야 한다.
			if (waitTimer > nitWaitTimer)
			{
				waitTimer = MAX_PDT_LOADED_WAIT_TIME_FOR_PRODUCT - nitWaitTimer;
			}
			else
			{
				waitTimer = 0;
			}
		}

		HxLOG_Print("nitWaitTimer (%ld), waitTimer (%ld), call\n", nitWaitTimer, waitTimer);

		// 기존 timer가 있음 없애 버려라..
		if (s_ulProductLoadTimerId != 0)
		{
			HxLOG_Print("Kill Timer - s_ulProductLoadTimerId(%d)\n", s_ulProductLoadTimerId);
			VK_TIMER_Cancel(s_ulProductLoadTimerId);
			s_ulProductLoadTimerId = 0;
		}

		VK_TIMER_EventAfter(waitTimer, local_caPpv_WaitPackageList_TimerCallback, NULL, 0, &s_ulProductLoadTimerId);
		HxLOG_Print("SET Timer for waiting product list loaded call back, s_ulProductLoadTimerId(%ld)!! \n", s_ulProductLoadTimerId);

	}
	else
	{
		HxLOG_Print("CPDT is avail !!!. new go ahead for request Package List!! \n");

		//이제 그냥 수행 하라....
		local_caPpv_RequestPackageDataList(0, NULL);
	}

	HxLOG_Print("EXIT\n");

	return errMsg;
}

static HBOOL local_caPpv_CheckBusinessProductTypeWithProviderID(TPpid providerID, TUnsignedInt8 businessType)
{
	HBOOL bRet = FALSE;

	//MEDIASET이던 아니던 무조건 이 조건은 OK...
	if ((businessType >= ePACKAGE_SPORT ) && (businessType <= ePACKAGE_OTHERS_MORE)) // 1 ~ 6 : OK
	{
		bRet = TRUE;
	}

	//Multi-op 는 PACKAGE_SPECIAL  와 PACKAGE_CHILDREN ~ PACKAGE_ADULT_MORE가 추가 된다.
	if (providerID != NA_MEDIASET_PROVIDER_ID && providerID != 0x101)
	{
		if ((businessType == ePACKAGE_SPECIAL) || // 7 : OK
			((businessType >= ePACKAGE_CHILDREN) && (businessType <= ePACKAGE_ADULT_MORE)) // 11 ~ 14 : OK
		)
		{
			bRet = TRUE;
		}
	}

	return bRet;
}

static HERROR local_caPpv_WaitForGivenTimeWithGivenInterval(UNIXTIME waitInterval, UNIXTIME maxWaitTime)
{
	//  CA_LISTENER_TYPE_PRODUCTS_LOADED 가 안왔을 경우 올때 까지 maxWaitTime, waitInterval으로 기다림.
	//  CA_LISTENER_TYPE_PRODUCTS_LOADING일 경우는 현재 CAK가 memory를 access 하고 있기 때문에 , memory를 access 하면 안된다.
	//  즉, CA_LISTENER_TYPE_PRODUCTS_LOADED가 와야 CAK가 memory를 놓아 준것이라 사용 가능 함.
//	TBoolean	loadable = FALSE;
	UNIXTIME 		timer = 0;

	if ( (maxWaitTime <= 0) || (waitInterval <= 0) )
	{
		HxLOG_Print("EXIT - error (max:%ld, interval:%ld)\n", maxWaitTime, waitInterval);
		return ERR_OK;
	}

        // Nagra stream이 아닐 경우 PDT를 위해 기다릴 필요가 없다.
	if (s_bIsNagraTS == FALSE)
	{
		HxLOG_Print("*** not nagra TS... do not wait!! just return ERR_OK\n");
		return ERR_OK;
	}

	while (timer <= maxWaitTime)
	{
		if (s_bIsProductsLoaded == TRUE)
		{
			HxLOG_Print("CA_LISTENER_TYPE_PRODUCTS_LOADED GOOD!!\n");
			break;
		}
		else
		{
			timer += waitInterval;
			HxLOG_Print("NOT LOADED YET: CA_LISTENER_TYPE_PRODUCTS_LOADED wait [%ldms]\n", timer);
			VK_TASK_Sleep(waitInterval);
		}
	}

	if (timer > maxWaitTime)
	{
		HxLOG_Print("Waited %ld ms BUT NOT LOADED YET!! I CAN't HELP ANYMORE\n", timer);
	}
	else
	{
		HxLOG_Print("Waited %ld ms and FINALLY LOADED !!! \n", timer);
	}

	return ERR_OK;
}

/**************************************************************************************************/
#define ________PACKAGE_LIST____________________________________________
/**************************************************************************************************/

// for check & debugging
#define 	DBG_PACKAGE_LIST_ON 0
#define 	DBG_PACKAGE_LIST_DETAIL 0
static void local_caPpv_PRINT_PACKAGE_LIST(PPV_PACKAGE_LIST *packList)
{
#if DBG_PACKAGE_LIST_ON
	TUnsignedInt32 			index, svcIndex;
	PPV_PACKAGE_LIST_ITEM* 	item;

	if (packList == NULL)
	{
		HxLOG_Print("[*****PPV_PACKAGE_LIST is NULL]\n");
		return;
	}

	HxLOG_Print("[*****PPV_PACKAGE_LIST (item num:%d)]\n", packList->xNbItems);

	CAPPV_PACKAGE_SEM_GET();

	item = packList->pList;

	if (item == NULL)
	{
		HxLOG_Print("[*****PPV_PACKAGE_LIST ALREADY DISTROYED]\n");
		CAPPV_PACKAGE_SEM_RELEASE();
		return;
	}

	for (index = 0; index < packList->xNbItems; index++)
	{
		if (item == NULL)
		{
			HxLOG_Print("[*****PPV_PACKAGE_LIST is NULL]\n");
			break;
		}

		if (item->PackageName)
		{
			HxLOG_Print("\t[%03d] name : %s : pProduct (0x%x) : productID(0x%d) : length(%d)\n", index, item->PackageName, item->pProduct, item->ProductID, item->PackageNameLength);
		}
		HxLOG_Print("\t	cost(%d), start(%04dy %02dm %02dd %02dh %02dm %02ds), end(%04dy %02dm %02dd %02dh %02dm %02ds), svc-num(%d)\n",
			item->Cost, item->PurchaseStartTime.year+1900, item->PurchaseStartTime.month, item->PurchaseStartTime.day, item->PurchaseStartTime.hour, item->PurchaseStartTime.minute,
			item->PurchaseStartTime.second, item->PurchaseEndTime.year+1900, item->PurchaseEndTime.month, item->PurchaseEndTime.day, item->PurchaseEndTime.hour,
			item->PurchaseEndTime.minute, item->PurchaseEndTime.second, item->NumOfServices));

		if  (item->PurchasableFlag & CA_PRODUCT_PURCHASABLE)// BIT0  /**<  Flag to specify: product is purchasable. */
			HxLOG_Print("\t	 PurchaseFlag (%d) : CA_PRODUCT_PURCHASABLE\n", item->PurchasableFlag);

		if  (item->PurchasableFlag & CA_PRODUCT_PURCHASED)// BIT1  /**<  Flag to specify: product has been purchased at least once.*/
			HxLOG_Print("\t	 PurchaseFlag (%d) : CA_PRODUCT_PURCHASED\n", item->PurchasableFlag);

		if  (item->PurchasableFlag & CA_PRODUCT_NOT_LOADED)// BIT2  /**<  Flag to specify: details on this product are not completely loaded.*/
			HxLOG_Print("\t	 PurchaseFlag (%d) : CA_PRODUCT_NOT_LOADED\n", item->PurchasableFlag);

		if  (item->PurchasableFlag & CA_PRODUCT_IMPULSIVE)// BIT3  /**<  Flag to specify: product can be purchased impulsively.*/
			HxLOG_Print("\t	 PurchaseFlag (%d) : CA_PRODUCT_IMPULSIVE\n", item->PurchasableFlag);

		for (svcIndex = 0; svcIndex < item->NumOfServices; svcIndex++)
		{
			HxLOG_Print("\t	 [%02d] service onID(%d), tsID(%d), serviceID(%d) \n", svcIndex,
				item->pServices[svcIndex].originalNetworkId, item->pServices[svcIndex].transportStreamId, item->pServices[svcIndex].serviceId));
		}

		if (item->NumOfServices <= 0)
		{
			HxLOG_Print("\t	 [%d] service  NOT AVAIL(0)!!! \n", index);
		}

#if 	DBG_PACKAGE_LIST_DETAIL
		//Package Info print
		{
			PACKAGE_INFO packageInfo;

			HxSTD_memcpy(&packageInfo, &item->PackageInfo, sizeof(PACKAGE_INFO));
			HxLOG_Print("\t	 [%d]PackageINFO:  ", index);
			if (packageInfo.pRequest)
			{
				HxLOG_Print("pRequest (0x%x) ", packageInfo.pRequest);
			}
			if (packageInfo.pProduct)
			{
				HxLOG_Print(": pProduct (0x%x) ", packageInfo.pProduct);
			}
			if (packageInfo.pServiceFilter)
			{
				HxLOG_Print(": pServiceFilter (0x%x) ", packageInfo.pServiceFilter);
			}
			if (packageInfo.nDesSize > 0)
			{
				HxLOG_Print("\t nDesSize(%d), DESC:(%s)\n",packageInfo.nDesSize, packageInfo.aucDescription);
			}
			else
			{
				HxLOG_Print(" : nDesSize(%d)\n", packageInfo.nDesSize);
			}
			HxLOG_Print(", ulNbEvents:(%d)\n", packageInfo.ulNbEvents);

			//xNbEvents를 믿는당.....^^
			for (svcIndex = 0; svcIndex < packageInfo.ulNbEvents; svcIndex++)
			{
				//그래도 혹시 NULL 확인...
				EVENT_ITEM *holder;

				holder = &(packageInfo.pEvents[svcIndex]);

				if ( holder != NULL)
				{

					HxLOG_Print("\t\t	[%d] EVENT:nameLength(%d), pStartTime: %d:%d:%d  %d:%d:%d\n", svcIndex, holder->EvtNameLength, holder->pStartTime.year,
						holder->pStartTime.month, holder->pStartTime.day, holder->pStartTime.hour,holder->pStartTime.minute,holder->pStartTime.second);
					HxLOG_Print("\t\t		Name:(%s)\n", holder->EvtName);
				}
				else
				{
					HxLOG_Print("\t\t  [%d] EVENT NULL!!! \n", svcIndex);
				}
			}

			if (packageInfo.nDisclaimerSize > 0)
			{
				HxLOG_Print("\t 	DISCLAIMER:(size:%d):(%s)\n", packageInfo.nDisclaimerSize, packageInfo.aucDisclaimer);
			}
			else
			{
				HxLOG_Print("\t 	DISCLAIMER: size == %d\n", packageInfo.nDisclaimerSize);
			}
		}
		else
		{
			HxLOG_Print("\t	[%d] PACKAGE INFO NOT AVAIL!!! \n", index);
		}
#endif

		item = item->pNext;
	}

	CAPPV_PACKAGE_SEM_RELEASE();

	HxLOG_Print("[******************** END ********************]\n");
#endif
}


static void local_caPpv_ResponsePackageList(TCaRequest* pRequest)
{
	//CAK에서 PackgaeList call back이 전달 되어 왔다. //CAK_UIT_PACKAGE_LIST_CALL_BACK
	//항상 package_list의 MW_CA_NA_DisposePackageList가 asynch라 불렸을 경우를 대비 하여 NULL을 확인 하여야 한다.
	//menu에서 계속 package list request <-> dispose를 불렀을 경우

	TCaRequestStatus 	reqStatus;
	TCaObjectStatus 	objStatus = CA_OBJECT_NO_ERROR, svcObjStatus = CA_OBJECT_NO_ERROR;
	TCaProcessingStatus procStatus;
	TUnsignedInt16 		index, serviceIndex;
	TUnsignedInt32 		packageNb;
	TCaProduct			**ppPackage;
	TCaService			**ppServices;
	TEventStatus 		result;
	TUnsignedInt16		i = 0;

	HxLOG_Print("ENTER - pRequest(%ld), s_stPackage_list.pRequest(%ld)\n", pRequest, s_stPackageList.pRequest);

	//call back이 진짜 현재 원하는 것인지 확인... 이전 request에 의한 것이면 버려라....
	CAPPV_PACKAGE_SEM_GET();
	if ((pRequest == NULL) || ((s_stPackageList.pRequest != NULL) && s_stPackageList.pRequest != pRequest))
	{
		HxLOG_Print("EXIT - pRequest is changed\n");

		CAPPV_PACKAGE_SEM_RELEASE();
		return;
	}
	CAPPV_PACKAGE_SEM_RELEASE();

	//내가 원하는 package_list가 맞다.. 현재까지는 ...두번이 올 수 없으므로, 여기서 package_list를 clear 시켜도 된다.
	//자 이제 부터 새로운 package_list 구조를 만든다..
	local_caPpv_InitPackageList(&s_stPackageList, FALSE);

	//여기서 부터는 package_list.pRequest = pRequest 이다.
	//Get  business type
	CAPPV_PACKAGE_SEM_GET();
	reqStatus = caRequestGetObjects(pRequest, &packageNb, &ppPackage);
	HxLOG_Print("packageNb = %d\n", packageNb);
	CAPPV_PACKAGE_SEM_RELEASE();

	if ((reqStatus == CA_REQUEST_NO_ERROR) && (packageNb > 0))
	{
		PPV_PACKAGE_LIST_ITEM	*holder;
		HERROR 				hErr;
		TPpid 				providerID;

		hErr = CA_NA_PpvGetProviderId(&providerID);
		if (hErr != ERR_OK)
		{
			HxLOG_Print("can not get providerID from card\n");
			reqStatus = CA_REQUEST_INVALID;
			goto notiToAP;
		}

		for(index = 0; index<packageNb; index++)
		{
			TChar				*pPackageName;
			TUnsignedInt8		businessType;
			PPV_PACKAGE_LIST_ITEM	*traveler = NULL;
			HBOOL				bRet;

			//Get  business type
			CAPPV_PACKAGE_SEM_GET();
			if (s_stPackageList.pRequest != NULL) //CA_NA_DisposePackageList가 asynch라 불렸을 경우 대비.
			{
				objStatus = caProductGetBusinessType(ppPackage[index], &(businessType));
				bRet = local_caPpv_CheckBusinessProductTypeWithProviderID(providerID, businessType);
				if ((objStatus != CA_OBJECT_NO_ERROR) || bRet != TRUE)
				{
					HxLOG_Print("ppPackage[%d] ignore - providerID[0x%x], Bussiness Type[%d], objStatus[%d], bRet[%d]\n", index, providerID, businessType, objStatus, bRet);
					CAPPV_PACKAGE_SEM_RELEASE();
					continue;
				}

			}
			else
			{
				HxLOG_Print("EXIT - s_stPackageList.pRequest canceled\n");
				CAPPV_PACKAGE_SEM_RELEASE();
				return;
			}
			CAPPV_PACKAGE_SEM_RELEASE();

			HxLOG_Print("ppPackage[%d] create PPV_PACKAGE_LIST_ITEM - Bussiness Type[%d]\n", index, businessType);

			traveler = (PPV_PACKAGE_LIST_ITEM*)OxCAS_Malloc(sizeof(PPV_PACKAGE_LIST_ITEM));
			if (traveler == NULL)
			{
				HxLOG_Print("ppPackage[%d] traveler OxCAS_Malloc FAIL.... notiToAP!!...\n", index);
				reqStatus = CA_REQUEST_NO_MORE_RESOURCES;
				goto notiToAP;
			}

			//now check rest of items.. traveler는 만들어 졌다.
			CAPPV_PACKAGE_SEM_GET();
			if (s_stPackageList.pRequest != NULL)
			{
				//일단 만들어져 traveler를 package_list에 달아라...  이 traveler는 init package list에서 지울 것이다.
				s_stPackageList.xNbItems ++;

				//clear new memory
				HxSTD_memset(traveler, 0, sizeof(PPV_PACKAGE_LIST_ITEM));

				// Product handler 저장
				traveler->pProduct = ppPackage[index];
				traveler->pRequest = s_stPackageList.pRequest;
				traveler->BusinessType = businessType;

				objStatus |= caProductGetId(traveler->pProduct, &(traveler->ProductID));
				HxLOG_Print("ppPackage[%d] ProductID, objStatus(%d)\n", traveler->ProductID, objStatus);

				// PDT load check
				local_caPpv_WaitForGivenTimeWithGivenInterval(MAX_PDT_LOADED_WAIT_TIME_INTERVAL, MAX_PDT_LOADED_WAIT_TIME_FOR_PRODUCT);

				// package list도 product name을 사용
				objStatus   |= caProductGetName(traveler->pProduct, &(traveler->PackageNameLength), &pPackageName);

				if (pPackageName)
				{
					HxLOG_Print("ppPackage[%d], length(%d), status(%d), xNbItems[%d]\n", index, traveler->PackageNameLength, objStatus, s_stPackageList.xNbItems);
				}

				if ((objStatus == CA_OBJECT_NO_ERROR) && ( traveler->PackageNameLength > 0))
				{
					if (traveler->PackageNameLength <= MAX_PACKAGE_NAME_LENGTH)
					{
						MWC_UTIL_DvbStrncpy(traveler->PackageName, pPackageName, traveler->PackageNameLength);
					}
					else
					{
						traveler->PackageNameLength = MAX_PACKAGE_NAME_LENGTH;
						MWC_UTIL_DvbStrncpy(traveler->PackageName, pPackageName, MAX_PACKAGE_NAME_LENGTH - 3);
						MWC_UTIL_DvbStrcat(traveler->PackageName, "...");
					}

					if (pPackageName)
					{
						HxLOG_Print("ppPackage[%d] package name[%s]\n", index, pPackageName);
					}
				}
				else
				{
					HxLOG_Print("ppPackage[%d] no package name avail....objStatus(%d)\n", index, objStatus);
				}

				objStatus |= caProductGetCost(ppPackage[index], &(traveler->Cost));
				HxLOG_Print("ppPackage[%d] Cost[%d], objStatus(%d)\n", index, traveler->Cost, objStatus);

				objStatus |= caProductGetPurchaseStartTime(ppPackage[index], &(traveler->PurchaseStartTime));
				HxLOG_Print("ppPackage[%d] objStatus(%d)\n", index, objStatus);

				objStatus |= caProductGetPurchaseEndTime(ppPackage[index], &(traveler->PurchaseEndTime));
				HxLOG_Print("ppPackage[%d] objStatus(%d)\n", index, objStatus);

				objStatus |= caProductGetFlags (ppPackage[index], &(traveler->PurchasableFlag));
				HxLOG_Print("ppPackage[%d] PurchasableFlag[%d], objStatus(%d)\n", index,traveler->PurchasableFlag, objStatus);

				//위의 것이 error가 나던 말던, 다시 service가 문제가 되니까...
				svcObjStatus = caProductGetServices(ppPackage[index], &(traveler->NumOfServices), &(ppServices));
				HxLOG_Print("ppPackage[%d] NumOfServices[%d], objStatus(%d)\n", index, traveler->NumOfServices, objStatus);

				if (svcObjStatus == CA_OBJECT_NO_ERROR)
				{
					traveler->pServices = OxCAS_Malloc(traveler->NumOfServices *sizeof(TDvbTriplet));

					// INIT package_list.pList;
					HxSTD_memset(traveler->pServices, 0x00, traveler->NumOfServices*sizeof(TDvbTriplet));

					for (serviceIndex = 0; serviceIndex < traveler->NumOfServices; serviceIndex++)
					{
						objStatus |= caServiceGetLocator(ppServices[serviceIndex], &(traveler->pServices[serviceIndex]));
						HxLOG_Print("ppPackage[%d] service index[%d], status(%d), locator[onID:0x%x][tsID:0x%x][svcID:0x%x]\n", index, serviceIndex, objStatus,
							traveler->pServices[serviceIndex].originalNetworkId, traveler->pServices[serviceIndex].transportStreamId, traveler->pServices[serviceIndex].serviceId));
					}
					HxLOG_Print("ppPackage[%d] END for Service List.....\n", index);
				}
				else if (svcObjStatus == CA_OBJECT_NOT_AVAILABLE)
				{
					traveler->NumOfServices = 0; //이 경우 AP에서는 해당 package를 보여 주어야 한다.
					objStatus |= CA_OBJECT_NO_ERROR; //이렇게 하여야 밑에서 CA_EVENT_SUCCESS를 전달 해 준다.
					HxLOG_Print("ppPackage[%d] END for Service List....CA_OBJECT_NOT_AVAILABLE.\n", index);

				}
				else
				{
					//distory를 시키나?  건 아닌 것 같고...^^ 뭐 하나가 없을 수도 있으니.. 하여간 하나라도 fail이면 package_list에 추가를 안 시킨다.
					HxLOG_Print("ppPackage[%d] Fail for Service List.... objStatus(%d)\n", index, objStatus);
					objStatus |= svcObjStatus;
				}

				//add new node to package_list
				traveler->pNext = NULL;

				if (s_stPackageList.xNbItems == 1)
				{
					s_stPackageList.pList = traveler;
					holder = s_stPackageList.pList;
				}
				else
				{
					holder->pNext = traveler;
					holder = holder->pNext;
				}
			}
			else
			{
				HxLOG_Print("s_stPackageList.pRequest canceled....\n");
				if (traveler != NULL)
				{
					OxCAS_Free(traveler);
					traveler = NULL;
				}
				CAPPV_PACKAGE_SEM_RELEASE();

				return;
			}

			CAPPV_PACKAGE_SEM_RELEASE();

		}

		HxLOG_Print("END for Product List......\n");
	}
	else if ((reqStatus == CA_REQUEST_NO_ERROR) && (packageNb == 0))
	{
		HxLOG_Print("1 ERROR  caRequestGetObjects, status(%d), packageNb(%d)......  \n", reqStatus, packageNb);
		objStatus = CA_OBJECT_NO_ERROR;
	}
	else
	{
		HxLOG_Print("2 ERROR  caRequestGetObjects, status(%d)......\n", reqStatus);
	}
	// 여기서 free 시키면 안된다.  왜냐면, product list는 purchase가 끝날 때 까지 dispose 시키면 안된다.
	//CAK6_I implementation guide ver2.0.2 p18, 3.6"this is important not disposing of the product list before the purchase is complete.  Otherwise, the
	//memory allocated for the product list request will be freeed and caProduct object will no longer be available : caRequestDispose(pRequest);

	notiToAP:

	HxLOG_Print("[notiToAP]1 NOW,  try to notiToAP......reqStatus(%d), objStatus(%d) \n", reqStatus, objStatus);

	if ((reqStatus == CA_REQUEST_NO_ERROR) && (objStatus == CA_OBJECT_NO_ERROR))
	{
		result = CA_EVENT_SUCCESS;
	}
	else//ERR 가 발생 했다.
	{
		result = CA_EVENT_ERROR;
	}


	local_caPpv_PRINT_PACKAGE_LIST(&s_stPackageList);	// only for debugging


	//ap로 보낸다.
	if ( s_stPackageList.pRequest != NULL)
	{
		for (i = 0; i < NA_MAX_CALLBACK_NUM; i ++)
		{
			if (s_pfnPackageListNofityCallback[i])
			{
				HxLOG_Print("notiToAP[%d] - result(%d), s_stPackageList.pRequest(%ld), s_stPackageList.xNbItems(%ld)\n", i, reqStatus, s_stPackageList.pRequest, s_stPackageList.xNbItems);
				//ap에서는 이 callback을 받고, 현재 Mediaset TP에 있지 않다고 Error Message를 출력 하여야 함.
				s_pfnPackageListNofityCallback[i](result, s_stPackageList.pRequest, s_stPackageList.xNbItems, (void *)s_stPackageList.pList);
			}
			else
			{
				HxLOG_Print("no callback[%d]\n", i);
			}
		}
	}

	HxLOG_Print("EXIT - Product list num[%d], final result reqStatus(%d)\n", s_stPackageList.xNbItems, reqStatus);

}

static void local_caPpv_WaitPackageList_TimerCallback(unsigned long nTimerId,  void *pArg)
{
	TSignedInt32	nErrCode;
	NaCasMsg_t 	 	stMsg;

	if (s_ulProductLoadTimerId != nTimerId)
	{
		return;
	}

	stMsg.wMsg = NA_CAK_ID;
	stMsg.lParam2 = CAK_PPV_PACKAGE_TIMER_CALL_BACK;
	stMsg.info = (TUnsignedInt32)nTimerId;
	stMsg.hWnd = (TUnsignedInt32)pArg;

	HxLOG_Info("TimerId(%d), s_ulProductLoadTimerId(%d)\n", nTimerId, s_ulProductLoadTimerId);

	nErrCode = VK_PostMessageTimeout(s_ulPpvQueueId, (void *)&stMsg, QUEUE_SIZE_1);
	if(nErrCode)
	{
		HxLOG_Info("error in VK_PostMessage (0x%x)\n", (unsigned int)nErrCode);
	}
}

static void local_caPpv_PackageListCallback(TCaRequest* pxRequest)
{
	TSignedInt32	nErrCode;
	NaCasMsg_t 	 	stMsg;

	HxLOG_Print("ENTER\n");

	stMsg.wMsg = NA_CAK_ID;
	stMsg.lParam2 = CAK_PPV_PACKAGE_LIST_CALL_BACK;
	stMsg.lParam1 = 0;
	stMsg.hWnd = pxRequest;

	nErrCode = VK_PostMessage(s_ulPpvQueueId, (void *)&stMsg, QUEUE_SIZE_1);
	if (nErrCode)
	{
		HxLOG_Info("error in VK_PostMessage (0x%x)\n", (unsigned int)nErrCode);
	}

	HxLOG_Print("Exit\n");
}

static void local_caPpv_PresentEventInfoCallback(TCaRequest* pxRequest)
{
	TSignedInt32	nErrCode;
	NaCasMsg_t 	 	stMsg;

	HxLOG_Print("ENTER\n");

	stMsg.wMsg = NA_CAK_ID;
	stMsg.lParam2 = CAK_REQUEST_PRESENT_EVENT_CALL_BACK;
	stMsg.hWnd = pxRequest;
	stMsg.lParam3 = 0;

	nErrCode = VK_PostMessage(s_ulPpvQueueId, (void *)&stMsg, QUEUE_SIZE_1);
	if (nErrCode)
	{
		HxLOG_Info("error in VK_PostMessage (0x%x)\n", (unsigned int)nErrCode);
	}

	HxLOG_Print("Exit\n");
}

static void local_caPpv_FollowingEventInfoCallback(TCaRequest* pxRequest)
{
	TSignedInt32	nErrCode;
	NaCasMsg_t 	 	stMsg;

	HxLOG_Print("ENTER\n");

	stMsg.wMsg = NA_CAK_ID;
	stMsg.lParam2 = CAK_REQUEST_FOLLOWING_EVENT_CALL_BACK;
	stMsg.hWnd = pxRequest;
	stMsg.lParam3 = 0;

	nErrCode = VK_PostMessage(s_ulPpvQueueId, (void *)&stMsg, QUEUE_SIZE_1);
	if (nErrCode)
	{
		HxLOG_Info("error in VK_PostMessage (0x%x)\n", (unsigned int)nErrCode);
	}

	HxLOG_Print("Exit\n");
}

static void local_caPpv_NotifyNITWaitTimeoutForPackageList(TUnsignedInt32 waitCounter, TPpid providerID)
{
	TEventStatus errMsg = CA_EVENT_SUCCESS;
	TUnsignedInt16	i = 0;

	//NIT가 오지 않아서 waitCounter 만큼  기다렸다..
	//여기서 다시 한번 PMT를 확인 해야 함

	HxLOG_Print("ENTER - waitCounter(%d), providerID(0x%x)\n", waitCounter, providerID);

	// 1. 현재 시점에서 Nagra 인지 확인
	if (!s_bIsNagraTS)
	{
		HxLOG_Print("Not Nagra TS...send CA_EVENT_TS_FAIL to callback\n");
		goto errNotiToAP;
	}
	else
	{
		//providerID로 구별.
		// 2. 현재 NagraTP인지 확인 됨 : NIT와 PDT(CPDT/HPDT) 확인.  이제 PMT.conditional_access_descriptor  : CA_descriptor.CA_System_ID를 확인 하라..
		if (providerID == NA_MEDIASET_PROVIDER_ID)
		{
			// 2.1 Mediaset Packgage List request.. PMT 확인
			HERROR			hErr = ERR_OK;
			TUnsignedInt8 	numOfCADesc = 0;
			HBOOL			result;
			CUR_NIT_INFO* 	sCurNITInfo;

			HxLOG_Print("check NA_MEDIASET_PROVIDER_ID(0x%x)\n", providerID);

			if (s_hCurSvc != HANDLE_NULL)
			{
				result = local_caPpv_CheckGivenProviderIDInPMT(&numOfCADesc, (TUnsignedInt16)NA_MEDIASET_PROVIDER_ID);
				HxLOG_Print("local_caPpv_CheckGivenProviderIDInPMT - result(%d), numOfCADesc(%d)\n", result, numOfCADesc);

				sCurNITInfo = local_caPpv_GetNITArrivedFlag();

				//result가 true이거나, numOfCADesc가 0 (Free-to-air:clear 구간) 일 경우, true return
				if (((result == TRUE) && (sCurNITInfo->orgNetId == MEDIASET_PREMIUM_ONID)) || (numOfCADesc == 0))
				{
					HxLOG_Print("Mediaset CA SystemID FOUND in PMT!!\n");
					goto requestPackageList;
				}
				else
				{
					//Mediaset package list를 request 하였는데, PMT에서 Mediaset CA System ID를 못 찾았다. error reurn...
					HxLOG_Print("No Mediaset CA SysemID in PMT return ERROR!!\n");
					goto errNotiToAP;
				}

			}
			else
			{
				//service Handle 자체를 못 가져 온다. error....Humm... 무조건 error reurn...
				HxLOG_Print("CAN't Be here...No svcHandle return ERROR\n");
				goto errNotiToAP;
			}

		}
		else
		{
			// 2.2 Multi-operator 고려 : Mediaset이 아닌 다른 provider... 무조건 그냥 request 하라....
			HxLOG_Print("non-mediaset provider package list has been requested!!\n");
			goto requestPackageList;
		}

	} // else ( if (!NIT_UIT_getNagraTS)


	errNotiToAP:

		errMsg = CA_EVENT_TS_FAIL;

	for (i = 0; i < NA_MAX_CALLBACK_NUM; i ++)
	{
		if (s_pfnPackageListNofityCallback[i])
		{
			HxLOG_Print("CUR TP is NOT MediasetTP, give Noti to callback[%d](%d)\n", i, errMsg);
			//ap에서는 이 callback을 받고, 현재 Mediaset TP에 있지 않다고 Error Message를 출력 하여야 함.
			s_pfnPackageListNofityCallback[i](errMsg, NULL,  0, NULL);
		}
		else
		{
			HxLOG_Print("CUR TP is NOT MediasetTP, no callback[%d]\n", i);
		}
	}

		HxLOG_Print("EXIT - CA_EVENT_TS_FAIL\n");

		return;


	requestPackageList:

		local_caPpv_RequestMediasetPackageList(waitCounter);
		HxLOG_Print("EXIT - request package list\n");

		return;

}

static TEventStatus local_caPpv_RequestPackageList(TUnsignedInt32 ulProviderId)
{
	//AP에서 CAK_UIT_REQUEST_PACKAGE_LIST를 uitask에 전달 하면 uitask에서 call 함.
	TCaRequestStatus 	status;
	TCaRequest			*pRequest;
	TEventStatus 		errMsg = CA_EVENT_SUCCESS;
	CUR_NIT_INFO		*sCurNITInfo;
	TUnsignedInt16		i = 0;
	NIT_WAIT_ITEM		stNitItem;

	HxLOG_Print("Enter - ulProviderId = 0x%x\n", ulProviderId);

	sCurNITInfo = local_caPpv_GetNITArrivedFlag();
	if (!s_bIsNagraTS)
	{
		if (sCurNITInfo->bArrived == TRUE) //NIT arrived....
		{
			HxLOG_Print("CPDT is NOT EXIT after NIT arrived!!, INT_UIT_getNagraTS(%d)\n", s_bIsNagraTS);

			// NIT가 들어 왔는데, NA PPV TS가 아니다.
			errMsg = CA_EVENT_TS_FAIL;
			for (i = 0; i < NA_MAX_CALLBACK_NUM; i ++)
			{
				if (s_pfnPackageListNofityCallback[i])
				{
					HxLOG_Print("callback[%d] exit, send to call back...ERR(%d) \n", i, errMsg);

					//ap에서는 이 callback을 받고, 현재 NA PPV TP에 있지 않다고 Error Message를 출력 하여야 함.
					s_pfnPackageListNofityCallback[0](errMsg, NULL,  0, NULL);
				}
				else
				{
					HxLOG_Print("no call back[%d] to give noti...\n", i);
				}
			}

			HxLOG_Print("EXIT - error(%d)\n", errMsg);

			return CA_EVENT_ERROR;
		}
		else
		{
			HxLOG_Print("SET Timer for waiting NIT to be ARRIVED!!, ! \n");
			stNitItem.eOwner = eNIT_WAIT_OWNER_MENU;
			stNitItem.flag = eNIT_WAIT_INIT;
			stNitItem.provider = (TPid)ulProviderId;

			HxLOG_Print("timerID(%d), who:(%d), providerID(0x%x)\n", s_ulNITwaitTimerId, eNIT_WAIT_OWNER_MENU, ulProviderId);
			local_caPpv_CheckNIT(0 ,(void*)&stNitItem);
			return CA_EVENT_SUCCESS;
		}
	}
	else
	{
		// 2. 현재 Nagra PPV TP인지 확인 됨 : NIT와 PDT(CPDT/HPDT) 확인.  이제 PMT.conditional_access_descriptor  : CA_descriptor.CA_System_ID를 확인 하라..

		// 2.1 Given Packgage List request.. PMT 확인
		TUnsignedInt8 	numOfCADesc = 0;
		TBoolean		bResult;

		HxLOG_Print("check PROVIDER_ID (given providerID=0x%x)!!\n", ulProviderId);

		if (s_hCurSvc != HANDLE_NULL)
		{
			HxLOG_Print("call CAI_PMT_checkGivenProviderIDInPMT(curSvcHandle:0x%x)!!\n", s_hCurSvc);

			bResult = local_caPpv_CheckGivenProviderIDInPMT(&numOfCADesc, (TUnsignedInt16)NA_MEDIASET_PROVIDER_ID);
			HxLOG_Print("local_caPpv_CheckGivenProviderIDInPMT - result(%d), numOfCADesc(%d)\n", bResult, numOfCADesc);

			//result가 true이거나, numOfCADesc가 0 (Free-to-air:clear 구간) 일 경우, true return
			if (((ulProviderId == NA_MEDIASET_PROVIDER_ID) && (bResult == TRUE) && (sCurNITInfo->orgNetId == MEDIASET_PREMIUM_ONID)) || (numOfCADesc == 0))
			{
				HxLOG_Print("Mediaset requested, Mediaset CA SystemID FOUND in PMT!!\n");
				goto requestPackageList;
			}
			else if (((ulProviderId == NA_OTHER_PROVIDER_ID) && (bResult == TRUE)) || (numOfCADesc == 0))
			{
				HxLOG_Print("Multi-OP requested, Multi-OP CA SystemID FOUND in PMT!!\n");
				goto requestPackageList;
			}
			else if (ulProviderId == NA_FAKE_PROVIDER_ID)
			{
				HxLOG_Print("NA_FAKE_PROVIDER_ID  ask CAK package list!!\n");
				goto requestPackageList;
			}
			else
			{
				// PMT에서 원하는 CA System ID를 못 찾았다. error reurn...
				HxLOG_Print("Requested , not no Mediaset CA SysemID in PMT reutnr ERROR!!\n");
			}
		}
		else
		{
			//service Handle 자체를 못 가져 온다. error....Humm... 무조건 error reurn...
			HxLOG_Print("CAN't Be here...No svcHandle return ERROR!! \n");
		}

		// PMT 관련 확인 ERROR
		errMsg = CA_EVENT_TS_FAIL;
		for (i = 0; i < NA_MAX_CALLBACK_NUM; i ++)
		{
			if (s_pfnPackageListNofityCallback[i])
			{
				HxLOG_Print("callback[%d] exit, send to call back...ERR(%d) \n", i, errMsg);

				//ap에서는 이 callback을 받고, 현재 NA PPV TP에 있지 않다고 Error Message를 출력 하여야 함.
				s_pfnPackageListNofityCallback[0](errMsg, NULL,  0, NULL);
			}
			else
			{
				HxLOG_Print("no call back[%d] to give noti...\n", i);
			}

		}
		HxLOG_Print("EXIT!! TP doen't fit the ruquirement\n");

		return CA_EVENT_ERROR;
	}

requestPackageList:
	//Now NagraTS인것이 확인 되었다.  또한, multi provider 관련도 확인 되었다.  정말 request Package list를 해라.
	local_caPpv_RequestMediasetPackageList(0);//waitTime ==0;

	HxLOG_Print("EXIT\n");

	return CA_EVENT_SUCCESS;
}


/**************************************************************************************************/
#define ________NIT_CHECK____________________________________________
/**************************************************************************************************/

static void local_caPpv_SetNITArrivedFlag(TBoolean flag, TUnsignedInt16 orgNetId)
{
	HxLOG_Print("flag(%d), orgNetId(0x%x)--->ENTER!!\n", flag, orgNetId);
 	s_stNitArrivedInfo.bArrived = flag;
	s_stNitArrivedInfo.orgNetId = orgNetId;
}

static CUR_NIT_INFO* local_caPpv_GetNITArrivedFlag(void)
{
	return &s_stNitArrivedInfo ;
}

static void local_caPpv_WaitNIT_TimerCallback(unsigned long TimerId,  void *pArg)
{
	TSignedInt32	nErrCode;
	NIT_WAIT_ITEM 	stNitItem;
	NaCasMsg_t 	 	stMsg;

	HxLOG_Print("ENTER - TimerId = %d, s_ulNITwaitTimerId = %d\n", TimerId, s_ulNITwaitTimerId);

	if (s_ulNITwaitTimerId != TimerId)
	{
		return;
	}


	if ((pArg == NULL) || (TimerId == 0))
	{
		HxLOG_Print("params is NULL or TimerID is NULL\n");
		return;
	}

	stNitItem.eOwner = ((NIT_WAIT_ITEM*)pArg)->eOwner;
	stNitItem.flag = ((NIT_WAIT_ITEM*)pArg)->flag;
	stNitItem.provider = ((NIT_WAIT_ITEM*)pArg)->provider;

	stMsg.wMsg = NA_CAK_ID;
	stMsg.lParam2 = CAK_PPV_NIT_TIMER_CALL_BACK;
	stMsg.hWnd = (TUnsignedInt32)TimerId;
	stMsg.lParam1 = stNitItem.eOwner; // 여기서 부터 3 인자는 임의로 사용
	stMsg.info = stNitItem.flag;
	stMsg.lParam3 = stNitItem.provider;

	nErrCode = VK_PostMessageTimeout(s_ulPpvQueueId, (void *)&stMsg, QUEUE_SIZE_1);
	if (nErrCode)
	{
		HxLOG_Info("error in VK_PostMessage (0x%x)\n", (unsigned int)nErrCode);
	}

	HxLOG_Print("Exit\n");
}

static void local_caPpv_CheckNIT(TUnsignedInt32 ulTimerId, void* pArg)
{
	static TUnsignedInt32 	waitCounter = 0;
	CUR_NIT_INFO*			sCurNitInfo;
	NIT_WAIT_ITEM 			stNitItem;

	//Wait NIT는 중복으로 걸릴 수 없다. 처음 nit wait timer 를 setting 한 곳에서 사용 하고 난 후는 꼭 필요 없을 시 cancel을 시켜 주어야 한다.
	HxLOG_Print("- ENTER - ulTimerId(%d), s_ulNITwaitTimerId(%d)\n", ulTimerId, s_ulNITwaitTimerId);

	stNitItem.eOwner = ((NIT_WAIT_ITEM*)pArg)->eOwner;
	stNitItem.flag = ((NIT_WAIT_ITEM*)pArg)->flag;
	stNitItem.provider = ((NIT_WAIT_ITEM*)pArg)->provider;
	HxLOG_Print("eOwner(%d), providerID(0x%x) flag (%d) ulTimerId(0x%x)\n", stNitItem.eOwner, stNitItem.provider, stNitItem.flag, ulTimerId);

	sCurNitInfo = local_caPpv_GetNITArrivedFlag();
	if ((stNitItem.eOwner == eNIT_WAIT_OWNER_NONE) && (stNitItem.flag == eNIT_WAIT_INIT))
	{
		//set wait NIT timer to init state
		s_eNitWaitFlag = eNIT_WAIT_INIT;
		waitCounter = 0;
		HxLOG_Print("- CANCEL - waitCounter(%d)\n", waitCounter);
		if (s_ulNITwaitTimerId && (ulTimerId != s_ulNITwaitTimerId))
		{
			VK_TIMER_Cancel(s_ulNITwaitTimerId);
			s_ulNITwaitTimerId = 0;
		}
		return;
	}

	if ((sCurNitInfo->bArrived != TRUE) && (waitCounter < MAX_NIT_WAIT_COUNTER))
	{
		s_eNitWaitFlag = eNIT_WAIT_ING;
		HxLOG_Print("- WAIT - waitCounter(%d)\n", waitCounter);

		//이전 timer가 아직 남아 있으면 kill timer
		if (s_ulNITwaitTimerId && (ulTimerId != s_ulNITwaitTimerId))
		{
			VK_TIMER_Cancel(s_ulNITwaitTimerId);
			s_ulNITwaitTimerId = 0;
		}
		VK_TIMER_EventAfter(NIT_WAIT_TIMER_INTERVAL, local_caPpv_WaitNIT_TimerCallback, &stNitItem, sizeof(NIT_WAIT_ITEM), &s_ulNITwaitTimerId);
		HxLOG_Print("s_ulNITwaitTimerId = %d\n", s_ulNITwaitTimerId);
		waitCounter++;
		return;
	}
	else
	{
		HxLOG_Print("WAIT END..... waitCounter(%d), set wait flag to NIT_WAIT_DONE\n", waitCounter);

		//드디어 NIT가 오거나, 10번의 retry가 끝났다  (CA_NIT_MAX_WAIT_COUNTER), 더 이상 안 기다린다.
		s_eNitWaitFlag = eNIT_WAIT_DONE;

		//기다리는 곳으로 다시 돌아 간다.
		if (stNitItem.eOwner == eNIT_WAIT_OWNER_MENU)
		{
			local_caPpv_NotifyNITWaitTimeoutForPackageList(waitCounter, stNitItem.provider);
		}
		else if (stNitItem.eOwner == eNIT_WAIT_OWNER_IPPV)
		{
			if (s_hCurSvc != HANDLE_NULL)
			{
				local_caPpv_ExecuteTsCheck(s_hCurSvc, eNIT_WAIT_DONE);
			}
			else
			{
				HxLOG_Print(" error s_hCurSvc HANDLE_NULL\n");
				if (s_pfnTSCheckNotifyCallback)
				{
					s_pfnTSCheckNotifyCallback(NULL, TS_PERIOD_ERROR, FALSE);
				}
			}
		}
		else
		{
			HxLOG_Print("who(%d)...plese implement whatever you want...\n", stNitItem.eOwner);
		}
		waitCounter = 0;
		HxLOG_Print("- EXIT - waitCounter(%d)\n", waitCounter);
		return;
	}

}

/******************************************************
  * local_caPpv_CheckGivenProviderIDInPMT :
  * input : providerID는 Mediaset과 Multi-op두개로 한정 됨.
  * output : 주어진 providerID가 PMT에 있으면 return TRUE, return FALSE otherwise
  *
  *******************************************************/
static HBOOL local_caPpv_CheckGivenProviderIDInPMT(TUnsignedInt8 *numOfCADesc, const TUnsignedInt16 providerID)
{
	TUnsignedInt32 	i = 0, j = 0;
	SvcCas_PmtInfo_t 	*pstPmtInfo = NULL;

	HxLOG_Print("Enter - providerID(0x%x)\n", providerID);

	if (numOfCADesc == NULL)
	{
		HxLOG_Print("svcHandle or numOfCADesc is NULL\n");
		return FALSE;
	}

	pstPmtInfo = svc_cas_MgrSubGetPmtInfo(s_hCurAction);
	if (!pstPmtInfo)
	{
		HxLOG_Print("stPmtInfo is NULL\n");
		return FALSE;
	}

	HxLOG_Print("pstPmtInfo->ulNumOfProgramCaDescriptor = %d\n", pstPmtInfo->ulNumOfProgramCaDescriptor);
	*numOfCADesc = (TUnsignedInt8)pstPmtInfo->ulNumOfProgramCaDescriptor;

	for (i = 0; i < pstPmtInfo->ulNumOfProgramCaDescriptor; i++)
	{
		HxLOG_Print("pstPmtInfo->stProgramCaDescriptor[%d].usCaSysId = 0x%x\n", i, pstPmtInfo->stProgramCaDescriptor[i].usCaSysId);

		if (providerID == NA_MEDIASET_PROVIDER_ID)
		{
			if ((pstPmtInfo->stProgramCaDescriptor[i].usCaSysId >= NA_CAS_MEDIASE_RANGE_FROM) &&
				(pstPmtInfo->stProgramCaDescriptor[i].usCaSysId <= NA_CAS_MEDIASE_RANGE_TO))
			{
				HxLOG_Print("MEDIASET CAS ID FOUND IN PROGRAM LEVEL\n");
				return TRUE;
			}
		}
		else //현재는 providerID가 0x8D11이 아닌 나머지는 다 Nagra의 Multi-op로 간주 한다.
		{
			if ((pstPmtInfo->stProgramCaDescriptor[i].usCaSysId & 0xFF00) == NA_CAS_ALL_RANGE)
			{
				HxLOG_Print("NAGRA MULTI-OP CAS ID FOUND IN PROGRAM LEVEL\n", pstPmtInfo->stProgramCaDescriptor[i].usCaSysId);
				return TRUE;
			}
		}
	}

	HxLOG_Print("check ELEMENT LEVEL - pstPmtInfo->ulNumOfElement = %d\n", pstPmtInfo->ulNumOfElement);
	for (i = 0; i < pstPmtInfo->ulNumOfElement; i++)
	{
		*numOfCADesc += (TUnsignedInt8)pstPmtInfo->stElementInfo[i].ulNumOfCaDescriptor;

		for (j = 0; j < pstPmtInfo->stElementInfo[i].ulNumOfCaDescriptor; j++)
		{
			HxLOG_Print("pstPmtInfo->stElementInfo[%d].stElementCaDescriptor[%d].usCaSysId = 0x%x\n", i, j, pstPmtInfo->stElementInfo[i].stElementCaDescriptor[j].usCaSysId);
			if (providerID == NA_MEDIASET_PROVIDER_ID)
			{
				if ((pstPmtInfo->stElementInfo[i].stElementCaDescriptor[j].usCaSysId >= NA_CAS_MEDIASE_RANGE_FROM) &&
				      (pstPmtInfo->stElementInfo[i].stElementCaDescriptor[j].usCaSysId <= NA_CAS_MEDIASE_RANGE_TO))
				{
					HxLOG_Print("MEDIASET CAS ID FOUND IN ELEMENT LEVEL\n");
					return TRUE;
				}

			}
			else //현재는 providerID가 0x8D11이 아닌 나머지는 다 Nagra의 Multi-op로 간주 한다.
			{
				if ( (pstPmtInfo->stElementInfo[i].stElementCaDescriptor[j].usCaSysId & 0xFF00) == NA_CAS_ALL_RANGE )
				{
					HxLOG_Print("NAGRA MULTI-OP CAS ID FOUND IN ELEMENT LEVEL\n");
					return TRUE;
				}
			}
		}
	}

	HxLOG_Print("Exit - CAS ID NOT FOUND\n");

	return FALSE;
}


/******************************************************************************************
  * CAI_PMT_checkGivenCaSysIdInPMT : 현재 PMT의 CaSysId에 원하는 ecmCaSystemID가 있으면 return TRUE
  ******************************************************************************************/
static HBOOL local_caPpv_CheckGivenCaSysIdInPMT(TUnsignedInt8 *numOfCADesc, const TUnsignedInt16 ecmCaSystemId)
{
	TUnsignedInt16	i = 0, j = 0;
	SvcCas_PmtInfo_t 	*pstPmtInfo = NULL;

	HxLOG_Print("Enter - ecmCaSystemId(0x%x)\n", ecmCaSystemId);

	if (numOfCADesc == NULL)
	{
		HxLOG_Print("svcHandle or numOfCADesc is NULL\n");
		return FALSE;
	}

	pstPmtInfo = svc_cas_MgrSubGetPmtInfo(s_hCurAction);
	if (!pstPmtInfo)
	{
		HxLOG_Print("stPmtInfo is NULL\n");
		return FALSE;
	}

	HxLOG_Print("pstPmtInfo->ulNumOfProgramCaDescriptor = %d\n", pstPmtInfo->ulNumOfProgramCaDescriptor);
	*numOfCADesc = (TUnsignedInt8)pstPmtInfo->ulNumOfProgramCaDescriptor;

	for (i = 0; i < pstPmtInfo->ulNumOfProgramCaDescriptor; i++)
	{
		HxLOG_Print("pstPmtInfo->stProgramCaDescriptor[%d].usCaSysId = 0x%x\n", i, pstPmtInfo->stProgramCaDescriptor[i].usCaSysId);

		if (pstPmtInfo->stProgramCaDescriptor[i].usCaSysId == ecmCaSystemId )
		{
			HxLOG_Print("GIVEN CAS ID FOUND IN PROGRAM LEVEL\n");
			return TRUE;
		}
	}

	HxLOG_Print("check ELEMENT LEVEL - pstPmtInfo->ulNumOfElement = %d\n", pstPmtInfo->ulNumOfElement);
	for (i = 0; i < pstPmtInfo->ulNumOfElement; i++)
	{
		*numOfCADesc += (TUnsignedInt8)pstPmtInfo->stElementInfo[i].ulNumOfCaDescriptor;

		for (j=0; j < pstPmtInfo->stElementInfo[i].ulNumOfCaDescriptor; j++)
		{
			HxLOG_Print("pstPmtInfo->stElementInfo[%d].stElementCaDescriptor[%d].usCaSysId = 0x%x\n", i, j, pstPmtInfo->stElementInfo[i].stElementCaDescriptor[j].usCaSysId);
			if ( pstPmtInfo->stElementInfo[i].stElementCaDescriptor[j].usCaSysId == ecmCaSystemId)
			{
				HxLOG_Print("NAGRA MULTI-OP CAS ID FOUND IN ELEMENT LEVEL\n");
				return TRUE;
			}
		}
	}

	HxLOG_Print("Exit - CAS ID NOT FOUND\n");
	return FALSE;

}

static void local_caPpv_ExecuteTsCheck(Handle_t svcHandle, NIT_WAIT_FLAG flag)
{
	// isSCmatchedWithPMT : AP에서 event info request를 부를 지 말 지를 결정 하는 boolean값.
	// 1. FALSE : 현재 card가 들어오는 PMT와 match가 안되므로  MW_CA_NA_PPV_requestEventInfo 를 부르지 않는다
	// 2. TRUE : 현재 card가 들어오는 PMT와 match가 되므로, MW_CA_NA_PPV_requestEventInfo 를 부른다.
	TBoolean 		isSCMatchedWithPMT = FALSE;
	NIT_WAIT_FLAG	myWaitFlag = flag;
	CUR_NIT_INFO	*sCurNITInfo;
	TS_PERIOD_TYPE 	tsType = 0;
	NIT_WAIT_ITEM	stNitItem;
	HERROR			hErr = ERR_OK;

	HxLOG_Info("Enter - svcHandle(0x%x), flag(%d)\n", svcHandle, flag);

	if ((s_eNitWaitFlag == eNIT_WAIT_ING) && (myWaitFlag == eNIT_WAIT_INIT)) // 새로운 TS check request가 왔다는 소리
	{
		HxLOG_Info("cancel previous TIMER - NIT_WAIT_TIMER_ID\n");
		stNitItem.eOwner = eNIT_WAIT_OWNER_NONE;
		stNitItem.flag = eNIT_WAIT_INIT;
		stNitItem.provider = 0x0000;
		local_caPpv_CheckNIT(0, &stNitItem);
	}

	// NIT_WAIT_DONE 상태에서는 뒤의 상태로 가지 않는다. (변경 안함)
	if (myWaitFlag == eNIT_WAIT_INIT)
	{
		myWaitFlag = eNIT_WAIT_ING;
	}

	sCurNITInfo = local_caPpv_GetNITArrivedFlag();
	if (s_hCurSvc != HANDLE_NULL)
	{
		if (svcHandle == s_hCurSvc)
		{
			if (!s_bIsNagraTS)
			{
				if (sCurNITInfo->bArrived == TRUE)
				{
					// NIT가 들어 왔는데, Nagra TS가 아니다.
					tsType = TS_NOT_NA_PERIOD; //isSCMatchedWithPMT = FALSE;
					HxLOG_Print("<---------- EXIT!! CUR TP is NOT MediasetTP\n");
					goto notiToAP;
				}
				else
				{
					//아직 NIT가 들어 오지 않았다. 기다려야 하는지 마는 지 결정.
					if (myWaitFlag == eNIT_WAIT_ING) // 5 if : myWaitFlag
					{
						//기다려라.
						HxLOG_Print("wait for NIT !!call local_caPpv_CheckNIT(eNIT_WAIT_OWNER_IPPV)\n");
						stNitItem.eOwner = eNIT_WAIT_OWNER_IPPV;
						stNitItem.flag = eNIT_WAIT_ING;
						stNitItem.provider = 0x0000;
						local_caPpv_CheckNIT(0, &stNitItem);
						return;
					}
					else
					{
						// 더 이상 기다리지 않는다.
						tsType = TS_PERIOD_ERROR; //isSCMatchedWithPMT = FALSE;
						HxLOG_Print("<--- Still NIT not arrived, but do not wait more.. NOT GOOOD!!\n");
						goto notiToAP;
					}
				}
			}
			else
			{
				TUnsignedInt32 	reqStatus;
				TCaSystemId 	ecmCaSystemId;
				HBOOL 			bSCIN = FALSE; //SC is in the slot
				TUnsignedInt8 	numOfCADesc = 0;
				TUnsignedInt16 	providerID = NA_MEDIASET_PROVIDER_ID; //SC가 slot에 없고, PMT에 Mediaset이 있으면, Mediaset TS로 간주 한다.
				HBOOL			bTrue;

				// NIT가 들어 왔고, Nagra TS이다, 이제 Mediaset TS 인지 check 하라.
				HxLOG_Print("<---- NAGRA TS, now check MediasetTP using PMT\n");

				//여기서 우선 card의 정보를 확인 하여라..
				// case A. In the case several CAS IDs are present in PMT, the application should use the one that matches the one in the smart card
				// case B. In case no CAS IDs are matching (or no smart card inserted), next rules should be followed in case of several CAS IDs:
				//   a) At least one CAS ID is in Mediaset range (0x1802 - 0x1805). So "signale cryptato" should be displayed in Mediaset graphics
				//   b) No CAS IDs are in Mediaset range. So "signale cryptato" should be displayed in Pangea graphics

				// SC 확인
				hErr = CA_NA_CakGetEcmCaSystemId(0, &ecmCaSystemId);	// 현재 CardSlot은 1개만 고려
				HxLOG_Print("CA_NA_EcmCaSystemId: hErr(%d), ecmCaSystemId(0x%x)\n", hErr, ecmCaSystemId);
				if (hErr == ERR_OK)
				{
					bSCIN = TRUE;
				}

				if ((bSCIN == TRUE) && ((ecmCaSystemId < NA_CAS_MEDIASE_RANGE_FROM) || (ecmCaSystemId > NA_CAS_MEDIASE_RANGE_TO)))
				{
					// 이 이외의 나머지 경우는 모두 Multi-op로 생각 함.
					providerID = NA_OTHER_PROVIDER_ID;
				}

				HxLOG_Print("bSCIN(%d), providerID(0x%x)\n", bSCIN, providerID);

				// 1.  clear channel인지 확인 후 scrambled 이면 현 stream의 owner 확인
				bTrue = local_caPpv_CheckGivenCaSysIdInPMT(&numOfCADesc, ecmCaSystemId);

				//scrambled service 확인 ...
				HxLOG_Print("bTrue = %d, numOfCADesc = %d, sCurNITInfo->bArrived(%d), sCurNITInfo->orgNetId(%d)!!\n",
												bTrue, numOfCADesc, sCurNITInfo->bArrived, sCurNITInfo->orgNetId));

				if (numOfCADesc == 0) // 6 : if (numOfCADesc : clear channel
				{
					isSCMatchedWithPMT = TRUE; // Following이 scrambled일 수 있으니까,...???

					// multi -operator관련 하여서는 더 확인 하여야 한다.
					if (sCurNITInfo->orgNetId == MEDIASET_PREMIUM_ONID)
					{
						tsType = TS_CLEAR_NA_MEDIASET_PERIOD;  // clear Na Mediaset period
						HxLOG_Print("<---- NAGRA TS & TS_CLEAR_NA_MEDIASET_PERIOD\n");
					}
					else
					{
						tsType = TS_CLEAR_NA_OTHER_PERIOD; // claar NA other period
						HxLOG_Print("<---- NAGRA TS & TS_CLEAR_NA_OTHER_PERIOD\n");
					}

					goto notiToAP;
				} // 6 end

				//!! scrambled channel
				//---------------------
				// case 1. bSCIN == TRUE, 원하는 provider ID가 PMT에 들어 있을 경우(bTrue == TRUE) (case A)
				//		: isSCMatchedWithPMT = TRUE;
				//		1.1. provider ID가 Mediaset일 경우 ==> Mediaset UI
				//		1.2. provider ID가 Pangea 일 경우 ==> Pangea UI
				// case 2. bSCIN == TRUE, 원하는 provider ID가 PMT에 없을 경우 (bTrue == FALSE)  (case B, Mediaset	check)
				//		: isSCMatchedWithPMT = FALSE;
				//		2.1. Mediaset caSystemID가 PMT에 들어 있을 경우 ==> Mediaset UI
				//		2.2. Mediaset caSystemID가 PMT에 없을 경우 ==>	Pangea UI
				// case 3. bSCIN == FALSE
				//		: isSCMatchedWithPMT = FALSE;
				//		3.1. Mediaset caSystemID가 PMT에 들어 있을 경우 ==> Mediaset UI
				//		3.2. Mediaset caSystemID가 PMT에 없을 경우 ==>	Pangea UI

				if ( (bSCIN == TRUE) && (bTrue == TRUE) )// 7 if
				{
					// case 1.  SC가 slot에 들어 있고, Mediaset이던 Pangea든 원하던 providerID에 해당 하는 caSystemID가 PMT에 존재 한다.
					isSCMatchedWithPMT = TRUE;

					if (providerID == NA_MEDIASET_PROVIDER_ID) //case 1.1
					{
						//Mediaset TS 이다
						HxLOG_Print("<---- case 1.1, bSCIN:%d, bTrue:%d, TS_NA_MEDIASET_PERIOD\n", bSCIN, bTrue);
						tsType = TS_NA_MEDIASET_PERIOD;
					}
					else // case 1.2
					{
						//다른 provider 이다
						HxLOG_Print("<---- case 1.2, bSCIN:%d, bTrue:%d, TS_NA_OTHER_PERIOD\n", bSCIN, bTrue);
						tsType = TS_NA_OTHER_PERIOD;
					}

					goto notiToAP;
				}
				else
				{
					// case 2 or case 3 : isSCMatchedWithPMT = FALSE;

					bTrue = local_caPpv_CheckGivenProviderIDInPMT(&numOfCADesc, (TUnsignedInt16)NA_MEDIASET_PROVIDER_ID);

					if (bTrue == TRUE) // Mediaset caSystemID(0x1802~0x1805)가 PMT에 있다
					{
						// case 2.1 or case 3.1
						HxLOG_Print("<---- case 2.1or3.1 : bSCIN:%d, bTrue:%d, NAGRA TS\n", bSCIN, bTrue);
						tsType = TS_NA_MEDIASET_PERIOD;
					}
					else
					{
						// case 2.2 or case 3.2
						HxLOG_Print("<---- case 2.2or3.2 : bSCIN:%d, bTrue:%d, TS_NA_OTHER_PERIOD TS\n", bSCIN, bTrue);
						tsType = TS_NA_OTHER_PERIOD; //다른 provider 이다
					}

					goto notiToAP;
				} //7 end
			}
		}
		else
		{
			HxLOG_Print("given svcHandle[0x%x] != s_hCurSvc[0x%x] TS_PERIOD_ERROR!!\n", svcHandle, s_hCurSvc);
			// request한 svcHandle과 current Service Handle이 다르다.  notify 함.
			tsType = TS_PERIOD_ERROR; //isSCMatchedWithPMT = FALSE;

			goto notiToAP;
		} // 2 end
	}


notiToAP:

	HxLOG_Print("Final : tsType = %d, isSCMatchedWithPMT(%d)\n", tsType, isSCMatchedWithPMT);

	if (s_pfnTSCheckNotifyCallback)
	{
		s_pfnTSCheckNotifyCallback(svcHandle, tsType, isSCMatchedWithPMT);
	}

	// 다음 단계 event info request 실행
	if (tsType == TS_NA_MEDIASET_PERIOD || tsType == TS_NA_OTHER_PERIOD)
	{
		//CA_NA_PpvRequestEventInfo();
	}

	HxLOG_Info("Exit\n");
}


static void local_caPpv_ExecuteEventInfo(void)
{
	//asynch 용...
	TCaRequestStatus 	status = CA_REQUEST_NO_ERROR;
	TCaRequest			*pRequest;
	TUnsignedInt32		i;
	TUnsignedInt32		curTime = 0;
	TSmartcardAlarm 	eScAlarm = CA_ALARM_SMARTCARD_OK;
	HERROR				hErr = ERR_OK;

	HxLOG_Info("Enter\n");

	if (s_ulEITwaitTimerId)
	{
		VK_TIMER_Cancel(s_ulEITwaitTimerId);
		s_ulEITwaitTimerId = 0;
	}

	hErr = VK_CLOCK_GetTime(&curTime);
	if (hErr != ERR_OK)
	{
		HxLOG_Print("VK_CLOCK_GetTime error\n");
		status = CA_REQUEST_ERROR;
		goto notiToAP;
	}

	// card status 체크
	hErr = MW_CA_NA_GetCurrScStatus(0, &eScAlarm);
	HxLOG_Print("hErr = %d, eScAlarm(%ld)\n", hErr, eScAlarm);
	if (hErr != ERR_OK || eScAlarm != CA_ALARM_SMARTCARD_OK)
	{
		HxLOG_Print("card check error\n");
		status = CA_REQUEST_ERROR;
		goto notiToAP;
	}

#if 0
	if (s_stLastPpvEvent.pCaRequest)
	{
		HxLOG_Print("before s_stLastPpvEvent.pCaRequest = %ld\n", s_stLastPpvEvent.pCaRequest);
		CA_NA_PpvDisposeRequest(s_stLastPpvEvent.pCaRequest);
		s_stLastPpvEvent.pCaRequest = NULL;
		HxLOG_Print("after s_stLastPpvEvent.pCaRequest = %ld\n", s_stLastPpvEvent.pCaRequest);
	}
#endif
	// 1. create request
	status = CA_NA_PpvCreateRequest(CA_REQUEST_TYPE_EVENT, &pRequest);
	if(status == CA_REQUEST_NO_ERROR)
	{
		// 2. set asynch, caRequestDispose가 callback에서 불려 져야 한다.
		status = caRequestSetAsynchronousResponse(pRequest, local_caPpv_PresentEventInfoCallback);
		if(status == CA_REQUEST_NO_ERROR)
		{
			HxLOG_Print("s_stPresentEvtPurchase.evtName:%s, evtID:%x, descLength(%d) \n",
						 s_stPresentEvtPurchase.EvtName,s_stPresentEvtPurchase.EvtId, s_stPresentEvtPurchase.DesLength));
			HxLOG_Print("startTime:%d, curTime:%d, endTime:%d \n",
						 s_stPresentEvtPurchase.startTime, curTime, s_stPresentEvtPurchase.startTime+s_stPresentEvtPurchase.duration));

			// 3. set the present event descriptors filtered from EIT
			status = caRequestSetDescriptors(pRequest, s_stPresentEvtPurchase.DesLength, s_stPresentEvtPurchase.Des);
//			status = caRequestSetStartTime(pRequest, &s_stPresentEvtPurchase.xEventStartTime);

			// 4. send request
			status = caRequestSend(pRequest);
			if (status == CA_REQUEST_NO_ERROR)
			{
				//여기가 ERROR가 없는 것....이제 callback을 기다려라...
				HxLOG_Print("EXIT - caRequestSend OKKK!! wait for call back..\n");
				return status;//CA_REQUEST_NO_ERROR
			}
			else
			{
				HxLOG_Print("caRequestSend ERR!!  call caRequestDispose. status(%d)\n", status);
			}
		}
		else
		{
			HxLOG_Print("caRequestSetAsynchronousResponse err status(%d)\n", status);
		}
	}
	else
	{
		HxLOG_Print("CA_NA_PpvCreateRequest(CA_REQUEST_TYPE_EVENT err status(%d)\n", status);
	}

	// caRequestFail... dispose request
	CA_NA_PpvDisposeRequest(pRequest);

	HxLOG_Print("caRequestDisposeEND.. send ERR noti to AP..\n");

notiToAP:

	//일단 여기까지 오면 ERROR 다.. callback이 있으면 AP에게 알려라...
	for (i = 0; i < NA_MAX_CALLBACK_NUM; i ++)
	{
		if (s_pfnEventInfoNofityCallback[i])
		{
			HxLOG_Print("callback[%d] exit, send to call back...ERR(%d) \n", i, status);
			s_pfnEventInfoNofityCallback[i](status, NULL, NULL); //뭐 딴 것  줄 것 있나?
		}
		else
		{
			HxLOG_Print("no call[%d] to AP!!, (%d)\n", i, status);
		}
	}

	HxLOG_Info("Exit\n");
}


static HERROR local_caPpv_GetPurchaseWIndowOfProduct(TCaProduct *pxProduct, UNIXTIME *pnStartTime, UNIXTIME *pnEndTime)
{
	TCalendarTime 	tStartTime;
	TCalendarTime 	tEndTime;
	TCaObjectStatus	objStatus;
	HxDATETIME_t 	dateTime;
	HERROR			hErr;

	if ((pxProduct == NULL) || (pnStartTime == NULL) || (pnEndTime == NULL))
	{
		HxLOG_Print("Input param is NULL\n");
		return ERR_FAIL;
	}

	objStatus = caProductGetPurchaseStartTime(pxProduct, & tStartTime);
	objStatus |= caProductGetPurchaseEndTime(pxProduct, &tEndTime);
	if (objStatus != CA_OBJECT_NO_ERROR)
	{
		HxLOG_Print("caProductGetPurchaseStartTime error(%d)\n", objStatus);
		return ERR_FAIL;
	}

	dateTime.stDate.usYear = (unsigned short)tStartTime.year +1900;
	dateTime.stDate.ucMonth = (unsigned short)tStartTime.month;
	dateTime.stDate.ucDay= (unsigned short)tStartTime.day;
	dateTime.stTime.ucHour= (unsigned short)tStartTime.hour;
	dateTime.stTime.ucMinute= (unsigned short)tStartTime.minute;
	dateTime.stTime.ucSecond= (unsigned short)tStartTime.second;
	dateTime.stTime.usMillisecond = 0 ;
	hErr = HLIB_DATETIME_ConvertDateTimeToUnixTimeWithOutOffset(&dateTime, pnStartTime);
	if (hErr != ERR_OK)
	{
		HxLOG_Print("HLIB_DATETIME_ConvertDateTimeToUnixTimeWithOutOffset error(%d)\n", objStatus);
		return ERR_FAIL;
	}
	HxLOG_Print("StartTime - %d/%d/%d %d:%d:%d\n", dateTime.stDate.usYear, dateTime.stDate.ucMonth,
		dateTime.stDate.ucDay, dateTime.stTime.ucHour, dateTime.stTime.ucMinute, dateTime.stTime.ucSecond));
	HxLOG_Print("*pnStartTime = %d\n", *pnStartTime);

	dateTime.stDate.usYear = (unsigned short)tEndTime.year +1900;
	dateTime.stDate.ucMonth = (unsigned short)tEndTime.month;
	dateTime.stDate.ucDay= (unsigned short)tEndTime.day;
	dateTime.stTime.ucHour= (unsigned short)tEndTime.hour;
	dateTime.stTime.ucMinute= (unsigned short)tEndTime.minute;
	dateTime.stTime.ucSecond= (unsigned short)tEndTime.second;
	dateTime.stTime.usMillisecond = 0 ;
	hErr = HLIB_DATETIME_ConvertDateTimeToUnixTimeWithOutOffset(&dateTime, pnEndTime);
	if (hErr != ERR_OK)
	{
		HxLOG_Print("HLIB_DATETIME_ConvertDateTimeToUnixTimeWithOutOffset error(%d)\n", objStatus);
		return ERR_FAIL;
	}
	HxLOG_Print("EndTime - %d/%d/%d %d:%d:%d\n", dateTime.stDate.usYear, dateTime.stDate.ucMonth,
		dateTime.stDate.ucDay, dateTime.stTime.ucHour, dateTime.stTime.ucMinute, dateTime.stTime.ucSecond));
	HxLOG_Print("*pnEndTime = %d\n", *pnEndTime);

	return ERR_OK;

}


static TEventStatus local_caPpv_ProcessPPT(TCaEvent *ppEvent, TCaProduct *pxProduct, TPurchasableEventType *xPuchaseType, UNIXTIME *xPurchaseStartTime, UNIXTIME *xPurchaseEndTime)
{
	UNIXTIME		nStartTime = 0, nEndTime = 0, nCurrTime = 0;
	HERROR			hErr;
	TCaObjectStatus objStatus;
	TCaAccess	    pAccess;

	HxLOG_Print("ENTER\n");

	if (!xPurchaseStartTime || !xPurchaseEndTime || !xPuchaseType)
	{
		HxLOG_Print("Input param is NULL\n");
		return CA_EVENT_ERROR;
	}

	// cak implementation guideline 2.2.2 p35
	// 1. check Purchase window
	if (local_caPpv_GetPurchaseWIndowOfProduct(pxProduct, &nStartTime, &nEndTime) != ERR_OK)
	{
		HxLOG_Print("local_caPpv_GetPurchaseWIndowOfProduct,,, error, EXIT!\n");
		return CA_EVENT_ERROR;
	}

	hErr = VK_CLOCK_GetTime((unsigned long*)&nCurrTime);
	if (hErr != ERR_OK)
	{
		// get current time error
		HxLOG_Print("VK_CLOCK_GetTime error\n");
		return CA_EVENT_ERROR;
	}

//	hErr = local_caPpv_AddLocalTimeOffset(&nStartTime, &nEndTime);
	HxLOG_Print("hErr = %d, nStartTime = %d, nEndTime = %d, nCurrTime = %d\n", hErr, nStartTime, nEndTime, nCurrTime);
	if (hErr == ERR_OK)
	{
		*xPurchaseStartTime = nStartTime;
		*xPurchaseEndTime = nEndTime;

		if (nCurrTime < nStartTime) //wait until purchase time...
		{
			HxLOG_Print("Error!not yet purchase time period..wait..Can't be here..!!\n");
			//여기서 어떻게 기다리지????, 뭘 return 해 줘야 하나?  processSpecialEvent와 달리 아무것도 안 해 주는 것 같음.
			//PPT는 여기 올 수 없다.   하여간 여기서 AP에 return은 해 주자
			return	CA_EVENT_PURCHASE_PERIOD_BEFORE; // purchase period is not started....
		}
		else if (nCurrTime > nEndTime)
		{
			//display "Purchase window is over"
			HxLOG_Print("Error!purchase time if over\n");
			return	CA_EVENT_PURCHASE_PERIOD_OVER; // purchase period is not started....
		}

		//in the middle of purchase period...
		// 2. access 확인
		HxLOG_Print("in the middle of PURCHASE PERIOD!!\n");

		objStatus = caEventGetAccess(ppEvent, &pAccess);
		if (objStatus == CA_OBJECT_NO_ERROR)
		{
			HxLOG_Print("check pAccess!!, pAccess = %d\n", pAccess);

			if (pAccess == CA_ACCESS_DENIED_NO_VALID_CREDIT)
			{
				//display "No Valid Credit" : AP와 뭘 return 하여 줄 지 결정 하여야 함.
				HxLOG_Print("display 'No Valid Credit'-CA_EVENT_NO_VALID_CREDIT!!\n");
				return CA_EVENT_NO_VALID_CREDIT;
			}
			else if ((pAccess == CA_ACCESS_GRANTED_PPT) || (pAccess == CA_ACCESS_DENIED) || (pAccess == CA_ACCESS_DENIED_BUT_PPT))
			{
				//display purchase banner
				HxLOG_Print("pAccess:%d: display purchase banner[CA_EVENT_PPT] in the middle of PURCHASE PERIOD!!\n", pAccess);
				if (xPuchaseType)
				{
					*xPuchaseType = CA_EVENT_PPT;
				}
				return CA_EVENT_SUCCESS;
			}
			else
			{
				// purchase banner를 보여 주면 안된다.
				HxLOG_Print("pAccess:%d: do not display purchase banner[CA_EVENT_ERROR] shouldn't be HERE!!\n", pAccess);
				return CA_EVENT_ERROR;
			}

		}
		else
		{
			//caEvetGetAccess ERROR!!
			HxLOG_Print("caEventGetAccess error(%d)\n", objStatus);
			return CA_EVENT_ERROR;
		}
	}
	else
	{
		// get current time error
		HxLOG_Print("CheckPurchasable - error[0x%x]\n", objStatus);
		return CA_EVENT_ERROR;//PURCHASE_PERIOD_ENDED;
	}

	HxLOG_Print("EXIT - CAN't BE HERE!!!\n");
}

static TEventStatus local_caPpv_ProcessSpecialEvent(TCaProduct *pxProduct, TPurchasableEventType *xPuchaseType, UNIXTIME *xPurchaseStartTime, UNIXTIME *xPurchaseEndTime)
{
	UNIXTIME		nStartTime = 0, nEndTime = 0, nCurrTime = 0;
	HERROR			hErr;

	HxLOG_Print("ENTER\n");

	if (!xPurchaseStartTime || !xPurchaseEndTime || !xPuchaseType)
	{
		HxLOG_Print("Input param is NULL\n");
		return CA_EVENT_ERROR;
	}

	// cak implementation guideline 2.2.2 p35
	// step 5.1 : Purchase window
	if (local_caPpv_GetPurchaseWIndowOfProduct(pxProduct, &nStartTime, &nEndTime) != ERR_OK)
	{
		HxLOG_Print("local_caPpv_GetPurchaseWIndowOfProduct,,, error, EXIT!\n");
		return CA_EVENT_ERROR;
	}

	hErr = VK_CLOCK_GetTime((unsigned long*)&nCurrTime);
	if (hErr != ERR_OK)
	{
		// get current time error
		HxLOG_Print("VK_CLOCK_GetTime error\n");
		return CA_EVENT_ERROR;
	}

//	hErr = local_caPpv_AddLocalTimeOffset(&nStartTime, &nEndTime);
	HxLOG_Print("hErr = %d, nStartTime = %d, nEndTime = %d, nCurrTime = %d\n", hErr, nStartTime, nEndTime, nCurrTime);
	if (hErr == ERR_OK)
	{
		*xPurchaseStartTime = nStartTime;
		*xPurchaseEndTime = nEndTime;

		if (nCurrTime <  nStartTime) //wait until purchase time...
		{
			HxLOG_Print("Error!not yet purchase time period..wait.., EXIT!\n");
			// product 시작시간에 다시 event 체크를 한다.
			VK_TIMER_EventAfter((nStartTime-nCurrTime+1)*1000, local_caPpv_WaitEIT_TimerCallback, NULL, 0, &s_ulEITwaitTimerId);
			return	CA_EVENT_PURCHASE_PERIOD_BEFORE; // purchase period is not started....
		}
		else if (nCurrTime > nEndTime)
		{
			HxLOG_Print("Error!purchase time if over, EXIT!\n");
			return	CA_EVENT_PURCHASE_PERIOD_OVER; // purchase period is not started....
		}
		else
		{
			//in the middle of purchase period...get out of if...
			HxLOG_Print("in the middle of PURCHASE PERIOD!!,CA_EVENT_SPECIAL_EVENT,  EXIT!\n");
			// step 5.2 : Purchase special event
			//now in the middle of purchase period.. AP에서 request purchase를 불러야 한다.
			if (xPuchaseType)
			{
				*xPuchaseType = CA_EVENT_SPECIAL_EVENT;
			}
			return CA_EVENT_SUCCESS;
		}
	}
	else
	{
		// get current time error
		HxLOG_Print("CheckPurchasable - local_caPpv_AddLocalTimeOffset error, EXIT!\n");
		return CA_EVENT_ERROR;
	}

	HxLOG_Print("EXIT - CAN't BE HERE!!!\n");
}


static HERROR local_caPpv_AddLocalTimeOffset(UNIXTIME *startTime, UNIXTIME *endTime)
{
	//주어진 time에 local offset 값을 적용 하여 변경 하여 주기....
	HERROR 		hErr = ERR_OK;
	UNIXTIME 	unixtimeOffset = 0;

	if ((startTime == NULL) && (endTime == NULL))
	{
		HxLOG_Info("input time all NULL\n");
		return ERR_FAIL;
	}

	hErr = HLIB_DATETIME_GetTimeOffsetWithDst(&unixtimeOffset);
	unixtimeOffset = unixtimeOffset * SECONDS_PER_MIN;
	HxLOG_Info("unixtimeOffset = %d\n", unixtimeOffset);

	if (hErr == ERR_OK)
	{
		if (startTime)
		{
			*startTime += unixtimeOffset;
		}
		if (endTime)
		{
			*endTime += unixtimeOffset;
		}
	}

	return hErr;
}


static void local_caPpv_ResponsePresentEventCallback(TCaRequest*  pxRequest)
{
	TCaRequest 			*pRequest = NULL;
	TCaRequestStatus 	reqStatus = CA_REQUEST_NO_ERROR;
	TCaObjectStatus 	objStatus = CA_OBJECT_NO_ERROR;
	TUnsignedInt32 		ulEventNb = 0;
	TCaEvent 			**ppEvent;
	TEventStatus 		resEventStatus = CA_EVENT_ERROR;
	TCaProcessingStatus status = CA_PROCESSING_NO_ERROR;
	TCaAccess			caAccess;
	TUnsignedInt32 		ulNumOfProducts = 0;
	TUnsignedInt16 		i = 0;
	TCaProduct 			**ppxProductArray;

	PPV_EVENT_INFO		stPpvEventType;
	TUnsignedInt8 		ucBusinessType = 0;
	TUnsignedInt8	 	*pName;
	TSize		 		length;
	TUnsignedInt32 		nSelProduct = 0;

	HxLOG_Print("ENTER - pxRequest(%ld)\n", pxRequest);

	//일단 전달받은 request 검사...
	if(pxRequest == NULL)
	{
		HxLOG_Print("pxRequest is NULL!!!\n");
		resEventStatus = CA_EVENT_ERROR;
		goto notiToAp;
	}
	else
	{
		pRequest = pxRequest;
	}

	HxSTD_memset(&stPpvEventType, 0, sizeof(PPV_EVENT_INFO));

	local_caPpv_WaitForGivenTimeWithGivenInterval(MAX_PDT_LOADED_WAIT_TIME_INTERVAL, MAX_PDT_LOADED_WAIT_TIME);

	//get processing status
	reqStatus = caRequestGetProcessingStatus(pRequest, &status);
	if ((reqStatus != CA_REQUEST_NO_ERROR) || (status != CA_PROCESSING_NO_ERROR))
	{
		HxLOG_Print("reqStatus(%d), status(%d) from caRequestGetProcessingStatus()\n", reqStatus, status);
		resEventStatus = CA_EVENT_ERROR;
		goto disposeRequest;
	}
	HxLOG_Print("caRequestGetProcessingStatus - status(%d)\n", status);

	stPpvEventType.xEventStartTime 	= s_stPresentEvtPurchase.startTime;
	stPpvEventType.xEventID 		= s_stPresentEvtPurchase.EvtId;
//	local_caPpv_AddLocalTimeOffset(&stPpvEventType.xEventStartTime, NULL); //사실 error는 신경 쓰지 않는다.
	stPpvEventType.duration			= s_stPresentEvtPurchase.duration;

	//get event object
	reqStatus = caRequestGetObjects(pRequest, &ulEventNb, &ppEvent);
	HxLOG_Print("caRequestGetObjects - ulEventNb(%d)\n", ulEventNb);

	//request status가 정상이고 get한 event object가 있다면 access를 get하여 현재 product type을 확인한다...
	if((reqStatus == CA_REQUEST_NO_ERROR) && (ulEventNb > 0))
	{
		//Get access
		objStatus = caEventGetAccess(ppEvent[0], &caAccess);
		if(objStatus == CA_OBJECT_NO_ERROR)
		{
			HxLOG_Print("caAccess (%d)\n", caAccess);

			if ((caAccess == CA_ACCESS_DENIED) || (caAccess == CA_ACCESS_DENIED_BUT_PPT) || (caAccess == CA_ACCESS_GRANTED_PPT) || (caAccess == CA_ACCESS_DENIED_NO_VALID_CREDIT))
			{
				//get related products
				objStatus = caEventGetRelatedProducts(ppEvent[0], &ulNumOfProducts, &ppxProductArray);
				HxLOG_Print("ulNumOfProducts = %d\n", ulNumOfProducts);
				if((objStatus == CA_OBJECT_NO_ERROR) && (ulNumOfProducts > 0))
				{
					//related products 중에 ppt 가 있는지 확인...
					for (i = 0; i < ulNumOfProducts; i++)
					{
						if (ppxProductArray[i] == NULL)
						{
							HxLOG_Print("Can't HAPPEN!!, ppxProductArray[%dth] == NULL\n", i);
							continue;
						}

						//get product business type
						objStatus = caProductGetBusinessType(ppxProductArray[i], &ucBusinessType);
						HxLOG_Print("[%dth] caProductGetBusinessType objStatus(%d), ucBusinessType(%d)\n", i, objStatus, ucBusinessType);
						if((objStatus == CA_OBJECT_NO_ERROR) && (ucBusinessType == ePACKAGE_PAY_PER_TIME))
						{
							//process ppt
							resEventStatus = local_caPpv_ProcessPPT(ppEvent[0], ppxProductArray[i], &stPpvEventType.xPurchaseType, &stPpvEventType.xPurchaseStartTime, &stPpvEventType.xPurchaseEndTime);
							if(resEventStatus == CA_EVENT_SUCCESS)
							{
								HxLOG_Print("[%dth] ppxProductArray[%d] = 0x%x\n",i, i, ppxProductArray[i]);
								objStatus = caProductGetSliceDuration(ppxProductArray[i], &stPpvEventType.xSliceTime);
								HxLOG_Print("[%dth] objStatus = %d, PPT_Duration = %d\n", i, objStatus, stPpvEventType.xSliceTime);
								objStatus = caProductGetCost(ppxProductArray[i], &stPpvEventType.xCost);
								HxLOG_Print("[%dth] objStatus= %d, PPT_Cost = %d\n", i, objStatus, stPpvEventType.xCost);
								objStatus = caProductGetId(ppxProductArray[i], &stPpvEventType.xProductId);
								HxLOG_Print("productId = %d\n", stPpvEventType.xProductId);

								//objStatus = caProductGetName(ppxProductArray[i], &(length), &pName);
								objStatus = caEventGetName(ppEvent[0], &length, &pName);
								HxSTD_memset(stPpvEventType.xEvtName, 0, (MAX_EVENT_ITEM_NAME_LENGTH + 1));

								if ((objStatus == CA_OBJECT_NO_ERROR) && (length != 0) && (pName != NULL))
								{
									HxLOG_Print("[%dth] length= %d, objStatus=%d\n", i, length, objStatus);
									if (length < MAX_EVENT_ITEM_NAME_LENGTH)
									{
										HxSTD_memcpy((void*)stPpvEventType.xEvtName, (void*)pName, length);
									}
									else
									{
										HxSTD_memcpy((void*)stPpvEventType.xEvtName, (void*)pName, MAX_EVENT_ITEM_NAME_LENGTH);
									}
									HxLOG_Print("[%dth] PpvEventType.xEvtName= %s\n", i, stPpvEventType.xEvtName);
								}
								else
								{
									//CAK에서 event 가져 오는 것이 error가 나면 EIT의 것으로 대체 시켜 준다.
									HxLOG_Print("[%dth] use EIT(%s), pName= NULL, length= %d, objStatus=%d\n",
											  i, s_stPresentEvtPurchase.EvtName, length, objStatus));
									HxSTD_memcpy((void*)stPpvEventType.xEvtName, &(s_stPresentEvtPurchase.EvtName), MAX_EVENT_ITEM_NAME_LENGTH);
								}

								nSelProduct = i; //찾은 product 저장
								stPpvEventType.pCaProduct = ppxProductArray[i];
								stPpvEventType.pCaRequest = pRequest;

								HxLOG_Print("[%dth] PPT FOUND, do not dispose Request, notiToAP, result = %d!!! \n", i, resEventStatus);
								//purchase 하기로 결정이 날 때까지, request를 dispose 시키지 않는다.  즉, AP에서 purchase를 하지 않으면 dispose 시켜 주어야만 한다.
								//PPT 이면서  purchaseable period이면 banner에 보여 주어야 함.
								goto notiToAp;
							}
							else
							{
								HxLOG_Print("[%dth] sProcessPPT error!! \n", i, resEventStatus);
							}
						}
						else
						{
							HxLOG_Print("objStatus(%d) from caProductGetBusinessType(%d), ppxProductArray[i] = 0x%x\n", objStatus, ucBusinessType, ppxProductArray[i]);
						}
					}

					//일단 여기까지 왔다는 것은  ppt가 없다는것...
					if(caAccess == CA_ACCESS_DENIED)
					{
						//special event product를 찾는다...
						for (i = 0; i < ulNumOfProducts; i++)
						{
							objStatus = caProductGetBusinessType(ppxProductArray[i], &ucBusinessType);
							HxLOG_Print("[%dth] caProductGetBusinessType objStatus(%d), ucBusinessType(%d)\n", i, objStatus, ucBusinessType);

							if ((objStatus == CA_OBJECT_NO_ERROR) && (ucBusinessType == ePACKAGE_SPECIAL))
							{
								//SPECIAL EVENT가 있으면, activate special event, 만일 period before일 경우, AP에서 timer를 set하여 purchasable time이 되면 다시 banner를 보여 주어야 한다.
								resEventStatus = local_caPpv_ProcessSpecialEvent(ppxProductArray[i], &stPpvEventType.xPurchaseType, &stPpvEventType.xPurchaseStartTime, &stPpvEventType.xPurchaseEndTime);
								HxLOG_Print("local_caPpv_ProcessSpecialEvent - result = %d, startTime(%d), endTime(%d)\n", resEventStatus, stPpvEventType.xPurchaseStartTime, stPpvEventType.xPurchaseEndTime);

								if  (resEventStatus == CA_EVENT_SUCCESS) // SPECIAL EVENT FOUND
								{
									HxLOG_Print("\t CA_EVENT_SPECIAL_EVENT FOUND,  in for ppxProductArray[%d] = 0x%x\n", i, ppxProductArray[i]);
									objStatus = caProductGetCost(ppxProductArray[i], &stPpvEventType.xCost);
									HxLOG_Print("caProductGetCost - objStatus= %d, IPPV_Cost = %d\n", objStatus, stPpvEventType.xCost);
									objStatus = caProductGetId(ppxProductArray[i], &stPpvEventType.xProductId);
									HxLOG_Print("caProductGetId - objStatus = %d, productId = %d\n", objStatus, stPpvEventType.xProductId);

									//objStatus = caProductGetName(ppxProductArray[i], &(length), &pName);
									objStatus = caEventGetName(ppEvent[0], &length, &pName);
									HxLOG_Print("caEventGetName - objStatus = %d, length = %d\n", objStatus, length);
									HxSTD_memset(stPpvEventType.xEvtName, 0, (MAX_EVENT_ITEM_NAME_LENGTH + 1));
									if ((objStatus == CA_OBJECT_NO_ERROR) && (length != 0) && (pName != NULL))
									{
										if (length  < MAX_EVENT_ITEM_NAME_LENGTH)
										{
											HxSTD_memcpy((void*)stPpvEventType.xEvtName, (void*)pName, length);
										}
										else
										{
											HxSTD_memcpy((void*)stPpvEventType.xEvtName, (void*)pName, MAX_EVENT_ITEM_NAME_LENGTH);
										}
										HxLOG_Print("stPpvEventType.xEvtName= %s\n", stPpvEventType.xEvtName);

									}
									else
									{
										//CAK에서 event 가져 오는 것이 error가 나면 EIT의 것으로 대체 시켜 준다.
										HxLOG_Print("s_stPresentEvtPurchase.EvtName %s\n", s_stPresentEvtPurchase.EvtName);
										HxSTD_memcpy((void*)stPpvEventType.xEvtName, s_stPresentEvtPurchase.EvtName, MAX_STR_LEN);
									}

									stPpvEventType.xSliceTime = 0;
									nSelProduct = i;
									stPpvEventType.pCaProduct = ppxProductArray[i];
									stPpvEventType.pCaRequest = pRequest;
									//SPECIAL EVENT FOUND
									//purchase 하기로 결정이 날 때까지, request를 dispose 시키지 않는다.  즉, AP에서 purchase를 하지 않으면 dispose 시켜 주어야만 한다.
										//SPECIAL EVENT 이고 purchaseable period in 이여만 , banner를 보여 주어야 한다.  만일 다른 period가 아닐 경우는 구매를 할 수 없으므로
										//request를 dispose 시켜 주어야 한다.
									goto notiToAp;
								}
								else if (resEventStatus == CA_EVENT_ERROR)
								{
									//purchaseable window나 local time을 가져 오는데 error 가 났다.   계속 loop 실행
									HxLOG_Print("local_caPpv_ProcessSpecialEvent has no vaild purchase window, look for NEXT!!\n");
								}
								else if (resEventStatus == CA_EVENT_PURCHASE_PERIOD_OVER)
								{
									objStatus = caProductGetId(ppxProductArray[i], &stPpvEventType.xProductId);
									HxLOG_Print("productId = %d\n", stPpvEventType.xProductId);
									goto disposeRequest;
								}
								else
								{
									//CA_EVENT_PURCHASE_PERIOD_BEFORE, CA_EVENT_PURCHASE_PERIOD_OVER, CA_EVENT_NO_VALID_CREDIT 의 경우 더 이상 안 찾음.
									//purchaseable period가 아니면 banner를 안 보여 준다.  더 이상 찾지 않고,  PPT가 있는데, period가 맞지 않는 다는 error를 내 보냄.
									//PURCHASE BEFORE의 경우 IPPV는 AP에서 timer를 돌려 실제 PURCHASE TIME이 되면 다시 request를 하고, PURCHASE OVER 일 경우 purchase over message를 보여줌.
									HxLOG_Print("local_caPpv_ProcessSpecialEvent [%dth] sProcessSpecialEvent period not valid %d!!, dispose request!! \n", i, resEventStatus);
									goto disposeRequest;
								}

								HxLOG_Print("local_caPpv_ProcessSpecialEvent in for (i=%d) no IPPV avail!!\n", i);
							}
						}
					}
				}
				else
				{
					HxLOG_Print("objStatus(%d) from caEventGetRelatedProducts() or ulNumOfProducts(%d)\n", objStatus, ulNumOfProducts);
					CA_NA_PpvDisposeRequest(pRequest);
					return local_caPpv_CheckFollowingEvent();		// following event에 IPPV가 있는지 check
				}
			}
			else
			{
				CA_NA_PpvDisposeRequest(pRequest);
				return local_caPpv_CheckFollowingEvent();		// following event에 IPPV가 있는지 check
			}
		}
		else
		{
			HxLOG_Print("objStatus(%d) from caEventGetAccess()\n", objStatus);
		}
	}
	else
	{
		HxLOG_Print("reqStatus(%d) from caRequestGetObjects() or ulEventNb(%d)\n", reqStatus, ulEventNb);
	}

disposeRequest:
	//dispose request
	CA_NA_PpvDisposeRequest(pRequest);
	if (stPpvEventType.pCaProduct)
	{
		stPpvEventType.pCaProduct = NULL;
	}

notiToAp:
	//위의 disposeRequest를 거치지 않고 여기 오는 것은 AP에서 pRequest를 추후에 사용 하기 때문이며,
	//AP에서 사용 후 CA_NA_PpvDisposeRequest를 통하여 꼭  추후에 dispose시켜 주어야 한다. -KIMYS-
	HxLOG_Print("local_caPpv_ProcessSpecialEvent - notiToAP..., objStatus(%d), resEventStatus(%d), nSelProduct(%d)\n", objStatus, resEventStatus, nSelProduct);

	for (i = 0; i < NA_MAX_CALLBACK_NUM; i++)
	{
		//now give purchase list to AP
		if (s_pfnEventInfoNofityCallback[i])
		{
			HxLOG_Print("call back[%d] to noti\n", i);
			s_pfnEventInfoNofityCallback[i](resEventStatus, ppEvent[0], &stPpvEventType);
			if (resEventStatus == CA_EVENT_SUCCESS)
			{
//				HxSTD_memcpy(&s_stLastPpvEvent, &stPpvEventType, sizeof(PPV_EVENT_INFO));
			}
		}
		else
		{
			HxLOG_Print("no call back[%d] to give noti...\n", i);
		}
	}

	HxLOG_Print("EXIT\n");
}


static void local_caPpv_ResponseFollowingEventCallback(TCaRequest*  pxRequest)
{
	TCaRequest			*pRequest;
	TCaRequestStatus 	reqStatus;
	TCaObjectStatus 	objStatus;
	TUnsignedInt32 		eventNb = 0;
	TCaEvent 			**ppEvent;
	TEventStatus 		resEventStatus = CA_EVENT_ERROR;
	TCaProcessingStatus status = CA_PROCESSING_NO_ERROR;
	TCaAccess			pxAccess;
	TCaProduct 			**ppxProductArray;
	TUnsignedInt32 		ulNumberOfProducts;
	TUnsignedInt8 		ucBusinessType = 0;

	PPV_EVENT_INFO		stPpvEventType;
	TUnsignedInt8		*pName = NULL;
	TSize		 		length = 0;
	TUnsignedInt32		nSelProduct = 0;
	TUnsignedInt16 		i = 0;

	HxLOG_Print("Enter\n");

	HxSTD_memset(&stPpvEventType, 0, sizeof(PPV_EVENT_INFO));

	if (pxRequest == NULL)
	{
		HxLOG_Print("given pxRequest == NULL!!, return FALSE\n");
		resEventStatus = CA_EVENT_ERROR;
		goto notiToAP;
	}

	pRequest = pxRequest;

	local_caPpv_WaitForGivenTimeWithGivenInterval(MAX_PDT_LOADED_WAIT_TIME_INTERVAL, MAX_PDT_LOADED_WAIT_TIME);

	reqStatus = caRequestGetProcessingStatus(pRequest, &status);
	if ((reqStatus != CA_REQUEST_NO_ERROR) || (status != CA_PROCESSING_NO_ERROR))
	{
		HxLOG_Print("Processing Status is not correct !!!(%d), dispose request!! \n", status);

		resEventStatus = CA_EVENT_ERROR;
		goto disposeRequest;
	}

	// retrieve a caEvent object.. (CAK6_I api ver 4.1.12 p43)
	reqStatus = caRequestGetObjects(pRequest, &eventNb, &ppEvent); //getter가 OK일 경우 하나의 caEvent를 return 한다.

	stPpvEventType.xEventStartTime = s_stFollowingEvtPurchase.startTime;
	stPpvEventType.xEventID = s_stFollowingEvtPurchase.EvtId;
	//local_caPpv_AddLocalTimeOffset(&stPpvEventType.xEventStartTime, NULL); //사실 error는 신경 쓰지 않는다.
	stPpvEventType.duration	= s_stFollowingEvtPurchase.duration;

	HxLOG_Print("eventNb number = %d, reqStatus(%d)\n", eventNb, reqStatus);

	if ((reqStatus == CA_REQUEST_NO_ERROR) && (eventNb > 0))
	{
		objStatus = caEventGetAccess(ppEvent[0], &pxAccess);

		// access 가 dinied 가 아니면, special event product를 찾아라.
		if (objStatus == CA_OBJECT_NO_ERROR)
		{
			objStatus = caEventGetRelatedProducts(ppEvent[0], &ulNumberOfProducts, &ppxProductArray);
			HxLOG_Print("caEventGetRelatedProducts - objStatus = %d, pNumberOfProducts(%d)\n", objStatus, ulNumberOfProducts);
			if ((objStatus == CA_OBJECT_NO_ERROR) && (pxAccess == CA_ACCESS_DENIED))
			{
				// special event product를 찾아라.
				for (i = 0; i < ulNumberOfProducts; i++)
				{
					HxLOG_Print("[%dth] START (ppxProductArray[%d] = 0x%x)\n", i,i, ppxProductArray[i]);

					objStatus = caProductGetBusinessType(ppxProductArray[i], &ucBusinessType);
					HxLOG_Print("[%dth] caProductGetBusinessType - objStatus = %d, ucBusinessType =%d\n", i, objStatus, ucBusinessType);

					if ((objStatus == CA_OBJECT_NO_ERROR) && (ucBusinessType == ePACKAGE_SPECIAL))
					{
						resEventStatus = local_caPpv_ProcessSpecialEvent(ppxProductArray[i], &stPpvEventType.xPurchaseType, &stPpvEventType.xPurchaseStartTime, &stPpvEventType.xPurchaseEndTime);
						HxLOG_Print("local_caPpv_ProcessSpecialEvent - result = %d, purchageType = %d, startTime = %d, endTime = %d\n", resEventStatus, stPpvEventType.xPurchaseType, stPpvEventType.xPurchaseStartTime, stPpvEventType.xPurchaseEndTime);
						if (resEventStatus == CA_EVENT_SUCCESS)
						{
							HxLOG_Print("\tSPECIAL EVENT FOUND, for (i = %d)\n", i);

							objStatus = caProductGetCost(ppxProductArray[i], &stPpvEventType.xCost);
							HxLOG_Print("\t[%dth] ppxProductArray[%d] = 0x%x \n", i, i,ppxProductArray[i]);
							HxLOG_Print("\t[%dth] objStatus= %d, SPECIAL_Cost = %d\n", i, objStatus, stPpvEventType.xCost);
							objStatus = caProductGetId(ppxProductArray[i], &stPpvEventType.xProductId);
							HxLOG_Print("\tcaProductGetId - objStatus = %d, productId = %d\n", objStatus, stPpvEventType.xProductId);

							//objStatus = caProductGetName(ppxProductArray[i], &(length), &pName);
							objStatus = caEventGetName(ppEvent[0], &(length), &pName);
							HxLOG_Print("\tcaEventGetName - objStatus = %d, length = %d\n", objStatus, length);
							if ((objStatus == CA_OBJECT_NO_ERROR) && (length != 0) && (pName != NULL))
							{
								HxSTD_memset(stPpvEventType.xEvtName, 0, (MAX_EVENT_ITEM_NAME_LENGTH + 1));
								if (length	< MAX_EVENT_ITEM_NAME_LENGTH)
								{
									HxSTD_memcpy((void*)stPpvEventType.xEvtName, (void*)pName, length);
								}
								else
								{
									HxSTD_memcpy((void*)stPpvEventType.xEvtName, (void*)pName, MAX_EVENT_ITEM_NAME_LENGTH);
								}
								HxLOG_Print("\t[%dth] stPpvEventType.xEvtName= %s\n", i, stPpvEventType.xEvtName);
							}
							else
							{
								//CAK에서 event 가져 오는 것이 error가 나면 EIT의 것으로 대체 시켜 준다.
								HxLOG_Print("\t[%dth] use EIT, pName = (%s), length = %d\n", i, s_stFollowingEvtPurchase.EvtName, length);
								HxSTD_memcpy((void*)stPpvEventType.xEvtName, &(s_stFollowingEvtPurchase.EvtName), MAX_STR_LEN);
							}

							nSelProduct = i;
							stPpvEventType.xSliceTime = 0;
							stPpvEventType.pCaProduct = ppxProductArray[i];
							stPpvEventType.pCaRequest = pRequest;

							//special event를 찾으면, 그 event의 activate 결과에 상관 없이, special event를 찾았으므로, noti를 보낸다.
							goto notiToAP;

						}
						else if (resEventStatus == CA_EVENT_ERROR)
						{
							//purchaseable window나 local time을 가져 오는데 error 가 났다.   계속 loop 실행
							HxLOG_Print("[%dth] has no vaild purchase window, look for NEXT!!\n", i);
						}
						else
						{
							//CA_EVENT_PURCHASE_PERIOD_BEFORE, CA_EVENT_PURCHASE_PERIOD_OVER, CA_EVENT_NO_VALID_CREDIT 의 경우 더 이상 안 찾음.
							//purchaseable period가 아니면 banner를 안 보여 준다.  더 이상 찾지 않고,  PPT가 있는데, period가 맞지 않는 다는 error를 내 보냄.
							//PURCHASE BEFORE의 경우 IPPV는 AP에서 timer를 돌려 실제 PURCHASE TIME이 되면 다시 request를 하고, PURCHASE OVER 일 경우 purchase over message를 보여줌.
							HxLOG_Print("[%dth] sProcessSpecialEvent period not valid %d!!, dispose request!! \n", i, resEventStatus);
							goto disposeRequest;
						}

					}
					else
					{
						HxLOG_Print("[%dth] ucBusinessType:%d, objStatus:%d--> ERR\n", i, ucBusinessType, objStatus);
					}

				}//for (i=0;i < pNumberOfProducts; i++)

				HxLOG_Print("END FOR!!!\n");

			}// caEventGetRelatedProducts
			else
			{
				HxLOG_Info("caEventGetRelatedProducts error objStatus:%d\n", objStatus);
			}

		}// caEventGetAccess
		else
		{
			HxLOG_Info("caEventGetAccess error objStatus:%d\n", objStatus);
		}

	}// if( (reqStatus == CA_REQUEST_NO_ERROR) && (eventNb > 0) )
	else
	{
		HxLOG_Info("caRequestGetObjects error reqStatus:%d, eventNb:%d\n", reqStatus, eventNb);
	}

disposeRequest:
	// FREE pRequest.
	CA_NA_PpvDisposeRequest(pRequest);
	if (stPpvEventType.pCaProduct)
	{
		stPpvEventType.pCaProduct = NULL;
	}
notiToAP:

	for (i = 0; i < NA_MAX_CALLBACK_NUM; i++)
	{
		//now give purchase list to AP
		if (s_pfnEventInfoNofityCallback[i])
		{
			HxLOG_Print("call back[%d] to noti\n", i);
			s_pfnEventInfoNofityCallback[i](resEventStatus, ppEvent[0], &stPpvEventType);
			if (resEventStatus == CA_EVENT_SUCCESS)
			{
//				HxSTD_memcpy(&s_stLastPpvEvent, &stPpvEventType, sizeof(PPV_EVENT_INFO));
			}
		}
		else
		{
			HxLOG_Print("no call back[%d] to give noti...\n", i);
		}
	}

	HxLOG_Print("Exit\n");
}


static void local_caPpv_CheckFollowingEvent(void)
{
	//asynch 용...
	TCaRequestStatus 	status;
	TCaRequest			*pRequest;
	TUnsignedInt16		i = 0;

	HxLOG_Print("ENTER\n");

#if 0
	if (s_stLastPpvEvent.pCaRequest)
	{
		CA_NA_PpvDisposeRequest(s_stLastPpvEvent.pCaRequest);
		s_stLastPpvEvent.pCaRequest = NULL;
	}
#endif

	// 1. create request
	status = CA_NA_PpvCreateRequest(CA_REQUEST_TYPE_EVENT, &pRequest);
	if(status == CA_REQUEST_NO_ERROR)
	{
		// 2. set asynch, caRequestDispose가 callback에서 불려 져야 한다.
		status = caRequestSetAsynchronousResponse(pRequest, local_caPpv_FollowingEventInfoCallback);

		if(status == CA_REQUEST_NO_ERROR)	{

			// 3. set the following event descriptors filtered from EIT
			status = caRequestSetDescriptors(pRequest, s_stFollowingEvtPurchase.DesLength, s_stFollowingEvtPurchase.Des);
//			status = caRequestSetStartTime(pRequest, &s_stFollowingEvtPurchase.xEventStartTime); // optional
			//caRequestSetStartTime(TCaRequest * pxRequest, const TCalendarTime * pxStartTime) : 이걸 써야 하나?

			// 4. send request
			status = caRequestSend(pRequest);
			if (status == CA_REQUEST_NO_ERROR)
			{
				//여기가 ERROR가 없는 것....이제 callback을 기다려라...
				HxLOG_Print("local_caPpv_CheckFollowingEvent - caRequestSend OKKK!! wait for call back..\n");
				return;
			}
			else
			{
				HxLOG_Print("local_caPpv_CheckFollowingEvent - caRequestSend ERR!!  call caRequestDispose.(%d)\n", status);
			}
		}
		else
		{
			HxLOG_Print("local_caPpv_CheckFollowingEvent - caRequestSetAsynchronousResponse err(%d)\n", status);
		}
	}
	else
	{
		HxLOG_Print("CA_NA_PpvCreateRequest(CA_REQUEST_TYPE_EVENT) err(%d)\n", status);
	}

	// caRequestFail... dispose request
	CA_NA_PpvDisposeRequest(pRequest);

	HxLOG_Print("CA_NA_PpvDisposeRequest.. send ERR noti to AP..\n");

	//일단 여기까지 오면 ERROR 다.. callback이 있으면 AP에게 알려라...
	for (i = 0; i < NA_MAX_CALLBACK_NUM; i++)
	{
		if (s_pfnEventInfoNofityCallback[i])
		{
			HxLOG_Print("call back[%d] to noti\n", i);
			s_pfnEventInfoNofityCallback[i](status, NULL, NULL);
		}
		else
		{
			HxLOG_Print("no call back[%d] to give noti...\n", i);
		}
	}

	HxLOG_Print("EXIT\n");
}


static void local_caPpv_SetProductsLoadable(TBoolean flag)
{
	// case 1. local_caPpv_SetProductsLoadable(TRUE) : PDT가 도착, caProductsLoadedExportation, 이 경우는 이미 bIsProductsLoaded = TRUE 로 setting
	// case 2. local_caPpv_SetProductsLoadable(FALSE) : TP CHANGED : PDT stop, Nagra TS = FALSE
	TBoolean curFlag = s_bIsProductsLoaded;
	TUnsignedInt16	i = 0;

	HxLOG_Print("curFlag[%d], flag[%d], s_eProductLoadFlag[%d]\n", curFlag, flag, s_eProductLoadFlag);

	// flag가 true일 경우는 이미 cak task에서 setting이 완료 되어서, 이를 update 할 필요 없다.
	// 다만 위의 case 2번일 경우 UIT 내부적으로 FALSE를 만듬.
	if (flag == FALSE)
	{
		s_bIsProductsLoaded = flag;
	}
	HxLOG_Print("s_bIsProductsLoaded = %d\n", s_bIsProductsLoaded);

	if (s_eProductLoadFlag == ePRODUCT_LOAD_WAIT_ING)
	{
		// 현재 ProductLoadable이 되기를 기다리는 request가 timer로 돌고 있다..
		HxLOG_Print("calcel TIMER and call local_caPpv_RequestPackageList()\n");

		if  (flag == TRUE)
		{
			//case 1, 드디어 PDT가 도착 하였다.  기다리고 있던 timer를 풀고, packagelist request를 해라.  UIT task 처리
			//만일 uit에서 TRUE가 왔는데, 이미 cak task에서 다시 LOADING (FALSE)가 와도 (curFlag == FLASE), Mediaset spec에 따르면,
			//일단 menu에 들어와서 packagelist가 request 되면 그 이후 update되는 PDT는 신경을 안 쓰므로 그냥 request 하면 된다.
			HxLOG_Print("CANCEL TIMER!!! PDT arrived!!, call local_caPpv_RequestPackageList)!\n");

			VK_TIMER_Cancel(s_ulProductLoadTimerId);
			s_ulProductLoadTimerId = 0;
			local_caPpv_RequestPackageDataList(0, NULL); // 이 안에서 s_uitProductLoadFlag = ePRODUCT_LOAD_WAIT_INIT; 수행 됨....
		}
		else
		{
			//현재 package list를 기다리고 있는 AP에게 중간에 cancel 되었다고 error 전달... (이럴 경우가 정말 없는데...)
			//flag== FALSE는 stopPDT에서만 불리는데, 이것은 TP가 change 되었을 때 만 불린다. 그러므로, menu에서 이게 불릴 일은 진짜 없다.
			//MHP 쪽도 TP 가 변경 되면 xlet이 distroy되므로 이것이 불릴 일은 진짜 없다.
			HxLOG_Print("CPDT is attempting to UPDATE!! Can't be happen often!!...\n");

			s_eProductLoadFlag = ePRODUCT_LOAD_WAIT_INIT;

			for (i = 0; i < NA_MAX_CALLBACK_NUM; i ++)
			{
				if (s_pfnPackageListNofityCallback[i])
				{
					HxLOG_Print("[%d] CAN't BE HERE in LIVE, CUR TP is NOT MediasetTP, give Noti to callback[0].(CA_EVENT_TS_FAIL)...\n", i);
					//ap에서는 이 callback을 받고, 현재 Mediaset TP에 있지 않다고 Error Message를 출력 하여야 함.
					s_pfnPackageListNofityCallback[i](CA_EVENT_TS_FAIL, NULL,  0, NULL);
				}
				else
				{
					HxLOG_Print("[%d] no callback\n", i);
				}
			}

			HxLOG_Print("CUR TP is NOT MediasetTP\n");
		}
	}
	else
	{
		// 현재 시점에서  product list가 request 된 적이 없다.
		HxLOG_Print("None timer is waiting for product loadable.  s_eProductLoadFlag(%d)\n",s_eProductLoadFlag);

		//  다시 한번 EIT update를 해라. 무조건 LOADED가 오면 EIT update를 불러 준다.
		if  (curFlag == TRUE)  // 그 사이에 CAK에서 다시 LOADING이 올 수 있으므로, 항상 현 task에서 확인한 flag로 check 한다.
		{
			//Phase3에서는 LOADED가 오기 전에는 EIT가 와도 PDT date가 없으면 banner를 못 보여 줌으로, 다시 한번 EIT update를 불러 banner를 띄워 주어야 한다.
			TSignedInt32	nErrCode;

			if (s_hCurSvc != HANDLE_NULL)
			{
				NaCasMsg_t		stMsg;

				stMsg.wMsg = NA_FILTER_ID;
				stMsg.lParam2 = CAK_FILTER_EIT_PF_ARRIVAL; //INT_UIT_setNagraTS(isNAPPVTS)는 FILTER_UIT_PDT_ARRIVAL 에서 수행 한다.
				stMsg.lParam1 = (TSignedInt32)s_hCurSvc;
				stMsg.lParam3 = TRUE;

				HxLOG_Print("send CAK_FILTER_EIT_PF_ARRIVAL to uiTask!!\n");
				nErrCode = VK_PostMessage(s_ulPpvQueueId, (void *)&stMsg, QUEUE_SIZE_1);
				if (nErrCode)
				{
					HxLOG_Info("error in VK_PostMessage (0x%x)\n", nErrCode);
				}
			}
			else
			{
				HxLOG_Print("s_hCurSvc HANDLE_NULL 0x%x\n");
			}
		}
		else
		{
			HxLOG_Print("CAK task curFlag(%d), given UITask Flag(%d) \n", curFlag, flag);
		}

	}

	// 실제 전달된 listener의 처리 결과와는 전혀 상관 없이 그냥 Listener가 왔다고 전달만 해 주면 된다.
	// 실제 curFlga가 바뀌었다고 하더라도, 한번 온것은 전달 하여야 하기 때문에 curFlag를 안 쓰고, flag로 확인
	if (flag == TRUE)
	{
		local_caPpv_SendCaListenerArrivedNotify(CA_LISTENER_TYPE_PRODUCTS_LOADED);
	}
}

static void local_caPpv_SendCaListenerArrivedNotify(TCaListenerType listenerType)
{
	HxLOG_Print("ENTER-->(%d)=>\n", listenerType);

	if ( (listenerType >= CA_LISTENER_TYPE_SMARTCARDS) && (listenerType < CA_LISTENER_NUM_TYPES) )
	{
		if (s_pfnCaListenerArrivedNotifyCallback)
		{
			HxLOG_Info("CAK_PPV_LOADING_LISTENER send\n");
			//실제 전달된 listener의 처리 결과와는 전혀 상관 없이 그냥 Listener가 왔다고 전달만 해 주면 된다.
			s_pfnCaListenerArrivedNotifyCallback(listenerType);
			HxLOG_Info("CAK_PPV_LOADING_LISTENER send ended\n");
		}
		else
		{
			HxLOG_Info("CAK_PPV_LOADING_LISTENER - s_pfnCaListenerArrivedNotifyCallback = NULL\n");
		}
	}

	HxLOG_Print("<-- EXIT (%d)=>\n", listenerType);
}


#define ________CA_CAK_PPV_NIT_________________________________________

static HERROR local_caPpv_NotifyPDTinNIT(Handle_t	hAction, TUnsignedInt16 usUniqueId,	TUnsignedInt8 *pucNit)
{
	HERROR 			hErr = ERR_OK;
	NaCasMsg_t		stMsg;
	TSignedInt32	nErrCode;
	TUnsignedInt16 	desSize = 0, size = 0;
	TUnsignedInt8 	*pDesNode = NULL;
	HBOOL 			isNAPPVTS = FALSE;
	PDT_DATA 		*HPDTData=NULL, *CPDTData=NULL;
	DbSvc_TsInfo_t	tsInfo, curTsInfo;

	HxLOG_Info("Enter - hAction(%d), uniqueId(%d)\n", hAction, usUniqueId);

	//현재 triple id와 같지 않으면 사용하지 않음
	if(s_hCurSvc == HANDLE_NULL)
	{
		HxLOG_Print("s_hCurSvc HANDLE_NULL\n");
		local_caPpv_SetNITArrivedFlag(TRUE, INVALID_NIT_ONID); // s_stNitArrivedInfo
		return hErr;
	}
	hErr = DB_SVC_GetTsInfoByServiceHandle(s_hCurSvc, &curTsInfo);
	HxLOG_Info("DB_SVC_GetTsInfoByServiceHandle - hErr(%d), curTsInfo.usOnId(%d), curTsInfo.usUniqueId(%d)\n", hErr, curTsInfo.usOnId, curTsInfo.usUniqueId);
	if(hErr != ERR_OK)
	{
		HxLOG_Print("DB_SVC_GetTsInfoByServiceHandle ERROR for current ServicdHandle, local_caPpv_SetNITArrivedFlag(TRUE)!!\n");
		local_caPpv_SetNITArrivedFlag(TRUE, INVALID_NIT_ONID); // s_stNitArrivedInfo
		return hErr;
	}

	hErr = DB_SVC_GetTsInfoByUniqueId(usUniqueId, &tsInfo);
	HxLOG_Print("DB_SVC_GetTsInfoByUniqueId - hErr = %d [OrgnetId:0x%x][tsId:0x%x][usTsIdx:0x%x]\n", hErr, tsInfo.usOnId, tsInfo.usTsId, tsInfo.usTsIdx);
	if(hErr != ERR_OK)
	{
		HxLOG_Print("DB_SVC_GetTsInfoByUniqueId ERROR for given usUniqueId, local_caPpv_SetNITArrivedFlag(TRUE)!!\n");
		local_caPpv_SetNITArrivedFlag(TRUE, INVALID_NIT_ONID); // s_stNitArrivedInfo
		return hErr;
	}

	if(tsInfo.usTsId != curTsInfo.usTsId || tsInfo.usOnId != curTsInfo.usOnId)
	{
		//service id는 비교 안해도 되는가....모르것네...해야 할거 같은데...할거면 나중에 pdt start도 같이 고쳐라
		HxLOG_Print("ERROR Different TripleID, local_caPpv_SetNITArrivedFlag(TRUE)!!\n");
		local_caPpv_SetNITArrivedFlag(TRUE, INVALID_NIT_ONID); // s_stNitArrivedInfo
		return hErr; //이 에러 처리는 안해도 됨
	}

	// 2. Nagra CPDT/HPDT  linkage descriptor를 확인 후 데이터 만듬.  CPDT의 star filter 시 사용 하기 위해(pid) raw data 저장
//	nRet = MXI_SM_CreateNitNetworkRawDescriptor(serviceHandle,	actualOrOther, netId, tsId, orgNetId, &size, &rawData);

	CAPPV_NIT_SEM_GET();

	size = (TUnsignedInt16)get12bit(pucNit+1)+3;	// pucNit rawdata total size

#ifndef WIN32
	if (pucNit != NULL)
	{
		TUnsignedInt16 j = 0, k = 1;



		HxLOG_Print("\n\n\n -------- NIT RAW DATA ---------(size:%d)-\n", size);

		while ( j < size)
		{
			HxLOG_Print("[");
			for (k = 1; k < 21; k++)
			{
				HxLOG_Print("%02x ",*(pucNit+j));
				j++;
				if (j >= size) break;
			}
			HxLOG_Print("]\n");
		}
		HxLOG_Print("----------------- NIT RAW DATA END---------------\n\n\n\n");
	}
#endif

	if (pucNit != NULL)
	{
		desSize = 10;			// network descrioptos length 까지 이동
		pDesNode = pucNit+desSize;
		while (desSize < size)
		{
			HxLOG_Print("pDesNode [%x], pDesNode+1=size [%x], pDesNode+8 [%x]\n", *pDesNode, *(pDesNode+1), *(pDesNode+8));
			if ((*pDesNode == TAG_LINKAGE) && (*(pDesNode+1) > 7))
			{
				//같은  linkage descriptor가 2개 이상이면 우야노... 현재는 제일 나중에 오는 것으로 overwriting 하는 것으로 함.
				if(*(pDesNode+8) == 0x91)
				{
					CPDTData = (PDT_DATA *)OxCAS_Malloc( sizeof( PDT_DATA) );
					CPDTData->tsId =  *(pDesNode+2) << 8 | *(pDesNode+3);
					CPDTData->orgNetId = *(pDesNode+4) << 8 | *(pDesNode+5);
					CPDTData->svcId = *(pDesNode+6) << 8 | *(pDesNode+7);
					CPDTData->pid = (*(pDesNode+9) << 8 | *(pDesNode+10)) >> 3;

					isNAPPVTS = TRUE; //우선 둘 중에 하나만 존재해도 true로 함
					HxLOG_Print("isNAPPVTS = TRUE, ox91 found...\n");
					HxLOG_Print("CPDT..[tdId:%d][orgNetId:%d][svcId:%d][pid:%d]\n", CPDTData->tsId,CPDTData->orgNetId ,CPDTData->svcId,CPDTData->pid);

				}
				else if(*(pDesNode+8) == 0x90)
				{
					HPDTData = (PDT_DATA *)OxCAS_Malloc( sizeof( PDT_DATA) );
					HPDTData->tsId = *(pDesNode+2) << 8 | *(pDesNode+3);
					HPDTData->orgNetId = *(pDesNode+4) << 8 | *(pDesNode+5);
					HPDTData->svcId = *(pDesNode+6) << 8 | *(pDesNode+7);
					HPDTData->pid = (*(pDesNode+9) << 8 | *(pDesNode+10)) >> 3;

					isNAPPVTS = TRUE; //우선 둘 중에 하나만 존재해도 true로 함
					HxLOG_Print("isNAPPVTS = TRUE, ox90 found...\n");
					HxLOG_Print("HPDT..[tdId:%d][orgNetId:%d][svcId:%d][pid:%d]\n", HPDTData->tsId,HPDTData->orgNetId ,HPDTData->svcId,HPDTData->pid);

				}
				else
				{
					HxLOG_Print("*(pDesNode+8) != 0x91 or 0x90 [%x]\n", *(pDesNode+8));
				}

			}
			else
			{
HxLOG_Print("not TAG_LINKAGE..pDesNode [%c], pDesNode+1 [%c], pDesNode+8 [%c]\n", *pDesNode, *(pDesNode+1), *(pDesNode+8));
			}

			desSize += (*(pDesNode+1) + 2); //Tag + size +  length of descriptor
			pDesNode = pDesNode + *(pDesNode+1) + 2; //Tag + size +  length of descriptor
			HxLOG_Print("desSize(%d), size(%d)!!\n", desSize, size);
		}
	}


	//current TS에 대한 NIT가 왔고, 그에 대한 판단도 이루어 졌다.
	//실제로 mediaset ts setting이 sRequestPDTStop / start 에서 이루어져 약간 synch가 안 맞을 수도 있다
	//그렇다고 아래 PDT 가 다 request 동작을 끝 낼 때 까지 기다릴 수 없음
	//KIMYS_FD : 20060730 : 싱크가 안 맞는다.... Nagra TS를 uit에서 setting 하므로 이를 UIT에서 message를 받아 setting 하게 함.
	//{{
	//local_caPpv_SetNITArrivedFlag(TRUE, orgNetId); // s_uitNitArrivedFlag
	//}}

	if (pucNit)
	{
		OxCAS_Free(pucNit);
		pucNit = NULL;
	}

	CAPPV_NIT_SEM_RELEASE();

	HxLOG_Print("isNAPPVTS Result (%s)!!\n", isNAPPVTS?"TRUE":"FALSE");

	//현재 play되고 있는 stream이 NA PPV 인가?
	//	case 1. 현재 (NA PPV) + NEW (NA PPV) => do nothing
	//	case 2. 현재 (NA PPV) + NEW (일반) => stop PDT
	//	case 3. 현재 (일반) + NEW (NA PPV) => strat PDT
	//	case 4. 현재 (일반) + NEW (일반) ==> do nothing
	if ( ( s_bIsNagraTS && (!isNAPPVTS) ) || // case 2: stop PDT
		( !s_bIsNagraTS && (isNAPPVTS) ) ) // case 3: start PDT
	{
		// uitask로 메시지를 보내 request cpdt start / stop filter를 해주도록 함
		// CPDTData와 HPDTData 가 둘다 NULL이면 stop filter 이다.
		stMsg.wMsg = NA_FILTER_ID;
		stMsg.lParam2 = CAK_FILTER_NIT_ARRIVAL; //INT_UIT_setNagraTS(isNAPPVTS)는 FILTER_UIT_PDT_ARRIVAL 에서 수행 한다.
		stMsg.lParam1 = (TUnsignedInt32)CPDTData;
		stMsg.hWnd = (TUnsignedInt32)HPDTData;
		stMsg.lParam3 = tsInfo.usOnId;

		HxLOG_Print("send CAK_FILTER_NIT_ARRIVAL to uiTask!!\n");
		nErrCode = VK_PostMessage(s_ulPpvQueueId, (void *)&stMsg, QUEUE_SIZE_1);

		if (nErrCode)
		{
			HxLOG_Info("error in VK_PostMessage 0x%x\n", nErrCode);
		}
	}
	else
	{
		local_caPpv_SetNITArrivedFlag(TRUE, tsInfo.usOnId);

		if ( s_bIsNagraTS && (isNAPPVTS) ) // case 1: do nothing
		{
			HxLOG_Print("[sNotifyPDTinNIT] NA PPV -> NA PPV : do nothing!!\n");
		}

		if (!s_bIsNagraTS && (!isNAPPVTS) )	// case 4: do nothing
		{
			HxLOG_Print("[sNotifyPDTinNIT] not NA PPV  -> not NA PPV : do nothing!!\n");
		}

		if (CPDTData)
		{
			OxCAS_Free(CPDTData);
			CPDTData = NULL;
		}
		if (HPDTData)
		{
			OxCAS_Free(HPDTData);
			HPDTData = NULL;
		}
	}


	return ERR_OK;
}

static HERROR local_caPpv_NotifyTdt(void)
{
	NaCasMsg_t 		stMsg;
	TSignedInt32	nErrCode;
	TBoolean		IsRealUpdated;

	if(s_hCurSvc == HANDLE_NULL)
	{
		HxLOG_Print("s_hCurSvc HANDLE_NULL\n");
		return ERR_FAIL;
	}

	if (s_bTdtReceived == FALSE)
	{
		s_bTdtReceived = TRUE;

		stMsg.wMsg = NA_FILTER_ID;
		stMsg.lParam2 = CAK_FILTER_TDT_ARRIVAL; //INT_UIT_setNagraTS(isNAPPVTS)는 FILTER_UIT_PDT_ARRIVAL 에서 수행 한다.
		stMsg.lParam1 = (TUnsignedInt32)s_hCurSvc;

		HxLOG_Print("send CAK_FILTER_NIT_ARRIVAL\n");
		nErrCode = VK_PostMessage(s_ulPpvQueueId, (void *)&stMsg, QUEUE_SIZE_1);

		if (nErrCode)
		{
			HxLOG_Info("error in VK_PostMessage 0x%x\n", nErrCode);
		}
	}

	return ERR_OK;
}

static void local_caPpv_RequestPDTStart(PDT_FILTER_MODE mode)
{
	TCaRequestStatus status;
	TCaRequest* pRequest;

	//TODO 1) 이전에 start 시킨 것이 있으면, 이전 pid와 새로 변경된 pid가 같은 지 확인은 실제 caPDTStartCallback을
	//		받은 후 current pid들과 비교해서 setting.
	//TODO 2) caPDTStartCallback에서 꼭 caRequestDispose() 불러 주기..
	HxLOG_Print("ENTER - mode(%d)\n", mode);

	//start는 NA PPV TS일 경우만 불린다.  다시 한번 확인.
	s_bIsNagraTS = TRUE;

	// pdt start filter request
	status = CA_NA_PpvCreateRequest(CA_REQUEST_TYPE_PDT_START, &pRequest);

	if(status == CA_REQUEST_NO_ERROR)
	{
		//KIMYS_NA {{asynch로 할 경우 아래를 풀어 쓰면 된다. 단, 그 때는 caRequestDispose가 callback에서 불려 져야 한다.  밑의 caRequestDispose를 지울 것.
		//status = caRequestSetAsynchronousResponse(pRequest, caPDTStartCallback);
		//}} KIMYS_NA

		//질문 2) CAK6_I extension API ver4.0.7 p78에 따르면, setter는 request send 하기 전에만 사용 할 수 있다고
		//		되어 있다.  근데 여기서는 request send 후 그 결과를 받아서 한다.....이상 타... 만일 set을 미리 한다고 하면
		//		caPDTStartCallback은 받아서 뭘 하지?
		if ((status == CA_REQUEST_NO_ERROR) && (mode != PDT_FILTER_NONE))
		{
			HxLOG_Print("triple id in PDT is useless, DO NOT care!!! mode(0x%x)\n", mode);

			if (mode & PDT_FILTER_CPDT)
			{
				Handle_t 	hAction;
				HERROR 		hErr;

				hErr = CA_NA_PSI_ifExt_GetLiveAction(&hAction);
				HxLOG_Print("hErr = %d, hAction = %d\n", hErr, hAction);
				if (hErr == ERR_OK)
				{
					hErr = caRequestSetTransportSessionId(pRequest, (TUnsignedInt32)hAction);
					status = caRequestSetPid(pRequest, s_CPDT_DATA.pid);

					if(status == CA_REQUEST_NO_ERROR)
					{
						HxLOG_Print("new setting CPDT_DATA PID(0x%x) called OK.... EXIT!!\n", s_CPDT_DATA.pid);
					}
					else
					{
						HxLOG_Print("setting CPDT_DATA PID(0x%x) result ERROR(%d).. EXIT!!\n", s_CPDT_DATA.pid, status);
					}
				}
				else
				{
					HxLOG_Print("CPDT is NULL\n");
				}
			}
			else
			{
				HxLOG_Print("CPDT is NULL\n");
			}

			//아래서 status까지 확인 해야 하나?
			if (mode & PDT_FILTER_HPDT)
			{
				Handle_t 	hAction;
				HERROR 		hErr;

				hErr = CA_NA_PSI_ifExt_GetLiveAction(&hAction);
				HxLOG_Print("hErr = %d, hAction = %d\n", hErr, hAction);
				if (hErr == ERR_OK)
				{
					hErr = caRequestSetTransportSessionId(pRequest, (TUnsignedInt32)hAction);
					status = caRequestSetPid(pRequest, s_HPDT_DATA.pid);

					if(status == CA_REQUEST_NO_ERROR)
					{
						HxLOG_Print("new setting HPDT_DATA PID(0x%x) called OK.... EXIT!!\n", s_HPDT_DATA.pid);
					}
					else
					{
						HxLOG_Print("setting HPDT_DATA PID(0x%x) result ERROR(%d).. EXIT!!\n", s_HPDT_DATA.pid, status);
					}
				}
				else
				{
					HxLOG_Print("HPDT is NULL !!!\n");
				}
			}
			else
			{
				HxLOG_Print("HPDT is NULL\n");
			}
		}
		else
		{
			HxLOG_Print("caRequestSetAsynchronousResponse error (%d)!!\n", status);
		}

		if (status == CA_REQUEST_NO_ERROR)
		{
			status = caRequestSend(pRequest);
			if (status != CA_REQUEST_NO_ERROR)
			{
				HxLOG_Print("caRequestSend error (%d)!!\n", status);
			}
			else
			{
				HxLOG_Print("request PDT filter start OKKK!!! noti to ac_media!!-> EXIT!!\n");
#if 0//defined(EU_MHP)
				//ac_media쪽에 현재 setting된 PDT pid들을 전달 한다.
				//TP가 변경 되어 새로운 video가 setting 될 때, 기존에 같은 pid가 PDT를 위해 setting이 되어 揚聆만? sRequestStop()이 불리어야 한다.
				MXI_MHP_StartPDT(CPDT_DATA.pid, HPDT_DATA.pid);
#endif
			}

		}
	}
	else
	{
		HxLOG_Print("CA_NA_PpvCreateRequest(CA_REQUEST_TYPE_PDT_START) error status(%d)!!\n", status);
	}

	CA_NA_PpvDisposeRequest(pRequest);

	HxLOG_Print("EXIT!!\n");

}

static unsigned long local_caPpv_RequestPDTStop(TBoolean bFromCakTask)
{
	//항상 synch 모드....
	TCaRequestStatus status;
	TCaRequest* pRequest;

	HxLOG_Print("ENTER - bFromCakTask(%d)\n", bFromCakTask);

	//더 이상 NIT가 와서 TS가 NA PPV TS인지 확인 하기 전까지는 NA PPV가 아니다.
	s_bIsNagraTS = FALSE;

	//실제 start가 안된 상태에서 stop을 하면 될까? : 된다.  순서에 상관 없이 cak 동작..

	// PDT request
	status = CA_NA_PpvCreateRequest(CA_REQUEST_TYPE_PDT_STOP, &pRequest);
	if(status == CA_REQUEST_NO_ERROR)
	{
		status = caRequestSend(pRequest);
		if (status != CA_REQUEST_NO_ERROR)
		{
			HxLOG_Print("caRequestSend ERR !!! --> (%d)!!\n", status);
		}
		else
		{
			HxLOG_Print("caRequestSend OK END request stop PDT filter (%d)!! \n", status);
#if 0//  검토 중 AV pid와 PDT pid가 우연히 같을 경우 고려하여 ac_playstream에 추가되어야 함. defined(EU_MHP)
			if (bFromCakTask)
				//MHP 쪽으로 PDT가 stop 되었다고 알려서 MHP쪽의 PDT data를 clear 시켜 붐.  data sync를 맞추기 위하여... KIMYS_NA
				MXI_MHP_StopPDT();
#endif
		}
	}
	else
	{
		HxLOG_Print("CA_NA_PpvCreateRequest ERROR status(%d)!! \n", status);
	}

	CA_NA_PpvDisposeRequest(pRequest);

	HxLOG_Print("EXIT - status(%d)\n", status);

	return status;
}


#define ______________________________________________________

/********************************************************************
 Function		: local_caPpv_NotifyTPChanged
 Description	: TP Change 됐을 때 전달되는 msg에 의해서 처리해야하는 사항....
 			  해당 Msg가 전달된 후 NIT Arrival이 도착할 것이다.
 			  그러므로 변경전 TP가 Mediaset TP 였다면 무조건 Stop하면 된다.
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
static void local_caPpv_NotifyTPChanged(void)
{
	//UIT task에서 처리 하면 다른 task에 밀려서 TP changed가 NIT arrived 보다 나중에 처리가 된다.  그래서 NIT가 왔음에도 불구 하고, 다시
	//오지 않았다고 reset이 됨.  이를 위해 TP changed는 바로 처리 하기로 함. [20060607 KIMYS]
	//Digital channel -> Analog channel 로 변경 됨.  TP change와 같은 효과...
	HxLOG_Print("called !! s_bIsNagraTS = %d!!\n", s_bIsNagraTS);

	//TP 변경... reset NIT wait flag ....
	local_caPpv_SetNITArrivedFlag(FALSE, INVALID_NIT_ONID); //s_stNitArrivedInfo

	if (s_bIsNagraTS)
	{
		HxLOG_Print("call sRequestPDTStop(TRUE)!!\n");
		local_caPpv_RequestPDTStop(TRUE);
	}
	else
	{
		HxLOG_Print("cuttentry NOT Mediaset TS...do nothing!!\n");
	}

	s_bTdtReceived = FALSE;
}

static HERROR	local_caPpv_MakePfEvent(EVENT_PF_INFO nEventPfType, TUnsignedInt8 *pucEit, EVT_PURCHASE *pEventPurchaseInfo)
{
	TUnsignedInt16	nEventId = 0, usSvcId = 0, usTsId = 0, usOnId = 0;
	UNIXTIME		ulStartTime = 0, ulDuration = 0;
	TUnsignedInt16	index = 0;
	HxDATETIME_t		dateTime;
	TUnsignedInt8 	ucVer = 0, ucRunningStatus = 0, ucFreeCaMode = 0;
	TUnsignedInt16	size = 0;
	TUnsignedInt8	aucTime[5];
	TUnsignedInt16	sub_desc_size = 0, remained_desc_size = 0, real_evt_name_size;
	TUnsignedInt8	ucDesTag, ucDesLen;
	TUnsignedInt16 	desSize = 0;
	TUnsignedInt8	*pDesNode = NULL;
	HERROR			hErr = ERR_OK;

	if (nEventPfType != eEIT_PF_PRESENT && nEventPfType != eEIT_PF_FOLLOWING)
	{
		return ERR_FAIL;
	}

	if (!pucEit || !pEventPurchaseInfo)
	{
		return ERR_FAIL;
	}

	size = (TUnsignedInt16)get12bit(pucEit+1)+3;	// pucNit rawdata total size
	HxLOG_Info("size = %d\n", size);

	if (size < 26 || size > 4096)	// 데이터가 제대로 들어있지 않다.
	{
		return ERR_FAIL;
	}

#ifndef WIN32
	if (pucEit != NULL)
	{
		TUnsignedInt16 j = 0, k = 1;

		HxLOG_Print("\n\n\n -------- EIT RAW DATA ---------(size:%d)-\n", size);

		while ( j < size)
		{
			HxLOG_Print("[");
			for (k = 1; k < 21; k++)
			{
				HxLOG_Print("%02x ",*(pucEit+j));
				j++;
				if (j >= size) break;
			}
			HxLOG_Print("]\n");
		}
		HxLOG_Print("----------------- EIT RAW DATA END---------------\n\n\n\n");
	}
#endif

	if (pucEit != NULL)
	{
		usSvcId = (TUnsignedInt16)get16bit(pucEit+3);
		ucVer = (TUnsignedInt8)(0x1f & (((pucEit[5]) & 0x3e) >> 1));
		usTsId = (TUnsignedInt16)get16bit(pucEit+8);
		usOnId = (TUnsignedInt16)get16bit(pucEit+10);

		desSize = 14;			// event_id 까지 이동
		pDesNode = pucEit+desSize;

		if (desSize < size)
		{
			sub_desc_size = (TUnsignedInt16)get12bit(pDesNode+10);
			HxLOG_Print("index = %d, sub_desc_size= %d\n", index, sub_desc_size);
			if (desSize + sub_desc_size > size)
			{
				HxLOG_Print("\n\nFail-Fail-Fail-Fail-Fail-Fail-Fail\n\n\n");
				return ERR_FAIL;
			}

			if (index >= 2)
			{
				HxLOG_Print("I don't know. index = %d, desSize = %d, size = %d\n", index, desSize, size);
				return ERR_FAIL;
			}

			// event id
			nEventId = (TUnsignedInt16)get16bit(pDesNode);
			HxLOG_Print("nEventId = %d\n", nEventId);

			// start time 5byte
			HxSTD_memset(aucTime, 0, sizeof(TUnsignedInt8)*5);
			HxSTD_memcpy(aucTime, pDesNode+2, sizeof(TUnsignedInt8)*5);
			HxLOG_Print("start time parsing (%02x)(%02x)(%02x)(%02x)(%02x)\n", aucTime[0], aucTime[1], aucTime[2], aucTime[3], aucTime[4]);
			if( aucTime[0] == 0xff
				&& aucTime[1] == 0xff
				&& aucTime[2] == 0xff
				&& aucTime[3] == 0xff
				&& aucTime[4] == 0xff)
			{
				return ERR_FAIL;
			}
			hErr = HLIB_DATETIME_ConvertMJDBcdTimeToUnixTime(aucTime, &ulStartTime);
			if (hErr != ERR_OK)
			{
				HxLOG_Print("HLIB_DATETIME_ConvertMJDBcdTimeToUnixTime(ulStartTime) fail");
				return ERR_FAIL;
			}

			// duration 3byte
			HxSTD_memset(aucTime, 0, sizeof(TUnsignedInt8)*5);
			HxSTD_memcpy(aucTime, pDesNode+7, sizeof(TUnsignedInt8)*3);
			HxLOG_Print("duration(%02x)(%02x)(%02x)\n", aucTime[0], aucTime[1], aucTime[2]);
			if( aucTime[0] == 0xff
				&& aucTime[1] == 0xff
				&& aucTime[2] == 0xff)
			{
				HxLOG_Print("duration parsing fail");
				return ERR_FAIL;
			}
			hErr = HLIB_DATETIME_ConvertBcdTimeToUnixTime(aucTime, &ulDuration);
			if (hErr != ERR_OK)
			{
				HxLOG_Print("UTIL_TCONV_ConvertMJDBcdTimeToUnixTime(ulDuration) fail");
				return ERR_FAIL;
			}
			HxLOG_Print("ulDuration = %d\n", ulDuration);

			ucRunningStatus = (TUnsignedInt8)(0x07 & (((pDesNode[10]) & 0xe0) >> 5));
			ucFreeCaMode = (TUnsignedInt8)(0x01 & (((pDesNode[10]) & 0x10) >> 4));

			pEventPurchaseInfo->EvtId = nEventId;
			pEventPurchaseInfo->startTime = ulStartTime;
			pEventPurchaseInfo->duration = ulDuration;
			pEventPurchaseInfo->DesLength = sub_desc_size;
			HxSTD_memset(pEventPurchaseInfo->Des, 0, MAX_EIT_DES_SIZE);
			if (sub_desc_size > MAX_EIT_DES_SIZE)
			{
				HxSTD_memcpy(pEventPurchaseInfo->Des, pDesNode+12, MAX_EIT_DES_SIZE);
			}
			else
			{
				HxSTD_memcpy(pEventPurchaseInfo->Des, pDesNode+12, sub_desc_size);
			}
			// s_stPresentEvtPurchase.xType : CAK_UIT_EVENT_INFO_RESPONSE 을 받아서 결정이 된다.
			hErr = HLIB_DATETIME_ConvertUnixTimeToDateTimeWithOutOffset(ulStartTime, &dateTime);
			pEventPurchaseInfo->xEventStartTime.year = (TUnsignedInt8)dateTime.stDate.usYear-1900;
			pEventPurchaseInfo->xEventStartTime.month = (TUnsignedInt8)dateTime.stDate.ucMonth;
			pEventPurchaseInfo->xEventStartTime.day = (TUnsignedInt8)dateTime.stDate.ucDay;
			pEventPurchaseInfo->xEventStartTime.hour = (TUnsignedInt8)dateTime.stTime.ucHour;
			pEventPurchaseInfo->xEventStartTime.minute = (TUnsignedInt8)dateTime.stTime.ucMinute;
			pEventPurchaseInfo->xEventStartTime.second = (TUnsignedInt8)dateTime.stTime.ucSecond;

			remained_desc_size = sub_desc_size;

			pDesNode += 12;

			while (remained_desc_size > 0)
			{
				ucDesTag = get8bit(pDesNode);
				ucDesLen = get8bit(pDesNode+1);
				HxLOG_Print("ucDesTag = 0x%x, ucDesLen = %d, remained_desc_size = %d\n", ucDesTag, ucDesLen, remained_desc_size);
				if (remained_desc_size < (TUnsignedInt16)(ucDesLen+2))	// 데이터가 제대로 들어있지 않다.
				{
					return ERR_FAIL;
				}

				if (ucDesTag == TAG_SHORT_EVENT)
				{
					pEventPurchaseInfo->rawEvtNameLength = get8bit(pDesNode+5);
					pEventPurchaseInfo->rawEvtName = (TUnsignedInt8*)OxCAS_Malloc(pEventPurchaseInfo->rawEvtNameLength+1);
					HxSTD_memset(pEventPurchaseInfo->rawEvtName, 0, pEventPurchaseInfo->rawEvtNameLength+1);
					HxSTD_memcpy(pEventPurchaseInfo->rawEvtName, pDesNode+6, pEventPurchaseInfo->rawEvtNameLength);

					real_evt_name_size = svc_cas_MgrUtilStrLen(pEventPurchaseInfo->rawEvtName);
					if (real_evt_name_size >= MAX_STR_LEN)
					{
						MWC_UTIL_DvbStrncpy(pEventPurchaseInfo->EvtName, pEventPurchaseInfo->rawEvtName, MAX_STR_LEN-1);
					}
					else
					{
						MWC_UTIL_DvbStrncpy(pEventPurchaseInfo->EvtName, pEventPurchaseInfo->rawEvtName, real_evt_name_size);
					}

					break;
				}
				pDesNode += (ucDesLen+2);

				remained_desc_size -= (ucDesLen+2);
			}

			desSize += sub_desc_size;
			HxLOG_Print("index = %d, sub_desc_size = %d, desSize = %d, size = %d\n", index, sub_desc_size, desSize, size);
		}

	}

	return ERR_OK;
}

static HERROR local_caPpv_NotifyEit(TUnsignedInt32 hAction, TUnsignedInt16 usUniqueId, TUnsignedInt8 *pucEitP, TUnsignedInt8 *pucEitF)
{
	HERROR			hErr = ERR_OK;
	EVT_PURCHASE	stPresentEventInfo, stFollowingInfo;
	TBoolean		bEvtChanged = FALSE;

	HxLOG_Info("Enter - hAction(%d), uniqueId(%d)\n", hAction, usUniqueId);

	//현재 triple id와 같지 않으면 사용하지 않음
	if(s_hCurSvc == HANDLE_NULL)
	{
		return hErr;
	}

	CAPPV_EIT_SEM_GET();

#if 1
	local_caPpv_ClearPurchaseInfo(&s_stPresentEvtPurchase);
	local_caPpv_ClearPurchaseInfo(&s_stFollowingEvtPurchase);

	if (pucEitP != NULL && pucEitF != NULL)
	{
		local_caPpv_MakePfEvent(eEIT_PF_PRESENT, pucEitP, &s_stPresentEvtPurchase);
		local_caPpv_MakePfEvent(eEIT_PF_FOLLOWING, pucEitF, &s_stFollowingEvtPurchase);
	}
#else
	if (pucEitP != NULL && pucEitF != NULL)
	{
		HxSTD_memset(&stPresentEventInfo, 0, sizeof(EVT_PURCHASE));
		HxSTD_memset(&stFollowingInfo, 0, sizeof(EVT_PURCHASE));
		local_caPpv_MakePfEvent(eEIT_PF_PRESENT, pucEitP, &stPresentEventInfo);
		local_caPpv_MakePfEvent(eEIT_PF_FOLLOWING, pucEitF, &stFollowingInfo);

		if ((local_caPpv_IsSamePurchaseInfo(s_stPresentEvtPurchase, stPresentEventInfo) == FALSE) || (local_caPpv_IsSamePurchaseInfo(s_stFollowingEvtPurchase, stFollowingInfo) == FALSE))
		{
			local_caPpv_ClearPurchaseInfo(&s_stPresentEvtPurchase);
			local_caPpv_ClearPurchaseInfo(&s_stFollowingEvtPurchase);

			local_caPpv_MakePfEvent(eEIT_PF_PRESENT, pucEitP, &s_stPresentEvtPurchase);
			local_caPpv_MakePfEvent(eEIT_PF_FOLLOWING, pucEitF, &s_stFollowingEvtPurchase);

			bEvtChanged = TRUE;
		}

		local_caPpv_ClearPurchaseInfo(&stPresentEventInfo);
		local_caPpv_ClearPurchaseInfo(&stFollowingInfo);
	}
#endif



	if (pucEitP)
	{
		OxCAS_Free(pucEitP);
		pucEitP = NULL;
	}
	if (pucEitF)
	{
		OxCAS_Free(pucEitF);
		pucEitF = NULL;
	}

	CAPPV_EIT_SEM_RELEASE();

	//CA_NA_PpvRequestEventInfo();
	//if (bEvtChanged)
	{
		local_caPpv_ExecuteTsCheck(s_hCurSvc, 0);
	}

	return ERR_OK;
}

#define ________CA_CAK_PPV_PRUCHASE_HISTORY_________________________________________

static void local_caPpv_ClearPurchaseInfo(EVT_PURCHASE *pEventPurchaseInfo)
{
	HxLOG_Print("ENTER\n");

	if (pEventPurchaseInfo->rawEvtName)
	{
		OxCAS_Free(pEventPurchaseInfo->rawEvtName);
		pEventPurchaseInfo->rawEvtName = NULL;
	}
	HxSTD_memset(pEventPurchaseInfo, 0x00, sizeof(EVT_PURCHASE));

	HxLOG_Print("EXIT\n");
}

static TBoolean local_caPpv_IsSamePurchaseInfo(EVT_PURCHASE stEvtPurchase1, EVT_PURCHASE stEvtPurchase2)
{
	if ((stEvtPurchase1.EvtId != stEvtPurchase2.EvtId) ||( stEvtPurchase1.xType != stEvtPurchase2.xType))
	{
		return FALSE;
	}

	if ((stEvtPurchase1.startTime != stEvtPurchase2.startTime) || (stEvtPurchase1.duration != stEvtPurchase2.duration))
	{
		return FALSE;
	}

	if (stEvtPurchase1.DesLength != stEvtPurchase2.DesLength)
	{
		return FALSE;
	}

	if (HxSTD_memcmp(stEvtPurchase1.Des, stEvtPurchase2.Des, stEvtPurchase1.DesLength))
	{
		return FALSE;
	}

	if (HxSTD_memcmp(stEvtPurchase1.EvtName, stEvtPurchase2.EvtName, MAX_STR_LEN))
	{
		return FALSE;
	}

	return TRUE;
}

static void local_caPpv_ClearPurchaseHistory(PPV_PURCHASE_LIST* pList)
{
	HxLOG_Print("ENTER\n");

	if (pList == NULL)
	{
		HxLOG_Print("EXIT - NULL\n");
		return;
	}

	if ((pList->xNbItems > 0) && (pList->pList))
	{
		OxCAS_Free(pList->pList);
		pList->pList = NULL;
	}

	HxSTD_memset(pList, 0x00, sizeof(PPV_PURCHASE_LIST));

	HxLOG_Print("EXIT\n");
}


static TEventStatus local_caPpv_RequestPurchaseHistory(TUnsignedInt32 ulProviderId)
{
	//AP에서 CAK_UIT_REQUEST_PACKAGE_LIST를 uitask에 전달 하면 uitask에서 call 함.
	TCaRequestStatus 	status;
	TCaRequest			*pRequest;
	TEventStatus 		errMsg = CA_EVENT_SUCCESS;
	TUnsignedInt16		i;

	// clear old purchase list
	local_caPpv_ClearPurchaseHistory(&s_stPurchaseList);
	s_stPurchaseList.providerID = ulProviderId; //providerID 저장
	HxLOG_Print("Enter - ulProviderId = 0x%x\n", ulProviderId);

	status = CA_NA_PpvCreateRequest(CA_REQUEST_TYPE_PURCHASE_HISTORY, &pRequest);
	if(status == CA_REQUEST_NO_ERROR)
	{
		status = caRequestSetAsynchronousResponse(pRequest, local_caPpv_PurchaseHistoryCallback);
		if(status == CA_REQUEST_NO_ERROR)
		{
			status = caRequestSend(pRequest);
			if (status == CA_REQUEST_NO_ERROR)
			{
				//여기가 ERROR가 없는 것....이제 callback을 기다려라...
				HxLOG_Print("caRequestSend OKKK!! wait for call back..\n");
				HxLOG_Print("EXIT\n");
				return errMsg;
			}
			else
			{
				errMsg = CA_EVENT_ERROR;
				HxLOG_Print("call caRequestDispose.(%d)\n", status);
			}
		}
		else
		{
			HxLOG_Print("caRequestSetAsynchronousResponse err(%d)\n", status);
		}
	}
	else
	{
		HxLOG_Print("CA_NA_PpvCreateRequest(CA_REQUEST_TYPE_PURCHASE_HISTORY err(%d) \n", status);
	}

	CA_NA_PpvDisposeRequest(pRequest);

	s_stPurchaseList.providerID = 0; //fail이 났으므로, providerID는 다시 init 시킨다.

	//일단 여기까지 오면 ERROR 다.. callback이 있으면 알려라...
	for (i = 0; i < NA_MAX_CALLBACK_NUM; i++)
	{
		//now give purchase list to AP
		if (s_pfnPurchaseHistoryNotifyCallback[i])
		{
			HxLOG_Print("call back[%d] to noti\n", i);
			s_pfnPurchaseHistoryNotifyCallback[i](status, 0, NULL);
		}
		else
		{
			HxLOG_Print("no call back[%d] to give noti...\n", i);
		}
	}

	return errMsg;
}

static void local_caPpv_PurchaseHistoryCallback(TCaRequest* pxRequest)
{
	TSignedInt32	nErrCode;
	NaCasMsg_t 	 	stMsg;

	HxLOG_Print("ENTER\n");

	stMsg.wMsg = NA_CAK_ID;
	stMsg.lParam2 = CAK_PURCHASE_HISTORY_CALL_BACK;
	stMsg.hWnd = (TUnsignedInt32)pxRequest;
	stMsg.lParam3 = 0;

	nErrCode = VK_PostMessage(s_ulPpvQueueId, (void *)&stMsg, QUEUE_SIZE_1);
	if (nErrCode)
	{
		HxLOG_Info("error in VK_PostMessage (0x%x)\n", (unsigned int)nErrCode);
	}

	HxLOG_Print("Exit\n");
}

static HBOOL local_caPpv_checkPMTCasIDForProviderID(TPpid providerID)
{
	TUnsignedInt8		numOfCADesc = 0;
	HBOOL				result;

	HxLOG_Print("ENTER - providerID (0x%x)!!\n", providerID);

	// 1. 현재 NagraTP && MediasetTS : NIT와 PDT(CPDT/HPDT) 확인.  이제 PMT.conditional_access_descriptor  : CA_descriptor.CA_System_ID를 확인 하라..

	if (s_bIsNagraTS == TRUE)
	{
		//여기까지 Nagra TS이다
		// 2.1 Mediaset Packgage List request.. PMT 확인


		HxLOG_Print("currenly Nagra TS, required casID = 0x%x \n", providerID);

		if(s_hCurSvc == HANDLE_NULL)
		{
			HxLOG_Print("s_hCurSvc HANDLE_NULL\n");
			return FALSE;
		}

		HxLOG_Print("s_hCurSvc = 0x%x\n", s_hCurSvc);
		result = local_caPpv_CheckGivenProviderIDInPMT(&numOfCADesc, (TUnsignedInt16)providerID) ;
		HxLOG_Print("local_caPpv_CheckGivenProviderIDInPMT result = %d, numOfCADesc(%d)!!\n", result, numOfCADesc);

		//numOfCADesc가 0 (Free-to-air:clear 구간) 일 경우, 모든 providerID에 true return
		if (numOfCADesc == 0)
		{
			HxLOG_Print("Mediaset requested, Mediaset CA SysemID in PMT FOUND!!\n");
			return TRUE;
		}
		else
		{
			//PMT에 CA Sys ID가 들어 있다.
			if ((providerID == NA_MEDIASET_PROVIDER_ID) && (result == TRUE))
			{
				//Mediaset package list를 request 하였고, PMT에서 Mediaset CA System ID를 찾았다.
				HxLOG_Print("Mediaset requested, Mediaset CA SysemID in PMT FOUND(0x1802~0x1805)!!\n");
				return TRUE;
			}

			if ((providerID == NA_OTHER_PROVIDER_ID) && (result == TRUE))
			{
				//other op package list를 request 하였고, PMT에서 Mediaset CA System ID를 찾았다.
				HxLOG_Print("Multi-Op requested, Multi-OP CA SysemID in PMT FOUND(0x18xx)!!\n");
				return TRUE;
			}

			//위의 두 경우를 제외한 나머지 경우는 return FALSE;
			HxLOG_Print("requested casID(%d) mismatch with PMT ca descriptor(%d)!!\n", providerID, result);
		}
	}
	else
	{
		HxLOG_Print("NO NagraTS,  return FALSE!! \n");
	}

	return FALSE;
}

static void local_caPpv_ResponsePurchaseHistoryCallBack(TCaRequest*  pxRequest)
{
	TCaRequest				*pRequest;
	TCaRequestStatus 		reqStatus;
	TCaObjectStatus 		objStatus;
	TUnsignedInt32 			index;
	TUnsignedInt32 			purchaseEventNb;
	TCaPurchase 			**ppPurchase;
	TEventStatus 			result = CA_EVENT_SUCCESS;
	TCaProcessingStatus 	status;
	TUnsignedInt16 			i = 0, j = 0;

	HxLOG_Print("Enter (%d)\n", pxRequest);

	pRequest = pxRequest;

	local_caPpv_WaitForGivenTimeWithGivenInterval(MAX_PDT_LOADED_WAIT_TIME_INTERVAL, MAX_PDT_LOADED_WAIT_TIME_FOR_PRODUCT);

	reqStatus = caRequestGetProcessingStatus(pRequest, &status);
	if ((reqStatus != CA_REQUEST_NO_ERROR) || (status != CA_PROCESSING_NO_ERROR))
	{
		HxLOG_Info("Processing Status is not correct !!!(0x%x), dispose request!! \n", status);
		result = CA_EVENT_ERROR;
		goto notiToAP;
	}

	CAPPV_PURCHASELIST_SEM_GET();

	reqStatus = caRequestGetObjects(pRequest, &purchaseEventNb, &ppPurchase);
	HxLOG_Print("purchase number = %d, reqStatus(%d)\n", purchaseEventNb, reqStatus);

	if( (reqStatus == CA_REQUEST_NO_ERROR) && (purchaseEventNb > 0) )
	{
		s_stPurchaseList.pList = (PPV_PURCHASE_LIST_ITEM *)OxCAS_Malloc(purchaseEventNb*sizeof(PPV_PURCHASE_LIST_ITEM));

		if  (s_stPurchaseList.pList == NULL)
		{
			result = CA_EVENT_ERROR;
			goto notiToAP;
		}

		HxSTD_memset(s_stPurchaseList.pList, 0x00, purchaseEventNb*sizeof(PPV_PURCHASE_LIST_ITEM));//init...

		s_stPurchaseList.xNbItems = purchaseEventNb;

		for(index=0; index<purchaseEventNb; index++)
		{
			TUnsignedInt8*	pName;
			TCaProduct	*pProduct;
			TSize		 length;

			objStatus = caPurchaseGetProduct(ppPurchase[index], &pProduct);
			if (objStatus == CA_OBJECT_NO_ERROR)
			{
				objStatus = caProductGetId(pProduct, &(s_stPurchaseList.pList[index].ProductID));
				HxLOG_Print("[%d] ProductID(%d), objStatus(%d)\n", index, s_stPurchaseList.pList[index].ProductID, objStatus);
				if (objStatus != CA_OBJECT_NO_ERROR)
				{
					HxSTD_memset(&(s_stPurchaseList.pList[index].ProductID), 0x00, sizeof(TCaProductId));
				}

				// purchase history도 product name을 사용
				objStatus = caProductGetName(pProduct, &length, &pName);
				if ((objStatus == CA_OBJECT_NO_ERROR) && pName)
				{
					HxLOG_Print("[%d] name(%s), length(%d), objStatus(%d)\n", index, pName, length, objStatus);
				}

				if ((objStatus== CA_OBJECT_NO_ERROR) && (length > 0))
				{
					s_stPurchaseList.pList[index].EvtNameLength = length;
					//caProductGetName returns CA_OBJECT_NO_ERROR && purchase_list.pList[index].EvtNameLength > 0
					if (s_stPurchaseList.pList[index].EvtNameLength  <= MAX_PPV_PURCHASE_LIST_NAME_LENGTH)
					{
						MWC_UTIL_DvbStrncpy(s_stPurchaseList.pList[index].EvtName, pName, s_stPurchaseList.pList[index].EvtNameLength);
					}
					else
					{
						s_stPurchaseList.pList[index].EvtNameLength = MAX_PPV_PURCHASE_LIST_NAME_LENGTH;
						MWC_UTIL_DvbStrncpy(s_stPurchaseList.pList[index].EvtName, pName, MAX_PPV_PURCHASE_LIST_NAME_LENGTH);
					}
				}
				else
				{
					HxLOG_Print("[%d], getName ERROR! objStatus(%d), name length(%d) s_bIsNagraTS(%d)\n",
						index, objStatus, s_stPurchaseList.pList[index].EvtNameLength, s_bIsNagraTS));

					if  ((s_stPurchaseList.providerID == NA_FAKE_PROVIDER_ID)  ||// MHP쪽 에서 사용 하는 것으로 MHP는 PMT를 확인 하지 말고 그냥  0을 넣어 준다.
						(local_caPpv_checkPMTCasIDForProviderID(s_stPurchaseList.providerID) == TRUE)) //이름이 없는 경우 현재 요구한 CAS ID의 purchase_list인지 확인 해야 한다. (CAK implementation guide v2.2.2 p39)
					{
						//AP에서는 이 경우 "Event n"을 사용 해야 한다...
						s_stPurchaseList.pList[index].EvtNameLength = 0;
						HxSTD_memset(s_stPurchaseList.pList[index].EvtName, 0x00, MAX_PPV_PURCHASE_LIST_NAME_LENGTH+1);
					}
					else
					{
						//현재 기 요구된 providerID의 purchase_list가 아니니까, provider 의 TS로 tuning하라는 message 를 보여 주어야 한다.
						//이는 cost나 다른 정보는 card에 저장 되어 있지만, 이름 정보는 HPDT에 저장 되어 있기 때문에
						//provider 의 TS로 tuning이 되어야 이를 받아 올 수 있다.
						result = CA_EVENT_TS_FAIL;
						HxLOG_Print("not providerID TS (0x%x) result= CA_EVENT_TS_FAIL!!!!, dispose request!!\n", s_stPurchaseList.providerID);
						goto notiToAP;
					}

				}

				//이 아래것 들은 그냥 error 가 나는 대로 그냥 전달....
				reqStatus = caPurchaseGetExpirationDate(ppPurchase[index], &(s_stPurchaseList.pList[index].ExpirationDate));
				if (reqStatus != CA_REQUEST_NO_ERROR)
				{
					HxSTD_memset(&(s_stPurchaseList.pList[index].ExpirationDate), 0x00, sizeof(TCalendarTime));
				}
				reqStatus = caPurchaseGetDate(ppPurchase[index], &(s_stPurchaseList.pList[index].PurchaseTime));
				if (reqStatus != CA_REQUEST_NO_ERROR)
				{
					HxSTD_memset(&(s_stPurchaseList.pList[index].PurchaseTime), 0x00, sizeof(TCalendarTime));
				}
				reqStatus = caPurchaseGetCost(ppPurchase[index], &(s_stPurchaseList.pList[index].Cost));
				if (reqStatus != CA_REQUEST_NO_ERROR)
				{
					HxSTD_memset(&(s_stPurchaseList.pList[index].Cost), 0x00, sizeof(TCreditAmount));
				}
				reqStatus = caPurchaseGetFlags(ppPurchase[index], &(s_stPurchaseList.pList[index].PurchaseFlag));
				if (reqStatus != CA_REQUEST_NO_ERROR)
				{
					HxSTD_memset(&(s_stPurchaseList.pList[index].PurchaseFlag), 0x00, sizeof(TPurchaseFlags));
				}
			}
			else
			{
				//질문: 아예, product 자체가 문제가 되면 이걸 안 올려 주는 것이 맞나?, 이미 create 할 때, memset을 해서 clear를 해 주었다.
				HxLOG_Print("[%d] caPurchaseGetProduct ERR!,   set to NULL!! objStatus(%d)\n", index, objStatus);
			}//end of objStatus != CA_OBJECT_NO_ERROR
		}//end for

	}
	else if ((reqStatus == CA_REQUEST_NO_ERROR) && (purchaseEventNb == 0) )
	{
		result = CA_EVENT_SUCCESS;
		HxLOG_Print("caRequestGetObjects SUCCESS but  purchaseEventNb == 0, reqStatus(%d), result(%d)\n" , reqStatus, result);
	}
	else
	{
		result = CA_EVENT_ERROR;
		HxLOG_Print("caRequestGetObjects ERR!,  purchaseEventNb(%d), reqStatus(%d), result(%d)\n", purchaseEventNb, reqStatus, result);
	}


	notiToAP:
	// FREE pRequest.
	CA_NA_PpvDisposeRequest(pRequest);

	HxLOG_Print("caRequestDispose(pRequest); successed..., result(%d)\n", result);

	// sorting
	if (result == CA_EVENT_SUCCESS)
	{
		PPV_PURCHASE_LIST_ITEM 	tempItem;
		TUnsignedInt16 			nMaxItem = 0;

		for (i = 0; i < s_stPurchaseList.xNbItems; i++)		// sorting  - Latest item을 가장 위에 보여준다.
		{
			nMaxItem = i;
			for (j = i + 1; j < s_stPurchaseList.xNbItems; j++)
			{
				if (s_stPurchaseList.pList[nMaxItem].PurchaseTime.year < s_stPurchaseList.pList[j].PurchaseTime.year)
					nMaxItem = j;
				else if (s_stPurchaseList.pList[nMaxItem].PurchaseTime.year == s_stPurchaseList.pList[j].PurchaseTime.year)
				{
					if (s_stPurchaseList.pList[nMaxItem].PurchaseTime.month < s_stPurchaseList.pList[j].PurchaseTime.month)
						nMaxItem = j;
					else if ((s_stPurchaseList.pList[nMaxItem].PurchaseTime.month == s_stPurchaseList.pList[j].PurchaseTime.month)
							&& (s_stPurchaseList.pList[nMaxItem].PurchaseTime.day < s_stPurchaseList.pList[j].PurchaseTime.day))
						nMaxItem = j;
				}
			}
			if (nMaxItem != i)
			{
				memcpy(&tempItem, &s_stPurchaseList.pList[i], sizeof(PPV_PURCHASE_LIST_ITEM));
				memcpy(&s_stPurchaseList.pList[i], &s_stPurchaseList.pList[nMaxItem], sizeof(PPV_PURCHASE_LIST_ITEM));
				memcpy(&s_stPurchaseList.pList[nMaxItem], &tempItem, sizeof(PPV_PURCHASE_LIST_ITEM));
			}
		}
	}

	//now give purchase list to AP
	for (i = 0; i < NA_MAX_CALLBACK_NUM; i++)
	{
		if (s_pfnPurchaseHistoryNotifyCallback[i])
		{
			HxLOG_Print("try to give call back[%d] to noti ., result(%d), purchase_list.xNbItems[%d], result(%d)\n", i, result, s_stPurchaseList.xNbItems, reqStatus);
			s_pfnPurchaseHistoryNotifyCallback[i](result, s_stPurchaseList.xNbItems, s_stPurchaseList.pList);
		}
		else
		{
			HxLOG_Print("no call back[%d] to give noti...,  result(%d)\n", i, result);
		}
	}

	CAPPV_PURCHASELIST_SEM_RELEASE();

	HxLOG_Print("EXIT!! (%d)\n");
}


#define ________CA_CAK_PPV_RECHARGE_LIST_________________________________________

static void local_caPpv_ClearRechargeList(RECHARGE_LIST* pList)
{
	HxLOG_Print("ENTER\n");

	if (pList == NULL)
	{
		HxLOG_Print("EXIT - NULL\n");
		return;
	}

	if ((pList->xNbItems > 0) && (pList->pList))
	{
		OxCAS_Free(pList->pList);
		pList->pList = NULL;
	}

	HxSTD_memset(pList, 0x00, sizeof(RECHARGE_LIST));

	HxLOG_Print("EXIT\n");
}

static TEventStatus local_caPpv_RequestRechargeList(void)
{
	//AP에서 CAK_UIT_REQUEST_PACKAGE_LIST를 uitask에 전달 하면 uitask에서 call 함.
	TCaRequestStatus 	status;
	TCaRequest			*pRequest;
	TUnsignedInt16		i;

	// clear old purchase list
	local_caPpv_ClearRechargeList(&s_stRechargeList);

	status = CA_NA_PpvCreateRequest(CA_REQUEST_TYPE_RECHARGE_HISTORY, &pRequest);
	if(status == CA_REQUEST_NO_ERROR)
	{
		status = caRequestSetAsynchronousResponse(pRequest, local_caPpv_RechargeListCallback);
		if(status == CA_REQUEST_NO_ERROR)
		{
			status = caRequestSend(pRequest);
			if (status == CA_REQUEST_NO_ERROR)
			{
				//여기가 ERROR가 없는 것....이제 callback을 기다려라...
				s_stRechargeList.pRequest = pRequest;
				HxLOG_Print("caRequestSend OKKK!! wait for call back.. s_stRechargeList.pRequest(%ld)\n", s_stRechargeList.pRequest);
				HxLOG_Print("EXIT\n");
				return ERR_OK;
			}
			else
			{
				HxLOG_Print("call caRequestDispose.(%d)\n", status);
			}
		}
		else
		{
			HxLOG_Print("caRequestSetAsynchronousResponse err(%d)\n", status);
		}
	}
	else
	{
		HxLOG_Print("CA_NA_PpvCreateRequest(CA_REQUEST_TYPE_RECHARGE_HISTORY err(%d) \n", status);
	}

	CA_NA_PpvDisposeRequest(pRequest);

	//일단 여기까지 오면 ERROR 다.. callback이 있으면 알려라...
	for (i = 0; i < NA_MAX_CALLBACK_NUM; i++)
	{
		//now give purchase list to AP
		if (s_pfnRechargeListNotifyCallback[i])
		{
			HxLOG_Print("call back[%d] to noti\n", i);
			s_pfnRechargeListNotifyCallback[i](status, 0, NULL);
		}
		else
		{
			HxLOG_Print("no call back[%d] to give noti...\n", i);
		}
	}
}

static void local_caPpv_RechargeListCallback(TCaRequest* pxRequest)
{
	TSignedInt32	nErrCode;
	NaCasMsg_t 	 	stMsg;

	HxLOG_Print("ENTER\n");

	stMsg.wMsg = NA_CAK_ID;
	stMsg.lParam2 = CAK_RECHARGE_LIST_CALL_BACK;
	stMsg.hWnd = pxRequest;
	stMsg.lParam3 = 0;

	nErrCode = VK_PostMessage(s_ulPpvQueueId, (void *)&stMsg, QUEUE_SIZE_1);
	if (nErrCode)
	{
		HxLOG_Info("error in VK_PostMessage (0x%x)\n", (unsigned int)nErrCode);
	}

	HxLOG_Print("Exit\n");
}

static void local_caPpv_ResponseRechargeListCallBack(TCaRequest*  pxRequest)
{
	// 데이터 리스트를 만들어냄
	TCaRequestStatus 		reqStatus;
	TCaProcessingStatus 	procStatus= CA_PROCESSING_NO_ERROR;
	TUnsignedInt32 			index;
	TUnsignedInt32 			rechargeNb;
	TCaRecharge				**ppRecharge;
	TEventStatus 			result = CA_EVENT_ERROR;
	TCaListenerStatus 		listenerStatus;
	TCaRequest				*pRequest;
	TUnsignedInt16			i = 0;

	HxLOG_Print("ENTER - pxRequest(%ld), s_stRecharge_list.pReques(%dl)\n", pxRequest, s_stRechargeList.pRequest);

	pRequest = pxRequest;

	if ((pxRequest == NULL) && ( s_stRechargeList.pRequest == NULL))
	{
		HxLOG_Print("EXIT - pxRequest == NULL or recharge_list.pRequest == NULL\n");
		return;
	}

	// 사실 ap, cak, sc의 요구가 겹쳐서 들어올 리가 없으나 만약을 대비해서
	//들어온 요청은 모두 실행해주도록 함
	//하나만 처리하고 싶으면 아래 if-if-if를 if-else if-else if로 해주면 된다.
	reqStatus = caRequestGetProcessingStatus(pRequest, &procStatus);

	if ((reqStatus != CA_REQUEST_NO_ERROR) || (procStatus != CA_PROCESSING_NO_ERROR))
	{
		HxLOG_Print("Processing Status is not correct !!! %d\n", procStatus);
		CA_NA_PpvDisposeRequest(pRequest);
		//call back에 알려 줄 필요 없지???/
		return;
	}

	CAPPV_RECHARGE_SEM_GET();
	if ((s_stRechargeList.pRequest == NULL) || (pRequest != s_stRechargeList.pRequest))
	{
		HxLOG_Print("this callback is not for recent recharge history request call back!![0x%x VS ", pRequest);
		if (s_stRechargeList.pRequest == NULL)
		{
			HxLOG_Print("pRequest NULL]\n");
		}
		else
		{
			HxLOG_Print("0x%x(recharge_list)]\n", s_stRechargeList.pRequest);
		}

		CAPPV_RECHARGE_SEM_RELEASE();
		CA_NA_PpvDisposeRequest(pRequest);
		//call back에 알려 줄 필요 없지???/
		return;
	}

	// 1. callback for AP - recharge list만들어서 ap에게 callback 형태로 알려준다
//	if(gRechargeRequestTask[RECHARGE_REQUEST_FROM_AP])
	{
		HxLOG_Print("previouse list xNbItems[%ld]\n", s_stRechargeList.xNbItems);

		reqStatus = caRequestGetObjects(pRequest, &rechargeNb, &ppRecharge);
		HxLOG_Print("rechargeNb = %ld\n", rechargeNb);

		if ((reqStatus == CA_REQUEST_NO_ERROR) &&  (rechargeNb > 0) )
		{
			s_stRechargeList.pList = OxCAS_Malloc(rechargeNb*sizeof(RECHARGE_LIST_ITEM));

			if (s_stRechargeList.pList == NULL) // 4
			{
				HxLOG_Print("recharge_list.pList MAM alloc FAIL>>>\n");
			}
			else // 4
			{
				HxLOG_Print("recharge_list.pList MAM alloc OKK>>>\n");
				result = CA_EVENT_SUCCESS;
				s_stRechargeList.xNbItems = rechargeNb;

				// cak에서는 오래 -> 최신 순서로 주지만, AP나 MW에는 최신 -> 오래된 순서로 넣어 준다.
				for(index= 0; index < rechargeNb; index++)
				{
					reqStatus = caRechargeGetReason(ppRecharge[index], &(s_stRechargeList.pList[index].Reason));
					if (reqStatus != CA_REQUEST_NO_ERROR)
					{
						HxSTD_memset(&(s_stRechargeList.pList[index].Reason), 0x00, sizeof(TCaRechargeReason));
					}

					reqStatus = caRechargeGetDate(ppRecharge[index], &(s_stRechargeList.pList[index].Date));
					if (reqStatus != CA_REQUEST_NO_ERROR)
					{
						HxSTD_memset(&(s_stRechargeList.pList[index].Date), 0x00, sizeof(TCalendarTime));
					}

					reqStatus = caRechargeGetAmount(ppRecharge[index], &(s_stRechargeList.pList[index].Amount));
					if (reqStatus != CA_REQUEST_NO_ERROR)
					{
						HxSTD_memset(&(s_stRechargeList.pList[index].Amount), 0x00, sizeof(TCreditAmount));
					}
				}

				HxLOG_Print("out of for (index =%d)>>> \n", index);
			} // 4
		}
		else if ((reqStatus == CA_REQUEST_NO_ERROR)&& (rechargeNb == 0))
		{
			result = CA_EVENT_SUCCESS;
			HxLOG_Print("(reqStatus == CA_REQUEST_NO_ERROR) && (rechargeNb == 0, result = CA_EVENT_SUCCESS......  \n");
		}
		else
		{
			HxLOG_Print("caRequestGetObjects ERR!! reqStatus(%d),set gRechargeRequestTask[RECHARGE_REQUEST_FROM_AP]=0\n", reqStatus);
		}
	}
	CAPPV_RECHARGE_SEM_RELEASE();

//	if(gRechargeRequestTask[RECHARGE_REQUEST_FROM_AP])
	{
//		gRechargeRequestTask[RECHARGE_REQUEST_FROM_AP] = 0;

		for (i = 0; i < NA_MAX_CALLBACK_NUM; i++)
		{
			if(s_pfnRechargeListNotifyCallback[i])
			{
				s_pfnRechargeListNotifyCallback[i](result , s_stRechargeList.xNbItems, s_stRechargeList.pList);
				HxLOG_Print("noti to AP callback[%d] SUCCESSED !!, reqStatus[%d]\n", i, reqStatus);
			}
			else
			{
				HxLOG_Print("NO AP callback[%d] EXIST!!, reqStatus[%d]\n", i, reqStatus);
			}
		}
	}// end of if(gRechargeRequestTask[RECHARGE_REQUEST_FROM_AP])

	CA_NA_PpvDisposeRequest(pRequest);

	HxLOG_Print("EXIT\n");
}

static void local_caPpv_PurchaseEventCallback(TCaRequest* pxRequest)
{
	TSignedInt32	nErrCode;
	NaCasMsg_t 	 	stMsg;

	HxLOG_Print("ENTER\n");

	stMsg.wMsg = NA_CAK_ID;
	stMsg.lParam2 = CAK_IPPV_PURCHASE_RESPONSE;
	stMsg.hWnd = (TUnsignedInt32)pxRequest;
	stMsg.lParam3 = 0;

	nErrCode = VK_PostMessage(s_ulPpvQueueId, (void *)&stMsg, QUEUE_SIZE_1);
	if (nErrCode)
	{
		HxLOG_Info("error in VK_PostMessage (0x%x)\n", (unsigned int)nErrCode);
	}

	HxLOG_Print("Exit\n");
}

static TCaRequestStatus local_caPpv_RequestWatchPPT(TCaProcessingStatus *procStatus)
{
	//Watch PPT 설정....
	TCaRequestStatus		reqStatus = CA_REQUEST_ERROR;
	TCaRequest			*pWatchRequest = NULL;
	TCaRequest			*pProgramRequest = NULL;
	TUnsignedInt32		numObject = 0;
	TCaProduct 			**ppxProductArray1;
	int i;
	TCaProcessingStatus 	pprocStatus =CA_PROCESSING_ERROR;

	//confirmation
	reqStatus = CA_NA_PpvCreateRequest(CA_REQUEST_TYPE_WATCH_PPT, &pWatchRequest);
	if (reqStatus == CA_REQUEST_NO_ERROR)
	{
		reqStatus = CA_NA_PpvCreateRequest(CA_REQUEST_TYPE_PROGRAMS, &pProgramRequest);
		if (reqStatus == CA_REQUEST_NO_ERROR)
		{
			reqStatus = caRequestSend(pProgramRequest);
			if (reqStatus == CA_REQUEST_NO_ERROR)
			{
				reqStatus = caRequestGetProcessingStatus(pProgramRequest, &pprocStatus);//결과는 그냥 AP로 전달 하면 된다.

				if ( (reqStatus == CA_REQUEST_NO_ERROR) && (pprocStatus == CA_PROCESSING_NO_ERROR) )
				{
					reqStatus = caRequestGetObjects(pProgramRequest, &numObject, &ppxProductArray1);

					if ( (reqStatus == CA_REQUEST_NO_ERROR) && (numObject > 0))
					{
						for (i = 0; i < numObject ; i++)
						{
							reqStatus = caRequestSetProgram(pWatchRequest, ppxProductArray1[i]);

							if (reqStatus == CA_REQUEST_NO_ERROR)
							{
								reqStatus = caRequestSend(pWatchRequest);

								if (reqStatus == CA_REQUEST_NO_ERROR)
								{
									reqStatus = caRequestGetProcessingStatus(pWatchRequest, &pprocStatus);//결과는 그냥 AP로 전달 하면 된다.

									if ((reqStatus == CA_REQUEST_NO_ERROR) && (pprocStatus == CA_PROCESSING_NO_ERROR) )
									{
#if 0 //debugging....
{
										TCaProduct * pxProduct;
										TCaService * pxServiceFilter;
										TUnsignedInt32 pxNumberOfEvents = 0;
										TCaEvent  ** pppxEventArray;
										TCaAccess pAccessTemp;
										int i = 0;
										//check status of event
										caProgramGetAccess(ppxProductArray1[i], &pAccessTemp);
										HxLOG_Print("\n\n\t[INT_UIT_requestPurchasePPT]--!! FINAL CHECK, START ***********\n");
										HxLOG_Print("\t[INT_UIT_requestPurchasePPT]    caProgramGetAccess = %d\n", pAccessTemp);
										caProductGetEvents(ppxProductArray1[i], NULL, &pxNumberOfEvents, &pppxEventArray);
										for (i = 0; i < pxNumberOfEvents; i++)
										{
											caEventGetAccess(pppxEventArray[i], &pAccessTemp);
											HxLOG_Print("[%d] [INT_UIT_requestPurchasePPT]    caEventGetAccess = %d\n", i, pAccessTemp);
										}
										HxLOG_Print("FINAL CHECK, END ***********\n\n\n");
}
#endif
										HxLOG_Print("caRequestGetProcessingStatus(pWatchRequest, pxProductArray1[%d],OKKKK!!(%d)\n", i, reqStatus);
										break;
									}
									else
									{
										HxLOG_Print("caRequestGetProcessingStatus(pWatchRequest, pxProductArray1[%d] ERR, reqStatus(%d), proStatus(%d)\n", i, reqStatus,  procStatus);
									}
								}
								else
								{
									HxLOG_Print("caRequestSend(pWatchRequest),  error(%d)\n",  reqStatus);
								}
							}
							else
							{
								HxLOG_Print("caRequestSetProgram(pWatchRequest, pxProductArray1[%d], error(%d)\n", i, reqStatus);
							}
						}
					}
					else
					{
						HxLOG_Print("caRequestGetObjects(CA_REQUEST_TYPE_PROGRAMS) err!!, reqStatus(%d)\n", reqStatus);
					}
				}
				else
				{
					HxLOG_Print("caRequestGetProcessingStatus(CA_REQUEST_TYPE_PROGRAMS) err!!, reqStatus(%d), proStatus(%d)\n", reqStatus, procStatus);
				}
			}
			else
			{
				HxLOG_Print("caRequestSend(CA_REQUEST_TYPE_PROGRAMS) err!!, reqStatus(%d)\n", reqStatus);
			}
		}
		else
		{
			HxLOG_Print("CA_NA_PpvCreateRequest(CA_REQUEST_TYPE_PROGRAMS) err!!, reqStatus(%d)\n", reqStatus);
		}
	}
	else
	{
		HxLOG_Print("CA_NA_PpvCreateRequest(CA_REQUEST_TYPE_WATCH_PPT) err!!, reqStatus(%d)\n", reqStatus);
	}

	CA_NA_PpvDisposeRequest(pWatchRequest);
	CA_NA_PpvDisposeRequest(pProgramRequest);

	*procStatus = pprocStatus;
	return reqStatus;
}

static HERROR local_caPpv_RequestPurchasePPT(TCaEvent *pEvent, TCaProduct *pProduct)
{
	//드디어 산다... 여긴 synchronouse mode....
	TCaRequestStatus	reqStatus = CA_REQUEST_ERROR;
	TCaProcessingStatus	procStatus = CA_PROCESSING_ERROR;
	TCaRequest			*pPPTRequest = NULL;
	TCaObjectStatus			objStatus = CA_OBJECT_NO_ERROR;
	TCaAccess			pAccess;

	if ( (pEvent == NULL) || (pProduct == NULL) )
	{
		HxLOG_Print("(pEvent ||pProduct) = NULL!!! END==> \n");
		goto disposeRequest;
	}

	HxLOG_Print("[INT_UIT_requestPurchasePPT] HERE Synch Mode------>Enter\n");

	local_caPpv_WaitForGivenTimeWithGivenInterval(MAX_PDT_LOADED_WAIT_TIME_INTERVAL, MAX_PDT_LOADED_WAIT_TIME);


	//check caEvent.access status
	objStatus = caEventGetAccess(pEvent, &pAccess);

	if (objStatus != CA_OBJECT_NO_ERROR)
	{
		goto disposeRequest;
	}

	HxLOG_Print("pAccess (%d) \n", pAccess);

	if (pAccess == CA_ACCESS_DENIED)
	{
		//purchase PPT Product
		//create the product purchase request
		reqStatus = CA_NA_PpvCreateRequest(CA_REQUEST_TYPE_PRODUCT_PURCHASE, &pPPTRequest);

		if (reqStatus == CA_REQUEST_NO_ERROR)
		{
			reqStatus = caRequestSetProduct(pPPTRequest, pProduct);
			if (reqStatus == CA_REQUEST_NO_ERROR)
			{
				reqStatus = caRequestSend(pPPTRequest);
				if (reqStatus == CA_REQUEST_NO_ERROR)
				{
					reqStatus = caRequestGetProcessingStatus(pPPTRequest, &procStatus);//결과는 그냥 AP로 전달 하면 된다.

					if ( (reqStatus == CA_REQUEST_NO_ERROR) && (procStatus == CA_PROCESSING_NO_ERROR) )
					{
						//NOW can watch PPT, set the WATCH_PPT
						reqStatus = local_caPpv_RequestWatchPPT(&procStatus );
					}
					else
					{
						HxLOG_Print("caRequestGetProcessingStatus error !!, reqStatus(%d), procStatus(%d)\n", reqStatus, procStatus);
					}
				}
				else
				{
					HxLOG_Print("caRequestSend(pRequest) ERROR (%d)\n", reqStatus);
				}
			}
			else
			{
				HxLOG_Print("caRequestSetProduct ERROR (%d) \n", reqStatus);
			}
		}
		else
		{
			HxLOG_Print("CA_NA_PpvCreateRequest(CA_REQUEST_TYPE_PRODUCT_PURCHASE) ERROR (%d) \n", reqStatus);
		}

		HxLOG_Print("caRequestGetProcessingStatus RESULT!! reqStatus(%d), procStatus(%d) \n", reqStatus, procStatus);

	}
	else if (pAccess == CA_ACCESS_DENIED_NO_VALID_CREDIT)
	{
		procStatus = CA_PROCESSING_LOW_CREDIT;
		HxLOG_Print("caRequestGetProcessingStatus RESULT!! reqStatus(%d), procStatus(%d) \n", reqStatus,procStatus);

	}
	else
	{
		//여기서는 다시 request purchase를 하지 않고 다만 watchppt만 하면 된다.
		HxLOG_Print("pAccess(%d) : set the WATCH PPT with program!! \n", pAccess);
		reqStatus = local_caPpv_RequestWatchPPT(&procStatus);
		HxLOG_Print("pAccess(%d) : result : reqStatus(%d), procStatus(%d)!\n", pAccess, reqStatus, procStatus);
	}

disposeRequest:

	//(reqStatus == CA_REQUEST_NO_ERROR) && (procStatus == CA_PROCESSING_ERROR) 일 경우만 purchase SUCCESS 이다.
	if (reqStatus != CA_REQUEST_NO_ERROR)
	{
		procStatus = CA_PROCESSING_ERROR;
	}

	//dispose pRequest for purchase.....
	CA_NA_PpvDisposeRequest(pPPTRequest);

	HxLOG_Print("caRequestGetProcessingStatus RESULT!! reqStatus(%d), procStatus(%d) \n", reqStatus, procStatus);

notiToAPP:

	//AP로 결과 전달...
	if (s_pfnPurchaseEventNotifyCallback)
		s_pfnPurchaseEventNotifyCallback((TUnsignedInt32)procStatus, CA_EVENT_PPT);

	return ERR_OK;
}

static HERROR local_caPpv_RequestPurchaseSpecialEvent(TCaProduct *pProduct)
{
	//드디어 산다... 여긴 synchronouse mode....
	TCaRequestStatus		reqStatus = CA_REQUEST_ERROR;
	TCaProcessingStatus		procStatus = CA_PROCESSING_ERROR;
	TCaRequest*				pRequest = NULL;

	if (pProduct == NULL)
	{
		HxLOG_Print("pProduct == NULL!!! END==> \n");
		goto disposeRequest;
	}

	HxLOG_Print("HERE Synch Mode------>Enter \n");


	local_caPpv_WaitForGivenTimeWithGivenInterval(MAX_PDT_LOADED_WAIT_TIME_INTERVAL, MAX_PDT_LOADED_WAIT_TIME);


	//create the product purchase request
	reqStatus = CA_NA_PpvCreateRequest(CA_REQUEST_TYPE_PRODUCT_PURCHASE, &pRequest);
	HxLOG_Print("CA_REQUEST_TYPE_PRODUCT_PURCHASE - pRequest = 0x%x, pProduct = 0x%x\n", pRequest, pProduct);

	if (reqStatus == CA_REQUEST_NO_ERROR)
	{
		reqStatus = caRequestSetProduct(pRequest, pProduct);
		if (reqStatus == CA_REQUEST_NO_ERROR)
		{
			reqStatus = caRequestSend(pRequest);
			if (reqStatus == CA_REQUEST_NO_ERROR)
			{
				reqStatus = caRequestGetProcessingStatus(pRequest, &procStatus);
				if ((reqStatus == CA_REQUEST_NO_ERROR) && (procStatus == CA_PROCESSING_NO_ERROR) )
				{
					//결과는 그냥 AP로 전달 하면 된다.  여기서는 할 것이 없다... 끝...
					HxLOG_Print("caRequestGetProcessingStatus OKKK!!  result !!, reqStatus(%d), procStatus(%d)\n", reqStatus, procStatus);
				}
				else
				{
					HxLOG_Print("caRequestGetProcessingStatus ERR!!  procStatus(%d), reqStatus(%d)\n", procStatus, reqStatus);
				}
			}
			else
			{
				HxLOG_Print("caRequestSend ERROR (%d) \n", reqStatus);
			}
		}
		else
		{
			HxLOG_Print("caRequestSetProduct ERROR (%d) \n", reqStatus);
		}
	}
	else
	{
		HxLOG_Print("CA_NA_PpvCreateRequest(CA_REQUEST_TYPE_PRODUCT_PURCHASE) ERROR (%d) \n", reqStatus);
	}

	HxLOG_Print("caRequestGetProcessingStatus RESULT!! reqStatus(%d), procStatus(%d) \n", reqStatus,procStatus);

disposeRequest:

	//(reqStatus == CA_REQUEST_NO_ERROR) && (procStatus == CA_PROCESSING_ERROR) 일 경우만 purchase SUCCESS 이다.
	if (reqStatus != CA_REQUEST_NO_ERROR)
	{
		procStatus = CA_PROCESSING_ERROR;
	}

	//일단 한번 send를 하고 나서는 더 이상 retry 하지 않고 purchase 관련 request를 dispose 시킨다.  질문 : 과연 그런가...??? error시 어찌 처리 해야 하나...KIMYS_NA
	//dispose pRequest for purchase.....
	CA_NA_PpvDisposeRequest(pRequest);

	HxLOG_Print("caRequestGetProcessingStatus RESULT!! reqStatus(%d), procStatus(%d) \n", reqStatus,procStatus);

notiToApp:

	//AP로 결과 전달...
	if (s_pfnPurchaseEventNotifyCallback)
	{
		s_pfnPurchaseEventNotifyCallback((TUnsignedInt32)procStatus, CA_EVENT_SPECIAL_EVENT);
	}

	return ERR_OK;
}

static void local_caPpv_WaitEIT_TimerCallback(unsigned long TimerId,  void *pArg)
{
	TSignedInt32	nErrCode;
	NaCasMsg_t 	 	stMsg;

	HxLOG_Print("ENTER - TimerId = %d, s_ulEITwaitTimerId = %d\n", TimerId, s_ulEITwaitTimerId);

	if (TimerId == 0 || TimerId != s_ulEITwaitTimerId)
	{
		HxLOG_Print("params is NULL or TimerID is NULL\n");
		return;
	}

	stMsg.wMsg = NA_CAK_ID;
	stMsg.lParam2 = CAK_PPV_EIT_TIMER_CALL_BACK;

	nErrCode = VK_PostMessageTimeout(s_ulPpvQueueId, (void *)&stMsg, QUEUE_SIZE_1);
	if (nErrCode)
	{
		HxLOG_Info("error in VK_PostMessage (0x%x)\n", (unsigned int)nErrCode);
	}

	s_ulEITwaitTimerId = 0;

	HxLOG_Print("Exit\n");
}

static void local_caPpv_ResponseNewRecharge(void)
{
	TUnsignedInt16 index;

	for (index = 0; index < NA_MAX_CALLBACK_NUM; index ++)
	{
		if (s_pfnRechargeAlarmNotifyCallback[index])
		{
			s_pfnRechargeAlarmNotifyCallback[index](CA_REQUEST_NO_ERROR, 0, 0);
			HxLOG_Print("s_pfnRechargeAlarmNotifyCallback[%d] called\n", index);
		}
		else
		{
			HxLOG_Print("s_pfnRechargeAlarmNotifyCallback[%d] NULL\n", index);
		}
	}
}

static void local_caPpv_ClearOPPVPurchaseInfo(OPPV_PURCHASE_INFO* OppvInfo)
{
	OPPV_PURCHASE_INFO_ITEM  	*traveler;
	OPPV_PURCHASE_INFO_ITEM  	*freeItem;
	TUnsignedInt16				index = 0;

	HxLOG_Print("ENTER!!\n");

	if (OppvInfo == NULL)
	{
		HxLOG_Print("OppvInfo(NULL) called, can't be here!!!!!!\n\n\n");
		return;
	}

	//arg로 받은 OPPV_PURCHASE_INFO를 clear 시켜서 초기화 시켜 준다.  또 쓰게...
	traveler = OppvInfo->pOPPVItem;
	while (traveler)
	{
		freeItem = traveler;
		traveler = freeItem->pNext;

		if (freeItem->pName)
		{
			HxLOG_Print("[%d] MEM free : name(%s), nameLength(%d)\n",	index, freeItem->pName,freeItem->nameLength);
			OxCAS_Free(freeItem->pName);
			freeItem->pName = NULL;
		}
		freeItem->nameLength = 0;
		OxCAS_Free(freeItem);
		freeItem = NULL;
		index++;
	}

	HxLOG_Print("finished [mem free index:%d][xNbItems:%d] \n", index, OppvInfo->xNbItems);
	OppvInfo->xNbItems = 0;
	OppvInfo->pOPPVItem = NULL;

	HxLOG_Print("EXIT!!\n");
}


static void local_caPpv_ResponseNewOppv(TCaExportation* 	pxExportation)
{
	TCaObjectStatus 		objStatus;
	TUnsignedInt32			ulNumPurchases;
	TCaPurchase				**ppPurchases;
	TUnsignedInt32			realItemCounter = 0; //실제로 OPPV purchase info list에 등록 되는 purchase item number
	OPPV_PURCHASE_INFO_ITEM *traveler;
	TUnsignedInt16 			index = 0;

	if (pxExportation == NULL)
	{
		HxLOG_Print("related exportation is NULL!! do nothing~~\n");
		return;
	}

	local_caPpv_WaitForGivenTimeWithGivenInterval(MAX_PDT_LOADED_WAIT_TIME_INTERVAL, MAX_PDT_LOADED_WAIT_TIME_FOR_PRODUCT);

	//curOPPVPurchaseInfo init
	local_caPpv_ClearOPPVPurchaseInfo(&s_stLastOPPVPurchaseInfo);


	//now pxExportaion in not NULL
	objStatus = caExportationGetObjects(pxExportation, &ulNumPurchases, &ppPurchases);

	if ((objStatus == CA_OBJECT_NO_ERROR) && (ulNumPurchases > 0)) // 1.
	{
		HxLOG_Print("numPurchases[%d]\n", ulNumPurchases);

		for (index = 0; index < ulNumPurchases; index++) // 2.
		{
			if (ppPurchases[index] != NULL) // 3
			{
				OPPV_PURCHASE_INFO_ITEM *newPurchaseInfoItem = NULL;
				TPurchaseFlags	flags;

				//get the purchase flags;
				objStatus = caPurchaseGetFlags(ppPurchases[index], &flags);
				HxLOG_Print("[%d] objStatus(%d), flags[0x%x] \n", index, objStatus, flags);

				if ((objStatus == CA_OBJECT_NO_ERROR) && (!(flags & CA_PURCHASE_THROUGH_CAK_API)))   // 4
				{
					TCaProduct		*pProduct = NULL;
					TUnsignedInt8 	*pName;
					TSize		 	nameLength;

					// get the related product in order to retreive the name
					objStatus = caPurchaseGetProduct(ppPurchases[index], &pProduct);
					HxLOG_Print("[%d]after caPurchaseGetProduct, status(%d)\n", index, objStatus);
					if ((objStatus == CA_OBJECT_NO_ERROR) && (pProduct != NULL)) // 6
					{
						//get the name of the product : OPPV는 product name을 보여 준다.
						objStatus = caProductGetName(pProduct, &nameLength, &pName);
						HxLOG_Print("[%d] after nameLength(%d), objStatus(%d)\n", index, nameLength, objStatus);

						//if ( (objStatus == CA_OBJECT_NO_ERROR) && (nameLength > 0) && (pName != NULL)) // 7
						if (1)//if (objStatus == CA_OBJECT_NO_ERROR)  // 7		// 이름을 못 가져와도 빈칸으로 뿌려는 주자
						{
							// 가끔 이름이 없이 오는 경우 space 2개를 넣어 준다.
							realItemCounter++;

							if (pName)
							{
								HxLOG_Print("[%d][real:%d] flags:0x%x, nameLength:%d, pName:%s\n", index, realItemCounter, flags, nameLength, pName);
							}

							newPurchaseInfoItem = (OPPV_PURCHASE_INFO_ITEM*)OxCAS_Malloc(sizeof(OPPV_PURCHASE_INFO_ITEM));
							if (newPurchaseInfoItem != NULL) // 8
							{
								HxSTD_memset(newPurchaseInfoItem, 0x00, sizeof(OPPV_PURCHASE_INFO_ITEM));

								if (nameLength <= 0)
								{
									newPurchaseInfoItem->nameLength = 2; //spage 2개
								}
								else if (nameLength <= MAX_EVENT_ITEM_NAME_LENGTH)
								{
									newPurchaseInfoItem->nameLength = nameLength;
								}
								else
								{
									//여기서의 length는 null을 포함 하지 않는 length이다.
									newPurchaseInfoItem->nameLength = MAX_EVENT_ITEM_NAME_LENGTH;
								}

								newPurchaseInfoItem->pName = (TUnsignedInt8*)OxCAS_Malloc((newPurchaseInfoItem->nameLength+1) * sizeof(TUnsignedInt8));
								if (newPurchaseInfoItem->pName != NULL) // 9
								{
									//Null terminator 추가
									HxSTD_memset(newPurchaseInfoItem->pName, 0x00, (newPurchaseInfoItem->nameLength+1));

									if (pName)
									{
										MWC_UTIL_DvbStrncpy(newPurchaseInfoItem->pName, pName, newPurchaseInfoItem->nameLength);
									}
									else
									{
										MWC_UTIL_DvbStrncpy(newPurchaseInfoItem->pName, "  ", newPurchaseInfoItem->nameLength);// pName이 null이 올라 올 경우 , space 2개를 넣어 준다.
									}
									newPurchaseInfoItem->pNext = NULL;

									//attach newPurchaseInfoItem to curOPPVPurchaseInfo
									s_stLastOPPVPurchaseInfo.xNbItems++;// increase s_stLastOPPVPurchaseInfo item number by one
									if (s_stLastOPPVPurchaseInfo.xNbItems == 1) //attach first item node to s_stLastOPPVPurchaseInfo
									{
										s_stLastOPPVPurchaseInfo.pOPPVItem = newPurchaseInfoItem;
									}
									else
									{
										traveler->pNext = newPurchaseInfoItem;
									}

									traveler = newPurchaseInfoItem;
								}
								else // 9. if ((newPurchaseInfoItem->pName != NULL)
								{
									HxLOG_Info("[%d] MEM alloc ERROR!!newPurchaseInfoItem->pName == NULL\n", index);
								}
							}
							else // 8. if (newPurchaseInfoItem != NULL)
							{
								HxLOG_Info("MEM alloc ERROR!!newPurchaseInfoItem == NULL\n");
							}
						}
						else
						{
							// 7. if (objStatus == CA_OBJECT_NO_ERROR)
							HxLOG_Info("caPurchaseGetName ERROR!!objStatus(%d)\n", objStatus);
						}
					}
					else// 6. if (objStatus == CA_OBJECT_NO_ERROR)
					{
						HxLOG_Info("caPurchaseGetProduct ERROR!!objStatus(%d)\n", objStatus);
					}
				}
				else // 4. if (objStatus == CA_OBJECT_NO_ERROR)
				{
					HxLOG_Info("caPurchaseGetFlags ERROR!!(flags:%d), objStatus(%d)\n", flags, objStatus);
				}
			}
			else // 3. if (ppPurchases[i] != NULL)
			{
				HxLOG_Info("ppPurchases[%d] == NULL!!\n", index);
			}
		} // 2. for (i = 0; i < numPurchases; i++)

		HxLOG_Info("END(%ld)\n", ulNumPurchases);
	}
	else // 1. if (objStatus == CA_OBJECT_NO_ERROR)
	{
		HxLOG_Info("caExportationGetObjects ERROR or numPurchases[%ld]\n", ulNumPurchases);
	}

	//dispose exportation from CAK
	if (pxExportation)
	{
		caExportationDispose(pxExportation);
	}

	for (index = 0; index < NA_MAX_CALLBACK_NUM; index ++)
	{
		if (s_pfnOPPVPurchaseListNotifyCallback[index])
		{
			s_pfnOPPVPurchaseListNotifyCallback[index](objStatus, &s_stLastOPPVPurchaseInfo);
			HxLOG_Print("s_pfnOPPVPurchaseListNotifyCallback[%d] called\n", index);
		}
		else
		{
			HxLOG_Print("s_pfnOPPVPurchaseListNotifyCallback[%d] NULL\n", index);
		}
	}
}

void local_caPpv_SetFullInfoPackageListRequestStatus(TBoolean flag)
{
	HxLOG_Print("--> ENTER, old Flag = %s\n", (s_bIsFullInfoPackageRequested==TRUE)?"TRUE":"FALSE");
	s_bIsFullInfoPackageRequested = flag;
	HxLOG_Print("<-- EXIT, new Flag = %s\n", (s_bIsFullInfoPackageRequested==TRUE)?"TRUE":"FALSE");
}

void local_caPpv_GetFullInfoPackageListRequestStatus(TBoolean *flag)
{
	HxLOG_Print("s_bIsFullInfoPackageRequested = %s\n", (s_bIsFullInfoPackageRequested==TRUE)?"TRUE":"FALSE");
	*flag = s_bIsFullInfoPackageRequested;
}

static void local_caPpv_MakeFullInfoPackageList(TCaRequest* pRequest, TUnsignedInt32 xStatus)
{
	TBoolean 		requestStatus = FALSE;
	PACKAGE_INFO 	*pPackageInfo = NULL;

	HxLOG_Print("requestStatus = %s, ----------> ENTER!! (%d) \n", (requestStatus==TRUE)?"TRUE":"FALSE");

	local_caPpv_GetFullInfoPackageListRequestStatus(&requestStatus);

	HxLOG_Print("2 requestStatus = %s, ----------> ENTER!! (%d) \n", (requestStatus==TRUE)?"TRUE":"FALSE");

	if (requestStatus == TRUE)
	{
		HxLOG_Print("NOW check pRequest!!\n");

		CAPPV_PACKAGE_SEM_GET();

		//call back이 진짜 현재 원하는 것인지 확인... 이전 request에 의한 것이면 버려라....
		if ((pRequest == NULL) || ((s_stPackageList.pRequest != NULL) && s_stPackageList.pRequest != pRequest))
		{
			HxLOG_Print("<---- WRONG request. call back");
			if (pRequest != NULL)
			{
				HxLOG_Print(", pRequest from CAK : 0x%x", pRequest);
			}

			if (s_stPackageList.pRequest != NULL)
			{
				HxLOG_Print(", current pRequest : 0x%x", s_stPackageList.pRequest);
			}
			HxLOG_Print("\n");

			HxLOG_Print("no need to send noti to call back!!< ----------EXIT!! \n");
			CAPPV_PACKAGE_SEM_RELEASE();
			return;
		}

		CAPPV_PACKAGE_SEM_RELEASE();

		if (xStatus == CA_EVENT_TS_FAIL)
		{
			HxLOG_Print("xStatus == CA_EVENT_TS_FAIL, no need to send noti to call back!!< ----------EXIT!! \n");
			return;

		}
		else if (xStatus == CA_EVENT_ERROR)
		{
			HxLOG_Print("xStatus == CA_EVENT_ERROR, no need to send noti to call back!!< ----------EXIT!! \n");
			return;
		}

		//여기서 부터는, xStatus == CA_EVENT_SUCCESS

		HxLOG_Print("(xStatus = %d), OK, I need to make package INFO!!\n", xStatus);

		//내가 원하는 package_list가 맞다.. 현재까지는 ...두번이 올 수 없으므로, 여기서 package_list info를 만들어라...
		//자 이제 부터 새로운 package_list info 구조를 만든다.. 여기 이 xNbEvents의 숫자를 믿는다..
		if ((s_stPackageList.pList!= NULL) && (s_stPackageList.xNbItems > 0 ) )
		{
			PPV_PACKAGE_LIST_ITEM *holder, *traveler;
			int index = 0;

			HxLOG_Print("package_list.pRequest = 0x%x, pRequest = 0x%x, xNbItems = %d\n", s_stPackageList.pRequest, pRequest, s_stPackageList.xNbItems);

			CAPPV_PACKAGE_SEM_GET();
			traveler = s_stPackageList.pList;

			while (traveler)
			{
				//move to next node;
				holder = traveler->pNext;
				HxLOG_Print("[%d]  START ============>  \n", index);

				//PACKAGE INFO clear

				HxSTD_memset(&traveler->PackageInfo, 0x00, sizeof (PACKAGE_INFO));

				HxLOG_Print("[%d]  request PACKAGE_INFO....... \n", index);

				//NOW make new PACKAGE INFO
				CA_NA_PpvGetPackageEventInfo(pRequest, traveler->pProduct, &pPackageInfo);
				HxSTD_memcpy(&traveler->PackageInfo, pPackageInfo, sizeof(PACKAGE_INFO));

				HxLOG_Print("[%d]  2 .... END <============  \n", index);
				//move to next node
				traveler = holder;
				index++;
			}//clear each package list one by one.

			CAPPV_PACKAGE_SEM_RELEASE();

		}

		HxLOG_Print("(xStatus = %d), END, Making package INFO!!\n", xStatus);
	}

	HxLOG_Print("set bIsFullInfoPackageRequested to FALSE<------------!!\n");
	local_caPpv_SetFullInfoPackageListRequestStatus(FALSE);

	if (s_pfnPackageListFullInfoNofityCallback)
	{
		HxLOG_Print("send callback to s_pfnPackageListFullInfoNofityCallback ^^ \n");
		s_pfnPackageListFullInfoNofityCallback(xStatus, s_stPackageList.pRequest,  s_stPackageList.xNbItems, (void *) s_stPackageList.pList);
	}
	else
	{
		HxLOG_Print("No s_pfnPackageListFullInfoNofityCallback to return!! \n");
	}

	HxLOG_Print("<======= EXIT!!\n");
}

HERROR local_caPpv_RequestFullInfoPackageList(TPpid providerID)
{
	TEventStatus result = ERR_OK;
	//MHP에서 CAK_UIT_REQUEST_FULL_INFO_PACKAGE_LIST를 uitask에 전달.
	//Package list에 package info까지 같이 전달

	HxLOG_Print("providerID:0x%x ------------> ENTER!!\n", providerID);
	local_caPpv_SetFullInfoPackageListRequestStatus(TRUE);

	result = local_caPpv_RequestPackageList(providerID);
	HxLOG_Print("hErr(%d)<------------!!\n", result);

	if (result != CA_EVENT_SUCCESS)
	{
		HxLOG_Print("set bIsFullInfoPackageRequested to FALSE (%d)<------------!!\n");
		local_caPpv_SetFullInfoPackageListRequestStatus(FALSE);

		if (s_pfnFullInfoPackageListNotifyCallback)
		{
			s_pfnFullInfoPackageListNotifyCallback(result, NULL, 0, NULL);
		}
	}
	else
	{
		HxLOG_Print("wait for callback, sUitPackageListNotifyCallback()<------------!! (%d) \n");
	}

	return result;
}

static TUnsignedInt32 local_caPpv_PackageListNotifyCallback(TUnsignedInt32 xStatus, TCaRequest* pRequest, TUnsignedInt32 xListNum, PPV_PACKAGE_LIST_ITEM* pList)
{
	//CAK에서 CAK_UIT_REQUEST_PACKAGE_LIST를 uitask에 전달 하면 불린다.
	//다시 UIT로 전달 하고 끊어 주어야 한다.
	//xListNum, pList는 어차피 package_list를 사용 하기 때문에 전달할 필요가 없다. ^^
	TBoolean requestStatus = FALSE;

	local_caPpv_GetFullInfoPackageListRequestStatus(&requestStatus);

	HxLOG_Print("requestStatus = %s, ----------> ENTER!! (%d) \n", (requestStatus==TRUE)?"TRUE":"FALSE");

	if (requestStatus == TRUE)
	{
		NaCasMsg_t 	 	stMsg;
		TSignedInt32	nErrCode;

		HxLOG_Print("send CAK_UIT_MAKE_FULL_INFO_PACKAGE_LIST !!\n");

		stMsg.wMsg = NA_CAK_ID;
		stMsg.lParam2 = CAK_PPV_MAKE_FULL_INFO_PACKAGE_LIST;
		stMsg.lParam1 = pRequest;//package list holder
		stMsg.hWnd= xStatus;

		nErrCode = VK_PostMessage(s_ulPpvQueueId, (void *)&stMsg, QUEUE_SIZE_1);
		if (nErrCode != ERR_OK)
		{
			HxLOG_Print("Q Send Fail in sUitPackageListNotifyCallback 0x%x\n", nErrCode);

			if (s_pfnPackageListFullInfoNofityCallback)
			{
				s_pfnPackageListFullInfoNofityCallback(CA_EVENT_ERROR, NULL,  0, NULL);
			}

			return ERR_FAIL;
		}

		HxLOG_Print("<----- send OK!!\n");

	}
	else
	{
		HxLOG_Print("<--- bIsFullInfoPackageRequested = FALSE, can't be here!! humm  !!\n");
	}

	HxLOG_Print("<----- EXIT!! return CA_EVENT_SUCCESS !!\n");

	return CA_EVENT_SUCCESS;

}


static void local_caPpv_CakPpv_Task(void* p)
{
	NaCasMsg_t 	stInMsg;
	HERROR		hErr = ERR_OK;

	while(1)
	{
		if (VK_GetMessage(s_ulPpvQueueId, (void *)&stInMsg, QUEUE_SIZE_1) != ERR_OK)
		{
			continue;
		}

		switch (stInMsg.wMsg)
		{
			case NA_CAK_ID:
				switch (stInMsg.lParam2)
				{
					case CAK_CREDIT_NOTIFICATION:
						{
							TSystemInfoStatus 	eSmartcardStatus;

							CA_NA_CakGetSystemInfo(&eSmartcardStatus, NULL, NULL, NULL);	// status만 확인한다.
							if (eSmartcardStatus == CA_SYSTEM_INFO_SUCCESS)
							{
								hErr = local_caPpv_RequestAsyncGetCredit();
							}
							HxLOG_Info("CAK_CREDIT_NOTIFICATION...hErr = 0x%x, eSmartcardStatus = %d\n", hErr, eSmartcardStatus);
						}
						break;

					case CAK_CREDIT_CALL_BACK:
						{
							TUnsignedInt8 nResult;
							TUnsignedInt32 nCredit;

							nCredit = (TUnsignedInt32)stInMsg.lParam1;
							nResult = (TUnsignedInt8)stInMsg.lParam3;

							if (nResult == CA_EVENT_SUCCESS)
							{
								s_ulCardRemainedCredit = nCredit;
							}
							else
							{
								s_ulCardRemainedCredit = 0;
							}

							HxLOG_Print("CAK_CREDIT_CALL_BACK...nResult = 0x%x, nCredit = %ld\n", nResult, nCredit);
							local_caPpv_SendCakMsg(CA_PPV_CALLBACK_CREDIT, nResult, NULL, nCredit);
						}
						break;
					case CAK_PPV_LOADING_LISTENER:
						local_caPpv_SendCaListenerArrivedNotify(CA_LISTENER_TYPE_PRODUCTS_LOADING);
						break;

					case CAK_PPV_LOADED_LISTENER:
						// bIsProductsLoaded 를 true로 setting 함으로써, 이제 package list를 request 할 수 있다.
						local_caPpv_SetProductsLoadable(TRUE);
						break;

					case CAK_PPV_NIT_TIMER_CALL_BACK:
						{
							NIT_WAIT_ITEM 	stNitItem;
							TUnsignedInt32	nTimerId;

							nTimerId = stInMsg.hWnd;

							stNitItem.eOwner = stInMsg.lParam1;
							stNitItem.flag = stInMsg.info;
							stNitItem.provider = stInMsg.lParam3;

							local_caPpv_CheckNIT(nTimerId, &stNitItem);
						}
						break;

					case CAK_PPV_EIT_TIMER_CALL_BACK:
						{
							local_caPpv_ExecuteEventInfo();
						}
						break;

					case CAK_REQUEST_PACKAGE_LIST:
						{
							//AP에서 해당 msg를 전달
							HxLOG_Print("CAK_REQUEST_PACKAGE_LIST... - ulProviderId(0x%x)\n", stInMsg.hWnd);
							local_caPpv_RequestPackageList(stInMsg.hWnd);
						}
						break;

					case CAK_PPV_PACKAGE_LIST_CALL_BACK:
						{
							HxLOG_Print("CAK_PPV_PACKAGE_LIST_CALL_BACK - pRequest(%ld)\n", stInMsg.hWnd);
							local_caPpv_ResponsePackageList((TCaRequest*)stInMsg.hWnd);
						}
						break;

					case CAK_PPV_PACKAGE_TIMER_CALL_BACK:
						{
							HxLOG_Print("CAK_PPV_PACKAGE_TIMER_CALL_BACK - timer(%ld)\t\r\n", stInMsg.info);
							local_caPpv_RequestPackageDataList((unsigned long) stInMsg.info, (void*)stInMsg.hWnd);
						}
						break;

					case CAK_REQUEST_PRESENT_EVENT_CALL_BACK:
						{
							HxLOG_Print("CAK_REQUEST_PRESENT_EVENT_CALL_BACK - pRequest(%ld)\t\r\n", stInMsg.hWnd);
							local_caPpv_ResponsePresentEventCallback((TCaRequest*)stInMsg.hWnd);
						}
						break;

					case CAK_REQUEST_FOLLOWING_EVENT_CALL_BACK:
						{
							HxLOG_Print("CAK_REQUEST_FOLLOWING_EVENT_CALL_BACK - pRequest(%ld)\t\r\n", stInMsg.hWnd);
							local_caPpv_ResponseFollowingEventCallback((TCaRequest*)stInMsg.hWnd);
						}
						break;

					case CAK_REQUEST_PURCHASE_HISTORY:
						{
							HxLOG_Print("CAK_REQUEST_PURCHASE_HISTORY - ulProviderId(0x%x)\t\r\n", stInMsg.hWnd);
							local_caPpv_RequestPurchaseHistory(stInMsg.hWnd);
						}
						break;

					case CAK_PURCHASE_HISTORY_CALL_BACK:
						{
							HxLOG_Print("CAK_PURCHASE_HISTORY_CALL_BACK - pRequest(%ld)\t\r\n", stInMsg.hWnd);
							local_caPpv_ResponsePurchaseHistoryCallBack((TCaRequest*)stInMsg.hWnd);
						}
						break;

					case CAK_REQUEST_RECHARGE_LIST:
						{
							HxLOG_Print("CAK_REQUEST_RECHARGE_LIST\n");
							local_caPpv_RequestRechargeList();
						}
						break;

					case CAK_RECHARGE_LIST_CALL_BACK:
						{
							HxLOG_Print("CAK_RECHARGE_LIST_CALL_BACK - pRequest(%ld)\t\r\n", stInMsg.hWnd);
							local_caPpv_ResponseRechargeListCallBack((TCaRequest*)stInMsg.hWnd);
						}
						break;

					case CAK_PPV_RESPONSE_NEW_RECHARGE:
						{
							HxLOG_Print("CAK_PPV_RESPONSE_NEW_RECHARGE\n");
							local_caPpv_ResponseNewRecharge();
						}
						break;

					case CAK_PPV_RESPONSE_NEW_OPPV:
						{
							HxLOG_Print("CAK_PPV_RESPONSE_NEW_OPPV\n");
							/* Nagra OPPV test에서 OPPV message가 잘못들어오는 문제로 인하여 message를 보내기 전에 바로 처리하도록 수정 */
							//local_caPpv_ResponseNewOppv((TCaRequest*)stInMsg.hWnd);
						}
						break;

					case CAK_IPPV_PURCHASE_RESPONSE:
						{
							HxLOG_Print("CAK_IPPV_PURCHASE_RESPONSE\n");

						}
						break;

					case CAK_PPV_REQUEST_FULL_INFO_PACKAGE_LIST:
						{
							//MHP에서 uitask로 전달..
							HxLOG_Print("CAK_UIT_REQUEST_FULL_INFO_PACKAGE_LIST providerId(0x%x)\n", stInMsg.lParam1);
							local_caPpv_RequestFullInfoPackageList((TPpid)stInMsg.lParam1);
						}
						break;//end of CAK_UIT_REQUEST_FULL_INFO_PACKAGE_LIST

					case CAK_PPV_MAKE_FULL_INFO_PACKAGE_LIST:
						{
							HxLOG_Print("CAK_PPV_MAKE_FULL_INFO_PACKAGE_LIST - stInMsg.lParam1 = %d\n", stInMsg.lParam1);
							local_caPpv_MakeFullInfoPackageList((TCaRequest*)stInMsg.lParam1, (TUnsignedInt32)stInMsg.hWnd);	//package list holder
						}
						break;

					default:
						break;
				}
				break;

			case NA_APP_ID:
				switch((TSignedInt32)stInMsg.lParam2)
				{
					case CAK_TS_CHECK_REQ_ID:
						local_caPpv_ExecuteTsCheck((Handle_t)stInMsg.lParam1, eNIT_WAIT_INIT);
						break;

					case CAK_EVT_INFO_REQ_ID:
						local_caPpv_ExecuteEventInfo();
						break;

					default:
						break;
				}
				break;

			case NA_FILTER_ID:
				switch ((TSignedInt32)stInMsg.lParam2)
				{
					case CAK_FILTER_NIT_ARRIVAL:
						{
							PDT_DATA 		*pCPDTData = NULL, *pHPDTData = NULL;
							PDT_FILTER_MODE mode = PDT_FILTER_NONE;
							TUnsignedInt16	onID = 0;

							//우선 current nit에 cpdt, hpdt데이터가 들어온 경우이므로 global에 저장한다..
							//왜냐하면 지금 현재 pdt request가 성공한건지 알 수가 없으므로
							//지금 상황에서 caRequestSetPid를 할 수 없다
							pCPDTData = (PDT_DATA*) stInMsg.lParam1;
							pHPDTData = (PDT_DATA*) stInMsg.hWnd;
							onID = (TUnsignedInt16) stInMsg.lParam3;

							HxLOG_Print("CAK_FILTER_NIT_ARRIVAL - onID(0x%x)\n", onID);

							//TP CHANGED가 MW task->UIT task로 들어와 서 NIT arrived를 지워 버린다.
							//기존 NIT arrived는 NAPI_NotifyPDTinNIT()에서 setting이 되었다. 즉, 정확히는 MW task에서 setting이 된 후,
							//PDT 확인은 UIT task에서 진행이 되었던 것이다. 즉, NIT flag가 set/unset이 MW task/UIT task로 분산 되어있어
							//각 task의 priority에 따라 다르게 동작 하였다. 그러므로 set과 reset이 모두 UIT task에서 진행 되게 하여야 한다.
							//NAPI_NotifyPDTinNIT()에 기존에 있던 것은 synch가 안 맞으므로 (Nagra TP flag setting) 지운다.
							//실제 UIT task에서 다시 한번 setting을 해 준다.
							local_caPpv_SetNITArrivedFlag(TRUE, onID);

							//현재 play되고 있는 stream이 Mediaset 것 인가?
							//	case 1. 현재 (NA PPV) + NEW (NA PPV) => do nothing
							//	case 2. 현재 (NA PPV) + NEW (일반) => stop PDT
							//	case 3. 현재 (일반) + NEW (NA PPV) => start PDT
							//	case 4. 현재 (일반) + NEW (일반) ==> do nothing
							if (!pCPDTData && !pHPDTData)
							{
								// case 2. 현재 (Mediaset Premium) + NEW (일반) => stop PDT
								HxLOG_Print("[current NA PPV -> normal ==> stopFilter !!\n");
								local_caPpv_RequestPDTStop(TRUE); //STOP에서 INT_UIT_setNagraTS(FALSE)가 불린다.
								break;
							}

							// case 3. 현재 (일반) + NEW ( NA PPV ) => start PDT
							if (pCPDTData)
							{
								HxSTD_memcpy(&s_CPDT_DATA, pCPDTData, sizeof(PDT_DATA));
								OxCAS_Free(pCPDTData);
								pCPDTData = NULL;
								mode |= PDT_FILTER_CPDT;
							}

							if (pHPDTData)
							{
								HxSTD_memcpy(&s_HPDT_DATA, pHPDTData, sizeof(PDT_DATA));
								OxCAS_Free(pHPDTData);
								pHPDTData = NULL;
								mode |= PDT_FILTER_HPDT;
							}

							// pdt start request함
							HxLOG_Print("current normal -> NA PPV ==> start Filter !!\n");
							local_caPpv_RequestPDTStart(mode);
						}
						break;

					case CAK_FILTER_EIT_PF_ARRIVAL:
						{
							TBoolean 		forceToUpdate = FALSE;

							forceToUpdate = (TBoolean)stInMsg.lParam3;
							HxLOG_Print("forceToUpdate = %d\n", forceToUpdate);

							if (forceToUpdate)
							{
								//local_caPpv_NotifyEventChanged(stInMsg.lParam1, s_stPresentEvtPurchase.EvtId);
								//CA_NA_PpvRequestEventInfo();
								local_caPpv_ExecuteTsCheck(stInMsg.lParam1, 0);
							}
						}
						break;

					case CAK_FILTER_TDT_ARRIVAL:
						{
							{
//								CA_NA_PpvRequestEventInfo(); // 상황을 보며 넣을 예정
							}
						}
						break;

					default:
						break;
				}
				break;

			default:
				break;
		}
	}
}


HERROR CA_NA_PpvRegisterScCreditCallback(TCardCreditCallback callback)
{
	TUnsignedInt16 index = 0;

	if(callback == NULL)
	{
		return ERR_FAIL;
	}

	for (index = 0; index < NA_MAX_CALLBACK_NUM; index ++)
	{
		if (s_pfnCardCreditCallBack[index] == NULL)
		{
			s_pfnCardCreditCallBack[index] = callback;
			break;
		}
	}

	if (index >= NA_MAX_CALLBACK_NUM)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}


HERROR CA_NA_PpvRegisterProductsLoadingCallback(TCaListenerArrivedNotifyCallback callback)
{
	if (callback == NULL)
	{
		return ERR_FAIL;
	}

	s_pfnCaListenerArrivedNotifyCallback = callback;

	return ERR_OK;
}


//ap로 주어진 ProvideID에 해당 하는 TS인지 확인 하여  notify를 올려주기 위해 register하는 함수
HERROR CA_NA_PpvRegisterTSCheckNotifyCallback(TTSCheckNotifyCallback callback)
{
	if(callback == NULL)
	{
		return ERR_FAIL;
	}

	s_pfnTSCheckNotifyCallback = callback;

	return ERR_OK;
}

HERROR CA_NA_PpvRegisterEventInfoNotifyCallback(TRequestEventNotifyCallback callback)
{
	TUnsignedInt16 index = 0;

	if(callback == NULL)
	{
		return ERR_FAIL;
	}

	for (index = 0; index < NA_MAX_CALLBACK_NUM; index ++)
	{
		if (s_pfnEventInfoNofityCallback[index] == NULL)
		{
			s_pfnEventInfoNofityCallback[index] = callback;
			break;
		}
	}

	if (index >= NA_MAX_CALLBACK_NUM)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR CA_NA_PpvRegisterPackageListNotifyCallback(TPackageInfoNotifyCallback callback)
{
	TUnsignedInt16 index = 0;

	if(callback == NULL)
	{
		return ERR_FAIL;
	}

	for (index = 0; index < NA_MAX_CALLBACK_NUM; index ++)
	{
		if (s_pfnPackageListNofityCallback[index] == NULL)
		{
			s_pfnPackageListNofityCallback[index] = callback;
			break;
		}
	}

	if (index >= NA_MAX_CALLBACK_NUM)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR CA_NA_PpvRegisterFullInfoPackageListNotifyCallback(TPackageInfoNotifyCallback callback)
{
	HERROR 			hErr = ERR_OK;

	if(callback == NULL)
	{
		return ERR_FAIL;
	}

	if (s_pfnPackageListFullInfoNofityCallback == NULL)
	{
		s_pfnPackageListFullInfoNofityCallback = callback;

		hErr = CA_NA_PpvRegisterPackageListNotifyCallback(local_caPpv_PackageListNotifyCallback);
		if (hErr == ERR_OK)
		{
			HxLOG_Print("sUitPackageListNotifyCallback added !!!\n");
			return ERR_OK;
		}
		else
		{
			HxLOG_Error("sUitPackageListNotifyCallback not added !!!\n");
		}
	}
	else
	{
		HxLOG_Error("s_pfnPackageListFullInfoNofityCallback not added !!!\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}


HERROR CA_NA_PpvRegisterPurchaseHistoryNotifyCallback(TPurchaseHistoryNotifyCallback callback)
{
	TUnsignedInt16 index = 0;

	if(callback == NULL)
	{
		return ERR_FAIL;
	}

	for (index = 0; index < NA_MAX_CALLBACK_NUM; index ++)
	{
		if (s_pfnPurchaseHistoryNotifyCallback[index] == NULL)
		{
			s_pfnPurchaseHistoryNotifyCallback[index] = callback;
			break;
		}
	}

	if (index >= NA_MAX_CALLBACK_NUM)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}


HERROR CA_NA_PpvRegisterRechargeListNotifyCallback(TRechargeListNotifyCallback callback)
{
	TUnsignedInt16 index = 0;

	if(callback == NULL)
	{
		return ERR_FAIL;
	}

	for (index = 0; index < NA_MAX_CALLBACK_NUM; index ++)
	{
		if (s_pfnRechargeListNotifyCallback[index] == NULL)
		{
			s_pfnRechargeListNotifyCallback[index] = callback;
			break;
		}
	}

	if (index >= NA_MAX_CALLBACK_NUM)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}


HERROR CA_NA_PpvRegisterOppvPurchaseListNotifyCallback(TOPPVPurchaseListNotifyCallback callback)
{
	TUnsignedInt16 index = 0;

	if(callback == NULL)
	{
		return ERR_FAIL;
	}

	for (index = 0; index < NA_MAX_CALLBACK_NUM; index ++)
	{
		if (s_pfnOPPVPurchaseListNotifyCallback[index] == NULL)
		{
			s_pfnOPPVPurchaseListNotifyCallback[index] = callback;
			break;
		}
	}

	if (index >= NA_MAX_CALLBACK_NUM)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}


HERROR CA_NA_PpvRegisterRechargeAlarmNotifyCallback(TRechargeNotifyCallback callback)
{
	TUnsignedInt16 index = 0;

	if(callback == NULL)
	{
		return ERR_FAIL;
	}

	for (index = 0; index < NA_MAX_CALLBACK_NUM; index ++)
	{
		if (s_pfnRechargeAlarmNotifyCallback[index] == NULL)
		{
			s_pfnRechargeAlarmNotifyCallback[index] = callback;
			break;
		}
	}

	if (index >= NA_MAX_CALLBACK_NUM)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

//ap로 request purchase event notify를 올려주기 위해 register하는 함수
HERROR CA_NA_PpvRegisterRequestPurchaseEventNotifyCallback(TPurchaseEventNotifyCallback callback)
{
	if(callback == NULL)
	{
		return ERR_FAIL;
	}

	/* CA_EVENT_PPT의 notify 일 경우 , AP에서 notify의 input으로 들어 가는
	 * CA_PROCESSING_LOW_CREDIT
	 * CA_PROCESSING_NO_CREDIT
	 * CA_PROCESSING_CREDIT_SUSPENDED
	 * CA_PROCESSING_CREDIT_EXPIRED
	 * 에 관하여서는 올 수 없고, purchase time에 credit이 decrease 하지 않는 다는 것에 유의 ... -cak implementation guide 2.2.2 p20 note-
	 */
	s_pfnPurchaseEventNotifyCallback = callback;
	return ERR_OK;
}

HERROR CA_NA_PpvRegisterRequestPurchasePackageNotifyCallback(TPurchasePackageNotifyCallback callback)
{
	if(callback == NULL)
	{
		return ERR_FAIL;
	}

	s_pfnPurchasePackageNotifyCallback = callback;
	return ERR_OK;
}

HERROR CA_NA_PpvStartup(void)
{
	TSignedInt32 nErrCode;

	nErrCode = VK_SEM_Create(&s_ulPackageListSemId, "s_ulPackageListSemId", VK_SUSPENDTYPE_FIFO);
	if(nErrCode != ERR_OK)
	{
		HxLOG_Info("error in VK_SEM_Create - s_ulPackageListSemId, nErrCode = %ld\n", nErrCode);
		return ERR_FAIL;
	}

	nErrCode = VK_SEM_Create(&s_ulRechargeListSemId, "s_ulRechargeListSemId", VK_SUSPENDTYPE_FIFO);
	if(nErrCode != ERR_OK)
	{
		HxLOG_Info("error in VK_SEM_Create - s_ulRechargeListSemId, nErrCode = %ld\n", nErrCode);
		return ERR_FAIL;
	}

	nErrCode = VK_SEM_Create(&s_ulPurchaseListSemId, "s_ulPurchaseListSemId", VK_SUSPENDTYPE_FIFO);
	if(nErrCode != ERR_OK)
	{
		HxLOG_Info("error in VK_SEM_Create - s_ulPurchaseListSemId, nErrCode = %ld\n", nErrCode);
		return ERR_FAIL;
	}

	nErrCode = VK_SEM_Create(&s_ulSmartcardSemId, "s_ulSmartcardSemId", VK_SUSPENDTYPE_FIFO);
	if(nErrCode != ERR_OK)
	{
		HxLOG_Info("error in VK_SEM_Create - s_ulSmartcardSemId, nErrCode = %ld\n", nErrCode);
		return ERR_FAIL;
	}

	nErrCode = VK_SEM_Create(&s_ulEitSemId, "s_ulEitSemId", VK_SUSPENDTYPE_FIFO);
	if(nErrCode != ERR_OK)
	{
		HxLOG_Info("error in VK_SEM_Create - s_ulEitSemId, nErrCode = %ld\n", nErrCode);
		return ERR_FAIL;
	}

	nErrCode = VK_SEM_Create(&s_ulNitSemId, "s_ulNitSemId", VK_SUSPENDTYPE_FIFO);
	if(nErrCode != ERR_OK)
	{
		HxLOG_Info("error in VK_SEM_Create - s_ulNitSemId, nErrCode = %ld\n", nErrCode);
		return ERR_FAIL;
	}

	nErrCode = VK_CreateMessageQueue(256, &s_ulPpvQueueId, NA_PPV_MSG_Q_NAME, QUEUE_SIZE_1);
	if (nErrCode != ERR_OK)
	{
		HxLOG_Info("error in VK_CreateMessageQueue %ld\n", nErrCode);
		return ERR_FAIL;
	}

	nErrCode = VK_TASK_Create(local_caPpv_CakPpv_Task, NA_CAK_TASK_PRIORITY, NA_CAK_TASK_STACK_SIZE, NA_PPV_TASK_NAME, NULL, &s_ulPpvTaskId, 0);
	if (nErrCode != ERR_OK)
	{
		HxLOG_Info("error in creating PPV thread %ld\n", nErrCode);
		return ERR_FAIL;
	}

	nErrCode = VK_TASK_Start(s_ulPpvTaskId);
	if (nErrCode != ERR_OK)
	{
		HxLOG_Info("error in starting PPV thread %ld\n", nErrCode);
		return ERR_FAIL;
	}

	return ERR_OK;
}


HERROR CA_NA_PpvGetScCredit(TCreditAmount  *pCredit)
{
	*pCredit = s_ulCardRemainedCredit;

	return ERR_OK;
	//return CA_NA_PpvRequestCardCredit();	// 일단 저장된 값을 전달하고 새로 요청하자
}

void CA_NA_PpvSetScCredit(TCreditAmount  pCredit)
{
	s_ulCardRemainedCredit = pCredit;
}

TCaStatus CA_NA_PpvRequestCardCredit(void)
{
	 NaCasMsg_t 	 stMsg;
	 TSignedInt32	 nErrCode;

	 HxLOG_Print("\n");
	 stMsg.wMsg = NA_CAK_ID;
	 stMsg.lParam2 = CAK_CREDIT_NOTIFICATION;

	 nErrCode = VK_PostMessage(s_ulPpvQueueId, (void *)&stMsg, QUEUE_SIZE_1);
	 if (nErrCode != ERR_OK)
	 {
		 HxLOG_Info("error in VK_PostMessage (%ld)\n", nErrCode);
		 return CA_ERROR;
	 }
	 return CA_NO_ERROR;
}

TCaStatus CA_NA_PpvProductLoadingListener(void)
{
	NaCasMsg_t 		stMsg;
	TSignedInt32 	nErrCode;

	 s_bIsProductsLoaded = FALSE;

	 HxLOG_Print("\n");
	stMsg.wMsg 		= NA_CAK_ID;
	 stMsg.lParam2 = CAK_PPV_LOADING_LISTENER;

	nErrCode = VK_PostMessage(s_ulPpvQueueId, (void *)&stMsg, QUEUE_SIZE_1);
	 if (nErrCode != ERR_OK)
	{
		 HxLOG_Info("error in VK_PostMessage (%ld)\n", nErrCode);
		 return CA_ERROR;
	}
	 return CA_NO_ERROR;
}

TCaStatus CA_NA_PpvProductLoadedListener(void)
{
	NaCasMsg_t 		stMsg;
	TSignedInt32 	nErrCode;

	s_bIsProductsLoaded = TRUE;

	 HxLOG_Print("\n");
	 stMsg.wMsg = NA_CAK_ID;
	 stMsg.lParam2 = CAK_PPV_LOADED_LISTENER;

	 nErrCode = VK_PostMessage(s_ulPpvQueueId, (void *)&stMsg, QUEUE_SIZE_1);
	 if (nErrCode != ERR_OK)
	 {
		 HxLOG_Info("error in VK_PostMessage (%ld)\n", nErrCode);
		 return CA_ERROR;
	 }
	 return CA_NO_ERROR;
}

TCaStatus CA_NA_PpvRequestTsCheck(Handle_t hSvcHandle)
{
	NaCasMsg_t 	 stMsg;
	TSignedInt32	 nErrCode;

	HxLOG_Print("Enter\n");

	if (hSvcHandle == HANDLE_NULL)
	{
		return CA_ERROR;
	}
	else if (hSvcHandle == 0)	// service handle을 다시 얻어오자.
	{
		if(s_hCurSvc == HANDLE_NULL)
		{
			return CA_ERROR;
		}

		hSvcHandle = s_hCurSvc;
	}

	stMsg.wMsg 		= NA_APP_ID;
	stMsg.lParam1 	= (TSignedInt32)hSvcHandle;
	stMsg.lParam2	= CAK_TS_CHECK_REQ_ID;
	stMsg.lParam3	= 0;

	nErrCode = VK_PostMessage(s_ulPpvQueueId, (void *)&stMsg, QUEUE_SIZE_1);
	if (nErrCode != ERR_OK)
	{
		HxLOG_Info("error in VK_PostMessage (%ld)\n", nErrCode);
		return CA_ERROR;
	}
	return CA_NO_ERROR;
}


HERROR CA_NA_PpvRequestEventInfo(void)
{
	NaCasMsg_t 		stMsg;
	TSignedInt32 	nErrCode;

	HxLOG_Print("Enter\n");

	stMsg.wMsg 		= NA_APP_ID;
	stMsg.lParam1 	= 0;
	stMsg.lParam2 	= CAK_EVT_INFO_REQ_ID;
	stMsg.lParam3 	= 0;

	nErrCode = VK_PostMessage(s_ulPpvQueueId, (void *)&stMsg, QUEUE_SIZE_1);
	if (nErrCode)
	{
		HxLOG_Info("error in VK_PostMessage (%ld)\n", nErrCode);
		return ERR_FAIL;
	}
	return ERR_OK;
}


HERROR CA_NA_PpvRequestRechargeList(void)
{
	NaCasMsg_t 		stMsg;
	TSignedInt32 	nErrCode;

	stMsg.wMsg 		= NA_CAK_ID;
	stMsg.lParam1 	= 0;
	stMsg.lParam2 	= CAK_REQUEST_RECHARGE_LIST;
	stMsg.lParam3 	= 0;

	nErrCode = VK_PostMessage(s_ulPpvQueueId, (void *)&stMsg, QUEUE_SIZE_1);
	if (nErrCode)
	{
		HxLOG_Info("error in VK_PostMessage (%ld)\n", nErrCode);
		return ERR_FAIL;
	}
	return ERR_OK;
}


HERROR CA_NA_PpvRequestPurchaseHistory(TUnsignedInt32 ulProviderId)
{
	NaCasMsg_t 		stMsg;
	TSignedInt32 	nErrCode;

	stMsg.wMsg 		= NA_CAK_ID;
	stMsg.hWnd 		= ulProviderId;
	stMsg.lParam2 	= CAK_REQUEST_PURCHASE_HISTORY;
	stMsg.lParam3 	= 0;

	nErrCode = VK_PostMessage(s_ulPpvQueueId, (void *)&stMsg, QUEUE_SIZE_1);
	if (nErrCode)
	{
		HxLOG_Info("error in VK_PostMessage (%ld)\n", nErrCode);
		return ERR_FAIL;
	}
	return ERR_OK;
}


void CA_NA_CakReceivedNit(TUnsignedInt32 hAction, TUnsignedInt16 usUniqueId, TUnsignedInt8 *pucNit)
{
	HERROR hErr;

	hErr = local_caPpv_NotifyPDTinNIT(hAction, usUniqueId, pucNit);
	HxLOG_Print("hErr = %d\n", hErr);
}

void CA_NA_CakReceivedEit(TUnsignedInt32 hAction, TUnsignedInt16 usUniqueId, TUnsignedInt8 *pucEitP, TUnsignedInt8 *pucEitF)
{
	HERROR hErr;

	hErr = local_caPpv_NotifyEit(hAction, usUniqueId, pucEitP, pucEitF);
	HxLOG_Print("hErr = %d\n", hErr);
}

void CA_NA_CakReceivedTDT(void)
{
	HERROR hErr;

	hErr = local_caPpv_NotifyTdt();
	/*
	HxLOG_Print("hErr = %d\n", hErr);
	*/
}

TCaRequestStatus  CA_NA_PpvCreateRequest(TCaRequestType type, TCaRequest** pRequest)
{
	TCaRequestStatus reqStatus;

	HxLOG_Print("ENTER - type(0x%x)\n" ,type);

	reqStatus = caRequestCreate(type, pRequest);

	HxLOG_Print("EXIT - pRequest(%d)\n", *pRequest);

	return reqStatus;
}


HERROR CA_NA_PpvDisposeRequest(TCaRequest* pRequest)
{
	if (pRequest)
	{
		HxLOG_Print("dispose request, pRequst = %d\n", pRequest);

		if (pRequest)
		{
			caRequestDispose(pRequest);
		}
		pRequest = NULL;
	}
	else
	{
		HxLOG_Print("dispose request, pRequst == NULL\n");
	}

	return ERR_OK;
}

HERROR CA_NA_PpvNotifyTPChanged(TBoolean	bTpChanged)
{
	local_caPpv_ClearPurchaseInfo(&s_stPresentEvtPurchase);
	local_caPpv_ClearPurchaseInfo(&s_stFollowingEvtPurchase);		// tp 전환이 아니라 채널 전환인 경우에도 eit p/f clear

	if (bTpChanged == TRUE)
	{
		local_caPpv_NotifyTPChanged();
	}

	return ERR_OK;
}

HERROR CA_NA_PpvNotifySvcChanged(Handle_t hSvc, Handle_t hAction)
{
	HxLOG_Print("s_hCurSvc changed 0x%x->0x%x, s_hCurAction changed 0x%x->0x%x\n", s_hCurSvc, hSvc, s_hCurAction, hAction);
	s_hCurSvc = hSvc;
	s_hCurAction = hAction;

	return ERR_OK;
}

HERROR CA_NA_PpvRequestNewRecharge(void)
{
	NaCasMsg_t 		stMsg;
	TSignedInt32 	nErrCode;

	stMsg.wMsg 		= NA_CAK_ID;
	stMsg.lParam1 	= 0;
	stMsg.lParam2 	= CAK_PPV_RESPONSE_NEW_RECHARGE;
	stMsg.lParam3 	= 0;

	nErrCode = VK_PostMessage(s_ulPpvQueueId, (void *)&stMsg, QUEUE_SIZE_1);
	if (nErrCode)
	{
		HxLOG_Info("error in VK_PostMessage (%ld)\n", nErrCode);
		return ERR_FAIL;
	}
	return ERR_OK;
}

HERROR CA_NA_PpvRequestNewPurchase(TCaExportation* 	pxExportation)
{
	#if 1
	if (pxExportation == NULL)
	{
		HxLOG_Print("related exportation is NULL!! do nothing~~\n");
		return ERR_FAIL;
	}
	local_caPpv_ResponseNewOppv(pxExportation);
	#else
	NaCasMsg_t 		stMsg;
	TSignedInt32 	nErrCode;

	stMsg.wMsg 		= NA_CAK_ID;
	stMsg.lParam1 	= 0;
	stMsg.lParam2 	= CAK_PPV_RESPONSE_NEW_OPPV;
	stMsg.hWnd 		= (TUnsignedInt32)pxExportation;

	nErrCode = VK_PostMessage(s_ulPpvQueueId, (void *)&stMsg, QUEUE_SIZE_1);
	if (nErrCode)
	{
		HxLOG_Info("error in VK_PostMessage (%ld)\n", nErrCode);
		return ERR_FAIL;
	}
	#endif
	return ERR_OK;
}

HERROR CA_NA_PpvRequestPackageList(TUnsignedInt32 ulProviderId)
{
	NaCasMsg_t 		stMsg;
	TSignedInt32 	nErrCode;

	stMsg.wMsg 		= NA_CAK_ID;
	stMsg.hWnd 		= ulProviderId;
	stMsg.lParam2 	= CAK_REQUEST_PACKAGE_LIST;
	stMsg.lParam3 	= 0;

	nErrCode = VK_PostMessage(s_ulPpvQueueId, (void *)&stMsg, QUEUE_SIZE_1);
	if (nErrCode)
	{
		HxLOG_Info("error in VK_PostMessage (%ld)\n", nErrCode);
		return ERR_FAIL;
	}
	return ERR_OK;
}

HERROR CA_NA_PpvRequestEventPurchase(TPurchasableEventType xStatus, TCaRequest* pRequest, TCaEvent *pEvent, TCaProduct *pProduct)
{
	HERROR 	hErr;

	HxLOG_Print("ENTER (xStatus=%d)\n", xStatus);

	if ((pRequest == NULL) || (pEvent == NULL) || (pProduct == NULL) )
	{
		HxLOG_Print("pRequest == NULL!!! END==>\n");
		return ERR_FAIL;
	}

	switch (xStatus)
	{
		case CA_EVENT_SPECIAL_EVENT: //process special event 에서 purchase period in이다.  purchase request 를 불러 줄 수 있다.
			HxLOG_Print("xStatus = CA_EVENT_SPECIAL_EVENT~~,(pProduct = 0x%x, pRequest = 0x%x)\n", pProduct, pRequest);
			hErr = local_caPpv_RequestPurchaseSpecialEvent(pProduct);
			HxLOG_Print("<~~ IPPV END, hErr(%d)\n", hErr);
			break;

		case CA_EVENT_PPT: //process PPT 에서 purchase period in이다.  purchase request 를 불러 줄 수 있다.
			HxLOG_Print("xStatus = CA_EVENT_PPT~~,(pProduct = 0x%x, pRequest = 0x%x)\n", pProduct, pRequest);
			hErr = local_caPpv_RequestPurchasePPT(  pEvent, pProduct);
			HxLOG_Print("<~~ PPT END, hErr(%d)\n", hErr);
			break;

		default:
			HxLOG_Print("UNKNOWN Purchase xStatus = %d\n", xStatus);
			break;
	}

	//여기서, purchae 를 끝낸  후, dispose시킨다
	CA_NA_PpvDisposeRequest(pRequest);

	return hErr;
}


HERROR CA_NA_PpvRequestPackagePurchase(TCaProduct* pProduct)
{
	TCaRequestStatus		reqStatus;
	TCaProcessingStatus 	procStatus = CA_PROCESSING_NO_ERROR;
	TCaRequest* 			pRequest = NULL;
	PURCHASE_RESULT_TYPE	ulPurchaseResult = 0;

	if (pProduct == NULL)
	{
		HxLOG_Print("Error - pProduct == NULL\n");
		return ERR_FAIL;
	}

	local_caPpv_WaitForGivenTimeWithGivenInterval(MAX_PDT_LOADED_WAIT_TIME_INTERVAL, MAX_PDT_LOADED_WAIT_TIME_FOR_PRODUCT);


	//create the product purchase request
	reqStatus = CA_NA_PpvCreateRequest(CA_REQUEST_TYPE_PRODUCT_PURCHASE, &pRequest);
	if (reqStatus == CA_REQUEST_NO_ERROR)
	{
		reqStatus = caRequestSetProduct(pRequest, pProduct);
		if (reqStatus == CA_REQUEST_NO_ERROR)
		{
			reqStatus = caRequestSend(pRequest);
			if (reqStatus == CA_REQUEST_NO_ERROR)
			{
				reqStatus = caRequestGetProcessingStatus(pRequest, &procStatus);
				if ((reqStatus == CA_REQUEST_NO_ERROR) && (procStatus == CA_PROCESSING_NO_ERROR) )
				{
					//결과는 그냥 AP로 전달 하면 된다.	여기서는 할 것이 없다... 끝...
					HxLOG_Print("caRequestGetProcessingStatus result !!, reqStatus(%d), procStatus(%d)\n", reqStatus, procStatus);
				}
				else
				{
					HxLOG_Print("caRequestGetProcessingStatus ERR!!, reqStatus(%d), procStatus(%d)\n", reqStatus, procStatus);
				}
			}
			else
			{
				HxLOG_Print("caRequestSend ERROR (%d) \n", reqStatus);
			}
		}
		else
		{
			HxLOG_Print("aRequestSetProduct ERROR (%d) \n", reqStatus);
		}
	}
	else
	{
		HxLOG_Print("CA_NA_PpvCreateRequest(CA_REQUEST_TYPE_PRODUCT_PURCHASE) ERROR (%d) \n", reqStatus);
	}

	//(reqStatus == CA_REQUEST_NO_ERROR) && (procStatus == CA_PROCESSING_ERROR) 일 경우만 purchase SUCCESS 이다.
	if (reqStatus != CA_REQUEST_NO_ERROR)
	{
		procStatus = CA_PROCESSING_ERROR;
	}

	//dispose pRequest for purchase.....
	CA_NA_PpvDisposeRequest(pRequest);

	//Now, dispose grobal Product List...여기서 안 부르고 AP에서 package menu를 빠져 나올 때 불러 주기로 한다.  {{
	//MW_CA_NA_DisposePackageList(package_list.pRequest); }}

result:

	switch (procStatus)
	{
		case CA_PROCESSING_NO_ERROR:
			ulPurchaseResult = eNA_PPV_PURCHASE_RESULT_TYPE_SUCCESS;
			break;
		case CA_EVENT_PURCHASE_MEMORY_FULL:
			ulPurchaseResult = eNA_PPV_PURCHASE_RESULT_TYPE_MEMORY_FULL;
			break;
		case CA_EVENT_PURCHASE_LOW_CREDIT:
		case CA_EVENT_PURCHASE_NO_CREDIT:
			ulPurchaseResult = eNA_PPV_PURCHASE_RESULT_TYPE_CREDIT_SHORTAGE;
			break;
		case CA_EVENT_PURCHASE_CREDIT_SUSPENDED:
			ulPurchaseResult = eNA_PPV_PURCHASE_RESULT_TYPE_SUSPENDED_CARD;
			break;
		case CA_EVENT_PURCHASE_CREDIT_EXPIRED:
			ulPurchaseResult = eNA_PPV_PURCHASE_RESULT_TYPE_TIME_EXPIRED;
			break;
		case CA_EVENT_PURCHASE_CA_SHUTDOWN:
		case CA_EVENT_PURCHASE_ERROR:
		case CA_EVENT_PURCHASE_NO_VALID_SMARTCARD:
		default:
			ulPurchaseResult = eNA_PPV_PURCHASE_RESULT_TYPE_INVALID_CARD;
			break;
	}

	//AP로 결과 전달...
	if (s_pfnPurchasePackageNotifyCallback)
		s_pfnPurchasePackageNotifyCallback(ulPurchaseResult);

	return ERR_OK;
}

HERROR CA_NA_PpvRequestProductPurchasable(TCaRequest* pRequest, TCaProduct*  pProduct)
{
	// Package list중 한 package를 선택 하여 그 package의 purchasable정보를 보여 주는 경우, ap와 synch이다.
	//product가 purchasable한지  질문 from AP
	TCaObjectStatus 	objStatus;
	TProductFlags 		flag;

	HxLOG_Print("ENTER!! \n");

	// 1. request 가 현재 가지고 있는 product list와 같은 지 확인
	if ((!pRequest) || (!pProduct) || (s_stPackageList.pRequest != pRequest) )
	{
		HxLOG_Print("request is NULL, send UIT_APP_PRODUCT_PRUCHASABLE_FAILURE!!\n");
		HxLOG_Print("s_stPackageList.pRequest = 0x%x, pRequest = 0x%x\n", s_stPackageList.pRequest, pRequest);
		return ERR_FAIL;
	}

	objStatus = caProductGetFlags(pProduct, &flag);
	if (objStatus == CA_OBJECT_NO_ERROR)
	{
		//이제 결과 전달.......
		HxLOG_Print("EXIT!! result OKKK [flag:%d] \n", flag);
		return ERR_OK;
	}

	HxLOG_Print("request is NULL, send UIT_APP_PRODUCT_PRUCHASABLE_FAILURE!! objStatus(%d)\n", objStatus);
	return ERR_FAIL; //FAIL 전달...
}

HERROR CA_NA_PpvGetProductGetDescription(const TCaProduct* pxProduct, TSize* pStrLen, TUnsignedInt8** ppBuffer)
{
	TCaObjectStatus reqStatus;

	// IPPV : use EIT for descriptions and disclaimers
	// PPT  : use cPDT for descriptions and disclaimers
	reqStatus = caProductGetDescription(pxProduct, pStrLen, ppBuffer);
	HxLOG_Print("pxProduct = 0x%x, reqStatus= %d, pStrLen = %d\n", pxProduct, reqStatus, *pStrLen);

	if (reqStatus != CA_OBJECT_NO_ERROR)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR CA_NA_PpvGetProductGetDisclaimer(const TCaProduct* pxProduct, TSize* pStrLen, TUnsignedInt8** ppBuffer)
{
	TCaObjectStatus reqStatus;

	// IPPV : use EIT for descriptions and disclaimers
	// PPT  : use cPDT for descriptions and disclaimers
	reqStatus = caProductGetDisclaimer(pxProduct, pStrLen, ppBuffer);
	HxLOG_Print("pxProduct = 0x%x, reqStatus= %d, pStrLen = %d\n", pxProduct, reqStatus, *pStrLen);

	if (reqStatus != CA_OBJECT_NO_ERROR)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR CA_NA_PpvGetPackageEventInfo(TCaRequest* pRequest, TCaProduct* pPackage, PACKAGE_INFO **pPackageInfo)
{
	//menu에서 package를 하나 선택 하여 관련 event list를 보여 주고자 한다 , synch mode.
	//새로 request 가 올때  마다 curPackageInfo의 정보를 갱신 하여 사용.
	TCaRequestStatus	reqStatus;
	TUnsignedInt8*		ppxDescription;
	TUnsignedInt8*		ppxDisclaimer;
	TUnsignedInt16		xNBEvents;
	TCaEvent**			ppEventArray;
	TUnsignedInt16 		index;

	HxLOG_Print("ENTER!\n");

	if ( (!s_stPackageList.pRequest) || (!pPackage) || (!pRequest))
	{
		if (s_stPackageList.pRequest == NULL)
		{
			HxLOG_Print("EXIT - s_stPackageList.pRequest NULL\n");
		}

		if (pPackage == NULL)
		{
			HxLOG_Print("EXIT - pPackage NULL\n");
		}

		if (pRequest == NULL)
		{
			HxLOG_Print("EXIT - pRequest NULL\n");
		}

		return ERR_FAIL;
	}

	// 1. request 가 현재 가지고 있는 product list와 같은 지 확인
	if (s_stPackageList.pRequest != pRequest)
	{
		HxLOG_Print("EXIT - package_list.pRequest(0x%x), pRequest(0x%x)\n", s_stPackageList.pRequest, pRequest);
		return ERR_FAIL;
	}

	// 2. uit에서 hold 하고 있는 curPackageInfo가 변경 된다. 우선 초기화 시키고 해라...
	if (pRequest == s_stCurPackageInfo.pRequest)
	{
		local_caPpv_InitPackageInfo(&s_stCurPackageInfo);
		HxLOG_Print("local_caPpv_InitPackageInfo OKKK!!\n");
	}
	else
	{
		HxLOG_Print("NO NEED TO Call local_caPpv_InitPackageInfo() !!\n");
	}

	s_stCurPackageInfo.pRequest = s_stPackageList.pRequest;
	s_stCurPackageInfo.pProduct = pPackage;
	s_stCurPackageInfo.pServiceFilter = NULL; //항상 NULL이라 setting 안해도 되긴 하는데..에이 그냥 하자..

	// 3. Package의 description을 가져 오자, CAK에서 nul-terminated string을 넘겨 준다.
	HxLOG_Print("START get description(get caProductGetDescription array from CAK)!!\n");
	reqStatus = caProductGetDescription(s_stCurPackageInfo.pProduct, &(s_stCurPackageInfo.nDesSize), &ppxDescription);
	if ((reqStatus == CA_REQUEST_NO_ERROR) && (s_stCurPackageInfo.nDesSize >0))
	{
		if (s_stCurPackageInfo.nDesSize <= MAX_MSET_PPV_DESCRIPTION_LENGTH)
		{
			MWC_UTIL_DvbStrncpy(s_stCurPackageInfo.aucDescription, ppxDescription, s_stCurPackageInfo.nDesSize);
		}
		else
		{
			s_stCurPackageInfo.nDesSize = MAX_MSET_PPV_DESCRIPTION_LENGTH;
			MWC_UTIL_DvbStrncpy(s_stCurPackageInfo.aucDescription, ppxDescription, MAX_MSET_PPV_DESCRIPTION_LENGTH);
		}
	}
	else
	{
		//CAK에서 disclaimer를 가져 올 때, error가 났다. size를 0으로 하고, disclaimer를 초기화 시켜  주자..
		HxLOG_Print("caProductGetDescription error reqStatus = %d", reqStatus);
		s_stCurPackageInfo.nDesSize = 0;
		HxSTD_memset(s_stCurPackageInfo.aucDescription, 0, MAX_MSET_PPV_DESCRIPTION_LENGTH);
	}

	// 4. Package의 disclaimer를 가져 오자, CAK에서 nul-terminated string을 넘겨 준다.
	HxLOG_Print("START get description(get caProductGetDisclaimer array from CAK)!!\n");
	reqStatus = caProductGetDisclaimer(s_stCurPackageInfo.pProduct, &(s_stCurPackageInfo.nDisclaimerSize), &ppxDisclaimer);
	if ((reqStatus == CA_REQUEST_NO_ERROR) && (s_stCurPackageInfo.nDisclaimerSize > 0))
	{
		if (s_stCurPackageInfo.nDisclaimerSize <= MAX_MSET_PPV_DISCLAIMER_LENGTH)
		{
			MWC_UTIL_DvbStrncpy(s_stCurPackageInfo.aucDisclaimer, ppxDisclaimer, s_stCurPackageInfo.nDisclaimerSize);
		}
		else
		{
			s_stCurPackageInfo.nDisclaimerSize = MAX_MSET_PPV_DISCLAIMER_LENGTH;
			MWC_UTIL_DvbStrncpy(s_stCurPackageInfo.aucDisclaimer, ppxDisclaimer, MAX_MSET_PPV_DISCLAIMER_LENGTH);
		}
	}
	else
	{
		//CAK에서 disclaimer를 가져 올 때, error가 났다. size를 0으로 하고, disclaimer를 초기화 시켜  주자..
		HxLOG_Print("caProductGetDisclaimer error reqStatus = %d", reqStatus);
		s_stCurPackageInfo.nDisclaimerSize = 0;
		HxSTD_memset(s_stCurPackageInfo.aucDisclaimer, 0, MAX_MSET_PPV_DISCLAIMER_LENGTH);
	}

	HxLOG_Print("START get EVENT(get caEvent array from CAK)!!\n");


	// 5. event list를 가져 오자.
	//실제 요구된 package에 있는 event들을 가져 온다...
	reqStatus = caProductGetEvents(pPackage, NULL/* no filter */, &xNBEvents, &ppEventArray);

	HxLOG_Print("reqStatus = %d, xNBEvents = 0x%x\n", reqStatus, xNBEvents);

	if ((reqStatus == CA_REQUEST_NO_ERROR) && (xNBEvents > 0))
	{
		s_stCurPackageInfo.ulNbEvents = xNBEvents;
		s_stCurPackageInfo.pEvents = (EVENT_ITEM*)OxCAS_Malloc(xNBEvents * sizeof(EVENT_ITEM));

		HxSTD_memset(s_stCurPackageInfo.pEvents, 0, xNBEvents * sizeof(EVENT_ITEM)); //한번 clear 시켜 줌.

		//이제 event의 갯수를 알았으니, 각 event 구조체 만들기
		for (index = 0; index < xNBEvents; index++)
		{
			TUnsignedInt8*	pName = NULL;
			TSize			pNameLength = 0;

			reqStatus = caEventGetName(ppEventArray[index], &pNameLength , &pName);

			if (pName)
			{
				HxLOG_Print("[%d] get caEventGetName length(%d) status(%d)..!!!!!\n", index, pNameLength, reqStatus);
			}

			if ((reqStatus == CA_REQUEST_NO_ERROR) && (pNameLength > 0))
			{
				if (pNameLength <= MAX_EVENT_ITEM_NAME_LENGTH)
				{
					s_stCurPackageInfo.pEvents[index].EvtNameLength = pNameLength;
					MWC_UTIL_DvbStrncpy(s_stCurPackageInfo.pEvents[index].EvtName, pName, pNameLength);
				}
				else
				{
					s_stCurPackageInfo.pEvents[index].EvtNameLength = MAX_EVENT_ITEM_NAME_LENGTH;
					MWC_UTIL_DvbStrncpy(s_stCurPackageInfo.pEvents[index].EvtName, pName, MAX_EVENT_ITEM_NAME_LENGTH);
				}

				HxLOG_Print("[%d] sRequestPackageInfo(), event name memcpy ok, pPackageInfo.pEvents[index].EvtName(%s)..!!!!!\n", index, s_stCurPackageInfo.pEvents[index].EvtName);
			}
			else
			{
				HxLOG_Print("[%d] sRequestPackageInfo(), get caEventGetName error. fail to get event neme.(%d)..!!!!!\n", index, reqStatus);
			}

			reqStatus = caEventGetStartTime(ppEventArray[index], &(s_stCurPackageInfo.pEvents[index].pStartTime));

			HxLOG_Print("[%d] sRequestPackageInfo()\n", index);
		}

		HxLOG_Print("---------------- END get EVENT ---------------\n");
	}
	else
	{       //event가 0일 경우 CA_REQUEST_NOT_AVAILABLE 를 준다. from CAK3.0.3
		HxLOG_Print("caProductGetEvents num = %d, reqStatus = %d\n", s_stCurPackageInfo.ulNbEvents, reqStatus);
		reqStatus = CA_REQUEST_NO_ERROR;
		s_stCurPackageInfo.ulNbEvents = 0;
		s_stCurPackageInfo.pEvents = NULL;
	}

	// s_stCurPackageInfo의 포인터만 전달해 준다.
	*pPackageInfo = &s_stCurPackageInfo;

	HxLOG_Print("EXIT!\n");

	return ERR_OK;
}


HERROR CA_NA_PpvGetPurchaseHistoryNum(TUnsignedInt32* pNbItems)
{
	// current purchase history 갯수만 달랑 넘겨 주는 함수.  synch mode
	TCaRequest* pRequest;
	TCaRequestStatus reqStatus;
	TCaObjectStatus objStatus;
	TCaProcessingStatus procStatus;
	TUnsignedInt32 index;
	TUnsignedInt32 purchaseEventNb = 0;
	TCaPurchase **ppPurchase;
	TEventStatus result = CA_EVENT_SUCCESS;
	TCaProcessingStatus status;

	HxLOG_Print("ENTER!\n");

	if (pNbItems == NULL)
	{
		HxLOG_Print("(pNbItems == NULL)<------------EXIT \n");
		return ERR_FAIL;
	}

	*pNbItems = 0;

	reqStatus = CA_NA_PpvCreateRequest(CA_REQUEST_TYPE_PURCHASE_HISTORY, &pRequest);
	if(reqStatus == CA_REQUEST_NO_ERROR)	//  1
	{
		reqStatus = caRequestSend(pRequest);
		if(reqStatus == CA_REQUEST_NO_ERROR)	// 2
		{
			HxLOG_Print("caRequestSend OKKK!!(%d)\n", reqStatus);

			reqStatus = caRequestGetProcessingStatus(pRequest, &procStatus);
			if ((reqStatus != CA_REQUEST_NO_ERROR) || (procStatus != CA_PROCESSING_NO_ERROR))
			{
				HxLOG_Print("Processing Status is not correct !!!(0x%x), dispose request!! \n", reqStatus);
				CA_NA_PpvDisposeRequest(pRequest);
				return ERR_FAIL;
			}

			HxLOG_Print("caRequestGetProcessingStatus OKKK!!(%d)\n", reqStatus);

			reqStatus = caRequestGetObjects(pRequest, &purchaseEventNb, &ppPurchase);
			if  (reqStatus == CA_REQUEST_NO_ERROR)
			{
				*pNbItems = purchaseEventNb;
				HxLOG_Print("caRequestGetObjects OKK!! purchase #(%d)\n", purchaseEventNb);
				CA_NA_PpvDisposeRequest(pRequest);
				return ERR_OK;
			}
			else
			{
				HxLOG_Print("caRequestGetObjects ERR!!(%d), set purchase # to 0\n", reqStatus);
			}
		}
		else
		{
			HxLOG_Print("caRequestSend ERROR!!, set purchase # to 0(%d)\n", reqStatus);
		}
	}
	else
	{
		HxLOG_Print("CA_NA_PpvCreateRequest ERROR!!, set purchase # to 0(%d)\n", reqStatus);
	}

	CA_NA_PpvDisposeRequest(pRequest);
	return ERR_FAIL;
}

HERROR CA_NA_PpvGetProviderId(TPpid *pProviderId)
{
	//synch mode; 0x8D11 for mediaset, test stream에서는 0x0101로 해야 한다.
    TCaRequestStatus reqStatus;
	TCaProcessingStatus proStatus;
	TCaRequest* pRequest;

	TCaSmartcard **ppSmartCard;
	TUnsignedInt32 Nb;
	TCaObjectStatus objStatus;
	TPpid pxProviderId = 0;

	HxLOG_Print("ENTER\n");

	if (pProviderId == NULL)
	{
		HxLOG_Print("pProviderId == NULL)<------------EXIT \n");
		return ERR_FAIL;
	}

	local_caPpv_WaitForGivenTimeWithGivenInterval(MAX_PDT_LOADED_WAIT_TIME_INTERVAL, MAX_PDT_LOADED_WAIT_TIME_SC);

	CAPPV_SMARTCARD_SEM_GET();
	reqStatus = CA_NA_PpvCreateRequest(CA_REQUEST_TYPE_SMARTCARDS, &pRequest);
	HxLOG_Print("CA_NA_PpvCreateRequest reqStatus = 0x%x\n", reqStatus);

	if(reqStatus == CA_REQUEST_NO_ERROR)	//  1
	{
		reqStatus = caRequestSend(pRequest);
		if(reqStatus == CA_REQUEST_NO_ERROR)	// 2
		{
			reqStatus = caRequestGetProcessingStatus(pRequest, &proStatus);
			HxLOG_Print("caRequestGetProcessingStatus reqStatus = %d, proStatus = %d\n", reqStatus, proStatus);
			if ((reqStatus != CA_REQUEST_NO_ERROR) || (proStatus != CA_PROCESSING_NO_ERROR))
			{
				CA_NA_PpvDisposeRequest(pRequest);

				CAPPV_SMARTCARD_SEM_RELEASE();
				return ERR_FAIL;
			}

			reqStatus = caRequestGetObjects(pRequest, &Nb, &ppSmartCard); // only one SMART CARD currently..
			if ((reqStatus == CA_REQUEST_NO_ERROR) && (Nb > 0) && (ppSmartCard != NULL))
			{
				objStatus = caSmartcardGetProviderId(ppSmartCard[0], &pxProviderId);
				if   (objStatus == CA_OBJECT_NO_ERROR)
				{
					*pProviderId = pxProviderId;
					HxLOG_Print("Get pxProviderId(0x%x)  OKKK!!\n", pxProviderId);
					MW_CA_NA_DisposeRequest(pRequest);

					CAPPV_SMARTCARD_SEM_RELEASE();
					return ERR_OK;
				}
				else
				{
					HxLOG_Print("caSmartcardGetProviderId ERR!!(%d)\n", objStatus);
				}
			}
			else
			{
				HxLOG_Print("caRequestGetObjects ERR!!(%d), Nb(%d)\n", reqStatus, Nb);
			}
		}
		else
		{
			HxLOG_Print("caRequestSend ERROR!!(%d)\n", reqStatus);
		}
	}
	else
	{
		HxLOG_Print("CA_NA_PpvCreateRequest ERROR!!(%d)\n", reqStatus);
	}

	CA_NA_PpvDisposeRequest(pRequest);

	CAPPV_SMARTCARD_SEM_RELEASE();
	HxLOG_Print("<....... dispose request ERR 2\n");

	return ERR_FAIL;
}

HERROR CA_NA_PpvRequestFullInfoPackageList(TPpid providerID)
{
	// Full Info Package list를 요청.  Synch mode로 모든 Package List의 정보를 전달.
	NaCasMsg_t		stMsg;
	TSignedInt32	nErrCode;

	HxLOG_Print("ENTER\n");

	stMsg.wMsg = NA_CAK_ID;
	stMsg.lParam1 = (TUnsignedInt32) providerID;
	stMsg.lParam2 = CAK_PPV_REQUEST_FULL_INFO_PACKAGE_LIST;
	stMsg.lParam3 = 0;

	nErrCode = VK_PostMessage(s_ulPpvQueueId, (void *)&stMsg, QUEUE_SIZE_1);
	if (nErrCode)
	{
		HxLOG_Print("Q Send Fail in MW_CA_NA_PPV_RequestFullInfoPackageList 0x%x\n", nErrCode);
		return ERR_FAIL;
	}

	return ERR_OK;
}


#ifdef CAK_CREATE_REQUEST_DEBUG
void MW_CA_NA_PRINT_CreatedCaRequestList(char* file, int line)
{
	HxLOG_Print("\n\n[MW_CA_NA_PRINT_CreatedCaRequestList] --> ENTER(%d)(%s)!!\n", line, file);
	INT_PRINT_CreatedCaRequestList();
	HxLOG_Print("[MW_CA_NA_PRINT_CreatedCaRequestList] --> ENTER!!\n\n\n");
}

static void INT_PRINT_CreatedCaRequestList()
{
	REQUEST_ITEM *holder;
	int	searchedIndex = 0;

	HxLOG_Print("[INT_PRINT_CreatedCaRequestList] ---------------> ENTER!!\n");

	if ((gCaRequestStatus.pNext == NULL) || (gCaRequestStatus.index<= 0))
	{
		HxLOG_Print("[INT_PRINT_CreatedCaRequestList] gCaRequestStatus.index = 0, END!!\n");
	}
	else
	{

		UIT_REQUEST_SEM_GET();
		holder = gCaRequestStatus.pNext;

		while (holder)
		{
			HxLOG_Print("\t[%d] pRequst = 0x%x, line = %d, type = ", searchedIndex,  holder->request, holder->line);

			switch(holder->type)
			{
				  case CA_REQUEST_TYPE_SYSTEM :
						HxLOG_Print("%s \n", "CA_REQUEST_TYPE_SYSTEM");
				  case CA_REQUEST_TYPE_SMARTCARDS :
						HxLOG_Print("%s \n", "CA_REQUEST_TYPE_SMARTCARDS");
				  case CA_REQUEST_TYPE_PRODUCTS :
						HxLOG_Print("%s \n", "CA_REQUEST_TYPE_PRODUCTS");
				  case CA_REQUEST_TYPE_PRODUCT_PURCHASE :
						HxLOG_Print("%s \n", "CA_REQUEST_TYPE_PRODUCT_PURCHASE");
				  case CA_REQUEST_TYPE_PURCHASE_HISTORY :
						HxLOG_Print("%s \n", "CA_REQUEST_TYPE_PURCHASE_HISTORY");
				  case CA_REQUEST_TYPE_RECHARGE_HISTORY :
						HxLOG_Print("%s \n", "CA_REQUEST_TYPE_RECHARGE_HISTORY");
				  case CA_REQUEST_TYPE_PDT_START :
						HxLOG_Print("%s \n", "CA_REQUEST_TYPE_PDT_START");
				  case CA_REQUEST_TYPE_PDT_STOP :
						HxLOG_Print("%s \n", "CA_REQUEST_TYPE_PDT_STOP");
				  case CA_REQUEST_TYPE_HASH_SIGN :
						HxLOG_Print("%s \n", "CA_REQUEST_TYPE_HASH_SIGN");
				  case CA_REQUEST_TYPE_EVENT :
						HxLOG_Print("%s \n", "CA_REQUEST_TYPE_EVENT");
				  case CA_REQUEST_TYPE_CREDITS :
						HxLOG_Print("%s \n", "CA_REQUEST_TYPE_CREDITS");
				  case CA_REQUEST_TYPE_PROGRAMS :
						HxLOG_Print("%s \n", "CA_REQUEST_TYPE_PROGRAMS");
				  case CA_REQUEST_TYPE_UNIQUE_KEY :
						HxLOG_Print("%s \n", "CA_REQUEST_TYPE_UNIQUE_KEY");
				  case CA_REQUEST_TYPE_RANDOM_KEY :
						HxLOG_Print("%s \n", "CA_REQUEST_TYPE_RANDOM_KEY");
				  case CA_REQUEST_TYPE_WATCH_PPT :
						HxLOG_Print("%s \n", "CA_REQUEST_TYPE_WATCH_PPT");
				  case CA_REQUEST_TYPE_VOD_PLAYOUT :
						HxLOG_Print("%s \n", "CA_REQUEST_TYPE_VOD_PLAYOUT");
				  case CA_REQUEST_NUM_TYPES :
						HxLOG_Print("%s \n", "CA_REQUEST_NUM_TYPES");
				  default :
						HxLOG_Print("%s \n", "ERR!! OUT of CA REQUEST TYPE BOUND(%d) !!", holder->type);
			}

			holder = holder->pNext;
			searchedIndex++;
		}

		UIT_REQUEST_SEM_RELEASE();

	}

	HxLOG_Print("[INT_PRINT_CreatedCaRequestList] <------------------- EXIT!\n");
}
#endif


#endif //end of #if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)
/*** End of File ***/
